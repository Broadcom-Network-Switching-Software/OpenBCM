/*! \file bcmlt_ltable_replay.c
 *
 * This file implements the logical table replay feature.
 * The format of keeping an element (entry/transaction) is as follows:
 * Entry format:
 * 0                 4     5     6
 * ------------------------------------------------------------
 * | time stamp (us) | ENT | Len | LT name | entry_req_info_t
 * ------------------------------------------------------------
 *
 * Transaction format:
 * 0                 4     5                [ entries section                ]+
 * ---------------------------------------------------------------------------
 * | time stamp (us) | TRN | bcmtrm_trans_t | Len | LT name | entry_req_info_t
 * ---------------------------------------------------------------------------
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_sleep.h>
#include <sal/sal_time.h>
#include <shr/shr_error.h>
#include <shr/shr_fmm.h>
#include <bcmdrd_config.h>
#include <bcmlt/bcmlt.h>
#include <bcmtrm/trm_api.h>
#include "bcmlt_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLT_REPLAY

#define ENTRY_TYPE    1
#define TRANS_TYPE    2

typedef struct {
    bool hi_prio;
    bool has_fields;
    uint8_t notif_opt;
    int unit;
    uint32_t attrib;
    bcmltm_combined_opcode_t opcode;
} entry_req_info_t;

typedef struct {
    uint16_t flags;
    bool has_next;
    uint32_t idx;
    uint32_t id;
    uint64_t data;
} field_req_info_t;

typedef struct {
    bcmlt_capture_cb_t app_cb;
    sal_mutex_t sync_obj;
    uint32_t filters;
} unit_info_t;

static unit_info_t replay_fh[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Determine if a certain entry should be filtered.
 *
 * \param [in] entry The entry to check.
 *
 * \return true if the entry should be filtered and false otherwise.
 */
static bool is_filtered(bcmtrm_entry_t *entry)
{
    int unit = entry->info.unit;
    bcmlt_opcode_t opcode = entry->opcode.lt_opcode;

    return (!entry->pt && (replay_fh[unit].filters & (1 << opcode)));
}

/*!
 *\brief Saves an entry using application supplied function.
 *
 * This function saves an entry using an application callback into the IO
 * provided via the parameter \c fd. The entry saved as the basic entry
 * structure followed by saving every field of the entry.
 *
 * \param [in] fd is the IO handle to be used by the application callback.
 * \param [in] entry is the entry to save.
 * \param [in] app_cb is the application supplied callback structure that
 * contain read and write functions that actually perform the saving of
 * the data into non-volatile memory that can be replay later.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int record_entry(void *fd,
                        bcmtrm_entry_t *entry,
                        bcmlt_capture_cb_t *app_cb)
{
    shr_fmm_t *field;
    int rv;
    uint8_t tbl_name_len = sal_strlen(entry->info.table_name) + 1;
    entry_req_info_t s_entry;
    field_req_info_t s_field;

    if ((app_cb->write(fd, &tbl_name_len, 1) != 1)  ||
        (app_cb->write(fd, (void *)entry->info.table_name, tbl_name_len) !=
             tbl_name_len)) {
        return SHR_E_INTERNAL;
    }
    s_entry.attrib = entry->attrib;
    s_entry.has_fields = entry->l_field != NULL;
    s_entry.hi_prio = entry->priority == BCMLT_PRIORITY_HIGH;
    s_entry.notif_opt = (uint8_t)entry->info.notif_opt;
    s_entry.opcode = entry->opcode;
    s_entry.unit = entry->info.unit;
    if (app_cb->write(fd, &s_entry, sizeof(s_entry)) != sizeof(s_entry)) {
        return SHR_E_INTERNAL;
    }

    /* save all the fields */
    for (field = entry->l_field; field; field = field->next) {
        s_field.id = field->id;
        s_field.idx = field->idx;
        s_field.flags = (uint16_t)field->flags;
        s_field.data = field->data;
        s_field.has_next = field->next != NULL;
        rv = app_cb->write(fd, &s_field, sizeof(s_field));
        if (rv != sizeof(s_field)) {
            return SHR_E_INTERNAL;
        }
    }
    return SHR_E_NONE;
}

/*!
 *\brief Delays the execution to replay at the same timing as original.
 *
 * This function sleeps the duration of the difference between the two input
 * parameters. However, the function limited the sleep time to 1 second. At
 * the end the function sets the input parameter \c current_ts to the time
 * slot representing the operation recording time.
 *
 * \param [in] ts is the timeslot indicating the time that the operation
 * was recorded.
 * \param [in,out] current_ts is the time slot of the previous operation
 * (or 0 if this is the first operation).
 *
 * \return None.
 */
static void wait_till_ready(sal_usecs_t ts,
                            sal_usecs_t *current_ts)
{
    sal_usecs_t diff;

    if (*current_ts == 0) {
        *current_ts = ts;
        return;
    }
    diff = ts - *current_ts;
    if (diff > 1000000) {   /* sleep maximum 1 second */
        diff = 1000000;
    }
    sal_usleep(diff);
    *current_ts = ts;
}

/*!
 *\brief Create new entry from the replay log.
 *
 * This function reads the replay log and construct a new entry. The entry is
 * fully built including the entry fields.
 *
 * \param [in] app_cb is the application provided IO structure to perform
 * read and write operations from/to the replay log.
 * \param [in] mtx Synchronizing object to protect entry/handle alloc/free.
 *
 * \return A valid pointer to an entry on success.
 * \return NULL Failure.
 */
static bcmtrm_entry_t *create_entry(bcmlt_replay_cb_t *app_cb, sal_mutex_t mtx)
{
    bcmtrm_entry_t *entry;
    bcmtrm_entry_t *tmp_entry;
    shr_fmm_t *field;
    bool more_field;
    void *fd = app_cb->fd;
    uint8_t tbl_name_len;
    char *tbl_name;
    bcmlt_table_attrib_t *table_attr;
    void *hdl;
    int rv;
    entry_req_info_t r_entry;
    field_req_info_t r_field;

    if (app_cb->read(fd, &tbl_name_len, 1) != 1) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Failed to read table name length\n")));
        return NULL;
    }
    tbl_name = sal_alloc(tbl_name_len,  "bcmltReplay");
    if (!tbl_name) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Failed to allocate table name\n")));
        return NULL;
    }
    if (app_cb->read(fd, tbl_name, tbl_name_len) != tbl_name_len) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Failed to read table name (%d bytes)\n"),
                   (int)tbl_name_len));
        sal_free(tbl_name);
        return NULL;
    }

    if (app_cb->read(fd, &r_entry, sizeof(r_entry)) != sizeof(r_entry)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Failed to read table information for %s\n"),
                   tbl_name));
        sal_free(tbl_name);
        return NULL;
    }

    /* Table ID may change so retrieve it again */
    rv = bcmlt_db_table_info_get(r_entry.unit,
                                 tbl_name,
                                 &table_attr,
                                 NULL,
                                 &hdl);
    if (rv != SHR_E_NONE) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Failed from bcmlt_db_table_info_get() for "
                            "%s (%d): %s\n"),
                   tbl_name, rv, shr_errmsg(rv)));
        sal_free(tbl_name);
        return NULL;
    }
    sal_free(tbl_name);   /* Done with local storage for table name */

    sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
    entry = bcmtrm_entry_alloc(r_entry.unit,
                               table_attr->table_id,
                               table_attr->interactive,
                               table_attr->pt,
                               NULL,
                               table_attr->name);
    if (!entry) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Failed from bcmtrm_entry_alloc() for %s\n"),
                   table_attr->name));
        sal_mutex_give(mtx);
        return NULL;
    }
    rv = bcmlt_hdl_alloc(entry, &entry->info.entry_hdl);
    if (rv != SHR_E_NONE) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Failed from bcmlt_hdl_alloc() for %s\n"),
                   table_attr->name));
        bcmtrm_entry_free(entry);
        sal_mutex_give(mtx);
        return NULL;
    }
    sal_mutex_give(mtx);
    tmp_entry = entry;


    entry->db_hdl = hdl;
    entry->attrib = r_entry.attrib;
    entry->info.unit = r_entry.unit;
    entry->info.notif_opt = r_entry.notif_opt;
    entry->opcode = r_entry.opcode;
    if (r_entry.hi_prio) {
        entry->priority = BCMLT_PRIORITY_HIGH;
    } else {
        entry->priority = BCMLT_PRIORITY_NORMAL;
    }
    entry->max_fid = 0;
    entry->fld_arr = NULL;

    /* Now start recovering the fields */
    more_field = r_entry.has_fields;
    while (more_field) {
        field = shr_fmm_alloc();
        if (!field) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META("Failed from shr_fmm_alloc() for %s\n"),
                       table_attr->name));
            tmp_entry = NULL;
            break;
        }
        if (app_cb->read(fd, &r_field, sizeof(r_field)) != sizeof(r_field)) {
            tmp_entry = NULL;
            break;
        }
        field->data = r_field.data;
        field->id = r_field.id;
        field->idx = r_field.idx;
        field->flags = r_field.flags;
        more_field = r_field.has_next;
        field->next = entry->l_field;
        entry->l_field = field;
    }

    if (!tmp_entry) {  /* Need clean up? */
        while (entry->l_field) {
            field = entry->l_field;
            entry->l_field = field->next;
            shr_fmm_free(field);
        }
        (void)sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
        (void)bcmlt_hdl_free(entry->info.entry_hdl);
        (void)bcmtrm_entry_free(entry);
        (void)sal_mutex_give(mtx);
        entry = NULL;
    }
    return entry;
}

/*!
 *\brief Replay an entry operation.
 *
 * This function replay one entry operation. It first ceates the entry
 * by reading its content from the IO and then posting it to the transaction
 * manager for processing.
 *
 * \param [in] app_cb is the application provided IO structure to perform
 * read and write operations from/to the replay log.
 * \param [in] cb Application callback can be use to display the content of
 * the entry.
 * \param [in] view_only Indicates if the entry should be replayed into the
 * system or only used for display purposes.
 * \param [in] mtx Synchronizing object to protect the entry/handle alloc/free.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int replay_entry(bcmlt_replay_cb_t *app_cb,
                        replay_action_f *cb,
                        bool view_only,
                        sal_mutex_t mtx,
                        sal_usecs_t ts)
{
    bcmtrm_entry_t *entry;
    int rv = SHR_E_NONE;
    bcmlt_object_hdl_t obj;

    entry = create_entry(app_cb, mtx);
    if (!entry) {
        return SHR_E_INTERNAL;
    }
    entry->asynch = false;

    /* See if the entry should not be filtered out */
    if (!is_filtered(entry)) {
        if (!view_only) {
            rv = bcmtrm_entry_req(entry);
        }
        if (cb) {
            obj.entry = true;
            obj.hdl.entry = entry->info.entry_hdl;
            obj.ts = ts;
            cb(entry->info.unit, &obj);
        }
    }

    (void)sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
    (void)bcmlt_hdl_free(entry->info.entry_hdl);
    (void)bcmtrm_entry_free(entry);
    (void)sal_mutex_give(mtx);
    return rv;
}

/*!
 *\brief Adds an entry to a transaction.
 *
 * This function creates a new entry and adds it to a given transaction.
 *
 * \param [in] trans is the transaction to add the entry to.
 * \param [in] app_cb is the application provided IO structure to perform
 * \param [in] mtx synchronization object required for entry/transaction
 * allocation and free.
 * read and write operations from/to the replay log.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int add_entry(bcmtrm_trans_t *trans,
                     bcmlt_replay_cb_t *app_cb,
                     sal_mutex_t mtx)
{
    bcmtrm_entry_t *entry;

    entry = create_entry(app_cb, mtx);
    if (!entry) {
        return SHR_E_MEMORY;
    }

    /* See if the entry should be filtered out */
    if (is_filtered(entry)) {
        (void)sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
        (void)bcmlt_hdl_free(entry->info.entry_hdl);
        (void)bcmtrm_entry_free(entry);
        (void)sal_mutex_give(mtx);
        return SHR_E_NONE;
    }

    /* Entry should be kept in the same order so add to the end */
    entry->next = NULL;
    if (!trans->l_entries) {
        trans->l_entries = entry;
    } else {
        trans->last_entry->next = entry;
    }
    trans->last_entry = entry;
    entry->p_trans = trans;
    return SHR_E_NONE;
}

/*!
 *\brief Replay transaction.
 *
 * This function creates a new transaction and all its associated
 * entries. It than posts the transaction to the transaction manager.
 *
 * \param [in] app_cb is the application provided IO structure to perform
 * read and write operations from/to the replay log.
 * \param [in] cb Application callback can be use to display the content of
 * the transaction.
 * \param [in] view_only Indicates if the transaction should be replayed into
 * the system or only used for display purposes.
 * \param [in] mtx Synchronizing object to protect the transaction/entry/handle
 * alloc/free operations.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int replay_trans(bcmlt_replay_cb_t *app_cb,
                        replay_action_f *cb,
                        bool view_only,
                        sal_mutex_t mtx,
                        sal_usecs_t ts)
{
    bcmtrm_trans_t *trans;
    bcmtrm_trans_t recovered_trans;
    uint32_t j;
    int rv;
    void *fd = app_cb->fd;
    bcmlt_object_hdl_t obj;
    bcmtrm_entry_t *entry;

    if (app_cb->read(fd, &recovered_trans, sizeof(*trans)) != sizeof(*trans)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Failed to read transaction information\n")));
        return SHR_E_INTERNAL;
    }
    (void)sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
    trans = bcmtrm_trans_alloc(recovered_trans.info.type);
    if (!trans) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Failed to allocate transaction information\n")));
        (void)sal_mutex_give(mtx);
        return SHR_E_MEMORY;
    }
    rv = bcmlt_hdl_alloc(trans, &trans->info.trans_hdl);
    if (rv != SHR_E_NONE) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Failed to allocate transaction handle\n")));
        bcmtrm_trans_free(trans);
        (void)sal_mutex_give(mtx);
        return rv;
    }
    (void)sal_mutex_give(mtx);
    trans->info.num_entries = recovered_trans.info.num_entries;
    trans->syncronous = true;
    trans->pt_trans = recovered_trans.pt_trans;
    trans->priority = recovered_trans.priority;
    trans->l_entries = NULL;
    for (j = 0; j < trans->info.num_entries; j++) {
        if ((rv = add_entry(trans, app_cb, mtx)) != SHR_E_NONE) {
            return rv;
        }
    }
    /*
     * Adjust the number of entries in the transaction since some may have
     * been filtered out.
     */
    trans->info.num_entries = 0;
    for (entry = trans->l_entries; entry; entry = entry->next) {
        trans->info.num_entries++;
    }
    if (trans->info.num_entries == 0) {  /* All filtered out? */
        (void)sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
        (void)bcmlt_hdl_free(trans->info.trans_hdl);
        (void)bcmtrm_trans_free(trans);
        (void)sal_mutex_give(mtx);
        return SHR_E_NONE;
    }
    if (!view_only) {
        rv = bcmtrm_trans_req(trans);
    }
    if (cb) {
        obj.entry = false;
        obj.hdl.trans = trans->info.trans_hdl;
        obj.ts = ts;
        cb(trans->unit, &obj);
    }

    /* Delete all the entries before freeing the transaction */
    (void)sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
    while (trans->l_entries) {
        entry = trans->l_entries;
        trans->l_entries = entry->next;
        entry->p_trans = NULL;
        (void)bcmlt_hdl_free(entry->info.entry_hdl);
        (void)bcmtrm_entry_free(entry);
    }
    (void)bcmlt_hdl_free(trans->info.trans_hdl);
    (void)bcmtrm_trans_free(trans);
    (void)sal_mutex_give(mtx);
    return rv;
}


/*******************************************************************************
 * Public functions
 */
int bcmlt_capture_filter_set(int unit,
                             bcmlt_opcode_t *filters,
                             uint32_t num_of_opcodes)
{
    uint32_t j;

    if ((unit < 0) || unit >= BCMDRD_CONFIG_MAX_UNITS) {
        return SHR_E_UNIT;
    }
    /*
     * Sanity to make sure that the number of opcode exceeded the storage
     * capacity.
     */
    if (BCMLT_OPCODE_NUM >= 31) {
        return SHR_E_INTERNAL;
    }
    replay_fh[unit].filters = 0; /* remove the previous filter */
    if (!filters || num_of_opcodes == 0) {
        return SHR_E_NONE;
    }
    for (j = 0; j < num_of_opcodes; j++) {
        if (filters[j] < 0 || filters[j] >= BCMLT_OPCODE_NUM) {
            replay_fh[unit].filters = 0;
            return SHR_E_PARAM;
        }
        replay_fh[unit].filters |= (1 << filters[j]);
    }
    return SHR_E_NONE;
}

int bcmlt_capture_start(int unit, bcmlt_capture_cb_t *app_cb)
{
    if ((unit < 0) || unit >= BCMDRD_CONFIG_MAX_UNITS) {
        return SHR_E_UNIT;
    }
    if (!bcmlt_device_attached(unit)) {
        return SHR_E_INIT;
    }
    if (replay_fh[unit].app_cb.write) {
        return SHR_E_BUSY;
    }
    replay_fh[unit].app_cb = *app_cb;
    replay_fh[unit].sync_obj = sal_mutex_create("bcmltCapture");
    if (!replay_fh[unit].sync_obj) {
        return SHR_E_MEMORY;
    }
    return SHR_E_NONE;
}

int bcmlt_capture_stop(int unit)
{
    if ((unit < 0) || unit >= BCMDRD_CONFIG_MAX_UNITS) {
        return SHR_E_UNIT;
    }
    replay_fh[unit].app_cb.write = 0;
    if (replay_fh[unit].sync_obj) {
        /* Make sure the entry/transaction reocoring is done */
        sal_mutex_take(replay_fh[unit].sync_obj, SAL_MUTEX_FOREVER);
        sal_mutex_give(replay_fh[unit].sync_obj);
        sal_mutex_destroy(replay_fh[unit].sync_obj);
        replay_fh[unit].sync_obj = NULL;
    }
    return SHR_E_NONE;
}

int bcmlt_replay_entry_record(bcmtrm_entry_t *entry)
{
    void *fd;
    sal_usecs_t *ts;
    sal_usecs_t cur_time;
    uint8_t buffer[sizeof(sal_usecs_t) + 1];
    bcmlt_capture_cb_t *app_cb = &replay_fh[entry->info.unit].app_cb;
    sal_mutex_t mtx = replay_fh[entry->info.unit].sync_obj;
    bcmlt_capture_cb_t cb;
    int rv;

    sal_memcpy(&cb, app_cb, sizeof(*app_cb));
    if (!app_cb->write) {
        return SHR_E_NONE;
    }

    /* Filter out unnecessary opcodes */
    if (is_filtered(entry)) {
        return SHR_E_NONE;
    }
    fd = cb.fd;
    ts = (sal_usecs_t *)buffer;
    cur_time = sal_time_usecs();
    *ts = cur_time;
    buffer[sizeof(sal_usecs_t)] = ENTRY_TYPE;
    sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
    /* Check whether the capture is stopped before entry recording */
    if (!app_cb->write) {
        sal_mutex_give(mtx);
        return SHR_E_NONE;
    }
    if (cb.write(fd, buffer, sizeof(buffer)) != sizeof(buffer)) {
        rv = SHR_E_INTERNAL;
    } else {
        rv = record_entry(fd, entry, &cb);
    }
    sal_mutex_give(mtx);
    return rv;
}

int bcmlt_replay_trans_record(bcmtrm_trans_t *trans)
{
    void *fd;
    sal_usecs_t *ts;
    sal_usecs_t cur_time;
    uint8_t buffer[sizeof(sal_usecs_t) + 1];
    bcmtrm_entry_t *entry;
    int rv;
    bcmlt_capture_cb_t *app_cb = &replay_fh[trans->unit].app_cb;
    sal_mutex_t mtx = replay_fh[trans->unit].sync_obj;
    bcmlt_capture_cb_t cb;
    uint32_t unfiltered_entries;
    uint32_t original_entry_count;

    sal_memcpy(&cb, app_cb, sizeof(*app_cb));
    if (!app_cb->write) {
        return SHR_E_NONE;
    }

    /* Determine how many entries will survive the filter */
    unfiltered_entries = 0;
    original_entry_count = trans->info.num_entries;
    for (entry = trans->l_entries; entry; entry = entry->next) {
        if (!is_filtered(entry)) {
            unfiltered_entries++;
        }
    }
    if (unfiltered_entries == 0) {  /* Everything was filtered out */
        return SHR_E_NONE;
    }

    fd = cb.fd;
    ts = (sal_usecs_t *)buffer;
    cur_time = sal_time_usecs();
    *ts = cur_time;
    buffer[sizeof(sal_usecs_t)] = TRANS_TYPE;

    sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
    /* Check whether the capture is stopped before transaction recording */
    if (!app_cb->write) {
        sal_mutex_give(mtx);
        return SHR_E_NONE;
    }
    rv = cb.write(fd, buffer, sizeof(buffer));
    if (rv != sizeof(buffer)) {
        sal_mutex_give(mtx);
        return SHR_E_INTERNAL;
    }
    trans->info.num_entries = unfiltered_entries;
    rv = cb.write(fd, trans, sizeof(*trans));
    if (rv != sizeof(*trans)) {
        sal_mutex_give(mtx);
        return SHR_E_INTERNAL;
    }
    trans->info.num_entries = original_entry_count;

    for (entry = trans->l_entries; entry; entry = entry->next) {
        if (is_filtered(entry)) {
            continue;   /* Filter out the entry */
        }
        rv = record_entry(fd, entry, &cb);
        if (rv != SHR_E_NONE) {
            break;
        }
    }
    sal_mutex_give(mtx);
    return rv;
}

int bcmlt_capture_replay(bool timing,
                         replay_action_f *cb,
                         bool view_only,
                         bcmlt_replay_cb_t *app_cb)
{
    int rv = SHR_E_NONE;
    void *fd;
    sal_usecs_t *ts;
    sal_usecs_t current_ts = 0;
    uint8_t buffer[sizeof(sal_usecs_t) + 1];
    sal_mutex_t mtx = bcmlt_entry_sync_obj_get();

    if (!app_cb || !app_cb->read || !app_cb->fd) {
        return SHR_E_PARAM;
    }
    fd = app_cb->fd;
    ts = (sal_usecs_t *)buffer;

    while (app_cb->read(fd, buffer, sizeof(buffer)) == sizeof(buffer)) {
        if (timing) {
            wait_till_ready(*ts, &current_ts);
        }
        if (buffer[sizeof(sal_usecs_t)] == TRANS_TYPE) {
            rv = replay_trans(app_cb, cb, view_only, mtx, *ts);
        } else if (buffer[sizeof(sal_usecs_t)] == ENTRY_TYPE) {
            rv = replay_entry(app_cb, cb, view_only, mtx, *ts);
        } else {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META("Failed to read time stamp\n")));
            rv = SHR_E_INTERNAL;
        }
        if (rv != SHR_E_NONE) {
            break;
        }
    }

    return rv;
}

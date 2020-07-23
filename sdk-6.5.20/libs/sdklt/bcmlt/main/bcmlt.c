/*! \file bcmlt.c
 *
 *  Logical Table basic API to handle initialization shutdown
 *  device attach and detach.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_sem.h>
#include <shr/shr_error.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_sysm.h>
#include <shr/shr_debug.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmcfg/comp/bcmcfg_trm_resources.h>
#include <bcmlt/bcmlt.h>
#include <bcmtrm/trm_api.h>

#include "bcmlt_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMLT_TABLE

static bool bcmlt_initialized = false;

static bool atomic_enabled = false;

/* Keeps map of all the attached units, i.e. if the unit is attached or not. */
static bool unit_attached_map[BCMDRD_CONFIG_MAX_UNITS];

static bool unit_attach_stat[BCMDRD_CONFIG_MAX_UNITS];

static sal_sem_t unit_attached_sem[BCMDRD_CONFIG_MAX_UNITS];

/*================================================================*
 *     C O M P O N E N T    I N T E R N A L   F U N C T I O N S   *
 *================================================================*/
/*!
 *\brief Callback function to create entry.
 *
 * This function being called by the notification mechanism to create a new
 * entry that can be used for user notification. This function being registered
 * with the TRM so it can be called when required.
 *
 * \param [in] unit The unit of this entry.
 * \param [in] tbl_id The logical table ID.
 * \param [in] opcode The opcode associated with this entry.
 * \param [in] fields The list of fields for this entry.
 * \param [out] entry The newly allocated entry.
 *
 * \return SHR_E_NONE on succes.
 * \return SHR_E_PARAM when one of the input parameters is invalid.
 * \return SHR_E_MEMORY when failed to allocate entry resources.
 */
static int entry_alloc_assist(int unit,
                              uint32_t tbl_id,
                              bcmlt_opcode_t opcode,
                              shr_fmm_t *fields,
                              bcmtrm_entry_t **entry)
{
    const bcmlrd_table_rep_t *lt_info;
    bcmlt_entry_handle_t entry_hdl;
    shr_fmm_t *fld;

    SHR_FUNC_ENTER(unit);

    lt_info = bcmlrd_table_get(tbl_id);
    if (lt_info == NULL) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Invalid table ID %u\n"), tbl_id));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, lt_info->name, &entry_hdl));

    *entry = bcmlt_hdl_data_get(entry_hdl);
    (*entry)->opcode.lt_opcode = opcode;
    (*entry)->l_field = fields;
    fld = fields;
    if ((*entry)->fld_arr) {
        while (fld) {
            /*
             * Make sure that the field ID can get into the array. Also the
             * array is being allocated empty. If there is already an element
             * in the same field ID it means that there is an array. In this
             * case don't overwrite the first element of the array.
             */
            if (fld->id > (*entry)->max_fid) {
                LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Invalid field ID %u\n"),
                                    fld->id));
            } else if ((*entry)->fld_arr[fld->id] == NULL) {
                (*entry)->fld_arr[fld->id] = fld;
            }
            fld = fld->next;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int entry_free_assist(bcmtrm_entry_t *entry)
{
    bcmlt_entry_handle_t entry_hdl = entry->info.entry_hdl;

    SHR_FUNC_ENTER(entry->info.unit);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
exit:
    SHR_FUNC_EXIT();
}

/*================================================================*
 *    Component global functions local to the component           *
 *================================================================*/
bool bcmlt_is_initialized(void)
{
    return bcmlt_initialized;
}

bool bcmlt_is_atomic_ena(void)
{
    return atomic_enabled;
}

int bcmlt_init(void)
{
    bcmcfg_feature_ctl_config_t feature_conf;
    bcmcfg_trm_resources_config_t ent_conf;
    uint32_t use_val;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (bcmlt_initialized) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    bcmlt_initialized = true;
    SHR_IF_ERR_EXIT(bcmlt_db_init());

    rv = bcmcfg_trm_resources_config_get(&ent_conf);
    if (SHR_FAILURE(rv) || ent_conf.max_entries == 0) {
        use_val = BCMTRM_DEFAULT_MAX_ACTIVE_ENTRIES;
    } else {
        use_val = ent_conf.max_entries;
    }
    if (SHR_FAILURE(rv) || ent_conf.max_transactions == 0) {
        use_val += BCMTRM_DEFAULT_MAX_ACTIVE_TRANSACTIONS;
    } else {
        use_val += ent_conf.max_transactions;
    }
    SHR_IF_ERR_EXIT(bcmlt_hdl_init());

    SHR_IF_ERR_EXIT(bcmlt_entry_module_init());

    bcmtrm_register_entry_assist(entry_alloc_assist,
                                 entry_free_assist,
                                 bcmlt_replay_entry_record,
                                 bcmlt_replay_trans_record);
    rv = bcmcfg_feature_ctl_config_get(&feature_conf);
    if (SHR_FAILURE(rv) || feature_conf.enable_atomic_trans == 0) {
        atomic_enabled = false;
    } else {
        atomic_enabled = true;
    }
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_close(void)
{
    int j;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (!bcmlt_initialized) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    /* Verify that all units had been detached */
    for (j = 0; j < BCMDRD_CONFIG_MAX_UNITS; j++) {
        if (unit_attached_map[j]) {
            LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META("Cannot shut down cleanly, "
                                  "Unit %d is still attached\n"),
                                j));
            SHR_ERR_EXIT(SHR_E_BUSY);
        }
    }
    bcmlt_initialized = false;

    SHR_IF_ERR_EXIT(bcmlt_entry_module_shutdown());
    SHR_IF_ERR_EXIT(bcmlt_db_delete());

    bcmlt_hdl_shutdown();
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_allocated_resources(int unit)
{
    bcmtrm_entry_t *e_list;
    bcmtrm_trans_t *t_list;
    sal_mutex_t mtx = NULL;

    SHR_FUNC_ENTER(unit);
    mtx = bcmlt_entry_sync_obj_get();
    /* The entire operation should be mutex protected */
    if (mtx) {
        sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
    }
    bcmtrm_active_element_lists_get(&e_list, &t_list);

    while (t_list) {
       if (t_list->unit == unit) {
            LOG_ERROR(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "Cannot shut down unit %d, "
                                   "one or more transactions still active"),
                            unit));
            SHR_ERR_EXIT(SHR_E_BUSY);
       }
       t_list = t_list->debug_next;
    }

    while (e_list) {
        if (e_list->info.unit == unit) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Can't shutdown unit,"
                                 " entry for LT %s had not been freed\n"),
                            e_list->info.table_name));
            SHR_ERR_EXIT(SHR_E_BUSY);
        }
        e_list = e_list->debug_next;
    }

exit:
    if (mtx) {
        sal_mutex_give(mtx);
    }
    SHR_FUNC_EXIT();
}

static void unit_init_cb(shr_sysm_categories_t instance_cat,
                         int unit,
                         int status)
{
    if (instance_cat == SHR_SYSM_CAT_UNIT) {
        unit_attach_stat[unit] = (status == SHR_E_NONE);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "System init returned %d\n"), status));
        sal_sem_give(unit_attached_sem[unit]);
    }
}

/*================================================================*
 *     C O M P O N E N T    E X T E R N A L   F U N C T I O N S   *
 *================================================================*/
int bcmlt_device_attach(int unit, bool warm)
{
    bool valid_unit = true;

    SHR_FUNC_ENTER(unit);
    if (!bcmlt_is_initialized()) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    if ((unit < 0) || (unit >= BCMDRD_CONFIG_MAX_UNITS)) {
        valid_unit = false;
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    if (unit_attached_map[unit]) {
        valid_unit = false;    /* To avoid the cleanup */
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    /* Verify that the device exist */
    if (!bcmdrd_dev_exists(unit)) {
        return SHR_E_UNIT;
    }

    if (!unit_attached_sem[unit]) {
        unit_attached_sem[unit] = sal_sem_create("bcmltAttach",
                                                 SAL_SEM_BINARY, 0);
        SHR_NULL_CHECK(unit_attached_sem[unit], SHR_E_MEMORY);
    }
    unit_attach_stat[unit] = false;
    SHR_IF_ERR_EXIT(shr_sysm_instance_new(SHR_SYSM_CAT_UNIT,
                                          unit,
                                          warm,
                                          false,
                                          unit_init_cb));

    sal_sem_take(unit_attached_sem[unit], SAL_SEM_FOREVER);
    if (!unit_attach_stat[unit]) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Failed unit attach\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Successfully attached unit\n")));

    /* Inform the TRM about the max number of fields in interactive tables */
    bcmtrm_interactive_table_init(unit,
                                  warm,
                                  bcmlt_db_interact_tbl_max_fields(unit));
    unit_attached_map[unit] = true;
exit:
    if (SHR_FUNC_ERR() && valid_unit && unit_attached_sem[unit]) {
        shr_sysm_instance_delete(SHR_SYSM_CAT_UNIT, unit);
        sal_sem_destroy(unit_attached_sem[unit]);
        unit_attached_sem[unit] = NULL;
    }
    SHR_FUNC_EXIT();
}

static void unit_stop_cb(shr_sysm_categories_t instance_cat,
                         int unit,
                         int status)
{
    if (instance_cat == SHR_SYSM_CAT_UNIT) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "System stop returned %d\n"), status));
        /* Unit stayed attached is the status is not OK or Partial */
        unit_attached_map[unit] = (status != SHR_E_NONE &&
                                   status != SHR_E_PARTIAL);
        sal_sem_give(unit_attached_sem[unit]);
    }
}

int bcmlt_device_detach(int unit)
{
    SHR_FUNC_ENTER(unit);
    if ((unit < 0) || (unit >= BCMDRD_CONFIG_MAX_UNITS) ||
        !unit_attached_map[unit]) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT(shr_sysm_instance_stop(SHR_SYSM_CAT_UNIT,
                                           unit,
                                           true,
                                           unit_stop_cb));
    sal_sem_take(unit_attached_sem[unit], SAL_SEM_FOREVER);
    /* Return error if the unit is still attached */
    if (unit_attached_map[unit]) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
    sal_sem_destroy(unit_attached_sem[unit]);
    shr_sysm_instance_delete(SHR_SYSM_CAT_UNIT, unit);
    unit_attached_sem[unit] = NULL;
exit:
    SHR_FUNC_EXIT();
}

bool bcmlt_device_attached(int unit)
{
    return ((unit >= 0) && (unit < BCMDRD_CONFIG_MAX_UNITS) &&
            unit_attached_map[unit]);
}

int bcmlt_api_clear(int unit)
{
    bcmtrm_entry_t *e_list;
    bcmtrm_trans_t *t_list;
    sal_mutex_t mtx = NULL;
    bcmtrm_trans_t *trans_it;
    bcmtrm_entry_t *entry_it;

    SHR_FUNC_ENTER(unit);
    if ((unit < 0) || (unit >= BCMDRD_CONFIG_MAX_UNITS) ||
        !unit_attached_map[unit]) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    mtx = bcmlt_entry_sync_obj_get();
    /* The entire operation should be mutex protected */
    if (mtx) {
        sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
    }

    bcmtrm_active_element_lists_get(&e_list, &t_list);

    /*
     * Before clearing the transactions clear all the entry handles as handles
     * are only accessible via this layer.
     */
    entry_it = e_list;
    while (entry_it) {
        if (entry_it->info.unit == unit && entry_it->state == E_ACTIVE) {
            bcmlt_hdl_free(entry_it->info.entry_hdl);
            entry_it->info.entry_hdl = 0;
        }
        entry_it = entry_it->debug_next;
    }

    /*
     * First clear the transaction list as each will clean up all the entries
     * associated with these transactions.
     */
    while (t_list) {
        trans_it = t_list;
        t_list = t_list->debug_next;
        if (trans_it->unit == unit && trans_it->state == T_ACTIVE) {
            bcmlt_hdl_free(trans_it->info.trans_hdl);
            trans_it->info.trans_hdl = 0;
            bcmtrm_trans_free(trans_it);
        }
    }

    /*
     * Refresh the entry list as it may have changed due to processing the
     * transaction above.
     */
    bcmtrm_active_element_lists_get(&e_list, NULL);

    while (e_list) {
        entry_it = e_list;
        e_list = e_list->debug_next;
        if (entry_it->info.unit == unit && entry_it->state == E_ACTIVE &&
            !entry_it->p_trans) {
            bcmtrm_entry_free(entry_it);
        }
    }
    if (mtx) {
        sal_mutex_give(mtx);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmlt_stat_entry(bcmlt_entity_stats_t *stats)
{
    shr_lmm_stat_t stat;

    if (!bcmlt_initialized) {
        return SHR_E_INIT;
    }
    if (!stats) {
        return SHR_E_PARAM;
    }

    bcmtrm_get_entry_hdl_stat(&stat);
    stats->avail = stat.avail;
    stats->in_use = stat.in_use;
    stats->high_wtr_mark = stat.hwtr_mark;
    return SHR_E_NONE;
}

int bcmlt_stat_transaction(bcmlt_entity_stats_t *stats)
{
    shr_lmm_stat_t stat;

    if (!bcmlt_initialized) {
        return SHR_E_INIT;
    }
    if (!stats) {
        return SHR_E_PARAM;
    }

    bcmtrm_get_trans_hdl_stat(&stat);
    stats->avail = stat.avail;
    stats->in_use = stat.in_use;
    stats->high_wtr_mark = stat.hwtr_mark;
    return SHR_E_NONE;
}

/*! \file bcmpc_db_imm.c
 *
 * IMM-based database APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_fmm.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_db_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_DB

/* The maximum number of PC LT fields. */
#define PC_FIELDS_NUM_MAX 128

/* PC fields element type. */
typedef struct pc_fields_s {
    bcmltd_field_t *field[PC_FIELDS_NUM_MAX];
    struct pc_fields_s *next;
} pc_fields_t;

/*
 * PC fields chunk size.
 *
 * The IMM interface uses the LMM API to allocate FMM field buffers in
 * order to minimize the number of calls to sal_alloc.
 *
 * Each LMM chunk represents a complete FMM field array of which we
 * need at least two (one for input data and one for output data).
 */
#define PC_FIELDS_CHUNK_SIZE 2

/* PC fields lmem handlers. */
static shr_lmm_hdl_t pc_fields_hdls[BCMPC_NUM_UNITS_MAX];

/* PC LT schema handlers. */
static bcmpc_db_entry_t *pc_db_schema_hdls[BCMPC_NUM_UNITS_MAX];

/*!
 * \brief PC LT schema handler entry structure.
 *
 * The registered schema handers are stored within a linked list.
 * This data structure represents the element of the list.
 *
 * The key of the entry, i.e. \c dbe->key, is the table ID.
 */
typedef struct pc_db_table_s {

    /*! DB entry. */
    bcmpc_db_entry_t dbe;

    /*! Schema handler. */
    bcmpc_db_imm_schema_handler_t hdl;

} pc_db_table_t;


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the schema handler for the given LT.
 *
 * \param [in] unit Unit number.
 * \param [in] sid sid Logical table ID.
 *
 * \return The pointer to the schema handler on success, NULL otherwise.
 */
static const bcmpc_db_imm_schema_handler_t *
pc_db_imm_schema_get(int unit, bcmltd_sid_t sid)
{
    pc_db_table_t *entry;
    bcmpc_db_entry_t *dbe;

    dbe = bcmpc_db_entry_search(pc_db_schema_hdls[unit], 1, &sid, NULL);
    if (!dbe) {
        return NULL;
    }

    entry = BCMPC_DB_ENTRY(dbe, pc_db_table_t, dbe);
    return &entry->hdl;
}

/*!
 * \brief Cleanup the resource of PC LT schema handlers.
 *
 * \param [in] unit Unit number.
 */
static void
pc_db_imm_schema_cleanup(int unit)
{
    bcmpc_db_entry_t *dbe, *next;
    pc_db_table_t *entry;

    dbe = bcmpc_db_entry_next(pc_db_schema_hdls[unit], NULL);

    while (dbe) {
        next = bcmpc_db_entry_next(pc_db_schema_hdls[unit], dbe);
        entry = BCMPC_DB_ENTRY(dbe, pc_db_table_t, dbe);
        bcmpc_db_entry_del(&pc_db_schema_hdls[unit], 1, entry->dbe.keys);
        SHR_FREE(entry);

        dbe = next;
    }
}

/*!
 * \brief Free bcmltd_fields_t resource.
 *
 * This function will always free PC_FIELDS_NUM_MAX fields.
 *
 * \param [in] unit Unit number.
 * \param [in] flds LT field array.
 */
static void
pc_fields_free(int unit, bcmltd_fields_t *flds)
{
    int i;

    if (flds->count == 0) {
        return;
    }

    for (i = 0; i < PC_FIELDS_NUM_MAX; i++) {
        shr_fmm_free(flds->field[i]);
    }
    shr_lmm_free(pc_fields_hdls[unit], flds->field);
}

/*!
 * \brief Clear bcmltd_fields_t resource.
 *
 * This function will always cleanup PC_FIELDS_NUM_MAX fields.
 *
 * \param [in] flds LT field array.
 */
static void
pc_fields_clear(bcmltd_fields_t *flds)
{
    int i;

    for (i = 0; i < PC_FIELDS_NUM_MAX; i++) {
        sal_memset(flds->field[i], 0, sizeof(bcmltd_field_t));
    }
    flds->count = PC_FIELDS_NUM_MAX;
}

/*!
 * \brief Allocate bcmltd_fields_t resource.
 *
 * This function will always try to allocate PC_FIELDS_NUM_MAX fields.
 * If the fields allocation fails, it will return an error code and
 * set \c flds->count = 0.
 *
 * \param [in] unit Unit number.
 * \param [out] flds LT field array.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_fields_alloc(int unit, bcmltd_fields_t *flds)
{
    size_t i;

    SHR_FUNC_ENTER(unit);

    flds->field = shr_lmm_alloc(pc_fields_hdls[unit]);
    SHR_NULL_CHECK(flds->field, SHR_E_MEMORY);
    sal_memset(flds->field, 0, PC_FIELDS_NUM_MAX * sizeof(bcmltd_field_t *));

    flds->count = 0;
    for (i = 0; i < PC_FIELDS_NUM_MAX; i++) {
        flds->field[i] = shr_fmm_alloc();
        SHR_NULL_CHECK(flds->field[i], SHR_E_MEMORY);
        sal_memset(flds->field[i], 0, sizeof(bcmltd_field_t));
        flds->count++;
    }

exit:
    if (SHR_FUNC_ERR()) {
        for (i = 0; i < flds->count; i++) {
            shr_fmm_free(flds->field[i]);
        }
        shr_lmm_free(pc_fields_hdls[unit], flds->field);
        flds->count = 0;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Fill bcmltd_fields_t resource.
 *
 * This function will fill \c flds with the given \c key and \c data, and return
 * the schema handler \c hdl of table \c sid as well.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 * \param [in] data_size Data size.
 * \param [in] data Data value.
 * \param [out] flds LT field array.
 * \param [out] hdl LT schema handler.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_fields_fill(int unit, bcmltd_sid_t sid,
               size_t key_size, void *key,
               size_t data_size, void *data,
               bcmltd_fields_t *flds,
               const bcmpc_db_imm_schema_handler_t **hdl)
{
    const bcmpc_db_imm_schema_handler_t *schema_hdl;

    SHR_FUNC_ENTER(unit);

    schema_hdl = pc_db_imm_schema_get(unit, sid);
    SHR_NULL_CHECK(schema_hdl, SHR_E_NO_HANDLER);

    SHR_IF_ERR_EXIT
        (pc_fields_alloc(unit, flds));

    SHR_IF_ERR_EXIT
        (schema_hdl->entry_fill(unit, key_size, key, data_size, data, flds));

    if (hdl) {
        *hdl = schema_hdl;
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmpc_db_imm_init(int unit)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    if (pc_fields_hdls[unit]) {
        SHR_EXIT();
    }

    LMEM_MGR_INIT
        (pc_fields_t, pc_fields_hdls[unit], PC_FIELDS_CHUNK_SIZE, true, rv);

    if (rv != 0) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_MEMORY,
             (BSL_META_U(unit, "Failed to initialize pc_fields_hdl.\n")));
   }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_db_imm_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    pc_db_imm_schema_cleanup(unit);

    shr_lmm_delete(pc_fields_hdls[unit]);
    pc_fields_hdls[unit] = NULL;

    SHR_FUNC_EXIT();
}

int
bcmpc_db_imm_schema_reg(int unit, bcmltd_sid_t sid,
                        bcmpc_db_imm_schema_handler_t *hdl)
{
    pc_db_table_t *entry = NULL;
    bcmpc_db_entry_t *dbe;

    SHR_FUNC_ENTER(unit);

    /* When the entry exists, update the data of the entry. */
    dbe = bcmpc_db_entry_search(pc_db_schema_hdls[unit], 1, &sid, NULL);
    if (dbe) {
        entry = BCMPC_DB_ENTRY(dbe, pc_db_table_t, dbe);
        sal_memcpy(&(entry->hdl), hdl, sizeof(*hdl));
        SHR_EXIT();
    }

    /* Otherwise, allocate memory for saving the new entry. */
    SHR_ALLOC(entry, sizeof(pc_db_table_t), "bcmpcDbImmHdl");
    SHR_NULL_CHECK(entry, SHR_E_MEMORY);

    sal_memset(entry, 0, sizeof(*entry));
    entry->dbe.keys[0] = sid;
    sal_memcpy(&entry->hdl, hdl, sizeof(*hdl));
    bcmpc_db_entry_add(&pc_db_schema_hdls[unit], &entry->dbe);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_db_imm_tbl_valid(int unit, bcmltd_sid_t sid)
{
    const bcmpc_db_imm_schema_handler_t *hdl;

    SHR_FUNC_ENTER(unit);

    hdl = pc_db_imm_schema_get(unit, sid);
    SHR_NULL_CHECK(hdl, SHR_E_NO_HANDLER);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_db_imm_entry_lookup(int unit, bcmltd_sid_t sid,
                          size_t key_size, void *key,
                          size_t data_size, void *data)
{
    int rv;
    const bcmpc_db_imm_schema_handler_t *hdl;
    bcmltd_fields_t in, out;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0 , sizeof(in));
    sal_memset(&out, 0 , sizeof(out));

    SHR_IF_ERR_EXIT
        (pc_fields_alloc(unit, &out));

    rv = pc_fields_fill(unit, sid, key_size, key, 0, NULL, &in, &hdl);

    /* When there is no table handler, return E_NONE directly. */
    if (rv == SHR_E_NO_HANDLER) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, sid, &in, &out));

    if (data) {
        SHR_IF_ERR_EXIT
            (hdl->entry_parse(unit, &out, 0, NULL, data_size, data));
    }

exit:
    pc_fields_free(unit, &in);
    pc_fields_free(unit, &out);

    SHR_FUNC_EXIT();
}

int
bcmpc_db_imm_entry_update(int unit, bcmltd_sid_t sid,
                          size_t key_size, void *key,
                          size_t data_size, void *data)
{
    bcmltd_fields_t in;
    int rv, hi_pri = 1;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0 , sizeof(in));

    rv = pc_fields_fill(unit, sid, key_size, key, data_size, data, &in, NULL);

    /* When there is no table handler, return E_NONE directly. */
    if (rv == SHR_E_NO_HANDLER) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, hi_pri, sid, &in));

exit:
    pc_fields_free(unit, &in);

    SHR_FUNC_EXIT();
}

int
bcmpc_db_imm_entry_delete(int unit, bcmltd_sid_t sid,
                          size_t key_size, void *key)
{
    int rv;
    bcmltd_fields_t in;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0 , sizeof(in));

    rv = pc_fields_fill(unit, sid, key_size, key, 0, NULL, &in, NULL);

    /* When there is no table handler, return E_NONE directly. */
    if (rv == SHR_E_NO_HANDLER) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    SHR_IF_ERR_EXIT
        (bcmimm_entry_delete(unit, sid, &in));

exit:
    pc_fields_free(unit, &in);

    SHR_FUNC_EXIT();
}

int
bcmpc_db_imm_entry_insert(int unit, bcmltd_sid_t sid,
                          size_t key_size, void *key,
                          size_t data_size, void *data)
{
    int rv;
    bcmltd_fields_t in;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0 , sizeof(in));

    rv = pc_fields_fill(unit, sid, key_size, key, data_size, data, &in, NULL);

    /* When there is no table handler, return E_NONE directly. */
    if (rv == SHR_E_NO_HANDLER) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    SHR_IF_ERR_EXIT
        (bcmimm_entry_insert(unit, sid, &in));

exit:
    pc_fields_free(unit, &in);

    SHR_FUNC_EXIT();
}

int
bcmpc_db_imm_entry_lock(int unit, bcmltd_sid_t sid,
                        size_t key_size, void *key)
{
    int rv;
    bcmltd_fields_t in;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0 , sizeof(in));

    rv = pc_fields_fill(unit, sid, key_size, key, 0, NULL, &in, NULL);

    /* When there is no table handler, return E_NONE directly. */
    if (rv == SHR_E_NO_HANDLER) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    SHR_IF_ERR_EXIT
        (bcmimm_entry_lock(unit, sid, &in));

exit:
    pc_fields_free(unit, &in);

    SHR_FUNC_EXIT();
}

int
bcmpc_db_imm_entry_unlock(int unit, bcmltd_sid_t sid,
                          size_t key_size, void *key)
{
    int rv;
    bcmltd_fields_t in;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0 , sizeof(in));

    rv = pc_fields_fill(unit, sid, key_size, key, 0, NULL, &in, NULL);

    /* When there is no table handler, return E_NONE directly. */
    if (rv == SHR_E_NO_HANDLER) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    SHR_IF_ERR_EXIT
        (bcmimm_entry_unlock(unit, sid, &in));

exit:
    pc_fields_free(unit, &in);

    SHR_FUNC_EXIT();
}

int
bcmpc_db_imm_entry_traverse(int unit, bcmltd_sid_t sid,
                            size_t key_size, size_t data_size,
                            bcmpc_db_imm_traverse_cb_f cb_func, void *cookie)
{
    int rv;
    const bcmpc_db_imm_schema_handler_t *hdl;
    char *kbuf = NULL, *dbuf = NULL;
    bcmltd_fields_t in, out;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0 , sizeof(in));
    sal_memset(&out, 0 , sizeof(out));

    if (!cb_func) {
        return SHR_E_NONE;
    }

    hdl = pc_db_imm_schema_get(unit, sid);
    /* When there is no table handler, return E_NONE directly. */
    if (!hdl) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (pc_fields_alloc(unit, &in));
    SHR_IF_ERR_EXIT
        (pc_fields_alloc(unit, &out));

    SHR_ALLOC(kbuf, key_size, "bcmpcKbuf");
    SHR_NULL_CHECK(kbuf, SHR_E_MEMORY);
    SHR_ALLOC(dbuf, data_size, "bcmpcDbuf");
    SHR_NULL_CHECK(dbuf, SHR_E_MEMORY);

    /* Get the first entry. */
    rv = bcmimm_entry_get_first(unit, sid, &out);

    while (SHR_SUCCESS(rv)) {
        sal_memset(kbuf, 0, key_size);
        sal_memset(dbuf, 0, data_size);
        SHR_IF_ERR_EXIT
            (hdl->entry_parse(unit, &out, key_size, kbuf, data_size, dbuf));

        /*
         * Get the next entry before executing the callback function because
         * the callback might delete the current entry.
         */
        pc_fields_clear(&in);
        pc_fields_clear(&out);
        SHR_IF_ERR_EXIT
            (hdl->entry_fill(unit, key_size, kbuf, 0, NULL, &in));
        rv = bcmimm_entry_get_next(unit, sid, &in, &out);

        SHR_IF_ERR_EXIT
            (cb_func(unit, key_size, kbuf, data_size, dbuf, cookie));

        /*
         * The following is a workaround to restart the traverse operation
         * which could otherwise stop prematurely if the array is updated
         * during the traverse operation.
         * Should be revisited if the traverse mechanism is updated.
         */
        /* Regenerate the snapshot in BCMIMM. */
        if (rv == SHR_E_UNAVAIL) {
            pc_fields_clear(&in);
            pc_fields_clear(&out);
            rv = bcmimm_entry_get_first(unit, sid, &in);
            SHR_IF_ERR_EXIT
                (hdl->entry_fill(unit, key_size, kbuf, 0, NULL, &out));
        }
    }

exit:
    SHR_FREE(kbuf);
    SHR_FREE(dbuf);
    pc_fields_free(unit, &in);
    pc_fields_free(unit, &out);

    SHR_FUNC_EXIT();
}

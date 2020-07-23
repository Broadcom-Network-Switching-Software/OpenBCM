/*! \file bcmlt_ltable_field.c
 *
 * Handles regular fields.
 * This module contains functions that handles field functionality. A field
 * can be symbol or scalar. The field can not be array field. These been
 * handled by a different module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_alloc.h>
#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_fmm.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlt/bcmlt.h>
#include <bcmtrm/trm_api.h>
#include <bcmltm/bcmltm_md.h>
#include <bcmltd/bcmltd_table.h>
#include "bcmlt_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLT_ENTRY

/* Adds a field into an entry */
#define FIELD_ADD(entry, fname, sym, field)  \
    if (!fname) {                                                \
        SHR_ERR_EXIT(SHR_E_PARAM);                        \
    }                                                            \
    { int _rv;                                                   \
        if ((_rv = alloc_new_field(entry, fname, sym, field))    \
            != SHR_E_NONE) {                                     \
            SHR_ERR_EXIT(_rv);                            \
        }                                                        \
    }

/* Obtain a field from an entry */
#define FIELD_GET(entry, fname, field) \
        if (!entry || (entry->state == E_IDLE) ||                    \
            !fname) {                                                \
            SHR_ERR_EXIT(SHR_E_PARAM);                        \
        }                                                            \
        { int _rv;                                                   \
            if ((_rv = find_field_by_name(entry, fname, field)) \
                 != SHR_E_NONE) {                                    \
                 SHR_ERR_EXIT(_rv);                           \
            }                                                        \
        }

#ifndef UINT32_MAX
#define UINT32_MAX 0xffffffffL
#endif

/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Allocates new field based on field ID and adds it to the entry.
 *
 * This function adds a new field onto an entry or return the field if
 * already exist.
 *
 * \param [in] entry is the entry to add the field into.
 * \param [in] fid is the field ID.
 *
 * \return Pointer to a field - Success.
 * \return NULL - Failure
 */
static shr_fmm_t *field_add_by_id(bcmtrm_entry_t *entry,
                                  uint32_t fid)
{
    shr_fmm_t *field;

    if (entry->fld_arr) {
        field = entry->fld_arr[fid];
    } else {
        field = bcmlt_find_field_in_entry(entry, fid, NULL);
    }
    if (!field) {
        if (entry->fld_mem_hdl) {
            if (!entry->free_fld) {
                sal_mutex_take(entry->entry_free_sync, SAL_MUTEX_FOREVER);
                field = shr_lmm_alloc(entry->fld_mem_hdl);
                sal_mutex_give(entry->entry_free_sync);
            } else {
                field = entry->free_fld;
                entry->free_fld = field->next;
            }
        } else {
            field = shr_fmm_alloc();
        }
        if (!field) {
            return NULL;
        }
        field->id = fid;
        field->idx = 0;
        field->flags = 0;
        field->next = entry->l_field;
        entry->l_field = field;
        if (entry->fld_arr) {
            entry->fld_arr[fid] = field;
        }
    }
    return field;
}

/*!
 *\brief Allocates new field and adds it to the entry.
 *
 * This function adds a new field onto an entry or update the value of
 * the field if already in the entry.
 *
 * \param [in] entry is the entry to add the field into.
 * \param [in] field_name is the name of the field to add.
 * \param [in] sym is true if the field is of symbol type.
 * \param [out] field is a pointer to the newly created/existed field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int alloc_new_field(bcmtrm_entry_t *entry,
                           const char *field_name,
                           bool sym,
                           shr_fmm_t **field)
{
    uint32_t field_id;
    bcmlt_field_def_t *attr;

    SHR_FUNC_ENTER(entry->info.unit);

    /* Do not allow to add field for notification */
    if (!entry->db_hdl) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    /* Get field ID from DB */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmlt_db_field_info_get(entry->info.unit,
                                field_name,
                                entry->db_hdl,
                                &attr,
                                &field_id));

    if ((attr->symbol != sym) ||
        (attr->depth > 0) || (attr->elements > 1)) { /* Is array type? */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit, "Field %s is not a scalar\n"),
                     field_name));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    *field = field_add_by_id(entry, field_id);
    SHR_NULL_CHECK(*field, SHR_E_MEMORY);
exit:
    SHR_FUNC_EXIT();
}

/*!
 *\brief Search for a field in an entry based on field name.
 *
 * This function searches the given entry for a specific field that matches
 * the field name. If found the function set the output value of \c field.
 * Note that the entry only contains internal field ID, therefore the function
 * searches the internal DB for the internal field ID.
 *
 * \param [in] entry is the entry to add the field into.
 * \param [in] field_name is the name of the field to search.
 * \param [out] field is a pointer to the newly created/existed field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int find_field_by_name(bcmtrm_entry_t *entry,
                               const char *field_name,
                               shr_fmm_t **field)
{
    uint32_t field_id;
    bcmlt_field_def_t *attr;

    SHR_FUNC_ENTER(entry->info.unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmlt_db_field_info_get(entry->info.unit,
                                field_name,
                                entry->db_hdl,
                                &attr,
                                &field_id));
    if (attr->depth > 0 || attr->elements > 1) { /* Is it array type? */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (entry->fld_arr) {
        *field = entry->fld_arr[field_id];
    } else {
        *field = bcmlt_find_field_in_entry(entry, field_id, NULL);
    }
    if (*field == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int bcmlt_entry_field_add(bcmlt_entry_handle_t entry_hdl,
                          const char *field_name,
                          uint64_t data)
{
    shr_fmm_t *field;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    FIELD_ADD(entry, field_name, false, &field);
    field->data = data;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_symbol_add(bcmlt_entry_handle_t entry_hdl,
                                 const char *field_name,
                                 const char *data)
{
    shr_fmm_t *field = NULL;
    int rv;
    uint32_t val;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    FIELD_ADD(entry, field_name, true, &field);
    if (!data) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    rv = bcmlrd_field_symbol_to_value(entry->info.unit,
                                 entry->table_id,
                                 field->id,
                                 data,
                                 &val);
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    field->data = val;
exit:
    /*
     * Failure can be due to invalid symbol or invalid field name. If the
     * later then field is NULL. Don't delete it!
     */
    if (SHR_FUNC_ERR() && field) {
        shr_fmm_t *prev_field;
        /* Delete the field */
        if (entry->fld_arr) {
            entry->fld_arr[field->id] = NULL;
        }
        field = bcmlt_find_field_in_entry(entry, field->id, &prev_field);
        if (!prev_field) {
            entry->l_field = field->next;
        } else {
            prev_field->next = field->next;
        }
        shr_fmm_free(field);
    }
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_get(bcmlt_entry_handle_t entry_hdl,
                          const char *field_name,
                          uint64_t *data)
{
    shr_fmm_t *field;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_GET_VALIDATE(entry);
    FIELD_GET(entry, field_name, &field);
    if (!data) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    *data = field->data;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_symbol_get(bcmlt_entry_handle_t entry_hdl,
                                 const char *field_name,
                                 const char **data)
{
    shr_fmm_t *field;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_GET_VALIDATE(entry);
    FIELD_GET(entry, field_name, &field);
    if (!data) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(
        bcmlrd_field_value_to_symbol(entry->info.unit,
                                      entry->table_id,
                                      field->id,
                                      (uint32_t)(field->data & 0xFFFFFFFF),
                                      data));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_unset(bcmlt_entry_handle_t entry_hdl,
                            const char *field_name)
{
    shr_fmm_t *field;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);
    uint32_t field_id;
    bcmlt_field_def_t *f_def;

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);

    ENTRY_VALIDATE(entry);
    if (!field_name) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmlt_db_field_info_get(entry->info.unit,
                                field_name,
                                entry->db_hdl,
                                &f_def,
                                &field_id));
    if (f_def->depth > 0 || f_def->elements > 1) { /* Check if it is array */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    field = field_add_by_id(entry, field_id);
    field->data = f_def->def;
    field->flags |= SHR_FMM_FIELD_DEL;

exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_remove(bcmlt_entry_handle_t entry_hdl,
                             const char *field_name)
{
    shr_fmm_t *p_field;
    shr_fmm_t *p_field_tmp;
    int rv;
    uint32_t field_id;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);
    bcmlt_field_def_t *f_def;

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    if (!field_name) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    rv = bcmlt_db_field_info_get(entry->info.unit,
                                 field_name,
                                 entry->db_hdl,
                                 &f_def,
                                 &field_id);
    if (rv != 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (f_def->depth > 0 || f_def->elements > 1) { /* Check if it is array */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (!entry->l_field) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (entry->fld_arr) {
        entry->fld_arr[field_id] = NULL;    /* Clean up the field array */
    }

    p_field = entry->l_field;
    /* If the field is the first in the list */
    if (p_field->id == field_id) {
        entry->l_field = p_field->next;
        if (entry->fld_mem_hdl) {
            p_field->next = entry->free_fld;
            entry->free_fld = p_field;
        } else {
            shr_fmm_free(p_field);
        }
        SHR_EXIT();
    }
    /* Search the rest of the field list */
    for (; p_field->next && (p_field->next->id != field_id);
         p_field = p_field->next);
    if (!p_field->next) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    p_field_tmp = p_field->next;
    p_field->next = p_field_tmp->next; /* Remove the field from the list */
    /* Add the field to the free fields */
    if (entry->fld_mem_hdl) {
        p_field_tmp->next = entry->free_fld;
        entry->free_fld = p_field_tmp;
    } else {
        shr_fmm_free(p_field_tmp);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_symbol_info_get(bcmlt_entry_handle_t entry_hdl,
                                      const char *field_name,
                                      uint32_t sym_names_size,
                                      const char *sym_names[],
                                      uint32_t *actual_size)
{
    uint32_t field_id;
    bcmlt_field_def_t *attr;
    shr_enum_map_t *names = NULL;
    size_t sym_count;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    if (!field_name) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmlt_db_field_info_get(entry->info.unit,
                            field_name,
                            entry->db_hdl,
                            &attr,
                            &field_id));
    if (!attr->symbol) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Field %s is not of symbol type\n"),
                     field_name));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT(bcmlrd_field_symbols_count_get(entry->info.unit,
                                        entry->table_id,
                                        field_id,
                                        &sym_count));
    if (actual_size) {
        *actual_size = (uint32_t)sym_count;
    }
    if (sym_names) {
        size_t j;
        size_t act;
        if (sym_names_size < sym_count) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        names = sal_alloc(sizeof(shr_enum_map_t) * sym_count, "bcmltField");
        SHR_NULL_CHECK(names, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(bcmlrd_field_symbols_get(entry->info.unit,
                                                 entry->table_id,
                                                 field_id,
                                                 sym_count,
                                                 names,
                                                 &act));
        if (act != sym_count) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        for (j = 0; j < sym_count; j++) {
            sym_names[j] = names[j].name;
        }
    }
exit:
    if (names) {
        sal_free(names);
    }
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_traverse(bcmlt_entry_handle_t entry_hdl,
                               const char **field_name,
                               bcmlt_field_data_info_t *data,
                               uint32_t *search_hdl)
{
    shr_fmm_t *field;
    bcmlrd_field_def_t field_info;
    bcmdrd_sym_field_info_t f_info;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);
    uint32_t j;

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    if (!search_hdl || !data || !field_name) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (!entry->l_field) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Find the next field */
    field = entry->l_field;
    if (*search_hdl != BCMLT_INVALID_FIELD_SEARCH_HDL) {
        for (j = 0; field && j <= *search_hdl; j++, field = field->next);
        if (!field) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        *search_hdl = j;
    } else {
        *search_hdl = 0;
    }

    /* Obtain the field information */
    if (entry->pt) {
        SHR_IF_ERR_EXIT(bcmltm_md_pt_field_info_get(entry->info.unit,
                                                    entry->table_id,
                                                    field->id,
                                                    &f_info));
        *field_name = f_info.name;
        data->symbol = false;
        data->array = true;
    } else {
        SHR_IF_ERR_EXIT(bcmlrd_table_field_def_get(entry->info.unit,
                                                   entry->table_id,
                                                   field->id,
                                                   &field_info));

        *field_name = field_info.name;
        data->symbol = field_info.symbol;
        data->array = field_info.array;
    }
    data->idx = field->idx;
    if (data->symbol) {
        SHR_IF_ERR_EXIT(
                        bcmlrd_field_value_to_symbol(entry->info.unit,
                                                     entry->table_id,
                                                     field->id,
                                                     field->data,
                                                     &data->sym_val));
    } else {
        data->val = field->data;
        data->sym_val = NULL;
    }
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_count(bcmlt_entry_handle_t entry_hdl,
                            uint32_t *field_count)
{
    shr_fmm_t *field;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);
    uint32_t count = 0;

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    if (!field_count) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    field = entry->l_field;
    while (field) {
        field = field->next;
        count++;
    }

    *field_count = count;
exit:
    SHR_FUNC_EXIT();
}

int
bcmlt_table_field_id_get_by_name(int unit,
                                 const char *tbl_name,
                                 const char *fld_name,
                                 uint32_t *ltid,
                                 uint32_t *lfid)
{
    void *hdl;
    bcmlt_table_attrib_t *table_attr;
    uint32_t lt_tid;
    uint32_t lt_fid = 0;

    SHR_FUNC_ENTER(unit);
    if ((!tbl_name) || (!fld_name)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    UNIT_VALIDATION(unit);
    /* Get table id by name. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmlt_db_table_info_get(unit, tbl_name, &table_attr,
                                 NULL, &hdl));
    lt_tid = table_attr->table_id;

    /* Get field id by name. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_db_field_info_get(unit, fld_name, hdl, NULL, &lt_fid));
    if (ltid) {
        *ltid = lt_tid;
    }
    if (lfid) {
        *lfid = lt_fid;
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmlt_field_desc_get(int unit,
                         const char *tbl_name,
                         const char *fld_name,
                         const char **fld_desc)
{
    int tbl_id;
    uint32_t fld;
    int field_found = FALSE;
    const bcmltd_table_rep_t *lt_info;
    const bcmltd_field_rep_t *fld_info;

    SHR_FUNC_ENTER(unit);

    /* Sanity checks. */
    UNIT_VALIDATION(unit);
    if (!bcmlt_is_initialized()) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    if ((!tbl_name) || (!fld_name) || (!fld_desc)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tbl_id = bcmlrd_table_name_to_idx(unit, tbl_name);

    /* Return if table is not found. */
    if (tbl_id == SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(tbl_id);
    }

    lt_info = bcmltd_table_get((bcmltd_sid_t)tbl_id);
    if (lt_info == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Loop through fields in LT to find the requested field. */
    for (fld = 0; fld < lt_info->fields; fld++) {
        fld_info = &lt_info->field[fld];
        if (!sal_strcmp(fld_name, fld_info->name)) {
            *fld_desc = fld_info->desc;
            field_found = TRUE;
            break;
        }
    }

    if (field_found == FALSE) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_add_by_id(bcmlt_entry_handle_t entry_hdl,
                                uint32_t fid,
                                uint64_t data)
{
    shr_fmm_t *field;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    VALIDATE_FID(entry, fid);

    field = field_add_by_id(entry, fid);
    SHR_NULL_CHECK(field, SHR_E_MEMORY);

    field->data = data;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_get_by_id(bcmlt_entry_handle_t entry_hdl,
                                uint32_t fid,
                                uint64_t *data)
{
    shr_fmm_t *field;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_GET_VALIDATE(entry);
    VALIDATE_FID(entry, fid);

    if (entry->fld_arr) {
        field = entry->fld_arr[fid];
    } else {
        field = bcmlt_find_field_in_entry(entry, fid, NULL);
    }
    if (!field) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    if (!data) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    *data = field->data;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_symbol_add_by_id(bcmlt_entry_handle_t entry_hdl,
                                       uint32_t fid,
                                       const char *data)
{
    shr_fmm_t *field = NULL;
    int rv;
    uint32_t val;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    VALIDATE_FID(entry, fid);
    if (!data) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    rv = bcmlrd_field_symbol_to_value(entry->info.unit,
                                 entry->table_id,
                                 fid,
                                 data,
                                 &val);
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    field = field_add_by_id(entry, fid);
    SHR_NULL_CHECK(field, SHR_E_MEMORY);
    field->data = val;

exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_symbol_get_by_id(bcmlt_entry_handle_t entry_hdl,
                                       uint32_t fid,
                                       const char **data)
{
    shr_fmm_t *field;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_GET_VALIDATE(entry);
    VALIDATE_FID(entry, fid);

    if (entry->fld_arr) {
        field = entry->fld_arr[fid];
    } else {
        field = bcmlt_find_field_in_entry(entry, fid, NULL);
    }
    if (!field) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    if (!data) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(
        bcmlrd_field_value_to_symbol(entry->info.unit,
                                      entry->table_id,
                                      fid,
                                      (uint32_t)(field->data & 0xFFFFFFFF),
                                      data));
exit:
    SHR_FUNC_EXIT();
}



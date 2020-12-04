/*! \file lt_intf.c
 *
 * LT Wrapper interface.
 * This file contains the lightweight wrapper of SDKLT LT APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <bcm/types.h>

#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_LT

#define LT_ENTRY_NFIELDS_CHECK(u, nfields)                                   \
    do {                                                                     \
        if (nfields > BCMI_LT_FIELD_MAX) {                                   \
            LOG_WARN(BSL_LOG_MODULE,                                         \
                     (BSL_META_U(u,                                          \
                      "The number of LT fields exceeds the MAX value.\n"))); \
        }                                                                    \
    } while(0)

/******************************************************************************
* Private functions
 */

/*!
 * \brief Set entry fields into the entry container based on handle.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  entry_hdl     Handle to the entry.
 * \param [in]  entry         LT entry information.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
lt_entry_info_set(int unit, bcmlt_entry_handle_t entry_hdl,
                  bcmi_lt_entry_t *entry)
{
    int i = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < entry->nfields; i++) {

        if (!(entry->fields[i].flags & BCMI_LT_FIELD_F_SET)) {
            continue;
        }

        if (entry->fields[i].flags & BCMI_LT_FIELD_F_ARRAY) {
            if (!(entry->fields[i].flags & BCMI_LT_FIELD_F_ELE_VALID)) {
                continue;
            }

            if (entry->fields[i].flags & BCMI_LT_FIELD_F_SYMBOL) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                        entry_hdl, entry->fields[i].fld_name,
                        entry->fields[i].idx,
                        &(entry->fields[i].u.sym_val), 1));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(entry_hdl,
                                                 entry->fields[i].fld_name,
                                                 entry->fields[i].idx,
                                                 &(entry->fields[i].u.val),
                                                 1));
            }
        } else {
            if (entry->fields[i].flags & BCMI_LT_FIELD_F_SYMBOL) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_add(entry_hdl,
                                                  entry->fields[i].fld_name,
                                                  entry->fields[i].u.sym_val));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl,
                                           entry->fields[i].fld_name,
                                           entry->fields[i].u.val));
            }
        }
    }

    if (entry->attr) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_attrib_set(entry_hdl, entry->attr));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get entry fields from the entry container based on handle.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  entry_hdl     Handle to the entry.
 * \param [OUT] entry         LT entry information.
 * \param [in]  get_all       Indicate to get all fields including keys.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
lt_entry_info_get(int unit, bcmlt_entry_handle_t entry_hdl,
                  bcmi_lt_entry_t *entry, int get_all)
{
    int      i   = 0;
    int      rv  = SHR_E_NONE;
    uint32_t cnt = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < entry->nfields; i++) {

        if (!(get_all || entry->fields[i].flags & BCMI_LT_FIELD_F_GET)) {
            continue;
        }

        if (entry->fields[i].flags & BCMI_LT_FIELD_F_ARRAY) {

            entry->fields[i].flags &= ~BCMI_LT_FIELD_F_ELE_VALID;

            if (entry->fields[i].flags & BCMI_LT_FIELD_F_SYMBOL) {
                rv = bcmlt_entry_field_array_symbol_get(
                        entry_hdl,
                        entry->fields[i].fld_name,
                        entry->fields[i].idx,
                        &(entry->fields[i].u.sym_val),
                        1, &cnt);

                if (SHR_SUCCESS(rv)) {
                    entry->fields[i].flags |= BCMI_LT_FIELD_F_ELE_VALID;
                }
            } else {
                rv = bcmlt_entry_field_array_get(entry_hdl,
                                                 entry->fields[i].fld_name,
                                                 entry->fields[i].idx,
                                                 &(entry->fields[i].u.val),
                                                 1, &cnt);
                if (SHR_SUCCESS(rv)) {
                    entry->fields[i].flags |= BCMI_LT_FIELD_F_ELE_VALID;
                }
            }
        } else {
            if (entry->fields[i].flags & BCMI_LT_FIELD_F_SYMBOL) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_get(
                        entry_hdl, entry->fields[i].fld_name,
                        &(entry->fields[i].u.sym_val)));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(entry_hdl,
                                           entry->fields[i].fld_name,
                                           &(entry->fields[i].u.val)));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set field information.
 *
 * \param [in]  field         LT entry field.
 * \param [in]  fld_name      LT field name.
 * \param [OUT] val           Scalar data.
 * \param [in]  sym_val       Symbol data.
 * \param [in]  idx           Indicating the index of the element in an array.
 * \param [in]  flags         Field attribute (ref LT_FIELD_F_xxx).
 *
 * \retval None.
 */
static void
lt_field_info_set(bcmi_lt_field_t *field, const char *fld_name,
                  uint64_t val, const char *sym_val,
                  uint16_t idx, uint16_t flags)
{
    field->flags = flags;

    field->fld_name = fld_name;

    field->idx = idx;

    if (flags & BCMI_LT_FIELD_F_SYMBOL) {

        field->u.sym_val = sym_val;

    } else {
        field->u.val = val;

    }

    return;
}

/******************************************************************************
* Public functions
 */

/*!
 * \brief LT Synchronous entry commit.
 *
 * SDKLT LT commit APIs have been successful if the operation had been executed
 * regardless of the actual result of the operation, and it is required to
 * validate the entry status after the function returns. This function provides
 * the encapsulation with the combination of above SDKLT operations.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry_hdl     Handle to the entry.
 * \param [in]  opcode        LT operations code.
 * \param [in]  priority      Priority.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_entry_commit(int unit, bcmlt_entry_handle_t entry_hdl,
                     bcmlt_opcode_t opcode, bcmlt_priority_level_t priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(entry_hdl, opcode, priority));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Commit an entry with LT set operation.
 *
 * If the LT entry exists, it will be updated directly. If the LT entry doesn't
 * exist, a new LT entry will be inserted.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry_hdl     Handle to the entry.
 * \param [in]  priority      Priority.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_entry_set_commit(int unit, bcmlt_entry_handle_t entry_hdl,
                         bcmlt_priority_level_t priority)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE, priority);

    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT, priority));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Transaction synchronous commit.
 *
 * SDKLT LT commit APIs have been successful if the operation had been executed
 * regardless of the actual result of the operation, and it is required to
 * validate the entry status after the function returns. This function provides
 * the encapsulation with the combination of above SDKLT operations in
 * transaction.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  trans_hdl     Handle to the transaction.
 * \param [in]  priority      Priority.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_transaction_commit(int unit, bcmlt_transaction_hdl_t trans_hdl,
                           bcmlt_priority_level_t priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_commit(trans_hdl, priority));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the definition of a specified LT field.
 *
 * This function retrieves the definition associated with the specified
 * LT field on a particular device.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tbl_name      LT name.
 * \param [in]  fld_name      LT field name.
 * \param [out] fld_def       LT field definition.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_field_def_get(int unit, const char *tbl_name,
                      const char *fld_name, bcmlt_field_def_t *fld_def)
{
    bcmlt_field_def_t *fld_defs = NULL;
    uint32_t          nflds     = 0;
    uint32_t          idx       = 0;
    int               dunit     = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fld_name, SHR_E_PARAM);
    SHR_NULL_CHECK(fld_def, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_table_field_defs_get(dunit, tbl_name, 0, NULL, &nflds));

    SHR_ALLOC
        (fld_defs, sizeof(bcmlt_field_def_t) * nflds, "field definitions");
    SHR_NULL_CHECK(fld_defs, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_table_field_defs_get(dunit, tbl_name, nflds, fld_defs, &nflds));

    for (idx = 0; idx < nflds; idx++) {
        if (sal_strcmp(fld_defs[idx].name, fld_name) == 0) {
            sal_memcpy(fld_def, &fld_defs[idx], sizeof(bcmlt_field_def_t));
            break;
        }
    }

    if (idx == nflds) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FREE(fld_defs);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the width of a specified LT field.
 *
 * This function retrieves the width(number of bits) of the specified
 * LT field on a particular device.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tbl_name      LT name.
 * \param [in]  fld_name      LT field name.
 * \param [out] width         LT field width.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_field_width_get(int unit, const char *tbl_name,
                        const char *fld_name, uint32_t *width)
{
    bcmlt_field_def_t fld_def;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(width, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, tbl_name, fld_name, &fld_def));

    *width = fld_def.width;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the range of a specified LT field value.
 *
 * This function retrieves the allowed value range of a specified LT field
 * on a particular device.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tbl_name      LT name.
 * \param [in]  fld_name      LT field name.
 * \param [out] value_min     Minimum field value allowed.
 * \param [out] value_max     Maximum field value allowed.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_field_value_range_get(int unit, const char *tbl_name,
                              const char *fld_name, uint64_t *value_min,
                              uint64_t *value_max)
{
    bcmi_lt_field_info_t fld_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_info_get(unit, tbl_name, fld_name, &fld_info));

    if (value_min) {
        *value_min = fld_info.min_limit;
    }

    if (value_max) {
        *value_max = fld_info.max_limit;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get immutable properties of a field.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tbl_name      LT name.
 * \param [in]  fld_name      LT field name.
 * \param [out] fld_info      LT field info.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_field_info_get(int unit, const char *tbl_name, const char *fld_name,
                       bcmi_lt_field_info_t *fld_info)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit;
    uint64_t val;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fld_info, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TABLE_FIELD_INFOs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, TABLE_IDs, tbl_name));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, FIELD_IDs, fld_name));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit,
                              entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, KEYs, &val));
    fld_info->key = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, SYMBOLs, &val));
    fld_info->symbol = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MIN_LIMITs, &val));
    fld_info->min_limit = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MAX_LIMITs, &val));
    fld_info->max_limit = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, ARRAY_DEPTHs, &val));
    fld_info->array_depth = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, ELEMENTSs, &val));
    fld_info->elements = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, FIELD_WIDTHs, &val));
    fld_info->field_width = val;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}


int
bcmi_lt_field_validate(int unit, const char *tbl_name, const char *fld_name)
{
    int dunit;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_table_field_id_get_by_name(dunit, tbl_name, fld_name,
                                          NULL, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear table content.
 *
 * This API clears the content of a table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tbl_name      LT name.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_clear(int unit, const char *tbl_name)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;
    int dunit = 0;
    uint64_t value = 0;
    const char *type = NULL, *map = NULL;
    bool snapshot = true;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Check the current number of entries inserted in this LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TABLE_INFOs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, TABLE_IDs, tbl_name));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit,
                              entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, ENTRY_INUSE_CNTs, &value));

    if (value == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, TYPEs, &type));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, MAPs, &map));

    if ((!sal_strcmp(type, INDEXs) || !sal_strcmp(type, INDEX_ALLOCATEs)) &&
        !sal_strcmp(map, DIRECTs)) {
        snapshot = false;
    } else {
        snapshot = true;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, tbl_name, &entry_hdl));

    if (snapshot) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_attrib_set(entry_hdl,
                                    BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT));
    }

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_CONT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL));
    }

    if (snapshot) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_attrib_clear(entry_hdl,
                                      BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_attrib_set(entry_hdl,
                                    BCMLT_ENT_ATTR_TRAVERSE_DONE));

       rv = bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_TRAVERSE,
                                 BCMLT_PRIORITY_NORMAL);
       if ((rv != SHR_E_NONE) && (rv != SHR_E_NOT_FOUND) &&
           (rv != SHR_E_UNAVAIL)) {
           SHR_ERR_EXIT(rv);
       }
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieves the capacity of a specified LT.
 *
 * This function retrieves the capacity of a specified LT.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tbl_name      LT name.
 * \param [out] entry_num     Maximum number of entries in a table.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_capacity_get(int unit, const char *tbl_name, uint32_t *entry_num)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t             value     = 0;
    int                  dunit     = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry_num, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TABLE_CONTROLs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, TABLE_IDs, tbl_name));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit,
                              entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MAX_ENTRIESs, &value));

    *entry_num = (uint32_t)value;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieves the current number of entries inserted in a specified LT.
 *
 * \param [in] unit Unit number.
 * \param [in] tbl_name LT name.
 * \param [out] cnt The number of entries inserted in a table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_lt_entry_inuse_cnt_get(int unit, const char *tbl_name, uint32_t *cnt)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t value;
    int dunit;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cnt, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TABLE_INFOs, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, TABLE_IDs, tbl_name));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit,
                              eh,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, ENTRY_INUSE_CNTs, &value));

    *cnt = (uint32_t)value;

exit:
    if (BCMLT_INVALID_HDL != eh) {
        (void) bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize LT entry struct.
 *
 * \param [in]  entry         LT entry information.
 * \param [in]  fields        Pointer to fields information array.
 *
 * \retval None.
 */
void
bcmi_lt_entry_init(bcmi_lt_entry_t *entry, bcmi_lt_field_t *fields)
{
    if (entry != NULL) {
        sal_memset(entry, 0, sizeof(bcmi_lt_entry_t));
        entry->fields = fields;
    }

    return;
}

/*!
 * \brief Set entry attributes.
 *
 * \param [in]  entry         LT entry information.
 * \param [in]  attr          Entry attributes of the entry table(ref
 * BCMLT_ENT_ATTR_F_xxx).
 *
 * \retval None.
 */
void
bcmi_lt_entry_attrib_set(bcmi_lt_entry_t *entry, uint32_t attr)
{

    entry->attr = attr;

    return;
}

/*!
 * \brief Insert an entry into LT table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       LT name.
 * \param [in]  cookie        LT entry information.
 * \param [in]  hdl_cb        User callback to set LT fields. If it's not
 * specified(NULL), cookie should be organized per \c bcmi_lt_entry_t
 * definition.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_entry_insert(int unit, const char *lt_name, void *cookie,
                     bcmi_lt_entry_handle_cb hdl_cb)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int                  dunit     = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &entry_hdl));

    if (NULL != hdl_cb) {
        SHR_IF_ERR_VERBOSE_EXIT
            (hdl_cb(unit, lt_name, entry_hdl, cookie));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (lt_entry_info_set(unit, entry_hdl, (bcmi_lt_entry_t *)cookie));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Modify the specified fields of an entry.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       LT name.
 * \param [in]  cookie        LT entry information.
 * \param [in]  hdl_cb        User callback to set LT fields. If it's not
 * specified(NULL), cookie should be organized per \c bcmi_lt_entry_t
 * definition.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_entry_update(int unit, const char *lt_name, void *cookie,
                     bcmi_lt_entry_handle_cb hdl_cb)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int                  dunit     = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmlt_entry_allocate(dunit, lt_name, &entry_hdl));

    if (NULL != hdl_cb) {
        SHR_IF_ERR_VERBOSE_EXIT
            (hdl_cb(unit, lt_name, entry_hdl, cookie));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (lt_entry_info_set(unit, entry_hdl, (bcmi_lt_entry_t *)cookie));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from LT table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       LT name.
 * \param [in]  cookie        LT entry information.
 * \param [in]  hdl_cb        User callback to set LT fields. If it's not
 * specified(NULL), cookie should be organized per \c bcmi_lt_entry_t
 * definition.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_entry_delete(int unit, const char *lt_name, void *cookie,
                     bcmi_lt_entry_handle_cb hdl_cb)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int                  dunit     = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &entry_hdl));

    if (NULL != hdl_cb) {
        SHR_IF_ERR_VERBOSE_EXIT
            (hdl_cb(unit, lt_name, entry_hdl, cookie));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (lt_entry_info_set(unit, entry_hdl, (bcmi_lt_entry_t *)cookie));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup if an entry exists or not.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       LT name.
 * \param [in]  cookie        LT entry information.
 * \param [in]  hdl_cb        User callback to set LT fields. If it's not
 * specified(NULL), cookie should be organized per \c bcmi_lt_entry_t
 * definition.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_entry_lookup(int unit, const char *lt_name, void *cookie,
                     bcmi_lt_entry_handle_cb hdl_cb)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int                  dunit     = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &entry_hdl));

    if (NULL != hdl_cb) {
        SHR_IF_ERR_VERBOSE_EXIT
            (hdl_cb(unit, lt_name, entry_hdl, cookie));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (lt_entry_info_set(unit, entry_hdl, (bcmi_lt_entry_t *)cookie));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the specified fields of an entry.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       LT name.
 * \param [out] cookie        LT entry information.
 * \param [in]  hdl_set_cb    User callback to set LT fields. If it's not
 * specified(NULL), cookie should be organized per \c bcmi_lt_entry_t
 * definition.
 * \param [in]  hdl_get_cb    User callback to get LT fields. If it's not
 * specified(NULL), cookie should be organized per \c bcmi_lt_entry_t
 * definition.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_entry_get(int unit, const char *lt_name, void *cookie,
                  bcmi_lt_entry_handle_cb hdl_set_cb,
                  bcmi_lt_entry_handle_cb hdl_get_cb)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int                  dunit     = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &entry_hdl));

    if (NULL != hdl_set_cb) {
        SHR_IF_ERR_VERBOSE_EXIT
            (hdl_set_cb(unit, lt_name, entry_hdl, cookie));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (lt_entry_info_set(unit, entry_hdl, (bcmi_lt_entry_t *)cookie));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    if (NULL != hdl_get_cb) {
        SHR_IF_ERR_VERBOSE_EXIT
            (hdl_get_cb(unit, lt_name, entry_hdl, cookie));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (lt_entry_info_get(unit, entry_hdl,
                               (bcmi_lt_entry_t *)cookie, FALSE));
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert or update an entry.
 *
 * If the LT entry exists, it will be updated directly. If the LT entry doesn't
 * exist, a new LT entry will be inserted.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       LT name.
 * \param [out] cookie        LT entry information.
 * \param [in]  hdl_cb        User callback to set LT fields. If it's not
 * specified(NULL), cookie should be organized per \c bcmi_lt_entry_t
 * definition.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_entry_set(int unit, const char *lt_name,
                  void *cookie, bcmi_lt_entry_handle_cb hdl_cb)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int                  dunit     = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &entry_hdl));

    if (NULL != hdl_cb) {
        SHR_IF_ERR_VERBOSE_EXIT
            (hdl_cb(unit, lt_name, entry_hdl, cookie));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (lt_entry_info_set(unit, entry_hdl, (bcmi_lt_entry_t *)cookie));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all the entries in an LT.
 *
 * This function goes through LT table entries and runs the user callback
 * function at each LT entry. Don't support the bcmi_lt_entry_t format so far.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       LT name.
 * \param [out] cookie        LT entry information.
 * \param [in]  hdl_cb        User callback to set LT fields/traverse entries.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_entry_traverse(int unit, const char *lt_name, void *cookie,
                       bcmi_lt_entry_handle_cb hdl_cb)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int                  rv        = SHR_E_NONE;
    int                  dunit     = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(hdl_cb, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (hdl_cb(unit, lt_name, entry_hdl, cookie));
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a field to an LT entry.
 *
 * This function is used to add a field to an entry. The field can be up to
 * 64-bits in size. Use field array for larger fields (see
 * \ref bcmi_lt_entry_field_array_add)
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [in]  data          64-bits value of the field to set.
 * \param [in]  flags         Field flags(\c BCMI_LT_FIELD_F_SET
 * or \c BCMI_LT_FIELD_F_GET).
 *
 * \retval None.
 */
void
bcmi_lt_entry_field_add(int unit, bcmi_lt_entry_t *entry, const char *fld_name,
                        uint64_t data, uint16_t flags)
{
    lt_field_info_set(&(entry->fields[entry->nfields]),
                      fld_name, data, NULL, 0, flags);
    entry->nfields++;

    LT_ENTRY_NFIELDS_CHECK(unit, entry->nfields);

    return;
}

/*!
 * \brief Get a field from an LT entry.
 *
 * This function is used to get a field value from an entry. The field
 * can be up to 64-bits in size. Use field array for larger fields (see
 * \ref bcmi_lt_entry_field_array_get)
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [out] data          64-bits value of the field to get.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_entry_field_get(int unit, bcmi_lt_entry_t *entry,
                        const char *fld_name, uint64_t *data)
{
    int i = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < entry->nfields; i++) {

        if (sal_strcmp(entry->fields[i].fld_name, fld_name) == 0) {
            *data = entry->fields[i].u.val;
            break;
        }
    }

    if (i == entry->nfields) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a symbol field to an entry.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [in]  data          Symbol of the field to set.
 * \param [in]  flags         Field flags(\c BCMI_LT_FIELD_F_SET
 * or \c BCMI_LT_FIELD_F_GET).
 *
 * \retval None.
 */
void
bcmi_lt_entry_field_symbol_add(int unit, bcmi_lt_entry_t *entry,
                               const char *fld_name, const char *data,
                               uint16_t flags)
{
    lt_field_info_set(&(entry->fields[entry->nfields]), fld_name,
                      0, data, 0, flags | BCMI_LT_FIELD_F_SYMBOL);

    entry->nfields++;

    LT_ENTRY_NFIELDS_CHECK(unit, entry->nfields);

    return;
}

/*!
 * \brief Get a symbol field to an entry.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [out] data          Symbol of the field to get.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_entry_field_symbol_get(int unit, bcmi_lt_entry_t *entry,
                               const char *fld_name, const char **data)
{
    int i = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < entry->nfields; i++) {

        if (sal_strcmp(entry->fields[i].fld_name, fld_name) == 0) {

            if (!(entry->fields[i].flags & BCMI_LT_FIELD_F_SYMBOL)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            *data = entry->fields[i].u.sym_val;
            break;
        }
    }

    if (i == entry->nfields) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a field array to an entry.
 *
 * This function is used to add a field array to an entry. Each element of the
 * field array can be up to 64-bits in size. This function can be called
 * multiple times with different \c start_idx values to construct a complete
 * array of elements. For example, if the application wants to insert two
 * elements at index 3 & 4 the application will set \c start_idx = 3 and
 * num_of_elem = 2. Note that the array index starts from index 0.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [in]  start_idx     Target array index where elements should start
 * being applied.
 * \param [in]  array         Pointer to array of 64-bits values.
 * \param [in]  size          Indicates the number of array elements.
 * \param [in]  flags         Field flags(\c BCMI_LT_FIELD_F_SET
 * or \c BCMI_LT_FIELD_F_GET).
 *
 * \retval None.
 */
void
bcmi_lt_entry_field_array_add(int unit, bcmi_lt_entry_t *entry,
                              const char *fld_name, uint16_t start_idx,
                              uint64_t *array, uint16_t size,
                              uint16_t flags)
{
    int i = 0;
    uint16_t attr = flags;

    attr |= (BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_ELE_VALID);

    for (i = 0; i < size; i++) {

        lt_field_info_set(&(entry->fields[entry->nfields]),
                          fld_name, array[i], NULL, start_idx + i, attr);

        entry->nfields++;
    }

    LT_ENTRY_NFIELDS_CHECK(unit, entry->nfields);

    return;
}

/*!
 * \brief Get a field array from an entry.
 *
 * This function is used to get a field array from an entry. Each element of the
 * field array must be up to 64 bits in size. This function can be called
 * multiple times with different \c start_idx values to construct a complete
 * array of elements. For example, if the application wants to get the values of
 * two elements in index 3 & 4 the application will set \c start_idx = 3 and
 * num_of_elem = 2. Note that the array index starts from index 0.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [in]  start_idx     Target array index where elements should start
 * being applied.
 * \param [in]  array         Pointer to array of 64-bits values.
 * \param [in]  size          Indicates the number of array elements.
 * \param [in]  r_size        Actual number of array elements that are written
 * into the data array.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_entry_field_array_get(int unit, bcmi_lt_entry_t *entry,
                              const char *fld_name, uint16_t start_idx,
                              uint64_t *array, uint16_t size,
                              uint16_t *r_size)
{
    int i = 0, j = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < entry->nfields; i++) {

        if (sal_strcmp(entry->fields[i].fld_name, fld_name) == 0) {

            if (!(entry->fields[i].flags & BCMI_LT_FIELD_F_ARRAY)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            if (!(entry->fields[i].flags & BCMI_LT_FIELD_F_ELE_VALID)) {
                continue;
            }

            if (start_idx + j == entry->fields[i].idx) {
                array[j] = entry->fields[i].u.val;
                j++;
            }

            if (j == size) {
                break;
            }
        }
    }

    if (0 == j) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (r_size) {
        *r_size = j;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a symbol array to an entry.
 *
 * This function is used to add a symbol array to an entry. This function can
 * be called multiple times with different start_idx values to construct a
 * complete array of elements. For example, if the application wants to insert
 * two elements at index 3 & 4 the application will set \c start_idx = 3 and
 * num_of_elem = 2. Note that the array index starts from index 0.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [in]  start_idx     Target array index where elements should start
 * being applied.
 * \param [in]  array         Pointer to array of symbols (strings).
 * \param [in]  size          Indicates the number of array elements.
 * \param [in]  flags         Field flags(\c BCMI_LT_FIELD_F_SET
 * or \c BCMI_LT_FIELD_F_GET).
 *
 * \retval None.
 */
void
bcmi_lt_entry_field_array_symbol_add(int unit, bcmi_lt_entry_t *entry,
                                     const char *fld_name, uint16_t start_idx,
                                     const char **array, uint16_t size,
                                     uint16_t flags)
{
    int i = 0;
    uint16_t attr = flags;

    attr |= (BCMI_LT_FIELD_F_ARRAY  |
             BCMI_LT_FIELD_F_SYMBOL |
             BCMI_LT_FIELD_F_ELE_VALID);

    for (i = 0; i < size; i++) {

        lt_field_info_set(&(entry->fields[entry->nfields]),
                          fld_name, 0, array[i], start_idx + i, attr);

        entry->nfields++;
    }

    LT_ENTRY_NFIELDS_CHECK(unit, entry->nfields);

    return;
}

/*!
 * \brief Get a symbol array from an entry.
 *
 * This function is used to get a symbol field array from an entry. This
 * function can be called multiple times with different \c start_idx values
 * to construct a complete array of elements. For example, if the application
 * wants to get the values of two elements in index 3 & 4 the application will
 * set \c start_idx = 3 and num_of_elem = 2. Note that the array index starts
 * from index 0.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [in]  start_idx     Target array index where elements should start
 * being applied.
 * \param [out] array         Pointer to array of symbols (strings).
 * \param [in]  size          Indicates the number of array elements.
 * \param [in]  r_size        Actual number of array elements that are written
 * into the data array.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_lt_entry_field_array_symbol_get(int unit, bcmi_lt_entry_t *entry,
                                     const char *fld_name, uint16_t start_idx,
                                     const char **array, uint16_t size,
                                     uint16_t *r_size)
{
    int i = 0, j = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < entry->nfields; i++) {

        if (sal_strcmp(entry->fields[i].fld_name, fld_name) == 0) {

            if (!(entry->fields[i].flags & BCMI_LT_FIELD_F_SYMBOL) ||
                !(entry->fields[i].flags & BCMI_LT_FIELD_F_ARRAY)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            if (!(entry->fields[i].flags & BCMI_LT_FIELD_F_ELE_VALID)) {
                continue;
            }

            if (start_idx + j == entry->fields[i].idx) {
                array[j] = entry->fields[i].u.sym_val;
                j++;
            }

            if (j == size) {
                break;
            }
        }
    }

    if (0 == j) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (r_size) {
        *r_size = j;
    }

exit:

    SHR_FUNC_EXIT();
}

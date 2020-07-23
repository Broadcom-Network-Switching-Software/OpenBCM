/*! \file bcmltm_ee_config.c
 *
 * Logical Table Manager Execution Engine.
 *
 * Configuration and stats.
 *
 * This file contains the LTM EE stage functions for LT table
 * statistics and configuration parameters. LT statistics are
 * counters of table operations that are not preserved over HA events.
 * LT configurations are parameters which may be modified at runtime
 * (though often only on an empty table) or statistics which _are_
 * preserved over HA events.
 *
 * These values reach into the core of the LTM data structure, and so are
 * not suitable for LTA dispatch.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>
#include <bcmltm/bcmltm_db_table_internal.h>
#include <bcmltm/bcmltm_fa_util_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_stats_internal.h>


#include <bcmltm/bcmltm_lta_intf_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_EXECUTIONENGINE

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Retrieve LT information.
 *
 * The TABLE_INFO LT provides access to the parameters of a Logical Table.
 * This function takes the target state and table info then uses the
 * TABLE_INFO value (fid, fidx) to return the requested value.
 *
 * This information is originally from the LRD, and stored in LTM
 * data structures during the metadata initialization sequence.
 *
 * Some information must be retrieved from a LTA function due to the
 * special nature of CTH implementations to provide context.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] ltid Target Logical Table ID.
 * \param [in] fid Logical Table field ID.
 * \param [in] fidx Logical Table field array index.
 * \param [in] lt_state Pointer to the state for the target LT.
 * \param [in] table_info Pointer to Logical Table information structure.
 * \param [out] val64_p pointer to return 64-bit field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL Info not available for this table,
 *                       or maintained by CTH implementation.
 */
static int
bcmltm_info_read(int unit,
                 uint32_t trans_id,
                 uint32_t ltid,
                 uint32_t fid,
                 uint32_t fidx,
                 bcmltm_lt_state_t *lt_state,
                 bcmltm_table_info_t *table_info,
                 uint64_t *val64_p)
{
    int rv;
    const bcmltd_table_entry_limit_handler_t *table_entry_limit = NULL;
    const bcmltd_table_entry_usage_handler_t *table_entry_usage = NULL;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
    case TABLE_INFOt_TYPEf:
        
        *val64_p = (table_info->type - 1);
        break;
    case TABLE_INFOt_MAPf:
        
        *val64_p = (table_info->map_type - 1);
        break;
    case TABLE_INFOt_MODELEDf:
        *val64_p = (table_info->mode == BCMLTM_TABLE_MODE_MODELED);
        break;
    case TABLE_INFOt_READ_ONLYf:
        *val64_p = table_info->read_only;
        break;
    case TABLE_INFOt_NUM_KEYSf:
        *val64_p = table_info->num_keys;
        break;
    case TABLE_INFOt_NUM_FIELDSf:
        *val64_p = table_info->num_fields;
        break;
    case TABLE_INFOt_ENTRY_INUSE_CNTf:
        /* Value or default for LTA */
        *val64_p = lt_state->entry_count;
        SHR_IF_ERR_EXIT
            (bcmltm_entry_usage_get(unit, ltid, &table_entry_usage));
        if ((table_entry_usage != NULL) &&
            (table_entry_usage->entry_inuse_get != NULL)) {
            bcmltd_table_entry_inuse_arg_t table_arg;
            bcmltd_table_entry_inuse_t table_data;

            sal_memset(&table_arg, 0, sizeof(table_arg));
            rv = table_entry_usage->entry_inuse_get(unit,
                                                    trans_id,
                                                    ltid,
                                                    &table_arg,
                                                    &table_data,
                                                    table_entry_usage->arg);
            if (rv != SHR_E_UNAVAIL) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            } /* Else discard UNAVAIL error */
            *val64_p = table_data.entry_inuse;
        }
        break;
    case TABLE_INFOt_ENTRY_MAXIMUMf:
        if ((table_info->type == BCMLTM_TABLE_TYPE_HASH) ||
            (table_info->type == BCMLTM_TABLE_TYPE_TCAM)) {
            /* No ENTRY_MAXIMUM for Keyed LTs. */
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
        *val64_p = table_info->entry_maximum;
        break;
    case TABLE_INFOt_ENTRY_LIMITf:
        /* Value or default for LTA */
        *val64_p = table_info->entry_maximum;
        SHR_IF_ERR_EXIT
            (bcmltm_entry_limit_get(unit, ltid, &table_entry_limit));
        if ((table_entry_limit != NULL) &&
            (table_entry_limit->entry_limit_get != NULL)) {
            bcmltd_table_entry_limit_arg_t table_arg;
            bcmltd_table_entry_limit_t table_data;

            sal_memset(&table_arg, 0, sizeof(table_arg));
            table_arg.entry_maximum = table_info->entry_maximum;
            rv = table_entry_limit->entry_limit_get(unit,
                                                    trans_id,
                                                    ltid,
                                                    &table_arg,
                                                    &table_data,
                                                    table_entry_limit->arg);
            if (rv != SHR_E_UNAVAIL) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            } /* Else discard UNAVAIL error */
            *val64_p = table_data.entry_limit;
        }
        break;
    case TABLE_INFOt_NUM_RESOURCE_INFOf:
        *val64_p = table_info->num_resource_info;
        break;
    case TABLE_INFOt_INSERT_OPCODEf:
        *val64_p = (table_info->opcodes & BCMLTM_OPCODE_FLAG_INSERT) != 0;
        break;
    case TABLE_INFOt_DELETE_OPCODEf:
        *val64_p = (table_info->opcodes & BCMLTM_OPCODE_FLAG_DELETE) != 0;
        break;
    case TABLE_INFOt_LOOKUP_OPCODEf:
        *val64_p = (table_info->opcodes & BCMLTM_OPCODE_FLAG_LOOKUP) != 0;
        break;
    case TABLE_INFOt_UPDATE_OPCODEf:
        *val64_p = (table_info->opcodes & BCMLTM_OPCODE_FLAG_UPDATE) != 0;
        break;
    case TABLE_INFOt_TRAVERSE_OPCODEf:
        *val64_p = (table_info->opcodes & BCMLTM_OPCODE_FLAG_TRAVERSE) != 0;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve per-field LT information.
 *
 * The TABLE_FIELD_INFO LT provides access to the parameters of each
 * field array element of a Logical Table.  This function takes the
 * target table field info structure along with the TABLE_FIELD_INFO
 * value (fid, fidx) and returns the requested value.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] ltid Target Logical Table ID.
 * \param [in] fid Logical Table field ID.
 * \param [in] fidx Logical Table field array index.
 * \param [in] field_info Pointer to LT field information structure.
 * \param [out] val64_p pointer to return 64-bit field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL Info not available for this table,
 *                       or maintained by CTH implementation.
 */
static int
bcmltm_field_info_read(int unit,
                       uint32_t trans_id,
                       uint32_t ltid,
                       uint32_t fid,
                       uint32_t fidx,
                       bcmltm_table_field_info_t *field_info,
                       uint64_t *val64_p)
{
    SHR_FUNC_ENTER(unit);

    switch (fid) {
    case TABLE_FIELD_INFOt_SYMBOLf:
        *val64_p = field_info->symbol;
        break;
    case TABLE_FIELD_INFOt_MIN_LIMITf:
        *val64_p = field_info->min_limit;
        break;
    case TABLE_FIELD_INFOt_MAX_LIMITf:
        *val64_p = field_info->max_limit;
        
        if ((field_info->key != 0) &&
            (table_is_itile_index(unit, ltid))) {
            int rv;
            const bcmltd_table_entry_limit_handler_t *table_entry_limit = NULL;

            SHR_IF_ERR_EXIT
                (bcmltm_entry_limit_get(unit, ltid, &table_entry_limit));
            if ((table_entry_limit != NULL) &&
                (table_entry_limit->entry_limit_get != NULL)) {
                bcmltd_table_entry_limit_arg_t table_arg;
                bcmltd_table_entry_limit_t table_data;

                sal_memset(&table_arg, 0, sizeof(table_arg));
                table_arg.entry_maximum = field_info->max_limit + 1;
                rv = table_entry_limit->entry_limit_get(unit,
                                                        trans_id,
                                                        ltid,
                                                        &table_arg,
                                                        &table_data,
                                                        table_entry_limit->arg);
                if (rv != SHR_E_UNAVAIL) {
                    SHR_IF_ERR_VERBOSE_EXIT(rv);
                } /* Else discard UNAVAIL error */
                if (table_data.entry_limit > 0) {
                    *val64_p = table_data.entry_limit - 1;
                } else {
                    *val64_p = 0;
                }
            }
        }
        break;
    case TABLE_FIELD_INFOt_DEFAULTf:
        if (field_info->symbol) {
            /* This will prevent the value from returning during LOOKUP. */
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
        *val64_p = field_info->default_value;
        break;
    case TABLE_FIELD_INFOt_SYMBOL_DEFAULTf:
        if (!field_info->symbol) {
            /* This will prevent the value from returning during LOOKUP. */
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
        *val64_p = field_info->default_value;
        break;
    case TABLE_FIELD_INFOt_KEYf:
        *val64_p = field_info->key != 0;
        break;
    case TABLE_FIELD_INFOt_INDEX_ALLOC_KEY_FIELDf:
        *val64_p = field_info->alloc_key_field;
        break;
    case TABLE_FIELD_INFOt_READ_ONLYf:
        *val64_p = field_info->read_only;
        break;
    case TABLE_FIELD_INFOt_ARRAY_DEPTHf:
        *val64_p = field_info->array_depth;
        break;
    case TABLE_FIELD_INFOt_ELEMENTSf:
        *val64_p = field_info->elements;
        break;
    case TABLE_FIELD_INFOt_FIELD_WIDTHf:
        *val64_p = field_info->width;
        break;
    case TABLE_FIELD_INFOt_SELECTORf:
        *val64_p = field_info->selector;
        break;
    case TABLE_FIELD_INFOt_NUM_GROUPf:
        *val64_p = field_info->num_select_group;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve per-field selection information of a LT.
 *
 * The TABLE_SELECT_INFO LT provides access to the selection overlays
 * of each field of a Logical Table.  This function takes the
 * target field selection structure along with the TABLE_FIELD_INFO
 * value (fid, fidx) and returns the requested value.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Logical Table field ID.
 * \param [in] fidx Logical Table field array index.
 * \param [in] field_select Pointer to LT field select structure.
 * \param [out] val64_p pointer to return 64-bit field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL Info not available for this table.
 */
static int
bcmltm_field_select_read(int unit,
                         uint32_t fid,
                         uint32_t fidx,
                         bcmltm_table_field_select_t *field_select,
                         uint64_t *val64_p)
{
    SHR_FUNC_ENTER(unit);

    switch (fid) {
    case TABLE_FIELD_SELECTt_SELECTOR_FIELD_IDf:
        *val64_p = field_select->selector_gfid;
        break;
    case TABLE_FIELD_SELECTt_SCALARf:
        *val64_p = field_select->selector_scalar;
        break;
    case TABLE_FIELD_SELECTt_SCALAR_VALUEf:
        *val64_p = field_select->selector_scalar_value;
        break;
    case TABLE_FIELD_SELECTt_ENUM_VALUEf:
        *val64_p = field_select->selector_enum_value;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve per-field selection information of a LT.
 *
 * The TABLE_SELECT_INFO LT provides access to the selection overlays
 * of each field of a Logical Table.  This function takes the
 * target field selection structure along with the TABLE_FIELD_INFO
 * value (fid, fidx) and returns the requested value.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Target Logical Table ID.
 * \param [in] fid Logical Table field ID.
 * \param [in] fidx Logical Table field array index.
 * \param [in] rid Resource info index.
 * \param [out] val64_p pointer to return 64-bit field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL Info not available for this table.
 */
static int
bcmltm_resource_info_read(int unit,
                          uint32_t ltid,
                          uint32_t fid,
                          uint32_t fidx,
                          uint32_t rid,
                          uint64_t *val64_p)
{
    bcmltm_table_resource_info_t info;
    SHR_FUNC_ENTER(unit);

    switch (fid) {
    case TABLE_RESOURCE_INFOt_RESOURCE_INFO_TABLE_IDf:
         SHR_IF_ERR_EXIT
             (bcmltm_db_table_resource_info_get(unit, ltid, rid, &info));
        *val64_p = info.ri_sid;
        break;
   default:
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the value of a LT_STATS field.
 *
 * Internal function to recover a per-LT statistic value.
 *
 * \param [in] lt_stats Pointer to the statistics array for this LT.
 * \param [in] field_id The ID number for this LT_CONFIG field.
 * \param [in] field_idx The array index for this LT_CONFIG field.
 * \param [in] data_value The uint64_t pointer to return the field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL Info not available for this table.
 */
static int
bcmltm_stats_read(uint32_t *lt_stats,
                  uint32_t field_id,
                  uint32_t field_idx,
                  uint64_t *data_value)
{
    if (field_id < BCMLRD_FIELD_STATS_NUM) {
        *data_value = lt_stats[field_id];
    } else {
        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Assign the value of a LT_STATS field.
 *
 * Internal function to set a per-LT statistic value.
 *
 * \param [in] lt_stats Pointer to the statistics array for this LT.
 * \param [in] field_id The ID number for this LT_CONFIG field.
 * \param [in] field_idx The array index for this LT_CONFIG field.
 * \param [in] unset Set field to default.
 * \param [in] data_value The uint64_t pointer to provide the field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL Info not available for this table.
 */
static int
bcmltm_stats_write(uint32_t *lt_stats,
                   uint32_t field_id,
                   uint32_t field_idx,
                   bool unset,
                   uint64_t data_value)
{
    if (field_id < BCMLRD_FIELD_STATS_NUM) {
        lt_stats[field_id]= unset ? 0 : data_value & 0xffffffff;
    } else {
        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Set the value of a TABLE_CONTROL field.
 *
 * Internal function to set the value of a per-LT
 * configuration in the HA-protected storage location.  Also
 * implements the configuration change.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] ltid Target Logical Table ID.
 * \param [in] lt_state Pointer to the state for the target LT.
 * \param [in] field_id The ID number for this TABLE_CONTROL field.
 * \param [in] field_idx The array index for this TABLE_CONTROL field.
 * \param [in] unset Set field to default.
 * \param [in] data_value The uint64_t value for this control.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL Control not available for this table,
 *                       or maintained by CTH implementation.
 * \retval SHR_E_PARAM New entry max exceeds entry limit.
 */
static int
bcmltm_control_write(int unit,
                     uint32_t trans_id,
                     uint32_t ltid,
                     bcmltm_lt_state_t *lt_state,
                     bcmltm_table_info_t *table_info,
                     uint32_t field_id,
                     uint32_t field_idx,
                     bool unset,
                     uint64_t data_value)
{
    uint64_t entry_limit, inuse_count;
    const char *table_name;
    const bcmltd_table_entry_usage_handler_t *table_entry_usage = NULL;

    SHR_FUNC_ENTER(unit);

    switch (field_id) {
    case TABLE_CONTROLt_MAX_ENTRIESf:
        /* Check new value does not exceed table maximum */
        SHR_IF_ERR_EXIT
            (bcmltm_info_read(unit, trans_id, ltid,
                              TABLE_INFOt_ENTRY_LIMITf, 0,
                              lt_state, table_info,
                              &entry_limit));
        if (unset) {
            data_value = entry_limit;
        }
        if (data_value > entry_limit) {
            table_name = bcmltm_lt_table_sid_to_name(unit, ltid);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "TABLE_CONTROL[%s(ltid=%d)]: "
                                    "new maximum %d exceeds limit %d\n"),
                         table_name, ltid,
                         (uint32_t)data_value, (uint32_t)entry_limit));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        /* Check new value is not less than current in-use count. */
        SHR_IF_ERR_EXIT
            (bcmltm_info_read(unit, trans_id, ltid,
                              TABLE_INFOt_ENTRY_INUSE_CNTf, 0,
                              lt_state, table_info,
                              &inuse_count));
        if (data_value < inuse_count) {
            table_name = bcmltm_lt_table_sid_to_name(unit, ltid);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "TABLE_CONTROL[%s(ltid=%d)]: "
                                    "new maximum %d less than existing %d\n"),
                         table_name, ltid,
                         (uint32_t)data_value, (uint32_t)inuse_count));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        lt_state->max_entry_count = data_value;
        /* Notify LTA of max entry change if necessary */
        SHR_IF_ERR_EXIT
            (bcmltm_entry_usage_get(unit, ltid, &table_entry_usage));
        if ((table_entry_usage != NULL) &&
            (table_entry_usage->max_entries_set != NULL)) {
            bcmltd_table_max_entries_arg_t table_arg;
            bcmltd_table_max_entries_t table_data;

            sal_memset(&table_arg, 0, sizeof(table_arg));
            sal_memset(&table_data, 0, sizeof(table_data));
            table_data.max_entries = data_value;
            SHR_IF_ERR_VERBOSE_EXIT
                (table_entry_usage->max_entries_set(unit,
                                                    trans_id,
                                                    ltid,
                                                    &table_arg,
                                                    &table_data,
                                                    table_entry_usage->arg));
            break;
        }
        break;
    case TABLE_CONTROLt_TABLE_OP_PT_INFOf:
        if (unset) {
            data_value = 0;
        }
        lt_state->table_op_pt_info_enable =
            (data_value != 0) ? TRUE : FALSE;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the value of a TABLE_CONTROL field.
 *
 * Internal function to recover the current value of a per-LT
 * control value for the HA-protected storage location.
 *
 * \param [in] lt_state Pointer to the metadata for the target LT.
 * \param [in] field_id The ID number for this TABLE_CONTROL field.
 * \param [in] field_idx The array index for this TABLE_CONTROL field.
 * \param [in] data_value The uint64_t pointer to return the field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL Control not available for this table.
 */
static int
bcmltm_control_read(bcmltm_lt_state_t *lt_state,
                    uint32_t field_id,
                    uint32_t field_idx,
                    uint64_t *data_value)
{
    switch (field_id) {
    case TABLE_CONTROLt_MAX_ENTRIESf:
        *data_value = lt_state->max_entry_count;
        break;
    case TABLE_CONTROLt_TABLE_OP_PT_INFOf:
        *data_value = lt_state->table_op_pt_info_enable;
        break;
    default:
        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_ee_node_lt_table_write(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie)
{
    int rv = SHR_E_NONE;
    uint32_t fs_ix;
    bcmltm_field_list_t *api_field_data;
    bcmltm_field_spec_list_t *fs_list;
    bcmltm_field_spec_t *cur_fs;
    uint32_t wb_offset;
    uint32_t target_ltid;
    bcmltm_lt_state_t *target_lt_state = NULL;
    uint32_t *target_lt_stats = NULL;
    bcmltm_table_info_t table_info;

    SHR_FUNC_ENTER(unit);

    fs_list = BCMLTM_EE_FIELD_SPEC_LIST(node_cookie);
    wb_offset = fs_list->key_offset[BCMLTM_TABLE_KEYS_LTID];
    if (wb_offset == BCMLTM_WB_OFFSET_INVALID) {
        /* The LTID offset must exist for these tables. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    target_ltid = ltm_buffer[wb_offset];

    if (lt_entry->table_id == TABLE_STATSt) {
        /* LT stats */
        SHR_IF_ERR_EXIT
            (bcmltm_stats_lt_get(unit, target_ltid, &target_lt_stats));
    } else {
        /* Dynamic LT state */
        SHR_IF_ERR_EXIT
            (bcmltm_state_lt_get(unit, target_ltid, &target_lt_state));
    }

    if (lt_entry->table_id == TABLE_CONTROLt) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmltm_db_table_info_get(unit, target_ltid, &table_info));
    }

    for (fs_ix = 0; fs_ix < fs_list->num_field_specs; fs_ix++) {
        cur_fs = &(fs_list->field_specs[fs_ix]);

        api_field_data = bcmltm_api_find_field(lt_entry->in_fields,
                                               cur_fs->field_id,
                                               cur_fs->field_idx);
        if (api_field_data == NULL) {  /* Value fields are optional */
            continue;  /* No configuration change required */
        }

        switch (lt_entry->table_id) {
        case TABLE_CONTROLt:
            rv = bcmltm_control_write(unit,
                                      lt_entry->trans_id,
                                      target_ltid,
                                      target_lt_state,
                                      &table_info,
                                      cur_fs->field_id,
                                      cur_fs->field_idx,
                         ((api_field_data->flags & SHR_FMM_FIELD_DEL) != 0),
                                      api_field_data->data);
            break;
        case TABLE_STATSt:
            rv = bcmltm_stats_write(target_lt_stats,
                                    cur_fs->field_id,
                                    cur_fs->field_idx,
                         ((api_field_data->flags & SHR_FMM_FIELD_DEL) != 0),
                                    api_field_data->data);
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        if (rv == SHR_E_UNAVAIL) {
            /* This configuration is not implemented for this LT. Skip. */
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_lt_table_read(int unit,
                             bcmltm_lt_md_t *lt_md,
                             bcmltm_lt_state_t *lt_state,
                             bcmltm_entry_t *lt_entry,
                             uint32_t *ltm_buffer,
                             void *node_cookie)
{
    int rv = SHR_E_NONE;
    uint64_t val64 = 0;
    uint32_t fs_ix;
    bcmltm_field_list_t *api_field_data, *last_out_field;
    bcmltm_field_spec_list_t *fs_list;
    bcmltm_field_spec_t *cur_fs;
    uint32_t wb_offset;
    uint32_t target_ltid, target_gfid, target_fid;
    uint32_t target_group;
    uint32_t target_rid = 0;
    bcmltm_lt_state_t *target_lt_state = NULL;
    uint32_t *target_lt_stats = NULL;
    bcmltm_table_info_t table_info;
    bcmltm_table_field_info_t field_info;
    bcmltm_table_field_select_t field_select;

    SHR_FUNC_ENTER(unit);

    fs_list = BCMLTM_EE_FIELD_SPEC_LIST(node_cookie);
    wb_offset = fs_list->key_offset[BCMLTM_TABLE_KEYS_LTID];
    if (wb_offset == BCMLTM_WB_OFFSET_INVALID) {
        /* The LTID offset must exist for these tables. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    target_ltid = ltm_buffer[wb_offset];

    if (lt_entry->table_id == TABLE_STATSt) {
        /* LT stats */
        SHR_IF_ERR_EXIT
            (bcmltm_stats_lt_get(unit, target_ltid, &target_lt_stats));
    } else {
        /* Dynamic LT state */
        SHR_IF_ERR_EXIT
            (bcmltm_state_lt_get(unit, target_ltid, &target_lt_state));
    }

    if (lt_entry->table_id == TABLE_INFOt) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmltm_db_table_info_get(unit, target_ltid, &table_info));
    }

    if ((lt_entry->table_id == TABLE_FIELD_INFOt) ||
        (lt_entry->table_id == TABLE_FIELD_SELECTt)) {
        wb_offset = fs_list->key_offset[BCMLTM_TABLE_KEYS_LT_FIELD_ID];
        if (wb_offset == BCMLTM_WB_OFFSET_INVALID) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        target_gfid = ltm_buffer[wb_offset];

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmltm_db_gfid_to_fid(unit, target_ltid, target_gfid, &target_fid));

        if (lt_entry->table_id == TABLE_FIELD_INFOt) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmltm_db_table_field_info_get_by_gfid(unit, target_ltid,
                                                        target_gfid,
                                                        &field_info));

        }

        if (lt_entry->table_id == TABLE_FIELD_SELECTt) {
            wb_offset = fs_list->key_offset[BCMLTM_TABLE_KEYS_SELECT_GROUP];
            if (wb_offset == BCMLTM_WB_OFFSET_INVALID) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            target_group = ltm_buffer[wb_offset];

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmltm_db_table_field_select_get_by_gfid(unit, target_ltid,
                                                          target_gfid,
                                                          target_group,
                                                          &field_select));
        }
    }

    if (lt_entry->table_id == TABLE_RESOURCE_INFOt) {
        wb_offset = fs_list->key_offset[BCMLTM_TABLE_KEYS_RESOURCE_INFO];
        if (wb_offset == BCMLTM_WB_OFFSET_INVALID) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        target_rid = ltm_buffer[wb_offset];
    }

    last_out_field = NULL;

    for (fs_ix = 0; fs_ix < fs_list->num_field_specs; fs_ix++) {
        cur_fs = &(fs_list->field_specs[fs_ix]);

        switch (lt_entry->table_id) {
        case TABLE_INFOt:
            rv = bcmltm_info_read(unit,
                                  lt_entry->trans_id,
                                  target_ltid,
                                  cur_fs->field_id,
                                  cur_fs->field_idx,
                                  target_lt_state,
                                  &table_info,
                                  &val64);
            break;
        case TABLE_CONTROLt:
            rv = bcmltm_control_read(target_lt_state,
                                     cur_fs->field_id,
                                     cur_fs->field_idx,
                                     &val64);
            break;
        case TABLE_STATSt:
            rv = bcmltm_stats_read(target_lt_stats, cur_fs->field_id,
                                   cur_fs->field_idx, &val64);
            break;
        case TABLE_FIELD_INFOt:
            rv = bcmltm_field_info_read(unit,
                                        lt_entry->trans_id,
                                        target_ltid,
                                        cur_fs->field_id,
                                        cur_fs->field_idx,
                                        &field_info,
                                        &val64);
            break;
        case TABLE_FIELD_SELECTt:
            rv = bcmltm_field_select_read(unit,
                                          cur_fs->field_id,
                                          cur_fs->field_idx,
                                          &field_select,
                                          &val64);
            break;
        case TABLE_RESOURCE_INFOt:
            rv = bcmltm_resource_info_read(unit,
                                           target_ltid,
                                           cur_fs->field_id,
                                           cur_fs->field_idx,
                                           target_rid,
                                           &val64);
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        if (rv == SHR_E_UNAVAIL) {
            /* This configuration is not implemented for this LT. Skip. */
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        api_field_data = (lt_entry->field_alloc_cb)();
        if (api_field_data == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        /* Add retrieved field value to entry */
        api_field_data->id = cur_fs->field_id;
        api_field_data->idx = cur_fs->field_idx;
        api_field_data->data = val64;
        api_field_data->flags = 0;
        api_field_data->next = NULL;
        if (last_out_field == NULL) {
            lt_entry->out_fields = api_field_data;
        } else {
            last_out_field->next = api_field_data;
        }
        last_out_field = api_field_data;
    }

 exit:
    SHR_FUNC_EXIT();
}

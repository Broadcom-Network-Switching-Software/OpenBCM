/**
 * \file dbal_journal.c
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * INFO: This module is one of the components needed for Error Recovery,
 * it is a rollback journal, saving the old values that were overridden
 * by dbal access calls since the beginning of the last transaction.
 */

#include <shared/bsl.h>
#include <sal/core/sync.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include <soc/dnx/dbal/dbal_dynamic.h>
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <soc/dnx/dnx_er_threading.h>
#include <shared/utilex/utilex_bitstream.h>

#ifdef INCLUDE_XFLOW_MACSEC
#include <dnx/dnx_sec.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>
#endif

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

static dnx_dbal_journal_manager_t
    dnx_dbal_journal_manager[SOC_MAX_NUM_DEVICES][DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION] = { {{0}} };

#define DNX_DBAL_ROLLBACK_JOURNAL_NUM 0
#define DNX_DBAL_COMPARISON_JOURNAL_NUM 1

#define DNX_DBAL_ROLLBACK_JOURNAL_TRANSACTION dnx_dbal_journal_manager[unit][er_thread_id].journals[DNX_DBAL_ROLLBACK_JOURNAL_NUM].transaction
#define DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION dnx_dbal_journal_manager[unit][er_thread_id].journals[DNX_DBAL_COMPARISON_JOURNAL_NUM].transaction

#define DNX_DBAL_ROLLBACK_JOURNAL_HANDLE dnx_dbal_journal_manager[unit][0].journals[DNX_DBAL_ROLLBACK_JOURNAL_NUM].handle
#define DNX_DBAL_COMPARISON_JOURNAL_HANDLE dnx_dbal_journal_manager[unit][0].journals[DNX_DBAL_COMPARISON_JOURNAL_NUM].handle

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING

#define DNX_DBAL_JOURNAL_ACCESS_NOTIFY(_unit, _er_thread_id, _table_ptr)                                                                       \
    do {                                                                                                                                       \
        if(NULL != dnx_dbal_journal_manager[unit][er_thread_id].journals[DNX_DBAL_COMPARISON_JOURNAL_NUM].access_cb) {                         \
            dnx_dbal_journal_manager[unit][er_thread_id].journals[DNX_DBAL_COMPARISON_JOURNAL_NUM].access_cb(_unit, _er_thread_id, _table_ptr);\
        }                                                                                                                                      \
        if(NULL != dnx_dbal_journal_manager[unit][er_thread_id].journals[DNX_DBAL_ROLLBACK_JOURNAL_NUM].access_cb) {                           \
            dnx_dbal_journal_manager[unit][er_thread_id].journals[DNX_DBAL_ROLLBACK_JOURNAL_NUM].access_cb(_unit, _er_thread_id, _table_ptr);  \
        }                                                                                                                                      \
    } while(0)
#else
#define DNX_DBAL_JOURNAL_ACCESS_NOTIFY(_unit, _er_thread_id, _table_ptr)
#endif

static uint8
dnx_dbal_journal_is_table_excluded(
    int unit,
    int er_thread_id,
    dbal_tables_e table_id,
    uint8 is_comparison)
{

    dbal_tables_e comparison_perm_excluded[] = {

        DBAL_TABLE_BFD_GENERAL_CONFIGURATION,
        DBAL_TABLE_INGRESS_OAM_ACC_MEP_ACTION_DB,
        DBAL_TABLE_OAMP_INIT_GENERAL_CFG,
        DBAL_TABLE_OAMP_BFD_CONFIGURATION,
        DBAL_TABLE_PEMLA_PARSERUDP,
        DBAL_TABLE_OAMP_PE_PROGRAMS_PROPERTIES,

        DBAL_TABLE_OAMP_MEP_DB,
        DBAL_TABLE_OAMP_MEP_PROFILE,

        DBAL_TABLE_SNIF_NEW_SYSTEM_HEADER_TABLE,
        DBAL_TABLE_IPQ_SYSTEM_PORT_TO_QUEUE_BASE_MAP,
        DBAL_TABLE_LATENCY_VOQ_QUARTET_PROFILE_MAP,
        DBAL_TABLE_IPQ_TRAFFIC_CLASS_TO_VOQ_OFFSET_MAP,
        DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_WRED_DROP,

        DBAL_TABLE_L3_VRRP_TCAM,

        DBAL_TABLE_FWD_MACT,
        DBAL_TABLE_NIF_ETHU_POWER_DOWN,

        DBAL_TABLE_TINY_MAC_RX_LSS_CTRL,
        DBAL_TABLE_NIF_FRAMER_TINY_MAC_CTRL,
        DBAL_TABLE_NIF_ILU_HRF_RX_CTRL,
        DBAL_TABLE_NIF_ILU_RX_HRF_COUNTER,

        DBAL_TABLE_FABRIC_MESH_DEV_ID_DEST,
        DBAL_TABLE_FABRIC_MESH_DEV_ID_DEST_INIT,

        DBAL_TABLE_FABRIC_MESH_DEV_ID_LOCAL,
        DBAL_TABLE_FABRIC_MESH_DEV_ID_LOCAL_INIT,

        DBAL_TABLE_EXAMPLE_IN_LIF_FORMAT_DIFFERENT_SIZES,

        DBAL_TABLE_METER_PROFILE_SET_FOR_SMALL_ENGINE,
        DBAL_TABLE_METER_DATABASE_FOR_SMALL_ENGINE,
        DBAL_TABLE_METER_GLOBAL_SHARING_FLAG_SMALL_ENGINES,
        DBAL_TABLE_METER_PROFILE_SET_FOR_BIG_ENGINE,
        DBAL_TABLE_METER_GLOBAL_SHARING_FLAG_BIG_ENGINES,
        DBAL_TABLE_METER_DATABASE_FOR_BIG_ENGINE,
        DBAL_TABLE_EMPTY
    };

    dbal_tables_e rollback_perm_excluded[] = {
        DBAL_TABLE_EMPTY
    };

    int ii = 0;
    int nof_tables = 0;

    if (!is_comparison)
    {

        for (ii = 0; ii < DNX_DBAL_ROLLBACK_JOURNAL_TRANSACTION.excluded_tables_count; ii++)
        {
            if (table_id == DNX_DBAL_ROLLBACK_JOURNAL_TRANSACTION.excluded_tables[ii])
            {
                return TRUE;
            }
        }

        nof_tables = sizeof(rollback_perm_excluded) / sizeof(dbal_tables_e);

        for (ii = 0; ii < nof_tables; ii++)
        {
            if (rollback_perm_excluded[ii] == table_id)
            {
                return TRUE;
            }
        }
    }
    else
    {

        for (ii = 0; ii < DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION.excluded_tables_count; ii++)
        {
            if (table_id == DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION.excluded_tables[ii])
            {
                return TRUE;
            }
        }

        nof_tables = sizeof(comparison_perm_excluded) / sizeof(dbal_tables_e);

        for (ii = 0; ii < nof_tables; ii++)
        {
            if (comparison_perm_excluded[ii] == table_id)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}

static shr_error_e
dnx_dbal_journal_entry_print_cb(
    int unit,
    uint8 *metadata,
    uint8 *payload)
{
    dnx_dbal_journal_entry_t *ptr_entry = NULL;
    dbal_entry_handle_t *ptr_payload = NULL;

    SHR_FUNC_INIT_VARS(unit);;

    ptr_entry = (dnx_dbal_journal_entry_t *) metadata;
    ptr_payload = (dbal_entry_handle_t *) payload;

    if (dnx_err_recovery_common_is_validation_enabled(unit))
    {
        if (NULL == ptr_entry || NULL == ptr_payload)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                        "dbal journal ERROR: null entry found in journal.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
    }

    LOG_CLI((BSL_META("********************************* DBAL Journal *********************************\n")));

    switch (ptr_entry->action)
    {
        case DBAL_ACTION_ENTRY_CLEAR:
            LOG_CLI((BSL_META("Type: CLEAR\n")));
            break;
        case DBAL_ACTION_ENTRY_COMMIT:
            LOG_CLI((BSL_META("Type: COMMIT\n")));
            break;
        default:
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "dbal journal ERROR: unrecognized entry type.\n%s%s%s", EMPTY,
                                     EMPTY, EMPTY);
    }

    LOG_CLI((BSL_META("Table name: %s\n"), ptr_payload->table->table_name));

    LOG_CLI((BSL_META("Data associated with DBAL entry:\n")));

    SHR_IF_ERR_EXIT(dbal_entry_print(unit, ptr_payload, TRUE));

    LOG_CLI((BSL_META("********************************* DBAL Journal End *****************************\n\n")));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_dbal_journal_rollback_common(
    int unit,
    int er_thread_id,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dnx_dbal_journal_entry_t *ptr_entry = NULL;
    dbal_entry_handle_t *ptr_payload = NULL;

    SHR_FUNC_INIT_VARS(unit);

    ptr_entry = (dnx_dbal_journal_entry_t *) metadata;
    ptr_payload = (dbal_entry_handle_t *) payload;

    if (dnx_err_recovery_common_is_validation_enabled(unit))
    {
        if (NULL == ptr_entry || NULL == ptr_payload)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                        "dbal journal ERROR: null entry found in journal.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
    }

    ptr_payload->er_flags = 0x0;
    switch (ptr_entry->action)
    {
        case DBAL_ACTION_ENTRY_COMMIT:
            SHR_IF_ERR_EXIT(dbal_actions_access_entry_commit(unit, ptr_payload, NULL));
            break;
        case DBAL_ACTION_ENTRY_CLEAR:
            SHR_IF_ERR_EXIT(dbal_actions_access_entry_clear(unit, ptr_payload));
            break;
        default:
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "dbal journal ERROR: unrecognized entry type.\n%s%s%s", EMPTY,
                                     EMPTY, EMPTY);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_dbal_journal_handle_trigger_value_mask(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * get_entry_handle,
    uint8 *ptr_compare_payload)
{
    multi_res_info_t result_set;
    int result_type_id;
    int result_id;
    uint8 contains_comparable_fields = FALSE;
    dbal_table_field_info_t field_info;
    dbal_logical_table_t *table = NULL;
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);;

    if (NULL == get_entry_handle)
    {
        SHR_EXIT();
    }

    table = entry_handle->table;

    for (result_type_id = 0; result_type_id < table->nof_result_types; result_type_id++)
    {
        result_set = table->multi_res_info[result_type_id];
        for (result_id = 0; result_id < result_set.nof_result_fields; result_id++)
        {
            field_info = result_set.results_info[result_id];

            if (DBAL_PERMISSION_TRIGGER == field_info.permission || DBAL_PERMISSION_READONLY == field_info.permission)
            {

                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_val,
                                                               field_info.bits_offset_in_buffer,
                                                               field_info.field_nof_bits,
                                                               &(entry_handle->phy_entry.payload[0])));

                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_val,
                                                               field_info.bits_offset_in_buffer,
                                                               field_info.field_nof_bits,
                                                               &(get_entry_handle->phy_entry.payload[0])));
            }

            if (DBAL_PERMISSION_TRIGGER != field_info.permission
                && DBAL_PERMISSION_READONLY != field_info.permission
                && DBAL_PERMISSION_WRITEONLY != field_info.permission)
            {
                contains_comparable_fields = TRUE;
            }
        }
    }

    *ptr_compare_payload = contains_comparable_fields;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#ifdef INCLUDE_XFLOW_MACSEC
static void
dbal_journal_is_labal_in_table_get(
    int unit,
    CONST dbal_logical_table_t * table,
    dbal_labels_e label,
    uint32 *is_labal_in_table)
{
    int label_idx;

    *is_labal_in_table = 0;

    for (label_idx = 0; label_idx < table->nof_labels; ++label_idx)
    {
        if (label == table->table_labels[label_idx])
        {
            *is_labal_in_table = 1;
            break;
        }
    }
}
#endif

static shr_error_e
dnx_dbal_comparison_journal_roll_back_entry_cb(
    int unit,
    int er_thread_id,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    int rv = _SHR_E_NONE;
    int hl_entry_is_default = 0;
    uint8 entry_not_found = FALSE;
    uint8 is_journaled_entry_default = FALSE;
    uint8 entries_match = TRUE;
    uint8 is_compare_payload = TRUE;
    dnx_dbal_journal_entry_t *ptr_entry = NULL;
    dbal_entry_handle_t *ptr_payload = NULL;
    dbal_entry_handle_t *get_entry_handle = NULL;
    dbal_logical_table_t *table = NULL;

    SHR_FUNC_INIT_VARS(unit);

    ptr_entry = (dnx_dbal_journal_entry_t *) metadata;
    ptr_payload = (dbal_entry_handle_t *) payload;

    if (dnx_err_recovery_common_is_validation_enabled(unit))
    {
        if (NULL == ptr_entry || NULL == ptr_payload)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                        "dbal journal ERROR: null entry found in journal.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
    }

    ptr_payload->er_flags = 0x0;

    SHR_ALLOC_SET_ZERO(get_entry_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);

#ifdef INCLUDE_XFLOW_MACSEC
    if (dnx_data_macsec.general.feature_get(unit, dnx_data_macsec_general_macsec_block_exists))
    {
        uint32 is_macsec_table;
        dbal_journal_is_labal_in_table_get(unit, ptr_payload->table, DBAL_LABEL_MACSEC_IP, &is_macsec_table);
        if (is_macsec_table)
        {

            SHR_IF_ERR_EXIT(dnx_xflow_macsec_control_power_down_force_disable_set(unit));
        }
    }
#endif

    if (ptr_entry->action != DBAL_JOURNAL_ACTION_TABLE_CREATE && ptr_entry->action != DBAL_JOURNAL_ACTION_TABLE_DESTROY)
    {
        rv = dbal_actions_access_entry_get(unit, ptr_payload, get_entry_handle);

#ifdef INCLUDE_XFLOW_MACSEC
        if (dnx_data_macsec.general.feature_get(unit, dnx_data_macsec_general_macsec_block_exists))
        {
            uint32 is_macsec_table;
            dbal_journal_is_labal_in_table_get(unit, ptr_payload->table, DBAL_LABEL_MACSEC_IP, &is_macsec_table);
            if (is_macsec_table)
            {
                int local_rc = _SHR_E_NONE;

                local_rc = dnx_xflow_macsec_control_power_down_force_disable_restore(unit);
                if (SHR_FAILURE(local_rc))
                {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U
                               (unit,
                                "Error in dbal comparison journal. table %s. Error %d during restore power down\n"),
                               ptr_payload->table->table_name, local_rc));
                    if (!SHR_FUNC_ERR())
                    {
                        SHR_SET_CURRENT_ERR(local_rc);
                    }
                }
            }
        }
#endif

        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

        SHR_IF_ERR_EXIT(dnx_dbal_journal_handle_trigger_value_mask
                        (unit, ptr_payload, get_entry_handle, &is_compare_payload));

        entry_not_found = (_SHR_E_NOT_FOUND == rv) || (NULL == get_entry_handle);

        if (!entry_not_found && !dbal_tables_is_non_direct(unit, ptr_payload->table_id))
        {
            SHR_IF_ERR_EXIT(dbal_entry_direct_is_default_internal(unit, get_entry_handle, &hl_entry_is_default));
            entry_not_found = (hl_entry_is_default != 0);

            SHR_IF_ERR_EXIT(dbal_entry_direct_is_default_internal(unit, ptr_payload, &hl_entry_is_default));
            is_journaled_entry_default = (hl_entry_is_default != 0);
        }
    }

    table = ptr_payload->table;

    if (DBAL_ACTION_ENTRY_CLEAR == ptr_entry->action)
    {
        if (!entry_not_found
            && !(table->table_type == DBAL_TABLE_TYPE_TCAM && table->access_method == DBAL_ACCESS_METHOD_MDB))
        {
            LOG_CLI((BSL_META("\n################################################################################\n")));
            LOG_CLI((BSL_META("DBAL entry found when it should not exist for table stamp: %s, core id: %d\n"),
                     table->table_name, (get_entry_handle->core_id)));
            LOG_CLI((BSL_META("Data associated with DBAL entry:\n")));
            SHR_IF_ERR_EXIT(dbal_entry_print(unit, get_entry_handle, TRUE));
            LOG_CLI((BSL_META("################################################################################\n")));

            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_INTERNAL);
        }
    }
    else if (DBAL_ACTION_ENTRY_COMMIT == ptr_entry->action)
    {

        if (entry_not_found && !is_journaled_entry_default)
        {
            LOG_CLI((BSL_META("\n################################################################################\n")));
            LOG_CLI((BSL_META("DBAL entry not found when it should exist for table stamp: %s, core id: %d\n"),
                     table->table_name, (ptr_payload->core_id)));
            LOG_CLI((BSL_META("Data associated with DBAL entry:\n")));
            SHR_IF_ERR_EXIT(dbal_entry_print(unit, ptr_payload, TRUE));
            LOG_CLI((BSL_META("################################################################################\n")));

            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_INTERNAL);
        }

        if (entries_match && is_compare_payload)
        {
            entries_match =
                (sal_memcmp
                 (ptr_payload->phy_entry.payload, get_entry_handle->phy_entry.payload,
                  WORDS2BYTES(BITS2WORDS(table->multi_res_info[ptr_payload->cur_res_type].entry_payload_size))) == 0);
        }

        if (DBAL_TABLE_IS_TCAM(table))
        {

            if (entries_match)
            {
                entries_match =
                    (sal_memcmp
                     (ptr_payload->phy_entry.key, get_entry_handle->phy_entry.key,
                      DBAL_PHYSICAL_KEY_SIZE_IN_BYTES) == 0);
            }

            if (entries_match)
            {
                entries_match =
                    (sal_memcmp
                     (ptr_payload->phy_entry.k_mask, get_entry_handle->phy_entry.k_mask,
                      DBAL_PHYSICAL_KEY_SIZE_IN_BYTES) == 0);
            }

        }

        if (!entries_match)
        {
            LOG_CLI((BSL_META("\n################################################################################\n")));
            LOG_CLI((BSL_META
                     ("Found difference in state before and after comparison for table stamp: %s, core id: %d\n"),
                     table->table_name, (get_entry_handle->core_id)));
            LOG_CLI((BSL_META("Entry data before comparison journal start:\n")));
            SHR_IF_ERR_EXIT(dbal_entry_print(unit, ptr_payload, TRUE));
            LOG_CLI((BSL_META("Entry data after comparison journal end:\n")));
            SHR_IF_ERR_EXIT(dbal_entry_print(unit, get_entry_handle, TRUE));
            LOG_CLI((BSL_META("################################################################################\n")));

            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_INTERNAL);
        }
    }
    else if (DBAL_JOURNAL_ACTION_TABLE_CREATE == ptr_entry->action)
    {
        int table_exists;
        SHR_IF_ERR_EXIT(dbal_tables_is_table_active(unit, ptr_payload->table_id, &table_exists));

        if (!table_exists)
        {
            SHR_EXIT();
        }

        if (!ISEMPTY(table->table_name))
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                     "dbal comparison journal ERROR: table %s was created but not destroyed in the transaction.\n%s%s",
                                     table->table_name, EMPTY, EMPTY);
        }
        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                     "dbal comparison journal ERROR: table DYNAMIC_%d was created but not destroyed in the transaction.\n%s%s",
                                     ptr_payload->table_id, EMPTY, EMPTY);
        }
    }
    else if (DBAL_JOURNAL_ACTION_TABLE_DESTROY == ptr_entry->action)
    {
        int table_exists;
        SHR_IF_ERR_EXIT(dbal_tables_is_table_active(unit, ptr_payload->table_id, &table_exists));

        if (table_exists)
        {
            SHR_EXIT();
        }

        if (!ISEMPTY(table->table_name))
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                     "dbal comparison journal ERROR: table %s was destroyed but not created in the transaction.\n%s%s",
                                     table->table_name, EMPTY, EMPTY);
        }
        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                     "dbal comparison journal ERROR: table DYNAMIC_%d was destroyed but not created in the transaction.\n%s%s",
                                     ptr_payload->table_id, EMPTY, EMPTY);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal comparison journal ERROR: unrecognized dbal journal entry type.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

exit:
    if (NULL != get_entry_handle)
    {
        SHR_FREE(get_entry_handle);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_dbal_comparison_journal_entries_make_distinct(
    int unit,
    dnx_rollback_journal_t * journal,
    dnx_rollback_journal_entry_t * first_entry,
    dnx_rollback_journal_entry_t * second_entry)
{
    uint8 entries_match = FALSE;
    uint8 *first_metadata, *second_metadata;
    uint8 *first_payload, *second_payload;
    uint32 should_release_first_metadata = 0;
    uint32 should_release_first_payload = 0;
    uint32 should_release_second_metadata = 0;
    uint32 should_release_second_payload = 0;
    dbal_entry_handle_t *first_dbal_payload;
    dbal_entry_handle_t *second_dbal_payload;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_get_merged_metadata_payload_ptr(unit, journal, first_entry,
                                                                               &first_metadata, &first_payload,
                                                                               &should_release_first_metadata,
                                                                               &should_release_first_payload));
    first_dbal_payload = (dbal_entry_handle_t *) first_payload;

    if (second_entry == NULL)
    {
        dnx_dbal_journal_entry_t *first_dbal_metadata = (dnx_dbal_journal_entry_t *) first_metadata;

        if (first_dbal_metadata->action == DBAL_JOURNAL_ACTION_TABLE_CREATE)
        {
            int table_exists;
            SHR_IF_ERR_EXIT(dbal_tables_is_table_active(unit, first_dbal_payload->table_id, &table_exists));

            if (table_exists)
            {
                LOG_CLI((BSL_META
                         ("\n################################################################################\n")));
                LOG_CLI((BSL_META("DBAL table ID %d creation entry found without a table destruction entry.\n"),
                         first_dbal_payload->table_id));
                LOG_CLI((BSL_META
                         ("################################################################################\n")));
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_INTERNAL);
            }
        }
        if (first_dbal_metadata->action == DBAL_JOURNAL_ACTION_TABLE_DESTROY)
        {
            int table_exists;
            SHR_IF_ERR_EXIT(dbal_tables_is_table_active(unit, first_dbal_payload->table_id, &table_exists));

            if (!table_exists)
            {
                LOG_CLI((BSL_META
                         ("\n################################################################################\n")));
                LOG_CLI((BSL_META("DBAL table ID %d destruction entry found without a table creation entry.\n"),
                         first_dbal_payload->table_id));
                LOG_CLI((BSL_META
                         ("################################################################################\n")));
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_INTERNAL);
            }
        }
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_get_merged_metadata_payload_ptr(unit, journal, second_entry,
                                                                               &second_metadata, &second_payload,
                                                                               &should_release_second_metadata,
                                                                               &should_release_second_payload));
    second_dbal_payload = (dbal_entry_handle_t *) second_payload;

    if (first_dbal_payload->table_id == second_dbal_payload->table_id)
    {
        dnx_dbal_journal_entry_t *first_dbal_metadata = (dnx_dbal_journal_entry_t *) first_metadata;
        dnx_dbal_journal_entry_t *second_dbal_metadata = (dnx_dbal_journal_entry_t *) second_metadata;

        if (first_dbal_metadata->action == DBAL_JOURNAL_ACTION_TABLE_CREATE
            && second_dbal_metadata->action == DBAL_JOURNAL_ACTION_TABLE_DESTROY)
        {
            first_entry->state = DNX_ROLLBACK_JOURNAL_ENTRY_STATE_INVALID;

            second_entry->state = DNX_ROLLBACK_JORUNAL_ENTRY_STATE_CONDITIONALLY_INVALID;
            SHR_EXIT();
        }
        if (second_dbal_metadata->action == DBAL_JOURNAL_ACTION_TABLE_DESTROY)
        {

            first_entry->state = DNX_ROLLBACK_JOURNAL_ENTRY_STATE_INVALID;
            SHR_EXIT();
        }
        entries_match =
            (sal_memcmp
             (first_dbal_payload->phy_entry.key, second_dbal_payload->phy_entry.key,
              DBAL_PHYSICAL_KEY_SIZE_IN_BYTES) == 0);
    }

    if (entries_match
        && (first_dbal_payload->core_id == DBAL_CORE_ALL)
        && (second_dbal_payload->core_id != DBAL_CORE_ALL)
        && (dnx_data_device.general.valid_cores_bitmap_get(unit) != 1))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal comparison journal ERROR:\n Unsupported matching between two dbal journal entries for table %s.\n There is mixed usage of CORE_ALL and singular core_ids in the dbal operations in the same transaction.%s%s",
                                 dbal_logical_table_to_string(unit, first_dbal_payload->table_id), EMPTY, EMPTY);
    }

    if (entries_match
        && ((first_dbal_payload->core_id == second_dbal_payload->core_id)
            || (second_dbal_payload->core_id == DBAL_CORE_ALL)
            || (dnx_data_device.general.valid_cores_bitmap_get(unit) == 1)))
    {
        second_entry->state = DNX_ROLLBACK_JOURNAL_ENTRY_STATE_INVALID;
    }
    SHR_EXIT();
exit:
    if (should_release_first_metadata)
        SHR_FREE(first_metadata);
    if (should_release_first_payload)
        SHR_FREE(first_payload);
    if (should_release_second_metadata)
        SHR_FREE(second_metadata);
    if (should_release_second_payload)
        SHR_FREE(second_payload);

    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_dbal_rollback_journal_entry_state_get(
    int unit,
    dbal_logical_table_t * table,
    dnx_rollback_journal_entry_state * entry_state)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((DBAL_ACCESS_METHOD_HARD_LOGIC == table->access_method)
        || (DBAL_ACCESS_METHOD_TCAM_CS == table->access_method)
        || (DBAL_ACCESS_METHOD_SW_STATE == table->access_method))
    {

        *entry_state = DNX_ROLLBACK_JOURNAL_ENTRY_STATE_ALWAYS_VALID;
    }
    else if ((DBAL_ACCESS_METHOD_MDB == table->access_method)
             || (DBAL_ACCESS_METHOD_PEMLA == table->access_method) || (DBAL_ACCESS_METHOD_KBP == table->access_method))
    {

        *entry_state = DNX_ROLLBACK_JOURNAL_ENTRY_STATE_VALID;
    }
    else
    {

        *entry_state = DNX_ROLLBACK_JOURNAL_ENTRY_STATE_INVALID;
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal journal ERROR: Unrecognized access method entry attempted to be inserted in dbal journal.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_dbal_journal_entry_insert(
    int unit,
    int er_thread_id,
    dbal_entry_handle_t * ptr_handle,
    int action,
    dnx_rollback_journal_rollback_handler rollback_handler,
    uint8 is_er,
    uint8 is_cmp,
    uint8 is_dbal_error)
{
    dnx_dbal_journal_entry_t journal_entry;

    dnx_rollback_journal_entry_state entry_state = DNX_ROLLBACK_JOURNAL_ENTRY_STATE_VALID;

    SHR_FUNC_INIT_VARS(unit);

    journal_entry.action = action;

    SHR_IF_ERR_EXIT(dnx_dbal_rollback_journal_entry_state_get(unit, ptr_handle->table, &entry_state));

    if (is_er && !dnx_dbal_journal_is_table_excluded(unit, er_thread_id, ptr_handle->table_id, FALSE)
        && (!is_dbal_error || (DNX_ROLLBACK_JOURNAL_ENTRY_STATE_ALWAYS_VALID == entry_state)))
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_new(unit,
                                                       er_thread_id,
                                                       DNX_DBAL_ROLLBACK_JOURNAL_HANDLE,
                                                       (uint8 *) (&journal_entry),
                                                       sizeof(dnx_dbal_journal_entry_t),
                                                       (uint8 *) ptr_handle,
                                                       sizeof(dbal_entry_handle_t),
                                                       entry_state,
                                                       rollback_handler, NULL, dnx_dbal_journal_entry_print_cb));
    }

    if (is_cmp && !dnx_dbal_journal_is_table_excluded(unit, er_thread_id, ptr_handle->table_id, TRUE))
    {

        SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_new(unit,
                                                       er_thread_id,
                                                       DNX_DBAL_COMPARISON_JOURNAL_HANDLE,
                                                       (uint8 *) (&journal_entry),
                                                       sizeof(dnx_dbal_journal_entry_t),
                                                       (uint8 *) ptr_handle,
                                                       sizeof(dbal_entry_handle_t),
                                                       DNX_ROLLBACK_JOURNAL_ENTRY_STATE_VALID,
                                                       &dnx_dbal_comparison_journal_roll_back_entry_cb,
                                                       NULL, dnx_dbal_journal_entry_print_cb));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

uint8
dnx_dbal_journal_is_done_init(
    int unit)
{
    return dnx_rollback_journal_is_done_init(unit);
}

shr_error_e
dnx_dbal_journal_initialize(
    int unit,
    dnx_rollback_journal_cbs_t rollback_journal_cbs,
    dnx_dbal_journal_access_cb access_cb,
    uint8 is_comparison)
{
    int idx;
    SHR_FUNC_INIT_VARS(unit);

    if (NULL == rollback_journal_cbs.is_on)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal journal ERROR: journal is on callback must be set.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    for (idx = 0; idx < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; idx++)
    {
        dnx_dbal_journal_manager[unit][idx].unsupported_table_bypass_counter = 0;

        if (is_comparison)
        {

            SHR_IF_ERR_EXIT(dnx_rollback_journal_new(unit,
                                                     idx,
                                                     TRUE,
                                                     DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
                                                     DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL,
                                                     rollback_journal_cbs, &(DNX_DBAL_COMPARISON_JOURNAL_HANDLE)));

            dnx_dbal_journal_manager[unit][idx].journals[DNX_DBAL_COMPARISON_JOURNAL_NUM].access_cb = access_cb;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_rollback_journal_new(unit,
                                                     idx,
                                                     FALSE,
                                                     DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
                                                     DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL,
                                                     rollback_journal_cbs, &(DNX_DBAL_ROLLBACK_JOURNAL_HANDLE)));
            dnx_dbal_journal_manager[unit][idx].journals[DNX_DBAL_ROLLBACK_JOURNAL_NUM].access_cb = access_cb;
        }

        if (is_comparison)
        {
            SHR_IF_ERR_EXIT(dnx_dbal_journal_exclude_table_clear(unit, idx, is_comparison));
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

uint8
dnx_dbal_journal_is_on(
    int unit,
    dnx_err_recovery_journal_e journals)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);

    if (er_thread_id == -1)
    {
        return FALSE;
    }

    switch (journals)
    {
        case DNX_DBAL_JOURNAL_ALL:
            return (dnx_rollback_journal_is_on(unit, er_thread_id, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE)
                    || dnx_rollback_journal_is_on(unit, er_thread_id, DNX_DBAL_COMPARISON_JOURNAL_HANDLE));
        case DNX_DBAL_JOURNAL_ROLLBACK:
            return dnx_rollback_journal_is_on(unit, er_thread_id, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE);
        case DNX_DBAL_JOURNAL_COMPARISON:
            return dnx_rollback_journal_is_on(unit, er_thread_id, DNX_DBAL_COMPARISON_JOURNAL_HANDLE);
        default:
            return FALSE;
    }
}

static inline shr_error_e
dnx_dbal_journal_destroy_internal(
    int unit,
    uint8 is_comparison)
{
    int er_thread_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_destroy(unit, &DNX_DBAL_COMPARISON_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_destroy(unit, &DNX_DBAL_ROLLBACK_JOURNAL_HANDLE));
    }

    for (er_thread_id = 0; er_thread_id < DNX_ERR_RECOVERY_MAX_NOF_ACTIVE_TRANSACTION; er_thread_id++)
    {
        dnx_dbal_journal_manager[unit][er_thread_id].unsupported_table_bypass_counter = 0;
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_destroy(
    int unit,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_dbal_journal_destroy_internal(unit, is_comparison));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_clear(
    int unit,
    int er_thread_id,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_clear(unit, er_thread_id, DNX_DBAL_COMPARISON_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_clear(unit, er_thread_id, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_roll_back(
    int unit,
    int er_thread_id,
    uint8 only_head_rollback,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_comparison)
    {
        int distinct_rv = _SHR_E_NONE;
        int rollback_rv = _SHR_E_NONE;

        distinct_rv = dnx_rollback_journal_distinct
            (unit, er_thread_id, DNX_DBAL_COMPARISON_JOURNAL_HANDLE,
             &dnx_dbal_comparison_journal_entries_make_distinct);

        rollback_rv = dnx_rollback_journal_rollback
            (unit, er_thread_id, only_head_rollback, DNX_DBAL_COMPARISON_JOURNAL_HANDLE);

        SHR_IF_ERR_EXIT(DNX_ROLLBACK_JOURNAL_CMP_ERROR_PARSE(distinct_rv, rollback_rv));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_rollback
                        (unit, er_thread_id, only_head_rollback, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static uint8
dnx_dbal_journal_is_unsupported_table(
    int unit,
    uint32 table_id)
{
    dbal_status_e status;
    dbal_table_status_e table_status;

    if (_SHR_E_NONE != dbal_status_get(unit, &status))
    {
        return TRUE;
    }

    DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status);

    if ((table_status == DBAL_TABLE_HW_ERROR) && (status == DBAL_STATUS_DEVICE_INIT_DONE))
    {
        return TRUE;
    }

    return ((DBAL_TABLE_HCC_HBM_DRAM_CPU_ACCESS == table_id) || (DBAL_TABLE_HBMC_INTERRUPTS == table_id));
}

uint8
dnx_dbal_journal_should_create_test(
    int unit,
    uint32 table_id)
{
    multi_res_info_t result_set;
    int result_type_id;
    int result_id;
    dbal_table_field_info_t field_info;
    CONST dbal_logical_table_t *table = NULL;

    if (dbal_tables_table_get(unit, table_id, &table) != _SHR_E_NONE)
    {
        return FALSE;
    }

    for (result_type_id = 0; result_type_id < table->nof_result_types; result_type_id++)
    {
        result_set = table->multi_res_info[result_type_id];
        for (result_id = 0; result_id < result_set.nof_result_fields; result_id++)
        {
            field_info = result_set.results_info[result_id];

            if (DBAL_PERMISSION_TRIGGER == field_info.permission || DBAL_PERMISSION_READONLY == field_info.permission)
            {
                return FALSE;
            }
        }
    }

    return !dnx_dbal_journal_is_unsupported_table(unit, table_id);
}

static shr_error_e
dnx_dbal_journal_log_common(
    int unit,
    int er_thread_id,
    dbal_entry_handle_t * ptr_handle,
    dbal_actions_e action,
    uint32 er_flags,
    uint8 is_dbal_error)
{
    uint8 is_er = (0 != (er_flags & DNX_DBAL_JOURNAL_ROLLBACK_FLAG));
    uint8 is_cmp = (0 != (er_flags & DNX_DBAL_JOURNAL_COMPARE_FLAG));

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_err_recovery_common_is_validation_enabled(unit))
    {
        if (NULL == ptr_handle)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                        "dbal journal ERROR: NULL data attempted to be inserted to journal.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
    }

    if (dnx_dbal_journal_is_unsupported_table(unit, ptr_handle->table_id))
    {

        if (dnx_dbal_journal_manager[unit][er_thread_id].unsupported_table_bypass_counter > 0)
        {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal journal ERROR: Unsupported table access: %s. Error recovery is not supported for this API !\n%s%s",
                                 ptr_handle->table->table_name, EMPTY, EMPTY);
    }

    if (ptr_handle->table->table_type == DBAL_TABLE_TYPE_TCAM
        && ptr_handle->table->access_method == DBAL_ACCESS_METHOD_MDB)
    {
        SHR_BITAND_RANGE(ptr_handle->phy_entry.key, ptr_handle->phy_entry.k_mask, 0, ptr_handle->phy_entry.key_size,
                         ptr_handle->phy_entry.key);
    }

    SHR_IF_ERR_EXIT(dnx_dbal_journal_entry_insert
                    (unit, er_thread_id, ptr_handle, action, &dnx_dbal_journal_rollback_common, is_er, is_cmp,
                     is_dbal_error));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_get_head_seq_id(
    int unit,
    int er_thread_id,
    uint8 is_comparison,
    uint32 *seq_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_get_head_seq_id
                        (unit, er_thread_id, seq_id, DNX_DBAL_COMPARISON_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_get_head_seq_id
                        (unit, er_thread_id, seq_id, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_all_tmp_suppress_unsafe(
    int unit,
    int er_thread_id,
    uint8 is_disable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_disabled_counter_change
                    (unit, er_thread_id, is_disable, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE, TRUE));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_disabled_counter_change
                    (unit, er_thread_id, is_disable, DNX_DBAL_COMPARISON_JOURNAL_HANDLE, TRUE));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_comparison_tmp_suppress_unsafe(
    int unit,
    uint8 is_disable)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    assert(er_thread_id != -1);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_disabled_counter_change
                    (unit, er_thread_id, is_disable, DNX_DBAL_COMPARISON_JOURNAL_HANDLE, TRUE));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

uint8
dnx_dbal_journal_is_tmp_suppressed_unsafe(
    int unit,
    int er_thread_id,
    uint8 is_comparison)
{
    uint32 counter = 0;

    if (is_comparison)
    {
        dnx_rollback_journal_disabled_counter_get(unit, er_thread_id, DNX_DBAL_COMPARISON_JOURNAL_HANDLE, &counter);
    }
    else
    {
        dnx_rollback_journal_disabled_counter_get(unit, er_thread_id, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE, &counter);
    }

    return (counter != 0);
}

static inline shr_error_e
dnx_dbal_journal_entry_access_id_update(
    int unit,
    dbal_entry_handle_t * handle,
    int access_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DBAL_TABLE_IS_TCAM(handle->table))
    {
        if (handle->table->access_method == DBAL_ACCESS_METHOD_KBP)
        {
            if (dnx_err_recovery_common_is_validation_enabled(unit))
            {
                if (handle->phy_entry.entry_hw_id == 0)
                {
                    DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                                "dbal journal ERROR: attempted to journal KBP TCAM entry with hw id = 0.\n%s%s%s",
                                                                EMPTY, EMPTY, EMPTY);
                }
            }
            handle->phy_entry.entry_hw_id = access_id;
            handle->access_id_set = TRUE;
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static inline shr_error_e
dnx_dbal_journal_log_add_internal(
    int unit,
    int er_thread_id,
    dbal_entry_handle_t * get_handle,
    dbal_entry_handle_t * entry_handle,
    uint8 is_dbal_error)
{
    int committed_entry_access_id = entry_handle->phy_entry.entry_hw_id;
    dbal_logical_table_t *table = entry_handle->table;

    SHR_FUNC_INIT_VARS(unit);

    if (get_handle == NULL)
    {

        if (!is_dbal_error)
        {
            SHR_IF_ERR_EXIT(dnx_dbal_journal_entry_access_id_update(unit, entry_handle, committed_entry_access_id));
        }

        SHR_IF_ERR_EXIT(dnx_dbal_journal_log_common
                        (unit, er_thread_id, entry_handle, DBAL_ACTION_ENTRY_CLEAR, entry_handle->er_flags,
                         is_dbal_error));
    }
    else
    {

        if (table->access_method == DBAL_ACCESS_METHOD_MDB && get_handle != entry_handle)
        {
            sal_memcpy(get_handle->phy_entry.p_mask, entry_handle->phy_entry.p_mask, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
        }
        get_handle->phy_entry.indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_UPDATE;
        get_handle->entry_merged = 1;

        if (!is_dbal_error)
        {
            SHR_IF_ERR_EXIT(dnx_dbal_journal_entry_access_id_update(unit, get_handle, committed_entry_access_id));
        }

        SHR_IF_ERR_EXIT(dnx_dbal_journal_log_common
                        (unit, er_thread_id, get_handle, DBAL_ACTION_ENTRY_COMMIT, entry_handle->er_flags,
                         is_dbal_error));

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_log_add(
    int unit,
    dbal_entry_handle_t * get_handle,
    dbal_entry_handle_t * entry_handle,
    uint8 is_dbal_error)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (er_thread_id == -1)
    {
        SHR_EXIT();
    }

    DNX_DBAL_JOURNAL_ACCESS_NOTIFY(unit, er_thread_id, entry_handle->table);

    if (0 != entry_handle->er_flags)
    {
        SHR_IF_ERR_EXIT(dnx_dbal_journal_log_add_internal(unit, er_thread_id, get_handle, entry_handle, is_dbal_error));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static inline shr_error_e
dnx_dbal_journal_log_clear_internal(
    int unit,
    int er_thread_id,
    dbal_entry_handle_t * entry_handle,
    uint8 is_dbal_error)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_dbal_journal_log_common
                    (unit, er_thread_id, entry_handle, DBAL_ACTION_ENTRY_COMMIT, entry_handle->er_flags,
                     is_dbal_error));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_log_clear(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 is_dbal_error)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (er_thread_id == -1)
    {
        SHR_EXIT();
    }

    DNX_DBAL_JOURNAL_ACCESS_NOTIFY(unit, er_thread_id, entry_handle->table);

    if (0 != entry_handle->er_flags)
    {
        SHR_IF_ERR_EXIT(dnx_dbal_journal_log_clear_internal(unit, er_thread_id, entry_handle, is_dbal_error));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_unsupported_tables_bypass(
    int unit,
    int er_thread_id,
    uint8 is_bypassed)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_bypassed)
    {
        dnx_dbal_journal_manager[unit][er_thread_id].unsupported_table_bypass_counter++;
    }
    else
    {
        if (0 == dnx_dbal_journal_manager[unit][er_thread_id].unsupported_table_bypass_counter)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                     "dbal journal ERROR: attempted to end unsupported tables bypass region without start.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }

        dnx_dbal_journal_manager[unit][er_thread_id].unsupported_table_bypass_counter--;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

uint8
dnx_dbal_journal_are_unsupported_tables_bypassed(
    int unit,
    int er_thread_id)
{
    return (dnx_dbal_journal_manager[unit][er_thread_id].unsupported_table_bypass_counter > 0) ? TRUE : FALSE;
}

shr_error_e
dnx_dbal_journal_exclude_table_by_stamp(
    int unit,
    int er_thread_id,
    char *stamp,
    uint8 is_comparison)
{
    dbal_tables_e table_id;
    SHR_FUNC_INIT_VARS(unit);

    if (!is_comparison)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal journal ERROR: rollback journal table exclusions are not supported.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    if (!dnx_rollback_journal_is_on(unit, er_thread_id, DNX_DBAL_COMPARISON_JOURNAL_HANDLE))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal journal ERROR: comparison journal table exclusions are done only if the dbal comparsion journal is on.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    if (DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION.excluded_tables_count + 1 >= DNX_DBAL_JOURNAL_EXCLUDED_TABLES_MAX_NOF)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal journal ERROR: comparison journal table exclusions exceed the maximum number of allowed.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, stamp, &table_id));

    DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION.
        excluded_tables[DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION.excluded_tables_count] = table_id;
    DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION.excluded_tables_count++;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_exclude_table_clear(
    int unit,
    int er_thread_id,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!is_comparison)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                                 "dbal journal ERROR: rollback journal table exclusions clear are not supported.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    DNX_DBAL_COMPARISON_JOURNAL_TRANSACTION.excluded_tables_count = 0;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_print(
    int unit,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_print(unit, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_print(unit, DNX_DBAL_COMPARISON_JOURNAL_HANDLE));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_logger_state_change(
    int unit,
    uint8 is_comparison)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!is_comparison)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_logger_state_change_by_type(unit, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_logger_state_change_by_type(unit, DNX_DBAL_COMPARISON_JOURNAL_HANDLE));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_logger_state_get(
    int unit,
    uint8 *is_logging)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_rollback_journal_is_logging(unit, DNX_DBAL_ROLLBACK_JOURNAL_HANDLE, is_logging);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_dbal_journal_rollback_dbal_table_create(
    int unit,
    int er_thread_id,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dbal_entry_handle_t *ptr_payload = (dbal_entry_handle_t *) payload;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_err_recovery_common_is_validation_enabled(unit))
    {
        if (NULL == metadata)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                        "dbal journal ERROR: invalid entry data detected.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_destroy(unit, ptr_payload->table_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_log_dbal_table_create(
    int unit,
    dbal_tables_e table_id)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);
    dbal_entry_handle_t entry;
    CONST dbal_logical_table_t *table = NULL;
    int add_to_cmp;

    SHR_FUNC_INIT_VARS(unit);

    DNX_DBAL_JOURNAL_EXIT_IF_OFF(unit, DNX_DBAL_JOURNAL_ALL);

    entry.table_id = table_id;

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    entry.table = (dbal_logical_table_t *) table;

    add_to_cmp = dnx_dbal_journal_is_on(unit, DNX_DBAL_JOURNAL_COMPARISON);
    SHR_IF_ERR_EXIT(dnx_dbal_journal_entry_insert
                    (unit, er_thread_id, &entry, DBAL_JOURNAL_ACTION_TABLE_CREATE,
                     &dnx_dbal_journal_rollback_dbal_table_create, TRUE, add_to_cmp, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_dbal_journal_rollback_dbal_table_destroy(
    int unit,
    int er_thread_id,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dbal_entry_handle_t *ptr_payload = (dbal_entry_handle_t *) (payload);

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_err_recovery_common_is_validation_enabled(unit))
    {
        if (NULL == metadata)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                        "dbal journal ERROR: invalid entry data detected.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_restore(unit, ptr_payload->table_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_log_dbal_table_destroy(
    int unit,
    dbal_tables_e table_id)
{
    int er_thread_id = dnx_er_threading_transaction_get(unit);
    dbal_entry_handle_t entry;
    CONST dbal_logical_table_t *table = NULL;
    int add_to_cmp;

    SHR_FUNC_INIT_VARS(unit);

    DNX_DBAL_JOURNAL_EXIT_IF_OFF(unit, DNX_DBAL_JOURNAL_ALL);

    if (table_id < DBAL_NOF_TABLES)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(_SHR_E_INTERNAL,
                                                    "dbal journal ERROR: Attempted to log change to non dynamic DBAL table %d.\n%s%s",
                                                    table_id, EMPTY, EMPTY);
    }

    entry.table_id = table_id;

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    entry.table = (dbal_logical_table_t *) table;

    add_to_cmp = dnx_dbal_journal_is_on(unit, DNX_DBAL_JOURNAL_COMPARISON);
    SHR_IF_ERR_EXIT(dnx_dbal_journal_entry_insert
                    (unit, er_thread_id, &entry, DBAL_JOURNAL_ACTION_TABLE_DESTROY,
                     &dnx_dbal_journal_rollback_dbal_table_destroy, TRUE, add_to_cmp, FALSE));

exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME

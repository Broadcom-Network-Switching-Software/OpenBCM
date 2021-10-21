
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#include <soc/dnx/field/tcam/tcam_access_profile_manager.h>

#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_profile_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

typedef enum
{
    DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_INVALID = -1,
    DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_REMOVE = 0,
    DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_LSB = 1,
    DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_MSB = 2,
    DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_ALL = 3,
} dnx_field_tcam_access_profile_action_bmp_e;

static shr_error_e
dnx_field_tcam_access_profile_algo_find_id(
    int unit,
    dnx_field_tcam_database_t * db,
    int *access_profile_id)
{
    int profile_id;
    uint8 occupied;

    SHR_FUNC_INIT_VARS(unit);

    for (profile_id = 0; profile_id < dnx_data_field.tcam.nof_access_profiles_get(unit); profile_id++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.occupied.get(unit, profile_id, &occupied));
        if (!occupied)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "algo_find_id, found id: %d%s%s%s", profile_id, EMPTY, EMPTY, EMPTY);
            if (db->key_size == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
            {

                int paired_profile_id = DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(profile_id);
                if (paired_profile_id >= dnx_data_field.tcam.nof_access_profiles_get(unit))
                {

                    continue;
                }
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                                occupied.get(unit, paired_profile_id, &occupied));
                if (occupied)
                {

                    continue;
                }
            }
            *access_profile_id = profile_id;
            break;
        }
    }

    if (profile_id == dnx_data_field.tcam.nof_access_profiles_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "Error allocating access profile: No access profile available");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_prefix_write(
    int unit,
    uint32 handler_id,
    uint32 prefix_value)
{
    uint32 key_size;
    int access_profile_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    DNX_STATE_COMPARISON_SUPPRESS(unit, TRUE);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_TCAM_ACCESS_PROFILES, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_id_get(unit, handler_id, &access_profile_id));

    if (key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {

        access_profile_id = DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(access_profile_id);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_ACCESS_PROFILE_ID, access_profile_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PREFIX, INST_SINGLE, prefix_value);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    DNX_STATE_COMPARISON_SUPPRESS(unit, FALSE);
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_access_profile_alloc_id_aux(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_database_t * db,
    int access_profile_id)
{
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    key_size.set(unit, access_profile_id, db->key_size));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    action_size.set(unit, access_profile_id, db->action_size));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                        bank_ids_bmp.set(unit, access_profile_id, core_id, 0));
    }
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    is_direct.set(unit, access_profile_id, db->is_direct));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    prefix.set(unit, access_profile_id, db->prefix_value));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    handler_id.set(unit, access_profile_id, handler_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.occupied.set(unit, access_profile_id, TRUE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_access_profile_alloc_id(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_database_t * db,
    int access_profile_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_alloc_id_aux(unit, handler_id, db, access_profile_id));
    if (db->key_size == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
    {

        int paired_profile_id = DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(access_profile_id);
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_alloc_id_aux(unit, handler_id, db, paired_profile_id));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_access_profile_convert_key_to_mode(
    int unit,
    dnx_field_key_length_type_e key_size,
    int *key_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (key_size)
    {
        case DNX_FIELD_KEY_LENGTH_TYPE_DT:
        case DNX_FIELD_KEY_LENGTH_TYPE_HALF:
            *key_mode = dnx_data_field.tcam.access_profile_half_key_mode_get(unit);
            break;
        case DNX_FIELD_KEY_LENGTH_TYPE_SINGLE:
            *key_mode = dnx_data_field.tcam.access_profile_single_key_mode_get(unit);
            break;
        case DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE:
            *key_mode = dnx_data_field.tcam.access_profile_double_key_mode_get(unit);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid key_size %d", key_size);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_access_profile_hw_set(
    int unit,
    dnx_field_tcam_database_t * db,
    int access_profile_id)
{
    uint32 entry_handle_id;
    int key_mode = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_convert_key_to_mode(unit, db->key_size, &key_mode));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_TCAM_ACCESS_PROFILES, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_ACCESS_PROFILE_ID, access_profile_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_MODE, INST_SINGLE, key_mode);

    if (!dnx_data_field.tcam.access_hw_mix_ratio_supported_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_CASCADED, INST_SINGLE, 0);
    }

    if (dnx_data_field.tcam.direct_supported_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DIRECT, INST_SINGLE, db->is_direct);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_access_profile_hw_clear(
    int unit,
    int access_profile_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_TCAM_ACCESS_PROFILES_BANKS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_ACCESS_PROFILE_ID, access_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FIELD_TCAM_BANK_ID, 0,
                                     dnx_data_field.tcam.nof_banks_get(unit) - 1);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FIELD_TCAM_ACCESS_PROFILES_BANKS, entry_handle_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_TCAM_ACCESS_PROFILES, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_ACCESS_PROFILE_ID, access_profile_id);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_access_profile_write_dbal(
    int unit,
    dnx_field_tcam_database_t * db,
    int access_profile_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_hw_set(unit, db, access_profile_id));

    if (db->key_size == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
    {

        int paired_profile_id = DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(access_profile_id);
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_hw_set(unit, db, paired_profile_id));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_access_profile_alloc_verify(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_database_t * db,
    int access_profile_id)
{
    uint8 occupied;

    SHR_FUNC_INIT_VARS(unit);

    if (access_profile_id != FIELD_TCAM_ACCESS_PROFILE_ID_AUTO)
    {
        if (access_profile_id >= dnx_data_field.tcam.nof_access_profiles_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Requested access profile id (%d) for handler (%d) is out of range (%d)\r\n",
                         access_profile_id, handler_id, dnx_data_field.tcam.nof_access_profiles_get(unit));
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                        occupied.get(unit, access_profile_id, &occupied));
        if (occupied)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Access profile id requested (%d) for handler (%d) already in use",
                         access_profile_id, handler_id);
        }

        if (db->key_size == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
        {

            int paired_profile_id = DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(access_profile_id);
            if (paired_profile_id >= dnx_data_field.tcam.nof_access_profiles_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Requested access profile id (%d) for double key mode handler (%d) is out of range (%d)\r\n",
                             access_profile_id, handler_id, dnx_data_field.tcam.nof_access_profiles_get(unit));
            }
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                            occupied.get(unit, paired_profile_id, &occupied));
            if (occupied)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Paired access profile id (%d) for the access profile id requested (%d) for double key mode handler (%d) already in use",
                             paired_profile_id, access_profile_id, handler_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_alloc(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_database_t * db,
    int access_profile_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_field_tcam_access_profile_alloc_verify(unit, handler_id, db, access_profile_id));

    if (access_profile_id == FIELD_TCAM_ACCESS_PROFILE_ID_AUTO)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_algo_find_id(unit, db, &access_profile_id));
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_alloc_id(unit, handler_id, db, access_profile_id));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_write_dbal(unit, db, access_profile_id));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_access_profile_bank_update_dbal(
    int unit,
    int access_profile_id,
    int bank_id,
    bcm_core_t core_id,
    dnx_field_tcam_access_fg_params_t * fg_params,
    uint8 remove)
{
    uint32 entry_handle_id;
    dnx_field_tcam_access_profile_action_bmp_e action_bmp;
    int profile_to_update;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fg_params, _SHR_E_PARAM, "fg_params");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_TCAM_ACCESS_PROFILES_BANKS, &entry_handle_id));

    if (fg_params->key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {

        profile_to_update = ((bank_id % 2) == 0) ? access_profile_id :
            DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(access_profile_id);
    }
    else
    {
        profile_to_update = access_profile_id;
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_ACCESS_PROFILE_ID, profile_to_update);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_TCAM_BANK_ID, bank_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    if (remove)
    {
        action_bmp = DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_REMOVE;
    }
    else
    {

        if (fg_params->direct_table)
        {
            action_bmp = DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_MSB;
        }
        else if (fg_params->key_size == dnx_data_field.tcam.key_size_single_get(unit) &&
                 fg_params->action_size == dnx_data_field.tcam.action_size_half_get(unit))
        {
            action_bmp = DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_LSB;
        }
        else
        {
            action_bmp = DNX_FIELD_TCAM_ACCESS_PROFILE_ACTION_BMP_ALL;
        }
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_SELECT, INST_SINGLE, action_bmp);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_SELECT, INST_SINGLE, ! !action_bmp);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_bank_add(
    int unit,
    uint32 handler_id,
    int bank_id,
    bcm_core_t core_id)
{
    int access_profile_id;
    uint16 bank_ids_bmp;
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_id_get(unit, handler_id, &access_profile_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Adding TCAM bank id: %d to handler: %d on core:%d%s", bank_id, handler_id, core_id,
                 EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    bank_ids_bmp.get(unit, access_profile_id, core_id, &bank_ids_bmp));

    bank_ids_bmp |= SAL_BIT(bank_id);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    bank_ids_bmp.set(unit, access_profile_id, core_id, bank_ids_bmp));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_bank_update_dbal
                    (unit, access_profile_id, bank_id, core_id, &fg_params, FALSE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_bank_remove(
    int unit,
    uint32 handler_id,
    int bank_id,
    bcm_core_t core_id)
{
    int access_profile_id;
    uint16 bank_ids_bmp;
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_id_get(unit, handler_id, &access_profile_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Removing bank id: %d from handler: %d%s%s", bank_id, handler_id, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    bank_ids_bmp.get(unit, access_profile_id, core_id, &bank_ids_bmp));

    bank_ids_bmp &= SAL_RBIT(bank_id);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    bank_ids_bmp.set(unit, access_profile_id, core_id, bank_ids_bmp));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_bank_update_dbal
                    (unit, access_profile_id, bank_id, core_id, &fg_params, TRUE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_free(
    int unit,
    uint32 handler_id)
{
    int access_profile_id;
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_id_get(unit, handler_id, &access_profile_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.occupied.set(unit, access_profile_id, FALSE));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_hw_clear(unit, access_profile_id));

    if (fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        int paired_profile_id = DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(access_profile_id);

        if (paired_profile_id >= dnx_data_field.tcam.nof_access_profiles_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Invalid state: double key-sized handler %d can't have access profile %d allocated (max number of access profiles: %d)\r\n",
                         handler_id, paired_profile_id, dnx_data_field.tcam.nof_access_profiles_get(unit));
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.occupied.set(unit, paired_profile_id, FALSE));

        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_hw_clear(unit, paired_profile_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_banks_bmp_get(
    int unit,
    uint32 handler_id,
    bcm_core_t core_id,
    uint32 *banks_bmp)
{
    uint16 banks_bmp_sw;
    int access_profile_id;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_id_get(unit, handler_id, &access_profile_id));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    bank_ids_bmp.get(unit, access_profile_id, core_id, &banks_bmp_sw));
    (*banks_bmp) = banks_bmp_sw;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_banks_sorted_get(
    int unit,
    uint32 handler_id,
    bcm_core_t core_id,
    int *bank_ids,
    int *bank_count)
{
    int access_profile_id;
    uint16 bank_ids_bmp;
    int bank_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_id_get(unit, handler_id, &access_profile_id));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                    bank_ids_bmp.get(unit, access_profile_id, core_id, &bank_ids_bmp));

    *bank_count = 0;
    for (bank_id = 0; bank_id < dnx_data_field.tcam.nof_banks_get(unit); bank_id++)
    {
        if (bank_ids_bmp & SAL_BIT(bank_id))
        {
            bank_ids[*bank_count] = bank_id;
            *bank_count = *bank_count + 1;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_id_get(
    int unit,
    uint32 handler_id,
    int *access_profile_id)
{
    int profile_id;
    uint8 occupied;
    uint32 used_handler_id;

    SHR_FUNC_INIT_VARS(unit);

    for (profile_id = 0; profile_id < dnx_data_field.tcam.nof_access_profiles_get(unit); profile_id++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.occupied.get(unit, profile_id, &occupied));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.
                        handler_id.get(unit, profile_id, &used_handler_id));
        if (occupied == TRUE && used_handler_id == handler_id)
        {
            *access_profile_id = profile_id;
            break;
        }
    }

    if (profile_id == dnx_data_field.tcam.nof_access_profiles_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Error retrieving access profile id for handler id: %d, no access profile allocated for given handler",
                     handler_id);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_profile_intersect(
    int unit,
    uint32 handler_id1,
    uint32 handler_id2,
    uint8 *intersect)
{
    int *bank_ids1_p = NULL;
    int banks_count1;
    int *bank_ids2_p = NULL;
    int banks_count2;
    int ii = 0;
    int jj = 0;
    bcm_core_t core_iter;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(bank_ids1_p, sizeof(*bank_ids1_p) * dnx_data_field.tcam.nof_banks_get(unit), "bank_ids1_p", "%s%s%s",
              EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(bank_ids2_p, sizeof(*bank_ids2_p) * dnx_data_field.tcam.nof_banks_get(unit), "bank_ids2_p", "%s%s%s",
              EMPTY, EMPTY, EMPTY);

    *intersect = FALSE;
    if (handler_id1 == handler_id2)
    {

        SHR_EXIT();
    }

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_iter)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_sorted_get
                        (unit, handler_id1, core_iter, bank_ids1_p, &banks_count1));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_sorted_get
                        (unit, handler_id2, core_iter, bank_ids2_p, &banks_count2));

        while (ii < banks_count1 && jj < banks_count2)
        {
            if (bank_ids1_p[ii] == bank_ids2_p[jj])
            {
                *intersect = TRUE;
                break;
            }

            if (bank_ids1_p[ii] < bank_ids2_p[jj])
            {
                ii++;
            }
            else
            {
                jj++;
            }
        }
    }
exit:
    SHR_FREE(bank_ids1_p);
    SHR_FREE(bank_ids2_p);
    SHR_FUNC_EXIT;
}

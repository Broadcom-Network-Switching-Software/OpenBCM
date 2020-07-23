/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MDBDNX

 

#include <shared/utilex/utilex_seq.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/utils/dnx_ire_packet_utils.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/mdb/mdb_init.h>
#include <bcm/port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm/switch.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_port_types.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>



static shr_error_e
dnx_switch_control_mdb_exact_match_false_hit_mdb(
    int unit)
{
    uint32 entry_handle_id;

    dbal_physical_tables_e dh_overwrite_dbal_table_ids[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES];
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id_iter;
    int dh_iter;

    int global_macro_idx;

    uint32 max_nof_macro_clusters = dnx_data_mdb.dh.max_nof_clusters_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dh_overwrite_dbal_table_ids[0] = DBAL_PHYSICAL_TABLE_LEM;
    dh_overwrite_dbal_table_ids[1] = DBAL_PHYSICAL_TABLE_IVSI;
    dh_overwrite_dbal_table_ids[2] = DBAL_PHYSICAL_TABLE_INLIF_2;
    dh_overwrite_dbal_table_ids[3] = DBAL_PHYSICAL_TABLE_LEM;
    dh_overwrite_dbal_table_ids[4] = DBAL_PHYSICAL_TABLE_LEM;
    dh_overwrite_dbal_table_ids[5] = DBAL_PHYSICAL_TABLE_LEM;
    dh_overwrite_dbal_table_ids[6] = DBAL_PHYSICAL_TABLE_LEM;
    dh_overwrite_dbal_table_ids[7] = DBAL_PHYSICAL_TABLE_LEM;
    dh_overwrite_dbal_table_ids[8] = DBAL_PHYSICAL_TABLE_LEM;
    dh_overwrite_dbal_table_ids[9] = DBAL_PHYSICAL_TABLE_LEM;
    dh_overwrite_dbal_table_ids[10] = DBAL_PHYSICAL_TABLE_LEM;
    dh_overwrite_dbal_table_ids[11] = DBAL_PHYSICAL_TABLE_LEM;
    dh_overwrite_dbal_table_ids[12] = DBAL_PHYSICAL_TABLE_ISEM_1;
    dh_overwrite_dbal_table_ids[13] = DBAL_PHYSICAL_TABLE_ISEM_1;
    dh_overwrite_dbal_table_ids[14] = DBAL_PHYSICAL_TABLE_ISEM_2;
    dh_overwrite_dbal_table_ids[15] = DBAL_PHYSICAL_TABLE_ISEM_3;
    dh_overwrite_dbal_table_ids[16] = DBAL_PHYSICAL_TABLE_ISEM_1;
    dh_overwrite_dbal_table_ids[17] = DBAL_PHYSICAL_TABLE_INLIF_3;
    dh_overwrite_dbal_table_ids[18] = DBAL_PHYSICAL_TABLE_ISEM_1;
    dh_overwrite_dbal_table_ids[19] = DBAL_PHYSICAL_TABLE_ISEM_2;

    for (mdb_physical_table_id_iter = 0; mdb_physical_table_id_iter < DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
         mdb_physical_table_id_iter++)
    {
        if (MDB_EM_TABLE_IS_EMP(mdb_physical_table_id_iter))
        {
            uint32 entry_handle_table_22;

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit,
                             dnx_data_mdb.em.mdb_emp_tables_info_get(unit, mdb_physical_table_id_iter)->mdb_22_table,
                             &entry_handle_table_22));

            dbal_entry_value_field32_set(unit, entry_handle_table_22, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, 0);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_table_22, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_table_22);
        }
    }
    for (dh_iter = 0; dh_iter < DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES; dh_iter++)
    {
        int dh_iter_inner;
        int dh_skip = FALSE;
        uint32 active_dhs_bitmap = 0;
        dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id =
            dnx_data_mdb.pdbs.pdb_info_get(unit, dh_overwrite_dbal_table_ids[dh_iter])->logical_to_physical;
        dbal_enum_value_field_mdb_db_type_e mdb_db_type;

        for (dh_iter_inner = 0; dh_iter_inner < dh_iter; dh_iter_inner++)
        {
            if (dh_overwrite_dbal_table_ids[dh_iter_inner] == dh_overwrite_dbal_table_ids[dh_iter])
            {
                dh_skip = TRUE;
                break;
            }
        }

        if (dh_skip == TRUE)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.pdbs.mdb_26_info_get(unit, mdb_physical_table_id)->dbal_table,
                         &entry_handle_id));

        for (dh_iter_inner = 0; dh_iter_inner < DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES; dh_iter_inner++)
        {
            if (dh_overwrite_dbal_table_ids[dh_iter_inner] == dh_overwrite_dbal_table_ids[dh_iter])
            {
                uint32 macro_serial_pos_of_pdb = dnx_data_mdb.dh.dh_info_get(unit,
                                                                             mdb_physical_table_id)->
                    table_macro_interface_mapping[dh_iter_inner];

                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&active_dhs_bitmap, macro_serial_pos_of_pdb));
            }
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, active_dhs_bitmap);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, active_dhs_bitmap);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_id);

        mdb_db_type = dnx_data_mdb.pdbs.pdb_info_get(unit, dh_overwrite_dbal_table_ids[dh_iter])->db_type;

        if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        {
            int nof_banks = dnx_data_mdb.em.mdb_29_info_get(unit, mdb_physical_table_id)->mdb_item_0_array_size;
            int bank_iter;

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, dnx_data_mdb.em.mdb_29_info_get(unit, mdb_physical_table_id)->dbal_table,
                             &entry_handle_id));
            for (bank_iter = 0; bank_iter < nof_banks; bank_iter++)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, bank_iter, 0x0);
            }
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_id);
        }
        else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, dnx_data_mdb.pdbs.mdb_11_info_get(unit, mdb_physical_table_id)->dbal_table,
                             &entry_handle_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, 0x0,
                                         dnx_data_mdb.dh.dh_info_get(unit,
                                                                     mdb_physical_table_id)->table_macro_interface_mapping
                                         [dh_iter]);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_id);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. Unexpected MDB db type %d.\n", mdb_db_type);
        }
    }

    for (global_macro_idx = 0; global_macro_idx < DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES; global_macro_idx++)
    {
        int cluster_idx;
        mdb_macro_types_e macro_type;
        int macro_local_idx;
        dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id =
            dnx_data_mdb.pdbs.pdb_info_get(unit, dh_overwrite_dbal_table_ids[global_macro_idx])->logical_to_physical;

        macro_type = dnx_data_mdb.dh.logical_macro_info_get(unit, global_macro_idx)->macro_type;
        macro_local_idx = dnx_data_mdb.dh.logical_macro_info_get(unit, global_macro_idx)->macro_index;

        for (cluster_idx = 0; cluster_idx < max_nof_macro_clusters; cluster_idx++)
        {
            int cluster_glob_index = (macro_local_idx * max_nof_macro_clusters) + cluster_idx;
            uint8 cluster_flags = 0;

            SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_assoc.
                            pdb.set(unit, macro_type, cluster_glob_index, mdb_physical_table_id));
            SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_assoc.
                            flags.set(unit, macro_type, cluster_glob_index, cluster_flags));
        }
    }

    for (dh_iter = 0; dh_iter < DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES; dh_iter++)
    {
        int cluster_iter;
        int cluster_interface_id;
        dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id =
            dnx_data_mdb.pdbs.pdb_info_get(unit, dh_overwrite_dbal_table_ids[dh_iter])->logical_to_physical;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.dh.mdb_1_info_get(unit, dh_iter)->dbal_table, &entry_handle_id));

        for (cluster_interface_id = 0; cluster_interface_id < dnx_data_mdb.dh.max_nof_cluster_interfaces_get(unit);
             cluster_interface_id++)
        {
            if (dnx_data_mdb.dh.macro_interface_mapping_get(unit, dh_iter)->interfaces[cluster_interface_id] ==
                mdb_physical_table_id)
            {
                break;
            }
        }

        for (cluster_iter = 0; cluster_iter < max_nof_macro_clusters; cluster_iter++)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, cluster_iter, 0x0);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, cluster_iter, 0x0);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_2, cluster_iter,
                                         cluster_interface_id);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_id);
    }

    SHR_IF_ERR_EXIT(mdb_dh_set_bpu_setting(unit));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_switch_control_mdb_exact_match_false_hit_le_and_pipe(
    int unit)
{
    uint32 entry_handle_id;
    uint32 ffc_instruction;
    uint32 ffc_bitmap_stage_a[2] = { 1, 0 };
    uint32 ffc_bitmap_stage_b[2] = { 4, 0 };
    uint32 ffc_bitmap_disable[2] = { 0, 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_VT1_KBR_INFO, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap_disable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 3, 4);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap_stage_a);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_VT1_FFC_INSTRUCTION, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FFC_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    ffc_instruction = 0x1 << 8;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VT1_FFC_LITERALLY, INST_SINGLE, ffc_instruction);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_VT23_RESOURCES_MAPPING, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_SELECTOR, 2,
                                 DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_A);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_GROUP_SELECTOR, 0,
                                 DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_A);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_SELECTOR, 0,
                                 DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_A);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_SELECTOR, 3,
                                 DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_B);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_GROUP_SELECTOR, 1,
                                 DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_B);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_SELECTOR, 1,
                                 DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_B);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_VT23_FFC_QUAD_IS_ACL, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FFC_QUAD_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_QUAD_IS_ACL, INST_SINGLE, FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_VT23_KBR_INFO, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap_disable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IS_ACL, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 2);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap_stage_a);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 0);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap_stage_a);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 3);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap_stage_b);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 1);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap_stage_b);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_VT23_FFC_INSTRUCTION, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FFC_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    ffc_instruction = 0x1 << 8;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VT23_FFC_LITERALLY, INST_SINGLE, ffc_instruction);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_VT23_PD_INFO, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_IDX, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_ORDER, INST_SINGLE, 6);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_IDX, 2);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_ORDER, INST_SINGLE, 6);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_IDX, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_ORDER, INST_SINGLE, 6);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_IDX, 2);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_ORDER, INST_SINGLE, 6);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_IRPP_VT2_CONTEXT_PROPERTIES, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_VT2_CONTEXT_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_ARR_RESOLUTION_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_IRPP_VT3_CONTEXT_PROPERTIES, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_VT3_CONTEXT_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_ARR_RESOLUTION_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_VT45_RESOURCES_MAPPING, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_SELECTOR, 2,
                                 DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_A);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_GROUP_SELECTOR, 0,
                                 DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_A);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_SELECTOR, 0,
                                 DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_A);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_SELECTOR, 3,
                                 DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_B);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_GROUP_SELECTOR, 1,
                                 DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_B);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_SELECTOR, 1,
                                 DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_B);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_VT45_FFC_QUAD_IS_ACL, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FFC_QUAD_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_QUAD_IS_ACL, INST_SINGLE, FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_VT45_KBR_INFO, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap_disable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IS_ACL, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 2);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap_stage_a);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 0);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap_stage_a);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 3);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap_stage_b);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 1);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap_stage_b);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_VT45_FFC_INSTRUCTION, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FFC_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    ffc_instruction = 0x1 << 8;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VT45_FFC_LITERALLY, INST_SINGLE, ffc_instruction);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_VT45_PD_INFO, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PD_IDX, 0, 0);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 2, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_ORDER, INST_SINGLE, 6);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PD_IDX, 2, 2);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 3, 3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_ORDER, INST_SINGLE, 6);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PD_IDX, 0, 0);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 0, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_ORDER, INST_SINGLE, 6);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PD_IDX, 2, 2);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_KBR_IDX, 1, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_ORDER, INST_SINGLE, 6);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_IRPP_VT4_CONTEXT_PROPERTIES, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_VT4_CONTEXT_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_ARR_RESOLUTION_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_IRPP_VT5_CONTEXT_PROPERTIES, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_VT5_CONTEXT_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_ARR_RESOLUTION_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_FWD12_RESOURCES_MAPPING, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_SELECTOR, INST_ALL,
                                 DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_A);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_GROUP_SELECTOR, INST_ALL,
                                 DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_A);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHR_PD_SELECTOR, INST_ALL,
                                 DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_A);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_FWD12_FFC_QUAD_IS_ACL, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FFC_QUAD_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_QUAD_IS_ACL, INST_SINGLE, FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_FWD12_KBR_INFO, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap_disable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IS_ACL, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, 6);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap_stage_a);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IS_ACL, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, 11);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap_stage_a);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBR_IS_ACL, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_FWD12_FFC_INSTRUCTION, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FFC_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    ffc_instruction = 0x1 << 8;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_FFC_LITERALLY, INST_SINGLE, ffc_instruction);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KLEAP_FWD12_PD_INFO, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PD_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_ORDER, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_IRPP_FWD1_CONTEXT_PROPERTIES, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FWD1_CONTEXT_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARNING_KEY_CONSTRUCTION_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARNING_FIXED_KEY_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LAYER_LB_KEY_CALCULATIONS_ENABLE, INST_SINGLE,
                                 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOAD_BALANCING_KEY_CONSTRUCTION_ENABLE, INST_SINGLE,
                                 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHERNET_SA_MC_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHERNET_SA_EQUAL_DA_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_VERSION_ERROR_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_CHECKSUM_ERROR_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_HAS_OPTIONS_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_SIP_EQUAL_DIP_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP_ZERO_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_SIP_IS_MC_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_VERSION_ERROR_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_UNSPECIFIED_DESTINATION_TRAP_ENABLE,
                                 INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_LOOPBACK_ADDRESS_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_MULTICAST_SOURCE_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_NEXT_HEADER_NULL_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_UNSPECIFIED_SOURCE_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_LOCAL_LINK_DESTINATION_TRAP_ENABLE, INST_SINGLE,
                                 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_LOCAL_SITE_DESTINATION_TRAP_ENABLE, INST_SINGLE,
                                 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_LOCAL_LINK_SOURCE_TRAP_DEST_ENABLE, INST_SINGLE,
                                 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_LOCAL_SITE_SOURCE_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_IPV4_COMPATIBLE_DESTINATION_TRAP_ENABLE,
                                 INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_IPV4_MAPPED_DESTINATION_TRAP_ENABLE,
                                 INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_MULTICAST_DESTINATION_TRAP_ENABLE, INST_SINGLE,
                                 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCP_SN_FLAGS_ZERO_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCP_SN_ZERO_FLAGS_SET_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCP_SYN_FIN_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCP_EQUAL_PORTS_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCP_FRAGMENT_INCOMPLETE_HEADER_TRAP_ENABLE,
                                 INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCP_FRAGMENT_OFFSET_LT_8_TRAP_ENABLE, INST_SINGLE,
                                 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDP_EQUAL_PORTS_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PHB_TC_DP_MAPPING_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PHB_ECN_MAPPING_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DSCP_REMARK_MAPPING_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESADI_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_MAPPING_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_TOTAL_LENGTH_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_LEARN_ALLOWED_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FCOE_FABRIC_PROVIDED_SECURITY_ENABLE, INST_SINGLE,
                                 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAVED_CONTEXT_PROFILE_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IDENTIFICATION_KEY_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_UNKNOWN_ADDRESS_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DO_DEFAULT_ETHERNET_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE, INST_SINGLE,
                                 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_TCAM_IDENTIFICATION_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IEEE_1588_IDENTIFICATION_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IEEE_1588_TCAM_IDENTIFICATION_ENABLE, INST_SINGLE,
                                 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_ADDITIONAL_INFO_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_PROCEDURE_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_RESOLUTION_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PCP_DEI_MAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHERNET_OAM_IDENTIFICATION_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_TP_OR_BFD_IDENTIFICATION_ENABLE, INST_SINGLE,
                                 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MULTI_HOP_BFD_IDENTIFICATION_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ONE_HOP_BFD_IDENTIFICATION_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_UPDATE_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RPF_UPDATE_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_LOOKUP_STATISTICS_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_LOOKUP_STATISTICS_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_DOMAIN_STATISTICS_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAVED_CONTEXT_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_SNP_STRENGTH, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_FWD_STRENGTH, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_ADDITIONAL_INFO, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAME_INTERFACE, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_IRPP_FWD2_CONTEXT_PROPERTIES, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OEM_0_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OEM_1_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OEM_2_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OEM_3_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAVED_CONTEXT_PROFILE_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_UNKNOWN_ADDRESS_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DO_DEFAULT_ETHERNET_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE, INST_SINGLE,
                                 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_CLASSIFICATION_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_RESOLUTION_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KAPS_RESOLUTION_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTROL_PATH_TRAPS_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_ADDITIONAL_INFO_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_PROCEDURE_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ELK_ERROR_TRAP_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_UPDATE_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RPF_UPDATE_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_LOOKUP_STATISTICS_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_LOOKUP_STATISTICS_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_DOMAIN_STATISTICS_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_0_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_1_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAVED_CONTEXT_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OEM_APP_DB_VAL_0, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OEM_APP_DB_VAL_1, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OEM_APP_DB_VAL_2, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OEM_APP_DB_VAL_3, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_ADDITIONAL_INFO, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_ACTION_PROFILE_VALUE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_CASCADED_PRIORITY_DECODER_ENABLE, INST_SINGLE,
                                 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RPF_CASCADED_PRIORITY_DECODER_ENABLE, INST_SINGLE,
                                 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_IRPP_VT1_CONTEXT_PROPERTIES, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_VT1_CONTEXT_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_LOOKUP_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_IRPP_VT2_CONTEXT_PROPERTIES, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_VT2_CONTEXT_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_LOOKUP_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_IRPP_VT3_CONTEXT_PROPERTIES, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_VT3_CONTEXT_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_LOOKUP_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_IRPP_VT4_CONTEXT_PROPERTIES, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_VT4_CONTEXT_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_LOOKUP_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_IRPP_VT5_CONTEXT_PROPERTIES, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_VT5_CONTEXT_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_LOOKUP_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_mdb_false_hit_build_packet(
    int unit,
    uint32 *in_pp_ports,
    uint32 *packet_0,
    uint32 *packet_1)
{
    int ire_packet_buffer_size_bits = 512;
    int current_packet_size_bits = 16;
    int start_offset_bits = ire_packet_buffer_size_bits - current_packet_size_bits;
    int length_bits = 0;

    SHR_FUNC_INIT_VARS(unit);

    
    
    length_bits = 8;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, in_pp_ports[0]));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, in_pp_ports[1]));
    start_offset_bits += length_bits;
    
    length_bits = 7;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, 0));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, 0));
    start_offset_bits += length_bits;
    
    length_bits = 1;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, 0));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, 0));
    start_offset_bits += length_bits;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_switch_control_mdb_exact_match_false_hit_send_packets(
    int unit,
    int verify)
{
    uint32 entry_handle_id;
    uint32 intf_iter = 0;
    uint32 dh_iter, cluster_iter;
    uint32 nof_mdb_lookup_intfs = 2;
    uint32 max_nof_macro_clusters = dnx_data_mdb.dh.max_nof_clusters_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (verify == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_ire_packet_start(unit));
        SHR_IF_ERR_EXIT(dnx_ire_packet_end(unit));
    }
    else
    {
        int verify_correct = TRUE;

        for (intf_iter = 0; intf_iter < nof_mdb_lookup_intfs * dnx_data_device.general.nof_cores_get(unit); intf_iter++)
        {
            uint32 cnt_mode =
                0x11 | ((intf_iter % nof_mdb_lookup_intfs) << 6) | ((1 - (intf_iter / nof_mdb_lookup_intfs)) << 5);

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_44, &entry_handle_id));
            for (dh_iter = 0; dh_iter < DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES; dh_iter++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_0, dh_iter);
                for (cluster_iter = 0; cluster_iter < max_nof_macro_clusters; cluster_iter++)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_1, cluster_iter);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, 0x0, cnt_mode);
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                }
            }
            DBAL_HANDLE_FREE(unit, entry_handle_id);

            SHR_IF_ERR_EXIT(dnx_ire_packet_start(unit));
            SHR_IF_ERR_EXIT(dnx_ire_packet_end(unit));

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_44, &entry_handle_id));
            for (dh_iter = 0; dh_iter < DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES; dh_iter++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_0, dh_iter);
                for (cluster_iter = 0; cluster_iter < max_nof_macro_clusters; cluster_iter++)
                {
                    uint32 cnt;

                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_1, cluster_iter);
                    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, &cnt);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

                    if (cnt == 0)
                    {
                        uint8 nof_macros_halved_type =
                            dnx_data_mdb.dh.macro_type_info_get(unit, MDB_MACRO_A)->nof_macros;
                        if ((dnx_data_mdb.dh.feature_get(unit, dnx_data_mdb_dh_macro_A_half_nof_clusters_enable))
                            && (dh_iter < nof_macros_halved_type)
                            && (cluster_iter >= dnx_data_mdb.dh.nof_bucket_clusters_get(unit)))
                        {
                            continue;
                        }

                        if ((dnx_data_mdb.dh.feature_get(unit, dnx_data_mdb_dh_macro_A_75_nof_clusters_enable))
                            && (dh_iter < nof_macros_halved_type)
                            && ((dnx_data_mdb.dh.mdb_75_macro_halved_get(unit, dh_iter))->macro_halved == TRUE)
                            && (cluster_iter >= dnx_data_mdb.dh.nof_bucket_clusters_get(unit)))
                        {
                            continue;
                        }

                        verify_correct = FALSE;

                        LOG_INFO(BSL_LOG_MODULE,
                                 (BSL_META_U
                                  (unit, "MDB EM counter not set, cnt_mode:0x%02x, DH: %2d, cluster: %d\n"),
                                  cnt_mode, dh_iter, cluster_iter));
                    }
                }
            }
            DBAL_HANDLE_FREE(unit, entry_handle_id);
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_44, &entry_handle_id));
        for (dh_iter = 0; dh_iter < DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES; dh_iter++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_0, dh_iter);
            for (cluster_iter = 0; cluster_iter < max_nof_macro_clusters; cluster_iter++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_1, cluster_iter);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, 0x0, 0x0);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
        DBAL_HANDLE_FREE(unit, entry_handle_id);

        if (verify_correct == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Error. MDB DH counter not set as expected, refer to LOG_INFO prints above for additional information.\n");
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_switch_control_mdb_exact_match_false_hit_config_send_packets(
    int unit,
    int verify)
{
    uint32 in_pp_ports[2] = { 0, 0 };
    uint32 in_tm_ports[2] = { 0, 0 };
    dnx_ire_packet_control_info_t packet_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&packet_info, 0, sizeof(dnx_ire_packet_control_info_t));
    SHR_IF_ERR_EXIT(dnx_mdb_false_hit_build_packet
                    (unit, in_pp_ports, packet_info.packet_data_0, packet_info.packet_data_1));

    packet_info.packet_mode = DNX_IRE_PACKET_MODE_SINGLE_SHOT;
    packet_info.nof_packets = 1;
    packet_info.valid_bytes = 64;
    packet_info.ptc_0 = in_tm_ports[0];
    if (dnx_data_device.general.nof_cores_get(unit) > 1)
    {
        packet_info.ptc_1 = in_tm_ports[1];
    }
    SHR_IF_ERR_EXIT(dnx_ire_packet_init(unit, &packet_info));

    SHR_IF_ERR_EXIT(dnx_switch_control_mdb_exact_match_false_hit_send_packets(unit, verify));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_control_mdb_exact_match_false_hit_set(
    int unit,
    int verify)
{
    uint8 run_journaling = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    if ((run_journaling) && (verify == FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_err_recovery_is_init_check_bypass(unit, TRUE));
    }

    if (run_journaling)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));
    }
    SHR_IF_ERR_EXIT(dnx_switch_control_mdb_exact_match_false_hit_mdb(unit));

    SHR_IF_ERR_EXIT(dnx_switch_control_mdb_exact_match_false_hit_le_and_pipe(unit));

    SHR_IF_ERR_EXIT(dnx_switch_control_mdb_exact_match_false_hit_config_send_packets(unit, verify));

exit:
    if (run_journaling)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));
    }

    if ((run_journaling) && (verify == FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_err_recovery_is_init_check_bypass(unit, FALSE));
    }

    SHR_FUNC_EXIT;
}

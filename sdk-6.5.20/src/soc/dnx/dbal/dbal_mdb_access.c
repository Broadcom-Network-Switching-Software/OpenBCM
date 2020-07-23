/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALACCESSDNX
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include "dbal_internal.h"
#include <soc/dnx/mdb.h>
#include <soc/dnx/field/tcam_access/tcam_access.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnx/dbal/dbal_dynamic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>


#define DBAL_MDB_ACCESS_APP_ID_GET(_unit, _entry_handle, _app_id)                                                 \
    if (DBAL_TABLE_IS_TCAM(_entry_handle->table))                                                                 \
    {                                                                                                             \
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.tcam_handler_id.get(unit, _entry_handle->table_id, &_app_id));     \
    }                                                                                                             \
    else                                                                                                          \
    {                                                                                                             \
        _app_id = _entry_handle->table->app_id;                                                                   \
    }




#define DBAL_MDB_ACCESS_NUM_ERPP_PROFILES_IN_ENTRY 4

#define DBAL_MDB_ACCESS_GREM_ZERO_PAD (dnx_data_lif.out_lif.glem_result_get(unit) - (DBAL_MDB_ACCESS_NUM_ERPP_PROFILES_IN_ENTRY * dnx_data_lif.out_lif.outrif_profile_width_get(unit)))

#define DBAL_MDB_ACCESS_ERPP_PROFILE_BIT_MASK   0xF

#define DBAL_MDB_ACCESS_GREM_INSTANCE_IN_ENTRY(global_rif) (global_rif % DBAL_MDB_ACCESS_NUM_ERPP_PROFILES_IN_ENTRY)

#define DBAL_MDB_ACCESS_ALIGNED_KEY(global_rif)    \
        (DBAL_MDB_ACCESS_GREM_INSTANCE_IN_ENTRY(global_rif) * dnx_data_lif.out_lif.outrif_profile_width_get(unit) + DBAL_MDB_ACCESS_GREM_ZERO_PAD)

#define DBAL_MDB_ACCESS_ALIGNED_PAYLOAD_GET(payload, global_rif) \
        payload = DBAL_MDB_ACCESS_ERPP_PROFILE_BIT_MASK & (payload >> DBAL_MDB_ACCESS_ALIGNED_KEY(global_rif))

#define DBAL_MDB_ACCESS_ALIGNED_PAYLOAD_SET(payload, global_rif)    \
        payload = payload << DBAL_MDB_ACCESS_ALIGNED_KEY(global_rif)

extern uint32 G_dbal_field_full_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];

static dbal_physical_table_actions_t mdb_empty_actions = {
    
    NULL,
    
    NULL,
    
    NULL,
    
    NULL,
    
    NULL,
    
    NULL,
    
    NULL,
    
    NULL,
    
    NULL,
    
    NULL
};

static dbal_physical_table_actions_t mdb_em_actions = {
    
    mdb_em_entry_add,
    
    mdb_em_entry_get,
    
    mdb_em_entry_delete,
    
    mdb_em_table_clear,
    
    NULL,
    
    NULL,
    
    NULL,
    
    mdb_em_iterator_init,
    
    mdb_em_iterator_get_next,
    
    mdb_em_iterator_deinit
};

static dbal_physical_table_actions_t mdb_direct_actions = {
    
    mdb_direct_table_entry_add,
    
    mdb_direct_table_entry_get,
    
    mdb_direct_table_entry_delete,
    
    mdb_direct_table_clear,
    
    NULL,
    
    NULL,
    
    NULL,
    
    mdb_direct_table_iterator_init,
    
    mdb_direct_table_iterator_get_next,
    
    mdb_direct_table_iterator_deinit
};

static dbal_physical_table_actions_t mdb_eedb_actions = {
    
    mdb_eedb_table_entry_add,
    
    mdb_eedb_table_entry_get,
    
    mdb_eedb_table_entry_delete,
    
    mdb_eedb_table_clear,
    
    NULL,
    
    NULL,
    
    NULL,
    
    mdb_eedb_table_iterator_init,
    
    mdb_eedb_table_iterator_get_next,
    
    mdb_eedb_table_iterator_deinit
};

static dbal_physical_table_actions_t mdb_tcam_actions = {
    
    dnx_field_tcam_access_entry_add,
    
    dnx_field_tcam_access_entry_get,
    
    dnx_field_tcam_access_entry_delete,
    
    NULL,
    
    NULL,
    
    NULL,
    
    NULL,
    
    dnx_field_tcam_access_iterator_init,
    
    dnx_field_tcam_access_iterator_get_next,
    
    dnx_field_tcam_access_iterator_deinit
};

static dbal_physical_table_actions_t mdb_lpm_actions = {
    
    mdb_lpm_entry_add,
    
    mdb_lpm_entry_get,
    
    mdb_lpm_entry_delete,
    
    mdb_lpm_table_clear,
    
    NULL,
    
    NULL,
    
    NULL,
    
    mdb_lpm_iterator_init,
    
    mdb_lpm_iterator_get_next,
    
    mdb_lpm_iterator_deinit
};

static dbal_physical_mngr_info_t physical_table_mngr = {
    {
     {
            
      "MDB TABLE NONE",
            
      DBAL_PHYSICAL_TABLE_NONE,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_empty_actions},
     {
            
      "MDB TABLE TCAM",
            
      DBAL_PHYSICAL_TABLE_TCAM,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_tcam_actions},
     {
            
      "MDB TABLE LPM PRIVATE",
            
      DBAL_PHYSICAL_TABLE_KAPS_1,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_lpm_actions},
     {
            
      "MDB TABLE LPM PUBLIC",
            
      DBAL_PHYSICAL_TABLE_KAPS_2,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_lpm_actions},
     {
            
      "MDB TABLE ISEM 1",
            
      DBAL_PHYSICAL_TABLE_ISEM_1,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE INLIF 1",
            
      DBAL_PHYSICAL_TABLE_INLIF_1,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_direct_actions},
     {
            
      "MDB TABLE IVSI",
            
      DBAL_PHYSICAL_TABLE_IVSI,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_direct_actions},
     {
            
      "MDB TABLE ISEM 2",
            
      DBAL_PHYSICAL_TABLE_ISEM_2,
            
      DBAL_CORE_MODE_DPC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE ISEM 3",
            
      DBAL_PHYSICAL_TABLE_ISEM_3,
            
      DBAL_CORE_MODE_DPC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE INLIF 2",
            
      DBAL_PHYSICAL_TABLE_INLIF_2,
            
      DBAL_CORE_MODE_DPC,
            
      &mdb_direct_actions},
     {
            
      "MDB TABLE INLIF 3",
            
      DBAL_PHYSICAL_TABLE_INLIF_3,
            
      DBAL_CORE_MODE_DPC,
            
      &mdb_direct_actions},
     {
            
      "MDB TABLE LEM",
            
      DBAL_PHYSICAL_TABLE_LEM,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE IOEM 0",
            
      DBAL_PHYSICAL_TABLE_IOEM_1,
            
      DBAL_CORE_MODE_DPC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE IOEM 1",
            
      DBAL_PHYSICAL_TABLE_IOEM_2,
            
      DBAL_CORE_MODE_DPC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE MAP",
            
      DBAL_PHYSICAL_TABLE_MAP,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_direct_actions},
     {
            
      "MDB TABLE FEC 1",
            
      DBAL_PHYSICAL_TABLE_FEC_1,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_direct_actions},
     {
            
      "MDB TABLE FEC 2",
            
      DBAL_PHYSICAL_TABLE_FEC_2,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_direct_actions},
     {
            
      "MDB TABLE FEC 3",
            
      DBAL_PHYSICAL_TABLE_FEC_3,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_direct_actions},
     {
            
      "MDB TABLE MC ID",
            
      DBAL_PHYSICAL_TABLE_PPMC,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE GLEM 0",
            
      DBAL_PHYSICAL_TABLE_GLEM_1,
            
      DBAL_CORE_MODE_DPC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE GLEM 1",
            
      DBAL_PHYSICAL_TABLE_GLEM_2,
            
      DBAL_CORE_MODE_DPC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE EEDB 1",
            
      DBAL_PHYSICAL_TABLE_EEDB_1,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_eedb_actions},
     {
            
      "MDB TABLE EEDB 2",
            
      DBAL_PHYSICAL_TABLE_EEDB_2,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_eedb_actions},
     {
            
      "MDB TABLE EEDB 3",
            
      DBAL_PHYSICAL_TABLE_EEDB_3,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_eedb_actions},
     {
            
      "MDB TABLE EEDB 4",
            
      DBAL_PHYSICAL_TABLE_EEDB_4,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_eedb_actions},
     {
            
      "MDB TABLE EEDB 5",
            
      DBAL_PHYSICAL_TABLE_EEDB_5,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_eedb_actions},
     {
            
      "MDB TABLE EEDB 6",
            
      DBAL_PHYSICAL_TABLE_EEDB_6,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_eedb_actions},
     {
            
      "MDB TABLE EEDB 7",
            
      DBAL_PHYSICAL_TABLE_EEDB_7,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_eedb_actions},
     {
            
      "MDB TABLE EEDB 8",
            
      DBAL_PHYSICAL_TABLE_EEDB_8,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_eedb_actions},
     {
            
      "MDB TABLE EOEM 0",
            
      DBAL_PHYSICAL_TABLE_EOEM_1,
            
      DBAL_CORE_MODE_DPC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE EOEM 1",
            
      DBAL_PHYSICAL_TABLE_EOEM_2,
            
      DBAL_CORE_MODE_DPC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE ESEM",
            
      DBAL_PHYSICAL_TABLE_ESEM,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE EVSI",
            
      DBAL_PHYSICAL_TABLE_EVSI,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_direct_actions},
     {
            
      "MDB TABLE EXEM 1",
            
      DBAL_PHYSICAL_TABLE_SEXEM_1,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE EXEM 2",
            
      DBAL_PHYSICAL_TABLE_SEXEM_2,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE EXEM 3",
            
      DBAL_PHYSICAL_TABLE_SEXEM_3,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE EXEM 4",
            
      DBAL_PHYSICAL_TABLE_LEXEM,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE RMEP",
            
      DBAL_PHYSICAL_TABLE_RMEP_EM,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_em_actions},
     {
            
      "MDB TABLE KBP",
            
      DBAL_PHYSICAL_TABLE_KBP,
            
      DBAL_CORE_MODE_SBC,
            
      &mdb_empty_actions}
     }
};

static shr_error_e
dbal_physical_entry_print(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_physical_tables_e physical_table_id,
    dbal_physical_entry_t * entry,
    uint32 app_id,
    uint8 print_only_key,
    char *action)
{
    int ii, bsl_severity;
    int key_size_in_words = BITS2WORDS(entry->key_size);
    int payload_size_in_words = BITS2WORDS(entry->payload_size);
    dbal_physical_table_def_t *PhysicalTable = NULL;
    int logger_action = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);

    if (bsl_severity >= bslSeverityInfo)
    {
        if (dbal_logger_is_enable(unit, entry_handle->table_id))
        {
            if ((!sal_strcasecmp(action, "get from")) || (!sal_strcasecmp(action, "get next from"))
                || (!sal_strcasecmp(action, "Access ID get")))
            {
                logger_action = 1;
            }
            else if (!sal_strcasecmp(action, "delete from"))
            {
                logger_action = 2;
            }
            else
            {
                logger_action = 0;
            }

            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_MDB, logger_action);

            
            SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_table_id, &PhysicalTable));
            LOG_CLI((BSL_META("Entry %s Physical Table: %s\n"), action, PhysicalTable->physical_name));

            LOG_CLI((BSL_META("Acccess ID %d\n"), entry->entry_hw_id));
            
            LOG_CLI((BSL_META("Phy. entry buffer:\n")));
            LOG_CLI((BSL_META("Key(%3d bits): 0x"), entry->key_size));
            for (ii = key_size_in_words - 1; ii >= 0; ii--)
            {
                LOG_CLI((BSL_META("%08x"), entry->key[ii]));
            }
            LOG_CLI((BSL_META("\n")));

            LOG_CLI((BSL_META("Key Mask     : 0x")));
            for (ii = key_size_in_words - 1; ii >= 0; ii--)
            {
                LOG_CLI((BSL_META("%08x"), entry->k_mask[ii]));
            }
            LOG_CLI((BSL_META("\n")));

            if (entry->prefix_length != 0)
            {
                
                LOG_CLI((BSL_META("Prefix length: %u\n"), entry->prefix_length));
            }

            if (!print_only_key)
            {
                
                LOG_CLI((BSL_META("Payload(%3d bits): 0x"), entry->payload_size));
                for (ii = payload_size_in_words - 1; ii >= 0; ii--)
                {
                    LOG_CLI((BSL_META("%08x"), entry->payload[ii]));
                }
                LOG_CLI((BSL_META("\n")));
                LOG_CLI((BSL_META("Payload Mask     : 0x")));
                for (ii = payload_size_in_words - 1; ii >= 0; ii--)
                {
                    LOG_CLI((BSL_META("%08x"), entry->p_mask[ii]));
                }
                LOG_CLI((BSL_META("\n")));
            }
            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_MDB, logger_action);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_outlif_sw_info_update_payload_size(
    int unit,
    dbal_tables_e table_id,
    int result_type,
    dbal_physical_entry_t * phy_entry,
    uint8 *rt_has_link_list,
    dbal_physical_tables_e * physical_table,
    uint8 *is_eedb_ll)
{
    lif_mngr_outlif_phase_e outlif_phase;
    dnx_algo_local_outlif_logical_phase_e logical_phase;
    int physical_phase;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get
                    (unit, phy_entry->key[0], NULL, NULL, &outlif_phase, NULL, is_eedb_ll, NULL));
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phase_enum_to_logical_phase_num(unit, outlif_phase, &logical_phase));
    physical_phase = (dnx_data_lif.out_lif.logical_to_physical_phase_map_get(unit, logical_phase + 1))->physical_phase;
    *physical_table = physical_phase + DBAL_PHYSICAL_TABLE_EEDB_1;

    if (!(*is_eedb_ll))
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_eedb_table_rt_has_linked_list(unit, table_id, result_type, rt_has_link_list));
        if (*rt_has_link_list)
        {
            phy_entry->payload_size -= MDB_DIRECT_BASIC_ENTRY_SIZE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_mdb_fec_tables_init(
    int unit)
{
    uint32 fec_hier_iter;

    SHR_FUNC_INIT_VARS(unit);
    for (fec_hier_iter = 0; fec_hier_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); fec_hier_iter++)
    {
        uint32 range_start, range_size, max_range = 0, min_range = 0;
        dbal_tables_e fec_table;

        if (fec_hier_iter == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1)
        {
            fec_table = DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY;
        }
        else if (fec_hier_iter == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2)
        {
            fec_table = DBAL_TABLE_SUPER_FEC_2ND_HIERARCHY;
        }
        else
        {
            fec_table = DBAL_TABLE_SUPER_FEC_3RD_HIERARCHY;
        }

        SHR_IF_ERR_EXIT(mdb_db_infos.fec_hierarchy_info.fec_id_range_size.get(unit, fec_hier_iter, &range_size));

        
        if (range_size > 0)
        {

            SHR_IF_ERR_EXIT(mdb_db_infos.fec_hierarchy_info.fec_id_range_start.get(unit, fec_hier_iter, &range_start));
            min_range = (range_start / dnx_data_mdb.direct.nof_fecs_in_super_fec_get(unit));
            max_range = (range_start + range_size - 1) / dnx_data_mdb.direct.nof_fecs_in_super_fec_get(unit);
        }

        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_update
                        (unit, fec_table, DBAL_FIELD_SUPER_FEC_ID, TRUE, 0, 0, DBAL_PREDEF_VAL_MIN_VALUE, min_range));
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_update
                        (unit, fec_table, DBAL_FIELD_SUPER_FEC_ID, TRUE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, max_range));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_init(
    int unit)
{
    int table_idx;
    dbal_physical_table_def_t *PhysicalTable;
    dbal_logical_table_t *table;
    
    int table_capacity[DBAL_NOF_PHYSICAL_TABLES];
    SHR_FUNC_INIT_VARS(unit);

    if (!sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(mdb_pre_init_step(unit));
    }

    for (table_idx = 0; table_idx < DBAL_NOF_PHYSICAL_TABLES; table_idx++)
    {
        SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, table_idx, &PhysicalTable));

        if (physical_table_mngr.physical_tables[table_idx].physical_db_type != table_idx)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR dbal_physical_table_init %d\n", table_idx);
        }

        if ((table_idx == DBAL_PHYSICAL_TABLE_TCAM) || (table_idx == DBAL_PHYSICAL_TABLE_KBP))
        {
            table_capacity[table_idx] = 0;
            if (!sw_state_is_warm_boot(unit))
            {
                SHR_IF_ERR_EXIT(mdb_db_infos.capacity.set(unit, table_idx, 0));
            }
            continue;
        }

        SHR_IF_ERR_EXIT(mdb_get_capacity(unit, table_idx, &table_capacity[table_idx]));

        if (!sw_state_is_warm_boot(unit))
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.set(unit, table_idx, table_capacity[table_idx]));
        }

        if (physical_table_mngr.physical_tables[table_idx].table_actions->table_init)
        {
            SHR_IF_ERR_EXIT(physical_table_mngr.physical_tables[table_idx].table_actions->table_init(unit));
        }
    }

    for (table_idx = 0; table_idx < DBAL_NOF_TABLES; table_idx++)
    {
        int jj;
        int max_capacity = 0;
        SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_idx, &table));

        if ((table->access_method != DBAL_ACCESS_METHOD_MDB) || DBAL_TABLE_IS_TCAM(table))
        {
            continue;
        }
        for (jj = 0; jj < table->nof_physical_tables; jj++)
        {
            max_capacity += table_capacity[table->physical_db_id[jj]];
        }
        table->max_capacity = max_capacity;
    }

    SHR_IF_ERR_EXIT(dbal_mdb_fec_tables_init(unit));

    if (!sw_state_is_warm_boot(unit))
    {
        
        SHR_IF_ERR_EXIT(dbal_flush_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_deinit(
    int unit)
{
    int table_idx;

    SHR_FUNC_INIT_VARS(unit);

    for (table_idx = 0; table_idx < DBAL_NOF_PHYSICAL_TABLES; table_idx++)
    {
        if (physical_table_mngr.physical_tables[table_idx].table_actions->table_deinit)
        {
            SHR_IF_ERR_EXIT(physical_table_mngr.physical_tables[table_idx].table_actions->table_deinit(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_physical_table_get(
    int unit,
    dbal_physical_tables_e physical_table_id,
    dbal_physical_table_def_t ** physical_table)
{
    SHR_FUNC_INIT_VARS(unit);

    if (physical_table_id >= DBAL_NOF_PHYSICAL_TABLES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR dbal_physical_table_get %d\n", physical_table_id);
    }
    *physical_table = &(physical_table_mngr.physical_tables[physical_table_id]);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_mdb_hw_payload_size_get(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    int *p_size)
{
    int payload_size;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (table->access_method != DBAL_ACCESS_METHOD_MDB)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "API can not called with table which is not MDB table. table %s.\n",
                     table->table_name);
    }

    if (res_type_idx < table->nof_result_types)
    {
        payload_size = table->multi_res_info[res_type_idx].entry_payload_size;
        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
        {
            payload_size -= table->multi_res_info[res_type_idx].results_info[0].field_nof_bits;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal result type index %d for table %s. num of result types is %d\n",
                     res_type_idx, table->table_name, table->nof_result_types);
    }

    *p_size = payload_size;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_by_mdb_phy_get(
    int unit,
    dbal_physical_tables_e phy_db_id,
    uint32 app_db_id,
    dbal_tables_e * table_id)
{
    int ii, jj, num_of_tables;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    num_of_tables = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);

    if (phy_db_id < DBAL_NOF_PHYSICAL_TABLES)
    {
        if (app_db_id < DBAL_NOF_APP_IDS_PER_PHYSICAL_TABLE)
        {
            for (ii = 0; ii < num_of_tables; ii++)
            {
                int is_table_active;
                SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, ii, &table));
                if (table->access_method != DBAL_ACCESS_METHOD_MDB)
                {
                    continue;
                }

                SHR_IF_ERR_EXIT(dbal_tables_is_table_active(unit, ii, &is_table_active));

                if (!is_table_active)
                {
                    continue;
                }

                if ((sal_strstr(table->table_name, "EXAMPLE") != NULL))
                {
                    continue;
                }

                for (jj = 0; jj < table->nof_physical_tables; jj++)
                {
                    if (table->physical_db_id[jj] == phy_db_id)
                    {
                        if (phy_db_id == DBAL_PHYSICAL_TABLE_TCAM)
                        {
                            if ((table->app_id == app_db_id) ||
                                ((table->app_id + 1 == app_db_id) && (table->key_size > 160)))
                            {
                                *table_id = (dbal_tables_e) ii;
                                SHR_EXIT();
                            }
                        }
                        else
                        {
                            uint32 app_id_1 = 0;
                            uint32 app_id_2 = 0;

                            app_id_1 = table->app_id & ((1 << table->app_id_size) - 1);
                            app_id_2 = app_db_id & ((1 << table->app_id_size) - 1);

                            if (app_id_1 == app_id_2)
                            {
                                *table_id = (dbal_tables_e) ii;
                                SHR_EXIT();
                            }
                        }
                    }
                }
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal AppDbId %d, Max Value is %d\n", app_db_id,
                         DBAL_NOF_APP_IDS_PER_PHYSICAL_TABLE - 1);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal phyDB %d, DBAL_NOF_PHYSICAL_TABLES=%d\n", phy_db_id,
                     DBAL_NOF_PHYSICAL_TABLES);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_id_and_result_type_from_etps_format_get(
    int unit,
    uint32 etps_format,
    dbal_tables_e * table_id,
    int *result_type)
{
    int ii, jj;
    CONST dbal_logical_table_t *table;
    dbal_table_status_e table_status;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_TABLES; ii++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, ii, &table_status));
        if (table_status == DBAL_TABLE_INCOMPATIBLE_IMAGE)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, ii, &table));
        if (table->access_method != DBAL_ACCESS_METHOD_MDB)
        {
            continue;
        }
        if ((!dbal_table_is_out_lif(table)) && (table->physical_db_id[0] != DBAL_PHYSICAL_TABLE_ESEM)
            && (table->physical_db_id[0] != DBAL_PHYSICAL_TABLE_SEXEM_2))
        {
            continue;
        }
        for (jj = 0; jj < table->nof_result_types; jj++)
        {
            if ((table->multi_res_info[jj].result_type_hw_value[0] == etps_format) &&
                (table->multi_res_info[jj].is_disabled == FALSE) &&
                (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW)))
            {
                *table_id = ii;
                *result_type = jj;
                SHR_EXIT();
            }
        }
    }
    *table_id = DBAL_NOF_TABLES;
    *result_type = 0;

exit:
    SHR_FUNC_EXIT;
}

uint8
dbal_table_is_in_lif(
    CONST dbal_logical_table_t * table)
{
    if ((table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_INLIF_1) ||
        ((table->physical_db_id[0] >= DBAL_PHYSICAL_TABLE_INLIF_2)
         && (table->physical_db_id[0] <= DBAL_PHYSICAL_TABLE_INLIF_3)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

uint8
dbal_table_is_out_lif(
    CONST dbal_logical_table_t * table)
{
    if (((table->physical_db_id[0] >= DBAL_PHYSICAL_TABLE_EEDB_1)
         && (table->physical_db_id[0] <= DBAL_PHYSICAL_TABLE_EEDB_8))
        && ((table->allocator_field_id == DBAL_FIELD_OUT_RIF) || (table->allocator_field_id == DBAL_FIELD_OUT_LIF)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

uint8
dbal_logical_table_is_fec(
    int unit,
    CONST dbal_logical_table_t * table)
{
    if ((table->physical_db_id[0] >= DBAL_PHYSICAL_TABLE_FEC_1)
        && (table->physical_db_id[0] <= DBAL_PHYSICAL_TABLE_FEC_3))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

uint8
dbal_logical_table_is_mact(
    int unit,
    dbal_tables_e table_id)
{
    uint8 is_std_1 = FALSE;
    int rv = 0;

    if (table_id == DBAL_TABLE_FWD_MACT)
    {
        rv = dnx_pp_prgm_default_image_check(unit, &is_std_1);

        if (!is_std_1 || rv != 0)
        {
            return FALSE;
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

shr_error_e
dbal_physical_table_app_id_is_mact(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    uint32 app_id,
    uint32 *is_mact)
{
    uint8 is_std_1 = FALSE;
    uint32 mact_app_id, mact_pcc_app_id;
    int app_db_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
    if (!is_std_1)
    {
        *is_mact = FALSE;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_FWD_MACT, &mact_app_id, &app_db_size));
    SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_PCC_STATE, &mact_pcc_app_id, &app_db_size));

    if ((dbal_physical_table == DBAL_PHYSICAL_TABLE_LEM) && ((app_id == mact_app_id) || (app_id == mact_pcc_app_id)))
    {
        *is_mact = TRUE;
    }
    else
    {
        *is_mact = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
static shr_error_e
dbal_mdb_mact_dynamic_entry_action_apply_update(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_handle, _SHR_E_EMPTY, "entry_handle");

    
    if (entry_handle->phy_entry.mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW)
    {
        entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_NONE;
    }
    else if (entry_handle->phy_entry.mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
    {
        entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_HW_ONLY;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_mact_dynamic_entry_check(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 *is_mact_dynamic)
{
    uint32 strength_value;
    dbal_table_field_info_t table_field_info;

    SHR_FUNC_INIT_VARS(unit);
    *is_mact_dynamic = FALSE;

    if (dbal_logical_table_is_mact(unit, entry_handle->table_id))
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                        (unit, entry_handle->table_id, DBAL_FIELD_MAC_STRENGTH, 0, entry_handle->cur_res_type, 0,
                         &table_field_info));
        SHR_IF_ERR_EXIT(dbal_field_from_buffer_get
                        (unit, &table_field_info, DBAL_FIELD_MAC_STRENGTH, entry_handle->phy_entry.payload,
                         &strength_value));
        if (strength_value <= DBAL_ENUM_FVAL_MAC_STRENGTH_1)
        {
            *is_mact_dynamic = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#endif


shr_error_e
dbal_mdb_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 resolve_from_sw)
{
    int jj;
    int curr_res_type = 0;
    uint32 field_value[1] = { 0 };
    dbal_logical_table_t *table = entry_handle->table;
    dbal_table_field_info_t table_field_info;
    int result_type_resolved = 0;

    SHR_FUNC_INIT_VARS(unit);

    
    if ((dbal_table_is_out_lif(table)) || (dbal_table_is_in_lif(table)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No need to do result type resolution for lif %s",
                     entry_handle->table->table_name);
    }

    dbal_logger_internal_disable_set(unit);
    if (resolve_from_sw != SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "MDB RT resolution, mismatch with result from SW flag. table = %s\n",
                     table->table_name);
    }

    while ((curr_res_type < table->nof_result_types) && (result_type_resolved == 0))
    {
        if (resolve_from_sw)
        {
            SHR_SET_CURRENT_ERR(dbal_sw_res_type_resolution(unit, entry_handle));
            curr_res_type = entry_handle->cur_res_type;
            result_type_resolved = 1;
        }
        else
        {
            
            if (entry_handle->table->multi_res_info[curr_res_type].is_disabled)
            {
                curr_res_type++;
                continue;
            }
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, entry_handle->table_id,
                                                       DBAL_FIELD_RESULT_TYPE, 0, curr_res_type, 0, &table_field_info));

            
            table_field_info.bits_offset_in_buffer =
                entry_handle->table->max_payload_size - table_field_info.field_nof_bits;

            SHR_IF_ERR_EXIT(dbal_field_from_buffer_get
                            (unit, &table_field_info, DBAL_FIELD_RESULT_TYPE, entry_handle->phy_entry.payload,
                             field_value));

            for (jj = 0; jj < entry_handle->table->multi_res_info[curr_res_type].result_type_nof_hw_values; jj++)
            {
                if (entry_handle->table->multi_res_info[curr_res_type].result_type_hw_value[jj] == field_value[0])
                {
                    entry_handle->cur_res_type = curr_res_type;
                    result_type_resolved = 1;
                    break;
                }
            }

            if (result_type_resolved)
            {
                break;
            }
            curr_res_type++;
        }
    }
    if (curr_res_type == table->nof_result_types)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    if (!SHR_FUNC_ERR())
    {
        if ((entry_handle->table->multi_res_info[curr_res_type].entry_payload_size <
             entry_handle->table->max_payload_size))
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_phy_entry_align
                            (unit,
                             entry_handle->table->max_payload_size -
                             entry_handle->table->multi_res_info[curr_res_type].entry_payload_size, TRUE,
                             entry_handle));
        }
    }

exit:
    dbal_logger_internal_disable_clear(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    if (table_id >= dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal table_id %d, max=%d\n", table_id,
                     dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit));
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_id, &table));

    if (physical_tbl_index >= table->nof_physical_tables)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "physical_tbl_index (%d) is out of range. max for %s is %d\n",
                     physical_tbl_index, table->table_name, table->nof_physical_tables);
    }
    *physical_table_id = table->physical_db_id[physical_tbl_index];

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_physical_table_def_t *PhysicalTable = NULL;
    dbal_physical_tables_e physical_db_id;
    uint32 app_id;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_iterator_init(unit, entry_handle));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, entry_handle->table_id,
                                                       iterator_info->physical_db_index, &physical_db_id));

        sal_memset(&iterator_info->mdb_iterator, 0x0, sizeof(dbal_physical_entry_iterator_t));

        SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));

        if (PhysicalTable->table_actions->iterator_init == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR missing implementation for %s\n", PhysicalTable->physical_name);
        }
        DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

        SHR_IF_ERR_EXIT(PhysicalTable->table_actions->iterator_init(unit, physical_db_id, app_id,
                                                                    &iterator_info->mdb_iterator));

        if (table->allocator_field_id != DBAL_FIELD_EMPTY)
        {
            dbal_table_field_info_t table_field_info = { 0 };

            SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                            (unit, entry_handle->table_id, table->allocator_field_id, 1, entry_handle->cur_res_type, 0,
                             &table_field_info));
            
            iterator_info->max_num_of_iterations = table_field_info.max_value;
        }
        if (entry_handle->table_id == DBAL_TABLE_GLOBAL_RIF_EM)
        {
            iterator_info->max_num_of_iterations = dnx_data_l3.rif.nof_rifs_get(unit);;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_iterator_rules_shadow_set(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 actions_bitmap)
{
    int ii, rule_counter = 0;
    int nof_result_types, nof_flush_rules;
    uint8 has_rt_rule = FALSE;
    int result_type_rule_value = 0;
    dbal_flush_shadow_info_t *flush_shadow = NULL;
    dbal_iterator_info_t *iterator_info;
    dbal_iterator_attribute_info_t *iterator_attrib_info;
    uint8 has_result_rule_or_action = FALSE;
    uint8 has_result_rule_or_action_on_aligned_fields_only = TRUE;
    uint32 mdb_entry_size_bitmap = 0;
    uint8 rule_per_entry_encoding = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));
    iterator_attrib_info = &iterator_info->attrib_info;

    nof_result_types = entry_handle->table->nof_result_types;
    nof_flush_rules = iterator_info->mdb_iterator.nof_flush_rules;

    
    SHR_ALLOC_SET_ZERO(flush_shadow, sizeof(dbal_flush_shadow_info_t) * nof_flush_rules,
                       "Flush shadow allocation", "%s%s%s\r\n", entry_handle->table->table_name, EMPTY, EMPTY);

    for (ii = 0; ii < iterator_info->nof_val_rules; ii++)
    {
        has_result_rule_or_action = TRUE;
        if ((iterator_info->val_rules_info[ii].field_id != DBAL_FIELD_DESTINATION) &&
            (iterator_info->val_rules_info[ii].field_id != DBAL_FIELD_MAC_STRENGTH) &&
            (iterator_info->val_rules_info[ii].field_id != DBAL_FIELD_ENTRY_GROUPING))
        {
            has_result_rule_or_action_on_aligned_fields_only = FALSE;
            break;
        }
    }

    for (ii = 0; ii < iterator_info->nof_actions; ii++)
    {
        if (iterator_info->actions_info[ii].action_type == DBAL_ITER_ACTION_UPDATE)
        {
            has_result_rule_or_action = TRUE;

            if ((iterator_info->actions_info[ii].field_id != DBAL_FIELD_DESTINATION) &&
                (iterator_info->actions_info[ii].field_id != DBAL_FIELD_MAC_STRENGTH) &&
                (iterator_info->actions_info[ii].field_id != DBAL_FIELD_ENTRY_GROUPING))
            {
                has_result_rule_or_action_on_aligned_fields_only = FALSE;
                break;
            }
        }
    }

    if (!has_result_rule_or_action)
    {
        has_result_rule_or_action_on_aligned_fields_only = FALSE;
    }

    
    for (ii = 0; ii < iterator_info->nof_val_rules; ii++)
    {
        if (iterator_info->val_rules_info[ii].field_id == DBAL_FIELD_RESULT_TYPE)
        {
            if (!has_rt_rule)
            {
                has_rt_rule = TRUE;
                result_type_rule_value = iterator_info->val_rules_info[ii].value[0];
                if (iterator_info->mdb_iterator.nof_flush_rules != 1)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found result type rule, but more than a single flush rule.\n");
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found two result type rules.\n");
            }
        }
    }

    rule_per_entry_encoding = (has_result_rule_or_action && has_result_rule_or_action_on_aligned_fields_only) ||
        (!has_result_rule_or_action);
    
    for (ii = 0; ii < nof_result_types; ii++)
    {
        int jj;
        uint8 field_not_exists = FALSE;
        uint32 build_rules_entry_handle_id = 0;
        dbal_access_condition_info_t *key_rules, *value_rules;
        dbal_iterator_actions_info_t *actions;
        dbal_entry_handle_t *build_rules_entry_handle;

        if (!utilex_bitstream_test_bit(&iterator_info->mdb_iterator.result_types_in_flush_bitmap, ii))
        {
            continue;
        }

        if (rule_per_entry_encoding)
        {
            
            mdb_em_entry_encoding_e entry_encoding = MDB_EM_NOF_ENTRY_ENCODINGS;
            
            SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                            (unit, DBAL_PHYSICAL_TABLE_LEM, entry_handle->table->key_size,
                             entry_handle->table->multi_res_info[ii].entry_payload_size, entry_handle->table->app_id,
                             entry_handle->table->app_id_size, &entry_encoding));

            if (!utilex_bitstream_test_bit(&mdb_entry_size_bitmap, entry_encoding))
            {
                utilex_bitstream_set_bit(&mdb_entry_size_bitmap, entry_encoding);
            }
            else
            {
                continue;
            }
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, entry_handle->table_id, &build_rules_entry_handle_id));

        key_rules = iterator_info->key_rules_info;
        value_rules = iterator_info->val_rules_info;
        actions = iterator_info->actions_info;

        
        for (jj = 0; jj < iterator_info->nof_key_rules; jj++)
        {
            dbal_entry_key_field_set(unit, build_rules_entry_handle_id, key_rules[jj].field_id, key_rules[jj].value,
                                     key_rules[jj].mask, DBAL_POINTER_TYPE_ARR_UINT32);
        }

        
        if (has_result_rule_or_action)
        {
            if (!has_rt_rule)
            {
                uint32 rt_value[1] = { 0 };
                rt_value[0] = ii;
                dbal_entry_value_field_set(unit, build_rules_entry_handle_id, DBAL_FIELD_RESULT_TYPE,
                                           0, DBAL_POINTER_TYPE_ARR_UINT32, rt_value, G_dbal_field_full_mask);
            }
            else if (ii != result_type_rule_value)
            {
                continue;
            }
        }

        for (jj = 0; jj < iterator_info->nof_val_rules; jj++)
        {
            int rv;
            dbal_table_field_info_t field_info;

            
            rv = dbal_tables_field_info_get_no_err(unit, entry_handle->table_id, value_rules[jj].field_id,
                                                   0, ii, 0, &field_info);
            if (rv == _SHR_E_NOT_FOUND)
            {
                field_not_exists = TRUE;
                continue;
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            dbal_entry_value_field_set(unit, build_rules_entry_handle_id, value_rules[jj].field_id,
                                       0, DBAL_POINTER_TYPE_ARR_UINT32, value_rules[jj].value, value_rules[jj].mask);
        }

        if (field_not_exists)
        {
            continue;
        }

        flush_shadow[rule_counter].key_rule_size = entry_handle->table->key_size;
        flush_shadow[rule_counter].value_rule_size = entry_handle->table->multi_res_info[ii].entry_payload_size;
        
        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, build_rules_entry_handle_id, &build_rules_entry_handle));
        sal_memcpy(flush_shadow[rule_counter].key_rule_buf, build_rules_entry_handle->phy_entry.key,
                   DBAL_PHYSICAL_KEY_SIZE_IN_BYTES);
        sal_memcpy(flush_shadow[rule_counter].key_rule_mask_buf, build_rules_entry_handle->phy_entry.k_mask,
                   DBAL_PHYSICAL_KEY_SIZE_IN_BYTES);

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(build_rules_entry_handle->phy_entry.payload, 0,
                                                       entry_handle->table->multi_res_info[ii].entry_payload_size,
                                                       flush_shadow[rule_counter].value_rule_buf));

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(build_rules_entry_handle->phy_entry.p_mask, 0,
                                                       entry_handle->table->multi_res_info[ii].entry_payload_size,
                                                       flush_shadow[rule_counter].value_rule_mask_buf));

        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, entry_handle->table_id, build_rules_entry_handle_id));
        for (jj = 0; jj < iterator_info->nof_actions; jj++)
        {
            int rv;
            dbal_table_field_info_t field_info;

            if (actions[jj].field_id == DBAL_FIELD_RESULT_TYPE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal action for field RESULT_TYPE\n");
            }
            rv = dbal_tables_field_info_get_no_err(unit, entry_handle->table_id, actions[jj].field_id,
                                                   0, ii, 0, &field_info);
            if (rv == _SHR_E_NOT_FOUND)
            {
                continue;
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            if (jj == 0)
            {
                uint32 rt_value[1] = { 0 };
                rt_value[0] = ii;
                dbal_entry_value_field_set(unit, build_rules_entry_handle_id, DBAL_FIELD_RESULT_TYPE,
                                           0, DBAL_POINTER_TYPE_ARR_UINT32, rt_value, G_dbal_field_full_mask);
            }
            dbal_entry_value_field_set(unit, build_rules_entry_handle_id, actions[jj].field_id,
                                       0, DBAL_POINTER_TYPE_ARR_UINT32, actions[jj].value, actions[jj].mask);
        }

        
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(build_rules_entry_handle->phy_entry.payload, 0,
                                                       entry_handle->table->multi_res_info[ii].entry_payload_size,
                                                       flush_shadow[rule_counter].value_action_buf));

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(build_rules_entry_handle->phy_entry.p_mask, 0,
                                                       entry_handle->table->multi_res_info[ii].entry_payload_size,
                                                       flush_shadow[rule_counter].value_action_mask_buf));

        
        if (iterator_attrib_info->hit_bit_rule_valid)
        {
            flush_shadow[rule_counter].hit_indication_rule_mask = TRUE;
            flush_shadow[rule_counter].hit_indication_rule = iterator_attrib_info->hit_bit_rule_is_hit;
        }

        
        flush_shadow[rule_counter].action_types_btmp = actions_bitmap & 0xF; 
        flush_shadow[rule_counter].value_action_size = entry_handle->table->max_payload_size;
        
        if (actions_bitmap & DBAL_ITER_ACTION_READ_TO_DMA)
        {
            flush_shadow[rule_counter].action_types_btmp |= DBAL_ITER_ACTION_GET;
        }

        if (rule_per_entry_encoding)
        {
            
            SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range
                            (flush_shadow[rule_counter].value_rule_mask_buf,
                             entry_handle->table->multi_res_info[ii].entry_payload_size -
                             entry_handle->table->multi_res_info[ii].results_info[0].field_nof_bits,
                             entry_handle->table->multi_res_info[ii].entry_payload_size - 1));
            SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range
                            (flush_shadow[rule_counter].value_action_mask_buf,
                             entry_handle->table->multi_res_info[ii].entry_payload_size -
                             entry_handle->table->multi_res_info[ii].results_info[0].field_nof_bits,
                             entry_handle->table->multi_res_info[ii].entry_payload_size - 1));
        }

        rule_counter++;
    }
    SHR_IF_ERR_EXIT(dbal_flush_rules_set(unit, entry_handle->table->app_id, rule_counter, flush_shadow));

exit:
    SHR_FREE(flush_shadow);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_iterator_is_flush_machine(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int ii, result_type_value_in_rule = 0;
    uint8 has_rt_rule = FALSE, use_flush = TRUE;
    dbal_iterator_info_t *iterator_info;
    dbal_physical_entry_iterator_t *mdb_iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));
    mdb_iterator_info = &iterator_info->mdb_iterator;

    mdb_iterator_info->result_types_in_flush_bitmap = 0;
    mdb_iterator_info->nof_flush_rules = 0;

    
    if (!dbal_logical_table_is_mact(unit, entry_handle->table_id))
    {
        use_flush = FALSE;
    }

    
    for (ii = 0; (ii < iterator_info->nof_key_rules) && use_flush; ii++)
    {
        if (iterator_info->key_rules_info[ii].type != DBAL_CONDITION_EQUAL_TO)
        {
            use_flush = FALSE;
        }
    }

    
    for (ii = 0; (ii < iterator_info->nof_val_rules) && use_flush; ii++)
    {
        if (iterator_info->val_rules_info[ii].type != DBAL_CONDITION_EQUAL_TO)
        {
            use_flush = FALSE;
        }
    }
    if (entry_handle->table->key_size > dnx_data_mdb.em.flush_max_supported_key_get(unit))
    {
        use_flush = FALSE;
    }

    for (ii = 0; ii < entry_handle->table->nof_result_types; ii++)
    {
        if (use_flush)
        {
            int result_size = entry_handle->table->multi_res_info[ii].entry_payload_size;
            if ((result_size <= dnx_data_mdb.em.flush_max_supported_payload_get(unit)) &&
                ((entry_handle->table->key_size + result_size) <=
                 dnx_data_mdb.em.flush_max_supported_key_plus_payload_get(unit)))
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&mdb_iterator_info->result_types_in_flush_bitmap, ii));
                mdb_iterator_info->nof_flush_rules++;
            }
            else
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&mdb_iterator_info->result_types_in_non_flush_bitmap, ii));
                mdb_iterator_info->nof_non_flush_rules++;
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&mdb_iterator_info->result_types_in_non_flush_bitmap, ii));
            mdb_iterator_info->nof_non_flush_rules++;
        }
    }

    
    for (ii = 0; ii < iterator_info->nof_val_rules; ii++)
    {
        if (iterator_info->val_rules_info[ii].field_id == DBAL_FIELD_RESULT_TYPE)
        {
            if (has_rt_rule == FALSE)
            {
                
                has_rt_rule = TRUE;

                
                result_type_value_in_rule = iterator_info->val_rules_info[ii].value[0];

                if (use_flush
                    && utilex_bitstream_test_bit(&mdb_iterator_info->result_types_in_flush_bitmap,
                                                 result_type_value_in_rule))
                {
                    mdb_iterator_info->nof_flush_rules = 1;
                    mdb_iterator_info->result_types_in_flush_bitmap = (1 << ii);
                    mdb_iterator_info->nof_non_flush_rules = 0;
                    mdb_iterator_info->result_types_in_non_flush_bitmap = 0;
                }
                else
                {
                    mdb_iterator_info->nof_flush_rules = 0;
                    mdb_iterator_info->result_types_in_flush_bitmap = 0;
                    mdb_iterator_info->nof_non_flush_rules = 1;
                    mdb_iterator_info->result_types_in_non_flush_bitmap = (1 << ii);
                }
            }
            else if (use_flush)
            {
                
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Two Result type rules with different values cannot be set. val1=%d, val2=%d",
                             result_type_value_in_rule, iterator_info->val_rules_info[ii].value[0]);
            }
        }
    }

    if (mdb_iterator_info->nof_flush_rules == 0)
    {
        use_flush = FALSE;
    }

    if (dnx_data_l2.general.feature_get(unit, dnx_data_l2_general_flush_machine_support))
    {
        mdb_iterator_info->iterate_in_flush_machine = use_flush;
    }
    else
    {
        mdb_iterator_info->iterate_in_flush_machine = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_mdb_global_rif_add(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_physical_table_def_t * physical_table,
    uint32 app_id,
    dbal_physical_tables_e * physical_db_ids,
    uint32 entry_flags)
{
    int global_rif, profile_width;
    dbal_table_field_info_t field_info;
    dbal_entry_handle_t *tmp_entry_handle = NULL;
    int entry_exists;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(tmp_entry_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);

    sal_memcpy(tmp_entry_handle, entry_handle, sizeof(*entry_handle));
    global_rif = entry_handle->phy_entry.key[0];
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, entry_handle->table_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE, 0, entry_handle->cur_res_type, 0,
                     &field_info));

    profile_width = field_info.field_nof_bits;
    
    tmp_entry_handle->phy_entry.payload_size = dnx_data_lif.out_lif.glem_result_get(unit);

    
    if (dnx_data_lif.global_lif.glem_rif_optimization_enabled_get(unit))
    {
        int idx, entry_base = global_rif - (global_rif % profile_width);
        tmp_entry_handle->phy_entry.payload_size = dnx_data_lif.out_lif.glem_result_get(unit);

        for (idx = 0; idx < 4; idx++)
        {
            tmp_entry_handle->phy_entry.key[0] = entry_base + idx;
            SHR_IF_ERR_EXIT(dbal_actions_optimized_is_exists(unit, tmp_entry_handle, &entry_exists));
            if (entry_exists)
            {
                break;
            }
        }
        tmp_entry_handle->phy_entry.key[0] = global_rif - (global_rif % profile_width);
        if (!entry_exists)
        {
            
            tmp_entry_handle->phy_entry.payload[0] =
                tmp_entry_handle->phy_entry.payload[0] << (DBAL_MDB_ACCESS_ALIGNED_KEY(global_rif));
        }
        else                                                                                                                                                                                                                 
        {
            uint32 payload = tmp_entry_handle->phy_entry.payload[0];
            int start_bit = DBAL_MDB_ACCESS_ALIGNED_KEY(global_rif);

            
            SHR_IF_ERR_EXIT(physical_table->
                            table_actions->entry_get(unit, physical_db_ids[0], app_id, &tmp_entry_handle->phy_entry,
                                                     0));
            
            SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range
                            (tmp_entry_handle->phy_entry.payload, start_bit,
                             start_bit + dnx_data_lif.out_lif.outrif_profile_width_get(unit)));

            
            tmp_entry_handle->phy_entry.payload[0] |= (payload << (DBAL_MDB_ACCESS_ALIGNED_KEY(global_rif)));
        }
    }
    else
    {
        
        DBAL_MDB_ACCESS_ALIGNED_PAYLOAD_SET(tmp_entry_handle->phy_entry.payload[0], global_rif);
    }
    tmp_entry_handle->phy_entry.indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_FORCE;
    SHR_IF_ERR_EXIT(physical_table->
                    table_actions->entry_add(unit, physical_db_ids, app_id, &tmp_entry_handle->phy_entry, entry_flags));

exit:
    SHR_FREE(tmp_entry_handle);
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_mdb_global_rif_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_physical_table_def_t * physical_table,
    uint32 app_id,
    dbal_physical_tables_e * physical_db_id,
    uint32 entry_flags)
{
    int global_rif;
    int entry_exists;
    int payload_size = entry_handle->phy_entry.payload_size;

    SHR_FUNC_INIT_VARS(unit);

    global_rif = entry_handle->phy_entry.key[0];

    
    SHR_IF_ERR_EXIT(dbal_actions_optimized_is_exists(unit, entry_handle, &entry_exists));
    if (!entry_exists)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    
    entry_handle->phy_entry.payload_size = dnx_data_lif.out_lif.glem_result_get(unit);
    if (dnx_data_lif.global_lif.glem_rif_optimization_enabled_get(unit))
    {
        entry_handle->phy_entry.key[0] = global_rif - DBAL_MDB_ACCESS_GREM_INSTANCE_IN_ENTRY(global_rif);
    }

    
    SHR_IF_ERR_EXIT(physical_table->table_actions->entry_get(unit, physical_db_id[0], app_id, &entry_handle->phy_entry,
                                                             entry_flags));
    
    DBAL_MDB_ACCESS_ALIGNED_PAYLOAD_GET(entry_handle->phy_entry.payload[0], global_rif);

exit:
    entry_handle->phy_entry.key[0] = global_rif;
    entry_handle->phy_entry.payload_size = payload_size;
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_mdb_global_rif_delete(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_physical_table_def_t * physical_table,
    uint32 app_id,
    dbal_physical_tables_e * physical_db_id,
    uint32 entry_flags)
{
    int global_rif, profile_width;
    dbal_table_field_info_t field_info;
    int entry_exists;
    int payload_size = entry_handle->phy_entry.payload_size;

    SHR_FUNC_INIT_VARS(unit);

    global_rif = entry_handle->phy_entry.key[0];
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, entry_handle->table_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE, 0, entry_handle->cur_res_type, 0,
                     &field_info));

    profile_width = field_info.field_nof_bits;

    
    SHR_IF_ERR_EXIT(dbal_actions_optimized_is_exists(unit, entry_handle, &entry_exists));
    if (!entry_exists)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    
    entry_handle->phy_entry.payload_size = dnx_data_lif.out_lif.glem_result_get(unit);
    if (dnx_data_lif.global_lif.glem_rif_optimization_enabled_get(unit))
    {
        entry_handle->phy_entry.key[0] = global_rif - DBAL_MDB_ACCESS_GREM_INSTANCE_IN_ENTRY(global_rif);
    }

    
    if (dnx_data_lif.global_lif.glem_rif_optimization_enabled_get(unit))
    {
        dbal_entry_handle_t tmp_entry_handle;
        int num_entries = 0;
        int idx, entry_base = global_rif - DBAL_MDB_ACCESS_GREM_INSTANCE_IN_ENTRY(global_rif);

        sal_memcpy(&tmp_entry_handle, entry_handle, sizeof(*entry_handle));
        tmp_entry_handle.phy_entry.payload_size = dnx_data_lif.out_lif.glem_result_get(unit);

        
        for (idx = 0; idx < 4; idx++)
        {
            tmp_entry_handle.phy_entry.key[0] = entry_base + idx;
            SHR_IF_ERR_EXIT(dbal_actions_optimized_is_exists(unit, &tmp_entry_handle, &entry_exists));
            if (entry_exists)
            {
                num_entries++;
            }
        }
        
        if (num_entries == 1)
        {
            SHR_IF_ERR_EXIT(physical_table->
                            table_actions->entry_delete(unit, physical_db_id, app_id, &entry_handle->phy_entry,
                                                        entry_flags));
        }
        
        else
        {
            
            tmp_entry_handle.phy_entry.key[0] = global_rif - DBAL_MDB_ACCESS_GREM_INSTANCE_IN_ENTRY(global_rif);
            SHR_IF_ERR_EXIT(physical_table->
                            table_actions->entry_get(unit, physical_db_id[0], app_id, &tmp_entry_handle.phy_entry, 0));
            SHR_IF_ERR_EXIT(utilex_bitstream_clear
                            (&tmp_entry_handle.phy_entry.payload[0] + DBAL_MDB_ACCESS_ALIGNED_KEY(global_rif),
                             profile_width));

            tmp_entry_handle.phy_entry.indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_UPDATE;
            SHR_IF_ERR_EXIT(physical_table->
                            table_actions->entry_add(unit, physical_db_id, app_id, &tmp_entry_handle.phy_entry,
                                                     entry_flags));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(physical_table->
                        table_actions->entry_delete(unit, physical_db_id, app_id, &entry_handle->phy_entry,
                                                    entry_flags));
    }

exit:
    entry_handle->phy_entry.key[0] = global_rif;
    entry_handle->phy_entry.payload_size = payload_size;
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_access_get_next(
    int unit,
    uint32 app_id,
    dbal_physical_tables_e physical_db_id,
    dbal_entry_handle_t * entry_handle,
    dbal_physical_entry_t * phy_entry,
    dbal_iterator_info_t * iterator_info)
{
    uint8 entry_found = FALSE;
    dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;
    dbal_physical_table_def_t *PhysicalTable = NULL;
    dbal_logical_table_t *table = entry_handle->table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));

    while (!entry_found && !iterator_info->is_end)
    {
        
        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_entry_get_next(unit, entry_handle));
            iterator_info->used_first_key = 1;
            if (!iterator_info->is_end)
            {
                SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get(unit, entry_handle));
                entry_found = TRUE;
                if (table->core_mode == DBAL_CORE_MODE_DPC)
                {
                    iterator_info->physical_db_index = entry_handle->core_id;
                }
            }
        }
        else if (table->allocator_field_id != DBAL_FIELD_EMPTY)
        {
            
            int key_bigger_then_capacity = 0;
            entry_found = TRUE;
            if (iterator_info->used_first_key)
            {
                entry_handle->phy_entry.key[0]++;
                if (entry_handle->phy_entry.key[0] > iterator_info->max_num_of_iterations)
                {
                    key_bigger_then_capacity = 1;
                    entry_found = FALSE;
                }
            }
            iterator_info->used_first_key = 1;

            if (!key_bigger_then_capacity)
            {
                if ((dbal_table_is_out_lif(table)) || (dbal_table_is_in_lif(table)))
                {
                    entry_handle->cur_res_type = DBAL_RESULT_TYPE_NOT_INITIALIZED;
                }
                SHR_IF_ERR_EXIT(dbal_iterator_increment_by_allocator(unit, entry_handle, &is_valid_entry));
            }

            if ((is_valid_entry == DBAL_KEY_IS_VALID) && (entry_found))
            {
                int rv = dbal_mdb_table_entry_get(unit, entry_handle);
                if (!rv)
                {
                    if (sal_memcmp
                        (zero_buffer_to_compare, entry_handle->phy_entry.payload,
                         DBAL_TABLE_BUFFER_IN_BYTES(entry_handle->table)) == 0)
                    {
                        
                        entry_found = FALSE;
                    }
                    else
                    {
                        entry_found = TRUE;
                    }
                }
                else
                {
                    if (rv == _SHR_E_NOT_FOUND)
                    {
                        
                        entry_found = FALSE;
                    }
                    else
                    {
                        
                        SHR_ERR_EXIT(rv, "internal error in entry get");
                    }
                }
            }
            else
            {
                iterator_info->is_end = 1;
            }
        }
        else
        {
            
            if (PhysicalTable->table_actions->iterator_get_next == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR missing implementation for %s\n", PhysicalTable->physical_name);
            }
            if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
            {
                entry_handle->cur_res_type = 0;
            }

            SHR_IF_ERR_EXIT(PhysicalTable->
                            table_actions->iterator_get_next(unit, physical_db_id, app_id, &iterator_info->mdb_iterator,
                                                             &entry_handle->phy_entry, &iterator_info->is_end));
            iterator_info->used_first_key = 1;
            if (!iterator_info->is_end)
            {
                if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
                {
                    int rv;
                    
                    rv = dbal_mdb_res_type_resolution(unit, entry_handle, FALSE);
                    if (rv == _SHR_E_NONE)
                    {
                        entry_found = TRUE;
                    }
                    else if (rv != _SHR_E_NOT_FOUND)
                    {
                        SHR_IF_ERR_EXIT(rv);
                    }
                }
                else
                {
                    entry_found = TRUE;
                }
            }
        }
    }

    if (table->table_type == DBAL_TABLE_TYPE_TCAM)
    {
        entry_handle->access_id_set = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_table_entry_get_next_inner(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_logical_table_t *table;
    dbal_iterator_info_t *iterator_info;
    dbal_physical_entry_iterator_t *mdb_iterator_info;
    dbal_iterator_attribute_info_t *iterator_attrib_info;
    dbal_physical_entry_t *phy_entry;
    uint32 app_id;
    uint32 is_action_get;
    uint32 perform_action;
    uint8 is_non_blocking = 0;
    uint8 to_read_entries_from_dma_only;

    SHR_FUNC_INIT_VARS(unit);

    table = entry_handle->table;

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));
    iterator_attrib_info = &iterator_info->attrib_info;
    mdb_iterator_info = &iterator_info->mdb_iterator;
    phy_entry = &entry_handle->phy_entry;
    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

    is_action_get = iterator_info->actions_bitmap & DBAL_ITER_ACTION_GET;
    perform_action = iterator_info->actions_bitmap & ~DBAL_ITER_ACTION_GET;
    to_read_entries_from_dma_only = iterator_info->mode == DBAL_ITER_MODE_GET_ENTRIES_FROM_DMA_NO_TRIGGER;

    if (!iterator_info->used_first_key && !to_read_entries_from_dma_only)
    {
        if (iterator_info->mode == DBAL_ITER_MODE_ACTION_NON_BLOCKING)
        {
            is_non_blocking = 1;
        }

        iterator_info->mdb_iterator.hit_bit_flags = entry_handle->phy_entry.hitbit;
        SHR_IF_ERR_EXIT(dbal_mdb_iterator_is_flush_machine(unit, entry_handle));

        if (is_non_blocking && !mdb_iterator_info->iterate_in_flush_machine)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Non-blocking iterator cannot work with given iterator information. "
                         "Iterator information cannot be ran over flush machine\n");
        }

        if (mdb_iterator_info->iterate_in_flush_machine)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_rules_shadow_set(unit, entry_handle, iterator_info->actions_bitmap));
            SHR_IF_ERR_EXIT(dbal_flush_start(unit, FALSE, (uint8) !is_action_get, is_non_blocking));

            if (!is_action_get && !is_non_blocking)
            {
                
                SHR_IF_ERR_EXIT(dbal_flush_end(unit));
            }
            if (is_non_blocking)
            {
                iterator_info->is_end = TRUE;
            }
        }
    }

    if ((is_action_get && mdb_iterator_info->iterate_in_flush_machine && !mdb_iterator_info->start_non_flush_iteration)
        || to_read_entries_from_dma_only)
    {
        SHR_IF_ERR_EXIT(dbal_flush_entry_get(unit, entry_handle));
        if (!iterator_info->is_end)
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "MDB entry iterated from FLUSH\n")));
            
            if (iterator_attrib_info->hit_bit_action_get || iterator_attrib_info->age_rule_valid)
            {
                if (iterator_attrib_info->hit_bit_action_get)
                {
                    SHR_IF_ERR_EXIT(dbal_iterator_attribute_action_add
                                    (unit, entry_handle->handle_id, iterator_attrib_info->hit_bit_action_get_type));
                }
                if (iterator_attrib_info->age_rule_valid)
                {
                    SHR_IF_ERR_EXIT(dbal_iterator_attribute_rule_add
                                    (unit, entry_handle->handle_id, DBAL_PHYSICAL_KEY_AGE_GET,
                                     iterator_attrib_info->age_rule_compare_value));
                }
                SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get(unit, entry_handle));
            }
            entry_handle->handle_status = DBAL_HANDLE_STATUS_ACTION_PREFORMED;
            entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_SW_SHADOW;
#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
            {
                uint8 is_mact_dynamic = 0;
                
                SHR_IF_ERR_EXIT(dbal_mdb_mact_dynamic_entry_check(unit, entry_handle, &is_mact_dynamic));
                if (is_mact_dynamic)
                {
                    SHR_IF_ERR_EXIT(dbal_mdb_mact_dynamic_entry_action_apply_update(unit, entry_handle));
                }
            }
#endif
        }
        else
        {
            
            SHR_IF_ERR_EXIT(dbal_flush_end(unit));
        }
    }

    
    if ((is_non_blocking == 0) && !to_read_entries_from_dma_only &&
        ((iterator_info->is_end && mdb_iterator_info->iterate_in_flush_machine) ||
         !is_action_get || !mdb_iterator_info->iterate_in_flush_machine
         || mdb_iterator_info->start_non_flush_iteration))
    {
        uint32 result_types_to_find = 0xFFFFFFFF;

        if (!mdb_iterator_info->start_non_flush_iteration)
        {
            iterator_info->is_end = FALSE;
        }

        mdb_iterator_info->start_non_flush_iteration = TRUE;
        
        if (mdb_iterator_info->iterate_in_flush_machine)
        {
            result_types_to_find = mdb_iterator_info->result_types_in_non_flush_bitmap;
        }

        SHR_IF_ERR_EXIT(dbal_mdb_access_get_next(unit, app_id, table->physical_db_id[iterator_info->physical_db_index],
                                                 entry_handle, phy_entry, iterator_info));
        if (!iterator_info->is_end)
        {
            dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;
            if (table->core_mode == DBAL_CORE_MODE_DPC)
            {
                entry_handle->core_id = iterator_info->physical_db_index;
            }
            SHR_IF_ERR_EXIT(dbal_key_buffer_validate(unit, entry_handle, &is_valid_entry));
            if (is_valid_entry == DBAL_KEY_IS_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "MDB iterator found invalid entry\n");
            }
            else if (is_valid_entry == DBAL_KEY_IS_OUT_OF_ITERATOR_RULE)
            {
                iterator_info->mdb_iterator.continue_calling_mdb_get_next = TRUE;
                SHR_EXIT();
            }
            else if (!utilex_bitstream_test_bit(&result_types_to_find, entry_handle->cur_res_type))
            {
                if (!is_action_get && perform_action)
                {
                    entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_SW_SHADOW;
#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
                    {
                        uint8 is_mact_dynamic = 0;
                        
                        SHR_IF_ERR_EXIT(dbal_mdb_mact_dynamic_entry_check(unit, entry_handle, &is_mact_dynamic));
                        if (is_mact_dynamic)
                        {
                            SHR_IF_ERR_EXIT(dbal_mdb_mact_dynamic_entry_action_apply_update(unit, entry_handle));
                        }
                    }
#endif
                }
                else
                {
                    iterator_info->mdb_iterator.continue_calling_mdb_get_next = TRUE;
                }
                SHR_EXIT();
            }
            iterator_info->mdb_iterator.continue_calling_mdb_get_next = FALSE;
            entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_ALL;
            SHR_IF_ERR_EXIT(dbal_physical_entry_print(unit, entry_handle,
                                                      table->physical_db_id[iterator_info->physical_db_index],
                                                      &entry_handle->phy_entry, app_id, FALSE, "get next from"));
        }
        else
        {
            if ((table->nof_physical_tables > 0)
                && (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
                && (table->core_mode == DBAL_CORE_MODE_DPC)
                && (iterator_info->physical_db_index < (table->nof_physical_tables - 1)) && (DBAL_MAX_NUM_OF_CORES > 1))
            {
                SHR_IF_ERR_EXIT(dbal_mdb_table_iterator_deinit(unit, entry_handle));
                iterator_info->physical_db_index++;
                iterator_info->is_end = FALSE;
                SHR_IF_ERR_EXIT(dbal_mdb_table_iterator_init(unit, entry_handle));
                SHR_IF_ERR_EXIT(dbal_mdb_access_get_next(unit, app_id,
                                                         table->physical_db_id[iterator_info->physical_db_index],
                                                         entry_handle, phy_entry, iterator_info));
            }
            if (!iterator_info->is_end)
            {
                dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;
                if (table->core_mode == DBAL_CORE_MODE_DPC)
                {
                    entry_handle->core_id = iterator_info->physical_db_index;
                }
                SHR_IF_ERR_EXIT(dbal_key_buffer_validate(unit, entry_handle, &is_valid_entry));
                if (is_valid_entry == DBAL_KEY_IS_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "MDB iterator found invalid entry\n");
                }
                else if (is_valid_entry == DBAL_KEY_IS_OUT_OF_ITERATOR_RULE)
                {
                    iterator_info->mdb_iterator.continue_calling_mdb_get_next = TRUE;
                    SHR_EXIT();
                }
                else if (!utilex_bitstream_test_bit(&result_types_to_find, entry_handle->cur_res_type))
                {
                    if (!is_action_get && perform_action)
                    {
                        entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_SW_SHADOW;
#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
                        {
                            uint8 is_mact_dynamic = 0;
                            
                            SHR_IF_ERR_EXIT(dbal_mdb_mact_dynamic_entry_check(unit, entry_handle, &is_mact_dynamic));
                            if (is_mact_dynamic)
                            {
                                SHR_IF_ERR_EXIT(dbal_mdb_mact_dynamic_entry_action_apply_update(unit, entry_handle));
                            }
                        }
#endif
                    }
                    else
                    {
                        iterator_info->mdb_iterator.continue_calling_mdb_get_next = TRUE;
                    }
                    SHR_EXIT();
                }
                iterator_info->mdb_iterator.continue_calling_mdb_get_next = FALSE;
                entry_handle->phy_entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_ALL;
                SHR_IF_ERR_EXIT(dbal_physical_entry_print(unit, entry_handle,
                                                          table->physical_db_id[iterator_info->physical_db_index],
                                                          &entry_handle->phy_entry, app_id, FALSE, "get next from"));
            }
            else
            {
                SHR_EXIT();
            }
        }
    }

    if (iterator_info->attrib_info.age_rule_valid && !iterator_info->is_end)
    {
        entry_handle->phy_entry.age = DBAL_PHYSICAL_KEY_AGE_GET;
        SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get(unit, entry_handle));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get_next_inner(unit, entry_handle));
    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    while (iterator_info->mdb_iterator.continue_calling_mdb_get_next)
    {
        iterator_info->mdb_iterator.continue_calling_mdb_get_next = FALSE;
        SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get_next_inner(unit, entry_handle));
    }
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dbal_mdb_table_iterator_deinit(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_physical_table_def_t *PhysicalTable = NULL;
    dbal_physical_tables_e physical_db_id;
    uint32 app_id;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_iterator_init(unit, entry_handle));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, entry_handle->table_id,
                                                       iterator_info->physical_db_index, &physical_db_id));

        SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));

        if (PhysicalTable->table_actions->iterator_deinit == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR missing implementation for %s\n", PhysicalTable->physical_name);
        }

        DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

        SHR_IF_ERR_EXIT(PhysicalTable->table_actions->iterator_deinit(unit, physical_db_id, app_id,
                                                                      &iterator_info->mdb_iterator));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_access_get(
    int unit,
    uint32 app_id,
    dbal_physical_tables_e physical_db_id,
    dbal_entry_handle_t * entry_handle,
    uint8 is_eedb_ll)
{
    int rv;
    dbal_physical_table_def_t *PhysicalTable = NULL;
    dbal_physical_entry_t *phy_entry = &entry_handle->phy_entry;
    uint32 entry_flags[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (is_eedb_ll)
    {
        SHR_BITSET(entry_flags, DBAL_MDB_ENTRY_FLAGS_EEDB_LL_EXISTS);
    }

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));
    if (PhysicalTable->table_actions->entry_get == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "missing implementation %s\n", PhysicalTable->physical_name);
    }

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        if (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal result type for entry table %s\n", entry_handle->table->table_name);
        }
        
        phy_entry->payload_size -= DBAL_RES_INFO.results_info[0].field_nof_bits;
    }

    
    {
        dbal_mdb_action_apply_type_e orig_access_apply_type = phy_entry->mdb_action_apply;
        uint8 skip_shadow = 0;

        
        SHR_IF_ERR_EXIT(dbal_db.mdb_access.skip_read_from_shadow.get(unit, &skip_shadow));
        if (skip_shadow)
        {
            if (phy_entry->mdb_action_apply != DBAL_MDB_ACTION_APPLY_SW_SHADOW)
            {
                phy_entry->mdb_action_apply = DBAL_MDB_ACTION_APPLY_HW_ONLY;
            }
        }
#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
        else if (phy_entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
        {
            if (!SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_LEARNING_EN))
            {
                
                phy_entry->mdb_action_apply = DBAL_MDB_ACTION_APPLY_SW_SHADOW;
            }
        }
#endif

        
        if ((entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM) && (entry_handle->access_id_set == 0))
        {
            rv = dbal_mdb_table_access_id_by_key_get(unit, entry_handle);
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }

        
        if (entry_handle->table_id == DBAL_TABLE_GLOBAL_RIF_EM)
        {
            
            rv = dbal_mdb_global_rif_get(unit, entry_handle, PhysicalTable, app_id, &physical_db_id, entry_flags[0]);
        }
        else
        {
            rv = PhysicalTable->table_actions->entry_get(unit, physical_db_id, app_id, phy_entry, entry_flags[0]);
        }

        phy_entry->mdb_action_apply = orig_access_apply_type;
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        if (dbal_table_is_out_lif(entry_handle->table))
        {
            uint8 rt_has_link_list = FALSE;

            if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
            {
                dbal_table_field_info_t *table_field_info;
                int curr_res_type = entry_handle->cur_res_type;

                 
                table_field_info = &(entry_handle->table->multi_res_info[curr_res_type].results_info[0]);

                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                (&entry_handle->table->multi_res_info[curr_res_type].result_type_hw_value[0],
                                 table_field_info->bits_offset_in_buffer, table_field_info->field_nof_bits,
                                 phy_entry->payload));
            }

            SHR_IF_ERR_EXIT(dnx_lif_mngr_eedb_table_rt_has_linked_list
                            (unit, entry_handle->table_id, entry_handle->cur_res_type, &rt_has_link_list));
            if ((is_eedb_ll == FALSE) && rt_has_link_list)
            {
                
                SHR_IF_ERR_EXIT(dbal_entry_handle_phy_entry_align
                                (unit, MDB_DIRECT_BASIC_ENTRY_SIZE, FALSE, entry_handle));
            }

        }
        else
        {
            if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
            {
                dbal_table_field_info_t *table_field_info;
                int curr_res_type = entry_handle->cur_res_type;

                if (entry_handle->table->multi_res_info[curr_res_type].entry_payload_size <
                    entry_handle->table->max_payload_size)
                {
                    SHR_IF_ERR_EXIT(dbal_entry_handle_phy_entry_align
                                    (unit,
                                     entry_handle->table->max_payload_size -
                                     entry_handle->table->multi_res_info[curr_res_type].entry_payload_size, TRUE,
                                     entry_handle));
                }

                 
                table_field_info = &(entry_handle->table->multi_res_info[curr_res_type].results_info[0]);

                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                (&entry_handle->table->multi_res_info[curr_res_type].result_type_hw_value[0],
                                 table_field_info->bits_offset_in_buffer, table_field_info->field_nof_bits,
                                 phy_entry->payload));
            }
            else
            {
                if (dbal_table_is_in_lif(entry_handle->table)
                    && entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "No need to do result type resolution for inlif %s",
                                 entry_handle->table->table_name);
                }
                else if (dbal_table_is_in_lif(entry_handle->table))
                {
                    
                    if ((entry_handle->table->multi_res_info[entry_handle->cur_res_type].entry_payload_size <
                         entry_handle->table->max_payload_size))
                    {
                        SHR_IF_ERR_EXIT(dbal_entry_handle_phy_entry_align
                                        (unit,
                                         entry_handle->table->max_payload_size -
                                         entry_handle->table->multi_res_info[entry_handle->
                                                                             cur_res_type].entry_payload_size, TRUE,
                                         entry_handle));
                        entry_handle->phy_entry.payload_size =
                            entry_handle->table->multi_res_info[entry_handle->cur_res_type].entry_payload_size;
                    }
                }
                else
                {
                    rv = dbal_mdb_res_type_resolution(unit, entry_handle, FALSE);
                    if (rv == _SHR_E_NOT_FOUND)
                    {
                        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                        SHR_EXIT();
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(rv);
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int core_id;
    dbal_logical_table_t *table;
    dbal_physical_entry_t *phy_entry;
    uint32 app_id;
    uint8 rt_has_link_list = FALSE;
    uint8 is_eedb_ll = FALSE;
    dbal_physical_tables_e eedb_physical_table, physical_table;

    SHR_FUNC_INIT_VARS(unit);
    table = entry_handle->table;
    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

    phy_entry = &entry_handle->phy_entry;

    phy_entry->payload_size = entry_handle->table->max_payload_size;
    phy_entry->key_size = entry_handle->table->key_size;
    if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        phy_entry->key_size -= DBAL_CORE_SIZE_IN_BITS;
    }

    if (dbal_table_is_out_lif(table))
    {
        if (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "handle result type not initialized table %s\n",
                         entry_handle->table->table_name);
        }

        phy_entry->payload_size = DBAL_RES_INFO.entry_payload_size;
        
        SHR_IF_ERR_EXIT(dbal_outlif_sw_info_update_payload_size
                        (unit, entry_handle->table_id, entry_handle->cur_res_type, phy_entry, &rt_has_link_list,
                         &eedb_physical_table, &is_eedb_ll));
    }

    {
        DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
        {
            int rv;
            if (table->physical_db_id[core_id] == DBAL_PHYSICAL_TABLE_NONE)
            {
                continue;
            }

            physical_table = (dbal_table_is_out_lif(table)) ? eedb_physical_table : table->physical_db_id[core_id];
            rv = dbal_mdb_access_get(unit, app_id, physical_table, entry_handle, is_eedb_ll);
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            SHR_IF_ERR_EXIT(dbal_physical_entry_print
                            (unit, entry_handle, physical_table, &entry_handle->phy_entry, app_id, FALSE, "get from"));
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_mdb_access_delete(
    int unit,
    uint32 app_id,
    dbal_physical_tables_e physical_db_ids[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    dbal_entry_handle_t * entry_handle,
    dbal_physical_entry_t * phy_entry,
    uint8 is_eedb_ll)
{
    dbal_physical_table_def_t *PhysicalTable = NULL;
    uint32 entry_flags[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_ids[0], &PhysicalTable));

    if (PhysicalTable->table_actions->entry_delete == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "missing implementation %s\n", PhysicalTable->physical_name);
    }

    if (is_eedb_ll)
    {
        SHR_BITSET(entry_flags, DBAL_MDB_ENTRY_FLAGS_EEDB_LL_EXISTS);
    }

    
    if ((entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM) && (entry_handle->access_id_set == 0))
    {
        int rv = dbal_mdb_table_access_id_by_key_get(unit, entry_handle);
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        int sw_access_needed = 1;

        if ((entry_handle->table->core_mode == DBAL_CORE_MODE_DPC) && (DBAL_MAX_NUM_OF_CORES == 1) &&
            (physical_db_ids[0] != entry_handle->table->physical_db_id[0]))
        {
            
            sw_access_needed = 0;
        }
        if (sw_access_needed)
        {
            if (entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT)
            {
                SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_clear(unit, entry_handle));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_delete(unit, entry_handle));
            }
        }
        phy_entry->payload_size -= entry_handle->table->multi_res_info[0].results_info[0].field_nof_bits;
    }

    if (entry_handle->table->table_type == DBAL_TABLE_TYPE_LPM)
    {
        phy_entry->hitbit = SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_HITBIT_EN);
    }

    if (entry_handle->table_id == DBAL_TABLE_GLOBAL_RIF_EM)
    {
        
        SHR_IF_ERR_EXIT(dbal_mdb_global_rif_delete
                        (unit, entry_handle, PhysicalTable, app_id, physical_db_ids, entry_flags[0]));
    }
    else
    {
        SHR_IF_ERR_EXIT(PhysicalTable->
                        table_actions->entry_delete(unit, physical_db_ids, app_id, phy_entry, entry_flags[0]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_logical_table_t *table;
    dbal_physical_entry_t *phy_entry;
    uint32 app_id;
    uint32 payload_size_to_delete = 0;
    uint8 rt_has_link_list = FALSE;
    uint8 is_eedb_ll = 0;
    dbal_physical_tables_e eedb_physical_table;
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(physical_tables, 0x0, sizeof(physical_tables));

    table = entry_handle->table;
    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);

    phy_entry = &entry_handle->phy_entry;

    if (dbal_table_is_out_lif(table))
    {
        payload_size_to_delete = DBAL_RES_INFO.entry_payload_size;
        
        SHR_IF_ERR_EXIT(dbal_outlif_sw_info_update_payload_size
                        (unit, entry_handle->table_id, entry_handle->cur_res_type, phy_entry, &rt_has_link_list,
                         &eedb_physical_table, &is_eedb_ll));
        if ((!is_eedb_ll) && (rt_has_link_list))
        {
            payload_size_to_delete -= MDB_DIRECT_BASIC_ENTRY_SIZE;
        }
    }
    else if (dbal_table_is_in_lif(table))
    {
        payload_size_to_delete = DBAL_RES_INFO.entry_payload_size;
    }
    else
    {
        payload_size_to_delete = table->multi_res_info[entry_handle->cur_res_type].entry_payload_size;
        
        if (table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_PPMC)
        {
            SHR_IF_ERR_EXIT(dbal_mdb_table_entry_get(unit, entry_handle));
        }
    }

    phy_entry->key_size = table->key_size;
    phy_entry->payload_size = payload_size_to_delete;
    if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        phy_entry->key_size -= DBAL_CORE_SIZE_IN_BITS;
    }

    {
        int core_id;
        DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
        {
            if (table->physical_db_id[core_id] == DBAL_PHYSICAL_TABLE_NONE)
            {
                continue;
            }
            
            physical_tables[core_id - first_core] = (dbal_table_is_out_lif(table)) ?
                eedb_physical_table : table->physical_db_id[core_id];
        }
        SHR_IF_ERR_EXIT(dbal_physical_entry_print
                        (unit, entry_handle, physical_tables[0], phy_entry, app_id, FALSE, "add to"));
        SHR_IF_ERR_EXIT(dbal_mdb_access_delete(unit, app_id, physical_tables, entry_handle, phy_entry, is_eedb_ll));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_mdb_access_write(
    int unit,
    uint32 app_id,
    dbal_physical_tables_e physical_db_ids[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    dbal_entry_handle_t * entry_handle,
    dbal_physical_entry_t * phy_entry,
    uint8 is_eedb_ll)
{
    dbal_physical_table_def_t *PhysicalTable = NULL;
    uint32 entry_flags[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (is_eedb_ll)
    {
        SHR_BITSET(entry_flags, DBAL_MDB_ENTRY_FLAGS_EEDB_LL_EXISTS);
    }

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_ids[0], &PhysicalTable));
    if (PhysicalTable->table_actions->entry_add == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "missing implementation %s\n", PhysicalTable->physical_name);
    }

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        if (entry_handle->table->table_type == DBAL_TABLE_TYPE_DIRECT)
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_direct_entry_set(unit, entry_handle));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_hash_entry_add(unit, entry_handle));
        }
    }
    if (entry_handle->table->table_type == DBAL_TABLE_TYPE_LPM)
    {
        int hitbit;

        hitbit = SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_HITBIT_EN);
        phy_entry->hitbit = (hitbit != 0) ? DBAL_PHYSICAL_KEY_HITBIT_ACTION : 0;
    }

    if (entry_handle->table_id == DBAL_TABLE_GLOBAL_RIF_EM)
    {
        
        SHR_IF_ERR_EXIT(dbal_mdb_global_rif_add
                        (unit, entry_handle, PhysicalTable, app_id, physical_db_ids, entry_flags[0]));
    }
    else
    {
        SHR_IF_ERR_EXIT(PhysicalTable->
                        table_actions->entry_add(unit, physical_db_ids, app_id, phy_entry, entry_flags[0]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_logical_table_t *table;
    dbal_physical_entry_t *phy_entry;
    uint32 app_id;
    uint8 rt_has_link_list = FALSE;
    uint8 is_eedb_ll = FALSE;
    dbal_physical_tables_e eedb_physical_table = DBAL_PHYSICAL_TABLE_NONE;
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    table = entry_handle->table;
    DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);
    phy_entry = &entry_handle->phy_entry;
    phy_entry->payload_size = table->multi_res_info[entry_handle->cur_res_type].entry_payload_size;
    phy_entry->key_size = table->key_size;
    if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
    {
        phy_entry->key_size -= DBAL_CORE_SIZE_IN_BITS;
    }

#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
    if (entry_handle->table_id == DBAL_TABLE_FWD_MACT)
    {
        uint8 is_mact_dynamic = 0;
        
        SHR_IF_ERR_EXIT(dbal_mdb_mact_dynamic_entry_check(unit, entry_handle, &is_mact_dynamic));
        if (is_mact_dynamic)
        {
            SHR_IF_ERR_EXIT(dbal_mdb_mact_dynamic_entry_action_apply_update(unit, entry_handle));
        }
    }
#endif

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        phy_entry->payload_size -= DBAL_RES_INFO.results_info[0].field_nof_bits;
    }

    if (dbal_table_is_out_lif(table))
    {
        
        SHR_IF_ERR_EXIT(dbal_outlif_sw_info_update_payload_size
                        (unit, entry_handle->table_id, entry_handle->cur_res_type, phy_entry, &rt_has_link_list,
                         &eedb_physical_table, &is_eedb_ll));
        if ((!is_eedb_ll) && (rt_has_link_list))
        {
            
            SHR_IF_ERR_EXIT(dbal_entry_handle_phy_entry_align(unit, MDB_DIRECT_BASIC_ENTRY_SIZE, TRUE, entry_handle));
        }
    }

    {
        int core_id;
        int bsl_severity;
        DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
        {
            if (table->physical_db_id[core_id] == DBAL_PHYSICAL_TABLE_NONE)
            {
                continue;
            }
            
            physical_tables[core_id - first_core] = (dbal_table_is_out_lif(table)) ?
                eedb_physical_table : table->physical_db_id[core_id];
        }
        SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);
        if (bsl_severity >= bslSeverityInfo)
        {
            SHR_IF_ERR_EXIT(dbal_physical_entry_print
                            (unit, entry_handle, physical_tables[0], phy_entry, app_id, FALSE, "add to"));
        }
        SHR_IF_ERR_EXIT(dbal_mdb_access_write(unit, app_id, physical_tables, entry_handle, phy_entry, is_eedb_ll));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mdb_access_table_clear(
    int unit,
    uint32 app_id,
    dbal_physical_tables_e physical_db_id,
    dbal_entry_handle_t * entry_handle)
{
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };
    dbal_physical_table_def_t *PhysicalTable = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));
    if (PhysicalTable->table_actions->table_clear == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "missing implementation %s\n", PhysicalTable->physical_name);
    }

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
    {
        SHR_IF_ERR_EXIT(dbal_sw_table_clear(unit, entry_handle));
    }

    physical_tables[0] = physical_db_id; 

    SHR_IF_ERR_EXIT(PhysicalTable->table_actions->table_clear(unit, physical_tables, app_id));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_access_table_clear_by_iter(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 iter_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, entry_handle->table_id, &iter_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, iter_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_entry_action_add(unit, iter_handle_id, DBAL_ITER_ACTION_DELETE));
    SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, iter_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_logical_table_t *table = entry_handle->table;
    uint32 app_id;
    SHR_FUNC_INIT_VARS(unit);

    
    if (table->table_type != DBAL_TABLE_TYPE_LPM)
    {
        SHR_IF_ERR_EXIT(dbal_mdb_access_table_clear_by_iter(unit, entry_handle));
    }
    else
    {
        DBAL_MDB_ACCESS_APP_ID_GET(unit, entry_handle, app_id);
        {
            int core_id;
            DBAL_ITERATE_OVER_CORES(entry_handle, core_id)
            {
                if (table->physical_db_id[core_id] == DBAL_PHYSICAL_TABLE_NONE)
                {
                    continue;
                }
                SHR_IF_ERR_EXIT(dbal_mdb_access_table_clear
                                (unit, app_id, table->physical_db_id[core_id], entry_handle));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mdb_table_access_id_by_key_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 tcam_handler_id;
    uint32 access_id;
    int rv;
    int core = 0;
    SHR_FUNC_INIT_VARS(unit);
    if (!DBAL_TABLE_IS_TCAM(entry_handle->table))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "get_access_id supported only for TCAM\n");
    }
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.tcam_handler_id.get(unit, entry_handle->table_id, &tcam_handler_id));
    SHR_IF_ERR_EXIT(dbal_physical_entry_print
                    (unit, entry_handle, entry_handle->table->physical_db_id[0], &entry_handle->phy_entry,
                     tcam_handler_id, TRUE, "Access ID get"));
    rv = dnx_field_tcam_access_key_to_access_id
        (unit, core, tcam_handler_id, entry_handle->phy_entry.key, entry_handle->phy_entry.k_mask, &access_id);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT_NO_MSG(rv);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }
    entry_handle->phy_entry.entry_hw_id = (int) access_id;
exit:
    SHR_FUNC_EXIT;
}


/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_techsupport.h>









static shr_error_e
q2a_a0_dnx_data_techsupport_tm_tm_mem_set(
    int unit)
{
    int block_index;
    int index_index;
    dnx_data_techsupport_tm_tm_mem_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_techsupport;
    int submodule_index = dnx_data_techsupport_submodule_tm;
    int table_index = dnx_data_techsupport_tm_table_tm_mem;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_techsupport_block_count;
    table->info_get.key_size[0] = dnx_techsupport_block_count;
    table->keys[1].size = dnx_data_techsupport.tm.num_of_tables_per_block_get(unit);
    table->info_get.key_size[1] = dnx_data_techsupport.tm.num_of_tables_per_block_get(unit);

    
    table->values[0].default_val = "NULL";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_techsupport_tm_tm_mem_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_techsupport_tm_table_tm_mem");

    
    default_data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->name = NULL;
    
    for (block_index = 0; block_index < table->keys[0].size; block_index++)
    {
        for (index_index = 0; index_index < table->keys[1].size; index_index++)
        {
            data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, block_index, index_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (dnx_techsupport_cgm < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 0);
        data->name = "CGM_VOQ_SHRD_FR_RJCT_TH";
    }
    if (dnx_techsupport_cgm < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 1);
        data->name = "CGM_GLBL_FR_SRAM_DP_RJCT_TH";
    }
    if (dnx_techsupport_cgm < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 2);
        data->name = "CGM_GLBL_FR_SRAM_TC_RJCT_TH";
    }
    if (dnx_techsupport_cgm < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 3);
        data->name = "CGM_GLBL_FR_DRAM_DP_RJCT_TH";
    }
    if (dnx_techsupport_cgm < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 4);
        data->name = "CGM_GLBL_FR_DRAM_TC_RJCT_TH";
    }
    if (dnx_techsupport_cgm < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 5);
        data->name = "CGM_VOQ_SRAM_DRAM_ONLY_MODE";
    }
    if (dnx_techsupport_cgm < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 6);
        data->name = "CGM_VOQ_HEADER_COMPENSATION";
    }
    if (dnx_techsupport_cgm < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 7);
        data->name = "CGM_IPP_HEADER_COMPENSATION";
    }
    if (dnx_techsupport_cgm < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 8);
        data->name = "CGM_VOQ_DRAM_RECOVERY_PRMS";
    }
    if (dnx_techsupport_cgm < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 9);
        data->name = "CGM_VOQ_DRAM_RECOVERY_CACHE_PRMS";
    }
    if (dnx_techsupport_cgm < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 10);
        data->name = "CGM_VOQ_GRNTD_PRMS";
    }
    if (dnx_techsupport_cgm < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 11);
        data->name = "CGM_VOQ_WORDS_RJCT_PRMS";
    }
    if (dnx_techsupport_cgm < table->keys[0].size && 12 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 12);
        data->name = "CGM_VOQ_SRAM_BUFFERS_RJCT_PRMS";
    }
    if (dnx_techsupport_cgm < table->keys[0].size && 13 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 13);
        data->name = "CGM_VOQ_SRAM_PDS_RJCT_PRMS";
    }
    if (dnx_techsupport_cgm < table->keys[0].size && 14 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 14);
        data->name = "CGM_VOQ_CONGESTION_PRMS";
    }
    if (dnx_techsupport_cgm < table->keys[0].size && 15 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_cgm, 15);
        data->name = "CGM_VOQ_DRAM_BOUND_PRMS";
    }
    if (dnx_techsupport_spb < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_spb, 0);
        data->name = "SPB_CONTEXT_MRU";
    }
    if (dnx_techsupport_spb < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_spb, 1);
        data->name = "SPB_PACKET_REJECT_CFG";
    }
    if (dnx_techsupport_spb < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_spb, 2);
        data->name = "SPB_COUNTERS";
    }
    if (dnx_techsupport_ecgm < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ecgm, 0);
        data->name = "ECGM_QDCT_TABLE";
    }
    if (dnx_techsupport_ecgm < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ecgm, 1);
        data->name = "ECGM_PQST_TABLE";
    }
    if (dnx_techsupport_ecgm < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ecgm, 2);
        data->name = "ECGM_PDCT_TABLE";
    }
    if (dnx_techsupport_ecgm < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ecgm, 3);
        data->name = "ECGM_FDCT_TABLE";
    }
    if (dnx_techsupport_ecgm < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ecgm, 4);
        data->name = "ECGM_FQSMAX";
    }
    if (dnx_techsupport_ecgm < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ecgm, 5);
        data->name = "ECGM_QQSMAX";
    }
    if (dnx_techsupport_ecgm < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ecgm, 6);
        data->name = "ECGM_QQST_TABLE";
    }
    if (dnx_techsupport_ecgm < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ecgm, 7);
        data->name = "ECGM_FDCMAX";
    }
    if (dnx_techsupport_ecgm < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ecgm, 8);
        data->name = "ECGM_QDCMAX";
    }
    if (dnx_techsupport_ecgm < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ecgm, 9);
        data->name = "ECGM_PQSMAX";
    }
    if (dnx_techsupport_ecgm < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ecgm, 10);
        data->name = "ECGM_PDCMAX";
    }
    if (dnx_techsupport_pqp < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_pqp, 0);
        data->name = "PQP_TC_DP_MAP";
    }
    if (dnx_techsupport_pqp < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_pqp, 1);
        data->name = "PQP_OTM_ATTRIBUTES_TABLE_DEQ";
    }
    if (dnx_techsupport_pqp < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_pqp, 2);
        data->name = "PQP_OTM_ATTRIBUTES_TABLE_ENQ";
    }
    if (dnx_techsupport_rqp < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_rqp, 0);
        data->name = "RQP_MC_SP_TC_MAP";
    }
    if (dnx_techsupport_fqp < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_fqp, 0);
        data->name = "FQP_FQP_NIF_PORT_MUX";
    }
    if (dnx_techsupport_fqp < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_fqp, 1);
        data->name = "FQP_PER_IFC_CFG";
    }
    if (dnx_techsupport_fqp < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_fqp, 2);
        data->name = "FQP_IFC_PROFILE_CFG";
    }
    if (dnx_techsupport_ebs < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ebs, 0);
        data->name = "ESB_IFC_PROFILE_CFG";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 0);
        data->name = "EPS_DWM";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 1);
        data->name = "EPS_PQP_NIF_PORT_MUX";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 2);
        data->name = "EPS_EPS_PRIO_MAP";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 3);
        data->name = "EPS_QP_CFG";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 4);
        data->name = "EPS_QP_CFG_2";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 5);
        data->name = "EPS_OTM_CFG";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 6);
        data->name = "EPS_OTM_CFG_2";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 7);
        data->name = "EPS_TCG_CFG";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 8);
        data->name = "EPS_TCG_CFG_2";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 9);
        data->name = "EPS_IFC_CFG";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 10);
        data->name = "EPS_IFC_CFG_2";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 11);
        data->name = "EPS_CAL_CAL_INDX_MUX";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 12 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 12);
        data->name = "EPS_OTM_CALENDAR_CRDT_TABLE";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 13 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 13);
        data->name = "EPS_OTM_HP_CRDT_TABLE";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 14 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 14);
        data->name = "EPS_OTM_LP_CRDT_TABLE";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 15 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 15);
        data->name = "EPS_QP_CREDIT_TABLE";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 16 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 16);
        data->name = "EPS_TCG_CREDIT_TABLE";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 17 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 17);
        data->name = "EPS_OTM_SHAPER_LENGTH";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 18 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 18);
        data->name = "EPS_MAP_PS_TO_IFC";
    }
    if (dnx_techsupport_eps < table->keys[0].size && 19 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_eps, 19);
        data->name = "EPS_MAP_OTM_TO_CAL_INDX";
    }
    if (dnx_techsupport_sch < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_sch, 0);
        data->name = "SCH_PORT_ENABLE_PORTEN";
    }
    if (dnx_techsupport_sch < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_sch, 1);
        data->name = "SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCC";
    }
    if (dnx_techsupport_sch < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_sch, 2);
        data->name = "SCH_CH_NIF_RATES_CONFIGURATION_CNRC";
    }
    if (dnx_techsupport_sch < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_sch, 3);
        data->name = "SCH_CIR_SHAPERS_STATIC_TABEL_CSST";
    }
    if (dnx_techsupport_sch < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_sch, 4);
        data->name = "SCH_PIR_SHAPERS_STATIC_TABEL_PSST";
    }
    if (dnx_techsupport_sch < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_sch, 5);
        data->name = "SCH_FC_MAP_FCM";
    }
    if (dnx_techsupport_ips < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ips, 0);
        data->name = "IPS_SLOW_FACTOR_TH";
    }
    if (dnx_techsupport_ips < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ips, 1);
        data->name = "IPS_CRBAL_TH";
    }
    if (dnx_techsupport_ips < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ips, 2);
        data->name = "IPS_EMPTY_Q_CRBAL_TH";
    }
    if (dnx_techsupport_ips < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ips, 3);
        data->name = "IPS_CRDT_VALUE_SEL";
    }
    if (dnx_techsupport_ips < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ips, 4);
        data->name = "IPS_QSIZE_TH";
    }
    if (dnx_techsupport_ips < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ips, 5);
        data->name = "IPS_CRDT_WD_TH";
    }
    if (dnx_techsupport_ips < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ips, 6);
        data->name = "IPS_QTYPE";
    }
    if (dnx_techsupport_ipt < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ipt, 0);
        data->name = "IPT_PER_SHAPER_CFG";
    }
    if (dnx_techsupport_ipt < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ipt, 1);
        data->name = "IPT_SHAPER_FMC_CFG";
    }
    if (dnx_techsupport_ipt < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_techsupport_ipt, 2);
        data->name = "IPT_PRIORITY_BITS_MAPPING_2_FDT";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
q2a_a0_data_techsupport_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_techsupport;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_techsupport_submodule_tm;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_techsupport_tm_table_tm_mem;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_techsupport_tm_tm_mem_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


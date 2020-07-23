

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INIT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm_int/dnx/spb/spb.h>
#include <bcm_int/dnx/init/init_custom_funcs.h>









static shr_error_e
j2p_a0_dnx_data_dev_init_mem_default_set(
    int unit)
{
    int index_index;
    dnx_data_dev_init_mem_default_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_mem;
    int table_index = dnx_data_dev_init_mem_table_default;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 57;
    table->info_get.key_size[0] = 57;

    
    table->values[0].default_val = "INVALIDr";
    table->values[1].default_val = "dnx_init_mem_default_mode_zero";
    table->values[2].default_val = "NULL";
    table->values[3].default_val = "INVALIDf";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_mem_default_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dev_init_mem_table_default");

    
    default_data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mem = INVALIDr;
    default_data->mode = dnx_init_mem_default_mode_zero;
    default_data->default_get_cb = NULL;
    default_data->field = INVALIDf;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->mem = IPS_SPMm;
        data->mode = dnx_init_mem_default_mode_all_ones;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->mem = MTM_EGR_BITMAP_MAPPING_TABLE_Am;
        data->mode = dnx_init_mem_default_mode_index;
        data->field = PP_DSPf;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->mem = MTM_EGR_BITMAP_MAPPING_TABLE_Bm;
        data->mode = dnx_init_mem_default_mode_index;
        data->field = PP_DSPf;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->mem = ERPP_PRP_PP_DSP_PTR_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_erpp_prp_pp_dsp_ptr_table_get;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->mem = ERPP_TM_PP_DSP_PTR_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_erpp_tm_pp_dsp_ptr_table_get;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->mem = ETPPA_DSP_DATA_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_etppa_dsp_data_table_get;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->mem = CGM_REP_CMD_FIFO_OCCUPANCY_RJCT_THm;
        data->mode = dnx_init_mem_default_mode_all_ones;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->mem = SPB_CONTEXT_MRUm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_spb_context_mru_default_get_f;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->mem = EPNI_ALIGNER_MIRR_QP_TO_CHANNEL_MAPm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = CHANNELf;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->mem = SCH_FLOW_TO_FIP_MAPPING_FFMm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_flow_to_fip_mapping_default_get;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->mem = ERPP_CFG_ENABLE_FILTER_PER_PORT_TABLEm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = CFG_SAME_INTERFACE_PER_PORTf;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->mem = ETPPB_VID_MIRROR_PROFILE_TABLEm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = OUT_PORT_MIRROR_PROFILEf;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->mem = SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_shaper_descr_default_get;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->mem = SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_flow_descr_default_get;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->mem = IPT_PCP_CFGm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_fabric_pcp_default_get;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->mem = SCH_SLOW_SCALE_A_SSAm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = MAX_BUCKETf;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->mem = SCH_SLOW_SCALE_B_SSBm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = MAX_BUCKETf;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->mem = IPPE_PINFO_LLRm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = RECYCLE_PROFILE_ENABLEf;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->mem = MTM_MCDBm;
        data->mode = dnx_init_mem_default_mode_all_ones;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->mem = SCH_CIR_SHAPER_CALENDAR_CSCm;
        data->mode = dnx_init_mem_default_mode_index;
        data->field = PG_NUMf;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->mem = SCH_RESERVED_51m;
        data->mode = dnx_init_mem_default_mode_index;
        data->field = HR_NUMf;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->mem = SCH_PORT_SCHEDULER_WEIGHTS_PSWm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_port_schduler_weights_psw_default_get;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->mem = IPPD_DESTINATION_STATUSm;
        data->mode = dnx_init_mem_default_mode_all_ones;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->mem = IPS_QSPMm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = SYSTEM_PORTf;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->mem = IPS_QPRIORITYm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = QPRIORITY_DATAf;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->mem = CGM_IPP_MAPm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_cgm_ipp_map_default_get;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->mem = IPPC_FES_2ND_INSTRUCTIONm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_ippc_fes_2nd_instruction_default_get;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->mem = IPPD_FES_2ND_INSTRUCTIONm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_ippd_fes_2nd_instruction_default_get;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->mem = ERPP_FES_2ND_INSTRUCTIONm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_erpp_fes_2nd_instruction_default_get;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->mem = ETPPA_PRP_FES_PROGRAM_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_etppa_prp_fes_program_table_default_get;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->mem = SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_psst_default_get;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->mem = SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_csst_default_get;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->mem = IPPC_FEM_BIT_SELECTm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_ippc_fem_bit_select_default_get;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->mem = IPPC_FEM_MAP_INDEX_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_ippc_fem_map_index_table_default_get;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->mem = TCAM_TCAM_BANK_COMMANDm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_tcam_tcam_bank_command_default_get;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->mem = TCAM_TCAM_ACTIONm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_tcam_tcam_action_default_get;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->mem = TCAM_TCAM_ACTION_SMALLm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_tcam_tcam_action_small_default_get;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->mem = MACSEC_ESEC_SC_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_xflow_macsec_default_esec_sc_entry_get;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->mem = MACSEC_SUB_PORT_POLICY_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_xflow_macsec_default_isec_policy_entry_get;
    }
    if (39 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->mem = ERPP_EPMFCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->mem = IPPA_VTDCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (41 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->mem = IPPA_VTECS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->mem = IPPB_FLPACS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (43 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 43, 0);
        data->mem = IPPB_FLPBCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (44 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->mem = IPPC_PMFACSA_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (45 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 45, 0);
        data->mem = IPPC_PMFACSB_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (46 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->mem = IPPD_PMFBCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (47 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 47, 0);
        data->mem = IPPE_LLRCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (48 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->mem = IPPE_PRTCAM_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (49 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 49, 0);
        data->mem = IPPF_VTACS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (50 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 50, 0);
        data->mem = IPPF_VTBCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (51 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 51, 0);
        data->mem = IPPF_VTCCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (52 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 52, 0);
        data->mem = TCAM_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (53 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 53, 0);
        data->mem = SPEED_ID_TABLEm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (54 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 54, 0);
        data->mem = SPEED_PRIORITY_MAP_TBLm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (55 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 55, 0);
        data->mem = OCB_OCB_CPU_ACCESSm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (56 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 56, 0);
        data->mem = DQM_BDMm;
        data->mode = dnx_init_mem_default_mode_none;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_mem_emul_ext_init_path_set(
    int unit)
{
    dnx_data_dev_init_mem_emul_ext_init_path_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_mem;
    int table_index = dnx_data_dev_init_mem_table_emul_ext_init_path;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "/projects/ntsw_dnx/leon/reset.sh.j2p";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_mem_emul_ext_init_path_t, (1 + 1  ), "data of dnx_data_dev_init_mem_table_emul_ext_init_path");

    
    default_data = (dnx_data_dev_init_mem_emul_ext_init_path_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->full_path = "/projects/ntsw_dnx/leon/reset.sh.j2p";
    
    data = (dnx_data_dev_init_mem_emul_ext_init_path_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[0].property.name = spn_CUSTOM_FEATURE;
    table->values[0].property.doc =
        "\n"
        "Emulation memories external init script full path\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_str;
    table->values[0].property.method_str = "suffix_str";
    table->values[0].property.suffix = "emul_ext_init_path";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_dev_init_mem_emul_ext_init_path_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, -1, &data->full_path));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);
    table->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2p_a0_dnx_data_dev_init_general_data_path_hw_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_data_path_hw;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_remove_crc_bytes_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_remove_crc_bytes;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_remove_crc_bytes_capability_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_remove_crc_bytes_capability;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_erpp_compensate_crc_size_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_erpp_compensate_crc_size;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_etpp_compensate_crc_size_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_etpp_compensate_crc_size;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_tail_editing_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_tail_editing_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_tail_editing_append_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_tail_editing_append_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_tail_editing_truncate_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_tail_editing_truncate_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_packet_size_compensation_term_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_packet_size_compensation_term;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_current_packet_size_compensation_term_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_current_packet_size_compensation_term;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_erpp_filters_non_separate_enablers_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_erpp_filters_non_separate_enablers;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_etppa_bypass_stamp_ftmh_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_etppa_bypass_stamp_ftmh_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_l4_protocol_fields_config_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_l4_protocol_fields_config_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_da_type_map_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_da_type_map_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_ecologic_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_ecologic_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_context_selection_index_valid_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_context_selection_index_valid;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_network_header_termination_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_network_header_termination;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = dnx_data_headers.system_headers.system_headers_mode_get(unit)==dnx_data_headers.system_headers.system_headers_mode_jericho2_get(unit)?1:0;

    
    feature->data = dnx_data_headers.system_headers.system_headers_mode_get(unit)==dnx_data_headers.system_headers.system_headers_mode_jericho2_get(unit)?1:0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_network_offset_for_system_headers_valid_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_network_offset_for_system_headers_valid;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_dev_init_general_access_only_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int define_index = dnx_data_dev_init_general_define_access_only;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "Set to boot with access only.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "access_only";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_heat_up_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int define_index = dnx_data_dev_init_general_define_heat_up;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "Set to boot for heat-up.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "heat_up";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2p_a0_dnx_data_dev_init_context_context_selection_profile_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int feature_index = dnx_data_dev_init_context_context_selection_profile;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_context_oam_sub_type_in_context_selection_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int feature_index = dnx_data_dev_init_context_oam_sub_type_in_context_selection;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_context_int_profile_in_context_selection_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int feature_index = dnx_data_dev_init_context_int_profile_in_context_selection;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_dev_init_context_forwarding_context_selection_mask_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_forwarding_context_selection_mask_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 39;

    
    define->data = 39;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_context_forwarding_context_selection_result_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_forwarding_context_selection_result_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 78;

    
    define->data = 78;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_context_termination_context_selection_mask_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_termination_context_selection_mask_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 50;

    
    define->data = 50;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_context_termination_context_selection_result_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_termination_context_selection_result_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 100;

    
    define->data = 100;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_context_trap_context_selection_mask_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_trap_context_selection_mask_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 31;

    
    define->data = 31;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_context_trap_context_selection_result_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_trap_context_selection_result_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 62;

    
    define->data = 62;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_context_fwd_reycle_priority_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_fwd_reycle_priority_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2p_a0_dnx_data_dev_init_ha_warmboot_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_ha;
    int define_index = dnx_data_dev_init_ha_define_warmboot_support;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_WARMBOOT_SUPPORT;
    define->property.doc = 
        "Specify if WarmBoot is supported\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "on";
    define->property.mapping[0].val = 1;
    define->property.mapping[1].name = "off";
    define->property.mapping[1].val = 0;
    define->property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_ha_sw_state_max_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_ha;
    int define_index = dnx_data_dev_init_ha_define_sw_state_max_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SW_STATE_MAX_SIZE;
    define->property.doc = 
        "\n"
        "Configures the size of memory to be pre allocated for sw state.\n"
        "The valid range is 0-1000000000 (i.e up to 1TB).\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 1000000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_ha_stable_location_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_ha;
    int define_index = dnx_data_dev_init_ha_define_stable_location;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STABLE_LOCATION;
    define->property.doc = 
        "\n"
        "Specify the stable cache option for Warm Boot operations.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 5;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_ha_stable_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_ha;
    int define_index = dnx_data_dev_init_ha_define_stable_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STABLE_SIZE;
    define->property.doc = 
        "\n"
        "Specify the stable cache size in bytes used for Warm boot operations.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 1000000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_dev_init_ha_stable_filename_set(
    int unit)
{
    dnx_data_dev_init_ha_stable_filename_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_ha;
    int table_index = dnx_data_dev_init_ha_table_stable_filename;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "NULL";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_ha_stable_filename_t, (1 + 1  ), "data of dnx_data_dev_init_ha_table_stable_filename");

    
    default_data = (dnx_data_dev_init_ha_stable_filename_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = NULL;
    
    data = (dnx_data_dev_init_ha_stable_filename_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[0].property.name = spn_STABLE_FILENAME;
    table->values[0].property.doc =
        "file system will be used to save the stable cache data with this filename\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_str;
    table->values[0].property.method_str = "str";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_dev_init_ha_stable_filename_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, -1, &data->val));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2p_a0_dnx_data_dev_init_shadow_uncacheable_mem_set(
    int unit)
{
    int index_index;
    dnx_data_dev_init_shadow_uncacheable_mem_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_shadow;
    int table_index = dnx_data_dev_init_shadow_table_uncacheable_mem;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 95;
    table->info_get.key_size[0] = 95;

    
    table->values[0].default_val = "INVALIDm";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_shadow_uncacheable_mem_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dev_init_shadow_table_uncacheable_mem");

    
    default_data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mem = INVALIDm;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->mem = CDPORT_TSC_UCMEM_DATAm;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->mem = FSRD_FSRD_PROM_MEMm;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->mem = ILE_PORT_0_CPU_ACCESSm;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->mem = ILE_PORT_1_CPU_ACCESSm;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->mem = IPPF_LEARN_PAYLOAD_CONTEXT_MAPPINGm;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->mem = SCH_SCHEDULER_INITm;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->mem = ERPP_ERPP_DEBUG_UNITm;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->mem = ETPPA_ETPPA_DEBUG_UNITm;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->mem = ETPPB_ETPPB_DEBUG_UNITm;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->mem = ETPPC_ETPPC_DEBUG_UNITm;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->mem = IPPA_IPPA_DEBUG_UNITm;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->mem = IPPB_IPPB_DEBUG_UNITm;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->mem = IPPC_IPPC_DEBUG_UNITm;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->mem = IPPD_IPPD_DEBUG_UNITm;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->mem = IPPE_IPPE_DEBUG_UNITm;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->mem = IPPF_IPPF_DEBUG_UNITm;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->mem = RX_LKUP_1588_MEM_400Gm;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->mem = RX_LKUP_1588_MEM_MPP0m;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->mem = RX_LKUP_1588_MEM_MPP1m;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->mem = SPEED_ID_TABLEm;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->mem = SPEED_PRIORITY_MAP_TBLm;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->mem = TX_LKUP_1588_MEM_400Gm;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->mem = TX_LKUP_1588_MEM_MPP0m;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->mem = TX_LKUP_1588_MEM_MPP1m;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->mem = UM_TABLEm;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->mem = AM_TABLEm;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->mem = MDB_ARM_KAPS_TCMm;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->mem = ERPP_EPMFCS_TCAM_BANK_COMMANDm;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->mem = IPPA_VTDCS_TCAM_BANK_COMMANDm;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->mem = IPPA_VTECS_TCAM_BANK_COMMANDm;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->mem = IPPB_FLPACS_TCAM_BANK_COMMANDm;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->mem = IPPB_FLPBCS_TCAM_BANK_COMMANDm;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->mem = IPPC_PMFACSA_TCAM_BANK_COMMANDm;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->mem = IPPC_PMFACSB_TCAM_BANK_COMMANDm;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->mem = IPPD_PMFBCS_TCAM_BANK_COMMANDm;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->mem = IPPE_LLRCS_TCAM_BANK_COMMANDm;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->mem = IPPE_PRTCAM_TCAM_BANK_COMMANDm;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->mem = IPPF_VTACS_TCAM_BANK_COMMANDm;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->mem = IPPF_VTBCS_TCAM_BANK_COMMANDm;
    }
    if (39 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->mem = IPPF_VTCCS_TCAM_BANK_COMMANDm;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->mem = TCAM_TCAM_BANK_COMMANDm;
    }
    if (41 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->mem = ERPP_EPMFCS_TCAM_BANK_REPLYm;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->mem = IPPA_VTDCS_TCAM_BANK_REPLYm;
    }
    if (43 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 43, 0);
        data->mem = IPPA_VTECS_TCAM_BANK_REPLYm;
    }
    if (44 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->mem = IPPB_FLPACS_TCAM_BANK_REPLYm;
    }
    if (45 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 45, 0);
        data->mem = IPPB_FLPBCS_TCAM_BANK_REPLYm;
    }
    if (46 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->mem = IPPC_PMFACSA_TCAM_BANK_REPLYm;
    }
    if (47 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 47, 0);
        data->mem = IPPC_PMFACSB_TCAM_BANK_REPLYm;
    }
    if (48 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->mem = IPPD_PMFBCS_TCAM_BANK_REPLYm;
    }
    if (49 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 49, 0);
        data->mem = IPPE_LLRCS_TCAM_BANK_REPLYm;
    }
    if (50 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 50, 0);
        data->mem = IPPE_PRTCAM_TCAM_BANK_REPLYm;
    }
    if (51 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 51, 0);
        data->mem = IPPF_VTACS_TCAM_BANK_REPLYm;
    }
    if (52 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 52, 0);
        data->mem = IPPF_VTBCS_TCAM_BANK_REPLYm;
    }
    if (53 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 53, 0);
        data->mem = IPPF_VTCCS_TCAM_BANK_REPLYm;
    }
    if (54 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 54, 0);
        data->mem = TCAM_TCAM_BANK_REPLYm;
    }
    if (55 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 55, 0);
        data->mem = ERPP_EPMFCS_TCAM_BANKm;
    }
    if (56 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 56, 0);
        data->mem = IPPA_VTDCS_TCAM_BANKm;
    }
    if (57 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 57, 0);
        data->mem = IPPA_VTECS_TCAM_BANKm;
    }
    if (58 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 58, 0);
        data->mem = IPPB_FLPACS_TCAM_BANKm;
    }
    if (59 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 59, 0);
        data->mem = IPPB_FLPBCS_TCAM_BANKm;
    }
    if (60 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 60, 0);
        data->mem = IPPC_PMFACSA_TCAM_BANKm;
    }
    if (61 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 61, 0);
        data->mem = IPPC_PMFACSB_TCAM_BANKm;
    }
    if (62 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 62, 0);
        data->mem = IPPD_PMFBCS_TCAM_BANKm;
    }
    if (63 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 63, 0);
        data->mem = IPPE_LLRCS_TCAM_BANKm;
    }
    if (64 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 64, 0);
        data->mem = IPPE_PRTCAM_TCAM_BANKm;
    }
    if (65 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 65, 0);
        data->mem = IPPF_VTACS_TCAM_BANKm;
    }
    if (66 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 66, 0);
        data->mem = IPPF_VTBCS_TCAM_BANKm;
    }
    if (67 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 67, 0);
        data->mem = IPPF_VTCCS_TCAM_BANKm;
    }
    if (68 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 68, 0);
        data->mem = TCAM_TCAM_BANKm;
    }
    if (69 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 69, 0);
        data->mem = SCH_TOKEN_MEMORY_CONTROLLER_TMCm;
    }
    if (70 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 70, 0);
        data->mem = MDB_ARM_MEM_0m;
    }
    if (71 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 71, 0);
        data->mem = MDB_ISEM_1m;
    }
    if (72 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 72, 0);
        data->mem = MDB_ISEM_2m;
    }
    if (73 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 73, 0);
        data->mem = MDB_ISEM_3m;
    }
    if (74 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 74, 0);
        data->mem = MDB_IOEM_0m;
    }
    if (75 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 75, 0);
        data->mem = MDB_IOEM_1m;
    }
    if (76 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 76, 0);
        data->mem = MDB_MC_IDm;
    }
    if (77 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 77, 0);
        data->mem = MDB_GLEM_0m;
    }
    if (78 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 78, 0);
        data->mem = MDB_GLEM_1m;
    }
    if (79 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 79, 0);
        data->mem = MDB_EOEM_0m;
    }
    if (80 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 80, 0);
        data->mem = MDB_EOEM_1m;
    }
    if (81 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 81, 0);
        data->mem = EVNT_ASSIGNED_CMIC_ENGINE_TABLEm;
    }
    if (82 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 82, 0);
        data->mem = EVNT_SPACE_REGIONSm;
    }
    if (83 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 83, 0);
        data->mem = EVNT_CMIC_ENG_PROPERTIESm;
    }
    if (84 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 84, 0);
        data->mem = OAMP_MEP_DBm;
    }
    if (85 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 85, 0);
        data->mem = OAMP_RMEP_DBm;
    }
    if (86 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 86, 0);
        data->mem = MDB_ESEMm;
    }
    if (87 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 87, 0);
        data->mem = MDB_EXEM_1m;
    }
    if (88 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 88, 0);
        data->mem = MDB_EXEM_2m;
    }
    if (89 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 89, 0);
        data->mem = MDB_RMEPm;
    }
    if (90 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 90, 0);
        data->mem = IPPC_PER_PORT_STATISTICSm;
    }
    if (91 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 91, 0);
        data->mem = SCH_RESERVED_54m;
    }
    if (92 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 92, 0);
        data->mem = SCH_RESERVED_37m;
    }
    if (93 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 93, 0);
        data->mem = ETPPC_LATENCY_TABLEm;
    }
    if (94 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 94, 0);
        data->mem = OAMP_RXP_R_MEP_INDEX_FIFOm;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
j2p_a0_data_dev_init_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_dev_init_submodule_mem;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_dev_init_mem_table_default;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dev_init_mem_default_set;
    data_index = dnx_data_dev_init_mem_table_emul_ext_init_path;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dev_init_mem_emul_ext_init_path_set;
    
    submodule_index = dnx_data_dev_init_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dev_init_general_define_access_only;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_general_access_only_set;
    data_index = dnx_data_dev_init_general_define_heat_up;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_general_heat_up_set;

    
    data_index = dnx_data_dev_init_general_data_path_hw;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_data_path_hw_set;
    data_index = dnx_data_dev_init_general_remove_crc_bytes;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_remove_crc_bytes_set;
    data_index = dnx_data_dev_init_general_remove_crc_bytes_capability;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_remove_crc_bytes_capability_set;
    data_index = dnx_data_dev_init_general_erpp_compensate_crc_size;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_erpp_compensate_crc_size_set;
    data_index = dnx_data_dev_init_general_etpp_compensate_crc_size;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_etpp_compensate_crc_size_set;
    data_index = dnx_data_dev_init_general_tail_editing_enable;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_tail_editing_enable_set;
    data_index = dnx_data_dev_init_general_tail_editing_append_enable;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_tail_editing_append_enable_set;
    data_index = dnx_data_dev_init_general_tail_editing_truncate_enable;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_tail_editing_truncate_enable_set;
    data_index = dnx_data_dev_init_general_packet_size_compensation_term;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_packet_size_compensation_term_set;
    data_index = dnx_data_dev_init_general_current_packet_size_compensation_term;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_current_packet_size_compensation_term_set;
    data_index = dnx_data_dev_init_general_erpp_filters_non_separate_enablers;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_erpp_filters_non_separate_enablers_set;
    data_index = dnx_data_dev_init_general_etppa_bypass_stamp_ftmh_enable;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_etppa_bypass_stamp_ftmh_enable_set;
    data_index = dnx_data_dev_init_general_l4_protocol_fields_config_enable;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_l4_protocol_fields_config_enable_set;
    data_index = dnx_data_dev_init_general_da_type_map_enable;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_da_type_map_enable_set;
    data_index = dnx_data_dev_init_general_ecologic_support;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_ecologic_support_set;
    data_index = dnx_data_dev_init_general_context_selection_index_valid;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_context_selection_index_valid_set;
    data_index = dnx_data_dev_init_general_network_header_termination;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_network_header_termination_set;
    data_index = dnx_data_dev_init_general_network_offset_for_system_headers_valid;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_network_offset_for_system_headers_valid_set;

    
    
    submodule_index = dnx_data_dev_init_submodule_context;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dev_init_context_define_forwarding_context_selection_mask_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_forwarding_context_selection_mask_offset_set;
    data_index = dnx_data_dev_init_context_define_forwarding_context_selection_result_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_forwarding_context_selection_result_offset_set;
    data_index = dnx_data_dev_init_context_define_termination_context_selection_mask_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_termination_context_selection_mask_offset_set;
    data_index = dnx_data_dev_init_context_define_termination_context_selection_result_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_termination_context_selection_result_offset_set;
    data_index = dnx_data_dev_init_context_define_trap_context_selection_mask_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_trap_context_selection_mask_offset_set;
    data_index = dnx_data_dev_init_context_define_trap_context_selection_result_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_trap_context_selection_result_offset_set;
    data_index = dnx_data_dev_init_context_define_fwd_reycle_priority_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_fwd_reycle_priority_size_set;

    
    data_index = dnx_data_dev_init_context_context_selection_profile;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_context_context_selection_profile_set;
    data_index = dnx_data_dev_init_context_oam_sub_type_in_context_selection;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_context_oam_sub_type_in_context_selection_set;
    data_index = dnx_data_dev_init_context_int_profile_in_context_selection;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_context_int_profile_in_context_selection_set;

    
    
    submodule_index = dnx_data_dev_init_submodule_ha;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dev_init_ha_define_warmboot_support;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_ha_warmboot_support_set;
    data_index = dnx_data_dev_init_ha_define_sw_state_max_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_ha_sw_state_max_size_set;
    data_index = dnx_data_dev_init_ha_define_stable_location;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_ha_stable_location_set;
    data_index = dnx_data_dev_init_ha_define_stable_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_ha_stable_size_set;

    

    
    data_index = dnx_data_dev_init_ha_table_stable_filename;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dev_init_ha_stable_filename_set;
    
    submodule_index = dnx_data_dev_init_submodule_shadow;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_dev_init_shadow_table_uncacheable_mem;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dev_init_shadow_uncacheable_mem_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


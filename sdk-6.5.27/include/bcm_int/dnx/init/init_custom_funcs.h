/** \file init_custom_funcs.h
 * Custom functions to be used to initialize DNX memories. 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_INIT_CUSTOM_FUNCS_H_INCLUDED
/*
 * { 
 */
#define _BCMINT_DNX_INIT_CUSTOM_FUNCS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Include files.
 * {
 */

/*
 * }
 */

/**
 * \brief - set entry_data with the init value of SCH_FLOW_TO_FIP_MAPPING_FFM  memory
 * 
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_sch_flow_to_fip_mapping_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set entry_data with the init value of  SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS memory
 * 
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_sch_shaper_descr_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set entry_data with the init value of  SCH_PORT_SCHEDULER_WEIGHTS_PSW memory
 * 
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_sch_port_schduler_weights_psw_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set entry_data with the init value of  SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm memory
 * 
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_sch_flow_descr_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set entry_data with the init value of IPT_PCP_CFGm memory
 * 
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_fabric_pcp_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set entry_data with the init value of ERPP_TM_PP_DSP_PTR_TABLEm memory
 *
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_erpp_tm_pp_dsp_ptr_table_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set entry_data with the init value of ERPP_PRP_PP_DSP_PTR_TABLEm memory
 *
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_erpp_prp_pp_dsp_ptr_table_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set entry_data with the init value of ETPPA_DSP_DATA_TABLEm memory
 *
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_etppa_dsp_data_table_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set entry_data with the init value of IPPE_PRT_PP_PORT_INFO memory
 *
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_ippe_prt_pp_port_info_table_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set entry_data with the init value of ETPPA_PRP_FES_PROGRAM_TABLEm memory
 *
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_etppa_prp_fes_program_table_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set entry_data with the init value of SCH_CIR_SHAPERS_STATIC_TABEL_CSST memory
 *
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_sch_csst_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set entry_data with the init value of SCH_PIR_SHAPERS_STATIC_TABEL_PSST memory
 *
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_sch_psst_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set entry_data with the init value of CGM_IPP_MAP memory
 *
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_cgm_ipp_map_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set entry_data with the init value of IPPC_FEM_BIT_SELECTm memory
 *
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_ippc_fem_bit_select_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set entry_data with the init value of TCAM_TCAM_BANK_COMMANDm memory
 *
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_tcam_tcam_bank_command_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set MTU of MACSEC_ESEC_SC_TABLE to 64
 *
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_xflow_macsec_default_esec_sc_entry_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set MTU of MACSEC_SUB_PORT_POLICY_TABLE to 300
 *
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_xflow_macsec_default_isec_policy_entry_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - Set IPPB_PINFO_FLP_MEM ACTION_PROFILE_SA_NOT_FOUND_INDEX to 1
 *
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_sa_not_found_action_profile_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - return EPS_CH_IFC_CFG default value per array_index
 *
 * \param [in] unit - unit number
 * \param [in] array_index - array index
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_eps_ch_ifc_cfg_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - return EPS_CH_IFC_CFG default value per array_index
 *
 * \param [in] unit - unit number
 * \param [in] array_index - array index
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_custom_arb_tx_tmac_cal_mem_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

#ifdef BCM_ACCESS_SUPPORT
/**
 * \brief - set entry_data with the init value of SCH_FLOW_TO_FIP_MAPPING_FFM  memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_sch_flow_to_fip_mapping_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - set entry_data with the init value of  SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data   - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_sch_shaper_descr_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - set entry_data with the init value of  SCH_PORT_SCHEDULER_WEIGHTS_PSW memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data   - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_sch_port_scheduler_weights_psw_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - set entry_data with the init value of  SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_sch_flow_descr_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - set entry_data with the init value of IPT_PCP_CFGm memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_fabric_pcp_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - set entry_data with the init value of ERPP_TM_PP_DSP_PTR_TABLEm memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_erpp_tm_pp_dsp_ptr_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - set entry_data with the init value of ERPP_PRP_PP_DSP_PTR_TABLEm memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_erpp_prp_pp_dsp_ptr_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - set entry_data with the init value of ETPPA_DSP_DATA_TABLEm memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data   - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_etppa_dsp_data_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - set entry_data with the init value of IPPE_PRT_PP_PORT_INFO memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_ippe_prt_pp_port_info_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - set entry_data with the init value of ETPPA_PRP_FES_PROGRAM_TABLEm memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_etppa_prp_fes_program_table_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - set entry_data with the init value of SCH_CIR_SHAPERS_STATIC_TABEL_CSST memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_sch_csst_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - set entry_data with the init value of SCH_PIR_SHAPERS_STATIC_TABEL_PSST memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_sch_psst_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - set entry_data with the init value of CGM_IPP_MAP memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_cgm_ipp_map_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - set entry_data with the init value of IPPC_FEM_BIT_SELECTm memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_ippc_fem_bit_select_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - set entry_data with the init value of TCAM_TCAM_BANK_COMMANDm memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] mem_info     - irrelevant for this cb
 * \param [in] array_index - irrelevant for this cb
 * \param [in] index       - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_tcam_tcam_bank_command_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    dnx_sw_init_mem_info * mem_info,
    unsigned array_index,
    int index,
    uint32 *entry_data);

/**
 * \brief - set MTU of MACSEC_ESEC_SC_TABLE to 64
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_xflow_macsec_esec_sc_entry_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - set MTU of MACSEC_SUB_PORT_POLICY_TABLE to 300
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_xflow_macsec_isec_policy_entry_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - Set IPPB_PINFO_FLP_MEM ACTION_PROFILE_SA_NOT_FOUND_INDEX to 1
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_sa_not_found_action_profile_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - return EPS_CH_IFC_CFG default value per array_index
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] mem_info     - irrelevant for this cb
 * \param [in] array_index - irrelevant for this cb
 * \param [in] index       - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_eps_ch_ifc_cfg_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    dnx_sw_init_mem_info * mem_info,
    unsigned array_index,
    int index,
    uint32 *entry_data);

/**
 * \brief - return EPS_CH_IFC_CFG default value per array_index
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_arb_tx_tmac_cal_mem_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - Call Back function for mem_init mechanism, will init the context MRU to simulation values
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_spb_context_mru_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - Call Back function for mem_init mechanism, sets index value
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] mem_info     - the mem_id and field_id to set
 * \param [in] array_index - irrelevant for this cb
 * \param [in] index       - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_index_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    dnx_sw_init_mem_info * mem_info,
    unsigned array_index,
    int index,
    uint32 *entry_data);

/**
 * \brief - Call Back function for mem_init mechanism, sets index seq array value
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] mem_info     - the mem_id and field_id to set
 * \param [in] array_index - irrelevant for this cb
 * \param [in] index       - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_index_seq_array_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    dnx_sw_init_mem_info * mem_info,
    unsigned array_index,
    int index,
    uint32 *entry_data);

/**
 * \brief - Call Back function for mem_init mechanism, sets all ones value
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_fill_all_ones_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 *
 * \brief - set entry_data with the init value of mIPPE_PINFO_LLR memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_ippe_pinfo_llr_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 *
 * \brief - set entry_data with the init value of mIPPC_FES_2ND_INSTRUCTION memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_ippc_fes_2nd_instruction_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 *
 * \brief - set entry_data with the init value of mIPPD_FES_2ND_INSTRUCTION memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_ippd_fes_2nd_instruction_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 *
 * \brief - set entry_data with the init value of mIPPD_FES_2ND_INSTRUCTION memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_erpp_fes_2nd_instruction_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 *
 * \brief - set entry_data with the init value of mETPPB_MTU_MAP_TABLE memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_etppb_mtu_map_table_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 *
 * \brief - set entry_data with the init value of mETPPC_MTU_MAP_TABLE memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_etppc_mtu_map_table_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 *
 * \brief - set entry_data with the init value of mSCH_SLOW_SCALE_A_SSA memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_sch_slow_scale_a_ssa_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 *
 * \brief - set entry_data with the init value of mSCH_SLOW_SCALE_B_SSB memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_sch_slow_scale_b_ssb_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 *
 * \brief - set entry_data with the init value of mIPS_QSPM memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_ips_qspm_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 *
 * \brief - set entry_data with the init value of mCGM_QSPM memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_cgm_qspm_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 *
 * \brief - set entry_data with the init value of mIPS_QPRIORITY memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_ips_qpriority_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 *
 * \brief - set entry_data with the init value of mEPNI_ALIGNER_MIRR_QP_TO_CHANNEL_MAP memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_epni_aligner_mirr_qp_to_channel_map_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 *
 * \brief - set entry_data with the init value of mERPP_CFG_ENABLE_FILTER_PER_PORT_TABLE memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_erpp_cfg_enable_filter_per_port_table_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 *
 * \brief - set entry_data with the init value of mETPPA_FES_2ND_INSTRUCTION memory
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_etppa_fes_2nd_instruction_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);

/**
 * \brief - Call Back function for mem_init mechanism, sets all zeros value
 *
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] entry_data - returned value to set to memory
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_access_init_custom_fill_all_zeros_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data);
#endif /* BCM_ACCESS_SUPPORT */
#endif /** _BCMINT_DNX_INIT_CUSTOM_FUNCS_H_INCLUDED */

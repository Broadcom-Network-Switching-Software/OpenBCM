
/** \file init_custom_funcs.c
 * Custom functions to be used to initialize DNX memories. 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MEM

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/mem.h>
#include <soc/types.h>
#include <soc/sand/sand_mem.h>
#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#include <soc/access/auto_generated/common_enum.h>
#endif /* BCM_ACCESS_SUPPORT */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/init/init_mem.h>

/*
 * The following functions are used to initialize memories of 
 * Jericho2
 *
 * For other devices,
 * a different version of the following functions might be required
 */

shr_error_e
dnx_init_custom_sch_flow_to_fip_mapping_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /** entry_data is already cleaned */
    soc_mem_field32_set(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, entry_data, DEVICE_NUMBERf,
                        dnx_data_device.general.invalid_fap_id_get(unit));

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_sch_port_schduler_weights_psw_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /** entry_data is already cleaned */

    /*
     * set all weight to default init value 
     */
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_0_WEIGHTf, 0x1);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_1_WEIGHTf, 0x2);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_2_WEIGHTf, 0x4);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_3_WEIGHTf, 0x8);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_4_WEIGHTf, 0x10);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_5_WEIGHTf, 0x20);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_6_WEIGHTf, 0x40);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, entry_data, WFQ_PG_7_WEIGHTf, 0x80);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_sch_shaper_descr_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    int i, nof_fields = 8;

    soc_field_t shaper_mantissa_field[] = {
        PEAK_RATE_MAN_0f,
        PEAK_RATE_MAN_1f,
        PEAK_RATE_MAN_2f,
        PEAK_RATE_MAN_3f,
        PEAK_RATE_MAN_4f,
        PEAK_RATE_MAN_5f,
        PEAK_RATE_MAN_6f,
        PEAK_RATE_MAN_7f
    };

    soc_field_t shaper_max_burst_field[] = {
        MAX_BURST_0f,
        MAX_BURST_1f,
        MAX_BURST_2f,
        MAX_BURST_3f,
        MAX_BURST_4f,
        MAX_BURST_5f,
        MAX_BURST_6f,
        MAX_BURST_7f
    };

    uint32 shaper_mantissa_max_value = utilex_max_value_by_size(dnx_data_sch.dbal.flow_shaper_mant_bits_get(unit));
    uint32 shaper_max_burst_value = utilex_max_value_by_size(dnx_data_sch.dbal.flow_shaper_max_burst_bits_get(unit));

    SHR_FUNC_INIT_VARS(unit);

    /** entry_data is already cleaned */

    /*
     * set all shapers to maximal possible rate and maximal max_burst 
     * the shaper is unlimited by default 
     */
    for (i = 0; i < nof_fields; i++)
    {
        /** set the shaper rate */
        soc_mem_field32_set(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, entry_data, shaper_mantissa_field[i],
                            shaper_mantissa_max_value);
        /** set the shaper max burst */
        soc_mem_field32_set(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, entry_data, shaper_max_burst_field[i],
                            shaper_max_burst_value);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_sch_flow_descr_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /** entry_data is already cleaned */

    /** set all flows to be attached to reserved SE */
    soc_mem_field32_set(unit, SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm, entry_data, SCH_NUMBERf,
                        dnx_data_sch.flow.default_credit_source_se_id_get(unit));

    /*
     * set cos value matching to dpp code 
     * could be set to 0, when all ported code is re-implemented
     */
    soc_mem_field32_set(unit, SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm, entry_data, COSf,
                        dnx_data_sch.se.default_cos_get(unit));
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_fabric_pcp_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.cell.pcp_enable_get(unit) == 1)
    {
        uint32 default_packing_mode = dnx_data_fabric.cell.default_packing_mode_get(unit);
        uint32 packing_config = 0;
        int field_length = soc_mem_field_length(unit, IPT_PCP_CFGm, PACKING_CONFf);
        int offset = 0;
        /*
         * Packing config per destination FAP.
         * Each line in the table consists of 16 FAPS.
         * Set continuous packing by default to all the FAPs.
         * (2 bits for a fap: 0x0 - disabled; 0x1 - simple packing; 0x2 - continuous packing)
         */
        for (offset = 0; offset < field_length; offset += 2)
        {
            SHR_BITCOPY_RANGE(&packing_config, offset, &default_packing_mode, 0, 2);
        }
        soc_mem_field_set(unit, IPT_PCP_CFGm, entry_data, PACKING_CONFf, &packing_config);
    }
    else
    {
        /** Do nothing. Assuming entry_data is 0 */
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_erpp_tm_pp_dsp_ptr_table_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field32_set(unit, ERPP_TM_PP_DSP_PTR_TABLEm, entry_data, OUT_TM_PORTf,
                        dnx_data_egr_queuing.params.invalid_otm_port_get(unit));
    soc_mem_field32_set(unit, ERPP_TM_PP_DSP_PTR_TABLEm, entry_data, DST_SYS_PORTf,
                        dnx_data_device.general.invalid_system_port_get(unit));

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_erpp_prp_pp_dsp_ptr_table_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field32_set(unit, ERPP_PRP_PP_DSP_PTR_TABLEm, entry_data, OUT_TM_PORTf,
                        dnx_data_egr_queuing.params.invalid_otm_port_get(unit));
    soc_mem_field32_set(unit, ERPP_PRP_PP_DSP_PTR_TABLEm, entry_data, DST_SYS_PORTf,
                        dnx_data_device.general.invalid_system_port_get(unit));

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_etppa_dsp_data_table_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field32_set(unit, ETPPA_DSP_DATA_TABLEm, entry_data, OUT_TM_PORTf,
                        dnx_data_egr_queuing.params.invalid_otm_port_get(unit));
    soc_mem_field32_set(unit, ETPPA_DSP_DATA_TABLEm, entry_data, DST_SYS_PORTf,
                        dnx_data_device.general.invalid_system_port_get(unit));
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_ippe_prt_pp_port_info_table_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field32_set(unit, IPPE_PRT_PP_PORT_INFOm, entry_data, SRC_SYS_PORTf,
                        dnx_data_device.general.invalid_system_port_get(unit));
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_etppa_prp_fes_program_table_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    int action_type_field_index;
    int nof_action_type_fields;
    uint32 hw_value;
    soc_field_t action_type_fields[] = {
        ACTION_TYPE_0f,
        ACTION_TYPE_1f,
        ACTION_TYPE_2f,
        ACTION_TYPE_3f,
        ACTION_TYPE_4f,
        ACTION_TYPE_5f,
        ACTION_TYPE_6f,
        ACTION_TYPE_7f
    };

    SHR_FUNC_INIT_VARS(unit);

    nof_action_type_fields = 8;

    hw_value = dnx_data_field.ace.hw_invalid_action_id_get(unit);
    for (action_type_field_index = 0; action_type_field_index < nof_action_type_fields; action_type_field_index++)
    {
        soc_mem_field32_set(unit, ETPPA_PRP_FES_PROGRAM_TABLEm, entry_data, action_type_fields[action_type_field_index],
                            hw_value);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_sch_psst_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    int max_value;
    soc_field_t fields[] = { MAX_BURST_0f, MAX_BURST_1f, MAX_BURST_2f, MAX_BURST_3f,
        MAX_BURST_4f, MAX_BURST_5f, MAX_BURST_6f, MAX_BURST_7f
    };
    int i;

    SHR_FUNC_INIT_VARS(unit);

    max_value = dnx_data_sch.ps.max_burst_get(unit);

    for (i = 0; i < 8; i++)
    {
        soc_mem_field32_set(unit, SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm, entry_data, fields[i], max_value);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_sch_csst_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    int max_value;
    soc_field_t fields[] = { MAX_BURST_0f, MAX_BURST_1f, MAX_BURST_2f, MAX_BURST_3f,
        MAX_BURST_4f, MAX_BURST_5f, MAX_BURST_6f, MAX_BURST_7f
    };
    int i;

    SHR_FUNC_INIT_VARS(unit);

    max_value = dnx_data_sch.ps.max_burst_get(unit);

    for (i = 0; i < 8; i++)
    {
        soc_mem_field32_set(unit, SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm, entry_data, fields[i], max_value);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_cgm_ipp_map_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{

    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field32_set(unit, CGM_IPP_MAPm, entry_data, NIF_PORTf,
                        dnx_data_ingr_congestion.vsq.vsq_e_default_get(unit));
    soc_mem_field32_set(unit, CGM_IPP_MAPm, entry_data, PG_BASEf, dnx_data_ingr_congestion.vsq.vsq_f_default_get(unit));

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_ippc_fem_bit_select_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    dnx_field_fem_condition_ms_bit_t bit_select_init_val;

    SHR_FUNC_INIT_VARS(unit);

    bit_select_init_val =
        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->fem_condition_ms_bit_min_value;

    soc_mem_field32_set(unit, IPPC_FEM_BIT_SELECTm, entry_data, BIT_SELECTf, bit_select_init_val);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_tcam_tcam_bank_command_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    uint32 data[BITS2WORDS(DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_DOUBLE) + 1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set WR CMD only for valid entries that can be written to (even entries within bank's limit) 
     */
    if (index % 2 == 0 && DNX_FIELD_TCAM_LOCATION_IS_VALID(index))
    {
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, entry_data, TCAM_CPU_CMD_WRf, 0x3);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, entry_data, TCAM_CPU_CMD_RDf, 0x0);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, entry_data, TCAM_CPU_CMD_CMPf, 0x0);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, entry_data, TCAM_CPU_CMD_VALIDf, 0x0);
        soc_mem_field_set(unit, TCAM_TCAM_BANK_COMMANDm, entry_data, TCAM_CPU_CMD_DIf, data);
    }
    else
    {
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, entry_data, TCAM_CPU_CMD_WRf, 0x0);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, entry_data, TCAM_CPU_CMD_RDf, 0x0);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, entry_data, TCAM_CPU_CMD_CMPf, 0x0);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_xflow_macsec_default_esec_sc_entry_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    uint32 data = 64;

    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field_set(unit, MACSEC_ESEC_SC_TABLEm, entry_data, MTUf, &data);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_xflow_macsec_default_isec_policy_entry_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    uint32 data = 300;

    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field_set(unit, MACSEC_SUB_PORT_POLICY_TABLEm, entry_data, MTUf, &data);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_sa_not_found_action_profile_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{

    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field32_set(unit, IPPB_PINFO_FLP_MEMm, entry_data, ACTION_PROFILE_SA_NOT_FOUND_INDEXf,
                        DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND1 -
                        DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND0);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_eps_ch_ifc_cfg_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{

    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field32_set(unit, EPS_CH_IFC_CFGm, entry_data, IFC_IS_CHANNELIZEDf, 0);

    /** mask array_index upto MAP_IFC_TO_CH_IDf size */
    soc_mem_field32_set(unit, EPS_CH_IFC_CFGm, entry_data, MAP_IFC_TO_CH_IDf,
                        array_index & SAL_UPTO_BIT(soc_mem_field_length(unit, EPS_CH_IFC_CFGm, MAP_IFC_TO_CH_IDf)));

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_custom_arb_tx_tmac_cal_mem_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    soc_reg_above_64_val_t field_above_64_val = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Value is 10 times 320 which is bubble for arbiter tmac calendar 
     */
    field_above_64_val[2] = 0x028140a0;
    field_above_64_val[1] = 0x5028140a;
    field_above_64_val[0] = 0x05028140;

    soc_mem_field_set(unit, ARB_TX_TMAC_CAL_MEMm, entry_data, CAL_ENTRYX_Nf, field_above_64_val);

    SHR_FUNC_EXIT;
}

#ifdef BCM_ACCESS_SUPPORT
/**
 * \brief - set entry_data with the init value of SCH_FLOW_TO_FIP_MAPPING_FFM  memory
 */
shr_error_e
dnx_access_init_custom_sch_flow_to_fip_mapping_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 sch_flow_to_fip_mapping_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        /** entry_data is already cleaned */
        access_field32_set(runtime_info, mSCH_FLOW_TO_FIP_MAPPING_FFM_fDEVICE_NUMBER, sch_flow_to_fip_mapping_value,
                           dnx_data_device.general.invalid_fap_id_get(unit));
        first_time = FALSE;
    }

    *entry_data = sch_flow_to_fip_mapping_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set entry_data with the init value of  SCH_PORT_SCHEDULER_WEIGHTS_PSW memory
 */
shr_error_e
dnx_access_init_custom_sch_port_scheduler_weights_psw_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 sch_port_schduler_weights_psw_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** entry_data is already cleaned */

    if (first_time)
    {
        /*
         * set all weight to default init value
         */
        access_field32_set(runtime_info, mSCH_PORT_SCHEDULER_WEIGHTS_PSW_fWFQ_PG_0_WEIGHT,
                           sch_port_schduler_weights_psw_value, 0x1);
        access_field32_set(runtime_info, mSCH_PORT_SCHEDULER_WEIGHTS_PSW_fWFQ_PG_1_WEIGHT,
                           sch_port_schduler_weights_psw_value, 0x2);
        access_field32_set(runtime_info, mSCH_PORT_SCHEDULER_WEIGHTS_PSW_fWFQ_PG_2_WEIGHT,
                           sch_port_schduler_weights_psw_value, 0x4);
        access_field32_set(runtime_info, mSCH_PORT_SCHEDULER_WEIGHTS_PSW_fWFQ_PG_3_WEIGHT,
                           sch_port_schduler_weights_psw_value, 0x8);
        access_field32_set(runtime_info, mSCH_PORT_SCHEDULER_WEIGHTS_PSW_fWFQ_PG_4_WEIGHT,
                           sch_port_schduler_weights_psw_value, 0x10);
        access_field32_set(runtime_info, mSCH_PORT_SCHEDULER_WEIGHTS_PSW_fWFQ_PG_5_WEIGHT,
                           sch_port_schduler_weights_psw_value, 0x20);
        access_field32_set(runtime_info, mSCH_PORT_SCHEDULER_WEIGHTS_PSW_fWFQ_PG_6_WEIGHT,
                           sch_port_schduler_weights_psw_value, 0x40);
        access_field32_set(runtime_info, mSCH_PORT_SCHEDULER_WEIGHTS_PSW_fWFQ_PG_7_WEIGHT,
                           sch_port_schduler_weights_psw_value, 0x80);
        first_time = FALSE;
    }

    *entry_data = sch_port_schduler_weights_psw_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set entry_data with the init value of  SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS memory
 */
shr_error_e
dnx_access_init_custom_sch_shaper_descr_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 sch_shaper_descr_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };
    int i, nof_fields = 8;

    access_field_id_t shaper_mantissa_field[] = {
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fPEAK_RATE_MAN_0,
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fPEAK_RATE_MAN_1,
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fPEAK_RATE_MAN_2,
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fPEAK_RATE_MAN_3,
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fPEAK_RATE_MAN_4,
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fPEAK_RATE_MAN_5,
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fPEAK_RATE_MAN_6,
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fPEAK_RATE_MAN_7
    };

    access_field_id_t shaper_max_burst_field[] = {
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fMAX_BURST_0,
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fMAX_BURST_1,
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fMAX_BURST_2,
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fMAX_BURST_3,
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fMAX_BURST_4,
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fMAX_BURST_5,
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fMAX_BURST_6,
        mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_fMAX_BURST_7
    };

    uint32 shaper_mantissa_max_value = utilex_max_value_by_size(dnx_data_sch.dbal.flow_shaper_mant_bits_get(unit));
    uint32 shaper_max_burst_value = utilex_max_value_by_size(dnx_data_sch.dbal.flow_shaper_max_burst_bits_get(unit));

    SHR_FUNC_INIT_VARS(unit);

    /** entry_data is already cleaned */
    if (first_time)
    {
        /*
         * set all shapers to maximal possible rate and maximal max_burst
         * the shaper is unlimited by default
         */
        for (i = 0; i < nof_fields; i++)
        {
            /** set the shaper rate */
            access_field32_set(runtime_info, shaper_mantissa_field[i], sch_shaper_descr_value,
                               shaper_mantissa_max_value);
            /** set the shaper max burst */
            access_field32_set(runtime_info, shaper_max_burst_field[i], sch_shaper_descr_value, shaper_max_burst_value);
        }

        first_time = FALSE;
    }

    *entry_data = sch_shaper_descr_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set entry_data with the init value of  SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm memory
 */
shr_error_e
dnx_access_init_custom_sch_flow_descr_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 sch_flow_descr_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** entry_data is already cleaned */
    if (first_time)
    {
        /** set all flows to be attached to reserved SE */
        access_field32_set(runtime_info, mSCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMS_fSCH_NUMBER, sch_flow_descr_value,
                           dnx_data_sch.flow.default_credit_source_se_id_get(unit));

        /*
         * set cos value matching to dpp code
         * could be set to 0, when all ported code is re-implemented
         */
        access_field32_set(runtime_info, mSCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMS_fCOS, sch_flow_descr_value,
                           dnx_data_sch.se.default_cos_get(unit));
        first_time = FALSE;
    }

    *entry_data = sch_flow_descr_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set entry_data with the init value of IPT_PCP_CFGm memory
 */
shr_error_e
dnx_access_init_custom_fabric_pcp_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 fabric_pcp_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        if (dnx_data_fabric.cell.pcp_enable_get(unit) == 1)
        {
            uint32 default_packing_mode = dnx_data_fabric.cell.default_packing_mode_get(unit);
            uint32 packing_config = 0;
            const access_device_field_t *field_info = device_info->local_fields + mIPT_PCP_CFG_fPACKING_CONF;
            int field_length = field_info->size_in_bits;
            int offset = 0;
            /*
             * Packing config per destination FAP.
             * Each line in the table consists of 16 FAPS.
             * Set continuous packing by default to all the FAPs.
             * (2 bits for a fap: 0x0 - disabled; 0x1 - simple packing; 0x2 - continuous packing)
             */
            for (offset = 0; offset < field_length; offset += 2)
            {
                SHR_BITCOPY_RANGE(&packing_config, offset, &default_packing_mode, 0, 2);
            }
            access_field_set(unit, mIPT_PCP_CFG_fPACKING_CONF, fabric_pcp_value, &packing_config);
        }
        else
        {
            /** Do nothing. Assuming entry_data is 0 */
        }
        first_time = FALSE;
    }

    *entry_data = fabric_pcp_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set entry_data with the init value of ERPP_TM_PP_DSP_PTR_TABLEm memory
 */
shr_error_e
dnx_access_init_custom_erpp_tm_pp_dsp_ptr_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 erpp_tm_pp_dsp_ptr_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field32_set(runtime_info, mERPP_TM_PP_DSP_PTR_TABLE_fOUT_TM_PORT, erpp_tm_pp_dsp_ptr_value,
                           dnx_data_egr_queuing.params.invalid_otm_port_get(unit));
        access_field32_set(runtime_info, mERPP_TM_PP_DSP_PTR_TABLE_fDST_SYS_PORT, erpp_tm_pp_dsp_ptr_value,
                           dnx_data_device.general.invalid_system_port_get(unit));
        first_time = FALSE;
    }

    *entry_data = erpp_tm_pp_dsp_ptr_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set entry_data with the init value of ERPP_PRP_PP_DSP_PTR_TABLEm memory
 */
shr_error_e
dnx_access_init_custom_erpp_prp_pp_dsp_ptr_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 erpp_prp_pp_dsp_ptr_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field32_set(runtime_info, mERPP_PRP_PP_DSP_PTR_TABLE_fOUT_TM_PORT, erpp_prp_pp_dsp_ptr_value,
                           dnx_data_egr_queuing.params.invalid_otm_port_get(unit));
        access_field32_set(runtime_info, mERPP_PRP_PP_DSP_PTR_TABLE_fDST_SYS_PORT, erpp_prp_pp_dsp_ptr_value,
                           dnx_data_device.general.invalid_system_port_get(unit));
        first_time = FALSE;
    }

    *entry_data = erpp_prp_pp_dsp_ptr_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set entry_data with the init value of ETPPA_DSP_DATA_TABLEm memory
 */
shr_error_e
dnx_access_init_custom_etppa_dsp_data_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 etppa_dsp_data_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field32_set(runtime_info, mETPPA_DSP_DATA_TABLE_fOUT_TM_PORT, etppa_dsp_data_value,
                           dnx_data_egr_queuing.params.invalid_otm_port_get(unit));
        access_field32_set(runtime_info, mETPPA_DSP_DATA_TABLE_fDST_SYS_PORT, etppa_dsp_data_value,
                           dnx_data_device.general.invalid_system_port_get(unit));
        first_time = FALSE;
    }

    *entry_data = etppa_dsp_data_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set entry_data with the init value of IPPE_PRT_PP_PORT_INFO memory
 */
shr_error_e
dnx_access_init_custom_ippe_prt_pp_port_info_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 ippe_prt_pp_port_info_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field32_set(runtime_info, mIPPE_PRT_PP_PORT_INFO_fSRC_SYS_PORT, ippe_prt_pp_port_info_value,
                           dnx_data_device.general.invalid_system_port_get(unit));

        first_time = FALSE;
    }

    *entry_data = ippe_prt_pp_port_info_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set entry_data with the init value of ETPPA_PRP_FES_PROGRAM_TABLEm memory
 */
shr_error_e
dnx_access_init_custom_etppa_prp_fes_program_table_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 etppa_prp_fes_program_table_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };
    int action_type_field_index;
    int nof_action_type_fields;

    uint32 hw_value;
    access_field_id_t action_type_fields[] = {
        mETPPA_PRP_FES_PROGRAM_TABLE_fACTION_TYPE_0,
        mETPPA_PRP_FES_PROGRAM_TABLE_fACTION_TYPE_1,
        mETPPA_PRP_FES_PROGRAM_TABLE_fACTION_TYPE_2,
        mETPPA_PRP_FES_PROGRAM_TABLE_fACTION_TYPE_3,
        mETPPA_PRP_FES_PROGRAM_TABLE_fACTION_TYPE_4,
        mETPPA_PRP_FES_PROGRAM_TABLE_fACTION_TYPE_5,
        mETPPA_PRP_FES_PROGRAM_TABLE_fACTION_TYPE_6,
        mETPPA_PRP_FES_PROGRAM_TABLE_fACTION_TYPE_7
    };

    SHR_FUNC_INIT_VARS(unit);

    hw_value = dnx_data_field.ace.hw_invalid_action_id_get(unit);
    if (first_time)
    {
        nof_action_type_fields = 8;

        for (action_type_field_index = 0; action_type_field_index < nof_action_type_fields; action_type_field_index++)
        {
            access_field32_set(runtime_info, action_type_fields[action_type_field_index],
                               etppa_prp_fes_program_table_value, hw_value);
        }

        first_time = FALSE;
    }

    *entry_data = etppa_prp_fes_program_table_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set entry_data with the init value of SCH_CIR_SHAPERS_STATIC_TABEL_PSST memory
 */
shr_error_e
dnx_access_init_custom_sch_psst_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 sch_psst_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };
    int max_value;
    access_field_id_t fields[] = {
        mSCH_PIR_SHAPERS_STATIC_TABEL_PSST_fMAX_BURST_0,
        mSCH_PIR_SHAPERS_STATIC_TABEL_PSST_fMAX_BURST_1,
        mSCH_PIR_SHAPERS_STATIC_TABEL_PSST_fMAX_BURST_2,
        mSCH_PIR_SHAPERS_STATIC_TABEL_PSST_fMAX_BURST_3,
        mSCH_PIR_SHAPERS_STATIC_TABEL_PSST_fMAX_BURST_4,
        mSCH_PIR_SHAPERS_STATIC_TABEL_PSST_fMAX_BURST_5,
        mSCH_PIR_SHAPERS_STATIC_TABEL_PSST_fMAX_BURST_6,
        mSCH_PIR_SHAPERS_STATIC_TABEL_PSST_fMAX_BURST_7
    };
    int i;

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        max_value = dnx_data_sch.ps.max_burst_get(unit);

        for (i = 0; i < 8; i++)
        {
            access_field32_set(runtime_info, fields[i], sch_psst_value, max_value);
        }

        first_time = FALSE;
    }

    *entry_data = sch_psst_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set entry_data with the init value of SCH_CIR_SHAPERS_STATIC_TABEL_PSST memory
 */
shr_error_e
dnx_access_init_custom_sch_csst_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 sch_csst_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };
    int max_value;
    access_field_id_t fields[] = {
        mSCH_CIR_SHAPERS_STATIC_TABEL_CSST_fMAX_BURST_0,
        mSCH_CIR_SHAPERS_STATIC_TABEL_CSST_fMAX_BURST_1,
        mSCH_CIR_SHAPERS_STATIC_TABEL_CSST_fMAX_BURST_2,
        mSCH_CIR_SHAPERS_STATIC_TABEL_CSST_fMAX_BURST_3,
        mSCH_CIR_SHAPERS_STATIC_TABEL_CSST_fMAX_BURST_4,
        mSCH_CIR_SHAPERS_STATIC_TABEL_CSST_fMAX_BURST_5,
        mSCH_CIR_SHAPERS_STATIC_TABEL_CSST_fMAX_BURST_6,
        mSCH_CIR_SHAPERS_STATIC_TABEL_CSST_fMAX_BURST_7
    };
    int i;

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        max_value = dnx_data_sch.ps.max_burst_get(unit);

        for (i = 0; i < 8; i++)
        {
            access_field32_set(runtime_info, fields[i], sch_csst_value, max_value);
        }

        first_time = FALSE;
    }

    *entry_data = sch_csst_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set entry_data with the init value of CGM_IPP_MAP memory
 */
shr_error_e
dnx_access_init_custom_cgm_ipp_map_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 cgm_ipp_map_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field32_set(runtime_info, mCGM_IPP_MAP_fNIF_PORT, cgm_ipp_map_value,
                           dnx_data_ingr_congestion.vsq.vsq_e_default_get(unit));
        access_field32_set(runtime_info, mCGM_IPP_MAP_fPG_BASE, cgm_ipp_map_value,
                           dnx_data_ingr_congestion.vsq.vsq_f_default_get(unit));
        first_time = FALSE;
    }

    *entry_data = cgm_ipp_map_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set entry_data with the init value of IPPC_FEM_BIT_SELECTm memory
 */
shr_error_e
dnx_access_init_custom_ippc_fem_bit_select_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 ippc_fem_bit_select_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };
    dnx_field_fem_condition_ms_bit_t bit_select_init_val;

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        bit_select_init_val =
            dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->fem_condition_ms_bit_min_value;

        access_field32_set(runtime_info, mIPPC_FEM_BIT_SELECT_fBIT_SELECT, ippc_fem_bit_select_value,
                           bit_select_init_val);

        first_time = FALSE;
    }

    *entry_data = ippc_fem_bit_select_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set entry_data with the init value of TCAM_TCAM_BANK_COMMANDm memory
 */
shr_error_e
dnx_access_init_custom_tcam_tcam_bank_command_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    dnx_sw_init_mem_info * mem_info,
    unsigned array_index,
    int index,
    uint32 *entry_data)
{
    int unit = runtime_info->unit;

    uint32 data[BITS2WORDS(DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_DOUBLE) + 1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set WR CMD only for valid entries that can be written to (even entries within bank's limit)
     */
    if (array_index % 2 == 0 && DNX_FIELD_TCAM_LOCATION_IS_VALID(array_index))
    {
        access_field32_set(runtime_info, mTCAM_TCAM_BANK_COMMAND_fTCAM_CPU_CMD_WR, entry_data, 0x3);
        access_field32_set(runtime_info, mTCAM_TCAM_BANK_COMMAND_fTCAM_CPU_CMD_RD, entry_data, 0x0);
        access_field32_set(runtime_info, mTCAM_TCAM_BANK_COMMAND_fTCAM_CPU_CMD_CMP, entry_data, 0x0);
        access_field32_set(runtime_info, mTCAM_TCAM_BANK_COMMAND_fTCAM_CPU_CMD_VALID, entry_data, 0x0);
        access_field_set(unit, mTCAM_TCAM_BANK_COMMAND_fTCAM_CPU_CMD_DI, entry_data, data);
    }
    else
    {
        access_field32_set(runtime_info, mTCAM_TCAM_BANK_COMMAND_fTCAM_CPU_CMD_WR, entry_data, 0x0);
        access_field32_set(runtime_info, mTCAM_TCAM_BANK_COMMAND_fTCAM_CPU_CMD_RD, entry_data, 0x0);
        access_field32_set(runtime_info, mTCAM_TCAM_BANK_COMMAND_fTCAM_CPU_CMD_CMP, entry_data, 0x0);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - set MTU of MACSEC_ESEC_SC_TABLE to 64
 */
shr_error_e
dnx_access_init_custom_xflow_macsec_esec_sc_entry_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 xflow_macsec_esec_sc_entry_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };
    uint32 data = 64;

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mMACSEC_ESEC_SC_TABLE_fMTU, xflow_macsec_esec_sc_entry_value, &data);
        first_time = FALSE;
    }

    *entry_data = xflow_macsec_esec_sc_entry_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set MTU of MACSEC_SUB_PORT_POLICY_TABLE to 300
 */
shr_error_e
dnx_access_init_custom_xflow_macsec_isec_policy_entry_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 xflow_macsec_isec_policy_entry_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };
    uint32 data = 300;

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mMACSEC_SUB_PORT_POLICY_TABLE_fMTU, xflow_macsec_isec_policy_entry_value, &data);
        first_time = FALSE;
    }

    *entry_data = xflow_macsec_isec_policy_entry_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - Set IPPB_PINFO_FLP_MEM ACTION_PROFILE_SA_NOT_FOUND_INDEX to 1
 */
shr_error_e
dnx_access_init_custom_sa_not_found_action_profile_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 sa_not_found_action_profile_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field32_set(runtime_info, mIPPB_PINFO_FLP_MEM_fACTION_PROFILE_SA_NOT_FOUND_INDEX,
                           sa_not_found_action_profile_value,
                           DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND1 -
                           DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_SA_NOT_FOUND0);
        first_time = FALSE;
    }

    *entry_data = sa_not_found_action_profile_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - return EPS_CH_IFC_CFG default value per array_index
 */
shr_error_e
dnx_access_init_custom_eps_ch_ifc_cfg_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    dnx_sw_init_mem_info * mem_info,
    unsigned array_index,
    int index,
    uint32 *entry_data)
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * * for the device type */
    const access_device_field_t *field_info = device_info->local_fields + mEPS_CH_IFC_CFG_fMAP_IFC_TO_CH_ID;
    int field_length = field_info->size_in_bits;

    SHR_FUNC_INIT_VARS(unit);

    access_field32_set(runtime_info, mEPS_CH_IFC_CFG_fIFC_IS_CHANNELIZED, entry_data, 0);

    /** mask array_index upto MAP_IFC_TO_CH_IDf size */
    access_field32_set(runtime_info, mEPS_CH_IFC_CFG_fMAP_IFC_TO_CH_ID, entry_data,
                       array_index & SAL_UPTO_BIT(field_length));

    SHR_FUNC_EXIT;
}

/**
 * \brief - Call Back function for mem_init mechanism, will init the context MRU to simulation values
 */
shr_error_e
dnx_access_init_spb_context_mru_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 spb_context_mru_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field32_set(runtime_info, mSPB_CONTEXT_MRU_fMAX_SIZE, spb_context_mru_value, 0x2fff);
        access_field32_set(runtime_info, mSPB_CONTEXT_MRU_fMAX_ORG_SIZE, spb_context_mru_value,
                           dnx_data_ingr_congestion.info.max_jumbo_packet_size_get(unit));
        access_field32_set(runtime_info, mSPB_CONTEXT_MRU_fMIN_SIZE, spb_context_mru_value, 0x20);
        access_field32_set(runtime_info, mSPB_CONTEXT_MRU_fMIN_ORG_SIZE, spb_context_mru_value, 0x20);
        first_time = FALSE;
    }

    *entry_data = spb_context_mru_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - Call Back function for mem_init mechanism, will init the memory to all ones value
 */

shr_error_e
dnx_access_init_custom_fill_all_ones_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint32 all_ones[SAND_MAX_U32S_IN_MEM_ENTRY] = {
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF
    };

    SHR_FUNC_INIT_VARS(unit);

    *entry_data = all_ones;

    SHR_FUNC_EXIT;
}

static const uint32 all_ones_entry[SAND_MAX_U32S_IN_MEM_ENTRY] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};
/**
 *
 * \brief - set entry_data with the init value of mIPPE_PINFO_LLR memory
 *
 */
shr_error_e
dnx_access_init_custom_ippe_pinfo_llr_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 ippe_pinfo_llr_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mIPPE_PINFO_LLR_fRECYCLE_PROFILE_ENABLE, ippe_pinfo_llr_value, all_ones_entry);
        first_time = FALSE;
    }

    *entry_data = ippe_pinfo_llr_value;

    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - set entry_data with the init value of mIPPC_FES_2ND_INSTRUCTION memory
 *
 */
shr_error_e
dnx_access_init_custom_ippc_fes_2nd_instruction_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 ippc_fes_2nd_instruction_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mIPPC_FES_2ND_INSTRUCTION_fACTION_TYPE, ippc_fes_2nd_instruction_value, all_ones_entry);
        first_time = FALSE;
    }

    *entry_data = ippc_fes_2nd_instruction_value;

    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - set entry_data with the init value of mIPPD_FES_2ND_INSTRUCTION memory
 *
 */
shr_error_e
dnx_access_init_custom_ippd_fes_2nd_instruction_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 ippd_fes_2nd_instruction_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mIPPD_FES_2ND_INSTRUCTION_fACTION_TYPE, ippd_fes_2nd_instruction_value, all_ones_entry);
        first_time = FALSE;
    }

    *entry_data = ippd_fes_2nd_instruction_value;

    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - set entry_data with the init value of mERPP_FES_2ND_INSTRUCTION memory
 *
 */
shr_error_e
dnx_access_init_custom_erpp_fes_2nd_instruction_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 erpp_fes_2nd_instruction_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mERPP_FES_2ND_INSTRUCTION_fACTION_TYPE, erpp_fes_2nd_instruction_value, all_ones_entry);
        first_time = FALSE;
    }

    *entry_data = erpp_fes_2nd_instruction_value;

    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - set entry_data with the init value of mETPPA_FES_2ND_INSTRUCTION memory
 *
 */
shr_error_e
dnx_access_init_custom_etppa_fes_2nd_instruction_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 etppa_fes_2nd_instruction_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mETPPA_FES_2ND_INSTRUCTION_fACTION_TYPE, etppa_fes_2nd_instruction_value,
                         all_ones_entry);
        first_time = FALSE;
    }

    *entry_data = etppa_fes_2nd_instruction_value;

    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - set entry_data with the init value of mETPPB_MTU_MAP_TABLE memory
 *
 */
shr_error_e
dnx_access_init_custom_etppb_mtu_map_table_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 etppb_mtu_map_table_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mETPPB_MTU_MAP_TABLE_fMTU, etppb_mtu_map_table_value, all_ones_entry);
        first_time = FALSE;
    }

    *entry_data = etppb_mtu_map_table_value;

    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - set entry_data with the init value of mETPPC_MTU_MAP_TABLE memory
 *
 */
shr_error_e
dnx_access_init_custom_etppc_mtu_map_table_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 etppc_mtu_map_table_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mETPPC_MTU_MAP_TABLE_fMTU, etppc_mtu_map_table_value, all_ones_entry);
        first_time = FALSE;
    }

    *entry_data = etppc_mtu_map_table_value;

    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - set entry_data with the init value of mSCH_SLOW_SCALE_A_SSA memory
 *
 */
shr_error_e
dnx_access_init_custom_sch_slow_scale_a_ssa_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 sch_slow_scale_a_ssa_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mSCH_SLOW_SCALE_A_SSA_fMAX_BUCKET, sch_slow_scale_a_ssa_value, all_ones_entry);
        first_time = FALSE;
    }

    *entry_data = sch_slow_scale_a_ssa_value;

    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - set entry_data with the init value of mSCH_SLOW_SCALE_B_SSB memory
 *
 */
shr_error_e
dnx_access_init_custom_sch_slow_scale_b_ssb_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 sch_slow_scale_b_ssb_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mSCH_SLOW_SCALE_B_SSB_fMAX_BUCKET, sch_slow_scale_b_ssb_value, all_ones_entry);
        first_time = FALSE;
    }

    *entry_data = sch_slow_scale_b_ssb_value;

    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - set entry_data with the init value of mIPS_QSPM memory
 *
 */
shr_error_e
dnx_access_init_custom_ips_qspm_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 ips_qspm_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mIPS_QSPM_fSYSTEM_PORT, ips_qspm_value, all_ones_entry);
        first_time = FALSE;
    }

    *entry_data = ips_qspm_value;

    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - set entry_data with the init value of mCGM_QSPM memory
 *
 */
shr_error_e
dnx_access_init_custom_cgm_qspm_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 cgm_qspm_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mCGM_QSPM_fSYSTEM_PORT, cgm_qspm_value, all_ones_entry);
        first_time = FALSE;
    }

    *entry_data = cgm_qspm_value;

    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - set entry_data with the init value of mIPS_QPRIORITY memory
 *
 */
shr_error_e
dnx_access_init_custom_ips_qpriority_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 ips_qpriority_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mIPS_QPRIORITY_fQPRIORITY_DATA, ips_qpriority_value, all_ones_entry);
        first_time = FALSE;
    }

    *entry_data = ips_qpriority_value;

    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - set entry_data with the init value of mEPNI_ALIGNER_MIRR_QP_TO_CHANNEL_MAP memory
 *
 */
shr_error_e
dnx_access_init_custom_epni_aligner_mirr_qp_to_channel_map_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 dnx_access_init_custom_epni_aligner_mirr_qp_to_channel_map_value_get[SAND_MAX_U32S_IN_MEM_ENTRY] =
        { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mEPNI_ALIGNER_MIRR_QP_TO_CHANNEL_MAP_fCHANNEL,
                         dnx_access_init_custom_epni_aligner_mirr_qp_to_channel_map_value_get, all_ones_entry);
        first_time = FALSE;
    }

    *entry_data = dnx_access_init_custom_epni_aligner_mirr_qp_to_channel_map_value_get;

    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - set entry_data with the init value of mERPP_CFG_ENABLE_FILTER_PER_PORT_TABLE memory
 *
 */
shr_error_e
dnx_access_init_custom_erpp_cfg_enable_filter_per_port_table_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint8 first_time = TRUE;
    static uint32 erpp_cfg_enable_filter_per_port_table_value[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (first_time)
    {
        access_field_set(unit, mERPP_CFG_ENABLE_FILTER_PER_PORT_TABLE_fCFG_SAME_INTERFACE_PER_PORT,
                         erpp_cfg_enable_filter_per_port_table_value, all_ones_entry);
        first_time = FALSE;
    }

    *entry_data = erpp_cfg_enable_filter_per_port_table_value;

    SHR_FUNC_EXIT;
}

/**
 * \brief - Call Back function for mem_init mechanism, will init the memory to all zeros value
 */
shr_error_e
dnx_access_init_custom_fill_all_zeros_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    uint32 **entry_data)
{
    int unit = runtime_info->unit;
    static uint32 all_zeros[SAND_MAX_U32S_IN_MEM_ENTRY] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    *entry_data = all_zeros;

    SHR_FUNC_EXIT;
}

/**
 * \brief - Call Back function for mem_init mechanism, will init the memory to index value
 */
shr_error_e
dnx_access_init_index_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    dnx_sw_init_mem_info * mem_info,
    unsigned array_index,
    int index,
    uint32 *entry_data)
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * * for the device type */
    uint32 field_val;
    uint32 field_mask;

    SHR_FUNC_INIT_VARS(unit);

    if (mem_info->field_id == ACCESS_INVALID_LOCAL_FIELD_ID)
    {
        *entry_data = index;
    }
    else
    {
        field_mask = (((uint32) 1) << device_info->local_fields[mem_info->field_id].size_in_bits) - 1;
        field_val = index & field_mask;
        access_local_field_set(runtime_info, mem_info->field_id, entry_data, &field_val);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Call Back function for mem_init mechanism, will init the memory to index seq array value
 */
shr_error_e
dnx_access_init_index_seq_array_value_get(
    access_runtime_info_t * runtime_info,       /* Access runtime info of the device */
    dnx_sw_init_mem_info * mem_info,
    unsigned array_index,
    int index,
    uint32 *entry_data)
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    const access_device_regmem_t *rm_info;
    int index_max;
    uint32 field[SOC_MAX_MEM_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    rm_info = device_info->local_regs + mem_info->mem;
    index_max = rm_info->u.mem.mem_nof_elements - 1;
    if (mem_info->field_id == ACCESS_INVALID_LOCAL_FIELD_ID)
    {
        *entry_data = index + ((index_max + 1) * array_index);
    }
    else
    {
        *field = index + ((index_max + 1) * array_index);
        SHR_IF_ERR_EXIT(access_local_field_set(runtime_info, mem_info->field_id, entry_data, field));
    }

exit:
    SHR_FUNC_EXIT;
}
#endif /* BCM_ACCESS_SUPPORT */

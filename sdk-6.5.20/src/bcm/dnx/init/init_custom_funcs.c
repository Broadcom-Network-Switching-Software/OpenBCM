
/** \file init_custom_funcs.c
 * Custom functions to be used to initialize DNX memories. 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MEM

#include <soc/mem.h> 
#include <shared/shrextend/shrextend_error.h>
#include <soc/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <bcm_int/dnx/field/field_map.h>

/*
 * The following functions are used to initialize memories of 
 * Jericho2
 *
 * For other devices,
 * a different version of the following functions might be required
 */

/** See .h file */
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

/** See .h file */
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

/** See .h file */
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

    uint32 shaper_mantissa_max_value = 0x3f; /** 6 bits */
    uint32 shaper_max_burst_value = 0x1ff; /** 9 bits */

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

/** See .h file */
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

/** See .h file */
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

/** See .h file */
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

/** See .h file */
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

/** See .h file */
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

/** See .h file */
shr_error_e
dnx_init_custom_ippc_fes_2nd_instruction_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    dnx_field_action_type_t action_type;
    uint32 hw_value;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, DNX_FIELD_STAGE_IPMF1, &action_type));
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, dnx_field_map_stage_info[DNX_FIELD_STAGE_IPMF1].action_field_id, action_type, &hw_value));

    soc_mem_field32_set(unit, IPPC_FES_2ND_INSTRUCTIONm, entry_data, ACTION_TYPEf, hw_value);

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_ippd_fes_2nd_instruction_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    dnx_field_action_type_t action_type;
    uint32 hw_value;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, DNX_FIELD_STAGE_IPMF3, &action_type));
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, dnx_field_map_stage_info[DNX_FIELD_STAGE_IPMF3].action_field_id, action_type, &hw_value));

    soc_mem_field32_set(unit, IPPD_FES_2ND_INSTRUCTIONm, entry_data, ACTION_TYPEf, hw_value);

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_erpp_fes_2nd_instruction_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    dnx_field_action_type_t action_type;
    uint32 hw_value;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, DNX_FIELD_STAGE_EPMF, &action_type));
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, dnx_field_map_stage_info[DNX_FIELD_STAGE_EPMF].action_field_id, action_type, &hw_value));

    soc_mem_field32_set(unit, ERPP_FES_2ND_INSTRUCTIONm, entry_data, ACTION_TYPEf, hw_value);

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
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
    dnx_field_action_type_t action_type;
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

    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, DNX_FIELD_STAGE_ACE, &action_type));
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, dnx_field_map_stage_info[DNX_FIELD_STAGE_ACE].action_field_id, action_type, &hw_value));

    for (action_type_field_index = 0; action_type_field_index < nof_action_type_fields; action_type_field_index++)
    {
        soc_mem_field32_set(unit, ETPPA_PRP_FES_PROGRAM_TABLEm, entry_data, action_type_fields[action_type_field_index],
                            hw_value);
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_sch_psst_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    int field_length = soc_mem_field_length(unit, SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm, MAX_BURST_0f);
    int max_value;
    soc_field_t fields[] = { MAX_BURST_0f, MAX_BURST_1f, MAX_BURST_2f, MAX_BURST_3f,
        MAX_BURST_4f, MAX_BURST_5f, MAX_BURST_6f, MAX_BURST_7f
    };
    int i;

    SHR_FUNC_INIT_VARS(unit);

    max_value = (1 << field_length) - 1;

    for (i = 0; i < 8; i++)
    {
        soc_mem_field32_set(unit, SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm, entry_data, fields[i], max_value);
    }

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_sch_csst_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    int field_length = soc_mem_field_length(unit, SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm, MAX_BURST_0f);
    int max_value;
    soc_field_t fields[] = { MAX_BURST_0f, MAX_BURST_1f, MAX_BURST_2f, MAX_BURST_3f,
        MAX_BURST_4f, MAX_BURST_5f, MAX_BURST_6f, MAX_BURST_7f
    };
    int i;

    SHR_FUNC_INIT_VARS(unit);

    max_value = (1 << field_length) - 1;

    for (i = 0; i < 8; i++)
    {
        soc_mem_field32_set(unit, SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm, entry_data, fields[i], max_value);
    }

    SHR_FUNC_EXIT;
}

/** See .h file */
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

/** See .h file */
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

/** See .h file */
shr_error_e
dnx_init_custom_ippc_fem_map_index_table_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    dnx_field_fem_action_valid_t valid_init_val;

    SHR_FUNC_INIT_VARS(unit);

    valid_init_val = (dnx_field_fem_action_valid_t) DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE;

    soc_mem_field32_set(unit, IPPC_FEM_MAP_INDEX_TABLEm, entry_data, VALIDf, valid_init_val);

    SHR_FUNC_EXIT;
}

/** See .h file */
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

    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, entry_data, TCAM_CPU_CMD_WRf, 0x3);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, entry_data, TCAM_CPU_CMD_RDf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, entry_data, TCAM_CPU_CMD_CMPf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, entry_data, TCAM_CPU_CMD_VALIDf, 0x0);
    soc_mem_field_set(unit, TCAM_TCAM_BANK_COMMANDm, entry_data, TCAM_CPU_CMD_DIf, data);

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_tcam_tcam_action_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    uint32 data[BITS2WORDS(DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_DOUBLE) + 1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field_set(unit, TCAM_TCAM_ACTIONm, entry_data, ACTIONf, data);

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_init_custom_tcam_tcam_action_small_default_get(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    uint32 data[BITS2WORDS(DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_DOUBLE) + 1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field_set(unit, TCAM_TCAM_ACTION_SMALLm, entry_data, ACTIONf, data);

    SHR_FUNC_EXIT;
}

/** See .h file */
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

/** See .h file */
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

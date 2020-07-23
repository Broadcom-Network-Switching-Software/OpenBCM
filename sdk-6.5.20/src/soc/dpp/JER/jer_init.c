/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

 

#include <shared/bsl.h>


#include <sal/appl/sal.h>


#include <soc/debug.h>
#include <soc/error.h>
#include <soc/iproc.h>
#include <soc/mem.h>


#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/sand/sand_mem.h>


#include <soc/dpp/JER/jer_drv.h>
#include <soc/dpp/JER/jer_defs.h>
#include <soc/dpp/JER/jer_init.h>
#include <soc/dpp/JER/jer_ocb_dram_buffers.h>
#include <soc/dpp/JER/jer_mgmt.h>
#include <soc/dpp/JER/jer_reset.h>
#include <soc/dpp/JER/jer_fabric.h>
#include <soc/dpp/JER/jer_trunk.h>
#include <soc/dpp/JER/jer_egr_queuing.h>
#include <soc/dpp/JER/jer_tdm.h>
#include <soc/dpp/JER/jer_ingress_packet_queuing.h>
#include <soc/dpp/JER/jer_ingress_traffic_mgmt.h>

#include <soc/dpp/JER/JER_PP/jer_pp_ing_protection.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_protection.h>
#include <soc/dpp/JER/JER_PP/jer_pp_lif.h>
#include <soc/dpp/JER/JER_PP/jer_pp_oam.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap.h>

#include <soc/dpp/JER/jer_regs.h>
#include <soc/dpp/JER/jer_tbls.h>
#include <soc/dpp/JER/jer_ofp_rates.h>
#include <soc/dpp/JER/jer_sch.h>
#include <soc/dpp/JER/jer_stat_if.h>
#include <soc/dpp/JER/jer_dram.h>
#include <soc/dpp/JER/jer_ports.h>

#include <soc/dpp/JER/JER_PP/jer_pp_lbp_init.h>

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#endif 

#include <soc/dpp/JER/JER_PP/jer_pp_mymac.h>

 
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_dram.h>
#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#if defined(INCLUDE_KBP)
#include <soc/dpp/ARAD/arad_kbp.h>
#endif
#include <soc/dpp/ARAD/arad_cnt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/JER/jer_flow_control.h>
#include <soc/dpp/multicast_imp.h>

#include <soc/dpp/QAX/qax_init.h>
#include <soc/dpp/QAX/qax_tbls.h>
#include <soc/dpp/QAX/qax_link_bonding.h>
#include <soc/dpp/QAX/qax_stat_if.h>

#include <soc/dpp/port_sw_db.h>

#ifdef PLISIM
    #include <soc/dpp/ARAD/arad_sim_em.h>
#endif


#include <soc/sand/sand_aux_access.h>


static soc_reg_t jer_blocks_with_standart_bubble_mechanism[] = {
    IRE_INDIRECT_COMMANDr,        IRE_INDIRECT_FORCE_BUBBLEr,
    IDR_INDIRECT_COMMANDr,        IDR_INDIRECT_FORCE_BUBBLEr,
    MRPS_INDIRECT_COMMANDr,       MRPS_INDIRECT_FORCE_BUBBLEr,
    IRR_INDIRECT_COMMANDr,        IRR_INDIRECT_FORCE_BUBBLEr,
    OCB_INDIRECT_COMMANDr,        OCB_INDIRECT_FORCE_BUBBLEr,
    IQM_INDIRECT_COMMANDr,        IQM_INDIRECT_FORCE_BUBBLEr,
    IQMT_INDIRECT_COMMANDr,       IQMT_INDIRECT_FORCE_BUBBLEr,
    CRPS_INDIRECT_COMMANDr,       CRPS_INDIRECT_FORCE_BUBBLEr,
    IPS_INDIRECT_COMMANDr,        IPS_INDIRECT_FORCE_BUBBLEr,
    IPST_INDIRECT_COMMANDr,       IPST_INDIRECT_FORCE_BUBBLEr,
    IPT_INDIRECT_COMMANDr,        IPT_INDIRECT_FORCE_BUBBLEr,
    RTP_INDIRECT_COMMANDr,        RTP_INDIRECT_FORCE_BUBBLEr,
    EGQ_INDIRECT_COMMANDr,        EGQ_INDIRECT_FORCE_BUBBLEr,
    EPNI_INDIRECT_COMMANDr,       EPNI_INDIRECT_FORCE_BUBBLEr,
    OLP_INDIRECT_COMMANDr,        OLP_INDIRECT_FORCE_BUBBLEr,
    IHP_INDIRECT_COMMANDr,        IHP_INDIRECT_FORCE_BUBBLEr,
    IHB_INDIRECT_COMMANDr,        IHB_INDIRECT_FORCE_BUBBLEr,

    
    INVALIDr
};


#define JER_REF_CLOCK_VALUE 25




int soc_jer_init_prepare_internal_data(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_IS_QAX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_qax_dram_buffer_conf_calc(unit));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_jer_ocb_dram_buffer_conf_calc(unit)); 
    }

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC void soc_jer_init_exclude_blocks(int unit)
{
    SOC_INFO(unit).block_valid[CLP_BLOCK(unit, 3)] = FALSE;
    SOC_INFO(unit).block_valid[CLP_BLOCK(unit, 4)] = FALSE;
    SOC_INFO(unit).block_valid[CLP_BLOCK(unit, 5)] = FALSE;
    SOC_INFO(unit).block_valid[NBIL_BLOCK(unit, 0)] = FALSE;
    SOC_INFO(unit).block_valid[NBIL_BLOCK(unit, 1)] = FALSE;
    SOC_INFO(unit).block_valid[MMU_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCA_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCB_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCC_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCD_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCE_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCF_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCG_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCH_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCBROADCAST_BLOCK(unit)] = FALSE;
}

int soc_jer_init_blocks_init_global_conf(int unit)
{
    uint32 reg32_val, field_val;
    ARAD_MGMT_INIT* init;

    SOCDNX_INIT_FUNC_DEFS;

    init = &SOC_DPP_CONFIG(unit)->arad->init;

    
    if (SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) {
        SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_SYS_HEADER_CFGr(unit, &reg32_val));
        soc_reg_field_set(unit, ECI_GLOBAL_SYS_HEADER_CFGr, &reg32_val, SYSTEM_HEADERS_MODEf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_SYS_HEADER_CFGr(unit, reg32_val));
    }
    else if (soc_property_get(unit, spn_SYSTEM_IS_ARAD_IN_SYSTEM, 0)) {
        SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_SYS_HEADER_CFGr(unit, &reg32_val));
        soc_reg_field_set(unit, ECI_GLOBAL_SYS_HEADER_CFGr, &reg32_val, SYSTEM_HEADERS_MODEf, 2);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_SYS_HEADER_CFGr(unit, reg32_val));
    }

    
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_SYS_HEADER_CFGr(unit, &reg32_val));
    field_val = init->fabric.ftmh_lb_ext_mode == ARAD_MGMT_FTMH_LB_EXT_MODE_DISABLED ? 0 : 1;
    soc_reg_field_set(unit, ECI_GLOBAL_SYS_HEADER_CFGr, &reg32_val, FTMH_LB_KEY_EXT_ENf, field_val);
    field_val = init->fabric.ftmh_stacking_ext_mode == 0 ? 0 : 1;
    soc_reg_field_set(unit, ECI_GLOBAL_SYS_HEADER_CFGr, &reg32_val, FTMH_STACKING_EXT_ENABLEf, field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_SYS_HEADER_CFGr(unit, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg32_val));
    if (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores == 1) {
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, TURBO_PIPEf, 1);
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, DUAL_FAP_MODEf, 0);
    } else {
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, TURBO_PIPEf, 0);
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, DUAL_FAP_MODEf, 1);
    }
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FMC_ENf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg32_val));

    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_1r(unit, &reg32_val));
      
    if (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_MESH || 
        
        SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP )
    {
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32_val, MESH_MODEf, 1);
    }
    else
    {
        
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32_val, MESH_MODEf, 0);
    }


    if(SOC_DPP_CONFIG(unit)->arad->init.fabric.is_128_in_system) {
        field_val = 0x1;
    } else if (SOC_DPP_CONFIG(unit)->arad->init.fabric.system_contains_multiple_pipe_device) {
        field_val = 0x2;
    } else {
        field_val = 0x0;
    }   
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32_val, RESERVED_QTSf, field_val);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32_val, TDM_ATTRIBUTEf, 0x1);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_1r(unit, reg32_val));
    
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_3r(unit, &reg32_val));
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_3r, &reg32_val, PACKET_CRC_ENf, 1);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_3r, &reg32_val, OPPORTUNISTIC_CRC_ENf, 1);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_3r, &reg32_val, TOD_MODEf, 3);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_3r(unit, reg32_val));
    
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_4r(unit, &reg32_val));
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_4r, &reg32_val, IPS_DEQ_CMD_RESf, 1);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_4r, &reg32_val, SINGLE_STAT_PORTf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_4r(unit, reg32_val));

    
    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_ECI_PAD_CONFIGURATION_REGISTERr(unit, &reg32_val));
        if (init->ex_vol_mod == ARAD_MGMT_EXT_VOL_MOD_HSTL_1p8V) {
            soc_reg_field_set(unit, ECI_PAD_CONFIGURATION_REGISTERr, &reg32_val, MODE_HV_2f, 0);
        } else if (init->ex_vol_mod == ARAD_MGMT_EXT_VOL_MOD_3p3V) {
            soc_reg_field_set(unit, ECI_PAD_CONFIGURATION_REGISTERr, &reg32_val, MODE_HV_2f, 1);
        } 
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_PAD_CONFIGURATION_REGISTERr(unit, reg32_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_init_blocks_init_general_conf(int unit)
{
    uint32 reg32_val, preference;
    soc_reg_above_64_val_t reg_above_64_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_ECI_GP_CONTROL_9r(unit, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_TXI_CREDITS_INIT_VALUEf, 0X0);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_RSTNf, 0X1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_OAMP_STRICT_PRIORITYf, 0X0);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_EGQ_1_ENABLEf, 0X1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_OAMP_ENABLEf, 0X1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_EGQ_0_RXI_RESET_Nf, 0X1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_EGQ_0_ENABLEf, 0X1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PMH_SYNCE_RSTNf, 0X0);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_TXI_CREDITS_INITf, 0X0);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_OAMP_RXI_RESET_Nf, 0X1);
    soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PIR_EGQ_1_RXI_RESET_Nf, 0X1);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GP_CONTROL_9r(unit, reg_above_64_val));

    
    SOCDNX_IF_ERR_EXIT(soc_jer_reset_nif_txi_oor(unit));

    
    reg32_val = 0;
    soc_reg_field_set(unit, IQMT_BDBLL_BANK_SIZESr, &reg32_val, BDB_LIST_SIZE_2f, 0X8);
    soc_reg_field_set(unit, IQMT_BDBLL_BANK_SIZESr, &reg32_val, BDB_LIST_SIZE_3f, 0X8);
    soc_reg_field_set(unit, IQMT_BDBLL_BANK_SIZESr, &reg32_val, BDB_LIST_SIZE_0f, 0X8);
    soc_reg_field_set(unit, IQMT_BDBLL_BANK_SIZESr, &reg32_val, BDB_LIST_SIZE_5f, 0X8);
    soc_reg_field_set(unit, IQMT_BDBLL_BANK_SIZESr, &reg32_val, BDB_LIST_SIZE_4f, 0X8);
    soc_reg_field_set(unit, IQMT_BDBLL_BANK_SIZESr, &reg32_val, BDB_LIST_SIZE_1f, 0X8);
    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_BDBLL_BANK_SIZESr(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IQMT_IQM_BDB_OFFSETr, &reg32_val, IQM_1_BDB_OFFSETf, 0X0);
    soc_reg_field_set(unit, IQMT_IQM_BDB_OFFSETr, &reg32_val, IQM_0_BDB_OFFSETf, 0X0);
    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_IQM_BDB_OFFSETr(unit, reg32_val));

    reg32_val = 0;
    SOCDNX_IF_ERR_EXIT(soc_dpp_prop_parse_admission_precedence_preference(unit, &preference));
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, FLMC_4K_REP_ENf, 0X1);
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, FLUSC_CNM_PROT_ENf, 0X0); 
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, VSQ_SIZE_MODEf, 0X1);
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, DSCRD_DPf, 0X4);
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, IGNORE_DPf, preference);
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, DEQ_CACHE_ENH_RR_ENf, 0X1);
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, CNGQ_ON_BUFF_ENf, 0X1);
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, VSQ_ISP_UPD_ENf, 0X1);
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, SET_TDM_Q_PER_QSIGNf, 0X1);
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, EN_IPT_CD_4_SNOOPf, 0X1);
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, FWD_ACT_SELf, 0X0); 
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, VSQ_TH_MODE_SELf, 0X1);
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, PDM_INIT_ENf, 0X1);
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, ISP_CD_SCND_CP_ENf, 0X1);
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, MNUSC_CNM_PROT_ENf, 0X0); 
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, CRPS_CMD_ENf, 0X0); 
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32_val, DSCRD_ALL_PKTf, 0x0);
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_IQM_ENABLERSr(unit, SOC_CORE_ALL, reg32_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_init_blocks_init_conf(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_jer_init_blocks_init_global_conf(unit));

    
    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_init_blocks_init_general_conf(unit));
    }


    
    SOCDNX_IF_ERR_EXIT(soc_jer_ocb_dram_buffer_conf_set(unit));
    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_ocb_conf_set(unit));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int soc_jer_led_tune(int unit)
{
    uint32 rval = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO_PLUS_A0(unit)) {
        soc_reg_field_set(unit, NBIL_PM_STRAP_LED_OUTPUT_DELAYr, &rval, PML_0_STRAP_LED_OUTPUT_DELAYf, 2);
        soc_reg_field_set(unit, NBIL_PM_STRAP_LED_OUTPUT_DELAYr, &rval, PML_1_STRAP_LED_OUTPUT_DELAYf, 2);
        soc_reg_field_set(unit, NBIL_PM_STRAP_LED_OUTPUT_DELAYr, &rval, PML_2_STRAP_LED_OUTPUT_DELAYf, 2);
        soc_reg_field_set(unit, NBIL_PM_STRAP_LED_OUTPUT_DELAYr, &rval, PML_3_STRAP_LED_OUTPUT_DELAYf, 2);
        soc_reg_field_set(unit, NBIL_PM_STRAP_LED_OUTPUT_DELAYr, &rval, PML_4_STRAP_LED_OUTPUT_DELAYf, 2);
        soc_reg_field_set(unit, NBIL_PM_STRAP_LED_OUTPUT_DELAYr, &rval, PML_5_STRAP_LED_OUTPUT_DELAYf, 2);

        
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PM_STRAP_LED_OUTPUT_DELAYr(unit, 1 | SOC_REG_ADDR_INSTANCE_MASK, rval));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_init_blocks_init(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "exclude_blocks_missing_in_partial_emul_compilation", 0)) {
        soc_jer_init_exclude_blocks(unit);
    }

    
    SOCDNX_IF_ERR_EXIT(soc_jer_regs_blocks_access_check(unit));

    
    SOCDNX_IF_ERR_EXIT(soc_jer_init_brdc_blk_id_set(unit));

    
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_set,(unit, 0))); 

   if (!SOC_IS_FLAIR(unit)) {
       
        SOCDNX_IF_ERR_EXIT(soc_jer_write_masks_set(unit));

        
        SOCDNX_IF_ERR_EXIT(soc_jer_led_tune(unit));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_init_bubble_config_standard_blocks (int unit)
{
    uint32 reg = 0;
    int iter = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bubble_cpu", 1) == 1) {
        
        while (jer_blocks_with_standart_bubble_mechanism[iter] != INVALIDr) {

            
            SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, jer_blocks_with_standart_bubble_mechanism[iter++], REG_PORT_ANY, INDIRECT_COMMAND_TIMEOUTf, 0x10));

            
            reg = 0;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, jer_blocks_with_standart_bubble_mechanism[iter], REG_PORT_ANY, 0, &reg));
            soc_reg_field_set (unit, jer_blocks_with_standart_bubble_mechanism[iter], &reg, FORCE_BUBBLE_PERIODf, 0x8);
            soc_reg_field_set (unit, jer_blocks_with_standart_bubble_mechanism[iter], &reg, FORCE_BUBBLE_ENf, 0x1);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, jer_blocks_with_standart_bubble_mechanism[iter++], REG_PORT_ANY, 0, reg));

        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_init_bubble_config_specific_egr (int unit)
{
    uint32 conf_reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    
    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bubble_egr", 0) == 1) {
        
        SOCDNX_IF_ERR_EXIT(READ_EGQ_FQP_BUBBLE_CONFIGURATIONr(unit, REG_PORT_ANY, &conf_reg_val));
        
        soc_reg_field_set(unit, EGQ_FQP_BUBBLE_CONFIGURATIONr, &conf_reg_val, FQP_CONST_BUBBLE_ENf, 1);
        
        soc_reg_field_set(unit, EGQ_FQP_BUBBLE_CONFIGURATIONr, &conf_reg_val, FQP_BUBBLE_DELAYf, 31);
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_FQP_BUBBLE_CONFIGURATIONr(unit, SOC_CORE_ALL, conf_reg_val));
    }
exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_init_bubble_config_specific_ing (int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bubble_ing", 1) == 1) {
        
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IHP_IHP_ENABLERSr, SOC_CORE_ALL, 0, FORCE_BUBBLESf,  1));
        
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IHP_SYNC_COUNTERr, SOC_CORE_ALL, 0, SYNC_COUNTERf,  0x3ff));
    }
exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_init_bubble_config (int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_jer_init_bubble_config_standard_blocks(unit));

    
    SOCDNX_IF_ERR_EXIT(soc_jer_init_bubble_config_specific_egr(unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_init_bubble_config_specific_ing(unit));
    
exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_write_masks_set (int unit)
{
    uint32
     wr_mask_val;
    SOCDNX_INIT_FUNC_DEFS;

    wr_mask_val = 0xffffffff;
    SOCDNX_IF_ERR_EXIT(WRITE_PPDB_A_INDIRECT_WR_MASKr(unit, wr_mask_val)); 

    SOCDNX_IF_ERR_EXIT(WRITE_PPDB_B_INDIRECT_WR_MASKr(unit, wr_mask_val)); 

    SOCDNX_IF_ERR_EXIT(WRITE_IHB_INDIRECT_WR_MASKr(unit, SOC_CORE_ALL,wr_mask_val)); 

    SOCDNX_IF_ERR_EXIT(WRITE_IHP_INDIRECT_WR_MASKr(unit,SOC_CORE_ALL, wr_mask_val)); 

    SOCDNX_IF_ERR_EXIT(WRITE_SCH_INDIRECT_WR_MASKr_REG32(unit,SOC_CORE_ALL, wr_mask_val));     

    if (SOC_IS_QUX(unit)) {
        soc_reg_above_64_val_t reg_data;
        soc_reg_t indirect_wr_mask_reg[] = {
                                         EDB_INDIRECT_WR_MASKr,
                                         EPNI_INDIRECT_WR_MASKr, 
                                         CGM_INDIRECT_WR_MASKr,
                                         KAPS_INDIRECT_WR_MASKr,
                                         OAMP_INDIRECT_WR_MASKr
                                      };
        uint32   i;

        reg_data[0] = 0xffffffff;
        reg_data[1] = 0xffffffff;
        reg_data[2] = 0xffffffff;

        for(i = 0; i < sizeof(indirect_wr_mask_reg)/sizeof(soc_reg_t); i++) {
           SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, indirect_wr_mask_reg[i], REG_PORT_ANY,0,reg_data));
        }

    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_init_blocks_general(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_FLAIR(unit)) {
        
        SOCDNX_IF_ERR_EXIT(soc_jer_ocb_dram_buffer_autogen_set(unit));

        
        SOCDNX_IF_ERR_EXIT(soc_jer_init_bubble_config(unit));

        
        SOCDNX_IF_ERR_EXIT(jer_mgmt_revision_fixes(unit));
    }

    
    SOCDNX_IF_ERR_EXIT(jer_mgmt_drv_pvt_monitor_enable(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(INCLUDE_KBP)
uint32 jer_kbp_init_jer_interface(int unit)
{
    uint32 offset;
    soc_port_t ilkn_port;
    uint32 reg_val, ilkn_num_lanes, ilkn_metaframe, copy_no;
    int ilkn_rate, core;
    soc_reg_t reg, rst_reg;
    soc_field_t tx_field;
    uint32 one_kbp_ilkn_at_least = 0;
    uint32 res = SOC_SAND_OK, rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_DPP_CORES_ITER(_SHR_CORE_ALL, core) {
        res = arad_kbp_ilkn_interface_param_get(unit, core, &ilkn_port, &ilkn_num_lanes, &ilkn_rate, &ilkn_metaframe);
        soc_sand_update_error_code(res, &rv);
        if (rv == SOC_E_NONE) {
            one_kbp_ilkn_at_least = 1;
        } else {
            continue;
        }
              
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, ilkn_port, 0, &offset));
        if (SOC_IS_QAX(unit)) {
            switch (offset) {
                case 0:
                    reg = NBIH_ELK_CFGr;
                    rst_reg = NBIH_ADDITIONAL_RESETSr;
                    copy_no = 0;
                    tx_field = ELK_ON_ILKN_PORT_0_ELK_Nf;
                    break;
                case 1:
                    reg = NBIL_ELK_CFGr;
                    rst_reg = NBIL_ADDITIONAL_RESETSr;
                    copy_no = 0;
                    tx_field = ELK_ON_ILKN_ELK_Nf;
                    break;
                case 2:
                    reg = NBIH_ELK_CFGr;
                    rst_reg = NBIH_ADDITIONAL_RESETSr;
                    copy_no = 0;
                    tx_field = ELK_ON_ILKN_PORT_2_ELK_Nf;
                    break;
                case 3:
                    reg = NBIL_ELK_CFGr;
                    rst_reg = NBIL_ADDITIONAL_RESETSr;
                    copy_no = 1;
                    tx_field = ELK_ON_ILKN_ELK_Nf;
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOC_MSG("invalid ilkn number, %u"), offset));
            }
        } else {
            switch (offset) {
                case 0:
                    reg = NBIH_ELK_CFGr;
                    rst_reg = NBIH_ADDITIONAL_RESETSr;
                    copy_no = 0;
                    tx_field = ELK_ON_ILKN_PORT_0_ELK_Nf;
                    break;
                case 1:
                    reg = NBIH_ELK_CFGr;
                    rst_reg = NBIH_ADDITIONAL_RESETSr;
                    copy_no = 0;
                    tx_field = ELK_ON_ILKN_PORT_1_ELK_Nf;
                    break;
                case 2:
                    reg = NBIL_ELK_CFGr;
                    rst_reg = NBIL_ADDITIONAL_RESETSr;
                    copy_no = 0;
                    tx_field = ELK_ON_ILKN_PORT_0_ELK_Nf;
                    break;
                case 3:
                    reg = NBIL_ELK_CFGr;
                    rst_reg = NBIL_ADDITIONAL_RESETSr;
                    copy_no = 0;
                    tx_field = ELK_ON_ILKN_PORT_1_ELK_Nf;
                    break;
                case 4:
                    reg = NBIL_ELK_CFGr;
                    rst_reg = NBIL_ADDITIONAL_RESETSr;
                    copy_no = 1;
                    tx_field = ELK_ON_ILKN_PORT_0_ELK_Nf;
                    break;
                case 5:
                    reg = NBIL_ELK_CFGr;
                    rst_reg = NBIL_ADDITIONAL_RESETSr;
                    copy_no = 1;
                    tx_field = ELK_ON_ILKN_PORT_1_ELK_Nf;
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOC_MSG("invalid ilkn number, %u"), offset));
            }
        }

        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, copy_no, core, &reg_val)); 
        soc_reg_field_set(unit, reg, &reg_val, tx_field, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, copy_no, core, reg_val));
          
        
        SOCDNX_IF_ERR_EXIT(READ_NBIH_ELK_CFGr(unit, core, &reg_val));
        if (SOC_IS_QAX(unit)) {
            switch (offset) {
                case 0:
                case 2:
                    soc_reg_field_set(unit, NBIH_ELK_CFGr, &reg_val, ELK_RX_ACTIVE_NBIH_ELK_Nf, 1);
                    break;
                case 1:
                    soc_reg_field_set(unit, NBIH_ELK_CFGr, &reg_val, ELK_RX_ACTIVE_NBIL_0_ELK_Nf, 1);
                    break;
                case 3:
                    soc_reg_field_set(unit, NBIH_ELK_CFGr, &reg_val, ELK_RX_ACTIVE_NBIL_1_ELK_Nf, 1);
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOC_MSG("invalid ilkn number, %u"), offset));
            }
        } else {
            switch (offset/2) {
                case 0:
                    soc_reg_field_set(unit, NBIH_ELK_CFGr, &reg_val, ELK_RX_ACTIVE_NBIH_ELK_Nf, 1);
                    break;
                case 1:
                    soc_reg_field_set(unit, NBIH_ELK_CFGr, &reg_val, ELK_RX_ACTIVE_NBIL_0_ELK_Nf, 1);
                    break;
                case 2:
                    soc_reg_field_set(unit, NBIH_ELK_CFGr, &reg_val, ELK_RX_ACTIVE_NBIL_1_ELK_Nf, 1);
                    break;
                
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOC_MSG("invalid ilkn number, %u"), offset));
            }
        }
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ELK_CFGr(unit, core, reg_val));

        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, rst_reg, copy_no, 0, &reg_val)); 
        if (core == 0) {
            soc_reg_field_set(unit, rst_reg, &reg_val, ELK_0_RX_RSTNf, 1); 
            soc_reg_field_set(unit, rst_reg, &reg_val, ELK_0_TX_RSTNf, 1);
        } else {
            soc_reg_field_set(unit, rst_reg, &reg_val, ELK_1_RX_RSTNf, 1);
            soc_reg_field_set(unit, rst_reg, &reg_val, ELK_1_TX_RSTNf, 1);
        }
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, rst_reg, copy_no, 0, reg_val));
    }

    if (one_kbp_ilkn_at_least == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOC_MSG("invalid ilkn number")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif 

int soc_jer_init_hw_interfaces_set(int unit)
{
    CONST ARAD_MGMT_INIT *init = NULL;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    init = &SOC_DPP_CONFIG(unit)->arad->init;
    if (init->drc_info.enable) { 
        
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_dram_init(unit, &SOC_DPP_CONFIG(unit)->arad->init.drc_info));

        
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_dram_recovery_init,(unit)));
    }
#if defined(INCLUDE_KBP)
    if(init->elk.enable) {
        SOCDNX_IF_ERR_EXIT(arad_kbp_init_arad_interface(unit));
        SOCDNX_IF_ERR_EXIT(jer_kbp_init_jer_interface(unit));
    } 
#endif 

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int soc_jer_init_port(int unit)
{
    uint32 soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;
    soc_sand_rv = arad_mgmt_pon_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);  
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int soc_jer_init_mesh_topology(int unit)
{
    uint32 soc_sand_rv = 0, reg_val, field_val = 0, gt_size = -1;
    ARAD_INIT_FABRIC *fabric;
    SOCDNX_INIT_FUNC_DEFS;

    fabric = &(SOC_DPP_CONFIG(unit)->arad->init.fabric);

    soc_sand_rv = arad_init_mesh_topology(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    if(fabric->is_128_in_system) {
        field_val = 0x1;
    } else if (fabric->system_contains_multiple_pipe_device) {
        field_val = 0x2;
    } else {
        field_val = 0x0;
    }

    gt_size = soc_property_suffix_num_get(unit,0,spn_CUSTOM_FEATURE, "mesh_topology_size", field_val);

    switch (gt_size) {
    case 0:
        SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val)); 
        soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val, RESERVED_2f, gt_size);
        SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val));
        break;
    case 1:
        SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val)); 
        soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val, RESERVED_2f, gt_size);
        SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val));
        break;
    case 2:
        SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val)); 
        soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val, RESERVED_2f, gt_size);
        SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val));  
        break;
    case 3:
        SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val)); 
        soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val, RESERVED_2f, gt_size);
        SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOC_MSG("GT size %d in invalid"), gt_size));
        break;
    }
    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_0117r(unit, &reg_val));
    soc_reg_field_set(unit, MESH_TOPOLOGY_REG_0117r, &reg_val, FIELD_4_8f, 0xc);
    soc_reg_field_set(unit, MESH_TOPOLOGY_REG_0117r, &reg_val, FIELD_9_9f, SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_mesh_topology_fast ? 1 : 0);
    SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_REG_0117r(unit, reg_val));


exit:
    SOCDNX_FUNC_RETURN;
}



STATIC int soc_jer_init_functional_init(int unit)
{
    soc_pbmp_t pbmp;
    soc_port_t port_i;
    ARAD_MGMT_INIT* init;
    
    uint32 flags;
    int i = 0, index = 0;
    uint32 entry[20];
    int cpu_pp_ports[]  =     {0,     200,   201,   202,   203};
    int cpu_sys_ports[] =     {0xff,  0xc8,  0xc9,  0xca,  0xcb};
    
    uint32 soc_sand_rv = 0, rv = 0;    
    uint32 reg32_val;
    uint32 core_freq;
    uint32 slow_port_freq;
    uint32 bus_width;
    uint32 reassembly_timeout;

    SOCDNX_INIT_FUNC_DEFS;

    init = &(SOC_DPP_CONFIG(unit)->arad->init);

    if (SOC_IS_QAX(unit)) {
        if (SOC_DPP_CONFIG(unit)->emulation_system != 0) { 
            SOCDNX_IF_ERR_EXIT(soc_qax_init_dpp_defs_overrides(unit));
        }
    }

    if (!SOC_IS_FLAIR(unit)) {
        soc_sand_rv = arad_parser_init(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

        soc_sand_rv = arad_pmf_low_level_init_unsafe(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ipt_init,(unit)));
    }

    if (!SOC_IS_QAX(unit) || SOC_IS_QAX_WITH_FABRIC_ENABLED(unit))
    {
        rv = soc_jer_init_mesh_topology(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = soc_jer_fabric_init(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }


    
    if (!SOC_IS_FLAIR(unit)) {
        SOC_DPP_CORES_ITER(SOC_CORE_ALL, i) {
            SOCDNX_IF_ERR_EXIT(READ_EGQ_GENERAL_RQP_CONFIGr(unit, i, &reg32_val));
            soc_reg_field_set(unit, EGQ_GENERAL_RQP_CONFIGr, &reg32_val, RCM_MODEf, 2);
            if (!SOC_IS_QUX(unit)) {
                soc_reg_field_set(unit, EGQ_GENERAL_RQP_CONFIGr, &reg32_val, RCM_TDM_RESERVEDf, 0);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_GENERAL_RQP_CONFIGr(unit, i, reg32_val));
        }

        DCMN_RUNTIME_DEBUG_PRINT_LOC(unit, "soc_jer_init_functional_init before arad_ports_init");

        soc_sand_rv = arad_ports_init(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
        DCMN_RUNTIME_DEBUG_PRINT_LOC(unit, "soc_jer_init_functional_init after arad_ports_init");
    }

    if (SOC_IS_QAX(unit)) {
        
        for (i = 0; i < 5; i++) {
            index = 0x7e00 + cpu_pp_ports[i];
            SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, IHP_VIRTUAL_PORT_TABLEm, MEM_BLOCK_ALL, index, entry));
            soc_mem_field32_set(unit, IHP_VIRTUAL_PORT_TABLEm, entry, VIRTUAL_PORT_SRC_SYS_PORTf, cpu_sys_ports[i]);
            SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, IHP_VIRTUAL_PORT_TABLEm, MEM_BLOCK_ALL, index, entry));

            index = 0x7f00 + cpu_pp_ports[i];
            SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, IHP_VIRTUAL_PORT_TABLEm, MEM_BLOCK_ALL, index, entry));
            soc_mem_field32_set(unit, IHP_VIRTUAL_PORT_TABLEm, entry, VIRTUAL_PORT_SRC_SYS_PORTf, cpu_sys_ports[i]);
            SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, IHP_VIRTUAL_PORT_TABLEm, MEM_BLOCK_ALL, index, entry));
        }
    }

    if (!SOC_IS_FLAIR(unit)) {
        soc_sand_rv = soc_jer_trunk_init(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);  

        soc_sand_rv = arad_egr_prog_editor_unsafe(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);  

        soc_sand_rv = soc_jer_lbp_init(unit); 
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv); 

        if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable == TRUE) {
            rv = soc_jer_lif_init(unit);
            SOCDNX_IF_ERR_EXIT(rv);
        }

        soc_sand_rv = jer_tdm_init(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = jer_flow_control_init(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    if (!SOC_IS_QAX(unit) && !SOC_IS_FLAIR(unit))
    {
        if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable == FALSE) {
            soc_sand_rv = arad_pp_isem_access_init_unsafe(unit);
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

            
            arad_pp_flp_prog_sel_cam_key_program_tm(unit);
            arad_pp_flp_process_key_program_tm(unit);
        }
    }

    if (!SOC_IS_FLAIR(unit) && SOC_DPP_CONFIG(unit)->arad->init.pp_enable == FALSE) {
        soc_sand_rv = arad_pp_trap_mgmt_init_unsafe(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    
    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(jer_mgmt_set_mru_by_dbuff_size(unit));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &pbmp));
    SOC_PBMP_ITER(pbmp, port_i) {
        if (SOC_IS_QAX(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags));
            if (SOC_PORT_IS_LB_MODEM(flags)) {
                continue;
            }
        }
        SOCDNX_IF_ERR_EXIT(arad_ports_header_type_update(unit, port_i));
    }
    DCMN_RUNTIME_DEBUG_PRINT_LOC(unit, "soc_jer_init_functional_init after arad_ports_header_type_update per port");

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_ports_init_interfaces_erp_setting(unit, &(init->ports)));

    
    core_freq = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency/1000; 
    bus_width = 256;
    slow_port_freq = 100; 
    reassembly_timeout = bus_width * core_freq * 8 /slow_port_freq;

    if (SOC_IS_QAX(unit)) {
        SOCDNX_IF_ERR_EXIT(WRITE_SPB_REASSEMBLY_TIMEOUTr(unit, SOC_CORE_ALL, reassembly_timeout));
    } 
    else if (!SOC_IS_FLAIR(unit)){
        SOCDNX_IF_ERR_EXIT(WRITE_IDR_REASSEMBLY_TIMEOUTr(unit, reassembly_timeout));
    }


    if (SOC_IS_QAX(unit))
    {
#ifdef BCM_LB_SUPPORT
        if (SOC_DPP_CONFIG(unit)->qax->link_bonding_enable) {
            rv = qax_lb_init(unit);
            SOCDNX_IF_ERR_EXIT(rv);
        }
#endif

        SOC_EXIT;
    }

    
    if (!SOC_IS_FLAIR(unit)){
        uint32 reg32 = 0;
        soc_reg_field_set(unit, IPT_CFG_EVENT_CNT_SELr, &reg32, CFG_EVENT_CNT_SELf, 5);
        SOCDNX_IF_ERR_EXIT(WRITE_IPT_CFG_EVENT_CNT_SELr(unit, reg32));

        reg32=0;
        soc_reg_field_set(unit, IPT_CFG_BYTE_CNT_SRC_SELr, &reg32, CFG_BYTE_CNT_SRC_SELf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_IPT_CFG_BYTE_CNT_SRC_SELr(unit, reg32));
    }


exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t soc_jer_pp_mgmt_functional_init(
    int unit)
{
    uint32 soc_sand_rv = 0;
    uint32 reg32;
    int rv;
    uint8 non_first_fragments_enable;

    SOCDNX_INIT_FUNC_DEFS;    
        
    reg32 = 0;
    soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, ENABLE_DATA_PATHf, 1);
    soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, ENABLE_TCAM_BUBBLESf, 1);
    soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, ENABLE_MACT_BUBBLESf, 1);

    non_first_fragments_enable = (soc_property_get(unit, spn_FIELD_IP_FIRST_FRAGMENT_PARSED, TRUE)) ? 0x0 : 0x1;
    soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, NON_FIRST_FRAGMENTS_ENABLEf, non_first_fragments_enable);
    soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, MAX_HEADER_STACKf, 5);
    soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, ADDITIONAL_TPID_ENABLEf, 1);
    soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, LLC_CHECK_LSBf, 1);
    soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, IP_NEXT_PROTOCOL_GRE_ETHERNETf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_IHP_IHP_ENABLERSr(unit, SOC_CORE_ALL, reg32));

    
    soc_sand_rv = soc_jer_pp_ing_protection_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    soc_sand_rv = soc_jer_pp_eg_protection_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    
    if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) && (SOC_JER_PP_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_ENABLE == 1)) {
        soc_sand_rv = soc_jer_eg_encap_ip_tunnel_size_protocol_template_init(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) && !SOC_IS_QAX_A0(unit)) {
        soc_sand_rv = soc_jer_eg_encap_push_2_swap_init(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    
    rv = soc_jer_pp_oam_init_from_init_sequence(unit);
    SOCDNX_IF_ERR_EXIT(rv);


exit:
    SOCDNX_FUNC_RETURN;

}




int soc_jer_tbls_init(
    int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "init_tbls_to_zero", !(
#ifdef PLISIM
            SAL_BOOT_PLISIM || 
#endif
            SOC_DPP_CONFIG(unit)->emulation_system))) {
        SOCDNX_IF_ERR_EXIT(soc_jer_dynamic_tbls_reset(unit));
        SOCDNX_IF_ERR_EXIT(soc_jer_static_tbls_reset(unit)); 

        if (!SOC_IS_FLAIR(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_jer_iqmt_tbls_init(unit)); 

            SOCDNX_IF_ERR_EXIT(soc_jer_ppdb_tbls_init(unit));
        }
    } else {
        SOCDNX_IF_ERR_EXIT(WRITE_FDA_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
        if (!SOC_IS_FLAIR(unit)) {
            SOCDNX_IF_ERR_EXIT(WRITE_PPDB_A_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
            SOCDNX_IF_ERR_EXIT(WRITE_PPDB_B_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
            SOCDNX_IF_ERR_EXIT(WRITE_EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
            if (SOC_DPP_CONFIG(unit)->emulation_system) {
                uint32 data[1] = {0};
                
                SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EGQ_IVEC_TABLEm, data));
                SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EPNI_IVEC_TABLEm, data));
            }
        }
    }

    SOCDNX_IF_ERR_EXIT(arad_tbl_access_init_unsafe(unit));

    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_sch_tbls_init(unit));

        SOCDNX_IF_ERR_EXIT(soc_jer_irr_tbls_init(unit));

        SOCDNX_IF_ERR_EXIT(soc_jer_ire_tbls_init(unit));

        SOCDNX_IF_ERR_EXIT(soc_jer_ihp_tbls_init(unit));

        SOCDNX_IF_ERR_EXIT(soc_jer_ihb_tbls_init(unit));

        SOCDNX_IF_ERR_EXIT(soc_jer_iqm_tbls_init(unit)); 

        SOCDNX_IF_ERR_EXIT(soc_jer_ips_tbls_init(unit));
    }
    SOCDNX_IF_ERR_EXIT(soc_jer_fcr_tbls_init(unit));
    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_ipt_tbls_init(unit));
    }

    SOCDNX_IF_ERR_EXIT(soc_jer_fdt_tbls_init(unit));

    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_egq_tbls_init(unit));

        SOCDNX_IF_ERR_EXIT(soc_jer_epni_tbls_init(unit));

        SOCDNX_IF_ERR_EXIT(soc_jer_oamp_tbls_init(unit));

        SOCDNX_IF_ERR_EXIT(soc_jer_mrps_tbls_init(unit));

        SOCDNX_IF_ERR_EXIT(soc_jer_mrpsEm_tbls_init(unit));

        SOCDNX_IF_ERR_EXIT(soc_jer_idr_tbls_init(unit));

        SOCDNX_IF_ERR_EXIT(dpp_mult_rplct_tbl_entry_unoccupied_set_all(unit));
        SOCDNX_IF_ERR_EXIT(dpp_mcds_multicast_init2(unit));

        SOCDNX_IF_ERR_EXIT(soc_jer_xlp_tbls_init(unit));
        SOCDNX_IF_ERR_EXIT(soc_jer_clp_tbls_init(unit));
    }

exit:
    SOCDNX_FUNC_RETURN;
}




int soc_jer_tbls_deinit(
    int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_tbl_access_deinit(unit));

exit:
    SOCDNX_FUNC_RETURN;
}


CONST STATIC int dtq_max_size_contexts[] = {DTQ_0_MAX_SIZE_0f, DTQ_0_MAX_SIZE_1f, DTQ_0_MAX_SIZE_2f, DTQ_0_MAX_SIZE_3f, DTQ_0_MAX_SIZE_4f, DTQ_0_MAX_SIZE_5f,
                                            DTQ_1_MAX_SIZE_0f, DTQ_1_MAX_SIZE_1f, DTQ_1_MAX_SIZE_2f, DTQ_1_MAX_SIZE_3f, DTQ_1_MAX_SIZE_4f, DTQ_1_MAX_SIZE_5f};
CONST STATIC int dtq_start_addr_contexts[] = {DTQ_0_START_0f, DTQ_0_START_1f, DTQ_0_START_2f, DTQ_0_START_3f, DTQ_0_START_4f, DTQ_0_START_5f,
                                              DTQ_1_START_0f, DTQ_1_START_1f, DTQ_1_START_2f, DTQ_1_START_3f, DTQ_1_START_4f, DTQ_1_START_5f};
CONST STATIC int dtq_th_contexts[] = {DTQ_0_TH_0f, DTQ_0_TH_1f, DTQ_0_TH_2f, DTQ_0_TH_3f, DTQ_0_TH_4f, DTQ_0_TH_5f,
                                      DTQ_1_TH_0f, DTQ_1_TH_1f, DTQ_1_TH_2f, DTQ_1_TH_3f, DTQ_1_TH_4f, DTQ_1_TH_5f};
CONST STATIC int dtq_dqcf_th_contexts[] = {DTQ_0_DQCF_TH_0f, DTQ_0_DQCF_TH_1f, DTQ_0_DQCF_TH_2f, DTQ_0_DQCF_TH_3f, DTQ_0_DQCF_TH_4f, DTQ_0_DQCF_TH_5f, 
                                           DTQ_1_DQCF_TH_0f, DTQ_1_DQCF_TH_1f, DTQ_1_DQCF_TH_2f, DTQ_1_DQCF_TH_3f, DTQ_1_DQCF_TH_4f, DTQ_1_DQCF_TH_5f};
CONST STATIC int dqcf_0_max_size_contexts[] = {DQCF_0_MAX_SIZE_0f, DQCF_0_MAX_SIZE_1f, DQCF_0_MAX_SIZE_2f, DQCF_0_MAX_SIZE_3f, DQCF_0_MAX_SIZE_4f, DQCF_0_MAX_SIZE_5f, DQCF_0_MAX_SIZE_6f, 
                                               DQCF_0_MAX_SIZE_7f, DQCF_0_MAX_SIZE_8f, DQCF_0_MAX_SIZE_9f, DQCF_0_MAX_SIZE_10f, DQCF_0_MAX_SIZE_11f, DQCF_0_MAX_SIZE_12f, DQCF_0_MAX_SIZE_13f,
                                               DQCF_0_MAX_SIZE_14f, DQCF_0_MAX_SIZE_15f, DQCF_0_MAX_SIZE_16f, DQCF_0_MAX_SIZE_17f, DQCF_0_MAX_SIZE_18f, DQCF_0_MAX_SIZE_19f};
CONST STATIC int dqcf_1_max_size_contexts[] = {DQCF_1_MAX_SIZE_0f, DQCF_1_MAX_SIZE_1f, DQCF_1_MAX_SIZE_2f, DQCF_1_MAX_SIZE_3f, DQCF_1_MAX_SIZE_4f, DQCF_1_MAX_SIZE_5f, DQCF_1_MAX_SIZE_6f, 
                                               DQCF_1_MAX_SIZE_7f, DQCF_1_MAX_SIZE_8f, DQCF_1_MAX_SIZE_9f, DQCF_1_MAX_SIZE_10f, DQCF_1_MAX_SIZE_11f, DQCF_1_MAX_SIZE_12f, DQCF_1_MAX_SIZE_13f,
                                               DQCF_1_MAX_SIZE_14f, DQCF_1_MAX_SIZE_15f, DQCF_1_MAX_SIZE_16f, DQCF_1_MAX_SIZE_17f, DQCF_1_MAX_SIZE_18f, DQCF_1_MAX_SIZE_19f};
CONST STATIC int dqcf_0_start_addr_contexts[] = {DQCF_0_START_0f, DQCF_0_START_1f, DQCF_0_START_2f, DQCF_0_START_3f, DQCF_0_START_4f, DQCF_0_START_5f, DQCF_0_START_6f, 
                                                 DQCF_0_START_7f, DQCF_0_START_8f, DQCF_0_START_9f, DQCF_0_START_10f, DQCF_0_START_11f, DQCF_0_START_12f, DQCF_0_START_13f, 
                                                 DQCF_0_START_14f, DQCF_0_START_15f, DQCF_0_START_16f, DQCF_0_START_17f, DQCF_0_START_18f, DQCF_0_START_19f};
CONST STATIC int dqcf_1_start_addr_contexts[] = {DQCF_1_START_0f, DQCF_1_START_1f, DQCF_1_START_2f, DQCF_1_START_3f, DQCF_1_START_4f, DQCF_1_START_5f, DQCF_1_START_6f, 
                                                 DQCF_1_START_7f, DQCF_1_START_8f, DQCF_1_START_9f, DQCF_1_START_10f, DQCF_1_START_11f, DQCF_1_START_12f, DQCF_1_START_13f, 
                                                 DQCF_1_START_14f, DQCF_1_START_15f, DQCF_1_START_16f, DQCF_1_START_17f, DQCF_1_START_18f, DQCF_1_START_19f};
CONST STATIC int dqcf_0_th_contexts[] = {DQCF_0_DQCQ_TH_0f, DQCF_0_DQCQ_TH_1f, DQCF_0_DQCQ_TH_2f, DQCF_0_DQCQ_TH_3f, DQCF_0_DQCQ_TH_4f, DQCF_0_DQCQ_TH_5f, DQCF_0_DQCQ_TH_6f, 
                                         DQCF_0_DQCQ_TH_7f, DQCF_0_DQCQ_TH_8f, DQCF_0_DQCQ_TH_9f, DQCF_0_DQCQ_TH_10f, DQCF_0_DQCQ_TH_11f, DQCF_0_DQCQ_TH_12f, DQCF_0_DQCQ_TH_13f, 
                                         DQCF_0_DQCQ_TH_14f, DQCF_0_DQCQ_TH_15f, DQCF_0_DQCQ_TH_16f, DQCF_0_DQCQ_TH_17f, DQCF_0_DQCQ_TH_18f, DQCF_0_DQCQ_TH_19f};
CONST STATIC int dqcf_1_th_contexts[] = {DQCF_1_DQCQ_TH_0f, DQCF_1_DQCQ_TH_1f, DQCF_1_DQCQ_TH_2f, DQCF_1_DQCQ_TH_3f, DQCF_1_DQCQ_TH_4f, DQCF_1_DQCQ_TH_5f, DQCF_1_DQCQ_TH_6f, 
                                         DQCF_1_DQCQ_TH_7f, DQCF_1_DQCQ_TH_8f, DQCF_1_DQCQ_TH_9f, DQCF_1_DQCQ_TH_10f, DQCF_1_DQCQ_TH_11f, DQCF_1_DQCQ_TH_12f, DQCF_1_DQCQ_TH_13f, 
                                         DQCF_1_DQCQ_TH_14f, DQCF_1_DQCQ_TH_15f, DQCF_1_DQCQ_TH_16f, DQCF_1_DQCQ_TH_17f, DQCF_1_DQCQ_TH_18f, DQCF_1_DQCQ_TH_19f};
CONST STATIC int dqcf_0_eir_th_contexts[] = {DQCF_0_EIR_CRDT_TH_0f, DQCF_0_EIR_CRDT_TH_1f, DQCF_0_EIR_CRDT_TH_2f, DQCF_0_EIR_CRDT_TH_3f, DQCF_0_EIR_CRDT_TH_4f, DQCF_0_EIR_CRDT_TH_5f, DQCF_0_EIR_CRDT_TH_6f, 
                                             DQCF_0_EIR_CRDT_TH_7f, DQCF_0_EIR_CRDT_TH_8f, DQCF_0_EIR_CRDT_TH_9f, DQCF_0_EIR_CRDT_TH_10f, DQCF_0_EIR_CRDT_TH_11f, DQCF_0_EIR_CRDT_TH_12f, DQCF_0_EIR_CRDT_TH_13f, 
                                             DQCF_0_EIR_CRDT_TH_14f, DQCF_0_EIR_CRDT_TH_15f, DQCF_0_EIR_CRDT_TH_16f, DQCF_0_EIR_CRDT_TH_17f, DQCF_0_EIR_CRDT_TH_18f, DQCF_0_EIR_CRDT_TH_19f};
CONST STATIC int dqcf_1_eir_th_contexts[] = {DQCF_1_EIR_CRDT_TH_0f, DQCF_1_EIR_CRDT_TH_1f, DQCF_1_EIR_CRDT_TH_2f, DQCF_1_EIR_CRDT_TH_3f, DQCF_1_EIR_CRDT_TH_4f, DQCF_1_EIR_CRDT_TH_5f, DQCF_1_EIR_CRDT_TH_6f, 
                                             DQCF_1_EIR_CRDT_TH_7f, DQCF_1_EIR_CRDT_TH_8f, DQCF_1_EIR_CRDT_TH_9f, DQCF_1_EIR_CRDT_TH_10f, DQCF_1_EIR_CRDT_TH_11f, DQCF_1_EIR_CRDT_TH_12f, DQCF_1_EIR_CRDT_TH_13f, 
                                             DQCF_1_EIR_CRDT_TH_14f, DQCF_1_EIR_CRDT_TH_15f, DQCF_1_EIR_CRDT_TH_16f, DQCF_1_EIR_CRDT_TH_17f, DQCF_1_EIR_CRDT_TH_18f, DQCF_1_EIR_CRDT_TH_19f};

STATIC int
soc_jer_ipt_dtq_contexts_init(int unit, int is_dram, int is_dual_fap, int nof_pipes)
{
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 dtq_max_size[12] = {0};
    uint32 *dtq_0_max_size = dtq_max_size, *dtq_1_max_size = dtq_max_size + 6;
    uint32 ctx_max_size_used, ctx_max_size_unused = 0x2; 
    uint32 total_size_dual =  1 * 1024 - 1;
    uint32 total_size_single =  2 * 1024 - 1;
    uint32 dtq_start_addr;
    int i, is_mesh, nof_unused, nof_used;
    SOCDNX_INIT_FUNC_DEFS;


    is_mesh = SOC_DPP_IS_MESH(unit) || 
        (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP);
    
    if (is_mesh) {
        nof_unused = (is_dram) ? 0 : 3; 
    } else {
        switch (nof_pipes) {
        case 1:
            nof_unused = (is_dram) ? 4 : 5; 
            break;
        case 2:
            nof_unused = (is_dram) ? 2 : 4; 
            break;
        case 3:
            nof_unused = (is_dram) ? 0 : 3; 
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOC_MSG("number of pipes %d in invalid"), nof_pipes));
        }
    }

    nof_used = SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE - nof_unused;
    total_size_dual = total_size_dual - ctx_max_size_unused * nof_unused;
    total_size_single = total_size_single - ctx_max_size_unused * nof_unused;
    ctx_max_size_used = (is_dual_fap) ? (total_size_dual / nof_used) : (total_size_single / nof_used) ;

    for (i = 0; i < SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE; i += 2) {
        if (is_dram) {
            dtq_0_max_size[i] = 2 * ctx_max_size_used / 3; 
            dtq_0_max_size[i + 1] = 4 * ctx_max_size_used / 3;
        } else {
            dtq_0_max_size[i] = ctx_max_size_used; 
            dtq_0_max_size[i + 1] = ctx_max_size_unused;
        }
 
        if (is_dual_fap) {
            if (is_dram) {
                dtq_1_max_size[i] = 2 * ctx_max_size_used / 3; 
                dtq_1_max_size[i + 1] = 4 * ctx_max_size_used / 3;
            } else {
                dtq_1_max_size[i] = ctx_max_size_used; 
                dtq_1_max_size[i + 1] = ctx_max_size_unused;
            }
        }
    }
    if ((!is_mesh) && (nof_pipes * 2 < SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE)) {
        for (i = nof_pipes * 2; i < SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE; ++i) {
            dtq_0_max_size[i] = ctx_max_size_unused;
            if (is_dual_fap) {
                dtq_1_max_size[i] = ctx_max_size_unused;
            }
        }
    }
    SOCDNX_IF_ERR_EXIT(READ_IPT_DTQ_MAX_SIZEr(unit, reg_above_64_val)); 
    for (i = 0; i < SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE * 2; ++i) {
        soc_reg_above_64_field32_set(unit, IPT_DTQ_MAX_SIZEr, reg_above_64_val, dtq_max_size_contexts[i], dtq_max_size[i]);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DTQ_MAX_SIZEr(unit, reg_above_64_val)); 

    
    dtq_start_addr = 0;
    SOCDNX_IF_ERR_EXIT(READ_IPT_DTQ_STARTr(unit, reg_above_64_val)); 
    soc_reg_above_64_field32_set(unit, IPT_DTQ_STARTr, reg_above_64_val, dtq_start_addr_contexts[0], dtq_start_addr);
    for (i = 1; i < SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE * 2; ++i) {
        dtq_start_addr += dtq_max_size[i - 1];
        soc_reg_above_64_field32_set(unit, IPT_DTQ_STARTr, reg_above_64_val, dtq_start_addr_contexts[i], dtq_start_addr);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DTQ_STARTr(unit, reg_above_64_val)); 

    
    SOCDNX_IF_ERR_EXIT(READ_IPT_DTQ_THr(unit, reg_above_64_val)); 
    for (i = 0; i < SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE * 2; ++i) {
        soc_reg_above_64_field32_set(unit, IPT_DTQ_THr, reg_above_64_val, dtq_th_contexts[i], dtq_max_size[i]/2);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DTQ_THr(unit, reg_above_64_val)); 

    SOCDNX_IF_ERR_EXIT(READ_IPT_DTQ_DQCF_THr(unit, reg_above_64_val)); 
    for (i = 0; i < SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE * 2; ++i) {
        soc_reg_above_64_field32_set(unit, IPT_DTQ_DQCF_THr, reg_above_64_val, dtq_dqcf_th_contexts[i], dtq_max_size[i]/2);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DTQ_DQCF_THr(unit, reg_above_64_val));

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC int 
soc_jer_ipt_dqcf_contexts_init(int unit, int is_dram, int is_dual_fap)
{
    soc_reg_above_64_val_t reg_above_64_val_0, reg_above_64_val_1;
    uint64 reg64_val;
    uint32 dbl_rsrc, reg_val;
    uint32 dqcf_0_max_size[20] = {0}, dqcf_1_max_size[20] = {0};
    uint32 dqcf_0_th_size[20] = {0}, dqcf_1_th_size[20] = {0};
    uint32 ocb_only_size, dram_ocb_size; 
    uint32 dqcf_0_start_addr, dqcf_1_start_addr;
    uint32 ctx_max_size_unused, ctx_max_size_used;
    uint32 size_dual, size_single;
    uint32 nof_unused_single, nof_unused_dual;
    uint32 total_size_dual =  8 * 1024 - 1;
    uint32 total_size_single =  16 * 1024 - 1;
    int is_mesh;
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    ctx_max_size_unused = 0x3;

    

    

    SOCDNX_IF_ERR_EXIT(READ_IPT_IPT_ENABLESr(unit, &reg_val));
    dbl_rsrc = soc_reg_field_get(unit, IPT_IPT_ENABLESr, reg_val, DBL_RSRC_ENf);
    
    is_mesh = SOC_DPP_IS_MESH(unit) || 
        (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP);

    if (is_mesh) {
        if (is_dram) { 
            nof_unused_dual = 0; 
            nof_unused_single = 4; 
            size_dual = (total_size_dual - nof_unused_dual * ctx_max_size_unused)/(SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE - nof_unused_dual);
            if (dbl_rsrc) {
                size_single = (total_size_single - nof_unused_single * ctx_max_size_unused) / (SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE - nof_unused_single); 
            } else {
                size_single = (total_size_dual - nof_unused_single * ctx_max_size_unused) / (SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE - nof_unused_single);
            }
            ctx_max_size_used = (is_dual_fap) ? size_dual : size_single;   
            ocb_only_size = 2 * ctx_max_size_used / 3;
            dram_ocb_size = 4 * ctx_max_size_used / 3;        
        } else {
            nof_unused_dual = 10; 
            nof_unused_single = 12; 
            size_dual = (total_size_dual - nof_unused_dual * ctx_max_size_unused)/(SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE - nof_unused_dual);
            if (dbl_rsrc) {
                size_single = (total_size_single - nof_unused_single * ctx_max_size_unused) / (SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE - nof_unused_single); 
            } else {
                size_single = (total_size_dual - nof_unused_single * ctx_max_size_unused) / (SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE - nof_unused_single);
            }
            ctx_max_size_used = (is_dual_fap) ? size_dual : size_single;   
            ocb_only_size = ctx_max_size_used;
            dram_ocb_size = ctx_max_size_unused;  
        }
    } else {
        if (is_dram) {
            
            nof_unused_dual = 4;
            nof_unused_single = 8; 
            size_dual = (total_size_dual - nof_unused_dual * ctx_max_size_unused)/(SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE - nof_unused_dual);
            if (dbl_rsrc) {
                size_single = (total_size_single - nof_unused_single * ctx_max_size_unused) / (SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE - nof_unused_single); 
            } else {
                size_single = (total_size_dual - nof_unused_single * ctx_max_size_unused) / (SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE - nof_unused_single);
            }
            ctx_max_size_used = (is_dual_fap) ? size_dual : size_single ;
            ocb_only_size = 2 * ctx_max_size_used / 3;
            dram_ocb_size = 4 * ctx_max_size_used / 3;  
        } else {
            
            nof_unused_dual = 12;
            nof_unused_single = 14; 
            size_dual = (total_size_dual - nof_unused_dual * ctx_max_size_unused)/(SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE - nof_unused_dual);
            if (dbl_rsrc) {
                size_single = (total_size_single - nof_unused_single * ctx_max_size_unused) / (SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE - nof_unused_single); 
            } else {
                size_single = (total_size_dual - nof_unused_single * ctx_max_size_unused) / (SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE - nof_unused_single);
            }
            ctx_max_size_used = (is_dual_fap) ? size_dual : size_single ;
            ocb_only_size = ctx_max_size_used;
            dram_ocb_size = ctx_max_size_unused;  
        }
    }

    for (i = 0; i < SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE; i += 4) {
        dqcf_0_max_size[i] = dqcf_0_max_size[1 + i] = ocb_only_size;
        dqcf_0_max_size[2 + i] = dqcf_0_max_size[3 + i] = dram_ocb_size;
        if (is_dual_fap) {
            dqcf_1_max_size[i] = dqcf_1_max_size[1 + i] = ocb_only_size;
            dqcf_1_max_size[2 + i] = dqcf_1_max_size[3 + i] = dram_ocb_size;
        } else {
            dqcf_1_max_size[i] = dqcf_1_max_size[1 + i] = ctx_max_size_unused;
            dqcf_1_max_size[2 + i] = dqcf_1_max_size[3 + i] = ctx_max_size_unused;
        }
    }
    if (!is_dual_fap) {
        dqcf_0_max_size[4] = dqcf_0_max_size[5] = dqcf_0_max_size[6] = dqcf_0_max_size[7] = ctx_max_size_unused; 
    }

    if (!is_mesh) {
        dqcf_0_max_size[8] = dqcf_0_max_size[9] = dqcf_0_max_size[10] = dqcf_0_max_size[11] = ctx_max_size_unused; 
        if (is_dual_fap) {
            dqcf_1_max_size[8] = dqcf_1_max_size[9] = dqcf_1_max_size[10] = dqcf_1_max_size[11] = ctx_max_size_unused;
        }

    }

    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_0_MAX_SIZEr(unit, reg_above_64_val_0)); 
    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_1_MAX_SIZEr(unit, reg_above_64_val_1)); 
    for (i = 0; i < SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE; ++i) {
        soc_reg_above_64_field32_set(unit, IPT_DQCF_0_MAX_SIZEr, reg_above_64_val_0, dqcf_0_max_size_contexts[i], dqcf_0_max_size[i]);
        soc_reg_above_64_field32_set(unit, IPT_DQCF_1_MAX_SIZEr, reg_above_64_val_1, dqcf_1_max_size_contexts[i], dqcf_1_max_size[i]);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_0_MAX_SIZEr(unit, reg_above_64_val_0)); 
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_1_MAX_SIZEr(unit, reg_above_64_val_1)); 

    
    dqcf_0_start_addr = 0;
    dqcf_1_start_addr = 0;
    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_0_STARTr(unit, reg_above_64_val_0));
    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_1_STARTr(unit, reg_above_64_val_1));
    soc_reg_above_64_field32_set(unit, IPT_DQCF_0_STARTr, reg_above_64_val_0, dqcf_0_start_addr_contexts[0], 0);
    soc_reg_above_64_field32_set(unit, IPT_DQCF_1_STARTr, reg_above_64_val_1, dqcf_1_start_addr_contexts[0], 0);
    for (i = 1; i < SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE; ++i) {
        dqcf_0_start_addr += dqcf_0_max_size[i - 1];
        dqcf_1_start_addr += dqcf_1_max_size[i - 1];
        soc_reg_above_64_field32_set(unit, IPT_DQCF_0_STARTr, reg_above_64_val_0, dqcf_0_start_addr_contexts[i], dqcf_0_start_addr);
        soc_reg_above_64_field32_set(unit, IPT_DQCF_1_STARTr, reg_above_64_val_1, dqcf_1_start_addr_contexts[i], dqcf_1_start_addr);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_0_STARTr(unit, reg_above_64_val_0));
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_1_STARTr(unit, reg_above_64_val_1));

    
    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_0_DQCQ_THr(unit, reg_above_64_val_0)); 
    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_1_DQCQ_THr(unit, reg_above_64_val_1)); 
    for (i = 0; i < SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE; ++i) {
        dqcf_0_th_size[i] = (dqcf_0_max_size[i] == ctx_max_size_unused) ? ctx_max_size_unused : dqcf_0_max_size[i]/2 - 1;  
        dqcf_1_th_size[i] = (dqcf_1_max_size[i] == ctx_max_size_unused) ? ctx_max_size_unused : dqcf_1_max_size[i]/2 - 1;

        soc_reg_above_64_field32_set(unit, IPT_DQCF_0_DQCQ_THr, reg_above_64_val_0, dqcf_0_th_contexts[i], dqcf_0_th_size[i]); 
        soc_reg_above_64_field32_set(unit, IPT_DQCF_1_DQCQ_THr, reg_above_64_val_1, dqcf_1_th_contexts[i], dqcf_1_th_size[i]);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_0_DQCQ_THr(unit, reg_above_64_val_0));
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_1_DQCQ_THr(unit, reg_above_64_val_1));

    
    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_0_EIR_CRDT_THr(unit, reg_above_64_val_0)); 
    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_1_EIR_CRDT_THr(unit, reg_above_64_val_1)); 
    for (i = 0; i < SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE; ++i) {
        dqcf_0_th_size[i] = (dqcf_0_max_size[i] == ctx_max_size_unused) ? ctx_max_size_unused : 2*dqcf_0_max_size[i]/3 - 1;  
        dqcf_1_th_size[i] = (dqcf_1_max_size[i] == ctx_max_size_unused) ? ctx_max_size_unused : 2*dqcf_1_max_size[i]/3 - 1;

         soc_reg_above_64_field32_set(unit, IPT_DQCF_0_EIR_CRDT_THr, reg_above_64_val_0, dqcf_0_eir_th_contexts[i], dqcf_0_th_size[i]);
         soc_reg_above_64_field32_set(unit, IPT_DQCF_1_EIR_CRDT_THr, reg_above_64_val_1, dqcf_1_eir_th_contexts[i], dqcf_1_th_size[i]);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_0_EIR_CRDT_THr(unit, reg_above_64_val_0));
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_1_EIR_CRDT_THr(unit, reg_above_64_val_1));

    
    if (!is_mesh) { 
        for (i = 16; i < 19; ++i) {
            dqcf_0_th_size[i] = (dqcf_0_max_size[i] == ctx_max_size_unused) ? ctx_max_size_unused : 2*dqcf_0_max_size[i]/3 - 1;  
            dqcf_1_th_size[i] = (dqcf_1_max_size[i] == ctx_max_size_unused) ? ctx_max_size_unused : 2*dqcf_1_max_size[i]/3 - 1;
        }
        SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_0_MC_THr(unit, &reg64_val));
        soc_reg64_field32_set(unit, IPT_DQCF_0_MC_THr, &reg64_val, DQCF_0_MC_GFMC_OCB_THf, dqcf_0_th_size[16]);
        soc_reg64_field32_set(unit, IPT_DQCF_0_MC_THr, &reg64_val, DQCF_0_MC_BFMC_OCB_THf, dqcf_0_th_size[17]);
        soc_reg64_field32_set(unit, IPT_DQCF_0_MC_THr, &reg64_val, DQCF_0_MC_GFMC_MIX_THf, dqcf_0_th_size[18]);
        soc_reg64_field32_set(unit, IPT_DQCF_0_MC_THr, &reg64_val, DQCF_0_MC_BFMC_MIX_THf, dqcf_0_th_size[19]);
        SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_0_MC_THr(unit, reg64_val));

        SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_1_MC_THr(unit, &reg64_val)); 
        soc_reg64_field32_set(unit, IPT_DQCF_1_MC_THr, &reg64_val, DQCF_1_MC_GFMC_OCB_THf, dqcf_1_th_size[16]);
        soc_reg64_field32_set(unit, IPT_DQCF_1_MC_THr, &reg64_val, DQCF_1_MC_BFMC_OCB_THf, dqcf_1_th_size[17]);
        soc_reg64_field32_set(unit, IPT_DQCF_1_MC_THr, &reg64_val, DQCF_1_MC_GFMC_MIX_THf, dqcf_1_th_size[18]);
        soc_reg64_field32_set(unit, IPT_DQCF_1_MC_THr, &reg64_val, DQCF_1_MC_BFMC_MIX_THf, dqcf_1_th_size[19]);
        SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_1_MC_THr(unit, reg64_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
soc_jer_ipt_contexts_init(
  SOC_SAND_IN int     unit)
{
    uint32 nof_pipes, is_dual_fap, is_dram, reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    nof_pipes = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pipe_map_config.nof_pipes;
    is_dram = SOC_DPP_CONFIG(unit)->arad->init.drc_info.dram_num ? 1 : 0;
    is_dual_fap = (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1) ? 1 : 0 ;

    
    SOCDNX_IF_ERR_EXIT(READ_IPT_IPT_ENABLESr(unit, &reg_val));
    soc_reg_field_set(unit, IPT_IPT_ENABLESr, &reg_val, IPT_ENf, 1);
    soc_reg_field_set(unit, IPT_IPT_ENABLESr, &reg_val, ZERO_ACT_LINKS_RATE_ENf, 1);
    
    if (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores == 1) {
        soc_reg_field_set(unit, IPT_IPT_ENABLESr, &reg_val, DBL_RSRC_ENf, 1);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_IPT_ENABLESr(unit, reg_val));

    
    SOCDNX_IF_ERR_EXIT(soc_jer_ipt_dtq_contexts_init(unit, is_dram, is_dual_fap, nof_pipes));

    
    SOCDNX_IF_ERR_EXIT(soc_jer_ipt_dqcf_contexts_init(unit, is_dram, is_dual_fap));

    
    SOCDNX_IF_ERR_EXIT(jer_itm_ingress_latency_init(unit));    
exit:
    SOCDNX_FUNC_RETURN;
}


int soc_qax_ihb_tbls_init(int unit)
{
    uint32 table_entry[5] = {0};
    uint64 data;
    int core;

    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_SET(data, 0, 0x7ffff);
    if (SOC_MEM_IS_VALID(unit, IHB_FEC_ENTRYm)) {
        soc_mem_field64_set(unit, IHB_FEC_ENTRYm, table_entry, DATA_0f, data);
        soc_mem_field64_set(unit, IHB_FEC_ENTRYm, table_entry, DATA_1f, data);
        soc_mem_field32_set(unit, IHB_FEC_ENTRYm, table_entry, PROTECTION_POINTERf, 0x7fff);

        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
            SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IHB_FEC_ENTRYm, IHB_BLOCK(unit, core), table_entry));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

static int soc_flair_fgl_regs_init(int unit)
{
    uint32 reg_val;
    soc_reg_above_64_val_t reg_val_above_64;
    SOCDNX_INIT_FUNC_DEFS;

    reg_val = SOC_REG_INFO(unit, ECI_FGL_EGRESS_CONFIGr).rst_val_lo;
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FGL_EGRESS_CONFIGr(unit, reg_val));

    reg_val = SOC_REG_INFO(unit, ECI_FGL_EGR_MC_CONTROLr).rst_val_lo;
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FGL_EGR_MC_CONTROLr(unit, reg_val));

    reg_val = SOC_REG_INFO(unit, ECI_FGL_INGRESS_CONFIGr).rst_val_lo;
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FGL_INGRESS_CONFIGr(unit, reg_val));

    reg_val = SOC_REG_INFO(unit, ECI_FGL_LOOPBACK_CONTROLr).rst_val_lo;
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FGL_LOOPBACK_CONTROLr(unit, reg_val));

    SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FGL_EGR_2_ING_LOOPBACK_CONTROLr(unit, reg_val_above_64));
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FGL_INGRESS_TDM_CONTEXT_DROPr(unit, reg_val_above_64));

exit:
    SOCDNX_FUNC_RETURN;
}

static int soc_flair_tdm_tbls_init(int unit)
{
    uint32 table_entry[8] = {0};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, ECI_TDM_CONFIGm, MEM_BLOCK_ANY, table_entry));
exit:
    SOCDNX_FUNC_RETURN;
}

static int soc_flair_port_map_tbls_init(int unit)
{
    uint32 table_entry[8] = {0};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, ECI_EGR_PORT_MAPm, MEM_BLOCK_ANY, table_entry));
exit:
    SOCDNX_FUNC_RETURN;
}

static int soc_flair_mct_tbls_init(int unit)
{
    uint32 table_entry[8] = {0};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, ECI_EGR_MCTm, MEM_BLOCK_ANY, table_entry));
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, ECI_EGR_MCT_MAPPINGm, MEM_BLOCK_ANY, table_entry));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_init_sequence_phase1(int unit)
{
    ARAD_MGMT_INIT* init;
    uint32 rval,is_valid,port_i;
    int core;
    uint64 r64;
    char* slow_rate_max_level_str;
    int etpp_pipe_len = 0;
    SOCDNX_INIT_FUNC_DEFS;

    init = &(SOC_DPP_CONFIG(unit)->arad->init);

#ifdef PLISIM
    if (SAL_BOOT_PLISIM) {
      chip_sim_em_init(unit, SOC_SAND_DEV_ARAD, TRUE , FALSE );
    }
#endif

    DCMN_RUNTIME_DEBUG_PRINT_LOC(unit, "starting oc_jer_init_sequence_phase1");
    
    SOCDNX_IF_ERR_EXIT(soc_jer_init_prepare_internal_data(unit));

    if (!SOC_WARM_BOOT(unit)) {
        
        LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Blocks Initial configuration\n"),unit));
        SOCDNX_IF_ERR_EXIT(soc_jer_init_blocks_init(unit));
        DCMN_RUNTIME_DEBUG_PRINT(unit);

       
       LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Configure tables defaults\n"),unit));

        if (SOC_IS_QAX(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_qax_tbls_init(unit));
            if (SOC_DPP_CONFIG(unit)->qax->link_bonding_enable) {
                SOCDNX_IF_ERR_EXIT(qax_lb_egr_init(unit));
            }

            SOCDNX_IF_ERR_EXIT(soc_qax_init_blocks_init_conf(unit));
        } else {
            SOCDNX_IF_ERR_EXIT(SOC_IS_JERICHO_PLUS(unit) ? soc_jerplus_tbls_init(unit) : soc_jer_tbls_init(unit));
            SOCDNX_IF_ERR_EXIT(soc_jer_init_blocks_init_conf(unit));
        }

        DCMN_RUNTIME_DEBUG_PRINT(unit);

        
        if(!SOC_IS_FLAIR(unit)) {
            etpp_pipe_len = 0x28;    
            if (SOC_IS_QUX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
                etpp_pipe_len = 111;
            }

            SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                SOCDNX_IF_ERR_EXIT(READ_EPNI_ETPP_PIPE_LENGTHr(unit, core, &rval));
                soc_reg_field_set(unit, EPNI_ETPP_PIPE_LENGTHr, &rval, ETPP_PIPE_LENGTHf, etpp_pipe_len);
                SOCDNX_IF_ERR_EXIT(WRITE_EPNI_ETPP_PIPE_LENGTHr(unit, core, rval));
            }
        }

        
        if (!SOC_IS_QAX(unit) && !SOC_IS_FLAIR(unit) && !SOC_WARM_BOOT(unit)) {
            SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                SOCDNX_IF_ERR_EXIT(READ_EPNI_GLOBAL_CONFIGr(unit, core, &rval));
                soc_reg_field_set(unit, EPNI_GLOBAL_CONFIGr, &rval, STATISTICS_ENAf, 0);
                SOCDNX_IF_ERR_EXIT(WRITE_EPNI_GLOBAL_CONFIGr(unit, core, rval));
            }
        }

        
        if(!SOC_IS_FLAIR(unit)) {
            core = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores - 1; 
            if (core > 0) {
                SOCDNX_IF_ERR_EXIT(READ_EPNI_MAPPING_RECYCLE_TRAFFICTO_INGRESS_PIPEr(unit, core, &rval));
                soc_reg_field_set(unit, EPNI_MAPPING_RECYCLE_TRAFFICTO_INGRESS_PIPEr, &rval, MAPPING_TRAP_TO_INGRESS_PIPEf, core);
                soc_reg_field_set(unit, EPNI_MAPPING_RECYCLE_TRAFFICTO_INGRESS_PIPEr, &rval, MAPPING_RCY_TO_INGRESS_PIPEf, core);
                soc_reg_field_set(unit, EPNI_MAPPING_RECYCLE_TRAFFICTO_INGRESS_PIPEr, &rval, MAPPING_MIRROR_TO_INGRESS_PIPEf, core);
                SOCDNX_IF_ERR_EXIT(WRITE_EPNI_MAPPING_RECYCLE_TRAFFICTO_INGRESS_PIPEr(unit, core, rval));
            }
        }

        
        LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Soft init Device Blocks\n"),unit));
        SOCDNX_IF_ERR_EXIT(soc_jer_reset_blocks_soft_init(unit, SOC_DPP_RESET_ACTION_INOUT_RESET));
        DCMN_RUNTIME_DEBUG_PRINT(unit);

        if (SOC_IS_QAX(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_qax_ihb_tbls_init(unit));
        }

        if (!SOC_IS_QAX(unit)) { 
            
            LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Blocks General configuration\n"),unit));
            SOCDNX_IF_ERR_EXIT(soc_jer_init_blocks_general(unit));
            DCMN_RUNTIME_DEBUG_PRINT(unit);
        }
        else
        { 
            
            LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Blocks General configuration\n"),unit));
            SOCDNX_IF_ERR_EXIT(soc_qax_init_blocks_general(unit));
            
            SOCDNX_IF_ERR_EXIT(jer_mgmt_drv_pvt_monitor_enable(unit));
            DCMN_RUNTIME_DEBUG_PRINT(unit);
        }
        
        
        LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Set core clock frequency\n"),unit));
        SOCDNX_IF_ERR_EXIT(arad_mgmt_init_set_core_clock_frequency(unit, init));
        DCMN_RUNTIME_DEBUG_PRINT(unit);

        
        if (init->credit.credit_worth_enable)
        {
            SOCDNX_IF_ERR_EXIT(jer_mgmt_credit_worth_set(unit, init->credit.credit_worth));
        }

        
        DCMN_RUNTIME_DEBUG_PRINT(unit);
        LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Set hardware adjustments (Dram)\n"),unit));
        SOCDNX_IF_ERR_EXIT(soc_jer_init_hw_interfaces_set(unit));

        
        if(!SOC_IS_FLAIR(unit)) {
            SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_config_dut_by_queue_database(unit));
        }

        
        if (!SOC_IS_FLAIR(unit) && (SOC_IS_JERICHO(unit) || SOC_IS_QAX(unit))) {
            LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Set port configurations \n"),unit));
            SOCDNX_IF_ERR_EXIT(soc_jer_init_port(unit));
            DCMN_RUNTIME_DEBUG_PRINT(unit);
        }

        if (SOC_IS_QAX(unit)) {
            SOCDNX_IF_ERR_EXIT(READ_EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr(unit, SOC_CORE_ALL, &r64));
            soc_reg64_field32_set(unit, EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, &r64, EGRESS_MC_1_REP_ENABLEf, 15);
            soc_reg64_field32_set(unit, EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, &r64, EGRESS_MC_2_REP_ENABLEf, 3);
            soc_reg64_field32_set(unit, EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, &r64, EGRESS_REP_DIRECT_BITMAP_CUDf, 0);
            soc_reg64_field32_set(unit, EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, &r64, TDM_REP_FORMAT_ENf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr(unit, SOC_CORE_ALL, r64));
        } else {
        }

        

        DISPLAY_SW_STATE_MEM ;

        LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Set default module configurations \n"),unit));
        SOCDNX_IF_ERR_EXIT(soc_jer_init_functional_init(unit));
        DCMN_RUNTIME_DEBUG_PRINT(unit);

        if (!SOC_IS_FLAIR(unit)) {
            
            LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Set Egress Port mapping \n"),unit));
            SOCDNX_IF_ERR_EXIT(soc_jer_egr_tm_init(unit));
            DCMN_RUNTIME_DEBUG_PRINT(unit);

            
            LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: OFP Rates Initialization\n"),unit));
            SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_init(unit));
            DCMN_RUNTIME_DEBUG_PRINT(unit);

            
            LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Scheduler Initialization\n"),unit));
            SOCDNX_IF_ERR_EXIT(soc_jer_sch_init(unit));

            LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Ingress Packet Queuing Initialization\n"),unit));
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_ipq_init,(unit)));

            LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Ingress Traffic Management Initialization\n"),unit));
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_init,(unit)));

            
            if (SOC_IS_ROO_ENABLE(unit)) {

                DISPLAY_SW_STATE_MEM ;

                LOG_INFO(BSL_LS_SOC_INIT,
                         (BSL_META_U(unit,
                                     "\t+ %d: Set 2nd myMAC configurations for ROO \n"),unit));
                SOCDNX_IF_ERR_EXIT(soc_jer_mymac_2nd_mymac_init(unit));
            }
             
            if (init->pll.ts_clk_mode == 0x1) {
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, 1));
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, SOC_DPP_CONFIG(unit)->arad->init.pll.ts_pll_phase_initial_lo));
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, 1));
            }

            
            if (soc_property_get(unit, spn_SAT_ENABLE, 0)) {
                SOCDNX_IF_ERR_EXIT(soc_jer_pp_oam_sat_init(unit));
            }

            SOCDNX_SAND_IF_ERR_EXIT(arad_cnt_init(unit));

            if (init->stat_if.stat_if_enable)
            {
                if(SOC_IS_QAX(unit))
                {
                    SOCDNX_IF_ERR_EXIT(soc_qax_stat_if_init_set(unit, &(init->stat_if)));
                }
                else
                {
                    SOCDNX_IF_ERR_EXIT(soc_jer_stat_if_init_set(unit, &(init->stat_if)));
                }
                
            }

            
            if (soc_property_get(unit, spn_ECN_DM_ENABLE, 0)) {
                SOCDNX_IF_ERR_EXIT(arad_pp_oam_ecn_init(unit));
            }
            
            
            if (soc_property_get(unit, spn_PHY_1588_DPLL_FREQUENCY_LOCK, 0)) {

                SOCDNX_IF_ERR_EXIT(soc_jer_pp_oam_init_eci_tod(unit, 1, 0));
            }
        
            
            for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port_i, &is_valid));
                if (!is_valid) {
                    continue;
                }
                SOCDNX_IF_ERR_EXIT(soc_jer_port_timestamp_and_ssp_update(unit, port_i));
                SOCDNX_IF_ERR_EXIT(soc_jer_port_oam_ts_init(unit, port_i));
            }

            
            if (init->ports.is_stacking_system == 0x1) {
                LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "\t+ %d: Stacking Initialization\n"),unit));
                SOCDNX_IF_ERR_EXIT(arad_mgmt_stk_init(unit, init));
            }
        }else { 
            SOCDNX_IF_ERR_EXIT(soc_flair_fgl_regs_init(unit));
            SOCDNX_IF_ERR_EXIT(soc_flair_mct_tbls_init(unit));
            SOCDNX_IF_ERR_EXIT(soc_flair_port_map_tbls_init(unit));
            SOCDNX_IF_ERR_EXIT(soc_flair_tdm_tbls_init(unit));
            SOCDNX_IF_ERR_EXIT(soc_flair_fgl_regs_init(unit));
        }
    }
    else
    {
      
      
      slow_rate_max_level_str = soc_property_get_str(unit, spn_SLOW_MAX_RATE_LEVEL);
      if ( (slow_rate_max_level_str == NULL) || (sal_strcmp(slow_rate_max_level_str, "HIGH")==0) )
      {
          
          SOC_DPP_CONFIG(unit)->arad->init.max_burst_default_value_bucket_width = (0x7f)*256;
      } 
      else if (sal_strcmp(slow_rate_max_level_str, "NORMAL") == 0)
      {
          
          SOC_DPP_CONFIG(unit)->arad->init.max_burst_default_value_bucket_width = (0xff)*256;
      } 
      else if (sal_strcmp(slow_rate_max_level_str, "LOW") == 0)
      {
          
          SOC_DPP_CONFIG(unit)->arad->init.max_burst_default_value_bucket_width = (0x1ff)*256;
      }
      else
      {
          SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Unsupported properties: slow_rate_max_level should be LOW/NORMAL/HIGH")));
      }

      if (SOC_DPP_CONFIG(unit)->arad->init.drc_info.enable) {
          
          SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_dram_init(unit, &SOC_DPP_CONFIG(unit)->arad->init.drc_info));
      }
    }
#if defined(INCLUDE_KBP)
    arad_kbp_sw_init(unit);
#endif
    
    if (SOC_IS_QUX(unit) && !SOC_WARM_BOOT(unit))
    {
        soc_reg_above_64_val_t reg_val_64;    uint32 reg_val;
        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRCA_DPRC_ENABLERSr, REG_PORT_ANY, 0, &reg_val));   
        soc_reg_field_set(unit, DRCA_DPRC_ENABLERSr, &reg_val, MMU_RSTNf, 0x0);      
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRCA_DPRC_ENABLERSr, REG_PORT_ANY, 0, reg_val));
        
        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_val_64));       
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_9f, 1);   
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_val_64));      
        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRCA_DPRC_ENABLERSr, REG_PORT_ANY, 0, &reg_val));    
        soc_reg_field_set(unit, DRCA_DPRC_ENABLERSr, &reg_val, MMU_FAST_CLK_DIV_RSTNf, 0x0);       
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRCA_DPRC_ENABLERSr, REG_PORT_ANY, 0, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_val_64));     
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_1f, 1);    
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_2f, 1);     
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_10f, 1);     
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_11f, 1);      
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_19f, 1);     
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_20f, 1);     
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_3f, 1);      
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_4f, 1);      
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_22f, 1);      
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_23f, 1);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_24f, 1);   
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_25f, 1);   
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_43f, 1);    
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_44f, 1);    
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_45f, 1);    
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_27f, 1);     
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_28f, 1);     
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_29f, 1);     
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_15f, 1);      
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_16f, 1);    
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_17f, 1);    
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_18f, 1);     
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_26f, 1);     
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_val_64, BLOCKS_SOFT_INIT_47f, 1);     
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_val_64));
        
        sal_memset(&reg_val_64,0x0,sizeof(soc_reg_above_64_val_t));	
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_val_64));  
        sal_sleep(1);     
        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRCA_DPRC_ENABLERSr, REG_PORT_ANY, 0, &reg_val));    
        soc_reg_field_set(unit, DRCA_DPRC_ENABLERSr, &reg_val, MMU_FAST_CLK_DIV_RSTNf, 0x1);      
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRCA_DPRC_ENABLERSr, REG_PORT_ANY, 0, reg_val));   
        
        sal_sleep(1);       
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRCA_DPRC_ENABLERSr, REG_PORT_ANY, 0, &reg_val));
        soc_reg_field_set(unit, DRCA_DPRC_ENABLERSr, &reg_val, MMU_RSTNf, 0x1);   
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRCA_DPRC_ENABLERSr, REG_PORT_ANY, 0, reg_val));
        
        
        SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_BUFFERS_FREE_MIN_STATUSr(unit, SOC_CORE_ALL, &rval));
        SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_PDBS_FREE_MIN_STATUSr(unit, SOC_CORE_ALL, &rval));
        SOCDNX_IF_ERR_EXIT(READ_CGM_DRAM_BDBS_FREE_MIN_STATUSr(unit, SOC_CORE_ALL, &rval));			
        SOCDNX_IF_ERR_EXIT(READ_DDP_FBC_CACHE_STATUSr(unit, &r64));
        SOCDNX_IF_ERR_EXIT(READ_DQM_FREE_BDBLL_MIN_SIZEr(unit, &rval));
        SOCDNX_IF_ERR_EXIT(READ_ECGM_CGM_DYN_TH_DEBUGr(unit, SOC_CORE_ALL, reg_val_64));
        SOCDNX_IF_ERR_EXIT(READ_ILB_FPC_STATUSr(unit, 0, &r64));
        SOCDNX_IF_ERR_EXIT(READ_IRE_IHP_STATUS_REGISTERr(unit, &r64));
        SOCDNX_IF_ERR_EXIT(READ_IRE_PEF_STATUS_REGISTERr(unit, &r64));
        SOCDNX_IF_ERR_EXIT(READ_IRE_TAR_STATUS_REGISTERr(unit, &r64));
        SOCDNX_IF_ERR_EXIT(READ_SPB_PEC_STATUSr(unit, SOC_CORE_ALL, reg_val_64));
        SOCDNX_IF_ERR_EXIT(READ_SPB_FBC_STATUSr(unit, reg_val_64));
        SOCDNX_IF_ERR_EXIT(READ_SPB_FPC_STATUSr(unit, 0, &r64));
        SOCDNX_IF_ERR_EXIT(READ_SPB_FPC_STATUSr(unit, 1, &r64));
        SOCDNX_IF_ERR_EXIT(READ_SPB_PTC_STATUSr(unit, reg_val_64));
        SOCDNX_IF_ERR_EXIT(READ_TXQ_LTQ_STATUSr(unit, 0, &r64));
        SOCDNX_IF_ERR_EXIT(READ_TXQ_LTQ_STATUSr(unit, 1, &r64));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}



int
soc_jer_core_clock_verification(int unit)
{
     uint32 reg_val[1];
     uint32 cpu_override, core_clock_speed_SOC_KHZ, ideal_core_clock_value_khz;
     uint32 ndiv[1]={0}, mdiv[1]={0};
     uint32 deviation, min_permitted_range, max_permitted_range, core_clock_ratio;
     
     uint32 ndiv_offset=0, ndiv_num_of_bits=8, mdiv_offset=8, mdiv_num_of_bits=4;
     SOCDNX_INIT_FUNC_DEFS;
     
     if (! (SAL_BOOT_PLISIM || soc_sand_is_emulation_system(unit)))
     {

        if (SOC_IS_QUX(unit))
        {
            ndiv_offset=0, ndiv_num_of_bits=7, mdiv_offset=7, mdiv_num_of_bits=5;
        }
        else if (SOC_IS_QAX(unit))
        {
            ndiv_offset=0, ndiv_num_of_bits=8, mdiv_offset=8, mdiv_num_of_bits=5;
        }
        reg_val[0]=0;
        cpu_override=0;
        deviation = 1;
        min_permitted_range = 100 - deviation;
        max_permitted_range = 100 + deviation;

        
        SOCDNX_IF_ERR_EXIT(READ_ECI_OGER_1000r(unit, reg_val));
        
        cpu_override=soc_reg_field_get(unit, ECI_OGER_1000r, reg_val[0], FIELD_0_0f);

        if (!cpu_override) 
        {
            
            SOCDNX_IF_ERR_EXIT(READ_ECI_POWERUP_CONFIGr_REG32(unit, reg_val));
            SHR_BITCOPY_RANGE(ndiv, 0,reg_val, ndiv_offset, ndiv_num_of_bits);
            SHR_BITCOPY_RANGE(mdiv, 0,reg_val, mdiv_offset, mdiv_num_of_bits);
            core_clock_speed_SOC_KHZ = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency;
            if (mdiv[0] == 0 )
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                        (_BSL_SOCDNX_MSG(" mdiv equal zero , can't validate core clock settings")));
            }
            ideal_core_clock_value_khz = 1000 * ndiv[0] / mdiv[0] * JER_REF_CLOCK_VALUE ;
            core_clock_ratio = 100 * core_clock_speed_SOC_KHZ / ideal_core_clock_value_khz;
            if (core_clock_ratio < min_permitted_range || core_clock_ratio > max_permitted_range)
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                        (_BSL_SOCDNX_MSG(" core clock was set to be %d . core clock should be in the range of %d percentage from %d , ndiv is %d and mdiv is %d"),
                        core_clock_speed_SOC_KHZ, deviation, ideal_core_clock_value_khz, ndiv[0], mdiv[0]));
            }
        }
     }

exit:
    SOCDNX_FUNC_RETURN;
}




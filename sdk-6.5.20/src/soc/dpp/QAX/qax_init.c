/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_init.c
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
#include <sal/core/thread.h> 
#include <sal/core/libc.h> 


#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dpp/JER/jer_reset.h>
#include <soc/dpp/JER/jer_intr.h>
#include <soc/dpp/JER/JER_PP/jer_pp_oam.h>
#include <soc/dpp/QAX/qax_init.h>
#include <soc/dpp/QAX/qax_sram.h>
#include <soc/dpp/QAX/qax_mgmt.h>
#include <soc/dpp/QAX/qax_ipsec.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_lif.h>
#include <soc/dpp/QAX/qax_ingress_traffic_mgmt.h>


static soc_reg_t qax_blocks_with_standart_bubble_mechanism[] = {
    EGQ_INDIRECT_COMMANDr,        EGQ_INDIRECT_FORCE_BUBBLEr,

    
    INVALIDr
};



int soc_qax_init_bubble_config_standard_blocks (int unit)
{
    uint32 reg = 0;
    int iter = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bubble_cpu", 1) == 1) {
        
        while (qax_blocks_with_standart_bubble_mechanism[iter] != INVALIDr) {

            
            SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, qax_blocks_with_standart_bubble_mechanism[iter++], REG_PORT_ANY, INDIRECT_COMMAND_TIMEOUTf, 0x10));

            
            reg = 0;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, qax_blocks_with_standart_bubble_mechanism[iter], REG_PORT_ANY, 0, &reg));
            soc_reg_field_set (unit, qax_blocks_with_standart_bubble_mechanism[iter], &reg, FORCE_BUBBLE_PERIODf, 0x8);
            soc_reg_field_set (unit, qax_blocks_with_standart_bubble_mechanism[iter], &reg, FORCE_BUBBLE_ENf, 0x1);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, qax_blocks_with_standart_bubble_mechanism[iter++], REG_PORT_ANY, 0, reg));

        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}





#define QAX_MASK_NIF_OVERRIDES













int soc_qax_init_blocks_init_global_conf(int unit)
{
    uint32 reg32_val, field_val;
    ARAD_MGMT_INIT* init;

    SOCDNX_INIT_FUNC_DEFS;

    init = &SOC_DPP_CONFIG(unit)->arad->init;

    
    if (SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) {
        SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_SYS_HEADER_CFGr(unit, &reg32_val));
        soc_reg_field_set(unit, ECI_GLOBAL_SYS_HEADER_CFGr, &reg32_val, SYSTEM_HEADERS_MODEf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_SYS_HEADER_CFGr(unit, reg32_val));
    } else if (soc_property_get(unit, spn_SYSTEM_IS_ARAD_IN_SYSTEM, 0)) {
        SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_SYS_HEADER_CFGr(unit, &reg32_val));
        soc_reg_field_set(unit, ECI_GLOBAL_SYS_HEADER_CFGr, &reg32_val, SYSTEM_HEADERS_MODEf, 2);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_SYS_HEADER_CFGr(unit, reg32_val));
    }

    
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_SYS_HEADER_CFGr(unit, &reg32_val));
    field_val = SOC_DPP_CONFIG(unit)->arad->init.fabric.ftmh_lb_ext_mode == ARAD_MGMT_FTMH_LB_EXT_MODE_DISABLED ? 0 : 1;
    soc_reg_field_set(unit, ECI_GLOBAL_SYS_HEADER_CFGr, &reg32_val, FTMH_LB_KEY_EXT_ENf, field_val);
    field_val = init->fabric.ftmh_stacking_ext_mode == 0 ? 0 : 1;
    soc_reg_field_set(unit, ECI_GLOBAL_SYS_HEADER_CFGr, &reg32_val, FTMH_STACKING_EXT_ENABLEf, field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_SYS_HEADER_CFGr(unit, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg32_val));
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_FABRICf, 0);
    if (soc_feature(unit, soc_feature_packet_tdm_marking)) {
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_LOCALf, 0);
    } else {
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_LOCALf, SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP ? 1 : 0);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg32_val));

    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_1r(unit, &reg32_val));
      
    if (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_MESH || 
        
        ((SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP) &&
         !soc_feature(unit, soc_feature_packet_tdm_marking)))
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
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_3r, &reg32_val, TOD_MODEf, 3);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_3r(unit, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(READ_ECI_PAD_CONFIGURATION_REGISTERr(unit, &reg32_val));
    if (init->ex_vol_mod == ARAD_MGMT_EXT_VOL_MOD_HSTL_1p8V) {
        soc_reg_field_set(unit, ECI_PAD_CONFIGURATION_REGISTERr, &reg32_val, MODE_HV_2f, 0);
    } else if (init->ex_vol_mod == ARAD_MGMT_EXT_VOL_MOD_3p3V) {
        soc_reg_field_set(unit, ECI_PAD_CONFIGURATION_REGISTERr, &reg32_val, MODE_HV_2f, 1);
    } 
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_PAD_CONFIGURATION_REGISTERr(unit, reg32_val));

    if (!soc_feature(unit, soc_feature_no_fabric)) {
        
        SOCDNX_IF_ERR_EXIT(READ_TXQ_FDT_PRG_COUNTER_CFGr(unit, 0, &reg32_val));
        soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_MC_MASKf, 1);
        soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_MC_VALf, 0);
        soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_SRAM_MASKf, 0);
        soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_SRAM_VALf, 1);
        soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_CTXT_MASKf, 1);
        soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_CTXT_VALf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_TXQ_FDT_PRG_COUNTER_CFGr(unit, 0, reg32_val));

        SOCDNX_IF_ERR_EXIT(READ_TXQ_FDT_PRG_COUNTER_CFGr(unit, 1, &reg32_val));
        soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_MC_MASKf, 1);
        soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_MC_VALf, 0);
        soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_SRAM_MASKf, 0);
        soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_SRAM_VALf, 1);
        soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_CTXT_MASKf, 1);
        soc_reg_field_set(unit, TXQ_FDT_PRG_COUNTER_CFGr, &reg32_val, PRG_N_CNT_CFG_CTXT_VALf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_TXQ_FDT_PRG_COUNTER_CFGr(unit, 1, reg32_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_init_blocks_init_general_conf(int unit)
{
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_above_64_val_t reg_field_a64;
    uint32                 reg32_val = 0;
    uint32                 user_hdr_size = 0;
    uint32                 ite_termination_enable;
    int i;

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

    SOCDNX_IF_ERR_EXIT(WRITE_ECI_OGER_1008r_REG32(unit, 0x20101977));

#ifdef QAX_COMMENT_OUT_CODE_NOT_YET_IMPLEMENTED
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
#endif
    SOCDNX_IF_ERR_EXIT(READ_IPS_UPDATE_INDICATIONr(unit, &reg32_val));
    soc_reg_field_set(unit, IPS_UPDATE_INDICATIONr, &reg32_val, FORCE_NO_UPDATEf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_IPS_UPDATE_INDICATIONr(unit, reg32_val));

    
    
    user_hdr_size =
        (soc_property_port_get(unit, 0, spn_FIELD_CLASS_ID_SIZE, 0) +
         soc_property_port_get(unit, 1, spn_FIELD_CLASS_ID_SIZE, 0) +
         soc_property_port_get(unit, 2, spn_FIELD_CLASS_ID_SIZE, 0) +
         soc_property_port_get(unit, 3, spn_FIELD_CLASS_ID_SIZE, 0)) / 8;
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    soc_reg_above_64_field32_set(unit, ITE_ITPP_GENERAL_CFGr, reg_above_64_val, UDH_FIXED_LENGTHf, user_hdr_size);
    soc_reg_above_64_field32_set(unit, ITE_ITPP_GENERAL_CFGr, reg_above_64_val, ENABLE_EEI_STAMPINGf, 1);
    soc_reg_above_64_field32_set(unit, ITE_ITPP_GENERAL_CFGr, reg_above_64_val, ENABLE_EEI_ADDINGf, 1);
    soc_reg_above_64_field32_set(unit, ITE_ITPP_GENERAL_CFGr, reg_above_64_val, ALWAYS_CHECK_IP_COMPATIBLE_MCf, 1);
    soc_reg_above_64_field32_set(unit, ITE_ITPP_GENERAL_CFGr, reg_above_64_val, CUD_1_INVALID_VALUEf, 0x0);
    soc_reg_above_64_field32_set(unit, ITE_ITPP_GENERAL_CFGr, reg_above_64_val, ENABLE_TERMINATIONf, 1);
    soc_reg_above_64_field32_set(unit, ITE_ITPP_GENERAL_CFGr, reg_above_64_val, CUD_0_INVALID_VALUEf, 0x0);
    soc_reg_above_64_field32_set(unit, ITE_ITPP_GENERAL_CFGr, reg_above_64_val, ENABLE_FALLBACK_TO_BRIDGEf, 0);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_ITPP_GENERAL_CFGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOCDNX_IF_ERR_EXIT(WRITE_TAR_INVALID_CUDr(unit, 0));


    
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, REG_PORT_ANY, 0, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, reg_above_64_val, BACKWARD_MCID_ENf, 0);
    soc_reg_above_64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, reg_above_64_val, BACKWARD_IS_MC_ENf, 0);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, ITE_STAMPING_FTMH_OUTLIF_ENABLEr, REG_PORT_ANY, 0, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, ITE_STAMPING_FTMH_OUTLIF_ENABLEr, reg_above_64_val, STAMP_FTMH_OUTLIF_ENf, 0x33);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_STAMPING_FTMH_OUTLIF_ENABLEr, REG_PORT_ANY, 0, reg_above_64_val));

    
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val); 
    soc_reg_above_64_field32_set(unit, EGQ_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, reg_above_64_val, MAP_FLEXIBLE_UDH_0_TYPE_TO_SIZEf, 0x8d1);
    soc_reg_above_64_field32_set(unit, EGQ_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, reg_above_64_val, MAP_FLEXIBLE_UDH_3_TYPE_TO_SIZEf, 0x8d1);
    soc_reg_above_64_field32_set(unit, EGQ_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, reg_above_64_val, MAP_FLEXIBLE_UDH_1_TYPE_TO_SIZEf, 0x8d1);
    soc_reg_above_64_field32_set(unit, EGQ_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, reg_above_64_val, MAP_FLEXIBLE_UDH_2_TYPE_TO_SIZEf, 0x8d1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EGQ_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, REG_PORT_ANY, 0, reg_above_64_val));

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val); 
    soc_reg_above_64_field32_set(unit, EPNI_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, reg_above_64_val, MAP_FLEXIBLE_UDH_0_TYPE_TO_SIZEf, 0x8d1);
    soc_reg_above_64_field32_set(unit, EPNI_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, reg_above_64_val, MAP_FLEXIBLE_UDH_3_TYPE_TO_SIZEf, 0x8d1);
    soc_reg_above_64_field32_set(unit, EPNI_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, reg_above_64_val, MAP_FLEXIBLE_UDH_1_TYPE_TO_SIZEf, 0x8d1);
    soc_reg_above_64_field32_set(unit, EPNI_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, reg_above_64_val, MAP_FLEXIBLE_UDH_2_TYPE_TO_SIZEf, 0x8d1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPNI_MAP_FLEXIBLE_UDH_TYPE_TO_SIZEr, REG_PORT_ANY, 0, reg_above_64_val));

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val); 
    SOC_REG_ABOVE_64_CLEAR(reg_field_a64);
    reg_field_a64[0]=0xcc520c41; reg_field_a64[1]=0xd62d4941; reg_field_a64[2]=0x0c4183dc; reg_field_a64[3]=0x4941cc52; reg_field_a64[4]=0x83dcd62d; reg_field_a64[5]=0xcc520c41;reg_field_a64[6]=0xd62d4941; reg_field_a64[7]=0x0c4183dc; reg_field_a64[8]=0x4941cc52; reg_field_a64[9]=0x83dcd62d;
    soc_reg_above_64_field_set(unit, ITE_MAP_UDH_TYPE_TO_LENGTHr, reg_above_64_val, TYPE_TO_LENGTHf, reg_field_a64);
    for (i=0; i<4; i++)
    {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_MAP_UDH_TYPE_TO_LENGTHr, REG_PORT_ANY, i, reg_above_64_val));
    }

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val); 
    soc_reg_above_64_field32_set(unit, ITE_STAMPING_USR_DEF_OUTLIF_ENABLEr, reg_above_64_val, STAMP_USR_DEF_OUTLIF_TYPE_ENf, 0x7770);
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)) {
        
        soc_reg_above_64_field32_set(unit, ITE_STAMPING_USR_DEF_OUTLIF_ENABLEr, reg_above_64_val, STAMP_USR_DEF_OUTLIF_TYPE_ENf, 0x7740);
    }
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_STAMPING_USR_DEF_OUTLIF_ENABLEr, REG_PORT_ANY, 0, reg_above_64_val));

    
    
    ite_termination_enable = soc_property_get(unit, spn_ITPP_NETWORK_HEADERS_TERMINATION, 0);
    if (ite_termination_enable != 0) {
        if (user_hdr_size != 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("ITE termination can't support if UDH size > 0")));
        } else {
            SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
            soc_reg_above_64_field32_set(unit, ITE_PPH_RES_EN_TERMr, reg_above_64_val, PPH_RES_EN_TERM_MASKf, 0);
            soc_reg_above_64_field32_set(unit, ITE_PPH_RES_EN_TERMr, reg_above_64_val, PPH_RES_EN_TERMf, 0);
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_PPH_RES_EN_TERMr, REG_PORT_ANY, 0, reg_above_64_val));
            
            SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
            soc_reg_above_64_field32_set(unit, ITE_MAPPING_FWD_CODE_TO_TERM_ENr, reg_above_64_val, EN_TERM_UP_TO_ETHf, 0x1E);
            soc_reg_above_64_field32_set(unit, ITE_MAPPING_FWD_CODE_TO_TERM_ENr, reg_above_64_val, EN_TERM_UP_TO_FWD_CODEf, 0x2B);
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_MAPPING_FWD_CODE_TO_TERM_ENr, REG_PORT_ANY, 0, reg_above_64_val));
        }
    } else {
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        soc_reg_above_64_field32_set(unit, ITE_PPH_RES_EN_TERMr, reg_above_64_val, PPH_RES_EN_TERM_MASKf, 3);
        soc_reg_above_64_field32_set(unit, ITE_PPH_RES_EN_TERMr, reg_above_64_val, PPH_RES_EN_TERMf, 3);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_PPH_RES_EN_TERMr, REG_PORT_ANY, 0, reg_above_64_val));

        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        soc_reg_above_64_field32_set(unit, ITE_MAPPING_FWD_CODE_TO_TERM_ENr, reg_above_64_val, EN_TERM_UP_TO_ETHf, 0x0);
        soc_reg_above_64_field32_set(unit, ITE_MAPPING_FWD_CODE_TO_TERM_ENr, reg_above_64_val, EN_TERM_UP_TO_FWD_CODEf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ITE_MAPPING_FWD_CODE_TO_TERM_ENr, REG_PORT_ANY, 0, reg_above_64_val));
    }

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val); 
    soc_reg_above_64_field32_set(unit, EPNI_CFG_LINK_FIFOS_FIXED_LATENCY_SETTINGr, reg_above_64_val, CFG_LINK_P_16_FIXED_LATENCY_SETTINGf, 0xa);
    soc_reg_above_64_field32_set(unit, EPNI_CFG_LINK_FIFOS_FIXED_LATENCY_SETTINGr, reg_above_64_val, CFG_NATIVE_LINK_P_9_FIXED_LATENCY_SETTINGf, 9);
    soc_reg_above_64_field32_set(unit, EPNI_CFG_LINK_FIFOS_FIXED_LATENCY_SETTINGr, reg_above_64_val, CFG_NATIVE_LINK_P_16_FIXED_LATENCY_SETTINGf, 6);
    soc_reg_above_64_field32_set(unit, EPNI_CFG_LINK_FIFOS_FIXED_LATENCY_SETTINGr, reg_above_64_val, CFG_LINK_P_9_FIXED_LATENCY_SETTINGf, 9);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPNI_CFG_LINK_FIFOS_FIXED_LATENCY_SETTINGr, REG_PORT_ANY, 0, reg_above_64_val));

    
    SOCDNX_IF_ERR_EXIT(qax_pp_lif_default_native_ac_outlif_init(unit)); 

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_nbil_phy_led_pmlq_bypass_enable(int unit, int quad, int enable) {

    int  reg_port, pm_index = 0;
    uint32 nof_pms_per_nbi    = SOC_DPP_DEFS_GET(unit, nof_pms_per_nbi);
    uint32 nof_instances_nbil = SOC_DPP_DEFS_GET(unit, nof_instances_nbil);
    uint32 reg_val;
    uint32 nbi_inst;
    SOCDNX_INIT_FUNC_DEFS;

    nbi_inst = quad / nof_pms_per_nbi; 
    pm_index = quad % nof_pms_per_nbi;
    reg_port = nbi_inst - 1;

    if ((reg_port < nof_instances_nbil) && (pm_index < (nof_pms_per_nbi - 1 ))) {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_PHY_LED_PMLQ_BYPASSr(unit, reg_port, pm_index, &reg_val));
        soc_reg_field_set(unit, NBIL_PHY_LED_PMLQ_BYPASSr, &reg_val, PHY_LED_PMLQ_N_BYPASS_MUX_SELECTf, (enable != 0));
        soc_reg_field_set(unit, NBIL_PHY_LED_PMLQ_BYPASSr, &reg_val, PHY_LED_PMLQ_N_OUTPUT_DELAYf, (enable == 0));
        soc_reg_field_set(unit, NBIL_PHY_LED_PMLQ_BYPASSr, &reg_val, PHY_LED_PMLQ_N_BYPASS_RESETf, (enable != 0));
        soc_reg_field_set(unit, NBIL_PHY_LED_PMLQ_BYPASSr, &reg_val, PHY_LED_PMLQ_N_ENABLEf, (enable != 0));
        soc_reg_field_set(unit, NBIL_PHY_LED_PMLQ_BYPASSr, &reg_val, PHY_LED_PMLQ_N_POWER_SAVING_MODEf, (enable == 0));
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PHY_LED_PMLQ_BYPASSr(unit, reg_port, pm_index, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int soc_qax_led_tune(int unit)
{
    uint32 rval = 0;
    soc_pbmp_t pbmp_qsgmii;
    soc_port_t port;
    int quad, phy_port;
    uint32 physical_phy;
    soc_error_t ret;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QAX(unit) && !SOC_IS_QUX(unit)) {
        soc_reg_field_set(unit, NBIL_PM_STRAP_LED_OUTPUT_DELAYr, &rval, PML_0_STRAP_LED_OUTPUT_DELAYf, 0);
        soc_reg_field_set(unit, NBIL_PM_STRAP_LED_OUTPUT_DELAYr, &rval, PML_1_STRAP_LED_OUTPUT_DELAYf, 0);
        soc_reg_field_set(unit, NBIL_PM_STRAP_LED_OUTPUT_DELAYr, &rval, PML_2_STRAP_LED_OUTPUT_DELAYf, 0);
        soc_reg_field_set(unit, NBIL_PM_STRAP_LED_OUTPUT_DELAYr, &rval, PML_3_STRAP_LED_OUTPUT_DELAYf, 1);

        
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PM_STRAP_LED_OUTPUT_DELAYr(unit, 0 | SOC_REG_ADDR_INSTANCE_MASK, rval));
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PM_STRAP_LED_OUTPUT_DELAYr(unit, 1 | SOC_REG_ADDR_INSTANCE_MASK, rval));

        SOC_PBMP_CLEAR(pbmp_qsgmii);
        SOC_PBMP_ASSIGN(pbmp_qsgmii, PBMP_QSGMII_ALL(unit));
        SOC_PBMP_ITER(pbmp_qsgmii, port) {
            if (soc_feature(unit, soc_feature_logical_port_num)) {
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
            } else {
                phy_port = port;
            }
            ret = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove,
            (unit, phy_port , &physical_phy));
            if (ret != SOC_E_NONE) {
                continue;
            }
            --physical_phy;
            quad = physical_phy / NUM_OF_LANES_IN_PM; 
            SOCDNX_IF_ERR_EXIT(soc_qax_nbil_phy_led_pmlq_bypass_enable(unit, quad, 1));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_init_blocks_init_conf(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_qax_init_blocks_init_global_conf(unit));

    
    SOCDNX_IF_ERR_EXIT(soc_qax_init_blocks_init_general_conf(unit));

    

    
    SOCDNX_IF_ERR_EXIT( soc_qax_sram_conf_set(unit));

    
    SOCDNX_IF_ERR_EXIT(soc_qax_dram_buffer_conf_set(unit));


    
    SOCDNX_IF_ERR_EXIT(soc_qax_ipsec_init(unit));

    
    SOCDNX_IF_ERR_EXIT(soc_qax_led_tune(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_init_blocks_general(int unit)
{
    soc_reg_above_64_val_t reg_above_64_val;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_qax_init_bubble_config_standard_blocks(unit));

    
    SOCDNX_IF_ERR_EXIT(qax_mgmt_revision_fixes(unit));

    
    SOCDNX_IF_ERR_EXIT(READ_PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr(unit, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, reg_above_64_val, LARGE_EM_CFG_LIMIT_MODE_FIDf, 0x1);   
    SOCDNX_IF_ERR_EXIT(WRITE_PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr(unit, reg_above_64_val));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_qax_init_dpp_defs_overrides(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;
    
    if (SOC_IS_QUX(unit)) {
        SOC_DPP_DEFS_SET(unit, nof_pm4x25, 0);
        SOC_DPP_DEFS_SET(unit, nof_pm4x10, 2);
        SOC_DPP_DEFS_SET(unit, nof_pm4x10q, 0);
        SOC_DPP_DEFS_SET(unit, pmh_base_lane, 0);
        SOC_DPP_DEFS_SET(unit, pml0_base_lane, 0);
        SOC_DPP_DEFS_SET(unit, nof_pms_per_nbi, 9);
        SOC_DPP_DEFS_SET(unit, nof_instances_nbil, 1);
        SOC_DPP_DEFS_SET(unit, nof_lanes_per_nbi, 36);
        SOC_DPP_DEFS_SET(unit, nof_ports_nbih, 0);
        SOC_DPP_DEFS_SET(unit, nof_ports_nbil, 36);
    }
    else {
        SOC_DPP_DEFS_SET(unit, nof_pm4x25, 4);
        SOC_DPP_DEFS_SET(unit, nof_pm4x10, 4);
        SOC_DPP_DEFS_SET(unit, nof_pm4x10q, 3);
        SOC_DPP_DEFS_SET(unit, pmh_base_lane, 0);
        SOC_DPP_DEFS_SET(unit, pml0_base_lane, 16);
        SOC_DPP_DEFS_SET(unit, nof_pms_per_nbi, 4);
        SOC_DPP_DEFS_SET(unit, nof_instances_nbil, 1);
        SOC_DPP_DEFS_SET(unit, nof_lanes_per_nbi, 16);
        SOC_DPP_DEFS_SET(unit, nof_ports_nbih, 16);
        SOC_DPP_DEFS_SET(unit, nof_ports_nbil, 52);
    }

    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_qax_dqcf_contexts_init(int unit)
{
    soc_reg_above_64_val_t above64;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_REG_ABOVE_64_CLEAR(above64);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_MC_THr, above64, PDQ_MC_GFMC_DRAM_THf, 0x6c);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_MC_THr, above64, PDQ_MC_BFMC_DRAM_THf, 0x6c);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_MC_THr, above64, PDQ_MC_GFMC_SRAM_THf, 0x17f);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_MC_THr, above64, PDQ_MC_BFMC_SRAM_THf, 0x17f);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, PTS_PDQ_MC_THr, REG_PORT_ANY, 0, above64));

    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, PTS_PDQ_OVTH_FC_MASKr, REG_PORT_ANY, 0, above64));

    
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_LOC_HP_OVTH_MASKf, 0x4);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, DRAM_LOC_HP_OVTH_MASKf, 0x8);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_LOC_LP_OVTH_MASKf, 0x5);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, DRAM_LOC_LP_OVTH_MASKf, 0xa);

    
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_TO_DRAM_HP_OVTH_MASKf, 0x1);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_TO_DRAM_LP_OVTH_MASKf, 0x1);

    
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_FUC_HP_OVTH_MASKf, 0x40);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, DRAM_FUC_HP_OVTH_MASKf, 0x80);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_FUC_LP_OVTH_MASKf, 0x41);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, DRAM_FUC_LP_OVTH_MASKf, 0x82);

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, PTS_PDQ_OVTH_FC_MASKr, REG_PORT_ANY, 0, above64));

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_qax_mc_dqcf_flow_control_recieve_mask_init(int unit, int sram_fmc_hp_mask, int dram_fmc_hp_mask, int sram_fmc_lp_mask, int dram_fmc_lp_mask)
{
    soc_reg_above_64_val_t above64;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, PTS_PDQ_OVTH_FC_MASKr, REG_PORT_ANY, 0, above64));
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_FMC_HP_OVTH_MASKf, sram_fmc_hp_mask);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, DRAM_FMC_HP_OVTH_MASKf, dram_fmc_hp_mask);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, SRAM_FMC_LP_OVTH_MASKf, sram_fmc_lp_mask);
    soc_reg_above_64_field32_set(unit, PTS_PDQ_OVTH_FC_MASKr, above64, DRAM_FMC_LP_OVTH_MASKf, dram_fmc_lp_mask);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, PTS_PDQ_OVTH_FC_MASKr, REG_PORT_ANY, 0, above64));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_qax_dtq_contexts_init(int unit)
{
    uint32 reg32_val;
    soc_reg_above_64_val_t above64;
    soc_dcmn_fabric_pipe_map_type_t fabric_pipe_map_type;
    soc_fabric_dtq_mode_type_t dtq_mode = SOC_FABRIC_DTQ_MODE_UC_HMC_LMC;  
    SOCDNX_INIT_FUNC_DEFS;

    fabric_pipe_map_type = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pipe_map_config.mapping_type;

    if (SOC_DPP_IS_MESH(unit)){
        dtq_mode = SOC_FABRIC_DTQ_MODE_UC_HMC_LMC; 
        SOCDNX_IF_ERR_EXIT(soc_qax_mc_dqcf_flow_control_recieve_mask_init(unit, 0x400, 0x800, 0x410, 0x820));

    } else if (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_FE ||
               soc_feature(unit, soc_feature_packet_tdm_marking)) {

        switch (fabric_pipe_map_type) { 
        case soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc:
            dtq_mode = SOC_FABRIC_DTQ_MODE_UC_HMC_LMC;
            SOCDNX_IF_ERR_EXIT(soc_qax_mc_dqcf_flow_control_recieve_mask_init(unit, 0x100, 0x200, 0x10, 0x20));
            break;

        case soc_dcmn_fabric_pipe_map_triple_uc_mc_tdm:
        case soc_dcmn_fabric_pipe_map_dual_uc_mc:
            dtq_mode = SOC_FABRIC_DTQ_MODE_UC_MC;
            SOCDNX_IF_ERR_EXIT(soc_qax_mc_dqcf_flow_control_recieve_mask_init(unit, 0x100, 0x200, 0x104, 0x208));
            break;

        case soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm:
        case soc_dcmn_fabric_pipe_map_single:
            dtq_mode = SOC_FABRIC_DTQ_MODE_SINGLE_QUEUE;
            SOCDNX_IF_ERR_EXIT(soc_qax_mc_dqcf_flow_control_recieve_mask_init(unit, 0x40, 0x80, 0x41, 0x82));
            break;

        case soc_dcmn_fabric_pipe_map_triple_custom:
        case soc_dcmn_fabric_pipe_map_dual_custom:
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOC_MSG("custom mode is not supported")));
            break;
        }
    }

    if (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode != ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP
            || soc_feature(unit, soc_feature_packet_tdm_marking)) {
        SOCDNX_IF_ERR_EXIT(READ_TXQ_TXQ_GENERAL_CONFIGURATIONr(unit, &reg32_val));
        
        soc_reg_field_set(unit, TXQ_TXQ_GENERAL_CONFIGURATIONr, &reg32_val, DTQ_MODEf, dtq_mode);
        
        soc_reg_field_set(unit, TXQ_TXQ_GENERAL_CONFIGURATIONr, &reg32_val, Q_0_Q_12_INTERLEAVE_ENf, 1);
        soc_reg_field_set(unit, TXQ_TXQ_GENERAL_CONFIGURATIONr, &reg32_val, Q_1_Q_2_INTERLEAVE_ENf, 1);
        
        soc_reg_field_set(unit, TXQ_TXQ_GENERAL_CONFIGURATIONr, &reg32_val, DTQ_DEQUEUE_WHEN_EOB_ON_QUEUEf, 42); 
        soc_reg_field_set(unit, TXQ_TXQ_GENERAL_CONFIGURATIONr, &reg32_val, DTQ_IS_MC_BMAPf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_TXQ_TXQ_GENERAL_CONFIGURATIONr(unit, reg32_val));
    }

    SOC_REG_ABOVE_64_CLEAR(above64); 
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, SRAM_DTQ_LOC_SPR_FC_THf, 64);
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, SRAM_DTQ_LOC_PDQ_FC_TH_1f, 48);
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, SRAM_DTQ_LOC_PDQ_FC_TH_2f, 64);

    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, DRAM_DTQ_LOC_SPR_FC_THf, 102);
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, DRAM_DTQ_LOC_PDQ_FC_TH_1f, 76);
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, DRAM_DTQ_LOC_PDQ_FC_TH_2f, 102);

    
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, SRAM_DTQ_LOC_EGQ_FC_THf, 72);
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, SRAM_DTQ_LOC_GEN_RCI_THf, 72);

    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, DRAM_DTQ_LOC_EGQ_FC_THf, 115);
    soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, above64, DRAM_DTQ_LOC_GEN_RCI_THf, 115);

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, TXQ_LOCAL_FIFO_CFGr, REG_PORT_ANY, 0, above64));

exit:
   SOCDNX_FUNC_RETURN;
}

STATIC int
soc_qax_pts_shapers_init(int unit)
{
    uint32 reg32_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_PTS_SHAPER_GENERAL_CONFIGSr(unit, &reg32_val));
    soc_reg_field_set(unit, PTS_SHAPER_GENERAL_CONFIGSr, &reg32_val, ZERO_ACT_LINKS_RATE_ENf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_PTS_SHAPER_GENERAL_CONFIGSr(unit, reg32_val));

exit:
   SOCDNX_FUNC_RETURN;
}

uint32
soc_qax_pdq_dtq_contexts_init(
  SOC_SAND_IN int     unit)
{
    uint32 reg32;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_PTS_PTS_DEBUG_CONTROLSr(unit, &reg32));
    soc_reg_field_set(unit, PTS_PTS_DEBUG_CONTROLSr, &reg32, GCI_ENf, 0x1);
    SOCDNX_IF_ERR_EXIT(WRITE_PTS_PTS_DEBUG_CONTROLSr(unit, reg32));

    
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_contexts_init(unit));

    
    SOCDNX_IF_ERR_EXIT(soc_qax_dqcf_contexts_init(unit));

    if (SOC_IS_QAX_WITH_FABRIC_ENABLED(unit)) {
        
        SOCDNX_IF_ERR_EXIT(READ_PTS_PACKET_DEQUEUE_GENERAL_CONFIGSr(unit, &reg32));
        soc_reg_field_set(unit, PTS_PACKET_DEQUEUE_GENERAL_CONFIGSr, &reg32, FAB_MEM_INTERLEAVE_ENf, 0xf);
        SOCDNX_IF_ERR_EXIT(WRITE_PTS_PACKET_DEQUEUE_GENERAL_CONFIGSr(unit, reg32));
    }

    
    SOCDNX_IF_ERR_EXIT(qax_itm_ingress_latency_init(unit));

    
    SOCDNX_IF_ERR_EXIT(soc_qax_pts_shapers_init(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

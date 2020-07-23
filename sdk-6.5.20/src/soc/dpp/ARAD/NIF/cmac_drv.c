/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC CMAC DRV
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <shared/bitop.h>
#include <soc/cm.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/NIF/common_drv.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/drv.h>
#include <soc/mcm/memregs.h>
#include <soc/cmic.h>
#include <soc/drv.h>
#include <soc/dpp/drv.h>

#define SOC_CMAC_DRV_SPEED_10000  0x4


soc_error_t 
soc_cmac_drv_init(int unit, soc_port_t port)
{
    uint32 reg_val32[1], is_hg, nof_lanes, fld_val[1], phy_port, cmac_id, runt_pad, offset;
    uint64 reg_val64;
    soc_reg_above_64_val_t reg_val_a64, field_val_a64;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));
    if (phy_port == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("CMAC doesn't support CPU port")));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_common_drv_bypass_mode_set(unit,port,1));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_hg_get(unit, port, &is_hg));

    
    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_RX_CTRLr(unit, port, reg_val32));
    soc_reg_field_set(unit, CLP_CMAC_RX_CTRLr, reg_val32, STRIP_CRCf, 1);
    soc_reg_field_set(unit, CLP_CMAC_RX_CTRLr, reg_val32, STRICT_PREAMBLEf, is_hg ? 0 : 1);
    soc_reg_field_set(unit, CLP_CMAC_RX_CTRLr, reg_val32, RUNT_THRESHOLDf, 64);
    SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_RX_CTRLr(unit, port, *reg_val32));

    
    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_TX_CTRLr(unit, port, &reg_val64));
    soc_reg64_field32_set(unit, CLP_CMAC_TX_CTRLr, &reg_val64, CRC_MODEf, 0);
    soc_reg64_field32_set(unit, CLP_CMAC_TX_CTRLr, &reg_val64, AVERAGE_IPGf, is_hg ? NIF_AVERAGE_IPG_HIGIG : NIF_AVERAGE_IPG_DEFAULT);
    soc_reg64_field32_set(unit, CLP_CMAC_TX_CTRLr, &reg_val64, TX_PREAMBLE_LENGTHf, NIF_TX_PREAMBLE_LENGTH_DEFAULT);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_runt_pad_get(unit, port, &runt_pad));
    soc_reg64_field32_set(unit, CLP_CMAC_TX_CTRLr, &reg_val64, PAD_ENf, runt_pad ? 1 : 0);
    if(runt_pad) {
        soc_reg64_field32_set(unit, CLP_CMAC_TX_CTRLr, &reg_val64, PAD_THRESHOLDf, runt_pad);
    }

    SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_TX_CTRLr(unit, port, reg_val64));

    
    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_PAUSE_CTRLr(unit, port, &reg_val64));
    soc_reg64_field32_set(unit, CLP_CMAC_PAUSE_CTRLr, &reg_val64, RX_PAUSE_ENf, 1);
    soc_reg64_field32_set(unit, CLP_CMAC_PAUSE_CTRLr, &reg_val64, TX_PAUSE_ENf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_PAUSE_CTRLr(unit, port, reg_val64));

    
    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_PFC_CTRLr(unit, port, &reg_val64));
    soc_reg64_field32_set(unit, CLP_CMAC_PFC_CTRLr, &reg_val64, RX_PFC_ENf, 1);
    soc_reg64_field32_set(unit, CLP_CMAC_PFC_CTRLr, &reg_val64, TX_PFC_ENf, 0);
    soc_reg64_field32_set(unit, CLP_CMAC_PFC_CTRLr, &reg_val64, PFC_STATS_ENf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_PFC_CTRLr(unit, port, reg_val64));

    SOCDNX_IF_ERR_EXIT(READ_PORT_CONFIGr(unit, port, reg_val32));
    soc_reg_field_set(unit, PORT_CONFIGr, reg_val32, PFC_ENABLEf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_PORT_CONFIGr(unit, port, *reg_val32));

    
    SOCDNX_IF_ERR_EXIT(READ_NBI_FC_LLFC_STOP_TX_FROM_CFC_MASKr(unit, reg_val32));
    SHR_BITCLR(reg_val32, (phy_port-1));
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_FC_LLFC_STOP_TX_FROM_CFC_MASKr(unit, *reg_val32));

    SOC_REG_ABOVE_64_CLEAR(field_val_a64);
    SOCDNX_IF_ERR_EXIT(READ_NBI_FC_PFC_RX_BITMAP_TYPE_Ar(unit, reg_val_a64));
    soc_reg_above_64_field_set(unit, NBI_FC_PFC_RX_BITMAP_TYPE_Ar, reg_val_a64, FC_LLFC_STOP_TX_FROM_PFC_TX_BITMAP_TYPE_Af, field_val_a64);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_FC_PFC_RX_BITMAP_TYPE_Ar(unit, reg_val_a64));

    SOCDNX_IF_ERR_EXIT(READ_NBI_FC_PFC_RX_BITMAP_TYPE_Br(unit, reg_val_a64));
    soc_reg_above_64_field_set(unit, NBI_FC_PFC_RX_BITMAP_TYPE_Br, reg_val_a64, FC_LLFC_STOP_TX_FROM_PFC_TX_BITMAP_TYPE_Bf, field_val_a64);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_FC_PFC_RX_BITMAP_TYPE_Br(unit, reg_val_a64));

    
    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_LLFC_CTRLr(unit, port, reg_val32));
    soc_reg_field_set(unit, CLP_CMAC_LLFC_CTRLr, reg_val32, RX_LLFC_ENf, 0);
    soc_reg_field_set(unit, CLP_CMAC_LLFC_CTRLr, reg_val32, TX_LLFC_ENf, 0);
    soc_reg_field_set(unit, CLP_CMAC_LLFC_CTRLr, reg_val32, LLFC_IN_IPG_ONLYf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_LLFC_CTRLr(unit, port, *reg_val32));

    SOCDNX_IF_ERR_EXIT(READ_PORT_CONFIGr(unit, port, reg_val32));
    soc_reg_field_set(unit, PORT_CONFIGr, reg_val32, LLFC_ENf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_PORT_CONFIGr(unit, port, *reg_val32));

    
    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_RX_LSS_CTRLr(unit, port, reg_val32));
    soc_reg_field_set(unit, CLP_CMAC_RX_LSS_CTRLr, reg_val32, DROP_TX_DATA_ON_LOCAL_FAULTf, 1);
    soc_reg_field_set(unit, CLP_CMAC_RX_LSS_CTRLr, reg_val32, DROP_TX_DATA_ON_REMOTE_FAULTf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_RX_LSS_CTRLr(unit, port, *reg_val32));

    
    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_RX_MAX_SIZEr(unit, port, reg_val32));
    soc_reg_field_set(unit, CLP_CMAC_RX_MAX_SIZEr, reg_val32, RX_MAX_SIZEf, NIF_PACKET_MAX_SIZE);
    SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_RX_MAX_SIZEr(unit, port, *reg_val32));

    SOCDNX_IF_ERR_EXIT(READ_PORT_CNTMAXSIZEr(unit, port, reg_val32));
    soc_reg_field_set(unit, PORT_CNTMAXSIZEr, reg_val32, CNTMAXSIZEf, soc_property_get(unit, spn_BCM_STAT_JUMBO, 1518));
    SOCDNX_IF_ERR_EXIT(WRITE_PORT_CNTMAXSIZEr(unit, port, *reg_val32));

    
    
    SOCDNX_IF_ERR_EXIT(WRITE_MAC_RSV_MASKr(unit, port, 0x20058));

    
    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_MODEr(unit, port, reg_val32));
    soc_reg_field_set(unit, CLP_CMAC_MODEr, reg_val32, SPEED_MODEf, SOC_CMAC_DRV_SPEED_10000);
    SOCDNX_IF_ERR_EXIT(soc_common_drv_mac_mode_reg_hdr_set(unit, port, CLP_CMAC_MODEr, reg_val32));
    soc_reg_field_set(unit, CLP_CMAC_MODEr, reg_val32, NO_SOP_FOR_CRC_HGf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_MODEr(unit, port, *reg_val32));

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &nof_lanes));
    SOCDNX_IF_ERR_EXIT(READ_CLP_PORT_CMAC_MODEr(unit, port, reg_val32));
    soc_reg_field_set(unit, CLP_PORT_CMAC_MODEr, reg_val32, CGMII_MODEf, (12 == nof_lanes) ? 1 : 0);
    SOCDNX_IF_ERR_EXIT(WRITE_CLP_PORT_CMAC_MODEr(unit, port, *reg_val32));

    
    SOCDNX_IF_ERR_EXIT(soc_common_drv_fast_port_set(unit, port, 0, 1));

    
    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_CTRLr(unit, port, reg_val32));
    soc_reg_field_set(unit, CLP_CMAC_CTRLr, reg_val32, TX_ENf, 1);
    soc_reg_field_set(unit, CLP_CMAC_CTRLr, reg_val32, RX_ENf, 1);
    soc_reg_field_set(unit, CLP_CMAC_CTRLr, reg_val32, SOFT_RESETf, 0);
    soc_reg_field_set(unit, CLP_CMAC_CTRLr, reg_val32, XLGMII_ALIGN_ENBf, is_hg ? 0 : 1);
    soc_reg_field_set(unit, CLP_CMAC_CTRLr, reg_val32, XGMII_IPG_CHECK_DISABLEf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_CTRLr(unit, port, *reg_val32));

    
    cmac_id = (1 == phy_port) ? 0 : 1;
    SOCDNX_IF_ERR_EXIT(READ_NBI_ENABLE_INTERLAKENr(unit, reg_val32));
    soc_reg_field_set(unit, NBI_ENABLE_INTERLAKENr, reg_val32, (0 == cmac_id) ? ENABLE_HRF_0f : ENABLE_HRF_1f, 1);
    soc_reg_field_set(unit, NBI_ENABLE_INTERLAKENr, reg_val32, (0 == cmac_id) ? USE_HRF_FOR_ILKN_0f : USE_HRF_FOR_ILKN_1f, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_ENABLE_INTERLAKENr(unit, *reg_val32));

    
    SOCDNX_IF_ERR_EXIT(READ_NBI_ILKN_RESETr(unit, reg_val32));
    soc_reg_field_set(unit, NBI_ILKN_RESETr, reg_val32, (0 == cmac_id) ? HRF_RX_0_CONTROLLER_RSTNf : HRF_RX_1_CONTROLLER_RSTNf, 0);
    soc_reg_field_set(unit, NBI_ILKN_RESETr, reg_val32, (0 == cmac_id) ? HRF_TX_0_CONTROLLER_RSTNf : HRF_TX_1_CONTROLLER_RSTNf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_ILKN_RESETr(unit, *reg_val32));
    soc_reg_field_set(unit, NBI_ILKN_RESETr, reg_val32, (0 == cmac_id) ? HRF_RX_0_CONTROLLER_RSTNf : HRF_RX_1_CONTROLLER_RSTNf, 1);
    soc_reg_field_set(unit, NBI_ILKN_RESETr, reg_val32, (0 == cmac_id) ? HRF_TX_0_CONTROLLER_RSTNf : HRF_TX_1_CONTROLLER_RSTNf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_ILKN_RESETr(unit, *reg_val32));

    
    SOCDNX_IF_ERR_EXIT(READ_NBI_FC_ILKNr(unit, reg_val32));
    *fld_val = soc_reg_field_get(unit, NBI_FC_ILKNr, *reg_val32, FC_CMAL_MODEf);
    *fld_val |= (1<<cmac_id);
    soc_reg_field_set(unit, NBI_FC_ILKNr, reg_val32, FC_CMAL_MODEf, *fld_val);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_FC_ILKNr(unit, *reg_val32));

    
    SOCDNX_IF_ERR_EXIT(READ_NBI_RX_MLF_RESET_PORTSr(unit, reg_val32));
    *fld_val = soc_reg_field_get(unit, NBI_RX_MLF_RESET_PORTSr, *reg_val32, RX_PORTS_SRSTNf);
    SHR_BITCLR(fld_val, (phy_port-1));
    soc_reg_field_set(unit, NBI_RX_MLF_RESET_PORTSr, reg_val32, RX_PORTS_SRSTNf, *fld_val);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_RX_MLF_RESET_PORTSr(unit, *reg_val32));

    SOCDNX_IF_ERR_EXIT(READ_NBI_TX_MLF_RESET_PORTSr(unit, reg_val32));
    *fld_val = soc_reg_field_get(unit, NBI_TX_MLF_RESET_PORTSr, *reg_val32, TX_PORTS_SRSTNf);
    SHR_BITCLR(fld_val, (phy_port-1));
    soc_reg_field_set(unit, NBI_TX_MLF_RESET_PORTSr, reg_val32, TX_PORTS_SRSTNf, *fld_val);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_TX_MLF_RESET_PORTSr(unit, *reg_val32));

    
    SOCDNX_IF_ERR_EXIT(READ_PORT_FAULT_LINK_STATUSr(unit, port, reg_val32));
    soc_reg_field_set(unit, PORT_FAULT_LINK_STATUSr, reg_val32, LOCAL_FAULTf , 1);
    soc_reg_field_set(unit, PORT_FAULT_LINK_STATUSr, reg_val32, REMOTE_FAULTf , 1);
    SOCDNX_IF_ERR_EXIT(WRITE_PORT_FAULT_LINK_STATUSr(unit, port, *reg_val32));

    
    SOCDNX_IF_ERR_EXIT(READ_PORT_ENABLE_REGr(unit, port, reg_val32));
    soc_reg_field_set(unit, PORT_ENABLE_REGr, reg_val32, PORT_0f , 1);
    SOCDNX_IF_ERR_EXIT(WRITE_PORT_ENABLE_REGr(unit, port, *reg_val32));

    
    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_TX_CTRLr(unit, port, &reg_val64));
    soc_reg64_field32_set(unit, CLP_CMAC_TX_CTRLr, &reg_val64, TX_256_BYTE_BUFFER_ENf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_TX_CTRLr(unit, port, reg_val64));

    
    *reg_val32 = 1;
    SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, port, *reg_val32));
    *reg_val32 = 0;
    SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, port, *reg_val32));

    
    
#ifdef BCM_88660_A0
    if(SOC_IS_ARADPLUS(unit)){
        int field_id;
        ARAD_PORTS_CAUI_CONFIG *caui_config;
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
        caui_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.caui[offset];
        SOCDNX_IF_ERR_EXIT(READ_NBI_CHOOSE_RX_WCLK_33_LANE_FOR_MLDr(unit, reg_val32));
        field_id = (cmac_id == 0)? CHOOSE_RX_WCLK_33_LANE_FOR_MLD_0f : CHOOSE_RX_WCLK_33_LANE_FOR_MLD_1f;
        soc_reg_field_set(unit, NBI_CHOOSE_RX_WCLK_33_LANE_FOR_MLDr, reg_val32, field_id, caui_config->rx_recovery_lane);
        SOCDNX_IF_ERR_EXIT(WRITE_NBI_CHOOSE_RX_WCLK_33_LANE_FOR_MLDr(unit, *reg_val32));
    }
#endif 

    
    if (SOC_IS_ARAD_A0(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_NBI_MASK_ECC_PARITY_ERROR_FROM_DATA_PATHr(unit, reg_val32));
        soc_reg_field_set(unit, NBI_MASK_ECC_PARITY_ERROR_FROM_DATA_PATHr, reg_val32, RX_MASK_PAR_ERR_DATA_MEMf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_NBI_MASK_ECC_PARITY_ERROR_FROM_DATA_PATHr(unit, *reg_val32));
    }

    
#ifdef BCM_88660_A0
    if(SOC_IS_ARADPLUS(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_PORT_MODE_REGr(unit, port, reg_val32));
        soc_reg_field_set(unit, PORT_MODE_REGr, reg_val32, EGR_1588_TIMESTAMPING_MODEf, (SOC_DPP_CONFIG(unit))->pp.ptp_48b_stamping_enable ? 1 : 0);
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_MODE_REGr(unit, port, *reg_val32));
     }
#endif 


exit:
    SOCDNX_FUNC_RETURN; 
}


STATIC int
soc_cmac_drv_enable_set(int unit, soc_port_t port, int enable)
{
    uint32 ctrl, octrl;
    uint32 reg_val, first_phy_port, flags;
    uint64 counter_val;
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit,port,&flags));
    if (!enable) {  
        SOCDNX_IF_ERR_EXIT(READ_PORT_TXFIFO_PKT_DROP_CTLr(unit, port, &reg_val));
         soc_reg_field_set(unit, PORT_TXFIFO_PKT_DROP_CTLr, &reg_val, DROP_ENf, 1);
         SOCDNX_IF_ERR_EXIT(WRITE_PORT_TXFIFO_PKT_DROP_CTLr(unit, port, reg_val));
    }

    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_CTRLr(unit, port, &ctrl));
    octrl = ctrl;
    
    soc_reg_field_set(unit, CLP_CMAC_CTRLr, &ctrl, TX_ENf, 1);
    soc_reg_field_set(unit, CLP_CMAC_CTRLr, &ctrl, RX_ENf, enable ? 1 : 0);
     
    soc_reg_field_set(unit, CLP_CMAC_CTRLr, &ctrl, SOFT_RESETf, enable ? 0 : 1); 
    if (ctrl != octrl) {
        SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_CTRLr(unit, port, ctrl));
    }
    if(enable) {
        
        SOCDNX_IF_ERR_EXIT(READ_PORT_MIB_RESETr(unit, port, &reg_val));
        soc_reg_field_set(unit, PORT_MIB_RESETr, &reg_val, CLR_CNTf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, port, reg_val));
        soc_reg_field_set(unit, PORT_MIB_RESETr, &reg_val, CLR_CNTf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, port, reg_val));
        
        COMPILER_64_ZERO(counter_val);
        SOCDNX_IF_ERR_EXIT(soc_counter_clear_by_port_reg(unit,port,INVALIDr,0,counter_val));
        
        SOCDNX_IF_ERR_EXIT(READ_PORT_TXFIFO_PKT_DROP_CTLr(unit, port, &reg_val));
        soc_reg_field_set(unit, PORT_TXFIFO_PKT_DROP_CTLr, &reg_val, DROP_ENf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_TXFIFO_PKT_DROP_CTLr(unit, port, reg_val));
    }



exit:
    SOCDNX_FUNC_RETURN
}


STATIC int
soc_cmac_drv_enable_get(int unit, soc_port_t port, int *enable)
{
    uint32 reg_val;
    int is_soft_reset;
    SOCDNX_INIT_FUNC_DEFS

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, CLP_CMAC_CTRLr, port, 0, &reg_val));
    is_soft_reset = soc_reg_field_get(unit, CLP_CMAC_CTRLr, reg_val, SOFT_RESETf);
    *enable = !is_soft_reset;

exit:
    SOCDNX_FUNC_RETURN
}


STATIC int
soc_cmac_drv_speed_set(int unit, soc_port_t port, int speed)
{
    uint32 reg_val32;
    int is_mac_enabled, is_need_to_enable_mac = 0;
    SOCDNX_INIT_FUNC_DEFS

    
    SOCDNX_IF_ERR_EXIT(soc_cmac_drv_enable_get(unit, port, &is_mac_enabled));
    if (is_mac_enabled) {
        SOCDNX_IF_ERR_EXIT(soc_cmac_drv_enable_set(unit, port, FALSE));
        is_need_to_enable_mac = 1;
    }

    
    SOCDNX_IF_ERR_EXIT(READ_PORT_MODE_REGr(unit, port, &reg_val32));
    soc_reg_field_set(unit, PORT_MODE_REGr, &reg_val32, MAC_MODEf, MAC_MODE_AGGR);
    SOCDNX_IF_ERR_EXIT(WRITE_PORT_MODE_REGr(unit, port, reg_val32));

    
    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_MODEr(unit, port, &reg_val32));
    soc_reg_field_set(unit, CLP_CMAC_MODEr, &reg_val32, SPEED_MODEf, SOC_CMAC_DRV_SPEED_10000);
    SOCDNX_IF_ERR_EXIT(soc_common_drv_mac_mode_reg_hdr_set(unit, port, CLP_CMAC_MODEr, &reg_val32));
    soc_reg_field_set(unit, CLP_CMAC_MODEr, &reg_val32, NO_SOP_FOR_CRC_HGf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_MODEr(unit, port, reg_val32));

    
    if (is_need_to_enable_mac) {
        SOCDNX_IF_ERR_EXIT(soc_cmac_drv_enable_set(unit, port, TRUE));
    }

exit:
    SOCDNX_FUNC_RETURN
}


STATIC int
soc_cmac_drv_loopback_set(int unit, soc_port_t port, int lb)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_CTRLr(unit, port, &reg_val));
    soc_reg_field_set(unit, CLP_CMAC_CTRLr, &reg_val, LINE_LOCAL_LPBKf, lb ? 1 : 0);
    SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_CTRLr(unit, port, reg_val));

exit:
    SOCDNX_FUNC_RETURN
}


STATIC int
soc_cmac_drv_loopback_get(int unit, soc_port_t port, int *lb)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_CTRLr(unit, port, &reg_val));
    *lb = soc_reg_field_get(unit, CLP_CMAC_CTRLr, reg_val, LINE_LOCAL_LPBKf);

exit:
    SOCDNX_FUNC_RETURN
}


STATIC int
soc_cmac_drv_frame_max_set(int unit, soc_port_t port, int size)
{
    uint32 reg_val32;
    SOCDNX_INIT_FUNC_DEFS

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, CLP_CMAC_RX_MAX_SIZEr, port, 0, &reg_val32));
    soc_reg_field_set(unit, CLP_CMAC_RX_MAX_SIZEr, &reg_val32, RX_MAX_SIZEf, size);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, CLP_CMAC_RX_MAX_SIZEr, port, 0, reg_val32));

exit:
    SOCDNX_FUNC_RETURN
}


STATIC int
soc_cmac_drv_frame_max_get(int unit, soc_port_t port, int *size)
{
    uint32 reg_val32;
    SOCDNX_INIT_FUNC_DEFS

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, CLP_CMAC_RX_MAX_SIZEr, port, 0, &reg_val32));
    (*size) = soc_reg_field_get(unit, CLP_CMAC_RX_MAX_SIZEr, reg_val32, RX_MAX_SIZEf);

exit:
    SOCDNX_FUNC_RETURN
}


STATIC int
soc_cmac_drv_control_set(int unit, soc_port_t port, soc_mac_control_t type,
                  int value)
{
    uint32 rval;
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS

    switch (type) {
        case SOC_MAC_CONTROL_FAULT_LOCAL_STATUS:
            SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_CLEAR_RX_LSS_STATUSr(unit, port, &rval));
            soc_reg_field_set(unit, CLP_CMAC_CLEAR_RX_LSS_STATUSr, &rval, CLEAR_LOCAL_FAULT_STATUSf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_CLEAR_RX_LSS_STATUSr(unit, port, rval));
            soc_reg_field_set(unit, CLP_CMAC_CLEAR_RX_LSS_STATUSr, &rval, CLEAR_LOCAL_FAULT_STATUSf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_CLEAR_RX_LSS_STATUSr(unit, port, rval));
            break;

        case SOC_MAC_CONTROL_FAULT_REMOTE_STATUS:
            SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_CLEAR_RX_LSS_STATUSr(unit, port, &rval));
            soc_reg_field_set(unit, CLP_CMAC_CLEAR_RX_LSS_STATUSr, &rval, CLEAR_REMOTE_FAULT_STATUSf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_CLEAR_RX_LSS_STATUSr(unit, port, rval));
            soc_reg_field_set(unit, CLP_CMAC_CLEAR_RX_LSS_STATUSr, &rval, CLEAR_REMOTE_FAULT_STATUSf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_CLEAR_RX_LSS_STATUSr(unit, port, rval));
            break;
#ifdef BCM_88660_A0
        case SOC_MAC_CONTROL_FAULT_REMOTE_TX_ENABLE:
            if(SOC_IS_ARADPLUS(unit)){
                SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_RX_LSS_CTRLr(unit, port , &rval));
                soc_reg_field_set(unit, CLP_CMAC_RX_LSS_CTRLr, &rval, USE_EXTERNAL_FAULTS_FOR_TXf, (value == 0)? 0: 1);
                SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_RX_LSS_CTRLr(unit, port, rval));
            }
            else{
                SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Unsupported control %d"), type));
            }
            break;
#endif
        case SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE:
            SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_RX_LSS_CTRLr(unit, port , &rval));
            soc_reg_field_set(unit, CLP_CMAC_RX_LSS_CTRLr, &rval, LOCAL_FAULT_DISABLEf, (value == 1) ? 0: 1);
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_RX_LSS_CTRLr(unit, port, rval));
            break;

        case SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE:
            SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_RX_LSS_CTRLr(unit, port , &rval));
            soc_reg_field_set(unit, CLP_CMAC_RX_LSS_CTRLr, &rval, REMOTE_FAULT_DISABLEf, (value == 1) ? 0: 1);
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_RX_LSS_CTRLr(unit, port, rval));
            break;

        case SOC_MAC_CONTROL_PAD_SIZE:
            SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_TX_CTRLr(unit, port, &reg_val64));
            soc_reg64_field32_set(unit, CLP_CMAC_TX_CTRLr, &reg_val64, PAD_ENf, value ? 1 : 0);
            if(value) {
                soc_reg64_field32_set(unit, CLP_CMAC_TX_CTRLr, &reg_val64, PAD_THRESHOLDf, value);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_TX_CTRLr(unit, port, reg_val64));
            break;

        case SOC_MAC_CONTROL_PFC_REFRESH_TIME:
            SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_PFC_CTRLr(unit, port, &reg_val64));
            soc_reg64_field32_set(unit, CLP_CMAC_PFC_CTRLr, &reg_val64, PFC_REFRESH_ENf, value ? 1 : 0);
            if(value) {
                soc_reg64_field32_set(unit, CLP_CMAC_PFC_CTRLr, &reg_val64, PFC_REFRESH_TIMERf, value);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_PFC_CTRLr(unit, port, reg_val64));
            break;

        case SOC_MAC_CONTROL_PFC_RX_ENABLE:
            SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_PFC_CTRLr(unit, port, &reg_val64));
            soc_reg64_field32_set(unit, CLP_CMAC_PFC_CTRLr, &reg_val64, RX_PFC_ENf, value ? 1 : 0);
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_PFC_CTRLr(unit, port, reg_val64));
            break;

        case SOC_MAC_CONTROL_PFC_TX_ENABLE:
            SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_PFC_CTRLr(unit, port, &reg_val64));
            soc_reg64_field32_set(unit, CLP_CMAC_PFC_CTRLr, &reg_val64, TX_PFC_ENf, value ? 1 : 0);
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_PFC_CTRLr(unit, port, reg_val64));
            break;

        case SOC_MAC_CONTROL_LLFC_RX_ENABLE:
            SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_LLFC_CTRLr(unit, port, &rval));
            soc_reg_field_set(unit, CLP_CMAC_LLFC_CTRLr, &rval, RX_LLFC_ENf, value ? 1 : 0);
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_LLFC_CTRLr(unit, port, rval));
            break;

        case SOC_MAC_CONTROL_LLFC_TX_ENABLE:
            SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_LLFC_CTRLr(unit, port, &rval));
            soc_reg_field_set(unit, CLP_CMAC_LLFC_CTRLr, &rval, TX_LLFC_ENf, value ? 1 : 0);
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_LLFC_CTRLr(unit, port, rval));
            break;

        case SOC_MAC_CONTROL_EEE_ENABLE:
            SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_EEE_CTRLr(unit, port, &rval));
            soc_reg_field_set(unit, CLP_CMAC_EEE_CTRLr, &rval, EEE_ENf, value ? 1 : 0);
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_EEE_CTRLr(unit, port, rval));
            break;

        case SOC_MAC_CONTROL_EEE_TX_IDLE_TIME:
            SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_EEE_TIMERSr(unit, port, &reg_val64));
            soc_reg64_field32_set(unit, CLP_CMAC_EEE_TIMERSr, &reg_val64, EEE_DELAY_ENTRY_TIMERf, value);
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_EEE_TIMERSr(unit, port, reg_val64));
            break;

        case SOC_MAC_CONTROL_EEE_TX_WAKE_TIME:
            SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_EEE_TIMERSr(unit, port, &reg_val64));
            soc_reg64_field32_set(unit, CLP_CMAC_EEE_TIMERSr, &reg_val64, EEE_WAKE_TIMERf, value);
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_EEE_TIMERSr(unit, port, reg_val64));
            break;

        case SOC_MAC_CONTROL_EEE_LINK_ACTIVE_DURATION:
            if (value >= (1 << soc_reg_field_length(unit, CLP_CMAC_EEE_1_SEC_LINK_STATUS_TIMERr, ONE_SECOND_TIMERf))) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Duration %d is too long\n"), value));
            }

            SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_EEE_1_SEC_LINK_STATUS_TIMERr(unit, port, &rval));
            soc_reg_field_set(unit, CLP_CMAC_EEE_1_SEC_LINK_STATUS_TIMERr, &rval, ONE_SECOND_TIMERf, value);
            SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_EEE_1_SEC_LINK_STATUS_TIMERr(unit, port, rval));
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Unsupported control %d"), type));
    }


exit:
    SOCDNX_FUNC_RETURN
}


STATIC int
soc_cmac_drv_control_get(int unit, soc_port_t port, soc_mac_control_t type,
                  int *value)
{
    uint32 rval;
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS

    switch (type) {

    case SOC_MAC_CONTROL_FAULT_LOCAL_STATUS:
        SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_RX_LSS_STATUSr(unit, port, &rval));
        *value = soc_reg_field_get(unit, CLP_CMAC_RX_LSS_STATUSr, rval, LOCAL_FAULT_STATUSf);
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_STATUS:
        SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_RX_LSS_STATUSr(unit, port, &rval));
        *value = soc_reg_field_get(unit, CLP_CMAC_RX_LSS_STATUSr, rval, REMOTE_FAULT_STATUSf);
        break;
#ifdef BCM_88660_A0
    case SOC_MAC_CONTROL_FAULT_REMOTE_TX_ENABLE:
        if(SOC_IS_ARADPLUS(unit)){
            SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_RX_LSS_CTRLr(unit, port , &rval));
            *value = soc_reg_field_get(unit, CLP_CMAC_RX_LSS_CTRLr, rval, USE_EXTERNAL_FAULTS_FOR_TXf);
        }
        else{
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Unsupported control %d"), type));
        }
        break;
#endif
    case SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE:
        SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_RX_LSS_CTRLr(unit, port , &rval));
        *value = soc_reg_field_get(unit, CLP_CMAC_RX_LSS_CTRLr, rval, LOCAL_FAULT_DISABLEf);
        *value = !(*value);
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE:
        SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_RX_LSS_CTRLr(unit, port , &rval));
        *value = soc_reg_field_get(unit, CLP_CMAC_RX_LSS_CTRLr, rval, REMOTE_FAULT_DISABLEf);
        *value = !(*value);
        break;

    case SOC_MAC_CONTROL_PFC_REFRESH_TIME:
        SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_PFC_CTRLr(unit, port, &reg_val64));
        *value = soc_reg64_field32_get(unit, CLP_CMAC_PFC_CTRLr, reg_val64, PFC_REFRESH_ENf);
        if(*value) { 
            *value = soc_reg64_field32_get(unit, CLP_CMAC_PFC_CTRLr, reg_val64, PFC_REFRESH_TIMERf);
        }
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_PFC_CTRLr(unit, port, &reg_val64));
        *value = soc_reg64_field32_get(unit, CLP_CMAC_PFC_CTRLr, reg_val64, RX_PFC_ENf);
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_PFC_CTRLr(unit, port, &reg_val64));
        *value = soc_reg64_field32_get(unit, CLP_CMAC_PFC_CTRLr, reg_val64, TX_PFC_ENf);
        break;

    case SOC_MAC_CONTROL_LLFC_RX_ENABLE:
        SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_LLFC_CTRLr(unit, port, &rval));
        *value = soc_reg_field_get(unit, CLP_CMAC_LLFC_CTRLr, rval, RX_LLFC_ENf);
        break;

    case SOC_MAC_CONTROL_LLFC_TX_ENABLE:
        SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_LLFC_CTRLr(unit, port, &rval));
        *value = soc_reg_field_get(unit, CLP_CMAC_LLFC_CTRLr, rval, TX_LLFC_ENf);
        break;
    case SOC_MAC_CONTROL_EEE_ENABLE:
        SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_EEE_CTRLr(unit, port, &rval));
        *value = soc_reg_field_get(unit, CLP_CMAC_EEE_CTRLr, rval, EEE_ENf);
        break;

    case SOC_MAC_CONTROL_EEE_TX_IDLE_TIME:
        SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_EEE_TIMERSr(unit, port, &reg_val64));
        *value = soc_reg64_field32_get(unit, CLP_CMAC_EEE_TIMERSr, reg_val64, EEE_DELAY_ENTRY_TIMERf);
        break;

    case SOC_MAC_CONTROL_EEE_TX_WAKE_TIME:
        SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_EEE_TIMERSr(unit, port, &reg_val64));
        *value = soc_reg64_field32_get(unit, CLP_CMAC_EEE_TIMERSr, reg_val64, EEE_WAKE_TIMERf);
        break;

    case SOC_MAC_CONTROL_EEE_LINK_ACTIVE_DURATION:
        SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_EEE_1_SEC_LINK_STATUS_TIMERr(unit, port, &rval));
        *value = soc_reg_field_get(unit, CLP_CMAC_EEE_1_SEC_LINK_STATUS_TIMERr, rval, ONE_SECOND_TIMERf);
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Unsupported control %d"), type));
    }


exit:
    SOCDNX_FUNC_RETURN
}


STATIC int
soc_cmac_ability_local_get(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
    int port_max_speed;

    SOCDNX_INIT_FUNC_DEFS
    SOCDNX_NULL_CHECK(ability);

    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_MII | SOC_PA_INTF_XGMII;
    ability->medium    = SOC_PA_MEDIUM_COPPER | SOC_PA_MEDIUM_FIBER;
    ability->loopback  = SOC_PA_LB_MAC | SOC_PA_LB_PHY;
    ability->flags     = SOC_PA_ABILITY_NONE;
    ability->encap = SOC_PA_ENCAP_IEEE | SOC_PA_ENCAP_HIGIG |
        SOC_PA_ENCAP_HIGIG2;

    port_max_speed = SOC_INFO(unit).port_speed_max[port];
    if (SOC_INFO(unit).port_num_lanes[port] == 12){
        if(port_max_speed >= 126500){
            ability->speed_full_duplex |= SOC_PA_SPEED_127GB;
        }
        if(port_max_speed >= 119500){
            ability->speed_full_duplex |= SOC_PA_SPEED_120GB;
        }
    }
    else
    {
        if(port_max_speed >= 105500){
            ability->speed_full_duplex |= SOC_PA_SPEED_106GB;
        }
        if(port_max_speed >= 99500){
            ability->speed_full_duplex |= SOC_PA_SPEED_100GB;
        }
    }
exit:
    SOCDNX_FUNC_RETURN
}

STATIC int
soc_cmac_pause_addr_set(int unit, soc_port_t port, sal_mac_addr_t m)
{
    uint64 r, tmp;
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(r);
    for (i = 0; i< 6; i++) {
        COMPILER_64_SET(tmp, 0, m[i]);
        COMPILER_64_SHL(r, 8);
        COMPILER_64_OR(r, tmp);
    }

    SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_TX_MAC_SAr(unit,  port, r));
    SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_RX_MAC_SAr(unit,  port, r));

exit:
    SOCDNX_FUNC_RETURN
}

STATIC int
soc_cmac_pause_addr_get(int unit, soc_port_t port, sal_mac_addr_t m)
{
    uint64 reg;
    uint32 msw;
    uint32 lsw;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_RX_MAC_SAr(unit, port, &reg));

    COMPILER_64_TO_32_HI(msw, reg);
    COMPILER_64_TO_32_LO(lsw, reg);

    m[0] = (uint8) ( ( msw & 0x0000ff00 ) >> 8 );
    m[1] = (uint8) ( msw & 0x000000ff );

    m[2] = (uint8)  ( ( lsw & 0xff000000) >> 24 );
    m[3] = (uint8)  ( ( lsw & 0x00ff0000) >> 16 );
    m[4] = (uint8)  ( ( lsw & 0x0000ff00) >> 8 );
    m[5] = (uint8)  ( lsw & 0x000000ff );

exit:
    SOCDNX_FUNC_RETURN
}

STATIC int
soc_cmac_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx)
{
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_PAUSE_CTRLr(unit, port, &reg_val64));
    soc_reg64_field32_set(unit, CLP_CMAC_PAUSE_CTRLr, &reg_val64, TX_PAUSE_ENf, pause_tx);
    soc_reg64_field32_set(unit, CLP_CMAC_PAUSE_CTRLr, &reg_val64, RX_PAUSE_ENf, pause_rx);
    SOCDNX_IF_ERR_EXIT(WRITE_CLP_CMAC_PAUSE_CTRLr(unit, port, reg_val64));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_cmac_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx)
{
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_CLP_CMAC_PAUSE_CTRLr(unit, port, &reg_val64));
    *pause_tx = soc_reg64_field32_get(unit, CLP_CMAC_PAUSE_CTRLr, reg_val64, TX_PAUSE_ENf);
    *pause_rx = soc_reg64_field32_get(unit, CLP_CMAC_PAUSE_CTRLr, reg_val64, RX_PAUSE_ENf);

exit:
    SOCDNX_FUNC_RETURN;
}

CONST mac_driver_t soc_cmac_driver = {
    "DPP CMAC Driver",              
    soc_cmac_drv_init,              
    soc_cmac_drv_enable_set,        
    soc_cmac_drv_enable_get,        
    NULL,                           
    NULL,                           
    soc_cmac_drv_speed_set,         
    NULL,                           
    soc_cmac_pause_set,             
    soc_cmac_pause_get,             
    soc_cmac_pause_addr_set,        
    soc_cmac_pause_addr_get,        
    soc_cmac_drv_loopback_set,      
    soc_cmac_drv_loopback_get,      
    NULL,                           
    NULL,                           
    NULL,                           
    soc_cmac_drv_frame_max_set,     
    soc_cmac_drv_frame_max_get,     
    NULL,                           
    NULL,                           
    NULL,                           
    NULL,                           
    soc_cmac_drv_control_set,       
    soc_cmac_drv_control_get,       
    soc_cmac_ability_local_get      
 };

#undef _ERR_MSG_MODULE_NAME


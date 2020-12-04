/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/error.h>
#include <soc/cmic.h>
#include <sal/core/alloc.h>
#include <soc/wb_engine.h>
#include <soc/drv.h>
#include <soc/dcmn/error.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/dcmn/dcmn_port.h>
#include <soc/portmod/portmod.h>
#include <soc/dpp/dpp_config_defs.h>

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#define QAX_FABRIC_FMAC_TX (0)
#define QAX_FABRIC_FMAC_RX (1)
#define QAX_FABRIC_LANES_PER_CORE SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac)

STATIC int _qax_fabric_with_packet_tdm_force_signal_detect_set(int unit, int quad);
STATIC int qax_fabric_with_packet_tdm_mac_loopback_set(int unit, int quad, portmod_loopback_mode_t loopback_type, int enable);


int qax_fabric_with_packet_tdm_disable_init(int unit, int quad)
{
    uint32 reg_val;
    uint32 field_val[1];
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, quad, &reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_RX_RST_Nf);
    for (i = 0; i < QAX_FABRIC_LANES_PER_CORE; i++)
    {
        SHR_BITSET(field_val, i);
    }
    soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_RX_RST_Nf, *field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, quad, reg_val));

    SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, quad, &reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_TX_RST_Nf);
    for (i = 0; i < QAX_FABRIC_LANES_PER_CORE; i++)
    {
        SHR_BITSET(field_val, i);
    }
    soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_TX_RST_Nf, *field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, quad, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}


int qax_fabric_with_packet_tdm_attach(int unit, int quad)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_disable_init(unit, quad));
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t qax_fabric_with_packet_tdm_fmac_enable_set(int unit, int quad, int rx_tx, int enable)
{
    soc_field_t field;
    uint32 field_val[1];
    uint32 reg_val;
    int i;
    int fmac_blk_id_masked = quad;
    SOCDNX_INIT_FUNC_DEFS;

    field = ((rx_tx == QAX_FABRIC_FMAC_TX) ? FMAC_TX_RST_Nf : FMAC_RX_RST_Nf);
    SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_id_masked, &reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, field);

    for (i = 0; i < QAX_FABRIC_LANES_PER_CORE; i++)
    {
        if (enable){
            SHR_BITCLR(field_val, i);
        }else{
            SHR_BITSET(field_val, i);
        }
    }

    soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, field, *field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_id_masked, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}


int qax_fabric_with_packet_tdm_enable_get(int unit, int quad, int *enable)
{
    uint32 reg_val;
    uint32 field_val[1];
    uint32 mac_reset;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, quad, &reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_RX_RST_Nf);
    mac_reset = SHR_BITGET(field_val, 0);

    *enable = !mac_reset;
exit:
    SOCDNX_FUNC_RETURN;
}

int qax_fabric_with_packet_tdm_enable_set(int unit, int quad, int enable)
{
    int cur_enable;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_enable_get(unit, quad, &cur_enable)); 
    if ((cur_enable ? 1 : 0) == (enable ? 1 : 0)){
        SOC_EXIT;
    }

    if(enable){
        
        SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_fmac_enable_set(unit, quad, QAX_FABRIC_FMAC_TX, enable));
        
        SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_fmac_enable_set(unit, quad, QAX_FABRIC_FMAC_RX, enable));
    } else
    {
        
        SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_fmac_enable_set(unit, quad, QAX_FABRIC_FMAC_RX, enable));
        
        SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_fmac_enable_set(unit, quad, QAX_FABRIC_FMAC_TX, enable));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int qax_fabric_with_packet_tdm_encoding_set(int unit, int quad, uint32 properties, portmod_port_pcs_t encoding)
{
    int fec_enable = 1;
    int encoding_type = 0;
    int rx_half_frame = 0;
    int tx_half_frame = 0;
    int rx_ctrl_bypass = 0;
    int enabled = 0;
    int llfc_cig = 0;
    int low_latency_llfc = 0;
    int rsf_err_mark = 0;
    int fec_err_mark = 0;
    int cig_ignore = 0;
    int fmac_blk_id_masked = quad; 
    uint32 reg_val;
    soc_reg_above_64_val_t reg_above64;
    int i;
    SOCDNX_INIT_FUNC_DEFS;


    switch(encoding){
    case PORTMOD_PCS_64B66B_FEC:
        encoding_type = 2;
        break;
    case PORTMOD_PCS_64B66B:
        encoding_type = 2;
        fec_enable = 0;
        break;
    case PORTMOD_PCS_64B66B_RS_FEC:
        encoding_type = 4;
        rx_ctrl_bypass = 3;
        low_latency_llfc = 1;
        llfc_cig = 1;
        break;
    case PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC:
        encoding_type = 4;
        rx_ctrl_bypass = 3;
        rx_half_frame = 1;
        tx_half_frame = 1;
        low_latency_llfc = 1;
        llfc_cig = 1;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported pcs type %d"), encoding));
    }

    
    
    if(PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties) && (encoding == PORTMOD_PCS_64B66B)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("low latency llfc is not supported 64/66 pcs")));
    }
    if(!PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties) && (encoding == PORTMOD_PCS_64B66B_RS_FEC || encoding == PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("RS_FEC pcs: low latency llfc is always enabled")));
    }
    if(PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties)){
        low_latency_llfc = 1;
    }

    
    if(PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties) && (encoding == PORTMOD_PCS_64B66B)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("error detect is not supported 64/66 pcs")));
    }
    if(encoding == PORTMOD_PCS_64B66B_FEC){
        fec_err_mark = PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties);
    } else if((encoding == PORTMOD_PCS_64B66B_RS_FEC) || (encoding == PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC)){
        rsf_err_mark = PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties);
    }

    
    if(PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_GET(properties) && (encoding != PORTMOD_PCS_64B66B_FEC)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("error detect is supported just for 64/66 kr fec")));
    }
    cig_ignore = PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_GET(properties) ? 0 : 1;

    SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_enable_get(unit, quad, &enabled));
    
    SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_enable_set(unit, quad, 0));
    for (i = 0; i < QAX_FABRIC_LANES_PER_CORE; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, i, &reg_val));
        
        soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_ENCODING_TYPEf, encoding_type);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, i, reg_val));
    }

    for (i = 0; i < QAX_FABRIC_LANES_PER_CORE; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_id_masked, i, &reg_val));
        
        soc_reg_field_set(unit,FMAC_FPS_RX_FEC_CONFIGURATIONr,&reg_val,FPS_N_RX_FEC_FEC_ENf, fec_enable);
        
        soc_reg_field_set(unit,FMAC_FPS_RX_FEC_CONFIGURATIONr,&reg_val,FPS_N_RX_FEC_ERR_MARK_ENf, fec_err_mark);
        soc_reg_field_set(unit,FMAC_FPS_RX_FEC_CONFIGURATIONr,&reg_val,FPS_N_RX_FEC_ERR_MARK_ALLf, fec_err_mark);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_id_masked, i, reg_val));
    }

    for (i = 0; i < QAX_FABRIC_LANES_PER_CORE; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_TX_CONFIGURATIONr(unit, fmac_blk_id_masked, i, &reg_val));
        soc_reg_field_set(unit,FMAC_FPS_TX_CONFIGURATIONr,&reg_val,FPS_N_TX_FEC_ENf, fec_enable);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FPS_TX_CONFIGURATIONr(unit, fmac_blk_id_masked, i, reg_val));
    }


    
    for (i = 0; i < QAX_FABRIC_LANES_PER_CORE; i++)
    {

        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_RX_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, i, &reg_val));
        
        soc_reg_field_set(unit,FMAC_FMAL_RX_GENERAL_CONFIGURATIONr,&reg_val,FMAL_N_LOW_LATENCY_LLFCf, low_latency_llfc);
        soc_reg_field_set(unit,FMAC_FMAL_RX_GENERAL_CONFIGURATIONr,&reg_val,FMAL_N_LOW_LATENCY_CIGf, llfc_cig);
        soc_reg_field_set(unit,FMAC_FMAL_RX_GENERAL_CONFIGURATIONr,&reg_val,FMAL_N_IGNORE_CIG_LLFC_CELLf, cig_ignore);
        soc_reg_field_set(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_RX_CTRL_BYPf, rx_ctrl_bypass);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_RX_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, i, reg_val));
    }

    for (i = 0; i < QAX_FABRIC_LANES_PER_CORE; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id_masked, i, reg_above64));
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_ERR_MARK_ENf, rsf_err_mark);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_HALF_FRAMEf, rx_half_frame);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_TX_HALF_FRAMEf, tx_half_frame);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_TX_SCRAMBLE_LLFC_BITS_ENf, 0x1);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_SCRAMBLE_LLFC_BITS_ENf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id_masked, i, reg_above64));
    }

    
    SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_enable_set(unit, quad, enabled));
exit:
    SOCDNX_FUNC_RETURN;
}

int qax_fabric_with_packet_tdm_encoding_get(int unit, int quad, uint32 *properties, portmod_port_pcs_t *encoding)
{
    int encoding_type = 0;
    int rx_half_frame = 0;
    int low_latency_llfc = 0;
    int err_mark = 0;
    int cig_ignore = 0;
    int fmac_blk_id_masked = quad;
    uint32 reg_val;
    soc_reg_above_64_val_t reg_above64;
    int fec_enable;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, 0, &reg_val));
    *properties = 0;
    encoding_type = soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_ENCODING_TYPEf);
    switch(encoding_type){
        case 2:
            SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_id_masked, 0, &reg_val));
            fec_enable = soc_reg_field_get(unit,FMAC_FPS_RX_FEC_CONFIGURATIONr, reg_val,FPS_N_RX_FEC_FEC_ENf);
            *encoding = fec_enable ? PORTMOD_PCS_64B66B_FEC : PORTMOD_PCS_64B66B;
            break;
        case 4:
            SOCDNX_IF_ERR_EXIT(READ_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id_masked, 0, reg_above64));
            rx_half_frame = soc_reg_above_64_field32_get(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_HALF_FRAMEf);
            if(rx_half_frame == 0){
                *encoding = PORTMOD_PCS_64B66B_RS_FEC;
            }
            else{
                *encoding = PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC;
            }
            break;
        case 0:
            *encoding = PORTMOD_PCS_UNKNOWN;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unknown pcs type %d"), encoding_type));
    }

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_RX_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, 0, &reg_val));
    
    low_latency_llfc = soc_reg_field_get(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, reg_val ,FMAL_N_LOW_LATENCY_LLFCf);
    cig_ignore = soc_reg_field_get(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_IGNORE_CIG_LLFC_CELLf);
    if (*encoding == PORTMOD_PCS_64B66B_FEC)
    {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_id_masked, 0, &reg_val));
        err_mark = soc_reg_field_get(unit, FMAC_FPS_RX_FEC_CONFIGURATIONr, reg_val, FPS_N_RX_FEC_ERR_MARK_ALLf);
    } else if (*encoding == PORTMOD_PCS_64B66B_RS_FEC || *encoding == PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC)
    {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id_masked, 0, reg_above64));
        err_mark = soc_reg_above_64_field32_get(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_ERR_MARK_ENf);

    } else {
        err_mark = 0;
    }

    if(low_latency_llfc){
         PORTMOD_ENCODING_LOW_LATENCY_LLFC_SET(*properties);
    }
    if(!cig_ignore){
         PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_SET(*properties);
    }
    if(err_mark){
         PORTMOD_ENCODING_FEC_ERROR_DETECT_SET(*properties);
    }
exit:
    SOCDNX_FUNC_RETURN;
}




int qax_fabric_with_packet_tdm_loopback_get(int unit, int quad, portmod_loopback_mode_t loopback_type, int* enable)
{
    uint32 reg_val = 0;
    uint32 field[1] = {0};
    uint32 fmac_blk_id_masked = quad ;
    uint32 phy_index = 0;
    SOCDNX_INIT_FUNC_DEFS;


    switch(loopback_type){
        case portmodLoopbackMacAsyncFifo:
            SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, &reg_val));
            *enable = soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_CORE_LOOPBACKf);
            break;
        case portmodLoopbackMacOuter:
            SOCDNX_IF_ERR_EXIT(READ_FMAC_LOOPBACK_ENABLE_REGISTERr(unit, fmac_blk_id_masked, &reg_val));
            *field = soc_reg_field_get(unit, FMAC_LOOPBACK_ENABLE_REGISTERr, reg_val, LCL_LPBK_ONf);
            *enable = SHR_BITGET(field, phy_index);
            break;
        case portmodLoopbackMacPCS:
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, &reg_val));
            *enable = soc_reg_field_get(unit, FMAC_KPCS_CONFIGURATIONr, reg_val, KPCS_N_RX_DSC_LOOPBACK_ENf);
            break;
        
        default:
            *enable = 0; 
    }
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
_qax_fabric_with_packet_tdm_force_signal_detect_set(int unit, int quad)
{
    int i;
    int force_signal_detect = 0;
    uint32 reg_val;
    int fmac_blk_id_masked = quad; 
    portmod_loopback_mode_t mac_loopbacks[]= { portmodLoopbackMacAsyncFifo, portmodLoopbackMacOuter, portmodLoopbackMacPCS, portmodLoopbackPhyGloopPMD,portmodLoopbackPhyRloopPMD};
    int lb_enable = FALSE;
    SOCDNX_INIT_FUNC_DEFS;

    for( i = 0 ; i < sizeof(mac_loopbacks)/sizeof(mac_loopbacks[0]); i++){
        SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_loopback_get(unit, quad, mac_loopbacks[i], &lb_enable));
        if(lb_enable){
            force_signal_detect = 1;
            break;
        }
    }
    SOCDNX_IF_ERR_EXIT(READ_FMAC_GENERAL_CONFIGURATION_REGISTERr_REG32(unit, fmac_blk_id_masked, &reg_val));
    soc_reg_field_set(unit, FMAC_GENERAL_CONFIGURATION_REGISTERr, &reg_val, FORCE_SIGNAL_DETECTf, force_signal_detect);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_GENERAL_CONFIGURATION_REGISTERr_REG32(unit, fmac_blk_id_masked, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
int qax_fabric_with_packet_tdm_mac_loopback_set(int unit, int quad, portmod_loopback_mode_t loopback_type, int enable)
{
    uint32 reg_val = 0;
    uint32 field[1] = {0};
    uint32 fmac_blk_id_masked = quad;
    uint32 phy_index;
    soc_reg_above_64_val_t reg_above64;

    SOCDNX_INIT_FUNC_DEFS;

    for (phy_index = 0; phy_index < QAX_FABRIC_LANES_PER_CORE; phy_index++) {
        if (SOC_REG_IS_VALID(unit, FMAC_MACT_PM_CREDIT_FILTERr)){
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FMAC_MACT_PM_CREDIT_FILTERr, fmac_blk_id_masked, phy_index, MACT_N_PM_CREDIT_FILTERf, enable));
        }

        switch(loopback_type){
            case portmodLoopbackMacOuter:
                SOCDNX_IF_ERR_EXIT(READ_FMAC_LOOPBACK_ENABLE_REGISTERr(unit, fmac_blk_id_masked, &reg_val));
                *field = soc_reg_field_get(unit, FMAC_LOOPBACK_ENABLE_REGISTERr, reg_val, LCL_LPBK_ONf);
                if(enable) {
                    SHR_BITSET(field, phy_index);
                } else {
                    SHR_BITCLR(field, phy_index);
                }
                soc_reg_field_set(unit, FMAC_LOOPBACK_ENABLE_REGISTERr, &reg_val, LCL_LPBK_ONf, *field);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_LOOPBACK_ENABLE_REGISTERr(unit, fmac_blk_id_masked, reg_val));
                if (enable)
                {
                    LOG_WARN(BSL_LS_SOC_PORT,
                             (BSL_META_U(unit,
                                         "Warning: MAC outer loopback is supported over rates 20.625 and higher\n")));
                }
                break;

            case portmodLoopbackMacAsyncFifo:
                SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, &reg_val));
                soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_CORE_LOOPBACKf, enable);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, reg_val));

                SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, fmac_blk_id_masked, phy_index, &reg_val));
                soc_reg_field_set(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, &reg_val, FMAL_N_CNTRL_BURST_PERIODf, enable ? 0xc : 8);
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, fmac_blk_id_masked, phy_index, reg_val));
                break;

            case portmodLoopbackMacPCS:
                SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, &reg_val));
                soc_reg_field_set(unit, FMAC_KPCS_CONFIGURATIONr, &reg_val, KPCS_N_RX_DSC_LOOPBACK_ENf, enable);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, reg_val));

                SOCDNX_IF_ERR_EXIT(READ_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, reg_above64));
                soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, FIELD_0_0f, enable? 0:1);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, reg_above64));

                SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, fmac_blk_id_masked, phy_index, &reg_val));
                soc_reg_field_set(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, &reg_val, FMAL_N_CNTRL_BURST_PERIODf, enable ? 0xd : 8);
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, fmac_blk_id_masked, phy_index, reg_val));
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("shouldnot reach here")));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}


int qax_fabric_with_packet_tdm_loopback_set(int unit, int quad, portmod_loopback_mode_t loopback_type, int enable)
{
    int lb_enable = 0;
    int enabled;
    uint32 regVal;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_enable_get(unit, quad, &enabled));

    
    SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_enable_set(unit, quad, 0));
    if(enable) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_GENERAL_CONFIGURATION_REGISTERr_REG32(unit, quad, &regVal));
        soc_reg_field_set(unit, FMAC_GENERAL_CONFIGURATION_REGISTERr, &regVal, DEL_DATA_CELL_LBf, 0);
        soc_reg_field_set(unit, FMAC_GENERAL_CONFIGURATION_REGISTERr, &regVal, DEL_FS_CR_CELL_LBf, 0);
        soc_reg_field_set(unit, FMAC_GENERAL_CONFIGURATION_REGISTERr, &regVal, DEL_ALL_CELLS_LBf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_GENERAL_CONFIGURATION_REGISTERr_REG32(unit, quad, regVal));
    }
    
    switch(loopback_type){
        case portmodLoopbackMacAsyncFifo:
        case portmodLoopbackMacOuter:
        case portmodLoopbackMacPCS:
            break;
        
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("unsupported loopback type %d"), loopback_type));
    }

    
    SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_loopback_get(unit, quad, loopback_type, &lb_enable));
    if(enable == lb_enable){
        SOC_EXIT;
    }
    
    SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_mac_loopback_set(unit, quad, loopback_type, enable));

    SOCDNX_IF_ERR_EXIT(_qax_fabric_with_packet_tdm_force_signal_detect_set(unit, quad));

    
    SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_enable_set(unit, quad, enabled));
exit:
    SOCDNX_FUNC_RETURN;
}


int qax_fabric_with_packet_tdm_link_get(int unit, int quad, int* link)
{
    int enable;
    uint32 phy_index;
    uint32 reg_val = 0, fld_val[1] = {0};
    SOCDNX_INIT_FUNC_DEFS;

    
    *link = 0;

    
    SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_enable_get(unit, quad, &enable));

    if (enable) {
        
        reg_val = 0;
        for (phy_index = 0; phy_index < QAX_FABRIC_LANES_PER_CORE; phy_index++) {
            SHR_BITSET(fld_val, phy_index);
        }
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit,  quad, &reg_val));
        soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, &reg_val, RX_LOST_OF_SYNCf, *fld_val);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit,  quad | SOC_REG_ADDR_SCHAN_ID_MASK, reg_val));

        
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit, quad | SOC_REG_ADDR_SCHAN_ID_MASK, &reg_val));

        *fld_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, reg_val, RX_LOST_OF_SYNCf);
        *link = (SHR_BITGET(fld_val, 0) == 0);
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int qax_fabric_with_packet_tdm_loopback_enable(int unit, int quad, int enable)
{
    portmod_loopback_mode_t  loopback_mode = portmodLoopbackMacPCS;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_loopback_set(unit, quad, loopback_mode, enable));
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
qax_fabric_with_packet_tdm_port_probe(int unit, int quad)
{
    uint32 encoding_properties = 0;
    int pcs = PORTMOD_PCS_64B66B_RS_FEC;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    PORTMOD_ENCODING_LOW_LATENCY_LLFC_SET(encoding_properties);
    if (!SOC_WARM_BOOT(unit)) {
        
        SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_attach(unit, quad));
        SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_encoding_set(unit, quad, encoding_properties, pcs));
        
        SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_enable_set(unit, quad, 0));
        
        SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_loopback_enable(unit, quad, 1));
        
        SOCDNX_IF_ERR_EXIT(qax_fabric_with_packet_tdm_enable_set(unit, quad, 1));

        SOCDNX_IF_ERR_EXIT(READ_FMAC_LEAKY_BUCKET_CONTROL_REGISTERr(unit, quad, &reg_val));
        soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val, SIG_DET_BKT_RST_ENAf, 0);
        soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val, BKT_LINK_UP_THf, 0);
        soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val, FIELD_29_29f, 0);
        soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val, ALIGN_LCK_BKT_RST_ENAf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_LEAKY_BUCKET_CONTROL_REGISTERr(unit, quad, reg_val));
    }
exit:
    SOCDNX_FUNC_RETURN;
}
#undef _ERR_MSG_MODULE_NAME


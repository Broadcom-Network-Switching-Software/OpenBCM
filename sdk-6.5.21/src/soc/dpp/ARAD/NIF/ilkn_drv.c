/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC ILKN DRV
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
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/mcm/memregs.h>
#include <soc/cmic.h>
#include <soc/drv.h>
#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/drv.h>
#endif

#define NIF_ILKN_BURST_MAX_256B_VAL         (0x3)
#define NIF_ILKN_TX_BURST_SHORT_64B_VAL     (0x1)
#define NIF_ILKN_FREE_ENTRIES_VAL           (0x6)

STATIC soc_error_t 
soc_ilkn_drv_retransmit_config(int unit, soc_port_t port)
{
    uint32 reg_val, retrans_multiply_rx, retrans_multiply_tx, offset;
    uint64 reg_val64;
    ARAD_PORTS_ILKN_CONFIG *ilkn_config;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));

    ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[offset];
    
    SOCDNX_IF_ERR_EXIT(READ_NBI_ENABLE_INTERLAKENr(unit, &reg_val));
    soc_reg_field_set(unit, NBI_ENABLE_INTERLAKENr, &reg_val, (0 == offset) ? FORCE_EGQ_SEGMENTATION_TX_0f:FORCE_EGQ_SEGMENTATION_TX_1f, ilkn_config->retransmit.enable_rx);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_ENABLE_INTERLAKENr(unit, reg_val));

    switch(ilkn_config->retransmit.nof_seq_number_repetitions_rx) {
        case 1:
            retrans_multiply_rx = 0;
            break;
        case 2:
            retrans_multiply_rx = 1;
            break;
        case 4:
            retrans_multiply_rx = 2;
            break;
        case 8:
            retrans_multiply_rx = 3;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid nof_seq_number_repetitions value %d"),ilkn_config->retransmit.nof_seq_number_repetitions_rx));
            break;
    }

    switch(ilkn_config->retransmit.nof_seq_number_repetitions_tx) {
        case 1:
            retrans_multiply_tx = 0;
            break;
        case 2:
            retrans_multiply_tx = 1;
            break;
        case 4:
            retrans_multiply_tx = 2;
            break;
        case 8:
            retrans_multiply_tx = 3;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid nof_seq_number_repetitions value %d"),ilkn_config->retransmit.nof_seq_number_repetitions_rx));
            break;
    }

     switch(offset) {
         case 0:
             
             SOCDNX_IF_ERR_EXIT(READ_NBI_ILKN_RX_0_RETRANSMIT_CONFIGr(unit, &reg_val));
             soc_reg_field_set(unit, NBI_ILKN_RX_0_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_0_RETRANS_ENABLEf, ilkn_config->retransmit.enable_rx);
             soc_reg_field_set(unit, NBI_ILKN_RX_0_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_0_RETRANS_RETRYf, ilkn_config->retransmit.nof_requests_resent);
             soc_reg_field_set(unit, NBI_ILKN_RX_0_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_0_RETRANS_MULTIPLYf, retrans_multiply_rx);
             soc_reg_field_set(unit, NBI_ILKN_RX_0_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_0_RETRANS_RESET_WHEN_ERRORf, ilkn_config->retransmit.rx_reset_when_error );
             soc_reg_field_set(unit, NBI_ILKN_RX_0_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_0_RETRANS_RESET_WHEN_WDOG_ERRORf, ilkn_config->retransmit.rx_reset_when_watchdog_err );
             soc_reg_field_set(unit, NBI_ILKN_RX_0_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_0_RETRANS_RESET_WHEN_ALIGNED_ERRORf, ilkn_config->retransmit.rx_reset_when_alligned_error);
             soc_reg_field_set(unit, NBI_ILKN_RX_0_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_0_RETRANS_RESET_WHEN_RETRY_ERRORf, ilkn_config->retransmit.rx_reset_when_retry_error);
             soc_reg_field_set(unit, NBI_ILKN_RX_0_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_0_RETRANS_RESET_WHEN_WRAP_AFTER_DISC_ERRORf, ilkn_config->retransmit.rx_reset_when_wrap_after_disc_error);
             soc_reg_field_set(unit, NBI_ILKN_RX_0_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_0_RETRANS_RESET_WHEN_TIMEOUT_ERRORf, ilkn_config->retransmit.rx_reset_when_timout_error);
             soc_reg_field_set(unit, NBI_ILKN_RX_0_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_0_RETRANS_RESET_WHEN_WRAP_B_4_DISC_ERRORf, ilkn_config->retransmit.rx_reset_when_wrap_before_disc_error);
             SOCDNX_IF_ERR_EXIT(WRITE_NBI_ILKN_RX_0_RETRANSMIT_CONFIGr(unit, reg_val));

            

             
             SOCDNX_IF_ERR_EXIT(READ_NBI_ILKN_RX_0_RETRANSMIT_CONFIG_TIMERSr(unit, &reg_val64));
             soc_reg64_field32_set(unit, NBI_ILKN_RX_0_RETRANSMIT_CONFIG_TIMERSr, &reg_val64, ILKN_RX_0_RETRANS_TIMEOUTf, ilkn_config->retransmit.rx_timeout_words);
             soc_reg64_field32_set(unit, NBI_ILKN_RX_0_RETRANSMIT_CONFIG_TIMERSr, &reg_val64, ILKN_RX_0_RETRANS_TIMEOUTf, ilkn_config->retransmit.rx_timeout_sn);
             soc_reg64_field32_set(unit, NBI_ILKN_RX_0_RETRANSMIT_CONFIG_TIMERSr, &reg_val64, ILKN_RX_0_RETRANS_SHORT_TIMERf, ilkn_config->retransmit.rx_ignore);
             soc_reg64_field32_set(unit, NBI_ILKN_RX_0_RETRANSMIT_CONFIG_TIMERSr, &reg_val64, ILKN_RX_0_RETRANS_WDOGf, ilkn_config->retransmit.rx_watchdog);
             SOCDNX_IF_ERR_EXIT(WRITE_NBI_ILKN_RX_0_RETRANSMIT_CONFIG_TIMERSr(unit, reg_val64));

             
             SOCDNX_IF_ERR_EXIT(READ_NBI_ILKN_TX_0_RETRANSMIT_CONFIGr(unit, &reg_val));
             soc_reg_field_set(unit, NBI_ILKN_TX_0_RETRANSMIT_CONFIGr, &reg_val, ILKN_TX_0_RETRANS_ENABLEf, ilkn_config->retransmit.enable_tx);
             soc_reg_field_set(unit, NBI_ILKN_TX_0_RETRANSMIT_CONFIGr, &reg_val, ILKN_TX_0_RETRANS_MULTIPLYf, retrans_multiply_tx);
             soc_reg_field_set(unit, NBI_ILKN_TX_0_RETRANSMIT_CONFIGr, &reg_val, ILKN_TX_0_RETRANS_NUM_ENTRIES_TO_SAVEf, ilkn_config->retransmit.buffer_size_entries);
             soc_reg_field_set(unit, NBI_ILKN_TX_0_RETRANSMIT_CONFIGr, &reg_val, ILKN_TX_0_RETRANS_WAIT_FOR_SEQ_NUM_CHANGEf, ilkn_config->retransmit.tx_wait_for_seq_num_change);
             soc_reg_field_set(unit, NBI_ILKN_TX_0_RETRANSMIT_CONFIGr, &reg_val, ILKN_TX_0_RETRANS_IGNORE_RETRANS_REQ_WHEN_FIFO_ALMOST_EMPTYf, ilkn_config->retransmit.tx_ignore_requests_when_fifo_almost_empty);
             SOCDNX_IF_ERR_EXIT(WRITE_NBI_ILKN_TX_0_RETRANSMIT_CONFIGr(unit, reg_val));

             break;
         case 1:
             
             SOCDNX_IF_ERR_EXIT(READ_NBI_ILKN_RX_1_RETRANSMIT_CONFIGr(unit, &reg_val));
             soc_reg_field_set(unit, NBI_ILKN_RX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_1_RETRANS_ENABLEf, ilkn_config->retransmit.enable_rx);
             soc_reg_field_set(unit, NBI_ILKN_RX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_1_RETRANS_RETRYf, ilkn_config->retransmit.nof_requests_resent);
             soc_reg_field_set(unit, NBI_ILKN_RX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_1_RETRANS_MULTIPLYf, retrans_multiply_rx);
             soc_reg_field_set(unit, NBI_ILKN_RX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_1_RETRANS_RESET_WHEN_WDOG_ERRORf, ilkn_config->retransmit.rx_reset_when_watchdog_err );
             soc_reg_field_set(unit, NBI_ILKN_RX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_1_RETRANS_RESET_WHEN_ERRORf, ilkn_config->retransmit.rx_reset_when_error );
             soc_reg_field_set(unit, NBI_ILKN_RX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_1_RETRANS_RESET_WHEN_WDOG_ERRORf, ilkn_config->retransmit.rx_reset_when_watchdog_err );
             soc_reg_field_set(unit, NBI_ILKN_RX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_1_RETRANS_RESET_WHEN_ALIGNED_ERRORf, ilkn_config->retransmit.rx_reset_when_alligned_error);
             soc_reg_field_set(unit, NBI_ILKN_RX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_1_RETRANS_RESET_WHEN_RETRY_ERRORf, ilkn_config->retransmit.rx_reset_when_retry_error);
             soc_reg_field_set(unit, NBI_ILKN_RX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_1_RETRANS_RESET_WHEN_WRAP_AFTER_DISC_ERRORf, ilkn_config->retransmit.rx_reset_when_wrap_after_disc_error);
             soc_reg_field_set(unit, NBI_ILKN_RX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_1_RETRANS_RESET_WHEN_TIMEOUT_ERRORf, ilkn_config->retransmit.rx_reset_when_timout_error);
             soc_reg_field_set(unit, NBI_ILKN_RX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_RX_1_RETRANS_RESET_WHEN_WRAP_B_4_DISC_ERRORf, ilkn_config->retransmit.rx_reset_when_wrap_before_disc_error);
             SOCDNX_IF_ERR_EXIT(WRITE_NBI_ILKN_RX_1_RETRANSMIT_CONFIGr(unit, reg_val));

             
             SOCDNX_IF_ERR_EXIT(READ_NBI_ILKN_RX_1_RETRANSMIT_CONFIG_TIMERSr(unit, &reg_val64));
             soc_reg64_field32_set(unit, NBI_ILKN_RX_1_RETRANSMIT_CONFIG_TIMERSr, &reg_val64, ILKN_RX_1_RETRANS_TIMEOUTf, ilkn_config->retransmit.rx_timeout_words);
             soc_reg64_field32_set(unit, NBI_ILKN_RX_1_RETRANSMIT_CONFIG_TIMERSr, &reg_val64, ILKN_RX_1_RETRANS_TIMEOUTf, ilkn_config->retransmit.rx_timeout_sn);
             soc_reg64_field32_set(unit, NBI_ILKN_RX_1_RETRANSMIT_CONFIG_TIMERSr, &reg_val64, ILKN_RX_1_RETRANS_SHORT_TIMERf, ilkn_config->retransmit.rx_ignore);
             soc_reg64_field32_set(unit, NBI_ILKN_RX_1_RETRANSMIT_CONFIG_TIMERSr, &reg_val64, ILKN_RX_1_RETRANS_WDOGf, ilkn_config->retransmit.rx_watchdog);
             SOCDNX_IF_ERR_EXIT(WRITE_NBI_ILKN_RX_1_RETRANSMIT_CONFIG_TIMERSr(unit, reg_val64));

             
             SOCDNX_IF_ERR_EXIT(READ_NBI_ILKN_TX_1_RETRANSMIT_CONFIGr(unit, &reg_val));
             soc_reg_field_set(unit, NBI_ILKN_TX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_TX_1_RETRANS_ENABLEf, ilkn_config->retransmit.enable_tx);
             soc_reg_field_set(unit, NBI_ILKN_TX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_TX_1_RETRANS_MULTIPLYf, retrans_multiply_tx);
             soc_reg_field_set(unit, NBI_ILKN_TX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_TX_1_RETRANS_NUM_ENTRIES_TO_SAVEf, ilkn_config->retransmit.buffer_size_entries);
             soc_reg_field_set(unit, NBI_ILKN_TX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_TX_1_RETRANS_WAIT_FOR_SEQ_NUM_CHANGEf, ilkn_config->retransmit.tx_wait_for_seq_num_change);
             soc_reg_field_set(unit, NBI_ILKN_TX_1_RETRANSMIT_CONFIGr, &reg_val, ILKN_TX_1_RETRANS_IGNORE_RETRANS_REQ_WHEN_FIFO_ALMOST_EMPTYf, ilkn_config->retransmit.tx_ignore_requests_when_fifo_almost_empty);
             SOCDNX_IF_ERR_EXIT(WRITE_NBI_ILKN_TX_1_RETRANSMIT_CONFIGr(unit, reg_val));
             break;

         default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong ILKN id %d"),offset));
            break;
     }


exit:
    SOCDNX_FUNC_RETURN; 
}


STATIC soc_error_t 
soc_ilkn_drv_reset(int unit, soc_port_t port, uint32 enable)
{
    uint32 reg_val, offset, flags = 0;
    uint64            field64;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));

    SOCDNX_IF_ERR_EXIT(READ_NBI_ILKN_RESETr(unit, &reg_val));
    
    soc_reg_field_set(unit, NBI_ILKN_RESETr, &reg_val, (offset == 0) ? ILKN_RX_0_PORT_RSTNf:ILKN_RX_1_PORT_RSTNf, enable);
    soc_reg_field_set(unit, NBI_ILKN_RESETr, &reg_val, (offset == 0) ? ILKN_TX_0_PORT_RSTNf:ILKN_TX_1_PORT_RSTNf, enable);

    if(!SOC_PORT_IS_ELK_INTERFACE(flags)){
        
        soc_reg_field_set(unit, NBI_ILKN_RESETr, &reg_val, (offset == 0) ? HRF_RX_0_CONTROLLER_RSTNf:HRF_RX_1_CONTROLLER_RSTNf, enable);
        soc_reg_field_set(unit, NBI_ILKN_RESETr, &reg_val, (offset == 0) ? HRF_TX_0_CONTROLLER_RSTNf:HRF_TX_1_CONTROLLER_RSTNf, enable);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_ILKN_RESETr(unit, reg_val));

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_mgmt_nbi_ecc_mask_get(unit, &field64));
    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, NBI_ECC_ERR_1B_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, field64));
    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, NBI_ECC_ERR_2B_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, field64));

exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t 
soc_ilkn_drv_init(int unit, soc_port_t port)
{
    uint32 reg_val32, num_lanes, clk_src, fld_val[1], phy_port, no_frag, flags, offset;
    uint64 reg_val64;
    soc_reg_above_64_val_t reg_above_64;
    soc_field_t field;
    uint32 hrf_for_ilkn;
    ARAD_PORTS_ILKN_CONFIG *ilkn_config;
    int field_len;

    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));

    if (phy_port == 0) {
    	SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ILKN DRV doesn't support CPU port")));
    }


    ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[offset];
    
    SOCDNX_IF_ERR_EXIT(soc_common_drv_bypass_mode_set(unit,port,1));

    
    if(1 == offset) {
        if(num_lanes <= 4) {
            clk_src = 0x6;
        } else if(num_lanes <= 8){
            clk_src = 0x4;
        } else {
            clk_src = 0x0;
        } 
        SOCDNX_IF_ERR_EXIT(READ_NBI_ENABLE_INTERLAKENr(unit, &reg_val32));
        soc_reg_field_set(unit, NBI_ENABLE_INTERLAKENr, &reg_val32, ILKN_TX_EXTERNAL_CLOCK_SELf, clk_src);
        SOCDNX_IF_ERR_EXIT(WRITE_NBI_ENABLE_INTERLAKENr(unit, reg_val32));
    }

    SOCDNX_IF_ERR_EXIT(soc_ilkn_drv_reset(unit, port, 0));
    
    SOCDNX_IF_ERR_EXIT(READ_NBI_ENABLE_INTERLAKENr(unit, &reg_val32));
    if(SOC_PORT_IS_ELK_INTERFACE(flags)) {
        hrf_for_ilkn = 0;
    } else { 
        hrf_for_ilkn = 1;
        soc_reg_field_set(unit, NBI_ENABLE_INTERLAKENr, &reg_val32, (offset == 0) ? ENABLE_HRF_0f:ENABLE_HRF_1f, 1);
        if(num_lanes >= 9) {
            soc_reg_field_set(unit, NBI_ENABLE_INTERLAKENr, &reg_val32, (offset == 0) ? ILKN_TX_0_USE_EXTENDED_MEMORYf:ILKN_TX_1_USE_EXTENDED_MEMORYf, 1);
        }
    }
    soc_reg_field_set(unit, NBI_ENABLE_INTERLAKENr, &reg_val32, (offset == 0) ? USE_HRF_FOR_ILKN_0f:USE_HRF_FOR_ILKN_1f, hrf_for_ilkn);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_ENABLE_INTERLAKENr(unit, reg_val32));

    if(!SOC_PORT_IS_ELK_INTERFACE(flags)) {
        SOCDNX_IF_ERR_EXIT(READ_NBI_HRF_TX_CONTROLLER_CONFIGr(unit, offset, &reg_val64));
        soc_reg64_field32_set(unit, NBI_HRF_TX_CONTROLLER_CONFIGr, &reg_val64, HRF_TX_N__512B_IN_2_CLKSf, (offset == 0) ? 0 : 1);
        soc_reg64_field32_set(unit, NBI_HRF_TX_CONTROLLER_CONFIGr, &reg_val64, HRF_TX_N_NUM_CREDITS_TO_EGQf, ilkn_config->retransmit.enable_tx ? 0x180 : 0x100);
        SOCDNX_IF_ERR_EXIT(WRITE_NBI_HRF_TX_CONTROLLER_CONFIGr(unit, offset, reg_val64));
    }
    
    SOCDNX_IF_ERR_EXIT(READ_NBI_FC_ILKNr(unit, &reg_val32));
    reg_val32 |= (1<<offset);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_FC_ILKNr(unit, reg_val32));

    
    if(!SOC_PORT_IS_ELK_INTERFACE(flags)) {
        SOCDNX_IF_ERR_EXIT(READ_NBI_RX_MLF_RESET_PORTSr(unit, &reg_val32));
        *fld_val = soc_reg_field_get(unit, NBI_RX_MLF_RESET_PORTSr, reg_val32, RX_PORTS_SRSTNf);
        
        SHR_BITCLR(fld_val, (phy_port-1));
        soc_reg_field_set(unit, NBI_RX_MLF_RESET_PORTSr, &reg_val32, RX_PORTS_SRSTNf, *fld_val);
        SOCDNX_IF_ERR_EXIT(WRITE_NBI_RX_MLF_RESET_PORTSr(unit, reg_val32));

        SOCDNX_IF_ERR_EXIT(READ_NBI_TX_MLF_RESET_PORTSr(unit, &reg_val32));
        *fld_val = soc_reg_field_get(unit, NBI_TX_MLF_RESET_PORTSr, reg_val32, TX_PORTS_SRSTNf);
        
        SHR_BITCLR(fld_val, (phy_port-1));
        soc_reg_field_set(unit, NBI_TX_MLF_RESET_PORTSr, &reg_val32, TX_PORTS_SRSTNf, *fld_val);
        SOCDNX_IF_ERR_EXIT(WRITE_NBI_TX_MLF_RESET_PORTSr(unit, reg_val32));
    }
    
    SOCDNX_IF_ERR_EXIT(soc_common_drv_fast_port_set(unit, port, 1, 1));

    
    switch (offset) {
        case 0:
            SOCDNX_IF_ERR_EXIT(READ_NBI_RX_0_ILKN_CONTROLr(unit, &reg_val32));
            soc_reg_field_set(unit, NBI_RX_0_ILKN_CONTROLr, &reg_val32, RX_0_FULL_PACKET_MODEf,  0);
            soc_reg_field_set(unit, NBI_RX_0_ILKN_CONTROLr, &reg_val32, RX_0_BURSTMAXf, NIF_ILKN_BURST_MAX_256B_VAL);
            soc_reg_field_set(unit, NBI_RX_0_ILKN_CONTROLr, &reg_val32, RX_0_MFRAME_LEN_MINUS_1f, (ilkn_config->metaframe_sync_period == 0)?0x0:(ilkn_config->metaframe_sync_period - 1));
            soc_reg_field_set(unit, NBI_RX_0_ILKN_CONTROLr, &reg_val32, RX_0_LAST_LANEf, num_lanes - 1);
            SOCDNX_IF_ERR_EXIT(WRITE_NBI_RX_0_ILKN_CONTROLr(unit, reg_val32));
            break;
        case 1:
            SOCDNX_IF_ERR_EXIT(READ_NBI_RX_1_ILKN_CONTROLr(unit, &reg_val32));
            soc_reg_field_set(unit, NBI_RX_1_ILKN_CONTROLr, &reg_val32, RX_1_FULL_PACKET_MODEf, 0);
            soc_reg_field_set(unit, NBI_RX_1_ILKN_CONTROLr, &reg_val32, RX_1_BURSTMAXf, NIF_ILKN_BURST_MAX_256B_VAL);
            soc_reg_field_set(unit, NBI_RX_1_ILKN_CONTROLr, &reg_val32, RX_1_MFRAME_LEN_MINUS_1f, (ilkn_config->metaframe_sync_period == 0)?0x0:(ilkn_config->metaframe_sync_period - 1));
            soc_reg_field_set(unit, NBI_RX_1_ILKN_CONTROLr, &reg_val32, RX_1_LAST_LANEf, num_lanes - 1);
            SOCDNX_IF_ERR_EXIT(WRITE_NBI_RX_1_ILKN_CONTROLr(unit, reg_val32));
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong ILKN id %d"),offset));
            break;
    }

    
    switch (offset) {
        case 0:
            SOCDNX_IF_ERR_EXIT(READ_NBI_TX_0_ILKN_CONTROLr(unit, &reg_val64));
             soc_reg64_field32_set(unit, NBI_TX_0_ILKN_CONTROLr, &reg_val64, TX_0_BURSTMAXf, NIF_ILKN_BURST_MAX_256B_VAL);
             soc_reg64_field32_set(unit, NBI_TX_0_ILKN_CONTROLr, &reg_val64, TX_0_BURSTSHORTf, NIF_ILKN_TX_BURST_SHORT_64B_VAL);
             soc_reg64_field32_set(unit, NBI_TX_0_ILKN_CONTROLr, &reg_val64, TX_0_MFRAME_LEN_MINUS_1f, (ilkn_config->metaframe_sync_period == 0)?0x0:(ilkn_config->metaframe_sync_period - 1));
             soc_reg64_field32_set(unit, NBI_TX_0_ILKN_CONTROLr, &reg_val64, TX_0_LAST_LANEf, num_lanes - 1);
             soc_reg64_field32_set(unit, NBI_TX_0_ILKN_CONTROLr, &reg_val64, TX_0_NUM_FREE_ENTRIES_FOR_ALMOST_FULLf, NIF_ILKN_FREE_ENTRIES_VAL);
            if(ilkn_config->interfcae_status_ignore) {
                soc_reg64_field32_set(unit, NBI_TX_0_ILKN_CONTROLr, &reg_val64, TX_0_LP_IFC_STAT_IGNOREf, 1);
                soc_reg64_field32_set(unit, NBI_TX_0_ILKN_CONTROLr, &reg_val64, TX_0_ENABLE_CPU_VALUEf, 1);
                soc_reg64_field32_set(unit, NBI_TX_0_ILKN_CONTROLr, &reg_val64, TX_0_ENABLE_CPU_OVERRIDEf, 1);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_NBI_TX_0_ILKN_CONTROLr(unit, reg_val64));

            break;
        case 1:
             SOCDNX_IF_ERR_EXIT(READ_NBI_TX_1_ILKN_CONTROLr(unit, &reg_val64));
             soc_reg64_field32_set(unit, NBI_TX_1_ILKN_CONTROLr, &reg_val64, TX_1_BURSTMAXf, NIF_ILKN_BURST_MAX_256B_VAL);
             soc_reg64_field32_set(unit, NBI_TX_1_ILKN_CONTROLr, &reg_val64, TX_1_BURSTSHORTf, NIF_ILKN_TX_BURST_SHORT_64B_VAL);
             soc_reg64_field32_set(unit, NBI_TX_1_ILKN_CONTROLr, &reg_val64, TX_1_MFRAME_LEN_MINUS_1f, (ilkn_config->metaframe_sync_period == 0)?0x0:(ilkn_config->metaframe_sync_period - 1));
             soc_reg64_field32_set(unit, NBI_TX_1_ILKN_CONTROLr, &reg_val64, TX_1_LAST_LANEf, num_lanes - 1);
             soc_reg64_field32_set(unit, NBI_TX_1_ILKN_CONTROLr, &reg_val64, TX_1_NUM_FREE_ENTRIES_FOR_ALMOST_FULLf, NIF_ILKN_FREE_ENTRIES_VAL);
            if(ilkn_config->interfcae_status_ignore) {
                soc_reg64_field32_set(unit, NBI_TX_1_ILKN_CONTROLr, &reg_val64, TX_1_LP_IFC_STAT_IGNOREf, 1);
                soc_reg64_field32_set(unit, NBI_TX_1_ILKN_CONTROLr, &reg_val64, TX_1_ENABLE_CPU_VALUEf, 1);
                soc_reg64_field32_set(unit, NBI_TX_1_ILKN_CONTROLr, &reg_val64, TX_1_ENABLE_CPU_OVERRIDEf, 1);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_NBI_TX_1_ILKN_CONTROLr(unit, reg_val64));
            break;
        
        
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong ILKN id %d"),offset));
            break;
    }



    
    no_frag = 0;
    SOCDNX_IF_ERR_EXIT(READ_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, REG_PORT_ANY, reg_above_64));
    field = SOC_IS_JERICHO(unit) ? NIF_NO_FRAG_Lf : NIF_NO_FRAGf;

    
    field_len = soc_reg_field_length(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, field);
    if ((phy_port-1) >= field_len) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Port %d is invalid phy %d"),port, phy_port));
    }

    *fld_val = soc_reg_above_64_field32_get(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, reg_above_64, field);
    
    SHR_BITCLR(fld_val, (phy_port-1));
    
    *fld_val |= (no_frag<<(phy_port-1));
    soc_reg_above_64_field32_set(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, reg_above_64, field, *fld_val);
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, REG_PORT_ANY, reg_above_64));

    
    SOCDNX_IF_ERR_EXIT(READ_NBI_STATISTICSr(unit, &reg_val32));
    soc_reg_field_set(unit, NBI_STATISTICSr, &reg_val32, (offset == 0) ? STAT_ILKN_0_CNT_PER_CHf:STAT_ILKN_1_CNT_PER_CHf,1);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_STATISTICSr(unit, reg_val32));

    
    SOCDNX_IF_ERR_EXIT(soc_ilkn_drv_retransmit_config(unit, port));

    
    SOCDNX_IF_ERR_EXIT(soc_ilkn_drv_reset(unit, port, 1));

    
    SOCDNX_IF_ERR_EXIT(READ_PORT_ENABLE_REGr(unit, port, &reg_val32));
    soc_reg_field_set(unit, PORT_ENABLE_REGr, &reg_val32, PORT_0f , 1);
    SOCDNX_IF_ERR_EXIT(WRITE_PORT_ENABLE_REGr(unit, port, reg_val32));
    
    
    if (SOC_IS_ARAD_A0(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_NBI_MASK_ECC_PARITY_ERROR_FROM_DATA_PATHr(unit, &reg_val32));
        soc_reg_field_set(unit, NBI_MASK_ECC_PARITY_ERROR_FROM_DATA_PATHr, &reg_val32, RX_MASK_PAR_ERR_DATA_MEMf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_NBI_MASK_ECC_PARITY_ERROR_FROM_DATA_PATHr(unit, reg_val32));
    }

exit:
    SOCDNX_FUNC_RETURN; 
}



soc_error_t
soc_ilkn_drv_num_of_lanes_get(int unit, soc_port_t port,  uint32 *num_of_lanes){
    uint32 reg_val32, offset;
    soc_reg_t rx_ilkn_control_reg;
    soc_field_t rx_field;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    switch(offset){
    case 0:
        rx_ilkn_control_reg = NBI_RX_0_ILKN_CONTROLr;
        rx_field = RX_0_LAST_LANEf;
        break;
    case 1:
        rx_ilkn_control_reg = NBI_RX_1_ILKN_CONTROLr;
        rx_field = RX_1_LAST_LANEf;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong ILKN id %d"),offset));
        break;
    }
    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, rx_ilkn_control_reg, REG_PORT_ANY, 0, &reg_val32));
    *num_of_lanes = soc_reg_field_get(unit, rx_ilkn_control_reg, reg_val32, rx_field);
    *num_of_lanes+=1;
exit:
    SOCDNX_FUNC_RETURN
}


soc_error_t
soc_ilkn_drv_num_of_lanes_set(int unit, soc_port_t port,  uint32 num_of_lanes)
{
    uint32 reg_val32, offset;
    uint64 reg_val64;
    soc_reg_t rx_ilkn_control_reg, tx_ilkn_control_reg;
    soc_field_t rx_field, tx_field;
    uint32 init_lanes_num;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &init_lanes_num));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    if(init_lanes_num > 12){
          SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("unsupported change of ILKN num of lanes for ILKN with more than 12 lanes")));
    }
    if(num_of_lanes > init_lanes_num){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Can't increase the ILKN lanes number above the init lanes number %d"),init_lanes_num));
    }
    switch(offset){
    case 0:
        rx_ilkn_control_reg = NBI_RX_0_ILKN_CONTROLr;
        tx_ilkn_control_reg = NBI_TX_0_ILKN_CONTROLr;
        rx_field = RX_0_LAST_LANEf;
        tx_field = TX_0_LAST_LANEf;
        break;
    case 1:
        rx_ilkn_control_reg = NBI_RX_1_ILKN_CONTROLr;
        tx_ilkn_control_reg = NBI_TX_1_ILKN_CONTROLr;
        rx_field = RX_1_LAST_LANEf;
        tx_field = TX_1_LAST_LANEf;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong ILKN id %d"),offset));
        break;
    }
    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, rx_ilkn_control_reg, REG_PORT_ANY, 0, &reg_val32));
    soc_reg_field_set(unit, rx_ilkn_control_reg, &reg_val32, rx_field, num_of_lanes - 1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, rx_ilkn_control_reg, REG_PORT_ANY, 0, reg_val32));

    SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, tx_ilkn_control_reg, REG_PORT_ANY, 0, &reg_val64));
    soc_reg64_field32_set(unit, tx_ilkn_control_reg, &reg_val64, tx_field, num_of_lanes - 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, tx_ilkn_control_reg, REG_PORT_ANY, 0, reg_val64)); 
exit:
    SOCDNX_FUNC_RETURN
}


STATIC int
soc_ilkn_drv_enable_set(int unit, soc_port_t port, int enable)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_ilkn_drv_reset(unit, port, enable));

exit:
    SOCDNX_FUNC_RETURN
}


STATIC int
soc_ilkn_drv_speed_set(int unit, soc_port_t port, int speed)
{
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_FUNC_RETURN
}


STATIC int
soc_ilkn_drv_loopback_set(int unit, soc_port_t port, int lb)
{
    SOCDNX_INIT_FUNC_DEFS

    if(lb){
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("MAC loopback isn't supported for ILKN port")));
    }

exit:
    SOCDNX_FUNC_RETURN
}


STATIC int
soc_ilkn_drv_loopback_get(int unit, soc_port_t port, int *lb)
{
    SOCDNX_INIT_FUNC_DEFS

    *lb = 0;

    SOCDNX_FUNC_RETURN
}


soc_error_t
soc_ilkn_link_status_get(int unit, soc_port_t port, int *is_link_up, int *is_latch_down)
{
    uint32 is_locked, reg32_val, latch_down, offset;
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    SOCDNX_IF_ERR_EXIT(READ_NBI_RX_ILKN_STATUSr(unit, offset, &reg32_val));
    is_locked = soc_reg_field_get(unit, NBI_RX_ILKN_STATUSr, reg32_val, RX_N_STAT_ALIGNED_RAWf);
    (*is_link_up) = (is_locked ? 1 : 0);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_latch_down_get(unit, port, is_latch_down));
    if (!(*is_latch_down) && (*is_link_up)) {
        latch_down = soc_reg_field_get(unit, NBI_RX_ILKN_STATUSr, reg32_val, RX_N_STAT_ALIGNED_LATCH_LOWf);
        (*is_latch_down) = (latch_down ? 0 : 1);            
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_latch_down_set(unit, port, *is_latch_down));
    } 
    
exit:
    SOCDNX_FUNC_RETURN
}


soc_error_t
soc_ilkn_link_status_clear(int unit, soc_port_t port)
{
    uint32 reg32_val, offset;
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    
    SOCDNX_IF_ERR_EXIT(READ_NBI_RX_ILKN_STATUSr(unit, offset, &reg32_val));

exit:
    SOCDNX_FUNC_RETURN
}

CONST mac_driver_t soc_ilkn_driver = {
    "DPP ILKN Driver",              
    soc_ilkn_drv_init,              
    soc_ilkn_drv_enable_set,        
    NULL,                           
    NULL,                           
    NULL,                           
    soc_ilkn_drv_speed_set,         
    NULL,                           
    NULL,                           
    NULL,                           
    NULL,                           
    NULL,                           
    soc_ilkn_drv_loopback_set,      
    soc_ilkn_drv_loopback_get,      
    NULL,                           
    NULL,                           
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

#undef _ERR_MSG_MODULE_NAME


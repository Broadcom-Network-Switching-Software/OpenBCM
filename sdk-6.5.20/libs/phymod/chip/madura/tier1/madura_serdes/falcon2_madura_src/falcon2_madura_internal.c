/***********************************************************************************
 ***********************************************************************************
 *  File Name     :  falcon2_madura_internal.c                                         *
 *  Created On    :  13/02/2014                                                    *
 *  Created By    :  Justin Gaither                                                *
 *  Description   :  APIs for Serdes IPs                                           *
 *  Revision      :  $Id: falcon2_madura_internal.c 1377 2016-03-14 23:19:21Z cvazquez $ *
 *                                                                                 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                           *
 *  No portions of this material may be reproduced in any form without             *
 *  the written permission of:                                                     *
 *      Broadcom Corporation                                                       *
 *      5300 California Avenue                                                     *
 *      Irvine, CA  92617                                                          *
 *                                                                                 *
 *  All information contained in this document is Broadcom Corporation             *
 *  company private proprietary, and trade secret.                                 *
 */

/** @file falcon2_madura_internal.c
 * Implementation of API functions
 */

#include "falcon2_madura_internal.h"

SDK_STATIC uint32_t _mult_with_overflow_check(uint32_t a, uint32_t b, uint8_t *of) {
    uint16_t c,d;
    uint32_t r,s;
    if (a > b) return _mult_with_overflow_check(b, a, of);
    *of = 0;
    c = b >> 16;
    d = UINT16_MAX & b;
    r = a * c;
    s = a * d;
    if (r > UINT16_MAX) *of = 1;
    r <<= 16;
    return (s + r);
}

#ifdef MADURA_UNTESTED_CODE
SDK_STATIC char* _e2s_err_code(err_code_t err_code)
{
        switch(err_code){
        case ERR_CODE_NONE: return "ERR_CODE_NONE";
        case ERR_CODE_INVALID_RAM_ADDR: return "ERR_CODE_INVALID_RAM_ADDR";
        case ERR_CODE_SERDES_DELAY: return "ERR_CODE_SERDES_DELAY";
        case ERR_CODE_POLLING_TIMEOUT: return "ERR_CODE_POLLING_TIMEOUT";
        case ERR_CODE_CFG_PATT_INVALID_PATTERN: return "ERR_CODE_CFG_PATT_INVALID_PATTERN";
        case ERR_CODE_CFG_PATT_INVALID_PATT_LENGTH: return "ERR_CODE_CFG_PATT_INVALID_PATT_LENGTH";
        case ERR_CODE_CFG_PATT_LEN_MISMATCH: return "ERR_CODE_CFG_PATT_LEN_MISMATCH";
        case ERR_CODE_CFG_PATT_PATTERN_BIGGER_THAN_MAXLEN: return "ERR_CODE_CFG_PATT_PATTERN_BIGGER_THAN_MAXLEN";
        case ERR_CODE_CFG_PATT_INVALID_HEX: return "ERR_CODE_CFG_PATT_INVALID_HEX";
        case ERR_CODE_CFG_PATT_INVALID_BIN2HEX: return "ERR_CODE_CFG_PATT_INVALID_BIN2HEX";
        case ERR_CODE_CFG_PATT_INVALID_SEQ_WRITE: return "ERR_CODE_CFG_PATT_INVALID_SEQ_WRITE";
        case ERR_CODE_PATT_GEN_INVALID_MODE_SEL: return "ERR_CODE_PATT_GEN_INVALID_MODE_SEL";
        case ERR_CODE_INVALID_UCODE_LEN: return "ERR_CODE_INVALID_UCODE_LEN";
        case ERR_CODE_MICRO_INIT_NOT_DONE: return "ERR_CODE_MICRO_INIT_NOT_DONE";
        case ERR_CODE_UCODE_LOAD_FAIL: return "ERR_CODE_UCODE_LOAD_FAIL";
        case ERR_CODE_UCODE_VERIFY_FAIL: return "ERR_CODE_UCODE_VERIFY_FAIL";
        case ERR_CODE_INVALID_TEMP_IDX: return "ERR_CODE_INVALID_TEMP_IDX";
        case ERR_CODE_INVALID_PLL_CFG: return "ERR_CODE_INVALID_PLL_CFG";
        case ERR_CODE_TX_HPF_INVALID: return "ERR_CODE_TX_HPF_INVALID";
        case ERR_CODE_VGA_INVALID: return "ERR_CODE_VGA_INVALID";
        case ERR_CODE_PF_INVALID: return "ERR_CODE_PF_INVALID";
        case ERR_CODE_TX_AMP_CTRL_INVALID: return "ERR_CODE_TX_AMP_CTRL_INVALID";
        case ERR_CODE_INVALID_EVENT_LOG_WRITE: return "ERR_CODE_INVALID_EVENT_LOG_WRITE";
        case ERR_CODE_INVALID_EVENT_LOG_READ: return "ERR_CODE_INVALID_EVENT_LOG_READ";
        case ERR_CODE_UC_CMD_RETURN_ERROR: return "ERR_CODE_UC_CMD_RETURN_ERROR";
        case ERR_CODE_DATA_NOTAVAIL: return "ERR_CODE_DATA_NOTAVAIL";
        case ERR_CODE_BAD_PTR_OR_INVALID_INPUT: return "ERR_CODE_BAD_PTR_OR_INVALID_INPUT";
        case ERR_CODE_UC_NOT_STOPPED: return "ERR_CODE_UC_NOT_STOPPED";
        case ERR_CODE_UC_CRC_NOT_MATCH: return "ERR_CODE_UC_CRC_NOT_MATCH";
        case ERR_CODE_CORE_DP_NOT_RESET: return "ERR_CODE_CORE_DP_NOT_RESET";
        case ERR_CODE_LANE_DP_NOT_RESET: return "ERR_CODE_LANE_DP_NOT_RESET";
        case ERR_CODE_CONFLICTING_PARAMETERS: return "ERR_CODE_CONFLICTING_PARAMETERS";
        case ERR_CODE_BAD_LANE_COUNT: return "ERR_CODE_BAD_LANE_COUNT";
        case ERR_CODE_BAD_LANE: return "ERR_CODE_BAD_LANE";
        case ERR_CODE_UC_ACTIVE: return "ERR_CODE_UC_ACTIVE";
        default:{
            switch(err_code>>8){
            case 1: return "ERR_CODE_TXFIR";
            case 2: return "ERR_CODE_DFE_TAP";
            case 3: return "ERR_CODE_DIAG";
            default: return "Invalid error code";
            }
        }
        }
}

SDK_STATIC err_code_t _print_err_msg(uint16_t err_code)
{
    if (err_code != 0) {
        USR_PRINTF(("ERROR: SerDes err_code = %s\n", _e2s_err_code(err_code)));
    }
    return err_code;
}
#endif /* #ifdef MADURA_UNTESTED_CODE */
 
SDK_STATIC err_code_t _check_uc_lane_stopped( const phymod_access_t *pa ) {
  uint8_t is_micro_stopped;
  ESTM(is_micro_stopped = rdv_falcon2_madura_usr_sts_micro_stopped());
  if (!is_micro_stopped) {
      return(_error(ERR_CODE_UC_NOT_STOPPED));
  } else {
      return(ERR_CODE_NONE);
  }
}

/* Formula for PVTMON: T = 410.04-0.48705*reg10bit (from PVTMON Analog Module Specification v5.0 section 6.2) */
#define _bin_to_degC(bin) ((int16_t)(410 + ((2212 - 1995*(int32_t)bin)>>12)))




SDK_STATIC int16_t _ladder_setting_to_mV(const phymod_access_t *pa,int8_t ctrl, uint8_t range_250) {
    uint8_t absv;                                     /* Absolute value of ctrl */
    int16_t nlmv;                                     /* Non-linear value */

    /* Get absolute value */
    absv = _abs(ctrl);

    {
       (void)range_250;

       nlmv = absv*300/127;
    }

    /* Add sign and return */
    return( (ctrl>=0) ? nlmv : -nlmv);
}

err_code_t falcon2_madura_set_lms_hoffset (const phymod_access_t *pa, char delta) {
    if (delta != 0) {
        EFUN(wr_falcon2_madura_rx_pi_slicers_en    (      0x30 ));
        EFUN(wr_falcon2_madura_rx_pi_phase_step_cnt(         0 ));
        EFUN(wr_falcon2_madura_rx_pi_phase_step_dir((delta > 0)));
        {   uint8_t loop = (uint8_t)(_abs(delta));
            while  (loop-- > 0) {
                EFUN(wr_falcon2_madura_rx_pi_manual_strobe(1));
            }
        }
        /* Undo setup */
        EFUN(wr_falcon2_madura_rx_pi_slicers_en    (0x0));
        EFUN(wr_falcon2_madura_rx_pi_phase_step_cnt(  0));
        EFUN(wr_falcon2_madura_rx_pi_phase_step_dir(  0));
    }
    return(ERR_CODE_NONE);
}


err_code_t falcon2_madura_log_full_pmd_state ( const phymod_access_t *pa, struct falcon2_madura_detailed_lane_status_st *lane_st) {
    uint16_t reg_data;
    int8_t tmp;

    if(!lane_st)
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

    ESTM(lane_st->pmd_lock = rd_falcon2_madura_pmd_rx_lock());

    lane_st->stop_state = 0;
    if (lane_st->pmd_lock == 1) {
        ESTM(lane_st->stop_state = rdv_falcon2_madura_usr_sts_micro_stopped());
        if (!lane_st->stop_state) {
            EFUN(falcon2_madura_stop_rx_adaptation( pa, 1));
        }
    } else {
        EFUN(falcon2_madura_pmd_uc_control( pa, CMD_UC_CTRL_STOP_IMMEDIATE,200));
    }

    ESTM(lane_st->reset_state = rd_falcon2_madura_lane_dp_reset_state());

    ESTM(lane_st->pmd_pll_select = rd_falcon2_madura_pll_select());
    ESTM(lane_st->temp_idx = rdcv_falcon2_madura_temp_idx());
    ESTM(lane_st->ams_tx_drv_hv_disable = rd_falcon2_madura_ams_tx_drv_hv_disable());
    ESTM(lane_st->ams_tx_ana_rescal = rd_falcon2_madura_ams_tx_ana_rescal());
    EFUN(falcon2_madura_read_tx_afe(pa, TX_AFE_AMP, &tmp)); lane_st->amp_ctrl = (uint8_t) tmp;
    EFUN(falcon2_madura_read_tx_afe(pa, TX_AFE_PRE, &tmp)); lane_st->pre_tap = (uint8_t) tmp;
    EFUN(falcon2_madura_read_tx_afe(pa, TX_AFE_MAIN, &tmp)); lane_st->main_tap = (uint8_t) tmp;
    EFUN(falcon2_madura_read_tx_afe(pa, TX_AFE_POST1, &tmp)); lane_st->post1_tap = (uint8_t) tmp;
    EFUN(falcon2_madura_read_tx_afe(pa, TX_AFE_POST2, &lane_st->post2_tap));
    EFUN(falcon2_madura_read_tx_afe(pa, TX_AFE_POST3, &lane_st->post3_tap));
    ESTM(lane_st->sigdet = rd_falcon2_madura_signal_detect());
    ESTM(lane_st->dsc_sm[0] = rd_falcon2_madura_dsc_state_one_hot());
    ESTM(lane_st->dsc_sm[1] = rd_falcon2_madura_dsc_state_one_hot());
#ifdef SERDES_API_FLOATING_POINT
    ESTM(lane_st->ppm = (((double)1e6/64/20/128/16)*(int16_t)(rd_falcon2_madura_cdr_integ_reg()/32)));
#else
    ESTM(lane_st->ppm = ((int16_t)(rd_falcon2_madura_cdr_integ_reg())*12/1000));
#endif
    ESTM(lane_st->vga = rd_falcon2_madura_rx_vga_ctrl());
    ESTM(lane_st->pf = rd_falcon2_madura_rx_pf_ctrl());
    ESTM(lane_st->pf2 = rd_falcon2_madura_rx_pf2_ctrl());
#ifdef SERDES_API_FLOATING_POINT
    ESTM(lane_st->main_tap_est = rdv_falcon2_madura_usr_main_tap_est()/32.0);
#else
    ESTM(lane_st->main_tap_est = rdv_falcon2_madura_usr_main_tap_est()/32);
#endif
    ESTM(lane_st->data_thresh = rd_falcon2_madura_rx_data_thresh_sel());
    ESTM(lane_st->phase_thresh = rd_falcon2_madura_rx_phase_thresh_sel());
    ESTM(lane_st->lms_thresh = rd_falcon2_madura_rx_lms_thresh_sel());
    EFUN(falcon2_madura_pmd_rdt_reg(pa, DSC_E_RX_PI_CNT_BIN_D,&reg_data));
    lane_st->ddq_hoffset = (uint8_t)dist_ccw(((reg_data>>8)&0xFF),(reg_data&0xFF));
    EFUN(falcon2_madura_pmd_rdt_reg(pa,DSC_E_RX_PI_CNT_BIN_P,&reg_data));
    lane_st->ppq_hoffset = (uint8_t)dist_ccw(((reg_data>>8)&0xFF),(reg_data&0xFF));
    EFUN(falcon2_madura_pmd_rdt_reg(pa,DSC_E_RX_PI_CNT_BIN_L,&reg_data));
    lane_st->llq_hoffset = (uint8_t)dist_ccw(((reg_data>>8)&0xFF),(reg_data&0xFF));
    EFUN(falcon2_madura_pmd_rdt_reg(pa,DSC_E_RX_PI_CNT_BIN_PD,&reg_data));
    lane_st->dp_hoffset = (uint8_t)dist_cw(((reg_data>>8)&0xFF),(reg_data&0xFF));
    EFUN(falcon2_madura_pmd_rdt_reg(pa,DSC_E_RX_PI_CNT_BIN_LD,&reg_data));
    lane_st->dl_hoffset = (uint8_t)dist_cw(((reg_data>>8)&0xFF),(reg_data&0xFF));
    ESTM(lane_st->dc_offset = rd_falcon2_madura_dc_offset_bin());
    ESTM(lane_st->dfe[1][0] = rd_falcon2_madura_rxa_dfe_tap2());
    ESTM(lane_st->dfe[1][1] = rd_falcon2_madura_rxb_dfe_tap2());
    ESTM(lane_st->dfe[1][2] = rd_falcon2_madura_rxc_dfe_tap2());
    ESTM(lane_st->dfe[1][3] = rd_falcon2_madura_rxd_dfe_tap2());
    ESTM(lane_st->dfe[2][0] = rd_falcon2_madura_rxa_dfe_tap3());
    ESTM(lane_st->dfe[2][1] = rd_falcon2_madura_rxb_dfe_tap3());
    ESTM(lane_st->dfe[2][2] = rd_falcon2_madura_rxc_dfe_tap3());
    ESTM(lane_st->dfe[2][3] = rd_falcon2_madura_rxd_dfe_tap3());
    ESTM(lane_st->dfe[3][0] = rd_falcon2_madura_rxa_dfe_tap4());
    ESTM(lane_st->dfe[3][1] = rd_falcon2_madura_rxb_dfe_tap4());
    ESTM(lane_st->dfe[3][2] = rd_falcon2_madura_rxc_dfe_tap4());
    ESTM(lane_st->dfe[3][3] = rd_falcon2_madura_rxd_dfe_tap4());
    ESTM(lane_st->dfe[4][0] = rd_falcon2_madura_rxa_dfe_tap5());
    ESTM(lane_st->dfe[4][1] = rd_falcon2_madura_rxb_dfe_tap5());
    ESTM(lane_st->dfe[4][2] = rd_falcon2_madura_rxc_dfe_tap5());
    ESTM(lane_st->dfe[4][3] = rd_falcon2_madura_rxd_dfe_tap5());
    ESTM(lane_st->dfe[5][0] = rd_falcon2_madura_rxa_dfe_tap6());
    ESTM(lane_st->dfe[5][1] = rd_falcon2_madura_rxb_dfe_tap6());
    ESTM(lane_st->dfe[5][2] = rd_falcon2_madura_rxc_dfe_tap6());
    ESTM(lane_st->dfe[5][3] = rd_falcon2_madura_rxd_dfe_tap6());
    ESTM(lane_st->dfe[6][0] = ((rd_falcon2_madura_rxa_dfe_tap7_mux()==0)?rd_falcon2_madura_rxa_dfe_tap7():0));
    ESTM(lane_st->dfe[6][1] = ((rd_falcon2_madura_rxb_dfe_tap7_mux()==0)?rd_falcon2_madura_rxb_dfe_tap7():0));
    ESTM(lane_st->dfe[6][2] = ((rd_falcon2_madura_rxc_dfe_tap7_mux()==0)?rd_falcon2_madura_rxc_dfe_tap7():0));
    ESTM(lane_st->dfe[6][3] = ((rd_falcon2_madura_rxd_dfe_tap7_mux()==0)?rd_falcon2_madura_rxd_dfe_tap7():0));
    ESTM(lane_st->dfe[7][0] = ((rd_falcon2_madura_rxa_dfe_tap8_mux()==0)?rd_falcon2_madura_rxa_dfe_tap8():0));
    ESTM(lane_st->dfe[7][1] = ((rd_falcon2_madura_rxb_dfe_tap8_mux()==0)?rd_falcon2_madura_rxb_dfe_tap8():0));
    ESTM(lane_st->dfe[7][2] = ((rd_falcon2_madura_rxc_dfe_tap8_mux()==0)?rd_falcon2_madura_rxc_dfe_tap8():0));
    ESTM(lane_st->dfe[7][3] = ((rd_falcon2_madura_rxd_dfe_tap8_mux()==0)?rd_falcon2_madura_rxd_dfe_tap8():0));
    ESTM(lane_st->dfe[8][0] = ((rd_falcon2_madura_rxa_dfe_tap9_mux()==0)?rd_falcon2_madura_rxa_dfe_tap9():0));
    ESTM(lane_st->dfe[8][1] = ((rd_falcon2_madura_rxb_dfe_tap9_mux()==0)?rd_falcon2_madura_rxb_dfe_tap9():0));
    ESTM(lane_st->dfe[8][2] = ((rd_falcon2_madura_rxc_dfe_tap9_mux()==0)?rd_falcon2_madura_rxc_dfe_tap9():0));
    ESTM(lane_st->dfe[8][3] = ((rd_falcon2_madura_rxd_dfe_tap9_mux()==0)?rd_falcon2_madura_rxd_dfe_tap9():0));
    ESTM(lane_st->dfe[9][0] = ((rd_falcon2_madura_rxa_dfe_tap10_mux()==0)?rd_falcon2_madura_rxa_dfe_tap10():0));
    ESTM(lane_st->dfe[9][1] = ((rd_falcon2_madura_rxb_dfe_tap10_mux()==0)?rd_falcon2_madura_rxb_dfe_tap10():0));
    ESTM(lane_st->dfe[9][2] = ((rd_falcon2_madura_rxc_dfe_tap10_mux()==0)?rd_falcon2_madura_rxc_dfe_tap10():0));
    ESTM(lane_st->dfe[9][3] = ((rd_falcon2_madura_rxd_dfe_tap10_mux()==0)?rd_falcon2_madura_rxd_dfe_tap10():0));
    ESTM(lane_st->dfe[10][0] = ((rd_falcon2_madura_rxa_dfe_tap7_mux()==1)?rd_falcon2_madura_rxa_dfe_tap7():(rd_falcon2_madura_rxa_dfe_tap11_mux()==0)?rd_falcon2_madura_rxa_dfe_tap11():0));
    ESTM(lane_st->dfe[10][1] = ((rd_falcon2_madura_rxb_dfe_tap7_mux()==1)?rd_falcon2_madura_rxb_dfe_tap7():(rd_falcon2_madura_rxb_dfe_tap11_mux()==0)?rd_falcon2_madura_rxb_dfe_tap11():0));
    ESTM(lane_st->dfe[10][2] = ((rd_falcon2_madura_rxc_dfe_tap7_mux()==1)?rd_falcon2_madura_rxc_dfe_tap7():(rd_falcon2_madura_rxc_dfe_tap11_mux()==0)?rd_falcon2_madura_rxc_dfe_tap11():0));
    ESTM(lane_st->dfe[10][3] = ((rd_falcon2_madura_rxd_dfe_tap7_mux()==1)?rd_falcon2_madura_rxd_dfe_tap7():(rd_falcon2_madura_rxd_dfe_tap11_mux()==0)?rd_falcon2_madura_rxd_dfe_tap11():0));
    ESTM(lane_st->dfe[11][0] = ((rd_falcon2_madura_rxa_dfe_tap8_mux()==1)?rd_falcon2_madura_rxa_dfe_tap8():(rd_falcon2_madura_rxa_dfe_tap12_mux()==0)?rd_falcon2_madura_rxa_dfe_tap12():0));
    ESTM(lane_st->dfe[11][1] = ((rd_falcon2_madura_rxb_dfe_tap8_mux()==1)?rd_falcon2_madura_rxb_dfe_tap8():(rd_falcon2_madura_rxb_dfe_tap12_mux()==0)?rd_falcon2_madura_rxb_dfe_tap12():0));
    ESTM(lane_st->dfe[11][2] = ((rd_falcon2_madura_rxc_dfe_tap8_mux()==1)?rd_falcon2_madura_rxc_dfe_tap8():(rd_falcon2_madura_rxc_dfe_tap12_mux()==0)?rd_falcon2_madura_rxc_dfe_tap12():0));
    ESTM(lane_st->dfe[11][3] = ((rd_falcon2_madura_rxd_dfe_tap8_mux()==1)?rd_falcon2_madura_rxd_dfe_tap8():(rd_falcon2_madura_rxd_dfe_tap12_mux()==0)?rd_falcon2_madura_rxd_dfe_tap12():0));
    ESTM(lane_st->dfe[12][0] = ((rd_falcon2_madura_rxa_dfe_tap9_mux()==1)?rd_falcon2_madura_rxa_dfe_tap9():(rd_falcon2_madura_rxa_dfe_tap13_mux()==0)?rd_falcon2_madura_rxa_dfe_tap13():0));
    ESTM(lane_st->dfe[12][1] = ((rd_falcon2_madura_rxb_dfe_tap9_mux()==1)?rd_falcon2_madura_rxb_dfe_tap9():(rd_falcon2_madura_rxb_dfe_tap13_mux()==0)?rd_falcon2_madura_rxb_dfe_tap13():0));
    ESTM(lane_st->dfe[12][2] = ((rd_falcon2_madura_rxc_dfe_tap9_mux()==1)?rd_falcon2_madura_rxc_dfe_tap9():(rd_falcon2_madura_rxc_dfe_tap13_mux()==0)?rd_falcon2_madura_rxc_dfe_tap13():0));
    ESTM(lane_st->dfe[12][3] = ((rd_falcon2_madura_rxd_dfe_tap9_mux()==1)?rd_falcon2_madura_rxd_dfe_tap9():(rd_falcon2_madura_rxd_dfe_tap13_mux()==0)?rd_falcon2_madura_rxd_dfe_tap13():0));
    ESTM(lane_st->dfe[13][0] = ((rd_falcon2_madura_rxa_dfe_tap10_mux()==1)?rd_falcon2_madura_rxa_dfe_tap10():(rd_falcon2_madura_rxa_dfe_tap14_mux()==0)?rd_falcon2_madura_rxa_dfe_tap14():0));
    ESTM(lane_st->dfe[13][1] = ((rd_falcon2_madura_rxb_dfe_tap10_mux()==1)?rd_falcon2_madura_rxb_dfe_tap10():(rd_falcon2_madura_rxb_dfe_tap14_mux()==0)?rd_falcon2_madura_rxb_dfe_tap14():0));
    ESTM(lane_st->dfe[13][2] = ((rd_falcon2_madura_rxc_dfe_tap10_mux()==1)?rd_falcon2_madura_rxc_dfe_tap10():(rd_falcon2_madura_rxc_dfe_tap14_mux()==0)?rd_falcon2_madura_rxc_dfe_tap14():0));
    ESTM(lane_st->dfe[13][3] = ((rd_falcon2_madura_rxd_dfe_tap10_mux()==1)?rd_falcon2_madura_rxd_dfe_tap10():(rd_falcon2_madura_rxd_dfe_tap14_mux()==0)?rd_falcon2_madura_rxd_dfe_tap14():0));
    ESTM(lane_st->dfe[14][0] = ((rd_falcon2_madura_rxa_dfe_tap7_mux()==2)?rd_falcon2_madura_rxa_dfe_tap7():(rd_falcon2_madura_rxa_dfe_tap11_mux()==1)?rd_falcon2_madura_rxa_dfe_tap11():0));
    ESTM(lane_st->dfe[14][1] = ((rd_falcon2_madura_rxb_dfe_tap7_mux()==2)?rd_falcon2_madura_rxb_dfe_tap7():(rd_falcon2_madura_rxb_dfe_tap11_mux()==1)?rd_falcon2_madura_rxb_dfe_tap11():0));
    ESTM(lane_st->dfe[14][2] = ((rd_falcon2_madura_rxc_dfe_tap7_mux()==2)?rd_falcon2_madura_rxc_dfe_tap7():(rd_falcon2_madura_rxc_dfe_tap11_mux()==1)?rd_falcon2_madura_rxc_dfe_tap11():0));
    ESTM(lane_st->dfe[14][3] = ((rd_falcon2_madura_rxd_dfe_tap7_mux()==2)?rd_falcon2_madura_rxd_dfe_tap7():(rd_falcon2_madura_rxd_dfe_tap11_mux()==1)?rd_falcon2_madura_rxd_dfe_tap11():0));
    ESTM(lane_st->dfe[15][0] = ((rd_falcon2_madura_rxa_dfe_tap8_mux()==2)?rd_falcon2_madura_rxa_dfe_tap8():(rd_falcon2_madura_rxa_dfe_tap12_mux()==1)?rd_falcon2_madura_rxa_dfe_tap12():0));
    ESTM(lane_st->dfe[15][1] = ((rd_falcon2_madura_rxb_dfe_tap8_mux()==2)?rd_falcon2_madura_rxb_dfe_tap8():(rd_falcon2_madura_rxb_dfe_tap12_mux()==1)?rd_falcon2_madura_rxb_dfe_tap12():0));
    ESTM(lane_st->dfe[15][2] = ((rd_falcon2_madura_rxc_dfe_tap8_mux()==2)?rd_falcon2_madura_rxc_dfe_tap8():(rd_falcon2_madura_rxc_dfe_tap12_mux()==1)?rd_falcon2_madura_rxc_dfe_tap12():0));
    ESTM(lane_st->dfe[15][3] = ((rd_falcon2_madura_rxd_dfe_tap8_mux()==2)?rd_falcon2_madura_rxd_dfe_tap8():(rd_falcon2_madura_rxd_dfe_tap12_mux()==1)?rd_falcon2_madura_rxd_dfe_tap12():0));
    ESTM(lane_st->dfe[16][0] = ((rd_falcon2_madura_rxa_dfe_tap9_mux()==2)?rd_falcon2_madura_rxa_dfe_tap9():(rd_falcon2_madura_rxa_dfe_tap13_mux()==1)?rd_falcon2_madura_rxa_dfe_tap13():0));
    ESTM(lane_st->dfe[16][1] = ((rd_falcon2_madura_rxb_dfe_tap9_mux()==2)?rd_falcon2_madura_rxb_dfe_tap9():(rd_falcon2_madura_rxb_dfe_tap13_mux()==1)?rd_falcon2_madura_rxb_dfe_tap13():0));
    ESTM(lane_st->dfe[16][2] = ((rd_falcon2_madura_rxc_dfe_tap9_mux()==2)?rd_falcon2_madura_rxc_dfe_tap9():(rd_falcon2_madura_rxc_dfe_tap13_mux()==1)?rd_falcon2_madura_rxc_dfe_tap13():0));
    ESTM(lane_st->dfe[16][3] = ((rd_falcon2_madura_rxd_dfe_tap9_mux()==2)?rd_falcon2_madura_rxd_dfe_tap9():(rd_falcon2_madura_rxd_dfe_tap13_mux()==1)?rd_falcon2_madura_rxd_dfe_tap13():0));
    ESTM(lane_st->dfe[17][0] = ((rd_falcon2_madura_rxa_dfe_tap10_mux()==2)?rd_falcon2_madura_rxa_dfe_tap10():(rd_falcon2_madura_rxa_dfe_tap14_mux()==1)?rd_falcon2_madura_rxa_dfe_tap14():0));
    ESTM(lane_st->dfe[17][1] = ((rd_falcon2_madura_rxb_dfe_tap10_mux()==2)?rd_falcon2_madura_rxb_dfe_tap10():(rd_falcon2_madura_rxb_dfe_tap14_mux()==1)?rd_falcon2_madura_rxb_dfe_tap14():0));
    ESTM(lane_st->dfe[17][2] = ((rd_falcon2_madura_rxc_dfe_tap10_mux()==2)?rd_falcon2_madura_rxc_dfe_tap10():(rd_falcon2_madura_rxc_dfe_tap14_mux()==1)?rd_falcon2_madura_rxc_dfe_tap14():0));
    ESTM(lane_st->dfe[17][3] = ((rd_falcon2_madura_rxd_dfe_tap10_mux()==2)?rd_falcon2_madura_rxd_dfe_tap10():(rd_falcon2_madura_rxd_dfe_tap14_mux()==1)?rd_falcon2_madura_rxd_dfe_tap14():0));
    ESTM(lane_st->dfe[18][0] = ((rd_falcon2_madura_rxa_dfe_tap7_mux()==3)?rd_falcon2_madura_rxa_dfe_tap7():(rd_falcon2_madura_rxa_dfe_tap11_mux()==2)?rd_falcon2_madura_rxa_dfe_tap11():0));
    ESTM(lane_st->dfe[18][1] = ((rd_falcon2_madura_rxb_dfe_tap7_mux()==3)?rd_falcon2_madura_rxb_dfe_tap7():(rd_falcon2_madura_rxb_dfe_tap11_mux()==2)?rd_falcon2_madura_rxb_dfe_tap11():0));
    ESTM(lane_st->dfe[18][2] = ((rd_falcon2_madura_rxc_dfe_tap7_mux()==3)?rd_falcon2_madura_rxc_dfe_tap7():(rd_falcon2_madura_rxc_dfe_tap11_mux()==2)?rd_falcon2_madura_rxc_dfe_tap11():0));
    ESTM(lane_st->dfe[18][3] = ((rd_falcon2_madura_rxd_dfe_tap7_mux()==3)?rd_falcon2_madura_rxd_dfe_tap7():(rd_falcon2_madura_rxd_dfe_tap11_mux()==2)?rd_falcon2_madura_rxd_dfe_tap11():0));
    ESTM(lane_st->dfe[19][0] = ((rd_falcon2_madura_rxa_dfe_tap8_mux()==3)?rd_falcon2_madura_rxa_dfe_tap8():(rd_falcon2_madura_rxa_dfe_tap12_mux()==2)?rd_falcon2_madura_rxa_dfe_tap12():0));
    ESTM(lane_st->dfe[19][1] = ((rd_falcon2_madura_rxb_dfe_tap8_mux()==3)?rd_falcon2_madura_rxb_dfe_tap8():(rd_falcon2_madura_rxb_dfe_tap12_mux()==2)?rd_falcon2_madura_rxb_dfe_tap12():0));
    ESTM(lane_st->dfe[19][2] = ((rd_falcon2_madura_rxc_dfe_tap8_mux()==3)?rd_falcon2_madura_rxc_dfe_tap8():(rd_falcon2_madura_rxc_dfe_tap12_mux()==2)?rd_falcon2_madura_rxc_dfe_tap12():0));
    ESTM(lane_st->dfe[19][3] = ((rd_falcon2_madura_rxd_dfe_tap8_mux()==3)?rd_falcon2_madura_rxd_dfe_tap8():(rd_falcon2_madura_rxd_dfe_tap12_mux()==2)?rd_falcon2_madura_rxd_dfe_tap12():0));
    ESTM(lane_st->dfe[20][0] = ((rd_falcon2_madura_rxa_dfe_tap9_mux()==3)?rd_falcon2_madura_rxa_dfe_tap9():(rd_falcon2_madura_rxa_dfe_tap13_mux()==2)?rd_falcon2_madura_rxa_dfe_tap13():0));
    ESTM(lane_st->dfe[20][1] = ((rd_falcon2_madura_rxb_dfe_tap9_mux()==3)?rd_falcon2_madura_rxb_dfe_tap9():(rd_falcon2_madura_rxb_dfe_tap13_mux()==2)?rd_falcon2_madura_rxb_dfe_tap13():0));
    ESTM(lane_st->dfe[20][2] = ((rd_falcon2_madura_rxc_dfe_tap9_mux()==3)?rd_falcon2_madura_rxc_dfe_tap9():(rd_falcon2_madura_rxc_dfe_tap13_mux()==2)?rd_falcon2_madura_rxc_dfe_tap13():0));
    ESTM(lane_st->dfe[20][3] = ((rd_falcon2_madura_rxd_dfe_tap9_mux()==3)?rd_falcon2_madura_rxd_dfe_tap9():(rd_falcon2_madura_rxd_dfe_tap13_mux()==2)?rd_falcon2_madura_rxd_dfe_tap13():0));
    ESTM(lane_st->dfe[21][0] = ((rd_falcon2_madura_rxa_dfe_tap10_mux()==3)?rd_falcon2_madura_rxa_dfe_tap10():(rd_falcon2_madura_rxa_dfe_tap14_mux()==2)?rd_falcon2_madura_rxa_dfe_tap14():0));
    ESTM(lane_st->dfe[21][1] = ((rd_falcon2_madura_rxb_dfe_tap10_mux()==3)?rd_falcon2_madura_rxb_dfe_tap10():(rd_falcon2_madura_rxb_dfe_tap14_mux()==2)?rd_falcon2_madura_rxb_dfe_tap14():0));
    ESTM(lane_st->dfe[21][2] = ((rd_falcon2_madura_rxc_dfe_tap10_mux()==3)?rd_falcon2_madura_rxc_dfe_tap10():(rd_falcon2_madura_rxc_dfe_tap14_mux()==2)?rd_falcon2_madura_rxc_dfe_tap14():0));
    ESTM(lane_st->dfe[21][3] = ((rd_falcon2_madura_rxd_dfe_tap10_mux()==3)?rd_falcon2_madura_rxd_dfe_tap10():(rd_falcon2_madura_rxd_dfe_tap14_mux()==2)?rd_falcon2_madura_rxd_dfe_tap14():0));
    ESTM(lane_st->dfe[22][0] = ((rd_falcon2_madura_rxa_dfe_tap11_mux()==3)?rd_falcon2_madura_rxa_dfe_tap11():0));
    ESTM(lane_st->dfe[22][1] = ((rd_falcon2_madura_rxb_dfe_tap11_mux()==3)?rd_falcon2_madura_rxb_dfe_tap11():0));
    ESTM(lane_st->dfe[22][2] = ((rd_falcon2_madura_rxc_dfe_tap11_mux()==3)?rd_falcon2_madura_rxc_dfe_tap11():0));
    ESTM(lane_st->dfe[22][3] = ((rd_falcon2_madura_rxd_dfe_tap11_mux()==3)?rd_falcon2_madura_rxd_dfe_tap11():0));
    ESTM(lane_st->dfe[23][0] = ((rd_falcon2_madura_rxa_dfe_tap12_mux()==3)?rd_falcon2_madura_rxa_dfe_tap12():0));
    ESTM(lane_st->dfe[23][1] = ((rd_falcon2_madura_rxb_dfe_tap12_mux()==3)?rd_falcon2_madura_rxb_dfe_tap12():0));
    ESTM(lane_st->dfe[23][2] = ((rd_falcon2_madura_rxc_dfe_tap12_mux()==3)?rd_falcon2_madura_rxc_dfe_tap12():0));
    ESTM(lane_st->dfe[23][3] = ((rd_falcon2_madura_rxd_dfe_tap12_mux()==3)?rd_falcon2_madura_rxd_dfe_tap12():0));
    ESTM(lane_st->dfe[24][0] = ((rd_falcon2_madura_rxa_dfe_tap13_mux()==3)?rd_falcon2_madura_rxa_dfe_tap13():0));
    ESTM(lane_st->dfe[24][1] = ((rd_falcon2_madura_rxb_dfe_tap13_mux()==3)?rd_falcon2_madura_rxb_dfe_tap13():0));
    ESTM(lane_st->dfe[24][2] = ((rd_falcon2_madura_rxc_dfe_tap13_mux()==3)?rd_falcon2_madura_rxc_dfe_tap13():0));
    ESTM(lane_st->dfe[24][3] = ((rd_falcon2_madura_rxd_dfe_tap13_mux()==3)?rd_falcon2_madura_rxd_dfe_tap13():0));
    ESTM(lane_st->dfe[25][0] = ((rd_falcon2_madura_rxa_dfe_tap14_mux()==3)?rd_falcon2_madura_rxa_dfe_tap14():0));
    ESTM(lane_st->dfe[25][1] = ((rd_falcon2_madura_rxb_dfe_tap14_mux()==3)?rd_falcon2_madura_rxb_dfe_tap14():0));
    ESTM(lane_st->dfe[25][2] = ((rd_falcon2_madura_rxc_dfe_tap14_mux()==3)?rd_falcon2_madura_rxc_dfe_tap14():0));
    ESTM(lane_st->dfe[25][3] = ((rd_falcon2_madura_rxd_dfe_tap14_mux()==3)?rd_falcon2_madura_rxd_dfe_tap14():0));
    ESTM(lane_st->thctrl_dp[0] = rd_falcon2_madura_rxa_slicer_offset_adj_dp());
    ESTM(lane_st->thctrl_dp[1] = rd_falcon2_madura_rxb_slicer_offset_adj_dp());
    ESTM(lane_st->thctrl_dp[2] = rd_falcon2_madura_rxc_slicer_offset_adj_dp());
    ESTM(lane_st->thctrl_dp[3] = rd_falcon2_madura_rxd_slicer_offset_adj_dp());
    ESTM(lane_st->thctrl_dn[0] = rd_falcon2_madura_rxa_slicer_offset_adj_dn());
    ESTM(lane_st->thctrl_dn[1] = rd_falcon2_madura_rxb_slicer_offset_adj_dn());
    ESTM(lane_st->thctrl_dn[2] = rd_falcon2_madura_rxc_slicer_offset_adj_dn());
    ESTM(lane_st->thctrl_dn[3] = rd_falcon2_madura_rxd_slicer_offset_adj_dn());
    ESTM(lane_st->thctrl_zp[0] = rd_falcon2_madura_rxa_slicer_offset_adj_zp());
    ESTM(lane_st->thctrl_zp[1] = rd_falcon2_madura_rxb_slicer_offset_adj_zp());
    ESTM(lane_st->thctrl_zp[2] = rd_falcon2_madura_rxc_slicer_offset_adj_zp());
    ESTM(lane_st->thctrl_zp[3] = rd_falcon2_madura_rxd_slicer_offset_adj_zp());
    ESTM(lane_st->thctrl_zn[0] = rd_falcon2_madura_rxa_slicer_offset_adj_zn());
    ESTM(lane_st->thctrl_zn[1] = rd_falcon2_madura_rxb_slicer_offset_adj_zn());
    ESTM(lane_st->thctrl_zn[2] = rd_falcon2_madura_rxc_slicer_offset_adj_zn());
    ESTM(lane_st->thctrl_zn[3] = rd_falcon2_madura_rxd_slicer_offset_adj_zn());
    ESTM(lane_st->thctrl_l[0] = rd_falcon2_madura_rxa_slicer_offset_adj_lms());
    ESTM(lane_st->thctrl_l[1] = rd_falcon2_madura_rxb_slicer_offset_adj_lms());
    ESTM(lane_st->thctrl_l[2] = rd_falcon2_madura_rxc_slicer_offset_adj_lms());
    ESTM(lane_st->thctrl_l[3] = rd_falcon2_madura_rxd_slicer_offset_adj_lms());
    ESTM(lane_st->heye_left = _eye_to_mUI(pa, rdv_falcon2_madura_usr_sts_heye_left()));
    ESTM(lane_st->heye_right = _eye_to_mUI(pa, rdv_falcon2_madura_usr_sts_heye_right()));
    ESTM(lane_st->veye_upper = _eye_to_mV(pa, rdv_falcon2_madura_usr_sts_veye_upper(), 0));
    ESTM(lane_st->veye_lower = _eye_to_mV(pa, rdv_falcon2_madura_usr_sts_veye_lower(), 0));
    ESTM(lane_st->link_time = (((uint32_t)rdv_falcon2_madura_usr_sts_link_time())*8)/10);
    ESTM(lane_st->prbs_chk_en = rd_falcon2_madura_prbs_chk_en());
    ESTM(reg_data = rd_falcon2_madura_prbs_chk_mode_sel());
    lane_st->prbs_chk_order = srds_return_prbs_order(reg_data);
    EFUN(falcon2_madura_prbs_chk_lock_state( pa, &lane_st->prbs_chk_lock));
    EFUN(falcon2_madura_prbs_err_count_ll( pa, &lane_st->prbs_chk_errcnt));

    if (lane_st->pmd_lock == 1) {
      if (!lane_st->stop_state) {
        EFUN(falcon2_madura_stop_rx_adaptation( pa, 0));
      }
    } else {
        EFUN(falcon2_madura_stop_rx_adaptation( pa, 0));
    }

    return(ERR_CODE_NONE);
}

err_code_t falcon2_madura_disp_full_pmd_state ( const phymod_access_t *pa, struct falcon2_madura_detailed_lane_status_st *lane_st, uint8_t num_lanes) {
    uint8_t i;

    EFUN_PRINTF(( "\n\n"));
    EFUN_PRINTF(( "------------------------------------------------------------------------\n"));
    EFUN_PRINTF(( "Falcon PMD State\n"));
    EFUN_PRINTF(( "%-16s\t%12s%12s%12s%12s\n", "PARAMETER","LN0","LN1","LN2","LN3"));
    EFUN_PRINTF(( "------------------------------------------------------------------------\n"));
    DISP_LN_VARS( "HEYE Left (mUI)",heye_left,"%12d");
    DISP_LN_VARS( "HEYE Rght (mUI)",heye_right,"%12d");
    DISP_LN_VARS( "VEYE Top  (mV)",veye_upper,"%12d");
    DISP_LN_VARS( "VEYE Bott (mV)",veye_lower,"%12d");
#ifdef SERDES_API_FLOATING_POINT
    DISP_LN_VARS( "Link Time (ms)",link_time/10.0,"%12.1f");
#else
    DISP_LN_VARS( "Link Time (ms)",link_time/10,"%12d");
#endif
    DISP_LN_VARS ("Ln Reset State",reset_state,"%12d");
    DISP_LN_VARS ("uC Stop",stop_state,"%12d");

    EFUN_PRINTF(( "------------------------------------------------------------------------\n"));
    DISP_LN_VARS( "TX drv_hv_disable",ams_tx_drv_hv_disable,"%12d");
    DISP_LN_VARS( "TX ana_rescal",ams_tx_ana_rescal,"%12d");
    DISP_LN_VARS( "TX amp_ctrl",amp_ctrl,"%12d");
    DISP_LN_VARS( "TX pre_tap",pre_tap,"%12d");
    DISP_LN_VARS( "TX main_tap",main_tap,"%12d");
    DISP_LN_VARS( "TX post1_tap",post1_tap,"%12d");
    DISP_LN_VARS( "TX post2_tap",post2_tap,"%12d");
    DISP_LN_VARS( "TX post3_tap",post3_tap,"%12d");
    EFUN_PRINTF(( "------------------------------------------------------------------------\n"));
    DISP_LN_VARS( "Sigdet",sigdet,"%12d");
    DISP_LN_VARS( "PMD_lock",pmd_lock,"%12d");
    DISP_LN_VARS( "PLL select",pmd_pll_select,"%12d");
    DISP_LN_VARS( "DSC_SM (1st read)",dsc_sm[0],"       %4xh");
    DISP_LN_VARS( "DSC_SM (2nd read)",dsc_sm[1],"       %4xh");
    EFUN_PRINTF(( "------------------------------------------------------------------------\n"));
#ifdef SERDES_API_FLOATING_POINT
    DISP_LN_VARS( "PPM",ppm,"%12.2f");
#else
    DISP_LN_VARS( "PPM",ppm,"%12d");
#endif
    DISP_LN_VARS( "VGA",vga,"%12d");
    DISP_LN_VARS( "PF",pf,"%12d");
    DISP_LN_VARS( "PF2",pf2,"%12d");
#ifdef SERDES_API_FLOATING_POINT
    DISP_LN_VARS( "main_tap",main_tap_est,"%12.2f");
#else
    DISP_LN_VARS( "main_tap",main_tap_est,"%12d");
#endif
    DISP_LN_VARS( "data_thresh",data_thresh,"%12d");
    DISP_LN_VARS( "phase_thresh",phase_thresh,"%12d");
    DISP_LN_VARS( "lms_thresh",lms_thresh,"%12d");
    EFUN_PRINTF(( "------------------------------------------------------------------------\n"));
    DISP_LN_VARS( "d/dq hoffset",ddq_hoffset,"%12u");
    DISP_LN_VARS( "p/pq hoffset",ppq_hoffset,"%12u");
    DISP_LN_VARS( "l/lq hoffset",llq_hoffset,"%12u");
    DISP_LN_VARS( "d/p hoffset",dp_hoffset,"%12u");
    DISP_LN_VARS( "d/l hoffset",dl_hoffset,"%12u");
    EFUN_PRINTF(( "------------------------------------------------------------------------\n"));
    DISP_LNQ_VARS("dfe[2][a,b]",dfe[1][0],dfe[1][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[2][c,d]",dfe[1][2],dfe[1][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[3][a,b]",dfe[2][0],dfe[2][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[3][c,d]",dfe[2][2],dfe[2][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[4][a,b]",dfe[3][0],dfe[3][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[4][c,d]",dfe[3][2],dfe[3][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[5][a,b]",dfe[4][0],dfe[4][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[5][c,d]",dfe[4][2],dfe[4][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[6][a,b]",dfe[5][0],dfe[5][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[6][c,d]",dfe[5][2],dfe[5][3],"%8d,%3d");
    EFUN_PRINTF(( "------------------------------------------------------------------------\n"));
    DISP_LNQ_VARS("dfe[7][a,b]",dfe[6][0],dfe[6][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[7][c,d]",dfe[6][2],dfe[6][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[8][a,b]",dfe[7][0],dfe[7][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[8][c,d]",dfe[7][2],dfe[7][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[9][a,b]",dfe[8][0],dfe[8][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[9][c,d]",dfe[8][2],dfe[8][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[10][a,b]",dfe[9][0],dfe[9][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[10][c,d]",dfe[9][2],dfe[9][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[11][a,b]",dfe[10][0],dfe[10][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[11][c,d]",dfe[10][2],dfe[10][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[12][a,b]",dfe[11][0],dfe[11][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[12][c,d]",dfe[11][2],dfe[11][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[13][a,b]",dfe[12][0],dfe[12][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[13][c,d]",dfe[12][2],dfe[12][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[14][a,b]",dfe[13][0],dfe[13][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[14][c,d]",dfe[13][2],dfe[13][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[15][a,b]",dfe[14][0],dfe[14][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[15][c,d]",dfe[14][2],dfe[14][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[16][a,b]",dfe[15][0],dfe[15][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[16][c,d]",dfe[15][2],dfe[15][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[17][a,b]",dfe[16][0],dfe[16][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[17][c,d]",dfe[16][2],dfe[16][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[18][a,b]",dfe[17][0],dfe[17][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[18][c,d]",dfe[17][2],dfe[17][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[19][a,b]",dfe[18][0],dfe[18][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[19][c,d]",dfe[18][2],dfe[18][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[20][a,b]",dfe[19][0],dfe[19][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[20][c,d]",dfe[19][2],dfe[19][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[21][a,b]",dfe[20][0],dfe[20][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[21][c,d]",dfe[20][2],dfe[20][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[22][a,b]",dfe[21][0],dfe[21][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[22][c,d]",dfe[21][2],dfe[21][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[23][a,b]",dfe[22][0],dfe[22][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[23][c,d]",dfe[22][2],dfe[22][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[24][a,b]",dfe[23][0],dfe[23][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[24][c,d]",dfe[23][2],dfe[23][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[25][a,b]",dfe[24][0],dfe[24][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[25][c,d]",dfe[24][2],dfe[24][3],"%8d,%3d");
    DISP_LNQ_VARS("dfe[26][a,b]",dfe[25][0],dfe[25][1],"%8d,%3d");
    DISP_LNQ_VARS("dfe[26][c,d]",dfe[25][2],dfe[25][3],"%8d,%3d");
    EFUN_PRINTF(( "------------------------------------------------------------------------\n"));
    DISP_LN_VARS ("dc_offset",dc_offset,"%12d");
    DISP_LNQ_VARS("data_p[a,b]",thctrl_dp[0],thctrl_dp[1],"%8d,%3d");
    DISP_LNQ_VARS("data_p[c,d]",thctrl_dp[2],thctrl_dp[3],"%8d,%3d");
    DISP_LNQ_VARS("data_n[a,b]",thctrl_dn[0],thctrl_dn[1],"%8d,%3d");
    DISP_LNQ_VARS("data_n[c,d]",thctrl_dn[2],thctrl_dn[3],"%8d,%3d");
    DISP_LNQ_VARS("phase_p[a,b]",thctrl_zp[0],thctrl_zp[1],"%8d,%3d");
    DISP_LNQ_VARS("phase_p[c,d]",thctrl_zp[2],thctrl_zp[3],"%8d,%3d");
    DISP_LNQ_VARS("phase_n[a,b]",thctrl_zn[0],thctrl_zn[1],"%8d,%3d");
    DISP_LNQ_VARS("phase_n[c,d]",thctrl_zn[2],thctrl_zn[3],"%8d,%3d");
    DISP_LNQ_VARS("lms[a,b]",thctrl_l[0],thctrl_l[1],"%8d,%3d");
    DISP_LNQ_VARS("lms[c,d]",thctrl_l[2],thctrl_l[3],"%8d,%3d");
    EFUN_PRINTF(( "------------------------------------------------------------------------\n"));
    DISP_LN_VARS ("PRBS_CHECKER",prbs_chk_en,"%12u");
    DISP_LN_VARS ("PRBS_ORDER",prbs_chk_order,"%12u");
    DISP_LN_VARS ("PRBS_LOCK",prbs_chk_lock,"%12u");
    DISP_LN_VARS ("PRBS_ERRCNT",prbs_chk_errcnt,"%12u");
    EFUN_PRINTF(( "------------------------------------------------------------------------\n"));
    EFUN_PRINTF(( "\n"));

    return (ERR_CODE_NONE);
}


static err_code_t _compute_bin( const phymod_access_t *pa, char var, char bin[]) {
    if(!bin) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

  switch (var) {
    case '0':  ENULL_STRCPY(bin,"0000");
               break;
    case '1':  ENULL_STRCPY(bin,"0001");
               break;
    case '2':  ENULL_STRCPY(bin,"0010");
               break;
    case '3':  ENULL_STRCPY(bin,"0011");
               break;
    case '4':  ENULL_STRCPY(bin,"0100");
               break;
    case '5':  ENULL_STRCPY(bin,"0101");
               break;
    case '6':  ENULL_STRCPY(bin,"0110");
               break;
    case '7':  ENULL_STRCPY(bin,"0111");
               break;
    case '8':  ENULL_STRCPY(bin,"1000");
               break;
    case '9':  ENULL_STRCPY(bin,"1001");
               break;
    case 'a':
    case 'A':  ENULL_STRCPY(bin,"1010");
               break;
    case 'b':
    case 'B':  ENULL_STRCPY(bin,"1011");
               break;
    case 'c':
    case 'C':  ENULL_STRCPY(bin,"1100");
               break;
    case 'd':
    case 'D':  ENULL_STRCPY(bin,"1101");
               break;
    case 'e':
    case 'E':  ENULL_STRCPY(bin,"1110");
               break;
    case 'f':
    case 'F':  ENULL_STRCPY(bin,"1111");
               break;
    case '_':  ENULL_STRCPY(bin,"");
               break;
    default :  ENULL_STRCPY(bin,"");
               EFUN_PRINTF(("ERROR: Invalid Hexadecimal Pattern\n"));
               return (_error(ERR_CODE_CFG_PATT_INVALID_HEX));
               break;
  }
  return (ERR_CODE_NONE);
}


static err_code_t _compute_hex( const phymod_access_t *pa, char bin[],uint8_t *hex) {
    if(!hex) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }


  if (!USR_STRCMP(bin,"0000")) {
    *hex = 0x0;
  }
  else if (!USR_STRCMP(bin,"0001")) {
    *hex = 0x1;
  }
  else if (!USR_STRCMP(bin,"0010")) {
    *hex = 0x2;
  }
  else if (!USR_STRCMP(bin,"0011")) {
    *hex = 0x3;
  }
  else if (!USR_STRCMP(bin,"0100")) {
    *hex = 0x4;
  }
  else if (!USR_STRCMP(bin,"0101")) {
    *hex = 0x5;
  }
  else if (!USR_STRCMP(bin,"0110")) {
    *hex = 0x6;
  }
  else if (!USR_STRCMP(bin,"0111")) {
    *hex = 0x7;
  }
  else if (!USR_STRCMP(bin,"1000")) {
    *hex = 0x8;
  }
  else if (!USR_STRCMP(bin,"1001")) {
    *hex = 0x9;
  }
  else if (!USR_STRCMP(bin,"1010")) {
    *hex = 0xA;
  }
  else if (!USR_STRCMP(bin,"1011")) {
    *hex = 0xB;
  }
  else if (!USR_STRCMP(bin,"1100")) {
    *hex = 0xC;
  }
  else if (!USR_STRCMP(bin,"1101")) {
    *hex = 0xD;
  }
  else if (!USR_STRCMP(bin,"1110")) {
    *hex = 0xE;
  }
  else if (!USR_STRCMP(bin,"1111")) {
    *hex = 0xF;
  }
  else {
    EFUN_PRINTF(("ERROR: Invalid Binary to Hex Conversion\n"));
    *hex = 0x0;
        return (_error(ERR_CODE_CFG_PATT_INVALID_BIN2HEX));

  }
  return (ERR_CODE_NONE);
}

/* Repeater Only APIs (Not applicable to PMD) */

/* Setup the Ultra low latency clk and datapath for TX [Return Val = Error Code (0 = PASS)] */
static err_code_t _ull_tx_mode_setup(const phymod_access_t *pa, uint8_t enable) {
    EFUN(wr_falcon2_madura_ams_tx_pdet_mode(enable));                       /* Randomize bit[0] */
    EFUN(wr_falcon2_madura_ams_tx_pdet_ll_clk(enable));                     /* New reister bit to enable ull PD mode */

    EFUN(wr_falcon2_madura_tx_pi_hs_fifo_phserr_invert(0x0));
    EFUN(wr_falcon2_madura_tx_pi_ext_phase_bwsel_integ(0x7));            /* 0 to 7 */
    EFUN(wr_falcon2_madura_tx_pi_ext_ctrl_en(enable));                      /* Turn on jitter filter's phase detector input path  */

    EFUN(falcon2_madura_delay_us(1024));                          /* Wait for phase detector path to settle */

    EFUN(wr_falcon2_madura_tx_pi_second_order_loop_en(enable));             /* Turn on 2nd order loop in jitter filter */

    EFUN(wr_falcon2_madura_ams_tx_fifo_depth(0x6));

    EFUN(wr_falcon2_madura_afe_tx_fifo_resetb(enable));                     /* Release user reset to FIFO */
                                                          /* When the correct event occurs, reset is released to AFE's actual FIFO */
    EFUN(falcon2_madura_poll_st_afe_tx_fifo_resetb_equals_1( pa, 1));  /* Check if FIFO reset is reported to be released within 1ms time interval */

    EFUN(wr_falcon2_madura_ams_tx_sel_ll(enable));                          /* Switch the tx data mux to ULL traffic and check for data integrity */
    return (ERR_CODE_NONE);
}
/* Setup the Ultra low latency for RX [Return Val = Error Code (0 = PASS)] */
static err_code_t _ull_rx_mode_setup(const phymod_access_t *pa, uint8_t enable) {
    EFUN(wr_falcon2_madura_ams_rx_data12t_ll_en(enable));                   /* Enable RX ULL datapath */
    return (ERR_CODE_NONE);
}




/********************************************************/
/*  Global RAM access through Micro Register Interface  */
/********************************************************/
/* Micro Global RAM Byte Read */
static uint8_t _falcon2_madura_rdb_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
    uint8_t rddata;

    if(!err_code_p) {
        return(0);
    }

    EPSTM(rddata = falcon2_madura_rdb_uc_ram( pa, err_code_p, addr)); /* Use Micro register interface for reading RAM */

    return (rddata);
}

/* Micro Global RAM Word Read */
static uint16_t _falcon2_madura_rdw_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
  uint16_t rddata;

  if(!err_code_p) {
      return(0);
  }

  if (addr%2 != 0) {                                                                /* Validate even address */
      *err_code_p = ERR_CODE_INVALID_RAM_ADDR;
      return (0);
  }
#ifdef ATE_LOG
  EFUN_PRINTF(("// ATE_LOG IGNORE FOLLOWING READ ACCESS!\n"));
#endif
  EPSTM(rddata = falcon2_madura_rdw_uc_ram( pa, err_code_p, (addr))); /* Use Micro register interface for reading RAM */

  return (rddata);
}

/* Micro Global RAM Byte Write  */
static err_code_t _falcon2_madura_wrb_uc_var( const phymod_access_t *pa, uint16_t addr, uint8_t wr_val) {

    return (falcon2_madura_wrb_uc_ram( pa, addr, wr_val));                                /* Use Micro register interface for writing RAM */
}
/* Micro Global RAM Word Write  */
static err_code_t _falcon2_madura_wrw_uc_var( const phymod_access_t *pa, uint16_t addr, uint16_t wr_val) {
    if (addr%2 != 0) {                                                                /* Validate even address */
        return (_error(ERR_CODE_INVALID_RAM_ADDR));
    }
    return (falcon2_madura_wrw_uc_ram( pa, addr, wr_val));                                 /* Use Micro register interface for writing RAM */
}

static char* _error_val_2_str(uint8_t val) {
        switch (val) {
    case INVALID_CORE_TEMP_IDX:
        return ("INVALID CORE TEMP INDEX");
        case INVALID_OTP_CONFIG:
                return ("INVALID OTP CONFIGURATION");
        case DSC_CONFIG_INVALID_REENTRY_ID:
                return ("DSC CONFIG INVALID REENTRY");
        case INVALID_REENTRY_ID:
                return ("INVALID REENTRY");
        case GENERIC_UC_ERROR:
                return ("GENERIC UC ERROR");
        default:
                return ("UNDEFINED");
        }
}



static err_code_t _display_event(const phymod_access_t *pa, uint8_t event_id,
                          uint8_t entry_len,
                          uint8_t prev_cursor,
                          uint8_t curr_cursor,
                          uint8_t post_cursor,
                          uint8_t *supp_info) {
    char *s1;
    switch (event_id) {                                                       /* decode event code */
    case EVENT_CODE_ENTRY_TO_DSC_RESET:
        EFUN_PRINTF(("  Entry to DSC reset"));
        break;
    case EVENT_CODE_RELEASE_USER_RESET:
        EFUN_PRINTF(("  Release user reset"));
        break;
    case EVENT_CODE_EXIT_FROM_DSC_RESET:
        EFUN_PRINTF(("  Exit from DSC reset"));
        break;
    case EVENT_CODE_ENTRY_TO_CORE_RESET:
        EFUN_PRINTF(("  Entry to core reset"));
        break;
    case EVENT_CODE_RELEASE_USER_CORE_RESET:
        EFUN_PRINTF(("  Release user core reset"));
        break;
    case EVENT_CODE_ACTIVE_RESTART_CONDITION:
        EFUN_PRINTF(("  Active restart condition"));
        break;
    case EVENT_CODE_EXIT_FROM_RESTART:
        EFUN_PRINTF(("  Exit from restart"));
        break;
    case EVENT_CODE_WRITE_TR_COARSE_LOCK:
        EFUN_PRINTF(("  Write timing recovery coarse lock"));
        break;
    case EVENT_CODE_CL72_READY_FOR_COMMAND:
        EFUN_PRINTF(("  General event - %d",EVENT_CODE_CL72_READY_FOR_COMMAND));
        break;
    case EVENT_CODE_EACH_WRITE_TO_CL72_TX_CHANGE_REQUEST:
        EFUN_PRINTF(("  General event - %d",EVENT_CODE_EACH_WRITE_TO_CL72_TX_CHANGE_REQUEST));
        break;
    case EVENT_CODE_FRAME_LOCK:
        EFUN_PRINTF(("  CL72 Frame locked"));
        break;
    case EVENT_CODE_LOCAL_RX_TRAINED:
        EFUN_PRINTF(("  CL72 Local Rx trained"));
        break;
    case EVENT_CODE_DSC_LOCK:
        EFUN_PRINTF(("  DSC lock"));
        break;
    case EVENT_CODE_FIRST_RX_PMD_LOCK:
        EFUN_PRINTF(("  Rx PMD lock"));
        break;
    case EVENT_CODE_PMD_RESTART_FROM_CL72_CMD_INTF_TIMEOUT:
        EFUN_PRINTF(("  PMD restart due to CL72 ready for command timeout"));
        break;
    case EVENT_CODE_LP_RX_READY:
        EFUN_PRINTF(("  CL72 Remote receiver ready"));
        break;
    case EVENT_CODE_STOP_EVENT_LOG:
        EFUN_PRINTF(("  Start reading event log"));
        break;
    case EVENT_CODE_GENERAL_EVENT_0:
        EFUN_PRINTF(("  General event 0, (0x%x%x)",post_cursor,prev_cursor));
        break;
    case EVENT_CODE_GENERAL_EVENT_1:
        EFUN_PRINTF(("  General event 1, (0x%x%x)",post_cursor,prev_cursor));
        break;
    case EVENT_CODE_GENERAL_EVENT_2:
        EFUN_PRINTF(("  General event 2, (0x%x%x)",post_cursor,prev_cursor));
        break;
    case EVENT_CODE_ERROR_EVENT:
        s1 = _error_val_2_str(post_cursor);
        EFUN_PRINTF(("  UC error event: %s", s1));
        break;
    case EVENT_CODE_NUM_TIMESTAMP_WRAPAROUND_MAXOUT:
        EFUN_PRINTF(("  Reset number of timestamp wraparounds"));
        break;
    case EVENT_CODE_RESTART_PMD_ON_CDR_LOCK_LOST:
        EFUN_PRINTF(("  Restart Rx PMD on CDR lock lost"));
        break;
    case EVENT_CODE_RESTART_PMD_ON_CLOSE_EYE:
        EFUN_PRINTF(("  Restart Rx PMD because of closed eye"));
        break;
    case EVENT_CODE_RESTART_PMD_ON_DFE_TAP_CONFIG:
        EFUN_PRINTF(("  Restart Rx PMD on maxed out DFE tap magnitude"));
        break;
    case EVENT_CODE_SM_STATUS_RESTART:
        EFUN_PRINTF(("  Check DSC SM status restart reg value"));
        break;
    case EVENT_CODE_CORE_PROGRAMMING:
        EFUN_PRINTF(("  Program core config value"));
        break;
    case EVENT_CODE_LANE_PROGRAMMING:
        EFUN_PRINTF(("  Program lane config value"));
        break;
    case EVENT_CODE_CL72_AUTO_POLARITY_CHANGE:
        EFUN_PRINTF(("  CL72 Auto Polarity Change"));
        break;
    case EVENT_CODE_RESTART_FROM_CL72_MAX_TIMEOUT:
        EFUN_PRINTF(("  Restart Rx PMD due to F-CL72 training timeout"));
        break;
    case EVENT_CODE_CL72_LOCAL_TX_CHANGED:
        EFUN_PRINTF(("  General event - %d",EVENT_CODE_CL72_LOCAL_TX_CHANGED));
        break;
    case EVENT_CODE_FIRST_WRITE_TO_CL72_TX_CHANGE_REQUEST:
        EFUN_PRINTF(("  First write to LP Cl72 transmit change request"));
        break;
    case EVENT_CODE_FRAME_UNLOCK:
        EFUN_PRINTF(("  General event - %d",EVENT_CODE_FRAME_UNLOCK));
        break;
    case EVENT_CODE_ENTRY_TO_CORE_PLL1_RESET:
        EFUN_PRINTF(("  Entry to core PLL1 reset"));
        break;
    case EVENT_CODE_RELEASE_USER_CORE_PLL1_RESET:
        EFUN_PRINTF(("  Release user core PLL1 reset"));
        break;

    default:
        EFUN_PRINTF(("  UNRECOGNIZED EVENT CODE (0x%x) !!!",event_id));
        break;
    }

    if (entry_len == 4) {
        EFUN_PRINTF(("\n"));
    }
    else {
        int ii=0;
        EFUN_PRINTF((", SUP_INFO={"));
        supp_info += (entry_len-5);
        for (ii=0; ii<entry_len-4; ii++) {
            if (ii != 0) {
                EFUN_PRINTF((", "));
            }
            EFUN_PRINTF(("0x%x",*supp_info));
            supp_info--;
        }
        EFUN_PRINTF(("}\n"));
        if ((event_id >= EVENT_CODE_MAX) && (event_id < EVENT_CODE_TIMESTAMP_WRAP_AROUND)) {
            /* newline for unrecognized event */
            EFUN_PRINTF(("\n"));
        }
    }

    return(ERR_CODE_NONE);
}



#ifdef TO_FLOATS
/* convert uint32_t to float8 */
static float8_t _int32_to_float8(uint32_t input) {
        int8_t cnt;
        uint8_t output;

        if(input == 0) {
                return(0);
        } else if(input == 1) {
                return(0xe0);
        } else {
                cnt = 0;
                input = input & 0x7FFFFFFF; /* get rid of MSB which may be lock indicator */
                do {
                        input = input << 1;
                        cnt++;
                } while ((input & 0x80000000) == 0);

                output = (uint8_t)((input & 0x70000000)>>23)+(31 - cnt%32);
                        return(output);
        }
}
#endif

/* convert float8 to uint32_t */
static uint32_t _float8_to_int32(float8_t input) {
        uint32_t x;
        if(input == 0) return(0);
        x = (input>>5) + 8;
        if((input & 0x1F) < 3) {
                return(x>>(3-(input & 0x1f)));
        }
        return(x<<((input & 0x1F)-3));
}

/* convert float12 to uint32 */
static uint32_t _float12_to_uint32(uint8_t byte, uint8_t multi) {

        return(((uint32_t)byte)<<multi);
}


static err_code_t _set_rx_pf_main(const phymod_access_t *pa, uint8_t val) {
        if (val > 15) {
                return (_error(ERR_CODE_PF_INVALID));
        }
        EFUN(wr_falcon2_madura_rx_pf_ctrl(val));
        return(ERR_CODE_NONE);
}

static err_code_t _get_rx_pf_main(const phymod_access_t *pa, int8_t *val) {
                ESTM(*val = (int8_t)rd_falcon2_madura_rx_pf_ctrl());
 return (ERR_CODE_NONE);
}

static err_code_t _set_rx_pf2(const phymod_access_t *pa, uint8_t val) {
        if (val > 7) {
                return (_error(ERR_CODE_PF_INVALID));
        }
        EFUN(wr_falcon2_madura_rx_pf2_ctrl(val));
        return(ERR_CODE_NONE);
}

static err_code_t _get_rx_pf2(const phymod_access_t *pa, int8_t *val) {
                ESTM(*val = (int8_t)rd_falcon2_madura_rx_pf2_ctrl());
 return (ERR_CODE_NONE);
}

static err_code_t _set_rx_vga(const phymod_access_t *pa, uint8_t val) {

    EFUN(_check_uc_lane_stopped( pa ));                     /* make sure uC is stopped to avoid race conditions */

        if (val > 39) {
                return (_error(ERR_CODE_VGA_INVALID));
        }
        EFUN(wr_falcon2_madura_rx_vga_ctrl_val(val));
        EFUN(wr_falcon2_madura_vga_write(1));
        return(ERR_CODE_NONE);
}

static err_code_t _get_rx_vga(const phymod_access_t *pa, int8_t *val) {
                ESTM(*val = (int8_t)rd_falcon2_madura_rx_vga_ctrl());
 return (ERR_CODE_NONE);
}


SDK_STATIC err_code_t _set_rx_dfe1( const phymod_access_t *pa, int8_t val) {

    EFUN(_check_uc_lane_stopped( pa ));                     /* make sure uC is stopped to avoid race conditions */

        EFUN(wr_falcon2_madura_data_thresh_sel_val(val));
        EFUN(wr_falcon2_madura_data_thresh_write(1));
        return(ERR_CODE_NONE);
}
SDK_STATIC err_code_t _get_rx_dfe1(const phymod_access_t *pa, int8_t *val) {
        ESTM(*val = (int8_t)rd_falcon2_madura_rx_data_thresh_sel());
 return (ERR_CODE_NONE);
}


SDK_STATIC err_code_t _set_rx_dfe2( const phymod_access_t *pa, int8_t val) {
    if ((val > 15) || (val < -15))
        return (_error(ERR_CODE_DFE_TAP));
    EFUN(wr_falcon2_madura_rxa_dfe_tap2(val));
    EFUN(wr_falcon2_madura_rxb_dfe_tap2(val));
    EFUN(wr_falcon2_madura_rxc_dfe_tap2(val));
    EFUN(wr_falcon2_madura_rxd_dfe_tap2(val));
    return(ERR_CODE_NONE);
}

SDK_STATIC err_code_t _get_rx_dfe2(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = (int8_t)((rd_falcon2_madura_rxa_dfe_tap2() + rd_falcon2_madura_rxb_dfe_tap2() + rd_falcon2_madura_rxc_dfe_tap2() + rd_falcon2_madura_rxd_dfe_tap2())>>2));
 return (ERR_CODE_NONE);
}

static err_code_t _set_rx_dfe3( const phymod_access_t *pa, int8_t val) {
    if ((val > 15) || (val < -15))
        return (_error(ERR_CODE_DFE_TAP));
    EFUN(wr_falcon2_madura_rxa_dfe_tap3(val));
    EFUN(wr_falcon2_madura_rxb_dfe_tap3(val));
    EFUN(wr_falcon2_madura_rxc_dfe_tap3(val));
    EFUN(wr_falcon2_madura_rxd_dfe_tap3(val));
    return(ERR_CODE_NONE);
}

static err_code_t _get_rx_dfe3(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = (rd_falcon2_madura_rxa_dfe_tap3() + rd_falcon2_madura_rxb_dfe_tap3() + rd_falcon2_madura_rxc_dfe_tap3() + rd_falcon2_madura_rxd_dfe_tap3())>>2);
 return (ERR_CODE_NONE);
}

static err_code_t _set_rx_dfe4( const phymod_access_t *pa, int8_t val) {
    if ((val > 7) || (val < -7))
            return (_error(ERR_CODE_DFE_TAP));
    EFUN(wr_falcon2_madura_rxa_dfe_tap4(val));
    EFUN(wr_falcon2_madura_rxb_dfe_tap4(val));
    EFUN(wr_falcon2_madura_rxc_dfe_tap4(val));
    EFUN(wr_falcon2_madura_rxd_dfe_tap4(val));
    return(ERR_CODE_NONE);
}

static err_code_t _get_rx_dfe4(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = (rd_falcon2_madura_rxa_dfe_tap4() + rd_falcon2_madura_rxb_dfe_tap4() + rd_falcon2_madura_rxc_dfe_tap4() + rd_falcon2_madura_rxd_dfe_tap4())>>2);
    return (ERR_CODE_NONE);
}

static err_code_t _set_rx_dfe5( const phymod_access_t *pa, int8_t val) {
    if ((val > 7) || (val < -7))
        return (_error(ERR_CODE_DFE_TAP));
    EFUN(wr_falcon2_madura_rxa_dfe_tap5(val));
    EFUN(wr_falcon2_madura_rxb_dfe_tap5(val));
    EFUN(wr_falcon2_madura_rxc_dfe_tap5(val));
    EFUN(wr_falcon2_madura_rxd_dfe_tap5(val));
    return(ERR_CODE_NONE);
}

static err_code_t _get_rx_dfe5(const phymod_access_t *pa, int8_t *val) {
        ESTM(*val = (rd_falcon2_madura_rxa_dfe_tap5() + rd_falcon2_madura_rxb_dfe_tap5() + rd_falcon2_madura_rxc_dfe_tap5() + rd_falcon2_madura_rxd_dfe_tap5())>>2);
 return (ERR_CODE_NONE);
}

static err_code_t _set_rx_dfe6( const phymod_access_t *pa, int8_t val) {
    if ((val > 15) || (val < -15))
            return (_error(ERR_CODE_DFE_TAP));
    EFUN(wr_falcon2_madura_rxa_dfe_tap6(val));
    EFUN(wr_falcon2_madura_rxb_dfe_tap6(val));
    EFUN(wr_falcon2_madura_rxc_dfe_tap6(val));
    EFUN(wr_falcon2_madura_rxd_dfe_tap6(val));
    return(ERR_CODE_NONE);
}
static err_code_t _get_rx_dfe6(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = (rd_falcon2_madura_rxa_dfe_tap6() + rd_falcon2_madura_rxb_dfe_tap6() + rd_falcon2_madura_rxc_dfe_tap6() + rd_falcon2_madura_rxd_dfe_tap6())>>2);
    return (ERR_CODE_NONE);
}

static err_code_t _set_rx_dfe7( const phymod_access_t *pa, int8_t val) {
    if ((val > 15) || (val < -15))
        return (_error(ERR_CODE_DFE_TAP));
    EFUN(wr_falcon2_madura_rxa_dfe_tap7(val));
    EFUN(wr_falcon2_madura_rxb_dfe_tap7(val));
    EFUN(wr_falcon2_madura_rxc_dfe_tap7(val));
    EFUN(wr_falcon2_madura_rxd_dfe_tap7(val));
    return(ERR_CODE_NONE);
}
static err_code_t _get_rx_dfe7(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = (rd_falcon2_madura_rxa_dfe_tap7() + rd_falcon2_madura_rxb_dfe_tap7() + rd_falcon2_madura_rxc_dfe_tap7() + rd_falcon2_madura_rxd_dfe_tap7())>>2);
    return (ERR_CODE_NONE);
}

static err_code_t _set_rx_dfe8( const phymod_access_t *pa, int8_t val) {
    if ((val > 15) || (val < -15))
        return (_error(ERR_CODE_DFE_TAP));
    EFUN(wr_falcon2_madura_rxa_dfe_tap8(val));
    EFUN(wr_falcon2_madura_rxb_dfe_tap8(val));
    EFUN(wr_falcon2_madura_rxc_dfe_tap8(val));
    EFUN(wr_falcon2_madura_rxd_dfe_tap8(val));
    return(ERR_CODE_NONE);
}

static err_code_t _get_rx_dfe8( const phymod_access_t *pa, int8_t *val) {

    ESTM(*val = (rd_falcon2_madura_rxa_dfe_tap8() + rd_falcon2_madura_rxb_dfe_tap8() + rd_falcon2_madura_rxc_dfe_tap8() + rd_falcon2_madura_rxd_dfe_tap8())>>2);
    return (ERR_CODE_NONE);
}


static err_code_t _set_rx_dfe9( const phymod_access_t *pa, int8_t val) {
    if ((val > 15) || (val < -15))
        return (_error(ERR_CODE_DFE_TAP));
    EFUN(wr_falcon2_madura_rxa_dfe_tap9(val));
    EFUN(wr_falcon2_madura_rxb_dfe_tap9(val));
    EFUN(wr_falcon2_madura_rxc_dfe_tap9(val));
    EFUN(wr_falcon2_madura_rxd_dfe_tap9(val));
    return(ERR_CODE_NONE);
}
static err_code_t _get_rx_dfe9( const phymod_access_t *pa, int8_t *val) {

    ESTM(*val = (rd_falcon2_madura_rxa_dfe_tap9() + rd_falcon2_madura_rxb_dfe_tap9() + rd_falcon2_madura_rxc_dfe_tap9() + rd_falcon2_madura_rxd_dfe_tap9())>>2);
    return (ERR_CODE_NONE);
}


static err_code_t _set_rx_dfe10( const phymod_access_t *pa, int8_t val) {
    if ((val > 15) || (val < -15))
        return (_error(ERR_CODE_DFE_TAP));
    EFUN(wr_falcon2_madura_rxa_dfe_tap10(val));
    EFUN(wr_falcon2_madura_rxb_dfe_tap10(val));
    EFUN(wr_falcon2_madura_rxc_dfe_tap10(val));
    EFUN(wr_falcon2_madura_rxd_dfe_tap10(val));
    return(ERR_CODE_NONE);
}
static err_code_t _get_rx_dfe10(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = (rd_falcon2_madura_rxa_dfe_tap10() + rd_falcon2_madura_rxb_dfe_tap10() + rd_falcon2_madura_rxc_dfe_tap10() + rd_falcon2_madura_rxd_dfe_tap10())>>2);
    return (ERR_CODE_NONE);
}


static err_code_t _set_rx_dfe11( const phymod_access_t *pa, int8_t val) {
    if ((val > 15) || (val < -15))
        return (_error(ERR_CODE_DFE_TAP));
    EFUN(wr_falcon2_madura_rxa_dfe_tap11(val));
    EFUN(wr_falcon2_madura_rxb_dfe_tap11(val));
    EFUN(wr_falcon2_madura_rxc_dfe_tap11(val));
    EFUN(wr_falcon2_madura_rxd_dfe_tap11(val));
    return(ERR_CODE_NONE);
}

static err_code_t _get_rx_dfe11(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = (rd_falcon2_madura_rxa_dfe_tap11() + rd_falcon2_madura_rxb_dfe_tap11() + rd_falcon2_madura_rxc_dfe_tap11() + rd_falcon2_madura_rxd_dfe_tap11())>>2);
    return (ERR_CODE_NONE);
}


static err_code_t _set_rx_dfe12( const phymod_access_t *pa, int8_t val) {
    if ((val > 15) || (val < -15))
        return (_error(ERR_CODE_DFE_TAP));
    EFUN(wr_falcon2_madura_rxa_dfe_tap12(val));
    EFUN(wr_falcon2_madura_rxb_dfe_tap12(val));
    EFUN(wr_falcon2_madura_rxc_dfe_tap12(val));
    EFUN(wr_falcon2_madura_rxd_dfe_tap12(val));
    return(ERR_CODE_NONE);
}


static err_code_t _get_rx_dfe12(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = (rd_falcon2_madura_rxa_dfe_tap12() + rd_falcon2_madura_rxb_dfe_tap12() + rd_falcon2_madura_rxc_dfe_tap12() + rd_falcon2_madura_rxd_dfe_tap12())>>2);
    return (ERR_CODE_NONE);
}


static err_code_t _set_rx_dfe13( const phymod_access_t *pa, int8_t val) {
    if ((val > 15) || (val < -15))
        return (_error(ERR_CODE_DFE_TAP));
    EFUN(wr_falcon2_madura_rxa_dfe_tap13(val));
    EFUN(wr_falcon2_madura_rxb_dfe_tap13(val));
    EFUN(wr_falcon2_madura_rxc_dfe_tap13(val));
    EFUN(wr_falcon2_madura_rxd_dfe_tap13(val));
    return(ERR_CODE_NONE);
}


static err_code_t _get_rx_dfe13(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = (rd_falcon2_madura_rxa_dfe_tap13() + rd_falcon2_madura_rxb_dfe_tap13() + rd_falcon2_madura_rxc_dfe_tap13() + rd_falcon2_madura_rxd_dfe_tap13())>>2);
    return (ERR_CODE_NONE);
}


static err_code_t _set_rx_dfe14( const phymod_access_t *pa, int8_t val) {

    if ((val > 15) || (val < -15))
        return (_error(ERR_CODE_DFE_TAP));
    EFUN(wr_falcon2_madura_rxa_dfe_tap14(val));
    EFUN(wr_falcon2_madura_rxb_dfe_tap14(val));
    EFUN(wr_falcon2_madura_rxc_dfe_tap14(val));
    EFUN(wr_falcon2_madura_rxd_dfe_tap14(val));
    return(ERR_CODE_NONE);
}


static err_code_t _get_rx_dfe14(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = (rd_falcon2_madura_rxa_dfe_tap14() + rd_falcon2_madura_rxb_dfe_tap14() + rd_falcon2_madura_rxc_dfe_tap14() + rd_falcon2_madura_rxd_dfe_tap14())>>2);
    return (ERR_CODE_NONE);
}


static err_code_t _set_tx_post1( const phymod_access_t *pa, uint8_t val) {
    if (val > 63) {
        return (_error(ERR_CODE_TXFIR_POST1_INVALID));
    }
    EFUN(wr_falcon2_madura_cl93n72_txfir_post(val));
    return(ERR_CODE_NONE);
}


static err_code_t _set_tx_pre( const phymod_access_t *pa, uint8_t val) {
    if (val > 31) {
        return (_error(ERR_CODE_TXFIR_PRE_INVALID));
    }
    EFUN(wr_falcon2_madura_cl93n72_txfir_pre(val));
    return(ERR_CODE_NONE);
}


static err_code_t _set_tx_main( const phymod_access_t *pa, uint8_t val) {
    if (val > 112) {
        return (_error(ERR_CODE_TXFIR_MAIN_INVALID));
    }
    EFUN(wr_falcon2_madura_cl93n72_txfir_main(val));
    return(ERR_CODE_NONE);
}

static err_code_t _set_tx_post2( const phymod_access_t *pa, int8_t val) {
    if ((val > 15) || (val < -15)) {
        return (_error(ERR_CODE_TXFIR_POST2_INVALID));
    }
    /* sign? */
    EFUN(wr_falcon2_madura_txfir_post2(val));
    return(ERR_CODE_NONE);
}


static err_code_t _get_tx_pre(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = rd_falcon2_madura_txfir_pre_after_ovr());
    return (ERR_CODE_NONE);
}


static err_code_t _get_tx_main(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = rd_falcon2_madura_txfir_main_after_ovr());
    return (ERR_CODE_NONE);
}


static err_code_t _get_tx_post1(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = rd_falcon2_madura_txfir_post_after_ovr());
    return (ERR_CODE_NONE);
}


static err_code_t _get_tx_post2(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = rd_falcon2_madura_txfir_post2_adjusted());
    return (ERR_CODE_NONE);
}


static err_code_t _set_tx_post3( const phymod_access_t *pa, int8_t val) {
    if ((val > 7) || (val < -7)) {
        return (_error(ERR_CODE_TXFIR_POST3_INVALID));
    }
    /* sign? */
    EFUN(wr_falcon2_madura_txfir_post3(val));

    return(ERR_CODE_NONE);
}


static err_code_t _get_tx_post3(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = rd_falcon2_madura_txfir_post3_adjusted());
    return(ERR_CODE_NONE);
}


static err_code_t _set_tx_amp( const phymod_access_t *pa, int8_t val) {
    if (val > 15) {
        return (_error(ERR_CODE_TX_AMP_CTRL_INVALID));
    }

    EFUN(wr_falcon2_madura_ams_tx_amp_ctl(val));
    return(ERR_CODE_NONE);
}

static err_code_t _get_tx_amp(const phymod_access_t *pa, int8_t *val) {
    ESTM(*val = rd_falcon2_madura_ams_tx_amp_ctl());
    return(ERR_CODE_NONE);
}


static err_code_t _falcon2_madura_core_clkgate( const phymod_access_t *pa, uint8_t enable) {

    if (enable) {
    }
    else {
    }
    return (ERR_CODE_NONE);
}

static err_code_t _falcon2_madura_lane_clkgate( const phymod_access_t *pa, uint8_t enable) {

    if (enable) {
        /* Use frc/frc_val to force all RX and TX clk_vld signals to 0 */
        EFUN(wr_falcon2_madura_pmd_rx_clk_vld_frc_val(0x0));
        EFUN(wr_falcon2_madura_pmd_rx_clk_vld_frc(0x1));
        EFUN(wr_falcon2_madura_pmd_tx_clk_vld_frc_val(0x0));
        EFUN(wr_falcon2_madura_pmd_tx_clk_vld_frc(0x1));

        /* Use frc_on to force clkgate */
        EFUN(wr_falcon2_madura_ln_rx_s_clkgate_frc_on(0x1));
        EFUN(wr_falcon2_madura_ln_tx_s_clkgate_frc_on(0x1));

    }
    else {
        EFUN(wr_falcon2_madura_pmd_rx_clk_vld_frc_val(0x0));
        EFUN(wr_falcon2_madura_pmd_rx_clk_vld_frc(0x0));
        EFUN(wr_falcon2_madura_pmd_tx_clk_vld_frc_val(0x0));
        EFUN(wr_falcon2_madura_pmd_tx_clk_vld_frc(0x1));

        EFUN(wr_falcon2_madura_ln_rx_s_clkgate_frc_on(0x0));
        EFUN(wr_falcon2_madura_ln_tx_s_clkgate_frc_on(0x0));
    }
    return (ERR_CODE_NONE);
}

SDK_STATIC uint16_t _eye_to_mUI(const phymod_access_t *pa, uint8_t var)
{
    /* var is in units of 1/512 th UI, so need to multiply by 1000/512 */
    return ((uint16_t)var)*125/64;
}


SDK_STATIC uint16_t _eye_to_mV(const phymod_access_t *pa, uint8_t var, uint8_t ladder_range)
{
    /* find nearest two vertical levels */
    uint16_t vl = _ladder_setting_to_mV(pa,var, ladder_range);
    return (vl);
}

SDK_STATIC err_code_t _falcon2_madura_get_osr_mode( const phymod_access_t *pa, falcon2_madura_osr_mode_st *imode) {
    falcon2_madura_osr_mode_st mode;

     ENULL_MEMSET(&mode, 0, sizeof(falcon2_madura_osr_mode_st));

    if(!imode)
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

  ESTM(mode.tx = rd_falcon2_madura_tx_osr_mode());
  ESTM(mode.rx = rd_falcon2_madura_rx_osr_mode());
  mode.tx_rx = 255;
  *imode = mode;
  return (ERR_CODE_NONE);

}

/*SDK_STATIC */err_code_t _falcon2_madura_read_lane_state(const phymod_access_t *pa, falcon2_madura_lane_state_st *istate) {

  falcon2_madura_lane_state_st state;
  uint8_t ladder_range = 0;
  uint16_t reg_data;

  ENULL_MEMSET(&state, 0, sizeof(falcon2_madura_lane_state_st));

  if(!istate)
          return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

  EFUN(_falcon2_madura_pmd_lock_status( pa, &state.rx_lock, &state.rx_lock_chg));

  if (state.rx_lock == 1) {
      ESTM(state.stop_state = rdv_falcon2_madura_usr_sts_micro_stopped());
      if (!state.stop_state) {
          EFUN(falcon2_madura_stop_rx_adaptation( pa, 1));
      }
  } else {
      EFUN(falcon2_madura_pmd_uc_control( pa, CMD_UC_CTRL_STOP_IMMEDIATE,200));
  }

  {  falcon2_madura_osr_mode_st osr_mode;
  ENULL_MEMSET(&osr_mode, 0, sizeof(falcon2_madura_osr_mode_st));
  EFUN(_falcon2_madura_get_osr_mode( pa, &osr_mode));
  state.osr_mode = osr_mode;
  }
  ESTM(state.ucv_config = rdv_falcon2_madura_config_word());
  ESTM(state.reset_state    = rd_falcon2_madura_rx_lane_dp_reset_state());
  ESTM(state.tx_reset_state = rd_falcon2_madura_tx_lane_dp_reset_state());
  ESTM(state.tx_pll_select = rd_falcon2_madura_tx_pll_select());
  ESTM(state.rx_pll_select = rd_falcon2_madura_rx_pll_select());
  EFUN(_falcon2_madura_sigdet_status( pa, &state.sig_det, &state.sig_det_chg));
  ESTM(state.rx_ppm = rd_falcon2_madura_cdr_integ_reg()/84);
  EFUN(falcon2_madura_get_clk90_offset( pa, &state.clk90));
  EFUN(falcon2_madura_pmd_rdt_reg(pa,DSC_E_RX_PI_CNT_BIN_LD,&reg_data));
  state.clkp1 = (uint8_t)dist_ccw(((reg_data>>8)&0xFF),(reg_data&0xFF));
  ESTM(state.br_pd_en = rd_falcon2_madura_br_pd_en());
  /* drop the MSB, the result is actually valid modulo 128 */
  /* Also flip the sign to account for d-m1, versus m1-d */
  state.clkp1 = state.clkp1<<1;
  state.clkp1 = -(state.clkp1>>1);

  EFUN(_get_rx_pf_main( pa, &state.pf_main));
  state.pf_hiz = 0;
  EFUN(_get_rx_pf2( pa, &state.pf2_ctrl));
  EFUN(_get_rx_vga( pa, &state.vga));
  ESTM(state.dc_offset = rd_falcon2_madura_dc_offset_bin());
  ESTM(state.p1_lvl = rdv_falcon2_madura_usr_main_tap_est()/32);
  state.p1_lvl = _ladder_setting_to_mV(pa,(int8_t)state.p1_lvl, 0);
  ESTM(state.m1_lvl = rd_falcon2_madura_rx_phase_thresh_sel());
  state.m1_lvl = _ladder_setting_to_mV(pa,(int8_t)state.m1_lvl, 0);

  /* ESTM(state.pf_bst = rd_falcon2_madura_ams_rx_pkbst()); */
  state.pf_bst = 0;

  EFUN(_get_rx_dfe1( pa, &state.dfe1));
  EFUN(_get_rx_dfe2( pa, &state.dfe2));
  EFUN(_get_rx_dfe3( pa, &state.dfe3));
  EFUN(_get_rx_dfe4( pa, &state.dfe4));
  EFUN(_get_rx_dfe5( pa, &state.dfe5));
  EFUN(_get_rx_dfe6( pa, &state.dfe6));


  /* tx_ppm = register/10.84 */
  ESTM(state.tx_ppm = (int16_t)(((int32_t)(rd_falcon2_madura_tx_pi_integ2_reg()))*3125/32768));

  EFUN(_get_tx_pre( pa, &state.txfir_pre));
  EFUN(_get_tx_main( pa, &state.txfir_main));
  EFUN(_get_tx_post1( pa, &state.txfir_post1));
  EFUN(_get_tx_post2( pa, &state.txfir_post2));
  EFUN(_get_tx_post3( pa, &state.txfir_post3));
  EFUN(_get_tx_amp( pa, &state.tx_amp));
  ESTM(state.drivermode = rd_falcon2_madura_ams_tx_driver_mode());
  ESTM(state.heye_left = _eye_to_mUI(pa,rdv_falcon2_madura_usr_sts_heye_left()));
  ESTM(state.heye_right = _eye_to_mUI(pa,rdv_falcon2_madura_usr_sts_heye_right()));
  ESTM(state.veye_upper = _eye_to_mV(pa,rdv_falcon2_madura_usr_sts_veye_upper(), ladder_range));
  ESTM(state.veye_lower = _eye_to_mV(pa,rdv_falcon2_madura_usr_sts_veye_lower(), ladder_range));
  ESTM(state.link_time = (((uint32_t)rdv_falcon2_madura_usr_sts_link_time())*8)/10);     /* convert from 80us to 0.1 ms units */

  if (state.rx_lock == 1) {
      if (!state.stop_state) {
          EFUN(falcon2_madura_stop_rx_adaptation( pa, 0));
      }
  } else {
      EFUN(falcon2_madura_stop_rx_adaptation( pa, 0));
  }

  *istate = state;
  return (ERR_CODE_NONE);
}

static err_code_t _falcon2_madura_display_lane_state_no_newline(const phymod_access_t *pa) {
  uint16_t lane_idx;
  falcon2_madura_lane_state_st state;

  const char* e2s_osr_mode_enum[14] = {
    "x1       ",
    "x2       ",
    "x4       ",
    "ERR      ",
    "ERR      ",
    "x8       ",
    "ERR      ",
    "ERR      ",
    "x16.5    ",
    "x16      ",
    "ERR      ",
    "ERR      ",
    "x20.6    ",
    "x32      "
  };

  const char* e2s_tx_osr_mode_enum[14] = {
    "x1  ",
    "x2  ",
    "x4  ",
    "ERR ",
    "ERR ",
    "x8  ",
    "ERR ",
    "ERR ",
    "x16p",
    "x16 ",
    "ERR ",
    "ERR ",
    "x20p",
    "x32 "
  };


  ENULL_MEMSET(&state, 0, sizeof(falcon2_madura_lane_state_st));

  EFUN(_falcon2_madura_read_lane_state( pa, &state));

  lane_idx = falcon2_madura_get_lane(pa);
  EFUN_PRINTF(("%2d ", lane_idx));
  if(state.osr_mode.tx_rx != 255) {
      char *s;
      s = (char *)e2s_osr_mode_enum[state.osr_mode.tx_rx];
      EFUN_PRINTF(("(%2s%s, 0x%04x,", (state.br_pd_en) ? "BR" : "OS", s, state.ucv_config));
  } else {
      char *s;
      char *r;
      s = (char *)e2s_tx_osr_mode_enum[state.osr_mode.tx];
      r = (char *)e2s_tx_osr_mode_enum[state.osr_mode.rx];
      EFUN_PRINTF(("(%2s%s:%s, 0x%04x,", (state.br_pd_en) ? "BR" : "OS", s,r, state.ucv_config));
  }
  if (state.tx_pll_select != 255) {
      EFUN_PRINTF(("   %01x,%01x   , %01x ,%1d,%1d) ",state.tx_reset_state,state.reset_state,state.stop_state,state.tx_pll_select,state.rx_pll_select));
  }
  else {
      EFUN_PRINTF((" %01x , %01x ) ",state.reset_state,state.stop_state));
  }
  if (state.sig_det_chg) {
    EFUN_PRINTF((" %1d*", state.sig_det));
  }
  else {
    EFUN_PRINTF((" %1d ", state.sig_det));
  }
  if (state.rx_lock_chg) {
    EFUN_PRINTF(("  %1d*", state.rx_lock));
  }
  else {
    EFUN_PRINTF(("  %1d ", state.rx_lock));
  }
  EFUN_PRINTF((" %4d ", state.rx_ppm));
  EFUN_PRINTF(("  %3d   %3d ", state.clk90, state.clkp1));
  EFUN_PRINTF(("  %2d,%1d ", state.pf_main, state.pf2_ctrl));
  EFUN_PRINTF(("   %2d ", state.vga));
  EFUN_PRINTF(("%3d ", state.dc_offset));
  EFUN_PRINTF(("%4d ", state.p1_lvl));
  EFUN_PRINTF(("%4d ", state.m1_lvl));
  EFUN_PRINTF(("%3d,%3d,%3d,%3d,%3d,%3d ", state.dfe1, state.dfe2, state.dfe3, state.dfe4, state.dfe5, state.dfe6));
  EFUN_PRINTF((" %4d ", state.tx_ppm));
  EFUN_PRINTF(("  %2d,%3d,%2d,%2d,%2d  ", state.txfir_pre, state.txfir_main, state.txfir_post1, state.txfir_post2,state.txfir_post3));
  EFUN_PRINTF((" %2d,%1d ", state.tx_amp,state.drivermode));
  EFUN_PRINTF((" %3d,%3d,%3d,%3d ", state.heye_left, state.heye_right, state.veye_upper, state.veye_lower));
  EFUN_PRINTF((" %4d.%01d", state.link_time/10, state.link_time%10));
  return (ERR_CODE_NONE);
}


SDK_STATIC err_code_t _falcon2_madura_read_core_state( const phymod_access_t *pa, falcon2_madura_core_state_st *istate) {
  falcon2_madura_core_state_st state;
  struct falcon2_madura_uc_core_config_st core_cfg;

  ENULL_MEMSET(&state, 0, sizeof(falcon2_madura_core_state_st));
  ENULL_MEMSET(&core_cfg, 0, sizeof(struct falcon2_madura_uc_core_config_st));

  if (!istate)
    return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

  EFUN(falcon2_madura_get_uc_core_config( pa, &core_cfg));
  ESTM(state.rescal               = rd_falcon2_madura_ams_tx_ana_rescal());
  ESTM(state.core_reset           = rdc_falcon2_madura_core_dp_reset_state());
  ESTM(state.pll_pwrdn            = rdc_falcon2_madura_afe_s_pll_pwrdn());
  ESTM(state.uc_active            = rdc_falcon2_madura_uc_active());
  ESTM(state.comclk_mhz           = rdc_falcon2_madura_heartbeat_count_1us());
  ESTM(state.ucode_version        = rdcv_falcon2_madura_common_ucode_version());
  ESTM(state.ucode_minor_version  = rdcv_falcon2_madura_common_ucode_minor_version());
  ESTM(state.afe_hardware_version = rdcv_falcon2_madura_afe_hardware_version());
  ESTM(state.temp_idx             = rdcv_falcon2_madura_temp_idx());
  {  int16_t                           die_temp = 0;
     EFUN(falcon2_madura_read_die_temperature( pa, &die_temp));
     state.die_temp               =    die_temp;
  }
  ESTM(state.avg_tmon             = rdcv_falcon2_madura_avg_tmon_reg13bit());
  state.vco_rate_mhz            = (uint16_t)core_cfg.vco_rate_in_Mhz;
  ESTM(state.analog_vco_range     = rdc_falcon2_madura_ams_pll_range());
  ESTM(state.pll_div              = rdc_falcon2_madura_ams_pll_ndiv());
  EFUN(_falcon2_madura_pll_lock_status( pa, &state.pll_lock, &state.pll_lock_chg));
  ESTM(state.core_status          = rdcv_falcon2_madura_status_byte());

  *istate = state;
  return (ERR_CODE_NONE);
}

static err_code_t _falcon2_madura_display_core_state_no_newline(const phymod_access_t *pa) {
    falcon2_madura_core_state_st state;
    falcon2_madura_core_state_st state_pll1;
    uint8_t pll_orig;

    ENULL_MEMSET(&state_pll1, 0, sizeof(state_pll1));
    ENULL_MEMSET(&state     , 0, sizeof(state     ));
    {
        ESTM(pll_orig = falcon2_madura_get_pll(pa));
        EFUN(falcon2_madura_set_pll(pa,0));
        EFUN(_falcon2_madura_read_core_state( pa, &state));
        EFUN(falcon2_madura_set_pll(pa,1));
        EFUN(_falcon2_madura_read_core_state( pa, &state_pll1));
        EFUN(falcon2_madura_set_pll(pa,pll_orig));
    }
    EFUN_PRINTF(("%02d  "              ,  falcon2_madura_get_core(pa)));
    EFUN_PRINTF((" %x,%x"              ,  state.core_reset, state_pll1.core_reset));
    EFUN_PRINTF(("  %02x  "            ,  state.core_status));
    EFUN_PRINTF(("  %1d,%1d    "       ,  state.pll_pwrdn,state_pll1.pll_pwrdn));
    EFUN_PRINTF(("   %1d    "          ,  state.uc_active));
    EFUN_PRINTF((" %3d.%02dMHz"        , (state.comclk_mhz/4),((state.comclk_mhz%4)*25)));    /* comclk in Mhz = heartbeat_count_1us / 4 */
    EFUN_PRINTF(("   %4X_%02X "        ,  state.ucode_version, state.ucode_minor_version));
    EFUN_PRINTF(("    0x%02X   "       ,  state.afe_hardware_version));
    EFUN_PRINTF(("   %3dC   "          ,  state.die_temp));
    EFUN_PRINTF(("   (%02d)%3dC "      ,  state.temp_idx, _bin_to_degC((state.avg_tmon>>3))));
    EFUN_PRINTF(("   0x%02X  "         ,  state.rescal));
    EFUN_PRINTF((" %2d.%03d,%2d.%03dGHz ", (state     .vco_rate_mhz / 1000),
                                           (state     .vco_rate_mhz % 1000),
                                           (state_pll1.vco_rate_mhz / 1000),
                                           (state_pll1.vco_rate_mhz % 1000)));
    EFUN_PRINTF(("  %03d,%03d   "      ,  state     .analog_vco_range,
                                          state_pll1.analog_vco_range));
    EFUN_PRINTF((" (%02d,%02d) "       ,  state     .pll_div,
                                          state_pll1.pll_div));
    EFUN(falcon2_madura_set_pll(pa,0));
    EFUN(_falcon2_madura_display_pll_to_divider( pa,   state     .pll_div));
    EFUN_PRINTF((","));
    EFUN(falcon2_madura_set_pll(pa,1));
    EFUN(_falcon2_madura_display_pll_to_divider( pa,   state_pll1.pll_div));
    if (state.pll_lock_chg) {
      EFUN_PRINTF(("    %01d*,"        ,  state.pll_lock));
    }
    else {
      EFUN_PRINTF(("    %01d ,"        ,  state.pll_lock));
    }
    if (state_pll1.pll_lock_chg) {
      EFUN_PRINTF(("%01d* "            ,  state_pll1.pll_lock));
    }
    else {
      EFUN_PRINTF(("%01d  "            ,  state_pll1.pll_lock));
    }
    EFUN(falcon2_madura_set_pll(pa,pll_orig));

    return (ERR_CODE_NONE);
}

SDK_STATIC err_code_t _falcon2_madura_pll_lock_status( const phymod_access_t *pa, uint8_t *pll_lock, uint8_t *pll_lock_chg) {

    uint16_t rddata;
    EFUN(falcon2_madura_pmd_rdt_reg(pa,PLL_STATUS_ADDR, &rddata));

    *pll_lock     = ((rddata >> 8) & 0x1);
    *pll_lock_chg = (rddata & 0x1);

    return(ERR_CODE_NONE);
}

SDK_STATIC err_code_t _falcon2_madura_pmd_lock_status( const phymod_access_t *pa, uint8_t *pmd_lock, uint8_t *pmd_lock_chg) {

    uint16_t rddata;
    EFUN(falcon2_madura_pmd_rdt_reg(pa,PMD_LOCK_STATUS_ADDR, &rddata));

    *pmd_lock     = (rddata & 0x1);
    *pmd_lock_chg = ((rddata >> 1) & 0x1);

    return(ERR_CODE_NONE);
}

SDK_STATIC err_code_t _falcon2_madura_sigdet_status( const phymod_access_t *pa, uint8_t *sig_det, uint8_t *sig_det_chg) {
    uint16_t rddata;
    EFUN(falcon2_madura_pmd_rdt_reg(pa,SIGDET_STATUS_ADDR, &rddata));

    *sig_det     = (rddata & 0x1);
    *sig_det_chg = ((rddata >> 1) & 0x1);

    return(ERR_CODE_NONE);
}


/* returns 000111 (7 = 8-1), for n = 3  */
#define BFMASK(n) ((1<<((n)))-1)

static err_code_t _update_uc_lane_config_st( const phymod_access_t *pa, struct  falcon2_madura_uc_lane_config_st *st) {
  uint16_t in = st->word;
  st->field.lane_cfg_from_pcs = in & BFMASK(1); in >>= 1;
  st->field.an_enabled = in & BFMASK(1); in >>= 1;
  st->field.dfe_on = in & BFMASK(1); in >>= 1;
  st->field.dfe_lp_mode = in & BFMASK(1); in >>= 1;
  st->field.force_brdfe_on = in & BFMASK(1); in >>= 1;
  st->field.media_type = in & BFMASK(2); in >>= 2;
  st->field.unreliable_los = in & BFMASK(1); in >>= 1;
  st->field.scrambling_dis = in & BFMASK(1); in >>= 1;
  st->field.cl72_auto_polarity_en = in & BFMASK(1); in >>= 1;
  st->field.cl72_restart_timeout_en = in & BFMASK(1); in >>= 1;
  st->field.en_short_chn_osx1 = in & BFMASK(1); in >>= 1;
  st->field.reserved = in & BFMASK(4); in >>= 4;
  return(ERR_CODE_NONE);
}

static err_code_t _update_usr_ctrl_disable_functions_st( const phymod_access_t *pa, struct falcon2_madura_usr_ctrl_disable_functions_st *st) {
  uint16_t in = st->word;
  st->field.pf_adaptation = in & BFMASK(1); in >>= 1;
  st->field.pf2_adaptation = in & BFMASK(1); in >>= 1;
  st->field.dc_adaptation = in & BFMASK(1); in >>= 1;
  st->field.vga_adaptation = in & BFMASK(1); in >>= 1;
  st->field.slicer_voffset_tuning = in & BFMASK(1); in >>= 1;
  st->field.slicer_hoffset_tuning = in & BFMASK(1); in >>= 1;
  st->field.phase_offset_adaptation = in & BFMASK(1); in >>= 1;
  st->field.eye_adaptation = in & BFMASK(1); in >>= 1;
  st->field.all_adaptation = in & BFMASK(1); in >>= 1;
  st->field.reserved = in & BFMASK(7); in >>= 7;
  return ERR_CODE_NONE;
}


static err_code_t _update_usr_ctrl_disable_dfe_functions_st( const phymod_access_t *pa, struct falcon2_madura_usr_ctrl_disable_dfe_functions_st *st) {
  uint8_t in = st->byte;
  st->field.dfe_tap1_adaptation = in & BFMASK(1); in >>= 1;
  st->field.dfe_fx_taps_adaptation = in & BFMASK(1); in >>= 1;
  st->field.dfe_fl_taps_adaptation = in & BFMASK(1); in >>= 1;
  st->field.dfe_dcd_adaptation = in & BFMASK(1); in >>= 1;
  return ERR_CODE_NONE;
}

/* word to fields, for display */
static err_code_t _update_uc_core_config_st( const phymod_access_t *pa, struct falcon2_madura_uc_core_config_st *st) {
  uint16_t in = st->word;
  st->field.vco_rate = in & BFMASK(8); in >>= 8;
  st->field.core_cfg_from_pcs = in & BFMASK(1); in >>= 1;
  st->field.disable_write_pll_iqp = in & BFMASK(1); in >>= 1;
  st->field.reserved = in & BFMASK(6); in >>= 6;
  st->vco_rate_in_Mhz = VCO_RATE_TO_MHZ(st->field.vco_rate);
  return ERR_CODE_NONE;
}

/* fields to word, to write into uC RAM */
static err_code_t _update_uc_core_config_word( const phymod_access_t *pa, struct falcon2_madura_uc_core_config_st *st) {
  uint16_t in = 0;
  in <<= 6; in |= 0/*st->field.reserved*/ & BFMASK(6);
  in <<= 1; in |= st->field.disable_write_pll_iqp & BFMASK(1);
  in <<= 1; in |= st->field.core_cfg_from_pcs & BFMASK(1);
  in <<= 8; in |= st->field.vco_rate & BFMASK(8);
  st->word = in;
  return ERR_CODE_NONE;
}

static err_code_t _update_uc_lane_config_word( const phymod_access_t *pa, struct falcon2_madura_uc_lane_config_st *st) {
  uint16_t in = 0;
  in <<= 4; in |= 0 /*st->field.reserved*/ & BFMASK(4);
  in <<= 1; in |= st->field.en_short_chn_osx1 & BFMASK(1);
  in <<= 1; in |= st->field.cl72_restart_timeout_en & BFMASK(1);
  in <<= 1; in |= st->field.cl72_auto_polarity_en & BFMASK(1);
  in <<= 1; in |= st->field.scrambling_dis & BFMASK(1);
  in <<= 1; in |= st->field.unreliable_los & BFMASK(1);
  in <<= 2; in |= st->field.media_type & BFMASK(2);
  in <<= 1; in |= st->field.force_brdfe_on & BFMASK(1);
  in <<= 1; in |= st->field.dfe_lp_mode & BFMASK(1);
  in <<= 1; in |= st->field.dfe_on & BFMASK(1);
  in <<= 1; in |= st->field.an_enabled & BFMASK(1);
  in <<= 1; in |= st->field.lane_cfg_from_pcs & BFMASK(1);
  st->word = in;
  return ERR_CODE_NONE;
}

static err_code_t _update_usr_ctrl_disable_functions_byte( const phymod_access_t *pa, struct falcon2_madura_usr_ctrl_disable_functions_st *st) {
  uint16_t in = 0;
  in <<= 7; in |= 0/*st->field.reserved*/ & BFMASK(7);
  in <<= 1; in |= st->field.all_adaptation & BFMASK(1);
  in <<= 1; in |= st->field.eye_adaptation & BFMASK(1);
  in <<= 1; in |= st->field.phase_offset_adaptation & BFMASK(1);
  in <<= 1; in |= st->field.slicer_hoffset_tuning & BFMASK(1);
  in <<= 1; in |= st->field.slicer_voffset_tuning & BFMASK(1);
  in <<= 1; in |= st->field.vga_adaptation & BFMASK(1);
  in <<= 1; in |= st->field.dc_adaptation & BFMASK(1);
  in <<= 1; in |= st->field.pf2_adaptation & BFMASK(1);
  in <<= 1; in |= st->field.pf_adaptation & BFMASK(1);
  st->word = in;
  return ERR_CODE_NONE;
}


static err_code_t _update_usr_ctrl_disable_dfe_functions_byte( const phymod_access_t *pa, struct  falcon2_madura_usr_ctrl_disable_dfe_functions_st *st) {
  uint8_t in = 0;
  in <<= 1; in |= st->field.dfe_dcd_adaptation & BFMASK(1);
  in <<= 1; in |= st->field.dfe_fl_taps_adaptation & BFMASK(1);
  in <<= 1; in |= st->field.dfe_fx_taps_adaptation & BFMASK(1);
  in <<= 1; in |= st->field.dfe_tap1_adaptation & BFMASK(1);
  st->byte = in;
  return ERR_CODE_NONE;
}

#undef BFMASK




SDK_STATIC err_code_t _calc_patt_gen_mode_sel( const phymod_access_t *pa, uint8_t *mode_sel, uint8_t *zero_pad_len, uint8_t patt_length) {

  if(!mode_sel) {
          return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
  if(!zero_pad_len) {
          return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  /* Select the correct Pattern generator Mode depending on Pattern length */
  if (!(140 % patt_length)) {
    *mode_sel = 6;
    *zero_pad_len = 100;
  }
  else if (!(160 % patt_length)) {
    *mode_sel = 5;
    *zero_pad_len = 80;
  }
  else if (!(180 % patt_length)) {
    *mode_sel = 4;
    *zero_pad_len = 60;
  }
  else if (!(200 % patt_length)) {
    *mode_sel = 3;
    *zero_pad_len = 40;
  }
  else if (!(220 % patt_length)) {
    *mode_sel = 2;
    *zero_pad_len = 20;
  }
  else if (!(240 % patt_length)) {
    *mode_sel = 1;
    *zero_pad_len = 0;
  } else {
    EFUN_PRINTF(("ERROR: Unsupported Pattern Length\n"));
    return (_error(ERR_CODE_CFG_PATT_INVALID_PATT_LENGTH));
  }
  return(ERR_CODE_NONE);
}

static err_code_t _falcon2_madura_display_pll_to_divider( const phymod_access_t *pa, uint8_t val) {

    { uint8_t mmd_en;
    ESTM(mmd_en = rdc_falcon2_madura_ams_pll_fracn_sel());
    if(mmd_en) {
        int primary;long int secondary;int frac;
        ESTM(primary = rdc_falcon2_madura_ams_pll_fracn_ndiv_int());
        ESTM(secondary = rdc_falcon2_madura_ams_pll_fracn_div());
        ESTM(secondary |= rdc_falcon2_madura_ams_pll_fracn_div_17_16()<<16);
        frac = secondary/262; /* 2^18 * 1000 */
        EFUN_PRINTF(("%3d.%03d",primary,frac));
    } else {
        switch(val) {
        case 0x00:  EFUN_PRINTF((" 64  "));
            break;
        case 0x01:  EFUN_PRINTF((" 66  "));
            break;
        case 0x02:  EFUN_PRINTF((" 80  "));
            break;
        case 0x03:  EFUN_PRINTF(("128  "));
            break;
        case 0x04:  EFUN_PRINTF(("132  "));
            break;
        case 0x05:  EFUN_PRINTF(("140  "));
            break;
        case 0x06:  EFUN_PRINTF(("160  "));
            break;
        case 0x07:  EFUN_PRINTF(("165  "));
            break;
        case 0x08:  EFUN_PRINTF(("168  "));
            break;
        case 0x09:  EFUN_PRINTF(("170  "));
            break;
        case 0x0a:  EFUN_PRINTF(("175  "));
            break;
        case 0x0b:  EFUN_PRINTF(("180  "));
            break;
        case 0x0c:  EFUN_PRINTF(("184  "));
            break;
        case 0x0d:  EFUN_PRINTF(("200  "));
            break;
        case 0x0e:  EFUN_PRINTF(("224  "));
            break;
        case 0x0f:  EFUN_PRINTF(("264  "));
            break;
        case 0x10:  EFUN_PRINTF((" 96  "));
            break;
        case 0x11:  EFUN_PRINTF(("120  "));
            break;
        case 0x12:  EFUN_PRINTF(("144  "));
            break;
        case 0x13:  EFUN_PRINTF(("198  "));
            break;
        default:    EFUN_PRINTF((" xxxx"));
            EFUN_PRINTF(("ERROR: Invalid PLL_DIV VALUE\n"));
            return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        }

    }
    }
    return(ERR_CODE_NONE);

}

static err_code_t _falcon2_madura_display_ber_scan_data( const phymod_access_t *pa, uint8_t ber_scan_mode, uint8_t timer_control, uint8_t max_error_control) {
    uint8_t i,prbs_byte,prbs_multi,time_byte,time_multi;
    uint16_t sts,dataword;
    uint8_t verbose = 0;
    int16_t offset_start;
    uint8_t cnt;
    uint32_t errors,timer_values;
    int rate,direction;
    uint8_t range250;
    falcon2_madura_osr_mode_st osr_mode;
    struct falcon2_madura_uc_core_config_st core_config;

    ENULL_MEMSET(&core_config,0,sizeof(core_config));
    ENULL_MEMSET(&osr_mode,0,sizeof(osr_mode));

    EFUN(falcon2_madura_get_uc_core_config( pa, &core_config));
    EFUN(_falcon2_madura_get_osr_mode( pa, &osr_mode));

    if(osr_mode.rx > 2) {
        EFUN_PRINTF(("ERROR DIAG display_ber_data: osr mode too high\n"));
        return(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }
    rate = core_config.vco_rate_in_Mhz/(1<<osr_mode.rx);
    direction = (ber_scan_mode & DIAG_BER_NEG) ? -1 : 1 ;
    range250 = (ber_scan_mode & DIAG_BER_P1_NARROW) ? 0 : 1;

    EFUN_PRINTF(("\n****  SERDES BER DATA    ****\n"));
    EFUN_PRINTF(("BER MODE = %x %d %d\n",ber_scan_mode,timer_control,max_error_control));
    EFUN_PRINTF(("DATA RATE = %d Mhz\n",rate));
    /* start UC acquisition */
    if(verbose > 2) EFUN_PRINTF(("start begin\n"));
    EFUN(falcon2_madura_start_ber_scan_test(pa, ber_scan_mode, timer_control, max_error_control));
    ESTM(offset_start = rd_falcon2_madura_uc_dsc_data());
    if(ber_scan_mode & DIAG_BER_HORZ) {
        EFUN_PRINTF(("STARTING OFFSET = %d : %d mUI\n",offset_start,(offset_start*1000)>>6));
    } else {
        EFUN_PRINTF(("STARTING OFFSET = %d : %d mV\n",offset_start,_ladder_setting_to_mV(pa,(int8_t)offset_start, range250)));
    }
    if(verbose > 2) EFUN_PRINTF(("start done\n"));


    /* This wait is VERY LONG and should be replaced with interupt or something */
    /* coverity[dead_error_condition] */
    if(verbose > 5) {
        do {
            EFUN(falcon2_madura_delay_us(2000000));
            ESTM(sts = rdv_falcon2_madura_usr_diag_status());
            EFUN_PRINTF(("sts=%04x\n",sts));

        } while ((sts & 0x8000) == 0);
    } else {
        EFUN_PRINTF(("Waiting for measurement time approx %d seconds",timer_control+(timer_control>>1)));
        EFUN(falcon2_madura_poll_diag_done( pa, &sts,timer_control*2000));
    }
    if(verbose > 2) EFUN_PRINTF(("delay done\n"));

    /* Check for completion read ln.diag_status byte?*/
    ESTM(sts = rdv_falcon2_madura_usr_diag_status());
    if((sts & 0x8000) == 0) {
        return(_error(ERR_CODE_DATA_NOTAVAIL));
    }
    cnt = (sts & 0x00FF)/3;
    for(i=0;i < cnt;i++) {
        /* Read 2 bytes of data */
        EFUN(falcon2_madura_pmd_uc_cmd(pa,  CMD_READ_DIAG_DATA_WORD, 0, 100));
        ESTM(dataword = rd_falcon2_madura_uc_dsc_data());           /* LSB contains 2 -4bit nibbles */
        time_byte = (uint8_t)(dataword>>8);    /* MSB is time byte */
        prbs_multi = (uint8_t)dataword & 0x0F; /* split nibbles */
        time_multi = (uint8_t)dataword>>4;
        /* Read 1 bytes of data */
        EFUN(falcon2_madura_pmd_uc_cmd(pa, CMD_READ_DIAG_DATA_BYTE, 0, 100));
        ESTM(prbs_byte = (uint8_t)rd_falcon2_madura_uc_dsc_data());
        errors = _float12_to_uint32(prbs_byte,prbs_multi); /* convert 12bits to uint32 */
        timer_values = (_float12_to_uint32(time_byte,time_multi)<<3);
        if(verbose < 5) {
            if (!(i % 4))  {
                EFUN_PRINTF(("\n"));
            }
            if(ber_scan_mode & DIAG_BER_HORZ) {
                EFUN_PRINTF(("%d %d %d ",direction*(((_abs(offset_start)-i)*1000)>>6),errors,timer_values));
            } else {
                EFUN_PRINTF(("%d %d %d ",direction*_ladder_setting_to_mV(pa,(int8_t)_abs(offset_start)-i, range250),errors,timer_values));
            }

        } else {
            EFUN_PRINTF(("BER Errors=%d (%02x<<%d): Time=%d (%02x<<%d)\n",errors,prbs_byte,prbs_multi,timer_values,time_byte,time_multi<<3));
        }
        /*if(timer_values == 0 && errors == 0) break;*/
    }
    EFUN_PRINTF(("\n"));
    EFUN(falcon2_madura_pmd_uc_cmd( pa, CMD_CAPTURE_BER_END,0x00,2000));

  return(ERR_CODE_NONE);
}

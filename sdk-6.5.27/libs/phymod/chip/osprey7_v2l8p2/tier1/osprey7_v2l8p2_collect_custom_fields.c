/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2021 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 *                                                                                 *
 *  Revision      :                                                                *
 *                                                                                 *
 *  Description   :  This is an auto generated diagnostic dump tool file           *
 *                                                                                 *
 **********************************************************************************/

#include "osprey7_v2l8p2_collect_custom_fields.h"
#include "osprey7_v2l8p2_functions.h"
#include "osprey7_v2l8p2_select_defns.h"
#include "osprey7_v2l8p2_access.h"
#include "osprey7_v2l8p2_debug_functions.h"
#include "osprey7_v2l8p2_config.h"
#include "osprey7_v2l8p2_internal.h"
#include "osprey7_v2l8p2_decode_print.h"

#ifndef SMALL_FOOTPRINT
err_code_t osprey7_v2l8p2_collect_custom_fields_osprey7_v2l8p2_detailed_lane_status_st_pre(srds_access_t *sa__, osprey7_v2l8p2_detailed_lane_status_st_t *st){
    INIT_SRDS_ERR_CODE

    EFUN(osprey7_v2l8p2_INTERNAL_pmd_lock_status(sa__,&st->pmd_lock, &st->pmd_lock_chg));
    {
        err_code_t err_code=ERR_CODE_NONE;
        st->stop_state = osprey7_v2l8p2_INTERNAL_stop_micro(sa__,st->pmd_lock,&err_code);
        if(err_code) USR_PRINTF(("Unable to stop microcontroller,  following data is suspect\n"));
    }
    return ERR_CODE_NONE;
}

err_code_t osprey7_v2l8p2_collect_custom_fields_osprey7_v2l8p2_detailed_lane_status_st_post(srds_access_t *sa__, osprey7_v2l8p2_detailed_lane_status_st_t *st){
    INIT_SRDS_ERR_CODE
    int8_t lms_thresh;
    uint8_t tap_num;
    int8_t d23, d14, d05;
    uint8_t i,x,y;
    uint16_t reg_data1;
    osprey7_v2l8p2_eye_margin_t eye_margin;
    enum osprey7_v2l8p2_rx_pam_mode_enum pam_mode = NRZ;
    enum srds_prbs_polynomial_enum prbs_poly_mode = PRBS_7;
    enum srds_prbs_checker_mode_enum prbs_checker_mode = PRBS_SELF_SYNC_HYSTERESIS;
    const uint8_t big_endian = osprey7_v2l8p2_INTERNAL_is_big_endian();

    EFUN(osprey7_v2l8p2_INTERNAL_get_rx_pam_mode(sa__, &pam_mode));
    st->rx_pam_mode = (uint8_t)pam_mode;
    ESTM(st->tx_pam_mode = rd_tx_pam4_mode());
    EFUN(osprey7_v2l8p2_prbs_chk_lock_state(sa__, &st->prbs_chk_lock));
    EFUN(osprey7_v2l8p2_prbs_err_count_state(sa__, &st->prbs_chk_errcnt, &st->prbs_chk_lock_lost));

    ESTM(st->dummy22    = rdv_usr_status_eq_debug17());
    EFUN(osprey7_v2l8p2_INTERNAL_get_link_time(sa__, &st->link_time));
    EFUN(osprey7_v2l8p2_INTERNAL_get_tuningdone_time(sa__, &st->tuningdone_time));
    if (st->prbs_chk_lock && (st->rx_pam_mode != NRZ)) {
        EFUN(osprey7_v2l8p2_dbgfb_collect_stats(sa__, 8000, &st->dbgfb_stats));
    }
    ESTM(st->dsc_sm[0] = rd_dsc_state_one_hot());
    ESTM(st->dsc_sm[1] = rd_dsc_state_one_hot()); 

    EFUN(osprey7_v2l8p2_INTERNAL_get_eye_margin_est(sa__, &st->heye_left, &st->heye_right, &st->veye_upper, &st->veye_lower));

    EFUN(osprey7_v2l8p2_INTERNAL_get_lms_thresh_bin(sa__, &lms_thresh));
    st->lms_thresh = lms_thresh;

    ESTM(d23 = rd_rx_data23_status());
    ESTM(d14 = rd_rx_data14_status());
    ESTM(d05 = rd_rx_data05_status());

    st->data23_thresh  = (int16_t)(d23 +   0);
    st->data14_thresh  = (int16_t)(d14 +  64);
    st->data05_thresh  = (int16_t)(d05 + 128);

    ESTM(st->main_tap_est = rdv_usr_main_tap_est());
    for (tap_num=0; tap_num<TXFIR_ST_NUM_TAPS; ++tap_num) {
        EFUN(osprey7_v2l8p2_INTERNAL_get_tx_tap(sa__, tap_num, &(st->txfir.tap[tap_num])));
    }


    ESTM(st->dummy20[0] = rdv_usr_status_pam4_fl_cdr_0());
    ESTM(st->dummy20[1] = rdv_usr_status_pam4_fl_cdr_1());
    ESTM(st->dummy21[0] = rdv_usr_status_pam4_fl_cdr_2());
    ESTM(st->dummy21[1] = rdv_usr_status_pam4_fl_cdr_3());

    ESTM(st->dummy24[0] = rdv_usr_status_worst_pam4_fl_cdr_0());
    ESTM(st->dummy24[1] = rdv_usr_status_worst_pam4_fl_cdr_1());
    ESTM(st->dummy25[0] = rdv_usr_status_worst_pam4_fl_cdr_2());
    ESTM(st->dummy25[1] = rdv_usr_status_worst_pam4_fl_cdr_3());

    ESTM(st->dummy8[0][0] = rdv_lvc_sts_0_0());
    ESTM(st->dummy8[0][1] = rdv_lvc_sts_0_1());
    ESTM(st->dummy8[1][0] = rdv_lvc_sts_1_0());
    ESTM(st->dummy8[1][1] = rdv_lvc_sts_1_1());
    ESTM(st->dummy8[2][0] = rdv_lvc_sts_2_0());
    ESTM(st->dummy8[2][1] = rdv_lvc_sts_2_1());
    ESTM(st->dummy8[3][0] = rdv_lvc_sts_3_0());
    ESTM(st->dummy8[3][1] = rdv_lvc_sts_3_1());
    ESTM(st->dummy8[4][0] = rdv_lvc_sts_4_0());
    ESTM(st->dummy8[4][1] = rdv_lvc_sts_4_1());
    ESTM(st->dummy8[5][0] = rdv_lvc_sts_5_0());
    ESTM(st->dummy8[5][1] = rdv_lvc_sts_5_1());
    ESTM(st->dummy8[0][2] = rdv_lvc_sts_0_2());
    ESTM(st->dummy8[0][3] = rdv_lvc_sts_0_3());
    ESTM(st->dummy8[1][2] = rdv_lvc_sts_1_2());
    ESTM(st->dummy8[1][3] = rdv_lvc_sts_1_3());
    ESTM(st->dummy8[2][2] = rdv_lvc_sts_2_2());
    ESTM(st->dummy8[2][3] = rdv_lvc_sts_2_3());

    ESTM(st->dummy8[3][2] = rdv_lvc_sts_3_2());
    ESTM(st->dummy8[3][3] = rdv_lvc_sts_3_3());
    ESTM(st->dummy8[4][2] = rdv_lvc_sts_4_2());
    ESTM(st->dummy8[4][3] = rdv_lvc_sts_4_3());
    ESTM(st->dummy8[5][2] = rdv_lvc_sts_5_2());
    ESTM(st->dummy8[5][3] = rdv_lvc_sts_5_3());

    EFUN(osprey7_v2l8p2_INTERNAL_get_pam_eye_margin_est(sa__, &eye_margin));
    ESTM(st->dc_offset_range_shift = rd_ams_rx_dc_offset_range_shift());

    for(i = 0; i < 3; i++) {
        st->pam_heye_left_mUI[i]   = eye_margin.left_eye_mUI [i];
        st->pam_heye_right_mUI[i]  = eye_margin.right_eye_mUI[i];
        st->pam_veye_upper_mV[i]   = eye_margin.upper_eye_mV [i];
        st->pam_veye_lower_mV[i]   = eye_margin.lower_eye_mV [i];
        ESTM(st->pam_heye_avg_mUI[i] = osprey7_v2l8p2_INTERNAL_eye_to_mUI(sa__, (uint8_t)((rdv_usr_sts_pam_heye_right(i) + rdv_usr_sts_pam_heye_left(i) + 1) >> 1)));
        ESTM(st->pam_veye_avg_mV[i] = osprey7_v2l8p2_INTERNAL_eye_to_mV(sa__, (uint8_t)((rdv_usr_sts_pam_veye_upper(i) + rdv_usr_sts_pam_veye_lower(i) + 1) >> 1), 0));
    }

    ESTM(st->prbs_chk_en = rd_prbs_chk_en());

    EFUN(osprey7_v2l8p2_get_rx_prbs_config(sa__, &prbs_poly_mode, &prbs_checker_mode, &st->prbs_chk_inv));
    st->prbs_chk_order = (uint8_t)prbs_poly_mode;

    ESTM(st->txfir_use_pam4_range = rd_txfir_nrz_tap_range_sel() ? 0 : 1);

    ESTM(tap_num = rd_txfir_tap_en());
    st->num_txfir_taps = (tap_num == 0) ? 3 : ((tap_num == 1) ? 6 : ((tap_num == 2) ? 9 : 12));

    EFUN(osprey7_v2l8p2_INTERNAL_sigdet_status(sa__, &st->sigdet, &st->sigdet_chg));

    ESTM(st->pmd_tx_pll_select = rd_tx_pll_select());
    ESTM(st->pmd_rx_pll_select = rd_rx_pll_select());

    EFUN(osprey7_v2l8p2_INTERNAL_get_rx_vga(sa__, &st->vga));

    EFUN(osprey7_v2l8p2_INTERNAL_get_rx_fga(sa__, &st->fga));
    EFUN(osprey7_v2l8p2_INTERNAL_get_rx_pf_main(sa__, &st->pf));
    EFUN(osprey7_v2l8p2_INTERNAL_get_rx_pf2(sa__, &st->pf2));
    EFUN(osprey7_v2l8p2_INTERNAL_get_rx_pf3(sa__, &st->pf3));

    ESTM(st->pf_reg[0]  = rd_rx_pfmid_zero_val());
    ESTM(st->pf_reg[1]  = rd_rx_pfmid_capbw_val());
    ESTM(st->pf_reg[2]  = rd_rx_pfmid_indbw_val());
    ESTM(st->pf3_reg[0] = rd_rx_pfhi_zero_val());
    ESTM(st->pf3_reg[1] = rd_rx_pfhi_capbw_val());
    ESTM(st->pf3_reg[2] = rd_rx_pfhi_indbw_val());

    EFUN(osprey7_v2l8p2_INTERNAL_get_ffe_enabled(sa__, &st->ffe_enable));

    ESTM(st->ffe_gain = osprey7_v2l8p2_INTERNAL_gray_to_uint8(rd_ams_rx_ffe_gain()));

    ESTM(st->ffe[0][0] = rd_rxa_ffe_tap1_val());
    ESTM(st->ffe[0][1] = rd_rxb_ffe_tap1_val());
    ESTM(st->ffe[0][2] = rd_rxc_ffe_tap1_val());
    ESTM(st->ffe[0][3] = rd_rxd_ffe_tap1_val());
    ESTM(st->ffe[1][0] = rd_rxa_ffe_tap2_val());
    ESTM(st->ffe[1][1] = rd_rxb_ffe_tap2_val());
    ESTM(st->ffe[1][2] = rd_rxc_ffe_tap2_val());
    ESTM(st->ffe[1][3] = rd_rxd_ffe_tap2_val());

    ESTM(st->clk90  = rdv_usr_sts_phase_hoffset());
    ESTM(st->dp_hoffset = rdv_dp_hoffset_0());

    ESTM(reg_data1 = reg_rd_DSC_E_RX_PI_CNT_BIN_LD());
    st->dl_hoffset = (int8_t)((reg_data1&0xFF) - ((reg_data1>>8)&0xFF));

    EFUN(osprey7_v2l8p2_get_dac4ck(sa__, &st->lms_dac4ck, &st->phase_dac4ck, &st->data_dac4ck));

    EFUN(osprey7_v2l8p2_get_dac4ck_q(sa__, &st->lms_dac4ck_q, &st->phase_dac4ck_q, &st->data_dac4ck_q));

    ESTM(st->dfe[1][0] = rd_rxa_dfe_tap2_status());
    ESTM(st->dfe[1][1] = rd_rxb_dfe_tap2_status());
    ESTM(st->dfe[2][0] = rd_rxa_dfe_tap3_status());
    ESTM(st->dfe[2][1] = rd_rxb_dfe_tap3_status());
    ESTM(st->dfe[1][2] = rd_rxc_dfe_tap2_status());
    ESTM(st->dfe[1][3] = rd_rxd_dfe_tap2_status());
    ESTM(st->dfe[2][2] = rd_rxc_dfe_tap3_status());
    ESTM(st->dfe[2][3] = rd_rxd_dfe_tap3_status());

    ESTM(st->dfe[3][0]  = rd_rxa_dfe_tap4_status());
    ESTM(st->dfe[3][1]  = rd_rxb_dfe_tap4_status());
    ESTM(st->dfe[4][0]  = rd_rxa_dfe_tap5_status());
    ESTM(st->dfe[4][1]  = rd_rxb_dfe_tap5_status());
    ESTM(st->dfe[5][0]  = rd_rxa_dfe_tap6_status());
    ESTM(st->dfe[5][1]  = rd_rxb_dfe_tap6_status());
    ESTM(st->dfe[6][0]  = rd_rxa_dfe_tap7_status());
    ESTM(st->dfe[6][1]  = rd_rxb_dfe_tap7_status());
    ESTM(st->dfe[7][0]  = rd_rxa_dfe_tap8_status());
    ESTM(st->dfe[7][1]  = rd_rxb_dfe_tap8_status());
    ESTM(st->dfe[8][0]  = rd_rxa_dfe_tap9());
    ESTM(st->dfe[8][1]  = rd_rxb_dfe_tap9());
    ESTM(st->dfe[9][0]  = rd_rxa_dfe_tap10());
    ESTM(st->dfe[9][1]  = rd_rxb_dfe_tap10());
    ESTM(st->dfe[10][0] = rd_rxa_dfe_tap11());
    ESTM(st->dfe[10][1] = rd_rxb_dfe_tap11());
    ESTM(st->dfe[11][0] = ((rd_rxa_dfe_tap12_mux()==0)?rd_rxa_dfe_tap12():0));
    ESTM(st->dfe[11][1] = ((rd_rxb_dfe_tap12_mux()==0)?rd_rxb_dfe_tap12():0));
    ESTM(st->dfe[12][0] = ((rd_rxa_dfe_tap13_mux()==0)?rd_rxa_dfe_tap13():0));
    ESTM(st->dfe[12][1] = ((rd_rxb_dfe_tap13_mux()==0)?rd_rxb_dfe_tap13():0));
    ESTM(st->dfe[13][0] = ((rd_rxa_dfe_tap14_mux()==0)?rd_rxa_dfe_tap14():0));
    ESTM(st->dfe[13][1] = ((rd_rxb_dfe_tap14_mux()==0)?rd_rxb_dfe_tap14():0));
    ESTM(st->dfe[14][0] = ((rd_rxa_dfe_tap15_mux()==0)?rd_rxa_dfe_tap15():0));
    ESTM(st->dfe[14][1] = ((rd_rxb_dfe_tap15_mux()==0)?rd_rxb_dfe_tap15():0));
    ESTM(st->dfe[15][0] = ((rd_rxa_dfe_tap12_mux()==1)?rd_rxa_dfe_tap12():(rd_rxa_dfe_tap16_mux()==0)?rd_rxa_dfe_tap16():0));
    ESTM(st->dfe[15][1] = ((rd_rxb_dfe_tap12_mux()==1)?rd_rxb_dfe_tap12():(rd_rxb_dfe_tap16_mux()==0)?rd_rxb_dfe_tap16():0));
    ESTM(st->dfe[16][0] = ((rd_rxa_dfe_tap13_mux()==1)?rd_rxa_dfe_tap13():(rd_rxa_dfe_tap17_mux()==0)?rd_rxa_dfe_tap17():0));
    ESTM(st->dfe[16][1] = ((rd_rxb_dfe_tap13_mux()==1)?rd_rxb_dfe_tap13():(rd_rxb_dfe_tap17_mux()==0)?rd_rxb_dfe_tap17():0));
    ESTM(st->dfe[17][0] = ((rd_rxa_dfe_tap14_mux()==1)?rd_rxa_dfe_tap14():(rd_rxa_dfe_tap18_mux()==0)?rd_rxa_dfe_tap18():0));
    ESTM(st->dfe[17][1] = ((rd_rxb_dfe_tap14_mux()==1)?rd_rxb_dfe_tap14():(rd_rxb_dfe_tap18_mux()==0)?rd_rxb_dfe_tap18():0));
    ESTM(st->dfe[18][0] = ((rd_rxa_dfe_tap15_mux()==1)?rd_rxa_dfe_tap15():0));
    ESTM(st->dfe[18][1] = ((rd_rxb_dfe_tap15_mux()==1)?rd_rxb_dfe_tap15():0));
    ESTM(st->dfe[19][0] = ((rd_rxa_dfe_tap12_mux()==2)?rd_rxa_dfe_tap12():(rd_rxa_dfe_tap16_mux()==1)?rd_rxa_dfe_tap16():0));
    ESTM(st->dfe[19][1] = ((rd_rxb_dfe_tap12_mux()==2)?rd_rxb_dfe_tap12():(rd_rxb_dfe_tap16_mux()==1)?rd_rxb_dfe_tap16():0));
    ESTM(st->dfe[20][0] = ((rd_rxa_dfe_tap13_mux()==2)?rd_rxa_dfe_tap13():(rd_rxa_dfe_tap17_mux()==1)?rd_rxa_dfe_tap17():0));
    ESTM(st->dfe[20][1] = ((rd_rxb_dfe_tap13_mux()==2)?rd_rxb_dfe_tap13():(rd_rxb_dfe_tap17_mux()==1)?rd_rxb_dfe_tap17():0));
    ESTM(st->dfe[21][0] = ((rd_rxa_dfe_tap14_mux()==2)?rd_rxa_dfe_tap14():(rd_rxa_dfe_tap18_mux()==1)?rd_rxa_dfe_tap18():0));
    ESTM(st->dfe[21][1] = ((rd_rxb_dfe_tap14_mux()==2)?rd_rxb_dfe_tap14():(rd_rxb_dfe_tap18_mux()==1)?rd_rxb_dfe_tap18():0));
    ESTM(st->dfe[22][0] = ((rd_rxa_dfe_tap15_mux()==2)?rd_rxa_dfe_tap15():0));
    ESTM(st->dfe[22][1] = ((rd_rxb_dfe_tap15_mux()==2)?rd_rxb_dfe_tap15():0));
    ESTM(st->dfe[23][0] = ((rd_rxa_dfe_tap12_mux()==3)?rd_rxa_dfe_tap12():(rd_rxa_dfe_tap16_mux()==2)?rd_rxa_dfe_tap16():0));
    ESTM(st->dfe[23][1] = ((rd_rxb_dfe_tap12_mux()==3)?rd_rxb_dfe_tap12():(rd_rxb_dfe_tap16_mux()==2)?rd_rxb_dfe_tap16():0));
    ESTM(st->dfe[24][0] = ((rd_rxa_dfe_tap13_mux()==3)?rd_rxa_dfe_tap13():(rd_rxa_dfe_tap17_mux()==2)?rd_rxa_dfe_tap17():0));
    ESTM(st->dfe[24][1] = ((rd_rxb_dfe_tap13_mux()==3)?rd_rxb_dfe_tap13():(rd_rxb_dfe_tap17_mux()==2)?rd_rxb_dfe_tap17():0));
    ESTM(st->dfe[25][0] = ((rd_rxa_dfe_tap14_mux()==3)?rd_rxa_dfe_tap14():(rd_rxa_dfe_tap18_mux()==2)?rd_rxa_dfe_tap18():0));
    ESTM(st->dfe[25][1] = ((rd_rxb_dfe_tap14_mux()==3)?rd_rxb_dfe_tap14():(rd_rxb_dfe_tap18_mux()==2)?rd_rxb_dfe_tap18():0));
    ESTM(st->dfe[26][0] = ((rd_rxa_dfe_tap15_mux()==3)?rd_rxa_dfe_tap15():0));
    ESTM(st->dfe[26][1] = ((rd_rxb_dfe_tap15_mux()==3)?rd_rxb_dfe_tap15():0));
    ESTM(st->dfe[27][0] = ((rd_rxa_dfe_tap12_mux()==4)?rd_rxa_dfe_tap12():(rd_rxa_dfe_tap16_mux()==3)?rd_rxa_dfe_tap16():0));
    ESTM(st->dfe[27][1] = ((rd_rxb_dfe_tap12_mux()==4)?rd_rxb_dfe_tap12():(rd_rxb_dfe_tap16_mux()==3)?rd_rxb_dfe_tap16():0));
    ESTM(st->dfe[28][0] = ((rd_rxa_dfe_tap13_mux()==4)?rd_rxa_dfe_tap13():(rd_rxa_dfe_tap17_mux()==3)?rd_rxa_dfe_tap17():0));
    ESTM(st->dfe[28][1] = ((rd_rxb_dfe_tap13_mux()==4)?rd_rxb_dfe_tap13():(rd_rxb_dfe_tap17_mux()==3)?rd_rxb_dfe_tap17():0));
    ESTM(st->dfe[29][0] = ((rd_rxa_dfe_tap14_mux()==4)?rd_rxa_dfe_tap14():(rd_rxa_dfe_tap18_mux()==3)?rd_rxa_dfe_tap18():0));
    ESTM(st->dfe[29][1] = ((rd_rxb_dfe_tap14_mux()==4)?rd_rxb_dfe_tap14():(rd_rxb_dfe_tap18_mux()==3)?rd_rxb_dfe_tap18():0));
    ESTM(st->dfe[30][0] = ((rd_rxa_dfe_tap15_mux()==4)?rd_rxa_dfe_tap15():0));
    ESTM(st->dfe[30][1] = ((rd_rxb_dfe_tap15_mux()==4)?rd_rxb_dfe_tap15():0));
    ESTM(st->dfe[31][0] = ((rd_rxa_dfe_tap12_mux()==5)?rd_rxa_dfe_tap12():(rd_rxa_dfe_tap16_mux()==4)?rd_rxa_dfe_tap16():0));
    ESTM(st->dfe[31][1] = ((rd_rxb_dfe_tap12_mux()==5)?rd_rxb_dfe_tap12():(rd_rxb_dfe_tap16_mux()==4)?rd_rxb_dfe_tap16():0));
    ESTM(st->dfe[32][0] = ((rd_rxa_dfe_tap13_mux()==5)?rd_rxa_dfe_tap13():(rd_rxa_dfe_tap17_mux()==4)?rd_rxa_dfe_tap17():0));
    ESTM(st->dfe[32][1] = ((rd_rxb_dfe_tap13_mux()==5)?rd_rxb_dfe_tap13():(rd_rxb_dfe_tap17_mux()==4)?rd_rxb_dfe_tap17():0));
    ESTM(st->dfe[33][0] = ((rd_rxa_dfe_tap14_mux()==5)?rd_rxa_dfe_tap14():(rd_rxa_dfe_tap18_mux()==4)?rd_rxa_dfe_tap18():0));
    ESTM(st->dfe[33][1] = ((rd_rxb_dfe_tap14_mux()==5)?rd_rxb_dfe_tap14():(rd_rxb_dfe_tap18_mux()==4)?rd_rxb_dfe_tap18():0));
    ESTM(st->dfe[34][0] = ((rd_rxa_dfe_tap15_mux()==5)?rd_rxa_dfe_tap15():0));
    ESTM(st->dfe[34][1] = ((rd_rxb_dfe_tap15_mux()==5)?rd_rxb_dfe_tap15():0));
    ESTM(st->dfe[35][0] = ((rd_rxa_dfe_tap16_mux()==5)?rd_rxa_dfe_tap16():0));
    ESTM(st->dfe[35][1] = ((rd_rxb_dfe_tap16_mux()==5)?rd_rxb_dfe_tap16():0));
    ESTM(st->dfe[36][0] = ((rd_rxa_dfe_tap17_mux()==5)?rd_rxa_dfe_tap17():0));
    ESTM(st->dfe[36][1] = ((rd_rxb_dfe_tap17_mux()==5)?rd_rxb_dfe_tap17():0));
    ESTM(st->dfe[37][0] = ((rd_rxa_dfe_tap18_mux()==5)?rd_rxa_dfe_tap18():0));
    ESTM(st->dfe[37][1] = ((rd_rxb_dfe_tap18_mux()==5)?rd_rxb_dfe_tap18():0));

    ESTM(st->dfe[3][2]  = rd_rxc_dfe_tap4_status());
    ESTM(st->dfe[3][3]  = rd_rxd_dfe_tap4_status());
    ESTM(st->dfe[4][2]  = rd_rxc_dfe_tap5_status());
    ESTM(st->dfe[4][3]  = rd_rxd_dfe_tap5_status());
    ESTM(st->dfe[5][2]  = rd_rxc_dfe_tap6_status());
    ESTM(st->dfe[5][3]  = rd_rxd_dfe_tap6_status());
    ESTM(st->dfe[6][2]  = rd_rxc_dfe_tap7_status());
    ESTM(st->dfe[6][3]  = rd_rxd_dfe_tap7_status());
    ESTM(st->dfe[7][2]  = rd_rxc_dfe_tap8_status());
    ESTM(st->dfe[7][3]  = rd_rxd_dfe_tap8_status());
    ESTM(st->dfe[8][2]  = rd_rxc_dfe_tap9());
    ESTM(st->dfe[8][3]  = rd_rxd_dfe_tap9());
    ESTM(st->dfe[9][2]  = rd_rxc_dfe_tap10());
    ESTM(st->dfe[9][3]  = rd_rxd_dfe_tap10());
    ESTM(st->dfe[10][2] = rd_rxc_dfe_tap11());
    ESTM(st->dfe[10][3] = rd_rxd_dfe_tap11());

    ESTM(st->dfe[11][2] = ((rd_rxc_dfe_tap12_mux()==0)?rd_rxc_dfe_tap12():0));
    ESTM(st->dfe[11][3] = ((rd_rxd_dfe_tap12_mux()==0)?rd_rxd_dfe_tap12():0));
    ESTM(st->dfe[12][2] = ((rd_rxc_dfe_tap13_mux()==0)?rd_rxc_dfe_tap13():0));
    ESTM(st->dfe[12][3] = ((rd_rxd_dfe_tap13_mux()==0)?rd_rxd_dfe_tap13():0));
    ESTM(st->dfe[13][2] = ((rd_rxc_dfe_tap14_mux()==0)?rd_rxc_dfe_tap14():0));
    ESTM(st->dfe[13][3] = ((rd_rxd_dfe_tap14_mux()==0)?rd_rxd_dfe_tap14():0));
    ESTM(st->dfe[14][2] = ((rd_rxc_dfe_tap15_mux()==0)?rd_rxc_dfe_tap15():0));
    ESTM(st->dfe[14][3] = ((rd_rxd_dfe_tap15_mux()==0)?rd_rxd_dfe_tap15():0));
    ESTM(st->dfe[15][2] = ((rd_rxc_dfe_tap12_mux()==1)?rd_rxc_dfe_tap12():(rd_rxc_dfe_tap16_mux()==0)?rd_rxc_dfe_tap16():0));
    ESTM(st->dfe[15][3] = ((rd_rxd_dfe_tap12_mux()==1)?rd_rxd_dfe_tap12():(rd_rxd_dfe_tap16_mux()==0)?rd_rxd_dfe_tap16():0));
    ESTM(st->dfe[16][2] = ((rd_rxc_dfe_tap13_mux()==1)?rd_rxc_dfe_tap13():(rd_rxc_dfe_tap17_mux()==0)?rd_rxc_dfe_tap17():0));
    ESTM(st->dfe[16][3] = ((rd_rxd_dfe_tap13_mux()==1)?rd_rxd_dfe_tap13():(rd_rxd_dfe_tap17_mux()==0)?rd_rxd_dfe_tap17():0));
    ESTM(st->dfe[17][2] = ((rd_rxc_dfe_tap14_mux()==1)?rd_rxc_dfe_tap14():(rd_rxc_dfe_tap18_mux()==0)?rd_rxc_dfe_tap18():0));
    ESTM(st->dfe[17][3] = ((rd_rxd_dfe_tap14_mux()==1)?rd_rxd_dfe_tap14():(rd_rxd_dfe_tap18_mux()==0)?rd_rxd_dfe_tap18():0));
    ESTM(st->dfe[18][2] = ((rd_rxc_dfe_tap15_mux()==1)?rd_rxc_dfe_tap15():0));
    ESTM(st->dfe[18][3] = ((rd_rxd_dfe_tap15_mux()==1)?rd_rxd_dfe_tap15():0));
    ESTM(st->dfe[19][2] = ((rd_rxc_dfe_tap12_mux()==2)?rd_rxc_dfe_tap12():(rd_rxc_dfe_tap16_mux()==1)?rd_rxc_dfe_tap16():0));
    ESTM(st->dfe[19][3] = ((rd_rxd_dfe_tap12_mux()==2)?rd_rxd_dfe_tap12():(rd_rxd_dfe_tap16_mux()==1)?rd_rxd_dfe_tap16():0));
    ESTM(st->dfe[20][2] = ((rd_rxc_dfe_tap13_mux()==2)?rd_rxc_dfe_tap13():(rd_rxc_dfe_tap17_mux()==1)?rd_rxc_dfe_tap17():0));
    ESTM(st->dfe[20][3] = ((rd_rxd_dfe_tap13_mux()==2)?rd_rxd_dfe_tap13():(rd_rxd_dfe_tap17_mux()==1)?rd_rxd_dfe_tap17():0));
    ESTM(st->dfe[21][2] = ((rd_rxc_dfe_tap14_mux()==2)?rd_rxc_dfe_tap14():(rd_rxc_dfe_tap18_mux()==1)?rd_rxc_dfe_tap18():0));
    ESTM(st->dfe[21][3] = ((rd_rxd_dfe_tap14_mux()==2)?rd_rxd_dfe_tap14():(rd_rxd_dfe_tap18_mux()==1)?rd_rxd_dfe_tap18():0));
    ESTM(st->dfe[22][2] = ((rd_rxc_dfe_tap15_mux()==2)?rd_rxc_dfe_tap15():0));
    ESTM(st->dfe[22][3] = ((rd_rxd_dfe_tap15_mux()==2)?rd_rxd_dfe_tap15():0));
    ESTM(st->dfe[23][2] = ((rd_rxc_dfe_tap12_mux()==3)?rd_rxc_dfe_tap12():(rd_rxc_dfe_tap16_mux()==2)?rd_rxc_dfe_tap16():0));
    ESTM(st->dfe[23][3] = ((rd_rxd_dfe_tap12_mux()==3)?rd_rxd_dfe_tap12():(rd_rxd_dfe_tap16_mux()==2)?rd_rxd_dfe_tap16():0));
    ESTM(st->dfe[24][2] = ((rd_rxc_dfe_tap13_mux()==3)?rd_rxc_dfe_tap13():(rd_rxc_dfe_tap17_mux()==2)?rd_rxc_dfe_tap17():0));
    ESTM(st->dfe[24][3] = ((rd_rxd_dfe_tap13_mux()==3)?rd_rxd_dfe_tap13():(rd_rxd_dfe_tap17_mux()==2)?rd_rxd_dfe_tap17():0));
    ESTM(st->dfe[25][2] = ((rd_rxc_dfe_tap14_mux()==3)?rd_rxc_dfe_tap14():(rd_rxc_dfe_tap18_mux()==2)?rd_rxc_dfe_tap18():0));
    ESTM(st->dfe[25][3] = ((rd_rxd_dfe_tap14_mux()==3)?rd_rxd_dfe_tap14():(rd_rxd_dfe_tap18_mux()==2)?rd_rxd_dfe_tap18():0));
    ESTM(st->dfe[26][2] = ((rd_rxc_dfe_tap15_mux()==3)?rd_rxc_dfe_tap15():0));
    ESTM(st->dfe[26][3] = ((rd_rxd_dfe_tap15_mux()==3)?rd_rxd_dfe_tap15():0));
    ESTM(st->dfe[27][2] = ((rd_rxc_dfe_tap12_mux()==4)?rd_rxc_dfe_tap12():(rd_rxc_dfe_tap16_mux()==3)?rd_rxc_dfe_tap16():0));
    ESTM(st->dfe[27][3] = ((rd_rxd_dfe_tap12_mux()==4)?rd_rxd_dfe_tap12():(rd_rxd_dfe_tap16_mux()==3)?rd_rxd_dfe_tap16():0));
    ESTM(st->dfe[28][2] = ((rd_rxc_dfe_tap13_mux()==4)?rd_rxc_dfe_tap13():(rd_rxc_dfe_tap17_mux()==3)?rd_rxc_dfe_tap17():0));
    ESTM(st->dfe[28][3] = ((rd_rxd_dfe_tap13_mux()==4)?rd_rxd_dfe_tap13():(rd_rxd_dfe_tap17_mux()==3)?rd_rxd_dfe_tap17():0));
    ESTM(st->dfe[29][2] = ((rd_rxc_dfe_tap14_mux()==4)?rd_rxc_dfe_tap14():(rd_rxc_dfe_tap18_mux()==3)?rd_rxc_dfe_tap18():0));
    ESTM(st->dfe[29][3] = ((rd_rxd_dfe_tap14_mux()==4)?rd_rxd_dfe_tap14():(rd_rxd_dfe_tap18_mux()==3)?rd_rxd_dfe_tap18():0));
    ESTM(st->dfe[30][2] = ((rd_rxc_dfe_tap15_mux()==4)?rd_rxc_dfe_tap15():0));
    ESTM(st->dfe[30][3] = ((rd_rxd_dfe_tap15_mux()==4)?rd_rxd_dfe_tap15():0));
    ESTM(st->dfe[31][2] = ((rd_rxc_dfe_tap12_mux()==5)?rd_rxc_dfe_tap12():(rd_rxc_dfe_tap16_mux()==4)?rd_rxc_dfe_tap16():0));
    ESTM(st->dfe[31][3] = ((rd_rxd_dfe_tap12_mux()==5)?rd_rxd_dfe_tap12():(rd_rxd_dfe_tap16_mux()==4)?rd_rxd_dfe_tap16():0));
    ESTM(st->dfe[32][2] = ((rd_rxc_dfe_tap13_mux()==5)?rd_rxc_dfe_tap13():(rd_rxc_dfe_tap17_mux()==4)?rd_rxc_dfe_tap17():0));
    ESTM(st->dfe[32][3] = ((rd_rxd_dfe_tap13_mux()==5)?rd_rxd_dfe_tap13():(rd_rxd_dfe_tap17_mux()==4)?rd_rxd_dfe_tap17():0));
    ESTM(st->dfe[33][2] = ((rd_rxc_dfe_tap14_mux()==5)?rd_rxc_dfe_tap14():(rd_rxc_dfe_tap18_mux()==4)?rd_rxc_dfe_tap18():0));
    ESTM(st->dfe[33][3] = ((rd_rxd_dfe_tap14_mux()==5)?rd_rxd_dfe_tap14():(rd_rxd_dfe_tap18_mux()==4)?rd_rxd_dfe_tap18():0));
    ESTM(st->dfe[34][2] = ((rd_rxc_dfe_tap15_mux()==5)?rd_rxc_dfe_tap15():0));
    ESTM(st->dfe[34][3] = ((rd_rxd_dfe_tap15_mux()==5)?rd_rxd_dfe_tap15():0));
    ESTM(st->dfe[35][2] = ((rd_rxc_dfe_tap16_mux()==5)?rd_rxc_dfe_tap16():0));
    ESTM(st->dfe[35][3] = ((rd_rxd_dfe_tap16_mux()==5)?rd_rxd_dfe_tap16():0));
    ESTM(st->dfe[36][2] = ((rd_rxc_dfe_tap17_mux()==5)?rd_rxc_dfe_tap17():0));
    ESTM(st->dfe[36][3] = ((rd_rxd_dfe_tap17_mux()==5)?rd_rxd_dfe_tap17():0));
    ESTM(st->dfe[37][2] = ((rd_rxc_dfe_tap18_mux()==5)?rd_rxc_dfe_tap18():0));
    ESTM(st->dfe[37][3] = ((rd_rxd_dfe_tap18_mux()==5)?rd_rxd_dfe_tap18():0));

    ESTM(st->thctrl_d[0][0] = rd_rxa_slicer_offset_adj_cal_d0());
    ESTM(st->thctrl_d[0][1] = rd_rxb_slicer_offset_adj_cal_d0());

    ESTM(st->thctrl_d[1][0] = rd_rxa_slicer_offset_adj_cal_d1());
    ESTM(st->thctrl_d[1][1] = rd_rxb_slicer_offset_adj_cal_d1());

    ESTM(st->thctrl_d[2][0] = rd_rxa_slicer_offset_adj_cal_d2());
    ESTM(st->thctrl_d[2][1] = rd_rxb_slicer_offset_adj_cal_d2());

    ESTM(st->thctrl_d[3][0] = rd_rxa_slicer_offset_adj_cal_d3());
    ESTM(st->thctrl_d[3][1] = rd_rxb_slicer_offset_adj_cal_d3());

    ESTM(st->thctrl_d[4][0] = rd_rxa_slicer_offset_adj_cal_d4());
    ESTM(st->thctrl_d[4][1] = rd_rxb_slicer_offset_adj_cal_d4());

    ESTM(st->thctrl_d[5][0] = rd_rxa_slicer_offset_adj_cal_d5());
    ESTM(st->thctrl_d[5][1] = rd_rxb_slicer_offset_adj_cal_d5());

    ESTM(st->thctrl_d[0][2] = rd_rxc_slicer_offset_adj_cal_d0());
    ESTM(st->thctrl_d[0][3] = rd_rxd_slicer_offset_adj_cal_d0());
    ESTM(st->thctrl_d[1][2] = rd_rxc_slicer_offset_adj_cal_d1());
    ESTM(st->thctrl_d[1][3] = rd_rxd_slicer_offset_adj_cal_d1());
    ESTM(st->thctrl_d[2][2] = rd_rxc_slicer_offset_adj_cal_d2());
    ESTM(st->thctrl_d[2][3] = rd_rxd_slicer_offset_adj_cal_d2());
    ESTM(st->thctrl_d[3][2] = rd_rxc_slicer_offset_adj_cal_d3());
    ESTM(st->thctrl_d[3][3] = rd_rxd_slicer_offset_adj_cal_d3());
    ESTM(st->thctrl_d[4][2] = rd_rxc_slicer_offset_adj_cal_d4());
    ESTM(st->thctrl_d[4][3] = rd_rxd_slicer_offset_adj_cal_d4());
    ESTM(st->thctrl_d[5][2] = rd_rxc_slicer_offset_adj_cal_d5());
    ESTM(st->thctrl_d[5][3] = rd_rxd_slicer_offset_adj_cal_d5());

    ESTM(st->thctrl_p[0][0] = rd_rxa_slicer_offset_adj_cal_p0());
    ESTM(st->thctrl_p[0][1] = rd_rxb_slicer_offset_adj_cal_p0());

    ESTM(st->thctrl_p[1][0] = rd_rxa_slicer_offset_adj_cal_p1());
    ESTM(st->thctrl_p[1][1] = rd_rxb_slicer_offset_adj_cal_p1());


    ESTM(st->thctrl_p[2][0] = rd_rxa_slicer_offset_adj_cal_p2());
    ESTM(st->thctrl_p[2][1] = rd_rxb_slicer_offset_adj_cal_p2());
    ESTM(st->thctrl_p[0][2] = rd_rxc_slicer_offset_adj_cal_p0());
    ESTM(st->thctrl_p[0][3] = rd_rxd_slicer_offset_adj_cal_p0());
    ESTM(st->thctrl_p[1][2] = rd_rxc_slicer_offset_adj_cal_p1());
    ESTM(st->thctrl_p[1][3] = rd_rxd_slicer_offset_adj_cal_p1());

    ESTM(st->thctrl_p[2][2] = rd_rxc_slicer_offset_adj_cal_p2());
    ESTM(st->thctrl_p[2][3] = rd_rxd_slicer_offset_adj_cal_p2());

    ESTM(st->thctrl_l[0]  = rd_rxa_slicer_offset_adj_cal_lms());
    ESTM(st->thctrl_l[1]  = rd_rxb_slicer_offset_adj_cal_lms());

    ESTM(st->thctrl_l[2]  = rd_rxc_slicer_offset_adj_cal_lms());
    ESTM(st->thctrl_l[3]  = rd_rxd_slicer_offset_adj_cal_lms());

    EFUN(osprey7_v2l8p2_INTERNAL_get_BER_string(sa__,100,&st->ber_string[0], sizeof(st->ber_string)));

    st->ams_rx_vga1_bwr = 0;
    st->ams_rx_vga_dis_ind = 0;
    st->ams_rx_eq2_hibw = 0;
    st->ams_rx_vga0_rescal_mux=0;
    st->ams_rx_vga1_rescal_mux=0;
    st->ams_rx_vga2_rescal_mux=0;
    st->VGA1_bwr = 0;
    st->VGA2_bwr = 0;
    st->EQ1_bwr = 0;
    st->EQ2_bwr = 0;
    st->vga_dis_ind = 0;
    st->data_cal_err=0; st->data_cal_err_q=0;
    st->max_data_cal_err=0; st->max_data_cal_err_q=0;
    {
        int8_t lms_hcal_residual_error[4], phs_hcal_residual_error[4];
        EFUN(osprey7_v2l8p2_INTERNAL_read_hcal_residual_errors(sa__, lms_hcal_residual_error, phs_hcal_residual_error));
        st->lms_cal_err[0]=lms_hcal_residual_error[0];
        st->lms_cal_err[1]=lms_hcal_residual_error[2];
        st->lms_cal_err_q[0]=lms_hcal_residual_error[1];
        st->lms_cal_err_q[1]=lms_hcal_residual_error[3];
        st->phase_cal_err[0]=phs_hcal_residual_error[0];
        st->phase_cal_err[1]=phs_hcal_residual_error[2];
        st->phase_cal_err_q[0]=phs_hcal_residual_error[1];
        st->phase_cal_err_q[1]=phs_hcal_residual_error[3];
    }
    for(x= 0; x < 4; x++)  {
        for(y = 0; y < 2; y++) {
            st->res_dac4ck_err[x][y]=0;
            st->sgn_chn_cnt[x][y]=0;
        }
    }
    st->dac4ck_cal_fail=0;
    st->eye_margin_fail=0;
    st->dac4ck_monotonicity_fail=0;
    st-> hoffset_rail_out=0;
    st->reset_calibrated_dac4ck=0;
    ESTM(st->iq_cal = rdv_usr_status_iq_cal());
    EFUN(osprey7_v2l8p2_INTERNAL_read_iq_cal_inls(sa__,st->iq_cal_inl_errors, &st->iq_cal_residual_inl_error));

    ESTM(st->dummy7[0][0] = rdv_lhc_sts_0_0());
    ESTM(st->dummy7[0][1] = rdv_lhc_sts_0_1());
    ESTM(st->dummy7[1][0] = rdv_lhc_sts_1_0());
    ESTM(st->dummy7[1][1] = rdv_lhc_sts_1_1());
    ESTM(st->dummy7[2][0] = rdv_lhc_sts_2_0());
    ESTM(st->dummy7[2][1] = rdv_lhc_sts_2_1());

    ESTM(st->dummy7[3][0] = rdv_lhc_sts_3_0());
    ESTM(st->dummy7[3][1] = rdv_lhc_sts_3_1());
    ESTM(st->dummy7[4][0] = rdv_lhc_sts_4_0());
    ESTM(st->dummy7[4][1] = rdv_lhc_sts_4_1());
    ESTM(st->dummy7[5][0] = rdv_lhc_sts_5_0());
    ESTM(st->dummy7[5][1] = rdv_lhc_sts_5_1());

    ESTM(st->dummy7[0][2] = rdv_lhc_sts_0_2());
    ESTM(st->dummy7[0][3] = rdv_lhc_sts_0_3());
    ESTM(st->dummy7[1][2] = rdv_lhc_sts_1_2());
    ESTM(st->dummy7[1][3] = rdv_lhc_sts_1_3());
    ESTM(st->dummy7[2][2] = rdv_lhc_sts_2_2());
    ESTM(st->dummy7[2][3] = rdv_lhc_sts_2_3());

    ESTM(st->dummy7[3][2] = rdv_lhc_sts_3_2());
    ESTM(st->dummy7[3][3] = rdv_lhc_sts_3_3());
    ESTM(st->dummy7[4][2] = rdv_lhc_sts_4_2());
    ESTM(st->dummy7[4][3] = rdv_lhc_sts_4_3());
    ESTM(st->dummy7[5][2] = rdv_lhc_sts_5_2());
    ESTM(st->dummy7[5][3] = rdv_lhc_sts_5_3());

    ESTM(st->dummy9[0] = rdv_psv_ctl_byte());
    ESTM(st->dummy9[1] = rdv_psv_sts_byte());
    ESTM(st->dummy9[2] = rdv_psv_slic_sts_0_byte());
    ESTM(st->dummy9[3] = rdv_psv_slic_sts_1_byte());
    ESTM(st->dummy9[4] = rdv_psv_slic_sts_2_byte());

    ESTM(st->dummy9[5] = rdv_psv_slic_sts_3_byte());
    ESTM(st->dummy9[6] = rdv_psv_slic_sts_4_byte());
    ESTM(st->dummy9[7] = rdv_psv_slic_sts_5_byte());
    ESTM(st->dummy9[8] = rdv_lth_ctl_0_byte());
    ESTM(st->dummy9[9] = rdv_lth_ctl_1_byte());
    ESTM(st->dummy9[10] = rdv_lth_ctl_2_byte());
    ESTM(st->dummy9[11] = rdv_lth_ctl_3_byte());

    ESTM(st->dummy10[0][0] = rdv_lc_sts_0_0_byte());
    ESTM(st->dummy10[0][1] = rdv_lc_sts_0_1_byte());
    ESTM(st->dummy10[1][0] = rdv_lc_sts_1_0_byte());
    ESTM(st->dummy10[1][1] = rdv_lc_sts_1_1_byte());
    ESTM(st->dummy10[2][0] = rdv_lc_sts_2_0_byte());
    ESTM(st->dummy10[2][1] = rdv_lc_sts_2_1_byte());

    ESTM(st->dummy10[3][0] = rdv_lc_sts_3_0_byte());
    ESTM(st->dummy10[3][1] = rdv_lc_sts_3_1_byte());
    ESTM(st->dummy10[4][0] = rdv_lc_sts_4_0_byte());
    ESTM(st->dummy10[4][1] = rdv_lc_sts_4_1_byte());
    ESTM(st->dummy10[5][0] = rdv_lc_sts_5_0_byte());
    ESTM(st->dummy10[5][1] = rdv_lc_sts_5_1_byte());

    ESTM(st->dummy11[0][0] = rdv_lhr_sts_0_0());
    ESTM(st->dummy11[0][1] = rdv_lhr_sts_0_1());
    ESTM(st->dummy11[1][0] = rdv_lhr_sts_1_0());
    ESTM(st->dummy11[1][1] = rdv_lhr_sts_1_1());
    ESTM(st->dummy11[2][0] = rdv_lhr_sts_2_0());
    ESTM(st->dummy11[2][1] = rdv_lhr_sts_2_1());

    ESTM(st->dummy11[3][0] = rdv_lhr_sts_3_0());
    ESTM(st->dummy11[3][1] = rdv_lhr_sts_3_1());
    ESTM(st->dummy11[4][0] = rdv_lhr_sts_4_0());
    ESTM(st->dummy11[4][1] = rdv_lhr_sts_4_1());
    ESTM(st->dummy11[5][0] = rdv_lhr_sts_5_0());
    ESTM(st->dummy11[5][1] = rdv_lhr_sts_5_1());


    ESTM(st->dummy12[0][0] = rdv_lvr_sts_0_0());
    ESTM(st->dummy12[0][1] = rdv_lvr_sts_0_1());
    ESTM(st->dummy12[1][0] = rdv_lvr_sts_1_0());
    ESTM(st->dummy12[1][1] = rdv_lvr_sts_1_1());
    ESTM(st->dummy12[2][0] = rdv_lvr_sts_2_0());
    ESTM(st->dummy12[2][1] = rdv_lvr_sts_2_1());

    ESTM(st->dummy12[3][0] = rdv_lvr_sts_3_0());
    ESTM(st->dummy12[3][1] = rdv_lvr_sts_3_1());
    ESTM(st->dummy12[4][0] = rdv_lvr_sts_4_0());
    ESTM(st->dummy12[4][1] = rdv_lvr_sts_4_1());
    ESTM(st->dummy12[5][0] = rdv_lvr_sts_5_0());
    ESTM(st->dummy12[5][1] = rdv_lvr_sts_5_1());

    ESTM(st->dummy10[0][2] = rdv_lc_sts_0_2_byte());
    ESTM(st->dummy10[0][3] = rdv_lc_sts_0_3_byte());
    ESTM(st->dummy10[1][2] = rdv_lc_sts_1_2_byte());
    ESTM(st->dummy10[1][3] = rdv_lc_sts_1_3_byte());
    ESTM(st->dummy10[2][2] = rdv_lc_sts_2_2_byte());
    ESTM(st->dummy10[2][3] = rdv_lc_sts_2_3_byte());

    ESTM(st->dummy10[3][2] = rdv_lc_sts_3_2_byte());
    ESTM(st->dummy10[3][3] = rdv_lc_sts_3_3_byte());
    ESTM(st->dummy10[4][2] = rdv_lc_sts_4_2_byte());
    ESTM(st->dummy10[4][3] = rdv_lc_sts_4_3_byte());
    ESTM(st->dummy10[5][2] = rdv_lc_sts_5_2_byte());
    ESTM(st->dummy10[5][3] = rdv_lc_sts_5_3_byte());

    ESTM(st->dummy11[0][2] = rdv_lhr_sts_0_2());
    ESTM(st->dummy11[0][3] = rdv_lhr_sts_0_3());
    ESTM(st->dummy11[1][2] = rdv_lhr_sts_1_2());
    ESTM(st->dummy11[1][3] = rdv_lhr_sts_1_3());
    ESTM(st->dummy11[2][2] = rdv_lhr_sts_2_2());
    ESTM(st->dummy11[2][3] = rdv_lhr_sts_2_3());

    ESTM(st->dummy11[3][2] = rdv_lhr_sts_3_2());
    ESTM(st->dummy11[3][3] = rdv_lhr_sts_3_3());
    ESTM(st->dummy11[4][2] = rdv_lhr_sts_4_2());
    ESTM(st->dummy11[4][3] = rdv_lhr_sts_4_3());
    ESTM(st->dummy11[5][2] = rdv_lhr_sts_5_2());
    ESTM(st->dummy11[5][3] = rdv_lhr_sts_5_3());


    ESTM(st->dummy12[0][2] = rdv_lvr_sts_0_2());
    ESTM(st->dummy12[0][3] = rdv_lvr_sts_0_3());
    ESTM(st->dummy12[1][2] = rdv_lvr_sts_1_2());
    ESTM(st->dummy12[1][3] = rdv_lvr_sts_1_3());
    ESTM(st->dummy12[2][2] = rdv_lvr_sts_2_2());
    ESTM(st->dummy12[2][3] = rdv_lvr_sts_2_3());

    ESTM(st->dummy12[3][2] = rdv_lvr_sts_3_2());
    ESTM(st->dummy12[3][3] = rdv_lvr_sts_3_3());
    ESTM(st->dummy12[4][2] = rdv_lvr_sts_4_2());
    ESTM(st->dummy12[4][3] = rdv_lvr_sts_4_3());
    ESTM(st->dummy12[5][2] = rdv_lvr_sts_5_2());
    ESTM(st->dummy12[5][3] = rdv_lvr_sts_5_3());

    ESTM(st->dummy18[0] = rdv_usr_status_tp_metric_1());
    ESTM(st->dummy18[1] = rdv_usr_status_tp_metric_2());
    ESTM(st->dummy18[2] = rdv_usr_status_tp_metric_3());

    st->big_endian = big_endian;

    if (st->pmd_lock == 1) {
        if (!st->stop_state) {
            EFUN(osprey7_v2l8p2_stop_rx_adaptation(sa__, 0));
        }
    } else {
        EFUN(osprey7_v2l8p2_stop_rx_adaptation(sa__, 0));
    }
    return ERR_CODE_NONE;
}

err_code_t osprey7_v2l8p2_collect_custom_fields_osprey7_v2l8p2_lane_state_st_pre(srds_access_t *sa__, osprey7_v2l8p2_lane_state_st_t *st) {
    INIT_SRDS_ERR_CODE

    EFUN(osprey7_v2l8p2_INTERNAL_pmd_lock_status(sa__,&st->pmd_lock, &st->pmd_lock_chg));
    EFUN(osprey7_v2l8p2_INTERNAL_get_stop_state_lane_state(sa__, st)); /* Dependency on st->pmd_lock. Updates st->stop_state AND stop micro as well */
    return ERR_CODE_NONE;
}

err_code_t osprey7_v2l8p2_collect_custom_fields_osprey7_v2l8p2_lane_state_st_post(srds_access_t *sa__, osprey7_v2l8p2_lane_state_st_t *st) {
    INIT_SRDS_ERR_CODE

    st->laneid = osprey7_v2l8p2_acc_get_lane(sa__);
    EFUN(osprey7_v2l8p2_get_rx_tuning_status(sa__, &st->rx_tuning_done));

    EFUN(osprey7_v2l8p2_INTERNAL_get_osr_mode(sa__, &st->osr_mode));
    EFUN(osprey7_v2l8p2_INTERNAL_get_osr_str_lane_state(sa__, st)); /* Dependency on st->osr_mode */

    EFUN(osprey7_v2l8p2_INTERNAL_get_rx_pam_mode_lane_state(sa__, st));
    ESTM(st->tx_pam_mode = rd_tx_pam4_mode());
    EFUN(osprey7_v2l8p2_INTERNAL_get_uc_cfg_lane_state(sa__, st));

    ESTM(st->UC_STS = rdv_status_byte());
    ESTM(st->UC_STS_EXT = rdv_ext_status_word());
    ESTM(st->tx_pll_select = rd_tx_pll_select());
    ESTM(st->rx_pll_select = rd_rx_pll_select());
    EFUN(osprey7_v2l8p2_INTERNAL_sigdet_status(sa__,&st->sig_det, &st->sig_det_chg));
    ESTM(st->ppm = rd_cdr_integ_reg()/84);
    EFUN(osprey7_v2l8p2_INTERNAL_decode_br_os_mode(sa__, &st->CDR));

    EFUN(osprey7_v2l8p2_INTERNAL_get_pf_lane_state(sa__, st));
    EFUN(osprey7_v2l8p2_read_rx_afe(sa__, RX_AFE_VGA, &st->VGA));
    EFUN(osprey7_v2l8p2_read_rx_afe(sa__, RX_AFE_FGA, &st->FGA));

    ESTM(st->P1mV = rdv_usr_main_tap_est()/32);
    EFUN(osprey7_v2l8p2_INTERNAL_ladder_setting_to_mV(sa__, (int16_t)st->P1mV, 0, LMS_SLICER, &st->P1mV));

    EFUN(osprey7_v2l8p2_INTERNAL_get_TP0_lane_state(sa__, st)); /* Dependency on st->rx_pam_mode */

    EFUN(osprey7_v2l8p2_INTERNAL_get_rx_ffe_lane_state(sa__, st));

    EFUN(osprey7_v2l8p2_INTERNAL_get_afe_bw_metric_lane_state(sa__, st)); /* Dependency on st->TP1 and st->blind_ctle_trnsum */

    EFUN(osprey7_v2l8p2_INTERNAL_get_dfe_lane_state(sa__, st)); /* Dependency on st->rx_pam_mode */
    EFUN(osprey7_v2l8p2_INTERNAL_get_tx_ppm(sa__, &st->tx_ppm));
    EFUN(osprey7_v2l8p2_INTERNAL_get_txfir_lane_state(sa__, st));
    EFUN(osprey7_v2l8p2_INTERNAL_get_eye_margin_est(sa__, &st->heye_left, &st->heye_right, &st->veye_upper, &st->veye_lower));

    EFUN(osprey7_v2l8p2_INTERNAL_get_disable_eye_lane_state(sa__, st));

    EFUN(osprey7_v2l8p2_INTERNAL_get_link_time(sa__, &st->link_time));
    EFUN(osprey7_v2l8p2_INTERNAL_get_pmd_lock_chg_lane_state(sa__, st));  /* Depedency on st->pmd_lock */

    EFUN(osprey7_v2l8p2_INTERNAL_resume_micro_lane_state(sa__, st)); /* Dependency on st->stop_state */

    EFUN(osprey7_v2l8p2_INTERNAL_get_BER_string(sa__, 100, st->BER, sizeof(st->BER)));
    EFUN(osprey7_v2l8p2_INTERNAL_get_uc_sts_decoded_lane_state(sa__, st)); /* Dependency on st->UC_STS and st->UC_STS_EXT */

    return ERR_CODE_NONE;
}

#endif /* !SMALL_FOOTPRINT */

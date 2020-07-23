/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       dscdump.c
 */

#include "types.h"
#include "error.h"
#include "bcm_utils.h"
#include "dscdump.h"
#include "tsc_functions.h"
#include "bcm_xmod_pe_api.h"

/* Formula for PVTMON: T = 410.04-0.48705*reg10bit (from PVTMON Analog Module Specification v5.0 section 6.2) */
#define _bin_to_degC(bin) ((int16)(410 + ((2212 - 1995 * (int32)bin) >> 12)))
/* returns 000111 (7 = 8-1), for n = 3  */
#define BFMASK(n) ((1 << ((n))) - 1)
/* Clockwise difference between phase counters */
#define dist_cw(a, b) (((a) <= (b)) ? ((b) - (a)) : ((uint16)256 - (a) + (b)))
/* Counter-clockwise difference between phase counters */
#define dist_ccw(a, b) (((a) >= (b)) ? ((a) - (b)) : ((uint16)256 + (a) - (b)))

/** Display a single member of a lane struct. */
#define DISP_LN_VARS(name, param, format)              \
    do {                               \
        BCM_LOG_CLI("%-16s\t", name);              \
        for (i = 0; i < num_lanes; i++) {          \
            BCM_LOG_CLI(format, (lane_st[i].param)); \
        }                          \
        BCM_LOG_CLI("\n");                 \
    }   while (0)

/** Display two members of a lane struct. */
#define DISP_LNQ_VARS(name, param1, param2, format)                  \
    do {                                         \
        BCM_LOG_CLI("%-16s\t", name);                        \
        for (i = 0; i < num_lanes; i++) {                    \
            BCM_LOG_CLI(format, (lane_st[i].param1), (lane_st[i].param2)); \
        }                                    \
        BCM_LOG_CLI("\n");                           \
    }   while (0)

int16 _bcm_ladder_setting_to_mV (phy_ctrl_t *pa, int8 ctrl, uint8 range_250)
{
    uint8 absv;                                   /* Absolute value of ctrl */
    int16 nlmv;                                   /* Non-linear value */

    /* Get absolute value */
    absv = _abs(ctrl);
    {
        (void)range_250;

        nlmv = absv * 300 / 127;
    }

    /* Add sign and return */
    return (ctrl >= 0) ? nlmv : -nlmv;
}

uint16 _bcm_eye_to_mUI (phy_ctrl_t *pa, uint8 var)
{
    /* var is in units of 1/512 th UI, so need to multiply by 1000/512 */
    return ((uint16)var) * 125 / 64;
}

uint16 _bcm_eye_to_mV (phy_ctrl_t *pa, uint8 var, uint8 ladder_range)
{
    /* find nearest two vertical levels */
    uint16 vl = _bcm_ladder_setting_to_mV (pa, var, ladder_range);

    return vl;
}

int _bcm_read_die_temperature (phy_ctrl_t *pa, int16 *die_temp)
{
    uint16 die_temp_sensor_reading;

    EFUN(bcm_pe_pmd_uc_cmd (pa, CMD_UC_DBG, CMD_UC_DBG_DIE_TEMP, 50));
    ESTM(die_temp_sensor_reading = rd_uc_dsc_data());
    *die_temp = _bin_to_degC(die_temp_sensor_reading);

    return SOC_E_NONE;
}

int _bcm_update_uc_core_config_st (phy_ctrl_t *pa, struct bcm_uc_core_config_st *st)
{
    uint16 in = st->word;

    st->field.vco_rate = in & BFMASK(8); in >>= 8;
    st->field.core_cfg_from_pcs = in & BFMASK(1); in >>= 1;
    st->field.reserved = in & BFMASK(7); in >>= 7;
    st->vco_rate_in_Mhz = (1000 * (224 + st->field.vco_rate)) >> 4;
    return SOC_E_NONE;
}

int bcm_get_uc_core_config (phy_ctrl_t *pa, struct bcm_uc_core_config_st *get_val)
{
    if (!get_val)
        return SOC_E_PARAM;

    switch (bcm_pe_get_pll()) {
    case  0:
        ESTM(get_val->word = rdcv_config_word());
        break;
    case  1:
        ESTM(get_val->word = rdcv_config_pll1_word());
        break;
    default:
        return SOC_E_PARAM;
        break;
    }
    EFUN(_bcm_update_uc_core_config_st (pa, get_val));
    return SOC_E_NONE;
}


int _bcm_read_core_state (phy_ctrl_t*pa, bcm_core_state_st *istate)
{
    bcm_core_state_st state;
    struct bcm_uc_core_config_st core_cfg;

    bcm_memset(&state, 0, sizeof(bcm_core_state_st));
    bcm_memset(&core_cfg, 0, sizeof(struct bcm_uc_core_config_st));

    if (!istate)
        return SOC_E_PARAM;

    EFUN(bcm_get_uc_core_config (pa, &core_cfg));
    ESTM(state.rescal               = rd_ams_tx_ana_rescal());
    ESTM(state.core_reset           = rdc_core_dp_reset_state());
    ESTM(state.pll_pwrdn            = rdc_afe_s_pll_pwrdn());
    ESTM(state.uc_active            = rdc_uc_active());
    ESTM(state.comclk_mhz           = rdc_heartbeat_count_1us());
    ESTM(state.ucode_version        = rdcv_common_ucode_version());
    ESTM(state.ucode_minor_version  = rdcv_common_ucode_minor_version());
    ESTM(state.afe_hardware_version = rdcv_afe_hardware_version());
    ESTM(state.temp_idx             = rdcv_temp_idx());
    {
        int16 die_temp = 0;
        EFUN(_bcm_read_die_temperature (pa, &die_temp));
        state.die_temp               =    die_temp;
    }
    ESTM(state.avg_tmon             = rdcv_avg_tmon_reg13bit());
    state.vco_rate_mhz            = (uint16)core_cfg.vco_rate_in_Mhz;
    ESTM(state.analog_vco_range     = rdc_ams_pll_range());
    ESTM(state.pll_div              = rdc_ams_pll_ndiv());
    EFUN(_bcm_pe_pll_lock_status (pa, &state.pll_lock, &state.pll_lock_chg));
    ESTM(state.core_status          = rdcv_status_byte());

    *istate = state;
    return SOC_E_NONE;
}


int _bcm_display_pll_to_divider (phy_ctrl_t *pa, uint8 val)
{
    uint8 mmd_en;
    ESTM(mmd_en = rdc_ams_pll_fracn_sel());
    if (mmd_en) {
        int primary; long int secondary; int frac;
        ESTM(primary = rdc_ams_pll_fracn_ndiv_int());
        ESTM(secondary = rdc_ams_pll_fracn_div());
        ESTM(secondary |= rdc_ams_pll_fracn_div_17_16() << 16);
        frac = secondary / 262; /* 2^18 * 1000 */
        BCM_LOG_CLI("%3d.%03d ", primary, frac);
    } else {
      switch (val) {
      case 0x00:  BCM_LOG_CLI(" 64     ");
          break;
      case 0x01:  BCM_LOG_CLI(" 66     ");
          break;
      case 0x02:  BCM_LOG_CLI(" 80     ");
          break;
      case 0x03:  BCM_LOG_CLI(" 128    ");
          break;
      case 0x04:  BCM_LOG_CLI(" 132    ");
          break;
      case 0x05:  BCM_LOG_CLI(" 140    ");
          break;
      case 0x06:  BCM_LOG_CLI(" 160    ");
          break;
      case 0x07:  BCM_LOG_CLI(" 165    ");
          break;
      case 0x08:  BCM_LOG_CLI(" 168    ");
          break;
      case 0x09:  BCM_LOG_CLI(" 170    ");
          break;
      case 0x0a:  BCM_LOG_CLI(" 175    ");
          break;
      case 0x0b:  BCM_LOG_CLI(" 180    ");
          break;
      case 0x0c:  BCM_LOG_CLI(" 184    ");
          break;
      case 0x0d:  BCM_LOG_CLI(" 200    ");
          break;
      case 0x0e:  BCM_LOG_CLI(" 224    ");
          break;
      case 0x0f:  BCM_LOG_CLI(" 264    ");
          break;
      case 0x10:  BCM_LOG_CLI(" 96     ");
          break;
      case 0x11:  BCM_LOG_CLI(" 120    ");
          break;
      case 0x12:  BCM_LOG_CLI(" 144    ");
          break;
      case 0x13:  BCM_LOG_CLI(" 198    ");
          break;
      default:    BCM_LOG_CLI(" xxxxxxx ");
          BCM_LOG_CLI("ERROR: Invalid PLL_DIV VALUE\n");
          return SOC_E_PARAM;
      }
    }
    return SOC_E_NONE;

}

int bcm_display_core_state_hdr (phy_ctrl_t *pa)
{
    BCM_LOG_CLI("CORE RST_ST  PLL_PWDN  UC_ATV   COM_CLK   UCODE_VER  AFE_VER   LIVE_TEMP   AVG_TMON   RESCAL     VCO_RATE     ANA_VCO_RANGE  PLL_DIV    PLL_LOCK\n");
    return SOC_E_NONE;
}

int bcm_display_core_state_line (phy_ctrl_t *pc)
{
    bcm_core_state_st state;
    bcm_core_state_st state_pll1;

    bcm_memset(&state_pll1, 0, sizeof(state_pll1));
    bcm_memset(&state, 0, sizeof(state   ));
    {
        uint8 pll_orig;
        ESTM(pll_orig = bcm_pe_get_pll());
        EFUN(bcm_pe_set_pll(0));
        EFUN(_bcm_read_core_state (pc, &state));
        EFUN(bcm_pe_set_pll(1));
        EFUN(_bcm_read_core_state (pc, &state_pll1));
        EFUN(bcm_pe_set_pll(pll_orig));
    }
    BCM_LOG_CLI("%02d   ",  bcm_pe_get_core());
    BCM_LOG_CLI("   %x,%02x  ",  state.core_reset, state.core_status);
    BCM_LOG_CLI("    %1d,%1d    ",  state.pll_pwrdn, state_pll1.pll_pwrdn);
    BCM_LOG_CLI("    %1d    ",  state.uc_active);
    BCM_LOG_CLI(" %3d.%2dMHz", (state.comclk_mhz / 4), ((state.comclk_mhz % 4) * 25));  /* comclk in Mhz = heartbeat_count_1us / 4 */
    BCM_LOG_CLI("    %4X_%02X ",  state.ucode_version, state.ucode_minor_version);
    BCM_LOG_CLI("     0x%02x   ",  state.afe_hardware_version);
    BCM_LOG_CLI("    %3dC   ",  state.die_temp);
    BCM_LOG_CLI("    (%02d)%3dC ",  state.temp_idx, _bin_to_degC((state.avg_tmon >> 3)));
    BCM_LOG_CLI("    0x%02x  ",  state.rescal);
    BCM_LOG_CLI(" %2d.%2d,%2d.%2dGHz ", (state.vco_rate_mhz / 1000),
             (state.vco_rate_mhz % 1000),
             (state_pll1.vco_rate_mhz / 1000),
             (state_pll1.vco_rate_mhz % 1000));
    BCM_LOG_CLI("   %03d,%03d     ",  state.analog_vco_range,
             state_pll1.analog_vco_range);
    BCM_LOG_CLI(" (%02d,%02d)",  state.pll_div,
             state_pll1.pll_div);
    EFUN(_bcm_display_pll_to_divider(pc,state.pll_div));
    BCM_LOG_CLI(",");
    EFUN(_bcm_display_pll_to_divider(pc,    state_pll1.pll_div));
    if (state.pll_lock_chg)
        BCM_LOG_CLI("     %01d*,",  state.pll_lock);
    else
        BCM_LOG_CLI("     %01d ,",  state.pll_lock);
    if (state_pll1.pll_lock_chg)
        BCM_LOG_CLI("%01d* ",   state_pll1.pll_lock);
    else
        BCM_LOG_CLI("%01d   ",  state_pll1.pll_lock);

    BCM_LOG_CLI("\n");

    return SOC_E_NONE;
}

int bcm_display_core_state_legend (phy_ctrl_t *pc)
{
    BCM_LOG_CLI("\n");
    BCM_LOG_CLI("**************************************************************************************************************\n");
    BCM_LOG_CLI("****                          Legend of Entries in display_core_state()                                   ****\n");
    BCM_LOG_CLI("**************************************************************************************************************\n");
    BCM_LOG_CLI("*  RST_ST           : Core DP Reset State{reset_active, reset_occured, reset_held}, Core uC Status byte(hex) *\n");
    BCM_LOG_CLI("*  PLL_PWDN         : PLL Powerdown Control Bit (active high)                                                *\n");
    BCM_LOG_CLI("*  UC_ATV           : UC Active bit                                                                          *\n");
    BCM_LOG_CLI("*  COM_CLK          : COM Clock frequency in MHz                                                             *\n");
    BCM_LOG_CLI("*  UCODE_VER        : Microcode Version [majorversion_minorversion]                                          *\n");
    BCM_LOG_CLI("*  AFE_VER          : AFE Hardware Vesrion                                                                   *\n");
    BCM_LOG_CLI("*  LIVE_TEMP        : Live Die temperature in Celsius                                                        *\n");
    BCM_LOG_CLI("*  AVG_TMON         : uC Temp_idx, Average temperature in Celsius                                            *\n");
    BCM_LOG_CLI("*  RESCAL           : Analog Resistor Calibration value                                                      *\n");
    BCM_LOG_CLI("*  VCO_RATE         : uC VCO Rate in GHz (approximate)                                                       *\n");
    BCM_LOG_CLI("*  ANA_VCO_RANGE    : Analog VCO Range                                                                       *\n");
    BCM_LOG_CLI("*  PLL_DIV          : (Register Value) Actual PLL Divider Value                                                                      *\n");
    BCM_LOG_CLI("*  PLL_Lock         : PLL Lock                                                                               *\n");

    BCM_LOG_CLI("**************************************************************************************************************\n");
    return SOC_E_NONE;
}


int bcm_display_lane_state_hdr (phy_ctrl_t *pa)
{
    BCM_LOG_CLI("LN (CDRxN  , UC_CFG,RST,STP) ");
    BCM_LOG_CLI("SD LCK RXPPM ");
    BCM_LOG_CLI("CLK90 CLKP1 ");
    BCM_LOG_CLI("PF(M,L) ");
    BCM_LOG_CLI("VGA DCO ");
    BCM_LOG_CLI("P1mV ");
    BCM_LOG_CLI("M1mV ");
    BCM_LOG_CLI(" DFE(1,2,3,4,5,6)        ");
    BCM_LOG_CLI("TXPPM TXEQ(n1,m,p1,2,3)   EYE(L,R,U,D)  ");
    BCM_LOG_CLI("LINK_TIME");
    BCM_LOG_CLI("\n");
    return SOC_E_NONE;
}

int _bcm_get_osr_mode (phy_ctrl_t *pa, bcm_osr_mode_st *imode)
{
    bcm_osr_mode_st mode;

    bcm_memset(&mode, 0, sizeof(bcm_osr_mode_st));

    if (!imode)
        return SOC_E_PARAM;

    ESTM(mode.tx_rx = rd_osr_mode());
    mode.tx = mode.tx_rx; mode.rx = mode.tx_rx;
    *imode = mode;
    return SOC_E_NONE;
}


int _bcm_sigdet_status (phy_ctrl_t *pa, uint8 *sig_det, uint8 *sig_det_chg)
{
    uint16 rddata;

    EFUN(bcm_pe_pmd_rdt_reg (pa, SIGDET_STATUS_ADDR, &rddata));

    *sig_det     = (rddata & 0x1);
    *sig_det_chg = ((rddata >> 1) & 0x1);

    return SOC_E_NONE;
}

int bcm_get_clk90_offset (phy_ctrl_t *pa, int8 *offset)
{
    if (!offset)
        return SOC_E_PARAM;

    ESTM(*offset = rdv_usr_sts_phase_hoffset());

    return SOC_E_NONE;
}


int _bcm_get_rx_pf_main (phy_ctrl_t *pa, int8 *val)
{
    ESTM(*val = (int8)rd_rx_pf_ctrl());
    return SOC_E_NONE;
}

int _bcm_get_rx_pf2 (phy_ctrl_t *pa, int8 *val)
{
    ESTM(*val = (int8)rd_rx_pf2_ctrl());
    return SOC_E_NONE;
}

int _bcm_get_rx_vga (phy_ctrl_t *pa, int8 *val)
{
    ESTM(*val = (int8)rd_rx_vga_ctrl());
    return SOC_E_NONE;
}

int _bcm_get_rx_dfe1 (phy_ctrl_t *pa, int8 *val)
{
    ESTM(*val = (int8)rd_rx_data_thresh_sel());
    return SOC_E_NONE;
}

int _bcm_get_rx_dfe2 (phy_ctrl_t *pa, int8 *val)
{
    ESTM(*val = (int8)((rd_rxa_dfe_tap2() + rd_rxb_dfe_tap2() + rd_rxc_dfe_tap2() + rd_rxd_dfe_tap2()) >> 2));
    return SOC_E_NONE;
}

int _bcm_get_rx_dfe3 (phy_ctrl_t *pa, int8 *val)
{
    ESTM(*val = (rd_rxa_dfe_tap3() + rd_rxb_dfe_tap3() + rd_rxc_dfe_tap3() + rd_rxd_dfe_tap3()) >> 2);
    return SOC_E_NONE;
}

int _bcm_get_rx_dfe4 (phy_ctrl_t *pa, int8 *val)
{
    ESTM(*val = (rd_rxa_dfe_tap4() + rd_rxb_dfe_tap4() + rd_rxc_dfe_tap4() + rd_rxd_dfe_tap4()) >> 2);
    return SOC_E_NONE;
}

int _bcm_get_rx_dfe5 (phy_ctrl_t *pa, int8 *val)
{
    ESTM(*val = (rd_rxa_dfe_tap5() + rd_rxb_dfe_tap5() + rd_rxc_dfe_tap5() + rd_rxd_dfe_tap5()) >> 2);
    return SOC_E_NONE;
}

int _bcm_get_rx_dfe6 (phy_ctrl_t *pa, int8 *val)
{
    ESTM(*val = (rd_rxa_dfe_tap6() + rd_rxb_dfe_tap6() + rd_rxc_dfe_tap6() + rd_rxd_dfe_tap6()) >> 2);
    return SOC_E_NONE;
}

int _bcm_get_tx_pre (phy_ctrl_t *pa, int8 *val)
{
    ESTM(*val = rd_txfir_pre_after_ovr());
    return SOC_E_NONE;
}

int _bcm_get_tx_main (phy_ctrl_t *pa, int8 *val)
{
    ESTM(*val = rd_txfir_main_after_ovr());
    return SOC_E_NONE;
}

int _bcm_get_tx_post1 (phy_ctrl_t *pa, int8 *val)
{
    ESTM(*val = rd_txfir_post_after_ovr());
    return SOC_E_NONE;
}

int _bcm_get_tx_post2 (phy_ctrl_t *pa, int8 *val)
{
    ESTM(*val = rd_txfir_post2_adjusted());
    return SOC_E_NONE;
}

int _bcm_get_tx_post3 (phy_ctrl_t *pa, int8 *val)
{
    ESTM(*val = rd_txfir_post3_adjusted());
    return SOC_E_NONE;
}


int _bcm_get_tx_amp (phy_ctrl_t *pa, int8 *val)
{
    ESTM(*val = rd_ams_tx_amp_ctl());
    return SOC_E_NONE;
}

int bcm_read_tx_afe (phy_ctrl_t *pa, enum srds_tx_afe_settings_enum param, int8 *val)
{
    if (!val)
        return SOC_E_PARAM;

    switch (param) {
    case TX_AFE_PRE:
        EFUN(_bcm_get_tx_pre (pa, val));
        break;
    case TX_AFE_MAIN:
        EFUN(_bcm_get_tx_main (pa, val));
        break;
    case TX_AFE_POST1:
        EFUN(_bcm_get_tx_post1 (pa, val));
        break;
    case TX_AFE_POST2:
        EFUN(_bcm_get_tx_post2 (pa, val));
        break;
    case TX_AFE_POST3:
        EFUN(_bcm_get_tx_post3 (pa, val));
        break;
    case TX_AFE_AMP:
        EFUN(_bcm_get_tx_amp (pa, val));
        break;
    default:
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

int _bcm_read_lane_state (phy_ctrl_t *pa, bcm_lane_state_st *istate)
{
    bcm_lane_state_st state;
    uint8 ladder_range = 0;
    uint16 reg_data;

    bcm_memset(&state, 0, sizeof(bcm_lane_state_st));

    if (!istate)
        return SOC_E_PARAM;

    EFUN(_bcm_pe_pmd_lock_status (pa, &state.rx_lock, &state.rx_lock_chg));

    if (state.rx_lock == 1) {
        ESTM(state.stop_state = rdv_usr_sts_micro_stopped());
        if (!state.stop_state)
            EFUN(bcm_pe_stop_rx_adaptation (pa, 1));
    } else
        EFUN(bcm_pe_pmd_uc_control (pa, CMD_UC_CTRL_STOP_IMMEDIATE, 200));

    {  bcm_osr_mode_st osr_mode;
       bcm_memset(&osr_mode, 0, sizeof(bcm_osr_mode_st));
       EFUN(_bcm_get_osr_mode (pa, &osr_mode));
       state.osr_mode = osr_mode; }
    ESTM(state.ucv_config = rdv_config_word());
    ESTM(state.reset_state = rd_lane_dp_reset_state());
    EFUN(_bcm_sigdet_status (pa, &state.sig_det, &state.sig_det_chg));
    ESTM(state.rx_ppm = rd_cdr_integ_reg() / 84);
    EFUN(bcm_get_clk90_offset (pa, &state.clk90));
    EFUN(bcm_pe_pmd_rdt_reg (pa, DSC_E_RX_PI_CNT_BIN_LD, &reg_data));
    state.clkp1 = (uint8)dist_ccw(((reg_data >> 8) & 0xFF), (reg_data & 0xFF));
    ESTM(state.br_pd_en = rd_br_pd_en());
    /* drop the MSB, the result is actually valid modulo 128 */
    /* Also flip the sign to account for d-m1, versus m1-d */
    state.clkp1 = state.clkp1 << 1;
    state.clkp1 = -(state.clkp1 >> 1);

    EFUN(_bcm_get_rx_pf_main (pa, &state.pf_main));
    state.pf_hiz = 0;
    EFUN(_bcm_get_rx_pf2 (pa, &state.pf2_ctrl));
    EFUN(_bcm_get_rx_vga (pa, &state.vga));
    ESTM(state.dc_offset = rd_dc_offset_bin());
    ESTM(state.p1_lvl = rdv_usr_main_tap_est() / 32);
    state.p1_lvl = _bcm_ladder_setting_to_mV (pa, (int8)state.p1_lvl, 0);
    ESTM(state.m1_lvl = rd_rx_phase_thresh_sel());
    state.m1_lvl = _bcm_ladder_setting_to_mV (pa, (int8)state.m1_lvl, 0);

    /* ESTM(state.pf_bst = rd_ams_rx_pkbst()); */
    state.pf_bst = 0;

    EFUN(_bcm_get_rx_dfe1 (pa, &state.dfe1));
    EFUN(_bcm_get_rx_dfe2 (pa, &state.dfe2));
    EFUN(_bcm_get_rx_dfe3 (pa, &state.dfe3));
    EFUN(_bcm_get_rx_dfe4 (pa, &state.dfe4));
    EFUN(_bcm_get_rx_dfe5 (pa, &state.dfe5));
    EFUN(_bcm_get_rx_dfe6 (pa, &state.dfe6));


    /* tx_ppm = register/10.84 */
    ESTM(state.tx_ppm = (int16)(((int32)(rd_tx_pi_integ2_reg())) * 3125 / 32768));

    EFUN(_bcm_get_tx_pre (pa, &state.txfir_pre));
    EFUN(_bcm_get_tx_main (pa, &state.txfir_main));
    EFUN(_bcm_get_tx_post1 (pa, &state.txfir_post1));
    EFUN(_bcm_get_tx_post2 (pa, &state.txfir_post2));
    EFUN(_bcm_get_tx_post3 (pa, &state.txfir_post3));
    ESTM(state.heye_left = _bcm_eye_to_mUI (pa, rdv_usr_sts_heye_left()));
    ESTM(state.heye_right = _bcm_eye_to_mUI (pa, rdv_usr_sts_heye_right()));
    ESTM(state.veye_upper = _bcm_eye_to_mV (pa, rdv_usr_sts_veye_upper(), ladder_range));
    ESTM(state.veye_lower = _bcm_eye_to_mV (pa, rdv_usr_sts_veye_lower(), ladder_range));
    ESTM(state.link_time = (((uint32)rdv_usr_sts_link_time()) * 8) / 10); /* convert from 80us to 0.1 ms units */

    if (state.rx_lock == 1) {
        if (!state.stop_state)
            EFUN(bcm_pe_stop_rx_adaptation (pa, 0));
    } else
        EFUN(bcm_pe_stop_rx_adaptation (pa, 0));

    *istate = state;
    return SOC_E_NONE;
}


int bcm_display_lane_state (phy_ctrl_t *pa)
{
    uint16 lane_idx;
    bcm_lane_state_st state;

    const char* e2s_osr_mode_enum[13] = {
        "x1   ",
        "x2   ",
        "x4   ",
        "ERR  ",
        "ERR  ",
        "ERR  ",
        "ERR  ",
        "ERR  ",
        "x16.5",
        "ERR  ",
        "ERR  ",
        "ERR  ",
        "x20.6"
    };
    const char* e2s_tx_osr_mode_enum[10] = {
        "x1",
        "x2",
        "x3",
        "x3",
        "x4",
        "x5",
        "x7",
        "x8",
        "x8",
        "xA"
    };

    char *s;

    bcm_memset(&state, 0, sizeof(bcm_lane_state_st));

    EFUN(_bcm_read_lane_state (pa, &state));

    lane_idx = bcm_pe_tsc_get_lane (pa );

    BCM_LOG_CLI("%2d ", lane_idx);
    if (state.osr_mode.tx_rx != 255) {
        s = (char*)e2s_osr_mode_enum[state.osr_mode.tx_rx];
        BCM_LOG_CLI("(%2s%s, 0x%04x,", (state.br_pd_en) ? "BR" : "OS", s, state.ucv_config);
    } else {
        char *r;
        s = (char*)e2s_tx_osr_mode_enum[state.osr_mode.tx];
        r = (char*)e2s_tx_osr_mode_enum[state.osr_mode.rx];
        BCM_LOG_CLI("(%2s%s:%s, 0x%04x,", (state.br_pd_en) ? "BR" : "OS", s, r, state.ucv_config);
    }
    BCM_LOG_CLI("   %01x, %01x)", state.reset_state, state.stop_state);
    if (state.sig_det_chg)
        BCM_LOG_CLI("  %1d*", state.sig_det);
    else
        BCM_LOG_CLI("  %1d ", state.sig_det);
    if (state.rx_lock_chg)
        BCM_LOG_CLI("  %1d*", state.rx_lock);
    else
        BCM_LOG_CLI("  %1d ", state.rx_lock);
    BCM_LOG_CLI(" %4d ", state.rx_ppm);
    BCM_LOG_CLI("  %3d   %3d ", state.clk90, state.clkp1);
    BCM_LOG_CLI("  %2d,%1d ", state.pf_main, state.pf2_ctrl);
    BCM_LOG_CLI("   %2d ", state.vga);
    BCM_LOG_CLI("%3d ", state.dc_offset);
    BCM_LOG_CLI("%4d ", state.p1_lvl);
    BCM_LOG_CLI("%4d ", state.m1_lvl);
    BCM_LOG_CLI("%3d,%3d,%3d,%3d,%3d,%3d ", state.dfe1, state.dfe2, state.dfe3, state.dfe4, state.dfe5, state.dfe6);
    BCM_LOG_CLI(" %4d ", state.tx_ppm);
    BCM_LOG_CLI("  %2d,%3d,%2d,%2d,%2d  ", state.txfir_pre, state.txfir_main, state.txfir_post1, state.txfir_post2, state.txfir_post3);
    BCM_LOG_CLI(" %3d,%3d,%3d,%3d ", state.heye_left, state.heye_right, state.veye_upper, state.veye_lower);
    BCM_LOG_CLI(" %4d.%01d", state.link_time / 10, state.link_time % 10);
    return SOC_E_NONE;
}


int bcm_log_full_pmd_state (phy_ctrl_t *pa, struct bcm_detailed_lane_status_st *lane_st)
{
    uint16 reg_data;
    int8 tmp;

    if (!lane_st)
        return SOC_E_PARAM;

    ESTM(lane_st->pmd_lock = rd_pmd_rx_lock());

    lane_st->stop_state = 0;
    if (lane_st->pmd_lock == 1) {
        ESTM(lane_st->stop_state = rdv_usr_sts_micro_stopped());
        if (!lane_st->stop_state)
            EFUN(bcm_pe_stop_rx_adaptation (pa, 1));
    } else
        EFUN(bcm_pe_pmd_uc_control (pa, CMD_UC_CTRL_STOP_IMMEDIATE, 200));

    ESTM(lane_st->reset_state = rd_lane_dp_reset_state());

    ESTM(lane_st->pmd_pll_select = rd_pll_select());
    ESTM(lane_st->temp_idx = rdcv_temp_idx());
    ESTM(lane_st->ams_tx_drv_hv_disable = rd_ams_tx_drv_hv_disable());
    ESTM(lane_st->ams_tx_ana_rescal = rd_ams_tx_ana_rescal());
    EFUN(bcm_read_tx_afe (pa, TX_AFE_AMP, &tmp)); lane_st->amp_ctrl = (uint8)tmp;
    EFUN(bcm_read_tx_afe (pa, TX_AFE_PRE, &tmp)); lane_st->pre_tap = (uint8)tmp;
    EFUN(bcm_read_tx_afe (pa, TX_AFE_MAIN, &tmp)); lane_st->main_tap = (uint8)tmp;
    EFUN(bcm_read_tx_afe (pa, TX_AFE_POST1, &tmp)); lane_st->post1_tap = (uint8)tmp;
    EFUN(bcm_read_tx_afe (pa, TX_AFE_POST2, &lane_st->post2_tap));
    EFUN(bcm_read_tx_afe (pa, TX_AFE_POST3, &lane_st->post3_tap));
    ESTM(lane_st->sigdet = rd_signal_detect());
    ESTM(lane_st->dsc_sm[0] = rd_dsc_state_one_hot());
    ESTM(lane_st->dsc_sm[1] = rd_dsc_state_one_hot());
#ifdef SERDES_API_FLOATING_POINT
    ESTM(lane_st->ppm = (((double)1e6 / 64 / 20 / 128 / 16) * (int16)(rd_cdr_integ_reg() / 32)));
#else
    ESTM(lane_st->ppm = ((int16)(rd_cdr_integ_reg()) * 12 / 1000));
#endif
    ESTM(lane_st->vga = rd_rx_vga_ctrl());
    ESTM(lane_st->pf = rd_rx_pf_ctrl());
    ESTM(lane_st->pf2 = rd_rx_pf2_ctrl());
#ifdef SERDES_API_FLOATING_POINT
    ESTM(lane_st->main_tap_est = rdv_usr_main_tap_est() / 32.0);
#else
    ESTM(lane_st->main_tap_est = rdv_usr_main_tap_est() / 32);
#endif
    ESTM(lane_st->data_thresh = rd_rx_data_thresh_sel());
    ESTM(lane_st->phase_thresh = rd_rx_phase_thresh_sel());
    ESTM(lane_st->lms_thresh = rd_rx_lms_thresh_sel());
    EFUN(bcm_pe_pmd_rdt_reg (pa, DSC_E_RX_PI_CNT_BIN_D, &reg_data));
    lane_st->ddq_hoffset = (uint8)dist_ccw(((reg_data >> 8) & 0xFF), (reg_data & 0xFF));
    EFUN(bcm_pe_pmd_rdt_reg (pa, DSC_E_RX_PI_CNT_BIN_P, &reg_data));
    lane_st->ppq_hoffset = (uint8)dist_ccw(((reg_data >> 8) & 0xFF), (reg_data & 0xFF));
    EFUN(bcm_pe_pmd_rdt_reg (pa, DSC_E_RX_PI_CNT_BIN_L, &reg_data));
    lane_st->llq_hoffset = (uint8)dist_ccw(((reg_data >> 8) & 0xFF), (reg_data & 0xFF));
    EFUN(bcm_pe_pmd_rdt_reg (pa, DSC_E_RX_PI_CNT_BIN_PD, &reg_data));
    lane_st->dp_hoffset = (uint8)dist_cw(((reg_data >> 8) & 0xFF), (reg_data & 0xFF));
    EFUN(bcm_pe_pmd_rdt_reg (pa, DSC_E_RX_PI_CNT_BIN_LD, &reg_data));
    lane_st->dl_hoffset = (uint8)dist_cw(((reg_data >> 8) & 0xFF), (reg_data & 0xFF));
    ESTM(lane_st->dc_offset = rd_dc_offset_bin());
    ESTM(lane_st->dfe[1][0] = rd_rxa_dfe_tap2());
    ESTM(lane_st->dfe[1][1] = rd_rxb_dfe_tap2());
    ESTM(lane_st->dfe[1][2] = rd_rxc_dfe_tap2());
    ESTM(lane_st->dfe[1][3] = rd_rxd_dfe_tap2());
    ESTM(lane_st->dfe[2][0] = rd_rxa_dfe_tap3());
    ESTM(lane_st->dfe[2][1] = rd_rxb_dfe_tap3());
    ESTM(lane_st->dfe[2][2] = rd_rxc_dfe_tap3());
    ESTM(lane_st->dfe[2][3] = rd_rxd_dfe_tap3());
    ESTM(lane_st->dfe[3][0] = rd_rxa_dfe_tap4());
    ESTM(lane_st->dfe[3][1] = rd_rxb_dfe_tap4());
    ESTM(lane_st->dfe[3][2] = rd_rxc_dfe_tap4());
    ESTM(lane_st->dfe[3][3] = rd_rxd_dfe_tap4());
    ESTM(lane_st->dfe[4][0] = rd_rxa_dfe_tap5());
    ESTM(lane_st->dfe[4][1] = rd_rxb_dfe_tap5());
    ESTM(lane_st->dfe[4][2] = rd_rxc_dfe_tap5());
    ESTM(lane_st->dfe[4][3] = rd_rxd_dfe_tap5());
    ESTM(lane_st->dfe[5][0] = rd_rxa_dfe_tap6());
    ESTM(lane_st->dfe[5][1] = rd_rxb_dfe_tap6());
    ESTM(lane_st->dfe[5][2] = rd_rxc_dfe_tap6());
    ESTM(lane_st->dfe[5][3] = rd_rxd_dfe_tap6());
    ESTM(lane_st->dfe[6][0] = ((rd_rxa_dfe_tap7_mux() == 0) ? rd_rxa_dfe_tap7() : 0));
    ESTM(lane_st->dfe[6][1] = ((rd_rxb_dfe_tap7_mux() == 0) ? rd_rxb_dfe_tap7() : 0));
    ESTM(lane_st->dfe[6][2] = ((rd_rxc_dfe_tap7_mux() == 0) ? rd_rxc_dfe_tap7() : 0));
    ESTM(lane_st->dfe[6][3] = ((rd_rxd_dfe_tap7_mux() == 0) ? rd_rxd_dfe_tap7() : 0));
    ESTM(lane_st->dfe[7][0] = ((rd_rxa_dfe_tap8_mux() == 0) ? rd_rxa_dfe_tap8() : 0));
    ESTM(lane_st->dfe[7][1] = ((rd_rxb_dfe_tap8_mux() == 0) ? rd_rxb_dfe_tap8() : 0));
    ESTM(lane_st->dfe[7][2] = ((rd_rxc_dfe_tap8_mux() == 0) ? rd_rxc_dfe_tap8() : 0));
    ESTM(lane_st->dfe[7][3] = ((rd_rxd_dfe_tap8_mux() == 0) ? rd_rxd_dfe_tap8() : 0));
    ESTM(lane_st->dfe[8][0] = ((rd_rxa_dfe_tap9_mux() == 0) ? rd_rxa_dfe_tap9() : 0));
    ESTM(lane_st->dfe[8][1] = ((rd_rxb_dfe_tap9_mux() == 0) ? rd_rxb_dfe_tap9() : 0));
    ESTM(lane_st->dfe[8][2] = ((rd_rxc_dfe_tap9_mux() == 0) ? rd_rxc_dfe_tap9() : 0));
    ESTM(lane_st->dfe[8][3] = ((rd_rxd_dfe_tap9_mux() == 0) ? rd_rxd_dfe_tap9() : 0));
    ESTM(lane_st->dfe[9][0] = ((rd_rxa_dfe_tap10_mux() == 0) ? rd_rxa_dfe_tap10() : 0));
    ESTM(lane_st->dfe[9][1] = ((rd_rxb_dfe_tap10_mux() == 0) ? rd_rxb_dfe_tap10() : 0));
    ESTM(lane_st->dfe[9][2] = ((rd_rxc_dfe_tap10_mux() == 0) ? rd_rxc_dfe_tap10() : 0));
    ESTM(lane_st->dfe[9][3] = ((rd_rxd_dfe_tap10_mux() == 0) ? rd_rxd_dfe_tap10() : 0));
    ESTM(lane_st->dfe[10][0] = ((rd_rxa_dfe_tap7_mux() == 1) ? rd_rxa_dfe_tap7() : (rd_rxa_dfe_tap11_mux() == 0) ? rd_rxa_dfe_tap11() : 0));
    ESTM(lane_st->dfe[10][1] = ((rd_rxb_dfe_tap7_mux() == 1) ? rd_rxb_dfe_tap7() : (rd_rxb_dfe_tap11_mux() == 0) ? rd_rxb_dfe_tap11() : 0));
    ESTM(lane_st->dfe[10][2] = ((rd_rxc_dfe_tap7_mux() == 1) ? rd_rxc_dfe_tap7() : (rd_rxc_dfe_tap11_mux() == 0) ? rd_rxc_dfe_tap11() : 0));
    ESTM(lane_st->dfe[10][3] = ((rd_rxd_dfe_tap7_mux() == 1) ? rd_rxd_dfe_tap7() : (rd_rxd_dfe_tap11_mux() == 0) ? rd_rxd_dfe_tap11() : 0));
    ESTM(lane_st->dfe[11][0] = ((rd_rxa_dfe_tap8_mux() == 1) ? rd_rxa_dfe_tap8() : (rd_rxa_dfe_tap12_mux() == 0) ? rd_rxa_dfe_tap12() : 0));
    ESTM(lane_st->dfe[11][1] = ((rd_rxb_dfe_tap8_mux() == 1) ? rd_rxb_dfe_tap8() : (rd_rxb_dfe_tap12_mux() == 0) ? rd_rxb_dfe_tap12() : 0));
    ESTM(lane_st->dfe[11][2] = ((rd_rxc_dfe_tap8_mux() == 1) ? rd_rxc_dfe_tap8() : (rd_rxc_dfe_tap12_mux() == 0) ? rd_rxc_dfe_tap12() : 0));
    ESTM(lane_st->dfe[11][3] = ((rd_rxd_dfe_tap8_mux() == 1) ? rd_rxd_dfe_tap8() : (rd_rxd_dfe_tap12_mux() == 0) ? rd_rxd_dfe_tap12() : 0));
    ESTM(lane_st->dfe[12][0] = ((rd_rxa_dfe_tap9_mux() == 1) ? rd_rxa_dfe_tap9() : (rd_rxa_dfe_tap13_mux() == 0) ? rd_rxa_dfe_tap13() : 0));
    ESTM(lane_st->dfe[12][1] = ((rd_rxb_dfe_tap9_mux() == 1) ? rd_rxb_dfe_tap9() : (rd_rxb_dfe_tap13_mux() == 0) ? rd_rxb_dfe_tap13() : 0));
    ESTM(lane_st->dfe[12][2] = ((rd_rxc_dfe_tap9_mux() == 1) ? rd_rxc_dfe_tap9() : (rd_rxc_dfe_tap13_mux() == 0) ? rd_rxc_dfe_tap13() : 0));
    ESTM(lane_st->dfe[12][3] = ((rd_rxd_dfe_tap9_mux() == 1) ? rd_rxd_dfe_tap9() : (rd_rxd_dfe_tap13_mux() == 0) ? rd_rxd_dfe_tap13() : 0));
    ESTM(lane_st->dfe[13][0] = ((rd_rxa_dfe_tap10_mux() == 1) ? rd_rxa_dfe_tap10() : (rd_rxa_dfe_tap14_mux() == 0) ? rd_rxa_dfe_tap14() : 0));
    ESTM(lane_st->dfe[13][1] = ((rd_rxb_dfe_tap10_mux() == 1) ? rd_rxb_dfe_tap10() : (rd_rxb_dfe_tap14_mux() == 0) ? rd_rxb_dfe_tap14() : 0));
    ESTM(lane_st->dfe[13][2] = ((rd_rxc_dfe_tap10_mux() == 1) ? rd_rxc_dfe_tap10() : (rd_rxc_dfe_tap14_mux() == 0) ? rd_rxc_dfe_tap14() : 0));
    ESTM(lane_st->dfe[13][3] = ((rd_rxd_dfe_tap10_mux() == 1) ? rd_rxd_dfe_tap10() : (rd_rxd_dfe_tap14_mux() == 0) ? rd_rxd_dfe_tap14() : 0));
    ESTM(lane_st->dfe[14][0] = ((rd_rxa_dfe_tap7_mux() == 2) ? rd_rxa_dfe_tap7() : (rd_rxa_dfe_tap11_mux() == 1) ? rd_rxa_dfe_tap11() : 0));
    ESTM(lane_st->dfe[14][1] = ((rd_rxb_dfe_tap7_mux() == 2) ? rd_rxb_dfe_tap7() : (rd_rxb_dfe_tap11_mux() == 1) ? rd_rxb_dfe_tap11() : 0));
    ESTM(lane_st->dfe[14][2] = ((rd_rxc_dfe_tap7_mux() == 2) ? rd_rxc_dfe_tap7() : (rd_rxc_dfe_tap11_mux() == 1) ? rd_rxc_dfe_tap11() : 0));
    ESTM(lane_st->dfe[14][3] = ((rd_rxd_dfe_tap7_mux() == 2) ? rd_rxd_dfe_tap7() : (rd_rxd_dfe_tap11_mux() == 1) ? rd_rxd_dfe_tap11() : 0));
    ESTM(lane_st->dfe[15][0] = ((rd_rxa_dfe_tap8_mux() == 2) ? rd_rxa_dfe_tap8() : (rd_rxa_dfe_tap12_mux() == 1) ? rd_rxa_dfe_tap12() : 0));
    ESTM(lane_st->dfe[15][1] = ((rd_rxb_dfe_tap8_mux() == 2) ? rd_rxb_dfe_tap8() : (rd_rxb_dfe_tap12_mux() == 1) ? rd_rxb_dfe_tap12() : 0));
    ESTM(lane_st->dfe[15][2] = ((rd_rxc_dfe_tap8_mux() == 2) ? rd_rxc_dfe_tap8() : (rd_rxc_dfe_tap12_mux() == 1) ? rd_rxc_dfe_tap12() : 0));
    ESTM(lane_st->dfe[15][3] = ((rd_rxd_dfe_tap8_mux() == 2) ? rd_rxd_dfe_tap8() : (rd_rxd_dfe_tap12_mux() == 1) ? rd_rxd_dfe_tap12() : 0));
    ESTM(lane_st->dfe[16][0] = ((rd_rxa_dfe_tap9_mux() == 2) ? rd_rxa_dfe_tap9() : (rd_rxa_dfe_tap13_mux() == 1) ? rd_rxa_dfe_tap13() : 0));
    ESTM(lane_st->dfe[16][1] = ((rd_rxb_dfe_tap9_mux() == 2) ? rd_rxb_dfe_tap9() : (rd_rxb_dfe_tap13_mux() == 1) ? rd_rxb_dfe_tap13() : 0));
    ESTM(lane_st->dfe[16][2] = ((rd_rxc_dfe_tap9_mux() == 2) ? rd_rxc_dfe_tap9() : (rd_rxc_dfe_tap13_mux() == 1) ? rd_rxc_dfe_tap13() : 0));
    ESTM(lane_st->dfe[16][3] = ((rd_rxd_dfe_tap9_mux() == 2) ? rd_rxd_dfe_tap9() : (rd_rxd_dfe_tap13_mux() == 1) ? rd_rxd_dfe_tap13() : 0));
    ESTM(lane_st->dfe[17][0] = ((rd_rxa_dfe_tap10_mux() == 2) ? rd_rxa_dfe_tap10() : (rd_rxa_dfe_tap14_mux() == 1) ? rd_rxa_dfe_tap14() : 0));
    ESTM(lane_st->dfe[17][1] = ((rd_rxb_dfe_tap10_mux() == 2) ? rd_rxb_dfe_tap10() : (rd_rxb_dfe_tap14_mux() == 1) ? rd_rxb_dfe_tap14() : 0));
    ESTM(lane_st->dfe[17][2] = ((rd_rxc_dfe_tap10_mux() == 2) ? rd_rxc_dfe_tap10() : (rd_rxc_dfe_tap14_mux() == 1) ? rd_rxc_dfe_tap14() : 0));
    ESTM(lane_st->dfe[17][3] = ((rd_rxd_dfe_tap10_mux() == 2) ? rd_rxd_dfe_tap10() : (rd_rxd_dfe_tap14_mux() == 1) ? rd_rxd_dfe_tap14() : 0));
    ESTM(lane_st->dfe[18][0] = ((rd_rxa_dfe_tap7_mux() == 3) ? rd_rxa_dfe_tap7() : (rd_rxa_dfe_tap11_mux() == 2) ? rd_rxa_dfe_tap11() : 0));
    ESTM(lane_st->dfe[18][1] = ((rd_rxb_dfe_tap7_mux() == 3) ? rd_rxb_dfe_tap7() : (rd_rxb_dfe_tap11_mux() == 2) ? rd_rxb_dfe_tap11() : 0));
    ESTM(lane_st->dfe[18][2] = ((rd_rxc_dfe_tap7_mux() == 3) ? rd_rxc_dfe_tap7() : (rd_rxc_dfe_tap11_mux() == 2) ? rd_rxc_dfe_tap11() : 0));
    ESTM(lane_st->dfe[18][3] = ((rd_rxd_dfe_tap7_mux() == 3) ? rd_rxd_dfe_tap7() : (rd_rxd_dfe_tap11_mux() == 2) ? rd_rxd_dfe_tap11() : 0));
    ESTM(lane_st->dfe[19][0] = ((rd_rxa_dfe_tap8_mux() == 3) ? rd_rxa_dfe_tap8() : (rd_rxa_dfe_tap12_mux() == 2) ? rd_rxa_dfe_tap12() : 0));
    ESTM(lane_st->dfe[19][1] = ((rd_rxb_dfe_tap8_mux() == 3) ? rd_rxb_dfe_tap8() : (rd_rxb_dfe_tap12_mux() == 2) ? rd_rxb_dfe_tap12() : 0));
    ESTM(lane_st->dfe[19][2] = ((rd_rxc_dfe_tap8_mux() == 3) ? rd_rxc_dfe_tap8() : (rd_rxc_dfe_tap12_mux() == 2) ? rd_rxc_dfe_tap12() : 0));
    ESTM(lane_st->dfe[19][3] = ((rd_rxd_dfe_tap8_mux() == 3) ? rd_rxd_dfe_tap8() : (rd_rxd_dfe_tap12_mux() == 2) ? rd_rxd_dfe_tap12() : 0));
    ESTM(lane_st->dfe[20][0] = ((rd_rxa_dfe_tap9_mux() == 3) ? rd_rxa_dfe_tap9() : (rd_rxa_dfe_tap13_mux() == 2) ? rd_rxa_dfe_tap13() : 0));
    ESTM(lane_st->dfe[20][1] = ((rd_rxb_dfe_tap9_mux() == 3) ? rd_rxb_dfe_tap9() : (rd_rxb_dfe_tap13_mux() == 2) ? rd_rxb_dfe_tap13() : 0));
    ESTM(lane_st->dfe[20][2] = ((rd_rxc_dfe_tap9_mux() == 3) ? rd_rxc_dfe_tap9() : (rd_rxc_dfe_tap13_mux() == 2) ? rd_rxc_dfe_tap13() : 0));
    ESTM(lane_st->dfe[20][3] = ((rd_rxd_dfe_tap9_mux() == 3) ? rd_rxd_dfe_tap9() : (rd_rxd_dfe_tap13_mux() == 2) ? rd_rxd_dfe_tap13() : 0));
    ESTM(lane_st->dfe[21][0] = ((rd_rxa_dfe_tap10_mux() == 3) ? rd_rxa_dfe_tap10() : (rd_rxa_dfe_tap14_mux() == 2) ? rd_rxa_dfe_tap14() : 0));
    ESTM(lane_st->dfe[21][1] = ((rd_rxb_dfe_tap10_mux() == 3) ? rd_rxb_dfe_tap10() : (rd_rxb_dfe_tap14_mux() == 2) ? rd_rxb_dfe_tap14() : 0));
    ESTM(lane_st->dfe[21][2] = ((rd_rxc_dfe_tap10_mux() == 3) ? rd_rxc_dfe_tap10() : (rd_rxc_dfe_tap14_mux() == 2) ? rd_rxc_dfe_tap14() : 0));
    ESTM(lane_st->dfe[21][3] = ((rd_rxd_dfe_tap10_mux() == 3) ? rd_rxd_dfe_tap10() : (rd_rxd_dfe_tap14_mux() == 2) ? rd_rxd_dfe_tap14() : 0));
    ESTM(lane_st->dfe[22][0] = ((rd_rxa_dfe_tap11_mux() == 3) ? rd_rxa_dfe_tap11() : 0));
    ESTM(lane_st->dfe[22][1] = ((rd_rxb_dfe_tap11_mux() == 3) ? rd_rxb_dfe_tap11() : 0));
    ESTM(lane_st->dfe[22][2] = ((rd_rxc_dfe_tap11_mux() == 3) ? rd_rxc_dfe_tap11() : 0));
    ESTM(lane_st->dfe[22][3] = ((rd_rxd_dfe_tap11_mux() == 3) ? rd_rxd_dfe_tap11() : 0));
    ESTM(lane_st->dfe[23][0] = ((rd_rxa_dfe_tap12_mux() == 3) ? rd_rxa_dfe_tap12() : 0));
    ESTM(lane_st->dfe[23][1] = ((rd_rxb_dfe_tap12_mux() == 3) ? rd_rxb_dfe_tap12() : 0));
    ESTM(lane_st->dfe[23][2] = ((rd_rxc_dfe_tap12_mux() == 3) ? rd_rxc_dfe_tap12() : 0));
    ESTM(lane_st->dfe[23][3] = ((rd_rxd_dfe_tap12_mux() == 3) ? rd_rxd_dfe_tap12() : 0));
    ESTM(lane_st->dfe[24][0] = ((rd_rxa_dfe_tap13_mux() == 3) ? rd_rxa_dfe_tap13() : 0));
    ESTM(lane_st->dfe[24][1] = ((rd_rxb_dfe_tap13_mux() == 3) ? rd_rxb_dfe_tap13() : 0));
    ESTM(lane_st->dfe[24][2] = ((rd_rxc_dfe_tap13_mux() == 3) ? rd_rxc_dfe_tap13() : 0));
    ESTM(lane_st->dfe[24][3] = ((rd_rxd_dfe_tap13_mux() == 3) ? rd_rxd_dfe_tap13() : 0));
    ESTM(lane_st->dfe[25][0] = ((rd_rxa_dfe_tap14_mux() == 3) ? rd_rxa_dfe_tap14() : 0));
    ESTM(lane_st->dfe[25][1] = ((rd_rxb_dfe_tap14_mux() == 3) ? rd_rxb_dfe_tap14() : 0));
    ESTM(lane_st->dfe[25][2] = ((rd_rxc_dfe_tap14_mux() == 3) ? rd_rxc_dfe_tap14() : 0));
    ESTM(lane_st->dfe[25][3] = ((rd_rxd_dfe_tap14_mux() == 3) ? rd_rxd_dfe_tap14() : 0));
    ESTM(lane_st->thctrl_dp[0] = rd_rxa_slicer_offset_adj_dp());
    ESTM(lane_st->thctrl_dp[1] = rd_rxb_slicer_offset_adj_dp());
    ESTM(lane_st->thctrl_dp[2] = rd_rxc_slicer_offset_adj_dp());
    ESTM(lane_st->thctrl_dp[3] = rd_rxd_slicer_offset_adj_dp());
    ESTM(lane_st->thctrl_dn[0] = rd_rxa_slicer_offset_adj_dn());
    ESTM(lane_st->thctrl_dn[1] = rd_rxb_slicer_offset_adj_dn());
    ESTM(lane_st->thctrl_dn[2] = rd_rxc_slicer_offset_adj_dn());
    ESTM(lane_st->thctrl_dn[3] = rd_rxd_slicer_offset_adj_dn());
    ESTM(lane_st->thctrl_zp[0] = rd_rxa_slicer_offset_adj_zp());
    ESTM(lane_st->thctrl_zp[1] = rd_rxb_slicer_offset_adj_zp());
    ESTM(lane_st->thctrl_zp[2] = rd_rxc_slicer_offset_adj_zp());
    ESTM(lane_st->thctrl_zp[3] = rd_rxd_slicer_offset_adj_zp());
    ESTM(lane_st->thctrl_zn[0] = rd_rxa_slicer_offset_adj_zn());
    ESTM(lane_st->thctrl_zn[1] = rd_rxb_slicer_offset_adj_zn());
    ESTM(lane_st->thctrl_zn[2] = rd_rxc_slicer_offset_adj_zn());
    ESTM(lane_st->thctrl_zn[3] = rd_rxd_slicer_offset_adj_zn());
    ESTM(lane_st->thctrl_l[0] = rd_rxa_slicer_offset_adj_lms());
    ESTM(lane_st->thctrl_l[1] = rd_rxb_slicer_offset_adj_lms());
    ESTM(lane_st->thctrl_l[2] = rd_rxc_slicer_offset_adj_lms());
    ESTM(lane_st->thctrl_l[3] = rd_rxd_slicer_offset_adj_lms());
    ESTM(lane_st->heye_left = _bcm_eye_to_mUI (pa, rdv_usr_sts_heye_left()));
    ESTM(lane_st->heye_right = _bcm_eye_to_mUI (pa, rdv_usr_sts_heye_right()));
    ESTM(lane_st->veye_upper = _bcm_eye_to_mV (pa, rdv_usr_sts_veye_upper(), 0));
    ESTM(lane_st->veye_lower = _bcm_eye_to_mV (pa, rdv_usr_sts_veye_lower(), 0));
    ESTM(lane_st->link_time = (((uint32)rdv_usr_sts_link_time()) * 8) / 10);
    ESTM(lane_st->prbs_chk_en = rd_prbs_chk_en());
    ESTM(reg_data = rd_prbs_chk_mode_sel());
    lane_st->prbs_chk_order = (reg_data == 0) ? 7 :
                  (reg_data == 1) ? 9 :
                  (reg_data == 2) ? 11 :
                  (reg_data == 3) ? 15 :
                  (reg_data == 4) ? 23 :
                  (reg_data == 5) ? 31 :
                  (reg_data == 6) ? 58 : 0;
    EFUN(bcm_pe_prbs_chk_lock_state (pa, &lane_st->prbs_chk_lock));
    EFUN(bcm_pe_prbs_err_count_ll (pa, &lane_st->prbs_chk_errcnt));

    if (lane_st->pmd_lock == 1) {
        if (!lane_st->stop_state)
            EFUN(bcm_pe_stop_rx_adaptation (pa, 0));
    } else
        EFUN(bcm_pe_stop_rx_adaptation (pa, 0));

    return SOC_E_NONE;
}


int bcm_disp_full_pmd_state (phy_ctrl_t *pa, struct bcm_detailed_lane_status_st *lane_st, uint8 num_lanes)
{
    uint8 i;

    BCM_LOG_CLI( "\n\n");
    BCM_LOG_CLI( "------------------------------------------------------------------------\n");
    BCM_LOG_CLI( "PHY8806x PMD State\n");
    BCM_LOG_CLI( "%-16s\t%12s%12s%12s%12s\n", "PARAMETER", "LN0", "LN1", "LN2", "LN3");
    BCM_LOG_CLI( "------------------------------------------------------------------------\n");
    DISP_LN_VARS( "HEYE Left (mUI)", heye_left, "%12d");
    DISP_LN_VARS( "HEYE Rght (mUI)", heye_right, "%12d");
    DISP_LN_VARS( "VEYE Top  (mV)", veye_upper, "%12d");
    DISP_LN_VARS( "VEYE Bott (mV)", veye_lower, "%12d");
#ifdef SERDES_API_FLOATING_POINT
    DISP_LN_VARS( "Link Time (ms)", link_time / 10.0, "%12.1f");
#else
    DISP_LN_VARS( "Link Time (ms)", link_time / 10, "%12d");
#endif
    DISP_LN_VARS("Ln Reset State", reset_state, "%12d");
    DISP_LN_VARS("uC Stop", stop_state, "%12d");

    BCM_LOG_CLI( "------------------------------------------------------------------------\n");
    DISP_LN_VARS( "TX drv_hv_disable", ams_tx_drv_hv_disable, "%12d");
    DISP_LN_VARS( "TX ana_rescal", ams_tx_ana_rescal, "%12d");
    DISP_LN_VARS( "TX amp_ctrl", amp_ctrl, "%12d");
    DISP_LN_VARS( "TX pre_tap", pre_tap, "%12d");
    DISP_LN_VARS( "TX main_tap", main_tap, "%12d");
    DISP_LN_VARS( "TX post1_tap", post1_tap, "%12d");
    DISP_LN_VARS( "TX post2_tap", post2_tap, "%12d");
    DISP_LN_VARS( "TX post3_tap", post3_tap, "%12d");
    BCM_LOG_CLI( "------------------------------------------------------------------------\n");
    DISP_LN_VARS( "Sigdet", sigdet, "%12d");
    DISP_LN_VARS( "PMD_lock", pmd_lock, "%12d");
    DISP_LN_VARS( "PLL select", pmd_pll_select, "%12d");
    DISP_LN_VARS( "DSC_SM (1st read)", dsc_sm[0], "       %4xh");
    DISP_LN_VARS( "DSC_SM (2nd read)", dsc_sm[1], "       %4xh");
    BCM_LOG_CLI( "------------------------------------------------------------------------\n");
#ifdef SERDES_API_FLOATING_POINT
    DISP_LN_VARS( "PPM", ppm, "%12.2f");
#else
    DISP_LN_VARS( "PPM", ppm, "%12d");
#endif
    DISP_LN_VARS( "VGA", vga, "%12d");
    DISP_LN_VARS( "PF", pf, "%12d");
    DISP_LN_VARS( "PF2", pf2, "%12d");
#ifdef SERDES_API_FLOATING_POINT
    DISP_LN_VARS( "main_tap", main_tap_est, "%12.2f");
#else
    DISP_LN_VARS( "main_tap", main_tap_est, "%12d");
#endif
    DISP_LN_VARS( "data_thresh", data_thresh, "%12d");
    DISP_LN_VARS( "phase_thresh", phase_thresh, "%12d");
    DISP_LN_VARS( "lms_thresh", lms_thresh, "%12d");
    BCM_LOG_CLI( "------------------------------------------------------------------------\n");
    DISP_LN_VARS( "d/dq hoffset", ddq_hoffset, "%12u");
    DISP_LN_VARS( "p/pq hoffset", ppq_hoffset, "%12u");
    DISP_LN_VARS( "l/lq hoffset", llq_hoffset, "%12u");
    DISP_LN_VARS( "d/p hoffset", dp_hoffset, "%12u");
    DISP_LN_VARS( "d/l hoffset", dl_hoffset, "%12u");
    BCM_LOG_CLI( "------------------------------------------------------------------------\n");
    DISP_LNQ_VARS("dfe[2][a,b]", dfe[1][0], dfe[1][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[2][c,d]", dfe[1][2], dfe[1][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[3][a,b]", dfe[2][0], dfe[2][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[3][c,d]", dfe[2][2], dfe[2][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[4][a,b]", dfe[3][0], dfe[3][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[4][c,d]", dfe[3][2], dfe[3][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[5][a,b]", dfe[4][0], dfe[4][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[5][c,d]", dfe[4][2], dfe[4][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[6][a,b]", dfe[5][0], dfe[5][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[6][c,d]", dfe[5][2], dfe[5][3], "%8d,%3d");
    BCM_LOG_CLI( "------------------------------------------------------------------------\n");
    DISP_LNQ_VARS("dfe[7][a,b]", dfe[6][0], dfe[6][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[7][c,d]", dfe[6][2], dfe[6][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[8][a,b]", dfe[7][0], dfe[7][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[8][c,d]", dfe[7][2], dfe[7][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[9][a,b]", dfe[8][0], dfe[8][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[9][c,d]", dfe[8][2], dfe[8][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[10][a,b]", dfe[9][0], dfe[9][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[10][c,d]", dfe[9][2], dfe[9][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[11][a,b]", dfe[10][0], dfe[10][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[11][c,d]", dfe[10][2], dfe[10][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[12][a,b]", dfe[11][0], dfe[11][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[12][c,d]", dfe[11][2], dfe[11][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[13][a,b]", dfe[12][0], dfe[12][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[13][c,d]", dfe[12][2], dfe[12][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[14][a,b]", dfe[13][0], dfe[13][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[14][c,d]", dfe[13][2], dfe[13][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[15][a,b]", dfe[14][0], dfe[14][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[15][c,d]", dfe[14][2], dfe[14][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[16][a,b]", dfe[15][0], dfe[15][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[16][c,d]", dfe[15][2], dfe[15][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[17][a,b]", dfe[16][0], dfe[16][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[17][c,d]", dfe[16][2], dfe[16][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[18][a,b]", dfe[17][0], dfe[17][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[18][c,d]", dfe[17][2], dfe[17][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[19][a,b]", dfe[18][0], dfe[18][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[19][c,d]", dfe[18][2], dfe[18][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[20][a,b]", dfe[19][0], dfe[19][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[20][c,d]", dfe[19][2], dfe[19][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[21][a,b]", dfe[20][0], dfe[20][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[21][c,d]", dfe[20][2], dfe[20][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[22][a,b]", dfe[21][0], dfe[21][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[22][c,d]", dfe[21][2], dfe[21][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[23][a,b]", dfe[22][0], dfe[22][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[23][c,d]", dfe[22][2], dfe[22][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[24][a,b]", dfe[23][0], dfe[23][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[24][c,d]", dfe[23][2], dfe[23][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[25][a,b]", dfe[24][0], dfe[24][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[25][c,d]", dfe[24][2], dfe[24][3], "%8d,%3d");
    DISP_LNQ_VARS("dfe[26][a,b]", dfe[25][0], dfe[25][1], "%8d,%3d");
    DISP_LNQ_VARS("dfe[26][c,d]", dfe[25][2], dfe[25][3], "%8d,%3d");
    BCM_LOG_CLI( "------------------------------------------------------------------------\n");
    DISP_LN_VARS("dc_offset", dc_offset, "%12d");
    DISP_LNQ_VARS("data_p[a,b]", thctrl_dp[0], thctrl_dp[1], "%8d,%3d");
    DISP_LNQ_VARS("data_p[c,d]", thctrl_dp[2], thctrl_dp[3], "%8d,%3d");
    DISP_LNQ_VARS("data_n[a,b]", thctrl_dn[0], thctrl_dn[1], "%8d,%3d");
    DISP_LNQ_VARS("data_n[c,d]", thctrl_dn[2], thctrl_dn[3], "%8d,%3d");
    DISP_LNQ_VARS("phase_p[a,b]", thctrl_zp[0], thctrl_zp[1], "%8d,%3d");
    DISP_LNQ_VARS("phase_p[c,d]", thctrl_zp[2], thctrl_zp[3], "%8d,%3d");
    DISP_LNQ_VARS("phase_n[a,b]", thctrl_zn[0], thctrl_zn[1], "%8d,%3d");
    DISP_LNQ_VARS("phase_n[c,d]", thctrl_zn[2], thctrl_zn[3], "%8d,%3d");
    DISP_LNQ_VARS("lms[a,b]", thctrl_l[0], thctrl_l[1], "%8d,%3d");
    DISP_LNQ_VARS("lms[c,d]", thctrl_l[2], thctrl_l[3], "%8d,%3d");
    BCM_LOG_CLI( "------------------------------------------------------------------------\n");
    DISP_LN_VARS("PRBS_CHECKER", prbs_chk_en, "%12u");
    DISP_LN_VARS("PRBS_ORDER", prbs_chk_order, "%12u");
    DISP_LN_VARS("PRBS_LOCK", prbs_chk_lock, "%12u");
    DISP_LN_VARS("PRBS_ERRCNT", prbs_chk_errcnt, "%12u");
    BCM_LOG_CLI( "------------------------------------------------------------------------\n");
    BCM_LOG_CLI( "\n");

    return SOC_E_NONE;
}

int bcm_event_log_stop (phy_ctrl_t *pa)
{
    BCM_LOG_CLI("\n\n********************************************\n");
    BCM_LOG_CLI(    "**** SERDES UC TRACE MEMORY DUMP ***********\n");
    BCM_LOG_CLI(    "********************************************\n");

    /* Start Read to stop uC logging and read the word at last event written by uC */
    EFUN(bcm_pe_pmd_uc_cmd (pa, CMD_EVENT_LOG_READ, CMD_EVENT_LOG_READ_START, 10));

    return SOC_E_NONE;
}

int bcm_event_log_readmem (phy_ctrl_t *pa, uint8 *trace_mem)
{
    uint8 uc_dsc_supp_info;
    uint16 addr = 0, trace_mem_size = 0;
    uint16 rdval;

    /* validate input arguments */
    if (trace_mem == NULL) return SOC_E_PARAM;

    ESTM(rdval = rdcv_trace_mem_rd_idx());
    BCM_LOG_CLI( "\n  DEBUG INFO: trace memory read index = 0x%04x\n", rdval);

    trace_mem_size = 768;
    BCM_LOG_CLI("  DEBUG INFO: trace memory size = 0x%04x\n\n", trace_mem_size);

    do {
        /* Read Next */
        EFUN(bcm_pe_pmd_uc_cmd (pa, CMD_EVENT_LOG_READ, CMD_EVENT_LOG_READ_NEXT, 10));

        if (addr >= trace_mem_size)
            return SOC_E_FAIL;
        else
            addr++;

        ESTM(*(trace_mem++) = (uint8)rd_uc_dsc_data());
        ESTM(uc_dsc_supp_info = rd_uc_dsc_supp_info());
    } while (uc_dsc_supp_info != 1);

    /* Read Done to resume logging  */
    EFUN(bcm_pe_pmd_uc_cmd (pa, CMD_EVENT_LOG_READ, CMD_EVENT_LOG_READ_DONE, 10));

    return SOC_E_NONE;
}

char* _bcm_error_val_2_str (phy_ctrl_t *pa, uint8 val)
{
    switch (val) {
    case INVALID_CORE_TEMP_IDX:
        return "INVALID CORE TEMP INDEX";
    case INVALID_OTP_CONFIG:
        return "INVALID OTP CONFIGURATION";
    case DSC_CONFIG_INVALID_REENTRY_ID:
        return "DSC CONFIG INVALID REENTRY";
    case INVALID_REENTRY_ID:
        return "INVALID REENTRY";
    case GENERIC_UC_ERROR:
        return "GENERIC UC ERROR";
    default:
        return "UNDEFINED";
    }
}

int _bcm_display_event (phy_ctrl_t *pa, uint8 event_id, uint8 entry_len, uint8 prev_cursor, uint8 curr_cursor, uint8 post_cursor, uint8 *supp_info)
{
    char *s1;

    switch (event_id) {                                                   /* decode event code */
    case EVENT_CODE_ENTRY_TO_DSC_RESET:
        BCM_LOG_CLI("  Entry to DSC reset");
        break;
    case EVENT_CODE_RELEASE_USER_RESET:
        BCM_LOG_CLI("  Release user reset");
        break;
    case EVENT_CODE_EXIT_FROM_DSC_RESET:
        BCM_LOG_CLI("  Exit from DSC reset");
        break;
    case EVENT_CODE_ENTRY_TO_CORE_RESET:
        BCM_LOG_CLI("  Entry to core reset");
        break;
    case EVENT_CODE_RELEASE_USER_CORE_RESET:
        BCM_LOG_CLI("  Release user core reset");
        break;
    case EVENT_CODE_ACTIVE_RESTART_CONDITION:
        BCM_LOG_CLI("  Active restart condition");
        break;
    case EVENT_CODE_EXIT_FROM_RESTART:
        BCM_LOG_CLI("  Exit from restart");
        break;
    case EVENT_CODE_WRITE_TR_COARSE_LOCK:
        BCM_LOG_CLI("  Write timing recovery coarse lock");
        break;
    case EVENT_CODE_CL72_READY_FOR_COMMAND:
        BCM_LOG_CLI("  General event - %d", EVENT_CODE_CL72_READY_FOR_COMMAND);
        break;
    case EVENT_CODE_EACH_WRITE_TO_CL72_TX_CHANGE_REQUEST:
        BCM_LOG_CLI("  General event - %d", EVENT_CODE_EACH_WRITE_TO_CL72_TX_CHANGE_REQUEST);
        break;
    case EVENT_CODE_FRAME_LOCK:
        BCM_LOG_CLI("  CL72 Frame locked");
        break;
    case EVENT_CODE_LOCAL_RX_TRAINED:
        BCM_LOG_CLI("  CL72 Local Rx trained");
        break;
    case EVENT_CODE_DSC_LOCK:
        BCM_LOG_CLI("  DSC lock");
        break;
    case EVENT_CODE_FIRST_RX_PMD_LOCK:
        BCM_LOG_CLI("  Rx PMD lock");
        break;
    case EVENT_CODE_PMD_RESTART_FROM_CL72_CMD_INTF_TIMEOUT:
        BCM_LOG_CLI("  PMD restart due to CL72 ready for command timeout");
        break;
    case EVENT_CODE_LP_RX_READY:
        BCM_LOG_CLI("  CL72 Remote receiver ready");
        break;
    case EVENT_CODE_STOP_EVENT_LOG:
        BCM_LOG_CLI("  Start reading event log");
        break;
    case EVENT_CODE_GENERAL_EVENT_0:
        BCM_LOG_CLI("  General event 0, (0x%x%x)", post_cursor, prev_cursor);
        break;
    case EVENT_CODE_GENERAL_EVENT_1:
        BCM_LOG_CLI("  General event 1, (0x%x%x)", post_cursor, prev_cursor);
        break;
    case EVENT_CODE_GENERAL_EVENT_2:
        BCM_LOG_CLI("  General event 2, (0x%x%x)", post_cursor, prev_cursor);
        break;
    case EVENT_CODE_ERROR_EVENT:
        s1 = _bcm_error_val_2_str (pa, post_cursor);
        BCM_LOG_CLI("  UC error event: %s", s1);
        break;
    case EVENT_CODE_NUM_TIMESTAMP_WRAPAROUND_MAXOUT:
        BCM_LOG_CLI("  Reset number of timestamp wraparounds");
        break;
    case EVENT_CODE_RESTART_PMD_ON_CDR_LOCK_LOST:
        BCM_LOG_CLI("  Restart Rx PMD on CDR lock lost");
        break;
    case EVENT_CODE_RESTART_PMD_ON_CLOSE_EYE:
        BCM_LOG_CLI("  Restart Rx PMD because of closed eye");
        break;
    case EVENT_CODE_RESTART_PMD_ON_DFE_TAP_CONFIG:
        BCM_LOG_CLI("  Restart Rx PMD on maxed out DFE tap magnitude");
        break;
    case EVENT_CODE_SM_STATUS_RESTART:
        BCM_LOG_CLI("  Check DSC SM status restart reg value");
        break;
    case EVENT_CODE_CORE_PROGRAMMING:
        BCM_LOG_CLI("  Program core config value");
        break;
    case EVENT_CODE_LANE_PROGRAMMING:
        BCM_LOG_CLI("  Program lane config value");
        break;
    case EVENT_CODE_CL72_AUTO_POLARITY_CHANGE:
        BCM_LOG_CLI("  CL72 Auto Polarity Change");
        break;
    case EVENT_CODE_RESTART_FROM_CL72_MAX_TIMEOUT:
        BCM_LOG_CLI("  Restart Rx PMD due to F-CL72 training timeout");
        break;
    case EVENT_CODE_CL72_LOCAL_TX_CHANGED:
        BCM_LOG_CLI("  General event - %d", EVENT_CODE_CL72_LOCAL_TX_CHANGED);
        break;
    case EVENT_CODE_FIRST_WRITE_TO_CL72_TX_CHANGE_REQUEST:
        BCM_LOG_CLI("  First write to LP Cl72 transmit change request");
        break;
    case EVENT_CODE_FRAME_UNLOCK:
        BCM_LOG_CLI("  General event - %d", EVENT_CODE_FRAME_UNLOCK);
        break;
    case EVENT_CODE_ENTRY_TO_CORE_PLL1_RESET:
        BCM_LOG_CLI("  Entry to core PLL1 reset");
        break;
    case EVENT_CODE_RELEASE_USER_CORE_PLL1_RESET:
        BCM_LOG_CLI("  Release user core PLL1 reset");
        break;
    default:
        BCM_LOG_CLI("  UNRECOGNIZED EVENT CODE (0x%x) !!!", event_id);
        break;
    }

    if (entry_len == 4)
        BCM_LOG_CLI("\n");
    else {
        int ii = 0;
        BCM_LOG_CLI(", SUP_INFO={");
        supp_info += (entry_len - 5);
        for (ii = 0; ii < entry_len - 4; ii++) {
            if (ii != 0)
                BCM_LOG_CLI(", ");
            BCM_LOG_CLI("0x%x", *supp_info);
            supp_info--;
        }
        BCM_LOG_CLI("}\n");
        if ((event_id >= EVENT_CODE_MAX) && (event_id < EVENT_CODE_TIMESTAMP_WRAP_AROUND))
            /* newline for unrecognized event */
            BCM_LOG_CLI("\n");
    }

    return SOC_E_NONE;
}

int bcm_event_log_display (phy_ctrl_t *pa, uint8 *trace_mem, enum srds_event_log_display_mode_enum display_mode)
{
#define MAX_ENTRY_SIZE   (7)
#define NOR_ENTRY_SIZE   (4)
    uint16 trace_mem_size = 0;
    uint8 num_lanes = 0;
    uint8 is_ref_time_found = 0, is_last_event_timewrap = 0;
    uint8 post_cursor = 0, curr_cursor = 0, prev_cursor = 0;
    uint16 addr = 0, rr = 0, prev_rr = 0;
    uint8 cc = 0;
    uint16 time = 0, ref_time = 0, num_time_wraparound = 0, this_num_time_wraparound = 0;
    USR_DOUBLE prev_time = 0, curr_time = 0;
    uint8 word_per_row = 8;
    uint8 prev_event = 0;
    uint8 supp_info[MAX_ENTRY_SIZE - NOR_ENTRY_SIZE];
    char uc_lane_id_str[256];
    uint8 char_0 = '0';

    /* validate input arguments */
    if (trace_mem == NULL || (display_mode >= EVENT_LOG_MODE_MAX))
        return SOC_E_PARAM;

    trace_mem_size = 768;
    num_lanes = 4;

    /* output */
    if (display_mode == EVENT_LOG_HEX || display_mode == EVENT_LOG_HEX_AND_DECODED) {
        /* print the 16-bit words in Hex format to screen, 8 words per row */
        for (rr = 0; rr < trace_mem_size; rr += (2 * word_per_row)) {
            for (cc = 0; cc < 2 * word_per_row; cc += 2)
                BCM_LOG_CLI("  0x%02x%02x", *(trace_mem + rr + cc), *(trace_mem + rr + cc + 1));
            BCM_LOG_CLI("    %d\n", rr);
        }
    }
    if (display_mode == EVENT_LOG_DECODED || display_mode == EVENT_LOG_HEX_AND_DECODED) {
        /* decode for level 1 events */

        /* print a text log of the events going backwards in time, showing time as T-10.340ms, where T is when the Start Read happened. */
        addr = 0;
        while (addr < trace_mem_size - 8) {                                                     /* last 8-byte reserved for trace memory wraparound */

            if (*(trace_mem + addr) == 0x0) {                                               /* reach event log end */
                BCM_LOG_CLI("\n========== End of Event Log ==================\n");
                break;
            }

            if (*(trace_mem + addr) == 0xff) {                                           /* timestamp wraparound event handling */

                this_num_time_wraparound = *(trace_mem + (++addr));
                this_num_time_wraparound = ((this_num_time_wraparound << 8) | *(trace_mem + (++addr)));
                num_time_wraparound += this_num_time_wraparound;

                if (!is_last_event_timewrap) {                                         /* display the rest of previous event info */
                    BCM_LOG_CLI(",");
                    EFUN(_bcm_display_event (pa, prev_event, (uint8)prev_rr, prev_cursor, curr_cursor, post_cursor, supp_info));
                    is_last_event_timewrap = 1;
                }

                BCM_LOG_CLI("\n  %5d timestamp wraparound(s). \n\n", this_num_time_wraparound);
                this_num_time_wraparound = 0;

                addr++;
                continue;
            }else  {
                cc = (trace_mem[addr] & 0x1f);                                         /* lane id */
                if (cc >= num_lanes) {
                    BCM_LOG_CLI("\n\n  Incorrect lane ID. Terminating event log display for current core... \n\n");
                    break;
                }

                rr = (uint16)((trace_mem[addr] >> 5) & 0x7);                         /* determine event entry length */
                if (rr > MAX_ENTRY_SIZE) return SOC_E_FAIL;

                time = *(trace_mem + (++addr));                                         /* timestamp */
                time = ((time << 8) | *(trace_mem + (++addr)));
                if (is_ref_time_found == 0) {                                           /* determine the reference time origin */
                    is_ref_time_found = 1;
                    ref_time = time;
                }else  {
#ifdef SERDES_API_FLOATING_POINT
                    curr_time = (time - ref_time - (num_time_wraparound * 65536)) / 100.0;
#else
                    curr_time = (time - ref_time - (num_time_wraparound * 65536)) / 100;
#endif
                    if (!is_last_event_timewrap) {
#ifdef SERDES_API_FLOATING_POINT
                        BCM_LOG_CLI(" (+%.2f),", prev_time - curr_time);
#else
                        BCM_LOG_CLI(" (+%d),", prev_time - curr_time);
#endif
                        EFUN(_bcm_display_event (pa, prev_event, (uint8)prev_rr, prev_cursor, curr_cursor, post_cursor, supp_info));
                    }else
                        is_last_event_timewrap = 0;
                }

                if (cc < 10) {
                    uc_lane_id_str[0] = char_0 + cc;
                    uc_lane_id_str[1] = '\0';
                } else if (cc < 100) {
                    uc_lane_id_str[0] = char_0 + (cc / 10);
                    uc_lane_id_str[1] = char_0 + (cc % 10);
                    uc_lane_id_str[2] = '\0';
                } else {
                    uc_lane_id_str[0] = char_0 + (cc / 100);
                    uc_lane_id_str[1] = char_0 + ((cc % 100) - (cc % 10)) / 10;
                    uc_lane_id_str[2] = char_0 + (cc % 10);
                    uc_lane_id_str[3] = '\0';
                }

                BCM_LOG_CLI("  Lane %s: ", uc_lane_id_str);
#ifdef SERDES_API_FLOATING_POINT
                BCM_LOG_CLI("  t= %.2f ms", curr_time);
#else
                BCM_LOG_CLI("  t= %d ms", curr_time);
#endif
                prev_time = curr_time;
                prev_rr = rr;

                prev_event = *(trace_mem + (++addr));
                switch (prev_event) {
                case EVENT_CODE_CL72_READY_FOR_COMMAND:
                case EVENT_CODE_EACH_WRITE_TO_CL72_TX_CHANGE_REQUEST:
                case EVENT_CODE_CL72_LOCAL_TX_CHANGED:
                    if (rr != NOR_ENTRY_SIZE) {
                        post_cursor = (*(trace_mem + (++addr)) & 0x30) >> 4;
                        curr_cursor = (*(trace_mem + addr) & 0x0C) >> 2;
                        prev_cursor = *(trace_mem + addr) & 0x03;
                        addr--;                                 /* rewind to populate supplement info */
                    }
                    break;
                case EVENT_CODE_GENERAL_EVENT_0:
                case EVENT_CODE_GENERAL_EVENT_1:
                case EVENT_CODE_GENERAL_EVENT_2:
                    post_cursor = *(trace_mem + (++addr));
                    prev_cursor = *(trace_mem + (++addr));
                    addr -= 2;                                     /* rewind to populate supplement info */
                    break;
                case EVENT_CODE_ERROR_EVENT:
                    post_cursor = *(trace_mem + (++addr));
                    addr--;
                    break;
                case EVENT_CODE_SM_STATUS_RESTART:
                    post_cursor = *(trace_mem + (++addr));
                    addr--;
                    break;
                default:
                    break;
                }

                /* retrieve supplement info, if any */
                for (cc = 0; cc < rr - NOR_ENTRY_SIZE; cc++)
                    supp_info[cc] = *(trace_mem + (++addr));

                addr++;
            }
        }
    }
    return SOC_E_NONE;
}

int bcm_read_event_log (phy_ctrl_t *pa, uint8 *trace_mem, enum srds_event_log_display_mode_enum display_mode)
{
    /* validate input arguments */
    if (trace_mem == NULL || (display_mode >= EVENT_LOG_MODE_MAX))
        return SOC_E_PARAM;

    /* stop writing event log */
    EFUN(bcm_event_log_stop (pa ));

    EFUN(bcm_event_log_readmem (pa, trace_mem));

    EFUN(bcm_event_log_display (pa, trace_mem, display_mode));

    return SOC_E_NONE;
}

int bcm_do_dscdump(phy_ctrl_t *pc, int sys_port)
{
    int j;
    uint8 trace_mem[768];
    struct bcm_detailed_lane_status_st lane_st[4];

    if (!pc->module)
        pc->lane_mask = bcm_port_lane_mask(pc->unit, pc->module, sys_port);

    BCM_LOG_CLI("DSC Dump %s port: %d; lane_mask: 0x%x\n", sys_port?"System":"Line", pc->phy_id, pc->lane_mask);

    pc->tsc_sys_port=sys_port;

    SOC_IF_ERROR_RETURN(bcm_display_core_state_hdr (pc));
    SOC_IF_ERROR_RETURN(bcm_display_core_state_line (pc ));
    SOC_IF_ERROR_RETURN(bcm_display_core_state_legend (pc));
    SOC_IF_ERROR_RETURN(bcm_display_lane_state_hdr(pc));
    SOC_IF_ERROR_RETURN(bcm_display_lane_state(pc));

    for (j = 0; j < 4; j++) {
      SOC_IF_ERROR_RETURN
         (bcm_log_full_pmd_state(pc, &lane_st[j]));
    }
    SOC_IF_ERROR_RETURN
       (bcm_disp_full_pmd_state(pc, lane_st, 4));

    SOC_IF_ERROR_RETURN
       (bcm_read_event_log(pc, trace_mem, 2));

    return SOC_E_NONE;
}


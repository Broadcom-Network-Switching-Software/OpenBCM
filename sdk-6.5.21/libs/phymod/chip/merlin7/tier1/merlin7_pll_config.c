/***********************************************************************************
 *                                                                                 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/**********************************************************************************
 **********************************************************************************
 *  File Name     :  merlin7_pll_config.c                                         *
 *  Created On    :  14/07/2013                                                   *
 *  Created By    :  Kiran Divakar                                                *
 *  Description   :  Merlin7 PLL Configuration API                                *
 *  Revision      :    *
 */

/** @file merlin7_pll_config.c
 * Merlin7 PLL Configuration
 */


#include "merlin7_tsc_config.h"
#include "merlin7_tsc_functions.h"
#include "merlin7_tsc_internal.h"
#include "merlin7_tsc_internal_error.h"
#include "merlin7_tsc_select_defns.h"

#define _ndiv_frac_l(x) ((x)&0xF)
#define _ndiv_frac_h(x) ((x)>>4)

#define _ndiv_frac_decode(l_, h_) (((l_) & 0xF) | (((h_) & 0x3FFF) << 4))

static err_code_t _merlin7_tsc_pll_powerdown(srds_access_t *sa__, uint8_t pwrdn_en);
static err_code_t _merlin7_tsc_restore_pll_defaults(srds_access_t *sa__);

static const uint8_t pll_fraction_width = 18;

static err_code_t _merlin7_tsc_pll_powerdown(srds_access_t *sa__, uint8_t pwrdn_en) {
    INIT_SRDS_ERR_CODE
    EFUN(wrc_ams_pll_pwrdn(pwrdn_en));
    return (ERR_CODE_NONE);
}

static err_code_t _merlin7_tsc_restore_pll_defaults(srds_access_t *sa__) {
    INIT_SRDS_ERR_CODE
    EFUN(wrc_ams_pll_i_ndiv_int(0x42));
    EFUN(wrc_ams_pll_i_ndiv_frac_h(_ndiv_frac_h(0x0)));
    EFUN(wrc_ams_pll_i_ndiv_frac_l(_ndiv_frac_l(0x0)));
    EFUN(wrc_ams_pll_i_pll_frac_mode(0x2));
    EFUN(wrc_ams_pll_sel_fp3cap(0x0));
    EFUN(wrc_ams_pll_i_pfd_offset(0x0));
    EFUN(wrc_ams_pll_i_ndiv_dither_en(0x0));
    EFUN(wrc_ams_pll_en_8p5g(0x0));
    EFUN(wrc_ams_pll_en_8p5g_vco(0x0));
    EFUN(wrc_ams_pll_en_hcur_vco(0x0));
    EFUN(wrc_ams_pll_refclk_in_bias(0x15));
    EFUN(wrc_ams_pll_vco_hkvco(0x0));
    EFUN(wrc_ams_pll_refclk_div2_frc_val(0x0));
    EFUN(wrc_ams_pll_refclk_div4_frc_val(0x0));
    EFUN(wrc_ams_pll_refclk_div_frc(0x0));
    EFUN(wrc_ams_pll_curr_sel(0x7));
    EFUN(wrc_ams_pll_rpar(0x2));
    EFUN(wrc_ams_pll_mmd_resetb(0x1));
    return (ERR_CODE_NONE);
}

err_code_t merlin7_tsc_INTERNAL_configure_pll(srds_access_t *sa__,
                                         enum merlin7_tsc_pll_refclk_enum refclk,
                                         enum merlin7_tsc_pll_div_enum srds_div,
                                         uint32_t vco_freq_khz,
                                         enum merlin7_tsc_pll_option_enum pll_option) {
    INIT_SRDS_ERR_CODE
    uint32_t refclk_freq_hz=0;
    enum merlin7_tsc_pll_option_enum pll_opt;
    
    pll_opt = (enum merlin7_tsc_pll_option_enum)(pll_option & MERLIN7_TSC_PLL_OPTION_REFCLK_MASK);
    if (pll_opt == MERLIN7_TSC_PLL_OPTION_POWERDOWN) {
        UNUSED(refclk_freq_hz);
        EFUN(_merlin7_tsc_pll_powerdown(sa__, 0x1));
        return (ERR_CODE_NONE);
    }

    EFUN(merlin7_tsc_INTERNAL_resolve_pll_parameters(sa__, refclk, &refclk_freq_hz, &srds_div, &vco_freq_khz, pll_option));

    /* Use this to restore defaults if reprogramming the PLL under dp-reset (typically Auto-Neg FW) */
    EFUN(_merlin7_tsc_restore_pll_defaults(sa__));

    {
        uint8_t reset_state;
        /* Use core_s_rstb to re-initialize all registers to default before calling this function. */
        ESTM(reset_state = rdc_core_dp_reset_state());

        if(reset_state < 7) {
            EFUN_PRINTF(("ERROR: merlin7_tsc_configure_pll(..) called without core_dp_s_rstb=0\n"));
            return (merlin7_tsc_error(sa__, ERR_CODE_CORE_DP_NOT_RESET));
        }
    }

    /* Clear PLL powerdown */
    EFUN(_merlin7_tsc_pll_powerdown(sa__, 0x0));

    EFUN(wrc_ams_pll_i_ndiv_int(SRDS_INTERNAL_GET_PLL_DIV_INTEGER(srds_div)));
    {
        const uint32_t pll_fraction_num = SRDS_INTERNAL_GET_PLL_DIV_FRACTION_NUM(srds_div, pll_fraction_width);
        const uint8_t  frac_mode_en = (pll_fraction_num != 0);
        EFUN(wrc_ams_pll_i_ndiv_frac_h   ((uint16_t)(_ndiv_frac_h(pll_fraction_num))));
        EFUN(wrc_ams_pll_i_ndiv_frac_l   (_ndiv_frac_l(pll_fraction_num)));
        EFUN(wrc_ams_pll_sel_fp3cap      ((frac_mode_en) ? 0xF : 0x0));
        EFUN(wrc_ams_pll_i_pfd_offset    ((frac_mode_en) ? 0x2 : 0x0));
        EFUN(wrc_ams_pll_i_ndiv_dither_en((frac_mode_en && (refclk != MERLIN7_TSC_PLL_REFCLK_50MHZ)) ? 0x1 : 0x0));
        EFUN(wrc_ams_pll_en_8p5g         ((vco_freq_khz <= 9375000) ? 0x1 : 0x0)); /* pll_ctrl<36> */
        EFUN(wrc_ams_pll_en_8p5g_vco     ((vco_freq_khz <= 9375000) ? 0x1 : 0x0)); /* pll_ctrl<21> */
        EFUN(wrc_ams_pll_en_hcur_vco     (0x1));  /* pll_ctrl<31> */
        EFUN(wrc_ams_pll_refclk_in_bias  (0x3F)); /* pll_ctrl<159:154> */
        EFUN(wrc_ams_pll_vco_hkvco       ((vco_freq_khz <= 9375000) ? 0x1 : 0x0)); /* pll_ctrl<71>, now called VCO_HKVCO */

        /* Handle refclk PLL options */
        if (pll_opt == MERLIN7_TSC_PLL_OPTION_REFCLK_DIV2_EN) {
            EFUN(wrc_ams_pll_refclk_div2_frc_val(1));
            EFUN(wrc_ams_pll_refclk_div_frc(1));
        } else if (pll_opt == MERLIN7_TSC_PLL_OPTION_REFCLK_DIV4_EN) {
            EFUN(wrc_ams_pll_refclk_div4_frc_val(1));
            EFUN(wrc_ams_pll_refclk_div_frc(1));
        }

        /* pll_ctrl<19:16> */
        if (refclk == MERLIN7_TSC_PLL_REFCLK_161P1328125MHZ)
            if (vco_freq_khz < 9375000)         EFUN(wrc_ams_pll_curr_sel(0x3));
            else                                EFUN(wrc_ams_pll_curr_sel(0x5));
        else if (frac_mode_en)                  EFUN(wrc_ams_pll_curr_sel(0x3));
        else                                    EFUN(wrc_ams_pll_curr_sel(0xD));
                
        

        /* pll_ctrl<11:9> */
        if (refclk == MERLIN7_TSC_PLL_REFCLK_161P1328125MHZ)
            if (vco_freq_khz < 9375000)         EFUN(wrc_ams_pll_rpar(0x2));
            else                                EFUN(wrc_ams_pll_rpar(0x3));
        else if (refclk == MERLIN7_TSC_PLL_REFCLK_156P25MHZ)
            if (vco_freq_khz > 12000000)        EFUN(wrc_ams_pll_rpar(0x5));
            else if (frac_mode_en)              EFUN(wrc_ams_pll_rpar(0x2));
            else                                EFUN(wrc_ams_pll_rpar(0x4));
        else if (refclk == MERLIN7_TSC_PLL_REFCLK_125MHZ)
            if (frac_mode_en)                   EFUN(wrc_ams_pll_rpar(0x2));
            else if (vco_freq_khz < 9375000)    EFUN(wrc_ams_pll_rpar(0x3));
            else                                EFUN(wrc_ams_pll_rpar(0x4));
        else if (refclk == MERLIN7_TSC_PLL_REFCLK_50MHZ)
            if (frac_mode_en)                   EFUN(wrc_ams_pll_rpar(0x1));
            else if (vco_freq_khz < 8500000)    EFUN(wrc_ams_pll_rpar(0x1));
            else if (vco_freq_khz < 9375000)    EFUN(wrc_ams_pll_rpar(0x2));
            else                                EFUN(wrc_ams_pll_rpar(0x3));
#ifdef MERLIN7_SHORTFIN
        else if (refclk == MERLIN7_TSC_PLL_REFCLK_400MHZ)
            if (frac_mode_en)                   EFUN(wrc_ams_pll_rpar(0x0));
            else                                EFUN(wrc_ams_pll_rpar(0x5));
#endif
        else                                    EFUN(wrc_ams_pll_rpar(0x2));

    }

    EFUN(wrc_ams_pll_i_pll_frac_mode(0x2));

    /* Toggling PLL mmd reset */
    EFUN(wrc_ams_pll_mmd_resetb(0x0));
    EFUN(wrc_ams_pll_mmd_resetb(0x1));

    /* NOTE: Might have to add some optimized PLL control bus settings post-DVT (See 28nm merlin_pll_config.c) */

    /* Update core variables with the VCO rate. */
    {
        struct merlin7_tsc_uc_core_config_st core_config;
        EFUN(merlin7_tsc_get_uc_core_config(sa__, &core_config));
        core_config.vco_rate_in_Mhz = (int32_t)((vco_freq_khz + 500) / 1000);
        core_config.field.vco_rate = MHZ_TO_VCO_RATE(core_config.vco_rate_in_Mhz);
        EFUN(merlin7_tsc_INTERNAL_set_uc_core_config(sa__, core_config));
    }

    return (ERR_CODE_NONE);

} /* merlin7_tsc_configure_pll */

err_code_t merlin7_tsc_INTERNAL_read_pll_div(srds_access_t *sa__, uint32_t *srds_div) {
    INIT_SRDS_ERR_CODE
    uint16_t ndiv_int;
    uint32_t ndiv_frac;
    ESTM(ndiv_int = rdc_ams_pll_i_ndiv_int());
    ESTM(ndiv_frac = (uint32_t)(_ndiv_frac_decode(rdc_ams_pll_i_ndiv_frac_l(), rdc_ams_pll_i_ndiv_frac_h())));
    *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV(ndiv_int, ndiv_frac, pll_fraction_width);
    return (ERR_CODE_NONE);
}


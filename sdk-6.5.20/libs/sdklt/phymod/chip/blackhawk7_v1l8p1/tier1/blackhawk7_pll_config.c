/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/**********************************************************************************
 **********************************************************************************
 *  File Name     :  blackhawk7_pll_config.c                                      *
 *  Created On    :  24 Apr 2018                                                  *
 *  Created By    :  Kiran Divakar                                                *
 *  Description   :  Blackhawk7 PLL Configuration API                             *
 */


/** @file blackhawk7_pll_config.c
 * Blackhawk7 PLL Configuration
 */


#include "blackhawk7_v1l8p1_config.h"
#include "blackhawk7_v1l8p1_common.h"
#include "blackhawk7_v1l8p1_functions.h"
#include "blackhawk7_v1l8p1_internal.h"
#include "blackhawk7_v1l8p1_internal_error.h"
#include "blackhawk7_v1l8p1_select_defns.h"

#define _ndiv_frac_l(x) ((x)&0xFFFF)
#define _ndiv_frac_h(x) ((x)>>16)

#define _ndiv_frac_decode(l_, h_) (((l_) & 0xFFFF) | (((h_) & 0x3) << 16))

extern err_code_t blackhawk7_v1l8p1_core_pwrdn(srds_access_t *sa__, enum srds_core_pwrdn_mode_enum mode);

static err_code_t _blackhawk7_v1l8p1_restore_pll_defaults(srds_access_t *sa__);

/* The pll_fracn_ndiv_int and pll_fracn_frac bitfields have this many bits. */
static const uint32_t pll_fracn_ndiv_int_bits = 10;
static const uint32_t pll_fracn_frac_bits     = 18;

static err_code_t _blackhawk7_v1l8p1_restore_pll_defaults(srds_access_t *sa__) {
    INIT_SRDS_ERR_CODE
    EFUN(wrc_ams_pll_fracn_ndiv_int(0xB4));
    EFUN(wrc_ams_pll_fracn_ndiv_frac_l(0x0));
    EFUN(wrc_ams_pll_fracn_ndiv_frac_h(0x0));
    EFUN(wrc_ams_pll_refclk_freq2x_en(0x0));
    EFUN(wrc_ams_pll_refdiv2(0x0));
    EFUN(wrc_ams_pll_refdiv4(0x0));
    EFUN(wrc_ams_pll_div4_2_sel(0x0));
    EFUN(wrc_ams_pll_pll_frac_mode(0x1));
    EFUN(wrc_ams_pll_resetb_mmd(0x1));
    EFUN(wrc_ams_pll_icp_sel(0xC));
    EFUN(wrc_ams_pll_rz_sel(0x4));
    EFUN(wrc_ams_pll_i_en_kvh(0x0));
    EFUN(wrc_ams_pll_cp_sel(0x0));
    EFUN(wrc_ams_pll_fp3_c_sel(0x0));
    EFUN(wrc_ams_pll_rtl_div_en(0x0));
    EFUN(wrc_ams_pll_rz_extra(0x0));
    EFUN(wrc_ams_pll_ptatadj(0xF));
    EFUN(wrc_ams_pll_ctatadj(0xF));
    EFUN(wrc_ams_pll_fp3_r_sel(0x3));
    EFUN(wrc_ams_pll_en_offset_refclk(0x0));
    EFUN(wrc_ams_pll_i_en_ovrrad(0x0));
    EFUN(wrc_ams_pll_i_rband_ovrrd(0x0));
    EFUN(wrc_ams_pll_i_en_extra(0x0));
    return (ERR_CODE_NONE);
}

err_code_t blackhawk7_v1l8p1_INTERNAL_configure_pll(srds_access_t *sa__,
                                         enum blackhawk7_v1l8p1_pll_refclk_enum refclk,
                                         enum blackhawk7_v1l8p1_pll_div_enum srds_div,
                                         uint32_t vco_freq_khz,
                                         enum blackhawk7_v1l8p1_pll_option_enum pll_option) {
    INIT_SRDS_ERR_CODE
    uint32_t refclk_freq_hz=0;
    enum blackhawk7_v1l8p1_pll_option_enum pll_opt;

    pll_opt = (enum blackhawk7_v1l8p1_pll_option_enum)(pll_option & BLACKHAWK7_V1L8P1_PLL_OPTION_REFCLK_MASK);
    if (pll_opt == BLACKHAWK7_V1L8P1_PLL_OPTION_POWERDOWN) {
        UNUSED(refclk_freq_hz);
        EFUN(blackhawk7_v1l8p1_core_pwrdn(sa__, PWRDN));
        return (ERR_CODE_NONE);
    }

#ifdef SMALL_FOOTPRINT
    EFUN(blackhawk7_v1l8p1_INTERNAL_get_refclk_in_hz(sa__, refclk, &refclk_freq_hz));
#else
    EFUN(blackhawk7_v1l8p1_INTERNAL_resolve_pll_parameters(sa__, refclk, &refclk_freq_hz, &srds_div, &vco_freq_khz, pll_option));
#endif

    /* Use this to restore defaults if reprogramming the PLL under dp-reset (typically Auto-Neg FW) */
    EFUN(_blackhawk7_v1l8p1_restore_pll_defaults(sa__));

    {
        uint8_t reset_state;
        /* Use core_s_rstb to re-initialize all registers to default before calling this function. */
        ESTM(reset_state = rdc_core_dp_reset_state());

        if(reset_state < 7) {
            EFUN_PRINTF(("ERROR: blackhawk7_v1l8p1_configure_pll(..) called without core_dp_s_rstb=0\n"));
            return (blackhawk7_v1l8p1_error(sa__, ERR_CODE_CORE_DP_NOT_RESET));
        }
    }
    /* Clear PLL powerdown */
    EFUN(blackhawk7_v1l8p1_core_pwrdn(sa__, PWR_ON));
    EFUN(wrc_ams_pll_pwrdn_hbvco(0x1));
    EFUN(wrc_ams_pll_pwrdn_lbvco(0x1));
    /* Use HBVCO for 20.5 GHz and above */
    if (vco_freq_khz >= 20500000) {
        EFUN(wrc_ams_pll_pwrdn_hbvco(0x0));
    }
    else {
        EFUN(wrc_ams_pll_pwrdn_lbvco(0x0));
    }


    {
        /* Get information needed for fractional mode configuration.
         * Start with the div value composed of an integer and a wide fractional value.
         */
        const uint8_t  div_fraction_width = 28; /* Must be less than 32 due to overflow detection below. */
        const uint16_t div_integer        = SRDS_INTERNAL_GET_PLL_DIV_INTEGER(srds_div);
        const uint32_t div_fraction       = SRDS_INTERNAL_GET_PLL_DIV_FRACTION_NUM(srds_div, div_fraction_width);

        /* The div_fraction may have more precision than our pll_fracn_frac bitfield.
         * So round it.  Start by adding 1/2 LSB of the fraction div_fraction.
         */
        const uint32_t div_fraction_0p5 = 1 << (div_fraction_width - pll_fracn_frac_bits - 1);
        const uint32_t div_fraction_plus_0p5 = div_fraction + div_fraction_0p5;

        /* Did div_fraction_plus_p5 have a carry bit? */
        const uint32_t div_fraction_plus_p5_carry = div_fraction_plus_0p5 >> div_fraction_width;

        /* The final rounded div_fraction, including carry up to div_integer.
         * This removes the carry and implements the fixed point truncation.
         */
        const uint16_t pll_fracn_ndiv_int  = (uint16_t)(div_integer + div_fraction_plus_p5_carry);
        const uint32_t pll_fracn_div = ((div_fraction_plus_0p5 & ((1U << div_fraction_width)-1))
                                        >> (div_fraction_width - pll_fracn_frac_bits));

        if (pll_fracn_ndiv_int != (pll_fracn_ndiv_int & ((1 << pll_fracn_ndiv_int_bits)-1))) {
            EFUN_PRINTF(("ERROR:  PLL divide is too large for div value 0x%08X\n", srds_div));
            return (blackhawk7_v1l8p1_error(sa__, ERR_CODE_PLL_DIV_INVALID));
        }

        if ((pll_fracn_ndiv_int < 16) || (pll_fracn_ndiv_int > 510)) {
            return (blackhawk7_v1l8p1_error(sa__, ERR_CODE_INVALID_PLL_CFG));
        }
        else if (pll_fracn_ndiv_int < 60) {
            EFUN(wrc_ams_pll_pll_frac_mode(0x2)); /* MMD 4/5 mode (pll_frac_mode = 2) => [16 <= Ndiv < 60] */
        }
        else {
            EFUN(wrc_ams_pll_pll_frac_mode(0x1)); /* MMD 8/9 mode (pll_frac_mode = 1) => [60 <= Ndiv < 511] */
        }

        EFUN(wrc_ams_pll_fracn_ndiv_int   (pll_fracn_ndiv_int));
        EFUN(wrc_ams_pll_fracn_ndiv_frac_l(_ndiv_frac_l(pll_fracn_div)));
        EFUN(wrc_ams_pll_fracn_ndiv_frac_h((uint8_t)(_ndiv_frac_h(pll_fracn_div))));

        /* Optimized PLL settings (Namik) */
        EFUN(wrc_ams_pll_i_en_kvh(0));
        EFUN(wrc_ams_pll_ptatadj(0x10));
        EFUN(wrc_ams_pll_ctatadj(0xE));
        if (pll_fracn_div == 0) {                /* Integer divider */
            EFUN(wrc_ams_pll_icp_sel(15));
            EFUN(wrc_ams_pll_rz_sel(3));
            EFUN(wrc_ams_pll_rz_extra(3));
            EFUN(wrc_ams_pll_cp_sel(0));
            EFUN(wrc_ams_pll_fp3_c_sel(0));
        }
        else {                                   /* Frac divider */
            EFUN(wrc_ams_pll_icp_sel(9));
            EFUN(wrc_ams_pll_rz_sel(4));
            EFUN(wrc_ams_pll_cp_sel(3));
            EFUN(wrc_ams_pll_fp3_c_sel(15));
            EFUN(wrc_ams_pll_fp3_r_sel(0));
            EFUN(wrc_ams_pll_en_offset_refclk(1));
        }
        EFUN(wrc_ams_pll_i_en_ovrrad(1));
        EFUN(wrc_ams_pll_i_rband_ovrrd(7));
        EFUN(wrc_ams_pll_i_en_extra(7));
        {
            uint8_t rtl_div_en=0, pll;
            uint8_t pll_orig = blackhawk7_v1l8p1_acc_get_pll_idx(sa__);

            if (refclk_freq_hz <= 225000000) {                 /* REFCLK <=225 MHz */
                rtl_div_en = 0;
            }
            else if (refclk_freq_hz <= 450000000) {            /* REFCLK >225 MHz and <=450 MHz */
                rtl_div_en = 2;
            }
            else {                                             /* REFCLK >450 MHz */
                rtl_div_en = 3;
            }
            /* Setting for both PLLs */
            for (pll = 0; pll < NUM_PLLS; ++pll) {
                EFUN(blackhawk7_v1l8p1_acc_set_pll_idx(sa__, pll));
                EFUN(wrc_ams_pll_rtl_div_en(rtl_div_en));
            }
            EFUN(blackhawk7_v1l8p1_acc_set_pll_idx(sa__, pll_orig));
        }

        /* Toggle ndiv_valid high, then low to load in a new value for fracn_div. */
        EFUN(wrc_ams_pll_i_ndiv_valid(1));
        EFUN(wrc_ams_pll_i_ndiv_valid(0));
    }

    /* Handle refclk PLL options */
    if (pll_opt == BLACKHAWK7_V1L8P1_PLL_OPTION_REFCLK_DOUBLER_EN) {
        EFUN(wrc_ams_pll_refclk_freq2x_en(0x1));
    } else if (pll_opt == BLACKHAWK7_V1L8P1_PLL_OPTION_REFCLK_DIV2_EN) {
        EFUN(wrc_ams_pll_refdiv2(1));
        EFUN(wrc_ams_pll_div4_2_sel(1));
    } else if (pll_opt == BLACKHAWK7_V1L8P1_PLL_OPTION_REFCLK_DIV4_EN) {
        EFUN(wrc_ams_pll_refdiv4(1));
        EFUN(wrc_ams_pll_div4_2_sel(1));
    }

    /* NOTE: Might have to add some optimized PLL control settings post-DVT*/

    /* Toggling PLL mmd reset */
    EFUN(wrc_ams_pll_resetb_mmd(0x0));
    EFUN(wrc_ams_pll_resetb_mmd(0x1));

    /* Update core variables with the VCO rate. */
    {
        struct blackhawk7_v1l8p1_uc_core_config_st core_config = UC_CORE_CONFIG_INIT;
        EFUN(blackhawk7_v1l8p1_get_uc_core_config(sa__, &core_config));
        core_config.vco_rate_in_Mhz = (int32_t)((vco_freq_khz + 500) / 1000);
        core_config.field.vco_rate = MHZ_TO_VCO_RATE(core_config.vco_rate_in_Mhz);
        EFUN(blackhawk7_v1l8p1_INTERNAL_set_uc_core_config(sa__, core_config));
    }
    return (ERR_CODE_NONE);
} /* blackhawk7_v1l8p1_configure_pll */

#ifndef SMALL_FOOTPRINT
err_code_t blackhawk7_v1l8p1_INTERNAL_read_pll_div(srds_access_t *sa__, uint32_t *srds_div) {
    INIT_SRDS_ERR_CODE
    uint16_t pll_fracn_ndiv_int;
    uint32_t pll_fracn_div;
    ESTM(pll_fracn_ndiv_int = rdc_ams_pll_fracn_ndiv_int());
    ESTM(pll_fracn_div = (uint32_t)(_ndiv_frac_decode(rdc_ams_pll_fracn_ndiv_frac_l(), rdc_ams_pll_fracn_ndiv_frac_h())));
    *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV(pll_fracn_ndiv_int, pll_fracn_div, pll_fracn_frac_bits);
    return (ERR_CODE_NONE);
}
#endif /* SMALL_FOOTPRINT */

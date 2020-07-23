/***********************************************************************************
 *                                                                                 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/**********************************************************************************
 **********************************************************************************
 *  File Name     :  falcon16_pll_config.c                                        *
 *  Created On    :  23/12/2013                                                   *
 *  Created By    :  Kiran Divakar                                                *
 *  Description   :  Falcon16 PLL Configuration API                               *
 *  Revision      :    *
 */


/** @file falcon16_pll_config.c
 * Falcon16 PLL Configuration
 */

#include "../include/falcon16_tsc_config.h"
#include "../include/falcon16_tsc_functions.h"
#include "../include/falcon16_tsc_internal.h"
#include "../include/falcon16_tsc_internal_error.h"
#include "../include/falcon16_tsc_select_defns.h"


#define VCO_FREQ_KHZ_NEAR(vco_freq_khz_, near_val_) (   ((vco_freq_khz_) >= (near_val_) - 10000) \
                                                     || ((vco_freq_khz_) <= (near_val_) + 10000))

/* The pll_fracn_ndiv_int and pll_fracn_frac bitfields have this many bits. */
static const uint32_t pll_fracn_ndiv_int_bits = 10;
static const uint32_t pll_fracn_frac_bits     = 18;

err_code_t falcon16_tsc_INTERNAL_configure_pll(srds_access_t *sa__,
                                         enum falcon16_tsc_pll_refclk_enum refclk,
                                         enum falcon16_tsc_pll_div_enum srds_div,
                                         uint32_t vco_freq_khz,
                                         enum falcon16_tsc_pll_option_enum pll_option) {
    uint32_t refclk_freq_hz;
    uint8_t int_configured;
    
    EFUN(falcon16_tsc_INTERNAL_resolve_pll_parameters(sa__, refclk, &refclk_freq_hz, &srds_div, &vco_freq_khz, pll_option));


    {
        uint8_t reset_state;
        /* Use core_s_rstb to re-initialize all registers to default before calling this function. */
        ESTM(reset_state = rdc_core_dp_reset_state());

        if(reset_state < 7) {
            EFUN_PRINTF(("ERROR: falcon16_tsc_configure_pll(..) called without core_dp_s_rstb=0\n"));
            return (falcon16_tsc_error(sa__, ERR_CODE_CORE_DP_NOT_RESET));
        }
    }

    /* Clear PLL powerdown */
    EFUN(wrc_ams_pll_pwrdn(0));

    /* Per AB on 13 Nov 2015:  Use VCO2 for 22.6 GHz and below.
     * The next supported frequency is 23.0 GHz, so let's use 22.8 GHz as the threshold.
     */
    EFUN(wrc_ams_pll_vco2_15g((vco_freq_khz < 22800000) ? 1 : 0));

    int_configured = 0;
    if (!SRDS_INTERNAL_IS_PLL_DIV_FRACTIONAL(srds_div)) {
        uint8_t pll_mode;
        int_configured = 1;
        switch (SRDS_INTERNAL_GET_PLL_DIV_INTEGER(srds_div)) {
            case  64: pll_mode =  0; break;
            case  66: pll_mode =  1; break;
            case  80: pll_mode =  2; break;
            case 128: pll_mode =  3; break;
            case 132: pll_mode =  4; break;
            case 140: pll_mode =  5; break;
            case 160: pll_mode =  6; break;
            case 165: pll_mode =  7; break;
            case 168: pll_mode =  8; break;
            case 170: pll_mode =  9; break;
            case 175: pll_mode = 10; break;
            case 180: pll_mode = 11; break;
            case 184: pll_mode = 12; break;
            case 200: pll_mode = 13; break;
            case 224: pll_mode = 14; break;
            case 264: pll_mode = 15; break;
            case  96: pll_mode = 16; break;
            case 120: pll_mode = 17; break;
            case 144: pll_mode = 18; break;
            case 198: pll_mode = 19; break;
            default: int_configured = 0;
        }
    
        if (int_configured) {
            EFUN(wrc_pll_mode(pll_mode));
        }
    }

    EFUN(wrc_ams_pll_fracn_sel(int_configured ? 0 : 1));

    if (!int_configured) {
        /* Either fractional mode was requested, or integer mode doesn't support the divisor.
         * Get information needed for fractional mode configuration.
         *
         * The value programmed into the pll_fracn_* bitfields which must account for the
         * initial div2 stage after the VCO.  For instance, a divide by 147.2 must be
         * programmed with an integer of 73 and a fraction of 0.6.
         *
         * Start with the div value, divided by 2, composed of an integer and a wide fractional value.
         */
        const uint8_t  div_fraction_width = 28; /* Must be less than 32 due to overflow detection below. */
        const uint16_t div_integer        = SRDS_INTERNAL_GET_PLL_DIV_INTEGER(srds_div) >> 1;
        const uint32_t div_fraction       = (((SRDS_INTERNAL_GET_PLL_DIV_INTEGER(srds_div) & 1) << (div_fraction_width-1))
                                             | SRDS_INTERNAL_GET_PLL_DIV_FRACTION_NUM(srds_div, div_fraction_width-1));

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
        const uint16_t pll_fracn_ndiv_int  = div_integer + div_fraction_plus_p5_carry;
        const uint32_t pll_fracn_div = ((div_fraction_plus_0p5 & ((1 << div_fraction_width)-1))
                                        >> (div_fraction_width - pll_fracn_frac_bits));

        if (pll_fracn_ndiv_int != (pll_fracn_ndiv_int & ((1 << pll_fracn_ndiv_int_bits)-1))) {
            EFUN_PRINTF(("ERROR:  PLL divide is too large for div value 0x%08X\n", srds_div));
            return (falcon16_tsc_error(sa__, ERR_CODE_PLL_DIV_INVALID));
        }

        EFUN(wrc_pll_mode(0));
        if ((pll_fracn_ndiv_int < 12) || (pll_fracn_ndiv_int > 251)) {
            return (falcon16_tsc_error(sa__, ERR_CODE_INVALID_PLL_CFG));
        }
        EFUN(wrc_ams_pll_fracn_ndiv_int (pll_fracn_ndiv_int));
        EFUN(wrc_ams_pll_fracn_div      (pll_fracn_div & 0xFFFF));
        EFUN(wrc_ams_pll_fracn_div_17_16(pll_fracn_div >> 16));
        EFUN(wrc_ams_pll_fracn_bypass   (0));
        EFUN(wrc_ams_pll_fracn_divrange ((pll_fracn_ndiv_int < 91) ? 1 : 0));
        if (pll_fracn_div != 0) {
            EFUN(wrc_ams_pll_fp3_rh(0x1));
            EFUN(wrc_ams_pll_fp3_ctrl(0xF));
        }
        /* toggle ndiv_frac_valid high, then low to load in a new value for fracn_div. */
        EFUN(wrc_ams_pll_ndiv_frac_valid(1));
        EFUN(wrc_ams_pll_ndiv_frac_valid(0));
    }

    /* If Refclk = 400MHz, ams_pll_comp_thresh = 0x2 */
    if (refclk_freq_hz > 380000000) {
        EFUN(wrc_ams_pll_comp_thresh(0x2));
    }

    /* Determine KVH and pll2rx_clkbw_val per AMS v1.1 and comments in CRSRDSAPI-137, CRSRDSFW-138, CRSRDSAPI-203 */
    {
        uint8_t kvh_force;
        uint8_t pll2rx_clkbw_val;

        if (vco_freq_khz > 27400000) {
            kvh_force = 0;
        } else if ((vco_freq_khz <= 27400000) && (vco_freq_khz > 18750000)) {
            kvh_force = 1;
        } else {
            kvh_force = 3;
        }

        if (vco_freq_khz > 25781250) {
            pll2rx_clkbw_val = 0;
        } else if ((vco_freq_khz <= 25781250) && (vco_freq_khz > 22600000)) {
            pll2rx_clkbw_val = 1;
        } else {
            pll2rx_clkbw_val = 3;
        }

        /* Per AB on 27 Oct 2015, vco_indicator is always forced to 0. */
#if defined(wrc_ams_pll_vco_indicator)
        EFUN(wrc_ams_pll_vco_indicator(0));
#endif
        EFUN(wrc_ams_pll_pll2rx_clkbw(pll2rx_clkbw_val));
        EFUN(wrc_ams_pll_kvh_force(kvh_force));
        EFUN(wrc_ams_pll_force_kvh_bw(1));

        /* Handle refclk PLL options */
        pll_option = (enum falcon16_tsc_pll_option_enum)(pll_option & FALCON16_TSC_PLL_OPTION_REFCLK_MASK);
        if (pll_option == FALCON16_TSC_PLL_OPTION_REFCLK_DOUBLER_EN) {
            EFUN(wrc_ams_pll_refclk_doubler(0x1));
            EFUN(wrc_ams_pll_doubler_res(0x6));
            EFUN(wrc_ams_pll_doubler_cap(0x3));
        } else if (pll_option == FALCON16_TSC_PLL_OPTION_REFCLK_DIV2_EN) {
            EFUN(wrc_ams_pll_div2(1));
            EFUN(wrc_ams_pll_div4_2_sel(1));
        } else if (pll_option == FALCON16_TSC_PLL_OPTION_REFCLK_DIV4_EN) {
            EFUN(wrc_ams_pll_div4(1));
            EFUN(wrc_ams_pll_div4_2_sel(1));
        }

        /* Force refclk_divcnt for refclk faster than 156.25 Mhz. See CRSRDSAPI-185. */
        if (refclk_freq_hz > 156250000) {
            uint16_t refclk_divcnt = 39 * (refclk_freq_hz/156250000);
            EFUN(wrc_refclk_divcnt(refclk_divcnt));
        }

        /* Force vco_step_time. See CRSRDSAPI-229. */
        EFUN(wrc_vco_step_time(8));
    }

    /* Update core variables with the VCO rate. */
    {
        struct falcon16_tsc_uc_core_config_st core_config;
        EFUN(falcon16_tsc_get_uc_core_config(sa__, &core_config));
        core_config.vco_rate_in_Mhz = (vco_freq_khz + 500) / 1000;
        core_config.field.vco_rate = MHZ_TO_VCO_RATE(core_config.vco_rate_in_Mhz);
        EFUN(falcon16_tsc_INTERNAL_set_uc_core_config(sa__, core_config));
    }

    return (ERR_CODE_NONE);
} /* falcon16_tsc_configure_pll */

err_code_t falcon16_tsc_INTERNAL_read_pll_div(srds_access_t *sa__, uint32_t *srds_div) {
    uint8_t fracn_sel;
    ESTM(fracn_sel = rdc_ams_pll_fracn_sel());

    if (fracn_sel) {
        uint16_t pll_fracn_ndiv_int;
        uint32_t pll_fracn_div;
        ESTM(pll_fracn_ndiv_int = rdc_ams_pll_fracn_ndiv_int());
        ESTM(pll_fracn_div = rdc_ams_pll_fracn_div() | (rdc_ams_pll_fracn_div_17_16() << 16));

        /* The value programmed into the pll_fracn_* bitfields which must
         * account for the initial div2 stage after the VCO.  For instance, a
         * divide by 147.2 must be programmed with an integer of 73 and a
         * fraction of 0.6.
         *
         * So multiply the bitfield reads by 2.
         */
        pll_fracn_ndiv_int <<= 1;
        pll_fracn_div      <<= 1;
        
        {
            /* If the post-multiplied fractional part overflows, then apply the carry to
             * the integer part.
             */
            const uint32_t pll_fracn_div_masked = pll_fracn_div & ((1 << pll_fracn_frac_bits)-1);
            if (pll_fracn_div_masked != pll_fracn_div) {
                ++pll_fracn_ndiv_int;
                pll_fracn_div = pll_fracn_div_masked;
            }
        }
    
        *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV(pll_fracn_ndiv_int, pll_fracn_div, pll_fracn_frac_bits);
    } else {
        uint8_t pll_mode;
        ESTM(pll_mode = rdc_pll_mode());
        switch (pll_mode) {
            case  0: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV(  64, 0, 18); break;
            case  1: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV(  66, 0, 18); break;
            case  2: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV(  80, 0, 18); break;
            case  3: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 128, 0, 18); break;
            case  4: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 132, 0, 18); break;
            case  5: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 140, 0, 18); break;
            case  6: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 160, 0, 18); break;
            case  7: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 165, 0, 18); break;
            case  8: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 168, 0, 18); break;
            case  9: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 170, 0, 18); break;
            case 10: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 175, 0, 18); break;
            case 11: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 180, 0, 18); break;
            case 12: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 184, 0, 18); break;
            case 13: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 200, 0, 18); break;
            case 14: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 224, 0, 18); break;
            case 15: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 264, 0, 18); break;
            case 16: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV(  96, 0, 18); break;
            case 17: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 120, 0, 18); break;
            case 18: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 144, 0, 18); break;
            case 19: *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV( 198, 0, 18); break;
            default:
                EFUN_PRINTF(("ERROR: falcon16_tsc_INTERNAL_read_pll_div() found invalid pll_mode value:  %d\n", pll_mode));
                return (falcon16_tsc_error(sa__, ERR_CODE_PLL_DIV_INVALID));
        }
    }
    return (ERR_CODE_NONE);
}


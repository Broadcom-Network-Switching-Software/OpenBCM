/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2021 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 *                                                                                 *
 ***********************************************************************************/

/**********************************************************************************
 **********************************************************************************
 *  File Name     :  osprey7_pll_config.c                                         *
 *  Created On    :  24 Apr 2018                                                  *
 *  Created By    :  Kiran Divakar                                                *
 *  Description   :  Osprey7 PLL Configuration APIs                               *
 *                                                                                *
 **********************************************************************************
 **********************************************************************************/


/** @file osprey7_pll_config.c
 * Osprey7 PLL Configuration APIs
 */


#include "osprey7_v2l8p2_config.h"
#include "osprey7_v2l8p2_functions.h"
#include "osprey7_v2l8p2_internal.h"
#include "osprey7_v2l8p2_internal_error.h"
#include "osprey7_v2l8p2_select_defns.h"

#define _ndiv_frac_l(x) ((x)&0xFFFF)
#define _ndiv_frac_h(x) ((x)>>16)

#define VCO_FREQ_50P0G_KHZ     (50000000)
#define VCO_FREQ_51P72G_KHZ    (51720000)
#define VCO_FREQ_53P125G_KHZ   (53125000)
#define VCO_FREQ_53P2G_KHZ     (53200000)
#define LBVCO_USL              (126)
#define HBVCO_LSL              (1)

#define _ndiv_frac_decode(l_, h_) (((l_) & 0xFFFF) | (((h_) & 0x3) << 16))

static err_code_t _osprey7_v2l8p2_restore_pll_defaults(srds_access_t *sa__);

/* The pll_fracn_ndiv_int and pll_fracn_frac bitfields have this many bits. */
static const uint32_t pll_fracn_ndiv_int_bits = 10;
static const uint32_t pll_fracn_frac_bits     = 18;

static err_code_t _osprey7_v2l8p2_restore_pll_defaults(srds_access_t *sa__) {
    INIT_SRDS_ERR_CODE
    EFUN(wrc_ams_pll_fracn_ndiv_int(0x3B));
    EFUN(wrc_ams_pll_fracn_ndiv_frac_l(0x0));
    EFUN(wrc_ams_pll_fracn_ndiv_frac_h(0x0));
    EFUN(wrc_ams_pll_refclk_freq2x_en(0x0));
    EFUN(wrc_ams_pll_refdiv2(0x0));
    EFUN(wrc_ams_pll_refdiv4(0x0));
    EFUN(wrc_ams_pll_div4_2_sel(0x0));
    EFUN(wrc_ams_pll_pll_frac_mode(0x1));
    EFUN(wrc_ams_pll_resetb_mmd(0x1));
    EFUN(wrc_ams_pll_en_lb_res(0x0));
    EFUN(wrc_ams_pll_en_lb_ind(0x0));
    EFUN(wrc_ams_pll_lb_sel_pll0(0x0));
    EFUN(wrc_ams_pll_rz_sel(0x4));
    EFUN(wrc_ams_pll_cp_calib_adj(0x7));
    EFUN(wrc_ams_pll_cap_l_pll0(0x0));
    EFUN(wrc_ams_pll_cap_r_pll0(0x0));
    EFUN(wrc_ams_pll_ictrl_pll0(0x2));
    EFUN(wrc_ams_pll_ictrl_rpt_pll0(0x2));
    EFUN(wrc_ams_pll_fracn_enable(0x0));
    EFUN(wrc_ams_pll_icp_sel(0xC));
    EFUN(wrc_ams_pll_cp_sel(0x0));
    EFUN(wrc_ams_pll_fp3_r_sel(0x3));
    EFUN(wrc_ams_pll_fp3_c_sel(0x0));
    EFUN(wrc_ams_pll_en_offset_fbck(0x0));
    EFUN(wrc_ams_pll_en_offset_refclk(0x0));
    EFUN(wrc_ams_pll_fracn_offset_ctrl(0x0));
    EFUN(wrc_ams_pll_pfd_pulse_adj(0x0));
    return (ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_INTERNAL_configure_pll(srds_access_t *sa__,
                                         enum osprey7_v2l8p2_pll_refclk_enum refclk,
                                         enum osprey7_v2l8p2_pll_div_enum srds_div,
                                         uint32_t vco_freq_khz,
                                         enum osprey7_v2l8p2_pll_option_enum pll_option) {
    INIT_SRDS_ERR_CODE
    uint32_t refclk_freq_hz=0;
    uint32_t pll_vco_cutoff_in_khz=VCO_FREQ_50P0G_KHZ;
    srds_info_t * const osprey7_v2l8p2_info_ptr = osprey7_v2l8p2_INTERNAL_get_osprey7_v2l8p2_info_ptr_with_check(sa__);
    uint8_t ams_version = 0;

    ESTM(ams_version = rd_ams_tx_version_id());
    if(ams_version == 0xa0) {
        pll_vco_cutoff_in_khz=VCO_FREQ_51P72G_KHZ;
    }
    else if((ams_version & 0xf0) == 0xc0) {
        pll_vco_cutoff_in_khz=VCO_FREQ_53P2G_KHZ;
    }
    /* else keep the default 50G cutoff frequency for A1, B, and D AFE versions  */

    if (pll_option & OSPREY7_V2L8P2_PLL_OPTION_POWERDOWN) {
        UNUSED(refclk_freq_hz);
        EFUN(wrc_ams_pll_pwrdn(0x1));
        return (ERR_CODE_NONE);
    }

    EFUN(osprey7_v2l8p2_INTERNAL_match_ucode_from_info(sa__, osprey7_v2l8p2_info_ptr));
    if ((osprey7_v2l8p2_info_ptr->ucode_version & 0xFFFFF) < 0x00201) {
        uint8_t core_ctrl = 0;
        ESTM(core_ctrl = rdcv_usr_int_core_ctrl_byte() | 0x1);
        EFUN(wrcv_usr_int_core_ctrl_byte(core_ctrl));
    }
#ifdef SMALL_FOOTPRINT
    EFUN(osprey7_v2l8p2_INTERNAL_get_refclk_in_hz(sa__, refclk, &refclk_freq_hz));
#else
    EFUN(osprey7_v2l8p2_INTERNAL_resolve_pll_parameters(sa__, refclk, &refclk_freq_hz, &srds_div, &vco_freq_khz, pll_option));
#endif

    /* Use this to restore defaults if reprogramming the PLL under dp-reset (typically Auto-Neg FW) */
    EFUN(_osprey7_v2l8p2_restore_pll_defaults(sa__));

    {
        uint8_t reset_state;
        /* Use core_s_rstb to re-initialize all registers to default before calling this function. */
        ESTM(reset_state = rdc_core_dp_reset_state());

        if(reset_state < 7) {
            EFUN_PRINTF(("ERROR: osprey7_v2l8p2_configure_pll(..) called without core_dp_s_rstb=0\n"));
            return (osprey7_v2l8p2_error(sa__, ERR_CODE_CORE_DP_NOT_RESET));
        }
    }

    pll_option = (enum osprey7_v2l8p2_pll_option_enum)(pll_option & OSPREY7_V2L8P2_PLL_OPTION_REFCLK_MASK);

    /* Handle refclk PLL options */
    if (pll_option == OSPREY7_V2L8P2_PLL_OPTION_REFCLK_DOUBLER_EN) {
        EFUN(wrc_ams_pll_refclk_freq2x_en(0x1));
        refclk_freq_hz <<= 1;
    } else if (pll_option == OSPREY7_V2L8P2_PLL_OPTION_REFCLK_DIV2_EN) {
        EFUN(wrc_ams_pll_refdiv2(1));
        EFUN(wrc_ams_pll_div4_2_sel(1));
        refclk_freq_hz >>= 1;
    } else if (pll_option == OSPREY7_V2L8P2_PLL_OPTION_REFCLK_DIV4_EN) {
        EFUN(wrc_ams_pll_refdiv4(1));
        EFUN(wrc_ams_pll_div4_2_sel(1));
        refclk_freq_hz >>= 2;
    }

    /* Clear PLL powerdown */
    EFUN(wrc_ams_pll_pwrdn(0x0));

    /* Use HBVCO for frequencies above pll_vco_cutoff_in_khz */
    EFUN(wrc_ams_pll_pwrdn_hbvco(0x1));
    EFUN(wrc_ams_pll_pwrdn_lbvco(0x1));
    if (vco_freq_khz > pll_vco_cutoff_in_khz) {
        EFUN(wrc_ams_pll_en_lb_res(0x0));
        EFUN(wrc_ams_pll_en_lb_ind(0x0));
        EFUN(wrc_ams_pll_vcobuf_bandcntrl(0x4));
        EFUN(wrc_ams_pll_pwrdn_hbvco(0x0)); /* enable HBVCO */
    }
    else {
        EFUN(wrc_ams_pll_en_lb_res(0x1));
        EFUN(wrc_ams_pll_en_lb_ind(0x1));
        EFUN(wrc_ams_pll_pwrdn_lbvco(0x0)); /* enable LBVCO */
    }
    if(vco_freq_khz <= VCO_FREQ_50P0G_KHZ) {
        EFUN(wrc_ams_pll_lb_sel_pll0(0x1));
    }
    EFUN(wrc_ams_pll_cp_calib_adj(0x6));
    {
        uint8_t cap_pll = 0;
        if (vco_freq_khz > 55800000) {
            cap_pll = 0;
        }
        else if (vco_freq_khz > 54800000) {
            cap_pll = 1;
        }
        else if (vco_freq_khz > 53600000) {
            cap_pll = 2;
        }
        else if (vco_freq_khz > 52600000) {
            cap_pll = 3;
        }
        else if (vco_freq_khz > 51600000) {
            cap_pll = 4;
        }
        else if (vco_freq_khz > 50600000) {
            cap_pll = 5;
        }
        else if (vco_freq_khz > 49900000) {
            cap_pll = 6;
        }
        else if (vco_freq_khz > 49200000) {
            cap_pll = 0;
        }
        else if (vco_freq_khz > 48200000) {
            cap_pll = 1;
        }
        else if (vco_freq_khz > 47400000) {
            cap_pll = 2;
        }
        else if (vco_freq_khz > 46600000) {
            cap_pll = 3;
        }
        else if (vco_freq_khz > 45800000) {
            cap_pll = 4;
        }
        else if (vco_freq_khz > 45000000) {
            cap_pll = 5;
        }
        else if (vco_freq_khz > 44400000) {
            cap_pll = 6;
        }
        else if (vco_freq_khz > 43600000) {
            cap_pll = 7;
        }
        else if (vco_freq_khz > 43000000) {
            cap_pll = 8;
        }
        else if (vco_freq_khz > 42400000) {
            cap_pll = 9;
        }
        else if (vco_freq_khz > 41800000) {
            cap_pll = 10;
        }
        else if (vco_freq_khz > 41200000) {
            cap_pll = 11;
        }
        else if (vco_freq_khz > 40800000) {
            cap_pll = 12;
        }
        EFUN(wrc_ams_pll_cap_l_pll0(cap_pll));
        EFUN(wrc_ams_pll_cap_r_pll0(cap_pll));
    }
    {
        uint8_t ictrl;
        uint8_t ictrl_rpt;
        uint8_t pll_idx;
        ESTM(pll_idx = osprey7_v2l8p2_acc_get_pll_idx(sa__));
        if(pll_idx == 0) {
            if (vco_freq_khz > 55140000) {
                ictrl = 3; ictrl_rpt = 2;
            }
            else if (vco_freq_khz > 53260000) {
                ictrl = 4; ictrl_rpt = 3;
            }
            else if (vco_freq_khz > 51380000) {
                ictrl = 5; ictrl_rpt = 4;
            }
            else if (vco_freq_khz > 49900000) {
                ictrl = 6; ictrl_rpt = 5;
            }
            else if (vco_freq_khz > 47320000) {
                ictrl = 1; ictrl_rpt = 0;
            }
            else if (vco_freq_khz > 44160000) {
                ictrl = 2; ictrl_rpt = 1;
            }
            else {
                ictrl = 3; ictrl_rpt = 2;
            }
        }
        else {
            if (vco_freq_khz > 54500000) {
                ictrl = 2;
            }
            else if (vco_freq_khz > 52000000) {
                ictrl = 3;
            }
            else if (vco_freq_khz > 49900000) {
                ictrl = 4;
            }
            else {
                ictrl = 1;
            }
            if (vco_freq_khz > 55140000) {
                ictrl_rpt = 0;
            }
            else if (vco_freq_khz > 53260000) {
                ictrl_rpt = 1;
            }
            else if (vco_freq_khz > 51380000) {
                ictrl_rpt = 2;
            }
            else if (vco_freq_khz > 49900000) {
                ictrl_rpt = 3;
            }
            else {
                ictrl_rpt = 1;
            }
        }
        EFUN(wrc_ams_pll_ictrl_pll0(ictrl));
        EFUN(wrc_ams_pll_ictrl_rpt_pll0(ictrl_rpt));
    }
    {
        /* Get information needed for fractional mode configuration.
         * Start with the div value composed of an integer and a wide fractional value.
         *
         * The value programmed into the pll_fracn_* bitfields which must account for the
         * initial div2 stage after the VCO.  For instance, a divide by 147.2 must be
         * programmed with an integer of 73 and a fraction of 0.6.
         *
         * Start with the div value, divided by 2, composed of an integer and a wide fractional value.
         */
        const uint8_t  div_fraction_width = 28; /* Must be less than 32 due to overflow detection below. */
        const uint16_t div_integer        = (uint16_t)(SRDS_INTERNAL_GET_PLL_DIV_INTEGER(srds_div) >> 1);
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
        const uint16_t pll_fracn_ndiv_int  = (uint16_t)(div_integer + div_fraction_plus_p5_carry);
        const uint32_t pll_fracn_div = ((div_fraction_plus_0p5 & ((1U << div_fraction_width)-1))
                                        >> (div_fraction_width - pll_fracn_frac_bits));

        if (pll_fracn_ndiv_int != (pll_fracn_ndiv_int & ((1 << pll_fracn_ndiv_int_bits)-1))) {
            EFUN_PRINTF(("ERROR:  PLL divide is too large for div value 0x%08X\n", srds_div));
            return (osprey7_v2l8p2_error(sa__, ERR_CODE_PLL_DIV_INVALID));
        }

        /* fracn_ndiv_int restrcited to 12 to 511) */
        if ((pll_fracn_ndiv_int < 12) || (pll_fracn_ndiv_int > 511)) {
            return (osprey7_v2l8p2_error(sa__, ERR_CODE_INVALID_PLL_CFG));
        }

        /* Fractional mode settings only */
        if (pll_fracn_div > 0) {
            EFUN(wrc_ams_pll_fracn_enable(0x1));
            EFUN(wrc_ams_pll_icp_sel(0x9));
            EFUN(wrc_ams_pll_rz_sel(0x5));
            EFUN(wrc_ams_pll_cp_sel(0x0));
            EFUN(wrc_ams_pll_fp3_r_sel(0x1));
            EFUN(wrc_ams_pll_fp3_c_sel(0xF));
            EFUN(wrc_ams_pll_en_offset_fbck(0x1));
            EFUN(wrc_ams_pll_en_offset_refclk(0x0));
            EFUN(wrc_ams_pll_fracn_offset_ctrl(0x1));
            EFUN(wrc_ams_pll_pfd_pulse_adj(0x1));
        }
        else {
            EFUN(wrc_ams_pll_rz_sel(0x1));
        }

        /* To ensure glitch-free operation - toggle ndiv_valid low; load ndiv values; toggle ndiv_valid high. */
        EFUN(wrc_ams_pll_resetb_mmd(0x0));                                /* reset PLL mmd */
        EFUN(wrc_ams_pll_resetb_mmd(0x1));                                /* release reset */
        EFUN(wrc_ams_pll_i_ndiv_valid(0x0));                              /* assert low before programming fracn PLL div value */
        if (pll_fracn_ndiv_int < 60) {
            EFUN(wrc_ams_pll_pll_frac_mode(0x2)); /* MMD 4/5 mode (pll_frac_mode = 2) => [12 <= Ndiv < 60] */
        }
        else {
            EFUN(wrc_ams_pll_pll_frac_mode(0x1)); /* MMD 8/9 mode (pll_frac_mode = 1) => [60 <= Ndiv < 512] */
        }
        EFUN(wrc_ams_pll_fracn_ndiv_int   (pll_fracn_ndiv_int));          /* interger part of fracn PLL div */
        EFUN(wrc_ams_pll_fracn_ndiv_frac_l(_ndiv_frac_l(pll_fracn_div))); /* set lower 16 bits of fractional part of fracn PLL div */
        EFUN(wrc_ams_pll_fracn_ndiv_frac_h((uint8_t)(_ndiv_frac_h(pll_fracn_div)))); /* set upper  2 bits of fractional part of fracn PLL div */
        EFUN(wrc_ams_pll_i_ndiv_valid(0x1));                              /* to load fracn_ndiv and ndiv_int */
        EFUN(USR_DELAY_US(5));                                            /* delay of atleast 8 Refclk cycles */
        EFUN(wrc_ams_pll_i_ndiv_valid(0x0));                              /* to latch fracn_ndiv and ndiv_int */
    }

    /* NOTE: Might have to add some optimized PLL control settings post-DVT*/

    /* Toggling PLL mmd reset */
    EFUN(wrc_ams_pll_resetb_mmd(0x0));                                    /* reset PLL mmd */
    EFUN(wrc_ams_pll_resetb_mmd(0x1));                                    /* release reset */

    /* Update core variables with the VCO rate. */
    {
        struct osprey7_v2l8p2_uc_core_config_st core_config = UC_CORE_CONFIG_INIT;
        EFUN(osprey7_v2l8p2_get_uc_core_config(sa__, &core_config));
        core_config.vco_rate_in_Mhz = (int32_t)((vco_freq_khz + 500) / 1000);
        core_config.field.vco_rate = MHZ_TO_VCO_RATE(core_config.vco_rate_in_Mhz);
        EFUN(osprey7_v2l8p2_INTERNAL_set_uc_core_config(sa__, core_config));
    }

    {
        uint8_t pll_idx;
        uint8_t other_pll_lock;
        ESTM(pll_idx = osprey7_v2l8p2_acc_get_pll_idx(sa__));
        EFUN(osprey7_v2l8p2_acc_set_pll_idx(sa__, ((pll_idx == 0) ? 1 : 0)));
        ESTM(other_pll_lock = rdc_pll_lock());
        if (!other_pll_lock) {
            uint8_t misc_ctrl_byte;
            ESTM(misc_ctrl_byte = rdcv_misc_ctrl_byte());
            if (pll_idx == 1) misc_ctrl_byte |= 1<<2;
            else              misc_ctrl_byte = (uint8_t)(misc_ctrl_byte & ~(1<<2));
            EFUN(wrcv_misc_ctrl_byte(misc_ctrl_byte));
        }
        EFUN(osprey7_v2l8p2_acc_set_pll_idx(sa__, pll_idx));
    }
    return (ERR_CODE_NONE);
} /* osprey7_v2l8p2_configure_pll */

err_code_t osprey7_v2l8p2_INTERNAL_configure_lb_53P125_pll(srds_access_t *sa__,
                                                enum osprey7_v2l8p2_pll_refclk_enum refclk,
                                                enum osprey7_v2l8p2_pll_div_enum srds_div,
                                                enum osprey7_v2l8p2_pll_option_enum pll_option) {
    INIT_SRDS_ERR_CODE
    uint8_t ams_version;

    ESTM(ams_version = rd_ams_tx_version_id());
    EFUN(osprey7_v2l8p2_core_dp_reset(sa__,1));
    EFUN(osprey7_v2l8p2_INTERNAL_configure_pll(sa__, refclk, srds_div, VCO_FREQ_53P125G_KHZ, pll_option));
    if(ams_version == 0xa0) {
        uint8_t core_ctrl = 0;
        EFUN(wrc_ams_pll_pwrdn_hbvco(0x1));
        EFUN(wrc_ams_pll_pwrdn_lbvco(0x0)); /* enable LBVCO */
        ESTM(core_ctrl = rdcv_usr_int_core_ctrl_byte() | 0x1);
        EFUN(wrcv_usr_int_core_ctrl_byte(core_ctrl));
    }
    EFUN(osprey7_v2l8p2_core_dp_reset(sa__,0));

    return (ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_INTERNAL_force_hb_53P125_pll(srds_access_t *sa__) {
    INIT_SRDS_ERR_CODE
    uint8_t ams_version;

    ESTM(ams_version = rd_ams_tx_version_id());
    if(ams_version == 0xa0) {
        EFUN(osprey7_v2l8p2_core_dp_reset(sa__,1));
        EFUN(wrc_ams_pll_pwrdn_lbvco(0x1));
        EFUN(wrc_ams_pll_pwrdn_hbvco(0x0)); /* enable HBVCO */
        EFUN(osprey7_v2l8p2_core_dp_reset(sa__,0));
    }

    return (ERR_CODE_NONE);
}

/* Get the PLL LB lock status */
err_code_t osprey7_v2l8p2_INTERNAL_pll_lb_range_check(srds_access_t *sa__, uint8_t *pll_out_range) {
    INIT_SRDS_ERR_CODE
    uint8_t pll_range = 0;

    ESTM(pll_range = rdc_pll_range());
    *pll_out_range = pll_range > LBVCO_USL;
    return ERR_CODE_NONE;
}

/* Get the PLL HB lock status */
err_code_t osprey7_v2l8p2_INTERNAL_pll_hb_range_check(srds_access_t *sa__, uint8_t *pll_out_range) {
    INIT_SRDS_ERR_CODE
    uint8_t pll_range = 0;

    ESTM(pll_range = rdc_pll_range());
    *pll_out_range = pll_range <= LBVCO_USL;
    return ERR_CODE_NONE;
}

err_code_t osprey7_v2l8p2_INTERNAL_configure_53P125_pll(srds_access_t *sa__,
                                                enum osprey7_v2l8p2_pll_refclk_enum refclk,
                                                enum osprey7_v2l8p2_pll_div_enum srds_div,
                                                enum osprey7_v2l8p2_pll_option_enum pll_option) {
    INIT_SRDS_ERR_CODE
    uint8_t pll_lock=0;
    uint8_t pll_lock_chg=0;
    uint8_t ams_version=0;

    ESTM(ams_version = rd_ams_tx_version_id());
    EFUN(osprey7_v2l8p2_INTERNAL_configure_lb_53P125_pll(sa__, refclk, srds_div, pll_option)); /* Enable LBVCO for A0, some A1, C0, C1 AFE versions */
    EFUN(USR_DELAY_MS(50));
    EFUN(osprey7_v2l8p2_INTERNAL_pll_lock_status(sa__,&pll_lock,&pll_lock_chg));
    if(ams_version > 0xa0) {
        if(pll_lock==1) {
            return ERR_CODE_NONE;
        }
    }
    else {
        uint8_t pll_out_range=0;
        EFUN(osprey7_v2l8p2_INTERNAL_pll_lb_range_check(sa__, &pll_out_range));
        if((pll_lock==0) || ((pll_lock==1) && (pll_out_range==1))) {
            EFUN(osprey7_v2l8p2_INTERNAL_force_hb_53P125_pll(sa__)); /* Enable HBVCO */
            EFUN(USR_DELAY_MS(50));
            EFUN(osprey7_v2l8p2_INTERNAL_pll_lock_status(sa__,&pll_lock,&pll_lock_chg));
            EFUN(osprey7_v2l8p2_INTERNAL_pll_hb_range_check(sa__, &pll_out_range));
            if((pll_lock==1) && (pll_out_range==0)) {
                return ERR_CODE_NONE;
            }
        }
        else {
            return ERR_CODE_NONE;
        }
    }
    ESTM_PRINTF(("ERROR: Reliable PLL Lock not possible; core %d PLL %d\n",osprey7_v2l8p2_acc_get_core(sa__), osprey7_v2l8p2_acc_get_pll_idx(sa__)));
    return ERR_CODE_NO_PLL_LOCK;
}

#ifndef SMALL_FOOTPRINT
err_code_t osprey7_v2l8p2_INTERNAL_read_pll_div(srds_access_t *sa__, uint32_t *srds_div) {
    INIT_SRDS_ERR_CODE
    uint16_t pll_fracn_ndiv_int;
    uint32_t pll_fracn_div;
    ESTM(pll_fracn_ndiv_int = rdc_ams_pll_fracn_ndiv_int());
    ESTM(pll_fracn_div = (uint32_t)(_ndiv_frac_decode(rdc_ams_pll_fracn_ndiv_frac_l(), rdc_ams_pll_fracn_ndiv_frac_h())));

    /* The value programmed into the pll_fracn_* bitfields which must
     * account for the initial div2 stage after the VCO.  For instance, a
     * divide by 147.2 must be programmed with an integer of 73 and a
     * fraction of 0.6.
     *
     * So multiply the bitfield reads by 2.
     */

      pll_fracn_ndiv_int = (uint16_t)(pll_fracn_ndiv_int << 1);
      pll_fracn_div <<= 1;

      {
          /* If the post-multiplied fractional part overflows, then apply the carry to
           * the integer part.
           */
          const uint32_t pll_fracn_div_masked = pll_fracn_div & ((1U << pll_fracn_frac_bits)-1);
          if (pll_fracn_div_masked != pll_fracn_div) {
              ++pll_fracn_ndiv_int;
              pll_fracn_div = pll_fracn_div_masked;
          }
      }

    *srds_div = SRDS_INTERNAL_COMPOSE_PLL_DIV(pll_fracn_ndiv_int, pll_fracn_div, pll_fracn_frac_bits);
    return (ERR_CODE_NONE);
}
#endif /* SMALL_FOOTPRINT */

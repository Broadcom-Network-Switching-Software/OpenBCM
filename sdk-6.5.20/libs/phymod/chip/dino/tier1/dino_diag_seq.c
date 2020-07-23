/*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
*/

/*
 *         
 * 
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 */
#define DINO_EXCLUDE_STATIC_FUNCTION_DECLARATIONS

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_diagnostics.h>
#include "bcmi_dino_defs.h"
#include "dino_cfg_seq.h"
#include "dino_diag_seq.h"
#include "dino_serdes/falcon2_dino_src/falcon2_dino_functions.h"
#include "dino_serdes/merlin_dino_src/merlin_dino_functions.h"
#include "dino_serdes/common/srds_api_enum.h"
#include "dino_serdes/falcon2_dino_src/falcon2_dino_internal.h"
#include "dino_serdes/merlin_dino_src/merlin_dino_internal.h"

int _dino_phy_prbs_config_set(const phymod_access_t* pa, uint32_t flags, uint16_t if_side, uint16_t lane, const phymod_prbs_t* prbs)
{
    uint16_t prbs_poly = 0;
    
    PHYMOD_IF_ERR_RETURN(
          _dino_phymod_prbs_poly_to_serdes_prbs_poly(prbs->poly, &prbs_poly));
    if (flags == 0 || PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        if (DINO_IS_FALCON_SIDE(if_side, lane)) {
            PHYMOD_IF_ERR_RETURN(
                falcon2_dino_config_tx_prbs(pa, prbs_poly, (uint8_t)prbs->invert)); 
        } else {
            PHYMOD_IF_ERR_RETURN(
                merlin_dino_config_tx_prbs(pa, prbs_poly, (uint8_t)prbs->invert)); 
        }
    }

    if (PHYMOD_PRBS_DIRECTION_RX_GET(flags) || flags == 0) {
        if (DINO_IS_FALCON_SIDE(if_side, lane)) {
            PHYMOD_IF_ERR_RETURN(
                falcon2_dino_config_rx_prbs(pa, prbs_poly, PRBS_INITIAL_SEED_HYSTERESIS, 
                                            (uint8_t)prbs->invert));
        } else {
            PHYMOD_IF_ERR_RETURN(
                merlin_dino_config_rx_prbs(pa, prbs_poly, PRBS_INITIAL_SEED_HYSTERESIS, 
                                           (uint8_t)prbs->invert)); 
        }
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_prbs_enable_set(const phymod_access_t* pa, uint32_t flags, uint16_t if_side, uint16_t lane, uint32_t enable)
{
    if (flags == 0 || PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        if (DINO_IS_FALCON_SIDE(if_side, lane)) {
            PHYMOD_IF_ERR_RETURN(
                falcon2_dino_tx_prbs_en(pa, enable));
        } else {
            PHYMOD_IF_ERR_RETURN(
                merlin_dino_tx_prbs_en(pa, enable));
        }
    }
    if (PHYMOD_PRBS_DIRECTION_RX_GET(flags) || flags == 0) {
        if (DINO_IS_FALCON_SIDE(if_side, lane)) {
            if (enable) {
                PHYMOD_IF_ERR_RETURN (
	                falcon2_dino_wr_prbs_chk_clk_en_frc_on(1));
            }
            PHYMOD_IF_ERR_RETURN(
                falcon2_dino_rx_prbs_en(pa, enable));
            if (!enable) {
                PHYMOD_IF_ERR_RETURN (
	                falcon2_dino_wr_prbs_chk_clk_en_frc_on(0));
            }
        } else {
            if (enable) {
                PHYMOD_IF_ERR_RETURN (
                     wr_prbs_chk_clk_en_frc_on(1));
            }
            PHYMOD_IF_ERR_RETURN (
                 merlin_dino_rx_prbs_en(pa, enable));
            if (!enable) {
                PHYMOD_IF_ERR_RETURN (
	               wr_prbs_chk_clk_en_frc_on(0));
            }
        }
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_prbs_enable_get(const phymod_access_t* pa, uint32_t flags, uint16_t if_side, uint16_t lane, uint32_t *enable)
{
    uint8_t gen_en = 0;
    uint8_t ckr_en = 0;

    if (flags == 0 || PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        if (DINO_IS_FALCON_SIDE(if_side, lane)) {
            PHYMOD_IF_ERR_RETURN(
                falcon2_dino_get_tx_prbs_en(pa, &gen_en));
            *enable = gen_en;
        } else {
            PHYMOD_IF_ERR_RETURN(
                  merlin_dino_get_tx_prbs_en(pa, &gen_en));
            *enable = gen_en;
        }
    }
    if (flags == 0 || PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        if (DINO_IS_FALCON_SIDE(if_side, lane)) {
            PHYMOD_IF_ERR_RETURN(
                falcon2_dino_get_rx_prbs_en(pa, &ckr_en));
            *enable = ckr_en;
        } else {
            PHYMOD_IF_ERR_RETURN(
                  merlin_dino_get_rx_prbs_en(pa, &ckr_en));
            *enable = ckr_en;
        }
    }
    if (flags == 0) {
        *enable = ckr_en & gen_en;
    } 
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int _dino_phy_prbs_config_get(const phymod_access_t* pa,
                              uint32_t flags,
                              uint16_t if_side,
                              uint16_t lane, 
                              phymod_prbs_t* prbs,
                              uint16_t* serdes_poly, 
                              uint16_t* chkr_mode) 
{
    uint8_t data = 0;
    enum srds_prbs_polynomial_enum srds_prbs_poly;
    enum srds_prbs_checker_mode_enum srds_prbs_chkr_mode;

    if (flags == 0 || PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        if (DINO_IS_FALCON_SIDE(if_side, lane)) {
            PHYMOD_IF_ERR_RETURN(falcon2_dino_get_tx_prbs_config(pa, &srds_prbs_poly, &data));
            prbs->invert = data;                                          
            *serdes_poly = srds_prbs_poly;
        } else { 
            PHYMOD_IF_ERR_RETURN(merlin_dino_get_tx_prbs_config(pa, &srds_prbs_poly, &data));
            prbs->invert = data;                                          
            *serdes_poly = srds_prbs_poly;
        }                                                                 
    }                                                                     
    if (flags == 0 || PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {              
        if (DINO_IS_FALCON_SIDE(if_side, lane)) {
            PHYMOD_IF_ERR_RETURN(falcon2_dino_get_rx_prbs_config(pa, &srds_prbs_poly, &srds_prbs_chkr_mode, &data));
            prbs->invert = data;
            *serdes_poly = srds_prbs_poly;
            *chkr_mode = srds_prbs_chkr_mode;
        } else {
            PHYMOD_IF_ERR_RETURN(merlin_dino_get_rx_prbs_config(pa, &srds_prbs_poly, &srds_prbs_chkr_mode, &data));
            prbs->invert = data;
            *serdes_poly = srds_prbs_poly;
            *chkr_mode = srds_prbs_chkr_mode;
        }
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_prbs_status_get(const phymod_access_t* pa, uint32_t flags, uint16_t if_side, uint16_t lane, phymod_prbs_status_t* prbs_status)
{
    uint8_t data = 0;
    uint32_t err_cnt = 0;

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN(falcon2_dino_prbs_chk_lock_state(pa, &data));
        prbs_status->prbs_lock &= data;
        PHYMOD_IF_ERR_RETURN(falcon2_dino_prbs_err_count_state(pa, &err_cnt, &data));
        prbs_status->prbs_lock_loss &= data;
        prbs_status->error_count |= err_cnt;
    } else {
        PHYMOD_IF_ERR_RETURN(merlin_dino_prbs_chk_lock_state(pa, &data));
        prbs_status->prbs_lock &= data;
        PHYMOD_IF_ERR_RETURN(merlin_dino_prbs_err_count_state(pa, &err_cnt, &data));
        prbs_status->prbs_lock_loss &= data;
        prbs_status->error_count |= err_cnt;
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_display_eyescan(const phymod_access_t *pa, uint16_t if_side, uint16_t lane)
{
    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN
            (falcon2_dino_display_core_state_hdr(pa));
        PHYMOD_IF_ERR_RETURN
            (falcon2_dino_display_core_state_line(pa));
        PHYMOD_IF_ERR_RETURN
            (falcon2_dino_display_lane_state_hdr(pa));
        PHYMOD_IF_ERR_RETURN
            (falcon2_dino_display_lane_state(pa)); 
        PHYMOD_IF_ERR_RETURN
            (falcon2_dino_display_eye_scan(pa));
    } else {
        PHYMOD_IF_ERR_RETURN
            (merlin_dino_display_core_state_hdr(pa)) ;
        PHYMOD_IF_ERR_RETURN
            (merlin_dino_display_core_state_line(pa));
        PHYMOD_IF_ERR_RETURN
            (merlin_dino_display_lane_state_hdr(pa));
        PHYMOD_IF_ERR_RETURN
            (merlin_dino_display_lane_state(pa)); 
        PHYMOD_IF_ERR_RETURN
            (merlin_dino_display_eye_scan(pa));
    }

    return PHYMOD_E_NONE;
}

err_code_t _dino_merlin_read_pll_range(const phymod_access_t *pa, uint32_t *pll_range)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *pll_range = rdc_pll_range();

  return (ERR_CODE_NONE);
}

err_code_t _dino_falcon2_read_pll_range(const phymod_access_t *pa, uint32_t *pll_range)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *pll_range = falcon2_dino_rdc_ams_pll_range();

  return (ERR_CODE_NONE);
}

int _dino_core_diagnostics_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane,phymod_core_diagnostics_t* diag)
{
    int16_t temp = 0;

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN(falcon2_dino_read_die_temperature(pa, &temp));
        diag->temperature = temp;
        PHYMOD_IF_ERR_RETURN(_dino_falcon2_read_pll_range(pa, &diag->pll_range));
    } else {  /* Merlin core status dump */
        PHYMOD_IF_ERR_RETURN(merlin_dino_read_die_temperature(pa, &temp));
        diag->temperature = temp;
        PHYMOD_IF_ERR_RETURN(_dino_merlin_read_pll_range(pa, &diag->pll_range));
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_diagnostics_get(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, phymod_phy_diagnostics_t* diag)
{
    falcon2_dino_lane_state_st falcon_state;
    merlin_dino_lane_state_st merlin_state;
    PHYMOD_MEMSET(&falcon_state, 0, sizeof(falcon2_dino_lane_state_st));
    PHYMOD_MEMSET(&merlin_state, 0, sizeof(merlin_dino_lane_state_st));
    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_DEBUG_VERBOSE(("Phy Diagnostics for Lane:%d \n",lane));
        /* set the core to get the master /slave */
        /* system side always master line side considered as slave*/
        PHYMOD_IF_ERR_RETURN(
            _falcon2_dino_read_lane_state(pa, &falcon_state));
        diag->signal_detect = falcon_state.sig_det;
        diag->osr_mode = falcon_state.osr_mode.tx_rx;
        diag->rx_lock = falcon_state.rx_lock;
        diag->tx_ppm = falcon_state.tx_ppm;
        diag->clk90_offset = falcon_state.clk90;
        diag->clkp1_offset = falcon_state.clkp1;
        diag->p1_lvl = falcon_state.p1_lvl;
        diag->dfe1_dcd = falcon_state.dfe1_dcd;
        diag->dfe2_dcd = falcon_state.dfe2_dcd;
        diag->slicer_offset.offset_pe = falcon_state.pe;
        diag->slicer_offset.offset_ze = falcon_state.ze;
        diag->slicer_offset.offset_me = falcon_state.me;
        diag->slicer_offset.offset_po = falcon_state.po;
        diag->slicer_offset.offset_zo = falcon_state.zo;
        diag->slicer_offset.offset_mo = falcon_state.mo;
        diag->eyescan.heye_left = falcon_state.heye_left;
        diag->eyescan.heye_right = falcon_state.heye_right;
        diag->eyescan.veye_upper = falcon_state.veye_upper;
        diag->eyescan.veye_lower = falcon_state.veye_lower;
        diag->link_time = falcon_state.link_time;
        diag->pf_main = falcon_state.pf_main;
        diag->pf_hiz = falcon_state.pf_hiz;
        diag->pf_bst = falcon_state.pf_bst;
        diag->pf_low = 0; /* not availble in serdes api*/
        diag->pf2_ctrl = falcon_state.pf2_ctrl;
        diag->vga = falcon_state.vga;
        diag->dc_offset = falcon_state.dc_offset;
        diag->p1_lvl_ctrl = falcon_state.p1_lvl_ctrl;
        diag->dfe1 = falcon_state.dfe1;
        diag->dfe2 = falcon_state.dfe2;
        diag->dfe3 = falcon_state.dfe3;
        diag->dfe4 = falcon_state.dfe4;
        diag->dfe5 = falcon_state.dfe5;
        diag->dfe6 = falcon_state.dfe6;
        diag->txfir_pre = falcon_state.txfir_pre;
        diag->txfir_main = falcon_state.txfir_main;
        diag->txfir_post1 = falcon_state.txfir_post1;
        diag->txfir_post2 = falcon_state.txfir_post2;
        diag->txfir_post3 = falcon_state.txfir_post3;
        diag->tx_amp_ctrl = 0; /* not availble in serdes api*/
        diag->br_pd_en = falcon_state.br_pd_en;
    } else {
        PHYMOD_IF_ERR_RETURN
            (_merlin_dino_read_lane_state(pa, &merlin_state));
        diag->signal_detect = merlin_state.sig_det;
        diag->osr_mode = merlin_state.osr_mode.tx_rx;
        diag->rx_lock = merlin_state.rx_lock;
        diag->tx_ppm = merlin_state.tx_ppm;
        diag->clk90_offset = merlin_state.clk90;
        diag->clkp1_offset = merlin_state.clkp1;
        diag->p1_lvl = merlin_state.p1_lvl;
        diag->dfe1_dcd = merlin_state.dfe1_dcd;
        diag->dfe2_dcd = merlin_state.dfe2_dcd;
        diag->slicer_offset.offset_pe = merlin_state.pe;
        diag->slicer_offset.offset_ze = merlin_state.ze;
        diag->slicer_offset.offset_me = merlin_state.me;
        diag->slicer_offset.offset_po = merlin_state.po;
        diag->slicer_offset.offset_zo = merlin_state.zo;
        diag->slicer_offset.offset_mo = merlin_state.mo;
        diag->eyescan.heye_left = merlin_state.heye_left;
        diag->eyescan.heye_right = merlin_state.heye_right;
        diag->eyescan.veye_upper = merlin_state.veye_upper;
        diag->eyescan.veye_lower = merlin_state.veye_lower;
        diag->link_time = merlin_state.link_time;
        diag->pf_main = merlin_state.pf_main;
        diag->pf_hiz = merlin_state.pf_hiz;
        diag->pf_bst = merlin_state.pf_bst;
        diag->pf_low = 0; /* Not Availble in serdes API*/
        diag->pf2_ctrl = merlin_state.pf2_ctrl;
        diag->vga = merlin_state.vga;
        diag->dc_offset = merlin_state.dc_offset;
        diag->p1_lvl_ctrl = merlin_state.p1_lvl_ctrl;
        diag->dfe1 = merlin_state.dfe1;
        diag->dfe2 = merlin_state.dfe2;
        diag->dfe3 = merlin_state.dfe3;
        diag->dfe4 = merlin_state.dfe4;
        diag->dfe5 = merlin_state.dfe5;
        diag->dfe6 = merlin_state.dfe6;
        diag->txfir_pre = merlin_state.txfir_pre;
        diag->txfir_main = merlin_state.txfir_main;
        diag->txfir_post1 = merlin_state.txfir_post1;
        diag->txfir_post2 = merlin_state.txfir_post2;
        diag->txfir_post3 = merlin_state.txfir_post3;
        diag->tx_amp_ctrl = 0; /* Not Availble in serdes API*/
        diag->br_pd_en = merlin_state.br_pd_en;
    }

    return PHYMOD_E_NONE;
}

int _dino_ber_proj_lane_speed_get(const phymod_phy_access_t *phy, phymod_phy_inf_config_t *config, uint16_t if_side, uint16_t lane, uint32_t *speed)
{
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMSET(&phy_copy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));
    
    PHYMOD_IF_ERR_RETURN(
        _dino_phy_interface_config_get(&phy_copy, 0, config));
    
    if (if_side == DINO_IF_LINE) {
        if (config->data_rate == DINO_SPD_100G || config->data_rate == DINO_SPD_106G) {
            if (PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(&phy_copy) &&
                (if_side == DINO_IF_LINE)) {
                *speed = 25781250;
                if (PHYMOD_INTF_MODES_OTN_GET(config)) {
                    *speed = 27952500;
                } else if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                    *speed = 27343750;
                }
            } else  {
                *speed = 10312500;
                if (PHYMOD_INTF_MODES_OTN_GET(config)) {
                    *speed = 10937500;
                } else if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                    *speed = 11180000;
                }
            }
        } else if ((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G ||
                    config->data_rate == DINO_SPD_10G || config->data_rate == DINO_SPD_11G
                    )) {
            *speed = 10312500;
            if (PHYMOD_INTF_MODES_OTN_GET(config)) {
                *speed = 10937500;
            } else if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                *speed = 11180000;
            }
        }
    } else {
            *speed = 10312500;
            if (PHYMOD_INTF_MODES_OTN_GET(config)) {
                *speed = 10937500;
            } else if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                *speed = 11180000;
            }
    }

    return PHYMOD_E_NONE;
}

int _dino_ber_proj(const phymod_phy_access_t *phy, uint16_t if_side, uint16_t lane, const phymod_phy_eyescan_options_t* eyescan_options)
{
    const phymod_access_t *pa = &phy->access;
    phymod_phy_inf_config_t config;
    int rv = PHYMOD_E_NONE;
#ifdef SERDES_API_FLOATING_POINT
    uint32_t speed;
    USR_DOUBLE speedD;
#endif
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));

    PHYMOD_DEBUG_VERBOSE(("eye_margin_proj for Lane:%d \n", lane));
    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN
            (falcon2_dino_display_core_state_hdr(pa));
        PHYMOD_IF_ERR_RETURN
            (falcon2_dino_display_core_state_line(pa));
        PHYMOD_IF_ERR_RETURN
            (falcon2_dino_display_lane_state_hdr(pa));
        PHYMOD_IF_ERR_RETURN
            (falcon2_dino_display_lane_state(pa));
#ifdef SERDES_API_FLOATING_POINT
        PHYMOD_IF_ERR_RETURN
            (_dino_ber_proj_lane_speed_get(phy, &config, if_side, lane, &speed));
        speedD = speed * 1000.0;
        PHYMOD_IF_ERR_RETURN (
            _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
        PHYMOD_IF_ERR_RETURN
            (falcon2_dino_eye_margin_proj(pa, speedD, eyescan_options->ber_proj_scan_mode,
                               eyescan_options->ber_proj_timer_cnt, eyescan_options->ber_proj_err_cnt));
#else
        PHYMOD_DIAG_OUT(("BER PROJ needs SERDES_API_FLOATING_POINT define to operate\n"));
        rv = PHYMOD_E_RESOURCE;
#endif
    } else {
        PHYMOD_IF_ERR_RETURN
            (merlin_dino_display_core_state_hdr(pa)) ;
        PHYMOD_IF_ERR_RETURN
            (merlin_dino_display_core_state_line(pa));
        PHYMOD_IF_ERR_RETURN
            (merlin_dino_display_lane_state_hdr(pa));
        PHYMOD_IF_ERR_RETURN
            (merlin_dino_display_lane_state(pa));
#ifdef SERDES_API_FLOATING_POINT
        PHYMOD_IF_ERR_RETURN
            (_dino_ber_proj_lane_speed_get(phy, &config, if_side, lane, &speed));
        speedD = speed * 1000.0;
        PHYMOD_IF_ERR_RETURN (
            _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
        PHYMOD_IF_ERR_RETURN
            (merlin_dino_eye_margin_proj(pa, speedD, eyescan_options->ber_proj_scan_mode,
                           eyescan_options->ber_proj_timer_cnt, eyescan_options->ber_proj_err_cnt));
        /*Work around for PRBS Locked with Errors */
#else
        PHYMOD_DIAG_OUT(("BER PROJ needs SERDES_API_FLOATING_POINT define to operate\n"));
        rv = PHYMOD_E_RESOURCE;
#endif
        PHYMOD_IF_ERR_RETURN
            (wr_trnsum_error_count_en(0));
    }

    return rv;
}

int _dino_phy_link_mon_enable_set(const phymod_access_t* pa, uint32_t enable) 
{
    uint32_t reg_val = 0;
    BCMI_DINO_MAIN_CTLr_t dino_main_ctl;
    PHYMOD_MEMSET(&dino_main_ctl, 0, sizeof(BCMI_DINO_MAIN_CTLr_t));

    /* Disable link mon before configuring it */
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MAIN_CTLr(pa, &dino_main_ctl));
    reg_val = BCMI_DINO_MAIN_CTLr_GET(dino_main_ctl);
    reg_val &= (~(1 << 2));
    BCMI_DINO_MAIN_CTLr_SET(dino_main_ctl, reg_val);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MAIN_CTLr(pa, dino_main_ctl));
    /* enable / disable link monitor */
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MAIN_CTLr(pa, &dino_main_ctl));
    reg_val = BCMI_DINO_MAIN_CTLr_GET(dino_main_ctl);
    reg_val |= (enable << 2);
    BCMI_DINO_MAIN_CTLr_SET(dino_main_ctl, reg_val);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MAIN_CTLr(pa, dino_main_ctl));

    return PHYMOD_E_NONE;
}

int _dino_phy_link_mon_enable_get(const phymod_access_t* pa, uint32_t *enable)
{
    BCMI_DINO_MAIN_CTLr_t dino_main_ctl;
    PHYMOD_MEMSET(&dino_main_ctl, 0, sizeof(BCMI_DINO_MAIN_CTLr_t));

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MAIN_CTLr(pa, &dino_main_ctl));
    if (BCMI_DINO_MAIN_CTLr_MAIN_CTRLf_GET(dino_main_ctl) & 0x1) {
        *enable &= 1;
    } else {
        *enable &= 0;
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_link_mon_status_get(const phymod_access_t* pa, uint32_t* lock_status, uint32_t* lock_lost_lh, uint32_t* error_count)
{
    uint16_t ber_count_lsb = 0;
    uint16_t ber_count_msb = 0;
    BCMI_DINO_MAIN_STSr_t dino_main_sts;
    BCMI_DINO_LIVE_STSr_t dino_live_sts;
    BCMI_DINO_CL49_BER_STSr_t cl49_ber_sts;
    BCMI_DINO_BER_CNT_MSBr_t ber_cnt_msb; 
    PHYMOD_MEMSET(&dino_live_sts, 0, sizeof(BCMI_DINO_LIVE_STSr_t));
    PHYMOD_MEMSET(&dino_main_sts, 0, sizeof(BCMI_DINO_MAIN_STSr_t));
    PHYMOD_MEMSET(&cl49_ber_sts, 0, sizeof(BCMI_DINO_CL49_BER_STSr_t));
    PHYMOD_MEMSET(&ber_cnt_msb, 0, sizeof (BCMI_DINO_BER_CNT_MSBr_t));

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_LIVE_STSr(pa, &dino_live_sts));
    if (dino_live_sts.v[0] & 0x2) {
        *lock_status &= 1;
    } else {
        *lock_status &= 0;
    }

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MAIN_STSr(pa, &dino_main_sts));
    if (dino_main_sts.v[0] & 0x8) {
        *lock_lost_lh &= 0;
    } else {
        *lock_lost_lh &= 1;
    }
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_CL49_BER_STSr(pa, &cl49_ber_sts));
    ber_count_lsb = (cl49_ber_sts.v[0] & 0x7f);
    ber_count_lsb >>= 1;
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_BER_CNT_MSBr(pa, &ber_cnt_msb));
    ber_count_msb = ber_cnt_msb.v[0];
    *error_count = (ber_count_msb << 6) | ber_count_lsb; 

    return PHYMOD_E_NONE;
}

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

#define  MADURA_EXCLUDE_STATIC_FUNCTION_DECLARATIONS

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_reg.h>
#include "bcmi_madura_defs.h"
#include "madura_cfg_seq.h"
#include "madura_diag_seq.h"
#include "madura_serdes/falcon2_madura_src/falcon2_madura_functions.h"
#include "madura_serdes/falcon2_madura_src/falcon2_madura_internal.h"
#include "madura_serdes/common2/srds_api_enum.h"

/*  #define DEB */

int disable_hw_state_machine(const phymod_access_t *pa )
{
    /*Disable HW state machine when there is no line side traffic for SA */
    uint32_t data = 0, rv = 0;
    rv = PHYMOD_BUS_WRITE(pa,0x18000,0x501);
    rv = PHYMOD_BUS_READ(pa,0x1be00,&data);
     data &= ~(0xc);
     data |= 0x8;
    rv = PHYMOD_BUS_WRITE(pa,0x18000,0x50f);
    rv = PHYMOD_BUS_WRITE(pa,0x1be00,data);
    return rv;
}

int _madura_phymod_prbs_poly_to_serdes_prbs_poly(uint16_t phymod_poly, uint16_t *serdes_poly)
{
    switch (phymod_poly) {
        case phymodPrbsPoly7:
            *serdes_poly = PRBS_7;
        break;
        case phymodPrbsPoly9:
            *serdes_poly = PRBS_9;
        break;
        case phymodPrbsPoly11:
            *serdes_poly = PRBS_11;
        break;
        case phymodPrbsPoly15:
            *serdes_poly = PRBS_15;
        break;
        case phymodPrbsPoly23:
            *serdes_poly = PRBS_23;
        break;
        case phymodPrbsPoly31:
            *serdes_poly = PRBS_31;
        break;
        case phymodPrbsPoly58:
            *serdes_poly = PRBS_58;
        break;
        default: 
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}
int _madura_serdes_prbs_poly_to_phymod_prbs_poly(uint16_t serdes_poly, phymod_prbs_poly_t *phymod_poly)
{
    switch (serdes_poly) {
        case PRBS_7:
            *phymod_poly = phymodPrbsPoly7;
        break; 
        case PRBS_9:
            *phymod_poly = phymodPrbsPoly9;
        break; 
        case PRBS_11:
            *phymod_poly = phymodPrbsPoly11;
        break; 
        case PRBS_15:
            *phymod_poly = phymodPrbsPoly15;
        break; 
        case PRBS_23:
            *phymod_poly = phymodPrbsPoly23;
        break; 
        case PRBS_31:
            *phymod_poly = phymodPrbsPoly31;
        break; 
        case PRBS_58:
            *phymod_poly = phymodPrbsPoly58;
        break; 
        default: 
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}


int _madura_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
    /*function to select prbs polynomial mode and inversion*/
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0 ;
    uint16_t prbs_poly = 0;
    const phymod_access_t *pa = &phy->access;
 
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        _madura_phymod_prbs_poly_to_serdes_prbs_poly (prbs->poly, &prbs_poly)));

    max_lane = (ip == MADURA_FALCON_CORE) ? 
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;
    
    MADURA_GET_CORE_SLICE_INFO(pa, ip, config);

    PHYMOD_DEBUG_VERBOSE(("IP:%s Max_lane:%d lanemask:0x%x flags=%x\n", 
                (ip == MADURA_FALCON_CORE)?"FALCON":"FALCON_2X", max_lane, lane_mask, flags));

        for (lane = 0; lane < max_lane; lane ++) {
            if (lane_mask & (1 << lane)) {
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));

                if (flags == 0 || PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
		     /*enable inversion on config tx is causing problem for lanes not to lock because of inverted lanes. Also same invert variable is used to invert				 lanes in two different registers. what it idealy should be ?*/
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        falcon2_madura_config_tx_prbs(pa, prbs_poly, (uint8_t)prbs->invert)));
                }
                if (PHYMOD_PRBS_DIRECTION_RX_GET(flags) || flags == 0) {
                    /* Enabling PRBS CHECKER */
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        falcon2_madura_config_rx_prbs(
                            pa, prbs_poly, 0, (uint8_t)prbs->invert)));
#ifdef DEB
 	 	 	uint32_t data1;
		 	PHYMOD_BUS_READ(pa,0x1d161,&data1);
		 	printf("[--d161-%x--]\n",data1);
#endif
                }
	    } else {
		    PHYMOD_DEBUG_VERBOSE(("Lane mask not Fine \n"));
	    }
	}

    PHYMOD_FREE(config.device_aux_modes);
    MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);

    return PHYMOD_E_NONE;
}

int _madura_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags, uint32_t enable)
{
    /*Function to enable prbs_tx && enable prbs_cfg_rx ralated fields*/
    phymod_phy_inf_config_t config;
    uint16_t ip = 0,side=-1;
    uint16_t lane = 0, max_lane = 0, dig_lpbk_en = 0;
    uint16_t lane_mask = 0;
    const phymod_access_t *pa = &phy->access;
#ifdef DEB
    uint32_t data1;
#endif	

   /*Disable HW state machine when there is no line side traffic for SA */
    disable_hw_state_machine(pa);

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);
    max_lane = (ip == MADURA_FALCON_CORE) ? 
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_CORE_SLICE_INFO(pa, ip, config );
    MADURA_GET_INTF_SIDE(pa,side);
 
    PHYMOD_DEBUG_VERBOSE(("\n IP:%s side=%d Max_lane:%d lanemask:0x%x  enable=%x\n", 
                (ip == MADURA_FALCON_CORE)?"FALCON":"FALCON_2X", side,max_lane, lane_mask, enable));

        for (lane = 0; lane < max_lane; lane ++) {
            if (lane_mask & (1 << lane)) {/*if starts*/
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));

		/*Remove system side fix to enable generation without line side traffic. To be handled in application by generating traffic on line side*/
                /*wr_falcon2_madura_pmd_tx_disable_frc(1);*/ 

                if (flags == 0 || PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        falcon2_madura_tx_prbs_en(pa, enable)));
#ifdef DEB
		 	PHYMOD_BUS_READ(pa,0x1d171,&data1);
		 	printf("[--d171-%x--]\n",data1);
#endif		   
                }

		    if (PHYMOD_PRBS_DIRECTION_RX_GET(flags) || flags == 0) {
			    /* Enabling PRBS CHECKER SELF SYNC HYS*/
			    if (enable) {
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        wr_falcon2_madura_prbs_chk_clk_en_frc_on(1)));
				    /* Check for digital loopback enable or not */
				    ESTM(dig_lpbk_en = rd_falcon2_madura_dig_lpbk_en());
				    /* Do not enable prbs_chk_en_auto_mode when digital lpbk is set */
				    if (!dig_lpbk_en) {
                        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                            wr_falcon2_madura_prbs_chk_en_auto_mode(1)));
				    }
			    } else {
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
						    wr_falcon2_madura_prbs_chk_clk_en_frc_on(0)));
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
						    wr_falcon2_madura_prbs_chk_en_auto_mode(0)));
			    }
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
					    falcon2_madura_rx_prbs_en(pa, enable)));
#ifdef DEB
		 	PHYMOD_BUS_READ(pa,0x1d161,&data1);
		 	printf("[--d161-%x--]\n",data1);
#endif
		    }
	    } else {
		    PHYMOD_DEBUG_VERBOSE(("Lane mask not Fine: lane_mask=%x lane=%x \n",lane_mask,lane));
	    }
	    /*for loop ends*/
	}

    PHYMOD_FREE(config.device_aux_modes);
    MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);
    return PHYMOD_E_NONE;
}

int _madura_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags, uint32_t *enable)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0, lane_mask = 0;
    uint8_t gen_en = 0, ckr_en = 0;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);
    max_lane = (ip == MADURA_FALCON_CORE) ? 
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_DEBUG_VERBOSE(("%s :: IP:%s Max_lane:%d lanemask:0x%x \n", FUNCTION_NAME(), 
                (ip == MADURA_FALCON_CORE)?"FALCON":"FALCON_2X", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if ((lane_mask & (1 << lane))) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));

            if (flags == 0 || PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    falcon2_madura_get_tx_prbs_en(pa, &gen_en)));
                    *enable = gen_en;
            }
            if (flags == 0 || PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    falcon2_madura_get_rx_prbs_en(pa, &ckr_en)));
                    *enable = ckr_en;
            }
            break;/*one lane at a time*/
        }
    }
    if (flags == 0) {
        *enable = ckr_en & gen_en;
    } 
    PHYMOD_FREE(config.device_aux_modes);
    MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);
    return PHYMOD_E_NONE;
}

int _madura_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0, lane_mask;
    uint8_t data = 0;
    enum srds_prbs_checker_mode_enum temp = 0;
    enum srds_prbs_polynomial_enum serdes_poly = 0;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);
    max_lane = (ip == MADURA_FALCON_CORE) ? 
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_DEBUG_VERBOSE(("%s :: IP:%s Max_lane:%d lanemask:0x%x flags=%x\n", FUNCTION_NAME(), 
                (ip == MADURA_FALCON_CORE)?"FALCON":"FALCON_2X", max_lane, lane_mask,flags));

    for (lane = 0; lane < max_lane; lane ++) {
        if ((lane_mask & (1 << lane))) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));

            if (flags == 0 || PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    falcon2_madura_get_tx_prbs_config(pa, &serdes_poly, &data)));
                prbs->invert = data;
            }                                                                     
            if (flags == 0 || PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {              
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    falcon2_madura_get_rx_prbs_config(pa, &serdes_poly, &temp, &data)));
                    prbs->invert = data;                                          
                }
            break;/*one lane enough to get config values*/
	}
    }

    PHYMOD_DEBUG_VERBOSE(("Checker POLY:%d\n", serdes_poly));
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        _madura_serdes_prbs_poly_to_phymod_prbs_poly(serdes_poly, &prbs->poly)));

    PHYMOD_FREE(config.device_aux_modes);
    MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);

    return PHYMOD_E_NONE;
}

int _madura_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
    /*Read prbs check register field*/
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0, lane_mask = 0;
    uint8_t data = 0;
    uint32_t err_cnt = 0;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);
    max_lane = (ip == MADURA_FALCON_CORE) ? 
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;
    PHYMOD_USLEEP(100000);
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n",  __func__,
                (ip == MADURA_FALCON_CORE)?"FALCON":"FALCON_2X", max_lane, lane_mask));
    prbs_status->prbs_lock = 1;
    prbs_status->prbs_lock_loss = 1;
    for (lane = 0; lane < max_lane; lane ++) {
        if ((lane_mask & (1 << lane))) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));

            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                falcon2_madura_prbs_chk_lock_state(pa, &data)));
                prbs_status->prbs_lock &= data;
#ifdef DEB
		uint32_t data1 = 0;
		 	PHYMOD_BUS_READ(pa,0x1d169,&data1);
		 	printf("[--d169-%x--]\n",data1);
#endif
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                falcon2_madura_prbs_err_count_state(pa, &err_cnt, &data)));

            prbs_status->prbs_lock_loss &= data;
            prbs_status->error_count |= err_cnt;
        }
    }

    PHYMOD_FREE(config.device_aux_modes);
    MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);
    return PHYMOD_E_NONE;
}

int _madura_phy_display_eyescan(const phymod_access_t *pa)
{
    phymod_phy_inf_config_t config;
    uint16_t ip=0;
    uint16_t intf_side=0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);
    MADURA_GET_INTF_SIDE(pa, intf_side); 
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_DEBUG_VERBOSE(("%s::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
                (intf_side == MADURA_IF_LINE)?"FALCON2X":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane++) {
        if ((lane_mask & (1 << lane))) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));
            
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                falcon2_madura_display_lane_state_hdr(pa)));

            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                falcon2_madura_display_lane_state(pa)));

            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                falcon2_madura_display_eye_scan(pa)));
        }
    }
    PHYMOD_FREE(config.device_aux_modes);

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_SLICEr(pa, slice_reg));

    return PHYMOD_E_NONE;
}


int _madura_phy_diagnostics_get(const phymod_access_t *pa, phymod_phy_diagnostics_t* diag)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;
    falcon2_madura_lane_state_st state;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);
    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_DEBUG_VERBOSE(("%s::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
                (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane++) {
        if ((lane_mask & (1 << lane))) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));

            PHYMOD_DEBUG_VERBOSE(("Phy Diagnostics for Lane:%d \n",lane));
            /* set the core to get the master /slave */
            /* system side always master line side considered as slave*/
                PHYMOD_MEMSET(&state, 0, sizeof(falcon2_madura_lane_state_st));
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    _falcon2_madura_read_lane_state(pa, &state)));

                diag->signal_detect = state.sig_det;
                diag->osr_mode = state.osr_mode.tx;
                diag->rx_lock = state.rx_lock;
                diag->tx_ppm = state.tx_ppm;
                diag->clk90_offset = state.clk90;
                diag->clkp1_offset = state.clkp1;
                diag->p1_lvl = state.p1_lvl;
                diag->dfe1_dcd = state.dfe1_dcd;
                diag->dfe2_dcd = state.dfe2_dcd;
                diag->slicer_offset.offset_pe = state.pe;
                diag->slicer_offset.offset_ze = state.ze;
                diag->slicer_offset.offset_me = state.me;
                diag->slicer_offset.offset_po = state.po;
                diag->slicer_offset.offset_zo = state.zo;
                diag->slicer_offset.offset_mo = state.mo;
                diag->eyescan.heye_left = state.heye_left;
                diag->eyescan.heye_right = state.heye_right;
                diag->eyescan.veye_upper = state.veye_upper;
                diag->eyescan.veye_lower = state.veye_lower;
                diag->link_time = state.link_time;
                diag->pf_main = state.pf_main;
                diag->pf_hiz = state.pf_hiz;
                diag->pf_bst = state.pf_bst;
                diag->pf_low = 0; /* not availble in serdes api*/
                diag->pf2_ctrl = state.pf2_ctrl;
                diag->vga = state.vga;
                diag->dc_offset = state.dc_offset;
                diag->p1_lvl_ctrl = state.p1_lvl_ctrl;
                diag->dfe1 = state.dfe1;
                diag->dfe2 = state.dfe2;
                diag->dfe3 = state.dfe3;
                diag->dfe4 = state.dfe4;
                diag->dfe5 = state.dfe5;
                diag->dfe6 = state.dfe6;
                diag->txfir_pre = state.txfir_pre;
                diag->txfir_main = state.txfir_main;
                diag->txfir_post1 = state.txfir_post1;
                diag->txfir_post2 = state.txfir_post2;
                diag->txfir_post3 = state.txfir_post3;
                diag->tx_amp_ctrl = 0; /* not availble in serdes api*/
                diag->br_pd_en = state.br_pd_en;
                break;
        }
    }
    PHYMOD_FREE(config.device_aux_modes);
    MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);

    return PHYMOD_E_NONE;
}


int _madura_ber_proj_lane_speed_get(phymod_phy_inf_config_t *config, uint16_t ip, uint32_t *speed)
{

    MADURA_DEVICE_AUX_MODE_T  *aux;
    aux = (MADURA_DEVICE_AUX_MODE_T*) config->device_aux_modes;

    if (config->data_rate == MADURA_SPD_100G || config->data_rate == MADURA_SPD_106G) {
            *speed = 25781250;
            if (PHYMOD_INTF_MODES_OTN_GET(config)) {
                *speed = 27952500;
            } else if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                *speed = 27343750;
            }
        } else if ((config->data_rate == MADURA_SPD_40G ) && (!aux->pass_thru)) {
            *speed = 20625000;
            if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                *speed = 21875000;
            }
        } else if ((config->data_rate == MADURA_SPD_20G ) && (!aux->pass_thru)) {
            *speed = 20625000;
            if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                *speed = 21875000;
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


int _madura_ber_proj(const phymod_phy_access_t *phy, const phymod_phy_eyescan_options_t* eyescan_options)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;
    const phymod_access_t *pa = &phy->access;
#ifdef SERDES_API_FLOATING_POINT
    uint32_t speed;
    USR_DOUBLE speedD;
#endif

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);
    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_DEBUG_VERBOSE(("%s::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
                (ip == MADURA_FALCON_CORE)?"FALCON":"FALCON_2X", max_lane, lane_mask));

    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            (falcon2_madura_display_core_state_hdr())));
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            (falcon2_madura_display_core_state_line(pa))));

    for (lane = 0; lane < max_lane; lane++) {
        if ((lane_mask & (1 << lane))) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));

		     PHYMOD_DEBUG_VERBOSE(("eye_margin_proj for Lane:%d \n",lane));
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    (falcon2_madura_display_lane_state_hdr(pa))));
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    (falcon2_madura_display_lane_state(pa))));
#ifdef SERDES_API_FLOATING_POINT
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    (_madura_ber_proj_lane_speed_get(&config, ip, &speed))));
                speedD = speed * 1000.0;
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    (falcon2_madura_eye_margin_proj(pa, speedD, eyescan_options->ber_proj_scan_mode,
                               eyescan_options->ber_proj_timer_cnt, eyescan_options->ber_proj_err_cnt))));
#else
                PHYMOD_DIAG_OUT(("BER PROJ needs SERDES_API_FLOATING_POINT define to operate\n"));
		PHYMOD_FREE(config.device_aux_modes);
		return PHYMOD_E_RESOURCE;
#endif

	}
    }

    PHYMOD_FREE(config.device_aux_modes);
    MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);

    return PHYMOD_E_NONE;
}


/*----------------------------------------------------------------------
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : dino_cfg_seq.h
 * Description: c functions implementing Tier1
 *---------------------------------------------------------------------*/
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
*/


#ifndef DINO_DIAG_SEQ_H 
#define DINO_DIAG_SEQ_H
#include <phymod/phymod_acc.h>

int _dino_phymod_prbs_poly_to_serdes_prbs_poly(uint16_t phymod_poly, uint16_t *serdes_poly);

int _dino_serdes_prbs_poly_to_phymod_prbs_poly(uint16_t serdes_poly,  phymod_prbs_poly_t *phymod_poly);

int _dino_phy_prbs_config_set(const phymod_access_t* pa, uint32_t flags, uint16_t if_side, uint16_t lane, const phymod_prbs_t* prbs);

int _dino_phy_prbs_enable_set(const phymod_access_t* pa, uint32_t flags, uint16_t if_side, uint16_t lane, uint32_t enable);

int _dino_phy_prbs_enable_get(const phymod_access_t* pa, uint32_t flags, uint16_t if_side, uint16_t lane, uint32_t *enable);

int _dino_phy_prbs_config_get(const phymod_access_t* pa,
                              uint32_t flags,
                              uint16_t if_side,
                              uint16_t lane,
                              phymod_prbs_t* prbs,
                              uint16_t* serdes_poly, 
                              uint16_t* chkr_mode);

int _dino_phy_prbs_status_get(const phymod_access_t* pa, uint32_t flags, uint16_t if_side, uint16_t lane, phymod_prbs_status_t* prbs_status);

int _dino_phy_display_eyescan(const phymod_access_t* pa, uint16_t if_side, uint16_t lane);

int _dino_ber_proj(const phymod_phy_access_t *phy, uint16_t if_side, uint16_t lane, const phymod_phy_eyescan_options_t* eyescan_options);

int _dino_phy_diagnostics_get(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, phymod_phy_diagnostics_t* diag);

int _dino_core_diagnostics_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_core_diagnostics_t* diag);

int _dino_ber_proj_lane_speed_get(const phymod_phy_access_t *phy, phymod_phy_inf_config_t *config, uint16_t if_side, uint16_t lane, uint32_t *speed);

int _dino_phy_link_mon_enable_set(const phymod_access_t* pa, uint32_t enable);

int _dino_phy_link_mon_enable_get(const phymod_access_t* pa, uint32_t *enable);

int _dino_phy_link_mon_status_get(const phymod_access_t* pa, uint32_t* lock_status, uint32_t* lock_lost_lh, uint32_t* error_count);

#endif /* DINO_DIAG_SEQ_H */

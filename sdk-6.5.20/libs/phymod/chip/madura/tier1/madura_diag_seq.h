/*----------------------------------------------------------------------
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : madura_cfg_seq.h
 * Description: c functions implementing Tier1
 *---------------------------------------------------------------------*/
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
*/


#ifndef MADURA_DIAG_SEQ_H 
#define MADURA_DIAG_SEQ_H
#include <phymod/phymod_acc.h>

int _madura_phymod_prbs_poly_to_serdes_prbs_poly(uint16_t phymod_poly, uint16_t *serdes_poly);

int _madura_serdes_prbs_poly_to_phymod_prbs_poly(uint16_t serdes_poly,  phymod_prbs_poly_t *phymod_poly);

int _madura_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs);

int _madura_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags, uint32_t enable);

int _madura_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags, uint32_t *enable);

int _madura_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs);

int _madura_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status);

int _madura_phy_display_eyescan(const phymod_access_t* phy);

int _madura_ber_proj(const phymod_phy_access_t *phy, const phymod_phy_eyescan_options_t* eyescan_options);

int _madura_phy_diagnostics_get(const phymod_access_t *pa, phymod_phy_diagnostics_t* diag);

#endif /* MADURA_DIAG_SEQ_H */

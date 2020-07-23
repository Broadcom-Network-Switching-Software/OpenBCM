/*----------------------------------------------------------------------
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : sesto_cfg_seq.h
 * Description: c functions implementing Tier1
 *---------------------------------------------------------------------*/
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
*/


#ifndef SESTO_DIAG_SEQ_H 
#define SESTO_DIAG_SEQ_H
#include <phymod/phymod_acc.h>

int _sesto_phymod_prbs_poly_to_serdes_prbs_poly(uint16_t phymod_poly, uint16_t *serdes_poly);

int _sesto_serdes_prbs_poly_to_phymod_prbs_poly(uint16_t serdes_poly,  phymod_prbs_poly_t *phymod_poly);

int _sesto_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs);

int _sesto_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags, uint32_t enable);

int _sesto_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags, uint32_t *enable);

int _sesto_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs);

int _sesto_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status);

int _sesto_phy_display_eyescan(const phymod_phy_access_t* phy);

int _sesto_ber_proj(const phymod_phy_access_t *phy, const phymod_phy_eyescan_options_t* eyescan_options);

int _sesto_phy_diagnostics_get(const phymod_phy_access_t *phy, phymod_phy_diagnostics_t* diag);

int _sesto_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag);

#endif /* SESTO_DIAG_SEQ_H */

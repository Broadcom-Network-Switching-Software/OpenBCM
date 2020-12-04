/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        fabric.h
 * Purpose:     FABRIC internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_FABRIC_H_
#define   _BCM_INT_DPP_FABRIC_H_

#include <bcm/debug.h>

#include <soc/dpp/dpp_config_defs.h>

/*
 * Defines
 */
#define BCM_FABRIC_MODID_IS_VALID(unit, modid)  \
        ((modid >= 0) && (modid < SOC_DPP_DEFS_GET(unit, modid_max_valid))) /* True if modid is between 0 and the max 
                                                                              limit of FAP's in the device  */
#define BCM_FABRIC_NUM_OF_LINKS_IS_VALID(unit, num_of_links)  \
        ((num_of_links >= 0) && (num_of_links <= SOC_DPP_DEFS_GET(unit, nof_fabric_links))) /* True if num_of_links is between 0 and the max 
                                                                                            fabric links in the device  */
#define BCM_FABRIC_PCP_MODE_IS_VALID(pcp_mode)  \
        ((pcp_mode >= 0) && (pcp_mode <= 2))        /* True if pcp_mode is one of the 
                                                        three supported pcp_modes (0-2) */
#define BCM_FABRIC_MC_ID_IS_VALID(unit, mc_id)  \
        ((mc_id >= 0) && (mc_id <= (SOC_DPP_DEFS_GET(unit, fabric_mesh_mc_id_max_valid))))    /* True if mc_id is between 0 and the max
                                                                                               limit of mc id's in mesh mode */
#define BCM_FABRIC_RCI_HIGH_SCORE_IS_VALID(high_score) \
        (((int)high_score >= 0) && (high_score <= 0x1FFF))

#define BCM_FABRIC_RCI_SEVERITY_FACTOR_IS_VALID(factor) \
        (((int)factor >= 0) && (factor <= 0x8))

#define BCM_FABRIC_RCI_THRESHOLD_IS_VALID(threshold) \
        (((int)threshold >= 0) && (threshold <= 0x1FFF))

#define BCM_FABRIC_RCI_CONGESTED_LINKS_THRESHOLD_IS_VALID(congested_th) \
        (((int)congested_th >= 0) && (congested_th <= 0X3F))


/*
 * External functions
 */
int
bcm_petra_fabric_enhance_ports_get(int unit, bcm_core_t core, int *nbr_ports, int *ports);
int
bcm_petra_fabric_multicast_scheduler_mode_get(int unit, int core_id, int *mode);
#endif /* _BCM_INT_DPP_FABRIC_H_ */

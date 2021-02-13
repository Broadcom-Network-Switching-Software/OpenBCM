/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *         
 *     
 * DO NOT EDIT THIS FILE!
 */

#ifndef _TSCF_DIAGNOSTICS_H__H_
#define _TSCF_DIAGNOSTICS_H__H_

#include <phymod/phymod_definitions.h>
/*Set\get slicer position*/
int tscf_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position);
int tscf_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position);

/*Get slicer range limitation*/
int tscf_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max);

/*set\get PRBS configuration*/
int tscf_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs);
int tscf_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs);

/*Set\get PRBS enable state*/
int tscf_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable);
int tscf_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable);

/*Get PRBS Status*/
int tscf_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status);

/*Set\get pattern state*/
int tscf_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern);
int tscf_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern);

/*Set\get pattern state*/
int tscf_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable, const phymod_pattern_t* pattern);
int tscf_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable);

/*Get core diagnostics information*/
int tscf_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag);

/*Get phy diagnostics information*/
int tscf_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char* type);

/*Get phy diagnostics information*/
int tscf_phy_pcs_info_dump(const phymod_phy_access_t* phy, const char* type);

/*Get CL74 FEC Corrected Codeword Counter*/
int tscf_phy_fec_correctable_counter_get(const phymod_phy_access_t* phy, uint32_t* count);

/*Get CL74 FEC Uncorrected Codeword Counter*/
int tscf_phy_fec_uncorrectable_counter_get(const phymod_phy_access_t* phy, uint32_t* count);

/*Get PHY Stat Counter*/
int tscf_phy_stat_get(const phymod_phy_access_t* phy, phymod_phy_stat_t* stat);

/*Get CL91 FEC Corrected Codeword Counter*/
int tscf_phy_fec_cl91_correctable_counter_get(const phymod_phy_access_t* phy, uint32_t* count);

/*Get CL91 FEC Uncorrected Codeword Counter*/
int tscf_phy_fec_cl91_uncorrectable_counter_get(const phymod_phy_access_t* phy, uint32_t* count);

/* Get RS FEC symbol error count.*/
int tscf_phy_rsfec_symbol_error_counter_get(const phymod_phy_access_t* phy, int max_count, int* actual_count, uint32_t* error_count);

/*Control one time fec error injection.*/
int tscf_phy_fec_error_inject_set(const phymod_phy_access_t* phy, uint16_t error_control_map, phymod_fec_error_mask_t bit_error_mask);
int tscf_phy_fec_error_inject_get(const phymod_phy_access_t* phy, uint16_t* error_control_map, phymod_fec_error_mask_t* bit_error_mask);

#endif /*_TSCF_DIAGNOSTICS_H_*/

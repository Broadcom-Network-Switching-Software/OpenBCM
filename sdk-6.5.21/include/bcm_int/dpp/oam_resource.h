/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * OAM Resources module (traps, profiles, IDs, ...)
 */
#ifndef _BCM_INT_DPP_OAM_RESOURCE_H_
#define _BCM_INT_DPP_OAM_RESOURCE_H_

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/oam.h>
#include <bcm/bfd.h>
#include <shared/hash_tbl.h>
#include <shared/swstate/sw_state_hash_tbl.h>

#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/PPC/ppc_api_oam.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>


/* Setting Invalid value as 32 bit max. */
#define _BCM_DPP_OAM_ERROR_TRAP_ID_INVALID 0xffffffff

int _bcm_dpp_oam_error_trap_init(int unit);
int _bcm_dpp_bfd_mep_id_alloc(int unit, uint32 flags, uint32 *mep_index);
/*
 * Function:
 *      _bcm_dpp_oam_mep_id_alloc
 *
 * Purpose:
 *      Allocate an index in the MEP-DB
 *
 * Parameters:
 *      unit       - (IN)     SOC unit number.
 *      flags      - (IN)     SW_STATE_RES_ALLOC_WITH_ID or 0
 *      is_short   - (IN)     1 indicates MAID uses short MA format
 *      is_maid_48 - (IN)     1 indicates MAID uses 48B MA format. used only for Jericho\Arad+ in order to allocate
 *                            specifec MEP ID which is valid for 48B maid(every 8th MEP)
 *      type       - (IN)     BCM_DPP_AM_OAM_MEP_DB_ENTRY_TYPE_MEP or 0. Indicates if this is MEP entry or a LM/DM entry
 *      mep_index  - (IN/OUT) Index in the MEP-DB
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      When WITH_ID flag is set, is_short parameter is ignored
 */
int _bcm_dpp_oam_mep_id_alloc(int unit, uint32 flags, uint8 is_short, uint8 is_maid_48, uint8 type, uint32 *mep_index);
int _bcm_dpp_oam_bfd_mep_id_is_alloced(int unit, uint32 mep_id);
int _bcm_dpp_oam_bfd_mep_id_dealloc(int unit, uint32 mep_id);

int _bcm_dpp_oam_mirror_data_free(int unit, uint32 trap_code);

int _bcm_dpp_oam_free_mirror_profile_find(int unit, uint32 * free_mirror_profile, uint32 cpu_trap_code, uint8 *found_match);

int _bcm_oam_trap_and_mirror_profile_set(int unit,
										 SOC_PPC_OAM_TRAP_ID trap_index,
										 SOC_PPC_OAM_UPMEP_TRAP_ID upmep_trap_index,
										 bcm_rx_trap_config_t * trap_config,
										 uint32 trap_code,
										 uint32 trap_code_upmep,
										 SOC_PPC_OAM_MIRROR_ID mirror_ndx,
										 uint8 set_upmep);

int _bcm_oam_trap_and_mirror_profile_up_lbm_set(int unit, bcm_rx_trap_config_t * trap_config);

int _bcm_oam_default_profile_get(int unit,
								  SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data,
								  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry,
								  uint8 is_accelerated,
								  uint8 is_default,
                                  uint8 is_upmep);

/* Configure default accelerated/non-accelerated profile for RFC6374 endpoints */
int _bcm_oam_rfc6374_default_profile_get(int unit,
                                         SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data,
                                         SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry,
                                         uint8 is_accelerated);

/* Gets the trap codes and trap strength from the destination gports, Sets mirror profile if required, Handles advanced egress snooping,
   Sets OAMP RX trap if required
   IN: unit, id, action, is_rfc6374_lm, is_rfc6374_dm
   OUT: trap_code, trap_code2, trap_strength(taken from 'destination' gport similar to trap_code) */
int _bcm_dpp_oam_bfd_trap_code_get_and_prepare_for_action(int unit, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY* classifier_mep_entry,
                                                          uint8 is_rfc6374_lm, uint8 is_rfc6374_dm,
                                                          bcm_gport_t destination, bcm_gport_t destination2,
                                                          uint32* trap_code, uint32* trap_code2);

/* OAMP errors trap allocation */
int _bcm_dpp_oam_error_trap_allocate(int unit, bcm_rx_trap_t trap_type, uint32 oamp_error_trap_id);
int _bcm_dpp_oam_error_trap_destroy(int unit, uint32 oamp_error_trap_id);
int _bcm_dpp_oam_error_trap_set(int unit, uint32 oamp_error_trap_id, bcm_gport_t dest_port);
int _bcm_dpp_oam_error_trap_get(int unit, uint32 oamp_error_trap_id, bcm_gport_t * dest_port);

#endif  /* _BCM_INT_DPP_OAM_RESOURCE_H_ */

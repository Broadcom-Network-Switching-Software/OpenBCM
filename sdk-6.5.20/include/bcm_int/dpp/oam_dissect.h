/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * OAM input analysis and validation internal header
 */

#ifndef _BCM_INT_DPP_OAM_DISSECT_H_
#define _BCM_INT_DPP_OAM_DISSECT_H_

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

/*
 * Defines and macros
 */

/* Trap code used for snooping to the CPU while keeping the original system-headers */
#define _BCM_OAM_TRAP_CODE_CPU_KEEP_ORIG_HEADERS(_trap_code) ((_trap_code)==SOC_PPC_TRAP_CODE_OAM_CPU_SNOOP_UP)

#define _BCM_OAM_IS_SERVER_CLIENT(endpoint_info) (endpoint_info->tx_gport==BCM_GPORT_INVALID && endpoint_info->remote_gport !=BCM_GPORT_INVALID)
#define _BCM_OAM_IS_SERVER_SERVER(endpoint_info) (endpoint_info->tx_gport!=BCM_GPORT_INVALID && endpoint_info->remote_gport !=BCM_GPORT_INVALID)

#define _BCM_OAM_IS_ENDPOINT_RFC6374(p_endpoint_info) \
                          (((p_endpoint_info)->type == bcmOAMEndpointTypeMplsLmDmPw) || \
                           ((p_endpoint_info)->type == bcmOAMEndpointTypeMplsLmDmLsp) || \
                           ((p_endpoint_info)->type == bcmOAMEndpointTypeMplsLmDmSection))

#define _BCM_OAM_IS_MEP_ID_DEFAULT(_unit, _epid)                                \
               (SOC_IS_ARAD_B1_AND_BELOW(_unit)                                 \
                  ? ((_epid) == -1)                                             \
                  : ((!SOC_IS_JERICHO_B0_AND_ABOVE(_unit))  \
                      ? (((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS0)         \
                         ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS1)       \
                         ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS2)       \
                         ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS3)       \
                         ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS0))       \
/*QAX and above*/     : (((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS0)         \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS1)      \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS2)      \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS3)      \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS0)       \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS1)       \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS2)       \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS3))))

#define _BCM_OAM_IS_MEP_ID_EGRESS_UNSUPPORTED_DEFAULT(_unit, _epid)                                \
                      ((SOC_IS_JERICHO_B0_AND_ABOVE(_unit)) &&  \
                         (((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS1)       \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS2)       \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS3)))

/* Is endpoint_info struct represents an accelerated endpoint */
#define _BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(p_endpoint_info) \
                          (((p_endpoint_info)->opcode_flags & BCM_OAM_OPCODE_CCM_IN_HW) == BCM_OAM_OPCODE_CCM_IN_HW)

/* Is endpoint_info struct represents an up-MEP */
#define _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(p_endpoint_info) \
                          (((p_endpoint_info)->flags & BCM_OAM_ENDPOINT_UP_FACING) == BCM_OAM_ENDPOINT_UP_FACING)

/* Is endpoint_info struct represents a MIP */
#define _BCM_OAM_DISSECT_IS_ENDPOINT_MIP(p_endpoint_info) \
                          (((p_endpoint_info)->flags & BCM_OAM_ENDPOINT_INTERMEDIATE) == BCM_OAM_ENDPOINT_INTERMEDIATE)

/* Is classifier struct represents an accelerated endpoint */
#define _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(p_classifier_mep_entry) \
                          (((p_classifier_mep_entry)->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) == SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)

/* Is classifier struct represents an up-MEP */
#define _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(p_classifier_mep_entry) \
                          (((p_classifier_mep_entry)->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) == SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP)

/* Is classifier struct represents a MIP */
#define _BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(p_classifier_mep_entry) \
                          (((p_classifier_mep_entry)->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT) == 0)

/* Is classifier struct Down MEP injection endpoint */
#define _BCM_OAM_DISSECT_IS_CLASSIFIER_DOWN_MEP_INJECTION(p_classifier_mep_entry) \
                          (((p_classifier_mep_entry)->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DOWN_MEP_INJECTION) == SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DOWN_MEP_INJECTION)



#define _BCM_OAM_DISSECT_IS_DEFAULT_ENDPOINT_UPMEP(endpoint) \
                         ((endpoint)->id == BCM_OAM_ENDPOINT_DEFAULT_EGRESS0 || (endpoint)->id <= BCM_OAM_ENDPOINT_DEFAULT_EGRESS1 \
                                      || (endpoint)->id <= BCM_OAM_ENDPOINT_DEFAULT_EGRESS2 || (endpoint)->id <= BCM_OAM_ENDPOINT_DEFAULT_EGRESS3 )


/* Classification is done through the IHB_OAMB table in the following cases:
   1. Accelerated endpoint
   2. "Multiple endpoint on LIF" (Arad+), Endpoint only
   3. Server on the client side*/
#define _BCM_OAM_DISSECT_CLASSIFICATION_BY_OAM2(unit, p_classifier_mep_entry, endpoint_id)\
           (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(p_classifier_mep_entry) ||  _BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint_id) \
           || (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && !_BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(p_classifier_mep_entry)) )

/* Check valid channel_type range - Channel type field is 16 bit */
#define _BCM_OAM_DISSECT_MPLS_TP_CHANNEL_TYPE_IN_VALID_RANGE(channel_type)\
                        (channel_type >= 0 && channel_type < (1 << 16))

/*
 * Function declaration
 */

/* Validity checks used by default and non default endpoints alike. */
int _bcm_oam_endpoint_validity_checks_all(int unit, bcm_oam_endpoint_info_t *endpoint_info);

/* Validity checks for all types of non-default endpoints */
int _bcm_oam_generic_endpoint_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info);

/* Validity checks for OAMoETH endpoints */
int _bcm_oam_ethernet_endpoint_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info);

/* Validity checks for OAMoMPLS/PWE endpoints */
int _bcm_oam_mpls_pwe_endpoint_validity_checks(int unit, bcm_oam_endpoint_info_t *endpoint_info);

/* Validity checks for RFC6374 endpoints */
int _bcm_oam_rfc6374_endpoint_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info);

/* perform endpoint create validity check for default endpoint. */
int _bcm_oam_default_endpoint_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info);

/* Validity checks before replacing a local MEP */
int _bcm_oam_endpoint_local_replace_validity_check(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl,
                                                   bcm_oam_endpoint_info_t *existing_endpoint_info,
                                                   SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry);

/* Validity checks before adding a loopback object to an endpoint (Arad+ only) */
int _bcm_oam_loopback_validity_check(int unit, bcm_oam_loopback_t *loopback_ptr,
                                     SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry);

/* Validity checks before adding an action_set */
int _bcm_oam_endpoint_action_set_validity_check(int unit, bcm_oam_endpoint_t id, bcm_oam_endpoint_action_t *action);

/* Validity checks before adding/updating LM/DM entries pointed by MEP
   (QAX style) */
int _bcm_oam_lm_dm_pointed_add_shared_verify(int unit, SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *lm_dm_mep_db_entry,
                                             SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_entry);

/* Validity checks before adding delay entry. */
int _bcm_oam_delay_validity_checks(int unit, const bcm_oam_delay_t *delay_ptr, const SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY* classifier_mep_entry);

/* Validity checks for Y.1711 LM info */
int _bcm_oam_y1711_lm_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info);


/* 
 * This function contains all the sanity 
 * checks required when creating or 
 * renaming maintenance endpoint groups 
 * (MEGs) 
*/ 
int _bcm_oam_group_create_verify(int unit, bcm_oam_group_info_t *group_info);

/* 
 * This function contains all the sanity 
 * checks required for the channel type values 
 * variable in channel_type_rx_set
 * function.
 */ 
int _bcm_oam_mpls_tp_rx_channel_type_values_validity_check (int unit, int num_channel_types, int *list_of_channel_types);

/* 
 * This function contains all the sanity 
 * checks required for the channel type value
 * variable in channel_type_tx_set
 * function.
 */ 
int _bcm_oam_mpls_tp_tx_channel_type_value_validity_check (int unit, int channel_type);

#endif /* _BCM_INT_DPP_OAM_DISSECT_H_ */

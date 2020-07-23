/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mpls.h
 * Purpose:     mpls internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_MPLS_H_
#define   _BCM_INT_DPP_MPLS_H_

#include <bcm/mpls.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>










#define BCM_MPLS_LABEL_MAX  ((1 << 20) - 1)

#define _BCM_DPP_MPLS_LABEL_IN_RANGE(_label_)   \
        (_label_ <= BCM_MPLS_LABEL_MAX)

/* Define MPLS supported types */
#define _BCM_DPP_MPLS_EEDB_TYPES(entry_type) \
  (entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP ||  \
   entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_POP_CMND || \
   entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND)

/* SW state for MPLS */
#define MPLS_INFO(_unit_)   (&_bcm_dpp_mpls_bk_info[_unit_])


typedef struct _bcm_dpp_mpls_bookkeeping_s {
    int         initialized;        /* Set to TRUE when MPLS module initialized */
    int *egress_tunnel_id;
    sal_mutex_t mpls_mutex;        /* Protection mutex. */
} _bcm_dpp_mpls_bookkeeping_t;

extern _bcm_dpp_mpls_bookkeeping_t  _bcm_dpp_mpls_bk_info[BCM_MAX_NUM_UNITS];
extern SOC_PPC_FRWRD_ILM_KEY *_bcm_mpls_traverse_ilm_keys[BCM_MAX_NUM_UNITS];
extern SOC_PPC_FRWRD_DECISION_INFO *_bcm_mpls_traverse_ilm_vals[BCM_MAX_NUM_UNITS];



/* match criteria setting */
int _bcm_dpp_mpls_match_get(int unit, bcm_mpls_port_t *mpls_port, int lif);

int _bcm_dpp_mpls_match_add(int unit, bcm_mpls_port_t *mpls_port, int lif);

int _bcm_dpp_mpls_match_delete(int unit, int lif);

int bcm_petra_mpls_tunnel_initiator_data_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array,
    int vsi_param, /* needed only in case intf is of type EEP*/
    int ll_eep_param, /* needed only in case intf is of type EEP*/
    int is_swap, /* needed only in case intf is of type EEP*/
    int *tunnel_eep
    ); /* needed only in case intf is of type EEP*/

int _bcm_mpls_tunnel_push_profile_alloc(
    int unit,
    uint32 * push_profile,
    uint8 cw,
    bcm_mpls_egress_label_t * label_entry,
    uint8 is_test,
    uint8 with_id,
    uint8 update_existing_profile,
    uint8 set_hw,
    uint8 * is_first,
    uint8 is_second_mpls_label_in_entry);

int _bcm_mpls_tunnel_push_profile_alloc_dummy_profiles(
    int unit,
    uint32 *dummy_profiles_array,
    int nof_dummy_profiles);

int _bcm_mpls_tunnel_initial_additional_push_profiles_alloc(
    int unit);

int _bcm_mpls_tunnel_push_profile_info_get(
    int unit,
    int push_profile,
    uint8 * has_cw,
    bcm_mpls_egress_label_t * label_entry);

/* Inner flags for MPLS termination */
#define _BCM_DPP_MPLS_LIF_TERM_FRR              (0x1)
#define _BCM_DPP_MPLS_LIF_TERM_COUPLING         (0x2)
#define _BCM_DPP_MPLS_LIF_TERM_CONTROL_WORD     (0x4)
#define _BCM_DPP_MPLS_LIF_TERM_ENTROPY          (0x8)
#define _BCM_DPP_MPLS_LIF_TERM_ELI             (0x10)
#define _BCM_DPP_MPLS_LIF_TERM_REJECT_TTL_0    (0x20)
#define _BCM_DPP_MPLS_LIF_TERM_REJECT_TTL_1    (0x40)
#define _BCM_DPP_MPLS_LIF_TERM_SKIP_ETHERNET   (0x80)
#define _BCM_DPP_MPLS_LIF_TERM_CHECK_BOS      (0x100)
/* GAL flag is only relevant for MPLS_PORT */
#define _BCM_DPP_MPLS_LIF_TERM_GAL            (0x200)
#define _BCM_DPP_MPLS_LIF_TERM_EVPN_IML      (0x400)

/* Pertinent to Mpls range action*/
#define _BCM_MPLS_TERM_LABEL_RANGE_TABLE_FULL   (1)
#define _BCM_MPLS_TERM_LABEL_RANGE_EXIST        (2)
#define _BCM_MPLS_TERM_LABEL_RANGE_INTERSECT    (3)
#define _BCM_MPLS_TERM_LABEL_RANGE_VACANT_ENTRY (4)
#define _BCM_MPLS_TERM_LABEL_RANGE_EXIST_IML    (5)
#define _BCM_MPLS_TERM_LABEL_RANGE_NOF_ENTRIES  (8)

/* indicate bos in gport in case of evpn iml */
#define _BCM_DPP_MPLS_LIF_TERM_EVPN_IML_BOS_INDICATION (1<<25)

/* index of outlif profile for first label in EVPN stack*/
#define _BCM_DPP_MPLS_EGRESS_EVPN_OUTLIF_PROFILE_INDEX(unit)  (SOC_IS_JERICHO_PLUS(unit) ? (3) : (1))
#define _BCM_DPP_MPLS_PWE_OUTLIF_PROFILE_INDEX(unit)          (SOC_IS_JERICHO_PLUS(unit) ? (3) : (1))

#define _BCM_DPP_MPLS_FRR_OUTLIF_PROFILE_INDEX(unit)          (SOC_IS_JERICHO_PLUS(unit) ? (5) : (1))

int _bcm_mpls_tunnel_lif_term_profile_exchange(
      int unit,
      SOC_PPC_LIF_ID lif_id,
      uint32 inner_flags,
      uint8 is_pwe_lif,
      uint8 expect_bos, /* Jericho */
      uint8* term_profile);

int _bcm_mpls_tunnel_lif_term_mpls_port_to_flags(
      int unit,
      bcm_mpls_port_t *   mpls_port,
      uint32 *inner_flags
    );

int _bcm_petra_mpls_term_resolve_lif_type_by_index(
    int unit,
    int port,
    bcm_mpls_label_t label,
    uint32 *lif_types);

int _bcm_petra_mpls_term_to_pwe_lif_types(
    int unit,
    uint32 lif_types,
    uint32 *pwe_lif_types);

int _bcm_petra_mpls_tunnel_initiator_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array);

int _bcm_petra_mpls_tunnel_switch_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info);

int _bcm_petra_mpls_tunnel_free_push_profile(
    int unit,
    int eep);

int bcm_petra_mpls_port_free_push_profile(
    int unit,
    int eep);

int _bcm_dpp_mpls_range_action_verify(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high,
    uint32 * vacant_entry_index,
    int *err_code);

int
_bcm_dpp_mpls_port_add(
    int unit,
    bcm_vpn_t   vpn,
    bcm_mpls_port_t *   mpls_port);

#endif /* _BCM_INT_DPP_MPLS_H_ */

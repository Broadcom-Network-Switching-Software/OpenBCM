/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        alloc_mngr_lif.h
 * Purpose:     Resource allocation for lif.
 */

#ifndef  INCLUDE_ALLOC_MNGR_LIF_H
#define  INCLUDE_ALLOC_MNGR_LIF_H

#include <bcm_int/dpp/alloc_mngr_shr.h>

/* Sync LIF start definiations { */
#define _BCM_DPP_AM_SYNC_LIF_INGRESS (1 << 0)
#define _BCM_DPP_AM_SYNC_LIF_EGRESS (1 << 1)

#define _BCM_DPP_AM_INGRESS_LIF_NOF_ENTRIES_PER_BANK (SOC_IS_JERICHO(unit) ? (8*1024) : (4*1024))
#define _BCM_DPP_AM_INGRESS_LIF_BANK_LOW_ID(bank_id) (bank_id*_BCM_DPP_AM_INGRESS_LIF_NOF_ENTRIES_PER_BANK)

#define _BCM_DPP_AM_EGRESS_MAP_ENCAP_INTPRI_COLOR_NOF_ENTRIES (SOC_IS_JERICHO(unit) ? (16) : (0)) /* Num of entries of the COS profile table */

#define _BCM_DPP_AM_EGRESS_ENCAP_NOF_ENTRIES_PER_BANK (SOC_IS_JERICHO(unit) ? (8*1024) : (4*1024))
#define _BCM_DPP_AM_EGRESS_ENCAP_BANK_LOW_ID(bank_id) (bank_id*_BCM_DPP_AM_EGRESS_ENCAP_NOF_ENTRIES_PER_BANK)

#define _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase,phase_id) \
  (SHR_BITSET(bitmap_phase,phase_id))

typedef enum _dpp_am_ingress_lif_phase_e{
      _dpp_am_ingress_lif_phase_vt_lookup_res_0 = 0,
      _dpp_am_ingress_lif_phase_vt_lookup_res_1,
      _dpp_am_ingress_lif_phase_tt_lookup_res_0,
      _dpp_am_ingress_lif_phase_tt_lookup_res_1,
      _dpp_am_ingress_lif_nof_phases
} _dpp_am_ingress_lif_phase_t;


typedef enum bcm_dpp_am_egress_encap_app_e{
    bcm_dpp_am_egress_encap_app_pwe=0,
    bcm_dpp_am_egress_encap_app_mpls_tunnel,
    bcm_dpp_am_egress_encap_app_second_mpls_tunnel,
    bcm_dpp_am_egress_encap_app_out_ac,
    bcm_dpp_am_egress_encap_app_3_tags_data,
    bcm_dpp_am_egress_encap_app_3_tags_out_ac,
    bcm_dpp_am_egress_encap_app_linker_layer,  /* ROO Linker-layer */
    bcm_dpp_am_egress_encap_app_ip_tunnel_roo, /* ROO ip tunnel */
    bcm_dpp_am_egress_encap_app_trill_roo ,     /* ROO Trill */
    bcm_dpp_am_egress_encap_app_l2_encap ,     /* l2 encap for external CPU */    
    bcm_dpp_am_egress_encap_app_out_big_ac,
    bcm_dpp_am_egress_encap_app_evpn,
    bcm_dpp_am_egress_encap_app_native_arp,
    bcm_dpp_am_egress_encap_app_native_ac,
    bcm_dpp_am_egress_encap_app_third_mpls_tunnel,/*on QAX, mpls tunnel using access stage 2*/
    bcm_dpp_am_egress_encap_app_phase_3_out_ac, /* Store out ac at phase 3 intead of 4 */
    bcm_dpp_am_egress_encap_app_ip_tunnel_l2tp, /* L2TP ip tunnel */
    bcm_dpp_am_egress_encap_nof_app_types
} bcm_dpp_am_egress_encap_app_t;

typedef struct bcm_dpp_am_egress_encap_alloc_info_s {
  _dpp_am_res_t         pool_id; /* Indication on pool id */
  /* Indication needed on application in case pool id refers to more than one application */
  bcm_dpp_am_egress_encap_app_t application_type; 
} bcm_dpp_am_egress_encap_alloc_info_t;

typedef enum bcm_dpp_am_ingress_lif_app_e{
    bcm_dpp_am_ingress_lif_app_ingress_ac=0,
    bcm_dpp_am_ingress_lif_app_ingress_isid,    
    bcm_dpp_am_ingress_lif_app_mpls_term,
    bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1,
    bcm_dpp_am_ingress_lif_app_mpls_term_indexed_2,
    bcm_dpp_am_ingress_lif_app_mpls_term_indexed_3,
    bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_2,
    bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_3,
    bcm_dpp_am_ingress_lif_app_mpls_frr_inrif_port_term_indexed_1,    
    bcm_dpp_am_ingress_lif_app_mpls_frr_inrif_port_term_indexed_3,
    bcm_dpp_am_ingress_lif_app_mpls_frr_term,
    bcm_dpp_am_ingress_lif_app_mpls_term_explicit_null,
    bcm_dpp_am_ingress_lif_app_mpls_term_mldp,
    bcm_dpp_am_ingress_lif_app_ip_term,
    bcm_dpp_am_ingress_lif_app_trill_nick,
    bcm_dpp_am_ingress_lif_app_vpn_ac,
    bcm_dpp_am_ingress_lif_app_l2tp,
    bcm_dpp_am_ingress_lif_nof_app_types
} bcm_dpp_am_ingress_lif_app_t;

typedef struct bcm_dpp_am_ingress_lif_alloc_info_s {
  _dpp_am_res_t         pool_id; /* Indication on pool id */
  /* Indication needed on application in case pool id refers to more than one application */
  bcm_dpp_am_ingress_lif_app_t application_type;
} bcm_dpp_am_ingress_lif_alloc_info_t;

typedef struct bcm_dpp_am_sync_lif_alloc_info_s {
  uint32 sync_flags;
  bcm_dpp_am_ingress_lif_alloc_info_t ingress_lif;
  bcm_dpp_am_egress_encap_alloc_info_t egress_lif;
} bcm_dpp_am_sync_lif_alloc_info_t;



uint32 
_bcm_dpp_am_egress_encap_alloc(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint32 flags, int count, int* object);

uint32 
_bcm_dpp_am_egress_encap_alloc_align(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint32 flags, int align, int offset, int count, int* object);

uint32 
_bcm_dpp_am_egress_encap_dealloc(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, int count, int object);

int
_bcm_dpp_am_egress_encap_is_allocated(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, int count, int object);

uint32 
_bcm_dpp_am_ingress_lif_alloc(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, uint32 flags, int count, int* object);

uint32 
_bcm_dpp_am_ingress_lif_dealloc(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, int count, int object);

int    
_bcm_dpp_am_ingress_lif_is_allocated(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, int count, int object);

uint32 
_bcm_dpp_am_sync_lif_alloc(int unit, bcm_dpp_am_sync_lif_alloc_info_t* allocation_info, uint32 flags, int count, int* object);

uint32 
_bcm_dpp_am_sync_lif_alloc_align(int unit, bcm_dpp_am_sync_lif_alloc_info_t* allocation_info, uint32 flags, int align, int offset, int count, int* object);

uint32 
_bcm_dpp_am_sync_lif_dealloc(int unit, bcm_dpp_am_sync_lif_alloc_info_t* allocation_info, int count, int object);

uint32
_bcm_dpp_am_ingress_lif_init(int unit);

uint32
_bcm_dpp_am_ingress_lif_deinit(int unit);

uint32
_bcm_dpp_am_egress_encap_sw_state_init(int unit);

uint32
_bcm_dpp_am_egress_encap_init(int unit);

uint32
_bcm_dpp_am_egress_encap_deinit(int unit);

uint32
_bcm_dpp_am_egress_encap_alloc_mngr_pool_id_to_access_phase(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, SOC_PPC_EG_ENCAP_ACCESS_PHASE *phase_number);

uint32
_bcm_dpp_am_sync_lif_init(int unit);

uint32      
_bcm_dpp_am_sync_lif_deinit(int unit);


#endif /*INCLUDE_ALLOC_MNGR_LIF_H*/


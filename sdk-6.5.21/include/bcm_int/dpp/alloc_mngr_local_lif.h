/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        alloc_mngr_lif.h
 * Purpose:     Resource allocation for lif.
 */

#ifndef  INCLUDE_ALLOC_MNGR_LOCAL_LIF_H
/* { */
#define  INCLUDE_ALLOC_MNGR_LOCAL_LIF_H

/*************
* INCLUDES  *
 */
/* { */
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>	 
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>
/* } */
/*************
 * DEFINES   *
 */
/*
 * SW_STATE ACCESS
 */
#define LIF_LINKED_LIST_ACCESS                  sw_state_access[unit].dpp.bcm.lif_linked_list
#define LIF_LINKED_LISTS_ARRAY_ACCESS           LIF_LINKED_LIST_ACCESS.lif_linked_lists_array
#define LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS    LIF_LINKED_LIST_ACCESS.lif_linked_list_members_array

#define BCM_DPP_AM_IN_LIF_FLAG_COMMON          (1<<0)
#define BCM_DPP_AM_IN_LIF_FLAG_WIDE            (1<<1)
#define BCM_DPP_AM_IN_LIF_FLAG_ALLOC_WITH_ID   (1<<2)
#define BCM_DPP_AM_IN_LIF_FLAG_ALLOC_CHECK_ONLY (1<<3)
 
#define BCM_DPP_AM_OUT_LIF_FLAG_DIRECT         (1<<0)
#define BCM_DPP_AM_OUT_LIF_FLAG_COUPLED        (1<<1)
#define BCM_DPP_AM_OUT_LIF_FLAG_WIDE           (1<<2)
#define BCM_DPP_AM_OUT_LIF_FLAG_ODD_HALF       (1<<3)
#define BCM_DPP_AM_OUT_LIF_FLAG_EVEN_HALF      (1<<4)
#define BCM_DPP_AM_OUT_LIF_FLAG_ALLOC_CHECK_ONLY (1<<5)
#define BCM_DPP_AM_OUT_LIF_FLAG_FORCE_FRONT_SIXTEEN (1<<6)


#define BCM_DPP_AM_COUNTING_PROFILE_NONE       (0xFF)
#define BCM_DPP_AM_COUNTING_PROFILE_RESERVED   (0xFE)

#define BCM_DPP_AM_LOCAL_IN_LIF_NOF_RESERVED_LIFS (0x10)

#define _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS                 (SOC_DPP_DEFS_GET(unit, nof_inlif_banks))
#define _BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_ENTRIES_PER_BANK  (SOC_DPP_DEFS_GET(unit, nof_local_lifs_per_bank))

#define _BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK \
    (2 * _BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_ENTRIES_PER_BANK)

/*The max number of inLIF IDs*/
#define _BCM_DPP_AM_INGRESS_LIF_NOF_INLIF_IDS    \
    (SOC_DPP_DEFS_NOF_INLIF_BANKS_MAX * SOC_DPP_DEFS_NOF_LOCAL_LIFS_PER_BANK_MAX)

#define _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_BLOCKS                   (256)
#define _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_BANK(_unit)          (2 * SOC_DPP_DEFS_GET(_unit, nof_eedb_lines_per_bank))
#define _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK(_unit)     (_BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_BANK(_unit) /2)
#define _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS_PER_EEDB_BANK          (4)
#define _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS_PER_EEDB_HALF_BANK     (_BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS_PER_EEDB_BANK / 2)
#define _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_TOP_BANK(unit)      (_BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK(unit) /_BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS_PER_EEDB_HALF_BANK)
#define _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS(unit)                  (SOC_DPP_DEFS_GET(unit, eg_encap_nof_top_banks))
#define _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_EEDB_BANKS(unit)             (_BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS(unit) / _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS_PER_EEDB_BANK)
#define _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_BANKS(unit)                 (SOC_DPP_DEFS_GET(unit, eg_encap_nof_banks))
#define _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_REGULAR_BANKS(unit)         (_BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_BANKS(unit)    \
                                                                        - _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_EEDB_BANKS(unit))
#define _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_HALF_BANKS(unit)            (_BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_BANKS(unit) * 2)
#define _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_REGULAR_HALF_BANKS(unit)    (_BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_REGULAR_BANKS(unit) * 2)

#define BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_TYPE      (1<<0)
#define BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_EXTENSION (1<<1)
#define BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_PHASE     (1<<2)
#define BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_EXT_TYPE  (1<<3)

#define _BCM_DPP_AM_EEDB_BANK_UNASSIGNED                   (0xFF)
#define _BCM_DPP_AM_LOCAL_LIF_ID_UNASSIGNED                (0xFFFFFFFF)

#define _BCM_DPP_AM_LOCAL_LIF_NOF_FAILOVER_IDS        (32 * 1024) /* 32K Failover-ids*/

#define BCM_DPP_AM_LOCAL_OUT_LIF_HALF_BANK_IN_RANGE(unit, bank_id) \
    (bank_id<_BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_HALF_BANKS(unit))

#define BCM_DPP_AM_LOCAL_OUT_LIF_REGULAR_HALF_BANK_IN_RANGE(unit, bank_id) \
    (bank_id<_BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_REGULAR_HALF_BANKS(unit))


#define _BCM_DPP_AM_INGRESS_LOCAL_COMMON_LIF_POOL_ID_START (dpp_am_res_local_inlif_common)

#define _BCM_DPP_AM_INGRESS_LOCAL_COMMON_LIF_POOL_ID_END \
    (_BCM_DPP_AM_INGRESS_LOCAL_COMMON_LIF_POOL_ID_START + _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS)

#define _BCM_DPP_AM_INGRESS_LOCAL_WIDE_LIF_POOL_ID_START \
    (_BCM_DPP_AM_INGRESS_LOCAL_COMMON_LIF_POOL_ID_END)

#define _BCM_DPP_AM_INGRESS_LOCAL_WIDE_LIF_POOL_ID_END \
    (_BCM_DPP_AM_INGRESS_LOCAL_WIDE_LIF_POOL_ID_START + _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS)


/* LIF ranges counting profiles */

#define _BCM_DPP_AM_LOCAL_IN_LIF_NOF_COUNTING_RANGES             (2)
#define _BCM_DPP_AM_LOCAL_IN_LIF_NOF_COUNTING_PROFILES_PER_RANGE (1)
#define _BCM_DPP_AM_LOCAL_IN_LIF_NOF_COUNTING_PROFILES \
    (_BCM_DPP_AM_LOCAL_IN_LIF_NOF_COUNTING_PROFILES_PER_RANGE * _BCM_DPP_AM_LOCAL_IN_LIF_NOF_COUNTING_RANGES)


#define _BCM_DPP_AM_LOCAL_OUT_LIF_NOF_COUNTING_RANGES             (64)
#define _BCM_DPP_AM_LOCAL_OUT_LIF_NOF_COUNTING_PROFILES_PER_RANGE (1)
#define _BCM_DPP_AM_LOCAL_OUT_LIF_NOF_COUNTING_PROFILES  \
    (_BCM_DPP_AM_LOCAL_OUT_LIF_NOF_COUNTING_PROFILES_PER_RANGE * _BCM_DPP_AM_LOCAL_OUT_LIF_NOF_COUNTING_RANGES)

/*IN QAX/J+ the number of outlif counting ranges were changed to 64*/
#define _BCM_DPP_AM_LOCAL_OUT_LIF_NOF_COUNTING_PROFILES_GET(unit) \
   ((SOC_IS_JERICHO_PLUS(unit)) ?  _BCM_DPP_AM_LOCAL_OUT_LIF_NOF_COUNTING_PROFILES : _BCM_DPP_AM_LOCAL_IN_LIF_NOF_COUNTING_RANGES)

#define _BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_NOF_BITS         (6)
#define _BCM_DPP_AM_LOCAL_OUT_LIF_NOF_ARP_POINTED                  (1 << _BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_NOF_BITS)
#define _BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_MASK             (_BCM_DPP_AM_LOCAL_OUT_LIF_NOF_ARP_POINTED - 1)


/* } */
/*************
* MACROS    *
 */
/* { */
#define BCM_DPP_AM_LOCAL_INLIF_HANDLE_TO_BASE_LIF_ID(lif_handle) \
    ( (lif_handle%2==0) ? (lif_handle/2) :  (lif_handle/2 + 1) )

#define BCM_DPP_AM_LOCAL_INLIF_HANDLE_TO_ADDITIONAL_LIF_ID(lif_handle) \
    ( (lif_handle%2==0) ? (lif_handle/2  + 1) :  (lif_handle/2) )

#define BCM_DPP_AM_LOCAL_INLIF_HANDLE_TO_SEM_RESULT_ID(lif_handle)  \
    (lif_handle/2 )


/* } */
/*************
* TYPE DEFS *
 */
/* { */
/*The following structure defines an entry in the Ingress Local LIF table*/
typedef struct bcm_dpp_am_local_inlif_info_s
{
    bcm_dpp_am_ingress_lif_app_t app_type;
    int local_lif_flags;
    int base_lif_id;
    int ext_lif_id;
    int glif;
    uint8 valid;
    uint8 counting_profile_id;
} bcm_dpp_am_local_inlif_info_t;
 
typedef struct bcm_dpp_am_local_inlif_six_packs_info_s
{
    uint8 is_used;
    uint8 lif_type;
} bcm_dpp_am_local_inlif_six_packs_info_t;


typedef struct bcm_dpp_am_local_inlif_table_bank_info_s
{
    uint8  valid;
    uint32 phase_bmp;
    bcm_dpp_am_local_inlif_six_packs_info_t packs_info[SOC_DPP_DEFS_NOF_LOCAL_LIFS_PER_BANK_MAX * 2/6 + 1];

} bcm_dpp_am_local_inlif_table_bank_info_t;


typedef struct bcm_dpp_am_local_inlif_application_type_info_s
{
    uint8 banks_bmp;
} bcm_dpp_am_local_inlif_application_type_info_t;


typedef struct bcm_dpp_am_local_lif_counter_profile_info_s{
    int size;
    int base_offset; 
    int double_entry; 
} bcm_dpp_am_local_lif_counter_profile_info_t;


typedef struct bcm_dpp_alloc_mngr_local_inlif_info_s {
    uint8                                           initalized;
    uint8                                           lif_allocated;
    bcm_dpp_am_local_inlif_info_t                   data_base[_BCM_DPP_AM_INGRESS_LIF_NOF_INLIF_IDS];
    bcm_dpp_am_local_inlif_table_bank_info_t        bank_info[SOC_DPP_DEFS_NOF_INLIF_BANKS_MAX];
    bcm_dpp_am_local_inlif_application_type_info_t  application_type_info[bcm_dpp_am_ingress_lif_nof_app_types];
    bcm_dpp_am_local_lif_counter_profile_info_t     counter_profile_info[_BCM_DPP_AM_LOCAL_IN_LIF_NOF_COUNTING_PROFILES];
} bcm_dpp_alloc_mngr_local_inlif_info_t;

typedef struct bcm_dpp_am_local_out_lif_info_s
{
    bcm_dpp_am_egress_encap_alloc_info_t app_alloc_info;
    int local_lif_flags;
    int glif;
    int failover_id; /*device mode */
    int base_lif_id;
    int ext_lif_id;
    uint8 counting_profile_id;
} bcm_dpp_am_local_out_lif_info_t;



/* failover-id linked list member struct */
typedef struct failover_free_lif_linked_list_member_s {
  int lif_id;
  uint8  is_even;
  int next;
} failover_free_lif_linked_list_member_t;

/* failover-id linked list struct */
typedef struct failover_free_lif_linked_list_s {
  int first_member;
  uint32 size;
} failover_free_lif_linked_list_t;

typedef struct bcm_dpp_am_arp_next_lif_info_s {
       int lif_base;
       int nof_alloc_lifs;                       /* nof lif allocated already from alloc_bmp*/
       uint8 ready;                              /* is LIF resource allocated in res_bmp */
       PARSER_HINT_ARR SHR_BITDCL *res_bmp;      /* available resource bitmap */
       PARSER_HINT_ARR SHR_BITDCL *alloc_bmp;    /* allocated resource bitmap */
} bcm_dpp_am_arp_next_lif_info_t;

typedef struct bcm_dpp_alloc_mngr_local_outlif_info_s {
   uint8                                           initalized;
   uint8                                           lif_allocated;
    PARSER_HINT_ARR bcm_dpp_am_local_out_lif_info_t *data_base;
    bcm_dpp_am_local_lif_counter_profile_info_t     counter_profile_info[_BCM_DPP_AM_LOCAL_OUT_LIF_NOF_COUNTING_PROFILES];
    bcm_dpp_am_arp_next_lif_info_t arp_next_lif;
} bcm_dpp_alloc_mngr_local_outlif_info_t;

typedef struct bcm_dpp_alloc_mngr_outlif_eedb_banks_info_s {
    PARSER_HINT_ARR     arad_pp_eg_encap_eedb_bank_info_t   *banks;
    PARSER_HINT_ARR arad_pp_eg_encap_eedb_top_bank_info_t   *top_banks;
} bcm_dpp_alloc_mngr_outlif_eedb_banks_info_t;

typedef struct bcm_dpp_alloc_mngr_local_lif_info_s {
   bcm_dpp_alloc_mngr_local_inlif_info_t       inlif_info;
   bcm_dpp_alloc_mngr_local_outlif_info_t      outlif_info;
   bcm_dpp_alloc_mngr_outlif_eedb_banks_info_t eedb_info;
} bcm_dpp_alloc_mngr_local_lif_info_t;
/*
 * Control Structure for all created fail-over link lists.
 * Each linked list is rooted by failover_free_lif_linked_list_t.
 * See _bcm_dpp_am_local_outlif_init()
 */
typedef struct bcm_dpp_lif_linked_list_s
{
  uint32              max_nof_lif_linked_lists ;
  PARSER_HINT_ARR     failover_free_lif_linked_list_t          *lif_linked_lists_array ;

  uint32              max_nof_lif_linked_list_members ;
  PARSER_HINT_ARR     failover_free_lif_linked_list_member_t   *lif_linked_list_members_array ;

} bcm_dpp_lif_linked_list_t ;

/* } */
/*************
* GLOBALS   *
 */
/* { */
	 
/* } */
/*************
* FUNCTIONS *
 */

int failover_linked_list_print(
  /*in*/
  int unit,
  int linked_list) ;

uint32
_bcm_dpp_am_local_inlif_init(int unit);

uint32
_bcm_dpp_am_local_inlif_deinit(int unit);

uint32
_bcm_dpp_am_local_inlif_alloc(int unit, int flags, bcm_dpp_am_local_inlif_info_t *lif_info);

uint32
_bcm_dpp_am_local_inlif_dealloc(int unit, int lif_index);

uint32
_bcm_dpp_am_local_inlif_is_alloc(int unit, int lif_index);

uint32
_bcm_dpp_am_local_inlif_counting_profile_set(int unit, int counting_profile_id, int base, int size);

uint32
_bcm_dpp_am_local_outlif_init(int unit);

uint32
_bcm_dpp_am_local_outlif_deinit(int unit);

uint32
_bcm_dpp_am_local_outlif_alloc(int unit, int flags, bcm_dpp_am_local_out_lif_info_t *lif_info);

uint32
_bcm_dpp_am_local_outlif_arp_pointed_alloc(int unit, int flags, bcm_dpp_am_local_out_lif_info_t *lif_info);

uint32
_bcm_dpp_am_local_outlif_dealloc(int unit, int lif_index);

uint32
_bcm_dpp_am_local_outlif_arp_pointed_dealloc(int unit, int lif_index);

uint32
_bcm_dpp_am_local_outlif_dealloc_only(int unit, int lif_index);

uint32
_bcm_dpp_am_local_outlif_is_alloc(int unit, int lif_index);

uint32
_bcm_dpp_am_local_outlif_extension_bank_alloc(int unit, int bank_id, int *ext_bank_id, uint8 *bank_found);

uint32
_bcm_dpp_am_local_outlif_extension_bank_dealloc(int unit, int bank_id);

uint32
_bcm_dpp_am_local_outlif_bank_phase_set(int unit, int bank_id,  int phase);

uint32
_bcm_dpp_am_local_outlif_counting_profile_set(int unit, int counting_profile_id, int base, int size, int double_entry);

uint32
_bcm_dpp_am_local_outlif_counting_profile_get(int unit, int counting_profile_id, int *base, int *size, int *double_entry);


uint32
_bcm_dpp_local_lif_is_wide_entry(int unit, int local_lif, int is_ingress, int *is_wide_entry);


/* } */
#endif /*INCLUDE_ALLOC_MNGR_LOCAL_LIF_H*/


/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l2.h
 * Purpose:     L2 internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_L2_H_
#define   _BCM_INT_DPP_L2_H_

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_llp_trap.h>
#include <soc/dpp/PPD/ppd_api_llp_sa_auth.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>

#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>
#include <bcm/l2.h>

#define DPP_AGING_PROFILE_FIRST_CONFIGURABLE (1)
#define DPP_AGING_PROFILE_LAST_CONFIGURABLE (3)
#define DPP_AGING_CYCLES_MAX (6)
#define DPP_AGING_CYCLES_IGNORE_VALUE (-1)

typedef struct bcm_dpp_user_defined_vsi_egress_profile_s {
    uint32 mtu;
    uint64 eve_da_lsbs_bitmap;
} bcm_dpp_vsi_egress_profile_t;

typedef struct bcm_dpp_user_defined_vsi_ingress_profile_s {
    uint64 ive_da_lsbs_bitmap;
    uint64 peer_da_lsbs_bitmap;
    uint64 drop_da_lsbs_bitmap;
} bcm_dpp_vsi_ingress_profile_t;

typedef bcm_dpp_vsi_egress_profile_t bcm_dpp_l2cp_egress_profile_t;

#define L2CP_DA_LSBS_BITMAP_EGRESS_SET(egress_profile_data, da_lsbs, handle_type)        \
  do {                                                                                   \
      uint32 hi=0;                                                                       \
      uint32 low=0;                                                                  \
      uint64 new_bitmap;                                                             \
      if (da_lsbs < 32) {                                                            \
          low = 1 << da_lsbs;                                                    \
      }                                                                              \
      else {                                                                         \
          hi = 1 << (da_lsbs - 32);                                              \
      }                                                                              \
      COMPILER_64_SET(new_bitmap, hi, low);                                          \
      if (handle_type == SOC_PPC_VSI_L2CP_HANDLE_TYPE_TUNNEL) {                      \
          COMPILER_64_OR(egress_profile_data.eve_da_lsbs_bitmap, new_bitmap);    \
      }                                                                              \
          else {                                                                         \
                  COMPILER_64_NOT(new_bitmap);                                           \
          COMPILER_64_AND(egress_profile_data.eve_da_lsbs_bitmap, new_bitmap);   \
          }                                                                              \
  } while (0);                                                                           \

#define L2CP_DA_LSBS_BITMAP_INGRESS_SET(ingress_profile_data, da_lsbs, handle_type)      \
  do {                                                                                   \
      uint32 hi=0;                                                                       \
      uint32 low=0;                                                                  \
      uint64 new_bitmap;                                                             \
      uint64 empty_bitmap;                                                           \
      if (da_lsbs < 32) {                                                            \
          low = 1 << da_lsbs;                                                    \
      }                                                                              \
      else {                                                                         \
          hi = 1 << (da_lsbs - 32);                                              \
      }                                                                              \
      COMPILER_64_SET(new_bitmap, hi, low);                                          \
      COMPILER_64_SET(empty_bitmap, hi, low);                                        \
          COMPILER_64_NOT(empty_bitmap);                                                 \
      if (handle_type == SOC_PPC_VSI_L2CP_HANDLE_TYPE_TUNNEL) {                      \
          COMPILER_64_OR(ingress_profile_data.ive_da_lsbs_bitmap, new_bitmap);   \
          COMPILER_64_AND(ingress_profile_data.peer_da_lsbs_bitmap, empty_bitmap);  \
          COMPILER_64_AND(ingress_profile_data.drop_da_lsbs_bitmap, empty_bitmap);  \
      }                                                                                 \
      else if (handle_type == SOC_PPC_VSI_L2CP_HANDLE_TYPE_PEER) {                      \
          COMPILER_64_OR(ingress_profile_data.peer_da_lsbs_bitmap, new_bitmap);     \
          COMPILER_64_AND(ingress_profile_data.ive_da_lsbs_bitmap, empty_bitmap);   \
          COMPILER_64_AND(ingress_profile_data.drop_da_lsbs_bitmap, empty_bitmap);  \
      }                                                                                 \
      else if (handle_type == SOC_PPC_VSI_L2CP_HANDLE_TYPE_DROP) {                      \
          COMPILER_64_OR(ingress_profile_data.drop_da_lsbs_bitmap, new_bitmap);     \
          COMPILER_64_AND(ingress_profile_data.peer_da_lsbs_bitmap, empty_bitmap);  \
          COMPILER_64_AND(ingress_profile_data.ive_da_lsbs_bitmap, empty_bitmap);   \
      }                                                                                 \
          else {                                                                            \
          COMPILER_64_AND(ingress_profile_data.ive_da_lsbs_bitmap, empty_bitmap);     \
          COMPILER_64_AND(ingress_profile_data.drop_da_lsbs_bitmap, empty_bitmap);  \
          COMPILER_64_AND(ingress_profile_data.peer_da_lsbs_bitmap, empty_bitmap);  \
          }                                                                                 \
  } while (0);

#define L2CP_DA_LSBS_BITMAP_INGRESS_GET(ingress_profile_data, da_lsbs, handle_type)      \
  do {                                                                                   \
      uint32 hi=0;                                                                       \
      uint32 low=0;                                                                  \
      uint64 new_bitmap;                                                             \
      if (da_lsbs < 32) {                                                            \
          low = 1 << da_lsbs;                                                    \
      }                                                                              \
      else {                                                                         \
          hi = 1 << (da_lsbs - 32);                                              \
      }                                                                              \
      COMPILER_64_SET(new_bitmap, hi, low);                                          \
      COMPILER_64_AND(new_bitmap, ingress_profile_data.ive_da_lsbs_bitmap);          \
      if (!COMPILER_64_IS_ZERO(new_bitmap)) {                                        \
          handle_type = SOC_PPC_VSI_L2CP_HANDLE_TYPE_TUNNEL;                     \
      }                                                                              \
      else {                                                                         \
          COMPILER_64_SET(new_bitmap, hi, low);                                  \
          COMPILER_64_AND(new_bitmap, ingress_profile_data.peer_da_lsbs_bitmap); \
          if (!COMPILER_64_IS_ZERO(new_bitmap)) {                                \
              handle_type = SOC_PPC_VSI_L2CP_HANDLE_TYPE_PEER;               \
          }                                                                      \
          else {                                                                 \
              COMPILER_64_SET(new_bitmap, hi, low);                          \
              COMPILER_64_AND(new_bitmap, ingress_profile_data.drop_da_lsbs_bitmap);\
              if (!COMPILER_64_IS_ZERO(new_bitmap)) {                        \
                  handle_type = SOC_PPC_VSI_L2CP_HANDLE_TYPE_DROP;       \
              }                                                              \
              else {                                                         \
                  handle_type = SOC_PPC_VSI_L2CP_HANDLE_TYPE_NORMAL;     \
              }                                                              \
          }                                                                      \
      }                                                                              \
  } while (0);


 /* The following macros indicate if the egress layer two control packets are 
  * Trapped according to VSI or OUTLIF.
  * On jericho_plus and above devices, l2cp_profile is mapped from outlif rather
  * than the same with vsi profile.
  */
#define _BCM_L2_MC_RESERVED_EGRESS_PROFILE_TYPE_VSI    (0)
#define _BCM_L2_MC_RESERVED_EGRESS_PROFILE_TYPE_L2CP   (1)
#define _BCM_L2_NUM_OF_ETH_TYPE_INDEX_ENTRY        (16)


 /* mact freeze thaw status*/
typedef struct _bcm_petra_l2_freeze_s {
    int                 frozen;
    int                 save_age_ena;
    int                 save_learn_ena;
} _bcm_petra_l2_freeze_t;

/* SW DB */
 /*
 * L2 Callback Message Registration
 */
#define _BCM_PETRA_L2_CB_MAX             3    /* Max registered callbacks per unit */

/* call backs handling*/
typedef struct _bcm_petra_l2_cb_entry_s {
    bcm_l2_addr_callback_t  fn;         /* Callback routine */
    void                    *fn_data;   /* User defined data */
} _bcm_petra_l2_cb_entry_t;

typedef struct _l2_cb_s {
    _bcm_petra_l2_cb_entry_t    entry[_BCM_PETRA_L2_CB_MAX];
    int               count;
} _bcm_petra_l2_cb_t;

typedef struct _l2_ether_type_index_entry_ref_s {
    uint32    eth_type_ref;
    uint32    outer_tpid_ref;
    uint32    inner_tpid_ref;
} _l2_ether_type_index_entry_ref_t;

typedef struct l2_data_s {
    uint8               mymac_msb_is_set;
    int l2_initialized;
    _bcm_petra_l2_freeze_t bcm_petra_l2_freeze_state;
    uint16              mpls_extended_label_ether_type_index_table_is_set;
    _l2_ether_type_index_entry_ref_t              ether_type_index_table_ref_cnt[_BCM_L2_NUM_OF_ETH_TYPE_INDEX_ENTRY];
} l2_data_t;

/* access macros */
#define _BCM_PETRA_L2_CB_ENTRY(unit, i)    (sw_state_cb_db[unit].dpp.l2_cb.entry[i])
#define _BCM_PETRA_L2_CB_COUNT(unit)       (sw_state_cb_db[unit].dpp.l2_cb.count)

/*
 * L2 definition
 */

#define _BCM_PETRA_L2_ILLEGAL_AGE_STATUS (ARAD_PP_FRWRD_MACT_ILLEGAL_AGE_STATUS)

#define _BCM_DPP_SNOOP_CMD_TO_CPU (1)

#define _BCM_PETRA_L2_NOF_RSRV_MC (0x40)

#define _BCM_PETRA_L2_RM_BUF_NOF_U32 (16)

#define _BCM_PETRA_L2_RM_BUF_INDEX_OFFSET (8)

#define _BCM_PETRA_L2_RM_BUF_INDEX_LEN (8)

#define BCM_PETRA_L2_STATION_GLOBAL_MSB (0)
/* valid only flag MIM is set (so no change of TRILL)*/ 
#define BCM_PETRA_L2_STATION_GLOBAL_LSB (1 << 16)

/* 
 * l2 station bit indications. Available bits are: 23-31
 */

/* bit 28: OAM CFM l2 station indication. */
#define BCM_PETRA_L2_STATION_OAM_CFM_BIT (1 << 28)

/* bit 29: MIM l2 station indication. */
#define BCM_PETRA_L2_STATION_MIM_BIT (1 << 29)

/* bit 30: Vrrp l2 station indication. */
#define BCM_PETRA_L2_STATION_VRRP_BIT (1 << 30)


/* The following constants and macros are used to create the station_id for l2 stations using the oam cfm table.
   The bit mapping is:
   bits 0 - 12: port
   bit 13 empty
   bits 14-21: Mymac lsb
   bits 22-27 empty
   bit 28: OAM CFM l2 station indication.
   bit 29-31: empty*/

#define BCM_PETRA_L2_STATION_OAM_PORT_SHIFT         (0)
#define BCM_PETRA_L2_STATION_OAM_PORT_MASK          (0xfff)
#define BCM_PETRA_L2_STATION_OAM_MYMAC_LSB_SHIFT    (14)
#define BCM_PETRA_L2_STATION_OAM_MYMAC_LSB_MASK     (0xff)


/* The variable in _station_id will be assigned an oam cfm station id according to the format above. */
#define BCM_PETRA_L2_STATION_OAM_CFM_SET(_station_id, _port, _lsb)              \
    ((_station_id) = (((_port) << BCM_PETRA_L2_STATION_OAM_PORT_SHIFT) |         \
                    ((_lsb) << BCM_PETRA_L2_STATION_OAM_MYMAC_LSB_SHIFT) |      \
                    (BCM_PETRA_L2_STATION_OAM_CFM_BIT)))

/* Wether the station id is an oam cfm station */
#define BCM_PETRA_L2_STATION_IS_OAM_CFM(_station_id) \
    ((_station_id) & BCM_PETRA_L2_STATION_OAM_CFM_BIT)

/* Gets the port from an oam cfm station id */
#define BCM_PETRA_L2_STATION_OAM_PORT_GET(_port, _station_id)    \
    ((_port) = ((_station_id) >>  BCM_PETRA_L2_STATION_OAM_PORT_SHIFT)  \
                & BCM_PETRA_L2_STATION_OAM_PORT_MASK )

/* Gets the lsbs from an oam cfm station id */
#define BCM_PETRA_L2_STATION_OAM_MYMAC_LSB_GET(_lsb, _station_id)    \
    ((_lsb) = ((_station_id) >>  BCM_PETRA_L2_STATION_OAM_MYMAC_LSB_SHIFT)  \
                 & BCM_PETRA_L2_STATION_OAM_MYMAC_LSB_MASK)


/* The following constants and macros are used to create the station_id for l2 stations using the vrrp table.
   The bit mapping is:
   bits 0 - 16: vsi
   bit 17 empty
   bit 18-22: Index in the vrrp cam table
   bits 23-29 empty
   bit 30: Vrrp l2 station indication.
   bit 31: Empty */

#define BCM_PETRA_L2_STATION_VRRP_VSI_SHIFT (0)
#define BCM_PETRA_L2_STATION_VRRP_VSI_MASK  (0x1ffff)
#define BCM_PETRA_L2_STATION_VRRP_CAM_INDEX_SHIFT (18)
#define BCM_PETRA_L2_STATION_VRRP_CAM_INDEX_MASK  (0x1f)

/* The variable in _station_id will be assigned a vrrp station id according to the format above. */
#define BCM_PETRA_L2_STATION_VRRP_SET(_station_id, _vsi, _cam_index)                \
    ((_station_id) = (((_vsi) << BCM_PETRA_L2_STATION_VRRP_VSI_SHIFT ) |              \
                    ((_cam_index) << BCM_PETRA_L2_STATION_VRRP_CAM_INDEX_SHIFT) |   \
                    (BCM_PETRA_L2_STATION_VRRP_BIT)))

/* Wether the station id is a vrrp station */
#define BCM_PETRA_L2_STATION_IS_VRRP(_station_id) \
    ((_station_id) & BCM_PETRA_L2_STATION_VRRP_BIT)

/* Gets the vsi from a vrrp station id */
#define BCM_PETRA_L2_STATION_VRRP_VSI_GET(_vsi, _station_id)    \
    ((_vsi) = ((_station_id) >>  BCM_PETRA_L2_STATION_VRRP_VSI_SHIFT)  \
                & BCM_PETRA_L2_STATION_VRRP_VSI_MASK )

/* Gets the cam table index from a vrrp station id */
#define BCM_PETRA_L2_STATION_VRRP_CAM_INDEX_GET(_cam_index, _station_id)    \
    ((_cam_index) = ((_station_id) >>  BCM_PETRA_L2_STATION_VRRP_CAM_INDEX_SHIFT)  \
                 & BCM_PETRA_L2_STATION_VRRP_CAM_INDEX_MASK )

/* Given the api's l3 protocol group flags, returns the soc ppc flags. */
#define BCM_PETRA_L2_STATION_TRANSLATE_API_FLAGS_TO_INTERNAL_PROTOCOL_GROUP_FLAGS(_src, _target)     \
    (_target) = 0;                                                                                   \
    DPP_TRANSLATE_FLAG(_src, BCM_L2_STATION_IPV4    , _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV4 );\
    DPP_TRANSLATE_FLAG(_src, BCM_L2_STATION_IPV6    , _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV6 );\
    DPP_TRANSLATE_FLAG(_src, BCM_L2_STATION_ARP_RARP, _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_ARP  );\
    DPP_TRANSLATE_FLAG(_src, BCM_L2_STATION_MPLS    , _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_MPLS );\
    DPP_TRANSLATE_FLAG(_src, BCM_L2_STATION_MIM     , _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_MIM  );\
    DPP_TRANSLATE_FLAG(_src, BCM_L2_STATION_TRILL   , _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_TRILL);\
    DPP_TRANSLATE_FLAG(_src, BCM_L2_STATION_FCOE    , _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_FCOE );

/* Given the soc ppc l3 protocol group flags, returns the api flags. */
#define BCM_PETRA_L2_STATION_TRANSLATE_PROTOCOL_GROUP_INTERNAL_FLAGS_TO_API(_src, _target)           \
    (_target) = 0;                                                                                   \
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV4 , _target, BCM_L2_STATION_IPV4    );\
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV6 , _target, BCM_L2_STATION_IPV6    );\
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_ARP  , _target, BCM_L2_STATION_ARP_RARP);\
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_MPLS , _target, BCM_L2_STATION_MPLS    );\
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_MIM  , _target, BCM_L2_STATION_MIM     );\
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_TRILL, _target, BCM_L2_STATION_TRILL   );\
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_FCOE , _target, BCM_L2_STATION_FCOE    );

#define BCM_PETRA_L3_MULTIPLE_MYMAC_STATION_SUPPORTED_PROTOCOL_FLAGS \
    (BCM_L2_STATION_IPV4    \
    |BCM_L2_STATION_IPV6     \
    |BCM_L2_STATION_ARP_RARP \
    |BCM_L2_STATION_MPLS     \
    |BCM_L2_STATION_MIM      \
    |BCM_L2_STATION_TRILL    \
    |BCM_L2_STATION_FCOE)

typedef struct bcm_dpp_prog_trap_s {
    SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER qual;
    bcm_dpp_user_defined_traps_t result;
} bcm_dpp_prog_trap_t;


typedef struct _bcm_petra_l2_rsrvd_mc_profile_info_s {
    uint32 buff[_BCM_PETRA_L2_RM_BUF_NOF_U32];
} _bcm_petra_l2_rsrvd_mc_profile_info_t;
 

typedef struct _bcm_l2_limit_llm_info_s
{
    int rx_channel;             /* Local RX DMA channel for BHH packets */
    int uc_num;                 /* uController number running BHH appl */
    int dma_buffer_len;         /* DMA max buffer size */
    uint8* dma_buffer;          /* DMA buffer */
    uint8* dmabuf_reply;        /* DMA reply buffer */
    sal_thread_t event_thread_id;  /* Event handler thread id */
    int event_thread_kill; /* Whether to stop event thread */
    uint32 event_enable;      /* Event enable/disable */
    sal_mutex_t  llm_mutex; /* mutex for LLM message */
    sal_mutex_t  l2_mutex;      /* arm restore */

} _bcm_l2_limit_llm_info_t;


extern SOC_PPC_FRWRD_MACT_ENTRY_KEY                
     *_bcm_l2_traverse_mact_keys[BCM_LOCAL_UNITS_MAX];
extern SOC_PPC_FRWRD_MACT_ENTRY_VALUE              
     *_bcm_l2_traverse_mact_vals[BCM_LOCAL_UNITS_MAX];
extern SOC_SAND_PP_MAC_ADDRESS                
     *_bcm_l2_auth_traverse_mac_addrs[BCM_LOCAL_UNITS_MAX];
extern SOC_PPC_LLP_SA_AUTH_MAC_INFO              
     *_bcm_l2_auth_traverse_mac_infos[BCM_LOCAL_UNITS_MAX];
extern _bcm_petra_l2_freeze_t _bcm_petra_l2_freeze_state[BCM_LOCAL_UNITS_MAX];
extern int l2_initialized[BCM_LOCAL_UNITS_MAX];
extern l2_data_t l2_data[BCM_LOCAL_UNITS_MAX];

int 
bcm_petra_l2_cache_set_unset(
                       int unit, 
                       int index, 
                       bcm_l2_cache_addr_t *addr, 
                       int *index_used,
                       int is_for_delete);
 
int _bcm_petra_l2_sw_traverse(int unit, 
                    bcm_l2_traverse_cb trav_fn, 
                    void *user_data);
 
int _bcm_petra_l2_to_petra_entry(int unit, bcm_l2_addr_t *l2addr, int key_only, 
                                 SOC_PPC_FRWRD_MACT_ENTRY_KEY *l2_entry_key, 
                                 SOC_PPC_FRWRD_MACT_ENTRY_VALUE *l2_entry_val);



int _bcm_petra_l2_freeze_set_no_wb(int unit, int frozen);

int _bcm_petra_l2_freeze_inc(int unit, int *frozen);

int _bcm_petra_l2_freeze_dec(int unit, int *frozen);

int _bcm_petra_l2_is_freezed(int unit, uint8 *is_freezed);

int _bcm_petra_l2_freeze_info_set(int unit, _bcm_petra_l2_freeze_t* freeze_info);

int _bcm_petra_l2_freeze_info_get(int unit, _bcm_petra_l2_freeze_t* freeze_info);


int
_bcm_petra_l2_mymac_msb_is_set_set(int unit, uint8 val);

int
_bcm_petra_l2_mymac_msb_is_set_get(int unit, uint8 *is_set);

int
_bcm_dpp_l2_vsi_profile_update(
    int unit, 
    uint32 egress_profile,
    uint32 egress_profile_type,
    uint32 ingress_profile,
    bcm_dpp_vsi_egress_profile_t egress_profile_data, 
    bcm_dpp_vsi_ingress_profile_t ingress_profile_data);

int _bcm_petra_l2_event_handle_profile_alloc(
                                             int unit, int check, int vsi, int learn_flags, int trans_flags,
                                             int del_flags, int refresh_flags, int *new_learn_profile, int *first_appear);

/* 
 * Purpose: process MACT_EVENT_FIFO interrupt.
 *   - read event-fifo
 *   - parse 
 *   - call callback function to handle this event.
 */
int _bcm_dpp_l2_event_fifo_interrupt_handler(int unit, uint32 *buf);
/* 
 * Purpose: Mask or unmask MACT_EVENT_FIFO interrupt.
 */
int _bcm_dpp_mact_interrupts_mask(int unit, uint8 mask_interrupts);

/* Convert multicast id bcm typedef to soc ppd typedef */
int
_bcm_petra_mc_id_to_ppd_val(int unit, int l2mc_index, SOC_TMC_MULT_ID *soc_ppd_mc_id);

/* Set the bit indicating whether ether type index table has been set for mpls extended configuration */
int
_bcm_petra_l2_mpls_extended_label_ether_type_index_table_is_set_set(int unit, uint16 val);

/* Get the bit indicating whether ether type index table has been set for mpls extended configuration */
int
_bcm_petra_l2_mpls_extended_label_ether_type_index_table_is_set_get(int unit, uint16 *is_set);

/* Fill the ether type index table for mpls extended configuration */
int _bcm_petra_l2_mpls_extended_label_fill_ether_type_index_table(int unit,SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO eth_type_index_entry);

#endif /* _BCM_INT_DPP_L2_H_ */

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * NAT - Broadcom StrataSwitch NAT API internal definitions 
 */

#ifndef _BCM_INT_NAT_H
#define _BCM_INT_NAT_H

#ifdef INCLUDE_L3

#include <sal/core/sync.h>

/* NAT egress state per unit 
  - hash table of nat entry to index
  - ref counts per entry index
  - bitmap for allocation
*/

typedef struct _bcm_l3_nat_egress_hash_entry_s {
    uint32  ip_addr;
    uint16  vrf;
    uint16  l4_port;
    struct _bcm_l3_nat_egress_hash_entry_t *next;
    uint16  nat_id;
} _bcm_l3_nat_egress_hash_entry_t;

typedef struct _bcm_l3_nat_egress_state_s {
    _bcm_l3_nat_egress_hash_entry_t *_bcm_l3_nat_egress_hash_tbl[256];
    uint16 nat_id_refcount[2048];
    SHR_BITDCL *nat_id_bitmap;
} _bcm_l3_nat_egress_state_t;

typedef struct _bcm_l3_nat_ingress_state_s {
    uint32  tbl_cnts[3];
    int  snat_napt_free_idx;
    int  snat_nat_free_idx;
#ifdef BCM_MONTEREY_SUPPORT
    int  dnat_address_type_free_idx;
#endif
} _bcm_l3_nat_ingress_state_t;

typedef struct _bcm_l3_nat_state_s {
    _bcm_l3_nat_egress_state_t e_state;
    _bcm_l3_nat_ingress_state_t i_state;
    sal_mutex_t lock;
} _bcm_l3_nat_state_t;

extern _bcm_l3_nat_state_t *_bcm_l3_nat_state[BCM_MAX_NUM_UNITS];


/* helper struct for nat ingress age/delete */
typedef struct _bcm_l3_nat_ingress_cb_context_s {
    void *user_data;
    bcm_l3_nat_ingress_traverse_cb user_cb;
    soc_mem_t   mem;     
} bcm_l3_nat_ingress_cb_context_t;

/* index into table counters */
#define BCM_L3_NAT_INGRESS_POOL_CNT    0
#define BCM_L3_NAT_INGRESS_DNAT_CNT    1
#define BCM_L3_NAT_INGRESS_SNAT_CNT    2

#define BCM_L3_NAT_EGRESS_INFO(_unit_) _bcm_l3_nat_state[(_unit_)]->e_state
#define BCM_L3_NAT_INGRESS_INFO(_unit_) _bcm_l3_nat_state[(_unit_)]->i_state

/* nat packet translate table bitmap operations */
#define BCM_L3_NAT_EGRESS_ENTRIES_PER_INDEX    2

#define BCM_L3_NAT_EGRESS_HW_IDX_GET(nat_idx, hw_idx, hw_half) \
do {\
    (hw_idx) = (nat_idx) >> 1;\
    (hw_half) = (nat_idx) & 1;\
} while (0)

#define BCM_L3_NAT_EGRESS_SW_IDX_GET(hw_idx, hw_half) \
    ((hw_idx) << 1 | (hw_half))

#define BCM_L3_NAT_FILL_SW_ENTRY_X(hw_buf, nat_info, num)\
do {\
    int bit_count;\
    if (soc_EGR_NAT_PACKET_EDIT_INFOm_field32_get(unit, (hw_buf), \
                            MODIFY_SRC_OR_DST_##num)) { /* DIP */\
        /* fill DNAT/NAPT */\
        (nat_info)->flags |= BCM_L3_NAT_EGRESS_DNAT;\
        (nat_info)->dip_addr = soc_EGR_NAT_PACKET_EDIT_INFOm_field32_get(unit, \
                                (hw_buf), IP_ADDRESS_##num);\
        if (soc_EGR_NAT_PACKET_EDIT_INFOm_field32_get(unit, (hw_buf), \
                                            MODIFY_L4_PORT_OR_PREFIX_##num)) {\
            /* modify prefix */\
            bit_count = soc_EGR_NAT_PACKET_EDIT_INFOm_field32_get(unit, (hw_buf), \
                                    L4_PORT_OR_PREFIX_##num);\
            if (bit_count < 32) {\
                (nat_info)->dip_addr_mask = ((1 << bit_count) - 1) << \
                                                            (32 - bit_count);\
            } else {\
                (nat_info)->dip_addr_mask = 0xffffffff;\
            }\
        } else {\
            /* modify port */\
            (nat_info)->flags |= BCM_L3_NAT_EGRESS_NAPT;\
            (nat_info)->dst_port = soc_EGR_NAT_PACKET_EDIT_INFOm_field32_get(unit, \
                                    (hw_buf), L4_PORT_OR_PREFIX_##num);\
        }\
    } else {\
        /* fill SNAT/NAPT */\
        (nat_info)->flags |= BCM_L3_NAT_EGRESS_SNAT;\
        (nat_info)->sip_addr = soc_EGR_NAT_PACKET_EDIT_INFOm_field32_get(unit, \
                                (hw_buf), IP_ADDRESS_##num);\
        if (soc_EGR_NAT_PACKET_EDIT_INFOm_field32_get(unit, (hw_buf), \
                                            MODIFY_L4_PORT_OR_PREFIX_##num)) {\
            /* modify prefix */\
            bit_count = soc_EGR_NAT_PACKET_EDIT_INFOm_field32_get(unit, (hw_buf), \
                                    L4_PORT_OR_PREFIX_##num);\
            if (bit_count < 32) {\
                (nat_info)->sip_addr_mask = ((1 << bit_count) - 1) << \
                                                            (32 - bit_count);\
            } else {\
                (nat_info)->sip_addr_mask = 0xffffffff;\
            }\
        } else {\
            /* modify port */\
            (nat_info)->flags |= BCM_L3_NAT_EGRESS_NAPT;\
            (nat_info)->src_port = soc_EGR_NAT_PACKET_EDIT_INFOm_field32_get(unit, \
                                    (hw_buf), L4_PORT_OR_PREFIX_##num);\
        }\
    }\
} while (0)

#define BCM_L3_NAT_EGRESS_INC_REF_COUNT(unit, nat_id, count) \
do {\
    BCM_L3_NAT_EGRESS_INFO((unit)).nat_id_refcount[(nat_id)]++;\
    /* currently max of only 2 can be incremented per call */\
    if ((count) > 1) {\
        BCM_L3_NAT_EGRESS_INFO((unit)).nat_id_refcount[(nat_id) + 1]++;\
    }\
} while (0)

#define BCM_L3_NAT_EGRESS_DEC_REF_COUNT(unit, nat_id) \
    BCM_L3_NAT_EGRESS_INFO((unit)).nat_id_refcount[(nat_id)]--

#define BCM_L3_NAT_EGRESS_GET_REF_COUNT(unit, nat_id) \
    BCM_L3_NAT_EGRESS_INFO((unit)).nat_id_refcount[(nat_id)]


#define BCM_L3_NAT_INGRESS_GET_MEM_POINTER(unit, nat_info, mem, pool, dnat, \
                                           snat, result, mem_counter) \
do {\
    /* figure out which mem to operate on */\
    if ((nat_info)->flags & BCM_L3_NAT_INGRESS_DNAT) {\
        if ((nat_info)->flags & BCM_L3_NAT_INGRESS_DNAT_POOL) {\
            (mem) = ING_DNAT_ADDRESS_TYPEm;\
            (result) = &(pool);\
            (mem_counter) = BCM_L3_NAT_INGRESS_POOL_CNT;\
        }\
        else {\
           if (SOC_MEM_IS_VALID(unit, L3_ENTRY_DOUBLEm)) {\
               (mem) = L3_ENTRY_DOUBLEm;\
           } else {\
               (mem) = L3_ENTRY_IPV4_MULTICASTm;\
           }\
           (result) = &(dnat);\
           (mem_counter) = BCM_L3_NAT_INGRESS_DNAT_CNT;\
        }\
    } else {\
        (mem) = ING_SNATm;\
        (result) = &(snat);\
        (mem_counter) = BCM_L3_NAT_INGRESS_SNAT_CNT;\
    }\
} while (0)

/* no counters */
#define BCM_L3_NAT_INGRESS_GET_MEM_ONLY(unit, nat_info, mem, pool, dnat, \
                                           snat, result) \
do {\
    /* figure out which mem to operate on */\
    if ((nat_info)->flags & BCM_L3_NAT_INGRESS_DNAT) {\
        if ((nat_info)->flags & BCM_L3_NAT_INGRESS_DNAT_POOL) {\
            (mem) = ING_DNAT_ADDRESS_TYPEm;\
            (result) = &(pool);\
        }\
        else {\
            (mem) = L3_ENTRY_IPV4_MULTICASTm;\
            if (SOC_MEM_IS_VALID(unit, L3_ENTRY_DOUBLEm)) {\
               (mem) = L3_ENTRY_DOUBLEm;\
            }\
            (result) = &(dnat);\
        }\
    } else {\
        (mem) = ING_SNATm;\
        (result) = &(snat);\
    }\
} while (0)

#define BCM_L3_NAT_INGRESS_INC_TBL_CNT(unit, tbl) \
    BCM_L3_NAT_INGRESS_INFO((unit)).tbl_cnts[(tbl)]++;

#define BCM_L3_NAT_INGRESS_DEC_TBL_CNT(unit, tbl) \
    BCM_L3_NAT_INGRESS_INFO((unit)).tbl_cnts[(tbl)]--;

#define BCM_L3_NAT_INGRESS_GET_TBL_CNT(unit, tbl) \
    BCM_L3_NAT_INGRESS_INFO((unit)).tbl_cnts[(tbl)];

#define BCM_L3_NAT_INGRESS_CLR_TBL_CNT(unit, tbl) \
    BCM_L3_NAT_INGRESS_INFO((unit)).tbl_cnts[(tbl)] = 0;

#define BCM_L3_NAT_INGRESS_SNAT_NAPT_FREE_IDX(unit) \
    BCM_L3_NAT_INGRESS_INFO((unit)).snat_napt_free_idx

#define BCM_L3_NAT_INGRESS_SNAT_NAT_FREE_IDX(unit) \
    BCM_L3_NAT_INGRESS_INFO((unit)).snat_nat_free_idx

#ifdef BCM_MONTEREY_SUPPORT
#define BCM_L3_NAT_ING_DNAT_ADDRESS_TYPE_FREE_IDX(unit) \
    BCM_L3_NAT_INGRESS_INFO((unit)).dnat_address_type_free_idx
#endif

#define BCM_L3_NAT_EGRESS_FLAGS_FULL_NAT(flags) \
    ((flags) & (BCM_L3_NAT_EGRESS_SNAT | BCM_L3_NAT_EGRESS_DNAT)) == \
                    (BCM_L3_NAT_EGRESS_SNAT | BCM_L3_NAT_EGRESS_DNAT)
 
#define BCM_TD2_NAT_INGRESS_BANK_ENTRY_HASH(mem, unit, bidx, bufp, index) \
do {\
    if ((mem) == L3_ENTRY_IPV4_MULTICASTm || mem == L3_ENTRY_DOUBLEm) {\
        (index) = soc_td2_l3x_bank_entry_hash((unit), (bidx), (bufp));\
    } else {\
        /* mem is ING_DNAT_ADDRESS_TYPEm */\
        (index) = soc_td2_ing_dnat_address_type_bank_entry_hash((unit), (bidx), \
                                                                (bufp));\
    }\
} while (0)

#define BCM_TD3_L3UC_DNAT_FIELDS 0
#define BCM_TD3_L3UC_SNAT_FIELDS 1
#define BCM_TD3_L3MC_FIELDS      2

#define BCM_TD3_L3UC_SNAT_KEY_TYPE 22
#define BCM_TD3_L3UC_DNAT_KEY_TYPE 24
#define BCM_TD3_L3MC_NAT_KEY_TYPE 23

#define BCM_L3_NAT_DATA_TYPE(_u_) \
    ((SOC_IS_HELIX5(_u_)) ? (22) : (21))

extern int _bcm_esw_l3_nat_init(int unit);
extern void _bcm_esw_l3_nat_free_resource(int unit);
extern int _bcm_esw_l3_nat_egress_reference_get(int unit,
               bcm_l3_nat_id_t nat_id, uint32 *ref_count);
#if defined (BCM_TRIDENT2_SUPPORT)
int _bcm_esw_nat_egress_ref_count_update(int unit, uint32 nat_id, int incr);
#endif
extern int _bcm_td3_large_nat_egress_add(int unit, bcm_l3_large_nat_egress_t *nat_info);
extern int _bcm_td3_large_nat_egress_key_set(int unit, soc_mem_t mem, bcm_l3_large_nat_egress_t *nat_info, int *nat_key, uint32 *entry);
extern int _bcm_td3_large_nat_egress_entry_get(int unit, soc_mem_t mem, uint32* entry, uint32 nat_key, bcm_l3_large_nat_egress_t *nat_info);
extern int _bcm_td3_large_nat_egress_traverse(int unit, soc_mem_t mem, bcm_l3_large_nat_egress_traverse_cb cb, void *user_data);
#endif /* INCLUDE_L3 */
#endif /* _BCM_INT_NAT_H */

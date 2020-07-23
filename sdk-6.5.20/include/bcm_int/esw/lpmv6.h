/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        lpmv6.h
 * Purpose:     Function declarations for LPMV6 Internal functions.
 */

#ifndef _BCM_INT_LPMV6_H
#define _BCM_INT_LPMV6_H

#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/l3.h>

#define _BCM_DEFIP_TCAM_DEPTH    1024
#define _BCM_DEFIP_TCAM_NUM      8

/* L3 LPM state */
#define BCM_DEFIP_PAIR128_HASH_SZ             (0x6)
#define BCM_DEFIP_PAIR128(_unit_)             l3_defip_pair128[(_unit_)]
#define BCM_DEFIP_PAIR128_INIT_CHECK(_unit_)  (l3_defip_pair128[(_unit_)] != NULL)
#define BCM_DEFIP_PAIR128_ARR(_unit_)         BCM_DEFIP_PAIR128((_unit_))->entry_array
#define BCM_DEFIP_PAIR128_IDX_MAX(_unit_)     BCM_DEFIP_PAIR128((_unit_))->idx_max
#define BCM_DEFIP_PAIR128_USED_COUNT(_unit_)  BCM_DEFIP_PAIR128((_unit_))->used_count
#define BCM_DEFIP_PAIR128_TOTAL(_unit_)       BCM_DEFIP_PAIR128((_unit_))->total_count
#define BCM_DEFIP_PAIR128_URPF_OFFSET(_unit_) BCM_DEFIP_PAIR128((_unit_))->urpf_offset
#define BCM_DEFIP_PAIR128_ENTRY_SET(_unit_, _idx_, _plen_, _hash_)          \
            BCM_DEFIP_PAIR128_ARR((_unit_))[(_idx_)].prefix_len = (_plen_); \
            BCM_DEFIP_PAIR128_ARR((_unit_))[(_idx_)].entry_hash = (_hash_)

typedef struct _bcm_defip_pair128_entry_s {
    uint16 prefix_len; /* Route entry  prefix length.*/
    uint16 entry_hash; /* Route entry key hash.      */
} _bcm_defip_pair128_entry_t;

typedef struct _bcm_defip_pair128_table_s {
    _bcm_defip_pair128_entry_t *entry_array; /* Cam entries array.      */
    uint16 idx_max;                   /* Last cam entry index.             */
    uint16 used_count;                /* Used cam entry count.             */
    uint16 total_count;               /* Total number of available entries.*/
    uint16 urpf_offset;               /* Src lookup start offset.          */
} _bcm_defip_pair128_table_t;

#if defined(INCLUDE_L3)

/* LPM */
extern _bcm_defip_pair128_table_t *l3_defip_pair128[BCM_MAX_NUM_UNITS];

extern int _bcm_l3_lpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                           int *nh_ecmp_idx);
extern int _bcm_l3_lpm_add(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                           int nh_ecmp_idx);
extern int _bcm_l3_lpm_del(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int _bcm_l3_lpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data);
extern int _bcm_l3_defip_init(int unit);
extern int _bcm_l3_defip_deinit(int unit);
extern int _bcm_l3_defip_urpf_enable(int unit, int enable);
extern int _bcm_defip_pair128_deinit(int unit);
extern int _bcm_l3_defip_pair128_get(int unit, _bcm_defip_cfg_t *lpm_cfg,
                                     int *nh_ecmp_idx);
extern int _bcm_l3_defip_pair128_add(int unit, _bcm_defip_cfg_t *lpm_cfg,
                                     int nh_ecmp_idx);
extern int _bcm_l3_defip_pair128_del(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int _bcm_l3_defip_pair128_update_match(int unit, 
                                              _bcm_l3_trvrs_data_t *trv_data);
extern int _bcm_l3_scaled_lpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg,
                                  int *nh_ecmp_idx);
extern int _bcm_l3_scaled_lpm_add(int unit, _bcm_defip_cfg_t *lpm_cfg,
                                  int nh_ecmp_idx);
extern int _bcm_l3_scaled_lpm_del(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int _bcm_l3_lpm128_ripple_entries(int unit);
extern int _bcm_l3_lpm_ripple_entries(int unit, _bcm_defip_cfg_t *lpm_cfg,
                                      int new_nh_ecmp_idx);
extern int _bcm_l3_scaled_lpm_update_match(int unit, 
                                           _bcm_l3_trvrs_data_t *trv_data);
extern int _bcm_defip_pair128_field_cache_init(int unit);

extern int _bcm_defip_pair128_idx_alloc(int unit, _bcm_defip_cfg_t *lpm_cfg,
                                        int *hw_index, int nh_ecmp_idx);
extern  void
_bcm_defip_pair128_ip6_addr_set(int unit, soc_mem_t mem, uint32 *entry,
                                const ip6_addr_t ip6, int ipmc);
extern void
_bcm_defip_pair128_ip6_mask_set(int unit, soc_mem_t mem, uint32 *entry,
                                const ip6_addr_t ip6, int ipmc);


#endif /* INCLUDE_L3 */
#endif

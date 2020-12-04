/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l3x.h
 * Purpose:     Draco L3X hardware table manipulation support
 */

#ifndef _L3X_H_
#define _L3X_H_

#include <soc/macipadr.h>
#include <soc/hash.h>

#define L3_LOCK(_unit_)   soc_esw_l3_lock(_unit_)
#define L3_UNLOCK(_unit_) soc_esw_l3_unlock(_unit_)

extern int soc_esw_l3_lock(int unit);
extern int soc_esw_l3_unlock(int unit);

#ifdef BCM_XGS_SWITCH_SUPPORT
#if defined(INCLUDE_L3)
#define SOC_MEM_L3X_ENTRY_VALID             (1 << 0)
#define SOC_MEM_L3X_ENTRY_MOVED             (1 << 1)
#define SOC_MEM_L3X_ENTRY_MOVE_FAIL         (1 << 2)

/*
 *  Dual hash l3x entry info.
 */
typedef struct soc_mem_l3x_entry_info_s {
    int index;       /* Entry start index in the bucket.    */
    int size;        /* Entry size - number of base memory  */
                     /* slots entry occupies.               */
    soc_mem_t mem;   /* View name.                          */
    uint8 flags;     /* Entry flags - valid/move attempted. */
} soc_mem_l3x_entry_info_t;

/*
 *  Dual hash l3x half bucket entries map.
 */
typedef struct soc_mem_l3x_bucket_map_s {
    int size;          /* Bucket map size  (#valid entries).         */            
    int valid_count;   /* Count of valid base entries in the bucket. */
    int total_count;   /* Total number of base entries in the bucket.*/
    soc_mem_t base_mem;/* Base memory/view name.                     */
    soc_mem_t base_idx;/* Bucket start index in base memory.         */
    soc_mem_l3x_entry_info_t *entry_arr;
                      /* Bucket entries information  array.          */
} soc_mem_l3x_bucket_map_t;
#endif /* INCLUDE_L3 */

extern int soc_l3x_init(int unit);

/* These defines should be replaced with unit-based accessors someday */
#define SOC_L3X_MAX_BUCKET_SIZE  16
#define SOC_L3X_BUCKET_SIZE(_u)  ((SOC_IS_RAVEN(_u) || SOC_IS_TR_VL(_u)) ? 16 : 8)
#define SOC_L3X_IP_MULTICAST(i)	 (((uint32)(i) & 0xf0000000) == 0xe0000000)
extern int soc_l3x_entries(int unit);	/* Warning: very slow */

extern int soc_l3x_lock(int unit);
extern int soc_l3x_unlock(int unit);

/*
 * L3 software-based sanity routines
 */

#endif	/* BCM_XGS_SWITCH_SUPPORT */

#ifdef	BCM_FIREBOLT_SUPPORT

extern int soc_fb_l3x_bank_insert(int unit, uint8 banks,
                                  l3_entry_ipv6_multicast_entry_t *entry);
extern int soc_fb_l3x_bank_delete(int unit, uint8 banks,
                                  l3_entry_ipv6_multicast_entry_t *entry);
extern int soc_fb_l3x_bank_lookup(int unit, uint8 banks,
                                  l3_entry_ipv6_multicast_entry_t *key,
                                  l3_entry_ipv6_multicast_entry_t *result,
                                  int *index_ptr);

extern int soc_fb_l3x_insert(int unit, l3_entry_ipv6_multicast_entry_t *entry);
extern int soc_fb_l3x_delete(int unit, l3_entry_ipv6_multicast_entry_t *entry);
extern int soc_fb_l3x_lookup(int unit, l3_entry_ipv6_multicast_entry_t *key,
                             l3_entry_ipv6_multicast_entry_t *result,
                             int *index_ptr);

/*
 * We must use SOC_MEM_INFO here, because we may redefine the
 * index_max due to configuration, but the SCHAN msg uses the maximum size.
 */
#define SOC_L3X_PERR_PBM_POS(unit, mem) \
        ((_shr_popcount(SOC_MEM_INFO(unit, mem).index_max) + \
            soc_mem_entry_bits(unit, mem)) %32)
#endif	/* BCM_FIREBOLT_SUPPORT */

#ifdef	BCM_EASYRIDER_SUPPORT

#define SOC_ER_L3V6_BUCKET_SIZE	 4
#define soc_er_l3v4_insert(unit, entry) \
        soc_mem_insert((unit),  L3_ENTRY_V4m, COPYNO_ALL, (entry))
#define soc_er_l3v6_insert(unit, entry) \
        soc_mem_insert((unit),  L3_ENTRY_V6m, COPYNO_ALL, (entry))
#define soc_er_l3v4_delete(unit, entry) \
        soc_mem_delete((unit),  L3_ENTRY_V4m, COPYNO_ALL, (entry))
#define soc_er_l3v6_delete(unit, entry) \
        soc_mem_delete((unit),  L3_ENTRY_V6m, COPYNO_ALL, (entry))
extern int soc_er_l3v4_lookup(int unit, l3_entry_v4_entry_t *key,
                              l3_entry_v4_entry_t *result, int *index_ptr);
extern int soc_er_l3v6_lookup(int unit, l3_entry_v6_entry_t *key,
                              l3_entry_v6_entry_t *result, int *index_ptr);

#endif	/* BCM_EASYRIDER_SUPPORT */

#endif	/* !_L3X_H_ */

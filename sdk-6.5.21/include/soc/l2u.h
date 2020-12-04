/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l2u.h
 * Purpose:     XGS3 L2 User table manipulation support
 */

#ifndef _L2U_H_
#define _L2U_H_

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
/* Provide a more reasonably named type from auto-generated type */
#define l2u_entry_t l2_user_entry_entry_t

/* More convenience */
#define _l2u_field_get soc_L2_USER_ENTRYm_field_get
#define _l2u_field_set soc_L2_USER_ENTRYm_field_set
#define _l2u_field32_get soc_L2_USER_ENTRYm_field32_get
#define _l2u_field32_set soc_L2_USER_ENTRYm_field32_set
#define _l2u_mac_addr_get soc_L2_USER_ENTRYm_mac_addr_get
#define _l2u_mac_addr_set soc_L2_USER_ENTRYm_mac_addr_set

/* Maximum number of BPDU addresses */
#define L2U_BPDU_COUNT 6

/* Valid fields in search key */
#define L2U_KEY_MAC             0x00000001
#define L2U_KEY_VLAN            0x00000002
#define L2U_KEY_PORT            0x00000004
#define L2U_KEY_MODID           0x00000008
#define L2U_KEY_MAC_MASK        0x00000010
#define L2U_KEY_VLAN_MASK       0x00000020

typedef struct l2u_key_s {
    uint32      flags;
    sal_mac_addr_t mac;
    int         vlan;
    int         port;
    int         modid;
    sal_mac_addr_t mac_mask;
    int         vlan_mask;
} l2u_key_t;

extern int soc_l2u_overlap_check(int unit, l2u_entry_t *entry, int *index);
extern int soc_l2u_search(int unit, l2u_entry_t *key, l2u_entry_t *result, int *index);
extern int soc_l2u_find_free_entry(int unit, l2u_entry_t *key, int *free_index);
extern int soc_l2u_insert(int unit, l2u_entry_t *entry, int index, int *index_used);
extern int soc_l2u_get(int unit, l2u_entry_t *entry, int index);
extern int soc_l2u_delete(int unit, l2u_entry_t *entry, int index, int *index_deleted);
extern int soc_l2u_delete_by_key(int unit, l2u_key_t *key);
extern int soc_l2u_delete_all(int unit);
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
#endif /* _L2U_H_ */

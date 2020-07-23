/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Firebolt VLAN_MAC Table Manipulation API routines.
 * soc_fb_vlan_mac_entry_hash - Compute hash for VLAN_MAC table entry
 * soc_fb_vlanmac_entry_ins - Insert an entry into VLAN_MAC table
 * soc_fb_vlanmac_entry_del - Delete an entry from VLAN_MAC table
 * soc_fb_vlanmac_entry_lkup - Lookup an entry in VLAN_MAC table
 */

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/vlan.h>
#include <soc/hash.h>
#include <soc/debug.h>

#ifdef BCM_FIREBOLT_SUPPORT
/* 
 * soc_fb_vlan_mac_entry_hash - Compute hash for VLAN_MAC table entry
 */
int
soc_fb_vlan_mac_entry_hash(int unit, int hash_sel, vlan_mac_entry_t *entry)
{
    uint8           key[6];
    int             index;
    sal_mac_addr_t  mac; 
    int             i;

    soc_VLAN_MACm_mac_addr_get(unit, entry, MAC_ADDRf, mac);
    for (i = 0; i < 6; i++) {
        key[i] = mac[5 - i];
    }
    index = soc_fb_vlan_mac_hash(unit, hash_sel, key);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "VLAN_MAC hash_sel %d hash index %d\n"),
                            hash_sel, index));

    return index;
}

/* 
 * soc_fb_vlanmac_entry_ins - Insert an entry into VLAN_MAC table
 */
int
soc_fb_vlanmac_entry_bank_ins(int unit, uint8 banks, vlan_mac_entry_t *entry)
{
    vlan_mac_entry_t    tmp;
    int                 index = 0, bucket, slot, free_slot;
    int                 slot_min, slot_max, hash_sel;

    slot_min = 0;
    slot_max = 3;

#ifdef BCM_RAVEN_SUPPORT
    if (soc_feature(unit, soc_feature_dual_hash) && (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit))) {
        switch (banks) {
        case 2:
            slot_min = 0;
            slot_max = 1;
            SOC_IF_ERROR_RETURN
                (soc_fb_rv_vlanmac_hash_sel_get(unit, 0, &hash_sel));	    
            break;
        case 1:
            slot_min = 2;
            slot_max = 3;
            SOC_IF_ERROR_RETURN
                (soc_fb_rv_vlanmac_hash_sel_get(unit, 1, &hash_sel));
            break;
        default:
            SOC_IF_ERROR_RETURN
                (soc_fb_rv_vlanmac_hash_sel_get(unit, 0, &hash_sel));
            break;
        }
    } else
#endif /* BCM_RAVEN_SUPPORT */
    {
        SOC_IF_ERROR_RETURN
            (soc_fb_rv_vlanmac_hash_sel_get(unit, 0, &hash_sel));
    }

    /* Buckets of 4 entry each */
    bucket = soc_fb_vlan_mac_entry_hash(unit, hash_sel, entry);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_fb_vlanmac_entry_ins: bucket %d\n"), bucket));

    /* Check if it should overwrite an existing entry */
    free_slot = -1;
    for (slot = slot_min; slot <= slot_max; slot++) {
        index = bucket * 4 + slot;
        SOC_IF_ERROR_RETURN(READ_VLAN_MACm(unit, MEM_BLOCK_ANY, index, &tmp));
        if (soc_mem_field32_get(unit, VLAN_MACm, &tmp, VALIDf)) {
            if (soc_mem_compare_key(unit, VLAN_MACm, entry, &tmp) == 0) {
                free_slot = slot;
                break;
            }
        } else {
            if (free_slot == -1) {
                free_slot = slot;
            }
        }
    }

    if (free_slot == -1) {
        return(SOC_E_FULL);
    }

    index = bucket * 4 + free_slot;
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_fb_vlanmac_entry_ins: write slot %d, index %d\n"),
                 free_slot, index));
    SOC_IF_ERROR_RETURN(WRITE_VLAN_MACm(unit, MEM_BLOCK_ANY, index, entry));

    return SOC_E_NONE;
}

int
soc_fb_vlanmac_entry_ins(int unit, vlan_mac_entry_t *entry)
{
#if defined(BCM_RAVEN_SUPPORT)
    if (soc_feature(unit, soc_feature_dual_hash) && (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit))) {
        return
            _soc_mem_dual_hash_insert(unit, VLAN_MACm, COPYNO_ALL, (void *)entry,
                          NULL, SOC_DUAL_HASH_MOVE_MAX_VLAN(unit));
    } else
#endif
    {
        return soc_fb_vlanmac_entry_bank_ins(unit, 0, entry);
    }

}

/* 
 * soc_fb_vlanmac_entry_del - Delete an entry from VLAN_MAC table
 */
int
soc_fb_vlanmac_entry_del(int unit, vlan_mac_entry_t *entry)
{
    vlan_mac_entry_t    tmp;
    int                 index = 0, bucket, slot, dual, banks;
    int                 slot_min, slot_max, hash_sel;

    slot_min = 0;
    slot_max = 3;

#ifdef BCM_RAVEN_SUPPORT
    if (soc_feature(unit, soc_feature_dual_hash) && (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit))) {
        banks = 2;
    } else
#endif /* BCM_RAVEN_SUPPORT */
    {
        banks = 1;
        SOC_IF_ERROR_RETURN
            (soc_fb_rv_vlanmac_hash_sel_get(unit, 0, &hash_sel));
    }

    /* Iterate over the banks manually to find the entry */
    for (dual = 0; dual < banks; dual++) {
        if (banks == 2 && dual == 0) {
            slot_min = 0;
            slot_max = 1;
            SOC_IF_ERROR_RETURN
                (soc_fb_rv_vlanmac_hash_sel_get(unit, 0, &hash_sel));
        } else if (banks == 2 && dual == 1) {
            slot_min = 2;
            slot_max = 3;
            SOC_IF_ERROR_RETURN
                (soc_fb_rv_vlanmac_hash_sel_get(unit, 1, &hash_sel));
        } 
        bucket = soc_fb_vlan_mac_entry_hash(unit, hash_sel, entry);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "soc_fb_vlanmac_entry_del: bucket %d\n"), 
                     bucket));

        /* Check if entry exists */
        for (slot = slot_min; slot <= slot_max; slot++) {
            index = bucket * 4 + slot;
            SOC_IF_ERROR_RETURN
                (READ_VLAN_MACm(unit, MEM_BLOCK_ANY, index, &tmp));
            if (soc_mem_field32_get(unit, VLAN_MACm, &tmp, VALIDf)) {
                if (soc_mem_compare_key(unit, VLAN_MACm, entry, &tmp) == 0) {
                    soc_mem_field32_set(unit, VLAN_MACm, &tmp, VALIDf, 0);
                    SOC_IF_ERROR_RETURN(
                        WRITE_VLAN_MACm(unit, MEM_BLOCK_ANY, index, &tmp));
                    return SOC_E_NONE;
                }
            }
        }
    }
    return(SOC_E_NONE); /* Entry doesn't exist */
}

/* 
 * soc_fb_vlanmac_entry_lkup - Lookup an entry in VLAN_MAC table
 *   Note: entry and result may overlap.
 */

int
soc_fb_vlanmac_entry_lkup(int unit, vlan_mac_entry_t *entry,
                          vlan_mac_entry_t *result, int *index_ptr)
{
    vlan_mac_entry_t    tmp;
    int                 index = 0, bucket, slot, banks, dual;
    int                 slot_min, slot_max, hash_sel;

    slot_min = 0;
    slot_max = 3;

#ifdef BCM_RAVEN_SUPPORT
    if (soc_feature(unit, soc_feature_dual_hash) && (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit))) {
        banks = 2;
    } else
#endif /* BCM_RAVEN_SUPPORT */
    {
        banks = 1;
        SOC_IF_ERROR_RETURN
            (soc_fb_rv_vlanmac_hash_sel_get(unit, 0, &hash_sel));
    }

    /* Iterate over the banks manually to find the entry */
    for (dual = 0; dual < banks; dual++) {
        if (banks == 2 && dual == 0) {
            slot_min = 0;
            slot_max = 1;
            SOC_IF_ERROR_RETURN
                (soc_fb_rv_vlanmac_hash_sel_get(unit, 0, &hash_sel));
        } else if (banks == 2 && dual == 1) {
            slot_min = 2;
            slot_max = 3;
            SOC_IF_ERROR_RETURN
                (soc_fb_rv_vlanmac_hash_sel_get(unit, 1, &hash_sel));
        }
        *index_ptr = -1;
        bucket = soc_fb_vlan_mac_entry_hash(unit, hash_sel, entry);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "soc_fb_vlanmac_entry_del: bucket %d\n"), 
                     bucket));

        /* Check if entry exists */
        for (slot = slot_min; slot <= slot_max; slot++) {
            index = bucket * 4 + slot;
            SOC_IF_ERROR_RETURN(READ_VLAN_MACm(unit, MEM_BLOCK_ANY, index, 
                                               &tmp));
            if (soc_mem_field32_get(unit, VLAN_MACm, &tmp, VALIDf)) {
                if (soc_mem_compare_key(unit, VLAN_MACm, entry, &tmp) == 0) {
                    sal_memcpy(result, &tmp, sizeof(tmp));
                    *index_ptr = index;
                    return SOC_E_NONE;
                }
            }
        }
    }
    return(SOC_E_NOT_FOUND);
}
#endif /* BCM_FIREBOLT_SUPPORT */



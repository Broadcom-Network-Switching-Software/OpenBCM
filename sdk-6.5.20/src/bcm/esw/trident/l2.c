/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l2.c
 * Purpose:     Trident L2 function implementations
 */
#include <soc/defs.h>
	 
#if defined(BCM_TRIDENT_SUPPORT)
	 
#include <assert.h>
	 
#include <sal/core/libc.h>
	 
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/hash.h>
#include <soc/l2x.h>
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <soc/ism_hash.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#include <soc/triumph.h>

#include <bcm/l2.h>
#include <bcm/error.h>

#include <bcm_int/esw/l2.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/trill.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>

typedef uint32 L2_ENTRY_T[SOC_MAX_MEM_FIELD_WORDS];

#if defined(INCLUDE_L3)
/*
 * Purpose:
 *      Add TRILL Multicast entry
 * Parameters:
 *      unit            - Device unit number
 *      key             - defines entry type
 *      vid             - Customer Vlan ID
 *      mac             - Customer MAC
 *      trill_tree_id   - specify Multicast Tree ID
 *      group           - Multicast group
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int 
bcm_td_l2_trill_multicast_entry_add(int unit, uint32 flags, int key, 
                                    bcm_vlan_t vid, bcm_mac_t mac, 
                                    uint8 trill_tree_id, bcm_multicast_t group)
{
    L2_ENTRY_T  l2_entry, l2_lookup;
    soc_mem_t mem; 
    soc_field_t field_key;
    int rv=BCM_E_NONE;
    int mc_index=0,l2_index=0;
    vlan_tab_entry_t  vt;

    sal_memset(&l2_entry, 0, sizeof (l2_entry));

    mem = L2Xm;

    if (_BCM_MULTICAST_IS_SET(group)) {

        mc_index = _BCM_MULTICAST_ID_GET(group);

        switch (key) {
        case TR_L2_HASH_KEY_TYPE_BRIDGE:
            field_key = TR_L2_HASH_KEY_TYPE_BRIDGE;
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                    VALIDf, 1);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 L2__DEST_TYPEf, 0);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 L2__L2MC_PTRf, mc_index);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 L2__TRILL_NETWORK_RECEIVERS_PRESENTf, 1);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                 L2__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry,
                                  L2__MAC_ADDRf, mac);
            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
                field_key =  TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS;
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                     TRILL_NONUC_ACCESS__DEST_TYPEf, 0x3);
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                     KEY_TYPEf, field_key);
                if (flags & BCM_TRILL_MULTICAST_STATIC) { 
                    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                  TRILL_NONUC_ACCESS__STATIC_BITf, 0x1);
                }
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                     TRILL_NONUC_ACCESS__L3MC_PTRf, mc_index);
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                     TRILL_NONUC_ACCESS__VLAN_IDf, vid);
                soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry, 
                                      TRILL_NONUC_ACCESS__MAC_ADDRf, mac);
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                    VALIDf, 1);
            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
            field_key = TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT; 
            if (flags & BCM_TRILL_MULTICAST_STATIC) {
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                    TRILL_NONUC_NETWORK_SHORT__STATIC_BITf, 0x1);
            }

            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                             TRILL_NONUC_NETWORK_SHORT__DEST_TYPEf, 0x3);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, VALIDf, 1);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_SHORT__TREE_IDf, trill_tree_id);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_SHORT__L3MC_INDEXf, mc_index);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_SHORT__VLAN_IDf, vid);

            BCM_IF_ERROR_RETURN(soc_mem_read(unit, VLAN_TABm, 
                                  MEM_BLOCK_ANY, vid, &vt));

            if (soc_mem_field32_get(unit, VLAN_TABm, &vt, 
                    TRILL_ACCESS_RECEIVERS_PRESENTf)) {
                  soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_SHORT__TRILL_ACCESS_RECEIVERS_PRESENTf, 1);
            }

            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
            field_key = TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG; 
            if (flags & BCM_TRILL_MULTICAST_STATIC) {
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                    TRILL_NONUC_NETWORK_LONG__STATIC_BITf, 0x1);
            }
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                             TRILL_NONUC_NETWORK_LONG__DEST_TYPEf, 0x3);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, VALIDf, 1);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__TREE_IDf, trill_tree_id);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__TRILL_ACCESS_RECEIVERS_PRESENTf, 1);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__L3MC_INDEXf, mc_index);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry, 
                                  TRILL_NONUC_NETWORK_LONG__MAC_ADDRESSf, mac);
            break;

        default:
            break;
        }
    }


    /* See if the entry already exists */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &l2_index,
                        (void *)&l2_entry, (void *)&l2_lookup, 0);

    if (rv == SOC_E_NONE) {
        if (key == TR_L2_HASH_KEY_TYPE_BRIDGE) {
            soc_mem_field32_set(unit, mem, (uint32*)&l2_lookup,
                                 L2__TRILL_NETWORK_RECEIVERS_PRESENTf, 1);
            rv = soc_mem_write(unit, mem,
                               MEM_BLOCK_ALL, l2_index,
                               &l2_lookup);
        }
    } else if ((rv < 0) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    } else {
        /* Add Entry */
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ANY, (void *)&l2_entry);
        if ((rv == SOC_E_FULL) &&
            ((key == TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT) ||
            (key == TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG)) &&
            (flags & BCM_TRILL_MULTICAST_REPLACE_DYNAMIC)) {
            /*
             *  If the table is FULL,
             *  replace the existing entry for TRILL_NW_SHORT and TRILL_NW_LONG.
             */
            l2x_entry_t l2x_entry;
            sal_memcpy((void *)&l2x_entry,
                       (void *)&l2_entry, sizeof(l2x_entry_t));
            rv = _bcm_l2_hash_dynamic_replace(unit, &l2x_entry);
        }
    }
    return rv;
}


/*
 * Purpose:
 *      Update TRILL Multicast entry
 * Parameters:
 *      unit            - Device unit number
 *      key             - defines entry type
 *      vid             - Customer Vlan ID
 *      mac             - Customer MAC
 *      trill_tree_id   - specify Multicast Tree ID
 *      group           - Trill Multicast group
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int 
bcm_td_l2_trill_multicast_entry_update(int unit, int key, 
                                    bcm_vlan_t vid, bcm_mac_t mac, 
                                    uint8 trill_tree_id, bcm_multicast_t group)
{
    L2_ENTRY_T  l2_entry, l2_lookup;
    soc_mem_t mem; 
    soc_field_t field_key;
    int rv=BCM_E_NONE;
    int l2_index=0;

    sal_memset(&l2_entry, 0, sizeof (l2_entry));

    mem = L2Xm;

    if (_BCM_MULTICAST_IS_TRILL(group)) {

        switch (key) {
        case TR_L2_HASH_KEY_TYPE_BRIDGE:
            field_key = TR_L2_HASH_KEY_TYPE_BRIDGE;
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                    VALIDf, 1);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                 L2__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry,
                                  L2__MAC_ADDRf, mac);
            break;

        }
    }

    /* See if the entry already exists */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &l2_index,
                        (void *)&l2_entry, (void *)&l2_lookup, 0);
    if (rv == SOC_E_NONE) {
        if (key == TR_L2_HASH_KEY_TYPE_BRIDGE) {
            soc_mem_field32_set(unit, mem, (uint32*)&l2_lookup,
                                 L2__TRILL_NETWORK_RECEIVERS_PRESENTf, 1);
            rv = soc_mem_write(unit, mem,
                               MEM_BLOCK_ALL, l2_index,
                               &l2_lookup);
        }
    } else {
        /* Entry must exist */
        return rv;
    }
    return rv;
}

/*
 * Purpose:
 *      Delete TRILL Multicast entry
 * Parameters:
 *      unit            - Device unit number
 *      key             - defines entry type
 *      vid             - Customer Vlan ID
 *      mac             - Customer MAC
 *      trill_tree_id   - specify Multicast Tree ID
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int 
bcm_td_l2_trill_multicast_entry_delete(int unit, int key, bcm_vlan_t vid, 
                                       bcm_mac_t mac, uint8 trill_tree_id)
{
    L2_ENTRY_T  l2_entry, l2_lookup;
    soc_mem_t mem; 
    soc_field_t field_key;
    int rv=BCM_E_NONE;
    int l2_index=0;

    sal_memset(&l2_entry, 0, sizeof (l2_entry));

    mem = L2Xm;

    switch (key) {
        case TR_L2_HASH_KEY_TYPE_BRIDGE:
            field_key = TR_L2_HASH_KEY_TYPE_BRIDGE;
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                L2__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry, 
                                 L2__MAC_ADDRf, mac);
            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
            field_key =  TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS;
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                L2__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry,
                                 L2__MAC_ADDRf, mac);
            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
            field_key = TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT; 
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                TRILL_NONUC_NETWORK_SHORT__TREE_IDf,
                                trill_tree_id);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                TRILL_NONUC_NETWORK_SHORT__VLAN_IDf, vid);
            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
            field_key = TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG; 
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__TREE_IDf,
                                trill_tree_id);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                TRILL_NONUC_NETWORK_LONG__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__MAC_ADDRESSf, mac);
            break;
                        
        default:
            break;
    }

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &l2_index, 
                        (void *)&l2_entry, (void *)&l2_lookup, 0);
                 
    if ((rv < 0) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    } 

    rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, (void *)&l2_entry);
    return rv;
}


/*
 * Description:
 *      Delete all TRILL tree-sepecific Multicast entries
 * Parameters:
 *     unit             - device number
 *     tree_id          - Trill Tree ID
 * Return:
 *     BCM_E_XXX
 */
int
bcm_td_l2_trill_multicast_entry_delete_all (int unit, uint8 tree_id)
{
    int           ix, key_type, chnk_end;
    int           chunksize, chunk, chunk_max, mem_idx_max;
    L2_ENTRY_T   *l2e, *l2ep;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_mem_t     mem;
    int           rv = BCM_E_NONE;
    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    mem = L2Xm;

    l2e = soc_cm_salloc(unit, chunksize * sizeof(*l2e), "l2entrydel_chunk");
    if (l2e == NULL) {
        return BCM_E_MEMORY;
    }
    mem_idx_max = soc_mem_index_max(unit, mem);
    soc_mem_lock(unit, mem);
    for (chunk = soc_mem_index_min(unit, mem); chunk <= mem_idx_max;
         chunk += chunksize) {
        
        chunk_max = chunk + chunksize - 1;
        if (chunk_max > mem_idx_max) {
            chunk_max = mem_idx_max;
        }

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chunk, chunk_max, l2e);
        if (rv < 0) {
            break;
        }
        chnk_end = (chunk_max - chunk);
        for (ix = 0; ix <= chnk_end; ix++) {
            l2ep = soc_mem_table_idx_to_pointer(unit, mem,
                                                 L2_ENTRY_T *, l2e, ix);
            if (!soc_mem_field32_get(unit, mem, l2ep, VALIDf)) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem, l2ep, PENDINGf)) {
                continue;
            }
            
            /* Match the Key Type to TRILL */
            key_type = soc_mem_field32_get(unit, mem, l2ep, KEY_TYPEf);
            if (key_type != TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS) {
                if (key_type != TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT) {
                    if (key_type != TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG) {
                        continue;
                    }
                }
            }

            if ((tree_id != soc_mem_field32_get (unit, mem, l2ep,
                                                  TRILL_NONUC_NETWORK_LONG__TREE_IDf)) &&
                (tree_id != soc_mem_field32_get(unit, mem, l2ep,
                                                 TRILL_NONUC_NETWORK_SHORT__TREE_IDf))){
                continue;
            }
            
            if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
                rv = BCM_E_RESOURCE;
                break;
            }
            if (((rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, l2ep)) < 0) ||
                ((rv = soc_l2x_sync_delete(unit, (uint32 *) l2ep,
                                           (chunk + ix), 0)) < 0)) {
                SOC_L2_DEL_SYNC_UNLOCK(soc);
                break;
            }
            SOC_L2_DEL_SYNC_UNLOCK(soc);
        }
        if (rv < 0) {
            break;
        }
    }
    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, l2e);

    return rv;
}

/*
 * Purpose:
 *      Get TRILL Multicast entry
 * Parameters:
 *      unit            - Device unit number
 *      key             - defines entry type
 *      vid             - Customer Vlan ID
 *      mac             - Customer MAC
 *      trill_tree_id   - specify Multicast Tree ID
 *      group           - (OUT) Multicast group
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int 
bcm_td_l2_trill_multicast_entry_get(int unit, int key, bcm_vlan_t vid, 
                                       bcm_mac_t mac, uint8 trill_tree_id, bcm_multicast_t *group)
{
    L2_ENTRY_T  l2_entry;
    soc_mem_t mem; 
    soc_field_t field_key;
    int rv=BCM_E_NONE;
    int l2_index=0;

    sal_memset(&l2_entry, 0, sizeof (l2_entry));

    if (SOC_IS_TRIUMPH3(unit)) {
        mem = L2_ENTRY_1m;
    } else {
        mem = L2Xm;
    }

    switch (key) {
        case TR_L2_HASH_KEY_TYPE_BRIDGE:
            field_key = TR_L2_HASH_KEY_TYPE_BRIDGE;
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                L2__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry, 
                                 L2__MAC_ADDRf, mac);
            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
            field_key =  TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS;
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                L2__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry,
                                 L2__MAC_ADDRf, mac);
            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
            field_key = TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT; 
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                TRILL_NONUC_NETWORK_SHORT__TREE_IDf,
                                trill_tree_id);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                TRILL_NONUC_NETWORK_SHORT__VLAN_IDf, vid);
            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
            field_key = TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG; 
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__TREE_IDf,
                                trill_tree_id);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                TRILL_NONUC_NETWORK_LONG__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__MAC_ADDRESSf, mac);
            break;
                        
        default:
            break;
    }

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &l2_index, 
                        (void *)&l2_entry, (void *)&l2_entry, 0);
                 
    if (BCM_FAILURE(rv)) {
        return rv;
    } 

    switch (key) {
        case TR_L2_HASH_KEY_TYPE_BRIDGE:
                _BCM_MULTICAST_GROUP_SET(*group, 
                                  _BCM_MULTICAST_TYPE_L2,  
                                  soc_mem_field32_get (unit, mem, (uint32 *)&l2_entry,
                                    L2__L2MC_PTRf));
                break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
            if (SOC_IS_TRIUMPH3(unit)) {
                _BCM_MULTICAST_GROUP_SET(*group, 
                                  _BCM_MULTICAST_TYPE_TRILL,  
                                  soc_mem_field32_get (unit, mem, (uint32 *)&l2_entry,
                                    L2__L2MC_PTRf));
                break;
            }  else if (SOC_IS_TRIDENT2X(unit)) {
                _BCM_MULTICAST_GROUP_SET(*group, 
                                  _BCM_MULTICAST_TYPE_TRILL,  
                                  soc_mem_field32_get (unit, mem, (uint32 *)&l2_entry,
                                    L2__L3MC_PTRf));
                break;
            } else {
                _BCM_MULTICAST_GROUP_SET(*group, 
                                  _BCM_MULTICAST_TYPE_TRILL,  
                                  soc_mem_field32_get (unit, mem, (uint32 *)&l2_entry,
                                    TRILL_NONUC_ACCESS__L3MC_PTRf));
                break;
            }

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
                _BCM_MULTICAST_GROUP_SET(*group, 
                                  _BCM_MULTICAST_TYPE_TRILL,  
                                  soc_mem_field32_get (unit, mem, (uint32 *)&l2_entry,
                                    TRILL_NONUC_NETWORK_SHORT__L3MC_INDEXf));
                break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
                _BCM_MULTICAST_GROUP_SET(*group, 
                                  _BCM_MULTICAST_TYPE_TRILL,  
                                  soc_mem_field32_get (unit, mem, (uint32 *)&l2_entry,
                                    TRILL_NONUC_NETWORK_LONG__L3MC_INDEXf));
                break;
        default:
            break;
    }
    return rv;
}


/*
 * Description:
 *      Traverse  TRILL Long and Short Multicast entries
 * Parameters:
 *     unit             - device number
 *     trav_st        - Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int
bcm_td_l2_trill_network_multicast_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st)
{
   /* Indexes to iterate over memories, chunks and entries */
    int        chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int        buf_size, chunksize, chnk_end;
    /* Buffer to store chunk of L2 table we currently work on */
    uint32     *l2_trill_tbl_chnk;
    L2_ENTRY_T *l2_entry;
    int        rv = BCM_E_NONE;
    soc_mem_t  mem;
    int        key_type=-1;
    uint16     tree_id = 0;
    bcm_trill_multicast_entry_t trill_mc;

    mem = L2Xm;
    
    sal_memset (&trill_mc, 0, sizeof (bcm_trill_multicast_entry_t));

    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    l2_trill_tbl_chnk = soc_cm_salloc(unit, buf_size, "trill network multicast traverse");
    if (NULL == l2_trill_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); chnk_idx <= mem_idx_max; 
         chnk_idx += chunksize) {
        sal_memset((void *)l2_trill_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) < mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, l2_trill_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            l2_entry = soc_mem_table_idx_to_pointer(unit, mem, L2_ENTRY_T *, 
                                             l2_trill_tbl_chnk, ent_idx);

            if (!soc_mem_field32_get(unit, mem, l2_entry, VALIDf)) {
                continue;
            }

            key_type = soc_mem_field32_get(unit, mem, l2_entry, KEY_TYPEf);
          
            /* Match the Key Type to TRILL */
            if (key_type != TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG) {
                if (key_type != TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT) {
                    continue;
                }
            }

            switch (key_type) {

                case  TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
                                tree_id = soc_mem_field32_get(unit, mem, l2_entry,
                                                 TRILL_NONUC_NETWORK_SHORT__TREE_IDf);
                                /* Check for Valid Tree_Id */
                                if (tree_id >= BCM_MAX_NUM_TRILL_TREES ) {
                                   continue;
                                }
                                (void) bcm_td_trill_root_name_get (unit, tree_id, &trill_mc.root_name);
                                _BCM_MULTICAST_GROUP_SET(trill_mc.group, 
                                                  _BCM_MULTICAST_TYPE_TRILL,  
                                                  soc_mem_field32_get (unit, mem, l2_entry,
                                                    TRILL_NONUC_NETWORK_SHORT__L3MC_INDEXf));
                                trill_mc.c_vlan = soc_mem_field32_get (unit, mem, l2_entry,
                                                  TRILL_NONUC_NETWORK_SHORT__VLAN_IDf);
                                break;

                case  TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
                                tree_id = soc_mem_field32_get (unit, mem, l2_entry,
                                                  TRILL_NONUC_NETWORK_LONG__TREE_IDf);
                                /* Check for Valid Tree_Id */
                                if (tree_id >= BCM_MAX_NUM_TRILL_TREES ) {
                                    continue;
                                }
                                (void) bcm_td_trill_root_name_get (unit, tree_id, &trill_mc.root_name);
                                _BCM_MULTICAST_GROUP_SET(trill_mc.group, 
                                                  _BCM_MULTICAST_TYPE_TRILL,  
                                                  soc_mem_field32_get (unit, mem, l2_entry,
                                                    TRILL_NONUC_NETWORK_LONG__L3MC_INDEXf));
                                trill_mc.c_vlan = soc_mem_field32_get (unit, mem, l2_entry,
                                                  TRILL_NONUC_NETWORK_LONG__VLAN_IDf);
                                soc_mem_mac_addr_get(unit, mem, l2_entry,
                                                   TRILL_NONUC_NETWORK_LONG__MAC_ADDRESSf, trill_mc.c_dmac);
                                break;

            }
            trill_mc.flags = BCM_TRILL_MULTICAST_STATIC;
            rv = trav_st->user_cb(unit, &trill_mc, trav_st->user_data);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    soc_cm_sfree(unit, l2_trill_tbl_chnk);
    return rv;        
}

/*
 * Description:
 *      Traverse  TRILL Access to Network Multicast entries
 * Parameters:
 *     unit             - device number
 *     trav_st        - Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int
bcm_td_l2_trill_access_multicast_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st)
{
   /* Indexes to iterate over memories, chunks and entries */
    int        chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int        buf_size, chunksize, chnk_end;
    /* Buffer to store chunk of L2 table we currently work on */
    uint32     *l2_trill_tbl_chnk;
    L2_ENTRY_T *l2_entry;
    int        rv = BCM_E_NONE;
    soc_mem_t  mem;
    int        key_type=-1;
    bcm_trill_multicast_entry_t  trill_mc;

    mem = L2Xm;
    
    sal_memset (&trill_mc, 0, sizeof (bcm_trill_multicast_entry_t));

    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    l2_trill_tbl_chnk = soc_cm_salloc(unit, buf_size, "trill access multicast traverse");
    if (NULL == l2_trill_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); chnk_idx <= mem_idx_max; 
         chnk_idx += chunksize) {
        sal_memset((void *)l2_trill_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) < mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, l2_trill_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            l2_entry = soc_mem_table_idx_to_pointer(unit, mem, L2_ENTRY_T *, 
                                             l2_trill_tbl_chnk, ent_idx);

            if (!soc_mem_field32_get(unit, mem, l2_entry, VALIDf)) {
                continue;
            }
            key_type = soc_mem_field32_get(unit, mem, l2_entry, KEY_TYPEf);
            if (key_type != TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS) {
                continue;
            } else {
                _BCM_MULTICAST_GROUP_SET(trill_mc.group, 
                                  _BCM_MULTICAST_TYPE_TRILL,  
                                  soc_mem_field32_get(unit, mem, l2_entry,
                                     TRILL_NONUC_ACCESS__L3MC_PTRf));
                trill_mc.c_vlan = soc_mem_field32_get(unit, mem, l2_entry,
                                     TRILL_NONUC_ACCESS__VLAN_IDf);
                soc_mem_mac_addr_get(unit, mem, l2_entry,
                                     TRILL_NONUC_ACCESS__MAC_ADDRf, trill_mc.c_dmac);
            }
            trill_mc.flags = BCM_TRILL_MULTICAST_ACCESS_TO_NETWORK | BCM_TRILL_MULTICAST_STATIC;
            rv = bcm_td_multicast_trill_rootname_get(unit, trill_mc.group, &trill_mc.root_name);
            if (BCM_FAILURE(rv)) {
                 continue;
            }

            rv = trav_st->user_cb(unit, &trill_mc, trav_st->user_data);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    soc_cm_sfree(unit, l2_trill_tbl_chnk);
    return rv;        
}

/*
 * Description:
 *      Modify  TRILL short Multicast entries
 * Parameters:
 *     unit             - device number
 *     trav_st        - Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int
bcm_td_l2_trill_vlan_multicast_entry_modify (int unit, bcm_vlan_t vlan, int set_bit)
{
   /* Indexes to iterate over memories, chunks and entries */
    int        chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int        buf_size, chunksize, chnk_end;
    /* Buffer to store chunk of L2 table we currently work on */
    uint32     *l2_trill_tbl_chnk;
    L2_ENTRY_T *l2_entry;
    int        rv = BCM_E_NONE;
    soc_mem_t  mem;
    int        key_type=-1;
    int        key_comp=-1;
    uint16     tree_id = 0;
    bcm_trill_multicast_entry_t trill_mc;
    int count=0;

#if defined (BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit)) {
        mem = L2Xm;
        /* coverity[mixed_enums] */
        key_comp = TD2_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT;
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined (BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        mem = L2_ENTRY_1m;
        /* Suppress coverity warning, because enum values used here
         * for key_comp are defined differently based on device type
         */
        /* coverity[mixed_enums] */
        key_comp = SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_SHORT_TRILL_NONUC_NETWORK_SHORT;
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        mem = L2Xm;
        /* Suppress coverity warning, because enum values used here
         * for key_comp are defined differently based on device type
         */
        /* coverity[mixed_enums] */
        key_comp = TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT;
    }

    sal_memset (&trill_mc, 0, sizeof (bcm_trill_multicast_entry_t));

    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    l2_trill_tbl_chnk = soc_cm_salloc(unit, buf_size, "trill network multicast traverse");
    if (NULL == l2_trill_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); chnk_idx <= mem_idx_max; 
         chnk_idx += chunksize) {
        sal_memset((void *)l2_trill_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) < mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, l2_trill_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            l2_entry = soc_mem_table_idx_to_pointer(unit, mem, L2_ENTRY_T *, 
                                             l2_trill_tbl_chnk, ent_idx);

            if (!soc_mem_field32_get(unit, mem, l2_entry, VALIDf)) {
                continue;
            }

            key_type = soc_mem_field32_get(unit, mem, l2_entry, KEY_TYPEf);
          
            /* Match the Key Type to TRILL */
            if (key_type != key_comp) {
               continue;
            } else {
                tree_id = soc_mem_field32_get(unit, mem, l2_entry,
                                 TRILL_NONUC_NETWORK_SHORT__TREE_IDf);
                /* Check for Valid Tree_Id */
                if (tree_id >= BCM_MAX_NUM_TRILL_TREES ) {
                   continue;
                }
                trill_mc.c_vlan = soc_mem_field32_get (unit, mem, l2_entry,
                                  TRILL_NONUC_NETWORK_SHORT__VLAN_IDf);
                if (vlan ==  trill_mc.c_vlan) {
                   if (set_bit) {
                       soc_mem_field32_set(unit, mem, (uint32 *)l2_entry, 
                             TRILL_NONUC_NETWORK_SHORT__TRILL_ACCESS_RECEIVERS_PRESENTf, 0x1);
                   } else {
                       soc_mem_field32_set(unit, mem, (uint32 *)l2_entry, 
                             TRILL_NONUC_NETWORK_SHORT__TRILL_ACCESS_RECEIVERS_PRESENTf, 0x0);
                   }
                   count ++;
                }
            }
        }
        if (count > 0) {
            rv = soc_mem_write_range(unit, mem,
                                     MEM_BLOCK_ANY, chnk_idx,
                                     chnk_idx_max, l2_trill_tbl_chnk);
        } 
    }
    soc_cm_sfree(unit, l2_trill_tbl_chnk);
    return rv;        
}

#endif /* INCLUDE_L3 */

#endif /* BCM_TRIDENT_SUPORT */

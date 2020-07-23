/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XGS3 L2 User Table Manipulation API routines.
 *
 * The L2 User Table (L2_USER_ENTRY) is used for various
 * purposes including:
 *
 *  - L2 table overflow
 *  - BPDU addresses
 *  - 802.1 reserved addresses
 *  - L2 address blocks (using MAC address mask)
 *
 * The table is segmented into two parts of which the
 * first is referenced by index, and the second is not.
 * This segmentation is implemented to facilitate
 * different requirements from the BCM API functions.
 */

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/l2u.h>
#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>

#ifdef BCM_XGS3_SWITCH_SUPPORT

#define SOC_MEM_COMPARE_RETURN(a, b) {		\
        if ((a) < (b)) { return -1; }		\
        if ((a) > (b)) { return  1; }		\
}

/*
 * Function:
 *	_soc_mem_cmp_l2u
 * Purpose:
 *	Compare two L2 User table entries
 * Parameters:
 *	unit, entry A and entry B
 * Returns:
 *	Negative for A < B, zero for A == B, positive for A > B
 */
STATIC int
_soc_mem_cmp_l2u(int unit, void *ent_a, void *ent_b)
{
    uint32 mask_a[SOC_MAX_MEM_WORDS / 2], mask_b[SOC_MAX_MEM_WORDS / 2];
    uint32 key_type_a, key_type_b;
    sal_mac_addr_t mac_a, mac_b;
    vlan_id_t vlan_a, vlan_b;

    soc_L2_USER_ENTRYm_field_get(unit, ent_a, MASKf, mask_a);
    soc_L2_USER_ENTRYm_field_get(unit, ent_b, MASKf, mask_b);
    SOC_MEM_COMPARE_RETURN(mask_a[0], mask_b[0]);
    SOC_MEM_COMPARE_RETURN(mask_a[1], mask_b[1]);

    vlan_a = soc_L2_USER_ENTRYm_field32_get(unit, ent_a, VLAN_IDf);
    vlan_b = soc_L2_USER_ENTRYm_field32_get(unit, ent_b, VLAN_IDf);
    SOC_MEM_COMPARE_RETURN(vlan_a, vlan_b);

    if (soc_mem_field_valid(unit, L2_USER_ENTRYm, KEY_TYPEf)) {
        key_type_a = soc_L2_USER_ENTRYm_field32_get(unit, ent_a, KEY_TYPEf);
        key_type_b = soc_L2_USER_ENTRYm_field32_get(unit, ent_b, KEY_TYPEf);
        SOC_MEM_COMPARE_RETURN(key_type_a, key_type_b);
    }

    soc_L2_USER_ENTRYm_mac_addr_get(unit, ent_a, MAC_ADDRf, mac_a);
    soc_L2_USER_ENTRYm_mac_addr_get(unit, ent_b, MAC_ADDRf, mac_b);

    return ENET_CMP_MACADDR(mac_a, mac_b);
}

/*
 * Function:
 *      _soc_l2u_overlap_get
 * Purpose:
 *      Check if L2 User table entries overlap
 * Parameters:
 *	unit, entry A and entry B
 * Returns:
 *      True if entries overlap
 */
STATIC int 
_soc_l2u_overlap_get(int unit, l2u_entry_t *ent_a, l2u_entry_t *ent_b)
{
    uint32 mask_a[SOC_MAX_MEM_WORDS / 2], mask_b[SOC_MAX_MEM_WORDS / 2];
    uint32 mac_a[2], mac_b[2];
    vlan_id_t vlan_a, vlan_b;

    soc_L2_USER_ENTRYm_field_get(unit, ent_a, MASKf, mask_a);
    soc_L2_USER_ENTRYm_field_get(unit, ent_b, MASKf, mask_b);

    vlan_a = soc_L2_USER_ENTRYm_field32_get(unit, ent_a, VLAN_IDf);
    vlan_a &= (mask_a[1] >> 16) & (mask_b[1] >> 16);
    vlan_b = soc_L2_USER_ENTRYm_field32_get(unit, ent_b, VLAN_IDf);
    vlan_b &= (mask_a[1] >> 16) & (mask_b[1] >> 16);
    if (vlan_a == vlan_b) {
        return TRUE;
    }

    soc_L2_USER_ENTRYm_field_get(unit, ent_a, MAC_ADDRf, mac_a);
    mac_a[0] &= mask_a[0] & mask_b[0];
    mac_a[1] &= mask_a[1] & mask_b[1];
    soc_L2_USER_ENTRYm_field_get(unit, ent_b, MAC_ADDRf, mac_b);
    mac_b[0] &= mask_a[0] & mask_b[0];
    mac_b[1] &= mask_a[1] & mask_b[1];

    if (sal_memcmp(mac_a, mac_b, sizeof(mac_a)) == 0) {
        return TRUE;
    }

    return FALSE;
}

/*
 * Function:
 *      soc_l2u_overlap
 * Purpose:
 *      Check for overlapping entry in L2 User table
 * Parameters:
 *      unit - SOC unit number
 *	entry - entry to check
 *      index - (OUT) index where found
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_EXISTS
 */
int 
soc_l2u_overlap_check(int unit, l2u_entry_t *entry, int *index)
{
    l2u_entry_t l2u_entry;
    int i, i_min, i_max, skip_l2u;

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);
    if (skip_l2u) {
        return SOC_E_UNAVAIL;
    }

    i_min = soc_mem_index_min(unit, L2_USER_ENTRYm);
    i_max = soc_mem_index_max(unit, L2_USER_ENTRYm);

    for (i = i_min; i <= i_max; i++) {
        SOC_IF_ERROR_RETURN(
            READ_L2_USER_ENTRYm(unit, MEM_BLOCK_ANY, i, &l2u_entry));
        if (!soc_L2_USER_ENTRYm_field32_get(unit, &l2u_entry, VALIDf)) {
            continue;
        }
        if (_soc_l2u_overlap_get(unit, &l2u_entry, entry)) {
            *index = i;
            return SOC_E_EXISTS;
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_l2u_search
 * Purpose:
 *      Search for entry in L2 User table
 * Parameters:
 *      unit - SOC unit number
 *	key - entry to look for
 *	result - matching entry (if found)
 *      index - (OUT) index where found
 * Returns:
 *      SOC_E_NONE      - matching entry found
 *      SOC_E_NOT_FOUND - no matching entries found
 */
int 
soc_l2u_search(int unit, l2u_entry_t *key, l2u_entry_t *result, int *index)
{
    int rv = SOC_E_NONE;
    l2u_entry_t *entry;
    int i, i_max, i_min;
    soc_mem_t mem = L2_USER_ENTRYm;
    void *buf = NULL;

    buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, mem), "l2_user");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }

    i_min = soc_mem_index_min(unit, mem);
    i_max = soc_mem_index_max(unit, mem);

    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, i_min, i_max, buf);
    if (SOC_FAILURE(rv)) {
        soc_cm_sfree(unit, buf);
        return rv;
    }

    for (i = i_min; i <= i_max; i++) {
        entry = soc_mem_table_idx_to_pointer(unit, mem, l2u_entry_t *, buf, i);
        if (!soc_mem_field32_get(unit, mem, entry, VALIDf)) {
            continue;
        }
        if (_soc_mem_cmp_l2u(unit, entry, key) == 0) {
            *index = i;
            sal_memcpy(result, entry, sizeof(l2u_entry_t));
            soc_cm_sfree(unit, buf);
            return SOC_E_NONE;
        }
    }
    soc_cm_sfree(unit, buf);
    return SOC_E_NOT_FOUND;
}

/*
 * Function:
 *      soc_l2u_find_unused
 * Purpose:
 *      Search for unused entry in L2 User table
 * Parameters:
 *      unit - SOC unit number
 *	key - entry to be stored in the unused entry
 *      index - (OUT) index where found
 * Returns:
 *      SOC_E_NONE      - free entry found
 *      SOC_E_FULL      - no free entries found
 */
int 
soc_l2u_find_free_entry(int unit, l2u_entry_t *key, int *free_index)
{
    l2u_entry_t entry, free_mask;
    int index, i, entry_words, rv;
    int start, end, step;
    uint32 mask[SOC_MAX_MEM_WORDS / 2];

    entry_words = soc_mem_entry_words(unit, L2_USER_ENTRYm);

    sal_memset(&free_mask, 0, sizeof(free_mask));
    soc_L2_USER_ENTRYm_field32_set(unit, &free_mask, VALIDf, 1);

    soc_L2_USER_ENTRYm_field_get(unit, key, MASKf, mask);
    if (mask[0] == 0xffffffff && (mask[1] & 0xffff) == 0xffff) {
        /* Search from high priority end */
        start = soc_mem_index_min(unit, L2_USER_ENTRYm);
        end = soc_mem_index_max(unit, L2_USER_ENTRYm) + 1;
        step = 1;
    } else {
        start = soc_mem_index_max(unit, L2_USER_ENTRYm);
        end = soc_mem_index_min(unit, L2_USER_ENTRYm) - 1;
        step = -1;
    }
    for (index = start; index != end; index += step) {
        rv = READ_L2_USER_ENTRYm(unit, MEM_BLOCK_ANY, index, &entry);
        if (SOC_SUCCESS(rv)) {
            for (i = 0; i < entry_words; i++) {
                if (entry.entry_data[i] & free_mask.entry_data[i]) {
                    break;
                }
            }
            if (i == entry_words) {
                *free_index = index;
                return SOC_E_NONE;
            }
        }
    }

    return SOC_E_FULL;
}

/*
 * Function:
 *      soc_l2u_insert
 * Purpose:
 *      Add entry to L2 User table
 * Parameters:
 *      unit - SOC unit number
 *	entry - pointer to l2u_entry_t
 *	index - where to insert or -1 for default insertion policy
 *	index_used - (OUT) entry used if index = -1
 * Returns:
 *      SOC_E_NONE - successfully added entry
 *      SOC_E_FULL - table is full
 *      SOC_E_FAIL - overlapping entry already exists
 * Notes:
 *      If index -1 is specified, an entry with no zeros in the MAC
 *      address mask will be inserted at the first unused slot found
 *      when searching from the high priority end of the table. Any
 *      other entry will be inserted at the first unused entry found
 *      when searching from the low priority end of the table.
 */
int 
soc_l2u_insert(int unit, l2u_entry_t *entry, int index, int *index_used)
{
    l2u_entry_t l2u_entry;
    int i, i_max, i_min, rv;

    i_min = soc_mem_index_min(unit, L2_USER_ENTRYm);
    i_max = soc_mem_index_max(unit, L2_USER_ENTRYm);

    if (index == -1) {

        soc_mem_lock(unit, L2_USER_ENTRYm);

        /* Avoid duplicates */
        rv = soc_l2u_search(unit, entry, &l2u_entry, &i);
        if (rv != SOC_E_NOT_FOUND) {
            soc_mem_unlock(unit, L2_USER_ENTRYm);
            *index_used = i;
            return rv;
        }

        rv = soc_l2u_find_free_entry(unit, entry, &i);
        soc_mem_unlock(unit, L2_USER_ENTRYm);
        if (SOC_FAILURE(rv)) {
            return rv;
        }
        index = i;

    } else if (index < i_min || index > i_max) {
        return SOC_E_PARAM;
    }

    soc_mem_lock(unit, L2_USER_ENTRYm);

    sal_memcpy(&l2u_entry, entry, sizeof(l2u_entry));
    rv = WRITE_L2_USER_ENTRYm(unit, MEM_BLOCK_ALL, index, &l2u_entry);

    soc_mem_unlock(unit, L2_USER_ENTRYm);

    *index_used = index;

    return rv;
}

/*
 * Function:
 *      soc_l2u_get
 * Purpose:
 *      Get entry from L2 User table
 * Parameters:
 *      unit - SOC unit number
 *	entry - entry to delete, or NULL if index is used
 *	index - entry to get
 * Returns:
 *      SOC_E_XXX
 */
int 
soc_l2u_get(int unit, l2u_entry_t *entry, int index)
{
    int i_max, i_min, rv;

    if (entry == NULL) {
        return SOC_E_PARAM;
    }

    i_min = soc_mem_index_min(unit, L2_USER_ENTRYm);
    i_max = soc_mem_index_max(unit, L2_USER_ENTRYm);

    if (index < i_min || index > i_max) {
        return SOC_E_PARAM;
    }

    soc_mem_lock(unit, L2_USER_ENTRYm);

    rv = READ_L2_USER_ENTRYm(unit, MEM_BLOCK_ANY, index, entry);

    soc_mem_unlock(unit, L2_USER_ENTRYm);

    return rv;
}

/*
 * Function:
 *      soc_l2u_delete
 * Purpose:
 *      Delete entry from L2 User table
 * Parameters:
 *      unit - SOC unit number
 *	entry - entry to delete, or NULL if index is used
 *	index - entry to delete, must be -1 if non-zero key
 *	index_deleted - (OUT) entry deleted if index = -1
 * Returns:
 *      SOC_E_XXX
 */
int 
soc_l2u_delete(int unit, l2u_entry_t *entry, int index, int *index_deleted)
{
    l2u_entry_t l2u_entry;
    int i, i_max, i_min, rv;

    /* We need either an entry or a valid index */
    if (entry == NULL && index == -1) {
        return SOC_E_PARAM;
    }

    i_min = soc_mem_index_min(unit, L2_USER_ENTRYm);
    i_max = soc_mem_index_max(unit, L2_USER_ENTRYm);

    soc_mem_lock(unit, L2_USER_ENTRYm);

    if (index == -1) {
        if (soc_l2u_search(unit, entry, &l2u_entry, &i) < 0) {
            /* Not in table */
            soc_mem_unlock(unit, L2_USER_ENTRYm);
            return SOC_E_NOT_FOUND;
        }
        index = i;

    } else if (index < i_min || index > i_max) {
        soc_mem_unlock(unit, L2_USER_ENTRYm);
        return SOC_E_PARAM;
    }

    sal_memset(&l2u_entry, 0, sizeof(l2u_entry));
    rv = WRITE_L2_USER_ENTRYm(unit, MEM_BLOCK_ALL, index, &l2u_entry);

    soc_mem_unlock(unit, L2_USER_ENTRYm);

    *index_deleted = index;

    return rv;
}

/*
 * Function:
 *	soc_l2u_delete_by_key
 * Description:
 *	Delete L2 User entries by search key.
 * Parameters:
 *	unit  - device unit
 *      key   - L2 User table search key
 * Returns:
 *	BCM_E_XXX
 */

int
soc_l2u_delete_by_key(int unit, l2u_key_t *key)
{
    l2u_entry_t l2u;
    sal_mac_addr_t addr;
    int i, i_min, i_max, rv;
    int value, skip_l2u;

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);
    if (skip_l2u) {
        return SOC_E_UNAVAIL;
    }

    i_min = soc_mem_index_min(unit, L2_USER_ENTRYm);
    i_max = soc_mem_index_max(unit, L2_USER_ENTRYm);

    soc_mem_lock(unit, L2_USER_ENTRYm);

    for (i = i_min; i <= i_max; i++) {
        rv = READ_L2_USER_ENTRYm(unit, MEM_BLOCK_ANY, i, &l2u);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2_USER_ENTRYm);
            return rv;
        }
        if (!_l2u_field32_get(unit, &l2u, VALIDf)) {
            continue;
        }
        if (key->flags & L2U_KEY_MAC) {
            _l2u_mac_addr_get(unit, &l2u, MAC_ADDRf, addr);
            if (ENET_CMP_MACADDR(key->mac, addr) != 0) {
                continue;
            }
        }
        
        value = _l2u_field32_get(unit, &l2u, VLAN_IDf);
        if ((key->flags & L2U_KEY_VLAN) && value != key->vlan) {
            continue;
        }

        value = _l2u_field32_get(unit, &l2u, PORT_TGIDf);
        if ((key->flags & L2U_KEY_PORT) && value  != key->port) {
            continue;
        }

        value = _l2u_field32_get(unit, &l2u, MODULE_IDf);
        if ((key->flags & L2U_KEY_MODID) && value != key->modid) {
            continue;
        }
        sal_memset(&l2u, 0, sizeof(l2u));
        rv = WRITE_L2_USER_ENTRYm(unit, MEM_BLOCK_ALL, i, &l2u);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2_USER_ENTRYm);
            return rv;
        }
    }

    soc_mem_unlock(unit, L2_USER_ENTRYm);

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2u_delete_all
 * Description:
 *	Delete all L2 User entries.
 * Parameters:
 *	unit  - device unit
 * Returns:
 *	BCM_E_XXX
 */

int
soc_l2u_delete_all(int unit)
{
    l2u_entry_t l2u;
    int i, i_min, i_max, skip_l2u, rv = SOC_E_NONE;

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);
    if (skip_l2u) {
        return SOC_E_UNAVAIL;
    }

    i_min = soc_mem_index_min(unit, L2_USER_ENTRYm);
    i_max = soc_mem_index_max(unit, L2_USER_ENTRYm);

    soc_mem_lock(unit, L2_USER_ENTRYm);

    sal_memset(&l2u, 0, sizeof(l2u));
    for (i = i_min; i <= i_max; i++) {
        rv = WRITE_L2_USER_ENTRYm(unit, MEM_BLOCK_ALL, i, &l2u);
        if (SOC_FAILURE(rv)) {
            break;
        }
    }

    soc_mem_unlock(unit, L2_USER_ENTRYm);

    return rv;
}

#endif /* BCM_XGS3_SWITCH_SUPPORT */

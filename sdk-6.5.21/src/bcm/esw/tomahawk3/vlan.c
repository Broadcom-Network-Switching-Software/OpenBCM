/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        vlan.c
 * Purpose:     Provide low-level access to Tomahawk3 VLAN resources
 */
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/trx.h>
#ifdef BCM_TRX_SUPPORT
#include <bcm_int/esw/triumph.h>
#endif
#include <bcm_int/esw/tomahawk3.h>
#include <bcm_int/esw/firebolt.h>

/* VLAN Cross Connect DB declaraions/definitions */
typedef struct _bcm_th3_vlan_xconnect_db_s {
    SHR_BITDCL valid[_SHR_BITDCLSIZE(BCM_VLAN_COUNT)]; /* Valid bitmap */
    SHR_BITDCL dest_type[_SHR_BITDCLSIZE(BCM_VLAN_COUNT*2)]; /* TGID/Port */
    uint8 destination[BCM_VLAN_COUNT*2]; /* Trunk/Port ID */
} _bcm_th3_vlan_xconnect_db_t;

static
_bcm_th3_vlan_xconnect_db_t *_bcm_th3_xconnect_db[BCM_MAX_NUM_UNITS]= {0};

#define BCM_TH3_VLAN_XC_DB_DESTTYPE_OFFSET(_ovid_)  ((_ovid_) * 2)
#define BCM_TH3_VLAN_XC_DB_DESTTYPE1_OFFSET(_ovid_) (((_ovid_) * 2) + 1)

/* VLAN Cross Connect Mutex defintion/macros */
sal_mutex_t _bcm_th3_xconnect_mtx[BCM_MAX_NUM_UNITS] = {0};

#define BCM_TH3_VLAN_XCONNECT_MUTEX_ACQ(_unit_) \
        sal_mutex_take(_bcm_th3_xconnect_mtx[(_unit_)], sal_mutex_FOREVER);

#define BCM_TH3_VLAN_XCONNECT_MUTEX_REL(_unit_) \
        sal_mutex_give(_bcm_th3_xconnect_mtx[(_unit_)]);

#define ING_ACTION_PROFILE_DEFAULT  0
/* Cache of Ingress Vlan Translate Action Profile Table */
static soc_profile_mem_t *ing_action_profile[BCM_MAX_NUM_UNITS] = {NULL};

/* Cache of Egress Vlan Translate Action Profile Table */
static soc_profile_mem_t *egr_action_profile[BCM_MAX_NUM_UNITS] = {NULL};

/* Cache of VLAN Port Bitmap Profile Table */
static soc_profile_mem_t *vlan_pbmp_profile[BCM_MAX_NUM_UNITS] = {NULL};

/*
 * Function:
 *      _bcm_th3_vlan_pbmp_profile_init
 *
 * Purpose:
 *       to initialize hardware PORT_BITMAP_PROFILE table
 *       and allocate memory to cache hardware tables in RAM.
 *
 * Parameters:
 *      unit - (IN)  Device Number
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      Allocate memory to cache the profile tables and initialize.
 *      If memory to cache the profile table is already allocated,
 *      just initialize the table.
 */
int
_bcm_th3_vlan_pbmp_profile_init(int unit) {

    int rv = BCM_E_NONE;
    uint32 profile_idx;
    soc_mem_t mem;
    int entry_dwords;
    bcm_pbmp_t pbmp;
#if defined(BCM_WARM_BOOT_SUPPORT)
    int mem_idx;
    vlan_2_tab_entry_t vlan_2_entry;
    const int mem_entry_cnt = soc_mem_index_count(unit, VLAN_2_TABm);
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (!soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {
        return BCM_E_UNAVAIL;
    }

    /* Initialize the PORT_BITMAP_PROFILE table */
    if (NULL == vlan_pbmp_profile[unit]) {
        vlan_pbmp_profile[unit] =
            sal_alloc(sizeof(soc_profile_mem_t), "VLAN PBMP Profile");
        if (NULL == vlan_pbmp_profile[unit]) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(vlan_pbmp_profile[unit]);
    }

    /* Create profile table cache( or re-init if it already exists */
    mem = PORT_BITMAP_PROFILEm;
    entry_dwords = sizeof(port_bitmap_profile_entry_t) / sizeof(uint32);

    SOC_IF_ERROR_CLEAN_RETURN(
        soc_profile_mem_create(
            unit, &mem, &entry_dwords, 1, vlan_pbmp_profile[unit]),
        (void)_bcm_th3_vlan_pbmp_profile_destroy(unit));

    /* Create a default entry with empty port bitmap */
    SOC_PBMP_CLEAR(pbmp);
    rv = _bcm_th3_vlan_pbmp_profile_entry_add(unit, pbmp, &profile_idx);
    if (BCM_FAILURE(rv)) {
        (void)_bcm_th3_vlan_pbmp_profile_destroy(unit);
        return rv;
    }

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        /* Sync the SW profile data structure with actual indexes is use */
        for (mem_idx = 0 ; mem_idx < mem_entry_cnt ; mem_idx++) {

            SOC_IF_ERROR_CLEAN_RETURN(
                READ_VLAN_2_TABm(unit, MEM_BLOCK_ANY, mem_idx, &vlan_2_entry),
                (void)_bcm_th3_vlan_pbmp_profile_destroy(unit));

            if (soc_mem_field32_get(unit, VLAN_2_TABm, &vlan_2_entry, VALIDf)) {
                profile_idx = soc_mem_field32_get(
                    unit, VLAN_2_TABm, &vlan_2_entry, PORT_BITMAP_PROFILE_PTRf);
                SOC_PROFILE_MEM_REFERENCE(
                    unit, vlan_pbmp_profile[unit], profile_idx, 1);
                SOC_PROFILE_MEM_ENTRIES_PER_SET(
                    unit, vlan_pbmp_profile[unit], profile_idx, 1);
            }
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
    return rv;
}

/*
 * Function:
 *      _bcm_th3_vlan_pbmp_profile_destroy
 *
 * Purpose:
 *      to deinitialize hardware PORT_BITMAP_PROFILE and
 *      deallocate cached RAM memory for the hardware table.
 *
 * Parameters:
 *      unit - (IN)  Device Number
 *
 * Returns:
 *      BCM_E_FAIL
 *      BCM_E_NONE
 *
 * Notes:
 */
int
_bcm_th3_vlan_pbmp_profile_destroy(int unit) {
    int rv;

    if (vlan_pbmp_profile[unit]) {
        rv = soc_profile_mem_destroy(unit, vlan_pbmp_profile[unit]);
        if (BCM_FAILURE(rv)) {
            return BCM_E_FAIL;
        }

        sal_free(vlan_pbmp_profile[unit]);
        vlan_pbmp_profile[unit] = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_vlan_pbmp_profile_entry_add
 *
 * Purpose:
 *      add a new entry to PORT_BITMAP_PROFILE table
 *
 * Parameters:
 *      unit         - (IN)  Device Number
 *      port_bitmap  - (IN)  Bitmap entry to be added into the memory
 *      index        - (OUT) Index into the profile table.
 *                           Only valid if the function returns BCM_E_NONE
 *
 * Returns:
 *      BCM_E_PARAM
 *      BCM_E_UNAVAIL
 *      BCM_E_FULL
 *      BCM_E_FAIL
 *      BCM_E_NONE
 *
 * Notes:
 */
int
_bcm_th3_vlan_pbmp_profile_entry_add(
    int unit, bcm_pbmp_t port_bitmap, uint32 *index) {

    int profile_idx;
    soc_error_t rv;
    port_bitmap_profile_entry_t vlan_pbmp_entry;

    /* Perform sanity checks */
    if (NULL == index) {
        return BCM_E_PARAM;
    }
    if (!soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {
        return BCM_E_UNAVAIL;
    }

    /* Create an entry from the given port bitmap */
    sal_memset(&vlan_pbmp_entry, 0, sizeof(port_bitmap_profile_entry_t));
    soc_mem_pbmp_field_set(
        unit, PORT_BITMAP_PROFILEm, &vlan_pbmp_entry, BITMAPf, &port_bitmap);

    /* Insert the entry into the profile memory */
    rv = soc_profile_mem_single_table_add(
            unit, vlan_pbmp_profile[unit], &vlan_pbmp_entry, 1, &profile_idx);

    /* Convert the SOC return value to BCM return code */
    if (SOC_FAILURE(rv)) {
        return (((SOC_E_FULL == rv) || (SOC_E_RESOURCE == rv)) ?
                BCM_E_FULL : BCM_E_FAIL);
    } else {
        /* Populate the given index buffer if the entry add succeeded */
        *index = profile_idx;
        return BCM_E_NONE;
    }
}

/*
 * Function:
 *      _bcm_th3_vlan_pbmp_profile_entry_delete
 *
 * Purpose:
 *      delete an entry from PORT_BITMAP_PROFILE table
 *
 * Parameters:
 *      unit         - (IN) Device Number
 *      index        - (IN) Index into the profile table.
 *
 * Returns:
 *      BCM_E_PARAM
 *      BCM_E_UNAVAIL
 *      BCM_E_NOT_FOUND
 *      BCM_E_FAIL
 *      BCM_E_NONE
 *
 * Notes:
 */
int
_bcm_th3_vlan_pbmp_profile_entry_delete(int unit, uint32 index) {

    soc_error_t rv;

    /* Perform sanity checks */
    if (!soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {
        return BCM_E_UNAVAIL;
    }

    /* Call soc interface to delete an entry */
    rv = soc_profile_mem_delete(unit, vlan_pbmp_profile[unit], index);

    if (SOC_FAILURE(rv)) {
        return ((SOC_E_PARAM!=rv && SOC_E_NOT_FOUND!=rv) ? BCM_E_FAIL : rv);
    } else {
        return BCM_E_NONE;
    }
}

/*
 * Function:
 *      _bcm_th3_vlan_pbmp_port_entry_get
 *
 * Purpose:
 *      Retrieve the port bitmap for a given index.
 *
 * Parameters:
 *      unit     - (IN)  Device Number
 *      index    - (IN)  Index into the profile table.
 *      pbmp     - (OUT) Bitmap entry to be retrieved from the memory
 *
 * Returns:
 *      BCM_E_PARAM
 *      BCM_E_UNAVAIL
 *      BCM_E_NOT_FOUND
 *      BCM_E_FAIL
 *      BCM_E_NONE
 *
 * Notes:
 */
int _bcm_th3_vlan_pbmp_profile_entry_get(
    int unit, uint32 index, bcm_pbmp_t *pbmp) {

    soc_error_t rv;
    port_bitmap_profile_entry_t vlan_pbmp_entry;

    /* Perform sanity checks */
    if (NULL == pbmp) {
        return BCM_E_PARAM;
    }
    if (!soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {
        return BCM_E_UNAVAIL;
    }

    /* Retrieve an entry from the profile memory */
    sal_memset(&vlan_pbmp_entry, 0, sizeof(port_bitmap_profile_entry_t));
    rv = soc_profile_mem_single_table_get(
            unit, vlan_pbmp_profile[unit], (int) index, 1, &vlan_pbmp_entry);

    /* Convert the SOC return value to BCM return code */
    if (SOC_FAILURE(rv)) {
        return ((SOC_E_PARAM != rv && SOC_E_NOT_FOUND != rv) ? BCM_E_FAIL : rv);
    } else {
        /* Extract the port bitmap from the table entry */
        soc_mem_pbmp_field_get(
            unit, PORT_BITMAP_PROFILEm, &vlan_pbmp_entry, BITMAPf, pbmp);
        return BCM_E_NONE;
    }
}

/*
 * Function:
 *      _bcm_th3_vlan_pbmp_index_get
 *
 * Purpose:
 *      Retrieve the portbitmap profile memory
 *      index from corresponding VLAN memory
 *
 * Parameters:
 *      unit     - (IN)  Device Number
 *      vlan_id  - (IN)  VLAN Identifier
 *      index    - (OUT) Index into the profile table.
 *
 * Returns:
 *      BCM_E_PARAM
 *      BCM_E_UNAVAIL
 *      BCM_E_NOT_FOUND
 *      BCM_E_FAIL
 *      BCM_E_NONE
 *
 * Notes:
 *      The function assumes that the VLAN_2 memory is already locked
 *      and hence no locks are explicitly acquired or released
 */
int _bcm_th3_vlan_pbmp_index_get(int unit, bcm_vlan_t vlan_id, uint32 *index) {

    soc_error_t         rv;
    vlan_2_tab_entry_t  vlan_2_entry;
    const soc_mem_t     mem = VLAN_2_TABm;

    /* Perform sanity checks */
    if (NULL == index || !BCM_VLAN_VALID(vlan_id)) {
        return BCM_E_PARAM;
    }
    if (!soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {
        return BCM_E_UNAVAIL;
    }

    /* Read the corresponding VLAN table entry */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, (int)vlan_id, &vlan_2_entry);
    if (SOC_FAILURE(rv)) {
        return BCM_E_FAIL;
    }

    /* Check if the given VLAN is an existing one */
    if (!soc_mem_field32_get(unit, mem, &vlan_2_entry, VALIDf)) {
        return BCM_E_NOT_FOUND;
    }

    /* Populate the given buffer with the correct index value */
    *index =
        soc_mem_field32_get(unit, mem, &vlan_2_entry, PORT_BITMAP_PROFILE_PTRf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_vlan_pbmp_index_set
 *
 * Purpose:
 *      Insert the portbitmap profile memory
 *      index to corresponding VLAN memory entry
 *
 * Parameters:
 *      unit     - (IN)  Device Number
 *      vlan_id  - (IN)  VLAN Identifier
 *      index    - (IN)  Index into the profile table.
 *
 * Returns:
 *      BCM_E_PARAM
 *      BCM_E_UNAVAIL
 *      BCM_E_NOT_FOUND
 *      BCM_E_FAIL
 *      BCM_E_NONE
 *
 * Notes:
 *      The function assumes that the VLAN_2 memory is already locked
 *      and hence no locks are explicitly acquired or released
 */
int _bcm_th3_vlan_pbmp_index_set(int unit, bcm_vlan_t vlan_id, uint32 index)
{

    soc_error_t         rv;
    vlan_2_tab_entry_t  vlan_2_entry;
    const soc_mem_t     mem = VLAN_2_TABm;

    /* Perform sanity checks */
    if (!BCM_VLAN_VALID(vlan_id)) {
        return BCM_E_PARAM;
    }
    

    if (!soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {
        return BCM_E_UNAVAIL;
    }

    /* Read the corresponding VLAN table entry */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, (int)vlan_id, &vlan_2_entry);
    if (SOC_FAILURE(rv)) {
        return BCM_E_FAIL;
    }

    /* Check if the given VLAN is an existing one */
    if (!soc_mem_field32_get(unit, mem, &vlan_2_entry, VALIDf)) {
        return BCM_E_NOT_FOUND;
    }

    /* Populate the given buffer with the correct index value */
    soc_mem_field32_set(
        unit, mem, &vlan_2_entry, PORT_BITMAP_PROFILE_PTRf, index);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, (int) vlan_id, &vlan_2_entry);

    return (SOC_FAILURE(rv) ? BCM_E_FAIL : BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_th3_vlan_pbmp_port_update
 *
 * Purpose:
 *      Add/Remove the ports in the given bitmap for a VLAN
 *      The table entry for the VLAN is provided as input
 *      Hence the profile index is set to the same entry
 *
 * Parameters:
 *      unit        - (IN)  Device Number
 *      vt          - (IN/OUT)  VLAN table entry.
 *      pbmp        - (IN) Bitmap of ports to be added/removed from the VLAN.
 *      operation   - (IN) 0 for Add, 1 for Remove
 *
 * Returns:
 *      BCM_E_PARAM
 *      BCM_E_UNAVAIL
 *      BCM_E_NOT_FOUND
 *      BCM_E_FAIL
 *      BCM_E_NONE
 *
 * Notes:
 */
STATIC int
_bcm_th3_vlan_pbmp_port_update(
    int unit, vlan_2_tab_entry_t *vt, bcm_pbmp_t *pbmp, uint8 operation)
{
    int                 rv = BCM_E_NONE;
    uint32              old_profile_idx;
    uint32              new_profile_idx;
    bcm_pbmp_t          cur_pbmp, org_pbmp;

    if (0 != operation && 1 != operation) {
        return BCM_E_UNAVAIL;
    }

    BCM_PBMP_CLEAR(cur_pbmp);

    /* Retrieve the profile index from the VLAN table entry */
    old_profile_idx =
        soc_mem_field32_get(unit, VLAN_2_TABm, vt, PORT_BITMAP_PROFILE_PTRf);

    /* Retrieve the port bmp from the profile table given the index */
    BCM_IF_ERROR_RETURN(
        _bcm_th3_vlan_pbmp_profile_entry_get(
            unit, old_profile_idx, &cur_pbmp));

    /* Cache the retrieved port bitmap for later use */
    BCM_PBMP_ASSIGN(org_pbmp, cur_pbmp);

    /* Perform read-modify-write operation on bitmap */
    if (operation) {
        BCM_PBMP_REMOVE(cur_pbmp, *pbmp);
    } else {
        BCM_PBMP_OR(cur_pbmp, *pbmp);
    }

    /* Only perform the profile modification if the pbmp has changed */
    if (BCM_PBMP_NEQ(cur_pbmp, org_pbmp)) {

        /* First delete the old profile entry from the profile table */
        rv =  _bcm_th3_vlan_pbmp_profile_entry_delete(unit, old_profile_idx);

        if (BCM_SUCCESS(rv)) {
            /* Insert the modified port bitmap into the profile table */
            rv = _bcm_th3_vlan_pbmp_profile_entry_add(unit,
                                                      cur_pbmp,
                                                      &new_profile_idx);

            /* If new entry add failed then add back the old pbmp */
            if (BCM_FAILURE(rv)) {
                if (BCM_SUCCESS(
                        _bcm_th3_vlan_pbmp_profile_entry_add(unit,
                                                        org_pbmp,
                                                        &old_profile_idx))) {
                    /* Set the old index into the given VLAN table entry */
                    soc_mem_field32_set(unit, VLAN_2_TABm,
                        vt, PORT_BITMAP_PROFILE_PTRf, old_profile_idx);
                }
            } else {
                /* Set the new profile index into the given VLAN table entry */
                soc_mem_field32_set(unit,
                    VLAN_2_TABm, vt, PORT_BITMAP_PROFILE_PTRf, new_profile_idx);
            }
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_th3_vlan_pbmp_port_add
 *
 * Purpose:
 *      Add the ports in the given bitmap for a VLAN
 *      The table entry for the VLAN is provided as input
 *      Hence the profile index is set to the same entry
 *
 * Parameters:
 *      unit    - (IN)  Device Number
 *      entry   - (IN/OUT)  VLAN table entry.
 *      pbmp    - (IN) Bitmap of ports to be added from the VLAN.
 *
 * Returns:
 *      BCM_E_PARAM
 *      BCM_E_UNAVAIL
 *      BCM_E_NOT_FOUND
 *      BCM_E_FAIL
 *      BCM_E_NONE
 *
 * Notes:
 *      It is assumed that the given vlan entry is genuine and has
 *      all the relevant parameters in correct state. Also the VLAN
 *      valid bit is assumed to be set to begin with.
 */
int _bcm_th3_vlan_pbmp_port_add(int unit, void *entry, bcm_pbmp_t *pbmp)
{
    /* Perform sanity checks */
    if (NULL == entry || NULL == pbmp) {
        return BCM_E_PARAM;
    }
    if (!soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {
        return BCM_E_UNAVAIL;
    }

    return(_bcm_th3_vlan_pbmp_port_update(unit, entry, pbmp, 0));
}

/*
 * Function:
 *      _bcm_th3_vlan_pbmp_port_remove
 *
 * Purpose:
 *      Remove the ports in the given bitmap for a VLAN
 *      The table entry for the VLAN is provided as input
 *      Hence the profile index is set to the same entry
 *
 * Parameters:
 *      unit    - (IN)  Device Number
 *      entry   - (IN/OUT)  VLAN table entry.
 *      pbmp    - (IN) Bitmap of ports to be removed from the VLAN.
 *
 * Returns:
 *      BCM_E_PARAM
 *      BCM_E_UNAVAIL
 *      BCM_E_NOT_FOUND
 *      BCM_E_FAIL
 *      BCM_E_NONE
 *
 * Notes:
 *      It is assumed that the given vlan entry is genuine and has
 *      all the relevant parameters in correct state. Also the VLAN
 *      valid bit is assumed to be set to begin with.
 */
int _bcm_th3_vlan_pbmp_port_remove(int unit, void *entry, bcm_pbmp_t *pbmp)
{
    /* Perform sanity checks */
    if (NULL == entry || NULL == pbmp) {
        return BCM_E_PARAM;
    }
    if (!soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {
        return BCM_E_UNAVAIL;
    }

    return(_bcm_th3_vlan_pbmp_port_update(unit, entry, pbmp, 1));
}

/*
 * Function:
 *      _bcm_th3_vlan_action_profile_init
 *
 * Purpose  : to initialize hardware ING_VLAN_TAG_ACTION_PROFILE and
 *            EGR_VLAN_TAG_ACTION_PROFILE tables and allocate memory
 *            to cache hardware tables in RAM.
 *
 * Parameters:
 *      unit         - (IN)  Device Number
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      Allocate memory to cache the profile tables and initialize.
 *      If memory to cache the profile table is already allocated, just
 *      initialize the table.
 */
int
_bcm_th3_vlan_action_profile_init(int unit)
{
    int i, idx, num_ports;
    ing_vlan_tag_action_profile_entry_t ing_profile_entry;
    egr_vlan_tag_action_profile_entry_t egr_profile_entry;
    uint32 rval;
    soc_mem_t mem;
    int temp_index;
    int entry_idxmax;
    int entry_words;
    int entry_idxmin = 0;
    const uint32 ing_ent_sz = sizeof(ing_vlan_tag_action_profile_entry_t);

    /* Initialize the ING_VLAN_TAG_ACTION_PROFILE table */
    if (ing_action_profile[unit] == NULL) {
        ing_action_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                             "Ing Action Profile Mem");
        if (ing_action_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(ing_action_profile[unit]);
    }

    mem = ING_VLAN_TAG_ACTION_PROFILEm;
    entry_idxmax = soc_mem_index_max(unit, mem);
    entry_words = ing_ent_sz / sizeof(uint32);
    if (SOC_FAILURE(
            soc_profile_mem_index_create(unit, &mem, &entry_words,
                                     &entry_idxmin, &entry_idxmax,
                                     NULL, 1, ing_action_profile[unit]))) {
        sal_free(ing_action_profile[unit]);
        ing_action_profile[unit] = NULL;
        return BCM_E_FAIL;
    }

    if (SOC_WARM_BOOT(unit)) {
        soc_mem_t port_mem = PORT_TABm;

        /* Increment the ref count for all ports */
        for (i = 0; i < soc_mem_index_count(unit, port_mem); i++) {
            SOC_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit, i,
                                TAG_ACTION_PROFILE_PTRf, &idx));
            SOC_IF_ERROR_RETURN(
                soc_profile_mem_reference(unit, ing_action_profile[unit],
                idx - entry_idxmin, 1));
            SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, ing_action_profile[unit],
                                            idx - entry_idxmin, 1);
        }

        /* One extra incr to preserve location ING_ACTION_PROFILE_DEFAULT */
        SOC_IF_ERROR_RETURN(
            soc_profile_mem_reference(unit, ing_action_profile[unit],
            ING_ACTION_PROFILE_DEFAULT, 1));
    } else {
        /* Initialize the ING_ACTION_PROFILE_DEFAULT. For untagged and
         * inner-tagged packets, always add an outer tag.
         */
        soc_mem_t port_mem = PORT_TABm;
        sal_memset(&ing_profile_entry, 0, ing_ent_sz);
        soc_mem_field32_set(unit, mem, &ing_profile_entry, UT_OTAG_ACTIONf,
                            _BCM_TRX_TAG_ACTION_ENCODE(bcmVlanActionAdd));
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            soc_mem_field32_set(unit, mem, &ing_profile_entry, UT_OPRI_ACTIONf,
                                _BCM_TRX_PRI_CFI_ACTION_ENCODE(bcmVlanActionAdd));
            soc_mem_field32_set(unit, mem, &ing_profile_entry, UT_OCFI_ACTIONf,
                                _BCM_TRX_PRI_CFI_ACTION_ENCODE(bcmVlanActionAdd));
        }
        soc_mem_field32_set(unit, mem, &ing_profile_entry, SOT_POTAG_ACTIONf,
                            _BCM_TRX_TAG_ACTION_ENCODE(bcmVlanActionReplace));
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_single_table_add(unit,
                ing_action_profile[unit], &ing_profile_entry, 1, &temp_index));

        for (i = 0; i < soc_mem_index_count(unit, port_mem); i++) {
            SOC_IF_ERROR_RETURN(
                soc_profile_mem_reference(unit, ing_action_profile[unit],
                ING_ACTION_PROFILE_DEFAULT, 1));
        }
    }

    /* Initialize the EGR_VLAN_TAG_ACTION_PROFILE table */
    if (egr_action_profile[unit] == NULL) {
        egr_action_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                             "Egr Action Profile Mem");
        if (egr_action_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(egr_action_profile[unit]);
    }

    /* Create profile table cache (or re-init if it already exists) */
    mem = EGR_VLAN_TAG_ACTION_PROFILEm;
    entry_words = sizeof(egr_vlan_tag_action_profile_entry_t) / sizeof(uint32);
    SOC_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                               egr_action_profile[unit]));

    SOC_PBMP_COUNT(PBMP_ALL(unit), num_ports);
    if (SOC_WARM_BOOT(unit)) {
        /* Increment the ref count for all ports */
        PBMP_ALL_ITER(unit, i) {
            SOC_IF_ERROR_RETURN
                (READ_EGR_VLAN_CONTROL_3r(unit, i, &rval));
            idx = soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
                    rval, TAG_ACTION_PROFILE_PTRf);
            SOC_PROFILE_MEM_REFERENCE(unit, egr_action_profile[unit], idx, 1);
            SOC_PROFILE_MEM_ENTRIES_PER_SET(
                unit, egr_action_profile[unit], idx, 1);
        }

        /* 1 extra increment to preserve location EGR_ACTION_PROFILE_DEFAULT */
        SOC_PROFILE_MEM_REFERENCE(unit, egr_action_profile[unit],
                              BCM_TH3_EGR_ACTION_PROFILE_DEFAULT, 1);
    } else {
        /* Initialize the BCM_TH3_EGR_ACTION_PROFILE_DEFAULT to have all
         * actions as NOP (0).
         */
        sal_memset(&egr_profile_entry, 0,
                    sizeof(egr_vlan_tag_action_profile_entry_t));
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_single_table_add(unit,
                egr_action_profile[unit], &egr_profile_entry, 1, &temp_index));

        /* Increment the ref count for all ports */
        SOC_PROFILE_MEM_REFERENCE(unit, egr_action_profile[unit],
                                BCM_TH3_EGR_ACTION_PROFILE_DEFAULT, num_ports);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_vlan_action_profile_detach
 *
 * Purpose  : to deallocate resources for hardware
 *            ING_VLAN_TAG_ACTION_PROFILE and
 *            EGR_VLAN_TAG_ACTION_PROFILE tables and
 *            their caches correspondingly in RAM.
 *
 * Parameters:
 *      unit     - (IN)  Device Number
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int
_bcm_th3_vlan_action_profile_detach(int unit)
{
    int rv;

    /* De-initialize the ING_VLAN_TAG_ACTION_PROFILE table */
    if (ing_action_profile[unit]) {
        rv = soc_profile_mem_destroy(unit, ing_action_profile[unit]);
        BCM_IF_ERROR_RETURN(rv);

        sal_free(ing_action_profile[unit]);
        ing_action_profile[unit] = NULL;
    }

    /* De-initialize the EGR_VLAN_TAG_ACTION_PROFILE table */
    if (egr_action_profile[unit]) {
        rv = soc_profile_mem_destroy(unit, egr_action_profile[unit]);
        BCM_IF_ERROR_RETURN(rv);

        sal_free(egr_action_profile[unit]);
        egr_action_profile[unit] = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_vlan_action_verify
 *
 * Purpose  : to validate all members of action structure
 *
 * Parameters:
 *      unit     - (IN)  Device Number
 *      action   - (IN)  tag actions to be verified
 *
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_PARAM
 *
 * Notes:
 */
STATIC int
_bcm_th3_vlan_action_verify(int unit, bcm_vlan_action_set_t *action)
{
    if (NULL == action) {
        return BCM_E_PARAM;
    }
    VLAN_CHK_ID(unit, action->new_outer_vlan);

    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        if (action->priority < -1 ||
            action->priority > 7 || action->new_outer_cfi > 1) {
            return BCM_E_PARAM;
        }
    }

    /* Double Tag operations not supported */
    if ((bcmVlanActionNone != action->dt_outer) ||
        (bcmVlanActionNone != action->dt_outer_prio) ||
        (bcmVlanActionNone != action->dt_outer_pkt_prio) ||
        (bcmVlanActionNone != action->dt_outer_cfi) ||
        (bcmVlanActionNone != action->dt_inner) ||
        (bcmVlanActionNone != action->dt_inner_prio) ||
        (bcmVlanActionNone != action->dt_inner_pkt_prio) ||
        (bcmVlanActionNone != action->dt_inner_cfi)) {
        return BCM_E_UNAVAIL;
    }

    /* Two tags not supported */
    if ((bcmVlanActionNone != action->ot_inner) ||
        (bcmVlanActionNone != action->ot_inner_pkt_prio) ||
        (bcmVlanActionNone != action->ot_inner_cfi) ||
        (bcmVlanActionNone != action->it_outer) ||
        (bcmVlanActionNone != action->it_outer_pkt_prio) ||
        (bcmVlanActionNone != action->it_outer_cfi)) {
        return BCM_E_UNAVAIL;
    }

    /* No Inner tag operations supported */
    if ((bcmVlanActionNone != action->it_inner) ||
        (bcmVlanActionNone != action->it_inner_prio) ||
        (bcmVlanActionNone != action->it_inner_pkt_prio) ||
        (bcmVlanActionNone != action->it_inner_cfi) ||
        (bcmVlanActionNone != action->ut_inner) ||
        (bcmVlanActionNone != action->ut_inner_pkt_prio) ||
        (bcmVlanActionNone != action->ut_inner_cfi)) {
        return BCM_E_UNAVAIL;
    }

    switch (action->ot_outer) {
        case bcmVlanActionNone:
        case bcmVlanActionReplace:
        case bcmVlanActionDelete:
            break;
        default:
            return BCM_E_PARAM;
    }

    switch (action->ot_outer_prio) {
        case bcmVlanActionNone:
        case bcmVlanActionReplace:
        case bcmVlanActionDelete:
            break;
        default:
            return BCM_E_PARAM;
    }

    switch (action->ot_outer_pkt_prio) {
        case bcmVlanActionNone:
            break;
        case bcmVlanActionReplace:
            if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
                return BCM_E_UNAVAIL;
            }
            break;
        default:
            return BCM_E_PARAM;
    }

    switch (action->ot_outer_cfi) {
        case bcmVlanActionNone:
            break;
        case bcmVlanActionReplace:
            if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
                return BCM_E_UNAVAIL;
            }
            break;
        default:
            return BCM_E_PARAM;
    }

    switch (action->ut_outer) {
        case bcmVlanActionNone:
        case bcmVlanActionAdd:
            break;
        default:
            return BCM_E_PARAM;
    }

    switch (action->ut_outer_pkt_prio) {
        case bcmVlanActionNone:
            break;
        case bcmVlanActionAdd:
            if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
                return BCM_E_UNAVAIL;
            }
            break;
        default:
            return BCM_E_PARAM;
    }

    switch (action->ut_outer_cfi) {
        case bcmVlanActionNone:
            break;
        case bcmVlanActionAdd:
            if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
                return BCM_E_UNAVAIL;
            }
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_vlan_action_profile_entry_add
 *
 * Purpose  : add a new entry to the given vlan
 *            action profile table at given index
 *
 * Parameters:
 *      unit     - (IN)  Device Number
 *      vtap_mem - (IN)  VLAN Tag Action Profile Memory
 *      action   - (IN)  tag actions to be added
 *      index    - (IN)  Index of profile memory where to add the entry
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
STATIC int
_bcm_th3_vlan_action_profile_entry_add(int unit,
                                        soc_mem_t vtap_mem,
                                        bcm_vlan_action_set_t *action,
                                        int *index)
{
    soc_profile_mem_t *profile = NULL;
    egr_vlan_tag_action_profile_entry_t egr_vtap_entry;
    ing_vlan_tag_action_profile_entry_t ing_vtap_entry;
    void *vtap_entry = NULL;

    if (ING_VLAN_TAG_ACTION_PROFILEm == vtap_mem) {
        profile = ing_action_profile[unit];
        vtap_entry = (void *)&ing_vtap_entry;
        sal_memset(vtap_entry, 0, sizeof(ing_vlan_tag_action_profile_entry_t));
    } else if (EGR_VLAN_TAG_ACTION_PROFILEm ==  vtap_mem) {
        profile = egr_action_profile[unit];
        vtap_entry = (void *)&egr_vtap_entry;
        sal_memset(vtap_entry, 0, sizeof(egr_vlan_tag_action_profile_entry_t));
    } else {
        return BCM_E_PARAM;
    }

    soc_mem_field32_set(unit, vtap_mem, vtap_entry, SOT_OTAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->ot_outer));
    soc_mem_field32_set(unit, vtap_mem, vtap_entry, SOT_POTAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->ot_outer_prio));
    soc_mem_field32_set(unit, vtap_mem, vtap_entry, UT_OTAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->ut_outer));

    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        soc_mem_field32_set
            (unit, vtap_mem, vtap_entry, SOT_OPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ot_outer_pkt_prio));
        soc_mem_field32_set
            (unit, vtap_mem, vtap_entry, SOT_OCFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ot_outer_cfi));
        soc_mem_field32_set
            (unit, vtap_mem, vtap_entry, UT_OPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ut_outer_pkt_prio));
        soc_mem_field32_set
            (unit, vtap_mem, vtap_entry, UT_OCFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ut_outer_cfi));
    }

    return
        (soc_profile_mem_single_table_add(unit, profile, vtap_entry, 1, index));
}

/*
 * Function:
 *      _bcm_th3_ing_vlan_action_profile_entry_add
 *
 * Purpose  : add a new entry to the ING_VLAN_TAG_ACTION_PROFILE
 *            memory at given index
 *
 * Parameters:
 *      unit     - (IN)  Device Number
 *      action   - (IN)  tag actions to be added
 *      index    - (IN)  Index of profile memory where to add the entry
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      It is assumed that the action data structure is thorougly scrutinized
 *      before the function is called. Hence no additional checks are performed
 */
int
_bcm_th3_ing_vlan_action_profile_entry_add(int unit,
                                      bcm_vlan_action_set_t *action,
                                      int *index)
{
    return(_bcm_th3_vlan_action_profile_entry_add(unit,
                                            ING_VLAN_TAG_ACTION_PROFILEm,
                                            action,
                                            index));
}

/*
 * Function:
 *      _bcm_th3_egr_vlan_action_profile_entry_add
 *
 * Purpose  : add a new entry to the EGR_VLAN_TAG_ACTION_PROFILE
 *            memory at given index
 *
 * Parameters:
 *      unit     - (IN)  Device Number
 *      action   - (IN)  tag actions to be added
 *      index    - (IN)  Index of profile memory where to add the entry
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      It is assumed that the action data structure is thorougly scrutinized
 *      before the function is called. Hence no additional checks are performed
 */
int
_bcm_th3_egr_vlan_action_profile_entry_add(int unit,
                                      bcm_vlan_action_set_t *action,
                                      int *index)
{
    return(_bcm_th3_vlan_action_profile_entry_add(unit,
                                            EGR_VLAN_TAG_ACTION_PROFILEm,
                                            action,
                                            index));
}

/*
 * Function:
 *      _bcm_th3_vlan_action_profile_entry_decode
 *
 * Purpose  : Decode and save the tag actions from the given
 *            memory entry for the given TAG Action memory
 *
 * Parameters:
 *      unit        - (IN)  Device Number
 *      mem         - (IN)  VLAN Tag Action memory
 *      vtap_entry  - (IN)  Retrieved entry from the memory 'mem'
 *      action      - (OUT)  save the decoded tag actions here
 *
 * Returns:
 *
 * Notes:
 */
STATIC void
_bcm_th3_vlan_action_profile_entry_decode(int unit,
                                            soc_mem_t mem,
                                            void *vtap_entry,
                                            bcm_vlan_action_set_t *action)
{
    uint32 val;

    val = soc_mem_field32_get(unit, mem, vtap_entry, SOT_OTAG_ACTIONf);
    action->ot_outer = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, vtap_entry, SOT_POTAG_ACTIONf);
    action->ot_outer_prio = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, vtap_entry, UT_OTAG_ACTIONf);
    action->ut_outer = _BCM_TRX_TAG_ACTION_DECODE(val);

    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        val = soc_mem_field32_get(unit, mem, vtap_entry, SOT_OPRI_ACTIONf);
        action->ot_outer_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, vtap_entry, SOT_OCFI_ACTIONf);
        action->ot_outer_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, vtap_entry, UT_OPRI_ACTIONf);
        action->ut_outer_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, vtap_entry, UT_OCFI_ACTIONf);
        action->ut_outer_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);
    } else {
        action->ot_outer_pkt_prio = bcmVlanActionNone;
        action->ot_outer_cfi = bcmVlanActionNone;
        action->ut_outer_pkt_prio = bcmVlanActionNone;
        action->ut_outer_cfi = bcmVlanActionNone;
    }

    action->dt_outer =
    action->dt_outer_prio =
    action->dt_outer_pkt_prio =
    action->dt_outer_cfi =
    action->dt_inner =
    action->dt_inner_prio =
    action->dt_inner_pkt_prio =
    action->dt_inner_cfi =
    action->ot_inner =
    action->ot_inner_pkt_prio =
    action->ot_inner_cfi =
    action->it_outer_pkt_prio =
    action->it_outer_cfi =
    action->it_outer =
    action->it_inner_pkt_prio =
    action->it_inner_cfi =
    action->it_inner =
    action->it_inner_prio =
    action->ut_inner =
    action->ut_inner_cfi =
    action->ut_inner_pkt_prio = bcmVlanActionNone;
}

/*
 * Function:
 *      _bcm_th3_ing_vlan_action_profile_entry_get
 *
 * Purpose  : get tag actions for given index to the Ingress VLAN Tag action
 *
 * Parameters:
 *      unit    - (IN)  Device Number
 *      action  - (OUT)  save the decoded tag actions here
 *      index   - (IN)  index to the ING_VLAN_TAG_ACTION_PROFILE
 *
 * Returns:
 *
 * Notes:
 */
void
_bcm_th3_ing_vlan_action_profile_entry_get(int unit,
                                      bcm_vlan_action_set_t *action,
                                      int index)
{
    ing_vlan_tag_action_profile_entry_t ing_vtap_entry;

    sal_memset(&ing_vtap_entry, 0, sizeof(ing_vlan_tag_action_profile_entry_t));

    /* Retrieve an entry from the profile memory */
    (void) soc_profile_mem_single_table_get(
            unit, ing_action_profile[unit], index, 1, &ing_vtap_entry);

    _bcm_th3_vlan_action_profile_entry_decode(unit,
                ING_VLAN_TAG_ACTION_PROFILEm, &ing_vtap_entry, action);
}

/*
 * Function:
 *      _bcm_th3_egr_vlan_action_profile_entry_get
 *
 * Purpose  : get a copy of cached vlan action profile table
 *
 * Parameters:
 *      unit    - (IN)  Device Number
 *      action  - (OUT)  save the decoded tag actions here
 *      index   - (IN)  index to the ING_VLAN_TAG_ACTION_PROFILE
 *
 * Returns:
 *
 * Notes:
 */
void
_bcm_th3_egr_vlan_action_profile_entry_get(int unit,
                                          bcm_vlan_action_set_t *action,
                                          uint32 index)
{
    egr_vlan_tag_action_profile_entry_t *vtap_entry;

    vtap_entry = SOC_PROFILE_MEM_ENTRY(unit, egr_action_profile[unit],
                                  egr_vlan_tag_action_profile_entry_t *,
                                  index);
    _bcm_th3_vlan_action_profile_entry_decode(unit,
                EGR_VLAN_TAG_ACTION_PROFILEm, vtap_entry, action);
}

/*
 * Function:
 *      _bcm_th3_ing_vlan_action_profile_entry_delete
 *
 * Purpose  : remove an entry from Ingress vlan action profile table
 *
 * Parameters:
 *      unit    - (IN)  Device Number
 *      index   - (IN)  index of the entry to be deleted
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int
_bcm_th3_ing_vlan_action_profile_entry_delete(int unit, uint32 index)
{
    return soc_profile_mem_delete(unit, ing_action_profile[unit], index);
}

/*
 * Function:
 *      _bcm_th3_egr_vlan_action_profile_entry_delete
 *
 * Purpose  : remove an entry from Egress vlan action profile table
 *
 * Parameters:
 *      unit    - (IN)  Device Number
 *      index   - (IN)  index of the entry to be deleted
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int
_bcm_th3_egr_vlan_action_profile_entry_delete(int unit, uint32 index)
{
    return soc_profile_mem_delete(unit, egr_action_profile[unit], index);
}

/*
 * Function:
 *      _bcm_th3_egr_vlan_action_profile_entry_increment
 *
 * Purpose  : increment the refcount for default egr vlan action profile table
 *             entry
 *
 * Parameters:
 *      unit - (IN) BCM unit.
 *      index - (IN) Profile table entry index
 *
 * Returns:
 *
 * Notes:
 */
void
_bcm_th3_egr_vlan_action_profile_entry_increment(int unit, uint32 index)
{
    soc_profile_mem_reference(unit, egr_action_profile[unit], index, 1);
}

/*
 * Function:
 *      _bcm_th3_vlan_port_egress_default_action_set
 *
 * Purpose:
 *      Set the egress port's default vlan tag actions
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int
_bcm_th3_vlan_port_egress_default_action_set(int unit, bcm_port_t port,
                                            bcm_vlan_action_set_t *action)
{
    int rv;
    uint32 rval;
    int profile_idx, old_profile_idx;

    BCM_IF_ERROR_RETURN(_bcm_th3_vlan_action_verify(unit, action));
    BCM_IF_ERROR_RETURN(
        _bcm_th3_egr_vlan_action_profile_entry_add(unit, action, &profile_idx));

    rv = READ_EGR_VLAN_CONTROL_2r(unit, port, &rval);
    if (rv < 0) {
        goto error;
    }

    if (-1 == action->priority) {
        /* point to default map profile(0) for default priority action */
        soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval, OPRI_CFI_SELf, 1);
    } else {
        soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval, OPRI_CFI_SELf, 0);
        soc_reg_field_set(
                    unit, EGR_VLAN_CONTROL_2r, &rval, OPRIf, action->priority);
        soc_reg_field_set(
                unit, EGR_VLAN_CONTROL_2r, &rval, OCFIf, action->new_outer_cfi);
    }
    soc_reg_field_set(
        unit, EGR_VLAN_CONTROL_2r, &rval, OVIDf, action->new_outer_vlan);

    rv = WRITE_EGR_VLAN_CONTROL_2r(unit, port, rval);
    if (rv < 0) {
        goto error;
    }

    rv = READ_EGR_VLAN_CONTROL_3r(unit, port, &rval);
    if (rv < 0) {
        goto error;
    }
    old_profile_idx = soc_reg_field_get(
                    unit, EGR_VLAN_CONTROL_3r, rval, TAG_ACTION_PROFILE_PTRf);
    soc_reg_field_set(
        unit, EGR_VLAN_CONTROL_3r, &rval, TAG_ACTION_PROFILE_PTRf, profile_idx);

    rv = WRITE_EGR_VLAN_CONTROL_3r(unit, port, rval);
    if (rv < 0) {
       goto error;
    }

    /* Delete the old profile entry even if it is same as the new one as
     * reference count has to adjust back to old value since it must have been
     * bumped up by one when the profile entry was added in the very beginning
     * of this function.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_th3_egr_vlan_action_profile_entry_delete(unit, old_profile_idx));

    return BCM_E_NONE;
error:
    /* Undo action profile entry addition */
    BCM_IF_ERROR_RETURN
        (_bcm_th3_egr_vlan_action_profile_entry_delete(unit, profile_idx));
    return rv;
}

/*
 * Function:
 *      _bcm_th3_vlan_port_egress_default_action_get
 *
 * Purpose:
 *      Get the egress port's default vlan tag actions
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (OUT) Vlan tag actions
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int
_bcm_th3_vlan_port_egress_default_action_get(int unit, bcm_port_t port,
                                            bcm_vlan_action_set_t *action)
{
    uint32 profile_idx, rval;

    if (NULL == action) {
        return BCM_E_PARAM;
    }

    bcm_vlan_action_set_t_init(action);

    BCM_IF_ERROR_RETURN(READ_EGR_VLAN_CONTROL_3r(unit, port, &rval));
    profile_idx =
        soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
                                rval, TAG_ACTION_PROFILE_PTRf);
    _bcm_th3_egr_vlan_action_profile_entry_get(unit, action, profile_idx);

    BCM_IF_ERROR_RETURN(READ_EGR_VLAN_CONTROL_2r(unit, port, &rval));
    action->new_outer_vlan =
        soc_reg_field_get(unit, EGR_VLAN_CONTROL_2r, rval, OVIDf);

    if (!soc_reg_field_get(unit, EGR_VLAN_CONTROL_2r, rval, OPRI_CFI_SELf)) {
        action->priority =
            soc_reg_field_get(unit, EGR_VLAN_CONTROL_2r, rval, OPRIf);
        action->new_outer_cfi =
            soc_reg_field_get(unit, EGR_VLAN_CONTROL_2r, rval, OCFIf);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_vlan_port_egress_default_action_delete
 *
 * Purpose:
 *      Deletes the egress port's default vlan tag actions
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int
_bcm_th3_vlan_port_egress_default_action_delete(int unit, bcm_port_t port)
{
    uint32 profile_idx, rval;

    BCM_IF_ERROR_RETURN(
            READ_EGR_VLAN_CONTROL_3r(unit, port, &rval));
    profile_idx = soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
            rval, TAG_ACTION_PROFILE_PTRf);

    BCM_IF_ERROR_RETURN(
            _bcm_th3_egr_vlan_action_profile_entry_delete(unit, profile_idx));

    sal_memset(&rval, 0, sizeof(uint32));
    BCM_IF_ERROR_RETURN(
            WRITE_EGR_VLAN_CONTROL_3r(unit, port, rval));
    BCM_IF_ERROR_RETURN(
            WRITE_EGR_VLAN_CONTROL_2r(unit, port, rval));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_vlan_port_default_action_set
 *
 * Purpose:
 *      Set the port's default vlan tag actions
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int
_bcm_th3_vlan_port_default_action_set(int unit, bcm_port_t port,
                                     bcm_vlan_action_set_t *action)
{
    int profile_idx;
    uint32 old_profile_idx;
    bcm_port_cfg_t pcfg;

    BCM_IF_ERROR_RETURN(_bcm_th3_vlan_action_verify(unit, action));
    BCM_IF_ERROR_RETURN(
        _bcm_th3_ing_vlan_action_profile_entry_add(unit, action, &profile_idx));

    BCM_IF_ERROR_RETURN(
        mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
    old_profile_idx = pcfg.pc_vlan_action;

    pcfg.pc_vlan = action->new_outer_vlan;
    pcfg.pc_vlan_action = profile_idx;
    if (action->priority >= BCM_PRIO_MIN && action->priority <= BCM_PRIO_MAX) {
        pcfg.pc_new_opri = action->priority;
    }
    pcfg.pc_new_ocfi = action->new_outer_cfi;
    BCM_IF_ERROR_RETURN(
        mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg));

    SOC_IF_ERROR_RETURN(
        _bcm_th3_ing_vlan_action_profile_entry_delete(unit, old_profile_idx));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_vlan_port_default_action_get
 *
 * Purpose:
 *      Get the port's default vlan tag actions
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (OUT) Vlan tag actions
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int
_bcm_th3_vlan_port_default_action_get(int unit, bcm_port_t port,
                                     bcm_vlan_action_set_t *action)
{
    bcm_port_cfg_t pcfg;

    if (NULL == action) {
        return BCM_E_PARAM;
    }

    bcm_vlan_action_set_t_init(action);

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));

    _bcm_th3_ing_vlan_action_profile_entry_get( unit,
                                                action,
                                                pcfg.pc_vlan_action);
    action->new_outer_vlan = pcfg.pc_vlan;
    action->priority = pcfg.pc_new_opri;
    action->new_outer_cfi = pcfg.pc_new_ocfi;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_vlan_action_profile_entry_increment
 *
 * Purpose  : increment the refcount for a vlan action profile table entry
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      index      - (IN)
 *
 * Returns:
 *
 * Notes:
 */
STATIC void
_bcm_th3_vlan_action_profile_entry_increment(int unit, uint32 index)
{
    SOC_PROFILE_MEM_REFERENCE(unit, ing_action_profile[unit], index, 1);
}

/*
 * Function:
 *      _bcm_th3_vlan_port_default_action_delete
 *
 * Purpose:
 *      Deletes the port's default vlan tag actions
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int
_bcm_th3_vlan_port_default_action_delete(int unit, bcm_port_t port)
{
    uint32          old_profile_idx;
    bcm_port_cfg_t  pcfg;

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
    old_profile_idx = pcfg.pc_vlan_action;

    pcfg.pc_vlan = BCM_VLAN_DEFAULT;
    pcfg.pc_vlan_action = ING_ACTION_PROFILE_DEFAULT;
    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg));

    _bcm_th3_vlan_action_profile_entry_increment(
                            unit, ING_ACTION_PROFILE_DEFAULT);
    BCM_IF_ERROR_RETURN
        (_bcm_th3_ing_vlan_action_profile_entry_delete(unit, old_profile_idx));

    return (BCM_E_NONE);
}

#if defined(BCM_WARM_BOOT_SUPPORT)
/*
 * Function:
 *      _bcm_th3_vlan_xconnect_db_reinit
 *
 * Purpose:
 *       In Warmboot, fill in the Cross Connect SW cached from the L2 HW table
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int _bcm_th3_vlan_xconnect_db_reinit(int unit) {
    int rv = BCM_E_NONE;

    /* Allocate memory for the database */
    BCM_IF_ERROR_RETURN(_bcm_th3_vlan_xconnect_db_init(unit));

    /* Acquire the Mutex */
    BCM_TH3_VLAN_XCONNECT_MUTEX_ACQ(unit);

    /* Invoke appr. function to parse through L2Xm */
    rv = _bcm_th3_l2_cross_connect_entry_add_all(unit);

    /* Release the Mutex */
    BCM_TH3_VLAN_XCONNECT_MUTEX_REL(unit);

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_th3_vlan_xconnect_db_init
 *
 * Purpose:
 *      Initialize/Allocate the Cross Connect SW cache
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_MEMORY
 *
 * Notes:
 *      If the memory is already available then only task
 *      done is to invalidate all entries in SW cache.
 */
int _bcm_th3_vlan_xconnect_db_init(int unit) {

    /* Allocate the mutex, if not allocated yet */
    if (NULL == _bcm_th3_xconnect_mtx[unit]) {
        _bcm_th3_xconnect_mtx[unit] = sal_mutex_create("VlanXConnect");
        if (NULL == _bcm_th3_xconnect_mtx[unit]) {
            return BCM_E_MEMORY;
        }
    }

    /* Acquire the Mutex */
    BCM_TH3_VLAN_XCONNECT_MUTEX_ACQ(unit);

    /* Allocate the database, if not yet allocated */
    if (NULL == _bcm_th3_xconnect_db[unit]) {
        _bcm_th3_xconnect_db[unit] =
            sal_alloc(sizeof(_bcm_th3_vlan_xconnect_db_t), "XConnectDB");

        if (NULL == _bcm_th3_xconnect_db[unit]) {
            sal_free(_bcm_th3_xconnect_mtx[unit]);
            _bcm_th3_xconnect_mtx[unit] = NULL;
            return BCM_E_MEMORY;
        }
    }

    /* Clear the state to begin-with */
    SHR_BITCLR_RANGE(_bcm_th3_xconnect_db[unit]->valid, 0, BCM_VLAN_COUNT);

    /* Release the Mutex */
    BCM_TH3_VLAN_XCONNECT_MUTEX_REL(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_vlan_xconnect_db_destroy
 *
 * Purpose:
 *      Deallocate the Cross Connect SW cache
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *
 * Returns:
 *      BCM_E_NONE
 *
 * Notes:
 *      The Mutex is not destroyed
 */
int _bcm_th3_vlan_xconnect_db_destroy(int unit) {

    /* Acquire the Mutex */
    BCM_TH3_VLAN_XCONNECT_MUTEX_ACQ(unit);

    /* Deallocate memory */
    if (NULL != _bcm_th3_xconnect_db[unit]) {
        sal_free(_bcm_th3_xconnect_db[unit]);
        _bcm_th3_xconnect_db[unit] = NULL;
    }

    /* Release the Mutex */
    BCM_TH3_VLAN_XCONNECT_MUTEX_REL(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_vlan_xconnect_db_entry_add
 *
 * Purpose:
 *      Add cross connect entry to HW and SW database
 *
 * Parameters:
 *      unit  - (IN) BCM unit.
 *      ovlan - (IN) Outer VLAN ID.
 *      destt - (IN) array of 2, specifies the corresponding dst is TGID or Port
 *      destv - (IN) array of 2, TGID or Port corresponding to two gports
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      1) All incoming paramaters are assumed to be valid and hence no further
 *          checks are performed on them
 *      2) Cross Connect Mutex is acquired in the very beginning as updating the
 *          HW table and SW cache has to be atomic with respect to VLAN module
 */
int _bcm_th3_vlan_xconnect_db_entry_add(
    int unit, bcm_vlan_t ovlan, uint8 *destt, int *destv) {

    int rv = BCM_E_NONE;

    /* Acquire the Mutex */
    BCM_TH3_VLAN_XCONNECT_MUTEX_ACQ(unit);

    /* Add the entry to L2 table at first */
    rv = _bcm_th3_l2_cross_connect_add(unit, ovlan, destt, destv);

    /* Only when the entry is successfully added to HW table add it to cache */
    if ( BCM_SUCCESS(rv) ) {

        /* Add the destination value */
        _bcm_th3_xconnect_db[unit]->destination[
            BCM_TH3_VLAN_XC_DB_DESTTYPE_OFFSET(ovlan)] = (uint8) destv[0];
        _bcm_th3_xconnect_db[unit]->destination[
            BCM_TH3_VLAN_XC_DB_DESTTYPE1_OFFSET(ovlan)] = (uint8) destv[1];

        /* Add the distinguishing parameter - Trunk or Port */
        SHR_BITWRITE(_bcm_th3_xconnect_db[unit]->dest_type,
                     BCM_TH3_VLAN_XC_DB_DESTTYPE_OFFSET(ovlan), !!destt[0]);
        SHR_BITWRITE(_bcm_th3_xconnect_db[unit]->dest_type,
                     BCM_TH3_VLAN_XC_DB_DESTTYPE1_OFFSET(ovlan), !!destt[1]);

        /* Mark the entry valid */
        SHR_BITSET(_bcm_th3_xconnect_db[unit]->valid, ovlan);
    }

    /* Release the Mutex */
    BCM_TH3_VLAN_XCONNECT_MUTEX_REL(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_th3_vlan_xconnect_db_entry_delete
 *
 * Purpose:
 *      Delete cross connect entry from HW and SW database
 *
 * Parameters:
 *      unit  - (IN) BCM unit.
 *      ovlan - (IN) Outer VLAN ID.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      Cross Connect Mutex is acquired in the very beginning as updating
 *      the HW table and SW cache has to be atomic with respect to VLAN module
 */
int _bcm_th3_vlan_xconnect_db_entry_delete(int unit, bcm_vlan_t ovlan) {

    int rv = BCM_E_NONE;

#ifdef BCM_TRX_SUPPORT
    /* Acquire the Mutex */
    BCM_TH3_VLAN_XCONNECT_MUTEX_ACQ(unit);

    /* Delete the entry in L2 HW Table at first */
    rv = bcm_tr_l2_cross_connect_delete(unit, ovlan, BCM_VLAN_INVALID);

    /* Invalidate the entry in SW cache, only when HW table entry is deleted */
    if( BCM_SUCCESS(rv) ) {
        SHR_BITCLR(_bcm_th3_xconnect_db[unit]->valid, ovlan);
    }

    /* Release the Mutex */
    BCM_TH3_VLAN_XCONNECT_MUTEX_REL(unit);
#else
    rv = BCM_E_UNAVAIL;
#endif

    return rv;
}

/*
 * Function:
 *      _bcm_th3_vlan_xconnect_db_entry_delete_all
 *
 * Purpose:
 *      Delete all cross connect entries from HW and SW database
 *
 * Parameters:
 *      unit  - (IN) BCM unit.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      Cross Connect Mutex is acquired in the very beginning as updating
 *      the HW table and SW cache has to be atomic with respect to VLAN module
 */
int _bcm_th3_vlan_xconnect_db_entry_delete_all(int unit) {

    int rv = BCM_E_NONE;

#ifdef BCM_TRX_SUPPORT
    /* Acquire the Mutex */
    BCM_TH3_VLAN_XCONNECT_MUTEX_ACQ(unit);

    /* Update L2 HW Table at first */
    rv = bcm_tr_l2_cross_connect_delete_all(unit);

    /* Invalidate all entries in SW cache, only when HW is updated accord. */
    if( BCM_SUCCESS(rv) ) {
        SHR_BITCLR_RANGE(_bcm_th3_xconnect_db[unit]->valid, 0, BCM_VLAN_COUNT);
    }

    /* Release the Mutex */
    BCM_TH3_VLAN_XCONNECT_MUTEX_REL(unit);
#else
    rv = BCM_E_UNAVAIL;
#endif

    return rv;
}

/*
 * Function:
 *      _bcm_th3_vlan_xconnect_traverse
 *
 * Purpose:
 *      Walks through the valid cross connect entries and
 *      calls the user supplied callback function for each entry.
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      cb         - (IN) Callback function.
 *      user_data  - (IN) User data to be passed to callback function.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int
_bcm_th3_vlan_xconnect_traverse(int unit,
                                 bcm_vlan_cross_connect_traverse_cb cb,
                                 void *user_data)
{
    int rv = BCM_E_NONE;
    uint32 destv;
    bcm_vlan_t ovlan;
    bcm_gport_t gport_1;
    bcm_gport_t gport_2;
    bcm_module_t local_modid;

    /* Get hold of chip local MODID */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &local_modid));

    /* Acquire the Mutex */
    BCM_TH3_VLAN_XCONNECT_MUTEX_ACQ(unit);

    /* Iterate through all valid cross connect entries */
    SHR_BIT_ITER(_bcm_th3_xconnect_db[unit]->valid, BCM_VLAN_COUNT, ovlan) {

        /* Deduce the first gport */
        destv = _bcm_th3_xconnect_db[unit]->
                    destination[BCM_TH3_VLAN_XC_DB_DESTTYPE_OFFSET(ovlan)];
        if (SHR_BITGET(_bcm_th3_xconnect_db[unit]->dest_type,
                       BCM_TH3_VLAN_XC_DB_DESTTYPE_OFFSET(ovlan))) {
            BCM_GPORT_TRUNK_SET(gport_1, destv);
        } else {
            BCM_GPORT_MODPORT_SET(gport_1, local_modid, destv);
        }

        /* Deduce the second gport */
        destv = _bcm_th3_xconnect_db[unit]->
                    destination[BCM_TH3_VLAN_XC_DB_DESTTYPE1_OFFSET(ovlan)];
        if (SHR_BITGET(_bcm_th3_xconnect_db[unit]->
                dest_type, BCM_TH3_VLAN_XC_DB_DESTTYPE1_OFFSET(ovlan))) {
            BCM_GPORT_TRUNK_SET(gport_2, destv);
        } else {
            BCM_GPORT_MODPORT_SET(gport_2, local_modid, destv);
        }

        /* Call application call-back */
        rv = cb(unit, ovlan, BCM_VLAN_INVALID, gport_1, gport_2, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            break;
        }
#endif
    }

    /* Release the Mutex */
    BCM_TH3_VLAN_XCONNECT_MUTEX_REL(unit);

    return rv;
}

/*
 * Function:
 *      bcm_th3_vlan_port_add
 *
 * Purpose:
 *      Invokes low level functions to program the port membership bitmaps
 *      in tables VLAN_2 and EGR_VLAN.
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      vid        - (IN) VLAN Identifier.
 *      pbmp       - (IN) Egress Tagged Port Bitmap to be added to the VLAN.
 *      upbmp      - (IN) Egress Untagged Port Bitmap to be added to the VLAN.
 *      ing_pbmp   - (IN) Ingress Port Bitmap to be added to the VLAN.
 *
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_PARAM
 *      BCM_E_NOT_FOUND
 *      BCM_E_FAIL
 *      BCM_E_FULL
 *
 * Notes:
 */
int
bcm_th3_vlan_port_add(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp,
                       pbmp_t ing_pbmp)
{
    if (BCM_VLAN_VALID(vid)) {
        BCM_IF_ERROR_RETURN(_bcm_xgs3_vlan_table_port_add(
                unit, vid, pbmp, ubmp, ing_pbmp, VLAN_2_TABm));

        BCM_IF_ERROR_RETURN(_bcm_xgs3_vlan_table_port_add(
                unit, vid, pbmp, ubmp, ing_pbmp, EGR_VLANm));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_vlan_port_remove
 *
 * Purpose:
 *      Invokes low level functions to remove the ports in the given bitmaps
 *      from tables VLAN_2 and EGR_VLAN.
 *
 * Parameters:
 *      unit     - (IN) BCM unit.
 *      vid      - (IN) VLAN Identifier.
 *      pbmp     - (IN) Egress Tagged Port Bitmap to be removed from the VLAN.
 *      upbmp    - (IN) Egress Untagged Port Bitmap to be removed from the VLAN.
 *      ing_pbmp - (IN) Ingress Port Bitmap to be removed from the VLAN.
 *
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_PARAM
 *      BCM_E_NOT_FOUND
 *      BCM_E_FAIL
 *      BCM_E_FULL
 *
 * Notes:
 */
int
bcm_th3_vlan_port_remove(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp,
                          pbmp_t ing_pbmp)
{
    if (BCM_VLAN_VALID(vid)) {
        BCM_IF_ERROR_RETURN(_bcm_xgs3_vlan_table_port_remove(
                    unit, vid, pbmp, ubmp, ing_pbmp, VLAN_2_TABm));
        BCM_IF_ERROR_RETURN(_bcm_xgs3_vlan_table_port_remove(
                    unit, vid, pbmp, ubmp, ing_pbmp, EGR_VLANm));
    }
    return BCM_E_NONE;
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *      _bcm_th3_vlan_sw_tag_action_profile_dump
 *
 * Purpose:
 *      Displays VLAN Tag action information maintained by software.
 *
 * Parameters:
 *      unit       - (IN) BCM unit.
 *
 * Returns:
 *
 * Notes:
 */
void
_bcm_th3_vlan_sw_tag_action_profile_dump(int unit)
{
    int         num_entries;
    int         ref_count;
    int         entries_per_set;
    int         index, idx;
    void        *entry;
    soc_mem_t   mem;
    soc_profile_mem_t *profile[2];

    profile[0] = ing_action_profile[unit];
    profile[1] = egr_action_profile[unit];

    for (idx = 0 ; idx < 2 ; idx++) {
      if (NULL != profile[idx]) {
        LOG_CLI((BSL_META_U(unit,
            "\nSW Information %sgress VLAN Action Profile - Unit %d\n")
            , (ing_action_profile[unit] == profile[idx])?"In":"E", unit));

        mem = profile[idx]->tables[0].mem;
        num_entries = soc_mem_index_count(unit, mem);
        LOG_CLI((BSL_META_U(unit, " Number of entries: %d\n\n"), num_entries));
        for (index = 0; index < num_entries; index ++) {
            if (SOC_FAILURE(
                    soc_profile_mem_ref_count_get(
                        unit, profile[idx], index, &ref_count))) {
                break;
            }

            /* Display entries with reference count > 0 */
            if (ref_count <= 0) {
                continue;
            }

            entries_per_set =
                profile[idx]->tables[0].entries[index].entries_per_set;
            entry = SOC_PROFILE_MEM_ENTRY(unit, profile[idx], void *, index);

            LOG_CLI((BSL_META_U(unit, " Index           = 0x%x\n"), index));
            LOG_CLI((BSL_META_U(unit, " Reference count = %d\n"), ref_count));
            LOG_CLI((BSL_META_U(unit, " Entries per set = %d\n")
                     , entries_per_set));
            soc_mem_entry_dump(unit, mem, entry, BSL_LSS_CLI);
            LOG_CLI((BSL_META_U(unit, "\n\n")));
        }
      }
    }

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */
#else /* BCM_TOMAHAWK3_SUPPORT */
typedef int _tomahawk3_vlan_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_TOMAHAWK3_SUPPORT */

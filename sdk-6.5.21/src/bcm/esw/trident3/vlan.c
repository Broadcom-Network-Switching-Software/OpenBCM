/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    vlan.c
 * Purpose: Handle trident3 specific vlan features.
 */

#include <soc/defs.h>
#include <soc/field.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#if defined(BCM_TRIDENT3_SUPPORT)
#include <sal/core/libc.h>
#include <soc/drv.h>
#include <soc/hash.h>
#include <soc/trident2.h>
#include <soc/trident3.h>
#include <soc/mem.h>
#include <bcm/port.h>
#include <bcm/error.h>

#include <bcm_int/esw/port.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/vpn.h>
#include <bcm_int/esw/trx.h>
#if defined(INCLUDE_L3)
#include <bcm_int/esw/virtual.h>
#endif

/*
 * Function:
 *      _bcm_td3_vlan_vfi_untag_profile_ptr_get
 * Purpose:
 *      Given a VLAN/VFI value, get the VLAN/VFI untag profile.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan_vfi   - (IN) VLAN/VFI in question
 *      prof_index - (OUT) The associated profile index if all goes well
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td3_vlan_vfi_untag_profile_ptr_get(int unit,
                                        bcm_vlan_t vlan_vfi,
                                        uint32 *prof_index)
{
    int                 rv = BCM_E_NONE;
    uint32              *entry = NULL, index;
    soc_mem_t           mem;

    if (prof_index == NULL) {
        return BCM_E_PARAM;
    }

    /* Depending on VLAN/VFI, set relevant memory and index */
    if (!_BCM_VPN_VFI_IS_SET(vlan_vfi)) {
        mem = EGR_VLANm;
        /* Index in case of a VLAN is same as what is passed in */
        index = vlan_vfi;
    } else {
        mem = EGR_VFIm;
        /* Index in case of a VFI is the encoded value */
        _BCM_VPN_GET(index, _BCM_VPN_TYPE_VFI, vlan_vfi);
    }

    /* Allocated the desired size */
    entry = sal_alloc(SOC_MEM_BYTES(unit, mem), "Get VLAN/VFI profile index");
    if (entry == NULL) {
        return BCM_E_MEMORY;
    }

    /* Read the memory and extract the field in question */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, entry);
    if (rv < 0) {
        sal_free(entry);
        return rv;
    }

    /* Assign the profile index */
    *prof_index = soc_mem_field32_get(unit, mem, entry, UNTAG_PROFILE_PTRf);

    sal_free(entry);

    return rv;
}

/*
 * Function:
 *      _bcm_td3_vlan_vfi_untag_profile_ptr_set
 * Purpose:
 *      Given a VLAN/VFI value, prof_index set
 *      the associated VLAN_VFI profile.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan_vfi   - (IN) VLAN/VFI value
 *      prof_index - (OUT) The associated profile index if all goes well
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td3_vlan_vfi_untag_profile_ptr_set(int unit,
                                        bcm_vlan_t vlan_vfi,
                                        int prof_index)
{
    int                 index = -1;
    soc_mem_t           mem;

    /* Depending on VLAN/VFI, write to relevant memories */
    if (!_BCM_VPN_VFI_IS_SET(vlan_vfi)) {
        mem = EGR_VLANm;
        index = vlan_vfi;
    } else {
        mem = EGR_VFIm;
        /* Index in case of a VFI is the encoded value */
        _BCM_VPN_GET(index, _BCM_VPN_TYPE_VFI, vlan_vfi);
    }

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!: mem: %s index: %d prof_index: %d\n"),
                        SOC_MEM_NAME(unit, mem), index, prof_index));

    SOC_IF_ERROR_RETURN(
        soc_mem_field32_modify(unit, mem, index, UNTAG_PROFILE_PTRf, prof_index));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_vlan_vfi_profile_entry_set
 * Purpose:
 *      Given a VLAN/VFI value and a direction (Igr/Egr), set the
 *      data associated with a profile entry and update the VLAN/VFI
 *      with a new profile index
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan_vfi - (IN) VLAN/VFI value
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress.
 *      entry_data - (IN) Entry to write
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td3_vlan_vfi_profile_entry_set(int unit, bcm_vlan_t vlan_vfi,
                                    void *entry_data)
{
    uint32 cur_prof_index = 0, new_prof_index = 0;
    int32 cur_idx_ref_count = 0, new_idx_ref_count = 0;

    /* Get the profile_idx associated with the VLAN/VFI */
    _bcm_td3_vlan_vfi_untag_profile_ptr_get(unit, vlan_vfi, &cur_prof_index);

    /* Gather the ref counts */
    BCM_IF_ERROR_RETURN
        (_bcm_vlan_vfi_untag_profile_ref_count(unit, cur_prof_index,
                                               &cur_idx_ref_count));

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"\n\n!:Enter!Before profile: vlan_vfi: %d Old index %d "
                         "old_ref_cnt %d \n"),
                         vlan_vfi, cur_prof_index, cur_idx_ref_count));

    /* Depending on the direction, update the  relevant profile index,
       delete the old one */

    BCM_IF_ERROR_RETURN(_bcm_vlan_vfi_untag_profile_entry_op(unit, &entry_data, 1, 1,
                            (uint32 *)&new_prof_index));

    /* Write the new index into the VALN/VFI tables */
    _bcm_td3_vlan_vfi_untag_profile_ptr_set(unit, vlan_vfi, new_prof_index);

    /*
     * If the old-profile-index entry is non-zero delete it, note that if the
     * same index is reallocated, the profile-mgmt would have incremented the
     * ref-count anyway.
     */
    if (cur_prof_index != 0) {
        _bcm_vlan_vfi_untag_profile_entry_op(unit, NULL, 1, 0,
                                             &cur_prof_index);
    }

    /* Gather the ref counts */
    BCM_IF_ERROR_RETURN
        (_bcm_vlan_vfi_untag_profile_ref_count(unit, cur_prof_index,
                                               &cur_idx_ref_count));
    BCM_IF_ERROR_RETURN
        (_bcm_vlan_vfi_untag_profile_ref_count(unit,new_prof_index,
                                               &new_idx_ref_count));

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!: After profile: old_index %d "
                         "old_ref_cnt %d new_index %d "
                         "new_ref_count: %d \n"),
                        cur_prof_index, cur_idx_ref_count,
                        new_prof_index, new_idx_ref_count));

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!: Old index %d New index %d "
                         "vlan-vfi %d Data0 0x%x Data1 0x%x "
                         "Data2 0x%x Data3 0x%x Data4 0x%x Data5 0x%x Exit!\n\n\n"),
                         cur_prof_index, new_prof_index, vlan_vfi,
                         ((egr_vlan_vfi_untag_entry_t *)entry_data)->entry_data[5],
                         ((egr_vlan_vfi_untag_entry_t *)entry_data)->entry_data[4],
                         ((egr_vlan_vfi_untag_entry_t *)entry_data)->entry_data[3],
                         ((egr_vlan_vfi_untag_entry_t *)entry_data)->entry_data[2],
                         ((egr_vlan_vfi_untag_entry_t *)entry_data)->entry_data[1],
                         ((egr_vlan_vfi_untag_entry_t *)entry_data)->entry_data[0]));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_vlan_vfi_profile_entry_get
 * Purpose:
 *      Given a VLAN/VFI value, return the utag profile entry
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan_vfi - (IN) VLAN/VFI value
 *      entry_data - (OUT) Entry returned
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td3_vlan_vfi_profile_entry_get(int unit, bcm_vlan_t vlan_vfi,
                                    void *entry_data)
{
    int rv = BCM_E_NONE;
    uint32 prof_index = 0;
    /* Get the profile_idx associated with the VLAN/VFI */
    BCM_IF_ERROR_RETURN(_bcm_td3_vlan_vfi_untag_profile_ptr_get(unit, vlan_vfi, &prof_index));

    rv = READ_EGR_VLAN_VFI_UNTAGm(unit, MEM_BLOCK_ANY, prof_index, entry_data);
    return rv;
}
/*
 * Function:
 *      bcm_td3_vlan_vfi_untag_init
 * Purpose:
 *      Program membership port bitmap in vlan vfi membership tables
 */
int
bcm_td3_vlan_vfi_untag_init(int unit, bcm_vlan_t vid, pbmp_t pbmp)
{
    egr_vlan_vfi_untag_entry_t egr_vlan_vfi;
    egr_vlan_entry_t egr_vtab;
    uint32 profile_ptr = 0;

    SOC_IF_ERROR_RETURN(READ_EGR_VLANm(unit, MEM_BLOCK_ANY, vid, &egr_vtab));
    profile_ptr = soc_EGR_VLANm_field32_get(unit, &egr_vtab,
                                            UNTAG_PROFILE_PTRf);

    SOC_IF_ERROR_RETURN
        (READ_EGR_VLAN_VFI_UNTAGm(unit, MEM_BLOCK_ANY, profile_ptr,
                                  &egr_vlan_vfi));

    soc_mem_pbmp_field_set(unit, EGR_VLAN_VFI_UNTAGm, &egr_vlan_vfi,
                           UT_PORT_BITMAPf, &pbmp);

    SOC_IF_ERROR_RETURN(
        _bcm_td3_vlan_vfi_profile_entry_set(unit, vid, &egr_vlan_vfi));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td3_vlan_vfi_untag_add
 * Purpose:
 *      Program membership port bitmap in vlan vfi untag tables
 */
int
bcm_td3_vlan_vfi_untag_add(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp)
{
    egr_vlan_vfi_untag_entry_t egr_vlan_vfi;
    pbmp_t      cur_pbmp;
    uint32      prof_index = 0;

    /* Get the profile_idx associated with the VLAN/VFI */
    _bcm_td3_vlan_vfi_untag_profile_ptr_get(unit, vid, &prof_index);

    /* Only allow untagged ports on the vlan. */
    SOC_PBMP_AND(ubmp, pbmp);

    /* Draco does not have room for CMIC/IPIC in ubmp */
    SOC_PBMP_REMOVE(ubmp, PBMP_CMIC(unit));
    SOC_PBMP_REMOVE(ubmp, PBMP_ST_ALL(unit));

    BCM_IF_ERROR_RETURN
        (READ_EGR_VLAN_VFI_UNTAGm(unit, MEM_BLOCK_ANY, prof_index, &egr_vlan_vfi));
    soc_mem_pbmp_field_get(unit, EGR_VLAN_VFI_UNTAGm, &egr_vlan_vfi,
            UT_PORT_BITMAPf, &cur_pbmp);
    BCM_PBMP_REMOVE(cur_pbmp, pbmp);
    BCM_PBMP_OR(cur_pbmp, ubmp);
    soc_mem_pbmp_field_set(unit, EGR_VLAN_VFI_UNTAGm, &egr_vlan_vfi,
            UT_PORT_BITMAPf, &cur_pbmp);

    _bcm_td3_vlan_vfi_profile_entry_set(unit, vid, &egr_vlan_vfi);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td3_vlan_vfi_untag_delete
 * Purpose:
 *      Program membership port bitmap in vlan vfi untag tables
 */
int
bcm_td3_vlan_vfi_untag_delete(int unit, bcm_vlan_t vid, pbmp_t pbmp)
{
    egr_vlan_vfi_untag_entry_t egr_vlan_vfi;
    pbmp_t      cur_pbmp;
    uint32      prof_index = 0;

    /* Get the profile_idx associated with the VLAN/VFI */
    _bcm_td3_vlan_vfi_untag_profile_ptr_get(unit, vid, &prof_index);

    BCM_IF_ERROR_RETURN
        (READ_EGR_VLAN_VFI_UNTAGm(unit, MEM_BLOCK_ANY, prof_index, &egr_vlan_vfi));
    soc_mem_pbmp_field_get(unit, EGR_VLAN_VFI_UNTAGm, &egr_vlan_vfi,
            UT_PORT_BITMAPf, &cur_pbmp);
    BCM_PBMP_REMOVE(cur_pbmp, pbmp);
    soc_mem_pbmp_field_set(unit, EGR_VLAN_VFI_UNTAGm, &egr_vlan_vfi,
            UT_PORT_BITMAPf, &cur_pbmp);

    _bcm_td3_vlan_vfi_profile_entry_set(unit, vid, &egr_vlan_vfi);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td3_egr_vlan_vfi_untag_destroy
 * Purpose:
 *      Delete the VLAN Vfi membership entry.
 * Parameters:
 *      unit  - (IN) SOC unit number.
 *      vlan  - (IN) VLAN ID.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td3_vlan_vfi_untag_destroy(int unit, bcm_vlan_t vlan)
{
    uint32 profile_ptr = 0;
    egr_vlan_vfi_untag_entry_t ing_vlan_vfi;
    int cur_ref_count = 0;

    sal_memset(&ing_vlan_vfi, 0, sizeof(egr_vlan_vfi_untag_entry_t));

    /* Get the MEMBERSHIP_PROFILE_PTR */
    _bcm_td3_vlan_vfi_untag_profile_ptr_get(unit, vlan, &profile_ptr);
    _bcm_vlan_vfi_untag_profile_ref_count(unit, profile_ptr, &cur_ref_count);

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!: Deleting profile %d for vlan_vfi: %d cur_ref_cnt: %d \n"),
                    profile_ptr, vlan, cur_ref_count));

    /*  Delete the Reference Count, if the profile is non-zero */
    if (profile_ptr != 0) {
        BCM_IF_ERROR_RETURN(_bcm_vlan_vfi_untag_profile_entry_op(unit, NULL, 1,
                                0, &profile_ptr));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_td3_vlan_table_utport_get
 * Purpose:
 *      Read the UT_PORT_BITMAP from a vlan entry.
 */
int
bcm_td3_vlan_table_ut_port_get(int unit, bcm_vlan_t vid, pbmp_t *ut_pbmp)
{
    egr_vlan_vfi_untag_entry_t egr_vlan_vfi;
    uint32      prof_index = 0;

    /* Upper layer already checks that vid is valid
     * Get the profile_idx associated with the VLAN/VFI */
    _bcm_td3_vlan_vfi_untag_profile_ptr_get(unit, vid, &prof_index);

    SOC_IF_ERROR_RETURN
        (READ_EGR_VLAN_VFI_UNTAGm(unit, MEM_BLOCK_ANY, prof_index,
            &egr_vlan_vfi));
    soc_mem_pbmp_field_get(unit, EGR_VLAN_VFI_UNTAGm, &egr_vlan_vfi,
            UT_PORT_BITMAPf, ut_pbmp);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_vlan_mac_action_add
 * Description   :
 *      Add association from MAC address to VLAN.
 *      If the entry already exists, update the action.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      mac       (IN) MAC address
 *      action    (IN) Action for outer and inner tag
 * Note:
 *   Program VLAN_XLATE_1_DOUBLEm.
 */
int _bcm_td3_vlan_mac_action_add(int unit, bcm_mac_t mac,
                                 bcm_vlan_action_set_t *action)
{
    int rv;
    uint32 profile_idx;
    vlan_xlate_2_double_entry_t vx1ent;
    soc_mem_t mem = VLAN_XLATE_2_DOUBLEm;

    if (soc_property_get(unit, spn_VLAN_MAC_ADD_XLATE_TABLE_SELECT, FALSE)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit, action));
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        if (action->priority == -1) {
            return BCM_E_PARAM;  /* no default priority action to take*/
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_add(unit, action, &profile_idx));

    sal_memset(&vx1ent, 0, sizeof(vx1ent));
    soc_mem_mac_addr_set(unit, mem, &vx1ent, MAC__MAC_ADDRf, mac);
    soc_mem_field32_set(unit, mem, &vx1ent, KEY_TYPEf,
                        TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC);
    soc_mem_field32_set(unit, mem, &vx1ent, DATA_TYPEf,
                        TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC);
    soc_mem_field32_set(unit, mem, &vx1ent, MAC__OVIDf, action->new_outer_vlan);
    soc_mem_field32_set(unit, mem, &vx1ent, MAC__IVIDf, action->new_inner_vlan);

    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        soc_mem_field32_set(unit, mem, &vx1ent, MAC__OPRIf, action->priority);
        soc_mem_field32_set(unit, mem, &vx1ent, MAC__OCFIf,
                            action->new_outer_cfi);
        soc_mem_field32_set(unit, mem, &vx1ent, MAC__IPRIf,
                            action->new_inner_pkt_prio);
        soc_mem_field32_set(unit, mem, &vx1ent, MAC__ICFIf,
                            action->new_inner_cfi);
    } else {
        if (action->priority >= BCM_PRIO_MIN &&
            action->priority <= BCM_PRIO_MAX) {
            soc_mem_field32_set(unit, mem, &vx1ent, MAC__PRIf, action->priority);
        }
    }
    soc_mem_field32_set(unit, mem, &vx1ent, MAC__TAG_ACTION_PROFILE_PTRf,
                        profile_idx);

    if (SOC_MEM_FIELD_VALID(unit, mem, MAC__VLAN_ACTION_VALIDf)) {
        soc_mem_field32_set(unit, mem, &vx1ent, MAC__VLAN_ACTION_VALIDf, 1);
    }
    soc_mem_field32_set(unit, mem, &vx1ent, BASE_VALID_0f, 3);
    soc_mem_field32_set(unit, mem, &vx1ent, BASE_VALID_1f, 7);

    rv = soc_mem_insert_return_old(unit, mem, MEM_BLOCK_ALL, &vx1ent, &vx1ent);

    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan action profile entry */
        profile_idx = soc_mem_field32_get(unit, mem, &vx1ent,
                                          MAC__TAG_ACTION_PROFILE_PTRf);
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td3_vlan_mac_action_get
 * Description   :
 *      Get association from MAC address to VLAN tag actions.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      mac       (IN) MAC address
 *      action    (OUT) Action for outer and inner tag
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_vlan_mac_action_get(int unit, bcm_mac_t mac,
                             bcm_vlan_action_set_t *action)
{
    vlan_xlate_2_double_entry_t vx1ent;     /* Lookup key hw buffer.      */
    vlan_xlate_2_double_entry_t res_vx1ent; /* Lookup result buffer.      */
    uint32 profile_idx;                     /* Vlan action profile index. */
    int rv;                                 /* Operation return status.   */
    int idx = 0;                            /* Lookup result entry index. */
    soc_mem_t mem = VLAN_XLATE_2_DOUBLEm;

    /* Input parameters check. */
    if (NULL == action) {
        return (BCM_E_PARAM);
    }

    if (soc_property_get(unit, spn_VLAN_MAC_ADD_XLATE_TABLE_SELECT, FALSE)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    /* Reset lookup key and result destination buffer. */
    sal_memset(&vx1ent, 0, sizeof(vx1ent));
    sal_memset(&res_vx1ent, 0, sizeof(res_vx1ent));

    /* Initialize lookup key. */
    soc_mem_mac_addr_set(unit, mem, &vx1ent, MAC__MAC_ADDRf, mac);
    soc_mem_field32_set(unit, mem, &vx1ent, KEY_TYPEf,
                        TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC);
    soc_mem_field32_set(unit, mem, &vx1ent, DATA_TYPEf,
                        TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC);
    /* Perform VLAN_XLATE table search by mac address. */
    soc_mem_lock(unit, mem);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &idx,
                        &vx1ent, &res_vx1ent, 0);
    soc_mem_unlock(unit, mem);
    BCM_IF_ERROR_RETURN(rv);

    action->new_outer_vlan =
        soc_mem_field32_get(unit, mem, &res_vx1ent, MAC__OVIDf);
    action->new_inner_vlan =
        soc_mem_field32_get(unit, mem, &res_vx1ent, MAC__IVIDf);
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        action->priority =
            soc_mem_field32_get(unit, mem, &res_vx1ent, MAC__OPRIf);
        action->new_outer_cfi =
            soc_mem_field32_get(unit, mem, &res_vx1ent, MAC__OCFIf);
        action->new_inner_pkt_prio =
            soc_mem_field32_get(unit, mem, &res_vx1ent, MAC__IPRIf);
        action->new_inner_cfi =
            soc_mem_field32_get(unit, mem, &res_vx1ent, MAC__ICFIf);
    } else {
        action->priority = soc_mem_field32_get(unit, mem, &res_vx1ent, MAC__PRIf);
    }

    /* Read action profile data. */
    profile_idx = soc_mem_field32_get(unit, mem, &res_vx1ent,
                                      MAC__TAG_ACTION_PROFILE_PTRf);
    _bcm_trx_vlan_action_profile_entry_get(unit, action, profile_idx);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_td3_vlan_mac_delete
 * Purpose:
 *      Delete a vlan mac lookup entry.
 * Parameters:
 *      unit      (IN) BCM unit number
 *      mac       (IN) MAC address
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_vlan_mac_delete(int unit, bcm_mac_t mac)
{
    int rv;
    vlan_xlate_2_double_entry_t vx1ent;
    uint32 profile_idx;
    soc_mem_t mem = VLAN_XLATE_2_DOUBLEm;

    if (soc_property_get(unit, spn_VLAN_MAC_ADD_XLATE_TABLE_SELECT, FALSE)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    sal_memset(&vx1ent, 0, sizeof(vx1ent));
    soc_mem_mac_addr_set(unit, mem, &vx1ent, MAC__MAC_ADDRf, mac);
    soc_mem_field32_set(unit, mem, &vx1ent, KEY_TYPEf,
                        TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC);
    soc_mem_field32_set(unit, mem, &vx1ent, DATA_TYPEf,
                        TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC);
    soc_mem_lock(unit, mem);
    rv = soc_mem_delete_return_old(unit, mem, MEM_BLOCK_ALL, &vx1ent, &vx1ent);
    soc_mem_unlock(unit, mem);
    if (rv == SOC_E_NOT_FOUND) {
        rv = SOC_E_NONE;
    } else if ((rv == SOC_E_NONE) &&
               (soc_mem_field32_get(unit, mem, &vx1ent, BASE_VALID_0f) == 3) &&
               (soc_mem_field32_get(unit, mem, &vx1ent, BASE_VALID_1f) == 7)) {
        profile_idx = soc_mem_field32_get(unit, mem, &vx1ent,
                                          MAC__TAG_ACTION_PROFILE_PTRf);
        /* Delete the old vlan action profile entry */
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td3_vlan_mac_delete_all
 * Purpose:
 *      Delete all vlan mac lookup entry.
 * Parameters:
 *      unit      (IN) BCM unit number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_vlan_mac_delete_all(int unit)
{
    int i, imin, imax, nent, rv;
    uint32 old_profile_idx;
    vlan_xlate_2_double_entry_t *vxtab, *vmnull, *vxtabp;
    soc_mem_t mem = VLAN_XLATE_2_DOUBLEm;

    if (soc_property_get(unit, spn_VLAN_MAC_ADD_XLATE_TABLE_SELECT, FALSE)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    imin = soc_mem_index_min(unit, mem);
    imax = soc_mem_index_max(unit, mem);
    nent = soc_mem_index_count(unit, mem);

    vxtab = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, mem), "vlan_xlate");

    if (vxtab == NULL) {
        return BCM_E_MEMORY;
    }

    vmnull = soc_mem_entry_null(unit, mem);

    soc_mem_lock(unit, mem);
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                            imin, imax, vxtab);
    if (rv < 0) {
        soc_mem_unlock(unit, mem);
        soc_cm_sfree(unit, vxtab);
        return rv;
    }

    for (i = 0; i < nent; i++) {
        vxtabp = soc_mem_table_idx_to_pointer(unit, mem,
                        vlan_xlate_2_double_entry_t *, vxtab, i);

        if (!((soc_mem_field32_get(unit, mem, vxtabp, BASE_VALID_0f) == 3) &&
              (soc_mem_field32_get(unit, mem, vxtabp, BASE_VALID_1f) == 7)) ||
            (soc_mem_field32_get(unit, mem, vxtabp, KEY_TYPEf) !=
             TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC)) {
            continue;
        }
        old_profile_idx = soc_mem_field32_get(unit, mem, vxtabp,
                                              MAC__TAG_ACTION_PROFILE_PTRf);

        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, i, vmnull);

        if (rv >= 0) {
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, old_profile_idx);
        }
    }

    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, vxtab);
    return rv;
}

/*
 * Function:
 *      _bcm_td3_vlan_mac_action_traverse
 * Description   :
 *      Traverse over vlan mac actions, which are used for VLAN
 *      tag/s assignment to untagged packets.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_vlan_mac_action_traverse(int unit,
                                  bcm_vlan_mac_action_traverse_cb cb,
                                  void *user_data)
{
    int idx, imin, imax, nent, rv;
    uint32 profile_idx;
    bcm_mac_t  mac;
    bcm_vlan_action_set_t action;
    vlan_xlate_2_double_entry_t * vmtab, *vmtabp;
    soc_mem_t mem = VLAN_XLATE_2_DOUBLEm;

    if (soc_property_get(unit, spn_VLAN_MAC_ADD_XLATE_TABLE_SELECT, FALSE)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    /* Input parameters check. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    imin = soc_mem_index_min(unit, mem);
    imax = soc_mem_index_max(unit, mem);
    nent = soc_mem_index_count(unit, mem);
    vmtab = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, mem), "vlan_mac");

    if (vmtab == NULL) {
        return BCM_E_MEMORY;
    }

    soc_mem_lock(unit, mem);
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, imin, imax, vmtab);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        soc_cm_sfree(unit, vmtab);
        return rv;
    }

    for (idx = 0; idx < nent; idx++) {
        sal_memset(mac, 0, sizeof(bcm_mac_t));
        sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
        vmtabp = soc_mem_table_idx_to_pointer(unit, mem,
                                              vlan_xlate_2_double_entry_t *,
                                              vmtab, idx);

        if (!((soc_mem_field32_get(unit, mem, vmtabp, BASE_VALID_0f) == 3) &&
              (soc_mem_field32_get(unit, mem, vmtabp, BASE_VALID_1f) == 7)) ||
            (soc_mem_field32_get(unit, mem, vmtabp, KEY_TYPEf) !=
             TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC)) {
            continue;
        }

        /* Get entry mac address. */
        soc_mem_mac_addr_get(unit, mem, vmtabp, MAC__MAC_ADDRf, mac);

        action.new_outer_vlan =
            soc_mem_field32_get(unit, mem, vmtabp, MAC__OVIDf);
        action.new_inner_vlan =
            soc_mem_field32_get(unit, mem, vmtabp, MAC__IVIDf);
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            action.priority = soc_mem_field32_get(unit, mem, vmtabp, MAC__OPRIf);
            action.new_outer_cfi =
                soc_mem_field32_get(unit, mem, vmtabp, MAC__OCFIf);
            action.new_inner_pkt_prio =
                soc_mem_field32_get(unit, mem, vmtabp, MAC__IPRIf);
            action.new_inner_cfi =
                soc_mem_field32_get(unit, mem, vmtabp, MAC__ICFIf);
        } else {
            action.priority =
                soc_mem_field32_get(unit, mem, vmtabp, MAC__PRIf);
        }

        /* Read action profile data. */
        profile_idx =
            soc_mem_field32_get(unit, mem, vmtabp, MAC__TAG_ACTION_PROFILE_PTRf);
        _bcm_trx_vlan_action_profile_entry_get(unit, &action, profile_idx);

        /* Call traverse callback with the data. */
        rv = cb(unit, mac, &action, user_data);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, mem);
            soc_cm_sfree(unit, vmtab);
            return rv;
        }
    }

    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, vmtab);
    return rv;
}

/*
 * Function:
 *      bcmi_td3_def_vfi_profile_add
 *
 * Purpose:
 *      Programs VFI_PROFILE table based on vpn type
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     int  - (IN) is_eline
 *     profile_index - (OUT) Profile index allocated by soc_mem_profile_add
 *
 * Returns:
 *      BCM_E_XXXs
 */
int
_bcm_td3_def_vfi_profile_add(int unit, int is_eline, uint32 *profile_index){
    vfi_profile_entry_t vfi_profile_entry;
    void *entries[1];
    int value = 0;

    sal_memset(&vfi_profile_entry, 0, sizeof(vfi_profile_entry_t));
    /* from bcm56870_a0_cancun_cmh.yml */
    if(is_eline) {
        value = 8;
    }
    else {
        value = 4;
    }

    soc_mem_field32_set(unit, VFI_PROFILEm, &vfi_profile_entry,
        PKT_FLOW_SELECT_CTRL_ID_2f, value);
    soc_mem_field32_set(unit, VFI_PROFILEm, &vfi_profile_entry,
        EN_IFILTERf, 1);
    entries[0] = &vfi_profile_entry;
    BCM_IF_ERROR_RETURN(_bcm_vfi_profile_entry_add(unit, entries, 1,
        profile_index));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_sd_tag_action_profile_get
 *
 * Purpose:
 *      get vlan profile index based on sd_tag action
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     int  - (IN) sd_tag_action_present
 *     int  - (IN) sd_tag_action_not_present
 *     index - (IN, OUT) Profile index
 *
 * Returns:
 *      BCM_E_XXXs
 */
int
_bcm_td3_sd_tag_action_profile_get(
        int unit,
        int sd_tag_action_present,
        int sd_tag_action_not_present,
        uint32 *index)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t action;
    uint32 new_index = 0;


    bcm_vlan_action_set_t_init(&action);

    /* sd_tag_action_not_present
     * 0 = NOOP 1 = ADD_VID_TPID 2 = RSVD_2 3 = RSVD_3
     */
    if (sd_tag_action_not_present == 1) {
        action.outer_tpid_action = bcmVlanTpidActionModify;
        action.it_outer = bcmVlanActionAdd;
        action.it_outer_pkt_prio = bcmVlanActionAdd;
        action.it_outer_cfi = bcmVlanActionAdd;
        action.ut_outer  = bcmVlanActionAdd;
        action.ut_outer_pkt_prio  = bcmVlanActionAdd;
        action.ut_outer_cfi  = bcmVlanActionAdd;
    } else {
        action.outer_tpid_action = bcmVlanTpidActionNone;
        action.it_outer = bcmVlanActionNone;
        action.ut_outer  = bcmVlanActionNone;
    }

    /* sd_tag_action_present
     * 0 = NOOP
     * 1 = REPLACE_VID_TPID
     * 2 = REPLACE_VID_ONLY
     * 3 = DELETE
     * 4 = REPLACE_VID_PRI_TPID
     * 5 = REPLACE_VID_PRI_ONLY
     * 6 = REPLACE_PRI_ONLY
     * 7 = REPLACE_TPID_ONLY
     */
    if (sd_tag_action_present == 0){
        action.outer_tpid_action = bcmVlanTpidActionNone;
        action.ot_outer = bcmVlanActionNone;
        action.ot_outer_pkt_prio = bcmVlanActionNone;
        action.ot_outer_cfi = bcmVlanActionNone;
        action.dt_outer = bcmVlanActionNone;
        action.dt_outer_pkt_prio = bcmVlanActionNone;
        action.dt_outer_cfi = bcmVlanActionNone;
    } else if (sd_tag_action_present == 1) {
        action.outer_tpid_action = bcmVlanTpidActionModify;
        action.ot_outer = bcmVlanActionReplace;
        action.ot_outer_prio = bcmVlanActionReplace;
        action.ot_outer_pkt_prio = bcmVlanActionNone;
        action.ot_outer_cfi = bcmVlanActionNone;
        action.dt_outer = bcmVlanActionReplace;
        action.dt_outer_prio = bcmVlanActionReplace;
        action.dt_outer_pkt_prio = bcmVlanActionNone;
        action.dt_outer_cfi = bcmVlanActionNone;
    } else if (sd_tag_action_present == 2) {
        action.outer_tpid_action = bcmVlanTpidActionNone;
        action.ot_outer = bcmVlanActionReplace;
        action.ot_outer_prio = bcmVlanActionReplace;
        action.ot_outer_pkt_prio = bcmVlanActionNone;
        action.ot_outer_cfi = bcmVlanActionNone;
        action.dt_outer = bcmVlanActionReplace;
        action.dt_outer_prio = bcmVlanActionReplace;
        action.dt_outer_pkt_prio = bcmVlanActionNone;
        action.dt_outer_cfi = bcmVlanActionNone;
    } else if (sd_tag_action_present == 3) {
        action.outer_tpid_action = bcmVlanTpidActionModify;
        action.ot_outer = bcmVlanActionDelete;
        action.ot_outer_pkt_prio = bcmVlanActionDelete;
        action.ot_outer_cfi = bcmVlanActionDelete;
        action.dt_outer = bcmVlanActionDelete;
        action.dt_outer_pkt_prio = bcmVlanActionDelete;
        action.dt_outer_cfi = bcmVlanActionDelete;
    } else if (sd_tag_action_present == 4) {
        action.outer_tpid_action = bcmVlanTpidActionModify;
        action.ot_outer = bcmVlanActionReplace;
        action.ot_outer_prio = bcmVlanActionReplace;
        action.ot_outer_pkt_prio = bcmVlanActionReplace;
        action.ot_outer_cfi = bcmVlanActionReplace;
        action.dt_outer = bcmVlanActionReplace;
        action.dt_outer_prio = bcmVlanActionReplace;
        action.dt_outer_pkt_prio = bcmVlanActionReplace;
        action.dt_outer_cfi = bcmVlanActionReplace;
    } else if (sd_tag_action_present == 5) {
        action.outer_tpid_action = bcmVlanTpidActionNone;
        action.ot_outer = bcmVlanActionReplace;
        action.ot_outer_prio = bcmVlanActionReplace;
        action.ot_outer_pkt_prio = bcmVlanActionReplace;
        action.ot_outer_cfi = bcmVlanActionReplace;
        action.dt_outer = bcmVlanActionReplace;
        action.dt_outer_prio = bcmVlanActionReplace;
        action.dt_outer_pkt_prio = bcmVlanActionReplace;
        action.dt_outer_cfi = bcmVlanActionReplace;
    } else if (sd_tag_action_present == 6) {
        action.outer_tpid_action = bcmVlanTpidActionNone;
        action.ot_outer = bcmVlanActionNone;
        action.ot_outer_pkt_prio = bcmVlanActionReplace;
        action.ot_outer_cfi = bcmVlanActionReplace;
        action.dt_outer = bcmVlanActionNone;
        action.dt_outer_pkt_prio = bcmVlanActionReplace;
        action.dt_outer_cfi = bcmVlanActionReplace;
    } else if (sd_tag_action_present == 7) {
        action.outer_tpid_action = bcmVlanTpidActionModify;
        action.ot_outer = bcmVlanActionNone;
        action.ot_outer_pkt_prio = bcmVlanActionNone;
        action.ot_outer_cfi = bcmVlanActionNone;
        action.dt_outer = bcmVlanActionNone;
        action.dt_outer_pkt_prio = bcmVlanActionNone;
        action.dt_outer_cfi = bcmVlanActionNone;
    } else  {
        /* default as 0 */
        action.outer_tpid_action = bcmVlanTpidActionNone;
        action.ot_outer = bcmVlanActionNone;
        action.ot_outer_pkt_prio = bcmVlanActionNone;
        action.dt_outer = bcmVlanActionNone;
        action.dt_outer_pkt_prio = bcmVlanActionNone;
    }

    rv = _bcm_trx_egr_vlan_action_profile_entry_add(unit, &action, &new_index);
    if (rv != BCM_E_NONE) {
        return rv;
    }
    if (new_index != *index) {
        if (*index != 0) {
            rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, *index);
        }
        *index = new_index;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td3_sd_tag_action_get
 *
 * Purpose:
 *      get sd_tag action based on vlan profile index
 *
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     index - (IN) Profile index
 *     int  - (OUT) sd_tag_action_present
 *     int  - (OUT) sd_tag_action_not_present
 *
 * Returns:
 *      BCM_E_XXXs
 */
int
_bcm_td3_sd_tag_action_get(
        int unit,
        uint32 index,
        int *sd_tag_action_present,
        int *sd_tag_action_not_present)
{
    bcm_vlan_action_set_t action;

    bcm_vlan_action_set_t_init(&action);

    _bcm_trx_egr_vlan_action_profile_entry_get(unit, &action, index);

    if ((action.it_outer == bcmVlanActionAdd) &&
        (action.ut_outer == bcmVlanActionAdd)) {
        *sd_tag_action_not_present = 1;
    } else {
        *sd_tag_action_not_present = 0;
    }

    if ((action.outer_tpid_action == bcmVlanTpidActionNone) &&
        (action.ot_outer == bcmVlanActionNone) &&
        (action.ot_outer_pkt_prio == bcmVlanActionNone)) {

        *sd_tag_action_present = 0;
    } else if ((action.outer_tpid_action == bcmVlanTpidActionModify) &&
        (action.ot_outer == bcmVlanActionReplace) &&
        (action.ot_outer_pkt_prio == bcmVlanActionNone)) {

        *sd_tag_action_present = 1;
    } else if ((action.outer_tpid_action == bcmVlanTpidActionNone) &&
        (action.ot_outer == bcmVlanActionReplace) &&
        (action.ot_outer_pkt_prio == bcmVlanActionNone)) {

        *sd_tag_action_present = 2;
    } else if ((action.outer_tpid_action == bcmVlanTpidActionModify) &&
        (action.ot_outer == bcmVlanActionDelete)) {

        *sd_tag_action_present = 3;
    } else if ((action.outer_tpid_action == bcmVlanTpidActionModify) &&
        (action.ot_outer == bcmVlanActionReplace) &&
        (action.ot_outer_pkt_prio == bcmVlanActionReplace)) {

        *sd_tag_action_present = 4;
    } else if ((action.outer_tpid_action == bcmVlanTpidActionNone) &&
        (action.ot_outer == bcmVlanActionReplace) &&
        (action.ot_outer_pkt_prio == bcmVlanActionReplace)) {

        *sd_tag_action_present = 5;
    } else if ((action.outer_tpid_action == bcmVlanTpidActionNone) &&
        (action.ot_outer == bcmVlanActionNone) &&
        (action.ot_outer_pkt_prio == bcmVlanActionReplace)) {

        *sd_tag_action_present = 6;
    } else if ((action.outer_tpid_action == bcmVlanTpidActionModify) &&
        (action.ot_outer == bcmVlanActionNone) &&
        (action.ot_outer_pkt_prio == bcmVlanActionNone)) {

        *sd_tag_action_present = 7;
    } else  {
        /* default as 0 */
        *sd_tag_action_present = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_def_vlan_profile_get
 *
 * Purpose:
 *      get vlan profile index to add vlan for untagged packet
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     index - (IN, OUT) Profile index
 *
 * Returns:
 *      BCM_E_XXXs
 */
int
_bcm_td3_def_vlan_profile_get(
        int unit,
        uint32 *index)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t action;
    uint32 new_index = 0;


    bcm_vlan_action_set_t_init(&action);


    action.ut_outer  = bcmVlanActionAdd;

    rv = _bcm_trx_vlan_action_profile_entry_add(unit, &action, &new_index);
    if (rv != BCM_E_NONE) {
        return rv;
    }
    if (new_index != *index) {
        if ((*index != 0) && (*index != 1)) {
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, *index);
        }
        *index = new_index;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td3_vlan_shared_vlan_enable_get
 *
 * Purpose:
 *      get shared vlan enable status
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     enable - (OUT) enable status
 *
 * Returns:
 *      BCM_E_XXXs
 */
int
_bcm_td3_vlan_shared_vlan_enable_get(int unit, int *enable)
{
    bcm_vlan_info_t *vi = &vlan_info[unit];
    *enable = vi->shared_vlan_enable;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_vlan_shared_vlan_enable_set
 *
 * Purpose:
 *      set shared vlan enable status
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     enable - (IN) enable status
 *
 * Returns:
 *      BCM_E_XXXs
 */
int
_bcm_td3_vlan_shared_vlan_enable_set(int unit, int enable)
{
    bcm_vlan_info_t *vi = &vlan_info[unit];
    vi->shared_vlan_enable = enable ? 1 : 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_vlan_mac_port_key_fields_set
 * Description   :
 *      Set Key Fields in VLAN_XLATE entry hw buffer for MAC+PORT View.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      mac       (IN) MAC address
 *      port      (IN) gport number
 *      vment     (OUT) VLAN_XLATE entry hw buffer
 */
STATIC int
_bcm_td3_vlan_mac_port_key_fields_set(
    int unit,
    bcm_vlan_match_info_t *match_info,
    vlan_xlate_1_double_entry_t *vment)
{
    int rv = BCM_E_NONE;
    int gport_id;
    int source_type;
    int max_bits;
    bcm_trunk_t trunk_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    soc_mem_t mem = VLAN_XLATE_1_DOUBLEm;

    /* Set Key Type */
    soc_mem_field32_set(unit, mem, vment, KEY_TYPEf,
            TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT);

    /* Set entry MAC address */
    soc_mem_mac_addr_set(unit, mem, (uint32 *)(vment),
                   MAC_PORT__MAC_ADDRf, match_info->src_mac);

    switch(match_info->match) {
        case BCM_VLAN_MATCH_MAC_PORT:
            if (BCM_GPORT_IS_SET(match_info->port)) {
                BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit,
                            match_info->port, &mod_out, &port_out,
                            &trunk_id, &gport_id));
                if (BCM_GPORT_IS_TRUNK(match_info->port)) {
                    rv = _bcm_trunk_id_validate(unit, trunk_id);
                    if (BCM_FAILURE(rv)) {
                        return BCM_E_PORT;
                    }
                    source_type = 1;
                    soc_mem_field32_set(unit, mem, vment, MAC_PORT__Tf, 1);
                    soc_mem_field32_set(unit, mem, vment, MAC_PORT__TGIDf,
                            trunk_id);
                } else if BCM_GPORT_IS_MODPORT(match_info->port) {
                    if ((-1 == mod_out) || (-1 == port_out)) {
                        return BCM_E_PORT;
                    }
                    source_type = 1;
                    soc_mem_field32_set(unit, mem, vment,
                            MAC_PORT__PORT_NUMf, port_out);
                    soc_mem_field32_set(unit, mem, vment,
                            MAC_PORT__MODULE_IDf, mod_out);
                } else {
                    if (gport_id == -1) {
                        return BCM_E_PORT;
                    }
                    source_type = 2;
                    soc_mem_field32_set(unit, mem, vment,
                            MAC_PORT__SOURCE_FIELDf, gport_id);
                }
            } else {
                return BCM_E_PORT;
            }
            break;
        case BCM_VLAN_MATCH_MAC_PORT_CLASS:
            max_bits =soc_mem_field_length(unit,
                    mem, MAC_PORT__SOURCE_FIELDf);
            if (match_info->port_class < (1 << max_bits)) {
                source_type = 3;
                soc_mem_field32_set(unit, mem, vment, MAC_PORT__SOURCE_FIELDf,
                        match_info->port_class);
            } else {
                return BCM_E_PORT;
            }
            break;
        default:
            return BCM_E_PARAM;
    }

    /* Set entry Source Field */
    soc_mem_field32_set(unit, mem, vment, MAC_PORT__SOURCE_TYPEf, source_type);

    return rv;
}

/*
 * Function:
 *      int _bcm_td3_vlan_match_action_add
 * Description   :
 *      Add association from PORT, MAC address to VLAN.
 *      If the entry already exists, update the action.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      mac       (IN) MAC address
 *      action    (IN) Action for outer and inner tag
 * Note:
 *   Program VLAN_XLATE_1_DOUBLEm.
 */
int _bcm_td3_vlan_match_action_add(
    int unit,
    uint32 options,
    bcm_gport_t port,
    bcm_vlan_match_info_t *match_info,
    bcm_vlan_action_set_t *action_set)
{
    int rv;
    int idx = 0;                            /* Lookup result entry index. */
    int gport_id;
    int svp_valid;
    bcm_trunk_t trunk_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    uint32 profile_idx;
    vlan_xlate_1_double_entry_t vx1ent;
    vlan_xlate_1_double_entry_t res_vx1ent;
    soc_mem_t mem = VLAN_XLATE_1_DOUBLEm;

    if (action_set != NULL) {
        BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit, action_set));
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            if (action_set->priority == -1) {
                return BCM_E_PARAM;  /* no default priority action to take*/
            }
        }
    }

    sal_memset(&vx1ent, 0, sizeof(vx1ent));
    sal_memset(&res_vx1ent, 0, sizeof(res_vx1ent));

    /* Set Key Fields */
    BCM_IF_ERROR_RETURN
        (_bcm_td3_vlan_mac_port_key_fields_set(unit, match_info, &vx1ent));
    soc_mem_lock(unit, mem);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &idx,
            &vx1ent, &res_vx1ent, 0);

    soc_mem_unlock(unit, mem);

    /* If Entry exists, check svp configuration */
    if (rv == BCM_E_NONE) {
        svp_valid = FALSE;
        if (soc_mem_field32_get(unit, mem, &vx1ent, MAC_PORT__SVP_VALIDf)) {
            svp_valid = TRUE;
        }
        if (((port == BCM_GPORT_INVALID) && (svp_valid == TRUE)) ||
                ((port != BCM_GPORT_INVALID) && (svp_valid == FALSE))) {
            return BCM_E_EXISTS;
        }
        if (!options & BCM_VLAN_MATCH_ACTION_REPLACE) {
            return BCM_E_EXISTS;
        }
    }

    /* If Entry does not exists and want to replace */
    if ((rv == BCM_E_NOT_FOUND) && (options & BCM_VLAN_MATCH_ACTION_REPLACE)) {
        return rv;
    }

    soc_mem_field32_set(unit, mem, &vx1ent, DATA_TYPEf,
                        TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT);

    /* Set SVP Port */
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit,
                    port, &mod_out, &port_out, &trunk_id, &gport_id));
        if (SOC_MEM_FIELD_VALID(unit, mem, MAC_PORT__SVPf)) {
            soc_mem_field32_set(unit, mem, &vx1ent, MAC_PORT__SVP_VALIDf, 1);
            soc_mem_field32_set(unit, mem, &vx1ent, MAC_PORT__SVPf, gport_id);
        }
    }

    if (action_set != NULL) {
        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_action_profile_entry_add(unit, action_set, &profile_idx));

        /* Set Data Fields */
        soc_mem_field32_set(unit, mem, &vx1ent,
                MAC_PORT__OVIDf, action_set->new_outer_vlan);
        soc_mem_field32_set(unit, mem, &vx1ent,
                MAC_PORT__IVIDf, action_set->new_inner_vlan);
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            soc_mem_field32_set(unit, mem, &vx1ent,
                    MAC_PORT__OPRIf, action_set->priority);
            soc_mem_field32_set(unit, mem, &vx1ent,
                    MAC_PORT__OCFIf, action_set->new_outer_cfi);
            soc_mem_field32_set(unit, mem, &vx1ent,
                    MAC_PORT__IPRIf, action_set->new_inner_pkt_prio);
            soc_mem_field32_set(unit, mem, &vx1ent,
                    MAC_PORT__ICFIf, action_set->new_inner_cfi);
        } else {
            if (action_set->priority >= BCM_PRIO_MIN &&
                    action_set->priority <= BCM_PRIO_MAX) {
                soc_mem_field32_set(unit, mem, &vx1ent,
                        MAC_PORT__PRIf, action_set->priority);
            }
        }
        soc_mem_field32_set(unit, mem, &vx1ent,
                MAC_PORT__TAG_ACTION_PROFILE_PTRf, profile_idx);
        if (SOC_MEM_FIELD_VALID(unit, mem, MAC_PORT__VLAN_ACTION_VALIDf)) {
            soc_mem_field32_set(unit, mem, &vx1ent,
                    MAC_PORT__VLAN_ACTION_VALIDf, 1);
        }
    }
    soc_mem_field32_set(unit, mem, &vx1ent, BASE_VALID_0f, 3);
    soc_mem_field32_set(unit, mem, &vx1ent, BASE_VALID_1f, 7);

    /* Insert to VLAN_XLATE_1_DOUBLE */
    rv = soc_mem_insert_return_old(unit, mem,
            MEM_BLOCK_ALL, &vx1ent, &vx1ent);
    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan action profile entry */
        if (soc_mem_field32_get(unit, mem, &vx1ent,
                    MAC_PORT__VLAN_ACTION_VALIDf)) {
            profile_idx = soc_mem_field32_get(unit, mem,
                    &vx1ent, MAC_PORT__TAG_ACTION_PROFILE_PTRf);
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        }
    }

    return rv;

}

/*
 * Function:
 *      _bcm_td3_vlan_match_action_delete
 * Purpose:
 *      Remove action for a match criteria
 * Parameters:
 *      unit       (IN)  Device Number
 *      match_info (IN)  Match criteria info.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_vlan_match_action_delete(
    int unit,
    bcm_gport_t port,
    bcm_vlan_match_info_t *vlan_match_info)
{
    int rv, vp, svp_valid;
    int matched;
    vlan_xlate_1_double_entry_t vx1ent;
    vlan_xlate_1_double_entry_t res_vx1ent;
    soc_mem_t mem = VLAN_XLATE_1_DOUBLEm;
    uint32 profile_idx;                 /* Vlan action profile index. */
    int idx = 0;                        /* Lookup result entry index. */

    sal_memset(&vx1ent, 0, sizeof(vlan_xlate_1_double_entry_t));
    sal_memset(&res_vx1ent, 0, sizeof(vlan_xlate_1_double_entry_t));

    /* Set Key fields */
    BCM_IF_ERROR_RETURN
        (_bcm_td3_vlan_mac_port_key_fields_set(unit, vlan_match_info, &vx1ent));

    /* Perform VLAN_XLATE table search by mac address and port. */
    soc_mem_lock(unit, mem);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &idx,
            &vx1ent, &res_vx1ent, 0);
    soc_mem_unlock(unit, mem);
    BCM_IF_ERROR_RETURN(rv);

    /* Evaluate SVP */
    matched = FALSE;
    svp_valid = soc_mem_field32_get(unit, mem, &res_vx1ent,
            MAC_PORT__SVP_VALIDf);
    if ((port != BCM_GPORT_INVALID) && (svp_valid == TRUE)) {
        vp = soc_mem_field32_get(unit, mem, &res_vx1ent, MAC_PORT__SVPf);
        matched = _bcm_esw_port_vp_gport_check(port, vp);
    } else if ((port == BCM_GPORT_INVALID) && (svp_valid == FALSE)) {
        matched = TRUE;
    } else {
        /* (((port != BCM_GPORT_INVALID) && (svp_valid == FALSE)) ||
           ((port == BCM_GPORT_INVALID) && (svp_valid == TRUE))) */
        matched = FALSE;
    }

    if (matched == FALSE) {
        return BCM_E_NOT_FOUND;
    }

    soc_mem_lock(unit, mem);
    rv = soc_mem_delete_return_old(unit, mem, MEM_BLOCK_ALL,
            &vx1ent, &vx1ent);

    if (rv == BCM_E_NONE &&
       (soc_mem_field32_get(unit, mem,
                     &vx1ent, MAC_PORT__VLAN_ACTION_VALIDf))) {
        /* Read action profile data. */
            profile_idx = soc_mem_field32_get(unit, mem,
                    &vx1ent, MAC_PORT__TAG_ACTION_PROFILE_PTRf);
        _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    soc_mem_unlock(unit, mem);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_td3_vlan_match_action_delete_all
 * Purpose:
 *      Remove all actions for vlan match
 * Parameters:
 *      unit       (IN)  Device Number
 *      match      (IN)  vlan match
 *      port       (IN)  port number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_vlan_match_action_delete_all(int unit,
                bcm_vlan_match_t match, bcm_gport_t port)
{
    int vp, svp_valid;
    int rv, i, matched;
    int imin, imax, nent, vmbytes;
    uint32 profile_idx;
    vlan_xlate_1_double_entry_t *vmtab = NULL;
    vlan_xlate_1_double_entry_t *vment = NULL;
    int key_type_value, key_type;
    bcm_vlan_match_info_t match_info;
    soc_mem_t mem = VLAN_XLATE_1_DOUBLEm;

    imin = soc_mem_index_min(unit, mem);
    imax = soc_mem_index_max(unit, mem);
    nent = soc_mem_index_count(unit, mem);
    vmbytes = soc_mem_entry_words(unit, mem);
    vmbytes = WORDS2BYTES(vmbytes);

    vmtab = soc_cm_salloc(unit, nent * vmbytes, "vlan_mac");

    if (vmtab == NULL) {
        return BCM_E_MEMORY;
    }

    soc_mem_lock(unit, mem);
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
            imin, imax, vmtab);
    soc_mem_unlock(unit, mem);

    if (rv < 0) {
        soc_cm_sfree(unit, vmtab);
        return rv;
    }

    soc_mem_lock(unit, mem);
    for(i = 0; i < nent; i++) {

        vment = soc_mem_table_idx_to_pointer(unit, mem,
                vlan_xlate_1_double_entry_t *, vmtab, i);

        if (!((soc_mem_field32_get(unit, mem, vment, BASE_VALID_0f) == 3) &&
              (soc_mem_field32_get(unit, mem, vment, BASE_VALID_1f) == 7))) {
            continue;
        }

        /* Read KeyType */
        key_type_value = soc_mem_field32_get(unit, mem, vment, KEY_TYPEf);
        (void) _bcm_esw_vlan_xlate_key_type_get(unit, key_type_value,
                &key_type);
        switch(key_type) {
            case VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT:
                matched = TRUE;
                /* Read Key Fields */
                bcm_vlan_match_info_t_init(&match_info);
                (void) _bcm_td3_vlan_match_action_entry_get(unit,
                        vment, &match_info, NULL);
                if (match_info.match == BCM_VLAN_MATCH_INVALID) {
                    matched = FALSE;
                    break;
                }

                /* Evaluate SVP */
                matched = FALSE;
                svp_valid = soc_mem_field32_get(unit, mem, vment, MAC_PORT__SVP_VALIDf);
                if ((port != BCM_GPORT_INVALID) && (svp_valid == TRUE)) {
                    vp = soc_mem_field32_get(unit, mem, vment, MAC_PORT__SVPf);
                    matched = _bcm_esw_port_vp_gport_check(port, vp);
                } else if ((port == BCM_GPORT_INVALID) && (svp_valid == FALSE)) {
                    matched = TRUE;
                } else {
                    /* (((port != BCM_GPORT_INVALID) && (svp_valid == FALSE)) ||
                       ((port == BCM_GPORT_INVALID) && (svp_valid == TRUE))) */
                    matched = FALSE;
                }
                /* Evaluate match criteria */
                if ((match != BCM_VLAN_MATCH_INVALID) &&
                        (match != match_info.match)) {
                    matched = FALSE;
                }
                break;
            default:
                matched = FALSE;
                break;
        }

        if (matched == FALSE) {
            continue;
        }

        rv = soc_mem_delete_return_old(unit, mem, MEM_BLOCK_ALL,
                vment, vment);
        if (rv != BCM_E_NONE) {
            break;
        }
        if (soc_mem_field32_get(unit, mem,
                     vment, MAC_PORT__VLAN_ACTION_VALIDf)) {
            /* Read action profile data. */
            profile_idx = soc_mem_field32_get(unit, mem,
                    vment, MAC_PORT__TAG_ACTION_PROFILE_PTRf);
            _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        }
    }

    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, vmtab);

    return rv;
}
/*
 * Function:
 *      _bcm_td3_vlan_match_action_get
 * Purpose:
 *      Get action for given match criteria.
 * Parameters:
 *      unit         (IN)  Device Number
 *      svp_port     (IN) source vp port
 *      match_info - (IN) Match criteria info
 *      action_set - (OUT) Action Set.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_vlan_match_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_match_info_t *match_info,
    bcm_vlan_action_set_t *action_set)
{
    int matched, svp_valid, vp;
    vlan_xlate_1_double_entry_t vx1ent;     /* Lookup key hw buffer.      */
    vlan_xlate_1_double_entry_t res_vx1ent; /* Lookup result buffer.      */
    uint32 profile_idx;                 /* Vlan action profile index. */
    int rv;                             /* Operation return status.   */
    int idx = 0;                        /* Lookup result entry index. */
    soc_mem_t mem = VLAN_XLATE_1_DOUBLEm;

    /* Input parameters check. */
    if (NULL == match_info) {
        return (BCM_E_PARAM);
    }

    sal_memset(&vx1ent, 0, sizeof(vlan_xlate_1_double_entry_t));
    sal_memset(&res_vx1ent, 0, sizeof(vlan_xlate_1_double_entry_t));

    /* Set Key fields */
    BCM_IF_ERROR_RETURN
        (_bcm_td3_vlan_mac_port_key_fields_set(unit, match_info, &vx1ent));

    /* Perform VLAN_XLATE table search by mac address and port. */
    soc_mem_lock(unit, mem);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &idx,
            &vx1ent, &res_vx1ent, 0);
    soc_mem_unlock(unit, mem);
    BCM_IF_ERROR_RETURN(rv);

    /* Evaluate svp */
    matched = FALSE;
    svp_valid = soc_mem_field32_get(unit, mem, &res_vx1ent, MAC_PORT__SVP_VALIDf);

    if ((port != BCM_GPORT_INVALID) && (svp_valid == TRUE)) {
        vp = soc_mem_field32_get(unit, mem, &res_vx1ent, MAC_PORT__SVPf);
        matched = _bcm_esw_port_vp_gport_check(port, vp);
    } else if ((port == BCM_GPORT_INVALID) && (svp_valid == FALSE)) {
        matched = TRUE;
    } else {
        /* (((port != BCM_GPORT_INVALID) && (svp_assigned == FALSE)) ||
           ((port == BCM_GPORT_INVALID) && (svp_assigned == TRUE))) */
        matched = FALSE;
    }
    if (matched == FALSE) {
        return BCM_E_NOT_FOUND;
    }

    /* Read Action Fields */
    if (soc_mem_field32_get(unit, mem, &res_vx1ent,
                MAC_PORT__VLAN_ACTION_VALIDf)) {
        if (NULL == action_set) {
            return BCM_E_PARAM;
        }

        action_set->new_outer_vlan =
            soc_mem_field32_get(unit, mem, &res_vx1ent, MAC_PORT__OVIDf);
        action_set->new_inner_vlan =
            soc_mem_field32_get(unit, mem, &res_vx1ent, MAC_PORT__IVIDf);
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            action_set->priority =
                soc_mem_field32_get(unit, mem, &res_vx1ent, MAC_PORT__OPRIf);
            action_set->new_outer_cfi =
                soc_mem_field32_get(unit, mem, &res_vx1ent, MAC_PORT__OCFIf);
            action_set->new_inner_pkt_prio =
                soc_mem_field32_get(unit, mem, &res_vx1ent, MAC_PORT__IPRIf);
            action_set->new_inner_cfi =
                soc_mem_field32_get(unit, mem, &res_vx1ent, MAC_PORT__ICFIf);
        } else {
            action_set->priority =
                soc_mem_field32_get(unit, mem, &res_vx1ent, MAC_PORT__PRIf);
        }

        /* Read action profile data. */
        profile_idx = soc_mem_field32_get(unit, mem,
                &res_vx1ent, MAC_PORT__TAG_ACTION_PROFILE_PTRf);
        _bcm_trx_vlan_action_profile_entry_get(unit, action_set, profile_idx);
    }

    return (BCM_E_NONE);
}

int _bcm_td3_vlan_match_action_entry_get(
    int unit,
    void *vment,
    bcm_vlan_match_info_t *match_array,
    bcm_vlan_action_set_t *action_set)
{
    int rv = BCM_E_NONE;
    uint32 port_in, mod_in;
    uint32 trunk_id;
    soc_mem_t mem = VLAN_XLATE_1_DOUBLEm;
    uint32 profile_idx;
#if defined(INCLUDE_L3)
    uint32 vp;
    _bcm_gport_dest_t gport_dest;
#endif

    /* Get entry mac address. */
    soc_mem_mac_addr_get(unit, mem, (uint32 *)(vment),
                   MAC_PORT__MAC_ADDRf, match_array->src_mac);

    /* Get Source Type */
    switch(soc_mem_field32_get(unit, mem, vment, SOURCE_TYPEf)) {
        case 1: /* Port or Trunk */
            match_array->match = BCM_VLAN_MATCH_MAC_PORT;
            if (soc_mem_field32_get(unit, mem, vment, MAC_PORT__Tf)) {
                trunk_id =
                    soc_mem_field32_get(unit, mem, vment, MAC_PORT__TGIDf);
                BCM_GPORT_TRUNK_SET(match_array->port, trunk_id);
            } else {
                port_in =
                    soc_mem_field32_get(unit, mem, vment, MAC_PORT__PORT_NUMf);
                mod_in =
                    soc_mem_field32_get(unit, mem, vment, MAC_PORT__MODULE_IDf);
                BCM_GPORT_MODPORT_SET(match_array->port, mod_in, port_in);
            }
            break;
#if defined(INCLUDE_L3)
        case 2: /* Virtual Port */
            match_array->match = BCM_VLAN_MATCH_MAC_PORT;
            vp = soc_mem_field32_get(unit, mem,
                    vment, MAC_PORT__SOURCE_FIELDf);
            _bcm_gport_dest_t_init(&gport_dest);
            rv = _bcm_vp_gport_dest_fill(unit, vp, &gport_dest);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_esw_gport_construct(unit, &gport_dest, &match_array->port);
            BCM_IF_ERROR_RETURN(rv);
            break;
#endif /* INCLUDE_L3 */
        case 3: /* Port Group */
            match_array->match = BCM_VLAN_MATCH_MAC_PORT_CLASS;
            match_array->port_class =
                soc_mem_field32_get(unit, mem, vment, MAC_PORT__SOURCE_FIELDf);
            break;
        default:
            match_array->match = BCM_VLAN_MATCH_INVALID;
            break;
    }
    /* Get vlan action */
    /*action_set = match_array->action;*/
    if (action_set == NULL) return rv;

    /* Read Data Fields */
    action_set->new_outer_vlan =
        soc_mem_field32_get(unit, mem, vment, MAC_PORT__OVIDf);
    action_set->new_inner_vlan =
        soc_mem_field32_get(unit, mem, vment, MAC_PORT__IVIDf);
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        action_set->priority =
            soc_mem_field32_get(unit, mem, vment, MAC_PORT__OPRIf);
        action_set->new_outer_cfi =
            soc_mem_field32_get(unit, mem, vment, MAC_PORT__OCFIf);
        action_set->new_inner_pkt_prio =
            soc_mem_field32_get(unit, mem, vment, MAC_PORT__IPRIf);
        action_set->new_inner_cfi =
            soc_mem_field32_get(unit, mem, vment, MAC_PORT__ICFIf);
    } else {
        action_set->priority =
            soc_mem_field32_get(unit, mem, vment, MAC_PORT__PRIf);
    }

    profile_idx = soc_mem_field32_get(unit, mem, vment,
            MAC_PORT__TAG_ACTION_PROFILE_PTRf);

    _bcm_trx_vlan_action_profile_entry_get(unit, action_set, profile_idx);

    return rv;
}

/*
 * Function:
 *      _bcm_td3_vlan_match_action_multi_get
 * Purpose:
 *      Get all actions and match criteria for given vlan match.
 * Parameters:
 *      unit       (IN)  Device Number
 *      match      (IN)  vlan match
 *      port       (IN)  gport number
 *      size       (IN)  Number of elements in match_info_array
 *      match_info_array (OUT) Vlan match info array
 *      action_set (OUT) Action Set
 *      count      (OUT) Actual number of elements in match_info_array
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_vlan_match_action_multi_get(int unit, bcm_vlan_match_t match,
    bcm_gport_t port, int size, bcm_vlan_match_info_t *match_info_array,
    bcm_vlan_action_set_t *action_set, int *count)
{
    int vp, svp_valid;
    int rv, i, matched, ctr;
    int imin, imax, nent, vmbytes;
    int key_type_value, key_type;
    bcm_vlan_match_info_t match_info;
    bcm_vlan_action_set_t vlan_action_set;
    vlan_xlate_1_double_entry_t *vmtab = NULL;
    vlan_xlate_1_double_entry_t *vment = NULL;
    soc_mem_t mem = VLAN_XLATE_1_DOUBLEm;

    /* Parameter checks */
    if ((size < 0) || (count == NULL)) {
        return BCM_E_PARAM;
    }
    if ((size > 0) && ((match_info_array == NULL) ||
                (action_set == NULL))) {
        return BCM_E_PARAM;
    }

    imin = soc_mem_index_min(unit, mem);
    imax = soc_mem_index_max(unit, mem);
    nent = soc_mem_index_count(unit, mem);
    vmbytes = soc_mem_entry_words(unit, mem);
    vmbytes = WORDS2BYTES(vmbytes);

    vmtab = soc_cm_salloc(unit, nent * vmbytes, "VLAN_XLATE_1_DOUBLE");

    if (vmtab == NULL) {
        return BCM_E_MEMORY;
    }

    soc_mem_lock(unit, mem);
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
            imin, imax, vmtab);
    soc_mem_unlock(unit, mem);

    if (rv < 0) {
        soc_cm_sfree(unit, vmtab);
        return rv;
    }

    soc_mem_lock(unit, mem);
    for(i = 0, ctr = 0; i < nent; i++) {
        if ((size != 0) && (size <= ctr)) {
            break;
        }

        vment = soc_mem_table_idx_to_pointer(unit, mem,
                vlan_xlate_1_double_entry_t *, vmtab, i);

        if (!((soc_mem_field32_get(unit, mem, vment, BASE_VALID_0f) == 3) &&
              (soc_mem_field32_get(unit, mem, vment, BASE_VALID_1f) == 7))) {
            continue;
        }

        /* Read KeyType */
        key_type_value = soc_mem_field32_get(unit, mem, vment, KEY_TYPEf);
        (void) _bcm_esw_vlan_xlate_key_type_get(unit, key_type_value,
                &key_type);
        switch(key_type) {
            case VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT:
                /* Read Key Fields */
                bcm_vlan_match_info_t_init(&match_info);
                bcm_vlan_action_set_t_init(&vlan_action_set);
                (void) _bcm_td3_vlan_match_action_entry_get(unit,
                        vment, &match_info, &vlan_action_set);
                if (match_info.match == BCM_VLAN_MATCH_INVALID) {
                    matched = FALSE;
                    break;
                }

                /* Evaluate SVP */
                matched = FALSE;
                svp_valid = soc_mem_field32_get(unit, mem, vment, MAC_PORT__SVP_VALIDf);
                if ((port != BCM_GPORT_INVALID) && (svp_valid == TRUE)) {
                    vp = soc_mem_field32_get(unit, mem, vment, MAC_PORT__SVPf);
                    matched = _bcm_esw_port_vp_gport_check(port, vp);
                } else if ((port == BCM_GPORT_INVALID) && (svp_valid == FALSE)) {
                    matched = TRUE;
                } else {
                    /* (((port != BCM_GPORT_INVALID) && (svp_valid == FALSE)) ||
                       ((port == BCM_GPORT_INVALID) && (svp_valid == TRUE))) */
                    matched = FALSE;
                }
                /* Evaluate match criteria */
                if ((match != BCM_VLAN_MATCH_INVALID) &&
                        (match != match_info.match)) {
                    matched = FALSE;
                }
                break;
            default:
                matched = FALSE;
                break;
        }

        if (matched == FALSE) {
            continue;
        }

        ctr++;
        if (size >= ctr) {
            /* Copy Key Fields */
            sal_memcpy(match_info_array, &match_info,
                    sizeof(bcm_vlan_match_info_t));

            /* Read Data Fields */
            sal_memcpy(action_set, &vlan_action_set,
                    sizeof(bcm_vlan_action_set_t));

            action_set++;
            match_info_array++;
        }
    }
    *count = ctr;

    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, vmtab);

    return rv;
}
#endif /* BCM_TRIDENT3_SUPPORT */

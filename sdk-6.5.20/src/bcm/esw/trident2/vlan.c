/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    vlan.c
 * Purpose: Handle trident2 specific vlan features:
 *             Manages VP VLAN membership tables.
 */

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3) 

#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/ptable.h>
#include <soc/mcm/memacc.h>
#include <bcm/error.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>

#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/ipmc.h>
#include <bcm_int/esw/stg.h>
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/vpn.h>
#endif
/*
 * Function:
 *      _bcm_td2_flag_to_stp_state
 * Purpose:
 *      Translate API flags to hardware stp state
 *
 * Parameters:
 *      unit        - (IN)  BCM device number
 *      flags       - (IN)  API flags
 *      egress      - (IN)  TRUE for egress, FALSE for ingress
 * Returns:
 *      STP state
 */
int
_bcm_td2_flags_to_stp_state(int unit, uint32 flags, int egress)
{
    uint32 mask;
    int stp_state;

    mask = egress ? BCM_VLAN_GPORT_ADD_EGRESS_STP_MASK : BCM_VLAN_GPORT_ADD_STP_MASK;

    switch (flags & mask) {
        case BCM_VLAN_GPORT_ADD_STP_BLOCK:
        case BCM_VLAN_GPORT_ADD_EGRESS_STP_BLOCK:
            stp_state = 1;
            break;
        case BCM_VLAN_GPORT_ADD_STP_LEARN:
        case BCM_VLAN_GPORT_ADD_EGRESS_STP_LEARN:
            stp_state = 2;
            break;
        case BCM_VLAN_GPORT_ADD_STP_FORWARD:
        case BCM_VLAN_GPORT_ADD_EGRESS_STP_FORWARD:
            stp_state = 3;
            break;
        default:
            stp_state = 0;
            break;
    }

    return stp_state;
}

/*
 * Function:
 *      _bcm_td2_stp_state_to_flags
 * Purpose:
 *      Translate hardware stp state to API flags
 *
 * Parameters:
 *      unit        - (IN)  BCM device number
 *      stp_state   - (IN)  STP state
 *      egress      - (IN)  TRUE for egress, FALSE for ingress
 *      flags       - (OUT) API flags
 * Returns:
 *      corresponding flag value
 */
int
_bcm_td2_stp_state_to_flags(int unit, int stp_state, int egress)
{
    int flags;

    switch (stp_state) {
        case 1:
            if (egress) {
                flags = BCM_VLAN_GPORT_ADD_EGRESS_STP_BLOCK;
            } else {
                flags = BCM_VLAN_GPORT_ADD_STP_BLOCK;
            }
            break;
        case 2:
            if (egress) {
                flags = BCM_VLAN_GPORT_ADD_EGRESS_STP_LEARN;
            } else {
                flags = BCM_VLAN_GPORT_ADD_STP_LEARN;
            }
            break;
        case 3:
            if (egress) {
                flags = BCM_VLAN_GPORT_ADD_EGRESS_STP_FORWARD;
            } else {
                flags = BCM_VLAN_GPORT_ADD_STP_FORWARD;
            }
            break;
        default:
            if (egress) {
                flags = BCM_VLAN_GPORT_ADD_EGRESS_STP_DISABLE;
            } else {
                flags = BCM_VLAN_GPORT_ADD_STP_DISABLE;
            }
            break;
    }

    return flags;
}

/*
 * Function:
 *      bcm_td2_ing_vp_vlan_membership_add
 * Purpose:
 *      create a ING_VP_VLAN_MEMBERSHIP entry
 *      
 * Parameters:
 *      unit - (IN) BCM device number
 *      vp   - (IN) VP number
 *      vlan - (IN) VLAN 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_ing_vp_vlan_membership_add(int unit, int vp, bcm_vlan_t vlan_vfi, 
								   uint32 flags, 
                                   _bcm_vp_vlan_mbrship_key_type_e key_type)
{
    int rv = BCM_E_NONE;
    ing_vp_vlan_membership_entry_t ivvm_ent;
    ing_vp_vlan_membership_entry_t ivvm_ent_result;
    int index;

    if (!SOC_MEM_IS_VALID(unit, ING_VP_VLAN_MEMBERSHIPm)) {
        return BCM_E_UNAVAIL;
    }

    sal_memset(&ivvm_ent, 0, sizeof(ivvm_ent));

    /* Setup the appropriate fields based on key_type */
    if(key_type == _bcm_vp_vlan_mbrship_vp_vlan_type) {
        /* Set key_type if is valid field, it is so TD2+ onwards. The below
           sections do not need this check since those key_types are not
           valid for earlier devices */
        if (soc_feature(unit, soc_feature_key_type_valid_on_vp_vlan_membership)) {
            soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, KEY_TYPEf, 
                                                    _bcm_vp_vlan_mbrship_vp_vlan_type);
        }
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VPf, vp);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VLANf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_glp_vlan_type){
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, KEY_TYPEf, 
                                                _bcm_vp_vlan_mbrship_glp_vlan_type);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, SGLPf, vp);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VLANf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_vp_vfi_type) {
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, KEY_TYPEf, 
                                                _bcm_vp_vlan_mbrship_vp_vfi_type);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VPf, vp);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VFIf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_glp_vfi_type) {
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, KEY_TYPEf,
                                                _bcm_vp_vlan_mbrship_glp_vfi_type);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, SGLPf, vp);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VFIf, vlan_vfi);
    }
    
    soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, SP_TREEf, 
                        _bcm_td2_flags_to_stp_state(unit, flags, FALSE));
    soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent,
        soc_feature(unit, soc_feature_base_valid) ? BASE_VALIDf : VALIDf, 1);

    rv = soc_mem_search(unit, ING_VP_VLAN_MEMBERSHIPm, MEM_BLOCK_ANY, 
                 &index, &ivvm_ent, &ivvm_ent_result, 0);

    if (rv == SOC_E_NONE) {
        /* found the entry, update the data field and write back  */
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent_result,
                SP_TREEf, _bcm_td2_flags_to_stp_state(unit, flags, FALSE));
        rv = soc_mem_write(unit, ING_VP_VLAN_MEMBERSHIPm,
                          MEM_BLOCK_ALL, index, &ivvm_ent_result);
        
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;  /* error return */
    } else {
        /* not found, add to the table */ 
        rv = soc_mem_insert(unit, ING_VP_VLAN_MEMBERSHIPm, 
                       MEM_BLOCK_ALL, &ivvm_ent);
    }
    return rv;
}

/*
 * Function:
 *      bcm_td2_egr_vp_vlan_membership_add
 * Purpose:
 *      create a EGR_VP_VLAN_MEMBERSHIP entry
 *      
 * Parameters:
 *      unit - (IN) BCM device number
 *      vp   - (IN) VP number
 *      vlan - (IN) VLAN
 *      key_type - (IN) Key type to identify specific entry
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_egr_vp_vlan_membership_add(int unit, int vp, bcm_vlan_t vlan_vfi, uint32 flags, 
                                                 _bcm_vp_vlan_mbrship_key_type_e key_type)
{
    int rv = BCM_E_NONE;
    egr_vp_vlan_membership_entry_t evvm_ent;
    egr_vp_vlan_membership_entry_t evvm_ent_result;
    int index;

    if (!SOC_MEM_IS_VALID(unit, EGR_VP_VLAN_MEMBERSHIPm)) {
        return BCM_E_UNAVAIL;
    }

    sal_memset(&evvm_ent, 0, sizeof(evvm_ent));

    /* Setup the appropriate fields based on key_type */
    if(key_type == _bcm_vp_vlan_mbrship_vp_vlan_type) {

        /* Set key_type if is valid field, it is so TD2+ onwards. The below
           sections do not need this check since those key_types are not
           valid for earlier devices */
        if (soc_feature(unit, soc_feature_key_type_valid_on_vp_vlan_membership)) {
            soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, KEY_TYPEf, 
                                                    _bcm_vp_vlan_mbrship_vp_vlan_type);
        }
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VPf, vp);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VLANf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_glp_vlan_type){
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, KEY_TYPEf, 
                                                _bcm_vp_vlan_mbrship_glp_vlan_type);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, DGLPf, vp);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VLANf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_vp_vfi_type) {
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, KEY_TYPEf, 
                                                _bcm_vp_vlan_mbrship_vp_vfi_type);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VPf, vp);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VFIf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_glp_vfi_type) {
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, KEY_TYPEf, 
                                                _bcm_vp_vlan_mbrship_glp_vfi_type);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, DGLPf, vp);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VFIf, vlan_vfi);
    }

    soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent,
        soc_feature(unit, soc_feature_base_valid) ? BASE_VALIDf : VALIDf, 1);
    soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, SP_TREEf, 
                        _bcm_td2_flags_to_stp_state(unit, flags, FALSE));
    soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, UNTAGf, 
              (flags & BCM_VLAN_PORT_UNTAGGED)? 1:0);

    rv = soc_mem_search(unit, EGR_VP_VLAN_MEMBERSHIPm, MEM_BLOCK_ANY, 
                 &index, &evvm_ent, &evvm_ent_result, 0);

    if (rv == SOC_E_NONE) {
        /* found the entry, update  */
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent_result,
                SP_TREEf, _bcm_td2_flags_to_stp_state(unit, flags, FALSE));
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, 
               &evvm_ent_result, UNTAGf, 
              (flags & BCM_VLAN_PORT_UNTAGGED)? 1:0);
        rv = soc_mem_write(unit, EGR_VP_VLAN_MEMBERSHIPm,
                          MEM_BLOCK_ALL, index, &evvm_ent_result);
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;  /* error return */
    } else {
        /* not found, add to the table */ 
        rv = soc_mem_insert(unit, EGR_VP_VLAN_MEMBERSHIPm, 
                       MEM_BLOCK_ALL, &evvm_ent);
    }
    return rv;
}

/*
 * Function:
 *      bcm_td2_ing_vp_vlan_membership_delete
 * Purpose:
 *      delete a ING_VP_VLAN_MEMBERSHIP entry
 *      
 * Parameters:
 *      unit - (IN) BCM device number
 *      vp   - (IN) VP number
 *      vlan - (IN) VLAN
 *      key_type - (IN) Key type to identify specific entry
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_ing_vp_vlan_membership_delete(int unit, int vp, bcm_vlan_t vlan_vfi, 
                                      _bcm_vp_vlan_mbrship_key_type_e key_type)
{
    int rv = BCM_E_NONE;
    ing_vp_vlan_membership_entry_t ivvm_ent;

    sal_memset(&ivvm_ent, 0, sizeof(ivvm_ent));

    /* Setup the appropriate fields based on key_type */
    if(key_type == _bcm_vp_vlan_mbrship_vp_vlan_type) {
        /* Set key_type if is valid field, it is so TD2+ onwards. The below
           sections do not need this check since those key_types are not
           valid for earlier devices */
        if (soc_feature(unit, soc_feature_key_type_valid_on_vp_vlan_membership)) {
            soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, KEY_TYPEf, 
                                                    _bcm_vp_vlan_mbrship_vp_vlan_type);
        }
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VPf, vp);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VLANf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_glp_vlan_type){
		soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, KEY_TYPEf, 
                                                _bcm_vp_vlan_mbrship_glp_vlan_type);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, SGLPf, vp);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VLANf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_vp_vfi_type) {
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, KEY_TYPEf, 
                                                _bcm_vp_vlan_mbrship_vp_vfi_type);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VPf, vp);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VFIf, vlan_vfi);
    }  else if (key_type == _bcm_vp_vlan_mbrship_glp_vfi_type) {
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, KEY_TYPEf,
                                                _bcm_vp_vlan_mbrship_glp_vfi_type);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, SGLPf, vp);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VFIf, vlan_vfi);
    }
    soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent,
        soc_feature(unit, soc_feature_base_valid) ? BASE_VALIDf : VALIDf, 1);

    rv = soc_mem_delete_return_old(unit, ING_VP_VLAN_MEMBERSHIPm, 
                    MEM_BLOCK_ALL, &ivvm_ent, &ivvm_ent);

    return rv;
}

/*
 * Function:
 *      bcm_td2_egr_vp_vlan_membership_delete
 * Purpose:
 *      delete a EGR_VP_VLAN_MEMBERSHIP entry
 *
 * Parameters:
 *      unit - (IN) BCM device number
 *      vp   - (IN) VP number
 *      vlan - (IN) VLAN
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_egr_vp_vlan_membership_delete(int unit, int vp, bcm_vlan_t vlan_vfi, 
                                      _bcm_vp_vlan_mbrship_key_type_e key_type)
{
    int rv = BCM_E_NONE;
    egr_vp_vlan_membership_entry_t evvm_ent;

    sal_memset(&evvm_ent, 0, sizeof(evvm_ent));

    /* Setup the appropriate fields based on key_type */
	if(key_type == _bcm_vp_vlan_mbrship_vp_vlan_type) {
        /* Set key_type if is valid field, it is so TD2+ onwards. The below
           sections do not need this check since those key_types are not
           valid for earlier devices */
        if (soc_feature(unit, soc_feature_key_type_valid_on_vp_vlan_membership)) {
            soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, KEY_TYPEf, 
                                                    _bcm_vp_vlan_mbrship_vp_vlan_type);
        }
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VPf, vp);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VLANf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_glp_vlan_type){
		soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, KEY_TYPEf, 
                                                _bcm_vp_vlan_mbrship_glp_vlan_type);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, DGLPf, vp);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VLANf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_vp_vfi_type) {
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, KEY_TYPEf, 
                                                _bcm_vp_vlan_mbrship_vp_vfi_type);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VPf, vp);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VFIf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_glp_vfi_type) {
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, KEY_TYPEf, 
                                                _bcm_vp_vlan_mbrship_glp_vfi_type);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, DGLPf, vp);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VFIf, vlan_vfi);
    }

    soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent,
        soc_feature(unit, soc_feature_base_valid) ? BASE_VALIDf : VALIDf, 1);

    rv = soc_mem_delete_return_old(unit, EGR_VP_VLAN_MEMBERSHIPm,
                    MEM_BLOCK_ALL, &evvm_ent, &evvm_ent);

    return rv;
}


/*
 * Function:
 *      bcm_td2_ing_vp_vlan_membership_get
 * Purpose:
 *      get the STP state of this ING_VP_VLAN_MEMBERSHIP entry
 *      
 * Parameters:
 *      unit - (IN) BCM device number
 *      vp   - (IN) VP number
 *      vlan - (IN) VLAN
 *      key_type - (IN) Key type to identify specific entry
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_ing_vp_vlan_membership_get(int unit, int vp, bcm_vlan_t vlan_vfi, uint32 *flags,
                                   _bcm_vp_vlan_mbrship_key_type_e key_type)
{
    ing_vp_vlan_membership_entry_t ivvm_ent;
    ing_vp_vlan_membership_entry_t ivvm_ent_result;
    int idx;
    int sp_state;

    *flags = 0;
    sal_memset(&ivvm_ent, 0, sizeof(ivvm_ent));

    /* Setup the appropriate fields based on key_type */
    if(key_type == _bcm_vp_vlan_mbrship_vp_vlan_type) {
        /* Set key_type if is valid field, it is so TD2+ onwards. The below
           sections do not need this check since those key_types are not
           valid for earlier devices */
        if (soc_feature(unit, soc_feature_key_type_valid_on_vp_vlan_membership)) {
            soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, KEY_TYPEf, 
                                                    _bcm_vp_vlan_mbrship_vp_vlan_type);
        }
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VPf, vp);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VLANf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_glp_vlan_type){
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, KEY_TYPEf, 
                                                _bcm_vp_vlan_mbrship_glp_vlan_type);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, SGLPf, vp);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VLANf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_vp_vfi_type) {
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, KEY_TYPEf, 
                                                _bcm_vp_vlan_mbrship_vp_vfi_type);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VPf, vp);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VFIf, vlan_vfi);
    }  else if (key_type == _bcm_vp_vlan_mbrship_glp_vfi_type) {
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, KEY_TYPEf,
                                                _bcm_vp_vlan_mbrship_glp_vfi_type);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, SGLPf, vp);
        soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VFIf, vlan_vfi);
    }
    
    soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent,
        soc_feature(unit, soc_feature_base_valid) ? BASE_VALIDf : VALIDf, 1);

    BCM_IF_ERROR_RETURN(soc_mem_search(unit, ING_VP_VLAN_MEMBERSHIPm, 
                         MEM_BLOCK_ALL, &idx,
                         &ivvm_ent, &ivvm_ent_result, 0));

    sp_state = soc_ING_VP_VLAN_MEMBERSHIPm_field32_get(unit, &ivvm_ent_result,
                                              SP_TREEf);
    if (sp_state == PVP_STP_DISABLED) {
        *flags = BCM_VLAN_GPORT_ADD_STP_DISABLE;
    } else if (sp_state == PVP_STP_LEARNING) {
        *flags = BCM_VLAN_GPORT_ADD_STP_LEARN;
    } else if (sp_state == PVP_STP_BLOCKING) {
        *flags = BCM_VLAN_GPORT_ADD_STP_BLOCK;
    } else {
        *flags = BCM_VLAN_GPORT_ADD_STP_FORWARD;
    }   
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_egr_vp_vlan_membership_get
 * Purpose:
 *      get the STP state of this EGR_VP_VLAN_MEMBERSHIP entry
 *      
 * Parameters:
 *      unit - (IN) BCM device number
 *      vp   - (IN) VP number
 *      vlan - (IN) VLAN
 *      key_type - (IN) Key type to identify specific entry
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_egr_vp_vlan_membership_get(int unit, int vp, bcm_vlan_t vlan_vfi, uint32 *flags,
                                   _bcm_vp_vlan_mbrship_key_type_e key_type)
{
    egr_vp_vlan_membership_entry_t evvm_ent;
    egr_vp_vlan_membership_entry_t evvm_ent_result;
    int idx;
    int sp_state;
    int untag;

    *flags = 0;
    sal_memset(&evvm_ent, 0, sizeof(evvm_ent));

    /* Setup the appropriate fields based on key_type */
    if(key_type == _bcm_vp_vlan_mbrship_vp_vlan_type) {

        /* Set key_type if is valid field, it is so TD2+ onwards. The below
           sections do not need this check since those key_types are not
           valid for earlier devices */
        if (soc_feature(unit, soc_feature_key_type_valid_on_vp_vlan_membership)) {
            soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, KEY_TYPEf, 
                                                    _bcm_vp_vlan_mbrship_vp_vlan_type);
        }
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VPf, vp);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VLANf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_glp_vlan_type){
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, KEY_TYPEf, 
                                                _bcm_vp_vlan_mbrship_glp_vlan_type);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, DGLPf, vp);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VLANf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_vp_vfi_type) {
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, KEY_TYPEf, 
                                                _bcm_vp_vlan_mbrship_vp_vfi_type);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VPf, vp);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VFIf, vlan_vfi);
    } else if (key_type == _bcm_vp_vlan_mbrship_glp_vfi_type) {
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, KEY_TYPEf, 
                                                _bcm_vp_vlan_mbrship_glp_vfi_type);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, DGLPf, vp);
        soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VFIf, vlan_vfi);
    }

    soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent,
        soc_feature(unit, soc_feature_base_valid) ? BASE_VALIDf : VALIDf, 1);
    BCM_IF_ERROR_RETURN(soc_mem_search(unit, EGR_VP_VLAN_MEMBERSHIPm, 
                         MEM_BLOCK_ALL, &idx,
                         &evvm_ent, &evvm_ent_result, 0));

    sp_state = soc_EGR_VP_VLAN_MEMBERSHIPm_field32_get(unit, &evvm_ent_result,
                                              SP_TREEf);
    if (sp_state == PVP_STP_DISABLED) {
        *flags = BCM_VLAN_GPORT_ADD_STP_DISABLE;
    } else if (sp_state == PVP_STP_LEARNING) {
        *flags = BCM_VLAN_GPORT_ADD_STP_LEARN;
    } else if (sp_state == PVP_STP_BLOCKING) {
        *flags = BCM_VLAN_GPORT_ADD_STP_BLOCK;
    } else {
        *flags = BCM_VLAN_GPORT_ADD_STP_FORWARD;
    }   
    untag = soc_EGR_VP_VLAN_MEMBERSHIPm_field32_get(unit, &evvm_ent_result,
                                              UNTAGf);
    *flags |= untag? BCM_VLAN_PORT_UNTAGGED:0; 
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_ing_vp_vlan_membership_get_all
 * Purpose:
 *      For the given vlan, get all the virtual ports and flags from
 *      ING_VP_VLAN_MEMBERSHIP table.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN.
 *      vp_bitmap - (OUT) Bitmap of virtual ports.
 *      arr_size  - (IN) Number of elements in flags_arr.
 *      flags_arr - (OUT) Array of flags.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_ing_vp_vlan_membership_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap, int arr_size, int *flags_arr)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    int chunk_size, num_chunks, chunk_index;
    int alloc_size;
    uint8 *buf = NULL;
    int i, entry_index_min, entry_index_max;
    uint32 *buf_entry;
    int vp;
    int sp_state;
    soc_field_t vld_fld = VALIDf;

    if (NULL == vp_bitmap) {
        return BCM_E_PARAM;
    }
    if (arr_size != soc_mem_index_count(unit, SOURCE_VPm)) {
        return BCM_E_PARAM;
    } 
    if (NULL == flags_arr) {
        return BCM_E_PARAM;
    } 

    mem = ING_VP_VLAN_MEMBERSHIPm;
    if (soc_feature(unit, soc_feature_base_valid)) {
        vld_fld = BASE_VALIDf;
    }

    chunk_size = 1024;
    num_chunks = soc_mem_index_count(unit, mem) / chunk_size;
    if (soc_mem_index_count(unit, mem) % chunk_size) {
        num_chunks++;
    }

    alloc_size = sizeof(ing_vp_vlan_membership_entry_t) * chunk_size;
    buf = soc_cm_salloc(unit, alloc_size, "ING_VP_VLAN_MEMBERSHIP buffer");
    if (NULL == buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    for (chunk_index = 0; chunk_index < num_chunks; chunk_index++) {

        /* Get a chunk of entries */
        entry_index_min = chunk_index * chunk_size;
        entry_index_max = entry_index_min + chunk_size - 1;
        if (entry_index_max > soc_mem_index_max(unit, mem)) {
            entry_index_max = soc_mem_index_max(unit, mem);
        }
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                entry_index_min, entry_index_max, buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        /* Read each entry of the chunk to extract VP and flags */
        for (i = 0; i < (entry_index_max - entry_index_min + 1); i++) {
            buf_entry = soc_mem_table_idx_to_pointer(unit,
                    mem, uint32 *, buf, i);

            if (soc_mem_field32_get(unit, mem, buf_entry, vld_fld) == 0) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem, buf_entry, VLANf) != vlan) {
                continue;
            }

            vp = soc_mem_field32_get(unit, mem, buf_entry, VPf);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                continue;
            }
            SHR_BITSET(vp_bitmap, vp);

            sp_state = soc_mem_field32_get(unit, mem, buf_entry, SP_TREEf);
            if (sp_state == PVP_STP_DISABLED) {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_DISABLE;
            } else if (sp_state == PVP_STP_LEARNING) {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_LEARN;
            } else if (sp_state == PVP_STP_BLOCKING) {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_BLOCK;
            } else {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_FORWARD;
            }   
        }
    }

cleanup:
    if (buf) {
        soc_cm_sfree(unit, buf);
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_egr_vp_vlan_membership_get_all
 * Purpose:
 *      For the given vlan, get all the virtual ports and flags from
 *      EGR_VP_VLAN_MEMBERSHIP table.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN.
 *      vp_bitmap - (OUT) Bitmap of virtual ports.
 *      arr_size  - (IN) Number of elements in flags_arr.
 *      flags_arr - (OUT) Array of flags.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_egr_vp_vlan_membership_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap, int arr_size, int *flags_arr)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    int chunk_size, num_chunks, chunk_index;
    int alloc_size;
    uint8 *buf = NULL;
    int i, entry_index_min, entry_index_max;
    uint32 *buf_entry;
    int vp;
    int sp_state;
    int untag;
    soc_field_t vld_fld = VALIDf;

    if (NULL == vp_bitmap) {
        return BCM_E_PARAM;
    }
    if (arr_size != soc_mem_index_count(unit, SOURCE_VPm)) {
        return BCM_E_PARAM;
    } 
    if (NULL == flags_arr) {
        return BCM_E_PARAM;
    } 

    mem = EGR_VP_VLAN_MEMBERSHIPm;
    if (soc_feature(unit, soc_feature_base_valid)) {
        vld_fld = BASE_VALIDf;
    }

    chunk_size = 1024;
    num_chunks = soc_mem_index_count(unit, mem) / chunk_size;
    if (soc_mem_index_count(unit, mem) % chunk_size) {
        num_chunks++;
    }

    alloc_size = sizeof(egr_vp_vlan_membership_entry_t) * chunk_size;
    buf = soc_cm_salloc(unit, alloc_size, "EGR_VP_VLAN_MEMBERSHIP buffer");
    if (NULL == buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    for (chunk_index = 0; chunk_index < num_chunks; chunk_index++) {

        /* Get a chunk of entries */
        entry_index_min = chunk_index * chunk_size;
        entry_index_max = entry_index_min + chunk_size - 1;
        if (entry_index_max > soc_mem_index_max(unit, mem)) {
            entry_index_max = soc_mem_index_max(unit, mem);
        }
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                entry_index_min, entry_index_max, buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        /* Read each entry of the chunk to extract VP and flags */
        for (i = 0; i < (entry_index_max - entry_index_min + 1); i++) {
            buf_entry = soc_mem_table_idx_to_pointer(unit,
                    mem, uint32 *, buf, i);

            if (soc_mem_field32_get(unit, mem, buf_entry, vld_fld) == 0) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem, buf_entry, VLANf) != vlan) {
                continue;
            }

            vp = soc_mem_field32_get(unit, mem, buf_entry, VPf);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                continue;
            }
            SHR_BITSET(vp_bitmap, vp);

            sp_state = soc_mem_field32_get(unit, mem, buf_entry, SP_TREEf);
            if (sp_state == PVP_STP_DISABLED) {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_DISABLE;
            } else if (sp_state == PVP_STP_LEARNING) {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_LEARN;
            } else if (sp_state == PVP_STP_BLOCKING) {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_BLOCK;
            } else {
                flags_arr[vp] = BCM_VLAN_GPORT_ADD_STP_FORWARD;
            }   

            untag = soc_mem_field32_get(unit, mem, buf_entry, UNTAGf);
            if (untag) {
                flags_arr[vp] |= BCM_VLAN_PORT_UNTAGGED;
            }
        }
    }

cleanup:
    if (buf) {
        soc_cm_sfree(unit, buf);
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_ing_vp_vlan_membership_delete_all
 * Purpose:
 *      For the given vlan, delete all the virtual ports from
 *      ING_VP_VLAN_MEMBERSHIP table.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_ing_vp_vlan_membership_delete_all(int unit, bcm_vlan_t vlan_vpn)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    int chunk_size, num_chunks, chunk_index;
    int alloc_size;
    uint8 *buf = NULL;
    int i, entry_index_min, entry_index_max;
    uint32 *buf_entry;
    int vp, key_type, glp_vlan_vfi = 0;
    int vlan_vfi;
    soc_field_t vlan_vfi_field;
    soc_field_t vld_fld = VALIDf;

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (_BCM_VPN_VFI_IS_SET(vlan_vpn)) {
        vlan_vfi_field = VFIf;
        _BCM_VPN_GET(vlan_vfi, _BCM_VPN_TYPE_VFI, vlan_vpn);
    } else
#endif
    {
        vlan_vfi = vlan_vpn;
        vlan_vfi_field = VLANf;
    }

    mem = ING_VP_VLAN_MEMBERSHIPm;
    if (soc_feature(unit, soc_feature_base_valid)) {
        vld_fld = BASE_VALIDf;
    }

    /* Set default key_type to vp_vlan_type */
    key_type = _bcm_vp_vlan_mbrship_vp_vlan_type;

    chunk_size = 1024;
    num_chunks = soc_mem_index_count(unit, mem) / chunk_size;
    if (soc_mem_index_count(unit, mem) % chunk_size) {
        num_chunks++;
    }

    alloc_size = sizeof(ing_vp_vlan_membership_entry_t) * chunk_size;
    buf = soc_cm_salloc(unit, alloc_size, "ING_VP_VLAN_MEMBERSHIP buffer");
    if (NULL == buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    for (chunk_index = 0; chunk_index < num_chunks; chunk_index++) {

        /* Get a chunk of entries */
        entry_index_min = chunk_index * chunk_size;
        entry_index_max = entry_index_min + chunk_size - 1;
        if (entry_index_max > soc_mem_index_max(unit, mem)) {
            entry_index_max = soc_mem_index_max(unit, mem);
        }
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                entry_index_min, entry_index_max, buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        /* Read each entry of the chunk */
        for (i = 0; i < (entry_index_max - entry_index_min + 1); i++) {
            buf_entry = soc_mem_table_idx_to_pointer(unit,
                    mem, uint32 *, buf, i);

            if (soc_mem_field32_get(unit, mem, buf_entry, vld_fld) == 0) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem, buf_entry, vlan_vfi_field) != vlan_vfi) {
                continue;
            }

            glp_vlan_vfi = 0;
            /* Get the VP */
            vp = soc_mem_field32_get(unit, mem, buf_entry, VPf);

            /* Set key_type if is valid field, it is so TD2+ onwards. The below
               sections do not need this check since those key_types are not
               valid for earlier devices */
            if (soc_feature(unit, soc_feature_key_type_valid_on_vp_vlan_membership)) {
                /* Get the key_type */
                key_type = soc_mem_field32_get(unit, mem, buf_entry, KEY_TYPEf);
                /* If the key_type is GLP_VLAN, set the flag */
                if ((key_type == _bcm_vp_vlan_mbrship_glp_vlan_type) ||
                    (key_type == _bcm_vp_vlan_mbrship_glp_vfi_type)) {
                    glp_vlan_vfi = 1;
                    vp = soc_mem_field32_get(unit, mem, buf_entry, SGLPf);
                }
            }

            if (!glp_vlan_vfi && !_bcm_vp_used_get(unit, vp, _bcmVpTypeAny)) {
                continue;
            }
            rv = bcm_td2_ing_vp_vlan_membership_delete(unit, vp, vlan_vfi, key_type);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }

cleanup:
    if (buf) {
        soc_cm_sfree(unit, buf);
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_egr_vp_vlan_membership_delete_all
 * Purpose:
 *      For the given vlan, delete all the virtual ports from
 *      EGR_VP_VLAN_MEMBERSHIP table.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_egr_vp_vlan_membership_delete_all(int unit, bcm_vlan_t vlan_vpn)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    int chunk_size, num_chunks, chunk_index;
    int alloc_size;
    uint8 *buf = NULL;
    int i, entry_index_min, entry_index_max;
    uint32 *buf_entry;
    int vp, key_type, glp_vlan_vfi = 0;
    int vlan_vfi;
    soc_field_t vlan_vfi_field;
    soc_field_t vld_fld = VALIDf;

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (_BCM_VPN_VFI_IS_SET(vlan_vpn)) {
        vlan_vfi_field = VFIf;
        _BCM_VPN_GET(vlan_vfi, _BCM_VPN_TYPE_VFI, vlan_vpn);
    } else
#endif
    {
        vlan_vfi = vlan_vpn;
        vlan_vfi_field = VLANf;
    }

    mem = EGR_VP_VLAN_MEMBERSHIPm;
    if (soc_feature(unit, soc_feature_base_valid)) {
        vld_fld = BASE_VALIDf;
    }

    /* Set default key_type to vp_vlan_type */
    key_type = _bcm_vp_vlan_mbrship_vp_vlan_type;

    chunk_size = 1024;
    num_chunks = soc_mem_index_count(unit, mem) / chunk_size;
    if (soc_mem_index_count(unit, mem) % chunk_size) {
        num_chunks++;
    }

    alloc_size = sizeof(egr_vp_vlan_membership_entry_t) * chunk_size;
    buf = soc_cm_salloc(unit, alloc_size, "EGR_VP_VLAN_MEMBERSHIP buffer");
    if (NULL == buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    for (chunk_index = 0; chunk_index < num_chunks; chunk_index++) {

        /* Get a chunk of entries */
        entry_index_min = chunk_index * chunk_size;
        entry_index_max = entry_index_min + chunk_size - 1;
        if (entry_index_max > soc_mem_index_max(unit, mem)) {
            entry_index_max = soc_mem_index_max(unit, mem);
        }
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                entry_index_min, entry_index_max, buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        /* Read each entry of the chunk */
        for (i = 0; i < (entry_index_max - entry_index_min + 1); i++) {
            buf_entry = soc_mem_table_idx_to_pointer(unit,
                    mem, uint32 *, buf, i);

            if (soc_mem_field32_get(unit, mem, buf_entry, vld_fld) == 0) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem, buf_entry, vlan_vfi_field) != vlan_vfi) {
                continue;
            }

            glp_vlan_vfi = 0;
            /* Get the VP */
            vp = soc_mem_field32_get(unit, mem, buf_entry, VPf);

            /* Set key_type if is valid field, it is so TD2+ onwards. The below
               sections do not need this check since those key_types are not
               valid for earlier devices */
            if (soc_feature(unit, soc_feature_key_type_valid_on_vp_vlan_membership)) {
                /* Get the key_type */
                key_type = soc_mem_field32_get(unit, mem, buf_entry, KEY_TYPEf);
                /* If the key_type is GLP_VLAN, set the flag */
                if ((key_type == _bcm_vp_vlan_mbrship_glp_vlan_type) ||
                    (key_type == _bcm_vp_vlan_mbrship_glp_vfi_type)) {
                    glp_vlan_vfi = 1;
                    vp = soc_mem_field32_get(unit, mem, buf_entry, DGLPf);
                }
            }

            if (!glp_vlan_vfi && !_bcm_vp_used_get(unit, vp, _bcmVpTypeAny)) {
                continue;
            }
            rv = bcm_td2_egr_vp_vlan_membership_delete(unit, vp, vlan_vfi, key_type);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }

cleanup:
    if (buf) {
        soc_cm_sfree(unit, buf);
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_vp_vlan_stp_set
 * Purpose:
 *      Set the spanning tree state for a virtual port
 * Parameters:
 *      unit      - (IN) BCM device number
 *      vp        - (IN) VP number
 *      vlan      - (IN) VLAN 
 *      stp_state - (IN) State to set
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_vp_vlan_stp_set(int unit, int vp, bcm_vlan_t vlan, int stp_state)
{
    int rv = BCM_E_NONE;
    int idx;
    int hw_stp_state;
    ing_vp_vlan_membership_entry_t ivvm_ent;
    ing_vp_vlan_membership_entry_t ivvm_ent_result;
    egr_vp_vlan_membership_entry_t evvm_ent;
    egr_vp_vlan_membership_entry_t evvm_ent_result;

    /* Translate API space port state to hw stp port state. */
    BCM_IF_ERROR_RETURN(_bcm_stg_stp_translate(unit, stp_state, &hw_stp_state));
    
    /* Ingress direction */
    sal_memset(&ivvm_ent, 0, sizeof(ivvm_ent));
    soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VPf, vp);
    soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VLANf, vlan);
    soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent,
        soc_feature(unit, soc_feature_base_valid) ? BASE_VALIDf : VALIDf, 1);
    
    BCM_IF_ERROR_RETURN(soc_mem_search(unit, ING_VP_VLAN_MEMBERSHIPm, 
                         MEM_BLOCK_ALL, &idx,
                         &ivvm_ent, &ivvm_ent_result, 0));
    soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent_result, SP_TREEf, 
                         hw_stp_state);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_VP_VLAN_MEMBERSHIPm,
                          MEM_BLOCK_ALL, idx, &ivvm_ent_result));
    
    /* Egress direction */
    sal_memset(&evvm_ent, 0, sizeof(evvm_ent));
    soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VPf, vp);
    soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent, VLANf, vlan);
    soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent,
        soc_feature(unit, soc_feature_base_valid) ? BASE_VALIDf : VALIDf, 1);
    
    BCM_IF_ERROR_RETURN(soc_mem_search(unit, EGR_VP_VLAN_MEMBERSHIPm, 
                         MEM_BLOCK_ALL, &idx,
                         &evvm_ent, &evvm_ent_result, 0));
    soc_EGR_VP_VLAN_MEMBERSHIPm_field32_set(unit, &evvm_ent_result, SP_TREEf, 
                         hw_stp_state);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_VP_VLAN_MEMBERSHIPm,
                          MEM_BLOCK_ALL, idx, &evvm_ent_result));
    
    return rv;
}

/*
 * Function:
 *      bcm_td2_vp_vlan_stp_get
 * Purpose:
 *      Get the spanning tree state for a virtual port
 * Parameters:
 *      unit      - (IN) BCM device number
 *      vp        - (IN) VP number
 *      vlan      - (IN) VLAN 
 *      stp_state - (OUT) State to set
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_vp_vlan_stp_get(int unit, int vp, bcm_vlan_t vlan, int *stp_state)
{
    int rv = BCM_E_NONE;
    int idx;
    int hw_stp_state;
    ing_vp_vlan_membership_entry_t ivvm_ent;
    ing_vp_vlan_membership_entry_t ivvm_ent_result;

    sal_memset(&ivvm_ent, 0, sizeof(ivvm_ent));
    soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VPf, vp);
    soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent, VLANf, vlan);
    soc_ING_VP_VLAN_MEMBERSHIPm_field32_set(unit, &ivvm_ent,
        soc_feature(unit, soc_feature_base_valid) ? BASE_VALIDf : VALIDf, 1);
    
    BCM_IF_ERROR_RETURN(soc_mem_search(unit, ING_VP_VLAN_MEMBERSHIPm, 
                         MEM_BLOCK_ALL, &idx,
                         &ivvm_ent, &ivvm_ent_result, 0));
    hw_stp_state = soc_ING_VP_VLAN_MEMBERSHIPm_field32_get(unit, &ivvm_ent_result, 
                         SP_TREEf);
    
    /* Translate hw stp port state to API format. */
    BCM_IF_ERROR_RETURN(_bcm_stg_pvp_translate(unit, hw_stp_state, stp_state));
    
    return rv;
}

/*
 * Function:
 *      bcm_td2_vp_vlan_membership_add
 * Purpose:
 *      create a VP_VLAN_MEMBERSHIP entry
 *
 * Parameters:
 *      unit - (IN) BCM device number
 *      vp   - (IN) VP number
 *      vlan - (IN) VLAN
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_vp_vlan_membership_add(int unit, int vp, bcm_vlan_t vlan, int flags)
{
    int rv = BCM_E_NONE;
    int vlan_vfi = 0;
    _bcm_vp_vlan_mbrship_key_type_e key_type;

    if ((flags & BCM_VLAN_GPORT_ADD_INGRESS_ONLY) &&
        (flags & BCM_VLAN_GPORT_ADD_EGRESS_ONLY)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (_BCM_VPN_VFI_IS_SET(vlan)) {
        _BCM_VPN_GET(vlan_vfi, _BCM_VPN_TYPE_VFI, vlan);
        key_type = _bcm_vp_vlan_mbrship_glp_vfi_type;
    } else
#endif
    {
        vlan_vfi = vlan;
        key_type = _bcm_vp_vlan_mbrship_glp_vlan_type;
    }

    if (!(flags & BCM_VLAN_GPORT_ADD_EGRESS_ONLY)) {
        BCM_IF_ERROR_RETURN(bcm_td2_ing_vp_vlan_membership_add(unit,
                    vp, vlan_vfi, flags, key_type));
    }

    if (!(flags & BCM_VLAN_GPORT_ADD_INGRESS_ONLY)) {
        BCM_IF_ERROR_RETURN(bcm_td2_egr_vp_vlan_membership_add(unit,
                    vp, vlan_vfi, flags, key_type));
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_vp_vlan_membership_delete
 * Purpose:
 *      delete VP_VLAN_MEMBERSHIP entry
 *
 * Parameters:
 *      unit - (IN) BCM device number
 *      vp   - (IN) VP number
 *      vlan - (IN) VLAN
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_vp_vlan_membership_delete(int unit, int vp, bcm_vlan_t vlan)
{
    int rv = BCM_E_NONE;
    int vlan_vfi = 0;
    int ing_vp_vlan_membership_found = 0;
    int egr_vp_vlan_membership_found = 0;
    _bcm_vp_vlan_mbrship_key_type_e key_type;

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (_BCM_VPN_VFI_IS_SET(vlan)) {
        _BCM_VPN_GET(vlan_vfi, _BCM_VPN_TYPE_VFI, vlan);
        key_type = _bcm_vp_vlan_mbrship_glp_vfi_type;
    } else
#endif
    {
        vlan_vfi = vlan;
        key_type = _bcm_vp_vlan_mbrship_glp_vlan_type;
    }

    rv = bcm_td2_ing_vp_vlan_membership_delete(unit, vp, vlan_vfi, key_type);
    if (BCM_SUCCESS(rv)) {
        ing_vp_vlan_membership_found = TRUE;
    } else if (rv == BCM_E_NOT_FOUND) {
        ing_vp_vlan_membership_found = FALSE;
        rv = BCM_E_NONE;
    } else {
        return rv;
    }

    rv = bcm_td2_egr_vp_vlan_membership_delete(unit, vp, vlan_vfi, key_type);
    if (BCM_SUCCESS(rv)) {
        egr_vp_vlan_membership_found = TRUE;
    } else if (rv == BCM_E_NOT_FOUND) {
        egr_vp_vlan_membership_found = FALSE;
        rv = BCM_E_NONE;
    } else {
        return rv;
    }

    if (!ing_vp_vlan_membership_found && !egr_vp_vlan_membership_found) {
        return BCM_E_NOT_FOUND;
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_vp_vlan_membership_get
 * Purpose:
 *      get the STP state of VP_VLAN_MEMBERSHIP entry
 *
 * Parameters:
 *      unit - (IN) BCM device number
 *      vp   - (IN) VP number
 *      vlan - (IN) VLAN
 *      flags - (OUT) STP state
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_vp_vlan_membership_get(int unit, int vp, bcm_vlan_t vlan, int *flags)
{
    int rv = BCM_E_NONE;
    int vlan_vfi = 0;
    uint32 ing_flags = 0;
    uint32 egr_flags = 0;
    int ing_vp_vlan_membership_found = 0;
    int egr_vp_vlan_membership_found = 0;
    _bcm_vp_vlan_mbrship_key_type_e key_type;

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (_BCM_VPN_VFI_IS_SET(vlan)) {
        _BCM_VPN_GET(vlan_vfi, _BCM_VPN_TYPE_VFI, vlan);
        key_type = _bcm_vp_vlan_mbrship_glp_vfi_type;
    } else
#endif
    {
        vlan_vfi = vlan;
        key_type = _bcm_vp_vlan_mbrship_glp_vlan_type;
    }

    rv = bcm_td2_ing_vp_vlan_membership_get(unit,
                vp, vlan_vfi, &ing_flags, key_type);
    if (BCM_SUCCESS(rv)) {
        ing_vp_vlan_membership_found = TRUE;
        *flags |= ing_flags;
        *flags |= BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP;
    } else if (rv == BCM_E_NOT_FOUND) {
        ing_vp_vlan_membership_found = FALSE;
        rv = BCM_E_NONE;
    } else {
        return rv;
    }

    rv = bcm_td2_egr_vp_vlan_membership_get(unit,
                vp, vlan_vfi, &egr_flags, key_type);
    if (BCM_SUCCESS(rv)) {
        egr_vp_vlan_membership_found = TRUE;
        *flags |= egr_flags;
        *flags |= BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP;
    } else if (rv == BCM_E_NOT_FOUND) {
        egr_vp_vlan_membership_found = FALSE;
        rv = BCM_E_NONE;
    } else {
        return rv;
    }

    if (!ing_vp_vlan_membership_found && !egr_vp_vlan_membership_found) {
        return BCM_E_NOT_FOUND;
    }

    if (ing_vp_vlan_membership_found && !egr_vp_vlan_membership_found) {
        *flags |= BCM_VLAN_GPORT_ADD_INGRESS_ONLY;
    } else if (!ing_vp_vlan_membership_found && egr_vp_vlan_membership_found) {
        *flags |= BCM_VLAN_GPORT_ADD_EGRESS_ONLY;
    }

    return rv;
}

int
bcm_td2_vp_vlan_membership_get_all(int unit, bcm_vlan_t vlan, int array_max,
                                    bcm_gport_t *gport_array, int *flags_array,
                                    int *array_size)
{
    int i = 0;
    int rv = 0;
    int num_vp = 0;
    SHR_BITDCL *ing_vp_bitmap = NULL;
    int *ing_flags_arr = NULL;
    SHR_BITDCL *egr_vp_bitmap = NULL;
    int *egr_flags_arr = NULL;
    bcm_gport_t local_gport;
    int index = 0;
    int gport_found = 0;
    int flags = 0;

    if ((*array_size == array_max) && (gport_array || flags_array)) {
        return BCM_E_NONE;
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    ing_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "ing_vp_bitmap");
    if (NULL == ing_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(ing_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

    ing_flags_arr = sal_alloc(num_vp * sizeof(int), "ing_flags_arr");
    if (NULL == ing_flags_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(ing_flags_arr, 0, num_vp * sizeof(int));

    if (soc_feature(unit, soc_feature_ing_vp_vlan_membership)) {
        rv = bcm_td2_ing_vp_vlan_membership_get_all(unit, vlan, ing_vp_bitmap,
                num_vp, ing_flags_arr);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

    egr_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "egr_vp_bitmap");
    if (NULL == egr_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(egr_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

    egr_flags_arr = sal_alloc(num_vp * sizeof(int), "egr_flags_arr");
    if (NULL == egr_flags_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(egr_flags_arr, 0, num_vp * sizeof(int));

    if (soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {
        rv = bcm_td2_egr_vp_vlan_membership_get_all(unit, vlan, egr_vp_bitmap,
                num_vp, egr_flags_arr);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

    for (i = 0; i < num_vp; i++) {
        if (!SHR_BITGET(ing_vp_bitmap, i) &&
            !SHR_BITGET(egr_vp_bitmap, i)) {
            continue;
        }

        if ((*array_size == array_max) && (gport_array || flags_array)) {
            break;
        }

        if (_bcm_vp_used_get(unit, i, _bcmVpTypeVlan)) {
            BCM_GPORT_VLAN_PORT_ID_SET(local_gport, i);
        } else if (_bcm_vp_used_get(unit, i, _bcmVpTypeNiv)) {
            BCM_GPORT_NIV_PORT_ID_SET(local_gport, i );
        } else if (_bcm_vp_used_get(unit, i, _bcmVpTypeExtender)) {
            BCM_GPORT_EXTENDER_PORT_ID_SET(local_gport, i);
        } else if (_bcm_vp_used_get(unit, i, _bcmVpTypeVxlan)) {
            BCM_GPORT_VXLAN_PORT_ID_SET(local_gport, i);
        } else if (_bcm_vp_used_get(unit, i, _bcmVpTypeMpls)) {
            BCM_GPORT_MPLS_PORT_ID_SET(local_gport, i);
        } else {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        if (NULL != gport_array) {
            gport_found = 0;
            for (index = 0; index < *array_size; index++) {
                if (gport_array[index] == local_gport) {
                    gport_found = 1;
                    break;
                }
            }
            if (gport_found == 1) {
                continue;
            }
            gport_array[*array_size] = local_gport;
        }

        if (NULL != flags_array) {
            flags_array[*array_size] = ing_flags_arr[i] |
                                       egr_flags_arr[i] |
                                       BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP;

            if (SHR_BITGET(ing_vp_bitmap, i) && !SHR_BITGET(egr_vp_bitmap, i)) {
                flags_array[*array_size] |= BCM_VLAN_GPORT_ADD_INGRESS_ONLY;
            }

            if (!SHR_BITGET(ing_vp_bitmap, i) && SHR_BITGET(egr_vp_bitmap, i)) {
                flags_array[*array_size] |= BCM_VLAN_GPORT_ADD_EGRESS_ONLY;
            }

            rv = bcm_tr2_vlan_gport_get(unit, vlan, local_gport, &flags);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            flags_array[*array_size] |= flags;
        }

        (*array_size)++;
    }

cleanup:
    if (NULL != ing_vp_bitmap) {
        sal_free(ing_vp_bitmap);
    }
    if (NULL != ing_flags_arr) {
        sal_free(ing_flags_arr);
    }
    if (NULL != egr_vp_bitmap) {
        sal_free(egr_vp_bitmap);
    }
    if (NULL != egr_flags_arr) {
        sal_free(egr_flags_arr);
    }

    return rv;

}

#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    l3.c
 * Purpose: Tomahawk L3 function implementations
 */

#include <soc/defs.h>
#include <shared/bsl.h>
#if defined(INCLUDE_L3) && defined(BCM_TOMAHAWK_SUPPORT) 

#include <soc/drv.h>
#include <bcm/vlan.h>
#include <bcm/error.h>

#include <bcm/l3.h>
#include <soc/l3x.h>
#include <soc/lpm.h>

#include <soc/tomahawk.h>
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>
#include <soc/trident2.h>
#include <soc/defragment.h>
#include <soc/scache.h>

#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/qos.h>

#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#endif

#if defined(BCM_TRX_SUPPORT) 
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */

#define BSL_LSS_SCACHE (BSL_LS_SHARED_SCACHE | BSL_VERBOSE)

#define _BCM_TD2_L3_MEM_BANKS_ALL     (-1)
#define _BCM_TD2_HOST_ENTRY_NOT_FOUND (-1)
static soc_profile_mem_t *_bcm_th_macda_oui_profile[BCM_MAX_NUM_UNITS];
static soc_profile_mem_t *_bcm_th_vntag_etag_profile[BCM_MAX_NUM_UNITS];


/* IP Options Handling */
#define L3_INFO(_unit_) (&_bcm_l3_bk_info[_unit_])
#define _BCM_IP_OPTION_PROFILE_CHUNK 256
#define _BCM_IP4_OPTIONS_LEN    \
            (soc_mem_index_count(unit, IP_OPTION_CONTROL_PROFILE_TABLEm)/ \
             _BCM_IP_OPTION_PROFILE_CHUNK)

/*
 * Function:
 *      _bcm_th_macda_oui_profile_init
 * Purpose:
 *      Allocate and initialize _bcm_th_macda_oui_profile_init
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
bcm_error_t
_bcm_th_macda_oui_profile_init(int unit)
{
    soc_mem_t mem_profile;
    int entry_words[1];
    
    /* Create profile for MACDA_OUI_PROFILE table*/
    if (_bcm_th_macda_oui_profile[unit] == NULL) {
        _bcm_th_macda_oui_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                          "MACDA OUI Profile Mem");
        if (_bcm_th_macda_oui_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_destroy(unit, _bcm_th_macda_oui_profile[unit]));
    }
    soc_profile_mem_t_init(_bcm_th_macda_oui_profile[unit]);
    mem_profile = EGR_MACDA_OUI_PROFILEm;
    entry_words[0] = BYTES2WORDS(sizeof(egr_macda_oui_profile_entry_t));
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem_profile, entry_words, 1,
                                _bcm_th_macda_oui_profile[unit]));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th_macda_oui_profile_deinit
 * Purpose:
 *      Deallocate _bcm_th_macda_oui_profile
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
void
_bcm_th_macda_oui_profile_deinit(int unit)
{
    if (_bcm_th_macda_oui_profile[unit]) {
        (void)soc_profile_mem_destroy(unit, _bcm_th_macda_oui_profile[unit]);
        sal_free(_bcm_th_macda_oui_profile[unit]);
        _bcm_th_macda_oui_profile[unit] = NULL;
    }
}

/*
 * Function:
 *      _bcm_th_vntag_etag_profile_init
 * Purpose:
 *      Allocate and initialize _bcm_th_vntag_etag_profile
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
bcm_error_t
_bcm_th_vntag_etag_profile_init(int unit)
{
    soc_mem_t mem_profile;
    int entry_words[1];
    void *entries[1];
    uint32 profile_idx;
    egr_vntag_etag_profile_entry_t vntag_etag_profile_entry;

    /* Create profile for EGR_VNTAG_ETAG_PROFILE table */
    if (_bcm_th_vntag_etag_profile[unit] == NULL) {
        _bcm_th_vntag_etag_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                          "EGR VNTAG ETAG Profile Mem");
        if (_bcm_th_vntag_etag_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_destroy(unit, _bcm_th_vntag_etag_profile[unit]));
    }
    soc_profile_mem_t_init(_bcm_th_vntag_etag_profile[unit]);
    mem_profile = EGR_VNTAG_ETAG_PROFILEm;
    entry_words[0] = BYTES2WORDS(sizeof(egr_vntag_etag_profile_entry_t));
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem_profile, entry_words, 1,
                                _bcm_th_vntag_etag_profile[unit]));

    /* Index 0 in EGR_VNTAG_ETAG_PROFILE table is reserved for non NIV/PE flow
     * hence adding this dummy entry at index 0 during init time
     */
    sal_memset(&vntag_etag_profile_entry, 0, sizeof(vntag_etag_profile_entry));
    entries[0] = &vntag_etag_profile_entry;
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_th_vntag_etag_profile[unit],
                             entries, 1, &profile_idx));
    
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th_vntag_etag_profile_deinit
 * Purpose:
 *      Deallocate _bcm_th_vntag_etag_profile
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
void
_bcm_th_vntag_etag_profile_deinit(int unit)
{
    if (_bcm_th_vntag_etag_profile[unit]) {
        (void)soc_profile_mem_destroy(unit, _bcm_th_vntag_etag_profile[unit]);
        sal_free(_bcm_th_vntag_etag_profile[unit]);
        _bcm_th_vntag_etag_profile[unit] = NULL;
    }
}

/*
 * Function:
 *      _bcm_th_l3_vp_entry_del
 * Purpose:
 *      Delete entry from profile table
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      ipv6     - (IN) IPV4 or IPv6 flag
 *      l3cfg    - (IN) L3 entry info.
 *      bufentry - (IN) hw buffer
 * Returns:
 */
bcm_error_t
_bcm_th_l3_vp_entry_del(int unit, _bcm_l3_cfg_t *l3cfg,
                        int macda_oui_profile_index ,int vntag_etag_profile_index)
{

    int rv = BCM_E_NONE;

    if (BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
        
        /* delete an entry or decrement ref count from EGR_MACDA_OUI_PROFILE table */
        soc_mem_lock(unit, EGR_MACDA_OUI_PROFILEm);
        if (macda_oui_profile_index != -1) {
            rv = soc_profile_mem_delete(unit, _bcm_th_macda_oui_profile[unit],
                                        macda_oui_profile_index);
        }
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, EGR_MACDA_OUI_PROFILEm);
            return rv;
        }
        soc_mem_unlock(unit, EGR_MACDA_OUI_PROFILEm);
        
        /* delete an entry or decrement ref count from EGR_VNTAG_ETAG_PROFILE table */
        soc_mem_lock(unit, EGR_VNTAG_ETAG_PROFILEm);
        /* profile 0 is reserved for NON NIV/PE entries */
        if (vntag_etag_profile_index != 0) {
            rv = soc_profile_mem_delete(unit, _bcm_th_vntag_etag_profile[unit],
                                        vntag_etag_profile_index);
        }
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, EGR_VNTAG_ETAG_PROFILEm);
            return rv;
        }
        soc_mem_unlock(unit, EGR_VNTAG_ETAG_PROFILEm);

    }
    
    return rv;
}

/*
 * Function:
 *      _bcm_th_l3_vp_ent_parse
 * Purpose:
 *      TH helper routine used to parse hw l3 entry to api format
 * Parameters:
 *      unit      - (IN) SOC unit number. 
 *      mem       - (IN) L3 table memory.
 *      l3cfg     - (IN) L3 entry info.
 *      l3x_entry - (IN) hw buffer.
 * Returns:
 */
bcm_error_t
_bcm_th_l3_vp_ent_parse(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg, void *l3x_entry)
{

    uint32 profile_index;
    uint32 dst_vif;
    uint32 mac_field;
    bcm_gport_t vp_gport;
    uint32 vntag_actions;
    int rv = BCM_E_NONE;
    uint32 glp;                   /* Global port.                */
    int ipv6;                     /* Entry is IPv6 flag.         */
    _bcm_l3_fields_t *fld;        /* L3 table common fields.     */
    uint32 *buf_p;                /* HW buffer address.          */
    egr_macda_oui_profile_entry_t macda_oui_profile_entry;
    egr_vntag_etag_profile_entry_t vntag_etag_profile_entry;
    int is_trunk = -1;
    uint32 HG_VNTAG_MODIFY_ENABLE_FLAG = 0;

    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    buf_p = (uint32 *)l3x_entry;

    /* Set table fields */
    BCM_TD2_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
    soc_mem_mac_address_get(unit, mem, buf_p, fld->macda_low, 
                            l3cfg->l3c_mac_addr,
                            SOC_MEM_MAC_LOWER_ONLY);
    /* Read upper 3 bytes of MAC address from profile table */
    profile_index = soc_mem_field32_get(unit, mem, buf_p,
                                              fld->oui_profile_id);
    SOC_IF_ERROR_RETURN
        (READ_EGR_MACDA_OUI_PROFILEm(unit, MEM_BLOCK_ANY, profile_index,
                                     &macda_oui_profile_entry));
    soc_EGR_MACDA_OUI_PROFILEm_field_get(unit, &macda_oui_profile_entry,
                                         MACDA_OUIf, &mac_field);
    l3cfg->l3c_mac_addr[0] = (uint8) (mac_field >> 16 & 0xff);
    l3cfg->l3c_mac_addr[1] = (uint8) (mac_field >> 8 & 0xff);
    l3cfg->l3c_mac_addr[2] = (uint8) (mac_field & 0xff);
    glp = soc_mem_field32_get(unit, mem, buf_p, fld->glp);
    _bcm_th_glp_resolve_embedded_nh(unit, glp, &l3cfg->l3c_modid,
                                    &l3cfg->l3c_port_tgid, &is_trunk);
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        uint32 dest, dtype;
        dest = soc_mem_field32_dest_get(unit, mem, buf_p,
                fld->dest, &dtype);
        if (dtype == SOC_MEM_FIF_DEST_LAG) {
            is_trunk = 1;
            l3cfg->l3c_port_tgid = (dest & SOC_MEM_FIF_DGPP_TGID_MASK);
        } else if (dtype == SOC_MEM_FIF_DEST_DGPP) {
            l3cfg->l3c_port_tgid = (dest & SOC_MEM_FIF_DGPP_PORT_MASK);
            l3cfg->l3c_modid = (dest & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
        }
    }
#endif
    if (is_trunk == 1) {
        l3cfg->l3c_flags |= BCM_L3_TGID;
    }
    
    dst_vif = soc_mem_field32_get(unit, mem, buf_p,
                                  fld->dst_vif);
    profile_index = soc_mem_field32_get(unit, mem, buf_p,
                                        fld->vntag_etag_profile_id);

    if (profile_index != L3_EXT_VIEW_INVALID_VNTAG_ETAG_PROFILE) {
        SOC_IF_ERROR_RETURN
            (READ_EGR_VNTAG_ETAG_PROFILEm(unit, MEM_BLOCK_ANY, profile_index,
                                         &vntag_etag_profile_entry));
        soc_EGR_VNTAG_ETAG_PROFILEm_field_get(unit, &vntag_etag_profile_entry,
                                              HG_VNTAG_MODIFY_ENABLEf,
                                              &HG_VNTAG_MODIFY_ENABLE_FLAG);
        l3cfg->l3c_flags2 = HG_VNTAG_MODIFY_ENABLE_FLAG;
        soc_EGR_VNTAG_ETAG_PROFILEm_field_get(unit, &vntag_etag_profile_entry,
                                              VNTAG_ACTIONSf, &vntag_actions);
        switch (vntag_actions) {
            case VNTAG_ACTIONS_NIV: /* NIV encap */
                if (_bcm_niv_gport_get(unit, is_trunk, l3cfg->l3c_modid,
                                       l3cfg->l3c_port_tgid, dst_vif,
                                       &vp_gport) == BCM_E_NONE) {
                    l3cfg->l3c_port_tgid = vp_gport;
                }
                break;
            case VNTAG_ACTIONS_ETAG: /* PE encap */
                if (_bcm_extender_gport_get(unit, is_trunk, l3cfg->l3c_modid,
                                            l3cfg->l3c_port_tgid, dst_vif,
                                            &vp_gport) == BCM_E_NONE) {
                    l3cfg->l3c_port_tgid = vp_gport;
                }
                break;
            default:
                break;
        }
    }
    
    return rv;
}


/*
 * Function:
 *      _bcm_th_l3_vp_entry_add
 * Purpose:
 *      TH helper routine used to parse hw l3 entry to api format
 * Parameters:
 *      unit      - (IN) SOC unit number. 
 *      mem       - (IN) L3 table memory.
 *      l3cfg     - (IN) L3 entry info.
 *      buf_p     - (IN) hw buffer.
 * Returns:
 *      BCM_X_XXX
 */
bcm_error_t
_bcm_th_l3_vp_entry_add(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg,
                        uint32 *buf_p, int *macda_oui_profile_index,
                        int *vntag_etag_profile_index)
{

    int rv = BCM_E_NONE;
    int rv1 = BCM_E_NONE;
    int ipv6;                     /* Entry is IPv6 flag.         */
    _bcm_l3_fields_t *fld;        /* L3 table common fields.     */
    uint32 *bufp;                 /* HW buffer address.          */
    uint32 profile_idx;
    uint32 mac_field;
    uint32 vntag_actions;
    void *entries[1];
    egr_macda_oui_profile_entry_t macda_oui_profile_entry;
    egr_vntag_etag_profile_entry_t vntag_etag_profile_entry;

    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    /* Set table fields */
    BCM_TD2_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
    bufp = buf_p;
    
    if (soc_feature(unit, soc_feature_virtual_port_routing) &&
        (l3cfg->l3c_encap_id > 0 &&
         l3cfg->l3c_encap_id < BCM_XGS3_EGRESS_IDX_MIN(unit))) {
        int virtual_interface_id;
        uint32 hg_vntag_modify = 1;
        int count;
        if (_bcm_vp_used_get(unit, l3cfg->l3c_encap_id,
                             _bcmVpTypeNiv)) { /* NIV */
            bcm_niv_port_t niv_port;
            bcm_niv_egress_t niv_egress;
            BCM_GPORT_NIV_PORT_ID_SET(niv_port.niv_port_id,
                                      l3cfg->l3c_encap_id);
            BCM_IF_ERROR_RETURN(bcm_esw_niv_port_get(unit, &niv_port));
            if (niv_port.flags & BCM_NIV_PORT_MATCH_NONE) {
                BCM_IF_ERROR_RETURN(bcm_esw_niv_egress_get(unit,
                                    niv_port.niv_port_id, 1,
                                    &niv_egress, &count));
                if (count == 0) {
                    return BCM_E_CONFIG;
                }
                if (niv_egress.flags & BCM_NIV_EGRESS_MULTICAST) {
                    return BCM_E_PARAM;
                }
                virtual_interface_id = niv_egress.virtual_interface_id;
            } else {
                if (niv_port.flags & BCM_NIV_PORT_MULTICAST) {
                    return BCM_E_PARAM;
                }
                virtual_interface_id = niv_port.virtual_interface_id;
            }
            soc_mem_field32_set(unit, mem, bufp, fld->dst_vif,
                                virtual_interface_id);
            
            sal_memset(&vntag_etag_profile_entry, 0, sizeof(vntag_etag_profile_entry));
            vntag_actions = 1;
            soc_EGR_VNTAG_ETAG_PROFILEm_field_set(unit, &vntag_etag_profile_entry,
                                                  VNTAG_ACTIONSf, &vntag_actions);
            if (l3cfg->l3c_flags2 & BCM_L3_FLAGS2_NIV_ENCAP_LOCAL) {
                soc_EGR_VNTAG_ETAG_PROFILEm_field_set(unit, &vntag_etag_profile_entry,
                                                      HG_VNTAG_MODIFY_ENABLEf, &hg_vntag_modify);
            }
            entries[0] = &vntag_etag_profile_entry;
            BCM_IF_ERROR_RETURN(soc_profile_mem_add(unit, _bcm_th_vntag_etag_profile[unit],
                                                    entries, 1, &profile_idx));
            *vntag_etag_profile_index = profile_idx;
            soc_mem_field32_set(unit, mem, bufp, fld->vntag_etag_profile_id,
                                profile_idx);
        } else if (_bcm_vp_used_get(unit, l3cfg->l3c_encap_id,
                                    _bcmVpTypeExtender)) { /* PE */
            bcm_extender_port_t ep;
            int etag_dot1p_mapping_ptr = 0;
            bcm_extender_egress_t extender_egress;
            uint16 extended_port_vid;
            int qos_map_id;

            BCM_GPORT_EXTENDER_PORT_ID_SET(ep.extender_port_id,
                                           l3cfg->l3c_encap_id);
            BCM_IF_ERROR_RETURN(bcm_esw_extender_port_get(unit, &ep));
            if (ep.flags & BCM_EXTENDER_PORT_MATCH_NONE) {
                bcm_extender_egress_t_init(&extender_egress);
                BCM_IF_ERROR_RETURN(bcm_esw_extender_egress_get_all(unit,
                            ep.extender_port_id, 1, &extender_egress, &count));
                if (count == 0) {
                    return BCM_E_CONFIG;
                }
                if (extender_egress.flags & BCM_EXTENDER_EGRESS_MULTICAST) {
                    return BCM_E_PARAM;
                }
                qos_map_id        = extender_egress.qos_map_id;
                extended_port_vid = extender_egress.extended_port_vid;
            } else {
                if (ep.flags & BCM_EXTENDER_PORT_MULTICAST) {
                    return BCM_E_PARAM;
                }
                qos_map_id        = ep.qos_map_id;
                extended_port_vid = ep.extended_port_vid;
            }

            sal_memset(&vntag_etag_profile_entry, 0, sizeof(vntag_etag_profile_entry));
            vntag_actions = 2;
            soc_EGR_VNTAG_ETAG_PROFILEm_field_set(unit, &vntag_etag_profile_entry,
                                                  VNTAG_ACTIONSf, &vntag_actions);
            if (l3cfg->l3c_flags2 & BCM_L3_FLAGS2_NIV_ENCAP_LOCAL) {
                soc_EGR_VNTAG_ETAG_PROFILEm_field_set(unit, &vntag_etag_profile_entry,
                                                      HG_VNTAG_MODIFY_ENABLEf, &hg_vntag_modify);
            }
            bcm_td2_qos_egr_etag_id2profile(unit, qos_map_id,
                                            &etag_dot1p_mapping_ptr);
            soc_EGR_VNTAG_ETAG_PROFILEm_field_set(unit, &vntag_etag_profile_entry,
                                                  ETAG_DOT1P_MAPPING_PTRf,
                                                  (uint32 *)&etag_dot1p_mapping_ptr);

            entries[0] = &vntag_etag_profile_entry;
            BCM_IF_ERROR_RETURN(soc_profile_mem_add(unit, _bcm_th_vntag_etag_profile[unit],
                                                    entries, 1, &profile_idx));
            *vntag_etag_profile_index = profile_idx;
            soc_mem_field32_set(unit, mem, bufp, fld->vntag_etag_profile_id,
                                profile_idx);
            virtual_interface_id = extended_port_vid;
            soc_mem_field32_set(unit, mem, bufp, fld->dst_vif, virtual_interface_id);
        } else {
            return BCM_E_PARAM;
        }
    } else {
        soc_mem_field32_set(unit, mem, bufp, fld->vntag_etag_profile_id,
                            L3_EXT_VIEW_INVALID_VNTAG_ETAG_PROFILE);
    }
    soc_mem_mac_address_set(unit, mem, bufp, fld->macda_low,
                            l3cfg->l3c_mac_addr,
                            SOC_MEM_MAC_LOWER_ONLY);
    mac_field = ((l3cfg->l3c_mac_addr[0] << 16) |
                 (l3cfg->l3c_mac_addr[1] << 8)  |
                 (l3cfg->l3c_mac_addr[2] << 0));
    sal_memset(&macda_oui_profile_entry, 0, sizeof(macda_oui_profile_entry));
    soc_EGR_MACDA_OUI_PROFILEm_field_set(unit, &macda_oui_profile_entry,
                                         MACDA_OUIf, &mac_field);
    entries[0] = &macda_oui_profile_entry;
    rv = soc_profile_mem_add(unit, _bcm_th_macda_oui_profile[unit],
                             entries, 1, &profile_idx);
    if (BCM_FAILURE(rv)) {
        /* delete previously added entry or decrement ref count
         * from EGR_VNTAG_ETAG_PROFILE table
         */
        soc_mem_lock(unit, EGR_VNTAG_ETAG_PROFILEm);
        /* profile 0 is reserved for NON NIV/PE entries */
        if (*vntag_etag_profile_index != 0) {
            rv1 = soc_profile_mem_delete(unit, _bcm_th_vntag_etag_profile[unit],
                                         *vntag_etag_profile_index);
        }
        if (BCM_FAILURE(rv1)) {
            soc_mem_unlock(unit, EGR_VNTAG_ETAG_PROFILEm);
            return rv1;
        }
        soc_mem_unlock(unit, EGR_VNTAG_ETAG_PROFILEm);
        return rv;
    }
  
    *macda_oui_profile_index = profile_idx;
    soc_mem_field32_set(unit, mem, bufp, fld->oui_profile_id,
                        profile_idx);

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        if (BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
            soc_mem_field32_set(unit, mem, bufp, MACDA_OUI_PROFILE_VALIDf, 1);
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *      th_glp_resolve
 * Purpose:
 *      TH helper routine  function to get modid, port, and trunk_id
 *      from a bcm_gport_t
 * Parameters  : (IN)  unit      - BCM device number
 *               (IN)  gport     - Global port identifier
 *               (OUT) modid     - Module ID
 *               (OUT) port      - Port number
 *               (OUT) is_trunk  - Trunk

 * Returns:
 *      BCM_X_XXX
 */

void _bcm_th_glp_resolve_embedded_nh(int unit, uint32 gport,
                                     bcm_module_t *modid, bcm_port_t *port,
                                     int *is_trunk)
{

    uint8 port_mask_len = 0, modid_mask_len = 0;
    uint32 temp = 0;    
               
    /* Determine length of port mask */
    temp = SOC_PORT_ADDR_MAX(unit);
    while (0 != temp) {
        port_mask_len++;
        temp >>= 1;
    }
    
    /* Determine length of modid mask */
    temp = SOC_MODID_MAX(unit);
    while (0 != temp) {
        modid_mask_len++;
        temp >>= 1;
    }
    *modid = (gport >> port_mask_len) & SOC_MODID_MAX(unit);   
                  
    if (gport & (1 << (port_mask_len + modid_mask_len))) {
        *is_trunk = 1;
    }
    *port = gport & SOC_PORT_ADDR_MAX(unit);
}

/*
 * Function:
 *      _bcm_th_l3_ext_ent_init
 * Purpose:
 *      TH helper routine used to init extended l3 host entry buffer
 * Parameters:
 *      unit      - (IN) SOC unit number. 
 *      mem       - (IN) L3 extended host memory.
 *      l3cfg     - (IN/OUT) l3 entry  lookup key & search result.
 *      l3x_entry - (IN) hw buffer.
 * Returns:
 *      void
 */
STATIC int
_bcm_th_l3_ext_ent_init(int unit, soc_mem_t mem_ext, 
                        _bcm_l3_cfg_t *l3cfg, void *l3x_entry)
{
    int ipv6;                     /* Entry is IPv6 flag.         */
    uint32 *buf_p;                /* HW buffer address.          */ 


    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Zero destination buffer. */
    buf_p = (uint32 *)l3x_entry;
    sal_memset(buf_p, 0, BCM_L3_MEM_ENT_SIZE(unit, mem_ext)); 

    if (ipv6) { /* ipv6 entry */
        if (BCM_XGS3_L3_MEM(unit, v6_4) == mem_ext) { 
            soc_mem_ip6_addr_set(unit, mem_ext, buf_p, IPV6UC_EXT__IP_ADDR_LWR_64f,
                                 l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_ONLY);
            soc_mem_ip6_addr_set(unit, mem_ext, buf_p, IPV6UC_EXT__IP_ADDR_UPR_64f,
                                l3cfg->l3c_ip6, SOC_MEM_IP6_UPPER_ONLY);
            soc_mem_field32_set(unit, mem_ext, buf_p, IPV6UC_EXT__VRF_IDf,
                                l3cfg->l3c_vrf);
            /* ipv6 extended host entry */
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                soc_mem_field32_set(unit, mem_ext, buf_p, KEY_TYPEf,
                                    TD3_L3_HASH_KEY_TYPE_V6UC_EXT);
                soc_mem_field32_set(unit, mem_ext, buf_p, DATA_TYPEf,
                                    TD3_L3_HASH_DATA_TYPE_V6UC_EXT);
                soc_mem_field32_set(unit, mem_ext, buf_p, BASE_VALID_0f, 3);
                soc_mem_field32_set(unit, mem_ext, buf_p, BASE_VALID_1f, 4);
                soc_mem_field32_set(unit, mem_ext, buf_p, BASE_VALID_2f, 7);
                soc_mem_field32_set(unit, mem_ext, buf_p, BASE_VALID_3f, 7);
            } else
#endif
            {
                soc_mem_field32_set(unit, mem_ext, buf_p, KEY_TYPE_0f, 3);
                soc_mem_field32_set(unit, mem_ext, buf_p, KEY_TYPE_1f, 3);
                soc_mem_field32_set(unit, mem_ext, buf_p, KEY_TYPE_2f, 3);
                soc_mem_field32_set(unit, mem_ext, buf_p, KEY_TYPE_3f, 3);

                soc_mem_field32_set(unit, mem_ext, buf_p, VALID_0f, 1);
                soc_mem_field32_set(unit, mem_ext, buf_p, VALID_1f, 1);
                soc_mem_field32_set(unit, mem_ext, buf_p, VALID_2f, 1);
                soc_mem_field32_set(unit, mem_ext, buf_p, VALID_3f, 1);
            }
        } else {
            return BCM_E_NOT_FOUND;
        }
    } else { /* ipv4 entry */
        if (BCM_XGS3_L3_MEM(unit, v4_2) == mem_ext) { 
            soc_mem_field32_set(unit, mem_ext, buf_p, IPV4UC_EXT__IP_ADDRf,
                                l3cfg->l3c_ip_addr);
            soc_mem_field32_set(unit, mem_ext, buf_p, IPV4UC_EXT__VRF_IDf,
                                l3cfg->l3c_vrf);
            /* ipv4 extended host entry */
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                soc_mem_field32_set(unit, mem_ext, buf_p, KEY_TYPEf,
                                    TD3_L3_HASH_KEY_TYPE_V4UC_EXT);
                soc_mem_field32_set(unit, mem_ext, buf_p, DATA_TYPEf,
                                    TD3_L3_HASH_DATA_TYPE_V4UC_EXT);
                soc_mem_field32_set(unit, mem_ext, buf_p, BASE_VALID_0f, 1);
                soc_mem_field32_set(unit, mem_ext, buf_p, BASE_VALID_1f, 7);
            } else
#endif
            {
                soc_mem_field32_set(unit, mem_ext, buf_p, KEY_TYPE_0f, 1);
                soc_mem_field32_set(unit, mem_ext, buf_p, KEY_TYPE_1f, 1);
                soc_mem_field32_set(unit, mem_ext, buf_p, VALID_1f, 1);
                soc_mem_field32_set(unit, mem_ext, buf_p, VALID_0f, 1);
            }
        } else {
            return BCM_E_NOT_FOUND;
        }
    }
    return BCM_E_NONE;
    
}

/*
 * Function:
 *      _bcm_th_l3_ent_init
 * Purpose:
 *      TH helper routine used to init l3 host entry buffer
 * Parameters:
 *      unit      - (IN) SOC unit number. 
 *      mem       - (IN) L3 host memory.
 *      l3cfg     - (IN/OUT) l3 entry  lookup key & search result.
 *      l3x_entry - (IN) hw buffer.
 * Returns:
 *      void
 */
STATIC int
_bcm_th_l3_ent_init(int unit, soc_mem_t mem, 
                    _bcm_l3_cfg_t *l3cfg, void *l3x_entry)
{
    int ipv6;                     /* Entry is IPv6 flag.         */
    uint32 *buf_p;                /* HW buffer address.          */ 


    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Zero destination buffer. */
    buf_p = (uint32 *)l3x_entry;
    sal_memset(buf_p, 0, BCM_L3_MEM_ENT_SIZE(unit, mem)); 

    if (ipv6) { /* ipv6 entry */
        if (BCM_XGS3_L3_MEM(unit, v6) == mem) {
            soc_mem_field32_set(unit, mem, buf_p, VRF_IDf,
                                l3cfg->l3c_vrf);
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6UC__IP_ADDR_LWR_64f,
                                     l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_ONLY);
                soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6UC__IP_ADDR_UPR_64f,
                                     l3cfg->l3c_ip6, SOC_MEM_IP6_UPPER_ONLY);
                soc_mem_field32_set(unit, mem, buf_p, KEY_TYPEf, 2);
                soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_0f, 3);
                soc_mem_field32_set(unit, mem, buf_p, BASE_VALID_1f, 4);
            } else
#endif
            {
                soc_mem_ip6_addr_set(unit, mem, buf_p, IP_ADDR_LWR_64f,
                                     l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_ONLY);
                soc_mem_ip6_addr_set(unit, mem, buf_p, IP_ADDR_UPR_64f,
                                     l3cfg->l3c_ip6, SOC_MEM_IP6_UPPER_ONLY);
                soc_mem_field32_set(unit, mem, buf_p, VALID_0f, 1);
                soc_mem_field32_set(unit, mem, buf_p, VALID_1f, 1);
                soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_0f, 2);
                soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_1f, 2);
            }
        } else {
            return BCM_E_NOT_FOUND;
        }
    } else { /* ipv4 entry */
        if (BCM_XGS3_L3_MEM(unit, v4) == mem) {
            soc_mem_field32_set(unit, mem, buf_p, IP_ADDRf,
                                l3cfg->l3c_ip_addr);
            soc_mem_field32_set(unit, mem, buf_p, VRF_IDf,
                                l3cfg->l3c_vrf);
            /* ipv4 unicast */
            soc_mem_field32_set(unit, mem, buf_p, KEY_TYPEf, 0); 
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                soc_mem_field32_set(unit, mem, buf_p, BASE_VALIDf, 1);
            } else
#endif
            {
                soc_mem_field32_set(unit, mem, buf_p, VALIDf, 1);
            }
        } else {
            return BCM_E_NOT_FOUND;
        }
    }
    return BCM_E_NONE;
    
}

/*
 * Function:
 *      _bcm_th_get_extended_profile_index
 * Purpose:
 *      TH helper routine used to get profile index for EGR_MACDA_OUI_PROFILE
 *      and EGR_VNTAG_ETAG_PROFILE
 * Parameters:
 *      unit                      - (IN) SOC unit number. 
 *      mem                       - (IN) L3 Extended table memory.
 *      l3cfg                     - (IN) L3 entry info.
 *      macda_oui_profile_index   - (out) EGR_MACDA_OUI profile index.
 *      vntag_etag_profile_index  - (out) EGR_VNTAG_ETAG profile index.
 * Returns:
 *      BCM_X_XXX
 */
bcm_error_t _bcm_th_get_extended_profile_index(int unit, soc_mem_t mem_ext,
                                               _bcm_l3_cfg_t *l3cfg,
                                               int *macda_oui_profile_index,
                                               int *vntag_etag_profile_index,
                                               int *ref_count) {
                                        
    int ipv6;                          /* IPv6 entry indicator.*/
    int rv = BCM_E_NONE;               /* Operation return status. */
    uint32 *buf_key, *buf_entry;       /* Key and entry buffer ptrs*/
    l3_entry_ipv4_unicast_entry_t l3v4_key, l3v4_entry;            /* IPv4 */
    l3_entry_ipv4_multicast_entry_t l3v4_ext_key, l3v4_ext_entry; /* IPv4-Embedded */
    l3_entry_ipv6_unicast_entry_t l3v6_key, l3v6_entry;           /* IPv6 */
    l3_entry_ipv6_multicast_entry_t l3v6_ext_key, l3v6_ext_entry; /* IPv6-Embedded */
    soc_field_t macda_oui_id, vntag_etag_id;
    soc_mem_t mem;
   

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    
    if (ipv6) {
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            macda_oui_id  = IPV6UC_EXT__MACDA_OUI_PROFILE_INDEXf;
        } else
#endif
        {
            macda_oui_id  = IPV6UC_EXT__MAC_DA_OUI_PROFILE_IDf;
        }
        vntag_etag_id = IPV6UC_EXT__VNTAG_ETAG_PROFILE_IDf;
        mem = BCM_XGS3_L3_MEM(unit, v6);
    } else {
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            macda_oui_id  = IPV4UC_EXT__MACDA_OUI_PROFILE_INDEXf;
        } else
#endif
        {
            macda_oui_id  = IPV4UC_EXT__MAC_DA_OUI_PROFILE_IDf;
        }
        vntag_etag_id = IPV4UC_EXT__VNTAG_ETAG_PROFILE_IDf;
        mem = BCM_XGS3_L3_MEM(unit, v4);
    }

    /* Assign entry-key buf based on table being used */
    BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_key,
                              l3v4_key,
                              l3v4_ext_key,
                              l3v6_key,
                              l3v6_ext_key);

    /* Assign entry buf based on table being used */
    BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_entry,
                              l3v4_entry,
                              l3v4_ext_entry,
                              l3v6_entry,
                              l3v6_ext_entry);

    /* Prepare lookup key. */
    BCM_IF_ERROR_RETURN
    (_bcm_th_l3_ext_ent_init(unit, mem_ext, l3cfg, buf_key));

    /* Perform lookup hw. */
    rv = soc_mem_generic_lookup(unit, mem_ext, MEM_BLOCK_ANY,
                                _BCM_TD2_L3_MEM_BANKS_ALL,
                                buf_key, buf_entry, &l3cfg->l3c_hw_index);
    if (BCM_SUCCESS(rv)) {
        *macda_oui_profile_index = soc_mem_field32_get(unit, mem_ext, buf_entry,
                                                       macda_oui_id);
        *vntag_etag_profile_index = soc_mem_field32_get(unit, mem_ext, buf_entry,
                                                        vntag_etag_id);
        rv = soc_profile_mem_ref_count_get(unit, 
                                           _bcm_th_macda_oui_profile[unit],
                                           *macda_oui_profile_index,
                                           ref_count);
        return rv;
    } else if (BCM_E_NOT_FOUND == rv) {
        /* Check if entry was added in regular L3_ENTRY table view */

        /* Assign entry-key buf based on table being used */
        BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem, buf_key,
                                  l3v4_key,
                                  l3v4_ext_key,
                                  l3v6_key,
                                  l3v6_ext_key);

        /* Assign entry buf based on table being used */
        BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry,
                                  l3v4_entry,
                                  l3v4_ext_entry,
                                  l3v6_entry,
                                  l3v6_ext_entry);
        
        /* Prepare lookup key. */
        BCM_IF_ERROR_RETURN(_bcm_th_l3_ent_init(unit, mem, l3cfg, buf_key));

        /* Perform lookup */
        rv = soc_mem_generic_lookup(unit, mem, MEM_BLOCK_ANY,
                                    _BCM_TD2_L3_MEM_BANKS_ALL,
                                    buf_key, buf_entry, &l3cfg->l3c_hw_index);
        return rv;
    }

    return rv;
   
}

/*
 * Function:
 *      _bcm_th_l3_ecmp_across_layers_move
 * Purpose:
 *      Move ecmp entries (which have no flags - BCM_L3_ECMP_UNDERLAY/OVERLAY)
 *      across two layers
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      threshold - (IN) Move threshold.
 *      direction - (IN) Move direction (0: L2 -> L1, 1: L1 -> L2).
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_th_l3_ecmp_across_layers_move(int unit, int threshold, int direction)
{
    int rv = BCM_E_NONE;
    int idx_min = 0;
    int idx_max = 0;
    int i = 0;
    int idx_increment = 0;
    int max_paths = 0;
    int base_ptr = 0;
    int boundary = 0;
    int old_ecmp_tbl_idx_min = 0;
    int old_ecmp_tbl_idx_max = 0;
    int move_width = 0;
    _bcm_l3_tbl_op_t data;
    _bcm_l3_tbl_t *ecmp_group_tbl_ptr = NULL;
    _bcm_l3_tbl_t *ecmp_tbl_ptr = NULL;
    ecmp_count_entry_t *ecmp_group_entry = NULL;
    uint8 *ecmp_group_buf = NULL;
    soc_defragment_member_op_t member_op;
    soc_defragment_group_op_t group_op;

    ecmp_group_tbl_ptr   = BCM_XGS3_L3_TBL_PTR(unit, ecmp_grp);
    ecmp_tbl_ptr         = BCM_XGS3_L3_TBL_PTR(unit, ecmp);
    old_ecmp_tbl_idx_max = ecmp_tbl_ptr->idx_max;
    old_ecmp_tbl_idx_min = ecmp_tbl_ptr->idx_min;
    if (direction) {
        /* direction: L1 -> L2, try to get free index in L2 */
        ecmp_tbl_ptr->idx_max = soc_mem_index_count(unit, L3_ECMPm) - 1;
        ecmp_tbl_ptr->idx_min = soc_mem_index_count(unit, L3_ECMPm) / 2;
    } else {
        /* direction: L2 -> L1, try to get free index in L1 */
        ecmp_tbl_ptr->idx_max = soc_mem_index_count(unit, L3_ECMPm) / 2 - 1;
        ecmp_tbl_ptr->idx_min = soc_mem_index_min(unit, L3_ECMPm);
    }

    sal_memset(&data, 0, sizeof(_bcm_l3_tbl_op_t));
    data.tbl_ptr     = ecmp_tbl_ptr;
    data.oper_flags  = _BCM_L3_SHR_TABLE_TRAVERSE_CONTROL;
    data.entry_index = -1;

    member_op.member_copy = _bcm_tr2_l3_ecmp_member_copy;
    member_op.member_clear = _bcm_tr2_l3_ecmp_member_clear;
    group_op.group_base_ptr_update = _bcm_tr2_l3_ecmp_group_base_ptr_update;

    boundary = soc_mem_index_count(unit, L3_ECMPm) / 2;

    /*
     * Max_Ecmp_Group  : TH 2K   TH2 4K
     * Max_Ecmp_Member : TH 16K  TH2 32K
     *
     * Combination_1 for ecmp_group_flags with 0 :
     *                   Group Index  [0 ~ Max_Ecmp_Group / 2 - 1]
     *                   Member Index [0 ~ Max_Ecmp_Member - 1]
     *
     * Combination_2 for ecmp_group_flags with 0 :
     *                   Group Index  [Max_Ecmp_Group / 2 ~ Max_Ecmp_Group - 1]
     *                   Member Index [Max_Ecmp_Member /2 ~ Max_Ecmp_Member - 1]
     *
     * So only can move ecmp members of the groups [0 ~ Max_Ecmp_Group / 2 - 1]
     * across layers
     */
    idx_min = soc_mem_index_min(unit, L3_ECMP_COUNTm);
    idx_max = soc_mem_index_count(unit, L3_ECMP_COUNTm) / 2 - 1;

    ecmp_group_buf =
        soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, L3_ECMP_COUNTm),
                      "L3_ECMP_COUNT buffer");
    if (NULL == ecmp_group_buf) {
        goto clean_up;
    }

    rv = soc_mem_read_range(unit, L3_ECMP_COUNTm, MEM_BLOCK_ANY,
                            idx_min, idx_max, ecmp_group_buf);
    if (SOC_FAILURE(rv)) {
        goto clean_up;
    }

    idx_increment = BCM_XGS3_L3_MAX_ECMP_MODE(unit) ?
                        _BCM_SINGLE_WIDE : _BCM_DOUBLE_WIDE;

    for (i = idx_min; i <= idx_max; i += idx_increment) {
        if (soc_feature(unit, soc_feature_vp_lag)) {
            /* Skip VP LAG entries */
            int is_vp_lag, is_used, has_member;
            rv = bcm_td2_vp_lag_status_get(
                     unit, i, &is_vp_lag, &is_used, &has_member);
            if (BCM_FAILURE(rv)) {
                goto clean_up;
            }
            if (is_vp_lag) {
                continue;
            }
        }

        if (!BCM_XGS3_L3_ENT_REF_CNT(ecmp_group_tbl_ptr, i)) {
            /* Skip unused entries */
            continue;
        }

        if (BCM_XGS3_L3_ECMP_GROUP_FLAGS_ISSET(unit, i, BCM_L3_ECMP_OVERLAY) ||
            BCM_XGS3_L3_ECMP_GROUP_FLAGS_ISSET(unit, i, BCM_L3_ECMP_UNDERLAY)) {
            /* Skip entries which have BCM_L3_ECMP_OVERLAY/OVERLAY flags */
            continue;
        }

        ecmp_group_entry = soc_mem_table_idx_to_pointer(unit, L3_ECMP_COUNTm,
                                                        ecmp_count_entry_t *,
                                                        ecmp_group_buf, i);
        base_ptr =
            soc_L3_ECMP_COUNTm_field32_get(unit, ecmp_group_entry, BASE_PTRf);
        rv = bcm_xgs3_l3_egress_ecmp_max_paths_get(
                 unit, i + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit), NULL, &max_paths);
        if (BCM_FAILURE(rv)) {
            goto clean_up;
        }

        if (((1 == direction) && (base_ptr < boundary)) ||
            ((0 == direction) && (base_ptr >= boundary))) {

            data.width = max_paths;
            rv = _bcm_xgs3_tbl_free_idx_get(unit, &data);
            if (BCM_SUCCESS(rv)) {
                rv = soc_defragment_block_move(unit, base_ptr,
                                               data.entry_index, max_paths,
                                               &member_op, i, &group_op);
                if (BCM_FAILURE(rv)) {
                    goto clean_up;
                }

                move_width += max_paths;
                if (move_width >= threshold) {
                    break;
                }
            }
        }
    }

    rv = BCM_E_NONE;

clean_up:

    if (NULL != ecmp_group_buf) {
        soc_cm_sfree(unit, ecmp_group_buf);
    }

    ecmp_tbl_ptr->idx_max = old_ecmp_tbl_idx_max;
    ecmp_tbl_ptr->idx_min = old_ecmp_tbl_idx_min;

    return rv;
}

/*
 * Function:
 *      bcm_th_l3_extended_required_scache_size_get
 * Purpose:
 *      Get required NIV module scache size for Level 2 Warm Boot in Tomahawk
 *      L3_ENTRY extended view
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      size - (OUT) Required scache size.
 */
void
bcm_th_l3_extended_required_scache_size_get(int unit, int *size)
{

    if (BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
        /* Sync the ref_count of EGR_MACDA_OUI_PROFILEm */
        *size += SOC_MEM_SIZE(unit, EGR_MACDA_OUI_PROFILEm) * sizeof(uint32);
    
        /* Sync the ref_count of EGR_VNTAG_ETAG_PROFILEm */
        *size += SOC_MEM_SIZE(unit, EGR_VNTAG_ETAG_PROFILEm) * sizeof(uint32);
    }
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      soc_profile_th_mem_ref_count_get
 * Purpose:
 *      Get the reference count of the cached entry at the specified index.
 *
 * Parameters:
 *      unit        - (IN) Unit
 *      profile     - (IN) Pointer to profile memory structure
 *      index0      - (IN) Base index to the entries in HW for table 0
 *      ref_count   - (OUT) Reference count
 * Returns:
 *      SOC_E_XXX
 */
static soc_error_t
soc_profile_th_mem_ref_count_get(int unit,
                                 soc_profile_mem_t *profile,
                                 int index0,
                                 uint32 *ref_count)
{
    soc_profile_mem_table_t *table;
    int base0;

    if (profile == NULL || ref_count == NULL) {
        return SOC_E_PARAM;
    }

    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    table = &profile->tables[0];
    if (index0 < table->index_min || index0 > table->index_max) {
        return SOC_E_PARAM;
    }

    base0 = index0 - table->index_min;

    *ref_count = table->entries[base0].ref_count;

    return SOC_E_NONE;
}

/*
 * Function:
 *      bcm_th_l3_extended_view_sync
 * Purpose:
 *      Record persistent info into the scache for the TH NIV software module 
 *      for L3_ENTRY extended view.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 *      offset - (IN) Scache offset
 * Returns:
 *      BCM_E_xxx
 */
bcm_error_t
bcm_th_l3_extended_view_sync(int unit, uint8 **scache_ptr, int offset)
{
    int rv = BCM_E_NONE;
    int i;
    uint32 ref_count;
    SOC_SCACHE_DUMP_DECL;

    if (!BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
        return rv;
    }
    
    if ((scache_ptr == NULL) || (*scache_ptr == NULL)) {
        return BCM_E_PARAM;
    }
    SOC_SCACHE_DUMP_START(unit, bcm_module_name(unit, BCM_MODULE_L3),
                          *scache_ptr - offset);
    SOC_SCACHE_DUMP_DATA_BEGIN("egr_macda_oui_profile_idx;ref_count");
    /* Record EGR_MACDA_OUI_PROFILEm ref_count in scache */
    for (i = 0; i < SOC_MEM_SIZE(unit, EGR_MACDA_OUI_PROFILEm); i++) {
        /* Generic profile library has a routine which takes ref_count as
         * int. However, MACDA_OUI_PROFILE table can have ref_count exceeding
         * int max value. Below routine takes ref_count as uint32 for TH
         */
        rv = soc_profile_th_mem_ref_count_get(unit,
                                              _bcm_th_macda_oui_profile[unit],
                                              i,
                                              &ref_count);
        if (!(rv == SOC_E_NONE)) {
            SOC_SCACHE_DUMP_STOP();
            return rv;
        }
        
        sal_memcpy((*scache_ptr), &ref_count, sizeof(uint32));
        SOC_SCACHE_DUMP_DATA_TVd((*scache_ptr), i, uint32);
        (*scache_ptr) += sizeof(ref_count);
    }
    SOC_SCACHE_DUMP_DATA_END();

    SOC_SCACHE_DUMP_DATA_BEGIN("egr_vntag_etag_profile_idx;ref_count");
    /* Record EGR_VNTAG_ETAG_PROFILEm ref_count in scache */
    for (i = 0; i < SOC_MEM_SIZE(unit, EGR_VNTAG_ETAG_PROFILEm); i++) {
        rv = soc_profile_th_mem_ref_count_get(unit,
                                              _bcm_th_vntag_etag_profile[unit],
                                              i,
                                              &ref_count);
        if (!(rv == SOC_E_NONE)) {
            SOC_SCACHE_DUMP_STOP();
            return rv;
        }
        
        sal_memcpy((*scache_ptr), &ref_count, sizeof(uint32));
        SOC_SCACHE_DUMP_DATA_TVd((*scache_ptr), i, uint32);
        (*scache_ptr) += sizeof(ref_count);
    }
    SOC_SCACHE_DUMP_DATA_END();

    SOC_SCACHE_DUMP_STOP();
    return rv;
}

/*
 * Function:
 *      bcm_th_l3_extended_view_reinit
 * Purpose:
 *      Warm boot recovery for the TH NIV software module for L3_ENTRY 
 *      extended view
 * Parameters:
 *      unit - Device Number
 *      defaul_ver - Warm boot default version
 * Returns:
 *      BCM_E_XXX
 */
bcm_error_t
bcm_th_l3_extended_view_reinit(int unit, uint8 **scache_ptr, int offset)
{
    int rv = BCM_E_NONE;
    soc_profile_mem_t *profile_mem;
    int i, j, size;
    uint32 ref_count;
    SOC_SCACHE_DUMP_DECL;

    if (!BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
        return rv;
    }

    if ((scache_ptr == NULL) || (*scache_ptr == NULL)) {
        return BCM_E_PARAM;
    }

    SOC_SCACHE_DUMP_START(unit, bcm_module_name(unit, BCM_MODULE_L3),
                          *scache_ptr - offset);
    /* Update EGR_MACDA_OUI_PROFILE memory profile reference counter */
    SOC_SCACHE_DUMP_DATA_BEGIN("egr_macda_oui_profile_idx;ref_count");
    profile_mem = _bcm_th_macda_oui_profile[unit];
    size = SOC_MEM_SIZE(unit, EGR_MACDA_OUI_PROFILEm);
    for (i = 0; i < size; i++) {
        sal_memcpy(&ref_count, (*scache_ptr), sizeof(uint32));
        SOC_SCACHE_DUMP_DATA_TVd((*scache_ptr), i, uint32);
        (*scache_ptr) += sizeof(ref_count);
        for (j = 0; j < ref_count; j++) {
            rv = soc_profile_mem_reference(unit, profile_mem, i, 1);
            if (BCM_FAILURE(rv)) {
                SOC_SCACHE_DUMP_STOP();
                return rv;
            }
        }
    }
    SOC_SCACHE_DUMP_DATA_END();

    /* Update EGR_VNTAG_ETAG_PROFILE memory profile reference counter */
    SOC_SCACHE_DUMP_DATA_BEGIN("egr_vntag_etag_profile_idx;ref_count");
    profile_mem = _bcm_th_vntag_etag_profile[unit];
    size = SOC_MEM_SIZE(unit, EGR_VNTAG_ETAG_PROFILEm);
    for (i = 0; i < size; i++) {
        sal_memcpy(&ref_count, (*scache_ptr), sizeof(uint32));
        SOC_SCACHE_DUMP_DATA_TVd((*scache_ptr), i, uint32);
        (*scache_ptr) += sizeof(ref_count);
        for (j = 0; j < ref_count; j++) {
            rv = soc_profile_mem_reference(unit, profile_mem, i, 1);
            if (BCM_FAILURE(rv)) {
                SOC_SCACHE_DUMP_STOP();
                return rv;
            }
        }
    }
    SOC_SCACHE_DUMP_DATA_END();

    SOC_SCACHE_DUMP_STOP();

    return rv;
}
#endif

#endif


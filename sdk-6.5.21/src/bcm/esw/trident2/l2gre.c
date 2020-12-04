/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    l2gre.c
 * Purpose: TD2 L2GRE enhancements
 */
#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/hash.h>
#include <soc/debug.h>
#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/l2gre.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/common/multicast.h>

#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/esw/cancun.h>
#endif

#define L2GRE_INFO(_unit_)   (_bcm_tr3_l2gre_bk_info[_unit_])

/*
 * Function:
 *        _bcm_td2_l2gre_match_vxlate_entry_set
 * Purpose:
 *       Set L2GRE Match Vxlate Entry
 * Parameters:
 *  IN :  Unit
 *  IN :  l2gre_port
 *  IN :  vlan_xlate_entry_t
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2_l2gre_match_vxlate_entry_set(int unit, bcm_l2gre_port_t *l2gre_port, uint32 *vent)
{
    int rv = BCM_E_UNAVAIL;
    int index;
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    bcm_trunk_t trunk_id = -1;
    int    gport_id = -1;
    soc_mem_t mem;
    uint32 rvent[SOC_MAX_MEM_WORDS];
    int vp = _SHR_GPORT_L2GRE_PORT_ID_GET(l2gre_port->l2gre_port_id);
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info = L2GRE_INFO(unit);


#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;

        soc_mem_field32_set(unit, mem, vent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    } else
#endif
    {
        mem = VLAN_XLATEm;
        soc_mem_field32_set(unit, mem, vent, VALIDf, 0x1);
    }

    if (l2gre_info->match_key[vp].flags != _BCM_L2GRE_PORT_MATCH_TYPE_SHARED) {
       BCM_IF_ERROR_RETURN(_bcm_tr3_l2gre_port_resolve(unit, l2gre_port->l2gre_port_id, -1, &mod_out,
                    &port_out, &trunk_id, &gport_id));
    } else {
       BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, l2gre_port->match_port, &mod_out,
                    &port_out, &trunk_id, &gport_id));
    }

    if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
        soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
        soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf, trunk_id);
    } else {
        soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf, port_out);
    }

    soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
    soc_mem_field32_set(unit, mem, vent, XLATE__MPLS_ACTIONf, 0x1); /* SVP */
    soc_mem_field32_set(unit, mem, vent, XLATE__SOURCE_VPf, vp);
    soc_mem_field32_set(unit, mem, vent, XLATE__DISABLE_VLAN_CHECKSf, 1);
    if (l2gre_port->flags & BCM_L2GRE_PORT_ENABLE_VLAN_CHECKS) {
        soc_mem_field32_set(unit, mem, vent, XLATE__VLAN_ACTION_VALIDf, 1);
    }


    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, vent, &rvent, 0);
    if (rv == BCM_E_NONE) {
         BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, vent));
    } else if (rv == BCM_E_NOT_FOUND) {
        if (l2gre_port->flags & BCM_VXLAN_PORT_REPLACE) {
            return BCM_E_NOT_FOUND;
        }
        BCM_IF_ERROR_RETURN(soc_mem_insert(unit, mem, MEM_BLOCK_ALL, vent));
    } else {
        return rv;
    }

	if (l2gre_info->match_key[vp].flags != _BCM_L2GRE_PORT_MATCH_TYPE_SHARED) {
        /* Update software state */
        if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
            l2gre_info->match_key[vp].trunk_id = trunk_id;
            l2gre_info->match_key[vp].modid = -1;
        } else {
            l2gre_info->match_key[vp].port = port_out;
            l2gre_info->match_key[vp].modid = mod_out;
            l2gre_info->match_key[vp].trunk_id = -1;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2_l2gre_match_add
 * Purpose:
 *      Assign match criteria of an L2GRE port
 * Parameters:
 *      unit    - (IN) Device Number
 *      l2gre_port - (IN) l2gre port information
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_td2_l2gre_match_add(int unit, bcm_l2gre_port_t *l2gre_port, int vp, bcm_vpn_t vpn)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info = L2GRE_INFO(unit);
    int rv = BCM_E_NONE;
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    bcm_trunk_t trunk_id = -1;
    int    src_trk_idx=0;  /*Source Trunk table index.*/
    int    gport_id=-1;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    _bcm_vp_info_t vp_info;
#endif
	soc_mem_t mem;
    uint32 vent[SOC_MAX_MEM_WORDS] = {0};

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        mem = VLAN_XLATEm;
    }
    if (l2gre_port->criteria == BCM_L2GRE_PORT_MATCH_SHARE) {
        /* there is no criteria to be configurated for shared vp,
           only updating software state to indicate this is a shared vp */
        l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_SHARED;
        return BCM_E_NONE;
    }
    if (l2gre_info->match_key[vp].flags != _BCM_L2GRE_PORT_MATCH_TYPE_SHARED) {
       rv = _bcm_tr3_l2gre_port_resolve(unit, l2gre_port->l2gre_port_id, -1, &mod_out,
                    &port_out, &trunk_id, &gport_id);
    } else {
       rv = _bcm_esw_gport_resolve(unit, l2gre_port->match_port, &mod_out,
                    &port_out, &trunk_id, &gport_id);
    }
    BCM_IF_ERROR_RETURN(rv);

    if (l2gre_port->criteria == BCM_L2GRE_PORT_MATCH_PORT_VLAN ) {
        if (!BCM_VLAN_VALID(l2gre_port->match_vlan)) {
             return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_OVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
            soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_OVID);
        }
#endif
        soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf, l2gre_port->match_vlan);
        if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf, trunk_id);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf, mod_out);
            soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf, port_out);
        }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vp_sharing)) {
            BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
            if (vp_info.flags & _BCM_VP_INFO_SHARED_PORT) {
                int vfi = 0;
                _BCM_L2GRE_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
                soc_mem_field32_set(unit, mem, vent, XLATE__VFIf, vfi);
            }
        }
#endif

        BCM_IF_ERROR_RETURN(
            _bcm_td2_l2gre_match_vxlate_entry_set(unit, l2gre_port, vent));
        if (l2gre_info->match_key[vp].flags != _BCM_L2GRE_PORT_MATCH_TYPE_SHARED) {
            /* Update software state */
            l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_VLAN;
            l2gre_info->match_key[vp].match_vlan = l2gre_port->match_vlan;
            if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
                l2gre_info->match_key[vp].trunk_id = trunk_id;
                l2gre_info->match_key[vp].modid = -1;
            } else {
                l2gre_info->match_key[vp].port = port_out;
                l2gre_info->match_key[vp].modid = mod_out;
                l2gre_info->match_key[vp].trunk_id = -1;
            }
        }
        if (!(l2gre_port->flags & BCM_L2GRE_PORT_REPLACE)) {
              bcm_tr3_l2gre_port_match_count_adjust(unit, vp, 1);
        }
    } else if (l2gre_port->criteria ==
                            BCM_L2GRE_PORT_MATCH_PORT_INNER_VLAN) {
        if (!BCM_VLAN_VALID(l2gre_port->match_inner_vlan)) {
            return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
            soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID);
        }
#endif
        soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf, l2gre_port->match_inner_vlan);
        if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf, trunk_id);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf, mod_out);
            soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf, port_out);
        }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vp_sharing)) {
            BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
            if (vp_info.flags & _BCM_VP_INFO_SHARED_PORT) {
                int vfi = 0;
                _BCM_L2GRE_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
                soc_mem_field32_set(unit, mem, vent, XLATE__VFIf, vfi);
            }
        }
#endif
        BCM_IF_ERROR_RETURN(
            _bcm_td2_l2gre_match_vxlate_entry_set(unit, l2gre_port, vent));
        if (l2gre_info->match_key[vp].flags != _BCM_L2GRE_PORT_MATCH_TYPE_SHARED) {
            l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_INNER_VLAN;
            l2gre_info->match_key[vp].match_inner_vlan = l2gre_port->match_inner_vlan;
            if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
                l2gre_info->match_key[vp].trunk_id = trunk_id;
                l2gre_info->match_key[vp].modid = -1;
            } else {
                l2gre_info->match_key[vp].port = port_out;
                l2gre_info->match_key[vp].modid = mod_out;
                l2gre_info->match_key[vp].trunk_id = -1;
            }
		}
        if (!(l2gre_port->flags & BCM_L2GRE_PORT_REPLACE)) {
             bcm_tr3_l2gre_port_match_count_adjust(unit, vp, 1);
        }
    } else if (l2gre_port->criteria ==
                            BCM_L2GRE_PORT_MATCH_PORT_VLAN_STACKED) {
        if (!BCM_VLAN_VALID(l2gre_port->match_vlan) ||
               !BCM_VLAN_VALID(l2gre_port->match_inner_vlan)) {
             return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
            soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
}
#endif
        soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf, l2gre_port->match_vlan);
        soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf, l2gre_port->match_inner_vlan);
        if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
           soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
           soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf, trunk_id);
        } else {
           soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf, mod_out);
           soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf, port_out);
        }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
	if (soc_feature(unit, soc_feature_vp_sharing)) {
            BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
            if (vp_info.flags & _BCM_VP_INFO_SHARED_PORT) {
                int vfi = 0;
                _BCM_L2GRE_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
                soc_mem_field32_set(unit, mem, vent, XLATE__VFIf, vfi);
            }
        }
#endif
        BCM_IF_ERROR_RETURN(
            _bcm_td2_l2gre_match_vxlate_entry_set(unit, l2gre_port, vent));
        if (l2gre_info->match_key[vp].flags != _BCM_L2GRE_PORT_MATCH_TYPE_SHARED) {
            l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_STACKED;
            l2gre_info->match_key[vp].match_vlan = l2gre_port->match_vlan;
            l2gre_info->match_key[vp].match_inner_vlan = l2gre_port->match_inner_vlan;
            if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
                l2gre_info->match_key[vp].trunk_id = trunk_id;
                l2gre_info->match_key[vp].modid = -1;
            } else {
                l2gre_info->match_key[vp].port = port_out;
                l2gre_info->match_key[vp].modid = mod_out;
                l2gre_info->match_key[vp].trunk_id = -1;
            }
        }
        if (!(l2gre_port->flags & BCM_L2GRE_PORT_REPLACE)) {
            bcm_tr3_l2gre_port_match_count_adjust(unit, vp, 1);
        }
    } else if (l2gre_port->criteria ==
                                      BCM_L2GRE_PORT_MATCH_VLAN_PRI) {
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_PRI_CFI);
        /* match_vlan : Bits 12-15 contains VLAN_PRI + CFI, vlan=BCM_E_NONE */
        soc_mem_field32_set(unit, mem, vent, OTAGf, l2gre_port->match_vlan);
        if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
             soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
             soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf, trunk_id);
        } else {
             soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf, mod_out);
             soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf, port_out);
        }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
		if (soc_feature(unit, soc_feature_vp_sharing)) {
            BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
            if (vp_info.flags & _BCM_VP_INFO_SHARED_PORT) {
                int vfi = 0;
                _BCM_L2GRE_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
                soc_mem_field32_set(unit, mem, vent, XLATE__VFIf, vfi);
            }
        }
#endif
        BCM_IF_ERROR_RETURN(
            _bcm_td2_l2gre_match_vxlate_entry_set(unit, l2gre_port, vent));
        if (l2gre_info->match_key[vp].flags != _BCM_L2GRE_PORT_MATCH_TYPE_SHARED) {
            l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_PRI;
            l2gre_info->match_key[vp].match_vlan = l2gre_port->match_vlan;
            if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
                l2gre_info->match_key[vp].trunk_id = trunk_id;
                l2gre_info->match_key[vp].modid = -1;
            } else {
                l2gre_info->match_key[vp].port = port_out;
                l2gre_info->match_key[vp].modid = mod_out;
                l2gre_info->match_key[vp].trunk_id = -1;
            }
        }
        if (!(l2gre_port->flags & BCM_L2GRE_PORT_REPLACE)) {
             bcm_tr3_l2gre_port_match_count_adjust(unit, vp, 1);
        }
    }else if (l2gre_port->criteria == BCM_L2GRE_PORT_MATCH_PORT) {
        if (l2gre_info->match_key[vp].flags ==
                     _BCM_L2GRE_PORT_MATCH_TYPE_SHARED) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
             rv = bcm_tr3_l2gre_match_trunk_add(unit, trunk_id, vp);
             if (rv >= 0) {
                   l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_TRUNK;
                   l2gre_info->match_key[vp].trunk_id = trunk_id;
             }
             BCM_IF_ERROR_RETURN(rv);
        } else {
            int is_local;

            BCM_IF_ERROR_RETURN
                ( _bcm_esw_modid_is_local(unit, mod_out, &is_local));

            /* Get index to source trunk map table */
            BCM_IF_ERROR_RETURN(
                   _bcm_esw_src_mod_port_table_index_get(unit, mod_out,
                     port_out, &src_trk_idx));

            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                        src_trk_idx, SOURCE_VPf, vp);
            BCM_IF_ERROR_RETURN(rv);
            /* Enable SVP Mode */
            if (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm, SVP_VALIDf) ) {
                rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                        src_trk_idx, SVP_VALIDf, 0x1);
                BCM_IF_ERROR_RETURN(rv);
            }
            if (is_local) {
                BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit,
                        l2gre_port->match_port, _BCM_CPU_TABS_NONE,
                        PORT_OPERATIONf, 0x1)); /* L2_SVP */

                /* Set TAG_ACTION_PROFILE_PTR */
                rv = bcm_tr3_l2gre_port_untagged_profile_set(unit, port_out);
                BCM_IF_ERROR_RETURN(rv);
            }

            l2gre_info->match_key[vp].flags = _BCM_L2GRE_PORT_MATCH_TYPE_PORT;
            l2gre_info->match_key[vp].index = src_trk_idx;
        }
    }else if (l2gre_port->criteria == BCM_L2GRE_PORT_MATCH_VPNID) {
        BCM_IF_ERROR_RETURN(
            bcm_tr3_l2gre_match_tunnel_entry_set(unit, vp, l2gre_port));
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_l2gre_match_delete
 * Purpose:
 *      Delete match criteria of an L2GRE port
 * Parameters:
 *      unit    - (IN) Device Number
 *      l2gre_port - (IN) L2GRE port information
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_td2_l2gre_match_delete(int unit,  int vp, bcm_l2gre_port_t *l2gre_port)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info = L2GRE_INFO(unit);
    int rv=BCM_E_NONE;
    bcm_trunk_t trunk_id;
    int    mod_id_idx=0;   /* Module_Id */
    soc_mem_t mem;
    uint32 vent[SOC_MAX_MEM_WORDS] = {0};
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    int    gport_id =-1;
#endif
    int    src_trk_idx = 0;   /*Source Trunk table index.*/
    int    port = 0;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;

        soc_mem_field32_set(unit, mem, vent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    } else
#endif
    {
        mem = VLAN_XLATEm;

        soc_mem_field32_set(unit, mem, vent, VALIDf, 1);
    }

    /* Get module id for unit. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &mod_id_idx));

    if  (l2gre_info->match_key[vp].flags == _BCM_L2GRE_PORT_MATCH_TYPE_VLAN) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_OVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (mem == VLAN_XLATE_1_DOUBLEm) {
            soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                TR_VLXLT_HASH_KEY_TYPE_OVID);
        }
#endif
        soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf,
                        l2gre_info->match_key[vp].match_vlan);
        if (l2gre_info->match_key[vp].modid != -1) {
            soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf,
                                        l2gre_info->match_key[vp].modid);
            soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf,
                                        l2gre_info->match_key[vp].port);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf,
                                    l2gre_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, vent);
        BCM_IF_ERROR_RETURN(rv);

        (void) bcm_tr3_l2gre_match_clear (unit, vp);
         bcm_tr3_l2gre_port_match_count_adjust(unit, vp, -1);
    } else if  (l2gre_info->match_key[vp].flags ==
                     _BCM_L2GRE_PORT_MATCH_TYPE_INNER_VLAN) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, VLXLT_HASH_KEY_TYPE_IVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (mem == VLAN_XLATE_1_DOUBLEm) {
            soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                VLXLT_HASH_KEY_TYPE_IVID);
        }
#endif
        soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf,
                    l2gre_info->match_key[vp].match_inner_vlan);
        if (l2gre_info->match_key[vp].modid != -1) {
            soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf,
                                        l2gre_info->match_key[vp].modid);
            soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf,
                                        l2gre_info->match_key[vp].port);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf,
                                l2gre_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);

        (void) bcm_tr3_l2gre_match_clear (unit, vp);
        bcm_tr3_l2gre_port_match_count_adjust(unit, vp, -1);
    } else if (l2gre_info->match_key[vp].flags ==
                    _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_STACKED) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (mem == VLAN_XLATE_1_DOUBLEm) {
            soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
        }
#endif
        soc_mem_field32_set(unit, mem, vent,
            XLATE__OVIDf, l2gre_info->match_key[vp].match_vlan);
        soc_mem_field32_set(unit, mem, vent,
            XLATE__IVIDf, l2gre_info->match_key[vp].match_inner_vlan);
        if (l2gre_info->match_key[vp].modid != -1) {
            soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf,
                                        l2gre_info->match_key[vp].modid);
            soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf,
                                        l2gre_info->match_key[vp].port);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf,
                                l2gre_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, vent);
        BCM_IF_ERROR_RETURN(rv);

        (void) bcm_tr3_l2gre_match_clear (unit, vp);
        bcm_tr3_l2gre_port_match_count_adjust(unit, vp, -1);
    } else if	(l2gre_info->match_key[vp].flags ==
                                               _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_PRI) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_PRI_CFI);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (mem == VLAN_XLATE_1_DOUBLEm) {
            soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                TR_VLXLT_HASH_KEY_TYPE_PRI_CFI);
        }
#endif

        soc_mem_field32_set(unit, mem, vent, XLATE__OTAGf,
                               l2gre_info->match_key[vp].match_vlan);
        if (l2gre_info->match_key[vp].modid != -1) {
            soc_mem_field32_set(unit, mem, vent,
                XLATE__MODULE_IDf, l2gre_info->match_key[vp].modid);
            soc_mem_field32_set(unit, mem, vent,
                XLATE__PORT_NUMf, l2gre_info->match_key[vp].port);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf,
                                  l2gre_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, vent);
        BCM_IF_ERROR_RETURN(rv);

        (void) bcm_tr3_l2gre_match_clear (unit, vp);
        bcm_tr3_l2gre_port_match_count_adjust(unit, vp, -1);
    }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    else if (l2gre_info->match_key[vp].flags ==
                    _BCM_L2GRE_PORT_MATCH_TYPE_SHARED) {
        bcm_trunk_t trunk_id = -1;
        if (NULL == l2gre_port) {
            return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);

        if (BCM_L2GRE_PORT_MATCH_PORT_VLAN == l2gre_port->criteria) {
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf,
                                TR_VLXLT_HASH_KEY_TYPE_OVID);
            soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf,
                                l2gre_port->match_vlan);
        } else if (BCM_L2GRE_PORT_MATCH_PORT_VLAN_STACKED == l2gre_port->criteria) {
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf,
                                TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (mem == VLAN_XLATE_1_DOUBLEm) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                    TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
            }
#endif
            soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf,
                                l2gre_port->match_vlan);
            soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf,
                                l2gre_port->match_inner_vlan);
        } else if (BCM_L2GRE_PORT_MATCH_PORT_INNER_VLAN == l2gre_port->criteria) {
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf,
                                TR_VLXLT_HASH_KEY_TYPE_IVID);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (mem == VLAN_XLATE_1_DOUBLEm) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                    TR_VLXLT_HASH_KEY_TYPE_IVID);
            }
#endif
            soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf,
                                l2gre_port->match_inner_vlan);
        } else if (BCM_L2GRE_PORT_MATCH_VLAN_PRI == l2gre_port->criteria) {
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf,
                                TR_VLXLT_HASH_KEY_TYPE_PRI_CFI);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (mem == VLAN_XLATE_1_DOUBLEm) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                    TR_VLXLT_HASH_KEY_TYPE_PRI_CFI);
            }
#endif
            soc_mem_field32_set(unit, mem, vent, XLATE__OTAGf,
                                l2gre_port->match_vlan);
        } else {
            return BCM_E_UNAVAIL;
        }

        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, l2gre_port->match_port, &mod_out,
                        &port_out, &trunk_id, &gport_id));

        if (BCM_GPORT_IS_TRUNK(l2gre_port->match_port)) {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf, trunk_id);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf, mod_out);
            soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf, port_out);
        }

        rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);

        bcm_tr3_l2gre_port_match_count_adjust(unit, vp, -1);
    }
#endif
    else if  (l2gre_info->match_key[vp].flags ==
                    _BCM_L2GRE_PORT_MATCH_TYPE_PORT) {
        int is_local;

        src_trk_idx = l2gre_info->match_key[vp].index;
        rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                  src_trk_idx, SOURCE_VPf, 0);
        BCM_IF_ERROR_RETURN(rv);
        /* Disable SVP Mode */
        if (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm, SVP_VALIDf) ) {
               rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                       src_trk_idx, SVP_VALIDf, 0x0);
               BCM_IF_ERROR_RETURN(rv);
        }

        BCM_IF_ERROR_RETURN(_bcm_esw_src_modid_port_get( unit, src_trk_idx,
                                                          &mod_id_idx, &port));
        BCM_IF_ERROR_RETURN
            ( _bcm_esw_modid_is_local(unit, mod_id_idx, &is_local));

        if (is_local) {
           BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit,
                port, _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0x0));

           /* Reset TAG_ACTION_PROFILE_PTR */
           rv = bcm_tr3_l2gre_port_untagged_profile_reset(unit, port);
           BCM_IF_ERROR_RETURN(rv);
        }
        (void) bcm_tr3_l2gre_match_clear (unit, vp);
    } else if (l2gre_info->match_key[vp].flags ==
                  _BCM_L2GRE_PORT_MATCH_TYPE_TRUNK) {
         trunk_id = l2gre_info->match_key[vp].trunk_id;
         rv = bcm_tr3_l2gre_match_trunk_delete(unit, trunk_id, vp);
         BCM_IF_ERROR_RETURN(rv);

        (void) bcm_tr3_l2gre_match_clear (unit, vp);
    } else if (l2gre_info->match_key[vp].flags ==
                  _BCM_L2GRE_PORT_MATCH_TYPE_VPNID) {
        rv = bcm_tr3_l2gre_match_tunnel_entry_reset(unit, vp);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td2_l2gre_match_get
 * Purpose:
 *      Obtain match information of an l2gre port
 * Parameters:
 *      unit    - (IN) Device Number
 *      l2gre_port - (OUT) l2gre port information
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_td2_l2gre_match_get(int unit, bcm_l2gre_port_t *l2gre_port, int vp)
{
    _bcm_tr3_l2gre_bookkeeping_t *l2gre_info = L2GRE_INFO(unit);
    bcm_module_t mod_in=0, mod_out=0;
    bcm_port_t port_in=0, port_out=0;
    bcm_trunk_t trunk_id=0;
    int    src_trk_idx=0;    /*Source Trunk table index.*/
    int    tunnel_idx = -1;
    int    index;
    soc_mem_t mem;
    uint32 vent[SOC_MAX_MEM_WORDS] = {0};

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;

        soc_mem_field32_set(unit, mem, vent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    } else
#endif
    {
        mem = VLAN_XLATEm;

        soc_mem_field32_set(unit, mem, vent, VALIDf, 1);
    }
    soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);

    if  (l2gre_info->match_key[vp].flags & _BCM_L2GRE_PORT_MATCH_TYPE_VLAN) {
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_OVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (mem == VLAN_XLATE_1_DOUBLEm) {
            soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                TR_VLXLT_HASH_KEY_TYPE_OVID);
        }
#endif

        l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_PORT_VLAN;
        l2gre_port->match_vlan = l2gre_info->match_key[vp].match_vlan;
        soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf, l2gre_port->match_vlan);
    } else if (l2gre_info->match_key[vp].flags &
                 _BCM_L2GRE_PORT_MATCH_TYPE_INNER_VLAN) {
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (mem == VLAN_XLATE_1_DOUBLEm) {
            soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                TR_VLXLT_HASH_KEY_TYPE_IVID);
        }
#endif

        l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_PORT_INNER_VLAN;
        l2gre_port->match_inner_vlan = l2gre_info->match_key[vp].match_inner_vlan;
        soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf, l2gre_port->match_inner_vlan);
    }else if (l2gre_info->match_key[vp].flags &
                   _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_STACKED) {
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (mem == VLAN_XLATE_1_DOUBLEm) {
            soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
        }
#endif

        l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_PORT_VLAN_STACKED;
        l2gre_port->match_vlan = l2gre_info->match_key[vp].match_vlan;
        l2gre_port->match_inner_vlan = l2gre_info->match_key[vp].match_inner_vlan;
        soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf, l2gre_port->match_vlan);
        soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf, l2gre_port->match_inner_vlan);
    } else if  (l2gre_info->match_key[vp].flags &  _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_PRI) {
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, TR_VLXLT_HASH_KEY_TYPE_PRI_CFI);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (mem == VLAN_XLATE_1_DOUBLEm) {
            soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                TR_VLXLT_HASH_KEY_TYPE_PRI_CFI);
        }
#endif

        l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_VLAN_PRI;
        l2gre_port->match_vlan = l2gre_info->match_key[vp].match_vlan;
        soc_mem_field32_set(unit, mem, vent, OTAGf, l2gre_port->match_vlan);
    } else if (l2gre_info->match_key[vp].flags &
                  _BCM_L2GRE_PORT_MATCH_TYPE_PORT) {

        src_trk_idx = l2gre_info->match_key[vp].index;
        BCM_IF_ERROR_RETURN(_bcm_esw_src_modid_port_get( unit, src_trk_idx,
                                                         &mod_in, &port_in));

        l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_PORT;
        BCM_IF_ERROR_RETURN(_bcm_esw_stk_modmap_map(unit,
            BCM_STK_MODMAP_GET, mod_in, port_in, &mod_out, &port_out));
        BCM_GPORT_MODPORT_SET(l2gre_port->match_port, mod_out, port_out);
    }else if (l2gre_info->match_key[vp].flags &
                 _BCM_L2GRE_PORT_MATCH_TYPE_TRUNK) {

        trunk_id = l2gre_info->match_key[vp].trunk_id;
        l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_PORT;
        BCM_GPORT_TRUNK_SET(l2gre_port->match_port, trunk_id);
    } else if ((l2gre_info->match_key[vp].flags &
                          _BCM_L2GRE_PORT_MATCH_TYPE_VPNID)) {
        l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_VPNID;

        tunnel_idx = l2gre_info->match_key[vp].match_tunnel_index;
        BCM_GPORT_TUNNEL_ID_SET(l2gre_port->match_tunnel_id, tunnel_idx);
    }else if (l2gre_info->match_key[vp].flags & _BCM_L2GRE_PORT_MATCH_TYPE_SHARED) {
        l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_SHARE;
    } else {
        l2gre_port->criteria = BCM_L2GRE_PORT_MATCH_NONE;
    }

    if (l2gre_port->criteria == BCM_L2GRE_PORT_MATCH_PORT_VLAN ||
        l2gre_port->criteria == BCM_L2GRE_PORT_MATCH_PORT_INNER_VLAN ||
        l2gre_port->criteria == BCM_L2GRE_PORT_MATCH_PORT_VLAN_STACKED ||
        l2gre_port->criteria == BCM_L2GRE_PORT_MATCH_VLAN_PRI) {

        if (l2gre_info->match_key[vp].trunk_id != -1) {
            trunk_id = l2gre_info->match_key[vp].trunk_id;
            BCM_GPORT_TRUNK_SET(l2gre_port->match_port, trunk_id);
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf, trunk_id);
        } else {
            port_in = l2gre_info->match_key[vp].port;
            mod_in = l2gre_info->match_key[vp].modid;
            BCM_IF_ERROR_RETURN(_bcm_esw_stk_modmap_map(unit,
                BCM_STK_MODMAP_GET, mod_in, port_in, &mod_out, &port_out));
            BCM_GPORT_MODPORT_SET(l2gre_port->match_port, mod_out, port_out);
            soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf, mod_in);
            soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf, port_in);
        }

        BCM_IF_ERROR_RETURN(
            soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, vent, vent, 0));

        if (soc_mem_field32_get(unit, mem, vent, XLATE__VLAN_ACTION_VALIDf)) {
            l2gre_port->flags |= BCM_L2GRE_PORT_ENABLE_VLAN_CHECKS;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_l2gre_eline_vp_configure
 * Purpose:
 *      Configure L2GRE ELINE virtual port
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2_l2gre_eline_vp_configure (int unit, int vfi_index, int active_vp,
                  source_vp_entry_t *svp, int tpid_enable, bcm_l2gre_port_t  *l2gre_port)
{
    int rv = BCM_E_NONE;
    int network_group=0;
    source_vp_2_entry_t svp_2_entry;

    /* Set SOURCE_VP */
    soc_SOURCE_VPm_field32_set(unit, svp, CLASS_IDf,
                                l2gre_port->if_class);

    network_group = l2gre_port->network_group_id;
    rv = _bcm_validate_splithorizon_network_group(unit,
            l2gre_port->flags & BCM_L2GRE_PORT_NETWORK, &network_group);
    BCM_IF_ERROR_RETURN(rv);

    /*Default Split Horizon Group Id
     *0 - For Access Port;1 - For Network Port*/
    if (l2gre_port->flags & BCM_L2GRE_PORT_NETWORK) {
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_GROUPf,
                        network_group);
        } else {
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_PORTf, 1);
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
        } else
#endif
        {
            soc_SOURCE_VPm_field32_set(unit, svp, TPID_SOURCEf, _BCM_L2GRE_TPID_SVP_BASED);
        }
    } else {
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_GROUPf,
                        network_group);
        } else {
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_PORTf, 0);
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
        } else
#endif
        {
            soc_SOURCE_VPm_field32_set(unit, svp, TPID_SOURCEf, _BCM_L2GRE_TPID_SGLP_BASED);
            /*  Configure IPARS Parser to signal to IMPLS Parser - Applicable only for HG2 PPD2 packets
                  to reconstruct TPID state based on HG header information - Only for VXLAN Access ports */
            sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
            soc_SOURCE_VP_2m_field32_set(unit, &svp_2_entry, PARSE_USING_SGLP_TPIDf, 1);
            BCM_IF_ERROR_RETURN(WRITE_SOURCE_VP_2m(unit,
                    MEM_BLOCK_ALL, active_vp, &svp_2_entry));
        }
    }

    if (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_TAGGED) {
         soc_SOURCE_VPm_field32_set(unit, svp, SD_TAG_MODEf, 1);
#if defined(BCM_TRIDENT3_SUPPORT)
         if (SOC_IS_TRIDENT3X(unit)) {
             uint32 svp1[SOC_MAX_MEM_WORDS];

             sal_memset(svp1, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

             soc_SVP_ATTRS_1m_field_set(unit, &svp1, TPID_ENABLEf, (uint32 *)&tpid_enable);
             BCM_IF_ERROR_RETURN(WRITE_SVP_ATTRS_1m(unit, MEM_BLOCK_ALL, active_vp, &svp1));
         } else
#endif
         {
             soc_SOURCE_VPm_field32_set(unit, svp, TPID_ENABLEf, tpid_enable);
         }
    } else {
         soc_SOURCE_VPm_field32_set(unit, svp, SD_TAG_MODEf, 0);
    }

    soc_SOURCE_VPm_field32_set(unit, svp, DISABLE_VLAN_CHECKSf, 1);
    soc_SOURCE_VPm_field32_set(unit, svp,
                           ENTRY_TYPEf, _BCM_L2GRE_SOURCE_VP_TYPE_VFI); /* VFI Type */
    soc_SOURCE_VPm_field32_set(unit, svp, VFIf, vfi_index);

    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, active_vp, svp);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        if ((vfi_index != _BCM_L2GRE_VFI_INVALID) &&
            (l2gre_port->flags & BCM_L2GRE_PORT_NETWORK)) {
            if ( BCM_E_NONE != soc_cancun_app_dest_entry_set (unit, SOURCE_VPm,
                active_vp, ENTRY_TYPEf, CANCUN_APP__SOURCE_VP__No_Control_Word,
                _BCM_L2GRE_SOURCE_VP_TYPE_VFI)) {
                LOG_ERROR(BSL_LS_BCM_L2GRE,
                   (BSL_META_U(unit,"SOURCE_VP cancun app cfg error\n")));
            }
        }
    }
#endif

    return rv;
}

/*
 * Function:
 *      _bcm_td2_l2gre_elan_vp_configure
 * Purpose:
 *      Configure L2GRE ELAN virtual port
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2_l2gre_elan_vp_configure (int unit, int vfi_index, int vp,
                  source_vp_entry_t *svp, int tpid_enable, bcm_l2gre_port_t  *l2gre_port)
{
    int rv = BCM_E_NONE;
    int network_group=0;
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;
    source_vp_2_entry_t svp_2_entry;

    /* Set SOURCE_VP */
    soc_SOURCE_VPm_field32_set(unit, svp, CLASS_IDf,
                         l2gre_port->if_class);

    if (vfi_index == _BCM_L2GRE_VFI_INVALID) {
        soc_SOURCE_VPm_field32_set(unit, svp,
                                   ENTRY_TYPEf, _BCM_L2GRE_SOURCE_VP_TYPE_INVALID); /* INVALID */
    } else {
        /* Initialize VP parameters */
        soc_SOURCE_VPm_field32_set(unit, svp,
                                   ENTRY_TYPEf, _BCM_L2GRE_SOURCE_VP_TYPE_VFI); /* VFI Type */
    }


    /*Default Split Horizon Group Id
     *0 - For Access Port;1 - For Network Port*/
    network_group = l2gre_port->network_group_id;
    rv = _bcm_validate_splithorizon_network_group(unit,
            l2gre_port->flags & BCM_L2GRE_PORT_NETWORK, &network_group);
    BCM_IF_ERROR_RETURN(rv);

    if (l2gre_port->flags & BCM_L2GRE_PORT_NETWORK) {
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_GROUPf,
                        network_group);
        } else {
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_PORTf, 1);
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
        } else
#endif
        {
            soc_SOURCE_VPm_field32_set(unit, svp, TPID_SOURCEf, _BCM_L2GRE_TPID_SVP_BASED);
        }
    } else {
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_GROUPf,
                         network_group);
        } else {
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_PORTf, 0);
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
        } else
#endif
        {
            soc_SOURCE_VPm_field32_set(unit, svp, TPID_SOURCEf, _BCM_L2GRE_TPID_SGLP_BASED);
            /*  Configure IPARS Parser to signal to IMPLS Parser - Applicable only for HG2 PPD2 packets
                 to reconstruct TPID state based on HG header information - Only for VXLAN Access ports */
            sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
            soc_SOURCE_VP_2m_field32_set(unit, &svp_2_entry, PARSE_USING_SGLP_TPIDf, 1);
            BCM_IF_ERROR_RETURN(WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, vp, &svp_2_entry));
        }
    }

    if (vfi_index == _BCM_L2GRE_VFI_INVALID) {
        soc_SOURCE_VPm_field32_set(unit, svp, VFIf, 0);
    } else {
        soc_SOURCE_VPm_field32_set(unit, svp, VFIf, vfi_index);
    }

    if (l2gre_port->flags & BCM_L2GRE_PORT_SERVICE_TAGGED) {
         soc_SOURCE_VPm_field32_set(unit, svp, SD_TAG_MODEf, 1);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            uint32 svp1[SOC_MAX_MEM_WORDS];

            sal_memset(svp1, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

            soc_SVP_ATTRS_1m_field_set(unit, &svp1, TPID_ENABLEf, (uint32 *)&tpid_enable);
            BCM_IF_ERROR_RETURN(WRITE_SVP_ATTRS_1m(unit, MEM_BLOCK_ALL, vp, &svp1));
        } else
#endif
        {
            soc_SOURCE_VPm_field32_set(unit, svp, TPID_ENABLEf, tpid_enable);
        }
    } else {
         soc_SOURCE_VPm_field32_set(unit, svp, SD_TAG_MODEf, 0);
    }
    rv = _bcm_vp_default_cml_mode_get (unit,
                       &cml_default_enable, &cml_default_new,
                       &cml_default_move);
    if (rv < 0) {
         return rv;
    }
    if (cml_default_enable) {
        /* Set the CML to default values */
        soc_SOURCE_VPm_field32_set(unit, svp, CML_FLAGS_NEWf, cml_default_new);
        soc_SOURCE_VPm_field32_set(unit, svp, CML_FLAGS_MOVEf, cml_default_move);
    } else {
        /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
        soc_SOURCE_VPm_field32_set(unit, svp, CML_FLAGS_NEWf, 0x8);
        soc_SOURCE_VPm_field32_set(unit, svp, CML_FLAGS_MOVEf, 0x8);
    }
    if (soc_mem_field_valid(unit, SOURCE_VPm, DISABLE_VLAN_CHECKSf)) {
        if (!soc_feature(unit, soc_feature_vlan_vfi_membership)) {
            soc_SOURCE_VPm_field32_set(unit, svp, DISABLE_VLAN_CHECKSf, 1);
        }
    }

    BCM_IF_ERROR_RETURN
           (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, svp));

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        if ((vfi_index != _BCM_L2GRE_VFI_INVALID) &&
            (l2gre_port->flags & BCM_L2GRE_PORT_NETWORK)) {
            if ( BCM_E_NONE != soc_cancun_app_dest_entry_set (unit, SOURCE_VPm,
                vp, ENTRY_TYPEf, CANCUN_APP__SOURCE_VP__No_Control_Word,
                _BCM_L2GRE_SOURCE_VP_TYPE_VFI)) {
                LOG_ERROR(BSL_LS_BCM_L2GRE,
                   (BSL_META_U(unit,"SOURCE_VP cancun app cfg error\n")));
            }
        }
    }
#endif

    return rv;
}
/*
 * Function:
 *      _bcm_td2_l2gre_match_vp_replace
 * Purpose:
 *      Replace VP value in L2GRE VP's match entry.
 * Parameters:
 *      unit   - (IN) SOC unit number.
 *      vp     - (IN) L2GRE VP whose match entry is being replaced.
 *      new_vp - (IN) New VP value.
 *      old_vp - (OUT) Old VP value.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_td2_l2gre_match_vp_replace(int unit, int vp, int new_vp,
                                int *old_vp)
{
    int                rv = BCM_E_NONE;
    soc_mem_t          mem;
    int                key_type = bcmVlanTranslateKeyInvalid;
    uint32             svp_valid, tunnel_index, tunnel_sip;
    int                entry_index;
    int                port, my_modid, stm_idx, i;
    int                local_port[SOC_MAX_NUM_PORTS];
    int                local_port_count = 0;
    uint32 vent[SOC_MAX_MEM_WORDS], old_vent[SOC_MAX_MEM_WORDS];
    source_trunk_map_table_entry_t stm_entry;
    uint32 ment[SOC_MAX_MEM_WORDS];
    _bcm_l2gre_match_port_info_t *mkey = NULL;

    mkey = &(L2GRE_INFO(unit)->match_key[vp]);

    switch (mkey->flags) {
        case _BCM_L2GRE_PORT_MATCH_TYPE_VLAN:
            if (key_type == bcmVlanTranslateKeyInvalid) {
                key_type = bcmVlanTranslateKeyPortOuter;
            }
            /* Fall through */
        case _BCM_L2GRE_PORT_MATCH_TYPE_INNER_VLAN:
            if (key_type == bcmVlanTranslateKeyInvalid) {
                key_type = bcmVlanTranslateKeyPortInner;
            }
            /* Fall through */
        case _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_STACKED:
            if (key_type == bcmVlanTranslateKeyInvalid) {
                key_type = bcmVlanTranslateKeyPortDouble;
            }
            /* Fall through */
        case _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_PRI:
            if (key_type == bcmVlanTranslateKeyInvalid) {
                key_type = bcmVlanTranslateKeyPortOuterPri;
            }

            mem = VLAN_XLATEm;

            if (mkey->modid != -1) {
                BCM_GPORT_MODPORT_SET(port, mkey->modid, mkey->port);
            } else {
                BCM_GPORT_TRUNK_SET(port, mkey->trunk_id);
            }
            /* Construct lookup key */
            sal_memset(vent, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            rv = _bcm_trx_vlan_translate_entry_assemble(
                     unit, &vent, port, key_type, mkey->match_inner_vlan,
                     mkey->match_vlan);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            soc_mem_lock(unit, mem);
            /* Lookup existing entry */
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL,
                                &entry_index, vent, old_vent, 0);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, mem);
                return rv;
            }
            /* Replace entry's VP value */
            *old_vp = soc_mem_field32_get(unit, mem, old_vent, SOURCE_VPf);
            soc_mem_field32_set(unit, mem, old_vent, SOURCE_VPf, new_vp);

            /* Insert new entry */
            rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, old_vent);
            if (rv == SOC_E_EXISTS) {
                rv = BCM_E_NONE;
            } else {
                soc_mem_unlock(unit, mem);
                return BCM_E_INTERNAL;
            }
            soc_mem_unlock(unit, mem);
            break;
        case _BCM_L2GRE_PORT_MATCH_TYPE_PORT:
            mem = SOURCE_TRUNK_MAP_TABLEm;

            soc_mem_lock(unit, mem);

            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, mkey->index,
                              &stm_entry);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, mem);
                return rv;
            }
            /* Resolve SVP Mode */
            if (soc_mem_field_valid(unit, mem, SVP_VALIDf)) {
                svp_valid = soc_mem_field32_get(unit, mem, &stm_entry,
                                                SVP_VALIDf);
                if (svp_valid == 0) {
                    soc_mem_unlock(unit, mem);
                    return BCM_E_INTERNAL;
                }
            }
            *old_vp = soc_mem_field32_get(unit, mem, &stm_entry, SOURCE_VPf);
            rv = soc_mem_field32_modify(unit, mem, mkey->index, SOURCE_VPf,
                                        new_vp);
            soc_mem_unlock(unit, mem);
            break;
        case _BCM_L2GRE_PORT_MATCH_TYPE_TRUNK:
            mem = SOURCE_TRUNK_MAP_TABLEm;

            /* Get module id for unit. */
            rv = bcm_esw_stk_my_modid_get(unit, &my_modid);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            rv = _bcm_esw_trunk_local_members_get(
                     unit, mkey->trunk_id, SOC_MAX_NUM_PORTS,
                     local_port, &local_port_count);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            soc_mem_lock(unit, mem);
            for (i = 0; i < local_port_count; i++) {
                /* Get index to source trunk map table */
                rv = _bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                                                           local_port[i],
                                                           &stm_idx);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, mem);
                    return rv;
                }

                rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, stm_idx,
                                  &stm_entry);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, mem);
                    return rv;
                }

                /* Resolve SVP Mode */
                if (soc_mem_field_valid(unit, mem, SVP_VALIDf)) {
                    svp_valid = soc_mem_field32_get(unit, mem, &stm_entry,
                                                    SVP_VALIDf);
                    if (svp_valid == 0) {
                        soc_mem_unlock(unit, mem);
                        return BCM_E_INTERNAL;
                    }
                }
                *old_vp = soc_mem_field32_get(unit, mem, &stm_entry,
                                              SOURCE_VPf);
                rv = soc_mem_field32_modify(unit, mem, stm_idx, SOURCE_VPf,
                                            new_vp);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, mem);
                    return rv;
                }
            }

            soc_mem_unlock(unit, mem);
            break;
        case _BCM_L2GRE_PORT_MATCH_TYPE_VPNID:
            tunnel_index = mkey->match_tunnel_index;
            tunnel_sip = L2GRE_INFO(unit)->l2gre_tunnel_term[tunnel_index].sip;

            sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            mem = MPLS_ENTRYm;
            soc_mem_lock(unit, mem);
            soc_mem_field32_set(unit, mem, &ment, VALIDf, 1);

            soc_mem_field32_set(unit, mem, &ment, L2GRE_SIP__SIPf, tunnel_sip);
            soc_mem_field32_set(unit, mem, &ment, KEY_TYPEf,
                                _BCM_L2GRE_KEY_TYPE_TUNNEL);
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &entry_index, &ment,
                                &ment, 0);

            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_PARAM;
            }

            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, mem);
                return rv;
            }

            /* Replace entry's VP value */
            *old_vp = soc_mem_field32_get(unit, mem, &ment, L2GRE_SIP__SVPf);
            soc_mem_field32_set(unit, mem, &ment, L2GRE_SIP__SVPf, new_vp);
            /* Insert new entry */
            rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, ment);

            soc_mem_unlock(unit, mem);

            if (rv == SOC_E_EXISTS) {
                rv = BCM_E_NONE;
            } else {
                return BCM_E_INTERNAL;
            }
            break;
        default:
            return BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_l2gre_port_source_vp_lag_set
 * Purpose:
 *      Set source VP LAG for a L2GRE virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) L2GRE virtual port GPORT ID.
 *      vp_lag_vp - (IN) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_l2gre_port_source_vp_lag_set(int unit, bcm_gport_t gport,
                                     int vp_lag_vp)
{
    int vp, old_vp;
    int rv = BCM_E_NONE;

    if (!BCM_GPORT_IS_L2GRE_PORT(gport)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_L2GRE_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
        return BCM_E_PARAM;
    }

    /* Set source VP LAG by replacing the SVP field in L2GRE VP's
     * match entry with the VP value representing the VP LAG.
     */
    rv = _bcm_td2_l2gre_match_vp_replace(unit, vp, vp_lag_vp, &old_vp);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    return rv;
}
/*
 * Function:
 *      bcm_td2_l2gre_port_source_vp_lag_clear
 * Purpose:
 *      Clear source VP LAG for a L2GRE virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) L2GRE virtual port GPORT ID.
 *      vp_lag_vp - (IN) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_l2gre_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
                                       int vp_lag_vp)
{
    int vp, old_vp;

    if (!BCM_GPORT_IS_L2GRE_PORT(gport)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_L2GRE_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
        return BCM_E_PARAM;
    }

    /* Clear source VP LAG by replacing the SVP field in L2GRE VP's
     * match entry with the VP value.
     */
    BCM_IF_ERROR_RETURN(
        _bcm_td2_l2gre_match_vp_replace(unit, vp, vp, &old_vp));

    /* Check that the old VP value matches the VP value representing
     * the VP LAG or has been restored.
     */
    if ((old_vp != vp_lag_vp) && (old_vp != vp)) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_l2gre_port_source_vp_lag_get
 * Purpose:
 *      Get source VP LAG for a L2GRE virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) L2GRE virtual port GPORT ID.
 *      vp_lag_vp - (OUT) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_l2gre_port_source_vp_lag_get(int unit, bcm_gport_t gport,
                                     int *vp_lag_vp)
{
    int                rv = BCM_E_NONE;
    soc_mem_t          mem;
    int                key_type = bcmVlanTranslateKeyInvalid;
    bcm_gport_t        vp;
    uint32             svp_valid, tunnel_index, tunnel_sip;
    int                entry_index;
    int                port, my_modid, stm_idx;
    int                local_port[SOC_MAX_NUM_PORTS];
    int                local_port_count = 0;
    uint32 vent[SOC_MAX_MEM_WORDS], old_vent[SOC_MAX_MEM_WORDS];
    uint32 ment[SOC_MAX_MEM_WORDS];
    source_trunk_map_table_entry_t stm_entry;
    _bcm_l2gre_match_port_info_t *mkey = NULL;


    if (!BCM_GPORT_IS_L2GRE_PORT(gport)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_L2GRE_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
        return BCM_E_PARAM;
    }

    mkey = &(L2GRE_INFO(unit)->match_key[vp]);

    switch (mkey->flags) {
        case _BCM_L2GRE_PORT_MATCH_TYPE_VLAN:
            if (key_type == bcmVlanTranslateKeyInvalid) {
                key_type = bcmVlanTranslateKeyPortOuter;
            }
            /* Fall through */
        case _BCM_L2GRE_PORT_MATCH_TYPE_INNER_VLAN:
            if (key_type == bcmVlanTranslateKeyInvalid) {
                key_type = bcmVlanTranslateKeyPortInner;
            }
            /* Fall through */
        case _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_STACKED:
            if (key_type == bcmVlanTranslateKeyInvalid) {
                key_type = bcmVlanTranslateKeyPortDouble;
            }
            /* Fall through */
        case _BCM_L2GRE_PORT_MATCH_TYPE_VLAN_PRI:
            if (key_type == bcmVlanTranslateKeyInvalid) {
                key_type = bcmVlanTranslateKeyPortOuterPri;
            }

            mem = VLAN_XLATEm;

            if (mkey->modid != -1) {
                BCM_GPORT_MODPORT_SET(port, mkey->modid, mkey->port);
            } else {
                BCM_GPORT_TRUNK_SET(port, mkey->trunk_id);
            }
            /* Construct lookup key */
            sal_memset(vent, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            rv = _bcm_trx_vlan_translate_entry_assemble(
                     unit, &vent, port, key_type, mkey->match_inner_vlan,
                     mkey->match_vlan);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            /* Lookup existing entry */
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &entry_index, vent,
                                old_vent, 0);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            /* Get VP value representing VP LAG */
            *vp_lag_vp = soc_mem_field32_get(unit, mem, old_vent, SOURCE_VPf);
            break;
        case _BCM_L2GRE_PORT_MATCH_TYPE_PORT:
            mem = SOURCE_TRUNK_MAP_TABLEm;

            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, mkey->index,
                              &stm_entry);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            /* Resolve SVP Mode */
            if (soc_mem_field_valid(unit, mem, SVP_VALIDf)) {
                svp_valid = soc_mem_field32_get(unit, mem, &stm_entry,
                                                SVP_VALIDf);
                if (svp_valid == 0) {
                    return BCM_E_INTERNAL;
                }
            }
            /* Get VP value representing VP LAG */
            *vp_lag_vp = soc_mem_field32_get(unit, mem, &stm_entry, SOURCE_VPf);
            break;
        case _BCM_L2GRE_PORT_MATCH_TYPE_TRUNK:
            mem = SOURCE_TRUNK_MAP_TABLEm;

            /* Get module id for unit. */
            rv = bcm_esw_stk_my_modid_get(unit, &my_modid);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            rv = _bcm_esw_trunk_local_members_get(
                     unit, mkey->trunk_id, SOC_MAX_NUM_PORTS, local_port,
                     &local_port_count);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            /* Get index to source trunk map table,
             * Only get it from the first member in LAG
             */
            rv = _bcm_esw_src_mod_port_table_index_get(
                     unit, my_modid, local_port[0], &stm_idx);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, stm_idx, &stm_entry);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            /* Resolve SVP Mode */
            if (soc_mem_field_valid(unit, mem, SVP_VALIDf)) {
                svp_valid = soc_mem_field32_get(unit, mem, &stm_entry,
                                                SVP_VALIDf);
                if (svp_valid == 0) {
                    return BCM_E_INTERNAL;
                }
            }
            /* Get VP value representing VP LAG */
            *vp_lag_vp = soc_mem_field32_get(unit, mem, &stm_entry, SOURCE_VPf);
            break;
        case _BCM_L2GRE_PORT_MATCH_TYPE_VPNID:
            tunnel_index = mkey->match_tunnel_index;
            tunnel_sip = L2GRE_INFO(unit)->l2gre_tunnel_term[tunnel_index].sip;

            sal_memset(ment, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            mem = MPLS_ENTRYm;
            soc_mem_lock(unit, mem);
            soc_mem_field32_set(unit, mem, &ment, VALIDf, 1);

            soc_mem_field32_set(unit, mem, &ment, L2GRE_SIP__SIPf, tunnel_sip);
            soc_mem_field32_set(unit, mem, &ment,
                KEY_TYPEf, _BCM_L2GRE_KEY_TYPE_TUNNEL);
            soc_mem_lock(unit, mem);
            rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &entry_index, &ment,
                                &ment, 0);
            soc_mem_unlock(unit, mem);
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_PARAM;
            }

            if (BCM_FAILURE(rv)) {
                return rv;
            }

            /* Get VP value representing VP LAG */
            *vp_lag_vp = soc_mem_field32_get(unit, mem, &ment, L2GRE_SIP__SVPf);
            break;
        default:
            return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, *vp_lag_vp, _bcmVpTypeVpLag)) {
        return BCM_E_INTERNAL;
    }

    return rv;
}
#else /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
typedef int bcm_esw_td2_l2gre_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

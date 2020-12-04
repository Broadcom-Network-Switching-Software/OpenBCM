/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    multicast.c
 * Purpose: Manages multicast functions
 */

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <soc/triumph.h>

#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/stack.h>
#ifdef BCM_TRIDENT_SUPPORT
#include <bcm_int/esw/trill.h>
#include <bcm_int/esw/trident.h>
#endif /* BCM_TRIDENT_SUPPORT */
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>

#if defined(BCM_TRIDENT_SUPPORT)
/*
 * Function:
 *      bcm_td_multicast_trill_group_update
 * Purpose:
 *      Update TRILL Tree ID of L3-IPMC group
 * Parameters:
 *      unit         - (IN) Unit number.
 *      ipmc_index (IN)
 *      trill_tree_id  (IN)
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_td_multicast_trill_group_update(int unit, int ipmc_index, uint8 trill_tree_id)
{
    int rv = BCM_E_NONE;
    egr_ipmc_entry_t egr_ipmc;

    soc_mem_lock(unit, EGR_IPMCm);

    rv = READ_EGR_IPMCm(unit, MEM_BLOCK_ALL, ipmc_index, &egr_ipmc);
    if (rv < 0 ) {
         soc_mem_unlock(unit, EGR_IPMCm);       
         return rv;
    }

    if(soc_mem_field_valid(unit, EGR_IPMCm, TRILL_TREE_PROFILE_PTRf)) {
         soc_EGR_IPMCm_field32_set(unit, &egr_ipmc, 
              TRILL_TREE_PROFILE_PTRf, trill_tree_id);
    }
    if(soc_mem_field_valid(unit, EGR_IPMCm, IPMC_GROUP_TYPEf)) {
         soc_EGR_IPMCm_field32_set(unit, &egr_ipmc, IPMC_GROUP_TYPEf, 0x1);
    }

    rv =  WRITE_EGR_IPMCm(unit, MEM_BLOCK_ALL, 
              ipmc_index, &egr_ipmc);
    soc_mem_unlock(unit, EGR_IPMCm);

    return rv;
}

/*
 * Function:
 *      bcm_td_multicast_trill_group_get
 * Purpose:
 *      Get L3-IPMC group for matching Trill Tree_id
 * Parameters:
 *      unit         - (IN) Unit number.
 *      ipmc_index  - (IN)
 *      trill_tree_id  - (OUT)
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_td_multicast_trill_group_get(int unit, int ipmc_index, uint8 *trill_tree_id)
{
    int rv = BCM_E_NONE;
    egr_ipmc_entry_t egr_ipmc;

    rv = READ_EGR_IPMCm(unit, MEM_BLOCK_ALL, ipmc_index, &egr_ipmc);
    if (rv < 0 ) {
         return rv;
    }

    if(soc_mem_field_valid(unit, EGR_IPMCm, IPMC_GROUP_TYPEf)) {
        if (soc_EGR_IPMCm_field32_get(unit, &egr_ipmc, IPMC_GROUP_TYPEf)) {
            if(soc_mem_field_valid(unit, EGR_IPMCm, TRILL_TREE_PROFILE_PTRf)) {
                *trill_tree_id = soc_EGR_IPMCm_field32_get(unit, &egr_ipmc, 
                                                   TRILL_TREE_PROFILE_PTRf);
                return BCM_E_NONE;
            }
        }
    }
    return BCM_E_NOT_FOUND;
}
#endif /* BCM_TRIDENT_SUPPORT */

/*
 * Function:
 *      bcm_multicast_vpls_encap_get
 * Purpose:
 *      Get the Encap ID for a MPLS port.
 * Parameters:
 *      unit         - (IN) Unit number.
 *      group        - (IN) Multicast group ID.
 *      port         - (IN) Physical port.
 *      mpls_port_id - (IN) MPLS port ID.
 *      encap_id     - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_tr2_multicast_vpls_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                bcm_gport_t mpls_port_id, bcm_if_t *encap_id)
{
    BCM_IF_ERROR_RETURN(
        bcm_tr_multicast_vpls_encap_get(unit, group, port, 
                                        mpls_port_id, encap_id));
    /* Triumph2 is able to do multicast replications over next hops and interfaces */
    *encap_id += BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_multicast_mim_encap_get
 * Purpose:
 *      Get the Encap ID for a MiM port.
 * Parameters:
 *      unit         - (IN) Unit number.
 *      group        - (IN) Multicast group ID.
 *      port         - (IN) Physical port.
 *      mim_port_id -  (IN) MIM port ID.
 *      encap_id     - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_tr2_multicast_mim_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                bcm_gport_t mim_port_id, bcm_if_t *encap_id)
{
    int vp;
    ing_dvp_table_entry_t dvp;

    if (!BCM_GPORT_IS_MIM_PORT(mim_port_id)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port_id); 
    if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
        return BCM_E_PARAM;
    }
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
            return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

    /* Next-hop index is used for multicast replication */
    *encap_id = (bcm_if_t) soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
    if (!SOC_IS_ENDURO(unit)) {
        *encap_id += BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_multicast_wlan_encap_get
 * Purpose:
 *      Get the Encap ID for a WLAN port.
 * Parameters:
 *      unit         - (IN) Unit number.
 *      group        - (IN) Multicast group ID.
 *      port         - (IN) Physical port.
 *      wlan_port_id - (IN) WLAN port ID.
 *      encap_id     - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_tr2_multicast_wlan_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                bcm_gport_t wlan_port_id, bcm_if_t *encap_id)
{
    int vp;
    ing_dvp_table_entry_t dvp;
    soc_mem_t wlan_svp_mem;

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        wlan_svp_mem = AXP_WRX_SVP_ASSIGNMENTm;
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        wlan_svp_mem = WLAN_SVP_TABLEm;
    }

    if (!BCM_GPORT_IS_WLAN_PORT(wlan_port_id)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port_id); 
    if (vp >= soc_mem_index_count(unit, wlan_svp_mem)) {
        return BCM_E_PARAM;
    }
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
            return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

    /* Next-hop index is used for multicast replication */
    *encap_id = (bcm_if_t) soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
    *encap_id += BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_multicast_subport_encap_get
 * Purpose:
 *      Get the Encap ID for a subport.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      group     - (IN) Multicast group ID.
 *      port      - (IN) Physical port.
 *      subport   - (IN) Subport ID.
 *      encap_id  - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_tr2_multicast_subport_encap_get(int unit, bcm_multicast_t group, 
                                    bcm_gport_t port,
                                    bcm_gport_t subport, bcm_if_t *encap_id)
{
    int vp, l3_idx,rv;
    ing_dvp_table_entry_t dvp;
    egr_l3_intf_entry_t l3_intf;

    if (!BCM_GPORT_IS_SUBPORT_PORT(subport)) {
        return BCM_E_PARAM;
    }

    l3_idx = BCM_GPORT_SUBPORT_PORT_GET(subport) & 0xfff;
    if (l3_idx >= BCM_XGS3_L3_IF_TBL_SIZE(unit)) { 
        return (BCM_E_PARAM);
    }

    rv = soc_mem_read(unit, EGR_L3_INTFm, MEM_BLOCK_ALL, l3_idx, &l3_intf);
    BCM_IF_ERROR_RETURN(rv);

    vp = soc_mem_field32_get(unit, EGR_L3_INTFm, &l3_intf, IVIDf);
    if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
        return BCM_E_PARAM;
    }
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeSubport)) {
            return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

    /* Next-hop index is used for multicast replication */
    *encap_id = (bcm_if_t) soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
    if (!SOC_IS_ENDURO(unit)) {
        *encap_id += BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr2_ipmc_remap_set
 * Purpose:
 *      Set a multicast binding.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      mc_from   - (IN) Multicast index from
 *      mc_to     - (IN) Multicast index to
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Mulitcast types are already validated as appropriate
 *      for IPMC
 */
int
bcm_tr2_ipmc_remap_set(int unit, bcm_multicast_t mc_from, int mc_to)
{
    int rv = BCM_E_UNAVAIL;
    int ipmc_from;
    int ipmc_to;
    int ipmc_idx_min;
    int ipmc_idx_max;
    bcm_multicast_t mcgroup;
    ipmc_remap_entry_t l3_ipmc_remap;

    ipmc_from = _BCM_MULTICAST_ID_GET(mc_from);
    ipmc_to = _BCM_MULTICAST_ID_GET(mc_to);
    ipmc_idx_min = soc_mem_index_min(unit, L3_IPMC_REMAPm);
    ipmc_idx_max = soc_mem_index_max(unit, L3_IPMC_REMAPm);

    /* Table index sanity check. */
    if ((ipmc_from < ipmc_idx_min) || (ipmc_from > ipmc_idx_max)) {
        return BCM_E_PARAM;
    }

    if ((ipmc_to < ipmc_idx_min) || (ipmc_to > ipmc_idx_max)) {
        return BCM_E_PARAM;
    }

    /* Sanity check the converted IPMC index to see if it really
       corresponds to a valid multicast group. Don't check if the
       multicast groups in and out are identical because it's possible
       that there's some interesting "cross group" programming going
       on. */
    rv = _bcm_tr_multicast_ipmc_group_type_get(unit, ipmc_from, &mcgroup);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_tr_multicast_ipmc_group_type_get(unit, ipmc_to, &mcgroup);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&l3_ipmc_remap, 0, sizeof(l3_ipmc_remap));

    soc_L3_IPMC_REMAPm_field32_set(unit, &l3_ipmc_remap, L3MC_INDEXf, ipmc_to);

    rv =  WRITE_L3_IPMC_REMAPm(unit, MEM_BLOCK_ALL, ipmc_from, &l3_ipmc_remap);

    return rv;
}

/*
 * Function:
 *      bcm_tr2_ipmc_remap_get
 * Purpose:
 *      Get a multicast binding.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      mc_from   - (IN) Multicast group from.
 *      mc_to     - (OUT) Pointer to multicast group to.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Mulitcast types are already validated as appropriate
 *      for IPMC
 */
int
bcm_tr2_ipmc_remap_get(int unit, bcm_multicast_t mc_from, int *mc_to)
{
    int rv = BCM_E_UNAVAIL;
    int ipmc_from;
    int ipmc_to;
    bcm_multicast_t mcgroup;
    ipmc_remap_entry_t l3_ipmc_remap;

    ipmc_from = _BCM_MULTICAST_ID_GET(mc_from);

    /* Table index sanity check. */
    if ((ipmc_from < soc_mem_index_min(unit, L3_IPMC_REMAPm)) ||
        (ipmc_from > soc_mem_index_max(unit, L3_IPMC_REMAPm))) {
        return BCM_E_PARAM;
    }

    /* validate that the multicast group is valid */
    rv = _bcm_tr_multicast_ipmc_group_type_get(unit, ipmc_from, &mcgroup);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&l3_ipmc_remap, 0, sizeof(l3_ipmc_remap));

    rv =  READ_L3_IPMC_REMAPm(unit, MEM_BLOCK_ALL, ipmc_from, &l3_ipmc_remap);

    if (SOC_SUCCESS(rv)) {
        ipmc_to =
            soc_L3_IPMC_REMAPm_field32_get(unit, &l3_ipmc_remap, L3MC_INDEXf);
        rv = _bcm_tr_multicast_ipmc_group_type_get(unit, ipmc_to, &mcgroup);
        if (BCM_SUCCESS(rv)) {
            *mc_to = (int)mcgroup;
        }
    }

    return rv;
}

#else  /* INCLUDE_L3 && BCM_TRIUMPH2_SUPPORT */
typedef int bcm_esw_triumph2_multicast_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_L3 && BCM_TRIUMPH2_SUPPORT */

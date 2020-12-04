/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <sal/core/libc.h>
#if defined(INCLUDE_L3)

#include <bcm/flow.h>
#include <bcm_int/esw/flow.h>
#include <bcm/error.h>
#include <soc/drv.h>
#include <bcm_int/esw/vpn.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/trident3.h>

/* Function:
 *      bcmi_esw_flow_vpn_is_valid
 * Purpose:
 *      Find if given FLOW VPN is Valid 
 * Parameters:
 *      unit     - Device Number
 *      vpn   - FLOW VPN
 * Returns:
 *      BCM_E_XXXX
 */
int
bcmi_esw_flow_vpn_is_valid( int unit, bcm_vpn_t vpn)
{
    bcm_vpn_t flow_vpn_min=0;
    int vfi_index=-1, num_vfi=0;

    num_vfi = soc_mem_index_count(unit, VFIm);

    /* Check for Valid vpn */
    _BCM_FLOW_VPN_SET(flow_vpn_min, _BCM_VPN_TYPE_VFI, 0);
    if ( vpn < flow_vpn_min || vpn > (flow_vpn_min+num_vfi-1) ) {
        return BCM_E_PARAM;
    }

    _BCM_FLOW_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI,  vpn);

    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeFlow)) {
        return BCM_E_NOT_FOUND;
    }
    return BCM_E_NONE;
}

 /* Function:
 *      bcmi_esw_flow_vpn_is_eline
 * Purpose:
 *      Find if given FLOW VPN is ELINE
 * Parameters:
 *      unit     - Device Number
 *      vpn      - FLOW VPN
 *      isEline  - Flag 
 * Returns:
 *      BCM_E_XXXX
 * Assumes:
 *      vpn is valid
 */

int
bcmi_esw_flow_vpn_is_eline( int unit, bcm_vpn_t vpn, uint8 *isEline)
{
    int vfi_index=-1;
    vfi_entry_t vfi_entry;

    BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_is_valid(unit, vpn));

    _BCM_FLOW_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI,  vpn);
    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    /* Set the returned VPN id */
    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        *isEline = 1;  /* ELINE */
    } else {
        *isEline = 0;  /* ELAN */
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_flow_vpn_create
 * Purpose:
 *      Create an L2 Flow VPN.  This is a service plane and is 
 *      independent of the connectivity protocol.
 *   Parameters:
 *      unit        - (IN)  Unit ID.
 *      info    - (IN/OUT)  VPN structure
 */
int bcmi_esw_flow_vpn_create(int unit, bcm_vpn_t *vpn,
                            bcm_flow_vpn_config_t *info)
{
    int rv = BCM_E_NONE;
    vfi_entry_t vfi_entry;
    int vfi_index=-1;
    int bc_group=0, umc_group=0, uuc_group=0;
    int bc_group_type=0, umc_group_type=0, uuc_group_type=0;
    uint8 vpn_alloc_flag=0;
    int proto_pkt_idx = 0;
    uint32 profile_idx = 0;

    if (!vpn) {
        return BCM_E_PARAM;
    }

    /*Allocate VFI*/
    if (info->flags & BCM_FLOW_VPN_REPLACE) {
        BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_is_valid(unit, *vpn));
        _BCM_FLOW_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, *vpn);
    } else if (info->flags & BCM_FLOW_VPN_WITH_ID) {
        rv = bcmi_esw_flow_vpn_is_valid(unit, *vpn);
        if (BCM_E_NONE == rv) {
            return BCM_E_EXISTS;
        } else if (BCM_E_NOT_FOUND != rv) {
            return rv;
        }
        _BCM_FLOW_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, *vpn);
        BCM_IF_ERROR_RETURN(_bcm_vfi_alloc_with_id(unit, VFIm, _bcmVfiTypeFlow, vfi_index));
        vpn_alloc_flag = 1;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_vfi_alloc(unit, VFIm, _bcmVfiTypeFlow, &vfi_index));
        _BCM_FLOW_VPN_SET(*vpn, _BCM_VPN_TYPE_VFI, vfi_index);
        vpn_alloc_flag = 1;
    }

    /*Initial and configure VFI*/
    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));
    if (info->flags & BCM_FLOW_VPN_ELINE) {
        soc_VFIm_field32_set(unit, &vfi_entry, PT2PT_ENf, 0x1);
    } else if (info->flags & BCM_FLOW_VPN_ELAN) {
        /* Check that the groups are valid. */
        bc_group_type = _BCM_MULTICAST_TYPE_GET(info->broadcast_group);
        bc_group = _BCM_MULTICAST_ID_GET(info->broadcast_group);
        umc_group_type = _BCM_MULTICAST_TYPE_GET(info->unknown_multicast_group);
        umc_group = _BCM_MULTICAST_ID_GET(info->unknown_multicast_group);
        uuc_group_type = _BCM_MULTICAST_TYPE_GET(info->unknown_unicast_group);
        uuc_group = _BCM_MULTICAST_ID_GET(info->unknown_unicast_group);

        if ((bc_group_type != _BCM_MULTICAST_TYPE_FLOW) ||
            (umc_group_type != _BCM_MULTICAST_TYPE_FLOW) ||
            (uuc_group_type != _BCM_MULTICAST_TYPE_FLOW) ||
            (bc_group >= soc_mem_index_count(unit, L3_IPMCm)) ||
            (umc_group >= soc_mem_index_count(unit, L3_IPMCm)) ||
            (uuc_group >= soc_mem_index_count(unit, L3_IPMCm))) {
            rv = BCM_E_PARAM;
            _BCM_FLOW_CLEANUP(rv)
        }

        /* Commit the entry to HW */
        soc_mem_field32_dest_set(unit, VFIm, &vfi_entry,
                UMC_DESTINATIONf, SOC_MEM_FIF_DEST_IPMC, umc_group);
        soc_mem_field32_dest_set(unit, VFIm, &vfi_entry,
                UUC_DESTINATIONf, SOC_MEM_FIF_DEST_IPMC, uuc_group);
        soc_mem_field32_dest_set(unit, VFIm, &vfi_entry,
                BC_DESTINATIONf, SOC_MEM_FIF_DEST_IPMC, bc_group);
    }
    rv = _bcm_td3_def_vfi_profile_add(unit,
            info->flags & BCM_FLOW_VPN_ELINE, &profile_idx);
    if (BCM_E_NONE == rv) {
        soc_VFIm_field32_set(unit, &vfi_entry, VFI_PROFILE_PTRf, profile_idx);
    }
    _BCM_FLOW_CLEANUP(rv)

    /* Configure protocol packet control */
    rv = _bcm_xgs3_protocol_packet_actions_validate(unit, &info->protocol_pkt);
    _BCM_FLOW_CLEANUP(rv)
    proto_pkt_idx = soc_VFIm_field32_get(unit, &vfi_entry, PROTOCOL_PKT_INDEXf);
    rv = _bcm_xgs3_protocol_pkt_ctrl_set(unit, proto_pkt_idx, &info->protocol_pkt, &proto_pkt_idx);
    _BCM_FLOW_CLEANUP(rv)
    soc_VFIm_field32_set(unit, &vfi_entry, PROTOCOL_PKT_INDEXf, proto_pkt_idx);

    /*Write VFI to ASIC*/
    rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry);
    _BCM_FLOW_CLEANUP(rv)

   return BCM_E_NONE;

cleanup:
    if (vpn_alloc_flag) {
        (void) _bcm_vfi_free(unit, _bcmVfiTypeFlow, vfi_index);
    }

    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_vpn_destroy
 * Purpose:
 *      Delete L2/L3 VPN
 *   Parameters:
 *      unit   - (IN)  Unit ID.
 *      vpn    - (IN)  VPN Id
 */
int bcmi_esw_flow_vpn_destroy(int unit, bcm_vpn_t vpn)
{
    int vfi = 0;
    vfi_entry_t vfi_entry;
    uint8 isEline = 0;
#if 0
    uint32 stat_counter_id;
    int num_ctr = 0; 
#endif
    int ref_count;
    int index;

    /* Get vfi index */
    BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_is_eline(unit, vpn, &isEline));
    if (isEline == 0x1 ) {
        _BCM_FLOW_VPN_GET(vfi, _BCM_FLOW_VPN_TYPE_ELINE, vpn);
    } else if (isEline == 0x0 ) {
        _BCM_FLOW_VPN_GET(vfi, _BCM_FLOW_VPN_TYPE_ELAN, vpn);
    }

    /* Delete all FLOW ports on this VPN */
    BCM_IF_ERROR_RETURN(bcmi_esw_flow_port_delete_all(unit, vpn));

#if 0
    /* TBD: Need to check on this piece of code */
    /* Check counters before delete vpn */
    if (bcm_esw_vxlan_stat_id_get(unit, BCM_GPORT_INVALID, vpn,
                   bcmVxlanOutPackets, &stat_counter_id) == BCM_E_NONE) { 
        num_ctr++;
    } 
    if (bcm_esw_vxlan_stat_id_get(unit, BCM_GPORT_INVALID, vpn,
                   bcmVxlanInPackets, &stat_counter_id) == BCM_E_NONE) {
        num_ctr++;
    }

    if (num_ctr != 0) {
        BCM_IF_ERROR_RETURN(
           bcm_esw_vxlan_stat_detach(unit, BCM_GPORT_INVALID, vpn));  
    }
#endif

    /* Delete protocol packet control */
    sal_memset(&vfi_entry, 0, sizeof(vfi_entry_t));
    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry));
    index = soc_VFIm_field32_get(unit, &vfi_entry, PROTOCOL_PKT_INDEXf);
    BCM_IF_ERROR_RETURN(
        _bcm_prot_pkt_ctrl_ref_count_get(unit, index, &ref_count));
    if (ref_count > 0) {
        BCM_IF_ERROR_RETURN(_bcm_prot_pkt_ctrl_delete(unit, index));
    }

    /* Reset VFI table */
    (void)_bcm_vfi_free(unit, _bcmVfiTypeFlow, vfi);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_flow_vpn_destroy_all
 * Purpose:
 *      Delete all VPN's
 *   Parameters:
 *      unit   - (IN)  Unit ID.
 */
int bcmi_esw_flow_vpn_destroy_all(int unit)
{
    int num_vfi = 0, idx = 0;
    bcm_vpn_t vpn = 0;

    /* Destroy all FLOW VPNs */
    num_vfi = soc_mem_index_count(unit, VFIm);
    for (idx = 0; idx < num_vfi; idx++) {
        if (_bcm_vfi_used_get(unit, idx, _bcmVfiTypeFlow)) {
            _BCM_FLOW_VPN_SET(vpn, _BCM_VPN_TYPE_VFI, idx);
            BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_destroy(unit, vpn));
        }
    }
    return BCM_E_NONE;

}

/*
 * Function:
 *      bcmi_esw_flow_vpn_get
 * Purpose:
 *      Get VPN properties
 *   Parameters:
 *      unit   - (IN)  Unit ID.
 *      vpn    - (IN)  VPN Id
 *      info    - (IN/OUT)  VPN structure
 */
int bcmi_esw_flow_vpn_get(int unit, bcm_vpn_t vpn,
                         bcm_flow_vpn_config_t *info)
{
    int rv = BCM_E_NONE;
    int vfi_index = -1;
    vfi_entry_t vfi_entry;
    uint8 isEline=0;
    int proto_pkt_inx;
    uint32 dt, dv;

    /*Get vfi table*/
    BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_is_eline(unit, vpn, &isEline));
    if (isEline == 0x1 ) {
        _BCM_FLOW_VPN_GET(vfi_index, _BCM_FLOW_VPN_TYPE_ELINE, vpn);
    } else if (isEline == 0x0 ) {
        _BCM_FLOW_VPN_GET(vfi_index, _BCM_FLOW_VPN_TYPE_ELAN, vpn);
    }
    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    /* Get info associated with vfi table */
    if (isEline) {
        info->flags =  BCM_FLOW_VPN_ELINE;
    } else {
         info->flags =  BCM_FLOW_VPN_ELAN;


         dv = soc_mem_field32_dest_get(unit, VFIm,
                 &vfi_entry, BC_DESTINATIONf, &dt);
         if (dt == SOC_MEM_FIF_DEST_IPMC) {
             _BCM_MULTICAST_GROUP_SET(info->broadcast_group,
                     _BCM_MULTICAST_TYPE_FLOW, dv);
         } else {
             return BCM_E_INTERNAL;
         }

         dv = soc_mem_field32_dest_get(unit, VFIm,
                 &vfi_entry, UUC_DESTINATIONf, &dt);
         if (dt == SOC_MEM_FIF_DEST_IPMC) {
             _BCM_MULTICAST_GROUP_SET(info->unknown_unicast_group,
                     _BCM_MULTICAST_TYPE_FLOW, dv);
         } else {
             return BCM_E_INTERNAL;
         }

         dv = soc_mem_field32_dest_get(unit, VFIm,
                 &vfi_entry, UMC_DESTINATIONf, &dt);
         if (dt == SOC_MEM_FIF_DEST_IPMC) {
             _BCM_MULTICAST_GROUP_SET(info->unknown_multicast_group,
                     _BCM_MULTICAST_TYPE_FLOW, dv);
         } else {
             return BCM_E_INTERNAL;
         }
    }
    /*Get protocol packet control*/
    proto_pkt_inx = soc_VFIm_field32_get(unit, &vfi_entry, PROTOCOL_PKT_INDEXf);
    BCM_IF_ERROR_RETURN(_bcm_xgs3_protocol_pkt_ctrl_get(unit,proto_pkt_inx, &info->protocol_pkt));

    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_vpn_traverse
 * Purpose:
 *      Traverse VPN's
 *   Parameters:
 *      unit      - (IN)  Unit ID.
 *      cb        - (IN)  callback function
 *      user_data -  (IN) User context data
 */
int bcmi_esw_flow_vpn_traverse(int unit,
                              bcm_flow_vpn_traverse_cb cb,
                              void *user_data)
{
    int idx, num_vfi;
    int vpn;
    bcm_flow_vpn_config_t info;

    if (cb == NULL) {
         return BCM_E_PARAM;
    }

    /* FLOW VPNs */
    num_vfi = soc_mem_index_count(unit, VFIm);
    for (idx = 0; idx < num_vfi; idx++) {
         if (_bcm_vfi_used_get(unit, idx, _bcmVfiTypeFlow)) {
              _BCM_FLOW_VPN_SET(vpn, _BCM_VPN_TYPE_VFI, idx);
              bcm_flow_vpn_config_t_init(&info);
              BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_get(unit, vpn, &info));
              BCM_IF_ERROR_RETURN(cb(unit, vpn, &info, user_data));
         }
    }
    return BCM_E_NONE;
}

 /* Function:
 *      bcmi_flow_vpnid_get
 * Purpose:
 *      Get L2-VPN instance for FLOW VPN
 * Parameters:
 *      unit     - Device Number
 *      vfi_index   - vfi_index
 *      vid     (OUT) VLAN Id (l2vpn id)

 * Returns:
 *      BCM_E_XXXX
 */
int
bcmi_esw_flow_vpnid_get(int unit, int vfi_index, bcm_vlan_t *vid)
{
    vfi_entry_t vfi_entry;

    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        _BCM_FLOW_VPN_SET(*vid, _BCM_FLOW_VPN_TYPE_ELINE, vfi_index);
    } else {
        _BCM_FLOW_VPN_SET(*vid, _BCM_FLOW_VPN_TYPE_ELAN, vfi_index);
    }
    return BCM_E_NONE;
}
#endif

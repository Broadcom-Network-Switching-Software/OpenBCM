/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        flexflow_tunnel.c
 * Purpose:     Flex flow Tunnel APIs.
 */

#include <shared/bsl.h>
#include <soc/defs.h>

#ifdef INCLUDE_L3

#include <assert.h>
#include <bcm/error.h>
#include <soc/error.h>
#include <bcm/types.h>
#include <soc/drv.h>
#include <soc/format.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/vpn.h>
#include <bcm_int/esw/flow.h>
#include <soc/esw/flow_db.h>
#include <bcm/flow.h>
#include <bcm_int/esw/vxlan.h>
#include <bcm_int/esw/l2gre.h>
#include <soc/esw/cancun.h>

#define _QUAD_ENTRY_MAX   (4 * SOC_MAX_MEM_WORDS)


#define TNL_SRV6_SID_MAX              3
#define TNL_SRV6_ACTION_DVP_ATTACH    1
#define TNL_SRV6_ACTION_DELETE        2
#define TNL_SRV6_ACTION_DVP_IDX_GET   3
#define TNL_SRV6_ACTION_TNL_ID_ATTACH 4
#define TNL_SRV6_ACTION_EGR_INTF_GET  5

 /*
  * Function:
  *      _bcm_flow_bud_loopback_termination_set
  * Purpose:
  *      Enable/Disable termination control for loopback port 
  *      for classic flow VXLAN/L2GRE BUD node multicast
  * Parameters:
  *   IN : unit
  *   IN : flow_handle
  *   IN : enable
  * Returns:
  *      BCM_E_XXX
  */

STATIC int
_bcm_flow_bud_loopback_termination_set(int unit, uint32 flow_handle, int enable)
{
    int i;
    int field_num = 1;

    soc_field_t lport_fields[] = {
        VXLAN_TERMINATION_ALLOWEDf,
    };
    uint32 lport_values[] = {
        0x1,  /* VXLAN_TERMINATION_ALLOWEDf */
    };

    if (flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
        lport_fields[0] = VXLAN_TERMINATION_ALLOWEDf;
    } else if (flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
        lport_fields[0] = L2GRE_TERMINATION_ALLOWEDf;
    } else {
        /* flex vxlan */
        lport_fields[0] = VXLAN_TERMINATION_ALLOWEDf;
    }

    lport_values[0] = enable ? 1: 0;

    /* update the default profile 0 referenced by STM table. 
     * Packet from loopback port will use pp port number as index to STM 
     */
    for (i = 0; i < field_num; i++) {
        SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, LPORT_TABm, 0,
                                lport_fields[i], lport_values[i]));
    }
    return BCM_E_NONE;
}

int
bcmi_flow_ip6_addr_is_valid(bcm_ip6_t addr)
{
    uint32 idx = 0;
    for (idx = 0; idx < 16; idx++) {
        if (addr[idx]) {
           return TRUE;
        }
    }
    return FALSE;
}

/*
 * Function:
 *      _bcm_flow_srv6_l3_tnl_object_ctrl
 * Purpose:
 *      perform various actions for the objects related to SRv6 L3 tunnel.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_X_XXX
 * NOTE:
 */

STATIC
int _bcm_flow_srv6_l3_tnl_object_ctrl(
         int unit,
         int action,
         int nh_idx,
         int *dvp,
         int tnl_idx,
         int *l3_oif)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    int rv;
    int data_type;
    uint32 action_set;
    uint32 nh_view_id;
    int nh2_idx;
    int dvp_vc_swap = 0;
    int flags;
    _bcm_flow_bookkeeping_t *flow_info;
    int next_ptr_type;
    int intf_id;

    mem = EGR_L3_NEXT_HOPm;
    flow_info = FLOW_INFO(unit);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, nh_idx, entry);
    BCM_IF_ERROR_RETURN(rv);

    data_type = soc_mem_field32_get(unit, mem, entry, DATA_TYPEf);
    rv = soc_flow_db_mem_to_view_id_get(unit,
                                 mem,
                                 SOC_FLOW_DB_KEY_TYPE_INVALID,
                                 data_type,
                                 0,
                                 NULL,
                                 &nh_view_id);
    BCM_IF_ERROR_RETURN(rv);

    if (action == TNL_SRV6_ACTION_TNL_ID_ATTACH) {
        action_set = soc_mem_field32_get(unit, nh_view_id, entry,
                        INTF_ACTION_SETf);
        intf_id = soc_format_field32_get(unit, INTF_ACTION_SETfmt,
                           &action_set, INTF_PTRf);
        rv = _bcm_flow_tunnel_idx_oif_set(unit, intf_id, tnl_idx);
        BCM_IF_ERROR_RETURN(rv);
        return BCM_E_NONE;
    }
    if (action == TNL_SRV6_ACTION_EGR_INTF_GET) {
        action_set = soc_mem_field32_get(unit, nh_view_id, entry,
                        INTF_ACTION_SETf);
        intf_id = soc_format_field32_get(unit, INTF_ACTION_SETfmt,
                           &action_set, INTF_PTRf);
        if (l3_oif != NULL) {
            *l3_oif = intf_id;
        }
        return BCM_E_NONE;
    }
    nh2_idx = 0;
    action_set = 0;
    action_set = soc_mem_field32_get(unit, nh_view_id, entry,
                        NEXT_PTR_ACTION_SETf);

    next_ptr_type = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTR_TYPEf);
    if (next_ptr_type == BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_NH) {
        nh2_idx = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTRf);
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, nh2_idx, entry);
        BCM_IF_ERROR_RETURN(rv);
        action_set = 0;
        action_set = soc_mem_field32_get(unit, flow_info->nh2_mem_view,
                        entry, NEXT_PTR_ACTION_SETf);
        next_ptr_type = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTR_TYPEf);
        if (next_ptr_type ==
                        BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_DVP_AND_VC_SWAP) {
            dvp_vc_swap = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTRf);
        }
    } else {
        if ((!next_ptr_type) || next_ptr_type ==
                          BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_DVP_AND_VC_SWAP) {
            dvp_vc_swap = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTRf);
        }
    }
    if (action == TNL_SRV6_ACTION_DVP_IDX_GET) {
        if (dvp != NULL) {
            *dvp = dvp_vc_swap;
        }

    } else if (action == TNL_SRV6_ACTION_DELETE) {

        if (nh2_idx) {
            flags = _BCM_L3_SHR_WRITE_DISABLE;
            rv = _bcm_flow_srv6_nh_del(unit, flags, nh2_idx);
            BCM_IF_ERROR_RETURN(rv);
        }
        if (dvp_vc_swap) {
            rv = _bcm_flow_dvp_vc_swap_entry_index_free(unit, NULL, TRUE,
                                  dvp_vc_swap);
            BCM_IF_ERROR_RETURN(rv);
        }

        /* clear the next_ptr field */
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, nh_idx, entry);
        BCM_IF_ERROR_RETURN(rv);
        soc_mem_field32_set(unit, nh_view_id, entry, NEXT_PTR_ACTION_SETf, 0);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, nh_idx, entry);
        BCM_IF_ERROR_RETURN(rv);

    } else if (action == TNL_SRV6_ACTION_DVP_ATTACH) {
        soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTRf, *dvp);
        soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTR_TYPEf, next_ptr_type);
        if (nh2_idx) {
            soc_mem_field32_set(unit, flow_info->nh2_mem_view, entry,
                        NEXT_PTR_ACTION_SETf, action_set);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, nh2_idx, entry);
            BCM_IF_ERROR_RETURN(rv);
        } else {
            soc_mem_field32_set(unit, nh_view_id, entry,
                        NEXT_PTR_ACTION_SETf, action_set);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, nh_idx, entry);
            BCM_IF_ERROR_RETURN(rv);
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_flow_tunnel_idx_from_oif_get(int unit, int oif_idx, int *tnl_idx)
{
    uint32 oif_entry[SOC_MAX_MEM_WORDS];
    soc_mem_t oif_mem_view;
    soc_mem_t oif_mem;
    int data_type;
    int rv;
    uint32 action_set;

    oif_mem = EGR_L3_INTFm;
    rv = soc_mem_read(unit, oif_mem, MEM_BLOCK_ANY,
                                  oif_idx,oif_entry);
    if (SOC_FAILURE(rv)) {
        return rv;
    }
    data_type = soc_mem_field32_get(unit, oif_mem,
                               oif_entry, DATA_TYPEf);

    /* find the memory view based on the memory and key type */
    rv = soc_flow_db_mem_to_view_id_get (unit, oif_mem,
                    SOC_FLOW_DB_KEY_TYPE_INVALID, data_type,
                    0, NULL, (uint32 *)&oif_mem_view);
    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    }
    if (SOC_SUCCESS(rv)) {
        if (SOC_MEM_FIELD_VALID(unit,oif_mem_view,
                                     TUNNEL_PTR_ACTION_SETf)) {
            action_set = 0;
            action_set = soc_mem_field32_get(unit, oif_mem_view, oif_entry,
                                      TUNNEL_PTR_ACTION_SETf);
            *tnl_idx = soc_format_field32_get(unit,TUNNEL_PTR_ACTION_SETfmt,
                                       &action_set,TUNNEL_INDEXf);
        }
    } else {
        if (SOC_MEM_FIELD_VALID(unit, oif_mem, TUNNEL_INDEXf)) {
            *tnl_idx = soc_mem_field32_get(unit, oif_mem, oif_entry,
                                      TUNNEL_INDEXf);
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_tunnel_dip_idx_set
 * Purpose:
 *      set L2 tunnel DIP address.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_X_XXX
 * NOTE:
 */

STATIC int
_bcm_flow_tunnel_dip_idx_set(int unit,
      bcm_flow_tunnel_initiator_t *info,
      int tnl_idx,
      int *tnl_in_oif)
{
    int rv = BCM_E_NONE;
    int data_type;
    soc_mem_t dvp_mem;
    uint32 dvp_attr[SOC_MAX_MEM_WORDS];
    soc_mem_t dvp_mem_view;
    int fixed_view = TRUE;
    uint32 dip6[4];
    int dvp;
    uint32 tnl_index;
    int l2tnl2oif = FALSE;
    int l2_tunnel = FALSE;

    if (tnl_in_oif != NULL) {
        *tnl_in_oif = FALSE;
    }
    if (info->flags & BCM_TUNNEL_REPLACE) {
        if (BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id) < 
                        _BCM_FLOW_TUNNEL_L2_MAX(unit)) {
            l2_tunnel = TRUE;
        }
    } else {
        if (_BCM_FLOW_TUNNEL_TYPE_L2(info->type)) {
            l2_tunnel = TRUE;
        }
    }

    if (l2_tunnel) {
        dvp_mem = EGR_DVP_ATTRIBUTEm;
        dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
        soc_mem_lock(unit,dvp_mem);
        rv = soc_mem_read(unit, dvp_mem, MEM_BLOCK_ANY, dvp, dvp_attr);
        data_type = soc_mem_field32_get(unit, dvp_mem, dvp_attr, DATA_TYPEf);
    
        /* find the memory view based on the memory and key type */
        rv = soc_flow_db_mem_to_view_id_get (unit, dvp_mem, 
                     SOC_FLOW_DB_KEY_TYPE_INVALID, data_type,
                    0, NULL, (uint32 *)&dvp_mem_view);
        LOG_VERBOSE(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
           "dvp(%d) mem_view_id: 0x%x\n"), dvp, dvp_mem_view));
        if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
            soc_mem_unlock(unit,dvp_mem);
            return rv;
        } else if (rv == SOC_E_NOT_FOUND) {
            /* not found, fixed view */
        } else {
            /* found, flex view */
            fixed_view = FALSE;
        }
    
        if (fixed_view == FALSE) {
            if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_DIP_VALID) {
                if (SOC_MEM_FIELD_VALID(unit,dvp_mem_view,
                             DIRECT_ASSIGNMENT_10_ACTION_SETf)) {
                    soc_mem_field32_set(unit, dvp_mem_view, dvp_attr,
                        DIRECT_ASSIGNMENT_10_ACTION_SETf, info->dip);
                }
            }
            if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_DIP6_VALID) {
    
                /* dip in dvp_attribute table */
                SAL_IP6_ADDR_TO_UINT32(info->dip6,dip6);
    
                /* OUTER_IPV6_DIP_0_31 */
                if (SOC_MEM_FIELD_VALID(unit,dvp_mem_view,
                                 DIRECT_ASSIGNMENT_10_ACTION_SETf)) {
                    soc_mem_field32_set(unit, dvp_mem_view, dvp_attr,
                            DIRECT_ASSIGNMENT_10_ACTION_SETf, dip6[0]);
                }
    
                /* OUTER_IPV6_DIP_32_63 */
                if (SOC_MEM_FIELD_VALID(unit,dvp_mem_view,
                                 DIRECT_ASSIGNMENT_12_ACTION_SETf)) {
                    soc_mem_field32_set(unit, dvp_mem_view, dvp_attr,
                            DIRECT_ASSIGNMENT_12_ACTION_SETf, dip6[1]);
                }
    
                /* OUTER_IPV6_DIP_64_95 */
                if (SOC_MEM_FIELD_VALID(unit,dvp_mem_view,
                                 DIRECT_ASSIGNMENT_14_ACTION_SETf)) {
                    soc_mem_field32_set(unit, dvp_mem_view, dvp_attr,
                            DIRECT_ASSIGNMENT_14_ACTION_SETf, dip6[2]);
                }
    
                /* OUTER_IPV6_DIP_96_127 */
                if (SOC_MEM_FIELD_VALID(unit,dvp_mem_view,
                                 DIRECT_ASSIGNMENT_16_ACTION_SETf)) {
                    soc_mem_field32_set(unit, dvp_mem_view, dvp_attr,
                            DIRECT_ASSIGNMENT_16_ACTION_SETf, dip6[3]);
                }
            }
            if (SOC_MEM_FIELD_VALID(unit,dvp_mem_view,
                                 TUNNEL_PTR_ACTION_SETf)) {
                tnl_index = 0;
                soc_format_field32_set(unit,TUNNEL_PTR_ACTION_SETfmt,
                                       &tnl_index,TUNNEL_INDEXf, tnl_idx);
                soc_mem_field32_set(unit, dvp_mem_view, dvp_attr,
                            TUNNEL_PTR_ACTION_SETf, tnl_index);
    
            } else if (info->l3_intf_id) {
                if (info->l3_intf_id < _BCM_FLOW_TUNNEL_L3_MAX(unit)) {
                    /* set to EGR_L3_INTF */
                    l2tnl2oif = TRUE;
                }
            }
        } else {   /* fixed view */
            /* L2GRE and VXLAN: DIP is in dvp_attr */
    
            if (data_type == _BCM_FLOW_VXLAN_EGRESS_DEST_VP_TYPE) {
                if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_DIP_VALID) {
                    soc_mem_field32_set(unit, dvp_mem, dvp_attr,
                               VXLAN__DIPf, info->dip);
                }
                soc_mem_field32_set(unit, dvp_mem, dvp_attr,
                               VXLAN__TUNNEL_INDEXf, tnl_idx);
    
            } else if (data_type == _BCM_FLOW_L2GRE_EGRESS_DEST_VP_TYPE) {
                if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_DIP_VALID) {
                    soc_mem_field32_set(unit, dvp_mem, dvp_attr,
                               L2GRE__DIPf, info->dip);
                }
                soc_mem_field32_set(unit, dvp_mem, dvp_attr,
                               L2GRE__TUNNEL_INDEXf, tnl_idx);
            }
        }
        rv = soc_mem_write(unit, dvp_mem, MEM_BLOCK_ALL, dvp, dvp_attr);
        soc_mem_unlock(unit,dvp_mem);
    }

    /* If l3 tunnel or certain l2 tunnel cases, set tunnel idx to oif table */
    if ((!_BCM_FLOW_TUNNEL_TYPE_L2(info->type)) || l2tnl2oif) {
        uint32 oif_entry[SOC_MAX_MEM_WORDS];
        soc_mem_t oif_mem_view;
        soc_mem_t oif_mem;

        oif_mem = EGR_L3_INTFm;
        soc_mem_lock(unit, oif_mem);
        rv = soc_mem_read(unit, oif_mem, MEM_BLOCK_ANY,
                                  info->l3_intf_id,oif_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit,oif_mem);
            return rv;
        }
        data_type = soc_mem_field32_get(unit, oif_mem,
                               oif_entry, DATA_TYPEf);
        fixed_view = TRUE;
        /* find the memory view based on the memory and key type */
        rv = soc_flow_db_mem_to_view_id_get (unit, oif_mem, 
                    SOC_FLOW_DB_KEY_TYPE_INVALID, data_type,
                    0, NULL, (uint32 *)&oif_mem_view);
        if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
            return rv;
        } else if (rv == SOC_E_NOT_FOUND) {
            /* not found, fixed view */
        } else {
            /* found, flex view */
            fixed_view = FALSE;
        }

        if (fixed_view == TRUE) {
            if (!_BCM_FLOW_TUNNEL_TYPE_L2(info->type)) {
                soc_mem_field32_set(unit, oif_mem, oif_entry,
                            TUNNEL_INDEXf, tnl_idx);
            }
        } else {
            if (SOC_MEM_FIELD_VALID(unit,oif_mem_view,
                                 TUNNEL_PTR_ACTION_SETf)) {
                tnl_index = 0;
                soc_format_field32_set(unit,TUNNEL_PTR_ACTION_SETfmt,
                                       &tnl_index,TUNNEL_INDEXf, tnl_idx);
                soc_mem_field32_set(unit, oif_mem_view, oif_entry,
                                      TUNNEL_PTR_ACTION_SETf, tnl_index);
            } else {
                soc_mem_unlock(unit, oif_mem);
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                   "TUNNEL_PTR_ACTION_SETf not in either dvp or oif")));
                return BCM_E_NOT_FOUND;
            }
        } 
        if (tnl_in_oif != NULL) {
            *tnl_in_oif = TRUE;
        }
        rv = soc_mem_write(unit, oif_mem, MEM_BLOCK_ALL, info->l3_intf_id, 
                           oif_entry);
        soc_mem_unlock(unit, oif_mem);
    }
    SOC_IF_ERROR_RETURN(rv);
    return rv;
}

/*
 * Function:
 *      _bcm_flow_tunnel_initiator_hash_calc
 * Purpose:
 *      Calculate tunnel initiator entry hash(signature).
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      buf  - (IN) Tunnel initiator entry information.
 *      hash - (OUT)Hash(signature) calculated value.
 * Returns:
 *      BCM_X_XXX
 * NOTE:
 * duplication of _bcm_td2_vxlan_tunnel_initiator_hash_calc for
 * not including legacy vxlan code in the future.
 *
 * plugged in _bcm_xgs3_tnl_init_hash_calc
 */

int
_bcm_flow_tunnel_initiator_hash_calc(int unit, void *buf, uint16 *hash)
{
    _bcm_flow_tunnel_record_t *tnl_rec;
    bcm_flow_tunnel_initiator_t tnl_entry;
    bcm_flow_tunnel_initiator_t *info;
    int ipv6;
   
    if ((NULL == buf) || (NULL == hash)) {
        return (BCM_E_PARAM);
    }

    tnl_rec = (_bcm_flow_tunnel_record_t *)buf;
    info = tnl_rec->info;

    ipv6 = _BCM_TUNNEL_OUTER_HEADER_IPV6(info->type) ||
                      info->type == bcmTunnelTypeSR6 ||
                      info->type == bcmTunnelTypeEthSR6;

    bcm_flow_tunnel_initiator_t_init(&tnl_entry);
    tnl_entry.type = info->type;
  
    if (_BCM_FLOW_TUNNEL_TYPE_L2(tnl_entry.type)) {
        /* Set source ip */
        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_SIP6_VALID) {
            sal_memcpy(tnl_entry.sip6, info->sip6, sizeof(bcm_ip6_t));
        }
    
        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_SIP_VALID) {
            tnl_entry.sip = info->sip;
        }
     
        tnl_entry.udp_dst_port = info->udp_dst_port;
        tnl_entry.udp_src_port = info->udp_src_port;

     } else {  /* L3 tunnel */
         /* include all fields, copy buffer to structure. */
         sal_memcpy(&tnl_entry, (void *)info, 
                           sizeof(bcm_flow_tunnel_initiator_t));

         /* Ignore flow label for not V6 tunnels. */
         if (ipv6)  {
             tnl_entry.flow_label = 0;
         }

         /* Mask fields we don't want to include in hash. */
         tnl_entry.flags = 0;
         tnl_entry.flow_handle = 0;
         tnl_entry.flow_option = 0;
         tnl_entry.flow_port = 0;
         tnl_entry.valid_elements = 0;
         tnl_entry.tunnel_id = 0;
         tnl_entry.l3_intf_id = 0;

         switch (tnl_entry.type) {
             case bcmTunnelTypeIp4In4:
             case bcmTunnelTypeIp6In4:
                 tnl_entry.type = bcmTunnelTypeIpAnyIn4;
                 break;
             case bcmTunnelTypeIp4In6:
             case bcmTunnelTypeIp6In6:
                 tnl_entry.type = bcmTunnelTypeIpAnyIn6;
                 break;
             case bcmTunnelTypeGre4In4:
             case bcmTunnelTypeGre6In4:
                 tnl_entry.type = bcmTunnelTypeGreAnyIn4;
                 break;
             case bcmTunnelTypeGre4In6:
             case bcmTunnelTypeGre6In6:
                 tnl_entry.type = bcmTunnelTypeGreAnyIn6;
                 break;
             case bcmTunnelTypeSR6:
                 sal_memset(tnl_entry.dip6, 0, sizeof(bcm_ip6_t));
                 break;
             default:
                 break;
         }
     }

     /* Calculate hash */
    *hash = _shr_crc16(0, (uint8 *)&tnl_entry,
        sizeof(bcm_flow_tunnel_initiator_t));
    
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_flow_tunnel_initiator_cmp
 * Purpose:
 *      Routine compares vxlan tunnel initiator entry with entry in the chip
 *      with specified index.
 * Parameters:
 *      unit       - (IN) SOC unit number.
 *      buf        - (IN) Tunnel initiator entry to compare.
 *      index      - (IN) Entry index in the chip to compare.
 *      cmp_result - (OUT)Compare result.
 * Returns:
 *      BCM_E_XXX
 *
 * NOTE:
 * duplication of _bcm_td2_vxlan_tunnel_initiator_cmp for
 * not including legacy vxlan code in the future.
 *
 * plugged in _bcm_xgs3_tnl_init_cmp
 */

int
_bcm_flow_tunnel_initiator_cmp(int unit, void *buf,
                                          int index, int *cmp_result)
{
    bcm_flow_tunnel_initiator_t hw_entry;   /* Entry read from hw */
    bcm_flow_tunnel_initiator_t *tnl_entry; /* Api passed buffer */
    _bcm_flow_tunnel_record_t *tnl_rec;
    int rv = BCM_E_NONE;
    bcm_flow_logical_field_t field[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32 data_ids[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32 mem_entry[SOC_MAX_MEM_WORDS];
    int i;
    uint32 cnt = 0;
    int data_type = 0;
    int flow_ctrl_id;
    int hw_data_type = 0;
    int hw_flow_ctrl_id;
    int flex_type_match = FALSE;
    soc_mem_t mem;
    bcm_flow_tunnel_srv6_srh_t srh;
    bcm_ip6_t sid_list[TNL_SRV6_SID_MAX];
    soc_mem_t view_id;
    bcm_ip6_t sid_null;

    if ((NULL == buf) || (NULL == cmp_result)) {
        return BCM_E_PARAM;
    }
    tnl_rec = (_bcm_flow_tunnel_record_t *)buf;

    *cmp_result = BCM_L3_CMP_NOT_EQUAL;
    mem = EGR_IP_TUNNELm;

    /* point to the input entry */
    tnl_entry = tnl_rec->info;

    /* Initialize entry to get from hw */
    bcm_flow_tunnel_initiator_t_init(&hw_entry);

    if (_BCM_FLOW_IS_FLEX_VIEW(tnl_entry)) {
        rv = soc_flow_db_mem_view_field_list_get(unit,
                      tnl_rec->mem_view,
                      SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                      _BCM_FLOW_LOGICAL_FIELD_MAX, data_ids, &cnt);
        SOC_IF_ERROR_RETURN(rv);
        for (i = 0; i < cnt; i++) {
            field[i].id = data_ids[i];
        }
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, mem_entry);
        SOC_IF_ERROR_RETURN(rv);
        hw_data_type = soc_mem_field32_get(unit, mem, mem_entry, DATA_TYPEf);
        hw_flow_ctrl_id = soc_mem_field32_get(unit, mem, mem_entry, 
                                         FLEX_FLOW_SELECT_CTRL_IDf);
        soc_flow_db_mem_view_entry_init(unit,tnl_rec->mem_view, mem_entry);
        data_type = soc_mem_field32_get(unit, tnl_rec->mem_view, 
                                        mem_entry, DATA_TYPEf);
        if (SOC_MEM_FIELD_VALID(unit, tnl_rec->mem_view,
                                 FLEX_FLOW_SELECT_CTRL_IDf)) {
            flow_ctrl_id = soc_mem_field32_get(unit, tnl_rec->mem_view,
                            mem_entry, FLEX_FLOW_SELECT_CTRL_IDf);
        } else {
            flow_ctrl_id = 0;
        }
        if (hw_data_type == data_type && hw_flow_ctrl_id == flow_ctrl_id) {
            flex_type_match = TRUE;
        } else {
            return (BCM_E_NONE);
        }
    } 

    /* Get tunnel initiator entry from hw */
    rv = _bcm_flow_hw_tunnel_entry_get(unit, index, &hw_entry,field,
                                       cnt,NULL);
    /* compare the type first */
    if ((tnl_entry->type != hw_entry.type) && (flex_type_match == FALSE)) {
        switch (tnl_entry->type) {
          case bcmTunnelTypeIp4In4:
          case bcmTunnelTypeIp6In4:
              if (bcmTunnelTypeIpAnyIn4 != hw_entry.type) {
                  return (BCM_E_NONE);
              }
              break;
          case bcmTunnelTypeIp4In6:
          case bcmTunnelTypeIp6In6:
              if (bcmTunnelTypeIpAnyIn6 != hw_entry.type) {
                  return (BCM_E_NONE);
              }
              break;
          case  bcmTunnelTypeGre4In4:
          case  bcmTunnelTypeGre6In4:
              if (bcmTunnelTypeGreAnyIn4 != hw_entry.type) {
                  return (BCM_E_NONE);
              }
              break;
          case    bcmTunnelTypeGre4In6:
          case    bcmTunnelTypeGre6In6:
              if (bcmTunnelTypeGreAnyIn6 != hw_entry.type) {
                  return (BCM_E_NONE);
              }
              break;
          default:
              return (BCM_E_NONE);
        }
    }

    if (data_type != hw_data_type) {
        return BCM_E_NONE;
    }
    
    /* Compare source ip */
    if (tnl_entry->valid_elements & BCM_FLOW_TUNNEL_INIT_SIP_VALID) {
        if (tnl_entry->sip != hw_entry.sip) {
            return BCM_E_NONE;
        }
    }
    if (tnl_entry->valid_elements & BCM_FLOW_TUNNEL_INIT_SIP6_VALID) {
        if (sal_memcmp(tnl_entry->sip6, hw_entry.sip6, sizeof(bcm_ip6_t))) {
            return BCM_E_NONE;
        }
    }

    if ((!_BCM_FLOW_TUNNEL_TYPE_L2(tnl_entry->type)) &&
        (tnl_entry->type != bcmTunnelTypeSR6)) {
        if (tnl_entry->valid_elements & BCM_FLOW_TUNNEL_INIT_DIP_VALID) {
            if (tnl_entry->dip != hw_entry.dip) {
                return BCM_E_NONE;
            }
        }
        if (tnl_entry->valid_elements & BCM_FLOW_TUNNEL_INIT_DIP6_VALID) {
            if (sal_memcmp(tnl_entry->dip6, hw_entry.dip6, sizeof(bcm_ip6_t))) {
                return BCM_E_NONE;
            }
        }
    }

    /* Compare udp dst port */
    if ((tnl_entry->udp_dst_port != hw_entry.udp_dst_port) ||
        (tnl_entry->udp_src_port != hw_entry.udp_src_port)) {
        return BCM_E_NONE;
    }

    /* Compare hw entry with passed one. */
    if ((tnl_entry->dscp_sel != hw_entry.dscp_sel) ||
        (tnl_entry->dscp != hw_entry.dscp)) {
        return (BCM_E_NONE);
    }

    if (tnl_entry->ttl != hw_entry.ttl) {
        return (BCM_E_NONE);
    }
    /* Compare destination mac. */
    if (sal_memcmp(tnl_entry->dmac, hw_entry.dmac, sizeof(bcm_mac_t))) {
        return (BCM_E_NONE);
    }

    /* compare the logical fields if it is flex view */
    if (_BCM_FLOW_IS_FLEX_VIEW(tnl_entry)) {
        for (i = 0; i < cnt && i < tnl_rec->num_of_fields; i++) {
            if (field[i].id == tnl_rec->field[i].id) {
                if (field[i].value != tnl_rec->field[i].value) {
                    return (BCM_E_NONE);
                }
            }                    
        }
    }

    /* check if SRv6 entry */
    if (data_type == BCMI_TD3_TNL_DATA_TYPE_SRV6_L3 ||
        data_type == BCMI_TD3_TNL_DATA_TYPE_SRV6_NON_CANONICAL ||
        data_type == BCMI_TD3_TNL_DATA_TYPE_SRV6_L2) {

        mem = EGR_IP_TUNNEL_IPV6m;
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index >> 1, mem_entry);
        SOC_IF_ERROR_RETURN(rv);
        rv = soc_flow_db_mem_to_view_id_get(unit,
                             mem,
                             SOC_FLOW_DB_KEY_TYPE_INVALID,
                             hw_data_type,
                             0,
                             NULL,
                             (uint32 *)&view_id);
        if (rv == BCM_E_NOT_FOUND) {
            return BCM_E_NONE;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }
        rv = _bcm_flow_hw_srv6_srh_entry_get(unit, mem_entry,
                          view_id, &srh, sid_list);
        if (rv == BCM_E_NOT_FOUND) {
            return BCM_E_NONE;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }
        if (tnl_rec->srv6_info != NULL) {
            if (tnl_rec->srv6_info->tag &&
                (tnl_rec->srv6_info->tag != srh.tag)) {
                return BCM_E_NONE;
            }
            if (tnl_rec->srv6_info->next_hdr &&
                (tnl_rec->srv6_info->next_hdr != srh.next_hdr)) {
                return BCM_E_NONE;
            }
            if (tnl_rec->srv6_info->flags &&
                (tnl_rec->srv6_info->flags != srh.flags)) {
                return BCM_E_NONE;
            }
        } else {
            if (srh.tag || srh.next_hdr || srh.flags) {
                return BCM_E_NONE;
            }
        }

        if (tnl_rec->sid_list != NULL) {
            for (i = 0; i < TNL_SRV6_SID_MAX; i++) {
                if (sal_memcmp(tnl_rec->sid_list[i], sid_list[i],
                    sizeof(bcm_ip6_t))) {
                    return BCM_E_NONE;
                }
            }
        } else {
            sal_memset(sid_null, 0, sizeof(bcm_ip6_t));
            for (i = 0; i < TNL_SRV6_SID_MAX; i++) {
                if (sal_memcmp(sid_null, sid_list[i], sizeof(bcm_ip6_t))) {
                    return BCM_E_NONE;
                }
            }
        }
    }

    *cmp_result = BCM_L3_CMP_EQUAL;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_tnl_init_add
 * Purpose:
 *       Allocate tunnel initiator table index & Write tunnel initiator entry.
 *       Routine trying to match new entry to an existing one,
 *       if match found reference cound is incremented, otherwise
 *       new index allocated & entry added to hw.
 * Parameters:
 *      unit       - (IN)StrataSwitch unit number.
 *      flags      - (IN)Table shared flags.
 *      tnl_entry  - (IN)Tunnel initiator entry.
 *      index      - (OUT)next hop index.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_flow_tnl_init_add(int unit, uint32 flags, 
                      bcm_flow_tunnel_initiator_t *tnl_entry,
                      bcm_flow_logical_field_t *field,
                      uint32 num_of_fields,
                      bcm_flow_tunnel_srv6_srh_t *srv6_srh,
                      bcm_ip6_t *sid_list,
                      int *index)
{
    _bcm_l3_tbl_op_t data;        /* Operation data.               */
    soc_flow_db_mem_view_info_t vinfo;
    int rv = BCM_E_NONE;
    soc_mem_t mem_view_id;
    int double_wide;
    _bcm_flow_tunnel_record_t tnl_rec;

    if (!(flags & _BCM_L3_SHR_WRITE_DISABLE)) {
        /* Make sure hw call is defined. */
        if (!BCM_XGS3_L3_HWCALL_CHECK(unit, tnl_init_add)) {
            return (BCM_E_UNAVAIL);
        }
    }

    tnl_rec.info = tnl_entry;
    tnl_rec.field = field;
    tnl_rec.num_of_fields = num_of_fields;
    tnl_rec.srv6_info = srv6_srh;
    tnl_rec.sid_list  = sid_list;

    /* get the physical memory view */
    if (_BCM_FLOW_IS_FLEX_VIEW(tnl_entry)) {
        rv = soc_flow_db_ffo_to_mem_view_id_get(unit,tnl_entry->flow_handle,
                   tnl_entry->flow_option,
                   SOC_FLOW_DB_FUNC_TUNNEL_INITIATOR_ID,
                    (uint32 *)&mem_view_id);
        BCM_IF_ERROR_RETURN(rv);
        rv = soc_flow_db_mem_view_info_get(unit,mem_view_id,&vinfo);
        BCM_IF_ERROR_RETURN(rv);
        double_wide = vinfo.width > _BCM_FLOW_IP_TUNNEL_SINGLE_WIDE_WIDTH? 
                      TRUE: FALSE;
        tnl_rec.mem_view = mem_view_id;
    } else {
        double_wide = (_BCM_TUNNEL_OUTER_HEADER_IPV6(tnl_entry->type) ||
                      tnl_entry->type == bcmTunnelTypeSR6 ||
                      tnl_entry->type == bcmTunnelTypeEthSR6)? TRUE: FALSE;
    }

    /* this flag must be set, so tnl_init_add never called */
    flags |= _BCM_L3_SHR_WRITE_DISABLE;

    /* Initialization */
    sal_memset(&data, 0, sizeof(_bcm_l3_tbl_op_t));
    data.tbl_ptr =  BCM_XGS3_L3_TBL_PTR(unit, tnl_init);
    data.width = double_wide? _BCM_DOUBLE_WIDE: _BCM_SINGLE_WIDE;
    data.oper_flags = flags;
    data.entry_buffer = (void *)&tnl_rec;
    data.hash_func = _bcm_flow_tunnel_initiator_hash_calc;
    data.cmp_func  = _bcm_flow_tunnel_initiator_cmp;
 
    /* init add_func to pass the error check */
    data.add_func  = BCM_XGS3_L3_HWCALL(unit, tnl_init_add);
    if (flags & _BCM_L3_SHR_WITH_ID) {
        data.entry_index = *index;
    }

    /* Add tunnel initiator entry. */
    BCM_IF_ERROR_RETURN (_bcm_xgs3_tbl_add(unit, &data));
    *index = data.entry_index;

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_flow_tunnel_init_delete
 * Purpose:
 *      Clear the tunnel initiator hardware entry.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      idx      - (IN)Index to tunnel initiator hardware entry table.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_flow_tunnel_init_delete(int unit, int idx, int width)
{
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to write hw entry */
    soc_mem_t mem;                        /* Tunnel initiator table memory */

    /* Get table memory. */
    mem = BCM_XGS3_L3_MEM(unit, tnl_init_v4);
    if (width == _BCM_DOUBLE_WIDE) {
        mem = BCM_XGS3_L3_MEM(unit, tnl_init_v6);
        idx >>= 1;    /* Each record takes two slots. */
    }
    /* Zero write buffer. */
    sal_memset(&tnl_entry, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Write buffer to hw. */
    return BCM_XGS3_MEM_WRITE(unit, mem, idx, &tnl_entry);
}

/*
 * Function:
 *      _bcm_flow_tunnel_init_get
 * Purpose:
 *      Add tunnel initiator entry to hw.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      idx      - (IN)Index to insert tunnel initiator.
 *      info     - (IN)Tunnel initiator entry info.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_flow_tunnel_init_get(int unit, 
            int soft_tnl_idx, 
            bcm_flow_tunnel_initiator_t *info,
            bcm_flow_logical_field_t *field,
            uint32 num_of_fields,
            uint32 *field_cnt)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    soc_mem_t mem_view;
    int rv = BCM_E_NONE;                  /* Return value                  */
    uint32 dip6[4];
    uint32 data_type;
    _bcm_flow_bookkeeping_t *flow_info;
    int oif_idx;
    int tnl_idx;
    uint32 *dvp_entry;
    uint32 *vc_swap_entry;
    _bcm_vp_info_t vp_info;
    int nh_idx;
    int dvp_idx;
    int l2_tunnel = FALSE;
    int flex_view = FALSE;

    flow_info = FLOW_INFO(unit);

    tnl_idx = flow_info->init_tunnel[soft_tnl_idx].idx;
    dvp_idx = 0;
    if (soft_tnl_idx >= _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit)) {

        /* check if SRv6 L3 tunnel */
        nh_idx = soft_tnl_idx - _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit);
        rv = _bcm_flow_srv6_l3_tnl_object_ctrl(unit,
                     TNL_SRV6_ACTION_DVP_IDX_GET, nh_idx, &dvp_idx, 0, NULL);
        if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
            return rv;
        }
        info->type = bcmTunnelTypeSR6;
        rv = BCM_E_NONE;
    } else if (soft_tnl_idx < _BCM_FLOW_TUNNEL_L2_MAX(unit)) {
        /* L2 tunnel */
        l2_tunnel = TRUE;
        dvp_idx = soft_tnl_idx;
    }

    if (dvp_idx) {
        dvp_entry = entry;
        mem = EGR_DVP_ATTRIBUTEm;
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, dvp_idx, dvp_entry);
        data_type = soc_mem_field32_get(unit, mem, dvp_entry,
                                            DATA_TYPEf); 

        /* find the memory view based on the memory and key type */
        rv = soc_flow_db_mem_to_view_id_get (unit, mem,
                    SOC_FLOW_DB_KEY_TYPE_INVALID, data_type,
                    0, NULL, (uint32 *)&mem_view);
        if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
            return rv;
        } else if (rv == SOC_E_NOT_FOUND) {
            /* not found, fixed view */
        } else {
            /* found, flex view */
            if (l2_tunnel) {
                info->type = bcmTunnelTypeL2Flex;
            }
            flex_view = TRUE;
        } 

        if (flex_view) {
            /* dip in dvp_attribute table */ 

            dip6[0] = 0;
            dip6[1] = 0;
            dip6[2] = 0;
            dip6[3] = 0;
            SAL_IP6_ADDR_FROM_UINT32(info->dip6,dip6);
            info->dip = 0;

            /* OUTER_IPV6_DIP_0_31 */
            if (SOC_MEM_FIELD_VALID(unit, mem_view,
                         DIRECT_ASSIGNMENT_10_ACTION_SETf)) {
                dip6[0] = soc_mem_field32_get(unit, mem_view, dvp_entry,
                                DIRECT_ASSIGNMENT_10_ACTION_SETf);
            }

            /* OUTER_IPV6_DIP_32_63 */
            if (SOC_MEM_FIELD_VALID(unit, mem_view,
                         DIRECT_ASSIGNMENT_12_ACTION_SETf)) {
                dip6[1] = soc_mem_field32_get(unit, mem_view, dvp_entry,
                                DIRECT_ASSIGNMENT_12_ACTION_SETf);
            }

            /* OUTER_IPV6_DIP_64_95 */
            if (SOC_MEM_FIELD_VALID(unit, mem_view,
                         DIRECT_ASSIGNMENT_14_ACTION_SETf)) {
                dip6[2] = soc_mem_field32_get(unit, mem_view, dvp_entry,
                                       DIRECT_ASSIGNMENT_14_ACTION_SETf);
            }

            /* OUTER_IPV6_DIP_96_127 */
            if (SOC_MEM_FIELD_VALID(unit, mem_view,
                                 DIRECT_ASSIGNMENT_16_ACTION_SETf)) {
                dip6[3] = soc_mem_field32_get(unit, mem_view, dvp_entry,
                                DIRECT_ASSIGNMENT_16_ACTION_SETf);
            }

            /* check if dvp extension entry */
            BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, dvp_idx, &vp_info));
            if (vp_info.flags & _BCM_VP_INFO_DVP_EXTENSION) {
                if (l2_tunnel) {
                    info->type = bcmTunnelTypeEthSR6;
                }
                vc_swap_entry = entry;
                mem = EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm;

                /* find the memory view based on the memory and key type */
                rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, dvp_idx,
                          vc_swap_entry);
                BCM_IF_ERROR_RETURN(rv);
                data_type = soc_mem_field32_get(unit, mem, vc_swap_entry,
                                            DATA_TYPEf);
                rv = soc_flow_db_mem_to_view_id_get (unit, mem,
                            SOC_FLOW_DB_KEY_TYPE_INVALID, data_type,
                            0, NULL, (uint32 *)&mem_view);
                BCM_IF_ERROR_RETURN(rv);
                /* OUTER_IPV6_DIP_96_127 */
                if (SOC_MEM_FIELD_VALID(unit, mem_view,
                                 DIRECT_ASSIGNMENT_16_ACTION_SETf)) {
                    dip6[3] = soc_mem_field32_get(unit, mem_view, vc_swap_entry,
                                DIRECT_ASSIGNMENT_16_ACTION_SETf);
                }
            }

            if ((dip6[1] == 0) && (dip6[2] == 0) && (dip6[3] == 0)) {
                info->dip = dip6[0];
            } else {
                SAL_IP6_ADDR_FROM_UINT32(info->dip6,dip6);
            }
        } else if (l2_tunnel == TRUE) {   /* fixed view, only for L2 */

            /* L2GRE and VXLAN: DIP is in dvp_attr */
 
            if (data_type == _BCM_FLOW_VXLAN_EGRESS_DEST_VP_TYPE) {
                info->type = bcmTunnelTypeVxlan;
                info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN;
                info->dip = soc_mem_field32_get(unit, mem, dvp_entry,
                                                   VXLAN__DIPf);
            } else if (data_type == _BCM_FLOW_L2GRE_EGRESS_DEST_VP_TYPE) {
                info->type = bcmTunnelTypeL2Gre;
                info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE;
                info->dip = soc_mem_field32_get(unit, mem, dvp_entry,
                                                 L2GRE__DIPf);
            } else {
                return BCM_E_INTERNAL;
            }
        }
    }
    if (l2_tunnel == TRUE) {
        if (tnl_idx >= _BCM_FLOW_TUNNEL_IP_TUNNEL_MAX(unit)) {
            /* l3_oif index */
            oif_idx = tnl_idx - _BCM_FLOW_TUNNEL_IP_TUNNEL_MAX(unit);
            info->l3_intf_id = oif_idx;
            BCM_IF_ERROR_RETURN(_bcm_flow_tunnel_idx_from_oif_get(unit,
                             oif_idx, &tnl_idx));
        }
    }

    rv = _bcm_flow_hw_tunnel_entry_get(unit, tnl_idx, info, field, 
                           num_of_fields,field_cnt);
    BCM_IF_ERROR_RETURN(rv);
    return rv;
}

/*
 * Function:
 *     _bcm_flow_hw_tunnel_entry_get 
 * Purpose:
 *      Get a IP_TUNNEL entry and convert to bcm_flow_tunnel_initiator_t info.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      idx      - (IN)Index to IP_TUNNEL hw table.
 *      info     - (IN)Tunnel initiator entry info.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_flow_hw_tunnel_entry_get(int unit,
            int idx,
            bcm_flow_tunnel_initiator_t *info,
            bcm_flow_logical_field_t *field,
            uint32 num_of_fields,
            uint32 *field_cnt)
{
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to write hw entry */
    soc_mem_t mem;                        /* Tunnel initiator table memory */
    soc_mem_t mem_view_id;
    int rv;                               /* Return value                  */
    int df_val=0;                           /* Don't fragment encoding       */
    uint32 sip6[4];
    uint32 dip6[4];
    uint32 tnl_data_type;
    int tnl_type = 0;
    uint32 data_ids[_BCM_FLOW_LOGICAL_FIELD_MAX];
    soc_flow_db_ffo_t ffo[SOC_FLOW_DB_MAX_VIEW_FFO_TPL];
    uint32 num_ffo = 0;
    uint32 cnt;
    int i;
    int j;
    uint32 ttl;
    int fixed_view = TRUE;
    soc_field_t qos_f;
    egr_sequence_number_table_entry_t entry;
    _bcm_flow_bookkeeping_t *flow_info;
    int dscp_sel;

    flow_info = FLOW_INFO(unit);

    mem = EGR_IP_TUNNELm;
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, tnl_entry);
    SOC_IF_ERROR_RETURN(rv);
    tnl_data_type = soc_mem_field32_get(unit, mem, tnl_entry, DATA_TYPEf);
    if (_BCM_FLOW_TUNNEL_ENTRY_DOUBLE_WIDE(tnl_data_type)) {
        mem = BCM_XGS3_L3_MEM(unit, tnl_init_v6);
        idx >>= 1;    /* Each record takes two slots. */

        rv = BCM_XGS3_MEM_READ(unit, mem, idx, tnl_entry);
        BCM_IF_ERROR_RETURN(rv);
    }
    
    /* find the memory view based on the memory and key type */
    rv = soc_flow_db_mem_to_view_id_get (unit, mem, 
                SOC_FLOW_DB_KEY_TYPE_INVALID, tnl_data_type,
                0, NULL, (uint32 *)&mem_view_id);
    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    } else if (rv == SOC_E_NOT_FOUND) {
        /* not found, fixed view */
        fixed_view = TRUE;
    } else {
        /* found, flex view */
        if (info->type != bcmTunnelTypeL2Flex &&
            info->type != bcmTunnelTypeSR6 &&
            info->type != bcmTunnelTypeEthSR6) {
            info->type = bcmTunnelTypeL3Flex; /* set as default */
        }
        fixed_view = FALSE;
    }

    if (fixed_view == TRUE) {  /* fixed view */
        tnl_type = soc_mem_field32_get(unit, mem, tnl_entry, TUNNEL_TYPEf);
        if (info->type == bcmTunnelTypeNone) {
            if (tnl_type == _BCM_FLOW_VXLAN_TUNNEL_TYPE) {
                info->type = bcmTunnelTypeVxlan;
            } else if (tnl_type == _BCM_FLOW_L2GRE_TUNNEL_TYPE) {
                info->type = bcmTunnelTypeL2Gre;
            } else {
                BCM_IF_ERROR_RETURN(_bcm_trx_tnl_hw_code_to_type(unit, tnl_type,
                                    tnl_data_type, &info->type));
            }
        }
        if (tnl_data_type == _BCM_FLOW_TUNNEL_DATA_TYPE_IPV4) {
            if ((tnl_type != _BCM_FLOW_VXLAN_TUNNEL_TYPE) &&
                (tnl_type != _BCM_FLOW_L2GRE_TUNNEL_TYPE)) {
                info->dip = soc_mem_field32_get(unit, mem, tnl_entry, DIPf);
            }
            info->sip = soc_mem_field32_get(unit, mem, tnl_entry, SIPf);
        } else if (tnl_data_type == _BCM_FLOW_TUNNEL_DATA_TYPE_IPV6) {

            /* Get destination ip. */
            soc_mem_ip6_addr_get(unit, mem, tnl_entry, DIPf, info->dip6,
                                 SOC_MEM_IP6_FULL_ADDR);

            /* Get source ip. */
            soc_mem_ip6_addr_get(unit, mem, tnl_entry, SIPf, info->sip6,
                                 SOC_MEM_IP6_FULL_ADDR);
        } else if (tnl_data_type == _BCM_FLOW_TUNNEL_DATA_TYPE_NONE) {
          /* Not a valid entry */
          return rv;
        }

        /* Tunnel header DSCP select. */
        info->dscp_sel = soc_mem_field32_get(unit, mem, tnl_entry, DSCP_SELf);

        if (info->dscp_sel == bcmTunnelDscpMap) {
            /* Set DSCP_MAP value. */
            int hw_map_idx = 0;
            hw_map_idx = soc_mem_field32_get(unit, mem, tnl_entry,
                            DSCP_MAPPING_PTRf);
            BCM_IF_ERROR_RETURN(
                _bcm_tr2_qos_idx2id(unit, hw_map_idx,
                      _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE, &info->dscp_map));
        } else {
            /* Set DSCP value. */
            info->dscp = soc_mem_field32_get(unit, mem, tnl_entry, DSCPf);
        }
        /* Set TTL. */
        info->ttl = soc_mem_field32_get(unit, mem, tnl_entry, TTLf);

        /* Set UDP Dest Port */
        info->udp_dst_port = soc_mem_field32_get(unit, mem, tnl_entry, 
                                        L4_DEST_PORTf);

        /* Set UDP Src Port */
        info->udp_src_port = soc_mem_field32_get(unit, mem, tnl_entry, 
                                    L4_SRC_PORTf);

        if (SOC_MEM_FIELD_VALID(unit, mem, IPV4_DF_SELf)) {
            /* IP tunnel hdr DF bit for IPv4 */
            df_val = soc_mem_field32_get(unit, mem, tnl_entry, IPV4_DF_SELf);
            if (0x2 <= df_val) {
                info->flags |= BCM_TUNNEL_INIT_USE_INNER_DF;
            } else if (0x1 == df_val) {
                info->flags |= BCM_TUNNEL_INIT_IPV4_SET_DF;
            }
        }

        if (SOC_MEM_FIELD_VALID(unit, mem, IPV6_DF_SELf)) {
            /* IP tunnel hdr DF bit for IPv6 */
            if (soc_mem_field32_get(unit, mem, tnl_entry, IPV6_DF_SELf)) {
                info->flags |= BCM_TUNNEL_INIT_IPV6_SET_DF;
            }
        }

        /* FRAGMENT ID */
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_SEQUENCE_NUMBER_TABLEm,
            MEM_BLOCK_ANY, idx + flow_info->frag_base_inx, &entry));
        info->ip4_id = soc_mem_field32_get(unit, EGR_SEQUENCE_NUMBER_TABLEm,
                        &entry, SEQUENCE_NUMBERf);

    } else {  /* flex view */
        uint32 qos = 0;
        /* Get the flow handle, function id, option id */
        BCM_IF_ERROR_RETURN(
              soc_flow_db_mem_view_to_ffo_get(unit,
                                         mem_view_id,
                                         SOC_FLOW_DB_MAX_VIEW_FFO_TPL,
                                         ffo,
                                         &num_ffo));

        info->flow_handle = ffo[0].flow_handle;
        info->flow_option = ffo[0].option_id;
        if (info->type == bcmTunnelTypeL3Flex) {
            dip6[0] = 0;
            dip6[1] = 0;
            dip6[2] = 0;
            dip6[3] = 0;
            info->dip = 0;
            SAL_IP6_ADDR_FROM_UINT32(info->dip6,dip6);

            /* OUTER_IPV6_DIP_0_31 */
            if (SOC_MEM_FIELD_VALID(unit,mem_view_id,
                         DIRECT_ASSIGNMENT_10_ACTION_SETf)) {
                dip6[0] = soc_mem_field32_get(unit, mem_view_id, tnl_entry,
                                        DIRECT_ASSIGNMENT_10_ACTION_SETf);
            }

            /* OUTER_IPV6_DIP_32_63 */
            if (SOC_MEM_FIELD_VALID(unit,mem_view_id,
                         DIRECT_ASSIGNMENT_12_ACTION_SETf)) {
                dip6[1] = soc_mem_field32_get(unit, mem_view_id, tnl_entry,
                                        DIRECT_ASSIGNMENT_12_ACTION_SETf);
            }

            /* OUTER_IPV6_DIP_64_95 */
            if (SOC_MEM_FIELD_VALID(unit,mem_view_id,
                             DIRECT_ASSIGNMENT_14_ACTION_SETf)) {
                dip6[2] = soc_mem_field32_get(unit, mem_view_id, tnl_entry,
                        DIRECT_ASSIGNMENT_14_ACTION_SETf);
            }

            /* OUTER_IPV6_DIP_96_127 */
            if (SOC_MEM_FIELD_VALID(unit,mem_view_id,
                             DIRECT_ASSIGNMENT_16_ACTION_SETf)) {
                dip6[3] = soc_mem_field32_get(unit, mem_view_id, tnl_entry,
                        DIRECT_ASSIGNMENT_16_ACTION_SETf);
            }
            if ((dip6[1] == 0) && (dip6[2] == 0) && (dip6[3] == 0)) {
                info->dip = dip6[0];
            } else {
                SAL_IP6_ADDR_FROM_UINT32(info->dip6,dip6);
            }
        }
                 
        sip6[0] = 0;
        sip6[1] = 0;
        sip6[2] = 0;
        sip6[3] = 0;
        info->sip = 0;
        SAL_IP6_ADDR_FROM_UINT32(info->sip6,sip6);

        if (SOC_MEM_FIELD_VALID(unit,mem_view_id,
                         DIRECT_ASSIGNMENT_11_ACTION_SETf)) {
            sip6[0] = soc_mem_field32_get(unit, mem_view_id, tnl_entry,
                            DIRECT_ASSIGNMENT_11_ACTION_SETf);
        }
        if (SOC_MEM_FIELD_VALID(unit,mem_view_id,
                         DIRECT_ASSIGNMENT_13_ACTION_SETf)) {
            sip6[1] = soc_mem_field32_get(unit, mem_view_id, tnl_entry,
                            DIRECT_ASSIGNMENT_13_ACTION_SETf);
        }
        if (SOC_MEM_FIELD_VALID(unit,mem_view_id,
                         DIRECT_ASSIGNMENT_15_ACTION_SETf)) {
            sip6[2] = soc_mem_field32_get(unit, mem_view_id, tnl_entry,
                                DIRECT_ASSIGNMENT_15_ACTION_SETf);
        }
        if (SOC_MEM_FIELD_VALID(unit,mem_view_id,
                         DIRECT_ASSIGNMENT_17_ACTION_SETf)) {
            sip6[3] = soc_mem_field32_get(unit, mem_view_id, tnl_entry,
                                DIRECT_ASSIGNMENT_17_ACTION_SETf);
        }
        if ((sip6[1] == 0) && (sip6[2] == 0) && (sip6[3] == 0)) {
            info->sip = sip6[0];
        } else {
            SAL_IP6_ADDR_FROM_UINT32(info->sip6,sip6);
        }

        if (SOC_MEM_FIELD_VALID(unit,mem_view_id,
                         QOS_A_ACTION_SETf)) {
            qos_f = QOS_A_ACTION_SETf;
        } else if (SOC_MEM_FIELD_VALID(unit,mem_view_id,
                         QOS_B_ACTION_SETf)) {
            qos_f = QOS_B_ACTION_SETf;
        } else {
            qos_f = 0;
        }
        if (qos_f) {    
            qos = soc_mem_field32_get(unit, mem_view_id, tnl_entry, qos_f);
            dscp_sel = soc_format_field32_get(unit,QOS_A_ACTION_SETfmt,
                               &qos,QOS_ACTION_SELf);
            if (dscp_sel == _BCM_FLOW_DSCP_SEL_ASSIGN_FROM_TABLE) {
                info->dscp_sel = bcmTunnelDscpAssign;
            } else if (dscp_sel == _BCM_FLOW_DSCP_SEL_COPY_FROM_ZONE) {
                info->dscp_sel = bcmTunnelDscpPacket;
            } else if (dscp_sel == _BCM_FLOW_DSCP_SEL_MAP_FROM_PHB) {
                info->dscp_sel = bcmTunnelDscpMap;
            } else {
                info->dscp_sel = bcmTunnelDscpCount;
            }
               
            if (info->dscp_sel == bcmTunnelDscpMap) {
                /* Set DSCP_MAP value. */
                int hw_map_idx = 0;
                hw_map_idx = soc_format_field32_get(unit,QOS_A_ACTION_SETfmt,
                               &qos,QOS_MAPPING_PTRf);
                BCM_IF_ERROR_RETURN(
                    _bcm_tr2_qos_idx2id(unit, hw_map_idx,
                        _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE,&info->dscp_map));
            } else if (info->dscp_sel == bcmTunnelDscpAssign) {
                /* Set DSCP value. */
                info->dscp = soc_format_field32_get(unit,QOS_A_ACTION_SETfmt,
                                   &qos,QOS_VALUEf);
            }
        }
        if (SOC_MEM_FIELD_VALID(unit,mem_view_id, TTL_B_ACTION_SETf)) {
            ttl = soc_mem_field32_get(unit, mem_view_id, tnl_entry, 
                          TTL_B_ACTION_SETf);
            info->ttl = soc_format_field32_get(unit,TTL_B_ACTION_SETfmt,
                               &ttl,TTL_VALUEf);
        }

        /* Set UDP Dest Port */
        if (SOC_MEM_FIELD_VALID(unit,mem_view_id, 
                             DIRECT_ASSIGNMENT_18_ACTION_SETf)) {
            info->udp_dst_port = soc_mem_field32_get(unit, mem_view_id, 
                          tnl_entry, DIRECT_ASSIGNMENT_18_ACTION_SETf);
        }

        /* Set UDP Src Port */
        if (SOC_MEM_FIELD_VALID(unit,mem_view_id, 
                             DIRECT_ASSIGNMENT_19_ACTION_SETf)) {
            info->udp_src_port = soc_mem_field32_get(unit, mem_view_id, 
                         tnl_entry, DIRECT_ASSIGNMENT_19_ACTION_SETf);
        }

        /* FRAGMENT ID */
        if (SOC_MEM_FIELD_VALID(unit,mem_view_id,
                         SEQUENCE_NUMBER_ACTION_SETf)) {
            uint32 seq_num;
            uint32 inx;
 
            seq_num = soc_mem_field32_get(unit, mem_view_id, tnl_entry, 
                            SEQUENCE_NUMBER_ACTION_SETf);
            inx = soc_format_field32_get(unit,SEQUENCE_NUMBER_ACTION_SETfmt,
                                  &seq_num, SEQ_NUM_COUNTER_INDEXf);
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_SEQUENCE_NUMBER_TABLEm,
                               MEM_BLOCK_ANY, inx, &entry));
            info->ip4_id = soc_mem_field32_get(unit, EGR_SEQUENCE_NUMBER_TABLEm,
                                     &entry, SEQUENCE_NUMBERf);
        }

    }

    if ((fixed_view == FALSE) && (field != NULL) && (num_of_fields)) {
        rv = soc_flow_db_mem_view_field_list_get(unit,
                      mem_view_id,
                      SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                      _BCM_FLOW_LOGICAL_FIELD_MAX, data_ids, &cnt);
        SOC_IF_ERROR_RETURN(rv);

        if (field_cnt == NULL) { 
            for (i = 0; i < num_of_fields; i++) {
                for (j = 0; j < cnt; j++) {
                    if (field[i].id == data_ids[j]) {
                        field[i].value = soc_mem_field32_get(unit,mem_view_id,
                                             tnl_entry, field[i].id);
                        break;
                    }
                }
            }
        } else {
            *field_cnt = cnt < num_of_fields? cnt: num_of_fields;
            if (cnt > num_of_fields) {
                cnt = num_of_fields;
            }
            for (i = 0; i < cnt; i++) {
                field[i].id = data_ids[i];
                field[i].value = soc_mem_field32_get(unit,mem_view_id, tnl_entry,
                         field[i].id);
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_tunnel_init_set
 * Purpose:
 *      Add tunnel initiator entry to hw.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      idx      - (IN)Index to insert tunnel initiator.
 *      info     - (IN)Tunnel initiator entry info.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_flow_tunnel_init_set(int unit, 
    int idx, 
    bcm_flow_tunnel_initiator_t *info,
    bcm_flow_logical_field_t *field,
    int num_of_fields,
    int *tnl_in_oif)
{
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to write hw entry */
    soc_mem_t mem;                        /* Tunnel initiator table memory */
    soc_mem_t mem_view;
    int rv = BCM_E_NONE;                  /* Return value                  */
    int df_val=0;                           /* Don't fragment encoding       */
    uint32 sip6[4];
    uint32 dip6[4];
    uint32 data_ids[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32 cnt;
    int i;
    int j;
    int ipv6;
    int entry_type = BCM_XGS3_TUNNEL_INIT_V4;
    uint16 random;
    egr_sequence_number_table_entry_t entry;
    _bcm_flow_bookkeeping_t *flow_info;
    int dvp = 0;
    int tnl_type = 0;
    bcm_ip6_t sid_list[2];
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
    egr_dvp_attribute_entry_t *dvp_entry;
    int old_dvp = 0;

    flow_info = FLOW_INFO(unit);

    if (info->type == bcmTunnelTypeSR6 || info->type == bcmTunnelTypeEthSR6) {
        sal_memcpy(&sid_list[0], &info->dip6, sizeof(bcm_ip6_t));
        sal_memset(&sid_list[1], 0, sizeof(bcm_ip6_t));
        dvp_entry = (egr_dvp_attribute_entry_t *)tnl_entry;
        sal_memset(dvp_entry, 0, sizeof(egr_dvp_attribute_entry_t));
        sal_memset(&vc_entry, 0, sizeof(vc_entry));

        if (info->type == bcmTunnelTypeSR6) {

            tnl_type = _BCM_FLOW_SRV6_SRH_TYPE_L3;
            rv = _bcm_flow_tunnel_srv6_dvp_vc_swap_init(unit, tnl_type,
                           sid_list, 0, &vc_entry,
                           dvp_entry);
            BCM_IF_ERROR_RETURN(rv);
            if (info->flags & BCM_TUNNEL_REPLACE) {
                rv = _bcm_flow_srv6_l3_tnl_object_ctrl(unit,
                        TNL_SRV6_ACTION_DVP_IDX_GET,
                        info->l3_intf_id - BCM_XGS3_EGRESS_IDX_MIN(unit),
                        &old_dvp, 0, NULL);
                BCM_IF_ERROR_RETURN(rv);
                rv = soc_mem_read(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                               MEM_BLOCK_ALL,
                               old_dvp, &vc_entry);
                BCM_IF_ERROR_RETURN(rv);
            }
            /* allocate joined dvp and vc_swap entry */
            rv = _bcm_flow_dvp_vc_swap_entry_index_alloc(unit, &vc_entry,
                      dvp_entry, TRUE, &dvp);
            BCM_IF_ERROR_RETURN(rv);

            if (info->flags & BCM_TUNNEL_REPLACE) {
                if (old_dvp) {
                    rv = _bcm_flow_dvp_vc_swap_entry_index_free(unit, NULL,
                              TRUE, old_dvp);
                    BCM_IF_ERROR_RETURN(rv);
                }
            }
            rv = _bcm_flow_srv6_l3_tnl_object_ctrl(unit,
                        TNL_SRV6_ACTION_DVP_ATTACH,
                        info->l3_intf_id - BCM_XGS3_EGRESS_IDX_MIN(unit),
                        &dvp, 0, NULL);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_flow_srv6_l3_tnl_object_ctrl(unit,
                        TNL_SRV6_ACTION_TNL_ID_ATTACH,
                        info->l3_intf_id - BCM_XGS3_EGRESS_IDX_MIN(unit),
                        NULL, idx, NULL);
            BCM_IF_ERROR_RETURN(rv);

            /* write to the dvp */
            rv = soc_mem_write(unit, EGR_DVP_ATTRIBUTEm, MEM_BLOCK_ALL,
                               dvp, dvp_entry);
            BCM_IF_ERROR_RETURN(rv);

            /* write to vc_and_swap */
            rv = soc_mem_write(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                            MEM_BLOCK_ALL, dvp, &vc_entry);
            BCM_IF_ERROR_RETURN(rv);

        } else {
            dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
            if (!(info->flags & BCM_TUNNEL_REPLACE)) {
                tnl_type = _BCM_FLOW_SRV6_SRH_TYPE_L2;

                /* vc_swap_entry may be initialized in encap_set */
                rv = soc_mem_read(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                               MEM_BLOCK_ALL,
                               dvp, &vc_entry);
                BCM_IF_ERROR_RETURN(rv);
                rv = _bcm_flow_tunnel_srv6_dvp_vc_swap_init(unit, tnl_type,
                           sid_list, 0, &vc_entry, NULL);
                BCM_IF_ERROR_RETURN(rv);

                /* write to vc_and_swap */
                rv = soc_mem_write(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                MEM_BLOCK_ALL, dvp, &vc_entry);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
    }

    if (info->type != bcmTunnelTypeSR6) {
        rv = _bcm_flow_tunnel_dip_idx_set(unit, info, idx, tnl_in_oif);
        BCM_IF_ERROR_RETURN(rv);
    }

    ipv6 = _BCM_TUNNEL_OUTER_HEADER_IPV6(info->type) ||
                      info->type == bcmTunnelTypeSR6 ||
                      info->type == bcmTunnelTypeEthSR6;
    if (ipv6) {
        idx >>= 1;
        mem = BCM_XGS3_L3_MEM(unit, tnl_init_v6);
    } else {
        mem =  BCM_XGS3_L3_MEM(unit, tnl_init_v4);
    }
    if (!SOC_MEM_IS_VALID(unit, mem)) {
        return (BCM_E_UNAVAIL);
    }

    /* get the physical memory view */
    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        rv = soc_flow_db_ffo_to_mem_view_id_get(unit,info->flow_handle,
                   info->flow_option, 
                   SOC_FLOW_DB_FUNC_TUNNEL_INITIATOR_ID,
                    (uint32 *)&mem_view);
        BCM_IF_ERROR_RETURN(rv);
    }

    soc_mem_lock(unit,mem);
    if (info->flags & BCM_TUNNEL_REPLACE) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, tnl_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit,mem);
            return rv;
        }
    } else {
        /* Zero write buffer. */
        sal_memset(tnl_entry, 0, sizeof(tnl_entry));
        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            soc_flow_db_mem_view_entry_init(unit,mem_view,tnl_entry);
        }
    }

    /* flex view doesn't seem to have these functions */
    if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {

       if ((!ipv6) && SOC_MEM_FIELD_VALID(unit, mem, IPV4_DF_SELf)) {
           /* Outer IPv4 tunnel hdr DF bit for Inner IPv4 hdr. */
           df_val = 0;
           if (info->flags & BCM_TUNNEL_INIT_USE_INNER_DF) {
               df_val |= 0x2;
           } else if (info->flags & BCM_TUNNEL_INIT_IPV4_SET_DF) {
               df_val |= 0x1;
           }
           soc_mem_field32_set(unit, mem, tnl_entry, IPV4_DF_SELf, df_val);
        }

        if ((!ipv6) && SOC_MEM_FIELD_VALID(unit, mem, IPV6_DF_SELf)) {
            /* Outer IPv4 tunnel hdr DF bit for Inner IPv6 hdr. */
            df_val = (info->flags & BCM_TUNNEL_INIT_IPV6_SET_DF) ? 0x1 : 0;
            soc_mem_field32_set(unit, mem, tnl_entry, IPV6_DF_SELf, df_val);
        }

    }

    if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {

        if (ipv6) {
            if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_DIP6_VALID) {
                /* Set destination ipv6 address. */
                soc_mem_ip6_addr_set(unit, mem, tnl_entry, DIPf, info->dip6,
                                     SOC_MEM_IP6_FULL_ADDR);
            }

            if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_SIP6_VALID) {
                /* Set source ipv6 address. */
                soc_mem_ip6_addr_set(unit, mem, tnl_entry, SIPf, info->sip6,
                                 SOC_MEM_IP6_FULL_ADDR);
            }
        } else {
            if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_DIP_VALID) {
                /* Set destination address. */
                soc_mem_field_set(unit, mem, tnl_entry, DIPf, 
                                               (uint32 *)&info->dip);
            }

            if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_SIP_VALID) {
                /* Set source address. */
                soc_mem_field_set(unit, mem, tnl_entry, SIPf, 
                                               (uint32 *)&info->sip);
            }
        }

        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_DSCP_VALID) {
            /* Tunnel header DSCP select. */
            soc_mem_field32_set(unit, mem, tnl_entry, DSCP_SELf,info->dscp_sel);

            if (info->dscp_sel == bcmTunnelDscpMap) {
                /* Set DSCP_MAP value. */
                int hw_map_idx = 0;
                rv = _bcm_tr2_qos_id2idx(unit, info->dscp_map, &hw_map_idx);
                if (SOC_FAILURE(rv)) {
                    soc_mem_unlock(unit,mem);
                    return rv;
                }
            
                soc_mem_field32_set(unit, mem, tnl_entry,
                            DSCP_MAPPING_PTRf, hw_map_idx);
            } else {
                /* Set DSCP value. */
                soc_mem_field32_set(unit, mem, tnl_entry, DSCPf, info->dscp);
            }
        }

        /* Set TTL. */
        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_TTL_VALID) {
            soc_mem_field32_set(unit, mem, tnl_entry, TTLf, info->ttl);
        }

        /* Set UDP Dest Port */
        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_UDP_DPORT_VALID) {
            soc_mem_field32_set(unit, mem, tnl_entry, L4_DEST_PORTf, 
                            info->udp_dst_port);
        }

        /* Set UDP Src Port */
        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_UDP_SPORT_VALID) {
            soc_mem_field32_set(unit, mem, tnl_entry, L4_SRC_PORTf, 
                                info->udp_src_port);
        }

        /* Set destination mac address. */
        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_DMAC_VALID) {
            if (SOC_MEM_FIELD_VALID(unit, mem, DEST_ADDRf)) {
                soc_mem_mac_addr_set(unit, mem, &tnl_entry, DEST_ADDRf, 
                                     info->dmac);
            }
        }

        /* IPv4 Fragment ID  */
        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_IP4_ID_VALID) {
            sal_memset(&entry,0,sizeof(egr_sequence_number_table_entry_t));
            if (info->flags & BCM_TUNNEL_INIT_IP4_ID_SET_FIXED) {
                soc_EGR_SEQUENCE_NUMBER_TABLEm_field32_set(unit, &entry,
                                    SEQUENCE_NUMBERf, info->ip4_id);
            } else if (info->flags & BCM_TUNNEL_INIT_IP4_ID_SET_RANDOM) {
                random = (uint16) (sal_time_usecs() & 0xFFFF);
                soc_EGR_SEQUENCE_NUMBER_TABLEm_field32_set(unit, &entry,
                                               SEQUENCE_NUMBERf, random);
            }
            rv = soc_mem_write(unit, EGR_SEQUENCE_NUMBER_TABLEm, 
                    MEM_BLOCK_ANY, idx + flow_info->frag_base_inx, &entry);
            if (SOC_FAILURE(rv)) {
                soc_mem_unlock(unit,mem);
                return rv;
            }
        }
 
        if (info->type == bcmTunnelTypeVxlan) {
            /* Set Tunnel type = VXLAN */
            soc_mem_field32_set(unit, mem, tnl_entry, TUNNEL_TYPEf, 
                                  _BCM_FLOW_VXLAN_TUNNEL_TYPE);
        } else if (info->type == bcmTunnelTypeL2Gre) {
            /* Set Tunnel type = L2GRE */
            soc_mem_field32_set(unit, mem, tnl_entry, TUNNEL_TYPEf, 
                                  _BCM_FLOW_L2GRE_TUNNEL_TYPE);
        } else {
            int hw_code;

            rv = _bcm_trx_tnl_type_to_hw_code(unit, info->type,
                                 &hw_code, &entry_type);
            if (SOC_FAILURE(rv)) {
                soc_mem_unlock(unit,mem);
                return rv;
            }
            soc_mem_field32_set(unit, mem, tnl_entry, TUNNEL_TYPEf, 
                                  hw_code);

        }

        /* Set entry type = IPv4 */
        if (entry_type == BCM_XGS3_TUNNEL_INIT_V6) {
            soc_mem_field32_set(unit, mem, tnl_entry, DATA_TYPEf, 
                        _BCM_FLOW_TUNNEL_DATA_TYPE_IPV6);
        } else {   /* ipv4 */
            soc_mem_field32_set(unit, mem, tnl_entry, DATA_TYPEf, 
                         _BCM_FLOW_TUNNEL_DATA_TYPE_IPV4);
        }
       
    } else {  /* flex view */
        uint32 qos = 0;
        uint32 ttl = 0;

        if (info->type == bcmTunnelTypeL3Flex) {
            if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_DIP_VALID) {
                if (SOC_MEM_FIELD_VALID(unit,mem_view,
                             DIRECT_ASSIGNMENT_10_ACTION_SETf)) {
                    soc_mem_field32_set(unit, mem_view, tnl_entry,
                                DIRECT_ASSIGNMENT_10_ACTION_SETf, info->dip);
                }
            }  
            if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_DIP6_VALID) {
     
                SAL_IP6_ADDR_TO_UINT32(info->dip6,dip6);

                /* OUTER_IPV6_DIP_0_31 */
                if (SOC_MEM_FIELD_VALID(unit,mem_view,
                             DIRECT_ASSIGNMENT_10_ACTION_SETf)) {
                    soc_mem_field32_set(unit, mem_view, tnl_entry,
                        DIRECT_ASSIGNMENT_10_ACTION_SETf, dip6[0]);
                }

                /* OUTER_IPV6_DIP_32_63 */
                if (SOC_MEM_FIELD_VALID(unit,mem_view,
                             DIRECT_ASSIGNMENT_12_ACTION_SETf)) {
                    soc_mem_field32_set(unit, mem_view, tnl_entry,
                        DIRECT_ASSIGNMENT_12_ACTION_SETf, dip6[1]);
                }

                /* OUTER_IPV6_DIP_64_95 */
                if (SOC_MEM_FIELD_VALID(unit,mem_view,
                             DIRECT_ASSIGNMENT_14_ACTION_SETf)) {
                    soc_mem_field32_set(unit, mem_view, tnl_entry,
                        DIRECT_ASSIGNMENT_14_ACTION_SETf, dip6[2]);
                }

                /* OUTER_IPV6_DIP_96_127 */
                if (SOC_MEM_FIELD_VALID(unit,mem_view,
                             DIRECT_ASSIGNMENT_16_ACTION_SETf)) {
                    soc_mem_field32_set(unit, mem_view, tnl_entry,
                        DIRECT_ASSIGNMENT_16_ACTION_SETf, dip6[3]);
                }
            }
        }
        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_SIP_VALID) {
            soc_mem_field32_set(unit, mem_view, tnl_entry,
                DIRECT_ASSIGNMENT_11_ACTION_SETf, info->sip);
        }
        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_SIP6_VALID) {
            SAL_IP6_ADDR_TO_UINT32(info->sip6,sip6);
            soc_mem_field32_set(unit, mem_view, tnl_entry,
                DIRECT_ASSIGNMENT_11_ACTION_SETf, sip6[0]);
            soc_mem_field32_set(unit, mem_view, tnl_entry,
                DIRECT_ASSIGNMENT_13_ACTION_SETf, sip6[1]);
            soc_mem_field32_set(unit, mem_view, tnl_entry,
                DIRECT_ASSIGNMENT_15_ACTION_SETf, sip6[2]);
            soc_mem_field32_set(unit, mem_view, tnl_entry,
                DIRECT_ASSIGNMENT_17_ACTION_SETf, sip6[3]);
        } 

        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_DSCP_VALID) {
            int sel_value;

            if (info->dscp_sel == bcmTunnelDscpAssign) {
                sel_value = _BCM_FLOW_DSCP_SEL_ASSIGN_FROM_TABLE; 
            } else if (info->dscp_sel == bcmTunnelDscpPacket) {
                sel_value = _BCM_FLOW_DSCP_SEL_COPY_FROM_ZONE; 
            } else if (info->dscp_sel == bcmTunnelDscpMap) {
                sel_value = _BCM_FLOW_DSCP_SEL_MAP_FROM_PHB; 
            } else {
                sel_value = 0; /* do not modify */
            }
            soc_format_field32_set(unit,QOS_A_ACTION_SETfmt,
                               &qos,QOS_ACTION_SELf, sel_value);
            if (info->dscp_sel == bcmTunnelDscpMap) {
                /* Set DSCP_MAP value. */
                int hw_map_idx = 0;
                rv = _bcm_tr2_qos_id2idx(unit, info->dscp_map, &hw_map_idx);
                if (SOC_FAILURE(rv)) {
                    soc_mem_unlock(unit,mem);
                    return rv;
                }
                soc_format_field32_set(unit,QOS_A_ACTION_SETfmt,
                               &qos,QOS_MAPPING_PTRf, hw_map_idx);
            } else if (info->dscp_sel == bcmTunnelDscpPacket) {
                /* Set Src ZONE. */
                soc_format_field32_set(unit,QOS_A_ACTION_SETfmt,
                                   &qos, ZONE_IDf, 4);
            } else {
                /* Set DSCP value. */
                soc_format_field32_set(unit,QOS_A_ACTION_SETfmt,
                                   &qos,QOS_VALUEf, info->dscp);
            }
            soc_mem_field32_set(unit, mem_view, tnl_entry, QOS_A_ACTION_SETf, qos);
        }

        /* Set TTL. */
        soc_format_field32_set(unit,TTL_B_ACTION_SETfmt,
                           &ttl,TTL_ACTION_TYPEf, 1);
        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_TTL_VALID) {
            soc_format_field32_set(unit,TTL_B_ACTION_SETfmt,
                               &ttl,TTL_VALUEf, info->ttl);
        } else {
            soc_format_field32_set(unit,TTL_B_ACTION_SETfmt,
                               &ttl,TTL_VALUEf, 0xff);
        }
        soc_mem_field32_set(unit, mem_view, tnl_entry, TTL_B_ACTION_SETf, ttl);

        /* Set UDP Dest Port */
        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_UDP_DPORT_VALID) {
            soc_mem_field32_set(unit, mem_view, tnl_entry, 
                         DIRECT_ASSIGNMENT_18_ACTION_SETf, info->udp_dst_port);
        }

        /* Set UDP Src Port */
        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_UDP_SPORT_VALID) {
            soc_mem_field32_set(unit, mem_view, tnl_entry, 
                         DIRECT_ASSIGNMENT_19_ACTION_SETf, info->udp_src_port);
        }

        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_IP4_ID_VALID) {
            uint32 seq_num = 0;

            /* Use the same tunnel base as for the fixed view */
            soc_format_field32_set(unit,SEQUENCE_NUMBER_ACTION_SETfmt,
                              &seq_num, SEQ_NUM_PROFILE_INDEXf, 1);
            soc_format_field32_set(unit,SEQUENCE_NUMBER_ACTION_SETfmt,
                                  &seq_num, SEQ_NUM_COUNTER_INDEXf, 
                                  idx + flow_info->frag_base_inx);
            soc_mem_field32_set(unit, mem_view, tnl_entry, 
                         SEQUENCE_NUMBER_ACTION_SETf, seq_num);
            sal_memset(&entry,0,sizeof(egr_sequence_number_table_entry_t));
            if (info->flags & BCM_TUNNEL_INIT_IP4_ID_SET_FIXED) {
                soc_EGR_SEQUENCE_NUMBER_TABLEm_field32_set(unit, &entry,
                                   SEQUENCE_NUMBERf, info->ip4_id);
            } else if (info->flags & BCM_TUNNEL_INIT_IP4_ID_SET_RANDOM) {
                random = (uint16) (sal_time_usecs() & 0xFFFF);
                soc_EGR_SEQUENCE_NUMBER_TABLEm_field32_set(unit, &entry,
                                   SEQUENCE_NUMBERf, random);
            }
            rv = soc_mem_write(unit, EGR_SEQUENCE_NUMBER_TABLEm,
                    MEM_BLOCK_ANY, idx + flow_info->frag_base_inx, &entry);
            if (SOC_FAILURE(rv)) {
                soc_mem_unlock(unit,mem);
                return rv;
            }
        }
 
        if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_FLEX_VALID) {
            rv = soc_flow_db_mem_view_field_list_get(unit,
                      mem_view,
                      SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                      _BCM_FLOW_LOGICAL_FIELD_MAX, data_ids, &cnt);
            if (SOC_FAILURE(rv)) {
                soc_mem_unlock(unit,mem);
                return rv;
            }
            for (i = 0; i < num_of_fields; i++) {
                for (j = 0; j < cnt; j++) {
                    if (field[i].id == data_ids[j]) {
                        soc_mem_field32_set(unit,mem_view, tnl_entry,
                              field[i].id, field[i].value);
                        break;
                    }
                }
            }
        }
    }

    /* Write buffer to hw. */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, tnl_entry);
    soc_mem_unlock(unit,mem);
    return rv;
}

/*
 * Function:
 *      _bcm_flow_tunnel_and_oif_index_get
 * Purpose:
 *      Get hardware tunnel index and l3 interface index from tunnel_id
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_flow_tunnel_and_oif_index_get(int unit,
               bcm_gport_t tunnel_id,
               int *oif_idx,
               int *tnl_idx)
{
    int soft_tnl_idx;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int nh_idx;
    soc_mem_t nh_mem = EGR_L3_NEXT_HOPm;
    int rv;
    int data_type;
    uint32 view_id;
    _bcm_flow_bookkeeping_t *flow_info;
    uint32 action_set;

    flow_info = FLOW_INFO(unit);
    soft_tnl_idx = BCM_GPORT_TUNNEL_ID_GET(tunnel_id);
    *oif_idx = 0;
    *tnl_idx = 0;

    if (soft_tnl_idx < _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit)) {
        *tnl_idx = flow_info->init_tunnel[soft_tnl_idx].idx;
        if (*tnl_idx >= _BCM_FLOW_TUNNEL_IP_TUNNEL_MAX(unit)) {
            /* l3_oif index */
            *oif_idx = *tnl_idx - _BCM_FLOW_TUNNEL_IP_TUNNEL_MAX(unit);
            BCM_IF_ERROR_RETURN(_bcm_flow_tunnel_idx_from_oif_get(unit,
                                     *oif_idx, tnl_idx));
        }
    } else {
        nh_idx = soft_tnl_idx - _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit);
        rv = soc_mem_read(unit, nh_mem, MEM_BLOCK_ALL, nh_idx, entry);
        BCM_IF_ERROR_RETURN(rv);
        data_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, entry, DATA_TYPEf);
        rv = soc_flow_db_mem_to_view_id_get(unit,
                             nh_mem,
                             SOC_FLOW_DB_KEY_TYPE_INVALID,
                             data_type,
                             0,
                             NULL,
                             &view_id);
        BCM_IF_ERROR_RETURN(rv);
        if (!SOC_MEM_FIELD_VALID(unit,view_id,INTF_ACTION_SETf)) {
            return SOC_E_NOT_FOUND;
        }
        action_set = soc_mem_field32_get(unit, view_id, entry,
                                INTF_ACTION_SETf);
        *oif_idx = soc_format_field32_get(unit, INTF_ACTION_SETfmt,
                           &action_set, INTF_PTRf);
        BCM_IF_ERROR_RETURN(_bcm_flow_tunnel_idx_from_oif_get(unit,
                                     *oif_idx, tnl_idx));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_tunnel_initiator_entry_add
 * Purpose:
 *      Configure VXLAN Tunnel initiator hardware Entry
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_flow_tunnel_initiator_entry_add(int unit, 
     uint32 flags, 
     bcm_flow_tunnel_initiator_t *info, 
    bcm_flow_logical_field_t *field,
    uint32 num_of_fields)
{
    int tnl_idx;
    int rv = BCM_E_NONE;
    _bcm_flow_bookkeeping_t *flow_info;
    int dvp;
    int soft_tnl_idx = 0;
    int tnl_in_oif;
    int oif_idx;

    flow_info = FLOW_INFO(unit);

    if (info->flags & BCM_TUNNEL_REPLACE) {
        soft_tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
        rv = _bcm_flow_tunnel_and_oif_index_get(unit, info->tunnel_id,
               &oif_idx, &tnl_idx);
        BCM_IF_ERROR_RETURN(rv);
        if (!info->l3_intf_id) {
            info->l3_intf_id = oif_idx;
        }

        /* cannot modify if this tunnel is used more than once and not identical*/
        if (BCM_XGS3_L3_ENT_REF_CNT(BCM_XGS3_L3_TBL_PTR(unit, tnl_init),tnl_idx) > 1) {
            int cmp_result;
            uint32 mem_view_id;
            _bcm_flow_tunnel_record_t tnl_rec;

            tnl_rec.info = info;
            tnl_rec.field = field;
            tnl_rec.num_of_fields = num_of_fields;

            if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
                BCM_IF_ERROR_RETURN(soc_flow_db_ffo_to_mem_view_id_get(unit,info->flow_handle,
                    info->flow_option,
                    SOC_FLOW_DB_FUNC_TUNNEL_INITIATOR_ID,
                    (uint32 *)&mem_view_id));
                tnl_rec.mem_view = mem_view_id;
            }
            /* Compare if the tunnel table remains the same */
            BCM_IF_ERROR_RETURN(_bcm_flow_tunnel_initiator_cmp(unit,
                (void *)&tnl_rec, tnl_idx, &cmp_result));

            if (cmp_result != BCM_L3_CMP_EQUAL) {
                return BCM_E_PARAM;
            }
        }
    }

    rv = _bcm_flow_tnl_init_add(unit, flags, info, field,
                                num_of_fields, NULL, NULL, &tnl_idx);
    if (BCM_FAILURE(rv)) {
          return rv;
    }
    /* Write the entry to HW */
    rv = _bcm_flow_tunnel_init_set(unit, tnl_idx, info,field,num_of_fields,
                      &tnl_in_oif);
    if (BCM_FAILURE(rv)) {
        flags = _BCM_L3_SHR_WRITE_DISABLE;
        (void) bcm_xgs3_tnl_init_del(unit, flags, tnl_idx);
        BCM_IF_ERROR_RETURN(rv);
    }
    if (_BCM_FLOW_TUNNEL_TYPE_L2(info->type)) {
        if (!(info->flags & BCM_TUNNEL_REPLACE)) {
            dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
            soft_tnl_idx = dvp;
        }
        if (tnl_in_oif == TRUE) { 
            /* tunnel index in oif, save oif instead */
            flow_info->init_tunnel[soft_tnl_idx].idx = 
                _BCM_FLOW_TUNNEL_IP_TUNNEL_MAX(unit) + info->l3_intf_id;
        } else {    
            flow_info->init_tunnel[soft_tnl_idx].idx = tnl_idx;  
        }
    } else { /* L3 tunnel */
        if (!(info->flags & BCM_TUNNEL_REPLACE)) {
            if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->l3_intf_id)) {
                soft_tnl_idx = _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit) +
                       info->l3_intf_id - BCM_XGS3_EGRESS_IDX_MIN(unit);
            } else {
                soft_tnl_idx = _BCM_FLOW_TUNNEL_L2_MAX(unit) + info->l3_intf_id;
            }
        }
        flow_info->init_tunnel[soft_tnl_idx].idx = tnl_idx;
    }
    BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id,soft_tnl_idx);
    return rv;
}

/*
 * Function:
 *      _bcm_esw_flow_tnl_initiator_validate
 * Purpose:
 *      Validate  the tunnel initiator attributes
 *
 * Parameters:
 *      unit          - (IN) Device Number
 *      info          - (IN) Flow tunnel initiator descriptor
 * Returns:
 *      TRUE/FALSE
 */
STATIC
int _bcm_esw_flow_tnl_initiator_validate(
    int unit,
    bcm_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    _bcm_flow_bookkeeping_t *flow_info;
    int soft_tnl_idx;
    int dvp;

    if (NULL == info) {
        return (BCM_E_PARAM);
    }
    if ((!_BCM_FLOW_TUNNEL_TYPE_L2(info->type)) &&
        (!_BCM_FLOW_TUNNEL_TYPE_L3(info->type))) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
         "Flow Tunnel Initiation: Invalid tunnel type.\n")));
        return BCM_E_PARAM;
    }

    flow_info = FLOW_INFO(unit);
    if (((info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) &&
        (info->type != bcmTunnelTypeVxlan)) ||
        ((info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) &&
        (info->type != bcmTunnelTypeL2Gre)) ||
        ((info->flow_handle >= SOC_FLOW_DB_FLOW_ID_START) &&
        ((info->type == bcmTunnelTypeL2Gre) ||
        (info->type == bcmTunnelTypeVxlan)))) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
        "Flow Tunnel Initiation: tunnel type and flow handle mismatch.\n")));
        return BCM_E_PARAM;
    }

    /* validate SRv6 handle and type */
    if (info->type == bcmTunnelTypeSR6 || info->type == bcmTunnelTypeEthSR6) {
        if (flow_info->flow_srv6 != info->flow_handle) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
             "Flow Tunnel Initiation: SRv6 type and flow handle mismatch.\n")));
            return BCM_E_PARAM;
        }
    }
    if (flow_info->flow_srv6 && (flow_info->flow_srv6 == info->flow_handle)) {
        if (!(info->type == bcmTunnelTypeSR6 ||
              info->type == bcmTunnelTypeEthSR6)) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
             "Flow Tunnel Initiation: SRv6 type and flow handle mismatch.\n")));
            return BCM_E_PARAM;
        }
    }

    if (info->flags & BCM_TUNNEL_REPLACE) {
        /* validate the tunnel index */
        soft_tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
        _BCM_FLOW_TUNNEL_ID_IF_INVALID_RETURN(unit, soft_tnl_idx);

        /* check if tunnel exist */
        if (!flow_info->init_tunnel[soft_tnl_idx].idx) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                   "Invalid replace action, tunnel not created\n")));
            return BCM_E_PARAM;
        }

    } else {
        if (_BCM_FLOW_TUNNEL_TYPE_L2(info->type)) {
            /* check the dvp is in range and already created and dvp_encap is set
             * The dvp entry should be already initialized. flow_port_encap_set
             * should be called first
             */
            dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
            _BCM_FLOW_TUNNEL_ID_IF_INVALID_RETURN(unit, dvp);

            if (!_BCM_FLOW_EGR_DVP_USED_GET(unit,dvp)) {
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                   "dvp(%d) type not set, call bcm_flow_port_encap_set first\n"),
                     dvp));
                return BCM_E_PARAM;
            }
            if (flow_info->init_tunnel[dvp].idx) {
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                   "tunnel already created, use replace action to change\n")));
                return BCM_E_PARAM;
            }
        } else {  /* L3 tunnel */
            if (info->type == bcmTunnelTypeSR6) {
                /* intf_id should be egress object */
                if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->l3_intf_id)) {
                    LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                       "Invalid l3 egress object ID\n")));
                    return BCM_E_PARAM;
                }
                soft_tnl_idx = _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit) +
                       info->l3_intf_id - BCM_XGS3_EGRESS_IDX_MIN(unit);
                /* tunnel already exists */
                if (flow_info->init_tunnel[soft_tnl_idx].idx) {
                    LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                       "L3 tunnel exists, use replace action to change\n")));
                    return BCM_E_PARAM;
                }
            } else {
                /* make sure intf_id is valid */
                if (info->l3_intf_id >= _BCM_FLOW_TUNNEL_L3_MAX(unit)) {
                    LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                       "Invalid l3 interface ID\n")));
                    return BCM_E_PARAM;
                }
                soft_tnl_idx = _BCM_FLOW_TUNNEL_L2_MAX(unit) + info->l3_intf_id;

                /* tunnel already exists */
                if (flow_info->init_tunnel[soft_tnl_idx].idx) {
                    LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                       "L3 tunnel exists, use replace action to change\n")));
                    return BCM_E_PARAM;
                }
            }
        }
    }
    if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_TTL_VALID) {
        if (!BCM_TTL_VALID(info->ttl)) {
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_DSCP_VALID) {
        if (info->dscp_sel > 2 || info->dscp_sel < 0) {
            return BCM_E_PARAM;
        }

        if (info->dscp > 63 || info->dscp < 0) {
            return BCM_E_PARAM;
        }
    }

    if (info->valid_elements & BCM_FLOW_TUNNEL_INIT_FLEX_VALID) {
        if ((field == NULL) || (!num_of_fields)) {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_flow_tunnel_initiator_create
 * Purpose:
 *      Create a flow tunnel initiator object on the given flow port for L2
 * tunnel or on the given L3 interface for L3 tunnel.
 *
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN/OUT) Flow tunnel initiator descriptor
 *      num_of_fields - (IN) number of logical fields
 *      field         - (IN) logical field array
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_esw_flow_tunnel_initiator_create(
    int unit,
    bcm_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    uint32 flags=0;
    int soft_tnl_idx;
    uint32 flow_option;
    bcm_flow_tunnel_initiator_t linfo; /* local copy of the parameter info */
    uint32 mpath_flag=0;
    int nh_idx = -1;
    int ref_count;
    _bcm_flow_bookkeeping_t *flow_info;

    flow_info = FLOW_INFO(unit);

    rv = bcmi_esw_flow_lock(unit);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_esw_flow_tnl_initiator_validate(unit,
               info, num_of_fields, field);
    _BCM_FLOW_CLEANUP(rv);

    linfo = *info;

    if (info->flags & BCM_TUNNEL_REPLACE) {
        soft_tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);

        /* l2 tunnel */
        if (soft_tnl_idx < _BCM_FLOW_TUNNEL_L2_MAX(unit)) {
            /* flow_port cannot be replaced */
            _SHR_GPORT_FLOW_PORT_ID_SET(linfo.flow_port,soft_tnl_idx);

        } else if (soft_tnl_idx <
                          _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit)) {
            /* L3 tunnel created with l3_intf */
            linfo.l3_intf_id = soft_tnl_idx - _BCM_FLOW_TUNNEL_L2_MAX(unit);

        } else {
            /* L3 tunnel created with egr_obj*/
            linfo.l3_intf_id = soft_tnl_idx -
                         _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit) +
                         BCM_XGS3_EGRESS_IDX_MIN(unit);
        }
    }

    if (info->type == bcmTunnelTypeSR6 || info->type == bcmTunnelTypeEthSR6) {
        if (info->type == bcmTunnelTypeEthSR6) {
            flow_option = flow_info->option_id_l2tnl;
        } else {
            flow_option = flow_info->option_id_l3tnl;

            if (!(info->flags & BCM_TUNNEL_REPLACE)) {
                BCM_XGS3_L3_MODULE_LOCK(unit);
                /* increment the egr_obj reference count */
                rv = bcm_xgs3_get_nh_from_egress_object(unit, info->l3_intf_id,
                                     &mpath_flag, 1, &nh_idx);
                BCM_XGS3_L3_MODULE_UNLOCK(unit);
                _BCM_FLOW_CLEANUP(rv);
            }
        }
        if (!info->flow_option) {
            linfo.flow_option = flow_option;
        }
    }

    flags =  _BCM_L3_SHR_WRITE_DISABLE | _BCM_L3_SHR_SKIP_INDEX_ZERO;

    if (info->flags & BCM_TUNNEL_REPLACE) {
        flags |= _BCM_L3_SHR_UPDATE | _BCM_L3_SHR_WITH_ID;
    }  
    
    /* Allocate either existing or new tunnel initiator entry */
    BCM_XGS3_L3_MODULE_LOCK(unit);
    rv = _bcm_flow_tunnel_initiator_entry_add(unit, flags, &linfo,
                                field,num_of_fields);
    BCM_XGS3_L3_MODULE_UNLOCK(unit);
    _BCM_FLOW_CLEANUP(rv);
    info->tunnel_id = linfo.tunnel_id;

    bcmi_esw_flow_unlock (unit);
    return BCM_E_NONE;

cleanup:
    if (nh_idx != -1) {
        (void)bcm_xgs3_get_ref_count_from_nhi(unit, 0, &ref_count, nh_idx);
    }
    bcmi_esw_flow_unlock (unit);
    return rv;
}

int
_bcm_flow_tunnel_egr_intf_used_count_get (
      int unit,
      int intf_id,  /* the given egress L3 interface ID used by tunnel */
      int *count)  /* return used count of this interface */
{
    int ix, iy;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    int rv;
    int data_type;
    uint32 action_set;
    uint32 nh_view_id;
    _bcm_flow_bookkeeping_t *flow_info;
    int nh_idx;
    int l3_oif;

    if (count == NULL) {
        return BCM_E_INTERNAL;
    }

    *count = 0;
    flow_info = FLOW_INFO(unit);

    /* check L2 tunnel first l3_oif */
    for (ix = 0; ix < _BCM_FLOW_TUNNEL_L2_MAX(unit); ix++) {
        if (flow_info->init_tunnel[ix].idx <
                         _BCM_FLOW_TUNNEL_IP_TUNNEL_MAX(unit)) {
            /* not use egr_l3_intf to attach tunnel */
            continue;
        }
        if (intf_id == (flow_info->init_tunnel[ix].idx -
                                _BCM_FLOW_TUNNEL_IP_TUNNEL_MAX(unit))) {
            *count += 1;
        }
    }

    /* now check egress object based L3 tunnel */
    mem = EGR_L3_NEXT_HOPm;
    for (ix = 0; ix < _BCM_FLOW_TUNNEL_EGR_OBJ_MAX(unit); ix++) {
        iy = ix + _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit);
        if (!flow_info->init_tunnel[iy].idx) {
            continue;
        }

        nh_idx = iy - _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit);
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, nh_idx, entry);
        BCM_IF_ERROR_RETURN(rv);

        data_type = soc_mem_field32_get(unit, mem, entry, DATA_TYPEf);
        rv = soc_flow_db_mem_to_view_id_get(unit,
                                 mem,
                                 SOC_FLOW_DB_KEY_TYPE_INVALID,
                                 data_type,
                                 0,
                                 NULL,
                                 &nh_view_id);
        if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
            return rv;
        } else if (rv == BCM_E_NOT_FOUND) {
            continue;
        }

        action_set = soc_mem_field32_get(unit, nh_view_id, entry,
                        INTF_ACTION_SETf);
        l3_oif = soc_format_field32_get(unit, INTF_ACTION_SETfmt,
                           &action_set, INTF_PTRf);
        if (l3_oif == intf_id) {
            *count += 1;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_flow_tunnel_initiator_destroy
 * Purpose:
 *      Delete the flow tunnel initiator object
 *
 * Parameters:
 *      unit    - (IN) Device Number
 *      flow_tunnel_id - (IN/OUT) Flow tunnel initiator object ID
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_esw_flow_tunnel_initiator_destroy(
    int unit,
    bcm_gport_t flow_tunnel_id)
{
    int soft_tnl_idx;
    _bcm_flow_bookkeeping_t *flow_info;
    bcm_flow_tunnel_initiator_t info;
    uint32 flags = 0;
    int idx;
    int rv;
    int oif_idx;
    _bcm_l3_tbl_op_t data;               /* Delete operation info. */
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to write hw entry */
    soc_mem_t mem;
    int data_type;
    int l3_oif_cnt = FALSE;
    int ref_count;
    egr_mpls_vc_and_swap_label_table_entry_t *vc_entry;
    int l3_lock = FALSE;

    mem = BCM_XGS3_L3_MEM(unit, tnl_init_v4);
    bcm_flow_tunnel_initiator_t_init(&info);
    flow_info = FLOW_INFO(unit);
    soft_tnl_idx = BCM_GPORT_TUNNEL_ID_GET(flow_tunnel_id);
    if (soft_tnl_idx < 0 || soft_tnl_idx >= _BCM_FLOW_TUNNEL_ID_MAX(unit)) {
        return BCM_E_PARAM;
    }

    if (soft_tnl_idx < _BCM_FLOW_TUNNEL_L2_MAX(unit)) {
        /* L2 tunnel */
        _SHR_GPORT_FLOW_PORT_ID_SET(info.flow_port,soft_tnl_idx);
        info.valid_elements = BCM_FLOW_TUNNEL_INIT_DIP_VALID |
                              BCM_FLOW_TUNNEL_INIT_DIP6_VALID;
        info.type = bcmTunnelTypeL2Flex;

    } else if (soft_tnl_idx < _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit)) {
        /* L3 tunnel */
        info.l3_intf_id = soft_tnl_idx - _BCM_FLOW_TUNNEL_L2_MAX(unit);
    } else if (soft_tnl_idx < _BCM_FLOW_TUNNEL_ID_MAX(unit)) {
        /* L3 tunnel */
        info.l3_intf_id = BCM_XGS3_EGRESS_IDX_MIN(unit) + soft_tnl_idx -
                          _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit);
    } else {
        return BCM_E_NOT_FOUND;
    }

    rv = bcmi_esw_flow_lock(unit);
    BCM_IF_ERROR_RETURN(rv);

    if (!flow_info->init_tunnel[soft_tnl_idx].idx) {
        rv = BCM_E_NOT_FOUND;
        goto cleanup;
    }

    idx = flow_info->init_tunnel[soft_tnl_idx].idx;

    if (idx >= _BCM_FLOW_TUNNEL_IP_TUNNEL_MAX(unit)) {
        /* l3_oif index */
        oif_idx = idx - _BCM_FLOW_TUNNEL_IP_TUNNEL_MAX(unit);
        info.l3_intf_id = oif_idx;
        rv = _bcm_flow_tunnel_idx_from_oif_get(unit, oif_idx, &idx);
        _BCM_FLOW_CLEANUP(rv);

        /* check if other tunnels also use the same l3_oif */
        rv = _bcm_flow_tunnel_egr_intf_used_count_get(unit, oif_idx,
                           &l3_oif_cnt);
        _BCM_FLOW_CLEANUP(rv);
    }
    if (!idx) {
        rv = BCM_E_NOT_FOUND;
        goto cleanup;
    }

    if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info.l3_intf_id)) {
        rv = _bcm_flow_tunnel_dip_idx_set(unit, &info,
                             l3_oif_cnt > 1? idx: 0, NULL);
        _BCM_FLOW_CLEANUP(rv);
    }

    BCM_XGS3_L3_MODULE_LOCK(unit);
    l3_lock = TRUE;
    flags = _BCM_L3_SHR_WRITE_DISABLE;

    /* Initialization */
    sal_memset(&data, 0, sizeof(_bcm_l3_tbl_op_t));

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx,tnl_entry);
    _BCM_FLOW_CLEANUP(rv);

    data_type = soc_mem_field32_get(unit, mem, tnl_entry, DATA_TYPEf);

    if (_BCM_FLOW_TUNNEL_ENTRY_DOUBLE_WIDE(data_type)) {
        data.width = _BCM_DOUBLE_WIDE;
    } else {
        /* Default is single table entry operation.*/
        data.width = _BCM_SINGLE_WIDE;
    }

    data.tbl_ptr =  BCM_XGS3_L3_TBL_PTR(unit, tnl_init);
    data.entry_index = idx;
    data.oper_flags = flags;
    data.delete_func = BCM_XGS3_L3_HWCALL(unit, tnl_init_del);

    /* Delete tunnel initiator table entry. */
    rv = _bcm_xgs3_tbl_del(unit, &data);
    _BCM_FLOW_CLEANUP(rv);

    /* don't clear if the entry is still been referenced by other objs */
    if (!BCM_XGS3_L3_ENT_REF_CNT(data.tbl_ptr, idx)) {
        (void) _bcm_flow_tunnel_init_delete(unit, idx, data.width);
    }
    if (data_type == BCMI_TD3_TNL_DATA_TYPE_SRV6_L3) {
        /* srv6 tunnel */
        rv = _bcm_flow_srv6_l3_tnl_object_ctrl(unit, TNL_SRV6_ACTION_DELETE,
                   info.l3_intf_id - BCM_XGS3_EGRESS_IDX_MIN(unit), NULL,
                   0, NULL);
        _BCM_FLOW_CLEANUP(rv);

        rv = _bcm_flow_srv6_l3_tnl_object_ctrl(unit,
                   TNL_SRV6_ACTION_EGR_INTF_GET,
                   info.l3_intf_id - BCM_XGS3_EGRESS_IDX_MIN(unit), NULL,
                   0, &oif_idx);
        _BCM_FLOW_CLEANUP(rv);

        rv = _bcm_flow_tunnel_egr_intf_used_count_get(unit, oif_idx,
                           &l3_oif_cnt);
        _BCM_FLOW_CLEANUP(rv);

        /* detach tunnel index from egr_l3_intf */
        if (l3_oif_cnt <= 1) {
            rv = _bcm_flow_tunnel_idx_oif_set(unit, oif_idx, 0);
            _BCM_FLOW_CLEANUP(rv);
        }

        rv = bcm_xgs3_get_ref_count_from_nhi(unit, 0, &ref_count,
                     info.l3_intf_id - BCM_XGS3_EGRESS_IDX_MIN(unit));
        _BCM_FLOW_CLEANUP(rv);
    } else if (data_type == BCMI_TD3_TNL_DATA_TYPE_SRV6_L2 &&
               soft_tnl_idx < _BCM_FLOW_TUNNEL_L2_MAX(unit)) {

        sal_memset(tnl_entry, 0, sizeof(tnl_entry));
        vc_entry = (egr_mpls_vc_and_swap_label_table_entry_t *)tnl_entry;
        rv = soc_mem_write(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                MEM_BLOCK_ANY, soft_tnl_idx, vc_entry);
        _BCM_FLOW_CLEANUP(rv);
    }

    flow_info->init_tunnel[soft_tnl_idx].idx = 0;

    BCM_XGS3_L3_MODULE_UNLOCK(unit);
    bcmi_esw_flow_unlock(unit);
    return BCM_E_NONE;

cleanup:
    if (l3_lock == TRUE) {
        BCM_XGS3_L3_MODULE_UNLOCK(unit);
    }
    bcmi_esw_flow_unlock(unit);
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_tunnel_initiator_destroy_all
 * Purpose:
 *      Delete all flow tunnel initiator object
 *
 * Parameters:
 *      unit    - (IN) Device Number
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_esw_flow_tunnel_initiator_destroy_all(
    int unit)
{
    int soft_tnl_idx;
    _bcm_flow_bookkeeping_t *flow_info;
    int num_tnl = 0;
    bcm_gport_t flow_tunnel_id;
    int rv = 0;

    num_tnl = _BCM_FLOW_TUNNEL_ID_MAX(unit);

    flow_info = FLOW_INFO(unit);
    for (soft_tnl_idx = 0; soft_tnl_idx < num_tnl; soft_tnl_idx++) {
        if (flow_info->init_tunnel[soft_tnl_idx].idx) {
            BCM_GPORT_TUNNEL_ID_SET(flow_tunnel_id, soft_tnl_idx);
            rv = bcmi_esw_flow_tunnel_initiator_destroy(unit, flow_tunnel_id);
            if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
              return rv;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_flow_tunnel_initiator_get
 * Purpose:
 *      Retrieve the flow tunnel initiator object on the given flow port for L2
 * tunnel or on the given L3 interface for L3 tunnel.
 *
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN/OUT) Flow tunnel initiator descriptor
 *      num_of_fields - (IN) number of logical fields
 *      field         - (IN) logical field array
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_esw_flow_tunnel_initiator_get(
    int unit,
    bcm_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int soft_tnl_idx;
    _bcm_flow_bookkeeping_t *flow_info;
    int dvp;
    int rv = BCM_E_NONE;

    flow_info = FLOW_INFO(unit);

    if (NULL == info) {
        return (BCM_E_PARAM);
    }

    /* Input parameters check. */
    soft_tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
    _BCM_FLOW_TUNNEL_ID_IF_INVALID_RETURN(unit, soft_tnl_idx);

    if (!flow_info->init_tunnel[soft_tnl_idx].idx) {
        return (BCM_E_PARAM);
    }

    if (soft_tnl_idx < _BCM_FLOW_TUNNEL_L2_MAX(unit)) {
        /* L2 tunnel */
        dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
        if (!dvp) {
            _SHR_GPORT_FLOW_PORT_ID_SET(info->flow_port, soft_tnl_idx); 
        } else if (dvp != soft_tnl_idx) {
            return BCM_E_PARAM;
        }
    } else if (soft_tnl_idx < _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit)) {
        /* L3 tunnel */
        if (!info->l3_intf_id) {
            info->l3_intf_id = soft_tnl_idx - _BCM_FLOW_TUNNEL_L2_MAX(unit);
        } else if (info->l3_intf_id !=
                 (soft_tnl_idx - _BCM_FLOW_TUNNEL_L2_MAX(unit))) {
            return BCM_E_PARAM;
        }
    } else if (soft_tnl_idx < _BCM_FLOW_TUNNEL_ID_MAX(unit)) {
        info->l3_intf_id = soft_tnl_idx -
                           _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit) +
                           BCM_XGS3_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_NOT_FOUND;
    }

    rv = _bcm_flow_tunnel_init_get(unit, soft_tnl_idx, info,field,
                                   num_of_fields,NULL);
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_tunnel_initiator_traverse
 * Purpose:
 *      Traverse all flow tunnel initiator objects and call the user specified
 * callback function on each object.
 *
 * Parameters:
 *      unit      - (IN) Device Number
 *      bcm_flow_tunnel_initiator_traverse_cb - (IN) user callback function 
 *      user_data - (IN) user context data for the callback 
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_esw_flow_tunnel_initiator_traverse(
    int unit,
    bcm_flow_tunnel_initiator_traverse_cb cb,
    void *user_data)
{
    bcm_flow_tunnel_initiator_t info;
    uint32 num_of_fields = _BCM_FLOW_LOGICAL_FIELD_MAX;
    bcm_flow_logical_field_t field[_BCM_FLOW_LOGICAL_FIELD_MAX];
    int rv = BCM_E_NONE;
    uint32 soft_tnl_idx;
    _bcm_flow_bookkeeping_t *flow_info;
    uint32 cnt = 0;

    if (NULL == cb) {
        return (BCM_E_PARAM);
    } 
    flow_info = FLOW_INFO(unit);

    for (soft_tnl_idx = 0; soft_tnl_idx < _BCM_FLOW_TUNNEL_ID_MAX(unit); 
                           soft_tnl_idx++) {
        if (!flow_info->init_tunnel[soft_tnl_idx].idx) {
            continue;
        } 
        bcm_flow_tunnel_initiator_t_init(&info);
        BCM_GPORT_TUNNEL_ID_SET(info.tunnel_id,soft_tnl_idx);
        rv = _bcm_flow_tunnel_init_get(unit, soft_tnl_idx, &info, field, 
                  num_of_fields,&cnt);
        if (rv == BCM_E_NOT_FOUND) { /* for entries not exposed to user */
            continue;
        } else if (BCM_FAILURE(rv)) {
            break;
        }
        if (soft_tnl_idx < _BCM_FLOW_TUNNEL_L2_MAX(unit)) {
            /* L2 tunnel */
            _SHR_GPORT_FLOW_PORT_ID_SET(info.flow_port, soft_tnl_idx);

        } else {
            /* L3 tunnel */
            info.l3_intf_id = soft_tnl_idx - _BCM_FLOW_TUNNEL_L2_MAX(unit);
        }

        rv = cb(unit,&info,cnt,field,user_data);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    return rv;
}

STATIC 
int _bcm_esw_flow_tnl_term_num_hw_entries_get(int unit, 
                                              soc_mem_t mem, 
                                              uint32 *entry,
                                              uint32 *num_hw_entries)
{
    int mode;

    if ((entry == NULL) ||
        (num_hw_entries == NULL)) {
        return BCM_E_PARAM;
    }
    *num_hw_entries = 0;
    if (mem == L3_TUNNELm) {
        mode =  soc_mem_field32_get(unit, mem, entry, MODEf);
        if (mode == _BCM_FLOW_TD3_L3_TUNNEL_QUAD_WIDE) {
            *num_hw_entries = _BCM_FLOW_TD3_L3_TUNNEL_QUAD;
        } else if (mode == _BCM_FLOW_TD3_L3_TUNNEL_DOUBLE_WIDE) {
            *num_hw_entries = _BCM_FLOW_TD3_L3_TUNNEL_DOUBLE;
        } else {
            *num_hw_entries = _BCM_FLOW_TD3_L3_TUNNEL_WIDTH_SINGLE;
        }
        return BCM_E_NONE;
    }
    return BCM_E_PARAM;
}
                                      
int bcmi_esw_flow_entry_to_tnl_term(int unit,
                                    uint32 *entry, 
                                    soc_tunnel_term_t *tnl_term) 
{
   uint32 num_hw_entries = 0;
   uint32 i = 0;
   uint32 entry_width = _BCM_FLOW_TD3_L3_TUNNEL_SINGLE_ENTRY_SIZE;

   BCM_IF_ERROR_RETURN(
        _bcm_esw_flow_tnl_term_num_hw_entries_get(unit, L3_TUNNELm,
                             entry, &num_hw_entries));
   for (i = 0; i < num_hw_entries; i++) {
       sal_memcpy((uint8 *) &tnl_term->entry_arr[i], 
                  entry + entry_width * i, entry_width);
   }
   return BCM_E_NONE;
}

int bcmi_esw_flow_tnl_term_to_entry(int unit,
                                    soc_tunnel_term_t *tnl_term,
                                    uint32 *entry)
{
   uint32 num_hw_entries = 0;
   uint32 i = 0;
   uint32 entry_width = _BCM_FLOW_TD3_L3_TUNNEL_SINGLE_ENTRY_SIZE;

   BCM_IF_ERROR_RETURN(
        _bcm_esw_flow_tnl_term_num_hw_entries_get(unit, L3_TUNNELm,
                             entry, &num_hw_entries));
   for (i = 0; i < num_hw_entries; i++) {
       sal_memcpy(entry + (entry_width * i),
                 (uint8 *) &tnl_term->entry_arr[i],
                  entry_width);
   }
   return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flow_tnl_term_validate
 * Purpose:
 *      Validate  the tunnel terrmination attributes
 *
 * Parameters:
 *      unit          - (IN) Device Number
 *      info          - (IN) Flow tunnel terminator descriptor
 * Returns:
 *      TRUE/FALSE
 */
STATIC 
int _bcm_esw_flow_tnl_term_validate(
    int unit,
    bcm_flow_tunnel_terminator_t *info)
{

    if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_DIP_VALID) {
        if (0 == info->dip) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Tunnel Termination : \
             mismatch valid_elements flag and dip field\n"))); 
            return FALSE;
        }
    }
    if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_SIP_VALID) {
        if (0 == info->sip) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Tunnel Termination : \
             mismatch valid_elements flag and sip field\n")));
            return FALSE;
        }
    }
    
    if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_SIP_MASK_VALID) {
        if (0 == info->sip_mask) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Tunnel Termination : \
             mismatch valid_elements flag and sip_mask field\n")));
            return FALSE;
        }
    }
    if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_DIP_MASK_VALID) {
        if (0 == info->dip_mask) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Tunnel Termination : \
             mismatch valid_elements flag and dip_mask field\n")));
            return FALSE;
        }
    }
    if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_DIP6_VALID) {
        if (!bcmi_flow_ip6_addr_is_valid(info->dip6)) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Tunnel Termination : \
             mismatch valid_elements flag and dip6 field\n")));
            return FALSE;
        }
    }
    if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_SIP6_VALID) {
        if (!bcmi_flow_ip6_addr_is_valid(info->sip6)) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Tunnel Termination : \
             mismatch valid_elements flag and sip6 field\n")));
            return FALSE;
        }
    }

    if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_SIP6_MASK_VALID) {
        if (!bcmi_flow_ip6_addr_is_valid(info->sip6_mask)) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Tunnel Termination : \
             mismatch valid_elements flag and sip6_mask field\n")));
            return FALSE;
        }
    }
    if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_DIP6_MASK_VALID) {
        if (!bcmi_flow_ip6_addr_is_valid(info->dip6_mask)) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Tunnel Termination : \
             mismatch valid_elements flag and dip6_mask field\n")));
            return FALSE;
        }
    }
    if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_UDP_SRC_PORT_VALID) {
        if (0 == info->udp_src_port) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Tunnel Termination : \
             mismatch valid_elements flag and udp_src_port field\n")));
            return FALSE;
        }
    }
    if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_UDP_DST_PORT_VALID) {
        if (0 == info->udp_dst_port) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Tunnel Termination : \
             mismatch valid_elements flag and udp_dst_port field\n")));
            return FALSE;
        }
    }
    if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_PROTOCOL_VALID) {
        if (0 == info->protocol) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Tunnel Termination : \
             mismatch valid_elements flag and protocol field\n")));
            return FALSE;
        }
    }
    if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_VLAN_VALID) {
        VLAN_CHK_ID(unit, info->vlan);
    }
    return TRUE;
}

/*
 * Function:
 *     _bcm_flow_tnl_term_entry_data_set
 * Purpose:
 *      Set the data information for flow tunnel terminator entry.
 * Parameters:
 *      unit          - (IN)     Device Number.
 *      info          - (IN)     Flow tunnel terminator descriptor.
 *      field         - (IN)     logical field array.      
 *      num_of_fields - (IN)     number of logical fields.
 *      entry         - (IN)     memory entry.
 *      vinfo         - (IN)     view information.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_flow_tnl_term_entry_data_set(int unit,
    bcm_flow_tunnel_terminator_t *info,
    bcm_flow_logical_field_t *field,
    uint32 num_of_fields, 
    uint32 *entry,
    soc_flow_db_mem_view_info_t vinfo)
{
    int rv = BCM_E_NONE;
    int i;
    int j;
    int index = -1;
    uint32 return_entry[SOC_MAX_MEM_WORDS];
    int new_entry_add = FALSE;
    uint32 field_list[SOC_FLOW_DB_MAX_VIEW_FIELDS];
    uint32 field_count = 0;
    soc_mem_t mem;
    uint32 mem_view_id;
    soc_tunnel_term_t tnl_term;
    soc_tunnel_term_t ret_tnl_term;
    uint32 vxlan_handle;
    int nh_idx;
    int vfi;

    mem = _BCM_FLOW_IS_FLEX_VIEW(info)?
             vinfo.mem_view_id: vinfo.mem;
    mem_view_id = vinfo.mem_view_id;

    if (vinfo.mem == L3_TUNNELm) {
        BCM_IF_ERROR_RETURN(
        bcmi_esw_flow_entry_to_tnl_term(unit, entry, &tnl_term));
        rv = soc_tunnel_term_match(unit, &tnl_term,
                              &ret_tnl_term, &index); 
    } else { 
        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, entry,
                        return_entry, 0);
    }

    if (BCM_E_NONE == rv) {
        /* found memory entry */
        if (!(info->flags & BCM_TUNNEL_REPLACE)) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
                (BSL_META_U(unit,
                 "Flow Tunnel Termination: \
                 Entry with same key already exists\n")));
            return BCM_E_EXISTS;
        }
    } else if (BCM_E_NOT_FOUND == rv) {
        new_entry_add = TRUE;
    } else {
        return rv;
    }

    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        /* Policy data fields*/
        if (soc_flow_db_mem_view_field_is_valid
                       (unit, mem_view_id, TERM_TUNNELf)) {
            soc_mem_field32_set(unit, mem, entry,
                                   TERM_TUNNELf, 1);
            soc_mem_field32_set(unit, mem, entry, 
                                   IGNORE_UDP_CHECKSUMf,
               (info->flags & BCM_TUNNEL_TERM_UDP_CHECKSUM_ENABLE) ? 0x0 : 0x1);
            if (info->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP) {
                soc_mem_field32_set(unit, mem, entry, 
                                        USE_OUTER_HEADER_PHBf, 0x1);
            } else if (info->flags & BCM_TUNNEL_TERM_USE_OUTER_PCP) {
                soc_mem_field32_set(unit, mem, entry,
                                       USE_OUTER_HEADER_PHBf, 0x2);
            } 
            if (info->flags & BCM_TUNNEL_TERM_USE_OUTER_TTL) {
                soc_mem_field32_set(unit, mem, entry, 
                    USE_OUTER_HDR_TTLf, 1);
            } 
            if (info->flags & BCM_TUNNEL_TERM_KEEP_INNER_DSCP) {
                soc_mem_field32_set(unit, mem, entry, 
                    DONOT_CHANGE_INNER_HDR_DSCPf, 1);
            }
            SOC_IF_ERROR_RETURN(soc_flow_db_flow_handle_get(unit, "VXLAN", 
                                 &vxlan_handle));

            if (info->multicast_flag == BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_ENABLE) {
                soc_mem_field32_set(unit, mem, entry, 
                                     NETWORK_RECEIVERS_PRESENTf, 0x01);
                if (info->flow_handle == vxlan_handle) {
                    BCM_IF_ERROR_RETURN(_bcm_flow_bud_loopback_termination_set(
                            unit, info->flow_handle,TRUE));
                }
            } else if (info->multicast_flag == BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_DISABLE) {
                soc_mem_field32_set(unit, mem, entry, NETWORK_RECEIVERS_PRESENTf, 0x00);
                if (info->flow_handle == vxlan_handle) {
                    BCM_IF_ERROR_RETURN(_bcm_flow_bud_loopback_termination_set(
                            unit, info->flow_handle,FALSE));
                }
            }
        }
        if (SOC_MEM_FIELD_VALID(unit, mem, TUNNEL_TERM_SUBLAYER_ACTION_SETf)) {
            soc_mem_field32_set(unit, mem, entry, 
                             TUNNEL_TERM_SUBLAYER_ACTION_SETf, 1);
        } 

        /* Logical policy data fields */
        if (info->valid_elements == BCM_FLOW_MATCH_FLEX_DATA_VALID) {
            BCM_IF_ERROR_RETURN(
                      soc_flow_db_mem_view_field_list_get(unit,
                      mem_view_id,
                      SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                      SOC_FLOW_DB_MAX_VIEW_FIELDS, field_list, &field_count));
            for (i = 0; (i < num_of_fields); i++) {
                for (j = 0; j < field_count; j++) {
                    if (field[i].id == field_list[j]) {
                        soc_mem_field32_set(unit,mem, entry,
                             field[i].id, field[i].value);
                    }
                }
            }
      
        }

        if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_CLASS_ID_VALID) {
            if (SOC_MEM_FIELD_VALID(unit, mem, CLASS_ID_ACTION_SETf)) {
                soc_mem_field32_set(unit, mem, entry,
                             CLASS_ID_ACTION_SETf, info->class_id);
            } else {
                return BCM_E_PARAM;
            }
        }
        if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_FLOW_PORT_VALID) {
            if (SOC_MEM_FIELD_VALID(unit, mem, SVP_ACTION_SETf)) {
                soc_mem_field32_set(unit, mem, entry,
                             SVP_ACTION_SETf,
                             BCM_GPORT_FLOW_PORT_ID_GET(info->flow_port) << 1);
            } else {
                return BCM_E_PARAM;
            }
        }
        if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_INTF_ID_VALID) {
            if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, info->intf_id)) {
                nh_idx = info->intf_id - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
                if (SOC_MEM_FIELD_VALID(unit, mem, DESTINATION_ACTION_SETf)) {
                    soc_mem_field32_dest_set(unit, mem, entry,
                        DESTINATION_ACTION_SETf, SOC_MEM_FIF_DEST_ECMP, nh_idx);
                    if (SOC_MEM_FIELD_VALID(unit, mem, L3_IIF_ACTION_SETf)) {
                        soc_mem_field32_set(unit, mem, entry,
                                L3_IIF_ACTION_SETf,
                                FLOW_INFO(unit)->direct_fwd_iif);
                    }
                } else {
                    return BCM_E_PARAM;
                }
            } else if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->intf_id)) {
                nh_idx = info->intf_id - BCM_XGS3_EGRESS_IDX_MIN(unit);
                if (SOC_MEM_FIELD_VALID(unit, mem, DESTINATION_ACTION_SETf)) {
                    soc_mem_field32_dest_set(unit, mem, entry,
                        DESTINATION_ACTION_SETf, SOC_MEM_FIF_DEST_NEXTHOP,
                        nh_idx);
                    if (SOC_MEM_FIELD_VALID(unit, mem, L3_IIF_ACTION_SETf)) {
                        soc_mem_field32_set(unit, mem, entry,
                                L3_IIF_ACTION_SETf,
                                FLOW_INFO(unit)->direct_fwd_iif);
                    }
                } else {
                    return BCM_E_PARAM;
                }
            } else if (SOC_MEM_FIELD_VALID(unit, mem, L3_IIF_ACTION_SETf)) {
                soc_mem_field32_set(unit, mem, entry,
                             L3_IIF_ACTION_SETf,
                             info->intf_id);
            } else {
                return BCM_E_PARAM;
            }
        }
        if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_VPN_VALID) {
            if (SOC_MEM_FIELD_VALID(unit, mem, VFI_ACTION_SETf)) {
                BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_is_valid(unit,
                                    info->vpn));
                _BCM_FLOW_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, info->vpn);
                soc_mem_field32_set(unit, mem, entry, VFI_ACTION_SETf, vfi);
            } else {
                return BCM_E_PARAM;
            }
        }
    } else {
        /* fixed views */
        if ((info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) &&
            (info->type == bcmTunnelTypeVxlan)) {
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                if (info->flags & BCM_TUNNEL_TERM_KEEP_INNER_DSCP) {
                    soc_mem_field32_set(unit, mem, entry,
                        VXLAN_FLEX_IPV4_DIP__DONOT_CHANGE_INNER_HDR_DSCPf, 1);
                }
                if (info->multicast_flag == BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_ENABLE) {
                    soc_mem_field32_set(unit, mem, entry,
                    VXLAN_FLEX_IPV4_DIP__NETWORK_RECEIVERS_PRESENTf, 0x01);
                    BCM_IF_ERROR_RETURN(_bcm_flow_bud_loopback_termination_set(unit,
                                        info->flow_handle,TRUE));
                } else if (info->multicast_flag == BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_DISABLE) {
                    soc_mem_field32_set(unit, mem, entry,
                    VXLAN_FLEX_IPV4_DIP__NETWORK_RECEIVERS_PRESENTf, 0x00);
                    BCM_IF_ERROR_RETURN(_bcm_flow_bud_loopback_termination_set(unit,
                                        info->flow_handle,FALSE));
                }
            } else {
                if (info->multicast_flag == BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_ENABLE) {
                    soc_mem_field32_set(unit, mem, entry,
                    VXLAN_DIP__NETWORK_RECEIVERS_PRESENTf, 0x01);
                    BCM_IF_ERROR_RETURN(_bcm_flow_bud_loopback_termination_set(unit,
                                        info->flow_handle,TRUE));
                } else if (info->multicast_flag == BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_DISABLE) {
                    soc_mem_field32_set(unit, mem, entry,
                    VXLAN_DIP__NETWORK_RECEIVERS_PRESENTf, 0x00);
                    BCM_IF_ERROR_RETURN(_bcm_flow_bud_loopback_termination_set(unit,
                                        info->flow_handle,FALSE));
                }
            }
        } else if ((info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) &&
            (info->type == bcmTunnelTypeL2Gre)) {
            if (info->multicast_flag == BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_ENABLE) {
                soc_mem_field32_set(unit, mem, entry,
                L2GRE_DIP__NETWORK_RECEIVERS_PRESENTf, 0x01);
                BCM_IF_ERROR_RETURN(_bcm_flow_bud_loopback_termination_set(unit,
                                    info->flow_handle,TRUE));
            } else if (info->multicast_flag == 
                       BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_DISABLE) {
                soc_mem_field32_set(unit, mem, entry,
                L2GRE_DIP__NETWORK_RECEIVERS_PRESENTf, 0x00);
                BCM_IF_ERROR_RETURN(_bcm_flow_bud_loopback_termination_set(unit,
                                    info->flow_handle,FALSE));
            }
            
        }
    } 


    if (vinfo.mem == L3_TUNNELm) {
        BCM_IF_ERROR_RETURN(
        bcmi_esw_flow_entry_to_tnl_term(unit, entry, &tnl_term));
        rv = soc_tunnel_term_insert(unit, &tnl_term, (uint32 *) &index);
    } else {
        if (new_entry_add) {
            rv = soc_mem_insert(unit, mem,
                              MEM_BLOCK_ALL, entry);
        } else {
            rv = soc_mem_write(unit, mem,
                          MEM_BLOCK_ALL, index, entry);
        }
    }
    return rv;
}

/*
 * Function:
 *     _bcm_flow_tnl_term_entry_data_get
 * Purpose:
 *      Get the data information from flow tunnel terminator entry.
 * Parameters:
 *      unit          - (IN)     Device Number.
 *      info          - (IN)     Flow tunnel terminator descriptor.
 *      field         - (IN)     logical field array.
 *      num_of_fields - (IN)     number of logical fields.
 *      entry         - (IN)     memory entry.
 *      vinfo         - (IN)     view information.
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_tnl_term_entry_data_get(int unit,
    bcm_flow_tunnel_terminator_t *info,
    bcm_flow_logical_field_t *field,
    uint32 num_of_fields, 
    uint32 *entry,
    soc_flow_db_mem_view_info_t vinfo) 
{
    int i;
    int j;
    int index = -1;
    uint32 rentry[SOC_MAX_MEM_WORDS];
    uint32 field_list[SOC_FLOW_DB_MAX_VIEW_FIELDS];
    uint32 field_count = 0;
    soc_tunnel_term_t tnl_entry;
    soc_tunnel_term_t ret_tnl_entry;
    soc_mem_t mem;
    uint32 mem_view_id;
    int vp;
    uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
    uint32 dest = 0;
    int vfi;

    mem = _BCM_FLOW_IS_FLEX_VIEW(info)?
             vinfo.mem_view_id: vinfo.mem;
    mem_view_id = vinfo.mem_view_id; 
    
    if (vinfo.mem == L3_TUNNELm) {
        BCM_IF_ERROR_RETURN(
        bcmi_esw_flow_entry_to_tnl_term(unit, entry, &tnl_entry));
        BCM_IF_ERROR_RETURN(
           soc_tunnel_term_match(unit, &tnl_entry,
                              &ret_tnl_entry, &index));
       bcmi_esw_flow_tnl_term_to_entry(unit, &ret_tnl_entry, rentry);
    } else {
        BCM_IF_ERROR_RETURN(
              soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, entry,
                        rentry, 0));
    }
    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        if (soc_flow_db_mem_view_field_is_valid
                       (unit, mem_view_id, TERM_TUNNELf)) { 
        /* Policy data fields*/
            if (soc_mem_field32_get(unit, mem, rentry, 
                                   IGNORE_UDP_CHECKSUMf) == 0x0) 
            {
                info->flags |= BCM_TUNNEL_TERM_UDP_CHECKSUM_ENABLE; 
            } 
            if (soc_mem_field32_get(unit, mem, rentry, 
                             USE_OUTER_HEADER_PHBf) ==  0x1) {
                info->flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP;
            } else if (soc_mem_field32_get(unit, mem, rentry,
                                       USE_OUTER_HEADER_PHBf) ==  0x2) {
                info->flags |= BCM_TUNNEL_TERM_USE_OUTER_PCP;
            }
            if (soc_mem_field32_get(unit, mem, rentry, 
                    USE_OUTER_HDR_TTLf) ==  1) {
                 info->flags |=  BCM_TUNNEL_TERM_USE_OUTER_TTL;
            }
            if (soc_mem_field32_get(unit, mem, rentry, 
                    DONOT_CHANGE_INNER_HDR_DSCPf) ==  1) {
                info->flags |= BCM_TUNNEL_TERM_KEEP_INNER_DSCP;
            }           
            if(soc_mem_field32_get(unit, mem, rentry, 
                    NETWORK_RECEIVERS_PRESENTf) == 0x01) {
               info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_ENABLE;
            } else if (soc_mem_field32_get(unit, mem, rentry, 
                    NETWORK_RECEIVERS_PRESENTf) == 0x00) {
               info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_DISABLE;
            }
        }
        if (SOC_MEM_FIELD_VALID(unit, mem, SVP_ACTION_SETf)) {
            vp = soc_mem_field32_get(unit, mem, rentry, SVP_ACTION_SETf);
            BCM_GPORT_FLOW_PORT_ID_SET(info->flow_port, vp);
        }
        if (SOC_MEM_FIELD_VALID(unit, mem, DESTINATION_ACTION_SETf)) {
            dest = soc_mem_field32_dest_get(unit, mem, rentry,
                                       DESTINATION_ACTION_SETf, &dest_type);
            if (dest_type == SOC_MEM_FIF_DEST_NEXTHOP) {
                info->intf_id = dest + BCM_XGS3_EGRESS_IDX_MIN(unit);
            }
        }
        if (dest_type == SOC_MEM_FIF_DEST_INVALID) {
            if (SOC_MEM_FIELD_VALID(unit, mem, L3_IIF_ACTION_SETf)) {
                info->intf_id = soc_mem_field32_get(unit, mem, rentry,
                                           L3_IIF_ACTION_SETf);
            }
        }
        if (SOC_MEM_FIELD_VALID(unit, mem, VFI_ACTION_SETf)) {
            vfi = soc_mem_field32_get(unit, mem, rentry, VFI_ACTION_SETf);
            _BCM_FLOW_VPN_SET(info->vpn, _BCM_VPN_TYPE_VFI, vfi);
        }

        /* Logical policy data fields */
        if (_BCM_FLOW_IS_FLEX_VIEW(info) && (field != NULL)) {
            BCM_IF_ERROR_RETURN(
                      soc_flow_db_mem_view_field_list_get(unit,
                      mem_view_id,
                      SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                      SOC_FLOW_DB_MAX_VIEW_FIELDS, field_list, &field_count));
            for (i = 0; (i < num_of_fields); i++) {
                for (j = 0; j < field_count; j++) {
                    if (field[i].id == field_list[j]) {
                       field[i].value = 
                           soc_mem_field32_get(unit, mem, rentry, field[i].id);;
                    }
                }
            }
      
        }

    } else {
        /* fixed views */
        if ((info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) &&
            (info->type == bcmTunnelTypeVxlan)) {
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                if (soc_mem_field32_get(unit, mem, rentry,
                        VXLAN_FLEX_IPV4_DIP__DONOT_CHANGE_INNER_HDR_DSCPf)) {
                    info->flags |= BCM_TUNNEL_TERM_KEEP_INNER_DSCP;
                }
                if (soc_mem_field32_get(unit, mem, rentry,
                           VXLAN_FLEX_IPV4_DIP__IGNORE_UDP_CHECKSUMf) == 0x0) {
                    info->flags |= BCM_TUNNEL_TERM_UDP_CHECKSUM_ENABLE;
                }
                if (soc_mem_field32_get(unit, mem, rentry,
                         VXLAN_FLEX_IPV4_DIP__USE_OUTER_HEADER_PHBf) ==  0x1) {
                    info->flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP;
                } else if (soc_mem_field32_get(unit, mem, rentry,
                         VXLAN_FLEX_IPV4_DIP__USE_OUTER_HEADER_PHBf) ==  0x2) {
                    info->flags |= BCM_TUNNEL_TERM_USE_OUTER_PCP;
                }
                /*Multicast flag information */
                if (soc_mem_field32_get(unit, mem, rentry,
                    VXLAN_FLEX_IPV4_DIP__NETWORK_RECEIVERS_PRESENTf) == 0x01) {
                    info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_ENABLE;
                } else if (soc_mem_field32_get(unit, mem, rentry,
                    VXLAN_FLEX_IPV4_DIP__NETWORK_RECEIVERS_PRESENTf) == 0x00) {
                    info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_DISABLE;
                }
            } else {
                if (soc_mem_field32_get(unit, mem, rentry,
                        VXLAN_DIP__DONOT_CHANGE_INNER_HDR_DSCPf)) {
                    info->flags |= BCM_TUNNEL_TERM_KEEP_INNER_DSCP;
                }
                if (soc_mem_field32_get(unit, mem, rentry,
                                    VXLAN_DIP__IGNORE_UDP_CHECKSUMf) == 0x0)
                {
                    info->flags |= BCM_TUNNEL_TERM_UDP_CHECKSUM_ENABLE;
                }
                if (soc_mem_field32_get(unit, mem, rentry,
                            VXLAN_DIP__USE_OUTER_HEADER_PHBf) ==  0x1) {
                    info->flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP;
                } else if (soc_mem_field32_get(unit, mem, rentry,
                                   VXLAN_DIP__USE_OUTER_HEADER_PHBf) ==  0x2) {
                    info->flags |= BCM_TUNNEL_TERM_USE_OUTER_PCP;
                }
                /*Multicast flag information */
                if(soc_mem_field32_get(unit, mem, rentry,
                        VXLAN_DIP__NETWORK_RECEIVERS_PRESENTf) == 0x01) {
                   info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_ENABLE;
                } else if (soc_mem_field32_get(unit, mem, rentry,
                        VXLAN_DIP__NETWORK_RECEIVERS_PRESENTf) == 0x00) {
                   info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_DISABLE;
                }
            }
        } else if ((info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) &&
                  (info->type == bcmTunnelTypeL2Gre)) {
            /*Multicast flag information */
            if(soc_mem_field32_get(unit, mem, rentry,
                    L2GRE_DIP__NETWORK_RECEIVERS_PRESENTf) == 0x01) {
               info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_ENABLE;
            } else if (soc_mem_field32_get(unit, mem, rentry,
                    L2GRE_DIP__NETWORK_RECEIVERS_PRESENTf) == 0x00) {
               info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_DISABLE;
            }

        }       
        
    } 
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_tnl_term_entry_key_set
 * Purpose:
 *      Set the key information for flow tunnel terminator object.
 * Parameters:
 *      unit          - (IN)     Device Number.
 *      info          - (IN/OUT) Flow tunnel terminator descriptor.
 *      field         - (IN)     logical field array.      
 *      num_of_fields - (IN)     number of logical fields.
 *      entry         - (IN)     memory entry.
 *      vinfo         - (IN)     view information.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_flow_tnl_term_entry_key_set(int unit,
    bcm_flow_tunnel_terminator_t *info,
    bcm_flow_logical_field_t *field,
    uint32 num_of_fields, 
    uint32 *entry,
    soc_flow_db_mem_view_info_t *vinfo)
{
    int rv = BCM_E_NONE;
    uint32  key_list[SOC_FLOW_DB_MAX_VIEW_FIELDS];
    uint32  key_count = 0;
    uint32  mem_view_id = 0;
    soc_mem_t   mem = 0;
    int i = 0;
    int j = 0;
    uint32 lfield_id;

    /* Get the view info */
    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        mem_view_id = vinfo->mem_view_id;
        mem = vinfo->mem_view_id;
        
        /* intitalize the entry with 
         * key_type , data_type , control fields
         */       
        soc_flow_db_mem_view_entry_init(unit, mem_view_id, entry);
        /* set the key fields */
        /* get the list of key fields
         * based VALID FIELDS , set the matching fields.  
         */  
        BCM_IF_ERROR_RETURN(
            soc_flow_db_mem_view_field_list_get(unit, 
                        mem_view_id, 
                        SOC_FLOW_DB_FIELD_TYPE_KEY,      
                        SOC_FLOW_DB_MAX_VIEW_FIELDS,
                        key_list,
                        &key_count));
        for (i = 0; i < key_count; i++) {
            if (key_list[i] == IPV4__DIPf) {
                if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_DIP_VALID) {
                    rv = (info->dip) ? BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, entry, 
                                     IPV4__DIPf, info->dip);
                } 
            } else if (key_list[i] == IPV4__SIPf) { 
                if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_SIP_VALID) {
                    rv = (info->sip) ? BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, entry, 
                                     IPV4__SIPf, info->sip);
                }
           } else if (key_list[i] == IPV4__DIP_MASKf) {
                if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_DIP_MASK_VALID) {
                    rv = (info->dip_mask) ? BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, entry, 
                                     IPV4__DIP_MASKf, info->dip);
                }
            } else if (key_list[i] == IPV4__SIP_MASKf) {
                if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_SIP_MASK_VALID) {
                    rv = (info->sip_mask) ? BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, entry, 
                                     IPV4__SIP_MASKf, info->sip_mask);
                }
            } else if (key_list[i] == L4_DEST_PORTf) {
                if(info->valid_elements & BCM_FLOW_TUNNEL_TERM_UDP_DST_PORT_VALID) {
                    rv = (info->udp_dst_port) ? BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, entry, 
                                     L4_DEST_PORTf, info->udp_dst_port);
                }
            } else if (key_list[i] == L4_SRC_PORTf) {
                if(info->valid_elements & BCM_FLOW_TUNNEL_TERM_UDP_SRC_PORT_VALID) {
                    rv = (info->udp_src_port) ? BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, entry, 
                           L4_SRC_PORTf, info->udp_src_port);
                }
            } else if (key_list[i] == PROTOCOLf) {
                if(info->valid_elements & BCM_FLOW_TUNNEL_TERM_PROTOCOL_VALID) {
                    rv = (info->protocol) ? BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem, entry,
                          PROTOCOLf, info->protocol);
                }
            } else if (key_list[i] == IPV6__SIPf) {
                if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_SIP6_VALID) {
                    rv = (bcmi_flow_ip6_addr_is_valid(info->sip6)) ?
                          BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_ip6_addr_set(unit, mem, entry, 
                               IPV6__SIPf, info->sip6, 0);
                }
            } else if (key_list[i] == IPV6__DIPf) {
                if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_DIP6_VALID) {
                    rv = (bcmi_flow_ip6_addr_is_valid(info->dip6)) ?
                          BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_ip6_addr_set(unit, mem, entry, 
                             IPV6__DIPf, info->dip6, 0);
                }
             } else if (key_list[i] == IPV6__SIP_MASKf) {
                if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_SIP6_MASK_VALID) {
                    rv = (bcmi_flow_ip6_addr_is_valid(info->sip6_mask)) ?
                          BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_ip6_addr_set(unit, mem, entry,
                             IPV6__SIP_MASKf, info->sip6_mask, 0);
                }
            } else if (key_list[i] == IPV6__DIP_MASKf) {
                if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_DIP6_MASK_VALID) {
                    rv = (bcmi_flow_ip6_addr_is_valid(info->dip6_mask)) ?
                          BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_ip6_addr_set(unit, mem, entry,
                             IPV6__DIP_MASKf, info->dip6_mask, 0);
                }
            } 
        }

        if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_FLEX_KEY_VALID) {
            /* Logical key fields */
            BCM_IF_ERROR_RETURN(
                soc_flow_db_mem_view_field_list_get(unit, 
                            mem_view_id, 
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,      
                            SOC_FLOW_DB_MAX_VIEW_FIELDS,
                            key_list,
                            &key_count));

            for (i = 0;(i < num_of_fields); i++) {
                for (j = 0; j < key_count; j++) {
                    if (field[i].id == key_list[j]) {
                        soc_mem_field32_set(unit, mem, entry,
                             field[i].id, field[i].value);
                    }
                }
            }
        }
        /* special handling for SRV6 */
        if (info->flow_handle == FLOW_INFO(unit)->flow_srv6) {
            if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_DIP6_VALID) {
                SOC_IF_ERROR_RETURN(soc_flow_db_mem_view_logical_field_id_get(
                         unit, info->flow_handle, "SRV6_DIP", &lfield_id));
                soc_mem_ip6_addr_set(unit, mem, entry, lfield_id, info->dip6,
                                 SOC_MEM_IP6_UPPER_96BIT);
            }
            if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_NEXT_HDR_VALID) {
                SOC_IF_ERROR_RETURN(soc_flow_db_mem_view_logical_field_id_get(
                         unit, info->flow_handle, "SRH_NEXT_HDR", &lfield_id));
                soc_mem_field32_set(unit, mem, entry, lfield_id, info->next_hdr);
            }
            if (info->valid_elements & BCM_FLOW_TUNNEL_TERM_VLAN_VALID) {
                SOC_IF_ERROR_RETURN(soc_flow_db_mem_view_logical_field_id_get(
                         unit, info->flow_handle, "PAYLOAD_OVID", &lfield_id));
                soc_mem_field32_set(unit, mem, entry, lfield_id, info->vlan);
            }
        }
   
    } else {
        /* legacy VXLAN and L2GRE */
        if ((info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) &&
            (info->type == bcmTunnelTypeVxlan)) {
            /* initialize the view info*/
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                mem = vinfo->mem = MPLS_ENTRY_SINGLEm;

                soc_mem_field32_set(unit, mem, entry, BASE_VALIDf, 1);
                soc_mem_field32_set(unit, mem, entry, DATA_TYPEf,
                                    _BCM_VXLAN_FLEX_KEY_TYPE_LOOKUP_DIP);
                soc_mem_field32_set(unit, mem, entry, KEY_TYPEf, _BCM_VXLAN_FLEX_KEY_TYPE_LOOKUP_DIP);
                soc_mem_field32_set(unit, mem, entry, VXLAN_FLEX_IPV4_DIP__DIPf, info->dip);
                soc_mem_field32_set(unit, mem, entry, VXLAN_FLEX_IPV4_DIP__TERM_TUNNELf, 0x1);
                soc_mem_field32_set(unit, mem, entry,
                                 VXLAN_FLEX_IPV4_DIP__IGNORE_UDP_CHECKSUMf,
                (info->flags & BCM_TUNNEL_TERM_UDP_CHECKSUM_ENABLE) ? 0x0 : 0x1);

                if (info->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP) {
                    soc_mem_field32_set(unit, mem, entry,
                     VXLAN_FLEX_IPV4_DIP__USE_OUTER_HEADER_PHBf, 0x1);
                } else if (info->flags & BCM_TUNNEL_TERM_USE_OUTER_PCP) {
                    soc_mem_field32_set(unit, mem, entry,
                     VXLAN_FLEX_IPV4_DIP__USE_OUTER_HEADER_PHBf, 0x2);
                }
            } else {
                mem = vinfo->mem = VLAN_XLATE_1_DOUBLEm;

                soc_mem_field32_set(unit, mem, entry, BASE_VALID_0f, 3);
                soc_mem_field32_set(unit, mem, entry, BASE_VALID_1f, 7);
                soc_mem_field32_set(unit, mem, entry, DATA_TYPEf,
                                    _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP);
                soc_mem_field32_set(unit, mem, entry, KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP);
                soc_mem_field32_set(unit, mem, entry, VXLAN_DIP__DIPf, info->dip);
                if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
                    soc_mem_field32_set(unit, mem, entry,
                                    VXLAN_DIP__OVIDf, info->vlan);
                }
                soc_mem_field32_set(unit, mem, entry, 
                                 VXLAN_DIP__IGNORE_UDP_CHECKSUMf,
                (info->flags & BCM_TUNNEL_TERM_UDP_CHECKSUM_ENABLE) ? 0x0 : 0x1);

                if (info->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP) {
                    soc_mem_field32_set(unit, mem, entry,
                     VXLAN_DIP__USE_OUTER_HEADER_PHBf, 0x1);
                } else if (info->flags & BCM_TUNNEL_TERM_USE_OUTER_PCP) {
                    soc_mem_field32_set(unit, mem, entry,
                     VXLAN_DIP__USE_OUTER_HEADER_PHBf, 0x2);
                }
            }
        } else if ((info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) &&
                   (info->type == bcmTunnelTypeL2Gre)) {
            mem = vinfo->mem = VLAN_XLATE_1_DOUBLEm;

            soc_mem_field32_set(unit, mem, entry, BASE_VALID_0f, 3);
            soc_mem_field32_set(unit, mem, entry, BASE_VALID_1f, 7);
            soc_mem_field32_set(unit, mem, entry, KEY_TYPEf,
                                     _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP);
            soc_mem_field32_set(unit, mem, entry, DATA_TYPEf,
                                    _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP);
            soc_mem_field32_set(unit, mem, entry, 
                                L2GRE_DIP__DIPf, info->dip);
        } else {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                     "Invalid Legacy flow handle or flow type\n")));
            return BCM_E_PARAM;
        }
    }
     
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_tunnel_terminator_create
 * Purpose:
 *      Create a flow tunnel terminator  object.
 *
 * Parameters:
 *      unit          - (IN)     Device Number
 *      info          - (IN/OUT) Flow tunnel terminator descriptor
 *      num_of_fields - (IN)     number of logical fields
 *      field         - (IN)     logical field array
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_esw_flow_tunnel_terminator_create(
    int unit,
    bcm_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    _bcm_flow_bookkeeping_t *flow_info;
    soc_mem_t mem_view_id;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_flow_db_mem_view_info_t vinfo;

    flow_info = FLOW_INFO(unit);

    if (NULL == info) {
        return (BCM_E_PARAM);
    }

    if (flow_info == NULL) {
        return (BCM_E_INTERNAL);
    }

    if ((!_BCM_FLOW_TUNNEL_TYPE_L2(info->type)) && 
        (!_BCM_FLOW_TUNNEL_TYPE_L3(info->type))) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
         "Flow Tunnel Termination: Invalid tunnel type.\n")));
        return BCM_E_PARAM;
    }
   
    BCM_IF_ERROR_RETURN(_bcm_esw_flow_tnl_term_validate(unit, info));

    /* Zero write buffer. */
    sal_memset(entry, 0, sizeof(entry));

    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        BCM_IF_ERROR_RETURN(
           soc_flow_db_ffo_to_mem_view_id_get(
                   unit,
                   info->flow_handle,
                   info->flow_option,
                   SOC_FLOW_DB_FUNC_TUNNEL_TERMINATOR_ID,
                   (uint32 *)&mem_view_id));
        BCM_IF_ERROR_RETURN(
            soc_flow_db_mem_view_info_get(unit, mem_view_id, &vinfo));
    }


    BCM_IF_ERROR_RETURN(_bcm_flow_tnl_term_entry_key_set(unit, info,
                                    field, num_of_fields,
                                    entry, &vinfo));
    /* Allocate tunnel terminator  entry */
    bcmi_esw_flow_lock(unit);
    rv = _bcm_flow_tnl_term_entry_data_set(unit, info,
                                    field, num_of_fields,
                                    entry, vinfo);
    bcmi_esw_flow_unlock(unit);
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_tunnel_terminator_destroy
 * Purpose:
 *      Destroy a flow tunnel terminator entry
 *
 * Parameters:
 *      unit          - (IN)     Device Number
 *      info          - (IN/OUT) Flow tunnel terminator descriptor
 *      num_of_fields - (IN)     number of logical fields
 *      field         - (IN)     logical field array
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_esw_flow_tunnel_terminator_destroy(
    int unit,
    bcm_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field) 
{
    int rv = BCM_E_NONE;
    _bcm_flow_bookkeeping_t *flow_info;
    soc_mem_t mem_view_id;
    soc_tunnel_term_t tnl_entry;
    soc_tunnel_term_t ret_tnl_entry;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 ret_entry[SOC_MAX_MEM_WORDS];
    int index = 0;
    soc_flow_db_mem_view_info_t vinfo;

    flow_info = FLOW_INFO(unit);

    if (NULL == info) {
        return (BCM_E_PARAM);
    }

    if (flow_info == NULL) {
        return (BCM_E_INTERNAL);
    }

    if ((!_BCM_FLOW_TUNNEL_TYPE_L2(info->type)) &&
        (!_BCM_FLOW_TUNNEL_TYPE_L3(info->type))) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
         "Flow Tunnel Termination : Update fail,\
          Invalid tunnel type for flow tunnels\n")));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_flow_tnl_term_validate(unit, info));
    /* Zero write buffer. */
    sal_memset(entry, 0, sizeof(entry));

    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        BCM_IF_ERROR_RETURN(
           soc_flow_db_ffo_to_mem_view_id_get(unit,
                   info->flow_handle,
                   info->flow_option,
                   SOC_FLOW_DB_FUNC_TUNNEL_TERMINATOR_ID,
                   (uint32 *)&mem_view_id));
        BCM_IF_ERROR_RETURN(
            soc_flow_db_mem_view_info_get(unit, mem_view_id, &vinfo));
    }

    BCM_IF_ERROR_RETURN(_bcm_flow_tnl_term_entry_key_set(unit, info,
                                    field, num_of_fields,
                                    entry, &vinfo));
    if (vinfo.mem == L3_TUNNELm) {
        BCM_IF_ERROR_RETURN(
        bcmi_esw_flow_entry_to_tnl_term(unit, entry, &tnl_entry));
        rv = soc_tunnel_term_match(unit, &tnl_entry, &ret_tnl_entry, &index);
        if (rv == BCM_E_NONE) {
            BCM_IF_ERROR_RETURN(
                soc_tunnel_term_delete(unit, &ret_tnl_entry));
        } else if (rv != BCM_E_NOT_FOUND) {
            return rv;
        }

    } else {
        rv = soc_mem_search(unit, vinfo.mem, MEM_BLOCK_ANY, &index,
                                     entry, ret_entry, 0);
        if (rv == BCM_E_NONE) {
            rv = soc_mem_delete(unit, vinfo.mem, MEM_BLOCK_ALL, &ret_entry);
            if (BCM_FAILURE(rv)) {
                 return rv;
            }
        } else if (rv != BCM_E_NOT_FOUND) {
            return rv;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_flow_tunnel_terminator_get
 * Purpose:
 *      Get a flow tunnel terminator entry
 *
 * Parameters:
 *      unit          - (IN)     Device Number
 *      info          - (IN/OUT) Flow tunnel terminator descriptor
 *      num_of_fields - (IN)     number of logical fields
 *      field         - (IN)     logical field array
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_esw_flow_tunnel_terminator_get(
    int unit,
    bcm_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field) 
{
    _bcm_flow_bookkeeping_t *flow_info;
    soc_mem_t mem_view_id;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_flow_db_mem_view_info_t vinfo;
      
    flow_info = FLOW_INFO(unit);

    if (NULL == info) {
        return (BCM_E_PARAM);
    }

    if (flow_info == NULL) {
        return (BCM_E_INTERNAL);
    }

    if ((!_BCM_FLOW_TUNNEL_TYPE_L2(info->type)) &&
        (!_BCM_FLOW_TUNNEL_TYPE_L3(info->type))) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
         "Flow Tunnel Termination : Update fail,\
          Invalid tunnel type for flow tunnels\n")));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_flow_tnl_term_validate(unit, info));
    /* Zero write buffer. */
    sal_memset(entry, 0, sizeof(entry));

    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        BCM_IF_ERROR_RETURN(
           soc_flow_db_ffo_to_mem_view_id_get(unit,
                   info->flow_handle,
                   info->flow_option,
                   SOC_FLOW_DB_FUNC_TUNNEL_TERMINATOR_ID,
                   (uint32 *)&mem_view_id));
        BCM_IF_ERROR_RETURN(
            soc_flow_db_mem_view_info_get(unit, mem_view_id, &vinfo));
    }

    BCM_IF_ERROR_RETURN(
           _bcm_flow_tnl_term_entry_key_set(unit, info,
                     field, num_of_fields, entry, &vinfo));
    BCM_IF_ERROR_RETURN(
           _bcm_flow_tnl_term_entry_data_get(unit, info,
                                  field, num_of_fields,
                                  entry, vinfo));
    return BCM_E_NONE;
}

STATIC int
_bcm_flow_tunnel_terminator_entry_valid(int unit,
                                       soc_mem_t mem,
                                       uint32 *entry,
                                       int *valid)
{
    if (INVALIDm == mem) {
        return BCM_E_MEMORY;
    }

    if ((NULL == entry) || (NULL == valid)) {
        return BCM_E_PARAM;
    }

    *valid = TRUE;
    if (SOC_MEM_FIELD_VALID(unit,mem,BASE_VALID_0f)) {
        if (!soc_mem_field32_get(unit, mem, entry, BASE_VALID_0f)) {
            *valid = FALSE;
        }
    }
    if (SOC_MEM_FIELD_VALID(unit,mem,BASE_VALID_1f)) {
        if (!soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f)) {
            *valid = FALSE;
        }
    }
    if (SOC_MEM_FIELD_VALID(unit, mem, VALIDf)) {
        if (!soc_mem_field32_get(unit, mem, entry, VALIDf)) {
            *valid = FALSE;
        }
    }
    /* validation for single view like MPLS_ENTRY_SINGLE */
    if (SOC_MEM_FIELD_VALID(unit, mem, BASE_VALIDf)) {
        if (0x1 != soc_mem_field32_get(unit, mem, entry, BASE_VALIDf)) {
            *valid = FALSE;
        }
    }

    return BCM_E_NONE;

}

/*
 * Function   :
 *      _bcm_flow_tunnel_terminator_entry_parse
 * Description   :
 *      Convert the memory entry field data to the tunnel terminator
 *      parameters.
 * Parameters   :
 *      unit             - (IN) Device Number
 *      mem              - (IN)  memory id.
 *      mem_view_id      - (IN)  memory view id.
 *      entry            - (IN) HW entry.
 *      info             - (OUT) Flow tunnel termination descriptor
 *      num_of_fields    - (OUT) number of logical fields
 *      field            - (OUT) logical field array
 */
STATIC int
_bcm_flow_tunnel_terminator_entry_parse(int unit, 
                             soc_mem_t mem, 
                             uint32 *entry,
                             bcm_flow_tunnel_terminator_t *info,
                             uint32 *num_of_fields,
                             bcm_flow_logical_field_t *field)
{
    int key_type = SOC_FLOW_DB_KEY_TYPE_INVALID;
    int data_type = SOC_FLOW_DB_KEY_TYPE_INVALID;
    uint32 data_count;
    int i;
    soc_flow_db_mem_view_info_t vinfo;
    uint32 key_list[SOC_FLOW_DB_MAX_VIEW_FIELDS];
    uint32 key_count = 0;
    uint32 field_list[SOC_FLOW_DB_MAX_VIEW_FIELDS];
    uint32 cnt = 0;
    soc_flow_db_ffo_t ffo[SOC_FLOW_DB_MAX_VIEW_FFO_TPL];
    uint32 num_ffo = 0;
    uint32 key_typef[] = {KEY_TYPEf,
                         KEY_TYPE0f,
                         KEY_TYPE1f};
    uint32 data_typef[] = {DATA_TYPEf,
                          DATA_TYPE0f,
                          DATA_TYPE1f};
    int rv = BCM_E_NONE;
    uint32 mem_view_id = 0;
    uint32 lfield_id;
    _bcm_flow_bookkeeping_t *flow_info;
    int vp;
    uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
    uint32 dest = 0;
    int vfi;

    flow_info = FLOW_INFO(unit);

    for (i = 0; i < SOC_FLOW_DB_MAX_KEY_TYPE; i++) {
        if (SOC_MEM_FIELD_VALID(unit,mem,key_typef[i])) {
            key_type = soc_mem_field32_get(unit, mem, entry, key_typef[i]);
            break;
        }
    }
    for (i = 0; i < SOC_FLOW_DB_MAX_DATA_TYPE; i++) {
        if (SOC_MEM_FIELD_VALID(unit,mem,data_typef[i])) {
            data_type = soc_mem_field32_get(unit, mem, entry, data_typef[i]);
        }
    }

    /* find the memory view based on the memory and key type */
    rv = soc_flow_db_mem_to_view_id_get (unit, mem, key_type, data_type,
                    0, NULL, (uint32 *) &mem_view_id);

    if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
        return rv;
    }

    if (rv == BCM_E_NOT_FOUND) {
        /* Fixed view */
        switch (key_type) {
            case _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP:
            case _BCM_VXLAN_FLEX_KEY_TYPE_LOOKUP_DIP:
                info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN;
                info->type = bcmTunnelTypeVxlan;
                break;
            case _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP:
                info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE;
                info->type = bcmTunnelTypeL2Gre;
                break;
            default: 
                return BCM_E_NONE;
        }

        if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                info->dip = soc_mem_field32_get(unit, mem, entry, VXLAN_FLEX_IPV4_DIP__DIPf);
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_DIP_VALID;
                if (soc_mem_field32_get(unit, mem, entry,
                                    VXLAN_FLEX_IPV4_DIP__IGNORE_UDP_CHECKSUMf) == 0x0) {
                    info->flags |= BCM_TUNNEL_TERM_UDP_CHECKSUM_ENABLE;
                }

                if (soc_mem_field32_get(unit, mem, entry,
                                    VXLAN_FLEX_IPV4_DIP__USE_OUTER_HEADER_PHBf) == 0x01) {
                    info->flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP;
                }
                if (soc_mem_field32_get(unit, mem, entry,
                        VXLAN_FLEX_IPV4_DIP__USE_OUTER_HEADER_PHBf) == 0x02) {
                    info->flags |= BCM_TUNNEL_TERM_USE_OUTER_PCP;
                }
                /* Multicast Flags */
                if(soc_mem_field32_get(unit, mem, entry,
                        VXLAN_FLEX_IPV4_DIP__NETWORK_RECEIVERS_PRESENTf) == 0x01) {
                   info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_ENABLE;
                } else if (soc_mem_field32_get(unit, mem, entry,
                        VXLAN_FLEX_IPV4_DIP__NETWORK_RECEIVERS_PRESENTf) == 0x00) {
                   info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_DISABLE;
                }
            } else {
                info->dip = soc_mem_field32_get(unit, mem,
                                                      entry, VXLAN_DIP__DIPf);
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_DIP_VALID;

                if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
                    info->vlan = soc_mem_field32_get(unit, mem, entry,
                                        VXLAN_DIP__OVIDf);
                }

                if (soc_mem_field32_get(unit, mem, entry,
                                    VXLAN_DIP__IGNORE_UDP_CHECKSUMf) == 0x0) {
                    info->flags |= BCM_TUNNEL_TERM_UDP_CHECKSUM_ENABLE;
                }

                if (soc_mem_field32_get(unit, mem, entry,
                                    VXLAN_DIP__USE_OUTER_HEADER_PHBf) == 0x01) {
                    info->flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP;
                }
                if (soc_mem_field32_get(unit, mem, entry,
                        VXLAN_DIP__USE_OUTER_HEADER_PHBf) == 0x02) {
                    info->flags |= BCM_TUNNEL_TERM_USE_OUTER_PCP;
                }
                /* Multicast Flags */
                if(soc_mem_field32_get(unit, mem, entry,
                        VXLAN_DIP__NETWORK_RECEIVERS_PRESENTf) == 0x01) {
                   info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_ENABLE;
                } else if (soc_mem_field32_get(unit, mem, entry,
                        VXLAN_DIP__NETWORK_RECEIVERS_PRESENTf) == 0x00) {
                   info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_DISABLE;
                }
            }
        } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
            info->dip = soc_mem_field32_get(unit, mem,
                                                  entry, L2GRE_DIP__DIPf);
            info->flags |= BCM_FLOW_TUNNEL_TERM_DIP_VALID;
            /* Multicast flags */
            if(soc_mem_field32_get(unit, mem, entry,
                    L2GRE_DIP__NETWORK_RECEIVERS_PRESENTf) == 0x01) {
               info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_ENABLE;
            } else if (soc_mem_field32_get(unit, mem, entry,
                    L2GRE_DIP__NETWORK_RECEIVERS_PRESENTf) == 0x00) {
               info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_DISABLE;
            }

        } 
        
    } else {
        /* Flex view */
        /* make sure it is termination entries */
        if ((mem != L3_TUNNELm) &&
            (!((SOC_MEM_FIELD_VALID(unit, mem_view_id, TERM_TUNNELf) &&
            soc_mem_field32_get(unit, mem_view_id, entry, TERM_TUNNELf)) ||
            (SOC_MEM_FIELD_VALID(unit, mem_view_id,
                                TUNNEL_TERM_SUBLAYER_ACTION_SETf) &&
             soc_mem_field32_get(unit, mem_view_id, entry,
                                TUNNEL_TERM_SUBLAYER_ACTION_SETf))))) {
            return BCM_E_NOT_FOUND;
        }

        /* find the memory view info */
        BCM_IF_ERROR_RETURN(
               soc_flow_db_mem_view_info_get(unit, mem_view_id, &vinfo));
      
        /* get the flow handle, function id, option id */
        BCM_IF_ERROR_RETURN(
              soc_flow_db_mem_view_to_ffo_get(unit,
                                         mem_view_id,
                                         SOC_FLOW_DB_MAX_VIEW_FFO_TPL,
                                         ffo,
                                         &num_ffo));
       for (i = 0; i < num_ffo; i++) {
           if (ffo[i].function_id == SOC_FLOW_DB_FUNC_TUNNEL_TERMINATOR_ID) {
               info->flow_handle = ffo[i].flow_handle;
               info->flow_option = ffo[i].option_id;
               break;
           }
       }
       if (i >= num_ffo) {
           return BCM_E_NOT_FOUND;
       }
       info->type = bcmTunnelTypeL2Flex;

        /* key - Flex view */ 
        BCM_IF_ERROR_RETURN(
                soc_flow_db_mem_view_field_list_get(unit,
                mem_view_id,
                SOC_FLOW_DB_FIELD_TYPE_KEY,
                SOC_FLOW_DB_MAX_VIEW_FIELDS,
                key_list,
                &key_count));

        for (i = 0; i < key_count; i++) {
            if (key_list[i] == IPV4__DIPf) {
                info->dip = soc_mem_field32_get(unit, mem_view_id,
                                              entry, IPV4__DIPf);
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_DIP_VALID;
            } else if (key_list[i] == IPV4__SIPf) {
                info->sip = soc_mem_field32_get(unit, mem_view_id, 
                                       entry, IPV4__SIPf);
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_SIP_VALID;
            }  else if (key_list[i] == IPV4__DIP_MASKf) {
                info->dip_mask = soc_mem_field32_get(unit, mem_view_id,
                                         entry, IPV4__DIP_MASKf);
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_DIP_MASK_VALID; 
            } else if (key_list[i] == IPV4__SIP_MASKf) {
                info->sip_mask = soc_mem_field32_get(unit, mem_view_id,
                                         entry, IPV4__SIP_MASKf);
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_SIP_MASK_VALID;
            } else if (key_list[i] == IPV6__SIPf) {
                soc_mem_ip6_addr_get(unit, mem_view_id,
                        entry, IPV6__SIPf, info->sip6, 0);
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_SIP6_VALID;
            } else if (key_list[i] == IPV6__DIPf) {
                soc_mem_ip6_addr_get(unit, mem_view_id, 
                        entry, IPV6__DIPf, info->dip6, 0);
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_DIP6_VALID;
            } else if (key_list[i] == IPV6__SIP_MASKf) {
                soc_mem_ip6_addr_get(unit, mem_view_id,
                        entry, IPV6__SIP_MASKf, info->sip6_mask, 0);
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_SIP6_MASK_VALID;
            } else if (key_list[i] == IPV6__DIP_MASKf) {
                soc_mem_ip6_addr_get(unit, mem_view_id,
                        entry, IPV6__DIP_MASKf, info->dip6_mask, 0);
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_DIP6_MASK_VALID;
            } else if (key_list[i] == L4_DEST_PORTf) {
                info->udp_dst_port = soc_mem_field32_get(unit, mem_view_id,
                                         entry, L4_DEST_PORTf);
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_UDP_DST_PORT_VALID;
            } else if (key_list[i] ==  L4_SRC_PORTf) {
                info->udp_src_port = soc_mem_field32_get(unit, mem_view_id,
                                         entry, L4_SRC_PORTf);
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_UDP_SRC_PORT_VALID;
            } else if (key_list[i] ==  PROTOCOLf) {
                info->protocol  = soc_mem_field32_get(unit, mem_view_id,
                                         entry, PROTOCOLf);
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_PROTOCOL_VALID;
            }
        }

        /* special handling of the logical field for SRv6 */
        if (flow_info->flow_srv6 == info->flow_handle &&
            (flow_info->option_term_adapt1 == info->flow_option ||
             flow_info->option_term_adapt2_vfi == info->flow_option ||
             flow_info->option_term_adapt2 == info->flow_option)) {
            SOC_IF_ERROR_RETURN(soc_flow_db_mem_view_logical_field_id_get(
                         unit, info->flow_handle, "SRV6_DIP", &lfield_id));
            soc_mem_ip6_addr_get(unit, mem_view_id, entry, lfield_id,
                       info->dip6, SOC_MEM_IP6_UPPER_96BIT);
            info->valid_elements |= BCM_FLOW_TUNNEL_TERM_DIP6_VALID;
            SOC_IF_ERROR_RETURN(soc_flow_db_mem_view_logical_field_id_get(
                         unit, info->flow_handle, "SRH_NEXT_HDR", &lfield_id));
            info->next_hdr = soc_mem_field32_get(unit, mem_view_id, entry,
                                       lfield_id);
            if (info->next_hdr) {
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_NEXT_HDR_VALID;
            }
            SOC_IF_ERROR_RETURN(soc_flow_db_mem_view_logical_field_id_get(
                         unit, info->flow_handle, "PAYLOAD_OVID", &lfield_id));
            info->vlan = soc_mem_field32_get(unit, mem_view_id, entry,
                                             lfield_id);
            if (info->vlan) {
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_VLAN_VALID;
            }
        } else {
            BCM_IF_ERROR_RETURN(soc_flow_db_mem_view_field_list_get(unit,
                             mem_view_id,
                             SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                             SOC_FLOW_DB_MAX_VIEW_FIELDS, key_list, &key_count));
        
            if(key_count) {
                info->valid_elements |= BCM_FLOW_TUNNEL_TERM_FLEX_KEY_VALID;
            }
            for (i = 0; i < key_count; i++) {
                field[cnt].id = key_list[i];
                    field[cnt++].value = soc_mem_field32_get(unit,mem_view_id, entry,
                     key_list[i]);
            }
        }

        /* DATA - Flex VIew */    
        if (soc_flow_db_mem_view_field_is_valid
                       (unit, mem_view_id, TERM_TUNNELf)) {
    
            if (soc_mem_field32_get(unit, mem_view_id, entry,
                                   IGNORE_UDP_CHECKSUMf) == 0x0)
            {
                info->flags |= BCM_TUNNEL_TERM_UDP_CHECKSUM_ENABLE;
            }
            if (soc_mem_field32_get(unit, mem_view_id, entry,
                             USE_OUTER_HEADER_PHBf) ==  0x1) {
                info->flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP;
            }
            if (soc_mem_field32_get(unit, mem_view_id, entry,
                                       USE_OUTER_HEADER_PHBf) ==  0x2) {
                info->flags |= BCM_TUNNEL_TERM_USE_OUTER_PCP;
            }
            if (soc_mem_field32_get(unit, mem_view_id, entry, 
                    USE_OUTER_HDR_TTLf) ==  1) {
                 info->flags |=  BCM_TUNNEL_TERM_USE_OUTER_TTL;
            }
            if (soc_mem_field32_get(unit, mem_view_id, entry, 
                    DONOT_CHANGE_INNER_HDR_DSCPf) ==  1) {
                info->flags |= BCM_TUNNEL_TERM_KEEP_INNER_DSCP;
            }
            if(soc_mem_field32_get(unit, mem_view_id, entry, 
                    NETWORK_RECEIVERS_PRESENTf) == 0x01) {
               info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_ENABLE;
            } else if (soc_mem_field32_get(unit, mem_view_id, entry, 
                    NETWORK_RECEIVERS_PRESENTf) == 0x00) {
               info->multicast_flag |= BCM_FLOW_MULTICAST_TUNNEL_STATE_BUD_DISABLE;
            }
        }
        if (SOC_MEM_FIELD_VALID(unit, mem_view_id, SVP_ACTION_SETf)) {
            vp = soc_mem_field32_get(unit, mem_view_id, entry, SVP_ACTION_SETf);
            BCM_GPORT_FLOW_PORT_ID_SET(info->flow_port, vp);
            if (vp) {
                info->flags |= BCM_FLOW_TUNNEL_TERM_FLOW_PORT_VALID;
            }
        }
        if (SOC_MEM_FIELD_VALID(unit, mem_view_id, DESTINATION_ACTION_SETf)) {
            dest = soc_mem_field32_dest_get(unit, mem_view_id, entry,
                                       DESTINATION_ACTION_SETf, &dest_type);
            if (dest_type == SOC_MEM_FIF_DEST_NEXTHOP) {
                info->intf_id = dest + BCM_XGS3_EGRESS_IDX_MIN(unit);
                info->flags |= BCM_FLOW_TUNNEL_TERM_INTF_ID_VALID;
            }
        }
        if (dest_type == SOC_MEM_FIF_DEST_INVALID) {
            if (SOC_MEM_FIELD_VALID(unit, mem_view_id, L3_IIF_ACTION_SETf)) {
                info->intf_id = soc_mem_field32_get(unit, mem_view_id, entry,
                                           L3_IIF_ACTION_SETf);
                if (info->intf_id) {
                    info->flags |= BCM_FLOW_TUNNEL_TERM_INTF_ID_VALID;
                }
            }
        }
        if (SOC_MEM_FIELD_VALID(unit, mem_view_id, VFI_ACTION_SETf)) {
            vfi = soc_mem_field32_get(unit, mem_view_id, entry,
                                      VFI_ACTION_SETf);
            _BCM_FLOW_VPN_SET(info->vpn, _BCM_VPN_TYPE_VFI, vfi);
            if (vfi) {
                info->flags |= BCM_FLOW_TUNNEL_TERM_VPN_VALID;
            }
        }
        BCM_IF_ERROR_RETURN(
                    soc_flow_db_mem_view_field_list_get(unit,
                        mem_view_id,
                        SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                        SOC_FLOW_DB_MAX_VIEW_FIELDS, field_list, &data_count));
       
        if (data_count) {
            info->valid_elements |= BCM_FLOW_TUNNEL_TERM_FLEX_DATA_VALID;
        } 
        for (i = 0; i < data_count; i++) {
            field[cnt].id = field_list[i]; 
            field[cnt++].value =
                soc_mem_field32_get(unit, mem_view_id, entry, field_list[i]);
        }
        *num_of_fields = cnt;
                 
    }
    
    return BCM_E_NONE; 

}

/*
 * Function:
 *      _bcm_flow_tunnel_terminator_traverse_mem
 * Purpose:
 *      Read the given memory table and call the user callback function on
 *      each entry.
 * Parameters:
 *      unit          - (IN) Device Number
 *      mem           - (IN) the memory id
 *      cb            - (IN) user callback function
 *      user_data     - (IN) user context data
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_tunnel_terminator_mem_traverse(int unit, soc_mem_t mem,
                             bcm_flow_tunnel_terminator_traverse_cb cb,
                             void *user_data)
{


    bcm_flow_tunnel_terminator_t info;
    uint32 num_of_fields = _BCM_FLOW_LOGICAL_FIELD_MAX;
    bcm_flow_logical_field_t field[_BCM_FLOW_LOGICAL_FIELD_MAX];
    int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int             buf_size, chunksize, chnk_end;
    /* Buffer to store chunk of table we currently work on */
    uint32          *vt_tbl_chnk;
    uint32          *entry;
    uint32          trav_entry[_QUAD_ENTRY_MAX];
    int             valid = FALSE;
    int             rv = BCM_E_NONE;
    uint32 entry_width = _BCM_FLOW_TD3_L3_TUNNEL_SINGLE_ENTRY_SIZE;
    uint32 num_hw_entries = 0;
    int i = 0;

    if (INVALIDm == mem) {
        return BCM_E_MEMORY;
    }

    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_VLANDELETE_CHUNKS,
                                 VLAN_MEM_CHUNKS_DEFAULT);
  
    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    vt_tbl_chnk = soc_cm_salloc(unit, buf_size, "flow tunnel terminator traverse");
    if (NULL == vt_tbl_chnk) {
        return BCM_E_MEMORY;
    }
    
    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem);
         chnk_idx <= mem_idx_max;
         chnk_idx += chunksize) {
        sal_memset((void *)vt_tbl_chnk, 0, buf_size);

        chnk_idx_max =
            ((chnk_idx + chunksize) <= mem_idx_max) ?
            chnk_idx + chunksize - 1: mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, vt_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ) {
            entry =
                soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                             vt_tbl_chnk, ent_idx);            
            rv = _bcm_flow_tunnel_terminator_entry_valid(unit, 
                              mem, entry, &valid);
            if (BCM_FAILURE(rv)) {
                break;
            }
            if (!valid) {
                ent_idx++;
                continue;
            }
            if (mem == L3_TUNNELm) {
                rv = _bcm_esw_flow_tnl_term_num_hw_entries_get(unit, mem, 
                             entry, &num_hw_entries); 
                if (BCM_FAILURE(rv)) {
                    break;
                }
                for (i= 0; i < num_hw_entries; i++) {                    
                    sal_memcpy((uint8 *)trav_entry + entry_width * i,
                                entry, entry_width);
                    ent_idx++;
                    entry = soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                             vt_tbl_chnk, ent_idx);
                }
            } else {
                sal_memcpy(trav_entry, entry,
                      WORDS2BYTES(soc_mem_entry_words(unit, mem)));
                ent_idx++;
            }   
            bcm_flow_tunnel_terminator_t_init(&info);
             
            rv = _bcm_flow_tunnel_terminator_entry_parse(unit, mem, trav_entry, &info,
                    &num_of_fields,field);
            if (rv == BCM_E_NOT_FOUND) { /* for entries not exposed to user */
                rv = BCM_E_NONE;
                continue;
            } else if (BCM_FAILURE(rv)) {
                break;
            }
            rv = cb(unit, &info, num_of_fields,field,user_data);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }
    soc_cm_sfree(unit, vt_tbl_chnk);
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_tunnel_terminator_traverse
 * Purpose:
 *      Traverse all flow tunnel initiator objects and call the user specified
 * callback function on each object.
 *
 * Parameters:
 *      unit      - (IN) Device Number
 *      bcm_flow_tunnel_initiator_traverse_cb - (IN) user callback function 
 *      user_data - (IN) user context data for the callback 
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_esw_flow_tunnel_terminator_traverse(
    int unit,
    bcm_flow_tunnel_terminator_traverse_cb cb,
    void *user_data)
{
    soc_mem_t mem; 
 
    if (NULL == cb) {
        return (BCM_E_PARAM);
    } 
    mem = VLAN_XLATE_1_DOUBLEm; 
    BCM_IF_ERROR_RETURN(
          _bcm_flow_tunnel_terminator_mem_traverse(unit, mem, cb, user_data));
    
    mem = MPLS_ENTRYm; /* MPLS_ENTRY_DOUBLE */
    BCM_IF_ERROR_RETURN(
           _bcm_flow_tunnel_terminator_mem_traverse(unit, mem, cb, user_data));
    
    mem = L3_TUNNELm;
    BCM_IF_ERROR_RETURN(
           _bcm_flow_tunnel_terminator_mem_traverse(unit, mem, cb, user_data));
    
    mem = MPLS_ENTRY_SINGLEm;
    BCM_IF_ERROR_RETURN(
           _bcm_flow_tunnel_terminator_mem_traverse(unit, mem, cb, user_data));
    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_flow_tunnel_terminator_entry_term
 * Description   :
 *      Determine whether this is a tunnel terminator entry.
 * Parameters   :
 *      unit             - (IN) Device Number
 *      mem              - (IN)  memory id.
 *      entry            - (IN) HW entry.
 *      term            - (OUT) Tunnel terminator entry.
 */
STATIC int
_bcm_flow_tunnel_terminator_entry_term(int unit,
                             soc_mem_t mem,
                             uint32 *entry,
                             int *term)
{
    int key_type = SOC_FLOW_DB_KEY_TYPE_INVALID;
    int data_type = SOC_FLOW_DB_KEY_TYPE_INVALID;
    int i;
    soc_flow_db_mem_view_info_t vinfo;
    soc_flow_db_ffo_t ffo[SOC_FLOW_DB_MAX_VIEW_FFO_TPL];
    uint32 num_ffo = 0;
    uint32 key_typef[] = {KEY_TYPEf, KEY_TYPE0f, KEY_TYPE1f};
    uint32 data_typef[] = {DATA_TYPEf, DATA_TYPE0f, DATA_TYPE1f};

    uint32 mem_view_id = 0;
    int rv = BCM_E_NONE;

    for (i = 0; i < SOC_FLOW_DB_MAX_KEY_TYPE; i++) {
        if (SOC_MEM_FIELD_VALID(unit,mem,key_typef[i])) {
            key_type = soc_mem_field32_get(unit, mem, entry, key_typef[i]);
            break;
        }
    }
    for (i = 0; i < SOC_FLOW_DB_MAX_DATA_TYPE; i++) {
        if (SOC_MEM_FIELD_VALID(unit,mem,data_typef[i])) {
            data_type = soc_mem_field32_get(unit, mem, entry, data_typef[i]);
        }
    }

    /* find the memory view based on the memory and key type */
    rv = soc_flow_db_mem_to_view_id_get (unit, mem, key_type, data_type,
        0, NULL, (uint32 *) &mem_view_id);

    if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
        return rv;
    }

    *term = 0;
    if (rv == BCM_E_NOT_FOUND) {
        /* Fixed view */
        if ((key_type == _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP) ||
            (key_type == _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP)) {
            *term = 1;
        } else {
            *term = 0;
        }
    } else {
        /* Flex view */
        /* find the memory view info */
        BCM_IF_ERROR_RETURN(
            soc_flow_db_mem_view_info_get(unit, mem_view_id, &vinfo));

        /* get the flow handle, function id, option id */
        BCM_IF_ERROR_RETURN(
            soc_flow_db_mem_view_to_ffo_get(unit,
                mem_view_id,
                SOC_FLOW_DB_MAX_VIEW_FFO_TPL,
                ffo,
                &num_ffo));


        if(ffo[0].function_id == SOC_FLOW_DB_FUNC_TUNNEL_TERMINATOR_ID) {
            *term = 1;
        } else {
            *term = 0;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_flow_tunnel_terminator_destroy_all
 * Purpose:
 *      Destroy all flow tunnel terminator entry
 *
 * Parameters:
 *      unit          - (IN)     Device Number
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_esw_flow_tunnel_terminator_destroy_all(
    int unit)
{
    soc_mem_t tunnel_mems[4] = {VLAN_XLATE_1_DOUBLEm, MPLS_ENTRYm,
                                    L3_TUNNELm, INVALIDm};
    int mem_index = 0;
    soc_tunnel_term_t tnl_entry;
    soc_tunnel_term_t ret_tnl_entry;
    uint32 ret_entry[SOC_MAX_MEM_WORDS];
    int index = 0;
    int chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int buf_size, chunksize, chnk_end;
    uint32 *tbl_chnk;
    uint32 *entry;
    int valid = FALSE;
    uint32 num_hw_entries = 0;
    int i = 0;
    soc_mem_t mem;
    int term = 0;
    int rv = BCM_E_NONE;

    if ((SOC_HW_RESET(unit) ||
        (SOC_SWITCH_BYPASS_MODE(unit) != SOC_SWITCH_BYPASS_MODE_NONE))) {
        return BCM_E_NONE;
    }

    while (INVALIDm != tunnel_mems[mem_index]){
        mem = tunnel_mems[mem_index++];
        if (!soc_mem_index_count(unit, mem)) {
            continue;
        }

        chunksize = 1024;
        buf_size = soc_mem_entry_words(unit, mem) * sizeof(uint32) * chunksize;
        tbl_chnk = soc_cm_salloc(unit, buf_size, "flow tunnel term destroy");
        if (NULL == tbl_chnk) {
            return BCM_E_MEMORY;
        }

        mem_idx_max = soc_mem_index_max(unit, mem);

        for (chnk_idx = soc_mem_index_min(unit, mem);
                chnk_idx <= mem_idx_max;
                chnk_idx += chunksize) {
            sal_memset((void *)tbl_chnk, 0, buf_size);

            chnk_idx_max =
                ((chnk_idx + chunksize) <= mem_idx_max) ?
                chnk_idx + chunksize - 1: mem_idx_max;

            rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                chnk_idx, chnk_idx_max, tbl_chnk);
            if (SOC_FAILURE(rv)) {
                break;
            }

            chnk_end = (chnk_idx_max - chnk_idx);
            for (ent_idx = 0 ; ent_idx <= chnk_end; ) {
                entry =
                    soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                        tbl_chnk, ent_idx);
                rv = _bcm_flow_tunnel_terminator_entry_valid(unit,
                        mem, entry, &valid);
                if (BCM_FAILURE(rv)) {
                    break;
                }
                if (!valid) {
                    ent_idx++;
                    continue;
                }
                if (mem == L3_TUNNELm) {
                    rv = _bcm_esw_flow_tnl_term_num_hw_entries_get(unit, mem,
                            entry, &num_hw_entries);
                    if (BCM_FAILURE(rv)) {
                        break;
                    }
                    for (i= 0; i < num_hw_entries; i++) {
                        ent_idx++;
                        entry = soc_mem_table_idx_to_pointer(unit, mem,
                            uint32 *,tbl_chnk, ent_idx);
                    }
                } else {
                    ent_idx++;
                }

                term = 0;
                rv = _bcm_flow_tunnel_terminator_entry_term(unit, mem,
                        entry, &term);
                if (BCM_FAILURE(rv)) {
                    break;
                } else if (term){
                    if (mem == L3_TUNNELm) {
                        rv = bcmi_esw_flow_entry_to_tnl_term(unit, entry,
                                &tnl_entry);
                        if (rv == BCM_E_NONE) {
                            rv = soc_tunnel_term_match(unit, &tnl_entry,
                                &ret_tnl_entry, &index);
                        }
                        if (rv == BCM_E_NONE) {
                            rv = soc_tunnel_term_delete(unit, &ret_tnl_entry);
                        }
                    } else {
                        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                            entry, ret_entry, 0);
                        if (rv == BCM_E_NONE) {
                            rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL,
                                &ret_entry);
                        }
                    }

                    if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE)) {
                        break;
                    }
                } else {
                    continue;
                }
            }
            /* error detect, ignore this table */
            if (ent_idx <= chnk_end ) {
                break;
            }
        }
        soc_cm_sfree(unit, tbl_chnk);
    }

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *      _bcm_flow_tunnel_initiator_reinit
 * Purpose:
 *      Tunnel initiator hash and ref-count recovery.
 * Parameters:
 *      unit          - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_flow_tunnel_initiator_reinit(int unit)
{
    bcm_flow_tunnel_initiator_t tnl_init_info;
    int rv = BCM_E_NONE;
    bcm_flow_logical_field_t field[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32 cnt;
    _bcm_flow_tunnel_record_t tnl_rec;
    int ipv6;
    int tnl_idx;
    int oif_idx;
    _bcm_flow_bookkeeping_t *flow_info;
    int soft_tnl_idx;
    bcm_gport_t tunnel_id;

    flow_info = FLOW_INFO(unit);
    for (soft_tnl_idx = 0; soft_tnl_idx < _BCM_FLOW_TUNNEL_ID_MAX(unit);
                           soft_tnl_idx++) {

        /* check if tunnel ID is valid */
        if (!flow_info->init_tunnel[soft_tnl_idx].idx) {
            continue;
        }
        BCM_GPORT_TUNNEL_ID_SET(tunnel_id, soft_tnl_idx);
        rv = _bcm_flow_tunnel_and_oif_index_get(unit, tunnel_id,
                               &oif_idx, &tnl_idx);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        } else if (rv == BCM_E_NOT_FOUND) {
            continue;
        }

        bcm_flow_tunnel_initiator_t_init(&tnl_init_info);
        rv = _bcm_flow_hw_tunnel_entry_get(unit, tnl_idx, &tnl_init_info,field,
                         _BCM_FLOW_LOGICAL_FIELD_MAX,&cnt);
        if (rv == SOC_E_NOT_FOUND) {
            continue;
        }
        BCM_IF_ERROR_RETURN(rv);

        tnl_rec.info = &tnl_init_info;
        tnl_rec.field = field;
        tnl_rec.num_of_fields = cnt;

        rv = _bcm_flow_tunnel_initiator_hash_calc(unit, &tnl_rec,
                    &BCM_XGS3_L3_ENT_HASH(BCM_XGS3_L3_TBL_PTR(
                        unit, tnl_init), tnl_idx));
        BCM_IF_ERROR_RETURN(rv);
        ipv6 = _BCM_TUNNEL_OUTER_HEADER_IPV6(tnl_init_info.type) ||
                          tnl_init_info.type == bcmTunnelTypeSR6 ||
                          tnl_init_info.type == bcmTunnelTypeEthSR6;
        BCM_XGS3_L3_ENT_REF_CNT_INC
                (BCM_XGS3_L3_TBL_PTR(unit, tnl_init), tnl_idx,
                    (ipv6 ? _BCM_DOUBLE_WIDE : _BCM_SINGLE_WIDE));

        if (soft_tnl_idx >= _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit)) {
            /* check if srv6 */
            if (_BCM_FLOW_SRV6_ENABLED(unit)) {
                rv = _bcm_flow_tunnel_srh_reinit(unit, soft_tnl_idx);
                if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                    return rv;
                }
            }
        }
    }
    return BCM_E_NONE;
}
#endif

#endif  /* INCLUDE_L3 */


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
#include <soc/format.h>
#include <bcm_int/esw/vpn.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/vxlan.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/api_xlate_port.h>
#include <soc/esw/flow_db.h>
#include <soc/esw/cancun.h>

#define VIRTUAL_INFO(_unit_)    (&_bcm_virtual_bk_info[_unit_])
#define PORT_DEFAULT_ALL (BCM_FLOW_PORT_DEFAULT | BCM_FLOW_PORT_DEFAULT_L2GRE | \
		BCM_FLOW_PORT_DEFAULT_GPE | BCM_FLOW_PORT_DEFAULT_GENEVE)
 
STATIC int
_bcm_td3_flow_default_port_get(int unit, int vp, bcm_flow_port_t *flow_port);
STATIC int
_bcmi_esw_flow_port_get(int unit, bcm_vpn_t vpn, int vp, bcm_flow_port_t  *flow_port);

  /* Function:
 *      bcmi_esw_flow_vp_is_eline
 * Purpose:
 *      Find if given FLOW VP is ELINE
 * Parameters:
 *      unit     - Device Number
 *      vp   - FLOW VP
 *      isEline  - Flag 
 * Returns:
 *      BCM_E_XXXX
 * Assumes:
 *      l2vpn is valid
 */

int
bcmi_esw_flow_vp_is_eline( int unit, int vp, uint8 *isEline)
{
    source_vp_entry_t svp;
    vfi_entry_t vfi_entry;
    int rv = BCM_E_PARAM;
    int vfi_index;

    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
        return BCM_E_NOT_FOUND;
    }

    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        return rv;
    }

    vfi_index = soc_SOURCE_VPm_field32_get(unit, &svp, VFIf);
    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    /* Set the returned VPN id */
    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
       *isEline = 0x1;
    }  else {
       *isEline = 0x0;
   }
   return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_flow_nexthop_glp_get
 * Purpose:
 *      Get the modid, port, trunk values for a FLOW nexthop
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td3_flow_nexthop_glp_get(int unit, int nh_idx,
        bcm_module_t *modid, bcm_port_t *port, bcm_trunk_t *trunk_id)

{
    ing_l3_next_hop_entry_t ing_nh;
    uint32 dt, dv;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit,
        ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, nh_idx, &ing_nh));

    dv = soc_mem_field32_dest_get(unit,
            ING_L3_NEXT_HOPm, &ing_nh, DESTINATIONf, &dt);
    if (dt == SOC_MEM_FIF_DEST_LAG) {
        *trunk_id = dv & SOC_MEM_FIF_DGPP_TGID_MASK;
    } else if (dt == SOC_MEM_FIF_DEST_DGPP) {
        *port = (dv & SOC_MEM_FIF_DGPP_PORT_MASK);
        *modid = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                    SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
    } else if (dt == SOC_MEM_FIF_DEST_DVP) {
        return BCM_E_NOT_FOUND;
    } else {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_flow_port_resolve
 * Purpose:
 *      Get the modid, port, trunk values for a FLOW port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_flow_port_resolve(int unit, bcm_gport_t flow_port_id, 
                          bcm_if_t encap_id, bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id)

{
    int rv = BCM_E_NONE;
    egr_l3_next_hop_entry_t egr_nh;
    ing_dvp_table_entry_t dvp;
    int ecmp=0, nh_index=-1, nh_ecmp_index=-1, vp=-1;
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read hw entry. */
    int  idx, max_ent_count, base_idx;

    rv = bcmi_esw_flow_check_init(unit);
    if (rv < 0) {
        return rv;
    }

    if (!BCM_GPORT_IS_FLOW_PORT(flow_port_id)) {
        return (BCM_E_BADID);
    }

    vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port_id);
    if (vp == -1) {
       return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
        return BCM_E_NOT_FOUND;
    }
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf);
    if (!ecmp) {
        nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, 
                                              NEXT_HOP_INDEXf);
        BCM_IF_ERROR_RETURN(_bcm_td3_flow_nexthop_glp_get(unit,
                                nh_index, modid, port, trunk_id));
    } else {
         /* Select the desired nhop from ECMP group - pointed by encap_id */
         nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMP_PTRf);
         BCM_IF_ERROR_RETURN(
              soc_mem_read(unit, L3_ECMP_COUNTm, MEM_BLOCK_ANY, 
                        nh_ecmp_index, hw_buf));

         if (soc_feature(unit, soc_feature_l3_ecmp_1k_groups)) {
              max_ent_count = soc_mem_field32_get(unit, L3_ECMP_COUNTm, 
                                                 hw_buf, COUNTf);
              base_idx = soc_mem_field32_get(unit, L3_ECMP_COUNTm, 
                                                 hw_buf, BASE_PTRf);
         }  else {
              max_ent_count = soc_mem_field32_get(unit, L3_ECMP_COUNTm, 
                                                 hw_buf, COUNT_0f);
              base_idx = soc_mem_field32_get(unit, L3_ECMP_COUNTm, 
                                                 hw_buf, BASE_PTR_0f);
         }
        max_ent_count++; /* Count is zero based. */ 

        if(encap_id == -1) {
              BCM_IF_ERROR_RETURN(
                   soc_mem_read(unit, L3_ECMPm, MEM_BLOCK_ANY, 
                        base_idx, hw_buf));
              nh_index = soc_mem_field32_get(unit, L3_ECMPm, 
                   hw_buf, NEXT_HOP_INDEXf);

              BCM_IF_ERROR_RETURN(_bcm_td3_flow_nexthop_glp_get(unit,
                                    nh_index, modid, port, trunk_id));
        } else {
            for (idx = 0; idx < max_ent_count; idx++) {
              BCM_IF_ERROR_RETURN(
                   soc_mem_read(unit, L3_ECMPm, MEM_BLOCK_ANY, 
                        (base_idx+idx), hw_buf));
              nh_index = soc_mem_field32_get(unit, L3_ECMPm, 
                   hw_buf, NEXT_HOP_INDEXf);
              BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, 
                   MEM_BLOCK_ANY, nh_index, &egr_nh));
              if (encap_id == soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, 
                                               &egr_nh, INTF_NUMf)) {
                  BCM_IF_ERROR_RETURN(_bcm_td3_flow_nexthop_glp_get(unit,
                                          nh_index, modid, port, trunk_id));
                break;
              }
            }
        }
    }
    *id = vp;
    return rv;
}

/*
 * Function:
 *      _bcm_td3_flow_port_cnt_update
 * Purpose:
 *      Update port's VP count.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      gport - (IN) GPORT ID.
 *      vp    - (IN) Virtual port number.
 *      incr  - (IN) If TRUE, increment VP count, else decrease VP count.
 * Returns:
 *      BCM_X_XXX
 */

STATIC int
_bcm_td3_flow_port_cnt_update(int unit, bcm_gport_t gport,
        int vp, int incr_decr_flag)
{
    int mod_out=-1, port_out=-1, tgid_out=-1, vp_out=-1;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count=0;
    int idx=-1;
    int mod_local=-1;
    _bcm_port_info_t *port_info;
    uint32 port_flags;

    BCM_IF_ERROR_RETURN(
       _bcm_td3_flow_port_resolve(unit, gport, -1, &mod_out,
                    &port_out, &tgid_out, &vp_out));

    if (vp_out == -1) {
       return BCM_E_PARAM;
    }

    /* Update the physical port's SW state. If associated with a trunk,
     * update each local physical port's SW state.
     */

    if (BCM_TRUNK_INVALID != tgid_out) {

        BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid_out, 
                    SOC_MAX_NUM_PORTS, local_member_array, &local_member_count));

        for (idx = 0; idx < local_member_count; idx++) {
            _bcm_port_info_access(unit, local_member_array[idx], &port_info);
            if (incr_decr_flag) {
                port_info->vp_count++;
            } else {
                port_info->vp_count--;
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_get(
                    unit, local_member_array[idx], &port_flags));
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_set(
                    unit, local_member_array[idx], port_flags));
        }
    } else {
#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
             (soc_feature(unit, soc_feature_channelized_switching))) &&
            (_bcm_xgs5_subport_coe_mod_port_local(unit, mod_out, port_out))) {
            BCM_IF_ERROR_RETURN(
                _bcmi_coe_subport_mod_port_physical_port_get(
                    unit, mod_out, port_out, &port_out));
            mod_local = 1;
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, mod_out, &mod_local));
            if (mod_local && soc_feature(unit, soc_feature_sysport_remap)) {
                BCM_XLATE_SYSPORT_S2P(unit, &port_out);
            }
        }
        if (mod_local) {
            if (soc_feature(unit, soc_feature_sysport_remap)) { 
                BCM_XLATE_SYSPORT_S2P(unit, &port_out); 
            }
            _bcm_port_info_access(unit, port_out, &port_info);
            if (incr_decr_flag) {
                port_info->vp_count++;
            } else {
                port_info->vp_count--;
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_get(
                    unit, port_out, &port_flags));
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_set(
                    unit, port_out, port_flags));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_flow_eline_vp_map_set
 * Purpose:
 *      Set FLOW ELINE ports from VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td3_flow_eline_vp_map_set(int unit, int vfi_index, int vp1, int vp2)
{
    vfi_entry_t vfi_entry;
    int rv=BCM_E_NONE;
    int num_vp=0;

    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeFlow)) {
        return BCM_E_NOT_FOUND;
    }
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
         if ((vp1 > 0) && (vp1 < num_vp)) {
              soc_VFIm_field32_set(unit, &vfi_entry, VP_0f, vp1);
         } else {
              return BCM_E_PARAM;
         }
         if ((vp2 > 0) && (vp2 < num_vp)) {
              soc_VFIm_field32_set(unit, &vfi_entry, VP_1f, vp2);
         } else {
              return BCM_E_PARAM;
         }
    } else {
         /* ELAN */
         return BCM_E_PARAM;
    }

    rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry);
    return rv;
}
/*
 * Function:
 *      _bcm_td3_flow_eline_vp_map_get
 * Purpose:
 *      Get FLOW ELINE ports from VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td3_flow_eline_vp_map_get(int unit, int vfi_index, int *vp1, int *vp2)
{
    vfi_entry_t vfi_entry;

    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeFlow)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        *vp1 = soc_VFIm_field32_get(unit, &vfi_entry, VP_0f);
        *vp2 = soc_VFIm_field32_get(unit, &vfi_entry, VP_1f);
         return BCM_E_NONE;
    } else {
         return BCM_E_PARAM;
    }
}

/*
 * Function:
 *      _bcm_td3_flow_eline_vp_map_clear
 * Purpose:
 *      Clear FLOW ELINE ports from VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td3_flow_eline_vp_map_clear(int unit, int vfi_index, int vp1, int vp2)
{
    vfi_entry_t vfi_entry;
    int rv=BCM_E_NONE;
    int num_vp=0;

    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeFlow)) {
        return BCM_E_NOT_FOUND;
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        if ((vp1 > 0) && (vp1 < num_vp)) {
            soc_VFIm_field32_set(unit, &vfi_entry, VP_0f, 0);
        }
        if ((vp2 > 0) && (vp2 < num_vp)) {
            soc_VFIm_field32_set(unit, &vfi_entry, VP_1f, 0);
        }
    } else {
        /* ELAN */
        return BCM_E_PARAM;
    }
    rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry);
    return rv;
}

/*
 * Function:
 *           _bcm_td3_flow_next_hop_set
 * Purpose:
 *           Set FLOW NextHop Entry.
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
_bcm_td3_flow_next_hop_set(int unit, int nh_index, uint32 flags, int vp, int drop)
{
    int rv = BCM_E_NONE;
    ing_l3_next_hop_entry_t ing_nh;
    bcm_port_t  port = -1;
    bcm_module_t modid = -1, local_modid = -1;
    bcm_trunk_t tgid = -1;
    int  num_ports=0, idx=-1;
    int  old_nh_index=-1;
    int replace=0;
    int gport = 0;
    _bcm_vp_info_t vp_info;
    soc_mem_t hw_mem = ING_L3_NEXT_HOPm;
    bcm_trunk_member_t *tmp_member_gports_array = NULL;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit,
        hw_mem, MEM_BLOCK_ANY, nh_index, &ing_nh));
    soc_mem_field32_set(unit, hw_mem, &ing_nh,
        ENTRY_TYPEf, _BCM_VXLAN_INGRESS_NEXT_HOP_ENTRY_TYPE);
    if (SOC_MEM_FIELD_VALID(unit, hw_mem, MTU_SIZEf)) {
        soc_mem_field32_set(unit, hw_mem, &ing_nh, MTU_SIZEf, 0x3fff);
    }
    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, hw_mem, MEM_BLOCK_ALL, nh_index, &ing_nh));

    if (flags & BCM_L3_IPMC) {
        egr_l3_next_hop_entry_t egr_nh;
        int entry_type = 0;
        int hw_mem = EGR_L3_NEXT_HOPm;

        (void)_bcm_vp_info_get(unit, vp, &vp_info);

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, hw_mem, MEM_BLOCK_ALL,
                                              nh_index, &egr_nh));
        entry_type = soc_mem_field32_get(unit, hw_mem, &egr_nh, DATA_TYPEf);

        if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) { /* network VP */

            /* most should be setup already */
            if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW) { 
                soc_mem_field32_set(unit, hw_mem, &egr_nh, L3MC__HG_HDR_SELf, 
                                    1);
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L3_DROPf, drop ? 1 : 0);
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, hw_mem,
                                  MEM_BLOCK_ALL, nh_index, &egr_nh));
            } else {
                /* no change for all other views  */
            }
        } else {   /* access VP */
            /* fixed view should use sd_tag view for access VP */
            if (!BCMI_L3_NH_FLEX_VIEW(entry_type)) {
                if (entry_type != _BCM_VXLAN_EGR_NEXT_HOP_SDTAG_VIEW) {
                    /* first time setup */
                    sal_memset(&egr_nh, 0, sizeof(egr_nh));
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            DATA_TYPEf, _BCM_VXLAN_EGR_NEXT_HOP_SDTAG_VIEW);
                }
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            SD_TAG__DVPf, vp);
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, hw_mem,
                                  MEM_BLOCK_ALL, nh_index, &egr_nh));
            } else {
                /* proper process for flex views should be done already */
            }
        }
    } else { /* Only for FLOW Unicast Nexthops */
        replace = flags & BCM_L3_REPLACE;

        BCM_IF_ERROR_RETURN(_bcm_td3_flow_nexthop_glp_get(unit,
                                nh_index, &modid, &port, &tgid));

        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &local_modid));
        if (tgid != -1) {
             BCM_IF_ERROR_RETURN(_bcm_trunk_id_validate(unit, tgid));
             rv = bcm_esw_trunk_get(unit, tgid, NULL, 0, NULL, &num_ports);
             if (BCM_FAILURE(rv)) {
                 return BCM_E_PORT;
             }
             if (num_ports == 0) {
                 return BCM_E_NONE;
             }
             if(soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
                 tmp_member_gports_array = sal_alloc(
                     sizeof(bcm_trunk_member_t) * num_ports, "tmp_member_gports_arry");
                 if (tmp_member_gports_array == NULL) {
                     return BCM_E_MEMORY;
                 }
                 sal_memset(tmp_member_gports_array, 0,
                     sizeof(bcm_trunk_member_t) * num_ports);
                 rv = bcm_esw_trunk_get(unit, tgid, NULL,
                          num_ports, tmp_member_gports_array, &num_ports);
                 if (BCM_FAILURE(rv)) {
                     goto clean_up;
                 }
             }
        } else {
            if (modid != local_modid) {
                if (!soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
                    /* Ignore EGR_PORT_TO_NHI_MAPPINGs */
                    return BCM_E_NONE;
                }
            }

            if(soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
                /* Alloc one trunk member size memory */
                tmp_member_gports_array = sal_alloc(
                    sizeof(bcm_trunk_member_t), "tmp_member_gports_arry");
                if (tmp_member_gports_array == NULL) {
                    return BCM_E_MEMORY;
                }
                sal_memset(tmp_member_gports_array, 0,
                    sizeof(bcm_trunk_member_t));
                BCM_GPORT_MODPORT_SET(gport, modid, port);
                tmp_member_gports_array[num_ports++].gport = gport;
            }
        }

       for (idx = 0; idx < num_ports; idx++) {
            if(soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
                gport = tmp_member_gports_array[idx].gport;
            }

            rv = _bcm_trx_gport_to_nhi_get(unit, gport, &old_nh_index);
            if (BCM_FAILURE(rv)) {
                 goto clean_up;
            }
            if (old_nh_index && !replace) {
                if (old_nh_index != nh_index) {
                    /* Limitation: For TD/TR3/TD2,
                       VxLAN egress port can be configured with one NHI */
                    rv = BCM_E_RESOURCE;
                    goto clean_up;
                }
            } else {
                rv = _bcm_trx_gport_to_nhi_set(unit, gport, nh_index);
                if (BCM_FAILURE(rv)) {
                    goto clean_up;
                }
            }
         }
    }

clean_up:
    if (NULL != tmp_member_gports_array) {
        sal_free(tmp_member_gports_array);
    }
    return rv;
}

/*
 * Function:
 *      bcm_td3_flow_nexthop_reset
 * Purpose:
 *      Reset VXLAN NextHop entry
 * Parameters:
 *      IN :  Unit
 *           IN :  nh_index
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td3_flow_nexthop_reset(int unit, int nh_index)
{
    int rv=BCM_E_NONE;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    bcm_port_t      port=0;
    bcm_trunk_t tgid= BCM_TRUNK_INVALID;
    int  num_ports=0, idx=-1;
    soc_mem_t      hw_mem;
    bcm_module_t modid=0, local_modid=0;
    uint8 multicast_entry=0;
    int  old_nh_index=-1;
    int gport = 0;
    uint32 entry_type=0;
    uint32 view_id;
    uint32 action_set;
    uint32 l3_intf;
    int nh_index_out;
    bcm_trunk_member_t *tmp_member_gports_array = NULL;

    hw_mem = EGR_L3_NEXT_HOPm;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, hw_mem,
                                  MEM_BLOCK_ANY, nh_index, &egr_nh));

    if (SOC_MEM_FIELD_VALID(unit, hw_mem, DATA_TYPEf)) {
        entry_type = soc_mem_field32_get(unit, hw_mem, &egr_nh, DATA_TYPEf);
    }

    if (BCMI_L3_NH_FLEX_VIEW(entry_type)) {
        /* Get view id corresponding to the entry type. */
        rv = soc_flow_db_mem_to_view_id_get(unit,
                                     EGR_L3_NEXT_HOPm,
                                     SOC_FLOW_DB_KEY_TYPE_INVALID,
                                     entry_type,
                                     0,
                                     NULL,
                                     &view_id);
        BCM_IF_ERROR_RETURN(rv);

        if (SOC_MEM_FIELD_VALID(unit, view_id, INTF_ACTION_SETf)) {
           action_set = soc_mem_field32_get(unit, view_id, &egr_nh,
                                   INTF_ACTION_SETf);

           l3_intf = soc_format_field32_get(unit, INTF_ACTION_SETfmt,
                                   &action_set, INTF_PTRf); /* INTF_NUMf */

            /* determine if this is an IPMC entry from the l3 interface */
            _bcm_th_ipmc_l3_intf_next_hop_get(unit, l3_intf,
                                              &nh_index_out);
            if (nh_index_out == -2) {
                multicast_entry = 1;
            }
        }
    }
    else if (_BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW == entry_type) {
       if (soc_feature(unit, soc_feature_egr_l3_next_hop_next_ptr)) {
           if (0x1 == soc_mem_field32_get(unit, hw_mem, &egr_nh,
                            L3MC__NEXT_PTR_TYPEf)) {
               multicast_entry = 1; /* Multicast NextHop */
           }
       }
    }

    hw_mem = ING_L3_NEXT_HOPm;
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, hw_mem,
                                  MEM_BLOCK_ANY, nh_index, &ing_nh));
    soc_mem_field32_set(unit, hw_mem, &ing_nh,
                                            DROPf, 0x0);
    soc_mem_field32_set(unit, hw_mem, &ing_nh,
                                            ENTRY_TYPEf, 0x0);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, hw_mem,
                                            MEM_BLOCK_ALL, nh_index, &ing_nh));
    rv = _bcm_td3_flow_nexthop_glp_get(unit,
                                nh_index, &modid, &port, &tgid);
    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_NOT_FOUND) {
            /* not a GLP, could be a DVP in riot case. No further process */
            return BCM_E_NONE;
        } else {
            return rv;
        }
    }

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get (unit, &local_modid));
    if (tgid != BCM_TRUNK_INVALID) {
         rv = _bcm_trunk_id_validate(unit, tgid);
         if (BCM_FAILURE(rv)) {
            return BCM_E_PORT;
         }
         rv = bcm_esw_trunk_get(unit, tgid, NULL, 0, NULL, &num_ports);
         if (BCM_FAILURE(rv)) {
             return BCM_E_PORT;
         }
         if (num_ports == 0) {
             return BCM_E_NONE;
         }

         if(soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
             tmp_member_gports_array = sal_alloc(
                 sizeof(bcm_trunk_member_t) * num_ports, "tmp_member_gports_arry");
             if (tmp_member_gports_array == NULL) {
                 return BCM_E_MEMORY;
             }
             sal_memset(tmp_member_gports_array, 0,
                        sizeof(bcm_trunk_member_t) * num_ports);
             rv = bcm_esw_trunk_get(unit, tgid, NULL,
                          num_ports, tmp_member_gports_array, &num_ports);
             if (BCM_FAILURE(rv)) {
                 goto clean_up;
             }
         }
    } else {
        if (modid != local_modid) {
            if (!soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
                /* Ignore EGR_PORT_TO_NHI_MAPPINGs */
                return BCM_E_NONE;
            }
        }
        if(soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
            /* Alloc one trunk member size memory */
            tmp_member_gports_array = sal_alloc(
                 sizeof(bcm_trunk_member_t), "tmp_member_gports_arry");
            if (tmp_member_gports_array == NULL) {
                 return BCM_E_MEMORY;
            }
            sal_memset(tmp_member_gports_array, 0,
                       sizeof(bcm_trunk_member_t));
            BCM_GPORT_MODPORT_SET(gport, modid, port);
            tmp_member_gports_array[num_ports++].gport = gport;
        }
    }

    if (!multicast_entry) {
        for (idx = 0; idx < num_ports; idx++) {
            if(soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
                gport = tmp_member_gports_array[idx].gport;
            }

            rv = _bcm_trx_gport_to_nhi_get(unit, gport, &old_nh_index);
            if (BCM_FAILURE(rv)) {
                 goto clean_up;
            }
            if (old_nh_index == nh_index) {
                rv = _bcm_trx_gport_to_nhi_set(unit, gport, 0);
                if (BCM_FAILURE(rv)) {
                    goto clean_up;
                }
            }
        }
    }

clean_up:
    if (NULL != tmp_member_gports_array) {
        sal_free(tmp_member_gports_array);
    }
    return rv;
}
/*
 * Function:
 *           _bcm_td3_flow_egress_reset
 * Purpose:
 *           Reset Flow Egress NextHop
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
_bcm_td3_flow_egress_reset(int unit, int nh_index)
{
    ing_l3_next_hop_entry_t ing_nh;
    soc_mem_t      hw_mem;
    int rv=BCM_E_NONE;

    hw_mem = ING_L3_NEXT_HOPm;
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, hw_mem,
                MEM_BLOCK_ANY, nh_index, &ing_nh));
    soc_mem_field32_set(unit, hw_mem, &ing_nh,
                ENTRY_TYPEf, 0x0);
    if (SOC_MEM_FIELD_VALID(unit, ING_L3_NEXT_HOPm, MTU_SIZEf)) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                &ing_nh, MTU_SIZEf, 0x0);
    }
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, hw_mem,
                                  MEM_BLOCK_ALL, nh_index, &ing_nh));

    rv = _bcm_td3_flow_nexthop_reset(unit, nh_index);
    return rv;
}
/*
 * Function:
 *           _bcm_td3_flow_egress_get
 * Purpose:
 *           Get FLOW Egress NextHop
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
_bcm_td3_flow_egress_get(int unit, bcm_l3_egress_t *egr, int nh_index)
{
    int int_l3_flag;
    int_l3_flag = BCM_XGS3_L3_ENT_FLAG(BCM_XGS3_L3_TBL_PTR(unit, next_hop),
                                       nh_index);
    if (int_l3_flag == _BCM_L3_FLOW_ONLY) {
         egr->flags2 |= BCM_L3_FLOW_ONLY;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_flow_eline_vp_configure
 * Purpose:
 *      Configure FLOW ELINE virtual port
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td3_flow_eline_vp_configure (int unit, int vfi_index, int active_vp, 
                  source_vp_entry_t *svp, int tpid_enable, bcm_flow_port_t  *flow_port)
{
    int rv = BCM_E_NONE;
    int network_group=0;

    /* Set SOURCE_VP */
    soc_SOURCE_VPm_field32_set(unit, svp, CLASS_IDf,
                                flow_port->if_class);

    /* Default Split Horizon Group Id
     * 0 - For Access Port;1 - For Network Port*/
    network_group = flow_port->network_group_id;
    rv = _bcm_validate_splithorizon_network_group(unit,
            flow_port->flags & BCM_FLOW_PORT_NETWORK, &network_group);
    BCM_IF_ERROR_RETURN(rv);
    if (flow_port->flags & BCM_FLOW_PORT_NETWORK) {
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_GROUPf,
                 network_group);
        } else {
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_PORTf, 1);
        }
    } else {
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_GROUPf,
                        network_group);
        } else {
            soc_SOURCE_VPm_field32_set(unit, svp, NETWORK_PORTf, 0);
        }  
    }

    if (flow_port->flags & BCM_FLOW_PORT_SERVICE_TAGGED) {
         soc_SOURCE_VPm_field32_set(unit, svp, SD_TAG_MODEf, 1);
         soc_SOURCE_VPm_field32_set(unit, svp, TPID_ENABLEf, tpid_enable);
    } else {
         soc_SOURCE_VPm_field32_set(unit, svp, SD_TAG_MODEf, 0);
    }

    soc_SOURCE_VPm_field32_set(unit, svp, DISABLE_VLAN_CHECKSf, 1);
    soc_SOURCE_VPm_field32_set(unit, svp, 
                           ENTRY_TYPEf, _BCM_VXLAN_SOURCE_VP_TYPE_VFI); /* VFI Type */
    soc_SOURCE_VPm_field32_set(unit, svp, VFIf, vfi_index);

    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, active_vp, svp);
    if ((vfi_index != _BCM_FLOW_VFI_INVALID) &&
        (flow_port->flags & BCM_FLOW_PORT_NETWORK)) {
        if ( BCM_E_NONE != soc_cancun_app_dest_entry_set (unit, SOURCE_VPm,
            active_vp, ENTRY_TYPEf, CANCUN_APP__SOURCE_VP__No_Control_Word,
            _BCM_VXLAN_SOURCE_VP_TYPE_VFI)) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
               (BSL_META_U(unit,"SOURCE_VP cancun app cfg error\n")));
        }
    }

    return rv;
}



/*
 * Function:
 *      _bcm_td3_flow_access_niv_pe_set
 * Purpose:
 *      Set VXLAN Access virtual port which is of NIV/PE type
 * Parameters:
 *      unit    - (IN) Device Number
 *      vp      -  (IN) Access Virtual Port
 *      vfi      -  (IN) Virtial forwarding instance
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td3_flow_access_niv_pe_set (int unit, int vp, int vfi)
{
    source_vp_entry_t svp;
    int  rv = BCM_E_PARAM; 

        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            return rv;
        }

        if (vfi == _BCM_FLOW_VFI_INVALID) {
            soc_SOURCE_VPm_field32_set(unit, &svp, 
                                       ENTRY_TYPEf, _BCM_VXLAN_SOURCE_VP_TYPE_INVALID);
        } else {
            /* Initialize VP parameters */
            soc_SOURCE_VPm_field32_set(unit, &svp, 
                                       ENTRY_TYPEf, _BCM_VXLAN_SOURCE_VP_TYPE_VFI);
        }     
        soc_SOURCE_VPm_field32_set(unit, &svp, VFIf, vfi);
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
        return rv;
}

/*
 * Function:
 *      _bcm_td3_flow_access_niv_pe_reset
 * Purpose:
 *      Reset FLOW Access virtual port which is of NIV/PE type
 * Parameters:
 *      unit    - (IN) Device Number
 *      vp      -  (IN) Access Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td3_flow_access_niv_pe_reset (int unit, int vp)
{
    source_vp_entry_t svp;

    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));

    /* Initialize VP parameters */
    soc_SOURCE_VPm_field32_set(unit, &svp,
        ENTRY_TYPEf, _BCM_VXLAN_SOURCE_VP_TYPE_VLAN);
    soc_SOURCE_VPm_field32_set(unit, &svp, VFIf, 0);
    BCM_IF_ERROR_RETURN(WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp));

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_td3_flow_default_port_add
 * Purpose:
 *      Add FLOW port to a VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      flow_port - (IN/OUT) flow_port information (OUT : flow_port_id)
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td3_flow_default_port_add(int unit, bcm_flow_port_t  *flow_port,
                               int *vp_number)
{
    source_vp_entry_t svp;
    int rv = BCM_E_PARAM, vp=0, num_vp=0;
    int network_group=0;
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;
    _bcm_vp_info_t vp_info;

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeFlow;
    if(flow_port->flags & BCM_FLOW_PORT_NETWORK) { 
        vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

   /* ======== Allocate/Get Virtual_Port =============== */
    if (flow_port->flags & BCM_FLOW_PORT_REPLACE) {
        vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port->flow_port_id);
        if (vp == -1) {
            return BCM_E_PARAM;
        }

        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
            return BCM_E_NOT_FOUND;
        }
        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            return rv;
        }

        /* Decrement old-port's VP count */
        rv = _bcm_td3_flow_port_cnt_update(unit, flow_port->flow_port_id, vp, FALSE);
        if (rv < 0) {
            return rv;
        }

    } else if (flow_port->flags & BCM_FLOW_PORT_WITH_ID ) {
       if (!BCM_GPORT_IS_FLOW_PORT(flow_port->flow_port_id)) {
            return (BCM_E_BADID);
        }

        vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port->flow_port_id);
        if (vp == -1) {
            return BCM_E_PARAM;
        }
        /* Vp index zero is reserved in function _bcm_virtual_init because of 
         * HW restriction. 
         */
        if (vp >= num_vp || vp < 1) {
            return BCM_E_BADID;
        } 
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) { 
            return BCM_E_EXISTS;
        }

        BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp, vp_info));
        sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    } else {
       rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, vp_info, &vp);
       if (rv < 0) {
           return rv;
       }
        sal_memset(&svp, 0, sizeof(source_vp_entry_t));
        BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp, vp_info));
    }

    /* ======== Configure SVP/DVP Properties ========== */
    soc_SOURCE_VPm_field32_set(unit, &svp, CLASS_IDf, 
                               flow_port->if_class);
    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
        network_group = flow_port->network_group_id;
        rv = _bcm_validate_splithorizon_network_group(unit,
                flow_port->flags & BCM_FLOW_PORT_NETWORK, &network_group);
        BCM_IF_ERROR_RETURN(rv);
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_GROUPf,
                network_group);
    } else {
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 1);
    }

    soc_SOURCE_VPm_field32_set(unit, &svp, 
                                      ENTRY_TYPEf, _BCM_VXLAN_SOURCE_VP_TYPE_VFI);

    /* Keep CML in the replace operation. It may be set by bcm_td2_vxlan_port_learn_set before */
    if (!(flow_port->flags & BCM_FLOW_PORT_REPLACE)) {
        rv = _bcm_vp_default_cml_mode_get (unit, 
                       &cml_default_enable, &cml_default_new, 
                       &cml_default_move);
        if (rv < 0) {
            return rv;
        }
            
        if (cml_default_enable) {
            /* Set the CML to default values */
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, cml_default_new);
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, cml_default_move);
        } else {
            /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, 0x8);
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, 0x8);
        }
    }
    if (soc_mem_field_valid(unit, SOURCE_VPm, DISABLE_VLAN_CHECKSf)) {
        if (!soc_feature(unit, soc_feature_vlan_vfi_membership)) {
            soc_SOURCE_VPm_field32_set(unit, &svp, DISABLE_VLAN_CHECKSf, 1);
        }
    }

    /* Configure VXLAN_DEFAULT_NETWORK_SVPr.SVP */
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    BCM_IF_ERROR_RETURN(rv);

    if ( BCM_E_NONE != soc_cancun_app_dest_entry_set (unit, SOURCE_VPm,
        vp, ENTRY_TYPEf, CANCUN_APP__SOURCE_VP__No_Control_Word,
         _BCM_VXLAN_SOURCE_VP_TYPE_VFI)) {
         LOG_ERROR(BSL_LS_BCM_FLOW,
           (BSL_META_U(unit,"SOURCE_VP cancun app cfg error\n")));
    }

    BCM_GPORT_FLOW_PORT_ID_SET(flow_port->flow_port_id, vp);

    /* Increment new-port's VP count */
    rv = _bcm_td3_flow_port_cnt_update(unit, flow_port->flow_port_id, vp, TRUE);
    BCM_IF_ERROR_RETURN(rv);
    *vp_number = vp;
    return rv;
}

/*
 * Function:
 *      _bcm_td3_flow_default_port_delete
 * Purpose:
 *      Delete FLOW Default port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      flow_port_id - (IN) vxlan port information
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td3_flow_default_port_delete(int unit, bcm_vpn_t vpn, int vp)
{
    source_vp_entry_t svp;
    int rv = BCM_E_UNAVAIL;
    int vfi_index= -1;
    bcm_gport_t flow_port_id;

    if ( vpn != BCM_FLOW_VPN_INVALID) {
         _BCM_FLOW_VPN_GET(vfi_index, _BCM_FLOW_VPN_TYPE_ELAN,  vpn);
         if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeFlow)) {
            return BCM_E_NOT_FOUND;
         }
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
        return BCM_E_NOT_FOUND;
    }

    /* Set FLOW port ID */
    BCM_GPORT_FLOW_PORT_ID_SET(flow_port_id, vp);

    /* Clear the SVP and DVP table entries */
    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    if (rv < 0) {
        return rv;
    }

    /* Decrement port's VP count */
    rv = _bcm_td3_flow_port_cnt_update(unit, flow_port_id, vp, FALSE);
    if (rv < 0) {
        return rv;
    }

    /* Free the VP */
    (void) _bcm_vp_free(unit, _bcmVpTypeFlow, 1, vp);

    return rv;

}

/*
 * Function:
 *      _bcm_td3_flow_eline_port_add
 * Purpose:
 *      Add FLOW ELINE port to a VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      flow_port - (IN/OUT) FLOW port information (OUT : flow_port_id)
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td3_flow_eline_port_add(int unit, bcm_vpn_t vpn, bcm_flow_port_t  *flow_port)
{
    int active_vp = 0; /* default VP */
    source_vp_entry_t svp1, svp2;
    uint8 vp_valid_flag = 0;
    int tpid_enable = 0, tpid_index=-1;
    int  rv = BCM_E_PARAM; 
    int num_vp=0;
    int vp1=-1, vp2=-1, vfi_index= -1;
    _bcm_vp_info_t vp_info;

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeFlow;

        if ( vpn != BCM_FLOW_VPN_INVALID) {
            _BCM_FLOW_VPN_GET(vfi_index, _BCM_FLOW_VPN_TYPE_ELINE,  vpn);
             if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeFlow)) {
                return BCM_E_NOT_FOUND;
             }
        } else {
             vfi_index = _BCM_FLOW_VFI_INVALID;
        }

        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        if ( vpn != BCM_FLOW_VPN_INVALID) {

             /* ---- Read in current table values for VP1 and VP2 ----- */
             (void) _bcm_td3_flow_eline_vp_map_get (unit, vfi_index,  &vp1,  &vp2);

            if (_bcm_vp_used_get(unit, vp1, _bcmVpTypeFlow)) {
                BCM_IF_ERROR_RETURN (
                   READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp1, &svp1));
                if (soc_SOURCE_VPm_field32_get(unit, &svp1, ENTRY_TYPEf) == 0x1) {
                    vp_valid_flag |= 0x1;  /* -- VP1 Valid ----- */
                }
            }

            if (_bcm_vp_used_get(unit, vp2, _bcmVpTypeFlow)) {
                BCM_IF_ERROR_RETURN (
                   READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp2, &svp2));
                if (soc_SOURCE_VPm_field32_get(unit, &svp2, ENTRY_TYPEf) == 0x1) {
                    vp_valid_flag |= 0x2;        /* -- VP2 Valid ----- */
                }
            }

            if (flow_port->flags & BCM_FLOW_PORT_REPLACE) {
                active_vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port->flow_port_id);
                if (active_vp == -1) {
                   return BCM_E_PARAM;
                }
                if (!_bcm_vp_used_get(unit, active_vp, _bcmVpTypeFlow)) {
                     return BCM_E_NOT_FOUND;
                }
                /* Decrement old-port's VP count */
                rv = _bcm_td3_flow_port_cnt_update(unit, flow_port->flow_port_id,
                                                     active_vp, FALSE);
                if (rv < 0) {
                    return rv;
                }
            }
        }

        switch (vp_valid_flag) {

              case 0x0: /* No VP is valid */
                             if (flow_port->flags & BCM_FLOW_PORT_REPLACE) {
                                  return BCM_E_NOT_FOUND;
                             }

                             if (flow_port->flags & BCM_FLOW_PORT_WITH_ID) {
                                vp1 = BCM_GPORT_FLOW_PORT_ID_GET(flow_port->flow_port_id);
                                if (vp1 == -1) {
                                     return BCM_E_PARAM;
                                }

                                if (_bcm_vp_used_get(unit, vp1, _bcmVpTypeFlow)) {
                                     return BCM_E_EXISTS;
                                }
                                if (vp1 >= num_vp) {
                                     return (BCM_E_BADID);
                                }
                                vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
                                BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp1, vp_info));

                             } else {

                                /* No entries are used, allocate a new VP index for VP1 */

                                vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
                                rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, vp_info, &vp1);
                                if (rv < 0) {
                                  return rv;
                                }
                                /* Allocate a new VP index for VP2 */
                                vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
                                 rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, vp_info, &vp2);
                                  if (rv < 0) {
                                     (void) _bcm_vp_free(unit, _bcmVpTypeFlow, 1, vp1);
                                      return rv;
                                  }
                             }

                             active_vp = vp1;
                             vp_valid_flag = 1;
                             sal_memset(&svp1, 0, sizeof(source_vp_entry_t));
                             sal_memset(&svp2, 0, sizeof(source_vp_entry_t));
                             (void) _bcm_td3_flow_eline_vp_map_set(unit, vfi_index, vp1, vp2);
                             break;


         case 0x1:    /* Only VP1 is valid */   
                             if (flow_port->flags & BCM_FLOW_PORT_REPLACE) {
                                  if (active_vp != vp1) {
                                       return BCM_E_NOT_FOUND;
                                  }
                             } else if (flow_port->flags & BCM_FLOW_PORT_WITH_ID) {
                                vp2 = BCM_GPORT_FLOW_PORT_ID_GET(flow_port->flow_port_id);
                                if (vp2 == -1) {
                                     return BCM_E_PARAM;
                                }

                                if (_bcm_vp_used_get(unit, vp2, _bcmVpTypeFlow)) {
                                     return BCM_E_EXISTS;
                                }
                                if (vp2 >= num_vp) {
                                     return (BCM_E_BADID);
                                }
                                vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
                                BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp2, vp_info));

                                active_vp = vp2;
                             } else {
                                  active_vp = vp2;
                                  vp_valid_flag = 3;
                                  sal_memset(&svp2, 0, sizeof(source_vp_entry_t));
                             }

                             (void) _bcm_td3_flow_eline_vp_map_set(unit, vfi_index, vp1, vp2);
                             break;



         case 0x2: /* Only VP2 is valid */
                             if (flow_port->flags & BCM_FLOW_PORT_REPLACE) {
                                  if (active_vp != vp2) {
                                       return BCM_E_NOT_FOUND;
                                  }
                             } else if (flow_port->flags & BCM_FLOW_PORT_WITH_ID) {
                                vp1 = BCM_GPORT_FLOW_PORT_ID_GET(flow_port->flow_port_id);
                                if (vp1 == -1) {
                                     return BCM_E_PARAM;
                                }

                                if (_bcm_vp_used_get(unit, vp1, _bcmVpTypeFlow)) {
                                     return BCM_E_EXISTS;
                                }
                                if (vp1 >= num_vp) {
                                     return (BCM_E_BADID);
                                }
                                vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
                                BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp1, vp_info));
                                active_vp = vp1;

                             } else {
                                  active_vp = vp1;
                                  vp_valid_flag = 3;
                                   sal_memset(&svp1, 0, sizeof(source_vp_entry_t));
                             }

                             (void) _bcm_td3_flow_eline_vp_map_set(unit, vfi_index, vp1, vp2);
                             break;


              
         case 0x3: /* VP1 and VP2 are valid */
                              if (!(flow_port->flags & BCM_FLOW_PORT_REPLACE)) {
                                   return BCM_E_FULL;
                              }
                             break;
       }

       if (active_vp == -1) {
           return BCM_E_CONFIG;
       }

        /* Set FLOW port ID */
        if (!(flow_port->flags & BCM_FLOW_PORT_REPLACE)) {
            BCM_GPORT_FLOW_PORT_ID_SET(flow_port->flow_port_id, active_vp);
        }

       /* ======== Configure Service-Tag Properties =========== */
       if (flow_port->flags & BCM_FLOW_PORT_SERVICE_TAGGED) {
            rv = _bcm_fb2_outer_tpid_lkup(unit, flow_port->egress_service_tpid,
                                           &tpid_index);
            if (rv < 0) {
                goto flow_cleanup;
            }
            tpid_enable = (1 << tpid_index);
       }

       /* ======== Configure SVP Property ========== */
       if (active_vp == vp1) {
           rv  = _bcm_td3_flow_eline_vp_configure (unit, vfi_index, active_vp, &svp1, 
                                                    tpid_enable, flow_port);
       } else if (active_vp == vp2) {
           rv  = _bcm_td3_flow_eline_vp_configure (unit, vfi_index, active_vp, &svp2, 
                                                    tpid_enable, flow_port);
       }
       if (rv < 0) {
            goto flow_cleanup;
       }

/* match API 
        rv = _bcm_td2_vxlan_match_add(unit, vxlan_port, active_vp, vpn);
        if (rv < 0) {
            goto vxlan_cleanup;
        }
*/

        /* Increment new-port's VP count */
        rv = _bcm_td3_flow_port_cnt_update(unit, flow_port->flow_port_id, active_vp, TRUE);
        if (rv < 0) {
            goto flow_cleanup;
        }

        /* Set FLOW port ID */
        if (!(flow_port->flags & BCM_FLOW_PORT_REPLACE)) {
            BCM_GPORT_FLOW_PORT_ID_SET(flow_port->flow_port_id, active_vp);
        }

flow_cleanup:
        if (rv < 0) {
            if (tpid_enable) {
                (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
            }
            /* Free the VP's */
            if (vp_valid_flag) {
                if (vp1 != -1) {
                    (void) _bcm_vp_free(unit, _bcmVpTypeFlow, 1, vp1);
                }
                if (vp2 != -1) {
                    (void) _bcm_vp_free(unit, _bcmVpTypeFlow, 1, vp2);
                }
            }
        }
        return rv;
}

/*
 * Function:
 *     _bcm_td3_flow_elan_port_add
 * Purpose:
 *      Add FLOW ELAN port to a VPN
 * Parameters:
 *   unit - (IN) Device Number
 *   vpn - (IN) VPN instance ID
 *   flow_port - (IN/OUT) flow port information (OUT : flow_port_id)
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_td3_flow_elan_port_add(int unit, bcm_vpn_t vpn, bcm_flow_port_t *flow_port)
{
    int vp, num_vp, vfi=0;
    source_vp_entry_t svp;
    source_vp_2_entry_t svp2;
    uint32 tpid_enable = 0; 
    int tpid_index=-1;
    int rv = BCM_E_PARAM;
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;
    _bcm_vp_info_t vp_info;
    int network_group=0;
    uint32 svp1[SOC_MAX_MEM_WORDS];

    if (vpn != BCM_FLOW_VPN_INVALID) {
        _BCM_FLOW_VPN_GET(vfi, _BCM_FLOW_VPN_TYPE_ELAN,  vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeFlow)) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        vfi = _BCM_FLOW_VFI_INVALID;
    }
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeFlow;
    if (flow_port->flags & BCM_FLOW_PORT_NETWORK) {
        vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
    }

   /* ======== Allocate/Get Virtual_Port =============== */
    if (flow_port->flags & BCM_FLOW_PORT_REPLACE) {
        if (BCM_GPORT_IS_NIV_PORT(flow_port->flow_port_id) ||
            BCM_GPORT_IS_EXTENDER_PORT(flow_port->flow_port_id)) {
            if (BCM_GPORT_IS_NIV_PORT(flow_port->flow_port_id)) {
               vp = BCM_GPORT_NIV_PORT_ID_GET((flow_port->flow_port_id));
            } else if (BCM_GPORT_IS_EXTENDER_PORT(flow_port->flow_port_id)) {
               vp = BCM_GPORT_EXTENDER_PORT_ID_GET((flow_port->flow_port_id));
            }
            rv = _bcm_td3_flow_access_niv_pe_set (unit, vp, vfi);
            if (BCM_SUCCESS(rv)) {
               /* Set FLOW port ID */
               BCM_GPORT_FLOW_PORT_ID_SET(flow_port->flow_port_id, vp);
               rv = _bcm_vp_used_set(unit, vp, vp_info);
            }
            return rv;
        } else if (BCM_GPORT_IS_TRUNK(flow_port->flow_port_id)) {
            bcm_trunk_member_t trunk_member;
            int                port_count = 0;
            bcm_trunk_t        tid = 0;
            int                tid_is_vp_lag = FALSE;
            tid = BCM_GPORT_TRUNK_GET(flow_port->flow_port_id);
            BCM_IF_ERROR_RETURN(
                _bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag));
            if (tid_is_vp_lag) {
                BCM_IF_ERROR_RETURN(
                    bcm_td2_vp_lag_get(unit, tid, NULL, 1,
                                       &trunk_member, &port_count));
                if (BCM_GPORT_IS_NIV_PORT(trunk_member.gport) ||
                    BCM_GPORT_IS_EXTENDER_PORT(trunk_member.gport)) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_trunk_tid_to_vp_lag_vp(unit, tid, &vp));
                    rv = _bcm_td3_flow_access_niv_pe_set(unit, vp, vfi);
                    if (BCM_SUCCESS(rv)) {
                       rv = _bcm_vp_used_set(unit, vp, vp_info);
                    }
                    return rv;
                }
            }
        }

        vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port->flow_port_id);
        if (vp == -1) {
            return BCM_E_PARAM;
        }

        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
            return BCM_E_NOT_FOUND;
        }

        BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));

        BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit, MEM_BLOCK_ANY, vp, &svp1));

        BCM_IF_ERROR_RETURN(READ_SOURCE_VP_2m(unit, MEM_BLOCK_ANY, vp, &svp2));

        /* Decrement old-port's VP count */
        BCM_IF_ERROR_RETURN(_bcm_td3_flow_port_cnt_update(
                unit, flow_port->flow_port_id, vp, FALSE));
    } else if (flow_port->flags & BCM_FLOW_PORT_WITH_ID ) {
       if (!BCM_GPORT_IS_FLOW_PORT(flow_port->flow_port_id)) {
            return (BCM_E_BADID);
        }

        vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port->flow_port_id);
        if (vp == -1) {
            return BCM_E_PARAM;
        }
        /* Vp index zero is reserved in function _bcm_virtual_init because of 
         * HW restriction. 
         */
        if (vp >= num_vp || vp < 1) {
            return BCM_E_BADID;
        }
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeAny)) {
            return BCM_E_EXISTS;
        }
        BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp, vp_info));

        sal_memset(&svp, 0, sizeof(source_vp_entry_t));

        sal_memset(svp1, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

        sal_memset(&svp2, 0, sizeof(source_vp_2_entry_t));
    } else {
        /* allocate a new VP index */
        BCM_IF_ERROR_RETURN(_bcm_vp_alloc
            (unit, 0, (num_vp - 1), 1, SOURCE_VPm, vp_info, &vp));
        BCM_GPORT_FLOW_PORT_ID_SET(flow_port->flow_port_id, vp);

        sal_memset(&svp, 0, sizeof(source_vp_entry_t));

        sal_memset(svp1, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

        sal_memset(&svp2, 0, sizeof(source_vp_2_entry_t));
    }

    /* ======== Configure Service-Tag Properties =========== */
    if (flow_port->flags & BCM_FLOW_PORT_SERVICE_TAGGED) {
        rv = _bcm_fb2_outer_tpid_lkup(unit,
            flow_port->egress_service_tpid, &tpid_index);
        _BCM_FLOW_CLEANUP(rv)

        tpid_enable = (1 << tpid_index);
        soc_SOURCE_VPm_field32_set(unit, &svp, SD_TAG_MODEf, 1);
        soc_SVP_ATTRS_1m_field_set(unit, &svp1, TPID_ENABLEf, &tpid_enable);
    } else {
        soc_SOURCE_VPm_field32_set(unit, &svp, SD_TAG_MODEf, 0);
    }

    /* ======== Configure SVP/DVP Properties ========== */
    soc_SOURCE_VPm_field32_set(unit, &svp, CLASS_IDf, flow_port->if_class);
    soc_SOURCE_VPm_field32_set(unit, &svp, ENTRY_TYPEf, _BCM_VXLAN_SOURCE_VP_TYPE_VFI);

    /* Default Split Horizon Group Id
     * 0 - For Access Port;1 - For Network Port*/
    network_group = flow_port->network_group_id;
    rv = _bcm_validate_splithorizon_network_group(unit,
            flow_port->flags & BCM_FLOW_PORT_NETWORK, &network_group);
    BCM_IF_ERROR_RETURN(rv);
    if (flow_port->flags & BCM_FLOW_PORT_NETWORK) {
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {  
            soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_GROUPf,
                        network_group);
        } else {
            soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 1);
        }
    } else {
           if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
            soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_GROUPf,
                    network_group);
           } else {
               soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 0);
           }
    }
    soc_SOURCE_VPm_field32_set(unit, &svp, VFIf, vfi);
    /* Keep CML in the replace operation. It may be set by bcm_td2_vxlan_port_learn_set before */
    if (!(flow_port->flags & BCM_FLOW_PORT_REPLACE)) {
        rv = _bcm_vp_default_cml_mode_get (unit, &cml_default_enable,
                                    &cml_default_new, &cml_default_move);
        _BCM_FLOW_CLEANUP(rv)

        if (cml_default_enable) {
            /* Set the CML to default values */
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, cml_default_new);
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, cml_default_move);
        } else {
            /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, 0x8);
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, 0x8);
        }
    }
    if (soc_mem_field_valid(unit, SOURCE_VPm, DISABLE_VLAN_CHECKSf)) {
        if (!soc_feature(unit, soc_feature_vlan_vfi_membership)) {
            soc_SOURCE_VPm_field32_set(unit, &svp, DISABLE_VLAN_CHECKSf, 1);
        }
    }

    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);

    _BCM_FLOW_CLEANUP(WRITE_SVP_ATTRS_1m(unit, MEM_BLOCK_ALL, vp, &svp1));

    _BCM_FLOW_CLEANUP(WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, vp, &svp2));

    if ((vfi != _BCM_FLOW_VFI_INVALID) &&
        (flow_port->flags & BCM_FLOW_PORT_NETWORK)) {
        if ( BCM_E_NONE != soc_cancun_app_dest_entry_set (unit, SOURCE_VPm,
            vp, ENTRY_TYPEf, CANCUN_APP__SOURCE_VP__No_Control_Word,
            _BCM_VXLAN_SOURCE_VP_TYPE_VFI)) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
               (BSL_META_U(unit,"SOURCE_VP cancun app cfg error\n")));
        }
    }

    _BCM_FLOW_CLEANUP(rv)

    /* Configure VP match criteria */
    /*rv = _bcm_td2_vxlan_match_add(unit, vxlan_port, vp, vpn);*/      /* for match API */
    _BCM_FLOW_CLEANUP(rv)

    /* Increment new-port's VP count */
    rv = _bcm_td3_flow_port_cnt_update(unit, flow_port->flow_port_id, vp, TRUE);
    _BCM_FLOW_CLEANUP(rv)

    return BCM_E_NONE;

cleanup:
    if (tpid_enable) {
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
    }
    if (!(flow_port->flags & BCM_FLOW_PORT_REPLACE)) {
        (void) _bcm_vp_free(unit, _bcmVpTypeFlow, 1, vp);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td3_flow_eline_port_delete
 * Purpose:
 *      Delete FLOW ELINE port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      flow_port_id - (IN) flow port information
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td3_flow_eline_port_delete(int unit, bcm_vpn_t vpn, int active_vp)
{
    source_vp_entry_t svp;
    source_vp_2_entry_t svp_2_entry;
    int network_port_flag=0;
    int vp1=0, vp2=0, vfi_index= -1;
    int rv = BCM_E_UNAVAIL;
    bcm_gport_t  flow_port_id;
    _bcm_vp_info_t vp_info;

    if ( vpn != BCM_FLOW_VPN_INVALID) {
         _BCM_FLOW_VPN_GET(vfi_index, _BCM_VXLAN_VPN_TYPE_ELINE,  vpn);
         if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeFlow)) {
            return BCM_E_NOT_FOUND;
         }
    } else {
         vfi_index = _BCM_FLOW_VFI_INVALID;
    }

    if (!_bcm_vp_used_get(unit, active_vp, _bcmVpTypeFlow)) {
        return BCM_E_NOT_FOUND;
    }

    /* Set FLOW port ID */
    BCM_GPORT_FLOW_PORT_ID_SET(flow_port_id, active_vp);

    /* ---- Read in current table values for VP1 and VP2 ----- */
    (void) _bcm_td3_flow_eline_vp_map_get (unit, vfi_index,  &vp1,  &vp2);

/*  flow_match API will handle this
    bcm_flow_port_t_init(&flow_port);
    flow_port.flow_port_id = flow_port_id;
    BCM_IF_ERROR_RETURN(bcmi_esw_flow_port_get(unit, vpn, &flow_port));
    rv = _bcm_td2_vxlan_match_delete(unit, active_vp, vxlan_port);
    if ( rv < 0 ) {
        if (rv != BCM_E_NOT_FOUND) {
             return rv;
        } else {
             rv = BCM_E_NONE;
        }
    }
*/

    /* If the other port is valid, point it to itself */
    if (active_vp == vp1) {
        rv = _bcm_td3_flow_eline_vp_map_clear (unit, vfi_index, active_vp, 0);
    } else if (active_vp == vp2) {
        rv = _bcm_td3_flow_eline_vp_map_clear (unit, vfi_index, 0, active_vp);
    }

    if (rv >= 0) {

        /* Check for Network-Port */
        BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, active_vp, &vp_info));
        if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
            network_port_flag = TRUE;
        }

        if (!network_port_flag) {
            sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
            BCM_IF_ERROR_RETURN
                (WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, active_vp, &svp_2_entry));
        }

        /* Invalidate the VP being deleted */
        sal_memset(&svp, 0, sizeof(source_vp_entry_t));
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, active_vp, &svp);
    }

    if (rv < 0) {
        return rv;
    }

    /* Decrement port's VP count */
    rv = _bcm_td3_flow_port_cnt_update(unit, flow_port_id, active_vp, FALSE);
    if (rv < 0) {
        return rv;
    }

    /* Free the VP */
    (void) _bcm_vp_free(unit, _bcmVpTypeVxlan, 1, active_vp);
    return rv;

}

/*
 * Function:
 *      _bcm_td3_flow_elan_port_delete
 * Purpose:
 *      Delete FLOW ELAN port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      flow_port_id - (IN) flow port information
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td3_flow_elan_port_delete(int unit, bcm_vpn_t vpn, int vp)
{
    source_vp_entry_t svp;
    source_vp_2_entry_t svp_2_entry;
    int network_port_flag=0;
    int vfi_index= -1;
    bcm_gport_t flow_port_id;
    bcm_flow_port_t  flow_port;
    _bcm_vp_info_t vp_info;

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
        return BCM_E_NOT_FOUND;
    }

    /* Check for Network-Port */
    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
        network_port_flag = TRUE;
    }

    /* Set FLOW port ID */
    BCM_GPORT_FLOW_PORT_ID_SET(flow_port_id, vp);
    bcm_flow_port_t_init(&flow_port);
    flow_port.flow_port_id = flow_port_id;
    BCM_IF_ERROR_RETURN(bcmi_esw_flow_port_get(unit, vpn, &flow_port));

/*  handled by flow_match API
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if ((!network_port_flag) && (vp_info.flags & _BCM_VP_INFO_SHARED_PORT)) {
        if (VXLAN_INFO(unit)->match_key[vp].match_count > 0) {
            BCM_IF_ERROR_RETURN(bcm_td2p_share_vp_delete(unit, vpn, vp));
        }
        if (vpn != BCM_FLOW_VPN_INVALID) {
            return BCM_E_NONE;
        }
    } else
#endif
    {
        rv = _bcm_td2_vxlan_match_delete(unit, vp, vxlan_port);
        if (rv < 0 && rv != BCM_E_NOT_FOUND) {
            return rv;
        }
    }
    (void)bcm_td2_vxlan_match_clear(unit, vp);
*/

    if ( vpn != BCM_FLOW_VPN_INVALID) {
        if (!network_port_flag) { /* Check VPN only for Access VP */
            _BCM_FLOW_VPN_GET(vfi_index, _BCM_FLOW_VPN_TYPE_ELAN,  vpn);
            if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeFlow)) {
                return BCM_E_NOT_FOUND;
            }
            if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv) ||
                _bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                (void) _bcm_vp_free(unit, _bcmVpTypeVxlan, 1, vp);
                return _bcm_td3_flow_access_niv_pe_reset (unit, vp);
            }
        }
    }

    /* Decrement port's VP count */
    BCM_IF_ERROR_RETURN(_bcm_td3_flow_port_cnt_update(unit, flow_port_id, vp, FALSE));

    /* Clear the SVP table entries */
    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    BCM_IF_ERROR_RETURN(WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp));

    if (!network_port_flag) {
        sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
        BCM_IF_ERROR_RETURN
            (WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, vp, &svp_2_entry));
    }

    /* Free the VP */
    BCM_IF_ERROR_RETURN(_bcm_vp_free(unit, _bcmVpTypeFlow, 1, vp));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_flow_port_get
 * Purpose:
 *      Get an FLOW port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      flow  - (IN/OUT) FLOW port information
 */
int
bcmi_esw_flow_port_get(int unit, bcm_vpn_t vpn, bcm_flow_port_t *flow_port)
{
    int vp=0;
    int rv = BCM_E_NONE;
    uint32 reg_val = 0;
    int dft_vxlan_vp = -1;
    int dft_l2gre_vp = -1;
    int dft_gpe_vp = -1;
    int dft_geneve_vp = -1;

    if (BCM_GPORT_IS_TRUNK(flow_port->flow_port_id)) {
        bcm_trunk_member_t trunk_member;
        int                port_count = 0;
        bcm_trunk_t        tid = 0;
        int                tid_is_vp_lag = FALSE;
        if (vpn != BCM_FLOW_VPN_INVALID) {
            BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_is_valid(unit, vpn));
        }
        tid = BCM_GPORT_TRUNK_GET(flow_port->flow_port_id);
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag));
        if (tid_is_vp_lag) {
            BCM_IF_ERROR_RETURN(
                bcm_td2_vp_lag_get(unit, tid, NULL, 1,
                                   &trunk_member, &port_count));
            if (BCM_GPORT_IS_NIV_PORT(trunk_member.gport) ||
                BCM_GPORT_IS_EXTENDER_PORT(trunk_member.gport)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_trunk_tid_to_vp_lag_vp(unit, tid, &vp));
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
                    return BCM_E_NOT_FOUND;
                }
                return BCM_E_NONE;
            }
        }
    }

    if (SOC_REG_IS_VALID(unit, VXLAN_DEFAULT_NETWORK_SVPr)) {
        BCM_IF_ERROR_RETURN(READ_VXLAN_DEFAULT_NETWORK_SVPr(unit, &reg_val));
        dft_vxlan_vp = soc_reg_field_get(unit,
                            VXLAN_DEFAULT_NETWORK_SVPr, reg_val, SVPf);
    }

    if (SOC_REG_IS_VALID(unit, L2GRE_DEFAULT_NETWORK_SVPr)) {
        BCM_IF_ERROR_RETURN(READ_L2GRE_DEFAULT_NETWORK_SVPr(unit, &reg_val));
        dft_l2gre_vp = soc_reg_field_get(unit,
                            L2GRE_DEFAULT_NETWORK_SVPr, reg_val, SVPf);
    }

    if (SOC_REG_IS_VALID(unit, GPE_DEFAULT_NETWORK_SVPr)) {
        BCM_IF_ERROR_RETURN(READ_GPE_DEFAULT_NETWORK_SVPr(unit, &reg_val));
        dft_gpe_vp = soc_reg_field_get(unit,
                            GPE_DEFAULT_NETWORK_SVPr, reg_val, SVPf);
    }

    if (SOC_REG_IS_VALID(unit, GENEVE_DEFAULT_NETWORK_SVPr)) {
        BCM_IF_ERROR_RETURN(READ_GENEVE_DEFAULT_NETWORK_SVPr(unit, &reg_val));
        dft_geneve_vp = soc_reg_field_get(unit,
                            GENEVE_DEFAULT_NETWORK_SVPr, reg_val, SVPf);
    }

    vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port->flow_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
        return BCM_E_NOT_FOUND;
    }

    if (vpn != BCM_FLOW_VPN_INVALID) {
        BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_is_valid(unit, vpn));
    }

    if (vp == dft_vxlan_vp || vp == dft_l2gre_vp || vp == dft_gpe_vp ||
        vp == dft_geneve_vp) {
        rv = _bcm_td3_flow_default_port_get(unit, vp, flow_port);
        if (vp == dft_vxlan_vp) {
            flow_port->flags |= BCM_FLOW_PORT_DEFAULT;
        }
        if (vp == dft_l2gre_vp) {
            flow_port->flags |= BCM_FLOW_PORT_DEFAULT_L2GRE;
        }
        if (vp == dft_gpe_vp) {
            flow_port->flags |= BCM_FLOW_PORT_DEFAULT_GPE;
        }
        if (vp == dft_geneve_vp) {
            flow_port->flags |= BCM_FLOW_PORT_DEFAULT_GENEVE;
        }
    } else {
        rv = _bcmi_esw_flow_port_get(unit, vpn, vp, flow_port);
    }

    return rv;
}

 /*
  * Function:
  *      _bcm_flow_share_vp_get
  * Purpose:
  *      Get shared VPs info per VPN
  * Parameters:
  *      unit       - (IN) Device Number
  *      vpn        - (IN) VPN that shared vp belong to
  *      vp_bitmap  - (OUT) shared vp bitmap
  * Returns:
  *      BCM_E_XXX
  */
STATIC int
_bcm_flow_share_vp_get(int unit, bcm_vpn_t vpn, SHR_BITDCL *vp_bitmap)
{
    vlan_xlate_entry_t *vent;
    soc_mem_t mem;
    int index_min = 0;
    int index_max = 0;
    uint8 *xlate_buf = NULL;
    int buf_size = 0;
    int i = 0;
    int rv = BCM_E_NONE;
    int vfi = -1;
    int vp = -1;

    if (vpn != BCM_FLOW_VPN_INVALID) {
         BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_is_valid(unit, vpn));
         _BCM_FLOW_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
    }

    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    } else {
        mem = VLAN_XLATEm;
    }
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    buf_size = SOC_MEM_TABLE_BYTES(unit, mem);
    xlate_buf = soc_cm_salloc(unit, buf_size, "VLAN_XLATE buffer");
    if (NULL == xlate_buf) {
        return BCM_E_MEMORY;
    }
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, 
                                xlate_buf);
    if (rv < 0) {
        soc_cm_sfree(unit, xlate_buf);
        return rv;
    }

    for (i = index_min; i <= index_max; i++) {
        vent = soc_mem_table_idx_to_pointer(unit, mem, void *, xlate_buf, i);
        if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
            if (soc_mem_field32_get(unit, mem, vent, BASE_VALID_0f) != 3) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem, vent, BASE_VALID_1f) != 7) {
                continue;
            }
        }

        if (soc_mem_field32_get(unit, mem, vent, XLATE__MPLS_ACTIONf) != 0x1) {
            continue;
        }

        if (vfi != -1 && soc_mem_field32_get(unit, mem, vent, XLATE__VFIf) != 
                  vfi) {
            continue;
        }

        vp = soc_mem_field32_get(unit, mem, vent, XLATE__SOURCE_VPf);
        SHR_BITSET(vp_bitmap, vp);
    }

    (void)soc_cm_sfree(unit, xlate_buf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_flow_port_get_all
 * Purpose:
 *      Get all FLOW ports from a VPN
 * Parameters:
 *      unit     - (IN) Device Number
 *      vpn      - (IN) VPN instance ID
 *      port_max   - (IN) Maximum number of interfaces in array
 *      port_array - (OUT) Array of FLOW ports
 *      port_count - (OUT) Number of interfaces returned in array
 */
int
bcmi_esw_flow_port_get_all(int unit, bcm_vpn_t vpn, int port_max,
                         bcm_flow_port_t *port_array, int *port_count)
{
    int vp, rv = BCM_E_NONE;
    int vfi_index=-1;
    int vp1 = 0, vp2 = 0;
    uint8 isEline = 0;
    SHR_BITDCL *share_vp_bitmap = NULL;

    if (vpn != BCM_FLOW_VPN_INVALID) {
        _BCM_FLOW_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
        BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_is_eline(unit, vpn, &isEline));
    } else {
        vfi_index = _BCM_FLOW_VFI_INVALID;
    }

    *port_count = 0;

    if (isEline == 0x1 ) {
        /* ---- Read in current table values for VP1 and VP2 ----- */
        (void) _bcm_td3_flow_eline_vp_map_get (unit, vfi_index,  &vp1,  &vp2);
        vp = vp1;
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
            if (0 == port_max) {
                (*port_count)++;

            } else if (*port_count < port_max) {
                rv = _bcmi_esw_flow_port_get(unit, vpn, vp, 
                                           &port_array[*port_count]);
                if (rv < 0) {
                    return rv;
                }
                (*port_count)++;
            }
        }

        vp = vp2;
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
            if (0 == port_max) {
                (*port_count)++;

            } else if (*port_count < port_max) {
                rv = _bcmi_esw_flow_port_get(unit, vpn, vp, 
                                           &port_array[*port_count]);
                if (rv < 0) {
                    return rv;
                }
                (*port_count)++;
            }
        }
    } else if (isEline == 0x0 ) {
        uint32 entry_type = 0;
        source_vp_entry_t svp;
        int num_vp = soc_mem_index_count(unit, SOURCE_VPm);

        share_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "sharr_vp_bitmap");
        if (share_vp_bitmap == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(share_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

        if (soc_feature(unit, soc_feature_vp_sharing)) {
            rv = _bcm_flow_share_vp_get(unit, vpn, share_vp_bitmap);
            _BCM_FLOW_CLEANUP(rv)
        }

        SHR_BIT_ITER(VIRTUAL_INFO(unit)->flow_vp_bitmap, num_vp, vp) {

            rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
            _BCM_FLOW_CLEANUP(rv)
            entry_type = soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf);

            if ((vfi_index == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf) &&
                    entry_type == _BCM_VXLAN_SOURCE_VP_TYPE_VFI) || 
                SHR_BITGET(share_vp_bitmap, vp)) {

                /* Check if number of ports is requested */
                if (0 == port_max) {
                    (*port_count)++;
                    continue;
                } else if (*port_count == port_max) {
                    break;
                }

               rv = _bcmi_esw_flow_port_get(unit,
                        vpn, vp, &port_array[*port_count]);
               _BCM_FLOW_CLEANUP(rv)

               (*port_count)++;
            }
        }
        sal_free(share_vp_bitmap);
    }

    return BCM_E_NONE;
cleanup:
    sal_free(share_vp_bitmap);
    return rv;
}

/*
 * Function:
 *      _bcm_td3_flow_default_port_get
 * Purpose:
 *      Get FLOW Default port information
 * Parameters:
 *      unit       - (IN) Device Number
 *      vp         - (IN) Source Virtual Port
 *      flow_port  - (IN/OUT) flow port information
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_flow_default_port_get(int unit, int vp, bcm_flow_port_t *flow_port)
{
    int rv = BCM_E_NONE;
    source_vp_entry_t svp;
    _bcm_vp_info_t vp_info;

    bcm_flow_port_t_init(flow_port);
    BCM_GPORT_FLOW_PORT_ID_SET(flow_port->flow_port_id, vp);

    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
    flow_port->if_class = soc_SOURCE_VPm_field32_get(unit, &svp, CLASS_IDf);

    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
        flow_port->flags |= BCM_FLOW_PORT_NETWORK;
    }

    return rv;
}

/*
 * Function:
 *      _bcmi_esw_flow_port_get
 * Purpose:
 *      Get FLOW port information from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      flow_port_id - (IN) vxlan port information
 * Returns:
 *      BCM_E_XXX
 */

int
_bcmi_esw_flow_port_get(int unit, bcm_vpn_t vpn, int vp, bcm_flow_port_t  *flow_port)
{
    int i, rv = BCM_E_NONE;
    uint32 tpid_enable = 0;
    source_vp_entry_t svp;
    int split_horizon_port_flag=0;
    _bcm_vp_info_t vp_info;

    /* Initialize the structure */
    bcm_flow_port_t_init(flow_port);
    BCM_GPORT_FLOW_PORT_ID_SET(flow_port->flow_port_id, vp);
    BCM_IF_ERROR_RETURN (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));

    /* Check for Network-Port */
    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
        split_horizon_port_flag = TRUE;
    }

    if ( vpn != BCM_FLOW_VPN_INVALID) {
         if (!split_horizon_port_flag) { /* Check VPN only for Access VP */
             if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv) ||
                  _bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                   return BCM_E_NONE;
             }
         }
    }

    if (split_horizon_port_flag) {
        flow_port->flags |= BCM_FLOW_PORT_NETWORK;
    }

    /* Get the match parameters */
    /* Done by match API
    rv = _bcm_td2_vxlan_match_get(unit, vxlan_port, vp);
    BCM_IF_ERROR_RETURN(rv);
    */ 

    /* Get the BCM_VXLAN_PORT_ENABLE_VLAN_CHECKS flag */
    /* Done by match API
    if (!soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        BCM_IF_ERROR_RETURN(
            _bcm_td2_vxlan_port_vlan_get(unit, vp, vxlan_port));
    }
    */ 

    /* Get Tunnel index */
/* to be done by flow encap set API
    rv = _bcm_td2_vxlan_egress_dvp_get(unit, vp, vxlan_port);
    BCM_IF_ERROR_RETURN(rv);
*/

    /* Fill in SVP parameters */
    flow_port->if_class = soc_SOURCE_VPm_field32_get(unit, &svp, CLASS_IDf);
    flow_port->network_group_id = soc_SOURCE_VPm_field32_get(unit, &svp, 
                                    NETWORK_GROUPf);

    if (soc_SOURCE_VPm_field32_get(unit, &svp, SD_TAG_MODEf)) {
        uint32 svp1[SOC_MAX_MEM_WORDS];

        BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit, MEM_BLOCK_ANY, vp, svp1));
        soc_SVP_ATTRS_1m_field_get(unit, &svp1, TPID_ENABLEf, &tpid_enable);

        if (tpid_enable) {
            flow_port->flags |= BCM_FLOW_PORT_SERVICE_TAGGED;
            for (i = 0; i < 4; i++) {
                if (tpid_enable & (1 << i)) {
                    _bcm_fb2_outer_tpid_entry_get(unit, &flow_port->egress_service_tpid, i);
                }
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_port_create
 * Purpose:
 *      Create Flex Flow port and add to a default VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      flow_port - (IN/OUT) flow_port information (OUT : flow_port_id)
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_esw_flow_port_create(int unit, bcm_vpn_t vpn, bcm_flow_port_t  *flow_port)
{
    int mode=0, rv = BCM_E_PARAM;
    uint8 isEline = 0;
    int vp = -1;
    int dft_vxlan_port = FALSE;
    int dft_l2gre_port = FALSE;
    int dft_gpe_port   = FALSE;
    int dft_geneve_port = FALSE;

    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_egress_mode_get(unit, &mode));
    if (!mode) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 egress mode must be set first\n")));
        return BCM_E_DISABLED;
    }

    if (flow_port->if_class > 0xfff) {
        return BCM_E_PARAM;
    }

/*  mtu programmed in EGR_DVP_ATTRIBUTEm, will be done by bcm_flow_port_set
    if (flow_port->mtu > 0x3fff) {
        return BCM_E_PARAM;
    }
*/

    if (vpn != BCM_FLOW_VPN_INVALID) {
        BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_is_eline(unit, vpn, &isEline));
    }

    if (flow_port->flags & PORT_DEFAULT_ALL) {
        rv =  _bcm_td3_flow_default_port_add(unit, flow_port, &vp);
        BCM_IF_ERROR_RETURN(rv);

        if (flow_port->flags & BCM_FLOW_PORT_DEFAULT) {
           if (!SOC_REG_IS_VALID(unit, VXLAN_DEFAULT_NETWORK_SVPr)) {
               rv = BCM_E_UNAVAIL;
               goto err_exit;
           }
           dft_vxlan_port = TRUE;

           rv = soc_reg_field32_modify(unit, VXLAN_DEFAULT_NETWORK_SVPr, 
                        REG_PORT_ANY, SVPf, vp);
           if (BCM_FAILURE(rv)) {
               goto err_exit;
           }

           if (SOC_REG_IS_VALID(unit, DEFAULT_SVP_ENABLEr)) {
               rv = soc_reg_field32_modify
                     (unit, DEFAULT_SVP_ENABLEr, REG_PORT_ANY,
                               VXLAN_DEFAULT_SVP_ENABLEf, 1);
               if (BCM_FAILURE(rv)) {
                   if (rv != BCM_E_NOT_FOUND) {
                       goto err_exit;
                   } else {
                       rv = BCM_E_NONE;
                   } 
               }
           }
       }
       if (flow_port->flags & BCM_FLOW_PORT_DEFAULT_L2GRE) {
           if (!SOC_REG_IS_VALID(unit, L2GRE_DEFAULT_NETWORK_SVPr)) {
               rv = BCM_E_UNAVAIL;
               goto err_exit;
           }
           rv = soc_reg_field32_modify(unit, L2GRE_DEFAULT_NETWORK_SVPr,
                        REG_PORT_ANY, SVPf, vp);
           if (BCM_FAILURE(rv)) {
               goto err_exit;
           }

           dft_l2gre_port = TRUE;
           if (SOC_REG_IS_VALID(unit, DEFAULT_SVP_ENABLEr)) {
               rv = soc_reg_field32_modify
                     (unit, DEFAULT_SVP_ENABLEr, REG_PORT_ANY,
                               L2GRE_DEFAULT_SVP_ENABLEf, 1);
               if (BCM_FAILURE(rv)) {
                   if (rv != BCM_E_NOT_FOUND) {
                       goto err_exit;
                   } else {
                       rv = BCM_E_NONE;
                   } 
               }
           }
       }

       if (flow_port->flags & BCM_FLOW_PORT_DEFAULT_GPE) {
           if (!SOC_REG_IS_VALID(unit, GPE_DEFAULT_NETWORK_SVPr)) {
               rv = BCM_E_UNAVAIL;
               goto err_exit;
           }
           rv = soc_reg_field32_modify(unit, GPE_DEFAULT_NETWORK_SVPr,
                        REG_PORT_ANY, SVPf, vp);
           if (BCM_FAILURE(rv)) {
               goto err_exit;
           }
           dft_gpe_port = TRUE;
       }

       if (flow_port->flags & BCM_FLOW_PORT_DEFAULT_GENEVE) {
           if (!SOC_REG_IS_VALID(unit, GENEVE_DEFAULT_NETWORK_SVPr)) {
               rv = BCM_E_UNAVAIL;
               goto err_exit;
           }
           rv = soc_reg_field32_modify(unit, GENEVE_DEFAULT_NETWORK_SVPr,
                    REG_PORT_ANY, SVPf, vp);
           if (BCM_FAILURE(rv)) {
               goto err_exit;
           }
           dft_geneve_port = TRUE;
       }
       return rv;
    }

    if (isEline) {
        rv = _bcm_td3_flow_eline_port_add(unit, vpn, flow_port);
    } else {
        rv = _bcm_td3_flow_elan_port_add(unit, vpn, flow_port);
    }

    return rv;

err_exit:
    if (vp >= 0) {
        if (dft_vxlan_port == TRUE) {
           BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
               VXLAN_DEFAULT_NETWORK_SVPr, REG_PORT_ANY, SVPf, 0));
        }
        if (dft_l2gre_port == TRUE) {
           BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
               L2GRE_DEFAULT_NETWORK_SVPr, REG_PORT_ANY, SVPf, 0));
        }
        if (dft_gpe_port == TRUE) {
           BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
               GPE_DEFAULT_NETWORK_SVPr, REG_PORT_ANY, SVPf, 0));
        }
        if (dft_geneve_port == TRUE) {
           BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
               GENEVE_DEFAULT_NETWORK_SVPr, REG_PORT_ANY, SVPf, 0));
        }
        BCM_IF_ERROR_RETURN(_bcm_td3_flow_default_port_delete(unit,
            BCM_FLOW_VPN_INVALID, vp));
    }
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_port_destroy
 * Purpose:
 *      Delete FLOW port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      flow_port_id - (IN) flow port information
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_esw_flow_port_destroy(int unit, bcm_vpn_t vpn, bcm_gport_t flow_port_id)
{
    int vp=0;
    int rv = BCM_E_UNAVAIL;
    uint8 isEline=0;
    uint32 reg_val=0;
    int default_vp_delete = FALSE;

    if (BCM_GPORT_IS_TRUNK(flow_port_id)) {
        bcm_trunk_member_t trunk_member;
        int                port_count = 0;
        bcm_trunk_t        tid = 0;
        int                tid_is_vp_lag = FALSE;
        if (vpn != BCM_FLOW_VPN_INVALID) {
            BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_is_valid(unit, vpn));
        }
        tid = BCM_GPORT_TRUNK_GET(flow_port_id);
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag));
        if (tid_is_vp_lag) {
            BCM_IF_ERROR_RETURN(
                bcm_td2_vp_lag_get(unit, tid, NULL, 1,
                                   &trunk_member, &port_count));
            if (BCM_GPORT_IS_NIV_PORT(trunk_member.gport) ||
                BCM_GPORT_IS_EXTENDER_PORT(trunk_member.gport)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_trunk_tid_to_vp_lag_vp(unit, tid, &vp));
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
                    return BCM_E_NOT_FOUND;
                }
                BCM_IF_ERROR_RETURN(_bcm_vp_free(unit, _bcmVpTypeFlow, 1, vp));
                return _bcm_td3_flow_access_niv_pe_reset(unit, vp);
            }
        }
    }

    /* Check for Valid Vpn */
    if (vpn != BCM_FLOW_VPN_INVALID) {
        BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_is_valid(unit, vpn));
    }

    vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
        return BCM_E_NOT_FOUND;
    }

    /* Check counters before delete port / --- fixme uncomment when flow stat counters are ready
    if (bcm_esw_vxlan_stat_id_get(unit, vxlan_port_id, BCM_VXLAN_VPN_INVALID,
                   bcmVxlanOutPackets, &stat_counter_id) == BCM_E_NONE) { 
        num_ctr++;
    } 
    if (bcm_esw_vxlan_stat_id_get(unit, vxlan_port_id, BCM_VXLAN_VPN_INVALID,
                   bcmVxlanInPackets, &stat_counter_id) == BCM_E_NONE) {
        num_ctr++;
    }

    if (num_ctr != 0) {
        BCM_IF_ERROR_RETURN(
           bcm_esw_vxlan_stat_detach(unit, vxlan_port_id, BCM_VXLAN_VPN_INVALID));  
    }
    */

    /* Check for VXLAN default port */
    if (SOC_REG_IS_VALID(unit, VXLAN_DEFAULT_NETWORK_SVPr)) {
        BCM_IF_ERROR_RETURN(READ_VXLAN_DEFAULT_NETWORK_SVPr(unit, &reg_val));
        if (vp == soc_reg_field_get(unit,
                    VXLAN_DEFAULT_NETWORK_SVPr, reg_val, SVPf)) {
            rv = soc_reg_field32_modify(unit, VXLAN_DEFAULT_NETWORK_SVPr,
                        REG_PORT_ANY, SVPf, 0);
            BCM_IF_ERROR_RETURN(rv);

            rv =  _bcm_td3_flow_default_port_delete(unit, vpn, vp);
            BCM_IF_ERROR_RETURN(rv);
            if (SOC_REG_IS_VALID(unit, DEFAULT_SVP_ENABLEr)) {
                rv = soc_reg_field32_modify
                      (unit, DEFAULT_SVP_ENABLEr, REG_PORT_ANY, VXLAN_DEFAULT_SVP_ENABLEf, 0);
                if (BCM_FAILURE(rv)) {
                    if (rv != BCM_E_NOT_FOUND) {
                        return rv;
                    } else {
                        rv = BCM_E_NONE;
                    } 
                }
            }
            default_vp_delete = TRUE;
        }
    }

    /* Check for L2GRE default port */
    if (SOC_REG_IS_VALID(unit, L2GRE_DEFAULT_NETWORK_SVPr)) {
        BCM_IF_ERROR_RETURN(READ_L2GRE_DEFAULT_NETWORK_SVPr(unit, &reg_val));
        if (vp == soc_reg_field_get(unit,
                    L2GRE_DEFAULT_NETWORK_SVPr, reg_val, SVPf)) {
            rv = soc_reg_field32_modify(unit, L2GRE_DEFAULT_NETWORK_SVPr,
                        REG_PORT_ANY, SVPf, 0);
            BCM_IF_ERROR_RETURN(rv);
            if (default_vp_delete == FALSE) {
                rv =  _bcm_td3_flow_default_port_delete(unit, vpn, vp);
                BCM_IF_ERROR_RETURN(rv);
            }
            if (SOC_REG_IS_VALID(unit, DEFAULT_SVP_ENABLEr)) {
                rv = soc_reg_field32_modify(unit, DEFAULT_SVP_ENABLEr,
                         REG_PORT_ANY, L2GRE_DEFAULT_SVP_ENABLEf, 0);
                if (BCM_FAILURE(rv)) {
                    if (rv != BCM_E_NOT_FOUND) {
                        return rv;
                    } else {
                        rv = BCM_E_NONE;
                    }
                } 
            }
            default_vp_delete = TRUE;
        }
    }

    /* Check for GPE default port */
    if (SOC_REG_IS_VALID(unit, GPE_DEFAULT_NETWORK_SVPr)) {
        BCM_IF_ERROR_RETURN(READ_GPE_DEFAULT_NETWORK_SVPr(unit, &reg_val));
        if (vp == soc_reg_field_get(unit,
                    GPE_DEFAULT_NETWORK_SVPr, reg_val, SVPf)) {
            rv = soc_reg_field32_modify(unit, GPE_DEFAULT_NETWORK_SVPr,
                        REG_PORT_ANY, SVPf, 0);
            BCM_IF_ERROR_RETURN(rv);
            if (default_vp_delete == FALSE) {
                rv =  _bcm_td3_flow_default_port_delete(unit, vpn, vp);
                BCM_IF_ERROR_RETURN(rv);
            }
            default_vp_delete = TRUE;
        }
    }

    /* Check for GENEVE default port */
    if (SOC_REG_IS_VALID(unit, GENEVE_DEFAULT_NETWORK_SVPr)) {
        BCM_IF_ERROR_RETURN(READ_GENEVE_DEFAULT_NETWORK_SVPr(unit, &reg_val));
        if (vp == soc_reg_field_get(unit,
                    GENEVE_DEFAULT_NETWORK_SVPr, reg_val, SVPf)) {
            rv = soc_reg_field32_modify(unit, GENEVE_DEFAULT_NETWORK_SVPr,
                        REG_PORT_ANY, SVPf, 0);
            BCM_IF_ERROR_RETURN(rv);
            if (default_vp_delete == FALSE) {
                rv =  _bcm_td3_flow_default_port_delete(unit, vpn, vp);
                BCM_IF_ERROR_RETURN(rv);
            }
            default_vp_delete = TRUE;
        }
    }

    if (default_vp_delete == TRUE) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(bcmi_esw_flow_vp_is_eline(unit, vp, &isEline));

    if (isEline == 0x1 ) {
       rv = _bcm_td3_flow_eline_port_delete(unit, vpn, vp);
    } else if (isEline == 0x0 ) {
       rv = _bcm_td3_flow_elan_port_delete(unit, vpn, vp);
    }
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_port_delete_all
 * Purpose:
 *      Delete all FLOW ports from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_esw_flow_port_delete_all(int unit, bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    int vfi_index=0;
    int vp1 = 0, vp2 = 0;
    uint8 isEline = 0;
    bcm_gport_t flow_port_id;
    uint32 vp=0;

    if (vpn != BCM_FLOW_VPN_INVALID) {
        _BCM_FLOW_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
        BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_is_eline(unit, vpn, &isEline));
    } else {
        vfi_index = _BCM_FLOW_VFI_INVALID;
    }

    if (isEline == 0x1 ) {
         /* ---- Read in current table values for VP1 and VP2 ----- */
         (void) _bcm_td3_flow_eline_vp_map_get (unit, vfi_index,  &vp1,  &vp2);
         if (vp1 != 0) {
              rv = _bcm_td3_flow_eline_port_delete(unit, vpn, vp1);
              BCM_IF_ERROR_RETURN(rv);
         }
         if (vp2 != 0) {
              rv = _bcm_td3_flow_eline_port_delete(unit, vpn, vp2);
              BCM_IF_ERROR_RETURN(rv);
         }
    } else if (isEline == 0x0 ) {
        uint32 num_vp = 0, entry_type = 0;
        source_vp_entry_t svp;
        uint32 reg_val=0;

        if (SOC_REG_IS_VALID(unit, VXLAN_DEFAULT_NETWORK_SVPr)) {
            BCM_IF_ERROR_RETURN(READ_VXLAN_DEFAULT_NETWORK_SVPr(unit, &reg_val));
            vp = soc_reg_field_get(unit, VXLAN_DEFAULT_NETWORK_SVPr, reg_val, SVPf);
            if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
                BCM_IF_ERROR_RETURN(_bcm_td3_flow_default_port_delete(unit, vpn, vp));
                if (SOC_REG_IS_VALID(unit, DEFAULT_SVP_ENABLEr)) {
                    rv = soc_reg_field32_modify
                          (unit, DEFAULT_SVP_ENABLEr, REG_PORT_ANY, VXLAN_DEFAULT_SVP_ENABLEf, 0);
                    if (BCM_FAILURE(rv)) {
                        if (rv != BCM_E_NOT_FOUND) {
                            return rv;
                        } else {
                            rv = BCM_E_NONE;
                        }
                    }
                }
            }
        }

        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        SHR_BIT_ITER(VIRTUAL_INFO(unit)->flow_vp_bitmap, num_vp, vp) {

            BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
            entry_type = soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf);
            if ((vfi_index == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf) &&
                    entry_type == _BCM_VXLAN_SOURCE_VP_TYPE_VFI)
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                    || SHR_BITGET(VIRTUAL_INFO(unit)->vp_shared_vp_bitmap, vp)
#endif
                    ) {
                if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVpLag)) {
                    bcm_trunk_t        tid = 0;
                    bcm_trunk_member_t trunk_member;
                    int                port_count = 0;
                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_trunk_vp_lag_vp_to_tid(unit, vp, &tid));
                    BCM_IF_ERROR_RETURN(
                        bcm_td2_vp_lag_get(unit, tid, NULL, 1,
                                           &trunk_member, &port_count));
                    if (BCM_GPORT_IS_NIV_PORT(trunk_member.gport) ||
                        BCM_GPORT_IS_EXTENDER_PORT(trunk_member.gport)) {
                        BCM_IF_ERROR_RETURN(
                            _bcm_vp_free(unit, _bcmVpTypeFlow, 1, vp));
                        BCM_IF_ERROR_RETURN(
                            _bcm_td3_flow_access_niv_pe_reset(unit, vp));
                        continue;
                    }
                }
                   /* Set FLOW port ID */
                BCM_GPORT_FLOW_PORT_ID_SET(flow_port_id, vp);
                BCM_IF_ERROR_RETURN(bcmi_esw_flow_port_destroy(unit, vpn, flow_port_id));
            }
        }
    }
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_trunk_member_delete
 * Purpose:
 *      Clear Trunk Port-membership from EGR_DGPP_TO_NHI
 * Parameters:
 *     unit  - (IN) Device Number
 *     tgid - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be deleted
 *     trunk_member_array - (IN) Trunk member gports to be deleted
*/
int
bcmi_esw_flow_trunk_member_delete(int unit, bcm_trunk_t trunk_id,
                int trunk_member_count, bcm_port_t *trunk_member_array)
{
    int idx = 0;
    int rv = BCM_E_NONE;
    int old_nh_index = 0;
    int nh_index = 0;
    int gport = 0;
    _bcm_port_info_t *port_info;
    int local_member = 0;
    int int_l3_flag;

    rv = _bcm_trunk_id_validate(unit, trunk_id);
    if (BCM_FAILURE(rv)) {
        return BCM_E_PORT;
    }

    /* Obtain nh_index for trunk_id */
    rv = _bcm_xgs3_trunk_nh_store_get(unit, trunk_id, &nh_index);
    if (nh_index == 0) {
        return rv;
    }

    int_l3_flag = BCM_XGS3_L3_ENT_FLAG(BCM_XGS3_L3_TBL_PTR(unit, next_hop),
                                   nh_index);

    if (int_l3_flag == _BCM_L3_FLOW_ONLY) {
        for (idx = 0; idx < trunk_member_count; idx++) {
            rv = bcm_esw_port_local_get(unit, trunk_member_array[idx], &local_member);

            /* Support trunk remote gports */
            gport = trunk_member_array[idx];
            BCM_IF_ERROR_RETURN(_bcm_trx_gport_to_nhi_get(unit,
                gport, &old_nh_index));

            if (0 != old_nh_index) {
                BCM_IF_ERROR_RETURN(_bcm_trx_gport_to_nhi_set(unit,
                    gport, 0));
            }
            if(BCM_SUCCESS(rv)) {
                _bcm_port_info_access(unit, local_member, &port_info);
                port_info->vp_count--;
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_flow_trunk_member_add
 * Purpose:
 *      Activate Trunk Port-membership from EGR_DGPP_TO_NHI
 * Parameters:
 *     unit  - (IN) Device Number
 *     tgid - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be added
 *     trunk_member_array - (IN) Trunk member gports to be added
*/
int
bcmi_esw_flow_trunk_member_add(int unit, bcm_trunk_t trunk_id,
                int trunk_member_count, bcm_port_t *trunk_member_array)
{
    int idx = 0;
    int rv = BCM_E_NONE;
    int nh_index = 0, old_nh_index = 0;
    int gport = 0;
    _bcm_port_info_t *port_info;
    int local_member = 0;
    int int_l3_flag;

    rv = _bcm_trunk_id_validate(unit, trunk_id);
    if (BCM_FAILURE(rv)) {
        return BCM_E_PORT;
    }  

    /* Obtain nh_index for trunk_id */
    rv = _bcm_xgs3_trunk_nh_store_get(unit, trunk_id, &nh_index);
    if (nh_index == 0) {
        return rv;
    }

    int_l3_flag = BCM_XGS3_L3_ENT_FLAG(BCM_XGS3_L3_TBL_PTR(unit, next_hop),
                                   nh_index);
    if (int_l3_flag == _BCM_L3_FLOW_ONLY) {
        for (idx = 0; idx < trunk_member_count; idx++) {
            rv = bcm_esw_port_local_get(unit, trunk_member_array[idx],
                &local_member);

            /* Support trunk remote gports */
            gport = trunk_member_array[idx];
            BCM_IF_ERROR_RETURN(_bcm_trx_gport_to_nhi_get(unit,
                gport, &old_nh_index));
            if (!old_nh_index) {
                BCM_IF_ERROR_RETURN(_bcm_trx_gport_to_nhi_set(unit,
                    gport, nh_index));
            }

            /* Only count the number of VPs on local physical ports */
            if(BCM_SUCCESS(rv)) {
                _bcm_port_info_access(unit, local_member, &port_info);
                port_info->vp_count++;
            }
        }
    }
    return BCM_E_NONE;
}

#endif

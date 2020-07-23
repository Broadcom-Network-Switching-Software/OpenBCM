/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    mpls.c
 * Purpose: TR3 MPLS enhancements
 */
#include <soc/defs.h>
#include <shared/bsl.h>
#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <bcm/qos.h>
#include <bcm/error.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw_dispatch.h> 
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw/ecn.h>

#define MPLS_INFO(_unit_)   (&_bcm_tr_mpls_bk_info[_unit_])
#define ECN_INFO(_unit_)   (&_bcm_xgs5_ecn_bk_info[_unit_])

int
_bcm_tr3_mpls_entry_convert_to_extd_entry_key(
            int                     unit,
            mpls_entry_entry_t      *mpls_entry_entry,
            mpls_entry_extd_entry_t *mpls_entry_extd_entry);

 /*
  * Function:
  * 	 _bcm_tr3_mpls_p2mp_loopback_enable
  * Purpose:
  * 	 Enable loopback for MPLS P2MP
  * Parameters:
  *   IN : unit
  * Returns:
  * 	 BCM_E_XXX
  */
 
STATIC int
_bcm_tr3_mpls_p2mp_loopback_enable(int unit)
{
    soc_field_t lport_fields[] = {
        PORT_TYPEf,
        MPLS_ENABLEf,
        V4L3_ENABLEf,
        V6L3_ENABLEf
    };
    uint32 lport_values[] = {
        0x2,  /* PORT_TYPEf */
        0x1,  /* MPLS_ENABLEf */
        0x1,  /* V4L3_ENABLEf */
        0x1   /* V6L3_ENABLEf */
   };
    
    /* Update LPORT Profile Table */
    return _bcm_lport_profile_fields32_modify(unit, LPORT_PROFILE_LPORT_TAB,
                                              COUNTOF(lport_fields),
                                              lport_fields, lport_values);
}

 /*
  * Function:
  * 	 _bcm_tr3_mpls_p2mp_loopback_disable
  * Purpose:
  * 	 Disable loopback for MPLS P2MP
  * Parameters:
  *   IN : unit
  * Returns:
  * 	 BCM_E_XXX
  */
 
STATIC int
_bcm_tr3_mpls_p2mp_loopback_disable(int unit)
{
    soc_field_t lport_fields[] = {
        PORT_TYPEf,
        MPLS_ENABLEf,
        V4L3_ENABLEf,
        V6L3_ENABLEf
    };
    uint32 lport_values[] = {
        0x0,  /* PORT_TYPEf */
        0x0,  /* MPLS_ENABLEf */
        0x0,  /* V4L3_ENABLEf */
        0x0   /* V6L3_ENABLEf */
   };
    
    /* Update LPORT Profile Table */
    return _bcm_lport_profile_fields32_modify(unit, LPORT_PROFILE_LPORT_TAB,
                                              COUNTOF(lport_fields),
                                              lport_fields, lport_values);
}

/*
 * Function:
 *      _bcm_tr3_mpls_entry_set_key
 * Purpose:
 *      Convert key part of application format to HW entry.
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
*      ment - MPLS entry
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_mpls_entry_set_key(int unit, bcm_mpls_tunnel_switch_t *info,
                           uint32 *hw_buf,
                           soc_mem_t hw_mem, uint8 key_type)
{
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int gport_id;
    int rv = BCM_E_NONE;

    if (info->port == BCM_GPORT_INVALID) {
        /* Global label, mod/port not part of lookup key */
        soc_mem_field32_set(unit, hw_mem, hw_buf, MPLS__MODULE_IDf, 0);
        soc_mem_field32_set(unit, hw_mem, hw_buf, MPLS__PORT_NUMf, 0);
        if (BCM_XGS3_L3_MPLS_LBL_VALID( info->label)) {
            soc_mem_field32_set(unit, hw_mem, hw_buf,  MPLS__MPLS_LABELf, info->label);
        } else {
            return BCM_E_PARAM;
        }

        if (hw_mem == MPLS_ENTRYm) {
            soc_mem_field32_set(unit, hw_mem, hw_buf, KEY_TYPEf, key_type);
            soc_mem_field32_set(unit, hw_mem, hw_buf, VALIDf, 1);
        } else if (hw_mem == MPLS_ENTRY_EXTDm) {
            soc_mem_field32_set(unit, hw_mem, hw_buf, KEY_TYPE_0f, key_type);
            soc_mem_field32_set(unit, hw_mem, hw_buf, KEY_TYPE_1f, key_type);
            soc_mem_field32_set(unit, hw_mem, hw_buf, VALID_0f, 1);
            soc_mem_field32_set(unit, hw_mem, hw_buf, VALID_1f, 1);
        }
        return BCM_E_NONE;
    }

    rv = _bcm_esw_gport_resolve(unit, info->port, &mod_out, 
                                &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

    if (BCM_GPORT_IS_TRUNK(info->port)) {
        soc_mem_field32_set(unit, hw_mem, hw_buf, MPLS__Tf, 1);
        soc_mem_field32_set(unit, hw_mem, hw_buf, MPLS__TGIDf, trunk_id);
    } else {
        soc_mem_field32_set(unit, hw_mem, hw_buf, MPLS__MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, hw_mem, hw_buf, MPLS__PORT_NUMf, port_out);
    }
    if (BCM_XGS3_L3_MPLS_LBL_VALID( info->label)) {
        soc_mem_field32_set(unit, hw_mem, hw_buf,  MPLS__MPLS_LABELf, info->label);
    } else {
        return BCM_E_PARAM;
    }
    if (hw_mem == MPLS_ENTRYm) {
        soc_mem_field32_set(unit, hw_mem, hw_buf, KEY_TYPEf, key_type);
        soc_mem_field32_set(unit, hw_mem, hw_buf, VALIDf, 1);
    } else if (hw_mem == MPLS_ENTRY_EXTDm) {
        soc_mem_field32_set(unit, hw_mem, hw_buf, KEY_TYPE_0f, key_type);
        soc_mem_field32_set(unit, hw_mem, hw_buf, KEY_TYPE_1f, key_type);
        soc_mem_field32_set(unit, hw_mem, hw_buf, VALID_0f, 1);
        soc_mem_field32_set(unit, hw_mem, hw_buf, VALID_1f, 1);
    }    
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_mpls_entry_get_key
 * Purpose:
 *      Convert key part of HW entry to application format. 
 * Parameters:
 *      unit - Device Number
 *      hw_buf - mpls entry
 *      hw_mem - MPLS Memory
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_mpls_entry_get_key(int unit, uint32 *hw_buf,
                           soc_mem_t hw_mem, bcm_mpls_tunnel_switch_t *info)
{
    bcm_port_t port_in, port_out;
    bcm_module_t mod_in, mod_out;
    bcm_trunk_t trunk_id;

    port_in = soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__PORT_NUMf);
    mod_in = soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__MODULE_IDf);
    if (soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__Tf)) {
        trunk_id = soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__TGIDf);
        BCM_GPORT_TRUNK_SET(info->port, trunk_id);
    } else if ((port_in == 0) && (mod_in == 0)) {
        /* Global label, mod/port not part of lookup key */
        info->port = BCM_GPORT_INVALID;
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    mod_in, port_in, &mod_out, &port_out));
        BCM_GPORT_MODPORT_SET(info->port, mod_out, port_out);
    } 
    info->label = soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__MPLS_LABELf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_mpls_bud_entry_find
 * Purpose:
 *      Search for Bud entry
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_mpls_bud_entry_find(int unit, int action, 
                      bcm_mpls_tunnel_switch_t *info, uint32 *bud_ment, 
                      soc_mem_t *bud_mem, int *bud_node_index, 
                      uint8 *key_type)
{
    int rv = BCM_E_NONE;

    if (action == 0x2) {
         *key_type = 0x10;
         *bud_mem = MPLS_ENTRYm;
         BCM_IF_ERROR_RETURN(
              _bcm_tr3_mpls_entry_set_key(unit, info, bud_ment, *bud_mem, *key_type));
    } else {
         *key_type = 0x12;
         *bud_mem = MPLS_ENTRYm;
         BCM_IF_ERROR_RETURN(
              _bcm_tr3_mpls_entry_set_key(unit, info, bud_ment, *bud_mem, *key_type));
    }

    /* See if entry already exists */
    rv = soc_mem_search(unit, *bud_mem, MEM_BLOCK_ANY, bud_node_index,
                bud_ment, bud_ment, 0);

    /* If not found, then search in EXTD memory */
    if (BCM_FAILURE(rv)) {
        if (action == 0x2) {
             *key_type = 0x11;
             *bud_mem = MPLS_ENTRY_EXTDm;
             BCM_IF_ERROR_RETURN(
                  _bcm_tr3_mpls_entry_set_key(unit, info, bud_ment, *bud_mem, *key_type));
        } else {
             *key_type = 0x13;
             *bud_mem = MPLS_ENTRY_EXTDm;
             BCM_IF_ERROR_RETURN(
                  _bcm_tr3_mpls_entry_set_key(unit, info, bud_ment, *bud_mem, *key_type));
        }
        /* See if entry already exists */
        rv = soc_mem_search(unit, *bud_mem, MEM_BLOCK_ANY, bud_node_index,
                          bud_ment, bud_ment, 0);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_mpls_entry_find
 * Purpose:
 *      Search for MPLS entry
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_mpls_entry_find(int unit,
                      bcm_mpls_tunnel_switch_t *info, uint32 *hw_buf, 
                      soc_mem_t *hw_mem, int *index)
{
    int rv = BCM_E_NONE;
    uint8  key_type=0;

         /* Type-1, First-Pass */
         sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
         *hw_mem = MPLS_ENTRYm;
         key_type = 0x10;
         rv = _bcm_tr3_mpls_entry_set_key(unit, info, hw_buf, 
                             *hw_mem, key_type);
         BCM_IF_ERROR_RETURN(rv);

         rv = soc_mem_search(unit, *hw_mem, MEM_BLOCK_ANY, index,
                        hw_buf, hw_buf, 0);

         if (BCM_FAILURE(rv)) {
              /* Type-1, Second-Pass */
             sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
             *hw_mem = MPLS_ENTRYm;
             key_type = 0x12;
             rv = _bcm_tr3_mpls_entry_set_key(unit, info, hw_buf, 
                             *hw_mem, key_type);
             BCM_IF_ERROR_RETURN(rv);

             rv = soc_mem_search(unit, *hw_mem, MEM_BLOCK_ANY, index,
                        hw_buf, hw_buf, 0);

             if (BCM_FAILURE(rv)) {
                  /* Type-2, First-Pass */
                  sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
                  *hw_mem = MPLS_ENTRY_EXTDm;
                  key_type = 0x11;
                  rv = _bcm_tr3_mpls_entry_set_key(unit, info, hw_buf, 
                                  *hw_mem, key_type);
                  BCM_IF_ERROR_RETURN(rv);

                  rv = soc_mem_search(unit, *hw_mem, MEM_BLOCK_ANY, index,
                                  hw_buf, hw_buf, 0);

                  if (BCM_FAILURE(rv)) {
                       /* Type-2, Second-)Pass */
                      sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
                      *hw_mem = MPLS_ENTRY_EXTDm;
                      key_type = 0x13;
                      rv = _bcm_tr3_mpls_entry_set_key(unit, info, hw_buf, 
                                  *hw_mem, key_type);
                      BCM_IF_ERROR_RETURN(rv);

                      rv = soc_mem_search(unit, *hw_mem, MEM_BLOCK_ANY, index,
                                  hw_buf, hw_buf, 0);
                  }
             }
         }
         return rv;
}

/*
 * Function:
 *      _bcm_tr3_mpls_entry_delete
 * Purpose:
 *      Delete entry from MPLS_ENTRY table
 * Parameters:
 *      unit - Device Number
 *      ment - MPLS Label entry to delete
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_mpls_entry_delete(int unit, uint32 *hw_buf,
                            soc_mem_t hw_mem,
                            bcm_mpls_tunnel_switch_t  *info, int index)
{   
    int action, ecmp_index = -1, nh_index = -1;
    bcm_if_t  egress_if=0;
    int  ref_count=0, forwarding_field_type=-1, ipmc_index=-1;
    int bud_node_trigger=-1, is_set=0;
    uint32 bud_ment[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t  bud_mem;
    uint8 key_type=0;
    int bud_node_index=-1;
    int rv = BCM_E_NONE;

    sal_memset (bud_ment, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    action = soc_mem_field32_get(unit, hw_mem, hw_buf,
                                                      MPLS__MPLS_ACTION_IF_BOSf);
    switch (action) {
        case 2: /* POP */
                    if (hw_mem == MPLS_ENTRYm) {
                        key_type = 0x12;
                        if (key_type == soc_mem_field32_get(unit, hw_mem, hw_buf,
                                                      KEY_TYPEf)) {
                            forwarding_field_type = _BCM_MPLS_FORWARD_MULTICAST_GROUP;
                        }
                    } else if (hw_mem == MPLS_ENTRY_EXTDm) {
                        key_type = 0x13;
                        if (key_type == soc_mem_field32_get(unit, hw_mem, hw_buf,
                                                      KEY_TYPE_0f)) {
                            forwarding_field_type = _BCM_MPLS_FORWARD_MULTICAST_GROUP;
                        }
                    }
                    break;
        case 3: /* SWAP_NHI */
        case 6: /* PHP_NHI */
                     forwarding_field_type = _BCM_MPLS_FORWARD_NEXT_HOP;
                     break;
        case 4: /* SWAP_ECMP */
        case 7: /* PHP_ECMP */
                     forwarding_field_type = _BCM_MPLS_FORWARD_ECMP_GROUP;
                     break;
        case 5: /* SWAP_L3MC_INDEX */
                     forwarding_field_type = _BCM_MPLS_FORWARD_MULTICAST_GROUP;
                     break;
        default:
                       break;
    }

    if (forwarding_field_type == _BCM_MPLS_FORWARD_MULTICAST_GROUP) {
        if (action == 0x2) {
             bud_node_trigger = 1;
        } else {
             ipmc_index = soc_mem_field32_get(unit, hw_mem, hw_buf,
                                                      MPLS__L3MC_INDEXf);
             bud_node_trigger = soc_mem_field32_get(unit, hw_mem, hw_buf,
                                                      MPLS__P2MP_LOCAL_RECEIVERS_PRESENTf);
             rv = bcm_xgs3_ipmc_id_is_set(unit, ipmc_index, &is_set);
             if (BCM_SUCCESS(rv)) {
                 if (is_set) {
                    /* First Delete IPMC_entry, then delete P2MP Label entry */
                   return BCM_E_BUSY;
                 }
             }
        }
        if ((bud_node_trigger) && (info->action != -1)) {
            /* Search for Bud_Node entry */
            rv = _bcm_tr3_mpls_bud_entry_find(unit, action, info, bud_ment, 
                                   &bud_mem, &bud_node_index, &key_type);
            /* If Not found in both Memory, then return error */
            if (BCM_FAILURE(rv)) {
                return (rv);
            }
            if ((info->action == BCM_MPLS_SWITCH_ACTION_SWAP)) {
                 /* Change key_type of POP Entry to key_type_1; Disable loopback of POP Entry */
                 if (bud_mem == MPLS_ENTRY_EXTDm) {
                     soc_mem_field32_set(unit, bud_mem, bud_ment, KEY_TYPE_0f, 0x11);
                 } else if (bud_mem == MPLS_ENTRYm) {
                     soc_mem_field32_set(unit, bud_mem, bud_ment, KEY_TYPEf, 0x10);
                 }
                 soc_mem_field32_set(unit, bud_mem, bud_ment, 
                             MPLS__P2MP_LOCAL_RECEIVERS_PRESENTf, 0x0);
                 (void) _bcm_tr3_mpls_p2mp_loopback_disable(unit);
                 /* Update POP Entry */
                 rv = soc_mem_write(unit, bud_mem,
                           MEM_BLOCK_ALL, bud_node_index, bud_ment);
                 if (BCM_SUCCESS(rv)) {
                      /* Delete SWAP Entry from HW */
                      rv = soc_mem_delete(unit, hw_mem, MEM_BLOCK_ALL, hw_buf);
                      if ( (rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE) ) {
                           return rv;
                      }
                 }
            } else if ((info->action == BCM_MPLS_SWITCH_ACTION_POP)) {
                 /* Disable Loopback of SWAP Entry */
                 soc_mem_field32_set(unit, bud_mem, bud_ment,
                        MPLS__P2MP_LOCAL_RECEIVERS_PRESENTf, 0x0);
                 (void) _bcm_tr3_mpls_p2mp_loopback_disable(unit);
                 /* Update SWAP Entry */
                 rv = soc_mem_write(unit, bud_mem,
                           MEM_BLOCK_ALL, bud_node_index, bud_ment);
                 if (BCM_SUCCESS(rv)) {
                      /* Delete POP Entry from HW */
                      rv = soc_mem_delete(unit, hw_mem, MEM_BLOCK_ALL, hw_buf);
                      return rv;
                  }
            }
        }else if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
              /* Delete SWAP Entry from HW */
              rv = soc_mem_delete(unit, hw_mem, MEM_BLOCK_ALL, hw_buf);
              if ( (rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE) ) {
                   return rv;
              }
        }
        return rv;
    } else if (forwarding_field_type == _BCM_MPLS_FORWARD_NEXT_HOP) {
        nh_index = soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__NEXT_HOP_INDEXf);
    } else if (forwarding_field_type == _BCM_MPLS_FORWARD_ECMP_GROUP) {
        ecmp_index = soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__ECMP_PTRf);
    }
        
    /* Delete the entry from HW */
    rv = soc_mem_delete(unit, hw_mem, MEM_BLOCK_ALL, hw_buf);
    if ( (rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE) ) {
         return rv;
    }

    if (forwarding_field_type != _BCM_MPLS_FORWARD_MULTICAST_GROUP) {
        if (action == 0x3) { /* SWAP_NHI */
            /* Check if tunnel_switch.egress_label mode is being used */
            rv = bcm_tr_mpls_get_vp_nh (unit, (bcm_if_t) nh_index, &egress_if);
            if (rv == BCM_E_NONE) {
                rv = bcm_tr_mpls_l3_nh_info_delete(unit, nh_index);
            } else {
                /* Decrement next-hop Reference count */
                rv = bcm_xgs3_get_ref_count_from_nhi(unit, 0, &ref_count, nh_index);
            }
        } else if (action == 0x6) { /* PHP_NHI */
            rv = bcm_xgs3_nh_del(unit, 0, nh_index);
        } else if (forwarding_field_type == _BCM_MPLS_FORWARD_ECMP_GROUP) {
            rv = bcm_xgs3_ecmp_group_del(unit, ecmp_index, 0);
        }
    }
    return rv;
}


/*
 * Function:
 *      _bcm_tr3_mpls_entry_get_data
 * Purpose:
 *      Convert data part of HW entry to application format. 
 * Parameters:
 *      unit - Device Number
 *       ment - mpls entry
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_mpls_entry_get_data(int unit, uint32 *hw_buf,
                            soc_mem_t hw_mem,
                            bcm_mpls_tunnel_switch_t *info)
{
    int action, nh_index, ecmp_group, vrf, ipmc_index;
    bcm_if_t egress_if=0;
    int rv=BCM_E_NONE;
    int mode = 0;
    int v4_v6_override;

    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_ingress_mode_get(unit, &mode));
  
    action = soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__MPLS_ACTION_IF_BOSf);
    switch(action) {
    case 0x2:
        info->action = BCM_MPLS_SWITCH_ACTION_POP;
        if (mode == 0) {
            vrf = soc_mem_field32_get(unit, hw_mem, hw_buf,  MPLS__L3_IIFf);
            vrf -= _BCM_TR_MPLS_L3_IIF_BASE;
            _BCM_MPLS_VPN_SET(info->vpn, _BCM_MPLS_VPN_TYPE_L3, vrf);
        } else {
            info->ingress_if = soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__L3_IIFf);
        }
        break;
    case 0x3:
        info->action = BCM_MPLS_SWITCH_ACTION_SWAP;
        nh_index = soc_mem_field32_get(unit, hw_mem, hw_buf,
                                               MPLS__NEXT_HOP_INDEXf);
        rv = bcm_tr_mpls_get_vp_nh (unit, nh_index, &egress_if);
        if (rv == BCM_E_NONE) {
            rv = bcm_tr_mpls_l3_nh_info_get(unit, info, nh_index);
            BCM_IF_ERROR_RETURN(rv);
        } else {
            info->egress_if = nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
            info->egress_label.label = 0;
        }
        break;
    case 0x4:
        info->action = BCM_MPLS_SWITCH_ACTION_SWAP;
        ecmp_group = soc_mem_field32_get(unit, hw_mem, hw_buf,
                                               MPLS__ECMP_PTRf);
        info->egress_if = ecmp_group + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        break;
    case 0x5:
         ipmc_index = soc_mem_field32_get(unit, hw_mem, hw_buf,
                                                      MPLS__L3MC_INDEXf);
         info->flags |= BCM_MPLS_SWITCH_P2MP;
         _BCM_MULTICAST_GROUP_SET(info->mc_group, 
                  _BCM_MULTICAST_TYPE_EGRESS_OBJECT, ipmc_index);
         info->egress_label.label = 0;
        break;
    case 0x6:
        info->action = BCM_MPLS_SWITCH_ACTION_PHP;
        nh_index = soc_mem_field32_get(unit, hw_mem, hw_buf,
                                               MPLS__NEXT_HOP_INDEXf);
        info->egress_if = nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
        break;
    case 0x7:
        info->action = BCM_MPLS_SWITCH_ACTION_PHP;
        ecmp_group = soc_mem_field32_get(unit, hw_mem, hw_buf,
                                               MPLS__ECMP_PTRf);
        info->egress_if = ecmp_group + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit); 
        break;
    default:
        return BCM_E_INTERNAL;
        break;
    }
    v4_v6_override = soc_property_get(unit, spn_MPLS_SWITCH_IPV4_IPV6_INDEPENDENT_CONTROL, 0);
    if (v4_v6_override) {
        if (soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__V4_ENABLEf)) {
            info->flags2 |= BCM_MPLS_SWITCH2_ENABLE_IPV4;
        }
        if (soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__V6_ENABLEf)) {
            info->flags2 |= BCM_MPLS_SWITCH2_ENABLE_IPV6;
        }
    }
    if (!soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__DECAP_USE_TTLf)) {
        info->flags |= BCM_MPLS_SWITCH_INNER_TTL;
    }
    if (soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__DECAP_USE_EXP_FOR_INNERf)) {
        info->flags |= BCM_MPLS_SWITCH_INNER_EXP;
    }
    if (soc_mem_field32_get(unit, hw_mem, hw_buf,
                                    MPLS__DECAP_USE_EXP_FOR_PRIf) == 0x1) {

        /* Use specified EXP-map to determine internal prio/color */
        info->flags |= BCM_MPLS_SWITCH_INT_PRI_MAP;
        info->exp_map = 
            soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__EXP_MAPPING_PTRf);
        info->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
    } else if (soc_mem_field32_get(unit, hw_mem, hw_buf,
                                           MPLS__DECAP_USE_EXP_FOR_PRIf) == 0x2) {

        /* Use the specified internal priority value */
        info->flags |= BCM_MPLS_SWITCH_INT_PRI_SET;
        info->int_pri =
            soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__NEW_PRIf);

        /* Use specified EXP-map to determine internal color */
        info->flags |= BCM_MPLS_SWITCH_COLOR_MAP;
        info->exp_map = 
            soc_mem_field32_get(unit, hw_mem, hw_buf, MPLS__EXP_MAPPING_PTRf);
        info->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_mpls_port_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction)  for specific vpn and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
bcm_error_t _bcm_tr3_mpls_port_stat_get_table_info(
            int                        unit,
            bcm_vpn_t                  vpn,
            bcm_gport_t                port,
            uint32                     *num_of_tables,
            bcm_stat_flex_table_info_t *table_info)
{
    int                   vp=0;
    ing_dvp_table_entry_t dvp={{0}};
    int                   nh_index=0;
    initial_prot_nhi_table_entry_t prot_entry;
    int vpless_failover_port = FALSE;
    int rv;
 
    if (_BCM_MPLS_GPORT_FAILOVER_VPLESS_GET(port)) {
        vpless_failover_port = TRUE;
        port = _BCM_MPLS_GPORT_FAILOVER_VPLESS_CLEAR(port);
    }

    (*num_of_tables)=0;
    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }
    if (soc_feature(unit, soc_feature_mpls)) {
        BCM_IF_ERROR_RETURN(bcm_tr_mpls_lock(unit));
        if (!_BCM_MPLS_VPN_IS_VPLS(vpn) &&
            !_BCM_MPLS_VPN_IS_VPWS(vpn)) {
            bcm_tr_mpls_unlock (unit);
            return BCM_E_PARAM;
        }
        vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
        if (vp == -1) {
            bcm_tr_mpls_unlock (unit);
            return BCM_E_PARAM;
        }
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
            bcm_tr_mpls_unlock (unit);
            return BCM_E_NOT_FOUND;
        }
        table_info[*num_of_tables].table= SOURCE_VPm;
        table_info[*num_of_tables].index=vp;
        table_info[*num_of_tables].direction=bcmStatFlexDirectionIngress;
        (*num_of_tables)++;
        if(READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp) == BCM_E_NONE) {
           nh_index=soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
           rv = BCM_E_NONE;
           if (vpless_failover_port == TRUE) {
               rv = READ_INITIAL_PROT_NHI_TABLEm(unit,MEM_BLOCK_ANY, nh_index, &prot_entry);
               if (BCM_SUCCESS(rv)) {
                   nh_index = soc_INITIAL_PROT_NHI_TABLEm_field32_get(unit,&prot_entry,
                              REPLACEMENT_DATAf);
               }
           }
           if (BCM_SUCCESS(rv)) {
               table_info[*num_of_tables].table= EGR_L3_NEXT_HOPm;
               table_info[*num_of_tables].index=nh_index;
               table_info[*num_of_tables].direction=bcmStatFlexDirectionEgress;
               (*num_of_tables)++;
           }
        }
        bcm_tr_mpls_unlock (unit);
        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_mpls_entry_convert_common_fields
 * Description:
 *      convert/copies commons fields in two mpls tables i.e
 *      MPLS_ENTRYm and MPLS_ENTRY_EXTDm
 *      (Generic routine)
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      from_table       - (IN) Source Table
 *      to_table         - (IN) Destination Table
 *      view_field       - (IN) Common view field(KEY_TYPE) deciding view
 *      view_name        - (IN) Common view name
 *      from_entry       - (IN) Source table entry
 *      to_entry         - (OUT) Destination table entry
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC 
bcm_error_t _bcm_esw_mpls_entry_convert_common_fields(
            int         unit,
            soc_mem_t   from_table,
            soc_mem_t   to_table,
            soc_field_t view_field,
            char        *view_name,
            void        *from_entry,
            void        *to_entry)
{
   soc_mem_info_t   *memp_from;
   uint32           key_type=0;
   int              f=0;
#if !defined(SOC_NO_NAMES)
   soc_mem_info_t   *memp_to;
   soc_field_info_t *fieldp_from;
   uint32           fval[SOC_MAX_MEM_FIELD_WORDS]={0};
   soc_field_info_t *fieldp_to;
#endif
   char             view_name_with_colon[64];

   /* sal_memset(to_entry, 0, sizeof(mpls_entry_extd_entry_t)); */
   memp_from = &SOC_MEM_INFO(unit, from_table);
   key_type= soc_mem_field32_get(unit, from_table,
                                 (uint32 *)from_entry , view_field);
   if (sal_strcmp(memp_from->views[key_type],view_name) != 0) {
       return BCM_E_PARAM;
   }
   sal_memset(to_entry, 0, soc_mem_entry_bytes(unit,to_table));
   sal_sprintf(view_name_with_colon,"%s:",view_name);
   for (f = memp_from->nFields - 1; f >= 0; f--) {
#if !defined(SOC_NO_NAMES)
        memp_to = &SOC_MEM_INFO(unit, to_table);
        fieldp_from = &memp_from->fields[f];
        /* LOG_CLI((BSL_META_U(unit,
                               ":%d:%s: ==>"),
                    f,soc_fieldnames[fieldp_from->field])); */
        if ((sal_strstr(soc_fieldnames[fieldp_from->field],
                    view_name_with_colon) != NULL) /* ||
            (sal_strstr(soc_fieldnames[fieldp_from->field], ":") == NULL) */) {
             if (soc_mem_field_valid(unit,
                                     to_table,fieldp_from->field)) {
                 SOC_FIND_FIELD(fieldp_from->field,
                                memp_to->fields,
                                memp_to->nFields,
                                fieldp_to);
                 if (fieldp_to->len < fieldp_from->len) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Warning:%d:%s: Mismatch in length %d:%d ==>"), 
                             f,soc_fieldnames[fieldp_from->field],
                             fieldp_from->len,fieldp_to->len)); 
                 } else if (fieldp_to->len > fieldp_from->len) {
                     sal_memset(fval,0,((fieldp_to->len-1) >> 5)+1);
                     LOG_CLI((BSL_META_U(unit,
                                         "Warning:%d:%s: More lengthy field %d:%d==>"), 
                              f,soc_fieldnames[fieldp_from->field],
                              fieldp_from->len,fieldp_to->len)); 
                 }
                 fval[0]=fval[1]=0;
                 soc_mem_field_get(unit, from_table,
                                   (uint32 *)from_entry ,
                                   fieldp_from->field,fval);
                 soc_mem_field_set(unit,to_table,
                                   (uint32 *)to_entry ,
                                   fieldp_from->field, fval);
                 /* LOG_CLI((BSL_META_U(unit,
                                        "Matched \n"))); */
             } else {
                 /* LOG_CLI((BSL_META_U(unit,
                                        "Not Matched \n"))); */
             }
        } else {
            /* LOG_CLI((BSL_META_U(unit,
                                   "Skipped \n"))); */
        }
#endif
   }
   return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_esw_mpls_entry_convert_to_extd_entry
 * Description:
 *      convert regular source mpls tables entry to extended destination mpls
 *      table entry(MPLS_ENTRYm and MPLS_ENTRY_EXTDm)
 *
 * Parameters:
 *      unit                  - (IN) unit number
 *      mpls_entry_entry      - (IN) Source table entry
 *      mpls_entry_extd_entry - (OUT) Destination table entry
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC 
bcm_error_t _bcm_esw_mpls_entry_convert_to_extd_entry(
            int                     unit,
            mpls_entry_entry_t      *mpls_entry_entry,
            mpls_entry_extd_entry_t *mpls_entry_extd_entry)
{
   uint32 key_type=0;
   uint32 fval[SOC_MAX_MEM_FIELD_WORDS]={0};
   uint32 new_key_type=0;

   BCM_IF_ERROR_RETURN(_bcm_esw_mpls_entry_convert_common_fields(
                       unit, 
                       MPLS_ENTRYm, MPLS_ENTRY_EXTDm,KEY_TYPEf,"MPLS",
                       (void *)mpls_entry_entry,
                       (void *)mpls_entry_extd_entry));

   /* Take care about special fields */
   soc_mem_field_get(unit, MPLS_ENTRYm,
                     (uint32 *)mpls_entry_entry , VALIDf,fval);
   soc_mem_field_set(unit, MPLS_ENTRY_EXTDm,
                     (uint32 *)mpls_entry_extd_entry, VALID_0f, fval);
   soc_mem_field_set(unit, MPLS_ENTRY_EXTDm,
                     (uint32 *)mpls_entry_extd_entry, VALID_1f, fval);
   fval[0] = 0;
   fval[1] = 0;
   soc_mem_field_get(unit,MPLS_ENTRYm,(uint32 *)mpls_entry_entry,MPLS__KEYf,fval);
   soc_mem_field_set(unit,MPLS_ENTRY_EXTDm,(uint32 *)mpls_entry_extd_entry,
                     MPLS__KEY_0f, fval); /* Default KEY_0f */

   soc_mem_field_get(unit, MPLS_ENTRYm, (uint32 *)mpls_entry_entry , 
                     KEY_TYPEf,&key_type);
   new_key_type=key_type+1;
   soc_mem_field_set(unit,MPLS_ENTRY_EXTDm,(uint32 *)mpls_entry_extd_entry,
                       KEY_TYPE_0f, &new_key_type);
   soc_mem_field_set(unit,MPLS_ENTRY_EXTDm,(uint32 *)mpls_entry_extd_entry,
                       KEY_TYPE_1f, &new_key_type);

   fval[0] = 0;
   fval[1] = 0;
   soc_mem_field_get(unit, MPLS_ENTRYm,(uint32 *)mpls_entry_entry, MPLS__DATAf,fval);
   soc_mem_field_set(unit, MPLS_ENTRY_EXTDm, 
                     (uint32 *)mpls_entry_extd_entry, MPLS__DATA_0f, fval);
   return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_esw_mpls_entry_convert_to_regular_entry
 * Description:
 *      convert extended source mpls tables entry to regular destination mpls
 *      table entry(MPLS_ENTRY_EXTDm and MPLS_ENTRYm)
 *
 * Parameters:
 *      unit                  - (IN) unit number
 *      mpls_entry_extd_entry - (IN) Source table entry
 *      mpls_entry_entry      - (OUT) Destination table entry
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC 
bcm_error_t _bcm_esw_mpls_entry_convert_to_regular_entry(
            int                     unit,
            mpls_entry_extd_entry_t *mpls_entry_extd_entry,
            mpls_entry_entry_t      *mpls_entry_entry)
{
   uint32 key_type=0;
   uint32 fval[SOC_MAX_MEM_FIELD_WORDS];
   uint32 new_key_type=0;

   BCM_IF_ERROR_RETURN(_bcm_esw_mpls_entry_convert_common_fields(
                       unit, 
                       MPLS_ENTRY_EXTDm, MPLS_ENTRYm,KEY_TYPE_0f,"MPLS",
                       (void *)mpls_entry_extd_entry,
                       (void *)mpls_entry_entry));
   /* Take care about special fields */

   fval[0] = 0;
   fval[1] = 0;
   soc_mem_field_get(unit, MPLS_ENTRY_EXTDm,
                     (uint32 *)mpls_entry_extd_entry , VALID_0f,fval);
   soc_mem_field_set(unit, MPLS_ENTRYm,(uint32 *)mpls_entry_entry,
                     VALIDf, fval);

   fval[0] = 0;
   fval[1] = 0;
   soc_mem_field_get(unit, MPLS_ENTRY_EXTDm,
                     (uint32 *)mpls_entry_extd_entry , MPLS__KEY_0f,fval);
   soc_mem_field_set(unit, MPLS_ENTRYm,(uint32 *)mpls_entry_entry,
                     MPLS__KEYf, fval);

   soc_mem_field_get(unit,MPLS_ENTRY_EXTDm, (uint32 *)mpls_entry_extd_entry,
                     KEY_TYPE_0f, &key_type);
   new_key_type = key_type - 1;
   soc_mem_field_set(unit, MPLS_ENTRYm, (uint32 *)mpls_entry_entry ,
                     KEY_TYPEf, &new_key_type);

   fval[0] = 0;
   fval[1] = 0;
   soc_mem_field_get(unit, MPLS_ENTRY_EXTDm, (uint32 *)mpls_entry_extd_entry, 
                     MPLS__DATA_0f,fval);
   /* fval[1] &= 0x3f; */
   soc_mem_field_set(unit, MPLS_ENTRYm, (uint32 *)mpls_entry_entry, MPLS__DATAf,fval);

   return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_tr3_mpls_label_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction)  for given mpls label and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label
 *      port             - (IN) MPLS Gport
 *      object           - (IN) STAT object
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

STATIC
bcm_error_t _bcm_tr3_mpls_label_stat_get_table_info(
            int                        unit,
            bcm_mpls_label_t           label,
            bcm_gport_t                port,
            bcm_stat_object_t          object,
            uint32                     *num_of_tables,
            bcm_stat_flex_table_info_t *table_info)
{
    bcm_error_t              rv=BCM_E_NOT_FOUND;
    bcm_mpls_tunnel_switch_t mpls_tunnel_switch={0};
    mpls_entry_entry_t      mpls_entry_entry={{0}};
    int                     index[2] = {0, 0};
    soc_mem_t               mem[2] = {MPLS_ENTRY_EXTDm, L3_TUNNELm};
    mpls_entry_extd_entry_t mpls_entry_extd_entry={{0}};
    int                     i, num_found = 0;

    (*num_of_tables)=0;
    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }
    if (!soc_feature(unit, soc_feature_mpls)) {
        return BCM_E_UNAVAIL;
    }
    if (!SOC_IS_TRIUMPH3(unit)) {
        return BCM_E_UNAVAIL;
    }
    sal_memset(&mpls_entry_entry, 0, sizeof(mpls_entry_entry_t));
    sal_memset(&mpls_entry_extd_entry, 0, sizeof(mpls_entry_extd_entry_t));

    mpls_tunnel_switch.port = port;
    if (BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
       mpls_tunnel_switch.label = label;
    } else {
        return BCM_E_PARAM;
    }

    if (object != bcmStatObjectIngMplsFrrLabel) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_entry_set_key(unit,
                    &mpls_tunnel_switch,(uint32 *) &mpls_entry_entry, MPLS_ENTRYm, 0x10));
        rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index[num_found],
                &mpls_entry_entry, &mpls_entry_entry, 0);
        if (BCM_SUCCESS(rv)) {
            BCM_IF_ERROR_RETURN(_bcm_esw_mpls_entry_convert_to_extd_entry(
                        unit,&mpls_entry_entry,&mpls_entry_extd_entry));
            BCM_IF_ERROR_RETURN(soc_mem_delete(
                        unit, MPLS_ENTRYm, MEM_BLOCK_ANY,
                        (void *)&mpls_entry_entry));
            BCM_IF_ERROR_RETURN(soc_mem_insert(
                        unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ALL,
                        (void *)&mpls_entry_extd_entry));
        } else {
            /* Possibly entry has already been moved to MPLS_ENTRY_EXTD table */
            BCM_IF_ERROR_RETURN(_bcm_esw_mpls_entry_convert_to_extd_entry(
                        unit,&mpls_entry_entry,&mpls_entry_extd_entry));
        }
        BCM_IF_ERROR_RETURN(soc_mem_search(unit, MPLS_ENTRY_EXTDm,
                    MEM_BLOCK_ALL,&index[num_found],
                    (void *)&mpls_entry_extd_entry,
                    (void *)&mpls_entry_extd_entry,0));
        mem[num_found] = MPLS_ENTRY_EXTDm;
        num_found++;
    }
    if (soc_feature(unit, soc_feature_mpls_frr_lookup)) {
        if ((!num_found && (object == bcmStatObjectIngMplsFrrLabel)) ||
                (object == bcmStatObjectMaxValue)) {
            rv =  _bcm_tr3_mpls_tunnel_switch_frr_get(unit,
                    &mpls_tunnel_switch, &index[num_found]);
            if (rv == BCM_E_NONE) {
                mem[num_found] = L3_TUNNELm;
                num_found++;
            }
        }
    }
    if (num_found && (rv == BCM_E_NOT_FOUND)) {
        rv = BCM_E_NONE;
    }

    for (i = 0; i < num_found; i++) {
        table_info[*num_of_tables].table=mem[i];
        table_info[*num_of_tables].index=index[i];
        table_info[*num_of_tables].direction=bcmStatFlexDirectionIngress;
        (*num_of_tables)++;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_egress_object_p2mp_set
 * Purpose:
 *      Set P2MP entry type within Egress Object entry
 * Parameters:
 *      unit - Device Number
 *      nh_index - Next Hop Index
 *      flag - Indicates SWAP or PHP
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_egress_object_p2mp_set(int unit, bcm_multicast_t  mc_group, int flag)
{
    int rv=BCM_E_UNAVAIL;
    egr_l3_next_hop_entry_t egr_nh;
    int num_encap_id=0;
    bcm_if_t *encap_id_array = NULL;
    int i;
    int nh_index;


    /* Get the number of encap IDs of the multicast group */
    rv = bcm_esw_multicast_egress_get(unit, mc_group, 0,
                                      NULL, NULL, &num_encap_id);
    if (rv < 0) {
        return rv;
    }

    /* Get all the encap IDs of the multicast group */

    encap_id_array = sal_alloc(sizeof(bcm_if_t) * num_encap_id,
            "encap_id_array");
    if (NULL == encap_id_array) {
        return BCM_E_MEMORY;
    }
    sal_memset(encap_id_array, 0, sizeof(bcm_if_t) * num_encap_id);

    rv = bcm_esw_multicast_egress_get(unit, mc_group, num_encap_id,
                                      NULL, encap_id_array, &num_encap_id);
    if (rv < 0) {
        sal_free(encap_id_array);
        return rv;
    }

    /* Convert encap IDs to next hop indices */

    for (i = 0; i < num_encap_id; i++) {
        if (BCM_IF_INVALID != (uint32)encap_id_array[i]) {
            nh_index = encap_id_array[i] - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &egr_nh));

            switch (flag) {
                 case BCM_MPLS_SWITCH_ACTION_SWAP:
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                            MPLS__LABEL_ACTIONf, 0x1); /* SWAP action */
                        break;

                 case BCM_MPLS_SWITCH_ACTION_PHP:
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                            MPLS__LABEL_ACTIONf, 0x2); /* PHP action */
                        break;

                 default:
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                            MPLS__LABEL_ACTIONf, 0x0); /* NOOP action */
                        break;
            }
            rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                            MEM_BLOCK_ALL, nh_index, &egr_nh);
            if (rv < 0) {
                sal_free(encap_id_array);
                return rv;
            }
        }
    }

    sal_free(encap_id_array);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_mpls_p2mp_bud_entry_set
 * Purpose:
 *     Create P2MP Bud Entry for POP Action
 * Parameters:
 *      unit - Device Number
 *      mpls_entry 
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_mpls_p2mp_bud_entry_set(int unit, bcm_mpls_tunnel_switch_t *info, 
                                                                      mpls_entry_entry_t  *ment_entry )
{
    mpls_entry_entry_t ment;
    uint8   key_type_2 = 0x12;
    int rv = BCM_E_NONE;
    int  vrf=-1, mode=-1;
    uint32 value;

    sal_memset(&ment, 0, sizeof(mpls_entry_entry_t));

     if ((info != NULL) && (ment_entry == NULL)) {
         if (_BCM_MPLS_VPN_IS_L3(info->vpn)) {
             _BCM_MPLS_VPN_GET(vrf, _BCM_MPLS_VPN_TYPE_L3, info->vpn);
             if (!bcm_tr_mpls_vrf_used_get(unit, vrf)) {
                 return BCM_E_PARAM;
             }
             /* Check L3 Ingress Interface mode. */ 
             mode = 0;
             rv = bcm_xgs3_l3_ingress_mode_get(unit, &mode);
             BCM_IF_ERROR_RETURN(rv);
         }
         BCM_IF_ERROR_RETURN(
               _bcm_tr3_mpls_entry_set_key(unit, info, (uint32 *) &ment, MPLS_ENTRYm, key_type_2));

         soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                        0x0); /* INVALID */
         soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_ACTION_IF_BOSf,
                        0x2); /* L3_IIF */
         if (!mode) {
              soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__L3_IIFf, 
                                    _BCM_TR_MPLS_L3_IIF_BASE + vrf);
         } else {
              soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__L3_IIFf, info->ingress_if);
         }

         soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__V4_ENABLEf, 1);
         soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__V6_ENABLEf, 1);
         if (info->flags & BCM_MPLS_SWITCH_INNER_TTL) {
             soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__DECAP_USE_TTLf, 0);
         } else {
             soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__DECAP_USE_TTLf, 1);
         }
         if (info->flags & BCM_MPLS_SWITCH_INNER_EXP) {
             soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__DECAP_USE_EXP_FOR_INNERf, 0);
         } else {
             soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__DECAP_USE_EXP_FOR_INNERf, 1);
         }
         if (info->flags & BCM_MPLS_SWITCH_INT_PRI_MAP) {
             soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__DECAP_USE_EXP_FOR_PRIf, 
                                    0x1); /* MAP_PRI_AND_CNG */
             /* Use specified EXP-map to determine internal prio/color */
             soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__EXP_MAPPING_PTRf, 
                                    info->exp_map & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK);
         } else if (info->flags & BCM_MPLS_SWITCH_INT_PRI_SET) {
             soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__DECAP_USE_EXP_FOR_PRIf, 
                                    0x2); /* NEW_PRI_MAP_CNG */
             /* Use the specified internal priority value */
             soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__NEW_PRIf, 
                                    info->int_pri);
             /* Use specified EXP-map to determine internal color */
             soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__EXP_MAPPING_PTRf, 
                                    info->exp_map & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK);
         }

         rv = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &ment);
    } else if ((info != NULL) && (ment_entry != NULL)) {
         BCM_IF_ERROR_RETURN(
               _bcm_tr3_mpls_entry_set_key(unit, info, (uint32 *) &ment, MPLS_ENTRYm, key_type_2));

          soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                        0x0); /* INVALID */
          soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_ACTION_IF_BOSf,
                        0x2); /* L3_IIF */
         soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__V4_ENABLEf, 1);
         soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__V6_ENABLEf, 1);

          value = soc_MPLS_ENTRYm_field32_get(unit, ment_entry, MPLS__L3_IIFf);
          soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__L3_IIFf, value);

          value = soc_MPLS_ENTRYm_field32_get(unit, ment_entry, MPLS__DECAP_USE_TTLf);
          soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__DECAP_USE_TTLf, value);

          value = soc_MPLS_ENTRYm_field32_get(unit, ment_entry, MPLS__DECAP_USE_EXP_FOR_INNERf);
          soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__DECAP_USE_EXP_FOR_INNERf, value);

          value = soc_MPLS_ENTRYm_field32_get(unit, ment_entry, MPLS__DECAP_USE_EXP_FOR_PRIf);
          soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__DECAP_USE_EXP_FOR_PRIf, value);

          value = soc_MPLS_ENTRYm_field32_get(unit, ment_entry, MPLS__EXP_MAPPING_PTRf);
          soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__EXP_MAPPING_PTRf, value);

          value = soc_MPLS_ENTRYm_field32_get(unit, ment_entry, MPLS__DECAP_USE_EXP_FOR_PRIf);
          soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__DECAP_USE_EXP_FOR_PRIf, value);

          value = soc_MPLS_ENTRYm_field32_get(unit, ment_entry, MPLS__NEW_PRIf);
          soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__NEW_PRIf, value);

          value = soc_MPLS_ENTRYm_field32_get(unit, ment_entry, MPLS__EXP_MAPPING_PTRf);
          soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__EXP_MAPPING_PTRf, value);

           rv = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &ment);

    } else if (info == NULL) {
        return BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_mpls_process_swap_label_action
 * Purpose:
 *      Process tunnel_switch.action = SWAP
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_mpls_process_swap_label_action(int unit, bcm_mpls_tunnel_switch_t *info,
                                                                                     int bud_node_trigger,
                                                                                     int *nh_index,
                                                                                     mpls_entry_entry_t *ment )
{
    int rv = BCM_E_NONE;
    uint32 mpath_flag=0;
    int mc_index = -1;

    if (info->flags & BCM_MPLS_SWITCH_P2MP) {
        if ( bud_node_trigger ) {
                /* Case of Leaf_LSR + Branch_LSR => BUD_LSR */

               /* Insert Second-pass POP entry */
               rv = _bcm_tr3_mpls_p2mp_bud_entry_set(unit, info, ment);
               BCM_IF_ERROR_RETURN(rv);
          
             /* Update First-pass SWAP entry, First-pass POP to First-pass SWAP entry */
            soc_MPLS_ENTRYm_field32_set(unit, ment, 
                             MPLS__P2MP_LOCAL_RECEIVERS_PRESENTf, 0x1);
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                0x0); /* INVALID */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_BOSf,
                                0x5); /* SWAP_L3MC_INDEX */
            soc_MPLS_ENTRYm_field32_set(unit, ment, KEY_TYPEf, 0x10);

            if (_BCM_MULTICAST_IS_SET(info->mc_group)) {
                mc_index = _BCM_MULTICAST_ID_GET(info->mc_group);
                soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__L3MC_INDEXf, 
                                mc_index);
            } else {
                return BCM_E_PARAM;
            }

            rv = _bcm_tr3_mpls_p2mp_loopback_enable(unit);
            if (rv < 0) {
                return rv;
            }
        } else {
            /* Case of P2MP BRANCH_LSR */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                0x0); /* INVALID */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_BOSf,
                                0x5); /* SWAP_L3MC_INDEX */
            if (_BCM_MULTICAST_IS_SET(info->mc_group)) {
                mc_index = _BCM_MULTICAST_ID_GET(info->mc_group);
                soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__L3MC_INDEXf, 
                                mc_index);
            } else {
                return BCM_E_PARAM;
            }
            soc_MPLS_ENTRYm_field32_set(unit, ment,  KEY_TYPEf, 0x10);
            soc_MPLS_ENTRYm_field32_set(unit, ment,  VALIDf, 0x1);
        }
        rv = _bcm_tr3_egress_object_p2mp_set(unit, info->mc_group, 
                             BCM_MPLS_SWITCH_ACTION_SWAP);
    } else {

        if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->egress_if)) {
            if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, info->egress_if)) {
                return BCM_E_PARAM;
            }
        }

        /* Case of LSR */
        if (BCM_XGS3_L3_MPLS_LBL_VALID(info->egress_label.label)) {         
            rv = bcm_tr_mpls_l3_nh_info_add(unit, info, nh_index);
            if (rv < 0) {
                return rv;
            }
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__NEXT_HOP_INDEXf, *nh_index);
            /* Swap to Next_Hop Type */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                0x3); /* SWAP_NHI */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_BOSf,
                                0x3); /* SWAP NHI */
        } else {
             rv = bcm_xgs3_get_nh_from_egress_object(unit, info->egress_if,
                                                    &mpath_flag, 1, nh_index);
             if (rv < 0) {
                 return rv;
             }
             if (mpath_flag == BCM_L3_MULTIPATH) {
                  soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__ECMP_PTRf, *nh_index);
                  /* Swap to ECMP Type */
                  soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                0x4); /* SWAP_ECMP */
                  soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_BOSf,
                                0x4); /* SWAP_ECMP */

             } else {
                  soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__NEXT_HOP_INDEXf, *nh_index);
                  /* Swap to Next_Hop Type */
                  soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                0x3); /* SWAP_NHI */
                  soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_BOSf,
                                0x3); /* SWAP NHI */
             }
        }
    }
    return rv;
}


/*
 * Function:
 *      _bcm_kt_mpls_process_pop_label_action
 * Purpose:
 *      Process tunnel_switch.action = SWAP
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_mpls_process_pop_label_action(int unit, bcm_mpls_tunnel_switch_t *info,
                                                                                     int bud_node_trigger,
                                                                                     int old_action,
                                                                                     mpls_entry_entry_t *ment )
{
    int rv = BCM_E_NONE;
    int  vrf=-1, mode=-1;
    int mc_index = -1;
    uint8  key_type_1 = 0x10;

    if (_BCM_MPLS_VPN_IS_L3(info->vpn)) {
            _BCM_MPLS_VPN_GET(vrf, _BCM_MPLS_VPN_TYPE_L3, info->vpn);

            if (!bcm_tr_mpls_vrf_used_get(unit, vrf)) {
                return BCM_E_PARAM;
            }

            /* Check L3 Ingress Interface mode. */ 
            mode = 0;
            rv = bcm_xgs3_l3_ingress_mode_get(unit, &mode);
            BCM_IF_ERROR_RETURN(rv);
    }

    if (info->flags & BCM_MPLS_SWITCH_P2MP) {
        if (bud_node_trigger == 1) {
            /* Case of Branch_LSR + Leaf_LSR => BUD_LSR */

            /* Insert Second-pass POP entry */
            rv = _bcm_tr3_mpls_p2mp_bud_entry_set(unit, info, NULL);
            BCM_IF_ERROR_RETURN (rv);
           
             /* Update First-pass SWAP to SWAP-BUD entry */
            soc_MPLS_ENTRYm_field32_set(unit, ment, 
                             MPLS__P2MP_LOCAL_RECEIVERS_PRESENTf, 0x1);
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                0x0); /* INVALID */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_BOSf,
                                0x5); /* SWAP_L3MC_INDEX */
            soc_MPLS_ENTRYm_field32_set(unit, ment, KEY_TYPEf, key_type_1);

            if (_BCM_MULTICAST_IS_SET(info->mc_group)) {
                mc_index = _BCM_MULTICAST_ID_GET(info->mc_group);
                soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__L3MC_INDEXf, 
                                mc_index);
            } else {
                return BCM_E_PARAM;
            }

            rv = _bcm_tr3_mpls_p2mp_loopback_enable(unit);
        } else {
            /* Case of LEAF_LSR */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                0x0); /* INVALID */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_BOSf,
                                0x2); /* L3_IIF */
            if (!mode) {
                 soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__L3_IIFf, 
                                            _BCM_TR_MPLS_L3_IIF_BASE + vrf);
            } else {
                 soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__L3_IIFf, info->ingress_if);
            }
        }
    } else {
        /* Case of LSR */
        soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                0x1); /* POP */
        soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_BOSf,
                                0x2); /* L3_IIF */
        if (!mode) {
            _bcm_l3_ingress_intf_t iif;
            sal_memset(&iif, 0, sizeof(_bcm_l3_ingress_intf_t));
            iif.intf_id =  _BCM_TR_MPLS_L3_IIF_BASE + vrf;
            rv = _bcm_tr_l3_ingress_interface_get(unit, NULL, &iif);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            iif.vrf = vrf;
            rv = _bcm_tr_l3_ingress_interface_set(unit, &iif, NULL, NULL);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__L3_IIFf,
                    _BCM_TR_MPLS_L3_IIF_BASE + vrf);
        } else {
             soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__L3_IIFf, info->ingress_if);
        }
    }
    return rv;
}


/*
 * Function:
 *      _bcm_kt_mpls_process_php_label_action
 * Purpose:
 *      Process tunnel_switch.action = PHP
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_mpls_process_php_label_action(int unit, bcm_mpls_tunnel_switch_t *info,
                                                                                     int *nh_index,
                                                                                     mpls_entry_entry_t *ment )
{
    int rv = BCM_E_NONE;
    uint32 mpath_flag=0;

    /* Case of LSR */
    if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->egress_if)) {
        if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, info->egress_if)) {
            return BCM_E_PARAM;
        }
    }
    /*
     * Get egress next-hop index from egress object and
     * increment egress object reference count.
     */

    BCM_IF_ERROR_RETURN(bcm_xgs3_get_nh_from_egress_object(unit, info->egress_if,
                                                           &mpath_flag, 1, nh_index));

    /* Fix: Entry_Type = 1, for PHP Packets with more than 1 Label */
    /* Read the egress next_hop entry pointed by Egress-Object */   
    rv = bcm_tr_mpls_egress_entry_modify(unit, *nh_index, mpath_flag, 1);
    if (rv < 0 ) {
       return rv;
    }
    if (mpath_flag == BCM_L3_MULTIPATH) {
       soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__ECMP_PTRf, *nh_index);
       /* Swap to ECMP Type */
       soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                0x7); /* PHP_ECMP */
       soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_BOSf,
                                0x7); /* PHP_ECMP */ 
    } else {
        soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__NEXT_HOP_INDEXf, *nh_index);
        /* Swap to Next_Hop Type */
        soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                0x6); /* PHP_NHI */
        soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__MPLS_ACTION_IF_BOSf,
                                0x6); /* PHP_NHI */ 
    }
    return rv;
}

/*
 * Function:
 *		bcm_tr3_mpls_entry_internal_qos_set
 * Purpose:
 *		Set MPLS Entry QoS parameters
 * Parameters:
 *  IN :  Unit
 *  IN :  mpls_port
 *  IN :  mpls_entry
 * Returns:
 *		BCM_E_XXX
 */

void
bcm_tr3_mpls_entry_internal_qos_set(int unit, bcm_mpls_port_t *mpls_port, 
                                                                          bcm_mpls_tunnel_switch_t  *info, mpls_entry_entry_t *ment)
{
    if (mpls_port != NULL ) {
        if (mpls_port->flags & BCM_MPLS_PORT_INT_PRI_MAP) {
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__DECAP_USE_EXP_FOR_PRIf,
                                        0x1); /* MAP_PRI_AND_CNG */
            /* Use specified EXP-map to determine internal prio/color */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__EXP_MAPPING_PTRf,
                mpls_port->exp_map & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK);
        } else if ((mpls_port->flags & BCM_MPLS_PORT_INT_PRI_SET) &&
                   (mpls_port->flags & BCM_MPLS_PORT_COLOR_MAP)) {
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__DECAP_USE_EXP_FOR_PRIf,
                                        0x2); /* NEW_PRI_MAP_CNG */
            /* Use the specified internal priority value */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__NEW_PRIf,
                                        mpls_port->int_pri);
            /* Use specified EXP-map to determine internal color */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__EXP_MAPPING_PTRf,
                mpls_port->exp_map & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK);
        } else if (mpls_port->flags & BCM_MPLS_PORT_INT_PRI_SET) {
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__DECAP_USE_EXP_FOR_PRIf, 0x2);
            if (soc_mem_field_valid(unit, MPLS_ENTRYm,
                                    MPLS__TRUST_OUTER_DOT1P_PTRf)) {
                soc_MPLS_ENTRYm_field32_set(unit, ment,
                                            MPLS__TRUST_OUTER_DOT1P_PTRf,
                                            _BCM_TR_MPLS_PRI_CNG_MAP_NONE);
            }
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__NEW_PRIf,
                                        mpls_port->int_pri);
        } else {
            /* Trust the encapsulated packet's 802.1p pri/cfi, 
             * and use the incoming ports mapping.
             */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__DECAP_USE_EXP_FOR_PRIf,
                                        0x0); /* NONE */
            if (soc_mem_field_valid(unit, MPLS_ENTRYm,
                                    MPLS__TRUST_OUTER_DOT1P_PTRf)) {
                soc_MPLS_ENTRYm_field32_set(unit, ment,
                                            MPLS__TRUST_OUTER_DOT1P_PTRf,
                                            _BCM_TR_MPLS_PRI_CNG_MAP_IDENTITY);
            }
        }
    }else if ( info != NULL ) {
        if (info->flags & BCM_MPLS_SWITCH_INT_PRI_MAP) {
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__DECAP_USE_EXP_FOR_PRIf,
                                        0x1); /* MAP_PRI_AND_CNG */
            /* Use specified EXP-map to determine internal prio/color */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__EXP_MAPPING_PTRf,
                info->exp_map & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK);
        } else if ((info->flags & BCM_MPLS_SWITCH_INT_PRI_SET) &&
                   (info->flags & BCM_MPLS_SWITCH_COLOR_MAP)) {
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__DECAP_USE_EXP_FOR_PRIf,
                                        0x2); /* NEW_PRI_MAP_CNG */
            /* Use the specified internal priority value */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__NEW_PRIf,
                                        info->int_pri);
            /* Use specified EXP-map to determine internal color */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__EXP_MAPPING_PTRf,
                info->exp_map & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK);
        } else if (info->flags & BCM_MPLS_SWITCH_INT_PRI_SET) {
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__DECAP_USE_EXP_FOR_PRIf, 0x2);
            if (soc_mem_field_valid(unit, MPLS_ENTRYm,
                                    MPLS__TRUST_OUTER_DOT1P_PTRf)) {
                soc_MPLS_ENTRYm_field32_set(unit, ment,
                                            MPLS__TRUST_OUTER_DOT1P_PTRf,
                                            _BCM_TR_MPLS_PRI_CNG_MAP_NONE);
            }
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__NEW_PRIf,
                                        info->int_pri);
        } else {
            /* Trust the encapsulated packet's 802.1p pri/cfi, 
             * and use the incoming ports mapping.
             */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS__DECAP_USE_EXP_FOR_PRIf,
                                        0x0); /* NONE */
            if (soc_mem_field_valid(unit, MPLS_ENTRYm,
                                    MPLS__TRUST_OUTER_DOT1P_PTRf)) {
                soc_MPLS_ENTRYm_field32_set(unit, ment,
                                            MPLS__TRUST_OUTER_DOT1P_PTRf,
                                            _BCM_TR_MPLS_PRI_CNG_MAP_IDENTITY);
            }
        }
    }
}

/*
 * Function:
 *      _bcm_tr3_mpls_tunnel_switch_frr_entry_key_get
 * Purpose:
 *      Populate key part from tunnel terminator entry
 * Parameters:
 *      unit      - Device Number
 *      tnl_entry - (IN)Tunnel terminator entry.
 *      info      - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
 int
_bcm_tr3_mpls_tunnel_switch_frr_entry_key_get(int unit, soc_tunnel_term_t *tnl_entry,
                           bcm_mpls_tunnel_switch_t *info)
{
    bcm_port_t port_in, port_out;
    bcm_module_t mod_in, mod_out;
    bcm_trunk_t trunk_id;

    port_in = soc_L3_TUNNELm_field32_get(unit, tnl_entry, MPLS__PORT_NUMf);
    mod_in = soc_L3_TUNNELm_field32_get(unit, tnl_entry, MPLS__MODULE_IDf);
    if (soc_L3_TUNNELm_field32_get(unit, tnl_entry, MPLS__Tf)) {
        trunk_id = soc_L3_TUNNELm_field32_get(unit, tnl_entry, MPLS__TGIDf);
        BCM_GPORT_TRUNK_SET(info->port, trunk_id);
    } else if ((port_in == 0) && (mod_in == 0)) {
        /* Global label, mod/port not part of lookup key */
        info->port = BCM_GPORT_INVALID;
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    mod_in, port_in, &mod_out, &port_out));
        BCM_GPORT_MODPORT_SET(info->port, mod_out, port_out);
    }
    info->label = soc_L3_TUNNELm_field32_get(unit, tnl_entry, MPLS__MPLS_LABELf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_mpls_tunnel_switch_frr_entry_key_init
 * Purpose:
 *      Set MPLS Tunnel-switch entry key for Fast Reroute
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      tnl_info - (IN)MPLS Tunnel terminator parameters. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_mpls_tunnel_switch_frr_entry_key_init(int unit, bcm_mpls_tunnel_switch_t *info, soc_tunnel_term_t *tnl_entry)
{
    soc_mem_t tunnel_mem;
    bcm_module_t mod_out=-1;
    bcm_port_t port_out=-1;
    bcm_trunk_t trunk_id=-1;
    int gport_id=-1;
    int rv=BCM_E_NONE;

    tunnel_mem = L3_TUNNELm;

    if (info->port == BCM_GPORT_INVALID) {
        /* Global label, mod/port not part of lookup key */
        soc_mem_field32_set(unit,  tunnel_mem, tnl_entry, MPLS__MODULE_IDf, 0);
        soc_mem_field32_set(unit,  tunnel_mem, tnl_entry, MPLS__PORT_NUMf, 0);
        if (BCM_XGS3_L3_MPLS_LBL_VALID(info->label)) {
            soc_mem_field32_set(unit,  tunnel_mem, tnl_entry, MPLS__MPLS_LABELf, info->label);
        } else {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit,  tunnel_mem, tnl_entry, VALIDf, 
                (1 << soc_mem_field_length(unit, tunnel_mem, VALIDf)) - 1);

#ifdef BCM_APACHE_SUPPORT
        if (soc_feature(unit, soc_feature_l3_tunnel_mode_fld_is_keytype)) {
            soc_mem_field32_set(unit,  tunnel_mem, tnl_entry,  MODEf, 0x2);
        } else
#endif
        {
            soc_mem_field32_set(unit,  tunnel_mem, tnl_entry,  KEY_TYPEf, 0x2);
        }

    } else {

        rv = _bcm_esw_gport_resolve(unit, info->port, &mod_out, 
                                &port_out, &trunk_id, &gport_id);
        BCM_IF_ERROR_RETURN(rv);

        if (BCM_GPORT_IS_TRUNK(info->port)) {
            soc_mem_field32_set(unit,  tunnel_mem, tnl_entry,  MPLS__Tf, 1);
            soc_mem_field32_set(unit,  tunnel_mem, tnl_entry,  MPLS__TGIDf, trunk_id);
        } else {
            soc_mem_field32_set(unit,  tunnel_mem, tnl_entry,  MPLS__MODULE_IDf, mod_out);
            soc_mem_field32_set(unit,  tunnel_mem, tnl_entry,  MPLS__PORT_NUMf, port_out);
        }
        if (BCM_XGS3_L3_MPLS_LBL_VALID(info->label)) {
            soc_mem_field32_set(unit,  tunnel_mem, tnl_entry, MPLS__MPLS_LABELf, info->label);
        } else {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit,  tunnel_mem, tnl_entry,  VALIDf, 
                (1 << soc_mem_field_length(unit, tunnel_mem, VALIDf)) - 1);

#ifdef BCM_APACHE_SUPPORT
        if (soc_feature(unit, soc_feature_l3_tunnel_mode_fld_is_keytype)) {
            soc_mem_field32_set(unit,  tunnel_mem, tnl_entry,  MODEf, 0x2);
        } else
#endif
        {
            soc_mem_field32_set(unit,  tunnel_mem, tnl_entry,  KEY_TYPEf, 0x2);
        }

    }

    soc_mem_field32_set(unit, tunnel_mem, 
                        tnl_entry, MPLS__MPLS_LABEL_MASKf, 0xfffff);
    soc_mem_field32_set(unit, tunnel_mem, 
                        tnl_entry, MPLS__T_MASKf, 0x1);
    if (SOC_IS_KATANA2(unit) ||
        (soc_feature(unit, soc_feature_l3_tunnel_expanded_mod_mask))) {
        soc_mem_field32_set(unit, tunnel_mem, 
                tnl_entry, MPLS__MODULE_ID_MASKf, 0xff);
    } else {
        soc_mem_field32_set(unit, tunnel_mem, 
                tnl_entry, MPLS__MODULE_ID_MASKf, 0xf);
    }
    soc_mem_field32_set(unit, tunnel_mem, 
                        tnl_entry, MPLS__PORT_NUM_MASKf, 0x7f);
    soc_mem_field32_set(unit, tunnel_mem, 
                        tnl_entry, MPLS__TGID_MASKf, 0x3ff);
#ifdef BCM_APACHE_SUPPORT
    if (soc_feature(unit, soc_feature_l3_tunnel_mode_fld_is_keytype)) {
        soc_mem_field32_set(unit, tunnel_mem, 
                tnl_entry, MPLS__MODE_MASKf, 0x3);
    } else
#endif
    {
        soc_mem_field32_set(unit, tunnel_mem, 
                tnl_entry, MPLS__KEY_TYPE_MASKf, 0x3);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_tr3_mpls_frr_entry_set
 * Purpose:
 *    Set MPLS Tunnel-switch entry for Fast Reroute
 * Parameters:
 *  IN :  Unit
 *  IN :  info
 * Returns:
 *    BCM_E_XXX
 */

int
_bcm_tr3_mpls_tunnel_switch_frr_set(int unit, bcm_mpls_tunnel_switch_t *info)
{
    soc_mem_t tunnel_mem;
    soc_tunnel_term_t tnl_entry;
    uint32 index;
    int rv = BCM_E_NONE;
    int max_int_pri = 15;

    sal_memset(&tnl_entry, 0, sizeof(tnl_entry));
    tunnel_mem = L3_TUNNELm;
   
    /* Setup TCAM key */
    rv = _bcm_tr3_mpls_tunnel_switch_frr_entry_key_init(unit, info, &tnl_entry);
    BCM_IF_ERROR_RETURN(rv);

    if (info->flags & BCM_MPLS_SWITCH_INT_PRI_SET) {
        if ((info->int_pri < BCM_PRIO_MIN) || (info->int_pri > max_int_pri)) {
            return BCM_E_PARAM;
        }
    }

    soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry,  
                                MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                0x1); /* POP */

    if (info->flags & BCM_MPLS_SWITCH_INT_PRI_MAP) {
        soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry, 
                                    MPLS__DECAP_USE_EXP_FOR_PRIf,
                                    0x1); /* MAP_PRI_AND_CNG */
        /* Use specified EXP-map to determine internal prio/color */
        soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry, 
            MPLS__EXP_MAPPING_PTRf,
            info->exp_map & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK);
    } else if ((info->flags & BCM_MPLS_SWITCH_INT_PRI_SET) &&
               (info->flags & BCM_MPLS_SWITCH_COLOR_MAP)) {
        soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry, 
                                    MPLS__DECAP_USE_EXP_FOR_PRIf,
                                    0x2); /* NEW_PRI_MAP_CNG */
        /* Use the specified internal priority value */
        soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry, 
                                    MPLS__NEW_PRIf,
                                    info->int_pri);
        /* Use specified EXP-map to determine internal color */
        soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry, 
            MPLS__EXP_MAPPING_PTRf,
            info->exp_map & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK);
    } else if (info->flags & BCM_MPLS_SWITCH_INT_PRI_SET) {
        soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry, 
                        MPLS__DECAP_USE_EXP_FOR_PRIf, 0x2);
        soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry, 
                        MPLS__NEW_PRIf, info->int_pri);
    } else {
        /* Trust the encapsulated packet's 802.1p pri/cfi, 
         * and use the incoming ports mapping.
         */
        soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry, 
                        MPLS__DECAP_USE_EXP_FOR_PRIf, 0x0); /* NONE */
    }

    if (info->flags & BCM_MPLS_SWITCH_INNER_TTL) {
        soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry, 
                             MPLS__DECAP_USE_TTLf, 0);
    } else {
        soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry, 
                             MPLS__DECAP_USE_TTLf, 1);
    }

    if (info->flags & BCM_MPLS_SWITCH_INNER_EXP) {
         soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry,
                             MPLS__DECAP_USE_EXP_FOR_INNERf, 0);
    } else {
         soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry,
                             MPLS__DECAP_USE_EXP_FOR_INNERf, 1);
    }

    if (info->flags & BCM_MPLS_SWITCH_DROP) {
         soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry,
                             MPLS__DROP_DATA_ENABLEf, 1);
    } else {
         soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry,
                             MPLS__DROP_DATA_ENABLEf, 0);
    }
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (soc_feature(unit, soc_feature_mpls_ecn) && 
        (info->flags & BCM_MPLS_SWITCH_INGRESS_ECN_MAP)) {
        int ecn_map_index;
        int ecn_map_type;
        int ecn_map_num;
        int ecn_map_hw_idx;
        ecn_map_type = info->ecn_map_id & 
                        _BCM_XGS5_MPLS_ECN_MAP_TYPE_MASK;
        ecn_map_index = info->ecn_map_id & 
                        _BCM_XGS5_MPLS_ECN_MAP_NUM_MASK;
        ecn_map_num = 
            soc_mem_index_count(unit, ING_EXP_TO_IP_ECN_MAPPINGm) / 
            _BCM_ECN_MAX_ENTRIES_PER_MAP;
        if (ecn_map_type != _BCM_XGS5_MPLS_ECN_MAP_TYPE_EXP2ECN) {
            return BCM_E_PARAM;
        }
        if (ecn_map_index >= ecn_map_num) {
            return BCM_E_PARAM;
        }
        if (!bcmi_xgs5_ecn_map_used_get(unit, ecn_map_index, 
                                        _bcmEcnmapTypeExp2Ecn)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(
            bcmi_ecn_map_id2hw_idx(unit, info->ecn_map_id, &ecn_map_hw_idx));        
        soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry,
                            MPLS__EXP_TO_IP_ECN_MAPPING_PTRf, ecn_map_hw_idx);
    }
#endif

#ifdef BCM_APACHE_SUPPORT
    /*CLASS_ID used for OAM applications*/
    if (SOC_MEM_FIELD_VALID(unit, tunnel_mem, MPLS__CLASS_IDf)) {
        if (info->class_id) {
            int bit_num = 0;
            bit_num = soc_mem_field_length(unit, tunnel_mem, MPLS__CLASS_IDf);
            if (info->class_id > (( 1 << bit_num ) - 1)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,  tunnel_mem, &tnl_entry,
                    MPLS__CLASS_IDf, info->class_id);
        }
    }
#endif

    /* Add tunnel terminator in TCAM. */
    rv = soc_tunnel_term_insert(unit, &tnl_entry, &index);
    return rv;
}

/*
 * Function:
 *    _bcm_tr3_mpls_tunnel_switch_frr_parse
 * Purpose:
 *    Parse MPLS Tunnel-switch entry for Fast Reroute
 * Parameters:
 *  IN :  Unit
 *  IN :  info
 * Returns:
 */

void
_bcm_tr3_mpls_tunnel_switch_frr_parse(int unit, soc_tunnel_term_t *tnl_entry, bcm_mpls_tunnel_switch_t *tnl_info)
{

    if ( 0x1 == soc_L3_TUNNELm_field32_get (unit, tnl_entry, MPLS__MPLS_ACTION_IF_NOT_BOSf)) {
        tnl_info->action = BCM_MPLS_SWITCH_ACTION_POP;
    }

    if (soc_L3_TUNNELm_field32_get(unit, tnl_entry, MPLS__DROP_DATA_ENABLEf)) {
        tnl_info->flags |= BCM_MPLS_SWITCH_DROP;
    }
    if (!soc_L3_TUNNELm_field32_get(unit, tnl_entry, MPLS__DECAP_USE_TTLf)) {
        tnl_info->flags |= BCM_MPLS_SWITCH_INNER_TTL;
    }
    if (!soc_L3_TUNNELm_field32_get(unit, tnl_entry, MPLS__DECAP_USE_EXP_FOR_INNERf)) {
        tnl_info->flags |= BCM_MPLS_SWITCH_INNER_EXP;
    }
    if (soc_L3_TUNNELm_field32_get(unit, tnl_entry, 
                                    MPLS__DECAP_USE_EXP_FOR_PRIf) == 0x1) {

        /* Use specified EXP-map to determine internal prio/color */
        tnl_info->flags |= BCM_MPLS_SWITCH_INT_PRI_MAP;
        tnl_info->exp_map = 
            soc_L3_TUNNELm_field32_get(unit, tnl_entry, MPLS__EXP_MAPPING_PTRf);
        tnl_info->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
    } else if (soc_L3_TUNNELm_field32_get(unit, tnl_entry, 
                                           MPLS__DECAP_USE_EXP_FOR_PRIf) == 0x2) {

        /* Use the specified internal priority value */
        tnl_info->flags |= BCM_MPLS_SWITCH_INT_PRI_SET;
        tnl_info->int_pri =
            soc_L3_TUNNELm_field32_get(unit, tnl_entry, MPLS__NEW_PRIf);

        /* Use specified EXP-map to determine internal color */
        tnl_info->flags |= BCM_MPLS_SWITCH_COLOR_MAP;
        tnl_info->exp_map = 
            soc_L3_TUNNELm_field32_get(unit, tnl_entry, MPLS__EXP_MAPPING_PTRf);
        tnl_info->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
    }
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (soc_feature(unit, soc_feature_mpls_ecn)) {
        int ecn_map_id;
        int ecn_map_hw_idx;
        int rv;
        ecn_map_hw_idx = soc_L3_TUNNELm_field32_get(unit, tnl_entry, 
                                                    MPLS__EXP_TO_IP_ECN_MAPPING_PTRf);
        rv = bcmi_ecn_map_hw_idx2id(unit, ecn_map_hw_idx, 
                                    _BCM_XGS5_MPLS_ECN_MAP_TYPE_EXP2ECN, 
                                    &ecn_map_id);
        if (BCM_SUCCESS(rv)) {
            tnl_info->flags |= BCM_MPLS_SWITCH_INGRESS_ECN_MAP;
            tnl_info->ecn_map_id = ecn_map_id;
        }
    } 
#endif

#ifdef BCM_APACHE_SUPPORT
    if (SOC_MEM_FIELD_VALID(unit, L3_TUNNELm, MPLS__CLASS_IDf)) {
        tnl_info->class_id =
            soc_L3_TUNNELm_field32_get(unit, tnl_entry, MPLS__CLASS_IDf);
    }
#endif

    /* Set FRR flag */
    tnl_info->flags |= BCM_MPLS_SWITCH_FRR;

    return;
}

/*
 * Function:
 *      _bcm_tr3_mpls_tunnel_switch_frr_delete
 * Purpose:
 *      Delete MPLS Tunnel-switch entry for Fast Reroute
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      tnl_info - (IN)MPLS Tunnel terminator parameters. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_mpls_tunnel_switch_frr_delete(int unit, bcm_mpls_tunnel_switch_t *tnl_info)
{
    soc_tunnel_term_t tnl_entry;
    int rv = BCM_E_NONE;

    sal_memset(&tnl_entry, 0, sizeof(tnl_entry));
   
    /* Setup TCAM key */
    rv = _bcm_tr3_mpls_tunnel_switch_frr_entry_key_init(unit, tnl_info, &tnl_entry);
    BCM_IF_ERROR_RETURN(rv);

    /* Delete tunnel terminator in TCAM. */
    return soc_tunnel_term_delete(unit, &tnl_entry);
}


/*
 * Function:
 *      _bcm_tr3_mpls_tunnel_switch_frr_get
 * Purpose:
 *      Get MPLS Tunnel-switch entry for Fast Reroute
 * Parameters:
 *      unit     -(IN)SOC unit number. 
 *      tnl_info -(IN)Buffer to fill tunnel info.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_mpls_tunnel_switch_frr_get(int unit, bcm_mpls_tunnel_switch_t *tnl_info, int *index)
{
    soc_tunnel_term_t tnl_entry;
    soc_tunnel_term_t tnl_key;
    int rv = BCM_E_NONE;
    int entry_index;

    sal_memset(&tnl_key, 0, sizeof(tnl_key));
    sal_memset(&tnl_entry, 0, sizeof(tnl_entry));

    /* Setup TCAM key */
    rv = _bcm_tr3_mpls_tunnel_switch_frr_entry_key_init(unit, tnl_info, &tnl_key);
    BCM_IF_ERROR_RETURN(rv);

    /* Find tunnel terminator in TCAM. */
    BCM_IF_ERROR_RETURN(soc_tunnel_term_match(unit, &tnl_key, &tnl_entry, &entry_index));
    *index = entry_index;

    /* Parse HW entry to User Information */
    (void) _bcm_tr3_mpls_tunnel_switch_frr_parse(unit, &tnl_entry, tnl_info);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_add
 * Purpose:
 *      Add an MPLS label entry.
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_mpls_tunnel_switch_add(int unit, bcm_mpls_tunnel_switch_t *info)
{
    int rv = BCM_E_NONE;
    mpls_entry_entry_t ment;
    mpls_entry_extd_entry_t ment_extd;
    int mode=0, nh_index = -1;
    int index, forwarding_field_type=-1, old_action = -1;
    int old_nh_index = -1, old_ecmp_index = -1;
    int  tunnel_switch_update=-1;
    int  ref_count=0, bud_node_trigger=0;
    bcm_if_t  egress_if=0;
    uint8   key_type_1 = 0x10;
    uint8   key_type_2 = 0x12;
    soc_mem_t mem = MPLS_ENTRYm;
    void *entry_buffer = &ment;
    int v4_v6_override;

    rv = bcm_xgs3_l3_egress_mode_get(unit, &mode);
    BCM_IF_ERROR_RETURN(rv);
    if (!mode) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 egress mode must be set first\n")));
        return BCM_E_DISABLED;
    }

    /* Check for Port_independent Label mapping */
    if (!BCM_XGS3_L3_MPLS_LBL_VALID(info->label)) {
        return BCM_E_PARAM;
    }

    /* BCM_MPLS_SWITCH_KEEP_TTL flag will work only for swap action and
     * it is ineffective for other actions.
     */
    if ((info->flags & BCM_MPLS_SWITCH_KEEP_TTL) &&
        (info->action != BCM_MPLS_SWITCH_ACTION_SWAP)) {
        return BCM_E_PARAM;
    }

    rv = bcm_tr_mpls_port_independent_range (unit, info->label, info->port);
    if (rv < 0) {
        return rv;
    }

    sal_memset(&ment, 0, sizeof(mpls_entry_entry_t));
    sal_memset(&ment_extd, 0, sizeof(mpls_entry_extd_entry_t));

    if (info->flags & BCM_MPLS_SWITCH_P2MP) {
        BCM_IF_ERROR_RETURN(
             _bcm_tr3_mpls_entry_set_key(unit, info, (uint32 *) &ment, MPLS_ENTRYm, key_type_1));

        /* See if entry already exists */
        rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                       &ment, &ment, 0);
        if (BCM_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN(
                _bcm_tr3_mpls_entry_convert_to_extd_entry_key(unit, &ment, &ment_extd));
            rv = soc_mem_search(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY, &index,
                       &ment_extd, &ment_extd, 0);
            if(rv == SOC_E_NONE) {
                mem = MPLS_ENTRY_EXTDm;
                entry_buffer = &ment_extd;
            }
        } else {
            mem = MPLS_ENTRYm;
            entry_buffer = &ment;
        }

        if (rv == SOC_E_NONE) {
            /* Entry exists */
            old_action =
                  soc_mem_field32_get(unit, mem, entry_buffer, MPLS__MPLS_ACTION_IF_BOSf);
            if (old_action == 5) { /* SWAP_L3MC */
                  if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
                       /* Update existing SWAP entry */
                       tunnel_switch_update = 1;
                  } else if (info->action == BCM_MPLS_SWITCH_ACTION_POP) {
                       /* Create New POP Entry */
                       tunnel_switch_update = 0;
                       bud_node_trigger = 1;
                  }
            } else if (old_action == 2) { /* POP L3_IIF */
                  if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
                       /* Create New SWAP entry */
                       tunnel_switch_update = 0;
                       bud_node_trigger = 1;
                  } else if (info->action == BCM_MPLS_SWITCH_ACTION_POP) {
                       /* Update POP Entry */
                       tunnel_switch_update = 1;
                  }
            }

            if ((soc_mem_field32_get(unit, mem, entry_buffer, MPLS__P2MP_LOCAL_RECEIVERS_PRESENTf)) &&
                       (info->action == BCM_MPLS_SWITCH_ACTION_POP)) {
                /* See if Second-pass BUD entry exists */
                BCM_IF_ERROR_RETURN(
                    _bcm_tr3_mpls_entry_set_key(unit, info, (uint32 *) &ment, MPLS_ENTRYm, key_type_2));
                rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                       &ment, &ment, 0);
                if (BCM_FAILURE(rv)) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_tr3_mpls_entry_convert_to_extd_entry_key(unit, &ment, &ment_extd));
                    rv = soc_mem_search(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY, &index,
                               &ment_extd, &ment_extd, 0);
                    if(rv == SOC_E_NONE) {
                        mem = MPLS_ENTRY_EXTDm;
                        entry_buffer = &ment_extd;
                    }
                } else {
                    mem = MPLS_ENTRYm;
                    entry_buffer = &ment;
                }
                if (rv == SOC_E_NONE) {
                   /* Entry exists */
                   old_action =
                      soc_mem_field32_get(unit, mem, entry_buffer, MPLS__MPLS_ACTION_IF_BOSf);
                   if (old_action == 2) { /* POP L3_IIF */
                       if (info->action == BCM_MPLS_SWITCH_ACTION_POP) {
                           /* Update existing POP entry */
                           tunnel_switch_update = 1;
                           bud_node_trigger = 0;
                       }
                   }
                }
           }
        }
    }    else  if (info->flags & BCM_MPLS_SWITCH_FRR) {
              rv = _bcm_tr3_mpls_tunnel_switch_frr_set(unit, info);
              return rv;
    } else {

         BCM_IF_ERROR_RETURN(
              _bcm_tr3_mpls_entry_set_key(unit, info, (uint32 *)&ment, MPLS_ENTRYm, key_type_1));

         /* See if entry already exists */
         rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        &ment, &ment, 0);
         if (BCM_FAILURE(rv)) {
             BCM_IF_ERROR_RETURN(
                 _bcm_tr3_mpls_entry_convert_to_extd_entry_key(unit, &ment, &ment_extd));
             rv = soc_mem_search(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY, &index,
                        &ment_extd, &ment_extd, 0);
             if(rv == SOC_E_NONE) {
                 mem = MPLS_ENTRY_EXTDm;
                 entry_buffer = &ment_extd;
             }
         } else {
             mem = MPLS_ENTRYm;
             entry_buffer = &ment;
         }

         if (rv == SOC_E_NONE) {
             /* Entry exists */
             tunnel_switch_update = 1;
         }
    }

   
    if (rv != BCM_E_NONE) {
         if (rv != SOC_E_NOT_FOUND) {
              return rv;
         } else if (rv == SOC_E_NOT_FOUND) {
              tunnel_switch_update = 0;
         }
    } else {
           if (tunnel_switch_update == -1) {
                return BCM_E_PARAM;
           }
    }

    if (tunnel_switch_update == 1) { /* Update existing entry */
        old_action =
              soc_mem_field32_get(unit, mem, entry_buffer, MPLS__MPLS_ACTION_IF_BOSf);
        switch (old_action) {
            case 3: /* SWAP_NHI */
            case 6: /* PHP_NHI */
                         forwarding_field_type = _BCM_MPLS_FORWARD_NEXT_HOP;
                         break;
            case 4: /* SWAP_ECMP */
            case 7: /* PHP_ECMP */
                         forwarding_field_type = _BCM_MPLS_FORWARD_ECMP_GROUP;
                         break;
            case 5: /* SWAP_L3MC_INDEX */
                         forwarding_field_type = _BCM_MPLS_FORWARD_MULTICAST_GROUP;
                         break;
            default:
                           break;
        }
        if (forwarding_field_type == _BCM_MPLS_FORWARD_NEXT_HOP) {
             old_nh_index = 
                   soc_mem_field32_get(unit, mem, entry_buffer, MPLS__NEXT_HOP_INDEXf);
        } else if (forwarding_field_type == _BCM_MPLS_FORWARD_ECMP_GROUP) {
             old_ecmp_index = 
                   soc_mem_field32_get(unit, mem, entry_buffer, MPLS__ECMP_PTRf);
        }  else if (forwarding_field_type == _BCM_MPLS_FORWARD_MULTICAST_GROUP) {
#if 0
            
             int old_ipmc_index = 
                 soc_MPLS_ENTRYm_field32_get(unit, &ment, MPLS__L3MC_INDEXf);
#endif
        }
    }

    if (MPLS_ENTRY_EXTDm == mem) {
        /* Convert extentened memory to regular memory */
        BCM_IF_ERROR_RETURN (
         _bcm_esw_mpls_entry_convert_to_regular_entry(unit, &ment_extd, &ment));
    }
    switch(info->action) {
         case BCM_MPLS_SWITCH_ACTION_SWAP:
              rv = _bcm_tr3_mpls_process_swap_label_action(unit, info,
                             bud_node_trigger, &nh_index, &ment );
              if (rv < 0) {
                   goto cleanup;
              }
              break;

         case BCM_MPLS_SWITCH_ACTION_POP:
              rv = _bcm_tr3_mpls_process_pop_label_action(unit, info,
                             bud_node_trigger, old_action, &ment );
              if (rv < 0) {
                   goto cleanup;
              }
              break;

         case BCM_MPLS_SWITCH_ACTION_PHP:
              rv = _bcm_tr3_mpls_process_php_label_action(unit, info,
                             &nh_index, &ment );
              if (rv < 0) {
                   goto cleanup;
              }
              break;

         default:
              return BCM_E_PARAM;
              break;
    }
    if (MPLS_ENTRY_EXTDm == mem) {
        /* Update data into extended memory buffer*/
        uint32 fval[2]={0};
        fval[0] = 0;
        fval[1] = 0;
        soc_mem_field_get(unit, MPLS_ENTRYm,(uint32 *)(&ment), MPLS__DATAf,fval);
        soc_mem_field_set(unit, MPLS_ENTRY_EXTDm,
                     (uint32 *)(&ment_extd), MPLS__DATA_0f, fval);
    }

    v4_v6_override = soc_property_get(unit, spn_MPLS_SWITCH_IPV4_IPV6_INDEPENDENT_CONTROL, 0);
    if (!v4_v6_override) {
        soc_mem_field32_set(unit, mem, entry_buffer, MPLS__V4_ENABLEf, 1);
        soc_mem_field32_set(unit, mem, entry_buffer, MPLS__V6_ENABLEf, 1);
    } else {
        soc_mem_field32_set(unit, mem, entry_buffer, MPLS__V4_ENABLEf, 0);
        soc_mem_field32_set(unit, mem, entry_buffer, MPLS__V6_ENABLEf, 0);
        if (info->flags2 & BCM_MPLS_SWITCH2_ENABLE_IPV4) {
            soc_mem_field32_set(unit, mem, entry_buffer, MPLS__V4_ENABLEf, 1);
        }
        if (info->flags2 & BCM_MPLS_SWITCH2_ENABLE_IPV6) {
            soc_mem_field32_set(unit, mem, entry_buffer, MPLS__V6_ENABLEf, 1);
        }
    }
    if (info->flags & BCM_MPLS_SWITCH_INNER_TTL) {
        if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        soc_mem_field32_set(unit, mem, entry_buffer, MPLS__DECAP_USE_TTLf, 0);
    } else {
        soc_mem_field32_set(unit, mem, entry_buffer, MPLS__DECAP_USE_TTLf, 1);
    }
    if (info->flags & BCM_MPLS_SWITCH_INNER_EXP) {
        if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        soc_mem_field32_set(unit, mem, entry_buffer, MPLS__DECAP_USE_EXP_FOR_INNERf, 0);
    } else {
        /* For SWAP, Do-not PUSH EXP */
        if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
            soc_mem_field32_set(unit, mem, entry_buffer, MPLS__DECAP_USE_EXP_FOR_INNERf, 0);
        } else {
            soc_mem_field32_set(unit, mem, entry_buffer, MPLS__DECAP_USE_EXP_FOR_INNERf, 1);
        }
    }
    if (info->flags & BCM_MPLS_SWITCH_DROP) {
         soc_mem_field32_set(unit, mem, entry_buffer,
                             MPLS__DROP_DATA_ENABLEf, 1);
    } else {
         soc_mem_field32_set(unit, mem, entry_buffer,
                             MPLS__DROP_DATA_ENABLEf, 0);
    }

    (void) bcm_tr3_mpls_entry_internal_qos_set(unit, NULL, info, entry_buffer);

    if ((!tunnel_switch_update) && (!bud_node_trigger)) {
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry_buffer);
    } else {
        rv = soc_mem_write(unit, mem,
                           MEM_BLOCK_ALL, index, entry_buffer);
    }

    if (rv < 0) {
        goto cleanup;
    }

    if ((tunnel_switch_update) && 
              (forwarding_field_type != _BCM_MPLS_FORWARD_MULTICAST_GROUP)) {
        if (old_action ==  0x3) { /* SWAP_NHI */
            /* Check if tunnel_switch.egress_label mode is being used */
            if (forwarding_field_type == _BCM_MPLS_FORWARD_NEXT_HOP) {
                rv = bcm_tr_mpls_get_vp_nh (unit, (bcm_if_t) old_nh_index, &egress_if);
                if (rv == BCM_E_NONE) {
                    rv = bcm_tr_mpls_l3_nh_info_delete(unit,  old_nh_index);
                } else {
                    /* Decrement next-hop Reference count */
                    rv = bcm_xgs3_get_ref_count_from_nhi(unit, 0, &ref_count,  old_nh_index);
                }
            }
        } else if (old_action == 0x4) {/* SWAP_ECMP */
            if (forwarding_field_type == _BCM_MPLS_FORWARD_ECMP_GROUP) {
                rv = bcm_xgs3_ecmp_group_del(unit, old_ecmp_index, 0);
            }
        } else if (old_action == 0x6) {/* PHP_NHI */
            if (forwarding_field_type == _BCM_MPLS_FORWARD_NEXT_HOP) {
                rv = bcm_xgs3_nh_del(unit, 0, old_nh_index);
            } 
        } else if (old_action == 0x7) {/* PHP_ECMP */
            if (forwarding_field_type == _BCM_MPLS_FORWARD_ECMP_GROUP) {
                rv = bcm_xgs3_ecmp_group_del(unit, old_ecmp_index, 0);
            }
        }
    }
    if (rv < 0) {
        goto cleanup;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return rv;

  cleanup:
    if (nh_index != -1) {
        if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
            if (BCM_XGS3_L3_MPLS_LBL_VALID(info->egress_label.label) ||
                (info->action == BCM_MPLS_SWITCH_ACTION_PHP)) {
                (void) bcm_tr_mpls_l3_nh_info_delete(unit, nh_index);
            }
        } else if (info->action == BCM_MPLS_SWITCH_ACTION_PHP) {
            (void) bcm_xgs3_nh_del(unit, 0, nh_index);
        }
    }
    return rv;
}

/*
 * Function:
 *      bcm_tr3_mpls_tunnel_switch_delete
 * Purpose:
 *      Delete an MPLS label entry.
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_mpls_tunnel_switch_delete(int unit, bcm_mpls_tunnel_switch_t *info)
{
    int index;
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t  hw_mem;
    int rv = BCM_E_NONE;

    if (info->flags & BCM_MPLS_SWITCH_FRR) {
         rv = _bcm_tr3_mpls_tunnel_switch_frr_delete(unit, info);
    } else {
        /* Search for MPLS entry */
        rv = _bcm_tr3_mpls_entry_find(unit, info, hw_buf, 
                               &hw_mem, &index);
        /* If Not found in both Memory, then return error */
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
        rv = _bcm_tr3_mpls_entry_delete(unit, hw_buf, hw_mem, info, index);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return rv;
}

/*
 * Function:
 *      bcm_tr3_mpls_tunnel_switch_delete_all
 * Purpose:
 *      Delete all MPLS label entries.
 * Parameters:
 *      unit   - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_mpls_tunnel_switch_delete_all(int unit)
{
    int i, num_entries;
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t  hw_mem;
    bcm_mpls_tunnel_switch_t info;
    soc_tunnel_term_t tnl_entry;
   uint32 key_type=0;
    int rv = BCM_E_NONE;

    

    hw_mem = MPLS_ENTRYm;
    num_entries = soc_mem_index_count(unit, hw_mem);
    for (i = 0; i < num_entries; i++) {
        sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        hw_mem = MPLS_ENTRYm;
        rv = READ_MPLS_ENTRYm(unit, MEM_BLOCK_ANY, i, hw_buf);
        if (rv < 0) {
            return rv;
        }
        key_type = soc_mem_field32_get(unit, hw_mem, hw_buf, KEY_TYPEf);
        if ((key_type == 0x11) || (key_type == 0x13)) {
             sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
             hw_mem = MPLS_ENTRY_EXTDm;
             rv = READ_MPLS_ENTRY_EXTDm(unit, MEM_BLOCK_ANY, i/2, hw_buf);
             if (rv < 0) {
                 return rv;
             }
             if (!soc_mem_field32_get(unit, hw_mem, hw_buf, VALID_0f)) {
                 continue;
             }
             if (!soc_mem_field32_get(unit, hw_mem, hw_buf, VALID_1f)) {
                 continue;
             }
        } else  if ((key_type == 0x10) || (key_type == 0x12)) {
             if (!soc_mem_field32_get(unit, hw_mem, hw_buf, VALIDf)) {
                 continue;
             }
        } else {
             continue;
        }

        bcm_mpls_tunnel_switch_t_init(&info);

        if (soc_mem_field32_get(unit, hw_mem, hw_buf,
                                        MPLS__MPLS_ACTION_IF_BOSf) == 0x1) {
            /* L2_SVP */
            continue;
        }
        rv = _bcm_tr3_mpls_entry_get_key(unit, hw_buf, hw_mem, &info);
        if (rv < 0) {
            return rv;
        }
        rv = _bcm_tr3_mpls_entry_get_data(unit, hw_buf, hw_mem, &info);
        if (rv < 0) {
            return rv;
        }
        rv = _bcm_tr3_mpls_entry_delete(unit, hw_buf, hw_mem, &info, i );
        if (rv < 0) {
            return rv;
        }
    }

    num_entries = soc_mem_index_count(unit, L3_TUNNELm);
    for (i = 0; i < num_entries; i++) {
        sal_memset(&tnl_entry, 0, sizeof(tnl_entry));
        rv = READ_L3_TUNNELm(unit, MEM_BLOCK_ANY, i, (uint32 *)&tnl_entry.entry_arr[0]);
        if (rv < 0) {
            return rv;
        }
        if (!soc_L3_TUNNELm_field32_get(unit, &tnl_entry, VALIDf)) {
            continue;
        }
        if (0x2 != soc_L3_TUNNELm_field32_get(unit, &tnl_entry, KEY_TYPEf)) {
            continue;
        }

        rv = soc_tunnel_term_delete(unit, &tnl_entry);
        if (rv < 0) {
            return rv;
        }

        /*_soc_tunnel_term_slot_free() api moves last entry to
         *deleted entry position using _soc_tunnel_term_entry_shift().
         *Therefore adding this index change to support delete all*/
        i--;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_mpls_tunnel_switch_get
 * Purpose:
 *      Get an MPLS label entry.
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_mpls_tunnel_switch_get(int unit, bcm_mpls_tunnel_switch_t *info)
{
    int index;
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t  hw_mem;
    int rv = BCM_E_NONE;

    if (info->flags & BCM_MPLS_SWITCH_FRR) {
         rv = _bcm_tr3_mpls_tunnel_switch_frr_get(unit, info, &index);
    } else {
        /* Search for MPLS entry */
        rv = _bcm_tr3_mpls_entry_find(unit, info, hw_buf, 
                               &hw_mem, &index);
        /* If Not found in both Memory, then return error */
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
         rv = _bcm_tr3_mpls_entry_get_data(unit, hw_buf, hw_mem, info);
    }
    return rv;
}

/*
 * Function:
 *      bcm_tr3_mpls_tunnel_switch_traverse
 * Purpose:
 *      Traverse all valid MPLS label entries an call the
 *      supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      cb        - User callback function, called once per MPLS entry.
 *      user_data - cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_mpls_tunnel_switch_traverse(int unit, 
                                   bcm_mpls_tunnel_switch_traverse_cb cb,
                                   void *user_data)
{
    int i, num_entries;
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t  hw_mem;
    bcm_mpls_tunnel_switch_t info;
    uint32 key_type=0;
    int rv = BCM_E_NONE;
    soc_tunnel_term_t tnl_entry;


    

    num_entries = soc_mem_index_count(unit, MPLS_ENTRYm);
    for (i = 0; i < num_entries; i++) {
        sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        hw_mem = MPLS_ENTRYm;
        rv = READ_MPLS_ENTRYm(unit, MEM_BLOCK_ANY, i, hw_buf);
        if (rv < 0) {
            return rv;
        }
        key_type = soc_mem_field32_get(unit, hw_mem, hw_buf, KEY_TYPEf);
        if ((key_type == 0x11) || (key_type == 0x13)) {
             sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
             hw_mem = MPLS_ENTRY_EXTDm;
             rv = READ_MPLS_ENTRY_EXTDm(unit, MEM_BLOCK_ANY, i/2, hw_buf);
             if (rv < 0) {
                 return rv;
             }
             if (!soc_mem_field32_get(unit, hw_mem, hw_buf, VALID_0f)) {
                 continue;
             }
             if (!soc_mem_field32_get(unit, hw_mem, hw_buf, VALID_1f)) {
                 continue;
             }
        } else  if ((key_type == 0x10) || (key_type == 0x12)) {
             if (!soc_mem_field32_get(unit, hw_mem, hw_buf, VALIDf)) {
                 continue;
             }
        } else {
             continue;
        }

        if (soc_mem_field32_get(unit, hw_mem, hw_buf,
                                        MPLS__MPLS_ACTION_IF_BOSf) == 0x1) {
            /* L2_SVP */
            continue;
        }
        sal_memset(&info, 0, sizeof(bcm_mpls_tunnel_switch_t));
        rv = _bcm_tr3_mpls_entry_get_key(unit, hw_buf, hw_mem, &info);
        if (rv < 0) {
            return rv;
        }
        rv = _bcm_tr3_mpls_entry_get_data(unit, hw_buf, hw_mem, &info);
        if (rv < 0) {
            return rv;
        }
        rv = cb(unit, &info, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#endif
    }

    /* traverse L3_TUNNEL entries */
    num_entries = soc_mem_index_count(unit, L3_TUNNELm);

    for (i = 0; i < num_entries; i++) {

        sal_memset(&tnl_entry, 0, sizeof(tnl_entry));
        BCM_IF_ERROR_RETURN
            (READ_L3_TUNNELm(unit, MEM_BLOCK_ANY, i, (uint32 *)&tnl_entry.entry_arr[0]));

        if (!soc_L3_TUNNELm_field32_get (unit, &tnl_entry, VALIDf)) {
            continue;
        }

        /* Check for MPLS entry key  */
        key_type = soc_L3_TUNNELm_field32_get(unit, &tnl_entry, KEY_TYPEf);
        if (key_type != 0x2) {
            continue;
        }

        sal_memset(&info, 0, sizeof(bcm_mpls_tunnel_switch_t));
        rv = _bcm_tr3_mpls_tunnel_switch_frr_entry_key_get (unit, &tnl_entry, &info);
        if (rv < 0) {
            return rv;
        }

        (void) _bcm_tr3_mpls_tunnel_switch_frr_parse(unit, &tnl_entry, &info);
        /* coverity[dead_error_begin] */          
        rv = cb(unit, &info, user_data);
        /* coverity[dead_error_begin] */
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#endif
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *		_bcm_tr3_mpls_match_label_entry_update
 * Purpose:
 *		Update Match MPLS Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  mpls_entry
 *  OUT :  mpls_entry

 */

STATIC int
_bcm_tr3_mpls_match_label_entry_update(int unit, bcm_mpls_port_t *mpls_port, 
                                                                     mpls_entry_entry_t *ment, 
                                                                     mpls_entry_entry_t *return_ment)
{
    uint32 value, key_type;
    bcm_module_t mod_id;
    bcm_port_t port_id;
    bcm_trunk_t trunk_id;
    bcm_mpls_label_t match_label;

    /* Check if Key_Type identical */
    key_type = soc_MPLS_ENTRYm_field32_get (unit, ment, KEY_TYPEf);
    value = soc_MPLS_ENTRYm_field32_get (unit, return_ment, KEY_TYPEf);
    if (key_type != value) {
         return BCM_E_PARAM;
    }

    value = soc_MPLS_ENTRYm_field32_get (unit, ment, MPLS__Tf);
    if (value) {
         soc_MPLS_ENTRYm_field32_set(unit, return_ment, MPLS__Tf, value);
         trunk_id = soc_MPLS_ENTRYm_field32_get (unit, ment, MPLS__TGIDf);
         soc_MPLS_ENTRYm_field32_set(unit, return_ment, MPLS__TGIDf, trunk_id);
    } else {
         mod_id = soc_MPLS_ENTRYm_field32_get (unit, ment, MPLS__MODULE_IDf);
         soc_MPLS_ENTRYm_field32_set(unit, return_ment, MPLS__MODULE_IDf, mod_id);
         port_id = soc_MPLS_ENTRYm_field32_get (unit, ment, MPLS__PORT_NUMf);
         soc_MPLS_ENTRYm_field32_set(unit, return_ment, MPLS__PORT_NUMf, port_id);
    }

    match_label = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__MPLS_LABELf);
    soc_MPLS_ENTRYm_field32_set(unit, return_ment, MPLS__MPLS_LABELf, match_label);
    soc_MPLS_ENTRYm_field32_set(unit, return_ment, VALIDf, 1);

    value = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__SOURCE_VPf);
    soc_MPLS_ENTRYm_field32_set(unit, return_ment, MPLS__SOURCE_VPf, value);
    soc_MPLS_ENTRYm_field32_set(unit, return_ment, MPLS__MPLS_ACTION_IF_BOSf, 0x1); /* L2 SVP */
    value = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__MPLS_ACTION_IF_NOT_BOSf);
    soc_MPLS_ENTRYm_field32_set(unit, return_ment, MPLS__MPLS_ACTION_IF_NOT_BOSf, value);
    value = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__CW_CHECK_CTRLf);
    soc_MPLS_ENTRYm_field32_set(unit, return_ment, MPLS__CW_CHECK_CTRLf, value);
    value = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__DECAP_USE_EXP_FOR_PRIf);
    soc_MPLS_ENTRYm_field32_set(unit, return_ment, MPLS__DECAP_USE_EXP_FOR_PRIf, 
                                                           value);
    value = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__EXP_MAPPING_PTRf);
    soc_MPLS_ENTRYm_field32_set(unit, return_ment, MPLS__EXP_MAPPING_PTRf, value);
    value = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__NEW_PRIf);
    soc_MPLS_ENTRYm_field32_set(unit, return_ment, MPLS__NEW_PRIf, value);
    if (soc_mem_field_valid(unit, MPLS_ENTRYm, MPLS__TRUST_OUTER_DOT1P_PTRf)) {
         value = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__TRUST_OUTER_DOT1P_PTRf);
         soc_MPLS_ENTRYm_field32_set(unit, return_ment, MPLS__TRUST_OUTER_DOT1P_PTRf, 
                                                           value);
    }
   return BCM_E_NONE;
}


/*
 * Function:
 *		_bcm_tr3_mpls_match_label_entry_set
 * Purpose:
 *		Set MPLS Match Label Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  mpls_port
 *  IN :  mpls_entry
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_tr3_mpls_match_label_entry_set(int unit, bcm_mpls_port_t *mpls_port, mpls_entry_entry_t *ment)
{
    mpls_entry_entry_t return_ment;
    int rv = BCM_E_UNAVAIL;
    int index;

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        ment, &return_ment, 0);

    if (rv == SOC_E_NONE) {
         BCM_IF_ERROR_RETURN(
              _bcm_tr3_mpls_match_label_entry_update (unit, mpls_port, ment, &return_ment));
         rv = soc_mem_write(unit, MPLS_ENTRYm,
                                           MEM_BLOCK_ALL, index,
                                           &return_ment);
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    } else {
             rv = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, ment);
    }

    return rv;
}
/*
 * Function:
 *		_bcm_tr3_mpls_match_vlan_extd_entry_update
 * Purpose:
 *		Update MPLS Match Vlan_xlate_extd  Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  vlan_xlate_extd_entry_t
 *  OUT :  vlan_xlate_extd_entry_t

 */

STATIC int
_bcm_tr3_mpls_match_vlan_extd_entry_update(int unit, vlan_xlate_extd_entry_t *vent, 
                                                                     vlan_xlate_extd_entry_t *return_ent)
{
    uint32  vp, key_type, value;


    /* Check if Key_Type identical */
    key_type = soc_VLAN_XLATE_EXTDm_field32_get (unit, vent, KEY_TYPE_0f);
    value = soc_VLAN_XLATE_EXTDm_field32_get (unit, return_ent, KEY_TYPE_0f);
    if (key_type != value) {
         return BCM_E_PARAM;
    }

    /* Retain original Keys -- Update data only */
    soc_VLAN_XLATE_EXTDm_field32_set(unit, return_ent, XLATE__MPLS_ACTIONf, 0x1);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, return_ent, XLATE__DISABLE_VLAN_CHECKSf, 1);
    vp = soc_VLAN_XLATE_EXTDm_field32_get (unit, vent, XLATE__SOURCE_VPf);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, return_ent, XLATE__SOURCE_VPf, vp);

   return BCM_E_NONE;
}


/*
 * Function:
 *        _bcm_tr3_mpls_match_vlan_extd_entry_set
 * Purpose:
 *       Set MPLS Match Vlan Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  mpls_port
 *  IN :  vlan_xlate_extd_entry_t
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_mpls_match_vlan_extd_entry_set(int unit, bcm_mpls_port_t *mpls_port, vlan_xlate_extd_entry_t *vent)
{
    vlan_xlate_extd_entry_t return_vent;
    int rv = BCM_E_UNAVAIL;
    int index;

    rv = soc_mem_search(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY, &index,
                        vent, &return_vent, 0);

    if (rv == SOC_E_NONE) {
         BCM_IF_ERROR_RETURN(
              _bcm_tr3_mpls_match_vlan_extd_entry_update (unit, vent, &return_vent));
         rv = soc_mem_write(unit, VLAN_XLATE_EXTDm,
                                           MEM_BLOCK_ALL, index,
                                           &return_vent);
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    } else {
         if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
             return BCM_E_NOT_FOUND;
         } else {
              rv = soc_mem_insert(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ALL, vent);
         }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_mpls_extd_entry_update
 * Purpose:
 *      Update extended MPLS entry
 * Parameters:
 *      unit    - (IN) Device Number
 *      mpls_port - (IN) mpls port information
 *      ment_extd  - (IN) old extended MPLS entry
 *      return_ment_extd  - (IN) new extended MPLS entry
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_mpls_extd_entry_update(int unit, bcm_mpls_port_t *mpls_port,
                              mpls_entry_extd_entry_t *ment_extd,
                              mpls_entry_extd_entry_t *return_ment_extd)
{
    uint32 value, key_type;
    bcm_module_t mod_id;
    bcm_port_t port_id;
    bcm_trunk_t trunk_id;
    bcm_mpls_label_t match_label;

    /* Check if Key_Type identical */
    key_type = soc_MPLS_ENTRY_EXTDm_field32_get (unit, ment_extd, KEY_TYPE_0f);
    value = soc_MPLS_ENTRY_EXTDm_field32_get (unit, return_ment_extd, KEY_TYPE_0f);
    if (key_type != value){
        return BCM_E_PARAM;
    }

    key_type = soc_MPLS_ENTRY_EXTDm_field32_get (unit, ment_extd, KEY_TYPE_1f);
    value = soc_MPLS_ENTRY_EXTDm_field32_get (unit, return_ment_extd, KEY_TYPE_1f);
    if (key_type != value){
        return BCM_E_PARAM;
    }

    value = soc_MPLS_ENTRY_EXTDm_field32_get (unit, ment_extd, MPLS__Tf);
    if (value) {
         soc_MPLS_ENTRY_EXTDm_field32_set(unit, return_ment_extd, MPLS__Tf, value);
         trunk_id = soc_MPLS_ENTRY_EXTDm_field32_get (unit, ment_extd, MPLS__TGIDf);
         soc_MPLS_ENTRY_EXTDm_field32_set(unit, return_ment_extd, MPLS__TGIDf, trunk_id);
    } else {
         mod_id = soc_MPLS_ENTRY_EXTDm_field32_get (unit, ment_extd, MPLS__MODULE_IDf);
         soc_MPLS_ENTRY_EXTDm_field32_set(unit, return_ment_extd, MPLS__MODULE_IDf, mod_id);
         port_id = soc_MPLS_ENTRY_EXTDm_field32_get (unit, ment_extd, MPLS__PORT_NUMf);
         soc_MPLS_ENTRY_EXTDm_field32_set(unit, return_ment_extd, MPLS__PORT_NUMf, port_id);
    }

    match_label = soc_MPLS_ENTRY_EXTDm_field32_get(unit, ment_extd, MPLS__MPLS_LABELf);
    soc_MPLS_ENTRY_EXTDm_field32_set(unit, return_ment_extd, MPLS__MPLS_LABELf, match_label);
    soc_MPLS_ENTRY_EXTDm_field32_set(unit, return_ment_extd, VALIDf, 1);

    value = soc_MPLS_ENTRY_EXTDm_field32_get(unit, ment_extd, MPLS__SOURCE_VPf);
    soc_MPLS_ENTRY_EXTDm_field32_set(unit, return_ment_extd, MPLS__SOURCE_VPf, value);
    soc_MPLS_ENTRY_EXTDm_field32_set(unit, return_ment_extd, MPLS__MPLS_ACTION_IF_BOSf, 0x1); /* L2 SVP */

    value = soc_MPLS_ENTRY_EXTDm_field32_get(unit, ment_extd, MPLS__MPLS_ACTION_IF_NOT_BOSf);
    soc_MPLS_ENTRY_EXTDm_field32_set(unit, return_ment_extd, MPLS__MPLS_ACTION_IF_NOT_BOSf, value);
    value = soc_MPLS_ENTRY_EXTDm_field32_get(unit, ment_extd, MPLS__CW_CHECK_CTRLf);
    soc_MPLS_ENTRY_EXTDm_field32_set(unit, return_ment_extd, MPLS__CW_CHECK_CTRLf, value);
    value = soc_MPLS_ENTRY_EXTDm_field32_get(unit, ment_extd, MPLS__DECAP_USE_EXP_FOR_PRIf);
    soc_MPLS_ENTRY_EXTDm_field32_set(unit, return_ment_extd, MPLS__DECAP_USE_EXP_FOR_PRIf,
                                                           value);
    value = soc_MPLS_ENTRY_EXTDm_field32_get(unit, ment_extd, MPLS__EXP_MAPPING_PTRf);
    soc_MPLS_ENTRY_EXTDm_field32_set(unit, return_ment_extd, MPLS__EXP_MAPPING_PTRf, value);
    value = soc_MPLS_ENTRY_EXTDm_field32_get(unit, ment_extd, MPLS__NEW_PRIf);
    soc_MPLS_ENTRY_EXTDm_field32_set(unit, return_ment_extd, MPLS__NEW_PRIf, value);
    if (soc_mem_field_valid(unit, MPLS_ENTRY_EXTDm, MPLS__TRUST_OUTER_DOT1P_PTRf)) {
         value = soc_MPLS_ENTRY_EXTDm_field32_get(unit, ment_extd, MPLS__TRUST_OUTER_DOT1P_PTRf);
         soc_MPLS_ENTRY_EXTDm_field32_set(unit, return_ment_extd, MPLS__TRUST_OUTER_DOT1P_PTRf,
                                                           value);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_mpls_extd_entry_add
 * Purpose:
 *      Write extended MPLS table entry
 * Parameters:
 *      unit    - (IN) Device Number
 *      mpls_port - (IN) mpls port information
 *      ment_extd  - (IN) Extended MPLS table entry
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_mpls_extd_entry_add(int unit, bcm_mpls_port_t *mpls_port,
                                mpls_entry_extd_entry_t *ment_extd)
{
    mpls_entry_extd_entry_t return_ment;
    int rv = BCM_E_UNAVAIL;
    int index;

    sal_memset(&return_ment, 0, sizeof(mpls_entry_extd_entry_t));

    rv = soc_mem_search(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY, &index,
                        ment_extd, &return_ment, 0);

    if (rv == SOC_E_NONE) {
         BCM_IF_ERROR_RETURN(
              _bcm_tr3_mpls_extd_entry_update(unit, mpls_port, ment_extd, &return_ment));
         rv = soc_mem_write(unit, MPLS_ENTRY_EXTDm,
                                           MEM_BLOCK_ALL, index, &return_ment);
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    } else {
        rv = soc_mem_insert(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ALL, ment_extd);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_mpls_entry_convert_to_extd_entry_key
 * Purpose:
 *      convert the key of regular source mpls tables 
 *      to the key of extended destination mpls table
 * Parameters:
 *      unit                  - (IN) Device Number
 *      mpls_entry_entry      - (IN) Source table entry
 *      mpls_entry_extd_entry - (OUT)Destination table entry
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_mpls_entry_convert_to_extd_entry_key(
            int                     unit,
            mpls_entry_entry_t      *mpls_entry_entry,
            mpls_entry_extd_entry_t *mpls_entry_extd_entry)
{
    uint32 value;

    BCM_IF_ERROR_RETURN(_bcm_esw_mpls_entry_convert_to_extd_entry(unit,
                                  mpls_entry_entry, mpls_entry_extd_entry));

    value = soc_MPLS_ENTRYm_field32_get(unit, mpls_entry_entry, MPLS__MODULE_IDf);
    soc_MPLS_ENTRY_EXTDm_field32_set(unit, mpls_entry_extd_entry, MPLS__MODULE_IDf, value);

    value = soc_MPLS_ENTRYm_field32_get(unit, mpls_entry_entry, MPLS__PORT_NUMf);
    soc_MPLS_ENTRY_EXTDm_field32_set(unit, mpls_entry_extd_entry, MPLS__PORT_NUMf, value);

    value = soc_MPLS_ENTRYm_field32_get(unit, mpls_entry_entry, MPLS__Tf);
    soc_MPLS_ENTRY_EXTDm_field32_set(unit, mpls_entry_extd_entry, MPLS__Tf, value);

    value = soc_MPLS_ENTRYm_field32_get(unit, mpls_entry_entry, MPLS__TGIDf);
    soc_MPLS_ENTRY_EXTDm_field32_set(unit, mpls_entry_extd_entry, MPLS__TGIDf, value);

    value = soc_MPLS_ENTRYm_field32_get(unit, mpls_entry_entry, MPLS__MPLS_LABELf);
    soc_MPLS_ENTRY_EXTDm_field32_set(unit, mpls_entry_extd_entry, MPLS__MPLS_LABELf, value);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_mpls_entry_convert_to_extd_entry_all
 * Description:
 *      convert regular source mpls tables entry to extended destination mpls
 *      table entry(MPLS_ENTRYm and MPLS_ENTRY_EXTDm)
 *
 * Parameters:
 *      unit                  - (IN) unit number
 *      ment                  - (IN) Source table entry
 *      ment_extd             - (OUT) Destination table entry
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_esw_mpls_entry_convert_to_extd_entry_all(
            int                     unit,
            mpls_entry_entry_t      *ment,
            mpls_entry_extd_entry_t *ment_extd)
{
   uint32 value;

   BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_entry_convert_to_extd_entry_key(unit,
                                  ment, ment_extd));

   /* Take care about special fields */
   value = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__SOURCE_VPf);
   soc_MPLS_ENTRY_EXTDm_field32_set(unit, ment_extd, MPLS__SOURCE_VPf, value);

   value = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__CW_CHECK_CTRLf);
   soc_MPLS_ENTRY_EXTDm_field32_set(unit, ment_extd, MPLS__CW_CHECK_CTRLf, value);

   value = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__PW_CC_TYPEf);
   soc_MPLS_ENTRY_EXTDm_field32_set(unit, ment_extd, MPLS__PW_CC_TYPEf, value);

   value = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__MPLS_ACTION_IF_BOSf);
   soc_MPLS_ENTRY_EXTDm_field32_set(unit, ment_extd, MPLS__MPLS_ACTION_IF_BOSf, value);

   value = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__MPLS_ACTION_IF_NOT_BOSf);
   soc_MPLS_ENTRY_EXTDm_field32_set(unit, ment_extd, MPLS__MPLS_ACTION_IF_NOT_BOSf, value);

   value = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__DECAP_USE_EXP_FOR_PRIf);
   soc_MPLS_ENTRY_EXTDm_field32_set(unit, ment_extd, MPLS__DECAP_USE_EXP_FOR_PRIf, value);

   value = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__EXP_MAPPING_PTRf);
   soc_MPLS_ENTRY_EXTDm_field32_set(unit, ment_extd, MPLS__EXP_MAPPING_PTRf, value);

   value = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__NEW_PRIf);
   soc_MPLS_ENTRY_EXTDm_field32_set(unit, ment_extd, MPLS__NEW_PRIf, value);

   if (soc_mem_field_valid(unit, MPLS_ENTRYm, MPLS__TRUST_OUTER_DOT1P_PTRf) &&
       soc_mem_field_valid(unit, MPLS_ENTRY_EXTDm, MPLS__TRUST_OUTER_DOT1P_PTRf)) {
       value = soc_MPLS_ENTRY_EXTDm_field32_get(unit, ment,
                                          MPLS__TRUST_OUTER_DOT1P_PTRf);
       soc_MPLS_ENTRY_EXTDm_field32_set(unit, ment_extd, MPLS__TRUST_OUTER_DOT1P_PTRf, value);
   }

   return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_mpls_extd_entry_set
 * Purpose:
 *      Set MPLS extend entry
 * Parameters:
 *      unit      - (IN) Device Number
 *      ment      - (IN) MPLS entry
 *      mpls_port - (IN) mpls port information
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_mpls_extd_entry_set(int unit, mpls_entry_entry_t *ment, bcm_mpls_port_t *mpls_port)
{
    int new_pw_term_cntr_idx=-1;
    mpls_entry_extd_entry_t mpls_entry_extd_entry;

    sal_memset(&mpls_entry_extd_entry, 0, sizeof(mpls_entry_extd_entry_t));
    BCM_IF_ERROR_RETURN(_bcm_esw_mpls_entry_convert_to_extd_entry_all(unit, ment, &mpls_entry_extd_entry));

    BCM_IF_ERROR_RETURN(_bcm_tr_mpls_pw_term_counter_set(unit, &new_pw_term_cntr_idx));
    if (new_pw_term_cntr_idx != -1) {
           soc_MPLS_ENTRY_EXTDm_field32_set(unit, &mpls_entry_extd_entry, MPLS__PW_TERM_NUMf, new_pw_term_cntr_idx);
           soc_MPLS_ENTRY_EXTDm_field32_set(unit, &mpls_entry_extd_entry, MPLS__PW_TERM_NUM_VALIDf, 1);
    }

    return _bcm_tr3_mpls_extd_entry_add(unit, mpls_port, &mpls_entry_extd_entry);
}

/*
 * Function:
 *      _bcm_tr3_mpls_match_add
 * Purpose:
 *      Assign match criteria of an MPLS port
 * Parameters:
 *      unit    - (IN) Device Number
 *      mpls_port - (IN) mpls port information
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_tr3_mpls_match_add(int unit, bcm_mpls_port_t *mpls_port, int vp,
                        int hw_update)
{
    _bcm_tr_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);
    int rv = BCM_E_NONE,  gport_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    vlan_xlate_extd_entry_t vent;
    int    mod_id_idx=0; /* Module Id */
    int    src_trk_idx=0;  /*Source Trunk table index.*/
    uint32 key_type_1 = 0x10;  /* MPLS_ENTRY Key Tyepe */
    int vpless_failover_port = FALSE;
    int old_entry_found  = BCM_E_NONE;  
    if (_BCM_MPLS_GPORT_FAILOVER_VPLESS_GET(vp)) {
        if (mpls_port->criteria & BCM_MPLS_PORT_MATCH_LABEL) {
            /* should use same entry as primary */
            return BCM_E_NONE;
        }
        vpless_failover_port = TRUE;
        vp = _BCM_MPLS_GPORT_FAILOVER_VPLESS_CLEAR(vp);
    }

    sal_memset(&vent, 0, sizeof(vlan_xlate_extd_entry_t));

    if (mpls_port->criteria != BCM_MPLS_PORT_MATCH_LABEL) {
         rv = _bcm_esw_gport_resolve(unit, mpls_port->port, &mod_out, 
                                &port_out, &trunk_id, &gport_id);
         BCM_IF_ERROR_RETURN(rv);
    }

    /* Get module id for unit. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &mod_id_idx));

    if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_VLAN ) {

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MPLS_ACTIONf, 0x1); /* SVP */
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__DISABLE_VLAN_CHECKSf, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__SOURCE_VPf, vp);
        if (!BCM_VLAN_VALID(mpls_port->match_vlan)) {
             return BCM_E_PARAM;
        }
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f, 
                                        TR3_VLXLT_X_HASH_KEY_TYPE_OVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f, 
                                        TR3_VLXLT_X_HASH_KEY_TYPE_OVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, OVIDf, 
                                        mpls_port->match_vlan);
        mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_VLAN;
        mpls_info->match_key[vp].match_vlan = mpls_port->match_vlan;

        if (BCM_GPORT_IS_TRUNK(mpls_port->port)) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf, trunk_id);
            mpls_info->match_key[vp].trunk_id = trunk_id;
        } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf, mod_out);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf, port_out);
            mpls_info->match_key[vp].port = port_out;
            mpls_info->match_key[vp].modid = mod_out;
        }
        rv = _bcm_tr3_mpls_match_vlan_extd_entry_set(unit, mpls_port, &vent);
        BCM_IF_ERROR_RETURN(rv);
        if (!(mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
              bcm_tr_mpls_port_match_count_adjust(unit, vp, 1);
        }
    } else if (mpls_port->criteria ==
                            BCM_MPLS_PORT_MATCH_PORT_INNER_VLAN) {

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MPLS_ACTIONf, 0x1); /* SVP */
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__DISABLE_VLAN_CHECKSf, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__SOURCE_VPf, vp);
        if (!BCM_VLAN_VALID(mpls_port->match_inner_vlan)) {
            return BCM_E_PARAM;
        }
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f,
                                    TR3_VLXLT_X_HASH_KEY_TYPE_IVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f,
                                    TR3_VLXLT_X_HASH_KEY_TYPE_IVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__IVIDf,
                                    mpls_port->match_inner_vlan);
        mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_INNER_VLAN;
        mpls_info->match_key[vp].match_inner_vlan = mpls_port->match_inner_vlan;

        if (BCM_GPORT_IS_TRUNK(mpls_port->port)) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf, trunk_id);
            mpls_info->match_key[vp].trunk_id = trunk_id;
        } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf, mod_out);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf, port_out);
            mpls_info->match_key[vp].port = port_out;
            mpls_info->match_key[vp].modid = mod_out;
        }
        rv = _bcm_tr3_mpls_match_vlan_extd_entry_set(unit, mpls_port, &vent);
        BCM_IF_ERROR_RETURN(rv);
        if (!(mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
             bcm_tr_mpls_port_match_count_adjust(unit, vp, 1);
        }
    } else if (mpls_port->criteria == 
                            BCM_MPLS_PORT_MATCH_PORT_VLAN_STACKED) {

         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MPLS_ACTIONf, 0x1); /* SVP */
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__DISABLE_VLAN_CHECKSf, 1);
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__SOURCE_VPf, vp);
         if (!BCM_VLAN_VALID(mpls_port->match_vlan) || 
                !BCM_VLAN_VALID(mpls_port->match_inner_vlan)) {
              return BCM_E_PARAM;
         }
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f, 
                   TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID);
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f, 
                   TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID);
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__OVIDf, 
                   mpls_port->match_vlan);
         soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__IVIDf, 
                   mpls_port->match_inner_vlan);
         mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_VLAN_STACKED;
         mpls_info->match_key[vp].match_vlan = mpls_port->match_vlan;
         mpls_info->match_key[vp].match_inner_vlan = mpls_port->match_inner_vlan;
         if (BCM_GPORT_IS_TRUNK(mpls_port->port)) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf, trunk_id);
            mpls_info->match_key[vp].trunk_id = trunk_id;
         } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf, mod_out);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf, port_out);
            mpls_info->match_key[vp].port = port_out;
            mpls_info->match_key[vp].modid = mod_out;
         }
         rv = _bcm_tr3_mpls_match_vlan_extd_entry_set(unit, mpls_port, &vent);
         BCM_IF_ERROR_RETURN(rv);
         if (!(mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
             bcm_tr_mpls_port_match_count_adjust(unit, vp, 1);
         }
    } else if (mpls_port->criteria ==
                                            BCM_MPLS_PORT_MATCH_VLAN_PRI) {

              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MPLS_ACTIONf, 0x1); /* SVP */
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__DISABLE_VLAN_CHECKSf, 1);
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__SOURCE_VPf, vp);
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f, 
                                                 TR3_VLXLT_X_HASH_KEY_TYPE_PRI_CFI);
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f, 
                                                 TR3_VLXLT_X_HASH_KEY_TYPE_PRI_CFI);
              /* match_vlan : Bits 12-15 contains VLAN_PRI + CFI, vlan=BCM_E_NONE */
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, OTAGf, 
                                                 mpls_port->match_vlan);
              mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_VLAN_PRI;
              mpls_info->match_key[vp].match_vlan = mpls_port->match_vlan;

              if (BCM_GPORT_IS_TRUNK(mpls_port->port)) {
                   soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
                   soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf, trunk_id);
                        mpls_info->match_key[vp].trunk_id = trunk_id;
              } else {
                   soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf, mod_out);
                   soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf, port_out);
                   mpls_info->match_key[vp].port = port_out;
                   mpls_info->match_key[vp].modid = mod_out;
              }
              rv = _bcm_tr3_mpls_match_vlan_extd_entry_set(unit, mpls_port, &vent);
              BCM_IF_ERROR_RETURN(rv);
              if (!(mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
                   bcm_tr_mpls_port_match_count_adjust(unit, vp, 1);
         }
    } else if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_VLAN_TAG) {

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MPLS_ACTIONf, 0x1); /* SVP */
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__DISABLE_VLAN_CHECKSf, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__SOURCE_VPf, vp);

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f,
                                         TR3_VLXLT_X_HASH_KEY_TYPE_OTAG);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f,
                                         TR3_VLXLT_X_HASH_KEY_TYPE_OTAG);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__OTAGf,
                                         mpls_port->match_vlan);
        mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_VLAN_TAG;
        mpls_info->match_key[vp].match_vlan = mpls_port->match_vlan;

        if (BCM_GPORT_IS_TRUNK(mpls_port->port)) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf, trunk_id);
            mpls_info->match_key[vp].trunk_id = trunk_id;
        } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf, mod_out);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf, port_out);
            mpls_info->match_key[vp].port = port_out;
            mpls_info->match_key[vp].modid = mod_out;
        }
        rv = _bcm_tr3_mpls_match_vlan_extd_entry_set(unit, mpls_port, &vent);
        BCM_IF_ERROR_RETURN(rv);
        if (!(mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
            bcm_tr_mpls_port_match_count_adjust(unit, vp, 1);
        }

    } else if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_INNER_VLAN_TAG) {

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MPLS_ACTIONf, 0x1); /* SVP */
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__DISABLE_VLAN_CHECKSf, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__SOURCE_VPf, vp);

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f,
                                         TR3_VLXLT_X_HASH_KEY_TYPE_ITAG);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f,
                                         TR3_VLXLT_X_HASH_KEY_TYPE_ITAG);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__ITAGf,
                                         mpls_port->match_inner_vlan);
        mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_INNER_VLAN_TAG;
        mpls_info->match_key[vp].match_inner_vlan = mpls_port->match_inner_vlan;

        if (BCM_GPORT_IS_TRUNK(mpls_port->port)) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf, trunk_id);
            mpls_info->match_key[vp].trunk_id = trunk_id;
        } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf, mod_out);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf, port_out);
            mpls_info->match_key[vp].port = port_out;
            mpls_info->match_key[vp].modid = mod_out;
        }
        rv = _bcm_tr3_mpls_match_vlan_extd_entry_set(unit, mpls_port, &vent);
        BCM_IF_ERROR_RETURN(rv);
        if (!(mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
            bcm_tr_mpls_port_match_count_adjust(unit, vp, 1);
        }

    } else if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT) {
        if (BCM_GPORT_IS_TRUNK(mpls_port->port)) {
             rv = bcm_tr_mpls_match_trunk_add(unit, trunk_id, vp);
             if (rv >= 0) {
                   mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_TRUNK;
                   mpls_info->match_key[vp].trunk_id = trunk_id;
             }
             BCM_IF_ERROR_RETURN(rv);       
        } else {
            source_trunk_map_modbase_entry_t modbase_entry;
            int is_local;

            BCM_IF_ERROR_RETURN
                ( _bcm_esw_modid_is_local(unit, mod_out, &is_local));

            if ((!is_local) && (mod_out != BCM_MODID_INVALID)) {
                mod_id_idx = mod_out;
            }

            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, SOURCE_TRUNK_MAP_MODBASEm, MEM_BLOCK_ANY,
                                                 mod_id_idx, &modbase_entry));
            src_trk_idx =  soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_MODBASEm,
                                                 &modbase_entry, BASEf) + port_out;
            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm, 
                                        src_trk_idx, SOURCE_VPf, vp);
            BCM_IF_ERROR_RETURN(rv);

            if (is_local) {
                /* Convert system ports to physical ports */ 
                if (soc_feature(unit, soc_feature_sysport_remap)) { 
                    BCM_XLATE_SYSPORT_S2P(unit, &port_out); 
                }

                rv = soc_mem_field32_modify(unit, PORT_TABm, port_out,
                                        PORT_OPERATIONf, 0x1); /* L2_SVP */
                BCM_IF_ERROR_RETURN(rv);

                /* Set TAG_ACTION_PROFILE_PTR */
                rv = bcm_tr_mpls_port_untagged_profile_set(unit, port_out);
                BCM_IF_ERROR_RETURN(rv);
            }

            mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_PORT;
            mpls_info->match_key[vp].index = src_trk_idx;
        }
    }else if ((mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL) ||
               (mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL_PORT)) {

        mpls_entry_entry_t ment;
        bcm_mpls_label_t old_match_label;
        int entry_index=-1;
        soc_mem_t mem = MPLS_ENTRYm;
        uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS];

        sal_memset(&ment, 0, sizeof(mpls_entry_entry_t));
        sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

        if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL_PORT) {
            /* Check for Port_independent Label mapping */
            rv = bcm_tr_mpls_port_independent_range (unit, mpls_port->match_label, mpls_port->port);
            if (rv < 0) {
                return rv;
            }
            if (BCM_GPORT_IS_TRUNK(mpls_port->port)) {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__Tf, 1);
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__TGIDf, trunk_id);
                if (vpless_failover_port == FALSE) {
                    mpls_info->match_key[vp].trunk_id = trunk_id;
                } else {
                    mpls_info->match_key[vp].fo_trunk_id = trunk_id;
                }
            } else {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MODULE_IDf, mod_out);
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__PORT_NUMf, port_out);
                if (vpless_failover_port == FALSE) {
                    mpls_info->match_key[vp].port = port_out;
                    mpls_info->match_key[vp].modid = mod_out;
                } else {
                    mpls_info->match_key[vp].fo_port = port_out;
                    mpls_info->match_key[vp].fo_modid = mod_out;
                }
            }
            mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_LABEL_PORT;

        } else if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MODULE_IDf, 0);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__PORT_NUMf, 0);
            mpls_info->match_key[vp].port = 0;
            mpls_info->match_key[vp].modid = 0;
            mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_LABEL;
        }

        /* Use old label to get old mpls entry for replace operation */
        old_match_label = mpls_info->match_key[vp].match_label;
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_LABELf, 
                                    old_match_label);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, VALIDf, 1);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__SOURCE_VPf, vp);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_ACTION_IF_BOSf, 
                                    0x1); /* L2 SVP */
        if (SOC_IS_TRIUMPH3(unit)) {
            if (mpls_port->flags & BCM_MPLS_PORT2_WITH_LSP_OAM) {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                            0x1); /* POP and forward based on the next label lookup */
            } else {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                            0x0); /* INVALID */
            }
        } else {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                        0x0); /* INVALID */
        }
        soc_MPLS_ENTRYm_field32_set(unit, &ment, KEY_TYPEf, key_type_1);

        if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
            /* Find Entry within Hardware */
            old_entry_found = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &entry_index,
                              &ment, hw_buf, 0);

            if (old_entry_found == SOC_E_NOT_FOUND) {
                mpls_entry_extd_entry_t mpls_entry_extd_entry;

                mem = MPLS_ENTRY_EXTDm;
                sal_memset(&mpls_entry_extd_entry, 0, sizeof(mpls_entry_extd_entry_t));

                BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_entry_convert_to_extd_entry_key(unit, &ment,
                          &mpls_entry_extd_entry));
                old_entry_found = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &entry_index,
                              &mpls_entry_extd_entry, hw_buf, 0);
            }
        }

        /* Add the new MPLS label for the new entry*/
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_LABELf,
                                    mpls_port->match_label);

        if (mpls_port->flags & BCM_MPLS_PORT_SEQUENCED) {
            /* Control word is present, check sequence number */
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__CW_CHECK_CTRLf, 
                                        0x3); /* CW_CHECK_LOOSE */
            

        } else if (mpls_port->flags & BCM_MPLS_PORT_CONTROL_WORD) {
            /* Control word is present, but no sequence number check */
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__CW_CHECK_CTRLf, 
                                        0x1); /* CW_NO_CHECK */
        }

        /* VCCV defines 3 control-channels */
        if (soc_mem_field_valid(unit, MPLS_ENTRYm, MPLS__PW_CC_TYPEf)) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__PW_CC_TYPEf, 
                                       mpls_port->vccv_type);
        }

        (void) bcm_tr3_mpls_entry_internal_qos_set(unit, mpls_port, NULL, &ment);

        if (soc_feature(unit, soc_feature_mpls_entropy)) {
            if ((mpls_port->flags & BCM_MPLS_PORT_ENTROPY_ENABLE) ||
                (mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ENTROPY_ENABLE)) {
                 soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_ACTION_IF_BOSf, 
                                0x0); /* INVALID */
                 soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_ACTION_IF_NOT_BOSf, 
                                0x2); /* POP_SVP_ENTROPY */
            }
        }

        /* Add the new MPLS entry */
        if (hw_update) {
            if (mpls_port->flags & BCM_MPLS_PORT_SEQUENCED) {
                rv = _bcm_tr3_mpls_extd_entry_set(unit, &ment, mpls_port);
            } else {
                rv = _bcm_tr3_mpls_match_label_entry_set(unit, mpls_port, &ment);
            }
        }

        /* update the new label in match key of the VP*/
        if (BCM_SUCCESS(rv)) {
            mpls_info->match_key[vp].match_label = mpls_port->match_label;
        }

        /* Delete the old MPLS entry */
        if (BCM_SUCCESS(rv) && (mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
            int failover_vp = -1;
            bcm_mpls_label_t failover_match_label = -1;

            failover_vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->failover_port_id);
            if ((failover_vp >= 0) &&
                _bcm_vp_used_get(unit, failover_vp, _bcmVpTypeMpls)) {
                failover_match_label = mpls_info->match_key[failover_vp].match_label;
            }

            if ((old_entry_found == BCM_E_NONE) &&
                (old_match_label != mpls_port->match_label)){
                if ((failover_match_label != -1) &&
                   (old_match_label == failover_match_label)) {
                    if (soc_mem_field_valid(unit, mem, MPLS__SOURCE_VPf)) {
                        soc_mem_field32_set(unit, mem, hw_buf, MPLS__SOURCE_VPf, failover_vp);
                    }
                    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem,
                                           MEM_BLOCK_ALL, entry_index,
                                           hw_buf));
                } else {
                    BCM_IF_ERROR_RETURN(soc_mem_delete(unit, mem,
                        MEM_BLOCK_ANY, hw_buf));
                }
            }
        }
    } else if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_NONE) {

        if (!(mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
            bcm_tr_mpls_port_match_count_adjust(unit, vp, 1);
        }

        /* If the current match criteria is BCM_MPLS_PORT_MATCH_PORT,
           clear up the existing related info from HW */
        if (mpls_info->match_key[vp].flags == _BCM_MPLS_PORT_MATCH_TYPE_PORT) {

            if (BCM_GPORT_IS_TRUNK(mpls_port->port)) {
                 rv = bcm_tr_mpls_match_trunk_delete(unit, trunk_id, vp);
                 BCM_IF_ERROR_RETURN(rv);
            } else {

                source_trunk_map_modbase_entry_t modbase_entry;
                int is_local;

                BCM_IF_ERROR_RETURN
                    ( _bcm_esw_modid_is_local(unit, mod_out, &is_local));

                if ((!is_local) && (mod_out != BCM_MODID_INVALID)) {
                    mod_id_idx = mod_out;
                }

                BCM_IF_ERROR_RETURN
                    (soc_mem_read(unit, SOURCE_TRUNK_MAP_MODBASEm, MEM_BLOCK_ANY,
                                                     mod_id_idx, &modbase_entry));
                src_trk_idx =  soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_MODBASEm,
                                                     &modbase_entry, BASEf) + port_out;
                rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                            src_trk_idx, SOURCE_VPf, 0);
                BCM_IF_ERROR_RETURN(rv);

                if (is_local) {
                    /* Convert system ports to physical ports */
                    if (soc_feature(unit, soc_feature_sysport_remap)) {
                        BCM_XLATE_SYSPORT_S2P(unit, &port_out);
                    }

                    rv = soc_mem_field32_modify(unit, PORT_TABm, port_out,
                                            PORT_OPERATIONf, 0x0);
                    BCM_IF_ERROR_RETURN(rv);

                    /* Set TAG_ACTION_PROFILE_PTR */
                    rv = bcm_tr_mpls_port_untagged_profile_reset(unit, port_out);
                    BCM_IF_ERROR_RETURN(rv);
                }

            }
        }

        /* Clear out the current vp info */
        (void) bcm_tr_mpls_match_clear (unit, vp);
        mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_NONE;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_mpls_match_del
 * Purpose:
 *      Delete match criteria of an MPLS port
 * Parameters:
 *      unit    - (IN) Device Number
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_tr3_mpls_match_delete(int unit, int vp, int sw_clear)
{
    _bcm_tr_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);
    int rv=BCM_E_NONE;
    vlan_xlate_extd_entry_t vent;
    bcm_trunk_t trunk_id;
    int    src_trk_idx=0;   /*Source Trunk table index.*/
    int    mod_id_idx=0;   /* Module_Id */
    int port;
    uint32   key_type = 0x10; 
    int vpless_failover_port = FALSE;

    if (_BCM_MPLS_GPORT_FAILOVER_VPLESS_GET(vp)) {
        vp = _BCM_MPLS_GPORT_FAILOVER_VPLESS_CLEAR(vp);
        if (mpls_info->match_key[vp].flags & _BCM_MPLS_PORT_MATCH_TYPE_LABEL) {
            /* label entry belongs to primary port */
            return BCM_E_NONE;
        }
        vpless_failover_port = TRUE;
    }
    sal_memset(&vent, 0, sizeof(vlan_xlate_extd_entry_t));


     if (mpls_info->match_key[vp].flags == _BCM_MPLS_PORT_MATCH_TYPE_NONE) {
        (void) bcm_tr_mpls_match_clear (unit, vp);
         bcm_tr_mpls_port_match_count_adjust(unit, vp, -1);
     } else if  (mpls_info->match_key[vp].flags == _BCM_MPLS_PORT_MATCH_TYPE_VLAN) {

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f, 
                                    TR3_VLXLT_X_HASH_KEY_TYPE_OVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f, 
                                    TR3_VLXLT_X_HASH_KEY_TYPE_OVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__OVIDf,
                                    mpls_info->match_key[vp].match_vlan);
        if (mpls_info->match_key[vp].modid != -1) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf, 
                                        mpls_info->match_key[vp].modid);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf, 
                                        mpls_info->match_key[vp].port);
        } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf, 
                                        mpls_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);
        
        (void) bcm_tr_mpls_match_clear (unit, vp);
         bcm_tr_mpls_port_match_count_adjust(unit, vp, -1);

    } else if  (mpls_info->match_key[vp].flags == 
                     _BCM_MPLS_PORT_MATCH_TYPE_INNER_VLAN) {

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f, 
                                    TR3_VLXLT_X_HASH_KEY_TYPE_IVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f, 
                                    TR3_VLXLT_X_HASH_KEY_TYPE_IVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__IVIDf,
                                    mpls_info->match_key[vp].match_inner_vlan);
        if (mpls_info->match_key[vp].modid != -1) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf, 
                                        mpls_info->match_key[vp].modid);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf, 
                                        mpls_info->match_key[vp].port);
        } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf, 
                                        mpls_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);
        
        (void) bcm_tr_mpls_match_clear (unit, vp);
        bcm_tr_mpls_port_match_count_adjust(unit, vp, -1);
    }else if (mpls_info->match_key[vp].flags == 
                    _BCM_MPLS_PORT_MATCH_TYPE_VLAN_STACKED) {

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f, 
                                    TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f, 
                                    TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__OVIDf,
                                    mpls_info->match_key[vp].match_vlan);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__IVIDf,
                              mpls_info->match_key[vp].match_inner_vlan);
        if (mpls_info->match_key[vp].modid != -1) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf, 
                                        mpls_info->match_key[vp].modid);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf, 
                                        mpls_info->match_key[vp].port);
        } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf, 
                                        mpls_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);

        (void) bcm_tr_mpls_match_clear (unit, vp);
        bcm_tr_mpls_port_match_count_adjust(unit, vp, -1);

    } else if (mpls_info->match_key[vp].flags ==
                                  _BCM_MPLS_PORT_MATCH_TYPE_VLAN_PRI) {

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f, 
                                                      TR3_VLXLT_X_HASH_KEY_TYPE_PRI_CFI);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f, 
                                                      TR3_VLXLT_X_HASH_KEY_TYPE_PRI_CFI);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__OTAGf,
                                                       mpls_info->match_key[vp].match_vlan);
        if (mpls_info->match_key[vp].modid != -1) {
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf, 
                                                      mpls_info->match_key[vp].modid);
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf, 
                                                      mpls_info->match_key[vp].port);
        } else {
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
              soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf, 
                                                      mpls_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);

        (void) bcm_tr_mpls_match_clear (unit, vp);
        bcm_tr_mpls_port_match_count_adjust(unit, vp, -1);
    } else if (mpls_info->match_key[vp].flags == 
                                  _BCM_MPLS_PORT_MATCH_TYPE_VLAN_TAG) {

        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f,
                TR3_VLXLT_X_HASH_KEY_TYPE_OTAG);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f,
                TR3_VLXLT_X_HASH_KEY_TYPE_OTAG);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__OTAGf,
                mpls_info->match_key[vp].match_vlan);
        if (mpls_info->match_key[vp].modid != -1) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf,
                    mpls_info->match_key[vp].modid);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf,
                    mpls_info->match_key[vp].port);
        } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf,
                    mpls_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);

        (void) bcm_tr_mpls_match_clear (unit, vp);
        bcm_tr_mpls_port_match_count_adjust(unit, vp, -1);

    } else if (mpls_info->match_key[vp].flags ==
                                    _BCM_MPLS_PORT_MATCH_TYPE_INNER_VLAN_TAG) {
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_0f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, VALID_1f, 1);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_0f,
                TR3_VLXLT_X_HASH_KEY_TYPE_ITAG);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, KEY_TYPE_1f,
                TR3_VLXLT_X_HASH_KEY_TYPE_ITAG);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__ITAGf,
                mpls_info->match_key[vp].match_inner_vlan);
        if (mpls_info->match_key[vp].modid != -1) {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__MODULE_IDf,
                    mpls_info->match_key[vp].modid);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__PORT_NUMf,
                    mpls_info->match_key[vp].port);
        } else {
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__Tf, 1);
            soc_VLAN_XLATE_EXTDm_field32_set(unit, &vent, XLATE__TGIDf,
                    mpls_info->match_key[vp].trunk_id);
        }
        rv = soc_mem_delete(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);

        (void) bcm_tr_mpls_match_clear (unit, vp);
        bcm_tr_mpls_port_match_count_adjust(unit, vp, -1);

    } else if  (mpls_info->match_key[vp].flags == 
                    _BCM_MPLS_PORT_MATCH_TYPE_PORT) {
         int is_local;
 
         src_trk_idx = mpls_info->match_key[vp].index;
         rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm, 
                                  src_trk_idx, SOURCE_VPf, 0);
         BCM_IF_ERROR_RETURN(rv);

         BCM_IF_ERROR_RETURN(_bcm_esw_src_modid_port_get( unit, src_trk_idx,
                                                          &mod_id_idx, &port));
         BCM_IF_ERROR_RETURN
             ( _bcm_esw_modid_is_local(unit, mod_id_idx, &is_local));
 
         if (is_local) {
             rv = soc_mem_field32_modify(unit, PORT_TABm, port,
                                           PORT_OPERATIONf, 0x0); /* NORMAL */
             BCM_IF_ERROR_RETURN(rv);

             /* Reset TAG_ACTION_PROFILE_PTR */
             rv = bcm_tr_mpls_port_untagged_profile_reset(unit, port);
             BCM_IF_ERROR_RETURN(rv);
         }
    } else if  (mpls_info->match_key[vp].flags == 
                  _BCM_MPLS_PORT_MATCH_TYPE_TRUNK) {
         trunk_id = mpls_info->match_key[vp].trunk_id;
         rv = bcm_tr_mpls_match_trunk_delete(unit, trunk_id, vp);
         BCM_IF_ERROR_RETURN(rv);

         (void) bcm_tr_mpls_match_clear (unit, vp);
    }else if  ((mpls_info->match_key[vp].flags &
                     _BCM_MPLS_PORT_MATCH_TYPE_LABEL) ||
                   (mpls_info->match_key[vp].flags ==
                     _BCM_MPLS_PORT_MATCH_TYPE_LABEL_PORT)) {
        mpls_entry_entry_t ment;
        soc_mem_t mem = MPLS_ENTRYm;
        uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS];

        sal_memset(&ment, 0, sizeof(mpls_entry_entry_t));
        sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

        if (vpless_failover_port) {
            if (mpls_info->match_key[vp].fo_modid != -1) {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MODULE_IDf,
                                       mpls_info->match_key[vp].fo_modid);
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__PORT_NUMf,
                                       mpls_info->match_key[vp].fo_port);
            } else {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__Tf, 1);
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__TGIDf,
                                       mpls_info->match_key[vp].fo_trunk_id);
            }
        } else {
            if (mpls_info->match_key[vp].modid != -1) {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MODULE_IDf, 
                                        mpls_info->match_key[vp].modid);
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__PORT_NUMf, 
                                        mpls_info->match_key[vp].port);
            } else {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__Tf, 1);
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__TGIDf, 
                                        mpls_info->match_key[vp].trunk_id);
            }
        }
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_LABELf,
                                    mpls_info->match_key[vp].match_label);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, KEY_TYPEf, key_type);

        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &src_trk_idx,
                                    &ment, hw_buf, 0);

        if (rv == SOC_E_NOT_FOUND) {
             mpls_entry_extd_entry_t ment_extd;

             mem = MPLS_ENTRY_EXTDm;
             sal_memset(&ment_extd, 0, sizeof(mpls_entry_extd_entry_t));
             BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_entry_convert_to_extd_entry_key(unit,
                                                       &ment, &ment_extd));
             rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY,
                                       &src_trk_idx, &ment_extd, hw_buf, 0);
        }

        if (rv == SOC_E_NONE) {
            rv =  soc_mem_delete(unit, mem, MEM_BLOCK_ANY, hw_buf);
            BCM_IF_ERROR_RETURN(rv);
            if (sw_clear) {
                if (vpless_failover_port) {
                    mpls_info->match_key[vp].fo_trunk_id = -1;
                    mpls_info->match_key[vp].fo_port = 0;
                    mpls_info->match_key[vp].fo_modid = -1;
                } else {
                    if (mpls_info->match_key[vp].flags &
                        _BCM_MPLS_PORT_MATCH_TYPE_NONE) {
                        bcm_tr_mpls_port_match_count_adjust(unit, vp, -1);
                    }
                    (void) bcm_tr_mpls_match_clear (unit, vp);
                }
            }
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_mpls_failover_nw_port_match_get
 * Purpose:
 *      Get match criteria of an MPLS port
 * Parameters:
 *      unit    - (IN) Device Number
 *      mpls_port - (IN) mpls port information
 *      vp  - (IN) Source Virtual Port
 *      return_ment - (OUT) matched entry
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_mpls_failover_nw_port_match_get(int unit, bcm_mpls_port_t *mpls_port,
                                    int vp, mpls_entry_entry_t *return_ment)
{

    _bcm_tr_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);
    mpls_entry_entry_t ment;
    int entry_index = -1;
    int key_type = 0x10; /* mpls_key_type */
    int rv = BCM_E_NONE;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    bcm_gport_t gport_id;

    /* return not found if both inputs are invalid. */
    if ((mpls_port == NULL) && (vp == -1)) {
        return BCM_E_NOT_FOUND;
    }

    sal_memset(&ment, 0, sizeof(mpls_entry_entry_t));
    if (mpls_port != NULL) {
        if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL_PORT) {
            /* Check for Port_independent Label mapping */
            rv = bcm_tr_mpls_port_independent_range (unit, mpls_port->match_label, mpls_port->port);
            if (rv < 0) {
                return rv;
            }

            rv = _bcm_esw_gport_resolve(unit, mpls_port->port, &mod_out,
                                &port_out, &trunk_id, &gport_id);
             BCM_IF_ERROR_RETURN(rv);

            if (BCM_GPORT_IS_TRUNK(mpls_port->port)) {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__Tf, 1);
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__TGIDf, mpls_port->port);
            } else {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MODULE_IDf, mod_out);
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__PORT_NUMf, port_out);
            }

        } else if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MODULE_IDf, 0);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__PORT_NUMf, 0);
        } else {
            return BCM_E_NOT_FOUND;
        }

        /* Use old label to get old mpls entry for replace operation */
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_LABELf,
                                    mpls_port->match_label);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, VALIDf, 1);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_ACTION_IF_BOSf,
                                    0x1); /* L2 SVP */
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                    0x0); /* INVALID */
        soc_MPLS_ENTRYm_field32_set(unit, &ment, KEY_TYPEf, key_type);
    } else {

        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
            return BCM_E_NOT_FOUND;
        }

        if ((mpls_info->match_key[vp].flags &
                _BCM_MPLS_PORT_MATCH_TYPE_LABEL) ||
               (mpls_info->match_key[vp].flags ==
                _BCM_MPLS_PORT_MATCH_TYPE_LABEL_PORT)) {

        if (mpls_info->match_key[vp].flags &
                    _BCM_MPLS_PORT_MATCH_TYPE_LABEL_PORT) {

            if (mpls_info->match_key[vp].trunk_id != -1) {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__Tf, 1);
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__TGIDf,
                    mpls_info->match_key[vp].trunk_id);
            } else {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MODULE_IDf,
                    mpls_info->match_key[vp].modid);
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__PORT_NUMf,
                    mpls_info->match_key[vp].port);
            }
        }

        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_LABELf,
            mpls_info->match_key[vp].match_label);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, KEY_TYPEf, key_type);

        } else {
            return BCM_E_NOT_FOUND;
        }
    }

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &entry_index,
             &ment, return_ment, 0);

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_mpls_match_get
 * Purpose:
 *      Get match criteria of an MPLS port
 * Parameters:
 *      unit    - (IN) Device Number
 *      mpls_port - (OUT) mpls port information
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_tr3_mpls_match_get(int unit, bcm_mpls_port_t *mpls_port, int vp)
{
    _bcm_tr_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);
    int rv = BCM_E_NONE;
    bcm_module_t mod_in=0, mod_out;
    bcm_port_t port_in=0, port_out;
    bcm_trunk_t trunk_id=0;
    int    src_trk_idx=0;    /*Source Trunk table index.*/
    int    mode_in=0;   /* Module_Id */
    uint32   key_type = 0x10;
    int vpless_failover_port = FALSE; /* the failover port without a dedicated vp */
    int match_trunk_id;
    int match_modid;
    int match_port;
    
    if (_BCM_MPLS_GPORT_FAILOVER_VPLESS_GET(vp)) {
        
        vp = _BCM_MPLS_GPORT_FAILOVER_VPLESS_CLEAR(vp);
        if (!(mpls_info->match_key[vp].flags & _BCM_MPLS_PORT_MATCH_TYPE_LABEL)) {
            vpless_failover_port = TRUE;
        } 
    }
    
    if (vpless_failover_port == FALSE) {
        match_trunk_id = mpls_info->match_key[vp].trunk_id;
        match_modid    = mpls_info->match_key[vp].modid;
        match_port     = mpls_info->match_key[vp].port;
    } else {
        match_trunk_id = mpls_info->match_key[vp].fo_trunk_id;
        match_modid    = mpls_info->match_key[vp].fo_modid;
        match_port     = mpls_info->match_key[vp].fo_port;
    }

    /* Get module id for unit. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &mode_in));

    if  (mpls_info->match_key[vp].flags &  _BCM_MPLS_PORT_MATCH_TYPE_VLAN) {

         mpls_port->criteria = BCM_MPLS_PORT_MATCH_PORT_VLAN;
         mpls_port->match_vlan = mpls_info->match_key[vp].match_vlan;

        if (mpls_info->match_key[vp].trunk_id != -1) {
             trunk_id = mpls_info->match_key[vp].trunk_id;
             BCM_GPORT_TRUNK_SET(mpls_port->port, trunk_id);
        } else {
             port_in = mpls_info->match_key[vp].port;
             mod_in = mpls_info->match_key[vp].modid;
             rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                  mod_in, port_in, &mod_out, &port_out);
             BCM_GPORT_MODPORT_SET(mpls_port->port, mod_out, port_out);
        }
    } else if (mpls_info->match_key[vp].flags &  
                 _BCM_MPLS_PORT_MATCH_TYPE_INNER_VLAN) {
         mpls_port->criteria = BCM_MPLS_PORT_MATCH_PORT_INNER_VLAN;
         mpls_port->match_inner_vlan = mpls_info->match_key[vp].match_inner_vlan;

        if (mpls_info->match_key[vp].trunk_id != -1) {
             trunk_id = mpls_info->match_key[vp].trunk_id;
             BCM_GPORT_TRUNK_SET(mpls_port->port, trunk_id);
        } else {
             port_in = mpls_info->match_key[vp].port;
             mod_in = mpls_info->match_key[vp].modid;
             rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                  mod_in, port_in, &mod_out, &port_out);
             BCM_GPORT_MODPORT_SET(mpls_port->port, mod_out, port_out);
        }
    }else if (mpls_info->match_key[vp].flags &
                    _BCM_MPLS_PORT_MATCH_TYPE_VLAN_STACKED) {

         mpls_port->criteria = BCM_MPLS_PORT_MATCH_PORT_VLAN_STACKED;
         mpls_port->match_vlan = mpls_info->match_key[vp].match_vlan;
         mpls_port->match_inner_vlan = mpls_info->match_key[vp].match_inner_vlan;

         if (mpls_info->match_key[vp].trunk_id != -1) {
              trunk_id = mpls_info->match_key[vp].trunk_id;
              BCM_GPORT_TRUNK_SET(mpls_port->port, trunk_id);
         } else {
              port_in = mpls_info->match_key[vp].port;
              mod_in = mpls_info->match_key[vp].modid;
              rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                             mod_in, port_in, &mod_out, &port_out);
              BCM_GPORT_MODPORT_SET(mpls_port->port, mod_out, port_out);
         }
    } else if  (mpls_info->match_key[vp].flags &  _BCM_MPLS_PORT_MATCH_TYPE_VLAN_PRI) {

         mpls_port->criteria = BCM_MPLS_PORT_MATCH_VLAN_PRI;
         mpls_port->match_vlan = mpls_info->match_key[vp].match_vlan;

        if (mpls_info->match_key[vp].trunk_id != -1) {
             trunk_id = mpls_info->match_key[vp].trunk_id;
             BCM_GPORT_TRUNK_SET(mpls_port->port, trunk_id);
        } else {
             port_in = mpls_info->match_key[vp].port;
             mod_in = mpls_info->match_key[vp].modid;
             rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                  mod_in, port_in, &mod_out, &port_out);
             BCM_GPORT_MODPORT_SET(mpls_port->port, mod_out, port_out);
        }
    } else if (mpls_info->match_key[vp].flags &  
                                _BCM_MPLS_PORT_MATCH_TYPE_VLAN_TAG) {
 
        mpls_port->criteria = BCM_MPLS_PORT_MATCH_PORT_VLAN_TAG;
        mpls_port->match_vlan = mpls_info->match_key[vp].match_vlan;

        if (mpls_info->match_key[vp].trunk_id != -1) {
            trunk_id = mpls_info->match_key[vp].trunk_id;
            BCM_GPORT_TRUNK_SET(mpls_port->port, trunk_id);
        } else {
            port_in = mpls_info->match_key[vp].port;
            mod_in = mpls_info->match_key[vp].modid;
            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                    mod_in, port_in, &mod_out, &port_out);
            BCM_GPORT_MODPORT_SET(mpls_port->port, mod_out, port_out);
        }

    } else if (mpls_info->match_key[vp].flags &  
                                _BCM_MPLS_PORT_MATCH_TYPE_INNER_VLAN_TAG) {

        mpls_port->criteria = BCM_MPLS_PORT_MATCH_PORT_INNER_VLAN_TAG;
        mpls_port->match_inner_vlan = mpls_info->match_key[vp].match_inner_vlan;

        if (mpls_info->match_key[vp].trunk_id != -1) {
            trunk_id = mpls_info->match_key[vp].trunk_id;
            BCM_GPORT_TRUNK_SET(mpls_port->port, trunk_id);
        } else {
            port_in = mpls_info->match_key[vp].port;
            mod_in = mpls_info->match_key[vp].modid;
            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                    mod_in, port_in, &mod_out, &port_out);
            BCM_GPORT_MODPORT_SET(mpls_port->port, mod_out, port_out);
        }

    } else if (mpls_info->match_key[vp].flags &
                   _BCM_MPLS_PORT_MATCH_TYPE_PORT) {
         source_trunk_map_modbase_entry_t modbase_entry;

         src_trk_idx = mpls_info->match_key[vp].index;

         BCM_IF_ERROR_RETURN
              (soc_mem_read(unit, SOURCE_TRUNK_MAP_MODBASEm, 
                                  MEM_BLOCK_ANY,  mode_in, &modbase_entry));
         port_in = src_trk_idx - soc_mem_field32_get(unit, 
                                  SOURCE_TRUNK_MAP_MODBASEm,
                                  &modbase_entry, BASEf);

        mpls_port->criteria = BCM_MPLS_PORT_MATCH_PORT;
        rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                               mod_in, port_in, &mod_out, &port_out);
         BCM_GPORT_MODPORT_SET(mpls_port->port, mod_out, port_out);
    }else if (mpls_info->match_key[vp].flags &
                  _BCM_MPLS_PORT_MATCH_TYPE_TRUNK) {

         trunk_id = mpls_info->match_key[vp].trunk_id;
         mpls_port->criteria = BCM_MPLS_PORT_MATCH_PORT;
         BCM_GPORT_TRUNK_SET(mpls_port->port, trunk_id);
    } else if ((mpls_info->match_key[vp].flags & 
                _BCM_MPLS_PORT_MATCH_TYPE_LABEL) ||
               (mpls_info->match_key[vp].flags == 
                _BCM_MPLS_PORT_MATCH_TYPE_LABEL_PORT)) {
        mpls_entry_entry_t ment;
        soc_mem_t mem = MPLS_ENTRYm;
        uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS];

        sal_memset(&ment, 0, sizeof(mpls_entry_entry_t));
        sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

         if (mpls_info->match_key[vp].flags & _BCM_MPLS_PORT_MATCH_TYPE_LABEL) {
              mpls_port->criteria = BCM_MPLS_PORT_MATCH_LABEL;
         } else if (mpls_info->match_key[vp].flags & 
                    _BCM_MPLS_PORT_MATCH_TYPE_LABEL_PORT) {
              mpls_port->criteria = BCM_MPLS_PORT_MATCH_LABEL_PORT; 
         }
         mpls_port->match_label = mpls_info->match_key[vp].match_label;

         if (match_trunk_id != -1) {
             trunk_id = match_trunk_id;
             soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__Tf, 1);
             soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__TGIDf, trunk_id);
             BCM_GPORT_TRUNK_SET(mpls_port->port, trunk_id);
         } else {
             mod_in = match_modid;
             port_in = match_port;
             soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MODULE_IDf, mod_in);
             soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__PORT_NUMf, port_in);

              rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                             mod_in, port_in, &mod_out, &port_out);
              if (rv < 0) {
                  return rv;
              }
              BCM_GPORT_MODPORT_SET(mpls_port->port, mod_out, port_out);
         }
         soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__MPLS_LABELf,
                             mpls_info->match_key[vp].match_label);
         soc_MPLS_ENTRYm_field32_set(unit, &ment, KEY_TYPEf, key_type);

         rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &src_trk_idx,
                             &ment, hw_buf, 0);

         if (rv == SOC_E_NOT_FOUND) {
             mpls_entry_extd_entry_t ment_extd;

             mem = MPLS_ENTRY_EXTDm;
             sal_memset(&ment_extd, 0, sizeof(mpls_entry_extd_entry_t));
             BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_entry_convert_to_extd_entry_key(unit,
                                                       &ment, &ment_extd));
             rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY,
                    &src_trk_idx, &ment_extd, hw_buf, 0);
         }
 
         if (rv == SOC_E_NONE) {
              if (soc_mem_field32_get(unit, mem, hw_buf,
                                            MPLS__CW_CHECK_CTRLf) == 0x3) {
                   
                   /* Control word is present, and check sequence number */
                   mpls_port->flags |= BCM_MPLS_PORT_SEQUENCED;
              } else if (soc_mem_field32_get(unit, mem, hw_buf,
                                            MPLS__CW_CHECK_CTRLf) == 0x1) {
                   mpls_port->flags |= BCM_MPLS_PORT_CONTROL_WORD;
              }

              /* VCCV defines 3 control-channels */
              if (soc_mem_field_valid(unit, mem, MPLS__PW_CC_TYPEf)) {
                  mpls_port->vccv_type = soc_mem_field32_get(unit, mem,
                                       hw_buf, MPLS__PW_CC_TYPEf);
              }
              if (SOC_IS_TRIUMPH3(unit)) {
                  if (soc_mem_field32_get(unit, mem, hw_buf,
                                          MPLS__MPLS_ACTION_IF_NOT_BOSf) == 0x1) {
                      mpls_port->flags2 |= BCM_MPLS_PORT2_WITH_LSP_OAM;
                  }
              }
              if (soc_feature(unit, soc_feature_mpls_entropy)) {
                   if (soc_mem_field32_get(unit, mem, hw_buf,
                             MPLS__MPLS_ACTION_IF_NOT_BOSf) == 0x2) {
                        mpls_port->flags2 |= BCM_MPLS_PORT2_INGRESS_ENTROPY_ENABLE;
                   }
              }
              if (soc_mem_field32_get(unit, mem, hw_buf,
                                        MPLS__DECAP_USE_EXP_FOR_PRIf) == 0x1) {
                   /* MAP_PRI_AND_CNG */
                   /* Use specified EXP-map to determine internal prio/color */
                   mpls_port->exp_map = 
                        soc_mem_field32_get(unit, mem, hw_buf, MPLS__EXP_MAPPING_PTRf);
                   mpls_port->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
                   mpls_port->flags |= BCM_MPLS_PORT_INT_PRI_MAP;
              } else if (soc_mem_field32_get(unit, mem, hw_buf,
                                               MPLS__DECAP_USE_EXP_FOR_PRIf) == 0x2) {
                   /* NEW_PRI_MAP_CNG */
                   /* Use the specified internal priority value */
                   mpls_port->int_pri = 
                        soc_mem_field32_get(unit, mem, hw_buf, MPLS__NEW_PRIf);
                   mpls_port->flags |= BCM_MPLS_PORT_INT_PRI_SET;

                   /* Use specified EXP-map to determine internal color */
                   mpls_port->exp_map = 
                        soc_mem_field32_get(unit, mem, hw_buf, MPLS__EXP_MAPPING_PTRf);
                   mpls_port->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
                   mpls_port->flags |= BCM_MPLS_PORT_COLOR_MAP;
              } else {
                   if (soc_mem_field_valid(unit, mem,
                                        MPLS__TRUST_OUTER_DOT1P_PTRf)) {
                        if (soc_mem_field32_get(unit, mem, hw_buf,
                                               MPLS__TRUST_OUTER_DOT1P_PTRf) == 
                                               _BCM_TR_MPLS_PRI_CNG_MAP_NONE) {
                          /*
                           * Don't trust the encapsulated packet's 802.1p pri/cfi.
                           * Use a fixed value for internal priority.
                          */
                           mpls_port->int_pri = 
                                soc_mem_field32_get(unit, mem, hw_buf, MPLS__NEW_PRIf);
                           mpls_port->flags |= BCM_MPLS_PORT_INT_PRI_SET;
                        }
                   }
              }
         }
    } else {
        mpls_port->criteria = BCM_MPLS_PORT_MATCH_NONE;
    }
    return rv;

}

/*
 * Function:
 *      bcm_tr3_mpls_port_stat_attach
 * Description:
 *      Attach counters entries to the given mpls gport and vpn
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_tr3_mpls_port_stat_attach(
                   int         unit,
                   bcm_vpn_t   vpn,
                   bcm_gport_t port,
                   uint32      stat_counter_id)
{
    soc_mem_t                 table=0;
    bcm_stat_flex_direction_t direction=bcmStatFlexDirectionIngress;
    uint32                    pool_number=0;
    uint32                    base_index=0;
    bcm_stat_flex_mode_t      offset_mode=0;
    bcm_stat_object_t         object=bcmStatObjectIngPort;
    bcm_stat_group_mode_t     group_mode= bcmStatGroupModeSingle;
    uint32                    count=0;
    uint32                     actual_num_tables=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode,&object,&offset_mode,&pool_number,&base_index);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,object,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit,group_mode));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit,object,1,&actual_num_tables,&table,&direction));

    BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_port_stat_get_table_info(
                   unit, vpn,port,&num_of_tables,&table_info[0]));
    for (count=0; count < num_of_tables ; count++) {
         if ((table_info[count].direction == direction) &&
           (table_info[count].table == table) ) {
              if (direction == bcmStatFlexDirectionIngress) {
              return _bcm_esw_stat_flex_attach_ingress_table_counters(
                     unit,
                     table_info[count].table,
                     table_info[count].index,
                     offset_mode,
                     base_index,
                     pool_number);
           } else {
              return _bcm_esw_stat_flex_attach_egress_table_counters(
                     unit,
                     table_info[count].table,
                     table_info[count].index,
                     0,
                     offset_mode,
                     base_index,
                     pool_number);
           } 
      }
    }
    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      bcm_tr3_mpls_port_stat_detach
 * Description:
 *      Detach counters entries to the given mpls port and vpn
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_tr3_mpls_port_stat_detach(
                   int         unit,
                   bcm_vpn_t   vpn,
                   bcm_gport_t port)
{
    uint32                     count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};

    BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_port_stat_get_table_info(
                   unit, vpn,port,&num_of_tables,&table_info[0]));

    for (count=0; count < num_of_tables ; count++) {
      if (table_info[count].direction == bcmStatFlexDirectionIngress) {
           rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                        unit, table_info[count].table, table_info[count].index);
           if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
           }
      } else {
           rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                        unit, 0, table_info[count].table, table_info[count].index);
           if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                err_code[bcmStatFlexDirectionEgress] = rv;
           }
      }
    }
    
    BCM_STAT_FLEX_DETACH_RETURN(err_code)
}
/*
 * Function:
 *      bcm_tr3_mpls_port_stat_counter_get
 * Description:
 *      Get counter statistic values for specific vpn and gport
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_tr3_mpls_port_stat_counter_get(
                   int                  unit,
                   int                  sync_mode,
                   bcm_vpn_t            vpn,
                   bcm_gport_t          port,
                   bcm_mpls_stat_t      stat,
                   uint32               num_entries,
                   uint32               *counter_indexes,
                   bcm_stat_value_t     *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t      table_info[
                                    BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmMplsInBytes) ||
        (stat == bcmMplsInPkts)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    if ((stat == bcmMplsInPkts) ||
        (stat == bcmMplsOutPkts)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }
    
    BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_port_stat_get_table_info(
                   unit, vpn,port,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_get(
                         unit, sync_mode,
                         table_info[table_count].index,
                         table_info[table_count].table,
                         0,
                         byte_flag,
                         counter_indexes[index_count],
                         &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_tr3_mpls_port_stat_counter_set
 * Description:
 *      Set counter statistic values for specific vpn and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_tr3_mpls_port_stat_counter_set(
                   int                  unit,
                   bcm_vpn_t            vpn,
                   bcm_gport_t          port,
                   bcm_mpls_stat_t      stat,
                   uint32               num_entries,
                   uint32               *counter_indexes,
                   bcm_stat_value_t     *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t      table_info[
                                    BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmMplsInBytes) ||
        (stat == bcmMplsInPkts)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    if ((stat == bcmMplsInPkts) ||
        (stat == bcmMplsOutPkts)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }
    
    BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_port_stat_get_table_info(
                   unit, vpn,port,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_set(
                         unit,
                         table_info[table_count].index,
                         table_info[table_count].table,
                         0,
                         byte_flag,
                         counter_indexes[index_count],
                         &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_tr3_mpls_port_stat_id_get
 * Description:
 *      Get stat counter id associated with specific vpn and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) VPN Id
 *      port             - (IN) MPLS GPORT ID
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_tr3_mpls_port_stat_id_get(
                   int             unit,
                   bcm_vpn_t       vpn,
                   bcm_gport_t     port,
                   bcm_mpls_stat_t stat,
                   uint32          *stat_counter_id)
{
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index=0;
    uint32                     num_stat_counter_ids=0;

    if ((stat == bcmMplsInBytes) ||
        (stat == bcmMplsInPkts)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
      }
    BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_port_stat_get_table_info(
                        unit,vpn,port,&num_of_tables,&table_info[0]));
    for (index=0; index < num_of_tables ; index++) {
         if (table_info[index].direction == direction)
             return _bcm_esw_stat_flex_get_counter_id(
                                  unit, 1, &table_info[index],
                                  &num_stat_counter_ids,stat_counter_id);
  }
    return BCM_E_NOT_FOUND;
}
/*
 * Function:
 *      bcm_tr3_mpls_label_stat_attach
 * Description:
 *      Attach counters entries to the given mpls label and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label
 *      port             - (IN) MPLS Gport
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_tr3_mpls_label_stat_attach(
                   int              unit,
                   bcm_mpls_label_t label,
                   bcm_gport_t      port,
                   uint32           stat_counter_id)
{
    soc_mem_t                 table=0;
    bcm_stat_flex_direction_t direction=bcmStatFlexDirectionIngress;
    uint32                    pool_number=0;
    uint32                    base_index=0;
    bcm_stat_flex_mode_t      offset_mode=0;
    bcm_stat_object_t         object=bcmStatObjectIngPort;
    bcm_stat_group_mode_t     group_mode= bcmStatGroupModeSingle;
    uint32                    count=0;
    uint32                     actual_num_tables=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode,&object,&offset_mode,&pool_number,&base_index);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,object,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit,group_mode));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit,object,1,&actual_num_tables,&table,&direction));

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_label_stat_get_table_info(
                   unit, label, port, object, &num_of_tables, &table_info[0]));
    for (count=0; count < num_of_tables ; count++) {
         if ((table_info[count].direction == direction) &&
           (table_info[count].table == table) ) {
              if (direction == bcmStatFlexDirectionIngress) {
              return _bcm_esw_stat_flex_attach_ingress_table_counters(
                     unit,
                     table_info[count].table,
                     table_info[count].index,
                     offset_mode,
                     base_index,
                     pool_number);
           } else {
              return _bcm_esw_stat_flex_attach_egress_table_counters(
                     unit,
                     table_info[count].table,
                     table_info[count].index,
                     0,
                     offset_mode,
                     base_index,
                     pool_number);
           } 
      }
    }
    return BCM_E_NOT_FOUND;
}
/*
 * Function:
 *      bcm_tr3_mpls_label_stat_detach
 * Description:
 *      Detach counters entries to the given mpls label and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label
 *      port             - (IN) MPLS Gport
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_tr3_mpls_label_stat_detach(
                   int              unit,
                   bcm_mpls_label_t label,
                   bcm_gport_t      port)
{
    uint32                     count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};
    mpls_entry_entry_t         mpls_entry_entry={{0}};
    mpls_entry_extd_entry_t    mpls_entry_extd_entry={{0}};

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_label_stat_get_table_info(
                unit, label, port, bcmStatObjectMaxValue,
                &num_of_tables,&table_info[0]));

    for (count=0; count < num_of_tables ; count++) {
      if (table_info[count].direction == bcmStatFlexDirectionIngress) {
           rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                        unit, table_info[count].table, table_info[count].index);
           if (SOC_IS_TRIUMPH3(unit)) {
               BCM_IF_ERROR_RETURN(soc_mem_read(
                                   unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY,
                                   table_info[count].index,
                                   (void *)&mpls_entry_extd_entry));
               /* NO check required */
               BCM_IF_ERROR_RETURN(
                  _bcm_esw_mpls_entry_convert_to_regular_entry(
                   unit,&mpls_entry_extd_entry,&mpls_entry_entry));
               BCM_IF_ERROR_RETURN(soc_mem_delete(
                                   unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY,
                                   (void *)&mpls_entry_extd_entry));
               BCM_IF_ERROR_RETURN(soc_mem_insert(
                                   unit, MPLS_ENTRYm, MEM_BLOCK_ALL,
                                   (void *)&mpls_entry_entry));
           }
           if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
           }
      } else {
           rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                        unit, 0, table_info[count].table, table_info[count].index);
           if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                err_code[bcmStatFlexDirectionEgress] = rv;
           }
      }
    }
    
    BCM_STAT_FLEX_DETACH_RETURN(err_code)
}
/*
 * Function:
 *      bcm_tr3_mpls_label_stat_counter_get
 * Description:
 *      Get counter statistic values for specific MPLS label and gport
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      label            - (IN) MPLS Label
 *      port             - (IN) MPLS Gport
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_tr3_mpls_label_stat_counter_get(
                   int                  unit,
                   int                  sync_mode,
                   bcm_mpls_label_t     label,
                   bcm_gport_t          port,
                   bcm_mpls_stat_t      stat,
                   uint32               num_entries,
                   uint32               *counter_indexes,
                   bcm_stat_value_t     *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t      table_info[
                                    BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmMplsInBytes) ||
        (stat == bcmMplsInPkts)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    if ((stat == bcmMplsInPkts) ||
        (stat == bcmMplsOutPkts)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }
    
    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_label_stat_get_table_info(
                unit, label, port, bcmStatObjectMaxValue,
                &num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_get(
                         unit, sync_mode,
                         table_info[table_count].index,
                         table_info[table_count].table,
                         0,
                         byte_flag,
                         counter_indexes[index_count],
                         &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_tr3_mpls_label_stat_counter_set
 * Description:
 *      Set counter statistic values for specific MPLS label and gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label
 *      port             - (IN) MPLS Gport
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_tr3_mpls_label_stat_counter_set(
                   int                  unit,
                   bcm_mpls_label_t     label,
                   bcm_gport_t          port,
                   bcm_mpls_stat_t      stat,
                   uint32               num_entries,
                   uint32               *counter_indexes,
                   bcm_stat_value_t     *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t      table_info[
                                    BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_vpn_t                       vpn = 0;

    if ((stat == bcmMplsInBytes) ||
        (stat == bcmMplsInPkts)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    if ((stat == bcmMplsInPkts) ||
        (stat == bcmMplsOutPkts)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }

    if (BCM_GPORT_IS_MPLS_PORT(port)) {
        _BCM_MPLS_VPN_SET(vpn, _BCM_MPLS_VPN_TYPE_VPWS, 1);
        BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_port_stat_get_table_info(
            unit, vpn, port, &num_of_tables, &table_info[0]));
    } else {
    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_label_stat_get_table_info(
                unit, label, port, bcmStatObjectMaxValue,
                &num_of_tables,&table_info[0]));
    }

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_set(
                         unit,
                         table_info[table_count].index,
                         table_info[table_count].table,
                         0,
                         byte_flag,
                         counter_indexes[index_count],
                         &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_tr3_mpls_label_stat_id_get
 * Description:
 *      Get stat counter id associated with given vlan
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      label            - (IN) MPLS Label
 *      port             - (IN) MPLS Gport
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_tr3_mpls_label_stat_id_get(
            int                  unit,
            bcm_mpls_label_t     label,
            bcm_gport_t          port,
            bcm_mpls_stat_t      stat,
            uint32               *stat_counter_id)
{
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index=0;
    uint32                     num_stat_counter_ids=0;

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

    if ((stat == bcmMplsInBytes) ||
        (stat == bcmMplsInPkts)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_label_stat_get_table_info(
                unit, label, port, bcmStatObjectMaxValue,
                &num_of_tables, &table_info[0]));
    for (index=0; index < num_of_tables ; index++) {
         if (table_info[index].direction == direction)
             return _bcm_esw_stat_flex_get_counter_id(
                                  unit, 1, &table_info[index],
                                  &num_stat_counter_ids,stat_counter_id);
    }
    return BCM_E_NOT_FOUND;
}
/*
 * Function:
 *      bcm_tr3_mpls_label_stat_enable_set
 * Purpose:
 *      Enable statistics collection for MPLS label or MPLS gport
 * Parameters:
 *      unit - (IN) Unit number.
 *      label - (IN) MPLS label
 *      port - (IN) MPLS gport
 *      enable - (IN) Non-zero to enable counter collection, zero to disable.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_tr3_mpls_label_stat_enable_set(
    int              unit, 
    bcm_mpls_label_t label,
    bcm_gport_t      port, 
    int              enable)
{
    uint32                     num_of_tables=0;
    uint32                     num_stat_counter_ids=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_stat_object_t          object=bcmStatObjectIngPort;
    uint32                     stat_counter_id[
                                       BCM_STAT_FLEX_COUNTER_MAX_DIRECTION]={0};
    uint32                     num_entries=0;
    int                        index=0;

   if (BCM_GPORT_IS_MPLS_PORT(port)) {
        /* When the gport is an MPLS port, we'll turn on the
         * port-based tracking to comply with API definition.
         */

        int vpn;

        /* just create a valid vpn id to pass the check */
        _BCM_MPLS_VPN_SET(vpn, _BCM_MPLS_VPN_TYPE_VPWS, 1);
        BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_port_stat_get_table_info(
                        unit,vpn,port,&num_of_tables,&table_info[0]));
        if (enable ) {
            for(index=0;index < num_of_tables ;index++) {
                if (table_info[index].direction == bcmStatFlexDirectionIngress) {
                    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_ingress_object(
                                        unit,table_info[index].table,
                                        table_info[index].index,NULL,&object));
                } else {
                    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_egress_object(
                                        unit,table_info[index].table,
                                        table_info[index].index,NULL,&object));
                }
                BCM_IF_ERROR_RETURN(bcm_esw_stat_group_create(
                                    unit,object,bcmStatGroupModeSingle,
                                &stat_counter_id[table_info[index].direction],
                                &num_entries));
                BCM_IF_ERROR_RETURN(bcm_tr3_mpls_port_stat_attach(
                                unit,vpn,port,
                                stat_counter_id[table_info[index].direction]));
            }
            return BCM_E_NONE;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_counter_id(
                            unit, num_of_tables,&table_info[0],
                            &num_stat_counter_ids,&stat_counter_id[0]));
            if ((stat_counter_id[bcmStatFlexDirectionIngress] == 0) &&
                (stat_counter_id[bcmStatFlexDirectionEgress] == 0)) {
                 return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(bcm_tr3_mpls_port_stat_detach(unit,vpn,port));
            if (stat_counter_id[bcmStatFlexDirectionIngress] != 0) {
                BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                                unit,
                                stat_counter_id[bcmStatFlexDirectionIngress]));
            }
            if (stat_counter_id[bcmStatFlexDirectionEgress] != 0) {
                BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                                unit,
                                stat_counter_id[bcmStatFlexDirectionEgress]));
            }
            return BCM_E_NONE;
        }

    }

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_mpls_label_stat_get_table_info(
                        unit, label, port, bcmStatObjectMaxValue,
                        &num_of_tables, &table_info[0]));
    if (enable ) {
        for(index=0;index < num_of_tables ;index++) {
            if (table_info[index].direction == bcmStatFlexDirectionIngress) {
                BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_ingress_object(
                                    unit,table_info[index].table,
                                    table_info[index].index,NULL,&object));
            } else {
                BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_egress_object(
                                    unit,table_info[index].table,
                                    table_info[index].index,NULL,&object));
            }
            BCM_IF_ERROR_RETURN(bcm_esw_stat_group_create(
                                unit,object,bcmStatGroupModeSingle,
                                &stat_counter_id[table_info[index].direction],
                                &num_entries));
            BCM_IF_ERROR_RETURN(bcm_esw_mpls_label_stat_attach(
                                unit,label,port,
                                stat_counter_id[table_info[index].direction]));
        }
        return BCM_E_NONE;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_counter_id(
                            unit, num_of_tables,&table_info[0],
                            &num_stat_counter_ids,&stat_counter_id[0]));
        if ((stat_counter_id[bcmStatFlexDirectionIngress] == 0) &&
            (stat_counter_id[bcmStatFlexDirectionEgress] == 0)) {
             return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(bcm_esw_mpls_label_stat_detach(unit,label,port));
        if (stat_counter_id[bcmStatFlexDirectionIngress] != 0) {
            BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                                unit,
                                stat_counter_id[bcmStatFlexDirectionIngress]));
        }
        if (stat_counter_id[bcmStatFlexDirectionEgress] != 0) {
            BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                                unit,
                                stat_counter_id[bcmStatFlexDirectionEgress]));
        }
        return BCM_E_NONE;
    }

}
/*
 * Function:
 *      bcm_tr3_mpls_label_stat_get
 * Purpose:
 *      Get L2 MPLS PW Stats
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      label  - (IN) MPLS label
 *      port   - (IN) MPLS gport
 *      stat   - (IN)  specify the Stat type
 *      val    - (OUT) 64-bit Stats value
 * Returns:
 *      BCM_E_XXX
 */

int bcm_tr3_mpls_label_stat_get(int              unit,
                                int              sync_mode,
                                bcm_mpls_label_t label, 
                                bcm_gport_t      port,
                                bcm_mpls_stat_t  stat, 
                                uint64           *val)
{
    uint32           counter_indexes=0   ;
    bcm_stat_value_t counter_values={0};

   if (BCM_GPORT_IS_MPLS_PORT(port)) {
        int vpn;

        /* just create a valid vpn id to pass the check */
        _BCM_MPLS_VPN_SET(vpn, _BCM_MPLS_VPN_TYPE_VPWS, 1);

        BCM_IF_ERROR_RETURN(
                    bcm_tr3_mpls_port_stat_counter_get(unit, sync_mode, vpn, 
                                                       port, stat, 1,
                                                       &counter_indexes,
                                                       &counter_values));
    } else {
        if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(
                    bcm_tr3_mpls_label_stat_counter_get(unit, sync_mode, label,
                                                        port, stat, 1,
                                                        &counter_indexes,
                                                        &counter_values));
    }

    if ((stat == bcmMplsInPkts) ||
        (stat == bcmMplsOutPkts)) {
         COMPILER_64_SET(*val,
                         COMPILER_64_HI(counter_values.packets64),
                         COMPILER_64_LO(counter_values.packets64));
    } else {
         COMPILER_64_SET(*val,
                         COMPILER_64_HI(counter_values.bytes),
                         COMPILER_64_LO(counter_values.bytes));
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_tr3_mpls_label_stat_get32
 * Purpose:
 *      Get L2 MPLS PW Stats
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      label  - (IN) MPLS label
 *      port   - (IN) MPLS gport
 *      stat   - (IN)  specify the Stat type
 *      val    - (OUT) 32-bit Stats value
 * Returns:
 *      BCM_E_XXX
 */

int bcm_tr3_mpls_label_stat_get32(int              unit, 
                                  int              sync_mode,
                                  bcm_mpls_label_t label, 
                                  bcm_gport_t      port, 
                                  bcm_mpls_stat_t  stat, 
                                  uint32           *val)
{
    uint32           counter_indexes=0;
    bcm_stat_value_t counter_values={0};

    if (BCM_GPORT_IS_MPLS_PORT(port)) {
        int vpn;

        /* just create a valid vpn id to pass the check */
        _BCM_MPLS_VPN_SET(vpn, _BCM_MPLS_VPN_TYPE_VPWS, 1);

        BCM_IF_ERROR_RETURN(
                    bcm_tr3_mpls_port_stat_counter_get(unit, sync_mode, vpn, 
                                                       port, stat, 1,
                                                       &counter_indexes,
                                                       &counter_values));
     } else {
         if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
            return BCM_E_PARAM;
         }
         BCM_IF_ERROR_RETURN(
                    bcm_tr3_mpls_label_stat_counter_get(unit, sync_mode, label,
                                                        port, stat, 1,
                                                        &counter_indexes,
                                                        &counter_values));
    }

    if ((stat == bcmMplsInPkts) ||
        (stat == bcmMplsOutPkts)) {
         *val = counter_values.packets;
    } else {
         /* Ignoring Hi bytes value */
         *val = COMPILER_64_LO(counter_values.bytes);
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_tr3_mpls_label_stat_clear
 * Purpose:
 *      Clear L2 MPLS PW Stats
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      label  - (IN) MPLS label
 *      port   - (IN) MPLS gport
 *      stat   - (IN)  specify the Stat type
 * Returns:
 *      BCM_E_XXX
 */

int bcm_tr3_mpls_label_stat_clear(
    int              unit, 
    bcm_mpls_label_t label, 
    bcm_gport_t      port, 
    bcm_mpls_stat_t  stat)
{
    uint32           counter_indexes=0   ;
    bcm_stat_value_t counter_values={0};

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_mpls_label_stat_counter_set(
                        unit,label,port,stat,1,
                        &counter_indexes,&counter_values));
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/* 
 * Function:
 *      _bcm_tr3_mpls_match_key_recover
 * Purpose:
 *      - Recover MPLS match key from h/w tables
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_mpls_match_key_recover(int unit)
{
    int key_type, i, vp, port_type;
    int index_min, index_max;
    vlan_xlate_extd_entry_t vent;
    mpls_entry_entry_t mpls_entry;
    source_trunk_map_table_entry_t trunk_entry;
    _bcm_tr_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);

    
    /* Recover match keys from VLAN_XLATE_EXTDm table */
    index_min = soc_mem_index_min(unit, VLAN_XLATE_EXTDm);
    index_max = soc_mem_index_max(unit, VLAN_XLATE_EXTDm);
    for (i = index_min; i <= index_max; i++) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit,
                                         VLAN_XLATE_EXTDm, MEM_BLOCK_ANY, i, &vent));
        if (soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, VALID_0f) == 0) {
            continue;
        }

        key_type = soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, KEY_TYPE_0f);
        if ((key_type != TR3_VLXLT_X_HASH_KEY_TYPE_OVID) &&
            (key_type != TR3_VLXLT_X_HASH_KEY_TYPE_IVID) &&
            (key_type != TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID) &&
            (key_type != TR3_VLXLT_X_HASH_KEY_TYPE_PRI_CFI)) {
            continue;
        }

        if (soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__MPLS_ACTIONf) != 0x1) {
            continue;
        }    

        vp = soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__SOURCE_VPf);

        /* Proceed only if this VP belongs to MPLS */
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
            if (key_type == TR3_VLXLT_X_HASH_KEY_TYPE_OVID) {

                mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_VLAN;
                mpls_info->match_key[vp].match_vlan =
                    soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__OVIDf);
                if (soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__Tf) == 0x1) {
                    mpls_info->match_key[vp].trunk_id = 
                              soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__TGIDf);
                } else {
                    mpls_info->match_key[vp].port = 
                            soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__PORT_NUMf);
                    mpls_info->match_key[vp].modid = 
                           soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__MODULE_IDf);
                }
                bcm_tr_mpls_port_match_count_adjust(unit, vp, 1);
        } else if (key_type == TR3_VLXLT_X_HASH_KEY_TYPE_IVID) {

                mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_INNER_VLAN;
                mpls_info->match_key[vp].match_inner_vlan =
                    soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__IVIDf);
                if (soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__Tf) == 0x1) {
                    mpls_info->match_key[vp].trunk_id = 
                              soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__TGIDf);
                } else {
                    mpls_info->match_key[vp].port = 
                            soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__PORT_NUMf);
                    mpls_info->match_key[vp].modid = 
                           soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__MODULE_IDf);
                }
                bcm_tr_mpls_port_match_count_adjust(unit, vp, 1);
            } else if (key_type == TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID) {

                mpls_info->match_key[vp].flags =
                                         _BCM_MPLS_PORT_MATCH_TYPE_VLAN_STACKED;
                mpls_info->match_key[vp].match_vlan =
                              soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__OVIDf);
                mpls_info->match_key[vp].match_inner_vlan = 
                              soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__IVIDf);
                if (soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, Tf) == 0x1) {
                    mpls_info->match_key[vp].trunk_id =
                              soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__TGIDf);
                } else {
                    mpls_info->match_key[vp].port = 
                            soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__PORT_NUMf);
                    mpls_info->match_key[vp].modid =
                           soc_VLAN_XLATE_EXTDm_field32_get(unit, &vent, XLATE__MODULE_IDf);
                }
                bcm_tr_mpls_port_match_count_adjust(unit, vp, 1);
            } 

#ifdef BCM_TRIUMPH2_SUPPORT    /* Recover flex stats */
            if (soc_feature(unit, soc_feature_gport_service_counters)) {
                BCM_IF_ERROR_RETURN(_bcm_tr_mpls_flex_stat_recover(unit, vp));
            }
#endif /* BCM_TRIUMPH2_SUPPORT */

            /* Recover TPID ref count */
            BCM_IF_ERROR_RETURN(_bcm_tr_mpls_source_vp_tpid_recover(unit, vp));
        }
    }

    /* Recover misc associated data */
    BCM_IF_ERROR_RETURN(_bcm_tr_mpls_associated_data_recover(unit));

    /* Recover match keys from SOURCE_TRUNK_MAP_TABLEm table */
    index_min = soc_mem_index_min(unit, SOURCE_TRUNK_MAP_TABLEm);
    index_max = soc_mem_index_max(unit, SOURCE_TRUNK_MAP_TABLEm);
    for (i = index_min; i <= index_max; i++) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, SOURCE_TRUNK_MAP_TABLEm,
                                               MEM_BLOCK_ANY, i, &trunk_entry));

        vp = soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit,
                                             &trunk_entry, SOURCE_VPf);

        /* Proceed only if this VP belongs to MPLS */
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {

            port_type = soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit,
                                                     &trunk_entry, PORT_TYPEf); 
            if (0 == port_type) { /* Normal port */
                mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_PORT;
                mpls_info->match_key[vp].index = i;
            } else if(1 == port_type) { /* Trunk port */
                mpls_info->match_key[vp].flags =_BCM_MPLS_PORT_MATCH_TYPE_TRUNK;
                mpls_info->match_key[vp].trunk_id = 
                 soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, &trunk_entry,
                                                                         TGIDf);
            }

#ifdef BCM_TRIUMPH2_SUPPORT    /* Recover flex stats */
            if (soc_feature(unit, soc_feature_gport_service_counters)) {
                BCM_IF_ERROR_RETURN(_bcm_tr_mpls_flex_stat_recover(unit, vp));
            }
#endif /* BCM_TRIUMPH2_SUPPORT */

            /* Recover TPID ref count */
            BCM_IF_ERROR_RETURN(_bcm_tr_mpls_source_vp_tpid_recover(unit, vp));
        }
    }

    /* Recover match keys from MPLS_ENTRYm table */
    index_min = soc_mem_index_min(unit, MPLS_ENTRYm);
    index_max = soc_mem_index_max(unit, MPLS_ENTRYm);
    for (i = index_min; i <= index_max; i++) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, MPLS_ENTRYm,
                                                MEM_BLOCK_ANY, i, &mpls_entry));

        if (soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry, VALIDf) == 0) {
            continue;
        }

        key_type = soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry, KEY_TYPEf);

        if (key_type != 0x12 && key_type != 0x10 &&
            key_type != 0x11 && key_type != 0x13) {
            continue;
        }
      
        if ((key_type == 0x11) ||
            (key_type == 0x13)) {
            i++; /* MPLS_ENTRY_2 */
        }
        
        vp = soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry, MPLS__SOURCE_VPf);

        /* Proceed only if this VP belongs to MPLS */
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
            bcm_module_t  modid;
            bcm_port_t    local_port;
            bcm_trunk_t   trunk_id;
            int           local_id;
            bcm_gport_t   mpls_port_id;
            bcm_trunk_t   mpls_tid = 0;
            bcm_port_t    mpls_local_port = 0;
            int           mpls_local_modid = 0;
            int           rv;

            BCM_GPORT_MPLS_PORT_ID_SET(mpls_port_id,vp);
            rv = _bcm_esw_gport_resolve(unit, mpls_port_id, &modid,
                                           &local_port, &trunk_id,
                                           &local_id);
            if (rv != BCM_E_NONE) {
                continue;
            }
            
            if (soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry, MPLS__Tf) == 0x1) {
                mpls_tid = soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry, MPLS__TGIDf);
                if (mpls_tid == trunk_id) {
                    mpls_info->match_key[vp].trunk_id = mpls_tid;
                } else {
                    mpls_info->match_key[vp].fo_trunk_id = mpls_tid;
                }
                mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_LABEL_PORT;
            } else {
                mpls_local_port = 
                        soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry, MPLS__PORT_NUMf);
                mpls_local_modid = 
                           soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry, MPLS__MODULE_IDf);
                if ((mpls_local_port == 0) && (mpls_local_modid == 0)) {
                    mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_LABEL;
                    mpls_info->match_key[vp].port  = mpls_local_port;
                    mpls_info->match_key[vp].modid = mpls_local_modid;
                } else {
                    mpls_info->match_key[vp].flags = _BCM_MPLS_PORT_MATCH_TYPE_LABEL_PORT;
                    if (mpls_local_port == local_port && mpls_local_modid == modid) {
                        mpls_info->match_key[vp].port  = mpls_local_port;
                        mpls_info->match_key[vp].modid = mpls_local_modid;
                    } else {
                        mpls_info->match_key[vp].fo_port  = mpls_local_port;
                        mpls_info->match_key[vp].fo_modid = mpls_local_modid;
                    }
                } 
            } 
            mpls_info->match_key[vp].match_label = 
                    soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry, MPLS__MPLS_LABELf);
            
#ifdef BCM_TRIUMPH2_SUPPORT    /* Recover flex stats */
            if (soc_feature(unit, soc_feature_gport_service_counters)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr_mpls_flex_stat_recover(unit, vp));
                _bcm_tr_mpls_label_flex_stat_recover(unit, &mpls_entry);
            }
#endif /* BCM_TRIUMPH2_SUPPORT */

            /* Recover TPID ref count */
            BCM_IF_ERROR_RETURN(_bcm_tr_mpls_source_vp_tpid_recover(unit, vp));
        }
    }
    return BCM_E_NONE;
}
#endif

#else /* BCM_TRIUMPH3_SUPPORT && BCM_MPLS_SUPPORT && INCLUDE_L3 */
typedef int bcm_esw_tr3_mpls_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_TRIUMPH3_SUPPORT && BCM_MPLS_SUPPORT && INCLUDE_L3 */


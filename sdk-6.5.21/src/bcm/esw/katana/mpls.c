/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    mpls.c
 * Purpose: Katana MPLS enhancements
 */
#include <soc/defs.h>

#include <assert.h>
#include <shared/bsl.h>
#include <sal/core/libc.h>
#if defined(BCM_KATANA_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <bcm/qos.h>
#include <bcm/error.h>
#include <bcm_int/esw/triumph2.h>
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


 /*
  * Function:
  * 	 _bcm_kt_mpls_p2mp_loopback_enable
  * Purpose:
  * 	 Enable loopback for MPLS P2MP
  * Parameters:
  *   IN : unit
  * Returns:
  * 	 BCM_E_XXX
  */
 
STATIC int
_bcm_kt_mpls_p2mp_loopback_enable(int unit)
{
    int rv = BCM_E_NONE;
    multipass_loopback_bitmap_entry_t  p2mp_loopback;
    soc_field_t lport_fields[] = {
#ifndef  BCM_KATANA2_SUPPORT
        PORT_TYPEf,
#endif
        MPLS_ENABLEf,
        V4L3_ENABLEf,
        V6L3_ENABLEf
    };
    uint32 lport_values[] = {
#ifndef  BCM_KATANA2_SUPPORT
        0x2,
#endif
        0x1,  /* MPLS_ENABLEf */
        0x1,  /* V4L3_ENABLEf */
        0x1   /* V6L3_ENABLEf */
    };

    if (SOC_IS_KATANA(unit)) {
        sal_memset(&p2mp_loopback, 0,
                sizeof(multipass_loopback_bitmap_entry_t));

        /* Enable Loopback */
        soc_mem_field32_set(unit, MULTIPASS_LOOPBACK_BITMAPm,
                &p2mp_loopback, BITMAP_W1f, 0x8);
        rv = soc_mem_write(unit, MULTIPASS_LOOPBACK_BITMAPm,
                MEM_BLOCK_ALL, 0, &p2mp_loopback);
        if (rv < 0) {
            return rv;
        }
    }

    /* Update LPORT Profile Table */
    rv = _bcm_lport_profile_fields32_modify(unit, LPORT_PROFILE_LPORT_TAB,
            COUNTOF(lport_fields),
            lport_fields, lport_values);

    return rv;
}

 /*
  * Function:
  * 	 _bcm_kt_mpls_p2mp_loopback_disable
  * Purpose:
  * 	 Disable loopback for MPLS P2MP
  * Parameters:
  *   IN : unit
  * Returns:
  * 	 BCM_E_XXX
  */
 
STATIC int
_bcm_kt_mpls_p2mp_loopback_disable(int unit)
{
    int rv = BCM_E_NONE;
    multipass_loopback_bitmap_entry_t  p2mp_loopback;
    soc_field_t lport_fields[] = {
#ifndef  BCM_KATANA2_SUPPORT
        PORT_TYPEf,
#endif
        MPLS_ENABLEf,
        V4L3_ENABLEf,
        V6L3_ENABLEf
    };
    uint32 lport_values[] = {
#ifndef  BCM_KATANA2_SUPPORT
        0x0,  /* PORT_TYPEf */
#endif
        0x0,  /* MPLS_ENABLEf */
        0x0,  /* V4L3_ENABLEf */
        0x0   /* V6L3_ENABLEf */
    };

    if (SOC_IS_KATANA(unit)) {
        sal_memset(&p2mp_loopback, 0,
                sizeof(multipass_loopback_bitmap_entry_t));

        /* Disable Loopback */
        soc_mem_field32_set(unit, MULTIPASS_LOOPBACK_BITMAPm,
                &p2mp_loopback, BITMAP_W1f, 0x0);
        rv = soc_mem_write(unit, MULTIPASS_LOOPBACK_BITMAPm,
                MEM_BLOCK_ALL, 0, &p2mp_loopback);
        if (rv < 0) {
            return rv;
        }
    }

    /* Update LPORT Profile Table */
    rv = _bcm_lport_profile_fields32_modify(unit, LPORT_PROFILE_LPORT_TAB,
            COUNTOF(lport_fields),
            lport_fields, lport_values);

    return rv;
}


/*
 * Function:
 *      _bcm_kt_mpls_entry_delete
 * Purpose:
 *      Delete entry from MPLS_ENTRY table
 * Parameters:
 *      unit - Device Number
 *      ment - MPLS Label entry to delete
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_kt_mpls_entry_delete(int unit, mpls_entry_entry_t *ment, 
                                                       bcm_mpls_tunnel_switch_t *info, int index)
{   
    ing_pw_term_counters_entry_t pw_cnt_entry;
    int rv, action, ecmp_index = -1, nh_index = -1, pw_cnt = -1;
    bcm_if_t  egress_if=0;
    int  ref_count=0, forwarding_field_type=-1, ipmc_index=-1;
    int two_pass=-1, l3_iif=-1, is_set=0;

    if (soc_MPLS_ENTRYm_field32_get(unit, ment, PW_TERM_NUM_VALIDf)) {
        pw_cnt = soc_MPLS_ENTRYm_field32_get(unit, ment, PW_TERM_NUMf);
    }

    action = soc_MPLS_ENTRYm_field32_get(unit, ment, 
                                                      MPLS_ACTION_IF_BOSf);
    forwarding_field_type =  soc_MPLS_ENTRYm_field32_get(unit, ment,  
                                                      FORWARDING_FIELD_TYPEf);

    if (forwarding_field_type == _BCM_MPLS_FORWARD_MULTICAST_GROUP) {
        ipmc_index = soc_MPLS_ENTRYm_field32_get(unit, ment, 
                                                      FORWARDING_FIELDf);
        two_pass = soc_MPLS_ENTRYm_field32_get(unit, ment, 
                                                      MPLS_SECOND_PASS_ACTION_VALIDf);
        rv = bcm_xgs3_ipmc_id_is_set(unit, ipmc_index, &is_set);
        if (BCM_SUCCESS(rv)) {
            if (is_set) {
               /* First Delete IPMC_entry, then delete P2MP Label entry */
               return BCM_E_BUSY;
            }
        }
        if ((two_pass) && (info->action != -1)) {
            if (info->action == BCM_MPLS_SWITCH_ACTION_PHP) {
                   l3_iif = soc_MPLS_ENTRYm_field32_get(unit, ment, 
                                                      L3_IIF_2f);
                   soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_NOT_BOSf,
                                0x0); /* INVALID */
                    soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_BOSf,
                                0x2); /* L3_IIF */
                 soc_MPLS_ENTRYm_field32_set(unit, ment, L3_IIFf,
                                l3_iif);
            }
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_SECOND_PASS_ACTION_VALIDf,
                                0x0);
            (void) _bcm_kt_mpls_p2mp_loopback_disable(unit);
            BCM_IF_ERROR_RETURN(rv);

            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_SECOND_PASS_ACTION_IF_BOSf,
                                0x0); 
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_SECOND_PASS_ACTION_IF_NOT_BOSf,
                                0x0);
            soc_MPLS_ENTRYm_field32_set(unit, ment, L3_IIF_2f,
                                0x0);
            rv = soc_mem_write(unit, MPLS_ENTRYm,
                           MEM_BLOCK_ALL, index,
                           ment);
            return rv;
        }
    } else if (forwarding_field_type == _BCM_MPLS_FORWARD_NEXT_HOP) {
        /* SWAP_NHI or L3_NHI */
        nh_index = soc_MPLS_ENTRYm_field32_get(unit, ment, NEXT_HOP_INDEXf);
    } else if (forwarding_field_type == _BCM_MPLS_FORWARD_ECMP_GROUP) {
        /* L3_ECMP */
        ecmp_index = soc_MPLS_ENTRYm_field32_get(unit, ment, ECMP_PTRf);
    }
        
    /* Delete the entry from HW */
    rv = soc_mem_delete(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, ment);
    if ( (rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE) ) {
         return rv;
    }

    if (pw_cnt != -1) {
        sal_memset(&pw_cnt_entry, 0, sizeof(ing_pw_term_counters_entry_t));
        (void) WRITE_ING_PW_TERM_COUNTERSm(unit, MEM_BLOCK_ALL, pw_cnt,
                                           &pw_cnt_entry);
        (void) bcm_tr_mpls_pw_used_clear(unit, pw_cnt);
    }

    if (forwarding_field_type != _BCM_MPLS_FORWARD_MULTICAST_GROUP) {
        if ((forwarding_field_type == _BCM_MPLS_FORWARD_NEXT_HOP) && (action == 0x3)) {
            /* SWAP_NHI */
            /* Check if tunnel_switch.egress_label mode is being used */
            rv = bcm_tr_mpls_get_vp_nh (unit, (bcm_if_t) nh_index, &egress_if);
            if (rv == BCM_E_NONE) {
                rv = bcm_tr_mpls_l3_nh_info_delete(unit, nh_index);
            } else {
                /* Decrement next-hop Reference count */
                rv = bcm_xgs3_get_ref_count_from_nhi(unit, 0, &ref_count, nh_index);
            }
        } else if ((forwarding_field_type == _BCM_MPLS_FORWARD_NEXT_HOP) && (action == 0x4)) {
            /* L3_NHI */
            rv = bcm_xgs3_nh_del(unit, 0, nh_index);
        } else if (forwarding_field_type == _BCM_MPLS_FORWARD_ECMP_GROUP) {
            /* L3_ECMP */
            rv = bcm_xgs3_ecmp_group_del(unit, ecmp_index, 0);
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_kt_mpls_entry_set_key
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
_bcm_kt_mpls_entry_set_key(int unit, bcm_mpls_tunnel_switch_t *info,
                           mpls_entry_entry_t *ment)
{
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int rv, gport_id;

    sal_memset(ment, 0, sizeof(mpls_entry_entry_t));

    if (info->port == BCM_GPORT_INVALID) {
        /* Global label, mod/port not part of lookup key */
        soc_MPLS_ENTRYm_field32_set(unit, ment, MODULE_IDf, 0);
        soc_MPLS_ENTRYm_field32_set(unit, ment, PORT_NUMf, 0);
        if (BCM_XGS3_L3_MPLS_LBL_VALID(info->label)) {
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_LABELf, info->label);
        } else {
            return BCM_E_PARAM;
        }
        soc_MPLS_ENTRYm_field32_set(unit, ment, VALIDf, 1);
        return BCM_E_NONE;
    }

    rv = _bcm_esw_gport_resolve(unit, info->port, &mod_out, 
                                &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

    if (BCM_GPORT_IS_TRUNK(info->port)) {
        soc_MPLS_ENTRYm_field32_set(unit, ment, Tf, 1);
        soc_MPLS_ENTRYm_field32_set(unit, ment, TGIDf, trunk_id);
    } else {
        soc_MPLS_ENTRYm_field32_set(unit, ment, MODULE_IDf, mod_out);
        soc_MPLS_ENTRYm_field32_set(unit, ment, PORT_NUMf, port_out);
    }
    if (BCM_XGS3_L3_MPLS_LBL_VALID(info->label)) {
        soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_LABELf, info->label);
    } else {
        return BCM_E_PARAM;
    }
    soc_MPLS_ENTRYm_field32_set(unit, ment, VALIDf, 1);
    soc_MPLS_ENTRYm_field32_set(unit, ment, KEY_TYPEf, 0);
    
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_kt_mpls_entry_get_key
 * Purpose:
 *      Convert key part of HW entry to application format. 
 * Parameters:
 *      unit - Device Number
 *      ment - mpls entry
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_kt_mpls_entry_get_key(int unit, mpls_entry_entry_t *ment,
                           bcm_mpls_tunnel_switch_t *info)
{
    bcm_port_t port_in, port_out;
    bcm_module_t mod_in, mod_out;
    bcm_trunk_t trunk_id;

    port_in = soc_MPLS_ENTRYm_field32_get(unit, ment, PORT_NUMf);
    mod_in = soc_MPLS_ENTRYm_field32_get(unit, ment, MODULE_IDf);
    if (soc_MPLS_ENTRYm_field32_get(unit, ment, Tf)) {
        trunk_id = soc_MPLS_ENTRYm_field32_get(unit, ment, TGIDf);
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
    info->label = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS_LABELf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt_mpls_entry_get_data
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
_bcm_kt_mpls_entry_get_data(int unit, mpls_entry_entry_t *ment,
                            bcm_mpls_tunnel_switch_t *info)
{
    int action, nh_index, ecmp_group, vrf, ipmc_index;
    uint32 forwarding_type=0;
    bcm_if_t egress_if=0;
    int rv=BCM_E_NONE;
    int mode = 0;

    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_ingress_mode_get(unit, &mode));

    action = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS_ACTION_IF_BOSf);
    switch(action) {
    case 0x2:
        info->action = BCM_MPLS_SWITCH_ACTION_POP;
        forwarding_type = soc_MPLS_ENTRYm_field32_get(unit, ment,
                                               FORWARDING_FIELD_TYPEf);
        if (forwarding_type == _BCM_MPLS_FORWARD_MULTICAST_GROUP) {
            info->flags |= BCM_MPLS_SWITCH_P2MP;
            if (!mode) {
                vrf = soc_MPLS_ENTRYm_field32_get(unit, ment, L3_IIF_2f);
                vrf -= _BCM_TR_MPLS_L3_IIF_BASE;
                _BCM_MPLS_VPN_SET(info->vpn, _BCM_MPLS_VPN_TYPE_L3, vrf);
            } else {
                info->ingress_if = soc_MPLS_ENTRYm_field32_get(unit, ment, L3_IIF_2f);
            }
        } else {
            if (!mode) {
                vrf = soc_MPLS_ENTRYm_field32_get(unit, ment, L3_IIFf);
                /*
                 * If the ingress mode is not set, then L3_IIF is added
                 * above the intreface of _BCM_TR_MPLS_L3_IIF_BASE.
                 * If this is 0 then 0 vpn index was sent.
                 */
                if (vrf != 0) {
                    vrf -= _BCM_TR_MPLS_L3_IIF_BASE;
                    _BCM_MPLS_VPN_SET(info->vpn, _BCM_MPLS_VPN_TYPE_L3, vrf);
                }
            } else {
                info->ingress_if = soc_MPLS_ENTRYm_field32_get(unit, ment, L3_IIFf);
            }
        }
        break;
    case 0x3:
        info->action = BCM_MPLS_SWITCH_ACTION_SWAP;
        forwarding_type = soc_MPLS_ENTRYm_field32_get(unit, ment,
                                               FORWARDING_FIELD_TYPEf);
        if (forwarding_type == _BCM_MPLS_FORWARD_NEXT_HOP) {
              nh_index = soc_MPLS_ENTRYm_field32_get(unit, ment,
                                               NEXT_HOP_INDEXf);
              rv = bcm_tr_mpls_get_vp_nh (unit, nh_index, &egress_if);
              if (rv == BCM_E_NONE) {
                  rv = bcm_tr_mpls_l3_nh_info_get(unit, info, nh_index);
                  BCM_IF_ERROR_RETURN(rv);
              } else {
                  info->egress_if = nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
                  info->egress_label.label = BCM_MPLS_LABEL_INVALID;
              }
        } else if (forwarding_type == _BCM_MPLS_FORWARD_ECMP_GROUP) {
              ecmp_group = soc_MPLS_ENTRYm_field32_get(unit, ment,
                                               ECMP_PTRf);
              info->egress_if = ecmp_group + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit); 
              info->egress_label.label = BCM_MPLS_LABEL_INVALID;
        } else if (forwarding_type == _BCM_MPLS_FORWARD_MULTICAST_GROUP) {
              ipmc_index = soc_MPLS_ENTRYm_field32_get(unit, ment, 
                                                           FORWARDING_FIELDf);
              info->flags |= BCM_MPLS_SWITCH_P2MP;
              info->egress_label.label = BCM_MPLS_LABEL_INVALID;
              _BCM_MULTICAST_GROUP_SET(info->mc_group, 
                  _BCM_MULTICAST_TYPE_EGRESS_OBJECT, ipmc_index);
        }
        break;
    case 0x4:
        info->action = BCM_MPLS_SWITCH_ACTION_PHP;
        forwarding_type = soc_MPLS_ENTRYm_field32_get(unit, ment,
                                               FORWARDING_FIELD_TYPEf);
        if (forwarding_type == _BCM_MPLS_FORWARD_NEXT_HOP) {
              nh_index = soc_MPLS_ENTRYm_field32_get(unit, ment,
                                               NEXT_HOP_INDEXf);
              info->egress_if = nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else if (forwarding_type == _BCM_MPLS_FORWARD_ECMP_GROUP) {
              ecmp_group = soc_MPLS_ENTRYm_field32_get(unit, ment,
                                               ECMP_PTRf);
              info->egress_if = ecmp_group + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit); 
        }
        break;
    default:
        return BCM_E_INTERNAL;
        break;
    }
    if (SOC_MEM_FIELD_VALID(unit, MPLS_ENTRYm, MPLS__DROP_DATA_ENABLEf)) {
        if (soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__DROP_DATA_ENABLEf)) {
            info->flags |= BCM_MPLS_SWITCH_DROP;
        }
    }

    if (SOC_MEM_FIELD_VALID(unit, MPLS_ENTRYm, MPLS__RX_PROT_GROUPf)) {
        if (soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__RX_PROT_GROUPf)) {
            info->failover_id = 
                soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS__RX_PROT_GROUPf);
        }
    }

    if (soc_MPLS_ENTRYm_field32_get(unit, ment, PW_TERM_NUM_VALIDf)) {
        info->flags |= BCM_MPLS_SWITCH_COUNTED;
    }
    if (!soc_MPLS_ENTRYm_field32_get(unit, ment, DECAP_USE_TTLf)) {
        info->flags |= BCM_MPLS_SWITCH_INNER_TTL;
    }
    if (soc_MPLS_ENTRYm_field32_get(unit, ment, DECAP_USE_EXP_FOR_INNERf)) {
        info->flags |= BCM_MPLS_SWITCH_INNER_EXP;
    }
    if (soc_MPLS_ENTRYm_field32_get(unit, ment, 
                                    DECAP_USE_EXP_FOR_PRIf) == 0x1) {

        /* Use specified EXP-map to determine internal prio/color */
        info->flags |= BCM_MPLS_SWITCH_INT_PRI_MAP;
        info->exp_map = 
            soc_MPLS_ENTRYm_field32_get(unit, ment, EXP_MAPPING_PTRf);
        info->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
    } else if (soc_MPLS_ENTRYm_field32_get(unit, ment, 
                                           DECAP_USE_EXP_FOR_PRIf) == 0x2) {

        /* Use the specified internal priority value */
        info->flags |= BCM_MPLS_SWITCH_INT_PRI_SET;
        info->int_pri =
            soc_MPLS_ENTRYm_field32_get(unit, ment, NEW_PRIf);

        /* Use specified EXP-map to determine internal color */
        info->flags |= BCM_MPLS_SWITCH_COLOR_MAP;
        info->exp_map = 
            soc_MPLS_ENTRYm_field32_get(unit, ment, EXP_MAPPING_PTRf);
        info->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_kt_mpls_port_stat_get_table_info
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
bcm_error_t _bcm_kt_mpls_port_stat_get_table_info(
            int                        unit,
            bcm_vpn_t                  vpn,
            bcm_gport_t                port,
            uint32                     *num_of_tables,
            bcm_stat_flex_table_info_t *table_info)
{
    int                   vp=0, rv = BCM_E_NONE;
    ing_dvp_table_entry_t dvp={{0}};
    int                   nh_index=0;
    initial_prot_nhi_table_entry_t prot_entry;
    int vpless_failover_port = FALSE;

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
           if (vpless_failover_port == TRUE) {
               rv = READ_INITIAL_PROT_NHI_TABLEm(unit, MEM_BLOCK_ANY, nh_index,
                                                 &prot_entry);
               if (BCM_FAILURE(rv)) {
                   bcm_tr_mpls_unlock (unit);
                   return rv;
               }
               nh_index = soc_INITIAL_PROT_NHI_TABLEm_field32_get(unit,
                              &prot_entry, PROT_NEXT_HOP_INDEXf);
           }
           table_info[*num_of_tables].table= EGR_L3_NEXT_HOPm;
           table_info[*num_of_tables].index=nh_index;
           table_info[*num_of_tables].direction=bcmStatFlexDirectionEgress;
           (*num_of_tables)++;
        }
        bcm_tr_mpls_unlock (unit);
        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      _bcm_kt_mpls_label_stat_get_table_info
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
bcm_error_t _bcm_kt_mpls_label_stat_get_table_info(
            int                        unit,
            bcm_mpls_label_t           label,
            bcm_gport_t                port,
            bcm_stat_object_t          object,
            uint32                     *num_of_tables,
            bcm_stat_flex_table_info_t *table_info)
{
    bcm_error_t              rv=BCM_E_NOT_FOUND;
    bcm_mpls_tunnel_switch_t mpls_tunnel_switch={0};
    mpls_entry_entry_t       ment={{0}};
    int                      index[2] = {0, 0};
    soc_mem_t                mem[2] = {MPLS_ENTRYm, L3_TUNNELm};
    int                      i, num_found = 0;
    (*num_of_tables)=0;
    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    if (soc_feature(unit, soc_feature_mpls)) {
        mpls_tunnel_switch.port = port;
        if (BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
            mpls_tunnel_switch.label = label;
        } else {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(bcm_tr_mpls_lock(unit));
        if (object != bcmStatObjectIngMplsFrrLabel) {
            rv = _bcm_kt_mpls_entry_set_key(unit, &mpls_tunnel_switch, &ment);
            if (rv == BCM_E_NONE) {
                if ((rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY,
                                &index[num_found], &ment, &ment, 0)) == BCM_E_NONE) {
                    mem[num_found] = MPLS_ENTRYm;
                    num_found++;
                }
            }
        }

#ifdef BCM_KATANA2_SUPPORT
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
#endif
        bcm_tr_mpls_unlock(unit);
    }

    if (num_found && (rv == BCM_E_NOT_FOUND)) {
        rv = BCM_E_NONE;
    }

    for (i = 0; i < num_found; i++) {
        table_info[*num_of_tables].table = mem[i];
        table_info[*num_of_tables].index = index[i];
        table_info[*num_of_tables].direction = bcmStatFlexDirectionIngress;
        (*num_of_tables)++;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_kt_egress_object_p2mp_set
 * Purpose:
 *      Set P2MP entry type within Egress Object entry
 * Parameters:
 *      unit - Device Number
 *      nh_index - Next Hop Index
 *      flag - Indicates SWAP or PHP
 *      value - Value to set
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_kt_egress_object_p2mp_set(int unit, bcm_multicast_t  mc_group, int flag, uint32 value)
{
    int rv=BCM_E_UNAVAIL;
    egr_l3_next_hop_entry_t egr_nh;
    int num_encap_id=0, num_sub_encap_id = 0, num_total_encap_id = 0;
    bcm_if_t *encap_id_array = NULL;
    bcm_if_t *sub_encap_id_array = NULL;
    bcm_if_t encap_id;
    int *q_array = NULL;
    int i;
    int nh_index;
    int if_max = BCM_MULTICAST_PORT_MAX;


    /* Get the number of encap IDs of the multicast group */
    rv = bcm_esw_multicast_egress_get(unit, mc_group, 0,
                                      NULL, NULL, &num_encap_id);
    if (rv < 0) {
        goto cleanup;
    }

    if (num_encap_id != 0) {
        encap_id_array = sal_alloc(sizeof(bcm_if_t) * num_encap_id,
                "encap_id_array");
        if (NULL == encap_id_array) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(encap_id_array, 0, sizeof(bcm_if_t) * num_encap_id);

        /* Get all the encap IDs of the multicast group */
        rv = bcm_esw_multicast_egress_get(unit, mc_group, num_encap_id,
                NULL, encap_id_array, &num_encap_id);
        if (rv < 0) {
            goto cleanup;
        }
    }

    q_array = sal_alloc(if_max * sizeof(bcm_if_t),
                            "mcast q array");
    if (q_array == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    sub_encap_id_array = sal_alloc(if_max * sizeof(bcm_if_t),
                            "mcast encap array");
    if (sub_encap_id_array == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }


    sal_memset(sub_encap_id_array, 0, sizeof(bcm_if_t) * if_max);
    /* Get the number of subscriber encap IDs of the multicast group */
    rv = bcm_esw_multicast_egress_subscriber_get(unit, mc_group, (if_max - 1),
            NULL, sub_encap_id_array, q_array, &num_sub_encap_id);
    if (rv < 0) {
        goto cleanup;
    }

    num_total_encap_id = num_encap_id + num_sub_encap_id;

    /* Convert encap IDs to next hop indices */

    for (i = 0; i < num_total_encap_id; i++) {
        encap_id = (i < num_encap_id) ? encap_id_array[i] :
            sub_encap_id_array[(i - num_encap_id)];
        if (BCM_IF_INVALID != (uint32)encap_id) {
            nh_index = encap_id - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
            rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm,
                                  MEM_BLOCK_ANY, nh_index, &egr_nh);
            if (rv < 0) {
                goto cleanup;
            }

            switch (flag) {
                 case BCM_MPLS_SWITCH_ACTION_SWAP:
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                            MPLS__LABEL_ACTION_SWAPf, value);
                        break;

                 case BCM_MPLS_SWITCH_ACTION_PHP:
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                            MPLS__LABEL_ACTION_PHPf, value);
                        break;

                 default:
                        break;
            }
            rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                            MEM_BLOCK_ALL, nh_index, &egr_nh);
            if (rv < 0) {
                goto cleanup;
            }
        }
    }

cleanup:
    if (sub_encap_id_array) {
        sal_free(sub_encap_id_array);
    }
    if (q_array) {
        sal_free(q_array);
    }
    if (encap_id_array) {
        sal_free(encap_id_array);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_kt_mpls_process_swap_label_action
 * Purpose:
 *      Process tunnel_switch.action = SWAP
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_kt_mpls_process_swap_label_action(int unit, bcm_mpls_tunnel_switch_t *info,
                                                                                     int tunnel_switch_update,
                                                                                     int forwarding_field_type,
                                                                                     int *nh_index,
                                                                                     mpls_entry_entry_t *ment )
{
    int rv = BCM_E_NONE;
    uint32 mpath_flag=0;
    int mc_index = -1;

    if (info->flags & BCM_MPLS_SWITCH_P2MP) {
        if ((tunnel_switch_update == 1) && 
              (forwarding_field_type == _BCM_MPLS_FORWARD_MULTICAST_GROUP) ) {
                /* Case of Leaf_LSR + Branch_LSR => BUD_LSR */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_NOT_BOSf,
                                0x0); /* INVALID */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_BOSf,
                                0x3); /* SWAP */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_SECOND_PASS_ACTION_VALIDf,
                                0x1);
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_SECOND_PASS_ACTION_IF_BOSf,
                                0x2); /*POP_USE_L3_IIF */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_SECOND_PASS_ACTION_IF_NOT_BOSf,
                                0x0); /* INVALID */
            if (_BCM_MULTICAST_IS_SET(info->mc_group)) {
                mc_index = _BCM_MULTICAST_ID_GET(info->mc_group);
                soc_MPLS_ENTRYm_field32_set(unit, ment, FORWARDING_FIELDf, 
                                mc_index);
            } else {
                return BCM_E_PARAM;
            }
            soc_MPLS_ENTRYm_field32_set(unit, ment,  FORWARDING_FIELD_TYPEf, 
                                _BCM_MPLS_FORWARD_MULTICAST_GROUP);
            rv = _bcm_kt_mpls_p2mp_loopback_enable(unit);
            BCM_IF_ERROR_RETURN(rv);
        } else {
            /* Case of P2MP BRANCH_LSR */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_NOT_BOSf,
                                0x0); /* INVALID */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_BOSf,
                                0x3); /* SWAP NHI */
            if (_BCM_MULTICAST_IS_SET(info->mc_group)) {
                mc_index = _BCM_MULTICAST_ID_GET(info->mc_group);
                soc_MPLS_ENTRYm_field32_set(unit, ment, FORWARDING_FIELDf, 
                                mc_index);
            } else {
                return BCM_E_PARAM;
            }
            soc_MPLS_ENTRYm_field32_set(unit, ment,  FORWARDING_FIELD_TYPEf, 
                                _BCM_MPLS_FORWARD_MULTICAST_GROUP);
        }
        rv = _bcm_kt_egress_object_p2mp_set(unit, info->mc_group, 
                             BCM_MPLS_SWITCH_ACTION_SWAP, 0x1);
    } else {

        if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->egress_if)) {
            if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, info->egress_if)) {
                return BCM_E_PARAM;
            }
        }

        /* Case of LSR */
        soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_NOT_BOSf,
                                0x3); /* SWAP_NHI */
        soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_BOSf,
                                0x3); /* SWAP NHI */
        if (BCM_XGS3_L3_MPLS_LBL_VALID(info->egress_label.label)) {         
            rv = bcm_tr_mpls_l3_nh_info_add(unit, info, nh_index);
            if (rv < 0) {
                return rv;
            }
            soc_MPLS_ENTRYm_field32_set(unit, ment, NEXT_HOP_INDEXf, *nh_index);
            soc_MPLS_ENTRYm_field32_set(unit, ment,  FORWARDING_FIELD_TYPEf, 
                                                               _BCM_MPLS_FORWARD_NEXT_HOP);
        } else {
             rv = bcm_xgs3_get_nh_from_egress_object(unit, info->egress_if,
                                                    &mpath_flag, 1, nh_index);
             if (rv < 0) {
                 return rv;
             }
             if (mpath_flag == BCM_L3_MULTIPATH) {
                  soc_MPLS_ENTRYm_field32_set(unit, ment, ECMP_PTRf, *nh_index);
                  soc_MPLS_ENTRYm_field32_set(unit, ment,  FORWARDING_FIELD_TYPEf, 
                                                                     _BCM_MPLS_FORWARD_ECMP_GROUP);
             } else {
                  soc_MPLS_ENTRYm_field32_set(unit, ment, NEXT_HOP_INDEXf, *nh_index);
                  soc_MPLS_ENTRYm_field32_set(unit, ment,  FORWARDING_FIELD_TYPEf, 
                                                                     _BCM_MPLS_FORWARD_NEXT_HOP);
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

int
_bcm_kt_mpls_process_pop_label_action(int unit, bcm_mpls_tunnel_switch_t *info,
                                                                                     int tunnel_switch_update,
                                                                                     int forwarding_field_type,
                                                                                     int old_action,
                                                                                     mpls_entry_entry_t *ment )
{
    int rv = BCM_E_NONE;
    int  vrf=-1, mode=-1;

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
        if ((tunnel_switch_update == 1) && 
              (forwarding_field_type == _BCM_MPLS_FORWARD_MULTICAST_GROUP) ) {
            /* Case of Branch_LSR + Leaf_LSR => BUD_LSR */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_NOT_BOSf,
                                0x0); /* INVALID */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_BOSf,
                                old_action); /* SWAP OR PHP */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_SECOND_PASS_ACTION_VALIDf,
                                0x1);
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_SECOND_PASS_ACTION_IF_BOSf,
                                0x2); /*POP_USE_L3_IIF */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_SECOND_PASS_ACTION_IF_NOT_BOSf,
                                0x0); /* INVALID */
            if (!mode) {
                 soc_MPLS_ENTRYm_field32_set(unit, ment, L3_IIF_2f, 
                                            _BCM_TR_MPLS_L3_IIF_BASE + vrf);
            } else {
                 soc_MPLS_ENTRYm_field32_set(unit, ment, L3_IIF_2f, info->ingress_if);
            }
            rv = _bcm_kt_mpls_p2mp_loopback_enable(unit);
        } else {
            /* Case of LEAF_LSR */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_NOT_BOSf,
                                0x0); /* INVALID */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_BOSf,
                                0x2); /* L3_IIF */
            if (!mode) {
                 soc_MPLS_ENTRYm_field32_set(unit, ment, L3_IIFf, 
                                            _BCM_TR_MPLS_L3_IIF_BASE + vrf);
            } else {
                 soc_MPLS_ENTRYm_field32_set(unit, ment, L3_IIFf, info->ingress_if);
            }
        }
    } else {
        /* Case of LSR */
        soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_NOT_BOSf,
                                0x1); /* POP */
        soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_BOSf,
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

            soc_MPLS_ENTRYm_field32_set(unit, ment, L3_IIFf,
                    _BCM_TR_MPLS_L3_IIF_BASE + vrf);
        } else {
             soc_MPLS_ENTRYm_field32_set(unit, ment, L3_IIFf, info->ingress_if);
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

int
_bcm_kt_mpls_process_php_label_action(int unit, bcm_mpls_tunnel_switch_t *info,
                                                                                     int tunnel_switch_update,
                                                                                     int forwarding_field_type,
                                                                                     int *nh_index,
                                                                                     mpls_entry_entry_t *ment )
{
    int rv = BCM_E_NONE;
    uint32 mpath_flag=0;
    int mc_index=-1;


    if (info->flags & BCM_MPLS_SWITCH_P2MP) {
        if ((tunnel_switch_update == 1) && 
              (forwarding_field_type == _BCM_MPLS_FORWARD_MULTICAST_GROUP) ) {
                /* Case of Leaf_LSR + Branch_LSR => BUD_LSR */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_NOT_BOSf,
                                0x0); /* INVALID */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_BOSf,
                                0x4); /* PHP */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_SECOND_PASS_ACTION_VALIDf,
                                0x1);
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_SECOND_PASS_ACTION_IF_BOSf,
                                0x2); /*POP_USE_L3_IIF */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_SECOND_PASS_ACTION_IF_NOT_BOSf,
                                0x0); /* INVALID */
            if (_BCM_MULTICAST_IS_SET(info->mc_group)) {
                mc_index = _BCM_MULTICAST_ID_GET(info->mc_group);
                soc_MPLS_ENTRYm_field32_set(unit, ment, FORWARDING_FIELDf, 
                                mc_index);
            } else {
                return BCM_E_PARAM;
            }
            soc_MPLS_ENTRYm_field32_set(unit, ment,  FORWARDING_FIELD_TYPEf, 
                                _BCM_MPLS_FORWARD_MULTICAST_GROUP);
            rv = _bcm_kt_mpls_p2mp_loopback_enable(unit);
            BCM_IF_ERROR_RETURN(rv);
        } else {
            /* Case of P2MP BRANCH_LSR */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_NOT_BOSf,
                                0x0); /* INVALID */
            soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_BOSf,
                                0x4); /* PHP NHI */
            if (_BCM_MULTICAST_IS_SET(info->mc_group)) {
                mc_index = _BCM_MULTICAST_ID_GET(info->mc_group);
                soc_MPLS_ENTRYm_field32_set(unit, ment, FORWARDING_FIELDf, 
                                mc_index);
            } else {
                return BCM_E_PARAM;
            }
            soc_MPLS_ENTRYm_field32_set(unit, ment,  FORWARDING_FIELD_TYPEf, 
                                _BCM_MPLS_FORWARD_MULTICAST_GROUP);
        }
        rv = bcm_xgs3_get_nh_from_egress_object(unit, info->egress_if,
                                                    &mpath_flag, 1, nh_index);
        if (rv < 0) {
             return rv;
        }
        rv = _bcm_kt_egress_object_p2mp_set(unit, info->mc_group, 
                             BCM_MPLS_SWITCH_ACTION_PHP, 0x1);

    } else {

        if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->egress_if)) {
            if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, info->egress_if)) {
                return BCM_E_PARAM;
            }
        }
        soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_NOT_BOSf,
                                    0x2); /* PHP_NHI */
        soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_ACTION_IF_BOSf,
                                    0x4); /* PHP_NHI */ 
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
            soc_MPLS_ENTRYm_field32_set(unit, ment, ECMP_PTRf, *nh_index);
            soc_MPLS_ENTRYm_field32_set(unit, ment,  FORWARDING_FIELD_TYPEf, 
                                                                     _BCM_MPLS_FORWARD_ECMP_GROUP);
        } else {
            soc_MPLS_ENTRYm_field32_set(unit, ment, NEXT_HOP_INDEXf, *nh_index);
            soc_MPLS_ENTRYm_field32_set(unit, ment,  FORWARDING_FIELD_TYPEf, 
                                                                     _BCM_MPLS_FORWARD_NEXT_HOP);
        }
    }
    return rv;
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
bcm_kt_mpls_tunnel_switch_add(int unit, bcm_mpls_tunnel_switch_t *info)
{
    mpls_entry_entry_t ment;
    int mode=0, nh_index = -1, rv, num_pw_term, old_pw_cnt = -1, pw_cnt = -1;
    int index, forwarding_field_type=-1, old_action = -1;
    int old_nh_index = -1, old_ecmp_index = -1, old_ipmc_index=-1;
    int  tunnel_switch_update=0;
    int  ref_count=0;
    bcm_if_t  egress_if=0;

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
#ifdef  BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) { 
        if (info->flags & BCM_MPLS_SWITCH_FRR) {
            rv = _bcm_tr3_mpls_tunnel_switch_frr_set(unit, info);
            return rv;
         }
    }
#endif /* BCM_KATANA2_SUPPORT */
    BCM_IF_ERROR_RETURN(_bcm_kt_mpls_entry_set_key(unit, info, &ment));

    /* See if entry already exists */
    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        &ment, &ment, 0);

    if (rv == SOC_E_NONE) {
        /* Entry exists, save old info */
        tunnel_switch_update = 1;
        old_action = soc_MPLS_ENTRYm_field32_get(unit, &ment, MPLS_ACTION_IF_BOSf);
        forwarding_field_type =  soc_MPLS_ENTRYm_field32_get(unit, &ment,  FORWARDING_FIELD_TYPEf);
        if ((old_action == 0x3) || (old_action == 0x4)) {
            if (forwarding_field_type == _BCM_MPLS_FORWARD_NEXT_HOP) {
                 old_nh_index = soc_MPLS_ENTRYm_field32_get(unit, &ment, NEXT_HOP_INDEXf);
            } else if (forwarding_field_type == _BCM_MPLS_FORWARD_ECMP_GROUP) {
                 old_ecmp_index = soc_MPLS_ENTRYm_field32_get(unit, &ment, ECMP_PTRf);
            }  else if (forwarding_field_type == _BCM_MPLS_FORWARD_MULTICAST_GROUP) {
                 old_ipmc_index = soc_MPLS_ENTRYm_field32_get(unit, &ment, FORWARDING_FIELDf);
                 
                 if (old_ipmc_index) {
                 }
            }
        }
        if (soc_MPLS_ENTRYm_field32_get(unit, &ment, PW_TERM_NUM_VALIDf)) {
            old_pw_cnt = soc_MPLS_ENTRYm_field32_get(unit, &ment, PW_TERM_NUMf);
        } 
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    }

    switch(info->action) {
         case BCM_MPLS_SWITCH_ACTION_SWAP:
              rv = _bcm_kt_mpls_process_swap_label_action(unit, info,
                             tunnel_switch_update, forwarding_field_type, &nh_index, &ment );
              if (rv < 0) {
                   goto cleanup;
              }
              break;

         case BCM_MPLS_SWITCH_ACTION_POP:
              rv = _bcm_kt_mpls_process_pop_label_action(unit, info,
                             tunnel_switch_update, forwarding_field_type, old_action, &ment );
              if (rv < 0) {
                   goto cleanup;
              }
              break;

         case BCM_MPLS_SWITCH_ACTION_PHP:
              rv = _bcm_kt_mpls_process_php_label_action(unit, info,
                             tunnel_switch_update, forwarding_field_type, &nh_index, &ment );
              if (rv < 0) {
                   goto cleanup;
              }
              break;

         default:
              return BCM_E_PARAM;
              break;
    }

    
    soc_MPLS_ENTRYm_field32_set(unit, &ment, V4_ENABLEf, 1);
    soc_MPLS_ENTRYm_field32_set(unit, &ment, V6_ENABLEf, 1);
    if (info->flags & BCM_MPLS_SWITCH_INNER_TTL) {
        if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_TTLf, 0);
    } else {
        soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_TTLf, 1);
    }
    if (info->flags & BCM_MPLS_SWITCH_INNER_EXP) {
        if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_EXP_FOR_INNERf, 0);
    } else {
        /* For SWAP, Do-not PUSH EXP */
        if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_EXP_FOR_INNERf, 0);
        } else {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_EXP_FOR_INNERf, 1);
        }
    }

    (void) bcm_tr_mpls_entry_internal_qos_set(unit, NULL, info, &ment);

    if ((info->flags & BCM_MPLS_SWITCH_COUNTED)) {
       if (SOC_MEM_IS_VALID(unit, ING_PW_TERM_COUNTERSm)) {
        if (old_pw_cnt == -1) {
            num_pw_term = soc_mem_index_count(unit, ING_PW_TERM_COUNTERSm);
            for (pw_cnt = 0; pw_cnt < num_pw_term; pw_cnt++) {
                if (! bcm_tr_mpls_pw_used_get(unit, pw_cnt)) {
                    break;
                }
            }
            if (pw_cnt == num_pw_term) {
                rv = BCM_E_RESOURCE;
                goto cleanup;
            }
            (void) bcm_tr_mpls_pw_used_set(unit, pw_cnt);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, PW_TERM_NUMf, pw_cnt);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, PW_TERM_NUM_VALIDf, 1);
        }
      }
    }

    if (SOC_MEM_FIELD_VALID(unit, MPLS_ENTRYm, MPLS__DROP_DATA_ENABLEf)) {
         soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__DROP_DATA_ENABLEf,
            (info->flags & BCM_MPLS_SWITCH_DROP) ? 1 : 0);
    }

    if (SOC_MEM_FIELD_VALID(unit, MPLS_ENTRYm, MPLS__RX_PROT_GROUPf)) {
        if ((info->failover_id >= 0) &&
            (info->failover_id <= soc_mem_index_max(unit, RX_PROT_GROUP_TABLEm))) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS__RX_PROT_GROUPf,
                info->failover_id);
        }
    }

    if (!tunnel_switch_update) {
        rv = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &ment);
    } else {
        rv = soc_mem_write(unit, MPLS_ENTRYm,
                           MEM_BLOCK_ALL, index,
                           &ment);
    }

    if (rv < 0) {
        goto cleanup;
    }

    if ((tunnel_switch_update) && 
              (forwarding_field_type != _BCM_MPLS_FORWARD_MULTICAST_GROUP)) {
        /* Clean up old next-hop and counter info if entry was replaced */
        if ((old_pw_cnt != -1) && !(info->flags & BCM_MPLS_SWITCH_COUNTED)) {
           (void) bcm_tr_mpls_pw_used_clear(unit, old_pw_cnt);
        }
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
            } else {
                if (forwarding_field_type == _BCM_MPLS_FORWARD_ECMP_GROUP) {
                    rv = bcm_xgs3_ecmp_group_del(unit, old_ecmp_index, 0);
                }
            }
        } else if (old_action == 0x4) {/* PHP_NHI */
            /* L3_NHI */
            if (forwarding_field_type == _BCM_MPLS_FORWARD_NEXT_HOP) {
                rv = bcm_xgs3_nh_del(unit, 0, old_nh_index);
            } else if (forwarding_field_type == _BCM_MPLS_FORWARD_ECMP_GROUP) {
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
    if (pw_cnt != -1) {
        (void) bcm_tr_mpls_pw_used_clear(unit, pw_cnt);
    }
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
 *      bcm_mpls_tunnel_switch_delete
 * Purpose:
 *      Delete an MPLS label entry.
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt_mpls_tunnel_switch_delete(int unit, bcm_mpls_tunnel_switch_t *info)
{
    int rv, index;
    mpls_entry_entry_t ment;
#ifdef  BCM_KATANA2_SUPPORT
    if ((SOC_IS_KATANA2(unit)) && (info->flags & BCM_MPLS_SWITCH_FRR)) { 
        rv = _bcm_tr3_mpls_tunnel_switch_frr_delete(unit, info);
    } else
#endif /*BCM_KATANA2_SUPPORT*/
    {
        rv = _bcm_kt_mpls_entry_set_key(unit, info, &ment);
        BCM_IF_ERROR_RETURN(rv);

        rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                &ment, &ment, 0);
        if (rv < 0) {
            return rv;
        }
        rv = _bcm_kt_mpls_entry_delete(unit, &ment, info, index);
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
 *      bcm_mpls_tunnel_switch_delete_all
 * Purpose:
 *      Delete all MPLS label entries.
 * Parameters:
 *      unit   - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt_mpls_tunnel_switch_delete_all(int unit)
{
    int rv, i, j, num_entries;
    mpls_entry_entry_t ment;
    bcm_mpls_tunnel_switch_t info;
    soc_tunnel_term_t tnl_entry;
    int num_del = 0;

    

    num_entries = soc_mem_index_count(unit, MPLS_ENTRYm);
    for (i = 0; i < num_entries; i++) {

        rv = READ_MPLS_ENTRYm(unit, MEM_BLOCK_ANY, i, &ment);
        if (rv < 0) {
            return rv;
        }
        if (!soc_MPLS_ENTRYm_field32_get(unit, &ment, VALIDf)) {
            continue;
        }
        if (soc_MPLS_ENTRYm_field32_get(unit, &ment,
                                        MPLS_ACTION_IF_BOSf) == 0x1) {
            /* L2_SVP */
            continue;
        }
        sal_memset(&info, 0, sizeof(bcm_mpls_tunnel_switch_t));

        /* Delete entries by default only once unless
         * its a P2MP entry with second pass action valid
         */
        num_del = 1;

        rv = _bcm_kt_mpls_entry_get_key(unit, &ment, &info);
        if (rv < 0) {
            return rv;
        }
        rv = _bcm_kt_mpls_entry_get_data(unit, &ment, &info);
        if (rv < 0) {
            return rv;
        }

        /* Check if entry is a P2MP entry with second pass action valid */
        if ((soc_MPLS_ENTRYm_field32_get(unit, &ment, FORWARDING_FIELD_TYPEf))
                                                == _BCM_MPLS_FORWARD_MULTICAST_GROUP) {
            if(soc_MPLS_ENTRYm_field32_get(unit, &ment,
                                           MPLS_SECOND_PASS_ACTION_VALIDf)) {
                num_del  = 2;
            }
        }

        for (j=0 ; j < num_del; j++) {
            rv = _bcm_kt_mpls_entry_delete(unit, &ment, &info, i );
            if (rv < 0) {
                return rv;
            }
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
         * deleted entry position using _soc_tunnel_term_entry_shift().
         * Therefore adding this index change to support delete all*/
        i--;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_get
 * Purpose:
 *      Get an MPLS label entry.
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt_mpls_tunnel_switch_get(int unit, bcm_mpls_tunnel_switch_t *info)
{
    int rv, index;
    mpls_entry_entry_t ment;
#ifdef  BCM_KATANA2_SUPPORT
    if ((SOC_IS_KATANA2(unit)) && (info->flags & BCM_MPLS_SWITCH_FRR)) { 
        rv = _bcm_tr3_mpls_tunnel_switch_frr_get(unit, info, &index);
    } else
#endif /*BCM_KATANA2_SUPPORT*/
    {
        rv = _bcm_kt_mpls_entry_set_key(unit, info, &ment);


        BCM_IF_ERROR_RETURN(rv);

        rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                &ment, &ment, 0);

        if (rv < 0) {
            return rv;
        }
        rv = _bcm_kt_mpls_entry_get_data(unit, &ment, info);
    }
    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_traverse
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
bcm_kt_mpls_tunnel_switch_traverse(int unit, 
                                   bcm_mpls_tunnel_switch_traverse_cb cb,
                                   void *user_data)
{
    int rv, i, num_entries;
    mpls_entry_entry_t ment;
    bcm_mpls_tunnel_switch_t info;
#ifdef  BCM_KATANA2_SUPPORT
    soc_tunnel_term_t tnl_entry;
    uint32 key_type=0;
#endif
    

    num_entries = soc_mem_index_count(unit, MPLS_ENTRYm);
    for (i = 0; i < num_entries; i++) {
        BCM_IF_ERROR_RETURN (READ_MPLS_ENTRYm(unit, MEM_BLOCK_ANY, i, &ment));
        if (!soc_MPLS_ENTRYm_field32_get(unit, &ment, VALIDf)) {
            continue;
        }
        if (soc_MPLS_ENTRYm_field32_get(unit, &ment,
                                        MPLS_ACTION_IF_BOSf) == 0x1) {
            /* L2_SVP */
            continue;
        }
        sal_memset(&info, 0, sizeof(bcm_mpls_tunnel_switch_t));
        rv = _bcm_kt_mpls_entry_get_key(unit, &ment, &info);
        if (rv < 0) {
            return rv;
        }
        rv = _bcm_kt_mpls_entry_get_data(unit, &ment, &info);
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
#ifdef  BCM_KATANA2_SUPPORT
    if ((SOC_IS_KATANA2(unit))) { 

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

            rv = cb(unit, &info, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#endif
        }
    }
#endif /*BCM_KATANA2_SUPPORT*/

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt_mpls_port_stat_attach
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
bcm_error_t bcm_kt_mpls_port_stat_attach(
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
                  unit,
                  stat_counter_id,
                  &group_mode,&object,&offset_mode,&pool_number,&base_index);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,object,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit,group_mode));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit,object,1,&actual_num_tables,&table,&direction));

    BCM_IF_ERROR_RETURN(_bcm_kt_mpls_port_stat_get_table_info(
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
 *      bcm_kt_mpls_port_stat_detach
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
bcm_error_t bcm_kt_mpls_port_stat_detach(
                   int         unit,
                   bcm_vpn_t   vpn,
                   bcm_gport_t port)
{
    uint32                     count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};

    BCM_IF_ERROR_RETURN(_bcm_kt_mpls_port_stat_get_table_info(
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
                        unit, 0, table_info[count].table,
                        table_info[count].index);
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
 *      bcm_kt_mpls_port_stat_counter_get
 * Description:
 *      Get counter statistic values for specific vpn and gport
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - if sync sw accumulated count with hw count 
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
bcm_error_t bcm_kt_mpls_port_stat_counter_get(
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
    
    BCM_IF_ERROR_RETURN(_bcm_kt_mpls_port_stat_get_table_info(
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
 *      bcm_kt_mpls_port_stat_counter_set
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
bcm_error_t bcm_kt_mpls_port_stat_counter_set(
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
    
    BCM_IF_ERROR_RETURN(_bcm_kt_mpls_port_stat_get_table_info(
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
 *      bcm_kt_mpls_port_stat_id_get
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
bcm_error_t bcm_kt_mpls_port_stat_id_get(
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
    BCM_IF_ERROR_RETURN(_bcm_kt_mpls_port_stat_get_table_info(
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
 *      bcm_kt_mpls_label_stat_attach
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
bcm_error_t bcm_kt_mpls_label_stat_attach(
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


    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode,&object,&offset_mode,&pool_number,&base_index);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,object,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit,group_mode));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit,object,1,&actual_num_tables,&table,&direction));

    BCM_IF_ERROR_RETURN(_bcm_kt_mpls_label_stat_get_table_info(
                   unit, label, port, object, &num_of_tables,&table_info[0]));
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
 *      bcm_kt_mpls_label_stat_detach
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
bcm_error_t bcm_kt_mpls_label_stat_detach(
                   int              unit,
                   bcm_mpls_label_t label,
                   bcm_gport_t      port)
{
    uint32                     count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_kt_mpls_label_stat_get_table_info(
                unit, label, port, bcmStatObjectMaxValue,
                &num_of_tables,&table_info[0]));

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
                        unit, 0, table_info[count].table,
                        table_info[count].index);
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
 *      bcm_kt_mpls_label_stat_counter_get
 * Description:
 *      Get counter statistic values for specific MPLS label and gport
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - if sync sw accumulated count with hw count 
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
bcm_error_t bcm_kt_mpls_label_stat_counter_get(
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

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

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
    
    BCM_IF_ERROR_RETURN(_bcm_kt_mpls_label_stat_get_table_info(
                unit, label, port, bcmStatObjectMaxValue,
                &num_of_tables, &table_info[0]));

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
 *      bcm_kt_mpls_label_stat_counter_set
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
bcm_error_t bcm_kt_mpls_label_stat_counter_set(
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

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
        return BCM_E_PARAM;
    }

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
        /* Just create a valid vpn id to pass the check */
        _BCM_MPLS_VPN_SET(vpn, _BCM_MPLS_VPN_TYPE_VPWS, 1);
        BCM_IF_ERROR_RETURN(_bcm_kt_mpls_port_stat_get_table_info(
            unit, vpn, port, &num_of_tables, &table_info[0]));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_kt_mpls_label_stat_get_table_info(
                    unit, label, port, bcmStatObjectMaxValue,
                    &num_of_tables, &table_info[0]));
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
 *      bcm_kt_mpls_label_stat_id_get
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
bcm_error_t bcm_kt_mpls_label_stat_id_get(
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
    BCM_IF_ERROR_RETURN(_bcm_kt_mpls_label_stat_get_table_info(
                unit, label, port, bcmStatObjectMaxValue,
                &num_of_tables,&table_info[0]));
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
 *      bcm_kt_mpls_label_stat_enable_set
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
int bcm_kt_mpls_label_stat_enable_set(
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
        BCM_IF_ERROR_RETURN(_bcm_kt_mpls_port_stat_get_table_info(
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
                BCM_IF_ERROR_RETURN(bcm_kt_mpls_port_stat_attach(
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
            BCM_IF_ERROR_RETURN(bcm_kt_mpls_port_stat_detach(unit,vpn,port));
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

    BCM_IF_ERROR_RETURN(_bcm_kt_mpls_label_stat_get_table_info(
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
 *      bcm_kt_mpls_label_stat_get
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

int bcm_kt_mpls_label_stat_get(int              unit,
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

        BCM_IF_ERROR_RETURN(bcm_kt_mpls_port_stat_counter_get(unit, sync_mode, 
                                                              vpn, port, 
                                                              stat, 1,
                                                              &counter_indexes,
                                                              &counter_values));
    } else {
        if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(bcm_kt_mpls_label_stat_counter_get(unit, sync_mode, 
                                                               label, port, 
                                                               stat, 1,
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
 *      bcm_kt_mpls_label_stat_get32
 * Purpose:
 *      Get L2 MPLS PW Stats
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 * Parameters:
 *      unit       - (IN) SOC unit #
 *      sync_mode  - (IN) hwcount is to be synced to sw count 
 *      label      - (IN) MPLS label
 *      port       - (IN) MPLS gport
 *      stat       - (IN)  specify the Stat type
 *      val        - (OUT) 32-bit Stats value
 * Returns:
 *      BCM_E_XXX
 */

int bcm_kt_mpls_label_stat_get32(int              unit,
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

        BCM_IF_ERROR_RETURN(bcm_kt_mpls_port_stat_counter_get(unit, sync_mode, 
                                                              vpn, port, 
                                                              stat, 1,
                                                              &counter_indexes,
                                                              &counter_values));
    } else {
        if (!BCM_XGS3_L3_MPLS_LBL_VALID(label)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(_bcm_esw_mpls_label_stat_counter_get(unit, 
                                                                 sync_mode,
                                                                 label, port,
                                                                 stat, 1,
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
 *      bcm_kt_mpls_label_stat_clear
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

int bcm_kt_mpls_label_stat_clear(
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




#else /* BCM_KATANA_SUPPORT && BCM_MPLS_SUPPORT && INCLUDE_L3 */
typedef int bcm_esw_katana_mpls_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_KATANA_SUPPORT && BCM_MPLS_SUPPORT && INCLUDE_L3 */

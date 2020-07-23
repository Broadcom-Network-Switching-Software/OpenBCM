/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    mpls.c
 * Purpose: Manages Tomahawk, TD2+ MPLS functions
 */
#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#ifdef INCLUDE_L3
#ifdef BCM_MPLS_SUPPORT
#if (defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT))

#include <soc/drv.h>
#include <soc/hash.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <soc/triumph.h>
#include <soc/format.h>

#include <bcm/error.h>
#include <bcm/mpls.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/ecn.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw/failover.h>
#ifdef BCM_TOMAHAWK2_SUPPORT
#include <bcm_int/esw/tomahawk2.h>
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <bcm_int/esw/tomahawk3.h>
#endif
#ifdef BCM_TRIDENT3_SUPPORT
#include <soc/esw/cancun.h>
#include <soc/esw/cancun_enums.h>
#endif /* BCM_TRIDENT3_SUPPORT */

#define MPLS_INFO(_unit_)   (&_bcm_tr_mpls_bk_info[_unit_])
#define L3_INFO(_unit_)   (&_bcm_l3_bk_info[_unit_])

#define bcmi_mpls_bookkeeping_t  _bcm_tr_mpls_bookkeeping_t

bcmi_egr_ip_tnl_mpls_tunnel_entry_t **egr_mpls_tnl_sw_state[BCM_MAX_NUM_UNITS];
bcmi_egr_ip_tnl_free_indexes_t fi_db[BCM_MAX_NUM_UNITS];

STATIC int
_bcmi_xgs5_mpls_entry_set_key(int unit, bcm_mpls_tunnel_switch_t *info,
                              mpls_entry_entry_t *ment);

/*
TD3 MPLS_ENTRY_X FORMAT:
----------------------------------------------------------------------------------------------------------
| MPLS_LABEL(0:19)                  | MPLS_PUSH_ACTION(20:21)          | MPLS_EXP_SELECT(22:23)           |
----------------------------------------------------------------------------------------------------------|
| MPLS_EXP_MAPPING_PTR(24:27)       | NEW_CFI(24:24)                   | NEW_PRI(25:27)                   |
----------------------------------------------------------------------------------------------------------|
| MPLS_EXP(30:28)                   | LABEL_SELECT(31:31)              | MPLS_TTL(32:39)                  |
----------------------------------------------------------------------------------------------------------|
| SPECIAL_LABEL_PUSH_TYPE(40:41)    | EXP_MAPPING_TABLE_SELECT(42:43)  | PKT_ECN_TO_EXP_MAPPING_PTR(44:45)|
----------------------------------------------------------------------------------------------------------|
| PKT_ECN_TO_EXP_PRIORITY(46:46)    | EGR_FLEX_CTR_ACTION_SET(47:65)   | FLEX_CTR_BASE_COUNTER_IDX(47:59) |
----------------------------------------------------------------------------------------------------------|
| FLEX_CTR_POOL_NUMBER(60:61)       | RSVD_FLEX_CTR_POOL_NUMBER(62:63) | FLEX_CTR_OFFSET_MODE(61:65)      |
----------------------------------------------------------------------------------------------------------|
| ECN_MAPPING_ENABLE(66:66)         | ECN_MAPPING_TABLE_SELECT(67:68)  | RESERVED(69:70)                  |
----------------------------------------------------------------------------------------------------------
*/
#ifdef BCM_TRIDENT3_SUPPORT
static soc_field_t _tnl_label_entry_f[] =
{ MPLS_ENTRY_0f, MPLS_ENTRY_1f, MPLS_ENTRY_2f, MPLS_ENTRY_3f,
  MPLS_ENTRY_4f, MPLS_ENTRY_5f, MPLS_ENTRY_6f, MPLS_ENTRY_7f };
#endif

static soc_field_t _tnl_label_f[] =
{ MPLS_LABEL_0f, MPLS_LABEL_1f, MPLS_LABEL_2f, MPLS_LABEL_3f,
  MPLS_LABEL_4f, MPLS_LABEL_5f, MPLS_LABEL_6f, MPLS_LABEL_7f };
static soc_field_t _tnl_push_action_f[] =
{ MPLS_PUSH_ACTION_0f, MPLS_PUSH_ACTION_1f,
  MPLS_PUSH_ACTION_2f, MPLS_PUSH_ACTION_3f,
  MPLS_PUSH_ACTION_4f, MPLS_PUSH_ACTION_5f,
  MPLS_PUSH_ACTION_6f, MPLS_PUSH_ACTION_7f };
static soc_field_t _tnl_exp_select_f[] =
{ MPLS_EXP_SELECT_0f, MPLS_EXP_SELECT_1f,
  MPLS_EXP_SELECT_2f, MPLS_EXP_SELECT_3f,
  MPLS_EXP_SELECT_4f, MPLS_EXP_SELECT_5f,
  MPLS_EXP_SELECT_6f, MPLS_EXP_SELECT_7f };
static soc_field_t _tnl_exp_ptr_f[] =
{ MPLS_EXP_MAPPING_PTR_0f, MPLS_EXP_MAPPING_PTR_1f,
  MPLS_EXP_MAPPING_PTR_2f, MPLS_EXP_MAPPING_PTR_3f,
  MPLS_EXP_MAPPING_PTR_4f, MPLS_EXP_MAPPING_PTR_5f,
  MPLS_EXP_MAPPING_PTR_6f, MPLS_EXP_MAPPING_PTR_7f };
static soc_field_t _tnl_exp_f[] =
{ MPLS_EXP_0f, MPLS_EXP_1f, MPLS_EXP_2f, MPLS_EXP_3f,
  MPLS_EXP_4f, MPLS_EXP_5f, MPLS_EXP_6f, MPLS_EXP_7f };
static soc_field_t _tnl_ttl_f[] =
{ MPLS_TTL_0f, MPLS_TTL_1f, MPLS_TTL_2f, MPLS_TTL_3f,
  MPLS_TTL_4f, MPLS_TTL_5f, MPLS_TTL_6f, MPLS_TTL_7f };
static soc_field_t _tnl_pri_f[] =
{ NEW_PRI_0f, NEW_PRI_1f, NEW_PRI_2f, NEW_PRI_3f,
  NEW_PRI_4f, NEW_PRI_5f, NEW_PRI_6f, NEW_PRI_7f };
static soc_field_t _tnl_cfi_f[] =
{ NEW_CFI_0f, NEW_CFI_1f, NEW_CFI_2f, NEW_CFI_3f,
  NEW_CFI_4f, NEW_CFI_5f, NEW_CFI_6f, NEW_CFI_7f };
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
static soc_field_t _tnl_ecn_ptr_f[] = 
{ IP_ECN_TO_EXP_MAPPING_PTR_0f, IP_ECN_TO_EXP_MAPPING_PTR_1f, 
  IP_ECN_TO_EXP_MAPPING_PTR_2f, IP_ECN_TO_EXP_MAPPING_PTR_3f,
  IP_ECN_TO_EXP_MAPPING_PTR_4f, IP_ECN_TO_EXP_MAPPING_PTR_5f, 
  IP_ECN_TO_EXP_MAPPING_PTR_6f, IP_ECN_TO_EXP_MAPPING_PTR_7f};
static soc_field_t _tnl_ecn_ptr_pri_f[] = 
{ IP_ECN_TO_EXP_PRIORITY_0f, IP_ECN_TO_EXP_PRIORITY_1f, 
  IP_ECN_TO_EXP_PRIORITY_2f, IP_ECN_TO_EXP_PRIORITY_3f,
  IP_ECN_TO_EXP_PRIORITY_4f, IP_ECN_TO_EXP_PRIORITY_5f, 
  IP_ECN_TO_EXP_PRIORITY_6f, IP_ECN_TO_EXP_PRIORITY_7f};
static soc_field_t _tnl_int_cn_ptr_f[] = 
{ INT_CN_TO_EXP_MAPPING_PTR_0f, INT_CN_TO_EXP_MAPPING_PTR_1f, 
  INT_CN_TO_EXP_MAPPING_PTR_2f, INT_CN_TO_EXP_MAPPING_PTR_3f,
  INT_CN_TO_EXP_MAPPING_PTR_4f, INT_CN_TO_EXP_MAPPING_PTR_5f, 
  INT_CN_TO_EXP_MAPPING_PTR_6f, INT_CN_TO_EXP_MAPPING_PTR_7f};
static soc_field_t _tnl_int_cn_ptr_pri_f[] = 
{ INT_CN_TO_EXP_PRIORITY_0f, INT_CN_TO_EXP_PRIORITY_1f, 
  INT_CN_TO_EXP_PRIORITY_2f, INT_CN_TO_EXP_PRIORITY_3f,
  INT_CN_TO_EXP_PRIORITY_4f, INT_CN_TO_EXP_PRIORITY_5f, 
  INT_CN_TO_EXP_PRIORITY_6f, INT_CN_TO_EXP_PRIORITY_7f};
#endif
/************************  FUNCTIONS BOUNDARY **********************/

STATIC soc_mem_t
bcmi_egr_ip_tnl_mem_name_get(int unit)
{
   soc_mem_t mem;

#if defined(BCM_TRIDENT3_SUPPORT)
   if (soc_feature(unit, soc_feature_td3_style_mpls)) {
       mem = EGR_IP_TUNNEL_MPLS_DOUBLE_WIDEm;
   } else
#endif
   {
       mem = EGR_IP_TUNNEL_MPLSm;
   }
   return mem;
}

STATIC int
bcmi_egr_ip_tnl_mpls_table_write(int unit,int tnl_id,void *tnl_buf)
{
  int rv = BCM_E_NONE;

#if defined(BCM_TRIDENT3_SUPPORT)
  if (soc_feature(unit, soc_feature_td3_style_mpls)) {
      rv = WRITE_EGR_IP_TUNNEL_MPLS_DOUBLE_WIDEm(unit, MEM_BLOCK_ANY,
              tnl_id, tnl_buf);
  } else
#endif
  {
      rv = WRITE_EGR_IP_TUNNEL_MPLSm(unit, MEM_BLOCK_ANY, tnl_id, tnl_buf);
  }
   return rv;
}


STATIC int
bcmi_egr_ip_tnl_mpls_table_read(int unit,int tnl_id,void *tnl_buf)
{
  int rv = BCM_E_NONE;

#if defined(BCM_TRIDENT3_SUPPORT)
  if (soc_feature(unit, soc_feature_td3_style_mpls)) {
      rv = READ_EGR_IP_TUNNEL_MPLS_DOUBLE_WIDEm(unit,
              MEM_BLOCK_ANY,tnl_id, tnl_buf);
  } else
#endif
  {
      rv = READ_EGR_IP_TUNNEL_MPLSm(unit, MEM_BLOCK_ANY,
              tnl_id,tnl_buf);
  }
  return rv;
}

STATIC void
bcmi_egr_ip_tnl_mpls_table_memset(int unit,void *tnl_buf)
{

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
        sal_memset(tnl_buf, 0,
                sizeof(egr_ip_tunnel_mpls_double_wide_entry_t));
    } else
#endif
    {
        sal_memset(tnl_buf, 0, sizeof(egr_ip_tunnel_mpls_entry_t));
    }
}

STATIC int
bcmi_egr_ip_tnl_mpls_entry_type_check(int unit,void *tnl_buf)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    mem = bcmi_egr_ip_tnl_mem_name_get(unit);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
        if (soc_mem_field32_get(unit,
                    mem,
                    tnl_buf, DATA_TYPEf) != 0x14) {
            return BCM_E_INTERNAL;
        }
    } else
#endif
    {
        if (soc_mem_field32_get(unit,
                    mem,
                    tnl_buf, ENTRY_TYPEf) != 3) {
            return BCM_E_INTERNAL;
        }
    }
    return rv;
}

int
bcmi_egress_tnl_label_entry_set(int unit, void *tnl_buf,
        int offset, uint32 *data_buf)
{
    int rv = BCM_E_NONE;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
        soc_mem_field_set(unit, EGR_IP_TUNNEL_MPLS_DOUBLE_WIDEm,
                (uint32 *)tnl_buf, _tnl_label_entry_f[offset], data_buf);
    }
#endif

    return rv;
}


int
bcmi_egress_tnl_label_entry_get(int unit, void *tnl_buf,
        int offset, uint32 *data_buf)
{
    int rv = BCM_E_NONE;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
        soc_mem_field_get(unit, EGR_IP_TUNNEL_MPLS_DOUBLE_WIDEm,
                (uint32 *)tnl_buf, _tnl_label_entry_f[offset], data_buf);
    }
#endif

    return rv;
}

STATIC int
bcmi_validate_field_range(int unit,uint32 set_val,int range)
{
#ifdef BCM_TRIDENT3_SUPPORT
    return BCM_E_NONE;
#else
    return BCM_E_NONE;
#endif

}

STATIC void
bcmi_egr_ip_tnl_copy_src_label_info_to_dest(int unit, void *src_entry ,
        void *dst_entry, int src_offset, int dst_offset, int no_of_elements)
{
    int i = 0;
    int src_value;

    for (; i < no_of_elements; i++) {
        src_value = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, src_entry,
                _tnl_push_action_f[src_offset+i]);
        soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, dst_entry,
                _tnl_push_action_f[dst_offset+i], src_value);
        src_value = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, src_entry,
                _tnl_label_f[src_offset+i]);
        soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, dst_entry,
                _tnl_label_f[dst_offset+i], src_value);
        src_value = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, src_entry,
                _tnl_exp_select_f[src_offset+i]);
        soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, dst_entry,
                _tnl_exp_select_f[dst_offset+i], src_value);
        src_value = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, src_entry,
                _tnl_exp_f[src_offset+i]);
        soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, dst_entry,
                _tnl_exp_f[dst_offset+i], src_value);
        src_value = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, src_entry,
                _tnl_pri_f[src_offset+i]);
        soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, dst_entry,
                _tnl_pri_f[dst_offset+i], src_value);
        src_value = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, src_entry,
                _tnl_cfi_f[src_offset+i]);
        soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, dst_entry,
                _tnl_cfi_f[dst_offset+i], src_value);
        src_value = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, src_entry,
                _tnl_exp_ptr_f[src_offset+i]);
        soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, dst_entry,
                _tnl_exp_ptr_f[dst_offset+i], src_value);
        src_value = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, src_entry,
                _tnl_ttl_f[src_offset+i]);
        soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, dst_entry,
                _tnl_ttl_f[dst_offset+i], src_value);
    }
}

STATIC int
bcmi_td3_destination_format_set(int unit, mpls_entry_entry_t *mpls_entry,
                                               int dest_type, int value)
{
    if (mpls_entry == NULL) {
        return BCM_E_PARAM;
    }

    switch (dest_type) {
        case SOC_MEM_FIF_DEST_NEXTHOP:
            soc_mem_field32_dest_set(unit, MPLS_ENTRYm, mpls_entry,
                    DESTINATIONf,
                    SOC_MEM_FIF_DEST_NEXTHOP, value);
            break;
        case SOC_MEM_FIF_DEST_ECMP:
            soc_mem_field32_dest_set(unit, MPLS_ENTRYm, mpls_entry,
                    DESTINATIONf,
                    SOC_MEM_FIF_DEST_ECMP, value);
            break;
        default:
            break;
    }

    return BCM_E_NONE;
}

STATIC int
_bcmi_xgs5_mpls_entry_get_key(int unit, mpls_entry_entry_t *ment,
                           bcm_mpls_tunnel_switch_t *info)
{
    bcm_port_t port_in = 0, port_out = 0;
    bcm_module_t mod_in = 0, mod_out = 0;
    bcm_trunk_t trunk_id;
    soc_mem_t mem = MPLS_ENTRYm;

    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        mem = MPLS_ENTRY_SINGLEm;
    } else {
        mod_in = soc_mem_field32_get(unit, mem, ment, MODULE_IDf);
    }
    port_in = soc_mem_field32_get(unit, mem, ment, PORT_NUMf);
    if (soc_mem_field32_get(unit, mem, ment, Tf)) {
        trunk_id = soc_mem_field32_get(unit, mem, ment, TGIDf);
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
    info->label = soc_mem_field32_get(unit, mem, ment, MPLS_LABELf);

    return BCM_E_NONE;
}

/* Convert data part of HW entry to application format. */
STATIC int
_bcmi_xgs5_mpls_entry_get_data(int unit, mpls_entry_entry_t *ment,
                            bcm_mpls_tunnel_switch_t *info)
{
    int rv, nh_index, vrf;
    int action_if_bos, action_if_not_bos;
    bcm_if_t egress_if=0;
    int mode = 0;
    soc_mem_t mem = MPLS_ENTRYm;
#ifdef BCM_MONTEREY_SUPPORT
    uint32 roe_type = 0;
    uint32 bit_num = 0;
#endif

    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        mem = MPLS_ENTRY_SINGLEm;
    }

    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_ingress_mode_get(unit, &mode));
    action_if_bos = soc_mem_field32_get(unit, mem, ment, MPLS_ACTION_IF_BOSf);
    action_if_not_bos = soc_mem_field32_get(unit, mem, ment,
                                            MPLS_ACTION_IF_NOT_BOSf); 

    if (action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_IIF ||
        action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_POP) {
        if (!mode) {
            _bcm_l3_ingress_intf_t iif;
#ifdef BCM_TOMAHAWK_SUPPORT
            int tunnel_term_ecn_map_id;
#endif
            vrf = soc_mem_field32_get(unit, mem, ment, L3_IIFf);
            /*
             * If the ingress mode is not set, then L3_IIF is added
             * above the intreface of _BCM_TR_MPLS_L3_IIF_BASE.
             * If this is 0 then 0 vpn index was set.
             */
            if (vrf != 0) {
                iif.intf_id =  vrf;
                vrf -= _BCM_TR_MPLS_L3_IIF_BASE;
                _BCM_MPLS_VPN_SET(info->vpn, _BCM_MPLS_VPN_TYPE_L3, vrf);
                rv = _bcm_tr_l3_ingress_interface_get(unit, NULL, &iif);
                BCM_IF_ERROR_RETURN(rv);
#ifdef BCM_TOMAHAWK_SUPPORT
                if (soc_feature(unit, soc_feature_mpls_ecn)) {
                    tunnel_term_ecn_map_id = iif.tunnel_term_ecn_map_id;
                    if (bcmi_xgs5_ecn_map_used_get(unit, tunnel_term_ecn_map_id,
                                _bcmEcnmapTypeTunnelTerm)) {
                        info->tunnel_term_ecn_map_id = tunnel_term_ecn_map_id |
                            _BCM_XGS5_ECN_MAP_TYPE_TUNNEL_TERM;
                        info->flags |= BCM_MPLS_SWITCH_TUNNEL_TERM_ECN_MAP;
                    }
                }
#endif
            }
        } else {
            info->ingress_if = soc_mem_field32_get(unit, mem, ment, L3_IIFf);
        } 
    }

    if (action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI ||
            action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_NHI) {
        nh_index = soc_mem_field32_get(unit, mem, ment,
                                               NEXT_HOP_INDEXf);
        rv = bcm_tr_mpls_get_vp_nh (unit, nh_index,&egress_if);
        if (rv == BCM_E_NONE) {
            rv = bcm_tr_mpls_l3_nh_info_get(unit, info, nh_index);
        } else
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
            rv = bcm_th3_mpls_l3_nh_info_get(unit, info, nh_index);
        } else
#endif
        {
            info->egress_if = nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
            info->egress_label.label = BCM_MPLS_LABEL_INVALID;
            rv = BCM_E_NONE;
        }
        BCM_IF_ERROR_RETURN(rv);
    }

    if (action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_NHI ||
        action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_ROE_NHI ||
        action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_NHI) {
        info->action = BCM_MPLS_SWITCH_ACTION_PHP;
        nh_index = soc_mem_field32_get(unit, mem, ment,
                                               NEXT_HOP_INDEXf);
        info->egress_if = nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
    }

    if (action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_ECMP ||
        action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_ECMP ||
        action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP ||
        action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_ECMP) {
        info->action = BCM_MPLS_SWITCH_ACTION_SWAP;
        nh_index = soc_mem_field32_get(unit, mem, ment,
                ECMP_PTRf);
        info->egress_if = nh_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    }


    switch(action_if_bos) {
        case _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_IIF:
            info->action_if_bos = BCM_MPLS_SWITCH_ACTION_POP;
            break;
        case _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI:
        case _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP:
            info->action_if_bos = BCM_MPLS_SWITCH_ACTION_SWAP;
            break;
        case _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_NHI:
        case _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_ROE_NHI:
        case _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_ECMP:
            info->action_if_bos = BCM_MPLS_SWITCH_ACTION_PHP;
            break;
        case _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_INVALID:
            info->action_if_bos = BCM_MPLS_SWITCH_ACTION_INVALID;
            break;
        default:
            return BCM_E_INTERNAL;
            break;
    }

   switch(action_if_not_bos) {
        case _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_POP:
            info->action_if_not_bos = BCM_MPLS_SWITCH_ACTION_POP;
            break;
        case _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_NHI:
        case _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_ECMP:
            info->action_if_not_bos = BCM_MPLS_SWITCH_ACTION_SWAP;
            break;
        case _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_NHI:
        case _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_ECMP:
            info->action_if_not_bos = BCM_MPLS_SWITCH_ACTION_PHP;
            break;
        case _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_INVALID:
            info->action_if_not_bos = BCM_MPLS_SWITCH_ACTION_INVALID;
            break;
        default:
            return BCM_E_INTERNAL;
            break;
    }

    if (info->action_if_bos == info->action_if_not_bos) {
        info->action = info->action_if_bos;
        if (info->action == BCM_MPLS_SWITCH_ACTION_INVALID) {
            /* Both actions are invalid in the HW entry 
             * Error out.
             */
            return BCM_E_INTERNAL;
        }
    } else {
        info->action = BCM_MPLS_SWITCH_ACTION_INVALID;
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, PW_TERM_NUM_VALIDf) &&
        soc_mem_field32_get(unit, mem, ment, PW_TERM_NUM_VALIDf)) {
        info->flags |= BCM_MPLS_SWITCH_COUNTED;
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
     else if (SOC_MEM_FIELD_VALID(unit, MPLS_ENTRYm, CLASS_IDf)) {
        info->class_id = soc_mem_field32_get(unit, mem, ment, CLASS_IDf);
#if defined(BCM_MONTEREY_SUPPORT)
        if (soc_feature(unit,soc_feature_mont_roe_classid_sw_war)) {
            if ((bit_num = soc_mem_field_length(unit, mem, CLASS_IDf)) > 0) {
                bit_num -= 1;
                info->class_id = info->class_id & (( 1 << bit_num ) - 1);
            }
        }
#endif
    }
#endif

#ifdef BCM_MONTEREY_SUPPORT
     if (SOC_MEM_FIELD_VALID(unit, MPLS_ENTRYm, ROE_TYPEf)) {
         roe_type = soc_MPLS_ENTRYm_field32_get(unit, ment, ROE_TYPEf);
         if (roe_type == _BCM_MPLS_XGS5_MPLS_ROE_TUNNEL) {
             info->flags2 |= BCM_MPLS_SWITCH2_ENABLE_ROE;
         } else if (roe_type == _BCM_MPLS_XGS5_MPLS_ROE_CUSTOM_TUNNEL) {
             info->flags2 |= BCM_MPLS_SWITCH2_ENABLE_ROE_CUSTOM;
         } else {
             /* Do nothing */
         }
     }
#endif

    if (!soc_mem_field32_get(unit, mem, ment, DECAP_USE_TTLf)) {
        info->flags |= BCM_MPLS_SWITCH_INNER_TTL;
    }
    if (soc_mem_field32_get(unit, mem, ment, DECAP_USE_EXP_FOR_INNERf)) {
        if ((info->action_if_bos == BCM_MPLS_SWITCH_ACTION_PHP) ||
            (info->action_if_bos == BCM_MPLS_SWITCH_ACTION_POP) ||
            (info->action_if_not_bos == BCM_MPLS_SWITCH_ACTION_PHP) ||
            (info->action_if_not_bos == BCM_MPLS_SWITCH_ACTION_POP)) {
            info->flags |= BCM_MPLS_SWITCH_INNER_EXP;
        }
    }
    if (soc_mem_field32_get(unit, mem, ment, 
                                    DECAP_USE_EXP_FOR_PRIf) == 0x1) {

        /* Use specified EXP-map to determine internal prio/color */
        info->flags |= BCM_MPLS_SWITCH_INT_PRI_MAP;
        info->exp_map = 
            soc_mem_field32_get(unit, mem, ment, EXP_MAPPING_PTRf);
        info->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
    } else if (soc_mem_field32_get(unit, mem, ment,
                                           DECAP_USE_EXP_FOR_PRIf) == 0x2) {

        /* Use the specified internal priority value */
        info->flags |= BCM_MPLS_SWITCH_INT_PRI_SET;
        info->int_pri =
            soc_mem_field32_get(unit, mem, ment, NEW_PRIf);

        /* Use specified EXP-map to determine internal color */
        info->flags |= BCM_MPLS_SWITCH_COLOR_MAP;
        info->exp_map = 
            soc_mem_field32_get(unit, mem, ment, EXP_MAPPING_PTRf);
        info->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
    }
    if (SOC_MEM_FIELD_VALID(unit,mem,
                            DO_NOT_CHANGE_PAYLOAD_DSCPf)) {
        if (soc_mem_field32_get(unit, mem, ment, 
                                        DO_NOT_CHANGE_PAYLOAD_DSCPf) == 0) {
            if ((info->action_if_bos == BCM_MPLS_SWITCH_ACTION_PHP) ||
                (info->action_if_bos == BCM_MPLS_SWITCH_ACTION_POP) ||
                (info->action_if_not_bos == BCM_MPLS_SWITCH_ACTION_PHP) ||
                (info->action_if_not_bos == BCM_MPLS_SWITCH_ACTION_POP)) {
                     info->flags |=
                         (BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL);
            }
        }
    }
#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (soc_feature(unit, soc_feature_mpls_ecn)) {
        int ecn_map_id;
        int ecn_map_hw_idx;
        int rv;
        ecn_map_hw_idx = soc_mem_field32_get(unit, mem, ment,
                                                     EXP_TO_IP_ECN_MAPPING_PTRf);
        rv = bcmi_ecn_map_hw_idx2id(unit, ecn_map_hw_idx, 
                                    _BCM_XGS5_MPLS_ECN_MAP_TYPE_EXP2ECN, 
                                    &ecn_map_id); 
        if (BCM_SUCCESS(rv)) {
            info->flags |= BCM_MPLS_SWITCH_INGRESS_ECN_MAP;
            info->ecn_map_id = ecn_map_id;
        }
    }
#endif /* BCM_TOMAHAWK2_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection) || 
        SOC_IS_TOMAHAWK2(unit)) {

        if (soc_mem_field32_get(unit, mem, ment,
                                        MPLS__DROP_DATA_ENABLEf)) {
            info->flags |= BCM_MPLS_SWITCH_DROP;
        }
        info->failover_id =
            soc_mem_field32_get(unit, mem, ment,
                                        MPLS__RX_PROT_GROUPf);
        if (info->failover_id != 0) {
            _BCM_ENCAP_TYPE_IN_FAILOVER_ID(info->failover_id,
                                           _BCM_FAILOVER_INGRESS);
        }
     }
#endif /*BCM_TRIDENT2PLUS_SUPPORT || BCM_TOMAHAWK2_SUPPORT */
    return BCM_E_NONE;
}

/*
 * Function:
 *        bcm_td3_mpls_l3_nh_info_get
 * Purpose:
 *        Get Tunnel_switch info for a specific EGR_NH Index
 * Parameters:
 *        unit - unit
 *        info - Tunnel switch info
 *        nh_index - next hop index
 * Returns:
 *        BCM_E_XXX
*/
STATIC int
bcm_td3_mpls_l3_nh_info_get(int unit, bcm_mpls_tunnel_switch_t *info, int nh_index)
{
    int vc_swap_idx = 0;
    int vc_data_type = 0;
    int pri_cfi_sel = 0;
    int mapping_ptr = 0;
    uint32 mpls_format[2];
    egr_l3_next_hop_entry_t egr_nh;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;

    SOC_IF_ERROR_RETURN(soc_mem_read(unit,
        EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, nh_index, &egr_nh));
    vc_swap_idx = soc_EGR_L3_NEXT_HOPm_field32_get(
        unit, &egr_nh, MPLS__VC_AND_SWAP_INDEXf);
    SOC_IF_ERROR_RETURN(soc_mem_read(unit,
        EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
        MEM_BLOCK_ANY, vc_swap_idx, &vc_entry));
    vc_data_type = soc_mem_field32_get(unit,
        EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
        &vc_entry, DATA_TYPEf);

    if (vc_data_type == _BCM_VC_SWAP_TBL_FLEX_DATA_TYPE) {
        sal_memset(mpls_format, 0, sizeof(mpls_format));
        soc_mem_field_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
            (uint32*)&vc_entry,
            MPLS_PHP__MPLS_ENTRY_0_ACTION_SETf,
            (uint32*)mpls_format);
        pri_cfi_sel = soc_format_field32_get(unit,
            MPLS_ENTRY_ACTION_SETfmt,
            (uint32*)mpls_format, PRI_CFI_SELf);
        mapping_ptr = soc_format_field32_get(unit,
            MPLS_ENTRY_ACTION_SETfmt,
            (uint32*)mpls_format, DOT1P_MAPPING_PTRf);

        if (pri_cfi_sel == 1) {
            info->flags2 |= BCM_MPLS_SWITCH2_VLAN_PRI_PHB_REMARK;
            BCM_IF_ERROR_RETURN(
                _bcm_tr2_qos_idx2id(unit, mapping_ptr,
                    _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS, &info->qos_map_id));
        } else if (pri_cfi_sel == 0 && mapping_ptr > 0) {
            info->flags2 |= BCM_MPLS_SWITCH2_VLAN_PRI_SET;
            info->pkt_cfi = mapping_ptr & 1;
            info->pkt_pri = (mapping_ptr >> 1) & 0x7;
        }
    }

    return BCM_E_NONE;
}

STATIC int
bcmi_td3_mpls_entry_get_data(int unit, mpls_entry_entry_t *ment,
                            bcm_mpls_tunnel_switch_t *info)
{
    int rv = BCM_E_NONE, nh_index, vrf;
    int action_if_bos, action_if_not_bos;
    bcm_if_t egress_if=0;
    int mode = 0;
    uint32 dest_type;
    uint32 value = 0;
    soc_mem_t mem = MPLS_ENTRYm;

    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_ingress_mode_get(unit, &mode));
    action_if_bos = soc_mem_field32_get(unit, mem, ment, MPLS_ACTION_IF_BOSf);
    action_if_not_bos = soc_mem_field32_get(unit, mem, ment,
                                            MPLS_ACTION_IF_NOT_BOSf);

    if (action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_POP_SVP_ENTROPY) {
       
    }

    /*POP OR L3_IIF*/
    if (action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_L3_IIF ||
            action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_POP) {
        if (!mode) {
            _bcm_l3_ingress_intf_t iif;
#ifdef BCM_TOMAHAWK_SUPPORT
            int tunnel_term_ecn_map_id;
#endif
            vrf = soc_mem_field32_get(unit, mem, ment, L3_IIFf);
            iif.intf_id =  vrf;
            vrf -= _BCM_TR_MPLS_L3_IIF_BASE;
            _BCM_MPLS_VPN_SET(info->vpn, _BCM_MPLS_VPN_TYPE_L3, vrf);
            rv = _bcm_tr_l3_ingress_interface_get(unit, NULL, &iif);
            BCM_IF_ERROR_RETURN(rv);
#ifdef BCM_TOMAHAWK_SUPPORT
            if (soc_feature(unit, soc_feature_mpls_ecn)) {
                tunnel_term_ecn_map_id = iif.tunnel_term_ecn_map_id;
                if (bcmi_xgs5_ecn_map_used_get(unit, tunnel_term_ecn_map_id,
                            _bcmEcnmapTypeTunnelTerm)) {
                    info->tunnel_term_ecn_map_id = tunnel_term_ecn_map_id |
                        _BCM_XGS5_ECN_MAP_TYPE_TUNNEL_TERM;
                    info->flags |= BCM_MPLS_SWITCH_TUNNEL_TERM_ECN_MAP;
                }
            }
#endif
        } else {
            info->ingress_if = soc_mem_field32_get(unit, mem, ment, L3_IIFf);
        }
    }

    if (action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP ||
            action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP) {
        value = soc_mem_field32_dest_get(unit, MPLS_ENTRYm, ment,
                DESTINATIONf, &dest_type);

        if (dest_type == SOC_MEM_FIF_DEST_NEXTHOP) {
            nh_index = value;

            rv = bcm_tr_mpls_get_vp_nh (unit, nh_index,&egress_if);
            if (rv == BCM_E_NONE) {
                rv = bcm_tr_mpls_l3_nh_info_get(unit, info, nh_index);
            } else {
                info->egress_if = nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
                info->egress_label.label = BCM_MPLS_LABEL_INVALID;
                rv = BCM_E_NONE;
            }
        }
        BCM_IF_ERROR_RETURN(rv);
    }

    if (action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_PHP ||
            action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_PHP) {
        info->action = BCM_MPLS_SWITCH_ACTION_PHP;
        value = soc_mem_field32_dest_get(unit, MPLS_ENTRYm, ment,
                DESTINATIONf, &dest_type);

        if (dest_type == SOC_MEM_FIF_DEST_NEXTHOP) {
            nh_index = value;
            info->egress_if = nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
            BCM_IF_ERROR_RETURN(
                bcm_td3_mpls_l3_nh_info_get(unit, info, nh_index));
        }
    }

    if (action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_PHP ||
            action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_PHP ||
            action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP ||
            action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP) {
        info->action = BCM_MPLS_SWITCH_ACTION_SWAP;
        value = soc_mem_field32_dest_get(unit, MPLS_ENTRYm, ment,
                DESTINATIONf, &dest_type);

        if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
            nh_index = value;
            info->egress_if = nh_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        }
    }

    switch(action_if_bos) {
        case _BCM_MPLS_TD3_MPLS_ACTION_L3_IIF:
            info->action_if_bos = BCM_MPLS_SWITCH_ACTION_POP;
            break;
        case _BCM_MPLS_TD3_MPLS_ACTION_SWAP:
            info->action_if_bos = BCM_MPLS_SWITCH_ACTION_SWAP;
            break;
        case _BCM_MPLS_TD3_MPLS_ACTION_PHP:
            info->action_if_bos = BCM_MPLS_SWITCH_ACTION_PHP;
            break;
        case _BCM_MPLS_TD3_MPLS_ACTION_INVALID:
            info->action_if_bos = BCM_MPLS_SWITCH_ACTION_INVALID;
            break;
        default:
            return BCM_E_INTERNAL;
            break;
    }

    switch(action_if_not_bos) {
        case _BCM_MPLS_TD3_MPLS_ACTION_POP:
            info->action_if_not_bos = BCM_MPLS_SWITCH_ACTION_POP;
            break;
        case _BCM_MPLS_TD3_MPLS_ACTION_SWAP:
            info->action_if_not_bos = BCM_MPLS_SWITCH_ACTION_SWAP;
            break;
        case _BCM_MPLS_TD3_MPLS_ACTION_PHP:
            info->action_if_not_bos = BCM_MPLS_SWITCH_ACTION_PHP;
            break;
        case _BCM_MPLS_TD3_MPLS_ACTION_INVALID:
            info->action_if_not_bos = BCM_MPLS_SWITCH_ACTION_INVALID;
            break;
        default:
            return BCM_E_INTERNAL;
            break;
    }

    if (info->action_if_bos == info->action_if_not_bos) {
        info->action = info->action_if_bos;
        if (info->action == BCM_MPLS_SWITCH_ACTION_INVALID) {
            /* Both actions are invalid in the HW entry
             * Error out.
             */
            return BCM_E_INTERNAL;
        }
    } else {
        info->action = BCM_MPLS_SWITCH_ACTION_INVALID;
    }

    if (soc_mem_field32_get(unit, mem, ment, PW_TERM_NUM_VALIDf)) {
        info->flags |= BCM_MPLS_SWITCH_COUNTED;
    }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
     else if (SOC_MEM_FIELD_VALID(unit, MPLS_ENTRYm, CLASS_IDf)) {
        info->class_id = soc_mem_field32_get(unit, mem, ment, CLASS_IDf);
    }
#endif

    if (!soc_mem_field32_get(unit, mem, ment, DECAP_USE_TTLf)) {
        info->flags |= BCM_MPLS_SWITCH_INNER_TTL;
    }
    if (soc_mem_field32_get(unit, mem, ment, DECAP_USE_EXP_FOR_INNERf)) {
        if ((info->action_if_bos == BCM_MPLS_SWITCH_ACTION_PHP) ||
            (info->action_if_bos == BCM_MPLS_SWITCH_ACTION_POP) ||
            (info->action_if_not_bos == BCM_MPLS_SWITCH_ACTION_PHP) ||
            (info->action_if_not_bos == BCM_MPLS_SWITCH_ACTION_POP)) {
            info->flags |= BCM_MPLS_SWITCH_INNER_EXP;
        }
    }
    if (soc_mem_field32_get(unit, mem, ment,
                                    DECAP_USE_EXP_FOR_PRIf) == 0x1) {

        /* Use specified EXP-map to determine internal prio/color */
        info->flags |= BCM_MPLS_SWITCH_INT_PRI_MAP;
        info->exp_map =
            soc_mem_field32_get(unit, mem, ment, EXP_MAPPING_PTRf);
        info->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
    } else if (soc_mem_field32_get(unit, mem, ment,
                                           DECAP_USE_EXP_FOR_PRIf) == 0x2) {

        /* Use the specified internal priority value */
        info->flags |= BCM_MPLS_SWITCH_INT_PRI_SET;
        info->int_pri =
            soc_mem_field32_get(unit, mem, ment, NEW_PRIf);

        /* Use specified EXP-map to determine internal color */
        info->flags |= BCM_MPLS_SWITCH_COLOR_MAP;
        info->exp_map =
            soc_mem_field32_get(unit, mem, ment, EXP_MAPPING_PTRf);
        info->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
    }
    if (SOC_MEM_FIELD_VALID(unit,MPLS_ENTRYm,
                            DO_NOT_CHANGE_PAYLOAD_DSCPf)) {
        if (soc_mem_field32_get(unit, mem, ment,
                                        DO_NOT_CHANGE_PAYLOAD_DSCPf) == 0) {
            if ((info->action_if_bos == BCM_MPLS_SWITCH_ACTION_PHP) ||
                (info->action_if_bos == BCM_MPLS_SWITCH_ACTION_POP) ||
                (info->action_if_not_bos == BCM_MPLS_SWITCH_ACTION_PHP) ||
                (info->action_if_not_bos == BCM_MPLS_SWITCH_ACTION_POP)) {
                     info->flags |=
                         (BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL);
            }
        }
    }

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_mpls_ecn)) {
        int ecn_map_id;
        int ecn_map_hw_idx = 0;
        int rv;

        if(soc_feature(unit, soc_feature_td3_style_mpls)) {
            ecn_map_hw_idx = soc_MPLS_ENTRYm_field32_get(unit, ment,
                    EXP_TO_ECN_MAPPING_PTRf);
        }

        rv = bcmi_ecn_map_hw_idx2id(unit, ecn_map_hw_idx,
                                    _BCM_XGS5_MPLS_ECN_MAP_TYPE_EXP2ECN,
                                    &ecn_map_id);
        if (BCM_SUCCESS(rv)) {
            info->flags |= BCM_MPLS_SWITCH_INGRESS_ECN_MAP;
            info->ecn_map_id = ecn_map_id;
        }
    }
#endif
    return rv;
}

/* Convert key part of application format to HW entry. */
STATIC int
_bcmi_xgs5_mpls_entry_set_key(int unit, bcm_mpls_tunnel_switch_t *info,
                           mpls_entry_entry_t *ment)
{
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int rv, gport_id;
    soc_mem_t mem = MPLS_ENTRYm;
    soc_field_t validf = VALIDf;

    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        mem = MPLS_ENTRY_SINGLEm;
        validf = BASE_VALIDf;
    }

    sal_memset(ment, 0, sizeof(mpls_entry_entry_t));

    if (info->port == BCM_GPORT_INVALID) {
        /* Global label, mod/port not part of lookup key */
        if (SOC_MEM_FIELD_VALID(unit, mem, MODULE_IDf)) {
            soc_mem_field32_set(unit, mem, ment, MODULE_IDf, 0);
        }
        soc_mem_field32_set(unit, mem, ment, PORT_NUMf, 0);
        if (BCM_XGS3_L3_MPLS_LBL_VALID(info->label)) {
            soc_mem_field32_set(unit, mem, ment, MPLS_LABELf, info->label);
        } else {
            return BCM_E_PARAM;
        }

        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            soc_mem_field32_set(unit, mem, ment, BASE_VALID_0f, 3);
            soc_mem_field32_set(unit, mem, ment, BASE_VALID_1f, 7);
        } else if (SOC_MEM_FIELD_VALID(unit, mem, BASE_VALIDf)) {
            soc_mem_field32_set(unit, mem, ment, BASE_VALIDf, 1);
        } else {
            soc_mem_field32_set(unit, mem, ment, validf, 1);
        }
        return BCM_E_NONE;
    }

    rv = _bcm_esw_gport_resolve(unit, info->port, &mod_out, 
                                &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

    if (BCM_GPORT_IS_TRUNK(info->port)) {
        soc_mem_field32_set(unit, mem, ment, Tf, 1);
        soc_mem_field32_set(unit, mem, ment, TGIDf, trunk_id);
    } else {
        if (SOC_MEM_FIELD_VALID(unit, mem, MODULE_IDf)) {
            soc_mem_field32_set(unit, mem, ment, MODULE_IDf, mod_out);
        }
        soc_mem_field32_set(unit, mem, ment, PORT_NUMf, port_out);
    }
    if (BCM_XGS3_L3_MPLS_LBL_VALID(info->label)) {
        soc_mem_field32_set(unit, mem, ment, MPLS_LABELf, info->label);
    } else {
        return BCM_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
        soc_mem_field32_set(unit, mem, ment, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, ment, BASE_VALID_1f, 7);
    } else if (SOC_MEM_FIELD_VALID(unit, mem, BASE_VALIDf)){
        soc_mem_field32_set(unit, mem, ment, BASE_VALIDf, 1);
    } else {
        soc_mem_field32_set(unit, mem, ment, validf, 1);
    }

    return BCM_E_NONE;
}

#if defined(BCM_TRIDENT3_SUPPORT)
STATIC int
bcmi_td3_php_ecmp_nhop_vcswap_add(int unit,bcm_mpls_tunnel_switch_t *php_info,
        int nh_index, int mpath_flag)
{
    int i = 0, ecmp_member_count = 0;
    int alloc_size, index;
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_l3_ecmp_member_t *ecmp_member_array = NULL;
    int rv = BCM_E_NONE;

    if (mpath_flag & BCM_L3_MULTIPATH) {

        alloc_size = sizeof(bcm_l3_ecmp_member_t) * BCM_XGS3_L3_ECMP_MAX(unit);
        ecmp_member_array = sal_alloc(alloc_size, "ecmp member array");
        if (NULL == ecmp_member_array) {
            return BCM_E_MEMORY;
        }
        sal_memset(ecmp_member_array, 0, alloc_size);

        ecmp_info.ecmp_intf = nh_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        rv = bcm_xgs3_l3_ecmp_get(unit, &ecmp_info, BCM_XGS3_L3_ECMP_MAX(unit),
                ecmp_member_array, &ecmp_member_count);
        if (BCM_FAILURE(rv)) {
            goto clean_up;
        }

        while (i < ecmp_member_count) {
            if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit,
                        ecmp_member_array[i].egress_if)) {
                index = (ecmp_member_array[i].egress_if -
                        BCM_XGS3_EGRESS_IDX_MIN(unit));
            } else {
                index = (ecmp_member_array[i].egress_if -
                        BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
            }

            rv = bcmi_td3_mpls_php_vcswap_info_add(unit, php_info, &index);
            if (BCM_FAILURE(rv)) {
                goto clean_up;
            }
            i++;
        }
    }

clean_up:
    if (ecmp_member_array) {
        sal_free(ecmp_member_array);
        ecmp_member_array = NULL;
    }

    return rv;
}

STATIC int
bcmi_td3_php_ecmp_nhop_vcswap_delete(int unit, int nh_index, int dest_type, int mpath_flag)
{
    int i = 0, ecmp_member_count = 0;
    int alloc_size, index;
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_l3_ecmp_member_t *ecmp_member_array = NULL;
    int rv = BCM_E_NONE;

    if (mpath_flag & BCM_L3_MULTIPATH) {

        alloc_size = sizeof(bcm_l3_ecmp_member_t) * BCM_XGS3_L3_ECMP_MAX(unit);
        ecmp_member_array = sal_alloc(alloc_size, "ecmp member array");
        if (NULL == ecmp_member_array) {
            return BCM_E_MEMORY;
        }
        sal_memset(ecmp_member_array, 0, alloc_size);

        ecmp_info.ecmp_intf = nh_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        rv = bcm_xgs3_l3_ecmp_get(unit, &ecmp_info, BCM_XGS3_L3_ECMP_MAX(unit),
                ecmp_member_array, &ecmp_member_count);
        if (BCM_FAILURE(rv)) {
            goto clean_up;
        }

        while (i < ecmp_member_count) {
            if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit,
                        ecmp_member_array[i].egress_if)) {
                index = (ecmp_member_array[i].egress_if -
                        BCM_XGS3_EGRESS_IDX_MIN(unit));
            } else {
                index = (ecmp_member_array[i].egress_if -
                        BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
            }

            rv = bcmi_td3_mpls_php_vcswap_info_delete(unit, index, dest_type);
            if (BCM_FAILURE(rv)) {
                goto clean_up;
            }
            i++;
        }
    }

clean_up:
    if (ecmp_member_array) {
        sal_free(ecmp_member_array);
        ecmp_member_array = NULL;
    }

    return rv;
}
#endif

STATIC int
bcmi_xgs5_mpls_entry_label_actions_set(int unit, int action_if_bos,
        int action_if_not_bos, bcm_mpls_tunnel_switch_t *info,
        mpls_entry_entry_t *ment, int *nh_index, int *nh_index_alloc_failed,
        int nh_changed)
{
    int rv = BCM_E_NONE;
    int mode,vrf=0;
    uint32 mpath_flag=0;
    soc_mem_t mem = MPLS_ENTRYm;
#if defined(BCM_TRIDENT3_SUPPORT)
    uint32 cancun_ver;
#endif

    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        mem = MPLS_ENTRY_SINGLEm;
    }

    if ((ment == NULL) || (info == NULL)) {
        return BCM_E_INTERNAL;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        SOC_IF_ERROR_RETURN(soc_cancun_version_get(unit, &cancun_ver));
    }
#endif

    /*POP ACTION*/
    if (BCM_MPLS_SWITCH_ACTION_POP == action_if_bos ||
            BCM_MPLS_SWITCH_ACTION_POP == action_if_not_bos) {

        /* uniform qos model if either of these two flags set.
         * Only apply to L3 MPLS and BOS
         */
        if (info->flags & (BCM_MPLS_SWITCH_OUTER_EXP |
                    BCM_MPLS_SWITCH_OUTER_TTL) ) {
            if (SOC_MEM_FIELD_VALID(unit,mem,
                        DO_NOT_CHANGE_PAYLOAD_DSCPf)) {
                soc_mem_field32_set(unit, mem, ment,
                        DO_NOT_CHANGE_PAYLOAD_DSCPf, 0);
            }
        }

        mode = -1;
        if (_BCM_MPLS_VPN_IS_L3(info->vpn)) {
            _BCM_MPLS_VPN_GET(vrf, _BCM_MPLS_VPN_TYPE_L3, info->vpn);
            if (!_BCM_MPLS_VRF_USED_GET(unit, vrf)) {
                return BCM_E_PARAM;
            }

            /* Check L3 Ingress Interface mode. */
            mode = 0;
            rv = bcm_xgs3_l3_ingress_mode_get(unit, &mode);
            BCM_IF_ERROR_RETURN(rv);
        }
        if (!mode) {
            _bcm_l3_ingress_intf_t iif;
            sal_memset(&iif, 0, sizeof(_bcm_l3_ingress_intf_t));
            iif.intf_id =  _BCM_TR_MPLS_L3_IIF_BASE + vrf;
            rv = _bcm_tr_l3_ingress_interface_get(unit, NULL, &iif);
            BCM_IF_ERROR_RETURN(rv);

            iif.vrf = vrf;
#ifdef BCM_TOMAHAWK_SUPPORT
            if (soc_feature(unit, soc_feature_ecn_wred) && 
                (info->flags & BCM_MPLS_SWITCH_TUNNEL_TERM_ECN_MAP)) {
                int ecn_map_index;
                int ecn_map_type;
                ecn_map_type = info->tunnel_term_ecn_map_id & 
                                _BCM_XGS5_ECN_MAP_TYPE_MASK;
                if (ecn_map_type != _BCM_XGS5_ECN_MAP_TYPE_TUNNEL_TERM) {
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN(bcmi_ecn_map_id2hw_idx(unit, 
                    info->tunnel_term_ecn_map_id, &ecn_map_index));
                iif.tunnel_term_ecn_map_id = ecn_map_index;
                iif.flags |= BCM_L3_INGRESS_TUNNEL_TERM_ECN_MAP;
            }
#endif

            rv = _bcm_tr_l3_ingress_interface_set(unit, &iif, NULL, NULL);
            BCM_IF_ERROR_RETURN(rv);

            soc_mem_field32_set(unit, mem, ment, L3_IIFf,
                    _BCM_TR_MPLS_L3_IIF_BASE + vrf);
        } else {
            soc_mem_field32_set(unit, mem, ment, L3_IIFf, info->ingress_if);
        }

        if (BCM_MPLS_SWITCH_ACTION_POP == action_if_not_bos) {
            if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                soc_mem_field32_set(unit, mem, ment,
                        MPLS_ACTION_IF_NOT_BOSf, 
                        _BCM_MPLS_TD3_MPLS_ACTION_POP);
            } else {
                soc_mem_field32_set(unit, mem, ment,
                        MPLS_ACTION_IF_NOT_BOSf,
                        _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_POP);
            }
        }

        if (BCM_MPLS_SWITCH_ACTION_POP == action_if_bos) {
            if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                soc_mem_field32_set(unit, mem, ment,
                        MPLS_ACTION_IF_BOSf,
                        _BCM_MPLS_TD3_MPLS_ACTION_L3_IIF);
            } else {
                soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_BOSf,
                        _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_IIF);
            }
        }
    }/*BCM_MPLS_SWITCH_ACTION_POP*/ 

    /*PHP ACTION*/
    if (BCM_MPLS_SWITCH_ACTION_PHP == action_if_bos ||
            BCM_MPLS_SWITCH_ACTION_PHP == action_if_not_bos) {

        if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, info->egress_if)) {
            if (BCM_MPLS_SWITCH_ACTION_PHP == action_if_bos) {

                if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                    soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_BOSf,
                            _BCM_MPLS_TD3_MPLS_ACTION_PHP);
                } else {
                    soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_BOSf,
                            _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_ECMP);
                }

            }

            if (BCM_MPLS_SWITCH_ACTION_PHP == action_if_not_bos) {
                if (soc_feature(unit, soc_feature_mpls_lsr_ecmp)) {

                    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                        soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_NOT_BOSf,
                                _BCM_MPLS_TD3_MPLS_ACTION_PHP);
                    } else {
                        soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_NOT_BOSf,
                                _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_ECMP);
                    }
                }
            }
        } else if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->egress_if)) {
            if (BCM_MPLS_SWITCH_ACTION_PHP == action_if_bos) {

                if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                    soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_BOSf,
                            _BCM_MPLS_TD3_MPLS_ACTION_PHP);
                } else {
#ifdef BCM_MONTEREY_SUPPORT
                    if ((info->flags2 & BCM_MPLS_SWITCH2_ENABLE_ROE) ||
                        (info->flags2 & BCM_MPLS_SWITCH2_ENABLE_ROE_CUSTOM)){
                        soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_BOSf,
                                _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_ROE_NHI);
                    } else
#endif
                    {
                        soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_BOSf,
                                _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_NHI);
                    }
                }
            }

            if (BCM_MPLS_SWITCH_ACTION_PHP == action_if_not_bos) {

                if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                    soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_NOT_BOSf,
                            _BCM_MPLS_TD3_MPLS_ACTION_PHP);
                } else {
                    soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_NOT_BOSf,
                            _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_NHI);
                }
            }
        } else {
            return BCM_E_PARAM;
        }

        /* uniform qos model if either of these two flags set.
         * Only apply to L3 MPLS and BOS
         */
        if (info->flags & (BCM_MPLS_SWITCH_OUTER_EXP |
                    BCM_MPLS_SWITCH_OUTER_TTL) ) {
            if (SOC_MEM_FIELD_VALID(unit,mem,
                        DO_NOT_CHANGE_PAYLOAD_DSCPf)) {
                soc_mem_field32_set(unit, mem, ment,
                        DO_NOT_CHANGE_PAYLOAD_DSCPf, 0);
            }
        }

        /* Check if nexthop has changed, if it has, increment
           next-hop ref counts, if it has not this is a REPLACE
           operation and hence ref counts are not altered */
        if (nh_changed) {
            /*
             * Get egress next-hop index from egress object and
             * increment egress object reference count.
             */

            BCM_IF_ERROR_RETURN(bcm_xgs3_get_nh_from_egress_object(unit,
                        info->egress_if, &mpath_flag, 1, nh_index));
        } else {
            /* Extract next hop index from unipath egress object. Needed
               here since we did not go into
               'bcm_xgs3_get_nh_from_egress_object' */
            if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->egress_if)) {
                *nh_index = info->egress_if - BCM_XGS3_EGRESS_IDX_MIN(unit);
            } else {
                *nh_index = info->egress_if - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
            }
        }

        /* Fix: Entry_Type = 1, for PHP Packets with more than 1 Label */
        /* Read the egress next_hop entry pointed by Egress-Object */
        if (!((info->flags2 & BCM_MPLS_SWITCH2_ENABLE_ROE) ||
              (info->flags2 & BCM_MPLS_SWITCH2_ENABLE_ROE_CUSTOM))) {
            rv = bcm_tr_mpls_egress_entry_modify(unit, *nh_index, mpath_flag, 1);
            if (rv < 0 ) {
                return rv;
            }
        }

        if (mpath_flag == BCM_L3_MULTIPATH) {
            if (soc_feature(unit, soc_feature_generic_dest)) {
                bcmi_td3_destination_format_set(unit, ment,
                        SOC_MEM_FIF_DEST_ECMP, *nh_index);
            } else {
                soc_mem_field32_set(unit, mem, ment, ECMP_PTRf,
                        *nh_index);
            }

            /* Swap to ECMP Type  PHP_ECMP or L3_ECMP flag is set up earlier */
        } else {
            if (soc_feature(unit, soc_feature_generic_dest)) {
                bcmi_td3_destination_format_set(unit, ment,
                        SOC_MEM_FIF_DEST_NEXTHOP, *nh_index);
            } else {
                soc_mem_field32_set(unit, mem, ment, NEXT_HOP_INDEXf,
                        *nh_index);
            }
        }

#if defined(BCM_TRIDENT3_SUPPORT) 
        /*PHP is similar to swap-to-self*/
        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            if (((SOC_IS_TRIDENT3(unit) && (cancun_ver < SOC_CANCUN_VERSION_DEF_6_0_0))  ||
                 (SOC_IS_MAVERICK2(unit) && (cancun_ver < SOC_CANCUN_VERSION_DEF_4_0_0)) ||
                 (SOC_IS_HELIX5(unit) && (cancun_ver < SOC_CANCUN_VERSION_DEF_4_0_0))) ||
                (BCM_MPLS_SWITCH_ACTION_PHP != action_if_not_bos)) {
                soc_mem_field32_set(unit, mem, ment,
                        DECAP_NEXT_FWD_LABELf, 1);

                soc_mem_field32_set(unit, mem, ment,
                        NEXT_FWD_LABEL_INHERITf, 1);

                soc_mem_field32_set(unit, mem, ment,
                        INHERITANCE_TYPEf, 1);
            }

            if (BCM_MPLS_SWITCH_ACTION_PHP == action_if_bos ||
                    BCM_MPLS_SWITCH_ACTION_PHP == action_if_not_bos) {
                if (info->action == BCM_MPLS_SWITCH_ACTION_INVALID) {
                    info->action = BCM_MPLS_SWITCH_ACTION_PHP;
                }
            }

            if (mpath_flag == BCM_L3_MULTIPATH) {
                rv = bcmi_td3_php_ecmp_nhop_vcswap_add(unit, info, *nh_index, mpath_flag);
            } else {
                rv = bcmi_td3_mpls_php_vcswap_info_add(unit, info, nh_index);
            }

            if (rv < 0) {
                return rv;
            }
        }
#endif
    } /*BCM_MPLS_SWITCH_ACTION_PHP*/ 

    /*SWAP ACTION*/
    if (BCM_MPLS_SWITCH_ACTION_SWAP == action_if_bos ||
            BCM_MPLS_SWITCH_ACTION_SWAP == action_if_not_bos) {

        if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->egress_if)) {
            if ((!soc_feature(unit, soc_feature_mpls_lsr_ecmp)) ||
                    (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, info->egress_if))) {
                return BCM_E_PARAM;
            }
        }
        if (BCM_XGS3_L3_MPLS_LBL_VALID(info->egress_label.label) ||
                _BCM_MPLS_EGRESS_LABEL_PRESERVE(unit, info->egress_label.flags)) {
            rv = bcm_tr_mpls_l3_nh_info_add(unit, info, nh_index);
            if (rv < 0) {
                *nh_index_alloc_failed = 1;
                return rv;
            }
            if (soc_feature(unit, soc_feature_generic_dest)) {
                bcmi_td3_destination_format_set(unit, ment,
                        SOC_MEM_FIF_DEST_NEXTHOP, *nh_index);
            } else {
                soc_mem_field32_set(unit, mem, ment, NEXT_HOP_INDEXf,
                        *nh_index);
            }

            if (BCM_MPLS_SWITCH_ACTION_SWAP == action_if_bos) {

                if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                    soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_BOSf,
                            _BCM_MPLS_TD3_MPLS_ACTION_SWAP);
                } else {
                    soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_BOSf,
                            _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI);
                }
            }

            if (BCM_MPLS_SWITCH_ACTION_SWAP == action_if_not_bos) {

                if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                    soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_NOT_BOSf,
                            _BCM_MPLS_TD3_MPLS_ACTION_SWAP);
                } else {
                    soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_NOT_BOSf,
                            _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_NHI);
                }
            }

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
            if (_BCM_MPLS_EGRESS_LABEL_PRESERVE(unit, info->egress_label.flags)) {
                /*Decap and add swap label to achieve swap-to-self.*/
                if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                    soc_mem_field32_set(unit, mem, ment,
                            CURR_FWD_LABEL_INHERITf, 1);

                    soc_mem_field32_set(unit, mem, ment,
                            INHERITANCE_TYPEf, 1);
                }
            }
#endif
        } else {
            /*
             * Get egress next-hop index from egress object and
             * increment egress object reference count.
             */
            BCM_IF_ERROR_RETURN(bcm_xgs3_get_nh_from_egress_object(unit, info->egress_if,
                        &mpath_flag, 1, nh_index));

            if (soc_feature(unit, soc_feature_mpls_lsr_ecmp) && (mpath_flag == BCM_L3_MULTIPATH)) {
                if (soc_feature(unit, soc_feature_generic_dest)) {
                    bcmi_td3_destination_format_set(unit, ment,
                            SOC_MEM_FIF_DEST_ECMP, *nh_index);
                } else {
                    soc_mem_field32_set(unit, mem, ment, ECMP_PTRf,
                            *nh_index);
                }

                /* Swap to ECMP Type */
                if (BCM_MPLS_SWITCH_ACTION_SWAP == action_if_not_bos) {

                    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                        soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_NOT_BOSf,
                                _BCM_MPLS_TD3_MPLS_ACTION_SWAP);
                    } else {
                        soc_mem_field32_set(unit, mem, ment, MPLS__MPLS_ACTION_IF_NOT_BOSf,
                                _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_ECMP);
                    }
                }

                if (BCM_MPLS_SWITCH_ACTION_SWAP == action_if_bos) {

                    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                        soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_BOSf,
                                _BCM_MPLS_TD3_MPLS_ACTION_SWAP);
                    } else {
                        soc_mem_field32_set(unit, mem, ment, MPLS__MPLS_ACTION_IF_BOSf,
                                _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP);
                    }
                }
            } else {
                if (soc_feature(unit, soc_feature_generic_dest)) {
                    bcmi_td3_destination_format_set(unit, ment,
                            SOC_MEM_FIF_DEST_NEXTHOP, *nh_index);
                } else {
                    soc_mem_field32_set(unit, mem, ment, NEXT_HOP_INDEXf,
                            *nh_index);
                }
                if (BCM_MPLS_SWITCH_ACTION_SWAP == action_if_not_bos) {

                    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                        soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_NOT_BOSf,
                                _BCM_MPLS_TD3_MPLS_ACTION_SWAP);
                    } else {
                        soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_NOT_BOSf,
                                _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_NHI);
                    }
                }

                if (BCM_MPLS_SWITCH_ACTION_SWAP == action_if_bos) {

                    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                        soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_BOSf,
                                _BCM_MPLS_TD3_MPLS_ACTION_SWAP);
                    } else {
                        soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_BOSf,
                                _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI);
                    }
                }
            }
        }
    }/* BCM_MPLS_SWITCH_ACTION_SWAP */

    /*INVALID ACTION*/
    if (BCM_MPLS_SWITCH_ACTION_INVALID == action_if_bos) {

        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_BOSf,
                    _BCM_MPLS_TD3_MPLS_ACTION_INVALID);
        } else {
            soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_BOSf,
                    _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_INVALID);
        }
    } else if (BCM_MPLS_SWITCH_ACTION_INVALID == action_if_not_bos) {

        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_NOT_BOSf,
                    _BCM_MPLS_TD3_MPLS_ACTION_INVALID);
        } else {
            soc_mem_field32_set(unit, mem, ment, MPLS_ACTION_IF_NOT_BOSf,
                    _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_INVALID);
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
bcmi_xgs5_mpls_tunnel_switch_add(int unit, bcm_mpls_tunnel_switch_t *info)
{
    mpls_entry_entry_t ment, ment_lkup, ment_old;
    int mode=0, nh_index = -1, rv, num_pw_term, old_pw_cnt = -1, pw_cnt = -1;
    int idx_l3_tunnel_mem, idx_mpls_mem;
    int old_nh_index = -1, old_ecmp_index = -1;
    int action, action_if_bos, action_if_not_bos;
    int old_action_if_bos = -1, old_action_if_not_bos = -1;
    int  tunnel_switch_update=0;
    int  ref_count=0;
    bcm_if_t  egress_if=0;
    uint32 retval = 0, dest_type =  SOC_MEM_FIF_DEST_INVALID;
    int nh_index_alloc_failed = 0;
    soc_mem_t mem = MPLS_ENTRYm;
    int new_nh_index = -1, nh_changed = 1;

    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        mem = MPLS_ENTRY_SINGLEm;
    }

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

    /*INT_CN TO EXP MAP is Deprecated in TD3*/
    if (soc_feature(unit, soc_feature_td3_style_mpls) &&
            (info->flags & BCM_MPLS_EGRESS_LABEL_INT_CN_TO_EXP_MAP)) {
        return BCM_E_UNAVAIL;
    }

    rv = bcm_tr_mpls_port_independent_range (unit, info->label, info->port);
    if (rv < 0) {
        return rv;
    }

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_mpls_frr_lookup)) {
        if (info->flags & BCM_MPLS_SWITCH_FRR) {
            rv = _bcm_tr3_mpls_tunnel_switch_frr_set(unit, info);
            return rv;
         }
    }
#endif /* BCM_TOMAHAWK2_SUPPORT || BCM_APACHE_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if((soc_feature(unit, soc_feature_fp_based_oam) ||
         soc_feature(unit, soc_feature_td3_style_fp)) &&
                SOC_MEM_FIELD_VALID(unit, MPLS_ENTRYm, CLASS_IDf)) {
        /*Ensure that class_id doesnt overcross CLASS_IDf*/
        int bit_num = 0;
        if((info->flags & BCM_MPLS_SWITCH_COUNTED) && (info->class_id)) {
            return BCM_E_PARAM;
        }
        if(info->class_id) {
            if ((bit_num = soc_mem_field_length(unit, mem, CLASS_IDf)) > 0) {
#if defined(BCM_MONTEREY_SUPPORT)
                if (soc_feature(unit,soc_feature_mont_roe_classid_sw_war)) {
                    bit_num -= 1;
                }
#endif
            }
            if(info->class_id > (( 1 << bit_num ) - 1)) {
                return BCM_E_PARAM;
            }
        }
    }
#endif

    action = info->action;
    if (info->action_if_bos != info->action_if_not_bos) {
        action = BCM_MPLS_SWITCH_ACTION_INVALID;
        action_if_bos = info->action_if_bos;
        action_if_not_bos = info->action_if_not_bos;
    } else {
        /* 
         * Same action specified in if_bos and if_not_bos.
         * If this action is of valid type, it will override
         * the action specified in info->action
         */
        if (info->action_if_bos != BCM_MPLS_SWITCH_ACTION_INVALID) {
            action = info->action_if_bos;
        }
        action_if_bos = action_if_not_bos = action;
    }

    if (!(BCM_MPLS_SWITCH_ACTION_SWAP == action_if_bos ||
          BCM_MPLS_SWITCH_ACTION_SWAP == action_if_not_bos ||
          BCM_MPLS_SWITCH_ACTION_POP == action_if_bos ||
          BCM_MPLS_SWITCH_ACTION_POP == action_if_not_bos ||
          BCM_MPLS_SWITCH_ACTION_PHP == action_if_bos ||
          BCM_MPLS_SWITCH_ACTION_PHP == action_if_not_bos)) {
        return BCM_E_PARAM;
    }
    if ((info->egress_label.flags & BCM_MPLS_EGRESS_LABEL_PRESERVE) &&
        (BCM_MPLS_SWITCH_ACTION_SWAP != action_if_bos ||
         BCM_MPLS_SWITCH_ACTION_SWAP != action_if_not_bos)) {
        return BCM_E_PARAM;
    }

#ifdef BCM_MONTEREY_SUPPORT
    if (!soc_feature(unit,soc_feature_roe)) {
        if (info->flags2 & BCM_MPLS_SWITCH2_ENABLE_ROE) {
            return BCM_E_UNAVAIL;
        }
    }
    if (!soc_feature(unit,soc_feature_roe_custom)) {
        if (info->flags2 & BCM_MPLS_SWITCH2_ENABLE_ROE_CUSTOM) {
            return BCM_E_UNAVAIL;
        }
    }
    if ((info->flags2 & BCM_MPLS_SWITCH2_ENABLE_ROE) && 
        (info->flags2 & BCM_MPLS_SWITCH2_ENABLE_ROE_CUSTOM)) {
        return BCM_E_PARAM;
    }
#endif

    BCM_IF_ERROR_RETURN(_bcmi_xgs5_mpls_entry_set_key(unit, info, &ment));

    /* See if entry already exists */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &idx_mpls_mem,
                        &ment, &ment, 0);

    if ((SOC_E_NONE == rv) &&
        soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        /* For TH3, if there exists an entry in MPLS_ENTRY_SINGLE table then
         * cache it, as at later stage the cached copy can be used to revert
         * the MPLS_ENTRY_SINGLE entry to its original format if the insertion
         * fails in L3_TUNNEL_SINGLEm
         */
        sal_memcpy(&ment_old, &ment, sizeof(mpls_entry_entry_t));
    }

    /* default not to use DSCP from ING_MPLS_EXP_MAPPING table */
    if (SOC_MEM_FIELD_VALID(unit,mem,DO_NOT_CHANGE_PAYLOAD_DSCPf)) {
        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            soc_mem_field32_set(unit, mem, &ment,
                                          DO_NOT_CHANGE_PAYLOAD_DSCPf, 0);
        } else {
            soc_mem_field32_set(unit, mem, &ment,
                                          DO_NOT_CHANGE_PAYLOAD_DSCPf, 1);
        }
    }

    if (rv == SOC_E_NONE) {
        /* Entry exists, save old info */
        tunnel_switch_update = 1;
        old_action_if_bos = soc_mem_field32_get(unit, mem, &ment,
                                                        MPLS_ACTION_IF_BOSf);
        old_action_if_not_bos = soc_mem_field32_get(unit, mem, &ment,
                MPLS_ACTION_IF_NOT_BOSf);

        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            /*TD3 old mpls actions*/
            if ((old_action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP) ||
                    (old_action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP) ||
                    (old_action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_PHP) ||
                    (old_action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_PHP)) {

                retval = soc_mem_field32_dest_get(unit, MPLS_ENTRYm, &ment,
                        DESTINATIONf, &dest_type);
                if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
                    old_ecmp_index = retval;
                }

                if (dest_type == SOC_MEM_FIF_DEST_NEXTHOP) {
                    old_nh_index = retval;
                }
            }

            if (((old_action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP) ||
                        (old_action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_ECMP)) &&
                    soc_feature(unit, soc_feature_mpls_lsr_ecmp)) {

                retval = soc_mem_field32_dest_get(unit, mem, &ment,
                        DESTINATIONf, &dest_type);

                if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
                    old_ecmp_index = retval;
                }
            }

        } else {

            if ((old_action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI) ||
                    (old_action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_NHI) ||
                    (old_action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_NHI) ||
                    (old_action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_NHI) ||
                    (old_action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_ROE_NHI)) {
                old_nh_index = soc_mem_field32_get(unit, mem, &ment, NEXT_HOP_INDEXf);
            }
            if ((old_action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_ECMP) ||
                    (old_action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_ECMP)) {
                old_ecmp_index = soc_mem_field32_get(unit, mem, &ment, ECMP_PTRf);
            }
            if (((old_action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP) ||
                        (old_action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_ECMP)) &&
                    soc_feature(unit, soc_feature_mpls_lsr_ecmp)) {
                old_ecmp_index = soc_mem_field32_get(unit, mem, &ment, ECMP_PTRf);
            }
        }
        if (SOC_MEM_FIELD_VALID(unit, mem, PW_TERM_NUM_VALIDf)) {
            if (soc_mem_field32_get(unit, mem, &ment, PW_TERM_NUM_VALIDf)) {
                old_pw_cnt = soc_mem_field32_get(unit, mem, &ment, PW_TERM_NUMf);
            }
        }
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    }

    /* Check if the entry exists already and is setup to use next-hop
       and if it has changed */
    if ((rv == SOC_E_NONE) && (dest_type == SOC_MEM_FIF_DEST_NEXTHOP)) {
        /* Extract next hop index from unipath egress object. */
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->egress_if)) {
            new_nh_index = info->egress_if - BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else {
            new_nh_index = info->egress_if - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        }
        /* If the NH has not changed, make a note */
        if (old_nh_index == new_nh_index) {
            nh_changed = 0;
        }
    }

    rv = bcmi_xgs5_mpls_entry_label_actions_set(unit, action_if_bos,
            action_if_not_bos, info, &ment, &nh_index, &nh_index_alloc_failed,
            nh_changed);

    if (rv < 0) {
        goto cleanup;
    }

    soc_mem_field32_set(unit, mem, &ment, V4_ENABLEf, 1);
    soc_mem_field32_set(unit, mem, &ment, V6_ENABLEf, 1);

    if (info->flags & BCM_MPLS_SWITCH_INNER_TTL) {
        if (BCM_MPLS_SWITCH_ACTION_SWAP == action_if_bos &&
            BCM_MPLS_SWITCH_ACTION_SWAP == action_if_not_bos) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        soc_mem_field32_set(unit, mem, &ment, DECAP_USE_TTLf, 0);
    } else {
        soc_mem_field32_set(unit, mem, &ment, DECAP_USE_TTLf, 1);
    }

    if (info->flags & BCM_MPLS_SWITCH_INNER_EXP) {
        if (BCM_MPLS_SWITCH_ACTION_SWAP == action_if_bos &&
            BCM_MPLS_SWITCH_ACTION_SWAP == action_if_not_bos) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        soc_mem_field32_set(unit, mem, &ment, DECAP_USE_EXP_FOR_INNERf, 0);
    } else {
        /* For SWAP, Do-not PUSH EXP */
        if (BCM_MPLS_SWITCH_ACTION_SWAP == action_if_bos &&
            BCM_MPLS_SWITCH_ACTION_SWAP == action_if_not_bos) {
            soc_mem_field32_set(unit, mem, &ment, DECAP_USE_EXP_FOR_INNERf, 0);
        } else {
            soc_mem_field32_set(unit, mem, &ment, DECAP_USE_EXP_FOR_INNERf, 1);
        }
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)){
        int local_failover_id = 0;
        int prot_type         = 0;
        if (info->flags & BCM_MPLS_SWITCH_DROP) {
            soc_mem_field32_set(unit, mem, &ment,
                    MPLS__DROP_DATA_ENABLEf, 1);
        } else {
            soc_mem_field32_set(unit, mem, &ment,
                    MPLS__DROP_DATA_ENABLEf, 0);
        }

        if (info->failover_id != 0) {
            local_failover_id = info->failover_id;
            _BCM_GET_FAILOVER_TYPE(local_failover_id, prot_type);
            _BCM_GET_FAILOVER_ID(local_failover_id);
            if ((prot_type & _BCM_FAILOVER_INGRESS) &&
                (BCM_SUCCESS(_bcm_td2p_failover_ingress_id_validate(
                unit, local_failover_id)))) {
                soc_mem_field32_set(unit, mem, &ment,
                                            MPLS__RX_PROT_GROUPf,
                                            local_failover_id);
            }
        } else {
            soc_mem_field32_set(unit, mem, &ment,
                                        MPLS__RX_PROT_GROUPf, 0);
        }
    }
#endif

#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)){
        int local_failover_id = 0;
        int prot_type         = 0;
        if (info->flags & BCM_MPLS_SWITCH_DROP) {
            soc_mem_field32_set(unit, mem, &ment,
                    MPLS__DROP_DATA_ENABLEf, 1);
        } else {
            soc_mem_field32_set(unit, mem, &ment,
                    MPLS__DROP_DATA_ENABLEf, 0);
        }

        if (info->failover_id != 0) {
            local_failover_id = info->failover_id;
            _BCM_GET_FAILOVER_TYPE(local_failover_id, prot_type);
            _BCM_GET_FAILOVER_ID(local_failover_id);
            if ((prot_type & _BCM_FAILOVER_INGRESS) &&
                (BCM_SUCCESS(_bcm_th2_failover_ingress_id_validate(
                unit, local_failover_id)))) {
                soc_mem_field32_set(unit, mem, &ment,
                                            MPLS__RX_PROT_GROUPf,
                                            local_failover_id);
            }
        } else {
            soc_mem_field32_set(unit, mem, &ment,
                                        MPLS__RX_PROT_GROUPf, 0);
        }
    }
#endif


    
    (void) bcm_tr_mpls_entry_internal_qos_set(unit, NULL, info, &ment);

    if ((info->flags & BCM_MPLS_SWITCH_COUNTED)) {
        if (SOC_MEM_IS_VALID(unit, ING_PW_TERM_COUNTERSm)) {
            if (old_pw_cnt == -1) {
                num_pw_term = soc_mem_index_count(unit, ING_PW_TERM_COUNTERSm);
                for (pw_cnt = 0; pw_cnt < num_pw_term; pw_cnt++) {
                    if (!_BCM_MPLS_PW_TERM_USED_GET(unit, pw_cnt)) {
                        break;
                    }
                }
                if (pw_cnt == num_pw_term) {
                    rv = BCM_E_RESOURCE;
                    goto cleanup;
                }
                _BCM_MPLS_PW_TERM_USED_SET(unit, pw_cnt);
                soc_mem_field32_set(unit, mem, &ment, PW_TERM_NUMf, pw_cnt);
                soc_mem_field32_set(unit, mem, &ment, PW_TERM_NUM_VALIDf, 1);
            }
        } else if (SOC_MEM_IS_VALID(unit, ING_PW_TERM_SEQ_NUMm)) {
            if (old_pw_cnt == -1) {
                num_pw_term = soc_mem_index_count(unit, ING_PW_TERM_SEQ_NUMm);
                for (pw_cnt = 0; pw_cnt < num_pw_term; pw_cnt++) {
                    if (!_BCM_MPLS_PW_TERM_USED_GET(unit, pw_cnt)) {
                        break;
                    }
                }
                if (pw_cnt == num_pw_term) {
                    rv = BCM_E_RESOURCE;
                    goto cleanup;
                }
                _BCM_MPLS_PW_TERM_USED_SET(unit, pw_cnt);
                soc_mem_field32_set(unit, mem, &ment, PW_TERM_NUMf, pw_cnt);
                soc_mem_field32_set(unit, mem, &ment, PW_TERM_NUM_VALIDf, 1);
            }
        }        
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    else if ((soc_feature(unit, soc_feature_fp_based_oam) ||
                soc_feature(unit, soc_feature_td3_style_fp)) &&
                SOC_MEM_FIELD_VALID(unit, MPLS_ENTRYm, CLASS_IDf)) {
        soc_mem_field32_set(unit, mem, &ment, CLASS_IDf, info->class_id);
    }
#endif

#ifdef BCM_MONTEREY_SUPPORT
    /* Enable ROE or ROE Custom frame handling */
    if (SOC_MEM_FIELD_VALID(unit,MPLS_ENTRYm,ROE_TYPEf)) {
        if (info->flags2 & BCM_MPLS_SWITCH2_ENABLE_ROE) {
            soc_mem_field32_set(unit, mem, &ment,
                    ROE_TYPEf, _BCM_MPLS_XGS5_MPLS_ROE_TUNNEL);
        } else if (info->flags2 & BCM_MPLS_SWITCH2_ENABLE_ROE_CUSTOM) {
            soc_mem_field32_set(unit, mem, &ment,
                    ROE_TYPEf, _BCM_MPLS_XGS5_MPLS_ROE_CUSTOM_TUNNEL);
        } else {
            soc_mem_field32_set(unit, mem, &ment, ROE_TYPEf, 0);
        }
    }
#endif

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT) 
    if (soc_feature(unit, soc_feature_mpls_ecn)) {
        if (info->flags & BCM_MPLS_SWITCH_INGRESS_ECN_MAP) { 
            int ecn_map_index;
            int ecn_map_type;
            int ecn_map_num;
            int ecn_map_hw_idx;
            soc_mem_t mem_map;

            ecn_map_type = info->ecn_map_id & _BCM_XGS5_MPLS_ECN_MAP_TYPE_MASK;
            ecn_map_index = info->ecn_map_id & _BCM_XGS5_MPLS_ECN_MAP_NUM_MASK;

#if defined(BCM_TRIDENT3_SUPPORT)
            if(soc_feature(unit, soc_feature_td3_style_mpls)) {
                mem_map = ING_EXP_TO_ECN_MAPPING_1m;
            } else
#endif
            {
                mem_map = ING_EXP_TO_IP_ECN_MAPPINGm;
            }

            ecn_map_num = 
                soc_mem_index_count(unit, mem_map) /
                _BCM_ECN_MAX_ENTRIES_PER_MAP;
            if (ecn_map_type != _BCM_XGS5_MPLS_ECN_MAP_TYPE_EXP2ECN) {
                rv = BCM_E_PARAM;
                goto cleanup;
            }
            if (ecn_map_index >= ecn_map_num) {
                rv = BCM_E_PARAM;
                goto cleanup;
            }
            if (!bcmi_xgs5_ecn_map_used_get(unit, ecn_map_index, 
                                            _bcmEcnmapTypeExp2Ecn)) {
                rv = BCM_E_PARAM;
                goto cleanup; 
            }
            if (bcmi_ecn_map_id2hw_idx(unit, info->ecn_map_id, &ecn_map_hw_idx)) {
                rv = BCM_E_PARAM;
                goto cleanup; 
            }
#if defined(BCM_TRIDENT3_SUPPORT)
            if(soc_feature(unit, soc_feature_td3_style_mpls)) {
                soc_MPLS_ENTRYm_field32_set(unit, &ment,
                        EXP_TO_ECN_MAPPING_PTRf,
                                        ecn_map_hw_idx);
            } else
#endif
            {
                soc_mem_field32_set(unit, mem, &ment, 
                                     EXP_TO_IP_ECN_MAPPING_PTRf,
                                     ecn_map_hw_idx);
            }
        }
    }
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
    if (BCM_MPLS_SWITCH_ACTION_SWAP == action_if_bos ||
            BCM_MPLS_SWITCH_ACTION_SWAP == action_if_not_bos) {
        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            /*It is needed for SWAPTOSELF scenarios*/
            soc_mem_field32_set(unit, mem, &ment,
                    CURR_FWD_LABEL_INHERITf, 1);

            soc_mem_field32_set(unit, mem, &ment,
                    INHERITANCE_TYPEf, 1);
        }
    }
#endif

    if (!tunnel_switch_update) {
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, &ment);
        if (BCM_SUCCESS(rv) &&
            soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
            rv = soc_mem_insert(unit, L3_TUNNEL_SINGLEm, MEM_BLOCK_ALL, &ment);
            if (rv < 0) {
                /* Revert the insertion into MPLS_ENTRY_SINGLEm */
                (void)soc_mem_delete(unit, mem, MEM_BLOCK_ALL, &ment);
            }
        }
    } else {
        rv = soc_mem_write(unit, mem,
                           MEM_BLOCK_ALL, idx_mpls_mem,
                           &ment);
        if (BCM_SUCCESS(rv) &&
            soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
            rv = _bcmi_xgs5_mpls_entry_set_key(unit, info, &ment_lkup);
            if (BCM_SUCCESS(rv)) {
                rv = soc_mem_search(unit, L3_TUNNEL_SINGLEm, MEM_BLOCK_ANY,
                                 &idx_l3_tunnel_mem, &ment_lkup, &ment_lkup, 0);
                if (BCM_SUCCESS(rv)) {
                    rv = soc_mem_write(unit, L3_TUNNEL_SINGLEm,
                                   MEM_BLOCK_ALL, idx_l3_tunnel_mem,
                                   &ment);
                }
            }

            if (rv < 0) {
                /* Revert the write into MPLS_ENTRY_SINGLEm */
                (void)soc_mem_write(
                        unit, mem, MEM_BLOCK_ALL, idx_mpls_mem, &ment_old);
            }
        }
    }

    if (rv < 0) {
        goto cleanup;
    }

    if (tunnel_switch_update) {
        /* Clean up old next-hop and counter info if entry was replaced */
        if ((old_pw_cnt != -1) && !(info->flags & BCM_MPLS_SWITCH_COUNTED)) {
            _BCM_MPLS_PW_TERM_USED_CLR(unit, old_pw_cnt);
        }

        if (soc_feature(unit, soc_feature_td3_style_mpls)) {

            if ((old_action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP) ||
                    (old_action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP)) {
                /* Check if tunnel_switch.egress_label mode is being used */
                if (dest_type == SOC_MEM_FIF_DEST_NEXTHOP) {

                    rv = bcm_tr_mpls_get_vp_nh (unit, (bcm_if_t) old_nh_index, &egress_if);
                    if (rv == BCM_E_NONE) {
                        rv = bcm_tr_mpls_l3_nh_info_delete(unit, old_nh_index);
                    } else {
                        /* Decrement next-hop Reference count */
                        rv = bcm_xgs3_get_ref_count_from_nhi(unit, 0, &ref_count, old_nh_index);
                    }
                }
            }

            if ((old_action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_PHP) ||
                    (old_action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_PHP)) {

                if (dest_type == SOC_MEM_FIF_DEST_NEXTHOP) {
                    rv = bcm_xgs3_nh_del(unit, 0, old_nh_index);
                }

                if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
                    rv = bcm_xgs3_ecmp_group_del(unit, old_ecmp_index, 0);
                }
            }

            if (soc_feature(unit, soc_feature_mpls_lsr_ecmp) &&
                    ((old_action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP) ||
                     (old_action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP))) {

                if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
                    rv = bcm_xgs3_ecmp_group_del(unit, old_ecmp_index, 0);
                }
            }

        } else {
            if ((old_action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI) ||
                    (old_action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_NHI)) {
                /* Check if tunnel_switch.egress_label mode is being used */
                rv = bcm_tr_mpls_get_vp_nh (unit, (bcm_if_t) old_nh_index, &egress_if);
                if (rv == BCM_E_NONE) {
                    rv = bcm_tr_mpls_l3_nh_info_delete(unit, old_nh_index);
                } else {
                    /* Decrement next-hop Reference count */
                    rv = bcm_xgs3_get_ref_count_from_nhi(unit, 0, &ref_count, old_nh_index);
                }
            }
            if ((old_action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_NHI) ||
                (old_action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_ROE_NHI) ||
                (old_action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_NHI)) {
                rv = bcm_xgs3_nh_del(unit, 0, old_nh_index);
            } 
            if ((old_action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_ECMP) ||
                    (old_action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_ECMP)) {
                rv = bcm_xgs3_ecmp_group_del(unit, old_ecmp_index, 0);
            }
            if (soc_feature(unit, soc_feature_mpls_lsr_ecmp) &&
                    ((old_action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP) ||
                     (old_action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_ECMP))) {
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
    if (nh_index_alloc_failed) {
        return rv;
    }
    if (pw_cnt != -1) {
        _BCM_MPLS_PW_TERM_USED_CLR(unit, pw_cnt);
    }
    /* coverity[check_after_sink : FALSE] */
    if (nh_index != -1) {
        if ((action_if_bos == BCM_MPLS_SWITCH_ACTION_SWAP) ||
            (action_if_not_bos == BCM_MPLS_SWITCH_ACTION_SWAP)) {
            if (BCM_XGS3_L3_MPLS_LBL_VALID(info->egress_label.label) ||
                (info->action == BCM_MPLS_SWITCH_ACTION_PHP)) {
                (void) bcm_tr_mpls_l3_nh_info_delete(unit, nh_index);
            }
        } else if ((action_if_bos == BCM_MPLS_SWITCH_ACTION_PHP) ||
                   (action_if_not_bos == BCM_MPLS_SWITCH_ACTION_PHP)) {
            (void) bcm_xgs3_nh_del(unit, 0, nh_index);
        }
    }
    return rv;
}

STATIC int
_bcmi_xgs5_mpls_entry_delete(int unit, mpls_entry_entry_t *ment)
{   
    ing_pw_term_counters_entry_t pw_cnt_entry;
    int rv, ecmp_index = -1, nh_index = -1, pw_cnt = -1;
    int action_if_bos, action_if_not_bos;
    bcm_if_t  egress_if=0;
    int  ref_count=0;
    uint32  dest_type = SOC_MEM_FIF_DEST_INVALID;
    uint32 value=0;
    soc_mem_t mem = MPLS_ENTRYm;

    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        mem = MPLS_ENTRY_SINGLEm;
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, PW_TERM_NUM_VALIDf)) {
        if (soc_mem_field32_get(unit, mem, ment, PW_TERM_NUM_VALIDf)) {
            pw_cnt = soc_mem_field32_get(unit, mem, ment, PW_TERM_NUMf);
        }
    }

    action_if_bos = soc_mem_field32_get(unit, mem, ment, MPLS_ACTION_IF_BOSf);
    action_if_not_bos = soc_mem_field32_get(unit, mem, ment,
                                                      MPLS_ACTION_IF_NOT_BOSf);

    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
        if ((action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP) ||
                (action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP) ||
                (action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_PHP) ||
                (action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_PHP)) {

            value = soc_mem_field32_dest_get(unit, MPLS_ENTRYm, ment,
                    DESTINATIONf, &dest_type);

            if (dest_type == SOC_MEM_FIF_DEST_NEXTHOP) {
                nh_index = value;
            }
            if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
                ecmp_index = value;
            }
        }
#if 0
        if ((action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_PHP) ||
                (action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_PHP)) {
            ecmp_index = soc_mem_field32_get(unit, mem, ment, ECMP_PTRf);
        }
#endif

        if (((action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP) ||
                    (action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP)) &&
                soc_feature(unit, soc_feature_mpls_lsr_ecmp)) {

            value = soc_mem_field32_dest_get(unit, mem, ment,
                    DESTINATIONf, &dest_type);

            if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
                ecmp_index = value;
            }
        }
    } else {
        if ((action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI) ||
                (action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_NHI) ||
                (action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_NHI) ||
                (action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_ROE_NHI) ||
                (action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_NHI)) {
            nh_index = soc_mem_field32_get(unit, mem, ment, NEXT_HOP_INDEXf);
        }
        if ((action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_ECMP) ||
                (action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_ECMP)) {
            ecmp_index = soc_mem_field32_get(unit, mem, ment, ECMP_PTRf);
        }
        if (((action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP) ||
                    (action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_ECMP)) &&
                soc_feature(unit, soc_feature_mpls_lsr_ecmp)) {
            ecmp_index = soc_mem_field32_get(unit, mem, ment, ECMP_PTRf);
        }
    }
    /* Delete the entry from HW */
    rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, ment);
    if ( (rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE) ) {
         return rv;
    }

    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        rv = soc_mem_delete(unit, L3_TUNNEL_SINGLEm, MEM_BLOCK_ALL, ment);
        if ( (rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE) ) {
             return rv;
        }
    }

    if (pw_cnt != -1) {
        if (SOC_MEM_IS_VALID(unit, ING_PW_TERM_SEQ_NUMm)) {
            ing_pw_term_seq_num_entry_t pw_seq_num_entry;

            sal_memset(&pw_seq_num_entry, 0, sizeof(ing_pw_term_seq_num_entry_t));
            (void) WRITE_ING_PW_TERM_SEQ_NUMm(unit, MEM_BLOCK_ALL, pw_cnt,
                                               &pw_seq_num_entry);
        } else {
            sal_memset(&pw_cnt_entry, 0, sizeof(ing_pw_term_counters_entry_t));
            (void) WRITE_ING_PW_TERM_COUNTERSm(unit, MEM_BLOCK_ALL, pw_cnt,
                                               &pw_cnt_entry);
        }

        _BCM_MPLS_PW_TERM_USED_CLR(unit, pw_cnt);
    }

    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
        if ((action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP) ||
                (action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP)) {
            /* Check if tunnel_switch.egress_label mode is being used */
            if (dest_type == SOC_MEM_FIF_DEST_NEXTHOP) {
                rv = bcm_tr_mpls_get_vp_nh (unit, (bcm_if_t) nh_index, &egress_if);
                if (rv == BCM_E_NONE) {
                    rv = bcm_tr_mpls_l3_nh_info_delete(unit, nh_index);
                } else {
                    /* Decrement next-hop Reference count */
                    rv = bcm_xgs3_get_ref_count_from_nhi(unit, 0, &ref_count, nh_index);
                }
            }
        }

        if ((action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_PHP) ||
                (action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_PHP)) {

            if (dest_type == SOC_MEM_FIF_DEST_NEXTHOP) {
#if defined(BCM_TRIDENT3_SUPPORT)
                rv = bcmi_td3_mpls_php_vcswap_info_delete(unit,
                        nh_index, dest_type);
                /* Decrement next-hop Reference count */
                rv = bcm_xgs3_get_ref_count_from_nhi(unit, 0,
                        &ref_count, nh_index);
#endif
                rv = bcm_xgs3_nh_del(unit, 0, nh_index);
            }

            if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
#if defined(BCM_TRIDENT3_SUPPORT)
                rv = bcmi_td3_php_ecmp_nhop_vcswap_delete(unit,
                        ecmp_index, dest_type,BCM_L3_MULTIPATH);
                /* Decrement next-hop Reference count */
                rv = bcm_xgs3_get_ref_count_from_nhi(unit, 0,
                        &ref_count, ecmp_index);
#endif
                rv = bcm_xgs3_ecmp_group_del(unit, ecmp_index, 0);
            }
        }

        if (soc_feature(unit, soc_feature_mpls_lsr_ecmp) &&
                ((action_if_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP) ||
                 (action_if_not_bos == _BCM_MPLS_TD3_MPLS_ACTION_SWAP))) {

            if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
                rv = bcm_xgs3_ecmp_group_del(unit, ecmp_index, 0);
            }
        }
    } else {
        if ((action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_NHI) ||
                (action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_NHI)) {
            /* Check if tunnel_switch.egress_label mode is being used */
            rv = bcm_tr_mpls_get_vp_nh (unit, (bcm_if_t) nh_index, &egress_if);
            if (rv == BCM_E_NONE) {
                rv = bcm_tr_mpls_l3_nh_info_delete(unit, nh_index);
            } else {
                /* Decrement next-hop Reference count */
                rv = bcm_xgs3_get_ref_count_from_nhi(unit, 0, &ref_count, nh_index);
            }
        }
        if ((action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_NHI) ||
            (action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_ROE_NHI) ||
                (action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_NHI)) {
            rv = bcm_xgs3_nh_del(unit, 0, nh_index);
        }
        if ((action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L3_ECMP) ||
                (action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_PHP_ECMP)) {
            rv = bcm_xgs3_ecmp_group_del(unit, ecmp_index, 0);
        }
        if (soc_feature(unit, soc_feature_mpls_lsr_ecmp) &&
                ((action_if_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_SWAP_ECMP) ||
                 (action_if_not_bos == _BCM_MPLS_XGS5_MPLS_ACTION_IF_NOT_BOS_SWAP_ECMP))) {
            rv = bcm_xgs3_ecmp_group_del(unit, ecmp_index, 0);
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
bcmi_xgs5_mpls_tunnel_switch_delete(int unit, bcm_mpls_tunnel_switch_t *info)
{
    int rv, index;
    mpls_entry_entry_t ment;
    soc_mem_t mem = MPLS_ENTRYm;

    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        mem = MPLS_ENTRY_SINGLEm;
    }
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_mpls_frr_lookup) && 
        (info->flags & BCM_MPLS_SWITCH_FRR)) { 
        rv = _bcm_tr3_mpls_tunnel_switch_frr_delete(unit, info);
    } else
#endif /* BCM_TOMAHAWK2_SUPPORT || BCM_APACHE_SUPPORT */
    {
        rv = _bcmi_xgs5_mpls_entry_set_key(unit, info, &ment);
        BCM_IF_ERROR_RETURN(rv);

        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                &ment, &ment, 0);
        if (rv < 0) {
            return rv;
        }
        rv = _bcmi_xgs5_mpls_entry_delete(unit, &ment);
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
bcmi_xgs5_mpls_tunnel_switch_delete_all(int unit)
{
    int rv = BCM_E_NONE, i;
    mpls_entry_entry_t *ment = NULL;
    int index_min, index_max;
    uint8 *mpls_entry_buf = NULL;
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    int num_entries;
    soc_tunnel_term_t tnl_entry;
#endif
    soc_mem_t mem = MPLS_ENTRYm;
    soc_field_t validf = VALIDf;

    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        mem = MPLS_ENTRY_SINGLEm;
        validf = BASE_VALIDf;
    }

    mpls_entry_buf = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, mem), "MPLS_ENTRY buffer");
    if (NULL == mpls_entry_buf) {
        return BCM_E_MEMORY;
    }

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
            index_min, index_max, mpls_entry_buf);
    if (SOC_FAILURE(rv)) {
        if (mpls_entry_buf) {
            soc_cm_sfree(unit, mpls_entry_buf);
        }
        return rv;
    }

    for (i = index_min; i <= index_max; i++) {
        ment = soc_mem_table_idx_to_pointer
            (unit, mem, mpls_entry_entry_t *, mpls_entry_buf, i);

        /* Check for valid entry */
        if (soc_feature(unit, soc_feature_td3_style_mpls)){
            if (soc_mem_field32_get(unit, mem, ment, BASE_VALID_0f) != 3) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem, ment, BASE_VALID_1f) != 7) {
                continue;
            }
        } else {
            if (!soc_mem_field32_get(unit, mem, ment, validf)) {
                continue;
            }
        }
        if (soc_mem_field32_get(unit, mem, ment, MPLS_ACTION_IF_BOSf) ==
                _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L2_SVP) {
            /* L2_SVP */
            continue;
        }

        rv = _bcmi_xgs5_mpls_entry_delete(unit, ment);
        if (BCM_FAILURE(rv)) {
            if (mpls_entry_buf) {
                soc_cm_sfree(unit, mpls_entry_buf);
            }
            return rv;
        }
    }

    if (mpls_entry_buf) {
        soc_cm_sfree(unit, mpls_entry_buf);
    }
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_mpls_frr_lookup)) {
        num_entries = soc_mem_index_count(unit, L3_TUNNELm);
        for (i = 0; i < num_entries; i++) {
            sal_memset(&tnl_entry, 0, sizeof(tnl_entry));
            rv = READ_L3_TUNNELm(unit, MEM_BLOCK_ANY, i,
                (uint32 *)&tnl_entry.entry_arr[0]);
            if (rv < 0) {
                return rv;
            }
            if (!soc_L3_TUNNELm_field32_get(unit, &tnl_entry, VALIDf)) {
                continue;
            }
            if (0x2 != soc_L3_TUNNELm_field32_get(unit, &tnl_entry, MODEf)) {
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
    }
#endif

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
bcmi_xgs5_mpls_tunnel_switch_get(int unit, bcm_mpls_tunnel_switch_t *info)
{
    int rv, index;
    mpls_entry_entry_t ment;
    soc_mem_t mem = MPLS_ENTRYm;

    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        mem = MPLS_ENTRY_SINGLEm;
    }
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_mpls_frr_lookup) && 
        (info->flags & BCM_MPLS_SWITCH_FRR)) { 
        rv = _bcm_tr3_mpls_tunnel_switch_frr_get(unit, info, &index);
    } else
#endif /* BCM_TOMAHAWK2_SUPPORT || BCM_APACHE_SUPPORT */
    {

        rv = _bcmi_xgs5_mpls_entry_set_key(unit, info, &ment);

        BCM_IF_ERROR_RETURN(rv);

        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                &ment, &ment, 0);

        if (rv < 0) {
            return rv;
        }

        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            rv =  bcmi_td3_mpls_entry_get_data(unit, &ment, info);
        } else {
            rv = _bcmi_xgs5_mpls_entry_get_data(unit, &ment, info);
        }
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
bcmi_xgs5_mpls_tunnel_switch_traverse(int unit, 
                                   bcm_mpls_tunnel_switch_traverse_cb cb,
                                   void *user_data)
{
    int rv, i;
    mpls_entry_entry_t *ment = NULL;
    bcm_mpls_tunnel_switch_t info;
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    soc_tunnel_term_t *tnl_entry = NULL;
    uint32 key_type=0;
    uint8 *tnl_entry_buf = NULL;
#endif
   int index_min, index_max;
    uint8 *mpls_entry_buf = NULL;
    soc_mem_t mem = MPLS_ENTRYm;
    soc_field_t validf = VALIDf;

    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        mem = MPLS_ENTRY_SINGLEm;
        validf = BASE_VALIDf;
    }

    mpls_entry_buf = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, mem), "MPLS_ENTRY buffer");
    if (NULL == mpls_entry_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
            index_min, index_max, mpls_entry_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    for (i = index_min; i <= index_max; i++) {
        ment = soc_mem_table_idx_to_pointer
            (unit, mem, mpls_entry_entry_t *, mpls_entry_buf, i);

        /* Check for valid entry */
        if (SOC_IS_TRIDENT3X(unit)) {
            if (soc_mem_field32_get(unit, mem, ment, BASE_VALID_0f) != 3) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem, ment, BASE_VALID_1f) != 7) {
                continue;
            }
        } else {
            if (!soc_mem_field32_get(unit, mem, ment, validf)) {
                continue;
            }
        }
        /* Check MPLS Key_type */
        if (soc_feature(unit, soc_feature_mpls_enhanced)) {
            if (0x0 != soc_mem_field32_get(unit, mem, ment, KEY_TYPEf)) {
                continue;
            }
        }
        if (soc_mem_field32_get(unit, mem, ment, MPLS_ACTION_IF_BOSf) ==
                _BCM_MPLS_XGS5_MPLS_ACTION_IF_BOS_L2_SVP) {
            continue;
        }
        sal_memset(&info, 0, sizeof(bcm_mpls_tunnel_switch_t));
        rv = _bcmi_xgs5_mpls_entry_get_key(unit, ment, &info);
        if (rv < 0) {
            goto cleanup;
        }

        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            rv =  bcmi_td3_mpls_entry_get_data(unit, ment, &info);
        } else {
            rv = _bcmi_xgs5_mpls_entry_get_data(unit, ment, &info);
        }
        if (rv < 0) {
            goto cleanup;
        }
        rv = cb(unit, &info, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            goto cleanup;
        }
#endif
    }

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_mpls_frr_lookup)) {

        /* traverse L3_TUNNEL entries */
        tnl_entry_buf = soc_cm_salloc(unit,
                SOC_MEM_TABLE_BYTES(unit, L3_TUNNELm), "L3_TUNNEL buffer");
        if (NULL == tnl_entry_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        index_min = soc_mem_index_min(unit, L3_TUNNELm);
        index_max = soc_mem_index_max(unit, L3_TUNNELm);
        rv = soc_mem_read_range(unit, L3_TUNNELm, MEM_BLOCK_ANY,
                index_min, index_max, tnl_entry_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        for (i = index_min; i <= index_max; i++) {
            tnl_entry = soc_mem_table_idx_to_pointer
                (unit, L3_TUNNELm, soc_tunnel_term_t *, tnl_entry_buf, i);

            if (!soc_L3_TUNNELm_field32_get (unit, tnl_entry, VALIDf)) {
                continue;
            }

            /* Check for MPLS entry key  */
            key_type = soc_L3_TUNNELm_field32_get(unit, tnl_entry, MODEf);
            if (key_type != 0x2) {
                continue;
            }

            sal_memset(&info, 0, sizeof(bcm_mpls_tunnel_switch_t));
            rv = _bcm_tr3_mpls_tunnel_switch_frr_entry_key_get (unit, tnl_entry, &info);
            if (rv < 0) {
                goto cleanup;
            }

            (void) _bcm_tr3_mpls_tunnel_switch_frr_parse(unit, tnl_entry, &info);

            rv = cb(unit, &info, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                goto cleanup;
            }
#endif
        }
    }
#endif /* BCM_TOMAHAWK2_SUPPORT || BCM_APACHE_SUPPORT */

cleanup:
    if (mpls_entry_buf) {
        soc_cm_sfree(unit, mpls_entry_buf);
    }
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (tnl_entry_buf) {
        soc_cm_sfree(unit, tnl_entry_buf);
    }
#endif /* BCM_TOMAHAWK2_SUPPORT || BCM_APACHE_SUPPORT */
    return rv;
}

/***************************************************
 ***************************************************
 ******                                       ******
 ******  Segment Routing Feature Development. ******
 */

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_free_indexes_init
 * Purpose:
 *      Initilize the free indexes.
 * Parameters:
 *      unit - Device Number
 *      fi   - Pointer to free index structure.
 * Returns:
 *      None.
 */
void
bcmi_egr_ip_tnl_mpls_free_indexes_init(int unit,
                bcmi_egr_ip_tnl_free_indexes_t *fi)
{

    int idx = 0,free_idx = 0;

    /* MAX_FREE_ENTRY = */
    for (;idx < MAX_ENTRY_INDEXES; idx++) {
        /* MAX FREE ENTRY COUNTING = 8 */
            free_idx=0;
        fi->free_index_count[idx]=0;
        for (; free_idx<MAX_FREE_ENTRY_INDEXES; free_idx++) {
            fi->free_entry_indexes[idx][free_idx] = -1;
        }

    }
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_free_indexes_clear
 * Purpose:
 *      clear free indexes structure in tunnel.
 *      When cleaning the full tunnel entry,
 *      we also need to free any free entry
 *      associated with this tunnel.
 * Parameters:
 *      unit   - Device Number
 *      tnl_id - (IN)Tunnel index.
 *      fi     - (IN)Pointer to free index structure.
 * Returns:
 *      None.
 */

void
bcmi_egr_ip_tnl_mpls_free_indexes_clear(int unit, int tnl_id,
                bcmi_egr_ip_tnl_free_indexes_t *fi)
{
    int idx = 0,free_idx = 0;
    int idx_min = tnl_id * _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);
    int idx_max = idx_min + _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);
    int entry_count = 0;

    for (idx=0;idx < MAX_ENTRY_INDEXES; idx++) {
        /* MAX FREE ENTRY COUNTING = 8 */
        entry_count = fi->free_index_count[idx];
        if (entry_count) {
            for (free_idx=0; free_idx<MAX_FREE_ENTRY_INDEXES; free_idx++) {

                entry_count = fi->free_index_count[idx];
                if (fi->free_entry_indexes[idx][free_idx] == -1) {
                    continue;
                }

                if ((fi->free_entry_indexes[idx][free_idx] >= idx_min) &&
                    (fi->free_entry_indexes[idx][free_idx] < idx_max)) {


                    if (free_idx == (entry_count - 1)) {
                        /* This is the last entry in free list. just clean it.*/
                        fi->free_entry_indexes[idx][free_idx] = -1;
                    } else {
                        fi->free_entry_indexes[idx][free_idx] =
                            fi->free_entry_indexes[idx][entry_count -1];

                        fi->free_entry_indexes[idx][entry_count -1] = -1;
                    }
                    fi->free_index_count[idx]--;
                    free_idx = -1;
                }
            }
        }
    }
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_free_indexes_dump
 * Purpose:
 *      Dump free indexes.
 * Parameters:
 *      unit - Device Number
 *      fi   - Pointer to free index structure.
 * Returns:
 *      None.
 */
void
bcmi_egr_ip_tnl_mpls_free_indexes_dump(int unit,
                bcmi_egr_ip_tnl_free_indexes_t *fi)
{

    int idx = 0,free_idx = 0;

    /* MAX_FREE_ENTRY = */
    for (;idx < MAX_ENTRY_INDEXES; idx++) {
        /* MAX FREE ENTRY COUNTING = 8 */
        free_idx=0;
        LOG_ERROR(BSL_LS_BCM_L3,
            (BSL_META_U(unit, "%s:%d: tnl idx = %d, count = %d \n"),
            FUNCTION_NAME(), __LINE__, idx, fi->free_index_count[idx]));

       for (; free_idx<MAX_FREE_ENTRY_INDEXES; free_idx++) {
            if (fi->free_entry_indexes[idx][free_idx] == -1) {
                continue;
            }

            LOG_CLI((BSL_META_U(unit, "start idx[%d] = %d "),
                free_idx, fi->free_entry_indexes[idx][free_idx]));
        }

        LOG_CLI((BSL_META_U(unit, "\n")));
    }
}

/*
 * Function:
 *      bcmi_egr_ip_tunnel_mpls_sw_cleanup
 * Purpose:
 *      cleanup and deallocate sw state for mpls tunnel.
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCM_E_NONE.
 */
int
bcmi_egr_ip_tunnel_mpls_sw_cleanup(int unit)
{
    bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tnl_sw_entry;
    int num_ip_tnl_mpls;
    int i,j;
    soc_mem_t mem;

    mem = bcmi_egr_ip_tnl_mem_name_get(unit);

    num_ip_tnl_mpls = soc_mem_index_count(unit, mem);

    tnl_sw_entry = egr_mpls_tnl_sw_state[unit];

    if (tnl_sw_entry == NULL) {
        return BCM_E_NONE;
    }

    for (i=0; i<num_ip_tnl_mpls; i++) {

        if (tnl_sw_entry[i] == NULL) {
            continue;
        }

        if (tnl_sw_entry[i]->label_entry == NULL) {
            continue;
        }

        for (j=0; j<_BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit); j++) {

            if (tnl_sw_entry[i]->label_entry[j] == NULL) {
                continue;
            }

            sal_free(tnl_sw_entry[i]->label_entry[j]);
            tnl_sw_entry[i]->label_entry[j] = NULL;
        }

        sal_free(tnl_sw_entry[i]->label_entry);
        tnl_sw_entry[i]->label_entry = NULL;

        sal_free(tnl_sw_entry[i]);
        tnl_sw_entry[i] = NULL;
    }

    sal_free(tnl_sw_entry);
    tnl_sw_entry = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_egr_ip_tunnel_mpls_sw_init
 * Purpose:
 *      initialize s/w state for egress mpls tunnel.
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCME_XXX.
 */
int
bcmi_egr_ip_tunnel_mpls_sw_init(int unit)
{
    int num_ip_tnl_mpls;
    int i,j;
    int ent_per_tnl = _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);
    soc_mem_t mem;

    mem = bcmi_egr_ip_tnl_mem_name_get(unit);

    num_ip_tnl_mpls = soc_mem_index_count(unit, mem);

    egr_mpls_tnl_sw_state[unit] = (bcmi_egr_ip_tnl_mpls_tunnel_entry_t **)
        sal_alloc((num_ip_tnl_mpls *
            sizeof(bcmi_egr_ip_tnl_mpls_tunnel_entry_t *)),
        "egress mpls tunnel sw state");

    if (egr_mpls_tnl_sw_state[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(egr_mpls_tnl_sw_state[unit], 0,
        (num_ip_tnl_mpls * sizeof(bcmi_egr_ip_tnl_mpls_tunnel_entry_t *)));

    for (i=0; i<num_ip_tnl_mpls; i++) {
        egr_mpls_tnl_sw_state[unit][i] = (bcmi_egr_ip_tnl_mpls_tunnel_entry_t *)
            sal_alloc((sizeof(bcmi_egr_ip_tnl_mpls_tunnel_entry_t)),
            "egress mpls tunnel entry");

        sal_memset(egr_mpls_tnl_sw_state[unit][i], 0,
            sizeof(bcmi_egr_ip_tnl_mpls_tunnel_entry_t));

        if (egr_mpls_tnl_sw_state[unit][i] == NULL) {
            bcmi_egr_ip_tunnel_mpls_sw_cleanup(unit);
            return BCM_E_MEMORY;
        }

        egr_mpls_tnl_sw_state[unit][i]->label_entry =
            (bcmi_egr_ip_tnl_mpls_label_entry_t **)
            sal_alloc((ent_per_tnl *
                sizeof(bcmi_egr_ip_tnl_mpls_label_entry_t *)),
            "egress mpls tunnel label bucket");

        if (egr_mpls_tnl_sw_state[unit][i]->label_entry == NULL) {
            bcmi_egr_ip_tunnel_mpls_sw_cleanup(unit);
            return BCM_E_MEMORY;
        }

        for (j=0; j<_BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit); j++) {
            egr_mpls_tnl_sw_state[unit][i]->label_entry[j] =
                (bcmi_egr_ip_tnl_mpls_label_entry_t *)
                sal_alloc((sizeof(bcmi_egr_ip_tnl_mpls_label_entry_t)),
                "egress mpls tunnel label entry");

            if (egr_mpls_tnl_sw_state[unit][i]->label_entry[j] == NULL) {
                bcmi_egr_ip_tunnel_mpls_sw_cleanup(unit);
                return BCM_E_MEMORY;
            }

            egr_mpls_tnl_sw_state[unit][i]->label_entry[j]->intf_list = NULL;
            egr_mpls_tnl_sw_state[unit][i]->label_entry[j]->flags = 0;
            egr_mpls_tnl_sw_state[unit][i]->label_entry[j]->num_elements = 0;
        }
    }
    bcmi_egr_ip_tnl_mpls_free_indexes_init(unit, &(fi_db[unit]));
    return 0;
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_tunnel_ref_count_get
 * Purpose:
 *      get the current reference Count
 * Parameters:
 *      unit              Unit number
 *      index            (IN) Entry Index
 */

STATIC void
bcmi_egr_ip_tnl_mpls_tunnel_ref_count_get(int unit, int index, int *cnt_value)
{
    bcmi_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);

    *cnt_value = mpls_info->egr_tunnel_ref_count[index];
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_ref_count_adjust
 * Purpose:
 *      Increment / Decrement Reference Count for Egress Tunnel
 * Parameters:
 *      unit              Unit number
 *      index            (IN) Entry Index
 *      step             (IN) no of references
 */

STATIC void
bcmi_egr_ip_tnl_mpls_ref_count_adjust (int unit, int index, int step)
{
    bcmi_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);

    if (( mpls_info->egr_tunnel_ref_count[index] == 0 ) && (step < 0)) {
         mpls_info->egr_tunnel_ref_count[index] = 0;
    } else {
         mpls_info->egr_tunnel_ref_count[index] += step;
    }

}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_ref_count_reset
 * Purpose:
 *      reset the reference Count
 * Parameters:
 *      unit              Unit number
 *      index             (IN) Entry Index
 */

STATIC void
bcmi_egr_ip_tnl_mpls_ref_count_reset (int unit, int index)
{
    bcmi_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);

    mpls_info->egr_tunnel_ref_count[index] = 0;
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_check_dup_free_index
 * Purpose:
 *      check for duplicate free index.
 * Parameters:
 *      unit              Unit number
 *      fi                (IN) free index structure.
 *      free_entry_idx    (IN) free index where we should look.
 *      mpls_tnl_idx      (IN) original mpls tnl idx.
 * Return
 *      TRUE/FALSE;
 */
int
bcmi_egr_ip_tnl_mpls_check_dup_free_index(int unit,
                      bcmi_egr_ip_tnl_free_indexes_t *fi,
                      int free_entry_num, int mpls_tnl_idx)
{

    int free_idx=0;
    int ent_idx = free_entry_num - 1;

    for (; free_idx < 8; free_idx++) {
        if (fi->free_index_count[ent_idx]) {
            if (fi->free_entry_indexes[ent_idx][free_idx] == -1) {
                break;
            }
            if (fi->free_entry_indexes[ent_idx][free_idx] == mpls_tnl_idx) {
                return TRUE;
            }

        }
    }
    return FALSE;
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_free_idx_update
 * Purpose:
 *      check for duplicate free index.
 * Parameters:
 *      unit              Unit number
 *      fi                (IN) free index structure.
 *      free_entries      (IN) no of free entries.
 *      start_index       (IN) start index.
 * Return
 *      TRUE/FALSE;
 */
void
bcmi_egr_ip_tnl_mpls_free_idx_update(int unit, int free_entries,
        int start_index, bcmi_egr_ip_tnl_free_indexes_t *fi)
{
    int free_entry_count;

    if (!(bcmi_egr_ip_tnl_mpls_check_dup_free_index(unit,
        fi, free_entries, start_index))) {

        if (fi->free_index_count[free_entries-1] < MAX_FREE_ENTRY_INDEXES) {
            free_entry_count = fi->free_index_count[free_entries-1];
            fi->free_entry_indexes[free_entries-1][free_entry_count] = start_index;
            fi->free_index_count[free_entries-1]++;
        }
    }
}

/*
 * Function:
 *      bcm_egr_ip_tnl_mpls_remark_free_indexes
 * Purpose:
 *      Mark correct free indexes for mpls tunnel entries.
 * Parameters:
 *      unit         - Device Number
 *      free_entries - number of free entries
 *      start_index  - Start index of tunnel entry
 *      fi           - free indexes database pointer.
 * Returns:
 *      BCM_E_XXX
 */
bcm_error_t
bcm_egr_ip_tnl_mpls_remark_free_indexes(int unit, int free_entries,
        int start_index, bcmi_egr_ip_tnl_free_indexes_t *fi)
{
    int temp_free_entries;
    int ent_per_tnl = _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);

    if (free_entries == 0) {
        return BCM_E_NONE;
    }
    if (start_index == -1) {
        return BCM_E_NONE;
    }

    /*
     * if start index is 0, then any entry can be put there.
     * if start index is 4, then only 3 and 4 number of entries
     * can be put there.
     */
    if (((start_index % ent_per_tnl) == 0) ||
        (((start_index %
            (ent_per_tnl / 2)) == 0) &&
        ((free_entries > 2) && free_entries < 5))) {

        bcmi_egr_ip_tnl_mpls_free_idx_update(unit, free_entries,
            start_index, fi);
    } else {
        if ((free_entries >= 4)
                && ((((start_index + free_entries) %  ent_per_tnl) == 0))) {

            /*
             * the entry is free till end and has more
             * than 4 consecutive spaces then
             * break them into smaller and take
             * 4 spaces into single big entry.
             */
            temp_free_entries = free_entries - 4;
            while(temp_free_entries > 0) {
                if (temp_free_entries == 1) {
                    bcmi_egr_ip_tnl_mpls_free_idx_update(unit, 1,
                        start_index, fi);

                    start_index++;
                    temp_free_entries--;
                } else if (temp_free_entries >= 2) {

                    bcmi_egr_ip_tnl_mpls_free_idx_update(unit, 2,
                        start_index, fi);

                    start_index +=2;
                    temp_free_entries-=2;
                }
            }
            if (!(bcmi_egr_ip_tnl_mpls_check_dup_free_index(unit,
                fi, 4, start_index))) {

                bcmi_egr_ip_tnl_mpls_free_idx_update(unit, 4, start_index, fi);
                start_index +=4;
                temp_free_entries-=4;
            }
        } else if ((free_entries >= 3)  &&
            ((((start_index + free_entries) %  ent_per_tnl) == 7))) {
            /*
             * the entry is free till second last and has more
             * than 3 consecutive spaces then break them into smaller
             * and take 3 spaces into single big entry.
             */
            temp_free_entries = free_entries - 3;
            while(temp_free_entries > 0) {
                if (temp_free_entries == 1) {
                    bcmi_egr_ip_tnl_mpls_free_idx_update(unit, 1,
                        start_index, fi);

                    start_index++;
                    temp_free_entries--;
                } else if (temp_free_entries >= 2) {
                    bcmi_egr_ip_tnl_mpls_free_idx_update(unit, 2,
                        start_index, fi);

                    start_index +=2;
                    temp_free_entries-=2;
                }
            }
            bcmi_egr_ip_tnl_mpls_free_idx_update(unit, 3, start_index, fi);
            start_index +=3;
            temp_free_entries-=3;
        } else {
            temp_free_entries = free_entries;
            while(temp_free_entries > 0) {
                if (temp_free_entries == 1) {
                    bcmi_egr_ip_tnl_mpls_free_idx_update(unit, 1,
                        start_index, fi);

                    start_index++;
                    temp_free_entries--;
                } else if (temp_free_entries >= 2) {
                    bcmi_egr_ip_tnl_mpls_free_idx_update(unit, 2,
                        start_index, fi);

                    start_index +=2;
                    temp_free_entries-=2;
                }
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_egr_ip_tnl_mpls_move_entries
 * Purpose:
 *      Move enties to create space for other entry.
 * Parameters:
 *      unit               - Device Number
 *      src_tnl_index      - Tunnel start index.
 *      src_entry_offset   - Mpls entry offset.
 *      dst_free_mpls_idx  - Dest mpls tunnel entry index.
 *      no_of_elements     - Number of elements to move.
 * Returns:
 *      BCM_E_XXX
 */
bcm_error_t
bcm_egr_ip_tnl_mpls_move_entries(int unit,
                        bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tnl_sw_entry,
                        int src_tnl_index, int src_entry_offset,
                        int dst_free_mpls_idx, int no_of_elements)
{

    int dst_offset;
    int src_offset;
    int i = 0;
    int ent_per_tnl = _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);
    int src_tnl_mpls_idx, dst_tnl_index;
    int ref_count;
    int rv;
    uint32 l3_max_entry[SOC_MAX_MEM_WORDS]; /* Buffer to write interface info. */
    uint32  *l3_if_entry_p;        /* Write buffer address.           */
    soc_mem_t mem;                 /* Interface table memory.         */
    intf_list_t *intf_list = NULL;
    uint32 src_tnl_entry[SOC_MAX_MEM_WORDS];
    uint32 dst_tnl_entry[SOC_MAX_MEM_WORDS];
    uint32 src_buf[3] = {0};

    bcmi_egr_ip_tnl_mpls_table_memset(unit, src_tnl_entry);
    bcmi_egr_ip_tnl_mpls_table_memset(unit, dst_tnl_entry);

    /* Get interface table memory. */
    mem = BCM_XGS3_L3_MEM(unit,  intf);

    /* Zero the buffer. */
    l3_if_entry_p = (uint32 *)&l3_max_entry;

    /* Compute offsets */
    src_offset = src_entry_offset;
    dst_offset = dst_free_mpls_idx % ent_per_tnl;
    dst_tnl_index = dst_free_mpls_idx / ent_per_tnl;

     /*Read src_tnl_entry*/
    bcmi_egr_ip_tnl_mpls_table_read(unit, src_tnl_index, src_tnl_entry);

     /*Read dst_entry*/
    bcmi_egr_ip_tnl_mpls_table_read(unit, dst_tnl_index, dst_tnl_entry);

    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
        for (; i < no_of_elements; i++) {
            /*Get Label,Pushaction,expselect,exp,pri,cfi,expptr,ttl into src_buf
             *and write src_buf into dst_entry_double.*/
            bcmi_egress_tnl_label_entry_get(unit , src_tnl_entry,
                    src_offset+i, src_buf);
            bcmi_egress_tnl_label_entry_set(unit , dst_tnl_entry,
                    dst_offset+i, src_buf);
        }
    } else {
        bcmi_egr_ip_tnl_copy_src_label_info_to_dest(unit, src_tnl_entry,
                dst_tnl_entry, src_offset, dst_offset, no_of_elements);
    }

    rv = bcmi_egr_ip_tnl_mpls_table_write(unit, dst_tnl_index, dst_tnl_entry);

    if (rv == BCM_E_NONE) {
        src_tnl_mpls_idx = (src_tnl_index * ent_per_tnl) + src_entry_offset;
        for (i=0; i < no_of_elements; i++) {
            _BCM_MPLS_TNL_USED_SET(unit, dst_free_mpls_idx + i);
            _BCM_MPLS_TNL_USED_CLR(unit, src_tnl_mpls_idx + i);
        }
        /* get the moved entry's original ref counter number */
        bcmi_egr_ip_tnl_mpls_tunnel_ref_count_get (unit,
            src_tnl_mpls_idx, &ref_count);
        bcmi_egr_ip_tnl_mpls_ref_count_reset (unit, src_tnl_mpls_idx);
        bcmi_egr_ip_tnl_mpls_ref_count_adjust (unit,
            dst_free_mpls_idx, ref_count);

        /*
         * Now change tunnel index in L3 interface entry.
         */
        intf_list = tnl_sw_entry[src_tnl_index]->label_entry[src_entry_offset]->intf_list;

        while (intf_list) {
            sal_memset(l3_if_entry_p, 0, BCM_XGS3_L3_ENT_SZ(unit, intf));
            BCM_IF_ERROR_RETURN(
                BCM_XGS3_MEM_READ(unit, mem, intf_list->intf_num, l3_if_entry_p));

            soc_mem_field32_set(unit, mem, l3_if_entry_p,
                MPLS_TUNNEL_INDEXf, dst_free_mpls_idx);

            rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, intf_list->intf_num, l3_if_entry_p);

            if (BCM_FAILURE(rv)) {
                return rv;
            }
            intf_list = intf_list->next;
        }

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_intf_list_alloc
 * Purpose:
 *      Allocates link memory and sets interface chain.
 * Parameters:
 *      intf_num - interface number
 * Returns:
 *      pointer to allocated link memory
 */
intf_list_t *
bcmi_egr_ip_tnl_mpls_intf_list_alloc(bcm_if_t intf_num)
{
    intf_list_t *ptr;

    ptr = (intf_list_t *) sal_alloc(sizeof(intf_list_t), "egr mpls tnl sw intf list");
    sal_memset(ptr, 0, sizeof(intf_list_t));

    ptr->intf_num = intf_num;
    ptr->next = NULL;

    return ptr;
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_intf_list_dump
 * Purpose:
 *      Dumps interface chain.
 * Parameters:
 *      unit         - device number
 *      tnl_sw_entry - pointer to tunnel database.
 *      intf_num     - interface number
 *      tnl_idx      - Tunnel Index
 *      mpls_offset  - mpls entry offset in tunnel
 * Returns:
 *      None.
 */
void
bcmi_egr_ip_tnl_mpls_intf_list_dump(int unit,
                    bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tnl_sw_entry,
                    bcm_if_t intf_num, int tnl_idx, int mpls_offset)
{

   intf_list_t *temp;

    temp = tnl_sw_entry[tnl_idx]->label_entry[mpls_offset]->intf_list;

    while (temp)  {
       LOG_ERROR(BSL_LS_BCM_L3,
           (BSL_META_U(unit, "intf index = %d, tnl_idx = %d, mpls_off = %d\n"),
            temp->intf_num, tnl_idx, mpls_offset));

        temp = temp->next;
    }
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_intf_list_add
 * Purpose:
 *      Adds interface chain at a mpls entry index.
 * Parameters:
 *      unit         - device number
 *      tnl_sw_entry - pointer to tunnel database.
 *      intf_id      - interface number
 *      tnl_idx      - Tunnel Index
 *      mpls_offset  - mpls entry offset in tunnel
 * Returns:
 *      bcm_error_t
 */
bcm_error_t
bcmi_egr_ip_tnl_mpls_intf_list_add(int unit,
                    bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tnl_sw_entry,
                    bcm_if_t intf_id, int tnl_idx, int mpls_offset)
{

    intf_list_t *ptr, *temp, *prev;

    temp = tnl_sw_entry[tnl_idx]->label_entry[mpls_offset]->intf_list;
    prev = temp;
    while (temp) {
        if (temp->intf_num == intf_id) {
        /*
         * We already have this interface at this index.
         * this is just a negative case test if
         * someone tries to send same entry at same intf.
         */
            return BCM_E_NONE;
        }
        prev = temp;
        temp = temp->next;
    }

    ptr = bcmi_egr_ip_tnl_mpls_intf_list_alloc(intf_id);

    if (ptr == NULL) {
        return BCM_E_MEMORY;
    }

    if(temp == prev) {
        tnl_sw_entry[tnl_idx]->label_entry[mpls_offset]->intf_list = ptr;
    } else {
        prev->next = ptr;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_intf_list_delete
 * Purpose:
 *      Deletes interface chain matching to a
 *      particular interface number.
 * Parameters:
 *      unit         - device number
 *      tnl_sw_entry - pointer to tunnel database.
 *      intf_num     - interface number
 *      tnl_idx      - Tunnel Index
 *      mpls_offset  - mpls entry offset in tunnel
 * Returns:
 *      bcm_error_t
 */
bcm_error_t
bcmi_egr_ip_tnl_mpls_intf_list_delete(int unit, bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tnl_sw_entry,
                    bcm_if_t intf_num, int tnl_idx, int mpls_offset)
{

    intf_list_t *temp_ptr, *prev_ptr;

    if(tnl_sw_entry[tnl_idx]->label_entry[mpls_offset]->intf_list == NULL) {
        /* why we are here. this is not possible */
        return BCM_E_INTERNAL;
    }

    temp_ptr = tnl_sw_entry[tnl_idx]->label_entry[mpls_offset]->intf_list;
    prev_ptr = temp_ptr;

    while ((temp_ptr) && ((temp_ptr->intf_num) != intf_num)) {
        prev_ptr = temp_ptr;
        temp_ptr = temp_ptr->next;
    }

    if (temp_ptr) {
        if (temp_ptr == prev_ptr) {
            tnl_sw_entry[tnl_idx]->label_entry[mpls_offset]->intf_list = temp_ptr->next;
        } else {
            prev_ptr->next = temp_ptr->next;
        }

        sal_free(temp_ptr);
    } else {
        return BCM_E_NOT_FOUND;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_intf_list_delete_all
 * Purpose:
 *      Deletes all nodex of Interface chain at that mpls
 *      entry index.
 * Parameters:
 *      unit         - device number
 *      tnl_sw_entry - pointer to tunnel database.
 *      intf_num     - interface number
 *      tnl_idx      - Tunnel Index
 *      mpls_offset  - mpls entry offset in tunnel
 * Returns:
 *      bcm_error_t
 */

void
bcmi_egr_ip_tnl_mpls_intf_list_delete_all(int unit,
                    bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tnl_sw_entry,
                    int tnl_idx, int mpls_offset)
{

    intf_list_t *temp_ptr;

    temp_ptr = tnl_sw_entry[tnl_idx]->label_entry[mpls_offset]->intf_list;

    while (temp_ptr) {
        tnl_sw_entry[tnl_idx]->label_entry[mpls_offset]->intf_list =
            temp_ptr->next;

        sal_free(temp_ptr);
        temp_ptr = tnl_sw_entry[tnl_idx]->label_entry[mpls_offset]->intf_list;
    }
}

#if 0
void
bcmi_egr_ip_tnl_mpls_intf_list_copy(int unit,
                     bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tnl_sw_entry,
                     int dst_tnl_idx, int dst_mpls_offset,
                     int tnl_idx, int mpls_offset)
{


    /* we can just change the pointer and whole list can be picked from there */
    tnl_sw_entry[dst_tnl_idx]->label_entry[dst_mpls_offset]->intf_list =
        tnl_sw_entry[tnl_idx]->label_entry[mpls_offset]->intf_list;

    tnl_sw_entry[tnl_idx]->label_entry[mpls_offset]->intf_list = NULL;
}
#endif

/*
 * Function:
 *      bcm_egr_ip_tnl_mpls_sw_entry_reset
 * Purpose:
 *      Dumps interface chain.
 * Parameters:
 *      unit         - device number
 *      tnl_sw_entry - pointer to tunnel database.
 *      tnl_idx      - Tunnel Index
 *      mpls_offset  - mpls entry offset in tunnel
 *      clean_list   _ flag to decide whether to clean interface list.
 * Returns:
 *      bcm_error_t
 */


void bcm_egr_ip_tnl_mpls_sw_entry_reset(int unit,
                    bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tnl_sw_entry,
                    int tnl_idx, int mpls_offset, int clean_list)
{

    if (clean_list) {
        tnl_sw_entry[tnl_idx]->label_entry[mpls_offset]->intf_list = NULL;
    }
    tnl_sw_entry[tnl_idx]->label_entry[mpls_offset]->flags = 0;
    tnl_sw_entry[tnl_idx]->label_entry[mpls_offset]->num_elements = 0;
}

/*
 * Function:
 *      bcmi_mpls_egr_tunnel_delete_free_indexes
 * Purpose:
 *      Delete free index entry from free indexes if the entry is used up.
 * Parameters:
 *      unit           - device number
 *      fi             - Pointer to free indexes database.
 *      num_labels     - Number of labels that entry contains.
 *      mpls_entry_idx - index to mpls entry index that is used up.
 * Returns:
 *      None.
 */
void
bcmi_mpls_egr_tunnel_delete_free_indexes(int unit,
                    bcmi_egr_ip_tnl_free_indexes_t *fi,
                    int num_labels, int mpls_entry_idx)
{
    int idx=0,free_idx=0;
    int entry_count = 0;
    int idx_min=0, idx_max=0;


    idx_min = mpls_entry_idx;
    idx_max = mpls_entry_idx + num_labels;

    if ((num_labels <= 0) || (num_labels > 8)) {
        return;
    }

    for (; idx<MAX_ENTRY_INDEXES; idx++) {

        if (!(fi->free_index_count[idx])) {
            continue;
        }

        for (free_idx=0; free_idx<MAX_FREE_ENTRY_INDEXES; free_idx++) {
            /* need to keep it here as we are chaning counts for next run. */
            entry_count = fi->free_index_count[idx];

            if (fi->free_entry_indexes[idx][free_idx] == -1) {
                break;
            }

            if ((fi->free_entry_indexes[idx][free_idx] >= idx_min) &&
                (fi->free_entry_indexes[idx][free_idx] < idx_max)) {
                /* free this space */
                if (free_idx == (entry_count - 1)) {
                    fi->free_entry_indexes[idx][free_idx] = -1;
                } else {
                    fi->free_entry_indexes[idx][free_idx] =
                        fi->free_entry_indexes[idx][entry_count-1];
                    fi->free_entry_indexes[idx][entry_count-1] = -1;
                   /*
                    * We should check free_idx from start again here.
                    * as some matching index may have come to this slot.
                    */
                   free_idx = -1;
                }
                fi->free_index_count[idx]--;
            }
        }
    }
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_move_tunnel_entries
 * Purpose:
 *      Once we identify that this is the tunnel index that can create
 *      space for our new entry, we try to move the mple tunnel entries
 *      from that tunnel index.
 * Parameters:
 *      unit           - (IN)device number
 *      fi             - (IN)Pointer to free indexes database.
 *      tunnel_entry   - (IN)Number of labels that entry contains.
 *      idx            - (IN)index to mpls entry index that is used up.
 *      num_labels     - (IN)Number of labels for which free space is made.
 *      mpls_tnl_idx   - (IN)mpls offset from where entry needs to be moved.
 * Returns:
 *      None.
 */
int
bcmi_egr_ip_tnl_mpls_move_tunnel_entries(int unit,
                      bcmi_egr_ip_tnl_free_indexes_t *fi,
                      bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tunnel_entry,
                      int src_tnl_idx, int src_mpls_tnl_off, int num_labels)
{
    int ent_per_idx = _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);
    int temp_num_labels = num_labels;
    int left_free_entries = 0;
    int found = 0;
    int rv = BCM_E_NONE;
    int mpls_index = 0;
    int mpls_off = 0;
    int no_of_elements = 0;
    int f_idx, free_idx;
    int new_start_index;
    int chk_tnl_idx, chk_mpls_idx;
    bcmi_egr_ip_tnl_mpls_label_entry_t *label_entry;
    bcmi_egr_ip_tnl_mpls_label_entry_t *src_label_entry;
    int free_db_entries = 0;
    int total_free_entries = 0;


    mpls_index = (src_tnl_idx * _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit));
    /* check for entry free */
    for(mpls_off=src_mpls_tnl_off; mpls_off < ent_per_idx; mpls_off++) {
        found = 0;

        if (!_BCM_MPLS_TNL_USED_GET(unit, (mpls_index + mpls_off))) {
            temp_num_labels--;
            free_db_entries++;
            total_free_entries++;
            continue;
        }

        /* If we are looking at same index as free index. then continue. */
        if (free_db_entries) {
            bcmi_mpls_egr_tunnel_delete_free_indexes(unit,
                fi, free_db_entries, (mpls_index + mpls_off - free_db_entries));
        }
        free_db_entries = 0;
        src_label_entry = tunnel_entry[src_tnl_idx]->label_entry[mpls_off];
        if(src_label_entry->flags
            & BCMI_EGR_IP_TUNNEL_MPLS_START_OF_ENTRY) {
            /* this is the start of this entry,
             * we should check if we have some free space for this entry
             */
            no_of_elements = src_label_entry->num_elements;

            /* we also should check if there are some bigger entries in this
             * tunnel entry, big entries can be adjusted to other big entries
             * if exists.
             * else we might have to move smaller entries to bigger entries.
             */
            for (f_idx=no_of_elements-1; f_idx < 8; f_idx++) {

                if (fi->free_index_count[f_idx] == 0) {
                    continue;
                }

                for (free_idx=0; free_idx<8; free_idx++) {
                    /* There is some empty place here.
                     * lets use the same size entry or bigger.
                     * returning the first free entry in that block.
                     */
                    if (fi->free_entry_indexes[f_idx][free_idx] == -1) {
                        break;
                    }
                    chk_tnl_idx  = fi->free_entry_indexes[f_idx][free_idx] /
                        _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);

                    chk_mpls_idx =  fi->free_entry_indexes[f_idx][free_idx] %
                        _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);

                    label_entry =
                        tunnel_entry[chk_tnl_idx]->label_entry[chk_mpls_idx];
                    if (!(label_entry->flags &
                        BCMI_EGR_IP_TUNNEL_MPLS_ENTRY_CHECKED)) {

                        /*
                         * we can use free_entry_indexes[idx][free_idx];
                         * mark this entry used so that next lookup should
                         * not use this.
                         */
                        label_entry->flags |=
                            BCMI_EGR_IP_TUNNEL_MPLS_ENTRY_CHECKED;
                        found = 1;
                        rv = bcm_egr_ip_tnl_mpls_move_entries(unit,
                                    tunnel_entry, src_tnl_idx, mpls_off,
                                    fi->free_entry_indexes[f_idx][free_idx],
                                    no_of_elements);

                        if (rv == BCM_E_NONE) {
                        /* movement is successful */
                        label_entry->flags |=
                                BCMI_EGR_IP_TUNNEL_MPLS_START_OF_ENTRY;
                        label_entry->num_elements = no_of_elements;
                        /* now copy interfaces.
                         * even if we are keeping pointer here for linked list,
                         * we can assign the same pointer to the new list.
                         */
                        label_entry->intf_list = src_label_entry->intf_list;
                        bcm_egr_ip_tnl_mpls_sw_entry_reset(unit, tunnel_entry,
                            src_tnl_idx, mpls_off, 1);
                        } else {
                            return rv;
                        }

                        temp_num_labels -= no_of_elements;
                        /* mpls_off will also get incremented in this loop. */
                        mpls_off += (no_of_elements - 1);
                        new_start_index =
                            fi->free_entry_indexes[f_idx][free_idx] +
                            no_of_elements;

                        bcmi_mpls_egr_tunnel_delete_free_indexes(unit, fi,
                            no_of_elements,
                            fi->free_entry_indexes[f_idx][free_idx]);

                        left_free_entries = (f_idx+1)-no_of_elements;
                        total_free_entries += no_of_elements;
                        bcm_egr_ip_tnl_mpls_remark_free_indexes(unit,
                            left_free_entries, new_start_index, fi);

                        break;
                    }
                }
                if (found) {
                    /*break from loop so that we can check next entry*/
                    break;
                }
            }
            if (!found) {
                temp_num_labels = num_labels;
            }
        }
        if (temp_num_labels <= 0) {

            /* we have moved all the entries to some other space.
             * we have this space free now.
             */
            bcm_egr_ip_tnl_mpls_remark_free_indexes(unit,
                (total_free_entries - num_labels),
                ((src_tnl_idx*ent_per_idx) + src_mpls_tnl_off + num_labels), fi);

            return BCM_E_NONE;
        }
     }
    /* We cant not free any entry here. so return BCM_E_FULL */
    return BCM_E_FULL;
}

/*
 * Function:
 *           bcmi_xgs5_mpls_egr_tunnel_lookup
 * Purpose:
 *           loopup matching entry into  EGR_IP_TUNNEL_MPLS
 * Parameters:
 *           IN :  Unit
 *           IN :  push_action
 *           IN :  label_array
 *           OUT : match_mpls_tunnel_index
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
bcmi_xgs5_mpls_egr_tunnel_lookup (int unit, int push_action,
                              bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tunnel_entry,
                              bcm_mpls_egress_label_t *label_array,
                              int *match_mpls_tunnel_index)
{
    int rv = BCM_E_NONE;
    uint32 label_value;
    uint32 entry_ttl, entry_exp, entry_push_action;
    int num_ip_tnl_mpls;
    int i = 0, j = 0, tnl_index = -1, mpls_index = -1;
    int temp_push_action = 0;
    int offset;
    soc_mem_t  mem;
    uint32 tnl_entry[SOC_MAX_MEM_WORDS];
    uint32 iptun_mpls_entry_format[3] = {0};

    mem = bcmi_egr_ip_tnl_mem_name_get(unit);
    num_ip_tnl_mpls = soc_mem_index_count(unit, mem);

    /* parameter checking */
    if (push_action != 0 && label_array == NULL) {
        return BCM_E_PARAM;
    }

    for (tnl_index = 0; tnl_index < num_ip_tnl_mpls; tnl_index++) {
        if (!_BCM_MPLS_IP_TNL_USED_GET(unit, tnl_index)) {
           continue;
        }

        mpls_index = tnl_index * _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);

        for (i = 0; i <  _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit); i++) {
            if (!(tunnel_entry[tnl_index]->label_entry[i]->flags &
                BCMI_EGR_IP_TUNNEL_MPLS_START_OF_ENTRY)) {
                continue;
            }
            if (!push_action) {
                if ((tunnel_entry[tnl_index]->label_entry[i]->flags &
                    BCMI_EGR_IP_TUNNEL_MPLS_ENTRY_ZERO)) {
                    *match_mpls_tunnel_index = (tnl_index *
                        _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit)) + i;
                    return BCM_E_NONE;
                }
            }

            if (!(tunnel_entry[tnl_index]->label_entry[i]->num_elements ==
                push_action)) {
                continue;
            }

            rv = bcmi_egr_ip_tnl_mpls_table_read(unit, tnl_index, tnl_entry);
            if (rv < 0) {
                return rv;
            }

            if (push_action > 1) {
                j = 0;
                temp_push_action = push_action;

                while (temp_push_action > 1) {
                    if (!(_BCM_MPLS_TNL_USED_GET(unit, mpls_index + i + j))) {
                        break;
                    }


                    offset = (i+j);
                    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                        bcmi_egress_tnl_label_entry_get(unit, tnl_entry,
                                offset, iptun_mpls_entry_format);
                    }

                    bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                            tnl_entry, offset,
                            iptun_mpls_entry_format, bcmiMplsLabel,
                            &label_value);

                    bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                            tnl_entry, offset,
                            iptun_mpls_entry_format, bcmiMplsTtl,
                            &entry_ttl);

                    bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                            tnl_entry, offset,
                            iptun_mpls_entry_format, bcmiMplsExp,
                            &entry_exp);

                    bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                            tnl_entry, offset,
                            iptun_mpls_entry_format, bcmiMplsPushAction,
                            &entry_push_action);
                    /*
                     * entry_push_action should be 2 as next entry will also
                     * be checked with this entry.
                     */
                    if ((label_array[j].label == label_value) &&
                        (label_array[j].ttl == entry_ttl) &&
                        (label_array[j].exp == entry_exp) &&
                        (entry_push_action == 2)) {
                         /* Do nothing. This is good case so continue */
                         temp_push_action--;
                    } else {
                         /* we are not getting matching entry here */
                         break;
                    }
                    j++;
                 }

                offset = (i+j);

                if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                    sal_memset(&iptun_mpls_entry_format, 0,
                            sizeof(iptun_mpls_entry_format));

                    bcmi_egress_tnl_label_entry_get(unit, tnl_entry,
                            offset, iptun_mpls_entry_format);
                }

                bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                        tnl_entry, offset,
                        iptun_mpls_entry_format, bcmiMplsLabel,
                        &label_value);

                bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                        tnl_entry, offset,
                        iptun_mpls_entry_format, bcmiMplsTtl,
                        &entry_ttl);

                bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                        tnl_entry, offset,
                        iptun_mpls_entry_format, bcmiMplsExp,
                        &entry_exp);

                bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                        tnl_entry, offset,
                        iptun_mpls_entry_format, bcmiMplsPushAction,
                        &entry_push_action);
                /*
                 * This is the last entry in the chain, therefore
                 * entry push actions should be 1.
                 */
                if ((label_array[j].label == label_value) &&
                    (label_array[j].ttl == entry_ttl) &&
                    (label_array[j].exp == entry_exp) &&
                    (entry_push_action == 1)) {

                    *match_mpls_tunnel_index = (tnl_index *
                        _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit)) + i;
                    return BCM_E_NONE;
                }

            } else if (push_action == 0x1) {

                offset = i;

                if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                    bcmi_egress_tnl_label_entry_get(unit, tnl_entry,
                       offset, iptun_mpls_entry_format);
                }

                bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                        tnl_entry, offset,
                        iptun_mpls_entry_format, bcmiMplsLabel,
                        &label_value);

                bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                        tnl_entry, offset,
                        iptun_mpls_entry_format, bcmiMplsTtl,
                        &entry_ttl);

                bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                        tnl_entry, offset,
                        iptun_mpls_entry_format, bcmiMplsExp,
                        &entry_exp);

                offset = (i+j);

                sal_memset(&iptun_mpls_entry_format, 0,
                        sizeof(iptun_mpls_entry_format));

                bcmi_egress_tnl_label_entry_get(unit, tnl_entry,
                        offset, iptun_mpls_entry_format);

                bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                        tnl_entry, offset,
                        iptun_mpls_entry_format, bcmiMplsPushAction,
                        &entry_push_action);

                if ((label_array[0].label == label_value) &&
                        (label_array[0].ttl == entry_ttl) &&
                        (label_array[0].exp == entry_exp) &&
                        (push_action == entry_push_action)) {

                    *match_mpls_tunnel_index = (tnl_index *
                            _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit)) + i;
                    return BCM_E_NONE;
                }
            } else if (push_action == 0x0) {

                offset = i;
                bcmi_egress_tnl_label_entry_get(unit, tnl_entry,
                        offset, iptun_mpls_entry_format);

                bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                        tnl_entry, offset,
                        iptun_mpls_entry_format, bcmiMplsLabel,
                        &label_value);

                bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                        tnl_entry, offset,
                        iptun_mpls_entry_format, bcmiMplsTtl,
                        &entry_ttl);

                bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                        tnl_entry, offset,
                        iptun_mpls_entry_format, bcmiMplsExp,
                        &entry_exp);

                offset = (i+j);
                sal_memset(&iptun_mpls_entry_format, 0,
                        sizeof(iptun_mpls_entry_format));

                bcmi_egress_tnl_label_entry_get(unit, tnl_entry,
                        offset, iptun_mpls_entry_format);
                bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                        tnl_entry, offset,
                        iptun_mpls_entry_format, bcmiMplsPushAction,
                        &entry_push_action);

                /* Case of Dummy entry */
                if ((0 == label_value) &&
                    (0 == entry_ttl) &&
                    (0 == entry_exp) &&
                    (0 == entry_push_action)) {

                    *match_mpls_tunnel_index = (tnl_index *
                        _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit)) + i;
                    return BCM_E_NONE;
                }
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_check_availability
 * Purpose:
 *      Checks availability for incoming mpls tunnel
 *      entry into the existing free entries.
 * Parameters:
 *      unit         - (IN)device number
 *      fi           - (IN)Pointer to free indexes database.
 *      num_labels   - (IN)Number of labels that entry contains.
 * Returns:
 *      (INT)
 */
int
bcmi_egr_ip_tnl_mpls_check_availability(int unit,
                bcmi_egr_ip_tnl_free_indexes_t *fi,
                int num_labels)
{
    int idx,free_idx;
    int mpls_entry_index;

    if (num_labels == 0) {
        return -1;
    }
    /* MAX_FREE_ENTRY = */
    for (idx=num_labels-1; idx < MAX_ENTRY_INDEXES; idx++) {
        /* MAX FREE ENTRY COUNTING = 8 */
        if (fi->free_index_count[idx]) {
            /* entry array starts from 0 but count is from 1 */
            free_idx=fi->free_index_count[idx] - 1;
            for (; free_idx>=0; free_idx--) {
                /* There is some empty place here.
                 * lets use the same size ehtry or bigger.
                 * returning the first free entry in that block.
                 */
                if (fi->free_entry_indexes[idx][free_idx] == -1) {
                    /*
                     * count is set but no free index here.
                     * please reduce the count
                     */
                    fi->free_index_count[idx]--;
                    continue;
                }
                /*
                 * if we have reached here that means we have got the space
                 */
                mpls_entry_index = fi->free_entry_indexes[idx][free_idx];
                fi->free_entry_indexes[idx][free_idx] = -1;
                fi->free_index_count[idx]--;
                bcm_egr_ip_tnl_mpls_remark_free_indexes(unit,
                    (idx+1)-num_labels, mpls_entry_index+num_labels, fi);

                return mpls_entry_index;
            }
        }
    }
    return -1;
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_get_free_index
 * Purpose:
 *      fills the free entry index database with after
 *      reading all the tunnel entries.
 * Parameters:
 *      unit         - (IN)device number
 *      num_labels   - (IN)Number of labels that entry contains.
 *      tunnel_entry - (IN)Pointer to mpls tunnel s/w state.
 *      fi           - (IN/OUT)Pointer to free indexes database.
 * Returns:
 *      (INT)
 */
int
bcmi_egr_ip_tnl_mpls_get_free_index(int unit, int num_labels,
                       bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tunnel_entry,
                       bcmi_egr_ip_tnl_free_indexes_t *fi)
{
    int idx=0, mpls_off=0;
    int mpls_index;
    int start_index = -1;
    int number_of_free_entries = 0;
    int free_tunnel_index = -1;
    int num_ip_tnl;
    bcmi_egr_ip_tnl_mpls_label_entry_t *label_entry;
    soc_mem_t mem;

    if (num_labels == 0) {
        return -1;
    }

    mem = bcmi_egr_ip_tnl_mem_name_get(unit);

    num_ip_tnl = soc_mem_index_count(unit, mem);

    for (;idx < num_ip_tnl; idx++) {

        if (!_BCM_MPLS_IP_TNL_USED_GET(unit, idx)) {
            continue;
        }

        mpls_index = idx * _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);
        start_index = -1;
        mpls_off = 0;
        number_of_free_entries = 0;
        for(;mpls_off < _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit); mpls_off++) {

            if (_BCM_MPLS_TNL_USED_GET(unit, (mpls_index + mpls_off))) {

                label_entry = tunnel_entry[idx]->label_entry[mpls_off];

                if (label_entry->flags &
                    BCMI_EGR_IP_TUNNEL_MPLS_START_OF_ENTRY) {

                    mpls_off +=
                        label_entry->num_elements - 1;
                }
                if (!(bcmi_egr_ip_tnl_mpls_check_dup_free_index(unit,
                        fi,number_of_free_entries, start_index))) {

                    bcm_egr_ip_tnl_mpls_remark_free_indexes(unit,
                        number_of_free_entries, start_index, fi);
                }
                start_index = -1;
                number_of_free_entries = 0;

            } else {

                number_of_free_entries++;
                if (start_index == -1) {
                    start_index = mpls_index + mpls_off;
                }
                if (mpls_off == 7) {
                    /*
                     * this is the last tunnel index and therefore we
                     * should just update free entries
                     */
                    if (!(bcmi_egr_ip_tnl_mpls_check_dup_free_index(unit,
                            fi,number_of_free_entries, start_index))) {

                        bcm_egr_ip_tnl_mpls_remark_free_indexes(unit,
                            number_of_free_entries, start_index, fi);
                    }
                    start_index= -1;
                    number_of_free_entries = 0;

                }
            }
        }
        if (idx % num_labels == 0) {
           /* just checking after getting data of five labels */
            free_tunnel_index = bcmi_egr_ip_tnl_mpls_check_availability(unit,
                                    fi, num_labels);
        }
        if (free_tunnel_index != -1) {
            return free_tunnel_index;
        }
    }
    return -1;
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_create_local_free_indexes
 * Purpose:
 *      Create duplicate free indexes for local arithmatic.
 * Parameters:
 *      unit                         - (IN)device number
 *      free_entry_structures        - (IN)Pointer to free indexes database.
 *      local_free_indexes_structure - (IN)Pointer to free indexes database.
 * Returns:
 *      None.
 */
void
bcmi_egr_ip_tnl_mpls_create_local_free_indexes(int unit,
                    bcmi_egr_ip_tnl_free_indexes_t *free_entry_structures,
                    bcmi_egr_ip_tnl_free_indexes_t *local_free_indexes_structure)
{
    int i, j;

    sal_memset(local_free_indexes_structure, 0,
        sizeof(bcmi_egr_ip_tnl_free_indexes_t));
    for (i=0; i<MAX_ENTRY_INDEXES;i++) {
        for (j=0;j<MAX_FREE_ENTRY_INDEXES; j++) {
            local_free_indexes_structure->free_entry_indexes[i][j] =
                free_entry_structures->free_entry_indexes[i][j];
        }
        local_free_indexes_structure->free_index_count[i] =
            free_entry_structures->free_index_count[i];
    }
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_uncheck_free_indexes
 * Purpose:
 *      while looking for entries, we mark some entries as checkes
 *      already so that we do not try to use the same entry
 *      while looking for multiple entries..
 * Parameters:
 *      unit         - (IN)device number
 *      fi           - (IN)Pointer to free indexes database.
 *      tunnel_entry - (IN)Pointer to tunnel database
 * Returns:
 *      None.
 */
void
bcmi_egr_ip_tnl_mpls_uncheck_free_indexes(int unit,
                    bcmi_egr_ip_tnl_free_indexes_t *fi,
                    bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tunnel_entry)
{
    int i, j;
    int tnl_idx, tnl_mpls_idx;

    for (i=0; i<MAX_ENTRY_INDEXES;i++) {
        for (j=0;j<MAX_FREE_ENTRY_INDEXES; j++) {

            if (fi->free_entry_indexes[i][j] == -1) {
                continue;
                                    }
            tnl_idx      = fi->free_entry_indexes[i][j] /
                               _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);
            tnl_mpls_idx = fi->free_entry_indexes[i][j] %
                               _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);

            tunnel_entry[tnl_idx]->label_entry[tnl_mpls_idx]->flags &=
                (~BCMI_EGR_IP_TUNNEL_MPLS_ENTRY_CHECKED);
        }

    }
}

/*
 * Function:
 *      bcmi_egr_ip_tnl_mpls_adjust_entry
 * Purpose:
 *      Reshuffle the entries if we are not able to fit
 *      the incoming entry.
 * Parameters:
 *      unit         - (IN)device number
 *      num_labels   - (IN)Number of labels that entry contains.
 *      tunnel_entry - (IN_Pointer to mpls tunnel s/w state.
 *      fi           - (IN)Pointer to free indexes database.
 *      tnl_idx      - (OUT)Pointer to index of freed entry if possible.
 * Returns:
 *      bcm_error_t
 */
bcm_error_t
bcmi_egr_ip_tnl_mpls_adjust_entry(int unit,
                    int num_labels,
                    bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tunnel_entry,
                    bcmi_egr_ip_tnl_free_indexes_t *fi,
                    int *tnl_idx)
{
    int idx=0;
    bcmi_egr_ip_tnl_free_indexes_t local_fi; /* locan free indexes */
    int num_ip_tnl;
    int mpls_off;
    int no_of_elements;
    int temp_num_labels;
    int found = 0;
    int free_indexes_changed = 1;
    int f_idx, free_idx;
    int mpls_index;
    int left_free_entries;
    int new_start_index;
    int chk_tnl_idx, chk_mpls_idx;
    int rv;
    int src_mpls_index = 0;
    int ent_per_tunnel = _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);
    bcmi_egr_ip_tnl_mpls_label_entry_t *label_entry;
    soc_mem_t mem;
    int free_db_entries=0;

    mem = bcmi_egr_ip_tnl_mem_name_get(unit);

    num_ip_tnl = soc_mem_index_count(unit, mem);

    for (;idx < num_ip_tnl; idx++) {
        mpls_off = 0;
        temp_num_labels = num_labels;

        bcmi_egr_ip_tnl_mpls_uncheck_free_indexes(unit, fi, tunnel_entry);

        if (!_BCM_MPLS_IP_TNL_USED_GET(unit, idx)) {
            continue;
        }
        if (free_indexes_changed) {
            bcmi_egr_ip_tnl_mpls_create_local_free_indexes(unit, fi, &local_fi);
        }
        free_indexes_changed=0;
        mpls_index = idx * ent_per_tunnel;
        /* check for quad entry free */
        for(;mpls_off < ent_per_tunnel; mpls_off++) {
            found = 0;
            /*
             * if we have just started for entry, then do basic index checking.
             * if number of labels are 3 or 4 then only index 0 or index 4 are
             * valid indexes.
             * if number of labels are bigger than 4 then entry can start only
             * at index 0.
             * based on above rules, move the index to the next valid index.
             */
            if (temp_num_labels == num_labels) {
                src_mpls_index = mpls_index + mpls_off;
                if ((num_labels < 5) && (num_labels > 2)) {
                    if (mpls_off < 4 && mpls_off > 0) {
                        mpls_off = 4;
                    } else if (mpls_off > 4) {
                        mpls_off = 7;
                        continue;
                    }
                }
                if ((num_labels > 4) && (mpls_off > 0)) {
                    mpls_off = 7;
                    continue;
                }
            }

            /* If this is free index then continue until you get free space. */
            if (!_BCM_MPLS_TNL_USED_GET(unit, (mpls_index + mpls_off))) {
                temp_num_labels--;
                free_db_entries++;
                if (temp_num_labels) {
                    continue;
                }
            }

            /* If we are looking at same index as free index. then continue. */
            if (free_db_entries) {
                bcmi_mpls_egr_tunnel_delete_free_indexes(unit,
                    &local_fi, free_db_entries, (mpls_index + mpls_off - free_db_entries));
                free_indexes_changed = 1;
            }

            free_db_entries = 0;
            if(tunnel_entry[idx]->label_entry[mpls_off]->flags &
                BCMI_EGR_IP_TUNNEL_MPLS_START_OF_ENTRY) {
                /* this is the start of this entry,
                 * we should check if we have some free space for this entry
                 */
                no_of_elements =
                    tunnel_entry[idx]->label_entry[mpls_off]->num_elements;
                /* we also should check if there are some bigger entries
                 * in this tunnel entry,
                 * big entries can be adjusted to other big entries if exists.
                 * else we might have to move smaller entries to bigger entries
                 */
                if (no_of_elements >= num_labels) {
                    /*
                     * no need to look for bigger entry as this is already
                     * checked above.
                     * if we could move bigger entry then we could have moved
                     * incoming entry itself
                     * so please return back with update to indexes.
                     */
                    mpls_off += no_of_elements;
                    temp_num_labels = num_labels;
                    if (free_indexes_changed) {
                        bcmi_egr_ip_tnl_mpls_create_local_free_indexes(unit,
                            fi, &local_fi);
                    }

                     /*
                      * we are doing continue here because we may like to
                      * check further entries also.
                      */
                     bcmi_egr_ip_tnl_mpls_uncheck_free_indexes(unit,
                         fi, tunnel_entry);
                     continue;
                }

                for (f_idx=no_of_elements-1; f_idx < 8; f_idx++) {
                    if (!(local_fi.free_index_count[f_idx])) {
                        continue;
                    }
                    for (free_idx=0; free_idx<8; free_idx++) {
                        /* There is some empty place here.
                         * lets use the same size entry or bigger.
                         * returning the first free entry in that block.
                         */
                        if (local_fi.free_entry_indexes[f_idx][free_idx]
                            == -1) {

                            break;
                        }

                        chk_tnl_idx =
                          local_fi.free_entry_indexes[f_idx][free_idx] /
                              ent_per_tunnel;
                        chk_mpls_idx =
                          local_fi.free_entry_indexes[f_idx][free_idx] %
                              ent_per_tunnel;

                        label_entry =
                          tunnel_entry[chk_tnl_idx]->label_entry[chk_mpls_idx];

                        if (!(label_entry->flags &
                                    BCMI_EGR_IP_TUNNEL_MPLS_ENTRY_CHECKED)) {
                            /*
                             * we can use free_entry_indexes[idx][free_idx];
                             * mark this entry used so that next lookup should
                             * not use this.
                             */
                            label_entry->flags |=
                                BCMI_EGR_IP_TUNNEL_MPLS_ENTRY_CHECKED;

                            found = 1;
                            free_indexes_changed = 1;

                            temp_num_labels -= no_of_elements;
                            /* mpls_off is loop incrementing value. so reduce it. */
                            mpls_off += (no_of_elements - 1);
                            new_start_index =
                                local_fi.free_entry_indexes[f_idx][free_idx] +
                                no_of_elements;
                            bcmi_mpls_egr_tunnel_delete_free_indexes(unit,
                                &local_fi, no_of_elements,
                                fi->free_entry_indexes[f_idx][free_idx]);

                            left_free_entries = (f_idx+1)-no_of_elements;
                            bcm_egr_ip_tnl_mpls_remark_free_indexes(unit,
                                left_free_entries, new_start_index, &local_fi);
                            break;
                        }
                    }
                    if (found) {
                        break;
                    }
                }
                if (!found) {
                    temp_num_labels = num_labels;
                }
            }
            if (temp_num_labels <= 0) {

                /* if we are here that means we have got required space.
                 * get ready to move the entries.
                 */
                int mpls_tnl_off = src_mpls_index % ent_per_tunnel;

                bcmi_egr_ip_tnl_mpls_uncheck_free_indexes(unit, fi,
                    tunnel_entry);
                rv = bcmi_egr_ip_tnl_mpls_move_tunnel_entries(unit, fi,
                    tunnel_entry, idx, mpls_tnl_off, num_labels);

                if (rv == BCM_E_NONE) {
                    *tnl_idx = src_mpls_index;
                    /* we have got the index where we can fill this entry
                     * But we can have some free entries beneath it.
                     * so return those free entries to free indexes.
                     */
/*
                    bcm_egr_ip_tnl_mpls_remark_free_indexes(unit,
                        (ent_per_tunnel - num_labels - mpls_tnl_off),
                        src_mpls_index, &(fi_db[unit]));
*/
                    return rv;
                }
            }
        }
    }
    /*
     * if we are here then we have not got the space.
     * Currently return BCM_E_FULLL.
     */
    return BCM_E_FULL;
}

/*
 * Function:
 *      bcmi_xgs5_mpls_tunnel_initiator_set
 * Purpose:
 *      Set MPLS Tunnel initiator
 * Parameters:
 *      unit - Device Number
 *      intf - The egress L3 interface
 *      num_labels  - Number of labels in the array
 *      label_array - Array of MPLS label and header information
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_mpls_tunnel_initiator_set(int unit, bcm_if_t intf, int num_labels,
                                 bcm_mpls_egress_label_t *label_array)
{
    egr_l3_intf_entry_t if_entry;
    int rv, num_mpls_map, i, push_action=0, offset = 0;
    int found = 0, tnl_index = 0, mpls_index = 0;
    int hw_map_idx;
    bcm_tunnel_initiator_t tnl_init;
    uint32 tnl_flags = 0;
    int match_mpls_tunnel_index=-1;
    int ent_per_tnl = 0;
    int tnl_id = 0;
    int mpls_offset = 0;
    int mpls_tunnel_update = 0;
    bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tunnel_entry;
    soc_mem_t mem;
    uint32 tnl_entry[SOC_MAX_MEM_WORDS];
    uint32 iptun_mpls_entry_format[3] = {0};

    mem = bcmi_egr_ip_tnl_mem_name_get(unit);
    ent_per_tnl = _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);
    tunnel_entry = egr_mpls_tnl_sw_state[unit];

    if ((num_labels < 0) || (num_labels > ent_per_tnl)  ||
        (intf < 0) || (intf >= L3_INFO(unit)->l3_intf_table_size)) {
        return BCM_E_PARAM;
    }

    if ((num_labels > 2) &&
         !(soc_feature(unit, soc_feature_mpls_segment_routing)) &&
         !(soc_feature(unit, soc_feature_th3_style_simple_mpls))) {
        return BCM_E_UNAVAIL;
    }

    /* Derive action from labels.
     * based on number of labels, we can set multiple entries to add
     * desired number of labels into the tunnel.
     * Therefore push action is directly derived from num labels.
     */
    push_action = num_labels;

    /* Param checking */
    num_mpls_map = soc_mem_index_count(unit, EGR_MPLS_EXP_MAPPING_1m) / 64;
    for (i = 0; i < num_labels; i++) {
        if (label_array[i].qos_map_id == 0) { /* treat it as using default */
            hw_map_idx = 0;
        } else {
            BCM_IF_ERROR_RETURN(_egr_qos_id2hw_idx(unit,
                            label_array[i].qos_map_id,&hw_map_idx));
        }
        if ((label_array[i].label > 0xfffff) ||
            (hw_map_idx < 0) ||
            (hw_map_idx >= num_mpls_map) ||
            (label_array[i].exp > 7) || (label_array[i].pkt_pri > 7) ||
            (label_array[i].pkt_cfi > 1)) {
            return BCM_E_PARAM;
        }
    }
    if (!BCM_L3_INTF_USED_GET(unit, intf)) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 interface not created\n")));
        return BCM_E_NOT_FOUND;
    }

    /* L3 interface info */
    rv = READ_EGR_L3_INTFm(unit, MEM_BLOCK_ANY, intf, &if_entry);
    if (rv < 0) {
        return rv;
    }
   /* init the allocated entry */
   bcmi_egr_ip_tnl_mpls_table_memset(unit,tnl_entry);

   /* Case of Dummy Tunnel entry sharing by multiple L3_INTF */
   if (num_labels == 0) {
       /* Lookup if Tunnel Entry with Tunnel Label exists? */
       rv = bcmi_xgs5_mpls_egr_tunnel_lookup (unit, push_action, tunnel_entry,
                               NULL, &match_mpls_tunnel_index);
       if (rv < 0) {
          return rv;
       }

       if (match_mpls_tunnel_index != -1) {
           /* L3_Interface to point to mpls_tunnel */
           soc_EGR_L3_INTFm_field32_set(unit, &if_entry,
                              MPLS_TUNNEL_INDEXf, match_mpls_tunnel_index);
           /* Increment Ref count */
           bcmi_egr_ip_tnl_mpls_ref_count_adjust (unit,
                              match_mpls_tunnel_index, 1);
           rv = WRITE_EGR_L3_INTFm(unit, MEM_BLOCK_ANY, intf, &if_entry);

           if (rv == BCM_E_NONE) {
               tnl_id = match_mpls_tunnel_index/ent_per_tnl;
               mpls_offset = match_mpls_tunnel_index % ent_per_tnl;
               rv = bcmi_egr_ip_tnl_mpls_intf_list_add
                   (unit, tunnel_entry, intf, tnl_id, mpls_offset);

           }
           return rv;
       }
   }

   for (i = 0; i < num_labels; i++) {
       /* Check for Port_independent Label mapping */
       rv = bcm_tr_mpls_port_independent_range (unit, label_array[i].label,
                                                      BCM_GPORT_INVALID);
       /* Either Port-scope-label or Explicit-Null-label */
       if ((rv == BCM_E_CONFIG) || (label_array[i].label == 0)) {
           /* Tunnel-label is Port-based */
           /* Lookup if Tunnel Entry with Tunnel Label exists? */
           rv = bcmi_xgs5_mpls_egr_tunnel_lookup (unit, push_action,
               tunnel_entry, label_array, &match_mpls_tunnel_index);
           if (rv < 0) {
              return rv;
           }

           if (match_mpls_tunnel_index != -1) {
               /* L3_Interface to point to mpls_tunnel */
               soc_EGR_L3_INTFm_field32_set(unit, &if_entry,
                   MPLS_TUNNEL_INDEXf, match_mpls_tunnel_index);
               /* Increment Ref count */
               bcmi_egr_ip_tnl_mpls_ref_count_adjust (unit,
                   match_mpls_tunnel_index, 1);
               /* we also need to add this entry into sw state linked list */
               rv = WRITE_EGR_L3_INTFm(unit, MEM_BLOCK_ANY, intf, &if_entry);
               if (rv == BCM_E_NONE) {
                   tnl_id = match_mpls_tunnel_index/ent_per_tnl;
                   offset = match_mpls_tunnel_index % ent_per_tnl;
                   rv = bcmi_egr_ip_tnl_mpls_intf_list_add(unit, tunnel_entry,
                       intf, tnl_id, offset);
               }
               return rv;
           }
       }
    }
    mpls_index = soc_EGR_L3_INTFm_field32_get(unit, &if_entry,
                                              MPLS_TUNNEL_INDEXf);
    tnl_index = mpls_index / ent_per_tnl;

    if ((mpls_index != 0) && (_BCM_MPLS_TNL_USED_GET(unit, mpls_index))) {
       /* Obtain referenced Tunnel entry */
       /* try using existing entry in tunnel */
        rv = bcmi_egr_ip_tnl_mpls_table_read(unit,tnl_index,tnl_entry);

        if (rv < 0) {
            return rv;
        }

        rv = bcmi_egr_ip_tnl_mpls_entry_type_check(unit, tnl_entry);

        if (rv < 0) {
            return rv;
        }

        /* we need to check here if at least number of labels is same.*/
        offset = mpls_index % ent_per_tnl;
        if (tunnel_entry[tnl_index]->label_entry[offset]->flags &
            BCMI_EGR_IP_TUNNEL_MPLS_START_OF_ENTRY) {
            if (tunnel_entry[tnl_index]->label_entry[offset]->num_elements !=
                num_labels) {
                /* Changing push action/ num labels is not permitted,
                 * need to clear/set
                 */
                return BCM_E_PARAM;
            }
        } else {
            return BCM_E_PARAM;
        }
        mpls_tunnel_update = 1;

    } else {
        /* allocate an unused EGR_IP_TUNNEL_MPLS_ENTRY */
        /*
         * First try finding an allocated tunnel entry with
         * unused MPLS entries.
         */
        mpls_index =  bcmi_egr_ip_tnl_mpls_check_availability(unit,
            &(fi_db[unit]), num_labels);

        if (mpls_index == -1) {
            /*
             * we have not got corresponding free indexes.
             * lets try to create some more free indexes
             * and then try again.
             */
            mpls_index = bcmi_egr_ip_tnl_mpls_get_free_index(unit, num_labels,
                tunnel_entry, &(fi_db[unit]));
        }
        if (mpls_index != -1) {
               found = 1;
            tnl_index = mpls_index /
                (_BCM_MPLS_EGR_L3_INTF_MPLS_INDEX_OFFSET_MASK(unit) + 1);

            rv = bcmi_egr_ip_tnl_mpls_table_read(unit,tnl_index,tnl_entry);
            if (rv < 0) {
                return rv;
            }

        } else {
            /*
             * Alloc an egr_ip_tunnel entry. By calling bcm_xgs3_tnl_init_add
             * with _BCM_L3_SHR_WRITE_DISABLE flag, a tunnel index is
             * allocated but nothing is written to hardware. The "tnl_init"
             * information is not used, set to all zero.
             */
            sal_memset(&tnl_init, 0, sizeof(bcm_tunnel_initiator_t));
            tnl_flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE |
                _BCM_L3_SHR_SKIP_INDEX_ZERO;
            tnl_init.type =  bcmTunnelTypeMpls;
            rv = bcm_xgs3_tnl_init_add(unit, tnl_flags, &tnl_init, &tnl_index);
            if (rv == BCM_E_FULL) {

                /* There is no entry left unused.
                 * now see if we can move some entries and make space
                 */

                rv = bcmi_egr_ip_tnl_mpls_adjust_entry(unit, num_labels,
                    tunnel_entry, &(fi_db[unit]), &mpls_index);
                if (rv < 0) {
                    return rv;
                }

            } else {
                found = 1;
                /* we have got full entry here. start at 0 index.
                 * we will get double entry from l3_tbl_add as it
                 *  works on common resorce of egr_ip_tunnel.
                 *  But egr_ip_tunnel_mpls uses double entries.
                 */
                tnl_index = (tnl_index + 1)/2;
                mpls_index =  tnl_index *
                    (_BCM_MPLS_EGR_L3_INTF_MPLS_INDEX_OFFSET_MASK(unit) + 1);
                /* we are using two tunnel entries at same time.*/
                _BCM_MPLS_IP_TNL_USED_SET(unit, tnl_index);
                _BCM_MPLS_TNL_USED_SET(unit, mpls_index);

                bcmi_egr_ip_tnl_mpls_table_memset(unit,tnl_entry);
                if (num_labels < ent_per_tnl) {
                    bcm_egr_ip_tnl_mpls_remark_free_indexes(unit,
                        (ent_per_tnl - ((num_labels) ? num_labels : 1)),
                        mpls_index+((num_labels) ? num_labels : 1),
                        &(fi_db[unit]));
                }
            }
        }
    }

    offset = mpls_index & _BCM_MPLS_EGR_L3_INTF_MPLS_INDEX_OFFSET_MASK(unit);
    tnl_id = mpls_index /
        (_BCM_MPLS_EGR_L3_INTF_MPLS_INDEX_OFFSET_MASK(unit) + 1);

    tunnel_entry[tnl_id]->label_entry[offset]->flags |=
                BCMI_EGR_IP_TUNNEL_MPLS_START_OF_ENTRY;
    tunnel_entry[tnl_id]->label_entry[offset]->num_elements =
        ((num_labels) ? num_labels : 1);

    if (!num_labels) {
        tunnel_entry[tnl_id]->label_entry[offset]->flags |=
                BCMI_EGR_IP_TUNNEL_MPLS_ENTRY_ZERO;
    }

    rv = bcmi_egr_ip_tnl_mpls_table_read(unit, tnl_id,tnl_entry);
    if (rv < 0) {
        return rv;
    }

    for (i = 0; i < num_labels; i++) {
        if (label_array[i].qos_map_id == 0) { /* treat it as using default */
            hw_map_idx = 0;
        } else {
            rv = _egr_qos_id2hw_idx(unit,
                            label_array[i].qos_map_id,&hw_map_idx);
            if (rv < 0) {
                goto cleanup;
            }
        }

        bcmi_egr_ip_tnl_mpls_entry_format_set(unit, bcmiMplsPushAction,
                tnl_entry, offset,
                iptun_mpls_entry_format,
                ((i == (num_labels - 1)) ? 1 : 2));
        if (num_labels > 0) {
            bcmi_egr_ip_tnl_mpls_entry_format_set(unit, bcmiMplsLabel,
                    tnl_entry, offset,
                    iptun_mpls_entry_format, label_array[i].label);
        }

#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            bcmi_egr_ip_tnl_mpls_entry_format_set(unit, bcmiSpecialLabelPushType,
                    tnl_entry, offset,
                    iptun_mpls_entry_format, label_array[i].spl_label_push_type);
        }
#endif

        if ((label_array[i].flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) ||
            (label_array[i].flags & BCM_MPLS_EGRESS_LABEL_PRI_SET)) {

            /* Use the specified EXP, PRI and CFI */
            if ((label_array[i].flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) ||
                (label_array[i].flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY) ||
                (label_array[i].flags & BCM_MPLS_EGRESS_LABEL_PRI_REMARK)) {
                rv = BCM_E_PARAM;
                goto cleanup;
            }

            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiMplsExpSelect,
                    tnl_entry, offset,
                    iptun_mpls_entry_format, 0);

            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiMplsExp,
                    tnl_entry, offset,
                    iptun_mpls_entry_format,
                    label_array[i].exp);

            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiNewPri,
                    tnl_entry, offset,
                    iptun_mpls_entry_format,
                    label_array[i].pkt_pri);

            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiNewCfi,
                    tnl_entry, offset,
                    iptun_mpls_entry_format,
                    label_array[i].pkt_cfi);
        } else if (label_array[i].flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) {
            if (label_array[i].flags & BCM_MPLS_EGRESS_LABEL_PRI_SET) {
                rv = BCM_E_PARAM;
                goto cleanup;
            }
            /* Use EXP-map for EXP, PRI and CFI */
            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiMplsExpSelect,
                    tnl_entry, offset,
                    iptun_mpls_entry_format, 0x1);

            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiMplsExpMappingPtr,
                    tnl_entry, offset,
                    iptun_mpls_entry_format,
                    hw_map_idx);

            if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                if(i == 1) {
                    /*EGR_MPLS_EXP_MAPPING_3 for Tunnel label 2*/
                    bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                            bcmiExpMappingTableSelect,
                            tnl_entry, offset,
                            iptun_mpls_entry_format, 0x2);
                }

                if(i == 0) {
                    /*EGR_MPLS_EXP_MAPPING_2 for Tunnel label 1*/
                    bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                            bcmiExpMappingTableSelect,
                            tnl_entry, offset,
                            iptun_mpls_entry_format, 0x1);
                }
            }

        } else { /* BCM_MPLS_EGRESS_LABEL_EXP_COPY */
            /* Use EXP from inner label. If there is no inner label,
             * use the specified EXP value. Use EXP-map for PRI/CFI.
             */
            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiMplsExpSelect,
                    tnl_entry, offset,
                    iptun_mpls_entry_format, 0x2); /* USE_INNER */

            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiMplsExp,
                    tnl_entry, offset,
                    iptun_mpls_entry_format,
                    label_array[i].exp);

            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiMplsExpMappingPtr,
                    tnl_entry, offset,
                    iptun_mpls_entry_format,
                    hw_map_idx);

        }

        if (label_array[i].flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) {
            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiMplsTtl,
                    tnl_entry, offset,
                    iptun_mpls_entry_format,
                    label_array[i].ttl);
        } else {
            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiMplsTtl,
                    tnl_entry, offset,
                    iptun_mpls_entry_format,
                    0);
        }

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_mpls_ecn)) {
            if (label_array[i].flags & BCM_MPLS_EGRESS_LABEL_ECN_TO_EXP_MAP) {
                int ecn_map_index;
                int ecn_map_type;
                int ecn_map_num;
                int ecn_map_hw_idx;
                soc_mem_t mem;

                ecn_map_type = label_array[i].ecn_map_id & 
                                _BCM_XGS5_MPLS_ECN_MAP_TYPE_MASK;
                ecn_map_index = label_array[i].ecn_map_id & 
                                _BCM_XGS5_MPLS_ECN_MAP_NUM_MASK;

#if defined(BCM_TRIDENT3_SUPPORT)
                if(soc_feature(unit, soc_feature_td3_style_mpls)) {
                    mem = EGR_PKT_ECN_TO_EXP_MAPPING_1m;
                } else
#endif
                {   
                    mem = EGR_IP_ECN_TO_EXP_MAPPING_TABLEm;
                }

                ecn_map_num = soc_mem_index_count(unit, mem) /
                    _BCM_ECN_MAX_ENTRIES_PER_MAP;
                if (ecn_map_type != _BCM_XGS5_MPLS_ECN_MAP_TYPE_ECN2EXP) {
                    rv = BCM_E_PARAM;
                    goto cleanup;
                }
                if (ecn_map_index >= ecn_map_num) {
                    rv = BCM_E_PARAM;
                    goto cleanup;
                }
                if (!bcmi_xgs5_ecn_map_used_get(unit, ecn_map_index, 
                                                _bcmEcnmapTypeIpEcn2Exp)) {
                    rv = BCM_E_PARAM;
                    goto cleanup; 
                }
                rv = bcmi_ecn_map_id2hw_idx(unit, label_array[i].ecn_map_id, 
                                            &ecn_map_hw_idx);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
#if defined(BCM_TRIDENT3_SUPPORT)
                if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                    bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                            bcmiPktEcnToExpMappingPtr,
                            tnl_entry, offset,
                            iptun_mpls_entry_format,
                            ecn_map_hw_idx);

                    /*ECN_MAPPING_ENABLE Remap EXP to encode ECN.*/
                    bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                            bcmiEcnMappingEnable,
                            tnl_entry, offset,
                            iptun_mpls_entry_format, 1);

                    bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                            bcmiEcnMappingTableSelect,
                            tnl_entry, offset,
                            iptun_mpls_entry_format, 2);

                    if (label_array[i].flags & 
                            BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST) {
                        bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                                bcmiPktEcnToExpPriority,
                                tnl_entry, offset,
                                iptun_mpls_entry_format, 1);
                    } else {
                        bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                                bcmiPktEcnToExpPriority,
                                tnl_entry, offset,
                                iptun_mpls_entry_format, 0);
                    }
                } else 
#endif
                {
                /* Coverity issue ignored:
                 * It cannot overrun size by 8 since the macro
                 * _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX is 8 or 4
                 * hence, the scenario does not arise */
                /* coverity[overrun-local : FALSE] */
                soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                    _tnl_ecn_ptr_f[offset], 
                                    ecn_map_hw_idx); 
                if (label_array[i].flags & 
                    BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST) {
                    /* coverity[overrun-local : FALSE] */
                    soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                        _tnl_ecn_ptr_pri_f[offset], 1);
                } else {
                    /* coverity[overrun-local : FALSE] */
                    soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                        _tnl_ecn_ptr_pri_f[offset], 0);

                }
            }
            }
            if (label_array[i].flags & BCM_MPLS_EGRESS_LABEL_INT_CN_TO_EXP_MAP) {
                int ecn_map_index;
                int ecn_map_type;
                int ecn_map_num;
                int ecn_map_hw_idx;
                soc_mem_t mem;
                ecn_map_type = label_array[i].int_cn_map_id & 
                                _BCM_XGS5_MPLS_ECN_MAP_TYPE_MASK;
                ecn_map_index = label_array[i].int_cn_map_id & 
                                _BCM_XGS5_MPLS_ECN_MAP_NUM_MASK;

#if defined(BCM_TRIDENT3_SUPPORT)
                if(soc_feature(unit, soc_feature_td3_style_mpls)) {
                    mem = EGR_PKT_ECN_TO_EXP_MAPPING_1m;
                } else
#endif
                {
                    mem = EGR_IP_ECN_TO_EXP_MAPPING_TABLEm;
                }

                ecn_map_num = soc_mem_index_count(unit, mem) /
                    _BCM_ECN_MAX_ENTRIES_PER_MAP;

                if (ecn_map_type != _BCM_XGS5_MPLS_ECN_MAP_TYPE_INTCN2EXP) {
                    rv = BCM_E_PARAM;
                    goto cleanup;

                }
                if (ecn_map_index >= ecn_map_num) {
                    rv = BCM_E_PARAM;
                    goto cleanup;
                }
                if (!bcmi_xgs5_ecn_map_used_get(unit, ecn_map_index, 
                                                _bcmEcnmapTypeIntcn2Exp)) {
                    rv = BCM_E_PARAM;
                    goto cleanup;
                }
                rv = bcmi_ecn_map_id2hw_idx(unit, label_array[i].int_cn_map_id, 
                                            &ecn_map_hw_idx);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
                /* Coverity issue ignored:
                 * It cannot overrun size by 8 since the macro
                 * _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX is 8 or 4
                 * hence, the scenario does not arise */
                /* coverity[overrun-local : FALSE] */
                soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                    _tnl_int_cn_ptr_f[offset], 
                                    ecn_map_hw_idx); 
                if (label_array[i].flags & 
                    BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST) {
                    /* coverity[overrun-local : FALSE] */
                    soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                        _tnl_int_cn_ptr_pri_f[offset], 1);
                } else {
                    /* coverity[overrun-local : FALSE] */
                    soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                        _tnl_int_cn_ptr_pri_f[offset], 0);

                }
            }            
        }
#endif  
 
         /*After filling all fields in EGR_IP_TUNNEL_MPLS_ENTRY_FORMAT
          *set MPLS_ENTRY_Xf*/
         /* Coverity issue ignored:
          * It cannot overrun size by 8 since the macro
          * _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX is 8 or 4
          * hence, the scenario does not arise */
        /* coverity[overrun-local : FALSE] */
        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            bcmi_egress_tnl_label_entry_set(unit, tnl_entry,
                    offset, iptun_mpls_entry_format);
        }

        offset++;
        _BCM_MPLS_TNL_USED_SET(unit, mpls_index + i);
    }
    bcmi_egr_ip_tnl_mpls_ref_count_adjust (unit,
                                mpls_index, 1);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
        soc_mem_field32_set(unit, mem,
                &tnl_entry, DATA_TYPEf, 0x14);
        soc_mem_field32_set(unit, mem,
                &tnl_entry, DATA_TYPE_COPYf, 0x14);
    } else
#endif
    {
        soc_mem_field32_set(unit, mem, &tnl_entry,
                ENTRY_TYPEf, 0x3);
        soc_mem_field32_set(unit, mem, &tnl_entry,
                ENTRY_TYPE_COPYf, 0x3);
    }

    /* Commit the values to HW */
    rv = bcmi_egr_ip_tnl_mpls_table_write(unit, tnl_id, tnl_entry);
    if (rv < 0) {
        goto cleanup;
    }

    /* Update the EGR_L3_INTF to point to the MPLS tunnel entry */
    rv = soc_mem_field32_modify(unit, EGR_L3_INTFm, intf,
                                MPLS_TUNNEL_INDEXf, mpls_index);
    if (rv < 0) {
        goto cleanup;
    }


    offset = mpls_index & _BCM_MPLS_EGR_L3_INTF_MPLS_INDEX_OFFSET_MASK(unit);
    tnl_id = mpls_index / (_BCM_MPLS_EGR_L3_INTF_MPLS_INDEX_OFFSET_MASK(unit) + 1);

    rv = bcmi_egr_ip_tnl_mpls_intf_list_add(unit, tunnel_entry, intf, tnl_id, offset);
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
    if (mpls_tunnel_update) {
        return rv;
    }
    if (!found) {
        /* Free the tunnel entry */
        (void) bcm_xgs3_tnl_init_del(unit, tnl_flags, tnl_index * 2);

        /* Clear "in-use" for the IP tunnel entry */
        _BCM_MPLS_IP_TNL_USED_CLR(unit, tnl_index);
    }
    /* Clear "in-use" for the MPLS tunnel entry(s) */
    for (i = 0; i < num_labels; i++) {
        _BCM_MPLS_TNL_USED_CLR(unit, mpls_index + i);
    }
    return rv;
}

/*
 * Function:
 *      bcmi_mpls_egr_tunnel_add_free_indexes
 * Purpose:
 *      Adds free index entry into free indexes if the entry is freed.
 * Parameters:
 *      unit           - device number
 *      fi             - Pointer to free indexes database.
 *      num_labels     - Number of labels that entry contains.
 *      mpls_entry_idx - index to mpls entry index that is used up.
 * Returns:
 *      None.
 */

void
bcmi_mpls_egr_tunnel_add_free_indexes(int unit,
                    bcmi_egr_ip_tnl_free_indexes_t *fi,
                    int num_labels, int mpls_entry_idx)
{
    int tnl_index, mpls_index, offset;
    int new_mpls_entry_idx, new_num_elements;
    int i=0, j=0;
    int ent_per_tnl = _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);

    tnl_index = mpls_entry_idx/ent_per_tnl;
    mpls_index = tnl_index * ent_per_tnl;

    new_mpls_entry_idx = mpls_entry_idx;
    new_num_elements     = num_labels;
   /* check from the start of an entry */
    for(; i<ent_per_tnl; i++) {
        if (_BCM_MPLS_TNL_USED_GET(unit, mpls_index + i)) {
            continue;
        }
        while ((mpls_index + i + j) < mpls_entry_idx) {

            if (_BCM_MPLS_TNL_USED_GET(unit, mpls_index + i + j)) {
                i += j;
                j = 0;
                break;
            }
            j++;
        }
    }

    /* delete this free entry from the free entry db */
    if (j > 0) {
        new_mpls_entry_idx = mpls_entry_idx - j;
        new_num_elements     = num_labels+j;
        bcmi_mpls_egr_tunnel_delete_free_indexes(unit, fi, j,
            new_mpls_entry_idx);
    }

    offset = mpls_entry_idx % ent_per_tnl;
    offset += num_labels;

    j = 0;
    /* now check towards end of the entry */
    for(i=offset; i<ent_per_tnl; i++) {
        if (_BCM_MPLS_TNL_USED_GET(unit, mpls_index + i)) {
            break;
        }

        j++;
    }
    if (j > 0) {
        /* delete this free entries from free entry db */
        bcmi_mpls_egr_tunnel_delete_free_indexes(unit, fi, j,
            mpls_index + offset);
        new_num_elements += j;
    }
    /* Now we can remark this new entry into the database. */
    bcm_egr_ip_tnl_mpls_remark_free_indexes(unit, new_num_elements,
        new_mpls_entry_idx, fi);
}

/*
 * Function:
 *      _bcm_tr_mpls_tunnel_initiator_clear
 * Purpose:
 *      Clear MPLS Tunnel initiator
 * Parameters:
 *      unit        - (IN)Device Number
 *      intf        - (IN)Egress L3 interface
 * Returns:
 *      BCM_E_XXX
 */
bcm_error_t
bcmi_xgs5_mpls_tunnel_initiator_clear(int unit, int intf_id)
{
    egr_l3_intf_entry_t if_entry;
    int rv, mpls_offset, mpls_entry_used, i;
    int tnl_idx = 0, mpls_index = 0;
    uint32 tnl_flags;
    _bcm_tr_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);
    bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tunnel_entry;
    bcmi_egr_ip_tnl_free_indexes_t *fi = &(fi_db[unit]);
    int mpls_tnl_start_index = 0;
    int no_of_elements;
    uint32 tnl_entry[SOC_MAX_MEM_WORDS];
    uint32 data_buf[3] = {0};

    tunnel_entry = egr_mpls_tnl_sw_state[unit];

    if (tunnel_entry == NULL) {
        return BCM_E_INIT;
    }

    bcmi_egr_ip_tnl_mpls_table_memset(unit, tnl_entry);

    /* L3 interface info */
    rv = READ_EGR_L3_INTFm(unit, MEM_BLOCK_ANY, intf_id, &if_entry);
    if (rv < 0) {
        return rv;
    }
    mpls_index = soc_EGR_L3_INTFm_field32_get(unit, &if_entry,
                                              MPLS_TUNNEL_INDEXf);
    tnl_idx = mpls_index / _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);
    mpls_tnl_start_index = tnl_idx * _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);

    if (!_BCM_MPLS_TNL_USED_GET(unit, mpls_index)) {
        return BCM_E_NOT_FOUND;
    }

    rv = bcmi_egr_ip_tnl_mpls_table_read(unit, tnl_idx, tnl_entry);
    if (rv < 0) {
        return rv;
    }

    rv = bcmi_egr_ip_tnl_mpls_entry_type_check(unit, tnl_entry);
    if (rv < 0) {
        return BCM_E_NOT_FOUND;
    }

    mpls_offset = mpls_index & _BCM_MPLS_EGR_L3_INTF_MPLS_INDEX_OFFSET_MASK(unit);


    if (tunnel_entry[tnl_idx]->label_entry[mpls_offset]->flags
         & BCMI_EGR_IP_TUNNEL_MPLS_START_OF_ENTRY) {

        no_of_elements =
            tunnel_entry[tnl_idx]->label_entry[mpls_offset]->num_elements;

        bcmi_egr_ip_tnl_mpls_ref_count_adjust (unit, mpls_index, -1);
    } else {
        return BCM_E_NOT_FOUND;
    }

    rv = bcmi_egr_ip_tnl_mpls_intf_list_delete(unit, tunnel_entry, intf_id,
        tnl_idx, mpls_offset);


    if (rv < 0) {
        return rv;
    }

    /* Update the EGR_L3_INTF to no longer point to the MPLS tunnel entry */
    rv = soc_mem_field32_modify(unit, EGR_L3_INTFm, intf_id,
                                MPLS_TUNNEL_INDEXf, 0);
    if (rv < 0) {
        return rv;
    }

    if ( mpls_info->egr_tunnel_ref_count[mpls_index] != 0x0 ) {
         return BCM_E_NONE;
    }

    bcm_egr_ip_tnl_mpls_sw_entry_reset(unit, tunnel_entry, tnl_idx,
        mpls_offset, 0);

    /* Clear the MPLS tunnel entry(s) */
    rv = bcmi_egr_ip_tnl_mpls_table_read(unit,tnl_idx,tnl_entry);
    if (rv < 0) {
        return rv;
    }

    for (i=0; i < no_of_elements; i++) {

        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            bcmi_egress_tnl_label_entry_set(unit, tnl_entry,
                    mpls_offset + i, data_buf);
        } else {
            /*data_buf is dontcare in non TD3*/
            /*label*/
            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiMplsLabel,
                    tnl_entry, mpls_offset + i,
                    data_buf,
                    0);
            /*Pushaction*/
            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiMplsPushAction,
                    tnl_entry, mpls_offset + i,
                    data_buf,
                    0);
            /*Expselect*/
            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiMplsExpSelect,
                    tnl_entry, mpls_offset + i,
                    data_buf,
                    0);
            /*Expptr*/
            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiMplsExpMappingPtr,
                    tnl_entry, mpls_offset + i,
                    data_buf,
                    0);
            /*Exp*/
            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiMplsExp,
                    tnl_entry, mpls_offset + i,
                    data_buf,
                    0);
            /*TTl*/
            bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                    bcmiMplsTtl,
                    tnl_entry, mpls_offset + i,
                    data_buf,
                    0);
        }
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_mpls_ecn)) {
            if(soc_feature(unit, soc_feature_td3_style_mpls)) {
                bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                        bcmiPktEcnToExpMappingPtr,
                        tnl_entry, mpls_offset + i,
                        data_buf,
                        0);

                bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                        bcmiPktEcnToExpPriority,
                        tnl_entry, mpls_offset + i,
                        data_buf,
                        0);

                bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                        bcmiEcnMappingEnable,
                        tnl_entry, mpls_offset + i,
                        data_buf,
                        0);

                bcmi_egr_ip_tnl_mpls_entry_format_set(unit,
                        bcmiEcnMappingTableSelect,
                        tnl_entry, mpls_offset + i,
                        data_buf,
                        0);
            } else {
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                                _tnl_ecn_ptr_f[mpls_offset + i], 0);
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                                _tnl_ecn_ptr_pri_f[mpls_offset + i], 0);
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                                _tnl_int_cn_ptr_f[mpls_offset + i], 0);
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                                _tnl_int_cn_ptr_pri_f[mpls_offset + i], 0);            
        }
        }
#endif
        _BCM_MPLS_TNL_USED_CLR(unit, mpls_index + i);
    }

    /* See if we can free the IP tunnel base entry */
    _BCM_MPLS_TNL_USED_RANGE_GET(unit, mpls_tnl_start_index,
            _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit), mpls_entry_used);

    if (!mpls_entry_used) {
        /* None of the 4 or 8 entries are used, free base entry */
        tnl_flags = 0;
        (void) bcm_xgs3_tnl_init_del(unit, tnl_flags, tnl_idx * 2);

        /* Clear "in-use" for the IP tunnel entry */
        _BCM_MPLS_IP_TNL_USED_CLR(unit, tnl_idx);

        /* Clear HW Entry-Type */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLS_DOUBLE_WIDEm,
                    tnl_entry, DATA_TYPE_COPYf, 0x0);
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLS_DOUBLE_WIDEm,
                    tnl_entry, DATA_TYPEf, 0x0);
        } else
#endif
        {
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, tnl_entry,
                    ENTRY_TYPEf, 0);
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, tnl_entry,
                    ENTRY_TYPE_COPYf, 0);
        }
        bcmi_egr_ip_tnl_mpls_free_indexes_clear(unit, tnl_idx, fi);
    } else {
        bcmi_mpls_egr_tunnel_add_free_indexes(unit, fi, no_of_elements,
             mpls_index);
    }

    rv = bcmi_egr_ip_tnl_mpls_table_write(unit, tnl_idx, tnl_entry);
    if (rv < 0) {
        return rv;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_mpls_tunnel_initiator_clear_all
 * Purpose:
 *      Clear all MPLS Tunnel Initiators
 * Parameters:
 *      unit - (IN)Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_mpls_tunnel_initiator_clear_all(int unit)
{
    egr_l3_intf_entry_t if_entry;
    int mpls_index;
    int rv=BCM_E_NONE;
    int num_ip_tnl_mpls;
    int i,j;
    soc_mem_t mem;

    for (i = 0; i < L3_INFO(unit)->l3_intf_table_size; i++) {
        if (!BCM_L3_INTF_USED_GET(unit, i)) {
            continue;
        }

        rv = READ_EGR_L3_INTFm(unit, MEM_BLOCK_ANY, i, &if_entry);
        if (rv < 0) {
            return rv;
        }
        mpls_index = soc_EGR_L3_INTFm_field32_get(unit, &if_entry,
                                                  MPLS_TUNNEL_INDEXf);
        if (!_BCM_MPLS_TNL_USED_GET(unit, mpls_index)) {
            continue;
        }
        rv = bcmi_xgs5_mpls_tunnel_initiator_clear(unit, i);
        if (rv < 0) {
            return rv;
        }
    }

    /* cleanup the s/w state for tunnel table. */
    mem = bcmi_egr_ip_tnl_mem_name_get(unit);

    num_ip_tnl_mpls = soc_mem_index_count(unit, mem);
    for (i=0; i<num_ip_tnl_mpls; i++) {
        for (j=0; j<_BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit); j++) {
            bcmi_egr_ip_tnl_mpls_intf_list_delete_all(unit,
                egr_mpls_tnl_sw_state[unit], i, j);

            egr_mpls_tnl_sw_state[unit][i]->label_entry[j]->intf_list = NULL;
            egr_mpls_tnl_sw_state[unit][i]->label_entry[j]->flags = 0;
            egr_mpls_tnl_sw_state[unit][i]->label_entry[j]->num_elements = 0;

        }
    }

    /* cleanup the free indexes */
    bcmi_egr_ip_tnl_mpls_free_indexes_init(unit, &(fi_db[unit]));

    return rv;
}


/*
 * Function:
 *      bcmi_xgs5_mpls_tunnel_initiator_reinit
 * Purpose:
 *      set s/w state of MPLS tunnel during warmboot.
 * Parameters:
 *      unit        - (IN) Device Number
 * Returns:
 *      BCM_E_XXX
 */
bcm_error_t
bcmi_xgs5_mpls_tunnel_initiator_reinit(int unit)
{
    int tnl_idx=0, mpls_off = 0;
    int start_of_entry = 0;
    bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tunnel_entry;
    /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int             buf_size, chunksize, chnk_end;
    uint32          *tbl_chnk;
    uint32          *el3inf_entry;
    int             rv = BCM_E_NONE;
    soc_mem_t mem;
    int last_push_action = 0;
    int num_labels = 0;
    int push_action = 0;
    int mpls_offset = 0;
    int mpls_index = 0;
    int index_min, index_max;
    uint8 *tunnel_entry_buf = NULL;
#if defined(BCM_TRIDENT3_SUPPORT)
    egr_ip_tunnel_mpls_double_wide_entry_t *tnl_entry_double = NULL;
    uint32 iptun_mpls_entry_format[3] = {0};
#endif
    egr_ip_tunnel_mpls_entry_t *tnl_entry = NULL;

    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
        mem = EGR_IP_TUNNEL_MPLS_DOUBLE_WIDEm;
    } else {
        mem = EGR_IP_TUNNEL_MPLSm;
    }

    tunnel_entry = egr_mpls_tnl_sw_state[unit];

    tunnel_entry_buf = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, mem), "MPLS TUNNEL ENTRY buffer");
    if (NULL == tunnel_entry_buf) {
        return BCM_E_MEMORY;
    }

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
            index_min, index_max, tunnel_entry_buf);
    if (SOC_FAILURE(rv)) {
        soc_cm_sfree(unit, tunnel_entry_buf);
        return rv;
    }

    for (tnl_idx = index_min; tnl_idx <= index_max; tnl_idx++) {

        if (!_BCM_MPLS_IP_TNL_USED_GET(unit, tnl_idx)) {
            continue;
        }

        last_push_action = 0;
        start_of_entry = 0;
        num_labels = 0;

#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            tnl_entry_double = soc_mem_table_idx_to_pointer
                (unit, EGR_IP_TUNNEL_MPLS_DOUBLE_WIDEm,
                 egr_ip_tunnel_mpls_double_wide_entry_t *,
                 tunnel_entry_buf, tnl_idx);
        } else
#endif
        {
            tnl_entry = soc_mem_table_idx_to_pointer
                (unit, EGR_IP_TUNNEL_MPLSm, egr_ip_tunnel_mpls_entry_t *, tunnel_entry_buf, tnl_idx);
        }

        /* Check for valid entry */

        for(mpls_off = 0; mpls_off<_BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit); mpls_off++) {
            mpls_index = tnl_idx * _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit) + mpls_off;

            if (!_BCM_MPLS_TNL_USED_GET(unit, mpls_index)) {
                start_of_entry = 0;
                last_push_action = 0;
                num_labels = 0;
                continue;
            }

            if (last_push_action == 2) {
                start_of_entry = 0;
            } else {
                start_of_entry = 1;
            }

            if (start_of_entry) {
                tunnel_entry[tnl_idx]->label_entry[mpls_off]->flags |=
                    BCMI_EGR_IP_TUNNEL_MPLS_START_OF_ENTRY;
            }

#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                soc_mem_field_get(unit, mem,
                        (uint32 *)tnl_entry_double,
                        _tnl_label_entry_f[mpls_off],
                        iptun_mpls_entry_format);

                bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                        tnl_entry_double, mpls_off,
                        iptun_mpls_entry_format, bcmiMplsPushAction,
                        (uint32 *)&push_action);
            } else
#endif
            {
                push_action = soc_mem_field32_get(unit, mem,
                        tnl_entry, _tnl_push_action_f[mpls_off]);
            }

            if (push_action == 2) {
                num_labels++;
            } else {
                if (last_push_action == 2) {
                    mpls_offset = mpls_off-num_labels;
                } else {
                    mpls_offset = mpls_off;
                }
                num_labels++;
                tunnel_entry[tnl_idx]->label_entry[mpls_offset]->num_elements =
                   num_labels;
                start_of_entry = 0;
                num_labels = 0;
            }
            last_push_action = push_action;
        }
    }

    soc_cm_sfree(unit, tunnel_entry_buf);

    /* need to look into EGR_L3_INTF table to get interface entry */
    mem = EGR_L3_INTFm;

    chunksize = 0x400; /* 1k */
    buf_size = sizeof(egr_l3_intf_entry_t) * chunksize;
    tbl_chnk = soc_cm_salloc(unit, buf_size, "egr_l3_intf traverse");
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

        soc_mem_lock(unit, mem);
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, tbl_chnk);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, mem);
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            el3inf_entry =
                soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                             tbl_chnk, ent_idx);

            mpls_index = soc_mem_field32_get(unit, mem, el3inf_entry,
                                            MPLS_TUNNEL_INDEXf);

            if (!mpls_index) {
                continue;
            }

            tnl_idx = mpls_index  /
                _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);
            mpls_off = mpls_index %
               _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);

            rv = bcmi_egr_ip_tnl_mpls_intf_list_add(unit, tunnel_entry,
                chnk_idx + ent_idx, tnl_idx, mpls_off);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        soc_mem_unlock(unit, mem);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    soc_cm_sfree(unit, tbl_chnk);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_mpls_tunnel_initiator_get
 * Purpose:
 *      Get MPLS Tunnel Initiator info
 * Parameters:
 *      unit        - (IN) Device Number
 *      intf        - (IN) The egress L3 interface
 *      label_max   - (IN) Number of entries in label_array
 *      label_array - (OUT) MPLS header information
 *      label_count - (OUT) Actual number of labels returned
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_mpls_tunnel_initiator_get(int unit, bcm_if_t intf, int label_max,
                                 bcm_mpls_egress_label_t *label_array,
                                 int *label_count)
{
    egr_l3_intf_entry_t if_entry;
    int rv, i, ix, offset;
    int tnl_index = 0, mpls_index = 0;
    bcmi_egr_ip_tnl_mpls_tunnel_entry_t **tunnel_entry = egr_mpls_tnl_sw_state[unit];
    int no_of_elements;
    int mpls_exp_select=0,mpls_tnl_ttl=0;
    uint32 iptun_mpls_entry_format[3];
    uint32 tnl_lbl=0, tnl_exp=0, tnl_exp_ptr=0;
    uint32 tnl_pri=0, tnl_cfi=0;
    uint32 tnl_entry[SOC_MAX_MEM_WORDS];

    bcmi_egr_ip_tnl_mpls_table_memset(unit, tnl_entry);

    if ((intf < 0) || (intf >= L3_INFO(unit)->l3_intf_table_size)) {
        return BCM_E_PARAM;
    }

    if (!BCM_L3_INTF_USED_GET(unit, intf)) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 interface not created\n")));
        return BCM_E_NOT_FOUND;
    }

    /* L3 interface info */
    rv = READ_EGR_L3_INTFm(unit, MEM_BLOCK_ANY, intf, &if_entry);
    if (rv < 0) {
        return rv;
    }
    mpls_index = soc_EGR_L3_INTFm_field32_get(unit, &if_entry,
                                              MPLS_TUNNEL_INDEXf);
    tnl_index = mpls_index / _BCM_MPLS_NUM_MPLS_ENTRIES_PER_INDEX(unit);

    if (!_BCM_MPLS_TNL_USED_GET(unit, mpls_index)) {
        return BCM_E_NOT_FOUND;
    }

    rv = bcmi_egr_ip_tnl_mpls_table_read(unit, tnl_index, tnl_entry);

    if (rv < 0) {
        return rv;
    }


    rv = bcmi_egr_ip_tnl_mpls_entry_type_check(unit, tnl_entry);

    if (rv < 0) {
        return BCM_E_NOT_FOUND;
    }

    *label_count = 0;
    offset = mpls_index & _BCM_MPLS_EGR_L3_INTF_MPLS_INDEX_OFFSET_MASK(unit);
    if (tunnel_entry[tnl_index]->label_entry[offset]->flags
        & BCMI_EGR_IP_TUNNEL_MPLS_START_OF_ENTRY) {

        if (tunnel_entry[tnl_index]->label_entry[offset]->flags
            & BCMI_EGR_IP_TUNNEL_MPLS_ENTRY_ZERO) {
            no_of_elements = 0;
        } else {
            no_of_elements =
                tunnel_entry[tnl_index]->label_entry[offset]->num_elements;
        }

    } else {
        return BCM_E_NOT_FOUND;
    }

    if ((label_array == NULL) || (label_max <= 0)) {
        *label_count = no_of_elements;
        return BCM_E_NONE;
    }

    if (label_max < no_of_elements) {
        *label_count = label_max;
    } else {
        *label_count = no_of_elements;
    }

    sal_memset(label_array, 0, sizeof(bcm_mpls_egress_label_t) * label_max);
    for (i = 0; i < *label_count; i++) {
        bcmi_egress_tnl_label_entry_get(unit, tnl_entry,
                offset + i,iptun_mpls_entry_format);

        bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                tnl_entry, offset + i,
                iptun_mpls_entry_format, bcmiSpecialLabelPushType,
                ((uint32 *)(&(label_array[i].spl_label_push_type))));

        bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                tnl_entry, offset + i,
                iptun_mpls_entry_format, bcmiMplsLabel,
                &tnl_lbl);

        label_array[i].label = tnl_lbl;
        bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                tnl_entry, offset + i,
                iptun_mpls_entry_format, bcmiMplsTtl,
                (uint32 *)&mpls_tnl_ttl);

        bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                tnl_entry, offset + i,
                iptun_mpls_entry_format, bcmiMplsExpSelect,
                (uint32 *)&mpls_exp_select);

        if (mpls_exp_select == 0x0) {
            /* Use the specified EXP, PRI and CFI */
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_PRI_SET;

            bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                    tnl_entry, offset + i,
                    iptun_mpls_entry_format, bcmiMplsExp,
                    &tnl_exp);

            label_array[i].exp = tnl_exp;
            bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                    tnl_entry, offset + i,
                    iptun_mpls_entry_format, bcmiNewPri,
                    &tnl_pri);

            label_array[i].pkt_pri = tnl_pri;
            bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                    tnl_entry, offset + i,
                    iptun_mpls_entry_format, bcmiNewCfi,
                    &tnl_cfi);

            label_array[i].pkt_cfi = tnl_cfi;

        } else if (mpls_exp_select == 0x1) {
            /* Use EXP-map for EXP, PRI and CFI */
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_EXP_REMARK;
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;

            bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                    tnl_entry, offset + i,
                    iptun_mpls_entry_format, bcmiMplsExpMappingPtr,
                    &tnl_exp_ptr);

            ix = tnl_exp_ptr;

            BCM_IF_ERROR_RETURN(
                    _egr_qos_hw_idx2id(unit, ix, &label_array[i].qos_map_id));
        } else {
            /* Use EXP from incoming label. If there is no incoming label,
             * use the specified EXP value.
             */
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;

            bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                    tnl_entry, offset + i,
                    iptun_mpls_entry_format, bcmiMplsExp,
                    &tnl_exp);

            label_array[i].exp = tnl_exp;
            bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                    tnl_entry, offset + i,
                    iptun_mpls_entry_format, bcmiMplsExpMappingPtr,
                    &tnl_exp_ptr);

            ix = tnl_exp_ptr;
            BCM_IF_ERROR_RETURN(
                    _egr_qos_hw_idx2id(unit, ix, &label_array[i].qos_map_id));
        }

        if (mpls_tnl_ttl){
            /* Use specified TTL value */
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
            label_array[i].ttl    = mpls_tnl_ttl;
        } else {
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        }
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT) 
        if (soc_feature(unit, soc_feature_mpls_ecn)) {
            int ecn_map_trust;
            int ecn_map_hw_idx;
            int ecn_map_id;
            int rv;

            if(soc_feature(unit, soc_feature_td3_style_mpls)) {
                bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                        tnl_entry, offset + i,
                        iptun_mpls_entry_format, bcmiPktEcnToExpMappingPtr,
                        (uint32 *)&ecn_map_hw_idx);
            } else {
            ecn_map_hw_idx = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, 
                                                 &tnl_entry,
                                                 _tnl_ecn_ptr_f[offset + i]);
            }

            rv = bcmi_ecn_map_hw_idx2id(unit, ecn_map_hw_idx, 
                                        _BCM_XGS5_MPLS_ECN_MAP_TYPE_ECN2EXP, 
                                        &ecn_map_id);  
            if (BCM_SUCCESS(rv)) {
                label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_ECN_TO_EXP_MAP;
                label_array[i].ecn_map_id = ecn_map_id;  

                if(soc_feature(unit, soc_feature_td3_style_mpls)) {
                    bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                            tnl_entry, offset + i,
                            iptun_mpls_entry_format, bcmiPktEcnToExpPriority,
                            (uint32 *)&ecn_map_trust);
                } else {
                ecn_map_trust = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, 
                                                    &tnl_entry,
                                                    _tnl_ecn_ptr_pri_f[offset + i]);
                }
                if (ecn_map_trust) {
                   label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST; 
                }
            }

            if (!soc_feature(unit, soc_feature_intcn_to_exp_nosupport)) {
            ecn_map_hw_idx = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, 
                                                &tnl_entry,
                                                _tnl_int_cn_ptr_f[offset + i]);
            rv = bcmi_ecn_map_hw_idx2id(unit, ecn_map_hw_idx, 
                                        _BCM_XGS5_MPLS_ECN_MAP_TYPE_INTCN2EXP, 
                                        &ecn_map_id);  
            if (BCM_SUCCESS(rv)) {
                label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_INT_CN_TO_EXP_MAP;
                label_array[i].int_cn_map_id = ecn_map_id;  
                ecn_map_trust = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, 
                                                    &tnl_entry,
                                                    _tnl_int_cn_ptr_pri_f[offset + i]);
                if (ecn_map_trust) {
                   label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST; 
                }
            }            
        }
        }
#endif
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *      bcmi_xgs5_mpls_failover_nw_port_match_get
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
bcmi_xgs5_mpls_failover_nw_port_match_get(int unit, bcm_mpls_port_t *mpls_port,
                                    int vp, mpls_entry_entry_t *return_ment)
{
    _bcm_tr_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);
    mpls_entry_entry_t ment;
    int entry_index = -1;
    int key_type = 0x00; /* mpls_key_type */
    int rv = BCM_E_NONE;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    bcm_gport_t gport_id;
    soc_mem_t mem = MPLS_ENTRYm;
    soc_field_t validf = VALIDf;

    if (soc_feature(unit, soc_feature_th3_style_simple_mpls)) {
        mem = MPLS_ENTRY_SINGLEm;
        validf = BASE_VALIDf;
    }

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
                soc_mem_field32_set(unit, mem, &ment, MPLS__Tf, 1);
                soc_mem_field32_set(unit, mem, &ment, MPLS__TGIDf, mpls_port->port);
            } else {
                if (SOC_MEM_FIELD_VALID(unit, mem, MPLS__MODULE_IDf)) {
                    soc_mem_field32_set(unit, mem, &ment, MPLS__MODULE_IDf, mod_out);
                }
                soc_mem_field32_set(unit, mem, &ment, MPLS__PORT_NUMf, port_out);
            }
        } else if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL) {
            if (SOC_MEM_FIELD_VALID(unit, mem, MPLS__MODULE_IDf)) {
                soc_mem_field32_set(unit, mem, &ment, MPLS__MODULE_IDf, 0);
            }
            soc_mem_field32_set(unit, mem, &ment, MPLS__PORT_NUMf, 0);
        } else {
            return BCM_E_NOT_FOUND;
        }

        /* Use old label to get old mpls entry for replace operation */
        soc_mem_field32_set(unit, mem, &ment, MPLS__MPLS_LABELf,
                                    mpls_port->match_label);

        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
            soc_mem_field32_set(unit, mem, &ment, BASE_VALID_0f, 3);
            soc_mem_field32_set(unit, mem, &ment, BASE_VALID_1f, 7);
        } else {
            soc_mem_field32_set(unit, mem, &ment, validf, 1);
        }

        soc_mem_field32_set(unit, mem, &ment, KEY_TYPEf, key_type);
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
                soc_mem_field32_set(unit, mem, &ment, MPLS__Tf, 1);
                soc_mem_field32_set(unit, mem, &ment, MPLS__TGIDf,
                    mpls_info->match_key[vp].trunk_id);
            } else {
                if (SOC_MEM_FIELD_VALID(unit, mem, MPLS__MODULE_IDf)) {
                    soc_mem_field32_set(unit, mem, &ment, MPLS__MODULE_IDf,
                        mpls_info->match_key[vp].modid);
                }
                soc_mem_field32_set(unit, mem, &ment, MPLS__PORT_NUMf,
                    mpls_info->match_key[vp].port);
            }
        }

        soc_mem_field32_set(unit, mem, &ment, MPLS__MPLS_LABELf,
            mpls_info->match_key[vp].match_label);
        soc_mem_field32_set(unit, mem, &ment, KEY_TYPEf, key_type);

        } else {
            return BCM_E_NOT_FOUND;
        }
    }

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &entry_index,
             &ment, return_ment, 0);

    return rv;
}

STATIC int
_bcmi_mpls_entry_field_val_set(int unit, void *tnl_buf, soc_field_t field,
        uint32 *p_fn_data, uint32 *set_val,
        uint32 offset, uint32 width)
{
  int rv = BCM_E_NONE;

#if defined(BCM_TRIDENT3_SUPPORT)
  if (soc_feature(unit, soc_feature_td3_style_mpls)) {
      uint32 u32_mask;
      int idx, wp, bp, len;

      if (NULL == p_fn_data || NULL == set_val) {
          LOG_VERBOSE(BSL_LS_BCM_MPLS, (BSL_META("Invalid Parameters\n")));
          return BCM_E_PARAM;
      }

      wp = offset / 32;
      bp = offset & (32 - 1);
      idx = 0;

      for (len = width; len > 0; len -= 32) {
          if (bp) {
              if (len < 32) {
                  u32_mask = (1 << len) - 1;
                  if ((set_val[idx] & ~u32_mask) != 0) {
                      return (BCM_E_PARAM);
                  }
              } else {
                  u32_mask = 0xffffffff;
              }

              p_fn_data[wp] &= ~(u32_mask << bp);
              p_fn_data[wp++] |= set_val[idx] << bp;
              p_fn_data[wp] &= ~(u32_mask >> (32 - bp));
              p_fn_data[wp] |= set_val[idx] >> (32 - bp) & ((1 << bp) - 1);

          } else {
              if (len < 32) {
                  u32_mask = (1 << len) - 1;
                  if ((set_val[idx] & ~u32_mask) != 0) {
                      return (BCM_E_PARAM);
                  }
                  p_fn_data[wp] &= ~u32_mask;
                  p_fn_data[wp++] |= set_val[idx];
              } else {
                  p_fn_data[wp++] = set_val[idx];
              }
          }

          idx++;
      }
  } else
#endif
  {
      soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, tnl_buf,
              field, *set_val);
  }

  return rv;
}

STATIC int
_bcmi_mpls_entry_field_val_get(int unit, void *tnl_buf, soc_field_t field,
        uint32 *p_fn_data, uint32 *p_data,
        uint32 offset, uint32 width)
{

  int rv = BCM_E_NONE;

#if defined(BCM_TRIDENT3_SUPPORT)
  if (soc_feature(unit, soc_feature_td3_style_mpls)) {
      int                 idx, wp, bp, len;

      /* Input parameters check. */
      if (NULL == p_fn_data || NULL == p_data) {
          LOG_VERBOSE(BSL_LS_BCM_MPLS, (BSL_META("Invalid Parameters\n")));
          return (BCM_E_PARAM);
      }

      wp = offset / 32;
      bp = offset & (32 - 1);
      idx = 0;

      for (len = width; len > 0; len -= 32) {
          if (bp) {
              p_data[idx] = (p_fn_data[wp++] >> bp) & ((1 << (32 - bp)) - 1);
              p_data[idx] |= p_fn_data[wp] << (32 - bp);
          } else {
              p_data[idx] = p_fn_data[wp++];
          }

          if (len < 32) {
              p_data[idx] &= ((1 << len) - 1);
          }

          idx++;
      }
  } else
#endif
  {
      soc_mem_t mem;
      uint32 value;
      mem = bcmi_egr_ip_tnl_mem_name_get(unit);
      value = soc_mem_field32_get(unit, mem, tnl_buf, field);
      *p_data = value;
  }
  return rv;
}


/*
 * Function:
 *     bcmi_egr_ip_tnl_mpls_entry_format_set 
 * Purpose:
 *      Set egress ip tunnel mpls entry format fields
 * Parameters:
 *      unit       - (IN)  Device Number
 *      field_type - (IN)  Field type
 *      tnl_buf    - (IN)  tnl entry buffer
 *      lbl_offset - (OUT) offset
 *      rebuf      - (OUT) return buffer
 *      value      - (IN)  value
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_egr_ip_tnl_mpls_entry_format_set(int unit,int field_type,
        void *tnl_buf, int lbl_offset,
        uint32 *retbuf, int value)
{
    int rv = BCM_E_NONE;
    uint32 set_val = 0;
    set_val = value;

    /*use tnl_buf,lbl_offset is non TD3 chips,
     *retbuf,offset,width is dont care.
     *use retbuf in TD3 tnl_buf,lbl_offset is dontcare.
     *Yet to set few fields in non TD3 chips*/
    switch (field_type) {
        case bcmiMplsLabel:

            rv = bcmi_validate_field_range(unit, set_val, 20);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], retbuf, &set_val, 0, 20);
            break;
        case bcmiMplsPushAction:

            rv = bcmi_validate_field_range(unit, set_val, 2);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_push_action_f[lbl_offset], retbuf, &set_val, 20, 2);
            break;
        case bcmiMplsExpSelect:

            rv = bcmi_validate_field_range(unit, set_val, 2);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_exp_select_f[lbl_offset], retbuf, &set_val, 22, 2);
            break;
        case bcmiMplsExpMappingPtr:

            rv = bcmi_validate_field_range(unit, set_val, 4);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_exp_ptr_f[lbl_offset], retbuf, &set_val, 24, 4);
            break;
        case bcmiNewCfi:

            rv = bcmi_validate_field_range(unit, set_val, 1);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_cfi_f[lbl_offset], retbuf, &set_val, 24, 1);
            break;
        case bcmiNewPri:

            rv = bcmi_validate_field_range(unit, set_val, 3);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_pri_f[lbl_offset], retbuf, &set_val, 25, 3);
            break;
        case bcmiMplsExp:

            rv = bcmi_validate_field_range(unit, set_val, 3);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_exp_f[lbl_offset], retbuf, &set_val, 28, 3);
            break;
        case bcmiLabelSelect:

            rv = bcmi_validate_field_range(unit, set_val, 1);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], retbuf, &set_val, 31, 1);
            break;
        case bcmiMplsTtl:

            rv = bcmi_validate_field_range(unit, set_val, 8);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_ttl_f[lbl_offset], retbuf, &set_val, 32, 8);
            break;
        case bcmiSpecialLabelPushType:

            rv = bcmi_validate_field_range(unit, set_val, 2);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], retbuf, &set_val, 40, 2);
            break;
        case bcmiExpMappingTableSelect:

            rv = bcmi_validate_field_range(unit, set_val, 2);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], retbuf, &set_val, 42, 2);
            break;
        case bcmiPktEcnToExpMappingPtr:

            rv = bcmi_validate_field_range(unit, set_val, 2);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], retbuf, &set_val, 44, 2);
            break;
        case bcmiPktEcnToExpPriority:

            rv = bcmi_validate_field_range(unit, set_val, 1);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], retbuf, &set_val, 46, 1);
            break;
        case bcmiEgrFlexCtrActionSet:

            rv = bcmi_validate_field_range(unit, set_val, 19);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], retbuf, &set_val, 47, 19);
            break;
        case bcmiFlexCtrBaseCounterIdx:

            rv = bcmi_validate_field_range(unit, set_val, 13);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], retbuf, &set_val, 47, 13);
            break;
        case bcmiFlexCtrPoolNumber:

            rv = bcmi_validate_field_range(unit, set_val, 2);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], retbuf, &set_val, 61, 2);
            break;
        case bcmiRsvdFlexCtrPoolNumber:

            rv = bcmi_validate_field_range(unit, set_val, 2);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], retbuf, &set_val, 63, 1);
            break;
        case bcmiFlexCtrOffsetMode:

            rv = bcmi_validate_field_range(unit, set_val, 5);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], retbuf, &set_val, 64, 2);
            break;
        case bcmiEcnMappingEnable:

            rv = bcmi_validate_field_range(unit, set_val, 1);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], retbuf, &set_val, 66, 1);
            break;
        case bcmiEcnMappingTableSelect:

            rv = bcmi_validate_field_range(unit, set_val, 2);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], retbuf, &set_val, 67, 2);
            break;
        case bcmiMplsEntryFormatReserved:

            rv = bcmi_validate_field_range(unit, set_val, 2);
            _bcmi_mpls_entry_field_val_set(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], retbuf, &set_val, 69, 2);
            break;
        default:
            rv =  BCM_E_PARAM;
            break;
    }

    return rv;
}

/*
 * Function:
 *     bcmi_egr_ip_tnl_mpls_entry_format_get
 * Purpose:
 *      get egress ip tunnel mpls entry format field value
 * Parameters:
 *      unit       - (IN)   Device Number
 *      tnl_buf    - (IN)   tnl entry buffer
 *      lbl_offset - (IN)   offset
 *      inbuf      - (IN)   input buffer
 *      field_type - (IN)   field type
 *      value      - (OUT)  return value
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_egr_ip_tnl_mpls_entry_format_get(int unit, void *tnl_buf, int lbl_offset,
        uint32 *inbuf, int field_type, uint32 *retval)
{
    int rv = BCM_E_NONE;

    switch (field_type) {
        case bcmiMplsLabel:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], inbuf, retval, 0, 20);
            break;
        case bcmiMplsPushAction:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_push_action_f[lbl_offset], inbuf, retval, 20, 2);
            break;
        case bcmiMplsExpSelect:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_exp_select_f[lbl_offset], inbuf, retval, 22, 2);
            break;
        case bcmiMplsExpMappingPtr:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_exp_ptr_f[lbl_offset], inbuf, retval, 24, 4);
            break;
        case bcmiNewCfi:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_cfi_f[lbl_offset], inbuf, retval, 24, 1);
            break;
        case bcmiNewPri:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_pri_f[lbl_offset], inbuf, retval, 25, 3);
            break;
        case bcmiMplsExp:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_exp_f[lbl_offset], inbuf, retval, 28, 3);
            break;
        case bcmiLabelSelect:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], inbuf, retval, 31, 1);
            break;
        case bcmiMplsTtl:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_ttl_f[lbl_offset], inbuf, retval, 32, 8);
            break;
        case bcmiSpecialLabelPushType:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], inbuf, retval, 40, 2);
            break;
        case bcmiExpMappingTableSelect:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], inbuf, retval, 42, 2);
            break;
        case bcmiPktEcnToExpMappingPtr:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], inbuf, retval, 44, 2);
            break;
        case bcmiPktEcnToExpPriority:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], inbuf, retval, 46, 1);
            break;
        case bcmiEgrFlexCtrActionSet:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                _tnl_label_f[lbl_offset], inbuf, retval, 47, 19);
            break;
        case bcmiFlexCtrBaseCounterIdx:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], inbuf, retval, 47, 13);
            break;
        case bcmiFlexCtrPoolNumber:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], inbuf, retval, 61, 2);
            break;
        case bcmiRsvdFlexCtrPoolNumber:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], inbuf, retval, 63, 1);
            break;
        case bcmiFlexCtrOffsetMode:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], inbuf, retval, 64, 2);
            break;
        case bcmiEcnMappingEnable:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], inbuf, retval, 66, 1);
            break;
        case bcmiEcnMappingTableSelect:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], inbuf, retval, 67, 2);
            break;
        case bcmiMplsEntryFormatReserved:
            _bcmi_mpls_entry_field_val_get(unit, tnl_buf,
                    _tnl_label_f[lbl_offset], inbuf, retval, 69, 2);
            break;
        default:
            rv =  BCM_E_PARAM;
            break;
    }

    return rv;
}

void
bcm_td3_ip_tnl_mpls_label_info_get(int unit, void *tnl_entry, int field_type,
        int offset, int *retval)
{
    uint32 iptun_mpls_entry_format[3] = {0};

    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
        bcmi_egress_tnl_label_entry_get(unit, tnl_entry,
                offset, iptun_mpls_entry_format);


        bcmi_egr_ip_tnl_mpls_entry_format_get(unit,
                tnl_entry, offset,
                iptun_mpls_entry_format, field_type,
                (uint32 *)retval);
    }
}

#endif /* (BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT) */
#endif /* BCM_MPLS_SUPPORT */
#endif /* INCLUDE_L3 */


/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l3.c
 * Purpose:     Triumph2 L3 function implementations
 */


#include <soc/defs.h>

#include <assert.h>
#include <shared/bsl.h>
#include <sal/core/libc.h>
#if defined(BCM_TRIUMPH2_SUPPORT)  && defined(INCLUDE_L3)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/l3x.h>
#include <soc/lpm.h>
#include <soc/tnl_term.h>
#include <soc/defragment.h>

#include <bcm/l3.h>
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw_dispatch.h>
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT*/
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <bcm_int/esw/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#include <bcm_int/esw/ecn.h>

static soc_profile_mem_t *_bcm_tnl_port_bitmap_profile[BCM_MAX_NUM_UNITS];
#ifdef BCM_TOMAHAWK3_SUPPORT
/*
 * The following are remembered when tnl_term_global_cfg setting,
 * so we can return accurate value for tnl_term_global_cfg_get().
 * Also these values should be saved and restored for WB to work correctly.
 */
static bcm_tunnel_type_t last_global_cfg_tnl_type1[BCM_MAX_NUM_UNITS] = {bcmTunnelTypeNone};
static bcm_tunnel_type_t last_global_cfg_tnl_type2[BCM_MAX_NUM_UNITS] = {bcmTunnelTypeNone};
#endif

soc_profile_mem_t *_bcm_get_tnl_port_bitmap_profile(int unit) {
    return _bcm_tnl_port_bitmap_profile[unit];
}

int
_bcm_tnl_port_bitmap_profile_init(int unit)
{
    int i, idx;
    soc_mem_t mem;
    int entry_words[1];
    void *entries[1];
    allowed_port_bitmap_profile_entry_t allowed_ports_profile;
    void *prof_entry;
    uint32 ent_sz;
    int temp_idx = 0, key_type = 0;
    uint32 entry_ptr[SOC_MAX_MEM_WORDS];
    uint32 temp_index = 0;
    soc_mem_t tunnel_mem = L3_TUNNEL_DOUBLEm;

    /* Create profile table cache (or re-init if it already exists) */
    mem = ALLOWED_PORT_BITMAP_PROFILEm;
    ent_sz = sizeof(allowed_port_bitmap_profile_entry_t);
    prof_entry = &allowed_ports_profile;

    if (_bcm_tnl_port_bitmap_profile[unit] == NULL) {
        _bcm_tnl_port_bitmap_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                             "TUNNEL Port bitmap Profile Mem");
        if (_bcm_tnl_port_bitmap_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_tnl_port_bitmap_profile[unit]);
    }

    /* Create profile table cache (or re-init if it already exists) */
    entry_words[0] = ent_sz / sizeof(uint32);
    SOC_IF_ERROR_RETURN(
        soc_profile_mem_create(unit, &mem, entry_words, 1,
                           _bcm_tnl_port_bitmap_profile[unit]));

    if (SOC_WARM_BOOT(unit)) {

        /* One extra increment to preserve location
         * ING_ACTION_PROFILE_DEFAULT */
        SOC_PROFILE_MEM_REFERENCE(unit, _bcm_tnl_port_bitmap_profile[unit],
                                  0, 1);

        for (i = 0; i < soc_mem_index_count(unit, tunnel_mem); i++) {

            sal_memset(prof_entry, 0, ent_sz);
            SOC_IF_ERROR_RETURN
                (READ_L3_TUNNEL_DOUBLEm(unit, MEM_BLOCK_ANY, i, &entry_ptr));
                idx = soc_mem_field32_get(unit, tunnel_mem,
                             &entry_ptr, BASE_VALID_0f);
            if (idx == 0) {
                continue;
            }

            key_type = soc_mem_field32_get(unit, tunnel_mem,
                             &entry_ptr, KEY_TYPEf);
            if (key_type == 0) {
                continue;
            }
            /* Check key type to know memory type. */
            tunnel_mem = ((key_type == 2) ?
                              L3_TUNNEL_QUADm : L3_TUNNEL_DOUBLEm);

            if (tunnel_mem == L3_TUNNEL_QUADm) {
               /* If this is quad entry then divide the index by 2. */
               temp_idx = i/2;
               /* Move one index further as two indexes will be used here.*/
               i++;
               SOC_IF_ERROR_RETURN(
                   soc_mem_read(unit, tunnel_mem, MEM_BLOCK_ANY,
                       temp_idx, &entry_ptr));
            }

            /* Get the index of ALLOWED_PORT_BITMAP_PROFILE table. */
            idx = soc_mem_field32_get(unit, tunnel_mem, entry_ptr,
                        (tunnel_mem == L3_TUNNEL_DOUBLEm) ?
                        IPV4__ALLOWED_PORT_BITMAP_PROFILE_PTRf:
                        IPV6__ALLOWED_PORT_BITMAP_PROFILE_PTRf);

            SOC_PROFILE_MEM_REFERENCE(unit, _bcm_tnl_port_bitmap_profile[unit], idx, 1);
            SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, _bcm_tnl_port_bitmap_profile[unit], idx, 1);

       }
    } else {
        sal_memset(prof_entry, 0, ent_sz);
        entries[0] = prof_entry;
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, _bcm_tnl_port_bitmap_profile[unit],
                                 (void *) &entries, 1, &temp_index));

        SOC_PROFILE_MEM_REFERENCE(unit, _bcm_tnl_port_bitmap_profile[unit],
            0,
            soc_mem_index_count(unit, tunnel_mem));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th3_l3_tnl_term_entry_init
 * Purpose:
 *      Initialize soc tunnel terminator entry key portion.
 * Parameters:
 *      unit     - (IN)  BCM device number.
 *      tnl_info - (IN)  BCM buffer with tunnel info.
 *      entry    - (OUT) SOC buffer with key filled in.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th3_l3_tnl_term_entry_init(int unit, bcm_tunnel_terminator_t *tnl_info,
                                soc_tunnel_term_t *entry)
{
    uint32    *entry_ptr;         /* Filled entry pointer.      */
    _bcm_tnl_term_type_t tnl_type;/* Tunnel type.               */
    soc_mem_t mem = L3_TUNNEL_DOUBLEm;
    uint32              ip6_field[4];
    uint8 *ip6 = NULL;
    soc_field_t protocol_f = IPV4__PROTOCOLf;
    soc_field_t Bfd_enable_f = IPV4__BFD_ENABLEf;

    /* Input parameters check. */
    if ((NULL == tnl_info) || (NULL == entry)) {
        return (BCM_E_PARAM);
    }

    /* Get tunnel type & sub_type. */
    BCM_IF_ERROR_RETURN
        (_bcm_xgs3_l3_set_tnl_term_type(unit, tnl_info, &tnl_type));

    if (tnl_type.tnl_outer_hdr_ipv6 == 1) {
       mem = L3_TUNNEL_QUADm;
    } else {
       mem = L3_TUNNEL_DOUBLEm;
    }

    /* Reset destination structure. */
    sal_memset(entry, 0, sizeof(soc_tunnel_term_t));

    /* Set Destination/Source pair. */
    entry_ptr = (uint32 *)&entry->entry_arr[0];
    if (tnl_type.tnl_outer_hdr_ipv6 == 1) {
        protocol_f = IPV6__PROTOCOLf;
        Bfd_enable_f = IPV6__BFD_ENABLEf;
        ip6 = tnl_info->sip6;

        ip6_field[2] = (((ip6[4] & 0x3) << 24) | (ip6[5] << 16) |
                        (ip6[6] << 8)  | (ip6[7] << 0));
        ip6_field[1] = ((ip6[8] << 24) | (ip6[9] << 16) |
                        (ip6[10] << 8) | (ip6[11] << 0));
        ip6_field[0] = ((ip6[12] << 24)| (ip6[13] << 16) |
                        (ip6[14] << 8) | (ip6[15] << 0));

        /* SIP [0-89] */
        soc_mem_field_set(unit, mem, (uint32 *)&entry->entry_arr[0], IPV6__SIP_LWR_90f, ip6_field);
        /* SIP [90-127] */
        ip6_field[1] = (ip6[0] >> 2);
        ip6_field[0] = ((ip6[1] << 24)| (ip6[2] << 16) |
                        (ip6[3] << 8) | (ip6[4] & 0xfc));
        ip6_field[0] = ((ip6[0] & 3) << 30) | (ip6_field[0] >> 2);
        soc_mem_field_set(unit, mem, (uint32 *)&entry->entry_arr[0], IPV6__SIP_UPR_38f, ip6_field);
        ip6 = tnl_info->dip6;

        ip6_field[2] = (((ip6[4] & 0x3) << 24) | (ip6[5] << 16) |
                        (ip6[6] << 8)  | (ip6[7] << 0));
        ip6_field[1] = ((ip6[8] << 24) | (ip6[9] << 16) |
                        (ip6[10] << 8) | (ip6[11] << 0));
        ip6_field[0] = ((ip6[12] << 24)| (ip6[13] << 16) |
                        (ip6[14] << 8) | (ip6[15] << 0));
        /* DIP [0-89] */
        soc_mem_field_set(unit, mem, (uint32 *)&entry->entry_arr[0], IPV6__DIP_LWR_90f, ip6_field);
        /* DIP [90-127] */
        ip6_field[1] = (ip6[0] >> 2);
        ip6_field[0] = ((ip6[1] << 24)| (ip6[2] << 16) |
                        (ip6[3] << 8) | (ip6[4] & 0xfc));
        ip6_field[0] = ((ip6[0] & 3) << 30) | (ip6_field[0] >> 2);
        soc_mem_field_set(unit, mem, (uint32 *)&entry->entry_arr[0], IPV6__DIP_UPR_38f, ip6_field);

        soc_mem_field32_set(unit, mem, entry_ptr, IPV6__SUB_TUNNEL_TYPEf,
                                   tnl_type.tnl_sub_type);

        /* Set tunnel type. */
        soc_mem_field32_set(unit, mem, entry_ptr, IPV6__TUNNEL_TYPEf,
                                   tnl_type.tnl_auto);
    }  else if (tnl_type.tnl_outer_hdr_ipv6 == 0) {

        /* Set destination ip. */
        soc_mem_field32_set(unit, mem, entry_ptr, IPV4__DIPf, tnl_info->dip);

        /* Set source ip. */
        soc_mem_field32_set(unit, mem, entry_ptr, IPV4__SIPf, tnl_info->sip);

        /* Set tunnel subtype. */
        soc_mem_field32_set(unit, mem, entry_ptr, IPV4__SUB_TUNNEL_TYPEf,
                                   tnl_type.tnl_sub_type);

        /* Set tunnel type. */
        soc_mem_field32_set(unit, mem, entry_ptr, IPV4__TUNNEL_TYPEf,
                                   tnl_type.tnl_auto);
    }


    /* Set valid bit. */
    soc_mem_field32_set(unit, mem, entry_ptr, BASE_VALID_0f, 1);
    soc_mem_field32_set(unit, mem, entry_ptr, BASE_VALID_1f, 2);

    if (tnl_type.tnl_outer_hdr_ipv6 == 1) {
        soc_mem_field32_set(unit, mem, entry_ptr, BASE_VALID_2f, 2);
        soc_mem_field32_set(unit, mem, entry_ptr, BASE_VALID_3f, 2);
    }


    /* Set entry mode. */

    if (SOC_MEM_FIELD_VALID(unit, mem, KEY_TYPEf)) {
        soc_mem_field32_set(unit, mem, entry_ptr, KEY_TYPEf,
                tnl_type.tnl_outer_hdr_ipv6 + 1);
    }

    /* Set the PROTOCOL field */
    if ((tnl_info->type == bcmTunnelTypeIpAnyIn4) ||
        (tnl_info->type == bcmTunnelTypeIpAnyIn6))
    {
      /* Set PROTOCOL and PROTOCOL_MASK field to zero for IpAnyInx tunnel type*/
       soc_mem_field32_set(unit, mem, entry_ptr, protocol_f, 0x0);
    } else {
        soc_mem_field32_set(unit, mem, entry_ptr, protocol_f,
                                   tnl_type.tnl_protocol);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, Bfd_enable_f)) {
        soc_mem_field32_set(unit, mem, entry_ptr, Bfd_enable_f, 0);
    }

    /* AMT tunnels not supported */

    /* Set GRE payload */
    if (tnl_type.tnl_gre) {
        if (tnl_type.tnl_outer_hdr_ipv6 == 1) {
            /* GRE IPv6 payload is allowed. */
            soc_mem_field32_set(unit, mem, entry_ptr, IPV6__GRE_PAYLOAD_IPV6f,
                                       tnl_type.tnl_gre_v6_payload);

            /* GRE IPv6 payload is allowed. */
            soc_mem_field32_set(unit, mem, entry_ptr, IPV6__GRE_PAYLOAD_IPV4f,
                                       tnl_type.tnl_gre_v4_payload);
            /* Save vlan id for ipmc lookup.*/
            if((tnl_info->vlan) && SOC_MEM_FIELD_VALID(unit, mem, IPV6__L3_IIFf)) {
                soc_mem_field32_set(unit, mem, entry_ptr, IPV6__L3_IIFf, tnl_info->vlan);
            }

        } else {

            /* GRE IPv6 payload is allowed. */
            soc_mem_field32_set(unit, mem, entry_ptr, IPV4__GRE_PAYLOAD_IPV6f,
                                       tnl_type.tnl_gre_v6_payload);

            /* GRE IPv6 payload is allowed. */
            soc_mem_field32_set(unit, mem, entry_ptr, IPV4__GRE_PAYLOAD_IPV4f,
                                       tnl_type.tnl_gre_v4_payload);

            /* Save vlan id for ipmc lookup.*/
            if((tnl_info->vlan) && SOC_MEM_FIELD_VALID(unit, mem, IPV4__L3_IIFf)) {
                soc_mem_field32_set(unit, mem, entry_ptr, IPV4__L3_IIFf, tnl_info->vlan);
            }
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_th3_l3_tnl_term_add
 * Purpose:
 *      Add tunnel termination entry to the hw.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      tnl_info - (IN)Tunnel terminator parameters.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th3_l3_tnl_term_add(int unit, uint32 *entry_ptr, bcm_tunnel_terminator_t *tnl_info)
{
    bcm_module_t mod_in, mod_out, my_mod;
    bcm_port_t port_in, port_out;
    int wlan;
    int rv;

    /* Program remote port */
    if ((tnl_info->type == bcmTunnelTypeWlanWtpToAc) ||
        (tnl_info->type == bcmTunnelTypeWlanAcToAc) ||
        (tnl_info->type == bcmTunnelTypeWlanWtpToAc6) ||
        (tnl_info->type == bcmTunnelTypeWlanAcToAc6)) {
        wlan = 1;
    } else {
        wlan = 0;
    }

    if (wlan) {
        if (tnl_info->flags & BCM_TUNNEL_TERM_WLAN_REMOTE_TERMINATE) {
            if (!BCM_GPORT_IS_MODPORT(tnl_info->remote_port)) {
                return BCM_E_PARAM;
            }
            mod_in = BCM_GPORT_MODPORT_MODID_GET(tnl_info->remote_port);
            port_in = BCM_GPORT_MODPORT_PORT_GET(tnl_info->remote_port);
            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, mod_in,
                                        port_in, &mod_out, &port_out));
            if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
                return (BCM_E_BADID);
            }
            if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
                return (BCM_E_PORT);
            }
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, REMOTE_TERM_GPPf,
                                       (mod_out << 6) | port_out);
        } else {
            /* Send to the local loopback port */
            rv = bcm_esw_stk_my_modid_get(unit, &my_mod);
            BCM_IF_ERROR_RETURN(rv);
            port_in = 54;
            mod_in = my_mod;
            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, mod_in,
                                        port_in, &mod_out, &port_out);
            BCM_IF_ERROR_RETURN(rv);

            soc_L3_TUNNELm_field32_set(unit, entry_ptr, REMOTE_TERM_GPPf,
                                       (mod_out << 6) | port_out);
        }
#if 0
        /* Program tunnel id */
        if (tnl_info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) {
            if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
                return BCM_E_PARAM;
            }
            if (SOC_MEM_FIELD_VALID(unit, L3_TUNNELm, TUNNEL_IDf)) {
                tunnel = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);
                soc_L3_TUNNELm_field32_set(unit, entry_ptr, TUNNEL_IDf,
                                           tunnel);
            }
        }
#endif

    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr2_l3_tnl_term_entry_parse
 * Purpose:
 *      Parse tunnel terminator entry portion.
 * Parameters:
 *      unit     - (IN)  BCM device number.
 *      entry    - (IN)  SOC buffer with tunne information.
 *      tnl_info - (OUT) BCM buffer with tunnel info.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th3_l3_tnl_term_entry_parse(int unit, soc_tunnel_term_t *entry,
                                  bcm_tunnel_terminator_t *tnl_info)
{
    uint32              ip6_field[4];
    _bcm_tnl_term_type_t tnl_type;     /* Tunnel type information.   */
    uint32 *entry_ptr;                 /* Filled entry pointer.      */
    uint8 *ip6 = NULL;
    soc_mem_t mem = L3_TUNNEL_DOUBLEm;
    allowed_port_bitmap_profile_entry_t allowed_ports_profile;
    void *profile_entries[1];
    int prof_id;

    /* Input parameters check. */
    if ((NULL == tnl_info) || (NULL == entry)) {
        return (BCM_E_PARAM);
    }

    /* Reset destination structure. */
    bcm_tunnel_terminator_t_init(tnl_info);
    sal_memset(&tnl_type, 0, sizeof(_bcm_tnl_term_type_t));

    entry_ptr = (uint32 *)&entry->entry_arr[0];

    /* Get valid bit. */
    if (!soc_mem_field32_get(unit, mem, entry_ptr, BASE_VALID_0f)) {
        return (BCM_E_NOT_FOUND);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, KEY_TYPEf)) {
        tnl_type.tnl_outer_hdr_ipv6 =
            soc_mem_field32_get(unit, mem, entry_ptr, KEY_TYPEf) - 1;

    }

    /* Get Destination/Source pair. */
    if (tnl_type.tnl_outer_hdr_ipv6 == 1) {
        mem = L3_TUNNEL_QUADm;
        ip6 = tnl_info->sip6;
        /* SIP [0-89] */
        soc_mem_field_get(unit, mem, (uint32 *)&entry->entry_arr[0], IPV6__SIP_LWR_90f, ip6_field);

        ip6[4] = (ip6_field[2]  >> 24) & 0x3;
        ip6[5] = (ip6_field[2]  >> 16) & 0xff;
        ip6[6] = (ip6_field[2]  >> 8) & 0xff;
        ip6[7] = (ip6_field[2] & 0xff);

        ip6[8] = (ip6_field[1] >> 24) & 0xff;
        ip6[9] = (ip6_field[1]  >> 16) & 0xff;
        ip6[10] = (ip6_field[1]  >> 8) & 0xff;
        ip6[11] = (ip6_field[1] & 0xff);

        ip6[12] = (ip6_field[0] >> 24) & 0xff;
        ip6[13] = (ip6_field[0]  >> 16) & 0xff;
        ip6[14] = (ip6_field[0]  >> 8) & 0xff;
        ip6[15] = (ip6_field[0] & 0xff);

        /* SIP [90-127] */
        soc_mem_field_get(unit, mem, (uint32 *)&entry->entry_arr[0], IPV6__SIP_UPR_38f, ip6_field);
        ip6[0] = (ip6_field[0] >> 30);
        ip6_field[1] <<= 2;
        ip6_field[0] <<= 2;
        ip6[0] |= (ip6_field[1] & 0xff);
        ip6[1] = (ip6_field[0]  >> 24) & 0xff;
        ip6[2] = (ip6_field[0]  >> 16) & 0xff;
        ip6[3] = (ip6_field[0]  >> 8) & 0xff;
        ip6[4] |= (ip6_field[0] & 0xff);

        ip6 = tnl_info->dip6;
        /* DIP [0-89] */
        soc_mem_field_get(unit, mem, (uint32 *)&entry->entry_arr[0], IPV6__DIP_LWR_90f, ip6_field);
        ip6[4] = (ip6_field[2]  >> 24) & 0x3;
        ip6[5] = (ip6_field[2]  >> 16) & 0xff;
        ip6[6] = (ip6_field[2]  >> 8) & 0xff;
        ip6[7] = (ip6_field[2] & 0xff);

        ip6[8] = (ip6_field[1] >> 24) & 0xff;
        ip6[9] = (ip6_field[1]  >> 16) & 0xff;
        ip6[10] = (ip6_field[1]  >> 8) & 0xff;
        ip6[11] = (ip6_field[1] & 0xff);

        ip6[12] = (ip6_field[0] >> 24) & 0xff;
        ip6[13] = (ip6_field[0]  >> 16) & 0xff;
        ip6[14] = (ip6_field[0]  >> 8) & 0xff;
        ip6[15] = (ip6_field[0] & 0xff);

        /* DIP [90-127] */
        soc_mem_field_get(unit, mem, (uint32 *)&entry->entry_arr[0], IPV6__DIP_UPR_38f, ip6_field);
        ip6[0] = (ip6_field[0] >> 30);
        ip6_field[1] <<= 2;
        ip6_field[0] <<= 2;
        ip6[0] |= (ip6_field[1] & 0xff);
        ip6[1] = (ip6_field[0]  >> 24) & 0xff;
        ip6[2] = (ip6_field[0]  >> 16) & 0xff;
        ip6[3] = (ip6_field[0]  >> 8) & 0xff;
        ip6[4] |= (ip6_field[0] & 0xff);

    }  else if (tnl_type.tnl_outer_hdr_ipv6 == 0) {
        /* Get destination ip. */
        tnl_info->dip = soc_mem_field32_get(unit, mem, entry_ptr, IPV4__DIPf);

        /* Get source ip. */
        tnl_info->sip = soc_mem_field32_get(unit, mem, entry_ptr, IPV4__SIPf);

        /* Destination subnet mask. */
        tnl_info->dip_mask = BCM_XGS3_L3_IP4_FULL_MASK;

    }

    /* Get tunnel subtype. */
    tnl_type.tnl_sub_type =
        soc_mem_field32_get(unit, mem, entry_ptr, (mem == L3_TUNNEL_DOUBLEm) ? IPV4__SUB_TUNNEL_TYPEf:IPV6__SUB_TUNNEL_TYPEf);

    prof_id = soc_mem_field32_get(unit, mem, entry_ptr,
                            (mem == L3_TUNNEL_DOUBLEm) ?
                            IPV4__ALLOWED_PORT_BITMAP_PROFILE_PTRf:
                            IPV6__ALLOWED_PORT_BITMAP_PROFILE_PTRf);
    profile_entries[0] = &allowed_ports_profile;
    soc_profile_mem_get(unit, _bcm_get_tnl_port_bitmap_profile(unit), prof_id,
                        1, profile_entries);
    soc_mem_pbmp_field_get(unit, ALLOWED_PORT_BITMAP_PROFILEm,
                           &allowed_ports_profile,
                           ING_PORT_BITMAPf, &tnl_info->pbmp);

    /* Get UDP tunnel type. */
    if (soc_mem_field_valid(unit, mem, IPV4__UDP_TUNNEL_TYPEf)) {
        tnl_type.tnl_udp_type =
            soc_mem_field32_get(unit, mem, entry_ptr, IPV4__UDP_TUNNEL_TYPEf);
    }
    /* Get tunnel type. */
    tnl_type.tnl_auto =
        soc_mem_field32_get(unit, mem, entry_ptr, (mem == L3_TUNNEL_DOUBLEm) ? IPV4__TUNNEL_TYPEf: IPV6__TUNNEL_TYPEf);

    /* Copy DSCP from outer header flag. */
    if (soc_mem_field32_get(unit, mem, entry_ptr, (mem == L3_TUNNEL_DOUBLEm) ? IPV4__USE_OUTER_HDR_DSCPf: IPV6__USE_OUTER_HDR_DSCPf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP;
    }
    /* Copy TTL from outer header flag. */
    if (soc_mem_field32_get(unit, mem, entry_ptr, (mem == L3_TUNNEL_DOUBLEm) ? IPV4__USE_OUTER_HDR_TTLf: IPV6__USE_OUTER_HDR_TTLf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_USE_OUTER_TTL;
    }
    /* Keep inner header DSCP flag. */
    if (soc_mem_field32_get(unit, mem, entry_ptr,
                                   (mem == L3_TUNNEL_DOUBLEm) ? IPV4__DONOT_CHANGE_INNER_HDR_DSCPf:IPV6__DONOT_CHANGE_INNER_HDR_DSCPf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_KEEP_INNER_DSCP;
    }

    /* Tunnel or IPMC lookup vlan id */
    tnl_info->vlan = soc_mem_field32_get(unit, mem, entry_ptr, (mem == L3_TUNNEL_DOUBLEm) ? IPV4__IINTFf:IPV6__IINTFf);

    /*  Get trust dscp per tunnel */
    if (soc_mem_field32_get(unit, mem, entry_ptr, (mem == L3_TUNNEL_DOUBLEm) ? IPV4__USE_OUTER_HDR_DSCPf:IPV6__USE_OUTER_HDR_DSCPf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_DSCP_TRUST;
    }

    /* Get Tunnel class id for VFP match */
        tnl_info->tunnel_class = soc_mem_field32_get(unit, mem, entry_ptr,
                               (mem == L3_TUNNEL_DOUBLEm) ? IPV4__TUNNEL_CLASS_IDf:IPV6__TUNNEL_CLASS_IDf);

    /* Get the protocol field and make some decisions */
    tnl_type.tnl_protocol = soc_mem_field32_get(unit, mem, entry_ptr,
                               (mem == L3_TUNNEL_DOUBLEm) ? IPV4__PROTOCOLf:IPV6__PROTOCOLf);
    switch (tnl_type.tnl_protocol) {
        case 0x2F:
            tnl_type.tnl_gre = 1;
            break;
        case 0x67:
            tnl_type.tnl_pim_sm = 1;
        default:
            break;
    }
    /* Get gre IPv4 payload allowed. */
    tnl_type.tnl_gre_v4_payload =
        soc_mem_field32_get(unit, mem, entry_ptr, (mem == L3_TUNNEL_DOUBLEm) ? IPV4__GRE_PAYLOAD_IPV4f:IPV6__GRE_PAYLOAD_IPV4f);

    /* Get gre IPv6 payload allowed. */
    tnl_type.tnl_gre_v6_payload =
        soc_mem_field32_get(unit, mem, entry_ptr, (mem == L3_TUNNEL_DOUBLEm) ? IPV4__GRE_PAYLOAD_IPV6f:IPV6__GRE_PAYLOAD_IPV6f);

    /* Get the L4 data */
    if (soc_mem_field_valid (unit, mem, L4_SRC_PORTf)) {
        tnl_info->udp_src_port = soc_mem_field32_get(unit, mem, entry_ptr,
                                                        (mem == L3_TUNNEL_DOUBLEm) ? IPV4__L4_SRC_PORTf:IPV6__L4_SRC_PORTf);
    }
    if (soc_mem_field_valid (unit, mem, L4_DEST_PORTf)) {
        tnl_info->udp_dst_port = soc_mem_field32_get(unit, mem, entry_ptr,
                                                        (mem == L3_TUNNEL_DOUBLEm) ? IPV4__L4_DEST_PORTf:IPV6__L4_DEST_PORTf);
    }

    /* Get tunnel type, sub_type and protocol. */
    BCM_IF_ERROR_RETURN
        (_bcm_xgs3_l3_get_tnl_term_type(unit, tnl_info, &tnl_type));
    /* Get IIF if l3 ingress mode is not set */
    if (!BCM_XGS3_L3_INGRESS_MODE_ISSET(unit) &&
        BCM_XGS3_L3_ING_IF_TBL_SIZE(unit) && (tnl_info->vlan > BCM_VLAN_MAX)) {
        _bcm_l3_ingress_intf_t iif;
        sal_memset(&iif, 0, sizeof(_bcm_l3_ingress_intf_t));
        iif.intf_id = tnl_info->vlan;
        BCM_IF_ERROR_RETURN(_bcm_tr_l3_ingress_interface_get(unit, NULL, &iif));

        tnl_info->vrf = iif.vrf;
        tnl_info->if_class = iif.if_class;
        tnl_info->qos_map_id = iif.qos_map_id;
        if (SOC_IS_TOMAHAWKX(unit) && soc_feature(unit, soc_feature_ecn_wred)) {
            if (bcmi_xgs5_ecn_map_used_get(unit, iif.tunnel_term_ecn_map_id,
                                           _bcmEcnmapTypeTunnelTerm)) {
                tnl_info->tunnel_term_ecn_map_id = iif.tunnel_term_ecn_map_id |
                    _BCM_XGS5_ECN_MAP_TYPE_TUNNEL_TERM;
                tnl_info->flags |= BCM_TUNNEL_TERM_ECN_MAP;
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_tomahawk3_l3_tunnel_term_global_cfg_set
 * Purpose:
 *      Get the configured l3 tunnel termination
 *      global attributes.
 * Parameters:
 *      unit    - (IN)SOC unit number. 
 *      extn_attribute - (IN) l3 tunnel global attributes
 *      to configure.
 * Returns:
 *      BCM_E_XXX
 */

#ifdef BCM_TOMAHAWK3_SUPPORT
int
bcm_tomahawk3_l3_tunnel_term_global_cfg_set(int unit,
                          bcm_l3_tunnel_term_extn_t *extn_attribute)
{
    int rv = BCM_E_UNAVAIL;
    uint32 ing_q_begin;
    uint64 ing_config_64;
    uint16 protocol1, protocol2;
    uint16      dev_id;
    uint8       rev_id;
    bcm_tunnel_terminator_t tnl_info;
    _bcm_tnl_term_type_t tnl_term_type;

    if (soc_cm_get_id(unit, &dev_id, &rev_id) < 0) {
        return BCM_E_FAIL;
    }
    if (((dev_id & BCM56980_DEVICE_ID_MASK) == BCM56980_DEVICE_ID)
                    && (rev_id == BCM56980_B0_REV_ID)) {

        rv = READ_ING_Q_BEGINr(unit, &ing_q_begin);
        SOC_IF_ERROR_RETURN(rv);

        /* convert tnl_type value to protocol value known to hardware  */

        tnl_info.type = extn_attribute->tnl_type1;
        BCM_IF_ERROR_RETURN(_bcm_xgs3_l3_set_tnl_term_type(unit, &tnl_info, &tnl_term_type));
        protocol1 = tnl_term_type.tnl_protocol;

        tnl_info.type = extn_attribute->tnl_type2;
        BCM_IF_ERROR_RETURN(_bcm_xgs3_l3_set_tnl_term_type(unit, &tnl_info, &tnl_term_type));
        protocol2 = tnl_term_type.tnl_protocol;

        soc_reg_field_set(unit, ING_Q_BEGINr, &ing_q_begin, L3_TUNNEL_WAR_IPV4_MASKf, extn_attribute->dip_mask);
        soc_reg_field_set(unit, ING_Q_BEGINr, &ing_q_begin, L3_TUNNEL_WAR_PROTOCOL_REG1f, protocol1);
        soc_reg_field_set(unit, ING_Q_BEGINr, &ing_q_begin, L3_TUNNEL_WAR_PROTOCOL_REG2f, protocol2);

        rv = WRITE_ING_Q_BEGINr(unit, ing_q_begin);
        SOC_IF_ERROR_RETURN(rv);

        rv = READ_ING_CONFIG_64r(unit, &ing_config_64);
        SOC_IF_ERROR_RETURN(rv);
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &ing_config_64,
                                    L3_TUNNEL_WAR_ENABLEf, extn_attribute->extn_enable);
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &ing_config_64,
                                    L3_TUNNEL_WAR_SIP_MASK_ENABLEf, extn_attribute->sip_mask_enable);
        rv = WRITE_ING_CONFIG_64r(unit, ing_config_64);
        SOC_IF_ERROR_RETURN(rv);
        last_global_cfg_tnl_type1[unit] = extn_attribute->tnl_type1;
        last_global_cfg_tnl_type2[unit] = extn_attribute->tnl_type2;
    }
    return rv;
}
/*
 * Function:
 *      bcm_tomahawk3_l3_tunnel_term_global_cfg_get
 * Purpose:
 *      Get the configured l3 tunnel termination
 *      global attributes.
 * Parameters:
 *      unit    - (IN)SOC unit number.
 *      extn_attribute - (OUT) l3 tunnel global attributes
 *      configured.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tomahawk3_l3_tunnel_term_global_cfg_get(int unit,
                               bcm_l3_tunnel_term_extn_t *extn_attribute)
{
    int rv = BCM_E_UNAVAIL;
    uint32 ing_q_begin;
    uint64 ing_config_64;
    uint16      dev_id;
    uint8       rev_id;

    if (soc_cm_get_id(unit, &dev_id, &rev_id) < 0) {
        return BCM_E_FAIL;
    }
    if (((dev_id & BCM56980_DEVICE_ID_MASK) == BCM56980_DEVICE_ID)
                    && (rev_id == BCM56980_B0_REV_ID)) {
        bcm_l3_tunnel_term_extn_t_init(extn_attribute);
        rv = READ_ING_Q_BEGINr(unit, &ing_q_begin);
        SOC_IF_ERROR_RETURN(rv);
        extn_attribute->dip_mask = soc_reg_field_get(unit, ING_Q_BEGINr,
                                       ing_q_begin, L3_TUNNEL_WAR_IPV4_MASKf);
        rv = READ_ING_CONFIG_64r(unit, &ing_config_64);
        SOC_IF_ERROR_RETURN(rv);
        extn_attribute->extn_enable = soc_reg64_field32_get(unit, ING_CONFIG_64r,
                                       ing_config_64, L3_TUNNEL_WAR_ENABLEf);
        extn_attribute->sip_mask_enable = soc_reg64_field32_get(unit, ING_CONFIG_64r,
                                       ing_config_64, L3_TUNNEL_WAR_SIP_MASK_ENABLEf);
        /* return the remembered values, as hardware -> user visible
         * value is not one-one.
         */
        extn_attribute->tnl_type1 = last_global_cfg_tnl_type1[unit];
        extn_attribute->tnl_type2 = last_global_cfg_tnl_type2[unit];
    }
    return rv;
}
#endif
#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_th3_tunnel_reinit
 * Purpose:
 *      Reinitialize tunnel warm boot state
 * Parameters:
 *      unit     - (IN)  BCM device number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th3_tunnel_reinit(int unit, uint8 **tnl_scache)
{
    uint16      dev_id;
    uint8       rev_id;

    if (soc_cm_get_id(unit, &dev_id, &rev_id) < 0) {
        return BCM_E_FAIL;
    }
    if (((dev_id & BCM56980_DEVICE_ID_MASK) == BCM56980_DEVICE_ID)
                    && (rev_id == BCM56980_B0_REV_ID))
    {
        last_global_cfg_tnl_type1[unit] = *(*tnl_scache)++;
        last_global_cfg_tnl_type2[unit] = *(*tnl_scache)++;
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_th3_tunnel_warm_boot_alloc
 * Purpose:
 *      Reinitialize tunnel warm boot state
 * Parameters:
 *      unit     - (IN)  BCM device number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th3_tunnel_warm_boot_alloc(int unit, uint32 *sz)
{
    uint16      dev_id;
    uint8       rev_id;

    *sz = 0;
    if (soc_cm_get_id(unit, &dev_id, &rev_id) < 0) {
        return BCM_E_FAIL;
    }
    if (((dev_id & BCM56980_DEVICE_ID_MASK) == BCM56980_DEVICE_ID)
                    && (rev_id == BCM56980_B0_REV_ID))
    {
        *sz = 2 * sizeof(bcm_tunnel_type_t);
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_th3_tunnel_sync
 * Purpose:
 *      Reinitialize tunnel warm boot state
 * Parameters:
 *      unit     - (IN)  BCM device number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th3_tunnel_sync(int unit, uint8 **tnl_scache)
{
    uint16      dev_id;
    uint8       rev_id;

    if (soc_cm_get_id(unit, &dev_id, &rev_id) < 0) {
        return BCM_E_FAIL;
    }
    if (((dev_id & BCM56980_DEVICE_ID_MASK) == BCM56980_DEVICE_ID)
                    && (rev_id == BCM56980_B0_REV_ID))
    {
        *(*tnl_scache)++ = last_global_cfg_tnl_type1[unit];
        *(*tnl_scache)++ = last_global_cfg_tnl_type2[unit];
    }
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif

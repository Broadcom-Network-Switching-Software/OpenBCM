/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        lpm.c
 * Purpose:     Maverick2 flex LPM table routines
 * Requires:
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>

#if defined(BCM_MAVERICK2_SUPPORT) && defined(INCLUDE_L3)

#include <soc/lpm.h>
#include <soc/format.h>
#include <soc/esw/flow_db.h>

static
int
_ipmask2pfx(uint32 ipv4m, int *mask_len)
{
    *mask_len = 0;
    while (ipv4m & (1 << 31)) {
        *mask_len += 1;
        ipv4m <<= 1;
    }

    return ((ipv4m) ? SOC_E_PARAM : SOC_E_NONE);
}
/*
 * Function:
 *      soc_mv2_lpm_flex_ip4entry0_to_1
 * Purpose:
 *      copy half-wide flex entry from src to dest.
 *      src and dst can be same.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      src       - (IN)source buffer
 *      dest      - (IN)destination buffer
 *      copy_hit  - (IN)copy hit bit as well
 * Returns:
 *      BCM_E_XXX
 */
int
soc_mv2_lpm_flex_ip4entry0_to_1(int u, void *src, void *dst, uint32 key_type, int copy_hit)
{
    uint32      ipv4a;
    soc_mem_t   view_id;
    uint32      ipv4_pdd[SOC_MAX_MEM_WORDS];

    SOC_IF_ERROR_RETURN(
        soc_flow_db_mem_to_view_id_get(u,
                    L3_DEFIPm,
                    key_type,
                    SOC_FLOW_DB_DATA_TYPE_INVALID,
                    0,
                    NULL,
                    (uint32*) &view_id));

    ipv4a = soc_mem_field32_get(u, view_id, src, VALID0f);
    soc_mem_field32_set(u, view_id, dst, VALID1f, ipv4a);

    ipv4a = soc_mem_field32_get(u, view_id, src, KEY_MODE0f);
    soc_mem_field32_set(u, view_id, dst, KEY_MODE0f, ipv4a);

    if (SOC_MEM_FIELD_VALID(u, view_id, KEY_MODE_MASK0f)) {
        ipv4a = soc_mem_field32_get(u, view_id, src, KEY_MODE_MASK0f);
        soc_mem_field32_set(u, view_id, dst, KEY_MODE_MASK1f, ipv4a);
    }

    ipv4a = soc_mem_field32_get(u, view_id, src, IP_ADDR0f);
    soc_mem_field32_set(u, view_id, dst, IP_ADDR1f, ipv4a);

    ipv4a = soc_mem_field32_get(u, view_id, src, IP_ADDR_MASK0f);
    soc_mem_field32_set(u, view_id, dst, IP_ADDR_MASK1f, ipv4a);

    if (SOC_MEM_FIELD_VALID(u, view_id, VRF_ID_0f)) {
        ipv4a = soc_mem_field32_get(u, view_id, src, VRF_ID_0f);
        soc_mem_field32_set(u, view_id, dst, VRF_ID_1f, ipv4a);
        ipv4a = soc_mem_field32_get(u, view_id, src, VRF_ID_MASK0f);
        soc_mem_field32_set(u, view_id, dst, VRF_ID_MASK1f, ipv4a);
    }

    if (SOC_MEM_FIELD_VALID(u, view_id, ENTRY_TYPE0f)) {
        ipv4a = soc_mem_field32_get(u, view_id, src, ENTRY_TYPE0f);
        soc_mem_field32_set(u, view_id, dst, ENTRY_TYPE1f, ipv4a);
        ipv4a = soc_mem_field32_get(u, view_id, src, ENTRY_TYPE_MASK0f);
        soc_mem_field32_set(u, view_id, dst, ENTRY_TYPE_MASK1f, ipv4a);
    }

    soc_mem_field_get(u, L3_DEFIPm, src, POLICY_DATA0f, ipv4_pdd);
    soc_mem_field_set(u, L3_DEFIPm, dst, POLICY_DATA1f, ipv4_pdd);

    if (copy_hit) {
        ipv4a = soc_mem_field32_get(u, view_id, src, HIT0f);
        soc_mem_field32_set(u, view_id, dst, HIT1f, ipv4a);
    }

    return(SOC_E_NONE);
}
/*
 * Function:
 *      soc_mv2_lpm_flex_vrf_get
 * Purpose:
 *      Service routine used to translate hw flex entry specific vrf id to API format.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      lpm_entry - (IN)Route info buffer from hw.
 *      view_id   - (IN)View-id for the flex entry
 *      vrf_id    - (OUT)Virtual router id.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
soc_mv2_lpm_flex_vrf_get(int unit, void *lpm_entry, uint32 view_id, int *vrf)
{
    int vrf_id;

    /* Get Virtual Router id if supported. */
    if (SOC_MEM_FIELD_VALID(unit, view_id, VRF_ID_MASK0f)){
        vrf_id = soc_mem_field32_get(unit, view_id, lpm_entry, VRF_ID_0f);

        /* Special vrf's handling. */
        if (soc_mem_field32_get(unit, view_id, lpm_entry, VRF_ID_MASK0f)) {
            *vrf = vrf_id;
        } else if (SOC_VRF_MAX(unit) == vrf_id) {
            *vrf = SOC_L3_VRF_GLOBAL;
        } else {
            *vrf = SOC_L3_VRF_OVERRIDE;
            if (SOC_MEM_FIELD_VALID(unit, view_id, FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETf)) {
                uint32 action_set = soc_mem_field32_get(unit, view_id, lpm_entry,
                                                FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETf);
                if (soc_format_field32_get(unit, FORWARDING_3_MISC_ATTRIBUTES_ACTION_SETfmt,
                                           &action_set, GLOBAL_ROUTEf)) {
                    *vrf = SOC_L3_VRF_GLOBAL;
                }
            }
        }
    } else {
        /* No vrf support on this device. */
        *vrf = SOC_L3_VRF_DEFAULT;
    }
    return (SOC_E_NONE);
}
/*
 * Function:
 *      soc_mv2_lpm_flex_prefix_length_get
 * Purpose:
 *      soc_mv2_lpm_flex_prefix_length_get (Extract vrf weighted  prefix lenght from the
 *      hw entry based on ip, mask & vrf)
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      entry     - (IN)Route info buffer from hw.
 *      pfx_len   - (OUT)Prefix len
 * Returns:
 *      BCM_E_XXX
 */
int
soc_mv2_lpm_flex_prefix_length_get(int u, void *entry, int *ipv6, int *vrf_id, int *pfx, uint32 key_type)
{
    int         rv;
    uint32      ipv4a;
    soc_mem_t   view_id;

    if (pfx == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(
        soc_flow_db_mem_to_view_id_get(u,
                    L3_DEFIPm,
                    key_type,
                    SOC_FLOW_DB_DATA_TYPE_INVALID,
                    0,
                    NULL,
                    (uint32*) &view_id));

    *ipv6 = soc_mem_field32_get(u, view_id, entry, KEY_MODE0f);

    if (*ipv6) {
        ipv4a = soc_mem_field32_get(u, view_id, entry, IP_ADDR_MASK0f);
        if ((rv = _ipmask2pfx(ipv4a, pfx)) < 0) {
            return(rv);
        }
        if (SOC_MEM_FIELD_VALID(u, view_id, IP_ADDR_MASK1f)) {
            ipv4a = soc_mem_field32_get(u, view_id, entry, IP_ADDR_MASK1f);
            if (*pfx) {
                if (ipv4a != 0xffffffff)  {
                    return(SOC_E_PARAM);
                }
                *pfx += 32;
            } else {
                if ((rv = _ipmask2pfx(ipv4a, pfx)) < 0) {
                    return(rv);
                }
            }
        }
    } else {
        ipv4a = soc_mem_field32_get(u, view_id, entry, IP_ADDR_MASK0f);
        if ((rv = _ipmask2pfx(ipv4a, pfx)) < 0) {
            return(rv);
        }
    }

    SOC_IF_ERROR_RETURN(soc_mv2_lpm_flex_vrf_get(u, entry, view_id, vrf_id));

    return (SOC_E_NONE);
}
#endif

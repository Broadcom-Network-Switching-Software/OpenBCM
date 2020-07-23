/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    vlan.c
 * Purpose: Manages VLAN virtual port functions
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>
#if defined(BCM_ENDURO_SUPPORT) && defined(INCLUDE_L3) 

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/hash.h>

#include <bcm/error.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>

#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/enduro.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/ipmc.h>

typedef struct _bcm_enduro_vlan_vp_info_s {
    bcm_vlan_port_match_t criteria;
    uint32 flags;
    bcm_vlan_t match_vlan;
    bcm_vlan_t match_inner_vlan;
    bcm_vlan_t egress_vlan;
    bcm_gport_t port;
} _bcm_enduro_vlan_vp_info_t;

/*
 * Software book keeping for VLAN virtual related information
 */
typedef struct _bcm_enduro_vlan_virtual_bookkeeping_s {
    int vlan_virtual_initialized; /* Flag to check initialized status */
    sal_mutex_t vlan_virtual_mutex; /* VLAN virtual module lock */
    _bcm_enduro_vlan_vp_info_t *port_info; /* VP state */
} _bcm_enduro_vlan_virtual_bookkeeping_t;

STATIC _bcm_enduro_vlan_virtual_bookkeeping_t _bcm_enduro_vlan_virtual_bk_info[BCM_MAX_NUM_UNITS];

#define VLAN_VIRTUAL_INFO(unit) (&_bcm_enduro_vlan_virtual_bk_info[unit])

#define VLAN_VIRTUAL_INIT(unit)                                   \
    do {                                                          \
        if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {          \
            return BCM_E_UNIT;                                    \
        }                                                         \
        if (!VLAN_VIRTUAL_INFO(unit)->vlan_virtual_initialized) { \
            return BCM_E_INIT;                                    \
        }                                                         \
    } while (0)

#define VLAN_VIRTUAL_LOCK(unit) \
        sal_mutex_take(VLAN_VIRTUAL_INFO(unit)->vlan_virtual_mutex, sal_mutex_FOREVER);

#define VLAN_VIRTUAL_UNLOCK(unit) \
        sal_mutex_give(VLAN_VIRTUAL_INFO(unit)->vlan_virtual_mutex);

#define VLAN_VP_INFO(unit, vp) (&VLAN_VIRTUAL_INFO(unit)->port_info[vp])

/*
 * Function:
 *      _bcm_enduro_vlan_vp_port_cnt_update
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
_bcm_enduro_vlan_vp_port_cnt_update(int unit, bcm_gport_t gport,
        int vp, int incr)
{
    int mod_out, port_out, tgid_out, id_out;
    int idx;
    int mod_local;
    _bcm_port_info_t *port_info;
    int local_member_count;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    uint32 port_flags;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, &mod_out, 
                                &port_out, &tgid_out, &id_out));
    if (-1 != id_out) {
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
            if (incr) {
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
        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod_out, &mod_local));
        if (mod_local) {
            if (soc_feature(unit, soc_feature_sysport_remap)) { 
                BCM_XLATE_SYSPORT_S2P(unit, &port_out); 
            }
            _bcm_port_info_access(unit, port_out, &port_info);
            if (incr) {
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
 *      _bcm_enduro_vlan_virtual_free_resources
 * Purpose:
 *      Free all allocated tables and memory
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_enduro_vlan_virtual_free_resources(int unit)
{
    if (VLAN_VIRTUAL_INFO(unit)->port_info) {
        sal_free(VLAN_VIRTUAL_INFO(unit)->port_info);
        VLAN_VIRTUAL_INFO(unit)->port_info = NULL;
    }
    if (VLAN_VIRTUAL_INFO(unit)->vlan_virtual_mutex) {
        sal_mutex_destroy(VLAN_VIRTUAL_INFO(unit)->vlan_virtual_mutex);
        VLAN_VIRTUAL_INFO(unit)->vlan_virtual_mutex = NULL;
    } 
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      bcm_enduro_vlan_virtual_reinit
 * Purpose:
 *      Warm boot recovery for the VLAN virtual software module
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_enduro_vlan_virtual_reinit(int unit)
{
    int rv = BCM_E_NONE;
    int stable_size;
    uint8 *vlan_xlate_buf = NULL;
    vlan_xlate_entry_t *vt_entry;
    int i, index_min, index_max;
    uint32 key_type, vp, nh_index, trunk_bit;
    source_vp_entry_t svp_entry;
    ing_dvp_table_entry_t dvp_entry;
    ing_l3_next_hop_entry_t ing_nh_entry;
    wlan_svp_table_entry_t wlan_svp_entry;
    bcm_trunk_t tgid;
    bcm_module_t modid, mod_out;
    bcm_port_t port_num, port_out;
    uint32 profile_idx;
    ing_vlan_tag_action_profile_entry_t ing_profile_entry;
    _bcm_vp_info_t vp_info;
    
    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeVlan;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Recover VLAN virtual ports from VLAN_XLATE table */

    vlan_xlate_buf = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, VLAN_XLATEm), "VLAN_XLATE buffer");
    if (NULL == vlan_xlate_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    index_min = soc_mem_index_min(unit, VLAN_XLATEm);
    index_max = soc_mem_index_max(unit, VLAN_XLATEm);
    rv = soc_mem_read_range(unit, VLAN_XLATEm, MEM_BLOCK_ANY,
            index_min, index_max, vlan_xlate_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    for (i = index_min; i <= index_max; i++) {
        vt_entry = soc_mem_table_idx_to_pointer
            (unit, VLAN_XLATEm, vlan_xlate_entry_t *, 
             vlan_xlate_buf, i);

        if (soc_VLAN_XLATEm_field32_get(unit, vt_entry, VALIDf) == 0) {
            continue;
        }

        key_type = soc_VLAN_XLATEm_field32_get(unit, vt_entry, KEY_TYPEf);
        if ((key_type != TR_VLXLT_HASH_KEY_TYPE_OVID) &&
                (key_type != TR_VLXLT_HASH_KEY_TYPE_IVID_OVID) &&
                (key_type != TR_VLXLT_HASH_KEY_TYPE_OTAG)) {
            continue;
        }

        if (soc_VLAN_XLATEm_field32_get(unit, vt_entry, MPLS_ACTIONf) != 1) {
            continue;
        }

        vp = soc_VLAN_XLATEm_field32_get(unit, vt_entry, SOURCE_VPf);
        if ((stable_size == 0) || SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
            /* Determine if VP is a VLAN VP by process of elimination.
             * Can rule out NIV VP since NIV VPs have different VIF key
             * types in VLAN_XLATEm.
             */

            rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            /* Rule out MPLS and MiM VPs, for which ENTRY_TYPE = 1 */
            if (soc_SOURCE_VPm_field32_get(unit, &svp_entry, ENTRY_TYPEf) != 3) {
                continue;
            }

            rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
                    NEXT_HOP_INDEXf);
            rv = READ_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, nh_index,
                    &ing_nh_entry);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            /* Rule out Subport VPs, for which ENTRY_TYPE = 3 */
            if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry,
                        ENTRY_TYPEf) != 2) {
                continue;
            }

            /* Rule out WLAN vp */
            if (SOC_MEM_IS_VALID(unit, WLAN_SVP_TABLEm)) {
                if (vp > soc_mem_index_max(unit, WLAN_SVP_TABLEm)) {
                    continue;
                }
                rv = READ_WLAN_SVP_TABLEm(unit, MEM_BLOCK_ANY, vp,
                    &wlan_svp_entry);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
                if (soc_WLAN_SVP_TABLEm_field32_get(unit, &wlan_svp_entry,
                            VALIDf) == 1) { 
                    continue;
                }
            }

            /* At this point, we are sure VP is a VLAN VP. */

            rv = _bcm_vp_used_set(unit, vp, vp_info);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        } else {
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                /* VP bitmap is recovered by virtual_init */
                continue;
            }
        }

        /* Recover VLAN_VP_INFO(unit, vp)->criteria, match_vlan, and
         * match_inner_vlan.
         */
        switch (key_type) {
            case TR_VLXLT_HASH_KEY_TYPE_OVID:
                VLAN_VP_INFO(unit, vp)->criteria =
                    BCM_VLAN_PORT_MATCH_PORT_VLAN;
                VLAN_VP_INFO(unit, vp)->match_vlan = 
                    soc_VLAN_XLATEm_field32_get(unit, vt_entry, OVIDf);
                break;
            case TR_VLXLT_HASH_KEY_TYPE_IVID_OVID:
                VLAN_VP_INFO(unit, vp)->criteria =
                    BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
                VLAN_VP_INFO(unit, vp)->match_vlan = 
                    soc_VLAN_XLATEm_field32_get(unit, vt_entry, OVIDf);
                VLAN_VP_INFO(unit, vp)->match_inner_vlan = 
                    soc_VLAN_XLATEm_field32_get(unit, vt_entry, IVIDf);
                break;
            case TR_VLXLT_HASH_KEY_TYPE_OTAG:
                VLAN_VP_INFO(unit, vp)->criteria =
                    BCM_VLAN_PORT_MATCH_PORT_VLAN16;
                VLAN_VP_INFO(unit, vp)->match_vlan = 
                    soc_VLAN_XLATEm_field32_get(unit, vt_entry, OTAGf);
                break;
            /* coverity[dead_error_begin] */
            default:
                continue;
        }

        /* Recover VLAN_VP_INFO(unit, vp)->port */
        trunk_bit = soc_VLAN_XLATEm_field32_get(unit, vt_entry, Tf);
        if (trunk_bit) {
            tgid = soc_VLAN_XLATEm_field32_get(unit, vt_entry, TGIDf);
            BCM_GPORT_TRUNK_SET(VLAN_VP_INFO(unit, vp)->port, tgid);
        } else {
            modid = soc_VLAN_XLATEm_field32_get(unit, vt_entry, MODULE_IDf);
            port_num = soc_VLAN_XLATEm_field32_get(unit, vt_entry, PORT_NUMf);
            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    modid, port_num, &mod_out, &port_out);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            BCM_GPORT_MODPORT_SET(VLAN_VP_INFO(unit, vp)->port,
                    mod_out, port_out);
        }

        /* Recover VLAN_VP_INFO(unit, vp)->flags */
        profile_idx = soc_VLAN_XLATEm_field32_get(unit, vt_entry,
                                                  TAG_ACTION_PROFILE_PTRf); 
        rv = READ_ING_VLAN_TAG_ACTION_PROFILEm(unit, MEM_BLOCK_ANY,
                profile_idx, &ing_profile_entry);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        if ((soc_ING_VLAN_TAG_ACTION_PROFILEm_field32_get(unit,
                    &ing_profile_entry, DT_OTAG_ACTIONf) ==
                    bcmVlanActionReplace) &&
            (soc_ING_VLAN_TAG_ACTION_PROFILEm_field32_get(unit,
                    &ing_profile_entry, DT_ITAG_ACTIONf) ==
                    bcmVlanActionNone)) {
            VLAN_VP_INFO(unit, vp)->flags |= BCM_VLAN_PORT_INNER_VLAN_PRESERVE;
        }
        if (VLAN_VP_INFO(unit, vp)->criteria ==
                BCM_VLAN_PORT_MATCH_PORT_VLAN16) {
            VLAN_VP_INFO(unit, vp)->flags |= BCM_VLAN_PORT_EGRESS_VLAN16;
        }

        if (stable_size == 0) {
            /* In the Port module, a port's VP count is not recovered in 
             * level 1 Warm Boot.
             */
            rv = _bcm_enduro_vlan_vp_port_cnt_update(unit,
                    VLAN_VP_INFO(unit, vp)->port, vp, TRUE);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }

cleanup:
    if (vlan_xlate_buf) {
        soc_cm_sfree(unit, vlan_xlate_buf);
    }

    if (BCM_FAILURE(rv)) {
        _bcm_enduro_vlan_virtual_free_resources(unit);
    }

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_enduro_vlan_vp_sw_dump
 * Purpose:
 *     Displays VLAN VP information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_enduro_vlan_vp_sw_dump(int unit)
{
    int i, num_vp;

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information VLAN VP - Unit %d\n"), unit));

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    for (i = 0; i < num_vp; i++) {
        if (VLAN_VP_INFO(unit, i)->port == 0) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n  VLAN vp = %d\n"), i));
        LOG_CLI((BSL_META_U(unit,
                            "  Criteria = 0x%x,"), VLAN_VP_INFO(unit, i)->criteria));
        switch (VLAN_VP_INFO(unit, i)->criteria) {
            case BCM_VLAN_PORT_MATCH_PORT_VLAN:
                LOG_CLI((BSL_META_U(unit,
                                    " port plus outer VLAN ID\n")));
                break;
            case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
                LOG_CLI((BSL_META_U(unit,
                                    " port plus outer and inner VLAN IDs\n")));
                break;
            case BCM_VLAN_PORT_MATCH_PORT_VLAN16:
                LOG_CLI((BSL_META_U(unit,
                                    " port plus outer VLAN tag\n")));
                break;
            default:
                LOG_CLI((BSL_META_U(unit,
                                    " \n")));
        }
        LOG_CLI((BSL_META_U(unit,
                            "  Flags = 0x%x\n"), VLAN_VP_INFO(unit, i)->flags));
        LOG_CLI((BSL_META_U(unit,
                            "  Match VLAN = 0x%x\n"), VLAN_VP_INFO(unit, i)->match_vlan));
        LOG_CLI((BSL_META_U(unit,
                            "  Match Inner VLAN = 0x%x\n"),
                 VLAN_VP_INFO(unit, i)->match_inner_vlan));
        LOG_CLI((BSL_META_U(unit,
                            "  Port = 0x%x\n"), VLAN_VP_INFO(unit, i)->port));
    }

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/*
 * Function:
 *      bcm_enduro_vlan_virtual_init
 * Purpose:
 *      Initialize the VLAN virtual port module.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_enduro_vlan_virtual_init(int unit)
{
    int num_vp;

    if (VLAN_VIRTUAL_INFO(unit)->vlan_virtual_initialized) {
        bcm_enduro_vlan_virtual_detach(unit);
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    if (NULL == VLAN_VIRTUAL_INFO(unit)->port_info) {
        VLAN_VIRTUAL_INFO(unit)->port_info =
            sal_alloc(sizeof(_bcm_enduro_vlan_vp_info_t) * num_vp, "vlan_vp_info");
        if (NULL == VLAN_VIRTUAL_INFO(unit)->port_info) {
            _bcm_enduro_vlan_virtual_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(VLAN_VIRTUAL_INFO(unit)->port_info, 0,
            sizeof(_bcm_enduro_vlan_vp_info_t) * num_vp);

    if (NULL == VLAN_VIRTUAL_INFO(unit)->vlan_virtual_mutex) {
        VLAN_VIRTUAL_INFO(unit)->vlan_virtual_mutex =
            sal_mutex_create("vlan virtual mutex");
        if (NULL == VLAN_VIRTUAL_INFO(unit)->vlan_virtual_mutex) {
            _bcm_enduro_vlan_virtual_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }

    VLAN_VIRTUAL_INFO(unit)->vlan_virtual_initialized = 1;

    /* Warm boot recovery of VLAN_VP_INFO depends on the completion
     * of _bcm_virtual_init, but _bcm_virtual_init is after the 
     * VLAN module in the init sequence. Hence, warm boot recovery
     * of VLAN_VP_INFO is moved to end of _bcm_virtual_init.
     */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_enduro_vlan_virtual_detach
 * Purpose:
 *      Detach the VLAN virtual port module.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_enduro_vlan_virtual_detach(int unit)
{
    _bcm_enduro_vlan_virtual_free_resources(unit);

    VLAN_VIRTUAL_INFO(unit)->vlan_virtual_initialized = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_enduro_vlan_vp_nh_info_set
 * Purpose:
 *      Get a next hop index and configure next hop tables.
 * Parameters:
 *      unit       - (IN) SOC unit number. 
 *      vlan_vp    - (IN) Pointer to VLAN virtual port structure. 
 *      vp         - (IN) Virtual port number. 
 *      drop       - (IN) Drop indication. 
 *      nh_index   - (IN/OUT) Next hop index. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_enduro_vlan_vp_nh_info_set(int unit, bcm_vlan_port_t *vlan_vp, int vp,
        int drop, int *nh_index)
{
    int rv;
    uint32 nh_flags;
    bcm_l3_egress_t nh_info;
    egr_l3_next_hop_entry_t egr_nh;
    uint8 egr_nh_entry_type;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int id;
    int ing_nh_port;
    int ing_nh_module;
    int ing_nh_trunk;
    ing_l3_next_hop_entry_t ing_nh;
    initial_ing_l3_next_hop_entry_t initial_ing_nh;

    /* Get a next hop index */

    if (vlan_vp->flags & BCM_VLAN_PORT_REPLACE) {
        if ((*nh_index > soc_mem_index_max(unit, EGR_L3_NEXT_HOPm)) ||
                (*nh_index < soc_mem_index_min(unit, EGR_L3_NEXT_HOPm)))  {
            return BCM_E_PARAM;
        }
    } else {
        /*
         * Allocate a next-hop entry. By calling bcm_xgs3_nh_add()
         * with _BCM_L3_SHR_WRITE_DISABLE flag, a next-hop index is
         * allocated but nothing is written to hardware. The "nh_info"
         * in this case is not used, so just set to all zeros.
         */
        bcm_l3_egress_t_init(&nh_info);

        nh_flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE;
        rv = bcm_xgs3_nh_add(unit, nh_flags, &nh_info, nh_index);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Write EGR_L3_NEXT_HOP entry */

    if (vlan_vp->flags & BCM_VLAN_PORT_REPLACE) {
        /* Read the existing egress next_hop entry */
        rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                *nh_index, &egr_nh);
        BCM_IF_ERROR_RETURN(rv);

        /* Be sure that the existing entry is programmed to SD-tag */
        egr_nh_entry_type = 
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);
        if (egr_nh_entry_type != 0x2) { /* != SD-tag */
            return BCM_E_PARAM;
        }
    } else {
        egr_nh_entry_type = 0x2; /* SD-tag */
    }

    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            ENTRY_TYPEf, egr_nh_entry_type);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            SD_TAG__DVPf, vp);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
            SD_TAG__HG_HDR_SELf, 1);
    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
            MEM_BLOCK_ALL, *nh_index, &egr_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Resolve gport */

    rv = _bcm_esw_gport_resolve(unit, vlan_vp->port, &mod_out, 
            &port_out, &trunk_id, &id);
    if (rv < 0) {
        goto cleanup;
    }

    ing_nh_port = -1;
    ing_nh_module = -1;
    ing_nh_trunk = -1;

    if (BCM_GPORT_IS_TRUNK(vlan_vp->port)) {
        ing_nh_module = -1;
        ing_nh_port = -1;
        ing_nh_trunk = trunk_id;
    } else {
        ing_nh_module = mod_out;
        ing_nh_port = port_out;
        ing_nh_trunk = -1;
    }

    /* Write ING_L3_NEXT_HOP entry */

    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));

    if (ing_nh_trunk == -1) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                &ing_nh, PORT_NUMf, ing_nh_port);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                &ing_nh, MODULE_IDf, ing_nh_module);
    } else {    
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                &ing_nh, Tf, 1);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                &ing_nh, TGIDf, ing_nh_trunk);
    }

    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, drop);
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
            &ing_nh, ENTRY_TYPEf, 0x2); /* L2 DVP */
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
            &ing_nh, MTU_SIZEf, 0x3fff);
    rv = soc_mem_write (unit, ING_L3_NEXT_HOPm,
            MEM_BLOCK_ALL, *nh_index, &ing_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Write INITIAL_ING_L3_NEXT_HOP entry */

    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    if (ing_nh_trunk == -1) {
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                &initial_ing_nh, PORT_NUMf, ing_nh_port);
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                &initial_ing_nh, MODULE_IDf, ing_nh_module);
    } else {
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                &initial_ing_nh, Tf, 1);
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                &initial_ing_nh, TGIDf, ing_nh_trunk);
    }
    rv = soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
            MEM_BLOCK_ALL, *nh_index, &initial_ing_nh);
    if (rv < 0) {
        goto cleanup;
    }

    return rv;

cleanup:
    if (!(vlan_vp->flags & BCM_VLAN_PORT_REPLACE)) {
        (void) bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, *nh_index);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_enduro_vlan_vp_nh_info_delete
 * Purpose:
 *      Free next hop index and clear next hop tables.
 * Parameters:
 *      unit     - (IN) SOC unit number. 
 *      nh_index - (IN) Next hop index. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_enduro_vlan_vp_nh_info_delete(int unit, int nh_index)
{
    egr_l3_next_hop_entry_t egr_nh;
    ing_l3_next_hop_entry_t ing_nh;
    initial_ing_l3_next_hop_entry_t initial_ing_nh;

    /* Clear EGR_L3_NEXT_HOP entry */
    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &egr_nh));

    /* Clear ING_L3_NEXT_HOP entry */
    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN(soc_mem_write (unit, ING_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &ing_nh));

    /* Clear INITIAL_ING_L3_NEXT_HOP entry */
    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &initial_ing_nh));

    /* Free the next-hop index. */
    BCM_IF_ERROR_RETURN
        (bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, nh_index));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_enduro_vlan_vp_match_add
 * Purpose:
 *      Add match criteria for VLAN VP.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan_vp - (IN) Pointer to VLAN virtual port structure. 
 *      vp - (IN) Virtual port number.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_enduro_vlan_vp_match_add(int unit, bcm_vlan_port_t *vlan_vp, int vp)
{
    vlan_xlate_entry_t vent, old_vent;
    int key_type = 0;
    bcm_vlan_action_set_t action;
    uint32 profile_idx;
    int rv;

    if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_NONE) {
       return BCM_E_NONE;
    }

    if (!((vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN) ||
          (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED) ||
          (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN16))) {
        return BCM_E_PARAM;
    }

    if ((vlan_vp->egress_vlan > BCM_VLAN_MAX) ||
        (vlan_vp->egress_inner_vlan > BCM_VLAN_MAX)) {
        return BCM_E_PARAM;
    }

    sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));

    soc_VLAN_XLATEm_field32_set(unit, &vent, VALIDf, 1);

    if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN) {
        key_type = bcmVlanTranslateKeyPortOuter;
    } else if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED) {
        key_type = bcmVlanTranslateKeyPortDouble;
    } else if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN16) {
        key_type = bcmVlanTranslateKeyPortOuterTag;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_translate_entry_assemble(unit, &vent,
                                                vlan_vp->port,
                                                key_type,
                                                vlan_vp->match_inner_vlan,
                                                vlan_vp->match_vlan));

    soc_VLAN_XLATEm_field32_set(unit, &vent, MPLS_ACTIONf, 0x1); /* SVP */
    soc_VLAN_XLATEm_field32_set(unit, &vent, SOURCE_VPf, vp);
    soc_VLAN_XLATEm_field32_set(unit, &vent, NEW_OVIDf,
                                vlan_vp->egress_vlan);
    soc_VLAN_XLATEm_field32_set(unit, &vent, NEW_IVIDf,
                                vlan_vp->egress_inner_vlan);

    bcm_vlan_action_set_t_init(&action);

    /* For double tagged packets */
    if (vlan_vp->flags & BCM_VLAN_PORT_INNER_VLAN_PRESERVE) {
        action.dt_outer = bcmVlanActionReplace;
        action.dt_outer_prio = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionNone;
        action.dt_inner_prio = bcmVlanActionNone;
    } else {
        /* Strip inner VLAN */
        action.dt_outer = bcmVlanActionReplace;
        action.dt_outer_prio = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.dt_inner_prio = bcmVlanActionDelete;
    }

    if (vlan_vp->flags & BCM_VLAN_PORT_INNER_VLAN_ADD) {
        action.ot_inner = bcmVlanActionAdd;
    } else {
        action.ot_inner = bcmVlanActionNone;
    }

    action.ot_outer = bcmVlanActionReplace;
    action.ot_outer_prio = bcmVlanActionReplace;

    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_add(unit, &action, &profile_idx));

    soc_VLAN_XLATEm_field32_set(unit, &vent, TAG_ACTION_PROFILE_PTRf,
                                profile_idx);

    rv = soc_mem_insert_return_old(unit, VLAN_XLATEm, MEM_BLOCK_ALL,
                                   &vent, &old_vent);
    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_VLAN_XLATEm_field32_get(unit, &old_vent,
                                                  TAG_ACTION_PROFILE_PTRf);       
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_enduro_vlan_vp_match_delete
 * Purpose:
 *      Delete match criteria for VLAN VP.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vp - (IN) Virtual port number.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_enduro_vlan_vp_match_delete(int unit, int vp)
{
    vlan_xlate_entry_t vent, old_vent;
    int key_type = 0;
    uint32 profile_idx;
    int rv;

    if (VLAN_VP_INFO(unit, vp)->criteria == BCM_VLAN_PORT_MATCH_NONE) {
        return BCM_E_NONE;
    }

    if (VLAN_VP_INFO(unit, vp)->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN) {
        key_type = bcmVlanTranslateKeyPortOuter;
    } else if (VLAN_VP_INFO(unit, vp)->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED) {
        key_type = bcmVlanTranslateKeyPortDouble;
    } else if (VLAN_VP_INFO(unit, vp)->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN16) {
        key_type = bcmVlanTranslateKeyPortOuterTag;
    } else {
        return BCM_E_INTERNAL;
    }

    sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_translate_entry_assemble(unit, &vent,
                                                VLAN_VP_INFO(unit, vp)->port,
                                                key_type,
                                                VLAN_VP_INFO(unit, vp)->match_inner_vlan,
                                                VLAN_VP_INFO(unit, vp)->match_vlan));

    rv = soc_mem_delete_return_old(unit, VLAN_XLATEm, MEM_BLOCK_ALL, &vent, &old_vent);
    if ((rv == SOC_E_NONE) && soc_VLAN_XLATEm_field32_get(unit, &old_vent, VALIDf)) {
        profile_idx = soc_VLAN_XLATEm_field32_get(unit, &old_vent,
                                                  TAG_ACTION_PROFILE_PTRf);       
        /* Delete the old vlan action profile entry */
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_enduro_vlan_vp_match_get
 * Purpose:
 *      Get match criteria for VLAN VP.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vp - (IN) Virtual port number.
 *      vlan_vp - (OUT) Pointer to VLAN virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_enduro_vlan_vp_match_get(int unit, int vp, bcm_vlan_port_t *vlan_vp)
{
    vlan_xlate_entry_t vent, vent_out;
    int key_type = 0;
    int idx;

    vlan_vp->criteria = VLAN_VP_INFO(unit, vp)->criteria;
    vlan_vp->match_vlan = VLAN_VP_INFO(unit, vp)->match_vlan;
    vlan_vp->match_inner_vlan = VLAN_VP_INFO(unit, vp)->match_inner_vlan;
    vlan_vp->port = VLAN_VP_INFO(unit, vp)->port;

    if (VLAN_VP_INFO(unit, vp)->criteria == BCM_VLAN_PORT_MATCH_NONE) {
        return BCM_E_NONE;
    }

    if (VLAN_VP_INFO(unit, vp)->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN) {
        key_type = bcmVlanTranslateKeyPortOuter;
    } else if (VLAN_VP_INFO(unit, vp)->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED) {
        key_type = bcmVlanTranslateKeyPortDouble;
    } else if (VLAN_VP_INFO(unit, vp)->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN16) {
        key_type = bcmVlanTranslateKeyPortOuterTag;
    } else {
        return BCM_E_INTERNAL;
    }

    sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_translate_entry_assemble(unit, &vent,
                                                VLAN_VP_INFO(unit, vp)->port,
                                                key_type,
                                                VLAN_VP_INFO(unit, vp)->match_inner_vlan,
                                                VLAN_VP_INFO(unit, vp)->match_vlan));

    BCM_IF_ERROR_RETURN
        (soc_mem_search(unit, VLAN_XLATEm, MEM_BLOCK_ALL,
                        &idx, &vent, &vent_out, 0));
    vlan_vp->egress_vlan = soc_VLAN_XLATEm_field32_get(unit, &vent_out,
                                                       NEW_OVIDf);
    vlan_vp->egress_inner_vlan = soc_VLAN_XLATEm_field32_get(unit, &vent_out,
                                                       NEW_IVIDf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_enduro_vlan_vp_create
 * Purpose:
 *      Create a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan_vp - (IN/OUT) Pointer to VLAN virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_enduro_vlan_vp_create(int unit,
                         bcm_vlan_port_t *vlan_vp)
{
    int mode;
    int vp;
    int rv = BCM_E_NONE;
    int num_vp;
    int nh_index = 0;
    ing_dvp_table_entry_t dvp_entry;
    source_vp_entry_t svp_entry;
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;
    _bcm_vp_info_t vp_info;
    
    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeVlan;

    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_egress_mode_get(unit, &mode));
    if (!mode) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 egress mode must be set first\n")));
        return BCM_E_DISABLED;
    }

    if (!(vlan_vp->flags & BCM_VLAN_PORT_REPLACE)) { /* Create new VLAN VP */

        if (vlan_vp->flags & BCM_VLAN_PORT_WITH_ID) {
            if (!BCM_GPORT_IS_VLAN_PORT(vlan_vp->vlan_port_id)) {
                return BCM_E_PARAM;
            }
            vp = BCM_GPORT_VLAN_PORT_ID_GET(vlan_vp->vlan_port_id);
            if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
                return BCM_E_PARAM;
            }
            if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                return BCM_E_EXISTS;
            } else {
                rv = _bcm_vp_used_set(unit, vp, vp_info);
                if (rv < 0) {
                    return rv;
                }
            }
        } else {
            /* allocate a new VP index */
            num_vp = soc_mem_index_count(unit, SOURCE_VPm);
            rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm,
                    vp_info, &vp);
            if (rv < 0) {
                return rv;
            }
        }

        /* Configure next hop tables */
        rv = _bcm_enduro_vlan_vp_nh_info_set(unit, vlan_vp, vp, 0,
                                              &nh_index);
        if (rv < 0) {
            goto cleanup;
        }

        /* Configure DVP table */
        sal_memset(&dvp_entry, 0, sizeof(ing_dvp_table_entry_t));
        soc_ING_DVP_TABLEm_field32_set(unit, &dvp_entry, NEXT_HOP_INDEXf,
                                       nh_index);
        rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp_entry);
        if (rv < 0) {
            goto cleanup;
        }

        /* Configure SVP table */
        sal_memset(&svp_entry, 0, sizeof(source_vp_entry_t));
        soc_SOURCE_VPm_field32_set(unit, &svp_entry, ENTRY_TYPEf, 3);

        /* Set the CML */
        rv = _bcm_vp_default_cml_mode_get (unit, 
                           &cml_default_enable, &cml_default_new, 
                           &cml_default_move);
         if (rv < 0) {
             goto cleanup;
         }

        if (cml_default_enable) {
            /* Set the CML to default values */
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_NEWf, cml_default_new);
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_MOVEf, cml_default_move);
        } else {
            /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_NEWf, 0x8);
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_MOVEf, 0x8);
        }

        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry);
        if (rv < 0) {
            goto cleanup;
        }

        /* Configure ingress VLAN translation table */
        rv = _bcm_enduro_vlan_vp_match_add(unit, vlan_vp, vp);
        if (rv < 0) {
            goto cleanup;
        }

        /* Increment port's VP count */
        rv = _bcm_enduro_vlan_vp_port_cnt_update(unit, vlan_vp->port, vp, TRUE);
        if (rv < 0) {
            goto cleanup;
        }

    } else { /* Replace properties of existing VLAN VP */

        if (!(vlan_vp->flags & BCM_VLAN_PORT_WITH_ID)) {
            return BCM_E_PARAM;
        }
        if (!BCM_GPORT_IS_VLAN_PORT(vlan_vp->vlan_port_id)) {
            return BCM_E_PARAM;
        }
        vp = BCM_GPORT_VLAN_PORT_ID_GET(vlan_vp->vlan_port_id);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
            return BCM_E_PARAM;
        }

        /* For existing vlan vp, NH entry already exists */
        BCM_IF_ERROR_RETURN
            (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));

        nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
                NEXT_HOP_INDEXf);

        /* Update existing next hop entries */
        BCM_IF_ERROR_RETURN
            (_bcm_enduro_vlan_vp_nh_info_set(unit, vlan_vp, vp, 0,
                                              &nh_index));

        /* Delete old ingress VLAN translation entry,
         * install new ingress VLAN translation entry
         */
        BCM_IF_ERROR_RETURN
            (_bcm_enduro_vlan_vp_match_delete(unit, vp));

        BCM_IF_ERROR_RETURN
            (_bcm_enduro_vlan_vp_match_add(unit, vlan_vp, vp));

        /* Decrement old port's VP count, increment new port's VP count */
        BCM_IF_ERROR_RETURN
            (_bcm_enduro_vlan_vp_port_cnt_update(unit,
                VLAN_VP_INFO(unit, vp)->port, vp, FALSE));

        BCM_IF_ERROR_RETURN
            (_bcm_enduro_vlan_vp_port_cnt_update(unit,
                vlan_vp->port, vp, TRUE));
    }

    /* Set VLAN VP software state */
    VLAN_VP_INFO(unit, vp)->criteria = vlan_vp->criteria;
    VLAN_VP_INFO(unit, vp)->flags = vlan_vp->flags;
    VLAN_VP_INFO(unit, vp)->match_vlan = vlan_vp->match_vlan;
    VLAN_VP_INFO(unit, vp)->match_inner_vlan = vlan_vp->match_inner_vlan;
    VLAN_VP_INFO(unit, vp)->egress_vlan = vlan_vp->egress_vlan;
    VLAN_VP_INFO(unit, vp)->port = vlan_vp->port;

    BCM_GPORT_VLAN_PORT_ID_SET(vlan_vp->vlan_port_id, vp);
    vlan_vp->encap_id = nh_index;

    return rv;

cleanup:
    if (!(vlan_vp->flags & BCM_VLAN_PORT_REPLACE)) {
        (void) _bcm_vp_free(unit, _bcmVpTypeVlan, 1, vp);
        _bcm_enduro_vlan_vp_nh_info_delete(unit, nh_index);

        sal_memset(&dvp_entry, 0, sizeof(ing_dvp_table_entry_t));
        (void)WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp_entry);

        sal_memset(&svp_entry, 0, sizeof(source_vp_entry_t));
        (void)WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry);

        (void) _bcm_enduro_vlan_vp_match_delete(unit, vp);
    }

    return rv;
}

/*
 * Function:
 *      bcm_enduro_vlan_vp_create
 * Purpose:
 *      Create a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan_vp - (IN/OUT) Pointer to VLAN virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_enduro_vlan_vp_create(int unit,
                         bcm_vlan_port_t *vlan_vp)
{
    int rv;

    VLAN_VIRTUAL_INIT(unit);

    VLAN_VIRTUAL_LOCK(unit);

    rv = _bcm_enduro_vlan_vp_create(unit, vlan_vp);

    VLAN_VIRTUAL_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_enduro_vlan_vp_destroy
 * Purpose:
 *      Destroy a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) VLAN VP GPORT ID.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_enduro_vlan_vp_destroy(int unit, bcm_gport_t gport)
{
    int vp;
    source_vp_entry_t svp_entry;
    int nh_index;
    ing_dvp_table_entry_t dvp_entry;

    if (!BCM_GPORT_IS_VLAN_PORT(gport)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }

    /* Delete ingress VLAN translation entry */
    BCM_IF_ERROR_RETURN
        (_bcm_enduro_vlan_vp_match_delete(unit, vp));

    /* Clear SVP entry */
    sal_memset(&svp_entry, 0, sizeof(source_vp_entry_t));
    BCM_IF_ERROR_RETURN
        (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));

    /* Clear DVP entry */
    BCM_IF_ERROR_RETURN
        (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry, NEXT_HOP_INDEXf);

    sal_memset(&dvp_entry, 0, sizeof(ing_dvp_table_entry_t));
    BCM_IF_ERROR_RETURN
        (WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp_entry));

    /* Clear next hop entries and free next hop index */
    BCM_IF_ERROR_RETURN
        (_bcm_enduro_vlan_vp_nh_info_delete(unit, nh_index));

    /* Decrement port's VP count */
    BCM_IF_ERROR_RETURN
        (_bcm_enduro_vlan_vp_port_cnt_update(unit,
                                              VLAN_VP_INFO(unit, vp)->port,
                                              vp, FALSE));
    /* Free VP */
    BCM_IF_ERROR_RETURN
        (_bcm_vp_free(unit, _bcmVpTypeVlan, 1, vp));

    /* Clear VLAN VP software state */
    sal_memset(VLAN_VP_INFO(unit, vp), 0, sizeof(_bcm_enduro_vlan_vp_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_enduro_vlan_vp_destroy
 * Purpose:
 *      Destroy a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) VLAN VP GPORT ID.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_enduro_vlan_vp_destroy(int unit, bcm_gport_t gport)
{
    int rv;

    VLAN_VIRTUAL_INIT(unit);

    VLAN_VIRTUAL_LOCK(unit);

    rv = _bcm_enduro_vlan_vp_destroy(unit, gport);

    VLAN_VIRTUAL_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_enduro_vlan_vp_find
 * Purpose:
 *      Get VLAN virtual port info.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan_vp - (IN/OUT) Pointer to VLAN virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_enduro_vlan_vp_find(int unit, bcm_vlan_port_t *vlan_vp)
{
    int vp;
    ing_dvp_table_entry_t dvp_entry;
    int nh_index;

    if (!BCM_GPORT_IS_VLAN_PORT(vlan_vp->vlan_port_id)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_VLAN_PORT_ID_GET(vlan_vp->vlan_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }

    bcm_vlan_port_t_init(vlan_vp);

    BCM_IF_ERROR_RETURN(_bcm_enduro_vlan_vp_match_get(unit, vp, vlan_vp));

    vlan_vp->flags = VLAN_VP_INFO(unit, vp)->flags;

    BCM_IF_ERROR_RETURN
        (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry, NEXT_HOP_INDEXf);
    vlan_vp->encap_id = nh_index;

    BCM_GPORT_VLAN_PORT_ID_SET(vlan_vp->vlan_port_id, vp);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_enduro_vlan_vp_find
 * Purpose:
 *      Get VLAN virtual port info.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan_vp - (IN/OUT) Pointer to VLAN virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_enduro_vlan_vp_find(int unit, bcm_vlan_port_t *vlan_vp)
{
    VLAN_VIRTUAL_INIT(unit);
    return _bcm_enduro_vlan_vp_find(unit, vlan_vp);
}

/*
 * Function:
 *      _bcm_enduro_vlan_vp_is_local
 * Purpose:
 *      Determine if VLAN VP resides on a local port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vp   - (IN) Virtual port number.
 *      vp_is_local - (OUT) Indicates if VLAN VP is local.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_enduro_vlan_vp_is_local(int unit, int vp, int *vp_is_local)
{
    bcm_gport_t gport;
    bcm_trunk_t trunk_id;
    int rv = BCM_E_NONE;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    int id;
    int modid_local;
    int local_member_count;

    *vp_is_local = 0;

    if (vp < 0) {
        return BCM_E_PARAM;
    }

    gport = VLAN_VP_INFO(unit, vp)->port;

    if (BCM_GPORT_IS_TRUNK(gport)) {
        trunk_id = BCM_GPORT_TRUNK_GET(gport);
        rv = _bcm_trunk_id_validate(unit, trunk_id);
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PORT);
        }
        rv = _bcm_esw_trunk_local_members_get(unit, trunk_id, 0, NULL,
                &local_member_count);
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PORT);
        }   

        if (local_member_count > 0) {
            *vp_is_local = 1;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, gport, &mod_out, &port_out,
                                    &trunk_id, &id)); 

        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod_out, &modid_local));

        *vp_is_local = modid_local;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_enduro_vlan_vp_untagged_add
 * Purpose:
 *      Set VLAN VP tagging/untagging status by adding
 *      a (VLAN VP, VLAN) egress VLAN translation entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN ID. 
 *      vp   - (IN) Virtual port number.
 *      flags - (IN) Untagging indication.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_enduro_vlan_vp_untagged_add(int unit, bcm_vlan_t vlan, int vp,
                                  int flags)
{
    egr_vlan_xlate_entry_t vent, old_vent;
    bcm_vlan_action_set_t action;
    uint32 profile_idx;
    int rv;

    sal_memset(&vent, 0, sizeof(egr_vlan_xlate_entry_t));

    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, VALIDf, 1);

    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, ENTRY_TYPEf, 1);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, DVPf, vp);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, vlan);

    if (VLAN_VP_INFO(unit, vp)->flags & BCM_VLAN_PORT_EGRESS_VLAN16) {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, NEW_OTAG_VPTAG_SELf, 1);
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, NEW_OTAG_VPTAGf,
                                        VLAN_VP_INFO(unit, vp)->match_vlan);
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, NEW_IVIDf,
                VLAN_VP_INFO(unit, vp)->egress_vlan & 0xfff);
    } else {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, NEW_OTAG_VPTAG_SELf, 0);
        /* egress_vlan in vlan_vp structure is used as switching vlan */
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, NEW_OVIDf,
                VLAN_VP_INFO(unit, vp)->match_vlan & 0xfff);
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, NEW_IVIDf,
                VLAN_VP_INFO(unit, vp)->egress_vlan & 0xfff);
    }

    bcm_vlan_action_set_t_init(&action);

    action.dt_outer = bcmVlanActionReplace;
    action.ot_outer = bcmVlanActionReplace;

    if (flags & BCM_VLAN_PORT_UNTAGGED) {
        action.dt_inner = bcmVlanActionNone;
        action.ot_inner = bcmVlanActionNone;
    } else {
        action.dt_inner = bcmVlanActionReplace;
        action.ot_inner = bcmVlanActionAdd;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_trx_egr_vlan_action_profile_entry_add(unit, &action, &profile_idx));

    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, TAG_ACTION_PROFILE_PTRf,
                                profile_idx);

    rv = soc_mem_insert_return_old(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL,
                                   &vent, &old_vent);
    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_EGR_VLAN_XLATEm_field32_get(unit, &old_vent,
                                                  TAG_ACTION_PROFILE_PTRf);       
        rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_enduro_vlan_vp_untagged_delete
 * Purpose:
 *      Delete (VLAN VP, VLAN) egress VLAN translation entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN ID. 
 *      vp   - (IN) Virtual port number.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_enduro_vlan_vp_untagged_delete(int unit, bcm_vlan_t vlan, int vp)
{
    egr_vlan_xlate_entry_t vent, old_vent;
    uint32 profile_idx;
    int rv;

    sal_memset(&vent, 0, sizeof(egr_vlan_xlate_entry_t));

    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, ENTRY_TYPEf, 1);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, DVPf, vp);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, vlan);

    rv = soc_mem_delete_return_old(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL,
                                   &vent, &old_vent);
    if ((rv == SOC_E_NONE) &&
            soc_EGR_VLAN_XLATEm_field32_get(unit, &old_vent, VALIDf)) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_EGR_VLAN_XLATEm_field32_get(unit, &old_vent,
                                                  TAG_ACTION_PROFILE_PTRf);       
        rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_enduro_vlan_vp_untagged_delete_all
 * Purpose:
 *      Delete all (VLAN VP, VLAN) egress VLAN translation entries for a
 *      given VLAN.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN ID. 
 *      array_size  - (IN) Number of elements in the virtual port array.
 *      gport_array - (IN) Array of VLAN VP GPORT IDs.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_enduro_vlan_vp_untagged_delete_all(int unit, bcm_vlan_t vlan,
        int array_size, bcm_gport_t *gport_array)
{
    int i;
    egr_vlan_xlate_entry_t vent, old_vent;
    int vp;
    uint32 profile_idx;
    int rv = BCM_E_NONE;

    for (i = 0; i < array_size; i++) {

        sal_memset(&vent, 0, sizeof(egr_vlan_xlate_entry_t));

        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, ENTRY_TYPEf, 1);
        vp = BCM_GPORT_VLAN_PORT_ID_GET(gport_array[i]);
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, DVPf, vp);
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, vlan);

        rv = soc_mem_delete_return_old(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL,
                &vent, &old_vent);
        BCM_IF_ERROR_RETURN(rv);

        if (soc_EGR_VLAN_XLATEm_field32_get(unit, &old_vent, VALIDf)) {
            /* Delete the old vlan translate profile entry */
            profile_idx = soc_EGR_VLAN_XLATEm_field32_get(unit, &old_vent,
                    TAG_ACTION_PROFILE_PTRf);       
            rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx);
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_enduro_vlan_vp_untagged_get
 * Purpose:
 *      Get tagging/untagging status of a VLAN virtual port by
 *      reading the (VLAN VP, VLAN) egress vlan translation entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN to remove virtual port from.
 *      vp   - (IN) Virtual port number.
 *      flags - (OUT) Untagging status of the VLAN virtual port.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_enduro_vlan_vp_untagged_get(int unit, bcm_vlan_t vlan, int vp,
                                 int *flags) 
{
    egr_vlan_xlate_entry_t vent, res_vent;
    int idx;
    uint32 profile_idx;
    int rv;
    bcm_vlan_action_set_t action;

    sal_memset(&vent, 0, sizeof(egr_vlan_xlate_entry_t));

    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, ENTRY_TYPEf, 1);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, DVPf, vp);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, vlan);

    rv = soc_mem_search(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL, &idx,
                                   &vent, &res_vent, 0);
    if ((rv == SOC_E_NONE) &&
            soc_EGR_VLAN_XLATEm_field32_get(unit, &res_vent, VALIDf)) {
        profile_idx = soc_EGR_VLAN_XLATEm_field32_get(unit, &res_vent,
                                                  TAG_ACTION_PROFILE_PTRf);       
        _bcm_trx_egr_vlan_action_profile_entry_get(unit, &action, profile_idx);

        if (bcmVlanActionNone == action.ot_inner) {
            *flags = BCM_VLAN_PORT_UNTAGGED;
        } else {
            *flags = 0;
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_enduro_vlan_vp_update_vlan_pbmp
 * Purpose:
 *      Update VLAN table's port bitmap.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      vlan  - (IN) VLAN ID.
 *      pbmp  - (IN) VLAN port bitmap. 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_enduro_vlan_vp_update_vlan_pbmp(int unit, bcm_vlan_t vlan,
        bcm_pbmp_t *pbmp)
{
    int rv = BCM_E_NONE;
    vlan_tab_entry_t vtab;
    egr_vlan_entry_t egr_vtab;

    soc_mem_lock(unit, VLAN_TABm);
    
    rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, vlan, &vtab);
    if (rv < 0) {
        soc_mem_unlock(unit, VLAN_TABm);
        return rv;
    }

    soc_mem_pbmp_field_set(unit, VLAN_TABm, &vtab, PORT_BITMAPf, pbmp);
    rv = soc_mem_write(unit, VLAN_TABm, MEM_BLOCK_ALL, vlan, &vtab);
    if (rv < 0) {
        soc_mem_unlock(unit, VLAN_TABm);
        return rv;
    }

    soc_mem_unlock(unit, VLAN_TABm);

    soc_mem_lock(unit, EGR_VLANm);

    rv = soc_mem_read(unit, EGR_VLANm, MEM_BLOCK_ANY, vlan, &egr_vtab); 
    if (rv < 0) {
        soc_mem_unlock(unit, EGR_VLANm);
        return rv;
    }

    soc_mem_pbmp_field_set(unit, EGR_VLANm, &egr_vtab, PORT_BITMAPf, pbmp);
    rv = soc_mem_write(unit, EGR_VLANm, MEM_BLOCK_ALL, vlan, &egr_vtab);
    if (rv < 0) {
        soc_mem_unlock(unit, EGR_VLANm);
        return rv;
    }

    soc_mem_unlock(unit, EGR_VLANm);

    return rv;
}

/*
 * Function:
 *      bcm_enduro_vlan_vp_add
 * Purpose:
 *      Add a VLAN virtual port to the specified vlan.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      vlan  - (IN) VLAN ID to add virtual port to as a member.
 *      gport - (IN) VLAN VP Gport ID
 *      flags - (IN) Indicates if packet should egress out of the given
 *                   VLAN virtual port untagged. 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_enduro_vlan_vp_add(int unit, bcm_vlan_t vlan, bcm_gport_t gport,
                       int flags)
{
    int rv = BCM_E_NONE;
    int vp;
    int vp_is_local;
    vlan_tab_entry_t vtab;
    bcm_pbmp_t vlan_pbmp, vlan_ubmp, l2_pbmp, l3_pbmp, l2_l3_pbmp;
    int i, mc_idx;
    bcm_vlan_port_t vlan_vp;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int id;
    int mod_local;
    bcm_gport_t local_gport;
    bcm_if_t encap_id;
    bcm_multicast_t group;
    int bc_idx, umc_idx, uuc_idx;
    soc_field_t group_type[3] = {BC_IDXf, UMC_IDXf, UUC_IDXf};
    int bc_do_not_add = 0, umc_do_not_add = 0, uuc_do_not_add = 0;

    VLAN_VIRTUAL_INIT(unit);

    vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_enduro_vlan_vp_is_local(unit, vp, &vp_is_local));
    if (!vp_is_local) {
        return BCM_E_PORT;
    }

    if (flags & ~(BCM_VLAN_PORT_UNTAGGED |
                  BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD |
                  BCM_VLAN_GPORT_ADD_UNKNOWN_MCAST_DO_NOT_ADD |
                  BCM_VLAN_GPORT_ADD_BCAST_DO_NOT_ADD)) {
        return BCM_E_PARAM;
    }

    sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));

    soc_mem_lock(unit, VLAN_TABm);

    rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, vlan, &vtab); 
    if (rv < 0) {
        soc_mem_unlock(unit, VLAN_TABm);
        return rv;
    }

    if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
        soc_mem_unlock(unit, VLAN_TABm);
        return BCM_E_NOT_FOUND;
    }

    /* Enable VP switching on the VLAN */
    if (!SHR_BITGET(vlan_info[unit].vp_mode, vlan) && 
        SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
           soc_mem_field32_set(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf, 1);
           rv = soc_mem_write(unit, VLAN_TABm, MEM_BLOCK_ALL, vlan, &vtab);
           if (rv < 0) {
              soc_mem_unlock(unit, VLAN_TABm);
              return rv;
           }
           soc_mem_unlock(unit, VLAN_TABm);

           /* Also need to copy the physical port members to the L2_BITMAP of
            * the IPMC entry for each group once we've gone virtual */
           rv = mbcm_driver[unit]->mbcm_vlan_port_get
               (unit, vlan, &vlan_pbmp, &vlan_ubmp, NULL);
           if (rv < 0) {
               return rv;
           }

           /* Deal with each group */
           for (i = 0; i < 3; i++) {
               mc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, group_type[i]);
               rv = _bcm_esw_multicast_ipmc_read(unit, mc_idx, &l2_pbmp, &l3_pbmp);
               if (rv < 0) {
                   return rv;
               }
               rv = _bcm_esw_multicast_ipmc_write(unit, mc_idx, vlan_pbmp,
                   l3_pbmp, TRUE);
               if (rv < 0) {
                   return rv;
               }
           }
        }   
    } else { 
        soc_mem_unlock(unit, VLAN_TABm);
    }

    bc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, BC_IDXf);
    umc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, UMC_IDXf);
    uuc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, UUC_IDXf);

    if (flags & BCM_VLAN_GPORT_ADD_BCAST_DO_NOT_ADD) {
        bc_do_not_add = 1;
    } else {
        bc_do_not_add = 0;
    }
    if (flags & BCM_VLAN_GPORT_ADD_UNKNOWN_MCAST_DO_NOT_ADD) {
        umc_do_not_add = 1;
    } else {
        umc_do_not_add = 0;
    }
    if (flags & BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD) {
        uuc_do_not_add = 1;
    } else {
        uuc_do_not_add = 0;
    }

    if ((bc_idx == umc_idx) && (bc_do_not_add != umc_do_not_add)) {
        return BCM_E_PARAM;
    }
    if ((bc_idx == uuc_idx) && (bc_do_not_add != uuc_do_not_add)) {
        return BCM_E_PARAM;
    }
    if ((umc_idx == uuc_idx) && (umc_do_not_add != uuc_do_not_add)) {
        return BCM_E_PARAM;
    }
    
    /* Get the VP attributes (need physical port) */
    bcm_vlan_port_t_init(&vlan_vp);
    vlan_vp.vlan_port_id = gport;
    rv = _bcm_enduro_vlan_vp_find(unit, &vlan_vp);
    if (rv < 0) {
        return rv;
    }

    /* Check if vlan_vp.port is a local port or trunk */
    if (BCM_GPORT_IS_TRUNK(vlan_vp.port)) {
        trunk_id = BCM_GPORT_TRUNK_GET(vlan_vp.port);
        rv = _bcm_trunk_id_validate(unit, trunk_id);
        if (BCM_FAILURE(rv)) {
            return BCM_E_PORT;
        }
        local_gport = vlan_vp.port;
    } else {
        rv = _bcm_esw_gport_resolve(unit, vlan_vp.port, &mod_out, &port_out,
                &trunk_id, &id); 
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PORT);
        }
        rv = _bcm_esw_modid_is_local(unit, mod_out, &mod_local);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        if (TRUE != mod_local) {
            /* Only add this to replication set if destination is local */
            return BCM_E_PORT;
        }
        /* Convert system local_port to physical local_port */
        if (soc_feature(unit, soc_feature_sysport_remap)) {
            BCM_XLATE_SYSPORT_S2P(unit, &port_out);
        }
        rv = bcm_esw_port_gport_get(unit, port_out, &local_gport);
        if (BCM_FAILURE(rv)) {
            return rv;
        }        
    }

    /* Add the VP to the BC group */
    if (!bc_do_not_add) {
        _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_VLAN, bc_idx);
        rv = bcm_esw_multicast_vlan_encap_get(unit, group, local_gport,
                gport, &encap_id);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        rv = bcm_esw_multicast_egress_add(unit, group, local_gport, encap_id); 
        if (rv < 0) {
            return rv;
        } 

    }

    /* Add the VP to the UMC group, if UMC group is different from BC group */
    if (!umc_do_not_add) {
        if (umc_idx != bc_idx) {
            _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_VLAN, umc_idx);
            rv = bcm_esw_multicast_vlan_encap_get(unit, group, local_gport,
                    gport, &encap_id);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            rv = bcm_esw_multicast_egress_add(unit, group, local_gport, encap_id); 
            if (rv < 0) {
                return rv;
            }
        }
    }

    /* Add the VP to the UUC group, if UUC group is different from BC/UMC group */
    if (!uuc_do_not_add) {
        if ((uuc_idx != bc_idx) && (uuc_idx != umc_idx)) {
            _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_VLAN, uuc_idx);
            rv = bcm_esw_multicast_vlan_encap_get(unit, group, local_gport,
                    gport, &encap_id);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            rv = bcm_esw_multicast_egress_add(unit, group, local_gport, encap_id); 
            if (rv < 0) {
                return rv;
            }
        }
    }

    /* Update the VLAN table's port bitmap to contain the BC/UMC/UUC groups' 
     * L2 and L3 bitmaps, since the VLAN table's port bitmap is used for
     * ingress and egress VLAN membership checks.
     */

    BCM_PBMP_CLEAR(l2_l3_pbmp);
    for (i = 0; i < 3; i++) {
        mc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, group_type[i]);
        rv = _bcm_esw_multicast_ipmc_read(unit, mc_idx, &l2_pbmp, &l3_pbmp);
        if (rv < 0) {
            return rv;
        }
        BCM_PBMP_OR(l2_l3_pbmp, l2_pbmp);
        BCM_PBMP_OR(l2_l3_pbmp, l3_pbmp);
    }

    rv = bcm_enduro_vlan_vp_update_vlan_pbmp(unit, vlan, &l2_l3_pbmp);
    if (rv < 0) {
        return rv;
    }

    VLAN_VIRTUAL_LOCK(unit);
    rv = _bcm_enduro_vlan_vp_untagged_add(unit, vlan, vp, flags);
    VLAN_VIRTUAL_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_enduro_vlan_vp_delete
 * Purpose:
 *      Remove a VLAN virtual port from the specified vlan.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      vlan  - (IN) VLAN to remove virtual port from.
 *      gport - (IN) VLAN VP Gport ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_enduro_vlan_vp_delete(int unit, bcm_vlan_t vlan, bcm_gport_t gport) 
{
    int rv = BCM_E_NONE;
    int vp;
    int vp_is_local;
    vlan_tab_entry_t vtab;
    bcm_vlan_port_t vlan_vp;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int id;
    int mod_local;
    bcm_gport_t local_gport;
    int bc_idx, umc_idx, uuc_idx;
    bcm_multicast_t group;
    bcm_if_t encap_id;
    int i, mc_idx;
    soc_field_t group_type[3] = {BC_IDXf, UMC_IDXf, UUC_IDXf};
    bcm_pbmp_t l2_pbmp, l3_pbmp, l2_l3_pbmp;

    VLAN_VIRTUAL_INIT(unit);

    vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_enduro_vlan_vp_is_local(unit, vp, &vp_is_local));
    if (!vp_is_local) {
        return BCM_E_PORT;
    }

    sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));

    rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, vlan, &vtab); 
    if (rv < 0) {
        return rv;
    }

    if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
        return BCM_E_NOT_FOUND;
    }

    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
      if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
         /* VP switching is not enabled on the VLAN */
         return BCM_E_PORT;
      }
    }

    /* Get the VP attributes (need physical port) */
    bcm_vlan_port_t_init(&vlan_vp);
    vlan_vp.vlan_port_id = gport;
    rv = _bcm_enduro_vlan_vp_find(unit, &vlan_vp);
    if (rv < 0) {
        return rv;
    }

    /* Check if vlan_vp.port is a local port or trunk */
    if (BCM_GPORT_IS_TRUNK(vlan_vp.port)) {
        trunk_id = BCM_GPORT_TRUNK_GET(vlan_vp.port);
        rv = _bcm_trunk_id_validate(unit, trunk_id);
        if (BCM_FAILURE(rv)) {
            return BCM_E_PORT;
        }
        local_gport = vlan_vp.port;
    } else {
        rv = _bcm_esw_gport_resolve(unit, vlan_vp.port, &mod_out, &port_out,
                &trunk_id, &id); 
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PORT);
        }
        rv = _bcm_esw_modid_is_local(unit, mod_out, &mod_local);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        if (TRUE != mod_local) {
            /* Only add this to replication set if destination is local */
            return BCM_E_PORT;
        }
        /* Convert system local_port to physical local_port */
        if (soc_feature(unit, soc_feature_sysport_remap)) {
            BCM_XLATE_SYSPORT_S2P(unit, &port_out);
        }
        rv = bcm_esw_port_gport_get(unit, port_out, &local_gport);
        if (BCM_FAILURE(rv)) {
            return rv;
        }        
    }

    /* Delete the VP from the BC group */
    bc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, BC_IDXf);
    _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_VLAN, bc_idx);
    rv = bcm_esw_multicast_vlan_encap_get(unit, group, local_gport,
            gport, &encap_id);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    rv = bcm_esw_multicast_egress_delete(unit, group, local_gport, encap_id); 
    if (rv < 0) {
        return rv;
    }

    /* Delete the VP from the UMC group, if UMC group is different
     * from BC group.
     */ 
    umc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, UMC_IDXf);
    if (umc_idx != bc_idx) {
        _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_VLAN, umc_idx);
        rv = bcm_esw_multicast_vlan_encap_get(unit, group, local_gport,
                gport, &encap_id);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        rv = bcm_esw_multicast_egress_delete(unit, group, local_gport, encap_id); 
        if (rv < 0) {
            return rv;
        }
    }

    /* Delete the VP from the UUC group, if UUC group is different
     * from BC and UMC groups.
     */
    uuc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, UUC_IDXf);
    if ((uuc_idx != bc_idx) && (uuc_idx != umc_idx)) {
        _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_VLAN, uuc_idx);
        rv = bcm_esw_multicast_vlan_encap_get(unit, group, local_gport,
                gport, &encap_id);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        rv = bcm_esw_multicast_egress_delete(unit, group, local_gport, encap_id); 
        if (rv < 0) {
            return rv;
        }
    }

    /* Update the VLAN table's port bitmap to contain the BC/UMC/UUC groups' 
     * L2 and L3 bitmaps, since the VLAN table's port bitmap is used for
     * ingress and egress VLAN membership checks.
     */

    BCM_PBMP_CLEAR(l2_l3_pbmp);
    for (i = 0; i < 3; i++) {
        mc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, group_type[i]);
        rv = _bcm_esw_multicast_ipmc_read(unit, mc_idx, &l2_pbmp, &l3_pbmp);
        if (rv < 0) {
            return rv;
        }
        BCM_PBMP_OR(l2_l3_pbmp, l2_pbmp);
        BCM_PBMP_OR(l2_l3_pbmp, l3_pbmp);
    }

    rv = bcm_enduro_vlan_vp_update_vlan_pbmp(unit, vlan, &l2_l3_pbmp);
    if (rv < 0) {
        return rv;
    }

    VLAN_VIRTUAL_LOCK(unit);
    rv = _bcm_enduro_vlan_vp_untagged_delete(unit, vlan, vp);
    VLAN_VIRTUAL_UNLOCK(unit);

    return rv;
}

STATIC int
_bcm_enduro_vlan_mc_group_gport_is_member(int unit, bcm_multicast_t group,
        bcm_gport_t gport, bcm_gport_t vp, int *is_member) 
{
    int rv = BCM_E_NONE;
    bcm_if_t encap_id;
    int if_max;
    bcm_if_t *if_array;
    int if_count, if_cur;
    int mc_idx;

    if (NULL == is_member) {
        return BCM_E_PARAM;
    }
    *is_member = FALSE;

    /* Get VP's multicast encap id */
    if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_multicast_vlan_encap_get(unit, group,
                    vp, gport, &encap_id));
    } else {
        return BCM_E_PARAM;
    }

    /* Find out if VP is a member of the multicast group */
    if_max = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    if_array = sal_alloc(if_max * sizeof(bcm_if_t),
            "temp repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    mc_idx = _BCM_MULTICAST_ID_GET(group);
    {
        rv = bcm_esw_ipmc_egress_intf_get(unit, mc_idx,
                vp, if_max, if_array, &if_count);
        if (BCM_FAILURE(rv)) {
            sal_free(if_array);
            return rv;
        }
        for (if_cur = 0; if_cur < if_count; if_cur++) {
            if (if_array[if_cur] == encap_id) {
                *is_member = TRUE;
                sal_free(if_array);
                return BCM_E_NONE;
            }
        }
    }

    sal_free(if_array);
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_enduro_vlan_vp_get
 * Purpose:
 *      Get untagging status of a VLAN virtual port.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      vlan  - (IN) VLAN to remove virtual port from.
 *      gport - (IN) VLAN VP Gport ID
 *      flags - (OUT) Untagging status of the VLAN virtual port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_enduro_vlan_vp_get(int unit, bcm_vlan_t vlan, bcm_gport_t gport,
        int *flags)
{
    int rv = BCM_E_NONE;
    int vp;
    bcm_vlan_port_t vlan_vp;
    bcm_gport_t phy_port_trunk = BCM_GPORT_INVALID;
    int vp_is_local;
    vlan_tab_entry_t vtab;
    int bc_idx, umc_idx, uuc_idx;
    bcm_multicast_t group;
    int is_bc_member = FALSE;
    int is_umc_member = FALSE;
    int is_uuc_member = FALSE;
    int untagged_flag = 0;

    VLAN_VIRTUAL_INIT(unit);

    vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }

    /* Get the physical port or trunk the VP resides on */
    bcm_vlan_port_t_init(&vlan_vp);
    vlan_vp.vlan_port_id = gport;
    BCM_IF_ERROR_RETURN(bcm_enduro_vlan_vp_find(unit, &vlan_vp));
    phy_port_trunk = vlan_vp.port;
    BCM_IF_ERROR_RETURN
        (_bcm_enduro_vlan_vp_is_local(unit, vp, &vp_is_local));
    if (!vp_is_local) {
        return BCM_E_PORT;
    }

    *flags = 0;
    
    sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, vlan, &vtab));
    if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
        return BCM_E_NOT_FOUND;
    }

    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
       if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
           /* No virtual port exists for this VLAN */
           return BCM_E_NOT_FOUND;
       } 
    }
    
    if (phy_port_trunk != BCM_GPORT_INVALID) {
    
        bc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, BC_IDXf);
        _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_VLAN, bc_idx);        
        BCM_IF_ERROR_RETURN(_bcm_enduro_vlan_mc_group_gport_is_member(unit, group,
                    gport, phy_port_trunk, &is_bc_member));

        umc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, UMC_IDXf);
        if (umc_idx != bc_idx) {
            BCM_IF_ERROR_RETURN(_bcm_tr_multicast_ipmc_group_type_get(unit, umc_idx,
                        &group));
            BCM_IF_ERROR_RETURN(_bcm_enduro_vlan_mc_group_gport_is_member(unit, group,
                        gport, phy_port_trunk, &is_umc_member));
        } else {
            is_umc_member = is_bc_member;
        }

        uuc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, UUC_IDXf);
        if (uuc_idx != bc_idx && uuc_idx != umc_idx) {
            BCM_IF_ERROR_RETURN(_bcm_tr_multicast_ipmc_group_type_get(unit, uuc_idx,
                        &group));
            BCM_IF_ERROR_RETURN(_bcm_enduro_vlan_mc_group_gport_is_member(unit, group,
                        gport, phy_port_trunk, &is_uuc_member));
        } else if (uuc_idx == bc_idx) {
            is_uuc_member = is_bc_member;
        } else {
            is_uuc_member = is_umc_member;
        }
        
    }
    if (!is_bc_member) {
        *flags |= BCM_VLAN_GPORT_ADD_BCAST_DO_NOT_ADD;
    }
    if (!is_umc_member) {
        *flags |= BCM_VLAN_GPORT_ADD_UNKNOWN_MCAST_DO_NOT_ADD;
    }
    if (!is_uuc_member) {
        *flags |= BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD;
    }
    
    rv = _bcm_enduro_vlan_vp_untagged_get(unit, vlan, vp, &untagged_flag);
    
    *flags |= untagged_flag;
    
    return rv;
    
}

/*
 * Function:
 *      _bcm_enduro_vlan_vp_mc_group_get_all
 * Purpose:
 *      Get all the next hop indices of the given multicast group.
 * Parameters:
 *      unit   - (IN) SOC unit number. 
 *      mc_idx - (IN) Multicast group index.
 *      nh_bitmap - (IN/OUT) Bitmap of next hop indices.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_enduro_vlan_vp_mc_group_get_all(int unit, int mc_idx,
        SHR_BITDCL *nh_bitmap)
{
    bcm_multicast_t mc_group;
    int rv;
    int num_encap_id;
    bcm_if_t *encap_id_array = NULL;
    int i;
    int nh_index;

    _BCM_MULTICAST_GROUP_SET(mc_group, _BCM_MULTICAST_TYPE_VLAN, mc_idx);

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
            nh_index = encap_id_array[i];
            SHR_BITSET(nh_bitmap, nh_index);
        }
    }

    sal_free(encap_id_array);
    return rv;
}

/*
 * Function:
 *      bcm_enduro_vlan_vp_get_all
 * Purpose:
 *      Get virtual ports of the given VLAN.
 * Parameters:
 *      unit      - (IN) SOC unit number. 
 *      vlan      - (IN) VLAN ID.
 *      array_max - (IN) Max number of elements in the array.
 *      gport_array       - (OUT) Array of virtual ports in GPORT format.
 *      flags_array       - (OUT) Array of untagging status.
 *      array_size        - (OUT) Actual number of elements in the array.
 * Returns:
 *      BCM_E_XXX
 * Notes: If array_max == 0 and gport_array == NULL, actual number of
 *        virtual ports in the VLAN will be returned in array_size.
 */
int
bcm_enduro_vlan_vp_get_all(int unit, bcm_vlan_t vlan, int array_max,
        bcm_gport_t *gport_array, int *flags_array, int *array_size) 
{
    vlan_tab_entry_t vtab;
    int        nh_tbl_size;
    SHR_BITDCL *nh_bitmap = NULL;
    int        bc_idx, umc_idx, uuc_idx;
    int        rv;
    int        i;
    egr_l3_next_hop_entry_t egr_nh;
    int        dvp;

    if (array_max < 0) {
        return BCM_E_PARAM;
    }

    if ((array_max > 0) && (NULL == gport_array)) {
        return BCM_E_PARAM;
    }

    if (NULL == array_size) {
        return BCM_E_PARAM;
    }

    *array_size = 0;

    VLAN_VIRTUAL_INIT(unit);

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vlan, &vtab));
   
    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
       if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
           /* This vlan has no virtual port members. */
           return BCM_E_NONE;
       } 
    }

    nh_tbl_size = soc_mem_index_count(unit, ING_L3_NEXT_HOPm);
    nh_bitmap = sal_alloc(SHR_BITALLOCSIZE(nh_tbl_size), "nh_bitmap");
    if (NULL == nh_bitmap) {
        return BCM_E_MEMORY;
    }
    sal_memset(nh_bitmap, 0, SHR_BITALLOCSIZE(nh_tbl_size));

    /* Get VPs from BC group */
    bc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, BC_IDXf);
    rv = _bcm_enduro_vlan_vp_mc_group_get_all(unit, bc_idx, nh_bitmap);
    if (rv < 0) {
        goto cleanup;
    }

    /* Get VPs from UMC group, if different from BC group */
    umc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, UMC_IDXf);
    if (umc_idx != bc_idx) {
        rv = _bcm_enduro_vlan_vp_mc_group_get_all(unit, umc_idx, nh_bitmap);
        if (rv < 0) {
            goto cleanup;
        }
    }

    /* Get VPs from UUC group, if different from BC and UMC group */
    uuc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, UUC_IDXf);
    if ((uuc_idx != bc_idx) && (uuc_idx != umc_idx)) {
        rv = _bcm_enduro_vlan_vp_mc_group_get_all(unit, uuc_idx, nh_bitmap);
        if (rv < 0) {
            goto cleanup;
        }
    }

    /* Convert next hop indices to VLAN VP gports */
    for (i = 0; i < nh_tbl_size; i++) {
        if (SHR_BITGET(nh_bitmap, i)) {
            if (0 == array_max) {
                (*array_size)++;
            } else {
                rv = READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, i, &egr_nh); 
                if (rv < 0) {
                    goto cleanup;
                }
                dvp = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, SD_TAG__DVPf);
                BCM_GPORT_VLAN_PORT_ID_SET(gport_array[*array_size], dvp);
                if (NULL != flags_array) {
                    rv = _bcm_enduro_vlan_vp_untagged_get(unit, vlan, dvp,
                            &flags_array[*array_size]);
                    if (rv < 0) {
                        goto cleanup;
                    }
                }
                (*array_size)++;
                if (*array_size == array_max) {
                    break;
                }
            }
        }
    }

cleanup:
    if (NULL != nh_bitmap) {
        sal_free(nh_bitmap);
    }

    return rv;
}

/*
 * Function:
 *      bcm_enduro_vlan_vp_delete_all
 * Purpose:
 *      Remove all VLAN virtual ports from the specified vlan.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      vlan  - (IN) VLAN to remove virtual ports from.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_enduro_vlan_vp_delete_all(int unit, bcm_vlan_t vlan) 
{
    vlan_tab_entry_t vtab;
    int num_vp;
    bcm_gport_t *gport_array = NULL;
    int rv = BCM_E_NONE;
    int bc_idx, umc_idx, uuc_idx;
    bcm_multicast_t group;

    VLAN_VIRTUAL_INIT(unit);

    sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, vlan, &vtab)); 

    if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
        return BCM_E_NOT_FOUND;
    }

    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
       if (soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {

           /* Get all VPs from the VLAN before deletion */

           BCM_IF_ERROR_RETURN
               (bcm_enduro_vlan_vp_get_all(unit, vlan, 0, NULL, NULL, &num_vp));

           gport_array = sal_alloc(sizeof(bcm_gport_t) * num_vp,
                  "vlan vp gport array");
           if (NULL == gport_array) {
               return BCM_E_MEMORY;
           }   
           sal_memset(gport_array, 0, sizeof(bcm_gport_t) * num_vp);

           rv = bcm_enduro_vlan_vp_get_all(unit, vlan, num_vp,
                   gport_array, NULL, &num_vp);
           if (rv < 0) {
               goto cleanup;
           }   

           /* Delete all VPs from the BC group */

           bc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, BC_IDXf);
           _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_VLAN, bc_idx);
           rv = bcm_esw_multicast_egress_delete_all(unit, group); 
           if (rv < 0) {
               goto cleanup;
           }   
           rv = bcm_esw_multicast_destroy(unit, group);
           if (rv < 0) {
               goto cleanup;
           }

           /* Delete all VPs from the UMC group, if UMC group is different
            * from BC group.
            */ 

           umc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, UMC_IDXf);
           if (umc_idx != bc_idx) {
            _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_VLAN, umc_idx);
            rv = bcm_esw_multicast_egress_delete_all(unit, group); 
            if (rv < 0) {
                goto cleanup;
            }
            rv = bcm_esw_multicast_destroy(unit, group);
            if (rv < 0) {
                goto cleanup;
            }
        }

        /* Delete all VPs from the UUC group, if UUC group is different
         * from BC and UMC groups.
         */
        uuc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, UUC_IDXf);
        if ((uuc_idx != bc_idx) && (uuc_idx != umc_idx)) {
            _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_VLAN, uuc_idx);
            rv = bcm_esw_multicast_egress_delete_all(unit, group); 
            if (rv < 0) {
                goto cleanup;
            }
            rv = bcm_esw_multicast_destroy(unit, group);
            if (rv < 0) {
                goto cleanup;
            }
        }

        if (!SHR_BITGET(vlan_info[unit].vp_mode, vlan) &&
            SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
            soc_mem_field32_set(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf, 0);
        }
        soc_mem_field32_set(unit, VLAN_TABm, &vtab, BC_IDXf, 0);
        soc_mem_field32_set(unit, VLAN_TABm, &vtab, UMC_IDXf, 0);
        soc_mem_field32_set(unit, VLAN_TABm, &vtab, UUC_IDXf, 0);
        rv = soc_mem_write(unit, VLAN_TABm, MEM_BLOCK_ALL, (int)vlan, &vtab);
        if (rv < 0) {
            goto cleanup;
        }

        /* Delete all DVP tagging/untagging entries */
        VLAN_VIRTUAL_LOCK(unit);
        rv = _bcm_enduro_vlan_vp_untagged_delete_all(unit, vlan,
                num_vp, gport_array);
        VLAN_VIRTUAL_UNLOCK(unit);
        if (rv < 0) {
            goto cleanup;
        }
      }
    }

cleanup:
    if (NULL != gport_array) {
        sal_free(gport_array);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_enduro_vlan_port_resolve
 * Purpose:
 *      Get the modid, port, trunk values for a VLAN virtual port
 * Parameters:
 *      unit - (IN) BCM device number
 *      gport - (IN) Global port identifier
 *      modid - (OUT) Module ID
 *      port - (OUT) Port number
 *      trunk_id - (OUT) Trunk ID
 *      id - (OUT) Virtual port ID
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_enduro_vlan_port_resolve(int unit, bcm_gport_t vlan_port_id,
                          bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id)

{
    int rv = BCM_E_NONE, nh_index, vp;
    ing_l3_next_hop_entry_t ing_nh;
    ing_dvp_table_entry_t dvp;

    VLAN_VIRTUAL_INIT(unit);

    if (!BCM_GPORT_IS_VLAN_PORT(vlan_port_id)) {
        return (BCM_E_BADID);
    }

    vp = BCM_GPORT_VLAN_PORT_ID_GET(vlan_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                              NEXT_HOP_INDEXf);
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      nh_index, &ing_nh));

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, ENTRY_TYPEf) != 0x2) {
        /* Entry type is not L2 DVP */
        return BCM_E_NOT_FOUND;
    }
    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
        *trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
    } else {
        /* Only add this to replication set if destination is local */
        *modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
        *port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
    }
    *id = vp;
    return rv;
}


#endif /* BCM_ENDURO_SUPPORT && INCLUDE_L3 */

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    vlan.c
 * Purpose: Manages VLAN virtual port creation and deletion.
 *          Also manages addition and removal of virtual
 *          ports from VLAN. The types of virtual ports that
 *          can be added to or removed from VLAN can be VLAN
 *          VPs, NIV VPs, or Extender VPs.
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3) 

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
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/ipmc.h>
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT*/

#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT*/

#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#endif /*BCM_KATANA2_SUPPORT*/
#if defined(BCM_KATANA_SUPPORT)
#include <bcm_int/esw/katana.h>
#endif /*BCM_KATANA_SUPPORT*/

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/vlan.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT*/

#include <bcm_int/esw/policer.h>


/* -------------------------------------------------------
 * Software book keeping for VLAN virtual port information
 * -------------------------------------------------------
 */

typedef struct _bcm_tr2_vlan_vp_info_s {
    bcm_vlan_port_match_t criteria;
    uint32 flags;
    int match_count;
    bcm_vlan_t match_vlan;
    bcm_vlan_t match_inner_vlan;
    bcm_vlan_t match_tunnel_value;
    bcm_gport_t port;
} _bcm_tr2_vlan_vp_info_t;

typedef struct _bcm_tr2_vlan_virtual_bookkeeping_s {
    int vlan_virtual_initialized; /* Flag to check initialized status */
    sal_mutex_t vlan_virtual_mutex; /* VLAN virtual module lock */
    _bcm_tr2_vlan_vp_info_t *port_info; /* VP state */
} _bcm_tr2_vlan_virtual_bookkeeping_t;

STATIC _bcm_tr2_vlan_virtual_bookkeeping_t _bcm_tr2_vlan_virtual_bk_info[BCM_MAX_NUM_UNITS];

#define VLAN_VIRTUAL_INFO(unit) (&_bcm_tr2_vlan_virtual_bk_info[unit])

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

#ifdef BCM_TRIUMPH3_SUPPORT
#define _BCM_VLAN_VPN_INVALID 0xFFFF     /* Invalid VLAN VPN */
#define _BCM_VLAN_VFI_INVALID 0          /* Invalid VFI INDEX */
#define VIRTUAL_INFO(unit)    (&_bcm_virtual_bk_info[unit])
#endif

/* ---------------------------------------------------------------------------- 
 *
 * Routines for creation and deletion of VLAN virtual ports
 *
 * ---------------------------------------------------------------------------- 
 */

/*
 * Function:
 *      _bcm_tr2_vlan_vp_port_cnt_update
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
int
_bcm_tr2_vlan_vp_port_cnt_update(int unit, bcm_gport_t gport,
        int vp, int incr)
{
    int mod_out, port_out, tgid_out, id_out;
    int idx;
    int mod_local = FALSE;
    _bcm_port_info_t *port_info;
    int local_member_count;
    int soc_max_num_ports = SOC_MAX_NUM_PORTS;
    uint32 port_flags;
#if (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t local_member_array[SOC_MAX_NUM_PP_PORTS];
#else
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
#endif

    if (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS) {
        soc_max_num_ports = SOC_MAX_NUM_PP_PORTS;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, &mod_out, 
                                &port_out, &tgid_out, &id_out));
    if (-1 != id_out) {
        return BCM_E_PARAM;
    }

    /* In case of extended queuing on KTX, tgid_out gets updated with
     * extended queue value. So set it to invalid value.
     */
#ifdef BCM_KATANA_SUPPORT
    if (soc_feature(unit, soc_feature_extended_queueing)) {
        if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) ||
            (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) ||
            (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport))) {
            tgid_out = BCM_TRUNK_INVALID;
        }
    }
#endif
    /* Update the physical port's SW state. If associated with a trunk,
     * update each local physical port's SW state.
     */

    if (BCM_TRUNK_INVALID != tgid_out) {
        trunk_private_t *t_info;

        BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid_out,
                soc_max_num_ports, local_member_array, &local_member_count));

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

        /* account for the TGID vp-port usage */
        t_info = _bcm_esw_trunk_info_get(unit, tgid_out);
        if (incr) {
            t_info->vlan_vp_count++;
        } else if (t_info->vlan_vp_count != 0) {
            t_info->vlan_vp_count--;
        }
    } else {


#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            BCM_GPORT_IS_SET(gport) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
            if(_bcm_xgs5_subport_coe_gport_local(unit, gport)) {

                BCM_IF_ERROR_RETURN(
                    _bcmi_coe_subport_physical_port_get(unit,
                                                        gport,
                                                        &port_out));

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
        } else
#endif

#if defined(BCM_KATANA2_SUPPORT)
        if (BCM_GPORT_IS_SET(gport) &&
            _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, gport)) {
            BCM_IF_ERROR_RETURN(_bcm_kt2_modport_is_local_coe_subport(unit,
                mod_out, port_out, &mod_local));
            if (mod_local) {
                port_out = BCM_GPORT_SUBPORT_PORT_GET(gport);
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
#endif
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
 *      _bcm_tr2_vlan_virtual_free_resources
 * Purpose:
 *      Free all allocated tables and memory
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_tr2_vlan_virtual_free_resources(int unit)
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

/*
 * Function:
 *      bcm_tr2_vlan_virtual_init
 * Purpose:
 *      Initialize the VLAN virtual port module.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_vlan_virtual_init(int unit)
{
    int num_vp;
    int rv = BCM_E_NONE;

    if (VLAN_VIRTUAL_INFO(unit)->vlan_virtual_initialized) {
        bcm_tr2_vlan_virtual_detach(unit);
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    if (NULL == VLAN_VIRTUAL_INFO(unit)->port_info) {
        VLAN_VIRTUAL_INFO(unit)->port_info =
            sal_alloc(sizeof(_bcm_tr2_vlan_vp_info_t) * num_vp, "vlan_vp_info");
        if (NULL == VLAN_VIRTUAL_INFO(unit)->port_info) {
            _bcm_tr2_vlan_virtual_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(VLAN_VIRTUAL_INFO(unit)->port_info, 0,
            sizeof(_bcm_tr2_vlan_vp_info_t) * num_vp);

    if (NULL == VLAN_VIRTUAL_INFO(unit)->vlan_virtual_mutex) {
        VLAN_VIRTUAL_INFO(unit)->vlan_virtual_mutex =
            sal_mutex_create("vlan virtual mutex");
        if (NULL == VLAN_VIRTUAL_INFO(unit)->vlan_virtual_mutex) {
            _bcm_tr2_vlan_virtual_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }

    VLAN_VIRTUAL_INFO(unit)->vlan_virtual_initialized = 1;

    /* Warm boot recovery of VLAN_VP_INFO depends on the completion
     * of _bcm_virtual_init, but _bcm_virtual_init is after the 
     * VLAN module in the init sequence. Hence, warm boot recovery
     * of VLAN_VP_INFO is moved to end of _bcm_virtual_init.
     */

    return rv;
}

/*
 * Function:
 *      bcm_tr2_vlan_virtual_detach
 * Purpose:
 *      Detach the VLAN virtual port module.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_vlan_virtual_detach(int unit)
{
    _bcm_tr2_vlan_virtual_free_resources(unit);

    VLAN_VIRTUAL_INFO(unit)->vlan_virtual_initialized = 0;

    return BCM_E_NONE;
}

#ifdef BCM_TRIUMPH3_SUPPORT
/*
 * Function:
 *      _bcm_tr3_vlan_nh_sd_tag_set
 * Purpose:
 *      Set SD TAG INFORMATION
 * Parameters:
 *      unit    - (IN) Device Number
 *      egr_nh  - (IN) Egress next hop information
 *      vlan_vp - (IN) VLAN port information
 *      drop    - (IN) Drop flag
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_vlan_nh_sd_tag_set(int unit, egr_l3_next_hop_entry_t *egr_nh,
                            bcm_vlan_port_t *vlan_vp, int drop)
{
    int sd_tag_pri = -1;
    int sd_tag_cfi = -1;
    int sd_tag_vlan = -1;
    int sd_tag_action_present = -1;
    int sd_tag_action_not_present = -1;
    int tpid_index = -1;

    /* Check that unsupported flags are not set */
    if (!(vlan_vp->flags & (BCM_VLAN_PORT_SERVICE_VLAN_ADD |
                  BCM_VLAN_PORT_SERVICE_VLAN_REPLACE |
                  BCM_VLAN_PORT_SERVICE_VLAN_DELETE |
                  BCM_VLAN_PORT_SERVICE_VLAN_TPID_REPLACE  |
                  BCM_VLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE |
                  BCM_VLAN_PORT_SERVICE_VLAN_PRI_REPLACE |
                  BCM_VLAN_PORT_SERVICE_PRI_REPLACE |
                  BCM_VLAN_PORT_SERVICE_TPID_REPLACE))) {
        return BCM_E_NONE;
    }

    if (vlan_vp->flags & BCM_VLAN_PORT_SERVICE_VLAN_ADD) {
        if (vlan_vp->egress_vlan >= BCM_VLAN_INVALID) {
            return BCM_E_PARAM;
        }
        if (vlan_vp->pkt_pri > BCM_PRIO_MAX) {
            return BCM_E_PARAM;
        }
        if (vlan_vp->pkt_cfi > 1) {
            return BCM_E_PARAM;
        }

        sd_tag_vlan = vlan_vp->egress_vlan;
        sd_tag_pri = vlan_vp->pkt_pri;
        sd_tag_cfi = vlan_vp->pkt_cfi;
        sd_tag_action_not_present = 0x1; /* ADD */
    }

    if (vlan_vp->flags & BCM_VLAN_PORT_SERVICE_VLAN_TPID_REPLACE) {
        if (vlan_vp->egress_vlan >= BCM_VLAN_INVALID) {
            return BCM_E_PARAM;
        }

        /* REPLACE_VID_TPID */
        sd_tag_vlan = vlan_vp->egress_vlan;
        sd_tag_action_present = 0x1;
    } else if (vlan_vp->flags & BCM_VLAN_PORT_SERVICE_VLAN_REPLACE) {
        if (vlan_vp->egress_vlan >= BCM_VLAN_INVALID) {
            return BCM_E_PARAM;
        }
        /* REPLACE_VID_ONLY */
        sd_tag_vlan = vlan_vp->egress_vlan;
        sd_tag_action_present = 0x2;
    } else if (vlan_vp->flags & BCM_VLAN_PORT_SERVICE_VLAN_DELETE) {
        sd_tag_action_present = 0x3;
        sd_tag_action_not_present = 0;
    } else if (vlan_vp->flags & BCM_VLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE) {
        if (vlan_vp->egress_vlan >= BCM_VLAN_INVALID) {
            return BCM_E_PARAM;
        }
        if (vlan_vp->pkt_pri > BCM_PRIO_MAX) {
            return BCM_E_PARAM;
        }
        if (vlan_vp->pkt_cfi > 1) {
            return BCM_E_PARAM;
        }

        /* REPLACE_VID_PRI_TPID */
        sd_tag_vlan = vlan_vp->egress_vlan;
        sd_tag_pri = vlan_vp->pkt_pri;
        sd_tag_cfi = vlan_vp->pkt_cfi;
        sd_tag_action_present = 0x4;
    } else if (vlan_vp->flags & BCM_VLAN_PORT_SERVICE_VLAN_PRI_REPLACE) {
        if (vlan_vp->egress_vlan >= BCM_VLAN_INVALID) {
            return BCM_E_PARAM;
        }
        if (vlan_vp->pkt_pri > BCM_PRIO_MAX) {
            return BCM_E_PARAM;
        }
        if (vlan_vp->pkt_cfi > 1) {
            return BCM_E_PARAM;
        }

        /* REPLACE_VID_PRI_ONLY */
        sd_tag_vlan = vlan_vp->egress_vlan;
        sd_tag_pri = vlan_vp->pkt_pri;
        sd_tag_cfi = vlan_vp->pkt_cfi;
        sd_tag_action_present = 0x5;
    } else if (vlan_vp->flags & BCM_VLAN_PORT_SERVICE_PRI_REPLACE) {
        if (vlan_vp->pkt_pri > BCM_PRIO_MAX) {
            return BCM_E_PARAM;
        }
        if (vlan_vp->pkt_cfi > 1) {
            return BCM_E_PARAM;
        }

        /* REPLACE_PRI_ONLY */
        sd_tag_pri = vlan_vp->pkt_pri;
        sd_tag_cfi = vlan_vp->pkt_cfi;
        sd_tag_action_present = 0x6;
    } else if (vlan_vp->flags & BCM_VLAN_PORT_SERVICE_TPID_REPLACE) {
        /* REPLACE_TPID_ONLY */
        sd_tag_action_present = 0x7;
    }

    if ((vlan_vp->flags & BCM_VLAN_PORT_SERVICE_VLAN_ADD) ||
        (vlan_vp->flags & BCM_VLAN_PORT_SERVICE_VLAN_TPID_REPLACE) ||
        (vlan_vp->flags & BCM_VLAN_PORT_SERVICE_TPID_REPLACE) ||
        (vlan_vp->flags & BCM_VLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE)) {
        /* TPID value is used */
        BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_add(unit, vlan_vp->egress_service_tpid, &tpid_index));
    }

    if (sd_tag_vlan != -1) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
            egr_nh, SD_TAG__SD_TAG_VIDf, sd_tag_vlan);
    }

    if (sd_tag_action_present != -1) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
            SD_TAG__SD_TAG_ACTION_IF_PRESENTf,
            sd_tag_action_present);
    }

    if (sd_tag_action_not_present != -1) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
            SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf,
            sd_tag_action_not_present);
    }

    if (sd_tag_pri != -1) {
        if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, SD_TAG__NEW_PRIf)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                         SD_TAG__NEW_PRIf, sd_tag_pri);
        }
    }

    if (sd_tag_cfi != -1) {
        if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, SD_TAG__NEW_CFIf)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                         SD_TAG__NEW_CFIf, sd_tag_cfi);
        }
        if(soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, SD_TAG__SD_TAG_REMARK_CFIf)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                         SD_TAG__SD_TAG_REMARK_CFIf, 0x1);
        }
    }

    if (tpid_index != -1) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                         egr_nh, SD_TAG__SD_TAG_TPID_INDEXf, tpid_index);
    }

    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                    SD_TAG__BC_DROPf, drop ? 1 : 0);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                    SD_TAG__UUC_DROPf, drop ? 1 : 0);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                    SD_TAG__UMC_DROPf, drop ? 1 : 0);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                    egr_nh, SD_TAG__DVP_IS_NETWORK_PORTf,
                    0);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                    SD_TAG__HG_LEARN_OVERRIDEf, 0);
    /* HG_MODIFY_ENABLE must be 0x0 for Ingress and Egress Chip */
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                    SD_TAG__HG_MODIFY_ENABLEf, 0x0);

    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH3_SUPPORT */

/*
 * Function:
 *      _bcm_tr2_vlan_vp_nh_info_set
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
_bcm_tr2_vlan_vp_nh_info_set(int unit, bcm_vlan_port_t *vlan_vp, int vp,
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
    soc_field_t ent_tpf = ENTRY_TYPEf;
    int bit_num;
    int network_group;
#ifdef BCM_KATANA_SUPPORT
    int extended_queue, index;
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    uint32 mtu_profile_index = 0;
#endif /* BCM_TRIUMPH3_SUPPORT */

    if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
        ent_tpf = DATA_TYPEf;
    }

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
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ent_tpf);
        if (egr_nh_entry_type != 0x2) { /* != SD-tag */
            return BCM_E_PARAM;
        }
    } else {
        egr_nh_entry_type = 0x2; /* SD-tag */
    }

    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            ent_tpf, egr_nh_entry_type);

    if ((vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL) ||
        (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_VLAN) ||
        (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_INNER_VLAN) ) {
        /* Add the LLTAG for outgoing packets */
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                SD_TAG__LLTAG_VIDf, vlan_vp->match_tunnel_value);
       /* Set the LLTAG ACTION as  ADDLLTAG if flag was not set */
        if (vlan_vp->flags & BCM_VLAN_PORT_EGRESS_PON_TUNNEL_NONE) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                                    SD_TAG__LLTAG_ACTIONSf, 0x0);
        } else {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                                    SD_TAG__LLTAG_ACTIONSf, 0x04); 
        }
    }

    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                SD_TAG__DVPf, vp);

    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
            SD_TAG__HG_HDR_SELf, 1);

    /* Set Egress Class */
    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, SD_TAG__CLASS_IDf)) {
        bit_num = soc_mem_field_length(unit, EGR_L3_NEXT_HOPm,
                                       SD_TAG__CLASS_IDf);
        if ( vlan_vp->if_class > ((1 << bit_num) - 1)) {
            return BCM_E_PARAM;
        } else {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                SD_TAG__CLASS_IDf, vlan_vp->if_class);
        }
    }

    /* Handle Split Horizon */

    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
        network_group = vlan_vp->egress_network_group_id;
        rv = _bcm_validate_splithorizon_network_group(unit,
             vlan_vp->flags & BCM_VLAN_PORT_NETWORK, &network_group);
        if (rv < 0) {
            goto cleanup;
        }
        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                SD_TAG__DVP_NETWORK_GROUPf)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                SD_TAG__DVP_NETWORK_GROUPf,
                                network_group);
        }
    }
#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_vfi)) {
        rv = _bcm_tr3_vlan_nh_sd_tag_set(unit, &egr_nh, vlan_vp, drop);
        if (rv < 0) {
            goto cleanup;
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

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

    if (soc_feature(unit, soc_feature_generic_dest)) {
        if (ing_nh_trunk == -1) {
            soc_mem_field32_dest_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                (ing_nh_module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS |
                ing_nh_port));
        } else {
            soc_mem_field32_dest_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_LAG, ing_nh_trunk);
        }
    } else {
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
    }

    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, drop);
    
    if ((vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL) ||
        (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_VLAN) ||
        (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_INNER_VLAN) ) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                        &ing_nh, ENTRY_TYPEf, 0x03); /* XPON DVP */
    } else {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                &ing_nh, ENTRY_TYPEf, 0x02); /* L2 DVP */

        /* Configuration of Mtu Size is valid only for L2 DVP Entry type  */
        if (SOC_MEM_FIELD_VALID(unit, ING_L3_NEXT_HOPm, MTU_SIZEf)) {
            soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                    &ing_nh, MTU_SIZEf, 0x3fff);
        }
#ifdef BCM_TRIUMPH3_SUPPORT
        else if (SOC_MEM_FIELD_VALID(unit, ING_L3_NEXT_HOPm,
                    DVP_ATTRIBUTE_1_INDEXf)) {
            BCM_IF_ERROR_RETURN(
                    _bcm_tr3_mtu_profile_index_get(unit, 0x3fff, &mtu_profile_index));
            soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                    DVP_ATTRIBUTE_1_INDEXf, mtu_profile_index);
        }
#endif /*BCM_TRIUMPH3_SUPPORT */
    }
    rv = soc_mem_write (unit, ING_L3_NEXT_HOPm,
            MEM_BLOCK_ALL, *nh_index, &ing_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Write INITIAL_ING_L3_NEXT_HOP entry */

    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));

    if (soc_feature(unit, soc_feature_generic_dest)) {
        if (ing_nh_trunk == -1) {
            soc_mem_field32_dest_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                &initial_ing_nh, DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                (ing_nh_module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS |
                ing_nh_port));
        } else {
            soc_mem_field32_dest_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                &initial_ing_nh, DESTINATIONf, SOC_MEM_FIF_DEST_LAG,
                ing_nh_trunk);
        }
    } else {
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
    }
    rv = soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
            MEM_BLOCK_ALL, *nh_index, &initial_ing_nh);
    if (rv < 0) {
        goto cleanup;
    }

#ifdef BCM_KATANA_SUPPORT
    if (BCM_SUCCESS(rv)) {
        if (soc_feature(unit, soc_feature_extended_queueing)) {
            if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, *nh_index)) {
                index = *nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
            } else {
                index = *nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
            }
            if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(vlan_vp->port)) ||
                (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(vlan_vp->port)) ||
                (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(vlan_vp->port))) {
#ifdef BCM_KATANA2_SUPPORT
                if (SOC_IS_KATANA2(unit)) {
                    BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_index_resolve(unit, vlan_vp->port, 0,
                                        _BCM_KT2_COSQ_INDEX_STYLE_BUCKET, NULL,
                                        &extended_queue, NULL));
                } else
#endif
                {
                    BCM_IF_ERROR_RETURN(_bcm_kt_cosq_index_resolve(unit, vlan_vp->port, 0,
                                        _BCM_KT_COSQ_INDEX_STYLE_BUCKET, NULL,
                                        &extended_queue, NULL));
                }
                rv = _bcm_kt_extended_queue_config(unit, vlan_vp->flags, vlan_vp->flags,
                                                   extended_queue,
                      (vlan_vp->qos_map_id & 0x3 /*_BCM_QOS_MAP_TYPE_MASK*/), &index);
            } else {
                /* Reset extended queue configuration, if any */
                if (vlan_vp->flags & BCM_L3_REPLACE) {
                    rv = _bcm_kt_extended_queue_config(unit, vlan_vp->flags, vlan_vp->flags,
                                                       0, 0, &index);
                }
            }
       }
       if (rv < 0) {
           goto cleanup;
       }
    }
#endif
    return rv;

cleanup:
    if (!(vlan_vp->flags & BCM_VLAN_PORT_REPLACE)) {
        (void) bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, *nh_index);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr2_vlan_vp_nh_info_delete
 * Purpose:
 *      Free next hop index and clear next hop tables.
 * Parameters:
 *      unit     - (IN) SOC unit number. 
 *      nh_index - (IN) Next hop index. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr2_vlan_vp_nh_info_delete(int unit, int nh_index)
{
    egr_l3_next_hop_entry_t egr_nh;
    ing_l3_next_hop_entry_t ing_nh;
    initial_ing_l3_next_hop_entry_t initial_ing_nh;

#ifdef BCM_TRIUMPH3_SUPPORT
    soc_field_t ent_tpf = ENTRY_TYPEf;

    if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
        ent_tpf = DATA_TYPEf;
    }
    if (soc_feature(unit, soc_feature_vlan_vfi)) {
        int action_present = 0, action_not_present = 0, old_tpid_idx = -1;
        uint32  entry_type = 0;

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm,
                                MEM_BLOCK_ANY, nh_index, &egr_nh));
        entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ent_tpf);
        if (entry_type == 0x2) {
            action_present = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                    SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
            action_not_present = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                    SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
            if ((action_not_present == 0x1) || (action_present == 0x1) ||
                (action_present == 0x4) || (action_present == 0x7)) {
                old_tpid_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                 &egr_nh, SD_TAG__SD_TAG_TPID_INDEXf);
                BCM_IF_ERROR_RETURN(
                        _bcm_fb2_outer_tpid_entry_delete(unit, old_tpid_idx));
            }
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

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

#if defined(BCM_TRIUMPH3_SUPPORT) 
/*
 * Function:
 *      _tr3_vlan_vp_match_add
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
_tr3_vlan_vp_match_add(int unit, bcm_vlan_port_t *vlan_vp, int vp)
{
    vlan_xlate_entry_t vent;
    vlan_xlate_extd_entry_t vxent, old_vxent;
    int key_type = 0;
    bcm_vlan_action_set_t action;
    uint32 profile_idx;
    int rv;

    if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_NONE) {
       return BCM_E_NONE;
    }

    if (!((vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN) ||
          (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED) ||
          (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN16) ||
          (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_INNER_VLAN))) {
        return BCM_E_PARAM;
    }

    if ((vlan_vp->egress_vlan > BCM_VLAN_MAX) ||
        (vlan_vp->egress_inner_vlan > BCM_VLAN_MAX)) {
        return BCM_E_PARAM;
    } 

    sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));
    sal_memset(&vxent, 0, sizeof(vlan_xlate_extd_entry_t));

    if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN) {
        key_type = bcmVlanTranslateKeyPortOuter;
    } else if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED) {
        key_type = bcmVlanTranslateKeyPortDouble;
    } else if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN16) {
        key_type = bcmVlanTranslateKeyPortOuterTag;
    } else if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_INNER_VLAN) {
        key_type = bcmVlanTranslateKeyPortInner;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_translate_entry_assemble(unit, &vent,
                                                vlan_vp->port,
                                                key_type,
                                                vlan_vp->match_inner_vlan,
                                                vlan_vp->match_vlan));

    BCM_IF_ERROR_RETURN
        (_bcm_tr3_vxlate2vxlate_extd(unit,&vent,&vxent));

    soc_VLAN_XLATE_EXTDm_field32_set(unit, &vxent, VALID_0f, 1);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, &vxent, VALID_1f, 1);

    soc_VLAN_XLATE_EXTDm_field32_set(unit, &vxent, XLATE__MPLS_ACTIONf, 0x1); /* SVP */
    soc_VLAN_XLATE_EXTDm_field32_set(unit, &vxent, SOURCE_VPf, vp);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, &vxent, NEW_OVIDf,
                                vlan_vp->egress_vlan);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, &vxent, NEW_IVIDf,
                                vlan_vp->egress_inner_vlan);

    bcm_vlan_action_set_t_init(&action);

    if (vlan_vp->flags & BCM_VLAN_PORT_INNER_VLAN_PRESERVE) {
        action.dt_outer = bcmVlanActionReplace;
        action.dt_outer_prio = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionNone;
        action.dt_inner_prio = bcmVlanActionNone;
    } else {
        if (soc_feature(unit, soc_feature_vlan_copy_action)) {
            action.dt_outer = bcmVlanActionCopy;
            action.dt_outer_prio = bcmVlanActionCopy;
        } else {
            action.dt_outer = bcmVlanActionReplace;
            action.dt_outer_prio = bcmVlanActionReplace;
        }
        action.dt_inner = bcmVlanActionDelete;
        action.dt_inner_prio = bcmVlanActionDelete;
    }

    action.ot_outer = bcmVlanActionReplace;
    action.ot_outer_prio = bcmVlanActionReplace;
    if (vlan_vp->flags & BCM_VLAN_PORT_INNER_VLAN_ADD) {
        action.ot_inner = bcmVlanActionAdd;
    } else {
        action.ot_inner = bcmVlanActionNone;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_add(unit, &action, &profile_idx));

    soc_VLAN_XLATE_EXTDm_field32_set(unit, &vxent, TAG_ACTION_PROFILE_PTRf,
                                profile_idx);

    rv = soc_mem_insert_return_old(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ALL,
                                   &vxent, &old_vxent);
    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_VLAN_XLATE_EXTDm_field32_get(unit, &old_vxent,
                                                  TAG_ACTION_PROFILE_PTRf);       
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    return rv;
}

/*
 * Function:
 *      _tr3_vlan_vp_match_delete
 * Purpose:
 *      Delete match criteria for VLAN VP.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vp - (IN) Virtual port number.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_tr3_vlan_vp_match_delete(int unit, int vp)
{
    vlan_xlate_entry_t vent;
    vlan_xlate_extd_entry_t vxent, old_vxent;
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
    } else if (VLAN_VP_INFO(unit, vp)->criteria == BCM_VLAN_PORT_MATCH_PORT_INNER_VLAN) {
        key_type = bcmVlanTranslateKeyPortInner;
    } else {
        return BCM_E_INTERNAL;
    }

    sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));
    sal_memset(&vxent, 0, sizeof(vlan_xlate_extd_entry_t));
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_translate_entry_assemble(unit, &vent,
                                                VLAN_VP_INFO(unit, vp)->port,
                                                key_type,
                                                VLAN_VP_INFO(unit, vp)->match_inner_vlan,
                                                VLAN_VP_INFO(unit, vp)->match_vlan));
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_vxlate2vxlate_extd(unit,&vent,&vxent));

    rv = soc_mem_delete_return_old(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ALL, 
                                   &vxent, &old_vxent);
    if ((rv == SOC_E_NONE) && 
        soc_VLAN_XLATE_EXTDm_field32_get(unit, &old_vxent, VALID_0f)) {
        profile_idx = soc_VLAN_XLATE_EXTDm_field32_get(unit, &old_vxent,
                                                  TAG_ACTION_PROFILE_PTRf);       
        /* Delete the old vlan action profile entry */
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    return rv;
}

/*
 * Function:
 *      _tr3_vlan_vp_match_get
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
_tr3_vlan_vp_match_get(int unit, int vp, bcm_vlan_port_t *vlan_vp)
{
    vlan_xlate_entry_t vent;
    vlan_xlate_extd_entry_t vxent, vxent_out;
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
    } else if (VLAN_VP_INFO(unit, vp)->criteria == BCM_VLAN_PORT_MATCH_PORT_INNER_VLAN) {
        key_type = bcmVlanTranslateKeyPortInner;
    } else {
        return BCM_E_INTERNAL;
    }

    sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));
    sal_memset(&vxent, 0, sizeof(vlan_xlate_extd_entry_t));

    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_translate_entry_assemble(unit, &vent,
                                                VLAN_VP_INFO(unit, vp)->port,
                                                key_type,
                                                VLAN_VP_INFO(unit, vp)->match_inner_vlan,
                                                VLAN_VP_INFO(unit, vp)->match_vlan));

    BCM_IF_ERROR_RETURN
        (_bcm_tr3_vxlate2vxlate_extd(unit,&vent,&vxent));

    BCM_IF_ERROR_RETURN
        (soc_mem_search(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ALL,
                        &idx, &vxent, &vxent_out, 0));
    vlan_vp->egress_vlan = soc_VLAN_XLATE_EXTDm_field32_get(unit, &vxent_out,
                                                       NEW_OVIDf);
    vlan_vp->egress_inner_vlan = soc_VLAN_XLATE_EXTDm_field32_get(unit, &vxent_out,
                                                       NEW_IVIDf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _tr3_vlan_vp_match_cleanup
 * Purpose:
 *      Clean up match criteria for VLAN VP.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      vlan_vp - (IN) Pointer to VLAN virtual port structure..
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_tr3_vlan_vp_match_cleanup(int unit, bcm_vlan_port_t *vlan_vp)
{
    vlan_xlate_entry_t vent;
    vlan_xlate_extd_entry_t vxent, old_vxent;
    int key_type = 0;
    uint32 profile_idx;
    int rv;

    if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_NONE) {
        return BCM_E_NONE;
    }

    if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN) {
        key_type = bcmVlanTranslateKeyPortOuter;
    } else if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED) {
        key_type = bcmVlanTranslateKeyPortDouble;
    } else if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN16) {
        key_type = bcmVlanTranslateKeyPortOuterTag;
    } else if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_INNER_VLAN) {
        key_type = bcmVlanTranslateKeyPortInner;
    } else {
        return BCM_E_INTERNAL;
    }

    sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));
    sal_memset(&vxent, 0, sizeof(vlan_xlate_extd_entry_t));
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_translate_entry_assemble(unit, &vent,
                                                vlan_vp->port,
                                                key_type,
                                                vlan_vp->match_inner_vlan,
                                                vlan_vp->match_vlan));
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_vxlate2vxlate_extd(unit,&vent,&vxent));

    rv = soc_mem_delete_return_old(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ALL,
                                   &vxent, &old_vxent);
    if ((rv == SOC_E_NONE) &&
        soc_VLAN_XLATE_EXTDm_field32_get(unit, &old_vxent, VALID_0f)) {
        profile_idx = soc_VLAN_XLATE_EXTDm_field32_get(unit, &old_vxent,
                                                  TAG_ACTION_PROFILE_PTRf);
        /* Delete the old vlan action profile entry */
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    return rv;
}
#endif   /* TRIUMPH3 support */

#ifdef BCM_TRIDENT2_SUPPORT

/*
 * Function:
 *      _bcm_td2_vlan_match_vp_replace
 * Purpose:
 *      Replace VP value in VLAN VP's match entry.
 * Parameters:
 *      unit   - (IN) SOC unit number. 
 *      vp     - (IN) VLAN VP whose match entry is being replaced.
 *      new_vp - (IN) New VP value.
 *      old_vp - (OUT) Old VP value.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_td2_vlan_match_vp_replace(int unit, int vp, int new_vp, int *old_vp)
{
    int rv = BCM_E_NONE;
    uint32      vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32      old_vent[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t   mem = VLAN_XLATEm;
    int key_type;
    int entry_index;
    _bcm_tr2_vlan_vp_info_t *vlan_vp = VLAN_VP_INFO(unit, vp);

    switch(vlan_vp->criteria) {
        case BCM_VLAN_PORT_MATCH_NONE:
            return BCM_E_NONE;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
            key_type = bcmVlanTranslateKeyPortOuter;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
            key_type = bcmVlanTranslateKeyPortDouble;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN16:
            key_type = bcmVlanTranslateKeyPortOuterTag;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_INNER_VLAN:
            key_type = bcmVlanTranslateKeyPortInner;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    sal_memset(vent, 0, sizeof(vent));
    sal_memset(old_vent, 0, sizeof(old_vent));
    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }
    /* Construct lookup key */
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_translate_entry_assemble(unit, vent,
                                                vlan_vp->port, key_type,
                                                vlan_vp->match_inner_vlan,
                                                vlan_vp->match_vlan));
    /* Lookup existing entry */
    SOC_IF_ERROR_RETURN(
        soc_mem_search(unit, mem, MEM_BLOCK_ALL,
                       &entry_index, vent, old_vent, 0));
    /* Replace entry's VP value */
    *old_vp = soc_mem_field32_get(unit, mem, old_vent, SOURCE_VPf);
    soc_mem_field32_set(unit, mem, old_vent, SOURCE_VPf, new_vp);

    /* Insert new entry */
    rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, old_vent);
    if (rv == SOC_E_EXISTS) {
        rv = BCM_E_NONE;
    } else {
        return BCM_E_INTERNAL;
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_vlan_vp_source_vp_lag_set
 * Purpose:
 *      Set source VP LAG for a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) VLAN virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_vlan_vp_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
    int rv;
    int vp, old_vp;

    VLAN_VIRTUAL_INIT(unit);

    VLAN_VIRTUAL_LOCK(unit);

    if (!BCM_GPORT_IS_VLAN_PORT(gport)) {
        VLAN_VIRTUAL_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
        VLAN_VIRTUAL_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    /* Set source VP LAG by replacing the SVP field in VLAN VP's
     * match entry with the VP value representing the VP LAG.
     */
    rv = _bcm_td2_vlan_match_vp_replace(unit, vp, vp_lag_vp, &old_vp);
    
    VLAN_VIRTUAL_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_td2_vlan_vp_source_vp_lag_clear
 * Purpose:
 *      Clear source VP LAG for a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) VLAN virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_vlan_vp_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
    int rv;
    int vp, old_vp;

    VLAN_VIRTUAL_INIT(unit);

    VLAN_VIRTUAL_LOCK(unit);

    if (!BCM_GPORT_IS_VLAN_PORT(gport)) {
        VLAN_VIRTUAL_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
        VLAN_VIRTUAL_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    /* Clear source VP LAG by replacing the SVP field in VLAN VP's
     * match entry with the VP value.
     */
    rv = _bcm_td2_vlan_match_vp_replace(unit, vp, vp, &old_vp);

    /* Check that the old VP value matches the VP value representing
     * the VP LAG or has been restored.
     */
    if (BCM_SUCCESS(rv)) {
        if ((old_vp != vp_lag_vp) && (old_vp != vp)) {
            rv = BCM_E_INTERNAL;
        }
    }

    VLAN_VIRTUAL_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_td2_vlan_vp_source_vp_lag_get
 * Purpose:
 *      Get source VP LAG for a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) VLAN virtual port GPORT ID. 
 *      vp_lag_vp - (OUT) VP representing VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_vlan_vp_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp)
{
    int rv;
    int vp;
    uint32      vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32      old_vent[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t   mem = VLAN_XLATEm;
    int key_type;
    int entry_index;
    _bcm_tr2_vlan_vp_info_t *vlan_vp = NULL;

    VLAN_VIRTUAL_INIT(unit);

    VLAN_VIRTUAL_LOCK(unit);

    if (!BCM_GPORT_IS_VLAN_PORT(gport)) {
        VLAN_VIRTUAL_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
        VLAN_VIRTUAL_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    vlan_vp = VLAN_VP_INFO(unit, vp);

    switch(vlan_vp->criteria) {
        case BCM_VLAN_PORT_MATCH_NONE:
            VLAN_VIRTUAL_UNLOCK(unit);
            return BCM_E_NONE;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
            key_type = bcmVlanTranslateKeyPortOuter;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
            key_type = bcmVlanTranslateKeyPortDouble;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN16:
            key_type = bcmVlanTranslateKeyPortOuterTag;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_INNER_VLAN:
            key_type = bcmVlanTranslateKeyPortInner;
            break;
        default:
            VLAN_VIRTUAL_UNLOCK(unit);
            return BCM_E_INTERNAL;
    }

    sal_memset(vent, 0, sizeof(vent));
    sal_memset(old_vent, 0, sizeof(old_vent));
    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }
    /* Construct lookup key */
    rv = _bcm_trx_vlan_translate_entry_assemble(unit, vent,
                                                vlan_vp->port, key_type,
                                                vlan_vp->match_inner_vlan,
                                                vlan_vp->match_vlan);
    if (BCM_FAILURE(rv)) {
        VLAN_VIRTUAL_UNLOCK(unit);
        return rv;
    }

    /* Lookup existing entry */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL,
                        &entry_index, vent, old_vent, 0);
    if (SOC_FAILURE(rv)) {
        VLAN_VIRTUAL_UNLOCK(unit);
        return rv;
    }

    /* Get VP value representing VP LAG */
    *vp_lag_vp = soc_mem_field32_get(unit, mem, old_vent, SOURCE_VPf);
    if (!_bcm_vp_used_get(unit, *vp_lag_vp, _bcmVpTypeVpLag)) {
        VLAN_VIRTUAL_UNLOCK(unit);
        return BCM_E_INTERNAL;
    }

    VLAN_VIRTUAL_UNLOCK(unit);

    return rv;
}

#endif /* BCM_TRIDENT2_SUPPORT */

/*
 * Function:
 *      _tr2_vlan_vp_match_add
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
_tr2_vlan_vp_match_add(int unit, bcm_vlan_port_t *vlan_vp, int vp)
{
    uint32      vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32      old_vent[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t   mem = VLAN_XLATEm;
    int key_type = 0;
    bcm_vlan_action_set_t action;
    uint32 profile_idx;
    int rv;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    bcm_port_t gport_id;
    int src_trk_idx;

    if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_NONE) {
       return BCM_E_NONE;
    }

    switch (vlan_vp->criteria) {
        case BCM_VLAN_PORT_MATCH_PORT:

            rv = _bcm_esw_gport_resolve(unit, vlan_vp->port, &mod_out,
                                &port_out, &trunk_id, &gport_id);
            BCM_IF_ERROR_RETURN(rv);

            /* Get index to source trunk map table */
            BCM_IF_ERROR_RETURN(
                   _bcm_esw_src_mod_port_table_index_get(unit, mod_out,
                     port_out, &src_trk_idx));

            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                        src_trk_idx, SOURCE_VPf, vp);
            BCM_IF_ERROR_RETURN(rv);
            if (SOC_MEM_FIELD_VALID(unit,SOURCE_TRUNK_MAP_TABLEm,SVP_VALIDf)) {
                rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                 src_trk_idx, SVP_VALIDf, 1);
                BCM_IF_ERROR_RETURN(rv);
            }
        break;

        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
            key_type = bcmVlanTranslateKeyPortOuter;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:  
            key_type = bcmVlanTranslateKeyPortDouble;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN16:
            key_type = bcmVlanTranslateKeyPortOuterTag;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_INNER_VLAN:
            key_type = bcmVlanTranslateKeyPortInner;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL:
            key_type = bcmVlanTranslateKeyPortPonTunnel;
            break;
        case  BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_VLAN:
            key_type = bcmVlanTranslateKeyPortPonTunnelOuter;
            break;
        case  BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_INNER_VLAN:
            key_type = bcmVlanTranslateKeyPortPonTunnelInner;
            break;
        default:
            return BCM_E_PARAM;
    }

    if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT) {
        return BCM_E_NONE;
    }

    if ((vlan_vp->egress_vlan > BCM_VLAN_MAX) ||
        (vlan_vp->egress_inner_vlan > BCM_VLAN_MAX)) {
        return BCM_E_PARAM;
    }

    sal_memset(vent, 0, sizeof(vent));
    sal_memset(old_vent, 0, sizeof(old_vent));

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    } else {
        soc_mem_field32_set(unit, mem, vent, VALIDf, 1);
    }

    switch (vlan_vp->criteria) {
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent,
                                                vlan_vp->port,
                                                key_type,
                                                vlan_vp->match_tunnel_value,
                                                0);
            break;
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_VLAN:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent,
                                                vlan_vp->port,
                                                key_type,
                                                vlan_vp->match_tunnel_value,
                                                vlan_vp->match_vlan);
            break;
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_INNER_VLAN:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent,
                                                vlan_vp->port,
                                                key_type,
                                                vlan_vp->match_tunnel_value,
                                                vlan_vp->match_inner_vlan);
            break;
        default:
            rv = _bcm_trx_vlan_translate_entry_assemble(unit, vent,
                                                vlan_vp->port,
                                                key_type,
                                                vlan_vp->match_inner_vlan,
                                                vlan_vp->match_vlan);
            break;
    }

    if(BCM_FAILURE(rv)) {
        return rv;
    }

    bcm_vlan_action_set_t_init(&action);

    if (vlan_vp->flags & BCM_VLAN_PORT_INNER_VLAN_PRESERVE) {
        action.dt_outer = bcmVlanActionReplace;
        action.dt_outer_prio = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionNone;
        action.dt_inner_prio = bcmVlanActionNone;
    } else {
        if (soc_feature(unit, soc_feature_vlan_copy_action)) {
            action.dt_outer = bcmVlanActionCopy;
            action.dt_outer_prio = bcmVlanActionCopy;
        } else {
            action.dt_outer = bcmVlanActionReplace;
            action.dt_outer_prio = bcmVlanActionReplace;
        }
        action.dt_inner = bcmVlanActionDelete;
        action.dt_inner_prio = bcmVlanActionDelete;
    }

    action.ot_outer = bcmVlanActionReplace;
    action.ot_outer_prio = bcmVlanActionReplace;
    if (vlan_vp->flags & BCM_VLAN_PORT_INNER_VLAN_ADD) {
        action.ot_inner = bcmVlanActionAdd;
    } else {
        action.ot_inner = bcmVlanActionNone;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_add(unit, &action, &profile_idx));

    if ((vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL) ||
        (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_VLAN) ||
        (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_INNER_VLAN)) {
        soc_mem_field32_set(unit, mem, vent, LLTAG__MPLS_ACTIONf, 0x1); /* SVP */
        soc_mem_field32_set(unit, mem, vent, LLTAG__SOURCE_VPf, vp);
        soc_mem_field32_set(unit, mem, vent, LLTAG__NEW_OVIDf,
                                vlan_vp->egress_vlan);
        soc_mem_field32_set(unit, mem, vent, LLTAG__NEW_IVIDf,
                                vlan_vp->egress_inner_vlan);
        soc_mem_field32_set(unit, mem, vent, LLTAG__TAG_ACTION_PROFILE_PTRf,
                                profile_idx);
        if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_TYPEf)) {
            soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        }
    } else {
        soc_mem_field32_set(unit, mem, vent, MPLS_ACTIONf, 0x1); /* SVP */
        soc_mem_field32_set(unit, mem, vent, SOURCE_VPf, vp);
        soc_mem_field32_set(unit, mem, vent, NEW_OVIDf,
                                vlan_vp->egress_vlan);
        soc_mem_field32_set(unit, mem, vent, NEW_IVIDf,
                                vlan_vp->egress_inner_vlan);
        soc_mem_field32_set(unit, mem, vent, TAG_ACTION_PROFILE_PTRf,
                                profile_idx);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, VLAN_ACTION_VALIDf)) {
        soc_mem_field32_set(unit, mem, vent, VLAN_ACTION_VALIDf, 1);
    }

    rv = soc_mem_insert_return_old(unit, mem, MEM_BLOCK_ALL,
                                   vent, old_vent);
    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_mem_field32_get(unit, mem, old_vent,
                                          TAG_ACTION_PROFILE_PTRf);
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    return rv;
}

/*
 * Function:
 *      _tr2_vlan_vp_match_delete
 * Purpose:
 *      Delete match criteria for VLAN VP.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vp - (IN) Virtual port number.
 *      old_vp - (OUT) Pointer to the old virtual port number of the entry.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_tr2_vlan_vp_match_delete(int unit, bcm_vlan_port_t *vlan_vp, int vp, int *old_vp)
{
    uint32      vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32      old_vent[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t   mem = VLAN_XLATEm;
    int key_type = 0;
    uint32 profile_idx;
    int rv;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    bcm_port_t gport_id;
    int src_trk_idx;

    if (vlan_vp && (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT)) {
        rv = _bcm_esw_gport_resolve(unit, vlan_vp->port, &mod_out,
                                &port_out, &trunk_id, &gport_id);
        BCM_IF_ERROR_RETURN(rv);

            /* Get index to source trunk map table */
            BCM_IF_ERROR_RETURN(
                   _bcm_esw_src_mod_port_table_index_get(unit, mod_out,
                     port_out, &src_trk_idx));

            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                        src_trk_idx, SOURCE_VPf, 0);
            BCM_IF_ERROR_RETURN(rv);
            if (SOC_MEM_FIELD_VALID(unit,SOURCE_TRUNK_MAP_TABLEm,SVP_VALIDf)) {
                rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                 src_trk_idx, SVP_VALIDf, 0);
                BCM_IF_ERROR_RETURN(rv);
            }
        return BCM_E_NONE;
    } else if (VLAN_VP_INFO(unit, vp)->criteria == BCM_VLAN_PORT_MATCH_NONE) {
        return BCM_E_NONE;
    }

    switch (VLAN_VP_INFO(unit, vp)->criteria) {
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
            key_type = bcmVlanTranslateKeyPortOuter;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:  
            key_type = bcmVlanTranslateKeyPortDouble;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN16:
            key_type = bcmVlanTranslateKeyPortOuterTag;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_INNER_VLAN:
            key_type = bcmVlanTranslateKeyPortInner;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL:
            key_type = bcmVlanTranslateKeyPortPonTunnel;
            break;
        case  BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_VLAN:
            key_type = bcmVlanTranslateKeyPortPonTunnelOuter;
            break;
        case  BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_INNER_VLAN:
            key_type = bcmVlanTranslateKeyPortPonTunnelInner;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    sal_memset(vent, 0, sizeof(vent));
    sal_memset(old_vent, 0, sizeof(old_vent));
    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    switch (VLAN_VP_INFO(unit, vp)->criteria) {
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent,
                                                VLAN_VP_INFO(unit, vp)->port,
                                                key_type,
                                                VLAN_VP_INFO(unit, vp)->match_tunnel_value,
                                                0);
            break;
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_VLAN:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent,
                                                VLAN_VP_INFO(unit, vp)->port,
                                                key_type,
                                                VLAN_VP_INFO(unit, vp)->match_tunnel_value,
                                                VLAN_VP_INFO(unit, vp)->match_vlan); 
            break;
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_INNER_VLAN:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent,
                                                VLAN_VP_INFO(unit, vp)->port,
                                                key_type,
                                                VLAN_VP_INFO(unit, vp)->match_tunnel_value,
                                                VLAN_VP_INFO(unit, vp)->match_inner_vlan);
            break;
        default:
            rv = _bcm_trx_vlan_translate_entry_assemble(unit, vent,
                                                VLAN_VP_INFO(unit, vp)->port,
                                                key_type,
                                                VLAN_VP_INFO(unit, vp)->match_inner_vlan,
                                                VLAN_VP_INFO(unit, vp)->match_vlan);
            break;
    }

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    rv = soc_mem_delete_return_old(unit, mem, MEM_BLOCK_ALL, vent, old_vent);
    if (rv == SOC_E_NONE) {
        int is_valid = 0;
        if (soc_feature(unit, soc_feature_base_valid)) {
            is_valid =
                soc_mem_field32_get(unit, mem, old_vent, BASE_VALID_0f) == 3 &&
                soc_mem_field32_get(unit, mem, old_vent, BASE_VALID_1f) == 7;
        } else {
            is_valid = soc_mem_field32_get(unit, mem, old_vent, VALIDf);
        }
        if (is_valid) {
            if (old_vp != NULL) {
                *old_vp = soc_mem_field32_get(unit, mem, old_vent, SOURCE_VPf);
            }
            profile_idx = soc_mem_field32_get(unit, mem, old_vent,
                                              TAG_ACTION_PROFILE_PTRf);
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        }
    }

  /* If the associated the VLAN_XLATE entry was deleted by translation action delete, 
    * it should return NOT FOUND.
    */
    if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_NONE;
    }
    return rv;
}

/*
 * Function:
 *      _tr2_vlan_vp_match_get
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
_tr2_vlan_vp_match_get(int unit, int vp, bcm_vlan_port_t *vlan_vp)
{
    uint32      vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32      vent_out[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t   mem = VLAN_XLATEm;

    int key_type = 0;
    int idx;
    int rv;

    vlan_vp->criteria = VLAN_VP_INFO(unit, vp)->criteria;
    vlan_vp->match_vlan = VLAN_VP_INFO(unit, vp)->match_vlan;
    vlan_vp->match_inner_vlan = VLAN_VP_INFO(unit, vp)->match_inner_vlan;
    vlan_vp->match_tunnel_value = VLAN_VP_INFO(unit, vp)->match_tunnel_value;
    vlan_vp->port = VLAN_VP_INFO(unit, vp)->port;

    if (VLAN_VP_INFO(unit, vp)->criteria == BCM_VLAN_PORT_MATCH_NONE) {
        return BCM_E_NONE;
    }

    switch (vlan_vp->criteria) {
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
            key_type = bcmVlanTranslateKeyPortOuter;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
            key_type = bcmVlanTranslateKeyPortDouble;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN16:
            key_type = bcmVlanTranslateKeyPortOuterTag;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_INNER_VLAN:
            key_type = bcmVlanTranslateKeyPortInner;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL:
            key_type = bcmVlanTranslateKeyPortPonTunnel;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_VLAN:
            key_type = bcmVlanTranslateKeyPortPonTunnelOuter;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_INNER_VLAN:
            key_type = bcmVlanTranslateKeyPortPonTunnelInner;
            break;
        default:
            return BCM_E_PARAM;
    }

    sal_memset(vent, 0, sizeof(vent));
    sal_memset(vent_out, 0, sizeof(vent_out));
    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    switch (vlan_vp->criteria) {
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL:
           rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent,
                                               vlan_vp->port,
                                               key_type,
                                               vlan_vp->match_tunnel_value,
                                               0);
           break;
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_VLAN:
           rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent,
                                               vlan_vp->port,
                                               key_type,
                                               vlan_vp->match_tunnel_value,
                                               vlan_vp->match_vlan);
           break;
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_INNER_VLAN:
           rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent,
                                               vlan_vp->port,
                                               key_type,
                                               vlan_vp->match_tunnel_value,
                                               vlan_vp->match_inner_vlan);
           break;
        default:
           rv = _bcm_trx_vlan_translate_entry_assemble(unit, vent,
                                               vlan_vp->port,
                                               key_type,
                                               vlan_vp->match_inner_vlan,
                                               vlan_vp->match_vlan);
           break;
        }

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    BCM_IF_ERROR_RETURN
        (soc_mem_search(unit, mem, MEM_BLOCK_ALL,
                        &idx, vent, vent_out, 0));
    vlan_vp->egress_vlan = soc_mem_field32_get(unit, mem, vent_out, NEW_OVIDf);
    vlan_vp->egress_inner_vlan = soc_mem_field32_get(unit, mem, vent_out,
                                                     NEW_IVIDf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _tr2_vlan_vp_match_cleanup
 * Purpose:
 *      Clean up match criteria for VLAN VP.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      vlan_vp - (IN) Pointer to VLAN virtual port structure.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_tr2_vlan_vp_match_cleanup(int unit, bcm_vlan_port_t *vlan_vp)
{
    uint32      vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32      old_vent[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t   mem = VLAN_XLATEm;

    int key_type = 0;
    uint32 profile_idx;
    int rv;

    if (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_NONE) {
        return BCM_E_NONE;
    }

    switch (vlan_vp->criteria) {
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
            key_type = bcmVlanTranslateKeyPortOuter;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
            key_type = bcmVlanTranslateKeyPortDouble;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN16:
            key_type = bcmVlanTranslateKeyPortOuterTag;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_INNER_VLAN:
            key_type = bcmVlanTranslateKeyPortInner;
            break;
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL:
            key_type = bcmVlanTranslateKeyPortPonTunnel;
            break;
        case  BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_VLAN:
            key_type = bcmVlanTranslateKeyPortPonTunnelOuter;
            break;
        case  BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_INNER_VLAN:
            key_type = bcmVlanTranslateKeyPortPonTunnelInner;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    sal_memset(vent, 0, sizeof(old_vent));
    sal_memset(old_vent, 0, sizeof(old_vent));
    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    switch (vlan_vp->criteria) {
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent,
                                                vlan_vp->port,
                                                key_type,
                                                vlan_vp->match_tunnel_value,
                                                0);
            break;
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_VLAN:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent,
                                                vlan_vp->port,
                                                key_type,
                                                vlan_vp->match_tunnel_value,
                                                vlan_vp->match_vlan);
            break;
        case BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_INNER_VLAN:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent,
                                                vlan_vp->port,
                                                key_type,
                                                vlan_vp->match_tunnel_value,
                                                vlan_vp->match_inner_vlan);
            break;
        default:
            rv = _bcm_trx_vlan_translate_entry_assemble(unit, vent,
                                                vlan_vp->port,
                                                key_type,
                                                vlan_vp->match_inner_vlan,
                                                vlan_vp->match_vlan);
            break;
    }

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    rv = soc_mem_delete_return_old(unit, mem, MEM_BLOCK_ALL,
                                   vent, old_vent);
    if (rv == SOC_E_NONE) {
        int is_valid = 0;
        if (soc_feature(unit, soc_feature_base_valid)) {
            is_valid =
                soc_mem_field32_get(unit, mem, old_vent, BASE_VALID_0f) == 3 &&
                soc_mem_field32_get(unit, mem, old_vent, BASE_VALID_1f) == 7;
        } else {
            is_valid = soc_mem_field32_get(unit, mem, old_vent, VALIDf);
        }
        if (is_valid) {
            profile_idx = soc_mem_field32_get(unit, mem, old_vent,
                                              TAG_ACTION_PROFILE_PTRf);
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        }
    }

   /* If the associated the VLAN_XLATE entry was deleted by translation action delete,
    * it should return NOT FOUND.
    */
    if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_NONE;
    }
    return rv;
}

STATIC int
_bcm_tr2_vlan_vp_match_add(int unit, bcm_vlan_port_t *vlan_vp, int vp)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _tr3_vlan_vp_match_add(unit, vlan_vp, vp);
    } else
#endif
    {
        return _tr2_vlan_vp_match_add(unit, vlan_vp, vp);
    }
}

STATIC int
_bcm_tr2_vlan_vp_match_delete(int unit, bcm_vlan_port_t *vlan_vp, int vp, int *old_vp)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _tr3_vlan_vp_match_delete(unit, vp);
    } else
#endif
    {
        return _tr2_vlan_vp_match_delete(unit, vlan_vp, vp, old_vp);
    }
}

STATIC int
_bcm_tr2_vlan_vp_match_get(int unit, int vp, bcm_vlan_port_t *vlan_vp)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _tr3_vlan_vp_match_get(unit, vp, vlan_vp);
    } else
#endif
    {
        return _tr2_vlan_vp_match_get(unit, vp, vlan_vp);
    }
}

STATIC int
_bcm_tr2_vlan_vp_match_cleanup(int unit, bcm_vlan_port_t *vlan_vp)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _tr3_vlan_vp_match_cleanup(unit, vlan_vp);
    } else
#endif
    {
        return _tr2_vlan_vp_match_cleanup(unit, vlan_vp);
    }
}

#ifdef BCM_TRIUMPH3_SUPPORT

/*
 * Function:
 *      _bcm_tr3_vlan_eline_vp_map_set
 * Purpose:
 *      Set VLAN ELINE ports from VPN
 * Parameters:
 *      unit      - (IN) Device Number
 *      vfi_index - (IN) VFI Index
 *      vp1       - (IN) VP1
 *      vp1       - (IN) VP2
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_vlan_eline_vp_map_set(int unit, int vfi_index, int vp1, int vp2)
{
    vfi_entry_t vfi_entry;
    int num_vp=0;

    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    if ((vp1 < 0) || (vp1 >= num_vp) ||
        (vp2 < 0) || (vp2 >= num_vp)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        soc_VFIm_field32_set(unit, &vfi_entry, VP_0f, vp1);
        soc_VFIm_field32_set(unit, &vfi_entry, VP_1f, vp2);
    } else {
        /* ELAN */
        return BCM_E_PARAM;
    }

    return WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry);
}

/*
 * Function:
 *      _bcm_tr3_vlan_eline_port_add
 * Purpose:
 *      Add VLAN ELINE port to a VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      vlan_port - (IN) VLAN port information
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_vlan_eline_port_add(int unit, bcm_vpn_t vpn, bcm_vlan_port_t *vlan_port)
{
    int active_vp = 0; /* default VP */
    source_vp_entry_t svp;
    int vfi_index = -1;

    if (vpn != _BCM_VLAN_VPN_INVALID) {
        _BCM_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
        if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVlan)) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        vfi_index = _BCM_VLAN_VFI_INVALID;
    }

    active_vp = BCM_GPORT_VLAN_PORT_ID_GET(vlan_port->vlan_port_id);
    if (active_vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, active_vp, _bcmVpTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, active_vp, &svp));
    (void) _bcm_tr3_vlan_eline_vp_map_set(unit, vfi_index, active_vp, 0);

    if (vlan_port->flags & BCM_VLAN_PORT_NETWORK) {
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 1);
        soc_SOURCE_VPm_field32_set(unit, &svp, TPID_SOURCEf, 0x0);
    } else {
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 0);
        soc_SOURCE_VPm_field32_set(unit, &svp, TPID_SOURCEf, 0x2);
    }

    soc_SOURCE_VPm_field32_set(unit, &svp, DISABLE_VLAN_CHECKSf, 1);
    soc_SOURCE_VPm_field32_set(unit, &svp,
                           ENTRY_TYPEf, 0x1); /* VFI Type */
    soc_SOURCE_VPm_field32_set(unit, &svp, VFIf, vfi_index);

    return WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, active_vp, &svp);
}

/*
 * Function:
 *     _bcm_tr3_vlan_elan_port_add
 * Purpose:
 *      Add VLAN ELAN port to a VPN
 * Parameters:
 *   unit - (IN) Device Number
 *   vpn - (IN) VPN instance ID
 *   vlan_vp - (IN) vlan port information
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_tr3_vlan_elan_port_add(int unit, bcm_vpn_t vpn, bcm_vlan_port_t *vlan_vp)
{
    int vp, vfi = 0;
    source_vp_entry_t svp;

    if (vpn != _BCM_VLAN_VPN_INVALID) {
        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeVlan)) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        vfi = _BCM_VLAN_VFI_INVALID;
    }

    vp = BCM_GPORT_VLAN_PORT_ID_GET(vlan_vp->vlan_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));


    /* ======== Configure SVP/DVP Properties ========== */
    if (vpn == _BCM_VLAN_VPN_INVALID) {
        soc_SOURCE_VPm_field32_set(unit, &svp, ENTRY_TYPEf, 0x0);
    } else {
        /* Initialize VP parameters */
        soc_SOURCE_VPm_field32_set(unit, &svp, ENTRY_TYPEf, 0x1);
    }

    soc_SOURCE_VPm_field32_set(unit, &svp, VFIf, vfi);
    if (vlan_vp->flags & BCM_VLAN_PORT_NETWORK) {
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 1);
        soc_SOURCE_VPm_field32_set(unit, &svp, TPID_SOURCEf, 0x0);
    } else {
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 0);
        soc_SOURCE_VPm_field32_set(unit, &svp, TPID_SOURCEf, 0x2);
    }

    if (soc_mem_field_valid(unit, SOURCE_VPm, DISABLE_VLAN_CHECKSf)) {
        soc_SOURCE_VPm_field32_set(unit, &svp, DISABLE_VLAN_CHECKSf, 1);
    }

    return WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
}

/* Function:
  *       _bcm_tr3_vlan_vpn_is_valid
  * Purpose:
  *       Find if given VLAN VPN is Valid
  * Parameters:
  *       unit     - Device Number
  *       vpn      - VLAN VPN
  * Returns:
  *       BCM_E_XXXX
*/
int
_bcm_tr3_vlan_vpn_is_valid( int unit, bcm_vpn_t vpn)
{
    bcm_vpn_t vpn_min=0;
    int vfi_index=-1, num_vfi=0;

    num_vfi = soc_mem_index_count(unit, VFIm);
    /* Check for Valid vpn */
    _BCM_VPN_SET(vpn_min, _BCM_VPN_TYPE_VFI, 0);
    if (vpn < vpn_min || vpn > (vpn_min + num_vfi - 1)) {
        return BCM_E_PARAM;
    }

    _BCM_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/* Function:
 *      _bcm_tr3_vlan_vpn_is_eline
 * Purpose:
 *      Find if given VLAN VPN is ELINE
 * Parameters:
 *      unit     - Device Number
 *      vpn      - VLAN VPN
 *      isEline  - Flag
 * Returns:
 *      BCM_E_XXXX
 * Assumes:
 *      l2vpn is valid
 */

int
_bcm_tr3_vlan_vpn_is_eline( int unit, bcm_vpn_t vpn, uint8 *isEline)
{
    int vfi_index=-1;
    vfi_entry_t vfi_entry;

    BCM_IF_ERROR_RETURN(_bcm_tr3_vlan_vpn_is_valid(unit, vpn));

    _BCM_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
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
 *      _bcm_tr3_vlan_port_add
 * Purpose:
 *      Add a VLAN port to the specified VPN.
 * Parameters:
 *      unit  - (IN) SOC unit number.
 *      vlan  - (IN) VLAN ID to add virtual port to as a member.
 *      vlan_vp - (IN) VP port information
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_vlan_port_add(int unit, bcm_vpn_t vpn,
                            bcm_vlan_port_t *vlan_vp)
{
    int mode=0, rv = BCM_E_PARAM;
    uint8 isEline=0xFF;

    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_egress_mode_get(unit, &mode));
    if (!mode) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 egress mode must be set first\n")));
        return BCM_E_DISABLED;
    }

    if ((vpn != _BCM_VLAN_VPN_INVALID) && (isEline != 0x0)) {
        BCM_IF_ERROR_RETURN
              (_bcm_tr3_vlan_vpn_is_eline(unit, vpn, &isEline));
    }

    if (isEline == 0x1) {
        rv = _bcm_tr3_vlan_eline_port_add(unit, vpn, vlan_vp);
    } else if (isEline == 0x0) {
        rv = _bcm_tr3_vlan_elan_port_add(unit, vpn, vlan_vp);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_vlan_eline_vp_map_get
 * Purpose:
 *      Get VLAN ELINE ports from VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_vlan_eline_vp_map_get(int unit, int vfi_index, int *vp1, int *vp2)
{
    vfi_entry_t vfi_entry;

    if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
        *vp1 = soc_VFIm_field32_get(unit, &vfi_entry, VP_0f);
        *vp2 = soc_VFIm_field32_get(unit, &vfi_entry, VP_1f);
        return BCM_E_NONE;
    }

    return BCM_E_PARAM;
}

/* Function:
 *      _bcm_tr3_vlan_vp_is_eline
 * Purpose:
 *      Find if given VLAN VP is ELINE
 * Parameters:
 *      unit     - Device Number
 *      vp        - VLAN VP
 *      isEline  - Flag
 * Returns:
 *      BCM_E_XXXX
 * Assumes:
 *      vp is valid
 */
STATIC int
_bcm_tr3_vlan_vp_is_eline( int unit, int vp, uint8 *isEline)
{
    source_vp_entry_t svp;
    vfi_entry_t vfi_entry;
    int vfi_index;

    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));

    vfi_index = soc_SOURCE_VPm_field32_get(unit, &svp, VFIf);
    BCM_IF_ERROR_RETURN
        (READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));

    /* Set the returned VPN id */
    if (soc_VFIm_field32_get(unit, &vfi_entry, PT2PT_ENf)) {
       *isEline = 0x1;
    } else {
       *isEline = 0x0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_vlan_eline_port_delete
 * Purpose:
 *      Delete VLAN ELINE port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      active_vp - (IN) Active vp
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_vlan_eline_port_delete(int unit, bcm_vpn_t vpn, int active_vp)
{
    source_vp_entry_t svp;
    int vp1=0, vp2=0, vfi_index= -1;

    if (vpn != _BCM_VLAN_VPN_INVALID) {
        _BCM_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
        if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVlan)) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        vfi_index = _BCM_VLAN_VFI_INVALID;
    }

    if (!_bcm_vp_used_get(unit, active_vp, _bcmVpTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }

    /* ---- Read in current table values for VP1 and VP2 ----- */
    (void) _bcm_tr3_vlan_eline_vp_map_get(unit, vfi_index, &vp1, &vp2);

    /* If the other port is valid, point it to itself */
    if (active_vp == vp1) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_vlan_eline_vp_map_set(unit, vfi_index, 0, vp2));
    } else if (active_vp == vp2) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_vlan_eline_vp_map_set(unit, vfi_index, vp1, 0));
    }

    /* Check for Network-Port */
    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, active_vp, &svp));

    /* Invalidate the VP being deleted */
    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    return WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, active_vp, &svp);
}

/*
 * Function:
 *      _bcm_tr3_vlan_elan_port_delete
 * Purpose:
 *      Delete VLAN ELAN port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn       - (IN) VPN instance ID
 *      vp         - (IN) vxlan port information
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_vlan_elan_port_delete(int unit, bcm_vpn_t vpn, int vp)
{
    source_vp_entry_t svp;
    int network_port_flag=0;
    int vfi_index= -1;

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
    network_port_flag = soc_SOURCE_VPm_field32_get(unit, &svp, NETWORK_PORTf);
    if (vpn != _BCM_VLAN_VPN_INVALID) {
        _BCM_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
        if (!network_port_flag) {
            if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVlan)) {
                return BCM_E_NOT_FOUND;
            }
        }

        if (!((soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) == 0x1) &&
            (vfi_index == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf)))) {
            return BCM_E_PARAM;
        }
    }

    /* Clear the SVP table entries */
    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    return WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
}

/*
 * Function:
 *      _bcm_tr3_vlan_port_delete
 * Purpose:
 *      Delete VLAN port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn       - (IN) VPN instance ID
 *      vp      - (IN) vlan port
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_vlan_port_delete(int unit, bcm_vpn_t vpn, int vp)
{
    int rv = BCM_E_UNAVAIL;
    uint8 isEline=0;

    if (vpn == _BCM_VLAN_VPN_INVALID) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_vlan_vpn_is_valid(unit, vpn));
    BCM_IF_ERROR_RETURN(_bcm_tr3_vlan_vp_is_eline(unit, vp, &isEline));
    if (isEline == 0x1) {
        rv = _bcm_tr3_vlan_eline_port_delete(unit, vpn, vp);
    } else if (isEline == 0x0) {
        rv = _bcm_tr3_vlan_elan_port_delete(unit, vpn, vp);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_vlan_port_delete_all
 * Purpose:
 *      Delete all VLAN ports from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_vlan_port_delete_all(int unit, bcm_vpn_t vpn)
{
    int vfi_index=0;
    int vp1 = 0, vp2 = 0;
    uint8 isEline=0xFF;
    uint32 vp=0;

    BCM_IF_ERROR_RETURN
         (_bcm_tr3_vlan_vpn_is_eline(unit, vpn, &isEline));

    if (isEline == 0x1) {
        if (vpn != _BCM_VLAN_VPN_INVALID) {
            _BCM_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
            if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVlan)) {
                return BCM_E_NOT_FOUND;
            }
        } else {
            vfi_index = _BCM_VLAN_VFI_INVALID;
        }

        /* ---- Read in current table values for VP1 and VP2 ----- */
        (void) _bcm_tr3_vlan_eline_vp_map_get(unit, vfi_index, &vp1, &vp2);
        if (vp1 != 0) {
            BCM_IF_ERROR_RETURN(_bcm_tr3_vlan_eline_port_delete(unit, vpn, vp1));
        }
        if (vp2 != 0) {
            BCM_IF_ERROR_RETURN(_bcm_tr3_vlan_eline_port_delete(unit, vpn, vp2));
        }
    } else if (isEline == 0x0) {
        uint32 vfi, num_vp;
        source_vp_entry_t svp;

        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeVlan)) {
            return BCM_E_NOT_FOUND;
        }
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);

        SHR_BIT_ITER(VIRTUAL_INFO(unit)->vlan_vp_bitmap, num_vp, vp) {

            BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
            if ((soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) == 0x1) &&
                (vfi == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf))) {
                BCM_IF_ERROR_RETURN(_bcm_tr3_vlan_port_delete(unit, vpn, vp));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_vlan_port_get_all
 * Purpose:
 *      Get all VLAN ports from a VPN
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      vlan      - (IN) VLAN ID.
 *      array_max - (IN) Max number of elements in the array.
 *      gport_array - (OUT) Array of virtual ports in GPORT format.
 *      flags_array - (OUT) Array of flags.
 *      array_size  - (OUT) Actual number of elements in the array.
 */
STATIC int
_bcm_tr3_vlan_port_get_all(int unit, bcm_vlan_t vpn, int array_max,
              bcm_gport_t *gport_array, int *flags_array, int *array_size)
{
    int vp;
    int vfi_index=-1;
    int vp1 = 0, vp2 = 0;
    uint8 isEline=0xFF;

    if (0 == array_max) {
        return BCM_E_NONE;
    }

    *array_size = 0;

    BCM_IF_ERROR_RETURN
         (_bcm_tr3_vlan_vpn_is_eline(unit, vpn, &isEline));

    if (isEline == 0x1) {
        if (vpn != _BCM_VLAN_VPN_INVALID) {
            _BCM_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
            if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVlan)) {
                return BCM_E_NOT_FOUND;
            }
        } else {
            vfi_index = _BCM_VLAN_VFI_INVALID;
        }

        /* ---- Read in current table values for VP1 and VP2 ----- */
        (void) _bcm_tr3_vlan_eline_vp_map_get (unit, vfi_index, &vp1, &vp2);
        if (_bcm_vp_used_get(unit, vp1, _bcmVpTypeVlan)) {
            BCM_GPORT_VLAN_PORT_ID_SET(gport_array[*array_size], vp1);
            flags_array[*array_size] = 0;
            (*array_size)++;
        }

        if (_bcm_vp_used_get(unit, vp2, _bcmVpTypeVlan)) {
            if (array_max > *array_size) {
                BCM_GPORT_VLAN_PORT_ID_SET(gport_array[*array_size], vp2);
                flags_array[*array_size] = 0;
                (*array_size)++;
            }
        }
    } else if (isEline == 0x0) {
        uint32 vfi, entry_type;
        int num_vp;
        source_vp_entry_t svp;

        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeVlan)) {
            return BCM_E_NOT_FOUND;
        }
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        SHR_BIT_ITER(VIRTUAL_INFO(unit)->vlan_vp_bitmap, num_vp, vp) {
            BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
            entry_type = soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf);

            if ((vfi == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf)) &&
                (entry_type == 0x1)) {
                if (array_max > *array_size) {
                    BCM_GPORT_VLAN_PORT_ID_SET(gport_array[*array_size], vp);
                    flags_array[*array_size] = 0;
                    (*array_size)++;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_vlan_vp_destroy_all
 * Purpose:
 *      Destroy all virtual port in VPN instance.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      vpn - (IN) VPN instance ID.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_tr3_vlan_vp_destroy_all(int unit, bcm_vpn_t vpn)
{
    int vfi_index=0;
    int vp1 = 0, vp2 = 0;
    uint8 isEline=0xFF;
    uint32 vp=0;
    bcm_gport_t vlan_port_id;

    BCM_IF_ERROR_RETURN
         (_bcm_tr3_vlan_vpn_is_eline(unit, vpn, &isEline));

    if (isEline == 0x1) {
        if (vpn != _BCM_VLAN_VPN_INVALID) {
            _BCM_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
            if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeVlan)) {
                return BCM_E_NOT_FOUND;
            }
        } else {
            vfi_index = _BCM_VLAN_VFI_INVALID;
        }

        /* ---- Read in current table values for VP1 and VP2 ----- */
        (void) _bcm_tr3_vlan_eline_vp_map_get(unit, vfi_index, &vp1, &vp2);
        if (vp1 != 0) {
            BCM_GPORT_VLAN_PORT_ID_SET(vlan_port_id, vp1);
            BCM_IF_ERROR_RETURN(bcm_tr2_vlan_vp_destroy(unit, vlan_port_id));
        }
        if (vp2 != 0) {
            BCM_GPORT_VLAN_PORT_ID_SET(vlan_port_id, vp2);
            BCM_IF_ERROR_RETURN(bcm_tr2_vlan_vp_destroy(unit, vlan_port_id));
        }
    } else if (isEline == 0x0) {
        uint32 vfi, num_vp;
        source_vp_entry_t svp;

        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeVlan)) {
            return BCM_E_NOT_FOUND;
        }
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);

        SHR_BIT_ITER(VIRTUAL_INFO(unit)->vlan_vp_bitmap, num_vp, vp) {
            BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
            if ((soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) == 0x1) &&
                (vfi == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf))) {
                BCM_GPORT_VLAN_PORT_ID_SET(vlan_port_id, vp);
                BCM_IF_ERROR_RETURN(bcm_tr2_vlan_vp_destroy(unit, vlan_port_id));
            }
        }
    }

    return BCM_E_NONE;
}

#endif /* BCM_TRIUMPH3_SUPPORT */

/*
 * Function:
 *      _bcm_tr2_vlan_vp_create
 * Purpose:
 *      Create a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan_vp - (IN/OUT) Pointer to VLAN virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr2_vlan_vp_create(int unit,
                         bcm_vlan_port_t *vlan_vp)
{
    int mode;
    int vp, old_vp = 0, matched_vp;
    int rv = BCM_E_NONE;
    int num_vp;
    int nh_index = 0;
    ing_dvp_table_entry_t dvp_entry;
    source_vp_entry_t svp_entry;
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;
    _bcm_vp_info_t vp_info;
    int tpid_enable = 0, tpid_index=-1;
    int policer_set = 0;
    int network_group;
    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeVlan;

    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_egress_mode_get(unit, &mode));
    if (!mode) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 egress mode must be set first\n")));
        return BCM_E_DISABLED;
    }

    if (((vlan_vp->flags &  BCM_VLAN_PORT_EGRESS_PON_TUNNEL_NONE) || 
        (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL) ||
        (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_VLAN) ||
        (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_INNER_VLAN)) && 
        (!soc_feature(unit, soc_feature_lltag)))  {
        return BCM_E_UNAVAIL;
    } else if ((vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL) ||
        (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_VLAN) ||
        (vlan_vp->criteria == BCM_VLAN_PORT_MATCH_PORT_PON_TUNNEL_INNER_VLAN)) {
        if (vlan_vp->match_tunnel_value >= BCM_VLAN_INVALID) {
            return BCM_E_PARAM;
        }
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
        rv = _bcm_tr2_vlan_vp_nh_info_set(unit, vlan_vp, vp, 0,
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

        rv = _bcm_vp_ing_dvp_to_ing_dvp2(unit, &dvp_entry, vp);
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

#if defined(BCM_TRIUMPH3_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_vfi) && (vlan_vp->flags & BCM_VLAN_PORT_SERVICE_TAGGED)) {
            BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_lkup(unit,
                            vlan_vp->egress_service_tpid, &tpid_index));
            tpid_enable = 1 << tpid_index;
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, TPID_ENABLEf, tpid_enable);
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, SD_TAG_MODEf, 1);
        }
#endif /*BCM_TRIUMPH3_SUPPORT*/

        if (vlan_vp->policer_id != 0) {
            if (soc_feature(unit, soc_feature_global_meter)) {
                rv = _bcm_esw_add_policer_to_table(unit, vlan_vp->policer_id,
                    SOURCE_VPm, vp, &svp_entry);
                policer_set = 1;
            } else {
                rv = BCM_E_PARAM;
            }
            if (rv < 0) {
                goto cleanup;
            }
        }
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            if (vlan_vp->flags & BCM_VLAN_PORT_USE_SGLP_TPID) {
                /* Use TPIDs based on SGLP */
                soc_SOURCE_VPm_field32_set(unit, &svp_entry, TPID_SOURCEf, 2);
            }
        }
#endif
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry);
        if (rv < 0) {
            goto cleanup;
        }

        /* Configure ingress VLAN translation table */
        rv = _bcm_tr2_vlan_vp_match_add(unit, vlan_vp, vp);
        if (rv < 0) {
            goto cleanup;
        }

        /* Increment port's VP count */
        rv = _bcm_tr2_vlan_vp_port_cnt_update(unit, vlan_vp->port, vp, TRUE);
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

        if (soc_feature(unit, soc_feature_global_meter)) {
           /* read Source_VP mem, add, write back */
            BCM_IF_ERROR_RETURN(
                READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
            BCM_IF_ERROR_RETURN(
                _bcm_esw_add_policer_to_table(unit, vlan_vp->policer_id,
                    SOURCE_VPm, vp, &svp_entry));
#if defined(BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit)) {
                if (vlan_vp->flags & BCM_VLAN_PORT_USE_SGLP_TPID) {
                    /* Use TPIDs based on SGLP */
                    soc_SOURCE_VPm_field32_set(unit, &svp_entry, TPID_SOURCEf, 2);
                } else {
                    if(soc_SOURCE_VPm_field32_get(unit, &svp_entry, TPID_SOURCEf) == 2) {
                        soc_SOURCE_VPm_field32_set(unit, &svp_entry, TPID_SOURCEf, 0);
                    }
                }
            }
#endif
            BCM_IF_ERROR_RETURN(
                WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
        } else {
            if (vlan_vp->policer_id != 0) {
                return BCM_E_PARAM;
            }
        }

        /* For existing vlan vp, NH entry already exists */
        BCM_IF_ERROR_RETURN
            (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));

        nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
                NEXT_HOP_INDEXf);

        /* Update existing next hop entries */
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_vlan_vp_nh_info_set(unit, vlan_vp, vp, 0,
                                              &nh_index));

        /* Delete old ingress VLAN translation entry,
         * install new ingress VLAN translation entry
         */
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_vlan_vp_match_delete(unit, NULL, vp, &old_vp));
#ifdef BCM_TRIDENT2_SUPPORT
        /* The vlan port id may be one of members of a vplag. */
        if (_bcm_vp_used_get(unit, old_vp, _bcmVpTypeVpLag)) {
            matched_vp = old_vp;
        } else
#endif
        {
            matched_vp = vp;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_vlan_vp_match_add(unit, vlan_vp, matched_vp));

        /* Decrement old port's VP count, increment new port's VP count */
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_vlan_vp_port_cnt_update(unit,
                VLAN_VP_INFO(unit, vp)->port, vp, FALSE));

        BCM_IF_ERROR_RETURN
            (_bcm_tr2_vlan_vp_port_cnt_update(unit,
                vlan_vp->port, vp, TRUE));
    }
    /* Handle Split Horizon */

    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
        network_group = vlan_vp->ingress_network_group_id;
        rv = _bcm_validate_splithorizon_network_group(unit,
             vlan_vp->flags & BCM_VLAN_PORT_NETWORK, &network_group);
        if (rv < 0) {
            goto cleanup;
        }
        if (soc_mem_field_valid(unit, SOURCE_VPm, NETWORK_GROUPf)) {
            rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry);
            if (rv < 0) {
                goto cleanup;
            }
            soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                                       NETWORK_GROUPf,
                                       network_group);
            rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry);
            if (rv < 0) {
                goto cleanup;
            }
        }
        network_group = vlan_vp->egress_network_group_id;
        rv = _bcm_validate_splithorizon_network_group(unit,
             vlan_vp->flags & BCM_VLAN_PORT_NETWORK, &network_group);
        if (rv < 0) {
            goto cleanup;
        }
        if (soc_mem_field_valid(unit, ING_DVP_TABLEm, NETWORK_GROUPf)) {
            rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry);
            if (rv < 0) {
                goto cleanup;
            }
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp_entry,
                                           NETWORK_GROUPf,
                                           network_group);
            rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp_entry);
            if (rv < 0) {
                goto cleanup;
            }
        }
    }


    /* Set VLAN VP software state */
    VLAN_VP_INFO(unit, vp)->criteria = vlan_vp->criteria;
    VLAN_VP_INFO(unit, vp)->flags = vlan_vp->flags;
    VLAN_VP_INFO(unit, vp)->match_vlan = vlan_vp->match_vlan;
    VLAN_VP_INFO(unit, vp)->match_inner_vlan = vlan_vp->match_inner_vlan;
    VLAN_VP_INFO(unit, vp)->port = vlan_vp->port;
    VLAN_VP_INFO(unit, vp)->match_tunnel_value = vlan_vp->match_tunnel_value;

    BCM_GPORT_VLAN_PORT_ID_SET(vlan_vp->vlan_port_id, vp);
    vlan_vp->encap_id = nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

    return rv;

cleanup:
    if (!(vlan_vp->flags & BCM_VLAN_PORT_REPLACE)) {
        (void) _bcm_vp_free(unit, _bcmVpTypeVlan, 1, vp);
        _bcm_tr2_vlan_vp_nh_info_delete(unit, nh_index);

        (void)_bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigClear, vp,
                               ING_DVP_CONFIG_INVALID_VP_TYPE, 
                               ING_DVP_CONFIG_INVALID_INTF_ID, 
                               ING_DVP_CONFIG_INVALID_PORT_TYPE);

        if (policer_set) {
            /* Decrement the reference count for the policer */
            if (soc_feature(unit, soc_feature_global_meter)) {
                (void)_bcm_esw_policer_decrement_ref_count(unit, vlan_vp->policer_id);
            }
        }
        sal_memset(&svp_entry, 0, sizeof(source_vp_entry_t));
        (void)WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry);

        (void)_bcm_tr2_vlan_vp_match_cleanup(unit, vlan_vp);
    }

    if (tpid_enable) {
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr2_vlan_vp_create
 * Purpose:
 *      Create a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      vlan_vp - (IN/OUT) Pointer to VLAN virtual port structure.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_tr2_vlan_vp_match_add(int unit,
                         bcm_vlan_port_t *vlan_vp, int vp)
{
    int rv;

    VLAN_VIRTUAL_INIT(unit);

    VLAN_VIRTUAL_LOCK(unit);

    rv = _bcm_tr2_vlan_vp_match_add(unit, vlan_vp, vp);

    VLAN_VIRTUAL_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_tr2_vlan_vp_match_delete
 * Purpose:
 *       a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      vlan_vp - (IN/OUT) Pointer to VLAN virtual port structure.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_tr2_vlan_vp_match_delete(int unit, bcm_vlan_port_t *vlan_vp, int vp)
{
    int rv;

    VLAN_VIRTUAL_INIT(unit);

    VLAN_VIRTUAL_LOCK(unit);

    rv = _bcm_tr2_vlan_vp_match_delete(unit, vlan_vp, vp, NULL);

    VLAN_VIRTUAL_UNLOCK(unit);

    return rv;
}




/*
 * Function:
 *      bcm_tr2_vlan_vp_create
 * Purpose:
 *      Create a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan_vp - (IN/OUT) Pointer to VLAN virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_tr2_vlan_vp_create(int unit,
                         bcm_vlan_port_t *vlan_vp)
{
    int rv;

    VLAN_VIRTUAL_INIT(unit);

    VLAN_VIRTUAL_LOCK(unit);

    rv = _bcm_tr2_vlan_vp_create(unit, vlan_vp);

    VLAN_VIRTUAL_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_tr2_vlan_vp_destroy
 * Purpose:
 *      Destroy a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) VLAN VP GPORT ID.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr2_vlan_vp_destroy(int unit, bcm_gport_t gport)
{
    int vp;
    source_vp_entry_t svp_entry;
    int nh_index;
    ing_dvp_table_entry_t dvp_entry;
    bcm_policer_t policer = 0;

    if (!BCM_GPORT_IS_VLAN_PORT(gport)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }

#ifdef BCM_TRIDENT_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership))  {
        BCM_IF_ERROR_RETURN
            (bcm_td2p_vp_vlan_member_set(unit, gport, 0));
    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership) ||
        soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) { 
        /* Disable ingress and egress filter modes */
        BCM_IF_ERROR_RETURN
            (bcm_td_vp_vlan_member_set(unit, gport, 0));
    }
#endif /* BCM_TRIDENT_SUPPORT */

    /* Delete ingress VLAN translation entry */
    BCM_IF_ERROR_RETURN
        (_bcm_tr2_vlan_vp_match_delete(unit, NULL, vp, NULL));

#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_vfi)) {
        bcm_vpn_t vpn;
        int vfi_index;
        BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
        vfi_index = soc_SOURCE_VPm_field32_get(unit, &svp_entry, VFIf);
        if (vfi_index != _BCM_VLAN_VFI_INVALID) {
            _BCM_VPN_SET(vpn, _BCM_VPN_TYPE_VFI, vfi_index);
            BCM_IF_ERROR_RETURN(_bcm_tr3_vlan_port_delete(unit, vpn, vp));
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    if (soc_feature(unit, soc_feature_global_meter)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_get_policer_from_table(unit,
                SOURCE_VPm, vp, &svp_entry, &policer, 0));
        if (policer != 0) {
            /* Decrement reference count */
            BCM_IF_ERROR_RETURN(
                    _bcm_esw_policer_decrement_ref_count(unit, policer));
        }
    }

    /* Clear SVP entry */
    sal_memset(&svp_entry, 0, sizeof(source_vp_entry_t));
    BCM_IF_ERROR_RETURN
        (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));

    /* Clear DVP entry */
    BCM_IF_ERROR_RETURN
        (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry, NEXT_HOP_INDEXf);

    BCM_IF_ERROR_RETURN
        (_bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigClear, vp,
                                ING_DVP_CONFIG_INVALID_VP_TYPE, 
                                ING_DVP_CONFIG_INVALID_INTF_ID, 
                                ING_DVP_CONFIG_INVALID_PORT_TYPE));

    /* Clear next hop entries and free next hop index */
    BCM_IF_ERROR_RETURN
        (_bcm_tr2_vlan_vp_nh_info_delete(unit, nh_index));

    /* Decrement port's VP count */
    BCM_IF_ERROR_RETURN
        (_bcm_tr2_vlan_vp_port_cnt_update(unit,
                                              VLAN_VP_INFO(unit, vp)->port,
                                              vp, FALSE));
    /* Free VP */
    BCM_IF_ERROR_RETURN
        (_bcm_vp_free(unit, _bcmVpTypeVlan, 1, vp));

    /* Clear VLAN VP software state */
    sal_memset(VLAN_VP_INFO(unit, vp), 0, sizeof(_bcm_tr2_vlan_vp_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr2_vlan_vp_destroy
 * Purpose:
 *      Destroy a VLAN virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) VLAN VP GPORT ID.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_tr2_vlan_vp_destroy(int unit, bcm_gport_t gport)
{
    int rv;

    VLAN_VIRTUAL_INIT(unit);

    VLAN_VIRTUAL_LOCK(unit);

    rv = _bcm_tr2_vlan_vp_destroy(unit, gport);

    VLAN_VIRTUAL_UNLOCK(unit);

    return rv;
}
#define  _BCM_QOS_MAP_SHIFT       10
/*
 * Function:
 *      _bcm_tr2_vlan_vp_find
 * Purpose:
 *      Get VLAN virtual port info.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan_vp - (IN/OUT) Pointer to VLAN virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr2_vlan_vp_find(int unit, bcm_vlan_port_t *vlan_vp)
{
    int vp;
    ing_dvp_table_entry_t dvp_entry;
    int nh_index;
    source_vp_entry_t svp_entry;
    egr_l3_next_hop_entry_t egr_nh;
#ifdef BCM_KATANA_SUPPORT
    bcm_if_t intf = 0;
    int profile_index = -1;
#endif
    if (!BCM_GPORT_IS_VLAN_PORT(vlan_vp->vlan_port_id)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_VLAN_PORT_ID_GET(vlan_vp->vlan_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }

    bcm_vlan_port_t_init(vlan_vp);

    BCM_IF_ERROR_RETURN(_bcm_tr2_vlan_vp_match_get(unit, vp, vlan_vp));

    vlan_vp->flags = VLAN_VP_INFO(unit, vp)->flags;

    BCM_IF_ERROR_RETURN
        (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry, NEXT_HOP_INDEXf);
    vlan_vp->encap_id = nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

    /* Get Egress Class */
    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, SD_TAG__CLASS_IDf)) {
        /* Read the existing egress next_hop entry */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                            nh_index, &egr_nh));
        vlan_vp->if_class = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                               &egr_nh, SD_TAG__CLASS_IDf);
    }

    BCM_GPORT_VLAN_PORT_ID_SET(vlan_vp->vlan_port_id, vp);

    if (soc_feature(unit, soc_feature_global_meter)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_get_policer_from_table(unit, SOURCE_VPm, vp, &svp_entry,
                    &(vlan_vp->policer_id), 0));
    }

#ifdef BCM_KATANA_SUPPORT
    if (soc_feature(unit, soc_feature_extended_queueing)) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, nh_index)) {
            intf = nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else {
            intf = nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        }
        if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(vlan_vp->port)) ||
            (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(vlan_vp->port)) ||
            (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(vlan_vp->port))) {
             BCM_IF_ERROR_RETURN(
                 _bcm_kt_extended_queue_qos_profile_get(unit, intf, &profile_index));
        }
        if (profile_index != -1) {
            profile_index = (profile_index & 0x3) |
            (_BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE << _BCM_QOS_MAP_SHIFT);
        }
        vlan_vp->qos_map_id =  profile_index;
    }
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr2_vlan_vp_find
 * Purpose:
 *      Get VLAN virtual port info.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan_vp - (IN/OUT) Pointer to VLAN virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_tr2_vlan_vp_find(int unit, bcm_vlan_port_t *vlan_vp)
{
    VLAN_VIRTUAL_INIT(unit);
    return _bcm_tr2_vlan_vp_find(unit, vlan_vp);
}
/*
 * Function:
 * bcm_tr2_vlan_port_learn_set
 * Purpose:
 * Modifies learning on a l2 logical port.
 * Parameters:
 * unit - SOC unit number
 * vlan_port_if - l2 logical port ID
 * flags - learning control flags
 * Returns:
 * BCM error code (BCM_E_XXX)
 */
int
bcm_tr2_vlan_port_learn_set(int unit, bcm_gport_t vlan_port_id, uint32 flags)
{
    int vp, cml = 0, rv = BCM_E_NONE;
    source_vp_entry_t svp;

    cml = 0;
    if (!(flags & BCM_PORT_LEARN_FWD)) {
        cml |= (1 << 0);
    }
    if (flags & BCM_PORT_LEARN_CPU) {
        cml |= (1 << 1);
    }
    if (flags & BCM_PORT_LEARN_PENDING) {
        cml |= (1 << 2);
    }
    if (flags & BCM_PORT_LEARN_ARL) {
        cml |= (1 << 3);
    }

    /* Get the VP index from the gport */
    vp = BCM_GPORT_VLAN_PORT_ID_GET(vlan_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    /* Be sure the entry is used and is set for VLAN virtual ports */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
        return BCM_E_NOT_FOUND;
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        return rv;
    }
    if (soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) != 3) { /* VLAN PORT (GPON)*/
        return BCM_E_NOT_FOUND;
    }

    soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, cml);
    soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, cml);
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);

    return rv;
}

/*
 * Function:
 *      bcm_tr2_vlan_port_phys_gport_get
 * Purpose:
 *      Get the physical gport of a VLAN port.
 * Returns: 
 *      BCM_E_XXX
 */     

int 
bcm_tr2_vlan_port_phys_gport_get(int unit, int vp, bcm_gport_t *gp)
{
    int nh_index;
    ing_dvp_table_entry_t dvp;
    ing_l3_next_hop_entry_t ing_nh;
    bcm_module_t mod_out, mod_in;
    bcm_port_t port_out, port_in;
    bcm_trunk_t trunk_id;

    BCM_IF_ERROR_RETURN(READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                     nh_index, &ing_nh));

    if (soc_feature(unit, soc_feature_generic_dest)) {
        uint32 dest_type = SOC_MEM_FIF_DEST_INVALID, dest = 0;
        dest = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm, &ing_nh,
                                         DESTINATIONf, &dest_type);
        if (dest_type == SOC_MEM_FIF_DEST_LAG) {
            trunk_id = (dest & SOC_MEM_FIF_DGPP_TGID_MASK);
            BCM_GPORT_TRUNK_SET(*gp, trunk_id);
        } else if (dest_type == SOC_MEM_FIF_DEST_DGPP) {
            mod_in = (dest & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                      SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
            port_in = dest & SOC_MEM_FIF_DGPP_PORT_MASK;
            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, mod_in, port_in,
                                        &mod_out, &port_out));
            BCM_GPORT_MODPORT_SET(*gp, mod_out, port_out);
        }
    } else {
        if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
            trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
            BCM_GPORT_TRUNK_SET(*gp, trunk_id);
        } else {
            mod_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
            port_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, mod_in, port_in,
                                        &mod_out, &port_out));
            BCM_GPORT_MODPORT_SET(*gp, mod_out, port_out);
        }
    }

    return BCM_E_NONE;
}

void 
bcm_tr2_vlan_port_match_count_adjust(int unit, int vp, int step)
{
    VLAN_VP_INFO(unit, vp)->match_count += step;
}

/*
 * Function:
 *      _bcm_tr2_vlan_port_resolve
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
_bcm_tr2_vlan_port_resolve(int unit, bcm_gport_t vlan_port_id,
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

    if ((soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, ENTRY_TYPEf) != 0x2) &&
        (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, ENTRY_TYPEf) != 0x3)) {
        /* Entry type is not L2 DVP and is not GPON DVP*/
        return BCM_E_NOT_FOUND;
    }

    if (soc_feature(unit, soc_feature_generic_dest)) {
        uint32 dest_type = SOC_MEM_FIF_DEST_INVALID, dest = 0;
        dest = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm, &ing_nh,
                                        DESTINATIONf, &dest_type);
        if (dest_type == SOC_MEM_FIF_DEST_LAG) {
            *trunk_id = (dest & SOC_MEM_FIF_DGPP_TGID_MASK);
        } else if (dest_type == SOC_MEM_FIF_DEST_DGPP) {
            *modid = (dest & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                      SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
            *port = dest & SOC_MEM_FIF_DGPP_PORT_MASK;
        }
    } else {
        if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
            *trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
        } else {
            /* Only add this to replication set if destination is local */
            *modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
            *port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
        }
    }
    *id = vp;
    return rv;
}

/* ---------------------------------------------------------------------------- 
 *
 * Routines for adding/removing virtual ports to/from VLAN
 *
 * ---------------------------------------------------------------------------- 
 */

/*
 * Function:
 *      _bcm_tr2_phy_port_trunk_is_local
 * Purpose:
 *      Determine if the given physical port or trunk is local.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      gport - (IN) Physical port or trunk GPORT ID.
 *      is_local - (OUT) Indicates if gport is local.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr2_phy_port_trunk_is_local(int unit, bcm_gport_t gport, int *is_local)
{
    bcm_trunk_t trunk_id;
    int rv = BCM_E_NONE;
    int local_member_count;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    int id;
    int modid_local;

    *is_local = 0;

    if (BCM_GPORT_IS_TRUNK(gport)) {
        trunk_id = BCM_GPORT_TRUNK_GET(gport);
        rv = _bcm_trunk_id_validate(unit, trunk_id);
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PORT);
        }
        rv = _bcm_esw_trunk_local_members_get(unit, trunk_id,
                0, NULL, &local_member_count);
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PORT);
        }   
        if (local_member_count > 0) {
            *is_local = 1;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, gport, &mod_out, &port_out,
                                    &trunk_id, &id)); 
       /* In case of extended queuing on KTX, trunk_id gets updated with
        * extended queue value. So set it to invalid value.
        */
#ifdef BCM_KATANA_SUPPORT
    if (soc_feature(unit, soc_feature_extended_queueing)) {
        if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) ||
            (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) ||
            (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport))) {
            trunk_id = BCM_TRUNK_INVALID;
        }
   }
#endif
        if ((trunk_id != -1) || (id != -1)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod_out, &modid_local));

        *is_local = modid_local;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_vlan_vp_untagged_add
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
_bcm_tr2_vlan_vp_untagged_add(int unit, bcm_vlan_t vlan, int vp, int flags)
{
    uint32      vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32      old_vent[SOC_MAX_MEM_FIELD_WORDS];
    bcm_vlan_action_set_t action;
    uint32 profile_idx;
    int rv;
    soc_mem_t mem = EGR_VLAN_XLATEm;

    if (soc_mem_is_valid(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
    }

    sal_memset(vent, 0, sizeof(vent));
    sal_memset(old_vent, 0, sizeof(old_vent));

    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    } else {
        soc_mem_field32_set(unit, mem, vent, VALIDf, 1);
    }

    if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, 1);
    }

    if (soc_mem_field_valid(unit, mem, ENTRY_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, ENTRY_TYPEf, 1);
    } else if (soc_mem_field_valid(unit, mem, KEY_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, 1);
    }

    soc_mem_field32_set(unit, mem, vent, DVPf, vp);
    soc_mem_field32_set(unit, mem, vent, OVIDf, vlan);

    if (VLAN_VP_INFO(unit, vp)->flags & BCM_VLAN_PORT_EGRESS_VLAN16) {
        soc_mem_field32_set(unit, mem, vent, NEW_OTAG_VPTAG_SELf, 1);
        soc_mem_field32_set(unit, mem, vent, NEW_OTAG_VPTAGf,
                                        VLAN_VP_INFO(unit, vp)->match_vlan);
        soc_mem_field32_set(unit, mem, vent, NEW_IVIDf, vlan);
    } else {
        soc_mem_field32_set(unit, mem, vent, NEW_OTAG_VPTAG_SELf, 0);
        /* egress_vlan in vlan_vp structure is used as swithcing vlan */
        soc_mem_field32_set(unit, mem, vent, NEW_OVIDf,
                VLAN_VP_INFO(unit, vp)->match_vlan & 0xfff);
        soc_mem_field32_set(unit, mem, vent, NEW_IVIDf, vlan);
    }

    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = bcmVlanActionReplace;
    action.ot_outer = bcmVlanActionReplace;
    if (flags & BCM_VLAN_PORT_UNTAGGED) {
        action.dt_inner = bcmVlanActionNone;
        action.ot_inner = bcmVlanActionNone;
    } else {
        if (soc_feature(unit, soc_feature_vlan_copy_action)) {
            action.dt_inner = bcmVlanActionCopy;
            action.ot_inner = bcmVlanActionCopy;
        } else {

            action.dt_inner = bcmVlanActionReplace;
            action.ot_inner = bcmVlanActionAdd;
        }
    }
    BCM_IF_ERROR_RETURN
        (_bcm_trx_egr_vlan_action_profile_entry_add(unit, &action, &profile_idx));

    soc_mem_field32_set(unit, mem, vent, TAG_ACTION_PROFILE_PTRf, profile_idx);
    rv = soc_mem_insert_return_old(unit, mem, MEM_BLOCK_ALL,
                                   vent, old_vent);
    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_mem_field32_get(unit, mem, old_vent,
                                          TAG_ACTION_PROFILE_PTRf);
        rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr2_vlan_vp_untagged_delete
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
_bcm_tr2_vlan_vp_untagged_delete(int unit, bcm_vlan_t vlan, int vp)
{
    uint32      vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32      old_vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32 profile_idx;
    int rv;
    soc_mem_t mem = EGR_VLAN_XLATEm;

    if (soc_mem_is_valid(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
    }

    sal_memset(vent, 0, sizeof(vent));
    sal_memset(old_vent, 0, sizeof(old_vent));

    if (soc_mem_field_valid(unit, mem, ENTRY_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, ENTRY_TYPEf, 1);
    } else if (soc_mem_field_valid(unit, mem, KEY_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, 1);
    }

    soc_mem_field32_set(unit, mem, vent, DVPf, vp);
    soc_mem_field32_set(unit, mem, vent, OVIDf, vlan);

    rv = soc_mem_delete_return_old(unit, mem, MEM_BLOCK_ALL,
                                   vent, old_vent);
    if (rv == SOC_E_NONE) {
        int is_valid = 0;
        if (soc_feature(unit, soc_feature_base_valid)) {
            is_valid = soc_mem_field32_get(unit, mem, old_vent,
                                           BASE_VALID_0f) == 3 &&
                       soc_mem_field32_get(unit, mem, old_vent,
                                           BASE_VALID_1f) == 7;
        } else {
            is_valid = soc_mem_field32_get(unit, mem, old_vent, VALIDf);
        }
        if (is_valid) {
            /* Delete the old vlan translate profile entry */
            profile_idx = soc_mem_field32_get(unit, mem, old_vent,
                                              TAG_ACTION_PROFILE_PTRf);
            rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx);
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr2_vlan_vp_untagged_get
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
_bcm_tr2_vlan_vp_untagged_get(int unit, bcm_vlan_t vlan, int vp,
                                  int *flags)
{
    uint32      vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32      res_vent[SOC_MAX_MEM_FIELD_WORDS];

    int rv, idx;
    uint32 profile_idx;
    bcm_vlan_action_set_t action;
    soc_mem_t mem = EGR_VLAN_XLATEm;

    if (soc_mem_is_valid(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
    }

    *flags = 0;

    sal_memset(vent, 0, sizeof(vent));
    sal_memset(res_vent, 0, sizeof(res_vent));

    if (soc_mem_field_valid(unit, mem, ENTRY_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, ENTRY_TYPEf, 1);
    } else if (soc_mem_field_valid(unit, mem, KEY_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, 1);
    }
    soc_mem_field32_set(unit, mem, vent, DVPf, vp);
    soc_mem_field32_set(unit, mem, vent, OVIDf, vlan);

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &idx, vent, res_vent, 0);
    if (rv == SOC_E_NONE) {
        int is_valid = 0;
        if (soc_feature(unit, soc_feature_base_valid)) {
            is_valid = soc_mem_field32_get(unit, mem, res_vent,
                                           BASE_VALID_0f) == 3 &&
                       soc_mem_field32_get(unit, mem, res_vent,
                                           BASE_VALID_1f) == 7;
        } else {
            is_valid = soc_mem_field32_get(unit, mem, res_vent, VALIDf);
        }

        if (is_valid) {
            profile_idx = soc_mem_field32_get(unit, mem, res_vent,
                                              TAG_ACTION_PROFILE_PTRf);
            _bcm_trx_egr_vlan_action_profile_entry_get(unit, &action, profile_idx);

            if (bcmVlanActionNone == action.ot_inner) {
                *flags = BCM_VLAN_PORT_UNTAGGED;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr2_vlan_gport_add
 * Purpose:
 *      Add a VLAN/NIV/Extender virtual port to the specified vlan.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      vlan  - (IN) VLAN ID to add virtual port to as a member.
 *      gport - (IN) VP Gport ID
 *      flags - (IN) Indicates if packet should egress out of the given
 *                   VLAN virtual port untagged. Not applicable to NIV or
 *                   Extender VPs. 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_vlan_gport_add(int unit, bcm_vlan_t vlan, bcm_gport_t gport,
                           int flags)
{
    int rv = BCM_E_NONE;
    int vp;
    bcm_vlan_port_t vlan_vp;
    bcm_gport_t phy_port_trunk = BCM_GPORT_INVALID;
    int is_local;
    vlan_tab_entry_t vtab;
    bcm_pbmp_t vlan_pbmp, vlan_ubmp, l2_pbmp, l3_pbmp;
    int i, mc_idx;
    bcm_if_t encap_id;
    bcm_multicast_t group;
    int bc_idx, umc_idx, uuc_idx;
    int bc_do_not_add = 0, umc_do_not_add = 0, uuc_do_not_add = 0;
    soc_field_t group_type[3] = {BC_IDXf, UMC_IDXf, UUC_IDXf};
    int egr_vt_added = FALSE, is_glp_port = 0;
    int ing_vp_vlan_membership_added = FALSE;
    int egr_vp_vlan_membership_added = FALSE;
    int ing_vp_group_added = FALSE, eg_vp_group_added = FALSE, vlan_vfi, is_vfi;
    _bcm_vp_vlan_mbrship_key_type_e key_type;

    COMPILER_REFERENCE(vlan_vfi);
    COMPILER_REFERENCE(key_type);

    /* check for specfic combination */
    if (flags & BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP) {
        if ((flags & (BCM_VLAN_GPORT_ADD_INGRESS_ONLY | 
                     BCM_VLAN_GPORT_ADD_EGRESS_ONLY)) ==
            (BCM_VLAN_GPORT_ADD_INGRESS_ONLY | 
             BCM_VLAN_GPORT_ADD_EGRESS_ONLY)) {
             return BCM_E_PARAM;
        }
    }

#ifdef BCM_TRIUMPH3_SUPPORT
    if (_BCM_VPN_VFI_IS_SET(vlan)) {
        is_vfi = 1;
        key_type = _bcm_vp_vlan_mbrship_vp_vfi_type;
        _BCM_VPN_GET(vlan_vfi, _BCM_VPN_TYPE_VFI, vlan);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        is_vfi = 0;
        key_type = _bcm_vp_vlan_mbrship_vp_vlan_type;
        vlan_vfi = vlan;

        /* Check that unsupported flags are not set */
        if (flags & ~(BCM_VLAN_PORT_UNTAGGED |
                      BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP |
                      BCM_VLAN_GPORT_ADD_INGRESS_ONLY |
                      BCM_VLAN_GPORT_ADD_EGRESS_ONLY  |
                      BCM_VLAN_GPORT_ADD_STP_MASK |
                      BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD |
                      BCM_VLAN_GPORT_ADD_UNKNOWN_MCAST_DO_NOT_ADD |
                      BCM_VLAN_GPORT_ADD_BCAST_DO_NOT_ADD)) {
            return BCM_E_PARAM;
        }
    }

    if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);

        /* Get the physical port or trunk the VP resides on */
        bcm_vlan_port_t_init(&vlan_vp);
        vlan_vp.vlan_port_id = gport;
        BCM_IF_ERROR_RETURN(bcm_tr2_vlan_vp_find(unit, &vlan_vp));
        if (is_vfi) {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_vlan_vfi)) {
                return _bcm_tr3_vlan_port_add(unit, vlan, &vlan_vp);
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
        } else {
            phy_port_trunk = vlan_vp.port;
        }
    } else
#ifdef BCM_TRIDENT_SUPPORT
    if (BCM_GPORT_IS_NIV_PORT(gport)) {
        bcm_niv_port_t niv_port;
        bcm_niv_egress_t niv_egress;
        int              count;

        vp = BCM_GPORT_NIV_PORT_ID_GET(gport);

        /* Get the physical port or trunk the VP resides on */
        bcm_niv_port_t_init(&niv_port);
        niv_port.niv_port_id = gport;
        BCM_IF_ERROR_RETURN(bcm_trident_niv_port_get(unit, &niv_port));
        if (niv_port.flags & BCM_NIV_PORT_MATCH_NONE) {
            bcm_niv_egress_t_init(&niv_egress);
            rv = bcm_trident_niv_egress_get(unit, niv_port.niv_port_id,
                                            1, &niv_egress, &count);
            if (BCM_SUCCESS(rv)) {
                if (!(niv_egress.flags & BCM_NIV_EGRESS_MULTICAST)) {
                    phy_port_trunk = niv_egress.port;
                } 
            }
        } else {
            phy_port_trunk = niv_port.port;
        }
    } else
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        bcm_extender_port_t extender_port;
        bcm_extender_egress_t extender_egress;
        int count;
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);

        /* Get the physical port or trunk the VP resides on */
        bcm_extender_port_t_init(&extender_port);
        extender_port.extender_port_id = gport;
        BCM_IF_ERROR_RETURN(bcm_tr3_extender_port_get(unit, &extender_port));
        if (extender_port.flags & BCM_EXTENDER_PORT_MATCH_NONE) {
            bcm_extender_egress_t_init(&extender_egress);
            rv = bcm_tr3_extender_egress_get_all(unit, extender_port.extender_port_id,
                                            1, &extender_egress, &count);
            if (count == 0) {
                /* No Extender egress object has been added to VP yet. */
                return BCM_E_CONFIG;
            }
            if (BCM_SUCCESS(rv)) {
                if (!(extender_egress.flags & BCM_EXTENDER_EGRESS_MULTICAST)) {
                    phy_port_trunk = extender_egress.port;
                }
            }
        } else {
            phy_port_trunk = extender_port.port;
        }
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (BCM_GPORT_IS_TRUNK(gport)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_tid_to_vp_lag_vp(unit, BCM_GPORT_TRUNK_GET(gport), &vp));

        /* VPLAG special process*/
        flags |= (BCM_VLAN_GPORT_ADD_BCAST_DO_NOT_ADD |
                  BCM_VLAN_GPORT_ADD_UNKNOWN_MCAST_DO_NOT_ADD |
                  BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD);
    } else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(gport);
    } else
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (BCM_GPORT_IS_VXLAN_PORT(gport) &&
        soc_feature(unit, soc_feature_vxlan)) {
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (BCM_GPORT_IS_L2GRE_PORT(gport)) {
        vp = BCM_GPORT_L2GRE_PORT_ID_GET(gport);
    }else if (BCM_GPORT_IS_MIM_PORT(gport)) {
        vp = BCM_GPORT_MIM_PORT_ID_GET(gport);
    } else if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
               (soc_feature(unit, soc_feature_channelized_switching))) &&
			BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        bcm_trunk_t trunk_id;
        if (BCM_SUCCESS(bcm_esw_trunk_find(unit, 0, gport, &trunk_id))) {
            vp = trunk_id | BCM_TGID_TRUNK_INDICATOR(unit);
        } else {
            /* Put together a VP like value for subport, which will be
               used below, this is a (mod,port) value for GPP */
            if (SOC_IS_TRIDENT3X(unit) && _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
                vp = _BCM_SUBPORT_COE_PORT_GLP_GET(BCM_GPORT_SUBPORT_PORT_GET(gport));
            } else {
                vp = (BCM_GPORT_SUBPORT_PORT_GET(gport) & 0x7FFF);
            }
        }
        is_glp_port = 1;

        /* Set the key_type as glp_vlan type */
        key_type = _bcm_vp_vlan_mbrship_glp_vlan_type;
    } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow) &&
            BCM_GPORT_IS_FLOW_PORT(gport)) {
        vp = BCM_GPORT_FLOW_PORT_ID_GET(gport);
    } else
#endif
#ifdef BCM_TRIDENT2PLUS_SUPPORT
if (BCM_GPORT_IS_MODPORT(gport)) {
        vp = (BCM_GPORT_MODPORT_MODID_GET(gport) << 8) | BCM_GPORT_MODPORT_PORT_GET(gport);
        is_glp_port = 1;
        key_type = _bcm_vp_vlan_mbrship_glp_vfi_type;
    } else
#endif
    {
        return BCM_E_PARAM;
    }

    if (phy_port_trunk != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_phy_port_trunk_is_local(unit, phy_port_trunk, &is_local));
        if (!is_local) {
            /* VP is added to replication lists only if it resides on a local port */
            return BCM_E_PORT;
        }
    }

    if(!is_vfi && !is_glp_port) {

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
        (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) &&
        (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf))) {
        
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
            mc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, group_type[i]);
            /* IPMC entry 0 is reserved */
            if (!mc_idx) {
                continue;
            }
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
        } else { 
            soc_mem_unlock(unit, VLAN_TABm);
        }

        bc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, BC_IDXf);
        umc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UMC_IDXf);
        uuc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UUC_IDXf);

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

        if (phy_port_trunk == BCM_GPORT_INVALID) {
            if (!bc_do_not_add || !umc_do_not_add || !uuc_do_not_add) {
                /* If VP's physical port is invalid, this VP cannot be added
                 * to multicast groups. User should set the DO_NOT_ADD flags.
                 */
                return BCM_E_PARAM;
            }
        }
    if (!bc_do_not_add) {
        /* Add the VP to the BC group */
        BCM_IF_ERROR_RETURN(_bcm_tr_multicast_ipmc_group_type_get(unit, bc_idx,
                    &group));
        if (BCM_GPORT_IS_VLAN_PORT(gport)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_multicast_vlan_encap_get(unit, group, phy_port_trunk,
                                                  gport, &encap_id));
        } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_multicast_niv_encap_get(unit, group, phy_port_trunk,
                                                 gport, &encap_id));
        } else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_multicast_extender_encap_get(unit, group, phy_port_trunk,
                                                 gport, &encap_id));
        }        
        /* coverity[stack_use_overflow : FALSE] */
        BCM_IF_ERROR_RETURN(bcm_esw_multicast_egress_add(unit, group,
                    phy_port_trunk, encap_id)); 
    }
    if (!umc_do_not_add) {
        /* Add the VP to the UMC group, if UMC group is different from BC group */
        BCM_IF_ERROR_RETURN(_bcm_tr_multicast_ipmc_group_type_get(unit, umc_idx,
                    &group));
        if (BCM_GPORT_IS_VLAN_PORT(gport)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_multicast_vlan_encap_get(unit, group, phy_port_trunk,
                                                  gport, &encap_id));
        } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_multicast_niv_encap_get(unit, group, phy_port_trunk,
                                                 gport, &encap_id));
        } else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_multicast_extender_encap_get(unit, group, phy_port_trunk,
                                                 gport, &encap_id));
        }
        if (umc_idx != bc_idx) {        
            BCM_IF_ERROR_RETURN(bcm_esw_multicast_egress_add(unit, group,
                        phy_port_trunk, encap_id)); 
        }
    }
    if (!uuc_do_not_add) {
        /* Add the VP to the UUC group, if UUC group is different from BC/UMC group */
        BCM_IF_ERROR_RETURN(_bcm_tr_multicast_ipmc_group_type_get(unit, uuc_idx,
                    &group));
        if (BCM_GPORT_IS_VLAN_PORT(gport)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_multicast_vlan_encap_get(unit, group, phy_port_trunk,
                                                  gport, &encap_id));
        } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_multicast_niv_encap_get(unit, group, phy_port_trunk,
                                                 gport, &encap_id));
        } else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_multicast_extender_encap_get(unit, group, phy_port_trunk,
                                                 gport, &encap_id));
        }
        if ((uuc_idx != bc_idx) && (uuc_idx != umc_idx)) {       
            BCM_IF_ERROR_RETURN(bcm_esw_multicast_egress_add(unit, group,
                        phy_port_trunk, encap_id)); 
        }
    }

        /* Add (vlan, vp) to egress vlan translation table and
         * configure the egress untagging status.
         */
        if (BCM_GPORT_IS_VLAN_PORT(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_vlan_vp_untagged_add(unit, vlan, vp, flags));
            egr_vt_added = TRUE;
        }
#ifdef BCM_TRIDENT_SUPPORT
        else if (BCM_GPORT_IS_NIV_PORT(gport)) {
            BCM_IF_ERROR_RETURN
                (bcm_trident_niv_untagged_add(unit, vlan, vp, flags, &egr_vt_added));
        }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
        else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
            BCM_IF_ERROR_RETURN
                (bcm_tr3_extender_untagged_add(unit, vlan, vp, flags, &egr_vt_added));
        }
#endif /* BCM_TRIUMPH3_SUPPORT */
 } else { /* !is_vfi && !is_glp_port */

        /* For VFI and GLP types, set the BC/UUC/UMC flags as 1 since they are not
           used, these flags will be validated below for config change */
        bc_do_not_add = uuc_do_not_add = umc_do_not_add = 1;
    }

    /* Configure ingress and egress VLAN membership checks */
    if (flags & BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP) {

#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_feature(unit, soc_feature_ing_vp_vlan_membership)) { 
            if (!(flags & BCM_VLAN_GPORT_ADD_EGRESS_ONLY)) {
                BCM_IF_ERROR_RETURN(bcm_td2_ing_vp_vlan_membership_add(unit, 
                              vp, vlan_vfi, flags, key_type));
                ing_vp_vlan_membership_added = TRUE;
            }
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
        {
            if (!(flags & BCM_VLAN_GPORT_ADD_EGRESS_ONLY)) {
                return BCM_E_UNAVAIL;
            }
        }

#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {
            if (!(flags & BCM_VLAN_GPORT_ADD_INGRESS_ONLY)) {

                BCM_IF_ERROR_RETURN(bcm_td2_egr_vp_vlan_membership_add(unit, 
                             vp, vlan_vfi, flags, key_type));
                egr_vp_vlan_membership_added = TRUE;
            }
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
        {
            if (!(flags & BCM_VLAN_GPORT_ADD_INGRESS_ONLY)) {
                return BCM_E_UNAVAIL;
            }
        }
    } else {
#ifdef BCM_TRIDENT_SUPPORT
        /* Due to change in VP's VLAN membership, may need to move VP to
         * another VP group.
         */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
            uint32 ifilter_flags;
            uint32 efilter_flags;

            BCM_IF_ERROR_RETURN(bcm_td2p_vp_vlan_member_get(
                    unit, gport, 1, &ifilter_flags));

            BCM_IF_ERROR_RETURN(bcm_td2p_vp_vlan_member_get(
                    unit, gport, 0, &efilter_flags));
            if (!bcm_td2p_ing_vp_group_unmanaged_get(unit)) {
                if (!(ifilter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)
                        && (ifilter_flags & BCM_PORT_VLAN_MEMBER_INGRESS)) {
                    BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_move(
                                unit, gport, vlan, TRUE, FALSE, flags));
                    ing_vp_group_added = TRUE;
                }
            } else {
                ing_vp_group_added = FALSE;
            }

            if (!bcm_td2p_egr_vp_group_unmanaged_get(unit)) {
                if (!(efilter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)
                        && (efilter_flags & BCM_PORT_VLAN_MEMBER_EGRESS)) {
                    BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_move(unit,
                                            gport, vlan, TRUE, TRUE, flags));
                    eg_vp_group_added = TRUE;
                }
            } else {
                eg_vp_group_added = FALSE;
            }
        } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
        {
            uint32 filter_flags;

            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_get(unit, gport, &filter_flags));

            if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
                if (!bcm_td_ing_vp_group_unmanaged_get(unit)) {
                    if ((filter_flags & BCM_PORT_VLAN_MEMBER_INGRESS) &&
                            !(filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)) {
                        BCM_IF_ERROR_RETURN
                            (bcm_td_ing_vp_group_move(unit, vp, vlan, TRUE));
                        ing_vp_group_added = TRUE;
                    }
                } else {
                    ing_vp_group_added = TRUE;
                }
            }     
            if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
                if (!bcm_td_egr_vp_group_unmanaged_get(unit)) {
                    if ((filter_flags & BCM_PORT_VLAN_MEMBER_EGRESS) &&
                            !(filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)) {
                        BCM_IF_ERROR_RETURN
                            (bcm_td_eg_vp_group_move(unit, vp, vlan, TRUE));
                        eg_vp_group_added = TRUE;
                    }
                } else {
                    eg_vp_group_added = TRUE;
                }
            }
        }
#endif /* BCM_TRIDENT_SUPPORT */
    } 

    if (bc_do_not_add && umc_do_not_add && uuc_do_not_add &&
        !egr_vt_added && !ing_vp_vlan_membership_added &&
        !egr_vp_vlan_membership_added && !ing_vp_group_added &&
        !eg_vp_group_added) {
        /* The given VP has not been added to VLAN's BC, UMC, and UUC groups.
         * It also has not been added to egress VLAN translation table, to
         * VP VLAN membership check table, or to a VP group.
         * This scenario does not make sense. Moreover, bcm_vlan_gport_get_all
         * would not be able to find this VP.
         */
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr2_vlan_gport_delete
 * Purpose:
 *      Remove a VLAN/NIV/Extender virtual port from the specified vlan.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      vlan  - (IN) VLAN to remove virtual port from.
 *      gport - (IN) VP Gport ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_vlan_gport_delete(int unit, bcm_vlan_t vlan, bcm_gport_t gport) 
{
    int rv = BCM_E_NONE;
    int vp;
    bcm_vlan_port_t vlan_vp;
    bcm_gport_t phy_port_trunk = BCM_GPORT_INVALID;
    int is_local;
    vlan_tab_entry_t vtab;
    int bc_idx, umc_idx, uuc_idx;
    bcm_multicast_t group;
    bcm_if_t encap_id;
    int bc_not_found = TRUE;
    int umc_not_found = TRUE;
    int uuc_not_found = TRUE;
    int egr_vt_found = FALSE;
    int ing_vp_vlan_membership_found = FALSE;
    int egr_vp_vlan_membership_found = FALSE;
    int ing_vp_group_found = FALSE, eg_vp_group_found = FALSE;
    int is_glp_port = 0, vlan_vfi = vlan, is_vfi = 0;
    _bcm_vp_vlan_mbrship_key_type_e key_type;
#ifdef BCM_TRIDENT_SUPPORT
    uint32 filter_flags;
#endif

    COMPILER_REFERENCE(vlan_vfi);
    COMPILER_REFERENCE(key_type);

#ifdef BCM_TRIUMPH3_SUPPORT
    if (_BCM_VPN_VFI_IS_SET(vlan)) {
        is_vfi = 1;
        key_type = _bcm_vp_vlan_mbrship_vp_vfi_type;
        _BCM_VPN_GET(vlan_vfi, _BCM_VPN_TYPE_VFI, vlan);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        is_vfi = 0;
        key_type = _bcm_vp_vlan_mbrship_vp_vlan_type;
        vlan_vfi = vlan;
    }

    if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);

        /* Get the physical port or trunk the VP resides on */
        bcm_vlan_port_t_init(&vlan_vp);
        vlan_vp.vlan_port_id = gport;
        BCM_IF_ERROR_RETURN(bcm_tr2_vlan_vp_find(unit, &vlan_vp));

#ifdef BCM_TRIUMPH3_SUPPORT
        if (is_vfi) {
            if (soc_feature(unit, soc_feature_vlan_vfi)) {
                return _bcm_tr3_vlan_port_delete(unit, vlan, vp);
            }
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            phy_port_trunk = vlan_vp.port;
        }
    } else
#ifdef BCM_TRIDENT_SUPPORT
    if (BCM_GPORT_IS_NIV_PORT(gport)) {
        bcm_niv_port_t niv_port;
        bcm_niv_egress_t niv_egress;
        int              count;

        vp = BCM_GPORT_NIV_PORT_ID_GET(gport);

        /* Get the physical port or trunk the VP resides on */
        bcm_niv_port_t_init(&niv_port);
        niv_port.niv_port_id = gport;
        BCM_IF_ERROR_RETURN(bcm_trident_niv_port_get(unit, &niv_port));
        if (niv_port.flags & BCM_NIV_PORT_MATCH_NONE) {
            phy_port_trunk = BCM_GPORT_INVALID;
            bcm_niv_egress_t_init(&niv_egress);
            rv = bcm_trident_niv_egress_get(unit, niv_port.niv_port_id,
                                            1, &niv_egress, &count);
            if (BCM_SUCCESS(rv)) {
                if (!(niv_egress.flags & BCM_NIV_EGRESS_MULTICAST)) {
                    phy_port_trunk = niv_egress.port;
                } 
            }
        } else {
            phy_port_trunk = niv_port.port;
        }
    } else
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        bcm_extender_port_t extender_port;
        bcm_extender_egress_t extender_egress;
        int count;

        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);

        /* Get the physical port or trunk the VP resides on */
        bcm_extender_port_t_init(&extender_port);
        extender_port.extender_port_id = gport;
        BCM_IF_ERROR_RETURN(bcm_tr3_extender_port_get(unit, &extender_port));
        if (extender_port.flags & BCM_EXTENDER_PORT_MATCH_NONE) {
            bcm_extender_egress_t_init(&extender_egress);
            rv = bcm_tr3_extender_egress_get_all(unit,
                        extender_port.extender_port_id, 1, &extender_egress, &count);
            if (count == 0) {
                /* No Extender egress object has been added to VP yet. */
                return BCM_E_CONFIG;
            }
            if (BCM_SUCCESS(rv)) {
                if (!extender_egress.flags & BCM_EXTENDER_EGRESS_MULTICAST) {
                    phy_port_trunk = extender_egress.port;
                }
            }
        } else {
            phy_port_trunk = extender_port.port;
        }

    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (BCM_GPORT_IS_TRUNK(gport)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_tid_to_vp_lag_vp(unit, BCM_GPORT_TRUNK_GET(gport), &vp));
    } else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(gport);
    } else
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (BCM_GPORT_IS_VXLAN_PORT(gport) &&
        soc_feature(unit, soc_feature_vxlan)) {
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (BCM_GPORT_IS_L2GRE_PORT(gport)) {
        vp = BCM_GPORT_L2GRE_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_MIM_PORT(gport)) {
        vp = BCM_GPORT_MIM_PORT_ID_GET(gport);
    } else if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
               (soc_feature(unit, soc_feature_channelized_switching))) &&
            BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        bcm_trunk_t trunk_id;
        if (BCM_SUCCESS(bcm_esw_trunk_find(unit, 0, gport, &trunk_id))) {
            vp = trunk_id | BCM_TGID_TRUNK_INDICATOR(unit);
        } else {
            /* Put together a VP like value for subport, which will be
               used below */
            if (SOC_IS_TRIDENT3X(unit) && _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
                vp = _BCM_SUBPORT_COE_PORT_GLP_GET(BCM_GPORT_SUBPORT_PORT_GET(gport));
            } else {
                vp = (BCM_GPORT_SUBPORT_PORT_GET(gport) & 0x7FFF);
            }
        }

        /* Make a note that this is a GPP port we are dealing with */
        is_glp_port = 1;

        /* Set the key_type as glp_vlan type */
        key_type = _bcm_vp_vlan_mbrship_glp_vlan_type;
    } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow) &&
            BCM_GPORT_IS_FLOW_PORT(gport)) {
        vp = BCM_GPORT_FLOW_PORT_ID_GET(gport);
    } else
#endif
#ifdef BCM_TRIDENT2PLUS_SUPPORT
if (BCM_GPORT_IS_MODPORT(gport)) {
        vp = (BCM_GPORT_MODPORT_MODID_GET(gport) << 8) | BCM_GPORT_MODPORT_PORT_GET(gport);
        is_glp_port = 1;
        key_type = _bcm_vp_vlan_mbrship_glp_vfi_type;
    } else
#endif
    {
        return BCM_E_PARAM;
    }

    if (phy_port_trunk != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_phy_port_trunk_is_local(unit, phy_port_trunk, &is_local));
        if (!is_local) {
            /* VP to be deleted must reside on a local port */
            return BCM_E_PORT;
        }
    }

    if(!is_vfi && !is_glp_port) {

        sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));

        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, vlan, &vtab)); 

        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
            return BCM_E_NOT_FOUND;
        }

        if (!SHR_BITGET(vlan_info[unit].vp_mode, vlan)) {
            if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
                if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
                    /* VP switching is not enabled on the VLAN */
                    return BCM_E_PORT;
                }
            }
        }

        if (phy_port_trunk != BCM_GPORT_INVALID) {
            /* Delete the VP from the BC group */
            bc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, BC_IDXf);
            BCM_IF_ERROR_RETURN(_bcm_tr_multicast_ipmc_group_type_get(unit, bc_idx,
                        &group));
            if (BCM_GPORT_IS_VLAN_PORT(gport)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_multicast_vlan_encap_get(unit, group, phy_port_trunk,
                                                      gport, &encap_id));
            } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_multicast_niv_encap_get(unit, group, phy_port_trunk,
                                                     gport, &encap_id));
            } else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_multicast_extender_encap_get(unit, group, phy_port_trunk,
                                                          gport, &encap_id));
            }

            rv = bcm_esw_multicast_egress_delete(unit, group, phy_port_trunk, encap_id); 
            /* BCM_E_NOT_FOUND is tolerated since the VP could have been added to the
             * VLAN with the BCM_VLAN_GPORT_ADD_BCAST_DO_NOT_ADD flag.
             */
            if (BCM_SUCCESS(rv)) {
                bc_not_found = FALSE;
            } else if (rv == BCM_E_NOT_FOUND) {
                bc_not_found = TRUE;
                rv = BCM_E_NONE;
            } else {
                return rv;
            }

            /* Delete the VP from the UMC group, if UMC group is different
             * from BC group.
             */ 
            umc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UMC_IDXf);
            BCM_IF_ERROR_RETURN(_bcm_tr_multicast_ipmc_group_type_get(unit, umc_idx,
                        &group));
            if (BCM_GPORT_IS_VLAN_PORT(gport)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_multicast_vlan_encap_get(unit, group, phy_port_trunk,
                                                      gport, &encap_id));
            } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_multicast_niv_encap_get(unit, group, phy_port_trunk,
                                                     gport, &encap_id));
            } else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_multicast_extender_encap_get(unit, group, phy_port_trunk,
                                                          gport, &encap_id));
            }

            if (umc_idx != bc_idx) {
                rv = bcm_esw_multicast_egress_delete(unit, group, phy_port_trunk, encap_id); 
                /* BCM_E_NOT_FOUND is tolerated since the VP could have been added to
                 * the VLAN with the BCM_VLAN_GPORT_ADD_UNKNOWN_MCAST_DO_NOT_ADD flag.
                 */
                if (BCM_SUCCESS(rv)) {
                    umc_not_found = FALSE;
                } else if (rv == BCM_E_NOT_FOUND) {
                    umc_not_found = TRUE;
                    rv = BCM_E_NONE;
                } else {
                    return rv;
                }
            } else {
                umc_not_found = bc_not_found;
            }

            /* Delete the VP from the UUC group, if UUC group is different
             * from BC and UMC groups.
             */
            uuc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UUC_IDXf);
            BCM_IF_ERROR_RETURN(_bcm_tr_multicast_ipmc_group_type_get(unit, uuc_idx,
                        &group));
            if (BCM_GPORT_IS_VLAN_PORT(gport)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_multicast_vlan_encap_get(unit, group, phy_port_trunk,
                                                      gport, &encap_id));
            } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_multicast_niv_encap_get(unit, group, phy_port_trunk,
                                                     gport, &encap_id));
            } else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_multicast_extender_encap_get(unit, group, phy_port_trunk,
                                                          gport, &encap_id));
            }

            if ((uuc_idx != bc_idx) && (uuc_idx != umc_idx)) {
                rv = bcm_esw_multicast_egress_delete(unit, group, phy_port_trunk, encap_id); 
                /* BCM_E_NOT_FOUND is tolerated since the VP could have been added to
                 * the VLAN with the BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD flag.
                 */
                if (BCM_SUCCESS(rv)) {
                    uuc_not_found = FALSE;
                } else if (rv == BCM_E_NOT_FOUND) {
                    uuc_not_found = TRUE;
                    rv = BCM_E_NONE;
                } else {
                    return rv;
                }
            } else if (uuc_idx == bc_idx) {
                uuc_not_found = bc_not_found;
            } else {
                uuc_not_found = umc_not_found;
            }
        }

        /* Delete (vlan, vp) from egress vlan translation table */
        if (BCM_GPORT_IS_VLAN_PORT(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_vlan_vp_untagged_delete(unit, vlan, vp));
            egr_vt_found = TRUE;
        }
#ifdef BCM_TRIDENT_SUPPORT
        else if (BCM_GPORT_IS_NIV_PORT(gport)) {
            rv = bcm_trident_niv_untagged_delete(unit, vlan, vp);
            if (BCM_SUCCESS(rv)) {
                egr_vt_found = TRUE;
            } else if (rv == BCM_E_NOT_FOUND) {
                egr_vt_found = FALSE;
                rv = BCM_E_NONE;
            } else {
                return rv;
            }
        }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
        else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
            rv = bcm_tr3_extender_untagged_delete(unit, vlan, vp);
            if (BCM_SUCCESS(rv)) {
                egr_vt_found = TRUE;
            } else if (rv == BCM_E_NOT_FOUND) {
                egr_vt_found = FALSE;
                rv = BCM_E_NONE;
            } else {
                return rv;
            }
        }
#endif /* BCM_TRIUMPH3_SUPPORT */
    }

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_ing_vp_vlan_membership)) {
        rv = bcm_td2_ing_vp_vlan_membership_delete(unit, vp, vlan_vfi,
                                                   key_type);
        if (BCM_SUCCESS(rv)) {
            ing_vp_vlan_membership_found = TRUE;
        } else if (rv == BCM_E_NOT_FOUND) {
            ing_vp_vlan_membership_found = FALSE;
            rv = BCM_E_NONE;
        } else {
            return rv;
        }
    }
        
    if (soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {
        rv = bcm_td2_egr_vp_vlan_membership_delete(unit, vp, vlan_vfi, 
                                                   key_type);
        if (BCM_SUCCESS(rv)) {
            egr_vp_vlan_membership_found = TRUE;
        } else if (rv == BCM_E_NOT_FOUND) {
            egr_vp_vlan_membership_found = FALSE;
            rv = BCM_E_NONE;
        } else {
            return rv;
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    BCM_IF_ERROR_RETURN(bcm_esw_port_vlan_member_get(unit, gport, &filter_flags));

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        if (!bcm_td2p_ing_vp_group_unmanaged_get(unit)) {
            if (!(filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP) &&
                    (filter_flags & BCM_PORT_VLAN_MEMBER_INGRESS)) {
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_move(unit,
                                        gport, vlan, FALSE, FALSE, 0));
                ing_vp_group_found = TRUE;
            }
        } else {
            ing_vp_group_found = FALSE;
        }

        if (!bcm_td2p_egr_vp_group_unmanaged_get(unit)) {
            if (!(filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP) &&
                    (filter_flags & BCM_PORT_VLAN_MEMBER_EGRESS)) {
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_port_move(unit,
                                        gport, vlan, FALSE, TRUE, 0));

                eg_vp_group_found = TRUE;
            }
        } else {
            eg_vp_group_found = FALSE;
        }
    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    {
        if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
            if (!bcm_td_ing_vp_group_unmanaged_get(unit)) {
                if ((filter_flags & BCM_PORT_VLAN_MEMBER_INGRESS) &&
                        !(filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)) {
                    BCM_IF_ERROR_RETURN
                        (bcm_td_ing_vp_group_move(unit, vp, vlan, FALSE));
                    ing_vp_group_found = TRUE;
                }
            } else {
                ing_vp_group_found = TRUE;
            }
        } 
        if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
            if (!bcm_td_egr_vp_group_unmanaged_get(unit)) {
                if ((filter_flags & BCM_PORT_VLAN_MEMBER_EGRESS) &&
                        !(filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)) {
                    BCM_IF_ERROR_RETURN
                        (bcm_td_eg_vp_group_move(unit, vp, vlan, FALSE));
                    eg_vp_group_found = TRUE;
                }
            } else {
                eg_vp_group_found = TRUE;
            }
        }
    }
#endif /* BCM_TRIDENT_SUPPORT */

    if (bc_not_found && umc_not_found && uuc_not_found &&
            !egr_vt_found && !ing_vp_vlan_membership_found &&
            !egr_vp_vlan_membership_found && !ing_vp_group_found &&
            !eg_vp_group_found) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_vlan_mc_group_gport_is_member
 * Purpose:
 *      Determine if the given VP is a member of the multicast group.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      group - (IN) Multicast group
 *      gport - (IN) VP Gport ID
 *      phy_port_trunk - (IN) Gport ID for physical port or trunk on which
 *                            the VP resides
 *      is_member - (OUT) Indicates if given VP belongs to VLAN.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_vlan_mc_group_gport_is_member(int unit, bcm_multicast_t group,
        bcm_gport_t gport, bcm_gport_t phy_port_trunk, int *is_member) 
{
    int rv = BCM_E_NONE;
    bcm_if_t encap_id;
    bcm_trunk_t trunk_id;
    int soc_max_num_ports = SOC_MAX_NUM_PORTS;
    int trunk_local_ports = 0;
    int if_max;
    bcm_if_t *if_array;
    int if_count, if_cur;
    int i, mc_idx;
#if (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t trunk_member_port[SOC_MAX_NUM_PP_PORTS];
#else
    bcm_port_t trunk_member_port[SOC_MAX_NUM_PORTS];
#endif

    if (NULL == is_member) {
        return BCM_E_PARAM;
    }
    *is_member = FALSE;

    if (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS) {
        soc_max_num_ports = SOC_MAX_NUM_PP_PORTS;
    }

    /* Get VP's multicast encap id */
    if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_multicast_vlan_encap_get(unit, group,
                    phy_port_trunk, gport, &encap_id));
    } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_multicast_niv_encap_get(unit, group,
                    phy_port_trunk, gport, &encap_id));
    } else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_multicast_extender_encap_get(unit, group,
                    phy_port_trunk, gport, &encap_id));
    } else {
        return BCM_E_PARAM;
    }

    /* Get all the local ports of the trunk */
    if (BCM_GPORT_IS_TRUNK(phy_port_trunk)) {
        trunk_id = BCM_GPORT_TRUNK_GET(phy_port_trunk);
        rv = _bcm_trunk_id_validate(unit, trunk_id);
        if (BCM_FAILURE(rv)) {
            return BCM_E_PORT;
        }
        rv = _bcm_esw_trunk_local_members_get(unit, trunk_id,
                soc_max_num_ports, trunk_member_port, &trunk_local_ports);
        if (BCM_FAILURE(rv)) {
            return BCM_E_PORT;
        }
    } 

    /* Find out if VP is a member of the multicast group */
    if_max = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    if_array = sal_alloc(if_max * sizeof(bcm_if_t),
            "temp repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }
    mc_idx = _BCM_MULTICAST_ID_GET(group);
    if (BCM_GPORT_IS_TRUNK(phy_port_trunk)) {
        /* Iterate over all local ports in the trunk and search for
         * a match on any local port
         */
        for (i = 0; i < trunk_local_ports; i++) {
            rv = bcm_esw_ipmc_egress_intf_get(unit, mc_idx,
                    trunk_member_port[i], if_max, if_array, &if_count);
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
    } else {
        rv = bcm_esw_ipmc_egress_intf_get(unit, mc_idx,
                phy_port_trunk, if_max, if_array, &if_count);
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
 *      bcm_tr2_vlan_gport_get
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
bcm_tr2_vlan_gport_get(int unit, bcm_vlan_t vlan, bcm_gport_t gport,
        int *flags) 
{
    int rv = BCM_E_NONE;
    int vp;
    bcm_vlan_port_t vlan_vp;
    bcm_gport_t phy_port_trunk = BCM_GPORT_INVALID;
    int is_local;
    vlan_tab_entry_t vtab;
    int bc_idx, umc_idx, uuc_idx;
    bcm_multicast_t group;
    int is_bc_member = FALSE;
    int is_umc_member = FALSE;
    int is_uuc_member = FALSE;
    int untagged_flag = 0;
    int egr_vt_found = FALSE;
    int ing_vp_vlan_membership_found = FALSE;
    int egr_vp_vlan_membership_found = FALSE;
    int ing_vp_group_found = FALSE;
    int eg_vp_group_found = FALSE;
    int is_glp_port = 0, is_vfi = 0, vlan_vfi = vlan;
    _bcm_vp_vlan_mbrship_key_type_e key_type;

    COMPILER_REFERENCE(vlan_vfi);
    COMPILER_REFERENCE(key_type);

#ifdef BCM_TRIUMPH3_SUPPORT
    if (_BCM_VPN_VFI_IS_SET(vlan)) {
        is_vfi = 1;
        key_type = _bcm_vp_vlan_mbrship_vp_vfi_type;
        _BCM_VPN_GET(vlan_vfi, _BCM_VPN_TYPE_VFI, vlan);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        is_vfi = 0;
        key_type = _bcm_vp_vlan_mbrship_vp_vlan_type;
        vlan_vfi = vlan;
    }

    *flags = 0;

    if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);

        /* Get the physical port or trunk the VP resides on */
        bcm_vlan_port_t_init(&vlan_vp);
        vlan_vp.vlan_port_id = gport;
        BCM_IF_ERROR_RETURN(bcm_tr2_vlan_vp_find(unit, &vlan_vp));

#ifdef BCM_TRIUMPH3_SUPPORT
        if (is_vfi) {
            if (soc_feature(unit, soc_feature_vlan_vfi)) {
                source_vp_entry_t svp;

                BCM_IF_ERROR_RETURN(
                    READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
                if ((soc_mem_field32_get(unit, SOURCE_VPm, &svp, ENTRY_TYPEf) != 1) ||
                    (soc_mem_field32_get(unit, SOURCE_VPm, &svp, VFIf) != vlan_vfi)) {
                    return BCM_E_NOT_FOUND;
                }
                return BCM_E_NONE;
            }
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            phy_port_trunk = vlan_vp.port;
        }
    } else
#ifdef BCM_TRIDENT_SUPPORT
    if (BCM_GPORT_IS_NIV_PORT(gport)) {
        bcm_niv_port_t niv_port;
        bcm_niv_egress_t niv_egress;
        int              count;

        vp = BCM_GPORT_NIV_PORT_ID_GET(gport);

        /* Get the physical port or trunk the VP resides on */
        bcm_niv_port_t_init(&niv_port);
        niv_port.niv_port_id = gport;
        BCM_IF_ERROR_RETURN(bcm_trident_niv_port_get(unit, &niv_port));
        if (niv_port.flags & BCM_NIV_PORT_MATCH_NONE) {
            phy_port_trunk = BCM_GPORT_INVALID;
            bcm_niv_egress_t_init(&niv_egress);
            rv = bcm_trident_niv_egress_get(unit, niv_port.niv_port_id,
                                            1, &niv_egress, &count);
            if (BCM_SUCCESS(rv)) {
                if (!(niv_egress.flags & BCM_NIV_EGRESS_MULTICAST)) {
                    phy_port_trunk = niv_egress.port;
                } 
            }
        } else {
            phy_port_trunk = niv_port.port;
        }
    } else
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        bcm_extender_port_t extender_port;
        bcm_extender_egress_t extender_egress;
        int count;
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);

        /* Get the physical port or trunk the VP resides on */
        bcm_extender_port_t_init(&extender_port);
        extender_port.extender_port_id = gport;
        BCM_IF_ERROR_RETURN(bcm_tr3_extender_port_get(unit, &extender_port));
        if (extender_port.flags & BCM_EXTENDER_PORT_MATCH_NONE) {
            bcm_extender_egress_t_init(&extender_egress);
            rv = bcm_tr3_extender_egress_get_all(unit,
                        extender_port.extender_port_id, 1, &extender_egress, &count);
            if (count == 0) {
                /* No Extender egress object has been added to VP yet. */
                return BCM_E_CONFIG;
            }
            if (BCM_SUCCESS(rv)) {
                if (!extender_egress.flags & BCM_EXTENDER_EGRESS_MULTICAST) {
                    phy_port_trunk = extender_egress.port;
                }
            }
        } else {
            phy_port_trunk = extender_port.port;
        }

    } else
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (BCM_GPORT_IS_TRUNK(gport)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_tid_to_vp_lag_vp(unit, BCM_GPORT_TRUNK_GET(gport), &vp));
    } else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(gport);
    } else
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (BCM_GPORT_IS_VXLAN_PORT(gport) &&
        soc_feature(unit, soc_feature_vxlan)) {
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (BCM_GPORT_IS_L2GRE_PORT(gport)) {
        vp = BCM_GPORT_L2GRE_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_MIM_PORT(gport)) {
        vp = BCM_GPORT_MIM_PORT_ID_GET(gport);
    } else if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
               (soc_feature(unit, soc_feature_channelized_switching))) &&
            BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        bcm_trunk_t   trunk_id;
        if (BCM_SUCCESS(bcm_esw_trunk_find(unit, 0, gport, &trunk_id))) {
            vp = trunk_id | BCM_TGID_TRUNK_INDICATOR(unit);
        } else {
            /* Put together a VP like value for subport, which will be
               used below, this is a (mod,port) value for GPP */
            if (SOC_IS_TRIDENT3X(unit) && _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
                vp = _BCM_SUBPORT_COE_PORT_GLP_GET(BCM_GPORT_SUBPORT_PORT_GET(gport));
            } else {
                vp = (BCM_GPORT_SUBPORT_PORT_GET(gport) & 0x7FFF);
            }
        }

        /* Mark that this is a GLP port */
        is_glp_port = 1;

        /* Set the key_type as glp_vlan type */
        key_type = _bcm_vp_vlan_mbrship_glp_vlan_type;
    } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow) &&
            BCM_GPORT_IS_FLOW_PORT(gport)) {
        vp = BCM_GPORT_FLOW_PORT_ID_GET(gport);
    } else
#endif
#ifdef BCM_TRIDENT2PLUS_SUPPORT
if (BCM_GPORT_IS_MODPORT(gport)) {
        vp = (BCM_GPORT_MODPORT_MODID_GET(gport) << 8) | BCM_GPORT_MODPORT_PORT_GET(gport);
        is_glp_port = 1;
        key_type = _bcm_vp_vlan_mbrship_glp_vfi_type;
    } else
#endif
    {
        return BCM_E_PARAM;
    }

    if (phy_port_trunk != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_phy_port_trunk_is_local(unit, phy_port_trunk, &is_local));
        if (!is_local) {
            return BCM_E_PORT;
        }
    }

    if(!is_vfi && !is_glp_port) {
        sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, vlan, &vtab));
        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
            return BCM_E_NOT_FOUND;
        }

        if (!SHR_BITGET(vlan_info[unit].vp_mode, vlan)) {
            if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
                if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
                    /* No virtual port exists for this VLAN */
                    return BCM_E_NOT_FOUND;
                }
            }
        }

        if (phy_port_trunk != BCM_GPORT_INVALID) {
            bc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, BC_IDXf);
            BCM_IF_ERROR_RETURN(_bcm_tr_multicast_ipmc_group_type_get(unit, bc_idx,
                        &group));
            BCM_IF_ERROR_RETURN(_bcm_tr2_vlan_mc_group_gport_is_member(unit, group,
                        gport, phy_port_trunk, &is_bc_member));

            umc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UMC_IDXf);
            if (umc_idx != bc_idx) {
                BCM_IF_ERROR_RETURN(_bcm_tr_multicast_ipmc_group_type_get(unit, umc_idx,
                            &group));
                BCM_IF_ERROR_RETURN(_bcm_tr2_vlan_mc_group_gport_is_member(unit, group,
                            gport, phy_port_trunk, &is_umc_member));
            } else {
                is_umc_member = is_bc_member;
            }

            uuc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UUC_IDXf);
            if (uuc_idx != bc_idx && uuc_idx != umc_idx) {
                BCM_IF_ERROR_RETURN(_bcm_tr_multicast_ipmc_group_type_get(unit, uuc_idx,
                            &group));
                BCM_IF_ERROR_RETURN(_bcm_tr2_vlan_mc_group_gport_is_member(unit, group,
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

        /* Get untagging status */
        if (BCM_GPORT_IS_VLAN_PORT(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_vlan_vp_untagged_get(unit, vlan, vp, &untagged_flag));
            egr_vt_found = TRUE;
        }
    #ifdef BCM_TRIDENT_SUPPORT
        else if (BCM_GPORT_IS_NIV_PORT(gport)) {
            rv = bcm_trident_niv_untagged_get(unit, vlan, vp, &untagged_flag);
            if (BCM_SUCCESS(rv)) {
                egr_vt_found = TRUE;
            } else if (rv == BCM_E_NOT_FOUND) {
                egr_vt_found = FALSE;
                rv = BCM_E_NONE;
            } else {
                return rv;
            }
        }
    #endif /* BCM_TRIDENT_SUPPORT */
    #ifdef BCM_TRIUMPH3_SUPPORT
        else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
            rv = bcm_tr3_extender_untagged_get(unit, vlan, vp, &untagged_flag);
            if (BCM_SUCCESS(rv)) {
                egr_vt_found = TRUE;
            } else if (rv == BCM_E_NOT_FOUND) {
                egr_vt_found = FALSE;
                rv = BCM_E_NONE;
            } else {
                return rv;
            }
        }
    #endif /* BCM_TRIUMPH3_SUPPORT */

        *flags |= untagged_flag;
    } /* !is_vfi && !is_glp_port */

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_ing_vp_vlan_membership)) {
        uint32 ing_flag;

        rv = bcm_td2_ing_vp_vlan_membership_get(unit, vp, vlan_vfi,
                                                &ing_flag, key_type);
        if (BCM_SUCCESS(rv)) {
            ing_vp_vlan_membership_found = TRUE;
            *flags |= ing_flag;
            *flags |= BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP;
        } else if (rv == BCM_E_NOT_FOUND) {
            ing_vp_vlan_membership_found = FALSE;
            rv = BCM_E_NONE;
        } else {
            return rv;
        }
    }
        
    if (soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {
        uint32 egr_flag;

        rv = bcm_td2_egr_vp_vlan_membership_get(unit, vp, vlan_vfi,
                                                &egr_flag, key_type);
        if (BCM_SUCCESS(rv)) {
            egr_vp_vlan_membership_found = TRUE;
            *flags |= egr_flag;
            *flags |= BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP;
        } else if (rv == BCM_E_NOT_FOUND) {
            egr_vp_vlan_membership_found = FALSE;
            rv = BCM_E_NONE;
        } else {
            return rv;
        }
    }

    if (ing_vp_vlan_membership_found && !egr_vp_vlan_membership_found) {
        *flags |= BCM_VLAN_GPORT_ADD_INGRESS_ONLY;
    } else if (!ing_vp_vlan_membership_found && egr_vp_vlan_membership_found) {
        *flags |= BCM_VLAN_GPORT_ADD_EGRESS_ONLY;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
        uint32 ing_filter_flags;

        BCM_IF_ERROR_RETURN(bcm_esw_port_vlan_member_get(unit, gport,
                            &ing_filter_flags));
        if ((ing_filter_flags & BCM_PORT_VLAN_MEMBER_INGRESS) &&
            !(ing_filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)) {
            ing_vp_group_found = TRUE;
        }
    }
    if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
        uint32 eg_filter_flags;

        BCM_IF_ERROR_RETURN(bcm_esw_port_vlan_member_get(unit, gport,
                            &eg_filter_flags));
        if ((eg_filter_flags & BCM_PORT_VLAN_MEMBER_EGRESS) &&
            !(eg_filter_flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)) {
            eg_vp_group_found = TRUE;
        }
    }

#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        int ut_state = 0;
        if (eg_vp_group_found) {
            if (!is_vfi) {
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_ut_state_get(
                                unit, gport, vlan, &ut_state));
            }
#if defined(BCM_TRIDENT3_SUPPORT)
            else if(SOC_MEM_IS_VALID(unit, EGR_VLAN_VFI_UNTAGm)) {
                BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_ut_state_get(
                                unit, gport, vlan, &ut_state));
            }
#endif
            if (ut_state) {
                *flags |= BCM_VLAN_GPORT_ADD_UNTAGGED;
            }
        }
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#endif /* BCM_TRIDENT_SUPPORT */

    if (!is_bc_member && !is_umc_member && !is_uuc_member &&
            !egr_vt_found && !ing_vp_vlan_membership_found &&
            !egr_vp_vlan_membership_found && !ing_vp_group_found &&
            !eg_vp_group_found) {
        return BCM_E_NOT_FOUND;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr2_mc_group_vp_get_all
 * Purpose:
 *      Get all the virtual ports of the given multicast group.
 * Parameters:
 *      unit   - (IN) SOC unit number. 
 *      mc_group - (IN) Multicast group.
 *      vp_bitmap - (OUT) Bitmap of virtual ports.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_mc_group_vp_get_all(int unit, bcm_multicast_t mc_group,
        SHR_BITDCL *vp_bitmap)
{
    int rv = BCM_E_NONE;
    int num_encap_id;
    bcm_if_t *encap_id_array = NULL;
    int i;
    int nh_index;
    egr_l3_next_hop_entry_t egr_nh;
    int vp;
    soc_field_t type_field = ENTRY_TYPEf;

    if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
        type_field = DATA_TYPEf;
    }

    if (NULL == vp_bitmap) {
        return BCM_E_PARAM;
    }

    /* Get the number of encap IDs of the multicast group */
    BCM_IF_ERROR_RETURN
        (bcm_esw_multicast_egress_get(unit, mc_group, 0,
                                      NULL, NULL, &num_encap_id));
    /* If no encap IDs in the multicast group, return directly */
    if (num_encap_id == 0) {
        return BCM_E_NONE;
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

    /* Convert encap IDs to virtual ports */
    for (i = 0; i < num_encap_id; i++) {
        if (BCM_IF_INVALID != encap_id_array[i]) {
            nh_index = encap_id_array[i] - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
            rv = READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, nh_index, &egr_nh); 
            if (BCM_FAILURE(rv)) {
                sal_free(encap_id_array);
                return rv;
            }
            if (2 != soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                        type_field)) {
                sal_free(encap_id_array);
                return BCM_E_INTERNAL;
            }
            vp = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, SD_TAG__DVPf);
            SHR_BITSET(vp_bitmap, vp);
        }
    }

    sal_free(encap_id_array);
    return rv;
}

/*
 * Function:
 *      _bcm_tr2_vp_untagged_get_all
 * Purpose:
 *      Get all the given VLAN's virtual ports and untagged status from egress
 *      VLAN translation table.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN.
 *      vp_bitmap - (OUT) Bitmap of virtual ports.
 *      arr_size  - (IN) Number of elements in flags_arr.
 *      flags_arr - (OUT) Array of flags.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_vp_untagged_get_all(int unit, bcm_vlan_t vlan, SHR_BITDCL *vp_bitmap,
        int arr_size, int *flags_arr)
{
    int rv = BCM_E_NONE;
    int chunk_size, num_chunks, chunk_index;
    int alloc_size;
    uint8 *egr_vt_buf = NULL;
    soc_field_t entry_type_f;
    int i, entry_index_min, entry_index_max;
    uint32 *ent;
    int vp, untagged_flag;
    soc_mem_t mem = EGR_VLAN_XLATEm;

    if (soc_mem_is_valid(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
    }

    if (NULL == vp_bitmap) {
        return BCM_E_PARAM;
    }
    if (arr_size != soc_mem_index_count(unit, SOURCE_VPm)) {
        return BCM_E_PARAM;
    }
    if (NULL == flags_arr) {
        return BCM_E_PARAM;
    }

    chunk_size = 1024;
    num_chunks = soc_mem_index_count(unit, mem) / chunk_size;
    if (soc_mem_index_count(unit, mem) % chunk_size) {
        num_chunks++;
    }

    alloc_size = soc_mem_entry_words(unit, mem) * 4 * chunk_size;
    egr_vt_buf = soc_cm_salloc(unit, alloc_size, "EGR_VLAN_XLATE buffer");
    if (NULL == egr_vt_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
        entry_type_f = DATA_TYPEf;
    } else if (soc_mem_field_valid(unit, mem, ENTRY_TYPEf)) {
        entry_type_f = ENTRY_TYPEf;
    } else {
        entry_type_f = KEY_TYPEf;
    }

    for (chunk_index = 0; chunk_index < num_chunks; chunk_index++) {

        /* Get a chunk of entries */
        entry_index_min = chunk_index * chunk_size;
        entry_index_max = entry_index_min + chunk_size - 1;
        if (entry_index_max > soc_mem_index_max(unit, mem)) {
            entry_index_max = soc_mem_index_max(unit, mem);
        }
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                entry_index_min, entry_index_max, egr_vt_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        /* Read each entry of the chunk to extract VP and untagged status */
        for (i = 0; i < (entry_index_max - entry_index_min + 1); i++) {
            int is_valid = 0;
            ent = soc_mem_table_idx_to_pointer(unit,
                    mem, uint32 *, egr_vt_buf, i);

            if (soc_feature(unit, soc_feature_base_valid)) {
                is_valid = soc_mem_field32_get(unit, mem, ent,
                                               BASE_VALID_0f) == 3 &&
                           soc_mem_field32_get(unit, mem, ent,
                                               BASE_VALID_1f) == 7;
            } else {
                is_valid = soc_mem_field32_get(unit, mem, ent, VALIDf);
            }

            if (!is_valid) {
                continue;
            }

            if (soc_mem_field32_get(unit, mem, ent, entry_type_f) != 1) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem, ent, OVIDf) != vlan) {
                continue;
            }

            vp = soc_mem_field32_get(unit, mem, ent, DVPf);

            /* Get untagged status */
            if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                rv = _bcm_tr2_vlan_vp_untagged_get(unit, vlan, vp,
                        &untagged_flag);
            }
#ifdef BCM_TRIDENT_SUPPORT
            else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
                rv = bcm_trident_niv_untagged_get(unit, vlan, vp,
                        &untagged_flag);
            }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
            else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                rv = bcm_tr3_extender_untagged_get(unit, vlan, vp,
                        &untagged_flag);
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
            else {
                continue;
            }
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            SHR_BITSET(vp_bitmap, vp);
            flags_arr[vp] = untagged_flag;
        }
    }

cleanup:
    if (egr_vt_buf) {
        soc_cm_sfree(unit, egr_vt_buf);
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr2_vlan_gport_get_all
 * Purpose:
 *      Get all virtual ports of the given VLAN.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      vlan      - (IN) VLAN ID.
 *      array_max - (IN) Max number of elements in the array.
 *      gport_array - (OUT) Array of virtual ports in GPORT format.
 *      flags_array - (OUT) Array of untagging status.
 *      array_size  - (OUT) Actual number of elements in the array.
 * Returns:
 *      BCM_E_XXX
 * Notes: If array_max == 0 and gport_array == NULL, actual number of
 *        virtual ports in the VLAN will be returned in array_size.
 */
int
bcm_tr2_vlan_gport_get_all(int unit, bcm_vlan_t vlan, int array_max,
        bcm_gport_t *gport_array, int *flags_array, int *array_size) 
{
    int rv = BCM_E_NONE;
    vlan_tab_entry_t vtab;
    int num_vp;
    SHR_BITDCL *bc_vp_bitmap = NULL;
    SHR_BITDCL *umc_vp_bitmap = NULL;
    SHR_BITDCL *uuc_vp_bitmap = NULL;
    int bc_idx, umc_idx, uuc_idx;
    bcm_multicast_t mc_group;
    SHR_BITDCL *egr_vt_vp_bitmap = NULL;
    int *egr_vt_flags_arr = NULL;
    SHR_BITDCL *ing_vp_bitmap = NULL;
    int *ing_flags_arr = NULL;
    SHR_BITDCL *egr_vp_bitmap = NULL;
    int *egr_flags_arr = NULL;
    SHR_BITDCL *ing_group_vp_bitmap = NULL;
    SHR_BITDCL *eg_group_vp_bitmap = NULL;
    int i;

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

#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_vfi)) {
        rv = _bcm_tr3_vlan_vpn_is_valid(unit, vlan);
        if (rv == BCM_E_NONE) {
            return _bcm_tr3_vlan_port_get_all(unit, vlan, array_max,
                                  gport_array, flags_array, array_size);
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vlan, &vtab));

    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
       if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
           /* This vlan has no virtual port members. */
           return BCM_E_NONE;
       } 
    }

    /* Get VPs from BC group */
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    bc_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "bc_vp_bitmap");
    if (NULL == bc_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(bc_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    bc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, BC_IDXf);
    rv = _bcm_tr_multicast_ipmc_group_type_get(unit, bc_idx, &mc_group);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    rv = _bcm_tr2_mc_group_vp_get_all(unit, mc_group, bc_vp_bitmap);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Get VPs from UMC group, if different from BC group */
    umc_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "umc_vp_bitmap");
    if (NULL == umc_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(umc_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    umc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UMC_IDXf);
    rv = _bcm_tr_multicast_ipmc_group_type_get(unit, umc_idx, &mc_group);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    if (umc_idx != bc_idx) {
        rv = _bcm_tr2_mc_group_vp_get_all(unit, mc_group, umc_vp_bitmap);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    } else {
        sal_memcpy(umc_vp_bitmap, bc_vp_bitmap, SHR_BITALLOCSIZE(num_vp));
    }

    /* Get VPs from UUC group, if different from BC and UMC group */
    uuc_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "uuc_vp_bitmap");
    if (NULL == uuc_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(uuc_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    uuc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UUC_IDXf);
    rv = _bcm_tr_multicast_ipmc_group_type_get(unit, uuc_idx, &mc_group);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    if ((uuc_idx != bc_idx) && (uuc_idx != umc_idx)) {
        rv = _bcm_tr2_mc_group_vp_get_all(unit, mc_group, uuc_vp_bitmap);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    } else if (uuc_idx == bc_idx) {
        sal_memcpy(uuc_vp_bitmap, bc_vp_bitmap, SHR_BITALLOCSIZE(num_vp));
    } else {
        sal_memcpy(uuc_vp_bitmap, umc_vp_bitmap, SHR_BITALLOCSIZE(num_vp));
    }

    /* Get VPs and untagged status from egress VLAN translation table */
    egr_vt_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "egr_vt_vp_bitmap");
    if (NULL == egr_vt_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(egr_vt_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    
    egr_vt_flags_arr = sal_alloc(num_vp * sizeof(int), "egr_vt_flags_arr");
    if (NULL == egr_vt_flags_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(egr_vt_flags_arr, 0, num_vp * sizeof(int));

    rv = _bcm_tr2_vp_untagged_get_all(unit, vlan, egr_vt_vp_bitmap,
            num_vp, egr_vt_flags_arr); 
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Get VPs and flags from ingress VP VLAN membership table */
    ing_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "ing_vp_bitmap");
    if (NULL == ing_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(ing_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    
    ing_flags_arr = sal_alloc(num_vp * sizeof(int), "ing_flags_arr");
    if (NULL == ing_flags_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(ing_flags_arr, 0, num_vp * sizeof(int));

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_ing_vp_vlan_membership)) {
        rv = bcm_td2_ing_vp_vlan_membership_get_all(unit, vlan, ing_vp_bitmap,
                num_vp, ing_flags_arr);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */
        
    /* Get VPs and flags from egress VP VLAN membership table */
    egr_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "egr_vp_bitmap");
    if (NULL == egr_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(egr_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    
    egr_flags_arr = sal_alloc(num_vp * sizeof(int), "egr_flags_arr");
    if (NULL == egr_flags_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(egr_flags_arr, 0, num_vp * sizeof(int));

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {
        rv = bcm_td2_egr_vp_vlan_membership_get_all(unit, vlan, egr_vp_bitmap,
                num_vp, egr_flags_arr);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    /* Get VPs from ingress VLAN VP groups */
    ing_group_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp),
            "ing_group_vp_bitmap");
    if (NULL == ing_group_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(ing_group_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    
#ifdef BCM_TRIDENT_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        if (!bcm_td2p_ing_vp_group_unmanaged_get(unit)) {
            rv = bcm_td2p_ing_vp_group_vlan_get_all(unit, vlan,
                    ing_group_vp_bitmap);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
        if (!bcm_td_ing_vp_group_unmanaged_get(unit)) {
            rv = bcm_td_ing_vp_group_vlan_get_all(unit, vlan, ing_group_vp_bitmap);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }
#endif /* BCM_TRIDENT_SUPPORT */

    /* Get VPs from egress VLAN VP groups */
    eg_group_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp),
            "eg_group_vp_bitmap");
    if (NULL == eg_group_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(eg_group_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    
#ifdef BCM_TRIDENT_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        if (!bcm_td2p_egr_vp_group_unmanaged_get(unit)) {
            rv = bcm_td2p_eg_vp_group_vlan_get_all(unit, vlan,
                    eg_group_vp_bitmap);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
        if (!bcm_td_egr_vp_group_unmanaged_get(unit)) {
            rv = bcm_td_eg_vp_group_vlan_get_all(unit, vlan, eg_group_vp_bitmap);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }
#endif /* BCM_TRIDENT_SUPPORT */

    /* Combine the VP bitmaps and the flags arrays */
    for (i = 0; i < num_vp; i++) {
        if (!SHR_BITGET(bc_vp_bitmap, i) &&
            !SHR_BITGET(umc_vp_bitmap, i) &&
            !SHR_BITGET(uuc_vp_bitmap, i) &&
            !SHR_BITGET(egr_vt_vp_bitmap, i) &&
            !SHR_BITGET(ing_vp_bitmap, i) &&
            !SHR_BITGET(egr_vp_bitmap, i) &&
            !SHR_BITGET(ing_group_vp_bitmap, i) &&
            !SHR_BITGET(eg_group_vp_bitmap, i)) {
            /* VP is not a member of this VLAN */
            continue;
        }

        if (0 == array_max) {
            (*array_size)++;
        } else {
            if (_bcm_vp_used_get(unit, i, _bcmVpTypeVlan)) {
                BCM_GPORT_VLAN_PORT_ID_SET(gport_array[*array_size], i);
            } else if (_bcm_vp_used_get(unit, i, _bcmVpTypeNiv)) {
                BCM_GPORT_NIV_PORT_ID_SET(gport_array[*array_size], i);
            } else if (_bcm_vp_used_get(unit, i, _bcmVpTypeExtender)) {
                BCM_GPORT_EXTENDER_PORT_ID_SET(gport_array[*array_size], i);
            } else if (_bcm_vp_used_get(unit, i, _bcmVpTypeVpLag)) {
                bcm_trunk_t tid;
                rv = _bcm_esw_trunk_vp_lag_vp_to_tid(unit, i, &tid);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
                BCM_GPORT_TRUNK_SET(gport_array[*array_size], tid);
            } else {
                /* Unexpected VP type */
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }

            if (NULL != flags_array) {
                flags_array[*array_size] = egr_vt_flags_arr[i] |
                    ing_flags_arr[i] | egr_flags_arr[i];
                if (!SHR_BITGET(bc_vp_bitmap, i)) {
                    flags_array[*array_size] |= BCM_VLAN_GPORT_ADD_BCAST_DO_NOT_ADD;
                }
                if (!SHR_BITGET(umc_vp_bitmap, i)) {
                    flags_array[*array_size] |= BCM_VLAN_GPORT_ADD_UNKNOWN_MCAST_DO_NOT_ADD;
                }
                if (!SHR_BITGET(uuc_vp_bitmap, i)) {
                    flags_array[*array_size] |= BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD;
                }
                if (SHR_BITGET(ing_vp_bitmap, i)) {
                    flags_array[*array_size] |= BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP;
                }
                if (SHR_BITGET(egr_vp_bitmap, i)) {
                    flags_array[*array_size] |= BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP;
                }
            }

            (*array_size)++;
            if (*array_size == array_max) {
                break;
            }
        }
    }

cleanup:
    if (NULL != bc_vp_bitmap) {
        sal_free(bc_vp_bitmap);
    }
    if (NULL != umc_vp_bitmap) {
        sal_free(umc_vp_bitmap);
    }
    if (NULL != uuc_vp_bitmap) {
        sal_free(uuc_vp_bitmap);
    }
    if (NULL != egr_vt_vp_bitmap) {
        sal_free(egr_vt_vp_bitmap);
    }
    if (NULL != egr_vt_flags_arr) {
        sal_free(egr_vt_flags_arr);
    }
    if (NULL != ing_vp_bitmap) {
        sal_free(ing_vp_bitmap);
    }
    if (NULL != ing_flags_arr) {
        sal_free(ing_flags_arr);
    }
    if (NULL != egr_vp_bitmap) {
        sal_free(egr_vp_bitmap);
    }
    if (NULL != egr_flags_arr) {
        sal_free(egr_flags_arr);
    }
    if (NULL != ing_group_vp_bitmap) {
        sal_free(ing_group_vp_bitmap);
    }
    if (NULL != eg_group_vp_bitmap) {
        sal_free(eg_group_vp_bitmap);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr2_vp_untagged_delete_all
 * Purpose:
 *      Delete all the given VLAN's virtual ports from egress
 *      VLAN translation table.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_vp_untagged_delete_all(int unit, bcm_vlan_t vlan)
{
    int rv = BCM_E_NONE;
    int chunk_size, num_chunks, chunk_index;
    int alloc_size;
    uint8 *egr_vt_buf = NULL;
    soc_field_t entry_type_f;
    int i, entry_index_min, entry_index_max;
    uint32 *ent;
    int vp;
    soc_mem_t mem = EGR_VLAN_XLATEm;

    if (soc_mem_is_valid(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
    }

    chunk_size = 1024;
    num_chunks = soc_mem_index_count(unit, mem) / chunk_size;
    if (soc_mem_index_count(unit, mem) % chunk_size) {
        num_chunks++;
    }

    alloc_size = soc_mem_entry_words(unit, mem) * 4 * chunk_size;
    egr_vt_buf = soc_cm_salloc(unit, alloc_size, "EGR_VLAN_XLATE buffer");
    if (NULL == egr_vt_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
        entry_type_f = DATA_TYPEf;
    } else if (soc_mem_field_valid(unit, mem, ENTRY_TYPEf)) {
        entry_type_f = ENTRY_TYPEf;
    } else {
        entry_type_f = KEY_TYPEf;
    }

    for (chunk_index = 0; chunk_index < num_chunks; chunk_index++) {
        /* Get a chunk of entries */
        entry_index_min = chunk_index * chunk_size;
        entry_index_max = entry_index_min + chunk_size - 1;
        if (entry_index_max > soc_mem_index_max(unit, mem)) {
            entry_index_max = soc_mem_index_max(unit, mem);
        }
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                entry_index_min, entry_index_max, egr_vt_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        /* Read each entry of the chunk */
        for (i = 0; i < (entry_index_max - entry_index_min + 1); i++) {
            int is_valid = 0;
            ent = soc_mem_table_idx_to_pointer(unit,
                    mem, uint32 *, egr_vt_buf, i);

            if (soc_feature(unit, soc_feature_base_valid)) {
                is_valid = soc_mem_field32_get(unit, mem, ent,
                                               BASE_VALID_0f) == 3 &&
                           soc_mem_field32_get(unit, mem, ent,
                                               BASE_VALID_1f) == 7;
            } else {
                is_valid = soc_mem_field32_get(unit, mem, ent, VALIDf);
            }

            if (!is_valid) {
                continue;
            }

            if (soc_mem_field32_get(unit, mem, ent, entry_type_f) != 1) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem, ent, OVIDf) != vlan) {
                continue;
            }

            vp = soc_mem_field32_get(unit, mem, ent, DVPf);

            if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                rv = _bcm_tr2_vlan_vp_untagged_delete(unit, vlan, vp);
            }
#ifdef BCM_TRIDENT_SUPPORT
            else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
                rv = bcm_trident_niv_untagged_delete(unit, vlan, vp);
            }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
            else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                rv = bcm_tr3_extender_untagged_delete(unit, vlan, vp);
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
            else {
                continue;
            }
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }

cleanup:
    if (egr_vt_buf) {
        soc_cm_sfree(unit, egr_vt_buf);
    }

    return rv;
}
/*
 * Function:
 *      bcm_tr2_vlan_gport_delete_all
 * Purpose:
 *      Remove all VLAN virtual ports from the specified vlan.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      vlan  - (IN) VLAN to remove virtual ports from.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_vlan_gport_delete_all(int unit, bcm_vlan_t vlan) 
{
    vlan_tab_entry_t vtab;
    int bc_idx, umc_idx, uuc_idx;
    bcm_multicast_t mc_group;

#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_vfi)) {
        int rv;
        rv = _bcm_tr3_vlan_vpn_is_valid(unit, vlan);
        if (rv == BCM_E_NONE) {
            return _bcm_tr3_vlan_port_delete_all(unit, vlan);
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, vlan, &vtab)); 

    if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
        return BCM_E_NOT_FOUND;
    }

    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
       if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
           /* There are no virtual ports to remove. */
           return BCM_E_NONE;
       }
    }

    /* Delete all VPs from the BC group */
    bc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, BC_IDXf);
    BCM_IF_ERROR_RETURN(_bcm_tr_multicast_ipmc_group_type_get(unit, bc_idx,
                &mc_group));
    BCM_IF_ERROR_RETURN(bcm_esw_multicast_egress_delete_all(unit, mc_group)); 

    /* Delete all VPs from the UMC group, if UMC group is different
     * from BC group.
     */ 
    umc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UMC_IDXf);
    if (umc_idx != bc_idx) {
        BCM_IF_ERROR_RETURN(_bcm_tr_multicast_ipmc_group_type_get(unit, umc_idx,
                    &mc_group));
        BCM_IF_ERROR_RETURN(bcm_esw_multicast_egress_delete_all(unit, mc_group)); 
    }

    /* Delete all VPs from the UUC group, if UUC group is different
     * from BC and UMC groups.
     */
    uuc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vtab, UUC_IDXf);
    if ((uuc_idx != bc_idx) && (uuc_idx != umc_idx)) {
        BCM_IF_ERROR_RETURN(_bcm_tr_multicast_ipmc_group_type_get(unit, uuc_idx,
                    &mc_group));
        BCM_IF_ERROR_RETURN(bcm_esw_multicast_egress_delete_all(unit, mc_group)); 
    }

    /* Clear the VIRTUAL_PORT_EN field since there are no virtual ports in
     * this VLAN anymore.
     */
    if (!SHR_BITGET(vlan_info[unit].vp_mode, vlan) && 
        SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
        soc_mem_field32_set(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf, 0);
    }
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, VLAN_TABm, MEM_BLOCK_ALL, vlan,
                &vtab));

    /* Delete given VLAN's (vlan, vp) entries from egress vlan translation table */
    BCM_IF_ERROR_RETURN(_bcm_tr2_vp_untagged_delete_all(unit, vlan));

#ifdef BCM_TRIDENT2_SUPPORT
    /* Delete given VLAN's (vlan, vp) entries from membership tables */
    if (soc_feature(unit, soc_feature_ing_vp_vlan_membership)) {
        BCM_IF_ERROR_RETURN
            (bcm_td2_ing_vp_vlan_membership_delete_all(unit, vlan));
    }
    if (soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {
        BCM_IF_ERROR_RETURN
            (bcm_td2_egr_vp_vlan_membership_delete_all(unit, vlan));
    }
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    /* Delete VP groups from VLAN */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        if (!bcm_td2p_ing_vp_group_unmanaged_get(unit)) {
            BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_delete_all(unit, vlan, FALSE));
        }
        if (!bcm_td2p_egr_vp_group_unmanaged_get(unit)) {
            BCM_IF_ERROR_RETURN(bcm_td2p_vp_group_delete_all(unit, vlan, TRUE));
        }
    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    {
        if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
            if (!bcm_td_ing_vp_group_unmanaged_get(unit)) {
                BCM_IF_ERROR_RETURN(bcm_td_ing_vp_group_vlan_delete_all(unit, vlan));
            }
        } 
        if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
            if (!bcm_td_egr_vp_group_unmanaged_get(unit)) {
                BCM_IF_ERROR_RETURN(bcm_td_eg_vp_group_vlan_delete_all(unit, vlan));
            }
        }
    }
#endif /* BCM_TRIDENT_SUPPORT */

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
/*
 * Function:
 *      _bcm_tr3_vlan_sd_tag_get
 * Purpose:
 *      Get SD TAG FLAG
 * Parameters:
 *      unit      - (IN) Device Number
 *      nh_index  - (IN) Next hop index
 *      flags     - (OUT) SD Tag flag
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_vlan_sd_tag_get(int unit, uint32 nh_index, uint32 *flags)
{
    egr_l3_next_hop_entry_t egr_nh;
    int action_present = 0, action_not_present = 0;

    BCM_IF_ERROR_RETURN(READ_EGR_L3_NEXT_HOPm(unit,
                             MEM_BLOCK_ANY, nh_index, &egr_nh));

    action_present = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                           &egr_nh, SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
    action_not_present = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                           &egr_nh, SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);

    if (action_not_present == 1) {
        *flags |= BCM_VLAN_PORT_SERVICE_VLAN_ADD;
    }

    switch (action_present) {
        case 1:
            *flags |= BCM_VLAN_PORT_SERVICE_VLAN_TPID_REPLACE;
            break;
        case 2:
            *flags |= BCM_VLAN_PORT_SERVICE_VLAN_REPLACE;
            break;
        case 3:
            *flags |= BCM_VLAN_PORT_SERVICE_VLAN_DELETE;
            break;
        case 4:
            *flags |= BCM_VLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE;
            break;
        case 5:
            *flags |= BCM_VLAN_PORT_SERVICE_VLAN_PRI_REPLACE;
            break;
        case 6:
            *flags |= BCM_VLAN_PORT_SERVICE_PRI_REPLACE;
            break;
        case 7:
            *flags |= BCM_VLAN_PORT_SERVICE_TPID_REPLACE;
            break;
        default:
            break;
    }

    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
int _bcm_td2_vlan_vp_scache_size_get(int unit, int *size)
{
    int alloc_sz = 0;
    int num_vp = 0;

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    /* Store VLAN_VP_INFO(unit, vp)->criteria */
    alloc_sz += num_vp * sizeof(bcm_vlan_port_match_t);

    /* Store VLAN_VP_INFO(unit, vp)->match_vlan */
    alloc_sz += num_vp * sizeof(bcm_vlan_t);

    /* Store VLAN_VP_INFO(unit, vp)->match_inner_vlan */
    alloc_sz += num_vp * sizeof(bcm_vlan_t);

    /* Store VLAN_VP_INFO(unit, vp)->port */
    alloc_sz += num_vp * sizeof(bcm_gport_t);

    /* Store VLAN_VP_INFO(unit, vp)->flags */
    alloc_sz += num_vp * sizeof(uint32);

    if (size != NULL) {
        *size = alloc_sz;
    }

    return BCM_E_NONE;
}

int _bcm_td2_vlan_vp_sync(int unit, uint8 **scache_ptr)
{
    int num_vp = 0, vp;
    _bcm_tr2_vlan_vp_info_t *vlan_vp;

    if ((*scache_ptr) == NULL) {
        return BCM_E_PARAM;
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    for (vp = 0; vp < num_vp; vp++) {
        vlan_vp = VLAN_VP_INFO(unit, vp);
        /* sync VLAN_VP_INFO(unit, vp)->criteria */
        sal_memcpy((*scache_ptr),
                   &(vlan_vp->criteria), sizeof(bcm_vlan_port_match_t));

        (*scache_ptr) += sizeof(bcm_vlan_port_match_t);

        /* sync VLAN_VP_INFO(unit, vp)->match_vlan */
        sal_memcpy((*scache_ptr),
                   &(vlan_vp->match_vlan), sizeof(bcm_vlan_t));

        (*scache_ptr) += sizeof(bcm_vlan_t);

        /* sync VLAN_VP_INFO(unit, vp)->match_inner_vlan */
        sal_memcpy((*scache_ptr),
                   &(vlan_vp->match_inner_vlan), sizeof(bcm_vlan_t));

        (*scache_ptr) += sizeof(bcm_vlan_t);

        /* sync VLAN_VP_INFO(unit, vp)->port */
        sal_memcpy((*scache_ptr),
                   &(vlan_vp->port), sizeof(bcm_gport_t));

        (*scache_ptr) += sizeof(bcm_gport_t);

        /* sync VLAN_VP_INFO(unit, vp)->flags */
        sal_memcpy((*scache_ptr),
                   &(vlan_vp->flags), sizeof(uint32));

        (*scache_ptr) += sizeof(uint32);
    }

    return BCM_E_NONE;
}

int _bcm_td2_vlan_vp_recover(int unit, uint8 **scache_ptr)
{
    int num_vp = 0, vp;
    _bcm_tr2_vlan_vp_info_t *vlan_vp;

    if ((*scache_ptr) == NULL) {
        _bcm_tr2_vlan_virtual_free_resources(unit);
        return BCM_E_PARAM;
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    for (vp = 0; vp < num_vp; vp++) {
        vlan_vp = VLAN_VP_INFO(unit, vp);
        /* Recover VLAN_VP_INFO(unit, vp)->criteria */
        sal_memcpy(&(vlan_vp->criteria),
                   (*scache_ptr), sizeof(bcm_vlan_port_match_t));

        (*scache_ptr) += sizeof(bcm_vlan_port_match_t);

        /* Recover VLAN_VP_INFO(unit, vp)->match_vlan */
        sal_memcpy(&(vlan_vp->match_vlan),
                   (*scache_ptr), sizeof(bcm_vlan_t));

        (*scache_ptr) += sizeof(bcm_vlan_t);

        /* Recover VLAN_VP_INFO(unit, vp)->match_inner_vlan */
        sal_memcpy(&(vlan_vp->match_inner_vlan),
                   (*scache_ptr), sizeof(bcm_vlan_t));

        (*scache_ptr) += sizeof(bcm_vlan_t);

        /* Recover VLAN_VP_INFO(unit, vp)->port */
        sal_memcpy(&(vlan_vp->port),
                   (*scache_ptr), sizeof(bcm_gport_t));

        (*scache_ptr) += sizeof(bcm_gport_t);

        /* Recover VLAN_VP_INFO(unit, vp)->flags */
        sal_memcpy(&(vlan_vp->flags),
                   (*scache_ptr), sizeof(uint32));

        (*scache_ptr) += sizeof(uint32);
    }

    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      bcm_tr2_vlan_virtual_reinit
 * Purpose:
 *      Warm boot recovery for the VLAN virtual software module
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_vlan_virtual_reinit(int unit)
{
    int rv = BCM_E_NONE;
    int stable_size;
    int chunk_size, num_chunks, chunk_index;
    uint8 *vt_buf = NULL;
    uint32 *vt_entry;
    int i, entry_index_min, entry_index_max;
    uint32 key_type, vp, nh_index = -1, trunk_bit;
    source_vp_entry_t svp_entry;
    ing_dvp_table_entry_t dvp_entry;
    ing_l3_next_hop_entry_t ing_nh_entry;
    wlan_svp_table_entry_t wlan_svp_entry;
    bcm_trunk_t tgid;
    bcm_module_t modid, mod_out;
    bcm_port_t port_num, port_out;
    uint32 profile_idx;
    ing_vlan_tag_action_profile_entry_t ing_profile_entry;
    soc_mem_t vt_mem;
    int vt_entry_size;
    soc_field_t valid_f;
    soc_field_t key_type_f;
    int key_type_ovid, key_type_ivid_ovid, key_type_otag, key_type_ivid;
    soc_field_t mpls_action_f;
    soc_field_t source_vp_f;
    soc_field_t ovid_f;
    soc_field_t ivid_f;
    soc_field_t otag_f;
    soc_field_t trunk_f;
    soc_field_t tgid_f;
    soc_field_t module_id_f;
    soc_field_t port_num_f;
    soc_field_t tag_action_profile_ptr_f;
    _bcm_vp_info_t vp_info;
#ifdef BCM_TRIUMPH3_SUPPORT
    int sd_tag = 0;
    int network_port = 0;
#endif /* BCM_TRIUMPH3_SUPPORT */

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeVlan;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (soc_feature(unit, soc_feature_vfi_profile)) {
        soc_mem_t mem;
        int buf_size;
        uint8 *buf = NULL;
        vfi_entry_t *vfi_entry = NULL;

        mem = VFIm;
        entry_index_min = soc_mem_index_min(unit, mem);
        entry_index_max = soc_mem_index_max(unit, mem);
        buf_size = SOC_MEM_TABLE_BYTES(unit, mem);

        buf = soc_cm_salloc(unit, buf_size, "VFI_Table buffer");
        if (NULL == buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                entry_index_min, entry_index_max, buf);
        if (SOC_FAILURE(rv)) {
            soc_cm_sfree(unit, buf);
            goto cleanup;
        }

        for (i = entry_index_min; i < entry_index_max; i++) {
            vfi_entry = soc_mem_table_idx_to_pointer(unit, mem,
                                                     vfi_entry_t *, buf, i);
            profile_idx = soc_mem_field32_get(unit, mem,
                                              vfi_entry, VFI_PROFILE_PTRf);

            rv = _bcm_vfi_profile_entry_reference(unit, profile_idx, 1);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                goto cleanup;
            }
        }
        soc_cm_sfree(unit, buf);
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

     /*
        *As to the chips which support vp_lag feature, already recover VLAN_VP_INFO from
        *scache in virtual_reinit, so don't need to recover it from VLAN_XLATE table.
        */
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_vp_lag) ||
        soc_feature(unit, soc_feature_vlan_vp_sync_and_recover)) {
        if (stable_size !=0 && !SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
            rv = BCM_E_NONE;
            goto cleanup;
        }
    }
#endif

    /* Recover VLAN virtual ports from VLAN_XLATE table */

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        vt_mem = VLAN_XLATE_EXTDm;
        vt_entry_size = sizeof(vlan_xlate_extd_entry_t);
        valid_f = VALID_0f;
        key_type_f = KEY_TYPE_0f;
        key_type_ovid = TR3_VLXLT_X_HASH_KEY_TYPE_OVID;
        key_type_ivid_ovid = TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID;
        key_type_otag = TR3_VLXLT_X_HASH_KEY_TYPE_OTAG;
        key_type_ivid = TR3_VLXLT_X_HASH_KEY_TYPE_IVID;
        mpls_action_f = XLATE__MPLS_ACTIONf;
        source_vp_f = XLATE__SOURCE_VPf;
        ovid_f = XLATE__OVIDf;
        ivid_f = XLATE__IVIDf;
        otag_f = XLATE__OTAGf;
        trunk_f = XLATE__Tf;
        tgid_f = XLATE__TGIDf;
        module_id_f = XLATE__MODULE_IDf;
        port_num_f = XLATE__PORT_NUMf;
        tag_action_profile_ptr_f = XLATE__TAG_ACTION_PROFILE_PTRf; 
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
            vt_mem = VLAN_XLATE_1_DOUBLEm;
            vt_entry_size = sizeof(vlan_xlate_1_double_entry_t);
        } else {
            vt_mem = VLAN_XLATEm;
            vt_entry_size = sizeof(vlan_xlate_entry_t);
        }
        valid_f = VALIDf;
        key_type_f = KEY_TYPEf;
        /* coverity[mixed_enums] */
        key_type_ovid = TR_VLXLT_HASH_KEY_TYPE_OVID;
        /* coverity[mixed_enums] */
        key_type_ivid_ovid = TR_VLXLT_HASH_KEY_TYPE_IVID_OVID;
        /* coverity[mixed_enums] */
        key_type_otag = TR_VLXLT_HASH_KEY_TYPE_OTAG;
        /* coverity[mixed_enums] */
        key_type_ivid = TR_VLXLT_HASH_KEY_TYPE_IVID;
        mpls_action_f = MPLS_ACTIONf;
        source_vp_f = SOURCE_VPf;
        ovid_f = OVIDf;
        ivid_f = IVIDf;
        otag_f = OTAGf;
        trunk_f = Tf;
        tgid_f = TGIDf;
        module_id_f = MODULE_IDf;
        port_num_f = PORT_NUMf;
        tag_action_profile_ptr_f = TAG_ACTION_PROFILE_PTRf; 
    }

    chunk_size = 1024;
    num_chunks = soc_mem_index_count(unit, vt_mem) / chunk_size;
    if (soc_mem_index_count(unit, vt_mem) % chunk_size) {
        num_chunks++;
    }

    vt_buf = soc_cm_salloc(unit, vt_entry_size * chunk_size,
            "VLAN_XLATE buffer");
    if (NULL == vt_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    for (chunk_index = 0; chunk_index < num_chunks; chunk_index++) {

        /* Get a chunk of entries */
        entry_index_min = chunk_index * chunk_size;
        entry_index_max = entry_index_min + chunk_size - 1;
        if (entry_index_max > soc_mem_index_max(unit, vt_mem)) {
            entry_index_max = soc_mem_index_max(unit, vt_mem);
        }
        rv = soc_mem_read_range(unit, vt_mem, MEM_BLOCK_ANY,
                entry_index_min, entry_index_max, vt_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        /* Read each entry of the chunk */
        for (i = 0; i < (entry_index_max - entry_index_min + 1); i++) {
            vt_entry = soc_mem_table_idx_to_pointer(unit,
                    vt_mem, uint32 *, vt_buf, i);

            if (soc_feature(unit, soc_feature_base_valid)) {
                if (soc_mem_field32_get(unit, vt_mem, vt_entry, BASE_VALID_0f) != 3 ||
                    soc_mem_field32_get(unit, vt_mem, vt_entry, BASE_VALID_1f) != 7) {
                    continue;
                }
            } else {
                if (soc_mem_field32_get(unit, vt_mem, vt_entry, valid_f) == 0) {
                    continue;
                }
            }

            key_type = soc_mem_field32_get(unit, vt_mem, vt_entry, key_type_f);
            if ((key_type != key_type_ovid) &&
                (key_type != key_type_ivid_ovid) &&
                (key_type != key_type_otag) &&
                (key_type != key_type_ivid)) {
                continue;
            }

            if (soc_mem_field32_get(unit, vt_mem, vt_entry, mpls_action_f) != 1) {
                continue;
            }

            vp = soc_mem_field32_get(unit, vt_mem, vt_entry, source_vp_f);
            if ((stable_size == 0) || SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
                /* Determine if VP is a VLAN VP by process of elimination.
                 * Can rule out NIV and Extender VPs since they have different
                 * VIF key types in VLAN_XLATEm.
                 */

                rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }

#ifdef BCM_TRIUMPH3_SUPPORT
                if (soc_feature(unit, soc_feature_vlan_vfi) &&
                    soc_SOURCE_VPm_field32_get(unit, &svp_entry, ENTRY_TYPEf) == 1) {
                    int vfi_index = soc_SOURCE_VPm_field32_get(unit, &svp_entry, VFIf);
                    if (vfi_index != _BCM_VLAN_VFI_INVALID) {
                        rv = _bcm_vfi_alloc_with_id(unit, VFIm, _bcmVfiTypeVlan, vfi_index);
                        if (BCM_FAILURE(rv)) {
                            goto cleanup;
                        }
                    }
                    sd_tag = soc_SOURCE_VPm_field32_get(unit, &svp_entry, SD_TAG_MODEf);
                    network_port = soc_SOURCE_VPm_field32_get(unit, &svp_entry, NETWORK_PORTf);
                } else
#endif /* BCM_TRIUMPH3_SUPPORT */
                /* Rule out MPLS and MiM VPs, for which ENTRY_TYPE = 1 */
                if (soc_SOURCE_VPm_field32_get(unit, &svp_entry, ENTRY_TYPEf) != 3) {
                    continue;
                }

                rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }

                if (!(soc_mem_field_valid(unit, ING_DVP_TABLEm, VP_TYPEf))) {
                    continue;
                }

                /* Rule out Trill VPs, for which VP_TYPE = 1 */
                if (soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry, VP_TYPEf) != 0) {
                    continue;
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
            if (key_type == key_type_ovid) {
                VLAN_VP_INFO(unit, vp)->criteria =
                    BCM_VLAN_PORT_MATCH_PORT_VLAN;
                VLAN_VP_INFO(unit, vp)->match_vlan = 
                    soc_mem_field32_get(unit, vt_mem, vt_entry, ovid_f);
            } else if (key_type == key_type_ivid_ovid) {
                VLAN_VP_INFO(unit, vp)->criteria =
                    BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
                VLAN_VP_INFO(unit, vp)->match_vlan = 
                    soc_mem_field32_get(unit, vt_mem, vt_entry, ovid_f);
                VLAN_VP_INFO(unit, vp)->match_inner_vlan = 
                    soc_mem_field32_get(unit, vt_mem, vt_entry, ivid_f);
            } else if (key_type == key_type_otag) {
                VLAN_VP_INFO(unit, vp)->criteria =
                    BCM_VLAN_PORT_MATCH_PORT_VLAN16;
                VLAN_VP_INFO(unit, vp)->match_vlan = 
                    soc_mem_field32_get(unit, vt_mem, vt_entry, otag_f);
            } else if (key_type == key_type_ivid) {
                VLAN_VP_INFO(unit, vp)->criteria =
                    BCM_VLAN_PORT_MATCH_PORT_INNER_VLAN;
                VLAN_VP_INFO(unit, vp)->match_inner_vlan =
                    soc_mem_field32_get(unit, vt_mem, vt_entry, ivid_f);
            } else {
                continue;
            }

            /* Recover VLAN_VP_INFO(unit, vp)->port */
            trunk_bit = soc_mem_field32_get(unit, vt_mem, vt_entry, trunk_f);
            if (trunk_bit) {
                tgid = soc_mem_field32_get(unit, vt_mem, vt_entry, tgid_f);
                BCM_GPORT_TRUNK_SET(VLAN_VP_INFO(unit, vp)->port, tgid);
            } else {
                modid = soc_mem_field32_get(unit, vt_mem, vt_entry, module_id_f);
                port_num = soc_mem_field32_get(unit, vt_mem, vt_entry, port_num_f);
                rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                        modid, port_num, &mod_out, &port_out);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
                BCM_GPORT_MODPORT_SET(VLAN_VP_INFO(unit, vp)->port,
                        mod_out, port_out);
            }

            /* Recover VLAN_VP_INFO(unit, vp)->flags */
            profile_idx = soc_mem_field32_get(unit, vt_mem, vt_entry,
                    tag_action_profile_ptr_f);
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
            if ((soc_ING_VLAN_TAG_ACTION_PROFILEm_field32_get(unit,
                     &ing_profile_entry, SOT_OTAG_ACTIONf) ==
                 bcmVlanActionReplace) &&
                (soc_ING_VLAN_TAG_ACTION_PROFILEm_field32_get(unit,
                     &ing_profile_entry, SOT_ITAG_ACTIONf) ==
                 bcmVlanActionAdd)) {
                VLAN_VP_INFO(unit, vp)->flags |= BCM_VLAN_PORT_INNER_VLAN_ADD;
            }

            if (VLAN_VP_INFO(unit, vp)->criteria ==
                    BCM_VLAN_PORT_MATCH_PORT_VLAN16) {
                VLAN_VP_INFO(unit, vp)->flags |= BCM_VLAN_PORT_EGRESS_VLAN16;
            }
#if defined(BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit)) {
                if (soc_SOURCE_VPm_field32_get(unit, &svp_entry, TPID_SOURCEf) == 2) {
                    VLAN_VP_INFO(unit, vp)->flags |= BCM_VLAN_PORT_USE_SGLP_TPID;
                }
            }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_vlan_vfi) && (sd_tag == 1)) {
                VLAN_VP_INFO(unit, vp)->flags |= BCM_VLAN_PORT_SERVICE_TAGGED;
            }

            if (soc_feature(unit, soc_feature_vlan_vfi) && (network_port == 1)) {
                VLAN_VP_INFO(unit, vp)->flags |= BCM_VLAN_PORT_NETWORK;
            }

            if (soc_feature(unit, soc_feature_vlan_vfi) && (nh_index != -1)) {
                uint32 flags = 0;
                rv = _bcm_tr3_vlan_sd_tag_get(unit, nh_index, &flags);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
                VLAN_VP_INFO(unit, vp)->flags |= flags;
            }
#endif /* BCM_TRIUMPH3_SUPPORT */

            if (stable_size == 0) {
                /* In the Port module, a port's VP count is not recovered in 
                 * level 1 Warm Boot.
                 */
                rv = _bcm_tr2_vlan_vp_port_cnt_update(unit,
                        VLAN_VP_INFO(unit, vp)->port, vp, TRUE);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
            }
        }
    }

cleanup:
    if (vt_buf) {
        soc_cm_sfree(unit, vt_buf);
    }

    if (BCM_FAILURE(rv)) {
        _bcm_tr2_vlan_virtual_free_resources(unit);
    }

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_tr2_vlan_vp_sw_dump
 * Purpose:
 *     Displays VLAN VP information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_tr2_vlan_vp_sw_dump(int unit)
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
            case BCM_VLAN_PORT_MATCH_PORT_INNER_VLAN:
                LOG_CLI((BSL_META_U(unit,
                                    " port plus Inner VLAN ID\n")));
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

#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

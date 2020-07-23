/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    extender.c
 * Purpose: Implements Port Extension APIs for Triumph3.
 */

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/hash.h>
#include <soc/l2x.h>
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#endif
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm_int/esw/switch.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/qos.h>
#include <bcm_int/esw/failover.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/tomahawk.h>
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/xgs5.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT*/
#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#endif /* BCM_SABER2_SUPPORT */

#define  TD_EXTENDER_PORT_HASH_BKT_SIZE          32
/*
 * Extender EGR_L3_NEXT_HOP View
 */
#define _BCM_EXTENDER_EGR_NEXT_HOP_SDTAG_VIEW    0x2
#define _BCM_EXTENDER_EGR_NEXT_HOP_L3MC_VIEW     0x7

/*
 * Extender EGR_L3_NEXT_HOP VNTAG ACTIONS
 */
enum _BCM_EXTENDEREGR_NEXT_HOP_VNTAG_ACTIONS {
    ETAG_NOOP            = 0,
    VNTAG_ADD_OR_REPLACE = 1,
    ETAG_ADD_OR_REPLACE  = 2,
    ETAG_DELETE          = 3,
    VNTAG_ACTIONS_COUNT
};

/*
 * Software book keeping for Port Extension information
 */
typedef struct _bcm_tr3_extender_egress_s {
    uint32 flags;
    bcm_gport_t port;
    uint16 extended_port_vid;
    bcm_vlan_t match_vlan;
    bcm_extender_pcp_de_select_t pcp_de_select;
    int qos_map_id;
    uint8 pcp;
    uint8 de;
    int service_qos_map_id;
    uint16 service_tpid;
    bcm_vlan_t service_vlan;
    uint8 service_pri;
    uint8 service_cfi;
    int next_hop_index;
    bcm_if_t intf;
    uint32 multicast_flags;
    struct _bcm_tr3_extender_egress_s *next; /* Pointer to next Extender egress
                                                object */
} _bcm_tr3_extender_egress_t;

typedef struct _bcm_tr3_extender_nh_info_s {
    bcm_port_t port;
    int nh_ref_cnt;
    struct _bcm_tr3_extender_nh_info_s *next;
} _bcm_tr3_extender_nh_info_t;

typedef struct _bcm_tr3_extender_port_info_s {
    uint32 flags;
    bcm_gport_t port;
    uint16 extended_port_vid;
    bcm_extender_pcp_de_select_t pcp_de_select;
    int qos_map_id;
    uint8 pcp;
    uint8 de;
    bcm_vlan_t match_vlan;
    _bcm_tr3_extender_egress_t *egress_list; /* Linked list of Extender egress
                                               objects */
    _bcm_tr3_extender_nh_info_t *mc_nh_info;
                               /* nh info for multicast egress objects. */
} _bcm_tr3_extender_port_info_t;

/* Structure to facilitate passing of SD-tag parameters */
typedef struct _bcm_tr3_extender_sd_tag_s {
    uint32 flags;
    uint16 service_tpid;
    bcm_vlan_t service_vlan;
    uint8 service_pri;
    uint8 service_cfi;
    int service_qos_map_id;
} _bcm_tr3_extender_sd_tag_t;

typedef struct _bcm_tr3_extender_bookkeeping_s {
    _bcm_tr3_extender_port_info_t *port_info; /* VP state */
    int invalid_next_hop_index;
} _bcm_tr3_extender_bookkeeping_t;

static _bcm_tr3_extender_bookkeeping_t _bcm_tr3_extender_bk_info[BCM_MAX_NUM_UNITS];

#define EXTENDER_INFO(unit) (&_bcm_tr3_extender_bk_info[unit])
#define EXTENDER_PORT_INFO(unit, vp) (&EXTENDER_INFO(unit)->port_info[vp])
#define EXTENDER_PORT_HASH_BKT(unit, vp, idx) \
            (&(EXTENDER_PORT_INFO(unit, vp)->egress_list[idx]))
#define EXTENDER_PORT_MC_NH_INFO(unit, vp) (EXTENDER_PORT_INFO(unit, vp)->mc_nh_info)

STATIC int
_bcm_tr3_extender_egress_pbmp_add(int unit, int vp, bcm_pbmp_t pbmp);

STATIC int
_bcm_tr3_extender_egress_hash_calc(int unit, void *hash_key, uint16 *hash);

/*
 * Function:
 *      _bcm_tr3_extender_port_cnt_update
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
_bcm_tr3_extender_port_cnt_update(int unit, bcm_gport_t gport,
        int vp, int incr)
{
    int mod_out, port_out, tgid_out, id_out;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count;
    int idx;
    int mod_local;
    _bcm_port_info_t *port_info;
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

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0     SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION BCM_WB_VERSION_1_0

/*
 * Function:
 *      _bcm_tr3_extender_egress_hash_recover
 * Purpose:
 *      Recover the vp based hash table according to hardware information.
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      vp   - (IN) Virtual port number.
 *      node - (IN) extender egress hardware information.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_extender_egress_hash_recover(
    int unit, int vp, _bcm_tr3_extender_egress_t *node)
{
    int bkt_idx, alloc_size;
    uint16 hash_val;
    bcm_extender_egress_t extender_egress;
    _bcm_tr3_extender_egress_t *temp_ptr = NULL;

    bcm_extender_egress_t_init(&extender_egress);
    extender_egress.flags                = node->flags;
    extender_egress.port                 = node->port;
    extender_egress.intf                 = node->intf;
    extender_egress.service_tpid         = node->service_tpid;
    extender_egress.service_vlan         = node->service_vlan;
    extender_egress.service_pri          = node->service_pri;
    extender_egress.service_cfi          = node->service_cfi;
    extender_egress.service_qos_map_id   = node->service_qos_map_id;
    extender_egress.extended_port_vid    = node->extended_port_vid;
    extender_egress.pcp                  = node->pcp;
    extender_egress.de                   = node->de;
    BCM_IF_ERROR_RETURN(
        _bcm_tr3_extender_egress_hash_calc(unit, (void *)&extender_egress,
                                          &hash_val));
    bkt_idx  = hash_val % TD_EXTENDER_PORT_HASH_BKT_SIZE;
    if ((EXTENDER_PORT_INFO(unit, vp)->egress_list == NULL)) {
        alloc_size = sizeof(_bcm_tr3_extender_egress_t) *
                     TD_EXTENDER_PORT_HASH_BKT_SIZE;
        temp_ptr = sal_alloc(alloc_size, "EXTENDER egress object");
        if (NULL == temp_ptr) {
            return BCM_E_MEMORY;
        }
        sal_memset(temp_ptr, 0, alloc_size);
        EXTENDER_PORT_INFO(unit, vp)->egress_list = temp_ptr;
    }
    node->next = EXTENDER_PORT_HASH_BKT(unit, vp, bkt_idx)->next;
    EXTENDER_PORT_HASH_BKT(unit, vp, bkt_idx)->next = node;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_extender_required_scache_size_get
 * Purpose:
 *      Get required Extender module scache size for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      size - (OUT) Required scache size.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_extender_required_scache_size_get(int unit, uint32 *size)
{
    int num_vp;
    int num_nh;
    int num_ports;

    *size = 0;
    /* Add size required to store the bitmap of Extender VPs that
     * were created without match criteria.
     */
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    *size += SHR_BITALLOCSIZE(num_vp);

    /* Add size required to store the bitmap of next hops that
     * are associated with Extender egress objects.
     */
    num_nh = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    *size += SHR_BITALLOCSIZE(num_nh);

    /* Add size required to store an array of physical port bitmaps.
     * The size of the array equals to the number of next hops.
     * A next hop entry may be shared by multiple Extender egress objects that
     * have identical attributes except for the physical port. Hence,
     * the physical port of each Extender egress object cannot be recovered
     * from hardware and needs to be stored in the next hop's physical
     * port bitmap.
     */
    num_ports = soc_mem_field_length(unit, L3_IPMCm, L3_BITMAPf);
    *size += (SHR_BITALLOCSIZE(num_ports) * num_nh);

    /* Add size required to store match_vlan. There can be at most
     * one match_vlan per Extender virtual port.
     */
    *size += (sizeof(bcm_vlan_t) * num_vp);

    /* Add size required to store the bitmap of next hops which
     * are associated with BCM_EXTENDER_EGRESS_MULTICAST.
     */
    *size += SHR_BITALLOCSIZE(num_nh);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_extender_sync
 * Purpose:
 *      Record persistent info into the scache.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */

int
_bcm_tr3_extender_sync(int unit, uint8 **scache_ptr)
{
    int rv = BCM_E_NONE;
    int num_vp, num_nh, num_ports;
    SHR_BITDCL *match_none_vp_bitmap = NULL;
    SHR_BITDCL *nh_bitmap = NULL;
    SHR_BITDCL *mc_nh_bitmap = NULL;
    SHR_BITDCL **pbmp_ptr_array = NULL;
    bcm_vlan_t *match_vlan_array = NULL;
    int i;
    _bcm_tr3_extender_egress_t *curr_ptr;
    int nh_index;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t tgid_out;
    int id_out;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count;
    int mod_local;
    int bkt_idx, bkt_size;
    int vp_mc = 0;
#if defined(BCM_SABER2_SUPPORT)
    uint32 source_vp_sb2_5626x = 0xFED5ABC7;
#endif /* BCM_SABER2_SUPPORT */

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    num_nh = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    num_ports = soc_mem_field_length(unit, L3_IPMCm, L3_BITMAPf);

    /* Allocate a bitmap of Extender VPs that were created without match criteria */
    match_none_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp),
            "Bitmap of Extender VPs without match criteria");
    if (NULL == match_none_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(match_none_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

    /* Allocate a bitmap of next hops associated with Extender egress objects */
    nh_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_nh), "Bitmap of NHs");
    if (NULL == nh_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(nh_bitmap, 0, SHR_BITALLOCSIZE(num_nh));

    /* Allocate an array of pointers to port bitmaps */
    pbmp_ptr_array = sal_alloc(sizeof(SHR_BITDCL *) * num_nh,
            "Array of pointers to pbmp");
    if (NULL == pbmp_ptr_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(pbmp_ptr_array, 0, sizeof(SHR_BITDCL *) * num_nh);

    /* Allocate an array of match_vlan */
    match_vlan_array = sal_alloc(sizeof(bcm_vlan_t) * num_vp,
            "Array of match_vlan");
    if (NULL == match_vlan_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(match_vlan_array, 0, sizeof(bcm_vlan_t) * num_vp);

    /* Allocate a bitmap of next hops associated with multicast Extender egress objects */
    mc_nh_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_nh), "Bitmap of multicast NHs");
    if (NULL == mc_nh_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(mc_nh_bitmap, 0, SHR_BITALLOCSIZE(num_nh));

    /* Traverse Extender VPs */
    for (i = 0; i < num_vp; i++) {
        if (EXTENDER_PORT_INFO(unit, i)->flags & BCM_EXTENDER_PORT_MATCH_NONE) {
            SHR_BITSET(match_none_vp_bitmap, i);
            /* Traverse this Extender VP's list of Extender egress objects */
            vp_mc = 0;
            if (EXTENDER_PORT_MC_NH_INFO(unit, i) != NULL) {
                /* The shared vp associates with mulicast egress objects. */
                if (EXTENDER_PORT_INFO(unit, i)->egress_list == NULL) {
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
                }
                vp_mc = 1;
            }
            bkt_size = vp_mc ? TD_EXTENDER_PORT_HASH_BKT_SIZE : 1;
            for (bkt_idx = 0; bkt_idx < bkt_size; bkt_idx++) {
                if (vp_mc) {
                    curr_ptr = EXTENDER_PORT_HASH_BKT(unit, i, bkt_idx)->next;
                } else {
                    curr_ptr = EXTENDER_PORT_INFO(unit, i)->egress_list;
                }

                while (NULL != curr_ptr) {
                    if (curr_ptr->match_vlan) {
                        /* Match_vlan can be nonzero only for unicast Extender egress
                         * objects, and there can be at most one unicast Extender
                         * egress object in the Extender port's egress_list. These
                         * constraints are enforced by bcm_tr3_extender_egress_add.
                         */
                        match_vlan_array[i] = curr_ptr->match_vlan;
                    }

                    nh_index = curr_ptr->next_hop_index;
                    SHR_BITSET(nh_bitmap, nh_index);

                    if (curr_ptr->flags & BCM_EXTENDER_EGRESS_MULTICAST) {
                        SHR_BITSET(mc_nh_bitmap, nh_index);
                    }

                    /* If not already done, allocate a port bitmap for this
                     * next hop index.
                     */
                    if (NULL == pbmp_ptr_array[nh_index]) {
                        pbmp_ptr_array[nh_index] =
                                sal_alloc(SHR_BITALLOCSIZE(num_ports), "NH pbmp");
                        sal_memset(pbmp_ptr_array[nh_index], 0,
                                SHR_BITALLOCSIZE(num_ports));
                    }

                    /* Set the bit in the port bitmap corresponding to the
                     * physical port this Extender egress object resides on.
                     */
                    rv = _bcm_esw_gport_resolve(unit, curr_ptr->port,
                            &mod_out, &port_out, &tgid_out, &id_out);
                    if (BCM_FAILURE(rv)) {
                        goto cleanup;
                    }
                    if (BCM_TRUNK_INVALID != tgid_out) {
                        rv = _bcm_esw_trunk_local_members_get(unit,
                                    tgid_out, SOC_MAX_NUM_PORTS, local_member_array,
                                    &local_member_count);
                        if (BCM_FAILURE(rv)) {
                            goto cleanup;
                        }
                        /* Only one member needs to be added for warm boot recovery
                         * of the trunk group ID.
                         */
                        if (local_member_count > 0) {
                            SHR_BITSET(pbmp_ptr_array[nh_index], local_member_array[0]);
                        }
                    } else {
                        rv = _bcm_esw_modid_is_local(unit, mod_out, &mod_local);
                        if (BCM_FAILURE(rv)) {
                            goto cleanup;
                        }
                        if (mod_local) {
                            SHR_BITSET(pbmp_ptr_array[nh_index], port_out);
                        } else {
                            rv = BCM_E_INTERNAL;
                            goto cleanup;
                        }
                    }

                    /* Advance to next egress object */
                    curr_ptr = curr_ptr->next;
                }
            }
        } else { /* BCM_EXTENDER_PORT_MATCH_NONE is not set */
            if (EXTENDER_PORT_INFO(unit, i)->match_vlan) {
                match_vlan_array[i] = EXTENDER_PORT_INFO(unit, i)->match_vlan;
            }
        }
    }

    /* Maximum virtual port supported on saber2 5626x devices are 4K
     * This fix is related to scaling down of vpn from 8K to 4K
     */
#if defined(BCM_SABER2_SUPPORT)
    if (soc_sb2_5626x_devtype(unit)) {
        sal_memcpy(*scache_ptr, &source_vp_sb2_5626x, sizeof(source_vp_sb2_5626x));
        *scache_ptr += sizeof(source_vp_sb2_5626x);
    }
#endif /* BCM_SABER2_SUPPORT */

    /* Store in scache the bitmap of Extender VPs that were created without
     * match criteria.
     */
    sal_memcpy(*scache_ptr, match_none_vp_bitmap, SHR_BITALLOCSIZE(num_vp));
    *scache_ptr += SHR_BITALLOCSIZE(num_vp);

    /* Store in scache the bitmap of next hops associated with
     * Extender egress objects.
     */
    sal_memcpy(*scache_ptr, nh_bitmap, SHR_BITALLOCSIZE(num_nh));
    *scache_ptr += SHR_BITALLOCSIZE(num_nh);

    /* Store in scache the physical port bitmap of the next hops
     * associated with Extender egress objects.
     */
    for (i = 0; i < num_nh; i++) {
        if (SHR_BITGET(nh_bitmap, i)) {
            sal_memcpy(*scache_ptr, pbmp_ptr_array[i],
                    SHR_BITALLOCSIZE(num_ports));
            *scache_ptr += SHR_BITALLOCSIZE(num_ports);
        }
    }

    /* Store in scache the array of match_vlan */
    sal_memcpy(*scache_ptr, match_vlan_array, sizeof(bcm_vlan_t) * num_vp);
    *scache_ptr += (sizeof(bcm_vlan_t) * num_vp);

    /*
     * Store in scache the bitmap of next hops which are associated
     * with BCM_EXTENDER_EGRESS_MULTICAST.
     */
    sal_memcpy(*scache_ptr, mc_nh_bitmap, SHR_BITALLOCSIZE(num_nh));
    *scache_ptr += SHR_BITALLOCSIZE(num_nh);

cleanup:
    if (NULL != match_none_vp_bitmap) {
        sal_free(match_none_vp_bitmap);
    }
    if (NULL != nh_bitmap) {
        sal_free(nh_bitmap);
    }
    if (NULL != pbmp_ptr_array) {
        for (i = 0; i < num_nh; i++) {
            if (NULL != pbmp_ptr_array[i]) {
                /* pbmp_ptr_array[i] is not null when nh_bitmap[bit i] is set */
                sal_free(pbmp_ptr_array[i]);
            }
        }
        sal_free(pbmp_ptr_array);
    }
    if (NULL != match_vlan_array) {
        sal_free(match_vlan_array);
    }
    if (NULL != mc_nh_bitmap) {
        sal_free(mc_nh_bitmap);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_extender_sd_tag_recover
 * Purpose:
 *      Recover SD-tag fields from a egress next hop entry.
 * Parameters:
 *      unit - Device Number
 *      egr_nh - Egress next hop entry
 *      sd_tag - (OUT) SD-tag fields
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr3_extender_sd_tag_recover(int unit, egr_l3_next_hop_entry_t *egr_nh,
        _bcm_tr3_extender_sd_tag_t *sd_tag)
{
    int sd_tag_action_if_not_present;
    int sd_tag_add;
    int sd_tag_action_if_present;
    int sd_tag_replace_vlan;
    int sd_tag_replace_pri;
    int sd_tag_replace_tpid;
    int sd_tag_delete;
    int tpid_index;
    int pri_select;
    int mapping_ptr;
    int qos_map_type;

    sal_memset(sd_tag, 0, sizeof(_bcm_tr3_extender_sd_tag_t));

    /* GET SD-tag action if packet doesn't have a SD-tag */
    sd_tag_action_if_not_present = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
            egr_nh, SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
    if (1 == sd_tag_action_if_not_present) {
        sd_tag_add = TRUE;
    } else {
        sd_tag_add = FALSE;
    }

    /* GET SD-tag action if packet already has a SD-tag */
    sd_tag_action_if_present = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
            egr_nh, SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
    sd_tag_replace_vlan = FALSE;
    sd_tag_replace_pri = FALSE;
    sd_tag_replace_tpid = FALSE;
    sd_tag_delete = FALSE;
    switch (sd_tag_action_if_present) {
        case 1: /* REPLACE_VID_TPID */
            sd_tag_replace_vlan = TRUE;
            sd_tag_replace_tpid = TRUE;
            break;
        case 2: /* REPLACE_VID_ONLY */
            sd_tag_replace_vlan = TRUE;
            break;
        case 3: /* DELETE SD-TAG */
            sd_tag_delete = TRUE;
            break;
        case 4: /* REPLACE_VID_PRI_TPID */
            sd_tag_replace_vlan = TRUE;
            sd_tag_replace_pri = TRUE;
            sd_tag_replace_tpid = TRUE;
            break;
        case 5: /* REPLACE_VID_PRI_ONLY */
            sd_tag_replace_vlan = TRUE;
            sd_tag_replace_pri = TRUE;
            break;
        case 6: /* REPLACE_PRI_ONLY */
            sd_tag_replace_pri = TRUE;
            break;
        case 7: /* REPLACE_TPID_ONLY */
            sd_tag_replace_tpid = TRUE;
            break;
        default:
            break;
    }

    /* Recover BCM_EXTENDER_EGRESS_SERVICE_xxx flags */
    if (sd_tag_add) {
        sd_tag->flags |= BCM_EXTENDER_EGRESS_SERVICE_VLAN_ADD;
    }
    if (sd_tag_delete) {
        sd_tag->flags |= BCM_EXTENDER_EGRESS_SERVICE_VLAN_DELETE;
    }
    if (sd_tag_replace_vlan) {
        sd_tag->flags |= BCM_EXTENDER_EGRESS_SERVICE_VLAN_REPLACE;
    }
    if (sd_tag_replace_pri) {
        sd_tag->flags |= BCM_EXTENDER_EGRESS_SERVICE_PRI_REPLACE;
    }
    if (sd_tag_replace_tpid) {
        sd_tag->flags |= BCM_EXTENDER_EGRESS_SERVICE_TPID_REPLACE;
    }

    /* Recover SD-tag TPID */
    if (sd_tag_add || sd_tag_replace_tpid) {
        tpid_index = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                egr_nh, SD_TAG__SD_TAG_TPID_INDEXf);
        BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_tab_ref_count_add(unit,
                    tpid_index, 1));
        BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_get(unit,
                    &sd_tag->service_tpid, tpid_index));
    }

    /* Recover SD-tag VLAN ID */
    if (sd_tag_add || sd_tag_replace_vlan) {
        sd_tag->service_vlan = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                egr_nh, SD_TAG__SD_TAG_VIDf);
    }

    /* Recover SD-tag Priority, CFI, and Qos Map ID */
    pri_select = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
            egr_nh, SD_TAG__SD_TAG_DOT1P_PRI_SELECTf);
    if (pri_select) {
        mapping_ptr = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                egr_nh, SD_TAG__SD_TAG_DOT1P_MAPPING_PTRf);
        qos_map_type = 2; /* _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS */
        BCM_IF_ERROR_RETURN(_bcm_tr2_qos_idx2id(unit, mapping_ptr,
                    qos_map_type, &sd_tag->service_qos_map_id));
    } else {
        sd_tag->service_pri = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                egr_nh, SD_TAG__NEW_PRIf);
        sd_tag->service_cfi = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                egr_nh, SD_TAG__NEW_CFIf);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_extender_reinit
 * Purpose:
 *      Warm boot recovery for the Extender module
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_tr3_extender_reinit(int unit)
{
    int rv = BCM_E_NONE;
    int num_vp, num_nh, num_ports;
    SHR_BITDCL *match_none_vp_bitmap = NULL;
    SHR_BITDCL *extender_vp_bitmap = NULL;
    SHR_BITDCL *nh_bitmap = NULL;
    SHR_BITDCL *mc_nh_bitmap = NULL;
    SHR_BITDCL **pbmp_ptr_array = NULL;
    bcm_vlan_t *match_vlan_array = NULL;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    uint16 recovered_ver;
    int stable_size;
    uint32 additional_scache_size = 0;
    uint8 *ing_nh_buf = NULL;
    ing_l3_next_hop_entry_t *ing_nh_entry;
    uint8 *egr_nh_buf = NULL;
    egr_l3_next_hop_entry_t *egr_nh_entry;
    int i, index_min, index_max;
    uint32 entry_type = 0, vp = 0, trunk_bit = 0, vntag_action = 0;
    int lower_th, higher_th;
#ifdef BCM_TRIDENT2_SUPPORT
    int etag_dot1p_mapping_ptr, map_id;
#endif
    uint32 pcp_de_select_mode = 0;
    _bcm_tr3_extender_sd_tag_t extender_sd_tag;
    bcm_trunk_t tgid = 0;
    bcm_module_t modid = 0, mod_out = 0;
    bcm_port_t port_num = 0, port_out = 0;
    int num_valid_ports;
    int j;
    _bcm_tr3_extender_egress_t *curr_ptr = NULL;
    bcm_gport_t gport;
    uint8 *svp_buf = NULL;
    source_vp_entry_t *svp_entry;
    int tpid_enable;
    int  vp_mc, mc_flags;
    bcm_pbmp_t pbmp;
    _bcm_vp_info_t vp_info;
    int  old_num_vp = 0;
#if defined(BCM_SABER2_SUPPORT)
    uint32 source_vp_sb2_5626x = 0;
#endif /* BCM_SABER2_SUPPORT */
    soc_field_t    entry_type_field;
    uint32 dv, dt = SOC_MEM_FIF_DEST_INVALID;

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeExtender;

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    num_nh = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    num_ports = soc_mem_field_length(unit, L3_IPMCm, L3_BITMAPf);

    /* Allocate a bitmap of Extender VPs that were created without match criteria */
    match_none_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp),
            "Bitmap of Extender VPs without match criteria");
    if (NULL == match_none_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(match_none_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

    /* Allocate a bitmap of Extender VPs */
    extender_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "Bitmap of Extender VPs");
    if (NULL == extender_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(extender_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

    /* Allocate a bitmap of next hops associated with Extender egress objects */
    nh_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_nh), "Bitmap of NHs");
    if (NULL == nh_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(nh_bitmap, 0, SHR_BITALLOCSIZE(num_nh));

    /* Allocate an array of pointers to port bitmaps */
    pbmp_ptr_array = sal_alloc(sizeof(SHR_BITDCL *) * num_nh,
            "Array of pointers to pbmp");
    if (NULL == pbmp_ptr_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(pbmp_ptr_array, 0, sizeof(SHR_BITDCL *) * num_nh);

    /* Allocate an array of match_vlan */
    match_vlan_array = sal_alloc(sizeof(bcm_vlan_t) * num_vp,
            "Array of match_vlan");
    if (NULL == match_vlan_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(match_vlan_array, 0, sizeof(bcm_vlan_t) * num_vp);

    /* Allocate a bitmap of next hops associated with multicast Extender egress objects */
    mc_nh_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_nh), "Bitmap of multicast NHs");
    if (NULL == mc_nh_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(mc_nh_bitmap, 0, SHR_BITALLOCSIZE(num_nh));

    rv = soc_stable_size_get(unit, &stable_size);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    /* Read scache */
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_EXTENDER, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
            0, &scache_ptr, BCM_WB_DEFAULT_VERSION, &recovered_ver);

    if (BCM_E_NOT_FOUND == rv) {
        if ((stable_size == 0) || SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
            rv = BCM_E_NONE;
        } else {
            /* Get scache size required for default version */
            rv = bcm_tr3_extender_required_scache_size_get(unit,
                     &additional_scache_size);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            /* Upgrading from SDK release that does not have warmboot state */
            rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                    additional_scache_size, &scache_ptr,
                    BCM_WB_DEFAULT_VERSION, NULL);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }

    } else if (BCM_FAILURE(rv)) {
        goto cleanup;
    } else {
        /* Maximum virtual port supported on saber2 5626x devices are 4K
         * This fix is related to scaling down of vpn from 8K to 4K
         */
#if defined(BCM_SABER2_SUPPORT)
        if (soc_sb2_5626x_devtype(unit)) {
            sal_memcpy(&source_vp_sb2_5626x, scache_ptr,
                    sizeof(source_vp_sb2_5626x));
            if (source_vp_sb2_5626x == 0xFED5ABC7) {
                scache_ptr += sizeof(source_vp_sb2_5626x);
            } else {
                old_num_vp = 1;
            }
        }
#endif /* BCM_SABER2_SUPPORT */
        /* Recover the bitmap of Extender VPs that were created without
         * match criteria.
         */
        sal_memcpy(match_none_vp_bitmap, scache_ptr, SHR_BITALLOCSIZE(num_vp));
        if (old_num_vp) {
            scache_ptr += SHR_BITALLOCSIZE(num_vp * 2);
        } else {
            scache_ptr += SHR_BITALLOCSIZE(num_vp);
        }

        /* Recover the bitmap of next hops associated with
         * Extender egress objects.
         */
        sal_memcpy(nh_bitmap, scache_ptr, SHR_BITALLOCSIZE(num_nh));
        scache_ptr += SHR_BITALLOCSIZE(num_nh);

        /* Recover the physical port bitmap of the next hops associated
         * with Extender egress objects.
         */
        for (i = 0; i < num_nh; i++) {
            if (SHR_BITGET(nh_bitmap, i)) {
                pbmp_ptr_array[i] = sal_alloc(SHR_BITALLOCSIZE(num_ports),
                        "NH pbmp");
                sal_memcpy(pbmp_ptr_array[i], scache_ptr,
                        SHR_BITALLOCSIZE(num_ports));
                scache_ptr += SHR_BITALLOCSIZE(num_ports);
            }
        }

        sal_memcpy(match_vlan_array, scache_ptr, sizeof(bcm_vlan_t) * num_vp);
        if (old_num_vp) {
            scache_ptr += (sizeof(bcm_vlan_t) * num_vp * 2);
        } else {
            scache_ptr += (sizeof(bcm_vlan_t) * num_vp);
        }

        /* Recover the bitmap of next hops associated with
         * multicast Extender egress objects.
         */
        sal_memcpy(mc_nh_bitmap, scache_ptr, SHR_BITALLOCSIZE(num_nh));
        scache_ptr += SHR_BITALLOCSIZE(num_nh);

        if (additional_scache_size > 0) {
            rv = soc_scache_realloc(unit, scache_handle,additional_scache_size);
            if(BCM_FAILURE(rv)) {
               goto cleanup;
            }
        }
    }

     /* Recover the BCM_EXTENDER_PORT_MATCH_NONE from match_none_vp_bitmap */
    for (i = 0; i < num_vp; i++) {
        if (SHR_BITGET(match_none_vp_bitmap, i)) {
            EXTENDER_PORT_INFO(unit, i)->flags |= BCM_EXTENDER_PORT_MATCH_NONE;
        }
    }

    /* Copy match_none_vp_bitmap to extender_vp_bitmap */
    sal_memcpy(extender_vp_bitmap, match_none_vp_bitmap, SHR_BITALLOCSIZE(num_vp));

    /* Recover EXTENDER ports and EXTENDER egress objects from next hop tables */
    ing_nh_buf = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, ING_L3_NEXT_HOPm), "Ing Next Hop buffer");
    if (NULL == ing_nh_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, ING_L3_NEXT_HOPm);
    index_max = soc_mem_index_max(unit, ING_L3_NEXT_HOPm);
    rv = soc_mem_read_range(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
            index_min, index_max, ing_nh_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    egr_nh_buf = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, EGR_L3_NEXT_HOPm), "Egr Next Hop buffer");
    if (NULL == egr_nh_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, EGR_L3_NEXT_HOPm);
    index_max = soc_mem_index_max(unit, EGR_L3_NEXT_HOPm);
    rv = soc_mem_read_range(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
            index_min, index_max, egr_nh_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    /* Get multicast E-VID range */
    rv = bcm_esw_switch_control_get(unit,
            bcmSwitchExtenderMulticastLowerThreshold, &lower_th);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }
    rv = bcm_esw_switch_control_get(unit,
            bcmSwitchExtenderMulticastHigherThreshold, &higher_th);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    /* Check entry type */
    if (SOC_IS_TRIDENT3X(unit)) {
        entry_type_field = DATA_TYPEf;
    } else {
        entry_type_field = ENTRY_TYPEf;
    }

    for (i = index_min; i <= index_max; i++) {
        egr_nh_entry = soc_mem_table_idx_to_pointer(unit, EGR_L3_NEXT_HOPm,
                egr_l3_next_hop_entry_t *, egr_nh_buf, i);

        /* Check entry type */
        entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                entry_type_field);

        if (entry_type == _BCM_EXTENDER_EGR_NEXT_HOP_SDTAG_VIEW) {
            /* Check that VN-tag action is ADD */
            vntag_action = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    SD_TAG__VNTAG_ACTIONSf);
            if ((ETAG_ADD_OR_REPLACE!= vntag_action) && (ETAG_NOOP != vntag_action)) {
                continue;
            }

            /* Check pcp_de_select mode */
            pcp_de_select_mode = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    SD_TAG__ETAG_PCP_DE_SOURCEf);
            if ((BCM_EXTENDER_PCP_DE_SELECT_DEFAULT != pcp_de_select_mode)
                && (BCM_EXTENDER_PCP_DE_SELECT_PHB != pcp_de_select_mode)) {
                continue;
            }

        } else if (entry_type == _BCM_EXTENDER_EGR_NEXT_HOP_L3MC_VIEW) {
            if (!soc_feature(unit, soc_feature_virtual_port_routing)) {
                continue;
            }

            /* Check that VN-tag action is ADD */
            vntag_action = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    L3MC__VNTAG_ACTIONSf);
            if ((ETAG_ADD_OR_REPLACE != vntag_action) && (ETAG_NOOP != vntag_action)) {
                continue;
            }

            /* Check pcp_de_select mode */
            pcp_de_select_mode = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    L3MC__ETAG_PCP_DE_SOURCEf);
            if ((BCM_EXTENDER_PCP_DE_SELECT_DEFAULT != pcp_de_select_mode)
                && (BCM_EXTENDER_PCP_DE_SELECT_PHB != pcp_de_select_mode)) {
                continue;
            }
        } else {
            /* Not Extender virtual port entry type */
            continue;
        }

        if (entry_type == _BCM_EXTENDER_EGR_NEXT_HOP_SDTAG_VIEW) {
            /* Recover VP */
            vp = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    SD_TAG__DVPf);
        } else if (entry_type == _BCM_EXTENDER_EGR_NEXT_HOP_L3MC_VIEW) {
            /* L3MC*/
            vp = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    L3MC__DVPf);
        }

        if ((stable_size == 0) || SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
            rv = _bcm_vp_used_set(unit, vp, vp_info);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        } else {
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                /* VP bitmap is recovered by virtual_init */
                continue;
            }
        }

        /* Add VP to EXTENDER VP bitmap */
        SHR_BITSET(extender_vp_bitmap, vp);

        if (!(EXTENDER_PORT_INFO(unit, vp)->flags & BCM_EXTENDER_PORT_MATCH_NONE)) {
            if (entry_type == _BCM_EXTENDER_EGR_NEXT_HOP_SDTAG_VIEW) {
                /* Recover ETAG PCP and DE info */
                if (BCM_EXTENDER_PCP_DE_SELECT_DEFAULT == pcp_de_select_mode) {
                    EXTENDER_PORT_INFO(unit, vp)->pcp_de_select =
                            BCM_EXTENDER_PCP_DE_SELECT_DEFAULT;
                    EXTENDER_PORT_INFO(unit, vp)->pcp = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                            egr_nh_entry, SD_TAG__ETAG_PCPf);
                    EXTENDER_PORT_INFO(unit, vp)->de = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                            egr_nh_entry, SD_TAG__ETAG_DEf);
                } else if (BCM_EXTENDER_PCP_DE_SELECT_PHB == pcp_de_select_mode) {
                    EXTENDER_PORT_INFO(unit, vp)->pcp_de_select =
                        BCM_EXTENDER_PCP_DE_SELECT_PHB;
#ifdef BCM_TRIDENT2_SUPPORT
                    /* Get ETAG priority mapping profile pointer */
                    etag_dot1p_mapping_ptr = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                            egr_nh_entry, SD_TAG__ETAG_DOT1P_MAPPING_PTRf);
#ifdef BCM_TOMAHAWK_SUPPORT
                    if (SOC_IS_TOMAHAWKX(unit)) {
                         rv = bcm_th_qos_egr_etag_profile2id(unit, etag_dot1p_mapping_ptr,
                                                             &map_id);
                    } else
#endif
                    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
                        rv = bcm_td2_qos_egr_etag_profile2id(unit, etag_dot1p_mapping_ptr,
                                                             &map_id);
                    } else {
                        rv = BCM_E_BADID;
                    }
                    if (rv == BCM_E_NONE) {
                        EXTENDER_PORT_INFO(unit, vp)->qos_map_id = map_id;
                    }
#endif
                }

                /* Recover ETAG VID */
                EXTENDER_PORT_INFO(unit, vp)->extended_port_vid =
                        soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        SD_TAG__VNTAG_DST_VIFf);

                /* Recover multicast flag */
                if ((EXTENDER_PORT_INFO(unit, vp)->extended_port_vid >= lower_th) &&
                    (EXTENDER_PORT_INFO(unit, vp)->extended_port_vid <= higher_th)) {
                    EXTENDER_PORT_INFO(unit, vp)->flags |= BCM_EXTENDER_PORT_MULTICAST;
                }

                /* Recover transparent flag */
                if (vntag_action == ETAG_NOOP) {
                    EXTENDER_PORT_INFO(unit, vp)->flags |=
                            BCM_EXTENDER_PORT_EGRESS_TRANSPARENT;
                }

                /* Recover physical trunk or port from ingress next hop table */
                ing_nh_entry = soc_mem_table_idx_to_pointer(unit,
                        ING_L3_NEXT_HOPm, ing_l3_next_hop_entry_t *, ing_nh_buf, i);
                if (soc_feature(unit, soc_feature_generic_dest)) {
                    dv = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm, ing_nh_entry,
                                                  DESTINATIONf, &dt);
                    if (dt == SOC_MEM_FIF_DEST_LAG) {
                        tgid = dv;
                        trunk_bit = 1;
                    } else {
                        trunk_bit = 0;
                        port_num = dv & SOC_MEM_FIF_DGPP_PORT_MASK;
                        modid = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                                    SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
                    }
                } else {
                    trunk_bit = soc_ING_L3_NEXT_HOPm_field32_get(unit, ing_nh_entry, Tf);
                    if (trunk_bit) {
                        tgid = soc_ING_L3_NEXT_HOPm_field32_get(unit, ing_nh_entry,
                                                                TGIDf);
                    } else {
                        modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, ing_nh_entry,
                                                                 MODULE_IDf);
                        port_num = soc_ING_L3_NEXT_HOPm_field32_get(unit, ing_nh_entry,
                                                                    PORT_NUMf);
                    }
                }

                if (trunk_bit) {
                    BCM_GPORT_TRUNK_SET(EXTENDER_PORT_INFO(unit, vp)->port, tgid);
                } else {
                    rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                            modid, port_num, &mod_out, &port_out);
                    if (BCM_FAILURE(rv)) {
                        goto cleanup;
                    }
                    BCM_GPORT_MODPORT_SET(EXTENDER_PORT_INFO(unit, vp)->port,
                            mod_out, port_out);
                }

                /* The match_vlan can be recovered from either match_vlan_array
                 * or EGR_L3_NEXT_HOP.SD_TAG_VID field. The latter approach
                 * is taken since older SDKs stored match_vlan in EGR_L3_NEXT_HOP
                 * entry, not in scache.
                 */
                EXTENDER_PORT_INFO(unit, vp)->match_vlan =
                        soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        SD_TAG__SD_TAG_VIDf);

                if (stable_size == 0) {
                    /* In the Port module, a port's VP count is not recovered in
                     * level 1 Warm Boot.
                     */
                    rv = _bcm_tr3_extender_port_cnt_update(unit,
                            EXTENDER_PORT_INFO(unit, vp)->port, vp, TRUE);
                    if (BCM_FAILURE(rv)) {
                        goto cleanup;
                    }
                }
            }
        } else { /* The Extender port was created without match criteria */
            vp_mc = 0;
            BCM_PBMP_CLEAR(pbmp);
            if (entry_type == _BCM_EXTENDER_EGR_NEXT_HOP_SDTAG_VIEW) {
                rv = _bcm_tr3_extender_sd_tag_recover(unit, egr_nh_entry, &extender_sd_tag);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }

                num_valid_ports = 0;
                for (j = 0; j < num_ports; j++) {
                    if ((pbmp_ptr_array[i] != NULL) ?
                        (SHR_BITGET(pbmp_ptr_array[i], j)) : 0) {
                        /* Insert a new Extender egress object into VP's egress list */
                        curr_ptr = sal_alloc(sizeof(_bcm_tr3_extender_egress_t),
                                "Extender egress object");
                        if (NULL == curr_ptr) {
                            rv = BCM_E_MEMORY;
                            goto cleanup;
                        }
                        sal_memset(curr_ptr, 0, sizeof(_bcm_tr3_extender_egress_t));

                        curr_ptr->extended_port_vid =
                                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                                SD_TAG__VNTAG_DST_VIFf);

                        /* Recover multicast flag */
                        if (curr_ptr->extended_port_vid >= lower_th &&
                            curr_ptr->extended_port_vid <= higher_th) {
                            curr_ptr->flags |= BCM_EXTENDER_EGRESS_MULTICAST;
                        }

                        if (BCM_EXTENDER_PCP_DE_SELECT_DEFAULT == pcp_de_select_mode) {
                            curr_ptr->pcp_de_select =
                                    BCM_EXTENDER_PCP_DE_SELECT_DEFAULT;
                            curr_ptr->pcp = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                    egr_nh_entry, SD_TAG__ETAG_PCPf);
                            curr_ptr->de = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                                egr_nh_entry, SD_TAG__ETAG_DEf);
                        } else if (BCM_EXTENDER_PCP_DE_SELECT_PHB == pcp_de_select_mode) {
                            curr_ptr->pcp_de_select = BCM_EXTENDER_PCP_DE_SELECT_PHB;
#ifdef BCM_TRIDENT2_SUPPORT
                            /* Get ETAG priority mapping profile pointer */
                            etag_dot1p_mapping_ptr = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                    egr_nh_entry, SD_TAG__ETAG_DOT1P_MAPPING_PTRf);
                            rv = bcm_td2_qos_egr_etag_profile2id(unit,
                                    etag_dot1p_mapping_ptr, &map_id);
                            if (rv == BCM_E_NONE) {
                                curr_ptr->qos_map_id = map_id;
                            }
#endif
                        }
                        rv = bcm_esw_port_gport_get(unit, j, &gport);
                        if (BCM_FAILURE(rv)) {
                            sal_free(curr_ptr);
                            goto cleanup;
                        }
                        rv = bcm_esw_trunk_find(unit, BCM_MODID_INVALID, gport, &tgid);
                        if (BCM_SUCCESS(rv)) {
                            BCM_GPORT_TRUNK_SET(curr_ptr->port, tgid);
                        } else if (rv == BCM_E_NOT_FOUND) {
                            rv = BCM_E_NONE;
                            curr_ptr->port = gport;
                        } else {
                            sal_free(curr_ptr);
                            goto cleanup;
                        }

                        curr_ptr->match_vlan = match_vlan_array[vp];
                        curr_ptr->flags |= extender_sd_tag.flags;
                        curr_ptr->service_tpid = extender_sd_tag.service_tpid;
                        curr_ptr->service_vlan = extender_sd_tag.service_vlan;
                        curr_ptr->service_pri = extender_sd_tag.service_pri;
                        curr_ptr->service_cfi = extender_sd_tag.service_cfi;
                        curr_ptr->service_qos_map_id = extender_sd_tag.service_qos_map_id;
                        curr_ptr->next_hop_index = i;
                        curr_ptr->next = EXTENDER_PORT_INFO(unit, vp)->egress_list;
                        if (curr_ptr->flags & BCM_EXTENDER_EGRESS_MULTICAST) {
                            vp_mc = 1;
                            rv = _bcm_tr3_extender_egress_hash_recover(unit, vp, curr_ptr);
                            if (BCM_FAILURE(rv)) {
                                sal_free(curr_ptr);
                                goto cleanup;
                            }
                            BCM_PBMP_PORT_ADD(pbmp, j);
                        } else {
                            EXTENDER_PORT_INFO(unit, vp)->egress_list = curr_ptr;
                        }

                        /* Increment the number of ports in port bitmap */
                        num_valid_ports++;
                    }
                }

                /* During warm boot, L3 module had already incremented reference
                 * count for non-null next hop entries. Hence, the next hop's
                 * reference count should be incremented further by num_valid_ports
                 * minus one.
                 */
                for (j = 0; j < num_valid_ports - 1; j++) {
                    _bcm_xgs3_nh_ref_cnt_incr(unit, i);
                }
            } else if (entry_type == _BCM_EXTENDER_EGR_NEXT_HOP_L3MC_VIEW) {
                num_valid_ports = 0;
                vp_mc = 1;
                for (j = 0; j < num_ports; j++) {
                    if ((pbmp_ptr_array[i] != NULL) ?
                        (!SHR_BITGET(pbmp_ptr_array[i], j)) : 1) {
                        continue;
                    }
                    /* Insert a new Extender egress object into VP's egress list */
                    curr_ptr = sal_alloc(sizeof(_bcm_tr3_extender_egress_t),
                            "Extender egress object");
                    if (NULL == curr_ptr) {
                        rv = BCM_E_MEMORY;
                        goto cleanup;
                    }
                    sal_memset(curr_ptr, 0, sizeof(_bcm_tr3_extender_egress_t));

                    /*entry_type == 7, L3MC */
                    curr_ptr->flags |= BCM_EXTENDER_EGRESS_L3;

                    if (BCM_EXTENDER_PCP_DE_SELECT_DEFAULT == pcp_de_select_mode) {
                        curr_ptr->pcp_de_select = BCM_EXTENDER_PCP_DE_SELECT_DEFAULT;
                        curr_ptr->pcp = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                egr_nh_entry, L3MC__ETAG_PCPf);
                        curr_ptr->de = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                    egr_nh_entry, L3MC__ETAG_DEf);
                    } else if (BCM_EXTENDER_PCP_DE_SELECT_PHB == pcp_de_select_mode) {
                        curr_ptr->pcp_de_select = BCM_EXTENDER_PCP_DE_SELECT_PHB;
#ifdef BCM_TRIDENT2_SUPPORT
                        /* Get ETAG priority mapping profile pointer */
                        etag_dot1p_mapping_ptr = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                egr_nh_entry, L3MC__ETAG_DOT1P_MAPPING_PTRf);
                        rv = bcm_td2_qos_egr_etag_profile2id(unit,
                                etag_dot1p_mapping_ptr, &map_id);
                        if (rv == BCM_E_NONE) {
                            curr_ptr->qos_map_id = map_id;
                        }
#endif
                    }
                    rv = bcm_esw_port_gport_get(unit, j, &gport);
                    if (BCM_FAILURE(rv)) {
                        sal_free(curr_ptr);
                        goto cleanup;
                    }
                    rv = bcm_esw_trunk_find(unit, BCM_MODID_INVALID, gport, &tgid);
                    if (BCM_SUCCESS(rv)) {
                        BCM_GPORT_TRUNK_SET(curr_ptr->port, tgid);
                    } else if (rv == BCM_E_NOT_FOUND) {
                        rv = BCM_E_NONE;
                        curr_ptr->port = gport;
                    } else {
                        sal_free(curr_ptr);
                        goto cleanup;
                    }
                    curr_ptr->extended_port_vid = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        L3MC__VNTAG_DST_VIFf);
                    /* Recover multicast flag */
                    if (curr_ptr->extended_port_vid >= lower_th &&
                        curr_ptr->extended_port_vid <= higher_th) {
                        curr_ptr->flags |= BCM_EXTENDER_EGRESS_MULTICAST;
                    }
                    curr_ptr->next_hop_index = i;
                    curr_ptr->intf = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        L3MC__INTF_NUMf);

                    mc_flags = 0;
                    if(soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        L3MC__L2_MC_DA_DISABLEf)) {
                        mc_flags |= BCM_L3_MULTICAST_L2_DEST_PRESERVE;
                    }
                    if(soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        L3MC__L2_MC_SA_DISABLEf)) {
                        mc_flags |= BCM_L3_MULTICAST_L2_SRC_PRESERVE;
                    }
                    if(soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        L3MC__L2_MC_VLAN_DISABLEf)) {
                        mc_flags |= BCM_L3_MULTICAST_L2_VLAN_PRESERVE;
                    }
                    if(soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        L3MC__L3_MC_TTL_DISABLEf)) {
                        mc_flags |= BCM_L3_MULTICAST_TTL_PRESERVE;
                    }
                    if(soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        L3MC__L3_MC_DA_DISABLEf)) {
                        mc_flags |= BCM_L3_MULTICAST_DEST_PRESERVE;
                    }
                    if(soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        L3MC__L3_MC_SA_DISABLEf)) {
                        mc_flags |= BCM_L3_MULTICAST_SRC_PRESERVE;
                    }
                    if(soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        L3MC__L3_MC_VLAN_DISABLEf)) {
                        mc_flags |= BCM_L3_MULTICAST_VLAN_PRESERVE;
                    }
                    if(soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        L3MC__L3_DROPf)) {
                        mc_flags |= BCM_L3_MULTICAST_L3_DROP;
                    }
                    if(soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        L3MC__L2_DROPf)) {
                        mc_flags |= BCM_L3_MULTICAST_L2_DROP;
                    }
                    curr_ptr->multicast_flags |= mc_flags;
                    rv = _bcm_tr3_extender_egress_hash_recover(
                             unit, vp, curr_ptr);
                    if (BCM_FAILURE(rv)) {
                        sal_free(curr_ptr);
                        goto cleanup;
                    }
                    /* Increment the number of ports in port bitmap */
                    num_valid_ports++;
                    BCM_PBMP_PORT_ADD(pbmp, j);
                }

                /* During warm boot, L3 module had already incremented reference
                 * count for non-null next hop entries. Hence, the next hop's
                 * reference count should be incremented further by num_valid_ports
                 * minus one.
                 */
                for (j = 0; j < num_valid_ports - 1; j++) {
                    _bcm_xgs3_nh_ref_cnt_incr(unit, i);
                }
            }

            if (vp_mc) {
                rv = _bcm_tr3_extender_egress_pbmp_add(unit, vp, pbmp);
                if (BCM_FAILURE(rv)) {
                    sal_free(curr_ptr);
                    goto cleanup;
                }
            }
        }
    }

    /* Recover SD-tag TPID reference count from SOURCE_VP table */
    if (entry_type == _BCM_EXTENDER_EGR_NEXT_HOP_SDTAG_VIEW) {
        svp_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, SOURCE_VPm),
                "SOURCE_VP buffer");
        if (NULL == svp_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        index_min = soc_mem_index_min(unit, SOURCE_VPm);
        index_max = soc_mem_index_max(unit, SOURCE_VPm);
        rv = soc_mem_read_range(unit, SOURCE_VPm, MEM_BLOCK_ANY,
                index_min, index_max, svp_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
        for (i = index_min; i <= index_max; i++) {
            svp_entry = soc_mem_table_idx_to_pointer
                (unit, SOURCE_VPm, source_vp_entry_t *, svp_buf, i);
            tpid_enable = soc_SOURCE_VPm_field32_get(unit, svp_entry, TPID_ENABLEf);
            if (tpid_enable) {
                for (j = 0; j < soc_mem_field_length(unit, SOURCE_VPm, TPID_ENABLEf);
                        j++) {
                    if (tpid_enable & (1 << j)) {
                        rv = _bcm_fb2_outer_tpid_tab_ref_count_add(unit, j, 1);
                        if (SOC_FAILURE(rv)) {
                            goto cleanup;
                        }
                    }
                }
            }
        }
    }

cleanup:
    if (match_none_vp_bitmap) {
        sal_free(match_none_vp_bitmap);
    }
    if (extender_vp_bitmap) {
        sal_free(extender_vp_bitmap);
    }
    if (nh_bitmap) {
        sal_free(nh_bitmap);
    }
    if (mc_nh_bitmap) {
        sal_free(mc_nh_bitmap);
    }
    if (pbmp_ptr_array) {
        for (i = 0; i < num_nh; i++) {
            if (pbmp_ptr_array[i]) {
                sal_free(pbmp_ptr_array[i]);
            }
        }
        sal_free(pbmp_ptr_array);
    }
    if (match_vlan_array) {
        sal_free(match_vlan_array);
    }
    if (ing_nh_buf) {
        soc_cm_sfree(unit, ing_nh_buf);
    }
    if (egr_nh_buf) {
        soc_cm_sfree(unit, egr_nh_buf);
    }
    if (svp_buf) {
        soc_cm_sfree(unit, svp_buf);
    }

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_tr3_extender_free_resources
 * Purpose:
 *      Free all allocated tables and memory
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_tr3_extender_free_resources(int unit)
{
    if (EXTENDER_INFO(unit)->port_info) {
        sal_free(EXTENDER_INFO(unit)->port_info);
        EXTENDER_INFO(unit)->port_info = NULL;
    }
}

/*
 * Function:
 *      bcm_tr3_extender_init
 * Purpose:
 *      Initialize the Extender module.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_extender_init(int unit)
{
    int num_vp;
    int rv = BCM_E_NONE;

    sal_memset(EXTENDER_INFO(unit), 0,
            sizeof(_bcm_tr3_extender_bookkeeping_t));

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    if (NULL == EXTENDER_INFO(unit)->port_info) {
        EXTENDER_INFO(unit)->port_info = sal_alloc
            (sizeof(_bcm_tr3_extender_port_info_t) * num_vp,
             "extender_port_info");
        if (NULL == EXTENDER_INFO(unit)->port_info) {
            _bcm_tr3_extender_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(EXTENDER_INFO(unit)->port_info, 0,
            sizeof(_bcm_tr3_extender_port_info_t) * num_vp);
    /* When creating a EXTENDER virtual port without any match criteria,
     * the ING_DVP_TABLE.NEXT_HOP_INDEX is configured with an invalid
     * next hop index, which is assigned to be the black hole next hop index.
     */
    EXTENDER_INFO(unit)->invalid_next_hop_index = BCM_XGS3_L3_BLACK_HOLE_NH_IDX(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        /* Warm Boot recovery */
        rv = _bcm_tr3_extender_reinit(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_extender_free_resources(unit);
        }
    } else
#endif /* BCM_WARM_BOOT_SUPPORT */
    {
        /* Initialize ETAG VID multicast range */
        BCM_IF_ERROR_RETURN(bcm_esw_switch_control_set(unit,
                    bcmSwitchExtenderMulticastLowerThreshold, 0x1000));
        BCM_IF_ERROR_RETURN(bcm_esw_switch_control_set(unit,
                    bcmSwitchExtenderMulticastHigherThreshold, 0x3fff));
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_td3_rx_etype_pe[unit] = 0;
    }
#endif

    return rv;
}

/*
 * Function:
 *      bcm_tr3_extender_cleanup
 * Purpose:
 *      Detach the Extender module.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_extender_cleanup(int unit)
{
    _bcm_tr3_extender_free_resources(unit);

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_td3_rx_etype_pe[unit] = 0;
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_extender_failover_add
 * Purpose:
 *      Setup a failover extender port id.
 * Parameters:
 *      unit       - (IN) SOC unit number. 
 *      extender_port  - (IN) Pointer to Extender port structure. 
 *      vp         - (IN) Virtual port number.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr3_extender_failover_add(int unit, bcm_extender_port_t *extender_port, int vp)
{
    int rv = BCM_E_NONE;
    ing_dvp_table_entry_t dvp_entry;
    int nh_index, failover_vp = -1, failover_nh_index = 0;
    uint32 protection_flags = 0;
    bcm_extender_port_t *ep = extender_port;

    if (_BCM_MULTICAST_IS_SET(ep->failover_mc_group)) {
        protection_flags |= _BCM_FAILOVER_1_PLUS_PROTECTION;
    } else {
        /* Get egress next-hop index from Failover EXTENDER gport */
        failover_vp = 
            BCM_GPORT_EXTENDER_PORT_ID_GET(ep->failover_port_id);
        if (failover_vp != -1) {
            BCM_IF_ERROR_RETURN(
                READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, failover_vp,
                                    &dvp_entry));
            failover_nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
                                                               NEXT_HOP_INDEXf);
        }
    }

    BCM_IF_ERROR_RETURN(
        READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry, 
                                              NEXT_HOP_INDEXf);
    /* Set the failover to a given extender port */
    rv = _bcm_esw_failover_prot_nhi_create(unit, protection_flags, 
                                           nh_index, failover_nh_index, 
                                           ep->failover_mc_group,
                                           ep->failover_id);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_extender_failover_delete
 * Purpose:
 *      Delete a failover extender port id.
 * Parameters:
 *      unit       - (IN) SOC unit number. 
 *      gport - (IN) Extender GPORT ID.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr3_extender_failover_delete(int unit, bcm_gport_t gport)
{
    int rv = BCM_E_NONE;
    int nh_index;
    uint32 vp;
    ing_dvp_table_entry_t dvp_entry;

    /* Get egress next-hop index from EXTENDER gport */
    vp = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);

    BCM_IF_ERROR_RETURN(
        READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry, 
                                              NEXT_HOP_INDEXf);
    /* Cleanup the failover from a given extender port */
    rv = _bcm_esw_failover_prot_nhi_cleanup(unit, nh_index);
    if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_extender_failover_get
 * Purpose:
 *      Setup a failover extender port id.
 * Parameters:
 *      unit          - (IN) SOC unit number. 
 *      extender_port - (IN) extender port Id. 
 *      failover_port - (OUT) failover extender port Id. 
 *      failover_id   - (OUT) failover object index.. 
 *      failover_mc_group - (OUT) failover multicast group id. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr3_extender_failover_get(int unit, bcm_gport_t extender_port, 
                               bcm_gport_t *failover_port, 
                               bcm_failover_t *failover_id,
                               bcm_multicast_t *failover_mc_group)
{
    int failover_vp;
    uint32 vp;
    int nh_index, prot_nh_index = -1, multicast_group = 0;
    bcm_failover_t fo_id;
    ing_dvp_table_entry_t dvp_entry;
    egr_l3_next_hop_entry_t egr_nh;

    vp = BCM_GPORT_EXTENDER_PORT_ID_GET(extender_port);

    BCM_IF_ERROR_RETURN
        (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    /* Get the next hop index */
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
                                              NEXT_HOP_INDEXf);
    /* Get the protecting information of this next hop index */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_failover_prot_nhi_get(unit, nh_index, &fo_id,
                                        &prot_nh_index, &multicast_group));
    if (fo_id > 0) {
        if (_BCM_MULTICAST_IS_SET(multicast_group)) {
            if (failover_mc_group) {
                *failover_mc_group = multicast_group;
            }
        } else {
            if (failover_port) {
                /* Read the existing egress next_hop entry */
                BCM_IF_ERROR_RETURN
                    (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                  prot_nh_index, &egr_nh));
                /* retrieve the failover virtual port from egress next hop entry */
                failover_vp = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                               SD_TAG__DVPf);
                /* failover port should also be a extender port */
                if (!_bcm_vp_used_get(unit, failover_vp, _bcmVpTypeExtender)) {
                    return BCM_E_PARAM;
                }
                BCM_GPORT_EXTENDER_PORT_ID_SET(*failover_port, failover_vp);
            }
        }
        if (failover_id) {
            *failover_id = fo_id;
        }

        return BCM_E_NONE;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_tr3_extender_nh_info_set
 * Purpose:
 *      Get a next hop index and configure next hop tables.
 * Parameters:
 *      unit       - (IN) SOC unit number.
 *      extender_port  - (IN) Pointer to Extender port structure.
 *      vp         - (IN) Virtual port number.
 *      drop       - (IN) Drop indication.
 *      nh_index   - (IN/OUT) Next hop index.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr3_extender_nh_info_set(int unit, bcm_extender_port_t *extender_port,
        _bcm_tr3_extender_sd_tag_t *sd_tag, int vp, int drop, int *nh_index)
{
    int rv;
    uint32 nh_flags;
    bcm_l3_egress_t nh_info;
    egr_l3_next_hop_entry_t egr_nh;
    uint8 egr_nh_entry_type;
    int etag_dot1p_mapping_ptr;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int id;
    int ing_nh_port;
    int ing_nh_module;
    int ing_nh_trunk;
    ing_l3_next_hop_entry_t ing_nh;
    initial_ing_l3_next_hop_entry_t initial_ing_nh;
    int sd_tag_add = 0;
    int sd_tag_delete = 0;
    int sd_tag_replace_vlan = 0;
    int sd_tag_replace_pri = 0;
    int sd_tag_replace_tpid = 0;
    int tpid_index;
    int mapping_ptr;
    soc_field_t ent_type_f = ENTRY_TYPEf;
    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
        ent_type_f = DATA_TYPEf;
    }

    /* Get a next hop index */

    if (extender_port->flags & BCM_EXTENDER_PORT_REPLACE) {
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

    if (extender_port->flags & BCM_EXTENDER_PORT_REPLACE) {
        /* Read the existing egress next_hop entry */
        rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                *nh_index, &egr_nh);
        BCM_IF_ERROR_RETURN(rv);

        /* Be sure that the existing entry is programmed to SD-tag */
        egr_nh_entry_type = 
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ent_type_f);
        if (egr_nh_entry_type != 0x2) { /* != SD-tag */
            return BCM_E_PARAM;
        }

        /* Get ETAG priority mapping profile pointer */
        etag_dot1p_mapping_ptr = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh, SD_TAG__ETAG_DOT1P_MAPPING_PTRf);
    } else {
        egr_nh_entry_type = 0x2; /* SD-tag */
        etag_dot1p_mapping_ptr = 0;
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            bcm_th_qos_egr_etag_id2profile(unit, extender_port->qos_map_id,
                                           &etag_dot1p_mapping_ptr);
        }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)){
            bcm_td2_qos_egr_etag_id2profile(unit, extender_port->qos_map_id,
                                            &etag_dot1p_mapping_ptr);
        }
#endif
    }

    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            ent_type_f, egr_nh_entry_type);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            SD_TAG__DVPf, vp);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
            SD_TAG__HG_HDR_SELf, 1);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
            SD_TAG__VNTAG_DST_VIFf, extender_port->extended_port_vid);
    if (extender_port->pcp_de_select == BCM_EXTENDER_PCP_DE_SELECT_DEFAULT) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                SD_TAG__ETAG_PCP_DE_SOURCEf, 2);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                SD_TAG__ETAG_PCPf, extender_port->pcp);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                SD_TAG__ETAG_DEf, extender_port->de);
    } else if (extender_port->pcp_de_select == BCM_EXTENDER_PCP_DE_SELECT_PHB) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                SD_TAG__ETAG_PCP_DE_SOURCEf, 3);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                SD_TAG__ETAG_DOT1P_MAPPING_PTRf, etag_dot1p_mapping_ptr);
    } else {
        return BCM_E_PARAM;
    }
    /* For the transparent port, do not modify the ETAG. */
    if ((extender_port->flags & BCM_EXTENDER_PORT_EGRESS_TRANSPARENT) == 0) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                            SD_TAG__VNTAG_ACTIONSf, 2);
    }

    if (NULL != sd_tag) {
        sd_tag_add = sd_tag->flags & BCM_EXTENDER_EGRESS_SERVICE_VLAN_ADD;
        sd_tag_delete = sd_tag->flags & BCM_EXTENDER_EGRESS_SERVICE_VLAN_DELETE;
        sd_tag_replace_vlan = sd_tag->flags & BCM_EXTENDER_EGRESS_SERVICE_VLAN_REPLACE;
        sd_tag_replace_pri = sd_tag->flags & BCM_EXTENDER_EGRESS_SERVICE_PRI_REPLACE;
        sd_tag_replace_tpid = sd_tag->flags & BCM_EXTENDER_EGRESS_SERVICE_TPID_REPLACE;

        /* Set SD-tag action for packets without an SD-tag */
        if (sd_tag_add) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf, 1);
        }

        /* Set SD-tag action for packets with an SD-tag */
        if (sd_tag_delete) {
            if (sd_tag_replace_vlan || sd_tag_replace_pri || sd_tag_replace_tpid) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 3); /* DELETE SD-TAG */
        } else {
            if (!sd_tag_replace_vlan && !sd_tag_replace_pri && !sd_tag_replace_tpid) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 0); /* NOOP */
            } else if (sd_tag_replace_vlan && !sd_tag_replace_pri && sd_tag_replace_tpid) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 1); /* REPLACE_VID_TPID */
            } else if (sd_tag_replace_vlan && !sd_tag_replace_pri && !sd_tag_replace_tpid) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 2); /* REPLACE_VID_ONLY */
            } else if (sd_tag_replace_vlan && sd_tag_replace_pri && sd_tag_replace_tpid) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 4); /* REPLACE_VID_PRI_TPID */
            } else if (sd_tag_replace_vlan && sd_tag_replace_pri && !sd_tag_replace_tpid) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 5); /* REPLACE_VID_PRI_ONLY */
            } else if (!sd_tag_replace_vlan && sd_tag_replace_pri && !sd_tag_replace_tpid) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 6); /* REPLACE_PRI_ONLY */
            } else if (!sd_tag_replace_vlan && !sd_tag_replace_pri && sd_tag_replace_tpid) {
                if (SOC_IS_TRIDENT(unit)) {
                    /* REPLACE_TPID_ONLY action is not available on Trident.
                                   * It's available on Triumph3 and Trident2.
                                   */
                    return BCM_E_PARAM;
                }
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 7); /* REPLACE_TPID_ONLY */
            } else {
                return BCM_E_PARAM;
            }
        }

        /* Set SD-tag TPID */
        if (sd_tag->service_tpid) {
            if (sd_tag_add || sd_tag_replace_tpid) {
                BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_add(unit,
                            sd_tag->service_tpid, &tpid_index));
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        SD_TAG__SD_TAG_TPID_INDEXf, tpid_index);
            } else {
                return BCM_E_PARAM;
            }
        } else {
            if (sd_tag_add || sd_tag_replace_tpid) {
                return BCM_E_PARAM;
            }
        }

        /* Set SD-tag VLAN ID */
        if (sd_tag->service_vlan) {
            if (sd_tag_add || sd_tag_replace_vlan) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        SD_TAG__SD_TAG_VIDf, sd_tag->service_vlan);
            } else {
                return BCM_E_PARAM;
            }
        } else {
            if (sd_tag_add || sd_tag_replace_vlan) {
                return BCM_E_PARAM;
            }
        }

        /* Set SD-tag Priority */
        if (sd_tag->service_qos_map_id) {
            if (sd_tag_add || sd_tag_replace_pri) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        SD_TAG__SD_TAG_DOT1P_PRI_SELECTf, 1);
                BCM_IF_ERROR_RETURN(_bcm_tr2_qos_id2idx(unit,
                            sd_tag->service_qos_map_id, &mapping_ptr));
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        SD_TAG__SD_TAG_DOT1P_MAPPING_PTRf, mapping_ptr);
            } else {
                return BCM_E_PARAM;
            }
        } else {
            if (sd_tag_add || sd_tag_replace_pri) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        SD_TAG__SD_TAG_DOT1P_PRI_SELECTf, 0);
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        SD_TAG__NEW_PRIf, sd_tag->service_pri);
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        SD_TAG__NEW_CFIf, sd_tag->service_cfi);
            }
        }
    }

    /* The field SD_TAG_VID is not used for Port Extension.
     * Store match_vlan here for warm boot recovery.
     */
    if (extender_port->match_vlan >= BCM_VLAN_INVALID) {
        return BCM_E_PARAM;
    }

    if (NULL == sd_tag) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                SD_TAG__SD_TAG_VIDf, extender_port->match_vlan);
    } else {
        /* Indicate this egress object is on shared VP */
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                 SD_TAG__RSVD_DVPf, 1);
    }
    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
            MEM_BLOCK_ALL, *nh_index, &egr_nh);
    if (rv < 0) {
    goto cleanup;
    }

    /* Resolve gport */

    rv = _bcm_esw_gport_resolve(unit, extender_port->port, &mod_out, &port_out, &trunk_id, &id);
    if (rv < 0) {
        goto cleanup;
    }

    ing_nh_port = -1;
    ing_nh_module = -1;
    ing_nh_trunk = -1;

    if (BCM_GPORT_IS_TRUNK(extender_port->port)) {
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
        if (soc_feature(unit, soc_feature_generic_dest)) {
            soc_mem_field32_dest_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                ing_nh_module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | ing_nh_port);
        } else {
            soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                    &ing_nh, PORT_NUMf, ing_nh_port);
            soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                    &ing_nh, MODULE_IDf, ing_nh_module);
        }
    } else {
        if (soc_feature(unit, soc_feature_generic_dest)) {
            soc_mem_field32_dest_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_LAG, ing_nh_trunk);
        } else {
            soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                    &ing_nh, Tf, 1);
            soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                    &ing_nh, TGIDf, ing_nh_trunk);
        }
    }

    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, drop);
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
            &ing_nh, ENTRY_TYPEf, 0x2); /* L2 DVP */

    if (soc_mem_field_valid(unit, ING_L3_NEXT_HOPm, DVP_ATTRIBUTE_1_INDEXf)) {
        uint32 mtu_profile_index;

        rv = _bcm_tr3_mtu_profile_index_get(unit, 0x3fff, &mtu_profile_index);
        if (rv < 0) {
            goto cleanup;
        }
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                DVP_ATTRIBUTE_1_INDEXf, mtu_profile_index);
    } else if (SOC_MEM_FIELD_VALID(unit, ING_L3_NEXT_HOPm, MTU_SIZEf)) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                MTU_SIZEf, 0x3fff);
    }

    rv = soc_mem_write (unit, ING_L3_NEXT_HOPm,
            MEM_BLOCK_ALL, *nh_index, &ing_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Write INITIAL_ING_L3_NEXT_HOP entry */

    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    if (ing_nh_trunk == -1) {
        if (soc_feature(unit, soc_feature_generic_dest)) {
            soc_mem_field32_dest_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                &initial_ing_nh, DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                ing_nh_module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | ing_nh_port);
        } else {
            soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                    &initial_ing_nh, PORT_NUMf, ing_nh_port);
            soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                    &initial_ing_nh, MODULE_IDf, ing_nh_module);
        }
    } else {
        if (soc_feature(unit, soc_feature_generic_dest)) {
            soc_mem_field32_dest_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                &initial_ing_nh, DESTINATIONf, SOC_MEM_FIF_DEST_LAG,
                ing_nh_trunk);
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

    return rv;

cleanup:
    if (!(extender_port->flags & BCM_EXTENDER_PORT_REPLACE)) {
        (void) bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, *nh_index);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_extender_nh_info_delete
 * Purpose:
 *      Free next hop index and clear next hop tables.
 * Parameters:
 *      unit     - (IN) SOC unit number. 
 *      nh_index - (IN) Next hop index. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr3_extender_nh_info_delete(int unit, int nh_index)
{
    egr_l3_next_hop_entry_t egr_nh;
    ing_l3_next_hop_entry_t ing_nh;
    initial_ing_l3_next_hop_entry_t initial_ing_nh;
    int ref_count;
    int sd_tag_action_if_not_present;
    int sd_tag_action_if_present;
    int sd_tag_add;
    int sd_tag_replace_tpid;
    int tpid_index;
    uint32 entry_type = 0;
    soc_field_t etfld;

    /* Read egress next hop entry before it's potentially deleted by
     * bcm_xgs3_nh_del.
     */
    BCM_IF_ERROR_RETURN
        (READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, nh_index, &egr_nh));
    /* Check next hop's reference count. If the count is zero,
     * the egress next hop entry was cleared by bcm_xgs3_nh_del.
     * In this case, the EGR_L3_NEXT_HOP.SD_TAG_TPID_INDEX's
     * reference count needs to be decremented.
     */
    BCM_IF_ERROR_RETURN(bcm_xgs3_nh_ref_count_get(unit, nh_index, &ref_count));
    /* Check entry type */
    etfld = SOC_IS_TRIDENT3X(unit) ? DATA_TYPEf : ENTRY_TYPEf;
    entry_type = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh, etfld);

    if (ref_count == 0 && entry_type == 2) {
        sd_tag_action_if_not_present = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                &egr_nh, SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
        if (1 == sd_tag_action_if_not_present) {
            sd_tag_add = TRUE;
        } else {
            sd_tag_add = FALSE;
        }

        sd_tag_action_if_present = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                &egr_nh, SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
        if ((1 == sd_tag_action_if_present) ||
                (4 == sd_tag_action_if_present) ||
                (7 == sd_tag_action_if_present)) {
            sd_tag_replace_tpid = TRUE;
        } else {
            sd_tag_replace_tpid = FALSE;
        }

        if (sd_tag_add || sd_tag_replace_tpid) {
            tpid_index = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                    &egr_nh, SD_TAG__SD_TAG_TPID_INDEXf);
            BCM_IF_ERROR_RETURN
                (_bcm_fb2_outer_tpid_entry_delete(unit, tpid_index));
        }
    }

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
 *      _bcm_tr3_extender_port_nh_info_set
 * Purpose:
 *      Get a next hop index and configure next hop tables.
 * Parameters:
 *      unit       - (IN) SOC unit number.
 *      port              - (IN) Physical GPORT ID.
 *      extender_port_vid - (IN) extender port VID in ETAG.
 *      pcp -             - (IN) PCP field in  ETAG.
 *      de -              - (IN) DE field in  ETAG.
 *      pcp_de_select     - (IN) Selection of PCP and DE fields for egress ETAG.
 *      qos_map_id-         (IN) Qos map id for egress etag mapping profile.
 *      sd_tag           - (IN) SD-tag attributes.
 *      vp         - (IN) Virtual port number.
 *      nh_index   - (IN/OUT) Next hop index.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr3_extender_port_nh_info_set(int unit,
        bcm_gport_t port, uint16 extended_port_vid,
        bcm_vlan_t match_vlan, uint8 pcp, uint8 de,
        uint32 pcp_de_select, int qos_map_id,
        _bcm_tr3_extender_sd_tag_t *sd_tag,
        int vp, int *nh_index)
{
    int drop;
    bcm_extender_port_t  extender_port;

    bcm_extender_port_t_init(&extender_port);
    extender_port.flags = EXTENDER_PORT_INFO(unit, vp)->flags;
    extender_port.port = port;
    extender_port.match_vlan = match_vlan;
    extender_port.extended_port_vid = extended_port_vid;
    extender_port.pcp = pcp;
    extender_port.de = de;
    extender_port.pcp_de_select = pcp_de_select;
    extender_port.qos_map_id = qos_map_id;
    drop = (extender_port.flags & BCM_EXTENDER_PORT_DROP) ? 1 : 0;

    return _bcm_tr3_extender_nh_info_set(unit, &extender_port, sd_tag, vp, drop, nh_index);
}

#ifdef BCM_TRIDENT2_SUPPORT
/*
 * Function:
 *      _bcm_tr3_extender_l3mc_nh_info_set
 * Purpose:
 *      Configure next hop tables for L3MC.
 * Parameters:
 *      unit             - (IN) SOC unit number.
 *      nh_index      - (IN) Next hop index.
 *      flags                    - (IN) EXTENDER multicast flag.
 *      multicast_flags      - (IN) EXTENDER multicast flag.
 *      port             - (IN) Physical GPORT ID.
 *      extended_port_vid - (IN) extender port VID in Etag.
 *      pcp_de_select      - (IN) Selection of PCP and DE fields for egress ETAG.
 *      qos_map_id-       - (IN) Qos map id for egress etag mapping profile.
 *      pcp -                  - (IN) PCP field in  ETAG.
 *      de -                    - (IN) DE field in  ETAG.
 *      vp               - (IN) Virtual port number.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr3_extender_l3mc_nh_info_set(int unit, int nh_index, uint32 flags,
        uint32 multicast_flags, bcm_gport_t port, bcm_if_t intf,
        uint16 extended_port_vid, int qos_map_id,
        uint32 pcp_de_select, uint8 pcp, uint8 de, int vp)
{
    egr_l3_next_hop_entry_t egr_nh;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int id;
    int ing_nh_port;
    int ing_nh_module;
    int ing_nh_trunk;
    ing_l3_next_hop_entry_t ing_nh;
    initial_ing_l3_next_hop_entry_t initial_ing_nh;
    int i;
    int flag_set;
    int etag_dot1p_mapping_ptr;
    int drop;
    soc_mem_t mem = EGR_L3_NEXT_HOPm;

    uint32 flag_array[] = {
        BCM_L3_MULTICAST_L2_DEST_PRESERVE,
        BCM_L3_MULTICAST_L2_SRC_PRESERVE,
        BCM_L3_MULTICAST_L2_VLAN_PRESERVE,
        BCM_L3_MULTICAST_TTL_PRESERVE,
        BCM_L3_MULTICAST_DEST_PRESERVE,
        BCM_L3_MULTICAST_SRC_PRESERVE,
        BCM_L3_MULTICAST_VLAN_PRESERVE,
        BCM_L3_MULTICAST_L3_DROP,
        BCM_L3_MULTICAST_L2_DROP
    };
    soc_field_t field_array[] = {
        L3MC__L2_MC_DA_DISABLEf,
        L3MC__L2_MC_SA_DISABLEf,
        L3MC__L2_MC_VLAN_DISABLEf,
        L3MC__L3_MC_TTL_DISABLEf,
        L3MC__L3_MC_DA_DISABLEf,
        L3MC__L3_MC_SA_DISABLEf,
        L3MC__L3_MC_VLAN_DISABLEf,
        L3MC__L3_DROPf,
        L3MC__L2_DROPf
    };

    if ((nh_index > soc_mem_index_max(unit, mem)) ||
            (nh_index < soc_mem_index_min(unit, mem))) {
        return BCM_E_PARAM;
    }

    etag_dot1p_mapping_ptr = 0;
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        bcm_th_qos_egr_etag_id2profile(unit, qos_map_id,
                                       &etag_dot1p_mapping_ptr);
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)){
        bcm_td2_qos_egr_etag_id2profile(unit, qos_map_id,
                                        &etag_dot1p_mapping_ptr);
    }
#endif

    /*SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm,
            MEM_BLOCK_ANY, nh_index, &egr_nh));*/
    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));

    soc_mem_field32_set(unit, mem, &egr_nh, ENTRY_TYPEf, 7); /* L3MC view */
    soc_mem_field32_set(unit, mem, &egr_nh, L3MC__INTF_NUMf, intf);

    for (i = 0; i < COUNTOF(flag_array); i++) {
        if (multicast_flags & flag_array[i]) {
            flag_set = 1;
        } else {
            flag_set = 0;
        }
        if (soc_mem_field_valid(unit, mem, field_array[i])) {
            soc_mem_field32_set(unit, mem, &egr_nh, field_array[i], flag_set);
        } else {
            if (flag_set) {
                return BCM_E_PARAM;
            }
        }
    }
    /* Set up EGR_L3_NEXT_HOP entry */
    /*soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            L3MC__L3MC_USE_CONFIGURED_MACf, 0);*/

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_egr_l3_next_hop_next_ptr)) {
        soc_mem_field32_set(unit, mem, &egr_nh, L3MC__NEXT_PTR_TYPEf, 1);
    } else
#endif
    {
        soc_mem_field32_set(unit, mem, &egr_nh, L3MC__DVP_VALIDf, 1);
    }
    soc_mem_field32_set(unit, mem, &egr_nh,
            L3MC__VNTAG_DST_VIFf, extended_port_vid);
    /* PPD2 header */
    soc_mem_field32_set(unit, mem, &egr_nh, L3MC__HG_HDR_SELf, 1);

    /* For the transparent port, do not modify the ETAG. */
    if ((flags & BCM_EXTENDER_PORT_EGRESS_TRANSPARENT) == 0) {
        soc_mem_field32_set(unit, mem, &egr_nh, L3MC__VNTAG_ACTIONSf, 2);
    }

    if (pcp_de_select == BCM_EXTENDER_PCP_DE_SELECT_DEFAULT) {
        soc_mem_field32_set(unit, mem, &egr_nh, L3MC__ETAG_PCP_DE_SOURCEf, 2);
        soc_mem_field32_set(unit, mem, &egr_nh, L3MC__ETAG_PCPf, pcp);
        soc_mem_field32_set(unit, mem, &egr_nh, L3MC__ETAG_DEf, de);
    } else if (pcp_de_select == BCM_EXTENDER_PCP_DE_SELECT_PHB) {
        soc_mem_field32_set(unit, mem, &egr_nh, L3MC__ETAG_PCP_DE_SOURCEf, 3);
        soc_mem_field32_set(unit, mem, &egr_nh,
                L3MC__ETAG_DOT1P_MAPPING_PTRf, etag_dot1p_mapping_ptr);
    } else {
        return BCM_E_PARAM;
    }

    /* Indicate this egress object is on shared VP */
    if (SOC_IS_TRIDENT3X(unit)) {
        int fld_len = soc_mem_field_length(unit, mem, L3MC__RESERVED_1f);
        soc_mem_field32_set(unit, mem, &egr_nh, L3MC__NEXT_PTRf, vp);
        if (fld_len > 0) {
            soc_mem_field32_set(unit, mem, &egr_nh,
                                L3MC__RESERVED_1f, 1 << (fld_len - 1));
        }
    } else {
        soc_mem_field32_set(unit, mem, &egr_nh, L3MC__RSVD_DVPf, 1);
        soc_mem_field32_set(unit, mem, &egr_nh, L3MC__DVPf, vp);
    }

    /* Resolve gport */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, port, &mod_out, &port_out,
                               &trunk_id, &id));

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_mem_field_valid(unit, mem, L3MC__HG_MC_DST_PORT_NUMf)) {
        if ((!BCM_GPORT_IS_TRUNK(port)) &&
            (_bcm_xgs5_subport_coe_mod_port_local(unit, mod_out, port_out))) {
            soc_mem_field32_set(unit, mem, &egr_nh,
                                L3MC__HG_MC_DST_PORT_NUMf, port_out);
            soc_mem_field32_set(unit, mem, &egr_nh,
                                L3MC__HG_MC_DST_MODIDf, mod_out);
        }
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    /* Write EGR_L3_NEXT_HOP entry */
    SOC_IF_ERROR_RETURN(
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, nh_index, &egr_nh));

    ing_nh_port = -1;
    ing_nh_module = -1;
    ing_nh_trunk = -1;
    if (BCM_GPORT_IS_TRUNK(port)) {
        ing_nh_module = -1;
        ing_nh_port = -1;
        ing_nh_trunk = trunk_id;
    } else {
        ing_nh_module = mod_out;
        ing_nh_port = port_out;
        ing_nh_trunk = -1;
    }

    /* Write ING_L3_NEXT_HOP entry. It is not necessary and to be removed */
    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));
    if (ing_nh_trunk == -1) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                PORT_NUMf, ing_nh_port);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                MODULE_IDf, ing_nh_module);
    } else {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                Tf, 1);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                TGIDf, ing_nh_trunk);
    }

    drop = (flags & BCM_EXTENDER_PORT_DROP) ? 1 : 0;
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, drop);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_L3_NEXT_HOPm,
                MEM_BLOCK_ALL, nh_index, &ing_nh));

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
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
                MEM_BLOCK_ALL, nh_index, &initial_ing_nh));

    return BCM_E_NONE;
}
#endif /* BCM_TRIDENT2_SUPPORT */

/*
 * Function:
 *      _bcm_tr3_extender_match_key_assemble
 * Purpose:
 *      Constructs a vlan_xlate key for extender match
 *      according to extender port.
 * Parameters:
 *      unit -          (IN) SOC unit number.
 *      extender_port - (IN) Pointer to VLAN virtual port structure.
 *      vent -          (OUT) Pointer to a vlan_xlate entry.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr3_extender_match_key_assemble(
    int                 unit,
    bcm_extender_port_t *extender_port,
    void  *vent)
{
    int                   key_type;
    bcm_module_t          mod_out;
    bcm_port_t            port_out;
    bcm_trunk_t           trunk_out;
    int                   tmp_id;
    soc_mem_t             mem = VLAN_XLATEm;

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
        sal_memset(vent, 0, sizeof(vlan_xlate_1_double_entry_t));
    } else {
        sal_memset(vent, 0, sizeof(vlan_xlate_entry_t));
    }

    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    } else {
        soc_mem_field32_set(unit, mem, vent, VALIDf, 1);
    }

    if (extender_port->match_vlan == BCM_VLAN_NONE) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_vlan_xlate_key_type_value_get(
                unit, VLXLT_HASH_KEY_TYPE_VIF,&key_type));
    } else {
        if (!BCM_VLAN_VALID(extender_port->match_vlan)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(
            _bcm_esw_vlan_xlate_key_type_value_get(
                unit, VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_mem_field32_set(unit, mem, vent, VIF__VLANf,
                            extender_port->match_vlan);
    }
    soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type);
    if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type);
    }

    if (extender_port->extended_port_vid >=
            (1 << soc_mem_field_length(unit, mem, VIF__SRC_VIFf))) {
        return BCM_E_PARAM;
    }
    soc_mem_field32_set(unit, mem, vent, VIF__SRC_VIFf,
                        extender_port->extended_port_vid);

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_PE_SUPPORT)
    if (soc_mem_field_valid(unit, mem, SOURCE_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
    }
#endif /* BCM_TRIDENT2_SUPPORT || BCM_PE_SUPPORT */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, extender_port->port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(extender_port->port)) {
        soc_mem_field32_set(unit, mem, vent, VIF__Tf, 1);
        soc_mem_field32_set(unit, mem, vent, VIF__TGIDf, trunk_out);
    } else {
        soc_mem_field32_set(unit, mem, vent, VIF__MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, mem, vent, VIF__PORT_NUMf, port_out);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_extender_match_key_get
 * Purpose:
 *      get a vlan_xlate key for extender match according to extender port id.
 * Parameters:
 *      unit -             (IN) SOC unit number.
 *      extender_port_id - (IN) extender gport id.
 *      vent -             (OUT) Pointer to a vlan_xlate entry.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_tr3_extender_match_key_get(
    int                 unit,
    bcm_gport_t         extender_port_id,
    void  *vent)
{
    bcm_extender_port_t   extender_port;

    bcm_extender_port_t_init(&extender_port);
    extender_port.extender_port_id = extender_port_id;
    BCM_IF_ERROR_RETURN(
        bcm_tr3_extender_port_get(unit, &extender_port));
    return _bcm_tr3_extender_match_key_assemble(unit, &extender_port, vent);
}

/*
 * Function:
 *      _bcm_tr3_extender_match_add
 * Purpose:
 *      Add match criteria for Extender VP.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      extender_port - (IN) Pointer to VLAN virtual port structure. 
 *      vp - (IN) Virtual port number.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr3_extender_match_add(int unit, bcm_extender_port_t *extender_port,
        int vp)
{
    uint32                vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32                old_vent[SOC_MAX_MEM_FIELD_WORDS];
    int                   key_type;
    bcm_module_t          mod_out;
    bcm_port_t            port_out;
    bcm_trunk_t           trunk_out;
    int                   tmp_id;
    bcm_vlan_action_set_t action;
    uint32                profile_idx;
    int                   rv;
    bcm_trunk_t           trunk_id;
    int                   idx;
    int                   mod_local;
    int                   num_local_ports;
    bcm_port_t            local_ports[SOC_MAX_NUM_PORTS];
    int                   local_member_count;
    bcm_port_t            local_member_array[SOC_MAX_NUM_PORTS];
    int                   port_key_type_vif_vlan, port_key_type_vif;
    int                   port_key_type_a, port_key_type_b;
    int                   use_port_a, use_port_b;
    int                   vt_enable;
    soc_mem_t             mem = VLAN_XLATEm;

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }
    sal_memset(vent, 0, sizeof(vent));

    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    } else {
        soc_mem_field32_set(unit, mem, vent, VALIDf, 1);
    }

    if (extender_port->match_vlan == BCM_VLAN_NONE) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    } else {
        if (!BCM_VLAN_VALID(extender_port->match_vlan)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_mem_field32_set(unit, mem, vent, VIF__VLANf,
                extender_port->match_vlan);
    }
    soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type);
    if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type);
    }

    if (extender_port->extended_port_vid >=
            (1 << soc_mem_field_length(unit, mem, VIF__SRC_VIFf))) {
        return BCM_E_PARAM;
    }
    soc_mem_field32_set(unit, mem, vent, VIF__SRC_VIFf,
            extender_port->extended_port_vid);

#if defined( BCM_TRIDENT2_SUPPORT) || defined(BCM_PE_SUPPORT)
    if (soc_mem_field_valid(unit, mem, SOURCE_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
    }
#endif /* BCM_TRIDENT2_SUPPORT || BCM_PE_SUPPORT */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, extender_port->port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(extender_port->port)) {
        soc_mem_field32_set(unit, mem, vent, VIF__Tf, 1);
        soc_mem_field32_set(unit, mem, vent, VIF__TGIDf, trunk_out);
    } else {
        soc_mem_field32_set(unit, mem, vent, VIF__MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, mem, vent, VIF__PORT_NUMf, port_out);
    }

    /* SVP */
    soc_mem_field32_set(unit, mem, vent, VIF__MPLS_ACTIONf,
        ((extender_port->flags & BCM_EXTENDER_PORT_ID_ASSIGN_DISABLE) ? 0x0 : 0x1));
    soc_mem_field32_set(unit, mem, vent, VIF__SOURCE_VPf, vp);

    bcm_vlan_action_set_t_init(&action);
    if (BCM_VLAN_VALID(extender_port->match_vlan)) {
        soc_mem_field32_set(unit, mem, vent, VIF__NEW_OVIDf,
                            extender_port->match_vlan);
        action.dt_outer = bcmVlanActionCopy;
        action.dt_inner = bcmVlanActionDelete;
        action.ot_outer = bcmVlanActionReplace;
    } else {
        action.ot_outer_prio = bcmVlanActionReplace;
        action.ut_outer = bcmVlanActionAdd;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_add(unit, &action, &profile_idx));
    soc_mem_field32_set(unit, mem, vent, VIF__TAG_ACTION_PROFILE_PTRf,
            profile_idx);
#if defined( BCM_TRIDENT2_SUPPORT) || defined(BCM_PE_SUPPORT)
    if (soc_mem_field_valid(unit, mem, VIF__VLAN_ACTION_VALIDf)) {
        soc_mem_field32_set(unit, mem, vent, VIF__VLAN_ACTION_VALIDf, 1);
    }
#endif /* BCM_TRIDENT2_SUPPORT || BCM_PE_SUPPORT */

    rv = soc_mem_insert_return_old(unit, mem, MEM_BLOCK_ALL,
            vent, old_vent);
    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_mem_field32_get(unit, mem, old_vent,
                VIF__TAG_ACTION_PROFILE_PTRf);
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    BCM_IF_ERROR_RETURN(rv);

    num_local_ports = 0;
    if (BCM_GPORT_IS_TRUNK(extender_port->port)) {
        trunk_id = BCM_GPORT_TRUNK_GET(extender_port->port);
        rv = _bcm_trunk_id_validate(unit, trunk_id);
        if (BCM_FAILURE(rv)) {
            return BCM_E_PORT;
        }
        rv = _bcm_esw_trunk_local_members_get(unit, trunk_id,
                SOC_MAX_NUM_PORTS, local_member_array, &local_member_count);
        if (BCM_FAILURE(rv)) {
            return BCM_E_PORT;
        }
        for (idx = 0; idx < local_member_count; idx++) {
            local_ports[num_local_ports++] = local_member_array[idx];
        }
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, extender_port->port, &mod_out, &port_out,
                                    &trunk_id, &tmp_id)); 
        if ((trunk_id != -1) || (tmp_id != -1)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod_out, &mod_local));
        if (mod_local) {
            local_ports[num_local_ports++] = port_out;
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_pt_vtkey_type_value_get(unit,
                VLXLT_HASH_KEY_TYPE_VIF_VLAN, &port_key_type_vif_vlan));
    BCM_IF_ERROR_RETURN(_bcm_esw_pt_vtkey_type_value_get(unit,
                VLXLT_HASH_KEY_TYPE_VIF, &port_key_type_vif));
    for (idx = 0; idx < num_local_ports; idx++) {
        BCM_IF_ERROR_RETURN(_bcm_esw_port_config_get(unit, local_ports[idx],
                    _bcmPortVTKeyTypeFirst, &port_key_type_a));
        BCM_IF_ERROR_RETURN(_bcm_esw_port_config_get(unit, local_ports[idx],
                    _bcmPortVTKeyPortFirst, &use_port_a));
        BCM_IF_ERROR_RETURN(_bcm_esw_port_config_get(unit, local_ports[idx],
                    _bcmPortVTKeyTypeSecond, &port_key_type_b));
        BCM_IF_ERROR_RETURN(_bcm_esw_port_config_get(unit, local_ports[idx],
                    _bcmPortVTKeyPortSecond, &use_port_b));

        if (BCM_VLAN_VALID(extender_port->match_vlan)) {
            if ((port_key_type_a != port_key_type_vif_vlan) &&
                    (port_key_type_b != port_key_type_vif_vlan)) {
                BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                            local_ports[idx], _bcmPortVTKeyTypeFirst,
                            port_key_type_vif_vlan));
                BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                            local_ports[idx], _bcmPortVTKeyPortFirst, 1));
                if (port_key_type_a == port_key_type_vif) {
                    /* The first slot contained VIF key type, which
                     * was just overwritten by VIF_VLAN key type. Hence,
                     * the VIF key type needs to be moved to second slot.
                     */
                    BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                                local_ports[idx], _bcmPortVTKeyTypeSecond,
                                port_key_type_a));
                    BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                                local_ports[idx], _bcmPortVTKeyPortSecond,
                                use_port_a));
                }
            }
        } else {
            if ((port_key_type_a != port_key_type_vif) &&
                    (port_key_type_b != port_key_type_vif)) {
                if (port_key_type_a != port_key_type_vif_vlan) {
                    BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                                local_ports[idx], _bcmPortVTKeyTypeFirst,
                                port_key_type_vif));
                    BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                                local_ports[idx], _bcmPortVTKeyPortFirst, 1));
                } else {
                    BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                                local_ports[idx], _bcmPortVTKeyTypeSecond,
                                port_key_type_vif));
                    BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                                local_ports[idx], _bcmPortVTKeyPortSecond, 1));
                }
            }
        }

        /* Enable ingress VLAN translation */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_get(unit, local_ports[idx],
                                      _bcmPortVlanTranslate, &vt_enable));
        if (!vt_enable) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_port_config_set(unit, local_ports[idx],
                                          _bcmPortVlanTranslate, 1));
        }

        if (BCM_VLAN_VALID(extender_port->match_vlan)) {
            /* Enable egress VLAN translation */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
                if (soc_feature(unit, soc_feature_egr_all_profile)) {
                    BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_field_set(unit,
                        local_ports[idx], EGR_VLAN_CONTROL_1m, VT_ENABLEf, 1));
                } else {
                    SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit,
                        EGR_VLAN_CONTROL_1m, local_ports[idx], VT_ENABLEf, 1));
                }
            } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
            {
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                            EGR_VLAN_CONTROL_1r, local_ports[idx], VT_ENABLEf, 1));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_extender_match_delete
 * Purpose:
 *      Delete match criteria for Extender VP.
 * Parameters:
 *      unit   - (IN) SOC unit number.
 *      vp     - (IN) Virtual port number.
 *      old_vp - (OUT) Pointer to the old virtual port number of the entry.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr3_extender_match_delete(int unit, int vp, int *old_vp)
{
    uint32             vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32             old_vent[SOC_MAX_MEM_FIELD_WORDS];
    int                key_type;
    bcm_module_t       mod_out;
    bcm_port_t         port_out;
    bcm_trunk_t        trunk_out;
    int                tmp_id;
    uint32             profile_idx;
    int                rv;
    soc_mem_t          mem = VLAN_XLATEm;

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }
    sal_memset(vent, 0, sizeof(vent));

    if (BCM_VLAN_VALID(EXTENDER_PORT_INFO(unit, vp)->match_vlan)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_mem_field32_set(unit, mem, vent, VIF__VLANf,
                EXTENDER_PORT_INFO(unit, vp)->match_vlan);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    }
    soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type);
    if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type);
    }

    soc_mem_field32_set(unit, mem, vent, VIF__SRC_VIFf,
            EXTENDER_PORT_INFO(unit, vp)->extended_port_vid);

#if defined( BCM_TRIDENT2_SUPPORT) || defined(BCM_PE_SUPPORT)
    if (soc_mem_field_valid(unit, mem, SOURCE_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
    }
#endif /* BCM_TRIDENT2_SUPPORT || BCM_PE_SUPPORT */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, EXTENDER_PORT_INFO(unit, vp)->port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(EXTENDER_PORT_INFO(unit, vp)->port)) {
        soc_mem_field32_set(unit, mem, vent, VIF__Tf, 1);
        soc_mem_field32_set(unit, mem, vent, VIF__TGIDf, trunk_out);
    } else {
        soc_mem_field32_set(unit, mem, vent, VIF__MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, mem, vent, VIF__PORT_NUMf, port_out);
    }

    rv = soc_mem_delete_return_old(unit, mem, MEM_BLOCK_ALL,
                                   vent, old_vent);
    if (rv == SOC_E_NONE) {
        uint32 is_valid = 0;
        if (soc_feature(unit, soc_feature_base_valid)) {
            is_valid =
                soc_mem_field32_get(unit, mem, old_vent, BASE_VALID_0f) == 3 &&
                soc_mem_field32_get(unit, mem, old_vent, BASE_VALID_1f) == 7;
        } else {
            is_valid = soc_mem_field32_get(unit, mem, old_vent, VALIDf);
        }
        if (is_valid) {
            if (old_vp != NULL) {
                *old_vp = soc_mem_field32_get(unit, mem, old_vent,
                                              VIF__SOURCE_VPf);
            }
            profile_idx = soc_mem_field32_get(unit, mem, old_vent,
                                              VIF__TAG_ACTION_PROFILE_PTRf);
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        }
    }

    return rv;
}


/*
 * Function:
 *      _bcm_tr3_extender_port_match_add
 * Purpose:
 *      Add a match to an existing port.
 * Parameters:
 *      unit -             (IN) SOC unit number. 
 *      port -             (IN) port of match criteria.
 *      extended_port_vid -(IN) extended_port_vid of match criteria.
 *      vlan -             (IN) vlan of match criteria.
 *      vp -               (IN) the vp of match destination.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_tr3_extender_port_match_add(int unit, bcm_gport_t port,
                                 uint16 extended_port_vid,
                                  bcm_vlan_t vlan, int vp)
{
    bcm_extender_port_t  extender_port;
    
    bcm_extender_port_t_init(&extender_port);
    extender_port.port = port;
    extender_port.match_vlan = vlan;
    extender_port.extended_port_vid = extended_port_vid;

    return _bcm_tr3_extender_match_add(unit, &extender_port, vp);    
}

/*
 * Function:
 *      _bcm_tr3_extender_port_match_delete
 * Purpose:
 *      delete a match from an existing vp.
 * Parameters:
 *      unit -             (IN) SOC unit number. 
 *      port -             (IN) port of match criteria.
 *      extender_port_vid -(IN) extended_port_vid of match criteria.
 *      vlan -             (IN) vlan of match criteria.
 *      vp -               (IN) the vp of match destination.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_tr3_extender_port_match_delete(int unit, bcm_gport_t port,
                                    uint16 extended_port_vid,
                                    bcm_vlan_t vlan, int vp)
{
    uint32             vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32             old_vent[SOC_MAX_MEM_FIELD_WORDS];
    int                key_type;
    bcm_module_t       mod_out;
    bcm_port_t         port_out;
    bcm_trunk_t        trunk_out;
    int                tmp_id;
    uint32             profile_idx;
    int                rv;
    int                old_vp;
    int                entry_index;
    soc_mem_t          mem = VLAN_XLATEm;

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }
    sal_memset(vent, 0, sizeof(vent));

    if (BCM_VLAN_VALID(vlan)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_vlan_xlate_key_type_value_get(unit,
                                                   VLXLT_HASH_KEY_TYPE_VIF_VLAN,
                                                   &key_type));
        soc_mem_field32_set(unit, mem, vent, VIF__VLANf, vlan);
    } else {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_vlan_xlate_key_type_value_get(unit,
                                                   VLXLT_HASH_KEY_TYPE_VIF,
                                                   &key_type));
    }
    soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type);
    if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type);
    }

    soc_mem_field32_set(unit, mem, vent, VIF__SRC_VIFf, extended_port_vid);

#if defined( BCM_TRIDENT2_SUPPORT) || defined(BCM_PE_SUPPORT)
    if (soc_mem_field_valid(unit, mem, SOURCE_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
    }
#endif /* BCM_TRIDENT2_SUPPORT || BCM_PE_SUPPORT */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out,
                                &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(port)) {
        soc_mem_field32_set(unit, mem, vent, VIF__Tf, 1);
        soc_mem_field32_set(unit, mem, vent, VIF__TGIDf, trunk_out);
    } else {
        soc_mem_field32_set(unit, mem, vent, VIF__MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, mem, vent, VIF__PORT_NUMf, port_out);
    }

    soc_mem_lock(unit, mem);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &entry_index,
                        vent, old_vent, 0);
    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    old_vp = soc_mem_field32_get(unit, mem, old_vent, VIF__SOURCE_VPf);
    if (vp != old_vp) {
        soc_mem_unlock(unit, mem);
        return BCM_E_NOT_FOUND;
    }

    rv = soc_mem_delete_return_old(unit, mem,
                                   MEM_BLOCK_ALL, vent, old_vent);
    soc_mem_unlock(unit, mem);
    if (rv == SOC_E_NONE) {
        uint32 is_valid = 0;
        if (soc_feature(unit, soc_feature_base_valid)) {
            is_valid =
                soc_mem_field32_get(unit, mem, old_vent, BASE_VALID_0f) == 3 &&
                soc_mem_field32_get(unit, mem, old_vent, BASE_VALID_1f) == 7;
        } else {
            is_valid = soc_mem_field32_get(unit, mem, old_vent, VALIDf);
        }
        if (is_valid) {
            profile_idx = soc_mem_field32_get(unit, mem, old_vent,
                                              VIF__TAG_ACTION_PROFILE_PTRf);
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr3_extender_port_add
 * Purpose:
 *      Create an Extender port on Controlling Bridge.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      extender_port - (IN/OUT) Extender port information
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_tr3_extender_port_add(int unit,
                         bcm_extender_port_t *extender_port)
{
    int mode;
    int vp, old_vp = 0, matched_vp;
    int rv = BCM_E_NONE;
    int num_vp;
    int nh_index = 0;
    bcm_if_t intf;
    ing_dvp_table_entry_t dvp_entry;
    source_vp_entry_t svp_entry;
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;
    int drop;
    _bcm_vp_info_t vp_info;

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeExtender;

    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_egress_mode_get(unit, &mode));
    if (!mode) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 egress mode must be set first\n")));
        return BCM_E_DISABLED;
    }

    if ((!soc_feature(unit, soc_feature_port_extender_vp_sharing)) &&
        (extender_port->flags & BCM_EXTENDER_PORT_MATCH_NONE)) {
        return BCM_E_UNAVAIL;
    }

    /* Check for invalid flag combinations */
    if (extender_port->flags & BCM_EXTENDER_PORT_MATCH_NONE) {
        if ((extender_port->flags & BCM_EXTENDER_PORT_REPLACE) ||
            (extender_port->flags & BCM_EXTENDER_PORT_MULTICAST)) {
            return BCM_E_PARAM;
        }
    }

    if (!(extender_port->flags & BCM_EXTENDER_PORT_REPLACE)) {
        /* Create new Extender VP */

        if (extender_port->flags & BCM_EXTENDER_PORT_WITH_ID) {
            if (!BCM_GPORT_IS_EXTENDER_PORT(extender_port->extender_port_id)) {
                return BCM_E_PARAM;
            }
            vp = BCM_GPORT_EXTENDER_PORT_ID_GET(extender_port->extender_port_id);
            if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
                return BCM_E_PARAM;
            }
            if (_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
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

        if (extender_port->flags & BCM_EXTENDER_PORT_MATCH_NONE) {
            /* For a EXTENDER port without any match criteria, an invalid
                      * next hop index will be written to the ING_DVP_TABLE.
                      */
            nh_index = EXTENDER_INFO(unit)->invalid_next_hop_index;
        } else {
            /* Configure next hop tables */
            drop = (extender_port->flags & BCM_EXTENDER_PORT_DROP) ? 1 : 0;
            rv = _bcm_tr3_extender_nh_info_set(unit, extender_port, NULL, vp, drop,
                                              &nh_index);
            if (rv < 0) {
                goto cleanup;
            }
        }

        /* Configure DVP table */
        intf = nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        rv = _bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigSet, vp,
                                    ING_DVP_CONFIG_INVALID_VP_TYPE, intf,
                                    ING_DVP_CONFIG_INVALID_PORT_TYPE);
        if (rv < 0) {
            goto cleanup;
        }

        /* Configure SVP table */
        sal_memset(&svp_entry, 0, sizeof(source_vp_entry_t));
        soc_SOURCE_VPm_field32_set(unit, &svp_entry, ENTRY_TYPEf, 3);

        /* Set the CML */
        rv = _bcm_vp_default_cml_mode_get(unit, &cml_default_enable,
                &cml_default_new, &cml_default_move);
        if (rv < 0) {
            goto cleanup;
        }
        if (cml_default_enable) {
            /* Set the CML to default values */
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_NEWf,
                    cml_default_new);
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_MOVEf,
                    cml_default_move);
        } else {
            /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_NEWf, 0x8);
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_MOVEf, 0x8);
        }

        /* Set the TPID_SOURCE based on SGLP */
        if (soc_mem_field_valid(unit, SOURCE_VPm, TPID_SOURCEf)) {
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, TPID_SOURCEf, 2);
        }

        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry);
        if (rv < 0) {
            goto cleanup;
        }

        /* Configure SOURCE_VP_2 table */
#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_mem_field_valid(unit, SOURCE_VP_2m, PARSE_USING_SGLP_TPIDf)) {
            source_vp_2_entry_t svp_2_entry;

            sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
            soc_SOURCE_VP_2m_field32_set(unit, &svp_2_entry, PARSE_USING_SGLP_TPIDf, 1);
            rv = WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, vp, &svp_2_entry);
            if (rv < 0) {
                goto cleanup;
            }
        }
#endif /* BCM_TRIDENT2_SUPPORT */

        if (!(extender_port->flags & BCM_EXTENDER_PORT_MULTICAST)
            && (!(extender_port->flags & BCM_EXTENDER_PORT_MATCH_NONE))) {
            /* Configure ingress VLAN translation table for unicast VPs */
           rv = _bcm_tr3_extender_match_add(unit, extender_port, vp);
           if (rv < 0) {
               goto cleanup;
           }
       }

        /* Configure the failover information */
        if (BCM_SUCCESS(_bcm_esw_failover_extender_check(unit, 
                                                         extender_port))) {
            rv = _bcm_tr3_extender_failover_add(unit, extender_port, vp);
            if (rv < 0) {
                goto cleanup;
            }
        }

        if (!(extender_port->flags & BCM_EXTENDER_PORT_MATCH_NONE)) {

            /* Increment port's VP count */
            rv = _bcm_tr3_extender_port_cnt_update(unit, extender_port->port, vp, TRUE);
            if (rv < 0) {
                goto cleanup;
            }
        }

    } else { /* Replace properties of existing Extender VP */

        if (!(extender_port->flags & BCM_EXTENDER_PORT_WITH_ID)) {
            return BCM_E_PARAM;
        }
        if (!BCM_GPORT_IS_EXTENDER_PORT(extender_port->extender_port_id)) {
            return BCM_E_PARAM;
        }
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(extender_port->extender_port_id);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
            return BCM_E_PARAM;
        }

        /* For existing Extender vp, NH entry already exists */
        BCM_IF_ERROR_RETURN
            (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
        nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
                NEXT_HOP_INDEXf);

        /* Update existing next hop entries */
        drop = (extender_port->flags & BCM_EXTENDER_PORT_DROP) ? 1 : 0;
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_extender_nh_info_set(unit, extender_port, NULL, vp, drop,
                                           &nh_index));

        /* Delete old ingress VLAN translation entry,
               * install new ingress VLAN translation entry
               */
        if (!(EXTENDER_PORT_INFO(unit, vp)->flags & BCM_EXTENDER_PORT_MULTICAST)
            && (!(EXTENDER_PORT_INFO(unit, vp)->flags & BCM_EXTENDER_PORT_MATCH_NONE))) {
            BCM_IF_ERROR_RETURN(
                _bcm_tr3_extender_match_delete(unit, vp, &old_vp));
        }
        if (!(extender_port->flags & BCM_EXTENDER_PORT_MULTICAST)) {
#ifdef BCM_TRIDENT2_SUPPORT
            /* The extender port id may be one of members of a vplag. */
            if (_bcm_vp_used_get(unit, old_vp, _bcmVpTypeVpLag)) {
                matched_vp = old_vp;
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                matched_vp = vp;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_tr3_extender_match_add(unit, extender_port, matched_vp));
        }

        /* Configure the failover information */
        if (BCM_SUCCESS(_bcm_esw_failover_extender_check(unit, 
                                                         extender_port))) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr3_extender_failover_add(unit, extender_port, vp));
        } else {
            /* remove the failover configuration if found*/
            bcm_gport_t gport = extender_port->extender_port_id;
            if (BCM_SUCCESS(_bcm_tr3_extender_failover_get(unit, gport, 
                                                           NULL, NULL, NULL))) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr3_extender_failover_delete(unit, gport));
            }
        }

        /* Decrement old port's VP count, increment new port's VP count */
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_extender_port_cnt_update(unit,
                EXTENDER_PORT_INFO(unit, vp)->port, vp, FALSE));
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_extender_port_cnt_update(unit,
                EXTENDER_PORT_INFO(unit, vp)->port, vp, TRUE));
    }

    /* Set Extender VP software state */
    EXTENDER_PORT_INFO(unit, vp)->flags = extender_port->flags;
    EXTENDER_PORT_INFO(unit, vp)->port = extender_port->port;
    EXTENDER_PORT_INFO(unit, vp)->extended_port_vid =
        extender_port->extended_port_vid;
    EXTENDER_PORT_INFO(unit, vp)->pcp_de_select = extender_port->pcp_de_select;
    EXTENDER_PORT_INFO(unit, vp)->qos_map_id = extender_port->qos_map_id;
    EXTENDER_PORT_INFO(unit, vp)->pcp = extender_port->pcp;
    EXTENDER_PORT_INFO(unit, vp)->de = extender_port->de;
    EXTENDER_PORT_INFO(unit, vp)->match_vlan = extender_port->match_vlan;

    BCM_GPORT_EXTENDER_PORT_ID_SET(extender_port->extender_port_id, vp);

    return rv;

cleanup:
    if (!(extender_port->flags & BCM_EXTENDER_PORT_REPLACE)) {
        (void) _bcm_vp_free(unit, _bcmVpTypeExtender, 1, vp);

        if (!(extender_port->flags & BCM_EXTENDER_PORT_MATCH_NONE)) {
            _bcm_tr3_extender_nh_info_delete(unit, nh_index);
        }

        (void)_bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigClear, vp, 
                                     ING_DVP_CONFIG_INVALID_VP_TYPE, 
                                     ING_DVP_CONFIG_INVALID_INTF_ID, 
                                     ING_DVP_CONFIG_INVALID_PORT_TYPE);

        sal_memset(&svp_entry, 0, sizeof(source_vp_entry_t));
        (void)WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry);

#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_mem_is_valid(unit, SOURCE_VP_2m)) {
            source_vp_2_entry_t svp_2_entry;

            sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
            (void)WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, vp, &svp_2_entry);
        }
#endif /* BCM_TRIDENT2_SUPPORT */

        if (!(extender_port->flags & BCM_EXTENDER_PORT_MULTICAST)
            && (!(extender_port->flags & BCM_EXTENDER_PORT_MATCH_NONE))) {
            (void) _bcm_tr3_extender_match_delete(unit, vp, NULL);
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr3_extender_port_delete
 * Purpose:
 *      Destroy a Extender virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) Extender GPORT ID.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_tr3_extender_port_delete(int unit, bcm_gport_t gport)
{
    int vp;
    source_vp_entry_t svp_entry;
    int nh_index;
    ing_dvp_table_entry_t dvp_entry;

    if (!BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
        return BCM_E_NOT_FOUND;
    }
    if (EXTENDER_PORT_INFO(unit, vp)->flags & BCM_EXTENDER_PORT_MATCH_NONE) {
        if (EXTENDER_PORT_INFO(unit, vp)->egress_list != NULL) {
            /* There are still EXTENDER egress objects attached to this
             * EXTENDER port.
             */
            return BCM_E_BUSY;
        }
    }
    /* Delete failover information if found */
    if (BCM_SUCCESS(_bcm_tr3_extender_failover_get(unit, gport, 
                                                   NULL, NULL, NULL))) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_extender_failover_delete(unit, gport));
    }

    /* Delete ingress VLAN translation entry */
    if (!(EXTENDER_PORT_INFO(unit, vp)->flags & BCM_EXTENDER_PORT_MULTICAST)
        && (!(EXTENDER_PORT_INFO(unit, vp)->flags & BCM_EXTENDER_PORT_MATCH_NONE))
    ) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_extender_match_delete(unit, vp, NULL));
    }

    /* Clear SVP entry */
    sal_memset(&svp_entry, 0, sizeof(source_vp_entry_t));
    BCM_IF_ERROR_RETURN
        (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));

    /* Clear SOURCE_VP_2 entry */
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_mem_is_valid(unit, SOURCE_VP_2m)) {
        source_vp_2_entry_t svp_2_entry;

        sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
        BCM_IF_ERROR_RETURN
            (WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, vp, &svp_2_entry));
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    /* Clear DVP entry */
    BCM_IF_ERROR_RETURN
        (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry, NEXT_HOP_INDEXf);
    BCM_IF_ERROR_RETURN(
        _bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigClear, vp, 
                               ING_DVP_CONFIG_INVALID_VP_TYPE, 
                               ING_DVP_CONFIG_INVALID_INTF_ID, 
                               ING_DVP_CONFIG_INVALID_PORT_TYPE));
    if (!(EXTENDER_PORT_INFO(unit, vp)->flags & BCM_EXTENDER_PORT_MATCH_NONE)) {
        /* Clear next hop entries and free next hop index */
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_extender_nh_info_delete(unit, nh_index));

        /* Decrement port's VP count */
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_extender_port_cnt_update(unit,
                                               EXTENDER_PORT_INFO(unit, vp)->port,
                                               vp, FALSE));
    }
    /* Free VP */
    BCM_IF_ERROR_RETURN
        (_bcm_vp_free(unit, _bcmVpTypeExtender, 1, vp));

    /* Clear Extender VP software state */
    sal_memset(EXTENDER_PORT_INFO(unit, vp), 0,
            sizeof(_bcm_tr3_extender_port_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_extender_port_delete_all
 * Purpose:
 *      Destroy all Extender virtual ports.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_tr3_extender_port_delete_all(int unit)
{
    int i;
    bcm_gport_t extender_port_id;

    for (i = soc_mem_index_min(unit, SOURCE_VPm);
         i <= soc_mem_index_max(unit, SOURCE_VPm);
         i++) {
        if (_bcm_vp_used_get(unit, i, _bcmVpTypeExtender)) {
            BCM_GPORT_EXTENDER_PORT_ID_SET(extender_port_id, i);
            BCM_IF_ERROR_RETURN
                (bcm_tr3_extender_port_delete(unit, extender_port_id));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_extender_port_get
 * Purpose:
 *      Get Extender virtual port info.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      extender_port - (IN/OUT) Pointer to Extender virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_tr3_extender_port_get(int unit, bcm_extender_port_t *extender_port)
{
    int rv, vp;

    if (!BCM_GPORT_IS_EXTENDER_PORT(extender_port->extender_port_id)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_EXTENDER_PORT_ID_GET(extender_port->extender_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
        return BCM_E_NOT_FOUND;
    }

    bcm_extender_port_t_init(extender_port);
    extender_port->flags = EXTENDER_PORT_INFO(unit, vp)->flags;
    BCM_GPORT_EXTENDER_PORT_ID_SET(extender_port->extender_port_id, vp);
    extender_port->port = EXTENDER_PORT_INFO(unit, vp)->port;
    extender_port->extended_port_vid = EXTENDER_PORT_INFO(unit, vp)->
                                       extended_port_vid;
    extender_port->pcp_de_select = EXTENDER_PORT_INFO(unit, vp)->pcp_de_select;
    extender_port->qos_map_id = EXTENDER_PORT_INFO(unit, vp)->qos_map_id;
    extender_port->pcp = EXTENDER_PORT_INFO(unit, vp)->pcp;
    extender_port->de = EXTENDER_PORT_INFO(unit, vp)->de;
    extender_port->match_vlan = EXTENDER_PORT_INFO(unit, vp)->match_vlan;

    rv = _bcm_tr3_extender_failover_get(unit, extender_port->extender_port_id,
                                        &extender_port->failover_port_id,
                                        &extender_port->failover_id,
                                        &extender_port->failover_mc_group);
    if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND && rv != BCM_E_UNAVAIL) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_extender_port_traverse
 * Purpose:
 *      Traverse all Extender ports and call supplied callback routine.
 * Parameters:
 *      unit      - (IN) Device Number
 *      cb        - (IN) User-provided callback
 *      user_data - (IN/OUT) Cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_extender_port_traverse(int unit, bcm_extender_port_traverse_cb cb, 
                                  void *user_data)
{
    int i;
    bcm_extender_port_t extender_port;

    for (i = soc_mem_index_min(unit, SOURCE_VPm);
         i <= soc_mem_index_max(unit, SOURCE_VPm);
         i++) {
        if (_bcm_vp_used_get(unit, i, _bcmVpTypeExtender)) {
            bcm_extender_port_t_init(&extender_port);
            BCM_GPORT_EXTENDER_PORT_ID_SET(extender_port.extender_port_id, i);
            BCM_IF_ERROR_RETURN(bcm_tr3_extender_port_get(unit, &extender_port));
            BCM_IF_ERROR_RETURN(cb(unit, &extender_port, user_data));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_extender_port_resolve
 * Purpose:
 *      Get the modid, port, trunk values for a Extender virtual port
 * Parameters:
 *      unit     - (IN) BCM device number
 *      gport    - (IN) Global port identifier
 *      modid    - (OUT) Module ID
 *      port     - (OUT) Port number
 *      trunk_id - (OUT) Trunk ID
 *      id       - (OUT) Virtual port ID
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_extender_port_resolve(int unit, bcm_gport_t extender_port_id,
                          bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id)

{
    int rv = BCM_E_NONE, nh_index, vp;
    ing_l3_next_hop_entry_t ing_nh;
    ing_dvp_table_entry_t dvp;
    uint32 dv, dt = SOC_MEM_FIF_DEST_INVALID;

    if (!BCM_GPORT_IS_EXTENDER_PORT(extender_port_id)) {
        return (BCM_E_BADID);
    }

    vp = BCM_GPORT_EXTENDER_PORT_ID_GET(extender_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
        return BCM_E_NOT_FOUND;
    }
    BCM_IF_ERROR_RETURN(READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                              NEXT_HOP_INDEXf);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      nh_index, &ing_nh));

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, ENTRY_TYPEf) != 0x2) {
        /* Entry type is not L2 DVP */
        return BCM_E_NOT_FOUND;
    }

    if (soc_feature(unit, soc_feature_generic_dest)) {
        dv = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm, &ing_nh,
                                      DESTINATIONf, &dt);
        if (dt == SOC_MEM_FIF_DEST_LAG) {
            *trunk_id = dv;
        } else {
            *port = dv & SOC_MEM_FIF_DGPP_PORT_MASK;
            *modid = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                        SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
        }
    } else {
        if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
            *trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
        } else {
            *modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
            *port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
        }
    }
    *id = vp;
    return rv;
}

/*
 * Purpose:
 *	Add a downstream forwarding entry
 * Parameters:
 *	unit - (IN) Device Number
 *	forward_entry - (IN) Forwarding entry
 */
int
bcm_tr3_extender_forward_add(int unit, bcm_extender_forward_t *forward_entry)
{
    int rv = BCM_E_NONE;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t tgid_out;
    int id_out;
    l2_entry_1_entry_t l2_entry;
    int lower_th, higher_th;

    if (forward_entry->name_space > 0xfff) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit,
                                   bcmSwitchExtenderMulticastLowerThreshold,
                                   &lower_th));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit,
                                   bcmSwitchExtenderMulticastHigherThreshold, 
                                   &higher_th));

    if ((forward_entry->extended_port_vid >=
         (1 << soc_mem_field_length(unit, L2_ENTRY_1m, PE_VID__ETAG_VIDf))) ||
        (forward_entry->class_id >=
         (1 << soc_mem_field_length(unit, L2_ENTRY_1m, PE_VID__CLASS_IDf)))) {
        return BCM_E_PARAM;
    }

    if (!(forward_entry->flags & BCM_EXTENDER_FORWARD_MULTICAST)) {
         if (forward_entry->extended_port_vid >=
            (1 << soc_mem_field_length(unit, L2_ENTRY_1m,
                                       PE_VID__ETAG_VIDf))) {
            return BCM_E_PARAM;
        }

        if ((forward_entry->extended_port_vid >= lower_th) &&
            (forward_entry->extended_port_vid <= higher_th)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit,
                    forward_entry->dest_port,
                    &mod_out, &port_out, &tgid_out, &id_out));
        if (-1 != id_out) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2_entry, 0, sizeof(l2_entry));
        soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, VALIDf, 1);
        soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, KEY_TYPEf,
                SOC_MEM_KEY_L2_ENTRY_1_PE_VID_PE_VID);
        soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__NAMESPACEf,
                forward_entry->name_space);
        soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__ETAG_VIDf,
                forward_entry->extended_port_vid);
        soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__CLASS_IDf,
                forward_entry->class_id);
        if (-1 != tgid_out) {
            BCM_IF_ERROR_RETURN(_bcm_trunk_id_validate(unit, tgid_out));
            soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__DEST_TYPEf,
                    1);
            soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__TGIDf,
                    tgid_out);
        } else {
            soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__MODULE_IDf,
                    mod_out);
            soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__PORT_NUMf,
                    port_out);
        }
        soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, STATIC_BITf, 1);

    } else { /* Multicast forward entry */
        if ((forward_entry->extended_port_vid < lower_th) ||
               (forward_entry->extended_port_vid > higher_th)) {
            return BCM_E_PARAM;
        }
        if (!_BCM_MULTICAST_IS_L2(forward_entry->dest_multicast)) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2_entry, 0, sizeof(l2_entry));
        soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, VALIDf, 1);
        soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, KEY_TYPEf,
                SOC_MEM_KEY_L2_ENTRY_1_PE_VID_PE_VID);
        soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__NAMESPACEf,
                forward_entry->name_space);
        soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__ETAG_VIDf,
                forward_entry->extended_port_vid);
        soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__CLASS_IDf,
                forward_entry->class_id);
        soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__L2MC_PTRf,
                _BCM_MULTICAST_ID_GET(forward_entry->dest_multicast));
        soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, STATIC_BITf, 1);
    }

    if (forward_entry->flags & BCM_EXTENDER_FORWARD_COPY_TO_CPU) {
        if (SOC_MEM_FIELD_VALID(unit, L2_ENTRY_1m, PE_VID__DST_CPUf)) {
            soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__DST_CPUf, 1);
        }
    }

    soc_mem_lock(unit, L2_ENTRY_1m);

    if (!(forward_entry->flags & BCM_EXTENDER_FORWARD_REPLACE)) {
        l2_entry_1_entry_t  l2x_lookup;
        int                 l2_index;

        /* See if the entry already exists */
        rv = soc_mem_generic_lookup(unit, L2_ENTRY_1m, MEM_BLOCK_ANY, 0,
                                    &l2_entry, &l2x_lookup, &l2_index);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            soc_mem_unlock(unit, L2_ENTRY_1m);
            return rv;
        }
        if (BCM_SUCCESS(rv)) {
            soc_mem_unlock(unit, L2_ENTRY_1m);
            return BCM_E_EXISTS;
        }

        rv = soc_mem_insert(unit, L2_ENTRY_1m, MEM_BLOCK_ALL, &l2_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2_ENTRY_1m);
            return rv;
        }
    } else { /* Replace existing entry */
        rv = soc_mem_delete(unit, L2_ENTRY_1m, MEM_BLOCK_ALL, &l2_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2_ENTRY_1m);
            return rv;
        }
        rv = soc_mem_insert(unit, L2_ENTRY_1m, MEM_BLOCK_ALL, &l2_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2_ENTRY_1m);
            return rv;
        }
    }

    soc_mem_unlock(unit, L2_ENTRY_1m);

    return rv;
}

/*
 * Purpose:
 *	Delete a downstream forwarding entry
 * Parameters:
 *      unit - (IN) Device Number
 *      forward_entry - (IN) Forwarding entry
 */
int
bcm_tr3_extender_forward_delete(int unit,
        bcm_extender_forward_t *forward_entry)
{
    int rv = BCM_E_NONE;
    l2_entry_1_entry_t l2_entry;
    int lower_th, higher_th;

    if (forward_entry->name_space > 0xfff) {
        return BCM_E_PARAM;
    }
    
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit,
                                   bcmSwitchExtenderMulticastLowerThreshold,
                                   &lower_th));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit,
                                   bcmSwitchExtenderMulticastHigherThreshold, 
                                   &higher_th));

    if (forward_entry->extended_port_vid >=
         (1 << soc_mem_field_length(unit, L2_ENTRY_1m, PE_VID__ETAG_VIDf))) {
        return BCM_E_PARAM;
    }
    
    if (!(forward_entry->flags & BCM_EXTENDER_FORWARD_MULTICAST)) {
        if ((forward_entry->extended_port_vid >= lower_th) &&
            (forward_entry->extended_port_vid <= higher_th)) {
            return BCM_E_PARAM;
        }
    } else { /* Multicast forward entry */
        if ((forward_entry->extended_port_vid < lower_th) ||
               (forward_entry->extended_port_vid > higher_th)) {
            return BCM_E_PARAM;
        }
    }

    sal_memset(&l2_entry, 0, sizeof(l2_entry));
    soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, KEY_TYPEf,
            SOC_MEM_KEY_L2_ENTRY_1_PE_VID_PE_VID);
    soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__NAMESPACEf,
            forward_entry->name_space);
    soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__ETAG_VIDf,
            forward_entry->extended_port_vid);
    soc_mem_lock(unit, L2_ENTRY_1m);
    rv = soc_mem_delete(unit, L2_ENTRY_1m, MEM_BLOCK_ALL, &l2_entry);
    soc_mem_unlock(unit, L2_ENTRY_1m);

    return rv;
}

/*
 * Purpose:
 *	Delete all downstream forwarding entries
 * Parameters:
 *	unit - Device Number
 */
int
bcm_tr3_extender_forward_delete_all(int unit)
{
    int rv = BCM_E_NONE;
    int seconds, enabled;
    l2_entry_1_entry_t match_mask;
    l2_entry_1_entry_t match_data;
    l2_bulk_entry_t l2_bulk;
    int match_mask_index, match_data_index;
    uint32 rval;
    int field_len;

    SOC_IF_ERROR_RETURN
        (SOC_FUNCTIONS(unit)->soc_age_timer_get(unit, &seconds, &enabled));
    if (enabled) {
        SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_stop(unit));
    }

    soc_mem_lock(unit, L2_ENTRY_1m);

    /* Set match mask and data */
    sal_memset(&match_mask, 0, sizeof(match_mask));
    sal_memset(&match_data, 0, sizeof(match_data));

    soc_mem_field32_set(unit, L2_ENTRY_1m, &match_mask, VALIDf, 1);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &match_data, VALIDf, 1);

    soc_mem_field32_set(unit, L2_ENTRY_1m, &match_mask, WIDEf, 1);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &match_data, WIDEf, 0);

    field_len = soc_mem_field_length(unit, L2_ENTRY_1m, KEY_TYPEf);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &match_mask, KEY_TYPEf,
            (1 << field_len) - 1);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &match_data, KEY_TYPEf,
            SOC_MEM_KEY_L2_ENTRY_1_PE_VID_PE_VID);

    sal_memset(&l2_bulk, 0, sizeof(l2_bulk));
    sal_memcpy(&l2_bulk, &match_mask, sizeof(match_mask));
    match_mask_index = 1;
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, match_mask_index, &l2_bulk);
    if (BCM_SUCCESS(rv)) {
        sal_memset(&l2_bulk, 0, sizeof(l2_bulk));
        sal_memcpy(&l2_bulk, &match_data, sizeof(match_data));
        match_data_index = 0;
        rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, match_data_index, &l2_bulk);
    }

    /* Set bulk control */
    rval = 0;
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 1);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, BURST_ENTRIESf, 
                      _SOC_TR3_L2_BULK_BURST_MAX);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                      soc_mem_index_count(unit, L2_ENTRY_1m));
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, EXTERNAL_L2_ENTRYf, 0);
    if (BCM_SUCCESS(rv)) {
        rv = WRITE_L2_BULK_CONTROLr(unit, rval);
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
    }

    soc_mem_unlock(unit, L2_ENTRY_1m);

    if(enabled) {
        SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_start(unit, seconds));
    }

    return rv;
}

/*
 * Purpose:
 *      Get a downstream forwarding entry
 * Parameters:
 *      unit - (IN) Device Number
 *      forward_entry - (IN/OUT) Forwarding entry
 */
int
bcm_tr3_extender_forward_get(int unit, bcm_extender_forward_t *forward_entry)
{
    int rv = BCM_E_NONE;
    l2_entry_1_entry_t l2_entry, l2_entry_out;
    int idx;
    _bcm_gport_dest_t dest;
    int l2mc_index;
    int lower_th, higher_th;

    if (forward_entry->name_space > 0xfff) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit,
                                   bcmSwitchExtenderMulticastLowerThreshold,
                                   &lower_th));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit,
                                   bcmSwitchExtenderMulticastHigherThreshold, 
                                   &higher_th));
   
    if (forward_entry->extended_port_vid >=
        (1 << soc_mem_field_length(unit, L2_ENTRY_1m, PE_VID__ETAG_VIDf))) {
        return BCM_E_PARAM;
    }

    if (!(forward_entry->flags & BCM_EXTENDER_FORWARD_MULTICAST)) {
        if ((forward_entry->extended_port_vid >= lower_th) &&
            (forward_entry->extended_port_vid <= higher_th)) {
            return BCM_E_PARAM;
        }
    } else { /* Multicast forward entry */
        if ((forward_entry->extended_port_vid < lower_th) ||
               (forward_entry->extended_port_vid > higher_th)) {
            return BCM_E_PARAM;
        }
    }

    sal_memset(&l2_entry, 0, sizeof(l2_entry));
    soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, KEY_TYPEf,
            SOC_MEM_KEY_L2_ENTRY_1_PE_VID_PE_VID);
    soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__NAMESPACEf,
            forward_entry->name_space);
    soc_L2_ENTRY_1m_field32_set(unit, &l2_entry, PE_VID__ETAG_VIDf,
            forward_entry->extended_port_vid);
    soc_mem_lock(unit, L2_ENTRY_1m);
    rv = soc_mem_search(unit, L2_ENTRY_1m, MEM_BLOCK_ALL, &idx, &l2_entry,
            &l2_entry_out, 0);
    soc_mem_unlock(unit, L2_ENTRY_1m);

    if (SOC_SUCCESS(rv)) {
        if (!(forward_entry->flags & BCM_EXTENDER_FORWARD_MULTICAST)) {
            if (soc_L2_ENTRY_1m_field32_get(unit, &l2_entry_out,
                        PE_VID__DEST_TYPEf)) {
                dest.tgid = soc_L2_ENTRY_1m_field32_get(unit, &l2_entry_out,
                        PE_VID__TGIDf);
                dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
            } else {
                dest.modid = soc_L2_ENTRY_1m_field32_get(unit, &l2_entry_out,
                        PE_VID__MODULE_IDf);
                dest.port = soc_L2_ENTRY_1m_field32_get(unit, &l2_entry_out,
                        PE_VID__PORT_NUMf);
                dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
            }
            BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest,
                        &(forward_entry->dest_port)));
        } else {
            l2mc_index = soc_L2_ENTRY_1m_field32_get(unit, &l2_entry_out,
                    PE_VID__L2MC_PTRf);
            _BCM_MULTICAST_GROUP_SET(forward_entry->dest_multicast,
                    _BCM_MULTICAST_TYPE_L2, l2mc_index);
        }
        forward_entry->class_id = soc_L2_ENTRY_1m_field32_get(unit, &l2_entry_out,
                                                              PE_VID__CLASS_IDf);
    }

    return rv;
}

/*
 * Purpose:
 *      Traverse all valid downstream forwarding entries and call the
 *      supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      cb        - User callback function, called once per forwarding entry.
 *      user_data - cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_extender_forward_traverse(int unit,
                             bcm_extender_forward_traverse_cb cb,
                             void *user_data)
{
    int rv = BCM_E_NONE;
    int chunk_entries, chunk_bytes;
    uint8 *l2_tbl_chunk = NULL;
    int chunk_idx_min, chunk_idx_max;
    int ent_idx;
    l2_entry_1_entry_t *l2_entry;
    bcm_extender_forward_t forward_entry;
    int l2mc_index;
    _bcm_gport_dest_t dest;
    int lower_th, higher_th;

    /* Get ETAG VID multicast range */
    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                bcmSwitchExtenderMulticastLowerThreshold, &lower_th));
    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                bcmSwitchExtenderMulticastHigherThreshold, &higher_th));

    chunk_entries = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);
    chunk_bytes = 4 * SOC_MEM_WORDS(unit, L2_ENTRY_1m) * chunk_entries;
    l2_tbl_chunk = soc_cm_salloc(unit, chunk_bytes, "extender forward traverse");
    if (NULL == l2_tbl_chunk) {
        return BCM_E_MEMORY;
    }
    for (chunk_idx_min = soc_mem_index_min(unit, L2_ENTRY_1m); 
         chunk_idx_min <= soc_mem_index_max(unit, L2_ENTRY_1m); 
         chunk_idx_min += chunk_entries) {

        /* Read a chunk of L2 table */
        sal_memset(l2_tbl_chunk, 0, chunk_bytes);
        chunk_idx_max = chunk_idx_min + chunk_entries - 1;
        if (chunk_idx_max > soc_mem_index_max(unit, L2_ENTRY_1m)) {
            chunk_idx_max = soc_mem_index_max(unit, L2_ENTRY_1m);
        }
        rv = soc_mem_read_range(unit, L2_ENTRY_1m, MEM_BLOCK_ANY,
                                chunk_idx_min, chunk_idx_max, l2_tbl_chunk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        for (ent_idx = 0;
             ent_idx <= (chunk_idx_max - chunk_idx_min);
             ent_idx++) {
            l2_entry = soc_mem_table_idx_to_pointer(unit, L2_ENTRY_1m,
                    l2_entry_1_entry_t *, l2_tbl_chunk, ent_idx);

            if (soc_L2_ENTRY_1m_field32_get(unit, l2_entry, VALIDf) == 0) {
                continue;
            }
            if (soc_L2_ENTRY_1m_field32_get(unit, l2_entry, KEY_TYPEf) != 
                    SOC_MEM_KEY_L2_ENTRY_1_PE_VID_PE_VID) {
                continue;
            }

            /* Get fields of forwarding entry */
            bcm_extender_forward_t_init(&forward_entry);
            forward_entry.name_space = soc_L2_ENTRY_1m_field32_get(unit,
                    l2_entry, PE_VID__NAMESPACEf);
            forward_entry.extended_port_vid = soc_L2_ENTRY_1m_field32_get(unit,
                    l2_entry, PE_VID__ETAG_VIDf);
            forward_entry.class_id = soc_L2_ENTRY_1m_field32_get(unit,
                    l2_entry, PE_VID__CLASS_IDf);
            if ((forward_entry.extended_port_vid >= lower_th) &&
                (forward_entry.extended_port_vid <= higher_th)) {
                /* Multicast downstream forwarding entry */
                forward_entry.flags |= BCM_EXTENDER_FORWARD_MULTICAST;
                l2mc_index = soc_L2_ENTRY_1m_field32_get(unit, l2_entry,
                        PE_VID__L2MC_PTRf);
                _BCM_MULTICAST_GROUP_SET(forward_entry.dest_multicast,
                        _BCM_MULTICAST_TYPE_L2, l2mc_index);
            } else {
                if (soc_L2_ENTRY_1m_field32_get(unit, l2_entry,
                            PE_VID__DEST_TYPEf)) {
                    dest.tgid = soc_L2_ENTRY_1m_field32_get(unit, l2_entry,
                            PE_VID__TGIDf);
                    dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                } else {
                    dest.modid = soc_L2_ENTRY_1m_field32_get(unit, l2_entry,
                            PE_VID__MODULE_IDf);
                    dest.port = soc_L2_ENTRY_1m_field32_get(unit, l2_entry,
                            PE_VID__PORT_NUMf);
                    dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                }
                rv = _bcm_esw_gport_construct(unit, &dest,
                        &(forward_entry.dest_port));
                if (BCM_FAILURE(rv)) {
                    break;
                }
            }

            if (SOC_MEM_FIELD_VALID(unit, L2_ENTRY_1m, PE_VID__DST_CPUf)) {
                if (soc_L2_ENTRY_1m_field32_get(unit, l2_entry,
                            PE_VID__DST_CPUf)) {
                    forward_entry.flags |= BCM_EXTENDER_FORWARD_COPY_TO_CPU;
                }
            }

            /* Invoke callback */
            rv = cb(unit, &forward_entry, user_data);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    soc_cm_sfree(unit, l2_tbl_chunk);

    return rv;
}

/*
 * Function:
 *      bcm_tr3_etag_ethertype_set
 * Purpose:
 *      Set ETAG Ethertype.
 * Parameters:
 *      unit      - (IN) BCM device number
 *      ethertype - (IN) Ethertype
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_etag_ethertype_set(int unit, int ethertype)
{

    if (ethertype < 0 || ethertype > 0xffff) {
        return BCM_E_PARAM;
    }

    if (soc_reg_field_valid(unit, PE_ETHERTYPEr, ETHERTYPEf)) {
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PE_ETHERTYPEr,
                                    REG_PORT_ANY, ETHERTYPEf, ethertype));
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PE_ETHERTYPEr,
                                    REG_PORT_ANY, ENABLEf, ethertype ? 1 : 0));
    }

    if (soc_reg_field_valid(unit, EGR_PE_ETHERTYPEr, ETHERTYPEf)) {
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PE_ETHERTYPEr,
                                    REG_PORT_ANY, ETHERTYPEf, ethertype));
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PE_ETHERTYPEr,
                                    REG_PORT_ANY, ENABLEf, ethertype ? 1 : 0));
    }

    if (soc_reg_field_valid(unit, EGR_PE_ETHERTYPE_2r, ETHERTYPEf)) {
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PE_ETHERTYPE_2r,
                                    REG_PORT_ANY, ETHERTYPEf, ethertype));
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PE_ETHERTYPE_2r,
                                    REG_PORT_ANY, ENABLEf, ethertype ? 1 : 0));
    }

    if (soc_reg_field_valid(unit, IDB_OBM0_PE_ETHERTYPEr, ETHERTYPEf)) {
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IDB_OBM0_PE_ETHERTYPEr,
                                    REG_PORT_ANY, ETHERTYPEf, ethertype));
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IDB_OBM0_PE_ETHERTYPEr,
                                    REG_PORT_ANY, ENABLEf, ethertype ? 1 : 0));
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_td3_rx_etype_pe[unit] = ethertype;
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_etag_ethertype_get
 * Purpose:
 *      Get ETAG Ethertype.
 * Parameters:
 *      unit      - (IN) BCM device number
 *      ethertype - (OUT) Ethertype
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_etag_ethertype_get(int unit, int *ethertype)
{
    uint32 rval;

    if (ethertype == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_PE_ETHERTYPEr(unit, &rval));
    if (soc_reg_field_get(unit, PE_ETHERTYPEr, rval, ENABLEf)) {
        *ethertype = soc_reg_field_get(unit, PE_ETHERTYPEr, rval, ETHERTYPEf);
    } else {
        *ethertype = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_extender_untagged_add
 * Purpose:
 *      Set Extender VP tagging/untagging status by adding
 *      a (Extender VP, VLAN) egress VLAN translation entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN ID. 
 *      vp   - (IN) Virtual port number.
 *      flags - (IN) Untagging indication.
 *      egr_vt_added - (OUT) Indicates if (VP, VLAN) added to egress VLAN
 *                           translation table.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_tr3_extender_untagged_add(int unit, bcm_vlan_t vlan, int vp, int flags,
        int *egr_vt_added)
{
    uint32                  vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32                  old_vent[SOC_MAX_MEM_FIELD_WORDS];
    bcm_vlan_action_set_t   action;
    uint32 profile_idx;
    int rv = BCM_E_NONE;
    soc_mem_t               mem = EGR_VLAN_XLATEm;
    if (soc_mem_is_valid(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
    }

    *egr_vt_added = FALSE;

    sal_memset(vent, 0, sizeof(vent));
    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    } else {
        soc_mem_field32_set(unit, mem, vent, VALIDf, 1);
    }
    if (soc_mem_field_valid(unit, mem, ENTRY_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, ENTRY_TYPEf, 1);
    } else if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, 1);
    }

    if (soc_mem_field_valid(unit, mem, KEY_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, 1);
    }
    soc_mem_field32_set(unit, mem, vent, DVPf, vp);
    soc_mem_field32_set(unit, mem, vent, OVIDf, vlan);

    if (!BCM_VLAN_VALID(EXTENDER_PORT_INFO(unit, vp)->match_vlan)) {
        if (!(flags & BCM_VLAN_PORT_UNTAGGED)) {
            /* No need to insert an egress vlan translation entry
             * to remove the outer tag.
             */
            return BCM_E_NONE;
        }

#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_mem_field_valid(unit, EGR_VP_VLAN_MEMBERSHIPm, UNTAGf)) {
            if (flags & BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP) {
                /* The UNTAG field of EGR_VP_VLAN_MEMBERSHIP table will
                 * be configured with untagging status. It would be redundant
                 * to insert an egress VLAN translation entry.
                 */
                return BCM_E_NONE;
            }
        } 
#endif /* BCM_TRIDENT2_SUPPORT */

        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionDelete;
        action.ot_outer = bcmVlanActionDelete;
    } else {
        /* Extender port's match_vlan is valid. It needs to be inserted
         * into the packet using an egress vlan translation entry.
         */
        soc_mem_field32_set(unit, mem, vent, NEW_OVIDf,
                EXTENDER_PORT_INFO(unit, vp)->match_vlan);
        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionReplace;
        action.ot_outer = bcmVlanActionReplace;
        if (flags & BCM_VLAN_PORT_UNTAGGED) {
            action.dt_inner = bcmVlanActionNone;
            action.ot_inner = bcmVlanActionNone;
        } else {
            action.dt_inner = bcmVlanActionCopy;
            action.ot_inner = bcmVlanActionCopy;
        }
    }
    BCM_IF_ERROR_RETURN
        (_bcm_trx_egr_vlan_action_profile_entry_add(unit, &action, &profile_idx));
    soc_mem_field32_set(unit, mem, vent, TAG_ACTION_PROFILE_PTRf,
            profile_idx);

    rv = soc_mem_insert_return_old(unit, mem, MEM_BLOCK_ALL,
                                   vent, old_vent);
    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_mem_field32_get(unit, mem, old_vent,
                                          TAG_ACTION_PROFILE_PTRf);
        rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx);
    }

    *egr_vt_added = TRUE;

    return rv;
}

/*
 * Function:
 *      bcm_tr3_extender_untagged_delete
 * Purpose:
 *      Delete (Extender VP, VLAN) egress VLAN translation entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN ID. 
 *      vp   - (IN) Virtual port number.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_tr3_extender_untagged_delete(int unit, bcm_vlan_t vlan, int vp)
{
    uint32                  vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32                  old_vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32 profile_idx;
    int rv;
    soc_mem_t               mem = EGR_VLAN_XLATEm;
    if (soc_mem_is_valid(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
    }

    sal_memset(vent, 0, sizeof(vent));
    if (soc_mem_field_valid(unit, mem, ENTRY_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, ENTRY_TYPEf, 1);
    } else if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, 1);
    }

    if (soc_mem_field_valid(unit, mem, KEY_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, 1);
    }
    soc_mem_field32_set(unit, mem, vent, DVPf, vp);
    soc_mem_field32_set(unit, mem, vent, OVIDf, vlan);

    rv = soc_mem_delete_return_old(unit, mem, MEM_BLOCK_ALL,
                                   vent, old_vent);
    if (rv == SOC_E_NONE) {
        uint32 is_valid = 0;
        if (soc_feature(unit, soc_feature_base_valid)) {
            is_valid =
                soc_mem_field32_get(unit, mem, old_vent, BASE_VALID_0f) == 3 &&
                soc_mem_field32_get(unit, mem, old_vent, BASE_VALID_1f) == 7;
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
 *      bcm_tr3_extender_untagged_get
 * Purpose:
 *      Get tagging/untagging status of a Extender virtual port by
 *      reading the (Extender VP, VLAN) egress vlan translation entry.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      vlan - (IN) VLAN to remove virtual port from.
 *      vp   - (IN) Virtual port number.
 *      flags - (OUT) Untagging status of the Extender virtual port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_extender_untagged_get(int unit, bcm_vlan_t vlan, int vp,
                                  int *flags)
{
    uint32                  vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32                  res_vent[SOC_MAX_MEM_FIELD_WORDS];
    int idx;
    uint32 profile_idx;
    int rv;
    bcm_vlan_action_set_t action;
    soc_mem_t               mem = EGR_VLAN_XLATEm;
    if (soc_mem_is_valid(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
    }

    *flags = 0;

    sal_memset(vent, 0, sizeof(vent));
    if (soc_mem_field_valid(unit, mem, ENTRY_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, ENTRY_TYPEf, 1);
    } else if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, 1);
    }

    if (soc_mem_field_valid(unit, mem, KEY_TYPEf)) {
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, 1);
    }
    soc_mem_field32_set(unit, mem, vent, DVPf, vp);
    soc_mem_field32_set(unit, mem, vent, OVIDf, vlan);

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &idx, vent, res_vent, 0);
    if (rv == SOC_E_NONE) {
        uint32 is_valid = 0;
        if (soc_feature(unit, soc_feature_base_valid)) {
            is_valid =
                soc_mem_field32_get(unit, mem, res_vent, BASE_VALID_0f) == 3 &&
                soc_mem_field32_get(unit, mem, res_vent, BASE_VALID_1f) == 7;
        } else {
            is_valid = soc_mem_field32_get(unit, mem, res_vent, VALIDf);
        }
        if (is_valid) {
            profile_idx = soc_mem_field32_get(unit, mem, res_vent,
                                              TAG_ACTION_PROFILE_PTRf);
            _bcm_trx_egr_vlan_action_profile_entry_get(unit, &action, profile_idx);

            if (!BCM_VLAN_VALID(EXTENDER_PORT_INFO(unit, vp)->match_vlan)) {
                if (bcmVlanActionDelete == action.ot_outer) {
                    *flags = BCM_VLAN_PORT_UNTAGGED;
                }
            } else {
                if (bcmVlanActionNone == action.ot_inner) {
                    *flags = BCM_VLAN_PORT_UNTAGGED;
                }
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr3_niv_port_untagged_vlan_set
 * Purpose:
 *      Set the default VLAN ID for an Extender port.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - Extender gport.
 *      vid -  VLAN ID used for packets that ingress without a VLAN tag.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_extender_port_untagged_vlan_set(int unit, bcm_port_t port,
        bcm_vlan_t vid)
{
    int                rv = BCM_E_NONE;
    int                vp;
    uint32             key_data[SOC_MAX_MEM_FIELD_WORDS];
    uint32             entry_data[SOC_MAX_MEM_FIELD_WORDS];
    int                key_type;
    bcm_module_t       mod_out;
    bcm_port_t         port_out;
    bcm_trunk_t        trunk_out;
    int                tmp_id;
    int                entry_index;
    soc_mem_t          mem = VLAN_XLATEm;

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    /* Get VP */
    if (BCM_GPORT_IS_EXTENDER_PORT(port)) {
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(port);
    } else {
        return BCM_E_PORT;
    }

    /* Construct lookup key */
    sal_memset(key_data, 0, sizeof(key_data));

    if (BCM_VLAN_VALID(EXTENDER_PORT_INFO(unit, vp)->match_vlan)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_mem_field32_set(unit, mem, key_data, VIF__VLANf,
                EXTENDER_PORT_INFO(unit, vp)->match_vlan);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    }
    soc_mem_field32_set(unit, mem, key_data, KEY_TYPEf, key_type);
    if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, mem, key_data, DATA_TYPEf, key_type);
    }

    soc_mem_field32_set(unit, mem, key_data, VIF__SRC_VIFf,
            EXTENDER_PORT_INFO(unit, vp)->extended_port_vid);

    if (soc_mem_field_valid(unit, mem, SOURCE_TYPEf)) {
        soc_mem_field32_set(unit, mem, key_data, SOURCE_TYPEf, 1);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, EXTENDER_PORT_INFO(unit, vp)->port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(EXTENDER_PORT_INFO(unit, vp)->port)) {
        soc_mem_field32_set(unit, mem, key_data, VIF__Tf, 1);
        soc_mem_field32_set(unit, mem, key_data, VIF__TGIDf, trunk_out);
    } else {
        soc_mem_field32_set(unit, mem, key_data, VIF__MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, mem, key_data, VIF__PORT_NUMf, port_out);
    }

    /* Lookup existing entry */
    SOC_IF_ERROR_RETURN(soc_mem_search(unit, mem, MEM_BLOCK_ALL,
                &entry_index, key_data, entry_data, 0));

    /* Replace entry's new VLAN */
    soc_mem_field32_set(unit, mem, entry_data, VIF__NEW_OVIDf, vid);

    /* Insert new entry */
    rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry_data);
    if (rv == SOC_E_EXISTS) {
        rv = BCM_E_NONE;
    } else {
        return BCM_E_INTERNAL;
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr3_extender_port_untagged_vlan_get
 * Purpose:
 *      Get the default VLAN ID for an Extender port.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - Extender gport.
 *      vid  - (OUT) VLAN ID used for packets that ingress without a VLAN tag.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_extender_port_untagged_vlan_get(int unit, bcm_port_t port,
        bcm_vlan_t *vid)
{
    int                rv = BCM_E_NONE;
    int                vp;
    uint32             key_data[SOC_MAX_MEM_FIELD_WORDS];
    uint32             entry_data[SOC_MAX_MEM_FIELD_WORDS];
    int                key_type;
    bcm_module_t       mod_out;
    bcm_port_t         port_out;
    bcm_trunk_t        trunk_out;
    int                tmp_id;
    int                entry_index;
    soc_mem_t          mem = VLAN_XLATEm;

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    /* Get VP */
    if (BCM_GPORT_IS_EXTENDER_PORT(port)) {
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(port);
    } else {
        return BCM_E_PORT;
    }

    /* Construct lookup key */
    sal_memset(key_data, 0, sizeof(key_data));

    if (BCM_VLAN_VALID(EXTENDER_PORT_INFO(unit, vp)->match_vlan)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_mem_field32_set(unit, mem, key_data, VIF__VLANf,
                EXTENDER_PORT_INFO(unit, vp)->match_vlan);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    }
    soc_mem_field32_set(unit, mem, key_data, KEY_TYPEf, key_type);
    if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, mem, key_data, DATA_TYPEf, key_type);
    }

    soc_mem_field32_set(unit, mem, key_data, VIF__SRC_VIFf,
            EXTENDER_PORT_INFO(unit, vp)->extended_port_vid);

    if (soc_mem_field_valid(unit, mem, SOURCE_TYPEf)) {
        soc_mem_field32_set(unit, mem, key_data, SOURCE_TYPEf, 1);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, EXTENDER_PORT_INFO(unit, vp)->port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(EXTENDER_PORT_INFO(unit, vp)->port)) {
        soc_mem_field32_set(unit, mem, key_data, VIF__Tf, 1);
        soc_mem_field32_set(unit, mem, key_data, VIF__TGIDf, trunk_out);
    } else {
        soc_mem_field32_set(unit, mem, key_data, VIF__MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, mem, key_data, VIF__PORT_NUMf, port_out);
    }

    /* Lookup existing entry */
    SOC_IF_ERROR_RETURN(soc_mem_search(unit, mem, MEM_BLOCK_ALL,
                &entry_index, key_data, entry_data, 0));

    /* Get entry's VLAN */
    *vid = soc_mem_field32_get(unit, mem, entry_data, VIF__NEW_OVIDf);

    return rv;
}

/*
 * Function:
 *      bcm_tr3_extender_port_learn_set
 * Purpose:
 *      Set the CML bits for an extender port id.
 * Parameters:
 *      unit             - (IN) SOC unit number.
 *      extender_port_id - (IN) Extender port id.
 *      flags            - (IN) Logical OR of BCM_PORT_LEARN_xxx flags
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_extender_port_learn_set(int unit, bcm_gport_t extender_port_id,
                                uint32 flags)
{
    int               vp = -1;
    int               cml = 0;
    int               rv = BCM_E_NONE;
    int               entry_type;
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
    vp = BCM_GPORT_EXTENDER_PORT_ID_GET(extender_port_id);
    if (vp == -1) {
       return BCM_E_PARAM;
    }

    MEM_LOCK(unit, SOURCE_VPm);
    /* Be sure the entry is used and is set for Extender */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return BCM_E_NOT_FOUND;
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (SOC_FAILURE(rv)) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return rv;
    }

    entry_type = soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf);
    if (entry_type != 3) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return BCM_E_NOT_FOUND;
    }

    soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, cml);
    soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, cml);
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);

    MEM_UNLOCK(unit, SOURCE_VPm);
    return rv;
}

/*
 * Function:
 *      bcm_tr3_extender_port_learn_get
 * Purpose:
 *      Get the CML bits for an extender port id
 * Parameters:
 *      unit             - (IN) SOC unit number.
 *      extender_port_id - (IN) Extender port id.
 *      flags            - (OUT) Logical OR of BCM_PORT_LEARN_xxx flags
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_extender_port_learn_get(int unit, bcm_gport_t extender_port_id,
                                uint32 *flags)
{
    int               vp = -1;
    int               cml = 0;
    int               rv = BCM_E_NONE;
    int               entry_type;
    source_vp_entry_t svp;

    /* Get the VP index from the gport */
    vp = BCM_GPORT_EXTENDER_PORT_ID_GET(extender_port_id);
    if (vp == -1) {
       return BCM_E_PARAM;
    }

    /* Be sure the entry is used and is set for EXTENDER */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
        return BCM_E_NOT_FOUND;
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        return rv;
    }

    entry_type = soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf);
    if (entry_type != 3) {
        return BCM_E_NOT_FOUND;
    }

    cml = soc_SOURCE_VPm_field32_get(unit, &svp, CML_FLAGS_NEWf);

    *flags = 0;
    if (!(cml & (1 << 0))) {
       *flags |= BCM_PORT_LEARN_FWD;
    }
    if (cml & (1 << 1)) {
       *flags |= BCM_PORT_LEARN_CPU;
    }
    if (cml & (1 << 2)) {
       *flags |= BCM_PORT_LEARN_PENDING;
    }
    if (cml & (1 << 3)) {
       *flags |= BCM_PORT_LEARN_ARL;
    }
    return BCM_E_NONE;
}

#ifdef BCM_TRIDENT2_SUPPORT

/*
 * Function:
 *      _bcm_td2_extender_match_vp_replace
 * Purpose:
 *      Replace VP value in Extender VP's match entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vp - (IN) Extender VP whose match entry is being replaced.
 *      new_vp - (IN) New VP value.
 *      old_vp - (OUT) Old VP value.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_td2_extender_match_vp_replace(int unit, int vp, int new_vp, int *old_vp)
{
    int                rv = BCM_E_NONE;
    uint32             key_data[SOC_MAX_MEM_FIELD_WORDS];
    uint32             entry_data[SOC_MAX_MEM_FIELD_WORDS];
    int                key_type;
    bcm_module_t       mod_out;
    bcm_port_t         port_out;
    bcm_trunk_t        trunk_out;
    int                tmp_id;
    int                entry_index;
    soc_mem_t          vx_mem = VLAN_XLATEm;

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        vx_mem = VLAN_XLATE_1_DOUBLEm;
    }

    /* Construct lookup key */
    sal_memset(key_data, 0, sizeof(key_data));

    if (BCM_VLAN_VALID(EXTENDER_PORT_INFO(unit, vp)->match_vlan)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_mem_field32_set(unit, vx_mem, key_data, VIF__VLANf,
                EXTENDER_PORT_INFO(unit, vp)->match_vlan);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    }
    soc_mem_field32_set(unit, vx_mem, key_data, KEY_TYPEf, key_type);
    if (SOC_MEM_FIELD_VALID(unit, vx_mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, vx_mem, key_data, DATA_TYPEf, key_type);
    }

    soc_mem_field32_set(unit, vx_mem, key_data, VIF__SRC_VIFf,
            EXTENDER_PORT_INFO(unit, vp)->extended_port_vid);
    if (soc_mem_field_valid(unit, vx_mem, SOURCE_TYPEf)) {
        soc_mem_field32_set(unit, vx_mem, key_data, SOURCE_TYPEf, 1);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, EXTENDER_PORT_INFO(unit, vp)->port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(EXTENDER_PORT_INFO(unit, vp)->port)) {
        soc_mem_field32_set(unit, vx_mem, key_data, VIF__Tf, 1);
        soc_mem_field32_set(unit, vx_mem, key_data, VIF__TGIDf, trunk_out);
    } else {
        soc_mem_field32_set(unit, vx_mem, key_data, VIF__MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, vx_mem, key_data, VIF__PORT_NUMf, port_out);
    }

    /* Lookup existing entry */
    SOC_IF_ERROR_RETURN(soc_mem_search(unit, vx_mem, MEM_BLOCK_ALL,
                &entry_index, key_data, entry_data, 0));

    /* Replace entry's VP value */
    *old_vp = soc_mem_field32_get(unit, vx_mem, entry_data, VIF__SOURCE_VPf);
    soc_mem_field32_set(unit, vx_mem, entry_data, VIF__SOURCE_VPf, new_vp);

    /* Insert new entry */
    rv = soc_mem_insert(unit, vx_mem, MEM_BLOCK_ALL, entry_data);
    if (rv == SOC_E_EXISTS) {
        rv = BCM_E_NONE;
    } else {
        return BCM_E_INTERNAL;
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_extender_port_source_vp_lag_set
 * Purpose:
 *      Set source VP LAG for an Extender virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) Extender virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_extender_port_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
    int vp, old_vp;

    if (!BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
        return BCM_E_PARAM;
    }

    /* Set source VP LAG by replacing the SVP field in Extender VP's
     * match entry with the VP value representing the VP LAG.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td2_extender_match_vp_replace(unit, vp, vp_lag_vp, &old_vp));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_extender_port_source_vp_lag_clear
 * Purpose:
 *      Clear source VP LAG for an Extender virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) Extender virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_extender_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
    int vp, old_vp;

    if (!BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
        return BCM_E_PARAM;
    }

    /* Clear source VP LAG by replacing the SVP field in Extender VP's
     * match entry with the VP value.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td2_extender_match_vp_replace(unit, vp, vp, &old_vp));

    /* Check that the old VP value matches the VP value representing
     * the VP LAG or has been restored.
     */
    if ((old_vp != vp_lag_vp) && (old_vp != vp)) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_extender_port_source_vp_lag_get
 * Purpose:
 *      Set source VP LAG for an Extender virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) Extender virtual port GPORT ID. 
 *      vp_lag_vp - (OUT) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_extender_port_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp)
{
    int                vp;
    uint32             key_data[SOC_MAX_MEM_FIELD_WORDS];
    uint32             entry_data[SOC_MAX_MEM_FIELD_WORDS];
    int                key_type;
    bcm_module_t       mod_out;
    bcm_port_t         port_out;
    bcm_trunk_t        trunk_out;
    int                tmp_id;
    int                entry_index;
    soc_mem_t          vx_mem = VLAN_XLATEm;

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        vx_mem = VLAN_XLATE_1_DOUBLEm;
    }

    if (!BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
        return BCM_E_PARAM;
    }

    /* Construct match entry lookup key */
    sal_memset(key_data, 0, sizeof(key_data));

    if (BCM_VLAN_VALID(EXTENDER_PORT_INFO(unit, vp)->match_vlan)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_mem_field32_set(unit, vx_mem, key_data, VIF__VLANf,
                EXTENDER_PORT_INFO(unit, vp)->match_vlan);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    }
    soc_mem_field32_set(unit, vx_mem, key_data, KEY_TYPEf, key_type);
    if (SOC_MEM_FIELD_VALID(unit, vx_mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, vx_mem, key_data, DATA_TYPEf, key_type);
    }

    soc_mem_field32_set(unit, vx_mem, key_data, VIF__SRC_VIFf,
            EXTENDER_PORT_INFO(unit, vp)->extended_port_vid);
    if (soc_mem_field_valid(unit, vx_mem, SOURCE_TYPEf)) {
        soc_mem_field32_set(unit, vx_mem, key_data, SOURCE_TYPEf, 1);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, EXTENDER_PORT_INFO(unit, vp)->port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(EXTENDER_PORT_INFO(unit, vp)->port)) {
        soc_mem_field32_set(unit, vx_mem, key_data, VIF__Tf, 1);
        soc_mem_field32_set(unit, vx_mem, key_data, VIF__TGIDf, trunk_out);
    } else {
        soc_mem_field32_set(unit, vx_mem, key_data, VIF__MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, vx_mem, key_data, VIF__PORT_NUMf, port_out);
    }

    /* Lookup existing entry */
    SOC_IF_ERROR_RETURN(soc_mem_search(unit, vx_mem, MEM_BLOCK_ALL,
                &entry_index, key_data, entry_data, 0));

    /* Get VP value representing VP LAG */
    *vp_lag_vp = soc_mem_field32_get(unit, vx_mem, entry_data, VIF__SOURCE_VPf);
    if (!_bcm_vp_used_get(unit, *vp_lag_vp, _bcmVpTypeVpLag)) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

#endif /* BCM_TRIDENT2_SUPPORT */

/* Function:
 *      _bcm_tr3_extender_local_port_match
 * Purpose:
 *      Determine if two gports represent the same local physical port(s).
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      gport1 - (IN) GPORT ID 1
 *      gport2 - (IN) GPORT ID 2
 *      match  - (OUT) Match indication
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_extender_local_port_match(int unit, bcm_gport_t gport1,
        bcm_gport_t gport2, int *match)
{
    bcm_module_t mod1, mod2;
    bcm_port_t port1, port2;
    bcm_trunk_t tgid1, tgid2;
    int id1, id2;
    int mod_local;

    *match = FALSE;

    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, gport1,
                &mod1, &port1, &tgid1, &id1));
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, gport2,
                &mod2, &port2, &tgid2, &id2));
    if (id1 != -1 || id2 != -1) {
        return BCM_E_PORT;
    }

    if (BCM_TRUNK_INVALID != tgid1) {
        if (BCM_TRUNK_INVALID != tgid2) {
            if (tgid1 == tgid2) {
                *match = TRUE;
            }
        }
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, mod1, &mod_local));
        if (!mod_local) {
            return BCM_E_PORT;
        }
        if (BCM_TRUNK_INVALID == tgid2) {
            BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, mod2, &mod_local));
            if (!mod_local) {
                return BCM_E_PORT;
            }
            if (mod1 == mod2 && port1 == port2) {
                *match = TRUE;
            }
        }
    }

    return BCM_E_NONE;
}

/* Function:
 *      _bcm_tr3_extender_egress_match_ex_port
 * Purpose:
 *      Determine if two Extender egress objects' attributes match, except for
 *      port.
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      extender_egress1 - (IN) Extender egress object 1, represented by
 *                                  bcm_extender_egress_t.
 *      extender_egress2 - (IN) Extender egress object 2, represented by
 *                                 _bcm_tr3_extender_egress_t.
 *      match - (OUT) Match indication
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_extender_egress_match_ex_port(int unit,
        bcm_extender_egress_t *extender_egress1,
        _bcm_tr3_extender_egress_t *extender_egress2,
        int *match)
{
    int flags_match, vlan_match = 1, etag_vid_match ;
    int etag_pcp_match = 1, etag_de_match = 1;
    int pcp_de_select_match = 1, l3_intf_match = 1;
    int service_qos_map_id_match = 1, service_tpid_match = 1;
    int service_vlan_match = 1, service_pri_match = 1;
    int service_cfi_match = 1, qos_map_id_match = 1;

    flags_match = (extender_egress1->flags == extender_egress2->flags);
    etag_vid_match   = (extender_egress1->extended_port_vid ==
                        extender_egress2->extended_port_vid);
    etag_pcp_match = (extender_egress1->pcp == extender_egress2->pcp);
    etag_de_match = (extender_egress1->de == extender_egress2->de);
    pcp_de_select_match  = (extender_egress1->pcp_de_select  ==
                             extender_egress2->pcp_de_select);
    qos_map_id_match  = (extender_egress1->qos_map_id  ==
                         extender_egress2->qos_map_id);
    if (extender_egress1->flags & BCM_EXTENDER_EGRESS_L3) {
        l3_intf_match = (extender_egress1->intf == extender_egress2->intf);
    } else {
        vlan_match  = (extender_egress1->match_vlan == extender_egress2->match_vlan);
        service_tpid_match = (extender_egress1->service_tpid == extender_egress2->service_tpid);
        service_vlan_match = (extender_egress1->service_vlan == extender_egress2->service_vlan);
        service_pri_match  = (extender_egress1->service_pri  == extender_egress2->service_pri);
        service_cfi_match  = (extender_egress1->service_cfi  == extender_egress2->service_cfi);
        service_qos_map_id_match = (extender_egress1->service_qos_map_id ==
                                   extender_egress2->service_qos_map_id);
    }

    if (flags_match && vlan_match && etag_vid_match &&
            etag_pcp_match && etag_de_match &&
            pcp_de_select_match && qos_map_id_match &&
            service_vlan_match && service_tpid_match &&
            service_pri_match && service_cfi_match &&
            service_qos_map_id_match && l3_intf_match) {
        *match = TRUE;
    } else {
        *match = FALSE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_extender_egress_hash_calc
 * Purpose:
 *      Calculate the hash value according to the egress object info.
 * Parameters:
 *      unit     -  (IN) SOC unit number.
 *      hash_key -  (IN) hash key.
 *      hash     -  (OUT) Hash(signature) calculated value.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_tr3_extender_egress_hash_calc(int unit, void *hash_key, uint16 *hash)
{
    uint32 sw_buf[8];
    int size;
    int buf_idx = 0;
    bcm_extender_egress_t *extender_egress;

    /* Input parameters check. */
    if ((NULL == hash_key) || (NULL == hash)) {
        return (BCM_E_PARAM);
    }
    size = sizeof(sw_buf);
    sal_memset(sw_buf, 0, size);
    extender_egress = (bcm_extender_egress_t *)hash_key;

    /*
     * Copy entry information to a temporary buffer, so we can
     * mask some fields we don't want to include in hash calculation.
     */
    sw_buf[buf_idx++] = extender_egress->flags;
    sw_buf[buf_idx++] = extender_egress->intf;
    sw_buf[buf_idx++] = ((extender_egress->extended_port_vid << 16) |
                          (extender_egress->pcp << 8) | extender_egress->de);
    sw_buf[buf_idx++] = extender_egress->qos_map_id;
    if (!(extender_egress->flags & BCM_EXTENDER_EGRESS_L3)) {
        sw_buf[buf_idx++] = ((extender_egress->service_tpid << 16) |
                              extender_egress->service_vlan);
        sw_buf[buf_idx++] = ((extender_egress->service_pri << 16) |
                              extender_egress->service_cfi);
        sw_buf[buf_idx++] = extender_egress->service_qos_map_id;
    }

    size = sizeof(uint32) * buf_idx;
    *hash = _shr_crc16(0, (uint8 *)&sw_buf[0], size);

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_tr3_extender_egress_pbmp_get
 * Purpose:
 *      Get the pbmp of the multicast egress objects
 *      attaching to vp in the linked list.
 * Parameters:
 *      unit  - (IN) SOC unit Number
 *      vp    - (IN) Virtual port number.
 *      pbmp  - (OUT) pbmp of the multicast egress objects attaching to vp.
 * Returns:
 *      BCM_E_XXX
 */
STATIC void
_bcm_tr3_extender_egress_pbmp_get(int unit, int vp, bcm_pbmp_t *pbmp)
{
    _bcm_tr3_extender_nh_info_t  *nh_info;
    BCM_PBMP_CLEAR(*pbmp);

    nh_info = EXTENDER_PORT_MC_NH_INFO(unit, vp);
    while (nh_info) {
        BCM_PBMP_PORT_ADD(*pbmp, nh_info->port);
        nh_info = nh_info->next;
    }

    return;
}

/*
 * Function:
 *      _bcm_tr3_extender_egress_pbmp_delete
 * Purpose:
 *      delete the pbmp of the multicast egress objects
 *      attaching to vp from the linked list.
 *      decrease the next hop ref_count of the port.
 *      if next hop ref_count of the port is zero,
 *      delete the node of the port from the list.
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      vp   - (IN) Virtual port number.
 *      pbmp - (IN) pbmp of the multicast egress objects attaching to vp.
 * Returns:
 *      BCM_E_XXX
 */
STATIC void
_bcm_tr3_extender_egress_pbmp_delete(int unit, int vp, bcm_pbmp_t pbmp)
{
    _bcm_tr3_extender_nh_info_t      *ni_pre, *ni_cur, *ni_del;

    ni_cur  = EXTENDER_PORT_MC_NH_INFO(unit, vp);
    ni_pre  = ni_cur;
    ni_del  = NULL;
    while (ni_cur) {
        if (!BCM_PBMP_MEMBER(pbmp, ni_cur->port)) {
            ni_pre = ni_cur;
            ni_cur = ni_cur->next;
            continue;
        }

        /* port match */
        ni_cur->nh_ref_cnt--;
        if (ni_cur->nh_ref_cnt > 0) {
            ni_pre = ni_cur;
            ni_cur = ni_cur->next;
            continue;
        }

        /* Next hop refence count is zero. */
        if (ni_cur == EXTENDER_PORT_MC_NH_INFO(unit, vp)) {
            EXTENDER_PORT_MC_NH_INFO(unit, vp) = ni_cur->next;
            ni_del = ni_cur;
            ni_cur = ni_cur->next;
            ni_pre = ni_cur;
        } else {
            ni_del = ni_cur;
            ni_pre->next = ni_cur->next;
            ni_cur = ni_cur->next;
        }
        sal_free(ni_del);
    }

    return;
}

/*
 * Function:
 *      _bcm_tr3_extender_egress_pbmp_add
 * Purpose:
 *      add the pbmp of the multicast egress objects
 *      attaching to vp to the linked list.
 *      decrease the next hop ref_count of the port.
 *      if next hop ref_count of the port is zero,
 *      delete the node of the port from the list.
 * Parameters:
 *      unit  - (IN) SOC unit Number
 *      vp    - (IN) Virtual port number.
 *      pbmp  - (IN) pbmp of the multicast egress objects attaching to vp.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_extender_egress_pbmp_add(int unit, int vp, bcm_pbmp_t pbmp)
{
    bcm_port_t                      port;
    int                             alloc_size;
    _bcm_tr3_extender_nh_info_t      *nh_info;
    bcm_pbmp_t                      sucess_pbmp;

    nh_info = EXTENDER_PORT_MC_NH_INFO(unit, vp);
    while (nh_info) {
        if (BCM_PBMP_MEMBER(pbmp, nh_info->port)) {
            nh_info->nh_ref_cnt++;
            BCM_PBMP_PORT_REMOVE(pbmp, nh_info->port);
        }
        nh_info = nh_info->next;
    }

    BCM_PBMP_CLEAR(sucess_pbmp);
    alloc_size = sizeof(_bcm_tr3_extender_nh_info_t);
    BCM_PBMP_ITER(pbmp, port) {
        nh_info = sal_alloc(alloc_size, "EXTENDER Egress Next Hop Info");
        if (NULL == nh_info) {
            _bcm_tr3_extender_egress_pbmp_delete(unit, vp, sucess_pbmp);
            return BCM_E_MEMORY;
        }
        sal_memset(nh_info, 0, alloc_size);
        nh_info->port = port;
        nh_info->nh_ref_cnt = 1;
        nh_info->next = EXTENDER_PORT_MC_NH_INFO(unit, vp);
        EXTENDER_PORT_MC_NH_INFO(unit, vp) = nh_info;
        BCM_PBMP_PORT_ADD(sucess_pbmp, port);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_extender_egress_pbmp_clear
 * Purpose:
 *      clear the linked list which maintain the pbmp of
 *      the multicast egress objects attaching to vp.
 * Parameters:
 *      unit  - (IN) SOC unit Number
 *      vp    - (IN) Virtual port number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC void
_bcm_tr3_extender_egress_pbmp_clear(int unit, int vp)
{
    _bcm_tr3_extender_nh_info_t  *cur_node, *next_node;

    next_node = NULL;
    cur_node = EXTENDER_PORT_MC_NH_INFO(unit, vp);
    while (cur_node) {
        next_node = cur_node->next;
        sal_free(cur_node);
        cur_node = next_node;
    }
    EXTENDER_PORT_MC_NH_INFO(unit, vp) = NULL;

    return;
}

/* Function:
 *      bcm_tr3_extender_egress_add
 * Purpose:
 *      Add a Extender egress object to a Extender Port
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      extender_port - (IN) Extender Port
 *      extender_egress - (IN/OUT) Extender egress object, the egress_if field is output.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_extender_egress_add(
    int unit,
    bcm_gport_t extender_port,
    bcm_extender_egress_t *extender_egress)
{
    int vp;
    int match_ex_port;
    int local_port_match;
    int shared_nh_found;
    bcm_pbmp_t existing_pbmp, new_pbmp, update_pbmp;
    _bcm_tr3_extender_egress_t *curr_ptr = NULL, *temp_ptr = NULL;
    int nh_index;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t tgid_out;
    int id_out;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count;
    int i;
    int mod_local = FALSE;
    bcm_port_t local_port;
    bcm_gport_t gport;
    _bcm_tr3_extender_sd_tag_t extender_sd_tag;
    bcm_if_t intf;
    int      alloc_size;
    uint16   hash_val;
    int      bkt_idx = 0;
#ifdef BCM_TRIDENT2_SUPPORT
    int vp_routing = FALSE;
    bcm_l3_egress_t nh_info;
    uint32 nh_flags;
#endif
#if defined(BCM_HGPROXY_COE_SUPPORT)
    int is_local_subport = FALSE;
#endif

    /* Verify extender_port is a valid EXTENDER port with no match criteria */
    if (!BCM_GPORT_IS_EXTENDER_PORT(extender_port)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_EXTENDER_PORT_ID_GET(extender_port);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
        return BCM_E_NOT_FOUND;
    }
    if (!(EXTENDER_PORT_INFO(unit, vp)->flags & BCM_EXTENDER_PORT_MATCH_NONE)) {
        return BCM_E_PARAM;
    }

    if (extender_egress == NULL) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_virtual_port_routing)) {
        vp_routing = TRUE;
    }
#endif

    if (!(extender_egress->flags & BCM_EXTENDER_EGRESS_MULTICAST)) {
        /* When adding a unicast EXTENDER egress object to a EXTENDER port,
         * check that there are no other EXTENDER egress objects on the
         * EXTENDER port's egress_list. A EXTENDER port cannot be overloaded
         * with multiple unicast EXTENDER egress objects.
         */
        if (NULL != EXTENDER_PORT_INFO(unit, vp)->egress_list) {
            return BCM_E_CONFIG;
        }
        /* BCM_EXTENDER_EGRESS_L3 must be with BCM_EXTENDER_EGRESS_MULTICAST */
        if (extender_egress->flags & BCM_EXTENDER_EGRESS_L3) {
            return BCM_E_PARAM;
        }
    } else {
        /* For a multicast EXTENDER egress object, the field
         * match_vlan is not applicable.
         */
        if (extender_egress->match_vlan) {
           return BCM_E_PARAM;
        }
#ifdef BCM_TRIDENT2_SUPPORT
        if (vp_routing) {
        } else
#endif
        {
            if (extender_egress->flags & BCM_EXTENDER_EGRESS_L3) {
                return BCM_E_UNAVAIL;
            }
        }
    }

    BCM_PBMP_CLEAR(new_pbmp);
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, extender_egress->port,
                &mod_out, &port_out, &tgid_out, &id_out));
    if (BCM_TRUNK_INVALID != tgid_out) {
        BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit,
                    tgid_out, SOC_MAX_NUM_PORTS, local_member_array,
                    &local_member_count));
        /*
         * For multicast, it will be unnecessary to create next hop
         * if there is no local port member.
         */
        if (extender_egress->flags & BCM_EXTENDER_EGRESS_MULTICAST) {
            if (local_member_count <= 0) {
                return BCM_E_NONE;
            }
        }

        for (i = 0; i < local_member_count; i++) {
            BCM_PBMP_PORT_ADD(new_pbmp, local_member_array[i]);
        }
    } else {
#if defined(BCM_HGPROXY_COE_SUPPORT)
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_is_local_subport(
                unit, extender_egress->port, mod_out, port_out,
                &is_local_subport, &port_out));
        if (is_local_subport) {
            mod_local = TRUE;
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, mod_out, &mod_local));
        }

        /*
         * For multicast, it will be unnecessary to create next hop
         * if there is no local port member.
         */
        if (extender_egress->flags & BCM_EXTENDER_EGRESS_MULTICAST) {
            if (!mod_local) {
                return BCM_E_NONE;
            }
        }

        if (mod_local) {
            BCM_PBMP_PORT_ADD(new_pbmp, port_out);
        } else {
            return BCM_E_PORT;
        }
    }

    /* Traverse extender port's existing list of extender egress objects.
     * If an existing egress object has the same multicast flag,
     * extended_port_vid, match_vlan, PCP, and DE  but different port,
     * its next hop index can be shared with the new extender egress object.
     * Also, create a bitmap of the physical ports on which the VP resides.
     */
    BCM_PBMP_CLEAR(existing_pbmp);
    shared_nh_found = FALSE;
    curr_ptr = EXTENDER_PORT_INFO(unit, vp)->egress_list;
    if (extender_egress->flags & BCM_EXTENDER_EGRESS_MULTICAST) {
        BCM_IF_ERROR_RETURN(
            _bcm_tr3_extender_egress_hash_calc(unit, (void *)extender_egress,
                                              &hash_val));
        bkt_idx  = hash_val % TD_EXTENDER_PORT_HASH_BKT_SIZE;
        if (EXTENDER_PORT_INFO(unit, vp)->egress_list != NULL) {
            curr_ptr = EXTENDER_PORT_HASH_BKT(unit, vp, bkt_idx)->next;
        }
    }
    while (NULL != curr_ptr) {
        /* Determine if the attributes of the current EXTENDER egress object match
         * those of the new EXTENDER egress object, except for the port attribute.
         */
        BCM_IF_ERROR_RETURN(_bcm_tr3_extender_egress_match_ex_port(unit,
                    extender_egress, curr_ptr, &match_ex_port));
        if (match_ex_port) {
            /* Check if ports also match. If so, the EXTENDER egress object already exists. */
            BCM_IF_ERROR_RETURN(_bcm_tr3_extender_local_port_match(unit,
                        extender_egress->port, curr_ptr->port, &local_port_match));
            if (local_port_match) {
                return BCM_E_EXISTS;
            }

            /* If ports don't match, the next hop index can be shared. */
            shared_nh_found = TRUE;
            nh_index = curr_ptr->next_hop_index;
            break;
        }
        /* Advance to next EXTENDER egress object */
        curr_ptr = curr_ptr->next;
    }

    if (!shared_nh_found) {
        /* If match not found,  Configure next hop entry */
#ifdef BCM_TRIDENT2_SUPPORT
        if (extender_egress->flags & BCM_EXTENDER_EGRESS_L3) {
        bcm_l3_egress_t_init(&nh_info);
        nh_flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE;
        BCM_IF_ERROR_RETURN
            (bcm_xgs3_nh_add(unit, nh_flags, &nh_info, &nh_index));

            /* Configure next hop entry for L3MC */
            BCM_IF_ERROR_RETURN
                (_bcm_tr3_extender_l3mc_nh_info_set(unit, nh_index,
                    EXTENDER_PORT_INFO(unit, vp)->flags, extender_egress->multicast_flags,
                    extender_egress->port, extender_egress->intf,
                    extender_egress->extended_port_vid,
                    extender_egress->qos_map_id, extender_egress->pcp_de_select,
                    extender_egress->pcp, extender_egress->de, vp));
        } else
#endif
        {
            sal_memset(&extender_sd_tag, 0, sizeof(extender_sd_tag));
            extender_sd_tag.flags = extender_egress->flags;
            extender_sd_tag.service_tpid = extender_egress->service_tpid;
            extender_sd_tag.service_vlan = extender_egress->service_vlan;
            extender_sd_tag.service_pri  = extender_egress->service_pri;
            extender_sd_tag.service_cfi  = extender_egress->service_cfi;
            extender_sd_tag.service_qos_map_id = extender_egress->service_qos_map_id;

            BCM_IF_ERROR_RETURN
                (_bcm_tr3_extender_port_nh_info_set(unit,
                        extender_egress->port, extender_egress->extended_port_vid,
                        extender_egress->match_vlan, extender_egress->pcp,
                        extender_egress->de, extender_egress->pcp_de_select,
                        extender_egress->qos_map_id, &extender_sd_tag, vp, &nh_index));
        }
    } else {
        /* Increment next hop's reference count */
        _bcm_xgs3_nh_ref_cnt_incr(unit, nh_index);
    }

    /* Update next hop index in ING_DVP_TABLE */
    intf = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit) + nh_index;
    BCM_IF_ERROR_RETURN(
        _bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigUpdate, vp,
                               ING_DVP_CONFIG_INVALID_VP_TYPE, intf,
                               ING_DVP_CONFIG_INVALID_PORT_TYPE));

    /* Configure ingress VLAN translation table for unicast
     * EXTENDER egress object.
     */
    if (!(extender_egress->flags & BCM_EXTENDER_EGRESS_MULTICAST)) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_extender_port_match_add(unit,
                                    extender_egress->port,
                                    extender_egress->extended_port_vid,
                                    extender_egress->match_vlan,
                                    vp));
    }

    /* Increment the VP count on the new EXTENDER egress object's physical
     * port(s), unless the physical port(s) match those of VP's
     * other EXTENDER egress objects.
     */
    if (extender_egress->flags & BCM_EXTENDER_EGRESS_MULTICAST) {
        _bcm_tr3_extender_egress_pbmp_get(unit, vp, &existing_pbmp);
    }
    BCM_PBMP_CLEAR(update_pbmp);
    BCM_PBMP_ASSIGN(update_pbmp, new_pbmp);
    BCM_PBMP_REMOVE(update_pbmp, existing_pbmp);
    BCM_PBMP_ITER(update_pbmp, local_port) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_gport_get(unit, local_port, &gport));
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_extender_port_cnt_update(unit, gport, vp, TRUE));
    }

    if (extender_egress->flags & BCM_EXTENDER_EGRESS_MULTICAST) {
        BCM_IF_ERROR_RETURN(
            _bcm_tr3_extender_egress_pbmp_add(unit, vp, new_pbmp));
    }

    /* Insert the new EXTENDER egress object into VP's egress object list */
    curr_ptr = sal_alloc(sizeof(_bcm_tr3_extender_egress_t),
            "EXTENDER egress object");
    if (NULL == curr_ptr) {
        return BCM_E_MEMORY;
    }
    sal_memset(curr_ptr, 0, sizeof(_bcm_tr3_extender_egress_t));
    curr_ptr->flags = extender_egress->flags;
    curr_ptr->port = extender_egress->port;
    curr_ptr->extended_port_vid = extender_egress->extended_port_vid;
    curr_ptr->qos_map_id = extender_egress->qos_map_id;
    curr_ptr->pcp_de_select = extender_egress->pcp_de_select;
    curr_ptr->pcp = extender_egress->pcp;
    curr_ptr->de = extender_egress->de;
    curr_ptr->match_vlan = extender_egress->match_vlan;
    curr_ptr->service_tpid = extender_egress->service_tpid;
    curr_ptr->service_vlan = extender_egress->service_vlan;
    curr_ptr->service_pri = extender_egress->service_pri;
    curr_ptr->service_cfi = extender_egress->service_cfi;
    curr_ptr->service_qos_map_id = extender_egress->service_qos_map_id;
    curr_ptr->next_hop_index = nh_index;
    curr_ptr->intf = extender_egress->intf;
    curr_ptr->multicast_flags = extender_egress->multicast_flags;
    if (!(extender_egress->flags & BCM_EXTENDER_EGRESS_MULTICAST)) {
        curr_ptr->next = EXTENDER_PORT_INFO(unit, vp)->egress_list;
        EXTENDER_PORT_INFO(unit, vp)->egress_list = curr_ptr;
    } else {
        if ((EXTENDER_PORT_INFO(unit, vp)->egress_list == NULL)) {
            alloc_size = sizeof(_bcm_tr3_extender_egress_t) *
                         TD_EXTENDER_PORT_HASH_BKT_SIZE;
            temp_ptr = sal_alloc(alloc_size, "EXTENDER egress object");
            if (NULL == temp_ptr) {
                sal_free(curr_ptr);
                return BCM_E_MEMORY;
            }
            sal_memset(temp_ptr, 0, alloc_size);
            EXTENDER_PORT_INFO(unit, vp)->egress_list = temp_ptr;
        }
        curr_ptr->next = EXTENDER_PORT_HASH_BKT(unit, vp, bkt_idx)->next;
        EXTENDER_PORT_HASH_BKT(unit, vp, bkt_idx)->next = curr_ptr;
    }
    /* Set Extender egress object ID */
    if (extender_egress->flags & BCM_EXTENDER_EGRESS_L3) {
        extender_egress->egress_if = nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    }
    else {
        extender_egress->egress_if = nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
    }

    return BCM_E_NONE;
}


/* Function:
 *      bcm_tr3_extender_egress_delete
 * Purpose:
 *      Delete a Extender egress object from a Extender port
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      extender_port - (IN) Extender port
 *      extender_egress - (IN) Extender egress object
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_extender_egress_delete(
    int unit,
    bcm_gport_t extender_port,
    bcm_extender_egress_t *extender_egress)
{
    int vp;
    int match_found;
    bcm_pbmp_t remaining_pbmp, pbmp;
    _bcm_tr3_extender_egress_t *curr_ptr, *prev_ptr, **head;
    _bcm_tr3_extender_egress_t *match_ptr = NULL;
    int local_port_match;
    int match_ex_port;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t tgid_out;
    int id_out;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count;
    int i;
    int mod_local = FALSE;
    bcm_port_t local_port;
    bcm_gport_t gport;
    int nh_index;
    bcm_if_t intf;
    uint16   hash_val;
    int      bkt_idx;
#if defined(BCM_HGPROXY_COE_SUPPORT)
    int is_local_subport = FALSE;
#endif

    /* Verify extender_port is a valid EXTENDER port with no match criteria */
    if (!BCM_GPORT_IS_EXTENDER_PORT(extender_port)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_EXTENDER_PORT_ID_GET(extender_port);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
        return BCM_E_NOT_FOUND;
    }
    if (!(EXTENDER_PORT_INFO(unit, vp)->flags & BCM_EXTENDER_PORT_MATCH_NONE)) {
        return BCM_E_PARAM;
    }

    if (extender_egress == NULL) {
        return BCM_E_PARAM;
    }

    /* Traverse EXTENDER port's list of EXTENDER egress objects to find the egress
     * object to be deleted. Also, among the remaining egress objects,
     * create a bitmap of the physical ports on which the VP resides.
     */
    match_found = FALSE;
    BCM_PBMP_CLEAR(remaining_pbmp);
    prev_ptr = NULL;
    if (extender_egress->flags & BCM_EXTENDER_EGRESS_MULTICAST) {
        if ((EXTENDER_PORT_INFO(unit, vp)->egress_list == NULL)) {
            return BCM_E_NOT_FOUND;
        }
        BCM_IF_ERROR_RETURN(
            _bcm_tr3_extender_egress_hash_calc(unit, (void *)extender_egress,
                                              &hash_val));
        bkt_idx  = hash_val % TD_EXTENDER_PORT_HASH_BKT_SIZE;
        head = &(EXTENDER_PORT_HASH_BKT(unit, vp, bkt_idx)->next);
    } else {
        head = &(EXTENDER_PORT_INFO(unit, vp)->egress_list);
    }
    curr_ptr = *head;
    while (NULL != curr_ptr) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_extender_egress_match_ex_port(unit,
                    extender_egress, curr_ptr, &match_ex_port));
        BCM_IF_ERROR_RETURN(_bcm_tr3_extender_local_port_match(unit,
                    extender_egress->port, curr_ptr->port, &local_port_match));
        if (match_ex_port && local_port_match) {

            match_found = TRUE;
            match_ptr = curr_ptr;
            if (curr_ptr == *head) {
                *head = curr_ptr->next;
            } else {
               /*
                * In the following line of code, Coverity thinks the
                * prev_ptr variable may still be NULL when dereferenced.
                * This situation will never occur because if curr_ptr
                * is not pointing to the head of the linked list,
                * prev_ptr would not be NULL.
                */
                /* coverity[var_deref_op : FALSE] */
                prev_ptr->next = curr_ptr->next;
            }
            break;
        } else {
            prev_ptr = curr_ptr;
        }

        /* Advance to next EXTENDER egress object */
        curr_ptr = curr_ptr->next;
    }
    if (!match_found) {
        return BCM_E_NOT_FOUND;
    }

    /* Delete ingress VLAN translation entry */
    if (!(match_ptr->flags & BCM_EXTENDER_EGRESS_MULTICAST)) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_extender_port_match_delete(unit,
                    match_ptr->port, match_ptr->extended_port_vid,
                    match_ptr->match_vlan, vp));
    }

    /* If the VP's list of remaining egress objects is empty, modify VP's
     * next hop index to invalid next hop index. If not empty,
     * pick one of the next hop indices in the list.
     */
    if (NULL == *head) {
        nh_index = EXTENDER_INFO(unit)->invalid_next_hop_index;
    } else {
        nh_index = (*head)->next_hop_index;
    }

    /* Update next hop index in ING_DVP_TABLE */
    intf = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit) + nh_index;
    BCM_IF_ERROR_RETURN(
        _bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigUpdate, vp,
                               ING_DVP_CONFIG_INVALID_VP_TYPE, intf,
                               ING_DVP_CONFIG_INVALID_PORT_TYPE));

    /* Decrement next hop's reference count. If count reaches zero,
     * free the next hop index and clear next hop table entries.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_extender_nh_info_delete(unit, match_ptr->next_hop_index));

    /* Decrement the VP count on the deleted EXTENDER egress object's physical
     * port(s), unless the physical port(s) are shared by the VP's remaining
     * EXTENDER egress objects.
     */
    BCM_PBMP_CLEAR(pbmp);
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, match_ptr->port,
                &mod_out, &port_out, &tgid_out, &id_out));
    if (BCM_TRUNK_INVALID != tgid_out) {
        BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit,
                    tgid_out, SOC_MAX_NUM_PORTS, local_member_array,
                    &local_member_count));
        for (i = 0; i < local_member_count; i++) {
            BCM_PBMP_PORT_ADD(pbmp, local_member_array[i]);
        }
    } else {
#if defined(BCM_HGPROXY_COE_SUPPORT)
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_is_local_subport(
                unit, extender_egress->port, mod_out, port_out,
                &is_local_subport, &port_out));
        if (is_local_subport) {
            mod_local = TRUE;
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, mod_out, &mod_local));
        }
        if (mod_local) {
            BCM_PBMP_PORT_ADD(pbmp, port_out);
        } else {
            return BCM_E_INTERNAL;
        }
    }
    if (extender_egress->flags & BCM_EXTENDER_EGRESS_MULTICAST) {
        _bcm_tr3_extender_egress_pbmp_delete(unit, vp, pbmp);
        _bcm_tr3_extender_egress_pbmp_get(unit, vp, &remaining_pbmp);
    }
    BCM_PBMP_REMOVE(pbmp, remaining_pbmp);
    BCM_PBMP_ITER(pbmp, local_port) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_gport_get(unit, local_port, &gport));
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_extender_port_cnt_update(unit, gport, vp, FALSE));
    }
    if ((extender_egress->flags & BCM_EXTENDER_EGRESS_MULTICAST) &&
         BCM_PBMP_IS_NULL(remaining_pbmp)) {
         sal_free(EXTENDER_PORT_INFO(unit, vp)->egress_list);
         EXTENDER_PORT_INFO(unit, vp)->egress_list = NULL;
    }
    /* Free the deleted EXTENDER egress object */
    sal_free(match_ptr);

    return BCM_E_NONE;
}

/* Function:
 *      bcm_tr3_extender_egress_set
 * Purpose:
 *      Delete existing egress object for a Extender port and add
 *      an array of Extender egress objects for the Extender port
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      extender_port - (IN) Extender port
 *      array_size      - (IN) Number of Extender egress objects.
 *      extender_egress_array - (IN/OUT) Array of Extender egress objects.
 * Returns:
 *      BCM_E_XXX
 */
 int
bcm_tr3_extender_egress_set(int unit, bcm_gport_t extender_port,
        int array_size, bcm_extender_egress_t *extender_egress_array)
{
    int vp;
    _bcm_tr3_extender_egress_t *curr_ptr, *next_ptr;
    bcm_pbmp_t pbmp;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t tgid_out;
    int id_out;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count;
    int i;
    int mod_local = FALSE;
    bcm_gport_t gport;
    bcm_port_t local_port;
    bcm_if_t intf;
    int      bkt_idx, bkt_size;
    int      vp_mc = 0;
#if defined(BCM_HGPROXY_COE_SUPPORT)
    int is_local_subport = FALSE;
#endif

    /* Verify extender_port is a valid EXTENDER port with no match criteria */
    if (!BCM_GPORT_IS_EXTENDER_PORT(extender_port)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_EXTENDER_PORT_ID_GET(extender_port);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
        return BCM_E_NOT_FOUND;
    }
    if (!(EXTENDER_PORT_INFO(unit, vp)->flags & BCM_EXTENDER_PORT_MATCH_NONE)) {
        return BCM_E_PARAM;
    }
    if ((array_size > 0) && (extender_egress_array == NULL)) {
        return BCM_E_PARAM;
    }

    /* Modify VP's next hop index to invalid next hop index */
    intf = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit) + EXTENDER_INFO(unit)->invalid_next_hop_index;
    BCM_IF_ERROR_RETURN(_bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigUpdate,
                        vp, ING_DVP_CONFIG_INVALID_VP_TYPE, intf,
                        ING_DVP_CONFIG_INVALID_PORT_TYPE));

    /* Delete the existing EXTENDER egress objects attached to the given EXTENDER port */
    if (EXTENDER_PORT_MC_NH_INFO(unit, vp) != NULL) {
        /* The shared vp associates with mulicast egress objects. */
        if (EXTENDER_PORT_INFO(unit, vp)->egress_list == NULL) {
            return BCM_E_INTERNAL;
        }
        vp_mc = 1;
    }
    BCM_PBMP_CLEAR(pbmp);
    bkt_size = vp_mc ? TD_EXTENDER_PORT_HASH_BKT_SIZE : 1;
    for (bkt_idx = 0; bkt_idx < bkt_size; bkt_idx++) {
        if (vp_mc) {
            curr_ptr = EXTENDER_PORT_HASH_BKT(unit, vp, bkt_idx)->next;
        } else {
            curr_ptr = EXTENDER_PORT_INFO(unit, vp)->egress_list;
        }
        while (NULL != curr_ptr) {
            /* Delete ingress VLAN translation entry */
            if (!(curr_ptr->flags & BCM_EXTENDER_EGRESS_MULTICAST)) {
                BCM_IF_ERROR_RETURN(_bcm_tr3_extender_port_match_delete(unit,
                            curr_ptr->port, curr_ptr->extended_port_vid,
                            curr_ptr->match_vlan, vp));
            }

            /* Decrement next hop's reference count. If count reaches zero,
             * free the next hop index and clear next hop table entries.
             */
            BCM_IF_ERROR_RETURN
                (_bcm_tr3_extender_nh_info_delete(unit, curr_ptr->next_hop_index));

            /* Add this EXTENDER egress object's port(s) to the bitmap of physical ports
             * on which the VP resides.
             */
            BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, curr_ptr->port,
                        &mod_out, &port_out, &tgid_out, &id_out));
            if (BCM_TRUNK_INVALID != tgid_out) {
                BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit,
                            tgid_out, SOC_MAX_NUM_PORTS, local_member_array,
                            &local_member_count));
                for (i = 0; i < local_member_count; i++) {
                    BCM_PBMP_PORT_ADD(pbmp, local_member_array[i]);
                }
            } else {
#if defined(BCM_HGPROXY_COE_SUPPORT)
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_port_is_local_subport(
                        unit, curr_ptr->port, mod_out, port_out,
                        &is_local_subport, &port_out));
                if (is_local_subport) {
                    mod_local = TRUE;
                } else
#endif
                {
                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_modid_is_local(unit, mod_out, &mod_local));
                }
                if (mod_local) {
                    BCM_PBMP_PORT_ADD(pbmp, port_out);
                } else {
                    return BCM_E_INTERNAL;
                }
            }

            /* Free current EXTENDER egress object and advance to the next one */
            next_ptr = curr_ptr->next;
            sal_free(curr_ptr);
            curr_ptr = next_ptr;
        }

        if (vp_mc) {
            EXTENDER_PORT_HASH_BKT(unit, vp, bkt_idx)->next = NULL;
        } else {
            EXTENDER_PORT_INFO(unit, vp)->egress_list = NULL;
        }
    }

    if (vp_mc) {
        sal_free(EXTENDER_PORT_INFO(unit, vp)->egress_list);
        EXTENDER_PORT_INFO(unit, vp)->egress_list = NULL;
        _bcm_tr3_extender_egress_pbmp_clear(unit, vp);
    }

    /* Decrement VP count on all the physical ports on which the VP resides */
    BCM_PBMP_ITER(pbmp, local_port) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_gport_get(unit, local_port, &gport));
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_extender_port_cnt_update(unit, gport, vp, FALSE));
    }

    /* Add new EXTENDER egress objects to EXTENDER port */
    for (i = 0; i < array_size; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_tr3_extender_egress_add(unit, extender_port, &extender_egress_array[i]));
    }

    return BCM_E_NONE;
}

/* Function:
 *      bcm_esw_extender_egress_get
 * Purpose:
 *      get an Extender egress object for Extender Port
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      extender_port - (IN) Extender port
 *      extender_egress - (IN/OUT) Extender egress object
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_extender_egress_get(int unit, bcm_gport_t extender_port,
        bcm_extender_egress_t *extender_egress)
{
    int vp;
    _bcm_tr3_extender_egress_t *curr_ptr;
    int bkt_idx, bkt_size;
    int vp_mc = 0;

    /* Verify extender_port is a valid Extender port with no match criteria */
    if (!BCM_GPORT_IS_EXTENDER_PORT(extender_port)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_EXTENDER_PORT_ID_GET(extender_port);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
        return BCM_E_NOT_FOUND;
    }
    if (!(EXTENDER_PORT_INFO(unit, vp)->flags & BCM_EXTENDER_PORT_MATCH_NONE)) {
        return BCM_E_PARAM;
    }
    if (extender_egress == NULL) {
        return BCM_E_PARAM;
    }

    /* Traverse Extender port's existing list of Extender egress objects */
    curr_ptr = EXTENDER_PORT_INFO(unit, vp)->egress_list;

    if (EXTENDER_PORT_MC_NH_INFO(unit, vp) != NULL) {
        /* The shared vp associates with mulicast egress objects. */
        if (EXTENDER_PORT_INFO(unit, vp)->egress_list == NULL) {
            return BCM_E_INTERNAL;
        }
        vp_mc = 1;
    }
    bkt_size = vp_mc ? TD_EXTENDER_PORT_HASH_BKT_SIZE : 1;
    for (bkt_idx = 0; bkt_idx < bkt_size; bkt_idx++) {
        if (vp_mc) {
            curr_ptr = EXTENDER_PORT_HASH_BKT(unit, vp, bkt_idx)->next;
        } else {
            curr_ptr = EXTENDER_PORT_INFO(unit, vp)->egress_list;
        }
        while (NULL != curr_ptr) {
            if ((extender_egress->port == curr_ptr->port) &&
                (extender_egress->extended_port_vid ==
                 curr_ptr->extended_port_vid) &&
                (extender_egress->flags == curr_ptr->flags)) {

                extender_egress->port = curr_ptr->port;
                extender_egress->extended_port_vid =
                    curr_ptr->extended_port_vid;
                extender_egress->qos_map_id = curr_ptr->qos_map_id;
                extender_egress->pcp_de_select =
                    curr_ptr->pcp_de_select;
                extender_egress->pcp = curr_ptr->pcp;
                extender_egress->de = curr_ptr->de;
                extender_egress->match_vlan = curr_ptr->match_vlan;
                extender_egress->service_tpid = curr_ptr->service_tpid;
                extender_egress->service_vlan = curr_ptr->service_vlan;
                extender_egress->service_pri = curr_ptr->service_pri;
                extender_egress->service_cfi = curr_ptr->service_cfi;
                extender_egress->service_qos_map_id =
                    curr_ptr->service_qos_map_id;
                extender_egress->intf = curr_ptr->intf;
                extender_egress->multicast_flags =
                    curr_ptr->multicast_flags;

                extender_egress->egress_if = curr_ptr->next_hop_index +
                                                BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                return BCM_E_NONE;
            } else {
                curr_ptr = curr_ptr->next;
            }
        }
    }
    return BCM_E_NOT_FOUND;
}

/* Function:
 *      bcm_tr3_extender_egress_get_all
 * Purpose:
 *      get an Array of Extender egress objects for Extender Port
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      extender_port - (IN) Extender port
 *      array_size      - (IN) Size of Extender egress objects array.
 *      extender_egress_array - (OUT) Array of Extender egress objects.
 *      count - (OUT) Number of Extender egress objects returned.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_extender_egress_get_all(int unit, bcm_gport_t extender_port,
        int array_size, bcm_extender_egress_t *extender_egress_array, int *count)
{
    int vp;
    _bcm_tr3_extender_egress_t *curr_ptr;
    int bkt_idx, bkt_size;
    int vp_mc = 0;

    /* Verify extender_port is a valid Extender port with no match criteria */
    if (!BCM_GPORT_IS_EXTENDER_PORT(extender_port)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_EXTENDER_PORT_ID_GET(extender_port);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
        return BCM_E_NOT_FOUND;
    }
    if (!(EXTENDER_PORT_INFO(unit, vp)->flags & BCM_EXTENDER_PORT_MATCH_NONE)) {
        return BCM_E_PARAM;
    }
    if (array_size < 0) {
        return BCM_E_PARAM;
    }
    if ((array_size > 0) && (extender_egress_array == NULL)) {
        return BCM_E_PARAM;
    }
    if (count == NULL) {
        return BCM_E_PARAM;
    }

    /* Traverse Extender port's existing list of Extender egress objects */
    *count = 0;
    curr_ptr = EXTENDER_PORT_INFO(unit, vp)->egress_list;

    if (EXTENDER_PORT_MC_NH_INFO(unit, vp) != NULL) {
        /* The shared vp associates with mulicast egress objects. */
        if (EXTENDER_PORT_INFO(unit, vp)->egress_list == NULL) {
            return BCM_E_INTERNAL;
        }
        vp_mc = 1;
    }
    bkt_size = vp_mc ? TD_EXTENDER_PORT_HASH_BKT_SIZE : 1;

    for (bkt_idx = 0; bkt_idx < bkt_size; bkt_idx++) {
        if (vp_mc) {
            curr_ptr = EXTENDER_PORT_HASH_BKT(unit, vp, bkt_idx)->next;
        } else {
            curr_ptr = EXTENDER_PORT_INFO(unit, vp)->egress_list;
        }
        while (NULL != curr_ptr) {
            if (array_size > 0) {
                extender_egress_array[*count].flags = curr_ptr->flags;
                extender_egress_array[*count].port = curr_ptr->port;
                extender_egress_array[*count].extended_port_vid=
                    curr_ptr->extended_port_vid;
                extender_egress_array[*count].match_vlan = curr_ptr->match_vlan;
                extender_egress_array[*count].pcp_de_select =
                    curr_ptr->pcp_de_select;
                extender_egress_array[*count].qos_map_id = curr_ptr->qos_map_id;
                extender_egress_array[*count].pcp = curr_ptr->pcp;
                extender_egress_array[*count].de = curr_ptr->de;
                extender_egress_array[*count].service_tpid = curr_ptr->service_tpid;
                extender_egress_array[*count].service_vlan = curr_ptr->service_vlan;
                extender_egress_array[*count].service_pri = curr_ptr->service_pri;
                extender_egress_array[*count].service_cfi = curr_ptr->service_cfi;
                extender_egress_array[*count].service_qos_map_id =
                    curr_ptr->service_qos_map_id;
                extender_egress_array[*count].intf = curr_ptr->intf;
                extender_egress_array[*count].multicast_flags =
                    curr_ptr->multicast_flags;
                if (curr_ptr->flags & BCM_EXTENDER_EGRESS_L3) {
                    extender_egress_array[*count].egress_if =
                        curr_ptr->next_hop_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                } else {
                    extender_egress_array[*count].egress_if =
                        curr_ptr->next_hop_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
                }
            }
            (*count)++;
            if (*count == array_size) {
                break;
            }
            curr_ptr = curr_ptr->next;
        }
        if ((array_size != 0) && (*count == array_size)) {
            break;
        }
    }
    return BCM_E_NONE;
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_tr3_extender_sw_dump
 * Purpose:
 *     Displays Extender module information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_tr3_extender_sw_dump(int unit)
{
    int i, num_vp;
    _bcm_tr3_extender_egress_t *curr_ptr;
    int egr_obj_index;
    char pfmt[SOC_PBMP_FMT_LEN];
    int  bkt_idx, bkt_size;
    int  vp_mc = 0;
    bcm_pbmp_t pbmp;

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information Extender - Unit %d\n"), unit));

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    LOG_CLI((BSL_META_U(unit,
                        "\n  Port Info    : \n")));
    for (i = 0; i < num_vp; i++) {
        if (!(EXTENDER_PORT_INFO(unit, i)->flags & BCM_EXTENDER_PORT_MATCH_NONE)) {
            if (EXTENDER_PORT_INFO(unit, i)->port == 0) {
                continue;
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n  Extender port vp = %d\n"), i));
            LOG_CLI((BSL_META_U(unit,
                                "Flags = 0x%x\n"), EXTENDER_PORT_INFO(unit, i)->flags));
            LOG_CLI((BSL_META_U(unit,
                                "Port = 0x%x\n"), EXTENDER_PORT_INFO(unit, i)->port));
            LOG_CLI((BSL_META_U(unit,
                                "ETAG VID = 0x%x\n"),
                     EXTENDER_PORT_INFO(unit, i)->extended_port_vid));
            LOG_CLI((BSL_META_U(unit,
                                "PCP DE Select = 0x%x\n"), 
                     EXTENDER_PORT_INFO(unit, i)->pcp_de_select));
            LOG_CLI((BSL_META_U(unit,
                                "Default PCP = 0x%x\n"), EXTENDER_PORT_INFO(unit, i)->pcp));
            LOG_CLI((BSL_META_U(unit,
                                "Default DE = 0x%x\n"), EXTENDER_PORT_INFO(unit, i)->de));
            LOG_CLI((BSL_META_U(unit,
                                "Match VLAN = 0x%x\n"), EXTENDER_PORT_INFO(unit, i)->match_vlan));
        }else {
            LOG_CLI((BSL_META_U(unit,
                                "\n  Extender port vp = %d\n"), i));
            LOG_CLI((BSL_META_U(unit,
                                "    Flags = 0x%x\n"), EXTENDER_PORT_INFO(unit, i)->flags));

            egr_obj_index = 0;
            vp_mc = 0;
            if (EXTENDER_PORT_MC_NH_INFO(unit, i) != NULL) {
                if (EXTENDER_PORT_INFO(unit, i)->egress_list == NULL) {
                    continue;
                }
                vp_mc = 1;
            }
            bkt_size = vp_mc ? TD_EXTENDER_PORT_HASH_BKT_SIZE : 1;
            for (bkt_idx = 0; bkt_idx < bkt_size; bkt_idx++) {
                /* The shared vp associates with mulicast egress objects. */
                if (vp_mc) {
                    curr_ptr = EXTENDER_PORT_HASH_BKT(unit, i, bkt_idx)->next;
                    if (curr_ptr) {
                        LOG_CLI((BSL_META_U(unit,
                                            "\n  Bkt_idx = %d\n"), bkt_idx));
                    }
                } else {
                    curr_ptr = EXTENDER_PORT_INFO(unit, i)->egress_list;
                }
                while (NULL != curr_ptr) {
                    LOG_CLI((BSL_META_U(unit,
                                        "    Egress object %d\n"),
                                        egr_obj_index));
                    LOG_CLI((BSL_META_U(unit,
                                        "      Flags = 0x%x\n"),
                                        curr_ptr->flags));
                    LOG_CLI((BSL_META_U(unit,
                                        "      Port = 0x%x\n"),
                                        curr_ptr->port));
                    LOG_CLI((BSL_META_U(unit,
                                        "      ETAG VID = 0x%x\n"),
                             curr_ptr->extended_port_vid));
                    LOG_CLI((BSL_META_U(unit,
                                        "PCP DE Select = 0x%x\n"),
                             curr_ptr->pcp_de_select));
                    LOG_CLI((BSL_META_U(unit,
                                        "Default PCP = 0x%x\n"), curr_ptr->pcp));
                    LOG_CLI((BSL_META_U(unit,
                                        "Default DE = 0x%x\n"), curr_ptr->de));

                    LOG_CLI((BSL_META_U(unit,
                                        "      Match VLAN = 0x%x\n"),
                             curr_ptr->match_vlan));
                    LOG_CLI((BSL_META_U(unit,
                                        "      Service TPID = 0x%x\n"),
                             curr_ptr->service_tpid));
                    LOG_CLI((BSL_META_U(unit,
                                        "      Service VLAN = 0x%x\n"),
                             curr_ptr->service_vlan));
                    LOG_CLI((BSL_META_U(unit,
                                        "      Service PRI = 0x%x\n"),
                             curr_ptr->service_pri));
                    LOG_CLI((BSL_META_U(unit,
                                        "      Service CFI = 0x%x\n"),
                             curr_ptr->service_cfi));
                    LOG_CLI((BSL_META_U(unit,
                                        "      Service Qos Map ID = 0x%x\n"),
                             curr_ptr->service_qos_map_id));
                    LOG_CLI((BSL_META_U(unit,
                                        "      Next Hop Index = 0x%x\n"),
                             curr_ptr->next_hop_index));
                    LOG_CLI((BSL_META_U(unit,
                                        "      L3 Intf = 0x%x\n"),
                             curr_ptr->intf));
                    LOG_CLI((BSL_META_U(unit,
                                        "      Multicast_flags = 0x%x\n"),
                             curr_ptr->multicast_flags));
                    egr_obj_index++;
                    curr_ptr = curr_ptr->next;
                }
            }

            if (vp_mc) {
                BCM_PBMP_CLEAR(pbmp);
                _bcm_tr3_extender_egress_pbmp_get(unit, i, &pbmp);
                LOG_CLI((BSL_META_U(unit,
                                    "      pbmp = %s\n"),
                         SOC_PBMP_FMT(pbmp, pfmt)));
            }

                }
    }

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#ifdef BCM_TOMAHAWK_SUPPORT
bcm_error_t
_bcm_extender_gport_get(int unit, int is_trunk, int modid,
                        int port, int etag_vid, bcm_gport_t *ext_gport)
{
    int vp;
    bcm_trunk_t trunk_id = -1;
    bcm_port_t local_port_id;
    bcm_module_t local_modid;
    int id_out;

    if (NULL == EXTENDER_INFO(unit)->port_info) {
        return BCM_E_INIT;
    }

    if (NULL == ext_gport) {
        return BCM_E_PARAM;
    }
    for (vp = soc_mem_index_min(unit, SOURCE_VPm);
         vp <= soc_mem_index_max(unit, SOURCE_VPm);
         vp++) {
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
            if (!(EXTENDER_PORT_INFO(unit, vp)->flags &
                BCM_EXTENDER_PORT_MULTICAST)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_esw_gport_resolve(unit, EXTENDER_PORT_INFO(unit, vp)->port,
                                                &local_modid,
                                                &local_port_id,
                                                &trunk_id,
                                                &id_out));                 
                if (EXTENDER_PORT_INFO(unit, vp)->extended_port_vid == etag_vid) {
                    if ((is_trunk == 1) && (trunk_id == port)) {
                        BCM_GPORT_EXTENDER_PORT_ID_SET(*ext_gport, vp);
                        return BCM_E_NONE;
                    } else if ((is_trunk != 1) && (trunk_id == -1)) {
                        if ((local_port_id == port) &&
                            (local_modid == modid )) {
                            BCM_GPORT_EXTENDER_PORT_ID_SET(*ext_gport, vp);
                            return BCM_E_NONE;
                        }
                    }
                }
            }
        }
    }
    return BCM_E_NOT_FOUND;
}
#endif /* BCM_TOMAHAWK_SUPPORT */
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

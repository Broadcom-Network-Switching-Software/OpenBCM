/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    niv.c
 * Purpose: Implements NIV APIs for Trident.
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>
#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/hash.h>
#include <soc/l2x.h>
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#endif

#include <bcm/error.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/niv.h>
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/xgs5.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT*/

#include <soc/katana2.h>
#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#endif /* BCM_SABER2_SUPPORT */

#define _NIV_ENTRY_TYPEf(u, mem) (soc_mem_field_valid(u, mem, DATA_TYPEf) ? \
                                  DATA_TYPEf : ENTRY_TYPEf)
/*
 * Software book keeping for NIV related information
 */
typedef struct _bcm_trident_niv_egress_s {
    uint32 flags;
    bcm_gport_t port;
    uint16 virtual_interface_id;
    bcm_vlan_t match_vlan;
    uint16 service_tpid;
    bcm_vlan_t service_vlan;
    uint8 service_pri;
    uint8 service_cfi;
    int service_qos_map_id;
    int next_hop_index;
    bcm_if_t intf;  
    uint32 multicast_flags;
    struct _bcm_trident_niv_egress_s *next; /* Pointer to next NIV egress
                                               object */
} _bcm_trident_niv_egress_t;

typedef struct _bcm_trident_niv_nh_info_s {
    bcm_port_t   port;
    int          nh_ref_cnt;
    struct       _bcm_trident_niv_nh_info_s *next;
} _bcm_trident_niv_nh_info_t;

typedef struct _bcm_trident_niv_port_info_s {
    uint32 flags;
    bcm_gport_t port;
    bcm_pbmp_t tp_pbmp; /* trunk port members pbmp */
    uint16 virtual_interface_id;
    bcm_vlan_t match_vlan;
    _bcm_trident_niv_egress_t *egress_list; /* Linked list of NIV egress
                                               objects */
    _bcm_trident_niv_nh_info_t *mc_nh_info;
                               /* nh info for multicast egress objects. */
} _bcm_trident_niv_port_info_t;

#define  TD_NIV_PORT_HASH_BKT_SIZE           32
typedef struct _bcm_trident_niv_bookkeeping_s {
    _bcm_trident_niv_port_info_t *port_info; /* VP state */
    int invalid_next_hop_index;
} _bcm_trident_niv_bookkeeping_t;

STATIC _bcm_trident_niv_bookkeeping_t _bcm_trident_niv_bk_info[BCM_MAX_NUM_UNITS];

#define NIV_INFO(unit) (&_bcm_trident_niv_bk_info[unit])
#define NIV_PORT_INFO(unit, vp) (&NIV_INFO(unit)->port_info[vp])
#define NIV_PORT_HASH_BKT(unit, vp, idx) \
            (&(NIV_PORT_INFO(unit, vp)->egress_list[idx]))
#define NIV_PORT_MC_NH_INFO(unit, vp) (NIV_PORT_INFO(unit, vp)->mc_nh_info)

/* Structure to facilitate passing of SD-tag parameters */
typedef struct _bcm_trident_niv_sd_tag_s {
    uint32 flags;
    uint16 service_tpid;
    bcm_vlan_t service_vlan;
    uint8 service_pri;
    uint8 service_cfi;
    int service_qos_map_id;
} _bcm_trident_niv_sd_tag_t;

STATIC int
_bcm_trident_niv_egress_hash_calc(int unit, void *hash_key, uint16 *hash);
STATIC int
_bcm_trident_niv_egress_pbmp_add(int unit, int vp, bcm_pbmp_t pbmp);

/*
 * Function:
 *      _bcm_trident_niv_port_cnt_update
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
_bcm_trident_niv_port_cnt_update(int unit, bcm_gport_t gport,
        int vp, int incr, int ingress)
{
    int mod_out, port_out, tgid_out, id_out;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count;
    int idx;
    int mod_local;
    _bcm_port_info_t *port_info;
    uint32 port_flags;
#if defined(BCM_HGPROXY_COE_SUPPORT)
    int is_local_subport;
#endif

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

        if (ingress) {
            bcm_pbmp_t pbmp;

            BCM_PBMP_CLEAR(pbmp);

            for (idx = 0; idx < local_member_count; idx++) {
                BCM_PBMP_PORT_ADD(pbmp, local_member_array[idx]);
            }
            
            if (incr) {
                BCM_PBMP_ASSIGN(NIV_PORT_INFO(unit, vp)->tp_pbmp, pbmp);
            }
            
            BCM_PBMP_ITER(NIV_PORT_INFO(unit, vp)->tp_pbmp, port_out) {
                _bcm_port_info_access(unit, port_out, &port_info);
                if (incr) {
                    port_info->vp_count++;
                } else {
                    port_info->vp_count--;
                }
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_vlan_member_get(unit, port_out, &port_flags));
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_vlan_member_set(unit, port_out, port_flags));
            }
            
            if (!incr) {
                BCM_PBMP_ASSIGN(NIV_PORT_INFO(unit, vp)->tp_pbmp, pbmp);
            }
        } else {
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
        }
    } else {
#if defined(BCM_HGPROXY_COE_SUPPORT)
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_is_local_subport(
                unit, gport, mod_out, port_out, &is_local_subport, &port_out));
        if (is_local_subport) {
            mod_local = TRUE;
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, mod_out, &mod_local));
        }
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
                    bcm_esw_port_vlan_member_get(unit, port_out, &port_flags));
            BCM_IF_ERROR_RETURN(
                    bcm_esw_port_vlan_member_set(unit, port_out, port_flags));
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_trident_niv_tpid_enb_get(int unit, uint32 *svp_ent, int vp, int *tpid_enb)
{
    soc_mem_t mem = SOURCE_VPm;

    if (soc_mem_field_valid(unit, SVP_ATTRS_1m, TPID_ENABLEf)) {
        mem = SVP_ATTRS_1m;
    }

    if (svp_ent != NULL) {
        *tpid_enb = soc_mem_field32_get(unit, mem, svp_ent, TPID_ENABLEf);
    } else {
        uint32 ent[SOC_MAX_MEM_FIELD_WORDS];
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, SOC_BLOCK_ANY, vp, ent));
        *tpid_enb = soc_mem_field32_get(unit, mem, ent, TPID_ENABLEf);
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_trident_niv_tpid_enb_set(int unit, uint32 *svp_ent, int vp, int tpid_enb)
{
    soc_mem_t mem = SOURCE_VPm;

    if (soc_mem_field_valid(unit, SVP_ATTRS_1m, TPID_ENABLEf)) {
        mem = SVP_ATTRS_1m;
    }

    if (svp_ent != NULL) {
        soc_mem_field32_set(unit, mem, svp_ent, TPID_ENABLEf, tpid_enb);
    } else {
        BCM_IF_ERROR_RETURN(soc_mem_field32_modify(unit, mem, vp, TPID_ENABLEf,
                                                   tpid_enb));
    }
    return BCM_E_NONE;
}


#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0     SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1     SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2     SOC_SCACHE_VERSION(1,2)
#define BCM_WB_VERSION_1_3     SOC_SCACHE_VERSION(1,3)
#define BCM_WB_DEFAULT_VERSION BCM_WB_VERSION_1_3

/* 
 * Function:
 *      _bcm_trident_niv_egress_hash_recover
 * Purpose:
 *      Recover the vp based hash table according to hardware information.
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      vp   - (IN) Virtual port number. 
 *      node - (IN) niv egress hardware information.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_trident_niv_egress_hash_recover(
    int unit, int vp, _bcm_trident_niv_egress_t *node)
{
    int              bkt_idx, alloc_size;
    uint16           hash_val;
    bcm_niv_egress_t niv_egress;
    _bcm_trident_niv_egress_t *temp_ptr = NULL;

    bcm_niv_egress_t_init(&niv_egress);
    niv_egress.flags                = node->flags;
    niv_egress.port                 = node->port;
    niv_egress.intf                 = node->intf;
    niv_egress.service_tpid         = node->service_tpid;
    niv_egress.service_vlan         = node->service_vlan;
    niv_egress.service_pri          = node->service_pri;
    niv_egress.service_cfi          = node->service_cfi;
    niv_egress.service_qos_map_id   = node->service_qos_map_id;
    niv_egress.virtual_interface_id = node->virtual_interface_id;
    BCM_IF_ERROR_RETURN(
        _bcm_trident_niv_egress_hash_calc(unit, (void *)&niv_egress,
                                          &hash_val));
    bkt_idx  = hash_val % TD_NIV_PORT_HASH_BKT_SIZE;
    if ((NIV_PORT_INFO(unit, vp)->egress_list == NULL)) {
        alloc_size = sizeof(_bcm_trident_niv_egress_t) *
                     TD_NIV_PORT_HASH_BKT_SIZE;
        temp_ptr = sal_alloc(alloc_size, "NIV egress object");
        if (NULL == temp_ptr) {
            return BCM_E_MEMORY;
        }
        sal_memset(temp_ptr, 0, alloc_size);
        NIV_PORT_INFO(unit, vp)->egress_list = temp_ptr;
    }
    node->next = NIV_PORT_HASH_BKT(unit, vp, bkt_idx)->next;
    NIV_PORT_HASH_BKT(unit, vp, bkt_idx)->next = node;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_trident_niv_required_scache_size_get
 * Purpose:
 *      Get required NIV module scache size for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      size - (OUT) Required scache size.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_required_scache_size_get(int unit, uint32 *size)
{
    int num_vp;
    int num_nh;
    int num_ports;

    *size = 0;

    /* Add size required to store the bitmap of NIV VPs that
     * were created without match criteria.
     */
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    *size += SHR_BITALLOCSIZE(num_vp);

    /* Add size required to store the bitmap of next hops that
     * are associated with NIV egress objects.
     */
    num_nh = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    *size += SHR_BITALLOCSIZE(num_nh);

    /* Add size required to store an array of physical port bitmaps.
     * The size of the array equals to the number of next hops.
     * A next hop entry may be shared by multiple NIV egress objects that
     * have identical attributes except for the physical port. Hence,
     * the physical port of each NIV egress object cannot be recovered
     * from hardware and needs to be stored in the next hop's physical
     * port bitmap.
     */
    num_ports = soc_mem_field_length(unit, L3_IPMCm, L3_BITMAPf);
    *size += (SHR_BITALLOCSIZE(num_ports) * num_nh);

    /* Add size required to store match_vlan. There can be at most
     * one match_vlan per NIV virtual port.
     */
    *size += (sizeof(bcm_vlan_t) * num_vp);

    /*
     * Add size required to store the bitmap of NIV VPs
     * which are added with BCM_NIV_PORT_MATCH_DO_NOT_ADD.
     */
    *size += SHR_BITALLOCSIZE(num_vp);

    /* Add size required to store the bitmap of next hops which
     * are associated with BCM_NIV_EGRESS_MULTICAST.
     */
    *size += SHR_BITALLOCSIZE(num_nh);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_trident_niv_sync
 * Purpose:
 *      Record persistent info into the scache.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_trident_niv_sync(
    int unit, 
    uint8 **scache_ptr)
{
    int rv = BCM_E_NONE;
    int num_vp, num_nh, num_ports;
    SHR_BITDCL *match_none_vp_bitmap = NULL;
    SHR_BITDCL *match_do_not_add_vp_bitmap = NULL;
    SHR_BITDCL *nh_bitmap = NULL;
    SHR_BITDCL *mc_nh_bitmap = NULL;
    SHR_BITDCL **pbmp_ptr_array = NULL;
    bcm_vlan_t *match_vlan_array = NULL;
    int i;
    _bcm_trident_niv_egress_t *curr_ptr;
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

    /* Allocate a bitmap of NIV VPs that were created without match criteria */
    match_none_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp),
            "Bitmap of NIV VPs without match criteria");
    if (NULL == match_none_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(match_none_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

    /* Allocate a bitmap of next hops associated with NIV egress objects */
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

    /*
     * Allocate a bitmap of NIV VPs which are added
     * with BCM_NIV_PORT_MATCH_DO_NOT_ADD.
     */
    match_do_not_add_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp),
            "Bitmap of NIV VPs");
    if (NULL == match_do_not_add_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(match_do_not_add_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

    /* Allocate a bitmap of next hops associated with multicast NIV egress objects */
    mc_nh_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_nh), "Bitmap of multicast NHs");
    if (NULL == mc_nh_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(mc_nh_bitmap, 0, SHR_BITALLOCSIZE(num_nh));

    /* Traverse NIV VPs */
    for (i = 0; i < num_vp; i++) {
        if (NIV_PORT_INFO(unit, i)->flags & BCM_NIV_PORT_MATCH_NONE) {
            SHR_BITSET(match_none_vp_bitmap, i);
            /* Traverse this NIV VP's list of NIV egress objects */
            vp_mc = 0;
            if (NIV_PORT_MC_NH_INFO(unit, i) != NULL) {
                /* The shared vp associates with mulicast egress objects. */
                if (NIV_PORT_INFO(unit, i)->egress_list == NULL) {
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
                }
                vp_mc = 1;
            }
            bkt_size = vp_mc ? TD_NIV_PORT_HASH_BKT_SIZE : 1;
            for (bkt_idx = 0; bkt_idx < bkt_size; bkt_idx++) {
                if (vp_mc) {
                    curr_ptr = NIV_PORT_HASH_BKT(unit, i, bkt_idx)->next;
                } else {
                    curr_ptr = NIV_PORT_INFO(unit, i)->egress_list;
                }

                while (NULL != curr_ptr) {
                    if (curr_ptr->match_vlan) {
                        /* Match_vlan can be nonzero only for unicast NIV egress
                         * objects, and there can be at most one unicast NIV
                         * egress object in the NIV port's egress_list. These
                         * constraints are enforced by bcm_trident_niv_egress_add.
                         */
                        match_vlan_array[i] = curr_ptr->match_vlan;
                    }

                    nh_index = curr_ptr->next_hop_index;
                    SHR_BITSET(nh_bitmap, nh_index);

                    if (curr_ptr->flags & BCM_NIV_EGRESS_MULTICAST) {
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
                     * physical port this NIV egress object resides on.
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
        } else { /* BCM_NIV_PORT_MATCH_NONE is not set */
            if (NIV_PORT_INFO(unit, i)->match_vlan) {
                match_vlan_array[i] = NIV_PORT_INFO(unit, i)->match_vlan;
            }
        }
        if (NIV_PORT_INFO(unit, i)->flags & BCM_NIV_PORT_MATCH_DO_NOT_ADD) {
            SHR_BITSET(match_do_not_add_vp_bitmap, i);
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

    /* Store in scache the bitmap of NIV VPs that were created without
     * match criteria.
     */
    sal_memcpy(*scache_ptr, match_none_vp_bitmap, SHR_BITALLOCSIZE(num_vp));
    *scache_ptr += SHR_BITALLOCSIZE(num_vp);

    /* Store in scache the bitmap of next hops associated with
     * NIV egress objects.
     */
    sal_memcpy(*scache_ptr, nh_bitmap, SHR_BITALLOCSIZE(num_nh));
    *scache_ptr += SHR_BITALLOCSIZE(num_nh);

    /* Store in scache the physical port bitmap of the next hops 
     * associated with NIV egress objects.
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
     * Store in scache the bitmap of NIV VPs which are added
     * with BCM_NIV_PORT_MATCH_DO_NOT_ADD.
     */
    sal_memcpy(*scache_ptr, match_do_not_add_vp_bitmap,
               SHR_BITALLOCSIZE(num_vp));
    *scache_ptr += SHR_BITALLOCSIZE(num_vp);

    /*
     * Store in scache the bitmap of next hops which are associated
     * with BCM_NIV_EGRESS_MULTICAST.
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
    if (NULL != match_do_not_add_vp_bitmap) {
        sal_free(match_do_not_add_vp_bitmap);
    }
    if (NULL != mc_nh_bitmap) {
        sal_free(mc_nh_bitmap);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_trident_niv_sd_tag_recover
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
_bcm_trident_niv_sd_tag_recover(int unit, egr_l3_next_hop_entry_t *egr_nh,
        _bcm_trident_niv_sd_tag_t *sd_tag)
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

    sal_memset(sd_tag, 0, sizeof(_bcm_trident_niv_sd_tag_t));

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

    /* Recover BCM_NIV_EGRESS_SERVICE_xxx flags */
    if (sd_tag_add) {
        sd_tag->flags |= BCM_NIV_EGRESS_SERVICE_VLAN_ADD;
    }
    if (sd_tag_delete) {
        sd_tag->flags |= BCM_NIV_EGRESS_SERVICE_VLAN_DELETE;
    }
    if (sd_tag_replace_vlan) {
        sd_tag->flags |= BCM_NIV_EGRESS_SERVICE_VLAN_REPLACE;
    }
    if (sd_tag_replace_pri) {
        sd_tag->flags |= BCM_NIV_EGRESS_SERVICE_PRI_REPLACE;
    }
    if (sd_tag_replace_tpid) {
        sd_tag->flags |= BCM_NIV_EGRESS_SERVICE_TPID_REPLACE;
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
 *      bcm_trident_niv_reinit
 * Purpose:
 *      Warm boot recovery for the NIV software module
 * Parameters:
 *      unit - Device Number
 *      defaul_ver - Warm boot default version
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_trident_niv_reinit(int unit)
{
    int rv = BCM_E_NONE;
    int num_vp, num_nh, num_ports;
    SHR_BITDCL *match_none_vp_bitmap = NULL;
    SHR_BITDCL *match_do_not_add_vp_bitmap = NULL;
    SHR_BITDCL *niv_vp_bitmap = NULL;
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
    uint32 entry_type = 0, vp = 0, trunk_bit;
    uint32 vif = 0, p_bit = 0,l_bit = 0;
    _bcm_trident_niv_sd_tag_t niv_sd_tag;
    bcm_trunk_t tgid;
    bcm_module_t modid, mod_out;
    bcm_port_t port_num, port_out;
    int num_valid_ports;
    int j;
    _bcm_trident_niv_egress_t *curr_ptr = NULL;
    bcm_gport_t gport;
    uint8 *svp_buf = NULL;
    int tpid_enable;
    int  vp_mc, mc_flags;
    bcm_pbmp_t pbmp;
    _bcm_vp_info_t vp_info;
    int        old_num_vp = 0;
#if defined(BCM_SABER2_SUPPORT)
    uint32     source_vp_sb2_5626x = 0;
#endif /* BCM_SABER2_SUPPORT */

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeNiv;

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    num_nh = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    num_ports = soc_mem_field_length(unit, L3_IPMCm, L3_BITMAPf);

    /* Allocate a bitmap of NIV VPs that were created without match criteria */
    match_none_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp),
            "Bitmap of NIV VPs without match criteria");
    if (NULL == match_none_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(match_none_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

    /* Allocate a bitmap of NIV VPs */
    niv_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp), "Bitmap of NIV VPs");
    if (NULL == niv_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(niv_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

    /* Allocate a bitmap of next hops associated with NIV egress objects */
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

    /*
     * Allocate a bitmap of NIV VPs which are added
     * with BCM_NIV_PORT_MATCH_DO_NOT_ADD.
     */
    match_do_not_add_vp_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vp),
            "Bitmap of NIV VPs");
    if (NULL == match_do_not_add_vp_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(match_do_not_add_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));

    /* Allocate a bitmap of next hops associated with multicast NIV egress objects */
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
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_NIV, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
            0, &scache_ptr, BCM_WB_DEFAULT_VERSION, &recovered_ver);

    if (BCM_E_NOT_FOUND == rv) {

        if ((stable_size == 0) || SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
            rv = BCM_E_NONE;
        } else {
            /* Get scache size required for default version */
            rv = bcm_trident_niv_required_scache_size_get(unit,
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
        /* Recover the bitmap of NIV VPs that were created without
         * match criteria.
         */
        sal_memcpy(match_none_vp_bitmap, scache_ptr, SHR_BITALLOCSIZE(num_vp));
        if (old_num_vp) {
            scache_ptr += SHR_BITALLOCSIZE(num_vp * 2);
        } else {
            scache_ptr += SHR_BITALLOCSIZE(num_vp);
        }

        /* Recover the bitmap of next hops associated with
         * NIV egress objects.
         */
        sal_memcpy(nh_bitmap, scache_ptr, SHR_BITALLOCSIZE(num_nh));
        scache_ptr += SHR_BITALLOCSIZE(num_nh);

       /* Recover the physical port bitmap of the next hops associated
        * with NIV egress objects.
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
        
        /* Recover the array of match_vlan */
        if (recovered_ver >= BCM_WB_VERSION_1_1) {
            sal_memcpy(match_vlan_array, scache_ptr, sizeof(bcm_vlan_t) * num_vp);
            if (old_num_vp) {
                scache_ptr += (sizeof(bcm_vlan_t) * num_vp * 2);
            } else {
                scache_ptr += (sizeof(bcm_vlan_t) * num_vp);
            }
        } else {
            if (old_num_vp) {
                additional_scache_size += (sizeof(bcm_vlan_t) * num_vp * 2);
            } else {
                additional_scache_size += (sizeof(bcm_vlan_t) * num_vp);
            }
        }

        /*
         * Recover the bitmap of NIV VPs which are added
         * with BCM_NIV_PORT_MATCH_DO_NOT_ADD.
         */
        if (recovered_ver >= BCM_WB_VERSION_1_2) {
            sal_memcpy(match_do_not_add_vp_bitmap, 
                       scache_ptr, SHR_BITALLOCSIZE(num_vp));
            if (old_num_vp) {
                scache_ptr += SHR_BITALLOCSIZE(num_vp * 2);
            } else {
                scache_ptr += SHR_BITALLOCSIZE(num_vp);
            }
        } else {
            if (old_num_vp) {
                additional_scache_size += SHR_BITALLOCSIZE(num_vp * 2);
            } else {
                additional_scache_size += SHR_BITALLOCSIZE(num_vp);
            }
        }

      /* Recover the bitmap of next hops associated with
       * multicast NIV egress objects.
       */
       if (recovered_ver >= BCM_WB_VERSION_1_3) {
            sal_memcpy(mc_nh_bitmap, scache_ptr, SHR_BITALLOCSIZE(num_nh));
            scache_ptr += SHR_BITALLOCSIZE(num_nh);
        } else {
            additional_scache_size += SHR_BITALLOCSIZE(num_nh);
        }

        if (additional_scache_size > 0) {
            rv = soc_scache_realloc(unit, scache_handle,additional_scache_size);
            if(BCM_FAILURE(rv)) {
               goto cleanup;
            }
        }
    }

    /*
     * Recover the BCM_NIV_PORT_MATCH_NONE from match_none_vp_bitmap,
     * and recover the BCM_NIV_PORT_MATCH_DO_NOT_ADD from
     * match_do_not_add_vp_bitmap
     */
    for (i = 0; i < num_vp; i++) {
        if (SHR_BITGET(match_none_vp_bitmap, i)) {
            NIV_PORT_INFO(unit, i)->flags |= BCM_NIV_PORT_MATCH_NONE;
        }
        if (SHR_BITGET(match_do_not_add_vp_bitmap, i)) {
            NIV_PORT_INFO(unit, i)->flags |= BCM_NIV_PORT_MATCH_DO_NOT_ADD;
        }
    }

    /* Copy match_none_vp_bitmap to niv_vp_bitmap */
    sal_memcpy(niv_vp_bitmap, match_none_vp_bitmap, SHR_BITALLOCSIZE(num_vp));

    /* Recover NIV ports and NIV egress objects from next hop tables */

    ing_nh_buf = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, ING_L3_NEXT_HOPm),
            "Ing Next Hop buffer");
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
            SOC_MEM_TABLE_BYTES(unit, EGR_L3_NEXT_HOPm),
            "Egr Next Hop buffer");
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

    for (i = index_min; i <= index_max; i++) {
        egr_nh_entry = soc_mem_table_idx_to_pointer
            (unit, EGR_L3_NEXT_HOPm, egr_l3_next_hop_entry_t *, 
             egr_nh_buf, i);

        /* Check entry type */
        entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        _NIV_ENTRY_TYPEf(unit, EGR_L3_NEXT_HOPm));
        if ((entry_type != 2) && (entry_type != 7)) {
            /* Not NIV virtual port entry type */
            continue;
        }

        if (entry_type == 7) {
            if (!soc_feature(unit, soc_feature_virtual_port_routing)) {
                continue;
            }
        }

        /* Check that VN-tag action is ADD */
        if (entry_type == 2) {            
            if (VNTAG_ADD != soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        SD_TAG__VNTAG_ACTIONSf)) {
                continue;
            }
        } else if (entry_type == 7) {
            if (VNTAG_ADD != soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        L3MC__VNTAG_ACTIONSf)) {
                continue;
            }
        }
        
        if (entry_type == 2) {            
            /* Recover VP */
            vp = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    SD_TAG__DVPf);
        } else if (entry_type == 7) {
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
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
                /* VP bitmap is recovered by virtual_init */
                continue;
            }
        }

        /* Add VP to NIV VP bitmap */
        SHR_BITSET(niv_vp_bitmap, vp);

        /* Read virtual interface ID, multicast flag, and SD-tag or L3mc*/
        if (entry_type == 2) {
            vif = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    SD_TAG__VNTAG_DST_VIFf);
            p_bit = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    SD_TAG__VNTAG_Pf); 
            rv = _bcm_trident_niv_sd_tag_recover(unit, egr_nh_entry, &niv_sd_tag);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }            
        } else if (entry_type == 7) {
            /* L3MC*/
            vif = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    L3MC__VNTAG_DST_VIFf);
            p_bit = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    L3MC__VNTAG_Pf);
            l_bit = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    L3MC__VNTAG_FORCE_Lf); 
        }

        if (!(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MATCH_NONE)) {

            if (entry_type == 2) {
                /* Recover multicast flag */
                if (p_bit) {
                    NIV_PORT_INFO(unit, vp)->flags |= BCM_NIV_PORT_MULTICAST;
                }

                /* Recover physical trunk or port from ingress next hop table */
                ing_nh_entry = soc_mem_table_idx_to_pointer
                    (unit, ING_L3_NEXT_HOPm, ing_l3_next_hop_entry_t *, 
                     ing_nh_buf, i);
                if (soc_feature(unit, soc_feature_generic_dest)) {
                    uint32 dt, dv;
                    dv = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm,
                            ing_nh_entry, DESTINATIONf, &dt);
                    if (dt == SOC_MEM_FIF_DEST_LAG) {
                        /* Trunk group */
                        tgid = dv & SOC_MEM_FIF_DGPP_TGID_MASK;
                        BCM_GPORT_TRUNK_SET(NIV_PORT_INFO(unit, vp)->port, tgid);
                    } else {
                        port_num = dv & SOC_MEM_FIF_DGPP_PORT_MASK;
                        modid = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                                 SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
                        rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                modid, port_num, &mod_out, &port_out);
                        if (BCM_FAILURE(rv)) {
                            goto cleanup;
                        }
                        BCM_GPORT_MODPORT_SET(NIV_PORT_INFO(unit, vp)->port,
                                              mod_out, port_out);
                    }
                } else {
                    trunk_bit = soc_ING_L3_NEXT_HOPm_field32_get(unit,
                                    ing_nh_entry, Tf);
                    if (trunk_bit) {
                        tgid = soc_ING_L3_NEXT_HOPm_field32_get(unit,
                                    ing_nh_entry, TGIDf);
                        BCM_GPORT_TRUNK_SET(NIV_PORT_INFO(unit, vp)->port, tgid);
                    } else {
                        modid = soc_ING_L3_NEXT_HOPm_field32_get(unit,
                                    ing_nh_entry, MODULE_IDf);
                        port_num = soc_ING_L3_NEXT_HOPm_field32_get(unit,
                                    ing_nh_entry, PORT_NUMf);
                        rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                modid, port_num, &mod_out, &port_out);
                        if (BCM_FAILURE(rv)) {
                            goto cleanup;
                        }
                        BCM_GPORT_MODPORT_SET(NIV_PORT_INFO(unit, vp)->port,
                                              mod_out, port_out);
                    }
                }

                /* Recover virtual interface ID */
                NIV_PORT_INFO(unit, vp)->virtual_interface_id = vif;

                /* The match_vlan can be recovered from either match_vlan_array
                 * or EGR_L3_NEXT_HOP.SD_TAG_VID field. The latter approach
                 * is taken since older SDKs stored match_vlan in EGR_L3_NEXT_HOP
                 * entry, not in scache.
                 */
                NIV_PORT_INFO(unit, vp)->match_vlan =
                    soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                            SD_TAG__SD_TAG_VIDf);

                if (stable_size == 0) {
                    /* In the Port module, a port's VP count is not recovered in 
                     * level 1 Warm Boot.
                     */
                    rv = _bcm_trident_niv_port_cnt_update(unit,
                            NIV_PORT_INFO(unit, vp)->port, vp, TRUE, TRUE);
                    if (BCM_FAILURE(rv)) {
                        goto cleanup;
                    }
                }
            }
        } else { /* The NIV port was created without match criteria */
            vp_mc = 0;
            BCM_PBMP_CLEAR(pbmp);
            if (entry_type == 2) {
                num_valid_ports = 0;
                for (j = 0; j < num_ports; j++) {
                    if ((pbmp_ptr_array[i] != NULL) ?
                        (SHR_BITGET(pbmp_ptr_array[i], j)) : 0) {
                        /* Insert a new NIV egress object into VP's egress list */
                        curr_ptr = sal_alloc(sizeof(_bcm_trident_niv_egress_t),
                                "NIV egress object");
                        if (NULL == curr_ptr) {
                            rv = BCM_E_MEMORY;
                            goto cleanup;
                        }
                        sal_memset(curr_ptr, 0, sizeof(_bcm_trident_niv_egress_t));

                        if (p_bit) {
                            curr_ptr->flags |= BCM_NIV_EGRESS_MULTICAST;
                        } else if (SHR_BITGET(mc_nh_bitmap, i)) {
                            curr_ptr->flags |= BCM_NIV_EGRESS_MULTICAST;
                            /*
                             * The P-bit could have been cleared when egress objects
                             * are added with BCM_NIV_EGRESS_P_BIT_CLEAR flag.
                             */
                            curr_ptr->flags |= BCM_NIV_EGRESS_P_BIT_CLEAR;
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
                        curr_ptr->virtual_interface_id = vif;
                        if (recovered_ver >= BCM_WB_VERSION_1_1) {
                            curr_ptr->match_vlan = match_vlan_array[vp];
                        } else {
                            curr_ptr->match_vlan =
                                soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                    egr_nh_entry, SD_TAG__SD_TAG_VIDf);
                        }                        
                        curr_ptr->flags |= niv_sd_tag.flags;
                        curr_ptr->service_tpid = niv_sd_tag.service_tpid;
                        curr_ptr->service_vlan = niv_sd_tag.service_vlan;
                        curr_ptr->service_pri = niv_sd_tag.service_pri;
                        curr_ptr->service_cfi = niv_sd_tag.service_cfi;
                        curr_ptr->service_qos_map_id = niv_sd_tag.service_qos_map_id;
                        curr_ptr->next_hop_index = i;
                        curr_ptr->next = NIV_PORT_INFO(unit, vp)->egress_list;
                        if (curr_ptr->flags & BCM_NIV_EGRESS_MULTICAST) {
                            vp_mc = 1;
                            rv = _bcm_trident_niv_egress_hash_recover(
                                     unit, vp, curr_ptr);
                            if (BCM_FAILURE(rv)) {
                                sal_free(curr_ptr);
                                goto cleanup;
                            }
                            BCM_PBMP_PORT_ADD(pbmp, j);
                        } else {
                            NIV_PORT_INFO(unit, vp)->egress_list = curr_ptr;
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
            } else if (entry_type == 7) {
                num_valid_ports = 0;
                vp_mc = 1;
                for (j = 0; j < num_ports; j++) {
                    if ((pbmp_ptr_array[i] != NULL) ?
                        (!SHR_BITGET(pbmp_ptr_array[i], j)) : 1) {
                        continue;                            
                    }
                    /* Insert a new NIV egress object into VP's egress list */
                    curr_ptr = sal_alloc(sizeof(_bcm_trident_niv_egress_t),
                            "NIV egress object");
                    if (NULL == curr_ptr) {
                        rv = BCM_E_MEMORY;
                        goto cleanup;
                    }
                    sal_memset(curr_ptr, 0, sizeof(_bcm_trident_niv_egress_t));

                    if (p_bit) {
                        curr_ptr->flags |= BCM_NIV_EGRESS_MULTICAST;
                    } else if (SHR_BITGET(mc_nh_bitmap, i)) {
                        curr_ptr->flags |= BCM_NIV_EGRESS_MULTICAST;
                        /*
                         * The P-bit could have been cleared when egress objects
                         * are added with BCM_NIV_EGRESS_P_BIT_CLEAR flag.
                         */
                        curr_ptr->flags |= BCM_NIV_EGRESS_P_BIT_CLEAR;
                    }

                    if (l_bit) {
                        curr_ptr->flags |= BCM_NIV_VNTAG_L_BIT_FORCE_1;
                    }
                    /*entry_type == 7, L3MC */
                    curr_ptr->flags |= BCM_NIV_EGRESS_L3;

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
                    curr_ptr->virtual_interface_id = vif;
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
                    rv = _bcm_trident_niv_egress_hash_recover(
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
                rv = _bcm_trident_niv_egress_pbmp_add(unit, vp, pbmp);
                if (BCM_FAILURE(rv)) {
                    sal_free(curr_ptr);
                    goto cleanup;
                }
            }
        }
    }


    /* Recover SD-tag TPID reference count from SOURCE_VP table */
    if (entry_type == 2) {
        soc_mem_t svpm = SOURCE_VPm;
        void *svp_entry;
        if (soc_mem_is_valid(unit, SVP_ATTRS_1m)) {
            svpm = SVP_ATTRS_1m;
        }
        svp_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, svpm),
                "SOURCE_VP buffer");
        if (NULL == svp_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        index_min = soc_mem_index_min(unit, svpm);
        index_max = soc_mem_index_max(unit, svpm);
        rv = soc_mem_read_range(unit, svpm, MEM_BLOCK_ANY,
                index_min, index_max, svp_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
        for (i = index_min; i <= index_max; i++) {
            if (svpm == SOURCE_VPm) {
                svp_entry = soc_mem_table_idx_to_pointer
                    (unit, svpm, source_vp_entry_t *, svp_buf, i);
            } else {
                svp_entry = soc_mem_table_idx_to_pointer
                    (unit, svpm, svp_attrs_1_entry_t *, svp_buf, i);
            }
            (void) _bcm_trident_niv_tpid_enb_get(unit, svp_entry, i,
                                                 &tpid_enable);
            if (tpid_enable) {
                for (j = 0; j < soc_mem_field_length(unit, svpm, TPID_ENABLEf);
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
    if (match_do_not_add_vp_bitmap) {
        sal_free(match_do_not_add_vp_bitmap);
    }
    if (niv_vp_bitmap) {
        sal_free(niv_vp_bitmap);
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
 *      _bcm_trident_niv_free_resources
 * Purpose:
 *      Free all allocated tables and memory
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_trident_niv_free_resources(int unit)
{
    int i;
    _bcm_trident_niv_egress_t *curr_ptr;
    _bcm_trident_niv_egress_t *next_ptr;

    if (NIV_INFO(unit)->port_info) {
        for (i = 0; i < soc_mem_index_count(unit, SOURCE_VPm); i++) {
            curr_ptr = NIV_PORT_INFO(unit, i)->egress_list;
            while (NULL != curr_ptr) {
                next_ptr = curr_ptr->next;
                sal_free(curr_ptr);
                curr_ptr = next_ptr;
            }
            NIV_PORT_INFO(unit, i)->egress_list = NULL;
        }
        sal_free(NIV_INFO(unit)->port_info);
        NIV_INFO(unit)->port_info = NULL;
    }
}

/*
 * Function:
 *      bcm_trident_niv_init
 * Purpose:
 *      Initialize the NIV module.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_init(int unit)
{
    int num_vp;
    int rv = BCM_E_NONE;

    sal_memset(NIV_INFO(unit), 0,
            sizeof(_bcm_trident_niv_bookkeeping_t));

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    if (NULL == NIV_INFO(unit)->port_info) {
        NIV_INFO(unit)->port_info =
            sal_alloc(sizeof(_bcm_trident_niv_port_info_t) * num_vp, "niv_port_info");
        if (NULL == NIV_INFO(unit)->port_info) {
            _bcm_trident_niv_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(NIV_INFO(unit)->port_info, 0,
            sizeof(_bcm_trident_niv_port_info_t) * num_vp);

    /* When creating a NIV virtual port without any match criteria,
     * the ING_DVP_TABLE.NEXT_HOP_INDEX is configured with an invalid
     * next hop index, which is assigned to be the black hole next hop index.
     */
    NIV_INFO(unit)->invalid_next_hop_index = BCM_XGS3_L3_BLACK_HOLE_NH_IDX(unit);

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_td3_rx_etype_niv[unit] = 0;
    }
#endif

    return rv;
}

/*
 * Function:
 *      bcm_trident_niv_detach
 * Purpose:
 *      Detach the NIV module.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_cleanup(int unit)
{
    _bcm_trident_niv_free_resources(unit);

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_td3_rx_etype_niv[unit] = 0;
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trident_niv_nh_info_set
 * Purpose:
 *      Configure next hop tables.
 * Parameters:
 *      unit       - (IN) SOC unit number. 
 *      nh_index   - (IN) Next hop index. 
 *      multicast  - (IN) NIV multicast flag.
 *      port       - (IN) Physical GPORT ID. 
 *      virtual_interface_id - (IN) Virtual interface ID. 
 *      match_vlan - (IN) Outer VLAN ID to match. 
 *      sd_tag     - (IN) SD-tag attributes. 
 *      vp         - (IN) Virtual port number. 
 *      allow_loop   - (IN) NIV allow loop flag
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_trident_niv_nh_info_set(int unit, int nh_index, int multicast,
        bcm_gport_t port, uint16 vintf_id, bcm_vlan_t match_vlan,
        _bcm_trident_niv_sd_tag_t *sd_tag, int vp, int allow_loop)
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
    int sd_tag_add = 0;
    int sd_tag_delete = 0;
    int sd_tag_replace_vlan = 0;
    int sd_tag_replace_pri = 0;
    int sd_tag_replace_tpid = 0;
    int tpid_index;
    int mapping_ptr;
    soc_mem_t mem = EGR_L3_NEXT_HOPm;

    if ((nh_index > soc_mem_index_max(unit, mem)) ||
        (nh_index < soc_mem_index_min(unit, mem))) {
        return BCM_E_PARAM;
    }

    /* Set up EGR_L3_NEXT_HOP entry */
    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    soc_mem_field32_set(unit, mem, &egr_nh,
            _NIV_ENTRY_TYPEf(unit, mem), 2); /* SD-tag entry type */
    soc_mem_field32_set(unit, mem, &egr_nh, SD_TAG__DVPf, vp);
    /* PPD2 header */
    soc_mem_field32_set(unit, mem, &egr_nh, SD_TAG__HG_HDR_SELf, 1);
    soc_mem_field32_set(unit, mem, &egr_nh, SD_TAG__VNTAG_DST_VIFf, vintf_id);
    soc_mem_field32_set(unit, mem, &egr_nh, SD_TAG__VNTAG_FORCE_Lf, allow_loop);
    soc_mem_field32_set(unit, mem, &egr_nh, SD_TAG__VNTAG_Pf, multicast);
    soc_mem_field32_set(unit, mem, &egr_nh, SD_TAG__VNTAG_ACTIONSf, 1);

    /* Set SD-tag related fields */
    if (NULL != sd_tag) {
        sd_tag_add = sd_tag->flags & BCM_NIV_EGRESS_SERVICE_VLAN_ADD;
        sd_tag_delete = sd_tag->flags & BCM_NIV_EGRESS_SERVICE_VLAN_DELETE;
        sd_tag_replace_vlan = sd_tag->flags & BCM_NIV_EGRESS_SERVICE_VLAN_REPLACE;
        sd_tag_replace_pri = sd_tag->flags & BCM_NIV_EGRESS_SERVICE_PRI_REPLACE;
        sd_tag_replace_tpid = sd_tag->flags & BCM_NIV_EGRESS_SERVICE_TPID_REPLACE;

        /* Set SD-tag action for packets without an SD-tag */
        if (sd_tag_add) {
            soc_mem_field32_set(unit, mem, &egr_nh,
                    SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf, 1);
        }

        /* Set SD-tag action for packets with an SD-tag */
        if (sd_tag_delete) {
            if (sd_tag_replace_vlan || sd_tag_replace_pri || sd_tag_replace_tpid) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, mem, &egr_nh,
                    SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 3); /* DELETE SD-TAG */
        } else {
            if (!sd_tag_replace_vlan && !sd_tag_replace_pri && !sd_tag_replace_tpid) {
                soc_mem_field32_set(unit, mem, &egr_nh,
                        SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 0); /* NOOP */
            } else if (sd_tag_replace_vlan && !sd_tag_replace_pri && sd_tag_replace_tpid) {
                soc_mem_field32_set(unit, mem, &egr_nh,
                        SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 1); /* REPLACE_VID_TPID */
            } else if (sd_tag_replace_vlan && !sd_tag_replace_pri && !sd_tag_replace_tpid) {
                soc_mem_field32_set(unit, mem, &egr_nh,
                        SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 2); /* REPLACE_VID_ONLY */
            } else if (sd_tag_replace_vlan && sd_tag_replace_pri && sd_tag_replace_tpid) {
                soc_mem_field32_set(unit, mem, &egr_nh,
                        SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 4); /* REPLACE_VID_PRI_TPID */
            } else if (sd_tag_replace_vlan && sd_tag_replace_pri && !sd_tag_replace_tpid) {
                soc_mem_field32_set(unit, mem, &egr_nh,
                        SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 5); /* REPLACE_VID_PRI_ONLY */
            } else if (!sd_tag_replace_vlan && sd_tag_replace_pri && !sd_tag_replace_tpid) {
                soc_mem_field32_set(unit, mem, &egr_nh,
                        SD_TAG__SD_TAG_ACTION_IF_PRESENTf, 6); /* REPLACE_PRI_ONLY */
            } else if (!sd_tag_replace_vlan && !sd_tag_replace_pri && sd_tag_replace_tpid) {
                if (SOC_IS_TRIDENT(unit)) {
                    /* REPLACE_TPID_ONLY action is not available on Trident.
                     * It's available on Triumph3 and Trident2.
                     */
                    return BCM_E_PARAM;
                }
                soc_mem_field32_set(unit, mem, &egr_nh,
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
                soc_mem_field32_set(unit, mem, &egr_nh,
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
                soc_mem_field32_set(unit, mem, &egr_nh, SD_TAG__SD_TAG_VIDf,
                                    sd_tag->service_vlan);
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
                soc_mem_field32_set(unit, mem, &egr_nh,
                        SD_TAG__SD_TAG_DOT1P_PRI_SELECTf, 1);
                BCM_IF_ERROR_RETURN(_bcm_tr2_qos_id2idx(unit,
                            sd_tag->service_qos_map_id, &mapping_ptr));
                soc_mem_field32_set(unit, mem, &egr_nh,
                        SD_TAG__SD_TAG_DOT1P_MAPPING_PTRf, mapping_ptr);
            } else {
                return BCM_E_PARAM;
            }
        } else {
            if (sd_tag_add || sd_tag_replace_pri) {
                soc_mem_field32_set(unit, mem, &egr_nh,
                        SD_TAG__SD_TAG_DOT1P_PRI_SELECTf, 0);
                soc_mem_field32_set(unit, mem, &egr_nh,
                        SD_TAG__NEW_PRIf, sd_tag->service_pri);
                soc_mem_field32_set(unit, mem, &egr_nh,
                        SD_TAG__NEW_CFIf, sd_tag->service_cfi);
            }
        }
    }

    /* Prior to the introduction of NIV SD-tag API enhancements,
     * the match_vlan was stored in EGR_L3_NEXT_HOP.SD_TAG_VID field
     * for warm boot recovery. With the NIV SD-tag API enhancements,
     * the match_vlan is now stored in scache. But for warm boot
     * downgrade, the match_vlan still needs to be stored in SD_TAG_VID field
     * when there is no SD-tag action.
     */ 
    if (!sd_tag_add && !sd_tag_delete && !sd_tag_replace_vlan &&
            !sd_tag_replace_pri && !sd_tag_replace_tpid) {
        soc_mem_field32_set(unit, mem, &egr_nh, SD_TAG__SD_TAG_VIDf, match_vlan);
    }

    /* Resolve gport */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, port, &mod_out, &port_out,
                               &trunk_id, &id));

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                            SD_TAG__HG_MC_DST_PORT_NUMf)) {
        if ((!BCM_GPORT_IS_TRUNK(port)) &&
            (_bcm_xgs5_subport_coe_mod_port_local(unit, mod_out, port_out))) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                SD_TAG__HG_MC_DST_PORT_NUMf, port_out);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                SD_TAG__HG_MC_DST_MODIDf, mod_out);
        }
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    /* Write EGR_L3_NEXT_HOP entry */
    SOC_IF_ERROR_RETURN(
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, nh_index, &egr_nh));

    mem = ING_L3_NEXT_HOPm;
    /* Resolve gport */
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &mod_out, 
                &port_out, &trunk_id, &id));
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

    /* Write ING_L3_NEXT_HOP entry */
    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));
    if (soc_feature(unit, soc_feature_generic_dest)) {
        if (ing_nh_trunk == -1) {
            soc_mem_field32_dest_set(unit, mem, &ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                ing_nh_module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | ing_nh_port);
        } else {
            soc_mem_field32_dest_set(unit, mem, &ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_LAG, ing_nh_trunk);
        }
    } else {
        if (ing_nh_trunk == -1) {
            soc_mem_field32_set(unit, mem, &ing_nh, PORT_NUMf, ing_nh_port);
            soc_mem_field32_set(unit, mem, &ing_nh, MODULE_IDf, ing_nh_module);
        } else {
            soc_mem_field32_set(unit, mem, &ing_nh, Tf, 1);
            soc_mem_field32_set(unit, mem, &ing_nh, TGIDf, ing_nh_trunk);
        }
    }
    /* L2 DVP */
    soc_mem_field32_set(unit, mem, &ing_nh, _NIV_ENTRY_TYPEf(unit, mem), 0x2);

#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_mem_field_valid(unit, mem, DVP_ATTRIBUTE_1_INDEXf)) {
        uint32 mtu_profile_index;

        BCM_IF_ERROR_RETURN
            (_bcm_tr3_mtu_profile_index_get(unit, 0x3fff, &mtu_profile_index));
        soc_mem_field32_set(unit, mem, &ing_nh,
                DVP_ATTRIBUTE_1_INDEXf, mtu_profile_index);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
    if (soc_mem_field_valid(unit, mem, MTU_SIZEf)) {
            soc_mem_field32_set(unit, mem, &ing_nh,
                MTU_SIZEf, 0x3fff);    
        }
    }

    SOC_IF_ERROR_RETURN(
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, nh_index, &ing_nh));

    mem = INITIAL_ING_L3_NEXT_HOPm;
    /* Write INITIAL_ING_L3_NEXT_HOP entry */
    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    if (soc_feature(unit, soc_feature_generic_dest)) {
        if (ing_nh_trunk == -1) {
            soc_mem_field32_dest_set(unit, mem, &initial_ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                ing_nh_module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | ing_nh_port);
        } else {
            soc_mem_field32_dest_set(unit, mem, &initial_ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_LAG, ing_nh_trunk);
        }
    } else {
        if (ing_nh_trunk == -1) {
            soc_mem_field32_set(unit, mem,
                    &initial_ing_nh, PORT_NUMf, ing_nh_port);
            soc_mem_field32_set(unit, mem,
                    &initial_ing_nh, MODULE_IDf, ing_nh_module);
        } else {
            soc_mem_field32_set(unit, mem, &initial_ing_nh, Tf, 1);
            soc_mem_field32_set(unit, mem, &initial_ing_nh, TGIDf, ing_nh_trunk);
        }
    }
    SOC_IF_ERROR_RETURN(
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, nh_index, &initial_ing_nh));

    return BCM_E_NONE;
}

#ifdef BCM_TRIDENT2_SUPPORT
/*
 * Function:
 *      _bcm_trident_niv_l3mc_nh_info_set
 * Purpose:
 *      Configure next hop tables for L3MC.
 * Parameters:
 *      unit       - (IN) SOC unit number. 
 *      nh_index   - (IN) Next hop index. 
 *      multicast  - (IN) NIV multicast flag.
 *      port       - (IN) Physical GPORT ID. 
 *      virtual_interface_id - (IN) Virtual interface ID. 
 *      match_vlan - (IN) Outer VLAN ID to match. 
 *      sd_tag     - (IN) SD-tag attributes. 
 *      vp         - (IN) Virtual port number. 
 *      allow_loop   - (IN) NIV allow loop flag
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_trident_niv_l3mc_nh_info_set(int unit, int nh_index, uint32 flags, 
        uint32 multicast_flags, bcm_gport_t port, bcm_if_t intf, 
        uint16 vintf_id, int vp, int allow_loop)
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

    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));

    /* L3MC view */
    soc_mem_field32_set(unit, mem, &egr_nh, _NIV_ENTRY_TYPEf(unit, mem), 7);
    soc_mem_field32_set(unit, mem, &egr_nh, L3MC__INTF_NUMf, intf);

    for (i = 0; i < COUNTOF(flag_array); i++) {
        if (multicast_flags & flag_array[i]) {
            flag_set = 1;
        } else {
            flag_set = 0;
        }
        if (soc_mem_field_valid(unit, mem, field_array[i])) {
            soc_mem_field32_set(unit, mem, &egr_nh,
                    field_array[i], flag_set);
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
    /* Add or replace VNTAG */
    soc_mem_field32_set(unit, mem, &egr_nh, L3MC__VNTAG_ACTIONSf, 1);
    soc_mem_field32_set(unit, mem, &egr_nh, L3MC__VNTAG_DST_VIFf, vintf_id);
    soc_mem_field32_set(unit, mem, &egr_nh, L3MC__VNTAG_FORCE_Lf, allow_loop);
    soc_mem_field32_set(unit, mem, &egr_nh, L3MC__VNTAG_Pf,
                        (flags & BCM_NIV_EGRESS_P_BIT_CLEAR) ? 0 : 1);
    /* PPD2 header */
    soc_mem_field32_set(unit, mem, &egr_nh, L3MC__HG_HDR_SELf, 1);

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
    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                            L3MC__HG_MC_DST_PORT_NUMf)) {
        if ((!BCM_GPORT_IS_TRUNK(port)) &&
            (_bcm_xgs5_subport_coe_mod_port_local(unit, mod_out, port_out))) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__HG_MC_DST_PORT_NUMf, port_out);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__HG_MC_DST_MODIDf, mod_out);
        }
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    /* Write EGR_L3_NEXT_HOP entry */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem,
                MEM_BLOCK_ALL, nh_index, &egr_nh));

    mem = ING_L3_NEXT_HOPm;
    /* Resolve gport */
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &mod_out, 
                &port_out, &trunk_id, &id));
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
    if (soc_feature(unit, soc_feature_generic_dest)) {
        if (ing_nh_trunk == -1) {
            soc_mem_field32_dest_set(unit, mem, &ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                ing_nh_module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | ing_nh_port);
        } else {
            soc_mem_field32_dest_set(unit, mem, &ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_LAG, ing_nh_trunk);
        }
    } else {
        if (ing_nh_trunk == -1) {
            soc_mem_field32_set(unit, mem, &ing_nh, PORT_NUMf, ing_nh_port);
            soc_mem_field32_set(unit, mem, &ing_nh, MODULE_IDf, ing_nh_module);
        } else {
            soc_mem_field32_set(unit, mem, &ing_nh, Tf, 1);
            soc_mem_field32_set(unit, mem, &ing_nh, TGIDf, ing_nh_trunk);
        }
    }

    SOC_IF_ERROR_RETURN(
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, nh_index, &ing_nh));
    
    mem = INITIAL_ING_L3_NEXT_HOPm;
    /* Write INITIAL_ING_L3_NEXT_HOP entry */
    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    if (soc_feature(unit, soc_feature_generic_dest)) {
        if (ing_nh_trunk == -1) {
            soc_mem_field32_dest_set(unit, mem, &initial_ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                ing_nh_module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | ing_nh_port);
        } else {
            soc_mem_field32_dest_set(unit, mem, &initial_ing_nh,
                DESTINATIONf, SOC_MEM_FIF_DEST_LAG, ing_nh_trunk);
        }
    } else {
        if (ing_nh_trunk == -1) {
            soc_mem_field32_set(unit, mem,
                    &initial_ing_nh, PORT_NUMf, ing_nh_port);
            soc_mem_field32_set(unit, mem,
                    &initial_ing_nh, MODULE_IDf, ing_nh_module);
        } else {
            soc_mem_field32_set(unit, mem, &initial_ing_nh, Tf, 1);
            soc_mem_field32_set(unit, mem, &initial_ing_nh, TGIDf, ing_nh_trunk);
        }
    }
    SOC_IF_ERROR_RETURN(
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, nh_index, &initial_ing_nh));

    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      _bcm_trident_niv_nh_info_delete
 * Purpose:
 *      Decrement next hop's reference count. If count reaches zero, free the
 *      next hop index and clear next hop table entries.
 * Parameters:
 *      unit     - (IN) SOC unit number. 
 *      nh_index - (IN) Next hop index. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_trident_niv_nh_info_delete(int unit, int nh_index)
{
    egr_l3_next_hop_entry_t egr_nh;
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

    /* Decrement the next-hop's reference count. If count reaches zero,
     * free the next hop index and clear ING_L3_NEXT_HOP,
     * INITIAL_ING_L3_NEXT_HOP, and EGR_L3_NEXT_HOP entries.
     */
    BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0, nh_index));

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

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trident_niv_match_info_get
 * Purpose:
 *      Get match criteria for NIV VP.
 * Parameters:
 *      unit                 - (IN) SOC unit number.
 *      vp                   - (IN) Virtual port number.
 *      port                 - (OUT) Physical GPORT ID. 
 *      match_vlan           - (OUT) Outer VLAN ID to match. 
 *      virtual_interface_id - (OUT) Virtual Interface ID. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC void
_bcm_trident_niv_match_info_get(
    int unit, int vp, bcm_gport_t *port,
    bcm_vlan_t *match_vlan, uint16 *vintf_id)
{
    if ((NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MATCH_NONE) &&
        (NIV_PORT_INFO(unit, vp)->egress_list != NULL) &&
        !(NIV_PORT_INFO(unit, vp)->egress_list->flags & BCM_NIV_EGRESS_MULTICAST)) {
        if (port) {
            *port = NIV_PORT_INFO(unit, vp)->egress_list->port;
        }
        
        if (match_vlan) {
            *match_vlan = NIV_PORT_INFO(unit, vp)->egress_list->match_vlan;
        }

        if (vintf_id) {
            *vintf_id =
                NIV_PORT_INFO(unit, vp)->egress_list->virtual_interface_id;
        }
    } else {
        if (port) {
            *port = NIV_PORT_INFO(unit, vp)->port;
        }

        if (match_vlan) {
            *match_vlan = NIV_PORT_INFO(unit, vp)->match_vlan;
        }

        if (vintf_id) {
            *vintf_id =
                NIV_PORT_INFO(unit, vp)->virtual_interface_id;
        }
    }

    return;
}

/*
 * Function:
 *      _bcm_trident_niv_match_add
 * Purpose:
 *      Add match criteria for NIV VP.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      port - (IN) Physical GPORT ID. 
 *      virtual_interface_id - (IN) Virtual Interface ID. 
 *      match_vlan - (IN) Outer VLAN ID to match. 
 *      vp - (IN) Virtual port number.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_trident_niv_match_add(int unit, bcm_gport_t port,
        uint16 virtual_interface_id, bcm_vlan_t match_vlan, int vp)
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
    int                   mod_local = FALSE;
    int                   num_local_ports;
    bcm_port_t            local_ports[SOC_MAX_NUM_PORTS];
    int                   local_member_count;
    bcm_port_t            local_member_array[SOC_MAX_NUM_PORTS];
    int                   port_key_type_vif_vlan, port_key_type_vif;
    int                   port_key_type_a, port_key_type_b;
    int                   use_port_a, use_port_b;
    int                   vt_enable;
    soc_mem_t             vxmem = VLAN_XLATEm;

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        vxmem = VLAN_XLATE_1_DOUBLEm;
    }
    sal_memset(vent, 0, sizeof(vent));
    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, vxmem, vent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, vxmem, vent, BASE_VALID_1f, 7);
    } else {
        soc_mem_field32_set(unit, vxmem, vent, VALIDf, 1);
    }

    if (BCM_VLAN_VALID(match_vlan)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_mem_field32_set(unit, vxmem, vent, VIF__VLANf,
                            match_vlan);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    }
    soc_mem_field32_set(unit, vxmem, vent, KEY_TYPEf, key_type);
    if (soc_mem_field_valid(unit, vxmem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, vxmem, vent, DATA_TYPEf, key_type);
    }

    if (virtual_interface_id >=
            (1 << soc_mem_field_length(unit, vxmem, VIF__SRC_VIFf))) {
        return BCM_E_PARAM;
    }
    soc_mem_field32_set(unit, vxmem, vent, VIF__SRC_VIFf,
            virtual_interface_id);

    if (soc_mem_field_valid(unit, vxmem, SOURCE_TYPEf)) {
        soc_mem_field32_set(unit, vxmem, vent, SOURCE_TYPEf, 1);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(port)) {
        soc_mem_field32_set(unit, vxmem, vent, VIF__Tf, 1);
        soc_mem_field32_set(unit, vxmem, vent, VIF__TGIDf, trunk_out);
    } else {
        soc_mem_field32_set(unit, vxmem, vent, VIF__MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, vxmem, vent, VIF__PORT_NUMf, port_out);
    }

    soc_mem_field32_set(unit, vxmem, vent, VIF__MPLS_ACTIONf, 0x1); /* SVP */
    soc_mem_field32_set(unit, vxmem, vent, VIF__SOURCE_VPf, vp);

    bcm_vlan_action_set_t_init(&action);
    if (BCM_VLAN_VALID(match_vlan)) {
        soc_mem_field32_set(unit, vxmem, vent, VIF__NEW_OVIDf, match_vlan);
        action.dt_outer = bcmVlanActionCopy;
        action.dt_inner = bcmVlanActionDelete;
        action.ot_outer = bcmVlanActionReplace;
    } else {
        action.ot_outer_prio = bcmVlanActionReplace;
        action.ut_outer = bcmVlanActionAdd;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_add(unit, &action, &profile_idx));
    soc_mem_field32_set(unit, vxmem, vent, VIF__TAG_ACTION_PROFILE_PTRf,
            profile_idx);
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_mem_field_valid(unit, vxmem, VIF__VLAN_ACTION_VALIDf)) {
        soc_mem_field32_set(unit, vxmem, vent, VIF__VLAN_ACTION_VALIDf, 1);
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    rv = soc_mem_insert_return_old(unit, vxmem, MEM_BLOCK_ALL, vent, old_vent);
    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_mem_field32_get(unit, vxmem, old_vent,
                VIF__TAG_ACTION_PROFILE_PTRf);
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    BCM_IF_ERROR_RETURN(rv);

    num_local_ports = 0;
    if (BCM_GPORT_IS_TRUNK(port)) {
        trunk_id = BCM_GPORT_TRUNK_GET(port);
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
            (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out,
                                    &trunk_id, &tmp_id)); 
        if ((trunk_id != -1) || (tmp_id != -1)) {
            return BCM_E_PARAM;
        }
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            _bcm_xgs5_subport_coe_mod_port_local(unit, mod_out, port_out)) {
            mod_local = TRUE;
            _BCM_SUBPORT_COE_PORT_ID_SET(port_out, mod_out, port_out);
            _BCM_SUBPORT_COE_PORT_TYPE_SET(port_out, _BCM_SUBPORT_COE_TYPE_SUBTAG);
        } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT && BCM_HGPROXY_COE_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, mod_out, &mod_local));
        }
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

        if (BCM_VLAN_VALID(match_vlan)) {
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
            if (port_key_type_b != port_key_type_vif) {
                BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                            local_ports[idx], _bcmPortVTKeyTypeSecond,
                            port_key_type_vif));
                BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                            local_ports[idx], _bcmPortVTKeyPortSecond, 1));
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

        if (BCM_VLAN_VALID(match_vlan)) {
            /* Enable egress VLAN translation */
            BCM_IF_ERROR_RETURN(
                _bcm_fb2_egr_vlan_control_field_set(
                    unit, local_ports[idx], VT_ENABLEf, 1));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trident_niv_match_delete
 * Purpose:
 *      Delete match criteria for NIV VP.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      port - (IN) Physical GPORT ID. 
 *      virtual_interface_id - (IN) Virtual Interface ID. 
 *      match_vlan - (IN) Outer VLAN ID to match. 
 *      old_vp - (OUT) Pointer to the old virtual port number of the entry.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_trident_niv_match_delete(int unit, bcm_gport_t port,
        uint16 virtual_interface_id, bcm_vlan_t match_vlan, int *old_vp)
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
    soc_mem_t             vxmem = VLAN_XLATEm;

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        vxmem = VLAN_XLATE_1_DOUBLEm;
    }
    sal_memset(vent, 0, sizeof(vent));

    if (BCM_VLAN_VALID(match_vlan)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_mem_field32_set(unit, vxmem, vent, VIF__VLANf, match_vlan);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    }
    soc_mem_field32_set(unit, vxmem, vent, KEY_TYPEf, key_type);
    if (soc_mem_field_valid(unit, vxmem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, vxmem, vent, DATA_TYPEf, key_type);
    }

    if (virtual_interface_id >=
              (1 << soc_mem_field_length(unit, vxmem, VIF__SRC_VIFf))) {
        return BCM_E_PARAM;
    }
    soc_mem_field32_set(unit, vxmem, vent, VIF__SRC_VIFf,
            virtual_interface_id);

    if (soc_mem_field_valid(unit, vxmem, SOURCE_TYPEf)) {
        soc_mem_field32_set(unit, vxmem, vent, SOURCE_TYPEf, 1);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_out,
                                &tmp_id));
    if (BCM_GPORT_IS_TRUNK(port)) {
        soc_mem_field32_set(unit, vxmem, vent, VIF__Tf, 1);
        soc_mem_field32_set(unit, vxmem, vent, VIF__TGIDf, trunk_out);
    } else {
        soc_mem_field32_set(unit, vxmem, vent, VIF__MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, vxmem, vent, VIF__PORT_NUMf, port_out);
    }

    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, vxmem, vent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, vxmem, vent, BASE_VALID_1f, 7);
    }

    rv = soc_mem_delete_return_old(unit, vxmem, MEM_BLOCK_ALL,
            vent, old_vent);
    if (BCM_SUCCESS(rv)) {
        int vld;

        if (soc_feature(unit, soc_feature_base_valid)) {
            vld = soc_mem_field32_get(unit, vxmem, old_vent,
                                      BASE_VALID_0f) == 3 &&
                  soc_mem_field32_get(unit, vxmem, old_vent,
                                      BASE_VALID_1f) == 7;
        } else {
            vld = soc_mem_field32_get(unit, vxmem, old_vent, VALIDf);
        }
        if (vld) {
            if (old_vp != NULL) {
                *old_vp = soc_mem_field32_get(unit, vxmem, old_vent,
                                              VIF__SOURCE_VPf);
            }
            profile_idx = soc_mem_field32_get(unit, vxmem, old_vent,
                                              VIF__TAG_ACTION_PROFILE_PTRf);
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        }
    }

    return rv;
}

/*
 * Function:
 *      _trident_niv_vxlate_traverse
 * Purpose:
 * traverse the vlan_xlate table to find out entries created
 * based on the niv port. Based on the delete_action flag to either 
 * modify or delete the found entries 
 *
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      vp - (IN) Virtual port number.
 *      niv_port - (IN) new niv port info
 *      delete_action - (IN) delete the found entry if TRUE or modify 
 *                           if FALSE
 * Returns:
 *      BCM_X_XXX
 */

STATIC int 
_trident_niv_vxlate_traverse (int unit, int vp, 
               bcm_niv_port_t *niv_port, int delete_action)
{
    /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int             buf_size, chunksize, chnk_end;
    uint32          *vt_tbl_chnk;
    uint32          *vent;
    int             valid,  rv = BCM_E_NONE;
    int             tmp_id;
    bcm_module_t    mod_out;
    bcm_port_t      port_out;
    bcm_trunk_t     trunk_out;
    uint32          old_vent[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t mem;
    int key_val;
    int vt_key;
    int vent_vp;
    int profile_idx;
    int vif_hit;
    int vif;
    int vent_gport;

    if ((!delete_action) && (niv_port == NULL)) {
        return BCM_E_INTERNAL;
    }
    mem = VLAN_XLATEm;
    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    chunksize = soc_property_get(unit, spn_VLANDELETE_CHUNKS,
                                 VLAN_MEM_CHUNKS_DEFAULT);
    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    vt_tbl_chnk = soc_cm_salloc(unit, buf_size, "vlan translate traverse");
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

        soc_mem_lock(unit, mem);
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, vt_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, mem);
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            vent = soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                                vt_tbl_chnk, ent_idx);
            if (soc_feature(unit, soc_feature_base_valid)) {
                valid = soc_mem_field32_get(unit, mem, vent,
                                            BASE_VALID_0f) == 3 &&
                        soc_mem_field32_get(unit, mem, vent,
                                            BASE_VALID_1f) == 7;
            } else {
                valid = soc_mem_field32_get(unit, mem, vent, VALIDf);
            }
            if (!valid) {
                continue;
            }
            key_val = soc_mem_field32_get(unit, mem, vent, KEY_TYPEf);
            rv = _bcm_esw_vlan_xlate_key_type_get(unit,
                  key_val,&vt_key);

            if (BCM_FAILURE(rv)) {
                break;
            }
            switch (vt_key) {
                case VLXLT_HASH_KEY_TYPE_VIF_OTAG:
                case VLXLT_HASH_KEY_TYPE_VIF_ITAG:
                case VLXLT_HASH_KEY_TYPE_VIF_VLAN:
                case VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
                    vif_hit = TRUE;
                    break;
                default:
                    vif_hit = FALSE;
                    break;
            }
            if (vif_hit == FALSE) {
                continue;
            }
            vent_vp = soc_mem_field32_get(unit, mem, vent, VIF__SOURCE_VPf);
            vif = soc_mem_field32_get(unit, mem, vent, VIF__SRC_VIFf);
            tmp_id = soc_mem_field32_get(unit, mem, vent, VIF__Tf);
            if (tmp_id) {
                trunk_out = soc_mem_field32_get(unit, mem, vent, VIF__TGIDf);
                BCM_GPORT_TRUNK_SET(vent_gport,trunk_out);
            } else {
                mod_out = soc_mem_field32_get(unit, mem, vent, VIF__MODULE_IDf);
                port_out = soc_mem_field32_get(unit, mem, vent, VIF__PORT_NUMf);
                BCM_GPORT_MODPORT_SET(vent_gport,mod_out,port_out);
            }
               
            if ((vent_vp != vp) || 
                (vent_gport != NIV_PORT_INFO(unit, vp)->port) ||
                (vif != NIV_PORT_INFO(unit, vp)->virtual_interface_id) ) {
                continue;
            }

            if (soc_feature(unit, soc_feature_base_valid)) {
                soc_mem_field32_set(unit, mem, vent, BASE_VALID_0f, 3);
                soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
            }

            /* found the match, delete it first */
            rv = soc_mem_delete_return_old(unit, mem,MEM_BLOCK_ALL,
                                           vent, old_vent);

            if (BCM_FAILURE(rv)) {
                break;
            }
            if (delete_action) {
                profile_idx = soc_mem_field32_get(unit, mem, old_vent,
                                    VIF__TAG_ACTION_PROFILE_PTRf);
                rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                             profile_idx);
                if (BCM_FAILURE(rv)) {
                    break;
                } else {
                    continue;
                }
            }

            /* modification action */
            vif = niv_port->virtual_interface_id;

            /* replace with the new niv port info */
            soc_mem_field32_set(unit, mem, old_vent, VIF__SRC_VIFf, vif);
            rv = _bcm_esw_gport_resolve(unit, niv_port->port,
                      &mod_out, &port_out, &trunk_out, &tmp_id);
            if (BCM_FAILURE(rv)) {
                break;
            }
            if (BCM_GPORT_IS_TRUNK(niv_port->port)) {
                soc_mem_field32_set(unit, mem, old_vent, VIF__Tf, 1);
                soc_mem_field32_set(unit, mem, old_vent, VIF__TGIDf,
                                    trunk_out);
            } else {
                soc_mem_field32_set(unit, mem, old_vent, VIF__MODULE_IDf,
                                    mod_out);
                soc_mem_field32_set(unit, mem, old_vent, VIF__PORT_NUMf,
                             port_out);
            }
            rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, old_vent);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        soc_mem_unlock(unit, mem);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    soc_cm_sfree(unit, vt_tbl_chnk);
    return rv;
}

/*
 * Function:
 *      bcm_trident_niv_port_add
 * Purpose:
 *      Create a NIV port on VIS.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      niv_port - (IN/OUT) NIV port information (OUT : niv_port_id)
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_trident_niv_port_add(int unit,
                         bcm_niv_port_t *niv_port)
{
    int mode;
    int vp, old_vp = 0, matched_vp;
    int rv = BCM_E_NONE;
    int num_vp;
    int nh_index = 0;
    bcm_if_t intf;
    uint32 nh_flags;
    bcm_l3_egress_t nh_info;
    ing_dvp_table_entry_t dvp_entry;
    source_vp_entry_t svp_entry;
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;
    int tpid_index, tpid_enable=0, old_tpid_enable=0;
    int i;
    _bcm_vp_info_t vp_info;

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeNiv;

    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_egress_mode_get(unit, &mode));
    if (!mode) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 egress mode must be set first\n")));
        return BCM_E_DISABLED;
    }

    /* Check for invalid flag combinations */
    if (niv_port->flags & BCM_NIV_PORT_MATCH_NONE) {
        if ((niv_port->flags & BCM_NIV_PORT_REPLACE) ||
            (niv_port->flags & BCM_NIV_PORT_MULTICAST) ||
            (niv_port->flags & BCM_NIV_PORT_MATCH_DO_NOT_ADD)) {
            return BCM_E_PARAM;
        }
    }

    if (!(niv_port->flags & BCM_NIV_PORT_REPLACE)) { /* Create new NIV VP */

        if (niv_port->flags & BCM_NIV_PORT_WITH_ID) {
            if (!BCM_GPORT_IS_NIV_PORT(niv_port->niv_port_id)) {
                return BCM_E_PARAM;
            }
            vp = BCM_GPORT_NIV_PORT_ID_GET(niv_port->niv_port_id);
            if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
                return BCM_E_PARAM;
            }
            if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
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

        if (niv_port->flags & BCM_NIV_PORT_MATCH_NONE) {
            /* For a NIV port without any match criteria, an invalid
             * next hop index will be written to the ING_DVP_TABLE.
             */
            nh_index = NIV_INFO(unit)->invalid_next_hop_index;
        } else {
            /* Allocate a next hop index */
            bcm_l3_egress_t_init(&nh_info);
            nh_flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE;
            rv = bcm_xgs3_nh_add(unit, nh_flags, &nh_info, &nh_index);
            if (rv < 0) {
                goto cleanup;
            }
           
            /* configure next hop tables */
            rv = _bcm_trident_niv_nh_info_set(unit, nh_index,
                    (niv_port->flags & BCM_NIV_PORT_MULTICAST) ? 1 : 0,
                    niv_port->port, niv_port->virtual_interface_id,
                    niv_port->match_vlan, NULL, vp,
                    (niv_port->flags & BCM_NIV_VNTAG_L_BIT_FORCE_1) ? 1 : 0);
            if (rv < 0) {
                goto cleanup;
            }
        }

        /* Configure DVP table */
        intf = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit) + nh_index;
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
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_NEWf, cml_default_new);
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_MOVEf, cml_default_move);
        } else {
            /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_NEWf, 0x8);
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_MOVEf, 0x8);
        }

        /* Set the SD-tag mode and TPID */
        if (niv_port->match_service_tpid) {
            rv = _bcm_fb2_outer_tpid_entry_add(unit, niv_port->match_service_tpid,
                    &tpid_index);
            if (rv < 0) {
                goto cleanup;
            }
            tpid_enable = 1 << tpid_index;
            if (soc_mem_is_valid(unit, SVP_ATTRS_1m)) {
                rv = _bcm_trident_niv_tpid_enb_set(unit, NULL, vp, tpid_enable);
                if (rv < 0) {
                    goto cleanup;
                }
            } else {
                rv = _bcm_trident_niv_tpid_enb_set(unit, (uint32 *)&svp_entry,
                                                   vp, tpid_enable);
                if (rv < 0) {
                    goto cleanup;
                }
            }
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, SD_TAG_MODEf, 1);
        } else {
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, SD_TAG_MODEf, 0);
        }

        soc_SOURCE_VPm_field32_set(unit, &svp_entry, CLASS_IDf, niv_port->if_class);

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
        } else {
            /* TD3TBD, in TD3, PARSE_USING_SGLP_TPID is removed */
        }
#endif /* BCM_TRIDENT2_SUPPORT */

        /* Configure ingress VLAN translation table for unicast VPs */
        if (!(niv_port->flags & BCM_NIV_PORT_MULTICAST) &&
            !(niv_port->flags & BCM_NIV_PORT_MATCH_NONE) &&
            !(niv_port->flags & BCM_NIV_PORT_MATCH_DO_NOT_ADD)) {
            rv = _bcm_trident_niv_match_add(unit, niv_port->port,
                    niv_port->virtual_interface_id, niv_port->match_vlan, vp);
            if (rv < 0) {
                goto cleanup;
            }
        }

        if (!(niv_port->flags & BCM_NIV_PORT_MATCH_NONE)) {
            /* Increment port's VP count */
            rv = _bcm_trident_niv_port_cnt_update(unit, niv_port->port, vp, 
                                                  TRUE, TRUE);
            if (rv < 0) {
                goto cleanup;
            }
        }

    } else { /* Replace properties of existing NIV VP */
        int all_same = 0;
        int tpid_enb_len = 0;
        bcm_niv_port_t niv_port_old;

        if (!(niv_port->flags & BCM_NIV_PORT_WITH_ID)) {
            return BCM_E_PARAM;
        }
        if (!BCM_GPORT_IS_NIV_PORT(niv_port->niv_port_id)) {
            return BCM_E_PARAM;
        }
        vp = BCM_GPORT_NIV_PORT_ID_GET(niv_port->niv_port_id);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
            return BCM_E_PARAM;
        }

        bcm_niv_port_t_init(&niv_port_old);
        niv_port_old.niv_port_id = niv_port->niv_port_id;
        BCM_IF_ERROR_RETURN
            (bcm_trident_niv_port_get(unit, &niv_port_old));

        if (BCM_GPORT_IS_TRUNK(niv_port->port)
            && niv_port->port == niv_port_old.port
            && (niv_port->flags & BCM_NIV_PORT_MULTICAST) 
                == (niv_port_old.flags & BCM_NIV_PORT_MULTICAST)
            && (niv_port->flags & BCM_NIV_PORT_MATCH_DO_NOT_ADD)
                == (niv_port_old.flags & BCM_NIV_PORT_MATCH_DO_NOT_ADD)
            && niv_port->virtual_interface_id 
                == niv_port_old.virtual_interface_id
            && niv_port->match_vlan == niv_port_old.match_vlan
            && niv_port->match_service_tpid
                == niv_port_old.match_service_tpid
            && niv_port->if_class == niv_port_old.if_class) {

            all_same = 1;
        } 

        matched_vp = vp;

        if (all_same == 0) {
            /* For existing niv vp, NH entry already exists */
            BCM_IF_ERROR_RETURN
                (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
            
            nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
                    NEXT_HOP_INDEXf);
            
            /* Update existing next hop entries */
        BCM_IF_ERROR_RETURN
            (_bcm_trident_niv_nh_info_set(unit, nh_index,
                    (niv_port->flags & BCM_NIV_PORT_MULTICAST) ? 1 : 0,
                    niv_port->port, niv_port->virtual_interface_id,
                    niv_port->match_vlan, NULL, vp,
                    (niv_port->flags & BCM_NIV_VNTAG_L_BIT_FORCE_1) ? 1 : 0));

            /* Update SD-tag mode, SD-tag TPID, and class ID */
            BCM_IF_ERROR_RETURN
                (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
            if (soc_mem_is_valid(unit, SVP_ATTRS_1m)) {
                BCM_IF_ERROR_RETURN(_bcm_trident_niv_tpid_enb_get(unit, NULL,
                                    vp, &old_tpid_enable));
                tpid_enb_len = soc_mem_field_length(unit, SVP_ATTRS_1m,
                                                    TPID_ENABLEf);
            } else {
                BCM_IF_ERROR_RETURN(_bcm_trident_niv_tpid_enb_get(unit,
                                    (uint32 *)&svp_entry, vp, &old_tpid_enable));
                tpid_enb_len = soc_mem_field_length(unit, SOURCE_VPm,
                                                    TPID_ENABLEf);
            }
            if (niv_port->match_service_tpid) {
                BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_add(unit,
                            niv_port->match_service_tpid, &tpid_index));
                tpid_enable = 1 << tpid_index;
                if (soc_mem_is_valid(unit, SVP_ATTRS_1m)) {
                    BCM_IF_ERROR_RETURN(_bcm_trident_niv_tpid_enb_set(unit, NULL,
                                        vp, tpid_enable));
                } else {
                    BCM_IF_ERROR_RETURN(_bcm_trident_niv_tpid_enb_set(unit,
                                        (uint32 *)&svp_entry, vp, tpid_enable));
                }
                soc_SOURCE_VPm_field32_set(unit, &svp_entry, SD_TAG_MODEf, 1);
            } else {
                soc_SOURCE_VPm_field32_set(unit, &svp_entry, SD_TAG_MODEf, 0);
            }
            if (old_tpid_enable) {
                for (i= 0; i < tpid_enb_len; i++) {
                    if (old_tpid_enable & (1 << i)) {
                        BCM_IF_ERROR_RETURN(
                            _bcm_fb2_outer_tpid_entry_delete(unit, i));
                    }
                }
            }
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CLASS_IDf,
                    niv_port->if_class);
            BCM_IF_ERROR_RETURN
                (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));

            /* Delete old ingress VLAN translation entry,
             * install new ingress VLAN translation entry
             */
            if (!(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MULTICAST) &&
                !(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MATCH_NONE) &&
                !(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MATCH_DO_NOT_ADD)) {
                BCM_IF_ERROR_RETURN(_bcm_trident_niv_match_delete(unit,
                            NIV_PORT_INFO(unit, vp)->port,
                            NIV_PORT_INFO(unit, vp)->virtual_interface_id,
                            NIV_PORT_INFO(unit, vp)->match_vlan,
                            &old_vp));
            }
#ifdef BCM_TRIDENT2_SUPPORT
            /* The NIV port id may be one of members of a vplag. */
            if (_bcm_vp_used_get(unit, old_vp, _bcmVpTypeVpLag)) {
                matched_vp = old_vp;
            }
#endif
        }

        if (!(niv_port->flags & BCM_NIV_PORT_MULTICAST) &&
            !(niv_port->flags & BCM_NIV_PORT_MATCH_DO_NOT_ADD)) {
            BCM_IF_ERROR_RETURN
                (_bcm_trident_niv_match_add(unit, niv_port->port,
                                            niv_port->virtual_interface_id,
                                            niv_port->match_vlan, matched_vp));
        }

        if (all_same == 0) {
            if (!(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MULTICAST)) {
                /* traverse the vlan_xlate table to find out entries created
                 * based on the niv port. Check the new niv port's multicast flag 
                 * to modify or delete existing entries accordingly 
                 */
                rv = _trident_niv_vxlate_traverse(unit,vp,niv_port,
                        (niv_port->flags & BCM_NIV_PORT_MULTICAST)? TRUE:FALSE);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
        /* Decrement old port's VP count, increment new port's VP count */
        BCM_IF_ERROR_RETURN
            (_bcm_trident_niv_port_cnt_update(unit,
                NIV_PORT_INFO(unit, vp)->port, vp, FALSE, TRUE));

        BCM_IF_ERROR_RETURN
            (_bcm_trident_niv_port_cnt_update(unit,
                niv_port->port, vp, TRUE, TRUE));

        if (all_same == 1) {
            return BCM_E_NONE;
        }
    }

    /* Set NIV VP software state */
    NIV_PORT_INFO(unit, vp)->flags = niv_port->flags;
    NIV_PORT_INFO(unit, vp)->port = niv_port->port;
    NIV_PORT_INFO(unit, vp)->virtual_interface_id = niv_port->virtual_interface_id;
    NIV_PORT_INFO(unit, vp)->match_vlan = niv_port->match_vlan;
    NIV_PORT_INFO(unit, vp)->egress_list = NULL;

    BCM_GPORT_NIV_PORT_ID_SET(niv_port->niv_port_id, vp);

    return rv;

cleanup: /* In case of failure */
    if (!(niv_port->flags & BCM_NIV_PORT_REPLACE)) {
        (void) _bcm_vp_free(unit, _bcmVpTypeNiv, 1, vp);
        if (!(niv_port->flags & BCM_NIV_PORT_MATCH_NONE)) {
            _bcm_trident_niv_nh_info_delete(unit, nh_index);
        }

        (void)_bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigClear, vp, 
                                     ING_DVP_CONFIG_INVALID_VP_TYPE, 
                                     ING_DVP_CONFIG_INVALID_INTF_ID, 
                                     ING_DVP_CONFIG_INVALID_PORT_TYPE);

        sal_memset(&svp_entry, 0, sizeof(source_vp_entry_t));
        (void)WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry);

        if (tpid_enable) {
            (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
        }

#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_mem_is_valid(unit, SOURCE_VP_2m)) {
            source_vp_2_entry_t svp_2_entry;

            sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
            (void)WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, vp, &svp_2_entry);
        }
#endif /* BCM_TRIDENT2_SUPPORT */

        if (!(niv_port->flags & BCM_NIV_PORT_MULTICAST) &&
            !(niv_port->flags & BCM_NIV_PORT_MATCH_NONE) &&
            !(niv_port->flags & BCM_NIV_PORT_MATCH_DO_NOT_ADD)) {
            (void) _bcm_trident_niv_match_delete(unit, niv_port->port,
                    niv_port->virtual_interface_id, niv_port->match_vlan, NULL);
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_trident_niv_port_delete
 * Purpose:
 *      Destroy a NIV virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) NIV GPORT ID.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_trident_niv_port_delete(int unit, bcm_gport_t gport)
{
    int vp;
    source_vp_entry_t svp_entry;
    int nh_index;
    ing_dvp_table_entry_t dvp_entry;
    int old_tpid_enable, i, tpid_enb_len;

    if (!BCM_GPORT_IS_NIV_PORT(gport)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_NOT_FOUND;
    }

    if (NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MATCH_NONE) {
        if (NIV_PORT_INFO(unit, vp)->egress_list != NULL) {
            /* There are still NIV egress objects attached to this
             * NIV port.
             */
            return BCM_E_BUSY;
        }
    }

    /* Delete ingress VLAN translation entry */
    if (!(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MULTICAST) &&
        !(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MATCH_NONE) &&
        !(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MATCH_DO_NOT_ADD)) {
        BCM_IF_ERROR_RETURN(_bcm_trident_niv_match_delete(unit,
                    NIV_PORT_INFO(unit, vp)->port,
                    NIV_PORT_INFO(unit, vp)->virtual_interface_id,
                    NIV_PORT_INFO(unit, vp)->match_vlan,
                    NULL));
        BCM_IF_ERROR_RETURN(_trident_niv_vxlate_traverse(unit,vp,
                    NULL, TRUE));
    }

    /* Clear SVP entry */
    BCM_IF_ERROR_RETURN
        (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
    if (soc_mem_is_valid(unit, SVP_ATTRS_1m)) {
        BCM_IF_ERROR_RETURN(_bcm_trident_niv_tpid_enb_get(unit, NULL,
                            vp, &old_tpid_enable));
        tpid_enb_len = soc_mem_field_length(unit, SVP_ATTRS_1m,
                                            TPID_ENABLEf);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_trident_niv_tpid_enb_get(unit,
                            (uint32 *)&svp_entry, vp, &old_tpid_enable));
        tpid_enb_len = soc_mem_field_length(unit, SOURCE_VPm,
                                            TPID_ENABLEf);
    }

    if (old_tpid_enable) {
        for (i= 0; i < tpid_enb_len; i++) {
            if (old_tpid_enable & (1 << i)) {
                BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_delete(unit, i));
            }
        }
    }
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

    BCM_IF_ERROR_RETURN
        (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry, NEXT_HOP_INDEXf);

    /* Clear DVP entry */
    BCM_IF_ERROR_RETURN(
        _bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigClear, vp, 
                               ING_DVP_CONFIG_INVALID_VP_TYPE, 
                               ING_DVP_CONFIG_INVALID_INTF_ID, 
                               ING_DVP_CONFIG_INVALID_PORT_TYPE));

    if (!(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MATCH_NONE)) {
        /* Clear next hop entries and free next hop index */
        BCM_IF_ERROR_RETURN
            (_bcm_trident_niv_nh_info_delete(unit, nh_index));

        /* Decrement port's VP count */
        BCM_IF_ERROR_RETURN
            (_bcm_trident_niv_port_cnt_update(unit,
                                              NIV_PORT_INFO(unit, vp)->port,
                                              vp, FALSE, TRUE));
    }

    /* Free VP */
    BCM_IF_ERROR_RETURN
        (_bcm_vp_free(unit, _bcmVpTypeNiv, 1, vp));

    /* Clear NIV VP software state */
    sal_memset(NIV_PORT_INFO(unit, vp), 0, sizeof(_bcm_trident_niv_port_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_trident_niv_port_delete_all
 * Purpose:
 *      Destroy all NIV virtual ports.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_trident_niv_port_delete_all(int unit)
{
    int i;
    bcm_gport_t niv_port_id;

    for (i = soc_mem_index_min(unit, SOURCE_VPm);
         i <= soc_mem_index_max(unit, SOURCE_VPm);
         i++) {
        if (_bcm_vp_used_get(unit, i, _bcmVpTypeNiv)) {
            BCM_GPORT_NIV_PORT_ID_SET(niv_port_id, i);
            BCM_IF_ERROR_RETURN(bcm_trident_niv_port_delete(unit, niv_port_id));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_trident_niv_port_get
 * Purpose:
 *      Get NIV virtual port info.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      niv_port - (IN/OUT) Pointer to NIV virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_trident_niv_port_get(int unit, bcm_niv_port_t *niv_port)
{
    int vp;
    source_vp_entry_t svp_entry;
    int old_tpid_enable, i;
    int tp_enb_length;

    if (!BCM_GPORT_IS_NIV_PORT(niv_port->niv_port_id)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_NIV_PORT_ID_GET(niv_port->niv_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_NOT_FOUND;
    }

    bcm_niv_port_t_init(niv_port);
    niv_port->flags = NIV_PORT_INFO(unit, vp)->flags;
    BCM_GPORT_NIV_PORT_ID_SET(niv_port->niv_port_id, vp);
    niv_port->port = NIV_PORT_INFO(unit, vp)->port;
    niv_port->virtual_interface_id = NIV_PORT_INFO(unit, vp)->virtual_interface_id;
    niv_port->match_vlan = NIV_PORT_INFO(unit, vp)->match_vlan;

    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));

    if (soc_mem_is_valid(unit, SVP_ATTRS_1m)) {
        BCM_IF_ERROR_RETURN(_bcm_trident_niv_tpid_enb_get(unit, NULL,
                            vp, &old_tpid_enable));
        tp_enb_length = soc_mem_field_length(unit, SVP_ATTRS_1m,
                                            TPID_ENABLEf);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_trident_niv_tpid_enb_get(unit,
                            (uint32 *)&svp_entry, vp, &old_tpid_enable));
        tp_enb_length = soc_mem_field_length(unit, SOURCE_VPm,
                                            TPID_ENABLEf);
    }

    if (old_tpid_enable) {
        for (i= 0; i < tp_enb_length; i++) {
            if (old_tpid_enable & (1 << i)) {
                BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_get(unit,
                            &niv_port->match_service_tpid, i));
            }
        }
    }
    niv_port->if_class = soc_SOURCE_VPm_field32_get(unit, &svp_entry, CLASS_IDf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_trident_niv_port_traverse
 * Purpose:
 *      Traverse all NIV ports and call supplied callback routine.
 * Parameters:
 *      unit      - (IN) Device Number
 *      cb        - (IN) User-provided callback
 *      user_data - (IN/OUT) Cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_port_traverse(int unit, bcm_niv_port_traverse_cb cb, 
                                  void *user_data)
{
    int i;
    bcm_niv_port_t niv_port;

    for (i = soc_mem_index_min(unit, SOURCE_VPm);
         i <= soc_mem_index_max(unit, SOURCE_VPm);
         i++) {
        if (_bcm_vp_used_get(unit, i, _bcmVpTypeNiv)) {
            bcm_niv_port_t_init(&niv_port);
            BCM_GPORT_NIV_PORT_ID_SET(niv_port.niv_port_id, i);
            BCM_IF_ERROR_RETURN(bcm_trident_niv_port_get(unit, &niv_port));
            BCM_IF_ERROR_RETURN(cb(unit, &niv_port, user_data));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trident_niv_port_resolve
 * Purpose:
 *      Get the modid, port, trunk values for a NIV virtual port
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
_bcm_trident_niv_port_resolve(int unit, bcm_gport_t niv_port_id,
                          bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id)

{
    int rv = BCM_E_NONE, nh_index, vp;
    ing_l3_next_hop_entry_t ing_nh;
    ing_dvp_table_entry_t dvp;

    if (!BCM_GPORT_IS_NIV_PORT(niv_port_id)) {
        return (BCM_E_BADID);
    }

    vp = BCM_GPORT_NIV_PORT_ID_GET(niv_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_NOT_FOUND;
    }
    BCM_IF_ERROR_RETURN(READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                              NEXT_HOP_INDEXf);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      nh_index, &ing_nh));

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh,
            _NIV_ENTRY_TYPEf(unit, ING_L3_NEXT_HOPm)) != 0x2) {
        /* Entry type is not L2 DVP */
        return BCM_E_NOT_FOUND;
    }

    if (soc_feature(unit, soc_feature_generic_dest)) {
        uint32 dt, dv;
        dv = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm,
                &ing_nh, DESTINATIONf, &dt);
        if (dt == SOC_MEM_FIF_DEST_LAG) {
            /* Trunk group */
            *trunk_id = dv & SOC_MEM_FIF_DGPP_TGID_MASK;
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
 *	Create NIV Forwarding table entry
 * Parameters:
 *	unit - (IN) Device Number
 *	iv_fwd_entry - (IN) NIV Forwarding table entry
 */
int
bcm_trident_niv_forward_add(int unit, bcm_niv_forward_t *iv_fwd_entry)
{
    int rv = BCM_E_NONE;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t tgid_out;
    int id_out;
    l2x_entry_t l2x_entry;

    if (iv_fwd_entry->name_space > 0xfff) {
        return BCM_E_PARAM;
    }

    sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
    if (!(iv_fwd_entry->flags & BCM_NIV_FORWARD_MULTICAST)) {
        if (iv_fwd_entry->virtual_interface_id > 0xfff) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit,
                    iv_fwd_entry->dest_port,
                    &mod_out, &port_out, &tgid_out, &id_out));
        if (-1 != id_out) {
            return BCM_E_PARAM;
        }

        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 0);

        if (tgid_out != -1) {
            BCM_IF_ERROR_RETURN(_bcm_trunk_id_validate(unit, tgid_out));
            if (soc_feature(unit, soc_feature_generic_dest)) {
                soc_mem_field32_dest_set(unit, L2Xm, &l2x_entry,
                    VIF__DESTINATIONf, SOC_MEM_FIF_DEST_LAG, tgid_out);
            } else {
                soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DEST_TYPEf, 1);
                soc_L2Xm_field32_set(unit, &l2x_entry, VIF__TGIDf, tgid_out);
            }
        } else {
            if (soc_feature(unit, soc_feature_generic_dest)) {
                soc_mem_field32_dest_set(unit, L2Xm, &l2x_entry,
                    VIF__DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                    mod_out << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | port_out);
            } else {
                soc_L2Xm_field32_set(unit, &l2x_entry, VIF__MODULE_IDf, mod_out);
                soc_L2Xm_field32_set(unit, &l2x_entry, VIF__PORT_NUMf, port_out);
            }
        }
    } else { /* Multicast forward entry */
        if (iv_fwd_entry->virtual_interface_id > 0x3fff) {
            return BCM_E_PARAM;
        }
        if (!_BCM_MULTICAST_IS_L2(iv_fwd_entry->dest_multicast)) {
            return BCM_E_PARAM;
        }

        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 1);
        if (soc_feature(unit, soc_feature_generic_dest)) {
            soc_mem_field32_dest_set(unit, L2Xm, &l2x_entry,
                VIF__DESTINATIONf, SOC_MEM_FIF_DEST_L2MC,
                _BCM_MULTICAST_ID_GET(iv_fwd_entry->dest_multicast));
        } else {
            soc_L2Xm_field32_set(unit, &l2x_entry, VIF__L2MC_PTRf,
                _BCM_MULTICAST_ID_GET(iv_fwd_entry->dest_multicast));
        }
    }

    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, BASE_VALIDf, 1);
    } else {
        soc_L2Xm_field32_set(unit, &l2x_entry, VALIDf, 1);
    }
    soc_L2Xm_field32_set(unit, &l2x_entry, VIF__NAMESPACEf,
            iv_fwd_entry->name_space);
    soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DST_VIFf,
            iv_fwd_entry->virtual_interface_id);

    if (SOC_IS_KATANA2(unit)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                KT2_L2_HASH_KEY_TYPE_VIF);
    } else {
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                TR_L2_HASH_KEY_TYPE_VIF);
    }
    soc_L2Xm_field32_set(unit, &l2x_entry, VIF__STATIC_BITf, 1);
#ifdef BCM_HURRICANE4_SUPPORT
    if (soc_feature(unit, soc_feature_hr4_l2x_static_bit_war)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__STATIC_DUPLICATEf, 1);
    }
#endif /* BCM_HURRICANE4_SUPPORT */

    soc_mem_lock(unit, L2Xm);

    if (!(iv_fwd_entry->flags & BCM_NIV_FORWARD_REPLACE)) {
        l2x_entry_t  l2x_lookup;
        int          l2_index;

        /* See if the entry already exists */
        rv = soc_mem_generic_lookup(unit, L2Xm, MEM_BLOCK_ANY, 0, &l2x_entry,
                                    &l2x_lookup, &l2_index);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
        if (BCM_SUCCESS(rv)) {
            soc_mem_unlock(unit, L2Xm);
            return BCM_E_EXISTS;
        }

        rv = soc_mem_insert(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
    } else { /* Replace existing entry */
        rv = soc_mem_delete(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
        rv = soc_mem_insert(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
    }

    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Purpose:
 *	Delete NIV Forwarding table entry
 * Parameters:
 *      unit - (IN) Device Number
 *      iv_fwd_entry - (IN) NIV Forwarding table entry
 */
int
bcm_trident_niv_forward_delete(int unit, bcm_niv_forward_t *iv_fwd_entry)
{
    int rv = BCM_E_NONE;
    l2x_entry_t l2x_entry;

    if (iv_fwd_entry->name_space > 0xfff) {
        return BCM_E_PARAM;
    }
    
    sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
    if (!(iv_fwd_entry->flags & BCM_NIV_FORWARD_MULTICAST)) {
        if (iv_fwd_entry->virtual_interface_id > 0xfff) {
            return BCM_E_PARAM;
        }
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 0);

    } else { /* Multicast forward entry */
        if (iv_fwd_entry->virtual_interface_id > 0x3fff) {
            return BCM_E_PARAM;
        }
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 1);
    }

    soc_L2Xm_field32_set(unit, &l2x_entry, VIF__NAMESPACEf,
            iv_fwd_entry->name_space);

    soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DST_VIFf,
            iv_fwd_entry->virtual_interface_id);
    if (SOC_IS_KATANA2(unit)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                KT2_L2_HASH_KEY_TYPE_VIF);
    } else {
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                TR_L2_HASH_KEY_TYPE_VIF);
    }

    soc_mem_lock(unit, L2Xm);
    rv = soc_mem_delete(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Purpose:
 *	Delete all NIV Forwarding table entries
 * Parameters:
 *	unit - Device Number
 */
int
bcm_trident_niv_forward_delete_all(int unit)
{
    int rv = BCM_E_NONE;
    int field_len;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_l2_bulk_unified_table)) {
        l2_bulk_entry_t match_mask;
        l2_bulk_entry_t match_data;
        soc_field_t vld_f = VALIDf;

        if (soc_feature(unit, soc_feature_base_valid)) {
            vld_f = BASE_VALIDf;
        }

        sal_memset(&match_mask, 0, sizeof(match_mask));
        sal_memset(&match_data, 0, sizeof(match_data));

        soc_mem_field32_set(unit, L2_BULKm, &match_mask, vld_f, 1);
        soc_mem_field32_set(unit, L2_BULKm, &match_data, vld_f, 1);

        field_len = soc_mem_field_length(unit, L2_BULKm, KEY_TYPEf);
        soc_mem_field32_set(unit, L2_BULKm, &match_mask, KEY_TYPEf,
                            (1 << field_len) - 1);
   
        if (SOC_IS_KATANA2(unit)) {
            soc_mem_field32_set(unit, L2_BULKm, &match_data, KEY_TYPEf,
                    KT2_L2_HASH_KEY_TYPE_VIF);
        } else {
            soc_mem_field32_set(unit, L2_BULKm, &match_data, KEY_TYPEf,
                    TR_L2_HASH_KEY_TYPE_VIF);
        }

        soc_mem_lock(unit, L2Xm);
        rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_MATCH_MASK_INX,
                            &match_mask);
        if (BCM_SUCCESS(rv)) {
            rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_MATCH_DATA_INX,
                                &match_data);
        }
        if (BCM_SUCCESS(rv)) {
            rv = soc_reg_field32_modify(unit, L2_BULK_CONTROLr, REG_PORT_ANY,
                                        ACTIONf, 1);
        }
        if (BCM_SUCCESS(rv)) {
            rv = soc_reg_field32_modify(unit, L2_BULK_CONTROLr, REG_PORT_ANY,
                                    NUM_ENTRIESf, soc_mem_index_count(unit, L2Xm));
        }
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
    {
        l2_bulk_match_mask_entry_t match_mask;
        l2_bulk_match_data_entry_t match_data;

        sal_memset(&match_mask, 0, sizeof(match_mask));
        sal_memset(&match_data, 0, sizeof(match_data));

        soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask, VALIDf, 1);
        soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, &match_data, VALIDf, 1);

        field_len = soc_mem_field_length(unit, L2_BULK_MATCH_MASKm, KEY_TYPEf);
        soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask, KEY_TYPEf,
                (1 << field_len) - 1);

        if (SOC_IS_KATANA2(unit)) {
            soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, &match_data, KEY_TYPEf,
                    KT2_L2_HASH_KEY_TYPE_VIF);
        } else {
            soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, &match_data, KEY_TYPEf,
                    TR_L2_HASH_KEY_TYPE_VIF);
        }

        soc_mem_lock(unit, L2Xm);
        rv = WRITE_L2_BULK_MATCH_MASKm(unit, MEM_BLOCK_ALL, 0,
                &match_mask);
        if (BCM_SUCCESS(rv)) {
            rv = WRITE_L2_BULK_MATCH_DATAm(unit, MEM_BLOCK_ALL, 0,
                    &match_data);
        }
        if (BCM_SUCCESS(rv)) {
            rv = soc_reg_field32_modify(unit, L2_BULK_CONTROLr, REG_PORT_ANY,
                    ACTIONf, 1);
        }
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
    }
    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Purpose:
 *      Get NIV Forwarding table entry
 * Parameters:
 *      unit - (IN) Device Number
 *      iv_fwd_entry - (IN/OUT) NIV forwarding table info
 */
int
bcm_trident_niv_forward_get(int unit, bcm_niv_forward_t *iv_fwd_entry)
{
    int rv = BCM_E_NONE;
    l2x_entry_t l2x_entry, l2x_entry_out;
    int idx;
    _bcm_gport_dest_t dest;
    int l2mc_index;

    if (iv_fwd_entry->name_space > 0xfff) {
        return BCM_E_PARAM;
    }
    sal_memset(&l2x_entry, 0, sizeof(l2x_entry));

    if (!(iv_fwd_entry->flags & BCM_NIV_FORWARD_MULTICAST)) {
        if (iv_fwd_entry->virtual_interface_id > 0xfff) {
            return BCM_E_PARAM;
        }

        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 0);

    } else { /* Multicast forward entry */
        if (iv_fwd_entry->virtual_interface_id > 0x3fff) {
            return BCM_E_PARAM;
        }
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 1);
    }

    if (SOC_IS_KATANA2(unit)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                KT2_L2_HASH_KEY_TYPE_VIF);
    } else {
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                TR_L2_HASH_KEY_TYPE_VIF);
    }
    soc_L2Xm_field32_set(unit, &l2x_entry, VIF__NAMESPACEf,
            iv_fwd_entry->name_space);
    soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DST_VIFf,
            iv_fwd_entry->virtual_interface_id);

    soc_mem_lock(unit, L2Xm);
    rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ALL, &idx, &l2x_entry,
            &l2x_entry_out, 0);
    soc_mem_unlock(unit, L2Xm);

    if (SOC_SUCCESS(rv)) {
        uint32 dv, dt = SOC_MEM_FIF_DEST_INVALID;
        if (!(iv_fwd_entry->flags & BCM_NIV_FORWARD_MULTICAST)) {
            if (soc_feature(unit, soc_feature_generic_dest)) {
                dv = soc_mem_field32_dest_get(unit, L2Xm, &l2x_entry_out,
                                              VIF__DESTINATIONf, &dt);
                if (dt == SOC_MEM_FIF_DEST_LAG) {
                    dest.tgid = dv;
                    dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                } else {
                    dest.port = dv & SOC_MEM_FIF_DGPP_PORT_MASK;
                    dest.modid = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                                    SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
                    dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                }
            } else {
                if (soc_L2Xm_field32_get(unit, &l2x_entry_out, VIF__DEST_TYPEf)) {
                    dest.tgid = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                        VIF__TGIDf);
                    dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                } else {
                    dest.modid = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                            VIF__MODULE_IDf);
                    dest.port = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                            VIF__PORT_NUMf);
                    dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                }
            }
            BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest,
                        &(iv_fwd_entry->dest_port)));
        } else {
            if (soc_feature(unit, soc_feature_generic_dest)) {
                dv = soc_mem_field32_dest_get(unit, L2Xm, &l2x_entry_out,
                                              VIF__DESTINATIONf, &dt);
                l2mc_index = (dt == SOC_MEM_FIF_DEST_L2MC) ? dv : 0;
            } else {
                l2mc_index = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                                                  VIF__L2MC_PTRf);
            }
            _BCM_MULTICAST_GROUP_SET(iv_fwd_entry->dest_multicast,
                    _BCM_MULTICAST_TYPE_L2, l2mc_index);
        }
    }

    return rv;
}

/*
 * Purpose:
 *      Traverse all valid NIV forward entries and call the
 *      supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      cb        - User callback function, called once per NIV forward entry.
 *      user_data - cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_forward_traverse(int unit,
                             bcm_niv_forward_traverse_cb cb,
                             void *user_data)
{
    int rv = BCM_E_NONE;
    int chunk_entries, chunk_bytes;
    uint8 *l2_tbl_chunk = NULL;
    int chunk_idx_min, chunk_idx_max, chunk_end;
    int ent_idx, mem_idx_min, mem_idx_max;
    l2x_entry_t *l2x_entry;
    bcm_niv_forward_t iv_fwd_entry;
    int l2mc_index;
    _bcm_gport_dest_t dest;
    uint32 dv, dt = SOC_MEM_FIF_DEST_INVALID;

    chunk_entries = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);
    chunk_bytes = 4 * SOC_MEM_WORDS(unit, L2Xm) * chunk_entries;
    l2_tbl_chunk = soc_cm_salloc(unit, chunk_bytes, "niv forward traverse");
    if (NULL == l2_tbl_chunk) {
        return BCM_E_MEMORY;
    }

    mem_idx_min = soc_mem_index_min(unit, L2Xm);
    mem_idx_max = soc_mem_index_max(unit, L2Xm);
    for (chunk_idx_min = mem_idx_min; chunk_idx_min <= mem_idx_max; 
         chunk_idx_min += chunk_entries) {
        sal_memset(l2_tbl_chunk, 0, chunk_bytes);

        chunk_idx_max = chunk_idx_min + chunk_entries - 1;
        if (chunk_idx_max > mem_idx_max) {
            chunk_idx_max = mem_idx_max;
        }

        rv = soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY,
                                chunk_idx_min, chunk_idx_max, l2_tbl_chunk);
        if (SOC_FAILURE(rv)) {
            break;
        }

        chunk_end = (chunk_idx_max - chunk_idx_min);
        for (ent_idx = 0; ent_idx <= chunk_end; ent_idx++) {
            l2x_entry = soc_mem_table_idx_to_pointer(unit, L2Xm, l2x_entry_t *, 
                                             l2_tbl_chunk, ent_idx);

            if (soc_L2Xm_field32_get(unit, l2x_entry,
                    (soc_feature(unit, soc_feature_base_valid) ?
                    BASE_VALIDf : VALIDf)) == 0) {
                continue;
            }
            if (SOC_IS_KATANA2(unit)) {
                if (soc_L2Xm_field32_get(unit, l2x_entry, KEY_TYPEf) != 
                        KT2_L2_HASH_KEY_TYPE_VIF) {
                    continue;
                }
            } else {
                if (soc_L2Xm_field32_get(unit, l2x_entry, KEY_TYPEf) != 
                        TR_L2_HASH_KEY_TYPE_VIF) {
                    continue;
                }
            }
            bcm_niv_forward_t_init(&iv_fwd_entry);

            iv_fwd_entry.name_space = soc_L2Xm_field32_get(unit, l2x_entry,
                    VIF__NAMESPACEf);
            iv_fwd_entry.virtual_interface_id = soc_L2Xm_field32_get(unit,
                    l2x_entry, VIF__DST_VIFf);

            if (soc_L2Xm_field32_get(unit, l2x_entry, VIF__Pf)) {
                /* Multicast NIV forward entry */
                iv_fwd_entry.flags |= BCM_NIV_FORWARD_MULTICAST;
                if (soc_feature(unit, soc_feature_generic_dest)) {
                    dv = soc_mem_field32_dest_get(unit, L2Xm, l2x_entry,
                                                  VIF__DESTINATIONf, &dt);
                    l2mc_index = (dt == SOC_MEM_FIF_DEST_L2MC) ? dv : 0;
                } else {
                    l2mc_index = soc_L2Xm_field32_get(unit, l2x_entry,
                            VIF__L2MC_PTRf);
                }
                _BCM_MULTICAST_GROUP_SET(iv_fwd_entry.dest_multicast,
                        _BCM_MULTICAST_TYPE_L2, l2mc_index);
            } else {
                if (soc_feature(unit, soc_feature_generic_dest)) {
                    dv = soc_mem_field32_dest_get(unit, L2Xm, l2x_entry,
                                                  VIF__DESTINATIONf, &dt);
                    if (dt == SOC_MEM_FIF_DEST_LAG) {
                        dest.tgid = dv;
                        dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                    } else {
                        dest.port = dv & SOC_MEM_FIF_DGPP_PORT_MASK;
                        dest.modid = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                                        SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
                        dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                    }
                } else {
                    if (soc_L2Xm_field32_get(unit, l2x_entry, VIF__DEST_TYPEf)) {
                        dest.tgid = soc_L2Xm_field32_get(unit, l2x_entry,
                                VIF__TGIDf);
                        dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                    } else {
                        dest.modid = soc_L2Xm_field32_get(unit, l2x_entry,
                                VIF__MODULE_IDf);
                        dest.port = soc_L2Xm_field32_get(unit, l2x_entry,
                                VIF__PORT_NUMf);
                        dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                    }
                }
                rv = _bcm_esw_gport_construct(unit, &dest,
                        &(iv_fwd_entry.dest_port));
                if (BCM_FAILURE(rv)) {
                    break;
                }
            }

            rv = cb(unit, &iv_fwd_entry, user_data);
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
 *      bcm_trident_niv_ethertype_set
 * Purpose:
 *      Set NIV Ethertype.
 * Parameters:
 *      unit      - (IN) BCM device number
 *      ethertype - (IN) NIV Ethertype
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_ethertype_set(int unit, int ethertype)
{
    if (ethertype < 0 || ethertype > 0xffff) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        if (SOC_REG_FIELD_VALID(unit, NIV_ETHERTYPEr, ETHERTYPEf)) {
            BCM_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, NIV_ETHERTYPEr,
                                    REG_PORT_ANY, ETHERTYPEf, ethertype));
            BCM_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, NIV_ETHERTYPEr,
                                    REG_PORT_ANY, ENABLEf, ethertype ? 1 : 0));
        }
        if (SOC_REG_FIELD_VALID(unit, EGR_NIV_ETHERTYPE_2r, ETHERTYPEf)) {
            BCM_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EGR_NIV_ETHERTYPE_2r,
                                    REG_PORT_ANY, ETHERTYPEf, ethertype));
            BCM_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EGR_NIV_ETHERTYPE_2r,
                                    REG_PORT_ANY, ENABLEf, ethertype ? 1 : 0));
        }
        soc_td3_rx_etype_niv[unit] = ethertype;
    } else
#endif
    {
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, NIV_ETHERTYPEr,
                                    REG_PORT_ANY, ETHERTYPEf, ethertype));
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, NIV_ETHERTYPEr,
                                    REG_PORT_ANY, ENABLEf, ethertype ? 1 : 0));
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_NIV_ETHERTYPEr,
                                    REG_PORT_ANY, ETHERTYPEf, ethertype));
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_NIV_ETHERTYPEr,
                                    REG_PORT_ANY, ENABLEf, ethertype ? 1 : 0));
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_NIV_ETHERTYPE_2r,
                                    REG_PORT_ANY, ETHERTYPEf, ethertype));
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_NIV_ETHERTYPE_2r,
                                    REG_PORT_ANY, ENABLEf, ethertype ? 1 : 0));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_trident_niv_ethertype_get
 * Purpose:
 *      Get NIV Ethertype.
 * Parameters:
 *      unit      - (IN) BCM device number
 *      ethertype - (OUT) NIV Ethertype
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_ethertype_get(int unit, int *ethertype)
{
    uint32 rval;
    soc_reg_t reg;

    if (ethertype == NULL) {
        return BCM_E_PARAM;
    }

    reg = NIV_ETHERTYPEr;

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    if (soc_reg_field_get(unit, reg, rval, ENABLEf)) {
        *ethertype = soc_reg_field_get(unit, reg, rval, ETHERTYPEf);
    } else {
        *ethertype = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_trident_niv_untagged_add
 * Purpose:
 *      Set NIV VP tagging/untagging status by adding
 *      a (NIV VP, VLAN) egress VLAN translation entry.
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
bcm_trident_niv_untagged_add(int unit, bcm_vlan_t vlan, int vp, int flags,
        int *egr_vt_added)
{
    uint32 vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32 old_vent[SOC_MAX_MEM_FIELD_WORDS];
    bcm_vlan_action_set_t action;
    uint32 profile_idx;
    int rv = BCM_E_NONE;
    bcm_vlan_t    match_vlan;
    soc_mem_t evx_mem = EGR_VLAN_XLATEm;

    if (soc_mem_is_valid(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        evx_mem = EGR_VLAN_XLATE_1_DOUBLEm;
    }

    *egr_vt_added = FALSE;
    _bcm_trident_niv_match_info_get(unit, vp, NULL, &match_vlan, NULL);

    sal_memset(vent, 0, sizeof(vent));
    if (evx_mem == EGR_VLAN_XLATE_1_DOUBLEm) {
        soc_mem_field32_set(unit, evx_mem, vent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, evx_mem, vent, BASE_VALID_1f, 7);
        soc_mem_field32_set(unit, evx_mem, vent, DATA_TYPEf, 1);
        soc_mem_field32_set(unit, evx_mem, vent, KEY_TYPEf, 1);
    } else {
        soc_mem_field32_set(unit, evx_mem, vent, VALIDf, 1);
        if (soc_mem_field_valid(unit, evx_mem, ENTRY_TYPEf)) {
            soc_mem_field32_set(unit, evx_mem, vent, ENTRY_TYPEf, 1);
        } else if (soc_mem_field_valid(unit, evx_mem, KEY_TYPEf)) {
            soc_mem_field32_set(unit, evx_mem, vent, KEY_TYPEf, 1);
        }
    }

    soc_mem_field32_set(unit, evx_mem, vent, DVPf, vp);
    soc_mem_field32_set(unit, evx_mem, vent, OVIDf, vlan);

    if (!BCM_VLAN_VALID(match_vlan)) {
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
        /* NIV port's match_vlan is valid. It needs to be inserted
         * into the packet using an egress vlan translation entry.
         */
        soc_mem_field32_set(unit, evx_mem, vent, NEW_OVIDf, match_vlan);
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
    soc_mem_field32_set(unit, evx_mem, vent, TAG_ACTION_PROFILE_PTRf,
            profile_idx);

    rv = soc_mem_insert_return_old(unit, evx_mem, MEM_BLOCK_ALL,
                                   vent, old_vent);
    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_mem_field32_get(unit, evx_mem, old_vent,
                                          TAG_ACTION_PROFILE_PTRf);
        rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx);
    }

    *egr_vt_added = TRUE;

    return rv;
}

/*
 * Function:
 *      bcm_trident_niv_untagged_delete
 * Purpose:
 *      Delete (NIV VP, VLAN) egress VLAN translation entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN ID. 
 *      vp   - (IN) Virtual port number.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_trident_niv_untagged_delete(int unit, bcm_vlan_t vlan, int vp)
{
    uint32 vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32 old_vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32 profile_idx;
    int rv;
    soc_mem_t evx_mem = EGR_VLAN_XLATEm;

    if (soc_mem_is_valid(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        evx_mem = EGR_VLAN_XLATE_1_DOUBLEm;
    }

    sal_memset(vent, 0, sizeof(vent));
    if (evx_mem == EGR_VLAN_XLATE_1_DOUBLEm) {
        soc_mem_field32_set(unit, evx_mem, vent, DATA_TYPEf, 1);
        soc_mem_field32_set(unit, evx_mem, vent, KEY_TYPEf, 1);
    } else {
        if (soc_mem_field_valid(unit, evx_mem, ENTRY_TYPEf)) {
            soc_mem_field32_set(unit, evx_mem, vent, ENTRY_TYPEf, 1);
        } else if (soc_mem_field_valid(unit, evx_mem, KEY_TYPEf)) {
            soc_mem_field32_set(unit, evx_mem, vent, KEY_TYPEf, 1);
        }
    }
    soc_mem_field32_set(unit, evx_mem, vent, DVPf, vp);
    soc_mem_field32_set(unit, evx_mem, vent, OVIDf, vlan);

    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, evx_mem, vent, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, evx_mem, vent, BASE_VALID_1f, 7);
    }

    rv = soc_mem_delete_return_old(unit, evx_mem, MEM_BLOCK_ALL,
                                   vent, old_vent);
    if (rv == SOC_E_NONE) {
        int vld;
        if (soc_feature(unit, soc_feature_base_valid)) {
            vld = soc_mem_field32_get(unit, evx_mem, old_vent,
                                      BASE_VALID_0f) == 3 &&
                  soc_mem_field32_get(unit, evx_mem, old_vent,
                                      BASE_VALID_1f) == 7;
        } else {
            vld = soc_mem_field32_get(unit, evx_mem, old_vent, VALIDf);
        }
        if (vld) {
            /* Delete the old vlan translate profile entry */
            profile_idx = soc_mem_field32_get(unit, evx_mem, old_vent,
                                              TAG_ACTION_PROFILE_PTRf);
            rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx);
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_trident_niv_untagged_get
 * Purpose:
 *      Get tagging/untagging status of a NIV virtual port by
 *      reading the (NIV VP, VLAN) egress vlan translation entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN to remove virtual port from.
 *      vp   - (IN) Virtual port number.
 *      flags - (OUT) Untagging status of the NIV virtual port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_untagged_get(int unit, bcm_vlan_t vlan, int vp,
                                  int *flags)
{
    uint32 vent[SOC_MAX_MEM_FIELD_WORDS];
    uint32 res_vent[SOC_MAX_MEM_FIELD_WORDS];
    int idx;
    uint32 profile_idx;
    int rv;
    bcm_vlan_action_set_t action;
    bcm_vlan_t    match_vlan;
    soc_mem_t evx_mem = EGR_VLAN_XLATEm;

    if (soc_mem_is_valid(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        evx_mem = EGR_VLAN_XLATE_1_DOUBLEm;
    }

    *flags = 0;
    _bcm_trident_niv_match_info_get(unit, vp, NULL, &match_vlan, NULL);

    sal_memset(vent, 0, sizeof(vent));
    if (evx_mem == EGR_VLAN_XLATE_1_DOUBLEm) {
        soc_mem_field32_set(unit, evx_mem, vent, DATA_TYPEf, 1);
        soc_mem_field32_set(unit, evx_mem, vent, KEY_TYPEf, 1);
    } else {
        if (soc_mem_field_valid(unit, evx_mem, ENTRY_TYPEf)) {
            soc_mem_field32_set(unit, evx_mem, vent, ENTRY_TYPEf, 1);
        } else if (soc_mem_field_valid(unit, evx_mem, KEY_TYPEf)) {
            soc_mem_field32_set(unit, evx_mem, vent, KEY_TYPEf, 1);
        }
    }

    soc_mem_field32_set(unit, evx_mem, vent, DVPf, vp);
    soc_mem_field32_set(unit, evx_mem, vent, OVIDf, vlan);

    rv = soc_mem_search(unit, evx_mem, MEM_BLOCK_ALL, &idx,
                        vent, res_vent, 0);
    if (rv == SOC_E_NONE) {
        int vld;
        if (soc_feature(unit, soc_feature_base_valid)) {
            vld = soc_mem_field32_get(unit, evx_mem, res_vent,
                                      BASE_VALID_0f) == 3 &&
                  soc_mem_field32_get(unit, evx_mem, res_vent,
                                      BASE_VALID_1f) == 7;
        } else {
            vld = soc_mem_field32_get(unit, evx_mem, res_vent, VALIDf);
        }

        if (vld) {
            profile_idx = soc_mem_field32_get(unit, evx_mem, res_vent,
                                              TAG_ACTION_PROFILE_PTRf);
            _bcm_trx_egr_vlan_action_profile_entry_get(unit, &action, profile_idx);

            if (!BCM_VLAN_VALID(match_vlan)) {
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
 *      bcm_trident_niv_port_untagged_vlan_set
 * Purpose:
 *      Set the default VLAN ID for a NIV port.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - NIV gport.
 *      vid -  VLAN ID used for packets that ingress without a VLAN tag.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_trident_niv_port_untagged_vlan_set(int unit, bcm_port_t port,
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
    bcm_gport_t        gport;
    bcm_vlan_t         match_vlan;
    uint16             virtual_interface_id;
    int                tmp_vp;
    uint8              *vt_buf = NULL;
    int                index, index_min, index_max;
    vlan_xlate_entry_t *vent = NULL;
    int                action;
    int                cnt = 0;

    soc_mem_t          vx_mem = VLAN_XLATEm;

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        vx_mem = VLAN_XLATE_1_DOUBLEm;
    }

    /* Get VP */
    if (BCM_GPORT_IS_NIV_PORT(port)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(port);
    } else {
        return BCM_E_PORT;
    }
    
    if (!(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MATCH_DO_NOT_ADD)) {
        _bcm_trident_niv_match_info_get(unit, vp, &gport,
                                        &match_vlan, &virtual_interface_id);

        /* Construct lookup key */
        sal_memset(key_data, 0, sizeof(key_data));

        if (BCM_VLAN_VALID(match_vlan)) {
            BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                        VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
            soc_mem_field32_set(unit, vx_mem, key_data, VIF__VLANf, match_vlan);
        } else {
            BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                        VLXLT_HASH_KEY_TYPE_VIF, &key_type));
        }
        soc_mem_field32_set(unit, vx_mem, key_data, KEY_TYPEf, key_type);
        if (soc_mem_field_valid(unit, vx_mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, vx_mem, key_data, DATA_TYPEf, key_type);
        }

        soc_mem_field32_set(unit, vx_mem, key_data, VIF__SRC_VIFf,
                            virtual_interface_id);

        if (soc_mem_field_valid(unit, vx_mem, SOURCE_TYPEf)) {
            soc_mem_field32_set(unit, vx_mem, key_data, SOURCE_TYPEf, 1);
        }
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, gport,
                                    &mod_out, &port_out, &trunk_out, &tmp_id));
        if (BCM_GPORT_IS_TRUNK(gport)) {
            soc_mem_field32_set(unit, vx_mem, key_data, VIF__Tf, 1);
            soc_mem_field32_set(unit, vx_mem, key_data, VIF__TGIDf, trunk_out);
        } else {
            soc_mem_field32_set(unit, vx_mem, key_data, VIF__MODULE_IDf, mod_out);
            soc_mem_field32_set(unit, vx_mem, key_data, VIF__PORT_NUMf, port_out);
        }

        /* Lookup existing entry */
        SOC_IF_ERROR_RETURN(soc_mem_search(unit, vx_mem, MEM_BLOCK_ALL,
                    &entry_index, key_data, entry_data, 0));

        /* Replace entry's new VLAN */
        soc_mem_field32_set(unit, vx_mem, entry_data, VIF__NEW_OVIDf, vid);

        /* Insert new entry */
        rv = soc_mem_insert(unit, vx_mem, MEM_BLOCK_ALL, entry_data);
        if (rv == SOC_E_EXISTS) {
            rv = BCM_E_NONE;
        } else {
            return BCM_E_INTERNAL;
        }
    } else {
        vt_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, vx_mem),
                               "VLAN_XLATE buffer");

        if (NULL == vt_buf) {
            return BCM_E_MEMORY;
        }

        index_min = soc_mem_index_min(unit, vx_mem);
        index_max = soc_mem_index_max(unit, vx_mem);

        soc_mem_lock(unit, vx_mem);
        if ((rv = soc_mem_read_range(unit, vx_mem, MEM_BLOCK_ANY,
                                     index_min, index_max, vt_buf)) < 0) {
            soc_cm_sfree(unit, vt_buf);
            soc_mem_unlock(unit, vx_mem);
            return rv;
        }

        for (index = index_min; index <= index_max; index++) {
            vent = soc_mem_table_idx_to_pointer(unit, vx_mem,
                                                void *,
                                                vt_buf, index);
            if (soc_feature(unit, soc_feature_base_valid)) {
                if ((soc_mem_field32_get(unit, vx_mem, vent, BASE_VALID_0f) != 3) ||
                         (soc_mem_field32_get(unit, vx_mem, vent, BASE_VALID_1f) != 7)) {
                    continue;
                }
            } else {
                if (soc_mem_field32_get(unit, vx_mem, vent, VALIDf) == 0) {
                    continue;
                }
            }
            action = soc_mem_field32_get(unit, vx_mem, vent, VIF__MPLS_ACTIONf);
            if (action != 1) {
                /* Entry is not for an SVP */
                continue;
            }
            tmp_vp = soc_mem_field32_get(unit, vx_mem, vent, VIF__SOURCE_VPf);
            if (vp != tmp_vp) {
                continue;
            }
            /* Replace entry's new VLAN */
            soc_mem_field32_set(unit, vx_mem, vent, VIF__NEW_OVIDf, vid);
            cnt++;
        }

        if (cnt == 0) {
            soc_cm_sfree(unit, vt_buf);
            soc_mem_unlock(unit, vx_mem);
            return BCM_E_NOT_FOUND;
        }

        rv = soc_mem_write_range(unit, vx_mem, MEM_BLOCK_ALL, index_min,
                                 index_max, vt_buf);
        soc_cm_sfree(unit, vt_buf);
        soc_mem_unlock(unit, vx_mem);
    }

    return rv;
}

/*
 * Function:
 *      bcm_trident_niv_port_untagged_vlan_get
 * Purpose:
 *      Get the default VLAN ID for a NIV port.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - NIV gport.
 *      vid  - (OUT) VLAN ID used for packets that ingress without a VLAN tag.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_trident_niv_port_untagged_vlan_get(int unit, bcm_port_t port,
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
    bcm_gport_t        gport;
    bcm_vlan_t         match_vlan;
    uint16             virtual_interface_id;
    soc_mem_t          vx_mem = VLAN_XLATEm;
    int                tmp_vp;
    uint8              *vt_buf = NULL;
    int                index, index_min, index_max;
    vlan_xlate_entry_t *vent = NULL;
    int                action;

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        vx_mem = VLAN_XLATE_1_DOUBLEm;
    }

    /* Get VP */
    if (BCM_GPORT_IS_NIV_PORT(port)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(port);
    } else {
        return BCM_E_PORT;
    }

    if (!(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MATCH_DO_NOT_ADD)) {
        /* Construct lookup key */
        sal_memset(key_data, 0, sizeof(key_data));
        _bcm_trident_niv_match_info_get(unit, vp, &gport,
                                        &match_vlan, &virtual_interface_id);

        if (BCM_VLAN_VALID(match_vlan)) {
            BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                        VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
            soc_mem_field32_set(unit, vx_mem, key_data, VIF__VLANf, match_vlan);
        } else {
            BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                        VLXLT_HASH_KEY_TYPE_VIF, &key_type));
        }
        soc_mem_field32_set(unit, vx_mem, key_data, KEY_TYPEf, key_type);
        if (soc_mem_field_valid(unit, vx_mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, vx_mem, key_data, DATA_TYPEf, key_type);
        }

        soc_mem_field32_set(unit, vx_mem, key_data, VIF__SRC_VIFf,
                                    virtual_interface_id);

        if (soc_mem_field_valid(unit, vx_mem, SOURCE_TYPEf)) {
            soc_mem_field32_set(unit, vx_mem, key_data, SOURCE_TYPEf, 1);
        }
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, gport,
                                    &mod_out, &port_out, &trunk_out, &tmp_id));
        if (BCM_GPORT_IS_TRUNK(gport)) {
            soc_mem_field32_set(unit, vx_mem, key_data, VIF__Tf, 1);
            soc_mem_field32_set(unit, vx_mem, key_data, VIF__TGIDf, trunk_out);
        } else {
            soc_mem_field32_set(unit, vx_mem, key_data, VIF__MODULE_IDf, mod_out);
            soc_mem_field32_set(unit, vx_mem, key_data, VIF__PORT_NUMf, port_out);
        }

        /* Lookup existing entry */
        SOC_IF_ERROR_RETURN(soc_mem_search(unit, vx_mem, MEM_BLOCK_ALL,
                    &entry_index, key_data, entry_data, 0));

        /* Get entry's VLAN */
        *vid = soc_mem_field32_get(unit, vx_mem, entry_data, VIF__NEW_OVIDf);
    } else {
        vt_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, vx_mem),
                               "VLAN_XLATE buffer");

        if (NULL == vt_buf) {
            return BCM_E_MEMORY;
        }

        index_min = soc_mem_index_min(unit, vx_mem);
        index_max = soc_mem_index_max(unit, vx_mem);

        soc_mem_lock(unit, vx_mem);
        if ((rv = soc_mem_read_range(unit, vx_mem, MEM_BLOCK_ANY,
                                     index_min, index_max, vt_buf)) < 0) {
            soc_cm_sfree(unit, vt_buf);
            soc_mem_unlock(unit, vx_mem);
            return rv;
        }

        for (index = index_min; index <= index_max; index++) {
            vent = soc_mem_table_idx_to_pointer(unit, vx_mem,
                                                void *,
                                                vt_buf, index);
            if (soc_mem_field32_get(unit, vx_mem, vent, VALIDf) == 0) {
                continue;
            }
            action = soc_mem_field32_get(unit, vx_mem, vent, VIF__MPLS_ACTIONf);
            if (action != 1) {
                /* Entry is not for an SVP */
                continue;
            }
            tmp_vp = soc_mem_field32_get(unit, vx_mem, vent, VIF__SOURCE_VPf);
            if (vp != tmp_vp) {
                continue;
            }
            /* Get entry's VLAN */
            *vid = soc_mem_field32_get(unit, vx_mem, vent, VIF__NEW_OVIDf);
            break;
        }
        soc_cm_sfree(unit, vt_buf);
        soc_mem_unlock(unit, vx_mem);
        if (index > index_max) {
            return BCM_E_NOT_FOUND;
        }
    }

    return rv;
}

#ifdef BCM_TRIDENT2_SUPPORT

/*
 * Function:
 *      _bcm_td2_niv_match_vp_replace
 * Purpose:
 *      Replace VP value in NIV VP's match entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vp - (IN) NIV VP whose match entry is being replaced.
 *      new_vp - (IN) New VP value.
 *      old_vp - (OUT) Old VP value.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_td2_niv_match_vp_replace(int unit, int vp, int new_vp, int *old_vp)
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
    bcm_gport_t        port;
    bcm_vlan_t         match_vlan;
    uint16             vtl_id;
    soc_mem_t          mem = VLAN_XLATEm;

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    /* Construct lookup key */
    sal_memset(key_data, 0, sizeof(key_data));
    _bcm_trident_niv_match_info_get(unit, vp, &port, &match_vlan, &vtl_id);
    if (BCM_VLAN_VALID(match_vlan)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_mem_field32_set(unit, mem, key_data, VIF__VLANf, match_vlan);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    }
    soc_mem_field32_set(unit, mem, key_data, KEY_TYPEf, key_type);
    if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, mem, key_data, DATA_TYPEf, key_type);
    }

    soc_mem_field32_set(unit, mem, key_data, VIF__SRC_VIFf, vtl_id);

    if (soc_mem_field_valid(unit, mem, SOURCE_TYPEf)) {
        soc_mem_field32_set(unit, mem, key_data, SOURCE_TYPEf, 1);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(port)) {
        soc_mem_field32_set(unit, mem, key_data, VIF__Tf, 1);
        soc_mem_field32_set(unit, mem, key_data, VIF__TGIDf, trunk_out);
    } else {
        soc_mem_field32_set(unit, mem, key_data, VIF__MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, mem, key_data, VIF__PORT_NUMf, port_out);
    }

    /* Lookup existing entry */
    SOC_IF_ERROR_RETURN(soc_mem_search(unit, mem, MEM_BLOCK_ALL,
                &entry_index, key_data, entry_data, 0));

    /* Replace entry's VP value */
    *old_vp = soc_mem_field32_get(unit, mem, entry_data, VIF__SOURCE_VPf);
    soc_mem_field32_set(unit, mem, entry_data, VIF__SOURCE_VPf, new_vp);

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
 *      bcm_td2_niv_port_source_vp_lag_set
 * Purpose:
 *      Set source VP LAG for a NIV virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) NIV virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_niv_port_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
    int vp, old_vp;

    if (!BCM_GPORT_IS_NIV_PORT(gport)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_PARAM;
    }

    /* Set source VP LAG by replacing the SVP field in NIV VP's
     * match entry with the VP value representing the VP LAG.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td2_niv_match_vp_replace(unit, vp, vp_lag_vp, &old_vp));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_niv_port_source_vp_lag_clear
 * Purpose:
 *      Clear source VP LAG for a NIV virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) NIV virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_niv_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
    int vp, old_vp;

    if (!BCM_GPORT_IS_NIV_PORT(gport)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_PARAM;
    }

    /* Clear source VP LAG by replacing the SVP field in NIV VP's
     * match entry with the VP value.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td2_niv_match_vp_replace(unit, vp, vp, &old_vp));

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
 *      bcm_td2_niv_port_source_vp_lag_get
 * Purpose:
 *      Set source VP LAG for a NIV virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) NIV virtual port GPORT ID. 
 *      vp_lag_vp - (OUT) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_niv_port_source_vp_lag_get(int unit, bcm_gport_t gport,
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
    bcm_gport_t        port;
    bcm_vlan_t         match_vlan;
    uint16             vtl_id;
    soc_mem_t          mem = VLAN_XLATEm;

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    if (!BCM_GPORT_IS_NIV_PORT(gport)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_PARAM;
    }

    /* Construct match entry lookup key */
    sal_memset(key_data, 0, sizeof(key_data));
    _bcm_trident_niv_match_info_get(unit, vp, &port, &match_vlan, &vtl_id);

    if (BCM_VLAN_VALID(match_vlan)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_mem_field32_set(unit, mem, key_data, VIF__VLANf, match_vlan);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    }
    soc_mem_field32_set(unit, mem, key_data, KEY_TYPEf, key_type);
    if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
        soc_mem_field32_set(unit, mem, key_data, DATA_TYPEf, key_type);
    }

    soc_mem_field32_set(unit, mem, key_data, VIF__SRC_VIFf, vtl_id);

    if (soc_mem_field_valid(unit, mem, SOURCE_TYPEf)) {
        soc_mem_field32_set(unit, mem, key_data, SOURCE_TYPEf, 1);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(port)) {
        soc_mem_field32_set(unit, mem, key_data, VIF__Tf, 1);
        soc_mem_field32_set(unit, mem, key_data, VIF__TGIDf, trunk_out);
    } else {
        soc_mem_field32_set(unit, mem, key_data, VIF__MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, mem, key_data, VIF__PORT_NUMf, port_out);
    }

    /* Lookup existing entry */
    SOC_IF_ERROR_RETURN(soc_mem_search(unit, mem, MEM_BLOCK_ALL,
                &entry_index, key_data, entry_data, 0));

    /* Get VP value representing VP LAG */
    *vp_lag_vp = soc_mem_field32_get(unit, mem, entry_data, VIF__SOURCE_VPf);
    if (!_bcm_vp_used_get(unit, *vp_lag_vp, _bcmVpTypeVpLag)) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

#endif /* BCM_TRIDENT2_SUPPORT */

/* Function:
 *      _bcm_trident_local_port_match
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
_bcm_trident_local_port_match(int unit, bcm_gport_t gport1,
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

/*
 * Function:
 *      bcm_trident_niv_port_learn_set
 * Purpose:
 *      Set the CML bits for an niv port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_port_learn_set(int unit, bcm_gport_t niv_port_id, 
                                uint32 flags)
{
    int vp, cml = 0, rv = BCM_E_NONE, entry_type;
    source_vp_entry_t svp;

    rv = _bcm_niv_check_init(unit);

    if (rv != BCM_E_NONE){
        return rv;
    }

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
    vp = BCM_GPORT_NIV_PORT_ID_GET(niv_port_id);

    MEM_LOCK(unit, SOURCE_VPm);
    /* Be sure the entry is used and is set for MIM */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return BCM_E_NOT_FOUND;
    }
    /* coverity[negative_returns] */
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return rv;
    }

    /* Ensure that the entry_type is either 3 or 1. The ENTRYP_TYPE is
     * modified from 3 to 1 when a NIV port is added to a VXLAN VPN via
     * bcm_vxlan_port_add. 
     */
    entry_type = soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf);
    if ((entry_type != 3) && ((entry_type != 1))){
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
 *      bcm_trident_niv_port_learn_get
 * Purpose:
 *      Get the CML bits for an mpls port
 * Returns: 
 *      BCM_E_XXX
 */     
int     
bcm_trident_niv_port_learn_get(int unit, bcm_gport_t niv_port_id, 
                                uint32 *flags)
{
    int rv, vp, cml = 0, entry_type;
    source_vp_entry_t svp;
    
    rv = _bcm_niv_check_init(unit);

    if (rv != BCM_E_NONE){
        return rv;
    }
	    
    /* Get the VP index from the gport */
    vp = BCM_GPORT_NIV_PORT_ID_GET(niv_port_id);
    if (vp == -1) {
       return BCM_E_PARAM;
    }

    /* Be sure the entry is used and is set for NIV */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_NOT_FOUND;
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        return rv;
    }

    /* Ensure that the entry_type is either 3 or 1. The ENTRYP_TYPE is
     * modified from 3 to 1 when a NIV port is added to a VXLAN VPN via
     * bcm_vxlan_port_add. 
     */
    entry_type = soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf);
    if ((entry_type != 3) && ((entry_type != 1))){
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

/* Function:
 *      _bcm_trident_niv_egress_match_ex_port
 * Purpose:
 *      Determine if two NIV egress objects' attributes match, except for
 *      port.
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      niv_egress1 - (IN) NIV egress object 1, represented by
 *                         bcm_niv_egress_t.
 *      niv_egress2 - (IN) NIV egress object 2, represented by
 *                         _bcm_trident_niv_egress_t.
 *      match - (OUT) Match indication
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_trident_niv_egress_match_ex_port(int unit,
        bcm_niv_egress_t *niv_egress1,
        _bcm_trident_niv_egress_t *niv_egress2,
        int *match)
{
    int flags_match, vif_match, vlan_match = 1;
    int service_tpid_match = 1, service_vlan_match = 1;
    int service_pri_match = 1, service_cfi_match = 1;
    int service_qos_map_id_match = 1, l3_intf_match = 1;

    flags_match = (niv_egress1->flags == niv_egress2->flags);
    vif_match   = (niv_egress1->virtual_interface_id ==
                   niv_egress2->virtual_interface_id);
    if (niv_egress1->flags & BCM_NIV_EGRESS_L3) {
        l3_intf_match = (niv_egress1->intf == niv_egress2->intf);
    } else {
        vlan_match  = (niv_egress1->match_vlan == niv_egress2->match_vlan);
        service_tpid_match = (niv_egress1->service_tpid == niv_egress2->service_tpid);
        service_vlan_match = (niv_egress1->service_vlan == niv_egress2->service_vlan);
        service_pri_match  = (niv_egress1->service_pri  == niv_egress2->service_pri);
        service_cfi_match  = (niv_egress1->service_cfi  == niv_egress2->service_cfi);
        service_qos_map_id_match = (niv_egress1->service_qos_map_id ==
                                   niv_egress2->service_qos_map_id);
    }

    if (flags_match && vif_match && vlan_match && service_tpid_match &&
            service_vlan_match && service_pri_match && service_cfi_match &&
            service_qos_map_id_match && l3_intf_match) {
        *match = TRUE;
    } else {
        *match = FALSE;
    }

    return BCM_E_NONE;
}

/* 
 * Function:
 *      _bcm_trident_niv_egress_pbmp_get
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
_bcm_trident_niv_egress_pbmp_get(int unit, int vp, bcm_pbmp_t *pbmp)
{
    _bcm_trident_niv_nh_info_t  *nh_info;
    BCM_PBMP_CLEAR(*pbmp);

    nh_info = NIV_PORT_MC_NH_INFO(unit, vp);
    while (nh_info) {
        BCM_PBMP_PORT_ADD(*pbmp, nh_info->port);
        nh_info = nh_info->next;
    }

    return;
}

/* 
 * Function:
 *      _bcm_trident_niv_egress_pbmp_delete
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
_bcm_trident_niv_egress_pbmp_delete(int unit, int vp, bcm_pbmp_t pbmp)
{
    _bcm_trident_niv_nh_info_t      *ni_pre, *ni_cur, *ni_del;

    ni_cur  = NIV_PORT_MC_NH_INFO(unit, vp);
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
        if (ni_cur == NIV_PORT_MC_NH_INFO(unit, vp)) {
            NIV_PORT_MC_NH_INFO(unit, vp) = ni_cur->next;
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
 *      _bcm_trident_niv_egress_pbmp_add
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
_bcm_trident_niv_egress_pbmp_add(int unit, int vp, bcm_pbmp_t pbmp)
{
    bcm_port_t                      port;
    int                             alloc_size;
    _bcm_trident_niv_nh_info_t      *nh_info;
    bcm_pbmp_t                      sucess_pbmp; 
    
    nh_info = NIV_PORT_MC_NH_INFO(unit, vp);
    while (nh_info) {
        if (BCM_PBMP_MEMBER(pbmp, nh_info->port)) {
            nh_info->nh_ref_cnt++;
            BCM_PBMP_PORT_REMOVE(pbmp, nh_info->port);
        }
        nh_info = nh_info->next;
    }

    BCM_PBMP_CLEAR(sucess_pbmp);
    alloc_size = sizeof(_bcm_trident_niv_nh_info_t);
    BCM_PBMP_ITER(pbmp, port) {
        nh_info = sal_alloc(alloc_size, "NIV Egress Next Hop Info");
        if (NULL == nh_info) {
            _bcm_trident_niv_egress_pbmp_delete(unit, vp, sucess_pbmp);
            return BCM_E_MEMORY;
        }
        sal_memset(nh_info, 0, alloc_size);
        nh_info->port = port;
        nh_info->nh_ref_cnt = 1;
        nh_info->next = NIV_PORT_MC_NH_INFO(unit, vp);
        NIV_PORT_MC_NH_INFO(unit, vp) = nh_info;
        BCM_PBMP_PORT_ADD(sucess_pbmp, port);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trident_niv_egress_pbmp_clear
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
_bcm_trident_niv_egress_pbmp_clear(int unit, int vp)
{
    _bcm_trident_niv_nh_info_t  *cur_node, *next_node;

    next_node = NULL;
    cur_node = NIV_PORT_MC_NH_INFO(unit, vp);
    while (cur_node) {
        next_node = cur_node->next;
        sal_free(cur_node);
        cur_node = next_node;
    }
    NIV_PORT_MC_NH_INFO(unit, vp) = NULL;

    return;
}

/*
 * Function:
 *      _bcm_trident_niv_egress_hash_calc
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
_bcm_trident_niv_egress_hash_calc(int unit, void *hash_key, uint16 *hash)
{
    uint32 sw_buf[7];
    int size;
    int buf_idx = 0;
    bcm_niv_egress_t *niv_egress;

    /* Input parameters check. */
    if ((NULL == hash_key) || (NULL == hash)) {
        return (BCM_E_PARAM);
    }
    size = sizeof(sw_buf);
    sal_memset(sw_buf, 0, size);
    niv_egress = (bcm_niv_egress_t *)hash_key;

    /*
     * Copy entry information to a temporary buffer, so we can
     * mask some fields we don't want to include in hash calculation.
     */
    sw_buf[buf_idx++] = niv_egress->flags;
    sw_buf[buf_idx++] = niv_egress->intf;
    sw_buf[buf_idx++] = niv_egress->virtual_interface_id << 16;
    if (!(niv_egress->flags & BCM_NIV_EGRESS_L3)) {
        sw_buf[buf_idx++] = ((niv_egress->service_tpid << 16) | 
                              niv_egress->service_vlan);
        sw_buf[buf_idx++] = ((niv_egress->service_pri << 16) | 
                              niv_egress->service_cfi);
        sw_buf[buf_idx++] = niv_egress->service_qos_map_id;
    }

    size = sizeof(uint32) * buf_idx;
    *hash = _shr_crc16(0, (uint8 *)&sw_buf[0], size);

    return BCM_E_NONE;
}

/* Function:
 *      bcm_trident_niv_egress_add
 * Purpose:
 *      Add a NIV egress object to a NIV port
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      niv_port - (IN) NIV port
 *      niv_egress - (IN/OUT) NIV egress object, the egress_if field is output.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_egress_add(
    int unit, 
    bcm_gport_t niv_port, 
    bcm_niv_egress_t *niv_egress)
{
    int vp;
    int match_ex_port;
    int local_port_match;
    int shared_nh_found;
    bcm_pbmp_t existing_pbmp, new_pbmp, update_pbmp;
    _bcm_trident_niv_egress_t *curr_ptr = NULL, *temp_ptr = NULL;
    int nh_index;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t tgid_out;
    int id_out;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count;
    int i;
    int mod_local = FALSE;
    bcm_l3_egress_t nh_info;
    uint32 nh_flags;
    bcm_port_t local_port;
    bcm_gport_t gport;
    _bcm_trident_niv_sd_tag_t niv_sd_tag;
    bcm_if_t intf;
    int      alloc_size;
    uint16   hash_val;
    int      bkt_idx = 0;
    int      multicast;
#ifdef BCM_TRIDENT2_SUPPORT    
    int vp_routing = FALSE;
#endif
#if defined(BCM_HGPROXY_COE_SUPPORT)
    int is_local_subport = FALSE;
#endif


    /* Verify niv_port is a valid NIV port with no match criteria */
    if (!BCM_GPORT_IS_NIV_PORT(niv_port)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_NIV_PORT_ID_GET(niv_port);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_NOT_FOUND;
    }
    if (!(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MATCH_NONE)) {
        return BCM_E_PARAM;
    }

    if (niv_egress == NULL) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_virtual_port_routing)) {
        vp_routing = TRUE;    
    }
#endif

    if (!(niv_egress->flags & BCM_NIV_EGRESS_MULTICAST)) {
        /* When adding a unicast NIV egress object to a NIV port,
         * check that there are no other NIV egress objects on the
         * NIV port's egress_list. A NIV port cannot be overloaded
         * with multiple unicast NIV egress objects.
         */
        if (NULL != NIV_PORT_INFO(unit, vp)->egress_list) {
            return BCM_E_CONFIG;
        }
        /* BCM_NIV_EGRESS_L3 must be with BCM_NIV_EGRESS_MULTICAST */
        if (niv_egress->flags & BCM_NIV_EGRESS_L3) {
            return BCM_E_PARAM;
        }
    } else {
        /* For a multicast NIV egress object, the field
         * match_vlan is not applicable.
         */
        if (niv_egress->match_vlan) {
           return BCM_E_PARAM;
        } 
#ifdef BCM_TRIDENT2_SUPPORT
        if (vp_routing) {
        } else
#endif
        {
            if (niv_egress->flags & BCM_NIV_EGRESS_L3) {
                return BCM_E_UNAVAIL;
            }
        }
       
    }

    BCM_PBMP_CLEAR(new_pbmp);
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, niv_egress->port,
                &mod_out, &port_out, &tgid_out, &id_out));
    if (BCM_TRUNK_INVALID != tgid_out) {
        BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit,
                    tgid_out, SOC_MAX_NUM_PORTS, local_member_array,
                    &local_member_count));

        /*
         * For multicast, it will be unnecessary to create next hop
         * if there is no local port member. 
         */
        if (niv_egress->flags & BCM_NIV_EGRESS_MULTICAST) {
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
                unit, niv_egress->port, mod_out, port_out,
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
        if (niv_egress->flags & BCM_NIV_EGRESS_MULTICAST) {
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

    /* Traverse NIV port's existing list of NIV egress objects.
     * If an existing egress object has the same multicast flag,
     * virtual_interface_id, match_vlan, and SD-tag but different port,
     * its next hop index can be shared with the new NIV egress object.
     * Also, create a bitmap of the physical ports on which the VP resides.
     */ 
    BCM_PBMP_CLEAR(existing_pbmp);
    shared_nh_found = FALSE;
    curr_ptr = NIV_PORT_INFO(unit, vp)->egress_list;
    if (niv_egress->flags & BCM_NIV_EGRESS_MULTICAST) {
        BCM_IF_ERROR_RETURN(
            _bcm_trident_niv_egress_hash_calc(unit, (void *)niv_egress,
                                              &hash_val));
        bkt_idx  = hash_val % TD_NIV_PORT_HASH_BKT_SIZE;
        if (NIV_PORT_INFO(unit, vp)->egress_list != NULL) {
            curr_ptr = NIV_PORT_HASH_BKT(unit, vp, bkt_idx)->next;
        }
    }
    while (NULL != curr_ptr) {
        /* Determine if the attributes of the current NIV egress object match
         * those of the new NIV egress object, except for the port attribute.
         */ 
        BCM_IF_ERROR_RETURN(_bcm_trident_niv_egress_match_ex_port(unit,
                    niv_egress, curr_ptr, &match_ex_port));
        if (match_ex_port) {
            /* Check if ports also match. If so, the NIV egress object
             * already exists.
             */
            BCM_IF_ERROR_RETURN(_bcm_trident_local_port_match(unit,
                        niv_egress->port, curr_ptr->port, &local_port_match));
            if (local_port_match) {
                return BCM_E_EXISTS;
            }

            /* If ports don't match, the next hop index can be shared. */
            shared_nh_found = TRUE;
            nh_index = curr_ptr->next_hop_index;
            break;
        }
        /* Advance to next NIV egress object */
        curr_ptr = curr_ptr->next;
    }

    if (!shared_nh_found) {
        /* If match not found, allocate a new next hop index. */
        bcm_l3_egress_t_init(&nh_info);
        nh_flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE;
        BCM_IF_ERROR_RETURN
            (bcm_xgs3_nh_add(unit, nh_flags, &nh_info, &nh_index));

        /* Configure next hop entry */
#ifdef BCM_TRIDENT2_SUPPORT
        if (niv_egress->flags & BCM_NIV_EGRESS_L3) {
            /* Configure next hop entry for L3MC */ 
            BCM_IF_ERROR_RETURN
                (_bcm_trident_niv_l3mc_nh_info_set(unit, nh_index, 
                    niv_egress->flags, niv_egress->multicast_flags, 
                    niv_egress->port, niv_egress->intf, 
                    niv_egress->virtual_interface_id, vp, 
                    (niv_egress->flags & BCM_NIV_VNTAG_L_BIT_FORCE_1) ? 1 : 0));
        }
        else 
#endif
        {
            sal_memset(&niv_sd_tag, 0, sizeof(niv_sd_tag));
            niv_sd_tag.flags = niv_egress->flags;
            niv_sd_tag.service_tpid = niv_egress->service_tpid;
            niv_sd_tag.service_vlan = niv_egress->service_vlan;
            niv_sd_tag.service_pri  = niv_egress->service_pri;
            niv_sd_tag.service_cfi  = niv_egress->service_cfi;
            niv_sd_tag.service_qos_map_id = niv_egress->service_qos_map_id;
            multicast = (niv_egress->flags & BCM_NIV_EGRESS_MULTICAST) ?
                            ((niv_egress->flags & BCM_NIV_EGRESS_P_BIT_CLEAR) ?
                                 0 : 1) : 0;
            BCM_IF_ERROR_RETURN
                (_bcm_trident_niv_nh_info_set(unit, nh_index, multicast,
                    niv_egress->port, niv_egress->virtual_interface_id,
                    niv_egress->match_vlan, &niv_sd_tag, vp, 
                    (niv_egress->flags & BCM_NIV_VNTAG_L_BIT_FORCE_1) ? 1 : 0));
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
     * NIV egress object.
     */
    if (!(niv_egress->flags & BCM_NIV_EGRESS_MULTICAST)) {
        BCM_IF_ERROR_RETURN(_bcm_trident_niv_match_add(unit,
                    niv_egress->port, niv_egress->virtual_interface_id,
                    niv_egress->match_vlan, vp));
    }

    /* Increment the VP count on the new NIV egress object's physical
     * port(s), unless the physical port(s) match those of VP's
     * other NIV egress objects.
     */
    
    if (niv_egress->flags & BCM_NIV_EGRESS_MULTICAST) {
        _bcm_trident_niv_egress_pbmp_get(unit, vp, &existing_pbmp);
    }
    BCM_PBMP_CLEAR(update_pbmp);
    BCM_PBMP_ASSIGN(update_pbmp, new_pbmp);
    BCM_PBMP_REMOVE(update_pbmp, existing_pbmp);
    BCM_PBMP_ITER(update_pbmp, local_port) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_gport_get(unit, local_port, &gport));
        BCM_IF_ERROR_RETURN
            (_bcm_trident_niv_port_cnt_update(unit, gport, vp, TRUE, FALSE));
    }

    if (niv_egress->flags & BCM_NIV_EGRESS_MULTICAST) {
        BCM_IF_ERROR_RETURN(
            _bcm_trident_niv_egress_pbmp_add(unit, vp, new_pbmp));
    }

    /* Insert the new NIV egress object into VP's egress object list */
    curr_ptr = sal_alloc(sizeof(_bcm_trident_niv_egress_t),
            "NIV egress object");
    if (NULL == curr_ptr) {
        return BCM_E_MEMORY;
    }
    sal_memset(curr_ptr, 0, sizeof(_bcm_trident_niv_egress_t));
    curr_ptr->flags = niv_egress->flags;
    curr_ptr->port = niv_egress->port;
    curr_ptr->virtual_interface_id = niv_egress->virtual_interface_id;
    curr_ptr->match_vlan = niv_egress->match_vlan;
    curr_ptr->service_tpid = niv_egress->service_tpid;
    curr_ptr->service_vlan = niv_egress->service_vlan;
    curr_ptr->service_pri = niv_egress->service_pri;
    curr_ptr->service_cfi = niv_egress->service_cfi;
    curr_ptr->service_qos_map_id = niv_egress->service_qos_map_id;
    curr_ptr->next_hop_index = nh_index;
    curr_ptr->intf = niv_egress->intf;
    curr_ptr->multicast_flags = niv_egress->multicast_flags;
    if (!(niv_egress->flags & BCM_NIV_EGRESS_MULTICAST)) {
        curr_ptr->next = NIV_PORT_INFO(unit, vp)->egress_list;
        NIV_PORT_INFO(unit, vp)->egress_list = curr_ptr;
    } else {
        if ((NIV_PORT_INFO(unit, vp)->egress_list == NULL)) {
            alloc_size = sizeof(_bcm_trident_niv_egress_t) *
                         TD_NIV_PORT_HASH_BKT_SIZE;
            temp_ptr = sal_alloc(alloc_size, "NIV egress object");
            if (NULL == temp_ptr) {
                sal_free(curr_ptr);
                return BCM_E_MEMORY;
            }
            sal_memset(temp_ptr, 0, alloc_size);
            NIV_PORT_INFO(unit, vp)->egress_list = temp_ptr;
        }
        curr_ptr->next = NIV_PORT_HASH_BKT(unit, vp, bkt_idx)->next;
        NIV_PORT_HASH_BKT(unit, vp, bkt_idx)->next = curr_ptr;
    }

    /* Set NIV egress object ID */
    if (niv_egress->flags & BCM_NIV_EGRESS_L3) {
        niv_egress->egress_if = nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    }
    else {    
        niv_egress->egress_if = nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
    }

    return BCM_E_NONE;
}

/* Function:
 *      bcm_trident_niv_egress_delete
 * Purpose:
 *      Delete a NIV egress object from a NIV port
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      niv_port - (IN) NIV port
 *      niv_egress - (IN) NIV egress object
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_egress_delete(
    int unit, 
    bcm_gport_t niv_port, 
    bcm_niv_egress_t *niv_egress)
{
    int vp;
    int match_found;
    bcm_pbmp_t remaining_pbmp, pbmp;
    _bcm_trident_niv_egress_t *curr_ptr, *prev_ptr, **head;
    _bcm_trident_niv_egress_t *match_ptr = NULL;
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

    /* Verify niv_port is a valid NIV port with no match criteria */
    if (!BCM_GPORT_IS_NIV_PORT(niv_port)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_NIV_PORT_ID_GET(niv_port);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_NOT_FOUND;
    }
    if (!(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MATCH_NONE)) {
        return BCM_E_PARAM;
    }

    if (niv_egress == NULL) {
        return BCM_E_PARAM;
    }

    /* Traverse NIV port's list of NIV egress objects to find the egress
     * object to be deleted. Also, among the remaining egress objects,
     * create a bitmap of the physical ports on which the VP resides.
     */ 
    match_found = FALSE;
    BCM_PBMP_CLEAR(remaining_pbmp);
    prev_ptr = NULL;
    if (niv_egress->flags & BCM_NIV_EGRESS_MULTICAST) {
        if ((NIV_PORT_INFO(unit, vp)->egress_list == NULL)) {
            return BCM_E_NOT_FOUND;
        }
        BCM_IF_ERROR_RETURN(
            _bcm_trident_niv_egress_hash_calc(unit, (void *)niv_egress,
                                              &hash_val));
        bkt_idx  = hash_val % TD_NIV_PORT_HASH_BKT_SIZE;
        head = &(NIV_PORT_HASH_BKT(unit, vp, bkt_idx)->next);
    } else {
        head = &(NIV_PORT_INFO(unit, vp)->egress_list);
    }
    curr_ptr = *head;
    while (NULL != curr_ptr) {
        BCM_IF_ERROR_RETURN(_bcm_trident_niv_egress_match_ex_port(unit,
                    niv_egress, curr_ptr, &match_ex_port));
        BCM_IF_ERROR_RETURN(_bcm_trident_local_port_match(unit,
                    niv_egress->port, curr_ptr->port, &local_port_match));
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

        /* Advance to next NIV egress object */
        curr_ptr = curr_ptr->next;
    }
    if (!match_found) {
        return BCM_E_NOT_FOUND;
    }

    /* Delete ingress VLAN translation entry */
    if (!(match_ptr->flags & BCM_NIV_EGRESS_MULTICAST)) {
        BCM_IF_ERROR_RETURN(_bcm_trident_niv_match_delete(unit,
                    match_ptr->port, match_ptr->virtual_interface_id,
                    match_ptr->match_vlan, NULL));
    }

    /* If the VP's list of remaining egress objects is empty, modify VP's
     * next hop index to invalid next hop index. If not empty,
     * pick one of the next hop indices in the list.
     */
    if (NULL == *head) {
        nh_index = NIV_INFO(unit)->invalid_next_hop_index;
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
        (_bcm_trident_niv_nh_info_delete(unit, match_ptr->next_hop_index));

    /* Decrement the VP count on the deleted NIV egress object's physical
     * port(s), unless the physical port(s) are shared by the VP's remaining
     * NIV egress objects.
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
                unit, niv_egress->port, mod_out, port_out,
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
    if (niv_egress->flags & BCM_NIV_EGRESS_MULTICAST) {
        _bcm_trident_niv_egress_pbmp_delete(unit, vp, pbmp);
        _bcm_trident_niv_egress_pbmp_get(unit, vp, &remaining_pbmp);
    }
    BCM_PBMP_REMOVE(pbmp, remaining_pbmp);
    BCM_PBMP_ITER(pbmp, local_port) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_gport_get(unit, local_port, &gport));
        BCM_IF_ERROR_RETURN
            (_bcm_trident_niv_port_cnt_update(unit, gport, vp, FALSE, FALSE));
    }
    if ((niv_egress->flags & BCM_NIV_EGRESS_MULTICAST) &&
         BCM_PBMP_IS_NULL(remaining_pbmp)) {
         sal_free(NIV_PORT_INFO(unit, vp)->egress_list);
         NIV_PORT_INFO(unit, vp)->egress_list = NULL;
    }
    /* Free the deleted NIV egress object */
    sal_free(match_ptr);

    return BCM_E_NONE;
}

/* Function:
 *      bcm_trident_niv_egress_set
 * Purpose:
 *      Set an array of NIV egress objects for a NIV port
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      niv_port - (IN) NIV port
 *      array_size - (IN) Number of NIV egress objects
 *      niv_egress_array - (IN/OUT) Array of NIV egress objects. Each NIV
 *                                  egress structure's egress_if field is
 *                                  an output.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_egress_set(
    int unit, 
    bcm_gport_t niv_port, 
    int array_size, 
    bcm_niv_egress_t *niv_egress_array)
{
    int vp;
    _bcm_trident_niv_egress_t *curr_ptr, *next_ptr;
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

    /* Verify niv_port is a valid NIV port with no match criteria */
    if (!BCM_GPORT_IS_NIV_PORT(niv_port)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_NIV_PORT_ID_GET(niv_port);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_NOT_FOUND;
    }
    if (!(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MATCH_NONE)) {
        return BCM_E_PARAM;
    }

    if ((array_size > 0) && (niv_egress_array == NULL)) {
        return BCM_E_PARAM;
    }

    /* Modify VP's next hop index to invalid next hop index */
    intf = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit) + NIV_INFO(unit)->invalid_next_hop_index;
    BCM_IF_ERROR_RETURN(_bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigUpdate,
                        vp, ING_DVP_CONFIG_INVALID_VP_TYPE, intf, 
                        ING_DVP_CONFIG_INVALID_PORT_TYPE));

    /* Delete the existing NIV egress objects attached to the given NIV port */
    if (NIV_PORT_MC_NH_INFO(unit, vp) != NULL) {
        /* The shared vp associates with mulicast egress objects. */
        if (NIV_PORT_INFO(unit, vp)->egress_list == NULL) {
            return BCM_E_INTERNAL;
        }
        vp_mc = 1;
    }
    BCM_PBMP_CLEAR(pbmp);
    bkt_size = vp_mc ? TD_NIV_PORT_HASH_BKT_SIZE : 1;
    for (bkt_idx = 0; bkt_idx < bkt_size; bkt_idx++) {
        if (vp_mc) {
            curr_ptr = NIV_PORT_HASH_BKT(unit, vp, bkt_idx)->next;
        } else {
            curr_ptr = NIV_PORT_INFO(unit, vp)->egress_list;
        }
        while (NULL != curr_ptr) {
            /* Delete ingress VLAN translation entry */
            if (!(curr_ptr->flags & BCM_NIV_EGRESS_MULTICAST)) {
                BCM_IF_ERROR_RETURN(_bcm_trident_niv_match_delete(unit,
                            curr_ptr->port, curr_ptr->virtual_interface_id,
                            curr_ptr->match_vlan, NULL));
            }

            /* Decrement next hop's reference count. If count reaches zero,
             * free the next hop index and clear next hop table entries.
             */
            BCM_IF_ERROR_RETURN
                (_bcm_trident_niv_nh_info_delete(unit, curr_ptr->next_hop_index));

            /* Add this NIV egress object's port(s) to the bitmap of physical ports
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

            /* Free current NIV egress object and advance to the next one */
            next_ptr = curr_ptr->next;
            sal_free(curr_ptr);
            curr_ptr = next_ptr;
        }
        
        if (vp_mc) {
            NIV_PORT_HASH_BKT(unit, vp, bkt_idx)->next = NULL;
        } else {
            NIV_PORT_INFO(unit, vp)->egress_list = NULL;
        }
    }
    
    if (vp_mc) {
        sal_free(NIV_PORT_INFO(unit, vp)->egress_list);
        NIV_PORT_INFO(unit, vp)->egress_list = NULL;
        _bcm_trident_niv_egress_pbmp_clear(unit, vp);
    }
    
    /* Decrement VP count on all the physical ports on which the VP resides */
    BCM_PBMP_ITER(pbmp, local_port) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_gport_get(unit, local_port, &gport));
        BCM_IF_ERROR_RETURN
            (_bcm_trident_niv_port_cnt_update(unit, gport, vp, FALSE, FALSE));
    }

    /* Add new NIV egress objects to NIV port */
    for (i = 0; i < array_size; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_trident_niv_egress_add(unit, niv_port, &niv_egress_array[i]));
    }

    return BCM_E_NONE;
}

/* Function:
 *      bcm_trident_niv_egress_get
 * Purpose:
 *      Get an array of NIV egress objects for a NIV port
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      niv_port - (IN) NIV port
 *      array_size - (IN) Size of NIV egress objects array.
 *      niv_egress_array - (OUT) Array of NIV egress objects.
 *      count - (OUT) Number of NIV egress objects returned.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_egress_get(
    int unit, 
    bcm_gport_t niv_port, 
    int array_size, 
    bcm_niv_egress_t *niv_egress_array,
    int *count)
{
    int vp;
    _bcm_trident_niv_egress_t *curr_ptr;
    int bkt_idx, bkt_size;
    int vp_mc = 0;

    /* Verify niv_port is a valid NIV port with no match criteria */
    if (!BCM_GPORT_IS_NIV_PORT(niv_port)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_NIV_PORT_ID_GET(niv_port);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_NOT_FOUND;
    }
    if (!(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MATCH_NONE)) {
        return BCM_E_PARAM;
    }

    if (array_size > 0 && niv_egress_array == NULL) {
        return BCM_E_PARAM;
    }
    if (count == NULL) {
        return BCM_E_PARAM;
    }

    /* Traverse NIV port's existing list of NIV egress objects */
    *count = 0;
    curr_ptr = NIV_PORT_INFO(unit, vp)->egress_list;

    if (NIV_PORT_MC_NH_INFO(unit, vp) != NULL) {
        /* The shared vp associates with mulicast egress objects. */
        if (NIV_PORT_INFO(unit, vp)->egress_list == NULL) {
            return BCM_E_INTERNAL;
        }
        vp_mc = 1;
    }
    bkt_size = vp_mc ? TD_NIV_PORT_HASH_BKT_SIZE : 1;
    if (array_size == BCM_NIV_GET_ONE_EGR_OBJECT) {
        for (bkt_idx = 0; bkt_idx < bkt_size; bkt_idx++) {
            if (vp_mc) {
                curr_ptr = NIV_PORT_HASH_BKT(unit, vp, bkt_idx)->next;
            } else {
                curr_ptr = NIV_PORT_INFO(unit, vp)->egress_list;
            }            
            while (NULL != curr_ptr) {
                if ((niv_egress_array[0].port == curr_ptr->port) &&
                    (niv_egress_array[0].virtual_interface_id ==
                     curr_ptr->virtual_interface_id) &&
                    (niv_egress_array[0].flags == curr_ptr->flags)) {
                    niv_egress_array[0].port = curr_ptr->port;
                    niv_egress_array[0].virtual_interface_id =
                        curr_ptr->virtual_interface_id;
                    niv_egress_array[0].match_vlan = curr_ptr->match_vlan;
                    niv_egress_array[0].service_tpid = curr_ptr->service_tpid;
                    niv_egress_array[0].service_vlan = curr_ptr->service_vlan;
                    niv_egress_array[0].service_pri = curr_ptr->service_pri;
                    niv_egress_array[0].service_cfi = curr_ptr->service_cfi;
                    niv_egress_array[0].service_qos_map_id =
                        curr_ptr->service_qos_map_id;
                    niv_egress_array[0].intf = curr_ptr->intf;
                    niv_egress_array[0].multicast_flags =
                        curr_ptr->multicast_flags;
                    
                    niv_egress_array[0].egress_if = curr_ptr->next_hop_index +
                                                    BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);                  
                    *count = 1;
                    return BCM_E_NONE;
                } else {
                    curr_ptr = curr_ptr->next;
                }   
            }
        } 
        return BCM_E_NOT_FOUND;
    } 

    for (bkt_idx = 0; bkt_idx < bkt_size; bkt_idx++) {
        if (vp_mc) {
            curr_ptr = NIV_PORT_HASH_BKT(unit, vp, bkt_idx)->next;
        } else {
            curr_ptr = NIV_PORT_INFO(unit, vp)->egress_list;
        }
        while (NULL != curr_ptr) {
            if (array_size > 0) {
                niv_egress_array[*count].flags = curr_ptr->flags;
                niv_egress_array[*count].port = curr_ptr->port;
                niv_egress_array[*count].virtual_interface_id =
                    curr_ptr->virtual_interface_id;
                niv_egress_array[*count].match_vlan = curr_ptr->match_vlan;
                niv_egress_array[*count].service_tpid = curr_ptr->service_tpid;
                niv_egress_array[*count].service_vlan = curr_ptr->service_vlan;
                niv_egress_array[*count].service_pri = curr_ptr->service_pri;
                niv_egress_array[*count].service_cfi = curr_ptr->service_cfi;
                niv_egress_array[*count].service_qos_map_id =
                    curr_ptr->service_qos_map_id;            
                niv_egress_array[*count].intf = curr_ptr->intf;
                niv_egress_array[*count].multicast_flags = 
                    curr_ptr->multicast_flags;
                if (curr_ptr->flags & BCM_NIV_EGRESS_L3) {
                    niv_egress_array[*count].egress_if =
                        curr_ptr->next_hop_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                } else {    
                    niv_egress_array[*count].egress_if =
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
 *     _bcm_niv_sw_dump
 * Purpose:
 *     Displays NIV information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_trident_niv_sw_dump(int unit)
{
    int i, num_vp;
    _bcm_trident_niv_egress_t *curr_ptr;
    int egr_obj_index;
    char pfmt[SOC_PBMP_FMT_LEN];
    int  bkt_idx, bkt_size;
    int  vp_mc = 0;
    bcm_pbmp_t pbmp;

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information NIV - Unit %d\n"), unit));

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    LOG_CLI((BSL_META_U(unit,
                        "\n  Port Info    : \n")));
    for (i = 0; i < num_vp; i++) {
        if (!(NIV_PORT_INFO(unit, i)->flags & BCM_NIV_PORT_MATCH_NONE)) {
            if (NIV_PORT_INFO(unit, i)->port == 0) {
                continue;
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n  NIV port vp = %d\n"), i));
            LOG_CLI((BSL_META_U(unit,
                                "    Flags = 0x%x\n"), NIV_PORT_INFO(unit, i)->flags));
            LOG_CLI((BSL_META_U(unit,
                                "    Port = 0x%x\n"), NIV_PORT_INFO(unit, i)->port));
            LOG_CLI((BSL_META_U(unit,
                                "    Trunk member bitmap = %s\n"), 
                     SOC_PBMP_FMT(NIV_PORT_INFO(unit, i)->tp_pbmp, pfmt)));
            LOG_CLI((BSL_META_U(unit,
                                "    Virtual Interface ID = 0x%x\n"),
                     NIV_PORT_INFO(unit, i)->virtual_interface_id));
            LOG_CLI((BSL_META_U(unit,
                                "    Match VLAN = 0x%x\n"),
                     NIV_PORT_INFO(unit, i)->match_vlan));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "\n  NIV port vp = %d\n"), i));
            LOG_CLI((BSL_META_U(unit,
                                "    Flags = 0x%x\n"), NIV_PORT_INFO(unit, i)->flags));

            egr_obj_index = 0;
            vp_mc = 0;
            if (NIV_PORT_MC_NH_INFO(unit, i) != NULL) {
                if (NIV_PORT_INFO(unit, i)->egress_list == NULL) {
                    continue;
                }
                vp_mc = 1;
            }
            bkt_size = vp_mc ? TD_NIV_PORT_HASH_BKT_SIZE : 1;
            for (bkt_idx = 0; bkt_idx < bkt_size; bkt_idx++) {
                /* The shared vp associates with mulicast egress objects. */
                if (vp_mc) {
                    curr_ptr = NIV_PORT_HASH_BKT(unit, i, bkt_idx)->next;
                    if (curr_ptr) {
                        LOG_CLI((BSL_META_U(unit,
                                            "\n  Bkt_idx = %d\n"), bkt_idx));
                    }
                } else {
                    curr_ptr = NIV_PORT_INFO(unit, i)->egress_list;
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
                                        "      Virtual Interface ID = 0x%x\n"),
                             curr_ptr->virtual_interface_id));
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
                _bcm_trident_niv_egress_pbmp_get(unit, i, &pbmp);
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
_bcm_niv_gport_get(int unit, int is_trunk, int modid,
                   int port, int dst_vif, bcm_gport_t *niv_gport)
{
    int vp;
    bcm_trunk_t trunk_id = -1;
    bcm_port_t local_port_id;
    bcm_module_t local_modid;
    int id_out;
    int dst_vif_match = 0;
    _bcm_trident_niv_egress_t *curr_ptr = NULL;
    if (NULL == niv_gport) {
        return BCM_E_PARAM;
    }
    if (NULL == NIV_INFO(unit)->port_info) {
        return BCM_E_INTERNAL;
    }

    for (vp = soc_mem_index_min(unit, SOURCE_VPm);
         vp <= soc_mem_index_max(unit, SOURCE_VPm);
         vp++) {
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
            curr_ptr =  NIV_PORT_INFO(unit, vp)->egress_list;
            dst_vif_match = 0;
            if (!(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MULTICAST)) {
                if (NULL != curr_ptr) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_esw_gport_resolve(unit, curr_ptr->port, &local_modid,
                                                &local_port_id, &trunk_id,
                                                &id_out));
                    if (curr_ptr->virtual_interface_id == dst_vif) {
                        dst_vif_match = 1;
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (_bcm_esw_gport_resolve(unit, NIV_PORT_INFO(unit, vp)->port,
                                                &local_modid,
                                                &local_port_id,
                                                &trunk_id,
                                                &id_out));                    
                    if (NIV_PORT_INFO(unit, vp)->virtual_interface_id == dst_vif) {
                        dst_vif_match = 1;
                    }
                }
                
                if (dst_vif_match) {
                    if ((is_trunk == 1) && (trunk_id == port)) {
                        BCM_GPORT_NIV_PORT_ID_SET(*niv_gport, vp);
                        return BCM_E_NONE;
                    } else if ((is_trunk != 1) && (trunk_id == -1)) {
                        if ((local_port_id == port) &&
                            (local_modid == modid )) {
                            BCM_GPORT_NIV_PORT_ID_SET(*niv_gport, vp);
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
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

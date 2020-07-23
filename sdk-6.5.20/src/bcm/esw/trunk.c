/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trunk.c
 * Purpose: BCM level APIs for trunking (a.k.a. Port Aggregation)
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/scache.h>
#if defined(BCM_BRADLEY_SUPPORT)
#include <soc/bradley.h>
#include <soc/firebolt.h>
#endif /* BCM_BRADLEY_SUPPORT */

#include <bcm/error.h>
#include <bcm/mirror.h>
#include <bcm/trunk.h>

#include <bcm_int/api_xlate_port.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/trident2.h>
#ifdef BCM_KATANA2_SUPPORT
#include <bcm_int/esw/katana2.h>
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_HGPROXY_COE_SUPPORT
#include <bcm_int/esw/xgs5.h>
#endif /* BCM_HGPROXY_COE_SUPPORT */
#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#endif /* BCM_SABER2_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/switch.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm/types.h>
#endif /* BCM_WARM_BOOT_SUPPORT */


typedef struct trunk_cntl_s {
    int                ngroups_fp;  /* number of (front panel) trunk groups */
    int                nports_fp;   /* max ports per trunk group */
    int                ngroups_hg;  /* number of fabric trunk groups */
    int                nports_hg;   /* max ports per fabric trunk group */
#ifdef BCM_APACHE_SUPPORT
    int              ngroups_rr_fp; /* number of fp trunks supporting Round Robin Load balancing */
    SHR_BITDCL      *fp_rr_lag_bmp; /* FP RoundRobin Lag usage bitmap */
#endif /* BCM_APACHE_SUPPORT */
    trunk_private_t   *t_info;
    sal_mutex_t        lock;        /* Trunk module lock. */
#ifdef BCM_WARM_BOOT_SUPPORT
    SHR_BITDCL  *trunk_bmp_ptr;     /* Warm Boot trunk use bitmap */
    int         trunk_bmp_alloc;    /* WB trunk use allocation */
    uint8       *fp_hwf_flags;      /* WB FP - HW failover flags record */
    int         fp_hwf_alloc;       /* WB FP - HW failover allocation */
    uint8       *hg_hwf_flags;      /* WB HG - HW failover flags record */
    int         hg_hwf_alloc;       /* WB HG - HW failover allocation */
#endif /* BCM_WARM_BOOT_SUPPORT */
} trunk_cntl_t;

/*
 * One trunk control entry for each SOC device containing trunk book keeping
 * info for that device.
 */
static trunk_cntl_t bcm_trunk_control[BCM_MAX_NUM_UNITS];

#define TRUNK_CNTL(unit)	bcm_trunk_control[unit]
#define TRUNK_INFO(unit, tid)	bcm_trunk_control[unit].t_info[tid]

/*
 * Cause a routine to return BCM_E_INIT if trunking subsystem is not
 * initialized.
 */

#define TRUNK_INIT(unit)	                    \
	if ( (TRUNK_CNTL(unit).ngroups_fp <= 0) && \
	     (TRUNK_CNTL(unit).ngroups_hg <= 0) ) { return BCM_E_INIT; }

#define TRUNK_NUM_GROUPS(unit) \
        (TRUNK_CNTL(unit).ngroups_fp + TRUNK_CNTL(unit).ngroups_hg)

/*
 * Make sure TID is within valid range.
 */

#define TRUNK_CHECK(unit, tid) \
	if ((tid) < 0 || (tid) >= TRUNK_NUM_GROUPS(unit)) \
           { return BCM_E_PARAM; }

#define TRUNK_FP_TID(unit, tid) \
    ( ((tid) >= 0) && ((tid) < TRUNK_CNTL(unit).ngroups_fp) )

#define TRUNK_FABRIC_TID(unit, tid)             \
    ( ((tid) >= TRUNK_CNTL(unit).ngroups_fp) && \
      ((tid) < TRUNK_NUM_GROUPS(unit)) )

/*
 * Function:
 *	_bcm_esw_trunk_gport_construct
 * Purpose:
 *	Converts ports and modules given in t_data structure into gports
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      fabric_trunk - trunk is a fabric trunk
 *      count - number of failover gports in list
 *      port_list - (IN) list of port numbers
 *      modid_list - (IN) list of module ids
 *      port_array - (OUT) list of gports to return
 *
 * Note:
 *      port_list and port_array may be the same list.  This updates
 *      the list in place.
 */
int
_bcm_esw_trunk_gport_construct(int unit, int fabric_trunk, int count,
                               bcm_port_t *port_list,
                               bcm_module_t *modid_list,
                               bcm_gport_t *port_array)
{
    bcm_gport_t         gport;
    int                 i, mod_is_local;
    _bcm_gport_dest_t   dest;
    bcm_module_t        base_modid;
    bcm_port_t          local_port;
#if defined(BCM_KATANA2_SUPPORT)
    int                 local_subport = 0, pp_port = 0;
#endif

    _bcm_gport_dest_t_init(&dest);
    
    for (i = 0; i < count; i++) {
        gport = 0;
        /* Stacking ports should be encoded as devport */
        if (fabric_trunk) {
            /* Error checking during set functions should guarantee that
             * stack ports iff fabric trunks */
            dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;
            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, modid_list[i],
                                        port_list[i], &dest.modid, &dest.port));
            BCM_IF_ERROR_RETURN(
                    _bcm_esw_gport_construct(unit, &dest, &gport));
        } else {
            dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
            dest.modid = modid_list[i];
            dest.port = port_list[i];
#if defined(BCM_KATANA2_SUPPORT)
            BCM_IF_ERROR_RETURN(_bcm_kt2_modport_is_local_coe_subport(unit,
                                    dest.modid, dest.port, &local_subport));
            if (local_subport) {
                 BCM_IF_ERROR_RETURN(_bcm_kt2_modport_to_pp_port_get(unit,
                                        dest.modid, dest.port, &pp_port)); 

                /* construct LinkPHY/SubportPkttag subport gport */
                _BCM_KT2_SUBPORT_PORT_ID_SET(gport, pp_port);
                if (BCM_PBMP_MEMBER(
                    SOC_INFO(unit).linkphy_pp_port_pbm, pp_port)) {
                    _BCM_KT2_SUBPORT_PORT_TYPE_SET(gport,
                        _BCM_KT2_SUBPORT_TYPE_LINKPHY);
                } else if (BCM_PBMP_MEMBER(
                    SOC_INFO(unit).subtag_pp_port_pbm, pp_port)) {
                    _BCM_KT2_SUBPORT_PORT_TYPE_SET(gport,
                        _BCM_KT2_SUBPORT_TYPE_SUBTAG);
                } else {
                    return BCM_E_PARAM;
                }
            } else
#endif
            {
#ifdef BCM_HGPROXY_COE_SUPPORT
                if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                    (soc_feature(unit, soc_feature_channelized_switching))) &&
                    _bcm_xgs5_subport_coe_mod_port_local(
                        unit, dest.modid, dest.port)) {
                } else
#endif
                {
                    BCM_IF_ERROR_RETURN
                        (_bcm_esw_modid_is_local(unit, modid_list[i],
                                                 &mod_is_local));
                    if (mod_is_local) {
                        BCM_IF_ERROR_RETURN
                            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                                     modid_list[i], port_list[i],
                                                     &base_modid, &local_port));
                        if (IS_ST_PORT(unit, local_port)) {
                            dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;
                            dest.modid = base_modid;
                            dest.port = local_port;
                        }
                    }
                }
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_gport_construct(unit, &dest, &gport));
            }
        }
        port_array[i] = gport;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_esw_trunk_gport_array_resolve
 * Purpose:
 *	Converts gports list into ports and modules
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      fabric_trunk - trunk is a fabric trunk
 *      count - number of failover gports in list
 *      port_array - (IN) list of gports
 *      port_list - (OUT) list of port numbers to return
 *      modid_list - (OUT) list of module ids to return
 *
 * Note:
 *      port_list and port_array may be the same list.  This updates
 *      the list in place.
 */
int
_bcm_esw_trunk_gport_array_resolve(int unit, int fabric_trunk, int count,
                                   bcm_gport_t *port_array,
                                   bcm_port_t *port_list,
                                   bcm_module_t *modid_list)
                             
{
    bcm_port_t      port;
    bcm_module_t    modid, local_modid;
    bcm_trunk_t     tgid;
    int             i, id, rv;

    rv = bcm_esw_stk_my_modid_get(unit, &local_modid);
    if (BCM_FAILURE(rv) && !SOC_IS_XGS_FABRIC(unit) ) {
        return (rv);
    }

    for (i = 0; i < count; i++) {
        if (BCM_GPORT_IS_SET(port_array[i])) {
            if (fabric_trunk) {
                /* Must be local */
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_local_get(unit, port_array[i], &port));
#if defined(BCM_HGPROXY_COE_SUPPORT)
                /*Cascaded port*/
                if (IS_SUBTAG_PORT(unit, port)
                    && (soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                    (soc_feature(unit, soc_feature_channelized_switching)))) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_esw_gport_resolve(unit, port_array[i],
                                                &modid, &port, &tgid, &id));
                    port_list[i] = port;
                    modid_list[i] = modid;
                } else 
#endif
                {
                    port_list[i] = port;
                    modid_list[i] = -1;
                }
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_gport_resolve(unit, port_array[i],
                                            &modid, &port, &tgid, &id));

#if defined(BCM_HGPROXY_COE_SUPPORT)
                if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                     (soc_feature(unit, soc_feature_channelized_switching))) &&
                    _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_array[i])) {
                            id = -1;
                } else
#endif

#if defined(BCM_KATANA2_SUPPORT)
                if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit,
                    port_array[i])) {
                    id = -1;
                }
#endif
                if ((-1 != tgid) || (-1 != id)) {
                    return BCM_E_PARAM;
                }
                port_list[i] = port;
                modid_list[i] = modid;
            }
        } else {
            if (fabric_trunk) {
                if (SOC_PORT_VALID(unit, port_array[i])) {
                    port_list[i] = port_array[i];
                    modid_list[i] = -1;
                } else {
                    return BCM_E_PORT;
                }
            } else {
                PORT_DUALMODID_VALID(unit, port_array[i]);
                if (port_array != port_list) {
                    port_list[i] = port_array[i];
                    modid_list[i] = local_modid;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_esw_trunk_port_property_get
 * Purpose:
 *	Determines whether a system port is part of a trunk
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      modid  - Module id.
 *      port   - Port number.
 *      tid - (OUT) Trunk ID if trunk member.
 * Notes:
 *      Currently used only by RX module, gport support not required.
 */
int
_bcm_esw_trunk_port_property_get(int unit,
                                 bcm_module_t modid,
                                 bcm_port_t port,
                                 int *tid)
{
#if defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
        return _bcm_trident_trunk_get_port_property(unit, modid, port, tid);
    }
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        return _bcm_th3_trunk_get_port_property(unit, port, tid);
    }
#endif
#endif
#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_XGS_SWITCH(unit)) {
        return _bcm_xgs3_trunk_get_port_property(unit, modid, port, tid);
    }
#endif
    return BCM_E_UNAVAIL;
}

#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT
int
_bcm_esw_trunk_subport_sanitize(int unit,
                            int member_count,
                            bcm_trunk_member_t *member_array,
                            bcm_trunk_member_t *new_member)
{
    int is_subport=0, is_phyport=0;
    int i=0;
    bcm_port_t gport, subport=0;
    int rv = 0;

    if (soc_feature(unit, soc_feature_channelized_switching)) {

        if (new_member != NULL) {
            gport = new_member->gport;
            if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
                is_subport = 1;
            } else {
                /* Just check if this is not modport for subport. */
                rv = _bcm_coe_subtag_subport_port_get_by_modport(unit,
                                                 gport, &subport);

                if (BCM_SUCCESS(rv)) {
                    if (!_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport)) {
                        is_phyport = 1;
                    }
                } else {
                    is_phyport = 1;
                }
            }
        }

        for (; i<member_count; i++) {
            gport = member_array[i].gport;
            if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
                is_subport = 1;
            } else {
               /* Just check if this is not modport for subport. */
                rv = _bcm_coe_subtag_subport_port_get_by_modport(unit,
                                                 gport, &subport);

                if (BCM_SUCCESS(rv)) {
                    if (!_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport)) {
                        is_phyport = 1;
                    }
                } else {
                    is_phyport = 1;
                }
            }

            if (is_phyport && is_subport) {
                return BCM_E_CONFIG;
            }
        }
    }

    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *	_bcm_esw_trunk_active_member_get
 * Purpose:
 *      Returns active port information for a given trunk.
 *      Returns only the members on which EGRESS_DISABLE is not set.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      tid    - Trunk ID.
 *      trunk_info   - (OUT) Place to store returned trunk info.
 *      member_max   - (IN) Size of member_array.
 *      active_member_array - (OUT) Place to store returned active trunk 
 *                                  members.
 *      active_member_count - (OUT) Place to store returned number of active
 *                                  trunk members.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 */
int
_bcm_esw_trunk_active_member_get(int unit, bcm_trunk_t tid, 
                                 bcm_trunk_info_t *trunk_info,
                                 int member_max, 
                                 bcm_trunk_member_t *active_member_array,
                                 int *active_member_count)
{
    int                  rv = BCM_E_NONE;
    int                  member_count;
    bcm_trunk_member_t   member_array[BCM_SWITCH_TRUNK_MAX_PORTCNT];
    int                  filled_count = 0;

    if ((member_max > 0) && (NULL == active_member_array)) {
        return BCM_E_PARAM;
    }

    if ((member_max > 0) && (NULL == active_member_count)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (bcm_esw_trunk_get(unit, tid, NULL, BCM_SWITCH_TRUNK_MAX_PORTCNT, 
                           member_array, &member_count));

    if (0 < member_count) {
        int i;
        if (active_member_array != NULL) {
            sal_memset(active_member_array, 0, 
                       sizeof(bcm_trunk_member_t) * member_max);
        }

        *active_member_count = 0;

        /* Get only the active member ports */
        for (i = 0; i < member_count; i++) {
             if (!(member_array[i].flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE)) {
                 if ( (active_member_array != NULL) && (member_max > 0) ) {
                     sal_memcpy(&active_member_array[*active_member_count], 
                                &member_array[i], sizeof(member_array[i]));
                     filled_count++;
                 }
                 (*active_member_count)++;

                 if ( (member_max > 0) && (filled_count == member_max) ) {
                     break;
                 }
             }
        }
    }
 
    return rv;
}

/*
 * Function:
 *	_bcm_esw_trunk_rtag_get
 * Purpose:
 *	Retrieve the RTAG HW value of a trunk
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - Trunk ID.
 *      rtag - (OUT) HW RTAG value for trunk.
 * Notes:
 *      Currently used by mirror module
 */
int
_bcm_esw_trunk_rtag_get(int unit,  bcm_trunk_t tid, int *rtag)

{
    trunk_private_t *t_info;

    if (NULL == rtag) {
        return BCM_E_PARAM;
    }

    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        return BCM_E_NOT_FOUND;
    }

    *rtag = t_info->rtag;
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_trunk_deinit
 * Purpose:
 *	Release allocated trunk data structures.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 */
void
_bcm_esw_trunk_deinit(int unit)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit)) {
#if defined(BCM_TRIDENT_SUPPORT)
        if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
            _bcm_trident_trunk_deinit(unit);
        } else
#endif /* BCM_TRIDENT_SUPPORT */
        {
            _bcm_xgs3_trunk_member_detach(unit);
            _bcm_xgs3_trunk_swfailover_detach(unit);
            _bcm_xgs3_trunk_mod_port_map_deinit(unit);
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
            _bcm_xgs3_trunk_hwfailover_detach(unit);
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
        }

#ifdef BCM_APACHE_SUPPORT
        if (soc_feature(unit, soc_feature_apache_round_robin_fp_lag)) {
            if (NULL != TRUNK_CNTL(unit).fp_rr_lag_bmp) {
                sal_free(TRUNK_CNTL(unit).fp_rr_lag_bmp);
                TRUNK_CNTL(unit).fp_rr_lag_bmp = NULL;
            }
        }
#endif /* BCM_APACHE_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
        if (NULL != TRUNK_CNTL(unit).trunk_bmp_ptr) {
            sal_free(TRUNK_CNTL(unit).trunk_bmp_ptr);
            TRUNK_CNTL(unit).trunk_bmp_ptr = NULL;
        }
        if (NULL != TRUNK_CNTL(unit).fp_hwf_flags) {
            sal_free(TRUNK_CNTL(unit).fp_hwf_flags);
            TRUNK_CNTL(unit).fp_hwf_flags = NULL;
        }
        if (NULL != TRUNK_CNTL(unit).hg_hwf_flags) {
            sal_free(TRUNK_CNTL(unit).hg_hwf_flags);
            TRUNK_CNTL(unit).hg_hwf_flags = NULL;
        }
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vp_lag)) {
        bcm_td2_vp_lag_deinit(unit);
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_4                SOC_SCACHE_VERSION(1,4)
#define BCM_WB_VERSION_1_5                SOC_SCACHE_VERSION(1,5)
#define BCM_WB_VERSION_1_6                SOC_SCACHE_VERSION(1,6)
/*add dlf_index to scache*/
#define BCM_WB_VERSION_1_7                SOC_SCACHE_VERSION(1,7)
/* Add bcm_trident_trunk_mbr_zero_rsvd to scache. */
#define BCM_WB_VERSION_1_8                SOC_SCACHE_VERSION(1,8)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_8


#endif /* BCM_WARM_BOOT_SUPPORT */

STATIC int
_bcm_esw_trunk_hwmem_clear(int unit, soc_mem_t mem)
{
    uint8 *mbuf;
    int rv, size;

    size = SOC_MEM_TABLE_BYTES(unit, mem);
    mbuf = soc_cm_salloc(unit, size, "trunk_memory");
    if (mbuf == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(mbuf, 0, size);

    rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ANY,
                             soc_mem_index_min(unit, mem),
                             soc_mem_index_max(unit, mem), mbuf);
    soc_cm_sfree(unit, mbuf);

    return rv;
}

/*
 * Function:
 *	bcm_trunk_init
 * Purpose:
 *	Initializes the trunk tables to empty (no trunks configured)
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 */

int
bcm_esw_trunk_init(int unit)
{
#ifdef BCM_XGS_SWITCH_SUPPORT
    uint16      dev_id;
    uint8       rev_id;
#endif
    trunk_private_t *t_info;
    bcm_trunk_t     tid;
    bcm_pbmp_t      all_pbmp;
    soc_mem_t       trunk_mem = TRUNK_GROUPm;

#ifdef BCM_TOMAHAWK3_SUPPORT
    soc_info_t *si;
    source_trunk_map_table_entry_t stm_ent;
    int stm_entry_per_pipe;
    int pipe;
#endif

    int rv = BCM_E_NONE;

    COMPILER_REFERENCE(t_info);
    COMPILER_REFERENCE(tid);
    COMPILER_REFERENCE(all_pbmp);

    if (soc_feature(unit, soc_feature_fastlag)) {
        trunk_mem = FAST_TRUNK_GROUPm;
    }

    /* Create protection mutex. */
    if (NULL == TRUNK_CNTL(unit).lock) {
        TRUNK_CNTL(unit).lock = sal_mutex_create("Trunk module mutex");
        if(NULL == TRUNK_CNTL(unit).lock) {
            return BCM_E_MEMORY;
        }
    }

    TRUNK_LOCK(unit);

    BCM_PBMP_CLEAR(all_pbmp);
    BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));

    TRUNK_CNTL(unit).ngroups_fp = 0;
    TRUNK_CNTL(unit).nports_fp = 0;
    TRUNK_CNTL(unit).ngroups_hg = 0;
    TRUNK_CNTL(unit).nports_hg = 0;

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (SOC_IS_XGS_SWITCH(unit)) {
#ifdef BCM_WARM_BOOT_SUPPORT
#ifdef BCM_XGS3_SWITCH_SUPPORT
        int                 ngroups_fp_max;
        soc_scache_handle_t scache_handle;
        uint32              trunk_scache_size;
        uint8               *trunk_scache;
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
        int                 trunk_fp_hwf_flags_alloc_size;
        int                 trunk_hg_hwf_flags_alloc_size;

        COMPILER_REFERENCE(trunk_fp_hwf_flags_alloc_size);
        COMPILER_REFERENCE(trunk_hg_hwf_flags_alloc_size);;

#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

        COMPILER_REFERENCE(ngroups_fp_max);
        COMPILER_REFERENCE(scache_handle);
        COMPILER_REFERENCE(trunk_scache_size);
        COMPILER_REFERENCE(trunk_scache);

#endif /* BCM_XGS3_SWITCH_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */

        if (SOC_IS_SHADOW(unit)) {
            TRUNK_UNLOCK(unit);
            return BCM_E_UNAVAIL;
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            TRUNK_CNTL(unit).ngroups_fp =
                soc_mem_index_count(unit, FAST_TRUNK_PORTS_1m);

            /* No HiGig support in TH3 */
            TRUNK_CNTL(unit).ngroups_hg = 0;
        } else
#endif
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit)) {
            TRUNK_CNTL(unit).ngroups_fp = SABER2_5626X_NGROUPS_FP;
        } else
#endif /* BCM_SABER2_SUPPORT */
        {
            TRUNK_CNTL(unit).ngroups_fp = soc_mem_index_count(unit, trunk_mem);
        }

#ifdef BCM_WARM_BOOT_SUPPORT
#ifdef BCM_XGS3_SWITCH_SUPPORT
        /* Record maximum HW size */
        ngroups_fp_max = TRUNK_CNTL(unit).ngroups_fp;
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            TRUNK_CNTL(unit).nports_fp = 1 << soc_mem_field_length(unit,
                    FAST_TRUNK_SIZEm, TG_SIZEf);

            /* No HiGig support in TH3 */
            TRUNK_CNTL(unit).nports_hg = 0;
        } else
#endif
        if (soc_mem_field_valid(unit, trunk_mem, TG_SIZEf)) {
            soc_cm_get_id(unit, &dev_id, &rev_id);
#if defined(BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit)) {
                TRUNK_CNTL(unit).nports_fp = SABER2_5626X_NPORTS_FP;
            } else
#endif /* BCM_SABER2_SUPPORT */
            if ((dev_id == BCM56830_DEVICE_ID)
                || (dev_id == BCM56838_DEVICE_ID)
                || (dev_id == BCM56831_DEVICE_ID)
                || (dev_id == BCM56834_DEVICE_ID)) {
                TRUNK_CNTL(unit).nports_fp = 16;
            } else if (dev_id == BCM56835_DEVICE_ID) {
                TRUNK_CNTL(unit).nports_fp = 32;
            } else {
                TRUNK_CNTL(unit).nports_fp = 1 << soc_mem_field_length(unit,
                        trunk_mem, TG_SIZEf);
            }
        } else {
            TRUNK_CNTL(unit).nports_fp = 8;
        }

	if (!SOC_WARM_BOOT(unit) && !SOC_HW_RESET(unit)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                /* Tomahawk 3's Fast LAG ports are split into two memories,
                 * 32 in each, for a total of 64 possible ports per LAG */
                rv = _bcm_esw_trunk_hwmem_clear(unit, FAST_TRUNK_PORTS_1m);
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return BCM_E_INTERNAL;
                }
                rv = _bcm_esw_trunk_hwmem_clear(unit, FAST_TRUNK_PORTS_2m);
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return BCM_E_INTERNAL;
                }
                rv = _bcm_esw_trunk_hwmem_clear(unit, FAST_TRUNK_SIZEm);
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return BCM_E_INTERNAL;
                }
            } else
#endif
            {
                rv = _bcm_esw_trunk_hwmem_clear(unit, trunk_mem);
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return BCM_E_INTERNAL;
                }
            }

            rv = _bcm_esw_trunk_hwmem_clear(unit, TRUNK_BITMAPm);
            if (BCM_FAILURE(rv)) {
                TRUNK_UNLOCK(unit);
                return BCM_E_INTERNAL;
            }

            if (soc_feature(unit, soc_feature_trunk_egress)) {
                rv = _bcm_esw_trunk_hwmem_clear(unit, TRUNK_EGR_MASKm);
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return BCM_E_INTERNAL;
                }
            }
	}

#if defined(BCM_XGS3_SWITCH_SUPPORT)
        if (SOC_IS_XGS3_SWITCH(unit)) {
            uint64 config64, oconfig64;
            soc_reg_t reg;
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
            soc_reg_t hg_trunk_bmap;
#if defined(BCM_GREYHOUND2_SUPPORT)
            soc_reg_t hg_trunk_bmap_hi = INVALIDr;
#endif /* BCM_GREYHOUND2_SUPPORT */
#endif

            if (SOC_WARM_BOOT(unit)) {

                if (SOC_IS_FBX(unit)) {

#if defined(BCM_TRX_SUPPORT)
                    reg = SOC_IS_TRX(unit) ? ING_CONFIG_64r : ING_CONFIGr;
#else /* BCM_TRX_SUPPORT */
                    reg = ING_CONFIGr;
#endif /* BCM_TRX_SUPPORT */
                    rv = soc_reg_get(unit, reg, REG_PORT_ANY, 0, &config64);
                    if (SOC_FAILURE(rv)) {
                        TRUNK_UNLOCK(unit);
                        return rv;
                    }

#if defined(BCM_SABER2_SUPPORT)
                    if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit)) {
                        TRUNK_CNTL(unit).ngroups_hg =
                            SABER2_5626X_NGROUPS_HG;
                        TRUNK_CNTL(unit).nports_hg =
                            SABER2_5626X_NPORTS_HG;
                    } else
#endif /* BCM_SABER2_SUPPORT */
#if defined(BCM_HAWKEYE_SUPPORT)
                    if (SOC_IS_HAWKEYE(unit)) {
		        TRUNK_CNTL(unit).ngroups_fp = 8;
                    } else
#endif
                    /* TD3 only supports trunks 128 mode and not trunks 32 mode.
                     * The field to distinguish is hence not present */
                    if (!SOC_IS_TRIDENT3X(unit) && !(SOC_IS_TOMAHAWK3(unit))) {
                        if (!soc_reg64_field32_get(unit, reg, config64,
                                    TRUNKS128f)) {
                            TRUNK_CNTL(unit).ngroups_fp = 32;
                        }
                    }

                    if (SOC_IS_XGS3_FABRIC(unit)) {
                        TRUNK_CNTL(unit).ngroups_fp = 0;
                        TRUNK_CNTL(unit).nports_fp = 0;
                    }
#if defined(BCM_HAWKEYE_SUPPORT)
                    if (SOC_IS_HAWKEYE(unit)) {
                        TRUNK_CNTL(unit).ngroups_hg = 0;
                        TRUNK_CNTL(unit).nports_hg = 0;
                    } else
#endif
                    if (SOC_IS_FB_FX_HX(unit) || SOC_IS_RAVEN(unit)) {
                        TRUNK_CNTL(unit).ngroups_hg = 2;
                        TRUNK_CNTL(unit).nports_hg = 4;
                    }

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
                    else if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit)) {
                        if (soc_mem_is_valid(unit, HG_TRUNK_BITMAPm)) {

#if defined(BCM_SABER2_SUPPORT)
                            if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit)) {
                                TRUNK_CNTL(unit).ngroups_hg =
                                    SABER2_5626X_NGROUPS_HG;
                            } else
#endif /* BCM_SABER2_SUPPORT */
                            {
                                TRUNK_CNTL(unit).ngroups_hg =
                                    soc_mem_index_count(unit, HG_TRUNK_BITMAPm);
                            }
#if defined(BCM_METROLITE_SUPPORT)
                            if (soc_feature(unit, soc_feature_hg_trunk_groups_max_2)) {
                              TRUNK_CNTL(unit).ngroups_hg = 2;
                            }
                            if (soc_feature(unit, soc_feature_hg_trunk_group_members_max_4)) {
                              TRUNK_CNTL(unit).nports_hg = 4;
                            }
#endif /* BCM_METROLITE_SUPPORT */
                        } else {
#if defined(BCM_TRIUMPH2_SUPPORT)
                            if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                                SOC_IS_VALKYRIE2(unit)) {
                                hg_trunk_bmap = HG_TRUNK_BITMAP_64r;
                            } else 
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
                            if (SOC_IS_GREYHOUND2(unit)) {
                                hg_trunk_bmap = HG_TRUNK_BITMAP_LOr;
                                hg_trunk_bmap_hi = HG_TRUNK_BITMAP_HIr;
                            } else
#endif /* BCM_GREYHOUND2_SUPPORT */
                            {
                                hg_trunk_bmap = HG_TRUNK_BITMAPr;
                            }
#if defined(BCM_TOMAHAWK3_SUPPORT)
                            /* TH3 doesn't support HG. ngroups_hg was set to 0
                             * above
                             */
                            if (!SOC_IS_TOMAHAWK3(unit))
#endif
                            {
                                TRUNK_CNTL(unit).ngroups_hg =
                                    SOC_REG_NUMELS(unit, hg_trunk_bmap);
                            }
                        }

#if defined(BCM_HURRICANE3_SUPPORT)
                        if (SOC_IS_HURRICANE3(unit)) {
                            soc_cm_get_id(unit, &dev_id, &rev_id);
                            if ((dev_id == BCM56163_DEVICE_ID) ||
                                (dev_id == BCM56164_DEVICE_ID) ||
                                (dev_id == BCM56166_DEVICE_ID)) { /* Hurricane3Lite */
                                TRUNK_CNTL(unit).ngroups_hg = 2;
                            }
                        }
#endif /* BCM_HURRICANE3_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT)
                        if (soc_mem_field_valid(unit, HG_TRUNK_GROUPm,
                                    TG_SIZEf)) {
                            soc_cm_get_id(unit, &dev_id, &rev_id);
#if defined(BCM_SABER2_SUPPORT)
                            if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit)) {
                                TRUNK_CNTL(unit).nports_hg =
                                    SABER2_5626X_NPORTS_HG;
                            } else
#endif /* BCM_SABER2_SUPPORT */
                            if ((dev_id == BCM56838_DEVICE_ID)
                                || (dev_id == BCM56831_DEVICE_ID)) {
                                TRUNK_CNTL(unit).nports_hg = 16;
                            } else if (dev_id == BCM56835_DEVICE_ID) {
                                TRUNK_CNTL(unit).nports_hg = 32;
                            } else {
#if defined(BCM_METROLITE_SUPPORT)
                            if (soc_feature(unit, soc_feature_hg_trunk_group_members_max_4)) {
                              TRUNK_CNTL(unit).nports_hg = 4;
                            } else
#endif /* BCM_METROLITE_SUPPORT */
                                TRUNK_CNTL(unit).nports_hg =
                                    1 << soc_mem_field_length(unit,
                                            HG_TRUNK_GROUPm, TG_SIZEf);
                            }
                        } else
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_SCORPION_SUPPORT)
                        if (SOC_IS_SC_CQ(unit)) {
                            TRUNK_CNTL(unit).nports_hg = 16;
                        } else 
#endif /* BCM_SCORPION_SUPPORT */
#if defined(BCM_HURRICANE3_SUPPORT)
                        if (soc_feature
                                (unit,
                                 soc_feature_hg_trunk_group_members_max_4)) {
                            TRUNK_CNTL(unit).nports_hg = 4;
                        } else
#endif /* BCM_HURRICANE3_SUPPORT */
                        {
#if defined(BCM_TOMAHAWK3_SUPPORT)
                            if (!SOC_IS_TOMAHAWK3(unit))
#endif
                            {
                                TRUNK_CNTL(unit).nports_hg = 8;
                            }
                        }
                    }
#endif /* BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */
                }

            } else {
                int trunk128;
                int stm_index;
                int size;
                int min_idx, max_idx, cnt_idx;
                source_trunk_map_table_entry_t *ent;
                uint8 *mbuf;
                soc_field_t tgid_fld = TGIDf;

                trunk128 = soc_property_get(unit, spn_TRUNK_EXTEND, 1);

                /* For Raptor and devices that support only the extended trunk
                 * mode, TRUNKS128 field should alwyas be programmed to 1 */
                if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit) || 
                        soc_feature(unit, soc_feature_trunk_extended_only)) {
                    trunk128 = 1;
                }
 
                if (!trunk128) {
                    if (SOC_IS_HAWKEYE(unit)){
                        TRUNK_CNTL(unit).ngroups_fp = 8;
                    } else {
                        TRUNK_CNTL(unit).ngroups_fp = 32;
                    }
                }

                if (SOC_IS_XGS3_FABRIC(unit)) {
                    TRUNK_CNTL(unit).ngroups_fp = 0;
                    TRUNK_CNTL(unit).nports_fp = 0;
                }

#if defined(BCM_SABER2_SUPPORT)
                if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit)) {
                    TRUNK_CNTL(unit).ngroups_hg =
                        SABER2_5626X_NGROUPS_HG;
                    TRUNK_CNTL(unit).nports_hg =
                        SABER2_5626X_NPORTS_HG;
                }
#endif /* BCM_SABER2_SUPPORT */
                /* Reset SOURCE_TRUNK_MAP table */
                min_idx = soc_mem_index_min(unit, SOURCE_TRUNK_MAP_TABLEm);
                max_idx = soc_mem_index_max(unit, SOURCE_TRUNK_MAP_TABLEm);
                cnt_idx = soc_mem_index_count(unit, SOURCE_TRUNK_MAP_TABLEm);

                size = SOC_MEM_TABLE_BYTES(unit, SOURCE_TRUNK_MAP_TABLEm);
                mbuf = soc_cm_salloc(unit, size, "source_trunk_map");
                if (NULL == mbuf) {
                    TRUNK_UNLOCK(unit);
                    return BCM_E_MEMORY;
                }

                soc_mem_lock(unit, SOURCE_TRUNK_MAP_TABLEm);

/* Tomahawk 3's memory is ADDR_SPLIT, so it cannot do soc_mem_read_range or 
 * soc_mem_write_range
 */
#ifdef BCM_TOMAHAWK3_SUPPORT
                /* Clear port type, which describes whether a normal port is
                 * being used or a trunk port, and trunk group id (TGID)
                 */
                if (SOC_IS_TOMAHAWK3(unit)) {
                    si = &SOC_INFO(unit);
                    stm_entry_per_pipe = cnt_idx / _TH3_PIPES_PER_DEV;
                    for (stm_index = 0; stm_index < cnt_idx; stm_index++) {
                        pipe = stm_index / stm_entry_per_pipe;

                        /* Skip the memory if the pipe is not active */
                        if (SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
                            continue;
                        }

                        rv = READ_SOURCE_TRUNK_MAP_TABLEm(unit,
                                MEM_BLOCK_ANY, stm_index, &stm_ent);
                        if (BCM_FAILURE(rv)) {
                            soc_cm_sfree(unit, mbuf);
                            soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                            TRUNK_UNLOCK(unit);
                            return rv;
                        }
                        /* clear the PORT_TYPEf and TGIDf fields */
                        soc_SOURCE_TRUNK_MAP_TABLEm_field32_set
                            (unit, &stm_ent, PORT_TYPEf, 0);
                        soc_SOURCE_TRUNK_MAP_TABLEm_field32_set
                            (unit, &stm_ent, TGIDf, 0);

                        rv =  WRITE_SOURCE_TRUNK_MAP_TABLEm(unit,
                                MEM_BLOCK_ALL, stm_index, &stm_ent);
                        if (BCM_FAILURE(rv)) {
                            soc_cm_sfree(unit, mbuf);
                            soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                            TRUNK_UNLOCK(unit);
                            return rv;
                        }

                    }
                } else
#endif
                {
                rv = soc_mem_read_range(unit, SOURCE_TRUNK_MAP_TABLEm, 
                                        MEM_BLOCK_ANY, min_idx, max_idx, mbuf);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, mbuf);
                    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                    TRUNK_UNLOCK(unit);
                    return rv;
                }
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                                        SRC_TGIDf)) {
                    tgid_fld = SRC_TGIDf;
                }
                for (stm_index = 0; stm_index < cnt_idx; stm_index++) {
                    ent = soc_mem_table_idx_to_pointer(unit,
                                SOURCE_TRUNK_MAP_TABLEm,
                                source_trunk_map_table_entry_t *,
                                mbuf, stm_index);
                    soc_SOURCE_TRUNK_MAP_TABLEm_field32_set
                        (unit, ent, PORT_TYPEf, 0);
                    soc_SOURCE_TRUNK_MAP_TABLEm_field32_set
                        (unit, ent, tgid_fld, 0);
                }
                rv = soc_mem_write_range(unit, SOURCE_TRUNK_MAP_TABLEm,
                                         MEM_BLOCK_ANY, min_idx, max_idx,
                                         mbuf);
                }
                soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                soc_cm_sfree(unit, mbuf);
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return rv;
                }

                if (SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) {
#if defined(BCM_TRX_SUPPORT)
                    reg = SOC_IS_TRX(unit) ? ING_CONFIG_64r : ING_CONFIGr;
#else /* BCM_TRX_SUPPORT */
                    reg = ING_CONFIGr;
#endif /* BCM_TRX_SUPPORT */
                    if (SOC_REG_FIELD_VALID(unit, reg, TRUNKS128f)) {
                        rv = soc_reg_get(unit, reg, REG_PORT_ANY, 0, &config64);
                        if (SOC_FAILURE(rv)) {
                            TRUNK_UNLOCK(unit);
                            return rv;
                        }
                        oconfig64 = config64;
                        soc_reg64_field32_set(unit, reg, &config64, TRUNKS128f,
                                          trunk128 ? 1 : 0);
                        if (COMPILER_64_NE(config64, oconfig64)) {
                            rv = soc_reg_set(unit, reg, REG_PORT_ANY, 0, config64);
                            if (SOC_FAILURE(rv)) {
                                TRUNK_UNLOCK(unit);
                                return rv;
                            }
                        }
                    }

                    rv = soc_mem_clear(unit, NONUCAST_TRUNK_BLOCK_MASKm,
                                        MEM_BLOCK_ALL, 0);

                    if (SOC_FAILURE(rv)) {
                        TRUNK_UNLOCK(unit);
                        return rv;
                    }

#if defined(BCM_TRIUMPH3_SUPPORT)
                    if (soc_feature(unit, soc_feature_axp)) {
                        if (soc_mem_is_valid(unit, AXP_WTX_TRUNK_GROUP_BITMAPm)) {
                            rv = soc_mem_clear(unit, AXP_WTX_TRUNK_GROUP_BITMAPm,
                                               MEM_BLOCK_ALL, 0);
                            if (SOC_FAILURE(rv)) {
                                TRUNK_UNLOCK(unit);
                                return rv;
                            }
                        }
                        if (soc_mem_is_valid(unit, AXP_WTX_TRUNK_BLOCK_MASKm)) {
                            rv = soc_mem_clear(unit, AXP_WTX_TRUNK_BLOCK_MASKm,
                                    MEM_BLOCK_ALL, 0);
                            if (SOC_FAILURE(rv)) {
                                TRUNK_UNLOCK(unit);
                                return rv;
                            }
                        }
                    }
#endif /* BCM_TRIUMPH3_SUPPORT */

                    if (soc_mem_is_valid(unit, TRUNK_MEMBERm)) {
                        rv = soc_mem_clear(unit, TRUNK_MEMBERm, MEM_BLOCK_ALL, 0);
                        if (SOC_FAILURE(rv)) {
                            TRUNK_UNLOCK(unit);
                            return rv;
                        }
                    }

                    if (soc_feature(unit, soc_feature_hg_trunking)) {
                        if (soc_mem_is_valid(unit, HIGIG_TRUNK_CONTROLm)) {
                            higig_trunk_control_entry_t entry;
                            rv = soc_mem_read(unit, HIGIG_TRUNK_CONTROLm,
                                              MEM_BLOCK_ANY, 0, &entry);
                            if (SOC_FAILURE(rv)) {
                                TRUNK_UNLOCK(unit);
                                return rv;
                            }
#if defined(BCM_KATANA2_SUPPORT)
                            if (soc_feature(unit, soc_feature_linkphy_coe) ||
                                soc_feature(unit, soc_feature_subtag_coe)) {
                                _bcm_kt2_subport_pbmp_update(unit, &all_pbmp);
                            }
                            if (SOC_IS_KATANA2(unit) &&
                                soc_feature(unit, soc_feature_flex_port)) {
                                rv = _bcm_kt2_flexio_pbmp_update(unit,
                                    &all_pbmp);
                                if (SOC_FAILURE(rv)) {
                                    TRUNK_UNLOCK(unit);
                                    return rv;
                                }
                            }
#endif
                            soc_mem_pbmp_field_set(unit, HIGIG_TRUNK_CONTROLm,
                                    &entry, ACTIVE_PORT_BITMAPf, &all_pbmp);
                            rv = soc_mem_write(unit, HIGIG_TRUNK_CONTROLm,
                                               MEM_BLOCK_ALL, 0, &entry);
                            if (SOC_FAILURE(rv)) {
                                TRUNK_UNLOCK(unit);
                                return rv;
                            }
                        } else {
                            uint64 val, hg_cntl_entry;
                            soc_reg_t hg_trunk_reg;
#if defined(BCM_GREYHOUND2_SUPPORT)
                            soc_reg_t hg_trunk_reg_hi = INVALIDr;
#endif /* BCM_GREYHOUND2_SUPPORT */

                            COMPILER_REFERENCE(val);

#if defined(BCM_TRIUMPH2_SUPPORT)
                            if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                                    SOC_IS_VALKYRIE2(unit)) {
                                hg_trunk_reg = HIGIG_TRUNK_CONTROL_64r;
                            } else  
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
                            if (SOC_IS_GREYHOUND2(unit)) {
                                hg_trunk_reg = HIGIG_TRUNK_CONTROL_LOr;
                                hg_trunk_reg_hi = HIGIG_TRUNK_CONTROL_HIr;
                            } else
#endif /* BCM_GREYHOUND2_SUPPORT */
                            {
                                hg_trunk_reg = HIGIG_TRUNK_CONTROLr;
                            }

                            rv = soc_reg_get(unit, hg_trunk_reg,
                                              REG_PORT_ANY, 0,
                                              &hg_cntl_entry);
                            if (SOC_FAILURE(rv)) {
                                TRUNK_UNLOCK(unit);
                                return rv;
                            }
                            val = soc_reg64_field_get(unit, hg_trunk_reg,
                                    hg_cntl_entry,
                                    ACTIVE_PORT_BITMAPf);
                            COMPILER_64_ZERO(hg_cntl_entry);
                            soc_reg64_field_set(unit, hg_trunk_reg,
                                    &hg_cntl_entry,
                                    ACTIVE_PORT_BITMAPf, val);
                            rv = soc_reg_set(unit, hg_trunk_reg,
                                               REG_PORT_ANY, 0,
                                               hg_cntl_entry);
                            if (SOC_FAILURE(rv)) {
                                TRUNK_UNLOCK(unit);
                                return rv;
                            }
#if defined(BCM_GREYHOUND2_SUPPORT)
                            if (SOC_IS_GREYHOUND2(unit)) {
                                rv = soc_reg_get(unit, hg_trunk_reg_hi,
                                                 REG_PORT_ANY, 0,
                                                 &hg_cntl_entry);
                                if (SOC_FAILURE(rv)) {
                                    TRUNK_UNLOCK(unit);
                                    return rv;
                                }
                                val = soc_reg64_field_get(unit,
                                                          hg_trunk_reg_hi,
                                                          hg_cntl_entry,
                                                          ACTIVE_PORT_BITMAPf);
                                COMPILER_64_ZERO(hg_cntl_entry);
                                soc_reg64_field_set(unit, hg_trunk_reg_hi,
                                                    &hg_cntl_entry,
                                                    ACTIVE_PORT_BITMAPf, val);
                                rv = soc_reg_set(unit, hg_trunk_reg_hi,
                                                 REG_PORT_ANY, 0,
                                                 hg_cntl_entry);
                                if (SOC_FAILURE(rv)) {
                                    TRUNK_UNLOCK(unit);
                                    return rv;
                                }
                            }
#endif /* BCM_GREYHOUND2_SUPPORT */
                        }
                    }

                    if (SOC_IS_FB_FX_HX(unit) || SOC_IS_RAVEN(unit)) {
                        if (soc_feature(unit, soc_feature_hg_trunking)) {
                            TRUNK_CNTL(unit).ngroups_hg = 2;
                            TRUNK_CNTL(unit).nports_hg = 4;
                            rv = WRITE_HIGIG_TRUNK_GROUPr(unit, 0);
                            if (SOC_FAILURE(rv)) {
                                TRUNK_UNLOCK(unit);
                                return rv;
                            }
                        }
                    }
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
                    else if ((SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit))
                             && !SOC_IS_TOMAHAWK3(unit)) {
                        uint64      val64;

#if defined(BCM_TRIDENT_SUPPORT)
                        if (soc_mem_is_valid(unit, HG_TRUNK_BITMAPm) &&
                                soc_mem_is_valid(unit, HG_TRUNK_GROUPm) &&
                                soc_mem_is_valid(unit, HG_TRUNK_MEMBERm)) {

#if defined(BCM_SABER2_SUPPORT)
                            if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit)) {
                                TRUNK_CNTL(unit).ngroups_hg =
                                    SABER2_5626X_NGROUPS_HG;
                                TRUNK_CNTL(unit).nports_hg =
                                    SABER2_5626X_NPORTS_HG;
                            } else
#endif /* BCM_SABER2_SUPPORT */
                            {
                                TRUNK_CNTL(unit).ngroups_hg =
                                    soc_mem_index_count(unit, HG_TRUNK_BITMAPm);
                                TRUNK_CNTL(unit).nports_hg =
                                    1 << soc_mem_field_length(unit, HG_TRUNK_GROUPm,
                                            TG_SIZEf);
                            }

#if defined(BCM_METROLITE_SUPPORT)
                            if (soc_feature(unit, soc_feature_hg_trunk_groups_max_2)) {
                                TRUNK_CNTL(unit).ngroups_hg = 2;
                            }
                            if (soc_feature(unit, soc_feature_hg_trunk_group_members_max_4)) {
                                TRUNK_CNTL(unit).nports_hg = 4;
                            }
#endif /* BCM_METROLITE_SUPPORT */

                            rv = soc_mem_clear(unit, HG_TRUNK_BITMAPm,
                                               MEM_BLOCK_ALL, TRUE);
                            if (SOC_FAILURE(rv)) {
                                TRUNK_UNLOCK(unit);
                                return rv;
                            }
                            rv = soc_mem_clear(unit, HG_TRUNK_GROUPm,
                                               MEM_BLOCK_ALL, TRUE);
                            if (SOC_FAILURE(rv)) {
                                TRUNK_UNLOCK(unit);
                                return rv;
                            }
                            rv = soc_mem_clear(unit, HG_TRUNK_MEMBERm,
                                               MEM_BLOCK_ALL, TRUE);

                            if (SOC_FAILURE(rv)) {
                                TRUNK_UNLOCK(unit);
                                return rv;
                            }
                        } else
#endif /* BCM_TRIDENT_SUPPORT */
                        {

#if defined(BCM_TRIUMPH2_SUPPORT)
                            if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                                    SOC_IS_VALKYRIE2(unit)) {
                                hg_trunk_bmap = HG_TRUNK_BITMAP_64r;
                            } else 
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
                            if (SOC_IS_GREYHOUND2(unit)) {
                                hg_trunk_bmap = HG_TRUNK_BITMAP_LOr;
                                hg_trunk_bmap_hi = HG_TRUNK_BITMAP_HIr;
                            } else
#endif /* BCM_GREYHOUND2_SUPPORT */
                            {
                                hg_trunk_bmap = HG_TRUNK_BITMAPr;
                            }

                            TRUNK_CNTL(unit).ngroups_hg =
                                SOC_REG_NUMELS(unit, hg_trunk_bmap);

#if defined(BCM_HURRICANE3_SUPPORT)
                            if (SOC_IS_HURRICANE3(unit)) {
                                soc_cm_get_id(unit, &dev_id, &rev_id);
                                if ((dev_id == BCM56163_DEVICE_ID) ||
                                    (dev_id == BCM56164_DEVICE_ID) ||
                                    (dev_id == BCM56166_DEVICE_ID)) { /* Hurricane3Lite */
                                    TRUNK_CNTL(unit).ngroups_hg = 2;
                                }
                            }
#endif /* BCM_HURRICANE3_SUPPORT */

#if defined(BCM_SCORPION_SUPPORT)
                            if (SOC_IS_SC_CQ(unit)) {
                                TRUNK_CNTL(unit).nports_hg = 16;
                            } else 
#endif /* BCM_SCORPION_SUPPORT */
#if defined(BCM_HURRICANE3_SUPPORT)
                            if (soc_feature
                                    (unit,
                                     soc_feature_hg_trunk_group_members_max_4)) {
                                TRUNK_CNTL(unit).nports_hg = 4;
                            } else
#endif /* BCM_HURRICANE3_SUPPORT */
                            {
                                TRUNK_CNTL(unit).nports_hg = 8;
                            }

                            COMPILER_64_ZERO(val64);
                            for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_hg; tid++) {
#if defined(BCM_GREYHOUND2_SUPPORT)
                                if (SOC_IS_GREYHOUND2(unit)) {
                                    rv = soc_reg_set(unit, hg_trunk_bmap_hi,
                                                     REG_PORT_ANY, tid, val64);
                                    if (SOC_FAILURE(rv)) {
                                        TRUNK_UNLOCK(unit);
                                        return rv;
                                    }
                                }
#endif /* BCM_GREYHOUND2_SUPPORT */
                                rv = soc_reg_set(unit, hg_trunk_bmap, REG_PORT_ANY,
                                                 tid, val64);
                                if (SOC_FAILURE(rv)) {
                                    TRUNK_UNLOCK(unit);
                                    return rv;
                                }

                                rv = WRITE_HG_TRUNK_GROUPr(unit, tid, val64);
                                if (SOC_FAILURE(rv)) {
                                    TRUNK_UNLOCK(unit);
                                    return rv;
                                }
                            }
                        }
                    }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
                }
            }
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                rv = _bcm_th3_trunk_init(unit);
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return rv;
                }
            } else
#endif
#ifdef BCM_TRIDENT_SUPPORT
            if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
                rv = _bcm_trident_trunk_init(unit);
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return rv;
                }
            } else 
#endif /* BCM_TRIDENT_SUPPORT */
            {
                rv = _bcm_xgs3_trunk_member_init(unit);
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    _bcm_esw_trunk_deinit(unit);
                    return rv;
                }
                rv = _bcm_xgs3_trunk_swfailover_init(unit);
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    _bcm_esw_trunk_deinit(unit);
                    return rv;
                }
                rv = _bcm_xgs3_trunk_mod_port_map_init(unit);
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    _bcm_esw_trunk_deinit(unit);
                    return rv;
                }
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
                if (soc_feature(unit, soc_feature_hg_trunk_failover) ||
                    soc_feature(unit, soc_feature_port_lag_failover)) {
                    rv = _bcm_xgs3_trunk_hwfailover_init(unit);
                    if (BCM_FAILURE(rv)) {
                        TRUNK_UNLOCK(unit);
                        _bcm_esw_trunk_deinit(unit);
                        return rv;
                    }
                }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
            }

#ifdef BCM_APACHE_SUPPORT
            if (soc_feature(unit, soc_feature_apache_round_robin_fp_lag)) {
                int num_groups_rr_fp = 2;
                TRUNK_CNTL(unit).ngroups_rr_fp = num_groups_rr_fp;

                if (NULL == TRUNK_CNTL(unit).fp_rr_lag_bmp) {
                    TRUNK_CNTL(unit).fp_rr_lag_bmp =
                    sal_alloc(SHR_BITALLOCSIZE(num_groups_rr_fp), "fp rr lags used bitmap");
                    if (NULL == TRUNK_CNTL(unit).fp_rr_lag_bmp) {
                        TRUNK_UNLOCK(unit);
                        _bcm_esw_trunk_deinit(unit);
                        return BCM_E_MEMORY;
                    }
                }
                sal_memset(TRUNK_CNTL(unit).fp_rr_lag_bmp, 0, SHR_BITALLOCSIZE(num_groups_rr_fp));
            }
#endif /* BCM_APACHE_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
            TRUNK_CNTL(unit).trunk_bmp_alloc =
                SHR_BITALLOCSIZE(ngroups_fp_max +
                                 TRUNK_CNTL(unit).ngroups_hg);

            /* Num FP trunks, num fabric trunks, bitmap of defined trunks */
            trunk_scache_size = sizeof(uint32) + sizeof(uint32) +
                TRUNK_CNTL(unit).trunk_bmp_alloc;

            if (NULL == TRUNK_CNTL(unit).trunk_bmp_ptr) {
                    TRUNK_CNTL(unit).trunk_bmp_ptr =
                        sal_alloc( TRUNK_CNTL(unit).trunk_bmp_alloc, "trunk used bitmap");
                    if (NULL == TRUNK_CNTL(unit).trunk_bmp_ptr) {
                        _bcm_esw_trunk_deinit(unit);
                        TRUNK_UNLOCK(unit);
                        return BCM_E_MEMORY;
                }
            }

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
            trunk_fp_hwf_flags_alloc_size = 0;
            trunk_hg_hwf_flags_alloc_size = 0;
            if (soc_feature(unit, soc_feature_port_lag_failover)) {
                /* FP trunk failover flags */
#if defined(BCM_TRIDENT_SUPPORT)
                if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
                    /* For Trident, ngproups_fp_max = 1024, nports_fp = 256,
                     * while the maximum number of front-panel trunk ports is 
                     * limited by the number of entries in
                     * SOURCE_TRUNK_MAP_TABLE, which is 4096. To efficiently 
                     * utilize scache space, for each trunk group, 1 byte of
                     * BCM_TRUNK_FLAG_xxx flags, 2 bytes of trunk size, and
                     * trunk-size bytes of hardware failover flags are stored.
                     */
                    trunk_fp_hwf_flags_alloc_size +=
                        (ngroups_fp_max * 3 +
                         soc_mem_index_count(unit, SOURCE_TRUNK_MAP_TABLEm)) * 
                        sizeof(uint8);
                } else
#if defined(BCM_TOMAHAWK3_SUPPORT)
                if (SOC_IS_TOMAHAWK3(unit)) {
                /*
                 * For TH3, soc_mem_index_count(unit, SOURCE_TRUNK_MAP_TABLEm)
                 * evaluates to 160
                 */
                    trunk_fp_hwf_flags_alloc_size +=
                        (ngroups_fp_max * 3 +
                         soc_mem_index_count(unit, SOURCE_TRUNK_MAP_TABLEm)) * 
                        sizeof(uint8);
                } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
#endif /* BCM_TRIDENT_SUPPORT */
                {
                    trunk_fp_hwf_flags_alloc_size +=
                        (ngroups_fp_max * (TRUNK_CNTL(unit).nports_fp + 1)) *
                        sizeof(uint8);
                }
            }

            if (soc_feature(unit, soc_feature_hg_trunk_failover)) {
                /* HG trunk failover flags */
                trunk_hg_hwf_flags_alloc_size +=
                    (TRUNK_CNTL(unit).ngroups_hg *
                     (TRUNK_CNTL(unit).nports_hg + 1)) *
                    sizeof(uint8);
            }

            if (0 != trunk_fp_hwf_flags_alloc_size) {
                if (NULL == TRUNK_CNTL(unit).fp_hwf_flags) {
                    TRUNK_CNTL(unit).fp_hwf_flags =
                        sal_alloc(trunk_fp_hwf_flags_alloc_size, "trunk fp hwf flags");
                }
                if (NULL == TRUNK_CNTL(unit).fp_hwf_flags) {
                    _bcm_esw_trunk_deinit(unit);
                    return BCM_E_MEMORY;
                }
                TRUNK_CNTL(unit).fp_hwf_alloc =
                    trunk_fp_hwf_flags_alloc_size;
                trunk_scache_size += trunk_fp_hwf_flags_alloc_size;
            }

            if (0 != trunk_hg_hwf_flags_alloc_size) {
                if (NULL == TRUNK_CNTL(unit).hg_hwf_flags) {
                    TRUNK_CNTL(unit).hg_hwf_flags =
                        sal_alloc(trunk_hg_hwf_flags_alloc_size, "trunk hg hwf flags");
                }
                if (NULL == TRUNK_CNTL(unit).hg_hwf_flags) {
                    _bcm_esw_trunk_deinit(unit);
                    return BCM_E_MEMORY;
                }
                TRUNK_CNTL(unit).hg_hwf_alloc =
                    trunk_hg_hwf_flags_alloc_size;
                trunk_scache_size += trunk_hg_hwf_flags_alloc_size;
            }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

            /* Allocate scache space for BCM_TRUNK_FLAG_IPMC_CLEAVE flag */
            trunk_scache_size += (ngroups_fp_max * sizeof(uint8));

            /* Allocate scache space for ipmc_psc */
            trunk_scache_size += (ngroups_fp_max * sizeof(int));

            /*allocate scache space for dlf_index */
            trunk_scache_size += (ngroups_fp_max * sizeof(int));


            /* Allocate scache space to store trunk membership */
#if defined(BCM_TRIDENT_SUPPORT)
            if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
                /* For each front-panel trunk group, store the following info:
                 * - 2 bytes containing the number of members, which can be
                 *   up to 256.
                 * - 2 bytes per member, containing the module and port IDs.
                 * - 1 byte per member, containing member flags.
                 *
                 * Trident can support up to 1k trunk groups. It's not 
                 * necessary to allocate space for 256 members per trunk group,
                 * since the maximum number of members Trident can support is
                 * limited by the TRUNK_MEMBER table.
                 */
                trunk_scache_size += (ngroups_fp_max * 2 +
                        soc_mem_index_count(unit, TRUNK_MEMBERm) * 3) *
                        sizeof(uint8);
            } else
#if defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_IS_TOMAHAWK3(unit)) {
                /* For each front-panel trunk group, store the following info:
                 * - 2 byte containing the number of members, which can be
                 *   up to 64.
                 * - 1 bytes per member, containing the port IDs. Mod Id is 0 always.
                 * - 1 byte per member, containing member flags.
                 */
                trunk_scache_size += (ngroups_fp_max * 2 +
                        soc_mem_index_count(unit, SOURCE_TRUNK_MAP_TABLEm) * 2) *
                        sizeof(uint8);
            } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
#endif /* BCM_TRIDENT_SUPPORT */
            {
                /* For each front-panel trunk group, store the following info:
                 * - 1 byte containing the number of members.
                 * - 2 bytes per member, containing the module and port IDs.
                 * - 1 byte per member, containing member flags.
                 */
                trunk_scache_size += 
                    (ngroups_fp_max * (1 + 3 * TRUNK_CNTL(unit).nports_fp))
                    * sizeof(uint8);
            }

            /* Allocate scache space for Higig dynamic load balancing info */
#ifdef BCM_TRIDENT_SUPPORT
            if (soc_feature(unit, soc_feature_hg_dlb) &&
                !soc_feature(unit, soc_feature_hgt_lag_dlb_optimized)) {
                int hg_dlb_scache_size;
                rv = bcm_trident_hg_dlb_wb_alloc_size_get(unit,
                            &hg_dlb_scache_size);
                if (SOC_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return rv;
                }
                trunk_scache_size += hg_dlb_scache_size;
            }
#endif /* BCM_TRIDENT_SUPPORT */

            /* Allocate scache space for LAG dynamic load balancing info */
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_lag_dlb) &&
                !soc_feature(unit, soc_feature_hgt_lag_dlb_optimized)) {
                int lag_dlb_scache_size;
                rv = bcm_tr3_lag_dlb_wb_alloc_size_get(unit,
                            &lag_dlb_scache_size);
                if (SOC_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return rv;
                }
                trunk_scache_size += lag_dlb_scache_size;
            }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TOMAHAWK2_SUPPORT
            if (soc_feature(unit, soc_feature_hgt_lag_dlb_optimized)) {
                int hgt_lag_dlb_scache_size;
                BCM_IF_ERROR_RETURN(bcm_th2_hgt_lag_dlb_wb_alloc_size_get(unit,
                            &hgt_lag_dlb_scache_size));
                trunk_scache_size += hgt_lag_dlb_scache_size;
            }
#endif /* BCM_TOMAHAWK2_SUPPORT */

            /* Allocate scache space for psc */
#if defined(BCM_TRIDENT_SUPPORT)
            if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
                trunk_scache_size += (ngroups_fp_max * sizeof(int));
            }
#endif /* BCM_TRIDENT_SUPPORT */

            /*
             * Allocate scache space for saving TRUNK_MEMBER table property.
             */
#if defined(BCM_TRIDENT_SUPPORT)
            if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
                trunk_scache_size += (sizeof(int));
            }
#endif /* BCM_TRIDENT_SUPPORT */

            /* Limited scache mode unsupported */
            if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
                SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TRUNK, 0);
                rv = _bcm_esw_scache_ptr_get(unit, scache_handle,
                        (0 == SOC_WARM_BOOT(unit)),
                        trunk_scache_size,
                        &trunk_scache, 
                        BCM_WB_DEFAULT_VERSION, NULL);
                if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                    TRUNK_UNLOCK(unit);
                    _bcm_esw_trunk_deinit(unit);
                    return rv;
                }
            }
#endif /* BCM_WARM_BOOT_SUPPORT */
        }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    }
#endif	/* BCM_XGS_SWITCH_SUPPORT */

#ifdef	BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
	bcm_port_t	port;

    TRUNK_CNTL(unit).ngroups_hg = SOC_REG_NUMELS(unit, ING_HGTRUNKr);
    TRUNK_CNTL(unit).nports_hg = 8;

        if (!SOC_WARM_BOOT(unit)) {
            for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_hg; tid++) {
                PBMP_PORT_ITER(unit, port) {
                    rv = WRITE_ING_HGTRUNKr(unit, port, tid, 0);
                    if (SOC_FAILURE(rv)) {
                        TRUNK_UNLOCK(unit);
                        return rv;
                    }
                }
            }
        }
    }
#endif	/* BCM_XGS12_FABRIC_SUPPORT */

    if (TRUNK_CNTL(unit).t_info != NULL) {
        sal_free(TRUNK_CNTL(unit).t_info);
        TRUNK_CNTL(unit).t_info = NULL;

    }

    if (TRUNK_NUM_GROUPS(unit) > 0) {
        TRUNK_CNTL(unit).t_info = 
            sal_alloc(TRUNK_NUM_GROUPS(unit) * sizeof(trunk_private_t),
                      "trunk_priv");
        if (NULL == TRUNK_CNTL(unit).t_info) {
            TRUNK_UNLOCK(unit);
            _bcm_esw_trunk_deinit(unit);
            return (BCM_E_MEMORY);
        }

	t_info = TRUNK_CNTL(unit).t_info;

        /* Clear the state for Cold or Warm Boot.
         * Warm Boot will update below. */
        for (tid = 0; tid < TRUNK_NUM_GROUPS(unit); tid++) {
            t_info->tid = BCM_TRUNK_INVALID;
            t_info->in_use = FALSE;
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                t_info->psc = BCM_TRUNK_PSC_PORTFLOW;
            } else
#endif
            {
                t_info->psc = BCM_TRUNK_PSC_DEFAULT;
            }
            t_info->ipmc_psc = 0;
            t_info->rtag = 0;
            t_info->flags = 0;
            t_info->dlf_index_spec = -1;
            t_info->dlf_index_used = -1;
            t_info->dlf_port_used = 0;
            t_info->mc_index_spec = -1;
            t_info->mc_index_used = -1;
            t_info->mc_port_used = 0;
            t_info->ipmc_index_spec = -1;
            t_info->ipmc_index_used = -1;
            t_info->ipmc_port_used = 0;
            t_info->dynamic_size = 0;
            t_info->dynamic_age = 0;
            t_info->dynamic_load_exponent = 0;
            t_info->dynamic_expected_load_exponent = 0;
            t_info->vlan_vp_count = 0;
            t_info++;
        }
    }

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vp_lag)) {
        rv = bcm_td2_vp_lag_init(unit);
        if (BCM_FAILURE(rv)) {
            TRUNK_UNLOCK(unit);
            return rv;
        }

    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

#if defined(BCM_XGS_FABRIC_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
      if (/*!SOC_IS_TOMAHAWK3(unit)*/ 1) {
        rv = _bcm_trunk_reinit(unit);
        if (BCM_FAILURE(rv)) {
            TRUNK_UNLOCK(unit);
            return rv;
        }
      }

    }
#endif

    TRUNK_UNLOCK(unit);

    return BCM_E_NONE;
}


/*
 * Function:
 *	bcm_trunk_detach
 * Purpose:
 *	Cleans up the trunk tables.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 */

int
bcm_esw_trunk_detach(int unit)
{
    /* Return if trunk was not initialized. */
    if (NULL == TRUNK_CNTL(unit).t_info) {
        return (BCM_E_NONE);
    }

    sal_free(TRUNK_CNTL(unit).t_info);
    /* Destroy protection mutex. */
    if (NULL != TRUNK_CNTL(unit).lock) {
        sal_mutex_destroy(TRUNK_CNTL(unit).lock);
        TRUNK_CNTL(unit).lock = NULL;
    }
    _bcm_esw_trunk_deinit(unit);
    sal_memset(&TRUNK_CNTL(unit), 0, sizeof(trunk_cntl_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_trunk_chip_info_vp_resource_get
 * Purpose:
 *      Get the VP related information.
 *      Be called internally for forward compatibility.
 * Parameters:
 *      unit         - StrataSwitch PCI device unit number.
 *      vp_id_min    - Minimum virtual port trunk ID number. 
 *      vp_id_max    - Maximum virtual port trunk ID number. 
 *      vp_ports_max - Maximum number of virtual ports per virtual
 *                     port trunk group.
 * Returns:
 *      BCM_E_NONE     Success.
 * Notes:
 *      None.
 */
int
_bcm_esw_trunk_chip_info_vp_resource_get(int unit, int *vp_id_min, 
                                         int *vp_id_max, int *vp_ports_max)
{
    /* Initialize the VP LAG information */
    if (vp_id_min != NULL) {
        *vp_id_min = -1;
    }

    if (vp_id_max != NULL) {
        *vp_id_max = -1;
    }

    if (vp_ports_max != NULL) {
        *vp_ports_max = 0;
    }

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vp_lag)) {
        int ngroups_vp_lag = 0, nports_vp_lag = 0;

        BCM_IF_ERROR_RETURN(bcm_td2_vp_lag_info_get(unit, &ngroups_vp_lag,
                                                    &nports_vp_lag));
        if (ngroups_vp_lag > 0) {

            if (vp_id_min != NULL) {
                *vp_id_min = TRUNK_CNTL(unit).ngroups_fp +
                    TRUNK_CNTL(unit).ngroups_hg;
            }

            if (vp_id_max != NULL) {
                *vp_id_max = TRUNK_CNTL(unit).ngroups_fp +
                    TRUNK_CNTL(unit).ngroups_hg + ngroups_vp_lag - 1;
            }

            if (vp_ports_max != NULL) {
                *vp_ports_max = nports_vp_lag;
            }
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_esw_trunk_id_is_vp_lag
 * Purpose:
 *      Determine if trunk ID is a VP LAG.
 * Parameters:
 *      unit  - SOC unit number
 *      tid   - Trunk ID
 *      tid_is_vp_lag - (OUT) Indicates if trun ID is a VP LAG
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_esw_trunk_id_is_vp_lag(int unit, bcm_trunk_t tid, int *tid_is_vp_lag)
{
    int vp_id_min = -1, vp_id_max = -1;

    if (tid < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_trunk_chip_info_vp_resource_get(unit, &vp_id_min,
                                                 &vp_id_max, NULL));

    if (tid >= vp_id_min && tid <= vp_id_max) {
        *tid_is_vp_lag = TRUE;
    } else {
        *tid_is_vp_lag = FALSE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_esw_trunk_tid_to_vp_lag_vp
 * Purpose:
 *      Convert trunk ID to the VP value representing VP LAG.
 * Parameters:
 *      unit  - SOC unit number
 *      tid   - Trunk ID
 *      vp_lag_vp - (OUT) VP value
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_esw_trunk_tid_to_vp_lag_vp(int unit, bcm_trunk_t tid, int *vp_lag_vp)
{
    int tid_is_vp_lag;

    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag));

    if (tid_is_vp_lag) {
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vp_lag)) {
            return bcm_td2_tid_to_vp_lag_vp(unit, tid, vp_lag_vp);
        } else
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
        {
            return BCM_E_PARAM;
        }
    } else {
        return BCM_E_PARAM;
    }
}

/*
 * Function:
 *	_bcm_esw_trunk_vp_lag_vp_to_tid
 * Purpose:
 *      Convert the VP value representing a VP LAG to trunk ID.
 * Parameters:
 *      unit      - SOC unit number
 *      vp_lag_vp - VP value
 *      tid       - (OUT) Trunk ID
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_esw_trunk_vp_lag_vp_to_tid(int unit, int vp_lag_vp, bcm_trunk_t *tid)
{
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vp_lag)) {
        return bcm_td2_vp_lag_vp_to_tid(unit, vp_lag_vp, tid);
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	_bcm_trunk_create_id
 * Purpose:
 *      Create the software data structure for the specified trunk ID.
 *      This function does not update any hardware tables,
 *      must call bcm_trunk_set() to finish trunk setting.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - The trunk ID.
 * Returns:
 *      BCM_E_NONE   - Success.
 *      BCM_E_INIT   - trunking software not initialized
 *      BCM_E_EXISTS - TID already used
 *      BCM_E_BADID  - TID out of range
 */
STATIC int
_bcm_esw_trunk_create_id(int unit, bcm_trunk_t tid)
{
    trunk_private_t *t_info;
    int rv;

    TRUNK_CHECK(unit, tid);

    TRUNK_LOCK(unit);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        t_info->tid = tid;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        t_info->psc = BCM_TRUNK_PSC_PORTFLOW;
    } else
#endif
    {
        t_info->psc = BCM_TRUNK_PSC_DEFAULT;
    }
        t_info->in_use = FALSE;
#ifdef BCM_APACHE_SUPPORT
        t_info->rr_lag_select = -1;
#endif /* BCM_APACHE_SUPPORT */
        rv = BCM_E_NONE;
    } else {
	rv = BCM_E_EXISTS;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    TRUNK_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *	bcm_trunk_vplag_get
 * Purpose:
 *      GET VPLAG port Id for VPLAG trunk.
 * Parameters:
 *      unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *      tid - (IN) The trunk ID.
 *      vplag_port_id - (OUT) VPLAG Port Id
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_PARAM     - Incorrect LAG Id
 *      BCM_E_NOT_FOUND - LAG Id not used
 */
int
bcm_esw_trunk_vplag_get(int unit, bcm_trunk_t tid, bcm_gport_t *vplag_port_id)
{
    int rv = BCM_E_NONE;

    if (!soc_feature(unit, soc_feature_vp_lag)) {
        return BCM_E_UNAVAIL;

    } else {
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
        int vp_id;
        rv = bcm_td2_tid_to_vp_lag_vp(unit, tid, &vp_id);
        if (BCM_SUCCESS(rv)) {
            BCM_GPORT_VPLAG_PORT_ID_SET(*vplag_port_id, vp_id);
        }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
    }

    return rv;
}

/*
 * Function:
 *	bcm_trunk_vplag_create
 * Purpose:
 *      Create VPLAG trunk.
 * Parameters:
 *      unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *      flags - (IN) Flags.
 *      tid - (IN/OUT) The trunk ID.
 *      vplag_port_id - (IN/OUT) VPLAG Port Id
 * Returns:
 *      BCM_E_NONE     - Success.
 *      BCM_E_FULL - run out of TIDs
 */
int
bcm_esw_trunk_with_vplag_create(int unit, uint32 flags, bcm_trunk_t *tid,
                           bcm_gport_t *vplag_port_id)
{
    int rv = BCM_E_NONE;

    TRUNK_INIT(unit);

    if (NULL == tid) {
        return BCM_E_PARAM;
    }

    if (!soc_feature(unit, soc_feature_vp_lag)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (flags & BCM_TRUNK_FLAG_WITH_ID) {
        int tid_is_vp_lag = 0;
        BCM_IF_ERROR_RETURN
            (_bcm_esw_trunk_id_is_vp_lag(unit, *tid, &tid_is_vp_lag));

        if (!(tid_is_vp_lag)) {
            return BCM_E_PARAM;
        }
    }

    VPLAG_LOCK(unit);
    rv = bcm_td2_vp_lag_create(unit, flags, tid, vplag_port_id);
    VPLAG_UNLOCK(unit);
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return rv;
}

/*
 * Function:
 *	bcm_trunk_create
 * Purpose:
 *      Create the software data structure for a trunk ID.
 *      This function does not update any hardware tables,
 *      must call bcm_trunk_set() to finish trunk setting.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      flags - Flags.
 *      tid - (Out), The trunk ID.
 * Returns:
 *      BCM_E_NONE     - Success.
 *      BCM_E_FULL - run out of TIDs
 */

int
bcm_esw_trunk_create(int unit, uint32 flags, bcm_trunk_t *tid)
{
    trunk_private_t *t_info;
    int rv, i;
    bcm_trunk_t max_tid;
    int tid_is_vp_lag = 0;

    TRUNK_INIT(unit);

    if (NULL == tid) {
        return BCM_E_PARAM;
    }

    if (flags & BCM_TRUNK_FLAG_WITH_ID) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_trunk_id_is_vp_lag(unit, *tid, &tid_is_vp_lag));
    }

    if (tid_is_vp_lag || (flags & BCM_TRUNK_FLAG_VP)) {
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vp_lag)) {
            VPLAG_LOCK(unit);
            rv = bcm_td2_vp_lag_create(unit, flags, tid, NULL);
            VPLAG_UNLOCK(unit);
            return rv;
        } else
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
        {
            return BCM_E_PARAM;
        }
    }

    if (flags & BCM_TRUNK_FLAG_WITH_ID) {
        return _bcm_esw_trunk_create_id(unit, *tid);
    }

    TRUNK_LOCK(unit);

    rv = BCM_E_FULL;

    t_info = TRUNK_CNTL(unit).t_info;

    if (NULL == t_info) {
        return BCM_E_PARAM;
    }

    if (TRUNK_CNTL(unit).ngroups_fp == 0) {
        max_tid = TRUNK_CNTL(unit).ngroups_hg - 1;
    }
    else {
        max_tid = TRUNK_CNTL(unit).ngroups_fp - 1;
    }

    for (i = 0; i <= max_tid; i++) {
        if (t_info->tid == BCM_TRUNK_INVALID) {
            t_info->tid = i;
            t_info->in_use = FALSE;
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            t_info->psc = BCM_TRUNK_PSC_PORTFLOW;
        } else
#endif
        {
            t_info->psc = BCM_TRUNK_PSC_DEFAULT;
        }
            *tid = i;
            rv = BCM_E_NONE;
            break;
        }
        t_info++;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    TRUNK_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *	_bcm_trunk_id_validate
 * Purpose:
 *  Service routine to validate validity of trunk id.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - The trunk ID.
 * Returns:
 *      BCM_E_NONE    - Success.
 *      BCM_E_INIT    - trunking software not initialized
 *      BCM_E_BADID   - TID out of range
 */
int
_bcm_trunk_id_validate(int unit, bcm_trunk_t tid)
{
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vp_lag)) {
        int tid_is_vp_lag;

        BCM_IF_ERROR_RETURN
            (_bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag));
        if (tid_is_vp_lag) {
            return BCM_E_NONE;
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    TRUNK_CHECK(unit, tid);

    return (BCM_E_NONE);
}


/*
 * Function:
 *      bcm_trunk_psc_get
 * Purpose:
 *      Get the trunk selection criteria.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - The trunk ID to be used.
 *      psc - (OUT) Identify the trunk selection criteria.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 */

int
bcm_esw_trunk_psc_get(int unit, bcm_trunk_t tid, int *psc)
{
    trunk_private_t *t_info;
    int rv;

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
	*psc = 0;
        rv = BCM_E_NOT_FOUND;
    } else {
	*psc = t_info->psc;
	rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *	bcm_trunk_chip_info_get
 * Purpose:
 *      Get the trunk information.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      t_info - (OUT) Identify the trunk selection criteria.
 * Returns:
 *      BCM_E_NONE              Success.
 * Notes:
 *      None.
 */

int
bcm_esw_trunk_chip_info_get(int unit, bcm_trunk_chip_info_t *ta_info)
{
    TRUNK_INIT(unit);

    ta_info->trunk_group_count = TRUNK_CNTL(unit).ngroups_fp;

    if (TRUNK_CNTL(unit).ngroups_fp > 0) {
        ta_info->trunk_id_min = 0;
	ta_info->trunk_id_max = TRUNK_CNTL(unit).ngroups_fp - 1;
    }
    else {
        ta_info->trunk_id_min = -1;
	ta_info->trunk_id_max = -1;
    }
    ta_info->trunk_ports_max = TRUNK_CNTL(unit).nports_fp;

    if (TRUNK_CNTL(unit).ngroups_hg > 0) {
        ta_info->trunk_fabric_id_min = TRUNK_CNTL(unit).ngroups_fp;
	ta_info->trunk_fabric_id_max =
	  TRUNK_CNTL(unit).ngroups_fp + TRUNK_CNTL(unit).ngroups_hg - 1;
    }
    else {
        ta_info->trunk_fabric_id_min = -1;
	ta_info->trunk_fabric_id_max = -1;
    }
    ta_info->trunk_fabric_ports_max = TRUNK_CNTL(unit).nports_hg;

    ta_info->vp_id_min = -1;
    ta_info->vp_id_max = -1;
    ta_info->vp_ports_max = 0;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_trunk_chip_info_vp_resource_get(unit, &(ta_info->vp_id_min),
                                                 &(ta_info->vp_id_max),
                                                 &(ta_info->vp_ports_max)));
    /* Get available VP LAG resource ONLY for API return */
    if (soc_feature(unit, soc_feature_reserve_vp_lag_resource_index_zero)) {
        /*
         * Since entry 0 of VP LAG releated resource (such as EGR_VPLAG_GROUP,
         * EGR_VPLAG_MEMBER table) are reserved, so the maximum number
         * of VP LAGs which are capable of performing VP LAG resolution should
         * be decreased by 1, and the initial tunnel id of VG LAG usage should
         * be increased by 1.
         */
        if (ta_info->vp_id_max > ta_info->vp_id_min) {
            ta_info->vp_id_min += 1;
        } else {
            /* Reset the vp resource */
            ta_info->vp_id_min = -1;
            ta_info->vp_id_max = -1;
            ta_info->vp_ports_max = 0;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_trunk_member_compar
 * Purpose:
 *      Internal utility routine for sorting on trunk member gport.
 * Parameters:
 *      a - Pointer to trunk member.
 *      b - Pointer to trunk member.
 * Returns:
 *      -1 - a is less than b.
 *      0 - a equals b.
 *      1 - a is more than b.
 */

STATIC int
_bcm_esw_trunk_member_compar(void *a, void *b)
{
    bcm_trunk_member_t *d1 = a;
    bcm_trunk_member_t *d2 = b;

    return (d1->gport < d2->gport ? -1 :
            d1->gport > d2->gport ? 1 : 0);
}

/*
 * Function:
 *      bcm_esw_trunk_sort
 * Purpose:
 *      Sort members in a trunk group.
 * Parameters:
 *      member_count - Number of trunk members.
 *      member_array - Array of trunk members.
 * Returns:
 *      BCM_E_XXX
 */
 
STATIC int
_bcm_esw_trunk_sort(int member_count, bcm_trunk_member_t **member_array)
{
    if (member_count > 0) {
        if (NULL == *member_array) {
            return BCM_E_PARAM;
        }
    } else {
        return BCM_E_NONE;
    }

    /* Sort members in the accending order */
    _shr_sort(*member_array, member_count, sizeof (bcm_trunk_member_t), _bcm_esw_trunk_member_compar);

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_esw_trunk_modify
 * Purpose:
 *      Set, add, or delete members to/from a trunk group.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - The trunk ID to be affected.
 *      trunk_info - Information on the trunk group.
 *      member_count - Number of trunk members.
 *      member_array - Array of trunk members.
 *      op - Type of operation: TRUNK_MEMBER_OP_SET, ADD, or DELETE.
 *      member - The member to add or delete.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_trunk_modify(int unit, bcm_trunk_t tid,
        bcm_trunk_info_t *trunk_info, int member_count,
        bcm_trunk_member_t *member_array, int op,
        bcm_trunk_member_t *member)
{
    int rv;
    trunk_private_t  *t_info;
    bcm_trunk_info_t local_trunk_info;
    int tid_is_vp_lag;
    bcm_module_t modid;
    bcm_port_t port;
    bcm_trunk_t tgid;
    int id;
    int i;

    TRUNK_INIT(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag));
    if (tid_is_vp_lag) {
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vp_lag)) {
            VPLAG_LOCK(unit);
            rv = bcm_td2_vp_lag_set(unit, tid, trunk_info, member_count,
                    member_array);
            VPLAG_UNLOCK(unit);
            return rv;
        } else
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
        {
            return BCM_E_PARAM;
        }
    }

    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        return BCM_E_NOT_FOUND;
    }

    if (NULL == trunk_info) {
        return BCM_E_PARAM;
    }

    if (member_count > 0) {
        if (NULL == member_array) {
            return BCM_E_PARAM;
        }
    } else {
        if (NULL != member_array) {
            return BCM_E_PARAM;
        }
    }

    /* Sort trunk members */
    if ((trunk_info->flags & BCM_TRUNK_FLAG_MEMBER_SORT)) {
        for (i = 0; i < member_count; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_gport_resolve(unit, member_array[i].gport,
                                        &modid, &port, &tgid, &id));
            SOC_GPORT_MODPORT_SET(member_array[i].gport, modid, port);  
        }
        rv = _bcm_esw_trunk_sort(member_count, &member_array);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
    
    /* Check number of ports in trunk group */
    if ((TRUNK_FP_TID(unit, tid) &&
         (member_count > TRUNK_CNTL(unit).nports_fp)) ||
        (TRUNK_FABRIC_TID(unit, tid) &&
         (member_count > TRUNK_CNTL(unit).nports_hg))) {
        return BCM_E_PARAM;
    }

#if defined (BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_apache_round_robin_fp_lag)) {
        int rr_lag_sel;
        soc_reg_t rr_counter_reg[] = {RR_COUNTER_1r, RR_COUNTER_2r};

        if ((t_info->psc != trunk_info->psc) &&
            (trunk_info->psc == BCM_TRUNK_PSC_ROUND_ROBIN)) {
            /* If PSC is changed to RR Lag, reserve RR_LAG_SELECT */

            for (rr_lag_sel = 0; rr_lag_sel < TRUNK_CNTL(unit).ngroups_rr_fp; rr_lag_sel++) {
                if (!SHR_BITGET(TRUNK_CNTL(unit).fp_rr_lag_bmp, rr_lag_sel)) {
                    t_info->rr_lag_select = rr_lag_sel;
                    break;
                }
            }
            if (rr_lag_sel == TRUNK_CNTL(unit).ngroups_rr_fp) {
                return BCM_E_RESOURCE;
            }
            SHR_BITSET(TRUNK_CNTL(unit).fp_rr_lag_bmp, rr_lag_sel);
            soc_reg32_set(unit, rr_counter_reg[rr_lag_sel], REG_PORT_ANY, 0, 0);

        } else if ((t_info->psc != trunk_info->psc) &&
                   (t_info->psc == BCM_TRUNK_PSC_ROUND_ROBIN)) {
            /* If PSC is changed from RR Lag, unreserve RR_LAG_SELECT */

            rr_lag_sel = t_info->rr_lag_select;
            t_info->rr_lag_select = -1;
            SHR_BITCLR(TRUNK_CNTL(unit).fp_rr_lag_bmp, rr_lag_sel);
            soc_reg32_set(unit, rr_counter_reg[rr_lag_sel], REG_PORT_ANY, 0, 0);
        }
    }
#endif /* BCM_APACHE_SUPPORT */

    bcm_trunk_info_t_init(&local_trunk_info);
    sal_memcpy(&local_trunk_info, trunk_info, sizeof(bcm_trunk_info_t));

    TRUNK_LOCK(unit);

    rv = mbcm_driver[unit]->mbcm_trunk_modify(unit, tid, &local_trunk_info,
            member_count, member_array, t_info, op, member);
    if (BCM_SUCCESS(rv)) {
        t_info->flags = trunk_info->flags;
        t_info->psc = trunk_info->psc;
        t_info->ipmc_psc = trunk_info->ipmc_psc;
        if (member_count > 0) {
            t_info->in_use = TRUE;
        }
    }

    TRUNK_UNLOCK(unit);

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    /* reconcile Mirror to Port programming with trunk member ports. */
    if (SOC_IS_XGS3_SWITCH(unit) && BCM_SUCCESS(rv)
            && !SOC_IS_TOMAHAWK3(unit)) {
        rv = _bcm_xgs3_mirror_trunk_update(unit, tid);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vp)) {
        if (BCM_SUCCESS(rv) && t_info->vlan_vp_count != 0) {
            /* This trunk is used by a virtual VLAN-port */
            /* update the vp_port count of newly added/deleted member */
            if (op == TRUNK_MEMBER_OP_DELETE) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_vlan_vp_port_cnt_update(unit,
                        member->gport, 0, FALSE));
            }
            else if (op == TRUNK_MEMBER_OP_ADD) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_vlan_vp_port_cnt_update(unit,
                        member->gport, 0, TRUE));
            }
            else {      /* TRUNK_MEMBER_OP_SET */
                for (i = 0; i < member_count; i++) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_tr2_vlan_vp_port_cnt_update(unit,
                            member_array[i].gport, 0, TRUE));
                }
            }
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

    return rv;
}

/*
 * Function:
 *      _bcm_esw_trunk_set
 * Purpose:
 *      Adds ports to a trunk group.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - The trunk ID to be affected.
 *      trunk_info - Information on the trunk group.
 *      member_count - Number of trunk members.
 *      member_array - Array of trunk members.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 * Notes:
        helper function callable by both bcm_esw_trunk_set()
 *      and bcm_esw_trunk_psc_set()
 */
STATIC int
_bcm_esw_trunk_set(int unit, bcm_trunk_t tid, bcm_trunk_info_t *trunk_info,
        int member_count, bcm_trunk_member_t *member_array)
{
    int rv = BCM_E_NONE;


#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        rv = _bcm_esw_trunk_subport_sanitize(unit, member_count, member_array,
                NULL);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TRUNK,
                (BSL_META_U(unit,"subport and phy port mixed in trunk.\n")));
            return rv;
        }
    }
#endif /*BCM_CHANNELIZED_SWITCHING_SUPPORT*/

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
    if (soc_feature(unit, soc_feature_epc_linkflap_recover)) {
        int  index, i = 0, count = 0, status;
        bcm_trunk_member_t *temp_member_array;

        temp_member_array = sal_alloc(
            sizeof(bcm_trunk_member_t) * member_count, "TR3 trunk WAR");
        if (temp_member_array == NULL) {
            return BCM_E_MEMORY;
        }
        for (index = 0; index < member_count; index++) {
             status = 1;
             SOC_LINKSCAN_MODPORT_MAP_LOCK(unit);
             rv =  _bcm_td_stk_modport_map_member_link_handler(unit,
                   member_array[index].gport, tid, 0, &status);
             SOC_LINKSCAN_MODPORT_MAP_UNLOCK(unit);
             if (BCM_FAILURE(rv)) {
                 sal_free(temp_member_array);
                 return (rv);
             }
             if (status == 0) {
                 count++;
             } else {
                 temp_member_array[i] = member_array[index];
                 i++;
             }
        }
        if ((count != 0) && (count != member_count)) {
            rv =  _bcm_esw_trunk_modify(unit, tid, trunk_info,
                                        member_count - count,
                                        temp_member_array,
                                        TRUNK_MEMBER_OP_SET, NULL);
        } else {
            rv = _bcm_esw_trunk_modify(unit, tid, trunk_info,
                                       member_count,
                                       member_array,
                                       TRUNK_MEMBER_OP_SET, NULL);
        }
        sal_free(temp_member_array);
     } else
#endif /* #(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT) */
        {
         rv =  _bcm_esw_trunk_modify(unit, tid, trunk_info,
                                     member_count,
                                     member_array,
                                     TRUNK_MEMBER_OP_SET, NULL);
        }
     return rv;
}

#ifdef BCM_TOMAHAWK3_SUPPORT
/*
 * Function:
 *      _bcm_th3_trunk_psc_validate
 * Purpose:
 *      Set the trunk selection criteria.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - The trunk ID to be affected.
 *      psc - Identify the trunk selection criteria.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 * Notes: th3 helper function callable by both bcm_esw_trunk_set()
 *      and bcm_esw_trunk_psc_set() for th3
 */
STATIC int
_bcm_th3_trunk_psc_validate(int unit, bcm_trunk_t tid, int psc)
                    
{
    trunk_private_t  *t_info;

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if ((psc == 0) || (t_info->psc == psc)) {
        return BCM_E_NONE;
    }

    /* If the new psc is one of the dynamic load balancing modes, but the old
     * psc is not, then return BCM_E_PARAM, since dynamic_size and dynamic_age
     * parameters are not available when calling bcm_trunk_set.
     */
    if (!((psc == BCM_TRUNK_PSC_PORTFLOW) ||
          (psc == BCM_TRUNK_PSC_RANDOMIZED))) {
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      bcm_trunk_psc_set
 * Purpose:
 *      Set the trunk selection criteria.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - The trunk ID to be affected.
 *      psc - Identify the trunk selection criteria.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 */

int
bcm_esw_trunk_psc_set(int unit, bcm_trunk_t tid, int psc)
{
    trunk_private_t  *t_info;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t *member_array = NULL;
    int member_count = 0, unused_count;
    int rv = BCM_E_NONE;

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
            return BCM_E_NOT_FOUND;
    }
    if (psc <= 0) {
        if (!SOC_IS_TOMAHAWK3(unit)) {
            psc = BCM_TRUNK_PSC_DEFAULT;
        } else {
            psc = BCM_TRUNK_PSC_PORTFLOW;
        }
    }
    if (t_info->psc == psc) {
            return BCM_E_NONE;
    }
    /* upto above is common for Legacy and TH3 */
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        rv = _bcm_th3_trunk_psc_validate(unit, tid, psc);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        BCM_IF_ERROR_RETURN(bcm_esw_trunk_get(unit, tid, &trunk_info, 0, NULL,
                              &member_count));
    } else 
#endif
    {
        if (!t_info->in_use) {
            t_info->psc = psc;
            return BCM_E_NONE;
        }
        BCM_IF_ERROR_RETURN(bcm_esw_trunk_get(unit, tid, &trunk_info, 0, NULL,
                    &member_count));
    
        /* If the new psc is one of the dynamic load balancing modes, but the old
         * psc is not, then return BCM_E_PARAM, since dynamic_size and dynamic_age
         * parameters are not available when calling bcm_trunk_set.
         */
        if ((psc == BCM_TRUNK_PSC_DYNAMIC) ||
            (psc == BCM_TRUNK_PSC_DYNAMIC_ASSIGNED) ||
            (psc == BCM_TRUNK_PSC_DYNAMIC_OPTIMAL)) {
            if ((trunk_info.psc != BCM_TRUNK_PSC_DYNAMIC) &&
                (trunk_info.psc != BCM_TRUNK_PSC_DYNAMIC_ASSIGNED) &&
                (trunk_info.psc != BCM_TRUNK_PSC_DYNAMIC_OPTIMAL)) {
                return BCM_E_PARAM;
            }
        }
    
        /* If the new psc is the resilient hashing mode, but the old
         * psc is not, then return BCM_E_PARAM, since the dynamic_size
         * parameter is not available when calling bcm_trunk_set.
         */
        if (psc == BCM_TRUNK_PSC_DYNAMIC_RESILIENT) {
            if (trunk_info.psc != BCM_TRUNK_PSC_DYNAMIC_RESILIENT) {
                return BCM_E_PARAM;
            }
        }
    }

    if (member_count > 0) {
        member_array = sal_alloc(sizeof(bcm_trunk_member_t) * member_count,
                "trunk member array");
        if (NULL == member_array) {
            return BCM_E_MEMORY;
        }
        rv = bcm_esw_trunk_get(unit, tid, &trunk_info, member_count,
                member_array, &unused_count);
        if (BCM_FAILURE(rv)) {
            sal_free(member_array);
            return rv;
        }
    } 
    trunk_info.psc = psc;
    trunk_info.flags = 0; /* Don't change failover config */
    rv = _bcm_esw_trunk_set(unit, tid, &trunk_info, member_count, member_array);
    if (NULL != member_array) {
        sal_free(member_array);
    }
    return rv;
}

/*
 * Function:
 *	bcm_trunk_set
 * Purpose:
 *      Adds ports to a trunk group.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - The trunk ID to be affected.
 *      trunk_info - Information on the trunk group.
 *      member_count - Number of trunk members.
 *      member_array - Array of trunk members.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 * Notes:
 *      The existing ports in the trunk group will be replaced with new ones.
 */
int
bcm_esw_trunk_set(int unit, bcm_trunk_t tid, bcm_trunk_info_t *trunk_info,
        int member_count, bcm_trunk_member_t *member_array)
{
    int rv;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        if (trunk_info->psc < 0) {
            trunk_info->psc = BCM_TRUNK_PSC_PORTFLOW;
        }
        rv = _bcm_th3_trunk_psc_validate(unit, tid, trunk_info->psc);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
#endif
    rv =  _bcm_esw_trunk_set(unit, tid, trunk_info,
                                   member_count, member_array);
    return rv;
}

/*
 * Function:
 *	bcm_esw_trunk_destroy
 * Purpose:
 *	Removes a trunk group.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - Trunk Id.
 * Returns:
 *      BCM_E_NONE     Success.
 *      BCM_E_XXX
 */

int
bcm_esw_trunk_destroy(int unit, bcm_trunk_t tid)
{
    trunk_private_t *t_info;
    int rv = BCM_E_NONE;
    int tid_is_vp_lag;
    bcm_trunk_chip_info_t chip_info;

    TRUNK_INIT(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag));
    if (tid_is_vp_lag) {
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vp_lag)) {
            VPLAG_LOCK(unit);
            rv = bcm_td2_vp_lag_destroy(unit, tid);
            VPLAG_UNLOCK(unit);
            return rv;
        } else
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
        {
            return BCM_E_PARAM;
        }
    }

    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        return (BCM_E_NOT_FOUND);
    }

    TRUNK_LOCK(unit);

    if (t_info->in_use) {
        rv = mbcm_driver[unit]->mbcm_trunk_destroy(unit, tid, t_info);
    } else {
        /* Trunk ID is valid but there are no members.
         * Need to clear resilient hashing state in hardware.
         */
        rv = bcm_esw_trunk_chip_info_get(unit, &chip_info);
        if (BCM_FAILURE(rv)) {
            TRUNK_UNLOCK(unit);
            return rv;
        }
        if (chip_info.trunk_fabric_id_min >= 0 &&
                tid >= chip_info.trunk_fabric_id_min) {
#ifdef BCM_TRIDENT2_SUPPORT
            int hgtid;
            hgtid = tid - chip_info.trunk_fabric_id_min;
            if (soc_feature(unit, soc_feature_hg_resilient_hash)) {
                /* Clear Higig resilient hashing configuration */
                rv = bcm_td2_hg_rh_free_resource(unit, hgtid);
            }
#endif /* BCM_TRIDENT2_SUPPORT */
        } else {
#ifdef BCM_TRIDENT2_SUPPORT
            if (soc_feature(unit, soc_feature_lag_resilient_hash)) {
                /* Clear LAG resilient hashing configuration */
                rv = bcm_td2_lag_rh_free_resource(unit, tid);
            }
#endif /* BCM_TRIDENT2_SUPPORT */
        }
        if (BCM_FAILURE(rv)) {
            TRUNK_UNLOCK(unit);
            return rv;
        }
    }

#ifdef BCM_APACHE_SUPPORT
    if (soc_feature(unit, soc_feature_apache_round_robin_fp_lag)) {
        if (t_info->rr_lag_select != -1) {
            SHR_BITCLR(TRUNK_CNTL(unit).fp_rr_lag_bmp, t_info->rr_lag_select);
            t_info->rr_lag_select = -1;
        }
    }
#endif /* BCM_APACHE_SUPPORT */

    t_info->tid = BCM_TRUNK_INVALID;
    t_info->in_use = FALSE;
    t_info->psc = BCM_TRUNK_PSC_DEFAULT;
    t_info->ipmc_psc = 0;
    t_info->rtag = 0;
    t_info->flags = 0;
    t_info->dlf_index_spec = -1;
    t_info->dlf_index_used = -1;
    t_info->dlf_port_used = 0;
    t_info->mc_index_spec = -1;
    t_info->mc_index_used = -1;
    t_info->mc_port_used = 0;
    t_info->ipmc_index_spec = -1;
    t_info->ipmc_index_used = -1;
    t_info->ipmc_port_used = 0;
    t_info->dynamic_size = 0;
    t_info->dynamic_age = 0;
    t_info->dynamic_load_exponent = 0;
    t_info->dynamic_expected_load_exponent = 0;
    t_info->vlan_vp_count = 0;

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    TRUNK_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_esw_trunk_match_multi_get
 * Purpose:
 *      Get all the matches for an existing tid.
 * Parameters:
 *      unit -        (IN) Unit number.
 *      tid -         (IN) Trunk Id
 *      size -        (IN) Number of elements in match array
 *      match_array - (OUT) Match array
 *      count -       (OUT) Match count
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_trunk_match_multi_get(
    int unit,
    bcm_trunk_t tid,
    int size,
    bcm_port_match_info_t *match_array,
    int *count)
{
    int              tid_is_vp_lag;
    int              rv = BCM_E_UNAVAIL;

    TRUNK_INIT(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag));
    if (tid_is_vp_lag) {
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vp_lag)) {
            VPLAG_LOCK(unit);
            rv = _bcm_td2_vp_lag_match_multi_get(unit, tid, size,
                                                 match_array, count);
            VPLAG_UNLOCK(unit);
            return rv;
        } else
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
        {
            return BCM_E_PARAM;
        }
    }
    return rv;
}

/*
 * Function:
 *	bcm_esw_trunk_get
 * Purpose:
 *      Return a port information of given trunk ID.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      tid    - Trunk ID.
 *      trunk_info   - (OUT) Place to store returned trunk info.
 *      member_max   - (IN) Size of member_array.
 *      member_array - (OUT) Place to store returned trunk members.
 *      member_count - (OUT) Place to store returned number of trunk members.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 */
int
bcm_esw_trunk_get(int unit, bcm_trunk_t tid, bcm_trunk_info_t *trunk_info,
        int member_max, bcm_trunk_member_t *member_array, int *member_count)
{
    trunk_private_t  *t_info;
    int              rv;
    bcm_trunk_info_t local_trunk_info;
    int              local_member_count;
    int              tid_is_vp_lag;

    TRUNK_INIT(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag));
    if (tid_is_vp_lag) {
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vp_lag)) {
            VPLAG_LOCK(unit);
            rv = bcm_td2_vp_lag_get(unit, tid, trunk_info, member_max,
                    member_array, member_count);
            VPLAG_UNLOCK(unit);
            return rv;
        } else
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
        {
            return BCM_E_PARAM;
        }
    }

    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        return (BCM_E_NOT_FOUND);
    }

    if ((member_max > 0) && (NULL == member_array)) {
        return BCM_E_PARAM;
    }

    if ((member_max > 0) && (NULL == member_count)) {
        return BCM_E_PARAM;
    }

    bcm_trunk_info_t_init(&local_trunk_info);

    TRUNK_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_trunk_get(unit, tid, &local_trunk_info,
            member_max, member_array, &local_member_count, t_info);
    TRUNK_UNLOCK(unit);

    if (NULL != trunk_info) {
        *trunk_info = local_trunk_info;
    }
    if (NULL != member_count) {
        *member_count = local_member_count;
    }

    return rv;
}

/*
 * Function:
 *	bcm_trunk_mcast_join
 * Purpose:
 *	Add the trunk group to existing MAC multicast entry.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - Trunk Id.
 *      vid - Vlan ID.
 *      mac - MAC address.
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *      Applications have to remove the MAC multicast entry and re-add in with
 *      new port bitmap to remove the trunk group from MAC multicast entry.
 */

int
bcm_esw_trunk_mcast_join(int unit, bcm_trunk_t tid, bcm_vlan_t vid, sal_mac_addr_t mac)
{
    trunk_private_t *t_info;
    int	rv;

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);
    if (t_info->tid == BCM_TRUNK_INVALID) {
        return (BCM_E_NOT_FOUND);
    }

    if (!t_info->in_use) {
        return (BCM_E_NONE);
    }

    TRUNK_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_trunk_mcast_join(unit, tid, vid, mac, t_info);
    TRUNK_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *	bcm_trunk_bitmap_expand
 * Purpose:
 *      Given a port bitmap, if any of the ports are in a trunk,
 *      add all of the ports of that trunk to the bitmap.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      pbmp_ptr - Input/output port bitmap
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_INTERNAL          Chip access failure.
 * Notes:
 *      Port bitmaps in the vtable and ptable must contain only one of
 *      the trunk ports when multiple ports are aggregated in a trunk.
 *      This is so DLF flooding, etc. goes out only one trunk port.
 */

int
bcm_esw_trunk_bitmap_expand(int unit, pbmp_t *pbmp_ptr)
{
    int		    rv = BCM_E_NONE;
    bcm_trunk_t     tid;
    pbmp_t          tports, tpbm;

    TRUNK_LOCK(unit);

#ifdef	BCM_XGS_SWITCH_SUPPORT
    if (SOC_IS_XGS_SWITCH(unit)) {
        pbmp_t    hg_pbmp, nonhg_pbmp;
        int       hg_trunk = FALSE;
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRX_SUPPORT) || \
    defined(BCM_BRADLEY_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        soc_reg_t hg_trunk_bmap;
#endif
        pbmp_t    in_pbm;
        int       trident_trunk_arch = FALSE;

        SOC_PBMP_ASSIGN(in_pbm, *pbmp_ptr);

#ifdef BCM_TRIUMPH2_SUPPORT
        hg_trunk_bmap = (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
                         SOC_IS_VALKYRIE2(unit)) ? HG_TRUNK_BITMAP_64r :
                                                   HG_TRUNK_BITMAPr;
#else

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
        hg_trunk_bmap = HG_TRUNK_BITMAPr;
#endif
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
        if (SOC_IS_GREYHOUND2(unit)) {
            hg_trunk_bmap = HG_TRUNK_BITMAP_LOr;
        }
#endif /* BCM_GREYHOUND2_SUPPORT */

        SOC_PBMP_ASSIGN(hg_pbmp, PBMP_ST_ALL(unit));
        SOC_PBMP_NEGATE(nonhg_pbmp, hg_pbmp);
        SOC_PBMP_AND(hg_pbmp, *pbmp_ptr);
        SOC_PBMP_AND(nonhg_pbmp, *pbmp_ptr);
        if (SOC_PBMP_NOT_NULL(hg_pbmp) && SOC_PBMP_NOT_NULL(nonhg_pbmp)) {
            TRUNK_UNLOCK(unit);
            return BCM_E_PARAM;
        } else if (SOC_PBMP_NOT_NULL(hg_pbmp)) {
            hg_trunk = TRUE;
        }

        if (hg_trunk && SOC_IS_FBX(unit)) {
            uint32     val;

            if ((SOC_IS_FB_FX_HX(unit) || SOC_IS_RAVEN(unit))  && 
                soc_feature(unit, soc_feature_hg_trunking)) {
                rv = READ_HIGIG_TRUNK_CONTROLr(unit, &val);
                if (rv >= 0) {
                    SOC_PBMP_CLEAR(tports);
                    SOC_PBMP_WORD_SET(tports, 0, soc_reg_field_get(unit,
                                                 HIGIG_TRUNK_CONTROLr, val,
                                                 HIGIG_TRUNK_BITMAP0f)<<24);
                    SOC_PBMP_ASSIGN(tpbm, *pbmp_ptr);
                    SOC_PBMP_AND(tpbm, tports);
                    if (SOC_PBMP_NOT_NULL(tpbm)) {
                        SOC_PBMP_OR(*pbmp_ptr, tports);
                    }

                    SOC_PBMP_CLEAR(tports);
                    SOC_PBMP_WORD_SET(tports, 0, soc_reg_field_get(unit,
                                                 HIGIG_TRUNK_CONTROLr, val,
                                                 HIGIG_TRUNK_BITMAP1f)<<24);
                    SOC_PBMP_ASSIGN(tpbm, *pbmp_ptr);
                    SOC_PBMP_AND(tpbm, tports);
                    if (SOC_PBMP_NOT_NULL(tpbm)) {
                        SOC_PBMP_OR(*pbmp_ptr, tports);
                    }
                }
            }
#if defined(BCM_TRIUMPH2_SUPPORT)
            else if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
                SOC_IS_VALKYRIE2(unit)) {
                uint64 val64;
                for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_hg; tid++) {
                    uint64    hg_trunk_bmap_f;
                    rv = soc_reg_get(unit, hg_trunk_bmap, REG_PORT_ANY, tid, &val64);
                    if (rv < 0) {
                        break;
                    }
                    SOC_PBMP_CLEAR(tports);
                    hg_trunk_bmap_f = soc_reg64_field_get(unit,
                                                          hg_trunk_bmap,
                                                          val64,
                                                          HIGIG_TRUNK_BITMAPf);
                    SOC_PBMP_WORD_SET(tports,
                                      0,
                                      COMPILER_64_LO(hg_trunk_bmap_f));
                    SOC_PBMP_WORD_SET(tports,
                                      1,
                                      COMPILER_64_HI(hg_trunk_bmap_f));
                    SOC_PBMP_ASSIGN(tpbm, *pbmp_ptr);
                    SOC_PBMP_AND(tpbm, tports);
                    if (SOC_PBMP_NOT_NULL(tpbm)) {
                        SOC_PBMP_OR(*pbmp_ptr, tports);
                    }
                }
            }
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
            else if (SOC_IS_GREYHOUND2(unit)) {
                uint64 val64;
                for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_hg; tid++) {
                    uint64 hg_trunk_bmap_f;
                    rv = soc_reg_get(unit, hg_trunk_bmap,
                                     REG_PORT_ANY, tid, &val64);
                    if (rv < 0) {
                        break;
                    }
                    SOC_PBMP_CLEAR(tports);
                    hg_trunk_bmap_f = soc_reg64_field_get(unit,
                                                          hg_trunk_bmap,
                                                          val64,
                                                          HIGIG_TRUNK_BITMAPf);
                    SOC_PBMP_WORD_SET(tports,
                                      0,
                                      COMPILER_64_LO(hg_trunk_bmap_f));
                    SOC_PBMP_WORD_SET(tports,
                                      1,
                                      COMPILER_64_HI(hg_trunk_bmap_f));

                    /* The maximum port number for Greyhound2 is 66 */
                    rv = soc_reg_get(unit, HG_TRUNK_BITMAP_HIr,
                                     REG_PORT_ANY, tid, &val64);
                    if (rv < 0) {
                        break;
                    }
                    hg_trunk_bmap_f = soc_reg64_field_get(unit,
                                                          HG_TRUNK_BITMAP_HIr,
                                                          val64,
                                                          HIGIG_TRUNK_BITMAPf);
                    SOC_PBMP_WORD_SET(tports,
                                      2,
                                      COMPILER_64_LO(hg_trunk_bmap_f));

                    SOC_PBMP_ASSIGN(tpbm, *pbmp_ptr);
                    SOC_PBMP_AND(tpbm, tports);
                    if (SOC_PBMP_NOT_NULL(tpbm)) {
                        SOC_PBMP_OR(*pbmp_ptr, tports);
                    }
                }
            }
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
            else if (SOC_REG_IS_VALID(unit, hg_trunk_bmap)) {
                uint64 val64;
                for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_hg; tid++) {
                    uint64    hg_trunk_bmap_f;
                    rv = soc_reg_get(unit, hg_trunk_bmap, REG_PORT_ANY, tid, &val64);
                    if (rv < 0) {
                        break;
                    }
                    SOC_PBMP_CLEAR(tports);
                    hg_trunk_bmap_f = soc_reg64_field_get(unit,
                                                          hg_trunk_bmap,    
                                                          val64,
                                                          HIGIG_TRUNK_BITMAPf);
                    SOC_PBMP_WORD_SET(tports,
                                      0,
                                      COMPILER_64_LO(hg_trunk_bmap_f));
                    soc_xgs3_higig_bitmap_to_bitmap(unit, tports, &tports);
                    SOC_PBMP_ASSIGN(tpbm, *pbmp_ptr);
                    SOC_PBMP_AND(tpbm, tports);
                    if (SOC_PBMP_NOT_NULL(tpbm)) {
                        SOC_PBMP_OR(*pbmp_ptr, tports);
                    }
                }
            }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
            else if (soc_mem_is_valid(unit, HG_TRUNK_BITMAPm)) {
                hg_trunk_bitmap_entry_t hg_trunk_bitmap_entry;

                for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_hg; tid++) {
                    rv = READ_HG_TRUNK_BITMAPm(unit, MEM_BLOCK_ANY, tid, &hg_trunk_bitmap_entry);
                    if (SOC_FAILURE(rv)) {
                        TRUNK_UNLOCK(unit);
                        return (rv);
                    }
                    SOC_PBMP_CLEAR(tports);
                    soc_mem_pbmp_field_get(unit, HG_TRUNK_BITMAPm, &hg_trunk_bitmap_entry,
                            HIGIG_TRUNK_BITMAPf, &tports);

                    SOC_PBMP_ASSIGN(tpbm, in_pbm);
                    SOC_PBMP_AND(tpbm, tports);
                    if (SOC_PBMP_NOT_NULL(tpbm)) {
                        SOC_PBMP_OR(*pbmp_ptr, tports);
                    }
                }
            }
#endif /* BCM_TRIDENT_SUPPORT */
        }
#if defined(BCM_TRIDENT_SUPPORT)
        if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
            trident_trunk_arch = TRUE;
        }
#endif /* BCM_TRIDENT_SUPPORT */
        if (!(hg_trunk && SOC_IS_FBX(unit)) || trident_trunk_arch) {
            trunk_bitmap_entry_t trunk_bitmap_entry;

            for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
                rv = READ_TRUNK_BITMAPm(unit, MEM_BLOCK_ANY, tid,
                                &trunk_bitmap_entry);
                if (rv < 0) {
                    break;
                }

                SOC_PBMP_CLEAR(tports);

                soc_mem_pbmp_field_get(unit, TRUNK_BITMAPm, &trunk_bitmap_entry,
                                           TRUNK_BITMAPf, &tports);

                SOC_PBMP_ASSIGN(tpbm, in_pbm);
                SOC_PBMP_AND(tpbm, tports);
                if (SOC_PBMP_NOT_NULL(tpbm)) {
                    SOC_PBMP_OR(*pbmp_ptr, tports);
                }
            }
        }
    }
#endif	/* BCM_XGS_SWITCH_SUPPORT */

#ifdef	BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
        uint32     val;
        bcm_port_t    port;

        for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_hg; tid++) {
             PBMP_HG_ITER(unit, port) {
                 rv = READ_ING_HGTRUNKr(unit, port, tid, &val);
                 break;
             }
             if (rv < 0) {
                 break;
             }
            SOC_PBMP_CLEAR(tports);
    /*    coverity[uninit_use_in_call : FALSE]    */
            SOC_PBMP_WORD_SET(tports, 0, soc_reg_field_get(unit,
                              ING_HGTRUNKr, val, BMAPf));
            SOC_PBMP_ASSIGN(tpbm, *pbmp_ptr);
            SOC_PBMP_AND(tpbm, tports);
            if (SOC_PBMP_NOT_NULL(tpbm)) {
                SOC_PBMP_OR(*pbmp_ptr, tports);
            }
        }
    }
#endif	/* BCM_XGS_FABRIC_SUPPORT */

    TRUNK_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *	bcm_trunk_egress_set
 * Description:
 *	Set switching only to indicated ports from given trunk.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - Trunk Id.  Negative trunk id means set all trunks.
 *	pbmp - bitmap of ports to allow egress.
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_esw_trunk_egress_set(int unit, bcm_trunk_t tid, bcm_pbmp_t pbmp)
{
    trunk_egr_mask_entry_t    tem_entry;
    bcm_trunk_t               tid_min, tid_max;

    TRUNK_INIT(unit);
    if (soc_feature(unit,soc_feature_trunk_egress)) {
        if (tid >= TRUNK_CNTL(unit).ngroups_fp) {
            return BCM_E_BADID;
        }

        if (!SOC_IS_XGS_SWITCH(unit)) {
            if (BCM_PBMP_EQ(pbmp, PBMP_ALL(unit))) {
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
        }

        sal_memset(&tem_entry, 0, sizeof(trunk_egr_mask_entry_t));

        BCM_PBMP_NEGATE(pbmp, pbmp);
        BCM_PBMP_AND(pbmp, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(pbmp, PBMP_LB(unit));
        soc_mem_pbmp_field_set(unit, TRUNK_EGR_MASKm, &tem_entry,
                               TRUNK_EGRESS_MASKf, &pbmp);

        if (tid < 0) {
            tid_min = 0;
            tid_max = TRUNK_CNTL(unit).ngroups_fp - 1;
        } else if (TRUNK_INFO(unit, tid).tid == BCM_TRUNK_INVALID) {
            return BCM_E_NOT_FOUND;
        } else {
            tid_min = tid_max = tid;
        }

        for (tid = tid_min; tid <= tid_max; tid++) {
            if (TRUNK_INFO(unit, tid).tid == BCM_TRUNK_INVALID) {
                continue;
            }
            BCM_IF_ERROR_RETURN
                (WRITE_TRUNK_EGR_MASKm(unit, MEM_BLOCK_ALL, tid, &tem_entry));

        }
    } else {
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_trunk_egress_get
 * Description:
 *	Retrieve bitmap of ports for which switching is enabled for trunk.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - Trunk Id.  Negative trunk id means choose any trunk.
 *	pbmp - (OUT) bitmap of ports where egress allowed.
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_esw_trunk_egress_get(int unit, bcm_trunk_t tid, bcm_pbmp_t *pbmp)
{
    trunk_egr_mask_entry_t    tem_entry;
    bcm_pbmp_t                tem_pbmp;

    TRUNK_INIT(unit);
    if (soc_feature(unit,soc_feature_trunk_egress)) {
        if (tid >= TRUNK_CNTL(unit).ngroups_fp) {
            return BCM_E_BADID;
        }

        if (!SOC_IS_XGS_SWITCH(unit)) {
            BCM_PBMP_ASSIGN(*pbmp, PBMP_ALL(unit));
            return BCM_E_NONE;
        }

        if (tid < 0) {
    	tid = 0;
        }

        if (TRUNK_INFO(unit, tid).tid == BCM_TRUNK_INVALID) {
            return BCM_E_NOT_FOUND;
        }

        BCM_PBMP_CLEAR(tem_pbmp);

        BCM_IF_ERROR_RETURN
            (READ_TRUNK_EGR_MASKm(unit, MEM_BLOCK_ANY, tid, &tem_entry));

        soc_mem_pbmp_field_get(unit, TRUNK_EGR_MASKm, &tem_entry,
                               TRUNK_EGRESS_MASKf, &tem_pbmp);

        BCM_PBMP_ASSIGN(*pbmp, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(*pbmp, PBMP_LB(unit));
        BCM_PBMP_XOR(*pbmp, tem_pbmp);
    } else{
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
#define _TRUNK_OVERRIDE_CHECK(_u, _p, _tid) \
        TRUNK_INIT(_u); \
        if (!SOC_IS_HERCULES15(_u) && (!SOC_IS_FBX(_u) || \
        !soc_feature(_u, soc_feature_hg_trunk_override)))  \
           { return (BCM_E_UNAVAIL); } \
        if (!TRUNK_FABRIC_TID(_u, _tid)) \
           { return (BCM_E_PARAM); } \
        if ((_p >= 0) && !IS_PORT(_u, _p)) \
           { return (BCM_E_PARAM); }
#ifdef BCM_HERCULES15_SUPPORT
#define _UCAST_RANGE_CHECK(_u, _id) \
        if (SOC_IS_HERCULES15(_u)) { \
            if ((_id < soc_mem_index_min(_u, MEM_UCm) || \
                _id > soc_mem_index_max(_u, MEM_UCm))) \
            { return (BCM_E_PARAM); } \
         } else if (soc_mem_is_valid(_u, MODPORT_MAPm)) { \
            if ((_id < soc_mem_index_min(_u, MODPORT_MAPm) || \
                _id > soc_mem_index_max(_u, MODPORT_MAPm))) \
             { return (BCM_E_PARAM); } \
         } else if (soc_mem_is_valid(_u, MODPORT_MAP_SWm)) { \
            if ((_id < soc_mem_index_min(_u, MODPORT_MAP_SWm) || \
                _id > soc_mem_index_max(_u, MODPORT_MAP_SWm))) \
             { return (BCM_E_PARAM); }}
#define _MCAST_RANGE_CHECK(_u, _id) \
        if (SOC_IS_HERCULES15(_u)) { \
            if ((_id < soc_mem_index_min(_u, MEM_MCm) || \
                _id > soc_mem_index_max(_u, MEM_MCm))) \
            { return (BCM_E_PARAM); } \
        } else { \
            if ((_id < soc_mem_index_min(_u, L2MCm) || \
                _id > soc_mem_index_max(_u, L2MCm))) \
             { return (BCM_E_PARAM); }}
#define _IPMC_RANGE_CHECK(_u, _id) \
        if (SOC_IS_HERCULES15(_u)) { \
            if ((_id < soc_mem_index_min(_u, MEM_IPMCm) || \
                _id > soc_mem_index_max(_u, MEM_IPMCm))) \
            { return (BCM_E_PARAM); } \
        } else { \
	    if ((_id < soc_mem_index_min(_u, L3_IPMCm) || \
                _id > soc_mem_index_max(_u, L3_IPMCm))) \
            { return (BCM_E_PARAM); }}
#define _VLAN_RANGE_CHECK(_u, _id) \
        if (SOC_IS_HERCULES15(_u)) { \
            if ((_id < soc_mem_index_min(_u, MEM_VIDm) || \
                _id > soc_mem_index_max(_u, MEM_VIDm))) \
            { return (BCM_E_PARAM); } \
        } else { \
            if ((_id < soc_mem_index_min(_u, VLAN_TABm) || \
                _id > soc_mem_index_max(_u, VLAN_TABm))) \
             { return (BCM_E_PARAM); }}
#else  /* BCM_HERCULES15_SUPPORT */
#define _UCAST_RANGE_CHECK(_u, _id) \
         if (soc_mem_is_valid(_u, MODPORT_MAPm)) { \
            if ((_id < soc_mem_index_min(_u, MODPORT_MAPm) || \
                _id > soc_mem_index_max(_u, MODPORT_MAPm))) \
             { return (BCM_E_PARAM); } \
         } else if (soc_mem_is_valid(_u, MODPORT_MAP_SWm)) { \
            if ((_id < soc_mem_index_min(_u, MODPORT_MAP_SWm) || \
                _id > soc_mem_index_max(_u, MODPORT_MAP_SWm))) \
             { return (BCM_E_PARAM); }}
#define _MCAST_RANGE_CHECK(_u, _id) \
        { \
            if ((_id < soc_mem_index_min(_u, L2MCm) || \
                _id > soc_mem_index_max(_u, L2MCm))) \
             { return (BCM_E_PARAM); }}
#define _IPMC_RANGE_CHECK(_u, _id) \
        { \
	    if ((_id < soc_mem_index_min(_u, L3_IPMCm) || \
                _id > soc_mem_index_max(_u, L3_IPMCm))) \
            { return (BCM_E_PARAM); }}
#define _VLAN_RANGE_CHECK(_u, _id) \
        { \
            if ((_id < soc_mem_index_min(_u, VLAN_TABm) || \
                _id > soc_mem_index_max(_u, VLAN_TABm))) \
             { return (BCM_E_PARAM); }}
#endif  /* BCM_HERCULES15_SUPPORT */
#endif  /* BCM_HERCULES15_SUPPORT  || BCM_FIREBOLT_SUPPORT */

/*
 * Function:
 *      bcm_trunk_override_ucast_set
 * Description:
 *      Set the trunk override over UC.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number, -1 to all ports.
 *      tid    - Trunk id.
 *      modid  - Module id.
 *      enable - TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_ucast_set(int unit, bcm_port_t port,
                             bcm_trunk_t tid, int modid, int enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    if (port >= 0) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_trunk_t     tgid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid)){
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
    }
    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _UCAST_RANGE_CHECK(unit, modid);

#ifdef  BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_uc_entry_t      uc;
        soc_field_t tf[] = {TRUNK0_OVER_UCf,TRUNK1_OVER_UCf,
                            TRUNK2_OVER_UCf,TRUNK3_OVER_UCf};
        int bk=-1, blk;

        if (port >= 0) {
            bk = SOC_PORT_BLOCK(unit, port);
        }

        soc_mem_lock(unit, MEM_UCm);
        SOC_MEM_BLOCK_ITER(unit, MEM_UCm, blk) {
            if ((port >= 0) && (bk != blk)) {
                continue;
            }
            rv = READ_MEM_UCm(unit, blk, modid, &uc);
            if (rv >= 0) {
                soc_MEM_UCm_field32_set(unit, &uc,
                                        tf[tid], enable ? 1: 0);
                rv = WRITE_MEM_UCm(unit, blk, modid, &uc);
            }
        }
        soc_mem_unlock(unit, MEM_UCm);
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT

    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
        (soc_feature(unit,soc_feature_hg_trunk_override)) ) { 
        modport_map_entry_t uc;
        uint32 val, hgo_bit;
        int i, min, max, idx, modid_count;
        soc_mem_t modport_map_mem;

        modid_count = SOC_MODID_MAX(unit) + 1;
        if (soc_mem_is_valid(unit, MODPORT_MAP_SWm)) {
            modport_map_mem = MODPORT_MAP_SWm;
        } else { 
            modport_map_mem = MODPORT_MAPm;
        }
        min = soc_mem_index_min(unit, modport_map_mem) / modid_count;
        max = soc_mem_index_max(unit, modport_map_mem) / modid_count;
        if (SOC_IS_HBX(unit)) {
            if (port >= min && port <= max) {
                min = max = port;
            }
        }

#ifdef BCM_TRIUMPH_SUPPORT
        if (soc_feature(unit, soc_feature_modport_map_profile)) {
            bcm_port_t ing_port;
            bcm_trunk_t hgtid;
            bcm_pbmp_t all_pbmp;

            BCM_PBMP_CLEAR(all_pbmp);
            BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));
#if defined(BCM_KATANA2_SUPPORT)
            if (soc_feature(unit, soc_feature_linkphy_coe) ||
                soc_feature(unit, soc_feature_subtag_coe)) {
                _bcm_kt2_subport_pbmp_update(unit, &all_pbmp);
            }
            if (SOC_IS_KATANA2(unit) &&
                soc_feature(unit, soc_feature_flex_port)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_kt2_flexio_pbmp_update(unit,
                    &all_pbmp));
            }
#endif

            hgtid = tid - TRUNK_CNTL(unit).ngroups_fp;
            if (port == BCM_GPORT_INVALID) {
                PBMP_ITER(all_pbmp, ing_port) {
#ifdef BCM_TRIDENT_SUPPORT
                    if (soc_feature(unit,
                                soc_feature_modport_map_dest_is_port_or_trunk)) {
                        BCM_IF_ERROR_RETURN
                            (bcm_td_stk_trunk_override_ucast_set(unit,
                                ing_port, hgtid, modid, enable));
                    } else
#endif /* BCM_TRIDENT_SUPPORT */
                    {
                        BCM_IF_ERROR_RETURN
                            (_bcm_esw_tr_trunk_override_ucast_set(unit,
                                ing_port, hgtid, modid, enable));
                    }
                }
            } else {
#ifdef BCM_TRIDENT_SUPPORT
                if (soc_feature(unit,
                            soc_feature_modport_map_dest_is_port_or_trunk)) {
                    BCM_IF_ERROR_RETURN
                        (bcm_td_stk_trunk_override_ucast_set(unit,
                            port, hgtid, modid, enable));
                } else
#endif /* BCM_TRIDENT_SUPPORT */
                {
                    BCM_IF_ERROR_RETURN
                        (_bcm_esw_tr_trunk_override_ucast_set(unit,
                            port, hgtid, modid, enable));
                }
            }
        } else 
#endif /* BCM_TRIUMPH_SUPPORT */
        {
            soc_mem_lock(unit, MODPORT_MAPm);
            for (i = min; i <= max; i++) {
                idx = i * modid_count + modid;
                rv = READ_MODPORT_MAPm(unit, MEM_BLOCK_ANY, idx, &uc);
                if (rv >= 0) {
                    hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
                    val = soc_MODPORT_MAPm_field32_get(unit, &uc,
                            HIGIG_TRUNK_OVERRIDEf);
                    if (enable) {
                        val |= hgo_bit;
                    } else {
                        val &= ~hgo_bit;
                    }
                    soc_MODPORT_MAPm_field32_set(unit, &uc,
                            HIGIG_TRUNK_OVERRIDEf, val);
                    rv = WRITE_MODPORT_MAPm(unit, MEM_BLOCK_ALL, idx, &uc);
                }
            }
            soc_mem_unlock(unit, MODPORT_MAPm);
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT  || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_override_ucast_get
 * Description:
 *      Get the trunk override over UC.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number.
 *      tid    - Trunk id.
 *      modid  - Module id.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_ucast_get(int unit, bcm_port_t port,
                             bcm_trunk_t tid, int modid, int *enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    if (BCM_GPORT_IS_SET(port)) {
        bcm_trunk_t     tgid;
        int             id;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != id) || (-1 != tgid)){
            return BCM_E_PARAM;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
    }
    
    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _UCAST_RANGE_CHECK(unit, modid);

#ifdef  BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_uc_entry_t      uc;
        soc_field_t tf[] = {TRUNK0_OVER_UCf,TRUNK1_OVER_UCf,
                            TRUNK2_OVER_UCf,TRUNK3_OVER_UCf};
        int blk;

        *enable = 0;

        blk = SOC_PORT_BLOCK(unit, port);
        rv = READ_MEM_UCm(unit, blk, modid, &uc);
        if (rv >= 0) {
            *enable = soc_MEM_UCm_field32_get(unit, &uc, tf[tid]);
        }
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT

    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
        (soc_feature(unit,soc_feature_hg_trunk_override)) ) { 
        modport_map_entry_t uc;
        uint32 val, hgo_bit;
        int idx;

        if (SOC_IS_FB_FX_HX(unit) || SOC_IS_TR_VL(unit)) {
            idx = modid;
        } else {
            idx = (port * (SOC_MODID_MAX(unit) + 1)) + modid;
        }
#ifdef BCM_TRIUMPH_SUPPORT
        if (soc_feature(unit, soc_feature_modport_map_profile)) {
            bcm_trunk_t hgtid;
            hgtid = tid - TRUNK_CNTL(unit).ngroups_fp;
#ifdef BCM_TRIDENT_SUPPORT
            if (soc_feature(unit, soc_feature_modport_map_dest_is_port_or_trunk)) {
                rv = bcm_td_stk_trunk_override_ucast_get(unit, port, hgtid,
                        modid, enable);
            } else
#endif /* BCM_TRIDENT_SUPPORT */
            {
                rv = _bcm_esw_tr_trunk_override_ucast_get(unit, port, hgtid,
                        modid, enable); 
            }
        } else 
#endif /* BCM_TRIUMPH_SUPPORT */
        {
            *enable = 0;

            rv = READ_MODPORT_MAPm(unit, MEM_BLOCK_ANY, idx, &uc);
            if (rv >= 0) {
                val = soc_MODPORT_MAPm_field32_get(unit, &uc,
                                                   HIGIG_TRUNK_OVERRIDEf);
                hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
                *enable = (val & hgo_bit) ? 1 : 0;
            }
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_override_mcast_set
 * Description:
 *      Set the trunk override over MC.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number, -1 to all ports.
 *      tid    - Trunk id.
 *      idx    - MC index carried in HiGig header.
 *      enable - TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_mcast_set(int unit, bcm_port_t port,
                                 bcm_trunk_t tid, int idx, int enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    /* Translate multicast group ID's */
    if (_BCM_MULTICAST_IS_L2(idx)) {
        idx = _BCM_MULTICAST_ID_GET(idx);
    } else if (_BCM_MULTICAST_IS_SET(idx)) {
        /* This is a non-L2 multicast group.
         * Redirect to the IPMC version below */
        idx = _BCM_MULTICAST_ID_GET(idx);
        return bcm_esw_trunk_override_ipmc_set(unit, port, tid,
                                               idx, enable);
    }

    if (port >= 0) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_module_t    modid;
            bcm_trunk_t     tgid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid)){
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
    }

    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _MCAST_RANGE_CHECK(unit, idx);

#ifdef BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_mc_entry_t      mc;
        soc_field_t tf[] = {TRUNK0_OVER_MCf,TRUNK1_OVER_MCf,
                            TRUNK2_OVER_MCf,TRUNK3_OVER_MCf};
        int  bk=-1, blk;

        if (port >= 0) {
            bk = SOC_PORT_BLOCK(unit, port);
        }

        soc_mem_lock(unit, MEM_MCm);
        SOC_MEM_BLOCK_ITER(unit, MEM_MCm, blk) {
            if ((port >= 0) && (bk != blk)) {
                continue;
            }
            rv = READ_MEM_MCm(unit, blk, idx, &mc);
            if (rv >= 0) {
                soc_MEM_MCm_field32_set(unit, &mc,
                                        tf[tid], enable ? 1: 0);
                rv = WRITE_MEM_MCm(unit, blk, idx, &mc);
            }
        }
        soc_mem_unlock(unit, MEM_MCm);
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT


    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
        (soc_feature(unit,soc_feature_hg_trunk_override)) ) { 

#ifdef BCM_TRIDENT_SUPPORT
        if (soc_feature(unit, soc_feature_hg_trunk_override_profile)) {
            bcm_trunk_t hgtid;

            hgtid = tid - TRUNK_CNTL(unit).ngroups_fp;
            rv = _bcm_trident_trunk_override_mcast_set(unit, hgtid, idx, enable); 
        } else 
#endif /* BCM_TRIDENT_SUPPORT */
        {
            l2mc_entry_t mc;
            uint32 val, hgo_bit;

            soc_mem_lock(unit, L2MCm);
            rv = READ_L2MCm(unit, MEM_BLOCK_ANY, idx, &mc);
            if (rv >= 0) {
                val = soc_L2MCm_field32_get(unit, &mc,
                        HIGIG_TRUNK_OVERRIDEf);
                hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
                if (enable) {
                    val |= hgo_bit;
                } else {
                    val &= ~hgo_bit;
                }
                soc_L2MCm_field32_set(unit, &mc,
                        HIGIG_TRUNK_OVERRIDEf, val);
                rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, idx, &mc);
            }
            soc_mem_unlock(unit, L2MCm);
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_override_mcast_get
 * Description:
 *      Get the trunk override over MC.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number.
 *      tid    - Trunk id.
 *      idx    - MC index carried in HiGig header.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_mcast_get(int unit, bcm_port_t port,
                                 bcm_trunk_t tid, int idx, int *enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    /* Translate multicast group ID's */
    if (_BCM_MULTICAST_IS_L2(idx)) {
        idx = _BCM_MULTICAST_ID_GET(idx);
    } else if (_BCM_MULTICAST_IS_SET(idx)) {
        /* This is a non-L2 multicast group.
         * Redirect to the IPMC version below */
        idx = _BCM_MULTICAST_ID_GET(idx);
        return bcm_esw_trunk_override_ipmc_get(unit, port, tid,
                                               idx, enable);
    }

    if (BCM_GPORT_IS_SET(port)) {
        bcm_module_t    modid;
        bcm_trunk_t     tgid;
        int             id;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != id) || (-1 != tgid)){
            return BCM_E_PARAM;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
    }

    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _MCAST_RANGE_CHECK(unit, idx);

#ifdef BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_mc_entry_t      mc;
        soc_field_t tf[] = {TRUNK0_OVER_MCf,TRUNK1_OVER_MCf,
                            TRUNK2_OVER_MCf,TRUNK3_OVER_MCf};
        int blk;

        *enable = 0;

        blk = SOC_PORT_BLOCK(unit, port);
        rv = READ_MEM_MCm(unit, blk, idx, &mc);
        if (rv >= 0) {
            *enable = soc_MEM_MCm_field32_get(unit, &mc, tf[tid]);
        }
    }
#endif /* SOC_IS_HERCULES15 */

#ifdef BCM_FIREBOLT_SUPPORT

    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
            (soc_feature(unit,soc_feature_hg_trunk_override))) { 

#ifdef BCM_TRIDENT_SUPPORT
        if (soc_feature(unit, soc_feature_hg_trunk_override_profile)) {
            int hgtid;

            hgtid = tid - TRUNK_CNTL(unit).ngroups_fp;
            rv = _bcm_trident_trunk_override_mcast_get(unit, hgtid, idx, enable); 
        } else
#endif /* BCM_TRIDENT_SUPPORT */
        {
            l2mc_entry_t mc;
            uint32 val, hgo_bit;

            *enable = 0;

            rv = READ_L2MCm(unit, MEM_BLOCK_ANY, idx, &mc);
            if (rv >= 0) {
                val = soc_L2MCm_field32_get(unit, &mc,
                        HIGIG_TRUNK_OVERRIDEf);
                hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
                *enable = (val & hgo_bit) ? 1 : 0;
            }
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT  || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_override_ipmc_set
 * Description:
 *      Set the trunk override over IPMC.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number, -1 to all ports.
 *      tid    - Trunk id.
 *      idx    - IPMC index carried in HiGig header.
 *      enable - TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_ipmc_set(int unit, bcm_port_t port,
                                bcm_trunk_t tid, int idx, int enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    /* Translate multicast group ID's */
    if (_BCM_MULTICAST_IS_L2(idx)) {
        /* This is an L2 multicast group.
         * Redirect to the mcast version above */
        idx = _BCM_MULTICAST_ID_GET(idx);
        return bcm_esw_trunk_override_mcast_set(unit, port, tid,
                                                idx, enable);
    } else if (_BCM_MULTICAST_IS_SET(idx)) {
        idx = _BCM_MULTICAST_ID_GET(idx);
    }

    if (!soc_feature(unit, soc_feature_ip_mcast)) {
        if (!SOC_IS_TITAN(unit)) {
            return BCM_E_UNAVAIL;
        }
    }

    if (port >= 0) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_module_t    modid;
            bcm_trunk_t     tgid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid)){
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
    }

    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _IPMC_RANGE_CHECK(unit, idx);

#ifdef  BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_ipmc_entry_t      ipmc;
        soc_field_t tf[] = {TRUNK0_OVER_IPMCf,TRUNK1_OVER_IPMCf,
                            TRUNK2_OVER_IPMCf,TRUNK3_OVER_IPMCf};
        int bk=-1, blk;

        if (port >= 0) {
            bk = SOC_PORT_BLOCK(unit, port);
        }

        soc_mem_lock(unit, MEM_IPMCm);
        SOC_MEM_BLOCK_ITER(unit, MEM_IPMCm, blk) {
            if ((port >= 0) && (bk != blk)) {
                continue;
            }
            rv = READ_MEM_IPMCm(unit, blk, idx, &ipmc);
            if (rv >= 0) {
                soc_MEM_IPMCm_field32_set(unit, &ipmc,
                                          tf[tid], enable ? 1: 0);
                rv = WRITE_MEM_IPMCm(unit, blk, idx, &ipmc);
            }
        }
        soc_mem_unlock(unit, MEM_IPMCm);
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
        (soc_feature(unit,soc_feature_hg_trunk_override)) ) { 

#ifdef BCM_TRIDENT_SUPPORT 
        if (soc_feature(unit, soc_feature_hg_trunk_override_profile)) {
            bcm_trunk_t hgtid;

            hgtid = tid - TRUNK_CNTL(unit).ngroups_fp;
            rv = _bcm_trident_trunk_override_ipmc_set(unit, hgtid, idx, enable); 
        } else 
#endif /* BCM_TRIDENT_SUPPORT */
        {
            ipmc_entry_t ipmc;
            uint32 val, hgo_bit;

            soc_mem_lock(unit, L3_IPMCm);
            rv = READ_L3_IPMCm(unit, MEM_BLOCK_ANY, idx, &ipmc);
            if (rv >= 0) {
                val = soc_L3_IPMCm_field32_get(unit, &ipmc,
                        HIGIG_TRUNK_OVERRIDEf);
                hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
                if (enable) {
                    val |= hgo_bit;
                } else {
                    val &= ~hgo_bit;
                }
                soc_L3_IPMCm_field32_set(unit, &ipmc,
                        HIGIG_TRUNK_OVERRIDEf, val);
                rv = WRITE_L3_IPMCm(unit, MEM_BLOCK_ALL, idx, &ipmc);
            }
            soc_mem_unlock(unit, L3_IPMCm);

#if defined(BCM_BRADLEY_SUPPORT)
            if (SOC_IS_HBX(unit)) {
                l2mc_entry_t mc;
                uint32 val, hgo_bit;
                int	mc_base, mc_size, mc_index;

                SOC_IF_ERROR_RETURN
                    (soc_hbx_ipmc_size_get(unit, &mc_base, &mc_size));
                if (idx < 0 || idx > mc_size) {
                    return BCM_E_PARAM;
                }
                mc_index = idx + mc_base;

                soc_mem_lock(unit, L2MCm);
                rv = READ_L2MCm(unit, MEM_BLOCK_ANY, mc_index, &mc);
                if (rv >= 0) {
                    val = soc_L2MCm_field32_get(unit, &mc,
                            HIGIG_TRUNK_OVERRIDEf);
                    hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
                    if (enable) {
                        val |= hgo_bit;
                    } else {
                        val &= ~hgo_bit;
                    }
                    soc_L2MCm_field32_set(unit, &mc,
                            HIGIG_TRUNK_OVERRIDEf, val);
                    rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, mc_index, &mc);
                }
                soc_mem_unlock(unit, L2MCm);

                
            }
#endif /* BCM_BRADLEY_SUPPORT */
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_override_ipmc_get
 * Description:
 *      Get the trunk override over IPMC.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number.
 *      tid    - Trunk id.
 *      idx    - IPMC index carried in HiGig header.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_ipmc_get(int unit, bcm_port_t port,
                                bcm_trunk_t tid, int idx, int *enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    /* Translate multicast group ID's */
    if (_BCM_MULTICAST_IS_L2(idx)) {
        /* This is an L2 multicast group.
         * Redirect to the mcast version above */
        idx = _BCM_MULTICAST_ID_GET(idx);
        return bcm_esw_trunk_override_mcast_get(unit, port, tid,
                                                idx, enable);
    } else if (_BCM_MULTICAST_IS_SET(idx)) {
        idx = _BCM_MULTICAST_ID_GET(idx);
    }

    if (!soc_feature(unit, soc_feature_ip_mcast)) {
        if (!SOC_IS_TITAN(unit)) {
            return BCM_E_UNAVAIL;
        }
    }

    if (BCM_GPORT_IS_SET(port)) {
        bcm_module_t    modid;
        bcm_trunk_t     tgid;
        int             id;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != id) || (-1 != tgid)){
            return BCM_E_PARAM;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
    }

    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _IPMC_RANGE_CHECK(unit, idx);

#ifdef  BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_ipmc_entry_t      ipmc;
        soc_field_t tf[] = {TRUNK0_OVER_IPMCf,TRUNK1_OVER_IPMCf,
                            TRUNK2_OVER_IPMCf,TRUNK3_OVER_IPMCf};
        int blk;

        *enable = 0;

        blk = SOC_PORT_BLOCK(unit, port);
        rv = READ_MEM_IPMCm(unit, blk, idx, &ipmc);
        if (rv >= 0) {
            *enable = soc_MEM_IPMCm_field32_get(unit, &ipmc, tf[tid]);
        }
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT

    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
            (soc_feature(unit,soc_feature_hg_trunk_override)) ) { 

#ifdef BCM_TRIDENT_SUPPORT 
        if (soc_feature(unit, soc_feature_hg_trunk_override_profile)) {
            bcm_trunk_t hgtid;

            hgtid = tid - TRUNK_CNTL(unit).ngroups_fp;
            rv = _bcm_trident_trunk_override_ipmc_get(unit, hgtid, idx, enable); 
        } else 
#endif /* BCM_TRIDENT_SUPPORT */
        {
            ipmc_entry_t ipmc;
            uint32 val, hgo_bit;

            *enable = 0;

            rv = READ_L3_IPMCm(unit, MEM_BLOCK_ANY, idx, &ipmc);
            if (rv >= 0) {
                val = soc_L3_IPMCm_field32_get(unit, &ipmc,
                        HIGIG_TRUNK_OVERRIDEf);
                hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
                *enable = (val & hgo_bit) ? 1 : 0;
            }
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT  || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_override_vlan_set
 * Description:
 *      Set the trunk override over VLAN.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number, -1 to all ports.
 *      tid    - Trunk id.
 *      vid    - VLAN id.
 *      enable - TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_vlan_set(int unit, bcm_port_t port,
                            bcm_trunk_t tid, bcm_vlan_t vid, int enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    if (port >= 0) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_module_t    modid;
            bcm_trunk_t     tgid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid)){
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
    }

    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _VLAN_RANGE_CHECK(unit, vid);

#ifdef  BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_vid_entry_t      ve;
        soc_field_t tf[] = {TRUNK0_OVER_VIDf,TRUNK1_OVER_VIDf,
                            TRUNK2_OVER_VIDf,TRUNK3_OVER_VIDf};
        int blk, bk = -1;

        if (port >= 0) {
            bk = SOC_PORT_BLOCK(unit, port);
        }

        soc_mem_lock(unit, MEM_VIDm);
        SOC_MEM_BLOCK_ITER(unit, MEM_VIDm, blk) {
            if ((port >= 0) && (bk != blk)) {
                continue;
            }
            rv = READ_MEM_VIDm(unit, blk, vid, &ve);
            if (rv >= 0) {
                soc_MEM_VIDm_field32_set(unit, &ve,
                                         tf[tid], enable ? 1: 0);
                rv = WRITE_MEM_VIDm(unit, blk, vid, &ve);
            }
        }
        soc_mem_unlock(unit, MEM_VIDm);
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT

    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
            (soc_feature(unit,soc_feature_hg_trunk_override)) ) { 

#ifdef BCM_TRIDENT_SUPPORT 
        if (soc_feature(unit, soc_feature_hg_trunk_override_profile)) {
            bcm_trunk_t hgtid;

            hgtid = tid - TRUNK_CNTL(unit).ngroups_fp;
            rv = _bcm_trident_trunk_override_vlan_set(unit, hgtid, vid, enable); 
        } else 
#endif /* BCM_TRIDENT_SUPPORT */
        {
            vlan_tab_entry_t ve;
            uint32 val, hgo_bit;

            soc_mem_lock(unit, VLAN_TABm);
            rv = READ_VLAN_TABm(unit, MEM_BLOCK_ANY, vid, &ve);
            if (rv >= 0) {
                val = soc_VLAN_TABm_field32_get(unit, &ve,
                        HIGIG_TRUNK_OVERRIDEf);
                hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
                if (enable) {
                    val |= hgo_bit;
                } else {
                    val &= ~hgo_bit;
                }
                soc_VLAN_TABm_field32_set(unit, &ve,
                        HIGIG_TRUNK_OVERRIDEf, val);
                rv = WRITE_VLAN_TABm(unit, MEM_BLOCK_ALL, vid, &ve);
            }
            soc_mem_unlock(unit, VLAN_TABm);
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_override_vlan_get
 * Description:
 *      Get the trunk override over VLAN.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number.
 *      tid    - Trunk id.
 *      vid    - VLAN id.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_vlan_get(int unit, bcm_port_t port,
                            bcm_trunk_t tid, bcm_vlan_t vid, int *enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    if (BCM_GPORT_IS_SET(port)) {
        bcm_module_t    modid;
        bcm_trunk_t     tgid;
        int             id;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != id) || (-1 != tgid)){
            return BCM_E_PARAM;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
    }

    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _VLAN_RANGE_CHECK(unit, vid);

#ifdef  BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_vid_entry_t      ve;
        soc_field_t tf[] = {TRUNK0_OVER_VIDf,TRUNK1_OVER_VIDf,
                            TRUNK2_OVER_VIDf,TRUNK3_OVER_VIDf};
        int blk;

        *enable = 0;

        blk = SOC_PORT_BLOCK(unit, port);
        rv = READ_MEM_VIDm(unit, blk, vid, &ve);
        if (rv >= 0) {
            *enable = soc_MEM_VIDm_field32_get(unit, &ve, tf[tid]);
        }
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT

    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
            (soc_feature(unit,soc_feature_hg_trunk_override)) ) {         

#ifdef BCM_TRIDENT_SUPPORT 
        if (soc_feature(unit, soc_feature_hg_trunk_override_profile)) {
            bcm_trunk_t hgtid;

            hgtid = tid - TRUNK_CNTL(unit).ngroups_fp;
            rv = _bcm_trident_trunk_override_vlan_get(unit, hgtid, vid, enable); 
        } else 
#endif /* BCM_TRIDENT_SUPPORT */
        {
            vlan_tab_entry_t ve;
            uint32 val, hgo_bit;

            *enable = 0;

            rv = READ_VLAN_TABm(unit, MEM_BLOCK_ANY, vid, &ve);
            if (rv >= 0) {
                val = soc_VLAN_TABm_field32_get(unit, &ve,
                        HIGIG_TRUNK_OVERRIDEf);
                hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
                *enable = (val & hgo_bit) ? 1 : 0;
            }
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_pool_set
 * Description:
 *      Set trunk pool table that contains the egress port number
 *      indexed by the hash value.
 * Parameters:
 *      unit    - StrataSwitch PCI device unit number (driver internal).
 *      port    - Port number, -1 to all ports.
 *      tid     - Trunk id.
 *      size    - Trunk pool size.
 *      weights - Weights for each port, all 0 means weighted fair.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_pool_set(int unit, bcm_port_t port, bcm_trunk_t tid,
                   int size, const int weights[BCM_TRUNK_MAX_PORTCNT])
{
#ifdef  BCM_HERCULES15_SUPPORT
    uint32 val;
    int idx_min, idx_max, weighted, p, idx;
    mem_trunk_port_pool_entry_t trk_pool;
    trunk_private_t *t_info;
    bcm_trunk_member_t member_array[BCM_TRUNK_FABRIC_MAX_PORTCNT];
    int member_count;
    bcm_port_t tp[BCM_TRUNK_FABRIC_MAX_PORTCNT];
    soc_field_t pf[] = {PORT_NO_0f, PORT_NO_1f, PORT_NO_2f, PORT_NO_3f};
    int token[BCM_TRUNK_MAX_PORTCNT], index;
    int i, reset=0, blk, rv=BCM_E_NONE, lp=0;

    if (!SOC_IS_HERCULES15(unit)) {
        return BCM_E_UNAVAIL;
    }

    if (port >= 0) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_trunk_t     tgid;
            bcm_module_t    modid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid)){
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port) || !IS_HG_PORT(unit, port) ) {
                return BCM_E_PORT;
            }
        }
    }

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);
    t_info = &TRUNK_INFO(unit, tid);
    if (t_info->tid == BCM_TRUNK_INVALID) {
        return BCM_E_NOT_FOUND;
    }

    idx_min = soc_mem_index_min(unit, MEM_TRUNK_PORT_POOLm);
    idx_max = soc_mem_index_max(unit, MEM_TRUNK_PORT_POOLm);
    if ((size < idx_min) || (size > idx_max)) {
        return BCM_E_PARAM;
    }
    if (size == 0) {
        index = size = idx_max;
    } else {
        index = size - 1;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_trunk_get(unit, tid, NULL,
                BCM_TRUNK_FABRIC_MAX_PORTCNT, member_array, &member_count));
    if (member_count <= 0) {
        return BCM_E_NOT_FOUND;
    }

    for (i=0; i < member_count; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_local_get(unit, member_array[i].gport, &tp[i])); 
         lp |= weights[tp[i]-1];
         if ((lp & ~0x3f) != 0) {
             return BCM_E_PARAM;
         }
    }
    weighted = (lp) ? 1 : 0;

    soc_mem_lock(unit, MEM_TRUNK_PORT_POOLm);
    PBMP_PORT_ITER(unit, p) {
        if ((port >= 0) && (p != port)) {
            continue;
        }

        if ((rv = READ_ING_HGTRUNKr(unit, p, tid, &val)) >= 0) {
            soc_reg_field_set(unit, ING_HGTRUNKr, &val,
                              TRUNK_POOL_SIZEf, size);
            rv = WRITE_ING_HGTRUNKr(unit, p, tid, val);
        }
        if (rv < 0) {
            break;
        }

        if (weighted) {
            for (i = 0; i < member_count; i++) {
                 token[tp[i]-1] = weights[tp[i]-1];
            }
        }

        idx = index;
        blk = SOC_PORT_BLOCK(unit, p);

        while (idx >= 0) {
            if (weighted) {
                for (i = 0; i < member_count; i++) {
                    if (token[tp[i]-1]) {
                        reset = 0;
                        break;
                    }
                    reset = 1;
                }
                if (reset) {
                    for (i = 0; i < member_count; i++) {
                        token[tp[i]-1] = weights[tp[i]-1];
                    }
                }
            }
            for (i = 0; i < member_count; i++) {
                if (idx < 0) break;
                sal_memset(&trk_pool, 0, sizeof(trk_pool));
                if (weighted) {
                    if (token[tp[i]-1]) {
                        if ((rv = READ_MEM_TRUNK_PORT_POOLm(unit, blk, index-idx, &trk_pool)) >= 0) {
                            soc_MEM_TRUNK_PORT_POOLm_field32_set(unit, &trk_pool, pf[tid], tp[i]);
                            rv = WRITE_MEM_TRUNK_PORT_POOLm(unit, blk, index-idx,  &trk_pool);
                        }
                        token[tp[i]-1]--;
                        idx--;
                    } else {
                      continue;
                    }
                } else {
                    if ((rv = READ_MEM_TRUNK_PORT_POOLm(unit, blk, index-idx, &trk_pool)) >= 0) {
                        soc_MEM_TRUNK_PORT_POOLm_field32_set(unit, &trk_pool, pf[tid], tp[i]);
                        rv = WRITE_MEM_TRUNK_PORT_POOLm(unit, blk, index-idx,  &trk_pool);
                    }
                    idx--;
                }
            }
        }
    }
    soc_mem_unlock(unit, MEM_TRUNK_PORT_POOLm);

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_pool_get
 * Description:
 *      Get trunk pool table that contains the egress port number
 *      indexed by the hash value.
 * Parameters:
 *      unit    - StrataSwitch PCI device unit number (driver internal).
 *      port    - Port number.
 *      tid     - Trunk id.
 *      size    - (OUT) Trunk pool size.
 *      weights - (OUT) Weights (total count) for each port.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_pool_get(int unit, bcm_port_t port, bcm_trunk_t tid,
                   int *size, int weights[BCM_TRUNK_MAX_PORTCNT])
{
#ifdef  BCM_HERCULES15_SUPPORT
    uint32 val;
    mem_trunk_port_pool_entry_t trk_pool;
    trunk_private_t *t_info;
    bcm_trunk_member_t member_array[BCM_TRUNK_FABRIC_MAX_PORTCNT];
    int member_count;
    bcm_port_t tp[BCM_TRUNK_FABRIC_MAX_PORTCNT];
    soc_field_t pf[] = {PORT_NO_0f, PORT_NO_1f, PORT_NO_2f, PORT_NO_3f};
    int i,pool, idx, blk, pool_port, rv=BCM_E_NONE;

    if (!SOC_IS_HERCULES15(unit)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_SET(port)) {
        bcm_trunk_t     tgid;
        bcm_module_t    modid;
        int             id;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != id) || (-1 != tgid)){
            return BCM_E_PARAM;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port) || !IS_HG_PORT(unit, port) ) {
            return BCM_E_PORT;
        }
    }

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);
    t_info = &TRUNK_INFO(unit, tid);
    if (t_info->tid == BCM_TRUNK_INVALID) {
        return BCM_E_NOT_FOUND;
    }

    *size = 0;
    for (idx=0; idx < BCM_TRUNK_MAX_PORTCNT; idx++) {
         weights[idx] = 0;
    }

    SOC_IF_ERROR_RETURN(READ_ING_HGTRUNKr(unit, port, tid, &val));
    *size = soc_reg_field_get(unit, ING_HGTRUNKr, val, TRUNK_POOL_SIZEf);
    if (*size == 0) { /* HW meaning, we never program 0 above in _set */
        *size = 1;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_trunk_get(unit, tid, NULL,
                BCM_TRUNK_FABRIC_MAX_PORTCNT, member_array, &member_count)); 
    for (i=0; i < member_count; i++) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit,
                    member_array[i].gport, &tp[i]));
    }

    blk = SOC_PORT_BLOCK(unit, port);
    pool = *size;
    for (idx = 0; idx < pool; idx++) {
         if ((rv = READ_MEM_TRUNK_PORT_POOLm(unit, blk, idx, &trk_pool)) >= 0) {
              pool_port = soc_MEM_TRUNK_PORT_POOLm_field32_get(unit, &trk_pool, pf[tid]);
              for (i=0; i < member_count; i++) {
                   if (pool_port == tp[i]) {
                       weights[tp[i]-1]++;
                       break;
                   }
              }
         }
    }

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT */
}

int _bcm_nuc_tpbm_get(int unit,
                      int num_ports,
                      bcm_module_t *tm,
                      uint32 *nuc_tpbm)
{
    int i, mod = -1;
    uint32 mod_type;
    int all_equal = 1;
    uint32 xgs12_tpbm = 0;
    uint32 xgs3_tpbm = 0;
    uint32 unknown_tpbm = 0;

    *nuc_tpbm = 0x1;
    for (i = 0; i < num_ports; i++) {
        if (i == 0) {
            mod = tm[i];
        } else if (mod != tm[i]) {
            all_equal = 0;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_switch_module_type_get(unit, tm[i], &mod_type));
        switch(mod_type) {
            case BCM_SWITCH_MODULE_XGS1   :
            case BCM_SWITCH_MODULE_XGS2   : xgs12_tpbm |= (1 << i); break;
            case BCM_SWITCH_MODULE_XGS3   : xgs3_tpbm |= (1 << i); break;
            case BCM_SWITCH_MODULE_UNKNOWN:
            default                       : unknown_tpbm |= (1 << i); break;
        }
    }
    if (all_equal) {
        *nuc_tpbm = ((0x1 << num_ports) - 1);
    } else if (unknown_tpbm || xgs12_tpbm) {
        *nuc_tpbm = 0x1;
    } else if (xgs3_tpbm) {
        *nuc_tpbm = xgs3_tpbm;
    }

    return(BCM_E_NONE);
}

/*
 * Function:
 *      bcm_trunk_find
 * Description:
 *      Get trunk id that contains the given system port
 * Parameters:
 *      unit    - StrataSwitch PCI device unit number (driver internal)
 *      modid   - Module ID
 *      port    - Port number
 *      tid     - (OUT) Trunk id
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_esw_trunk_find(int unit, bcm_module_t modid, bcm_port_t port,
                   bcm_trunk_t *tid)
{
    bcm_module_t hw_mod;
    bcm_port_t   hw_port;
    int          tgid, id;
    int          isLocal;

    TRUNK_INIT(unit);

    if (tid == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &hw_mod, &hw_port, &tgid, &id));

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
            id = -1;
        } else
#endif
#if defined(BCM_KATANA2_SUPPORT)
        if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
            id = -1;
        }
#endif
        if (-1 != tgid) {
            return BCM_E_PORT;
        } else if (-1 != id) {
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
            if (soc_feature(unit, soc_feature_vp_lag)) {
                return bcm_td2_vp_lag_find(unit, port, tid);
            } else
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
            {
                return BCM_E_PORT;
            }
        }
    } else {
        PORT_DUALMODID_VALID(unit, port);
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                    modid, port,
                                    &hw_mod, &hw_port));

        if (!SOC_MODID_ADDRESSABLE(unit, hw_mod)) {
            return BCM_E_BADID;
        }

        if (!SOC_PORT_ADDRESSABLE(unit, hw_port)) {
            return BCM_E_PORT;
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, hw_mod, &isLocal));

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            return(_bcm_th3_trunk_get_port_property(unit, hw_port, tid));
        } else
#endif
        {
#ifdef BCM_TRIDENT_SUPPORT
        int rv = BCM_E_NONE;
        if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
            if (isLocal && (IS_ST_PORT(unit, hw_port)
                || (soc_feature(unit, soc_feature_subtag_coe) &&
                    IS_SUBTAG_PORT (unit, hw_port))
                || (soc_feature(unit, soc_feature_linkphy_coe) &&
                    IS_LP_PORT (unit, hw_port))
                || ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                    (soc_feature(unit, soc_feature_channelized_switching))) &&
                    IS_SUBTAG_PORT (unit, hw_port)))) {

                int my_modid = 0;
                /* If the passed in mod-id matches the local mod-id, look for
                   the trunk with only the port. Else, for CoE type of
                   applications look for both the mod-id and port-id */
                BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
                if (IS_ST_PORT(unit, hw_port) && (my_modid == hw_mod)) {
                    rv = _bcm_trident_trunk_fabric_find(unit, hw_port, tid);
                } else {
                    return (_bcm_trident_trunk_get_port_property(unit, hw_mod,
                                                              hw_port, tid));
                }

                if (BCM_SUCCESS(rv)) {
                    return rv;
                }
                
                return(_bcm_trident_trunk_get_port_property(unit, hw_mod, 
                                                               hw_port, tid));
            } else {
                return(_bcm_trident_trunk_get_port_property(unit, hw_mod, hw_port, tid));
            } 
        } else 
#endif /* BCM_TRIDENT_SUPPORT */
        {
            if (isLocal && IS_ST_PORT(unit, hw_port)) {
                return(_bcm_xgs3_trunk_fabric_find(unit, hw_port, tid));
            } else {
                return(_bcm_xgs3_trunk_get_port_property(unit, hw_mod, hw_port, tid)); 
            }
        }
        }
    } else
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    {
        /* Hercules 1.5 */

        int idx = 0;
        int i;
        bcm_trunk_member_t member_array[BCM_TRUNK_FABRIC_MAX_PORTCNT];
        int member_count;
        bcm_port_t tp[BCM_TRUNK_FABRIC_MAX_PORTCNT];

        for (idx = 0; idx < TRUNK_NUM_GROUPS(unit); idx++) {
             if (TRUNK_INFO(unit, idx).tid == BCM_TRUNK_INVALID) {
                continue;
             }
             
             BCM_IF_ERROR_RETURN(bcm_esw_trunk_get(unit, idx, NULL,
                BCM_TRUNK_FABRIC_MAX_PORTCNT, member_array, &member_count));
             if (member_count <= 0) {
                 continue;
             }

             for (i = 0; i < member_count; i++) {
                 BCM_IF_ERROR_RETURN
                     (bcm_esw_port_local_get(unit, member_array[i].gport, &tp[i])); 

                 if (isLocal && IS_ST_PORT(unit, hw_port)) {
                     if (TRUNK_FABRIC_TID(unit, idx)) {
                         if (tp[i] == hw_port) {
                             *tid = idx;
                             return BCM_E_NONE;
                         }
                     }
                 }
             }
        }

        return BCM_E_NOT_FOUND;
    }
}

/*
 * Function:
 *      bcm_esw_trunk_failover_set
 * Purpose:
 *      Assign the failover port list for a specific trunk port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tid - (IN) Trunk id.
 *      failport - (IN) Port in trunk for which to specify failover port list.
 *      psc - (IN) Port selection criteria for failover port list.
 *      flags - (IN) BCM_TRUNK_FLAG_FAILOVER_xxx.
 *      count - (IN) Number of ports in failover port list.
 *      fail_to_array - (IN) Failover port list.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_trunk_failover_set(int unit, bcm_trunk_t tid, bcm_gport_t failport, 
                           int psc, uint32 flags, int count, 
                           bcm_gport_t *fail_to_array)
{
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    trunk_private_t *t_info;
    int rv, id, fabric_trunk;
    bcm_port_t port, *ftp=NULL;
    bcm_module_t modid, *ftm=NULL;
    bcm_trunk_t tgid;

    TRUNK_INIT(unit);

    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        return (BCM_E_NOT_FOUND);
    }

    fabric_trunk = TRUNK_FABRIC_TID(unit, tid);

    if (fabric_trunk) {
        if (!soc_feature(unit, soc_feature_hg_trunk_failover)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(failport)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, failport, &port));
        } else {
            port = failport;
        }
        modid = -1;
    } else {
        if (!soc_feature(unit, soc_feature_port_lag_failover)) {
            return BCM_E_UNAVAIL;
        }
#if defined(BCM_TRIDENT_SUPPORT)
        if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
            if (BCM_GPORT_IS_SET(failport)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_local_get(unit, failport, &port));
            } else {
                port = failport;
            }

            if (IS_HG_PORT(unit, port)) {
                return BCM_E_UNAVAIL;
            }
            /* not supported for Cascaded Physical Ports*/
            if ((soc_feature(unit, soc_feature_linkphy_coe) &&
                IS_LP_PORT(unit, port)) ||
                (soc_feature(unit, soc_feature_subtag_coe) &&
                IS_SUBTAG_PORT(unit, port)) || 
                ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                 (soc_feature(unit, soc_feature_channelized_switching))) &&
                IS_SUBTAG_PORT (unit, port))) {
                return BCM_E_UNAVAIL;
            }
        }
#endif

        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, failport, &modid, &port, &tgid, &id));
        if ((-1 != tgid) || (-1 != id)) {
            return BCM_E_PARAM;
        }
    }

    if (psc <= 0) {
	psc = BCM_TRUNK_PSC_DEFAULT;
    }

    /* Failover options:
     * 1) Specify plan with a flag
     * 2) Specify plan with count > 0 and fail_to_array listing ports
     * 3) Disable failover with count == 0
     */
    if ((flags == 0) && ((count < 0) ||
                         ((count > 0) && (fail_to_array == NULL)))) {
        return BCM_E_PARAM;
    }

    if (count > 0) {
        if ((TRUNK_FP_TID(unit, tid) &&
             (count > TRUNK_CNTL(unit).nports_fp)) ||
            (TRUNK_FABRIC_TID(unit, tid) &&
             (count > TRUNK_CNTL(unit).nports_hg)) ) {
            return (BCM_E_PARAM);
        } else {
            ftp = sal_alloc(sizeof(bcm_port_t) * count, "failover ports");
            if (NULL == ftp) {
                return BCM_E_MEMORY;
            }
            ftm = sal_alloc(sizeof(bcm_module_t) * count, "failover mods");
            if (NULL == ftm) {
                sal_free(ftp);
                return BCM_E_MEMORY;
            }
            rv = _bcm_esw_trunk_gport_array_resolve(unit, fabric_trunk,
                                                    count, fail_to_array,
                                                    ftp, ftm);
            if (BCM_FAILURE(rv)) {
                sal_free(ftp);
                sal_free(ftm);
            }
        }
    }

    /* Offset of trunk type */
    tgid = tid - (fabric_trunk ? TRUNK_CNTL(unit).ngroups_fp : 0);

    TRUNK_LOCK(unit);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        rv = _bcm_th3_trunk_hwfailover_set(unit, tid,
                port, psc, flags, count, fail_to_array);
    } else 
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
        rv = _bcm_trident_trunk_hwfailover_set(unit, tgid, fabric_trunk,
                port, modid, psc, flags, count, ftp, ftm);
    } else 
#endif /* BCM_TRIDENT_SUPPORT */
    {
        rv = _bcm_xgs3_trunk_hwfailover_set(unit, tgid, fabric_trunk,
                port, modid, psc, flags, count, ftp, ftm);
    }
    if (NULL != ftp) {
        sal_free(ftp);
    }
    if (NULL != ftm) {
        sal_free(ftm);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    TRUNK_UNLOCK(unit);

    return rv;
#else /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
    return BCM_E_UNAVAIL; 
#endif
}

/*
 * Function:
 *      bcm_esw_trunk_failover_get
 * Purpose:
 *      Retrieve the failover port list for a specific trunk port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tid - (IN) Trunk id.
 *      failport - (IN) Port in trunk for which to retrieve failover port list.
 *      psc - (OUT) Port selection criteria for failover port list.
 *      flags - (OUT) BCM_TRUNK_FLAG_FAILOVER_xxx.
 *      array_size - (IN) Maximum number of ports in provided failover port list.
 *      fail_to_array - (OUT) Failover port list.
 *      array_count - (OUT) Number of ports in returned failover port list.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_trunk_failover_get(int unit, bcm_trunk_t tid, bcm_gport_t failport, 
                           int *psc, uint32 *flags, int array_size, 
                           bcm_gport_t *fail_to_array, int *array_count)
{
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    trunk_private_t *t_info;
    int rv, id, fabric_trunk;
    bcm_port_t port, *ftp=NULL;
    bcm_module_t modid, *ftm=NULL;
    bcm_trunk_t tgid;

    TRUNK_INIT(unit);

    /* Following check is for Non-Tomahawk3 */
    if (!SOC_IS_TOMAHAWK3(unit)) {
        if (!soc_feature(unit, soc_feature_hg_trunk_failover)) {
            return BCM_E_UNAVAIL;
        }
    }

    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        return (BCM_E_NOT_FOUND);
    }

    fabric_trunk = TRUNK_FABRIC_TID(unit, tid);

    if (fabric_trunk) {
        if (!soc_feature(unit, soc_feature_hg_trunk_failover)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(failport)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, failport, &port));
        } else {
            port = failport;
        }
        modid = -1;
    } else {
        if (!soc_feature(unit, soc_feature_port_lag_failover)) {
            return BCM_E_UNAVAIL;
        }
        
#if defined(BCM_TRIDENT_SUPPORT)
        if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
            if (BCM_GPORT_IS_SET(failport)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_local_get(unit, failport, &port));
            } else {
                port = failport;
            }

            if (IS_HG_PORT(unit, port)) {
                return BCM_E_UNAVAIL;
            }
            /* not supported for Cascaded Physical Ports*/
            if ((soc_feature(unit, soc_feature_linkphy_coe) &&
                IS_LP_PORT(unit, port)) ||
                (soc_feature(unit, soc_feature_subtag_coe) &&
                IS_SUBTAG_PORT(unit, port)) ||
                ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                 (soc_feature(unit, soc_feature_channelized_switching))) &&
                IS_SUBTAG_PORT (unit, port))) {
                return BCM_E_UNAVAIL;
            }
        }
#endif
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, failport, &modid, &port, &tgid, &id));
        if ((-1 != tgid) || (-1 != id)) {
            return BCM_E_PARAM;
        }
    }

    if ((psc == NULL) || (flags == NULL) || (array_size < 0) ||
        (fail_to_array == NULL) || (array_count == NULL)) {
        return BCM_E_PARAM;
    }

    /* Offset of trunk type */
    tgid = tid - (fabric_trunk ? TRUNK_CNTL(unit).ngroups_fp : 0);

    if (array_size > 0) {
        ftp = sal_alloc(sizeof(bcm_port_t) * array_size, "failover ports");
        if (NULL == ftp) {
            return BCM_E_MEMORY;
        }
        ftm = sal_alloc(sizeof(bcm_module_t) * array_size, "failover mods");
        if (NULL == ftm) {
            sal_free(ftp);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(ftm, 0, sizeof(bcm_module_t) * array_size);
    sal_memset(ftp, 0, sizeof(bcm_port_t) * array_size);

    TRUNK_LOCK(unit);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        rv = _bcm_th3_trunk_hwfailover_get(unit, tid,
                port, psc, flags, array_size, ftp, array_count);
    } else 
#endif
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
        rv = _bcm_trident_trunk_hwfailover_get(unit, tgid, fabric_trunk,
                port, modid, psc, flags, array_size, ftp, ftm, array_count);
    } else
#endif /* BCM_TRIDENT_SUPPORT */
    {
        rv = _bcm_xgs3_trunk_hwfailover_get(unit, tgid, fabric_trunk,
                port, modid, psc, flags, array_size, ftp, ftm, array_count);
    }
    TRUNK_UNLOCK(unit);

    if (BCM_SUCCESS(rv)) {
        rv = _bcm_esw_trunk_gport_construct(unit, fabric_trunk, *array_count,
                                          ftp, ftm, fail_to_array);
    }

    if (NULL != ftp) {
        sal_free(ftp);
    }
    if (NULL != ftm) {
        sal_free(ftm);
    }

    return rv;

#else /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
    return BCM_E_UNAVAIL; 
#endif
}

/*
 * Function:
 *      bcm_esw_trunk_member_add
 * Purpose:
 *      Add a member to a trunk group.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      tid    - (IN) Trunk id.
 *      member - (IN) Pointer to a trunk member structure.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_trunk_member_add(
    int unit, 
    bcm_trunk_t tid, 
    bcm_trunk_member_t *member)
{
    trunk_private_t    *t_info;
    int                rv = BCM_E_NONE;
    int                member_count;
    bcm_trunk_member_t *member_array = NULL;
    bcm_trunk_info_t   trunk_info;
    int                tid_is_vp_lag;
    bcm_module_t       modid, member_modid;
    bcm_port_t         port, member_port;
    bcm_trunk_t        tgid, member_tgid;
    int                id, member_id;
    int                i;

    TRUNK_INIT(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag));
    if (!tid_is_vp_lag) {
        TRUNK_CHECK(unit, tid);
        t_info = &TRUNK_INFO(unit, tid);
        if (t_info->tid == BCM_TRUNK_INVALID) {
            return BCM_E_NOT_FOUND;
        }
    }
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
    if (soc_feature(unit, soc_feature_epc_linkflap_recover)) {
        int status = 1;
        SOC_LINKSCAN_MODPORT_MAP_LOCK(unit);
        rv =  _bcm_td_stk_modport_map_member_link_handler(unit,
                                  member->gport, tid, 0, &status);
        SOC_LINKSCAN_MODPORT_MAP_UNLOCK(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        if (status == 0) {
            return rv;
        } else {
            rv = BCM_E_NONE;
        }
    }
#endif /* #(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT) */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, member->gport,
                                &modid, &port, &tgid, &id));
    if (!tid_is_vp_lag && (-1 != tgid)) {
        return BCM_E_PARAM;
    }

    /* Get number of trunk group's existing member ports */
    BCM_IF_ERROR_RETURN
        (bcm_esw_trunk_get(unit, tid, &trunk_info, 0, NULL, &member_count));

    /* Allocate member array to accomodate existing members and
     * the new member to be added.
     */
    member_array = sal_alloc(sizeof(bcm_trunk_member_t) *
            (member_count + 1), "trunk_member_array");
    if (member_array == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(member_array, 0,
            sizeof(bcm_trunk_member_t) * (member_count + 1));

    /* Retrieve existing member ports */
    rv = bcm_esw_trunk_get(unit, tid, &trunk_info, member_count + 1,
            member_array, &member_count);
    if (BCM_FAILURE(rv)) {
        sal_free(member_array);
        return rv;
    }

    /* If resilient hashing is enabled, check that the given member is not
     * already a member of the group.
     */
    if (trunk_info.psc == BCM_TRUNK_PSC_DYNAMIC_RESILIENT) {
        for (i = 0; i < member_count; i++) {
            rv = _bcm_esw_gport_resolve(unit, member_array[i].gport,
                    &member_modid, &member_port, &member_tgid, &member_id);
            if (BCM_FAILURE(rv)) { 
                sal_free(member_array);
                return rv;
            }
            if (-1 != member_id) {
                if (member_array[i].gport == member->gport) {
                    sal_free(member_array);
                    return BCM_E_EXISTS;
                }
            } else if (-1 != member_tgid) {
                sal_free(member_array);
                return BCM_E_INTERNAL;
            } else {
                if ((member_modid == modid) && (member_port == port)) {
                    sal_free(member_array);
                    return BCM_E_EXISTS;
                }
            }
        }
    }

#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        rv = _bcm_esw_trunk_subport_sanitize(unit, member_count,
                 member_array, member);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TRUNK,
                (BSL_META_U(unit,"subport and phy port mixed in trunk.\n")));
            sal_free(member_array);
            return rv;
        }
    }
#endif /*BCM_CHANNELIZED_SWITCHING_SUPPORT*/

    /* Add given member to group */
    member_array[member_count].flags = member->flags;
    member_array[member_count].gport = member->gport;
    member_array[member_count].dynamic_scaling_factor =
        member->dynamic_scaling_factor;
    member_array[member_count].dynamic_load_weight =
        member->dynamic_load_weight;
    member_array[member_count].dynamic_queue_size_weight =
        member->dynamic_queue_size_weight;
    rv = _bcm_esw_trunk_modify(unit, tid, &trunk_info, member_count + 1,
            member_array, TRUNK_MEMBER_OP_ADD, member);
    sal_free(member_array);
    return rv;
}

/*
 * Function:
 *      bcm_esw_trunk_member_delete
 * Purpose:
 *      Remove a member from a trunk group.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      tid    - (IN) Trunk id.
 *      member - (IN) Pointer to a trunk member structure.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_trunk_member_delete(
    int unit, 
    bcm_trunk_t tid, 
    bcm_trunk_member_t *member)
{
    trunk_private_t    *t_info;
    bcm_module_t       modid;
    bcm_port_t         port;
    bcm_trunk_t        tgid;
    int                id;
    int                rv = BCM_E_NONE;
    int                i, k;
    int                member_count;
    bcm_trunk_member_t *member_array;
    bcm_module_t       member_modid;
    bcm_port_t         member_port;
    bcm_trunk_t        member_tgid;
    int                member_id;
    bcm_trunk_info_t   trunk_info;
    int                tid_is_vp_lag;

    TRUNK_INIT(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag));
    if (!tid_is_vp_lag) {
        TRUNK_CHECK(unit, tid);
        t_info = &TRUNK_INFO(unit, tid);
        if (t_info->tid == BCM_TRUNK_INVALID) {
            return BCM_E_NOT_FOUND;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, member->gport,
                                &modid, &port, &tgid, &id));
    if (!tid_is_vp_lag && (-1 != tgid)) {
        return BCM_E_PARAM;
    }

    /* Get number of trunk group's existing member ports */
    BCM_IF_ERROR_RETURN
        (bcm_esw_trunk_get(unit, tid, &trunk_info, 0, NULL, &member_count));

    if (member_count == 0) {
        return BCM_E_NOT_FOUND;
    } 

    /* Allocate member array to get existing members */
    member_array = sal_alloc(sizeof(bcm_trunk_member_t) * member_count,
            "trunk_member_array");
    if (member_array == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(member_array, 0,
            sizeof(bcm_trunk_member_t) * member_count);

    /* Retrieve existing member ports */
    rv = bcm_esw_trunk_get(unit, tid, &trunk_info, member_count,
            member_array, &member_count);
    if (BCM_FAILURE(rv)) {
        sal_free(member_array);
        return rv;
    }

    /* Find the index of the given member in the array */
    for (i = 0; i < member_count; i++) {
        rv = _bcm_esw_gport_resolve(unit, member_array[i].gport,
                &member_modid, &member_port,
                &member_tgid, &member_id);
        if (BCM_FAILURE(rv)) { 
            sal_free(member_array);
            return rv;
        }
        if (-1 != member_id) {
            if (member_array[i].gport == member->gport) {
                break;
            }
        } else if (-1 != member_tgid) {
            sal_free(member_array);
            return BCM_E_INTERNAL;
        } else {
            if ((member_modid == modid) && (member_port == port)) {
                break;
            }
        }
    }

    if (i == member_count) {
        sal_free(member_array);
        return BCM_E_NOT_FOUND;
    }

    /* Delete given member from array, and shift the remaining
     * elements of the array.
 */
    for (k = i; k < member_count - 1; k++) {
        member_array[k].flags = member_array[k+1].flags;
        member_array[k].gport = member_array[k+1].gport;
    }

    member_count--;
    if (0 == member_count) {
        rv = _bcm_esw_trunk_modify(unit, tid, &trunk_info,
                0, NULL, TRUNK_MEMBER_OP_DELETE, member);
    } else {
        rv = _bcm_esw_trunk_modify(unit, tid, &trunk_info,
                member_count, member_array, TRUNK_MEMBER_OP_DELETE, member);
    }

    sal_free(member_array);
    return rv;
}

/*
 * Function:
 *      bcm_esw_trunk_member_delete_all
 * Purpose:
 *      Remove all members of a trunk group.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      tid    - (IN) Trunk id.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_trunk_member_delete_all(
    int unit, 
    bcm_trunk_t tid) 
{
    bcm_trunk_info_t trunk_info;
    int member_count;

    BCM_IF_ERROR_RETURN
        (bcm_esw_trunk_get(unit, tid, &trunk_info, 0, NULL, &member_count));

    if (member_count > 0) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_trunk_set(unit, tid, &trunk_info, 0, NULL));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_trunk_member_status_set
 * Purpose:
 *      Set trunk member status.
 * Parameters:
 *      unit - (IN) Unit number.
 *      member - (IN) Trunk member GPORT ID.
 *      status - (IN) Trunk member link status: 0 = down, 1 = up.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_trunk_member_status_set(
    int unit, 
    bcm_gport_t member, 
    int status)
{
    int rv = BCM_E_UNAVAIL;
    bcm_module_t mod;
    bcm_port_t port;
    bcm_trunk_t tid;
    int hw_id;
    int is_local;
    int set_hg_dlb_member_status = FALSE;

    TRUNK_INIT(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, member, &mod, &port, &tid, &hw_id));
    if ((tid != -1) || (hw_id != -1)) {
        return BCM_E_PORT;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, mod, &is_local));

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_hgt_lag_dlb_optimized)) {
        if (!is_local) {
            return BCM_E_PORT;
        }
        if (soc_feature(unit, soc_feature_hg_dlb)) {
            if (!IS_HG_PORT(unit, port)) {
                return BCM_E_PORT;
            }
        }
        TRUNK_LOCK(unit);
        rv = bcm_th2_hgt_lag_dlb_member_status_set(unit, port, status);
        TRUNK_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TOMAHAWK2_SUPPORT */

    if (is_local) {
        if (IS_HG_PORT(unit, port)) {
            set_hg_dlb_member_status = TRUE;
        }
    }

    TRUNK_LOCK(unit);

    if (set_hg_dlb_member_status) {
        if (soc_feature(unit, soc_feature_hg_dlb)) {
#ifdef BCM_TRIDENT_SUPPORT
            rv = bcm_trident_hg_dlb_member_status_set(unit, port, status);
#endif /* BCM_TRIDENT_SUPPORT */
        } else {
            rv = BCM_E_UNAVAIL;
        }
    } else {
        if (soc_feature(unit, soc_feature_lag_dlb)) {
#ifdef BCM_TRIUMPH3_SUPPORT
            rv = bcm_tr3_lag_dlb_member_status_set(unit, mod, port, status);
#endif /* BCM_TRIUMPH3_SUPPORT */
        } else {
            rv = BCM_E_UNAVAIL;
        }
    }

    TRUNK_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_trunk_member_status_get
 * Purpose:
 *      Get trunk member status.
 * Parameters:
 *      unit - (IN) Unit number.
 *      member - (IN) Trunk member GPORT ID.
 *      status - (OUT) Trunk member link status.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_trunk_member_status_get(
    int unit, 
    bcm_gport_t member, 
    int *status)
{
    int rv = BCM_E_UNAVAIL;
    bcm_module_t mod;
    bcm_port_t port;
    bcm_trunk_t tid;
    int hw_id;
    int is_local;
    int get_hg_dlb_member_status = FALSE;

    if (NULL == status) {
        return BCM_E_PARAM;
    }

    TRUNK_INIT(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, member, &mod, &port, &tid, &hw_id));
    if ((tid != -1) || (hw_id != -1)) {
        return BCM_E_PORT;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, mod, &is_local));

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_hgt_lag_dlb_optimized)) {
        if (!is_local) {
            return BCM_E_PORT;
        }
        if (soc_feature(unit, soc_feature_hg_dlb)) {
            if (!IS_HG_PORT(unit, port)) {
                return BCM_E_PORT;
            }
        }
        TRUNK_LOCK(unit);
        rv = bcm_th2_hgt_lag_dlb_member_status_get(unit, port, status);
        TRUNK_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TOMAHAWK2_SUPPORT */
    if (is_local) {
        if (IS_HG_PORT(unit, port)) {
            get_hg_dlb_member_status = TRUE;
        }
    }

    TRUNK_LOCK(unit);

    if (get_hg_dlb_member_status) {
        if (soc_feature(unit, soc_feature_hg_dlb)) {
#ifdef BCM_TRIDENT_SUPPORT
            rv = bcm_trident_hg_dlb_member_status_get(unit, port, status);
#endif /* BCM_TRIDENT_SUPPORT */
        } else {
            rv = BCM_E_UNAVAIL;
        }
    } else {
        if (soc_feature(unit, soc_feature_lag_dlb)) {
#ifdef BCM_TRIUMPH3_SUPPORT
            rv = bcm_tr3_lag_dlb_member_status_get(unit, mod, port, status);
#endif /* BCM_TRIUMPH3_SUPPORT */
        } else {
            rv = BCM_E_UNAVAIL;
        }
    }

    TRUNK_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_trunk_ethertype_set
 * Purpose:
 *      Set the Ethertypes that are eligible or ineligible for
 *      dynamic load balancing or resilient hashing.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_TRUNK_DYNAMIC_ETHERTYPE_xxx flags.
 *      ethertype_count - (IN) Number of elements in ethertype_array.
 *      ethertype_array - (IN) Array of Ethertypes.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_trunk_ethertype_set(
    int unit, 
    uint32 flags, 
    int ethertype_count, 
    int *ethertype_array)
{
    int rv;
    int i;

    if ((ethertype_count > 0) && (NULL == ethertype_array)) {
        return BCM_E_PARAM;
    }
    for (i = 0; i < ethertype_count; i++) {
        if (ethertype_array[i] < 0 || ethertype_array[i] > 0xffff) {
            return BCM_E_PARAM;
        }
    }

    TRUNK_INIT(unit);
    TRUNK_LOCK(unit);

    rv = BCM_E_UNAVAIL;

    if (flags & BCM_TRUNK_DYNAMIC_ETHERTYPE_TRUNK) {
        if (flags & BCM_TRUNK_DYNAMIC_ETHERTYPE_RESILIENT) {
#ifdef BCM_TRIDENT2_SUPPORT
            if (soc_feature(unit, soc_feature_lag_resilient_hash)) {
                rv = bcm_td2_lag_rh_ethertype_set(unit, flags, ethertype_count,
                        ethertype_array);
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return rv;
                }
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                /* Resilient hashing not supported for LAGs */
                return BCM_E_UNAVAIL;
            }

        } else {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_lag_dlb)) {
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
                if (soc_feature(unit, soc_feature_hgt_lag_dlb_optimized)) {
                    rv = bcm_th2_hgt_lag_dlb_ethertype_set(unit, flags,
                            ethertype_count, ethertype_array);
                } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
                {
                    rv = bcm_tr3_lag_dlb_ethertype_set(unit, flags, ethertype_count,
                            ethertype_array);
                }
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return rv;
                }
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
        }
    } else if (flags & BCM_TRUNK_DYNAMIC_ETHERTYPE_FABRIC_TRUNK) {
        if (flags & BCM_TRUNK_DYNAMIC_ETHERTYPE_RESILIENT) {
#ifdef BCM_TRIDENT2_SUPPORT
            if (soc_feature(unit, soc_feature_hg_resilient_hash)) {
                rv = bcm_td2_hg_rh_ethertype_set(unit, flags, ethertype_count,
                        ethertype_array);
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return rv;
                }
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                /* Resilient hashing not supported for LAGs */
                return BCM_E_UNAVAIL;
            }
        } else {
            if (soc_feature(unit, soc_feature_hg_dlb)) {
#ifdef BCM_TRIUMPH3_SUPPORT
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
                if (soc_feature(unit, soc_feature_hgt_lag_dlb_optimized)) {
                    rv = bcm_th2_hgt_lag_dlb_ethertype_set(unit, flags, ethertype_count,
                            ethertype_array);
                    if (BCM_FAILURE(rv)) {
                        TRUNK_UNLOCK(unit);
                        return rv;
                    }
                } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
                {
                    if (soc_mem_is_valid(unit, DLB_HGT_ETHERTYPE_ELIGIBILITY_MAPm)) {
                        rv = bcm_tr3_hg_dlb_ethertype_set(unit, flags, ethertype_count,
                                ethertype_array);
                        if (BCM_FAILURE(rv)) {
                            TRUNK_UNLOCK(unit);
                            return rv;
                        }
                    }
                }
#endif /* BCM_TRIUMPH3_SUPPORT */
            }
        }
    } else {
        TRUNK_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    TRUNK_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_trunk_ethertype_get
 * Purpose:
 *      Get the Ethertypes that are eligible or ineligible for
 *      dynamic load balancing or resilient hashing.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (INOUT) BCM_TRUNK_DYNAMIC_ETHERTYPE_xxx flags.
 *      ethertype_max - (IN) Number of elements in ethertype_array.
 *      ethertype_array - (OUT) Array of Ethertypes.
 *      ethertype_count - (OUT) Number of elements returned in ethertype_array.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_trunk_ethertype_get(
    int unit, 
    uint32 *flags, 
    int ethertype_max, 
    int *ethertype_array, 
    int *ethertype_count)
{
    int rv;

    if (NULL == flags) {
        return BCM_E_PARAM;
    }
    if ((ethertype_max > 0) && (NULL == ethertype_array)) {
        return BCM_E_PARAM;
    }
    if (NULL == ethertype_count) {
        return BCM_E_PARAM;
    }

    TRUNK_INIT(unit);
    TRUNK_LOCK(unit);

    rv = BCM_E_UNAVAIL;

    if (*flags & BCM_TRUNK_DYNAMIC_ETHERTYPE_TRUNK) {
        if (*flags & BCM_TRUNK_DYNAMIC_ETHERTYPE_RESILIENT) {
#ifdef BCM_TRIDENT2_SUPPORT
            if (soc_feature(unit, soc_feature_lag_resilient_hash)) {
                rv = bcm_td2_lag_rh_ethertype_get(unit, flags, ethertype_max,
                        ethertype_array, ethertype_count);
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return rv;
                }
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                /* Resilient hashing not supported for LAGs */
                return BCM_E_UNAVAIL;
            }
        } else {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_lag_dlb)) {
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
                if (soc_feature(unit, soc_feature_hgt_lag_dlb_optimized)) {
                    rv = bcm_th2_hgt_lag_dlb_ethertype_get(unit, flags,
                            ethertype_max, ethertype_array, ethertype_count);
                } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
                {
                    rv = bcm_tr3_lag_dlb_ethertype_get(unit, flags, ethertype_max,
                            ethertype_array, ethertype_count);
                }
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return rv;
                }
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
        }
    } else if (*flags & BCM_TRUNK_DYNAMIC_ETHERTYPE_FABRIC_TRUNK) {
        if (*flags & BCM_TRUNK_DYNAMIC_ETHERTYPE_RESILIENT) {
#ifdef BCM_TRIDENT2_SUPPORT
            if (soc_feature(unit, soc_feature_hg_resilient_hash)) {
                rv = bcm_td2_hg_rh_ethertype_get(unit, flags, ethertype_max,
                        ethertype_array, ethertype_count);
                if (BCM_FAILURE(rv)) {
                    TRUNK_UNLOCK(unit);
                    return rv;
                }
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                /* Resilient hashing not supported for Higig trunks */
                return BCM_E_UNAVAIL;
            }
        } else {
            if (soc_feature(unit, soc_feature_hg_dlb)) {
#ifdef BCM_TRIUMPH3_SUPPORT
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
                if (soc_feature(unit, soc_feature_hgt_lag_dlb_optimized)) {
                    rv = bcm_th2_hgt_lag_dlb_ethertype_get(unit, flags,
                            ethertype_max, ethertype_array, ethertype_count);
                    if (BCM_FAILURE(rv)) {
                        TRUNK_UNLOCK(unit);
                        return rv;
                    }
                } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
                {
                    if (soc_mem_is_valid(unit, DLB_HGT_ETHERTYPE_ELIGIBILITY_MAPm)) {
                        rv = bcm_tr3_hg_dlb_ethertype_get(unit, flags, ethertype_max,
                                ethertype_array, ethertype_count);
                        if (BCM_FAILURE(rv)) {
                            TRUNK_UNLOCK(unit);
                            return rv;
                        }
                    }
                }
#endif /* BCM_TRIUMPH3_SUPPORT */
            }
        }
    } else {
        TRUNK_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    TRUNK_UNLOCK(unit);
    return rv;
}

#ifdef BCM_TOMAHAWK_SUPPORT

STATIC int
_bcm_th_trunk_agm_update(int unit, bcm_trunk_t tid,
                         bcm_switch_agm_type_t agm_type,
                         int agm_pool_id,
                         int agm_index)
{
    int rv = BCM_E_NONE, hgtid;
    uint32 tg_entry[SOC_MAX_MEM_WORDS];
    hg_trunk_group_entry_t hg_trunk_group_entry;
    bcm_trunk_chip_info_t chip_info;
    int tid_is_vp_lag;
    trunk_private_t *t_info;

    TRUNK_INIT(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_trunk_id_is_vp_lag(unit, tid, &tid_is_vp_lag));
    if (!tid_is_vp_lag) {
        TRUNK_CHECK(unit, tid);
        t_info = &TRUNK_INFO(unit, tid);
        if (t_info->tid == BCM_TRUNK_INVALID) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        /* XXX vp_lag ? */
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_trunk_chip_info_get(unit, &chip_info));
    LOG_INFO(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "tid %d min %d tid max %d fabId min %d fabId max %d \n"),
                tid, chip_info.trunk_id_min, chip_info.trunk_id_max,
                chip_info.trunk_fabric_id_min, chip_info.trunk_fabric_id_max));

    if ((tid >= chip_info.trunk_id_min) && (tid <= chip_info.trunk_id_max)) {
        if (agm_type != bcmSwitchAgmTypeTrunk) {
            return BCM_E_PARAM;
        }
        /* trunk group entry */
        if (soc_feature(unit, soc_feature_fastlag)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                 fast_trunk_ports_1_entry_t  fast_1;
                 SOC_IF_ERROR_RETURN
                     (READ_FAST_TRUNK_PORTS_1m(unit, MEM_BLOCK_ALL, tid, &fast_1));
                 soc_FAST_TRUNK_PORTS_1m_field32_set(unit, &fast_1, AGM_MONITOR_POOL_IDf, agm_pool_id);
                 soc_FAST_TRUNK_PORTS_1m_field32_set(unit, &fast_1, AGM_MONITOR_IDf, agm_index);
                 SOC_IF_ERROR_RETURN
                     (WRITE_FAST_TRUNK_PORTS_1m(unit, MEM_BLOCK_ALL, tid, &fast_1));
            } else
#endif
            {
                SOC_IF_ERROR_RETURN
                    (READ_FAST_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tid, &tg_entry));
                soc_FAST_TRUNK_GROUPm_field32_set(unit, &tg_entry, AGM_MONITOR_POOL_IDf,
                                             agm_pool_id);
                soc_FAST_TRUNK_GROUPm_field32_set(unit, &tg_entry, AGM_MONITOR_IDf,
                                             agm_index);
                SOC_IF_ERROR_RETURN
                    (WRITE_FAST_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tid, &tg_entry));
            }
        } else { /* Not a Fast lag */
            SOC_IF_ERROR_RETURN
                (READ_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tid, &tg_entry));
            soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, AGM_MONITOR_POOL_IDf,
                                         agm_pool_id);
            soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, AGM_MONITOR_IDf,
                                         agm_index);
            SOC_IF_ERROR_RETURN
                (WRITE_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tid, &tg_entry));
        }
    } else if ((tid >= chip_info.trunk_fabric_id_min) &&
               (tid <= (chip_info.trunk_fabric_id_max + 1))) {
        if (agm_type != bcmSwitchAgmTypeFabricTrunk) {
            return BCM_E_PARAM;
        }
        /* Higig trunk group entry */
        hgtid = tid - chip_info.trunk_fabric_id_min;
        SOC_IF_ERROR_RETURN
            (READ_HG_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, hgtid,
                                  &hg_trunk_group_entry));
        soc_HG_TRUNK_GROUPm_field32_set(unit, &hg_trunk_group_entry,
                                        AGM_MONITOR_POOL_IDf, agm_pool_id);
        soc_HG_TRUNK_GROUPm_field32_set(unit, &hg_trunk_group_entry,
                                        AGM_MONITOR_IDf, agm_index);
        SOC_IF_ERROR_RETURN
            (WRITE_HG_TRUNK_GROUPm(unit, MEM_BLOCK_ALL, hgtid,
                &hg_trunk_group_entry));
    } else {
        rv = BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *      bcm_th_trunk_agm_attach
 * Purpose:
 *      Attach already created monitor entry to a trunk or a fabric trunk
 *      Internally, Map the monitor_entry_id to an aggregation group.
 */
int
bcm_th_trunk_agm_attach(int unit, bcm_trunk_t tid,
                        bcm_switch_agm_id_t agm_id)
{
    int rv = BCM_E_NONE;

    agm_monitor_t agm_mnt;
    bcm_switch_agm_id_t agm_id_old;
    int agm_pool_id;
    uint32 agm_index;

    bcm_trunk_info_t tinfo;
    int mbm_cnt, mbm_max;
    bcm_trunk_member_t *mbm_arr;

    /* Validate parameters */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    TRUNK_CHECK(unit, tid);

    BCM_IF_ERROR_RETURN(bcm_th_switch_agm_info(unit, agm_id, &agm_mnt));
    if (agm_mnt.group_id == (int)tid) {
        return BCM_E_EXISTS;
    }

    mbm_max = agm_mnt.attr.num_members;
    mbm_arr = sal_alloc(mbm_max * sizeof(*mbm_arr), "mbm_arr");
    if (mbm_arr == NULL) {
        return BCM_E_MEMORY;
    }

    rv = bcm_esw_trunk_get(unit, tid, &tinfo, mbm_max, mbm_arr, &mbm_cnt);
    sal_free(mbm_arr);    
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (mbm_cnt != agm_mnt.attr.num_members) {
        LOG_VERBOSE(BSL_LS_BCM_TRUNK, (BSL_META_U(unit,
                    "AGM %d member cnt %d doesn't match trunk %d member cnt %d\n"), 
                    agm_id, agm_mnt.attr.num_members, tid, mbm_cnt));
        return BCM_E_PARAM;
    }

    agm_pool_id = agm_mnt.agm_pool_id;
    agm_index   = agm_mnt.attr.agm_id;

    BCM_IF_ERROR_RETURN(_bcm_th_trunk_agm_update(unit, tid, agm_mnt.attr.agm_type,
                                                 agm_pool_id, agm_index));

    /* in case of changing AGM, detach the old AGM first */
    if (BCM_SUCCESS(bcm_th_switch_agm_id_get_by_group(unit, tid, &agm_id_old))) {
        BCM_IF_ERROR_RETURN(bcm_th_switch_agm_fwd_grp_update(unit, agm_id_old, 
                            _AGM_GROUP_ID_INVALID));
    }

    BCM_IF_ERROR_RETURN(bcm_th_switch_agm_fwd_grp_update(unit, agm_id, tid));

    return rv;
}

/*
 * Function:
 *      bcm_th_trunk_agm_detach
 * Purpose:
 *      Detach a monitor entry from a trunk or a fabric trunk
 *      Internally, un-map the monitor_entry_id from an aggregation
 *      group (fp_lag/fabric_channel_lag).
 */
int
bcm_th_trunk_agm_detach(int unit, bcm_trunk_t tid,
                        bcm_switch_agm_id_t agm_id)
{
    int rv = BCM_E_NONE;
    agm_monitor_t agm_mnt;

    /* Validate parameters */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    TRUNK_CHECK(unit, tid);

    BCM_IF_ERROR_RETURN(bcm_th_switch_agm_info(unit, agm_id, &agm_mnt));
    if (agm_mnt.group_id != (int)tid) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(_bcm_th_trunk_agm_update(unit, tid, 
                        agm_mnt.attr.agm_type, 0, _AGM_ID_INVALID));
    BCM_IF_ERROR_RETURN(bcm_th_switch_agm_fwd_grp_update(unit, agm_id, 
                        _AGM_GROUP_ID_INVALID));

    return rv;
}

/*
 * Function:
 *      bcm_th_trunk_agm_attach_get
 * Purpose:
 *      Retrieve a monitor id attached to a (fabric)trunk
 */
int
bcm_th_trunk_agm_attach_get(int unit, bcm_trunk_t trunk_id,
                            bcm_switch_agm_id_t *agm_id)
{
    /* Validate parameters */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    TRUNK_CHECK(unit, trunk_id);

    return bcm_th_switch_agm_id_get_by_group(unit, trunk_id, agm_id);
}

#endif /* BCM_TOMAHAWK_SUPPORT */

/*
 * Function:
 *      bcm_esw_trunk_agm_attach
 * Purpose:
 *      Attach already created monitor entry to a trunk or a fabric trunk
 *      Internally, Map the monitor_entry_id to an aggregation group.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      trunk_id - (IN) id of a (fabric) trunk.
 *      agm_id - (IN) id of aggregation group monitoring entry created
 *                        via the bcm_switch_agm_create api.
 * Returns:
 *      BCM_E_NONE: if the attachment is succeed
 *      BCM_E_PARAM: if the monitor_id is not valid or the monitor
 *                   entry is not created. If the trunk id is not valid
 *      BCM_E_FULL: if a monitor entry is already attached to
 *      BCM_E_PARAM: if number of trunk member is different from the
 *                   number on the monitor entry
 */
int
bcm_esw_trunk_agm_attach(int unit, bcm_trunk_t trunk_id,
                         bcm_switch_agm_id_t agm_id)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_agm)) {
        TRUNK_LOCK(unit);
        rv = bcm_th_trunk_agm_attach(unit, trunk_id, agm_id);
        TRUNK_UNLOCK(unit);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
    return rv;
}

/*
 * Function:
 *      bcm_esw_trunk_agm_detach
 * Purpose:
 *      Detach a monitor entry from a trunk or a fabric trunk
 *      Internally, un-map the monitor_entry_id from an aggregation
 *      group (fp_lag/fabric_channel_lag).
 * Parameters:
 *      unit       - (IN) bcm device.
 *      trunk_id   - (IN) id of a (fabric) trunk.
 *      agm_id - (IN) id of aggregation group monitoring entry created
 *                        via the bcm_switch_agm_create api.
 * Returns:
 *      BCM_E_NONE: if the enable or disable is succeed
 *      BCM_E_PARAM: If the ecmp id is not valid
 *      BCM_E_NOT_FOUND: if there is no monitor entry attached to the
 *                       aggregation group.
 */
int
bcm_esw_trunk_agm_detach(int unit, bcm_trunk_t trunk_id,
                         bcm_switch_agm_id_t agm_id)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_agm)) {
        TRUNK_LOCK(unit);
        rv = bcm_th_trunk_agm_detach(unit, trunk_id, agm_id);
        TRUNK_UNLOCK(unit);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
    return rv;
}

/*
 * Function:
 *      bcm_esw_trunk_agm_attach_get
 * Purpose:
 *      Retrieve a monitor id attached to a (fabric)trunk
 * Parameters:
 *      unit       - (IN) bcm device.
 *      trunk_id   - (IN) (fabric) trunk id.
 *      agm_id - (IN/OUT) aggregation group monitoring entry.
 * Returns:
 *      BCM_E_NONE: if monitor entry id found
 *      BCM_E_NOT_FOUND: if no monitor entry is attached to the
 *                       aggregation group.
 */
int
bcm_esw_trunk_agm_attach_get(int unit, bcm_trunk_t trunk_id,
                             bcm_switch_agm_id_t *agm_id)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_agm)) {
        TRUNK_LOCK(unit);
        rv = bcm_th_trunk_agm_attach_get(unit, trunk_id, agm_id);
        TRUNK_UNLOCK(unit);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
    return rv;
}

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
/*
 * Function    : _bcm_esw_trunk_gport_add
 * Description : Add the GPORT ID for the specified trunk.
 *
 * Parameters  : (IN)  unit          - BCM device number
 *                     (IN)  tid     - Trunk Group
 *                     (IN)  gport   - GPORT ID
 * Returns     : BCM_E_XXX
 */
int
_bcm_esw_trunk_gport_add(int unit, bcm_trunk_t tid, bcm_gport_t gport)
{
    int id_out = 0;
    int result = FALSE;
    int is_trunk = 0;
    int base_index = 0;
    int subport_index = 0;
    bcm_port_t port_out = 0;
    bcm_module_t mod_out = 0;
    bcm_trunk_t tgid_out = 0;
    bcm_trunk_t old_tid = 0;
    modid_base_ptr_entry_t modid_base_entry;
    modport_map_subport_entry_t modport_map_subport_entry;
    trunk_private_t *t_info;

    /* Make sure trunk module is initialized. */
    TRUNK_INIT(unit);

    /* Parameter check */
    if (!TRUNK_FABRIC_TID(unit, tid)) {
        return BCM_E_PARAM;
    }

    t_info = &TRUNK_INFO(unit, tid);
    if (t_info->tid == BCM_TRUNK_INVALID) {
        return BCM_E_NOT_FOUND;
    }

    tid -= TRUNK_CNTL(unit).ngroups_fp;

    if (!BCM_GPORT_IS_MODPORT(gport)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, gport,
                            &mod_out, &port_out, &tgid_out, &id_out));
    if ((-1 != tgid_out) || (-1 != id_out)) {
        return BCM_E_PARAM;
    }

    /* Check modport in one of additional module */
    BCM_IF_ERROR_RETURN (
        _bcm_esw_my_modid_additional_check(unit, mod_out, &result));
    if (result == FALSE) {
        return BCM_E_PARAM;
    }

    mod_out %= 64;  /* lower 6 bit */
    SOC_IF_ERROR_RETURN
        (READ_MODID_BASE_PTRm(unit, MEM_BLOCK_ANY, mod_out,
                                  &modid_base_entry));
    base_index = soc_MODID_BASE_PTRm_field32_get(unit,
                                       &modid_base_entry, MODID_BASEf);
    subport_index = base_index; /* the index is indicated by module base ptr */
    SOC_IF_ERROR_RETURN
        (READ_MODPORT_MAP_SUBPORTm(unit, MEM_BLOCK_ANY, subport_index,
                                       &modport_map_subport_entry));
    is_trunk = soc_MODPORT_MAP_SUBPORTm_field32_get(unit,
                                       &modport_map_subport_entry, ISTRUNKf);
    old_tid = soc_MODPORT_MAP_SUBPORTm_field32_get(unit,
                                       &modport_map_subport_entry, DESTf);
    if (is_trunk && (old_tid == tid)) {
        return BCM_E_NONE;
    }
    if (is_trunk && old_tid >= 0) {
        return BCM_E_RESOURCE;
    }

    soc_MODPORT_MAP_SUBPORTm_field32_set (unit,
                         &modport_map_subport_entry, DESTf, tid);
    soc_MODPORT_MAP_SUBPORTm_field32_set (unit,
                         &modport_map_subport_entry, ISTRUNKf, 1);
    soc_MODPORT_MAP_SUBPORTm_field32_set (unit,
                         &modport_map_subport_entry, ENABLEf, 1);
    SOC_IF_ERROR_RETURN
        (WRITE_MODPORT_MAP_SUBPORTm(unit, MEM_BLOCK_ANY, subport_index,
                                        &modport_map_subport_entry));

    return BCM_E_NONE;
}

/*
 * Function    : _bcm_esw_trunk_gport_get_all
 * Description : Add the GPORT ID for the specified trunk.
 *
 * Parameters  : (IN)  unit               - BCM device number
 *                     (IN)  tid          - Trunk Group
 *                     (IN)  gport_size   - GPORT ID
                       (OUT) gport_array  - GPORT Array
                       (OUT) count        - GPORT Count
 * Returns     : BCM_E_XXX
 */
int
_bcm_esw_trunk_gport_get_all(int unit, bcm_trunk_t tid, int gport_size,
                            bcm_gport_t *gport_array, int *count)
{
    int rv = BCM_E_NONE;
    int mod_idx = 0;
    int port_idx = 0;
    int is_trunk = 0;
    int base_index = 0;
    bcm_gport_t modport = 0;
    _bcm_gport_dest_t dest;
    bcm_trunk_t egress_tid = 0;
    bcm_stk_modid_config_t mod_cfg;
    modid_base_ptr_entry_t modid_base_entry;
    modport_map_subport_entry_t modport_map_subport_entry;
    trunk_private_t *t_info;

    /* Make sure trunk module is initialized. */
    TRUNK_INIT(unit);

    /* Parameter check */
    if (!TRUNK_FABRIC_TID(unit, tid)) {
        return BCM_E_PARAM;
    }

    t_info = &TRUNK_INFO(unit, tid);
    if (t_info->tid == BCM_TRUNK_INVALID) {
        return BCM_E_NOT_FOUND;
    }

    tid -= TRUNK_CNTL(unit).ngroups_fp;

    if (gport_size < 0 || count == NULL) {
        return BCM_E_PARAM;
    }

    if ((gport_size > 0) && gport_array == NULL) {
        return BCM_E_PARAM;
    }

    mod_cfg.modid_type = bcmStkModidTypeMultiNextHops;
    BCM_IF_ERROR_RETURN(bcm_esw_stk_modid_config_get(unit, &mod_cfg));

    *count = 0;
    if (gport_size == 0) {
        for (mod_idx = mod_cfg.modid; mod_idx < mod_cfg.modid + mod_cfg.num_mods;
             mod_idx ++) {
             mod_idx %= 64;  /* lower 6 bit */
             SOC_IF_ERROR_RETURN
                (READ_MODID_BASE_PTRm(unit, MEM_BLOCK_ANY, mod_idx, &modid_base_entry));
             base_index = soc_MODID_BASE_PTRm_field32_get(unit,
                                       &modid_base_entry, MODID_BASEf);
             /* Search for unused index. */
             for (port_idx = base_index; port_idx < base_index + mod_cfg.num_ports;
                  port_idx++) {
                  SOC_IF_ERROR_RETURN
                      (READ_MODPORT_MAP_SUBPORTm(unit, MEM_BLOCK_ANY, port_idx,
                                                 &modport_map_subport_entry));
                  is_trunk = soc_MODPORT_MAP_SUBPORTm_field32_get(unit,
                                              &modport_map_subport_entry, ISTRUNKf);
                  /* Check if index is used. */
                  egress_tid = soc_MODPORT_MAP_SUBPORTm_field32_get(unit,
                                              &modport_map_subport_entry, DESTf);
                  if (is_trunk && (egress_tid == tid)) {/*  Entry found. */
                      (*count)++;
                  }
              }
        }
        return BCM_E_NONE;
    }

    _bcm_gport_dest_t_init(&dest);
    for (mod_idx = mod_cfg.modid; mod_idx < mod_cfg.modid + mod_cfg.num_mods;
         mod_idx ++) {
        mod_idx %= 64;  /* lower 6 bit */
        SOC_IF_ERROR_RETURN
            (READ_MODID_BASE_PTRm(unit, MEM_BLOCK_ANY, mod_idx, &modid_base_entry));
        base_index = soc_MODID_BASE_PTRm_field32_get(unit,
                                       &modid_base_entry, MODID_BASEf);
        /* Search for unused index. */
        for (port_idx = base_index; port_idx < base_index + mod_cfg.num_ports;
             port_idx++) {
            if ((*count) < gport_size) {
                SOC_IF_ERROR_RETURN
                    (READ_MODPORT_MAP_SUBPORTm(unit, MEM_BLOCK_ANY, port_idx,
                                               &modport_map_subport_entry));
                is_trunk = soc_MODPORT_MAP_SUBPORTm_field32_get(unit,
                                              &modport_map_subport_entry, ISTRUNKf);
                /* Check if index is used. */
                egress_tid = soc_MODPORT_MAP_SUBPORTm_field32_get(unit,
                                               &modport_map_subport_entry, DESTf);
                if (is_trunk && (egress_tid == tid)) {
                    /*  Entry found. */
                    dest.port = port_idx;
                    dest.modid = mod_idx;
                    dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                    rv = _bcm_esw_gport_construct(unit, &dest, &modport);
                    gport_array[(*count)++] = modport;
                }
            }
        }
     }

    return rv;

}

/*
 * Function    : _bcm_esw_trunk_gport_delete
 * Description : Delete the GPORT ID for the specified trunk.
 *
 * Parameters  : (IN)  unit          - BCM device number
 *                     (IN)  tid     - Trunk Group
 *                     (IN)  gport   - GPORT ID
 * Returns     : BCM_E_XXX
 */
int
_bcm_esw_trunk_gport_delete(int unit, bcm_trunk_t tid, bcm_gport_t gport)
{
    int id_out = 0;
    int base_index = 0;
    int is_trunk = 0;
    int result = FALSE;
    int subport_index = 0;
    bcm_port_t port_out = 0;
    bcm_module_t mod_out = 0;
    bcm_trunk_t tgid_out = 0;
    modid_base_ptr_entry_t modid_base_entry;
    modport_map_subport_entry_t modport_map_subport_entry;
    trunk_private_t *t_info;

    /* Make sure trunk module is initialized. */
    TRUNK_INIT(unit);

    /* Parameter check */
    if (!TRUNK_FABRIC_TID(unit, tid)) {
        return BCM_E_PARAM;
    }

    t_info = &TRUNK_INFO(unit, tid);
    if (t_info->tid == BCM_TRUNK_INVALID) {
        return BCM_E_NOT_FOUND;
    }
    tid -= TRUNK_CNTL(unit).ngroups_fp;

    if (!BCM_GPORT_IS_MODPORT(gport)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, gport,
                            &mod_out, &port_out, &tgid_out, &id_out));
    if ((-1 != tgid_out) || (-1 != id_out)) {
        return BCM_E_PARAM;
    }

    /* Check modport in one of additional module */
    BCM_IF_ERROR_RETURN (
        _bcm_esw_my_modid_additional_check(unit, mod_out, &result));
    if (result == FALSE) {
        return BCM_E_PARAM;
    }

    mod_out %= 64;   /* lower 6 bit */
    SOC_IF_ERROR_RETURN
            (READ_MODID_BASE_PTRm(unit, MEM_BLOCK_ANY, mod_out,
                                   &modid_base_entry));
    base_index = soc_MODID_BASE_PTRm_field32_get(unit,
                                   &modid_base_entry, MODID_BASEf);
    subport_index = base_index; /* the index is indicated by module base ptr */
    SOC_IF_ERROR_RETURN
        (READ_MODPORT_MAP_SUBPORTm(unit, MEM_BLOCK_ANY, subport_index,
                                   &modport_map_subport_entry));
    is_trunk = soc_MODPORT_MAP_SUBPORTm_field32_get(unit,
                                       &modport_map_subport_entry, ISTRUNKf);
    if (!is_trunk) {
        return BCM_E_NOT_FOUND;
    }
    soc_MODPORT_MAP_SUBPORTm_field32_set(unit,
                                   &modport_map_subport_entry, DESTf, 0);
    soc_MODPORT_MAP_SUBPORTm_field32_set (unit,
                                   &modport_map_subport_entry, ISTRUNKf, 0);
    soc_MODPORT_MAP_SUBPORTm_field32_set (unit,
                                 &modport_map_subport_entry, ENABLEf, 0);
    SOC_IF_ERROR_RETURN
        (WRITE_MODPORT_MAP_SUBPORTm(unit, MEM_BLOCK_ANY, subport_index,
                                        &modport_map_subport_entry));

    return BCM_E_NONE;
}

/*
 * Function    : _bcm_esw_trunk_gport_delete_all
 * Description : Delete the GPORT ID for the specified trunk.
 *
 * Parameters  : (IN)  unit          - BCM device number
 *                     (IN)  tid     - Trunk Group
 * Returns     : BCM_E_XXX
 */
int
_bcm_esw_trunk_gport_delete_all(int unit, bcm_trunk_t tid)
{
    int mod_idx = 0;
    int port_idx = 0;
    int is_trunk = 0;
    int base_index = 0;
    bcm_trunk_t egress_tid = 0;
    bcm_stk_modid_config_t mod_cfg;
    modid_base_ptr_entry_t modid_base_entry;
    modport_map_subport_entry_t modport_map_subport_entry;
    trunk_private_t *t_info;

    /* Make sure trunk module is initialized. */
    TRUNK_INIT(unit);

    /* Parameter check */
    if (!TRUNK_FABRIC_TID(unit, tid)) {
        return BCM_E_PARAM;
    }
    t_info = &TRUNK_INFO(unit, tid);
    if (t_info->tid == BCM_TRUNK_INVALID) {
        return BCM_E_NOT_FOUND;
    }
    tid -= TRUNK_CNTL(unit).ngroups_fp;

    mod_cfg.modid_type = bcmStkModidTypeMultiNextHops;
    BCM_IF_ERROR_RETURN(
        bcm_esw_stk_modid_config_get(unit, &mod_cfg));

    for (mod_idx = mod_cfg.modid; mod_idx < mod_cfg.modid + mod_cfg.num_mods;
         mod_idx ++) {
        mod_idx %= 64;   /* lower 6 bit */
        SOC_IF_ERROR_RETURN
            (READ_MODID_BASE_PTRm(unit, MEM_BLOCK_ANY, mod_idx, &modid_base_entry));
        base_index = soc_MODID_BASE_PTRm_field32_get(unit,
                                           &modid_base_entry, MODID_BASEf);
        /* Search for unused index. */
        for (port_idx = base_index; port_idx < base_index + mod_cfg.num_ports;
            port_idx++) {
            SOC_IF_ERROR_RETURN
                (READ_MODPORT_MAP_SUBPORTm(unit, MEM_BLOCK_ANY, port_idx,
                                           &modport_map_subport_entry));
            is_trunk = soc_MODPORT_MAP_SUBPORTm_field32_get(unit,
                                       &modport_map_subport_entry, ISTRUNKf);
            /* Check if index is used. */
            egress_tid = soc_MODPORT_MAP_SUBPORTm_field32_get(unit,
                                           &modport_map_subport_entry, DESTf);
            if (is_trunk && (egress_tid == tid)) {
                /*  Entry found. */
                soc_MODPORT_MAP_SUBPORTm_field32_set (unit,
                                           &modport_map_subport_entry, DESTf, 0);
                soc_MODPORT_MAP_SUBPORTm_field32_set (unit,
                                           &modport_map_subport_entry, ISTRUNKf, 0);
                soc_MODPORT_MAP_SUBPORTm_field32_set (unit,
                                           &modport_map_subport_entry, ENABLEf, 0);
                SOC_IF_ERROR_RETURN
                    (WRITE_MODPORT_MAP_SUBPORTm(unit, MEM_BLOCK_ANY, port_idx,
                                                &modport_map_subport_entry));
            }
        }
    }

    return BCM_E_NONE;
}

#endif /* BCM_TRIDENT2PLUS_SUPPORT */

/*
 * Function    : bcm_esw_trunk_gport_add
 * Description : Add the GPORT ID for the specified trunk.
 *
 * Parameters  : (IN)  unit          - BCM device number
 *                     (IN)  tid     - Trunk Group
 *                     (IN)  gport   - GPORT ID
 * Returns     : BCM_E_XXX
 */
int
bcm_esw_trunk_gport_add(int unit, bcm_trunk_t tid, bcm_gport_t gport)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_multi_next_hops_on_port)) {
        rv = _bcm_esw_trunk_gport_add(unit, tid, gport);
    }
#endif
    return rv;
}

/*
 * Function    : bcm_esw_trunk_gport_get_all
 * Description : Add the GPORT ID for the specified trunk.
 *
 * Parameters  : (IN)  unit               - BCM device number
 *                     (IN)  tid          - Trunk Group
 *                     (IN)  gport_size   - GPORT ID
                       (OUT) gport_array  - GPORT Array
                       (OUT) count        - GPORT Count
 * Returns     : BCM_E_XXX
 */
int
bcm_esw_trunk_gport_get_all(int unit, bcm_trunk_t tid, int gport_size,
                            bcm_gport_t *gport_array, int *count)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_multi_next_hops_on_port)) {
        if (gport_size < 0 || count == NULL) {
            return BCM_E_PARAM;
        }

        if ((gport_size > 0) && gport_array == NULL) {
            return BCM_E_PARAM;
        }

        rv = _bcm_esw_trunk_gport_get_all(unit, tid, gport_size, gport_array, count);
    }
#endif
    return rv;
}

/*
 * Function    : bcm_esw_trunk_gport_delete
 * Description : Delete the GPORT ID for the specified trunk.
 *
 * Parameters  : (IN)  unit          - BCM device number
 *                     (IN)  tid     - Trunk Group
 *                     (IN)  gport   - GPORT ID
 * Returns     : BCM_E_XXX
 */
int
bcm_esw_trunk_gport_delete(int unit, bcm_trunk_t tid, bcm_gport_t gport)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_multi_next_hops_on_port)) {
        rv = _bcm_esw_trunk_gport_delete(unit, tid, gport);
    }
#endif
    return rv;
}

/*
 * Function    : bcm_esw_trunk_gport_delete_all
 * Description : Delete the GPORT ID for the specified trunk.
 *
 * Parameters  : (IN)  unit          - BCM device number
 *                     (IN)  tid     - Trunk Group
 * Returns     : BCM_E_XXX
 */
int
bcm_esw_trunk_gport_delete_all(int unit, bcm_trunk_t tid)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_multi_next_hops_on_port)) {
        rv = _bcm_esw_trunk_gport_delete_all(unit, tid);
    }
#endif
    return rv;
}


/*
 * Function:
 *      _bcm_esw_trunk_local_members_get
 * Purpose:
 *      Get local member ports of a trunk group.
 * Parameters:
 *      unit       - (IN) Device Number
 *      trunk_id   - (IN) Trunk Identifier
 *      local_member_max   - (OUT) Size of local_member_array
 *      local_member_array - (OUT) Array of local trunk members 
 *      local_member_count - (OUT) Number of local trunk members
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If local_member_max = 0 and local_member_array == NULL,
 *      the number of local trunk members will still be returned in
 *      local_member_count. 
 */

int 
_bcm_esw_trunk_local_members_get(int unit, bcm_trunk_t trunk_id, 
                                 int local_member_max, 
                                 bcm_port_t *local_member_array, 
                                 int *local_member_count)
{
    int rv;
    int member_count;
    bcm_trunk_member_t *member_array;
    int i;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t tgid_out;
    int id_out;
    int modid_local;
    bcm_trunk_info_t trunk_info;
#if defined(BCM_KATANA2_SUPPORT)
    int local_subport = 0;
#endif

    if (local_member_max < 0) {
        return BCM_E_PARAM;
    }

    if ((local_member_max == 0) && (NULL != local_member_array)) {
        return BCM_E_PARAM;
    }

    if ((local_member_max > 0) && (NULL == local_member_array)) {
        return BCM_E_PARAM;
    }

    if (NULL == local_member_count) {
        return BCM_E_PARAM;
    }

    *local_member_count = 0;

    BCM_IF_ERROR_RETURN(_bcm_trunk_id_validate(unit, trunk_id));

    /* First, get the number of trunk members. */
    rv = bcm_esw_trunk_get(unit, trunk_id, &trunk_info, 0, NULL, &member_count);
    if (BCM_FAILURE(rv)) {
        return BCM_E_PORT;
    }

    if (0 == member_count) {
        return BCM_E_NONE;
    }

    member_array = sal_alloc(sizeof(bcm_trunk_member_t) * member_count,
            "trunk member array");
    if (member_array == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(member_array, 0, sizeof(bcm_trunk_member_t) * member_count);

    rv = bcm_esw_trunk_get(unit, trunk_id, &trunk_info, member_count,
            member_array, &member_count);
    if (BCM_FAILURE(rv)) {
        sal_free(member_array);
        return BCM_E_PORT;
    }

    for (i = 0; i < member_count; i++) {
        rv = _bcm_esw_gport_resolve(unit, member_array[i].gport,
                &mod_out, &port_out, &tgid_out, &id_out);
        if (BCM_FAILURE(rv)) {
            sal_free(member_array);
            return rv;
        }

#if defined(BCM_KATANA2_SUPPORT)
        /* Coverity fix for leaked_storage issue. Freeing member_array
        if the function returns with error */
        rv = _bcm_kt2_modport_is_local_coe_subport(unit,
                 mod_out, port_out, &local_subport);
        if (BCM_FAILURE(rv)) {
            sal_free(member_array);
            return rv;
        }
        if (local_subport) {
            if (NULL != local_member_array) {
                local_member_array[*local_member_count] =
                    BCM_GPORT_SUBPORT_PORT_GET(member_array[i].gport);
            }
            (*local_member_count)++;
            if (*local_member_count == local_member_max) {
                break;
            }
        }
#endif

#ifdef BCM_HGPROXY_COE_SUPPORT
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            _bcm_xgs5_subport_coe_mod_port_local(unit, mod_out, port_out)) {

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
            if (soc_feature(unit, soc_feature_channelized_switching)) {
                port_out = member_array[i].gport;
                rv = _bcm_coe_subtag_subport_port_get_by_modport(unit, port_out,
                         &port_out);
            } else
#endif
            {
                rv = _bcmi_coe_subport_mod_port_physical_port_get(
                     unit, mod_out, port_out, &port_out);
            }

            if (BCM_FAILURE(rv)) {
                sal_free(member_array);
                return rv;
            }
            modid_local = 1;
        } else
#endif
        {
            rv = _bcm_esw_modid_is_local(unit, mod_out, &modid_local);
            if (BCM_FAILURE(rv)) {
                sal_free(member_array);
                return rv;
            }
        }

        if (modid_local) {
            /* Convert system port to physical port */
            if (soc_feature(unit, soc_feature_sysport_remap)) {
                BCM_XLATE_SYSPORT_S2P(unit, &port_out);
            }

            if (NULL != local_member_array) {
                local_member_array[*local_member_count] = port_out;
            }
            (*local_member_count)++;
            if (*local_member_count == local_member_max) {
                break;
            }
        }
    }

    sal_free(member_array);

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define TRUNK_MIN_FABRIC_TID(unit) \
    (TRUNK_CNTL(unit).ngroups_hg ? TRUNK_CNTL(unit).ngroups_fp : -1)
#define TRUNK_MAX_FABRIC_TID(unit) \
    ( TRUNK_CNTL(unit).ngroups_hg ? \
     (TRUNK_CNTL(unit).ngroups_fp + TRUNK_CNTL(unit).ngroups_hg - 1): -1)

/*
 * _bcm_trunk_lag_reinit
 *
 * Restores sw state for front pannel trunks
 * Note: Application must register modid-port mapping function
 *       using bcm_stk_modmap_register before calling this
 */
STATIC int
_bcm_trunk_lag_reinit(int unit)
{
#if defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
        return (_bcm_trident_trunk_lag_reinit(unit, TRUNK_CNTL(unit).ngroups_fp,
                                   TRUNK_CNTL(unit).t_info));
    } else 
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {

        return (_bcm_th3_trunk_lag_reinit(unit, TRUNK_CNTL(unit).ngroups_fp,
                                   TRUNK_CNTL(unit).t_info));
    } else
#endif /*BCM_TOMAHAWK3_SUPPORT*/
#endif /* BCM_TRIDENT_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FBX(unit)) {
        return (_xgs3_trunk_reinit(unit, TRUNK_CNTL(unit).ngroups_fp,
                                   TRUNK_CNTL(unit).t_info));
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return BCM_E_NONE;
}

/*
 * _bcm_trunk_fabric_reinit
 *
 * Restores sw state for fabric trunks
 */
STATIC int
_bcm_trunk_fabric_reinit(int unit)
{
#if defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
        int min_tid = TRUNK_MIN_FABRIC_TID(unit);

        return (_bcm_trident_trunk_fabric_reinit(unit, 
                                          TRUNK_MIN_FABRIC_TID(unit),
                                          TRUNK_MAX_FABRIC_TID(unit),
                                          &TRUNK_INFO(unit,min_tid)));
    } else 
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FBX(unit) && !SOC_IS_RAPTOR(unit)) {
        int min_tid = TRUNK_MIN_FABRIC_TID(unit);
        if (SOC_IS_TOMAHAWKX(unit)) {
            if (soc_feature(unit, soc_feature_higig2)) {
                /* In TH, HG ports are not supported in low latency modes */
                return (_xgs3_trunk_fabric_reinit(unit, 
                                          TRUNK_MIN_FABRIC_TID(unit),
                                          TRUNK_MAX_FABRIC_TID(unit),
                                          &TRUNK_INFO(unit,min_tid)));
            }
        } else {
            return (_xgs3_trunk_fabric_reinit(unit, 
                                          TRUNK_MIN_FABRIC_TID(unit),
                                          TRUNK_MAX_FABRIC_TID(unit),
                                          &TRUNK_INFO(unit,min_tid)));
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return BCM_E_NONE;
}

/*
 * _bcm_trunk_reinit
 *
 * Recovers trunk state from hardware (Reload)
 */
int
_bcm_trunk_reinit(int unit)
{
    int                 rv;
    bcm_trunk_t         tid;
    trunk_private_t     *t_info;
    soc_scache_handle_t scache_handle;
    uint8               *trunk_scache;
    uint16              recovered_ver;
    uint32              ngroups;
    uint8               flags;
    int                 recovered_dlf_index;
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    bcm_trunk_t         hgtid;
    int                 tix, idx;
    int                 fp_hwf_recover = 1;
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
    uint32              additional_scache_size;
#if defined(BCM_SABER2_SUPPORT)
    uint32              fp_ngroups = 0;
    uint32              hg_ngroups = 0;
    uint16              default_ver = BCM_WB_DEFAULT_VERSION;
#endif /* BCM_SABER2_SUPPORT */
 	 
    /*
     * Trunk state initialization
     */
    t_info = TRUNK_CNTL(unit).t_info;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TRUNK, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &trunk_scache,  
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);

    if (BCM_E_NOT_FOUND == rv) {
        trunk_scache = NULL;
    } else if (BCM_FAILURE(rv)) {
        return rv;
    } else {
        /* Validate trunk info matches */
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit) &&
                (recovered_ver <= BCM_WB_VERSION_1_4) &&
                (default_ver > BCM_WB_VERSION_1_4)) {
            sal_memcpy(&fp_ngroups, trunk_scache, sizeof(ngroups));
            ngroups = SABER2_5626X_NGROUPS_FP;
        } else
#endif /* BCM_SABER2_SUPPORT */
        {
            sal_memcpy(&ngroups, trunk_scache, sizeof(ngroups));
        }
        if (ngroups != TRUNK_CNTL(unit).ngroups_fp) {
            return BCM_E_CONFIG;
        }
        trunk_scache += sizeof(ngroups);
        
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit) &&
                (recovered_ver <= BCM_WB_VERSION_1_4) &&
                (default_ver > BCM_WB_VERSION_1_4)) {
            sal_memcpy(&hg_ngroups, trunk_scache, sizeof(ngroups));
            ngroups = SABER2_5626X_NGROUPS_HG;
        } else
#endif /* BCM_SABER2_SUPPORT */
        {
            sal_memcpy(&ngroups, trunk_scache, sizeof(ngroups));
        }
        if (ngroups != TRUNK_CNTL(unit).ngroups_hg) {
            return BCM_E_CONFIG;
        }
        trunk_scache += sizeof(ngroups);

        sal_memset(TRUNK_CNTL(unit).trunk_bmp_ptr, 0,
                   TRUNK_CNTL(unit).trunk_bmp_alloc);
        sal_memcpy(TRUNK_CNTL(unit).trunk_bmp_ptr, trunk_scache,
                   TRUNK_CNTL(unit).trunk_bmp_alloc);

        for (tid = 0; tid < TRUNK_NUM_GROUPS(unit); tid++) {
            if (SHR_BITGET(TRUNK_CNTL(unit).trunk_bmp_ptr, tid)) {
                t_info->tid = tid;
            }
            t_info++;
        }
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit) &&
                (recovered_ver <= BCM_WB_VERSION_1_4) &&
                (default_ver > BCM_WB_VERSION_1_4)) {
            int tmp_trunk_bmp_alloc = SHR_BITALLOCSIZE(fp_ngroups + hg_ngroups);
            trunk_scache += tmp_trunk_bmp_alloc;
        } else
#endif /* BCM_SABER2_SUPPORT */
        {
            trunk_scache += TRUNK_CNTL(unit).trunk_bmp_alloc;
        }

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
#if defined(BCM_TRIDENT2_SUPPORT)
        /* WarmBoot recovery support for front panel ports hardware failover
         * configuration(for scorpion) is always present in SDK.
         * But hardware failover support for front panel ports in Trident2
         * is added when trunk scache is at WB version 1.3 by enabling the
         * corresponding soc_feature. A new version 1.4 is introducted to
         * recover front panel hardware failover configuration on Trident2.
         */
        if (SOC_IS_TRIDENT2(unit) && recovered_ver <= BCM_WB_VERSION_1_3) {
            fp_hwf_recover = 0;
        }
#endif /* BCM_TRIDENT2_SUPPORT */

        if ((TRUNK_CNTL(unit).fp_hwf_alloc) && (fp_hwf_recover)) {
            idx = 0;
            sal_memcpy(TRUNK_CNTL(unit).fp_hwf_flags, trunk_scache,
                       TRUNK_CNTL(unit).fp_hwf_alloc);
#if defined(BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit) &&
                    (recovered_ver <= BCM_WB_VERSION_1_4) &&
                    (default_ver > BCM_WB_VERSION_1_4)) {
                int tmp_fp_hwf_alloc = (fp_ngroups * 3 +
                        soc_mem_index_count(unit, SOURCE_TRUNK_MAP_TABLEm)) *
                    sizeof(uint8);
                trunk_scache += tmp_fp_hwf_alloc;
            } else
#endif /* BCM_SABER2_SUPPORT */
            {
                trunk_scache += TRUNK_CNTL(unit).fp_hwf_alloc;
            }

            if (soc_feature(unit, soc_feature_port_lag_failover)) {
                /* FP trunk failover flags */
                for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
                    TRUNK_INFO(unit, tid).flags =
                        TRUNK_CNTL(unit).fp_hwf_flags[idx++];
#ifdef BCM_TRIDENT_SUPPORT
                    if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
                        uint16 num_ports;
                        num_ports = TRUNK_CNTL(unit).fp_hwf_flags[idx++] << 8;
                        num_ports += TRUNK_CNTL(unit).fp_hwf_flags[idx++];
                        _bcm_trident_hw_failover_num_ports_set(unit, tid,
                                FALSE, num_ports);
                        for (tix = 0; tix < num_ports; tix++) {
                            BCM_IF_ERROR_RETURN
                                (_bcm_trident_hw_failover_flags_set(unit, tid,
                                    tix, FALSE, 
                                    TRUNK_CNTL(unit).fp_hwf_flags[idx++]));
                        }

                    } else
 #ifdef BCM_TOMAHAWK3_SUPPORT
                    if (SOC_IS_TOMAHAWK3(unit)) {
                        uint16 num_ports;
                        num_ports = TRUNK_CNTL(unit).fp_hwf_flags[idx++] << 8;
                        num_ports += TRUNK_CNTL(unit).fp_hwf_flags[idx++];
                        _bcm_th3_trunk_hwfailover_num_ports_set(unit, tid,
                                num_ports);
                        for (tix = 0; tix < num_ports; tix++) {
                                _bcm_th3_trunk_hwfailover_flags_set(unit, tid,
                                    tix, TRUNK_CNTL(unit).fp_hwf_flags[idx++]);
                        }

                    } else
#endif
#endif /* BCM_TRIDENT_SUPPORT */
                    {
                        for (tix = 0; tix < TRUNK_CNTL(unit).nports_fp; tix++) {
                            _bcm_xgs3_hw_failover_flags_set(unit, tid,
                                    tix, FALSE,
                                    TRUNK_CNTL(unit).fp_hwf_flags[idx++]);
                        }
                    }
                }
            }
        }

        if (TRUNK_CNTL(unit).hg_hwf_alloc) {
            idx = 0;

            sal_memcpy(TRUNK_CNTL(unit).hg_hwf_flags, trunk_scache,
                       TRUNK_CNTL(unit).hg_hwf_alloc);
#if defined(BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit) &&
                    (recovered_ver <= BCM_WB_VERSION_1_4) &&
                    (default_ver > BCM_WB_VERSION_1_4)) {
                int  tmp_hg_hwf_alloc = (hg_ngroups * (hg_ngroups + 1)) *
                    sizeof(uint8);
                trunk_scache += tmp_hg_hwf_alloc;
            } else
#endif /* BCM_SABER2_SUPPORT */
            {
                trunk_scache += TRUNK_CNTL(unit).hg_hwf_alloc;
            }

            if (soc_feature(unit, soc_feature_hg_trunk_failover)) {
                /* HG trunk failover flags */
                for (tid = TRUNK_CNTL(unit).ngroups_fp;
                     tid < TRUNK_NUM_GROUPS(unit); tid++) {
                    TRUNK_INFO(unit, tid).flags =
                        TRUNK_CNTL(unit).hg_hwf_flags[idx++];
                    hgtid = tid - TRUNK_CNTL(unit).ngroups_fp;
#ifdef BCM_TRIDENT_SUPPORT
                    if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
                        uint16 num_ports;
                        num_ports = TRUNK_CNTL(unit).hg_hwf_flags[idx++];
                        _bcm_trident_hw_failover_num_ports_set(unit, hgtid,
                                TRUE, num_ports);
                        for (tix = 0; tix < num_ports; tix++) {
                            _bcm_trident_hw_failover_flags_set(unit, hgtid,
                                    tix, TRUE, 
                                    TRUNK_CNTL(unit).hg_hwf_flags[idx++]);
                        }

                    } else
#endif /* BCM_TRIDENT_SUPPORT */
                    {
                        for (tix = 0; tix < TRUNK_CNTL(unit).nports_hg; tix++) {
                            _bcm_xgs3_hw_failover_flags_set(unit, hgtid,
                                    tix, TRUE,
                                    TRUNK_CNTL(unit).hg_hwf_flags[idx++]);
                        }
                    }
                }
            }
        }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

        additional_scache_size = 0;

        if (recovered_ver >= BCM_WB_VERSION_1_1) {
            /* Recover BCM_TRUNK_FLAG_IPMC_CLEAVE flag */
            for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
                sal_memcpy(&flags, trunk_scache, sizeof(uint8));
                trunk_scache += sizeof(uint8);
                if (flags & 0x1) {
                    t_info = &TRUNK_INFO(unit, tid);
                    t_info->flags |= BCM_TRUNK_FLAG_IPMC_CLEAVE;
                }
            }
#if defined(BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit) &&
                    (recovered_ver <= BCM_WB_VERSION_1_4) &&
                    (default_ver > BCM_WB_VERSION_1_4)) {
                int  tmp_ngroups_fp =
                    (fp_ngroups - TRUNK_CNTL(unit).ngroups_fp) *
                    sizeof(uint8);
                trunk_scache += tmp_ngroups_fp;
            }
#endif /* BCM_SABER2_SUPPORT */
            /* Recover ipmc_psc */ 
            for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
                t_info = &TRUNK_INFO(unit, tid);
                sal_memcpy(&t_info->ipmc_psc, trunk_scache, sizeof(int));
                trunk_scache += sizeof(int);
            }
#if defined(BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit) &&
                    (recovered_ver <= BCM_WB_VERSION_1_4) &&
                    (default_ver > BCM_WB_VERSION_1_4)) {
                int  tmp_ngroups_fp =
                    (fp_ngroups - TRUNK_CNTL(unit).ngroups_fp) *
                    sizeof(int);
                trunk_scache += tmp_ngroups_fp;
            }
#endif /* BCM_SABER2_SUPPORT */
        } else {
            additional_scache_size += (TRUNK_CNTL(unit).ngroups_fp *
                    sizeof(uint8));
            additional_scache_size += (TRUNK_CNTL(unit).ngroups_fp *
                    sizeof(int));
        }

        if (recovered_ver >= BCM_WB_VERSION_1_2) {
            /* Recover front panel trunk membership */
            for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
#if defined(BCM_TRIDENT_SUPPORT)
                if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
                    uint16 num_ports;
                    uint16 *modport;
                    uint32 *member_flags;
                    uint8 truncated_flags;
                    int i;

                    sal_memcpy(&num_ports, trunk_scache, sizeof(uint16));
                    trunk_scache += sizeof(uint16);

                    modport = sal_alloc(sizeof(uint16) * num_ports, "modport");
                    if (NULL == modport) {
                        return BCM_E_MEMORY;
                    }
                    member_flags = sal_alloc(sizeof(uint32) * num_ports,
                            "member_flags");
                    if (NULL == member_flags) {
                        sal_free(modport);
                        return BCM_E_MEMORY;
                    }

#if defined(BCM_SABER2_SUPPORT)
                    if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit) &&
                            (recovered_ver <= BCM_WB_VERSION_1_4) &&
                            (default_ver > BCM_WB_VERSION_1_4) &&
                            (num_ports > TRUNK_CNTL(unit).nports_fp)) {
                        LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit,
                                        "ERROR: Maximum port supported on "
                                        "5626x devices are %d\n"
                                        "Trunk id: %d has extra %d ports "
                                        "Please remove extra ports "
                                        "re-sync and update/warm boot\n"),
                                    TRUNK_CNTL(unit).nports_fp, tid,
                                    num_ports -
                                    TRUNK_CNTL(unit).nports_fp));
                        sal_free(modport);
                        sal_free(member_flags);
                        return BCM_E_CONFIG;
                    }
#endif /* BCM_SABER2_SUPPORT */
                    for (i = 0; i < num_ports; i++) {
                        sal_memcpy(&modport[i], trunk_scache, sizeof(uint16));
                        trunk_scache += sizeof(uint16);
                        sal_memcpy(&truncated_flags, trunk_scache,
                                sizeof(uint8));
                        trunk_scache += sizeof(uint8);
                        member_flags[i] = truncated_flags;
                    }
                    rv = _bcm_trident_trunk_member_info_set(unit, tid,
                            num_ports, modport, member_flags);
                    sal_free(modport);
                    sal_free(member_flags);
                    if (BCM_FAILURE(rv)) {
                        return rv;
                    }
                } else 
#if defined(BCM_TOMAHAWK3_SUPPORT)
                if (SOC_IS_TOMAHAWK3(unit)) {
                    uint16 num_ports;
                    uint8  *member_port;
                    uint32 *member_flags;
                    uint8 truncated_flags;
                    int i;

                    sal_memcpy(&num_ports, trunk_scache, sizeof(uint16));
                    trunk_scache += sizeof(uint16);
                    if (num_ports == 0) {
                        continue;
                    }
                    t_info = &TRUNK_INFO(unit, tid);
                    t_info->in_use = 1;

                    member_port = sal_alloc(sizeof(uint8) * num_ports, "port");
                    if (NULL == member_port) {
                        return BCM_E_MEMORY;
                    }
                    member_flags = sal_alloc(sizeof(uint32) * num_ports,
                            "member_flags");
                    if (NULL == member_flags) {
                        sal_free(member_port);
                        return BCM_E_MEMORY;
                    }

                    for (i = 0; i < num_ports; i++) {
                        sal_memcpy(&member_port[i], trunk_scache, sizeof(uint8));
                        trunk_scache += sizeof(uint8);
                        sal_memcpy(&truncated_flags, trunk_scache,
                                sizeof(uint8));
                        trunk_scache += sizeof(uint8);
                        member_flags[i] = truncated_flags;
                    }
                    rv = _bcm_th3_trunk_member_info_set(unit, tid,
                            num_ports, member_port, member_flags);
                    sal_free(member_port);
                    sal_free(member_flags);
                    if (BCM_FAILURE(rv)) {
                        return rv;
                    }
                } else 
#endif /* BCM_TOMAHAWK3_SUPPORT */
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_XGS3_SWITCH_SUPPORT)
                {
                    uint8 num_ports = 0;
                    uint16 *modport;
                    uint32 *member_flags;
                    uint8 truncated_flags;
                    int i;

                    sal_memcpy(&num_ports, trunk_scache, sizeof(uint8));
                    trunk_scache += sizeof(uint8);
                    modport = sal_alloc(sizeof(uint16) * num_ports, "modport");
                    if (NULL == modport) {
                        return BCM_E_MEMORY;
                    }
                    member_flags = sal_alloc(sizeof(uint32) * num_ports,
                            "member_flags");
                    if (NULL == member_flags) {
                        sal_free(modport);
                        return BCM_E_MEMORY;
                    }

                    sal_memset(modport, 0,
                               (sizeof(uint16) * num_ports));
                    sal_memset(member_flags, 0,
                               (sizeof(uint32) * num_ports));
                    for (i = 0; i < num_ports; i++) {
                        sal_memcpy(&modport[i], trunk_scache, sizeof(uint16));
                        trunk_scache += sizeof(uint16);
                        sal_memcpy(&truncated_flags, trunk_scache,
                                sizeof(uint8));
                        trunk_scache += sizeof(uint8);
                        member_flags[i] = truncated_flags;
                    }
                    _bcm_xgs3_trunk_member_info_set(unit, tid, num_ports,
                            modport, member_flags);
                    sal_free(modport);
                    sal_free(member_flags);
                }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
            }
#if defined(BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit) && soc_sb2_5626x_devtype(unit) &&
                    (recovered_ver <= BCM_WB_VERSION_1_4) &&
                    (default_ver > BCM_WB_VERSION_1_4)) {
                for (tid = TRUNK_CNTL(unit).ngroups_fp; tid < fp_ngroups;
                        tid++) {
                    if (soc_feature(unit,
                                soc_feature_shared_trunk_member_table)) {
                        uint16 num_ports;
                        sal_memcpy(&num_ports, trunk_scache, sizeof(uint8));
                        trunk_scache += sizeof(uint8);
                        if (num_ports > 0) {
                            LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit,
                                            "ERROR: Maximum trunk supported "
                                            "on 5626x devices are %d \n"
                                            "Please remove Trunk id %d "
                                            "re-sync and update/warm boot\n"),
                                        TRUNK_CNTL(unit).ngroups_fp - 1,
                                        tid));
                            return BCM_E_CONFIG;
                        }
                    }
                }
            }
#endif /* BCM_SABER2_SUPPORT */
        } else {
#if defined(BCM_TRIDENT_SUPPORT)
            if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
                additional_scache_size += (TRUNK_CNTL(unit).ngroups_fp * 2 +
                        soc_mem_index_count(unit, TRUNK_MEMBERm) * 3) *
                        sizeof(uint8);
            } else
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_XGS3_SWITCH_SUPPORT)
            {
                additional_scache_size += (TRUNK_CNTL(unit).ngroups_fp *
                        (1 + 3 * TRUNK_CNTL(unit).nports_fp) * sizeof(uint8));
            }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
        }

        if (recovered_ver >= BCM_WB_VERSION_1_3) {
#ifdef BCM_TRIDENT_SUPPORT 
            if (soc_feature(unit, soc_feature_hg_dlb) &&
                !soc_feature(unit, soc_feature_hgt_lag_dlb_optimized)) {
                BCM_IF_ERROR_RETURN(bcm_trident_hg_dlb_scache_recover(unit,
                            &trunk_scache));
            }
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT 
            if (soc_feature(unit, soc_feature_lag_dlb) &&
                !soc_feature(unit, soc_feature_hgt_lag_dlb_optimized)) {
                /* Recover LAG dynamic load balancing info */ 
                BCM_IF_ERROR_RETURN(bcm_tr3_lag_dlb_scache_recover(unit,
                            &trunk_scache));
            }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TOMAHAWK2_SUPPORT
            if (soc_feature(unit, soc_feature_hgt_lag_dlb_optimized)) {
                /* Recover HGT_LAG dynamic load balancing info */
                BCM_IF_ERROR_RETURN(bcm_th2_hgt_lag_dlb_scache_recover(unit,
                            &trunk_scache));
            }
#endif /* BCM_TOMAHAWK2_SUPPORT */

        } else {
#ifdef BCM_TRIDENT_SUPPORT 
            if (soc_feature(unit, soc_feature_hg_dlb)) {
                int hg_dlb_scache_size;
                BCM_IF_ERROR_RETURN(bcm_trident_hg_dlb_wb_alloc_size_get(unit,
                            &hg_dlb_scache_size));
                additional_scache_size += hg_dlb_scache_size;
            }
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT 
            if (soc_feature(unit, soc_feature_lag_dlb)) {
                int lag_dlb_scache_size;
                BCM_IF_ERROR_RETURN(bcm_tr3_lag_dlb_wb_alloc_size_get(unit,
                            &lag_dlb_scache_size));
                additional_scache_size += lag_dlb_scache_size;
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
        }

        if (recovered_ver <= BCM_WB_VERSION_1_3) {
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TRIDENT2(unit)) {
                additional_scache_size += TRUNK_CNTL(unit).fp_hwf_alloc;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
        }

#if defined(BCM_TRIDENT_SUPPORT)
        if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
            if (recovered_ver >= BCM_WB_VERSION_1_6) {
                /* Recover psc from scache in Level 2 warmboot */
                for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
                    t_info = &TRUNK_INFO(unit, tid);
                    sal_memcpy(&t_info->psc, trunk_scache, sizeof(int));
                    trunk_scache += sizeof(int);
                }
            } else {
                additional_scache_size += (TRUNK_CNTL(unit).ngroups_fp *
                    sizeof(int));
            }
        }
/*#if defined(BCM_TOMAHAWK3_SUPPORT) Will Revisit why this piece is a problem. */
#if 0
        else {
                for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
                    t_info = &TRUNK_INFO(unit, tid);
                    sal_memcpy(&t_info->psc, trunk_scache, sizeof(int));
                    trunk_scache += sizeof(int);
                }
        }
#endif

#endif /* BCM_TRIDENT_SUPPORT */
        if (recovered_ver >= BCM_WB_VERSION_1_7) {
            for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {

                t_info = &TRUNK_INFO(unit, tid);
                sal_memcpy(&recovered_dlf_index, trunk_scache,
                               sizeof(int));
                trunk_scache += sizeof(int);

                t_info->dlf_index_spec = recovered_dlf_index;
                t_info->dlf_index_used = recovered_dlf_index;
                t_info->mc_index_spec = recovered_dlf_index;
                t_info->mc_index_used = recovered_dlf_index;
                t_info->ipmc_index_spec = recovered_dlf_index;
                t_info->ipmc_index_used = recovered_dlf_index;
            }
        } else {
            additional_scache_size += (TRUNK_CNTL(unit).ngroups_fp *
                                             sizeof(int));
        }

#if defined(BCM_TRIDENT_SUPPORT)
        if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
            if (recovered_ver >= BCM_WB_VERSION_1_8) {
                sal_memcpy(&bcm_trident_trunk_mbr_zero_rsvd[unit],
                           trunk_scache,
                           sizeof(int));
            } else {
                additional_scache_size += (sizeof(int));
            }
        }
#endif /* BCM_TRIDENT_SUPPORT */

        if (additional_scache_size > 0) {
            SOC_IF_ERROR_RETURN
                (soc_scache_realloc(unit, scache_handle,
                                    additional_scache_size));
        }

        /* To add new state:
         * if (recovered_ver >= BCM_WB_VERSION_1_x) {
         *     Recover state for ver 1_x 
         * } else {
         *     Re-alloc scache for 'sync' operation for ver 1_x state. 
         * }
         */
    }

    if (SOC_IS_XGS12_FABRIC(unit)) {
#ifdef BCM_XGS12_FABRIC_SUPPORT
        bcm_pbmp_t tports;
        bcm_port_t port;
        bcm_trunk_info_t *t_data, td;        
        int member_count;
        uint32 val;
  	         
        for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_hg; tid++) {
            PBMP_HG_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(READ_ING_HGTRUNKr(unit, port, tid, &val));
                break;
            }
  	             
            SOC_PBMP_CLEAR(tports);
            SOC_PBMP_WORD_SET(tports, 0,
                              soc_reg_field_get(unit, ING_HGTRUNKr, val, BMAPf));
            SOC_PBMP_COUNT(tports, member_count);
            if (0 == member_count) {
                /*
                 * Nothing to update for this TID
                 */
                continue;
            }
  	             
            sal_memset(&td, 0, sizeof(bcm_trunk_info_t));
            t_data = &td;
  	             
            switch(soc_reg_field_get(unit, ING_HGTRUNKr, val, ALGORITHMf)) {
            case 0:
                t_data->psc = BCM_TRUNK_PSC_SRCDSTMAC;
  	                 
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_IPMACSAf)) {
                    t_data->psc |= BCM_TRUNK_PSC_IPMACSA;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_IPMACDAf)) {
                    t_data->psc |= BCM_TRUNK_PSC_IPMACDA;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_IPTYPEf)) {
                    t_data->psc |= BCM_TRUNK_PSC_IPTYPE;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_IPVIDf)) {
                    t_data->psc |= BCM_TRUNK_PSC_IPVID;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_IPSAf)) {
                    t_data->psc |= BCM_TRUNK_PSC_IPSA;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_IPDAf)) {
                    t_data->psc |= BCM_TRUNK_PSC_IPDA;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_L4SSf)) {
                    t_data->psc |= BCM_TRUNK_PSC_L4SS;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_L4DSf)) {
                    t_data->psc |= BCM_TRUNK_PSC_L4DS;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_MACDAf)) {
                    t_data->psc |= BCM_TRUNK_PSC_MACDA;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_MACSAf)) {
                    t_data->psc |= BCM_TRUNK_PSC_MACSA;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_TYPEf)) {
                    t_data->psc |= BCM_TRUNK_PSC_TYPE;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_VIDf)) {
                    t_data->psc |= BCM_TRUNK_PSC_VID;
                }
                break;
            case 1:
                t_data->psc = BCM_TRUNK_PSC_REDUNDANT;
                break;
            default:
                t_data->psc = -1;
                break;
            }
  	             
            t_data->dlf_index = -1;
            t_data->mc_index = -1;
            t_data->ipmc_index = -1;
  	             
            /*
             * Fill in t_info struct for this tid
             */
            t_info = &TRUNK_INFO(unit, tid);
  	             
            /*
             * Check number of ports in trunk group
             */
            if (member_count < 1 ||
                (TRUNK_FP_TID(unit, tid) &&
                 (member_count > TRUNK_CNTL(unit).nports_fp)) ||
                (TRUNK_FABRIC_TID(unit, tid) &&
                 (member_count > TRUNK_CNTL(unit).nports_hg))) {
                return (BCM_E_PARAM);
            }
  	             
            TRUNK_LOCK(unit);
            t_info->tid = tid;
            t_info->psc = t_data->psc;
            t_info->in_use = TRUE;
            TRUNK_UNLOCK(unit);
        }
#endif /* BCM_XGS12_FABRIC_SUPPORT */
    } else if (SOC_IS_XGS3_FABRIC(unit)) {
#ifdef BCM_XGS3_FABRIC_SUPPORT
        /*
         * recover software state for fabric trunks
         */
        BCM_IF_ERROR_RETURN(_bcm_trunk_fabric_reinit(unit));
#endif /* BCM_XGS3_FABRIC_SUPPORT */
    }

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (SOC_IS_FBX(unit)) { 
            /*
             * recover software state for fabric trunks
             */
            BCM_IF_ERROR_RETURN(_bcm_trunk_fabric_reinit(unit));

            /*
             * recover software state for ether trunks
             */
            BCM_IF_ERROR_RETURN(_bcm_trunk_lag_reinit(unit));
        }
    }
#endif

 #ifdef BCM_APACHE_SUPPORT
    if (soc_feature(unit, soc_feature_apache_round_robin_fp_lag)) {
        int tid;
        for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
            if (TRUNK_CNTL(unit).t_info[tid].rr_lag_select != -1) {
                SHR_BITSET(TRUNK_CNTL(unit).fp_rr_lag_bmp,
                           TRUNK_CNTL(unit).t_info[tid].rr_lag_select);
            }
        }
    }
#endif /* BCM_APACHE_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_trunk_sync
 * Purpose:
 *      Record Trunk module persisitent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_esw_trunk_sync(int unit)
{
    bcm_trunk_t tid;
    trunk_private_t *t_info;
    soc_scache_handle_t scache_handle;
    uint8               *trunk_scache;
    uint32              ngroups;
    uint8               flags;
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    bcm_trunk_t         hgtid;
    int                 tix, idx;
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

    if (SOC_IS_XGS12_FABRIC(unit)) {
        return BCM_E_NONE;
    }

    TRUNK_INIT(unit);

    /* Limited scache mode unsupported */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TRUNK, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0,
                                 &trunk_scache, BCM_WB_DEFAULT_VERSION, NULL));

    ngroups = TRUNK_CNTL(unit).ngroups_fp;
    sal_memcpy(trunk_scache, &ngroups, sizeof(ngroups));
    trunk_scache += sizeof(ngroups);
        
    ngroups = TRUNK_CNTL(unit).ngroups_hg;
    sal_memcpy(trunk_scache, &ngroups, sizeof(ngroups));
    if (ngroups != TRUNK_CNTL(unit).ngroups_hg) {
        return BCM_E_CONFIG;
    }
    trunk_scache += sizeof(ngroups);

    sal_memset(TRUNK_CNTL(unit).trunk_bmp_ptr, 0,
               TRUNK_CNTL(unit).trunk_bmp_alloc);

    for (tid = 0; tid < TRUNK_NUM_GROUPS(unit); tid++) {
        t_info = &TRUNK_INFO(unit, tid);
        if (BCM_TRUNK_INVALID != t_info->tid) {
            SHR_BITSET(TRUNK_CNTL(unit).trunk_bmp_ptr, tid);
        }
        t_info++;
    }
    sal_memcpy(trunk_scache, TRUNK_CNTL(unit).trunk_bmp_ptr,
               TRUNK_CNTL(unit).trunk_bmp_alloc);
    trunk_scache += TRUNK_CNTL(unit).trunk_bmp_alloc;

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    if (TRUNK_CNTL(unit).fp_hwf_alloc) {
        idx = 0;

        if (soc_feature(unit, soc_feature_port_lag_failover)) {
            /* FP trunk failover flags */
            for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
                TRUNK_CNTL(unit).fp_hwf_flags[idx++] =
                    TRUNK_INFO(unit, tid).flags;
#if defined(BCM_TRIDENT_SUPPORT)
                if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
                    uint16 num_ports; 
                    num_ports = _bcm_trident_hw_failover_num_ports_get(unit, tid, FALSE);
                    /* Max number of ports per front-panel trunk group is 256,
                     * requiring two bytes of storage. */
                    TRUNK_CNTL(unit).fp_hwf_flags[idx++] = num_ports >> 8;
                    TRUNK_CNTL(unit).fp_hwf_flags[idx++] = num_ports & 0xff;
                    for (tix = 0; tix < num_ports; tix++) {
                        TRUNK_CNTL(unit).fp_hwf_flags[idx++] = 
                            _bcm_trident_hw_failover_flags_get(unit, tid,
                                    tix, FALSE);
                    }
                } else
#if defined(BCM_TOMAHAWK3_SUPPORT)
                if (SOC_IS_TOMAHAWK3(unit)) {
                    int fail_to_ports, fpid;
                    fail_to_ports = _bcm_th3_trunk_hwfailover_num_ports_get(unit, tid);
                    TRUNK_CNTL(unit).fp_hwf_flags[idx++] = fail_to_ports >> 8;
                    TRUNK_CNTL(unit).fp_hwf_flags[idx++] = fail_to_ports & 0xff;
                    for (fpid = 0; fpid < fail_to_ports; fpid++) {
                        TRUNK_CNTL(unit).fp_hwf_flags[idx++] = 
                               _bcm_th3_trunk_hwfailover_flags_get(unit, tid, fpid);
                    }
                } else
#endif
#endif /* BCM_TRIDENT_SUPPORT */
                {
                    for (tix = 0; tix < TRUNK_CNTL(unit).nports_fp; tix++) {
                        TRUNK_CNTL(unit).fp_hwf_flags[idx++] = 
                            _bcm_xgs3_hw_failover_flags_get(unit, tid,
                                    tix, FALSE);
                    }
                }
            } /* tid */
        } /* lag failover */
        sal_memcpy(trunk_scache, TRUNK_CNTL(unit).fp_hwf_flags,
                   TRUNK_CNTL(unit).fp_hwf_alloc);
        trunk_scache += TRUNK_CNTL(unit).fp_hwf_alloc;
    }
    if (TRUNK_CNTL(unit).hg_hwf_alloc) {

        if (soc_feature(unit, soc_feature_hg_trunk_failover)) {
            idx = 0;
            /* HG trunk failover flags */
            for (tid = TRUNK_CNTL(unit).ngroups_fp;
                    tid < TRUNK_NUM_GROUPS(unit); tid++) {
                TRUNK_CNTL(unit).hg_hwf_flags[idx++] =
                    TRUNK_INFO(unit, tid).flags;
                hgtid = tid - TRUNK_CNTL(unit).ngroups_fp;
#if defined(BCM_TRIDENT_SUPPORT)
                if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
                    uint16 num_ports; 
                    num_ports = _bcm_trident_hw_failover_num_ports_get(unit, hgtid, TRUE);
                    /* Max number of ports per front-panel trunk group is 32,
                     * requiring one byte of storage. */
                    TRUNK_CNTL(unit).hg_hwf_flags[idx++] = num_ports;
                    for (tix = 0; tix < num_ports; tix++) {
                        TRUNK_CNTL(unit).hg_hwf_flags[idx++] = 
                            _bcm_trident_hw_failover_flags_get(unit, hgtid,
                                    tix, TRUE);
                    }
                } else
#endif /* BCM_TRIDENT_SUPPORT */
                {
                    for (tix = 0; tix < TRUNK_CNTL(unit).nports_hg; tix++) { 
                        TRUNK_CNTL(unit).hg_hwf_flags[idx++] =  
                            _bcm_xgs3_hw_failover_flags_get(unit, hgtid,
                                    tix, TRUE);
                    }
                }
            }
        }
        sal_memcpy(trunk_scache, TRUNK_CNTL(unit).hg_hwf_flags,
                   TRUNK_CNTL(unit).hg_hwf_alloc);
        trunk_scache += TRUNK_CNTL(unit).hg_hwf_alloc;
    }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

    /* Store BCM_TRUNK_FLAG_IPMC_CLEAVE flag */
    for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
        t_info = &TRUNK_INFO(unit, tid);
        if (t_info->flags & BCM_TRUNK_FLAG_IPMC_CLEAVE) {
            flags = 0x1;
        } else {
            flags = 0x0;
        }
        sal_memcpy(trunk_scache, &flags, sizeof(uint8));
        trunk_scache += sizeof(uint8);
    }

    /* Store ipmc_psc */
    for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
        t_info = &TRUNK_INFO(unit, tid);
        sal_memcpy(trunk_scache, &t_info->ipmc_psc, sizeof(int));
        trunk_scache += sizeof(int);
    }

    /* Store front-panel trunk membership info */
    for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
#if defined(BCM_TRIDENT_SUPPORT)
        if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
            uint16 num_ports;
            uint16 *modport;
            uint32 *member_flags;
            uint8  truncated_flags;
            int rv, i;

            BCM_IF_ERROR_RETURN(_bcm_trident_trunk_member_info_get(unit, tid,
                        0, NULL, NULL, &num_ports));
            modport = sal_alloc(sizeof(uint16) * num_ports, "modport");
            if (NULL == modport) {
                return BCM_E_MEMORY;
            }
            member_flags = sal_alloc(sizeof(uint32) * num_ports,
                    "member_flags");
            if (NULL == member_flags) {
                sal_free(modport);
                return BCM_E_MEMORY;
            }
            rv = _bcm_trident_trunk_member_info_get(unit, tid,
                        num_ports, modport, member_flags, &num_ports);
            if (BCM_FAILURE(rv)) {
                sal_free(modport);
                sal_free(member_flags);
                return rv;
            }

            sal_memcpy(trunk_scache, &num_ports, sizeof(uint16));
            trunk_scache += sizeof(uint16);
            for (i = 0; i < num_ports; i++) {
                sal_memcpy(trunk_scache, &modport[i], sizeof(uint16));
                trunk_scache += sizeof(uint16);

                /* Truncate the flags to 1 byte, as there are only 8 flags */
                truncated_flags = member_flags[i] & 0xff;
                sal_memcpy(trunk_scache, &truncated_flags, sizeof(uint8));
                trunk_scache += sizeof(uint8);
            }

            sal_free(modport);
            sal_free(member_flags);
        } else
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(unit)) {
            _esw_trunk_add_info_t *t_data;
            uint16 num_ports;
            uint8 port;
            uint8 mflags;
            int rv, i;

            t_info = &TRUNK_INFO(unit, tid);
            t_data = sal_alloc(sizeof(_esw_trunk_add_info_t), "trunk_add_info");
            if (t_data == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(t_data, 0, sizeof(_esw_trunk_add_info_t));

            /* first obtain the number of member ports */

            rv = _bcm_th3_trunk_member_info_get(unit, tid, t_data, t_info);
            if (BCM_FAILURE(rv)) {
	        sal_free(t_data);
                return rv;
            }
            num_ports = t_data->num_ports;
            sal_memcpy(trunk_scache, &num_ports, sizeof(uint16));
            trunk_scache += sizeof(uint16);
            if (t_data->num_ports != 0) {
                /* Then obtain the member ports info */
                rv = _bcm_th3_trunk_member_info_get(unit, tid, t_data, t_info);
                if (BCM_FAILURE(rv)) {
	            sal_free(t_data);
                    return rv;
                }
                for (i = 0; i < num_ports; i++) {
                    /* Module is 0 for TH3 */
                    port = t_data->tp[i];
                    sal_memcpy(trunk_scache, &port, sizeof(uint8));
                    trunk_scache += sizeof(uint8);
        
                    /* Truncate the flags to 1 byte, as there are only 8 flags */
                    mflags = t_data->member_flags[i];
                    sal_memcpy(trunk_scache, &mflags, sizeof(mflags));
                    trunk_scache += sizeof(uint8);
                }
	        sal_free(t_data->tp);
	        sal_free(t_data->tm);
	        sal_free(t_data->member_flags);
	        sal_free(t_data->dynamic_scaling_factor);
	        sal_free(t_data->dynamic_load_weight);
	        sal_free(t_data->dynamic_queue_size_weight);
            } /* if num_ports */
	    sal_free(t_data);

        } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_XGS3_SWITCH_SUPPORT
        {
            uint8  num_ports;
            uint16 modport[BCM_XGS3_TRUNK_MAX_PORTCNT];
            uint32 member_flags[BCM_XGS3_TRUNK_MAX_PORTCNT];
            uint8  truncated_flags;
            int i;

            _bcm_xgs3_trunk_member_info_get(unit, tid, &num_ports,
                    modport, member_flags);
            sal_memcpy(trunk_scache, &num_ports, sizeof(uint8));
            trunk_scache += sizeof(uint8);
            for (i = 0; i < num_ports; i++) {
                sal_memcpy(trunk_scache, &modport[i], sizeof(uint16));
                trunk_scache += sizeof(uint16);

                /* Truncate the flags to 1 byte, as there are only 4 flags */
                truncated_flags = member_flags[i] & 0xff;
                sal_memcpy(trunk_scache, &truncated_flags, sizeof(uint8));
                trunk_scache += sizeof(uint8);
            }
        }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    }
    /* Store Higig dynamic load balancing info */
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_hg_dlb) &&
        !soc_feature(unit, soc_feature_hgt_lag_dlb_optimized)) {
        BCM_IF_ERROR_RETURN(bcm_trident_hg_dlb_sync(unit, &trunk_scache));
    }
#endif /* BCM_TRIDENT_SUPPORT */

    /* Store LAG dynamic load balancing info */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_lag_dlb) &&
        !soc_feature(unit, soc_feature_hgt_lag_dlb_optimized)) {
        BCM_IF_ERROR_RETURN(bcm_tr3_lag_dlb_sync(unit, &trunk_scache));
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (soc_feature(unit, soc_feature_hgt_lag_dlb_optimized)) {
        BCM_IF_ERROR_RETURN(bcm_th2_hgt_lag_dlb_sync(unit, &trunk_scache));
    }
#endif /* BCM_TOMAHAWK2_SUPPORT */

    /* Store front-panel trunk psc info */
#if defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
        for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
            t_info = &TRUNK_INFO(unit, tid);
            sal_memcpy(trunk_scache, &t_info->psc, sizeof(int));
            trunk_scache += sizeof(int);
        }
    }
/*#if defined(BCM_TOMAHAWK3_SUPPORT) Will Revisit why this piece is a problem. */
#if 0
    else {
        for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
            t_info = &TRUNK_INFO(unit, tid);
            sal_memcpy(trunk_scache, &t_info->psc, sizeof(int));
            trunk_scache += sizeof(int);
        }
    }
#endif
#endif /* BCM_TRIDENT_SUPPORT */

    /* Store dlf_index */
    for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
        t_info = &TRUNK_INFO(unit, tid);
        sal_memcpy(trunk_scache, &t_info->dlf_index_used, sizeof(int));
        trunk_scache += sizeof(int);
    }

    /* Store bcm_trident_trunk_mbr_zero_rsvd info. */
#if defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
        sal_memcpy(trunk_scache, &bcm_trident_trunk_mbr_zero_rsvd[unit], sizeof(int));
        trunk_scache += sizeof(int);
    }
#endif /* BCM_TRIDENT_SUPPORT */

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
  	 
#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_trunk_sw_dump
 * Purpose:
 *     Displays trunk information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_trunk_sw_dump(int unit)
{
    trunk_cntl_t    *t_cntl;
    trunk_private_t *t_info;
    int             i;
    int             print_trunk_info;

    t_cntl = &TRUNK_CNTL(unit);
    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information TRUNK - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "  Front  panel trunk groups    : %d\n"), t_cntl->ngroups_fp));
    LOG_CLI((BSL_META_U(unit,
                        "  Front  panel trunk max ports : %d\n"), t_cntl->nports_fp));
    LOG_CLI((BSL_META_U(unit,
                        "  Fabric panel trunk groups    : %d\n"), t_cntl->ngroups_hg));
    LOG_CLI((BSL_META_U(unit,
                        "  Fabric panel trunk max ports : %d\n"), t_cntl->nports_hg));

    print_trunk_info = TRUE;
    for (i = 0; i < TRUNK_NUM_GROUPS(unit); i++) {
        t_info = &TRUNK_INFO(unit, i);
#if defined(BCM_TRIDENT_SUPPORT)
        if (TRUNK_CNTL(unit).ngroups_fp > 128) {
            /* Trident has 1024 front panel trunk groups.
             * Print only the ones that are in use. 
             */
            print_trunk_info = t_info->tid != BCM_TRUNK_INVALID;
        } 
#endif /* BCM_TRIDENT_SUPPORT */
        if (print_trunk_info) {
            LOG_CLI((BSL_META_U(unit,
                                "  Trunk %d\n"), i));
            LOG_CLI((BSL_META_U(unit,
                                "      ID              : %d\n"), t_info->tid));
            LOG_CLI((BSL_META_U(unit,
                                "      in use          : %d\n"), t_info->in_use));
            LOG_CLI((BSL_META_U(unit,
                                "      psc             : 0x%x\n"), t_info->psc));
            LOG_CLI((BSL_META_U(unit,
                                "      ipmc_psc        : 0x%x\n"), t_info->ipmc_psc));
            LOG_CLI((BSL_META_U(unit,
                                "      rtag            : 0x%x\n"), t_info->rtag));
            LOG_CLI((BSL_META_U(unit,
                                "      flags           : 0x%x\n"), t_info->flags));
            if (soc_feature(unit, soc_feature_hg_dlb) ||
                soc_feature(unit, soc_feature_lag_dlb)) {
                LOG_CLI((BSL_META_U(unit,
                                    "      dynamic_size    : 0x%x\n"), t_info->dynamic_size));
                LOG_CLI((BSL_META_U(unit,
                                    "      dynamic_age     : 0x%x\n"), t_info->dynamic_age));
            }
            if (!SOC_IS_XGS3_SWITCH(unit)) {
                /* Unused for XGS3 devices */
                LOG_CLI((BSL_META_U(unit,
                                    "      dlf index spec  : %d\n"),
                         t_info->dlf_index_spec));
                LOG_CLI((BSL_META_U(unit,
                                    "      dlf index used  : %d\n"),
                         t_info->dlf_index_used));
                LOG_CLI((BSL_META_U(unit,
                                    "      dlf port used   : %d\n"), 
                         t_info->dlf_port_used));
                LOG_CLI((BSL_META_U(unit,
                                    "      mc index spec   : %d\n"),
                         t_info->mc_index_spec));
                LOG_CLI((BSL_META_U(unit,
                                    "      mc index used   : %d\n"),
                         t_info->mc_index_used));
                LOG_CLI((BSL_META_U(unit,
                                    "      mc port used    : %d\n"),
                         t_info->mc_port_used));
                LOG_CLI((BSL_META_U(unit,
                                    "      ipmc index spec : %d\n"),
                         t_info->ipmc_index_spec));
                LOG_CLI((BSL_META_U(unit,
                                    "      ipmc index used : %d\n"),
                         t_info->ipmc_index_used));
                LOG_CLI((BSL_META_U(unit,
                                    "      ipmc port used  : %d\n"),
                         t_info->ipmc_port_used));
            }
        }
    }

#if defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
        _bcm_trident_trunk_sw_dump(unit);
    } else 
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_XGS3_SWITCH_SUPPORT)
        if (SOC_IS_FBX(unit) && !SOC_IS_RAPTOR(unit)) {
            _bcm_xgs3_trunk_sw_dump(unit);
        }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vp_lag)) {
        bcm_td2_vp_lag_sw_dump(unit);
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/*
 * Function:
 *     _bcm_esw_trunk_tgid_extended_mem_get
 * Purpose:
 *     Called by the macro TRUNK_CHK_TGID_EXTENDED to fetch the memory
 *     that will be used to determine the number of trunk group IDs
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     The memory used to determine the number of trunk group IDs
 */
soc_mem_t
_bcm_esw_trunk_tgid_extended_mem_get(int unit)
{
    soc_mem_t ret = TRUNK_GROUPm;

    if (soc_feature(unit, soc_feature_fastlag)) {
        ret = FAST_TRUNK_GROUPm;

#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            ret = FAST_TRUNK_SIZEm;
        }
#endif
    }

    return ret;
}

/*
 * Function:
 *      _bcm_esw_trunk_lock
 * Purpose:
 *     Lock Trunk module - if module was not initialized NOOP
 *    
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_trunk_lock(int unit)
{
    if (NULL != TRUNK_CNTL(unit).lock) {
        return sal_mutex_take(TRUNK_CNTL(unit).lock, 
                              sal_mutex_FOREVER);
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *      _bcm_esw_trunk_unlock
 * Purpose:
 *     Unlock Trunk module - if module was not initialized NOOP
 *    
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_trunk_unlock(int unit)
{
    if (NULL != TRUNK_CNTL(unit).lock) {
        return sal_mutex_give(TRUNK_CNTL(unit).lock);
    }
    return (BCM_E_NONE);
}

int
_bcm_esw_trunk_group_num(int unit)
{

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vp_lag)) {
            return TRUNK_NUM_GROUPS(unit) + _bcm_esw_num_vp_trunk_groups(unit);
        }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

        return TRUNK_NUM_GROUPS(unit);
}

trunk_private_t *
_bcm_esw_trunk_info_get(int unit, int tid)
{
    return &TRUNK_INFO(unit, tid);
}

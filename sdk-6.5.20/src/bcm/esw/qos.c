/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * QoS module
 */
#include <sal/core/libc.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/l2u.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/l2.h>
#include <bcm/l3.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/ipmc.h>
#include <bcm/qos.h>
#include <bcm/stack.h>
#include <bcm/topo.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/qos.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/xgs3.h>
#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
#include <bcm_int/esw/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_GREYHOUND_SUPPORT)
#include <bcm_int/esw/greyhound.h>
#endif /* BCM_GREYHOUND_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <bcm_int/esw/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
#include <bcm_int/esw/katana2.h>
#endif

/* Initialize the QoS module. */
int 
bcm_esw_qos_init(int unit)
{
    int rv = BCM_E_NONE;

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TD2_TT2(unit) || SOC_IS_KATANA2(unit)) {
        port_tab_entry_t pent;
        bcm_port_t port;
        bcm_pbmp_t all_pbmp;
        uint32 dscp_index = 0;

        if (!SOC_WARM_BOOT(unit)) {
            uint32 dscp_map_entries[SOC_MAX_MEM_FIELD_WORDS];
            int  count = 0, i = 0;
            void *entries_buf[1];
            soc_mem_t mem;

            /* Create the default profile */
            sal_memset(dscp_map_entries, 0, sizeof(dscp_map_entries));

            entries_buf[0] = &dscp_map_entries;
            BCM_IF_ERROR_RETURN(
                _bcm_dscp_table_entry_add(unit, entries_buf, 64, &dscp_index));

            count = 0;
            mem = SOC_IS_TRIDENT3X(unit) ? LPORT_TABm : PORT_TABm;
            soc_mem_lock(unit, mem);

            BCM_PBMP_CLEAR(all_pbmp);
            BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit) &&
                soc_feature(unit, soc_feature_flex_port)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_kt2_flexio_pbmp_update(unit, &all_pbmp));
            }
            if (soc_feature(unit, soc_feature_linkphy_coe) ||
                soc_feature(unit, soc_feature_subtag_coe)) {
                _bcm_kt2_subport_pbmp_update(unit, &all_pbmp);
            }
#endif

            PBMP_ITER(all_pbmp, port) {
                if (IS_LB_PORT(unit, port) ||
                    IS_MACSEC_PORT(unit, port)) {
                    continue;
                }

                if (SOC_IS_TRIDENT3X(unit)) {
                    rv = _bcm_esw_port_tab_set(unit, port,
                            _BCM_CPU_TABS_BOTH, TRUST_DSCP_PTRf,
                            dscp_index / 64);
                    if (BCM_FAILURE(rv)) {
                        soc_mem_unlock(unit, mem);
                        return rv;
                    }
                } else {
                    /* Get profile index from port table. */
                    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &pent);

                    if (BCM_FAILURE(rv)) {
                        soc_mem_unlock(unit, mem);
                        return rv;
                    }
                    soc_mem_field32_set(unit, mem, &pent, TRUST_DSCP_PTRf,
                            dscp_index / 64);
                    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, port, &pent);

                    if (BCM_FAILURE(rv)) {
                        soc_mem_unlock(unit, mem);
                        return rv;
                    }
                }
                count++;
            }
            soc_mem_unlock(unit, mem);
            for (i = 1; i < count; i++) {
                 BCM_IF_ERROR_RETURN(
                     _bcm_dscp_table_entry_reference(unit, dscp_index, 0));
            }
        } /* !SOC_WARM_BOOT  */
#ifdef BCM_TOMAHAWK3_SUPPORT
        else if(SOC_IS_TOMAHAWK3(unit)) {
            int ref_count;

            /* In WarmBoot scenario update the profile table reference count */
            BCM_PBMP_CLEAR(all_pbmp);
            BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));

            PBMP_ITER(all_pbmp, port) {
                if (IS_LB_PORT(unit, port)) {
                    continue;
                }
                SOC_IF_ERROR_RETURN(
                    soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ALL, port, &pent));
                dscp_index =
                  soc_mem_field32_get(unit, PORT_TABm, &pent, TRUST_DSCP_PTRf);
                BCM_IF_ERROR_RETURN(
                    _bcm_dscp_table_entry_reference(unit, dscp_index * 64, 64));
            }

            /* Fetch the Default DSCP, 0, reference count.  If it is 0, increment
             * it */
            BCM_IF_ERROR_RETURN(_bcm_dscp_table_entry_ref_count_get(unit,
                                        0, &ref_count));
            if (ref_count == 0) {
                BCM_IF_ERROR_RETURN(
                    _bcm_dscp_table_entry_reference(unit, 0, 64));
            }
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */
    }
#endif

#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        rv = bcm_td2_qos_init(unit);
    }
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        rv = bcm_th_qos_init(unit);
    }
#endif
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) ||
        SOC_IS_TD_TT(unit) || SOC_IS_HURRICANEX(unit) ||
        SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit)||
        SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {

        rv = bcm_tr2_qos_init(unit);
    }
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)) {
        rv = bcm_tr_qos_init(unit);
    }
#endif
#if defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TD_TT(unit) || SOC_IS_KATANAX(unit) ||
        SOC_IS_TRIUMPH3(unit)) {
        soc_mem_t mem = EGR_MAP_MHm;

        if (soc_mem_is_valid(unit, mem)) {
            uint32 idx;
            uint8 *mbuf;
            egr_map_mh_entry_t *mapmh = NULL;
            uint32 min = soc_mem_index_min(unit, mem);
            uint32 max = soc_mem_index_max(unit, mem);
            int alloc_sz =
                sizeof(egr_map_mh_entry_t) * soc_mem_index_count(unit, mem);

            /* Program the table */
            mbuf = soc_cm_salloc(unit, alloc_sz, SOC_MEM_NAME(unit, mem));
            if (NULL == mbuf) {
                rv = BCM_E_MEMORY;
            }
            soc_mem_lock(unit, mem);
            if (BCM_SUCCESS(rv)) {
                sal_memset(mbuf, 0, alloc_sz);
            }
            if (BCM_SUCCESS(rv)) {
                for (idx = min; idx <= max; idx++) {
                    mapmh = soc_mem_table_idx_to_pointer(unit, mem,
                                             egr_map_mh_entry_t *, mbuf, idx);
                    soc_mem_field32_set(unit, mem, mapmh, HG_TCf, idx & 0xf);
                }
            }
            if (BCM_SUCCESS(rv)) {
                rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ANY, min, max, mbuf);
            }
            soc_mem_unlock(unit, mem);
            if (mbuf != NULL) {
                soc_cm_sfree(unit, mbuf);
            }
        }
    }
#endif

#if defined(BCM_GREYHOUND_SUPPORT)
    if (SOC_IS_GREYHOUND(unit)||SOC_IS_HURRICANE3(unit)||
        SOC_IS_GREYHOUND2(unit)) {
        rv = bcm_gh_qos_init(unit);
    }
#endif
#ifdef BCM_WARM_BOOT_SUPPORT
#if defined BCM_KATANA2_SUPPORT || defined BCM_TRIDENT2_SUPPORT
    if ((SOC_IS_KATANA2(unit)) || SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        rv = _bcm_notify_qos_reinit_to_fp (unit);
    }
#endif /* BCM_KATANA2_SUPPORT, BCM_TRIDENT2_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */


    return rv;
}

/* Detach the QoS module. */
int 
bcm_esw_qos_detach(int unit)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) ||
        SOC_IS_TD_TT(unit) || SOC_IS_HURRICANEX(unit) ||
        SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit)||
        SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
        rv = bcm_tr2_qos_detach(unit);
    }
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)) {
        rv = bcm_tr_qos_detach(unit);
    }
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT3X(unit)) {
        rv = bcm_td2_qos_detach(unit);
    }
#endif

#if defined(BCM_GREYHOUND_SUPPORT)
    if (SOC_IS_GREYHOUND(unit)||SOC_IS_HURRICANE3(unit)||
        SOC_IS_GREYHOUND2(unit)) {
        rv = bcm_gh_qos_detach(unit);
    }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        rv = bcm_th_qos_detach(unit);
    }
#endif


    return rv;
}

/* Create a QoS map profile */
int 
bcm_esw_qos_map_create(int unit, uint32 flags, int *map_id)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_HURRICANE_SUPPORT)
    /* For HurricaneX/Greyhound, there is no support for MPLS and L3 */
    if ((SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit)) &&
        ((flags & BCM_QOS_MAP_MPLS) || (flags & BCM_QOS_MAP_L3))) {
        return rv;
    }
#endif /* BCM_HURRICANE_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
    /*
     * For Greyhound2, there is no support for MPLS.
     * For L3, it only supports egress DSCP mapping for VXLAN tunnel support.
     */
    if (SOC_IS_GREYHOUND2(unit)) {
        if (flags & BCM_QOS_MAP_MPLS) {
            return rv;
        }
        /* GH2-B0 support ingress DSCP mapping for VXLAN-RIOT */
        if (!soc_feature(unit, soc_feature_vxlan_lite_riot)) {
            if ((flags & BCM_QOS_MAP_L3) && (flags & BCM_QOS_MAP_INGRESS)) {
                return rv;
            }
        }
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
    if ((QOS_FLAGS_ARE_FCOE(flags) || (flags & BCM_QOS_MAP_L2_VLAN_ETAG)) 
        && (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit))) {
        return bcm_td2_qos_map_create(unit, flags, map_id);
    }
#endif

#ifdef BCM_GREYHOUND_SUPPORT
    if ((flags & BCM_QOS_MAP_L2_VLAN_ETAG) 
        && (SOC_IS_GREYHOUND(unit)||SOC_IS_HURRICANE3(unit)||
            SOC_IS_GREYHOUND2(unit))) {
        return bcm_gh_qos_map_create(unit, flags, map_id);
    }
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
    if ((flags & BCM_QOS_MAP_L2_VLAN_ETAG) &&
        SOC_IS_TOMAHAWKX(unit)) {
        if (SOC_IS_TOMAHAWK3(unit)) {
            /* TH3 does not support L2 VLAN ETAG */
            return rv;
        } else {
            return bcm_th_qos_map_create(unit, flags, map_id);
        }
    }
#endif

#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) ||
        SOC_IS_TD_TT(unit) || SOC_IS_HURRICANEX(unit) ||
        SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit)||
        SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
        rv = bcm_tr2_qos_map_create(unit, flags, map_id);
    }
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)) {
        rv = bcm_tr_qos_map_create(unit, flags, map_id);
    }
#endif
    return rv;
}

/* Destroy a QoS map profile */
int 
bcm_esw_qos_map_destroy(int unit, int map_id)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT2_SUPPORT
    if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit))
         && (QOS_MAP_IS_FCOE(map_id) || QOS_MAP_IS_L2_VLAN_ETAG(map_id))) {
        return bcm_td2_qos_map_destroy(unit, map_id);
    }
#endif
#ifdef BCM_GREYHOUND_SUPPORT
    if ((SOC_IS_GREYHOUND(unit)||SOC_IS_HURRICANE3(unit) ||
        SOC_IS_GREYHOUND2(unit))&& 
        QOS_MAP_IS_L2_VLAN_ETAG(map_id)) {
        return bcm_gh_qos_map_destroy(unit, map_id);
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)&& QOS_MAP_IS_L2_VLAN_ETAG(map_id)) {
        if (SOC_IS_TOMAHAWK3(unit)) {
            /* TH3 does not support L2 VLAN ETAG */
            return rv;
        } else {
            return bcm_th_qos_map_destroy(unit, map_id);
        }
    }
#endif
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) ||
        SOC_IS_TD_TT(unit) || SOC_IS_HURRICANEX(unit) ||
        SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit)||
        SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
        rv = bcm_tr2_qos_map_destroy(unit, map_id);
    }
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)) {
        rv = bcm_tr_qos_map_destroy(unit, map_id);
    }
#endif
    return rv;
}

/* Add an entry to a QoS map */
int 
bcm_esw_qos_map_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT2_SUPPORT
    if ((SOC_IS_TRIDENT2X(unit)|| SOC_IS_TRIDENT3X(unit))&& 
        (QOS_MAP_IS_FCOE(map_id)|| QOS_MAP_IS_L2_VLAN_ETAG(map_id))) {
        return bcm_td2_qos_map_add(unit, flags, map, map_id);
    }
#endif
#ifdef BCM_GREYHOUND_SUPPORT
    if ((SOC_IS_GREYHOUND(unit)||SOC_IS_HURRICANE3(unit)||
        SOC_IS_GREYHOUND2(unit))&& 
        QOS_MAP_IS_L2_VLAN_ETAG(map_id)) {
        return bcm_gh_qos_map_add(unit, flags, map, map_id);
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)&& QOS_MAP_IS_L2_VLAN_ETAG(map_id)) {
        if (SOC_IS_TOMAHAWK3(unit)) {
            /* TH3 does not support L2 VLAN ETAG */
            return rv;
        } else {
            return bcm_th_qos_map_add(unit, flags, map, map_id);
        }
    }
#endif
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) ||
        SOC_IS_TD_TT(unit) || SOC_IS_HURRICANEX(unit) ||
        SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit)||
        SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
        rv = bcm_tr2_qos_map_add(unit, flags, map, map_id, 0);
    }
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)) {
        rv = bcm_tr_qos_map_add(unit, flags, map, map_id);
    }
#endif
    return rv;
}

/* bcm_esw_qos_map_multi_get */
int
bcm_esw_qos_map_multi_get(int unit, uint32 flags,
                          int map_id, int array_size, 
                          bcm_qos_map_t *array, int *array_count)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TRIDENT2_SUPPORT
    if ((SOC_IS_TRIDENT2X(unit)|| SOC_IS_TRIDENT3X(unit))
        && (QOS_MAP_IS_L2_VLAN_ETAG(map_id) ||
            QOS_MAP_IS_FCOE(map_id))) {
        return bcm_td2_qos_map_multi_get(unit, flags, map_id, array_size, array, 
                                       array_count);
    }
#endif

#ifdef BCM_GREYHOUND_SUPPORT
    if ((SOC_IS_GREYHOUND(unit)||SOC_IS_HURRICANE3(unit)||
        SOC_IS_GREYHOUND2(unit))&& 
        QOS_MAP_IS_L2_VLAN_ETAG(map_id)) {
        return bcm_gh_qos_map_multi_get(unit, flags, map_id, array_size, array, 
                                       array_count);
    }
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)&& QOS_MAP_IS_L2_VLAN_ETAG(map_id)) {
        if (SOC_IS_TOMAHAWK3(unit)) {
            /* TH3 does not support L2 VLAN ETAG */
            return rv;
        } else {
            return bcm_th_qos_map_multi_get(unit, flags, map_id,
                                    array_size, array, array_count);
        }
    }
#endif
    
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) ||
        SOC_IS_TD_TT(unit) || SOC_IS_HURRICANEX(unit) ||
        SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit)||
        SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
        rv = bcm_tr2_qos_map_multi_get(unit, flags, map_id, array_size, array, 
                                       array_count);
    }
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)) {
        rv = bcm_tr_qos_map_multi_get(unit, flags, map_id, array_size, array, 
                                       array_count);
    }
#endif
    return rv;
}

/* Clear an entry from a QoS map */
int 
bcm_esw_qos_map_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT2_SUPPORT
    if ((SOC_IS_TRIDENT2X(unit)|| SOC_IS_TRIDENT3X(unit))&& 
        (QOS_MAP_IS_FCOE(map_id) || QOS_MAP_IS_L2_VLAN_ETAG(map_id))) {
        return bcm_td2_qos_map_delete(unit, flags, map, map_id);
    }
#endif
#ifdef BCM_GREYHOUND_SUPPORT
    if ((SOC_IS_GREYHOUND(unit)||SOC_IS_HURRICANE3(unit)||
        SOC_IS_GREYHOUND2(unit))&& 
        QOS_MAP_IS_L2_VLAN_ETAG(map_id)) {
        return bcm_gh_qos_map_delete(unit, flags, map, map_id);
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)&& QOS_MAP_IS_L2_VLAN_ETAG(map_id)) {
        if (SOC_IS_TOMAHAWK3(unit)) {
            /* TH3 does not support L2 VLAN ETAG */
            return rv;
        } else {
            return bcm_th_qos_map_delete(unit, flags, map, map_id);
        }
    }
#endif
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) ||
        SOC_IS_TD_TT(unit) || SOC_IS_HURRICANEX(unit) ||
        SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit)||
        SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
        rv = bcm_tr2_qos_map_delete(unit, flags, map, map_id);
    }
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)) {
        rv = bcm_tr_qos_map_delete(unit, flags, map, map_id);
    }
#endif
    return rv;
}

/* Attach a QoS map to an object (Gport) */
int 
bcm_esw_qos_port_map_set(int unit, bcm_gport_t port, int ing_map, int egr_map)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT2_SUPPORT
    if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit))
        && (QOS_MAP_IS_FCOE(ing_map) || QOS_MAP_IS_L2_VLAN_ETAG(ing_map) 
        || QOS_MAP_IS_FCOE(egr_map))) {
        return bcm_td2_qos_port_map_set(unit, port, ing_map, egr_map);
    }
#endif
#ifdef BCM_GREYHOUND_SUPPORT
    if ((SOC_IS_GREYHOUND(unit)||SOC_IS_HURRICANE3(unit)||
        SOC_IS_GREYHOUND2(unit))
        && (QOS_MAP_IS_L2_VLAN_ETAG(ing_map) || QOS_MAP_IS_L2_VLAN_ETAG(egr_map))) {
        return bcm_gh_qos_port_map_set(unit, port, ing_map, egr_map);
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) &&
        (QOS_MAP_IS_L2_VLAN_ETAG(ing_map) ||
        QOS_MAP_IS_L2_VLAN_ETAG(egr_map))) {
        if (SOC_IS_TOMAHAWK3(unit)) {
            /* TH3 does not support L2 VLAN ETAG */
            return BCM_E_PARAM;
        } else {
            return bcm_th_qos_port_map_set(unit, port, ing_map, egr_map);
        }
    }
#endif
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) ||
        SOC_IS_TD_TT(unit) || SOC_IS_HURRICANEX(unit) ||
        SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit)||
        SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {

        rv = bcm_tr2_qos_port_map_set(unit, port, ing_map, egr_map);
    }
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)) {
        rv = bcm_tr_qos_port_map_set(unit, port, ing_map, egr_map);
    }
#endif
    return rv;
}


/* bcm_qos_port_map_type_get*/
int
bcm_esw_qos_port_map_type_get(int unit, bcm_gport_t port, uint32 flags,
                          int *map_id)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TRIDENT2_SUPPORT
    if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit))
        && (QOS_FLAGS_ARE_FCOE(flags) || (flags & BCM_QOS_MAP_L2_VLAN_ETAG))) {
       
        return bcm_td2_qos_port_map_type_get(unit, port, flags, map_id);
    }
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) &&
        (flags & BCM_QOS_MAP_L2_VLAN_ETAG)) {
        if (SOC_IS_TOMAHAWK3(unit)) {
            /* TH3 does not support L2 VLAN ETAG */
            return rv;
        } else {
            return bcm_th_qos_port_map_type_get(unit, port, flags, map_id);
        }
    }
#endif

#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) ||
        SOC_IS_TD_TT(unit) || SOC_IS_HURRICANEX(unit) ||
        SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit)||
        SOC_IS_GREYHOUND(unit)) {

        rv = bcm_tr2_qos_port_map_type_get(unit, port, flags, map_id);
    }
#endif


    return rv;
}


/* bcm_qos_port_map_get */
int
bcm_esw_qos_port_map_get(int unit, bcm_gport_t port, 
                         int *ing_map, int *egr_map)
{
    return BCM_E_UNAVAIL;
}

/* bcm_qos_port_vlan_map_set */
int
bcm_esw_qos_port_vlan_map_set(int unit,  bcm_port_t port, bcm_vlan_t vid,
                              int ing_map, int egr_map)
{
    int rv = BCM_E_UNAVAIL;
    
#if defined(BCM_TRIUMPH2_SUPPORT)
    if ( SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TD_TT(unit) || \
         SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) || \
         SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND2(unit)) {
        rv = bcm_tr2_qos_port_vlan_map_set(unit, port, vid, ing_map, egr_map);
    }
#endif

    return rv;
}

/* bcm_qos_port_vlan_map_get */
int
bcm_esw_qos_port_vlan_map_get(int unit, bcm_port_t port, bcm_vlan_t vid,
                              int *ing_map, int *egr_map)
{
    int rv = BCM_E_UNAVAIL;
    
#if defined(BCM_TRIUMPH2_SUPPORT)
    if ( SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TD_TT(unit) || \
         SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) || \
         SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND2(unit)) {
        rv = bcm_tr2_qos_port_vlan_map_get(unit, port, vid, ing_map, egr_map);
    }
#endif
    return rv;

}

/* bcm_qos_multi_get */
int
bcm_esw_qos_multi_get(int unit, int array_size, int *map_ids_array, 
                      int *flags_array, int *array_count)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) ||
        SOC_IS_TD_TT(unit) || SOC_IS_HURRICANEX(unit) ||
        SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit)||
        SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
        rv = bcm_tr2_qos_multi_get(unit, array_size, map_ids_array, 
                                   flags_array, array_count);
    }
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)) {
        rv = bcm_tr_qos_multi_get(unit, array_size, map_ids_array, 
                                   flags_array, array_count);
    }
#endif
    return rv;
}

/* bcm_qos_vpn_map_set */
int bcm_esw_qos_vpn_map_set(int unit, bcm_vpn_t vpn, int ing_map, int egr_map)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_derive_phb_based_on_vfi)) {
        rv = bcm_td3_qos_vpn_map_set(unit, vpn, ing_map, egr_map);
    }
#endif
    return rv;
}

/* bcm_qos_vpn_map_type_get */
int bcm_esw_qos_vpn_map_type_get( int unit, bcm_vpn_t vpn, uint32 flags, int *map_id)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_derive_phb_based_on_vfi)) {
        rv = bcm_td3_qos_vpn_map_type_get(unit, vpn, flags, map_id);
    }
#endif
    return rv;

}

#ifdef BCM_WARM_BOOT_SUPPORT
int
_bcm_esw_qos_sync(int unit)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) ||
        SOC_IS_TD_TT(unit) || SOC_IS_HURRICANEX(unit) ||
        SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit)||
        SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
        rv = _bcm_tr2_qos_sync(unit);
    }
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)) {
        rv = _bcm_tr_qos_sync(unit);
    }
#endif
    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
void
_bcm_esw_qos_sw_dump(int unit)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) ||
        SOC_IS_TD_TT(unit) || SOC_IS_HURRICANEX(unit) ||
        SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit)||
        SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
        _bcm_tr2_qos_sw_dump(unit);
    }
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)) {
        _bcm_tr_qos_sw_dump(unit);
    }
#endif
#if defined (BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit)) {
        _bcm_td2_qos_sw_dump(unit);
    }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        _bcm_th_qos_sw_dump(unit);
    }
#endif
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

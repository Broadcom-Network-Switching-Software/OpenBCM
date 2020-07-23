/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    multicast.c
 * Purpose: Manages multicast functions
 */
#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <sal/core/time.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/util.h>
#if defined(BCM_BRADLEY_SUPPORT)
#include <soc/bradley.h>
#endif /* BCM_BRADLEY_SUPPORT */
#include <bcm/l2.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/multicast.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/ipmc.h>
#include <bcm_int/esw/switch.h>
#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT)
#include <bcm_int/esw/trident.h>
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_KATANA_SUPPORT)
#include <bcm_int/esw/katana.h>
#include <soc/katana2.h>
#endif /* BCM_KATANA_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_HGPROXY_COE_SUPPORT)
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/subport.h>
#endif /* BCM_HGPROXY_COE_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/subport.h>
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <bcm_int/esw/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/esw/tomahawk3.h>
#include <soc/flexport/tomahawk3_mmu_flexport.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <bcm_int/esw/greyhound2.h>
#endif /* BCM_GREYHOUND2_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
#include <soc/esw/cancun.h>
#include <soc/esw/cancun_enums.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
#include <bcm_int/esw/firebolt6.h>
#include <soc/firebolt6.h>
#endif

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw/triumph3.h>
#include<bcm_int/esw/saber2.h> 

#define _BCM_MULTICAST_MTU_TABLE_OFFSET(unit) \
        ((SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit) || \
          SOC_IS_METROLITE(unit) || SOC_IS_GREYHOUND2(unit)) ? \
          (SOC_IS_FIRELIGHT(unit) ? 128 : 512) : \
          SOC_IS_FIREBOLT6(unit) ? 16384 : \
          SOC_IS_HURRICANE4(unit) ? 2048 : \
          SOC_IS_TRIDENT2PLUS(unit) ? 12288 : SOC_IS_MONTEREY(unit) ? 2048 : \
          (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) ? 16384 : \
          SOC_IS_APACHE(unit) ? 12288 : 8192)
#define _BCM_TR3_MULTICAST_MTU_TABLE_OFFSET(unit) (SOC_IS_HELIX4(unit) ? 12288 : 16384)

#define BCM_FB6_PBMP_PORT_MAX           512

#define BCM_PBMP_NUM_PORTS(unit) \
            ((soc_feature(unit, soc_feature_channelized_switching)) ? \
                BCM_FB6_PBMP_PORT_MAX :  BCM_PBMP_PORT_MAX)

#ifdef BCM_WARM_BOOT_SUPPORT
static int multicast_mode_set[BCM_MAX_NUM_UNITS];
#endif /* BCM_WARM_BOOT_SUPPORT */

#define _BCM_MULTICAST_L2_OR_FABRIC(_unit_, _group_) \
        (_BCM_MULTICAST_IS_L2(_group_) || SOC_IS_XGS3_FABRIC(_unit_))

#ifdef	BCM_XGS3_FABRIC_SUPPORT
typedef struct _bcm_fabric_multicast_info_s {
    int         mcast_size;
    int         ipmc_size;
    uint32      *mcast_used;
} _bcm_fabric_multicast_info_t;

static _bcm_fabric_multicast_info_t _bcm_fabric_mc_info[BCM_MAX_NUM_UNITS];
uint8 *_fabric_multicast_ipmc_group_types[BCM_MAX_NUM_UNITS];

STATIC int
_bcm_esw_multicast_egress_get(int unit, bcm_multicast_t group, int port_max,
                              bcm_gport_t *port_array,
                              bcm_if_t *encap_id_array,
                              int *port_count);
STATIC int
_bcm_esw_multicast_egress_set(int unit, bcm_multicast_t group, int port_count,
                            bcm_gport_t *port_array, bcm_if_t *encap_id_array);

#ifdef BCM_WARM_BOOT_SUPPORT
/* Warm boot scache version to store
 * _fabric_multicast_ipmc_group_types array information.
 */
#define BCM_FABRIC_IPMC_WB_VERSION_1_0     SOC_SCACHE_VERSION(1,0)
#define BCM_FABRIC_IPMC_WB_DEFAULT_VERSION BCM_FABRIC_IPMC_WB_VERSION_1_0
#endif

#define BCM_FABRIC_MCAST_BASE(unit) (0)
#define BCM_FABRIC_MCAST_MAX(unit) (_bcm_fabric_mc_info[unit].mcast_size - 1)
#define BCM_FABRIC_IPMC_BASE(unit) (_bcm_fabric_mc_info[unit].mcast_size)
#define BCM_FABRIC_IPMC_MAX(unit) \
        (_bcm_fabric_mc_info[unit].mcast_size + \
         _bcm_fabric_mc_info[unit].ipmc_size - 1)
#define BCM_FABRIC_MC_USED_BITMAP(unit) \
        (_bcm_fabric_mc_info[unit].mcast_used)
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
/* Cache of MTU profile size regs */
STATIC soc_profile_reg_t *_bcm_mtu_profile[BCM_MAX_NUM_UNITS];
#endif


#if defined(BCM_KATANA_SUPPORT)
STATIC int
_bcm_esw_multicast_egress_subscriber_delete_all(int unit, bcm_multicast_t group);
extern int _bcm_kt_hw_to_sw_queue(int unit, int *queue_array, bcm_port_t *port);
#endif
STATIC int multicast_initialized[BCM_MAX_NUM_UNITS];
#define MULTICAST_INIT_CHECK(unit) \
    if (!multicast_initialized[unit]) { \
        return BCM_E_INIT; \
    }

int
_bcm_esw_multicast_flags_to_group_type(uint32 flags)
{
    int group_type = 0;

    if (flags & BCM_MULTICAST_TYPE_L2) {
        group_type = _BCM_MULTICAST_TYPE_L2;
    } else if (flags & BCM_MULTICAST_TYPE_L3){
        group_type = _BCM_MULTICAST_TYPE_L3;
    } else if (flags & BCM_MULTICAST_TYPE_VPLS){
        group_type = _BCM_MULTICAST_TYPE_VPLS;
    } else if (flags & BCM_MULTICAST_TYPE_SUBPORT){
        group_type = _BCM_MULTICAST_TYPE_SUBPORT;
    } else if (flags & BCM_MULTICAST_TYPE_MIM){
        group_type = _BCM_MULTICAST_TYPE_MIM;
    } else if (flags & BCM_MULTICAST_TYPE_WLAN){
        group_type = _BCM_MULTICAST_TYPE_WLAN;
    } else if (flags & BCM_MULTICAST_TYPE_VLAN) { 
        group_type = _BCM_MULTICAST_TYPE_VLAN;
    } else if (flags & BCM_MULTICAST_TYPE_TRILL) {
        group_type = _BCM_MULTICAST_TYPE_TRILL;
    } else if (flags & BCM_MULTICAST_TYPE_NIV) {
        group_type = _BCM_MULTICAST_TYPE_NIV;
    } else if (flags & BCM_MULTICAST_TYPE_EGRESS_OBJECT) {
        group_type = _BCM_MULTICAST_TYPE_EGRESS_OBJECT;
    } else if (flags & BCM_MULTICAST_TYPE_L2GRE) {
        group_type = _BCM_MULTICAST_TYPE_L2GRE;
    } else if (flags & BCM_MULTICAST_TYPE_VXLAN) {
        group_type = _BCM_MULTICAST_TYPE_VXLAN;
    } else if (flags & BCM_MULTICAST_TYPE_FLOW) {
        group_type = _BCM_MULTICAST_TYPE_FLOW;
    } else if (flags & BCM_MULTICAST_TYPE_EXTENDER) {
        group_type = _BCM_MULTICAST_TYPE_EXTENDER;
    }/* Else default to 0 above */

    return group_type;
}

uint32
_bcm_esw_multicast_group_type_to_flags(int group_type)
{
    uint32 flags = 0;

    switch (group_type) {
    case _BCM_MULTICAST_TYPE_L2:
        flags = BCM_MULTICAST_TYPE_L2;
        break;
    case _BCM_MULTICAST_TYPE_L3:
        flags = BCM_MULTICAST_TYPE_L3;
        break;
    case _BCM_MULTICAST_TYPE_VPLS:
        flags = BCM_MULTICAST_TYPE_VPLS;
        break;
    case _BCM_MULTICAST_TYPE_SUBPORT:
        flags = BCM_MULTICAST_TYPE_SUBPORT;
        break;
    case _BCM_MULTICAST_TYPE_MIM:
        flags = BCM_MULTICAST_TYPE_MIM;
        break;
    case _BCM_MULTICAST_TYPE_WLAN:
        flags = BCM_MULTICAST_TYPE_WLAN;
        break;
    case _BCM_MULTICAST_TYPE_VLAN:
        flags = BCM_MULTICAST_TYPE_VLAN;
        break;
    case _BCM_MULTICAST_TYPE_TRILL:
        flags = BCM_MULTICAST_TYPE_TRILL;
        break;
    case _BCM_MULTICAST_TYPE_NIV:
        flags = BCM_MULTICAST_TYPE_NIV;
        break;
    case _BCM_MULTICAST_TYPE_EGRESS_OBJECT:
        flags = BCM_MULTICAST_TYPE_EGRESS_OBJECT;
        break;
    case _BCM_MULTICAST_TYPE_L2GRE:
        flags = BCM_MULTICAST_TYPE_L2GRE;
        break;
    case _BCM_MULTICAST_TYPE_VXLAN:
        flags = BCM_MULTICAST_TYPE_VXLAN;
        break;
    case _BCM_MULTICAST_TYPE_FLOW:
        flags = BCM_MULTICAST_TYPE_FLOW;
        break;
    case _BCM_MULTICAST_TYPE_EXTENDER:
        flags = BCM_MULTICAST_TYPE_EXTENDER;
        break;

    default:
        /* flags defaults to 0 above */
        break;
    }

    return flags;
}

#ifdef BCM_XGS3_FABRIC_SUPPORT
/*
 * Function:
 *      _bcm_esw_fabric_multicast_param_check
 * Purpose:
 *      Validate fabric multicast parameters and adjust HW index.
 * Parameters:
 *      unit       - (IN)   Device Number
 *      group      - (IN) Multicast group ID.
 *      mc_id      - (IN/OUT)   multicast index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_fabric_multicast_param_check(int unit, bcm_multicast_t group,
                                      int *mc_index)
{
    if (_BCM_MULTICAST_IS_L2(group)) {
        /* L2 multicast groups start at index 1 */
        if ((*mc_index < BCM_FABRIC_MCAST_BASE(unit)) ||
            (*mc_index > BCM_FABRIC_MCAST_MAX(unit))) {
            return BCM_E_PARAM;
        }
    } else if (_BCM_MULTICAST_IS_SET(group)) {
        /* Non-L2 groups are offset */
        *mc_index += BCM_FABRIC_IPMC_BASE(unit);
        if ((*mc_index < BCM_FABRIC_IPMC_BASE(unit)) ||
            (*mc_index > BCM_FABRIC_IPMC_MAX(unit))) {
            return BCM_E_PARAM;
        }
    } else {
        return BCM_E_PARAM;
    }
    if (!SHR_BITGET(BCM_FABRIC_MC_USED_BITMAP(unit), *mc_index)) {
        return BCM_E_NOT_FOUND;
    }
    return BCM_E_NONE;
}
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

/*
 * Function:
 *      _bcm_esw_multicast_ipmc_read
 * Purpose:
 *      Read L3 multicast distribuition ports.
 * Parameters:
 *      unit       - (IN)   Device Number
 *      ipmc_id    - (IN)   IPMC index.
 *      l2_pbmp    - (IN)   L2 distribution ports.
 *      l3_pbmp    - (IN)   L3 distribuition port. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_multicast_ipmc_read(int unit, int ipmc_id, bcm_pbmp_t *l2_pbmp,
                             bcm_pbmp_t *l3_pbmp)
{
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];  /* hw entry bufffer. */

    /* Input parameters check. */
    if ((NULL == l2_pbmp) || (NULL == l3_pbmp)) {
        return (BCM_E_PARAM);
    }

    /* Table index sanity check. */
    if ((ipmc_id < soc_mem_index_min(unit, L3_IPMCm)) ||
        (ipmc_id > soc_mem_index_max(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

    /* Read L3 ipmc table. */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY,
                                     ipmc_id, entry));

    /* If entry is invalid - clear L2 & L3 bitmaps. */
    if (soc_mem_field32_get(unit, L3_IPMCm, entry, VALIDf) == 0) {
        BCM_PBMP_CLEAR(*l2_pbmp);
        BCM_PBMP_CLEAR(*l3_pbmp);
        return (BCM_E_NONE);
    }
    /* Extract L2 & L3 bitmaps. */
    soc_mem_pbmp_field_get(unit, L3_IPMCm, entry, L2_BITMAPf, l2_pbmp);
    soc_mem_pbmp_field_get(unit, L3_IPMCm, entry, L3_BITMAPf, l3_pbmp);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_multicast_ipmc_write
 * Purpose:
 *      Write L3 multicast distribuition ports.
 * Parameters:
 *      unit       - (IN)   Device Number
 *      ipmc_id    - (IN)   IPMC index.
 *      l2_pbmp    - (IN)   L2 distribution ports.
 *      l3_pbmp    - (IN)   L3 distribuition port. 
 *      valid      - (IN)   Distribuition is valid bit.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_multicast_ipmc_write(int unit, int ipmc_id, bcm_pbmp_t l2_pbmp,
                              bcm_pbmp_t l3_pbmp, int valid)
{
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];  /* hw entry bufffer. */
    int rv;
#if defined(BCM_KATANA_SUPPORT)
    int remap_id = 0;
#endif

    sal_memset(&entry, 0, sizeof(ipmc_entry_t));

    /* Currently there is no need to invalidate distribution. */
    if (0 == valid) {
        BCM_PBMP_CLEAR(l2_pbmp);
        BCM_PBMP_CLEAR(l3_pbmp);
    }

    /* Table index sanity check. */
    if ((ipmc_id < soc_mem_index_min(unit, L3_IPMCm)) ||
        (ipmc_id > soc_mem_index_max(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

    /* Read / Modify / Write section. */
    soc_mem_lock(unit, L3_IPMCm);
    rv = soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, ipmc_id, entry);
    if (BCM_SUCCESS(rv)) {
#if defined(BCM_KATANA_SUPPORT)
        if (SOC_IS_KATANAX(unit)) {
            remap_id = soc_mem_field32_get(unit, L3_IPMCm, entry,
                                    MMU_MC_REDIRECTION_PTRf);
            if (soc_mem_field32_get(unit, L3_IPMCm, entry, VALIDf) == 0 || !valid) {
                /* Invalid entry, flush */
                sal_memset(&entry, 0, sizeof(ipmc_entry_t)); 
                if (remap_id) {
                     soc_mem_field32_set(unit, L3_IPMCm, entry,
                                    MMU_MC_REDIRECTION_PTRf, remap_id);
                }
            } 
            soc_mem_field32_set(unit, L3_IPMCm, entry, VALIDf, valid);
            if ((0 == soc_mem_field32_get(unit, L3_IPMCm, entry,
                                          MMU_MC_REDIRECTION_PTRf))) {
                soc_mem_field32_set(unit, L3_IPMCm, entry,
                                    MMU_MC_REDIRECTION_PTRf, ipmc_id);
            }
        } else
#endif
        {
            if (soc_mem_field32_get(unit, L3_IPMCm, entry, VALIDf) == 0 || !valid) {
                /* Invalid entry, flush */
                sal_memset(&entry, 0, sizeof(ipmc_entry_t));
            }
            soc_mem_field32_set(unit, L3_IPMCm, entry, VALIDf, valid); 
        }
    }

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        bcm_port_t port;

        PBMP_ITER(l2_pbmp, port) {
            if (SOC_INFO(unit).port_num_subport[port] > 0) {
                BCM_PBMP_PORT_REMOVE(l2_pbmp, port);
            }
        }
    }
#endif

    soc_mem_pbmp_field_set(unit, L3_IPMCm, entry, L2_BITMAPf, &l2_pbmp);
    soc_mem_pbmp_field_set(unit, L3_IPMCm, entry, L3_BITMAPf, &l3_pbmp);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, L3_IPMCm, L2_L3f) &&
        BCM_PBMP_NOT_NULL(l3_pbmp)) {
        uint32 cancun_ver;

        BCM_IF_ERROR_RETURN(soc_cancun_version_get(unit, &cancun_ver));

        if (SOC_IS_TRIDENT3(unit) &&
            (cancun_ver < SOC_CANCUN_VERSION_DEF_5_3_2)) {
            soc_mem_field32_set(unit, L3_IPMCm, entry, L2_L3f, 1);
        }
    }
#endif

    rv = soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ALL, ipmc_id, &entry);

    soc_mem_unlock(unit, L3_IPMCm);

#if defined(BCM_BRADLEY_SUPPORT)
    if (BCM_SUCCESS(rv) && (SOC_IS_HBX(unit) || SOC_IS_TD_TT(unit)) &&
        !(soc_feature(unit, soc_feature_l2mc_table_no_shared_ipmc))) {
        /* On these devices, the Higig packets use the L2MC table with
         * IPMC offset to get the L2 bitmap. */
        int		mc_base, mc_size, mc_index;
        l2mc_entry_t	l2mc;

        SOC_IF_ERROR_RETURN
            (soc_hbx_ipmc_size_get(unit, &mc_base, &mc_size));
        /* coverity[check_after_sink] */
        if (ipmc_id < 0 || ipmc_id > mc_size) {
            return BCM_E_PARAM;
        }
        mc_index = ipmc_id + mc_base;

        soc_mem_lock(unit, L2MCm);
        if (!valid) {
            rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, mc_index,
                    soc_mem_entry_null(unit, L2MCm));
        } else {
            rv = READ_L2MCm(unit, MEM_BLOCK_ANY, mc_index, &l2mc);
            if (rv >= 0) {
                soc_mem_pbmp_field_set(unit, L2MCm, &l2mc,
                        PORT_BITMAPf, &l2_pbmp);
                soc_mem_field32_set(unit, L2MCm, &l2mc, VALIDf, 1);
                rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, mc_index, &l2mc);
            }
        }
        soc_mem_unlock(unit, L2MCm);
    }
#endif /* BCM_BRADLEY_SUPPORT */

    return rv;
}

/*
 * Function:
 *      _bcm_esw_ipmc_egress_intf_add
 * Purpose:
 *      Helper function to add ipmc egress interface on different devices.
 * Parameters:
 *      unit       - (IN)  Device Number
 *      ipmc_id    - (IN)  ipmc id
 *      port       - (IN)  Port number
 *      nh_index   - (IN)  Next Hop index
 *      is_l3      - (IN)  Indicates if multicast group type is IPMC.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                          int nh_index, int is_l3)
{
#if defined (INCLUDE_L3)
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        uint16 dev_id;
        uint8 rev_id;
        int local_port, intf_max, port_intf_count;
        int rv = BCM_E_NONE;
        int alloc_size;
        bcm_if_t *port_intf_array = NULL;
        bcm_pbmp_t l2_pbmp, l3_pbmp, pbmp_all;
        soc_info_t *si = &SOC_INFO(unit);
        uint32 itm_map = si->itm_map;
        int itm;
        bcm_pbmp_t link_bmap;
        uint32 entry[SOC_MAX_MEM_FIELD_WORDS];

        soc_cm_get_id(unit, &dev_id, &rev_id);
        if (rev_id != BCM56980_A0_REV_ID && dev_id != BCM56983_DEVICE_ID) {
            intf_max = TH3_IPMC_MAX_INTF_COUNT;

            /*Allocate intf array for intfs*/
            alloc_size = intf_max * sizeof(bcm_if_t);
            port_intf_array = sal_alloc(alloc_size, "IPMC group, port intfs array");
            if (port_intf_array == NULL) {
                return BCM_E_MEMORY;
            }
            BCM_PBMP_CLEAR(pbmp_all);
            BCM_PBMP_ASSIGN(pbmp_all, PBMP_ALL(unit));
            PBMP_ITER(pbmp_all, local_port) {
                if (!IS_LB_PORT(unit, local_port)) {
                    port_intf_count = 0;
                    /* retrieve the interface id array for the {group, port} */
                    rv = bcm_esw_ipmc_egress_intf_get(unit, ipmc_id, local_port,
                            intf_max, port_intf_array, &port_intf_count);
                    if (rv != BCM_E_NONE) {
                        sal_free(port_intf_array);
                        return rv;
                    }
                    if ((local_port == port) && (port_intf_count < intf_max)) {
                        /* Current port is the port for new encap id addition */
                        /* Current interface list doesnt have all the interface
                         * ids */
                        port_intf_array[port_intf_count] = nh_index;
                        port_intf_count++;
                    }
                    /* Add the new port, intf list to group 0*/
                    if (port_intf_count > 0) {
                        rv = _bcm_esw_ipmc_egress_intf_set(unit, 0, local_port,
                                port_intf_count, port_intf_array, 1, FALSE);
                        if (rv != BCM_E_NONE) {
                            sal_free(port_intf_array);
                            return rv;
                        }
                    }
                }
            }
            /* Free memory for interface array as it is not needed further*/
            sal_free(port_intf_array);
            /* Add port 158 to IP's  EPC_LINK_BMAP */
            soc_mem_lock(unit, EPC_LINK_BMAPm);
            rv = soc_mem_read(unit, EPC_LINK_BMAPm, MEM_BLOCK_ANY, 0, entry);
            if (rv != BCM_E_NONE) {
                soc_mem_unlock(unit, EPC_LINK_BMAPm);
                return rv;
            }
            soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                    &link_bmap);
            BCM_PBMP_PORT_ADD(link_bmap, 158);
            soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                    &link_bmap);
            rv = soc_mem_write(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0, &entry);
            soc_mem_unlock(unit, EPC_LINK_BMAPm);
            if (rv != BCM_E_NONE) {
                return rv;
            }

            /* Add port 158 to IP's  ING_DEST_PORT_ENABLE */
            soc_mem_lock(unit, ING_DEST_PORT_ENABLEm);
            rv = soc_mem_read(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ANY,
                                     0, entry);
            if (rv != BCM_E_NONE) {
                soc_mem_unlock(unit, ING_DEST_PORT_ENABLEm);
                return rv;
            }
            soc_mem_pbmp_field_get(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
                    &link_bmap);
            BCM_PBMP_PORT_ADD(link_bmap, 158);
            soc_mem_pbmp_field_set(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
                    &link_bmap);
            rv = soc_mem_write(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ALL, 0,
                        &entry);
            soc_mem_unlock(unit, ING_DEST_PORT_ENABLEm);
            if (rv != BCM_E_NONE) {
                return rv;
            }
            /* Add port 158 to the IPMC_L3_BITMAP  */
            soc_mem_lock(unit, L3_IPMCm);
            rv = _bcm_esw_multicast_ipmc_read(unit, ipmc_id, &l2_pbmp, &l3_pbmp);
            if (rv != BCM_E_NONE) {
                return rv;
            }
            BCM_PBMP_PORT_ADD(l3_pbmp, 158);
            rv = _bcm_esw_multicast_ipmc_write(unit, ipmc_id, l2_pbmp,
                                               l3_pbmp, TRUE);
            soc_mem_unlock(unit, L3_IPMCm);

            if (rv != BCM_E_NONE) {
                return rv;
            }
            if (!SAL_BOOT_SIMULATION) {
                /* mmu rqe Flush*/
                for (itm = 0; itm < NUM_ITM(unit); itm++) {
                    if (itm_map & (1 << itm)) {
                        /* Issue RQE flush twice per each ITM*/
                       rv = soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 1);
                       if (rv != BCM_E_NONE) {
                           return rv;
                       }
                       rv = soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 2);
                       if (rv != BCM_E_NONE) {
                           return rv;
                       }
                    }
                }
            }
            /* Update the repl group to add new repl_id to the port*/
            BCM_IF_ERROR_RETURN(_bcm_th_ipmc_egress_intf_add(unit, ipmc_id, port,
                        nh_index, is_l3));

             /* Remove port 158 from the IPMC_L3_BITMAP  */
            soc_mem_lock(unit, L3_IPMCm);
            rv = _bcm_esw_multicast_ipmc_read(unit, ipmc_id, &l2_pbmp, &l3_pbmp);
            if (rv != BCM_E_NONE) {
                soc_mem_unlock(unit, L3_IPMCm);
                return rv;
            }
            BCM_PBMP_PORT_REMOVE(l3_pbmp, 158);
            rv = _bcm_esw_multicast_ipmc_write(unit, ipmc_id, l2_pbmp,
                                               l3_pbmp, TRUE);
            soc_mem_unlock(unit, L3_IPMCm);
            if (rv != BCM_E_NONE) {
                return rv;
            }

            /* Remove port 158 from IP's EPC_LINK_BMAP */
            soc_mem_lock(unit, EPC_LINK_BMAPm);
            rv = soc_mem_read(unit, EPC_LINK_BMAPm, MEM_BLOCK_ANY, 0, entry);
            if (rv != BCM_E_NONE) {
                soc_mem_unlock(unit, EPC_LINK_BMAPm);
                return rv;
            }
            soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                    &link_bmap);
            BCM_PBMP_PORT_REMOVE(link_bmap, 158);
            soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                    &link_bmap);
            rv = soc_mem_write(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0,
                        &entry);
            soc_mem_unlock(unit, EPC_LINK_BMAPm);
            if (rv != BCM_E_NONE) {
                return rv;
            }

            /* Remove port 158 from IP's  ING_DEST_PORT_ENABLE */
            soc_mem_lock(unit, ING_DEST_PORT_ENABLEm);
            rv = soc_mem_read(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ANY,
                                     0, entry);
            if (rv != BCM_E_NONE) {
                soc_mem_unlock(unit, ING_DEST_PORT_ENABLEm);
                return rv;
            }
            soc_mem_pbmp_field_get(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
                    &link_bmap);
            BCM_PBMP_PORT_REMOVE(link_bmap, 158);
            soc_mem_pbmp_field_set(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
                    &link_bmap);
            rv = soc_mem_write(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ALL, 0,
                        &entry);
            soc_mem_unlock(unit, ING_DEST_PORT_ENABLEm);
            if (rv != BCM_E_NONE) {
                return rv;
            }


            if (!SAL_BOOT_SIMULATION) {
                /* mmu rqe Flush*/
                for (itm = 0; itm < NUM_ITM(unit); itm++) {
                    if (itm_map & (1 << itm)) {
                        /* Issue RQE flush twice per each ITM*/
                       rv = soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 1);
                       if (rv != BCM_E_NONE) {
                           return rv;
                       }
                       rv = soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 2);
                       if (rv != BCM_E_NONE) {
                           return rv;
                       }
                    }
                }
            }

            /* reset programming in repl group 0*/
            PBMP_ITER(pbmp_all, local_port) {
                if (!IS_LB_PORT(unit, local_port)) {
                    rv = _bcm_esw_ipmc_egress_intf_set(unit, 0, local_port, 0,
                            NULL, 1, FALSE);
                    if (rv != BCM_E_NONE) {
                            return rv;
                    }
                }
            }

            return rv;

        } else {
            return _bcm_th_ipmc_egress_intf_add(unit, ipmc_id, port, nh_index,
                                             is_l3);
        }
    }
#endif /* BCM_TOMAHAWK3_SUPPORT*/
#if defined(BCM_TOMAHAWK_SUPPORT)
    if ((SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) &&
            !(SOC_IS_TOMAHAWK3(unit))) {
        return _bcm_th_ipmc_egress_intf_add(unit, ipmc_id, port, nh_index,
                                             is_l3);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT2X(unit) ||
        SOC_IS_KATANA2(unit)) {
        return _bcm_tr3_ipmc_egress_intf_add(unit, ipmc_id, port, nh_index,
                                             is_l3);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT) 
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_VALKYRIE2(unit) ||
        SOC_IS_APOLLO(unit) || SOC_IS_TD_TT(unit) ||
        SOC_IS_KATANA(unit)) {
        return _bcm_tr2_ipmc_egress_intf_add(unit, ipmc_id, port, nh_index);
    }
#endif
#if defined(BCM_FIREBOLT_SUPPORT)
    return _bcm_fb_ipmc_egress_intf_add(unit, ipmc_id, port, nh_index, is_l3);
#endif /* BCM_FIREBOLT_SUPPORT */
#endif /* INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      _bcm_esw_ipmc_egress_intf_delete
 * Purpose:
 *      Helper function to delete ipmc egress interface on different devices.
 * Parameters:
 *      unit       - (IN)   Device Number
 *      ipmc_id    - (IN)   ipmc id
 *      port       - (IN)  Port number
 *      if_max     - (IN)  Maximal interface
 *      nh_index   - (IN)  Next Hop index
 *      is_l3      - (IN)  Indicates if multicast group type is IPMC.
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_esw_ipmc_egress_intf_delete(int unit, int ipmc_id,
                                 bcm_port_t port, int if_max,
                                 int nh_index, int is_l3)
{
#if defined (INCLUDE_L3)
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        uint16 dev_id;
        uint8 rev_id;
        int local_port, intf_max, port_intf_count;
        int rv = BCM_E_NONE;
        int alloc_size;
        bcm_if_t *port_intf_array = NULL;
        bcm_pbmp_t l2_pbmp, l3_pbmp, pbmp_all;
        soc_info_t *si = &SOC_INFO(unit);
        uint32 itm_map = si->itm_map;
        int itm, intf_loop, intf_del_idx = -1;
        bcm_pbmp_t link_bmap;
        uint32 entry[SOC_MAX_MEM_FIELD_WORDS];

        soc_cm_get_id(unit, &dev_id, &rev_id);
        if (rev_id != BCM56980_A0_REV_ID) {
            intf_max = TH3_IPMC_MAX_INTF_COUNT;

            /*Allocate intf array for intfs*/
            alloc_size = intf_max * sizeof(bcm_if_t);
            port_intf_array = sal_alloc(alloc_size, "IPMC group, port intfs array");
            if (port_intf_array == NULL) {
                return BCM_E_MEMORY;
            }
            BCM_PBMP_CLEAR(pbmp_all);
            BCM_PBMP_ASSIGN(pbmp_all, PBMP_ALL(unit));
            PBMP_ITER(pbmp_all, local_port) {
                if (!IS_LB_PORT(unit, local_port)) {
                    port_intf_count = 0;
                    /* retrieve the interface id array for the {group, port} */
                    rv = bcm_esw_ipmc_egress_intf_get(unit, ipmc_id, local_port,
                            intf_max, port_intf_array, &port_intf_count);
                    if (rv != BCM_E_NONE) {
                        sal_free(port_intf_array);
                        return rv;
                    }
                    if (local_port == port) {
                        /* Current port is the port for new encap id removal */
                        for (intf_loop = 0; intf_loop < port_intf_count;
                                intf_loop++) {
                            if (port_intf_array[intf_loop] == nh_index) {
                                intf_del_idx = intf_loop;
                                break;
                            }
                        }
                        if (intf_del_idx != -1) {
                            for (intf_loop = intf_del_idx;
                                    intf_loop < port_intf_count;
                                    intf_loop++) {
                                port_intf_array[intf_loop] =
                                    port_intf_array[(intf_loop + 1)];
                            }
                            port_intf_count--;
                        }
                    }
                    if (port_intf_count > 0) {
                        rv = _bcm_esw_ipmc_egress_intf_set(unit, 0, local_port,
                                port_intf_count, port_intf_array, 1, FALSE);
                        if (rv != BCM_E_NONE) {
                            sal_free(port_intf_array);
                            return rv;
                        }
                    }
                }
            }
            /*Free memory reserved for interafce array*/
            sal_free(port_intf_array);
                        /* Add port 158 to IP's  EPC_LINK_BMAP */
            soc_mem_lock(unit, EPC_LINK_BMAPm);
            rv = soc_mem_read(unit, EPC_LINK_BMAPm, MEM_BLOCK_ANY, 0, entry);
            if (rv != BCM_E_NONE) {
                soc_mem_unlock(unit, EPC_LINK_BMAPm);
                return rv;
            }
            soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                    &link_bmap);
            BCM_PBMP_PORT_ADD(link_bmap, 158);
            soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                    &link_bmap);
            rv = soc_mem_write(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0, &entry);
            soc_mem_unlock(unit, EPC_LINK_BMAPm);
            if (rv != BCM_E_NONE) {
                return rv;
            }

            /* Add port 158 to IP's  ING_DEST_PORT_ENABLE */
            soc_mem_lock(unit, ING_DEST_PORT_ENABLEm);
            rv = soc_mem_read(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ANY,
                                     0, entry);
            if (rv != BCM_E_NONE) {
                soc_mem_unlock(unit, ING_DEST_PORT_ENABLEm);
                return rv;
            }
            soc_mem_pbmp_field_get(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
                    &link_bmap);
            BCM_PBMP_PORT_ADD(link_bmap, 158);
            soc_mem_pbmp_field_set(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
                    &link_bmap);
            rv = soc_mem_write(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ALL, 0,
                        &entry);
            soc_mem_unlock(unit, ING_DEST_PORT_ENABLEm);
            if (rv != BCM_E_NONE) {
                return rv;
            }
            /* Add port 158 to the IPMC_L3_BITMAP  */
            soc_mem_lock(unit, L3_IPMCm);
            rv = _bcm_esw_multicast_ipmc_read(unit, ipmc_id, &l2_pbmp, &l3_pbmp);
            if (rv != BCM_E_NONE) {
                soc_mem_unlock(unit, L3_IPMCm);
                return rv;
            }
            BCM_PBMP_PORT_ADD(l3_pbmp, 158);
            rv = _bcm_esw_multicast_ipmc_write(unit, ipmc_id, l2_pbmp,
                                               l3_pbmp, TRUE);
            soc_mem_unlock(unit, L3_IPMCm);

            if (rv != BCM_E_NONE) {
                return rv;
            }
            if (!SAL_BOOT_SIMULATION) {
                /* mmu rqe Flush*/
                for (itm = 0; itm < NUM_ITM(unit); itm++) {
                    if (itm_map & (1 << itm)) {
                        /* Issue RQE flush twice per each ITM*/
                       rv = soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 1);
                       if (rv != BCM_E_NONE) {
                           return rv;
                       }
                       rv = soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 2);
                       if (rv != BCM_E_NONE) {
                           return rv;
                       }
                    }
                }
            }
            /* Update the repl group to add new repl_id to the port*/
            BCM_IF_ERROR_RETURN(_bcm_th_ipmc_egress_intf_delete(unit, ipmc_id,
                        port, intf_max, nh_index, is_l3));

             /* Remove port 158 from the IPMC_L3_BITMAP  */
            soc_mem_lock(unit, L3_IPMCm);
            rv = _bcm_esw_multicast_ipmc_read(unit, ipmc_id, &l2_pbmp, &l3_pbmp);
            if (rv != BCM_E_NONE) {
                soc_mem_unlock(unit, L3_IPMCm);
                return rv;
            }
            BCM_PBMP_PORT_REMOVE(l3_pbmp, 158);
            rv = _bcm_esw_multicast_ipmc_write(unit, ipmc_id, l2_pbmp,
                                               l3_pbmp, TRUE);
            soc_mem_unlock(unit, L3_IPMCm);
            if (rv != BCM_E_NONE) {
                return rv;
            }
            /* Remove port 158 from IP's EPC_LINK_BMAP */
            soc_mem_lock(unit, EPC_LINK_BMAPm);
            rv = soc_mem_read(unit, EPC_LINK_BMAPm, MEM_BLOCK_ANY, 0, entry);
            if (rv != BCM_E_NONE) {
                soc_mem_unlock(unit, EPC_LINK_BMAPm);
                return rv;
            }
            soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                    &link_bmap);
            BCM_PBMP_PORT_REMOVE(link_bmap, 158);
            soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                    &link_bmap);
            rv = soc_mem_write(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0,
                        &entry);
            soc_mem_unlock(unit, EPC_LINK_BMAPm);
            if (rv != BCM_E_NONE) {
                return rv;
            }

            /* Remove port 158 from IP's  ING_DEST_PORT_ENABLE */
            soc_mem_lock(unit, ING_DEST_PORT_ENABLEm);
            rv = soc_mem_read(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ANY,
                                     0, entry);
            if (rv != BCM_E_NONE) {
                soc_mem_unlock(unit, ING_DEST_PORT_ENABLEm);
                return rv;
            }
            soc_mem_pbmp_field_get(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
                    &link_bmap);
            BCM_PBMP_PORT_REMOVE(link_bmap, 158);
            soc_mem_pbmp_field_set(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf,
                    &link_bmap);
            rv = soc_mem_write(unit, ING_DEST_PORT_ENABLEm, MEM_BLOCK_ALL, 0,
                        &entry);
            soc_mem_unlock(unit, ING_DEST_PORT_ENABLEm);
            if (rv != BCM_E_NONE) {
                return rv;
            }

            if (!SAL_BOOT_SIMULATION) {
                /* mmu rqe Flush*/
                for (itm = 0; itm < NUM_ITM(unit); itm++) {
                    if (itm_map & (1 << itm)) {
                        /* Issue RQE flush twice per each ITM*/
                       rv = soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 1);
                       if (rv != BCM_E_NONE) {
                           return rv;
                       }
                       rv = soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 2);
                       if (rv != BCM_E_NONE) {
                           return rv;
                       }
                    }
                }
            }
            /* reset programming in repl group 0*/
            PBMP_ITER(pbmp_all, local_port) {
                if (!IS_LB_PORT(unit, local_port)) {
                    rv = _bcm_esw_ipmc_egress_intf_set(unit, 0, local_port, 0,
                            NULL, 1, FALSE);
                    if (rv != BCM_E_NONE) {
                            return rv;
                    }
                }
            }

            return rv;

        } else {
            return _bcm_th_ipmc_egress_intf_delete(unit, ipmc_id, port,
                    if_max, nh_index, is_l3);
        }
    }
#endif /* BCM_TOMAHAWK3_SUPPORT*/

#if defined(BCM_TOMAHAWK_SUPPORT)
    if ((SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) &&
            (!SOC_IS_TOMAHAWK3(unit))) {
        return _bcm_th_ipmc_egress_intf_delete(unit, ipmc_id, port,
                                                if_max, nh_index, is_l3);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT2X(unit) ||
        SOC_IS_KATANA2(unit))     {
        return _bcm_tr3_ipmc_egress_intf_delete(unit, ipmc_id, port,
                                                if_max, nh_index, is_l3);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT) 
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_VALKYRIE2(unit) ||
        SOC_IS_APOLLO(unit) || SOC_IS_TD_TT(unit) || SOC_IS_KATANA(unit)) {
        return _bcm_tr2_ipmc_egress_intf_delete(unit, ipmc_id, port, 
                                                if_max, nh_index);
    }
#endif
#if defined(BCM_FIREBOLT_SUPPORT)
    return _bcm_fb_ipmc_egress_intf_delete(unit, ipmc_id, port,
                                           if_max, nh_index, is_l3);
#endif /* BCM_FIREBOLT_SUPPORT */
 #endif /* INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

#ifdef BCM_WARM_BOOT_SUPPORT
#ifdef BCM_XGS3_FABRIC_SUPPORT
/*
 * Function:
 *      _bcm_fabric_multicast_sync
 * Purpose:
 *      Record Fabric's Multicast module persistent info for Level 2 Warm Boot
 * Parameters:
 *      unit - Device number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_fabric_multicast_sync(int unit)
{
    int ipmc_mem_size = 0, ipmc_base, alloc_size;
    soc_scache_handle_t scache_handle;
    uint8               *multicast_scache;

    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
        return BCM_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (soc_hbx_ipmc_size_get(unit, &ipmc_base, &ipmc_mem_size));

    alloc_size = ipmc_mem_size * sizeof(uint8);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_MULTICAST, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0,
                                 &multicast_scache,
                                 BCM_FABRIC_IPMC_WB_DEFAULT_VERSION, NULL));

    if (_fabric_multicast_ipmc_group_types[unit] != NULL) {
        sal_memcpy(multicast_scache, _fabric_multicast_ipmc_group_types[unit],
                alloc_size);
    }

    return BCM_E_NONE;
}
#endif
/*
 * Function:
 *      _bcm_esw_multicast_sync
 * Purpose:
 *      Record Multicast module persisitent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_multicast_sync(int unit)
{
#ifdef BCM_XGS3_FABRIC_SUPPORT
    if(SOC_IS_XGS3_FABRIC(unit)){
        return _bcm_fabric_multicast_sync(unit);
    }
#endif
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRX(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
        return _bcm_trx_multicast_sync(unit);
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */


#if defined(BCM_XGS3_SWITCH_SUPPORT) 

/*
 * Function:
 *      _bcm_esw_multicast_l2_add
 * Purpose:
 *      Helper function to add a port to L2 multicast group
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_esw_multicast_l2_add(int unit, bcm_multicast_t group, 
                          bcm_port_t port, bcm_if_t encap_id)
{
    l2mc_entry_t    l2mc_entry;
    bcm_pbmp_t      l2_pbmp;
    int             mc_index, gport_id, modid_local, idx, trunk_local_ports = 0;
    bcm_module_t    mod_out;
    bcm_port_t      port_out;
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t      local_trunk_member_ports[SOC_MAX_NUM_PP_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t      local_trunk_member_ports[SOC_MAX_NUM_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif
    bcm_trunk_t     trunk_id;
#if defined(BCM_KATANA2_SUPPORT)
    int is_local_subport = FALSE, pp_port;
#endif
 
    modid_local = 0;

    mc_index = _BCM_MULTICAST_ID_GET(group);

    /* Table index sanity check. */
#ifdef BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_fabric_multicast_param_check(unit, group, &mc_index));
    } else
#endif /* BCM_XGS3_FABRIC_SUPPORT */
    if ((mc_index < 0) ||
        (mc_index >= soc_mem_index_count(unit, L2MCm))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out,
                                &trunk_id, &gport_id)); 

#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        if (BCM_GPORT_IS_SET(port) &&
            _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
            if (_bcm_kt2_mod_is_coe_mod_check(unit, mod_out) == BCM_E_NONE) {
                pp_port = BCM_GPORT_SUBPORT_PORT_GET(port);
                if (_bcm_kt2_is_pp_port_linkphy_subtag(unit, pp_port)) {
                    is_local_subport = TRUE;
                }
            }
            if (is_local_subport == FALSE) {
                return BCM_E_PORT;
            }
        }
    }
    if (is_local_subport) {
        port_out = pp_port;
    } else
#endif

    if (BCM_TRUNK_INVALID != trunk_id) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_local_members_get(unit, trunk_id, 
                                             max_num_ports,
                                             local_trunk_member_ports, 
                                             &trunk_local_ports));
        /* Convertion of system ports to physical ports done in */
        /* _bcm_esw_multicast_trunk_members_setup */
    } else {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_modid_is_local(unit, mod_out, &modid_local));
        if (TRUE != modid_local) {
            /* Only add this to replication set if destination is local */
            return BCM_E_PORT;
        }
        /* Convert system port to physical port */
        if (soc_feature(unit, soc_feature_sysport_remap)) {
            BCM_XLATE_SYSPORT_S2P(unit, &port_out);
        }
    }

    /* Add the port to the L2MC port bitmap */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, L2MCm, MEM_BLOCK_ANY, mc_index, &l2mc_entry));

    soc_mem_pbmp_field_get(unit, L2MCm, &l2mc_entry, PORT_BITMAPf,
                           &l2_pbmp);

    if (BCM_TRUNK_INVALID != trunk_id) {
        for (idx = 0; idx < trunk_local_ports; idx++) {
            BCM_PBMP_PORT_ADD(l2_pbmp, local_trunk_member_ports[idx]);
        }
    } else {
        BCM_PBMP_PORT_ADD(l2_pbmp, port_out);
    }
    soc_mem_pbmp_field_set(unit, L2MCm, &l2mc_entry, PORT_BITMAPf, &l2_pbmp);

    return soc_mem_write(unit, L2MCm, MEM_BLOCK_ALL, mc_index, &l2mc_entry);
}

/*
 * Function:
 *      _bcm_esw_multicast_l2_delete
 * Purpose:
 *      Helper function to delete a port from L2 multicast group
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_esw_multicast_l2_delete(int unit, bcm_multicast_t group, 
                             bcm_port_t port, bcm_if_t encap_id)
{
    l2mc_entry_t    l2mc_entry;
    bcm_pbmp_t      l2_pbmp;
    int             mc_index, gport_id, modid_local, idx, trunk_local_ports = 0;
    bcm_module_t    mod_out;
    bcm_port_t      port_out;
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t      local_trunk_member_ports[SOC_MAX_NUM_PP_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t      local_trunk_member_ports[SOC_MAX_NUM_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif
    bcm_trunk_t     trunk_id;
#if defined(BCM_KATANA2_SUPPORT)
    int is_local_subport = FALSE, pp_port;
#endif

    modid_local = 0;
    mc_index = _BCM_MULTICAST_ID_GET(group);

#ifdef BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_fabric_multicast_param_check(unit, group, &mc_index));
    } else
#endif /* BCM_XGS3_FABRIC_SUPPORT */
    if ((mc_index < 0) ||
        (mc_index >= soc_mem_index_count(unit, L2MCm))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, 
                                &trunk_id, &gport_id)); 

#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        if (BCM_GPORT_IS_SET(port) &&
            _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
            if (_bcm_kt2_mod_is_coe_mod_check(unit, mod_out) == BCM_E_NONE) {
                pp_port = BCM_GPORT_SUBPORT_PORT_GET(port);
                if (_bcm_kt2_is_pp_port_linkphy_subtag(unit, pp_port)) {
                        is_local_subport = TRUE;
                }
            }
            if (is_local_subport == FALSE) {
                return BCM_E_PORT;
            }
        }
    }
    if (is_local_subport) {
        port_out = pp_port;
    } else
#endif

    if (BCM_TRUNK_INVALID != trunk_id) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_local_members_get(unit, trunk_id, 
                                             max_num_ports,
                                             local_trunk_member_ports, 
                                             &trunk_local_ports));
        /* Convertion of system ports to physical ports done in */
        /* _bcm_esw_trunk_local_members_get */
    } else {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_modid_is_local(unit, mod_out, &modid_local));
        if (TRUE != modid_local) {
            /* Only add this to replication set if destination is local */
            return BCM_E_PORT;
        }
        /* Convert system port to physical port */
        if (soc_feature(unit, soc_feature_sysport_remap)) {
            BCM_XLATE_SYSPORT_S2P(unit, &port_out);
        }
    }

    /* Delete the port from the L2MC port bitmap */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, L2MCm, MEM_BLOCK_ANY, mc_index, &l2mc_entry));

    soc_mem_pbmp_field_get(unit, L2MCm, &l2mc_entry, PORT_BITMAPf,
                           &l2_pbmp);


    if (BCM_TRUNK_INVALID != trunk_id) {
        for (idx = 0; idx < trunk_local_ports; idx++) {
            BCM_PBMP_PORT_REMOVE(l2_pbmp, local_trunk_member_ports[idx]);
        }
    } else {
        BCM_PBMP_PORT_REMOVE(l2_pbmp, port_out);
    }
    soc_mem_pbmp_field_set(unit, L2MCm, &l2mc_entry, PORT_BITMAPf, &l2_pbmp);

    return soc_mem_write(unit, L2MCm, MEM_BLOCK_ALL, mc_index, &l2mc_entry);
}

#if (defined(BCM_TRX_SUPPORT) || defined(BCM_XGS3_SWITCH_SUPPORT)) && defined(INCLUDE_L3) 
/*
 * Function:
 *      _bcm_esw_multicast_l2_get
 * Purpose:
 *      Helper function to get ports that are part of L2 multicast 
 * Parameters: 
 *      unit           - (IN) Device Number
 *      mc_index       - (IN) Multicast index
 *      port_max       - (IN) Number of entries in "port_array"
 *      port_array     - (OUT) List of ports
 *      encap_id_array - (OUT) List of encap identifiers
 *      port_count     - (OUT) Actual number of ports returned
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the input parameter port_max = 0, return in the output parameter
 *      port_count the total number of ports/encapsulation IDs in the 
 *      specified multicast group's replication list.
 */

STATIC int     
_bcm_esw_multicast_l2_get(int unit, bcm_multicast_t group, int port_max,
                             bcm_gport_t *port_array, bcm_if_t *encap_id_array, 
                             int *port_count)
{
    int             mc_index, i, rv;
    l2mc_entry_t    l2mc_entry;
    bcm_pbmp_t      l2_pbmp;
    bcm_port_t      port_iter;

    i = 0;
    rv = BCM_E_NONE;

    mc_index = _BCM_MULTICAST_ID_GET(group);

#ifdef BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_fabric_multicast_param_check(unit, group, &mc_index));
    } else
#endif /* BCM_XGS3_FABRIC_SUPPORT */
    if ((mc_index < 0) ||
        (mc_index >= soc_mem_index_count(unit, L2MCm))) {
        return BCM_E_PARAM;
    }

    soc_mem_lock(unit, L2MCm);
    rv = soc_mem_read(unit, L2MCm, MEM_BLOCK_ANY, mc_index, &l2mc_entry);
    soc_mem_unlock(unit, L2MCm);
    BCM_IF_ERROR_RETURN(rv);

    soc_mem_pbmp_field_get(unit, L2MCm, &l2mc_entry, PORT_BITMAPf, &l2_pbmp);
    PBMP_ITER(l2_pbmp, port_iter) {
        if ((port_max > 0) && (i >= port_max)) {
            break;
        }
        if (NULL != port_array) {
#if defined(BCM_KATANA2_SUPPORT)
            /* For KT2, pp_port >=42 are used for LinkPHY/SubportPktTag subport.
             * Get the subport info associated with the pp_port and form the subport_gport 
             * subport gport should be returned in the port_array for pp_port >=42*/

            if (_bcm_kt2_is_pp_port_linkphy_subtag(unit, port_iter)) {
                _BCM_KT2_SUBPORT_PORT_ID_SET(port_array[i], port_iter);
                if (BCM_PBMP_MEMBER(
                    SOC_INFO(unit).linkphy_pp_port_pbm, port_iter)) {
                    _BCM_KT2_SUBPORT_PORT_TYPE_SET(port_array[i],
                        _BCM_KT2_SUBPORT_TYPE_LINKPHY);
                } else if (BCM_PBMP_MEMBER(
                    SOC_INFO(unit).subtag_pp_port_pbm, port_iter)) {
                    _BCM_KT2_SUBPORT_PORT_TYPE_SET(port_array[i],
                        _BCM_KT2_SUBPORT_TYPE_SUBTAG);
                } else {
                    return BCM_E_PORT;
                }
            } else
#endif /* BCM_KATANA2_SUPPORT */
            {
                BCM_IF_ERROR_RETURN
                (bcm_esw_port_gport_get(unit, port_iter, (port_array + i)));
            }
        }

        if (NULL != encap_id_array) {
            encap_id_array[i] = BCM_IF_INVALID;
        }
        i++;
    }
    *port_count = i;
    return BCM_E_NONE;
}
#endif /* #if (defined(BCM_TRX_SUPPORT) || defined(BCM_XGS3_SWITCH_SUPPORT)) && defined(INCLUDE_L3) */

/*
 * Function:
 *      _bcm_esw_multicast_l2_create
 * Purpose:
 *      Helper function to allocate a multicast group index for l2
 * Parameters:
 *      unit       - (IN)   Device Number
 *      flags      - (IN)   BCM_MULTICAST_*
 *      group      - (OUT)  Group ID
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_multicast_l2_create(int unit, uint32 flags, bcm_multicast_t *group)
{
    int             mc_index;
    int             is_set = FALSE;
    l2mc_entry_t    l2mc_entry;

    if (flags & BCM_MULTICAST_WITH_ID) {
        if (_BCM_MULTICAST_IS_SET(*group) && !_BCM_MULTICAST_IS_L2(*group)) {
            /* Group ID doesn't match creation flag type */
            return BCM_E_PARAM;
        }
        mc_index = _BCM_MULTICAST_ID_GET(*group);
        if ((mc_index < 0) ||
            (mc_index >= soc_mem_index_count(unit, L2MCm))) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(_bcm_xgs3_l2mc_index_is_set(unit, mc_index, &is_set));
        if (is_set) {
            return BCM_E_EXISTS;
        }
        
        BCM_IF_ERROR_RETURN(_bcm_xgs3_l2mc_id_alloc(unit, mc_index));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_xgs3_l2mc_free_index(unit, &mc_index));
    }
    sal_memset(&l2mc_entry, 0, sizeof(l2mc_entry));
    soc_mem_field32_set(unit, L2MCm, &l2mc_entry, VALIDf, 1);
    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, L2MCm, MEM_BLOCK_ALL, mc_index, &l2mc_entry));
    _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L2, mc_index);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_multicast_l2_destroy
 * Purpose:
 *      Helper function to destroy L2 multicast 
 * Parameters: 
 *      unit           - (IN) Device Number
 *      group          - (IN) Multicast group to destroy
 * Returns:
 *      BCM_E_XXX
 */

STATIC int     
_bcm_esw_multicast_l2_destroy(int unit, bcm_multicast_t group)
{
    int         mc_index;
#if defined(BCM_TSN_SR_SUPPORT)
    int l2mc_index;
    bcm_port_t l2mc_port;
    sr_port_table_entry_t sr_port_entry;
    sr_lport_tab_entry_t sr_lport_entry;
#endif /* BCM_TSN_SR_SUPPORT */
    mc_index = _BCM_MULTICAST_ID_GET(group);

    if ((mc_index < 0) ||
        (mc_index >= soc_mem_index_count(unit, L2MCm))) {
        return BCM_E_PARAM;
    }

    /* Clear the HW entry */
    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, L2MCm, MEM_BLOCK_ALL, 
                      mc_index, soc_mem_entry_null(unit, L2MCm)));

#if defined(BCM_TSN_SR_SUPPORT)
    /* Clear L2MC for related SR ports if mc_index matches */
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        int sr_lport_idx;
        bcm_module_t my_modid = -1;
        int src_trk_idx = 0; /* Source Trunk table index.*/
        uint32 mem_entry[SOC_MAX_MEM_WORDS];

        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &my_modid));

        BCM_PBMP_ITER(PBMP_ALL(unit), l2mc_port) {
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, SR_PORT_TABLEm, MEM_BLOCK_ANY,
                             l2mc_port, &sr_port_entry));
            l2mc_index = soc_mem_field32_get(unit, SR_PORT_TABLEm,
                                             &sr_port_entry, L2MCf);
            if (l2mc_index == mc_index) {
                soc_mem_field32_set(unit, SR_PORT_TABLEm, &sr_port_entry,
                                    L2MCf, 0x0);
                BCM_IF_ERROR_RETURN(
                    soc_mem_write(unit, SR_PORT_TABLEm, MEM_BLOCK_ALL,
                                  l2mc_port, &sr_port_entry));
            }

            BCM_IF_ERROR_RETURN(
                _bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                                                      l2mc_port, &src_trk_idx));
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ANY,
                             src_trk_idx, mem_entry));

            sr_lport_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                               mem_entry, LPORT_PROFILE_IDXf);

            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, SR_LPORT_TABm, MEM_BLOCK_ANY,
                             sr_lport_idx, &sr_lport_entry));
            l2mc_index = soc_mem_field32_get(unit, SR_LPORT_TABm,
                                             &sr_lport_entry, L2MCf);
            if (l2mc_index == mc_index) {
                soc_mem_field32_set(unit, SR_LPORT_TABm, &sr_lport_entry,
                                    L2MCf, 0x0);
                BCM_IF_ERROR_RETURN(
                    soc_mem_write(unit, SR_LPORT_TABm, MEM_BLOCK_ALL,
                                  sr_lport_idx, &sr_lport_entry));
            }
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */

    /* Free the L2MC index */
    return _bcm_xgs3_l2mc_id_free(unit, mc_index);
}

#if defined (INCLUDE_L3)

/*
 * Function:
 *      _bcm_esw_multicast_egress_mapped_trunk_member_find
 * Purpose:
 *      Find the trunk member the encap id resides on.
 * Parameters:
 *      unit  - (IN) Device Number
 *      group - (IN) Multicast group ID
 *      port  - (IN) GPORT ID
 *      encap_id - (IN) Encap identifiers
 *      trunk_member - (OUT) Trunk member the encap id resides on
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_multicast_egress_mapped_trunk_member_find(int unit,
        bcm_multicast_t group, bcm_gport_t port, bcm_if_t encap_id,
        bcm_gport_t *trunk_member)
{
    int rv = BCM_E_NONE;
    bcm_trunk_t trunk_id;
    bcm_trunk_info_t tinfo;
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t      local_trunk_port_array[SOC_MAX_NUM_PP_PORTS];
    int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t      local_trunk_port_array[SOC_MAX_NUM_PORTS];
    int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif
    int num_local_trunk_ports;
    int port_count;
    bcm_gport_t *port_array;
    bcm_if_t *encap_id_array;
    int match;
    int i, j;
    bcm_port_t local_port;
    int mc_index;
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    int is_local_subport = FALSE;
#endif
    if (NULL == trunk_member) {
        return BCM_E_PARAM;
    }

    *trunk_member = port;

    if (0 == _BCM_MULTICAST_IS_L3(group)) {
        /* Software trunk resolution is available only for IPMC groups */
        return BCM_E_NONE;
    }
    mc_index = _BCM_MULTICAST_ID_GET(group);
    if ((mc_index < 0) || (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

    if (!BCM_GPORT_IS_TRUNK(port)) {
        return BCM_E_NONE;
    }

    if (BCM_IF_INVALID == encap_id) {
        return BCM_E_NONE;
    }

    /* Get trunk property and local member ports */
    trunk_id = BCM_GPORT_TRUNK_GET(port);
    BCM_IF_ERROR_RETURN
        (bcm_esw_trunk_get(unit, trunk_id, &tinfo, 0, NULL, NULL));
    if (!(tinfo.flags & BCM_TRUNK_FLAG_IPMC_CLEAVE)) {
        return BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, trunk_id,
                max_num_ports, local_trunk_port_array,
                &num_local_trunk_ports));

    /* Get all ports and encap_ids of the multicast group */
    BCM_IF_ERROR_RETURN(bcm_esw_multicast_egress_get(unit, group, 0,
                NULL, NULL, &port_count));
    if (0 == port_count) {
        return BCM_E_NOT_FOUND;
    }
    port_array = sal_alloc(sizeof(bcm_gport_t) * port_count, "port_array");
    if (NULL == port_array) {
        return BCM_E_MEMORY;
    }
    encap_id_array = sal_alloc(sizeof(bcm_if_t) * port_count, "encap_id_array");
    if (NULL == encap_id_array) {
        sal_free(port_array);
        return BCM_E_MEMORY;
    }
    rv = bcm_esw_multicast_egress_get(unit, group, port_count,
            port_array, encap_id_array, &port_count);
    if (BCM_FAILURE(rv)) {
        sal_free(port_array);
        sal_free(encap_id_array);
        return rv;
    }

    /* Search for the trunk member the encap id resides on */
    match = FALSE;
    for (i = 0; i < port_count; i++) {
        if (encap_id == encap_id_array[i]) {

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            BCM_GPORT_IS_SET(port_array[i]) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
            is_local_subport = TRUE;
            _bcmi_coe_subport_physical_port_get(unit, port_array[i],
                                                      &local_port);
        } else 
#endif

#if defined(BCM_KATANA2_SUPPORT)
        if (soc_feature(unit, soc_feature_linkphy_coe) ||
            soc_feature(unit, soc_feature_subtag_coe)) {
            if (BCM_GPORT_IS_SET(port_array[i]) &&
                _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit,
                port_array[i])) {
                is_local_subport = TRUE;
                local_port = _BCM_KT2_SUBPORT_PORT_ID_GET(port_array[i]);
            }
        }
#endif

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
            if (is_local_subport) {
                /* Local subport assigned above */
            } else
#endif
            {
                rv = bcm_esw_port_local_get(unit, port_array[i], &local_port);
                if (BCM_FAILURE(rv)) {
                    sal_free(port_array);
                    sal_free(encap_id_array);
                    return rv;
                }
            }
            for (j = 0; j < num_local_trunk_ports; j++) {
                if (local_trunk_port_array[j] == local_port) {
                    match = TRUE;
                    break;
                } 
            }
            if (match) {
                break;
            }
        }
    }

    if (match) {
        *trunk_member = port_array[i];
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    sal_free(port_array);
    sal_free(encap_id_array);
    return rv;
}

/*
 * Function:
 *      _bcm_esw_multicast_egress_encap_id_to_trunk_member_map
 * Purpose:
 *      Maps encap id to a trunk group member. This is performed
 *      if (1) the given group is an IPMC group, (2) the port in port_array is 
 *      a trunk gport, and (3) IPMC trunk resolution is disabled for that trunk
 *      group in hardware by setting the BCM_TRUNK_FLAG_IPMC_CLEAVE flag in
 *      bcm_trunk_info_t. The hash fields used for trunk group member
 *      selection are specified by ipmc_psc field in bcm_trunk_info_t.
 *      The valid values for ipmc_psc are BCM_TRUNK_PSC_IPSA, IPDA, EGRESS_VID,
 *      and RANDOM.
 * Parameters:
 *      unit       - (IN) Device Number
 *      group      - (IN) Multicast group ID
 *      port_count     - (IN) Number of ports in replication list
 *      port_array     - (IN) List of GPORT Identifiers
 *      encap_id_array - (IN) List of encap identifiers
 *      mapped_port_array - (OUT) List of mapped GPORT identifiers
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_multicast_egress_encap_id_to_trunk_member_map(int unit,
        bcm_multicast_t group, int port_count,
        bcm_gport_t *port_array, bcm_if_t *encap_id_array,
        bcm_gport_t *mapped_port_array)
{
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t ipmc_data;
    uint8 *hash_key = NULL;
    uint8 *hash_key_ptr = NULL;
    int hash_key_size, max_hash_key_size;
    int i, j;
    bcm_trunk_t trunk_id;
    bcm_trunk_info_t tinfo;
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
        bcm_port_t      local_trunk_port_array[SOC_MAX_NUM_PP_PORTS];
        int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
        bcm_port_t      local_trunk_port_array[SOC_MAX_NUM_PORTS];
        int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif
    int num_local_trunk_ports;
    bcm_l3_intf_t l3_intf;
    uint32 hash;
    bcm_port_t mapped_port;
    int mc_index;
    int random_num;
    int optimize_for_space;
    bcm_if_t *temp_encap_array = NULL;
    int *sublist_id_array = NULL;
    int temp_encap_count;
    int sublist_max, sublist_count = 0;
    int first_free_sublist_id, next_free_sublist_id;
    int selected_sublist_id;
    int sublist0_trunk_member_index;
    int trunk_member_index;
    int temp_encap_index;

    if (port_count < 0) {
        return BCM_E_PARAM;
    }

    if (0 == port_count) {
        /* Nothing to do */
        return BCM_E_NONE;
    }

    if (NULL == port_array) {
        return BCM_E_PARAM;
    }

    /* Copy port_array to mapped_port_array */
    sal_memcpy(mapped_port_array, port_array,
            sizeof(bcm_gport_t) * port_count);

    if (0 == _BCM_MULTICAST_IS_L3(group)) {
        /* Software trunk resolution is available only for IPMC groups */
        return BCM_E_NONE;
    }
    mc_index = _BCM_MULTICAST_ID_GET(group);
    if ((mc_index < 0) || (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

    if (NULL == encap_id_array) {
        /* For IPMC groups, encap_id_array should not be NULL */
        return BCM_E_PARAM;
    }

    /* Allocate hash key. The hash key contains at most two IPv6 address,
     * a VLAN ID, and a random integer.
     */
    max_hash_key_size = 2 * sizeof(bcm_ip6_t) + sizeof(bcm_vlan_t) +
                        sizeof(int);
    hash_key = sal_alloc(max_hash_key_size, "hash key");
    if (NULL == hash_key) {
        return BCM_E_MEMORY;
    }
    sal_memset(hash_key, 0, max_hash_key_size);

    /* Iterate through mapped_port_array */
    for (i = 0; i < port_count; i++) {
        if (!BCM_GPORT_IS_TRUNK(mapped_port_array[i])) {
            continue;
        }

        if (BCM_IF_INVALID == encap_id_array[i]) {
            continue;
        }

        trunk_id = BCM_GPORT_TRUNK_GET(mapped_port_array[i]);
        rv = bcm_esw_trunk_get(unit, trunk_id, &tinfo, 0, NULL, NULL);
        if (BCM_FAILURE(rv)) {
            goto done;
        }
        if (!(tinfo.flags & BCM_TRUNK_FLAG_IPMC_CLEAVE)) {
            /* IPMC trunk resolution is enabled in hardware,
             * do not perform software trunk resolution.
             */
            continue;
        }
        /* system ports to physical ports conversion done in
         * _bcm_esw_trunk_local_members_get
         */

        rv = _bcm_esw_trunk_local_members_get(unit, trunk_id,
                                         max_num_ports,
                                         local_trunk_port_array,
                                         &num_local_trunk_ports);

        if (BCM_FAILURE(rv)) {
             goto done;
        }

        /* Compute hash */
        sal_memset(hash_key, 0, max_hash_key_size);
        hash_key_ptr = hash_key;

        if ((tinfo.ipmc_psc & BCM_TRUNK_PSC_IPSA) ||
                (tinfo.ipmc_psc & BCM_TRUNK_PSC_IPDA)) {
            bcm_ipmc_addr_t_init(&ipmc_data);
            rv = bcm_esw_ipmc_get_by_index(unit, _BCM_MULTICAST_ID_GET(group),
                    &ipmc_data);
            if (BCM_FAILURE(rv)) {
                goto done;
            }
            if (ipmc_data.flags & BCM_IPMC_IP6) {
                if (tinfo.ipmc_psc & BCM_TRUNK_PSC_IPSA) {
                    sal_memcpy(hash_key_ptr, &ipmc_data.s_ip6_addr,
                            sizeof(ipmc_data.s_ip6_addr));
                    hash_key_ptr += sizeof(ipmc_data.s_ip6_addr);
                }
                if (tinfo.ipmc_psc & BCM_TRUNK_PSC_IPDA) {
                    sal_memcpy(hash_key_ptr, &ipmc_data.mc_ip6_addr,
                            sizeof(ipmc_data.mc_ip6_addr));
                    hash_key_ptr += sizeof(ipmc_data.mc_ip6_addr);
                }
            } else {
                if (tinfo.ipmc_psc & BCM_TRUNK_PSC_IPSA) {
                    sal_memcpy(hash_key_ptr, &ipmc_data.s_ip_addr,
                            sizeof(ipmc_data.s_ip_addr));
                    hash_key_ptr += sizeof(ipmc_data.s_ip_addr);
                }
                if (tinfo.ipmc_psc & BCM_TRUNK_PSC_IPDA) {
                    sal_memcpy(hash_key_ptr, &ipmc_data.mc_ip_addr,
                            sizeof(ipmc_data.mc_ip_addr));
                    hash_key_ptr += sizeof(ipmc_data.mc_ip_addr);
                }
            }
        }

        if (tinfo.ipmc_psc & BCM_TRUNK_PSC_EGRESS_VID) {
            if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, encap_id_array[i])) {
                bcm_l3_intf_t_init(&l3_intf);
                l3_intf.l3a_intf_id = encap_id_array[i];
                rv = bcm_esw_l3_intf_get(unit, &l3_intf);
                if (BCM_FAILURE(rv)) {
                    goto done;
                }
                sal_memcpy(hash_key_ptr, &l3_intf.l3a_vid,
                        sizeof(bcm_vlan_t));
                hash_key_ptr += sizeof(bcm_vlan_t);
            }
        }

        if (tinfo.ipmc_psc & BCM_TRUNK_PSC_RANDOM) {
            random_num = sal_time_usecs();
            sal_memcpy(hash_key_ptr, &random_num, sizeof(int));
            hash_key_ptr += sizeof(int);
        }

        hash_key_size = hash_key_ptr - hash_key;
        if (hash_key_size > 0) {
            hash = _shr_crc32(0, hash_key, hash_key_size);
        } else {
            hash = 0;
        }

        optimize_for_space = FALSE;
#if defined(BCM_TRIUMPH2_SUPPORT) 
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                SOC_IS_VALKYRIE2(unit) || SOC_IS_TRIDENT(unit) ||
                SOC_IS_KATANA(unit)) {
            int available_percent, threshold;

            rv = bcm_esw_switch_control_get(unit,
                    bcmSwitchIpmcReplicationAvailabilityThreshold, &threshold);
            if (BCM_FAILURE(rv)) {
                goto done;
            }
            if (threshold > 0) {
                rv = bcm_tr2_ipmc_repl_availability_get(unit, &available_percent);
                if (BCM_FAILURE(rv)) {
                    goto done;
                }
                if (available_percent < threshold) {
                    /* IPMC replication table available space has fallen below the
                     * configured threshold. Conservation of IPMC replication table
                     * space now takes precedence over hashing when assigning encap 
                     * ids to trunk members.
                     */
                    optimize_for_space = TRUE;
                }
            }
        } 
#endif  /* BCM_TRIUMPH2_SUPPORT */

        if (!optimize_for_space) {
            mapped_port = local_trunk_port_array[hash % num_local_trunk_ports];
            rv = bcm_esw_port_gport_get(unit, mapped_port,
                    &mapped_port_array[i]);
            if (BCM_FAILURE(rv)) {
                goto done;
            }
            continue;
        } 

        /* Assign encap ids to trunk members with conservation of IPMC
         * replication table space as highest priority.
         * First, gather all encap ids with the same trunk ID as the
         * current trunk group.
         */

        if (NULL == temp_encap_array) {
            temp_encap_array = sal_alloc(sizeof(bcm_if_t) * port_count,
                    "temp encap array");
            if (NULL == temp_encap_array) {
                rv = BCM_E_MEMORY;
                goto done;
            }
        }
        sal_memset(temp_encap_array, 0, sizeof(bcm_if_t) * port_count);

        if (NULL == sublist_id_array) {
            sublist_id_array = sal_alloc(sizeof(int) * port_count,
                    "sublist id array");
            if (NULL == sublist_id_array) {
                rv = BCM_E_MEMORY;
                goto done;
            }
        }
        sal_memset(sublist_id_array, 0, sizeof(int) * port_count);

        temp_encap_count = 0;
        for (j = i; j < port_count; j++) {
            if (!BCM_GPORT_IS_TRUNK(mapped_port_array[j])) {
                continue;
            }
            if (BCM_IF_INVALID == encap_id_array[j]) {
                continue;
            }
            if (BCM_GPORT_TRUNK_GET(mapped_port_array[j]) != trunk_id) {
                continue;
            }
            temp_encap_array[temp_encap_count++] = encap_id_array[j];
        }

        /* Split the current trunk group's list of encap ids into sublists,
         * such that the sublists match existing replication lists,
         * enabling sharing of IPMC replication table entries.
         * It's not necessary to generate more sublists than there are
         * trunk members, since mapping multiple sublists to a trunk member
         * would effectively join those sublists.
         */

        sublist_max = num_local_trunk_ports;
        for (j = 0; j < temp_encap_count; j++) {
            /* Initialize each encap id's sublist id to be
             * the invalid value of sublist_max.
             */
            sublist_id_array[j] = sublist_max;
        }

#if defined(BCM_TRIUMPH2_SUPPORT) 
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                SOC_IS_VALKYRIE2(unit) || SOC_IS_TRIDENT(unit) ||
                SOC_IS_KATANA(unit)) {

            rv = bcm_tr2_ipmc_repl_list_split(unit, temp_encap_count,
                    temp_encap_array, sublist_id_array, 
                    sublist_max, &sublist_count);
            if (BCM_FAILURE(rv)) {
                goto done;
            }
        } 
#endif  /* BCM_TRIUMPH2_SUPPORT */

        /* If the number of sublists used, given by sublist_count,
         * is less than the max number of sublists allowed,
         * given by sublist_max, the encap ids that are not yet
         * a member of any sublist are assigned to the remaining
         * free sublists.
         * If there are no free sublist left, the encap ids that are
         * not yet a member of any sublist are assigned to an
         * existing sublist selected by hash.
         */
        if (sublist_count < sublist_max) {
            first_free_sublist_id = sublist_count;
            next_free_sublist_id = first_free_sublist_id;
            for (j = 0; j < temp_encap_count; j++) {
                if (sublist_id_array[j] == sublist_max) {
                    sublist_id_array[j] = next_free_sublist_id;
                    if ((next_free_sublist_id + 1) == sublist_max) {
                        /* wrap around to the first free sublist id */
                        next_free_sublist_id = first_free_sublist_id;
                    } else {
                        next_free_sublist_id++;
                    }
                }
            }
        } else if (sublist_count == sublist_max) {
            selected_sublist_id = hash % sublist_max;
            for (j = 0; j < temp_encap_count; j++) {
                if (sublist_id_array[j] == sublist_max) {
                    sublist_id_array[j] = selected_sublist_id;
                }
            }
        } else {
            /* sublist_count should never be greater than sublist_max */
            rv = BCM_E_INTERNAL;
            goto done;
        }

        /* The sublists will be assigned to trunk members in a round-robin
         * fashion, with the starting trunk member selected by hash.
         * Sublist 0 will be assigned to a local trunk member selected
         * by hashing. Then sublist 1 will be assigned to the
         * next trunk member in local_trunk_port_array, and so on.
         */
        sublist0_trunk_member_index = hash % num_local_trunk_ports;
        temp_encap_index = 0;
        for (j = i; j < port_count; j++) {
            if (!BCM_GPORT_IS_TRUNK(mapped_port_array[j])) {
                continue;
            }
            if (BCM_IF_INVALID == encap_id_array[j]) {
                continue;
            }
            if (BCM_GPORT_TRUNK_GET(mapped_port_array[j]) != trunk_id) {
                continue;
            }
            if (temp_encap_array[temp_encap_index] != encap_id_array[j]) {
                /* They should always match */
                rv = BCM_E_INTERNAL;
                goto done;
            }

            trunk_member_index = (sublist0_trunk_member_index +
                sublist_id_array[temp_encap_index]) % num_local_trunk_ports;

            mapped_port = local_trunk_port_array[trunk_member_index];
            rv = bcm_esw_port_gport_get(unit, mapped_port,
                    &mapped_port_array[j]);
            if (BCM_FAILURE(rv)) {
                goto done;
            }
            temp_encap_index++;
        }
    }

done:
    if (hash_key) {
        sal_free(hash_key);
    }
    if (temp_encap_array) {
        sal_free(temp_encap_array);
    }
    if (sublist_id_array) {
        sal_free(sublist_id_array);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_multicast_l3_group_check
 * Purpose:
 *      Helper function to validate L3 or Virtual multicast group
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      is_l3     - (OUT) L3 Group identifier for Firebolt
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_esw_multicast_l3_group_check(int unit, bcm_multicast_t group, int *is_l3)
{
    if (_BCM_MULTICAST_IS_L3(group) 
        || _BCM_MULTICAST_IS_TRILL(group)
        || _BCM_MULTICAST_IS_FLOW(group)
        || _BCM_MULTICAST_IS_VXLAN(group)) {
        if (NULL != is_l3) {    /* If caller cares about this information */
            *is_l3 = 1;
        }        
    } else if (!SOC_IS_TRX(unit)) {
        /* Pre TX devices don't support Virutal multicast groups */
        return (BCM_E_PARAM);
    } else if (!_BCM_MULTICAST_IS_VPLS(group) &&
               !_BCM_MULTICAST_IS_MIM(group) &&
               !_BCM_MULTICAST_IS_WLAN(group) &&
               !_BCM_MULTICAST_IS_VLAN(group) &&
               !_BCM_MULTICAST_IS_NIV(group) &&
               !_BCM_MULTICAST_IS_L2GRE(group) &&
               !_BCM_MULTICAST_IS_SUBPORT(group) &&
               !_BCM_MULTICAST_IS_EGRESS_OBJECT(group) &&
               !_BCM_MULTICAST_IS_EXTENDER(group)) {
        return (BCM_E_PARAM);
    }

    return (BCM_E_NONE);
}

#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
typedef struct _bcm_trunk_first_member_s {
    int trunk_id;
    int pipe_id;
    int first_member;
} _bcm_trunk_first_member_t;


/*
 * Function:
 *      _bcm_esw_ipmc_egress_intf_destroy_in_trunk_repl_mode
 * Purpose:
 *      Destory ipmc egress intf of given ports which are represented by e_pbmp.
 * Parameters:
 *      unit           - (IN) Device Number
 *      mc_index       - (IN) Multicast group ID.
 *      e_pbmp         - (IN) Port bitmap
 *      is_l3          - (IN) Indicates if multicast group type is IPMC.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_ipmc_egress_intf_destroy_in_trunk_repl_mode(int unit,
    int mc_index, bcm_pbmp_t e_pbmp, int is_l3)
{
    bcm_trunk_t tgid;
    _bcm_trunk_first_member_t *first_member_array;
    bcm_port_t first_member, port_iter;
    int count, i, is_first_member, pipe;
    int rv;
    bcm_port_t port, phy_port;
    int num_ports = 0;

    first_member_array =
        sal_alloc(sizeof(_bcm_trunk_first_member_t) * SOC_MAX_NUM_PORTS,
                  "first_member_array pointers");
    if (NULL == first_member_array) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        first_member_array[i].trunk_id = BCM_TRUNK_INVALID;
        first_member_array[i].first_member = -1;
        first_member_array[i].pipe_id = -1;
    }

    count = 0;


    /*
     * In case of COE, the access ports on line card connected
     * to this device are not included in this device local front
     * panel port bitmap. Therefore we need to iterate over the local
     * front panel ports as well as the line card access ports for
     *  multicast membership related operations.
     */
    num_ports = BCM_PBMP_NUM_PORTS(unit);

    for (port_iter=0; port_iter < num_ports; port_iter++) {
        port = port_iter;
        phy_port = port_iter;
        /* BCM_PBMP_ITER(mc_eligible_pbmp, port_iter) {*/
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
            /*
             * if this is a subport then add all the encap ids of subport
             * in same list.
             * Same is done for physical ports.
             */
            if (soc_feature(unit, soc_feature_channelized_switching)) {
                if (port_iter < FB6_NUM_PORT) {
                    if (!(BCM_PBMP_MEMBER(e_pbmp, port_iter))) {
                        continue;
                    }
                }
                _bcm_coe_subtag_subport_num_subport_gport_get(unit, port_iter, &port);
                if (!_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
                    /* This is not a valid subport. continue;*/
                    continue;
                }
                rv = _bcmi_coe_subport_physical_port_get(unit, port, &phy_port);
                if (BCM_FAILURE(rv)) {
                    sal_free(first_member_array);
                    return rv;
                }
            } else
#endif
            {
                if (!(BCM_PBMP_MEMBER(e_pbmp, port_iter))) {
                    continue;
                }
            }

/*    BCM_PBMP_ITER(e_pbmp, port_iter) {*/
        rv = _bcm_esw_ipmc_port_trunkid_get(unit, port, &tgid);
        if (BCM_FAILURE(rv)) {
            sal_free(first_member_array);
            return rv;
        }
        if (tgid != BCM_TRUNK_INVALID) {
            is_first_member = TRUE;
            first_member = -1;
            pipe = SOC_INFO(unit).port_pipe[phy_port];
            for (i = 0; i < count; i++) {
                if ((first_member_array[i].trunk_id == tgid) &&
                    (first_member_array[i].pipe_id == pipe)) {
                    is_first_member = FALSE;
                    first_member = first_member_array[i].first_member;
                    break;
                }
            }
            if (i == count) {
                first_member_array[count].trunk_id = tgid;
                first_member_array[count].pipe_id = pipe;
                first_member_array[count].first_member = port;
                is_first_member = TRUE;
                count++;
            }

            if (is_first_member) {
                rv = _bcm_esw_ipmc_egress_intf_set_for_trunk_first_member(
                         unit, mc_index, port, 0, NULL,
                         is_l3, FALSE, tgid);
            } else {
                rv = _bcm_esw_ipmc_egress_intf_set_for_same_pipe_member(
                         unit, mc_index, port,
                         first_member, BCM_TRUNK_INVALID);
            }
        } else {
            rv = _bcm_esw_ipmc_egress_intf_set_for_trunk_first_member(
                     unit, mc_index, port,0, NULL,
                     is_l3, FALSE, BCM_TRUNK_INVALID);
        }

        if (BCM_FAILURE(rv)) {
            sal_free(first_member_array);
            return rv;
        }
    }

    sal_free(first_member_array);

    return BCM_E_NONE;
}
#endif

#ifdef BCM_TRIDENT2_SUPPORT
static int
_bcm_esw_multicast_l3_mpls_nh_check(int unit, int nh, int* is_l3_mpls)
{
    int rv = 0;
    int entry_type = 0;
    soc_field_t entry_type_field = 0;
    egr_l3_next_hop_entry_t egr_entry;
    *is_l3_mpls = 0;

    if (SOC_IS_TRIDENT3X(unit)) {
        entry_type_field = DATA_TYPEf;
    } else {
        entry_type_field = ENTRY_TYPEf;
    }

    if (SOC_MEM_IS_VALID(unit, EGR_L3_NEXT_HOPm)) {
        if (!BCM_XGS3_L3_ENT_REF_CNT(BCM_XGS3_L3_TBL_PTR(unit, next_hop), nh)) {
            return BCM_E_NOT_FOUND;
        }

        sal_memset(&egr_entry, 0, sizeof(egr_l3_next_hop_entry_t));
        rv = BCM_XGS3_MEM_READ(unit, EGR_L3_NEXT_HOPm, nh, &egr_entry);
        if (rv != BCM_E_NONE) {
            return rv;
        }

        if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, entry_type_field)) {
            entry_type = soc_mem_field32_get(
                unit, EGR_L3_NEXT_HOPm, &egr_entry, entry_type_field);
            if (entry_type == 1) {
                *is_l3_mpls = 1;
            }
        }
    }

    return BCM_E_NONE;
}
#endif

#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT
/*
 * Function:
 *      _bcm_esw_multicast_trunk_local_members_sanitize
 * Purpose:
 *      sanitize local member ports of a trunk group.
 * Parameters:
 *      unit       - (IN) Device Number
 *      local_member_array - (IN) Array of local trunk members
 *      local_member_count - (IN) Number of local trunk members
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If there is any subport is added to trunk then
 *      error is returned.
 */

int
_bcm_esw_multicast_trunk_local_members_sanitize(int unit,
                                 bcm_port_t *member_array,
                                 int local_member_count)
{
    int i=0;

    for (; i<local_member_count; i++) {
        if (BCM_GPORT_IS_SET(member_array[i])) {
            if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, member_array[i])) {
                LOG_ERROR(BSL_LS_BCM_MULTICAST,
                    (BSL_META_U(unit,"trunk added in group has subport.\n")));

                return BCM_E_PARAM;
            }
        }
    }
    return BCM_E_NONE;
}
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

/*
 * Function:
 *      _bcm_esw_multicast_l3_add
 * Purpose:
 *      Helper function to add a port to L3 or Virtual multicast group
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_esw_multicast_l3_add(int unit, bcm_multicast_t group, 
                          bcm_gport_t port, bcm_if_t encap_id)

{
    int mc_index, is_l3, gport_id, idx, idx_max, trunk_local_ports = 0; 
    bcm_module_t mod_out;
    bcm_port_t port_out, trunk_local_port;
    bcm_port_t port_gport = port;
    bcm_trunk_t trunk_id;
    int modid_local, rv = BCM_E_NONE;
    int is_subport_trunk = FALSE;
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t      local_trunk_member_ports[SOC_MAX_NUM_PP_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t      local_trunk_member_ports[SOC_MAX_NUM_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif
    bcm_pbmp_t l2_pbmp, l3_pbmp, old_pbmp, *new_pbmp;
    int if_max = BCM_MULTICAST_PORT_MAX;

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    int is_local_subport = FALSE;
#endif
#if defined(BCM_HGPROXY_COE_SUPPORT)
    bcm_trunk_t          subport_trunk_id;
    bcm_trunk_member_t   *trunk_member = NULL;
    bcm_port_t           local_port;
    int                  member_count;
    int                  is_local;
#endif

#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    /* Size of "L3 Next hop table is limited to 2K on dagger(Enduro variant).
     * On Ranger+ max replication interface is limited by
     * number of EGR_L3_Intfm.
     */

    int nexthop_count = 0, egr_intf_count = 0;
    if (SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH3(unit)) {
        nexthop_count =  soc_mem_index_count(unit, ING_L3_NEXT_HOPm);
        egr_intf_count = soc_mem_index_count(unit, EGR_L3_INTFm);
        if_max = (nexthop_count < egr_intf_count)?
             ((if_max < nexthop_count)? if_max : nexthop_count):
             ((if_max < egr_intf_count)?  if_max : egr_intf_count);
    }
#endif /* BCM_ENDURO_SUPPORT || BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_HURRICANE_SUPPROT) || defined(BCM_HURRICANE2_SUPPORT) || \
    defined(BCM_GREYHOUND_SUPPORT)
    if (SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit) || \
        SOC_IS_GREYHOUND2(unit)) {
        /* Assign proper if_max for HURRICANEx and GH since if_max is 4096 
         * by default.         */
        if_max = soc_mem_index_count(unit, EGR_L3_INTFm);
    }
#endif  /* BCM_HURRICANE_SUPPORT || BCM_HURRICANE2_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        /* To assign proper if_max to avoid getting unexpected parameter
         * validation error since if_max is 4096 by default.
         */
        if_max = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    }
#endif

    is_l3 = 0;      /* Needed to support Firebolt style of L3 Multicast */
    trunk_local_ports = 0; 
    modid_local = FALSE;

    mc_index = _BCM_MULTICAST_ID_GET(group);

    if ((mc_index < 0) ||
        (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_multicast_l3_group_check(unit, group, &is_l3));

#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_trill)) {
        if (_BCM_MULTICAST_IS_TRILL(group)) {
             BCM_IF_ERROR_RETURN
                (bcm_td_trill_egress_add(unit, group));
        }
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, port, &mod_out, &port_out,
                               &trunk_id, &gport_id));

    /* In case of extended queuing on KTX, trunk_id gets updated with
     * extended queue value. So set it to invalid value.
     */
#ifdef BCM_KATANA_SUPPORT
    if (soc_feature(unit, soc_feature_extended_queueing)) {
        if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) ||
            (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port)) ||
            (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port))) {
            trunk_id = BCM_TRUNK_INVALID;
        }
    }
#endif

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_is_local_subport(
            unit, port, mod_out, port_out, &is_local_subport, &port_out));

    if (is_local_subport) {
        /* Local subport assigned above */
    } else
#endif

    if (BCM_TRUNK_INVALID != trunk_id) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_local_members_get(unit, trunk_id,
                                     max_num_ports,
                                     local_trunk_member_ports,
                                     &trunk_local_ports));

#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT
        if (soc_feature(unit, soc_feature_channelized_switching)) {
            /* Subport trunks can not be added to multicast group. */
            BCM_IF_ERROR_RETURN(
                _bcm_esw_multicast_trunk_local_members_sanitize(unit,
                                    local_trunk_member_ports,
                                    trunk_local_ports));
        }
#endif

    }
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
   else if ((BCM_MC_PER_TRUNK_REPL_MODE(unit)) && 
            (encap_id == BCM_ENCAP_TRUNK_MEMBER)) {

            port_gport = port_out;
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
            if ((soc_feature(unit, soc_feature_channelized_switching)) &&
                BCM_GPORT_IS_SET(port) &&
                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
                port_gport = port;
            }
#endif

        BCM_IF_ERROR_RETURN(
            _bcm_esw_ipmc_egress_intf_add_trunk_member(
                unit, mc_index, port_gport));
    }
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */
    else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod_out, &modid_local));

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if ((!modid_local) &&
              soc_feature(unit, soc_feature_multi_next_hops_on_port) &&
              _bcm_esw_is_multi_nexthop_mod_port(unit, mod_out, port_out)) {
            bcm_gport_t tmp_gport;

            BCM_GPORT_MODPORT_SET(tmp_gport, mod_out, port_out);
            BCM_IF_ERROR_RETURN(
               _bcm_esw_modport_local_get(unit, tmp_gport, &port_out));
            modid_local = TRUE;
        }
#endif /* defined(BCM_TRIDENT2PLUS_SUPPORT) */

        if (TRUE != modid_local) {
            /* Only add this to replication set if destination is local */
            return BCM_E_PORT;
        }

        /* Convert system port to physical port */
        if (soc_feature(unit, soc_feature_sysport_remap)) {
            BCM_XLATE_SYSPORT_S2P(unit, &port_out);
        }
    }

#ifdef BCM_TRIUMPH3_SUPPORT 
    if (SOC_IS_TRIUMPH3(unit)) {
        int use_axp_port = 0;
        bcm_l3_egress_t l3_egr;

        if (_BCM_MULTICAST_IS_WLAN(group)) {
            /* Triumph3 requires WLAN replications to be configured on
             * AXP port.
             */
            use_axp_port = 1;
        } else if (_BCM_MULTICAST_IS_L3(group)) {
            /* Triumph3 requires IPMC replications destined for WLAN
             * virtual ports to be configured on AXP port.
             */
            if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, encap_id)) {

                bcm_l3_egress_t_init(&l3_egr);
                rv = bcm_esw_l3_egress_get(unit, encap_id, &l3_egr);
                if (BCM_SUCCESS(rv)) { 
                    if (BCM_GPORT_IS_WLAN_PORT(l3_egr.port)) {
                        use_axp_port = 1;
                    }
                }
            }
        }

        if (use_axp_port) {
            trunk_id = BCM_TRUNK_INVALID;
            port_out = AXP_PORT(unit, SOC_AXP_NLF_WLAN_ENCAP);
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(INCLUDE_XFLOW_MACSEC)
    if (soc_feature(unit, soc_feature_xflow_macsec)) {
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            bcm_xflow_macsec_port_info_t port_info;
            if(BCM_E_NONE == bcm_common_xflow_macsec_port_info_get
                    (unit, port, &port_info)) {
                if(port_info.enable) {
                    /* Macsec IPMC requires macsec port in
                     * IPMC tables.
                     */
                    port_out = 65 + port_info.encrypt_port_offset;
                }
            }
        }
#endif
#if defined(BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit)) {
            bcm_l3_egress_t l3_egr;
            if (_BCM_MULTICAST_IS_L3(group)) {
                bcm_l3_egress_t_init(&l3_egr);
                rv = bcm_esw_l3_egress_get(unit, encap_id, &l3_egr);
                if (BCM_SUCCESS(rv)) {
                    if ((l3_egr.flags2 & BCM_L3_FLAGS2_XFLOW_MACSEC_ENCRYPT) ||
                            (l3_egr.flags2 & BCM_L3_FLAGS2_XFLOW_MACSEC_DECRYPT)) {
                        /* Macsec IPMC requires macsec port (port 66) in
                         * IPMC tables.
                         */
                        port_out = 66;
                    }
                }
            }
        }
#endif
    }
#endif

    if (BCM_IF_INVALID != encap_id) {

#ifdef BCM_TRIDENT2_SUPPORT
        /* if this is a overlay NH enap id, then it
         * is part of RioT flow.
         * As part of that flow or L2 tunnel encap flow,
         * the outer VPLS header will
         * be added by underlay NH. No need to add anything from
         * egress nh. so clear l3_payload field.
         */
        if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, encap_id)) {
            int is_l3_mpls = 0;
            int nh = encap_id - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

            if (_BCM_MULTICAST_IS_L3(group) ||
                _BCM_MULTICAST_IS_EGRESS_OBJECT(group)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_multicast_l3_mpls_nh_check(unit, nh, &is_l3_mpls));
            }

            if (!is_l3_mpls) {
                int local_mc_idx = 0;
                egr_ipmc_entry_t   egr_ipmc_entry;
                local_mc_idx = _BCM_MULTICAST_ID_GET(mc_index);

                BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_IPMCm, MEM_BLOCK_ANY,
                    local_mc_idx, &egr_ipmc_entry));
                soc_mem_field32_set(unit, EGR_IPMCm, &egr_ipmc_entry,
                    L3_PAYLOADf, 0);
                BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_IPMCm,
                    MEM_BLOCK_ALL, local_mc_idx, &egr_ipmc_entry));
            }
        }
#endif

        /* Add all local ports in trunk to the multicast egress interface */
        if (BCM_TRUNK_INVALID != trunk_id) {
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_ipmc_egress_intf_add_for_trunk(
                        unit, mc_index, trunk_id, encap_id, is_l3));
            } else
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */
            {
                idx_max = trunk_local_ports;
                for (idx = 0; idx < idx_max; idx++) {
                    rv = _bcm_esw_ipmc_egress_intf_add(unit, mc_index,
                                                   local_trunk_member_ports[idx],
                                                   encap_id, is_l3);
                    if (BCM_FAILURE(rv)) {
                        while (idx--) {
                            /* Error code ignored because we already in error */
                            (void) _bcm_esw_ipmc_egress_intf_delete(unit,mc_index,
                                                        local_trunk_member_ports[idx],
                                                        if_max, encap_id, is_l3);
                        }
                        return rv;
                    }
                }
            }
        } else {
            /* Add  the required port to the multicast egress interface */
            port_gport = port_out;
#if defined(BCM_HGPROXY_COE_SUPPORT)
            if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                (soc_feature(unit, soc_feature_channelized_switching))) &&
                BCM_GPORT_IS_SET(port) &&
                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
                port_gport = port;
            }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_ipmc_egress_intf_add_in_per_trunk_mode(
                        unit, mc_index, port_gport, encap_id, is_l3));
            } else
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_ipmc_egress_intf_add(unit, mc_index, port_gport,
                                                  encap_id, is_l3));
            }
        }
        new_pbmp = &l3_pbmp;
    } else {
        /* Updating the L2 bitmap */
        new_pbmp = &l2_pbmp;
    }

    /* Add port to the IPMC_L3_BITMAP or L2_BITMAP as decided above */
    rv = _bcm_esw_multicast_ipmc_read(unit, mc_index, &l2_pbmp, &l3_pbmp);

    if (BCM_SUCCESS(rv)) {
        BCM_PBMP_ASSIGN(old_pbmp, *new_pbmp);
        if (BCM_TRUNK_INVALID != trunk_id) {
            for (idx = 0; idx < trunk_local_ports; idx++) {
                trunk_local_port = local_trunk_member_ports[idx];
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
                if ((soc_feature(unit, soc_feature_channelized_switching)) &&
                    BCM_GPORT_IS_SET(port) &&
                    _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit,
                     trunk_local_port)) {

                    BCM_IF_ERROR_RETURN(
                        _bcmi_coe_subport_physical_port_get(unit,
                        trunk_local_port,
                        &trunk_local_port));
                    ((BCMI_MC_GROUP_PORT_REFCOUNT(unit, mc_index, trunk_local_port))++);
                }
#endif
                BCM_PBMP_PORT_ADD(*new_pbmp, trunk_local_port);
            }
        } else {
            BCM_PBMP_PORT_ADD(*new_pbmp, port_out);
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
            if (soc_feature(unit, soc_feature_channelized_switching)) {
                ((BCMI_MC_GROUP_PORT_REFCOUNT(unit, mc_index, port_out))++);
            }
#endif
        }
#if defined (BCM_HGPROXY_COE_SUPPORT)
        /*
         * If bcm_vlan_control_vlan_set is called before this API, L2_BITMAP
         * may be intialized with the port bitmap including subtending ports.
         * The subtending ports need to be deleted from the L2_BITMAP here.
         */
        if (is_local_subport) {
            rv = bcm_esw_trunk_find(unit, 0, port, &subport_trunk_id);
            if (BCM_SUCCESS(rv)) {
                trunk_member = sal_alloc(sizeof(bcm_trunk_member_t) *
                                         SOC_MAX_NUM_PORTS, "trunk member");
                if (NULL == trunk_member) {
                    return BCM_E_MEMORY;
                }
                rv = bcm_esw_trunk_get(unit, subport_trunk_id,
                                       NULL, SOC_MAX_NUM_PORTS,
                                       trunk_member, &member_count);
                if (BCM_FAILURE(rv)) {
                    sal_free(trunk_member);
                    return rv;
                }

                for (idx = 0; idx < member_count; idx++) {
                    rv = _bcm_esw_modid_is_local(unit,
                                            _BCM_SUBPORT_COE_PORT_ID_MOD_GET(
                                            trunk_member[idx].gport),
                                            &is_local);
                    if (BCM_FAILURE(rv)) {
                        sal_free(trunk_member);
                        return rv;
                    }
                    if (!is_local) {
                        continue;
                    }
                    rv = bcm_esw_port_local_get(unit,
                                                trunk_member[idx].gport,
                                                &local_port);
                    if (BCM_FAILURE(rv)) {
                        sal_free(trunk_member);
                        return rv;
                    }
                    BCM_PBMP_PORT_REMOVE(l2_pbmp, local_port);
                }
                is_subport_trunk = TRUE;
                if (NULL != trunk_member) {
                    sal_free(trunk_member);
                }
            } else {
                rv = BCM_E_NONE;
                is_subport_trunk = FALSE;
            }
        }
#endif
        if (BCM_PBMP_NEQ(old_pbmp, *new_pbmp) ||
            is_subport_trunk) {
            rv = _bcm_esw_multicast_ipmc_write(unit, mc_index, l2_pbmp, 
                                               l3_pbmp, TRUE);
        }
    } 
    if (BCM_FAILURE(rv) && (encap_id != BCM_IF_INVALID)) {
        if (BCM_TRUNK_INVALID != trunk_id) {
#if defined (BCM_TOMAHAWK_SUPPORT) || defined (BCM_TRIDENT2PLUS_SUPPORT)
            if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_ipmc_egress_intf_del_for_trunk(
                        unit, mc_index, trunk_id, if_max, encap_id, is_l3));
            } else
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */
            {
                idx_max = trunk_local_ports;
                for (idx = 0; idx < idx_max; idx++) {
                    (void) _bcm_esw_ipmc_egress_intf_delete(unit,
                                 mc_index, local_trunk_member_ports[idx],
                                 if_max, encap_id, is_l3);
                }
            }
        } else {

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
            if ((soc_feature(unit, soc_feature_channelized_switching)) &&
                BCM_GPORT_IS_SET(port) &&
                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
                port_out = port;
            }
#endif

#if defined (BCM_TOMAHAWK_SUPPORT) || defined (BCM_TRIDENT2PLUS_SUPPORT)
            if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_ipmc_egress_intf_del_in_per_trunk_mode(
                        unit, mc_index, port_gport, if_max, encap_id, is_l3));
            } else
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */
            {
               (void) _bcm_esw_ipmc_egress_intf_delete(unit, mc_index, port_gport,
                                   if_max, encap_id, is_l3);
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_multicast_l3_delete
 * Purpose:
 *      Helper function to delete a port from L3 or Virtual multicast group
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_esw_multicast_l3_delete(int unit, bcm_multicast_t group, 
                             bcm_gport_t port, bcm_if_t encap_id)

{
    int mc_index, is_l3, gport_id, i, idx, idx_max, trunk_local_ports = 0; 
    bcm_module_t mod_out;
    bcm_port_t port_out, trunk_local_port;
    bcm_trunk_t trunk_id;
    int modid_local, local_count, port_count, rv = BCM_E_NONE; 
    bcm_port_t    temp_port = BCM_GPORT_INVALID;
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t    local_trunk_member_ports[SOC_MAX_NUM_PP_PORTS] = {0};
    int           max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t    local_trunk_member_ports[SOC_MAX_NUM_PORTS] = {0};
    int           max_num_ports = SOC_MAX_NUM_PORTS;
#endif
    bcm_pbmp_t l2_pbmp, l3_pbmp, old_pbmp;
    bcm_gport_t *port_array;
    bcm_if_t *encap_array;
    int if_max = BCM_MULTICAST_PORT_MAX;
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    bcm_port_t port_gport;
#endif

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    int is_local_subport = FALSE;
#endif
    int l3_pbm_remove = 1;
#if defined(BCM_ENDURO_SUPPORT)
    /* Size of "L3 Next hop table is limited to 2K on dagger(Enduro variant).
     * On Ranger+ max replication interface is limited by 
     * number of EGR_L3_Intfm.
     */

    int nexthop_count = 0, egr_intf_count = 0;
    if ((SOC_IS_ENDURO(unit) || (SOC_IS_METROLITE(unit)))) {
        nexthop_count =  soc_mem_index_count(unit, ING_L3_NEXT_HOPm);
        egr_intf_count = soc_mem_index_count(unit, EGR_L3_INTFm);
        if_max = (nexthop_count < egr_intf_count)?
                 ((if_max < nexthop_count)? if_max : nexthop_count):
                 ((if_max < egr_intf_count)? if_max : egr_intf_count);
    }
#endif /* BCM_ENDURO_SUPPORT || BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        if_max = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    }
#endif

#if defined(BCM_HURRICANE_SUPPROT) || defined(BCM_HURRICANE2_SUPPORT)
    if (SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit)) {
        /* To assign proper if_max to avoid getting unexpected parameter
         * validation error since if_max is 4096 by default.
         */
        if_max = soc_mem_index_count(unit, EGR_L3_INTFm);
    }
#endif  /* BCM_HURRICANE_SUPPORT || BCM_HURRICANE2_SUPPORT */
#ifdef BCM_GREYHOUND_SUPPORT
    if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
        /* To assign proper if_max to avoid delete operation got unexpect 
         * parameter validation error due to if_max is default at 4096.
         *
         * GH has egress logic changed for IPMC replication to reach 
         *  EGR_L3_NEXT_HOPm before EGR_L3_INTFm. But the maximum replication 
         *  still limited by EGR_L3_INTFm.
         */
        if_max = soc_mem_index_count(unit, EGR_L3_INTFm);
    }
#endif /* BCM_GREYHOUND_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        if_max = TH3_IPMC_MAX_INTF_COUNT;
    } else
#endif
    if (SOC_IS_TD2_TT2(unit)) {
        /* To assign proper if_max to avoid getting unexpected parameter
         * validation error since if_max is 4096 by default.
         */
        if_max = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    }
#endif

    is_l3 = 0;      /* Needed to support Firebolt style of L3 Multicast */
    trunk_local_ports = 0; 
    modid_local = FALSE;
    port_count = local_count = 0;

    mc_index = _BCM_MULTICAST_ID_GET(group);

    if ((mc_index < 0) ||
        (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_multicast_l3_group_check(unit, group, &is_l3));

#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_trill)) {
        if (_BCM_MULTICAST_IS_TRILL(group)) {
             BCM_IF_ERROR_RETURN
                (bcm_td_trill_egress_delete(unit, group));
        }
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, port, &mod_out, &port_out,
                               &trunk_id, &gport_id));

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_is_local_subport(
            unit, port, mod_out, port_out, &is_local_subport, &port_out));

    if (is_local_subport) {
        /* Local subport assigned above */
    } else
#endif

    if (BCM_TRUNK_INVALID != trunk_id) {
        /* system ports to physical ports conversion done in
         * _bcm_esw_trunk_local_members_get
         */

        BCM_IF_ERROR_RETURN(
                _bcm_esw_trunk_local_members_get(unit, trunk_id,
                                         max_num_ports,
                                         local_trunk_member_ports,
                                         &trunk_local_ports));
    }

#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
   else if ((BCM_MC_PER_TRUNK_REPL_MODE(unit)) &&
            (encap_id == BCM_ENCAP_TRUNK_MEMBER)) {
            port_gport = port_out;
#if defined(BCM_HGPROXY_COE_SUPPORT)
            if ((soc_feature(unit, soc_feature_channelized_switching)) &&
                BCM_GPORT_IS_SET(port) &&
                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
                port_gport = port;
            }
#endif

        BCM_IF_ERROR_RETURN(
            _bcm_esw_ipmc_egress_intf_del_trunk_member(
                unit, mc_index, port_gport));
    }
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */
    else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod_out, &modid_local));

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if ((!modid_local) &&
              soc_feature(unit, soc_feature_multi_next_hops_on_port) &&
              _bcm_esw_is_multi_nexthop_mod_port(unit, mod_out, port_out)) {
            bcm_gport_t tmp_gport;

            BCM_GPORT_MODPORT_SET(tmp_gport, mod_out, port_out);
            BCM_IF_ERROR_RETURN(
               _bcm_esw_modport_local_get(unit, tmp_gport, &port_out));
            modid_local = TRUE;
        }
#endif /* defined(BCM_TRIDENT2PLUS_SUPPORT) */

        if (TRUE != modid_local) {
            /* Only add this to replication set if destination is local */
            return BCM_E_PORT;
        }

        /* Convert system port to physical port */
        if (soc_feature(unit, soc_feature_sysport_remap)) {
            BCM_XLATE_SYSPORT_S2P(unit, &port_out);
        }
    }

#ifdef BCM_TRIUMPH3_SUPPORT 
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANA2(unit)) {
        if (_BCM_MULTICAST_IS_WLAN(group)) {
            trunk_id = BCM_TRUNK_INVALID;
            port_out = AXP_PORT(unit, SOC_AXP_NLF_WLAN_ENCAP);
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    if (encap_id == BCM_IF_INVALID) {
        /* Remove L2 port */

        soc_mem_lock(unit, L3_IPMCm);
        /* Delete the port from the IPMC L2_BITMAP */
        rv = _bcm_esw_multicast_ipmc_read(unit, mc_index, 
                                          &l2_pbmp, &l3_pbmp);
        if (BCM_SUCCESS(rv)) {
            BCM_PBMP_ASSIGN(old_pbmp, l2_pbmp);
            if (BCM_TRUNK_INVALID != trunk_id) {
                for (idx = 0; idx < trunk_local_ports; idx++) {
                    trunk_local_port = local_trunk_member_ports[idx];
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
                    if ((soc_feature(unit, soc_feature_channelized_switching))
                         && BCM_GPORT_IS_SET(port) &&
                        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(
                            unit, trunk_local_port)) {

                        BCM_IF_ERROR_RETURN(
                            _bcmi_coe_subport_physical_port_get(unit,
                            trunk_local_port, &trunk_local_port));
                    }
#endif
                    BCM_PBMP_PORT_REMOVE(l2_pbmp, trunk_local_port);
                }
            } else {
                BCM_PBMP_PORT_REMOVE(l2_pbmp, port_out);
            }

            if (BCM_PBMP_NEQ(old_pbmp, l2_pbmp)) {
                rv = _bcm_esw_multicast_ipmc_write(unit, mc_index,
                                        l2_pbmp, l3_pbmp, TRUE);
            } else if (BCM_TRUNK_INVALID == trunk_id) {
                /* trying to remove members of an empty trunk is acceptable */
                rv = BCM_E_NOT_FOUND;
            }
        }
        soc_mem_unlock(unit, L3_IPMCm);
        return rv;
    }

    /*
     * Walk through the list of egress interfaces for this group.
     * Check if the interface getting deleted is the ONLY
     * instance of the local port in the replication list.
     * If so, we can delete the port from the IPMC L3_BITMAP.
     */
    port_array = sal_alloc(if_max * sizeof(bcm_gport_t),
                           "mcast port array");
    if (port_array == NULL) {
        return BCM_E_MEMORY;
    }
    encap_array = sal_alloc(if_max * sizeof(bcm_if_t),
                            "mcast encap array");
    if (encap_array == NULL) {
        sal_free (port_array);
        return BCM_E_MEMORY;
    }

    rv = bcm_esw_multicast_egress_get(unit, group, if_max, 
                                      port_array, encap_array, &port_count);
    if (BCM_FAILURE(rv)) {
        sal_free (port_array);
        sal_free (encap_array);
        return (rv);
    } 

    for (i = 0; i < port_count ; i++) {
        if (encap_id == encap_array[i]) {
            /* Skip the one we're about to delete */
            continue;
        }

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
        is_local_subport = FALSE;
#endif

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            BCM_GPORT_IS_SET(port_array[i]) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_array[i])) {
                is_local_subport = TRUE;

            rv = _bcmi_coe_subport_physical_port_get(unit,
                                                     port_array[i],
                                                     &temp_port);
            if (BCM_FAILURE(rv)) {
                sal_free (port_array);
                sal_free (encap_array);
                return (rv);
            }
        } else
#endif
#if defined(BCM_KATANA2_SUPPORT)
        if (soc_feature(unit, soc_feature_linkphy_coe) ||
            soc_feature(unit, soc_feature_subtag_coe)) {
            if (BCM_GPORT_IS_SET(port_array[i]) &&
                _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit,
                port_array[i])) {
                is_local_subport = TRUE;
                temp_port = _BCM_KT2_SUBPORT_PORT_ID_GET(port_array[i]);
            }
        }
#endif

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
        if (is_local_subport) {
            /* Local subport assigned above */
        } else
#endif
        {
            (void) bcm_esw_port_local_get(unit, port_array[i], &temp_port);
        }

        if (temp_port == port_out) {
            local_count = 1;
            break;
        } else {
            for (idx = 0; idx < trunk_local_ports; idx++) {
                trunk_local_port = local_trunk_member_ports[idx];
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
                if ((soc_feature(unit, soc_feature_channelized_switching)) &&
                    BCM_GPORT_IS_SET(port) &&
                    _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, trunk_local_port)) {
                        BCM_IF_ERROR_RETURN(
                            _bcmi_coe_subport_physical_port_get(unit, trunk_local_port,
                                &trunk_local_port));
                }
#endif
                if (temp_port == trunk_local_port) {
                       local_count = 1;
                       break;
                    }
            }
            if (local_count) {
                break;
            }
        }
    }
    sal_free(port_array);
    sal_free(encap_array);

    if (!local_count) {
        soc_mem_lock(unit, L3_IPMCm);
        rv = _bcm_esw_multicast_ipmc_read(unit, mc_index, 
                                          &l2_pbmp, &l3_pbmp);
        if (BCM_SUCCESS(rv)) {
            if (BCM_TRUNK_INVALID != trunk_id) {
                for (idx = 0; idx < trunk_local_ports; idx++) {
                    trunk_local_port = local_trunk_member_ports[idx];
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
                    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
                        BCM_GPORT_IS_SET(port) &&
                        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit,
                        trunk_local_port)) {

                        BCM_IF_ERROR_RETURN(
                            _bcmi_coe_subport_physical_port_get(unit,
                            trunk_local_port, &trunk_local_port));

                        if (BCMI_MC_GROUP_PORT_REFCOUNT(unit, mc_index,
                            trunk_local_port)) {
                            ((BCMI_MC_GROUP_PORT_REFCOUNT(unit, mc_index,
                                trunk_local_port))--);
                            if (BCMI_MC_GROUP_PORT_REFCOUNT(unit, mc_index,
                                trunk_local_port) > 0) {
                                l3_pbm_remove = 0;
                            } else {
                                l3_pbm_remove = 1;
                            }
                        }
                    }
#endif

                    if (l3_pbm_remove) {
                        BCM_PBMP_PORT_REMOVE(l3_pbmp, trunk_local_port);
                    }
                }
            } else {
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
                    if (soc_feature(unit, soc_feature_channelized_switching)) {

                        if (BCMI_MC_GROUP_PORT_REFCOUNT(unit, mc_index,
                            port_out)) {
                            ((BCMI_MC_GROUP_PORT_REFCOUNT(unit, mc_index,
                                port_out))--);
                            if (BCMI_MC_GROUP_PORT_REFCOUNT(unit, mc_index,
                                port_out) > 0) {
                                 l3_pbm_remove = 0;
                            }
                        }
                    }
#endif

                if (l3_pbm_remove) {
                    BCM_PBMP_PORT_REMOVE(l3_pbmp, port_out);
                }
            }
            rv = _bcm_esw_multicast_ipmc_write(unit, mc_index, l2_pbmp,
                                               l3_pbmp, TRUE);
        }
        soc_mem_unlock(unit, L3_IPMCm);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (BCM_TRUNK_INVALID != trunk_id) {
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_ipmc_egress_intf_del_for_trunk(
                    unit, mc_index, trunk_id, if_max, encap_id, is_l3));
        } else
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */
        {
            idx_max = trunk_local_ports;
            for (idx = 0; idx < idx_max; idx++) {
                rv =  _bcm_esw_ipmc_egress_intf_delete(unit, mc_index,
                                    local_trunk_member_ports[idx], if_max,
                                                   encap_id, is_l3);
                if (rv < 0) {
                    while (idx--) {
                        (void) _bcm_esw_ipmc_egress_intf_add(unit, mc_index,
                                    local_trunk_member_ports[idx], encap_id, is_l3);
                    }
                    return rv;
                }
            }
         }
    } else {

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
            if ((soc_feature(unit, soc_feature_channelized_switching)) &&
                BCM_GPORT_IS_SET(port) &&
                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
                port_out = port;
            }
#endif

 #if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_ipmc_egress_intf_del_in_per_trunk_mode(
                    unit, mc_index, port_out, if_max, encap_id, is_l3));
        } else
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */
        {
            rv =  _bcm_esw_ipmc_egress_intf_delete(unit, mc_index, port_out,
                                               if_max, encap_id, is_l3);
        }
    }

    return rv;
}


/*
 * Function:
 *      _bcm_esw_multicast_l3_get
 * Purpose:
 *      Helper function to get ports that are part of L3 or Virtual multicast 
 * Parameters: 
 *      unit           - (IN) Device Number
 *      mc_index       - (IN) Multicast index
 *      port_max       - (IN) Number of entries in "port_array"
 *      port_array     - (OUT) List of ports
 *      encap_id_array - (OUT) List of encap identifiers
 *      port_count     - (OUT) Actual number of ports returned
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the input parameter port_max = 0, return in the output parameter
 *      port_count the total number of ports/encapsulation IDs in the 
 *      specified multicast group's replication list.
 */

STATIC int     
_bcm_esw_multicast_l3_get(int unit, bcm_multicast_t group, int port_max,
                             bcm_gport_t *port_array, bcm_if_t *encap_id_array, 
                             int *port_count)
{
    int             mc_index, i, count, rv;
    bcm_pbmp_t      ether_higig_pbmp, l2_pbmp, l3_pbmp;
    bcm_port_t      port_iter, port;
    bcm_if_t        *local_id_array;
#if defined(BCM_KATANA2_SUPPORT)
    int is_local_subport = FALSE;
#endif
    int num_ports = 0;

    i= 0;
    rv = BCM_E_NONE;
    local_id_array = NULL;

    mc_index = _BCM_MULTICAST_ID_GET(group);
    if ((mc_index < 0) ||
        (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_multicast_l3_group_check(unit, group, NULL));

    if (port_max > 0) {
        local_id_array = sal_alloc(sizeof(bcm_if_t) * port_max,
                                   "local array of interfaces");
        if (NULL == local_id_array) {
            return BCM_E_MEMORY;
        }
        sal_memset(local_id_array, 0x00, sizeof(bcm_if_t) * port_max);
    }

    /* Collect list of GPORTs for each front-panel and Higig port. */
    *port_count = 0;
    BCM_PBMP_ASSIGN(ether_higig_pbmp, PBMP_E_ALL(unit));
    BCM_PBMP_OR(ether_higig_pbmp, PBMP_HG_ALL(unit));

    /* CPU ports are valid for replication on TD2 */
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
        BCM_PBMP_OR(ether_higig_pbmp, PBMP_CMIC(unit));
    }
#if defined(INCLUDE_XFLOW_MACSEC)
    if (soc_feature(unit, soc_feature_xflow_macsec)) {
#ifdef BCM_HURRICANE4_SUPPORT
        /* MCASEC ports are valid for replication on HR4 */
        if (SOC_IS_HURRICANE4(unit) ) {
            BCM_PBMP_OR(ether_higig_pbmp, PBMP_MACSEC_ALL(unit));
        }
#endif
    }
#endif
    if (soc_feature(unit, soc_feature_axp)) {
        if (_BCM_MULTICAST_IS_WLAN(group)) {
            BCM_PBMP_PORT_SET(ether_higig_pbmp,
                              AXP_PORT(unit, SOC_AXP_NLF_WLAN_ENCAP));
        }
    }
#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_subtag_coe) ||
        soc_feature(unit, soc_feature_linkphy_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &ether_higig_pbmp);
    }
#endif

    /*
     * In case of COE, the access ports on line card connected
     * to this device are not included in this device local front
     * panel port bitmap. Therefore we need to iterate over the local
     * front panel ports as well as the line card access ports for
     *  multicast membership related operations.
     */
    num_ports = BCM_PBMP_NUM_PORTS(unit);

    for (port_iter=0; port_iter < num_ports; port_iter++) {
        port = port_iter;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
        /*
         * if this is a subport then add all the encap ids of subport
         * in same list.
         * Same is done for physical ports.
         */
        if (soc_feature(unit, soc_feature_channelized_switching)) {
            if (port_iter < FB6_NUM_PORT) {
                if (!(BCM_PBMP_MEMBER(ether_higig_pbmp, port_iter))) {
                    continue;
                }
            } else {
                _bcm_coe_subtag_subport_num_subport_gport_get(unit, port_iter, &port);
                if (!_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
                    /* This is not a valid subport. continue;*/
                    continue;
                }
            }

        } else
#endif
        {
            if (!(BCM_PBMP_MEMBER(ether_higig_pbmp, port_iter))) {
                continue;
            }
        }
/*    BCM_PBMP_ITER(ether_higig_pbmp, port_iter) {*/

        if (port_max > 0) {
            rv = bcm_esw_ipmc_egress_intf_get(unit, mc_index, port,
                                             port_max - *port_count, 
                                             local_id_array, 
                                             &count);
        } else {
            rv = bcm_esw_ipmc_egress_intf_get(unit, mc_index, port,
                                             0,
                                             NULL, 
                                             &count);
        }
        if (BCM_FAILURE(rv)) {
            sal_free((void *)local_id_array);
            return rv;
        }
#if defined(BCM_KATANA2_SUPPORT)
        /* For KT2, pp_port >=42 are used for LinkPHY/SubportPktTag subport.
         * Get the subport info associated with the pp_port and
         * form the subport_gport.
         */ 
        is_local_subport = FALSE;
        if (_bcm_kt2_is_pp_port_linkphy_subtag(unit, port)) {
            is_local_subport = TRUE;
        }
#endif

        for (i = 0; i < count; i++) {
            /* Copy data to provided arrays */
            if ((NULL != encap_id_array) && (NULL != local_id_array)) {
                if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
                    /* Translate the HW index to encap_id */
                    encap_id_array[*port_count + i] = local_id_array[i] +
                            BCM_XGS3_EGRESS_IDX_MIN(unit);
                } else {
                    encap_id_array[*port_count + i] = local_id_array[i];
                }
            }
            if (NULL != port_array) {
                /* Convert to GPORT values */
#if defined(BCM_KATANA2_SUPPORT)
                if (is_local_subport) {

                    _BCM_KT2_SUBPORT_PORT_ID_SET(port_array[*port_count + i],
                            port_iter);
                    if (BCM_PBMP_MEMBER(
                        SOC_INFO(unit).linkphy_pp_port_pbm, port_iter)) {
                        _BCM_KT2_SUBPORT_PORT_TYPE_SET(
                            port_array[*port_count + i],
                            _BCM_KT2_SUBPORT_TYPE_LINKPHY);
                    } else if (BCM_PBMP_MEMBER(
                        SOC_INFO(unit).subtag_pp_port_pbm, port_iter)) {
                        _BCM_KT2_SUBPORT_PORT_TYPE_SET(
                            port_array[*port_count + i],
                            _BCM_KT2_SUBPORT_TYPE_SUBTAG);
                    } else {
                        sal_free((void *)local_id_array);
                        return BCM_E_PORT;
                    }
                } else
#endif /* BCM_KATANA2_SUPPORT */
                {
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
                    if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
                        /* This is a valid subport. */
                        port_array[(*port_count + i)] = port;
                    } else
#endif
                    {
                        rv = bcm_esw_port_gport_get(unit, port_iter, 
                                  &(port_array[(*port_count + i)]));
                        if (BCM_FAILURE(rv)) {
                            sal_free((void *)local_id_array);
                            return rv;
                        }
                    }
                }
            }
        }
        *port_count += count;
        if ((port_max > 0) && (*port_count == port_max)) {
            break;
        }
    }

    /* Check for L2 ports */
    rv =_bcm_esw_multicast_ipmc_read(unit, mc_index, &l2_pbmp, &l3_pbmp);
    if (BCM_FAILURE(rv)) {
        sal_free((void *)local_id_array);
        return rv;
    }

    BCM_PBMP_ITER(l2_pbmp, port_iter) {
        if ((port_max > 0) && (*port_count == port_max)) {
            break;
        }
        if (NULL != port_array) {
#if defined(BCM_KATANA2_SUPPORT)
            /* For KT2, pp_port >=42 are used for LinkPHY/SubportPktTag subport.
             * Get the subport info associated with the pp_port and
             * form the subport_gport.
             */ 
            is_local_subport = FALSE;
            if (_bcm_kt2_is_pp_port_linkphy_subtag(unit, port_iter)) {
                is_local_subport = TRUE;
            }
            if (is_local_subport) {
                _BCM_KT2_SUBPORT_PORT_ID_SET(port_array[*port_count],
                    port_iter);
                if (BCM_PBMP_MEMBER(
                    SOC_INFO(unit).linkphy_pp_port_pbm, port_iter)) {
                    _BCM_KT2_SUBPORT_PORT_TYPE_SET(port_array[*port_count],
                        _BCM_KT2_SUBPORT_TYPE_LINKPHY);
                } else if (BCM_PBMP_MEMBER(
                    SOC_INFO(unit).subtag_pp_port_pbm, port_iter)) {
                    _BCM_KT2_SUBPORT_PORT_TYPE_SET(port_array[*port_count],
                        _BCM_KT2_SUBPORT_TYPE_SUBTAG);
                } else {
                    sal_free((void *)local_id_array);
                    return BCM_E_PORT;
                }
    
            } else
#endif /* BCM_KATANA2_SUPPORT */
            {
                rv = bcm_esw_port_gport_get(unit, port_iter, 
                                        &(port_array[*port_count]));

                if (BCM_FAILURE(rv)) {
                    sal_free((void *)local_id_array);
                    return rv;
                }
            }
        }
        if (NULL != encap_id_array) {
            encap_id_array[*port_count] = BCM_IF_INVALID;
        }
        *port_count += 1;
    }

    if (local_id_array) {
        sal_free((void *)local_id_array);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_esw_multicast_l3_create
 * Purpose:
 *      Helper function to allocate a multicast group index for L3 or Virtual
 * Parameters:
 *      unit       - (IN)   Device Number
 *      flags      - (IN)   BCM_MULTICAST_*
 *      group      - (OUT)  Group ID
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_multicast_l3_create(int unit, uint32 flags, bcm_multicast_t *group)
{
    int                 mc_index, rv;
    int                 is_set = FALSE;
    egr_ipmc_entry_t    egr_ipmc;
    bcm_pbmp_t          active, l2_pbmp, l3_pbmp;
    uint32              type;
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];  /* hw entry bufffer. */

    /* Check if IPMC module is initialized */
    IPMC_INIT(unit);

    rv  = BCM_E_NONE;
    type = flags & BCM_MULTICAST_TYPE_MASK;

    if (flags & BCM_MULTICAST_WITH_ID) {
        mc_index = _BCM_MULTICAST_ID_GET(*group);
        if ((mc_index < 0) ||
            (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
            return BCM_E_PARAM;
        }
        rv = bcm_xgs3_ipmc_id_is_set(unit, mc_index, &is_set);
        if (BCM_SUCCESS(rv)) {
            if (is_set) {
                return BCM_E_EXISTS;
            }
        } else {
            return rv;
        }
        rv = bcm_xgs3_ipmc_id_alloc(unit, mc_index);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else {
        /* Allocate an IPMC index */
        BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_create(unit, &mc_index));
    }

    /*Per IPMC Group Attributes */
    sal_memset(&egr_ipmc, 0, sizeof(egr_ipmc));

    /* For L3 IPMC case , L3_PAYLOAD must be set to 1 */
    if (soc_feature(unit, soc_feature_egr_ipmc_mem_field_l3_payload_valid) &&
           (type == BCM_MULTICAST_TYPE_L3)) {
        uint32 val=1;
        soc_EGR_IPMCm_field_set(unit, &egr_ipmc, L3_PAYLOADf, &val);
    }

    if ((type & _BCM_VIRTUAL_TYPES_MASK) && 
        (type != BCM_MULTICAST_TYPE_WLAN)) {
        if (soc_mem_field_valid(unit, EGR_IPMCm, REPLICATION_TYPEf)) {
            soc_EGR_IPMCm_field32_set(unit, &egr_ipmc,
                                      REPLICATION_TYPEf, 0x1);
            /* Next-hop indexes from MMU */
        }
        if (soc_mem_field_valid(unit, EGR_IPMCm, DONT_PRUNE_VLANf)) {
            soc_EGR_IPMCm_field32_set(unit, &egr_ipmc, DONT_PRUNE_VLANf, 1);
        }
    }

    /* Commit the values to HW */
    if (SOC_MEM_IS_VALID(unit, EGR_IPMCm)) {
        rv = WRITE_EGR_IPMCm(unit, MEM_BLOCK_ALL, mc_index, &egr_ipmc);
        if (BCM_FAILURE(rv)) {
            (void) bcm_xgs3_ipmc_id_free(unit, mc_index);
            return rv;
        }
    }

    /* Add stack ports to L2 PBMP */
    BCM_PBMP_CLEAR(l2_pbmp);
    BCM_PBMP_CLEAR(l3_pbmp);
    SOC_PBMP_STACK_ACTIVE_GET(unit, active);
    BCM_PBMP_OR(l2_pbmp, active);
    BCM_PBMP_REMOVE(l2_pbmp, SOC_PBMP_STACK_INACTIVE(unit));

    rv = _bcm_esw_multicast_ipmc_write(unit, mc_index, l2_pbmp,
                                       l3_pbmp, TRUE);
    if (BCM_FAILURE(rv)) {
        sal_memset(&egr_ipmc, 0, sizeof(egr_ipmc));
        if (SOC_MEM_IS_VALID(unit, EGR_IPMCm)) {
            (void) WRITE_EGR_IPMCm(unit, MEM_BLOCK_ALL, mc_index, 
                                   &egr_ipmc);
            (void) bcm_xgs3_ipmc_id_free(unit, mc_index);
            return rv;
        }
    }

    if (type == BCM_MULTICAST_TYPE_L3) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L3,
                                 mc_index);
    } else if (type == BCM_MULTICAST_TYPE_VPLS) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_VPLS,
                                 mc_index);
    } else if (type == BCM_MULTICAST_TYPE_MIM) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_MIM,
                                 mc_index);
    } else if (type == BCM_MULTICAST_TYPE_WLAN) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_WLAN,
                                 mc_index);
    } else if (type == BCM_MULTICAST_TYPE_SUBPORT) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_SUBPORT,
                                 mc_index);
    }else if (type == BCM_MULTICAST_TYPE_TRILL) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_TRILL,
                                 mc_index);
    } else if (type == BCM_MULTICAST_TYPE_VLAN) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_VLAN,
                                 mc_index);
    } else if (type == BCM_MULTICAST_TYPE_NIV) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_NIV,
                                 mc_index);
    } else if (type == BCM_MULTICAST_TYPE_EGRESS_OBJECT) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_EGRESS_OBJECT,
                                 mc_index);
    } else if (type == BCM_MULTICAST_TYPE_L2GRE) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L2GRE,
                                 mc_index);
    } else if (type == BCM_MULTICAST_TYPE_VXLAN) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_VXLAN,
                                 mc_index);
    } else if (type == BCM_MULTICAST_TYPE_FLOW) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_FLOW,
                                 mc_index);
    } else if (type == BCM_MULTICAST_TYPE_EXTENDER) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_EXTENDER,
                                 mc_index);
    }

    if (soc_feature(unit, soc_feature_trill)) {
        if (type == BCM_MULTICAST_TYPE_TRILL) {
            if (SOC_MEM_FIELD_VALID(unit, L3_IPMCm, REMOVE_SGLP_FROM_L3_BITMAPf)) {
                rv = soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, mc_index, entry);
                if (BCM_SUCCESS(rv)) {
                    soc_mem_field32_set(unit, L3_IPMCm, entry, 
                                    REMOVE_SGLP_FROM_L3_BITMAPf, 0x1);
                    rv = soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ALL, mc_index, &entry);
                }
            }
        }
    }

    if (BCM_SUCCESS(rv) && SOC_IS_TRX(unit)) {
        /* Record IPMC types for reference */
        rv = _bcm_tr_multicast_ipmc_group_type_set(unit, *group);
    }

    return rv;
}


/*
 * Function:
 *      _bcm_esw_multicast_l3_destroy
 * Purpose:
 *      Helper function to destroy L3 and Virtual multicast 
 * Parameters: 
 *      unit           - (IN) Device Number
 *      group          - (IN) Multicast group to destroy
 * Returns:
 *      BCM_E_XXX
 */

STATIC int     
_bcm_esw_multicast_l3_destroy(int unit, bcm_multicast_t group)
{
    int                 is_l3, mc_index;
    bcm_port_t          port_iter;
    bcm_pbmp_t          ether_higig_pbmp, l2_pbmp, l3_pbmp;
    egr_ipmc_entry_t    egr_ipmc;


    /* Check if IPMC module is initialized */
    IPMC_INIT(unit);

    is_l3 = 0;
    
    BCM_IF_ERROR_RETURN(_bcm_esw_multicast_l3_group_check(unit, group, &is_l3));

    mc_index = _BCM_MULTICAST_ID_GET(group);
    if ((mc_index < 0) || (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

    /* Decrement ref count by calling 'free' */
    BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_id_free(unit, mc_index));

    if (IPMC_USED_ISSET(unit, mc_index)) {
        /* Group is in use, correct the ref count
         * decremented by above 'free' and return error. */
        IPMC_USED_SET(unit, mc_index)                                   \
        return BCM_E_BUSY;
    } else { /* Ref count is zero. Group is not being used. */
        /* Clear the replication set */
        BCM_IF_ERROR_RETURN(
        _bcm_esw_multicast_ipmc_read(unit, mc_index, &l2_pbmp, &l3_pbmp));
        if(is_l3) {
           BCM_PBMP_ASSIGN(ether_higig_pbmp, l3_pbmp);
        } else {
           BCM_PBMP_ASSIGN(ether_higig_pbmp, l2_pbmp);
        }
        BCM_PBMP_OR(ether_higig_pbmp, PBMP_HG_ALL(unit));
        /* CPU ports are valid for replication on TD2 */
        if (SOC_IS_TRIDENT2X(unit)) {
            BCM_PBMP_OR(ether_higig_pbmp, PBMP_CMIC(unit));
        }
        if (soc_feature(unit, soc_feature_axp)) {
            if (_BCM_MULTICAST_IS_WLAN(group)) {
                BCM_PBMP_PORT_SET(ether_higig_pbmp,
                                  AXP_PORT(unit, SOC_AXP_NLF_WLAN_ENCAP));
            }
        }
#if defined(BCM_KATANA2_SUPPORT)
        if (soc_feature(unit, soc_feature_subtag_coe) ||
            soc_feature(unit, soc_feature_linkphy_coe)) {
           _bcm_kt2_subport_pbmp_update(unit, &ether_higig_pbmp);
       }
#endif

#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_ipmc_egress_intf_destroy_in_trunk_repl_mode(
                     unit, mc_index, ether_higig_pbmp, is_l3));
        } else
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */
        {
            BCM_PBMP_ITER(ether_higig_pbmp, port_iter) {
                BCM_IF_ERROR_RETURN(
                     _bcm_esw_ipmc_egress_intf_set(unit, mc_index, port_iter, 0,
                                                   NULL, is_l3, FALSE));
            }
        }

        /* Clear the IPMC related tables */
        BCM_PBMP_CLEAR(l2_pbmp);
        BCM_PBMP_CLEAR(l3_pbmp);

        BCM_IF_ERROR_RETURN(
        _bcm_esw_multicast_ipmc_write(unit, mc_index, l2_pbmp, l3_pbmp, FALSE));

        if (SOC_MEM_IS_VALID(unit, EGR_IPMCm)) {
            sal_memset(&egr_ipmc, 0, sizeof(egr_ipmc));
            BCM_IF_ERROR_RETURN
                (WRITE_EGR_IPMCm(unit, MEM_BLOCK_ALL, mc_index, &egr_ipmc));
        }

        /* Free IPMC type record */
        if (SOC_IS_TRX(unit)) {
            BCM_IF_ERROR_RETURN(
                _bcm_tr_multicast_ipmc_group_type_set(unit, mc_index));
        }
    }
    
    return BCM_E_NONE;
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *      _bcm_esw_multicast_type_validate
 * Purpose:
 *      Helper function to validate multicast type support 
 * Parameters:
 *      unit       - (IN)   Device Number
 *      type       - (IN)   BCM_MULTICAST_TYPE_*
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_esw_multicast_type_validate(int unit, uint32 type)
{
    int rv; 

    /* Only single multicast type is allowed per API call */
    /* therefore number of bits on indicating Multicast type must equal to 1 */
    if (1 != _shr_popcount(type)) {
        return BCM_E_PARAM;
    }

    rv = BCM_E_PARAM; /* return error by default */

    switch (type) {
        case BCM_MULTICAST_TYPE_L2: 
            rv = BCM_E_NONE;
            break;
        case BCM_MULTICAST_TYPE_L3: 
            if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
                rv = BCM_E_NONE;
            }else {
                rv = BCM_E_UNAVAIL;
            }
            break;
        case BCM_MULTICAST_TYPE_VPLS:
            if (soc_feature(unit, soc_feature_mpls)) {
                rv = BCM_E_NONE;
            } else {
                rv = BCM_E_UNAVAIL;
            }
            break;
        case BCM_MULTICAST_TYPE_SUBPORT:
            if (soc_feature(unit, soc_feature_subport)) {
                rv = BCM_E_NONE;
            }
            break;
        case BCM_MULTICAST_TYPE_MIM:
            if (soc_feature(unit, soc_feature_mim)) {
                rv = BCM_E_NONE;
            }
            break;
        case BCM_MULTICAST_TYPE_WLAN:
            if (soc_feature(unit, soc_feature_wlan)) {
                rv = BCM_E_NONE;
            }
            break;
        case BCM_MULTICAST_TYPE_VLAN:
            if (soc_feature(unit, soc_feature_vlan_vp)) {
                rv = BCM_E_NONE;
            }
            break;
        case BCM_MULTICAST_TYPE_TRILL:
            if (soc_feature(unit, soc_feature_trill)) {
                rv = BCM_E_NONE;
            }
            break;
        case BCM_MULTICAST_TYPE_NIV:
            if (soc_feature(unit, soc_feature_niv)) {
                rv = BCM_E_NONE;
            }
            break;
        case BCM_MULTICAST_TYPE_EGRESS_OBJECT:
            if (soc_feature(unit, soc_feature_mpls)) {
                rv = BCM_E_NONE;
            }
            break;
        case BCM_MULTICAST_TYPE_L2GRE:
            if (soc_feature(unit, soc_feature_l2gre)) {
                rv = BCM_E_NONE;
            } else {
                rv = BCM_E_UNAVAIL;
            }
            break;
        case BCM_MULTICAST_TYPE_VXLAN:
            if (soc_feature(unit, soc_feature_vxlan) ||
                soc_feature(unit, soc_feature_vxlan_lite)) {
                rv = BCM_E_NONE;
            } else {
                rv = BCM_E_UNAVAIL;
            }
            break;
        case BCM_MULTICAST_TYPE_FLOW:
            if (soc_feature(unit, soc_feature_flex_flow)) {
                rv = BCM_E_NONE;
            } else {
                rv = BCM_E_UNAVAIL;
            }
            break;
        case BCM_MULTICAST_TYPE_EXTENDER:
            if (soc_feature(unit, soc_feature_port_extension)) {
                rv = BCM_E_NONE;
            }
            break;

        default:
            rv = BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_multicast_create
 * Purpose:
 *      Helper function to allocate a multicast group index
 * Parameters:
 *      unit       - (IN)   Device Number
 *      flags      - (IN)   BCM_MULTICAST_*
 *      group      - (OUT)  Group ID
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_multicast_create(int unit, uint32 flags, bcm_multicast_t *group)
{
    uint32 type;

    type = flags & BCM_MULTICAST_TYPE_MASK;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_multicast_type_validate(unit, type));

    if (type == BCM_MULTICAST_TYPE_L2) {
        /* Create L2 multicast group */
        return _bcm_esw_multicast_l2_create(unit, flags, group);
    } else {
#if defined(INCLUDE_L3)
        /* Create L3/Virtual multicast group */
        return _bcm_esw_multicast_l3_create(unit, flags, group);
#endif /* INCLUDE_L3 */
    }

    return BCM_E_UNAVAIL;
}


/* Macro to free memory and return code instead of using goto */
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
#define BCM_MULTICAST_MEM_FREE_RETURN(_r_)                      \
        if (local_trunk_port_array) {                           \
            for (j = 0; j < port_count; j++) {                  \
                if (local_trunk_port_array[j]) {                \
                    sal_free(local_trunk_port_array[j]);        \
                }                                               \
            }                                                   \
            sal_free(local_trunk_port_array);                   \
        }                                                       \
        if (local_port_array) {                                 \
            sal_free(local_port_array);                         \
        }                                                       \
        if (trunk_local_ports) {                                \
            sal_free(trunk_local_ports);                        \
        }                                                       \
        if (temp_encap_list) {                                  \
            sal_free(temp_encap_list);                          \
            temp_encap_list = NULL;                             \
        }                                                       \
        if (trunk_pipe_first_member_array) {                    \
            for (j = 0; j < port_count; j++) {                  \
                if (trunk_pipe_first_member_array[j]) {         \
                    sal_free(trunk_pipe_first_member_array[j]); \
                }                                               \
            }                                                   \
            sal_free(trunk_pipe_first_member_array);            \
        }                                                       \
        return _r_;
#else
#define BCM_MULTICAST_MEM_FREE_RETURN(_r_)              \
    if (local_trunk_port_array) {                       \
        for (j = 0; j < port_count; j++) {              \
            if (local_trunk_port_array[j]) {            \
                sal_free(local_trunk_port_array[j]);    \
            }                                           \
        }                                               \
        sal_free(local_trunk_port_array);               \
    }                                                   \
    if (local_port_array) {                             \
        sal_free(local_port_array);                     \
    }                                                   \
    if (trunk_local_ports) {                            \
        sal_free(trunk_local_ports);                    \
    }                                                   \
    if (temp_encap_list) {                              \
        sal_free(temp_encap_list);                      \
        temp_encap_list = NULL;                         \
    }                                                   \
    return _r_;
#endif



#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_esw_multicast_egress_set
 * Purpose:
 *      Helper function to assign the complete set of egress GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit       - (IN) Device Number
 *      group      - (IN) Multicast group ID
 *      port_count   - (IN) Number of ports in replication list
 *      port_array   - (IN) List of GPORT Identifiers
 *      encap_id_array - (IN) List of encap identifiers
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_multicast_egress_set(int unit, bcm_multicast_t group, int port_count,
                            bcm_gport_t *port_array, bcm_if_t *encap_id_array)
{
    int mc_index, i, j, rv = BCM_E_NONE;
    int gport_id, *temp_encap_list = NULL;
    bcm_port_t *local_port_array = NULL, **local_trunk_port_array = NULL;
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    int max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    int max_num_ports = SOC_MAX_NUM_PORTS;
#endif
    bcm_module_t mod_out;
    bcm_port_t port_out, local_port, log_port, port_gport, gport_in;
    bcm_pbmp_t l2_pbmp, l3_pbmp;
    l2mc_entry_t l2mc_entry; 
    bcm_trunk_t trunk_id;
    soc_mem_t   mem;
    int idx, idx_max;
    int modid_local = 0;
    int   *trunk_local_ports = NULL;
#if defined (INCLUDE_L3)
    int list_count, is_l3 = 0;
    bcm_pbmp_t mc_eligible_pbmp, e_pbmp;
    bcm_port_t port_iter, sub_port;
    int trunk_local_found;
#endif /* INCLUDE_L3 */
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    int is_local_subport = FALSE;
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    int **trunk_pipe_first_member_array = NULL;
    int pos, pipe;
#endif /* BCM_TRIDENT2PLUS_SUPPORT || BCM_TOMAHAWK_SUPPORT */
    int num_ports;

    /* Input parameters check. */
    mc_index = _BCM_MULTICAST_ID_GET(group);
    mem = (_BCM_MULTICAST_L2_OR_FABRIC(unit, group)) ? L2MCm : L3_IPMCm; 
    /* Table index sanity check. */
#ifdef BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_fabric_multicast_param_check(unit, group, &mc_index));
    } else
#endif /* BCM_XGS3_FABRIC_SUPPORT */
    if ((mc_index < 0) || (mc_index >= soc_mem_index_count(unit, mem))) {
        return (BCM_E_PARAM);
    }

    if (port_count > 0) {
        if (NULL == port_array) {
            return (BCM_E_PARAM);
        }
        if (!_BCM_MULTICAST_L2_OR_FABRIC(unit, group) && 
            (NULL == encap_id_array)) {
            return (BCM_E_PARAM);
        }
    } else if (port_count < 0) {
        return (BCM_E_PARAM);
    }   

#ifdef BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        /* The group was already validated above */
    } else
#endif /* BCM_XGS3_FABRIC_SUPPORT */
    if (!SOC_IS_TRX(unit)) {
        if ((0 == _BCM_MULTICAST_IS_L2(group)) && 
            (0 == _BCM_MULTICAST_IS_L3(group))) {
            return (BCM_E_PARAM);
        }
    } else {
#if defined (INCLUDE_L3)
         rv = _bcm_esw_multicast_l3_group_check(unit, group, NULL);
#endif /* INCLUDE_L3 */
        if (BCM_FAILURE(rv) && (0 == _BCM_MULTICAST_IS_L2(group))) {
            return BCM_E_PARAM;
        }
    }

    /* Convert GPORT array into local port numbers */
    if (port_count > 0) {
        local_port_array =
            sal_alloc(sizeof(bcm_port_t) * port_count, "local_port array");
        if (NULL == local_port_array) {
            BCM_MULTICAST_MEM_FREE_RETURN(BCM_E_MEMORY);
        }
        
        temp_encap_list =
            sal_alloc(sizeof(int) * port_count, "temp_encap_list");
        if (NULL == temp_encap_list) {
            BCM_MULTICAST_MEM_FREE_RETURN(rv);
        }

        trunk_local_ports =
            sal_alloc(sizeof(int) * port_count, "trunk_local_ports");
        if (NULL == trunk_local_ports) {
            BCM_MULTICAST_MEM_FREE_RETURN(BCM_E_MEMORY);
        }

        local_trunk_port_array =
            sal_alloc(sizeof(bcm_port_t *) * port_count,
                      "local_trunk_port_array pointers");
        if (NULL == local_trunk_port_array) {
            BCM_MULTICAST_MEM_FREE_RETURN(BCM_E_MEMORY);
        }

        for (i = 0; i < port_count; i++) {
            local_trunk_port_array[i] =
                sal_alloc(sizeof(bcm_port_t) * max_num_ports,
                          "local_trunk_port_array");
            if (NULL == local_trunk_port_array[i]) {
                BCM_MULTICAST_MEM_FREE_RETURN(BCM_E_MEMORY);
            }
        }

        for (i = 0; i < port_count; i++) {
            trunk_local_ports[i] = 0;
            for (idx = 0; idx < max_num_ports; idx++) {
                local_trunk_port_array[i][idx] = 0;
            }
        }

#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
        if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
            trunk_pipe_first_member_array =
                sal_alloc(sizeof(int *) * port_count,
                          "trunk_pipe_first_member_array pointers");
            if (NULL == trunk_pipe_first_member_array) {
                BCM_MULTICAST_MEM_FREE_RETURN(BCM_E_MEMORY);
            }

            for (i = 0; i < port_count; i++) {
                trunk_pipe_first_member_array[i] =
                    sal_alloc(sizeof(int) * BCM_PIPES_MAX,
                              "trunk_pipe_first_member_array");
                if (NULL == trunk_pipe_first_member_array[i]) {
                    BCM_MULTICAST_MEM_FREE_RETURN(BCM_E_MEMORY);
                }
            }

            for (i = 0; i < port_count; i++) {
                for (idx = 0; idx < BCM_PIPES_MAX; idx++) {
                    trunk_pipe_first_member_array[i][idx] = -1;
                }
            }
        }
#endif /* BCM_TRIDENT2PLUS_SUPPORT || BCM_TOMAHAWK_SUPPORT */

    }


    for (i = 0; i < port_count ; i++) {
        rv = _bcm_esw_gport_resolve(unit, port_array[i],
                                    &mod_out, &port_out,
                                    &trunk_id, &gport_id); 
        if (BCM_FAILURE(rv)) {
            BCM_MULTICAST_MEM_FREE_RETURN(rv);
        }

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
        rv = _bcm_esw_port_is_local_subport(
                 unit, port_array[i], mod_out, port_out,
                 &is_local_subport, &local_port_array[i]);
        if (BCM_FAILURE(rv)) {
            BCM_MULTICAST_MEM_FREE_RETURN(rv);
        }

        if (is_local_subport) {
            /* Local subport assigned above */
#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT
            if (soc_feature(unit, soc_feature_channelized_switching)) {
                local_port_array[i] = port_array[i];
            }
#endif
        } else
#endif

        if (BCM_TRUNK_INVALID != trunk_id) {
            /* system ports to physical ports conversion done in
             * _bcm_esw_trunk_local_members_get
             */

            rv = _bcm_esw_trunk_local_members_get(unit, trunk_id,
                    max_num_ports, local_trunk_port_array[i],
                    &trunk_local_ports[i]);

                if (BCM_FAILURE(rv)) {
                    BCM_MULTICAST_MEM_FREE_RETURN(rv);
               }
#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT
            if (soc_feature(unit, soc_feature_channelized_switching)) {
                /* Subport trunks can not be added to multicast group. */
                rv = _bcm_esw_multicast_trunk_local_members_sanitize(unit,
                                    local_trunk_port_array[i],
                                    trunk_local_ports[i]);

                if (BCM_FAILURE(rv)) {
                    BCM_MULTICAST_MEM_FREE_RETURN(rv);
               }
        }
#endif

            /* Record the trunk ID to use later */
            BCM_GPORT_TRUNK_SET(local_port_array[i], trunk_id);
        } else {
            rv = _bcm_esw_modid_is_local(unit, mod_out, &modid_local);
            if (BCM_FAILURE(rv)) {
                BCM_MULTICAST_MEM_FREE_RETURN(rv);
            }

            /* Convert system local_port to physical local_port */ 
            if (soc_feature(unit, soc_feature_sysport_remap)) { 
                BCM_XLATE_SYSPORT_S2P(unit, &port_out); 
            }

            if (TRUE != modid_local) {
                BCM_MULTICAST_MEM_FREE_RETURN(BCM_E_PARAM);
            }
            local_port_array[i] = port_out;

        }
    }

    BCM_PBMP_CLEAR(l2_pbmp);
    BCM_PBMP_CLEAR(l3_pbmp);

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        BCMI_MC_GROUP_PORT_REFCOUNT_CLEAR(unit, mc_index);
        bcmi_subport_multicast_lc_pbm_clear_all(unit, mc_index);
    }
#endif

    if (_BCM_MULTICAST_L2_OR_FABRIC(unit, group)) {
        for (i = 0; i < port_count; i++) {
            if (BCM_GPORT_IS_TRUNK(local_port_array[i])) {
                for (idx = 0; idx < trunk_local_ports[i]; idx++) {
                bcm_port_t trunk_local_port;
                trunk_local_port = local_trunk_port_array[i][idx];
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)

                    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
                        BCM_GPORT_IS_SET(trunk_local_port) &&
                        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit,
                            trunk_local_port)) {
                        rv = _bcmi_coe_subport_physical_port_get(unit,
                                trunk_local_port, &trunk_local_port);
                        if (BCM_FAILURE(rv)) {
                            BCM_MULTICAST_MEM_FREE_RETURN(rv);
                        }
                    }
#endif
                    BCM_PBMP_PORT_ADD(l2_pbmp, trunk_local_port);
                }
            } else {
                bcm_port_t local_port;
                local_port = local_port_array[i];
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)

                    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
                        BCM_GPORT_IS_SET(local_port) &&
                        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit,
                            local_port)) {
                        rv = _bcmi_coe_subport_physical_port_get(unit,
                                local_port, &local_port);
                        if (BCM_FAILURE(rv)) {
                            BCM_MULTICAST_MEM_FREE_RETURN(rv);
                        }
                    }
#endif
                BCM_PBMP_PORT_ADD(l2_pbmp, local_port);
            }
        }

        /* Update the L2MC port bitmap */
        soc_mem_lock(unit, L2MCm);
        rv = soc_mem_read(unit, L2MCm, MEM_BLOCK_ANY, mc_index, &l2mc_entry);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, L2MCm);
            BCM_MULTICAST_MEM_FREE_RETURN(rv);
        }
        soc_mem_pbmp_field_set(unit, L2MCm, &l2mc_entry, PORT_BITMAPf,
                               &l2_pbmp);
        rv = soc_mem_write(unit, L2MCm, MEM_BLOCK_ALL,
                           mc_index, &l2mc_entry);
        soc_mem_unlock(unit, L2MCm);
#if defined(INCLUDE_L3)
    } else {
        if (_BCM_MULTICAST_IS_L3(group)) {
            is_l3 = 1;
        }
        if (0 == port_count) {
            /* Clearing the replication set */
            list_count = 0;
            BCM_PBMP_CLEAR(e_pbmp);
            BCM_PBMP_ASSIGN(e_pbmp, PBMP_E_ALL(unit));
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TRIDENT2X(unit)) {
                BCM_PBMP_PORT_ADD(e_pbmp,CMIC_PORT(unit));
            }
#endif	    
#if defined(BCM_KATANA2_SUPPORT)
            if (soc_feature(unit, soc_feature_subtag_coe) ||
                soc_feature(unit, soc_feature_linkphy_coe)) {
               _bcm_kt2_subport_pbmp_update(unit, &e_pbmp);
           }
#endif

#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_ipmc_egress_intf_destroy_in_trunk_repl_mode(
                         unit, mc_index, e_pbmp, is_l3));
            } else
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */
            {
                /*
                 * In case of COE, the access ports on line card connected
                 * to this device are not included in this device local front
                 * panel port bitmap. Therefore we need to iterate over the local
                 * front panel ports as well as the line card access ports for
                 *  multicast membership related operations.
                 */
                num_ports = BCM_PBMP_NUM_PORTS(unit);
                for (port_iter=0; port_iter < num_ports; port_iter++) {
                    sub_port = port_iter;
                    log_port = port_iter;
                    /* BCM_PBMP_ITER(mc_eligible_pbmp, port_iter) {*/
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
                    /*
                     * if this is a subport then add all the encap ids of subport
                     * in same list.
                     * Same is done for physical ports.
                     */
                    if (soc_feature(unit, soc_feature_channelized_switching)) {
                        if (port_iter < FB6_NUM_PORT) {
                            if (!(BCM_PBMP_MEMBER(e_pbmp, port_iter))) {
                                continue;
                            }
                        }  else {
                            _bcm_coe_subtag_subport_num_subport_gport_get(unit,
                                port_iter, &sub_port);
                                if (!_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, sub_port)) {
                                /* This is not a valid subport. continue;*/
                                 continue;
                            }
                        }
                    } else
#endif
                    {
                        if (!(BCM_PBMP_MEMBER(e_pbmp, port_iter))) {
                            continue;
                        }
                    }
                    rv = _bcm_esw_ipmc_egress_intf_set(unit, mc_index, sub_port,
                                            list_count, NULL, is_l3, FALSE);
                    if (BCM_FAILURE(rv)) {
                        BCM_MULTICAST_MEM_FREE_RETURN(rv);
                    }
                }
            }
        } else {
            /* 
             * For each port, walk through the list of GPORTs
             * and collect the ones that match the port.
             */

#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                /* MAx number of replications across all ports cannot exceed
                 * TH3_IPMC_MAX_INTF_COUNT*/
                if (port_count > TH3_IPMC_MAX_INTF_COUNT) {
                    rv = BCM_E_PARAM;
                    BCM_MULTICAST_MEM_FREE_RETURN(rv);
                }
            }
#endif
            /* Valid SOC ports plus the CMIC port */
            BCM_PBMP_ASSIGN(mc_eligible_pbmp, PBMP_PORT_ALL(unit));
            BCM_PBMP_PORT_ADD(mc_eligible_pbmp, CMIC_PORT(unit));
#if defined(BCM_KATANA2_SUPPORT)
            if (soc_feature(unit, soc_feature_subtag_coe) ||
                soc_feature(unit, soc_feature_linkphy_coe)) {
               _bcm_kt2_subport_pbmp_update(unit, &mc_eligible_pbmp);
           }
#endif

            /*
            * In case of COE, the access ports on line card connected
            * to this device are not included in this device local front
            * panel port bitmap. Therefore we need to iterate over the local
            * front panel ports as well as the line card access ports for
            *  multicast membership related operations.
            */
            num_ports = BCM_PBMP_NUM_PORTS(unit);
            for (port_iter=0; port_iter < num_ports; port_iter++) {

                /* BCM_PBMP_ITER(mc_eligible_pbmp, port_iter) {*/
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
                /*
                 * if this is a subport then add all the encap ids of subport
                 * in same list.
                 * Same is done for physical ports.
                 */
                if (soc_feature(unit, soc_feature_channelized_switching)) {
                    if (port_iter < FB6_NUM_PORT) {
                        if (!(BCM_PBMP_MEMBER(mc_eligible_pbmp, port_iter))) {
                            continue;
                        }
                    }
                } else
#endif
                {
                    if (!(BCM_PBMP_MEMBER(mc_eligible_pbmp, port_iter))) {
                        continue;
                    }
                }

                list_count = 0;

                /* initialize all ports. */
                log_port = port_iter;
                port_gport = port_iter;
                gport_in = port_iter;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
                trunk_id = BCM_TRUNK_INVALID;
                pos = -1;
#endif /* BCM_TRIDENT2PLUS_SUPPORT || BCM_TOMAHAWK_SUPPORT */
                for (i = 0; i < port_count ; i++) {
                    if (BCM_GPORT_IS_TRUNK(local_port_array[i])) {
                        trunk_local_found = 0;
                        idx_max = trunk_local_ports[i];

                        for (idx = 0; idx < idx_max; idx++) {
                            port_gport = local_trunk_port_array[i][idx];
                            local_port = port_gport;
                            log_port = port_gport;
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
                            /*
                             * if this is a subport then add all the encap ids of subport
                             * in same list.
                             * Same is done for physical ports.
                             */
                            if ((soc_feature(unit, soc_feature_channelized_switching)) &&
                                 BCM_GPORT_IS_SET(port_gport) &&
                                 _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit,
                                 port_gport)) {

                                BCM_IF_ERROR_RETURN(
                                    _bcmi_coe_subport_physical_port_get(unit,
                                    port_gport, &log_port));

                                 BCM_IF_ERROR_RETURN(
                                     _bcm_coe_subtag_subport_port_subport_num_get(unit,
                                     port_gport, NULL, (int *)&local_port));

                            }
#endif
                            if (local_port == port_iter) {
                                trunk_local_found = 1;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
                                if (trunk_id == BCM_TRUNK_INVALID) {
                                    pos = i;
                                    trunk_id = BCM_GPORT_TRUNK_GET(local_port_array[i]);
                                }
#endif /* BCM_TRIDENT2PLUS_SUPPORT || BCM_TOMAHAWK_SUPPORT */
                                break;
                            }
                        }
                        if (!trunk_local_found) {
                            continue;
                        }   
                    } else {
                        port_gport = local_port_array[i];
                        local_port = port_gport;
                        log_port = port_gport;
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
                        /*
                         * if this is a subport then add all the encap ids of subport
                         * in same list.
                         * Same is done for physical ports.
                         */
                        if ((soc_feature(unit, soc_feature_channelized_switching)) &&
                             BCM_GPORT_IS_SET(port_gport) &&
                             _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit,
                             port_gport)) {

                             BCM_IF_ERROR_RETURN(
                                 _bcmi_coe_subport_physical_port_get(unit,
                                 port_gport, &log_port));

                             BCM_IF_ERROR_RETURN(
                                 _bcm_coe_subtag_subport_port_subport_num_get(unit,
                                 port_gport, NULL, (int *)&local_port));

                        }
#endif
                        if (local_port != port_iter) {
                            continue;
                        }
                    }
                    /* Updated the matched port. This will get programmed. */
                    gport_in = port_gport;
                    if (encap_id_array[i] == BCM_IF_INVALID) {
                        /* Add port to L2 pbmp */
                        BCM_PBMP_PORT_ADD(l2_pbmp, log_port);
                    } else {
                        if (soc_feature(unit, 
                                soc_feature_l3mc_use_egress_next_hop)) {
                            /* _bcm_esw_ipmc_egress_intf_set() processing the 
                             * real HW index to indicate egress NH object.
                             */
                            temp_encap_list[list_count] = encap_id_array[i] - 
                                     BCM_XGS3_EGRESS_IDX_MIN(unit);
                        } else {
                            temp_encap_list[list_count] = encap_id_array[i];
                        }
                        list_count++;
                    }
                }
                log_port = gport_in;
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
                /*
                 * Get the port based on gport being programmed.
                 */
                if (soc_feature(unit, soc_feature_channelized_switching)) {
                    if (BCM_GPORT_IS_SET(gport_in) &&
                        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport_in)) {

                        BCM_IF_ERROR_RETURN(
                           _bcmi_coe_subport_physical_port_get(unit,
                            gport_in, &log_port));
                    }

                    if (list_count == 0) {
                        if (port_iter < FB6_NUM_PORT) {
                            gport_in = port_iter;
                        } else {
                            _bcm_coe_subtag_subport_num_subport_gport_get(unit,
                                port_iter, &gport_in);
                            if (!_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit,
                                 gport_in)) {
                                gport_in = 0;
                            }
                        }
                    }
                }
#endif

                if ((0 == list_count) && (CMIC_PORT(unit) == log_port)) {
                    if (!SOC_IS_TRIDENT2(unit)) {
                        /* Don't install L3 ports on CMIC */
                        continue;
                    }
                }
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
                if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                    if (trunk_id != BCM_TRUNK_INVALID) {
                        bcm_port_t first_member;
                        pipe = SOC_INFO(unit).port_pipe[log_port];
                        first_member = trunk_pipe_first_member_array[pos][pipe];
                        if (first_member == -1) {
                            rv = _bcm_esw_ipmc_egress_intf_set_for_trunk_first_member(
                                     unit, mc_index, gport_in, list_count,
                                     temp_encap_list, is_l3, TRUE, trunk_id);
                            trunk_pipe_first_member_array[pos][pipe] = gport_in;
#if defined(BCM_TOMAHAWK3_SUPPORT)
                            if(SOC_IS_TOMAHAWK3(unit)) {
                                int th3_idx;
                                for(th3_idx = 0; th3_idx < BCM_PIPES_MAX; th3_idx++) {
                                    trunk_pipe_first_member_array[pos][th3_idx] = port_iter;
                                }
                            }
#endif
                        } else {
                            rv = _bcm_esw_ipmc_egress_intf_set_for_same_pipe_member(
                                     unit, mc_index, gport_in, first_member,
                                     trunk_id);
                        }
                    } else {
                        rv = _bcm_esw_ipmc_egress_intf_set_for_trunk_first_member(
                             unit, mc_index, gport_in, list_count,
                             temp_encap_list, is_l3, TRUE, BCM_TRUNK_INVALID);
                    }
                } else
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */
                {
                    rv = _bcm_esw_ipmc_egress_intf_set(
                             unit, mc_index, gport_in, list_count,
                             temp_encap_list, is_l3, TRUE);
                }
                if (BCM_FAILURE(rv)) {
                    BCM_MULTICAST_MEM_FREE_RETURN(rv);
                }
                if (list_count) {
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
                if (soc_feature(unit, soc_feature_channelized_switching)) {
                    (BCMI_MC_GROUP_PORT_REFCOUNT(unit, mc_index,
                        log_port))++;
                }
#endif

                    BCM_PBMP_PORT_ADD(l3_pbmp, log_port);
                }
            }
        }

        rv = _bcm_esw_multicast_ipmc_write(unit, mc_index, l2_pbmp,
                                           l3_pbmp, TRUE);
#endif  /* INCLUDE_L3 */
    }

    BCM_MULTICAST_MEM_FREE_RETURN(rv);
}
#endif


#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
int
_bcm_fabric_multicast_ipmc_group_type_set(int unit, bcm_multicast_t group)
{
    uint8 group_type = _BCM_MULTICAST_TYPE_GET(group);
    uint32 ipmc_id = _BCM_MULTICAST_ID_GET(group);

    /* Don't check if MULTICAST_GROUP_IS_SET because we also want to
     * be able to clear a group. */


    /* It's an IPMC type group */
    _fabric_multicast_ipmc_group_types[unit][ipmc_id] = group_type;

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif
    return BCM_E_NONE;
}

int
_bcm_fabric_multicast_ipmc_group_type_get(int unit, uint32 ipmc_id,
                                      bcm_multicast_t *group)
{
    if ((NULL != _fabric_multicast_ipmc_group_types[unit]) && /* Verify init */
        (0 != _fabric_multicast_ipmc_group_types[unit][ipmc_id])) {
        _BCM_MULTICAST_GROUP_SET(*group,
                                 _fabric_multicast_ipmc_group_types[unit][ipmc_id],
                                 ipmc_id);
        return BCM_E_NONE;
    } else {
        *group = 0;
        return BCM_E_NOT_FOUND;
    }
    return BCM_E_NONE;
}
#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int
_bcm_fabric_multicast_reinit(int unit)
{
    int ipmc_base, ipmc_size = 0, alloc_size, ipmc_id;
    int index_min, index_max, search_index;
    soc_scache_handle_t scache_handle;
    uint8 *multicast_scache;
    int rv = BCM_E_NONE;

    SOC_IF_ERROR_RETURN
        (soc_hbx_ipmc_size_get(unit, &ipmc_base, &ipmc_size));

    alloc_size = ipmc_size * sizeof(uint8);
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_MULTICAST, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
            0, &multicast_scache,
            BCM_FABRIC_IPMC_WB_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        multicast_scache = NULL;
        /* Version which doesnt support fabric IPMC group types.
         * Update type as L3.
         */
        index_min = BCM_FABRIC_IPMC_BASE(unit);
        index_max = BCM_FABRIC_IPMC_MAX(unit);

        for (search_index = index_min; search_index <= index_max;
                search_index++) {
            ipmc_id = search_index - index_min;
            /* If that mc index exists */
            if (SHR_BITGET(BCM_FABRIC_MC_USED_BITMAP(unit),
                        search_index)) {
                _fabric_multicast_ipmc_group_types[unit][ipmc_id] =
                                                _BCM_MULTICAST_TYPE_L3;
            }
        }
    } else if (BCM_FAILURE(rv)) {
        return rv;
    } else {
        sal_memcpy(_fabric_multicast_ipmc_group_types[unit], multicast_scache,
                alloc_size);
    }
    return BCM_E_NONE;
}
#endif
/*
 * Function:
 *      _bcm_esw_fabric_multicast_init
 * Purpose:
 *      Initialize the multicast module on XGS3 fabric devices.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int 
_bcm_esw_fabric_multicast_init(int unit)
{
    int	mc_base, mc_size;
    int	ipmc_base, ipmc_size, alloc_size;
    int l2mc_table_size = soc_mem_index_count(unit, L2MCm);
#ifdef BCM_WARM_BOOT_SUPPORT
    int rv = BCM_E_NONE;
    soc_scache_handle_t scache_handle;
    uint8               *multicast_scache;
#endif /* BCM_WARM_BOOT_SUPPORT */

    SOC_IF_ERROR_RETURN
        (soc_hbx_mcast_size_get(unit, &mc_base, &mc_size));
    SOC_IF_ERROR_RETURN
        (soc_hbx_ipmc_size_get(unit, &ipmc_base, &ipmc_size));

    _bcm_fabric_mc_info[unit].mcast_size = mc_size;
    _bcm_fabric_mc_info[unit].ipmc_size = ipmc_size;

    if (NULL != _bcm_fabric_mc_info[unit].mcast_used) {
        sal_free(_bcm_fabric_mc_info[unit].mcast_used);
        _bcm_fabric_mc_info[unit].mcast_used = NULL;
    }
    alloc_size = SHR_BITALLOCSIZE(l2mc_table_size);
    _bcm_fabric_mc_info[unit].mcast_used = 
        sal_alloc(alloc_size, "Fabric MC entries used");
    if (NULL == _bcm_fabric_mc_info[unit].mcast_used) {
        return BCM_E_MEMORY;
    }
    sal_memset(_bcm_fabric_mc_info[unit].mcast_used, 0, alloc_size);

    alloc_size = ipmc_size * sizeof(uint8);
#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_MULTICAST, 0);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle,
                                     (0 == SOC_WARM_BOOT(unit)),
                                     alloc_size,
                                     &multicast_scache,
                                     BCM_FABRIC_IPMC_WB_DEFAULT_VERSION, NULL);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        }
    } /* Else, not enough memory, don't allocate scache */
#endif /* BCM_WARM_BOOT_SUPPORT */


    if (NULL == _fabric_multicast_ipmc_group_types[unit]) {
        _fabric_multicast_ipmc_group_types[unit] =
            sal_alloc(alloc_size, "fabric_multicast_group_types");
        if (NULL == _fabric_multicast_ipmc_group_types[unit]) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_fabric_multicast_ipmc_group_types[unit], 0, alloc_size);
    
#ifdef BCM_WARM_BOOT_SUPPORT
    multicast_mode_set[unit] = TRUE;

    /* Recover the used multicast entries */
    if (SOC_WARM_BOOT(unit)) {
        l2mc_entry_t *l2mc_entry, *l2mc_table;
        int index, index_min, index_max, l2mc_tbl_sz;

        /*
         * Go through L2MC table to find valid entries
         */
        index_min = soc_mem_index_min(unit, L2MCm);
        index_max = soc_mem_index_max(unit, L2MCm);

        l2mc_tbl_sz = sizeof(l2mc_entry_t) * l2mc_table_size;
        l2mc_table = soc_cm_salloc(unit, l2mc_tbl_sz, "l2mc tbl dma");
        if (l2mc_table == NULL) {
            sal_free(BCM_FABRIC_MC_USED_BITMAP(unit));
            return BCM_E_MEMORY;
        }

        memset((void *)l2mc_table, 0, l2mc_tbl_sz);
        if (soc_mem_read_range(unit, L2MCm, MEM_BLOCK_ANY,
                               index_min, index_max, l2mc_table) < 0) {
            sal_free(BCM_FABRIC_MC_USED_BITMAP(unit));
            soc_cm_sfree(unit, l2mc_table);
            return SOC_E_INTERNAL;
        }

        for (index = index_min; index <= index_max; index++) {

            l2mc_entry = soc_mem_table_idx_to_pointer(unit, L2MCm,
                                                      l2mc_entry_t *,
                                                      l2mc_table, index);

            if (!soc_mem_field32_get(unit, L2MCm, l2mc_entry, VALIDf)) {
                continue;
            }

            SHR_BITSET(BCM_FABRIC_MC_USED_BITMAP(unit), index);
        }

        soc_cm_sfree(unit, l2mc_table);
    }
    if (SOC_WARM_BOOT(unit)) {
        return (_bcm_fabric_multicast_reinit(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    SOC_IF_ERROR_RETURN(soc_mem_clear(unit, L2MCm, MEM_BLOCK_ALL, FALSE));

    if (SOC_IS_TITAN2X(unit)) {
        BCM_IF_ERROR_RETURN(bcm_xgs3_l2_addr_mcast_delete_all(unit));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_fabric_multicast_detach
 * Purpose:
 *      Detach the multicast module on XGS3 fabric devices.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_esw_fabric_multicast_detach(int unit)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    int autosync;
    BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchControlAutoSync, &autosync));
    if (autosync) {
        BCM_IF_ERROR_RETURN(_bcm_fabric_multicast_sync(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
    _bcm_fabric_mc_info[unit].mcast_size = 0;
    _bcm_fabric_mc_info[unit].ipmc_size = 0;

    if (NULL != _bcm_fabric_mc_info[unit].mcast_used) {
        sal_free(_bcm_fabric_mc_info[unit].mcast_used);
        _bcm_fabric_mc_info[unit].mcast_used = NULL;
    }

    if (NULL != _fabric_multicast_ipmc_group_types[unit]) {
        sal_free(_fabric_multicast_ipmc_group_types[unit]);
        _fabric_multicast_ipmc_group_types[unit] = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_fabric_multicast_create
 * Purpose:
 *      Create a multicast group on XGS3 fabric devices.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int 
_bcm_esw_fabric_multicast_create(int unit, uint32 flags, bcm_multicast_t *group)
{
    uint32 type;
    int search_index, fabric_index = -1;
    int index_min, index_max, rv;
    l2mc_entry_t	l2mc;

    /* On switch devices, we verify that the particular multicast group
     * type is valid and supported on the device.
     * We Must Not Do This on Fabrics!
     * Fabric devices must be able to pass all group types.  We translate
     * the type here to what is appropriate on the specific device HW.
     */
    type = flags & BCM_MULTICAST_TYPE_MASK;
    if (1 != _shr_popcount(type)) {
        return BCM_E_PARAM;
    }

    if (type == BCM_MULTICAST_TYPE_L2) {
        index_min = BCM_FABRIC_MCAST_BASE(unit);
        index_max =  BCM_FABRIC_MCAST_MAX(unit);
        if (flags & BCM_MULTICAST_WITH_ID) {
            if (!_BCM_MULTICAST_IS_L2(*group)) {
                /* Group ID doesn't match creation flag type */
                return BCM_E_PARAM;
            }
        }
    } else {
        index_min = BCM_FABRIC_IPMC_BASE(unit);
        index_max = BCM_FABRIC_IPMC_MAX(unit);
        if (flags & BCM_MULTICAST_WITH_ID) {
            if (_BCM_MULTICAST_IS_L2(*group)) {
                /* Group ID doesn't match creation flag type */
                return BCM_E_PARAM;
            }
        }
    }

    if (flags & BCM_MULTICAST_WITH_ID) {
        fabric_index = _BCM_MULTICAST_ID_GET(*group) + index_min;
        if ((fabric_index < index_min) || (fabric_index > index_max)) {
            /* Group index out of bounds */
            return BCM_E_PARAM;
        }
        if (SHR_BITGET(BCM_FABRIC_MC_USED_BITMAP(unit),
                       fabric_index)) {
            /* Requested group has already been created */
            return BCM_E_EXISTS;
        }
    } else {
        for (search_index = index_min; search_index <= index_max;
             search_index++) {
            if (!SHR_BITGET(BCM_FABRIC_MC_USED_BITMAP(unit),
                            search_index)) {
                fabric_index = search_index;
                break;
            }
        }
        if (-1 == fabric_index) {
            /* Out of free groups */
            return BCM_E_RESOURCE;
        }
    }

    /* Mark HW entry as valid */
    sal_memset(&l2mc, 0, sizeof(l2mc));
    soc_mem_field32_set(unit, L2MCm, &l2mc, VALIDf, 1);
    soc_mem_lock(unit, L2MCm);
    rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, fabric_index, &l2mc);
    soc_mem_unlock(unit, L2MCm);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Record group allocation */
    SHR_BITSET(BCM_FABRIC_MC_USED_BITMAP(unit), fabric_index);

    if (type == BCM_MULTICAST_TYPE_L2) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L2,
                                 fabric_index - index_min);
    } else if (type == BCM_MULTICAST_TYPE_L3) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L3,
                                 fabric_index - index_min);
    } else if (type == BCM_MULTICAST_TYPE_VPLS) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_VPLS,
                                 fabric_index - index_min);
    } else if (type == BCM_MULTICAST_TYPE_MIM) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_MIM,
                                 fabric_index - index_min);
    } else if (type == BCM_MULTICAST_TYPE_WLAN) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_WLAN,
                                 fabric_index - index_min);
    } else if (type == BCM_MULTICAST_TYPE_SUBPORT) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_SUBPORT,
                                 fabric_index - index_min);
    }else if (type == BCM_MULTICAST_TYPE_TRILL) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_TRILL,
                                 fabric_index - index_min);
    } else if (type == BCM_MULTICAST_TYPE_VLAN) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_VLAN,
                                 fabric_index - index_min);
    } else if (type == BCM_MULTICAST_TYPE_NIV) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_NIV,
                                 fabric_index - index_min);
    } else if (type == BCM_MULTICAST_TYPE_EGRESS_OBJECT) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_EGRESS_OBJECT,
                                 fabric_index - index_min);
    } else if (type == BCM_MULTICAST_TYPE_L2GRE) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L2GRE,
                                 fabric_index - index_min);
    } else if (type == BCM_MULTICAST_TYPE_VXLAN) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_VXLAN,
                                 fabric_index - index_min);
    } else if (type == BCM_MULTICAST_TYPE_FLOW) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_FLOW,
                                 fabric_index - index_min);
    } else if (type == BCM_MULTICAST_TYPE_EXTENDER) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_EXTENDER,
                                 fabric_index - index_min);
    }

    /* Record IPMC types for reference */
    if (type != BCM_MULTICAST_TYPE_L2) {
        BCM_IF_ERROR_RETURN(
                _bcm_fabric_multicast_ipmc_group_type_set(unit, *group));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_fabric_multicast_destroy
 * Purpose:
 *      Destroy a multicast group on XGS3 fabric devices.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int 
_bcm_esw_fabric_multicast_destroy(int unit, bcm_multicast_t group)
{
    int fabric_index = -1;
    int index_min, index_max, rv;
    l2mc_entry_t	l2mc;

    if (_BCM_MULTICAST_IS_L2(group)) {
        index_min = BCM_FABRIC_MCAST_BASE(unit);
        index_max =  BCM_FABRIC_MCAST_MAX(unit);
    } else {
        /* All non-L2 types are the same on fabric devices */
        index_min = BCM_FABRIC_IPMC_BASE(unit);
        index_max = BCM_FABRIC_IPMC_MAX(unit);
    }
    fabric_index = _BCM_MULTICAST_ID_GET(group) + index_min;

    if ((fabric_index < index_min) || (fabric_index > index_max)) {
        /* Group index out of bounds */
        return BCM_E_PARAM;
    }
    if (!SHR_BITGET(BCM_FABRIC_MC_USED_BITMAP(unit), fabric_index)) {
        /* Requested group has not been created */
        return BCM_E_NOT_FOUND;
    }

    /* Mark HW entry as invalid */
    sal_memset(&l2mc, 0, sizeof(l2mc));
    soc_mem_lock(unit, L2MCm);
    rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, fabric_index, &l2mc);
    soc_mem_unlock(unit, L2MCm);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Clear group allocation */
    SHR_BITCLR(BCM_FABRIC_MC_USED_BITMAP(unit), fabric_index);

    /* Free IPMC type record. fabric_index does not have type info */
    if (!_BCM_MULTICAST_IS_L2(group)) {
        BCM_IF_ERROR_RETURN(
                _bcm_fabric_multicast_ipmc_group_type_set(unit, group));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_fabric_multicast_group_get
 * Purpose:
 *      Retrieve the flags associated with a multicast group.
 * Parameters:
 *      unit - (IN) Unit number.
 *      group - (IN) Multicast group ID
 *      flags - (OUT) BCM_MULTICAST_*
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int 
_bcm_esw_fabric_multicast_group_get(int unit, bcm_multicast_t group,
                                    uint32 *flags)
{
    int mc_index , rv = BCM_E_NONE;
    uint32 type_flags;
    bcm_multicast_t get_group;

    /* Check if this is a known group */
    mc_index = _BCM_MULTICAST_ID_GET(group);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_fabric_multicast_param_check(unit, group, &mc_index));

    /* Check for the type of the multicast group */
    if (_BCM_MULTICAST_IS_L2(group)) {
        type_flags = BCM_MULTICAST_TYPE_L2;
    } else {
        if (!_BCM_MULTICAST_IS_L3(group) &&
                !_BCM_MULTICAST_IS_VPLS(group) &&
                !_BCM_MULTICAST_IS_MIM(group) &&
                !_BCM_MULTICAST_IS_SUBPORT(group) &&
                !_BCM_MULTICAST_IS_WLAN(group) &&
                !_BCM_MULTICAST_IS_VLAN(group) &&
                !_BCM_MULTICAST_IS_TRILL(group) &&
                !_BCM_MULTICAST_IS_NIV(group) &&
                !_BCM_MULTICAST_IS_L2GRE(group) &&
                !_BCM_MULTICAST_IS_VXLAN(group) &&
                !_BCM_MULTICAST_IS_FLOW(group) &&
                !_BCM_MULTICAST_IS_EGRESS_OBJECT(group) &&
                !_BCM_MULTICAST_IS_EXTENDER(group)) {
            return BCM_E_PARAM;
        }
        rv = _bcm_fabric_multicast_ipmc_group_type_get(unit, mc_index,
                &get_group);

        if (BCM_E_NOT_FOUND == rv) {
            type_flags = 0;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        } else if (get_group != group) {
            /* IPMC index is allocated, but not to the same group type */
            type_flags = 0;
            return BCM_E_NOT_FOUND;
        } else {
            type_flags =
                _bcm_esw_multicast_group_type_to_flags(
                        _BCM_MULTICAST_TYPE_GET(group));
        }
    }

    *flags = (type_flags | BCM_MULTICAST_WITH_ID);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_fabric_multicast_group_traverse
 * Purpose:
 *      Iterate over the defined multicast groups of the type
 *      specified in 'flags'.  If all types are desired, use
 *      MULTICAST_TYPE_MASK.
 * Parameters:
 *      unit - (IN) Unit number.
 *      trav_fn - (IN) Callback function.
 *      flags - (IN) BCM_MULTICAST_*
 *      user_data - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_esw_fabric_multicast_group_traverse(int unit,
                                 bcm_multicast_group_traverse_cb_t trav_fn, 
                                         uint32 flags, void *user_data)
{
    int search_index;
    int index_min, index_max;
    int rv = BCM_E_NONE;
    uint32 group_flags, flags_mask;
    bcm_multicast_t group;

    /* Create a flags_mask with all flags */
    flags_mask = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_TYPE_L3 |
                 BCM_MULTICAST_TYPE_VPLS | BCM_MULTICAST_TYPE_SUBPORT |
                 BCM_MULTICAST_TYPE_MIM | BCM_MULTICAST_TYPE_WLAN |
                 BCM_MULTICAST_TYPE_VLAN | BCM_MULTICAST_TYPE_TRILL |
                 BCM_MULTICAST_TYPE_NIV | BCM_MULTICAST_TYPE_EGRESS_OBJECT |
                 BCM_MULTICAST_TYPE_EXTENDER | BCM_MULTICAST_TYPE_VXLAN;


    if (0 == (flags & flags_mask)) {
        /* No recognized multicast types to traverse */
        return BCM_E_PARAM;
    }

    flags &= flags_mask;

    /* L2 groups */
    if (flags & BCM_MULTICAST_TYPE_L2) {
        index_min = BCM_FABRIC_MCAST_BASE(unit);
        index_max =  BCM_FABRIC_MCAST_MAX(unit);
        group_flags = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID;

        for (search_index = index_min; search_index <= index_max;
                search_index++) {
            if (SHR_BITGET(BCM_FABRIC_MC_USED_BITMAP(unit), search_index)) {
                _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_L2,
                        search_index);
                BCM_IF_ERROR_RETURN
                    ((*trav_fn)(unit, group, group_flags, user_data));
            }
        }
    }
        
    /* L3 groups */
    if (flags & ~(BCM_MULTICAST_TYPE_L2)) {
        index_min = BCM_FABRIC_IPMC_BASE(unit);
        index_max = BCM_FABRIC_IPMC_MAX(unit);

        for (search_index = index_min; search_index <= index_max;
                search_index++) {
            if (SHR_BITGET(BCM_FABRIC_MC_USED_BITMAP(unit), search_index)) {
                rv = _bcm_fabric_multicast_ipmc_group_type_get(unit,
                                        search_index - index_min, &group);

                if (BCM_E_NOT_FOUND == rv) {
                    continue;
                } else if (BCM_FAILURE(rv)) {
                    return rv;
                } else {
                    group_flags =
                        _bcm_esw_multicast_group_type_to_flags(
                                _BCM_MULTICAST_TYPE_GET(group)) |
                                BCM_MULTICAST_WITH_ID;

                    BCM_IF_ERROR_RETURN
                        ((*trav_fn)(unit, group, group_flags, user_data));
                }
            }
        }
    }

    return BCM_E_NONE;
}
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

/*
 * Function:
 *      bcm_esw_multicast_init
 * Purpose:
 *      Initialize the multicast module.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_multicast_init(int unit)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    multicast_mode_set[unit] = FALSE;
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        /* Fabrics must track the multicast groups here, since they
         * are not recorded in the mcast or IPMC modules. */
        BCM_IF_ERROR_RETURN(_bcm_esw_fabric_multicast_init(unit));
        multicast_initialized[unit] = TRUE;
        return BCM_E_NONE;
    }
#endif /* BCM_XGS3_FABRIC_SUPPORT */

#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        multicast_initialized[unit] = TRUE;
        return BCM_E_NONE;
    }
#endif /*BCM_SHADOW_SUPPORT*/

#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRX(unit)) {
        BCM_IF_ERROR_RETURN(bcm_xgs3_mcast_init(unit));
        BCM_IF_ERROR_RETURN(bcm_trx_multicast_init(unit));
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
    if (SOC_REG_IS_VALID(unit, IPMC_L3_MTUr)) {
        soc_reg_t reg;
        uint64 rval64, *rval64s[1];
        uint32 pindex;
        /* Create profile for IPMC_L3_MTU register */
        if (_bcm_mtu_profile[unit] == NULL) {
            _bcm_mtu_profile[unit] =
                sal_alloc(sizeof(soc_profile_reg_t), "MTU size Profile Reg");
            if (_bcm_mtu_profile[unit] == NULL) {
                return BCM_E_MEMORY;
            }
            soc_profile_reg_t_init(_bcm_mtu_profile[unit]);
            reg = IPMC_L3_MTUr;
            BCM_IF_ERROR_RETURN
                (soc_profile_reg_create(unit, &reg, 1, _bcm_mtu_profile[unit]));
            /* Add a default entry for backward comaptability */
            COMPILER_64_ZERO(rval64);
            COMPILER_64_SET(rval64, 0, 0x3fff);
            rval64s[0] = &rval64;
            BCM_IF_ERROR_RETURN
                (soc_profile_reg_add(unit, _bcm_mtu_profile[unit], rval64s,
                                     1, &pindex)); 
        }
    }
#endif

    multicast_initialized[unit] = TRUE;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_multicast_detach
 * Purpose:
 *      Shut down (uninitialize) the multicast module.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_multicast_detach(int unit)
{
#ifdef BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        /* Fabrics must track the multicast groups here, since they
         * are not recorded in the mcast or IPMC modules. */
        BCM_IF_ERROR_RETURN(_bcm_esw_fabric_multicast_detach(unit));
        multicast_initialized[unit] = FALSE;
        return BCM_E_NONE;
    }
#endif /* BCM_XGS3_FABRIC_SUPPORT */

#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRX(unit)) {
        BCM_IF_ERROR_RETURN(bcm_trx_multicast_detach(unit));
        multicast_initialized[unit] = FALSE;
        return BCM_E_NONE;
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */

    multicast_initialized[unit] = FALSE;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_multicast_create
 * Purpose:
 *      Allocate a multicast group index
 * Parameters:
 *      unit       - (IN)   Device Number
 *      flags      - (IN)   BCM_MULTICAST_*
 *      group      - (OUT)  Group ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_multicast_create(int unit, uint32 flags, bcm_multicast_t *group)
{
    int rv = BCM_E_UNAVAIL;

    MULTICAST_INIT_CHECK(unit);

#ifdef BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        return _bcm_esw_fabric_multicast_create(unit, flags, group);
    }
#endif /* BCM_XGS3_FABRIC_SUPPORT */

#if defined(BCM_XGS3_SWITCH_SUPPORT) 
    if (SOC_IS_XGS3_SWITCH(unit)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        /* Check for unsupported multicast interface types */
        if (SOC_IS_TOMAHAWK3(unit)) {
            uint32 unsupp_flags;

            unsupp_flags = BCM_MULTICAST_TYPE_VPLS |
                           BCM_MULTICAST_TYPE_SUBPORT |
                           BCM_MULTICAST_TYPE_MIM |
                           BCM_MULTICAST_TYPE_WLAN |
                           BCM_MULTICAST_TYPE_VLAN |
                           BCM_MULTICAST_TYPE_TRILL |
                           BCM_MULTICAST_TYPE_NIV |
                           BCM_MULTICAST_TYPE_L2GRE |
                           BCM_MULTICAST_TYPE_VXLAN |
                           BCM_MULTICAST_TYPE_EXTENDER |
                           BCM_MULTICAST_TYPE_MAC |
                           BCM_MULTICAST_TYPE_PORTS_GROUP |
                           BCM_MULTICAST_DISABLE_SRC_KNOCKOUT |
                           BCM_MULTICAST_INGRESS_GROUP |
                           BCM_MULTICAST_EGRESS_GROUP;

            if (flags & unsupp_flags) {
                return BCM_E_UNAVAIL;
            }
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */
        rv = _bcm_esw_multicast_create(unit, flags, group);

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(INCLUDE_L3)
        if (BCM_SUCCESS(rv) && !multicast_mode_set[unit]) {
            /* Notify IPMC module that we're operating in multicast mode
             * for Warm Boot purposes */
            BCM_IF_ERROR_RETURN
                (_bcm_esw_ipmc_repl_wb_flags_set(unit,
                                                 _BCM_IPMC_WB_MULTICAST_MODE,
                                                 _BCM_IPMC_WB_MULTICAST_MODE));
            multicast_mode_set[unit] = TRUE;
        }
#endif /* BCM_WARM_BOOT_SUPPORT && INCLUDE_L3 */
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return rv;
}

/*
 * Function:
 *      bcm_multicast_destroy
 * Purpose:
 *      Free a multicast group index
 * Parameters:
 *      unit       - (IN) Device Number
 *      group      - (IN) Group ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_multicast_destroy(int unit, bcm_multicast_t group)
{
    MULTICAST_INIT_CHECK(unit);

    if (!_BCM_MULTICAST_IS_SET(group)) {
        return BCM_E_PARAM;
    }

#ifdef BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        return _bcm_esw_fabric_multicast_destroy(unit, group);
    }
#endif /* BCM_XGS3_FABRIC_SUPPORT */

#if defined(BCM_XGS3_SWITCH_SUPPORT) 
    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (_BCM_MULTICAST_IS_L2(group)) {
            return _bcm_esw_multicast_l2_destroy(unit, group);
        } else {
#if defined(INCLUDE_L3)
            return _bcm_esw_multicast_l3_destroy(unit, group);
#endif
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmi_esw_multicast_multi_alloc
 * Purpose:
 *      Create/alloc multiple multicast groups
 * Parameters:
 *      unit          - (IN) Unit number.
 *      info          - (IN) info for creating groups.
 *      base_mc_group - (IN/OUT) base group number
 *                               of multiple groups.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_multicast_multi_alloc(int unit, bcm_multicast_multi_info_t info,
                              bcm_multicast_t *base_mc_group)
{
    int rv = BCM_E_NONE;

#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    int rv2 = BCM_E_NONE;
    ipmc_entry_t entry;
    int num_entries = info.number_of_elements;
    int i=0,j=0; /* iterators */
    bcm_multicast_t group;
    int flags=0, type;
    uint32 ipmc_id = 0;

    IPMC_INIT(unit);

    /* dont allow more than 4 groups allocation */
    if ((info.number_of_elements < 1) ||
        (info.number_of_elements > 4)) {
        return BCM_E_PARAM;
    }

    IPMC_LOCK(unit);

    /* create multi consecutive multicast groups */
    if (info.flags & BCM_MULTICAST_MULTI_WITH_ID) {
        ipmc_id = _BCM_MULTICAST_ID_GET(*base_mc_group);
        if ((ipmc_id >= (IPMC_GROUP_NUM(unit)) - num_entries) ||
            (ipmc_id < 1)) {

            IPMC_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        i = ipmc_id;
        for (j=0; j < num_entries; j++) {
            if ((ipmc_id + j) == IPMC_GROUP_NUM(unit)) {
                IPMC_UNLOCK(unit);
                return BCM_E_FULL;
            }

            if (IPMC_USED_ISSET(unit, ipmc_id + j)) {
                IPMC_UNLOCK(unit);
                return BCM_E_EXISTS;
            }
        }
    } else {
        for (i = 0; i < (IPMC_GROUP_NUM(unit) - num_entries); i++) {
            if (!IPMC_USED_ISSET(unit, i)) {
                for (j=1; j<num_entries ; j++) {
                    if(IPMC_USED_ISSET(unit, (i+j))) {
                        break;
                    }
                }
            }
            if (j == num_entries) {
                break;
            }
        }
        if (j < num_entries) {
            IPMC_UNLOCK(unit);
            return BCM_E_FULL;
        }
    }

    IPMC_UNLOCK(unit);
    /*
     * we only support 2 type of groups till now.
     * make L3 type as default
     */
    if (info.flags & BCM_MULTICAST_MULTI_TYPE_VPLS) {
        flags |= BCM_MULTICAST_TYPE_VPLS;
        type = _BCM_MULTICAST_TYPE_VPLS;
    } else {
        flags |= BCM_MULTICAST_TYPE_L3;
        type = _BCM_MULTICAST_TYPE_L3;
    }

    flags |= BCM_MULTICAST_WITH_ID;
    for (j=0; j<num_entries; j++) {
        group = i + j;
        rv = bcm_esw_multicast_create(unit, flags, &group);
        if (BCM_FAILURE(rv)) {
            /* we need to clear all the groups that we have created here */
            /* reduce one iteration as this is not successful */
            j--;
            goto create_cleanup;
        }
        IPMC_USED_SET(unit, i+j);
        if (j == 0) {
            *base_mc_group = group;
        }
        rv = soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, i+j, &entry);

        if (BCM_FAILURE(rv)) {
            goto create_cleanup;
        }
        soc_L3_IPMCm_field32_set(unit, &entry, VALIDf, 1);
        rv = soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ALL, i+j, &entry);
        if (BCM_FAILURE(rv)) {
            goto create_cleanup;
        }
    }
    rv =  bcmi_multicast_multi_count_set(unit, *base_mc_group, num_entries);
    if (BCM_FAILURE(rv)) {
        goto create_cleanup;
    }

    return BCM_E_NONE;

create_cleanup:
    if (BCM_FAILURE(rv)) {
        while (j >= 0) {
            _BCM_MULTICAST_GROUP_SET(group, type, i+j);
            rv2 = bcm_esw_multicast_destroy(unit, group);
            LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                 "destroy for multicast group 0x%x failed.  error : %s \n"),
                 group, bcm_errmsg(rv2)));

             IPMC_USED_CLR(unit, i+j);
            /*
             * Capturing the error but not returning back.
             * We should return the original error.
             * But even if this fails, we will have to keep moving
             * to report the original error to user.
             * Displaying one error message.
             */
            rv2 = soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, i+j, &entry);
            LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                 "mem read for L3_IPMCm at index %d failed.  error : %s \n"),
                 i+j, bcm_errmsg(rv2)));

            soc_L3_IPMCm_field32_set(unit, &entry, VALIDf, 0);
            rv2 = soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ALL, i+j, &entry);
            LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                 "mem write for L3_IPMCm at index %d failed.  error : %s \n"),
                 i+j, bcm_errmsg(rv2)));
            /* reduce the interator for next cycle */
            j--;
        }
        *base_mc_group = -1;
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcmi_esw_multicast_multi_free
 * Purpose:
 *      Free multiple multicast groups
 * Parameters:
 *      unit          - (IN) Unit number.
 *      base_mc_group - (IN) base group number
 *                               of multiple groups.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_multicast_multi_free(int unit, bcm_multicast_t base_mc_group)
{
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)

    int rv;
    int num_entries = 4;
    int i = 0; /* iterators */
    int ipmc_id = _BCM_MULTICAST_ID_GET(base_mc_group);

    if (!_BCM_MULTICAST_IS_SET(base_mc_group)) {
        return BCM_E_PARAM;
    }

    if ((ipmc_id < 0) || (ipmc_id >= soc_mem_index_count(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

    IPMC_INIT(unit);

    if(!(IPMC_USED_ISSET(unit, ipmc_id))) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_multicast_multi_count_get(unit, base_mc_group, &num_entries));

    if (num_entries == 0) {
        /* nothing to be done. just return */
        return BCM_E_NONE;
    }

    for (; i< num_entries; i++) {
        IPMC_USED_CLR(unit, (ipmc_id + i));
        rv = bcm_esw_multicast_destroy(unit, base_mc_group + i);

        if (BCM_FAILURE(rv)) {
            return rv;
        }

    }
#endif
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_multicast_l3_encap_get
 * Purpose:
 *      Get the Encap ID for L3.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      group - (IN) Multicast group ID.
 *      port  - (IN) Physical port.
 *      intf  - (IN) L3 interface ID.
 *      encap_id - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_multicast_l3_encap_get(int unit, bcm_multicast_t group,
                               bcm_gport_t port, bcm_if_t intf,
                               bcm_if_t *encap_id)
{
    MULTICAST_INIT_CHECK(unit);

    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }

#ifdef BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        /* Encap ID is not used for fabric devices in XGS3 */
        *encap_id = BCM_IF_INVALID;
        return BCM_E_NONE;
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (BCM_GPORT_IS_VLAN_PORT(port) ||
            BCM_GPORT_IS_NIV_PORT(port) ||
            BCM_GPORT_IS_EXTENDER_PORT(port)) {
#ifdef BCM_TRIDENT2_SUPPORT
            if (soc_feature(unit, soc_feature_virtual_port_routing)) {
                return bcm_td2_multicast_l3_vp_encap_get(unit, group, port,
                        intf, encap_id);
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                return BCM_E_UNAVAIL;
            }
        }

#ifdef BCM_GREYHOUND_SUPPORT
        if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
            /* intf for this feature must be egress_nh index
             * Note: intf=BCM_IF_INVALID is special process for adding
             *      l2mc egress. this value must be handled.
             */
            if (intf != BCM_IF_INVALID) {
                if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf)){
                    return bcm_esw_multicast_egress_object_encap_get(unit,
                           group, intf, encap_id);
                }
            }
        }
#endif /* BCM_GREYHOUND_SUPPORT */

        /* For L3 IPMC, encap_id is simply the L3 interface ID */
        *encap_id = intf;
        return BCM_E_NONE;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_multicast_l2_encap_get
 * Purpose:
 *      Get the Encap ID for L2.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      group    - (IN) Multicast group ID.
 *      port     - (IN) Physical port.
 *      vlan     - (IN) Vlan.
 *      encap_id - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_multicast_l2_encap_get(int unit, bcm_multicast_t group,
                               bcm_gport_t port, bcm_vlan_t vlan,
                               bcm_if_t *encap_id)
{
    MULTICAST_INIT_CHECK(unit);
    
    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }

    *encap_id = BCM_IF_INVALID;

#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_XGS3_SWITCH(unit)) {

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
            /* Encap ID is used for L2 in XGS for TD2+CoE GPORT-SUBPORT
               types */
            return _bcmi_coe_multicast_subport_encap_get(unit, port, encap_id);

        } else
#endif
		{
			return BCM_E_NONE;
		}
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_multicast_trill_encap_get
 * Purpose:
 *      Get the Encap ID for a TRILL port.
 * Parameters:
 *      unit         - (IN) Unit number.
 *      group        - (IN) Multicast group ID.
 *      port         - (IN) Physical port.
 *      intf          - (IN) L3 interface ID.
 *      encap_id     - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_multicast_trill_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                 bcm_if_t intf, bcm_if_t *encap_id)
{
    MULTICAST_INIT_CHECK(unit);

    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_trill)) { 
        /* For TRILL IPMC, encap_id is simply the L3 interface ID */
        *encap_id = intf;
        return BCM_E_NONE;
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_multicast_vpls_encap_get
 * Purpose:
 *      Get the Encap ID for a MPLS port.
 * Parameters:
 *      unit         - (IN) Unit number.
 *      group        - (IN) Multicast group ID.
 *      port         - (IN) Physical port.
 *      mpls_port_id - (IN) MPLS port ID.
 *      encap_id     - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_multicast_vpls_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                 bcm_gport_t mpls_port_id, bcm_if_t *encap_id)
{
    MULTICAST_INIT_CHECK(unit);
    
    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TD_TT(unit) || SOC_IS_TRIUMPH2(unit) ||
            SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit) ||
            SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr2_multicast_vpls_encap_get(unit, group, port, 
                                               mpls_port_id, encap_id);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TR_VL(unit) && !SOC_IS_HURRICANEX(unit) && 
        !SOC_IS_GREYHOUND2(unit)) {
        return bcm_tr_multicast_vpls_encap_get(unit, group, port, 
                                               mpls_port_id, encap_id);
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_multicast_subport_encap_get
 * Purpose:
 *      Get the Encap ID for a subport.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      group     - (IN) Multicast group ID.
 *      port      - (IN) Physical port.
 *      subport   - (IN) Subport ID.
 *      encap_id  - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_multicast_subport_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                    bcm_gport_t subport, bcm_if_t *encap_id)
{
    MULTICAST_INIT_CHECK(unit);

    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TD_TT(unit) || SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) || SOC_IS_KATANAX(unit) ||
        SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr2_multicast_subport_encap_get(unit, group, port, 
                                                  subport, encap_id);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRX(unit) && !SOC_IS_SCORPION(unit) && !SOC_IS_HURRICANE(unit) && 
        !SOC_IS_HURRICANE2(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
        return bcm_tr_multicast_subport_encap_get(unit, group, port, 
                                                  subport, encap_id);
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_multicast_mim_encap_get
 * Purpose:
 *      Get the Encap ID for MiM.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      group     - (IN) Multicast group ID.
 *      port      - (IN) Physical port.
 *      mim_port  - (IN) MiM port ID.
 *      encap_id  - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_multicast_mim_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                bcm_gport_t mim_port, bcm_if_t *encap_id)
{
    MULTICAST_INIT_CHECK(unit);

    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_mim)) {
        return bcm_tr2_multicast_mim_encap_get(unit, group, port, 
                                              mim_port, encap_id);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_multicast_wlan_encap_get
 * Purpose:
 *      Get the Encap ID for WLAN.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      group     - (IN) Multicast group ID.
 *      port      - (IN) Physical port.
 *      wlan_port - (IN) WLAN port ID.
 *      encap_id  - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_multicast_wlan_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                 bcm_gport_t wlan_port, bcm_if_t *encap_id)
{
    MULTICAST_INIT_CHECK(unit);
    
    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_wlan)) {
        return bcm_tr2_multicast_wlan_encap_get(unit, group, port, 
                                               wlan_port, encap_id);
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_multicast_vlan_encap_get
 * Purpose:
 *      Get the Encap ID for VLAN virtual port.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      group     - (IN) Multicast group ID.
 *      port      - (IN) Physical port.
 *      vlan_port - (IN) VLAN port ID.
 *      encap_id  - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_multicast_vlan_encap_get(int unit, bcm_multicast_t group,
                                 bcm_gport_t port, bcm_gport_t vlan_port_id,
                                 bcm_if_t *encap_id)
{
    MULTICAST_INIT_CHECK(unit);
    
    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vp)) {
        int vp;
        ing_dvp_table_entry_t dvp;

        if (!BCM_GPORT_IS_VLAN_PORT(vlan_port_id)) {
            return BCM_E_PARAM;
        }
        vp = BCM_GPORT_VLAN_PORT_ID_GET(vlan_port_id); 
        if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
            return BCM_E_PARAM;
        }
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

        /* Next-hop index is used for multicast replication */
        *encap_id = (bcm_if_t) soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
        if (!SOC_IS_ENDURO(unit)) {
            *encap_id += BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        }
        return BCM_E_NONE;
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_multicast_niv_encap_get
 * Purpose:
 *      Get the Encap ID for NIV virtual port.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      group       - (IN) Multicast group ID.
 *      port        - (IN) Physical port.
 *      niv_port_id - (IN) NIV port ID.
 *      encap_id    - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_multicast_niv_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                bcm_gport_t niv_port_id, bcm_if_t *encap_id)
{
    MULTICAST_INIT_CHECK(unit);
    
    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_niv)) {
        int vp;
        ing_dvp_table_entry_t dvp;

        if (!BCM_GPORT_IS_NIV_PORT(niv_port_id)) {
            return BCM_E_PARAM;
        }
        vp = BCM_GPORT_NIV_PORT_ID_GET(niv_port_id); 
        if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
            return BCM_E_PARAM;
        }
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

        /* Next-hop index is used for multicast replication */
        *encap_id = (bcm_if_t) soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
        *encap_id += BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

        return BCM_E_NONE;
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_multicast_egress_object_encap_get
 * Purpose:
 *      Get the Encap ID for Egress_object
 * Parameters:
 *      unit        - (IN) Unit number.
 *      group       - (IN) Multicast group ID.
 *      intf          - (IN) Egress Object ID.
 *      encap_id    - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_multicast_egress_object_encap_get(int unit, bcm_multicast_t group, bcm_if_t intf,
                                bcm_if_t *encap_id)
{
    MULTICAST_INIT_CHECK(unit);
    
    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)
#if defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_l3)) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf)) {
            if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
                *encap_id = intf;
            } else {
                *encap_id = (intf - BCM_XGS3_EGRESS_IDX_MIN(unit)) + 
                        BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
            }
        } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf)) {
            *encap_id = intf;
        } else if (BCM_XGS3_PROXY_EGRESS_IDX_VALID(unit, intf)) {
            *encap_id = (intf - BCM_XGS3_PROXY_EGRESS_IDX_MIN(unit)) + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            return BCM_E_PARAM;
        }
        return BCM_E_NONE;
    }
#endif  /* INCLUDE_L3 */
#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_multicast_l2gre_encap_get
 * Purpose:
 *      Get the Encap ID for L2GRE virtual port
 * Parameters:
 *      unit        - (IN) Unit number.
 *      group       - (IN) Multicast group ID.
 *      port        - (IN) Physical port.
 *      l2gre_port_id - (IN) L2GRE port ID.
 *      encap_id    - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_multicast_l2gre_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                bcm_gport_t l2gre_port_id, bcm_if_t *encap_id)
{
	if (encap_id == NULL) {
	    return BCM_E_PARAM;
	}

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_l2gre)) {
        int vp;
        ing_dvp_table_entry_t dvp;

        if (!BCM_GPORT_IS_L2GRE_PORT(l2gre_port_id)) {
            return BCM_E_PARAM;
        }
        /* Get Multicast DVP index */
        vp = BCM_GPORT_L2GRE_PORT_ID_GET(l2gre_port_id); 
        if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
            return BCM_E_PARAM;
        }
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
            return BCM_E_PARAM;
        }
        /* Multicast DVP will never point to ECMP group */
        BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
        if (soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf)) {
            return BCM_E_PARAM;
        }

        /* Next-hop index is used for multicast replication */
        *encap_id = (bcm_if_t) soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
        *encap_id += BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

        return BCM_E_NONE;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_esw_multicast_vxlan_encap_get
 * Purpose:
 *      Get the Encap ID for VXLAN virtual port
 * Parameters:
 *      unit        - (IN) Unit number.
 *      group       - (IN) Multicast group ID.
 *      port        - (IN) Physical port.
 *      vxlan_port_id - (IN) L2GRE port ID.
 *      encap_id    - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_multicast_vxlan_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                bcm_gport_t vxlan_port_id, bcm_if_t *encap_id)
{
#if defined(INCLUDE_L3)
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    int vp = -1;
#endif /* BCM_TRIDENT2_SUPPORT || BCM_GREYHOUND2_SUPPORT */
#endif /* INCLUDE_L3 */

    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vxlan)) {
        ing_dvp_table_entry_t dvp;

        if (!BCM_GPORT_IS_VXLAN_PORT(vxlan_port_id)) {
            return BCM_E_PARAM;
        }

        /* Get Multicast DVP index */
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port_id); 
        if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
            return BCM_E_PARAM;
        }

        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
            return BCM_E_PARAM;
        }

        /* Multicast DVP will never point to ECMP group */
        BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

        /* Next-hop index is used for multicast replication */
        *encap_id = (bcm_if_t) soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
        *encap_id += BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

        return BCM_E_NONE;
    } else
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        int ecmp = 0, nh_ecmp_index = 0;

        if (!BCM_GPORT_IS_VXLAN_PORT(vxlan_port_id)) {
            return BCM_E_PARAM;
        }

        /* Get Multicast DVP index */
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port_id);
        if (vp == -1) {
            return BCM_E_PARAM;
        }

        if (!bcmi_gh2_vxlan_vp_used_get(unit, vp)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_port_to_nh_ecmp_index(unit, vp,
                                                 &ecmp, &nh_ecmp_index));
        if (ecmp) {
            return BCM_E_PARAM;
        }

        /* Next-hop index is used for multicast replication */
        *encap_id = (bcm_if_t)nh_ecmp_index;
        *encap_id += BCM_XGS3_EGRESS_IDX_MIN(unit);

        return BCM_E_NONE;
    } else
#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3 */
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_multicast_extender_encap_get
 * Purpose:
 *      Get the Encap ID for Extender virtual port.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      group       - (IN) Multicast group ID.
 *      port        - (IN) Physical port.
 *      extender_port_id - (IN) Extender port ID.
 *      encap_id    - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_multicast_extender_encap_get(int unit, bcm_multicast_t group,
        bcm_gport_t port, bcm_gport_t extender_port_id, bcm_if_t *encap_id)
{
    MULTICAST_INIT_CHECK(unit);

    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {
        int vp;
        ing_dvp_table_entry_t dvp;

        if (!BCM_GPORT_IS_EXTENDER_PORT(extender_port_id)) {
            return BCM_E_PARAM;
        }
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(extender_port_id); 
        if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
            return BCM_E_PARAM;
        }
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

        /* Next-hop index is used for multicast replication */
        *encap_id = (bcm_if_t) soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                NEXT_HOP_INDEXf);
        *encap_id += BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

        return BCM_E_NONE;
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_multicast_mac_encap_get
 * Purpose:
 *      Get the Encap ID for MAC virtual port.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      group       - (IN) Multicast group ID.
 *      port        - (IN) Physical port.
 *      mac_port_id - (IN) MAC port ID.
 *      encap_id    - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_multicast_mac_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                bcm_gport_t mac_port_id, bcm_if_t *encap_id)
{
    MULTICAST_INIT_CHECK(unit);
    
    if (encap_id == NULL) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_mac_virtual_port)) {
        int vp;
        ing_dvp_table_entry_t dvp;

        if (!BCM_GPORT_IS_MAC_PORT(mac_port_id)) {
            return BCM_E_PARAM;
        }
        vp = BCM_GPORT_MAC_PORT_ID_GET(mac_port_id); 
        if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

        /* Next-hop index is used for multicast replication */
        *encap_id = (bcm_if_t) soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                NEXT_HOP_INDEXf);
        *encap_id += BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

        return BCM_E_NONE;
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_multicast_egress_add
 * Purpose:
 *      Add a GPORT to the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_multicast_egress_add(int unit, bcm_multicast_t group, 
                             bcm_gport_t port, bcm_if_t encap_id)
{
    int rv=BCM_E_NONE;

    MULTICAST_INIT_CHECK(unit);

   /* Check whether the given multicast group is available */
    rv = bcm_esw_multicast_group_is_free(unit, group);
    if (rv != BCM_E_EXISTS) {
        if (BCM_SUCCESS(rv)) {
            rv = BCM_E_NOT_FOUND;
        }
        return rv;
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT) 
    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (_BCM_MULTICAST_L2_OR_FABRIC(unit, group)) {
            return _bcm_esw_multicast_l2_add(unit, group, port, encap_id);
        } else {
#if defined(INCLUDE_L3)
            /* If (1) the given group is an IPMC group, (2) the given port is
             * a trunk gport, and (3) IPMC trunk resolution is disabled for the
             * trunk group in hardware, then encap_id is added to only one
             * member of the trunk group. The following procedure finds which
             * trunk member the encap_id is mapped to.
             */
            bcm_gport_t mapped_port;
            BCM_IF_ERROR_RETURN
                (_bcm_esw_multicast_egress_encap_id_to_trunk_member_map(unit,
                    group, 1, &port, &encap_id, &mapped_port));
            return _bcm_esw_multicast_l3_add(unit, group, mapped_port, encap_id);
#endif /* INCLUDE_L3 */
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_multicast_egress_delete
 * Purpose:
 *      Delete GPORT from the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_multicast_egress_delete(int unit, bcm_multicast_t group, 
                                bcm_gport_t port, bcm_if_t encap_id)
{
    int rv=BCM_E_NONE;

    MULTICAST_INIT_CHECK(unit);

    /* Check whether the given multicast group is available */
    rv = bcm_esw_multicast_group_is_free(unit, group);
    if (rv != BCM_E_EXISTS) {
        if (BCM_SUCCESS(rv)) {
            rv = BCM_E_NOT_FOUND;
        }
        return rv;
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT) 
    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (_BCM_MULTICAST_L2_OR_FABRIC(unit, group)) {
            return _bcm_esw_multicast_l2_delete(unit, group, port, encap_id);
        } else {
#if defined(INCLUDE_L3)
            /* If (1) the given group is an IPMC group, (2) the given port is
             * a trunk gport, and (3) IPMC trunk resolution is disabled for
             * the trunk group in hardware, then encap_id resides on only one
             * member of the trunk group. The following procedure finds which
             * trunk member the encap_id resides on.
             */
            bcm_gport_t mapped_port;
            BCM_IF_ERROR_RETURN
                (_bcm_esw_multicast_egress_mapped_trunk_member_find(unit,
                    group, port, encap_id, &mapped_port));
            return _bcm_esw_multicast_l3_delete(unit, group, mapped_port, encap_id);
#endif /* INCLUDE_L3 */
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_multicast_egress_delete_all
 * Purpose:
 *      Delete all replications for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_multicast_egress_delete_all(int unit, bcm_multicast_t group)
{ 
    int rv=BCM_E_NONE;
    MULTICAST_INIT_CHECK(unit);

    /* Check whether the given multicast group is available */
    rv = bcm_esw_multicast_group_is_free(unit, group);
    if (rv != BCM_E_EXISTS) {
        if (BCM_SUCCESS(rv)) {
            rv = BCM_E_NOT_FOUND;
        }
        return rv;
    }

    rv = bcm_esw_multicast_egress_set(unit, group, 0, NULL, NULL);
#if defined(INCLUDE_L3) && defined(BCM_KATANA_SUPPORT)
    if ((SOC_IS_SABER2(unit)|| SOC_IS_KATANAX(unit)) && BCM_SUCCESS(rv)) {
        return _bcm_esw_multicast_egress_subscriber_delete_all(unit, group);
    }
#endif
    return rv;
}
/*  
 * Function:
 *      bcm_multicast_egress_set
 * Purpose:
 *      Assign the complete set of egress GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit       - (IN) Device Number
 *      group      - (IN) Multicast group ID
 *      port_count - (IN) Number of ports in replication list
 *      port_array - (IN) List of GPORT Identifiers
 *      encap_id_array - (IN) List of encap identifiers
 * Returns:
 *      BCM_E_XXX
 */     
int     
bcm_esw_multicast_egress_set(int unit, bcm_multicast_t group, int port_count,
                             bcm_gport_t *port_array, bcm_if_t *encap_id_array)
{
    int rv=BCM_E_NONE;

    MULTICAST_INIT_CHECK(unit);

    /* Check whether the given multicast group is available */
    rv = bcm_esw_multicast_group_is_free(unit, group);
    if (rv != BCM_E_EXISTS) {
        if (BCM_SUCCESS(rv)) {
            rv = BCM_E_NOT_FOUND;
        }
        return rv;
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        /* If (1) the given group is an IPMC group, (2) the given port is a
         * trunk gport, and (3) IPMC trunk resolution is disabled for the
         * trunk group in hardware, then each encap_id is added to only one
         * member in trunk group. The following procedure maps the encap_ids
         * to trunk members.
         */
        int rv;
        bcm_gport_t *mapped_port_array = NULL;
        if (port_count > 0) {
            mapped_port_array = sal_alloc(sizeof(bcm_gport_t) * port_count,
                    "resolved port array");
            if (NULL == mapped_port_array) {
                return BCM_E_MEMORY;
            }
            rv = _bcm_esw_multicast_egress_encap_id_to_trunk_member_map(unit,
                    group, port_count, port_array, encap_id_array,
                    mapped_port_array);
            if (BCM_FAILURE(rv)) {
                sal_free(mapped_port_array);
                return rv;
            }
        }

        rv = _bcm_esw_multicast_egress_set(unit, group, port_count, 
                mapped_port_array, encap_id_array);

        if (mapped_port_array) {
            sal_free(mapped_port_array);
        }

        return rv;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}   

#if (defined(BCM_TRX_SUPPORT) || defined(BCM_XGS3_SWITCH_SUPPORT)) && defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_esw_multicast_egress_get
 * Purpose:
 *      Helper function to API 
 * Parameters: 
 *      unit           - (IN) Device Number
 *      mc_index       - (IN) Multicast index
 *      port_max       - (IN) Number of entries in "port_array"
 *      port_array     - (OUT) List of ports
 *      encap_id_array - (OUT) List of encap identifiers
 *      port_count     - (OUT) Actual number of ports returned
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the input parameter port_max = 0, return in the output parameter
 *      port_count the total number of ports/encapsulation IDs in the 
 *      specified multicast group's replication list.
 */
STATIC int     
_bcm_esw_multicast_egress_get(int unit, bcm_multicast_t group, int port_max,
                              bcm_gport_t *port_array,
                              bcm_if_t *encap_id_array, 
                              int *port_count)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT) 
    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (_BCM_MULTICAST_L2_OR_FABRIC(unit, group)) {
            return _bcm_esw_multicast_l2_get(unit, group, port_max, port_array, 
                                             encap_id_array, port_count);
        } else {
#if defined(INCLUDE_L3)
            return _bcm_esw_multicast_l3_get(unit, group, port_max, port_array, 
                                             encap_id_array, port_count);
#endif /* INCLUDE_L3 */
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return BCM_E_UNAVAIL;
}
#endif

/*
 * Function:
 *      bcm_multicast_egress_get
 * Purpose:
 *      Retrieve a set of egress multicast GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters: 
 *      unit           - (IN) Device Number
 *      mc_index       - (IN) Multicast index
 *      port_max       - (IN) Number of entries in "port_array"
 *      port_array     - (OUT) List of ports
 *      encap_id_array - (OUT) List of encap identifiers
 *      port_count     - (OUT) Actual number of ports returned
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the input parameter port_max = 0, return in the output parameter
 *      port_count the total number of ports/encapsulation IDs in the 
 *      specified multicast group's replication list.
 */
int     
bcm_esw_multicast_egress_get(int unit, bcm_multicast_t group, int port_max,
                             bcm_gport_t *port_array,
                             bcm_if_t *encap_id_array, 
                             int *port_count)
{
    int rv = BCM_E_UNAVAIL;
#if (defined(BCM_TRX_SUPPORT) || defined(BCM_XGS3_SWITCH_SUPPORT)) && defined(INCLUDE_L3)
    int i;
    bcm_module_t mod_out, my_modid;
    bcm_port_t port_in, port_out;
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    int is_local_subport = FALSE;
#endif
#if defined(BCM_TOMAHAWK_SUPPORT) || defined (BCM_TRIDENT2PLUS_SUPPORT)
    int r_rv;
    int tgid;
    bcm_module_t  local_modid = 0;
    int trunk_local_ports = 0;
    bcm_trunk_info_t tinfo;
    #if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
        bcm_port_t      local_trunk_member_ports[SOC_MAX_NUM_PP_PORTS] = {0};
        int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
    #else
        bcm_port_t      local_trunk_member_ports[SOC_MAX_NUM_PORTS] = {0};
        int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif    
#endif

    MULTICAST_INIT_CHECK(unit);

    /* Check whether the given multicast group is available */
    rv = bcm_esw_multicast_group_is_free(unit, group);
    if (rv != BCM_E_EXISTS) {
        if (BCM_SUCCESS(rv)) {
            rv = BCM_E_NOT_FOUND;
        }
        return rv;
    }

    /* port_array and encap_id_array are allowed to be NULL */
    if ((NULL == port_count) || (port_max < 0) ){
        return BCM_E_PARAM;
    }

    /* If port_max = 0, port_array and encap_id_array must be NULL */
    if (port_max == 0) {
        if ((NULL != port_array) || (NULL != encap_id_array)) {
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_multicast_egress_get(unit, group, port_max, port_array,
                                      encap_id_array, port_count));
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (!SOC_IS_TOMAHAWK3(unit)) {
        if ((BCM_MC_PER_TRUNK_REPL_MODE(unit)) && (NULL != port_array)) {
            BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get (unit, &local_modid));
            r_rv = bcm_esw_trunk_find(unit, local_modid, *port_array, &tgid);
            if ((BCM_E_NONE == r_rv) && (BCM_TRUNK_INVALID != tgid)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_trunk_get(unit, tgid, &tinfo, 0, NULL, NULL));
                if (!(tinfo.flags & BCM_TRUNK_FLAG_IPMC_CLEAVE)) {
                    BCM_IF_ERROR_RETURN(
                    _bcm_esw_trunk_local_members_get(unit, tgid,
                                                     max_num_ports,
                                                     local_trunk_member_ports,
                                                     &trunk_local_ports));
                    for (i = 0; i < trunk_local_ports; i++) {
                        BCM_IF_ERROR_RETURN
                        (bcm_esw_port_gport_get(unit, local_trunk_member_ports[i],
                                                (port_array + i)));
                        encap_id_array[i] = encap_id_array[0];
                    }
                }
            }
        }
    } /* if (!SOC_IS_TOMAHAWK3(unit)) */
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */
    if (NULL != port_array) {
        /* Convert GPORT_LOCAL format to GPORT_MODPORT format */
        if (bcm_esw_stk_my_modid_get(unit, &my_modid) < 0) {
            return BCM_E_INTERNAL;
        }
        for (i = 0; i < *port_count; i++) {

#if defined(BCM_HGPROXY_COE_SUPPORT)
			if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                (soc_feature(unit, soc_feature_channelized_switching))) &&
				BCM_GPORT_IS_SET(port_array[i]) &&
				_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_array[i])) {

				if(_bcm_xgs5_subport_coe_gport_local(unit, port_array[i])) {
					is_local_subport = TRUE;
				}
			} else
#endif
#if defined(BCM_KATANA2_SUPPORT)
			/* For LinkPHY/SubTag subport return in subport_gport format*/
			if (soc_feature(unit, soc_feature_linkphy_coe) ||
				soc_feature(unit, soc_feature_subtag_coe)) {
				is_local_subport = FALSE;
				if (BCM_GPORT_IS_SET(port_array[i]) &&
					_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit,
					port_array[i])) {
					is_local_subport = TRUE;
				}
			}
#endif

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
            if (is_local_subport) {

            } else
#endif
            {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_local_get(unit, port_array[i], &port_in));
        
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                            my_modid, port_in, &mod_out, &port_out));
                BCM_GPORT_MODPORT_SET(port_array[i], mod_out, port_out);
            }
        }
    }

    rv = BCM_E_NONE;
#endif

    return rv;
}   



int
bcm_esw_multicast_repl_set(int unit, int mc_index, bcm_port_t port,
                      bcm_vlan_vector_t vlan_vec)
{
    return BCM_E_UNAVAIL;
}

int
bcm_esw_multicast_repl_get(int unit, int index, bcm_port_t port,
                      bcm_vlan_vector_t vlan_vec)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_multicast_group_get
 * Purpose:
 *      Retrieve the flags associated with a multicast group.
 * Parameters:
 *      unit - (IN) Unit number.
 *      group - (IN) Multicast group ID
 *      flags - (OUT) BCM_MULTICAST_*
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_multicast_group_get(int unit, bcm_multicast_t group, uint32 *flags)
{
    MULTICAST_INIT_CHECK(unit);

#ifdef BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        return _bcm_esw_fabric_multicast_group_get(unit, group, flags);
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRX(unit)) {
        return bcm_trx_multicast_group_get(unit, group, flags);
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        return bcm_xgs3_multicast_group_get(unit, group, flags);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_multicast_group_is_free
 * Purpose:
 *      Request if the given multicast group is available on the
 *      device
 * Parameters:
 *      unit - (IN) Unit number.
 *      group - (IN) Multicast group ID.
 * Returns:
 *      BCM_E_NONE - multicast group is valid and available on the device
 *      BCM_E_EXISTS - multicast group is valid but already in use
 *                     on this device
 *      BCM_E_PARAM - multicast group is not valid on this device 
 * Notes:
 */
int 
bcm_esw_multicast_group_is_free(int unit, bcm_multicast_t group)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    int             mc_index;
    int             rv = BCM_E_PARAM;
    int             is_set;

    mc_index = _BCM_MULTICAST_ID_GET(group);

#ifdef BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        rv = _bcm_esw_fabric_multicast_param_check(unit, group, &mc_index);
        /* Remap return codes */
        if (BCM_E_NOT_FOUND == rv) {
            /* Multicast group is available, return without error */
            rv = BCM_E_NONE;
        } else if (BCM_E_NONE == rv) {
            /* Multicast group is already configured */
            rv = BCM_E_EXISTS;
        }
    } else 
#endif	/* BCM_XGS3_FABRIC_SUPPORT */
    if (_BCM_MULTICAST_IS_L2(group)) {
        rv = _bcm_xgs3_l2mc_index_is_set(unit, mc_index, &is_set);
        if (BCM_SUCCESS(rv)) {
            if (is_set) {
                rv = BCM_E_EXISTS;
            }
        }
#if defined(INCLUDE_L3)
    } else if (_BCM_MULTICAST_IS_SET(group)) {
        uint32 flags;
        uint8 group_type;

        group_type = _BCM_MULTICAST_TYPE_GET(group);
        flags = _bcm_esw_multicast_group_type_to_flags(group_type);
        /* Check if this group type is supported on this device. */
        BCM_IF_ERROR_RETURN(_bcm_esw_multicast_type_validate(unit, flags));

        rv = bcm_xgs3_ipmc_id_is_set(unit, mc_index, &is_set);
        if (BCM_SUCCESS(rv)) {
            if (is_set) {
                rv = BCM_E_EXISTS;
            }
        }
#endif /* INCLUDE_L3 */
    }

    return rv; 
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_XGS3_SWITCH_SUPPORT */
}

/*
 * Function:
 *      bcm_esw_multicast_group_free_range_get
 * Purpose:
 *      Retrieve the minimum and maximum unallocated multicast groups
 *      for a given multicast type.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type_flag - (IN) One of BCM_MULTICAST_TYPE_*.
 *      group_min - (OUT) Minimum available multicast group of specified type.
 *      group_max - (OUT) Maximum available multicast group of specified type.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_multicast_group_free_range_get(int unit, uint32 type_flag, 
                                       bcm_multicast_t *group_min,
                                       bcm_multicast_t *group_max)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    bcm_multicast_t dev_min, dev_max, group, free_min, free_max;
    int             index_min, index_max, group_type, min_group_index;
    int             rv = BCM_E_PARAM;
    uint32          type;

    type = type_flag & BCM_MULTICAST_TYPE_MASK;
    if (1 != _shr_popcount(type)) {
        return BCM_E_PARAM;
    }

#ifdef BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        if (type_flag == BCM_MULTICAST_TYPE_L2) {
            index_min = BCM_FABRIC_MCAST_BASE(unit);
            index_max =  BCM_FABRIC_MCAST_MAX(unit);
            min_group_index = 1;
        } else {
            index_min = BCM_FABRIC_IPMC_BASE(unit);
            index_max = BCM_FABRIC_IPMC_MAX(unit);
            min_group_index = 0;
        }
    } else 
#endif	/* BCM_XGS3_FABRIC_SUPPORT */
    if (type_flag == BCM_MULTICAST_TYPE_L2) {
        index_min = soc_mem_index_min(unit, L2MCm);
        index_max = soc_mem_index_max(unit, L2MCm);
        min_group_index = 1;
    } else {
        index_min = soc_mem_index_min(unit, L3_IPMCm);
        index_max = soc_mem_index_max(unit, L3_IPMCm);
        min_group_index = 0;
    }

    group_type = _bcm_esw_multicast_flags_to_group_type(type_flag);
    _BCM_MULTICAST_GROUP_SET(dev_min, group_type, min_group_index);
    _BCM_MULTICAST_GROUP_SET(dev_max, group_type, index_max - index_min);


    free_min = free_max = 0; /* Invalid multicast group */
    for (group = dev_min; group <= dev_max; group++) {
        rv = bcm_esw_multicast_group_is_free(unit, group);
        if (BCM_SUCCESS(rv)) {
            if (0 == free_min) {
                free_min = group;
            }
            free_max = group;
        } else if (BCM_E_EXISTS == rv) {
            /* Nothing to do but clear the error */
            rv = BCM_E_NONE;
        } else {
            /* Real error, return */
            break;
        }
    }

    if (BCM_SUCCESS(rv)) {
        if (0 == free_min) {
            /* No available groups of this type */
            return BCM_E_NOT_FOUND;
        } else {
            /* Copy the results */
            *group_min = free_min;
            *group_max = free_max;
        }
    }

    return rv; 
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_XGS3_SWITCH_SUPPORT */
}

/*
 * Function:
 *      bcm_esw_multicast_group_traverse
 * Purpose:
 *      Iterate over the defined multicast groups of the type
 *      specified in 'flags'.  If all types are desired, use
 *      MULTICAST_TYPE_MASK.
 * Parameters:
 *      unit - (IN) Unit number.
 *      trav_fn - (IN) Callback function.
 *      flags - (IN) BCM_MULTICAST_*
 *      user_data - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_multicast_group_traverse(int unit,
                                 bcm_multicast_group_traverse_cb_t trav_fn, 
                                 uint32 flags, void *user_data)
{
    MULTICAST_INIT_CHECK(unit);

#ifdef BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        return _bcm_esw_fabric_multicast_group_traverse(unit, trav_fn,
                                                        flags, user_data);
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TRX(unit)) {
        return bcm_trx_multicast_group_traverse(unit, trav_fn,
                                                flags, user_data);
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        return bcm_xgs3_multicast_group_traverse(unit, trav_fn,
                                                 flags, user_data);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_esw_multicast_remap_group_set
 * Purpose:
 *      Set an IPMC remapping.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      from      - (IN) Multicast group ID to map from
 *      to        - (IN) Multicast group ID to map to
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_esw_multicast_remap_group_set(int unit, bcm_multicast_t from, int to)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_ipmc_remap)) {

        rv = _bcm_esw_multicast_l3_group_check(unit, from, NULL);
        BCM_IF_ERROR_RETURN(rv);

        rv = _bcm_esw_multicast_l3_group_check(unit, to, NULL);
        BCM_IF_ERROR_RETURN(rv);

        rv = bcm_tr2_ipmc_remap_set(unit, from, to);
    }
#endif

    return rv;
}


/*
 * Function:
 *      _bcm_esw_multicast_remap_group_get
 * Purpose:
 *      Set an IPMC remapping.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      from      - (IN) Multicast group ID being mapped
 *      to        - (OUT) Pointer to Multicast group ID being mapped to,
 *                        using the same group type as 'from'.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_esw_multicast_remap_group_get(int unit, bcm_multicast_t from, int *to)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_ipmc_remap)) {
        
        rv = _bcm_esw_multicast_l3_group_check(unit, from, NULL);
        if (BCM_SUCCESS(rv)) {
            rv = bcm_tr2_ipmc_remap_get(unit, from, to);
        }
    }
#endif

    return rv;
}
#endif

/*
 * Function:
 *      bcm_multicast_control_set
 * Purpose:
 *      Set multicast group control.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      group     - (IN) Multicast group ID.
 *      type      - (IN) Multicast group control type.
 *      arg       - (IN) Multicast group control argument.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_multicast_control_set(int unit,
        bcm_multicast_t group,
        bcm_multicast_control_t type,
        int arg)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_XGS3_SWITCH_SUPPORT) 
#if defined(INCLUDE_L3)
    int index;
    mtu_values_entry_t l3_mtu_values_entry;
#endif

    MULTICAST_INIT_CHECK(unit);

    if (!SOC_IS_XGS3_SWITCH(unit)) {
        return BCM_E_UNAVAIL;
    }

#if defined(INCLUDE_L3)
    rv = (_bcm_esw_multicast_l3_group_check(unit, group, NULL));
#endif

    if (!_BCM_MULTICAST_IS_L2(group) && BCM_FAILURE(rv)) {
        return rv;
    }
    
    switch (type) {
        case bcmMulticastControlMtu:
            if (_BCM_MULTICAST_IS_L2(group)) {
                /* Not applicable to multicast types that uses a L2MC index.
                 * Only applicable to multicast types that uses a L3MC index.
                 */
                return BCM_E_PARAM;
            }
            if (SOC_MEM_IS_VALID(unit, L3_MTU_VALUESm)) {
#if defined(INCLUDE_L3) 
                index = _BCM_MULTICAST_MTU_TABLE_OFFSET(unit) +
                        _BCM_MULTICAST_ID_GET(group);
                if (!soc_mem_index_valid(unit, L3_MTU_VALUESm, index)) {
                    return BCM_E_PARAM;
                }

                SOC_IF_ERROR_RETURN(READ_L3_MTU_VALUESm(unit, MEM_BLOCK_ANY,
                            index, &l3_mtu_values_entry));

                /* coverity[zero_return : FALSE] */
                /* coverity[large_shift : FALSE] */
                if(!SOC_MEM_FIELD32_VALUE_FIT(unit, L3_MTU_VALUESm,
                            MTU_SIZEf, arg)) {
                    return BCM_E_PARAM;
                }
                soc_L3_MTU_VALUESm_field32_set(unit, &l3_mtu_values_entry,
                        MTU_SIZEf, arg);

                rv = WRITE_L3_MTU_VALUESm(unit, MEM_BLOCK_ALL, 
                                          index, &l3_mtu_values_entry);
#endif /* INCLUDE_L3 */
            } else if (SOC_MEM_FIELD_VALID(unit, L3_IPMCm, IPMC_MTU_INDEXf) && 
                       SOC_REG_IS_VALID(unit, IPMC_L3_MTUr)) {
#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
#if defined(INCLUDE_L3) 
                uint64 rval64, *rval64s[1];
                uint32 pindex, entry[SOC_MAX_MEM_WORDS];

                rval64s[0] = &rval64;
                COMPILER_64_ZERO(rval64);
                COMPILER_64_SET(rval64, 0, arg);
                index = _BCM_MULTICAST_ID_GET(group);
                if (!soc_mem_index_valid(unit, L3_IPMCm, index)) {
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN
                    (soc_profile_reg_add(unit, _bcm_mtu_profile[unit], rval64s,
                                         1, &pindex));
                BCM_IF_ERROR_RETURN
                    (soc_mem_read(unit, L3_IPMCm,
                                  MEM_BLOCK_ANY, index, &entry));
                soc_mem_field32_set(unit, L3_IPMCm, &entry,
                                    IPMC_MTU_INDEXf, pindex);
                BCM_IF_ERROR_RETURN
                    (soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ANY, index, 
                                   &entry));
#endif /* INCLUDE_L3 */
#endif
            } else if (soc_feature(unit, soc_feature_l3_nh_attr_table)) {
#if defined(BCM_TRIUMPH3_SUPPORT)
#if defined(INCLUDE_L3) 
                int mtu_profile_index;
                ing_l3_next_hop_attribute_1_index_entry_t mtu_idx_value_buf;
                void *null_idx_entry =
                    soc_mem_entry_null(unit, ING_L3_NEXT_HOP_ATTRIBUTE_1_INDEXm);

                index = _BCM_TR3_MULTICAST_MTU_TABLE_OFFSET(unit) +
                        _BCM_MULTICAST_ID_GET(group);

                if (!soc_mem_index_valid(unit, 
                            ING_L3_NEXT_HOP_ATTRIBUTE_1_INDEXm, index)) {
                    return BCM_E_PARAM;
                }

                /* coverity[zero_return : FALSE] */
                /* coverity[large_shift : FALSE] */
                if (!SOC_MEM_FIELD32_VALUE_FIT(unit, 
                            ING_L3_NEXT_HOP_ATTRIBUTE_1m,
                            MTU_SIZEf, arg)) {
                    return (BCM_E_PARAM);
                }

                BCM_IF_ERROR_RETURN(
                        _bcm_tr3_mtu_profile_index_get(unit, arg,
                            (uint32 *)&mtu_profile_index));

                sal_memcpy(&mtu_idx_value_buf, null_idx_entry,
                        sizeof(ing_l3_next_hop_attribute_1_index_entry_t));
                soc_mem_field32_set(unit, ING_L3_NEXT_HOP_ATTRIBUTE_1_INDEXm,
                        &mtu_idx_value_buf, INDEXf, mtu_profile_index);

                rv = WRITE_ING_L3_NEXT_HOP_ATTRIBUTE_1_INDEXm(unit, MEM_BLOCK_ALL,
                        index, &mtu_idx_value_buf);

#endif /* INCLUDE_L3 */
#endif /* BCM_TRIUMPH3_SUPPORT */
            } else {
                return BCM_E_UNAVAIL;
            }
            break;

        case bcmMulticastVpTrunkResolve:
            if (_BCM_MULTICAST_IS_L2(group)) {
                /* Not applicable to multicast types that uses a L2MC index. */
                return BCM_E_PARAM;
            }
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
            if (soc_feature(unit, soc_feature_vp_lag)) {
                egr_ipmc_entry_t egr_ipmc_entry;

                index = _BCM_MULTICAST_ID_GET(group);
                BCM_IF_ERROR_RETURN(READ_EGR_IPMCm(unit, MEM_BLOCK_ANY, index,
                            &egr_ipmc_entry));
                soc_EGR_IPMCm_field32_set(unit, &egr_ipmc_entry,
                        ENABLE_VPLAG_RESOLUTIONf, arg ? TRUE : FALSE);
                BCM_IF_ERROR_RETURN(WRITE_EGR_IPMCm(unit, MEM_BLOCK_ALL, index,
                            &egr_ipmc_entry));
            } else
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
            {
                return BCM_E_PARAM;
            }
            break;

#if defined(INCLUDE_L3)
        case bcmMulticastRemapGroup:
            rv = _bcm_esw_multicast_remap_group_set(unit, group, arg);
            break;
#endif

        default:
            return BCM_E_PARAM;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return rv;
}

/*
 * Function:
 *      bcm_multicast_control_get
 * Purpose:
 *      Get multicast group control.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      group     - (IN) Multicast group ID.
 *      type      - (IN) Multicast group control type.
 *      arg       - (OUT) Multicast group control argument.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_multicast_control_get(int unit,
        bcm_multicast_t group,
        bcm_multicast_control_t type,
        int *arg)
{
    int rv = BCM_E_UNAVAIL;
    

#if defined(BCM_XGS3_SWITCH_SUPPORT) 
#if defined(INCLUDE_L3)
    mtu_values_entry_t l3_mtu_values_entry;
    int index;
#endif

    MULTICAST_INIT_CHECK(unit);

    if (!SOC_IS_XGS3_SWITCH(unit)) {
        return BCM_E_UNAVAIL;
    }

#if defined(INCLUDE_L3)
    rv = (_bcm_esw_multicast_l3_group_check(unit, group, NULL));
#endif

    if (!_BCM_MULTICAST_IS_L2(group) && BCM_FAILURE(rv)) {
        return rv;
    }

    switch (type) {
        case bcmMulticastControlMtu:
            if (_BCM_MULTICAST_IS_L2(group)) {
                /* Not applicable to multicast types that uses a L2MC index.
                 * Only applicable to multicast types that uses a L3MC index.
                 */
                return BCM_E_PARAM;
            }

            if (SOC_MEM_IS_VALID(unit, L3_MTU_VALUESm)) {
#if defined(INCLUDE_L3) 
                index = _BCM_MULTICAST_MTU_TABLE_OFFSET(unit) +
                        _BCM_MULTICAST_ID_GET(group);
                if (!soc_mem_index_valid(unit, L3_MTU_VALUESm, index)) {
                    return BCM_E_PARAM;
                }

                SOC_IF_ERROR_RETURN(READ_L3_MTU_VALUESm(unit, MEM_BLOCK_ANY,
                            index, &l3_mtu_values_entry));
                *arg = soc_L3_MTU_VALUESm_field32_get(unit,
                        &l3_mtu_values_entry, MTU_SIZEf);
                /* if no error assume success */
                rv = BCM_E_NONE;
#endif /* INCLUDE_L3 */
            } else if (SOC_MEM_FIELD_VALID(unit, L3_IPMCm, IPMC_MTU_INDEXf) && 
                       SOC_REG_IS_VALID(unit, IPMC_L3_MTUr)) {
#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
#if defined(INCLUDE_L3) 
                uint64 rval64, *rval64s[1];
                uint32 pindex, entry[SOC_MAX_MEM_WORDS];

                rval64s[0] = &rval64;
                index = _BCM_MULTICAST_ID_GET(group);
                if (!soc_mem_index_valid(unit, L3_IPMCm, index)) {
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN
                    (soc_mem_read(unit, L3_IPMCm,
                                  MEM_BLOCK_ANY, index, &entry));
                pindex = soc_mem_field32_get(unit, L3_IPMCm, &entry,
                                             IPMC_MTU_INDEXf);
                BCM_IF_ERROR_RETURN
                    (soc_profile_reg_get(unit, _bcm_mtu_profile[unit], pindex,
                                         1, rval64s));
                *arg = COMPILER_64_LO(rval64);
#endif /* INCLUDE_L3 */
#endif
            } else if (soc_feature(unit, soc_feature_l3_nh_attr_table)) {
#if defined(BCM_TRIUMPH3_SUPPORT)
#if defined(INCLUDE_L3) 

                /* Buffer for mtu idx */
                ing_l3_next_hop_attribute_1_index_entry_t mtu_idx_value_buf;
                /* Buffer for mtu profile table entry */
                ing_l3_next_hop_attribute_1_entry_t mtu_value_buf;
                int mtu_profile_index;

                index = _BCM_TR3_MULTICAST_MTU_TABLE_OFFSET(unit) +
                        _BCM_MULTICAST_ID_GET(group);

                if (!soc_mem_index_valid(unit, 
                            ING_L3_NEXT_HOP_ATTRIBUTE_1_INDEXm, index)) {
                    return BCM_E_PARAM;
                }

                BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit,
                            ING_L3_NEXT_HOP_ATTRIBUTE_1_INDEXm,
                            index,
                            (uint32 *)&mtu_idx_value_buf));

                mtu_profile_index =
                    soc_mem_field32_get(unit, ING_L3_NEXT_HOP_ATTRIBUTE_1_INDEXm,
                            (uint32 *)&mtu_idx_value_buf, INDEXf);

                BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, 
                            ING_L3_NEXT_HOP_ATTRIBUTE_1m,
                            mtu_profile_index, (uint32 *)&mtu_value_buf));
                *arg = 
                    soc_mem_field32_get(unit, ING_L3_NEXT_HOP_ATTRIBUTE_1m,
                            (uint32 *)&mtu_value_buf, MTU_SIZEf);

                /* if no error assume success */
                rv = BCM_E_NONE;
#endif /* INCLUDE_L3 */
#endif /* BCM_TRIUMPH3_SUPPORT */
            } else {
                return BCM_E_UNAVAIL;
            }
            break;

        case bcmMulticastVpTrunkResolve:
            if (_BCM_MULTICAST_IS_L2(group)) {
                /* Not applicable to multicast types that uses a L2MC index. */
                return BCM_E_PARAM;
            }
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
            if (soc_feature(unit, soc_feature_vp_lag)) {
                egr_ipmc_entry_t egr_ipmc_entry;

                index = _BCM_MULTICAST_ID_GET(group);
                BCM_IF_ERROR_RETURN(READ_EGR_IPMCm(unit, MEM_BLOCK_ANY, index,
                            &egr_ipmc_entry));
                *arg = soc_EGR_IPMCm_field32_get(unit, &egr_ipmc_entry,
                        ENABLE_VPLAG_RESOLUTIONf);
            } else
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
            {
                return BCM_E_PARAM;
            }
            break;

#if defined(INCLUDE_L3)
        case bcmMulticastRemapGroup:
            rv = _bcm_esw_multicast_remap_group_get(unit, group, arg);
            break;
#endif

        default:
            return BCM_E_PARAM;
    }

#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return rv;
}

/*
 * Function:
 *      _bcm_esw_ipmc_subscriber_egress_intf_add
 * Purpose:
 *      Helper function to add ipmc egress interface on different devices.
 * Parameters:
 *      unit       - (IN)   Device Number
 *      ipmc_id    - (IN)   ipmc id
 *      port       - (IN)  Port number
 *      nh_index   - (IN)  Next Hop index
 *      subscriber_queue - (IN) Subscriber gport
 *      is_l3      - (IN)  Identifier of L2/L3 IPMC interface, relevant only for FB
 *      queue_count - (OUT) number of queues in the extended queue list
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_esw_ipmc_subscriber_egress_intf_add(int unit, int ipmc_id, 
                                         bcm_port_t port,
                                         int nh_index, 
                                         bcm_gport_t subscriber_queue,
                                         int is_l3,
                                         int *queue_count,
                                         int *q_count_increased)
{
#if defined (INCLUDE_L3)
#if defined(BCM_KATANA_SUPPORT) 
    int id;

#if defined(BCM_SABER2_SUPPORT) 
    if (SOC_IS_SABER2(unit)) {
        id  = nh_index;
        return _bcm_sb2_ipmc_subscriber_egress_intf_add(unit, ipmc_id, port, 
                                                id, subscriber_queue, is_l3,
                                                queue_count, q_count_increased);
    }
#endif 
    if (SOC_IS_KATANAX(unit)) {
        id = nh_index;

        return _bcm_kt_ipmc_subscriber_egress_intf_set(unit, ipmc_id, port, 1,
                                                &id, subscriber_queue, is_l3,
                                                queue_count, q_count_increased);
    }
#endif /* BCM_KATANA_SUPPORT */
#endif /* INCLUDE_L3 */
        return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_ipmc_subscriber_egress_intf_delete
 * Purpose:
 *      Helper function to add ipmc egress interface on different devices.
 * Parameters:
 *      unit       - (IN)   Device Number
 *      ipmc_id    - (IN)   ipmc id
 *      port       - (IN)  Port number
 *      nh_index   - (IN)  Next Hop index
 *      subscriber_queue - (IN) Subscriber gport
 *      is_l3      - (IN)  Identifier of L2/L3 IPMC interface, relevant only for FB
 *      queue_count - (OUT) number of queues in the extended queue list
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_esw_ipmc_subscriber_egress_intf_delete(int unit, int ipmc_id, 
                                            bcm_port_t port,
                                            int nh_index, 
                                            bcm_gport_t subscriber_queue, 
                                            int is_l3,
                                            int *queue_count,
                                            int *q_count_decreased)
{
#if defined (INCLUDE_L3)
    
#if defined(BCM_KATANA_SUPPORT)
#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        return _bcm_sb2_ipmc_subscriber_egress_intf_delete(unit, ipmc_id, port, 
                                                nh_index, subscriber_queue,
                                                queue_count, q_count_decreased);

    }
#endif 

    if (SOC_IS_KATANAX(unit)) {
        return _bcm_kt_ipmc_subscriber_egress_intf_delete(unit, ipmc_id, port, 
                                                nh_index, subscriber_queue,
                                                queue_count, q_count_decreased);
    }
#endif /* BCM_KATANA_SUPPORT */
#endif /* INCLUDE_L3 */
        return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_ipmc_subscriber_egress_intf_set
 * Purpose:
 *      Helper function to add ipmc egress interface on different devices.
 * Parameters:
 *      unit       - (IN)   Device Number
 *      ipmc_id    - (IN)   ipmc id
 *      port       - (IN)  Port number
 *      nh_index   - (IN)  Next Hop index
 *      subscriber_queue - (IN) Subscriber gport
 *      is_l3      - (IN)  Identifier of L2/L3 IPMC interface, relevant only for FB
 *      queue_count - (OUT) number of queues in the extended queue list
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_esw_ipmc_subscriber_egress_intf_set(int unit, int ipmc_id, 
                                         bcm_port_t port,
                                         int if_count,
                                         int *if_array, 
                                         bcm_gport_t subscriber_queue, 
                                         int is_l3,
                                         int *queue_count,
                                         int *q_count_increased)
{
#if defined (INCLUDE_L3)
#if defined(BCM_KATANA_SUPPORT)
#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        return _bcm_sb2_ipmc_subscriber_egress_intf_set(unit, ipmc_id, port, 
                                                if_count, if_array, 
                                                subscriber_queue, is_l3,
                                                queue_count,
                                                q_count_increased);

    }
#endif 

    if (SOC_IS_KATANAX(unit)) {
        return _bcm_kt_ipmc_subscriber_egress_intf_set(unit, ipmc_id, port, 
                                                if_count, if_array, 
                                                subscriber_queue, is_l3,
                                                queue_count,
                                                q_count_increased);
    }
#endif /* BCM_KATANA_SUPPORT */
#endif /* INCLUDE_L3 */
        return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_ipmc_subscriber_egress_intf_get
 * Purpose:
 *      Helper function to add ipmc egress interface on different devices.
 * Parameters:
 *      unit       - (IN)   Device Number
 *      ipmc_id    - (IN)   ipmc id
 *      port       - (IN)  Port number
 *      nh_index   - (IN)  Next Hop index
 *      subscriber_queue - (IN) Subscriber gport
 *      is_l3      - (IN)  Identifier of L2/L3 IPMC interface, relevant only for FB
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_esw_ipmc_subscriber_egress_intf_get(int unit, bcm_multicast_t group,
                                         int port_count, 
                                         bcm_gport_t *port_array,
                                         bcm_if_t *encap_id_array,
                                         bcm_gport_t *subscriber_queue_array, 
                                         int *count) 
{
#if defined (INCLUDE_L3)
#if defined(BCM_KATANA_SUPPORT) 
#if defined(BCM_SABER2_SUPPORT) 
    if (SOC_IS_SABER2(unit)) {
        return _bcm_sb2_ipmc_subscriber_egress_intf_get(unit, group, port_count,
                                                port_array, encap_id_array, 
                                                subscriber_queue_array, count);
    }
#endif 

    if (SOC_IS_KATANAX(unit)) {
        return _bcm_kt_ipmc_subscriber_egress_intf_get(unit, group, port_count,
                                                port_array, encap_id_array, 
                                                subscriber_queue_array, count);
    }
#endif /* BCM_KATANA_SUPPORT */
#endif /* INCLUDE_L3 */
        return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_ipmc_mmu_mc_remap_ptr
 * Purpose:
 *      Helper function to set/get ipmc mmu remap pointer
 * Parameters:
 *      unit       - (IN)   Device Number
 *      ipmc_id    - (IN)   ipmc id
 *      remap_id - (IN/OUT) set/get remap id
 *      set          -  (IN) set/get input
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_esw_ipmc_mmu_mc_remap_ptr(int unit, int ipmc_id, 
                                           int *remap_id, int set)
{
#if defined (INCLUDE_L3)
#if defined(BCM_KATANA_SUPPORT) 
    if (SOC_IS_KATANAX(unit)) {
        return _bcm_kt_ipmc_mmu_mc_remap_ptr(unit, ipmc_id, 
                                             remap_id, set); 
    }
#endif /* BCM_KATANA_SUPPORT */
#endif /* INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_ipmc_set_remap_group
 * Purpose:
 *      Helper function to set/get ipmc mmu remap pointer
 * Parameters:
 *      unit       - (IN)   Device Number
 *      ipmc_id    - (IN)   ipmc id
 *      port      - (IN)  port id
 *      count          -  (IN) count
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_esw_ipmc_set_remap_group(int unit, int ipmc_id, 
                                       bcm_port_t port_out, 
                                       int count)
{
#if defined (INCLUDE_L3)
#if defined(BCM_KATANA_SUPPORT) 

#if defined(BCM_SABER2_SUPPORT) 
    if (SOC_IS_SABER2(unit)) {
        return _bcm_sb2_ipmc_set_remap_group(unit, ipmc_id, 
                                    port_out, count); 
    } 
#endif 

    if (SOC_IS_KATANAX(unit)) {
        return _bcm_kt_ipmc_set_remap_group(unit, ipmc_id, 
                                    port_out, count); 
    }
#endif /* BCM_KATANA_SUPPORT */
#endif /* INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

#if defined(BCM_KATANA_SUPPORT)

/* Macro to free memory and return code instead of using goto */

#define BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(_r_)   \
    if (local_port_array) {                             \
        sal_free(local_port_array);                     \
    }                                                   \
    if (temp_encap_list) {                              \
        sal_free(temp_encap_list);                      \
        temp_encap_list = NULL;                         \
    }                                                   \
    return _r_;

/*
 * Function:
 *      _bcm_esw_multicast_egress_subscriber_set
 * Purpose:
 *      Set the  replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port_max - (IN) Maximum ports
 *      port_array      - (IN) GPORT array
 *      encap_array  - (IN) Encap ID array.
 *      subscriber_array - (IN) Subscriber array
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_multicast_egress_subscriber_set(int unit, bcm_multicast_t group,
                                         int port_count, 
                                         bcm_gport_t *port_array,
                                         bcm_if_t *encap_id_array,
                                         bcm_gport_t *subscriber_queue_array) 
{
    int mc_index, i, rv = BCM_E_NONE;
    int gport_id, *temp_encap_list = NULL;
    bcm_port_t *local_port_array = NULL;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_pbmp_t l2_pbmp;
    l2mc_entry_t l2mc_entry; 
    bcm_trunk_t trunk_id;
    soc_mem_t   mem;
    int modid_local = 0;
#if defined (INCLUDE_L3)
    int list_count;
    bcm_pbmp_t mc_eligible_pbmp, e_pbmp;
    bcm_port_t port_iter;
    int q_id = 0;
    int remap_index;
    int q_count = 0;
    int q_count_increased = 0;
#endif /* INCLUDE_L3 */
#if defined(BCM_KATANA2_SUPPORT)
    int pp_port;
#endif
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    int is_local_subport = FALSE;
#endif

    /* Input parameters check. */
    mc_index = _BCM_MULTICAST_ID_GET(group);
    mem = (_BCM_MULTICAST_L2_OR_FABRIC(unit, group)) ? L2MCm : L3_IPMCm; 
    
    /* BCM_XGS3_FABRIC_SUPPORT */
    if ((mc_index < 0) || (mc_index >= soc_mem_index_count(unit, mem))) {
        return (BCM_E_PARAM);
    }

    if (port_count > 0) {
        if (NULL == port_array) {
            return (BCM_E_PARAM);
        }
        if (!_BCM_MULTICAST_L2_OR_FABRIC(unit, group) && 
            (NULL == encap_id_array || NULL == subscriber_queue_array)) {
            return (BCM_E_PARAM);
        }
    } else {
        return (BCM_E_PARAM);
    }   

#if defined (INCLUDE_L3)
    rv = _bcm_esw_multicast_l3_group_check(unit, group, NULL);
#endif /* INCLUDE_L3 */
    if (BCM_FAILURE(rv) && (0 == _BCM_MULTICAST_IS_L2(group))) {
         return BCM_E_PARAM;
    }

    /* Convert GPORT array into local port numbers */
    if (port_count > 0) {
        local_port_array =
            sal_alloc(sizeof(bcm_port_t) * port_count, "local_port array");
        if (NULL == local_port_array) {
            BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(BCM_E_MEMORY);
        }
        
        temp_encap_list =
            sal_alloc(sizeof(int) * port_count, "temp_encap_list");
        if (NULL == temp_encap_list) {
            BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(rv);
        }
    }

    for (i = 0; i < port_count ; i++) {
        rv = _bcm_esw_gport_resolve(unit, port_array[i],
                                    &mod_out, &port_out,
                                    &trunk_id, &gport_id); 
        if (BCM_FAILURE(rv)) {
            BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(rv);
        }

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            BCM_GPORT_IS_SET(port_array[i]) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_array[i])) {

            if(_bcm_xgs5_subport_coe_gport_local(unit, port_array[i])) {
                is_local_subport = TRUE;
                rv = _bcmi_coe_subport_physical_port_get(unit,
                                                        port_array[i],
                                                        &local_port_array[i]);
                if(rv != BCM_E_NONE) {
                    BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(BCM_E_PORT);
                }
            } else {
                BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(BCM_E_PORT);
            }

        } else
#endif

#if defined(BCM_KATANA2_SUPPORT)
        if (soc_feature(unit, soc_feature_linkphy_coe) ||
            soc_feature(unit, soc_feature_subtag_coe)) {
            if (BCM_GPORT_IS_SET(port_array[i]) &&
                _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit,
                    port_array[i])) {
                if (_bcm_kt2_mod_is_coe_mod_check(unit, mod_out) == BCM_E_NONE) {
                    pp_port = BCM_GPORT_SUBPORT_PORT_GET(port_array[i]);
                    if (_bcm_kt2_is_pp_port_linkphy_subtag(unit, pp_port)) {
                        is_local_subport = TRUE;
                        local_port_array[i] = pp_port;
                    }
                }
                if (is_local_subport == FALSE) {
                    BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(BCM_E_PORT);
                }
            }
        }
#endif
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
        if (is_local_subport) {
            /* Local subport assigned above */
        } else
#endif
        if (BCM_TRUNK_INVALID != trunk_id) {
            BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(BCM_E_PARAM);
        } else {
            rv = _bcm_esw_modid_is_local(unit, mod_out, &modid_local);
            if (BCM_FAILURE(rv)) {
                BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(rv);
            }

            if ((0 == IS_E_PORT(unit, port_out)) &&
                !(_BCM_MULTICAST_IS_L2(group) ||
                  (BCM_IF_INVALID == encap_id_array[i]))) {
                /* Stack ports should not perform interface replication */
                BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(BCM_E_PARAM);
            }
            if (TRUE != modid_local) {
                BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(BCM_E_PARAM);
            }
            local_port_array[i] = port_out;
        }
    }

    BCM_PBMP_CLEAR(l2_pbmp);
    if (_BCM_MULTICAST_L2_OR_FABRIC(unit, group)) {
        for (i = 0; i < port_count; i++) {
            BCM_PBMP_PORT_ADD(l2_pbmp, local_port_array[i]);
        }

        /* Update the L2MC port bitmap */
        soc_mem_lock(unit, L2MCm);
        rv = soc_mem_read(unit, L2MCm, MEM_BLOCK_ANY, mc_index, &l2mc_entry);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, L2MCm);
            BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(rv);
        }
        soc_mem_pbmp_field_set(unit, L2MCm, &l2mc_entry, PORT_BITMAPf,
                               &l2_pbmp);
        rv = soc_mem_write(unit, L2MCm, MEM_BLOCK_ALL,
                           mc_index, &l2mc_entry);
        soc_mem_unlock(unit, L2MCm);
#if defined(INCLUDE_L3)
    } else {
        BCM_PBMP_CLEAR(e_pbmp);
        BCM_PBMP_ASSIGN(e_pbmp, PBMP_E_ALL(unit));
        /* 
         * For each port, walk through the list of GPORTs
         * and collect the ones that match the port.
         */
        /* Valid SOC ports plus the CMIC port */
        BCM_PBMP_ASSIGN(mc_eligible_pbmp, PBMP_PORT_ALL(unit));
        BCM_PBMP_PORT_ADD(mc_eligible_pbmp, CMIC_PORT(unit));
#if defined(BCM_KATANA2_SUPPORT)
        if (soc_feature(unit, soc_feature_subtag_coe) ||
            soc_feature(unit, soc_feature_linkphy_coe)) {
            _bcm_kt2_subport_pbmp_update(unit, &mc_eligible_pbmp);
            _bcm_kt2_subport_pbmp_update(unit, &e_pbmp);
        }
#endif

        BCM_PBMP_ITER(mc_eligible_pbmp, port_iter) {
            list_count = 0;
            for (i = 0; i < port_count ; i++) {
                   if (local_port_array[i] != port_iter) {
                     continue;
                   } 
                   if (encap_id_array[i] == BCM_IF_INVALID) {
                       /* Add port to L2 pbmp */
                       BCM_PBMP_PORT_ADD(l2_pbmp, port_iter);
                   } else if (0 == BCM_PBMP_MEMBER(e_pbmp, port_iter)) {
                       /* Must be a front-panel port to have an
                        * interface replication list. */
                       BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(BCM_E_PARAM);
                   } else {
                        if (!list_count) {
                            q_id = subscriber_queue_array[i];
                        } else {
                            if (q_id != subscriber_queue_array[i]) {
                                BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(BCM_E_PARAM);
                            }
                        }
                        temp_encap_list[list_count] = encap_id_array[i];
                        list_count++;
                    }
                }
                
                if (list_count) {
                    rv = _bcm_esw_ipmc_mmu_mc_remap_ptr(unit, mc_index, &remap_index, FALSE);
                    if (BCM_FAILURE(rv)) {
                        BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(rv);
                    }

                    q_count_increased = 0;
                    if (!SOC_PORT_VALID(unit, port_iter)) {
                        rv = BCM_E_PORT;
                        BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(rv);
                    }
                    rv = _bcm_esw_ipmc_subscriber_egress_intf_set(unit,
                             remap_index, port_iter, list_count,
                             temp_encap_list,q_id, TRUE, &q_count,
                             &q_count_increased);
                    if (BCM_FAILURE(rv)) {
                        BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(rv);
                    } else {
                        if (q_count_increased) {
#if defined(BCM_SABER2_SUPPORT) 
                            if (SOC_IS_SABER2(unit)) {
                                /* coverity[overrun-call : FALSE] */
                                _bcm_sb2_ipmc_port_ext_queue_count_increment(unit,
                                                        mc_index, port_iter);
                            } else 
#endif 
                            {
                                /* coverity[overrun-call : FALSE] */
                                _bcm_kt_ipmc_port_ext_queue_count_increment(
                                     unit, mc_index, port_iter);
                            }
                        }
                        /* coverity[overrun-call : FALSE] */
                        rv = _bcm_esw_ipmc_set_remap_group(unit, mc_index, 
                                                         port_iter, q_count); 
                        if (BCM_FAILURE(rv)) {
                            BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(rv);
                        } 
                    }
                }
            }
#endif  /* INCLUDE_L3 */
    }

    BCM_SUBSCRIBER_MULTICAST_MEM_FREE_RETURN(rv);
}

/*
 * Function:
 *      _bcm_esw_multicast_egress_subscriber_get
 * Purpose:
 *      Get the  replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port_max - (IN) Maximum ports
 *      port_array      - (OUT) GPORT array
 *      encap_array  - (OUT) Encap ID array.
 *      subscriber_array - (OUT) Subscriber array
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_multicast_egress_subscriber_get(int unit, bcm_multicast_t group,
                                         int port_max, bcm_gport_t *port_array,
                                         bcm_if_t *encap_id_array,
                                         bcm_gport_t *queue_array,
                                         int *port_count)
{
    int             mc_index, i, rv;
    int             count = 0;
    bcm_port_t      port;
    int             remap_index;
    int             uc_queue ; 
    int             temp_queue = 0;
    i= 0;
    rv = BCM_E_NONE;

    mc_index = _BCM_MULTICAST_ID_GET(group);
    if ((mc_index < 0) ||
        (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

#if defined (INCLUDE_L3)
    BCM_IF_ERROR_RETURN(_bcm_esw_multicast_l3_group_check(unit, group, NULL));
#endif /* INCLUDE_L3 */ 
    if (port_max <= 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
    (_bcm_esw_ipmc_mmu_mc_remap_ptr(unit, mc_index, &remap_index, FALSE));
    BCM_IF_ERROR_RETURN
    (_bcm_esw_ipmc_subscriber_egress_intf_get(unit, remap_index, port_max, 
                                    port_array, encap_id_array, 
                                    queue_array, &count)); 

    *port_count = count;    
    for (i = 0; i < count; i++) {
        uc_queue = 0; 
        if (NULL != port_array && NULL != queue_array) {
           /* Convert to GPORT values */
#ifdef BCM_KATANA2_SUPPORT
             if (SOC_IS_KATANA2(unit)) {
                 BCM_IF_ERROR_RETURN
                     (bcm_kt2_cosq_port_get(unit, queue_array[i], &port));
             } else 
#endif /* BCM_KATANA2_SUPPORT */
             {
                 BCM_IF_ERROR_RETURN
                     (bcm_kt_cosq_port_get(unit, queue_array[i], &port));
             }
#ifdef BCM_KATANA2_SUPPORT
             if (SOC_IS_KATANA2(unit)) {
                 temp_queue = queue_array[i];
                 rv = bcm_kt2_sw_hw_queue(unit, &queue_array[i]);
                 if (BCM_FAILURE(rv)) {
                     queue_array[i] = temp_queue;
                     BCM_IF_ERROR_RETURN(_bcm_kt_hw_to_sw_queue(unit,
                                                                &queue_array[i],
                                                                &port));
                     uc_queue = 0;
                 } else {
                     bcm_kt2_is_uc_queue(unit, queue_array[i], &uc_queue);
                 }
                 if(uc_queue){
                     BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(queue_array[i],port,
                                                            queue_array[i]); 
                 }
                 else 
                 {
                     if (BCM_SUCCESS(rv)) {
                         bcm_kt2_cosq_subscriber_qid_set(unit, &queue_array[i],
                                                         queue_array[i]);
                     } else {
                         rv = _bcm_kt2_subscriber_sw_qid_set(unit,
                                                             &queue_array[i],
                                                             queue_array[i]);
                     }
                 }
             } else 
#endif /* BCM_KATANA2_SUPPORT */
             {
                 temp_queue = queue_array[i];
                 rv = bcm_kt_sw_hw_queue(unit, &queue_array[i]);
                 if (BCM_FAILURE(rv)) {
                     queue_array[i] = temp_queue;
                     rv = _bcm_kt_hw_to_sw_queue(unit, &queue_array[i],
                                                 &port);
                     uc_queue = 0;
                 } else {
                     bcm_kt_is_uc_queue(unit, queue_array[i], &uc_queue);
                 }
                 if (uc_queue) 
                 {
                     BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET
                            (queue_array[i],port,queue_array[i]);
                 }
                 else  
                 {
                     bcm_kt_cosq_subscriber_qid_set(unit, &queue_array[i],
                         queue_array[i]);
                 } 
             }
             if(!uc_queue){ 
                queue_array[i] |= (port << 16);
             }
#if defined(BCM_KATANA2_SUPPORT)
             /*
              * Encode with subport type if this is a subport
              */
             if (_bcm_kt2_is_pp_port_linkphy_subtag(unit, port)) {
                 _BCM_KT2_SUBPORT_PORT_ID_SET(port_array[i], port);
                 if (BCM_PBMP_MEMBER(SOC_INFO(unit).linkphy_pp_port_pbm,
                             port)) {
                     _BCM_KT2_SUBPORT_PORT_TYPE_SET(port_array[i],
                             _BCM_KT2_SUBPORT_TYPE_LINKPHY);
                 } else if (BCM_PBMP_MEMBER(SOC_INFO(unit).subtag_pp_port_pbm,
                             port)) {
                     _BCM_KT2_SUBPORT_PORT_TYPE_SET(port_array[i],
                             _BCM_KT2_SUBPORT_TYPE_SUBTAG);
                 } else {
                     return BCM_E_PORT;
                 }
             } else
#endif
             {
                 BCM_IF_ERROR_RETURN
                     (bcm_esw_port_gport_get(unit, port, (port_array + i)));
             }
        }
        if (i == port_max) {
            *port_count = port_max;
            break;
        }
    }

    return rv;    
}

/*
 * Function:
 *   _bcm_esw_multicast_egress_subscriber_qos_map_set
 * Purpose:
 *   Assign the complete set of egress GPORTs in the
 *   replication list for the specified multicast index.
 * Parameters:
 *   unit              - (IN) Device Number
 *   group             - (IN) Multicast group ID
 *   port              - (IN) GPORT Identifier
 *   subscriber queue  - (IN) Subscriber queue group GPORT Identifiers
 *   qos_map_id        - (IN) Qos Map ID
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_esw_multicast_egress_subscriber_qos_map_set(int unit,
                                        bcm_multicast_t group,
                                        bcm_gport_t port,
                                        bcm_gport_t subscriber_queue,
                                        int qos_map_id)
{
    int             mc_index;
    int             remap_index;
    int             rv = BCM_E_NONE;

    mc_index = _BCM_MULTICAST_ID_GET(group);
    if ((mc_index < 0) ||
        (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

#if defined (INCLUDE_L3)
    BCM_IF_ERROR_RETURN(_bcm_esw_multicast_l3_group_check(unit, group, NULL));
#endif /* INCLUDE_L3 */
    BCM_IF_ERROR_RETURN
    (_bcm_esw_ipmc_mmu_mc_remap_ptr(unit, mc_index, &remap_index, FALSE));
#if defined(BCM_KATANA2_SUPPORT) && defined(INCLUDE_L3)
#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
        rv = _bcm_sb2_ipmc_subscriber_egress_intf_qos_map_set(unit,
                                            remap_index,
					    port, subscriber_queue,
					    qos_map_id);
    } else
#endif
    if (SOC_IS_KATANA2(unit)) {
        rv = _bcm_kt2_ipmc_subscriber_egress_intf_qos_map_set(unit,
                                            remap_index,
					    port, subscriber_queue,
					    qos_map_id);
    }
#endif
    return rv;
}

/*
 * Function:
 *   _bcm_esw_multicast_egress_subscriber_qos_map_get
 * Purpose:
 *   Get QOS Map ID associated with
 *   multicast subscriber queue replication list.
 * Parameters:
 *   unit              - (IN) Device Number
 *   group             - (IN) Multicast group ID
 *   port              - (IN) GPORT Identifier
 *   subscriber queue  - (IN) Subscriber queue group GPORT Identifiers
 *   qos_map_id        - (OUT) Qos Map ID
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_esw_multicast_egress_subscriber_qos_map_get(int unit,
                                        bcm_multicast_t group,
                                        bcm_gport_t port,
                                        bcm_gport_t subscriber_queue,
                                        int *qos_map_id)
{
    int             mc_index, rv;
    int             remap_index;

    rv = BCM_E_NONE;

    mc_index = _BCM_MULTICAST_ID_GET(group);
    if ((mc_index < 0) ||
        (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

#if defined (INCLUDE_L3)
    BCM_IF_ERROR_RETURN(_bcm_esw_multicast_l3_group_check(unit, group, NULL));
#endif /* INCLUDE_L3 */
    BCM_IF_ERROR_RETURN
    (_bcm_esw_ipmc_mmu_mc_remap_ptr(unit, mc_index, &remap_index, FALSE));
#if defined (BCM_KATANA2_SUPPORT) && defined(INCLUDE_L3)
#if defined (BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        rv =_bcm_sb2_ipmc_subscriber_egress_intf_qos_map_get(unit, remap_index,
                                    port, subscriber_queue,
                                    qos_map_id);
    } else
#endif
    if (SOC_IS_KATANA2(unit)) {
        rv =_bcm_kt2_ipmc_subscriber_egress_intf_qos_map_get(unit, remap_index,
                                    port, subscriber_queue,
                                    qos_map_id);
    }
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_esw_multicast_egress_subscriber_add
 * Purpose:
 *      Add a GPORT to the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 *      subscriber_queue - (IN) Subscriber gport
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_multicast_egress_subscriber_add(int unit, bcm_multicast_t group,
                                         bcm_gport_t port,
                                         bcm_if_t encap_id,
                                         bcm_gport_t subscriber_queue)
{
    int mc_index, is_l3, gport_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int modid_local, rv = BCM_E_NONE; 
    bcm_pbmp_t l2_pbmp, l3_pbmp, old_pbmp, *new_pbmp;
    int remap_index;
    int q_count = 0;
    int q_count_increased = 0;
    int q_count_decreased = 0;
    int if_max = BCM_MULTICAST_PORT_MAX;
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    int is_local_subport = FALSE;
#endif
#if defined(BCM_ENDURO_SUPPORT)
    uint16              dev_id;
    uint8               rev_id;
#if defined(BCM_KATANA2_SUPPORT)
    int my_modid = 0, pp_port;
#endif

    if (SOC_IS_ENDURO(unit)) {
        soc_cm_get_id(unit, &dev_id, &rev_id);

        if ((dev_id == BCM56230_DEVICE_ID) || (dev_id == BCM56231_DEVICE_ID)){
            /* 
             * Because the size of "L3 Next Hop Table" 
             * is reduced to 2K only on Dagger (Enduro Variant), 
             * the if_max needs to be changed to the size of "L3 Next Hop 
             * Table (2K)" instead of "BCM_MULTICAST_PORT_MAX(4K)".
             */ 
            if_max = soc_mem_index_count(unit, ING_L3_NEXT_HOPm);
        }
    }
#endif /* BCM_ENDURO_SUPPORT */

    is_l3 = 0;      
    modid_local = FALSE;

    mc_index = _BCM_MULTICAST_ID_GET(group);

    if ((mc_index < 0) ||
        (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

    if (!(BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(subscriber_queue)) 
         && !(BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(subscriber_queue))
         && !(BCM_GPORT_IS_UCAST_QUEUE_GROUP(subscriber_queue))) {
        return BCM_E_PARAM;
    }

#if defined (INCLUDE_L3)
    BCM_IF_ERROR_RETURN(_bcm_esw_multicast_l3_group_check(unit, group, &is_l3));
#endif /* INCLUDE_L3 */

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, 
                                &trunk_id, &gport_id));

#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        if(_bcm_xgs5_subport_coe_gport_local(unit, port)) {
            is_local_subport = TRUE;
            BCM_IF_ERROR_RETURN(
                _bcmi_coe_subport_physical_port_get(unit,
                                                    port,
                                                    &port_out));
        } else {
            return BCM_E_PORT;
        }

    } else
#endif

#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
        if (BCM_GPORT_IS_SET(port) &&
            _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
            if (_bcm_kt2_mod_is_coe_mod_check(unit, mod_out) == BCM_E_NONE) {
                pp_port = BCM_GPORT_SUBPORT_PORT_GET(port);
                if (_bcm_kt2_is_pp_port_linkphy_subtag(unit, pp_port)) {
                        is_local_subport = TRUE;
                        port_out = pp_port;
                }
            }
            if (is_local_subport == FALSE) {
                return BCM_E_PORT;
            }
        }
    }
#endif

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    if (is_local_subport) {
        /* Local subport assigned above */
    } else
#endif

    if (BCM_TRUNK_INVALID != trunk_id) {
        return BCM_E_PARAM;
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod_out, &modid_local));

        if ((!IS_E_PORT(unit, port_out)) && (BCM_IF_INVALID != encap_id)) {
            /* Stack ports should not perform interface replication */
            return BCM_E_PORT;
        }
        if (TRUE != modid_local) {
            /* Only add this to replication set if destination is local */
            return BCM_E_PORT;
        }
        /* Convert system port to physical port */
        if (soc_feature(unit, soc_feature_sysport_remap)) {
            BCM_XLATE_SYSPORT_S2P(unit, &port_out);
        }
    }
    if (!SOC_PORT_VALID(unit, port_out)) {
        return BCM_E_PORT;
    }
    if (BCM_IF_INVALID != encap_id) {
        /* Add  the required port to the multicast egress interface */
        BCM_IF_ERROR_RETURN(_bcm_esw_ipmc_mmu_mc_remap_ptr(unit,
            mc_index, &remap_index, FALSE));

        rv = _bcm_esw_ipmc_subscriber_egress_intf_add(unit, remap_index,
                 port_out, encap_id, subscriber_queue, is_l3, &q_count,
                 &q_count_increased);
        if (BCM_SUCCESS(rv)) {
            new_pbmp = &l3_pbmp;
#if defined(INCLUDE_L3)
            if (q_count_increased) {
#if defined(BCM_SABER2_SUPPORT) 
                if (SOC_IS_SABER2(unit)) {
                    /* coverity[overrun-call : FALSE] */
                    _bcm_sb2_ipmc_port_ext_queue_count_increment(unit,
                             mc_index, port_out);
                } else 
#endif 
                {
                    /* coverity[overrun-call : FALSE] */
                    _bcm_kt_ipmc_port_ext_queue_count_increment(unit, mc_index,
                                                                port_out);
                }
            }
#endif
        } else {
            return rv;
        }
    } else {
        if (!_BCM_MULTICAST_IS_VPLS(group)) {
            /* Updating the L2 bitmap */
            new_pbmp = &l2_pbmp;
        } else {
            return BCM_E_PARAM;
        }
    }
    /* coverity[overrun-call : FALSE] */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_ipmc_set_remap_group(unit, mc_index, port_out, q_count));

    rv = _bcm_esw_multicast_ipmc_read(unit, mc_index, &l2_pbmp, &l3_pbmp);

    if (BCM_SUCCESS(rv) && (BCM_IF_INVALID == encap_id)) {
        BCM_PBMP_ASSIGN(old_pbmp, *new_pbmp);
        BCM_PBMP_PORT_ADD(*new_pbmp, port_out);
        
        if (BCM_PBMP_NEQ(old_pbmp, *new_pbmp)) {

            /* Inside ipmc_write update num_copies, int, ext and 
             * redirection pointer need to manage the work around
             */
            rv = _bcm_esw_multicast_ipmc_write(unit, mc_index, l2_pbmp, 
                                               l3_pbmp, TRUE);
        }
    }

    if (BCM_FAILURE(rv)) {
        (void) _bcm_esw_ipmc_subscriber_egress_intf_delete(unit, mc_index, 
                                   port_out, if_max, 
                                   encap_id, is_l3, &q_count,
                                   &q_count_decreased);
    }

    return rv;
}

#if defined (BCM_KATANA_SUPPORT) && defined(INCLUDE_L3)
STATIC int
_bcm_esw_ipmc_subscriber_egress_intf_delete_all(int unit, int remap_index,
                                     bcm_port_t *port_array,
                                     int *encap_array,
                                     bcm_if_t *q_array,
                                     int port_count, int mc_index)
{
    int q_count_decrease = 0;
    int index;
    bcm_port_t temp_port = BCM_GPORT_INVALID;  
    int temp_queue, rv = BCM_E_NONE;

#if defined (BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        BCM_IF_ERROR_RETURN(
            _bcm_sb2_ipmc_subscriber_egress_intf_delete_all(unit, remap_index,
                 port_array, encap_array, q_array, &q_count_decrease));

    } else
#endif /* BCM_SABER2_SUPPORT */
    {
        BCM_IF_ERROR_RETURN(
            _bcm_kt_ipmc_subscriber_egress_intf_delete_all(unit, remap_index,
                 port_count, port_array, encap_array, q_array,
                 &q_count_decrease));
    }

    if (q_count_decrease) {
        for (index = 0; index < port_count; index++) {
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit) || SOC_IS_SABER2(unit)) {
                BCM_IF_ERROR_RETURN(bcm_kt2_cosq_port_get(unit, q_array[index],
                                                          &temp_port));
            } else
#endif /* BCM_KATANA2_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(bcm_kt_cosq_port_get(unit, q_array[index],
                                                         &temp_port));
            }
#if defined (BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit)) {
                if (SOC_PORT_VALID(unit, temp_port)) {
                    /* coverity[overrun-call : FALSE] */
                    _bcm_sb2_ipmc_port_ext_queue_count_decrement(unit, mc_index,
                                                                 temp_port);
                } else {
                    return BCM_E_PORT;
                }
            } else
#endif /* BCM_SABER2_SUPPORT */
            {
                temp_queue = q_array[index];
#ifdef BCM_KATANA2_SUPPORT
                if (SOC_IS_KATANA2(unit)) {
                    rv = bcm_kt2_sw_hw_queue(unit, &q_array[index]);
                } else
#endif /* BCM_KATANA2_SUPPORT */
                {
                    rv = bcm_kt_sw_hw_queue(unit, &q_array[index]);
                }
                if (BCM_FAILURE(rv)) {
                    q_array[index] = temp_queue;
                    BCM_IF_ERROR_RETURN(_bcm_kt_hw_to_sw_queue(unit,
                                                               &q_array[index],
                                                               &temp_port));
                }
                if (SOC_PORT_VALID(unit, temp_port)) {
                    /* coverity[overrun-call : FALSE] */
                    _bcm_kt_ipmc_port_ext_queue_count_decrement(unit, mc_index,
                                                                temp_port);
                } else {
                    return BCM_E_PORT;
                }
            }
        }
        if (temp_port != BCM_GPORT_INVALID) {
            /* coverity[overrun-call : FALSE] */
            BCM_IF_ERROR_RETURN(_bcm_esw_ipmc_set_remap_group(unit, mc_index,
                        temp_port, 0));
        }
    }
    return BCM_E_NONE;
}


STATIC int
_bcm_esw_multicast_egress_subscriber_delete_all(int unit, 
                                bcm_multicast_t group)
{
    int mc_index;
    bcm_gport_t *port_array;
    bcm_if_t *encap_array;
    int *q_array;
    int if_max = BCM_MULTICAST_PORT_MAX;
    int remap_index;
    bcm_error_t rv = BCM_E_NONE;
    int port_count = 0;
    /*get multicast index for multicast group */
    mc_index = _BCM_MULTICAST_ID_GET(group);
    if ((mc_index < 0) ||
            (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
        return BCM_E_UNAVAIL;
    }

    /*get the port_array, encap array and q_array */
    port_array = sal_alloc(if_max * sizeof(bcm_gport_t),
            "mcast port array");
    if (port_array == NULL) {
        return BCM_E_MEMORY;
    }
    encap_array = sal_alloc(if_max * sizeof(bcm_if_t),
            "mcast encap array");
    if (encap_array == NULL) {
        sal_free (port_array);
        return BCM_E_MEMORY;
    }
    q_array = sal_alloc(if_max * sizeof(bcm_if_t),
            "mcast q array");
    if (q_array == NULL) {
        sal_free (port_array);
        sal_free (encap_array);
        return BCM_E_MEMORY;
    }
    sal_memset(q_array, 0, (if_max * sizeof(bcm_if_t)));

    /*get the encap array and port array associated with the group*/
    rv = _bcm_esw_ipmc_mmu_mc_remap_ptr(unit, mc_index, &remap_index, FALSE);
    if (BCM_FAILURE(rv)) {
        goto failure;
    }
    
    rv = (_bcm_esw_ipmc_subscriber_egress_intf_get(unit, remap_index, (if_max -1),
                                    port_array, encap_array,
                                    q_array, &port_count));

    if (BCM_FAILURE(rv)) {
        goto failure;
    }

    if (port_count) {
        rv = _bcm_esw_ipmc_subscriber_egress_intf_delete_all (unit, 
                              remap_index,port_array, 
                              encap_array, q_array, port_count, mc_index);                                               
    } 
failure:
    sal_free (port_array);
    sal_free (encap_array);
    sal_free (q_array);
    return rv;
}
#endif


/*
 * Function:
 *      _bcm_esw_multicast_egress_subscriber_delete
 * Purpose:
 *      Delete a GPORT from the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 *      subscriber_queue - (IN) Subscriber gport
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_multicast_egress_subscriber_delete(int unit, bcm_multicast_t group,
                                            bcm_gport_t port,
                                            bcm_if_t encap_id,
                                            bcm_gport_t subscriber_queue) 
{
    int mc_index, is_l3, gport_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int modid_local, rv = BCM_E_NONE;
    bcm_pbmp_t l2_pbmp, l3_pbmp, old_pbmp;
    int remap_index;
    int q_count = 0;
    int q_count_decreased = 0;
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    int is_local_subport = FALSE;
#endif
#if defined(BCM_KATANA2_SUPPORT)
    int pp_port;
#endif

    is_l3 = 0;      
    modid_local = FALSE;

    mc_index = _BCM_MULTICAST_ID_GET(group);

    if ((mc_index < 0) ||
        (mc_index >= soc_mem_index_count(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

#if defined (INCLUDE_L3)
    BCM_IF_ERROR_RETURN(_bcm_esw_multicast_l3_group_check(unit, group, &is_l3));
#endif /* INCLUDE_L3 */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, 
                                &trunk_id, &gport_id)); 


#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        if(_bcm_xgs5_subport_coe_gport_local(unit, port)) {
            is_local_subport = TRUE;
            BCM_IF_ERROR_RETURN(
                _bcmi_coe_subport_physical_port_get(unit,
                                                    port,
                                                    &port_out));
        } else {
            return BCM_E_PORT;
        }

    } else
#endif

#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        if (BCM_GPORT_IS_SET(port) &&
            _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
            if (_bcm_kt2_mod_is_coe_mod_check(unit, mod_out) == BCM_E_NONE) {
                pp_port = BCM_GPORT_SUBPORT_PORT_GET(port);
                if (_bcm_kt2_is_pp_port_linkphy_subtag(unit, pp_port)) {
                    is_local_subport = TRUE;
                    port_out = pp_port;
                }
            }
            if (is_local_subport == FALSE) {
                return BCM_E_PORT;
            }
        }
    }
#endif

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    if (is_local_subport) {
        /* Local subport assigned above */
    } else
#endif

    if (BCM_TRUNK_INVALID != trunk_id) {
        return BCM_E_PARAM;
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod_out, &modid_local));

        if ((!IS_E_PORT(unit, port_out)) && (BCM_IF_INVALID != encap_id)) {
            /* Stack ports should not perform interface replication */
            return BCM_E_PORT;
        }
        if (TRUE != modid_local) {
            /* Only add this to replication set if destination is local */
            return BCM_E_PORT;
        }
        /* Convert system port to physical port */
        if (soc_feature(unit, soc_feature_sysport_remap)) {
            BCM_XLATE_SYSPORT_S2P(unit, &port_out);
        }
    }

    if (encap_id == BCM_IF_INVALID) {
        /* Remove L2 port */
        if (_BCM_MULTICAST_IS_VPLS(group)) {
            return BCM_E_PARAM;
        }

        soc_mem_lock(unit, L3_IPMCm);
        /* Delete the port from the IPMC L2_BITMAP */
        rv = _bcm_esw_multicast_ipmc_read(unit, mc_index, 
                                          &l2_pbmp, &l3_pbmp);
        if (BCM_SUCCESS(rv)) {
            BCM_PBMP_ASSIGN(old_pbmp, l2_pbmp);

            BCM_PBMP_PORT_REMOVE(l2_pbmp, port_out);

            if (BCM_PBMP_NEQ(old_pbmp, l2_pbmp)) {
                rv = _bcm_esw_multicast_ipmc_write(unit, mc_index,
                                        l2_pbmp, l3_pbmp, TRUE);
            } else {
                rv = BCM_E_NOT_FOUND;
            }
        }
        soc_mem_unlock(unit, L3_IPMCm);
        return rv;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_ipmc_mmu_mc_remap_ptr(unit, mc_index, &remap_index, FALSE));
    if (!SOC_PORT_VALID(unit, port_out)) {
        return BCM_E_PORT;
    }
    rv = _bcm_esw_ipmc_subscriber_egress_intf_delete(unit,
             remap_index, port_out, encap_id, subscriber_queue,
             is_l3, &q_count, &q_count_decreased);
    if (BCM_SUCCESS(rv)) {
#if defined(INCLUDE_L3)
        if (q_count_decreased) {
#if defined(BCM_SABER2_SUPPORT) 
             if (SOC_IS_SABER2(unit)) {
                 /* coverity[overrun-call : FALSE] */
                 _bcm_sb2_ipmc_port_ext_queue_count_decrement(unit,
                             mc_index, port_out);
             } else 
#endif 
             {
                 /* coverity[overrun-call : FALSE] */
                 _bcm_kt_ipmc_port_ext_queue_count_decrement(unit, mc_index,
                                                             port_out);
             }
        }
#endif
        /* coverity[overrun-call : FALSE] */
        BCM_IF_ERROR_RETURN(_bcm_esw_ipmc_set_remap_group(unit, mc_index, 
                                                       port_out, q_count)); 
    }
   
    return rv;
}

#endif /* BCM_KATANA_SUPPORT */

/*
 * Function:
 *      bcm_esw_multicast_egress_subscriber_add
 * Purpose:
 *      Add a GPORT to the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 *      subscriber_queue - (IN) Subscriber gport
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_multicast_egress_subscriber_add(int unit, bcm_multicast_t group,
                                        bcm_gport_t port,
                                        bcm_if_t encap_id,
                                        bcm_gport_t subscriber_queue)
{
    MULTICAST_INIT_CHECK(unit);

#if defined(BCM_KATANA_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_KATANAX(unit)) {    
       return _bcm_esw_multicast_egress_subscriber_add(unit, group, port, 
                                                       encap_id, subscriber_queue);
    }
#endif /* BCM_KATANA_SUPPORT && INCLUDE_L3 */
        return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_multicast_egress_subscriber_delete
 * Purpose:
 *      Delete a GPORT from the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 *      subscriber_queue - (IN) Subscriber gport
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_multicast_egress_subscriber_delete(int unit, bcm_multicast_t group,
                                           bcm_gport_t port,
                                           bcm_if_t encap_id,
                                           bcm_gport_t subscriber_queue) 
{
    MULTICAST_INIT_CHECK(unit);

#if defined(BCM_KATANA_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_KATANAX(unit)) {    
        return _bcm_esw_multicast_egress_subscriber_delete(unit, group, port, 
                                                    encap_id, subscriber_queue);
    }
#endif /* BCM_KATANA_SUPPORT && INCLUDE_L3 */
            return BCM_E_UNAVAIL;
}

#if defined(BCM_KATANA_SUPPORT) && defined(INCLUDE_L3)

int
_bcm_esw_multicast_egress_subscriber_set_update(int unit, bcm_multicast_t group,
    int port_count, bcm_gport_t *port_array, bcm_if_t *encap_id_array,
    bcm_gport_t *subscriber_queue_array)

{
    bcm_gport_t *cur_port_array, *new_port_array;
    bcm_if_t *cur_encap_array, *new_encap_array;
    bcm_gport_t *cur_q_array, *new_q_array;
    int if_max = BCM_MULTICAST_PORT_MAX;
    bcm_error_t rv = BCM_E_NONE;
    int if_index, if_cur_index, cur_port_count = 0, new_port_count = 0;

    /*Allocate memory for current and new port_array, encap array and q_array */
    cur_port_array = sal_alloc(if_max * sizeof(bcm_gport_t),
                               "current mcast port array");
    if (cur_port_array == NULL) {
        return BCM_E_MEMORY;
    }
    cur_encap_array = sal_alloc(if_max * sizeof(bcm_if_t),
                                "current mcast encap array");
    if (cur_encap_array == NULL) {
        sal_free (cur_port_array);
        return BCM_E_MEMORY;
    }
    cur_q_array = sal_alloc(if_max * sizeof(bcm_gport_t),
                            "current mcast q array");
    if (cur_q_array == NULL) {
        sal_free (cur_port_array);
        sal_free (cur_encap_array);
        return BCM_E_MEMORY;
    }

    sal_memset(cur_port_array, 0, (if_max * sizeof(bcm_gport_t)));
    sal_memset(cur_encap_array, 0, (if_max * sizeof(bcm_if_t)));
    sal_memset(cur_q_array, 0, (if_max * sizeof(bcm_gport_t)));

    new_port_array = sal_alloc(port_count * sizeof(bcm_gport_t),
                               "new mcast port array");
    if (new_port_array == NULL) {
        sal_free (cur_port_array);
        sal_free (cur_encap_array);
        sal_free (cur_q_array);
        return BCM_E_MEMORY;
    }
    new_encap_array = sal_alloc(port_count * sizeof(bcm_if_t),
                                "new mcast encap array");
    if (new_encap_array == NULL) {
        sal_free (cur_port_array);
        sal_free (cur_encap_array);
        sal_free (cur_q_array);
        sal_free (new_port_array);
        return BCM_E_MEMORY;
    }
    new_q_array = sal_alloc(port_count * sizeof(bcm_gport_t),
                            "new mcast q array");
    if (new_q_array == NULL) {
        sal_free (cur_port_array);
        sal_free (cur_encap_array);
        sal_free (cur_q_array);
        sal_free (new_port_array);
        sal_free (new_encap_array);
        return BCM_E_MEMORY;
    }

    sal_memset(new_port_array, 0, (port_count * sizeof(bcm_gport_t)));
    sal_memset(new_encap_array, 0, (port_count * sizeof(bcm_if_t)));
    sal_memset(new_q_array, 0, (port_count * sizeof(bcm_gport_t)));

    /* get the port array, encap array, queue array associated with the group */
    rv = _bcm_esw_multicast_egress_subscriber_get(unit, group, (if_max -1),
                                                  cur_port_array,
                                                  cur_encap_array, cur_q_array,
                                                  &cur_port_count);
    if (BCM_FAILURE(rv)) {
        goto done;
    }

    for (if_index = 0; if_index < port_count; if_index++) {
        for (if_cur_index=0; if_cur_index < cur_port_count; if_cur_index++) {
            if ((encap_id_array[if_index] == cur_encap_array[if_cur_index]) &&
                (subscriber_queue_array[if_index] ==
                 cur_q_array[if_cur_index])) {
                /* This replication is already attached to the queue and should
                 * not be deleted.
                 */
                cur_encap_array[if_cur_index] = -1;
                break;
            }
        }

        if (if_cur_index == cur_port_count) {
            /* This is the new replication yet to be attached to the queue */
            new_port_array[new_port_count] = port_array[if_index];
            new_encap_array[new_port_count] = encap_id_array[if_index];
            new_q_array[new_port_count] = subscriber_queue_array[if_index];
            new_port_count++;
        }
    }

    if (new_port_count != 0) {
        rv = _bcm_esw_multicast_egress_subscriber_set(unit, group,
                 new_port_count, new_port_array, new_encap_array,
                 new_q_array);
        if (BCM_FAILURE(rv)) {
            goto done;
        }
    }

    for (if_cur_index=0; if_cur_index < cur_port_count; if_cur_index++) {
        if (cur_encap_array[if_cur_index] == -1) {
            /* Skip deleting the replication entry already attached */
            continue;
        } else {
            /* Delete the replication which is not present in the input list */
            rv = _bcm_esw_multicast_egress_subscriber_delete(unit, group,
                     cur_port_array[if_cur_index],
                     cur_encap_array[if_cur_index], cur_q_array[if_cur_index]);
            if (BCM_FAILURE(rv)) {
                goto done;
            }
        }
    }
done:
    if (cur_port_array) {
        sal_free (cur_port_array);
    }
    if (cur_encap_array) {
        sal_free (cur_encap_array);
    }
    if (cur_q_array) {
        sal_free (cur_q_array);
    }
    if (new_port_array) {
        sal_free (new_port_array);
    }
    if (new_encap_array) {
        sal_free (new_encap_array);
    }
    if (new_q_array) {
        sal_free (new_q_array);
    }
    return rv;
}

#endif

/*  
 * Function:
 *      bcm_esw_multicast_egress_subscriber_set
 * Purpose:
 *      Assign the complete set of egress GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit       - (IN) Device Number
 *      group      - (IN) Multicast group ID
 *      port_count - (IN) Number of ports in replication list
 *      port_array - (IN) List of GPORT Identifiers
 *      encap_id_array - (IN) List of encap identifiers
 *      subscriber_queue_array - (IN) Subscriber array
 * Returns:
 *      BCM_E_XXX
 */     
int
bcm_esw_multicast_egress_subscriber_set(int unit, bcm_multicast_t group,
                                        int port_count, bcm_gport_t *port_array,
                                        bcm_if_t *encap_id_array,
                                        bcm_gport_t *subscriber_queue_array) 
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    MULTICAST_INIT_CHECK(unit);
#if defined(BCM_KATANA_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_KATANAX(unit)) {
        if (port_count == 0) {
            BCM_IF_ERROR_RETURN(_bcm_esw_multicast_egress_subscriber_delete_all
                    (unit, group));

            /* If port_count is 0, then there is nothing to set and we can
             * return BCM_E_NONE since all replications were already deleted
             * using _bcm_esw_multicast_egress_subscriber_delete_all.
             */
            return BCM_E_NONE;
        }
        rv = _bcm_esw_multicast_egress_subscriber_set_update(unit, group,
                 port_count, port_array, encap_id_array,
                 subscriber_queue_array);
    }
#endif /* BCM_KATANA_SUPPORT && INCLUDE_L3 */
    return rv;
}

/*  
 * Function:
 *      bcm_esw_multicast_egress_subscriber_get
 * Purpose:
 *      Get the complete set of egress GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit       - (IN) Device Number
 *      group      - (IN) Multicast group ID
 *      port_count - (IN) Number of ports in replication list
 *      port_array - (OUT) List of GPORT Identifiers
 *      encap_id_array - (OUT) List of encap identifiers
  *      subscriber_queue_array - (OUT) Subscriber array
 * Returns:
 *      BCM_E_XXX
 */ 
int
bcm_esw_multicast_egress_subscriber_get(int unit, bcm_multicast_t group,
                                        int port_max,
                                        bcm_gport_t *port_array,
                                        bcm_if_t *encap_id_array,
                                        bcm_gport_t *subscriber_queue_array,
                                        int *port_count)
{
    MULTICAST_INIT_CHECK(unit);

#if defined(BCM_KATANA_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_KATANAX(unit)) {            
        return _bcm_esw_multicast_egress_subscriber_get(unit, group, 
                                                        port_max, port_array, 
                                                        encap_id_array, 
                                                        subscriber_queue_array,
                                                        port_count);
    }
#endif /* BCM_KATANA_SUPPORT && INCLUDE_L3 */
            return BCM_E_UNAVAIL;  
}

/*
 * Function:
 *      bcm_esw_multicast_egress_subscriber_qos_map_set
 * Purpose:
 *      Assign the complete set of egress GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit              - (IN) Device Number
 *      group             - (IN) Multicast group ID
 *      port              - (IN) GPORT Identifier
 *      subscriber queue  - (IN) Subscriber queue group GPORT Identifiers
 *      qos_map_id        - (IN) Qos Map ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_multicast_egress_subscriber_qos_map_set(int unit,
                                        bcm_multicast_t group,
                                        bcm_gport_t port,
                                        bcm_gport_t subscriber_queue,
                                        int qos_map_id)
{
    MULTICAST_INIT_CHECK(unit);

#if defined(BCM_KATANA2_SUPPORT) && defined(INCLUDE_L3)
        if (SOC_IS_KATANA2(unit)) {
            return _bcm_esw_multicast_egress_subscriber_qos_map_set(unit,
                                                    group, port,
                                                    subscriber_queue,
                                                    qos_map_id);
        }
#endif /* BCM_KATANA_SUPPORT && INCLUDE_L3 */
        return BCM_E_UNAVAIL;

}

/*
 * Function:
 *      bcm_esw_multicast_egress_subscriber_qos_map_get
 * Purpose:
 *      Get QOS Map ID associated with
 *      multicast subscriber queue replication list.
 * Parameters:
 *      unit              - (IN) Device Number
 *      group             - (IN) Multicast group ID
 *      port              - (IN) GPORT Identifier
 *      subscriber queue  - (IN) Subscriber queue group GPORT Identifiers
 *      qos_map_id        - (OUT) Qos Map ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_multicast_egress_subscriber_qos_map_get(int unit,
                                        bcm_multicast_t group,
                                        bcm_gport_t port,
                                        bcm_gport_t subscriber_queue,
                                        int *qos_map_id)
{
    MULTICAST_INIT_CHECK(unit);

#if defined(BCM_KATANA2_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_KATANA2(unit)) {
        return _bcm_esw_multicast_egress_subscriber_qos_map_get(unit, group,
                                                     port,
                                                    subscriber_queue,
                                                    qos_map_id);
    }
#endif /* BCM_KATANA_SUPPORT && INCLUDE_L3 */
            return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_multicast_multi_alloc
 * Purpose:
 *      Allocates multiple multicast groups consecutively.
 * Parameters:
 *      unit           - (IN)   Device Number
 *      mc_multi_info  - (IN)   informtaion to create multicast mc groups.
 *      base_mc_group  - (IN/OUT)  base group id for allocated multiple groups.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_multicast_multi_alloc( int unit,
        bcm_multicast_multi_info_t mc_multi_info,
        bcm_multicast_t *base_mc_group)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
    MULTICAST_INIT_CHECK(unit);
    rv = bcmi_esw_multicast_multi_alloc(unit, mc_multi_info, base_mc_group);
#endif
    return rv;
}

/*
 * Function:
 *      bcm_esw_multicast_multi_free
 * Purpose:
 *      Frees multiple multicast groups.
 * Parameters:
 *      unit           - (IN)   Device Number
 *      base_mc_group  - (IN)  base group id for multiple groups.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_multicast_multi_free(int unit, bcm_multicast_t base_mc_group)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
    MULTICAST_INIT_CHECK(unit);
    rv = bcmi_esw_multicast_multi_free(unit, base_mc_group);
#endif
    return rv;

}



#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *      _bcm_esw_multicast_sw_dump_cb
 * Purpose:
 *      Callback to print Multicast info
 * Parameters:
 *      unit       - (IN)   Device Number
 *      group      - (IN)   Group ID
 *      flags      - (OUT)  BCM_MULTICAST_*
 *      user_data  - (IN)   User data to be passed to callback function.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_multicast_sw_dump_cb(int unit, bcm_multicast_t group, 
                              uint32 flags, void *user_data)
{
    LOG_CLI((BSL_META_U(unit,
                        "    0x%08x    0x%08x     "), group, flags));
    if (flags & BCM_MULTICAST_TYPE_L2) {
        LOG_CLI((BSL_META_U(unit,
                            " L2")));
    }
    if (flags & BCM_MULTICAST_TYPE_L3) {
        LOG_CLI((BSL_META_U(unit,
                            " L3")));
    }
    if (flags & BCM_MULTICAST_TYPE_VPLS) {
        LOG_CLI((BSL_META_U(unit,
                            " VPLS")));
    }
    if (flags & BCM_MULTICAST_TYPE_SUBPORT) {
        LOG_CLI((BSL_META_U(unit,
                            " SUBPORT")));
    }
    if (flags & BCM_MULTICAST_TYPE_MIM) {
        LOG_CLI((BSL_META_U(unit,
                            " MIM")));
    }
    if (flags & BCM_MULTICAST_TYPE_WLAN) {
        LOG_CLI((BSL_META_U(unit,
                            " WLAN")));
    }
    if (flags & BCM_MULTICAST_TYPE_VLAN) {
        LOG_CLI((BSL_META_U(unit,
                            " VLAN")));
    }
    if (flags & BCM_MULTICAST_TYPE_TRILL) {
        LOG_CLI((BSL_META_U(unit,
                            " TRILL")));
    }
    if (flags & BCM_MULTICAST_TYPE_NIV) {
        LOG_CLI((BSL_META_U(unit,
                            " NIV")));
    }
    if (flags &  BCM_MULTICAST_TYPE_EGRESS_OBJECT) {
        LOG_CLI((BSL_META_U(unit,
                            " MPLS P2MP")));
    }
    if (flags &  BCM_MULTICAST_TYPE_L2GRE) {
        LOG_CLI((BSL_META_U(unit,
                            " L2GRE")));
    }
    if (flags &  BCM_MULTICAST_TYPE_VXLAN) {
        LOG_CLI((BSL_META_U(unit,
                            " VXLAN")));
    }
    if (flags & BCM_MULTICAST_TYPE_EXTENDER) {
        LOG_CLI((BSL_META_U(unit,
                            " EXTENDER")));
    }
    if (flags & BCM_MULTICAST_WITH_ID) {
        LOG_CLI((BSL_META_U(unit,
                            " ID")));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_multicast_sw_dump
 * Purpose:
 *     Displays Multicast information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 * Note:
 *    The multicast info is really handled by other modules, but
 *    this is a handy summary of the information.
 *    Dump of l2_data is skipped.
 */
void
_bcm_multicast_sw_dump(int unit)
{
    int rv;

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information Multicast - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "    Initialized: %d\n"), multicast_initialized[unit]));
    LOG_CLI((BSL_META_U(unit,
                        "    Groups:       Flag value:     Flags:\n")));

    rv = bcm_esw_multicast_group_traverse(unit,
                                          &_bcm_esw_multicast_sw_dump_cb,
                                          BCM_MULTICAST_TYPE_MASK, NULL);

    if (BCM_FAILURE(rv)) {
        LOG_CLI((BSL_META_U(unit,
                            "\n  *** Multicast traverse error ***: %s\n"),
                 bcm_errmsg(rv)));
    }

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
#if defined(INCLUDE_L3)
    if (SOC_REG_IS_VALID(unit, IPMC_L3_MTUr) && (NULL != _bcm_mtu_profile)) {
        int i, num_entries, ref_count;
        uint64 rval64, *rval64s[1];

        rval64s[0] = &rval64;
        num_entries = SOC_REG_NUMELS(unit, IPMC_L3_MTUr);
        LOG_CLI((BSL_META_U(unit,
                            "  IPMC_L3_MTU\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "    Index RefCount -  IPMC_L3_MTU\n")));
        
        for (i = 0; i < num_entries; i++) {
            rv = soc_profile_reg_ref_count_get(unit,
                                               _bcm_mtu_profile[unit],
                                               i, &ref_count);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    " *** Error retrieving profile reference: %d ***\n"),
                         rv));
                break;
            }
            if (ref_count <= 0) {
                continue;
            }
            rv = soc_profile_reg_get(unit, _bcm_mtu_profile[unit], i,
                                     1, rval64s);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    " *** Error retrieving profile value: %d ***\n"),
                         rv));
                break;
            }
            LOG_CLI((BSL_META_U(unit,
                                "  %5d %8d       0x%08x\n"),
                     i, ref_count, COMPILER_64_LO(rval64)));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }
#endif /* INCLUDE_L3 */
#endif

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3) 
    if (soc_feature(unit, soc_feature_virtual_port_routing)) {
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
        bcm_td2_multicast_l3_vp_sw_dump(unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
    }
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    return;
}
int
_bcm_fabric_multicast_ipmc_group_types_dump(int unit)
{
#ifdef BCM_XGS3_FABRIC_SUPPORT
    int search_index, index_min, index_max,ipmc_id;
    if (SOC_IS_XGS3_FABRIC(unit)) {

        index_min = BCM_FABRIC_IPMC_BASE(unit);
        index_max = BCM_FABRIC_IPMC_MAX(unit);

        for (search_index = index_min; search_index <= index_max;
                search_index++) {

            ipmc_id = search_index - index_min;

            /* If that mc index exists */
            if (SHR_BITGET(BCM_FABRIC_MC_USED_BITMAP(unit),
                        search_index)) {
                LOG_CLI((BSL_META_U(unit, "Unit %d IPMC ID %d Group type %d\r\n"),
                        unit, ipmc_id,
                     _fabric_multicast_ipmc_group_types[unit][ipmc_id]));
            }
        }
    }else {
        return BCM_E_PARAM;
    }
#endif
    return BCM_E_NONE;
}

#endif /* BCM_SW_STATE_DUMP_DISABLE */

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * VXLAN API
 */

#include <shared/bsl.h>
#include <soc/defs.h>
#include <sal/core/libc.h>
#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/util.h>
#include <soc/hash.h>
#include <soc/debug.h>
#include <soc/greyhound2.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/hurricane3.h>
#include <bcm_int/esw/greyhound2.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/vxlan.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/common/multicast.h>

/*
 * GH2 VXLAN VFI info data structure
 */
typedef struct gh2_vxlan_vpn_info_s {
    int valid;   /* VFI is valid */
    uint32 vnid; /* VNID */
} gh2_vxlan_vpn_info_t;

/* GH2 VXLAN DVP Type definition */
#define GH2_VXLAN_DEST_VP_TYPE_INVALID   0x0
#define GH2_VXLAN_DEST_VP_TYPE_ACCESS    0x1
#define GH2_VXLAN_DEST_VP_TYPE_TUNNEL    0x2

/*
 * GH2 VXLAN VP info data structure
 */
typedef struct gh2_vxlan_vp_info_s {
    uint32 vp_type;           /* VP type (GH2_VXLAN_DEST_VP_TYPE_XXX) */
    int vfi;                  /* VFI domain */
    int ecmp;                 /* ECMP type */
    int nh_ecmp_index;        /* Next Hop or ECMP index */
    int tpid_valid;           /* TPID is valid */
    uint16 tpid_value;        /* The value of outer TPID */
    bcm_if_t ch_encap_id;     /* Custom Header encap ID */
    int ch_match_id_ipmc;     /* Custom Header Match ID for IPMC */
    int ch_match_id_non_ipmc; /* Custom Header Match ID for non-IPMC */
} gh2_vxlan_vp_info_t;

/*
 * GH2 VXLAN Outer TPID info data structure (per system)
 */
typedef struct gh2_vxlan_outer_tpid_info_s {
    uint16 tpid_value; /* The value of outer TPID */
    int ref_count;     /* Reference count */
} gh2_vxlan_outer_tpid_info_t;

/*
 * GH2 VXLAN Ingress VLAN XLATE info data structure
 */
typedef struct gh2_vxlan_vlan_xlate_info_s {
    int ref_count;     /* Reference count */
} gh2_vxlan_vlan_xlate_info_t;

typedef struct bcmi_gh2_vxlan_tunnel_endpoint_s {
     bcm_ip_t      dip;  /* Tunnel DIP */
     bcm_ip_t      sip;  /* Tunnel SIP */
     bcm_ip6_t     dip6; /* Tunnel DIP6 */
     bcm_ip6_t     sip6; /* Tunnel SIP6 */
     uint16        tunnel_state;  /* Multicast Reference Count */
     int           activate_flag; /* Tunnel terminator is deactivated or activated */
     bcm_vlan_t    vlan; /* Out vlan for tunnel termination */
} COMPILER_ATTRIBUTE((packed)) bcmi_gh2_vxlan_tunnel_endpoint_t;

/*
 * Software book keeping for GH2's VXLAN-LITE related information
 */
typedef struct bcmi_gh2_vxlan_bookkeeping_s {
    int initialized;  /* Set to TRUE when VXLAN module initialized */
    sal_mutex_t vxlan_mutex;                 /* Protection mutex. */
    SHR_BITDCL *vxlan_ip_tnl_bitmap;         /* EGR_IP_TUNNEL index bitmap */
    _bcm_vxlan_match_port_info_t *match_key; /* Match Key */
    bcmi_gh2_vxlan_tunnel_endpoint_t *vxlan_tunnel_term;
    bcmi_gh2_vxlan_tunnel_endpoint_t *vxlan_tunnel_init;
    int num_vxlan_tnl;
    SHR_BITDCL *vxlan_vfi_bitmap;            /* VXLAN VFI bitmap */
    gh2_vxlan_vpn_info_t *vxlan_vpn_info;    /* VXLAN per VFI info */
    int num_vxlan_vfi;
    SHR_BITDCL *vxlan_vp_bitmap;             /* VXLAN Virtual Port bitmap */
    SHR_BITDCL *vxlan_network_vp_bitmap;     /* VXLAN Network VP bitmap */
    gh2_vxlan_outer_tpid_info_t *vxlan_outer_tpid; /* VXLAN Outer TPID info */
    int num_vxlan_outer_tpid;                /* Number of VXLAN outer TPIDs */
    gh2_vxlan_vlan_xlate_info_t *vxlan_vlan_xlate; /* VXLAN VLAN XLATE info */
    gh2_vxlan_vp_info_t *vxlan_vp_info;      /* VXLAN per VP info */
    int num_vxlan_vp;
    int riot_enable;                         /* VXLAN RIOT enabled status */
    SHR_BITDCL *vxlan_loopback_vp_bitmap;    /* VXLAN Loopback VP bitmap */
    int riot_custom_header_index0_ref_count; /* Reference count of
                                                Custom Header index0 */
} bcmi_gh2_vxlan_bookkeeping_t;

STATIC bcmi_gh2_vxlan_bookkeeping_t
    *bcmi_gh2_vxlan_bk_info[BCM_MAX_NUM_UNITS];

/* Global initialized parameter for VXLAN bookkeeping info */
STATIC uint8 gh2_vxlan_initialized = FALSE;

#define GH2_VXLAN_INFO(_unit_) (bcmi_gh2_vxlan_bk_info[_unit_])
#define GH2_L3_INFO(_unit_) (&_bcm_l3_bk_info[_unit_])

/* VXLAN-LITE RIOT enable info (only support for GH2-B0) */
#define GH2_VXLAN_RIOT_ENABLE(_unit_) \
    (bcmi_gh2_vxlan_bk_info[_unit_]->riot_enable)

/*
 * VXLAN-LITE VFI usage bitmap operations
 */
#define GH2_VXLAN_VFI_USED_GET(_u_, _vfi_)     \
    ((GH2_VXLAN_INFO(_u_)->vxlan_vfi_bitmap) ? \
    SHR_BITGET(GH2_VXLAN_INFO(_u_)->vxlan_vfi_bitmap, (_vfi_)) : 0)
#define GH2_VXLAN_VFI_USED_SET(_u_, _vfi_)     \
    SHR_BITSET(GH2_VXLAN_INFO((_u_))->vxlan_vfi_bitmap, (_vfi_))
#define GH2_VXLAN_VFI_USED_CLR(_u_, _vfi_)     \
    SHR_BITCLR(GH2_VXLAN_INFO((_u_))->vxlan_vfi_bitmap, (_vfi_))

/*
 * VXLAN-LITE Virtual Port usage bitmap operations
 */
#define GH2_VXLAN_VP_USED_GET(_u_, _vp_)       \
    ((GH2_VXLAN_INFO(_u_)->vxlan_vp_bitmap) ?  \
    SHR_BITGET(GH2_VXLAN_INFO(_u_)->vxlan_vp_bitmap, (_vp_)) : 0)
#define GH2_VXLAN_VP_USED_SET(_u_, _vp_)       \
    SHR_BITSET(GH2_VXLAN_INFO((_u_))->vxlan_vp_bitmap, (_vp_))
#define GH2_VXLAN_VP_USED_CLR(_u_, _vp_)       \
    SHR_BITCLR(GH2_VXLAN_INFO((_u_))->vxlan_vp_bitmap, (_vp_))

/*
 * Network Port bit-map for VXLAN-LITE usage bitmap operations
 */
#define GH2_VXLAN_NETWORK_VP_USED_GET(_u_, _vp_)      \
    ((GH2_VXLAN_INFO(_u_)->vxlan_network_vp_bitmap) ? \
    SHR_BITGET(GH2_VXLAN_INFO(_u_)->vxlan_network_vp_bitmap, (_vp_)) : 0)
#define GH2_VXLAN_NETWORK_VP_USED_SET(_u_, _vp_)      \
    SHR_BITSET(GH2_VXLAN_INFO((_u_))->vxlan_network_vp_bitmap, (_vp_))
#define GH2_VXLAN_NETWORK_VP_USED_CLR(_u_, _vp_)      \
    SHR_BITCLR(GH2_VXLAN_INFO((_u_))->vxlan_network_vp_bitmap, (_vp_))

/*
 * Loopback Port bit-map for VXLAN-LITE RIOT usage bitmap operations
 */
#define GH2_VXLAN_LOOPBACK_VP_USED_GET(_u_, _vp_)      \
    ((GH2_VXLAN_INFO(_u_)->vxlan_loopback_vp_bitmap) ? \
    SHR_BITGET(GH2_VXLAN_INFO(_u_)->vxlan_loopback_vp_bitmap, (_vp_)) : 0)
#define GH2_VXLAN_LOOPBACK_VP_USED_SET(_u_, _vp_)      \
    SHR_BITSET(GH2_VXLAN_INFO((_u_))->vxlan_loopback_vp_bitmap, (_vp_))
#define GH2_VXLAN_LOOPBACK_VP_USED_CLR(_u_, _vp_)      \
    SHR_BITCLR(GH2_VXLAN_INFO((_u_))->vxlan_loopback_vp_bitmap, (_vp_))

/* VP applications */
#define GH2_VXLAN_VP_INFO_NETWORK_PORT  0x00000001 /* Network side port */
#define GH2_VXLAN_VP_INFO_SHARED_PORT   0x00000002 /* Shared virtual port */
#define GH2_VXLAN_VP_INFO_LOOPBACK_PORT 0x00000004 /* Loopback side port */

/* Port Operations */
#define GH2_VXLAN_PORT_OPERATION_NORMAL 0x0 /* Port Operation for normal */
#define GH2_VXLAN_PORT_OPERATION_VFI    0x5 /* Port Operation for VFI */

/*
 * Definitions for VXLAN RIOT port's Custom Header(32 bits)
 *  +-------------------------------+
 *  |      Ethertype (16 bits)      |
 *  +--------------+----------------+
 *  | IPMC Flag    | SGPP (15 bits) |
 *  +--------------+----------------+
 *
 *  Field location :
 *  - SGPP : b0~b14
 *  - IPMC Flag : b15
 *  - Ethertype : b16~b31
 *
 *  Note :
 *  - Macros defined below for the term of "CH" means "Custom Header"
 */
#define GH2_VXLAN_RIOT_CH_HEADER_MASK        0xffffffff
#define GH2_VXLAN_RIOT_CH_ETHERTYPE_MASK     0xffff
#define GH2_VXLAN_RIOT_CH_ETHERTYPE_OFFSET   16
#define GH2_VXLAN_RIOT_CH_IPMC_OFFSET        15
#define GH2_VXLAN_RIOT_CH_MODID_MASK         0xff
#define GH2_VXLAN_RIOT_CH_MODID_OFFSET       7
#define GH2_VXLAN_RIOT_CH_PORT_MASK          0x7f
#define GH2_VXLAN_RIOT_CH_PORT_OFFSET        0

/* SW defined a non-common value for VXLAN RIOT usage */
#define GH2_VXLAN_RIOT_CH_ETHERTYPE          0x21cd

#define GH2_VXLAN_RIOT_CH_ENCAP_VALUE(_ipmc_, _modid_, _port_)             \
    ((GH2_VXLAN_RIOT_CH_ETHERTYPE << GH2_VXLAN_RIOT_CH_ETHERTYPE_OFFSET) | \
    (((_ipmc_) ? 1 : 0) << GH2_VXLAN_RIOT_CH_IPMC_OFFSET) |                \
    (((_modid_) & GH2_VXLAN_RIOT_CH_MODID_MASK) <<                         \
     GH2_VXLAN_RIOT_CH_MODID_OFFSET) |                                     \
    (((_port_) & GH2_VXLAN_RIOT_CH_PORT_MASK) <<                           \
     GH2_VXLAN_RIOT_CH_PORT_OFFSET))

/* QoS mapping Operations for Custom Header usage */
#define GH2_VXLAN_RIOT_CH_COS_REMAP_NONE 0x0         /* Don't use internal
                                                        Priority from CH */
#define GH2_VXLAN_RIOT_CH_COS_REMAP_INT_PRI 0x1      /* Use internal Priority
                                                        from CH */
#define GH2_VXLAN_RIOT_CH_COS_REMAP_INT_PRI_MAP 0x2  /* Use L2 QoS mapping
                                                        pointer from CH */
#define GH2_VXLAN_RIOT_CH_COS_REMAP_DSCP_QOS_MAP 0x3 /* Use L3 QoS mapping
                                                        pointer from CH  */

typedef struct bcmi_vxlan_vp_info_s {
    uint32 flags;
} bcmi_vxlan_vp_info_t;

#define GH2_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(_unit_, _id_)     \
    do {                                                        \
        if (((_id_) < 0) ||                                     \
            (_id_) >= (GH2_VXLAN_INFO(_unit_)->num_vxlan_tnl)) {\
            return BCM_E_BADID;                                 \
        }                                                       \
    } while (0)

typedef struct bcmi_gh2_vxlan_nh_info_s {
    int entry_type;
    int vnid;
    int intf_num;
    int tunnel_index;
    int udp_src_port_range_enable;
    int apply_evxlt_modify_to_payload;
    int use_nhi_in_evxlt_key;
    int payload_otag_delete;
    int tunnel_otag_delete;
    int tunnel_itag_delete;
    int qos_map_id;
    int tunnel_pkt_pri;
    int tunnel_pkt_cfi;
    int custom_header_ipmc;
} bcmi_gh2_vxlan_nh_info_t;

STATIC int
bcmi_gh2_vxlan_port_vp_get(
    int unit,
    bcm_vpn_t vpn,
    int vp,
    bcm_vxlan_port_t *vxlan_port);

STATIC void
bcmi_gh2_vxlan_match_clear(int unit, int vp);

STATIC int
bcmi_gh2_vxlan_riot_custom_header_add(
    int unit,
    bcm_vxlan_port_t *vxlan_port,
    bcm_vxlan_port_t *old_vxlan_port,
    int vp);
STATIC int
bcmi_gh2_vxlan_riot_custom_header_delete(
    int unit,
    bcm_vxlan_port_t *vxlan_port,
    int vp);

/*
 * Function:
 *    gh2_vxlan_bk_info_instance_get
 * Description:
 *    Helper function to retrieve VXLAN bookkeeping info pointer
 * Parameters:
 *    unit          - (IN) Device Number
 *    vxlan_bk_info - (OUT) VXLAN bookkeeping info data structure
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
gh2_vxlan_bk_info_instance_get(
    int unit,
    bcmi_gh2_vxlan_bookkeeping_t **vxlan_bk_info)
{
    /* Input parameter check */
    if (NULL == vxlan_bk_info) {
        return BCM_E_PARAM;
    }
    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
        return BCM_E_UNIT;
    }

    /* Check if VXLAN-LITE supported and initialized */
    if (!soc_feature(unit, soc_feature_vxlan_lite)) {
        return BCM_E_UNAVAIL;
    }
    if (NULL == bcmi_gh2_vxlan_bk_info[unit]) {
        LOG_ERROR(BSL_LS_BCM_VXLAN,
                  (BSL_META_U(unit,
                              "VXLAN bookkeeping info Error: "
                              "not initialized\n")));
        return BCM_E_INIT;
    }
    if (!bcmi_gh2_vxlan_bk_info[unit]->initialized) {
        LOG_ERROR(BSL_LS_BCM_VXLAN,
                  (BSL_META_U(unit,
                              "VXLAN module not initialized\n")));
        return BCM_E_INIT;
    }

    /* Fill info structure. */
    *vxlan_bk_info = bcmi_gh2_vxlan_bk_info[unit];

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_check_init
 * Purpose:
 *    Check if VXLAN is initialized
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_check_init(int unit)
{
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
        return BCM_E_UNIT;
    }

    vxlan_info = GH2_VXLAN_INFO(unit);

    if ((vxlan_info == NULL) || (vxlan_info->initialized == FALSE)) {
        return BCM_E_INIT;
    } else {
        return BCM_E_NONE;
    }
}

/*
 * Function:
 *    bcmi_gh2_vxlan_lock
 * Purpose:
 *    Take VXLAN Lock Semaphore
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_lock(int unit)
{
    int rv = BCM_E_NONE;

    rv = bcmi_gh2_vxlan_check_init(unit);

    if (rv == BCM_E_NONE) {
        sal_mutex_take(GH2_VXLAN_INFO(unit)->vxlan_mutex, sal_mutex_FOREVER);
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_unlock
 * Purpose:
 *    Release VXLAN Lock Semaphore
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
void
bcmi_gh2_vxlan_unlock(int unit)
{
    int rv = BCM_E_NONE;

    rv = bcmi_gh2_vxlan_check_init(unit);

    if (rv == BCM_E_NONE) {
        sal_mutex_give(GH2_VXLAN_INFO(unit)->vxlan_mutex);
    }
}

/*
 * Function:
 *    bcmi_gh2_vxlan_udp_dest_port_set
 * Purpose:
 *    Set UDP Dest port for VXLAN
 * Parameters:
 *    unit - (IN) Device Number
 *    type - (IN) The desired configuration parameter to modify
 *    udp_destport - (IN) UDP Dest port (Non-zero value)
 * Returns:
 *    BCM_E_XXX.
 */
int
bcmi_gh2_vxlan_udp_dest_port_set(
    int unit,
    bcm_switch_control_t type,
    int udp_destport)
{
    soc_reg_t reg = INVALIDr ;
    soc_field_t reg_field = INVALIDf;
    uint64 regval64;
    uint32 fieldval;

    COMPILER_64_ZERO(regval64);

    /* Given control type select register. */
    switch (type) {
        case bcmSwitchVxlanUdpDestPortSet:
            reg = ING_VXLAN_CONTROLr;
            reg_field = UDP_DST_PORT_NUMBER_1f;
            break;
        case bcmSwitchVxlanUdpDestPortSet1:
            reg = ING_VXLAN_CONTROLr;
            reg_field = UDP_DST_PORT_NUMBER_2f;
            break;
        default:
            return BCM_E_PARAM;
    }

    if (!SOC_REG_FIELD_VALID(unit, reg, reg_field)) {
        return BCM_E_UNAVAIL;
    }

    /* input parameter range check */
    if ((udp_destport < 0) || (udp_destport > 0xFFFF)) {
         return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &regval64));
    fieldval = soc_reg64_field32_get(unit, reg, regval64, reg_field);
    if (udp_destport != fieldval) {
        soc_reg64_field32_set(unit, reg, &regval64, reg_field, udp_destport);
        BCM_IF_ERROR_RETURN(
            soc_reg64_set(unit, reg, REG_PORT_ANY, 0, regval64));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_udp_dest_port_get
 * Purpose:
 *    Get UDP Dest port for VXLAN
 * Parameters:
 *    unit - (IN) Device Number
 *    type - (IN) The desired configuration parameter to modify
 *    udp_destport - (OUT) UDP Dest port (Non-zero value)
 * Returns:
 *    BCM_E_XXX.
 */
int
bcmi_gh2_vxlan_udp_dest_port_get(
    int unit,
    bcm_switch_control_t type,
    int *udp_destport)
{
    soc_reg_t reg = INVALIDr ;
    soc_field_t reg_field = INVALIDf;
    uint64 regval64;
    uint32 fieldval;

    COMPILER_64_ZERO(regval64);

    /* Given control type select register. */
    switch (type) {
        case bcmSwitchVxlanUdpDestPortSet:
            reg = ING_VXLAN_CONTROLr;
            reg_field = UDP_DST_PORT_NUMBER_1f;
            break;
        case bcmSwitchVxlanUdpDestPortSet1:
            reg = ING_VXLAN_CONTROLr;
            reg_field = UDP_DST_PORT_NUMBER_2f;
            break;
        default:
            return BCM_E_PARAM;
    }

    if (!SOC_REG_FIELD_VALID(unit, reg, reg_field)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(
        soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &regval64));
    fieldval = soc_reg64_field32_get(unit, reg, regval64, reg_field);

    *udp_destport = fieldval;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_udp_source_port_set
 * Purpose:
 *    Enable UDP Source port based HASH for VXLAN
 * Parameters:
 *    unit - (IN) Device Number
 *    type - (IN) The desired configuration parameter to modify
 *    hash_enable - (IN) Enable Hash for UDP SourcePort
 * Returns:
 *    BCM_E_XXX.
 */
int
bcmi_gh2_vxlan_udp_source_port_set(
    int unit,
    bcm_switch_control_t type,
    int hash_enable)
{
    soc_reg_t reg = INVALIDr ;
    soc_field_t reg_field = INVALIDf;
    uint32 regval, fieldval;

    /* Given control type select register. */
    switch (type) {
        case bcmSwitchVxlanEntropyEnable:
            reg = EGR_VXLAN_CONTROLr;
            reg_field = UDP_SOURCE_PORT_SELf;
            break;
        case bcmSwitchVxlanEntropyEnableIP6:
            reg = EGR_VXLAN_CONTROLr;
            reg_field = IPV6_FLOW_LABEL_SELf;
            break;
        default:
            return BCM_E_PARAM;
    }

    if (!SOC_REG_FIELD_VALID(unit, reg, reg_field)) {
        return BCM_E_UNAVAIL;
    }

    if ((hash_enable < 0) || (hash_enable > 1)) {
         return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &regval));
    fieldval = soc_reg_field_get(unit, reg, regval, reg_field);
    if (hash_enable != fieldval) {
        soc_reg_field_set(unit, reg, &regval, reg_field, hash_enable);
        BCM_IF_ERROR_RETURN(
            soc_reg32_set(unit, reg, REG_PORT_ANY, 0, regval));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_udp_source_port_get
 * Purpose:
 *    Get UDP Source port based HASH for VXLAN
 * Parameters:
 *    unit - (IN) Device Number
 *    type - (IN) The desired configuration parameter to modify
 *    hash_enable - (OUT) Enable Hash for UDP SourcePort
 * Returns:
 *    BCM_E_XXX.
 */
int
bcmi_gh2_vxlan_udp_source_port_get(
    int unit,
    bcm_switch_control_t type,
    int *hash_enable)
{
    soc_reg_t reg = INVALIDr ;
    soc_field_t reg_field = INVALIDf;
    uint32 regval, fieldval;

    /* Given control type select register. */
    switch (type) {
        case bcmSwitchVxlanEntropyEnable:
            reg = EGR_VXLAN_CONTROLr;
            reg_field = UDP_SOURCE_PORT_SELf;
            break;
        case bcmSwitchVxlanEntropyEnableIP6:
            reg = EGR_VXLAN_CONTROLr;
            reg_field = IPV6_FLOW_LABEL_SELf;
            break;
        default:
            return BCM_E_PARAM;
    }

    if (!SOC_REG_FIELD_VALID(unit, reg, reg_field)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &regval));
    fieldval = soc_reg_field_get(unit, reg, regval, reg_field);

    *hash_enable = fieldval;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_free_resource
 * Purpose:
 *    Free all allocated software resources
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    Nothing
 */
STATIC void
bcmi_gh2_vxlan_free_resource(int unit)
{
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* If software tables were not allocated we are done. */
    if (NULL == GH2_VXLAN_INFO(unit)) {
        return;
    }

    vxlan_info = GH2_VXLAN_INFO(unit);

    if (vxlan_info->vxlan_mutex != NULL) {
        sal_mutex_destroy(vxlan_info->vxlan_mutex);
        vxlan_info->vxlan_mutex = NULL;
    }

    /* Destroy EGR_IP_TUNNEL usage bitmap */
    if (vxlan_info->vxlan_ip_tnl_bitmap) {
        sal_free(vxlan_info->vxlan_ip_tnl_bitmap);
        vxlan_info->vxlan_ip_tnl_bitmap = NULL;
    }

    if (vxlan_info->match_key) {
        sal_free(vxlan_info->match_key);
        vxlan_info->match_key = NULL;
    }

    if (vxlan_info->vxlan_tunnel_init) {
        sal_free(vxlan_info->vxlan_tunnel_init);
        vxlan_info->vxlan_tunnel_init = NULL;
    }

    if (vxlan_info->vxlan_tunnel_term) {
        sal_free(vxlan_info->vxlan_tunnel_term);
        vxlan_info->vxlan_tunnel_term = NULL;
    }

    /* Destroy VXLAN VFI usage bitmap */
    if (vxlan_info->vxlan_vfi_bitmap) {
        sal_free(vxlan_info->vxlan_vfi_bitmap);
        vxlan_info->vxlan_vfi_bitmap = NULL;
    }

    /* Destroy VXLAN VFI info */
    if (vxlan_info->vxlan_vpn_info) {
        sal_free(vxlan_info->vxlan_vpn_info);
        vxlan_info->vxlan_vpn_info = NULL;
    }

    /* Destroy VXLAN Virtual Port usage bitmap */
    if (vxlan_info->vxlan_vp_bitmap) {
        sal_free(vxlan_info->vxlan_vp_bitmap);
        vxlan_info->vxlan_vp_bitmap = NULL;
    }

    /* Destroy Network VXLAN Virtual Port usage bitmap */
    if (vxlan_info->vxlan_network_vp_bitmap) {
        sal_free(vxlan_info->vxlan_network_vp_bitmap);
        vxlan_info->vxlan_network_vp_bitmap = NULL;
    }

    /* Destroy VXLAN Outer TPID info */
    if (vxlan_info->vxlan_outer_tpid) {
        sal_free(vxlan_info->vxlan_outer_tpid);
        vxlan_info->vxlan_outer_tpid = NULL;
    }

    /* Destroy VXLAN Ingress VLAN XLATE info */
    if (vxlan_info->vxlan_vlan_xlate) {
        sal_free(vxlan_info->vxlan_vlan_xlate);
        vxlan_info->vxlan_vlan_xlate = NULL;
    }

    /* Destroy VXLAN VP info */
    if (vxlan_info->vxlan_vp_info) {
        sal_free(vxlan_info->vxlan_vp_info);
        vxlan_info->vxlan_vp_info = NULL;
    }

    /* Destroy Loopback VXLAN Virtual Port usage bitmap (used for RIOT) */
    if (vxlan_info->vxlan_loopback_vp_bitmap) {
        sal_free(vxlan_info->vxlan_loopback_vp_bitmap);
        vxlan_info->vxlan_loopback_vp_bitmap = NULL;
    }

    /* Free module data. */
    sal_free(GH2_VXLAN_INFO(unit));
    GH2_VXLAN_INFO(unit) = NULL;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_allocate_bk
 * Purpose:
 *    Initialize VXLAN software book-kepping
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_allocate_bk(int unit)
{
    /* Allocate/Init unit software tables. */
    if (NULL == GH2_VXLAN_INFO(unit)) {
        BCMI_VXLAN_ALLOC(GH2_VXLAN_INFO(unit),
                         sizeof(bcmi_gh2_vxlan_bookkeeping_t),
                         "vxlan_bk_module_data");
        if (NULL == GH2_VXLAN_INFO(unit)) {
            return BCM_E_MEMORY;
        } else {
            GH2_VXLAN_INFO(unit)->initialized = FALSE;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_outer_tpid_ref_inc
 * Purpose:
 *    Increase reference count for the specified VXLAN outer TPID (per system)
 * Parameters:
 *    unit       - (IN) unit number
 *    tpid_value - (IN) TPID value
 * Returns:
 *    BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_gh2_vxlan_outer_tpid_ref_inc(int unit, uint16 tpid_value)
{
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_outer_tpid_info_t *vxlan_outer_tpid;
    int num_tpid, i;

    vxlan_info = GH2_VXLAN_INFO(unit);
    if (NULL == vxlan_info) {
        LOG_ERROR(BSL_LS_BCM_VXLAN,
                  (BSL_META_U(unit,
                              "VXLAN bookkeeping info Error: "
                              "not initialized\n")));
        return BCM_E_INIT;
    }

    /* Get total number of VXLAN outer TPID */
    num_tpid = vxlan_info->num_vxlan_outer_tpid;

    for (i = 0; i < num_tpid; i++) {
        vxlan_outer_tpid = &(vxlan_info->vxlan_outer_tpid[i]);
        if (vxlan_outer_tpid->ref_count == 0) {
            /* Find free index */
            vxlan_outer_tpid->tpid_value = tpid_value;
            /* Update the reference count */
            vxlan_outer_tpid->ref_count++;
            break;
        } else {
            if (tpid_value == vxlan_outer_tpid->tpid_value) {
                /* Update the reference count */
                vxlan_outer_tpid->ref_count++;
                break;
            }
        }
    }

    if (i == num_tpid) {
        return BCM_E_RESOURCE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_outer_tpid_ref_dec
 * Purpose:
 *    Decrease reference count for the specified VXLAN outer TPID (per system)
 * Parameters:
 *    unit       - (IN) unit number
 *    tpid_value - (IN) TPID value
 * Returns:
 *    BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_gh2_vxlan_outer_tpid_ref_dec(int unit, uint16 tpid_value)
{
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_outer_tpid_info_t *vxlan_outer_tpid;
    int num_tpid, i;
    bcm_switch_tpid_info_t tpid_info;

    vxlan_info = GH2_VXLAN_INFO(unit);
    if (NULL == vxlan_info) {
        LOG_ERROR(BSL_LS_BCM_VXLAN,
                  (BSL_META_U(unit,
                              "VXLAN bookkeeping info Error: "
                              "not initialized\n")));
        return BCM_E_INIT;
    }

    /* Get total number of VXLAN outer TPID */
    num_tpid = vxlan_info->num_vxlan_outer_tpid;

    for (i = 0; i < num_tpid; i++) {
        vxlan_outer_tpid = &(vxlan_info->vxlan_outer_tpid[i]);
        if (vxlan_outer_tpid->ref_count > 0) {
            if (tpid_value == vxlan_outer_tpid->tpid_value) {
                /* Update the reference count */
                vxlan_outer_tpid->ref_count--;
                if (vxlan_outer_tpid->ref_count == 0) {
                    sal_memset(&tpid_info, 0, sizeof(tpid_info));
                    tpid_info.tpid_type = bcmTpidTypeVxlanPayloadOuter;
                    tpid_info.tpid_value = tpid_value;
                    (void)bcm_esw_switch_tpid_delete(unit, &tpid_info);
                }
                break;
            }
        }
    }

    if (i == num_tpid) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vlan_xlate_ref_inc
 * Purpose:
 *    Increase reference count for the specified VXLAN Vlan XLATE entry.
 * Parameters:
 *    unit        - (IN) unit number
 *    entry_index - (IN) Entry index
 * Returns:
 *    BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_gh2_vxlan_vlan_xlate_ref_inc(int unit, int entry_index)
{
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vlan_xlate_info_t *vxlan_vlan_xlate;
    int num_vfi;

    vxlan_info = GH2_VXLAN_INFO(unit);
    if (NULL == vxlan_info) {
        LOG_ERROR(BSL_LS_BCM_VXLAN,
                  (BSL_META_U(unit,
                              "VXLAN bookkeeping info Error: "
                              "not initialized\n")));
        return BCM_E_INIT;
    }

    /* Input parameter check */
    num_vfi = vxlan_info->num_vxlan_vfi;
    if (entry_index >= num_vfi) {
        return BCM_E_RESOURCE;
    }

    /* Get VXLAN VLAN XLATE entry pointer with specified entry_index */
    vxlan_vlan_xlate = &(vxlan_info->vxlan_vlan_xlate[entry_index]);

    /* Update the reference count */
    vxlan_vlan_xlate->ref_count++;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vlan_xlate_ref_dec
 * Purpose:
 *    Decrease reference count for the specified VXLAN Vlan XLATE entry.
 * Parameters:
 *    unit        - (IN) unit number
 *    entry_index - (IN) Entry index
 *    ref_cnt     - (OUT) Reference Count
 * Returns:
 *    BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_gh2_vxlan_vlan_xlate_ref_dec(int unit, int entry_index, int *ref_cnt)
{
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vlan_xlate_info_t *vxlan_vlan_xlate;
    int num_vfi;

    vxlan_info = GH2_VXLAN_INFO(unit);
    if (NULL == vxlan_info) {
        LOG_ERROR(BSL_LS_BCM_VXLAN,
                  (BSL_META_U(unit,
                              "VXLAN bookkeeping info Error: "
                              "not initialized\n")));
        return BCM_E_INIT;
    }

    /* Input parameter check */
    num_vfi = vxlan_info->num_vxlan_vfi;
    if (entry_index >= num_vfi) {
        return BCM_E_RESOURCE;
    }

    if (NULL == ref_cnt) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN VLAN XLATE entry pointer with specified entry_index */
    vxlan_vlan_xlate = &(vxlan_info->vxlan_vlan_xlate[entry_index]);
    if (vxlan_vlan_xlate->ref_count > 0) {
        /* Update the reference count */
        vxlan_vlan_xlate->ref_count--;
    }

    *ref_cnt = vxlan_vlan_xlate->ref_count;

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1, 0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

STATIC int bcmi_gh2_vxlan_wb_alloc(int unit);

/*
 * Function:
 *    bcmi_gh2_vxlan_ecmp_nexthop_entry_recover
 * Purpose:
 *    WB recovery for vxlan ecmp member object's Egress next hop entry
 * Parameters:
 *    unit           - (IN) Device Number
 *    ecmp_nh_index - (IN) ecmp group number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_egr_mac_da_recover(int unit)
{
    int rv = BCM_E_NONE;
    int min, max, i;
    egr_l3_next_hop_entry_t nh_entry;
    int macda_idx;

    min = soc_mem_index_min(unit, EGR_L3_NEXT_HOPm);
    max = soc_mem_index_max(unit, EGR_L3_NEXT_HOPm);

    for (i = min; i < max; i++) {

        /* Recover MAC_DA profile count */
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                         i, &nh_entry));
        if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &nh_entry,
                                             ENTRY_TYPEf) == 0xa) {
            macda_idx = soc_EGR_L3_NEXT_HOPm_field32_get
                             (unit, &nh_entry,
                              VXLAN__MAC_DA_PROFILE_INDEXf);
            _bcm_common_profile_mem_ref_cnt_update(
                unit, EGR_MAC_DA_PROFILEm, macda_idx, 1);
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_wb_recover
 * Purpose:
 *    Recover VXLAN module info for Level 2 Warm Boot from persisitent memory
 * Warm Boot Version Map:
 *    see _bcm_esw_vxlan_sync definition
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_wb_recover(int unit)
{
    int rv = BCM_E_NONE;
    int i;
    int num_tnl = 0, num_vp = 0;
    int stable_size;
    int add_sz = 0;
    bcm_gport_t gport;
    uint8 *vxlan_state = NULL;
    soc_scache_handle_t scache_handle;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    int num_vfi = 0;
    int intf;
    uint32 flag = 0;
    int index = -1;
    uint16 tpid_value;

    vxlan_info = GH2_VXLAN_INFO(unit);

    BCM_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Requires extended scache support level-2 warmboot */
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_VXLAN, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &vxlan_state,
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (vxlan_state != NULL) {
        num_vp = vxlan_info->num_vxlan_vp;
        num_tnl = vxlan_info->num_vxlan_tnl;
        num_vfi = vxlan_info->num_vxlan_vfi;

        /* Version_1_0: bcmi_gh2_vxlan_tunnel_endpoint_t in vxlan_bookkeeping_t
         * bcm_ip_t dip + bcm_ip_t sip + bcm_ip6_t dip6 + bcm_ip6_t sip6 +
         * uin16 tunnel_state + int activate_flag + bcm_vlan_t vlan.
         */
        /* size of (bcmi_gh2_vxlan_tunnel_endpoint_t) */
        add_sz = sizeof(bcmi_gh2_vxlan_tunnel_endpoint_t);

        /*
         * Recover Vxlan Tunnel Terminator
         * (DIP, SIP, DIP6, SIP6, Tunnel State(uint16), activate_flag, vlan)
         * To correspond with the sync actions in Version_1_0
         */
        for (i = 0; i < num_tnl; i++) {
            sal_memcpy(&(vxlan_info->vxlan_tunnel_term[i]),
                       vxlan_state, add_sz);
            vxlan_state += add_sz;
        }

        /*
         * Recover Vxlan Tunnel Initiator
         * (DIP, SIP, DIP6, SIP6, Tunnel State(uint16), activate_flag, vlan)
         * To correspond with the sync actions in Version_1_0
         */
        for (i = 0; i < num_tnl; i++) {
            sal_memcpy(&(vxlan_info->vxlan_tunnel_init[i]),
                       vxlan_state, add_sz);
            vxlan_state += add_sz;
        }

        /* Recover the BITMAP of Vxlan Tunnel usage */
        sal_memcpy(vxlan_info->vxlan_ip_tnl_bitmap, vxlan_state,
                   SHR_BITALLOCSIZE(num_tnl));
        vxlan_state += SHR_BITALLOCSIZE(num_tnl);

        /* Recover the flags & match_tunnel_index of each Match Key */
        for (i = 0; i < num_vp; i++) {
            sal_memcpy(&(vxlan_info->match_key[i].flags),
                       vxlan_state, sizeof(uint32));
            vxlan_state += sizeof(uint32);
            sal_memcpy(&(vxlan_info->match_key[i].match_tunnel_index),
                       vxlan_state, sizeof(uint32));
            vxlan_state += sizeof(uint32);
        }

        /*
         * Recover the index && gport(trunk_id, modid, port) && match_vlan &&
         * match_inner_vlan of each Match Key
         */
        for (i = 0; i < num_vp; i++) {
            sal_memcpy(&(vxlan_info->match_key[i].index), vxlan_state,
                       sizeof(uint32));
            vxlan_state += sizeof(uint32);
            sal_memcpy(&gport, vxlan_state, sizeof(bcm_gport_t));
            if (BCM_GPORT_IS_TRUNK(gport)) {
                vxlan_info->match_key[i].trunk_id =
                    BCM_GPORT_TRUNK_GET(gport);
            } else if (BCM_GPORT_IS_MODPORT(gport)) {
                vxlan_info->match_key[i].modid =
                    BCM_GPORT_MODPORT_MODID_GET(gport);
                vxlan_info->match_key[i].port =
                    BCM_GPORT_MODPORT_PORT_GET(gport);
                vxlan_info->match_key[i].trunk_id = -1;
            }
            vxlan_state += sizeof(bcm_gport_t);
            sal_memcpy(&(vxlan_info->match_key[i].match_vlan), vxlan_state,
                       sizeof(bcm_vlan_t));
            vxlan_state += sizeof(bcm_vlan_t);
            sal_memcpy(&(vxlan_info->match_key[i].match_inner_vlan),
                       vxlan_state, sizeof(bcm_vlan_t));
            vxlan_state += sizeof(bcm_vlan_t);
        }

        /* Recover the BITMAP of Vxlan VFI usage */
        sal_memcpy(vxlan_info->vxlan_vfi_bitmap, vxlan_state,
                   SHR_BITALLOCSIZE(num_vfi));
        vxlan_state += SHR_BITALLOCSIZE(num_vfi);

        /* Recover the type of each vxlan vpn */
        for (i = 0; i < num_vfi; i++) {
            sal_memcpy(&(vxlan_info->vxlan_vpn_info[i].valid),
                       vxlan_state, sizeof(int));
            vxlan_state += sizeof(int);
            sal_memcpy(&(vxlan_info->vxlan_vpn_info[i].vnid),
                       vxlan_state, sizeof(uint32));
            vxlan_state += sizeof(uint32);
        }

        /* Recover the vfi index of each vxlan vp */
        for (i = 0; i < num_vp; i++) {
            sal_memcpy(&(vxlan_info->match_key[i].vfi),
                       vxlan_state, sizeof(int));
            vxlan_state += sizeof(int);
        }

        /* Recover the BITMAP of Vxlan VP usage */
        sal_memcpy(vxlan_info->vxlan_vp_bitmap, vxlan_state,
                   SHR_BITALLOCSIZE(num_vp));
        vxlan_state += SHR_BITALLOCSIZE(num_vp);

        /* Recover the BITMAP of Vxlan VP usage for Network side */
        sal_memcpy(vxlan_info->vxlan_network_vp_bitmap, vxlan_state,
                   SHR_BITALLOCSIZE(num_vp));
        vxlan_state += SHR_BITALLOCSIZE(num_vp);

        /* Recover per VP info */
        for (i = 0; i < num_vp; i++) {
            sal_memcpy(&(vxlan_info->vxlan_vp_info[i].vp_type),
                       vxlan_state, sizeof(uint32));
            vxlan_state += sizeof(uint32);
            sal_memcpy(&(vxlan_info->vxlan_vp_info[i].vfi),
                       vxlan_state, sizeof(int));
            vxlan_state += sizeof(int);
            sal_memcpy(&(vxlan_info->vxlan_vp_info[i].ecmp),
                       vxlan_state, sizeof(int));
            vxlan_state += sizeof(int);
            sal_memcpy(&(vxlan_info->vxlan_vp_info[i].nh_ecmp_index),
                       vxlan_state, sizeof(int));
            vxlan_state += sizeof(int);
            /* Recovering EgrObj ref_count (NextHop/ECMP) */
            if (vxlan_info->vxlan_vp_info[i].ecmp) {
                intf = vxlan_info->vxlan_vp_info[i].nh_ecmp_index +
                       BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
            } else {
                intf = vxlan_info->vxlan_vp_info[i].nh_ecmp_index +
                       BCM_XGS3_EGRESS_IDX_MIN(unit);
            }
            rv = bcm_xgs3_get_nh_from_egress_object(unit, intf, &flag, 1,
                                                    &index);
            BCM_IF_ERROR_RETURN(rv);

            sal_memcpy(&(vxlan_info->vxlan_vp_info[i].tpid_valid),
                       vxlan_state, sizeof(int));
            vxlan_state += sizeof(int);
            sal_memcpy(&(vxlan_info->vxlan_vp_info[i].tpid_value),
                       vxlan_state, sizeof(uint16));
            vxlan_state += sizeof(uint16);

            /* Recovering VXLAN Outer TPID reference count */
            if (vxlan_info->vxlan_vp_info[i].tpid_valid) {
                tpid_value = vxlan_info->vxlan_vp_info[i].tpid_value;
                BCM_IF_ERROR_RETURN(
                    bcmi_gh2_vxlan_outer_tpid_ref_inc(unit, tpid_value));
            }
        }

        /* Recover the reference count of each vxlan vlan xlate entry */
        for (i = 0; i < num_vfi; i++) {
            sal_memcpy(&(vxlan_info->vxlan_vlan_xlate[i].ref_count),
                       vxlan_state, sizeof(int));
            vxlan_state += sizeof(int);
        }

        /* Recover Custom Header info per VP info (used for RIOT) */
        for (i = 0; i < num_vp; i++) {
            sal_memcpy(&(vxlan_info->vxlan_vp_info[i].ch_encap_id),
                       vxlan_state, sizeof(bcm_if_t));
            vxlan_state += sizeof(bcm_if_t);
            sal_memcpy(&(vxlan_info->vxlan_vp_info[i].ch_match_id_ipmc),
                       vxlan_state, sizeof(int));
            vxlan_state += sizeof(int);
            sal_memcpy(&(vxlan_info->vxlan_vp_info[i].ch_match_id_non_ipmc),
                       vxlan_state, sizeof(int));
            vxlan_state += sizeof(int);
        }

        /*
         * Recover the BITMAP of Vxlan VP usage for Loopback side
         * (used for RIOT)
         */
        sal_memcpy(vxlan_info->vxlan_loopback_vp_bitmap, vxlan_state,
                   SHR_BITALLOCSIZE(num_vp));
        vxlan_state += SHR_BITALLOCSIZE(num_vp);

        /*
         * Recover Reference count of Custom Header index0
         * (used for RIOT)
         */
        sal_memcpy(&(vxlan_info->riot_custom_header_index0_ref_count),
                   vxlan_state, sizeof(int));
        vxlan_state += sizeof(int);

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_wb_alloc
 * Purpose:
 *    Alloc persisitent memory for Level 2 Warm Boot scache.
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_wb_alloc(int unit)
{
    int rv = BCM_E_NONE;
    int alloc_sz = 0;
    soc_scache_handle_t scache_handle;
    int num_tnl = 0, num_vp = 0;
    int num_vfi = 0;
    uint8 *vxlan_state;
    int stable_size;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    vxlan_info = GH2_VXLAN_INFO(unit);

    BCM_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    /* Size of Tunnel Terminator & Initiator */
    num_tnl = vxlan_info->num_vxlan_tnl;
    alloc_sz += num_tnl * sizeof(bcmi_gh2_vxlan_tunnel_endpoint_t) * 2;

    /* Size of EGR_IP_TUNNEL index bitmap */
    alloc_sz += SHR_BITALLOCSIZE(num_tnl);

    /* Size of All match_key's flags & match_tunnel_index */
    num_vp = vxlan_info->num_vxlan_vp;
    alloc_sz += num_vp * sizeof(uint32) * 2;

    /*
     * Size of All match_key's index && gport(trunk_id, modid, port) &&
     * match_vlan && match_inner_vlan
     */
    alloc_sz += num_vp * (sizeof(uint32) + sizeof(bcm_gport_t) +
                          sizeof(bcm_vlan_t) * 2);

    /* Size of VXLAN VFI bitmap */
    num_vfi = vxlan_info->num_vxlan_vfi;
    alloc_sz += SHR_BITALLOCSIZE(num_vfi);

    /* Size of VXLAN VFI info data structure */
    alloc_sz += num_vfi * sizeof(gh2_vxlan_vpn_info_t);
    /* Size of match_key's vfi related with vp */
    alloc_sz += num_vp * sizeof(int);

    /* Size of VXLAN Virtual Port bitmap & Network Virtual Port bitmap */
    alloc_sz += SHR_BITALLOCSIZE(num_vp);
    alloc_sz += SHR_BITALLOCSIZE(num_vp);

    /* Size of VXLAN per VLAN XLATE entry info */
    alloc_sz += num_vfi * sizeof(gh2_vxlan_vlan_xlate_info_t);

    /* Size of VXLAN per VP info */
    alloc_sz += num_vp * sizeof(gh2_vxlan_vp_info_t);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_VXLAN, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 alloc_sz, (uint8**)&vxlan_state,
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_initiator_reinit
 * Purpose:
 *    Tunnel initiator hash and ref-count recovery.
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_tunnel_initiator_reinit(int unit)
{
    bcm_tunnel_initiator_t tnl_init_info;
    soc_mem_t mem, mem_ip6;
    int i = 0;
    int idx_min = 0;
    int idx_max = 0;
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read hw entry */
    int entry_type = 0; /* Entry type (IPv4 or IPv6). */
    int tnl_type = 0; /* Tunnel type. */
    _bcm_l3_tbl_t *tbl_ptr; /* Tunnel init table pointer. */

    mem = BCM_XGS3_L3_MEM(unit, tnl_init_v4);
    mem_ip6 = BCM_XGS3_L3_MEM(unit, tnl_init_v6);
    idx_min = soc_mem_index_min(unit, mem);
    idx_max = soc_mem_index_max(unit, mem);
    tbl_ptr = BCM_XGS3_L3_TBL_PTR(unit, tnl_init);
    for (i = idx_min; i <= idx_max; i++) {
        /* Initialize the buffer */
        sal_memset(tnl_entry, 0, sizeof(tnl_entry));
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, i, tnl_entry));

        tnl_type = soc_mem_field32_get(unit, mem, tnl_entry, TUNNEL_TYPEf);
        entry_type = soc_mem_field32_get(unit, mem, tnl_entry, ENTRY_TYPEf);

        if (tnl_type == _BCM_VXLAN_TUNNEL_TYPE) {
            if (entry_type == BCM_XGS3_TUNNEL_INIT_V6) {
                /* Initialize the buffer */
                sal_memset(tnl_entry, 0, sizeof(tnl_entry));
                BCM_IF_ERROR_RETURN(
                    soc_mem_read(unit, mem_ip6, MEM_BLOCK_ANY,
                                 (i / 2), tnl_entry));

                bcm_tunnel_initiator_t_init(&tnl_init_info);

                /* Tunnel for IPv6 */
                tnl_init_info.type = bcmTunnelTypeVxlan6;

                /* Extract IPv6 SIP/DIP address */
                soc_mem_ip6_addr_get(unit, mem_ip6, tnl_entry, SIPf,
                                     tnl_init_info.sip6,
                                     SOC_MEM_IP6_FULL_ADDR);
                soc_mem_ip6_addr_get(unit, mem_ip6, tnl_entry, DIPf,
                                     tnl_init_info.dip6,
                                     SOC_MEM_IP6_FULL_ADDR);

                tnl_init_info.udp_dst_port =
                    soc_mem_field32_get(unit, mem_ip6,
                                        tnl_entry, L4_DST_PORTf);
                BCM_IF_ERROR_RETURN(bcmi_gh2_vxlan_tunnel_initiator_hash_calc
                    (unit, &tnl_init_info,
                    &BCM_XGS3_L3_ENT_HASH(tbl_ptr, i)));
                BCM_IF_ERROR_RETURN(bcmi_gh2_vxlan_tunnel_initiator_hash_calc
                    (unit, &tnl_init_info,
                    &BCM_XGS3_L3_ENT_HASH(tbl_ptr, (i + 1))));
                BCM_XGS3_L3_ENT_REF_CNT_INC(
                    tbl_ptr, i,
                    ((_BCM_TUNNEL_OUTER_HEADER_IPV6(tnl_init_info.type)) ?
                    _BCM_DOUBLE_WIDE : _BCM_SINGLE_WIDE));
                /* Update Maximum used index */
                tbl_ptr->idx_maxused = i;
            } else if (entry_type == BCM_XGS3_TUNNEL_INIT_V4) {
                bcm_tunnel_initiator_t_init(&tnl_init_info);

                /* Tunnel for IPv4 */
                tnl_init_info.type = bcmTunnelTypeVxlan;

                /* Get IPv4 SIP/DIP address */
                tnl_init_info.sip =
                    soc_mem_field32_get(unit, mem, tnl_entry, SIPf);
                tnl_init_info.dip =
                    soc_mem_field32_get(unit, mem, tnl_entry, DIPf);

                tnl_init_info.udp_dst_port =
                    soc_mem_field32_get(unit, mem, tnl_entry, L4_DST_PORTf);
                BCM_IF_ERROR_RETURN(bcmi_gh2_vxlan_tunnel_initiator_hash_calc
                    (unit, &tnl_init_info,
                     &BCM_XGS3_L3_ENT_HASH(tbl_ptr, i)));
                BCM_XGS3_L3_ENT_REF_CNT_INC(
                    tbl_ptr, i,
                    ((_BCM_TUNNEL_OUTER_HEADER_IPV6(tnl_init_info.type)) ?
                    _BCM_DOUBLE_WIDE : _BCM_SINGLE_WIDE));
                /* Update Maximum used index */
                tbl_ptr->idx_maxused = i;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_reinit
 * Purpose:
 *    Warm boot recovery for the VXLAN software module
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_reinit(int unit)
{

    /* Tunnel initiator hash and ref-count recovery */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_tunnel_initiator_reinit(unit));

    /* Recover L2 scache */
    BCM_IF_ERROR_RETURN(bcmi_gh2_vxlan_wb_recover(unit));

    /* Recover EGR_MAC_DA reference count */
    BCM_IF_ERROR_RETURN(bcmi_gh2_vxlan_egr_mac_da_recover(unit));

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_sync
 * Purpose:
 *    This routine extracts the state that needs to be stored from the
 *    book-keeping structs and stores it in the allocated scache location.
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_sync(int unit)
{
    int rv = BCM_E_NONE;
    int i, num_tnl = 0, num_vp = 0;
    int num_vfi = 0;
    int size;
    int stable_size;
    bcm_gport_t gport;
    uint8 *vxlan_state;
    soc_scache_handle_t scache_handle;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    if (!soc_feature(unit, soc_feature_vxlan_lite)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    vxlan_info = GH2_VXLAN_INFO(unit);

    BCM_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Requires extended scache support level-2 warmboot */
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_VXLAN, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &vxlan_state,
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv) && (rv != BCM_E_INTERNAL)) {
        return rv;
    }

    num_vp = vxlan_info->num_vxlan_vp;
    num_tnl = vxlan_info->num_vxlan_tnl;
    size = num_tnl * sizeof(bcmi_gh2_vxlan_tunnel_endpoint_t);

    /* Store each Vxlan Tunnnel Terminator entry (SIP, DIP, Tunnel State) */
    /*
     * vxlan_tunnel_term points to a struct array with a base type of
     * bcmi_gh2_vxlan_tunnel_endpoint_t, sized to hold the info of each
     * virtual port tunnel.
     *
     * EGR_IP_TUNNELm hw index equals to array index, related to the IP & state
     * info of each VP.
     *
     * bcmi_gh2_vxlan_tunnel_endpoint_t[size of (EGR_IP_TUNNELm)] *
     * vxlan_tunnel_term
     */
    sal_memcpy(vxlan_state, vxlan_info->vxlan_tunnel_term, size);
    vxlan_state += size;

    /* Store each Vxlan Tunnel Initiator entry (SIP, DIP, Tunnel State) */
    /*
     * Same as vxlan_tunnel_term.
     */
    sal_memcpy(vxlan_state, vxlan_info->vxlan_tunnel_init, size);
    vxlan_state += size;

    /* Store the BITMAP of Vxlan Tunnel usage */
    /*
     * vxlan_ip_tnl_bitmap points to a struct array with a base type of uint32.
     * sized to hold the maximum number of Egress IP Tunnel.
     *
     * EGR_IP_TUNNELm hw index used as offset of this wide bitmap.
     *
     * uint32[(size of (EGR_IP_TUNNELm) + 31) / 32] * vxlan_ip_tnl_bitmap
     */
    sal_memcpy(vxlan_state, vxlan_info->vxlan_ip_tnl_bitmap,
               SHR_BITALLOCSIZE(num_tnl));
    vxlan_state += SHR_BITALLOCSIZE(num_tnl);

    /* Store the flags & match_tunnel_index of each Match Key */
    /*
     * Since we can't recover flags & match_tunnel_index from HW,
     * so we need to store it for warmboot recovery.
     */
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(vxlan_state, &(vxlan_info->match_key[i].flags),
                   sizeof(uint32));
        vxlan_state += sizeof(uint32);
        sal_memcpy(vxlan_state, &(vxlan_info->match_key[i].match_tunnel_index),
                   sizeof(uint32));
        vxlan_state += sizeof(uint32);
    }

    /*
     * Store the index & gport(trunk_id, modid, port) & match_vlan &
     * match_inner_vlan of each Match Key
     *
     * Since we can't recover index & gport(trunk_id, modid, port) &
     * match_vlan & match_inner_vlan from HW, so we need to store it
     * for warmboot recovery.
     */
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(vxlan_state, &(vxlan_info->match_key[i].index),
                   sizeof(uint32));
        vxlan_state += sizeof(uint32);
        if (vxlan_info->match_key[i].trunk_id != -1) {
            BCM_GPORT_TRUNK_SET(gport, vxlan_info->match_key[i].trunk_id);
        } else if (vxlan_info->match_key[i].modid != -1) {
            BCM_GPORT_MODPORT_SET(gport,
                                  vxlan_info->match_key[i].modid,
                                  vxlan_info->match_key[i].port);
        } else {
            gport = BCM_GPORT_INVALID;
        }
        sal_memcpy(vxlan_state, &gport, sizeof(bcm_gport_t));
        vxlan_state += sizeof(bcm_gport_t);
        sal_memcpy(vxlan_state, &(vxlan_info->match_key[i].match_vlan),
                   sizeof(bcm_vlan_t));
        vxlan_state += sizeof(bcm_vlan_t);
        sal_memcpy(vxlan_state, &(vxlan_info->match_key[i].match_inner_vlan),
                   sizeof(bcm_vlan_t));
        vxlan_state += sizeof(bcm_vlan_t);
    }

    num_vfi = vxlan_info->num_vxlan_vfi;

    /* Store the BITMAP of Vxlan VFI usage */
    /*
     * vxlan_vfi_bitmap points to a struct array with a base type of uint32.
     * sized to hold the maximum number of Vxlan VFI.
     *
     * VFI sw index used as offset of this wide bitmap.
     *
     * uint32[(size of (VLAN_XLATEm or VFIm) + 31) / 32] * vxlan_vfi_bitmap
     * VFIm is support for GH2-B0. VLAN_XLATEm is used for GH2-A0.
     */
    sal_memcpy(vxlan_state, vxlan_info->vxlan_vfi_bitmap,
               SHR_BITALLOCSIZE(num_vfi));
    vxlan_state += SHR_BITALLOCSIZE(num_vfi);

    /* Store the type of each vxlan vpn */
    for (i = 0; i < num_vfi; i++) {
        sal_memcpy(vxlan_state, &(vxlan_info->vxlan_vpn_info[i].valid),
                   sizeof(int));
        vxlan_state += sizeof(int);
        sal_memcpy(vxlan_state, &(vxlan_info->vxlan_vpn_info[i].vnid),
                   sizeof(uint32));
        vxlan_state += sizeof(uint32);
    }

    /* Store the vfi index for vp */
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(vxlan_state, &(vxlan_info->match_key[i].vfi),
                   sizeof(int));
        vxlan_state += sizeof(int);
    }

    /* Store the BITMAP of Vxlan VP usage */
    /*
     * vxlan_vp_bitmap points to a struct array with a base type of uint32.
     * sized to hold the maximum number of Vxlan VP.
     *
     * VP sw index used as offset of this wide bitmap.
     *
     * uint32[(size of (VLAN_XLATEm) + 31) / 32] * vxlan_vp_bitmap
     */
    sal_memcpy(vxlan_state, vxlan_info->vxlan_vp_bitmap,
               SHR_BITALLOCSIZE(num_vp));
    vxlan_state += SHR_BITALLOCSIZE(num_vp);

    /* Store the BITMAP of Vxlan VP usage for Network side */
    /*
     * Same as vxlan_vp_bitmap.
     */
    sal_memcpy(vxlan_state, vxlan_info->vxlan_network_vp_bitmap,
               SHR_BITALLOCSIZE(num_vp));
    vxlan_state += SHR_BITALLOCSIZE(num_vp);

    /* Store per VP info */
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(vxlan_state, &(vxlan_info->vxlan_vp_info[i].vp_type),
                   sizeof(uint32));
        vxlan_state += sizeof(uint32);
        sal_memcpy(vxlan_state, &(vxlan_info->vxlan_vp_info[i].vfi),
                   sizeof(int));
        vxlan_state += sizeof(int);
        sal_memcpy(vxlan_state, &(vxlan_info->vxlan_vp_info[i].ecmp),
                   sizeof(int));
        vxlan_state += sizeof(int);
        sal_memcpy(vxlan_state, &(vxlan_info->vxlan_vp_info[i].nh_ecmp_index),
                   sizeof(int));
        vxlan_state += sizeof(int);
        sal_memcpy(vxlan_state, &(vxlan_info->vxlan_vp_info[i].tpid_valid),
                   sizeof(int));
        vxlan_state += sizeof(int);
        sal_memcpy(vxlan_state, &(vxlan_info->vxlan_vp_info[i].tpid_value),
                   sizeof(uint16));
        vxlan_state += sizeof(uint16);
    }

    /* Store the ref_cnt of each vxlan vlan xlate entry */
    for (i = 0; i < num_vfi; i++) {
        sal_memcpy(vxlan_state, &(vxlan_info->vxlan_vlan_xlate[i].ref_count),
                   sizeof(int));
        vxlan_state += sizeof(int);
    }

    /* Store Cstom Header info per VP info (used for RIOT) */
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(vxlan_state, &(vxlan_info->vxlan_vp_info[i].ch_encap_id),
                   sizeof(bcm_if_t));
        vxlan_state += sizeof(bcm_if_t);
        sal_memcpy(vxlan_state,
                   &(vxlan_info->vxlan_vp_info[i].ch_match_id_ipmc),
                   sizeof(int));
        vxlan_state += sizeof(int);
        sal_memcpy(vxlan_state,
                   &(vxlan_info->vxlan_vp_info[i].ch_match_id_non_ipmc),
                   sizeof(int));
        vxlan_state += sizeof(int);
    }

    /* Store the BITMAP of Vxlan VP usage for Loopback side (used for RIOT) */
    sal_memcpy(vxlan_state, vxlan_info->vxlan_loopback_vp_bitmap,
               SHR_BITALLOCSIZE(num_vp));
    vxlan_state += SHR_BITALLOCSIZE(num_vp);

    /* Store Reference count of Custom Header index0 (used for RIOT) */
    sal_memcpy(vxlan_state, &(vxlan_info->riot_custom_header_index0_ref_count),
               sizeof(int));
    vxlan_state += sizeof(int);

    return rv;

}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *    bcmi_gh2_vxlan_hw_clear
 * Purpose:
 *    Perform hw tables clean up for VXLAN module.
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_hw_clear(int unit)
{
    int rv = BCM_E_NONE, rv_error = BCM_E_NONE;

    rv = bcmi_gh2_vxlan_tunnel_terminator_destroy_all(unit);
    if (BCM_FAILURE(rv)) {
        rv_error = rv;
    }

    rv = bcmi_gh2_vxlan_tunnel_initiator_destroy_all(unit);
    if (BCM_FAILURE(rv) && (rv_error == BCM_E_NONE)) {
        rv_error = rv;
    }

    rv = bcmi_gh2_vxlan_vpn_destroy_all(unit);
    if (BCM_FAILURE(rv) && (rv_error == BCM_E_NONE)) {
        rv_error = rv;
    }

    return rv_error;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vfi_alloc
 * Purpose:
 *    Internal function for allocating a VFI
 * Parameters:
 *    unit - (IN) Device Number
 *    vfi  - (OUT) VFI index
 * Returns:
 *    BCM_X_XXX
 */
STATIC int
bcmi_gh2_vxlan_vfi_alloc(int unit, int *vfi)
{
    int i, num_vfi;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info = GH2_VXLAN_INFO(unit);

    if (NULL == vfi) {
        return BCM_E_PARAM;
    }

    num_vfi = vxlan_info->num_vxlan_vfi;

    for (i = 0; i < num_vfi; i++) {
        if (!GH2_VXLAN_VFI_USED_GET(unit, i)) {
            break;
        }
    }

    if (i == num_vfi) {
        return BCM_E_RESOURCE;
    }

    GH2_VXLAN_VFI_USED_SET(unit, i);
    *vfi = i;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vfi_alloc_with_id
 * Purpose:
 *    Internal function for allocating a VFI with a given ID
 * Parameters:
 *    unit - (IN) Device Number
 *    vfi  - (IN) VFI index
 * Returns:
 *    BCM_X_XXX
 */
STATIC int
bcmi_gh2_vxlan_vfi_alloc_with_id(int unit, int vfi)
{
    int num_vfi;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info = GH2_VXLAN_INFO(unit);

    num_vfi = vxlan_info->num_vxlan_vfi;

    /* Check Valid range of VFI */
    if (vfi < 0 || vfi >= num_vfi) {
         return BCM_E_RESOURCE;
    }

    if (GH2_VXLAN_VFI_USED_GET(unit, vfi)) {
        return BCM_E_EXISTS;
    }

    GH2_VXLAN_VFI_USED_SET(unit, vfi);

    return BCM_E_NONE;
}


/*
 * Function:
 *    bcmi_gh2_vxlan_vfi_free
 * Purpose:
 *    Internal function for freeing a VFI
 * Parameters:
 *    unit - (IN) Device Number
 *    vfi  - (IN) VFI index
 * Returns:
 *    None
 */
STATIC int
bcmi_gh2_vxlan_vfi_free(int unit, int vfi)
{
    int num_vfi;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info = GH2_VXLAN_INFO(unit);
    vfi_entry_t vfi_entry;

    num_vfi = vxlan_info->num_vxlan_vfi;

    /* Check Valid range of VFI */
    if (vfi < 0 || vfi >= num_vfi) {
         return BCM_E_RESOURCE;
    }

    GH2_VXLAN_VFI_USED_CLR(unit, vfi);

    if (soc_feature(unit, soc_feature_vxlan_lite_riot)) {
        /* Only support for GH2-B0 with new VFI table */
        sal_memset(&vfi_entry, 0, sizeof(vfi_entry_t));
        SOC_IF_ERROR_RETURN(
            soc_mem_write(unit, VFIm, MEM_BLOCK_ALL, vfi, &vfi_entry));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vfi_used_get
 * Purpose:
 *    Check whether a VFI is used or not
 * Parameters:
 *    unit - (IN) Device Number
 *    vfi  - (IN) VFI index
 * Returns:
 *    Boolean
 */
int
bcmi_gh2_vxlan_vfi_used_get(int unit, int vfi)
{
    int rv;
    int num_vfi;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info = GH2_VXLAN_INFO(unit);

    num_vfi = vxlan_info->num_vxlan_vfi;

    /* Check Valid range of VFI */
    if (vfi < 0 || vfi >= num_vfi) {
         return BCM_E_RESOURCE;
    }

    rv = GH2_VXLAN_VFI_USED_GET(unit, vfi);

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vp_info_init
 * Purpose:
 *    Internal function to initialize bcmi_vxlan_vp_info_t structure
 * Parameters:
 *    vxlan_vp_info - (IN/OUT) variable
 * Returns:
 *    void
 */
STATIC void
bcmi_gh2_vxlan_vp_info_init(bcmi_vxlan_vp_info_t *vxlan_vp_info)
{
    if (vxlan_vp_info != NULL) {
        sal_memset(vxlan_vp_info, 0, sizeof(*vxlan_vp_info));
    }

    return;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vp_alloc
 * Purpose:
 *    Internal function for allocating a group of VPs
 * Parameters:
 *    unit  - (IN) Device Number
 *    start - (IN) First VP index to allocate from
 *    end   - (IN) Last VP index to allocate from
 *    count - (IN) How many consecutive VPs to allocate
 *    vxlan_vp_info - (IN) VXLAN VP information
 *    base_vp - (OUT) Base VP index
 * Returns:
 *    BCM_X_XXX
 */
STATIC int
bcmi_gh2_vxlan_vp_alloc(
    int unit,
    int start,
    int end,
    int count,
    bcmi_vxlan_vp_info_t vxlan_vp_info,
    int *base_vp)
{
    int num_vp;
    int i, j;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info = GH2_VXLAN_INFO(unit);

    num_vp = vxlan_info->num_vxlan_vp;

    /* Input parameter check */
    if (NULL == base_vp) {
        return BCM_E_PARAM;
    }
    if ((end < 0) || (start < 0) || (end < start) ||
        (end >= num_vp) || (start >= num_vp)) {
        return BCM_E_PARAM;
    }

    if (vxlan_vp_info.flags) {
        /* Loopback VP is only support when VXLAN RIOT is enabled */
        if (GH2_VXLAN_RIOT_ENABLE(unit)) {
            if (!(vxlan_vp_info.flags & GH2_VXLAN_VP_INFO_NETWORK_PORT) &&
                !(vxlan_vp_info.flags & GH2_VXLAN_VP_INFO_LOOPBACK_PORT)) {
                return BCM_E_UNAVAIL;
            }
        } else {
            if (!(vxlan_vp_info.flags & GH2_VXLAN_VP_INFO_NETWORK_PORT)) {
                return BCM_E_UNAVAIL;
            }
        }
    }

    for (i = start; (i + count - 1) <= end; i += (j + 1)) {
        for (j = 0; j < count; j++) {
            if (GH2_VXLAN_VP_USED_GET(unit, (i + j))) {
                break;
            }
        }
        if (j == count) {
            break;
        }
    }

    if ((i + count - 1) <= end) {
        *base_vp = i;
        for (j = 0; j < count; j++) {
            GH2_VXLAN_VP_USED_SET(unit, i + j);
            if (vxlan_vp_info.flags & GH2_VXLAN_VP_INFO_NETWORK_PORT) {
                GH2_VXLAN_NETWORK_VP_USED_SET(unit, (i + j));
            } else if (vxlan_vp_info.flags & GH2_VXLAN_VP_INFO_LOOPBACK_PORT) {
                GH2_VXLAN_LOOPBACK_VP_USED_SET(unit, (i + j));
            }
        }
        return BCM_E_NONE;
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vp_free
 * Purpose:
 *    Internal function for freeing a group of VPs
 * Parameters:
 *    unit  - (IN) Device Number
 *    count   -  (IN) How many consecutive VPs to free
 *    base_vp -  (IN) Base VP index
 * Returns:
 *    BCM_X_XXX
 */
STATIC int
bcmi_gh2_vxlan_vp_free(int unit, int count, int base_vp)
{
    int i;

    for (i = 0; i < count; i++) {
        GH2_VXLAN_VP_USED_CLR(unit, (base_vp + i));
        GH2_VXLAN_NETWORK_VP_USED_CLR(unit, (base_vp + i));
        /* Loopback VP is only support when VXLAN RIOT is enabled */
        if (GH2_VXLAN_RIOT_ENABLE(unit)) {
            GH2_VXLAN_LOOPBACK_VP_USED_CLR(unit, (base_vp + i));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vp_used_set
 * Purpose:
 *    Mark the VP as used
 * Parameters:
 *    unit - (IN) Device Number
 *    vp   - (IN) VP
 *    vxlan_vp_info - (IN) VXLAN VP info
 * Returns:
 *    Boolean
 */
STATIC int
bcmi_gh2_vxlan_vp_used_set(
    int unit,
    int vp,
    bcmi_vxlan_vp_info_t vxlan_vp_info)
{
    if (vxlan_vp_info.flags & GH2_VXLAN_VP_INFO_SHARED_PORT) {
        return BCM_E_UNAVAIL;
    }

    GH2_VXLAN_VP_USED_SET(unit, vp);

    if (vxlan_vp_info.flags & GH2_VXLAN_VP_INFO_NETWORK_PORT) {
        GH2_VXLAN_NETWORK_VP_USED_SET(unit, vp);
    } else {
        GH2_VXLAN_NETWORK_VP_USED_CLR(unit, vp);
    }

    /* Loopback VP is only support when VXLAN RIOT is enabled */
    if (GH2_VXLAN_RIOT_ENABLE(unit)) {
        if (vxlan_vp_info.flags & GH2_VXLAN_VP_INFO_LOOPBACK_PORT) {
            GH2_VXLAN_LOOPBACK_VP_USED_SET(unit, vp);
        } else {
            GH2_VXLAN_LOOPBACK_VP_USED_CLR(unit, vp);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vp_used_get
 * Purpose:
 *    Check whether a VP is used or not
 * Parameters:
 *    unit - (IN) Device Number
 *    vp   - (IN) VP
 * Returns:
 *    Boolean
 */
int
bcmi_gh2_vxlan_vp_used_get(int unit, int vp)
{
    int rv = BCM_E_NONE;
    int num_vp;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info = GH2_VXLAN_INFO(unit);

    num_vp = vxlan_info->num_vxlan_vp;

    /* Check Valid range of VFI */
    if ((vp < 0) || (vp >= num_vp)) {
         return rv;
    }

    rv = GH2_VXLAN_VP_USED_GET(unit, vp);

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vp_info_get
 * Purpose:
 *    Get the VP Type of a VP, given its Id
 * Parameters:
 *    unit  - (IN) Device Number
 *    vp   -  (IN) VP
 *    vp_info - (OUT) VP info structure
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_vp_info_get(int unit, int vp, bcmi_vxlan_vp_info_t *vp_info)
{
    if (NULL == vp_info) {
        return BCM_E_PARAM;
    }

    bcmi_gh2_vxlan_vp_info_init(vp_info);

    if (bcmi_gh2_vxlan_vp_used_get(unit, vp)) {
        if (GH2_VXLAN_NETWORK_VP_USED_GET(unit, vp)) {
            vp_info->flags |= GH2_VXLAN_VP_INFO_NETWORK_PORT;
        } else {
            /* Loopback VP is only support when VXLAN RIOT is enabled */
            if (GH2_VXLAN_RIOT_ENABLE(unit)) {
                if (GH2_VXLAN_LOOPBACK_VP_USED_GET(unit, vp)) {
                    vp_info->flags |= GH2_VXLAN_VP_INFO_LOOPBACK_PORT;
                }
            }
        }

        return BCM_E_NONE;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_match_count_adjust
 * Purpose:
 *    Obtain ref-count for a VXLAN port
 * Parameters:
 *    unit - (IN) Device Number
 *    vp   - (IN) VP
 *    step - (IN) number for rer-count increment
 * Returns:
 *    None
 */

STATIC void
bcmi_gh2_vxlan_port_match_count_adjust(int unit, int vp, int step)
{
    int num_vp;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info = GH2_VXLAN_INFO(unit);

    num_vp = vxlan_info->num_vxlan_vp;

    /* Check Valid range of VFI */
    if ((vp < 0) || (vp >= num_vp)) {
         return;
    }

    vxlan_info->match_key[vp].match_count += step;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_cleanup
 * Purpose:
 *    DeInit VXLAN software module
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_cleanup(int unit)
{
    int rv = BCM_E_UNAVAIL;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    vxlan_info = GH2_VXLAN_INFO(unit);

    if (vxlan_info->initialized == FALSE) {
        return BCM_E_NONE;
    }

    rv = bcmi_gh2_vxlan_lock(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (0 == SOC_HW_ACCESS_DISABLE(unit)) {
        rv = bcmi_gh2_vxlan_hw_clear(unit);
    }

    bcmi_gh2_vxlan_unlock(unit);

    /* Mark the state as uninitialized */
    vxlan_info->initialized = FALSE;

    /* Free software resources */
    (void)bcmi_gh2_vxlan_free_resource(unit);

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_header_init
 * Purpose:
 *    Initialize the VXLAN header info
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_header_init(int unit)
{
    uint32 mem_entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;

    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        sal_memset(mem_entry, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));

        /*
         * Ingress VXLAN class selection:
         * only support one class (VXLAN_FLAGS = 0x08, others are all zero)
         * the VXLAN_CLASS_ID hit will be one condition of VXLAN-LITE decap
         */
        /* ING_VXLAN_CLASS_ID_TCAMm */
        mem = ING_VXLAN_CLASS_ID_TCAMm;
        sal_memset(mem_entry, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, 0, mem_entry));
        soc_mem_field32_set(unit, mem, mem_entry, VALIDf, 0x1);
        soc_mem_field32_set(unit, mem, mem_entry, VXLAN_FLAGSf, 0x8);
        soc_mem_field32_set(unit, mem, mem_entry, VXLAN_FLAGS_MASKf, 0xff);
        /* Write buffer to hw. */
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, mem_entry));

        /* ING_VXLAN_CLASS_ID_POLICY_TABLE */
        mem = ING_VXLAN_CLASS_ID_POLICY_TABLEm;
        sal_memset(mem_entry, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, 0, mem_entry));
        soc_mem_field32_set(unit, mem, mem_entry, VALIDf, 0x1);
        soc_mem_field32_set(unit, mem, mem_entry, CLASS_IDf, 0x0);
        /* Write buffer to hw. */
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, mem_entry));

        /* Egress VXLAN header */
        mem = EGR_VXLAN_HEADERm;
        sal_memset(mem_entry, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, 0, mem_entry));
        soc_mem_field32_set(unit, mem, mem_entry, VXLAN_FLAGSf, 0x8);
        /* Write buffer to hw. */
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, mem_entry));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_init
 * Purpose:
 *    Initialize the VXLAN software module
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_init(int unit)
{
    int rv = BCM_E_NONE;
    int i, num_tnl = 0, num_vp = 0, num_vfi = 0, num_tpid = 0;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    int riot_enable = 0;

    if (!GH2_L3_INFO(unit)->l3_initialized) {
        LOG_ERROR(BSL_LS_BCM_VXLAN,
                  (BSL_META_U(unit,
                              "L3 module must be initialized" \
                              "prior to VXLAN Init\n")));
        return BCM_E_CONFIG;
    }

    /* VXLAN-Lite RIOT is only support for GH2-B0 */
    riot_enable = soc_property_get(unit, spn_RIOT_ENABLE, 0);
    if (riot_enable && !soc_feature(unit, soc_feature_vxlan_lite_riot)) {
        LOG_ERROR(BSL_LS_BCM_VXLAN,
                  (BSL_META_U(unit,
                              "VXLAN RIOT is not supported\n")));
        return BCM_E_CONFIG;
    }

    /* Global initialization flag setup */
    if (gh2_vxlan_initialized == FALSE) {
        for (i = 0; i < BCM_MAX_NUM_UNITS; i++) {
            bcmi_gh2_vxlan_bk_info[i] = NULL;
        }
        gh2_vxlan_initialized = TRUE;
    }

    /* Allocate BK Info */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_allocate_bk(unit));
    vxlan_info = GH2_VXLAN_INFO(unit);

    /*
     * Allocate Resources
     */
    if (vxlan_info->initialized) {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_cleanup(unit));
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_allocate_bk(unit));
        vxlan_info = GH2_VXLAN_INFO(unit);
    }

    num_vp = soc_mem_index_count(unit, VLAN_XLATEm);

    /* VXLAN Virtual Port store */
    BCMI_VXLAN_ALLOC(vxlan_info->match_key,
                     sizeof(_bcm_vxlan_match_port_info_t) * num_vp,
                     "match_key");
    if (vxlan_info->match_key == NULL) {
        bcmi_gh2_vxlan_free_resource(unit);
        return BCM_E_MEMORY;
    }

    /* Stay same after recover */
    for (i = 0; i < num_vp; i++) {
        bcmi_gh2_vxlan_match_clear(unit, i);
    }

    num_tnl = soc_mem_index_count(unit, BCM_XGS3_L3_MEM(unit, tnl_init_v4));

    /* Create EGR_IP_TUNNEL usage bitmap */
    BCMI_VXLAN_ALLOC(vxlan_info->vxlan_ip_tnl_bitmap,
                     SHR_BITALLOCSIZE(num_tnl), "vxlan_ip_tnl_bitmap");
    if (vxlan_info->vxlan_ip_tnl_bitmap == NULL) {
        bcmi_gh2_vxlan_free_resource(unit);
        return BCM_E_MEMORY;
    }

    /* Create VXLAN protection mutex. */
    vxlan_info->vxlan_mutex = sal_mutex_create("vxlan_mutex");
    if (!vxlan_info->vxlan_mutex) {
         bcmi_gh2_vxlan_free_resource(unit);
         return BCM_E_MEMORY;
    }

    /* VXLAN tunnel termination store */
    BCMI_VXLAN_ALLOC(vxlan_info->vxlan_tunnel_term,
                     sizeof(bcmi_gh2_vxlan_tunnel_endpoint_t) * num_tnl,
                     "vxlan tunnel term store");
    if (vxlan_info->vxlan_tunnel_term == NULL) {
        bcmi_gh2_vxlan_free_resource(unit);
        return BCM_E_MEMORY;
    }

    /* VXLAN tunnel initialization store */
    BCMI_VXLAN_ALLOC(vxlan_info->vxlan_tunnel_init,
                     sizeof(bcmi_gh2_vxlan_tunnel_endpoint_t) * num_tnl,
                     "vxlan tunnel init store");
    if (vxlan_info->vxlan_tunnel_init == NULL) {
        bcmi_gh2_vxlan_free_resource(unit);
        return BCM_E_MEMORY;
    }

    vxlan_info->num_vxlan_tnl = num_tnl;

    if (soc_feature(unit, soc_feature_vxlan_lite_riot)) {
        /* GH2-B0 support hardware VFI table */
        num_vfi = soc_mem_index_count(unit, VFIm);
    } else {
        /* GH2-A0 support software VFI table (depends on VLAN_XLATEm) */
        num_vfi = soc_mem_index_count(unit, VLAN_XLATEm);
    }

    /* VXLAN VFI bitmap initialization store */
    BCMI_VXLAN_ALLOC(vxlan_info->vxlan_vfi_bitmap,
                     SHR_BITALLOCSIZE(num_vfi), "vxlan_vfi_bitmap");
    if (vxlan_info->vxlan_vfi_bitmap == NULL) {
        bcmi_gh2_vxlan_free_resource(unit);
        return BCM_E_MEMORY;
    }

    /* VXLAN VFI info initialization store */
    BCMI_VXLAN_ALLOC(vxlan_info->vxlan_vpn_info,
                     sizeof(gh2_vxlan_vpn_info_t) * num_vfi, "vxlan_vpn_info");
    if (vxlan_info->vxlan_vpn_info == NULL) {
        bcmi_gh2_vxlan_free_resource(unit);
        return BCM_E_MEMORY;
    }
    vxlan_info->num_vxlan_vfi = num_vfi;

    /* VXLAN Virtual Port bitmap initialization store */
    BCMI_VXLAN_ALLOC(vxlan_info->vxlan_vp_bitmap,
                     SHR_BITALLOCSIZE(num_vp), "vxlan_vp_bitmap");
    if (vxlan_info->vxlan_vp_bitmap == NULL) {
        bcmi_gh2_vxlan_free_resource(unit);
        return BCM_E_MEMORY;
    }

    /* Network VXLAN Virtual Port bitmap initialization store */
    BCMI_VXLAN_ALLOC(vxlan_info->vxlan_network_vp_bitmap,
                     SHR_BITALLOCSIZE(num_vp), "vxlan_network_vp_bitmap");
    if (vxlan_info->vxlan_network_vp_bitmap == NULL) {
        bcmi_gh2_vxlan_free_resource(unit);
        return BCM_E_MEMORY;
    }

    /* VXLAN Outer TPID info initialization store */
    num_tpid = soc_reg_field_length(unit, ING_VXLAN_CONTROLr,
                                    PAYLOAD_OUTER_TPID_ENABLEf);
    BCMI_VXLAN_ALLOC(vxlan_info->vxlan_outer_tpid,
                     sizeof(gh2_vxlan_outer_tpid_info_t) * num_tpid,
                     "vxlan_outer_tpid");
    if (vxlan_info->vxlan_outer_tpid == NULL) {
        bcmi_gh2_vxlan_free_resource(unit);
        return BCM_E_MEMORY;
    }
    vxlan_info->num_vxlan_outer_tpid = num_tpid;

    /* VXLAN Ingress VLAN XLATE info initialization store */
    BCMI_VXLAN_ALLOC(vxlan_info->vxlan_vlan_xlate,
                     sizeof(gh2_vxlan_vlan_xlate_info_t) * num_vfi,
                     "vxlan_vlan_xlate");
    if (vxlan_info->vxlan_vlan_xlate == NULL) {
        bcmi_gh2_vxlan_free_resource(unit);
        return BCM_E_MEMORY;
    }

    /* VXLAN VP info initialization store */
    BCMI_VXLAN_ALLOC(vxlan_info->vxlan_vp_info,
                     sizeof(gh2_vxlan_vp_info_t) * num_vp, "vxlan_vp_info");
    if (vxlan_info->vxlan_vp_info == NULL) {
        bcmi_gh2_vxlan_free_resource(unit);
        return BCM_E_MEMORY;
    }
    vxlan_info->num_vxlan_vp = num_vp;

    /* RIOT is Enabled/Disabled on this device (for GH2-B0 only) */
    vxlan_info->riot_enable = riot_enable;

    /*
     * Loopback VXLAN Virtual Port bitmap initialization store
     * (used for RIOT)
     */
    BCMI_VXLAN_ALLOC(vxlan_info->vxlan_loopback_vp_bitmap,
                     SHR_BITALLOCSIZE(num_vp), "vxlan_loopback_vp_bitmap");
    if (vxlan_info->vxlan_loopback_vp_bitmap == NULL) {
        bcmi_gh2_vxlan_free_resource(unit);
        return BCM_E_MEMORY;
    }

    /* Reference count of Custom Header index0 initialization store */
    vxlan_info->riot_custom_header_index0_ref_count = 0;

    /* Initialize the VXLAN header info */
    rv = bcmi_gh2_vxlan_header_init(unit);
    if (BCM_FAILURE(rv)) {
        bcmi_gh2_vxlan_free_resource(unit);
        return rv;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = bcmi_gh2_vxlan_reinit(unit);
    } else {
        rv = bcmi_gh2_vxlan_wb_alloc(unit);
    }
    /* clean up the bookkeeping resource if any errors */
    if (BCM_FAILURE(rv)) {
        bcmi_gh2_vxlan_free_resource(unit);
        return rv;
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* Mark the state as initialized */
    vxlan_info->initialized = TRUE;

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vpn_is_valid
 * Purpose:
 *    Find if given VXLAN VPN is Valid
 * Parameters:
 *    unit  - (IN) Device Number
 *    l2vpn - (IN) VXLAN VPN
 * Returns:
 *    BCM_E_XXXX
 */
STATIC int
bcmi_gh2_vxlan_vpn_is_valid(int unit, bcm_vpn_t l2vpn)
{
    bcm_vpn_t vxlan_vpn_min = 0;
    int vfi_index = -1, num_vfi = 0;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    num_vfi = vxlan_info->num_vxlan_vfi;

    /* Check for Valid vpn */
    _BCM_VXLAN_VPN_SET(vxlan_vpn_min, _BCM_VPN_TYPE_VFI, 0);
    if (l2vpn < vxlan_vpn_min || l2vpn > (vxlan_vpn_min + num_vfi - 1)) {
        return BCM_E_PARAM;
    }

    _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, l2vpn);

    if (!bcmi_gh2_vxlan_vfi_used_get(unit, vfi_index)) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vpn_is_eline
 * Purpose:
 *    Find if given VXLAN VPN is ELINE
 * Parameters:
 *    unit  - (IN) Device Number
 *    l2vpn   - VXLAN VPN
 *    is_eline  - Flag
 * Returns:
 *    BCM_E_XXXX
 */
STATIC int
bcmi_gh2_vxlan_vpn_is_eline(int unit, bcm_vpn_t l2vpn, uint8 *is_eline)
{
    /* Input parameter check */
    if (NULL == is_eline) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_vpn_is_valid(unit, l2vpn));

    /* Currently, VXLAN-LITE only support ELAN */
    *is_eline = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vp_is_eline
 * Purpose:
 *    Find if given VXLAN VP is ELINE
 * Parameters:
 *    unit     - (IN) Device Number
 *    vp       - (IN) VXLAN VP
 *    is_eline - (IN) Flag
 * Returns:
 *    BCM_E_XXXX
 * Assumes:
 *    l2vpn is valid
 */
STATIC int
bcmi_gh2_vxlan_vp_is_eline( int unit, int vp, uint8 *is_eline)
{
    /* Input parameter check */
    if (NULL == is_eline) {
        return BCM_E_PARAM;
    }
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!bcmi_gh2_vxlan_vp_used_get(unit, vp)) {
        return BCM_E_NOT_FOUND;
    }

   *is_eline = 0x0;

   return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_match_vnid_entry_reset
 * Purpose:
 *    Reset VXLAN Match VNID Entry
 * Parameters:
 *    unit  - (IN) Device Number
 *    IN :  Vnid
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_match_vnid_entry_reset(int unit, uint32 vnid)
{
    soc_mem_t mem;
    uint32 vxlate_entry[SOC_MAX_MEM_WORDS];

    sal_memset(vxlate_entry, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));

    mem = VLAN_XLATEm;

    soc_mem_field32_set(unit, mem, vxlate_entry, VALIDf, 0x1);
    soc_mem_field32_set(unit, mem, vxlate_entry, XLATE__VNIDf, vnid);
    /* incoming port not used in lookup key, set field to all 1's */
    soc_mem_field32_set(unit, mem, vxlate_entry, XLATE__GLPf,
                        SOC_VLAN_XLATE_GLP_WILDCARD(unit));
    soc_mem_field32_set(unit, mem, vxlate_entry, KEY_TYPEf,
                        GH2_VLXLT_HASH_KEY_TYPE_VNID);

    BCM_IF_ERROR_RETURN(
        soc_mem_delete(unit, mem, MEM_BLOCK_ALL, vxlate_entry));

    return BCM_E_NONE;
}

 /*
 * Function:
 *    bcmi_gh2_vxlan_vfi_to_vpn_get
 * Purpose:
 *    Get L2-VPN instance for VXLAN
 * Parameters:
 *    unit - (IN) Device Number
 *    vfi_index - (IN) vfi_index
 *    vid  - (OUT) VLAN Id (l2vpn id)
 * Returns:
 *    BCM_E_XXXX
 */
int
bcmi_gh2_vxlan_vfi_to_vpn_get(int unit, int vfi_index, bcm_vlan_t *vid)
{
    /* Input parameter check */
    if (NULL == vid) {
        return BCM_E_PARAM;
    }

    if (!bcmi_gh2_vxlan_vfi_used_get(unit, vfi_index)) {
        return BCM_E_NOT_FOUND;
    }

    _BCM_VXLAN_VPN_SET(*vid, _BCM_VXLAN_VPN_TYPE_ELAN, vfi_index);

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vpn_create
 * Purpose:
 *    Create a VPN instance
 * Parameters:
 *    unit - (IN) Device Number
 *    info - (IN/OUT) VPN configuration info
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_vpn_create(int unit, bcm_vxlan_vpn_config_t *info)
{
    int rv, rv2;
    int vfi_index = -1;
    uint32 vnid = 0;
    uint8 vpn_alloc_flag = 0;
    bcm_vlan_protocol_packet_ctrl_t protocol_pkt;
    soc_mem_t mem;
    uint32 vxlate_entry[SOC_MAX_MEM_WORDS];
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vpn_info_t *vfi_info;
    vfi_entry_t vfi_entry;
    int uuc_umc_bc_group = 0;
    int uuc_umc_bc_group_type = 0;

    /* Input parameter check */
    if (NULL == info) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    sal_memset(vxlate_entry, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));

    /* Allocate VFI */
    if (info->flags & BCM_VXLAN_VPN_REPLACE) {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_vpn_is_valid(unit, info->vpn));
        _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, info->vpn);
    } else if (info->flags & BCM_VXLAN_VPN_WITH_ID) {
        rv = bcmi_gh2_vxlan_vpn_is_valid(unit, info->vpn);
        if (rv == BCM_E_NONE) {
            return BCM_E_EXISTS;
        } else if (rv != BCM_E_NOT_FOUND) {
            return rv;
        }
        _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, info->vpn);
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_vfi_alloc_with_id(unit, vfi_index));
        vpn_alloc_flag = 1;
    } else {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_vfi_alloc(unit, &vfi_index));
        _BCM_VXLAN_VPN_SET(info->vpn, _BCM_VPN_TYPE_VFI, vfi_index);
        vpn_alloc_flag = 1;
    }

    /* Initial and configure VFI */
    if (info->flags & BCM_VXLAN_VPN_ELINE) {
        /* Currently, VXLAN-LITE doesn't support ELINE on VPN */
        return BCM_E_UNAVAIL;
    } else if (info->flags & BCM_VXLAN_VPN_ELAN) {
        /* Configure unknown UC/MC/BC packets forwarding on the VFI */
        if (soc_feature(unit, soc_feature_vxlan_lite_riot)) {
            /* Only support for GH2-B0 with new VFI table */

            /* Check that the groups are valid */
            /* Unknown UC/MC/BC group share the same field for GH2-B0 */
            if ((info->unknown_unicast_group !=
                info->unknown_multicast_group) ||
                (info->unknown_multicast_group !=
                info->broadcast_group)) {
                if (vpn_alloc_flag) {
                    rv = bcmi_gh2_vxlan_vfi_free(unit, vfi_index);
                    if (BCM_FAILURE(rv)) {
                        return rv;
                    }
                }
                return BCM_E_PARAM;
            }

            /* Select one of groups (suppose they should be the same) */
            if (info->unknown_unicast_group != 0) {
                uuc_umc_bc_group_type =
                    _BCM_MULTICAST_TYPE_GET(info->unknown_unicast_group);
                uuc_umc_bc_group =
                    _BCM_MULTICAST_ID_GET(info->unknown_unicast_group);

                if ((uuc_umc_bc_group_type != _BCM_MULTICAST_TYPE_VXLAN) ||
                    (uuc_umc_bc_group >= soc_mem_index_count(unit,
                                                             L3_IPMCm))) {
                    if (vpn_alloc_flag) {
                        rv = bcmi_gh2_vxlan_vfi_free(unit, vfi_index);
                        if (BCM_FAILURE(rv)) {
                            return rv;
                        }
                    }
                    return BCM_E_PARAM;
                }

                /* Commit the entry to HW */
                rv = soc_mem_read(unit, VFIm, MEM_BLOCK_ANY,
                                  vfi_index, &vfi_entry);
                if (SOC_FAILURE(rv)) {
                    if (vpn_alloc_flag) {
                        rv = bcmi_gh2_vxlan_vfi_free(unit, vfi_index);
                        if (BCM_FAILURE(rv)) {
                            return rv;
                        }
                    }
                    return rv;
                }
                soc_mem_field32_set(unit, VFIm, &vfi_entry,
                                    UUC_UMC_BC_INDEXf, uuc_umc_bc_group);
                rv = soc_mem_write(unit, VFIm, MEM_BLOCK_ALL,
                                   vfi_index, &vfi_entry);
                if (SOC_FAILURE(rv)) {
                    if (vpn_alloc_flag) {
                        rv = bcmi_gh2_vxlan_vfi_free(unit, vfi_index);
                        if (BCM_FAILURE(rv)) {
                            return rv;
                        }
                    }
                    return rv;
                }
            }
        } else {
            /* Not support for GH2-A0 (no VFI table) */
            if ((info->broadcast_group != 0) ||
                (info->unknown_multicast_group != 0) ||
                (info->unknown_unicast_group != 0)) {
                if (vpn_alloc_flag) {
                    rv = bcmi_gh2_vxlan_vfi_free(unit, vfi_index);
                    if (BCM_FAILURE(rv)) {
                        return rv;
                    }
                }
                return BCM_E_PARAM;
            }
        }
    }

    /* Configure protocol packet control (not support for VXLAN-LITE) */
    sal_memset(&protocol_pkt, 0, sizeof(protocol_pkt));
    if (0 != sal_memcmp(&(info->protocol_pkt), &protocol_pkt,
                        sizeof(bcm_vlan_protocol_packet_ctrl_t))) {
        if (vpn_alloc_flag) {
            rv = bcmi_gh2_vxlan_vfi_free(unit, vfi_index);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
        return BCM_E_UNAVAIL;
    }

    if (info->flags & BCM_VXLAN_VPN_WITH_VPNID) {
        /* VXLAN-LITE doesn't support BCM_VXLAN_VPN_SERVICE_XXX */
        if ((info->flags & BCM_VXLAN_VPN_SERVICE_TAGGED) ||
            (info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_ADD) ||
            (info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_REPLACE) ||
            (info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_DELETE) ||
            (info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_TPID_REPLACE) ||
            (info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_PRI_TPID_REPLACE) ||
            (info->flags & BCM_VXLAN_VPN_SERVICE_VLAN_PRI_REPLACE) ||
            (info->flags & BCM_VXLAN_VPN_SERVICE_PRI_REPLACE) ||
            (info->flags & BCM_VXLAN_VPN_SERVICE_TPID_REPLACE)) {
            if (vpn_alloc_flag) {
                rv = bcmi_gh2_vxlan_vfi_free(unit, vfi_index);
                if (BCM_FAILURE(rv)) {
                    return rv;
                }
            }
            return BCM_E_UNAVAIL;
        }

        /* Valid check for VNID value */
        if ((info->vnid > 0xFFFFFF) ||
            (info->flags & BCM_VXLAN_VPN_TUNNEL_BASED_VNID)) {
            if (vpn_alloc_flag) {
                rv = bcmi_gh2_vxlan_vfi_free(unit, vfi_index);
                if (BCM_FAILURE(rv)) {
                    return rv;
                }
            }
            return BCM_E_PARAM;
        }

        /* Configure Ingress VNID (use VLAN_XLATE) */
        mem = VLAN_XLATEm;

        soc_mem_field32_set(unit, mem, vxlate_entry, VALIDf, 0x1);
        soc_mem_field32_set(unit, mem, vxlate_entry, XLATE__MPLS_ACTIONf, 0x5);
        soc_mem_field32_set(unit, mem, vxlate_entry,
                            XLATE__DISABLE_VLAN_CHECKSf, 0x1);
        soc_mem_field32_set(unit, mem, vxlate_entry, XLATE__VFI_VALIDf, 0x1);
        soc_mem_field32_set(unit, mem, vxlate_entry, XLATE__VFIf, vfi_index);

        soc_mem_field32_set(unit, mem, vxlate_entry, KEY_TYPEf,
                            GH2_VLXLT_HASH_KEY_TYPE_VNID);
        /* incoming port not used in lookup key, set field to all 1's */
        soc_mem_field32_set(unit, mem, vxlate_entry, XLATE__GLPf,
                            SOC_VLAN_XLATE_GLP_WILDCARD(unit));

        vfi_info = &(vxlan_info->vxlan_vpn_info[vfi_index]);
        if (info->flags & BCM_VXLAN_VPN_REPLACE) {
            /* Delete old match vnid entry */
            vnid = vfi_info->vnid;
            if (vfi_info->valid) {
                if (info->vnid == vnid) {
                    /* Return when the vnid is not changed */
                    return BCM_E_NONE;
                }

                soc_mem_field32_set(unit, mem, vxlate_entry,
                                    XLATE__VNIDf, vnid);
                rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, vxlate_entry);
                if (BCM_FAILURE(rv)) {
                    if (vpn_alloc_flag) {
                        rv2 = bcmi_gh2_vxlan_vfi_free(unit, vfi_index);
                        if (BCM_FAILURE(rv2)) {
                            return rv2;
                        }
                    }
                    return rv;
                }
            }
        }
        soc_mem_field32_set(unit, mem, vxlate_entry,
                            XLATE__VNIDf, info->vnid);
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, vxlate_entry);
        if (BCM_FAILURE(rv)) {
            if (vpn_alloc_flag) {
                rv2 = bcmi_gh2_vxlan_vfi_free(unit, vfi_index);
                if (BCM_FAILURE(rv2)) {
                    return rv2;
                }
            }
            return rv;
        }

        /* Update vnid for VXLAN VFI */
        vfi_info->valid = 1;
        vfi_info->vnid = info->vnid;

        /* Configure Egress VNID (use EGR_L3_NEXT_HOPm) */
        /* Will be programmed with bcm_vxlan_port_add */
    } else {
        if (info->flags & BCM_VXLAN_VPN_TUNNEL_BASED_VNID) {
            if (vpn_alloc_flag) {
                rv = bcmi_gh2_vxlan_vfi_free(unit, vfi_index);
                if (BCM_FAILURE(rv)) {
                    return rv;
                }
            }
            return BCM_E_PARAM;
        }

        /* Reset match vnid table */
        vfi_info = &(vxlan_info->vxlan_vpn_info[vfi_index]);
        if (vfi_info->valid) {
            vnid = vfi_info->vnid;
            /* Delete vxlate_entry */
            rv = bcmi_gh2_vxlan_match_vnid_entry_reset(unit, vnid);
            if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                return rv;
            }

            /* Update vnid for VXLAN VFI */
            vfi_info->valid = 0;
            vfi_info->vnid = 0;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vpn_destroy
 * Purpose:
 *    Delete a VPN instance
 * Parameters:
 *    unit  - (IN) Device Number
 *    l2vpn - (IN) VPN instance ID
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_vpn_destroy(int unit, bcm_vpn_t l2vpn)
{
    int rv = 0;
    int vfi_index = 0;
    uint8 is_eline = 0;
    uint32 vnid = 0;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vpn_info_t *vfi_info;

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    /* Get vfi index */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_vpn_is_eline(unit, l2vpn, &is_eline));
    if (is_eline != 0x0) {
        /* VXLAN-LITE doesn't support ELINE */
        return BCM_E_INTERNAL;
    } else {
        _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, l2vpn);
    }

    /* Reset match vnid table */
    vfi_info = &(vxlan_info->vxlan_vpn_info[vfi_index]);
    if (vfi_info->valid) {
        vnid = vfi_info->vnid;
        /* Delete vxlate_entry */
        rv = bcmi_gh2_vxlan_match_vnid_entry_reset(unit, vnid);
        if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        }

        /* Update vnid for VXLAN VFI */
        vfi_info->valid = 0;
        vfi_info->vnid = 0;
    }

    /* Delete all VXLAN ports on this VPN */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_port_delete_all(unit, l2vpn));

    /* Reset VFI table */
    rv = bcmi_gh2_vxlan_vfi_free(unit, vfi_index);

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vpn_id_destroy_all
 * Purpose:
 *    Delete all L2-VPN instances
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXXX
 */
int
bcmi_gh2_vxlan_vpn_destroy_all(int unit)
{
    int num_vfi = 0, idx = 0;
    bcm_vpn_t l2vpn = 0;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    /* Destroy all VXLAN VPNs */
    num_vfi = vxlan_info->num_vxlan_vfi;
    for (idx = 0; idx < num_vfi; idx++) {
        if (bcmi_gh2_vxlan_vfi_used_get(unit, idx)) {
            _BCM_VXLAN_VPN_SET(l2vpn, _BCM_VPN_TYPE_VFI, idx);
            BCM_IF_ERROR_RETURN(
                bcmi_gh2_vxlan_vpn_destroy(unit, l2vpn));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vpn_get
 * Purpose:
 *    Get L2-VPN instance
 * Parameters:
 *    unit  - (IN) Device Number
 *    l2vpn - (IN) VXLAN VPN
 *    info  - (OUT) VXLAN VPN Config
 * Returns:
 *    BCM_E_XXXX
 */
int
bcmi_gh2_vxlan_vpn_get(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_vxlan_vpn_config_t *info)
{
    int vfi_index = -1;
    uint8 is_eline = 0;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vpn_info_t *vfi_info;
    vfi_entry_t vfi_entry;
    int uuc_umc_bc_group = 0;

    /* Input parameter check */
    if (NULL == info) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    /* Get vfi table */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_vpn_is_eline(unit, l2vpn, &is_eline));
    if (is_eline != 0x0) {
        /* VXLAN-LITE doesn't support ELINE */
        return BCM_E_INTERNAL;
    } else {
        _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VXLAN_VPN_TYPE_ELAN, l2vpn);
        /* Get info associated with vfi table */
        info->flags =  BCM_VXLAN_VPN_ELAN;

        if (soc_feature(unit, soc_feature_vxlan_lite_riot)) {
            /* Only support for GH2-B0 with new VFI table */
            SOC_IF_ERROR_RETURN(
                soc_mem_read(unit, VFIm, MEM_BLOCK_ANY,
                             vfi_index, &vfi_entry));

            /* Unknown UC/MC/BC group share the same field for GH2-B0 */
            uuc_umc_bc_group =
                soc_mem_field32_get(unit, VFIm, &vfi_entry, UUC_UMC_BC_INDEXf);
            if (uuc_umc_bc_group != 0) {
                _BCM_MULTICAST_GROUP_SET(info->broadcast_group,
                                         _BCM_MULTICAST_TYPE_VXLAN,
                                         uuc_umc_bc_group);
                _BCM_MULTICAST_GROUP_SET(info->unknown_unicast_group,
                                         _BCM_MULTICAST_TYPE_VXLAN,
                                         uuc_umc_bc_group);
                _BCM_MULTICAST_GROUP_SET(info->unknown_multicast_group,
                                         _BCM_MULTICAST_TYPE_VXLAN,
                                         uuc_umc_bc_group);
            }
        }
    }

    _BCM_VXLAN_VPN_SET(info->vpn, _BCM_VPN_TYPE_VFI, vfi_index);

    /* Get VNID */
    vfi_info = &(vxlan_info->vxlan_vpn_info[vfi_index]);
    if (vfi_info->valid) {
        info->flags |= BCM_VXLAN_VPN_WITH_VPNID;
        info->vnid = vfi_info->vnid;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vpn_traverse
 * Purpose:
 *    Get information about a VPN instance
 * Parameters:
 *    unit - (IN) Device Number
 *    cb   - (IN)  User-provided callback
 *    user_data  - (IN/OUT) callback user data
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_vpn_traverse(
    int unit,
    bcm_vxlan_vpn_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_NONE;
    int idx, num_vfi;
    int vpn;
    bcm_vxlan_vpn_config_t info;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Input parameter check */
    if (NULL == cb) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    num_vfi = vxlan_info->num_vxlan_vfi;
    for (idx = 0; idx < num_vfi; idx++) {
        if (bcmi_gh2_vxlan_vfi_used_get(unit, idx)) {
            _BCM_VXLAN_VPN_SET(vpn, _BCM_VPN_TYPE_VFI, idx);
            bcm_vxlan_vpn_config_t_init(&info);
            rv = bcmi_gh2_vxlan_vpn_get(unit, vpn, &info);
            if (BCM_SUCCESS(rv)) {
                BCM_IF_ERROR_RETURN(
                    cb(unit, &info, user_data));
            }
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_ip6_is_zero
 * Purpose:
 *    Helper function to check input IPv6 address is zero or not
 * Parameters:
 *    ip6  - (IN) IPv6 address
 * Returns:
 *    TRUE: IPv6 address is all zero, FALSE: IPv6 address is not zero
 */
STATIC int
bcmi_gh2_vxlan_tunnel_ip6_is_zero(bcm_ip6_t ip6)
{
    bcm_ip6_t ip6_zero;
    int is_zero = FALSE;

    sal_memset(ip6_zero, 0, BCM_IP6_ADDRLEN);

    if (sal_memcmp(ip6, ip6_zero, BCM_IP6_ADDRLEN) == 0) {
        is_zero = TRUE;
    }

    return is_zero;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_terminator_create
 * Purpose:
 *    Set VXLAN Tunnel terminator
 * Parameters:
 *    unit - (IN) Device Number
 *    tnl_info - (IN) Tunnel terminator info data structure
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_tunnel_terminator_create(
    int unit,
    bcm_tunnel_terminator_t *tnl_info)
{
    int tunnel_idx = -1;
    bcm_ip6_t tnl_ip6;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Input parameter check */
    if (NULL == tnl_info) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    /* Input Tunnel Type check */
    if (tnl_info->type == bcmTunnelTypeVxlan6) {
        /* Tunnel for IPv6 */
        if (bcmi_gh2_vxlan_tunnel_ip6_is_zero(tnl_info->dip6) ||
            bcmi_gh2_vxlan_tunnel_ip6_is_zero(tnl_info->sip6)) {
            return BCM_E_PARAM;
        }
    } else if (tnl_info->type == bcmTunnelTypeVxlan) {
        /* Tunnel for IPv4 */
        if ((tnl_info->dip == 0) || (tnl_info->sip == 0)) {
            return BCM_E_PARAM;
        }
    } else {
        /* Others are not supported */
        return BCM_E_PARAM;
    }

    /* GH2 doesn't support below tunnel flags */
    if (tnl_info->flags & BCM_TUNNEL_TERM_UDP_CHECKSUM_ENABLE ||
        tnl_info->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP ||
        tnl_info->flags & BCM_TUNNEL_TERM_USE_OUTER_PCP) {
        return BCM_E_PARAM;
    }

    /* Program tunnel id */
    if (tnl_info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) {
        if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
            return BCM_E_PARAM;
        }

        tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);
        GH2_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tunnel_idx);
    } else {
        /* Only BCM_TUNNEL_TERM_TUNNEL_WITH_ID supported */
        /* Use tunnel_id which is got by _create */
        return BCM_E_PARAM;
    }

    if (tnl_info->type == bcmTunnelTypeVxlan6) {
        /* Tunnel for IPv6 */
        sal_memcpy(tnl_ip6,
                   vxlan_info->vxlan_tunnel_term[tunnel_idx].dip6,
                   BCM_IP6_ADDRLEN);
        if (!bcmi_gh2_vxlan_tunnel_ip6_is_zero(tnl_ip6)) {
            /*
             * Entry exists with the same idx no matter the dip6
             * is the same or not
             */
            return BCM_E_EXISTS;
        }
    } else {
        /* Tunnel for IPv4 */
        if (vxlan_info->vxlan_tunnel_term[tunnel_idx].dip != 0) {
            /*
             * Entry exists with the same idx no matter the dip
             * is the same or not
             */
            return BCM_E_EXISTS;
        }
    }

    sal_memset(&(vxlan_info->vxlan_tunnel_term[tunnel_idx]),
               0, sizeof(bcmi_gh2_vxlan_tunnel_endpoint_t));
    if (tunnel_idx != -1) {
        if (tnl_info->type == bcmTunnelTypeVxlan6) {
            /* Tunnel for IPv6 */
            sal_memcpy(vxlan_info->vxlan_tunnel_term[tunnel_idx].sip6,
                       tnl_info->sip6, BCM_IP6_ADDRLEN);
            sal_memcpy(vxlan_info->vxlan_tunnel_term[tunnel_idx].dip6,
                       tnl_info->dip6, BCM_IP6_ADDRLEN);
        } else {
            /* Tunnel for IPv4 */
            vxlan_info->vxlan_tunnel_term[tunnel_idx].sip = tnl_info->sip;
            vxlan_info->vxlan_tunnel_term[tunnel_idx].dip = tnl_info->dip;
        }
        vxlan_info->vxlan_tunnel_term[tunnel_idx].vlan = tnl_info->vlan;
        vxlan_info->vxlan_tunnel_term[tunnel_idx].tunnel_state =
            _BCM_VXLAN_TUNNEL_TERM_MULTICAST_LEAF;
        vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag |=
            _BCM_VXLAN_TUNNEL_TERM_ENABLE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_terminator_update
 * Purpose:
 *    Update VXLAN Tunnel terminator multicast state
 * Parameters:
 *    unit - (IN) Device Number
 *    tnl_info - (IN) Tunnel terminator info data structure
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_tunnel_terminator_update(
    int unit,
    bcm_tunnel_terminator_t *tnl_info)
{
    int tunnel_idx = -1;
    bcm_ip_t tunnel_dip = 0;
    bcm_ip6_t tunnel_dip6;
    uint16 tunnel_multicast_state = 0;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Input parameter check */
    if (NULL == tnl_info) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    /* Program tunnel id */
    if (tnl_info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) {
        if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
            return BCM_E_PARAM;
        }
        tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);
        GH2_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tunnel_idx);
    } else {
        /* Only BCM_TUNNEL_TERM_TUNNEL_WITH_ID supported */
        /* Use tunnel_id which is got by _create */
        return BCM_E_PARAM;
    }

    tunnel_dip = vxlan_info->vxlan_tunnel_term[tunnel_idx].dip;
    sal_memcpy(tunnel_dip6,
               vxlan_info->vxlan_tunnel_term[tunnel_idx].dip6,
               BCM_IP6_ADDRLEN);

    if (tnl_info->flags & BCM_TUNNEL_TERM_TUNNEL_DEACTIVATE) {
        vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag &=
            (~_BCM_VXLAN_TUNNEL_TERM_ENABLE);
        return BCM_E_NONE;
    } else if (!(vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag &
        _BCM_VXLAN_TUNNEL_TERM_ENABLE)) {
        if ((tunnel_dip != 0) ||
            (!bcmi_gh2_vxlan_tunnel_ip6_is_zero(tunnel_dip6))) {
            /*
             * Below tunnel flags are not supported for GH2:
             * - _BCM_VXLAN_TUNNEL_TERM_UDP_CHECKSUM_ENABLE
             * - _BCM_VXLAN_TUNNEL_TERM_USE_OUTER_DSCP
             * - _BCM_VXLAN_TUNNEL_TERM_USE_OUTER_PCP
             */
            vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag |=
                _BCM_VXLAN_TUNNEL_TERM_ENABLE;
        }
    }

    /* Set (SIP, DIP) based Software State */
    tunnel_multicast_state =
        vxlan_info->vxlan_tunnel_term[tunnel_idx].tunnel_state;

    if (tnl_info->multicast_flag ==
        BCM_VXLAN_MULTICAST_TUNNEL_STATE_BUD_DISABLE) {
        if (tunnel_multicast_state != _BCM_VXLAN_TUNNEL_TERM_MULTICAST_LEAF) {
             vxlan_info->vxlan_tunnel_term[tunnel_idx].tunnel_state =
                 _BCM_VXLAN_TUNNEL_TERM_MULTICAST_LEAF;
        }
    } else {
        /* GH2 doesn't support BUD node */
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_terminate_destroy
 * Purpose:
 *    Destroy VXLAN tunnel_terminate Entry
 * Parameters:
 *    unit - (IN) Device Number
 *    vxlan_tunnel_id - (IN) vxlan_tunnel_id
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_tunnel_terminator_destroy(
    int unit,
    bcm_gport_t vxlan_tunnel_id)
{
    int tunnel_idx = -1;
    bcm_ip_t tunnel_dip = 0;
    bcm_ip6_t tunnel_dip6;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    /* Input parameter check */
    if (!BCM_GPORT_IS_TUNNEL(vxlan_tunnel_id)) {
        return BCM_E_PARAM;
    }

    tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(vxlan_tunnel_id);
    GH2_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tunnel_idx);

    tunnel_dip = vxlan_info->vxlan_tunnel_term[tunnel_idx].dip;
    sal_memcpy(tunnel_dip6,
               vxlan_info->vxlan_tunnel_term[tunnel_idx].dip6,
               BCM_IP6_ADDRLEN);

    if ((tunnel_dip == 0) &&
        (bcmi_gh2_vxlan_tunnel_ip6_is_zero(tunnel_dip6))) {
        return BCM_E_NOT_FOUND;
    }

    if (!(_BCM_VXLAN_TUNNEL_TERM_ENABLE &
        vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag)) {
        /* Deactivated tunnel terminator can not be destroyed */
        return BCM_E_DISABLED;
    }

    /* Remove Tunnel term-entry for given DIP from Software based */
    if (tunnel_idx != -1) {
            vxlan_info->vxlan_tunnel_term[tunnel_idx].dip = 0;
            sal_memset(vxlan_info->vxlan_tunnel_term[tunnel_idx].dip6,
                       0, BCM_IP6_ADDRLEN);
            vxlan_info->vxlan_tunnel_term[tunnel_idx].vlan = 0;
            vxlan_info->vxlan_tunnel_term[tunnel_idx].tunnel_state = 0;
            vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_terminator_destroy_all
 * Purpose:
 *    Destroy all incoming VXLAN tunnel
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_tunnel_terminator_destroy_all(int unit)
{
    int rv = BCM_E_NONE;
    int tnl_idx = 0, num_tnl = 0;
    bcm_gport_t vxlan_tunnel_id;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    /* Remove all Tunnel term-entry from Software based */
    num_tnl = vxlan_info->num_vxlan_tnl;
    for (tnl_idx = 0; tnl_idx < num_tnl; tnl_idx++) {
        BCM_GPORT_TUNNEL_ID_SET(vxlan_tunnel_id, tnl_idx);
        rv = bcmi_gh2_vxlan_tunnel_terminator_destroy(unit, vxlan_tunnel_id);
        if (BCM_FAILURE(rv)) {
            if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_DISABLED)) {
                return rv;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_terminator_get
 * Purpose:
 *    Get VXLAN tunnel_terminate Entry
 * Parameters:
 *    unit - (IN) Device Number
 *    info - (IN/OUT) Tunnel terminator structure
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_tunnel_terminator_get(
    int unit,
    bcm_tunnel_terminator_t *tnl_info)
{
    int tunnel_idx = -1;
    bcm_ip_t tunnel_dip = 0;
    bcm_ip6_t tunnel_dip6;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Input parameter check */
    if (NULL == tnl_info) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    /* Program tunnel id */
    if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
        return BCM_E_PARAM;
    }

    tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);
    GH2_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tunnel_idx);

    tunnel_dip = vxlan_info->vxlan_tunnel_term[tunnel_idx].dip;
    sal_memcpy(tunnel_dip6,
               vxlan_info->vxlan_tunnel_term[tunnel_idx].dip6,
               BCM_IP6_ADDRLEN);

    if (tunnel_dip != 0) {
        /* Tunnel for IPv4 */
        tnl_info->sip = vxlan_info->vxlan_tunnel_term[tunnel_idx].sip;
        tnl_info->dip = vxlan_info->vxlan_tunnel_term[tunnel_idx].dip;
        tnl_info->type = bcmTunnelTypeVxlan;
    } else if (!bcmi_gh2_vxlan_tunnel_ip6_is_zero(tunnel_dip6)) {
        /* Tunnel for IPv6 */
        sal_memcpy(tnl_info->sip6,
                   vxlan_info->vxlan_tunnel_term[tunnel_idx].sip6,
                   BCM_IP6_ADDRLEN);
        sal_memcpy(tnl_info->dip6,
                   vxlan_info->vxlan_tunnel_term[tunnel_idx].dip6,
                   BCM_IP6_ADDRLEN);
        tnl_info->type = bcmTunnelTypeVxlan6;
    } else {
        /* not exist */
        return BCM_E_NOT_FOUND;
    }

    /* De-activated */
    if (!(vxlan_info->vxlan_tunnel_term[tunnel_idx].activate_flag &
        _BCM_VXLAN_TUNNEL_TERM_ENABLE)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_TUNNEL_DEACTIVATE;
    }

    tnl_info->vlan = vxlan_info->vxlan_tunnel_term[tunnel_idx].vlan;
    if (vxlan_info->vxlan_tunnel_term[tunnel_idx].tunnel_state ==
        _BCM_VXLAN_TUNNEL_TERM_MULTICAST_LEAF) {
        tnl_info->multicast_flag =
            BCM_VXLAN_MULTICAST_TUNNEL_STATE_BUD_DISABLE;
    } else {
        tnl_info->multicast_flag = 0;
    }

    /*
     * Below tunnel flags are not supported for GH2:
     * - BCM_TUNNEL_TERM_UDP_CHECKSUM_ENABLE
     * - BCM_TUNNEL_TERM_USE_OUTER_DSCP
     * - BCM_TUNNEL_TERM_USE_OUTER_PCP
     */

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_terminator_traverse
 * Purpose:
 *    Traverse VXLAN tunnel terminator entries
 * Parameters:
 *    unit - (IN) Device Number
 *    cb   - (IN) User callback function, called once per entry
 *    user_data - (IN) User supplied cookie used in callback function
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_tunnel_terminator_traverse(
    int unit,
    bcm_tunnel_terminator_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_NONE;
    int num_tnl = 0, idx = 0;
    bcm_tunnel_terminator_t info;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Input parameter check */
    if (NULL == cb) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    num_tnl = vxlan_info->num_vxlan_tnl;

    /* Iterate over all the entries - search valid ones */
    for (idx = 0; idx < num_tnl; idx++) {
        /* Check a valid entry */
        if ((vxlan_info->vxlan_tunnel_term[idx].dip) ||
            (vxlan_info->vxlan_tunnel_term[idx].sip) ||
            (!bcmi_gh2_vxlan_tunnel_ip6_is_zero(
            vxlan_info->vxlan_tunnel_term[idx].dip6)) ||
            (!bcmi_gh2_vxlan_tunnel_ip6_is_zero(
            vxlan_info->vxlan_tunnel_term[idx].sip6))) {
            /* Reset buffer before read */
            bcm_tunnel_terminator_t_init(&info);

            BCM_GPORT_TUNNEL_ID_SET(info.tunnel_id, idx);
            rv = bcmi_gh2_vxlan_tunnel_terminator_get(unit, &info);

            /* search failure */
            if (BCM_FAILURE(rv)) {
                if (rv != BCM_E_NOT_FOUND) {
                    break;
                }
                continue;
            }

            rv = (*cb)(unit, &info, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                break;
            }
#endif
        }
    }

    /* Reset last read status */
    if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_initiator_add
 * Purpose:
 *    Add tunnel initiator entry to hw.
 * Parameters:
 *    unit - (IN) Device Number
 *    idx  - (IN) Index to insert tunnel initiator.
 *    info - (IN) Tunnel initiator entry info.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_tunnel_initiator_add(
    int unit,
    int idx,
    bcm_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to write hw entry */
    soc_mem_t mem; /* Tunnel initiator table memory */
    int ipv6; /* IPv6 tunnel initiator */
    int df_val = 0; /* Don't fragment encoding */
    int hw_map_idx = 0;
    int entry_type = 0;
    int val;
    uint16 random;
    egr_fragment_id_table_entry_t entry;

    /* Input parameters check */
    if (NULL == info) {
        return BCM_E_PARAM;
    }

    ipv6 = _BCM_TUNNEL_OUTER_HEADER_IPV6(info->type);

    /* Get table memory. */
    if (ipv6) {
        /* Tunnel for IPv6 */
        idx >>= 1;
        mem = BCM_XGS3_L3_MEM(unit, tnl_init_v6);
        entry_type = BCM_XGS3_TUNNEL_INIT_V6;
    } else {
        /* Tunnel for IPv4 */
        mem = BCM_XGS3_L3_MEM(unit, tnl_init_v4);
        entry_type = BCM_XGS3_TUNNEL_INIT_V4;
    }

    /* Zero write buffer. */
    sal_memset(tnl_entry, 0, sizeof(tnl_entry));

    /*
     * If replacing existing entry, first read the entry to get old
     * profile pointer and TPID
     */
    if (info->flags & BCM_TUNNEL_REPLACE) {
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, tnl_entry));
    }

    if (info->type == bcmTunnelTypeVxlan6) {
        /* Tunnel for IPv6 */
        /* Set source address within Tunnel Table */
        soc_mem_ip6_addr_set(unit, mem, tnl_entry, SIPf, info->sip6,
                             SOC_MEM_IP6_FULL_ADDR);

        /* Set destination address within Tunnel Table */
        soc_mem_ip6_addr_set(unit, mem, tnl_entry, DIPf, info->dip6,
                             SOC_MEM_IP6_FULL_ADDR);

        /* Set FLOW_LABEL. */
        soc_mem_field32_set(unit, mem, tnl_entry,
                            FLOW_LABELf, info->flow_label);

        /* Set HOP_LIMIT. */
        soc_mem_field32_set(unit, mem, tnl_entry, HOP_LIMITf, info->ttl);
    } else {
        /* Tunnel for IPv4 */
        /* Set source address within Tunnel Table */
        soc_mem_field_set(unit, mem, tnl_entry, SIPf, (uint32 *)&info->sip);

        /* Set destination address within Tunnel Table */
        soc_mem_field_set(unit, mem, tnl_entry, DIPf, (uint32 *)&info->dip);

        /* IP tunnel hdr DF bit for IPv4. */
        df_val = 0;
        if (info->flags & BCM_TUNNEL_INIT_USE_INNER_DF) {
            df_val |= 0x2;
        } else if (info->flags & BCM_TUNNEL_INIT_IPV4_SET_DF) {
            df_val |= 0x1;
        }
        soc_mem_field32_set(unit, mem, tnl_entry, IPV4_DF_SELf, df_val);

        /* IP tunnel hdr DF bit for IPv6. */
        if (info->flags & BCM_TUNNEL_INIT_IPV6_SET_DF) {
            soc_mem_field32_set(unit, mem, tnl_entry, IPV6_DF_SELf, 1);
        }

        /* Set TTL. */
        soc_mem_field32_set(unit, mem, tnl_entry, TTLf, info->ttl);
    }

    /* Tunnel header DSCP select. */
    soc_mem_field32_set(unit, mem, tnl_entry, DSCP_SELf, info->dscp_sel);

    if (info->dscp_sel == bcmTunnelDscpMap) {
        /* Set DSCP_MAP value. */
        if (SOC_MEM_FIELD_VALID(unit, mem, DSCP_MAPPING_PTRf)) {
            BCM_IF_ERROR_RETURN(
                _bcm_tr2_qos_id2idx(unit, info->dscp_map, &hw_map_idx));
            soc_mem_field32_set(unit, mem, tnl_entry,
                                DSCP_MAPPING_PTRf, hw_map_idx);
        } else {
            return BCM_E_PARAM;
        }
    } else if (info->dscp_sel == bcmTunnelDscpAssign) {
        /* Set DSCP value. */
        if (SOC_MEM_FIELD_VALID(unit, mem, DSCPf)) {
            soc_mem_field32_set(unit, mem, tnl_entry,
                                DSCPf, info->dscp);
        } else {
            return BCM_E_PARAM;
        }
    }

    /* Set UDP Dest Port */
    soc_mem_field32_set(unit, mem, tnl_entry,
                        L4_DST_PORTf, info->udp_dst_port);

    /* Set UDP Src Port */
    soc_mem_field32_set(unit, mem, tnl_entry,
                        L4_SRC_PORTf, info->udp_src_port);

    /* Set Tunnel type = VXLAN */
    soc_mem_field32_set(unit, mem, tnl_entry,
                        TUNNEL_TYPEf, _BCM_VXLAN_TUNNEL_TYPE);

    /*
      * Set entry type:
      * IPv4: BCM_XGS3_TUNNEL_INIT_V4 = 0x1
      * IPv6: BCM_XGS3_TUNNEL_INIT_V6 = 0x2
      */
    soc_mem_field32_set(unit, mem, tnl_entry, ENTRY_TYPEf, entry_type);

    if ((info->flags & BCM_TUNNEL_INIT_IP4_ID_SET_FIXED) ||
        (info->flags & BCM_TUNNEL_INIT_IP4_ID_SET_RANDOM)) {
        if (info->type != bcmTunnelTypeVxlan) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit,
                                       bcmSwitchTunnelIp4IdShared, &val));
        if (val == 0) {
            sal_memset(&entry, 0, sizeof(egr_fragment_id_table_entry_t));

            if (info->flags & BCM_TUNNEL_INIT_IP4_ID_SET_FIXED) {
                soc_EGR_FRAGMENT_ID_TABLEm_field32_set(unit, &entry,
                                                       FRAGMENT_IDf,
                                                       info->ip4_id);
            } else if (info->flags & BCM_TUNNEL_INIT_IP4_ID_SET_RANDOM) {
                random = (uint16)(sal_time_usecs() & 0xFFFF);
                soc_EGR_FRAGMENT_ID_TABLEm_field32_set(unit, &entry,
                                                       FRAGMENT_IDf, random);
            }
            BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, EGR_FRAGMENT_ID_TABLEm,
                              SOC_BLOCK_ALL, idx, &entry));
        }
    }

    /* Write buffer to hw */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, tnl_entry);

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tnl_init_del
 * Purpose:
 *    Delete tunnels initiator entry. Entry kept if reference count
 *    not equals to 1 (Other tunnels reference the same entry).
 * Parameters:
 *    unit      - (IN) Device Number
 *    flags     - (IN) Table shared flags
 *    info      - (IN) The informaton of tunnel initiator
 *    ent_index - (IN) Entry index
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_tnl_init_del(
    int unit,
    uint32 flags,
    bcm_tunnel_initiator_t *info,
    int ent_index)
{
    _bcm_l3_tbl_op_t data; /* Delete operation info. */

    /* Initialization */
    sal_memset(&data, 0, sizeof(_bcm_l3_tbl_op_t));

    /* Check hw call is defined. */
    if (!BCM_XGS3_L3_HWCALL_CHECK(unit, tnl_init_del)) {
        return BCM_E_UNAVAIL;
    }

    if(_BCM_EGR_IP_TUNNEL_DOUBLE_WIDE_ENTRY(unit, info->type)) {
        /* IPv6 tunnel is double table entry operation (occupy 2 slots). */
        data.width = _BCM_DOUBLE_WIDE;
    } else {
        /* IPv4 tunnel is single table entry operation.*/
        data.width = _BCM_SINGLE_WIDE;
    }

    data.tbl_ptr =  BCM_XGS3_L3_TBL_PTR(unit, tnl_init);
    data.entry_index = ent_index;
    data.oper_flags = flags;
    data.delete_func = BCM_XGS3_L3_HWCALL(unit, tnl_init_del);

    /* Delete tunnel initiator table entry. */
    return _bcm_xgs3_tbl_del(unit, &data);
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_initiator_entry_add
 * Purpose:
 *    Configure VXLAN Tunnel initiator hardware Entry
 * Parameters:
 *    unit  - (IN) Device Number
 *    flags - (IN) the flags of tunnel initiator
 *    info  - (IN) the informaton of tunnel initiator
 *    tnl_idx - (OUT) Tunnel id for tunnel initiator
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_tunnel_initiator_entry_add(
    int unit,
    uint32 flags,
    bcm_tunnel_initiator_t *info,
    int *tnl_idx)
{
    int rv = BCM_E_NONE;

    /* Input parameters check */
    if ((NULL == info) || (NULL == tnl_idx)) {
        return BCM_E_PARAM;
    }

    rv = bcm_xgs3_tnl_init_add(unit, flags, info, tnl_idx);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Write the entry to HW */
    rv = bcmi_gh2_vxlan_tunnel_initiator_add(unit, *tnl_idx, info);
    if (BCM_FAILURE(rv)) {
        flags = _BCM_L3_SHR_WRITE_DISABLE;
        (void)bcmi_gh2_vxlan_tnl_init_del(unit, flags, info, *tnl_idx);
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_initiator_get_tunnel_id
 * Purpose:
 *    Get an available tunnel id for vxlan tunnel initiator
 * Parameters:
 *    unit - (IN) Device Number
 *    info - (IN) the informaton of tunnel initiator
 *    hw_match_flag - (IN) Indicate if an existing EGR_IP_TUNNEL entry
 *    soft_tnl_idx  - (OUT) Tunnel id for tunnel initiator
 *    new_id_flag   - (OUT) Indicate if a new free tunnel id
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_tunnel_initiator_get_tunnel_id(
    int unit,
    bcm_tunnel_initiator_t *info,
    int hw_match_flag,
    int *soft_tnl_idx,
    int *new_id_flag)
{
    int rv = BCM_E_NONE;
    int idx = 0, num_tnl = 0;
    int free_index = -1, free_entry_found = 0;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    if ((NULL == info) || (NULL == soft_tnl_idx) || (NULL == new_id_flag)) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    num_tnl = vxlan_info->num_vxlan_tnl;
    *new_id_flag = FALSE;

    for (idx = 0; idx < num_tnl; idx++) {
        if (info->type == bcmTunnelTypeVxlan6) {
            /* Tunnel for IPv6 */
            if (((sal_memcmp(vxlan_info->vxlan_tunnel_init[idx].sip6,
                             info->sip6, BCM_IP6_ADDRLEN) == 0)) &&
                ((sal_memcmp(vxlan_info->vxlan_tunnel_init[idx].dip6,
                             info->dip6, BCM_IP6_ADDRLEN) == 0)) &&
                (vxlan_info->vxlan_tunnel_init[idx].tunnel_state ==
                             info->udp_dst_port)) {
                /* Obtain existed index */
                *soft_tnl_idx = idx;
                return BCM_E_NONE;
            }
        } else {
            /* Tunnel for IPv4 */
            if ((vxlan_info->vxlan_tunnel_init[idx].sip == info->sip) &&
                (vxlan_info->vxlan_tunnel_init[idx].dip == info->dip) &&
                (vxlan_info->vxlan_tunnel_init[idx].tunnel_state ==
                info->udp_dst_port)) {
                /* Obtain existed index */
                *soft_tnl_idx = idx;
                return BCM_E_NONE;
            }
        }

        if ((vxlan_info->vxlan_tunnel_init[idx].sip == 0) &&
            (bcmi_gh2_vxlan_tunnel_ip6_is_zero(
            vxlan_info->vxlan_tunnel_init[idx].sip6)) &&
            !free_entry_found) {
            /* Obtain Free index */
            free_index = idx;
            free_entry_found = 1;
            if (!hw_match_flag) {
                break;
            }
        }
    }

    if (free_entry_found) {
        *new_id_flag = TRUE;
        *soft_tnl_idx = free_index;
    } else {
        /* No free resource */
        return BCM_E_RESOURCE;
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_initiator_idx_translate
 * Purpose:
 *    Translate the index of software object to the index
 *    of hw entry for tunnel initiator
 * Parameters:
 *    unit - (IN) Device Number
 *    soft_idx - (IN) The index of software object
 *    hw_idx   - (OUT) The index of hw entry
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_tunnel_initiator_idx_translate(
    int unit,
    int soft_idx,
    int *hw_idx)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t init_tnl_info;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Input parameter check */
    if (NULL == hw_idx) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    bcm_tunnel_initiator_t_init(&init_tnl_info);

    if (vxlan_info->vxlan_tunnel_init[soft_idx].sip != 0) {
        /* Tunnel for IPv4 */
        init_tnl_info.type = bcmTunnelTypeVxlan;
        init_tnl_info.sip = vxlan_info->vxlan_tunnel_init[soft_idx].sip;
    } else {
        /* Tunnel for IPv6 */
        init_tnl_info.type = bcmTunnelTypeVxlan6;
        sal_memcpy(init_tnl_info.sip6,
                   vxlan_info->vxlan_tunnel_init[soft_idx].sip6,
                   BCM_IP6_ADDRLEN);
    }

    init_tnl_info.udp_dst_port =
        vxlan_info->vxlan_tunnel_init[soft_idx].tunnel_state;

    rv = _bcm_xgs3_l3_tnl_init_find(unit, &init_tnl_info, hw_idx);

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_initiator_cmp
 * Purpose:
 *    Routine compares vxlan tunnel initiator entry with entry in the chip
 *    with specified index.
 * Parameters:
 *    unit - (IN) Device Number
 *    buf  - (IN) Tunnel initiator entry to compare.
 *    index- (IN) Entry index in the chip to compare.
 *    cmp_result - (OUT) Compare result.
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_tunnel_initiator_cmp(
    int unit,
    void *buf,
    int index,
    int *cmp_result)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t hw_entry;   /* Entry read from hw */
    bcm_tunnel_initiator_t *tnl_entry; /* Api passed buffer */

    if ((NULL == buf) || (NULL == cmp_result)) {
        return BCM_E_PARAM;
    }

    *cmp_result = BCM_L3_CMP_NOT_EQUAL;

    /* Copy buffer to structure */
    tnl_entry = (bcm_tunnel_initiator_t *)buf;

    /* Initialization */
    bcm_tunnel_initiator_t_init(&hw_entry);

    /* Get tunnel initiator entry from hw */
    if (BCM_XGS3_L3_HWCALL_CHECK(unit, tnl_init_get)) {
        BCM_XGS3_L3_MODULE_LOCK(unit);
        rv = (BCM_XGS3_L3_HWCALL_EXEC
            (unit, tnl_init_get)(unit, index, &hw_entry));
        BCM_XGS3_L3_MODULE_UNLOCK(unit);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        return BCM_E_UNAVAIL;
    }

    /* Compare source ip */
    if ((tnl_entry->sip != hw_entry.sip) ||
        (sal_memcmp(tnl_entry->sip6, hw_entry.sip6,
                    BCM_IP6_ADDRLEN) != 0)) {
        return BCM_E_NONE;
    }

    /* Compare udp dst port */
    if (tnl_entry->udp_dst_port != hw_entry.udp_dst_port) {
        return BCM_E_NONE;
    }

    *cmp_result = BCM_L3_CMP_EQUAL;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_initiator_hash_calc
 * Purpose:
 *    Calculate tunnel initiator entry hash (signature).
 * Parameters:
 *    unit - (IN) Device Number
 *    buf  - (IN) Tunnel initiator entry information.
 *    hash - (OUT) Hash (signature) calculated value.
 * Returns:
 *    BCM_X_XXX
 */
int
bcmi_gh2_vxlan_tunnel_initiator_hash_calc(int unit, void *buf, uint16 *hash)
{
    bcm_tunnel_initiator_t tnl_entry;

    if ((NULL == buf) || (NULL == hash)) {
        return (BCM_E_PARAM);
    }

    bcm_tunnel_initiator_t_init(&tnl_entry);
    tnl_entry.type = ((bcm_tunnel_initiator_t *)buf)->type;

    /* Set source ip */
    if (tnl_entry.type == bcmTunnelTypeVxlan6) {
        /* Tunnel for IPv6 */
        sal_memcpy(tnl_entry.sip6,
                   ((bcm_tunnel_initiator_t *)buf)->sip6, BCM_IP6_ADDRLEN);
    } else {
        /* Tunnel for IPv4 */
        tnl_entry.sip = ((bcm_tunnel_initiator_t *)buf)->sip;
    }

    tnl_entry.udp_dst_port = ((bcm_tunnel_initiator_t *)buf)->udp_dst_port;

    /* Calculate hash */
    *hash = _shr_crc16(0, (uint8 *)&tnl_entry, sizeof(bcm_tunnel_initiator_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_initiator_create
 * Purpose:
 *    Set VXLAN Tunnel initiator
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_tunnel_initiator_create(int unit, bcm_tunnel_initiator_t *info)
{
    int tnl_idx = -1, ref_count = 0;
    uint32 flags = 0;
    int idx = 0, num_tnl = 0;
    uint8 match_entry_present = 0;
    int soft_tnl_idx = -1, tunnel_idx = -1;
    int new_idx_flag = FALSE;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Input parameter check */
    if (NULL == info) {
        return BCM_E_PARAM;
    }

    /* Input Tunnel Type check */
    if (info->type == bcmTunnelTypeVxlan6) {
        /* Tunnel for IPv6 */
        if (bcmi_gh2_vxlan_tunnel_ip6_is_zero(info->dip6) ||
            bcmi_gh2_vxlan_tunnel_ip6_is_zero(info->sip6)) {
            return BCM_E_PARAM;
        }

        /* IPv6 doesn't support DSCP value. */
        if (info->dscp_sel == bcmTunnelDscpAssign) {
            return BCM_E_PARAM;
        }
    } else if (info->type == bcmTunnelTypeVxlan) {
        /* Tunnel for IPv4 */
        if ((info->dip == 0) || (info->sip == 0)) {
            return BCM_E_PARAM;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Input TTL valid range check */
    if (!BCM_TTL_VALID(info->ttl)) {
        return BCM_E_PARAM;
    }
    /* Input DSCP selection valid range check */
    if (info->dscp_sel > bcmTunnelDscpMap || info->dscp_sel < 0) {
        return BCM_E_PARAM;
    }
    /* Validate IP tunnel DSCP value. */
    if (info->dscp > 63 || info->dscp < 0) {
        return BCM_E_PARAM;
    }
    /* Validate flow label value. */
    if (_BCM_TUNNEL_OUTER_HEADER_IPV6(info->type)) {
        if (info->flow_label >= (1 << 20)) {
            return BCM_E_PARAM;
        }
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    num_tnl = vxlan_info->num_vxlan_tnl;
    if (info->flags & BCM_TUNNEL_REPLACE) {
        tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
        GH2_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tunnel_idx);
        if (info->type == bcmTunnelTypeVxlan6) {
            /* Tunnel for IPv6 */
            if ((sal_memcmp(info->sip6,
                            vxlan_info->vxlan_tunnel_init[tunnel_idx].sip6,
                            BCM_IP6_ADDRLEN) != 0) ||
                (sal_memcmp(info->dip6,
                            vxlan_info->vxlan_tunnel_init[tunnel_idx].dip6,
                            BCM_IP6_ADDRLEN) != 0) ||
                (info->udp_dst_port !=
                    vxlan_info->vxlan_tunnel_init[tunnel_idx].tunnel_state)) {
                return BCM_E_UNAVAIL;
            }
        } else {
            /* Tunnel for IPv4 */
            if ((info->sip !=
                    vxlan_info->vxlan_tunnel_init[tunnel_idx].sip) ||
                (info->udp_dst_port !=
                    vxlan_info->vxlan_tunnel_init[tunnel_idx].tunnel_state) ||
                (info->dip !=
                    vxlan_info->vxlan_tunnel_init[tunnel_idx].dip)) {
                return BCM_E_UNAVAIL;
            }
        }
    }

    /* Allocate either existing or new tunnel initiator entry */
    flags = _BCM_L3_SHR_MATCH_DISABLE |
            _BCM_L3_SHR_WRITE_DISABLE |
            _BCM_L3_SHR_SKIP_INDEX_ZERO;

    if (_bcm_xgs3_l3_tnl_init_find(unit, info, &tnl_idx) == BCM_E_NONE) {
        match_entry_present = 1;
    }

    /* Get an available tunnel id */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_tunnel_initiator_get_tunnel_id(
            unit, info, match_entry_present, &soft_tnl_idx, &new_idx_flag));

    if (match_entry_present) {
        if (info->flags & BCM_TUNNEL_REPLACE) {
            if (new_idx_flag) {
                return BCM_E_UNAVAIL;
            }

            /* Check for Ref-count in (SIP, multi-DIP) situation */
            for (idx = 0; idx < num_tnl; idx++) {
                if (info->type == bcmTunnelTypeVxlan6) {
                    /* Tunnel for IPv6 */
                    if ((sal_memcmp(info->sip6,
                                    vxlan_info->vxlan_tunnel_init[idx].sip6,
                                    BCM_IP6_ADDRLEN) == 0) &&
                        (info->udp_dst_port ==
                        vxlan_info->vxlan_tunnel_init[idx].tunnel_state)) {
                        if (++ref_count > 1) {
                            return BCM_E_RESOURCE;
                        }
                    }
                } else {
                    /* Tunnel for IPv4 */
                    if ((info->sip ==
                        vxlan_info->vxlan_tunnel_init[idx].sip) &&
                        (info->udp_dst_port ==
                        vxlan_info->vxlan_tunnel_init[idx].tunnel_state)) {
                        if (++ref_count > 1) {
                            return BCM_E_RESOURCE;
                        }
                    }
                }
            }

            flags |= _BCM_L3_SHR_UPDATE | _BCM_L3_SHR_WITH_ID;
            BCM_IF_ERROR_RETURN(
                bcmi_gh2_vxlan_tunnel_initiator_entry_add(unit, flags,
                                                          info, &tnl_idx));
        }
    } else {
         /* Parse tunnel replace flag */
         if (info->flags & BCM_TUNNEL_REPLACE) {
             /* replace an inexistent tunnel initiator */
             return BCM_E_NOT_FOUND;
         }
         BCM_IF_ERROR_RETURN(
             bcmi_gh2_vxlan_tunnel_initiator_entry_add(unit, flags,
                                                       info, &tnl_idx));
    }

    if (soft_tnl_idx != -1) {
        if (info->type == bcmTunnelTypeVxlan6) {
            /* Tunnel for IPv6 */
            sal_memcpy(vxlan_info->vxlan_tunnel_init[soft_tnl_idx].sip6,
                       info->sip6, BCM_IP6_ADDRLEN);
            sal_memcpy(vxlan_info->vxlan_tunnel_init[soft_tnl_idx].dip6,
                       info->dip6, BCM_IP6_ADDRLEN);
        } else {
            /* Tunnel for IPv4 */
            vxlan_info->vxlan_tunnel_init[soft_tnl_idx].sip = info->sip;
            vxlan_info->vxlan_tunnel_init[soft_tnl_idx].dip = info->dip;
        }
        vxlan_info->vxlan_tunnel_init[soft_tnl_idx].tunnel_state =
            info->udp_dst_port;
        BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, soft_tnl_idx);
    } else {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_initiator_destroy
 * Purpose:
 *    Destroy outgoing VXLAN tunnel
 * Parameters:
 *    unit - (IN) Device Number
 *    vxlan_tunnel_id - (IN) Tunnel ID (Gport)
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_tunnel_initiator_destroy(
    int unit,
    bcm_gport_t vxlan_tunnel_id)
{
    int rv = BCM_E_NONE;
    int tnl_idx = -1, temp_tnl_idx = -1, ref_count = 0, idx = 0;
    uint32 flags = 0;
    int num_tnl = 0;
    bcm_ip_t sip = 0;       /* Tunnel header SIP (IPv4). */
    bcm_ip6_t sip6;         /* Tunnel header SIP (IPv6). */
    uint16 udp_dst_port;    /* Destination UDP port */
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Input parameters check */
    if (!BCM_GPORT_IS_TUNNEL(vxlan_tunnel_id)) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    tnl_idx = BCM_GPORT_TUNNEL_ID_GET(vxlan_tunnel_id);
    GH2_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tnl_idx);

    /* Tunnel for IPv4/IPv6 */
    sip = vxlan_info->vxlan_tunnel_init[tnl_idx].sip;
    sal_memcpy(sip6, vxlan_info->vxlan_tunnel_init[tnl_idx].sip6,
               BCM_IP6_ADDRLEN);
    udp_dst_port =  vxlan_info->vxlan_tunnel_init[tnl_idx].tunnel_state;

    if ((sip == 0) && (bcmi_gh2_vxlan_tunnel_ip6_is_zero(sip6))) {
        return BCM_E_NOT_FOUND;
    }

    /*
     * Remove HW TNL entry only if all matching SIP, UDP_DEST_PORT
     * gets removed
     */
    num_tnl = vxlan_info->num_vxlan_tnl;
    for (idx = 0; idx < num_tnl; idx++) {
        if (sip != 0) {
            /* Tunnel for IPv4 */
            if ((vxlan_info->vxlan_tunnel_init[idx].sip == sip) &&
                (vxlan_info->vxlan_tunnel_init[idx].tunnel_state ==
                udp_dst_port)) {
                ref_count++;
            }
        } else {
            /* Tunnel for IPv6 */
            if ((sal_memcmp(vxlan_info->vxlan_tunnel_init[idx].sip6,
                            sip6, BCM_IP6_ADDRLEN) == 0) &&
                (vxlan_info->vxlan_tunnel_init[idx].tunnel_state ==
                udp_dst_port)) {
                ref_count++;
            }
        }
    }

    if (ref_count == 1) {
        /* Get the entry first */
        temp_tnl_idx = tnl_idx;
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_tunnel_initiator_idx_translate(unit, temp_tnl_idx,
                                                          &tnl_idx));
        /* Destroy the tunnel entry */
        (void)bcm_xgs3_tnl_init_del(unit, flags, tnl_idx);
        vxlan_info->vxlan_tunnel_init[temp_tnl_idx].sip = 0;
        vxlan_info->vxlan_tunnel_init[temp_tnl_idx].dip = 0;
        sal_memset(vxlan_info->vxlan_tunnel_init[temp_tnl_idx].sip6,
                   0, BCM_IP6_ADDRLEN);
        sal_memset(vxlan_info->vxlan_tunnel_init[temp_tnl_idx].dip6,
                   0, BCM_IP6_ADDRLEN);
        vxlan_info->vxlan_tunnel_init[temp_tnl_idx].tunnel_state = 0;
    } else if (ref_count > 1) {
        vxlan_info->vxlan_tunnel_init[tnl_idx].sip = 0;
        vxlan_info->vxlan_tunnel_init[tnl_idx].dip = 0;
        sal_memset(vxlan_info->vxlan_tunnel_init[tnl_idx].sip6,
                   0, BCM_IP6_ADDRLEN);
        sal_memset(vxlan_info->vxlan_tunnel_init[tnl_idx].dip6,
                   0, BCM_IP6_ADDRLEN);
        vxlan_info->vxlan_tunnel_init[tnl_idx].tunnel_state = 0;
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_initiator_destroy_all
 * Purpose:
 *    Destroy all outgoing VXLAN tunnel
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_tunnel_initiator_destroy_all(int unit)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem; /* Tunnel initiator table memory */
    egr_ip_tunnel_entry_t *tnl_entry, *tnl_entries;
    int tnl_idx, num_entries, num_tnl;
    int tnl_type = 0;   /* Tunnel type. */
    uint32 entry_type;  /* Entry type (IPv4/IPv6) */
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    num_tnl = vxlan_info->num_vxlan_tnl;

    /* Destroy VXLAN IPv4/IPv6 Tunnel from hardware */
    mem =  BCM_XGS3_L3_MEM(unit, tnl_init_v4);
    num_entries = soc_mem_index_count(unit, mem);
    tnl_entries = soc_cm_salloc(unit, num_entries * sizeof(*tnl_entries),
                                "egr_ip_tunnel");
    if (tnl_entries == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(tnl_entries, 0, num_entries * sizeof(*tnl_entries));

    soc_mem_lock(unit, mem);
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                            0, (num_entries - 1), tnl_entries);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        soc_cm_sfree(unit, tnl_entries);
        return rv;
    }

    for (tnl_idx = 0; tnl_idx < num_entries; tnl_idx++) {
        tnl_entry =
            soc_mem_table_idx_to_pointer(unit, mem,
                                         egr_ip_tunnel_entry_t *,
                                         tnl_entries, tnl_idx);
        tnl_type = soc_mem_field32_get(unit, mem, tnl_entry, TUNNEL_TYPEf);
        entry_type = soc_mem_field32_get(unit, mem, tnl_entry, ENTRY_TYPEf);
        if (tnl_type == _BCM_VXLAN_TUNNEL_TYPE) {
            if ((entry_type == BCM_XGS3_TUNNEL_INIT_V4) ||
                (entry_type == BCM_XGS3_TUNNEL_INIT_V6)) {
                /* Delete the entry from HW */
                /* Destroy the tunnel entry */
                (void)bcm_xgs3_tnl_init_del(unit, 0, tnl_idx);
            }
        }
    }
    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, tnl_entries);

    /* Clear all VXLAN IPv4/IPv6 Tunnel from software */
    for (tnl_idx = 0; tnl_idx < num_tnl; tnl_idx++) {
        vxlan_info->vxlan_tunnel_init[tnl_idx].sip = 0;
        vxlan_info->vxlan_tunnel_init[tnl_idx].dip = 0;
        sal_memset(vxlan_info->vxlan_tunnel_init[tnl_idx].sip6,
                   0, BCM_IP6_ADDRLEN);
        sal_memset(vxlan_info->vxlan_tunnel_init[tnl_idx].dip6,
                   0, BCM_IP6_ADDRLEN);
        vxlan_info->vxlan_tunnel_init[tnl_idx].tunnel_state = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_initiator_get
 * Purpose:
 *    Get an outgong VXLAN tunnel info
 * Parameters:
 *    unit - (IN) Device Number
 *    info - (IN/OUT) Tunnel initiator structure
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_tunnel_initiator_get(int unit, bcm_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    int soft_idx = -1, hw_idx = -1;
    egr_fragment_id_table_entry_t entry;
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read hw entry */
    soc_mem_t mem;      /* Tunnel initiator table memory.*/
    int df_val = 0;     /* Don't fragment encoded value. */
    int tnl_type = 0;   /* Tunnel type. */
    uint32 entry_type;  /* Entry type (IPv4/IPv6) */
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Input parameters check */
    if (NULL == info) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    if (!BCM_GPORT_IS_TUNNEL(info->tunnel_id)) {
        return BCM_E_PARAM;
    }

    soft_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
    GH2_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, soft_idx);

    /* Get SIP/DIP info from hardware */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_tunnel_initiator_idx_translate(unit, soft_idx,
                                                      &hw_idx));

    /* Get table memory for IPv4 */
    mem = BCM_XGS3_L3_MEM(unit, tnl_init_v4);
    /* Initialize the buffer */
    sal_memset(tnl_entry, 0, sizeof(tnl_entry));
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, tnl_entry));

    /* Get entry_type. */
    entry_type = soc_mem_field32_get(unit, mem, tnl_entry, ENTRY_TYPEf);
    if (entry_type == BCM_XGS3_TUNNEL_INIT_V6) {
        /* Tunnel for IPv6 */
        mem = BCM_XGS3_L3_MEM(unit, tnl_init_v6);
        hw_idx >>= 1;    /* Each record takes two slots. */

        /* Initialize the buffer */
        sal_memset(tnl_entry, 0, sizeof(tnl_entry));
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, tnl_entry));
    }

    /* Get tunnel type. */
    tnl_type = soc_mem_field32_get(unit, mem, tnl_entry, TUNNEL_TYPEf);
    if (tnl_type != _BCM_VXLAN_TUNNEL_TYPE) {
        return BCM_E_NOT_FOUND;
    }

    if (entry_type == BCM_XGS3_TUNNEL_INIT_V6) {
        /* Tunnel for IPv6 */

        /* Extract IPv6 SIP/DIP address */
        soc_mem_ip6_addr_get(unit, mem, tnl_entry, SIPf, info->sip6,
                             SOC_MEM_IP6_FULL_ADDR);
        sal_memcpy(info->dip6, vxlan_info->vxlan_tunnel_init[soft_idx].dip6,
                   BCM_IP6_ADDRLEN);

        /* Get FLOW_LABEL. */
        info->flow_label = soc_mem_field32_get(unit, mem,
                                               tnl_entry, FLOW_LABELf);

        /* Get HOP_LIMIT. */
        info->ttl = soc_mem_field32_get(unit, mem, tnl_entry, HOP_LIMITf);
    } else if (entry_type == BCM_XGS3_TUNNEL_INIT_V4) {
        /* Tunnel for IPv4 */

        /* Extract IPv4 SIP/DIP address */
        info->sip = soc_mem_field32_get(unit, mem, tnl_entry, SIPf);
        info->dip = vxlan_info->vxlan_tunnel_init[soft_idx].dip;

        /* IP tunnel hdr DF bit for IPv4 */
        df_val = soc_mem_field32_get(unit, mem, tnl_entry, IPV4_DF_SELf);
        if (0x2 <= df_val) {
            info->flags |= BCM_TUNNEL_INIT_USE_INNER_DF;
        } else if (0x1 == df_val) {
            info->flags |= BCM_TUNNEL_INIT_IPV4_SET_DF;
        }

        df_val = soc_mem_field32_get(unit, mem, tnl_entry, IPV6_DF_SELf);
        if (0x1 == df_val) {
            info->flags |= BCM_TUNNEL_INIT_IPV6_SET_DF;
        }

        /* Get TTL. */
        info->ttl = soc_mem_field32_get(unit, mem, tnl_entry, TTLf);
    } else {
        return BCM_E_NOT_FOUND;
    }

    /* Tunnel header DSCP select. */
    info->dscp_sel = soc_mem_field32_get(unit, mem, tnl_entry, DSCP_SELf);
    if (info->dscp_sel == bcmTunnelDscpMap) {
        /* Tunnel header DSCP_MAP value. */
        if (SOC_MEM_FIELD_VALID(unit, mem, DSCP_MAPPING_PTRf)) {
            BCM_IF_ERROR_RETURN(
                _bcm_tr2_qos_idx2id
                    (unit, soc_mem_field32_get(unit, mem, tnl_entry,
                                               DSCP_MAPPING_PTRf),
                     _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE,
                     &info->dscp_map));
        } else {
            return BCM_E_INTERNAL;
        }
    } else if (info->dscp_sel == bcmTunnelDscpAssign) {
        /* Tunnel header DSCP value. */
        if (SOC_MEM_FIELD_VALID(unit, mem, DSCPf)) {
            info->dscp = soc_mem_field32_get(unit, mem, tnl_entry, DSCPf);
        } else {
            return BCM_E_INTERNAL;
        }
    }

    /* Get UDP Dest Port */
    info->udp_dst_port =
        soc_mem_field32_get(unit, mem, tnl_entry, L4_DST_PORTf);

    /* Get UDP Src Port */
    info->udp_src_port =
        soc_mem_field32_get(unit, mem, tnl_entry, L4_SRC_PORTf);

    /*
     * Translate hw tunnel type into API encoding:
     * - bcmTunnelTypeVxlan (IPv4) or bcmTunnelTypeVxlan6 (IPv6)
     */
    BCM_IF_ERROR_RETURN(
        _bcm_trx_tnl_hw_code_to_type(unit,
                                     tnl_type, entry_type, &info->type));

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, EGR_FRAGMENT_ID_TABLEm,
                     MEM_BLOCK_ANY, hw_idx, &entry));
    info->ip4_id = soc_mem_field32_get(
        unit, EGR_FRAGMENT_ID_TABLEm, &entry, FRAGMENT_IDf);

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_tunnel_initiator_traverse
 * Purpose:
 *    Traverse VXLAN tunnel initiator entries
 * Parameters:
 *    unit - (IN) Device Number
 *    cb   - (IN) User callback function, called once per entry
 *    user_data - (IN) User supplied cookie used in parameter
 *                           in callback function
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_tunnel_initiator_traverse(
    int unit,
    bcm_tunnel_initiator_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_NONE;
    int num_tnl = 0, idx = 0;
    bcm_tunnel_initiator_t info;
    bcm_ip6_t sip6;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Input parameter check */
    if (NULL == cb) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    num_tnl = vxlan_info->num_vxlan_tnl;

    sal_memset(sip6, 0, BCM_IP6_ADDRLEN);

    /* Iterate over all the entries - search valid ones */
    for (idx = 0; idx < num_tnl; idx++) {
        /* Check a valid entry */
        sal_memcpy(sip6, vxlan_info->vxlan_tunnel_init[idx].sip6,
                   BCM_IP6_ADDRLEN);
        if ((vxlan_info->vxlan_tunnel_init[idx].sip != 0) ||
            (!bcmi_gh2_vxlan_tunnel_ip6_is_zero(sip6))) {
            /* Reset buffer before read */
            bcm_tunnel_initiator_t_init(&info);

            BCM_GPORT_TUNNEL_ID_SET(info.tunnel_id, idx);
            rv = bcmi_gh2_vxlan_tunnel_initiator_get(unit, &info);

            /* search failure */
            if (BCM_FAILURE(rv)) {
                if (rv != BCM_E_NOT_FOUND) {
                    break;
                }
                continue;
            }

            rv = (*cb)(unit, &info, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                break;
            }
#endif
        }
    }

    /* Reset last read status */
    if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_resolve
 * Purpose:
 *    Get the modid, port, trunk values for a VXLAN port
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_port_resolve(
    int unit,
    bcm_gport_t vxlan_port_id,
    bcm_if_t encap_id,
    bcm_module_t *modid,
    bcm_port_t *port,
    bcm_trunk_t *trunk_id,
    int *id)

{
    int rv = BCM_E_NONE;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    int ecmp = 0, nh_index = -1, nh_ecmp_index = -1, vp = -1;
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read hw entry */
    int idx, max_ent_count, base_idx;

    /* Input parameter check */
    if ((NULL == modid) || (NULL == port) ||
        (NULL == trunk_id) || (NULL == id)) {
       return BCM_E_PARAM;
    }
    if (!BCM_GPORT_IS_VXLAN_PORT(vxlan_port_id)) {
        return BCM_E_BADID;
    }

    vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port_id);
    if (vp == -1) {
       return BCM_E_PARAM;
    }

    if (!bcmi_gh2_vxlan_vp_used_get(unit, vp)) {
        return BCM_E_NOT_FOUND;
    }

    /* GH2 requires to retrieve vp to ecmp or nhi from software database */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_port_to_nh_ecmp_index(unit, vp,
                                             &ecmp, &nh_ecmp_index));

    if (!ecmp) {
        nh_index = nh_ecmp_index;
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                         nh_index, &ing_nh));

        if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
            *trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit,
                                                         &ing_nh, TGIDf);
        } else {
            /* Only add this to replication set if destination is local */
            *modid = soc_ING_L3_NEXT_HOPm_field32_get(unit,
                                                      &ing_nh, MODULE_IDf);
            *port = soc_ING_L3_NEXT_HOPm_field32_get(unit,
                                                     &ing_nh, PORT_NUMf);
        }
    } else {
        /* Select the desired next hop from ECMP group - pointed by encap_id */
        BCM_IF_ERROR_RETURN(
             soc_mem_read(unit, L3_ECMP_COUNTm, MEM_BLOCK_ANY,
                          nh_ecmp_index, hw_buf));

        max_ent_count = soc_mem_field32_get(unit, L3_ECMP_COUNTm,
                                            hw_buf, COUNTf);
        base_idx = soc_mem_field32_get(unit, L3_ECMP_COUNTm,
                                       hw_buf, BASE_PTRf);
        max_ent_count++; /* Count is zero based */

        if (encap_id == -1) {
            BCM_IF_ERROR_RETURN(
                 soc_mem_read(unit, L3_ECMPm, MEM_BLOCK_ANY,
                              base_idx, hw_buf));

            nh_index = soc_mem_field32_get(unit, L3_ECMPm,
                                           hw_buf, NEXT_HOP_INDEXf);

            BCM_IF_ERROR_RETURN (
                soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                             nh_index, &ing_nh));

            if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
                *trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit,
                                                             &ing_nh, TGIDf);
            } else {
                /* Only add this to replication set if destination is local */
                *modid = soc_ING_L3_NEXT_HOPm_field32_get(unit,
                                                          &ing_nh, MODULE_IDf);
                *port = soc_ING_L3_NEXT_HOPm_field32_get(unit,
                                                         &ing_nh, PORT_NUMf);
           }
        } else {
            for (idx = 0; idx < max_ent_count; idx++) {
                BCM_IF_ERROR_RETURN(
                     soc_mem_read(unit, L3_ECMPm, MEM_BLOCK_ANY,
                                  (base_idx + idx), hw_buf));
                nh_index = soc_mem_field32_get(unit, L3_ECMPm,
                                               hw_buf, NEXT_HOP_INDEXf);
                BCM_IF_ERROR_RETURN(
                     soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                  nh_index, &egr_nh));
                if (encap_id == soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                                    &egr_nh, INTF_NUMf)) {
                    BCM_IF_ERROR_RETURN(
                        soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                     nh_index, &ing_nh));
                    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
                        *trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit,
                                                                     &ing_nh,
                                                                     TGIDf);
                    } else {
                        /*
                         * Only add this to replication set
                         * if destination is local
                         */
                        *modid = soc_ING_L3_NEXT_HOPm_field32_get(unit,
                                                                  &ing_nh,
                                                                  MODULE_IDf);
                        *port = soc_ING_L3_NEXT_HOPm_field32_get(unit,
                                                                 &ing_nh,
                                                                 PORT_NUMf);
                    }
                    break;
                }
            }
        }
    }

    *id = vp;

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_cnt_update
 * Purpose:
 *    Update port's VP count.
 * Parameters:
 *    unit - (IN) Device Number
 *    gport- (IN) GPORT ID
 *    vp   - (IN) Virtual port number
 *    incr - (IN) If TRUE, increment VP count, else decrease VP count
 * Returns:
 *    BCM_X_XXX
 */
STATIC int
bcmi_gh2_vxlan_port_cnt_update(
    int unit,
    bcm_gport_t gport,
    int vp,
    int incr_decr_flag)
{
    int mod_out = -1, port_out = -1, tgid_out = -1, vp_out = -1;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count = 0;
    int idx = -1;
    int mod_local = -1;
    _bcm_port_info_t *port_info;
    uint32 port_flags;

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_port_resolve(unit, gport, -1, &mod_out,
                                    &port_out, &tgid_out, &vp_out));

    if (vp_out == -1) {
        return BCM_E_PARAM;
    }

    /*
     * Update the physical port's SW state. If associated with a trunk,
     * update each local physical port's SW state.
     */
    if (BCM_TRUNK_INVALID != tgid_out) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_local_members_get(unit, tgid_out,
                                             SOC_MAX_NUM_PORTS,
                                             local_member_array,
                                             &local_member_count));

        for (idx = 0; idx < local_member_count; idx++) {
            _bcm_port_info_access(unit, local_member_array[idx], &port_info);
            if (incr_decr_flag) {
                port_info->vp_count++;
            } else {
                if (port_info->vp_count > 0) {
                    port_info->vp_count--;
                }
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_get(unit,
                                             local_member_array[idx],
                                             &port_flags));
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_set(unit,
                                             local_member_array[idx],
                                             port_flags));
        }
    } else {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_modid_is_local(unit, mod_out, &mod_local));

        if (mod_local) {
            _bcm_port_info_access(unit, port_out, &port_info);

            if (incr_decr_flag) {
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

/*
 * Function:
 *    bcmi_gh2_vxlan_ingress_dvp_set
 * Purpose:
 *    Set Ingress DVP to Next Hop info
 * Parameters:
 *    unit - (IN) Device Number
 *    vp   - (IN) Virtual port number
 *    mpath_flag       - (IN) Multiple Path flag
 *    vp_nh_ecmp_index - (IN) nhi or ecmp index
 *    vxlan_port       - (IN) vxlan_port information
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_ingress_dvp_set(
    int unit,
    int vp,
    uint32 mpath_flag,
    int vp_nh_ecmp_index,
    bcm_vxlan_port_t *vxlan_port)
{
    int rv = BCM_E_NONE;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vp_info_t *vp_info;

    /* Input parameter check */
    if (NULL == vxlan_port) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    vp_info = &(vxlan_info->vxlan_vp_info[vp]);

    if (!(vxlan_port->flags & BCM_VXLAN_PORT_SHARE)) {
        if (mpath_flag) {
             vp_info->ecmp = 1; /* ECMP */
        } else {
             vp_info->ecmp = 0; /* Next Hop */
        }
        vp_info->nh_ecmp_index = vp_nh_ecmp_index;
    }

    if (vxlan_port->flags & BCM_VXLAN_PORT_EGRESS_TUNNEL) {
        vp_info->vp_type = GH2_VXLAN_DEST_VP_TYPE_TUNNEL;
    } else {
        vp_info->vp_type = GH2_VXLAN_DEST_VP_TYPE_ACCESS;
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_egress_tunnel_set
 * Purpose:
 *    Set Egress Tunnel index for Egress Next Hop Info
 * Parameters:
 *    unit        - (IN) Device Number
 *    vxlan_port  - (IN) vxlan_port information
 *    egr_nh_info - (OUT) Egress Next Hop Info
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_egress_tunnel_set(
    int unit,
    bcm_vxlan_port_t *vxlan_port,
    bcmi_gh2_vxlan_nh_info_t  *egr_nh_info)
{
    int rv = BCM_E_NONE;
    int tunnel_index = -1, hw_idx = -1;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Input parameter check */
    if ((NULL == vxlan_port) || (NULL == egr_nh_info)) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    tunnel_index = BCM_GPORT_TUNNEL_ID_GET(vxlan_port->egress_tunnel_id);
    GH2_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tunnel_index);

    /* Got HW tunnel index */
    BCM_IF_ERROR_RETURN(
           bcmi_gh2_vxlan_tunnel_initiator_idx_translate(unit, tunnel_index,
                                                         &hw_idx));
    egr_nh_info->tunnel_index = hw_idx;

    if (vxlan_port->flags & BCM_VXLAN_PORT_UDP_SRC_PORT_RANGE_ENABLE) {
        egr_nh_info->udp_src_port_range_enable = 1;
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_egress_tunnel_get
 * Purpose:
 *    Get Egress Tunnel index Info
 * Parameters:
 *    unit         - (IN) Device Number
 *    vxlan_port   - (IN/OUT) vxlan_port information
 *    tunnel_index - (IN) Tunnel index
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_egress_tunnel_get(
    int unit,
    bcm_vxlan_port_t *vxlan_port,
    int tunnel_index)
{
    int idx = 0, num_tnl = 0;
    soc_mem_t mem; /* Tunnel initiator table memory */
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read hw entry */
    uint32 entry_type;  /* Entry type (IPv4/IPv6) */
    bcm_ip_t dip = 0;       /* Tunnel header DIP (IPv4). */
    bcm_ip6_t dip6;         /* Tunnel header DIP (IPv6). */
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Input parameter check */
    if (NULL == vxlan_port) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    num_tnl = vxlan_info->num_vxlan_tnl;

    /* First to read for IPv4 */
    mem = BCM_XGS3_L3_MEM(unit, tnl_init_v4);
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, mem, MEM_BLOCK_ANY, tunnel_index, tnl_entry));
    entry_type = soc_mem_field32_get(unit, mem, tnl_entry, ENTRY_TYPEf);
    if (entry_type != BCM_XGS3_TUNNEL_INIT_V4) {
        mem = BCM_XGS3_L3_MEM(unit, tnl_init_v6);
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, tunnel_index, tnl_entry));
        entry_type = soc_mem_field32_get(unit, mem, tnl_entry, ENTRY_TYPEf);
    }

    if (entry_type == BCM_XGS3_TUNNEL_INIT_V6) {
        /* Extract IPv6 address. */
        soc_mem_ip6_addr_get(unit, mem, tnl_entry, DIPf, dip6,
                             SOC_MEM_IP6_FULL_ADDR);

        /* Find DIP within Softare State */
        for (idx = 0; idx < num_tnl; idx++) {
            if (sal_memcmp(dip6, vxlan_info->vxlan_tunnel_init[idx].dip6,
                           BCM_IP6_ADDRLEN) == 0) {
                break;
            }
        }
    } else if (entry_type == BCM_XGS3_TUNNEL_INIT_V4) {
        /* Extract IPv4 address. */
        dip = soc_mem_field32_get(unit, mem, tnl_entry, DIPf);

        /* Find DIP within Softare State */
        for (idx = 0; idx < num_tnl; idx++) {
            if (vxlan_info->vxlan_tunnel_init[idx].dip == dip) {
                break;
            }
        }
    }

    if (idx == num_tnl) {
        return BCM_E_NOT_FOUND;
    }

    BCM_GPORT_TUNNEL_ID_SET(vxlan_port->egress_tunnel_id, idx);

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vlan_tag_set
 * Purpose:
 *    Program inner payload/outer tunnel VLAN TAG settings
 * Parameters:
 *    unit            - (IN) Device Number
 *    vxlan_port      - (IN) VXLAN port info
 *    egr_nh_info     - (IN/OUT) Egress Next Hop Info
 *    nh_index        - (IN) Egress Next Hop Index
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_vlan_tag_set(
    int unit,
    bcm_vxlan_port_t *vxlan_port,
    bcmi_gh2_vxlan_nh_info_t *egr_nh_info,
    int nh_index)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t action;
    egr_vlan_xlate_entry_t egr_vent, return_vent;
    int index = -1;
    uint32 egr_profile_idx;
    soc_mem_t hw_mem = EGR_VLAN_XLATEm;

    /* Input parameter check */
    if ((NULL == vxlan_port) || (NULL == egr_nh_info)) {
        return BCM_E_PARAM;
    }

    /* VXLAN-LITE doesn't support replace TPID related actions */
    if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_TPID_REPLACE ||
        vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_TPID_REPLACE ||
        vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_TPID_REPLACE) {
        return BCM_E_UNAVAIL;
    }

    /* Common setting for below flags */
    if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_ADD ||
        vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_REPLACE ||
        vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_REPLACE ||
        vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_PRI_REPLACE ||
        (!(vxlan_port->flags & BCM_VXLAN_PORT_EGRESS_TUNNEL) &&
        vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_DELETE)) {
        bcm_vlan_action_set_t_init(&action);
        sal_memset(&egr_vent, 0, sizeof(egr_vent));
        /* Valid Entry */
        soc_mem_field32_set(unit, hw_mem, &egr_vent, VALIDf, 1);
        /* Entry type = NHI */
        soc_mem_field32_set(unit, hw_mem, &egr_vent, ENTRY_TYPEf, 0x07);
        /* Key type = nh_index */
        soc_mem_field32_set(unit, hw_mem, &egr_vent, NHI__NHIf, nh_index);

        /* Old Outer VID = match_vlan */
        if (!BCM_VLAN_VALID(vxlan_port->match_vlan)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__OLD_VLAN_IDf, vxlan_port->match_vlan);
    }

    if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_ADD) {
        if (!BCM_VLAN_VALID(vxlan_port->egress_service_vlan)) {
            return BCM_E_PARAM;
        }

        if (vxlan_port->pkt_pri > BCM_PRIO_MAX) {
            return BCM_E_PARAM;
        }
        if (vxlan_port->pkt_cfi != 0) {
            return BCM_E_PARAM;
        }

        action.ut_outer = bcmVlanActionAdd;
        action.it_outer = bcmVlanActionAdd;
        BCM_IF_ERROR_RETURN(
            _bcm_trx_egr_vlan_action_profile_entry_add(unit, &action,
                                                       &egr_profile_idx));

        /* Data */
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__TAG_ACTION_PROFILE_PTRf, egr_profile_idx);
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__NEW_OVIDf, vxlan_port->egress_service_vlan);
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__NEW_OPRIf, vxlan_port->pkt_pri);
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__NEW_ORPEf, 0x1);
    }

    if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_REPLACE) {
        if (!BCM_VLAN_VALID(vxlan_port->egress_service_vlan)) {
            return BCM_E_PARAM;
        }

        action.ot_outer = bcmVlanActionReplace;
        action.dt_outer = bcmVlanActionReplace;
        BCM_IF_ERROR_RETURN(
            _bcm_trx_egr_vlan_action_profile_entry_add(unit, &action,
                                                       &egr_profile_idx));

        /* Data */
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__TAG_ACTION_PROFILE_PTRf, egr_profile_idx);
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__NEW_OVIDf, vxlan_port->egress_service_vlan);
    } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_REPLACE) {
        if (!BCM_VLAN_VALID(vxlan_port->egress_service_vlan)) {
            return BCM_E_PARAM;
        }

        if (vxlan_port->pkt_pri > BCM_PRIO_MAX) {
            return BCM_E_PARAM;
        }
        if (vxlan_port->pkt_cfi != 0) {
            return BCM_E_PARAM;
        }

        action.ot_outer = bcmVlanActionReplace;
        action.ot_outer_prio = bcmVlanActionReplace;
        action.dt_outer = bcmVlanActionReplace;
        action.dt_outer_prio = bcmVlanActionReplace;
        BCM_IF_ERROR_RETURN(
            _bcm_trx_egr_vlan_action_profile_entry_add(unit, &action,
                                                       &egr_profile_idx));

        /* Data */
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__TAG_ACTION_PROFILE_PTRf, egr_profile_idx);
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__NEW_OVIDf, vxlan_port->egress_service_vlan);
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__NEW_OPRIf, vxlan_port->pkt_pri);
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__NEW_ORPEf, 0x1);
    } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_PRI_REPLACE) {
        if (vxlan_port->pkt_pri > BCM_PRIO_MAX) {
            return BCM_E_PARAM;
        }
        if (vxlan_port->pkt_cfi != 0) {
            return BCM_E_PARAM;
        }

        action.ot_outer_prio = bcmVlanActionReplace;
        action.dt_outer_prio = bcmVlanActionReplace;
        BCM_IF_ERROR_RETURN(
            _bcm_trx_egr_vlan_action_profile_entry_add(unit, &action,
                                                       &egr_profile_idx));

        /* Data */
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__TAG_ACTION_PROFILE_PTRf, egr_profile_idx);
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__NEW_OPRIf, vxlan_port->pkt_pri);
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__NEW_ORPEf, 0x1);
    } else if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_DELETE) {
        if (vxlan_port->flags & BCM_VXLAN_PORT_EGRESS_TUNNEL) {
            egr_nh_info->payload_otag_delete = 1;
        } else {
            action.ot_outer = bcmVlanActionDelete;
            action.dt_outer = bcmVlanActionDelete;
            BCM_IF_ERROR_RETURN(
                _bcm_trx_egr_vlan_action_profile_entry_add(unit, &action,
                                                           &egr_profile_idx));

            /* Data */
            soc_mem_field32_set(unit, hw_mem, &egr_vent,
                                NHI__TAG_ACTION_PROFILE_PTRf, egr_profile_idx);
        }
    }

    /* Common memory write for below flags */
    if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_ADD ||
        vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_REPLACE ||
        vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_PRI_REPLACE ||
        vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_PRI_REPLACE ||
        (!(vxlan_port->flags & BCM_VXLAN_PORT_EGRESS_TUNNEL) &&
        vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_VLAN_DELETE)) {
        rv = soc_mem_search(unit, hw_mem,
                            MEM_BLOCK_ANY, &index, &egr_vent, &return_vent, 0);
        if (BCM_SUCCESS(rv)) {
            /* Replace the new egress vlan translate entry */
            BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, hw_mem, MEM_BLOCK_ALL, index, &egr_vent));
            /* Delete the old vlan translate profile entry */
            egr_profile_idx =
                soc_mem_field32_get(unit, hw_mem, &return_vent,
                                    NHI__TAG_ACTION_PROFILE_PTRf);
            BCM_IF_ERROR_RETURN(
                _bcm_trx_egr_vlan_action_profile_entry_delete
                    (unit, egr_profile_idx));
        } else if (rv == BCM_E_NOT_FOUND) {
            BCM_IF_ERROR_RETURN(
                soc_mem_insert(unit, hw_mem, MEM_BLOCK_ALL, &egr_vent));
        } else {
            return rv;
        }

        egr_nh_info->use_nhi_in_evxlt_key = 1;
        /* Apply EVXLT modification to inner payload VLAN tag */
        egr_nh_info->apply_evxlt_modify_to_payload = 1;
    }

    if (vxlan_port->flags & BCM_VXLAN_PORT_EGRESS_TUNNEL) {
        /* Use NHI as key for Egress VLAN Translation on Egress Tunnel */
        egr_nh_info->use_nhi_in_evxlt_key = 1;

        /* Delete Tunnel Outer Tag */
        if (vxlan_port->flags & BCM_VXLAN_PORT_TUNNEL_OUTER_VLAN_DELETE) {
            egr_nh_info->tunnel_otag_delete = 1;
        }
        /* Delete Tunnel Inner Tag */
        if (vxlan_port->flags & BCM_VXLAN_PORT_TUNNEL_INNER_VLAN_DELETE) {
            egr_nh_info->tunnel_itag_delete = 1;
        }

        /* DOT1P priority selection for Tunnel outer header */
        if (vxlan_port->flags & BCM_VXLAN_PORT_TUNNEL_QOS_MAP) {
            /* Apply qos_map_id to outer VXLAN header directly */
            egr_nh_info->apply_evxlt_modify_to_payload = 1;
            /* USE_MAPPING */
            egr_nh_info->qos_map_id = vxlan_port->qos_map_id;
        } else {
            if ((vxlan_port->tunnel_pkt_pri) || (vxlan_port->tunnel_pkt_cfi)) {
                /* Apply NEW_PCP/NEW_CFI to outer VXLAN header directly */
                egr_nh_info->apply_evxlt_modify_to_payload = 1;
                /* USE_FIXED */
                egr_nh_info->tunnel_pkt_pri = vxlan_port->tunnel_pkt_pri;
                egr_nh_info->tunnel_pkt_cfi = vxlan_port->tunnel_pkt_cfi;
            }
        }
    }

    /* Only support for GH2-B0 when VXLAN RIOT is enabled */
    if (GH2_VXLAN_RIOT_ENABLE(unit)) {
        if (vxlan_port->flags & BCM_VXLAN_PORT_LOOPBACK) {
            /* Copied to Custom Header IPMC bit for encap */
            if (vxlan_port->flags & BCM_VXLAN_PORT_MULTICAST) {
                egr_nh_info->custom_header_ipmc = 1;
            } else {
                egr_nh_info->custom_header_ipmc = 0;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_vlan_tag_get
 * Purpose:
 *    Program inner payload/outer tunnel VLAN TAG settings
 * Parameters:
 *    unit         - (IN) Device Number
 *    vxlan_port   - (IN/OUT) VXLAN port info
 *    egr_nh_entry - (IN) Egress L3 Next Hop entry
 *    nh_index     - (IN) Egress Next Hop Index
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_vlan_tag_get(
    int unit,
    bcm_vxlan_port_t *vxlan_port,
    egr_l3_next_hop_entry_t *egr_nh_entry,
    int nh_index)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t action;
    egr_vlan_xlate_entry_t egr_vent;
    egr_vlan_xlate_entry_t ret_egr_vent;
    int index = -1, use_nhi_in_evxlt_key = 0;
    uint32 entry_type, egr_profile_idx;
    soc_mem_t hw_mem = EGR_VLAN_XLATEm;
    uint32 value = 0, dot1p_pri_select = 0;
    int custom_header_ipmc = 0;

    /* Input parameter check */
    if ((NULL == vxlan_port) || (NULL == egr_nh_entry)) {
        return BCM_E_PARAM;
    }

    entry_type =
        soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry, ENTRY_TYPEf);

    if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_VXLAN_VIEW) {
        use_nhi_in_evxlt_key =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                                             VXLAN__USE_NHI_IN_EVXLT_KEYf);
    } else if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) {
        use_nhi_in_evxlt_key =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                                             L3__USE_NHI_IN_EVXLT_KEYf);

        /* Only support for GH2-B0 when VXLAN RIOT is enabled */
        if (GH2_VXLAN_RIOT_ENABLE(unit)) {
            custom_header_ipmc =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                                                 L3__CUSTOM_HEADER_IPMCf);
        }
    } else if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW) {
        use_nhi_in_evxlt_key =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                                             L3MC__USE_NHI_IN_EVXLT_KEYf);

        /* Only support for GH2-B0 when VXLAN RIOT is enabled */
        if (GH2_VXLAN_RIOT_ENABLE(unit)) {
            custom_header_ipmc =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                                                 L3MC__CUSTOM_HEADER_IPMCf);
        }
    } else if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_IFP_ACTION_VIEW) {
        /* Only support for GH2-B0 when VXLAN RIOT is enabled */
        if (GH2_VXLAN_RIOT_ENABLE(unit)) {
            custom_header_ipmc =
                soc_EGR_L3_NEXT_HOPm_field32_get
                    (unit, egr_nh_entry, IFP_ACTIONS__CUSTOM_HEADER_IPMCf);
        }
    }

    if (use_nhi_in_evxlt_key) {
        /* Initialize */
        sal_memset(&egr_vent, 0, sizeof(egr_vlan_xlate_entry_t));
        sal_memset(&ret_egr_vent, 0, sizeof(egr_vlan_xlate_entry_t));
        bcm_vlan_action_set_t_init(&action);

        /* Valid Entry */
        soc_mem_field32_set(unit, hw_mem, &egr_vent, VALIDf, 1);
        /* Entry type = NHI */
        soc_mem_field32_set(unit, hw_mem, &egr_vent, ENTRY_TYPEf, 0x07);
        /* Key type = nh_index */
        soc_mem_field32_set(unit, hw_mem, &egr_vent, NHI__NHIf, nh_index);
        /* Old Outer VID = match_vlan */
        VLAN_CHK_ID(unit, vxlan_port->match_vlan);
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__OLD_VLAN_IDf, vxlan_port->match_vlan);
        rv = soc_mem_search(unit, hw_mem, MEM_BLOCK_ANY, &index,
                            &egr_vent, &ret_egr_vent, 0);
        if (BCM_SUCCESS(rv)) {
            egr_profile_idx = soc_mem_field32_get(unit, hw_mem, &ret_egr_vent,
                                                  NHI__TAG_ACTION_PROFILE_PTRf);

            _bcm_trx_egr_vlan_action_profile_entry_get(unit, &action,
                                                       egr_profile_idx);

            if (action.ut_outer == bcmVlanActionAdd &&
                action.it_outer == bcmVlanActionAdd) {
                vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_ADD;
                vxlan_port->egress_service_vlan =
                    soc_mem_field32_get(unit, hw_mem, &ret_egr_vent,
                                        NHI__NEW_OVIDf);
                vxlan_port->pkt_pri =
                    soc_mem_field32_get(unit, hw_mem, &ret_egr_vent,
                                        NHI__NEW_OPRIf);
            }

            if (action.ot_outer == bcmVlanActionReplace &&
                action.ot_outer_prio == bcmVlanActionReplace &&
                action.dt_outer == bcmVlanActionReplace &&
                action.dt_outer_prio == bcmVlanActionReplace) {
                vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_PRI_REPLACE;
                vxlan_port->egress_service_vlan =
                    soc_mem_field32_get(unit, hw_mem, &ret_egr_vent,
                                        NHI__NEW_OVIDf);
                vxlan_port->pkt_pri =
                    soc_mem_field32_get(unit, hw_mem, &ret_egr_vent,
                                        NHI__NEW_OPRIf);
            } else if (action.ot_outer == bcmVlanActionReplace &&
                       action.dt_outer == bcmVlanActionReplace) {
                vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_REPLACE;
                vxlan_port->egress_service_vlan =
                    soc_mem_field32_get(unit, hw_mem, &ret_egr_vent,
                                        NHI__NEW_OVIDf);
            } else if (action.ot_outer_prio == bcmVlanActionReplace &&
                       action.dt_outer_prio == bcmVlanActionReplace) {
                vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_PRI_REPLACE;
                vxlan_port->pkt_pri =
                    soc_mem_field32_get(unit, hw_mem, &ret_egr_vent,
                                        NHI__NEW_OPRIf);
            } else if (action.ot_outer == bcmVlanActionDelete &&
                       action.dt_outer == bcmVlanActionDelete) {
                vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_DELETE;
                vxlan_port->egress_service_vlan = BCM_VLAN_INVALID;
            }

            vxlan_port->match_vlan =
                soc_mem_field32_get(unit, hw_mem, &ret_egr_vent,
                                    NHI__OLD_VLAN_IDf);
        }
    }

    if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_VXLAN_VIEW) {
        /* Delete Payload Outer Tag */
        value = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                                 egr_nh_entry,
                                                 VXLAN__DELETE_PAYLOAD_OTAGf);
        if (value) {
            vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_VLAN_DELETE;
        }

        /* Delete Tunnel Outer Tag */
        value = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                                 egr_nh_entry,
                                                 VXLAN__DELETE_TUNNEL_OTAGf);
        if (value) {
            vxlan_port->flags |= BCM_VXLAN_PORT_TUNNEL_OUTER_VLAN_DELETE;
        }

        /* Delete Tunnel Inner Tag */
        value = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                                 egr_nh_entry,
                                                 VXLAN__DELETE_TUNNEL_ITAGf);
        if (value) {
            vxlan_port->flags |= BCM_VXLAN_PORT_TUNNEL_INNER_VLAN_DELETE;
        }

        /* DOT1P priority selection for Tunnel outer header */
        dot1p_pri_select =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                                             VXLAN__DOT1P_PRI_SELECTf);
        if (dot1p_pri_select == 0x1) {
            /* USE_MAPPING */
            value = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                                                     VXLAN__DOT1P_MAPPING_PTRf);
            BCM_IF_ERROR_RETURN(
                _bcm_tr2_qos_idx2id(unit, value,
                _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS, &vxlan_port->qos_map_id));
            vxlan_port->flags |=  BCM_VXLAN_PORT_TUNNEL_QOS_MAP;
        } else {
            /* USE_FIXED */
            vxlan_port->tunnel_pkt_pri =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                                                 VXLAN__NEW_PCPf);
            vxlan_port->tunnel_pkt_cfi =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                                                 VXLAN__NEW_CFIf);
        }
    }

    /* Only support for GH2-B0 when VXLAN RIOT is enabled */
    if (GH2_VXLAN_RIOT_ENABLE(unit)) {
        if (custom_header_ipmc) {
            vxlan_port->flags |= BCM_VXLAN_PORT_MULTICAST;
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_nexthop_entry_modify
 * Purpose:
 *    Modify Egress entry
 * Parameters:
 *    unit           - (IN) Device Number
 *    nh_index       - (IN) Next Hop Index
 *    egr_nh_info    - (IN) Egress Next Hop info
 *    new_entry_type - (IN) New Entry type
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_nexthop_entry_modify(
    int unit,
    int nh_index,
    bcmi_gh2_vxlan_nh_info_t *egr_nh_info,
    int new_entry_type)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    egr_l3_next_hop_entry_t egr_nh_entry;
    uint32 old_entry_type = 0, intf_num = 0;
    bcm_mac_t mac_addr; /* Next hop forwarding destination mac. */
    uint32 macda_index;
    void *entries[1];
    egr_mac_da_profile_entry_t macda_entry;
    int pri_field_len = 0;
    int max_pri = 0;
    int hw_idx = 0;
    uint32 value;

    /* Input parameter check */
    if (NULL == egr_nh_info) {
        return BCM_E_PARAM;
    }

    mem = EGR_L3_NEXT_HOPm;

    sal_memset(&mac_addr, 0, sizeof(mac_addr));
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, mem, MEM_BLOCK_ANY, nh_index, &egr_nh_entry));

    old_entry_type = soc_mem_field32_get(unit, mem, &egr_nh_entry,
                                         ENTRY_TYPEf);

    if ((new_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_VXLAN_VIEW) &&
        ((old_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) ||
         (old_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_VXLAN_VIEW))) {

        if (old_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) {
            /* Get mac address for L3 (Normal view) */
            soc_mem_mac_addr_get(unit, mem, &egr_nh_entry,
                             L3__MAC_ADDRESSf, mac_addr);

            /* Get int_num for L3 (Normal view) */
            intf_num = soc_mem_field32_get(unit, mem, &egr_nh_entry,
                                           L3__INTF_NUMf);

            /* Zero buffers for new Entry Type (VXLAN) */
            sal_memset(&egr_nh_entry, 0, sizeof(egr_nh_entry));

            /* Add mac_addr (MAC_DA) into EGR_MAC_DA_PROFILEm */
            sal_memset(&macda_entry, 0, sizeof(macda_entry));
            soc_mem_mac_addr_set(unit, EGR_MAC_DA_PROFILEm, &macda_entry,
                                 MAC_ADDRESSf, mac_addr);
            entries[0] = &macda_entry;
            BCM_IF_ERROR_RETURN(
                _bcm_mac_da_profile_entry_add(unit, entries, 1, &macda_index));

            /* Set MAC_DA_PROFILE_INDEX */
            soc_mem_field32_set(unit, mem, &egr_nh_entry,
                                VXLAN__MAC_DA_PROFILE_INDEXf, macda_index);

            /* Set INTF_NUM */
            soc_mem_field32_set(unit, mem, &egr_nh_entry,
                                VXLAN__INTF_NUMf, intf_num);
        }

        /* Set Tunnel Index */
        if (egr_nh_info->tunnel_index != -1) {
            soc_mem_field32_set(unit, mem, &egr_nh_entry,
                                VXLAN__TUNNEL_INDEXf,
                                egr_nh_info->tunnel_index);
        }

        /* Set UDP source port range checking for Entropy */
        value = 0;
        if (egr_nh_info->udp_src_port_range_enable != -1) {
            value = egr_nh_info->udp_src_port_range_enable;
        }
        soc_mem_field32_set(unit, mem, &egr_nh_entry,
                            VXLAN__UDP_SRC_PORT_RANGE_ENf, value);

        /* Set VXLAN_CLASS_ID (currently use index = 0 for GH2) */
        value = 0;
        soc_mem_field32_set(unit, mem, &egr_nh_entry,
                            VXLAN__VXLAN_CLASS_IDf, value);

        /* Set UDP source port range checking for Entropy */
        value = 0;
        if (egr_nh_info->vnid != -1) {
            value = egr_nh_info->vnid;
        }
        soc_mem_field32_set(unit, mem, &egr_nh_entry,
                            VXLAN__VN_IDf, value);

        /* Use NHI as key in EGR_VLAN_XLATE */
        value = 0;
        if (egr_nh_info->use_nhi_in_evxlt_key != -1) {
            value = egr_nh_info->use_nhi_in_evxlt_key;
        }
        soc_mem_field32_set(unit, mem, &egr_nh_entry,
                            VXLAN__USE_NHI_IN_EVXLT_KEYf, value);

        /* Set for Egress VLAN XLATE modify to Payload/Tunnel */
        value = 0;
        if (egr_nh_info->apply_evxlt_modify_to_payload != -1) {
            value = egr_nh_info->apply_evxlt_modify_to_payload;
        }
        soc_mem_field32_set(unit, mem, &egr_nh_entry,
                            VXLAN__DO_EVXLT_BEFORE_TUNNEL_ENCAPf, value);

        /* Delete payload OTAG */
        value = 0;
        if (egr_nh_info->payload_otag_delete != -1) {
            value = egr_nh_info->payload_otag_delete;
        }
        soc_mem_field32_set(unit, mem, &egr_nh_entry,
                            VXLAN__DELETE_PAYLOAD_OTAGf, value);

        /* Delete Tunnel OTAG */
        value = 0;
        if (egr_nh_info->tunnel_otag_delete != -1) {
            value = egr_nh_info->tunnel_otag_delete;
        }
        soc_mem_field32_set(unit, mem, &egr_nh_entry,
                            VXLAN__DELETE_TUNNEL_OTAGf, value);

        /* Delete Tunnel ITAG */
        value = 0;
        if (egr_nh_info->tunnel_itag_delete != -1) {
            value = egr_nh_info->tunnel_itag_delete;
        }
        soc_mem_field32_set(unit, mem, &egr_nh_entry,
                            VXLAN__DELETE_TUNNEL_ITAGf, value);

        /* DOT1P priority selection for Tunnel outer header */
        if (egr_nh_info->qos_map_id != -1) {
            /* USE_MAPPING */
            BCM_IF_ERROR_RETURN(
                _bcm_tr2_qos_id2idx(unit, egr_nh_info->qos_map_id, &hw_idx));
            soc_mem_field32_set(unit, mem, &egr_nh_entry,
                                VXLAN__DOT1P_PRI_SELECTf, 0x1);
            soc_mem_field32_set(unit, mem, &egr_nh_entry,
                                VXLAN__DOT1P_MAPPING_PTRf, hw_idx);
        } else if ((egr_nh_info->tunnel_pkt_pri != -1) &&
                   (egr_nh_info->tunnel_pkt_cfi != -1)) {
            /* USE_FIXED */
            pri_field_len = soc_mem_field_length(unit, mem, VXLAN__NEW_PCPf);
            max_pri = (1 << pri_field_len) - 1;

            if ((egr_nh_info->tunnel_pkt_pri > max_pri) ||
                (egr_nh_info->tunnel_pkt_cfi > 1)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, mem, &egr_nh_entry,
                                VXLAN__DOT1P_PRI_SELECTf, 0x0);
            soc_mem_field32_set(unit, mem, &egr_nh_entry,
                                VXLAN__NEW_PCPf, egr_nh_info->tunnel_pkt_pri);
            soc_mem_field32_set(unit, mem, &egr_nh_entry,
                                VXLAN__NEW_CFIf, egr_nh_info->tunnel_pkt_cfi);
        }

        /* Set Entry Type */
        soc_mem_field32_set(unit, mem, &egr_nh_entry, ENTRY_TYPEf,
                            new_entry_type);
    } else if ((new_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW) &&
        (old_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW)) {
        /* Use NHI as key in EGR_VLAN_XLATE */
        value = 0;
        if (egr_nh_info->use_nhi_in_evxlt_key != -1) {
            value = egr_nh_info->use_nhi_in_evxlt_key;
        }
        soc_mem_field32_set(unit, mem, &egr_nh_entry,
                            L3MC__USE_NHI_IN_EVXLT_KEYf, value);

        /* Only support for GH2-B0 when VXLAN RIOT is enabled */
        if (GH2_VXLAN_RIOT_ENABLE(unit)) {
            /* Set to copied to Custom Header IPMC bit */
            value = 0;
            if (egr_nh_info->custom_header_ipmc != -1) {
                value = egr_nh_info->custom_header_ipmc;
            }
            soc_mem_field32_set(unit, mem, &egr_nh_entry,
                                L3MC__CUSTOM_HEADER_IPMCf, value);
        }
    } else if ((new_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) &&
        (old_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW)) {
        /* Use NHI as key in EGR_VLAN_XLATE */
        value = 0;
        if (egr_nh_info->use_nhi_in_evxlt_key != -1) {
            value = egr_nh_info->use_nhi_in_evxlt_key;
        }
        soc_mem_field32_set(unit, mem, &egr_nh_entry,
                            L3__USE_NHI_IN_EVXLT_KEYf, value);

        /* Only support for GH2-B0 when VXLAN RIOT is enabled */
        if (GH2_VXLAN_RIOT_ENABLE(unit)) {
            /* Set to copied to Custom Header IPMC bit */
            value = 0;
            if (egr_nh_info->custom_header_ipmc != -1) {
                value = egr_nh_info->custom_header_ipmc;
            }
            soc_mem_field32_set(unit, mem, &egr_nh_entry,
                                L3__CUSTOM_HEADER_IPMCf, value);
        }
    } else if ((new_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_IFP_ACTION_VIEW) &&
        ((old_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) ||
         (old_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_IFP_ACTION_VIEW))) {
        /* Only support for GH2-B0 when VXLAN RIOT is enabled */

        /* Set to copied to Custom Header IPMC bit */
        value = 0;
        if (egr_nh_info->custom_header_ipmc != -1) {
            value = egr_nh_info->custom_header_ipmc;
        }
        soc_mem_field32_set(unit, mem, &egr_nh_entry,
                            IFP_ACTIONS__CUSTOM_HEADER_IPMCf, value);

        /* Disable the DA/SA replacement when unicast decap flow */
        value = 1;
        soc_mem_field32_set(unit, mem, &egr_nh_entry,
                            IFP_ACTIONS__L3_UC_DA_DISABLEf, value);
        soc_mem_field32_set(unit, mem, &egr_nh_entry,
                            IFP_ACTIONS__L3_UC_SA_DISABLEf, value);

        /* Set Entry Type */
        soc_mem_field32_set(unit, mem, &egr_nh_entry, ENTRY_TYPEf,
                            new_entry_type);
    } else if (new_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) {
        if (old_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_VXLAN_VIEW) {
            /* Get MAC_DA_PROFILE_INDEX for VXLAN view */
            macda_index =
                soc_mem_field32_get(unit, mem, &egr_nh_entry,
                                    VXLAN__MAC_DA_PROFILE_INDEXf);
            /* Write MAC_DA from profile into NH table in L3 view */
            sal_memset(&macda_entry, 0, sizeof(macda_entry));
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, EGR_MAC_DA_PROFILEm, MEM_BLOCK_ANY,
                             macda_index, &macda_entry));
            soc_mem_mac_addr_get(unit, EGR_MAC_DA_PROFILEm, &macda_entry,
                                 MAC_ADDRESSf, mac_addr);
            soc_mem_mac_addr_set(unit, mem, &egr_nh_entry, L3__MAC_ADDRESSf,
                                 mac_addr);
            /* Delete mac_addr (MAC_DA) from EGR_MAC_DA_PROFILEm */
            BCM_IF_ERROR_RETURN(
                _bcm_mac_da_profile_entry_delete(unit, macda_index));
        }

        /* Use NHI as key in EGR_VLAN_XLATE */
        value = 0;
        if (egr_nh_info->use_nhi_in_evxlt_key != -1) {
            value = egr_nh_info->use_nhi_in_evxlt_key;
        }
        soc_mem_field32_set(unit, mem, &egr_nh_entry,
                            L3__USE_NHI_IN_EVXLT_KEYf, value);

        /* Only support for GH2-B0 when VXLAN RIOT is enabled */
        if (GH2_VXLAN_RIOT_ENABLE(unit)) {
            /* Set to copied to Custom Header IPMC bit */
            value = 0;
            if (egr_nh_info->custom_header_ipmc != -1) {
                value = egr_nh_info->custom_header_ipmc;
            }
            soc_mem_field32_set(unit, mem, &egr_nh_entry,
                                L3__CUSTOM_HEADER_IPMCf, value);
        }

        /* Set Entry Type */
        soc_mem_field32_set(unit, mem, &egr_nh_entry, ENTRY_TYPEf,
                            new_entry_type);
    } else if ((new_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_VXLAN_VIEW) &&
        (old_entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW)) {
        return BCM_E_PARAM;
    }

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, nh_index, &egr_nh_entry);

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_ecmp_nexthop_entry_modify
 * Purpose:
 *    Modify vxlan ecmp member object's Egress next hop entry
 * Parameters:
 *    unit           - (IN) Device Number
 *    vxlan_port - (IN) vxlan_port information
 *    egr_nh_info    - (IN) Egress Next Hop info
 *    new_entry_type - (IN) New Entry type
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_ecmp_nexthop_entry_modify(
    int unit,
    bcm_vxlan_port_t *vxlan_port,
    bcmi_gh2_vxlan_nh_info_t *egr_nh_info,
    int new_entry_type)
{
    int rv = BCM_E_NONE;
    int i = 0, ecmp_member_count = 0;
    int ecmp_index;
    int alloc_size, index;
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_if_t *ecmp_member_array = NULL;

    if ((NULL == vxlan_port) || (NULL == egr_nh_info) ) {
        return BCM_E_PARAM;
    }

    ecmp_index = vxlan_port->egress_if;
    if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp_index)) {
        return BCM_E_PARAM;
    }

    ecmp_info.ecmp_intf = ecmp_index;
    alloc_size = sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit);
    ecmp_member_array = sal_alloc(alloc_size, "ecmp member array");
    if (NULL == ecmp_member_array) {
        return BCM_E_MEMORY;
    }
    sal_memset(ecmp_member_array, 0, alloc_size);

    rv = bcm_esw_l3_egress_ecmp_get(unit, &ecmp_info,
                                    BCM_XGS3_L3_ECMP_MAX(unit),
                                    ecmp_member_array, &ecmp_member_count);
    if (BCM_FAILURE(rv)) {
        sal_free(ecmp_member_array);
        ecmp_member_array = NULL;
        return rv;
    }

    while (i < ecmp_member_count) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, ecmp_member_array[i])) {
            index = (ecmp_member_array[i] - BCM_XGS3_EGRESS_IDX_MIN(unit));
        } else {
            sal_free(ecmp_member_array);
            ecmp_member_array = NULL;
            return BCM_E_PARAM;
        }

        /* Configure inner payload/outer tunnel VLAN TAG setting */
        rv = bcmi_gh2_vxlan_vlan_tag_set(unit, vxlan_port,
                                         egr_nh_info, index);
        if (BCM_FAILURE(rv)) {
            sal_free(ecmp_member_array);
            ecmp_member_array = NULL;
            return rv;
        }

        rv = bcmi_gh2_vxlan_nexthop_entry_modify(unit, index,
                                                 egr_nh_info, new_entry_type);
        if (BCM_FAILURE(rv)) {
            sal_free(ecmp_member_array);
            ecmp_member_array = NULL;
            return rv;
        }

        i++;
    }

    if (ecmp_member_array) {
        sal_free(ecmp_member_array);
        ecmp_member_array = NULL;
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_ecmp_member_egress_get_first
 * Purpose:
 *    Get ECMP first member nexthop index
 * Parameters:
 *    unit       - (IN) Device Number
 *    ecmp_index - (IN) ECMP index
 *    index      - (OUT) poiter to first next hop index
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_ecmp_member_egress_get_first(
    int unit,
    int ecmp_index,
    int *index)
{
    int rv = BCM_E_NONE;
    int alloc_size;
    int ecmp_member_count = 0;
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_if_t *ecmp_member_array = NULL;

    /* Input parameter check */
    if (NULL == index) {
        return BCM_E_PARAM;
    }

    if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp_index)) {
        return BCM_E_PARAM;
    }

    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.ecmp_intf = ecmp_index;
    alloc_size = sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit);
    ecmp_member_array = sal_alloc(alloc_size, "ecmp member array");
    if (NULL == ecmp_member_array) {
        return BCM_E_MEMORY;
    }
    sal_memset(ecmp_member_array, 0, alloc_size);

    rv = bcm_esw_l3_egress_ecmp_get(unit, &ecmp_info,
                                    BCM_XGS3_L3_ECMP_MAX(unit),
                                    ecmp_member_array, &ecmp_member_count);
    if (BCM_FAILURE(rv)) {
        sal_free(ecmp_member_array);
        ecmp_member_array = NULL;
        return rv;
    }

    if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, ecmp_member_array[0])) {
        *index = (ecmp_member_array[0] - BCM_XGS3_EGRESS_IDX_MIN(unit));
    } else {
        rv = BCM_E_PARAM;
    }

    if (ecmp_member_array) {
        sal_free(ecmp_member_array);
        ecmp_member_array = NULL;
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_egress_nexthop_reset
 * Purpose:
 *    Reset VXLAN Egress NextHop
 * Parameters:
 *    unit     - (IN) Device Number
 *    nh_index - (IN) Next Hop index
 *    vp_type - (IN) VP type
 *    vp       - (IN) vxlan virtual port
 *    vpn      - (IN) VPN instance ID
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_port_egress_nexthop_reset(
    int unit,
    int nh_index,
    int vp_type,
    int vp,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    egr_l3_next_hop_entry_t egr_nh_entry;
    uint32 entry_type = 0;
    bcmi_gh2_vxlan_nh_info_t egr_nh_info;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    int index = -1, use_nhi_in_evxlt_key = 0;
    egr_vlan_xlate_entry_t egr_vent;
    egr_vlan_xlate_entry_t ret_egr_vent;
    soc_mem_t hw_mem = EGR_VLAN_XLATEm;
    uint32 egr_profile_idx;

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                     nh_index, &egr_nh_entry));

    /* Egressing into a regular port */
    entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                                  &egr_nh_entry, ENTRY_TYPEf);
    if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_VXLAN_VIEW) {
        use_nhi_in_evxlt_key =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh_entry,
                                             VXLAN__USE_NHI_IN_EVXLT_KEYf);
    } else if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) {
        use_nhi_in_evxlt_key =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh_entry,
                                             L3__USE_NHI_IN_EVXLT_KEYf);
    } else if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW) {
        use_nhi_in_evxlt_key =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh_entry,
                                             L3MC__USE_NHI_IN_EVXLT_KEYf);
    }

    if (use_nhi_in_evxlt_key) {
        /* Initialize */
        sal_memset(&egr_vent, 0, sizeof(egr_vlan_xlate_entry_t));
        sal_memset(&ret_egr_vent, 0, sizeof(egr_vlan_xlate_entry_t));

        /* Valid Entry */
        soc_mem_field32_set(unit, hw_mem, &egr_vent, VALIDf, 1);
        /* Entry type = NHI */
        soc_mem_field32_set(unit, hw_mem, &egr_vent, ENTRY_TYPEf, 0x07);
        /* Key type = nh_index */
        soc_mem_field32_set(unit, hw_mem, &egr_vent, NHI__NHIf, nh_index);
        /* Old Outer VID = match_vlan */
        soc_mem_field32_set(unit, hw_mem, &egr_vent,
                            NHI__OLD_VLAN_IDf,
                            vxlan_info->match_key[vp].match_vlan);
        rv = soc_mem_search(unit, hw_mem, MEM_BLOCK_ANY, &index,
                            &egr_vent, &ret_egr_vent, 0);
        if (BCM_SUCCESS(rv)) {
            /* Existing EGR_VLAN_XLATE entry is found */

            /* Delete the old vlan translate profile entry */
            egr_profile_idx =
                soc_mem_field32_get(unit, hw_mem, &ret_egr_vent,
                                    NHI__TAG_ACTION_PROFILE_PTRf);
            BCM_IF_ERROR_RETURN(
                _bcm_trx_egr_vlan_action_profile_entry_delete
                    (unit, egr_profile_idx));

            /* Delete the related EGR_VLAN_XLATE entry with nh_index */
            BCM_IF_ERROR_RETURN(
                soc_mem_delete(unit, hw_mem, MEM_BLOCK_ALL, &egr_vent));
        } else {
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_NONE;
            } else {
                return rv;
            }
        }
    }

    sal_memset(&egr_nh_info, 0, sizeof(egr_nh_info));
    if (vp_type == GH2_VXLAN_DEST_VP_TYPE_ACCESS) {
        /* Normalize Next-hop Entry - L3 View */
        rv = bcmi_gh2_vxlan_nexthop_entry_modify
                  (unit, nh_index, &egr_nh_info,
                   _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW);

    } else if (vp_type == GH2_VXLAN_DEST_VP_TYPE_TUNNEL) {
        /*
         * Egressing into a tunnel, vp type should be
         * GH2_VXLAN_DEST_VP_TYPE_TUNNEL
         */
        /* Normalize Next-hop Entry - L3 View */
        if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_VXLAN_VIEW) {
            rv = bcmi_gh2_vxlan_nexthop_entry_modify
                      (unit, nh_index, &egr_nh_info,
                       _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW);
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_ecmp_port_egress_nexthop_reset
 * Purpose:
 *    Reset VXLAN ecmp Egress NextHop
 * Parameters:
 *    unit       - (IN) Device Number
 *    ecmp_index - (IN) ECMP index
 *    vp_type    - (IN) VP type
 *    vp         - (IN) vxlan virtual port
 *    vpn        - (IN) VPN instance ID
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_ecmp_port_egress_nexthop_reset(
    int unit,
    int ecmp_index,
    int vp_type,
    int vp,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    int i = 0, ecmp_member_count = 0;
    int alloc_size, index;
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_if_t *ecmp_member_array = NULL;
    uint32 ref_count;

    if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp_index)) {
        return BCM_E_PARAM;
    }

    /* Initialize ecmp_info data structure */
    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.ecmp_intf = ecmp_index;
    alloc_size = sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit);
    ecmp_member_array = sal_alloc(alloc_size, "ecmp member array");
    if (NULL == ecmp_member_array) {
        return BCM_E_MEMORY;
    }
    sal_memset(ecmp_member_array, 0, alloc_size);

    rv = bcm_esw_l3_egress_ecmp_get(unit, &ecmp_info,
                                    BCM_XGS3_L3_ECMP_MAX(unit),
                                    ecmp_member_array, &ecmp_member_count);
    if (BCM_FAILURE(rv)) {
        sal_free(ecmp_member_array);
        ecmp_member_array = NULL;
        return rv;
    }

    while (i < ecmp_member_count) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, ecmp_member_array[i])) {
            index = (ecmp_member_array[i] - BCM_XGS3_EGRESS_IDX_MIN(unit));
        } else {
            sal_free(ecmp_member_array);
            ecmp_member_array = NULL;
            return BCM_E_PARAM;
        }

        /* Get current reference count */
        _bcm_esw_l3_egress_reference_get(unit, index, 0, &ref_count);

        /*
         * If (ref_count == 1), it means the NHI is used for
         * ECMP group only, then we must reset VXLAN Egress NextHop.
         */
        if (ref_count == 1) {
            if (index) {
                rv = bcmi_gh2_vxlan_port_egress_nexthop_reset
                         (unit, index, vp_type, vp, vpn);
            }
        }

        if (BCM_FAILURE(rv)) {
            sal_free(ecmp_member_array);
            ecmp_member_array = NULL;
            return rv;
        }
        i++;
    }

    if (ecmp_member_array) {
        sal_free(ecmp_member_array);
        ecmp_member_array = NULL;
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_nexthop_ecmp_find
 * Purpose:
 *    Find ECMP group with specified egress forwarding object and
 *    reset VXLAN Egress NextHop when found
 * Parameters:
 *    unit       - (IN) Device Number
 *    ecmp       - (IN) ECMP group info.
 *    intf_count - (IN) Number of elements in intf_array.
 *    intf_array - (IN) Array of Egress forwarding objects.
 *    user_data  - (IN) User data to be passed to callback function.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_nexthop_ecmp_find(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array,
    void *user_data)
{
    int rv = BCM_E_NONE;
    int *nh_ecmp_index;
    int idx, nh_ecmp_intf, found, nhi;
    egr_l3_next_hop_entry_t egr_nh_entry;
    bcmi_gh2_vxlan_nh_info_t egr_nh_info;
    uint32 entry_type = 0;

    /* Input parameters check */
    if ((NULL == ecmp) || (NULL == intf_array) || (NULL == user_data)) {
        return BCM_E_PARAM;
    }

    nh_ecmp_index = (int *)user_data;
    nh_ecmp_intf = (*nh_ecmp_index + BCM_XGS3_EGRESS_IDX_MIN(unit));
    found = 0;
    for (idx = 0; idx < intf_count; idx++) {
        if (*intf_array == nh_ecmp_intf) {
            found = 1;
            break;
        }
        intf_array++;
    }

    if (found != 0) {
        sal_memset(&egr_nh_info, 0, sizeof(egr_nh_info));

        nhi = (nh_ecmp_intf - BCM_XGS3_EGRESS_IDX_MIN(unit));
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                         nhi, &egr_nh_entry));

        /* Egressing into a regular port */
        entry_type =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh_entry,
                                             ENTRY_TYPEf);

        /* Normalize Next-hop Entry - L3 View */
        if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_VXLAN_VIEW) {
            rv = bcmi_gh2_vxlan_nexthop_entry_modify
                      (unit, nhi, &egr_nh_info,
                       _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW);
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_nh_add
 * Purpose:
 *    Add VXLAN Next Hop entry
 * Parameters:
 *    unit - (IN) Device Number
 *    vxlan_port - (IN/OUT) vxlan_port information (OUT : vxlan_port_id)
 *    vp   - (IN) vxlan virtual port
 *    vpn  - (IN) VPN instance ID
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_port_nh_add(
    int unit,
    bcm_vxlan_port_t *vxlan_port,
    int vp,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    int vfi_index = -1;
    int new_entry_type;
    egr_l3_next_hop_entry_t egr_nh_entry;
    bcmi_gh2_vxlan_nh_info_t egr_nh_info;
    uint32 mpath_flag = 0;
    int vp_nh_ecmp_index = -1;
    int ref_count = 0;
    int l3_flag = 0;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vpn_info_t *vfi_info;
    gh2_vxlan_vp_info_t *vp_info;

    /* Input parameter check */
    if (NULL == vxlan_port) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
    /* Get VFI info for VN_ID */
    vfi_info = &(vxlan_info->vxlan_vpn_info[vfi_index]);

    /* Get VP info for vfi_index */
    vp_info = &(vxlan_info->vxlan_vp_info[vp]);
    vp_info->vfi = vfi_index;

    sal_memset(&egr_nh_info, 0, sizeof(egr_nh_info));
    egr_nh_info.vnid = vfi_info->vnid;
    egr_nh_info.entry_type = -1;
    egr_nh_info.intf_num = -1;
    egr_nh_info.tunnel_index = -1;
    egr_nh_info.udp_src_port_range_enable = -1;
    egr_nh_info.payload_otag_delete = -1;
    egr_nh_info.use_nhi_in_evxlt_key = -1;
    egr_nh_info.apply_evxlt_modify_to_payload = -1;
    egr_nh_info.tunnel_otag_delete = -1;
    egr_nh_info.tunnel_itag_delete = -1;
    egr_nh_info.qos_map_id = -1;
    egr_nh_info.tunnel_pkt_pri = -1;
    egr_nh_info.tunnel_pkt_cfi = -1;
    egr_nh_info.custom_header_ipmc = -1;

    /*
     * Get egress next-hop index from egress object and
     * increase egress object reference count.
     */
    if (!(vxlan_port->flags & BCM_VXLAN_PORT_SHARE)) {
        rv = bcm_xgs3_get_nh_from_egress_object(unit, vxlan_port->egress_if,
                                                &mpath_flag, 1,
                                                &vp_nh_ecmp_index);
        BCM_IF_ERROR_RETURN(rv);

        /* Read the existing egress next_hop entry */
        if (mpath_flag == 0) {
            if (vxlan_port->flags & BCM_VXLAN_PORT_NETWORK) {
                l3_flag = BCM_XGS3_L3_ENT_FLAG(
                    BCM_XGS3_L3_TBL_PTR(unit, next_hop), vp_nh_ecmp_index);
                if (l3_flag != _BCM_L3_VXLAN_ONLY) {
                    if (vp_nh_ecmp_index != -1) {
                        /* Decrement reference count */
                        (void)bcm_xgs3_get_ref_count_from_nhi
                                    (unit, mpath_flag, &ref_count,
                                     vp_nh_ecmp_index);
                    }
                    return BCM_E_CONFIG;
                }
            }
        }

        sal_memset(&egr_nh_entry, 0, sizeof(egr_l3_next_hop_entry_t));
        rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                          vp_nh_ecmp_index, &egr_nh_entry);
        if (BCM_FAILURE(rv)) {
            if (vp_nh_ecmp_index != -1) {
                /* Decrement reference count */
                (void)bcm_xgs3_get_ref_count_from_nhi
                            (unit, mpath_flag, &ref_count,
                             vp_nh_ecmp_index);
            }
            return rv;
        }
    }

    if (vxlan_port->flags & BCM_VXLAN_PORT_EGRESS_TUNNEL) {
        /* Egress into Network - Tunnel */

        if (!(vxlan_port->flags & BCM_VXLAN_PORT_SHARE)) {
            if (vxlan_port->flags & BCM_VXLAN_PORT_MULTICAST) {
                /* GH2 doesn't support multicast VXLAN tunnel */
                rv = BCM_E_UNAVAIL;
            }

            /*
             * Select PPD type for Higig header:
             * - Must be BCM_VXLAN_PORT_ENCAP_LOCAL for GH2
             */
            if (!(vxlan_port->flags & BCM_VXLAN_PORT_ENCAP_LOCAL)) {
                rv = BCM_E_PARAM;
            }

            if (BCM_FAILURE(rv)) {
                if (vp_nh_ecmp_index != -1) {
                    /* Decrement reference count */
                    (void)bcm_xgs3_get_ref_count_from_nhi
                                (unit, mpath_flag, &ref_count,
                                 vp_nh_ecmp_index);
                }
                return rv;
            }
        }

        /* Program software Egress Tunnel info */
        rv = bcmi_gh2_vxlan_egress_tunnel_set(unit, vxlan_port, &egr_nh_info);
        if (BCM_FAILURE(rv)) {
            if (vp_nh_ecmp_index != -1) {
                /* Decrement reference count */
                (void)bcm_xgs3_get_ref_count_from_nhi
                            (unit, mpath_flag, &ref_count,
                             vp_nh_ecmp_index);
            }
            return rv;
        }

        /* Program software DVP to NHI/ECMP */
        rv = bcmi_gh2_vxlan_ingress_dvp_set(unit, vp, mpath_flag,
                                            vp_nh_ecmp_index, vxlan_port);
        if (BCM_FAILURE(rv)) {
            if (vp_nh_ecmp_index != -1) {
                /* Decrement reference count */
                (void)bcm_xgs3_get_ref_count_from_nhi
                            (unit, mpath_flag, &ref_count,
                             vp_nh_ecmp_index);
            }
            return rv;
        }

        new_entry_type = _BCM_VXLAN_EGR_NEXT_HOP_VXLAN_VIEW;
        if (mpath_flag == 0) {
            /* Configure inner payload/outer tunnel VLAN TAG setting */
            rv = bcmi_gh2_vxlan_vlan_tag_set(unit, vxlan_port,
                                             &egr_nh_info, vp_nh_ecmp_index);
            if (BCM_FAILURE(rv)) {
                if (vp_nh_ecmp_index != -1) {
                    /* Decrement reference count */
                    (void)bcm_xgs3_get_ref_count_from_nhi
                                (unit, mpath_flag, &ref_count,
                                 vp_nh_ecmp_index);
                }
                return rv;
            }

            rv = bcmi_gh2_vxlan_nexthop_entry_modify
                (unit, vp_nh_ecmp_index, &egr_nh_info, new_entry_type);
            if (BCM_FAILURE(rv)) {
                if (vp_nh_ecmp_index != -1) {
                    /* Decrement reference count */
                    (void)bcm_xgs3_get_ref_count_from_nhi
                                (unit, mpath_flag, &ref_count,
                                 vp_nh_ecmp_index);
                }
                return rv;
            }
        } else {
            /* ECMP */
            rv = bcmi_gh2_vxlan_ecmp_nexthop_entry_modify
                (unit, vxlan_port, &egr_nh_info, new_entry_type);
            if (BCM_FAILURE(rv)) {
                if (vp_nh_ecmp_index != -1) {
                    /* Decrement reference count */
                    (void)bcm_xgs3_get_ref_count_from_nhi
                                (unit, mpath_flag, &ref_count,
                                 vp_nh_ecmp_index);
                }
                return rv;
            }
        }
    } else {
        /* Egress into Access - Non Tunnel */

        /* Configure inner payload VLAN TAG setting */
        rv = bcmi_gh2_vxlan_vlan_tag_set(unit, vxlan_port,
                                         &egr_nh_info, vp_nh_ecmp_index);
        if (BCM_FAILURE(rv)) {
            if (vp_nh_ecmp_index != -1) {
                /* Decrement reference count */
                (void)bcm_xgs3_get_ref_count_from_nhi
                            (unit, mpath_flag, &ref_count,
                             vp_nh_ecmp_index);
            }
            return rv;
        }

        egr_nh_info.entry_type =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh_entry, ENTRY_TYPEf);
        if (vxlan_port->flags & BCM_VXLAN_PORT_LOOPBACK) {
            /* Loopback VP is only support when VXLAN RIOT is enabled */
            if ((egr_nh_info.entry_type !=
                 _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) &&
                (egr_nh_info.entry_type !=
                 _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW) &&
                (egr_nh_info.entry_type !=
                 _BCM_VXLAN_EGR_NEXT_HOP_IFP_ACTION_VIEW)) {
                if (vp_nh_ecmp_index != -1) {
                    /* Decrement reference count */
                    (void)bcm_xgs3_get_ref_count_from_nhi
                                (unit, mpath_flag, &ref_count,
                                 vp_nh_ecmp_index);
                }
                return BCM_E_PARAM;
            }

            new_entry_type = _BCM_VXLAN_EGR_NEXT_HOP_IFP_ACTION_VIEW;
        } else {
            if ((egr_nh_info.entry_type !=
                 _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) &&
                (egr_nh_info.entry_type !=
                 _BCM_VXLAN_EGR_NEXT_HOP_L3MC_VIEW)) {
                if (vp_nh_ecmp_index != -1) {
                    /* Decrement reference count */
                    (void)bcm_xgs3_get_ref_count_from_nhi
                                (unit, mpath_flag, &ref_count,
                                 vp_nh_ecmp_index);
                }
                return BCM_E_PARAM;
            }

            new_entry_type = egr_nh_info.entry_type;
        }

        /* Program software DVP to NHI/ECMP */
        rv = bcmi_gh2_vxlan_ingress_dvp_set(unit, vp, mpath_flag,
                                            vp_nh_ecmp_index, vxlan_port);
        if (BCM_FAILURE(rv)) {
            if (vp_nh_ecmp_index != -1) {
                /* Decrement reference count */
                (void)bcm_xgs3_get_ref_count_from_nhi
                            (unit, mpath_flag, &ref_count,
                             vp_nh_ecmp_index);
            }
            return rv;
        }

        /* Configure EGR Next Hop entry - Normal or L3MC view */
        rv = bcmi_gh2_vxlan_nexthop_entry_modify(unit, vp_nh_ecmp_index,
                                                 &egr_nh_info, new_entry_type);
        if (BCM_FAILURE(rv)) {
            if (vp_nh_ecmp_index != -1) {
                /* Decrement reference count */
                (void)bcm_xgs3_get_ref_count_from_nhi
                            (unit, mpath_flag, &ref_count,
                             vp_nh_ecmp_index);
            }
            return rv;
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_nh_delete
 * Purpose:
 *    Delete VXLAN Next Hop entry
 * Parameters:
 *    unit - (IN) Device Number
 *    vpn  - (IN) VPN instance ID
 *    vp   - (IN) vxlan virtual port
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_port_nh_delete(int unit, bcm_vpn_t vpn, int vp)
{
    uint32 flags = 0;
    int  ref_count = 0;
    uint32 vp_type = 0;
    int ecmp = -1;
    int nh_ecmp_index = -1;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vp_info_t *vp_info;

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    /* GH2 requires to retrieve vp to ecmp or nhi from software database */
    vp_info = &(vxlan_info->vxlan_vp_info[vp]);
    vp_type = vp_info->vp_type;
    ecmp = vp_info->ecmp;
    nh_ecmp_index = vp_info->nh_ecmp_index;

    if (ecmp) {
        flags = BCM_L3_MULTIPATH;
        /* Decrease reference count */
        BCM_IF_ERROR_RETURN(
            bcm_xgs3_get_ref_count_from_nhi(unit, flags,
                                            &ref_count, nh_ecmp_index));
        if (ref_count == 1) {
            if (vp_type == GH2_VXLAN_DEST_VP_TYPE_TUNNEL) {
                /* support ecmp on network port only */
                BCM_IF_ERROR_RETURN(
                    bcmi_gh2_vxlan_ecmp_port_egress_nexthop_reset
                    (unit, (nh_ecmp_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit)),
                     vp_type, vp, vpn));
            }
        }
    } else {
        if (nh_ecmp_index != 0) {
            /* Decrease reference count */
            BCM_IF_ERROR_RETURN(
                bcm_xgs3_get_ref_count_from_nhi(unit, flags,
                                                &ref_count, nh_ecmp_index));
        }

        if (ref_count == 1) {
            if (nh_ecmp_index) {
                BCM_IF_ERROR_RETURN(
                    bcmi_gh2_vxlan_port_egress_nexthop_reset
                        (unit, nh_ecmp_index, vp_type, vp, vpn));
            }
        } else if (ref_count == 2) {
            /*
             * Need to check if the NHI is used for ECMP group only when
             * (ref_count == 2), if TRUE, we must reset VXLAN Egress NextHop.
             */
            if (nh_ecmp_index) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_l3_egress_ecmp_traverse
                        (unit, bcmi_gh2_vxlan_nexthop_ecmp_find,
                         (void *)&nh_ecmp_index));
            }
        }
    }

    /* Decrement VXLAN outer TPID ref count */
    if (vp_info->tpid_valid) {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_outer_tpid_ref_dec(unit, vp_info->tpid_value));
    }

    /* Clear software database for vp_info */
    vp_info->vp_type = GH2_VXLAN_DEST_VP_TYPE_INVALID;
    vp_info->vfi = -1;
    vp_info->ecmp = 0;
    vp_info->nh_ecmp_index = 0;
    vp_info->tpid_valid= 0;
    vp_info->tpid_value = 0x0;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_nh_get
 * Purpose:
 *    Get VXLAN Next Hop entry
 * Parameters:
 *    unit - (IN) Device Number
 *    vpn  - (IN) VPN instance ID
 *    vp   - (IN) vxlan virtual port
 *    vxlan_port - (IN/OUT) vxlan_port information
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_port_nh_get(
    int unit,
    bcm_vpn_t vpn,
    int vp,
    bcm_vxlan_port_t *vxlan_port)
{
    int nh_ecmp_index = 0, tunnel_index = 0, udp_src_port_range_enable = 0;
    uint32 ecmp = 0;
    uint32 entry_type;
    egr_l3_next_hop_entry_t egr_nh_entry;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vp_info_t *vp_info;

    /* Input parameter check */
    if (NULL == vxlan_port) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    /* Clear egr_nh_entry */
    sal_memset(&egr_nh_entry, 0, sizeof(egr_nh_entry));

    /* Get SW VP info */
    vp_info = &(vxlan_info->vxlan_vp_info[vp]);

    if (vp_info->vp_type == GH2_VXLAN_DEST_VP_TYPE_TUNNEL) {
        /* Egress into VXLAN tunnel, find the tunnel_if */
        vxlan_port->flags |= BCM_VXLAN_PORT_EGRESS_TUNNEL;

        ecmp = vp_info->ecmp;
        nh_ecmp_index = vp_info->nh_ecmp_index;
        if (ecmp) {
            /* Extract next hop index from multipath egress object */
            vxlan_port->egress_if =
                (nh_ecmp_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit));

            /* Get ecmp's first member nexthop index */
            BCM_IF_ERROR_RETURN(
                bcmi_gh2_vxlan_ecmp_member_egress_get_first
                    (unit, vxlan_port->egress_if, &nh_ecmp_index));
        } else {
            /* Extract next hop index from unipath egress object */
            vxlan_port->egress_if =
                (nh_ecmp_index + BCM_XGS3_EGRESS_IDX_MIN(unit));
        }

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                         nh_ecmp_index, &egr_nh_entry));

    } else if (vp_info->vp_type == GH2_VXLAN_DEST_VP_TYPE_ACCESS) {
        /* Egress into Access-side, find the tunnel_if */
        nh_ecmp_index = vp_info->nh_ecmp_index;

        /* Extract next hop index from unipath egress object. */
        vxlan_port->egress_if =
            (nh_ecmp_index + BCM_XGS3_EGRESS_IDX_MIN(unit));

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                         nh_ecmp_index, &egr_nh_entry));
    } else {
        return BCM_E_NOT_FOUND;
    }

    entry_type = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh_entry,
                                     ENTRY_TYPEf);
    if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_VXLAN_VIEW) {
        /* Get Tunnel index */
        tunnel_index =
            soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh_entry, VXLAN__TUNNEL_INDEXf);

        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_egress_tunnel_get(unit, vxlan_port, tunnel_index));

        /* Get UDP source port range checking for Entropy */
        udp_src_port_range_enable =
            soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh_entry, VXLAN__UDP_SRC_PORT_RANGE_ENf);
        if (udp_src_port_range_enable) {
            vxlan_port->flags |= BCM_VXLAN_PORT_UDP_SRC_PORT_RANGE_ENABLE;
        }
    }

    if (vp_info->vp_type == GH2_VXLAN_DEST_VP_TYPE_TUNNEL) {
        /* Must be BCM_VXLAN_PORT_ENCAP_LOCAL for GH2 */
        vxlan_port->flags |= BCM_VXLAN_PORT_ENCAP_LOCAL;
    }

    /* Get VXLAN vlan tag info */
    (void)bcmi_gh2_vxlan_vlan_tag_get(unit, vxlan_port,
                                      &egr_nh_entry, nh_ecmp_index);

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_untagged_profile_set
 * Purpose:
 *    Set  VLan profile per Physical port entry
 * Parameters:
 *    unit - (IN) Device Number
 *    port - (IN) Physical port
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_port_untagged_profile_set(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t action;
    uint32 ing_profile_idx = 0xffffffff;

    bcm_vlan_action_set_t_init(&action);
    action.ut_outer = 0x0; /* No Op */
    action.ut_inner = 0x0; /* No Op */

    rv = _bcm_trx_vlan_action_profile_entry_add(unit, &action,
                                                &ing_profile_idx);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                               TAG_ACTION_PROFILE_PTRf, ing_profile_idx);

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_untagged_profile_reset
 * Purpose:
 *    Reset  VLan profile per Physical port entry
 * Parameters:
 *    unit - (IN) Device Number
 *    port  - (IN) Physical port
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_port_untagged_profile_reset(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;
    int ing_profile_idx = -1;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_tab_get(unit, port, TAG_ACTION_PROFILE_PTRf,
                              &ing_profile_idx));

    rv = _bcm_trx_vlan_action_profile_entry_delete(unit, ing_profile_idx);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                               TAG_ACTION_PROFILE_PTRf, 0);

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_match_vxlate_entry_set
 * Purpose:
 *    Set VXLAN Match Vxlate Entry
 * Parameters:
 *    unit - (IN) Device Number
 *    vpn  - (IN) VPN instance ID
 *    vxlan_port - (IN/OUT) vxlan_port information (OUT : vxlan_port_id)
 *    vent - (IN) vlan xlate entry pointer
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_match_vxlate_entry_set(
    int unit,
    bcm_vpn_t vpn,
    bcm_vxlan_port_t *vxlan_port,
    uint32 *vent)
{
    int rv = BCM_E_NONE;
    int index = -1, added_index = -1;
    soc_mem_t mem;
    uint32 return_vent[SOC_MAX_MEM_WORDS];
    int vfi = 0;

    mem = VLAN_XLATEm;

    soc_mem_field32_set(unit, mem, vent, VALIDf, 0x1);
    soc_mem_field32_set(unit, mem, vent, XLATE__MPLS_ACTIONf, 0x5); /* VFI */
    soc_mem_field32_set(unit, mem, vent, XLATE__VFI_VALIDf, 0x1);
    if (!(vxlan_port->flags & BCM_VXLAN_PORT_ENABLE_VLAN_CHECKS)) {
        soc_mem_field32_set(unit, mem, vent, XLATE__DISABLE_VLAN_CHECKSf, 0x1);
    }

    _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
    soc_mem_field32_set(unit, mem, vent, XLATE__VFIf, vfi);

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY,
                        &index, vent, return_vent, 0);
    if (BCM_SUCCESS(rv)) {
        /* Existing VLAN_XLATE entry is found */
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, vent));

        if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
            added_index = index;
        }
    } else if (rv == BCM_E_NOT_FOUND) {
        /* New VLAN_XLATE entry to be inserted */
        if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
            return BCM_E_NOT_FOUND;
        }
        BCM_IF_ERROR_RETURN(
            soc_mem_insert(unit, mem, MEM_BLOCK_ALL, vent));

        /* Search inserted VLAN_XLATE entry index */
        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY,
                            &index, vent, return_vent, 0);
        added_index = index;
    } else {
        return rv;
    }

    /* Increase ref_cnt for the specified VXLAN Vlan XLATE entry  */
    if (added_index != -1) {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_vlan_xlate_ref_inc(unit, added_index));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_match_vxlate_entry_reset
 * Purpose:
 *    Reset VXLAN Match Vxlate Entry
 * Parameters:
 *    unit - (IN) Device Number
 *    vpn  - (IN) VPN instance ID
 *    vxlan_port - (IN/OUT) vxlan_port information (OUT : vxlan_port_id)
 *    vent - (IN) vlan xlate entry pointer
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_match_vxlate_entry_reset(
    int unit,
    uint32 *vent)
{
    int rv = BCM_E_NONE;
    int index = -1, deleted_index = -1;
    soc_mem_t mem;
    uint32 return_vent[SOC_MAX_MEM_WORDS];
    int ref_cnt = 0;

    mem = VLAN_XLATEm;

    soc_mem_field32_set(unit, mem, vent, VALIDf, 0x1);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY,
                        &index, vent, return_vent, 0);
    if (BCM_SUCCESS(rv)) {
        /* Existing VLAN_XLATE entry is found */
        deleted_index = index;
    } else {
        return rv;
    }

    /* Decrease ref_cnt for the specified VXLAN Vlan XLATE entry  */
    if (deleted_index != -1) {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_vlan_xlate_ref_dec(unit, deleted_index, &ref_cnt));

        if (ref_cnt == 0) {
            BCM_IF_ERROR_RETURN(
                soc_mem_delete(unit, mem, MEM_BLOCK_ALL, vent));
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_trunk_table_set
 * Purpose:
 *    Configure VXLAN Trunk port entry
 * Parameters:
 *    unit - (IN) Device Number
 *    port   - (IN) Trunk member port
 *    tgid - (IN) Trunk group Id
 *    vpn  - (IN) VPN instance ID
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_trunk_table_set(
    int unit,
    bcm_port_t port,
    bcm_trunk_t tgid,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    source_trunk_map_table_entry_t trunk_map_entry;
    bcm_module_t my_modid;
    int src_trk_idx = -1;
    int port_type;
    int vfi = 0;

    if (tgid != BCM_TRUNK_INVALID) {
        port_type = 1; /* TRUNK_PORT_TYPE */
    } else {
        return BCM_E_PARAM;
    }

    if (vpn != BCM_VXLAN_VPN_INVALID) {
        _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
        if (!bcmi_gh2_vxlan_vfi_used_get(unit, vfi)) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Get module id for unit */
    BCM_IF_ERROR_RETURN(
        bcm_esw_stk_my_modid_get(unit, &my_modid));

    /* Get index to source trunk map table */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                                              port, &src_trk_idx));

    soc_mem_lock(unit, SOURCE_TRUNK_MAP_TABLEm);

    /* Read source trunk map table */
    rv = soc_mem_read(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ANY,
                      src_trk_idx, &trunk_map_entry);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
        return rv;
    }

    /* Set trunk group id */
    soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, &trunk_map_entry,
                        TGIDf, tgid);

    /* Set port is part of Trunk group */
    soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, &trunk_map_entry,
                        PORT_TYPEf, port_type);

    /* Set vfi to provide default VFI (for untag & ptag packets) */
    soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, &trunk_map_entry,
                        VFIf, vfi);

    /* Disable VLAN check for SOURCE_TRUNK_MAP_TABLEm */
    soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, &trunk_map_entry,
                        DISABLE_VLAN_CHECKSf, 1);

    /* Write entry to hw. */
    rv = soc_mem_write(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ALL,
                       src_trk_idx, &trunk_map_entry);

    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);

    return rv;
}


/*
 * Function:
 *    bcmi_gh2_vxlan_trunk_table_reset
 * Purpose:
 *    Reset VXLAN Trunk port entry
 * Parameters:
 *    unit    - (IN) Device Number
 *    port   - (IN) Trunk member port
 * Returns:
 *    BCM_E_XXX
 */

STATIC int
bcmi_gh2_vxlan_trunk_table_reset(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;
    source_trunk_map_table_entry_t trunk_map_entry;
    bcm_module_t my_modid;
    int src_trk_idx = -1;

    /* Get module id for unit. */
    BCM_IF_ERROR_RETURN(
        bcm_esw_stk_my_modid_get(unit, &my_modid));

    /* Get index to source trunk map table */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                                              port, &src_trk_idx));

    soc_mem_lock(unit, SOURCE_TRUNK_MAP_TABLEm);

    /* Read source trunk map table */
    rv = soc_mem_read(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ANY,
                      src_trk_idx, &trunk_map_entry);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
        return rv;
    }

    /* Set vfi */
    soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, &trunk_map_entry,
                        VFIf, 0);

    soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, &trunk_map_entry,
                        DISABLE_VLAN_CHECKSf, 0);

    /* Write entry to hw */
    rv = soc_mem_write(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ALL,
                       src_trk_idx, &trunk_map_entry);

    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_trunk_member_delete
 * Purpose:
 *    Clear Trunk Port-membership when the members to be deleted.
 * Parameters:
 *    unit - (IN) Device Number
 *    tgid - (IN)  Trunk Group ID
 *    trunk_member_count - (IN) Count of Trunk members to be deleted
 *    trunk_member_array - (IN) Trunk member gports to be deleted
*/
int
bcmi_gh2_vxlan_trunk_member_delete(
    int unit,
    bcm_trunk_t trunk_id,
    int trunk_member_count,
    bcm_port_t *trunk_member_array)
{
    int rv = BCM_E_NONE, rv2 = BCM_E_NONE;
    int idx = 0;
    int value;
    bcm_port_t local_member;

    if (NULL == trunk_member_array) {
        return BCM_E_PARAM;
    }

    rv = _bcm_trunk_id_validate(unit, trunk_id);
    if (BCM_FAILURE(rv)) {
        return BCM_E_PORT;
    }

    for (idx = 0; idx < trunk_member_count; idx++) {
        rv2 = bcm_esw_port_local_get(unit, trunk_member_array[idx],
                                     &local_member);
        /* Only support trunk local ports for trunk match */
        if (BCM_FAILURE(rv2)) {
            continue;
        }
        (void)_bcm_esw_port_tab_get(unit, local_member,
                                    PORT_OPERATIONf, &value);
        if (value == GH2_VXLAN_PORT_OPERATION_VFI) {
            (void)_bcm_esw_port_tab_set(unit, local_member,
                                        _BCM_CPU_TABS_NONE, PORT_OPERATIONf,
                                        GH2_VXLAN_PORT_OPERATION_NORMAL);
            BCM_IF_ERROR_RETURN(
                bcmi_gh2_vxlan_trunk_table_reset(unit, local_member));
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_riot_custom_header_add
 * Purpose:
 *    Add Custom Header entry for 2-Pass VXLAN RIOT support.
 * Parameters:
 *    unit           - (IN) Device Number
 *    vxlan_port     - (IN) vxlan_port information
 *    old_vxlan_port - (IN) old vxlan_port information (for replace)
 *    vp             - (IN) Source Virtual Port
 * Returns:
 *    BCM_X_XXX
 */
STATIC int
bcmi_gh2_vxlan_riot_custom_header_add(
    int unit,
    bcm_vxlan_port_t *vxlan_port,
    bcm_vxlan_port_t *old_vxlan_port,
    int vp)
{
    int rv = BCM_E_NONE;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vp_info_t *vp_info;
    bcm_switch_match_control_info_t match_control_info;
    bcm_switch_encap_info_t encap_info;
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    bcm_trunk_t trunk_id = -1;
    int gport_id = -1;
    int local_port_out[SOC_MAX_NUM_PORTS];
    int local_member_count = 0;
    int port_idx = 0;
    bcm_if_t encap_id;
    int index_array[2];
    bcm_module_t lmodid;
    int egr_gpp_index = 0;
    egr_gpp_attributes_entry_t egr_gpp_entry;
    int i;
    int is_ipmc;
    int match_id_ipmc, match_id_non_ipmc;
    int *match_id_ptr;
    bcm_switch_match_config_info_t match_info;
    bcm_switch_match_service_t match_service;
    uint32 match_id_hw;
    custom_header_policy_table_entry_t policy_entry;
    int mode = 0;
    int qos_map_ptr = 0;

    /* Input parameter check */
    if ((NULL == vxlan_port) || (NULL == old_vxlan_port)) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
        /* Delete old VP related Custom Header configurations */
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_riot_custom_header_delete(unit,
                                                     old_vxlan_port, vp));
    }

    /*
     * Set Custom Header match mask value (global) when
     * VXLAN RIOT enabled (for GH2-B0 only)
     */
    if (vxlan_port->flags & BCM_VXLAN_PORT_LOOPBACK) {
        /* Get current Custom Header match mask value (global) */
        bcm_switch_match_control_info_t_init(&match_control_info);

        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_match_control_get(
                unit, bcmSwitchMatchServiceCustomHeader,
                bcmSwitchMatchControlMatchMask,
                0, &match_control_info));

        if (match_control_info.mask32 != GH2_VXLAN_RIOT_CH_HEADER_MASK) {
            bcm_switch_match_control_info_t_init(&match_control_info);
            match_control_info.mask32 = GH2_VXLAN_RIOT_CH_HEADER_MASK;

            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_match_control_set(
                    unit, bcmSwitchMatchServiceCustomHeader,
                    bcmSwitchMatchControlMatchMask,
                    0, &match_control_info));
        }
    } else {
        /*
         * Create one dummy Custom Header encap entry with index = 0 :
         * - the Custom Header index could not be 0 when using
         *   the EGR_GPP_ATTRIBUTES
         */
        index_array[0] = 0; /* hw index = 0 */
        BCM_IF_ERROR_RETURN(
            _bcm_hr3_switch_encap_idx2id(unit,
                                         _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER,
                                         1, index_array, &encap_id));

        bcm_switch_encap_info_t_init(&encap_info);
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_encap_get(unit, encap_id, &encap_info));

        if (encap_info.value32 == 0) {
            bcm_switch_encap_info_t_init(&encap_info);
            encap_info.encap_service = BCM_SWITCH_ENCAP_SERVICE_CUSTOM_HEADER;
            encap_info.value32 = GH2_VXLAN_RIOT_CH_HEADER_MASK;
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_encap_create(unit, &encap_info, &encap_id));

            vxlan_info->riot_custom_header_index0_ref_count++;
        } else if (encap_info.value32 == GH2_VXLAN_RIOT_CH_HEADER_MASK) {
            vxlan_info->riot_custom_header_index0_ref_count++;
        }
    }

    /*
     * For all VXLAN RIOT flow support (loopback, access or network ports),
     * we must use vxlan_port->match_port for Custom Header related
     * configurations
     */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, vxlan_port->match_port, &mod_out,
                               &port_out, &trunk_id, &gport_id));

    sal_memset(local_port_out, 0, sizeof(local_port_out));
    if (BCM_GPORT_IS_TRUNK(vxlan_port->match_port)) {
        /* Trunk port */
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_local_members_get(unit, trunk_id,
                                             SOC_MAX_NUM_PORTS,
                                             local_port_out,
                                             &local_member_count));

        if ((vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_NONE) ||
            (vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_VN_ID)) {
            vxlan_info->match_key[vp].trunk_id = trunk_id;
            vxlan_info->match_key[vp].modid = -1;
        }
    } else {
        /* Local port */
        local_port_out[0] = port_out;
        local_member_count++;

        if ((vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_NONE) ||
            (vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_VN_ID)) {
            vxlan_info->match_key[vp].port = port_out;
            vxlan_info->match_key[vp].modid = mod_out;
            vxlan_info->match_key[vp].trunk_id = -1;
        }
    }

    /* Get current MY_MODID */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &lmodid));

    vp_info = &(vxlan_info->vxlan_vp_info[vp]);
    for (port_idx = 0; port_idx < local_member_count; port_idx++) {
        if (vxlan_port->flags & BCM_VXLAN_PORT_LOOPBACK) {
            /*
             * For Loopback Ports:
             * - Enable Custom Header match/encap on loopbcak port
             */

            /* Enable the capability of Custom Header match detection */
            bcm_switch_match_control_info_t_init(&match_control_info);
            match_control_info.port_enable = TRUE;
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_match_control_set(
                    unit, bcmSwitchMatchServiceCustomHeader,
                    bcmSwitchMatchControlPortEnable,
                    local_port_out[port_idx], &match_control_info));

            /* Enable the capability of Custom Header encapsulation */
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_control_set(
                    unit, local_port_out[port_idx],
                    bcmPortControlCustomHeaderEncapEnable, TRUE));
        } else {
            /*
             * For Access or Network Ports:
             * - Prepare Custom Header encap, match and policy entries:
             *   Encap - 1 entry per access/network port
             *   Match and Policy - 2 entries for each table
             */

            /* Encap: Create one Custom Header encap */
            bcm_switch_encap_info_t_init(&encap_info);
            encap_info.encap_service = BCM_SWITCH_ENCAP_SERVICE_CUSTOM_HEADER;
            encap_info.value32 =
                GH2_VXLAN_RIOT_CH_ENCAP_VALUE(0, lmodid,
                                              local_port_out[port_idx]);
            rv = bcm_esw_switch_encap_create(unit, &encap_info, &encap_id);
            if (BCM_SUCCESS(rv) || rv == BCM_E_EXISTS) {
                if ((encap_id >> _BCM_SWITCH_ENCAP_SHIFT) ==
                    _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_hr3_switch_encap_id2idx(unit, encap_id, 1,
                                                     index_array));
                } else {
                    return BCM_E_INTERNAL;
                }

                if (rv == BCM_E_EXISTS) {
                    /* Increase ref_cnt */
                    _bcm_common_profile_mem_ref_cnt_update
                        (unit, EGR_HEADER_ENCAP_DATAm, index_array[0], 1);
                }
            } else {
                return rv;
            }

            /* Encap: Program EGR_GPP_ATTRIBUTES table with created encap_id */
            /*
             * Assign Custom Header index using the EGR_GPP_ATTRIBUTES:
             * - egr_gpp_index = EGR_GPP_ATTRIBUTES_MODBASE[my_modid].BASE +
             *                   source tgid or egress port
             * - EGR_GPP_ATTRIBUTES[egr_gpp_index].CUSTOM_HEADER_INDEXf =
             *   index_array[0] (Custom Header index)
             */
            BCM_IF_ERROR_RETURN(
                _bcm_esw_src_mod_port_table_index_get(unit, lmodid,
                                                      local_port_out[port_idx],
                                                      &egr_gpp_index));
            BCM_IF_ERROR_RETURN(
                READ_EGR_GPP_ATTRIBUTESm(unit, MEM_BLOCK_ANY,
                                         egr_gpp_index, &egr_gpp_entry));
            soc_mem_field32_set(unit, EGR_GPP_ATTRIBUTESm, &egr_gpp_entry,
                                CUSTOM_HEADER_INDEXf, (uint32)index_array[0]);
            BCM_IF_ERROR_RETURN(
                WRITE_EGR_GPP_ATTRIBUTESm(unit, MEM_BLOCK_ALL,
                                          egr_gpp_index, &egr_gpp_entry));

            vp_info->ch_encap_id = encap_id;


            /* QoS mapping: get current DSCP map mode on this port */
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_dscp_map_mode_get(unit,
                                               local_port_out[port_idx],
                                               &mode));
            if (mode == BCM_PORT_DSCP_MAP_NONE) {
                BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set
                    (unit, local_port_out[port_idx], _BCM_CPU_TABS_NONE,
                     USE_CUSTOM_HEADER_FOR_PRIf,
                     GH2_VXLAN_RIOT_CH_COS_REMAP_INT_PRI_MAP));

                BCM_IF_ERROR_RETURN(
                    _bcm_esw_port_tab_get(unit, local_port_out[port_idx],
                                          TRUST_DOT1P_PTRf, &qos_map_ptr));
            } else {
                /* trust incoming IPv4/IPv6 DSCP */
                BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set
                    (unit, local_port_out[port_idx], _BCM_CPU_TABS_NONE,
                     USE_CUSTOM_HEADER_FOR_PRIf,
                     GH2_VXLAN_RIOT_CH_COS_REMAP_DSCP_QOS_MAP));

                qos_map_ptr = local_port_out[port_idx];
            }

            /* Match and Policy: Setup two Custom Header match and policy */
            for (i = 0; i < 2; i++) {
                is_ipmc = i; /* i = 0: non-IPMC, i = 1: IPMC */

                /* Setup Custom Header match */
                match_id_ptr = (is_ipmc) ? &match_id_ipmc : &match_id_non_ipmc;

                bcm_switch_match_config_info_t_init(&match_info);
                match_info.value32 =
                    GH2_VXLAN_RIOT_CH_ENCAP_VALUE((is_ipmc) ? 1 : 0, lmodid,
                                                  local_port_out[port_idx]);
                rv = bcm_esw_switch_match_config_add
                         (unit, bcmSwitchMatchServiceCustomHeader,
                          &match_info, match_id_ptr);
                if (BCM_SUCCESS(rv) || rv == BCM_E_EXISTS) {
                    BCM_IF_ERROR_RETURN(
                        bcmi_switch_match_hw_id_get(
                            unit, *match_id_ptr, &match_service,
                            &match_id_hw));
                    if (match_service != bcmSwitchMatchServiceCustomHeader) {
                        return BCM_E_INTERNAL;
                    }

                    if (rv == BCM_E_EXISTS) {
                        /* Increase ref_cnt */
                        _bcm_common_profile_mem_ref_cnt_update
                            (unit, CUSTOM_HEADER_MATCHm, (int)match_id_hw, 1);
                    }
                } else {
                    return rv;
                }

                /* Setup Custom Header policy table */
                sal_memset(&policy_entry, 0, sizeof(policy_entry));

                SOC_IF_ERROR_RETURN(
                    soc_mem_read(unit, CUSTOM_HEADER_POLICY_TABLEm,
                                 MEM_BLOCK_ANY, match_id_hw, &policy_entry));

                soc_mem_field32_set(unit, CUSTOM_HEADER_POLICY_TABLEm,
                                    &policy_entry, VALIDf, 1);
                soc_mem_field32_set(unit, CUSTOM_HEADER_POLICY_TABLEm,
                                    &policy_entry, REPLACE_SGPPf, 1);
                soc_mem_field32_set(unit, CUSTOM_HEADER_POLICY_TABLEm,
                                    &policy_entry, VFI_INVALIDf,
                                    (is_ipmc) ? 1 : 0);

                /* Setup Custom Header QoS mapping */
                soc_mem_field32_set(unit, CUSTOM_HEADER_POLICY_TABLEm,
                                    &policy_entry, QOS_MAPPING_PTRf,
                                    qos_map_ptr);

                SOC_IF_ERROR_RETURN(
                    soc_mem_write(unit, CUSTOM_HEADER_POLICY_TABLEm,
                                  MEM_BLOCK_ALL, match_id_hw, &policy_entry));
            }

            vp_info->ch_match_id_ipmc = match_id_ipmc;
            vp_info->ch_match_id_non_ipmc = match_id_non_ipmc;
        }

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_riot_custom_header_delete
 * Purpose:
 *    Delete Custom Header entry for 2-Pass VXLAN RIOT support.
 * Parameters:
 *    unit       - (IN) Device Number
 *    vxlan_port - (IN) vxlan_port information
 *    vp         - (IN) Source Virtual Port
 * Returns:
 *    BCM_X_XXX
 */
STATIC int
bcmi_gh2_vxlan_riot_custom_header_delete(
    int unit,
    bcm_vxlan_port_t *vxlan_port,
    int vp)
{
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vp_info_t *vp_info;
    _bcm_port_info_t *port_info;
    bcm_switch_match_control_info_t match_control_info;
    bcm_switch_encap_info_t encap_info;
    int index_array[2];
    bcm_if_t encap_id;
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    bcm_trunk_t trunk_id = -1;
    int gport_id = -1;
    int local_port_out[SOC_MAX_NUM_PORTS];
    int local_member_count = 0;
    int port_idx = 0;
    int i;
    int is_ipmc;
    int match_id_ptr;
    bcm_switch_match_service_t match_service;
    uint32 match_id_hw;
    custom_header_policy_table_entry_t policy_entry;
    bcm_module_t lmodid;
    int egr_gpp_index = 0;
    egr_gpp_attributes_entry_t egr_gpp_entry;
    int ref_count = 0;

    /* Input parameter check */
    if (NULL == vxlan_port) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    /*
     * Delete dummy Custom Header encap entry with index = 0 :
     * - the Custom Header index could not be 0 when using
     *   the EGR_GPP_ATTRIBUTES
     */
    if (!(vxlan_port->flags & BCM_VXLAN_PORT_LOOPBACK)) {
        index_array[0] = 0; /* hw index = 0 */
        BCM_IF_ERROR_RETURN(
            _bcm_hr3_switch_encap_idx2id(unit,
                                         _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER,
                                         1, index_array, &encap_id));

        bcm_switch_encap_info_t_init(&encap_info);
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_encap_get(unit, encap_id, &encap_info));

        if (encap_info.value32 == GH2_VXLAN_RIOT_CH_HEADER_MASK) {
            if (vxlan_info->riot_custom_header_index0_ref_count >= 1) {
                vxlan_info->riot_custom_header_index0_ref_count--;
            }

            if (vxlan_info->riot_custom_header_index0_ref_count == 0) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_switch_encap_destroy(unit, encap_id));
            }
        }
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, vxlan_port->match_port, &mod_out,
                               &port_out, &trunk_id, &gport_id));

    sal_memset(local_port_out, 0, sizeof(local_port_out));
    if (BCM_GPORT_IS_TRUNK(vxlan_port->match_port)) {
        /* Trunk port */
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_local_members_get(unit, trunk_id,
                                             SOC_MAX_NUM_PORTS,
                                             local_port_out,
                                             &local_member_count));
    } else {
        /* Local port */
        local_port_out[0] = port_out;
        local_member_count++;
    }

    vp_info = &(vxlan_info->vxlan_vp_info[vp]);
    for (port_idx = 0; port_idx < local_member_count; port_idx++) {
        /* Get current VP count on this local port */
        _bcm_port_info_access(unit, local_port_out[port_idx], &port_info);

        if (vxlan_port->flags & BCM_VXLAN_PORT_LOOPBACK) {
                /*
                 * For Loopback Ports:
                 * - Disable Custom Header match/encap when (vp_count == 1)
                 *   on loopbcak port
                 */
            if (port_info->vp_count == 1) {
                /* Disable the capability of Custom Header match detection */
                bcm_switch_match_control_info_t_init(&match_control_info);
                match_control_info.port_enable = FALSE;
                BCM_IF_ERROR_RETURN(
                    bcm_esw_switch_match_control_set(
                        unit, bcmSwitchMatchServiceCustomHeader,
                        bcmSwitchMatchControlPortEnable,
                        local_port_out[port_idx], &match_control_info));

                /* Disable the capability of Custom Header encapsulation */
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_control_set(
                        unit, local_port_out[port_idx],
                        bcmPortControlCustomHeaderEncapEnable, FALSE));
            }
        } else {
            /*
             * For Access or Network Ports:
             * - Destroy Custom Header encap, match and policy entries:
             *   Encap - 1 entry per access/network port
             *   Match and Policy - 2 entries for each table
             */

            /* Encap: Delete Custom Header encap */
            encap_id = vp_info->ch_encap_id;
            if ((encap_id >> _BCM_SWITCH_ENCAP_SHIFT) ==
                _BCM_SWITCH_ENCAP_TYPE_CUSTOM_HEADER) {
                BCM_IF_ERROR_RETURN(
                    _bcm_hr3_switch_encap_id2idx(unit, encap_id, 1,
                                                 index_array));
            } else {
                return BCM_E_INTERNAL;
            }

            BCM_IF_ERROR_RETURN(
                _bcm_egr_header_encap_data_entry_ref_count_get
                    (unit, (uint32)index_array[0], &ref_count));
            if (ref_count >= 2) {
                /* Decrease ref_cnt */
                _bcm_common_profile_mem_ref_cnt_update
                    (unit, EGR_HEADER_ENCAP_DATAm, index_array[0], -1);
            } else {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_switch_encap_destroy(unit, encap_id));
            }

            /* Match and Policy: Delete two Custom Header match and policy */
            for (i = 0; i < 2; i++) {
                is_ipmc = i; /* i = 0: non-IPMC, i = 1: IPMC */

                /* Setup Custom Header match */
                match_id_ptr = (is_ipmc) ? vp_info->ch_match_id_ipmc :
                                           vp_info->ch_match_id_non_ipmc;

                BCM_IF_ERROR_RETURN(
                    bcmi_switch_match_hw_id_get(
                        unit, match_id_ptr, &match_service, &match_id_hw));
                if (match_service != bcmSwitchMatchServiceCustomHeader) {
                    return BCM_E_INTERNAL;
                }

                BCM_IF_ERROR_RETURN(
                    _bcm_custom_header_match_entry_ref_count_get
                        (unit, match_id_hw, &ref_count));
                if (ref_count >= 2) {
                    /* Decrease ref_cnt */
                    _bcm_common_profile_mem_ref_cnt_update
                        (unit, CUSTOM_HEADER_MATCHm, (int)match_id_hw, -1);
                } else {
                    BCM_IF_ERROR_RETURN(
                        bcm_esw_switch_match_config_delete(
                            unit, bcmSwitchMatchServiceCustomHeader,
                            match_id_ptr));
                }

                /* Clear Custom Header policy table when (vp_count == 1) */
                if (port_info->vp_count == 1) {
                    sal_memset(&policy_entry, 0, sizeof(policy_entry));
                    SOC_IF_ERROR_RETURN(
                        soc_mem_write(unit, CUSTOM_HEADER_POLICY_TABLEm,
                                      MEM_BLOCK_ALL, match_id_hw,
                                      &policy_entry));
                }
            }

            if (port_info->vp_count == 1) {
                /* Encap: Reset related EGR_GPP_ATTRIBUTES table */
                BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &lmodid));
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_src_mod_port_table_index_get
                        (unit, lmodid, local_port_out[port_idx],
                         &egr_gpp_index));
                BCM_IF_ERROR_RETURN(
                    READ_EGR_GPP_ATTRIBUTESm(unit, MEM_BLOCK_ANY,
                                             egr_gpp_index, &egr_gpp_entry));
                soc_mem_field32_set(unit, EGR_GPP_ATTRIBUTESm, &egr_gpp_entry,
                                    CUSTOM_HEADER_INDEXf, 0x0);
                BCM_IF_ERROR_RETURN(
                    WRITE_EGR_GPP_ATTRIBUTESm(unit, MEM_BLOCK_ALL,
                                              egr_gpp_index, &egr_gpp_entry));

                /* Reset Custom Header QoS mapping on this port */
                BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set
                    (unit, local_port_out[port_idx], _BCM_CPU_TABS_NONE,
                     USE_CUSTOM_HEADER_FOR_PRIf,
                     GH2_VXLAN_RIOT_CH_COS_REMAP_NONE));
            }
        }
    }
    vp_info->ch_encap_id = 0;
    vp_info->ch_match_id_ipmc = 0;
    vp_info->ch_match_id_non_ipmc = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_match_trunk_cleanup
 * Purpose:
 *    Helper function to clearnup VXLAN match trunk
 * Parameters:
 *    unit - (IN) Device Number
 *    tgid - (IN) Trunk group Id
 *    vpn  - (IN) VPN instance ID
 *    port_out   - (IN) member ports array in trunk
 *    port_count - (IN) trunk member port count
 *    port_op    - (IN) Port operation for Normal/VFI
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
 bcmi_gh2_vxlan_match_trunk_cleanup(
    int unit,
    bcm_trunk_t tgid,
    bcm_vpn_t vpn,
    int *port_out,
    int port_count,
    uint32 port_op)
{
    int rv = BCM_E_NONE;
    int port_idx = 0;
    int *local_port_out = NULL;

    /* Input parameter check */
    if (NULL == port_out) {
        return BCM_E_PARAM;
    }

    local_port_out = port_out;
    port_idx = port_count;
    if (port_op == GH2_VXLAN_PORT_OPERATION_NORMAL) {
        for (; port_idx >= 0; port_idx--) {
            (void)_bcm_esw_port_tab_set(unit, local_port_out[port_idx],
                                        _BCM_CPU_TABS_NONE, PORT_OPERATIONf,
                                        port_op);

            (void)bcmi_gh2_vxlan_trunk_table_reset(unit, local_port_out[port_idx]);
        }
    } else if (port_op == GH2_VXLAN_PORT_OPERATION_VFI) {
        for (; port_idx >= 0; port_idx--) {
            (void)_bcm_esw_port_tab_set(unit, local_port_out[port_idx],
                                        _BCM_CPU_TABS_NONE, PORT_OPERATIONf,
                                        port_op);

            (void)bcmi_gh2_vxlan_trunk_table_set(unit, local_port_out[port_idx],
                                                 tgid, vpn);
        }
    } else {
        return BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_match_trunk_add
 * Purpose:
 *    Assign SVP of VXLAN Trunk port
 * Parameters:
 *    unit - (IN) Device Number
 *    tgid - (IN) Trunk group Id
 *    vpn  - (IN) VPN instance ID
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
 bcmi_gh2_vxlan_match_trunk_add(int unit, bcm_trunk_t tgid, bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE, rv2 = BCM_E_NONE;
    int port_idx = 0;
    int local_port_out[SOC_MAX_NUM_PORTS];
    int local_member_count = 0;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_trunk_local_members_get(unit, tgid, SOC_MAX_NUM_PORTS,
                                         local_port_out, &local_member_count));
    for (port_idx = 0; port_idx < local_member_count; port_idx++) {
        rv = bcmi_gh2_vxlan_trunk_table_set(unit, local_port_out[port_idx],
                                            tgid, vpn);
        if (BCM_FAILURE(rv)) {
            rv2 = bcmi_gh2_vxlan_match_trunk_cleanup
                      (unit, tgid, vpn, local_port_out, port_idx,
                       GH2_VXLAN_PORT_OPERATION_NORMAL);
            if (BCM_FAILURE(rv2)) {
                return rv2;
            }

            return rv;
        }
        rv = _bcm_esw_port_tab_set(unit, local_port_out[port_idx],
                                   _BCM_CPU_TABS_NONE,
                                   PORT_OPERATIONf,
                                   GH2_VXLAN_PORT_OPERATION_VFI);
        if (BCM_FAILURE(rv)) {
            rv2 = bcmi_gh2_vxlan_match_trunk_cleanup
                      (unit, tgid, vpn, local_port_out, port_idx,
                       GH2_VXLAN_PORT_OPERATION_NORMAL);
            if (BCM_FAILURE(rv2)) {
                return rv2;
            }

            return rv;
        }

        rv = bcmi_gh2_vxlan_port_untagged_profile_set(
            unit, local_port_out[port_idx]);
        if (BCM_FAILURE(rv)) {
            rv2 = bcmi_gh2_vxlan_match_trunk_cleanup
                      (unit, tgid, vpn, local_port_out, port_idx,
                       GH2_VXLAN_PORT_OPERATION_NORMAL);
            if (BCM_FAILURE(rv2)) {
                return rv2;
            }

            rv2 = bcmi_gh2_vxlan_port_untagged_profile_reset
                      (unit, local_port_out[port_idx]);
            if (BCM_FAILURE(rv2)) {
                return rv2;
            }

            return rv;
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_match_trunk_delete
 * Purpose:
 *    Remove SVP of VXLAN Trunk port
 * Parameters:
 *    unit - (IN) Device Number
 *    tgid - (IN) Trunk group Id
 *    vpn  - (IN) VPN instance ID
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_match_trunk_delete(int unit, bcm_trunk_t tgid, bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE, rv2 = BCM_E_NONE;
    int port_idx = 0;
    int local_port_out[SOC_MAX_NUM_PORTS];
    int local_member_count = 0;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_trunk_local_members_get(unit, tgid, SOC_MAX_NUM_PORTS,
                                         local_port_out, &local_member_count));

    for (port_idx = 0; port_idx < local_member_count; port_idx++) {
        rv = bcmi_gh2_vxlan_trunk_table_reset(unit, local_port_out[port_idx]);
        if (BCM_FAILURE(rv)) {
            rv2 = bcmi_gh2_vxlan_match_trunk_cleanup
                      (unit, tgid, vpn, local_port_out, port_idx,
                       GH2_VXLAN_PORT_OPERATION_VFI);
            if (BCM_FAILURE(rv2)) {
                return rv2;
            }

            return rv;
        }
        rv = _bcm_esw_port_tab_set(unit, local_port_out[port_idx],
                                   _BCM_CPU_TABS_NONE,
                                   PORT_OPERATIONf,
                                   GH2_VXLAN_PORT_OPERATION_NORMAL);
        if (BCM_FAILURE(rv)) {
            rv2 = bcmi_gh2_vxlan_match_trunk_cleanup
                      (unit, tgid, vpn, local_port_out, port_idx,
                       GH2_VXLAN_PORT_OPERATION_VFI);
            if (BCM_FAILURE(rv2)) {
                return rv2;
            }

            return rv;
        }

        rv = bcmi_gh2_vxlan_port_untagged_profile_reset
                  (unit, local_port_out[port_idx]);
        if (BCM_FAILURE(rv)) {
            rv2 = bcmi_gh2_vxlan_match_trunk_cleanup
                      (unit, tgid, vpn, local_port_out, port_idx,
                       GH2_VXLAN_PORT_OPERATION_VFI);
            if (BCM_FAILURE(rv2)) {
                return rv2;
            }

            return rv;
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_match_clear
 * Purpose:
 *    Clear VXLAN Match Software State
 * Parameters:
 *    unit - (IN) Device Number
 *    vp  - (IN) Source Virtual Port
 * Returns:
 *    void
 */
STATIC void
bcmi_gh2_vxlan_match_clear(int unit, int vp)
{
    int num_vp;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info = GH2_VXLAN_INFO(unit);

    if (NULL == vxlan_info) {
        return;
    }

    num_vp = vxlan_info->num_vxlan_vp;
    if ((vp < 0) || (vp >= num_vp)) {
        return;
    }

    vxlan_info->match_key[vp].flags = 0;
    vxlan_info->match_key[vp].match_vlan = 0;
    vxlan_info->match_key[vp].match_inner_vlan = 0;
    vxlan_info->match_key[vp].trunk_id = -1;
    vxlan_info->match_key[vp].port = 0;
    vxlan_info->match_key[vp].modid = -1;
    vxlan_info->match_key[vp].match_tunnel_index = 0;

    return;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_match_add
 * Purpose:
 *    Assign match criteria of an VXLAN port
 * Parameters:
 *    unit - (IN) Device Number
 *    vxlan_port - (IN/OUT) vxlan_port information (OUT : vxlan_port_id)
 *    vp   - (IN) Source Virtual Port
 *    vpn  - (IN) VPN instance ID
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_match_add(
    int unit,
    bcm_vxlan_port_t *vxlan_port,
    int vp,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    bcm_trunk_t trunk_id = -1;
    soc_mem_t mem;
    uint32 vent[SOC_MAX_MEM_WORDS];
    int src_trk_idx = 0; /* Source Trunk table index.*/
    int gport_id = -1;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    int tunnel_idx = -1;
    int vfi_index = -1;
    gh2_vxlan_vpn_info_t *vfi_info;

    /* Input parameter check */
    if (NULL == vxlan_port) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    if (vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_NONE) {
        vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_NONE;
        return BCM_E_NONE;
    }

    if (vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_SHARE) {
        /* there is no criteria to be configured for shared vp,
           only updating software state to indicate this is a shared vp */
        /* GH2 doesn't support SHARE criteria and flags */
        return BCM_E_UNAVAIL;
    }

    sal_memset(vent, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));

    mem = VLAN_XLATEm;
    /* To create additional IVXLT for Loopback VP (GH2-B0 only) */
    if (vxlan_port->flags & BCM_VXLAN_PORT_LOOPBACK) {
        /* Loopback VP is only support when VXLAN RIOT is enabled */
        if (!BCM_VLAN_VALID(vxlan_port->match_vlan)) {
            return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, mem, vent,
                            KEY_TYPEf, GH2_VLXLT_HASH_KEY_TYPE_OVID);
        soc_mem_field32_set(unit, mem, vent,
                            XLATE__OVIDf, vxlan_port->match_vlan);
        /* incoming port not used in lookup key, set field to all 1's */
        soc_mem_field32_set(unit, mem, vent, XLATE__GLPf,
                            SOC_VLAN_XLATE_GLP_WILDCARD(unit));

        rv = bcmi_gh2_vxlan_match_vxlate_entry_set(unit, vpn,
                                                   vxlan_port, vent);
        BCM_IF_ERROR_RETURN(rv);
    }

    sal_memset(vent, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));
    if (vxlan_port->criteria != BCM_VXLAN_PORT_MATCH_VN_ID) {
        rv = _bcm_esw_gport_resolve(unit, vxlan_port->match_port, &mod_out,
                                    &port_out, &trunk_id, &gport_id);
        BCM_IF_ERROR_RETURN(rv);

        if (BCM_GPORT_IS_TRUNK(vxlan_port->match_port)) {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 0x1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf, trunk_id);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf, mod_out);
            soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf, port_out);
        }
    }

    if (vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_PORT_VLAN) {
        if (!BCM_VLAN_VALID(vxlan_port->match_vlan)) {
            return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, mem, vent,
                            KEY_TYPEf, GH2_VLXLT_HASH_KEY_TYPE_OVID);
        soc_mem_field32_set(unit, mem, vent,
                            XLATE__OVIDf, vxlan_port->match_vlan);

        rv = bcmi_gh2_vxlan_match_vxlate_entry_set(unit, vpn,
                                                   vxlan_port, vent);
        BCM_IF_ERROR_RETURN(rv);

        if (vxlan_info->match_key[vp].flags !=
            _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {
            vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_VLAN;
            vxlan_info->match_key[vp].match_vlan = vxlan_port->match_vlan;
            if (BCM_GPORT_IS_TRUNK(vxlan_port->match_port)) {
                vxlan_info->match_key[vp].trunk_id = trunk_id;
                vxlan_info->match_key[vp].modid = -1;
            } else {
                vxlan_info->match_key[vp].port = port_out;
                vxlan_info->match_key[vp].modid = mod_out;
                vxlan_info->match_key[vp].trunk_id = -1;
            }
        }

        if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
            bcmi_gh2_vxlan_port_match_count_adjust(unit, vp, 1);
        }
    } else if (vxlan_port->criteria ==
        BCM_VXLAN_PORT_MATCH_PORT_INNER_VLAN) {
        if (!BCM_VLAN_VALID(vxlan_port->match_inner_vlan)) {
            return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, mem, vent,
                            KEY_TYPEf, GH2_VLXLT_HASH_KEY_TYPE_IVID);
        soc_mem_field32_set(unit, mem, vent,
                            XLATE__IVIDf, vxlan_port->match_inner_vlan);

        rv = bcmi_gh2_vxlan_match_vxlate_entry_set(unit, vpn,
                                                   vxlan_port, vent);
        BCM_IF_ERROR_RETURN(rv);

        if (vxlan_info->match_key[vp].flags !=
            _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {
            vxlan_info->match_key[vp].flags =
                _BCM_VXLAN_PORT_MATCH_TYPE_INNER_VLAN;
            vxlan_info->match_key[vp].match_inner_vlan =
                vxlan_port->match_inner_vlan;
            if (BCM_GPORT_IS_TRUNK(vxlan_port->match_port)) {
                vxlan_info->match_key[vp].trunk_id = trunk_id;
                vxlan_info->match_key[vp].modid = -1;
            } else {
                vxlan_info->match_key[vp].port = port_out;
                vxlan_info->match_key[vp].modid = mod_out;
                vxlan_info->match_key[vp].trunk_id = -1;
            }
        }

        if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
            bcmi_gh2_vxlan_port_match_count_adjust(unit, vp, 1);
        }
    } else if (vxlan_port->criteria ==
        BCM_VXLAN_PORT_MATCH_PORT_VLAN_STACKED) {
        if (!BCM_VLAN_VALID(vxlan_port->match_vlan) ||
            !BCM_VLAN_VALID(vxlan_port->match_inner_vlan)) {
            return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, mem, vent,
                            KEY_TYPEf, GH2_VLXLT_HASH_KEY_TYPE_IVID_OVID);
        soc_mem_field32_set(unit, mem, vent,
                            XLATE__OVIDf, vxlan_port->match_vlan);
        soc_mem_field32_set(unit, mem, vent,
                            XLATE__IVIDf, vxlan_port->match_inner_vlan);

        rv = bcmi_gh2_vxlan_match_vxlate_entry_set(unit, vpn,
                                                   vxlan_port, vent);
        BCM_IF_ERROR_RETURN(rv);

        if (vxlan_info->match_key[vp].flags !=
            _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {
            vxlan_info->match_key[vp].flags =
                _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_STACKED;
            vxlan_info->match_key[vp].match_vlan =
                vxlan_port->match_vlan;
            vxlan_info->match_key[vp].match_inner_vlan =
                vxlan_port->match_inner_vlan;
            if (BCM_GPORT_IS_TRUNK(vxlan_port->match_port)) {
                vxlan_info->match_key[vp].trunk_id = trunk_id;
                vxlan_info->match_key[vp].modid = -1;
            } else {
                vxlan_info->match_key[vp].port = port_out;
                vxlan_info->match_key[vp].modid = mod_out;
                vxlan_info->match_key[vp].trunk_id = -1;
            }
        }

        if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
            bcmi_gh2_vxlan_port_match_count_adjust(unit, vp, 1);
        }
    } else if (vxlan_port->criteria ==
        BCM_VXLAN_PORT_MATCH_VLAN_PRI) {

        soc_mem_field32_set(unit, mem, vent,
                            KEY_TYPEf, GH2_VLXLT_HASH_KEY_TYPE_PRI_CFI);
        /* match_vlan: Bits 12-15 contains VLAN_PRI + CFI, vlan = BCM_E_NONE */
        soc_mem_field32_set(unit, mem, vent,
                            XLATE__OTAGf, vxlan_port->match_vlan);

        rv = bcmi_gh2_vxlan_match_vxlate_entry_set(unit, vpn,
                                                   vxlan_port, vent);
        BCM_IF_ERROR_RETURN(rv);

        if (vxlan_info->match_key[vp].flags !=
            _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {
            vxlan_info->match_key[vp].flags =
                _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_PRI;
            vxlan_info->match_key[vp].match_vlan =
                vxlan_port->match_vlan;
            if (BCM_GPORT_IS_TRUNK(vxlan_port->match_port)) {
                vxlan_info->match_key[vp].trunk_id = trunk_id;
                vxlan_info->match_key[vp].modid = -1;
            } else {
                vxlan_info->match_key[vp].port = port_out;
                vxlan_info->match_key[vp].modid = mod_out;
                vxlan_info->match_key[vp].trunk_id = -1;
            }
        }

        if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
            bcmi_gh2_vxlan_port_match_count_adjust(unit, vp, 1);
        }
    } else if (vxlan_port->criteria ==
        BCM_VXLAN_PORT_MATCH_PORT) {
        int is_local = 0, vfi = 0;

        if (vxlan_info->match_key[vp].flags ==
            _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {
            return BCM_E_PARAM;
        }

        if (BCM_GPORT_IS_TRUNK(vxlan_port->match_port)) {
            BCM_IF_ERROR_RETURN(
                bcmi_gh2_vxlan_match_trunk_add(unit, trunk_id, vpn));
            vxlan_info->match_key[vp].flags =
                _BCM_VXLAN_PORT_MATCH_TYPE_TRUNK;
            vxlan_info->match_key[vp].trunk_id = trunk_id;
        } else {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, mod_out, &is_local));

            /* Get index to source trunk map table */
            BCM_IF_ERROR_RETURN(
                _bcm_esw_src_mod_port_table_index_get(unit, mod_out,
                                                      port_out, &src_trk_idx));

            /* Set vfi to provide default VFI (for untag & ptag packets) */
            if (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm, VFIf)) {
                if (vpn != BCM_VXLAN_VPN_INVALID) {
                    _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
                    if (!bcmi_gh2_vxlan_vfi_used_get(unit, vfi)) {
                        return BCM_E_NOT_FOUND;
                    }
                } else {
                    return BCM_E_PARAM;
                }
                /* Set vfi to provide default VFI (for untag & ptag packets) */
                rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                            src_trk_idx, VFIf, vfi);
                BCM_IF_ERROR_RETURN(rv);
            }

            /* Disable VLAN check for SOURCE_TRUNK_MAP_TABLEm */
            if (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm,
                                    DISABLE_VLAN_CHECKSf)) {
                rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                            src_trk_idx, DISABLE_VLAN_CHECKSf,
                                            1);
                BCM_IF_ERROR_RETURN(rv);
            }

            if (is_local) {
                rv = _bcm_esw_port_tab_set(unit, vxlan_port->match_port,
                                           _BCM_CPU_TABS_NONE,
                                           PORT_OPERATIONf,
                                           GH2_VXLAN_PORT_OPERATION_VFI);
                BCM_IF_ERROR_RETURN(rv);

                /* Set TAG_ACTION_PROFILE_PTR */
                rv = bcmi_gh2_vxlan_port_untagged_profile_set(
                    unit, vxlan_port->match_port);
                BCM_IF_ERROR_RETURN(rv);
            }
            /*
             * The vp has been created based on trunk.
             * When a new port is added into the trunk, a match will be
             * added for the vp through bcm_port_match_add.
             */
            if ((vxlan_info->match_key[vp].flags !=
                _BCM_VXLAN_PORT_MATCH_TYPE_TRUNK) &&
                (vxlan_info->match_key[vp].flags !=
                _BCM_VXLAN_PORT_MATCH_TYPE_SHARED)) {
                vxlan_info->match_key[vp].flags =
                    _BCM_VXLAN_PORT_MATCH_TYPE_PORT;
                vxlan_info->match_key[vp].index = src_trk_idx;
                vxlan_info->match_key[vp].port = port_out;
                vxlan_info->match_key[vp].modid = mod_out;
                vxlan_info->match_key[vp].trunk_id = -1;
            }
        }
    } else if (vxlan_port->criteria ==
        BCM_VXLAN_PORT_MATCH_VN_ID) {

        if (!BCM_GPORT_IS_TUNNEL(vxlan_port->match_tunnel_id)) {
            return BCM_E_BADID;
        }

        tunnel_idx = BCM_GPORT_TUNNEL_ID_GET(vxlan_port->match_tunnel_id);
        GH2_VXLAN_TUNNEL_ID_IF_INVALID_RETURN(unit, tunnel_idx);

        /* Configure Ingress VNID (use VLAN_XLATE) */
        _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
        /* Get VFI info for VN_ID */
        vfi_info = &(vxlan_info->vxlan_vpn_info[vfi_index]);

        soc_mem_field32_set(unit, mem, vent,
                            KEY_TYPEf, GH2_VLXLT_HASH_KEY_TYPE_VNID);
        soc_mem_field32_set(unit, mem, vent,
                            XLATE__VNIDf, vfi_info->vnid);
        /* incoming port not used in lookup key, set field to all 1's */
        soc_mem_field32_set(unit, mem, vent, XLATE__GLPf,
                            SOC_VLAN_XLATE_GLP_WILDCARD(unit));

        rv = bcmi_gh2_vxlan_match_vxlate_entry_set(unit, vpn,
                                                   vxlan_port, vent);
        BCM_IF_ERROR_RETURN(rv);

        if (vxlan_info->match_key[vp].flags !=
            _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {
            vxlan_info->match_key[vp].flags = _BCM_VXLAN_PORT_MATCH_TYPE_VNID;
            vxlan_info->match_key[vp].match_tunnel_index = tunnel_idx;
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_match_delete
 * Purpose:
 *    Delete match criteria of an VXLAN port
 * Parameters:
 *    unit - (IN) Device Number
 *    vp   - (IN) Source Virtual Port
 *    vxlan_port - (IN/OUT) vxlan_port information (OUT : vxlan_port_id)
 *    vpn  - (IN) VPN instance ID
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_match_delete(
    int unit,
    int vp,
    bcm_vxlan_port_t vxlan_port,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    uint32 vent[SOC_MAX_MEM_WORDS];
    bcm_trunk_t trunk_id;
    int src_trk_idx = 0;   /* Source Trunk table index */
    int mod_id_idx = 0;   /* Module_Id */
    int port = 0;
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    int gport_id = -1;
    int vfi_index = -1;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vpn_info_t *vfi_info;

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    sal_memset(vent, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));

    mem = VLAN_XLATEm;
    /* To delete additional IVXLT for Loopback VP (GH2-B0 only) */
    if (vxlan_port.flags & BCM_VXLAN_PORT_LOOPBACK) {
        /* Loopback VP is only support when VXLAN RIOT is enabled */
        soc_mem_field32_set(unit, mem, vent,
                            KEY_TYPEf, GH2_VLXLT_HASH_KEY_TYPE_OVID);
        soc_mem_field32_set(unit, mem, vent,
                            XLATE__OVIDf,
                            vxlan_info->match_key[vp].match_vlan);
        /* incoming port not used in lookup key, set field to all 1's */
        soc_mem_field32_set(unit, mem, vent, XLATE__GLPf,
                            SOC_VLAN_XLATE_GLP_WILDCARD(unit));

        rv = bcmi_gh2_vxlan_match_vxlate_entry_reset(unit, vent);
        BCM_IF_ERROR_RETURN(rv);
    }

    sal_memset(vent, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));
    if (vxlan_info->match_key[vp].flags == _BCM_VXLAN_PORT_MATCH_TYPE_VLAN) {
        soc_mem_field32_set(unit, mem, vent,
                            KEY_TYPEf, GH2_VLXLT_HASH_KEY_TYPE_OVID);
        soc_mem_field32_set(unit, mem, vent,
                            XLATE__OVIDf,
                            vxlan_info->match_key[vp].match_vlan);
        if (vxlan_info->match_key[vp].modid != -1) {
            soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf,
                                vxlan_info->match_key[vp].modid);
            soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf,
                                vxlan_info->match_key[vp].port);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 0x1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf,
                                vxlan_info->match_key[vp].trunk_id);
        }
        rv = bcmi_gh2_vxlan_match_vxlate_entry_reset(unit, vent);
        BCM_IF_ERROR_RETURN(rv);

        bcmi_gh2_vxlan_port_match_count_adjust(unit, vp, -1);
    } else if (vxlan_info->match_key[vp].flags ==
        _BCM_VXLAN_PORT_MATCH_TYPE_INNER_VLAN) {
        soc_mem_field32_set(unit, mem, vent,
                            KEY_TYPEf, GH2_VLXLT_HASH_KEY_TYPE_IVID);
        soc_mem_field32_set(unit, mem, vent,
                            XLATE__IVIDf,
                            vxlan_info->match_key[vp].match_inner_vlan);
        if (vxlan_info->match_key[vp].modid != -1) {
            soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf,
                                vxlan_info->match_key[vp].modid);
            soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf,
                                vxlan_info->match_key[vp].port);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 0x1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf,
                                vxlan_info->match_key[vp].trunk_id);
        }
        rv = bcmi_gh2_vxlan_match_vxlate_entry_reset(unit, vent);
        BCM_IF_ERROR_RETURN(rv);

        bcmi_gh2_vxlan_port_match_count_adjust(unit, vp, -1);
    } else if (vxlan_info->match_key[vp].flags ==
        _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_STACKED) {
        soc_mem_field32_set(unit, mem, vent,
                            KEY_TYPEf, GH2_VLXLT_HASH_KEY_TYPE_IVID_OVID);
        soc_mem_field32_set(unit, mem, vent,
                            XLATE__OVIDf,
                            vxlan_info->match_key[vp].match_vlan);
        soc_mem_field32_set(unit, mem, vent,
                            XLATE__IVIDf,
                            vxlan_info->match_key[vp].match_inner_vlan);
        if (vxlan_info->match_key[vp].modid != -1) {
            soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf,
                                vxlan_info->match_key[vp].modid);
            soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf,
                                vxlan_info->match_key[vp].port);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 0x1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf,
                                vxlan_info->match_key[vp].trunk_id);
        }
        rv = bcmi_gh2_vxlan_match_vxlate_entry_reset(unit, vent);
        BCM_IF_ERROR_RETURN(rv);

        bcmi_gh2_vxlan_port_match_count_adjust(unit, vp, -1);
    } else if (vxlan_info->match_key[vp].flags ==
        _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_PRI) {
        soc_mem_field32_set(unit, mem, vent,
                            KEY_TYPEf, GH2_VLXLT_HASH_KEY_TYPE_PRI_CFI);
        soc_mem_field32_set(unit, mem, vent,
                            XLATE__OTAGf,
                            vxlan_info->match_key[vp].match_vlan);
        if (vxlan_info->match_key[vp].modid != -1) {
            soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf,
                                vxlan_info->match_key[vp].modid);
            soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf,
                                vxlan_info->match_key[vp].port);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 0x1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf,
                                vxlan_info->match_key[vp].trunk_id);
        }
        rv = bcmi_gh2_vxlan_match_vxlate_entry_reset(unit, vent);
        BCM_IF_ERROR_RETURN(rv);

        bcmi_gh2_vxlan_port_match_count_adjust(unit, vp, -1);
    } else if ((vxlan_info->match_key[vp].flags ==
        _BCM_VXLAN_PORT_MATCH_TYPE_PORT) ||
        ((vxlan_info->match_key[vp].flags ==
        _BCM_VXLAN_PORT_MATCH_TYPE_TRUNK) &&
        (!BCM_GPORT_IS_TRUNK(vxlan_port.match_port)))) {

        if (vxlan_info->match_key[vp].flags ==
            _BCM_VXLAN_PORT_MATCH_TYPE_PORT) {
            src_trk_idx = vxlan_info->match_key[vp].index;
        } else {
            /*
             * The vp has been created based on trunk.
             * When a new port is deleted from the trunk, a match
             * will be deleted for the vp through bcm_port_match_delete.
             */
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, vxlan_port.match_port, &mod_out,
                                       &port_out, &trunk_id, &gport_id));
            /* Get index to source trunk map table */
            BCM_IF_ERROR_RETURN(
                _bcm_esw_src_mod_port_table_index_get(unit, mod_out,
                                                      port_out, &src_trk_idx));
        }

        /* Disable VFI */
        if (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm, VFIf)) {
            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                        src_trk_idx, VFIf, 0x0);
            BCM_IF_ERROR_RETURN(rv);
        }

        if (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm,
                                DISABLE_VLAN_CHECKSf)) {
            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                        src_trk_idx, DISABLE_VLAN_CHECKSf, 0x0);
            BCM_IF_ERROR_RETURN(rv);
        }

        mod_id_idx = vxlan_info->match_key[vp].modid;
        port = vxlan_info->match_key[vp].port;
        rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                     mod_id_idx, port, &mod_out, &port_out);
        BCM_IF_ERROR_RETURN(rv);

        rv = _bcm_esw_port_tab_set(unit, port_out,
                                   _BCM_CPU_TABS_NONE,
                                   PORT_OPERATIONf,
                                   GH2_VXLAN_PORT_OPERATION_NORMAL);
        BCM_IF_ERROR_RETURN(rv);

        /* Reset TAG_ACTION_PROFILE_PTR */
        rv = bcmi_gh2_vxlan_port_untagged_profile_reset(unit, port_out);
        BCM_IF_ERROR_RETURN(rv);
    } else if (vxlan_info->match_key[vp].flags ==
        _BCM_VXLAN_PORT_MATCH_TYPE_TRUNK) {
        trunk_id = vxlan_info->match_key[vp].trunk_id;

        rv = bcmi_gh2_vxlan_match_trunk_delete(unit, trunk_id, vpn);
        BCM_IF_ERROR_RETURN(rv);
    } else if (vxlan_info->match_key[vp].flags ==
        _BCM_VXLAN_PORT_MATCH_TYPE_VNID) {

        _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
        /* Get VFI info for VN_ID */
        vfi_info = &(vxlan_info->vxlan_vpn_info[vfi_index]);

        soc_mem_field32_set(unit, mem, vent,
                            KEY_TYPEf, GH2_VLXLT_HASH_KEY_TYPE_VNID);
        soc_mem_field32_set(unit, mem, vent,
                            XLATE__VNIDf, vfi_info->vnid);
        /* incoming port not used in lookup key, set field to all 1's */
        soc_mem_field32_set(unit, mem, vent, XLATE__GLPf,
                            SOC_VLAN_XLATE_GLP_WILDCARD(unit));

        rv = bcmi_gh2_vxlan_match_vxlate_entry_reset(unit, vent);
        BCM_IF_ERROR_RETURN(rv);
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_match_get
 * Purpose:
 *    Obtain match information of an VXLAN port
 * Parameters:
 *    unit - (IN) Device Number
 *    vxlan_port - (IN/OUT) vxlan_port information
 *    vp   - (IN) Source Virtual Port
 *    vpn  - (IN) VPN instance ID (unused for GH2)
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_match_get(
    int unit,
    bcm_vxlan_port_t *vxlan_port,
    int vp,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    bcm_module_t mod_in = 0, mod_out = 0;
    bcm_port_t port_in = 0, port_out = 0;
    bcm_trunk_t trunk_id = 0;
    int tunnel_idx = -1;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Input parameter check */
    if (NULL == vxlan_port) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    if (vxlan_info->match_key[vp].flags & _BCM_VXLAN_PORT_MATCH_TYPE_VLAN) {
        vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_PORT_VLAN;
        vxlan_port->match_vlan = vxlan_info->match_key[vp].match_vlan;

        if (vxlan_info->match_key[vp].trunk_id != -1) {
            trunk_id = vxlan_info->match_key[vp].trunk_id;
            BCM_GPORT_TRUNK_SET(vxlan_port->match_port, trunk_id);
        } else {
            port_in = vxlan_info->match_key[vp].port;
            mod_in = vxlan_info->match_key[vp].modid;
            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                         mod_in, port_in, &mod_out, &port_out);
            vxlan_port->match_port =
                _bcm_esw_port_gport_get_from_modport(unit, mod_out, port_out);
        }
    } else if (vxlan_info->match_key[vp].flags &
        _BCM_VXLAN_PORT_MATCH_TYPE_INNER_VLAN) {
        vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_PORT_INNER_VLAN;
        vxlan_port->match_inner_vlan =
            vxlan_info->match_key[vp].match_inner_vlan;

        if (vxlan_info->match_key[vp].trunk_id != -1) {
            trunk_id = vxlan_info->match_key[vp].trunk_id;
            BCM_GPORT_TRUNK_SET(vxlan_port->match_port, trunk_id);
        } else {
            port_in = vxlan_info->match_key[vp].port;
            mod_in = vxlan_info->match_key[vp].modid;
            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                         mod_in, port_in, &mod_out, &port_out);
            vxlan_port->match_port =
                _bcm_esw_port_gport_get_from_modport(unit, mod_out, port_out);
        }
    } else if (vxlan_info->match_key[vp].flags &
        _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_STACKED) {
        vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_PORT_VLAN_STACKED;
        vxlan_port->match_vlan = vxlan_info->match_key[vp].match_vlan;
        vxlan_port->match_inner_vlan =
            vxlan_info->match_key[vp].match_inner_vlan;

        if (vxlan_info->match_key[vp].trunk_id != -1) {
            trunk_id = vxlan_info->match_key[vp].trunk_id;
            BCM_GPORT_TRUNK_SET(vxlan_port->match_port, trunk_id);
        } else {
            port_in = vxlan_info->match_key[vp].port;
            mod_in = vxlan_info->match_key[vp].modid;
            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                         mod_in, port_in, &mod_out, &port_out);
            vxlan_port->match_port =
                _bcm_esw_port_gport_get_from_modport(unit, mod_out, port_out);
        }
    } else if (vxlan_info->match_key[vp].flags &
        _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_PRI) {
        vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_VLAN_PRI;
        vxlan_port->match_vlan = vxlan_info->match_key[vp].match_vlan;

        if (vxlan_info->match_key[vp].trunk_id != -1) {
            trunk_id = vxlan_info->match_key[vp].trunk_id;
            BCM_GPORT_TRUNK_SET(vxlan_port->match_port, trunk_id);
        } else {
            port_in = vxlan_info->match_key[vp].port;
            mod_in = vxlan_info->match_key[vp].modid;
            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                         mod_in, port_in, &mod_out, &port_out);
            vxlan_port->match_port =
                _bcm_esw_port_gport_get_from_modport(unit, mod_out, port_out);
        }
    } else if (vxlan_info->match_key[vp].flags &
        _BCM_VXLAN_PORT_MATCH_TYPE_PORT) {
        port_in = vxlan_info->match_key[vp].port;
        mod_in = vxlan_info->match_key[vp].modid;

        vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_PORT;
        rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                     mod_in, port_in, &mod_out, &port_out);
        vxlan_port->match_port =
            _bcm_esw_port_gport_get_from_modport(unit, mod_out, port_out);
    } else if (vxlan_info->match_key[vp].flags &
        _BCM_VXLAN_PORT_MATCH_TYPE_TRUNK) {
        trunk_id = vxlan_info->match_key[vp].trunk_id;

        vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_PORT;
        BCM_GPORT_TRUNK_SET(vxlan_port->match_port, trunk_id);
    } else if ((vxlan_info->match_key[vp].flags &
        _BCM_VXLAN_PORT_MATCH_TYPE_VNID)) {
        vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_VN_ID;

        tunnel_idx = vxlan_info->match_key[vp].match_tunnel_index;
        BCM_GPORT_TUNNEL_ID_SET(vxlan_port->match_tunnel_id, tunnel_idx);
        vxlan_port->match_vlan = vxlan_info->match_key[vp].match_vlan;
    } else if (vxlan_info->match_key[vp].flags &
        _BCM_VXLAN_PORT_MATCH_TYPE_SHARED) {
        rv = BCM_E_INTERNAL;
    } else {
        vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_NONE;
    }

    /*
     * For all VXLAN RIOT flow support (loopback, access or network ports),
     * we must use vxlan_port->match_port for Custom Header
     * related configurations
     */
    if (GH2_VXLAN_RIOT_ENABLE(unit)) {
        if ((vxlan_info->match_key[vp].flags &
            _BCM_VXLAN_PORT_MATCH_TYPE_VNID) ||
            (vxlan_info->match_key[vp].flags &
            _BCM_VXLAN_PORT_MATCH_TYPE_NONE)) {
            if (vxlan_info->match_key[vp].trunk_id != -1) {
                trunk_id = vxlan_info->match_key[vp].trunk_id;
                BCM_GPORT_TRUNK_SET(vxlan_port->match_port, trunk_id);
            } else {
                port_in = vxlan_info->match_key[vp].port;
                mod_in = vxlan_info->match_key[vp].modid;
                rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                             mod_in, port_in,
                                             &mod_out, &port_out);
                vxlan_port->match_port =
                    _bcm_esw_port_gport_get_from_modport(unit, mod_out,
                                                         port_out);
            }
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_elan_port_add
 * Purpose:
 *    Add VXLAN ELAN port to a VPN
 * Parameters:
 *    unit - (IN) Device Number
 *    vpn  - (IN) VPN instance ID
 *    vxlan_port - (IN/OUT) vxlan_port information (OUT : vxlan_port_id)
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_elan_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_vxlan_port_t *vxlan_port)
{
    int rv = BCM_E_PARAM, rv_error = BCM_E_NONE;
    int vp, num_vp, vfi = 0;
    bcmi_vxlan_vp_info_t vxlan_vp_info;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vp_info_t *vp_info =NULL;
    int old_tpid_valid = 0;
    uint16 old_tpid = 0, new_tpid = 0;
    bcm_switch_tpid_info_t tpid_info;
    int tpid_enable = 0;
    bcm_vxlan_port_t old_vxlan_port;

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    /* Input parameter check */
    if (vpn != BCM_VXLAN_VPN_INVALID) {
        _BCM_VXLAN_VPN_GET(vfi, _BCM_VXLAN_VPN_TYPE_ELAN, vpn);
        if (!bcmi_gh2_vxlan_vfi_used_get(unit, vfi)) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        vfi = _BCM_VXLAN_VFI_INVALID;
    }

    if (NULL == vxlan_port) {
        return BCM_E_PARAM;
    }

    if (vxlan_port->flags & BCM_VXLAN_PORT_DROP) {
        return BCM_E_PARAM;
    }

    num_vp = vxlan_info->num_vxlan_vp;

    bcmi_gh2_vxlan_vp_info_init(&vxlan_vp_info);
    if (vxlan_port->flags & BCM_VXLAN_PORT_NETWORK) {
        vxlan_vp_info.flags |= GH2_VXLAN_VP_INFO_NETWORK_PORT;
    } else if (vxlan_port->flags & BCM_VXLAN_PORT_LOOPBACK) {
        /* Only support for GH2-B0 when VXLAN RIOT is enabled */
        vxlan_vp_info.flags |= GH2_VXLAN_VP_INFO_LOOPBACK_PORT;
    }
    if ((vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_SHARE) ||
        (vxlan_port->flags & BCM_VXLAN_PORT_SHARE)) {
        /* GH2 doesn't support SHARE criteria and flags */
        return BCM_E_UNAVAIL;
    }

    /* =============== Allocate/Get Virtual Port =============== */
    if (vxlan_port->flags & BCM_VXLAN_PORT_REPLACE) {
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);

        if (vp == -1) {
            return BCM_E_PARAM;
        }
        if (!bcmi_gh2_vxlan_vp_used_get(unit, vp)) {
            return BCM_E_NOT_FOUND;
        }

        /* Decrease old-port's nexthop count */
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_port_nh_delete(unit, vpn, vp));
        /* Decrease old-port's VP count */
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_port_cnt_update(unit, vxlan_port->vxlan_port_id,
                                           vp, FALSE));

        /* Get old-port's vxlan_port information when VXLAN RIOT is enabled */
        if (GH2_VXLAN_RIOT_ENABLE(unit)) {
            bcm_vxlan_port_t_init(&old_vxlan_port);
            old_vxlan_port.vxlan_port_id = vxlan_port->vxlan_port_id;

            BCM_IF_ERROR_RETURN(
                bcmi_gh2_vxlan_port_get(unit, vpn, &old_vxlan_port));
        }
    } else if (vxlan_port->flags & BCM_VXLAN_PORT_WITH_ID) {
        if (!BCM_GPORT_IS_VXLAN_PORT(vxlan_port->vxlan_port_id)) {
             return BCM_E_BADID;
        }

        vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
        if (vp == -1) {
            return BCM_E_PARAM;
        }
        /*
         * In TD2, Vp index zero is reserved in function _bcm_virtual_init
         * because of HW restriction. No such restriction on GH2.
         */
        if ((vp < 0) || (vp >= num_vp)) {
            return BCM_E_BADID;
        }
        if (bcmi_gh2_vxlan_vp_used_get(unit, vp)) {
            return BCM_E_EXISTS;
        }

        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_vp_used_set(unit, vp, vxlan_vp_info));
    } else {
        /* Allocate a new VP index */
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_vp_alloc(unit, 0, (num_vp - 1), 1,
                                    vxlan_vp_info, &vp));
        BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port->vxlan_port_id, vp);
    }

    /* =============== Configure Next-Hop Properties =============== */
    rv = bcmi_gh2_vxlan_port_nh_add(unit, vxlan_port, vp, vpn);
    if (BCM_FAILURE(rv)) {
        if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
            (void)bcmi_gh2_vxlan_vp_free(unit, 1, vp);
        }
        return rv;
    }

    /* Need to parse Service-Tag Properties */
    if (vxlan_port->flags & BCM_VXLAN_PORT_SERVICE_TAGGED) {
        sal_memset(&tpid_info, 0, sizeof(tpid_info));
        tpid_info.tpid_type = bcmTpidTypeVxlanPayloadOuter;
        tpid_info.tpid_value = vxlan_port->egress_service_tpid;

        rv = bcm_esw_switch_tpid_add(unit, 0, &tpid_info);
        if (BCM_FAILURE(rv) && rv != BCM_E_EXISTS) {
            if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
                rv_error = bcmi_gh2_vxlan_port_nh_delete(unit, vpn, vp);
                if (BCM_FAILURE(rv_error) && rv_error != BCM_E_NOT_FOUND) {
                    return rv_error;
                }
                (void)bcmi_gh2_vxlan_vp_free(unit, 1, vp);
            }
            return rv;
        }

        tpid_enable = 1;

        /* Get current VXLAN Outer TPID info with specified VP */
        vp_info = &(vxlan_info->vxlan_vp_info[vp]);
        old_tpid_valid = vp_info->tpid_valid;
        old_tpid = vp_info->tpid_value; /* old TPID value */
        new_tpid = tpid_info.tpid_value; /* new TPID value */
        if (old_tpid_valid) {
            if (new_tpid != old_tpid) {
                /* Decrease ref_count for old VXLAN Outer TPID */
                (void)bcmi_gh2_vxlan_outer_tpid_ref_dec(unit, old_tpid);

                /* Increase ref_count for new VXLAN Outer TPID */
                (void)bcmi_gh2_vxlan_outer_tpid_ref_inc(unit, new_tpid);

                /* Update latest VXLAN Outer TPID info with specified VP */
                vp_info->tpid_value = new_tpid;
            }
        } else {
            /* Increase ref_count for new VXLAN Outer TPID */
            (void)bcmi_gh2_vxlan_outer_tpid_ref_inc(unit, new_tpid);

            /* Update latest VXLAN Outer TPID info with specified VP */
            vp_info->tpid_valid = 1;
            vp_info->tpid_value = new_tpid;
        }
    }

    /* Configure VP match criteria */
    rv = bcmi_gh2_vxlan_match_add(unit, vxlan_port, vp, vpn);
    if (BCM_FAILURE(rv)) {
        if (tpid_enable) {
            if (old_tpid_valid) {
                if (new_tpid != old_tpid) {
                    /* Decrease ref_count for new VXLAN Outer TPID */
                    (void)bcmi_gh2_vxlan_outer_tpid_ref_dec(unit, new_tpid);

                    /* Restore ref_count for old VXLAN Outer TPID */
                    sal_memset(&tpid_info, 0, sizeof(tpid_info));
                    tpid_info.tpid_type = bcmTpidTypeVxlanPayloadOuter;
                    tpid_info.tpid_value = old_tpid;
                    rv_error = bcm_esw_switch_tpid_add(unit, 0, &tpid_info);
                    if (BCM_FAILURE(rv_error) && rv_error != BCM_E_EXISTS) {
                        return rv_error;
                    }
                    (void)bcmi_gh2_vxlan_outer_tpid_ref_inc(unit, old_tpid);
                    vp_info->tpid_value = old_tpid;
                }
            } else {
                /* Decrease ref_count for new VXLAN Outer TPID */
                (void)bcmi_gh2_vxlan_outer_tpid_ref_dec(unit, new_tpid);
                vp_info->tpid_valid = 0;
                vp_info->tpid_value = 0x0;
            }
        }

        if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
            rv_error = bcmi_gh2_vxlan_port_nh_delete(unit, vpn, vp);
            if (BCM_FAILURE(rv_error) && rv_error != BCM_E_NOT_FOUND) {
                return rv_error;
            }
            (void)bcmi_gh2_vxlan_vp_free(unit, 1, vp);
        }
        return rv;
    }

    /* Increment new-port's VP count */
    rv = bcmi_gh2_vxlan_port_cnt_update(unit, vxlan_port->vxlan_port_id,
                                        vp, TRUE);
    if (BCM_FAILURE(rv)) {
        if (tpid_enable) {
            if (old_tpid_valid) {
                if (new_tpid != old_tpid) {
                    /* Decrease ref_count for new VXLAN Outer TPID */
                    (void)bcmi_gh2_vxlan_outer_tpid_ref_dec(unit, new_tpid);

                    /* Restore ref_count for old VXLAN Outer TPID */
                    sal_memset(&tpid_info, 0, sizeof(tpid_info));
                    tpid_info.tpid_type = bcmTpidTypeVxlanPayloadOuter;
                    tpid_info.tpid_value = old_tpid;
                    rv_error = bcm_esw_switch_tpid_add(unit, 0, &tpid_info);
                    if (BCM_FAILURE(rv_error) && rv_error != BCM_E_EXISTS) {
                        return rv_error;
                    }
                    (void)bcmi_gh2_vxlan_outer_tpid_ref_inc(unit, old_tpid);
                    vp_info->tpid_value = old_tpid;
                }
            } else {
                /* Decrease ref_count for new VXLAN Outer TPID */
                (void)bcmi_gh2_vxlan_outer_tpid_ref_dec(unit, new_tpid);
                vp_info->tpid_valid = 0;
                vp_info->tpid_value = 0x0;
            }
        }

        if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
            rv_error = bcmi_gh2_vxlan_match_delete(unit, vp, *vxlan_port, vpn);
            if (BCM_FAILURE(rv_error) && rv_error != BCM_E_NOT_FOUND) {
                return rv_error;
            }

            rv_error = bcmi_gh2_vxlan_port_nh_delete(unit, vpn, vp);
            if (BCM_FAILURE(rv_error) && rv_error != BCM_E_NOT_FOUND) {
                return rv_error;
            }
            (void)bcmi_gh2_vxlan_vp_free(unit, 1, vp);
        }
        return rv;
    }

    /* Configure VP related Custom Header when VXLAN RIOT is enabled */
    if (GH2_VXLAN_RIOT_ENABLE(unit)) {
        rv = bcmi_gh2_vxlan_riot_custom_header_add(unit, vxlan_port,
                                                   &old_vxlan_port, vp);
        if (BCM_FAILURE(rv)) {
            if (tpid_enable) {
                if (old_tpid_valid) {
                    if (new_tpid != old_tpid) {
                        /* Decrease ref_count for new VXLAN Outer TPID */
                        (void)bcmi_gh2_vxlan_outer_tpid_ref_dec(unit,
                                                                new_tpid);

                        /* Restore ref_count for old VXLAN Outer TPID */
                        sal_memset(&tpid_info, 0, sizeof(tpid_info));
                        tpid_info.tpid_type = bcmTpidTypeVxlanPayloadOuter;
                        tpid_info.tpid_value = old_tpid;
                        rv_error =
                            bcm_esw_switch_tpid_add(unit, 0, &tpid_info);
                        if (BCM_FAILURE(rv_error) &&
                            rv_error != BCM_E_EXISTS) {
                            return rv_error;
                        }
                        (void)bcmi_gh2_vxlan_outer_tpid_ref_inc(unit,
                                                                old_tpid);
                        vp_info->tpid_value = old_tpid;
                    }
                } else {
                    /* Decrease ref_count for new VXLAN Outer TPID */
                    (void)bcmi_gh2_vxlan_outer_tpid_ref_dec(unit, new_tpid);
                    vp_info->tpid_valid = 0;
                    vp_info->tpid_value = 0x0;
                }
            }

            if (!(vxlan_port->flags & BCM_VXLAN_PORT_REPLACE)) {
                /* Decrement port's VP count */
                BCM_IF_ERROR_RETURN(
                    bcmi_gh2_vxlan_port_cnt_update(unit,
                                                   vxlan_port->vxlan_port_id,
                                                   vp, FALSE));

                rv_error =
                    bcmi_gh2_vxlan_match_delete(unit, vp, *vxlan_port, vpn);
                if (BCM_FAILURE(rv_error) && rv_error != BCM_E_NOT_FOUND) {
                    return rv_error;
                }

                rv_error = bcmi_gh2_vxlan_port_nh_delete(unit, vpn, vp);
                if (BCM_FAILURE(rv_error) && rv_error != BCM_E_NOT_FOUND) {
                    return rv_error;
                }
                (void)bcmi_gh2_vxlan_vp_free(unit, 1, vp);
            }
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_elan_port_delete
 * Purpose:
 *    Delete VXLAN ELAN port from a VPN
 * Parameters:
 *    unit - (IN) Device Number
 *    vpn  - (IN) VPN instance ID
 *    vp   - (IN) VP
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_elan_port_delete(int unit, bcm_vpn_t vpn, int vp)
{
    int rv = BCM_E_NONE;
    bcm_gport_t vxlan_port_id;
    bcm_vxlan_port_t vxlan_port;
    bcmi_vxlan_vp_info_t vxlan_vp_info;

    if (!bcmi_gh2_vxlan_vp_used_get(unit, vp)) {
        return BCM_E_NOT_FOUND;
    }

    /* Check for Network-Port */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_vp_info_get(unit, vp, &vxlan_vp_info));

    /* Set VXLAN port ID */
    BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port_id, vp);
    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.vxlan_port_id = vxlan_port_id;
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_port_get(unit, vpn, &vxlan_port));

    rv = bcmi_gh2_vxlan_match_delete(unit, vp, vxlan_port, vpn);
    if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
        return rv;
    }

    /* Delete VP related Custom Header setting when VXLAN RIOT is enabled */
    if (GH2_VXLAN_RIOT_ENABLE(unit)) {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_riot_custom_header_delete(unit, &vxlan_port, vp));
    }

    /* Decrement port's VP count */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_port_cnt_update(unit, vxlan_port_id, vp, FALSE));

    /* Delete the next-hop info */
    rv = bcmi_gh2_vxlan_port_nh_delete(unit, vpn, vp);
    if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
        return rv;
    }

    (void)bcmi_gh2_vxlan_match_clear(unit, vp);

    /* Free the VP */
    BCM_IF_ERROR_RETURN(bcmi_gh2_vxlan_vp_free(unit, 1, vp));

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_egress_get
 * Purpose:
 *    Get VXLAN Egress NextHop
 * Parameters:
 *    unit - (IN) Device Number
 *    egr  - (OUT) L3 Egress object info
 *    nh_index - (IN) nh_index
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_egress_get(int unit, bcm_l3_egress_t *egr, int nh_index)
{
    int int_l3_flag;
    int_l3_flag = BCM_XGS3_L3_ENT_FLAG(BCM_XGS3_L3_TBL_PTR(unit, next_hop),
                                       nh_index);

    /* Input parameter check */
    if (NULL == egr) {
        return BCM_E_PARAM;
    }

    if (int_l3_flag == _BCM_L3_VXLAN_ONLY) {
         egr->flags |= BCM_L3_VXLAN_ONLY;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_vlan_get
 * Purpose:
 *    Obtain vlan information
 * Parameters:
 *    unit - (IN) Device Number
 *    vp  - (IN) Virtual Port
 *    vxlan_port - (OUT) VXLAN port information
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_port_vlan_get(int unit, int vp, bcm_vxlan_port_t *vxlan_port)
{
    int rv = BCM_E_NONE, index = 0;
    soc_mem_t mem;
    uint32 vent[SOC_MAX_MEM_WORDS], ret_vent[SOC_MAX_MEM_WORDS];
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Input parameter check */
    if (NULL == vxlan_port) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    sal_memset(vent, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));
    sal_memset(ret_vent, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));

    mem = VLAN_XLATEm;
    if (vxlan_info->match_key[vp].flags == _BCM_VXLAN_PORT_MATCH_TYPE_VLAN) {
        soc_mem_field32_set(unit, mem, vent, VALIDf, 0x1);
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf,
                            GH2_VLXLT_HASH_KEY_TYPE_OVID);
        soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf,
                            vxlan_info->match_key[vp].match_vlan);
    } else if (vxlan_info->match_key[vp].flags ==
        _BCM_VXLAN_PORT_MATCH_TYPE_INNER_VLAN) {
        soc_mem_field32_set(unit, mem, vent, VALIDf, 0x1);
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf,
                            GH2_VLXLT_HASH_KEY_TYPE_IVID);
        soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf,
                            vxlan_info->match_key[vp].match_inner_vlan);
    } else if (vxlan_info->match_key[vp].flags ==
        _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_STACKED) {
        soc_mem_field32_set(unit, mem, vent, VALIDf, 0x1);
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf,
                            GH2_VLXLT_HASH_KEY_TYPE_IVID_OVID);
        soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf,
                            vxlan_info->match_key[vp].match_vlan);
        soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf,
                            vxlan_info->match_key[vp].match_inner_vlan);
    } else if (vxlan_info->match_key[vp].flags ==
        _BCM_VXLAN_PORT_MATCH_TYPE_VLAN_PRI) {
        soc_mem_field32_set(unit, mem, vent, VALIDf, 0x1);
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf,
                            GH2_VLXLT_HASH_KEY_TYPE_PRI_CFI);
        soc_mem_field32_set(unit, mem, vent, XLATE__OTAGf,
                            vxlan_info->match_key[vp].match_vlan);
    }

    if (soc_mem_field32_get(unit, mem, vent, VALIDf)) {
        if (vxlan_info->match_key[vp].modid != -1) {
            soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf,
                                vxlan_info->match_key[vp].modid);
            soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf,
                                vxlan_info->match_key[vp].port);
        } else {
            soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 0x1);
            soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf,
                                vxlan_info->match_key[vp].trunk_id);
        }

        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY,
                            &index, vent, ret_vent, 0);
        BCM_IF_ERROR_RETURN(rv);
        if (!soc_mem_field32_get(unit, mem, ret_vent,
                                 XLATE__DISABLE_VLAN_CHECKSf)) {
            vxlan_port->flags |= BCM_VXLAN_PORT_ENABLE_VLAN_CHECKS;
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_vp_get
 * Purpose:
 *    Get VXLAN port information from a VPN and VP
 * Parameters:
 *    unit - (IN) Device Number
 *    vpn  - (IN) VPN instance ID
 *    vp   - (IN) VPN instance ID
 *    vxlan_port - (IN/OUT) VXLAN port information
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_gh2_vxlan_port_vp_get(
    int unit,
    bcm_vpn_t vpn,
    int vp,
    bcm_vxlan_port_t *vxlan_port)
{
    int network_port_flag = 0, loopback_port_flag = 0;
    bcmi_vxlan_vp_info_t vxlan_vp_info;
    int tpid_enable, tpid_index;
    uint16 tpid = 0;
    uint64 reg_val64;

    /* Input parameter check */
    if (NULL == vxlan_port) {
        return BCM_E_PARAM;
    }

    /* Check for Network-Port */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_vp_info_get(unit, vp, &vxlan_vp_info));
    if (vxlan_vp_info.flags & GH2_VXLAN_VP_INFO_NETWORK_PORT) {
        network_port_flag = TRUE;
    } else if (vxlan_vp_info.flags & GH2_VXLAN_VP_INFO_LOOPBACK_PORT) {
        loopback_port_flag = TRUE;
    }

    if (network_port_flag) {
        vxlan_port->flags |= BCM_VXLAN_PORT_NETWORK;
        vxlan_port->flags |= BCM_VXLAN_PORT_EGRESS_TUNNEL;
        vxlan_port->flags |= BCM_VXLAN_PORT_VPN_BASED;
        if (vxlan_vp_info.flags & GH2_VXLAN_VP_INFO_SHARED_PORT) {
            /* Note: no such case in GH2 */
            vxlan_port->flags |= BCM_VXLAN_PORT_SHARE;
            /* Shared VXLAN port is used only for multicast now */
            vxlan_port->flags |= BCM_VXLAN_PORT_MULTICAST;
        }
    } else if (loopback_port_flag) {
        vxlan_port->flags |= BCM_VXLAN_PORT_LOOPBACK;
    }

    /* Get the match parameters */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_match_get(unit, vxlan_port, vp, vpn));

    /* Get the BCM_VXLAN_PORT_ENABLE_VLAN_CHECKS flag */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_port_vlan_get(unit, vp, vxlan_port));

    /*
     * Get the next-hop parameters,
     * - for shared VXLAN port, don't get next hop here
     */
    if (!(vxlan_port->flags & BCM_VXLAN_PORT_SHARE)) {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_port_nh_get(unit, vpn, vp, vxlan_port));
    }

    COMPILER_64_ZERO(reg_val64);

    /* VXLAN payload outer TPIDs */
    BCM_IF_ERROR_RETURN(
        READ_ING_VXLAN_CONTROLr(unit, &reg_val64));
    tpid_enable = soc_reg64_field32_get(unit, ING_VXLAN_CONTROLr,
                                        reg_val64, PAYLOAD_OUTER_TPID_ENABLEf);
    if (tpid_enable) {
        vxlan_port->flags |= BCM_VXLAN_PORT_SERVICE_TAGGED;

        tpid_index = 0;
        while (tpid_enable) {
            /* Get the first found TPID since it is per chip not per VP */
            if (tpid_enable & 0x1) {
                BCM_IF_ERROR_RETURN(
                    _bcm_fb2_outer_tpid_entry_get(unit, &tpid,
                                                  tpid_index));
                break;
            }
            tpid_enable >>= 1;
            tpid_index++;
        }

        vxlan_port->egress_service_tpid = tpid;
    }

    /* Set VXLAN port ID */
    BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port->vxlan_port_id, vp);

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_to_nh_ecmp_index
 * Purpose:
 *    Translate VXLAN port (vp) to Next Hot or ECMP index
 * Parameters:
 *    unit - (IN) Device Number
 *    vp   - (IN) Virtual port number
 *    ecmp - (OUT) ECMP type
 *    nh_ecmp_index - (OUT) Next Hot or ECMP index
 * Returns:
 *    BCM_E_XXX.
 */
int
bcmi_gh2_vxlan_port_to_nh_ecmp_index(
    int unit,
    int vp,
    int *ecmp,
    int *nh_ecmp_index)
{
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vp_info_t *vp_info;

    if ((NULL == ecmp) || (NULL == nh_ecmp_index)) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    if (!bcmi_gh2_vxlan_vp_used_get(unit, vp)) {
        return BCM_E_PARAM;
    }

    vp_info = &(vxlan_info->vxlan_vp_info[vp]);
    *ecmp = vp_info->ecmp;
    *nh_ecmp_index = vp_info->nh_ecmp_index;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_from_nh_ecmp_index
 * Purpose:
 *    Translate Next Hot or ECMP index to VXLAN port (vp)
 * Parameters:
 *    unit - (IN) Device Number
 *    ecmp - (IN) ECMP type
 *    nh_ecmp_index - (IN) Next Hot or ECMP index
 *    vp   - (OUT) Virtual port number
 * Returns:
 *    BCM_E_XXX.
 */
int
bcmi_gh2_vxlan_port_from_nh_ecmp_index(
    int unit,
    int ecmp,
    int nh_ecmp_index,
    int *vp)
{
    int num_vp, idx;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vp_info_t *vp_info;

    if (NULL == vp) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    num_vp = vxlan_info->num_vxlan_vp;
    for (idx = 0; idx < num_vp; idx++) {
        if (bcmi_gh2_vxlan_vp_used_get(unit, idx)) {
            vp_info = &(vxlan_info->vxlan_vp_info[idx]);
            if ((vp_info->ecmp == ecmp) &&
                (vp_info->nh_ecmp_index == nh_ecmp_index)) {
                break;
            }
        }
    }

    if (idx == num_vp) {
        return BCM_E_NOT_FOUND;
    }

    *vp = idx;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_add
 * Purpose:
 *    Add VXLAN port to a VPN
 * Parameters:
 *    unit - (IN) Device Number
 *    vpn  - (IN) VPN instance ID
 *    vxlan_port - (IN/OUT) vxlan_port information (OUT : vxlan_port_id)
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_vxlan_port_t *vxlan_port)
{
    int rv = BCM_E_PARAM;
    int mode = 0;
    uint8 is_eline = 0;

    /* L3 egress mode check */
    BCM_IF_ERROR_RETURN(
        bcm_xgs3_l3_egress_mode_get(unit, &mode));
    if (!mode) {
        LOG_INFO(BSL_LS_BCM_VXLAN,
                 (BSL_META_U(unit,
                             "L3 egress mode must be set first\n")));
        return BCM_E_DISABLED;
    }

    /* Input parameter check */
    if (NULL == vxlan_port) {
        return BCM_E_PARAM;
    }

    if ((vxlan_port->if_class != 0) || (vxlan_port->mtu != 0)) {
        return BCM_E_PARAM;
    }

    if (vxlan_port->criteria == BCM_VXLAN_PORT_MATCH_SHARE) {
        if (soc_feature(unit, soc_feature_vp_sharing)) {
            if (vpn != BCM_VXLAN_VPN_INVALID) {
                return BCM_E_PARAM;
            }
        } else {
            return BCM_E_UNAVAIL;
        }
    }

    /*
     * To indicate a network VP is non-sharing for VXLAN-LITE on GH2.
     * - the sharing network VP is not supported on GH2 currently
     */
    if (vxlan_port->flags & BCM_VXLAN_PORT_VPN_BASED) {
        if (!(vxlan_port->flags & BCM_VXLAN_PORT_EGRESS_TUNNEL)) {
            return BCM_E_PARAM;
        }
    }
    if (vxlan_port->flags & BCM_VXLAN_PORT_SHARE) {
        return BCM_E_UNAVAIL;
    }

    /* GH2 doesn't have default virtual port support */
    if (vxlan_port->flags & BCM_VXLAN_PORT_DEFAULT) {
        return BCM_E_UNAVAIL;
    }

    /* To indicate a Loopback VP for VXLAN-LITE RIOT support (GH2-B0 only) */
    if (vxlan_port->flags & BCM_VXLAN_PORT_LOOPBACK) {
        if (soc_feature(unit, soc_feature_vxlan_lite_riot)) {
            /* Loopback VP is only support when VXLAN RIOT is enabled */
            if (!GH2_VXLAN_RIOT_ENABLE(unit)) {
                return BCM_E_PARAM;
            }

            /* Can't set VP as Loopback and Network simultaneously */
            if (vxlan_port->flags & BCM_VXLAN_PORT_NETWORK) {
                return BCM_E_PARAM;
            }
        } else {
            return BCM_E_UNAVAIL;
        }
    }

    if (vpn != BCM_VXLAN_VPN_INVALID) {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_vpn_is_eline(unit, vpn, &is_eline));
    }

    /* Currently, VXLAN-LITE only support ELAN */
    if (is_eline == 0) {
        rv = bcmi_gh2_vxlan_elan_port_add(unit, vpn, vxlan_port);
    } else {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_delete
 * Purpose:
 *    Delete VXLAN port from a VPN
 * Parameters:
 *    unit - (IN) Device Number
 *    vpn  - (IN) VPN instance ID
 *    vxlan_port_id - (IN) vxlan port information
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_port_delete(
    int unit,
    bcm_vpn_t vpn,
    bcm_gport_t vxlan_port_id)
{
    int rv = BCM_E_UNAVAIL;
    int vp = 0;
    uint8 is_eline = 0;

    /* Check for valid VPN */
    if (vpn != BCM_VXLAN_VPN_INVALID) {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_vpn_is_valid(unit, vpn));
    }

    vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!bcmi_gh2_vxlan_vp_used_get(unit, vp)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_vp_is_eline(unit, vp, &is_eline));

    /* Currently, VXLAN-LITE only support ELAN */
    if (is_eline == 0) {
        rv = bcmi_gh2_vxlan_elan_port_delete(unit, vpn, vp);
    } else {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_delete_all
 * Purpose:
 *    Delete all VXLAN ports from a VPN
 * Parameters:
 *    unit - (IN) Device Number
 *    vpn  - (IN) VPN instance ID
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_port_delete_all(int unit, bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    int vfi_index = 0;
    uint8 is_eline = 0;
    bcm_gport_t vxlan_port_id;
    uint32 vp = 0, num_vp;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vp_info_t *vp_info;

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    if (vpn != BCM_VXLAN_VPN_INVALID) {
        _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_vpn_is_eline(unit, vpn, &is_eline));
    } else {
        vfi_index = _BCM_VXLAN_VFI_INVALID;
    }

    if (is_eline == 0x1) {
        /* VXLAN-LITE doesn't have ELINE support */
        return BCM_E_UNAVAIL;
    }

    /* Currently, VXLAN-LITE only support ELAN (is_eline = 0x0) */
    num_vp = vxlan_info->num_vxlan_vp;

    SHR_BIT_ITER(vxlan_info->vxlan_vp_bitmap, num_vp, vp) {
        vp_info = &(vxlan_info->vxlan_vp_info[vp]);
        if (vp_info->vfi == vfi_index) {
            /* Set VXLAN port ID */
            BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port_id, vp);
            rv = bcmi_gh2_vxlan_port_delete(unit, vpn, vxlan_port_id);
            if (BCM_FAILURE(rv)) {
                /*
                 * Need to take care rv == BCM_E_DISABLED case
                 * since L3 Egress Mode could be disabled after L3 initialized
                 */
                if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_DISABLED)) {
                    return rv;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_get
 * Purpose:
 *    Get an VXLAN port from a VPN
 * Parameters:
 *    unit - (IN) Device Number
 *    vpn  - (IN) VPN instance ID
 *    vxlan_port - (IN/OUT) VXLAN port information
 */
int
bcmi_gh2_vxlan_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_vxlan_port_t *vxlan_port)
{
    int vp = 0;

    /* Input parameter check */
    if (NULL == vxlan_port) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!bcmi_gh2_vxlan_vp_used_get(unit, vp)) {
        return BCM_E_NOT_FOUND;
    }

    if (vpn != BCM_VXLAN_VPN_INVALID) {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_vpn_is_valid(unit, vpn));
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_vxlan_port_vp_get(unit, vpn, vp, vxlan_port));

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_get_all
 * Purpose:
 *    Get all VXLAN ports from a VPN
 * Parameters:
 *    unit - (IN) Device Number
 *    vpn  - (IN) VPN instance ID
 *    port_max   - (IN) Maximum number of interfaces in array
 *    port_array - (OUT) Array of VXLAN ports
 *    port_count - (OUT) Number of interfaces in array
 */
int
bcmi_gh2_vxlan_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_vxlan_port_t *port_array,
    int *port_count)
{
    int vp;
    int vfi_index = -1;
    uint8 is_eline = 0;
    int num_vp;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;
    gh2_vxlan_vp_info_t *vp_info;

    /* Input parameter check */
    if ((NULL == port_array) || (NULL == port_count)) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    if (vpn != BCM_VXLAN_VPN_INVALID) {
        _BCM_VXLAN_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, vpn);
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_vxlan_vpn_is_eline(unit, vpn, &is_eline));
    } else {
        vfi_index = _BCM_VXLAN_VFI_INVALID;
    }

    if (is_eline == 0x1) {
        /* VXLAN-LITE doesn't have ELINE support */
        return BCM_E_UNAVAIL;
    } else {
        /* Currently, VXLAN-LITE only support ELAN (is_eline = 0x0) */
        *port_count = 0;
        num_vp = vxlan_info->num_vxlan_vp;

        SHR_BIT_ITER(vxlan_info->vxlan_vp_bitmap, num_vp, vp) {
            vp_info = &(vxlan_info->vxlan_vp_info[vp]);
            if ((vp_info->vp_type) && (vp_info->vfi == vfi_index)) {
                /* Check if number of ports is requested */
                if (port_max == 0) {
                    (*port_count)++;
                    continue;
                } else if (*port_count == port_max) {
                    break;
                }

               BCM_IF_ERROR_RETURN(
                   bcmi_gh2_vxlan_port_vp_get(unit, vpn,
                                              vp, &port_array[*port_count]));
               (*port_count)++;
            }
        }

        if (*port_count == 0) {
            return BCM_E_NOT_FOUND;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_port_traverse
 * Purpose:
 *    Get information about a port instance
 * Parameters:
 *    unit - (IN) Device Number
 *    cb   - (IN) User-provided callback
 *    user_data  - (IN/OUT) callback user data
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vxlan_port_traverse(
    int unit,
    bcm_vxlan_port_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_NONE;
    int idx = 0, num_vp = 0;
    bcm_vxlan_port_t info;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    if (NULL == cb) {
        return BCM_E_PARAM;
    }

    /* Get VXLAN bookkeeping info instance */
    BCM_IF_ERROR_RETURN(
        gh2_vxlan_bk_info_instance_get(unit, &vxlan_info));

    num_vp = vxlan_info->num_vxlan_vp;
    for (idx = 0; idx < num_vp; idx++) {
        if (bcmi_gh2_vxlan_vp_used_get(unit, idx)) {
            bcm_vxlan_port_t_init(&info);

            rv = bcmi_gh2_vxlan_port_vp_get(unit, BCM_VXLAN_VPN_INVALID,
                                            idx, &info);
            if (BCM_SUCCESS(rv)) {
                BCM_IF_ERROR_RETURN(
                    cb(unit, &info, user_data));
            }
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vxlan_egress_entry_mac_replace
 * Purpose:
 *    Replace VXLAN egress MAC entry
 * Parameters:
 *    unit - (IN) Device Number
 *    nh_index  - (IN) Egress L3 NH idx
 *    egr - (IN) Egress forwarding destination
 */
int
bcmi_gh2_vxlan_egress_entry_mac_replace(
    int unit,
    int nh_index,
    bcm_l3_egress_t *egr)
{
    egr_l3_next_hop_entry_t egr_nh;
    uint32 new_macda_index = -1, old_macda_index = -1;
    void *entries[1];
    int rv = BCM_E_NONE;
    int entry_type;
    egr_mac_da_profile_entry_t old_macda_entry, new_macda_entry;

    /* Parameter Check */
    if (egr == NULL) {
        return BCM_E_PARAM;
    }
    if (nh_index < soc_mem_index_min(unit, EGR_L3_NEXT_HOPm) ||
        nh_index > soc_mem_index_max(unit, EGR_L3_NEXT_HOPm)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                     nh_index, &egr_nh));
    entry_type = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                     ENTRY_TYPEf);
    /* Replace vxlan egress mac da */
    if (entry_type == 0xa) {
        /* Get old mac_da_profile info */
        old_macda_index = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                              VXLAN__MAC_DA_PROFILE_INDEXf);
        sal_memset(&old_macda_entry, 0, sizeof(old_macda_entry));
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, EGR_MAC_DA_PROFILEm, MEM_BLOCK_ANY,
                         old_macda_index, &old_macda_entry));
        /* Delete old mac_da_profile index */
        BCM_IF_ERROR_RETURN(
            _bcm_mac_da_profile_entry_delete(unit, old_macda_index));

        /* Add new mac_da_profile index */
        sal_memset(&new_macda_entry, 0, sizeof(new_macda_entry));
        soc_mem_mac_addr_set(unit, EGR_MAC_DA_PROFILEm, &new_macda_entry,
                             MAC_ADDRESSf, egr->mac_addr);
        entries[0] = &new_macda_entry;
        rv = _bcm_mac_da_profile_entry_add(unit, entries,
                                           1, &new_macda_index);
        if (BCM_FAILURE(rv)) {
            /* Restore previous old mac_da_profile info */
            entries[0] = &old_macda_entry;
            BCM_IF_ERROR_RETURN(
                _bcm_mac_da_profile_entry_add(unit, entries,
                                              1, &new_macda_index));
        }

        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            VXLAN__MAC_DA_PROFILE_INDEXf, new_macda_index);
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL,
                          nh_index, &egr_nh));
    }

    return rv;
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *    bcmi_gh2_vxlan_sw_dump
 * Purpose:
 *    Displays VXLAN information maintained by software.
 * Parameters:
 *    unit - (IN) Device Number
 * Returns:
 *    None
 */
void
bcmi_gh2_vxlan_sw_dump(int unit)
{
    int i, num_tnl, num_vp, num_vfi;
    bcm_ip6_t dip6, sip6;
    bcmi_gh2_vxlan_bookkeeping_t *vxlan_info;

    /* Get VXLAN bookkeeping info instance */
    (void)gh2_vxlan_bk_info_instance_get(unit, &vxlan_info);

    num_tnl = vxlan_info->num_vxlan_tnl;
    LOG_CLI((BSL_META_U(unit,
                        "Tunnel Initiator Endpoints:\n")));
    for (i = 0; i < num_tnl; i++) {
        if (vxlan_info->vxlan_tunnel_init[i].dip != 0 &&
            vxlan_info->vxlan_tunnel_init[i].sip != 0) {
            /* Tunnel for IPv4 */
            LOG_CLI((BSL_META_U(unit,
                                "Tunnel idx:%d, sip:%x, dip:%x, vlan:%x\n"), i,
                    vxlan_info->vxlan_tunnel_init[i].sip,
                    vxlan_info->vxlan_tunnel_init[i].dip,
                    vxlan_info->vxlan_tunnel_init[i].vlan));
        } else if (!bcmi_gh2_vxlan_tunnel_ip6_is_zero
                       (vxlan_info->vxlan_tunnel_init[i].dip6) &&
                   !bcmi_gh2_vxlan_tunnel_ip6_is_zero
                       (vxlan_info->vxlan_tunnel_init[i].sip6)) {
            /* Tunnel for IPv6 */
            sal_memcpy(sip6,
                       vxlan_info->vxlan_tunnel_init[i].sip6,
                       BCM_IP6_ADDRLEN);
            sal_memcpy(dip6,
                       vxlan_info->vxlan_tunnel_init[i].dip6,
                       BCM_IP6_ADDRLEN);
            LOG_CLI((BSL_META_U
                        (unit, "Tunnel idx:%d, "
                         "sip6:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x, "
                         "dip6:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x, "
                         "vlan:%x\n"), i,
                    sip6[0], sip6[1], sip6[2], sip6[3], sip6[4], sip6[5],
                    sip6[6], sip6[7], sip6[8], sip6[9], sip6[10], sip6[11],
                    sip6[12], sip6[13], sip6[14], sip6[15],
                    dip6[0], dip6[1], dip6[2], dip6[3], dip6[4], dip6[5],
                    dip6[6], dip6[7], dip6[8], dip6[9], dip6[10], dip6[11],
                    dip6[12], dip6[13], dip6[14], dip6[15],
                    vxlan_info->vxlan_tunnel_init[i].vlan));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nTunnel Terminator Endpoints:\n")));
    for (i = 0; i < num_tnl; i++) {
        if (vxlan_info->vxlan_tunnel_term[i].dip != 0 &&
            vxlan_info->vxlan_tunnel_term[i].sip != 0) {
            /* Tunnel for IPv4 */
            LOG_CLI((BSL_META_U(unit,
                                "Tunnel idx:%d, sip:%x, dip:%x, vlan:%x\n"), i,
                    vxlan_info->vxlan_tunnel_term[i].sip,
                    vxlan_info->vxlan_tunnel_term[i].dip,
                    vxlan_info->vxlan_tunnel_term[i].vlan));
        } else if (!bcmi_gh2_vxlan_tunnel_ip6_is_zero
                       (vxlan_info->vxlan_tunnel_term[i].dip6) &&
                   !bcmi_gh2_vxlan_tunnel_ip6_is_zero
                       (vxlan_info->vxlan_tunnel_term[i].sip6)) {
            /* Tunnel for IPv6 */
            sal_memcpy(sip6,
                       vxlan_info->vxlan_tunnel_term[i].sip6,
                       BCM_IP6_ADDRLEN);
            sal_memcpy(dip6,
                       vxlan_info->vxlan_tunnel_term[i].dip6,
                       BCM_IP6_ADDRLEN);
            LOG_CLI((BSL_META_U
                        (unit, "Tunnel idx:%d, "
                         "sip6:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x, "
                         "dip6:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x, "
                         "vlan:%x\n"), i,
                    sip6[0], sip6[1], sip6[2], sip6[3], sip6[4], sip6[5],
                    sip6[6], sip6[7], sip6[8], sip6[9], sip6[10], sip6[11],
                    sip6[12], sip6[13], sip6[14], sip6[15],
                    dip6[0], dip6[1], dip6[2], dip6[3], dip6[4], dip6[5],
                    dip6[6], dip6[7], dip6[8], dip6[9], dip6[10], dip6[11],
                    dip6[12], dip6[13], dip6[14], dip6[15],
                    vxlan_info->vxlan_tunnel_term[i].vlan));
        }
    }

    num_vp = vxlan_info->num_vxlan_vp;
    LOG_CLI((BSL_META_U(unit,
                        "\n  Match Info    : \n")));
    for (i = 0; i < num_vp; i++) {
        if ((vxlan_info->match_key[i].trunk_id == 0 ||
            vxlan_info->match_key[i].trunk_id == -1) &&
            (vxlan_info->match_key[i].modid == 0 ||
            vxlan_info->match_key[i].modid == -1) &&
            (vxlan_info->match_key[i].port == 0) &&
            (vxlan_info->match_key[i].flags == 0)) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n  VXLAN port vp = %d\n"), i));
        LOG_CLI((BSL_META_U(unit,
                            "Flags = %x\n"), vxlan_info->match_key[i].flags));
        LOG_CLI((BSL_META_U(unit,
                            "Index = %x\n"), vxlan_info->match_key[i].index));
        LOG_CLI((BSL_META_U(unit,
                            "TGID = %d\n"), vxlan_info->match_key[i].trunk_id));
        LOG_CLI((BSL_META_U(unit,
                            "Modid = %d\n"), vxlan_info->match_key[i].modid));
        LOG_CLI((BSL_META_U(unit,
                            "Port = %d\n"), vxlan_info->match_key[i].port));
        LOG_CLI((BSL_META_U(unit,
                            "Match VLAN = %d\n"),
                vxlan_info->match_key[i].match_vlan));
        LOG_CLI((BSL_META_U(unit,
                            "Match Inner VLAN = %d\n"),
                vxlan_info->match_key[i].match_inner_vlan));
        LOG_CLI((BSL_META_U(unit,
                            "Match tunnel Index = %x\n"),
                vxlan_info->match_key[i].match_tunnel_index));
        LOG_CLI((BSL_META_U(unit,
                            "Match count = %x\n"),
                vxlan_info->match_key[i].match_count));
        LOG_CLI((BSL_META_U(unit,
                            "Related VFI = %d\n"),
                vxlan_info->match_key[i].vfi));
    }

    num_vfi = vxlan_info->num_vxlan_vfi;
    LOG_CLI((BSL_META_U(unit,
                        "\nVxlan VPN INFO:\n")));
    for (i = 0; i < num_vfi; i++) {
        if (vxlan_info->vxlan_vpn_info[i].valid != 0 ||
            vxlan_info->vxlan_vpn_info[i].vnid != 0) {
            LOG_CLI((BSL_META_U(unit,
                                "\nVxlan VPN VFI %d:\n"), i));
            LOG_CLI((BSL_META_U(unit,
                                "VXLAN VPN VFI VALID = %x \n"),
                    vxlan_info->vxlan_vpn_info[i].valid));
            LOG_CLI((BSL_META_U(unit,
                                "VXLAN VPN VNID = %d \n"),
                    vxlan_info->vxlan_vpn_info[i].vnid));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nVxlan Outer TPID INFO:\n")));
    for (i = 0; i < vxlan_info->num_vxlan_outer_tpid; i++) {
        LOG_CLI((BSL_META_U(unit,
                            "Index %d: "), i));
        LOG_CLI((BSL_META_U(unit,
                            "TPID value = %x, "),
                vxlan_info->vxlan_outer_tpid[i].tpid_value));
        LOG_CLI((BSL_META_U(unit,
                            "Ref count = %d \n"),
                vxlan_info->vxlan_outer_tpid[i].ref_count));
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nVxlan VLAN translation INFO:\n")));
    for (i = 0; i < vxlan_info->num_vxlan_vfi; i++) {
        if (vxlan_info->vxlan_vlan_xlate[i].ref_count != 0) {
            LOG_CLI((BSL_META_U(unit,
                                "Index %d: "), i));
            LOG_CLI((BSL_META_U(unit,
                                "Ref count = %d \n"),
                    vxlan_info->vxlan_vlan_xlate[i].ref_count));
        }
    }

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3 */


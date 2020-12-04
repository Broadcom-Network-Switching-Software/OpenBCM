/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    wlan.c
 * Purpose: Manages WLAN functions
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/hash.h>
#include <soc/mem.h>
#include <soc/scache.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <soc/triumph3.h>

#include <bcm/error.h>
#include <bcm/wlan.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/stack.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>

/*
 * WLAN port state - need to remember where to find the port.
 * Flags indicate the type of the port and hence the tables where the data is.
 * The match attributes are the keys for the AXP_WRX_SVP table.
 */
#define _BCM_WLAN_PORT_MATCH_BSSID                    (1 << 0)
#define _BCM_WLAN_PORT_MATCH_BSSID_RADIO              (1 << 1)
#define _BCM_WLAN_PORT_MATCH_TUNNEL                   (1 << 2)
typedef struct _bcm_tr3_wlan_port_info_s {
    uint32 flags;
    bcm_trunk_t tgid; /* Trunk group ID */
    bcm_module_t modid; /* Module id */
    bcm_port_t port; /* Port */
    bcm_mac_t match_bssid;         /* Match BSSID */
    int match_radio;               /* Match radio */
    bcm_gport_t match_tunnel;      /* Match tunnel */
    bcm_gport_t egress_tunnel;     /* Egress tunnel */
} _bcm_tr3_wlan_port_info_t;

/*
 * Software book keeping for WLAN related information
 */
typedef struct _bcm_tr3_wlan_bookkeeping_s {
    _bcm_tr3_wlan_port_info_t *port_info;   /* VP state                       */
    soc_profile_reg_t *wlan_tpid_profile;   /* WLAN TUNNEL TPID Profile       */
    soc_profile_mem_t *wlan_dvp_profile;    /* WLAN DVP Profile               */
    soc_profile_mem_t *wlan_frag_profile;   /* WLAN Fragment Id profile       */
    bcm_vlan_t  *tunnel_vlan;               /* Tunnel VLAN cache              */
    uint8       *tunnel_pri;                /* Tunnel VLAN priority           */
    uint8       *tunnel_cfi;                /* Tunnel VLAN CFI                */
    SHR_BITDCL  *intf_bitmap;               /* L3 interfaces used             */
    SHR_BITDCL  *wlan_tnl_bitmap;           /* WTX_TUNNEL index bitmap        */
    uint8       **vlan_grp_bmp_use_cnt;     /* Vlan group bitmap use count per VLAN */
} _bcm_tr3_wlan_bookkeeping_t;

static _bcm_tr3_wlan_bookkeeping_t  _bcm_tr3_wlan_bk_info[BCM_MAX_NUM_UNITS];

/* initialized state machine */
static int _tr3_wlan_initialized[BCM_MAX_NUM_UNITS];

/* wlan module lock */
static sal_mutex_t _tr3_wlan_mutex[BCM_MAX_NUM_UNITS] = {NULL};

#define L3_INFO(unit)    (&_bcm_l3_bk_info[unit])
#define WLAN_INFO(_unit_) (&_bcm_tr3_wlan_bk_info[_unit_])
#define WLAN_TPID_PROFILE(_unit_) \
    (_bcm_tr3_wlan_bk_info[_unit_].wlan_tpid_profile)
#define TUNNEL_VLAN(_unit_, _tnl_idx_) \
        (_bcm_tr3_wlan_bk_info[_unit_].tunnel_vlan[_tnl_idx_])
#define TUNNEL_PRI(_unit_, _tnl_idx_) \
        (_bcm_tr3_wlan_bk_info[_unit_].tunnel_pri[_tnl_idx_])
#define TUNNEL_CFI(_unit_, _tnl_idx_) \
        (_bcm_tr3_wlan_bk_info[_unit_].tunnel_cfi[_tnl_idx_])
#define WLAN_DVP_PROFILE(_unit_) \
    (_bcm_tr3_wlan_bk_info[_unit_].wlan_dvp_profile)
#define WLAN_FRAG_PROFILE(_unit_) \
    (_bcm_tr3_wlan_bk_info[_unit_].wlan_frag_profile)


#define WLAN_INIT(unit)                                   \
    do {                                                  \
        if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {  \
            return BCM_E_UNIT;                            \
        }                                                 \
        if (!_tr3_wlan_initialized[unit]) {               \
            return BCM_E_INIT;                            \
        }                                                 \
    } while (0)

/*
 * WLAN module lock
 */
#define WLAN_LOCK(unit) \
        sal_mutex_take(_tr3_wlan_mutex[unit], sal_mutex_FOREVER);

#define WLAN_UNLOCK(unit) \
        sal_mutex_give(_tr3_wlan_mutex[unit]);

/*
 * WTX_TUNNEL table usage bitmap operations
 */
#define _BCM_WLAN_TNL_USED_GET(_u_, _tnl_) \
        SHR_BITGET(WLAN_INFO(_u_)->wlan_tnl_bitmap, (_tnl_))
#define _BCM_WLAN_TNL_USED_SET(_u_, _tnl_) \
        SHR_BITSET(WLAN_INFO((_u_))->wlan_tnl_bitmap, (_tnl_))
#define _BCM_WLAN_TNL_USED_CLR(_u_, _tnl_) \
        SHR_BITCLR(WLAN_INFO((_u_))->wlan_tnl_bitmap, (_tnl_))

/*
 * L3 interface usage bitmap operations
 */
#define _BCM_WLAN_INTF_USED_GET(_u_, _intf_) \
        SHR_BITGET(WLAN_INFO(_u_)->intf_bitmap, (_intf_))
#define _BCM_WLAN_INTF_USED_SET(_u_, _intf_) \
        SHR_BITSET(WLAN_INFO((_u_))->intf_bitmap, (_intf_))
#define _BCM_WLAN_INTF_USED_CLR(_u_, _intf_) \
        SHR_BITCLR(WLAN_INFO((_u_))->intf_bitmap, (_intf_))

/* WLAN TUNNEL IP TYPES */
#define WLAN_TUNNEL_TYPE_V4       (0x0)
#define WLAN_TUNNEL_TYPE_V6       (0x1)
#define WLAN_TUNNEL_TYPE_AC2AC    (0x0)
#define WLAN_TUNNEL_TYPE_WTP2AC   (0x1)

/* Wlan tunnel entry width */
#define _BCM_WLAN_TNL_ENTRY_WIDTH 4
#define TNL_ENTRY_0 0
#define TNL_ENTRY_1 1
#define TNL_ENTRY_2 2
#define TNL_ENTRY_3 3

/* WLAN get tunnel index to/from tunnel id */
#define WLAN_TNL_INDEX_TO_ID(idx) (idx / _BCM_WLAN_TNL_ENTRY_WIDTH)
#define WLAN_TNL_ID_TO_INDEX(id)  (id * _BCM_WLAN_TNL_ENTRY_WIDTH)

/* capwap encap sizes */
#define IP_HDR_FIXED_LEN           20
#define IP6_HDR_FIXED_LEN          40
/*dmac(6)+smac(6)+8100(2)+vlan/Qos(2)+etype(2)+crc(4) */
#define ETHER_SGL_TAG_FIXED_LEN    22
/*Source port(2) + Destination port(2) + len(2) + cksum(2) */
#define UDP_HDR_LEN                8
/*
        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |CAPWAP Preamble|  HLEN   |   RID   | WBID    |T|F|L|W|M|K|Flags|
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |          Fragment ID          |     Frag Offset         |Rsvd |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                 (optional) Radio MAC Address (8 bytes)        |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |            (optional) Wireless Specific Information (8 bytes) |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
#define CAPWAP_HDR_MAX_LEN         24
#define CAPWAP_IPV4_ENCAP_LEN      (IP_HDR_FIXED_LEN + ETHER_SGL_TAG_FIXED_LEN + \
                                   UDP_HDR_LEN + CAPWAP_HDR_MAX_LEN)
#define CAPWAP_IPV6_ENCAP_LEN      (IP6_HDR_FIXED_LEN + ETHER_SGL_TAG_FIXED_LEN + \
                                   UDP_HDR_LEN + CAPWAP_HDR_MAX_LEN)

#define GET_CAPWAP_ENCAP_LEN(t)    (_BCM_TUNNEL_OUTER_HEADER_IPV6(t) ?  \
                                   CAPWAP_IPV6_ENCAP_LEN : CAPWAP_IPV4_ENCAP_LEN)

/* AXP WTX priority map chunk size */
#define _BCM_AXP_PRI_MAP_CHUNK 64

/*
 * Function:
 *      _bcm_tr3_wlan_tnl_create
 * Purpose: 
 *      Function creates tunnel initiator unused index.
 *
 * Parameters: 
 *      unit        - (IN) bcm device
 *      tnl_id_p    - (OUT)tunnel id
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */

STATIC int
_bcm_tr3_wlan_tnl_create(int unit, int *tnl_id_p)
{
    int idx_cnt, max_idx;
    int         rv  = BCM_E_NONE;
    soc_mem_t   mem = AXP_WTX_TUNNELm;
   
    /* Validate Args */
    if ((NULL == tnl_id_p)) {
        return BCM_E_PARAM;
    }
         
    max_idx = (soc_mem_index_count(unit, mem) / _BCM_WLAN_TNL_ENTRY_WIDTH);
    
    /* Get First free index */
    for (idx_cnt = 0; idx_cnt < max_idx; idx_cnt++) {
        if (!_BCM_WLAN_TNL_USED_GET(unit, idx_cnt)) {
            *tnl_id_p = idx_cnt; /* free index */
            break;
       }
    }
   
    /* Check if all tunnel index is used */
    if (idx_cnt == max_idx) {
        *tnl_id_p = -1;
        return BCM_E_FULL; 
    }

    /* Set Tunnel index usage bitmap */
    _BCM_WLAN_TNL_USED_SET(unit, *tnl_id_p);

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_wlan_tnl_delete
 * Purpose: 
 *      Function deletes tunnel entry for tunnel id
 *
 * Parameters: 
 *      unit       - (IN) bcm device
 *      tnl    - (IN) Tunnel index
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_tr3_wlan_tnl_delete(int unit, int tnl_id) 
{
    int idx_cnt, tnl_idx;
    axp_wtx_tunnel_entry_t axp_wtx_tunnel[4]; 
    int rv = BCM_E_NONE;

    tnl_idx =  WLAN_TNL_ID_TO_INDEX(tnl_id);
  
    /* Validate Args */
    if (!_BCM_WLAN_TNL_USED_GET(unit, tnl_id)) {
        return BCM_E_PARAM;
    }
  

    sal_memset((void*)&axp_wtx_tunnel, 0, sizeof(axp_wtx_tunnel));
    /* clear tunnel entry for tunnel index */
    for (idx_cnt =0; idx_cnt < _BCM_WLAN_TNL_ENTRY_WIDTH; idx_cnt++) {
        rv = soc_mem_write(unit, AXP_WTX_TUNNELm, MEM_BLOCK_ALL, (tnl_idx + idx_cnt),
                           (void *)&axp_wtx_tunnel[idx_cnt]);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* Clear tunnel bitmap usage */
    _BCM_WLAN_TNL_USED_CLR(unit, tnl_id);
    
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_wlan_tnl_write
 * Purpose: 
 *      Function to write tunnel entries for tnl_id
 *
 * Parameters: 
 *      unit       - (IN) bcm device
 *      tnl_id     - (IN) Tunnel index
 *      tnl_entry_p- (OUT) tunnel entry
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_tr3_wlan_tnl_write(int unit, int tnl_id, 
                                            axp_wtx_tunnel_entry_t *tnl_entries_p) 
{
    int idx_cnt, tnl_idx;
    int rv = BCM_E_NONE;
    

    /* Validate Args */
    if (NULL == tnl_entries_p) {
        return BCM_E_PARAM;
    }

    tnl_idx = WLAN_TNL_ID_TO_INDEX(tnl_id);
    for (idx_cnt =0; idx_cnt < _BCM_WLAN_TNL_ENTRY_WIDTH; idx_cnt++) {
        /* Get Tunnel entries from wlan tunnel table */
        rv = soc_mem_write(unit, AXP_WTX_TUNNELm, MEM_BLOCK_ALL, (tnl_idx + idx_cnt),
                           (void *)&tnl_entries_p[idx_cnt]);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_wlan_tnl_read
 * Purpose: 
 *      Function to read tunnel entries for tunnel id
 *
 * Parameters: 
 *      unit       - (IN) bcm device
 *      tnl_id     - (IN) Tunnel index
 *      tnl_entry_p- (OUT) tunnel entry
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_tr3_wlan_tnl_read(int unit, int tnl_id, axp_wtx_tunnel_entry_t *tnl_entries_p) 
{
    int idx_cnt, tnl_idx;
    int rv = BCM_E_NONE;
    

    /* Validate Args */
    if (NULL == tnl_entries_p) {
        return BCM_E_PARAM;
    }

    tnl_idx = WLAN_TNL_ID_TO_INDEX(tnl_id);
    for (idx_cnt =0; idx_cnt < _BCM_WLAN_TNL_ENTRY_WIDTH; idx_cnt++) {
        /* Get Tunnel entries from wlan tunnel table */
        rv = soc_mem_read(unit, AXP_WTX_TUNNELm, MEM_BLOCK_ALL, (tnl_idx + idx_cnt),
                           (void *)&tnl_entries_p[idx_cnt]);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_wlan_profile_init
 * Purpose:
 *      Internal function to create profile for WLAN profile tables
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_wlan_profile_init (int unit)
{
    soc_mem_t   mem;
    soc_reg_t   reg;
    int         entry_words, alloc_size;
    void *entry;
    int rv = BCM_E_NONE;
    uint32 base_index, tpid_index;
    uint64 rval64, *rval64s[1];

    /*
     * Initialize WLAN dvp (AXP_WTX_DVP_PROFILE) profile
     * 2k entries with 1 entry per set
     */
    if (WLAN_DVP_PROFILE(unit) == NULL) {
        WLAN_DVP_PROFILE(unit) = sal_alloc (sizeof(soc_profile_mem_t),
                                                     "Axp wtx dvp profile mem");
        if (WLAN_DVP_PROFILE(unit) == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(WLAN_DVP_PROFILE(unit));
    } else {
        soc_profile_mem_destroy(unit, WLAN_DVP_PROFILE(unit));
    }

    mem = AXP_WTX_DVP_PROFILEm;
    entry_words = sizeof(axp_wtx_dvp_profile_entry_t) / sizeof(uint32);
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                WLAN_DVP_PROFILE(unit)));

    /* Add wlan dvp default entry */
    alloc_size = sizeof(axp_wtx_dvp_profile_entry_t);
    entry = sal_alloc(alloc_size, "axp wtx dvp profile");
    if (entry == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(entry, 0, alloc_size);
    rv = soc_profile_mem_add(unit, WLAN_DVP_PROFILE(unit), &entry, 1,
                             &base_index);
    sal_free(entry);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /*
     * Initialize WLAN TPID (AXP_WTX_TUNNEL_TPID) profile
     */
    if (WLAN_TPID_PROFILE(unit) == NULL) {
        WLAN_TPID_PROFILE(unit) = sal_alloc(sizeof(soc_profile_reg_t),
                                         "Axp wtx tunnel tpid");
        if (WLAN_TPID_PROFILE(unit) == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_reg_t_init(WLAN_TPID_PROFILE(unit));
    } else {
        soc_profile_reg_destroy(unit, WLAN_TPID_PROFILE(unit));
    }

    reg = AXP_WTX_TUNNEL_TPIDr;
    SOC_IF_ERROR_RETURN
        (soc_profile_reg_create(unit, &reg, 1, WLAN_TPID_PROFILE(unit)));

    /* Add default tpid entry */
    COMPILER_64_ZERO(rval64);
    rval64s[0] = &rval64;
    rv = soc_profile_reg_add(unit, WLAN_TPID_PROFILE(unit), rval64s, 1,
                            &tpid_index);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /*
     * Initialize WLAN frag id table (AXP_WTX_FRAG_ID_PROFILE) profile
     * 1K entries of fragment id entries with 1 entry per set
     */
    if (WLAN_FRAG_PROFILE(unit) == NULL) {
        WLAN_FRAG_PROFILE(unit) = sal_alloc (sizeof(soc_profile_mem_t),
                                                     "Axp Wtx Frag Id mem");
        if (WLAN_FRAG_PROFILE(unit) == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(WLAN_FRAG_PROFILE(unit));
    } else {
        soc_profile_mem_destroy(unit, WLAN_FRAG_PROFILE(unit));
    }

    mem = AXP_WTX_FRAG_IDm;
    entry_words = sizeof(axp_wtx_frag_id_entry_t) / sizeof(uint32);
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                WLAN_FRAG_PROFILE(unit)));
    /* Add wlan frag id default entry */
    alloc_size = sizeof(axp_wtx_frag_id_entry_t);
    entry = sal_alloc(alloc_size, "axp wtx frag id profile");
    if (entry == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(entry, 0, alloc_size);
    rv = soc_profile_mem_add(unit, WLAN_FRAG_PROFILE(unit), &entry, 1,
                             &base_index);
    sal_free(entry);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_wlan_profile_delete
 * Purpose:
 *      Internal function to delete all wlan profile tables
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_wlan_profile_delete (int unit)
{
    /* Destroy wlan frag profile */
    if (WLAN_FRAG_PROFILE(unit)) {
        BCM_IF_ERROR_RETURN(
            soc_profile_mem_destroy(unit, WLAN_FRAG_PROFILE(unit)));
        sal_free(WLAN_FRAG_PROFILE(unit));
        WLAN_FRAG_PROFILE(unit) = NULL;
    }

    /* Destroy wlan tpid profile */
    if (WLAN_TPID_PROFILE(unit)) {
        BCM_IF_ERROR_RETURN(
            soc_profile_reg_destroy(unit, WLAN_TPID_PROFILE(unit)));
        sal_free(WLAN_TPID_PROFILE(unit));
        WLAN_TPID_PROFILE(unit) = NULL;
    }

    /* Destroy wlan dvp profile */
    if (WLAN_DVP_PROFILE(unit)) {
        BCM_IF_ERROR_RETURN(
            soc_profile_mem_destroy(unit, WLAN_DVP_PROFILE(unit)));
        sal_free(WLAN_DVP_PROFILE(unit));
        WLAN_DVP_PROFILE(unit) = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_wlan_free_resources
 * Purpose:
 *      Free all allocated tables and memory
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_tr3_wlan_free_resources(int unit)
{
    _bcm_tr3_wlan_bookkeeping_t *wlan_info = WLAN_INFO(unit);
    int i;

    /* Delete wlan table profiles */
    _bcm_tr3_wlan_profile_delete(unit);

    /* Destroy WLAN mutex */
    if (_tr3_wlan_mutex[unit]) {
        sal_mutex_destroy(_tr3_wlan_mutex[unit]);
        _tr3_wlan_mutex[unit] = NULL;
    }

    /* Destroy the VLAN group bitmap use count */
    if (wlan_info->vlan_grp_bmp_use_cnt) {
        for (i = 0; i < BCM_VLAN_COUNT; i++) {
            if (wlan_info->vlan_grp_bmp_use_cnt[i]) {
                sal_free((uint8 *)(wlan_info->vlan_grp_bmp_use_cnt[i]));
                wlan_info->vlan_grp_bmp_use_cnt[i] = NULL;
            }
        }
        sal_free(wlan_info->vlan_grp_bmp_use_cnt);
        wlan_info->vlan_grp_bmp_use_cnt = NULL;
    }


    /* Destroy EGR_L3_INTF usage bitmap */
    if (wlan_info->intf_bitmap) {
        sal_free(wlan_info->intf_bitmap);
        wlan_info->intf_bitmap = NULL;
    }

    /* Destroy tunnel VLAN cache */
    if (wlan_info->tunnel_vlan) {
        sal_free(wlan_info->tunnel_vlan);
        wlan_info->tunnel_vlan = NULL;
    }

    /* Destroy tunnel VLAN pri cache */
    if (wlan_info->tunnel_pri) {
        sal_free(wlan_info->tunnel_pri);
        wlan_info->tunnel_pri = NULL;
    }

    /* Destroy tunnel VLAN cfi cache */
    if (wlan_info->tunnel_cfi) {
        sal_free(wlan_info->tunnel_cfi);
        wlan_info->tunnel_cfi = NULL;
    }



    /* Destroy WTX_TUNNEL usage bitmap */
    if (wlan_info->wlan_tnl_bitmap) {
        sal_free(wlan_info->wlan_tnl_bitmap);
        wlan_info->wlan_tnl_bitmap = NULL;
    }

    /* Destroy WLAN port usage bitmap */
    if (wlan_info->port_info) {
        sal_free(wlan_info->port_info);
        wlan_info->port_info = NULL;
    }
}

#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *      _bcm_tr3_wlan_port_flex_stat_recover
 * Purpose: 
 *      This function retrieves flex stats from source vp table
 * Parameters: 
 *      unit       - (IN) bcm device
 *      source_vp  - (IN/OUT) source vp table
 *      index      - (IN) Source vp
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC void
_bcm_tr3_wlan_port_flex_stat_recover(int unit, source_vp_entry_t *source_vp,
                                     int index)
{
    int fs_idx;
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        /* vp index into ING_VINTF_COUNTER_TABLE */
        if (soc_mem_field_valid(unit, SOURCE_VPm, VINTF_CTR_IDXf)) {
            fs_idx = soc_mem_field32_get(unit, SOURCE_VPm, source_vp,
                                         VINTF_CTR_IDXf);
            if (fs_idx) {
                BCM_GPORT_WLAN_PORT_ID_SET(index, index);
                _bcm_esw_flex_stat_reinit_add(unit, _bcmFlexStatTypeGport,
                                              fs_idx, index);
            }
        }
    }
    return;
}
/*
 * Function:
 *      _bcm_tr3_wlan_lport_tpid_recover
 * Purpose: 
 *      This function gets tpid reference from lport table
 * Parameters: 
 *      unit       - (IN) bcm device
 *      lport_tab  - (IN) LPORT Table
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_tr3_wlan_lport_tpid_recover(int unit, lport_tab_entry_t *lport_tab)
{
    int tpid_enable, index, rv = BCM_E_NONE;
    tpid_enable = soc_LPORT_TABm_field32_get(unit, lport_tab,
                                             OUTER_TPID_ENABLEf);
    for (index = 0; index < 4; index++) {
        if (tpid_enable & (1 << index)) {
            rv = _bcm_fb2_outer_tpid_tab_ref_count_add(unit, index, 1);
            break;
        }
    }
    return rv;
}
/*
 * Function:
 *      _bcm_tr3_wlan_port_associated_data_recover
 * Purpose: 
 *      This function recovers wlan port data
 * Parameters: 
 *      unit       - (IN) bcm device
 *      vp         - (IN) wlan virtual port
 *      stable_size- (IN) shared table size
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_tr3_wlan_port_associated_data_recover(int unit, int vp, int stable_size)
{
    int idx, nh_index, intf_num, rv = BCM_E_NONE;
    uint32                      nh_flags;
    ing_l3_next_hop_entry_t     ing_nh_entry;
    egr_l3_next_hop_entry_t     egr_nh_entry;
    ing_dvp_table_entry_t       ing_dvp_entry;
    bcm_module_t                mod_in, mod_out;
    bcm_port_t                  port_in, port_out, phys_port;
    bcm_trunk_t                 trunk_id;
    bcm_l3_egress_t             nh_info;
    _bcm_port_info_t            *info;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count;
    uint32 port_flags;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_DVP_TABLEm, MEM_BLOCK_ANY,
                                     vp, &ing_dvp_entry));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &ing_dvp_entry,
                                              NEXT_HOP_INDEXf);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                     nh_index, &ing_nh_entry));
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                     nh_index, &egr_nh_entry));

    bcm_l3_egress_t_init(&nh_info);

    nh_flags = _BCM_L3_SHR_UPDATE | _BCM_L3_SHR_WRITE_DISABLE |
               _BCM_L3_SHR_WITH_ID;
    rv = bcm_xgs3_nh_add(unit, nh_flags, &nh_info, &nh_index);
    BCM_IF_ERROR_RETURN(rv);

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry,
                                         ENTRY_TYPEf) == 0x2) {
        /* Type is SVP */
        if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, Tf)) {
            trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit,
                                                        &ing_nh_entry, TGIDf);
            WLAN_INFO(unit)->port_info[vp].modid = -1;
            WLAN_INFO(unit)->port_info[vp].port = -1;
            WLAN_INFO(unit)->port_info[vp].tgid = trunk_id;
            /* Update each local physical port's SW state */
            if (stable_size == 0) {
                /* When persistent storage is available, this state is recovered
                   in the port module */
                rv = _bcm_esw_trunk_local_members_get(unit, trunk_id,
                        SOC_MAX_NUM_PORTS, local_member_array, &local_member_count);
                BCM_IF_ERROR_RETURN(rv);
                for (idx = 0; idx < local_member_count; idx++) {
                    _bcm_port_info_access(unit, local_member_array[idx], &info);
                    info->vp_count++;
                    BCM_IF_ERROR_RETURN(
                        bcm_esw_port_vlan_member_get(
                            unit, local_member_array[idx], &port_flags));
                    BCM_IF_ERROR_RETURN(
                        bcm_esw_port_vlan_member_set(
                            unit, local_member_array[idx], port_flags));
                }
            }
        } else {
            mod_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry,
                                                      MODULE_IDf);
            port_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry,
                                                       PORT_NUMf);
            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                        mod_in, port_in, &mod_out, &port_out);
            WLAN_INFO(unit)->port_info[vp].modid = mod_out;
            WLAN_INFO(unit)->port_info[vp].port = port_out;
            WLAN_INFO(unit)->port_info[vp].tgid = -1;
            /* Update the physical port's SW state */
            if (stable_size == 0) {
                /* When persistent storage is available, this state is recovered
                   in the port module */
                phys_port = WLAN_INFO(unit)->port_info[vp].port;
                if (soc_feature(unit, soc_feature_sysport_remap)) {
                    BCM_XLATE_SYSPORT_S2P(unit, &phys_port);
                }
                _bcm_port_info_access(unit, phys_port, &info);
                info->vp_count++;
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_vlan_member_get(
                        unit, phys_port, &port_flags));
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_vlan_member_set(
                        unit, phys_port, port_flags));
            }
        }
    } else {
        return BCM_E_INTERNAL; /* Should never happen */
    }
    intf_num = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh_entry,
                                                WLAN__INTF_NUMf);
    _BCM_WLAN_INTF_USED_SET(unit, intf_num);
    BCM_L3_INTF_USED_SET(unit, intf_num);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_wlan_reinit
 * Purpose:
 *      Warm boot recovery for the WLAN software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_wlan_reinit(int unit)
{
    int         rv, i, j, index_min, index_max, lport_profile_ptr;
    int         stable_size, vvp, vp = 0;
    int         tnl_idx, wlan_dvp_index;
    uint64      grp_bmp;
    uint32      grp_vec[2];
    uint32      entry_type;
    vlan_tab_entry_t                *vtab;
    source_vp_entry_t               source_vp;
    lport_tab_entry_t               lport_profile;
    axp_wrx_svp_assignment_entry_t  *axp_wrx_svp;
    source_vp_attributes_2_entry_t  *source_vp_attributes_2;
    egr_dvp_attribute_entry_t       egr_dvp_attribute;
    axp_wtx_dvp_profile_entry_t     axp_wtx_dvp;
    uint8  *source_vp_attributes_2_buf = NULL; /* source vp DMA buffer      */
    uint8  *axp_wtx_svp_buf            = NULL; /* axp_wrx_svp DMA buffer    */
    uint8  *vbuf                       = NULL; /* VLAN_TABLE DMA buffer     */
    void        *entries[1];                   /* for profile memories      */ 
    _bcm_vp_info_t vp_info;

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeWlan;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* DMA various tables */
    source_vp_attributes_2_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES
                                 (unit, SOURCE_VP_ATTRIBUTES_2m),
                                 "source_vp_attributes_2 buffer");
    if (NULL == source_vp_attributes_2_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, SOURCE_VP_ATTRIBUTES_2m);
    index_max = soc_mem_index_max(unit, SOURCE_VP_ATTRIBUTES_2m);
    if ((rv = soc_mem_read_range(unit, SOURCE_VP_ATTRIBUTES_2m, MEM_BLOCK_ANY,
                                 index_min, index_max,
                                 source_vp_attributes_2_buf)) < 0 ) {
        goto cleanup;
    }
    axp_wtx_svp_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES
                                       (unit, AXP_WRX_SVP_ASSIGNMENTm),
                                       "axp_wrx_svp buffer");
    if (NULL == axp_wtx_svp_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, AXP_WRX_SVP_ASSIGNMENTm);
    index_max = soc_mem_index_max(unit, AXP_WRX_SVP_ASSIGNMENTm);
    if ((rv = soc_mem_read_range(unit, AXP_WRX_SVP_ASSIGNMENTm, MEM_BLOCK_ANY,
                                 index_min, index_max,
                                 axp_wtx_svp_buf)) < 0 ) {
        goto cleanup;
    }

    vbuf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, VLAN_TABLE(unit)),
                         "VLAN_TAB buffer");
    if (NULL == vbuf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, VLAN_TABLE(unit));
    index_max = soc_mem_index_max(unit, VLAN_TABLE(unit));
    if ((rv = soc_mem_read_range(unit, VLAN_TABLE(unit), MEM_BLOCK_ANY,
                                 index_min, index_max, vbuf)) < 0 ) {
        goto cleanup;
    }

    /* Get the VPs */
    index_min = soc_mem_index_min(unit, AXP_WRX_SVP_ASSIGNMENTm);
    index_max = soc_mem_index_max(unit, AXP_WRX_SVP_ASSIGNMENTm);
    for (i = index_min; i <= index_max; i++) {
        axp_wrx_svp = soc_mem_table_idx_to_pointer
                        (unit, AXP_WRX_SVP_ASSIGNMENTm,
                         axp_wrx_svp_assignment_entry_t *, axp_wtx_svp_buf, i);
        if (soc_AXP_WRX_SVP_ASSIGNMENTm_field32_get(unit,
                                            axp_wrx_svp, VALIDf) == 0) {
            continue;
        }
        entry_type = soc_AXP_WRX_SVP_ASSIGNMENTm_field32_get(unit, axp_wrx_svp,
                                                             ENTRY_TYPEf);
        if ((entry_type != 1) && (entry_type != 2) && (entry_type != 3)) {
            continue; /* unknown entry type */
        }
        vp = soc_AXP_WRX_SVP_ASSIGNMENTm_field32_get(unit, axp_wrx_svp,
                                             SVPf);
        source_vp_attributes_2 = soc_mem_table_idx_to_pointer
                                    (unit, SOURCE_VP_ATTRIBUTES_2m, 
                                     source_vp_attributes_2_entry_t *, 
                                     source_vp_attributes_2_buf, vp);
        rv = _bcm_vp_used_set(unit, vp, vp_info);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        switch (entry_type) {
            case 1:
                WLAN_INFO(unit)->port_info[vp].match_radio =
                    soc_AXP_WRX_SVP_ASSIGNMENTm_field32_get(unit, axp_wrx_svp,
                                                    RIDf);
                soc_mem_mac_addr_get(unit, AXP_WRX_SVP_ASSIGNMENTm, axp_wrx_svp,
                                    BSSIDf,
                                    WLAN_INFO(unit)->port_info[vp].match_bssid);
                BCM_GPORT_TUNNEL_ID_SET
                        (WLAN_INFO(unit)->port_info[vp].match_tunnel,
                         soc_AXP_WRX_SVP_ASSIGNMENTm_field32_get(unit,
                         axp_wrx_svp, EXP_TUNNEL_IDf));
                WLAN_INFO(unit)->port_info[vp].flags |=
                                                _BCM_WLAN_PORT_MATCH_BSSID_RADIO;
                break;
            case 2:
                soc_mem_mac_addr_get(unit, AXP_WRX_SVP_ASSIGNMENTm, axp_wrx_svp,
                                    BSSIDf,
                                    WLAN_INFO(unit)->port_info[vp].match_bssid);
                BCM_GPORT_TUNNEL_ID_SET
                            (WLAN_INFO(unit)->port_info[vp].match_tunnel,
                             soc_AXP_WRX_SVP_ASSIGNMENTm_field32_get(unit,
                             axp_wrx_svp, EXP_TUNNEL_IDf));
                WLAN_INFO(unit)->port_info[vp].flags |= _BCM_WLAN_PORT_MATCH_BSSID;
                break;

            case 3:
                BCM_GPORT_TUNNEL_ID_SET
                        (WLAN_INFO(unit)->port_info[vp].match_tunnel,
                        soc_AXP_WRX_SVP_ASSIGNMENTm_field32_get(unit,
                        axp_wrx_svp, TUNNEL_IDf));
                WLAN_INFO(unit)->port_info[vp].flags |= 
                                            _BCM_WLAN_PORT_MATCH_TUNNEL;
                break;
            /*
             * COVERITY
             * This default is unreachable. It is kept intentionally as a 
             * defensive default for future development.
             */
            /* coverity[dead_error_begin] */
            default:
                rv = BCM_E_INTERNAL;
                goto cleanup;
                break;
        }
        /* Update LPORT profile entry refererence count */
        lport_profile_ptr = soc_SOURCE_VP_ATTRIBUTES_2m_field32_get
                                (unit, source_vp_attributes_2,
                                 LPORT_PROFILE_INDEXf);
        rv = _bcm_lport_profile_mem_reference(unit, lport_profile_ptr, 1);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        rv = READ_LPORT_TABm(unit, MEM_BLOCK_ANY, lport_profile_ptr,
                             &lport_profile);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        /* Get TPID reference count from the LPORT entry */
        rv = _bcm_tr3_wlan_lport_tpid_recover(unit, &lport_profile);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        /* Recover flex stats from the SOURCE_VP table */
        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &source_vp);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        _bcm_tr3_wlan_port_flex_stat_recover(unit, &source_vp, vp);
        /* Get the TUNNEL_VLAN/PRI/CFI from the EGR_WLAN_DVP table */
        rv = READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, &egr_dvp_attribute);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        
        wlan_dvp_index = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit,
                                         &egr_dvp_attribute, WLAN_DVP_PROFILEf);
        entries[0] = &axp_wtx_dvp;
        rv = soc_profile_mem_get(unit, WLAN_DVP_PROFILE(unit), wlan_dvp_index,
                                 1, entries);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        
        /* Restore tunnel id bitmap */        
        tnl_idx = soc_AXP_WTX_DVP_PROFILEm_field32_get(unit,
                                             &axp_wtx_dvp, WLAN_TUNNEL_INDEXf);
        _BCM_WLAN_TNL_USED_SET(unit, tnl_idx);


        TUNNEL_VLAN(unit, tnl_idx) = 
            soc_AXP_WTX_DVP_PROFILEm_field32_get(unit, &axp_wtx_dvp, TUNNEL_VLANf);
        TUNNEL_PRI(unit, tnl_idx) = 
            soc_AXP_WTX_DVP_PROFILEm_field32_get(unit, &axp_wtx_dvp, TUNNEL_PRIf);
        TUNNEL_CFI(unit, tnl_idx) = 
            soc_AXP_WTX_DVP_PROFILEm_field32_get(unit, &axp_wtx_dvp, TUNNEL_CFIf);
        
        /* Get the VLAN validation state from TUNNEL VPs */
        if (WLAN_INFO(unit)->port_info[vp].flags &
            _BCM_WLAN_PORT_MATCH_TUNNEL) {
            vvp = soc_SOURCE_VP_ATTRIBUTES_2m_field32_get(unit, &source_vp,
                                                  VLAN_MEMBERSHIP_PROFILEf);
            for (j = BCM_VLAN_MIN; j < BCM_VLAN_MAX; j++) {
                vtab = soc_mem_table_idx_to_pointer(unit, VLAN_TABLE(unit),
                                                   vlan_tab_entry_t *, vbuf, j);

                grp_vec[0] = grp_vec[1] = 0;
                soc_mem_field_get(unit, VLAN_TABLE(unit), (uint32*)vtab, VP_GROUP_BITMAPf, grp_vec);
                COMPILER_64_ZERO(grp_bmp);
                COMPILER_64_SET(grp_bmp, grp_vec[1], grp_vec[0]);

                if (COMPILER_64_BITTEST(grp_bmp, vvp)) {
                    /* Increase use-count for this bit for this VLAN */
                    WLAN_INFO(unit)->vlan_grp_bmp_use_cnt[j][vvp]++;
                }
            }
        }
        /* Recover other port data */
        rv = _bcm_tr3_wlan_port_associated_data_recover(unit, vp, stable_size);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

cleanup:
    if (source_vp_attributes_2_buf) {
        soc_cm_sfree(unit, source_vp_attributes_2_buf);
    }
    if (axp_wtx_svp_buf) {
        soc_cm_sfree(unit, axp_wtx_svp_buf);
    }
    if (vbuf) {
        soc_cm_sfree(unit, vbuf);
    }
    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_wlan_init
 * Purpose:
 *      Initialize the WLAN software module
 * Parameters:
 *      unit     - (IN)Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_wlan_init(int unit)
{
    int i, num_vp, num_tnl, num_intf, rv = BCM_E_NONE;
    _bcm_tr3_wlan_bookkeeping_t *wlan_info = WLAN_INFO(unit);

    if (!soc_feature(unit, soc_feature_wlan)) {
        return BCM_E_UNAVAIL;
    }
    if (!L3_INFO(unit)->l3_initialized) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 module must be initialized first\n")));
        return BCM_E_NONE;
    }
    if (_tr3_wlan_initialized[unit]) {
        BCM_IF_ERROR_RETURN(bcm_tr3_wlan_detach(unit));
    }

    sal_memset(wlan_info, 0, sizeof(_bcm_tr3_wlan_bookkeeping_t));

    /* Create WLAN virtual port usage bitmap */
    num_vp = soc_mem_index_count(unit, SOURCE_VP_ATTRIBUTES_2m);
    if (wlan_info->port_info == NULL) {
        wlan_info->port_info = sal_alloc(sizeof(_bcm_tr3_wlan_port_info_t)
                                       * num_vp, "wlan_port_info");
        if (wlan_info->port_info == NULL) {
            _bcm_tr3_wlan_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(wlan_info->port_info, 0, sizeof(_bcm_tr3_wlan_port_info_t)
               * num_vp);

    /* 
     *  Create WTX_TUNNEL tunnelid usage bitmap of 1k tunnels
     *  TR3 specifies, max of 1k tunnel is supported per chip
     *  WTX_TUNNEL has 4K indexes with 4 indexes per tunnel
     */
    num_tnl = (soc_mem_index_count(unit, AXP_WTX_TUNNELm) / _BCM_WLAN_TNL_ENTRY_WIDTH);
    wlan_info->wlan_tnl_bitmap =
                            sal_alloc(SHR_BITALLOCSIZE(num_tnl), "wlan tnl_bitmap");
    if (wlan_info->wlan_tnl_bitmap == NULL) {
        _bcm_tr3_wlan_free_resources(unit);
	    return BCM_E_MEMORY;
    }
    sal_memset(wlan_info->wlan_tnl_bitmap, 0, SHR_BITALLOCSIZE(num_tnl));

    /* Allocate tunnel VLAN cache */
    if (NULL == wlan_info->tunnel_vlan) {
        wlan_info->tunnel_vlan =
                sal_alloc(sizeof(bcm_vlan_t) * num_tnl, "tunnel vlan cache");
        if (wlan_info->tunnel_vlan == NULL) {
            _bcm_tr3_wlan_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(wlan_info->tunnel_vlan, 0, sizeof(bcm_vlan_t) * num_tnl);

    /* Allocate tunnel VLAN cache */
    if (NULL == wlan_info->tunnel_pri) {
        wlan_info->tunnel_pri =
                sal_alloc(sizeof(uint8) * num_tnl, "tunnel vlan pri cache");
        if (wlan_info->tunnel_pri == NULL) {
            _bcm_tr3_wlan_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(wlan_info->tunnel_pri, 0, sizeof(uint8) * num_tnl);

    /* Allocate tunnel VLAN cache */
    if (NULL == wlan_info->tunnel_cfi) {
        wlan_info->tunnel_cfi =
                sal_alloc(sizeof(uint8) * num_tnl, "tunnel vlan cfi cache");
        if (wlan_info->tunnel_cfi == NULL) {
            _bcm_tr3_wlan_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(wlan_info->tunnel_cfi, 0, sizeof(uint8) * num_tnl);

    /* Allocate L3 interface usage bitmap */
    num_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    if (NULL == wlan_info->intf_bitmap) {
        wlan_info->intf_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_intf), "intf_bitmap");
        if (wlan_info->intf_bitmap == NULL) {
            _bcm_tr3_wlan_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(wlan_info->intf_bitmap, 0, SHR_BITALLOCSIZE(num_intf));


    /* Allocate the VLAN group bitmap use count per VLAN */
    wlan_info->vlan_grp_bmp_use_cnt = (uint8 **)sal_alloc(BCM_VLAN_COUNT *
                             sizeof(uint8 *), "VLAN group bitmap pointer list");
    if (wlan_info->vlan_grp_bmp_use_cnt == NULL) {
        _bcm_tr3_wlan_free_resources(unit);
	return BCM_E_MEMORY;
    }
    for (i = 0; i < BCM_VLAN_COUNT; i++) {
        int bitmap_size = soc_mem_field_length(unit, VLAN_TABLE(unit), VP_GROUP_BITMAPf);
        wlan_info->vlan_grp_bmp_use_cnt[i] = (uint8 *)sal_alloc(bitmap_size *
                                            sizeof(uint8), "VLAN group bitmap");
        if (wlan_info->vlan_grp_bmp_use_cnt[i] == NULL) {
            _bcm_tr3_wlan_free_resources(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(wlan_info->vlan_grp_bmp_use_cnt[i], 0, bitmap_size * sizeof(uint8));
    }

    /* Create WLAN mutex */
    if (_tr3_wlan_mutex[unit] == NULL) {
        _tr3_wlan_mutex[unit] = sal_mutex_create("wlan mutex");
        if (_tr3_wlan_mutex[unit] == NULL) {
            _bcm_tr3_wlan_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }

    /* Create WLAN profile tables */
    rv = _bcm_tr3_wlan_profile_init(unit);
    if (BCM_FAILURE(rv)) {
       _bcm_tr3_wlan_free_resources(unit);
       return rv;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        /* Warm Boot recovery */
        rv = _bcm_tr3_wlan_reinit(unit);
    }
#endif

    /* Enable egress VXLT lookup for tunnel and payload vlanids 
     * This requires to enable VXLT for WLAN ENCAP and DECAP loopback types
     */
    BCM_IF_ERROR_RETURN(bcm_esw_vlan_control_port_set
                        (unit, AXP_PORT(unit,SOC_AXP_NLF_WLAN_ENCAP), 
                               bcmVlanTranslateEgressEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_esw_vlan_control_port_set
                        (unit, AXP_PORT(unit,SOC_AXP_NLF_WLAN_DECAP), 
                               bcmVlanTranslateEgressEnable, 1));


    /* set initialize state */
    _tr3_wlan_initialized[unit] = TRUE;
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_wlan_hw_clear
 * Purpose:
 *     Perform HW tables clean up for WLAN module.
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_wlan_hw_clear(int unit)
{
    int rv, rv_error = BCM_E_NONE;

    /* Delete all ports */
    rv = bcm_tr3_wlan_port_delete_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    /* Delete all clients */
    rv = bcm_tr3_wlan_client_delete_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    return rv_error;
}

/*
 * Function:
 *      bcm_wlan_detach
 * Purpose:
 *      Detach the WLAN software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_wlan_detach(int unit)
{
    int rv = BCM_E_NONE;

    if (_tr3_wlan_initialized[unit])
    {
        if (0 == SOC_HW_ACCESS_DISABLE(unit)) {
            rv = _bcm_tr3_wlan_hw_clear(unit);
        }

        _bcm_tr3_wlan_free_resources(unit);
        _tr3_wlan_initialized[unit] = FALSE;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_wlan_client_api_to_hw
 * Purpose:
 *      Convert a hardware-independent WLAN client entry to a TR3 Wireless
 *      client database (WCD) HW entry
 * Parameters:
 *      unit - Unit number
 *      hw_entry - (OUT) Triumph 2 HW entry
 *      info - Hardware-independent WLAN client entry
 */
STATIC int
_bcm_tr3_wlan_client_api_to_hw(int unit, axp_wrx_wcd_entry_t *hw_entry,
                               bcm_wlan_client_t *info)
{
    sal_memset(hw_entry, 0, sizeof(axp_wrx_wcd_entry_t));
    if ((info->flags & BCM_WLAN_CLIENT_ROAMED_IN) &&
        (info->flags & BCM_WLAN_CLIENT_ROAMED_OUT)) {
         LOG_WARN(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "Invalid! ROAMED_IN and ROAMED_OUT flags are enabled \n")));
        return BCM_E_PARAM;
    }
    if (BCM_MAC_IS_MCAST(info->mac)) {
        return BCM_E_PARAM;
    }
    soc_AXP_WRX_WCDm_field32_set(unit, hw_entry, VALIDf, 1);
    soc_mem_mac_addr_set(unit, AXP_WRX_WCDm, hw_entry, MAC_ADDRf, info->mac);

    /* Client MACSA/MACDA is RIC, associate WTP DVP and AC2AC Home Agent DVP */
    if (info->flags & BCM_WLAN_CLIENT_ROAMED_IN) {
        int vp;
        if (!BCM_GPORT_IS_WLAN_PORT(info->home_agent) ||
            !BCM_GPORT_IS_WLAN_PORT(info->wtp)) {
            return BCM_E_PARAM;
        }
        soc_AXP_WRX_WCDm_field32_set(unit, hw_entry, RICf, 1);
        vp = BCM_GPORT_WLAN_PORT_ID_GET(info->home_agent);
        soc_AXP_WRX_WCDm_field32_set(unit, hw_entry, RIC_HA_VPf, vp);
        vp = BCM_GPORT_WLAN_PORT_ID_GET(info->wtp);
        soc_AXP_WRX_WCDm_field32_set(unit, hw_entry, RIC_WTP_VPf, vp);
    }

    /* Client MACSA is ROC, DVP is determined by forwarding logic */
    if (info->flags & BCM_WLAN_CLIENT_ROAMED_OUT) {
        soc_AXP_WRX_WCDm_field32_set(unit, hw_entry, ROCf, 1);
    }

    if (info->flags & BCM_WLAN_CLIENT_AUTHORIZED) {
        soc_AXP_WRX_WCDm_field32_set(unit, hw_entry, DOT1X_STATEf, 1);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_wlan_client_api_from_hw
 * Purpose:
 *      Convert a TR3 Wireless Client Datbase(WCD) HW entry to a hardware-independent
 *      WLAN client entry
 * Parameters:
 *      unit - Unit number
 *      hw_entry - Triumph 3 HW entry
 *      info - (OUT) Hardware-independent WLAN client entry
 */
STATIC int
_bcm_tr3_wlan_client_api_from_hw(int unit, bcm_wlan_client_t *info,
                                 axp_wrx_wcd_entry_t *hw_entry)
{
    int vp;
    sal_memset(info, 0, sizeof (bcm_wlan_client_t));
    soc_mem_mac_addr_get(unit, AXP_WRX_WCDm, hw_entry, MAC_ADDRf,
                         info->mac);
    if (soc_AXP_WRX_WCDm_field32_get(unit, hw_entry, RICf)) {
        info->flags |= BCM_WLAN_CLIENT_ROAMED_IN;
        vp = soc_AXP_WRX_WCDm_field32_get(unit, hw_entry,
                                         RIC_HA_VPf);
        BCM_GPORT_WLAN_PORT_ID_SET(info->home_agent, vp);
        vp = soc_AXP_WRX_WCDm_field32_get(unit, hw_entry,
                                         RIC_WTP_VPf);
        BCM_GPORT_WLAN_PORT_ID_SET(info->wtp, vp);
    }
    if (soc_AXP_WRX_WCDm_field32_get(unit, hw_entry, ROCf)) {
        info->flags |= BCM_WLAN_CLIENT_ROAMED_OUT;
    }
    if (soc_AXP_WRX_WCDm_field32_get(unit, hw_entry, DOT1X_STATEf)) {
        info->flags |= BCM_WLAN_CLIENT_AUTHORIZED;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_wlan_client_add
 * Purpose:
 *      Add a wireless client to the wireless client database
 * Parameters:
 *      unit     - Device Number
 *      info     - (IN/OUT) Wireless Client structure
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_wlan_client_add(int unit, bcm_wlan_client_t *info)
{
    int index, rv = BCM_E_UNAVAIL;
    axp_wrx_wcd_entry_t client_entry_key, client_entry_lookup;

    WLAN_INIT(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_tr3_wlan_client_api_to_hw(unit, &client_entry_key, info));

    WLAN_LOCK(unit);
    rv = soc_mem_search(unit, AXP_WRX_WCDm, MEM_BLOCK_ANY, &index,
                        (void *)&client_entry_key,
                        (void *)&client_entry_lookup, 0);

    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        WLAN_UNLOCK(unit);
        return rv;
    } else if ((rv == BCM_E_NONE) &&
               !(info->flags & BCM_WLAN_CLIENT_REPLACE)) {
        WLAN_UNLOCK(unit);
        return BCM_E_EXISTS;
    }

    rv = soc_mem_insert(unit, AXP_WRX_WCDm, MEM_BLOCK_ALL,
                                       (void *)&client_entry_key);
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_wlan_client_delete
 * Purpose:
 *      Delete a wireless client from the database
 * Parameters:
 *      unit     - Device Number
 *      mac      - MAC address of client (lookup key)
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_wlan_client_delete(int unit, bcm_mac_t mac)
{
    int index, rv = BCM_E_UNAVAIL;
    axp_wrx_wcd_entry_t client_entry_key, client_entry_lookup;

    WLAN_INIT(unit);

    sal_memset(&client_entry_key, 0, sizeof(axp_wrx_wcd_entry_t));
    sal_memset(&client_entry_lookup, 0, sizeof(axp_wrx_wcd_entry_t));

    soc_AXP_WRX_WCDm_field32_set(unit, &client_entry_key, VALIDf, 1);
    soc_mem_mac_addr_set(unit, AXP_WRX_WCDm, &client_entry_key,
                         MAC_ADDRf, mac);
    WLAN_LOCK(unit);
    rv = soc_mem_search(unit, AXP_WRX_WCDm, MEM_BLOCK_ANY, &index,
                        (void *)&client_entry_key,
                        (void *)&client_entry_lookup, 0);
    if (BCM_FAILURE(rv)) {
        WLAN_UNLOCK(unit);
        return rv;
    }
    rv = soc_mem_delete(unit, AXP_WRX_WCDm, MEM_BLOCK_ALL,
                        (void *)&client_entry_key);
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_wlan_client_delete_all
 * Purpose:
 *      Delete all wireless clients from the database
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_wlan_client_delete_all(int unit)
{
    int i, valid, index_min, index_max, rv = BCM_E_NONE;
    axp_wrx_wcd_entry_t hw_entry;

    WLAN_INIT(unit);

    index_min = soc_mem_index_min(unit, AXP_WRX_WCDm);
    index_max = soc_mem_index_max(unit, AXP_WRX_WCDm);
    WLAN_LOCK(unit);
    for (i = index_min; i <= index_max; i++) {
        rv = soc_mem_read(unit, AXP_WRX_WCDm, MEM_BLOCK_ANY, i,
                          (void *)&hw_entry);
        if (BCM_FAILURE(rv)) {
            break;
        }
        valid = soc_AXP_WRX_WCDm_field32_get(unit, &hw_entry, VALIDf);
        if (!valid ) {
            continue;
        }
        soc_AXP_WRX_WCDm_field32_set(unit, &hw_entry, VALIDf, 0);
        rv = soc_mem_write(unit, AXP_WRX_WCDm, MEM_BLOCK_ALL, i,
                           (void *)&hw_entry);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_wlan_client_get
 * Purpose:
 *      Get a wireless client from the database
 * Parameters:
 *      unit     - Device Number
 *      mac      - MAC address of client (lookup key)
 *      info     - (IN/OUT) Wireless Client structure
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_wlan_client_get(int unit, bcm_mac_t mac, bcm_wlan_client_t *info)
{
    int index, rv = BCM_E_UNAVAIL;
    axp_wrx_wcd_entry_t client_entry_key, client_entry_lookup;

    WLAN_INIT(unit);

    sal_memset(&client_entry_key, 0, sizeof(axp_wrx_wcd_entry_t));
    sal_memset(&client_entry_lookup, 0, sizeof(axp_wrx_wcd_entry_t));

    soc_AXP_WRX_WCDm_field32_set(unit, &client_entry_key, VALIDf, 1);
    soc_mem_mac_addr_set(unit, AXP_WRX_WCDm, &client_entry_key,
                         MAC_ADDRf, mac);
    rv = soc_mem_search(unit, AXP_WRX_WCDm, MEM_BLOCK_ANY, &index,
                        (void *)&client_entry_key,
                        (void *)&client_entry_lookup, 0);
    BCM_IF_ERROR_RETURN(rv);
    bcm_wlan_client_t_init(info);
    rv = _bcm_tr3_wlan_client_api_from_hw(unit, info, &client_entry_lookup);
    return rv;
}

/*
 * Function:
 *      bcm_wlan_client_traverse
 * Purpose:
 *      Walks through the valid WLAN client entries and calls
 *      the user supplied callback function for each entry.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      trav_fn    - (IN) Callback function.
 *      user_data  - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_tr3_wlan_client_traverse(int unit,
                             bcm_wlan_client_traverse_cb cb,
                             void *user_data)
{
    int i, index_min, index_max, rv = BCM_E_NONE;
    int valid, *buffer = NULL;
    axp_wrx_wcd_entry_t *hw_entry;
    bcm_wlan_client_t info;

    WLAN_INIT(unit);

    index_min = soc_mem_index_min(unit, AXP_WRX_WCDm);
    index_max = soc_mem_index_max(unit, AXP_WRX_WCDm);

    WLAN_LOCK(unit);
    buffer = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, AXP_WRX_WCDm),
                           "wlan client traverse");
    if (NULL == buffer) {
        WLAN_UNLOCK(unit);
        return BCM_E_MEMORY;
    }
    if ((rv = soc_mem_read_range(unit, AXP_WRX_WCDm, MEM_BLOCK_ALL,
                                 index_min, index_max, buffer)) < 0 ) {
        soc_cm_sfree(unit, buffer);
        WLAN_UNLOCK(unit);
        return rv;
    }
    for (i = index_min; i <= index_max; i++) {
        hw_entry = soc_mem_table_idx_to_pointer(unit, AXP_WRX_WCDm,
                                         axp_wrx_wcd_entry_t *, buffer, i);
        valid = soc_AXP_WRX_WCDm_field32_get(unit, hw_entry, VALIDf);
        if (!valid ) {
            continue;
        }
        bcm_wlan_client_t_init(&info);
        rv = _bcm_tr3_wlan_client_api_from_hw(unit, &info, hw_entry);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, buffer);
            WLAN_UNLOCK(unit);
            return rv;
        }
        rv = cb(unit, &info, user_data);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, buffer);
            WLAN_UNLOCK(unit);
            return rv;
        }
    }
    soc_cm_sfree(unit, buffer);
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_wlan_port_resolve
 * Purpose:
 *      Get the modid, port, trunk values for a WLAN port
 * Returns:
 *      BCM_E_XXX
 */
/*
 * Function:
 *      _bcm_tr3_wlan_port_resolve
 * Purpose:
 *      Get the modid, port, trunk values for a WLAN port
 * Parameters:
 *      unit            - (IN) bcm device
 *      wlan_port_id    - (IN) wlan gport
 *      modid           - (OUT) module id
 *      port            - (OUT) local port id
 *      trunk_id        - (OUT) trunk id
 *      id              - (OUT) virtual port id
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_tr3_wlan_port_resolve(int unit, bcm_gport_t wlan_port_id,
                          bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id)

{
    int rv = BCM_E_NONE, nh_index, vp;
    ing_l3_next_hop_entry_t ing_nh;
    ing_dvp_table_entry_t dvp;

    WLAN_INIT(unit);

    if (!BCM_GPORT_IS_WLAN_PORT(wlan_port_id)) {
        return (BCM_E_BADID);
    }

    vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
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

/*
 * Function:
 *      bcm_tr3_wlan_port_learn_get
 * Purpose:
 *      Get the CPU Managed Learning (CML) bits for an wlan port
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_wlan_port_learn_get(int unit, bcm_gport_t wlan_port_id, uint32 *flags)
{
    int rv, cml = 0;
    uint32 vp;
    source_vp_entry_t svp;
    int lport_ptr = 0;
    lport_tab_entry_t lport_profile;
    rtag7_port_based_hash_entry_t rtag7_entry;
    void *entries[2];

    WLAN_INIT(unit);

    /* Get the VP index from the gport */
    vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port_id);

    /* Be sure the entry is used and is set for wlan */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
        return BCM_E_NOT_FOUND;
    }
    rv = READ_SOURCE_VP_ATTRIBUTES_2m(unit, MEM_BLOCK_ANY, vp, &svp);
    BCM_IF_ERROR_RETURN(rv);

    /* Get lport_profile */
    lport_ptr = soc_SOURCE_VP_ATTRIBUTES_2m_field32_get(unit, &svp,
                                                LPORT_PROFILE_INDEXf);
    entries[0] = &lport_profile;
    entries[1] = &rtag7_entry;
    rv = _bcm_lport_profile_entry_get(unit, lport_ptr, 1, entries);
    BCM_IF_ERROR_RETURN(rv);
    cml = soc_LPORT_TABm_field32_get(unit, &lport_profile, CML_FLAGS_NEWf);

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

/*
 * Function:
 *      bcm_tr3_wlan_port_learn_set
 * Purpose:
 *      Set the CML bits for an wlan port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_wlan_port_learn_set(int unit, bcm_gport_t wlan_port_id, uint32 flags)
{
    int cml_old = 0, cml = 0, rv = BCM_E_NONE;
    uint32 vp;
    source_vp_attributes_2_entry_t svp;
    lport_tab_entry_t lport_profile;
    rtag7_port_based_hash_entry_t rtag7_entry;
    int lport_ptr, old_lport_ptr = -1;
    void *entries[2];

    WLAN_INIT(unit);

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
    vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port_id);

    WLAN_LOCK(unit);
    /* Be sure the entry is used and is set for WLAN */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
        WLAN_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }
    rv = READ_SOURCE_VP_ATTRIBUTES_2m(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        return rv;
    }
    /* Get old lport_profile */
    old_lport_ptr = soc_SOURCE_VP_ATTRIBUTES_2m_field32_get(unit, &svp,
                                                    LPORT_PROFILE_INDEXf);
    entries[0] = &lport_profile;
    entries[1] = &rtag7_entry;
    rv = _bcm_lport_profile_entry_get(unit, old_lport_ptr, 1, entries);
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        return rv;
    }
    cml_old = soc_LPORT_TABm_field32_get(unit, &lport_profile, CML_FLAGS_NEWf);
    if (cml != cml_old) {
        soc_LPORT_TABm_field32_set(unit, &lport_profile, CML_FLAGS_MOVEf, cml);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, CML_FLAGS_NEWf, cml);
        rv = _bcm_lport_profile_entry_add(unit, entries, 1,
                                          (uint32 *) &lport_ptr);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            return rv;
        }
        soc_SOURCE_VP_ATTRIBUTES_2m_field32_set(unit, &svp, LPORT_PROFILE_INDEXf,
                                        lport_ptr);
        /* coverity[negative_returns : FALSE] */
        rv = WRITE_SOURCE_VP_ATTRIBUTES_2m(unit, MEM_BLOCK_ALL, vp, &svp);
        if (BCM_FAILURE(rv)) {
            WLAN_UNLOCK(unit);
            return rv;
        }
        rv = _bcm_lport_profile_entry_delete(unit, old_lport_ptr);
    }
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_wlan_l3_intf_add
 * Purpose:
 *      This function adds egress l3 interface for wlan
 * Parameters:
 *      unit       - (IN) bcm device
 *      if_info    - (OUT) egress l3 interface
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_tr3_wlan_l3_intf_add(int unit, _bcm_l3_intf_cfg_t *if_info)
{
    int i, num_intf;
    egr_l3_intf_entry_t egr_intf;
    bcm_mac_t hw_mac;
    num_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    for (i = 0; i < num_intf; i++) {
        if (_BCM_WLAN_INTF_USED_GET(unit, i)) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_INTFm,
                                             MEM_BLOCK_ANY, i, &egr_intf));
            soc_mem_mac_addr_get(unit, EGR_L3_INTFm, &egr_intf,
                                 MAC_ADDRESSf, hw_mac);
            if (SAL_MAC_ADDR_EQUAL(hw_mac, if_info->l3i_mac_addr)) {
                if_info->l3i_index = i;
                return BCM_E_NONE;
            }
        }
    }
    /* Create an interface */
    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_intf_create(unit, if_info));
    _BCM_WLAN_INTF_USED_SET(unit, if_info->l3i_index);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_wlan_trunk_group_update
 * Purpose:
 *      Function to update trunk group bitmap entry
 *
 * Parameters:
 *      unit       - (IN) bcm device
 *      trunk     - (IN) Trunk ID
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_tr3_wlan_trunk_group_update(int unit, int trunk)
{
    int i, mem_size;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count;
    axp_wtx_trunk_group_bitmap_entry_t trunk_group_bitmap_entry; /* AXP Trunk bitmap */
    bcm_pbmp_t trunk_pbmp;
    soc_mem_t   mem = AXP_WTX_TRUNK_GROUP_BITMAPm;
    int rv = BCM_E_NONE;

    mem_size = soc_mem_index_count(unit, mem) ;
    /* mem doesn't exist. no necessary to update */
    if ( mem_size <= 0 ) {
        return BCM_E_NONE;
    }

    if ( trunk >= mem_size ) {
        return BCM_E_PARAM;
    }

    sal_memset(&trunk_group_bitmap_entry, 0, sizeof(trunk_group_bitmap_entry));

    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, trunk,
        SOC_MAX_NUM_PORTS, local_member_array, &local_member_count));

    BCM_PBMP_CLEAR(trunk_pbmp);
    for (i = 0; i < local_member_count; i++) {
        BCM_PBMP_PORT_ADD(trunk_pbmp, local_member_array[i]);
    }
    soc_mem_pbmp_field_set(unit, mem, &trunk_group_bitmap_entry,
        TRUNK_GROUP_BITMAPf, &trunk_pbmp);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, trunk, &trunk_group_bitmap_entry);

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_wlan_trunk_block_mask_update
 * Purpose:
 *      Function to update trunk block mask table
 *
 * Parameters:
 *      unit       - (IN) bcm device
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_tr3_wlan_trunk_block_mask_update(int unit)
{
    int index, mem_size1, mem_size2, mem_size_min;
    soc_mem_t  mem1, mem2;
    bcm_pbmp_t pbmp;
    nonucast_trunk_block_mask_entry_t *mask_entry_1;
    nonucast_trunk_block_mask_entry_t *mask_table_1;
    axp_wtx_trunk_block_mask_entry_t *mask_entry_2;
    axp_wtx_trunk_block_mask_entry_t *mask_table_2;
    int rv = BCM_E_NONE;

    mem1 = NONUCAST_TRUNK_BLOCK_MASKm;
    mem2 = AXP_WTX_TRUNK_BLOCK_MASKm;

    mem_size1 = soc_mem_index_count(unit, mem1);
    mem_size2 = soc_mem_index_count(unit, mem2);

    /* mem doesn't exist. no necessary to update */
    if (( mem_size1 <= 0 ) || ( mem_size2 <= 0 ) ) {
        return BCM_E_NONE;
    }

    mem_size_min = mem_size1 > mem_size2 ?  mem_size2 : mem_size1;

    mask_table_1 = soc_cm_salloc(unit,
                                    sizeof(nonucast_trunk_block_mask_entry_t) * mem_size_min,
                                    "nonuc trunk mask tbl dma");
    mask_table_2 = soc_cm_salloc(unit,
                                    sizeof(axp_wtx_trunk_block_mask_entry_t) * mem_size_min,
                                    "axp wtx trunk mask dma");

    soc_mem_lock(unit, mem1);
    if ((rv = soc_mem_read_range(unit, mem1, MEM_BLOCK_ANY, 0,
                            mem_size_min - 1, mask_table_1)) < 0) {
        soc_mem_unlock(unit, mem1);
        soc_cm_sfree(unit, mask_table_1);
        soc_cm_sfree(unit, mask_table_2);
        return rv;
    }

    soc_mem_lock(unit, mem2);
    if ((rv = soc_mem_read_range(unit, mem2, MEM_BLOCK_ANY, 0,
                            mem_size_min - 1, mask_table_2)) < 0) {
        soc_mem_unlock(unit, mem1);
        soc_mem_unlock(unit, mem2);
        soc_cm_sfree(unit, mask_table_1);
        soc_cm_sfree(unit, mask_table_2);
        return rv;
    }

    for (index = 0; index < mem_size_min; index++) {
        mask_entry_1 = soc_mem_table_idx_to_pointer(unit, mem1,
                                            nonucast_trunk_block_mask_entry_t *,
                                            mask_table_1, index);
        soc_mem_pbmp_field_get(unit, mem1, mask_entry_1, BLOCK_MASKf, &pbmp);

        mask_entry_2 = soc_mem_table_idx_to_pointer(unit, mem2,
                                            axp_wtx_trunk_block_mask_entry_t *,
                                            mask_table_2, index);
        soc_mem_pbmp_field_set(unit, mem2, mask_entry_2, TRUNK_BLOCK_MASKf, &pbmp);
    }

    rv = soc_mem_write_range(unit, mem2, MEM_BLOCK_ANY, 0, mem_size_min - 1, mask_table_2);

    soc_mem_unlock(unit, mem2);
    soc_mem_unlock(unit, mem1);

    soc_cm_sfree(unit, mask_table_1);
    soc_cm_sfree(unit, mask_table_2);

    return rv;
}

typedef struct _bcm_tr3_ing_nh_info_s {
    int      port;
    int      module;
    int      trunk;
    uint32   mtu;
} _bcm_tr3_ing_nh_info_t;

typedef struct _bcm_tr3_egr_nh_info_s {
    uint16    port;
    uint8     entry_type;
    uint16    capwap_mc_bitmap;
    int       dvp;
    int       intf_num;
    bcm_mac_t macda;
} _bcm_tr3_egr_nh_info_t;

/*
 * Function:
 *      bcm_tr3_wlan_nh_info_add
 * Purpose:
 *      This function adds resolved wlan gport in ingress and egress next hop
 *      table
 * Parameters:
 *      unit        - (IN) bcm device
 *      wlan_port   - (IN) wlan port info
 *      vp          - (IN) virtual port
 *      drop        - (IN) drop packet flag
 *      nh_index    - (OUT) nh_index
 *      local_port  - (OUT) outgoing local port
 *      is_local    - (OUT) local port flag
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 *      Get DVP from wlan gport
 *      Get next hop index, calling bcm_xgs3_nh_add
 *      Resolve wlan gport to port, modid, trunk id
 *      Resolve trunk ports to physical ports
 *      Add to L3 ingress next hop table with L2 DVP type
 *      Create L2 interface and add l2 intf, dvp toegress next hop table
 */
STATIC int
_bcm_tr3_wlan_nh_info_add(int unit, bcm_wlan_port_t *wlan_port, int vp, int drop,
                           int *nh_index, bcm_port_t *local_port, int *is_local)
{
    initial_ing_l3_next_hop_entry_t initial_ing_nh; /* Initial ingress L3 NH  */
    ing_l3_next_hop_entry_t     ing_nh;             /* Ingress Next hop entry */
    egr_l3_next_hop_entry_t     egr_nh;             /* Egress Next hop entry  */
    _bcm_tr3_ing_nh_info_t      ing_nh_info;        /* Ingress Next hop info  */
    _bcm_tr3_egr_nh_info_t      egr_nh_info;        /* Egress Next hop info   */
    bcm_l3_egress_t             nh_info;            /* L3 Next hop info       */
    _bcm_l3_intf_cfg_t          if_info;            /* L3 Interface           */
    uint32                      nh_flags;           /* next hop flags         */
    int                         gport_id, rv;       /* function vars          */
    bcm_module_t                mod_out;            /* Port module out id     */
    bcm_port_t                  port_out;           /* port destined          */
    bcm_trunk_t                 trunk_id;           /* Trunk Id               */
    _bcm_port_info_t            *info;              /* port info              */
    uint32                      mtu_profile_index;  /* Index to MTU profile   */
    uint32                      port_flags;

    /* Initialize values */
    sal_memset(&ing_nh_info, 0, sizeof(_bcm_tr3_ing_nh_info_t));
    sal_memset(&egr_nh_info, 0, sizeof(_bcm_tr3_egr_nh_info_t));
    *local_port = 0;
    *is_local = 0;
    ing_nh_info.mtu = 0x3fff;
    ing_nh_info.port = -1;
    ing_nh_info.module = -1;
    ing_nh_info.trunk = -1;

    egr_nh_info.dvp = vp;           /* dvp port */
    egr_nh_info.intf_num = -1;
    egr_nh_info.capwap_mc_bitmap = 0;
    egr_nh_info.entry_type = 0x4;   /* wlan entry type */

    if (wlan_port->flags & BCM_WLAN_PORT_REPLACE) {
        if ((*nh_index > soc_mem_index_max(unit, EGR_L3_NEXT_HOPm)) ||
            (*nh_index < soc_mem_index_min(unit, EGR_L3_NEXT_HOPm)))  {
            return BCM_E_PARAM;
        }
        /* Read the existing egress next_hop entry */
        rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                          *nh_index, &egr_nh);
        BCM_IF_ERROR_RETURN(rv);
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

    /* Resolve the gport */
    rv = _bcm_esw_gport_resolve(unit, wlan_port->port, &mod_out,
                                &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

    if (BCM_GPORT_IS_TRUNK(wlan_port->port)) {
        ing_nh_info.module = -1;
        ing_nh_info.port = -1;
        egr_nh_info.port = -1;
        ing_nh_info.trunk = trunk_id;
        WLAN_INFO(unit)->port_info[vp].modid = -1;
        WLAN_INFO(unit)->port_info[vp].port = -1;
        WLAN_INFO(unit)->port_info[vp].tgid = trunk_id;
    } else {
        ing_nh_info.module = mod_out;
        ing_nh_info.port = port_out;
        egr_nh_info.port = port_out;
        ing_nh_info.trunk = -1;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_modid_is_local(unit, mod_out, is_local));
        if (TRUE == *is_local) {
            /* Indicated to calling function that this is a local port */
            *is_local = 1;
            *local_port = ing_nh_info.port;
        }
        WLAN_INFO(unit)->port_info[vp].modid = mod_out;
        WLAN_INFO(unit)->port_info[vp].port = port_out;
        WLAN_INFO(unit)->port_info[vp].tgid = -1;
    }

    if (wlan_port->flags & BCM_WLAN_PORT_EGRESS_CLIENT_MULTICAST) {
        /* CAPWAP_WLAN_MC_BITMAP and MAC_ADDRESS are overlays */
        egr_nh_info.capwap_mc_bitmap = wlan_port->client_multicast;
    }

    /* Write INITIAL_ING_L3_NEXT_HOP entry */
    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    if (ing_nh_info.trunk == -1) {
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, PORT_NUMf, ing_nh_info.port);
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, MODULE_IDf, ing_nh_info.module);
    } else {
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, Tf, 1);
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, TGIDf, ing_nh_info.trunk);
        BCM_GPORT_TRUNK_SET(*local_port, ing_nh_info.trunk);
    }
    rv = soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, *nh_index, &initial_ing_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Add an L3 interface entry with L2_SWITCH=1 - ref count if exists */
    sal_memset(&if_info, 0, sizeof(_bcm_l3_intf_cfg_t));
    if_info.l3i_flags |= BCM_L3_L2ONLY | BCM_L3_SECONDARY;
    rv = _bcm_tr3_wlan_l3_intf_add(unit, &if_info);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Populate the fields of WLAN::EGR_l3_NEXT_HOP */
    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        ENTRY_TYPEf, egr_nh_info.entry_type);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        WLAN__DVPf, egr_nh_info.dvp);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        WLAN__INTF_NUMf, if_info.l3i_index);
    if (WLAN_INFO(unit)->port_info[vp].tgid == -1) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, WLAN__DGLPf,
                            BCM_L3_DGLP_GET(mod_out, egr_nh_info.port));
    } else {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, WLAN__DGLPf,
                            BCM_TGID_TRUNK_INDICATOR(unit) | WLAN_INFO(unit)->port_info[vp].tgid);
    }

    if (egr_nh_info.capwap_mc_bitmap != 0) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            WLAN__CAPWAP_WLAN_MC_BITMAPf,
                            egr_nh_info.capwap_mc_bitmap);
    }

    /* Write EGR_L3_NEXT_HOP entry */
    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, *nh_index, &egr_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Write ING_L3_NEXT_HOP entry */
    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, drop);
    if (ing_nh_info.trunk == -1) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, PORT_NUMf, ing_nh_info.port);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, MODULE_IDf, ing_nh_info.module);
    } else {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, Tf, 1);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, TGIDf, ing_nh_info.trunk);
    }
    if (drop) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, drop);
    }
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                       &ing_nh, ENTRY_TYPEf, 0x2); /* L2 DVP */
    /* L3 api interfaces for ING_L3_ATTRIBUTE table */
    BCM_IF_ERROR_RETURN(_bcm_tr3_mtu_profile_index_get(unit, 
                                   ing_nh_info.mtu, &mtu_profile_index));
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                   &ing_nh, DVP_ATTRIBUTE_1_INDEXf, mtu_profile_index);
    /* Enable NLF forwarding */
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                       &ing_nh, NLF_FORWARDING_ENABLEf, 1);
    rv = soc_mem_write (unit, ING_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, *nh_index, &ing_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Update the physical port's SW state */
    if (*is_local) {
        bcm_port_t phys_port = WLAN_INFO(unit)->port_info[vp].port;
        if (soc_feature(unit, soc_feature_sysport_remap)) {
            BCM_XLATE_SYSPORT_S2P(unit, &phys_port);
        }
        _bcm_port_info_access(unit, phys_port, &info);
        info->vp_count++;
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_vlan_member_get(
                unit, phys_port, &port_flags));
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_vlan_member_set(
                unit, phys_port, port_flags));
    }

    /* If associated with a trunk, update each local physical port's SW state */
    if (ing_nh_info.trunk != -1) {
        int idx;
        bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
        int local_member_count;

        rv = _bcm_esw_trunk_local_members_get(unit, trunk_id,
                SOC_MAX_NUM_PORTS, local_member_array, &local_member_count);
        if (rv < 0) {
            goto cleanup;
        }
        for (idx = 0; idx < local_member_count; idx++) {
            _bcm_port_info_access(unit, local_member_array[idx], &info);
            info->vp_count++;
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_get(
                    unit, local_member_array[idx], &port_flags));
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_set(
                    unit, local_member_array[idx], port_flags));
        }
    }

    /* If associated with a trunk, update axp trunk group bitmap table */
    if (WLAN_INFO(unit)->port_info[vp].tgid != -1) {
        _bcm_tr3_wlan_trunk_group_update(unit, WLAN_INFO(unit)->port_info[vp].tgid);
    }

    return rv;

cleanup:
    if (!(wlan_port->flags & BCM_WLAN_PORT_REPLACE)) {
        (void) bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, *nh_index);
    }
    return rv;
}

/*
 * Function:
 *      bcm_tr3_wlan_nh_info_delete
 * Purpose:
 *      This function clears and delete next hop table
 * Parameters:
 *      unit        - (IN) bcm device
 *      nh_index    - (IN) nh_index
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 *      Clear Ingress and egress next hop table
 *      Delete Next hop index
 */
STATIC int
_bcm_tr3_wlan_nh_info_delete(int unit, int nh_index)
{
    int rv = BCM_E_NONE;
    initial_ing_l3_next_hop_entry_t initial_ing_nh;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    int attr_index;

    if (nh_index <= 0) {
        return BCM_E_NONE;
    }    

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm,
                                  MEM_BLOCK_ANY, nh_index, &egr_nh));

    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                ENTRY_TYPEf) != 0x4) {
        /* Not a WLAN VP */
        return BCM_E_NOT_FOUND;
    }

    /* Clear EGR_L3_NEXT_HOP entry */
    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &egr_nh));

    /* Read  ING_L3_NEXT_HOPm */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm,
                                  MEM_BLOCK_ANY, nh_index, &ing_nh));


    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh,
                                                ENTRY_TYPEf) != 0x2) {
        /* Not a L2 DVP */
        return BCM_E_NOT_FOUND;
    }


    /* Delete ING_L3_NEXT_HOP_ATTRIBUTE_1 reference */
    attr_index = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh,
                                                DVP_ATTRIBUTE_1_INDEXf);
    BCM_IF_ERROR_RETURN(_bcm_ing_l3_nh_attrib_entry_delete(unit, attr_index));

    /* Clear ING_L3_NEXT_HOP entry */
    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write (unit, ING_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &ing_nh));

    /* Clear INITIAL_ING_L3_NEXT_HOP entry */
    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &initial_ing_nh));

    /* Free the next-hop entry. */
    rv = bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, nh_index);
    return rv;
}
/*
 * Function:
 *      _bcm_tr3_wlan_nh_info_get
 * Purpose:
 *      This funtion gets wlan port info from next hop index
 * Parameters:
 *      unit       - (IN) bcm device
 *      wlan_port  - (IN/OUT) wlan port info
 *      nh_index   - next hop index
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_tr3_wlan_nh_info_get(int unit, bcm_wlan_port_t *wlan_port, int nh_index)
{
    ing_l3_next_hop_entry_t ing_nh;
    bcm_module_t mod_out, mod_in;
    bcm_port_t port_out, port_in;
    bcm_trunk_t trunk_id;

    /* Read the HW entries */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      nh_index, &ing_nh));

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh,
                                                ENTRY_TYPEf) == 0x2) {
        if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
            trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
            BCM_GPORT_TRUNK_SET(wlan_port->port, trunk_id);
        } else {
            mod_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
            port_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);

            SOC_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                         mod_in, port_in, &mod_out, &port_out));
            BCM_GPORT_MODPORT_SET(wlan_port->port, mod_out, port_out);
        }
    } else {
        return BCM_E_NOT_FOUND;
    }

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, DROPf)) {
        wlan_port->flags |= BCM_WLAN_PORT_DROP;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_wlan_match_get
 * Purpose:
 *      Get wlan port info for the virtual port
 * Parameters:
 *      unit       - (IN) bcm device
 *      vp         - (IN) Virtual port no.
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_tr3_wlan_match_get(int unit, bcm_wlan_port_t *wlan_port, int vp)
{
    int rv = BCM_E_NONE;
    uint64 temp;

    if (WLAN_INFO(unit)->port_info[vp].flags &
         _BCM_WLAN_PORT_MATCH_TUNNEL) {

        wlan_port->flags |= BCM_WLAN_PORT_MATCH_TUNNEL;
        wlan_port->match_tunnel = WLAN_INFO(unit)->port_info[vp].match_tunnel;

    } else if (WLAN_INFO(unit)->port_info[vp].flags &
               _BCM_WLAN_PORT_MATCH_BSSID_RADIO) {
        wlan_port->flags |= BCM_WLAN_PORT_BSSID_RADIO;

        SAL_MAC_ADDR_TO_UINT64
           (WLAN_INFO(unit)->port_info[vp].match_bssid, temp);
        SAL_MAC_ADDR_FROM_UINT64(wlan_port->bssid, temp);
        wlan_port->radio = WLAN_INFO(unit)->port_info[vp].match_radio;
        wlan_port->match_tunnel = WLAN_INFO(unit)->port_info[vp].match_tunnel;

    } else if (WLAN_INFO(unit)->port_info[vp].flags &
               _BCM_WLAN_PORT_MATCH_BSSID) {
        wlan_port->flags |= BCM_WLAN_PORT_BSSID;

        SAL_MAC_ADDR_TO_UINT64
           (WLAN_INFO(unit)->port_info[vp].match_bssid, temp);
        SAL_MAC_ADDR_FROM_UINT64(wlan_port->bssid, temp);
        wlan_port->match_tunnel = WLAN_INFO(unit)->port_info[vp].match_tunnel;
    }
    return rv;
}
/*
 * Function:
 *      _bcm_tr3_wlan_match_delete
 * Purpose:
 *      Tunnel Id, BSSID, RID keys are matched to delete Source Virtual port.
 * Parameters:
 *      unit       - (IN) bcm device
 *      vp         - (IN) Virtual port no.
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_tr3_wlan_match_delete(int unit, int vp)
{
    int  rv = BCM_E_NONE;
    axp_wrx_svp_assignment_entry_t wlan_svp;
    uint32 tunnel;

    sal_memset(&wlan_svp, 0, sizeof(axp_wrx_svp_assignment_entry_t));

    if (WLAN_INFO(unit)->port_info[vp].flags & _BCM_WLAN_PORT_MATCH_BSSID) {
        soc_AXP_WRX_SVP_ASSIGNMENTm_field32_set(unit, &wlan_svp, ENTRY_TYPEf, 0x2);
        soc_mem_mac_addr_set(unit, AXP_WRX_SVP_ASSIGNMENTm, &wlan_svp, BSSIDf,
                             WLAN_INFO(unit)->port_info[vp].match_bssid);

    } else if (WLAN_INFO(unit)->port_info[vp].flags & _BCM_WLAN_PORT_MATCH_BSSID_RADIO) {
        soc_AXP_WRX_SVP_ASSIGNMENTm_field32_set(unit, &wlan_svp, ENTRY_TYPEf, 0x1);
        soc_mem_mac_addr_set(unit, AXP_WRX_SVP_ASSIGNMENTm, &wlan_svp, BSSIDf,
                             WLAN_INFO(unit)->port_info[vp].match_bssid);
        soc_AXP_WRX_SVP_ASSIGNMENTm_field32_set(unit, &wlan_svp, RIDf,
                                        WLAN_INFO(unit)->port_info[vp].match_radio);

    } else if (WLAN_INFO(unit)->port_info[vp].flags & _BCM_WLAN_PORT_MATCH_TUNNEL) {
        soc_AXP_WRX_SVP_ASSIGNMENTm_field32_set(unit, &wlan_svp, ENTRY_TYPEf, 0x3);
        tunnel = BCM_GPORT_TUNNEL_ID_GET(WLAN_INFO(unit)->port_info[vp].match_tunnel);
        soc_AXP_WRX_SVP_ASSIGNMENTm_field32_set(unit, &wlan_svp, TUNNEL_IDf,
                                        tunnel);
    } else {
        return rv;
    }

    rv = soc_mem_delete(unit, AXP_WRX_SVP_ASSIGNMENTm, MEM_BLOCK_ALL, &wlan_svp);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_wlan_match_add
 * Purpose:
 *      Tunnel Id, BSSID, RID keys are matched to assign Source Virtual port.
 *      This function adds Expected Tunnel Id, Tunnel Id, BSSID, RID keys for
 *      the Virtual port in  wlan svp assignment table.
 * Parameters:
 *      unit       - (IN) bcm device
 *      wlan_port  - (IN/OUT) WLAN Port Configuration Info
 *      vp         - (IN) Virtual port no.
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 *     (keys)    +----------+
 *    BSSID/RID -| WLAN SVP |- SVP
 *    TunnelID  -|Assignment|- EXP_TUNNEL_ID
 *               +----------+
 */
STATIC int
_bcm_tr3_wlan_match_add(int unit, bcm_wlan_port_t *wlan_port, int vp)
{
    int rv = BCM_E_NONE;
    axp_wrx_svp_assignment_entry_t wlan_svp;
    uint32 tunnel;
    uint64 temp;

    sal_memset(&wlan_svp, 0, sizeof(axp_wrx_svp_assignment_entry_t));
    soc_AXP_WRX_SVP_ASSIGNMENTm_field32_set(unit, &wlan_svp, VALIDf, 1);
    soc_AXP_WRX_SVP_ASSIGNMENTm_field32_set(unit, &wlan_svp, SVPf, vp);

    if (wlan_port->flags & BCM_WLAN_PORT_BSSID) {
        /* Entry Type BSSID_KEY (0x2) */
        soc_AXP_WRX_SVP_ASSIGNMENTm_field32_set(unit, &wlan_svp, ENTRY_TYPEf, 0x2);
        soc_mem_mac_addr_set(unit, AXP_WRX_SVP_ASSIGNMENTm, &wlan_svp, BSSIDf,
                             wlan_port->bssid);
        tunnel = BCM_GPORT_TUNNEL_ID_GET(wlan_port->match_tunnel);
        soc_AXP_WRX_SVP_ASSIGNMENTm_field32_set(unit, &wlan_svp, EXP_TUNNEL_IDf,
                                        tunnel);
        WLAN_INFO(unit)->port_info[vp].flags |= _BCM_WLAN_PORT_MATCH_BSSID;
        SAL_MAC_ADDR_TO_UINT64(wlan_port->bssid, temp);
        SAL_MAC_ADDR_FROM_UINT64
           (WLAN_INFO(unit)->port_info[vp].match_bssid, temp);
        WLAN_INFO(unit)->port_info[vp].match_tunnel = wlan_port->match_tunnel;

    } else if (wlan_port->flags & BCM_WLAN_PORT_BSSID_RADIO) {
        /* Entry Key Type RID_BSSID_KEY (0x01) */
        soc_AXP_WRX_SVP_ASSIGNMENTm_field32_set(unit, &wlan_svp, ENTRY_TYPEf, 0x1);
        soc_mem_mac_addr_set(unit, AXP_WRX_SVP_ASSIGNMENTm, &wlan_svp, BSSIDf,
                             wlan_port->bssid);
        soc_AXP_WRX_SVP_ASSIGNMENTm_field32_set(unit, &wlan_svp, RIDf,
                                        wlan_port->radio);
        tunnel = BCM_GPORT_TUNNEL_ID_GET(wlan_port->match_tunnel);
        soc_AXP_WRX_SVP_ASSIGNMENTm_field32_set(unit, &wlan_svp, EXP_TUNNEL_IDf,
                                        tunnel);
        WLAN_INFO(unit)->port_info[vp].flags |= _BCM_WLAN_PORT_MATCH_BSSID_RADIO;
        SAL_MAC_ADDR_TO_UINT64(wlan_port->bssid, temp);
        SAL_MAC_ADDR_FROM_UINT64
           (WLAN_INFO(unit)->port_info[vp].match_bssid, temp);
        WLAN_INFO(unit)->port_info[vp].match_radio = wlan_port->radio;
        WLAN_INFO(unit)->port_info[vp].match_tunnel = wlan_port->match_tunnel;

    } else if (wlan_port->flags & BCM_WLAN_PORT_MATCH_TUNNEL) {
        /* Entry type TUNNEL_ID_KEY (0x3) */
        soc_AXP_WRX_SVP_ASSIGNMENTm_field32_set(unit, &wlan_svp, ENTRY_TYPEf, 0x3);
        tunnel = BCM_GPORT_TUNNEL_ID_GET(wlan_port->match_tunnel);
        soc_AXP_WRX_SVP_ASSIGNMENTm_field32_set(unit, &wlan_svp, TUNNEL_IDf,
                                        tunnel);
        WLAN_INFO(unit)->port_info[vp].flags |= _BCM_WLAN_PORT_MATCH_TUNNEL;
        WLAN_INFO(unit)->port_info[vp].match_tunnel = wlan_port->match_tunnel;
    }
    rv = soc_mem_insert(unit, AXP_WRX_SVP_ASSIGNMENTm, MEM_BLOCK_ALL, &wlan_svp);
    return rv;
}



/*
 * Function:
 *      bcm_wlan_port_add
 * Purpose:
 *      Create or replace a WLAN port
 * Parameters:
 *      unit        - (IN)  Device Number
 *      wlan_port   - (IN/OUT) WLAN port configuration info
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      For AC-to-AC VPs, the match_tunnel and egress_tunnel must have the same
 *      ID. That way we can use a single VP for both directions. This constrains
 *      the application to use the lower half of the tunnel initiators. Below is
 *      the wlan port linkage to tables.
 *
 *      wlan_port(gport)
 *          |
 *          |                                                                    
 *          +---[VP]-[SVP]-->[SOURCE_VP]---->{LPORT_TABLE}                       
 *              |     |                                                         
 *              |     +---->[ING_DVP_TABLE]                                     
 *              |         (nhi)|                                                
 *              |              +->{ING_L3_NEXT_HOP}                             
 *              |              |                                                
 *              |              +->{INITIAL_ING_L3_NEXT_HOP}                     
 *              |              |                                                
 *              |              +->{EGR_L3_NEXT_HOP}->(EGR_L3_INTF) 
 *              |                                                               
 *              |                                                               
 *              +---[DVP]-->[EGR_DVP_ATTRIBUTE]                                 
 *                             |                                                
 *                             +--{AXP_WTX_DVP_PROFILE}->(AXP_WTX_TUNNEL)
 */
int
bcm_tr3_wlan_port_add(int unit, bcm_wlan_port_t *wlan_port)
{
    int drop, mode, is_local = 0, rv = BCM_E_PARAM, nh_index = 0;
    int vp, num_vp, lport_ptr = -1;
    int tpid_enable = 0, tpid_index;
    uint32 wlan_dvp_idx;
    bcm_port_t                      local_port;     /* Local port             */
    bcm_module_t                    my_modid;       /* Switch module id       */
    source_vp_attributes_2_entry_t  svp;            /* SVP Attributes         */
    ing_dvp_table_entry_t           dvp;            /* Ingress DVP Attributes */
    egr_dvp_attribute_entry_t       wlan_dvp;       /* Egress wlan dvp        */
    axp_wtx_dvp_profile_entry_t     axp_wtx_dvp;    /* AXP DVP Profile        */
    axp_wtx_tunnel_entry_t          axp_wtx_tunnel[_BCM_WLAN_TNL_ENTRY_WIDTH]; 
                                                    /* AXP WTX Tunnel table   */
    lport_tab_entry_t               lport_profile;  /* VP Lport table         */
    rtag7_port_based_hash_entry_t   rtag7_entry;    /* Part of Lport table    */
    uint32                          tunnel;         /* match tunnelid         */
    void                            *entries[2];    /* For lport profile mem  */
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;
    _bcm_vp_info_t vp_info;

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeWlan;

    WLAN_INIT(unit);

    rv = bcm_xgs3_l3_egress_mode_get(unit, &mode);
    BCM_IF_ERROR_RETURN(rv);
    if (!mode) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "L3 egress mode must be set first\n")));
        return BCM_E_DISABLED;
    }

    if (!BCM_GPORT_IS_TUNNEL(wlan_port->match_tunnel)) {
        return BCM_E_BADID;
    } else {
        tunnel = BCM_GPORT_TUNNEL_ID_GET(wlan_port->match_tunnel);
        if (tunnel > (1 << soc_mem_field_length(unit, AXP_WRX_SVP_ASSIGNMENTm,
                                               EXP_TUNNEL_IDf)) - 1) {
            return BCM_E_PARAM;
        }
    }

    WLAN_LOCK(unit);
    /* Allocate a new VP or use provided one */
    if (wlan_port->flags & BCM_WLAN_PORT_WITH_ID) {
        if (!BCM_GPORT_IS_WLAN_PORT((wlan_port->wlan_port_id))) {
            WLAN_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port->wlan_port_id);
        if (vp >= soc_mem_index_count(unit, SOURCE_VP_ATTRIBUTES_2m)) {
            WLAN_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
            if (!(wlan_port->flags & BCM_WLAN_PORT_REPLACE)) {
                WLAN_UNLOCK(unit);
                return BCM_E_EXISTS;
            }
        } else {
            rv = _bcm_vp_used_set(unit, vp, vp_info);
            if (rv < 0) {
                WLAN_UNLOCK(unit);
                return rv;
            }
        }
    } else {
        /* allocate a new VP index */
        num_vp = soc_mem_index_count(unit, SOURCE_VP_ATTRIBUTES_2m);
        rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VP_ATTRIBUTES_2m,
                           vp_info, &vp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            return rv;
        }
    }

    /* Program, the next hop, matching and egress tables */
    sal_memset(&lport_profile, 0, sizeof(lport_tab_entry_t));
    sal_memset(&rtag7_entry, 0, sizeof(rtag7_port_based_hash_entry_t));

    if (wlan_port->flags & BCM_WLAN_PORT_REPLACE) {
        /* For existing ports, NH entry already exists */
        rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            return rv;
        }
        nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                                  NEXT_HOP_INDEXf);
        /* Read the existing SVP entry for potential modifications */
        rv = READ_SOURCE_VP_ATTRIBUTES_2m(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            return rv;
        }
    } else {
        sal_memset(&svp, 0, sizeof(source_vp_attributes_2_entry_t));
        sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
        sal_memset(&wlan_dvp, 0, sizeof(egr_dvp_attribute_entry_t));
        /* Allocate a default profile entry for SVPs */

        /* Add 802.1Q outer TPID entry */
        rv = _bcm_fb2_outer_tpid_entry_add(unit, 0x8100, &tpid_index);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        }
        tpid_enable = (1 << tpid_index);

        /* Set to Default VID (0x1) */
        soc_LPORT_TABm_field32_set(unit, &lport_profile, PORT_VIDf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile,
                                   TRUST_INCOMING_VIDf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile,
                                   OUTER_TPID_ENABLEf, tpid_enable);
        soc_LPORT_TABm_field32_set(unit, &lport_profile,
                                   MAC_BASED_VID_ENABLEf, 0x1);
        if (!(wlan_port->flags & BCM_WLAN_PORT_NETWORK)) {
            soc_LPORT_TABm_field32_set(unit, &lport_profile,
                                       PORT_BRIDGEf, 0x1);
        }

        /* Q-in-Q( subnet based inner-outer vids) enable */
        soc_LPORT_TABm_field32_set(unit, &lport_profile,
                                   SUBNET_BASED_VID_ENABLEf, 0x1);
        /* Set Hardware Learning */

        /* Set the CML */
        rv = _bcm_vp_default_cml_mode_get (unit, 
                           &cml_default_enable, &cml_default_new, 
                           &cml_default_move);
         if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
         }

        if (cml_default_enable) {
            /* Set the CML to default values */
           soc_LPORT_TABm_field32_set(unit, &lport_profile, CML_FLAGS_NEWf, cml_default_new);
           soc_LPORT_TABm_field32_set(unit, &lport_profile, CML_FLAGS_MOVEf, cml_default_move);
        } else {
            /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
            soc_LPORT_TABm_field32_set(unit, &lport_profile, CML_FLAGS_NEWf, 0x8);
            soc_LPORT_TABm_field32_set(unit, &lport_profile, CML_FLAGS_MOVEf, 0x8);
        }

        soc_LPORT_TABm_field32_set(unit, &lport_profile, PRI_MAPPINGf,
                                   0xfac688);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, CFI_0_MAPPINGf, 0);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, CFI_1_MAPPINGf, 1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, V4L3_ENABLEf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, V6L3_ENABLEf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, V4IPMC_ENABLEf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, V6IPMC_ENABLEf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, IPMC_DO_VLANf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, FILTER_ENABLEf, 0x1);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, VFP_ENABLEf, 0x1);
        /* Allocate a fixed high index for the PORT_FIELD_SEL */
        soc_LPORT_TABm_field32_set(unit, &lport_profile,
                                   FP_PORT_FIELD_SEL_INDEXf,
                                   soc_mem_index_max(unit, FP_PORT_FIELD_SELm));
        /* The vt_key_types must be set to (MACSA)0x3 or (IPv4SIP)0x7 for WLAN */
        soc_LPORT_TABm_field32_set(unit, &lport_profile, VT_KEY_TYPEf,
                                   TR3_PT_XLT_KEY_TYPE_VLAN_MAC);
        soc_LPORT_TABm_field32_set(unit, &lport_profile, VT_KEY_TYPE_2f,
                                   TR3_PT_XLT_KEY_TYPE_VLAN_MAC);
        /* Program MY_MODID for the LPORT entry */
        rv = bcm_esw_stk_my_modid_get(unit, &my_modid);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        }
        soc_LPORT_TABm_field32_set(unit, &lport_profile, MY_MODIDf, my_modid);
        entries[0] = &lport_profile;
        entries[1] = &rtag7_entry;
        rv = _bcm_lport_profile_entry_add(unit, entries, 1,
                                          (uint32 *) &lport_ptr);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        }
        soc_SOURCE_VP_ATTRIBUTES_2m_field32_set(unit, &svp, LPORT_PROFILE_INDEXf,
                                        lport_ptr);
    }

    /* Program next hop entry using existing or new index */
    drop = (wlan_port->flags & BCM_WLAN_PORT_DROP) ? 1 : 0;
    rv = _bcm_tr3_wlan_nh_info_add(unit, wlan_port, vp, drop,
                                   &nh_index, &local_port, &is_local);
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        goto port_cleanup;
    }

    /* Program the VP tables */
    if (wlan_port->flags & BCM_WLAN_PORT_NETWORK) {
        /* Enable SVP & DVP network port to set split horizon pruning */
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 1);
        soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &wlan_dvp,
                                           WLAN__DISABLE_VP_PRUNINGf, 0);
    } else {
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 0);
        soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &wlan_dvp,
                                           WLAN__DISABLE_VP_PRUNINGf, 1);
    }

    /* Select HG2 PPD2 header for remote egress */
    soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &wlan_dvp, WLAN__HG_HDR_SELf, 1);

    /* Program the matching criteria */
    if (wlan_port->flags & BCM_WLAN_PORT_REPLACE) {
        rv = WRITE_SOURCE_VP_ATTRIBUTES_2m(unit, MEM_BLOCK_ALL, vp, &svp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        }
    } else {
        /* Link in the newly allocated next-hop entry */
        soc_ING_DVP_TABLEm_field32_set(unit, &dvp, NEXT_HOP_INDEXf,
                                       nh_index);
        rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        }

        if (!BCM_GPORT_IS_TUNNEL(wlan_port->egress_tunnel)) {
            WLAN_UNLOCK(unit);
            rv = BCM_E_BADID;
            goto port_cleanup;
        }
        tunnel = BCM_GPORT_TUNNEL_ID_GET(wlan_port->egress_tunnel);
        if ((wlan_port->flags & BCM_WLAN_PORT_BSSID) ||
            (wlan_port->flags & BCM_WLAN_PORT_BSSID_RADIO)) {
            /* Set BSSID mac address in wlan dvp profile */
            soc_mem_mac_addr_set(unit, AXP_WTX_DVP_PROFILEm, &axp_wtx_dvp, BSSIDf,
                                 wlan_port->bssid);
            rv = _bcm_tr3_wlan_tnl_read(unit, tunnel, axp_wtx_tunnel);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

           /* Enable Radio mac header CAPWAP encap */
            if ((wlan_port->flags & BCM_WLAN_PORT_EGRESS_BSSID)) {
                soc_AXP_WTX_TUNNELm_field32_set(unit, &axp_wtx_tunnel[TNL_ENTRY_0],
                                              BSSID_ENf, 1);
            }

            /* Write to tunnel table */
            rv = _bcm_tr3_wlan_tnl_write(unit, tunnel, axp_wtx_tunnel);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

        }

        /* 
         * Enable HA/FA for WLAN DVP port. HA/FA bit determines FROM_HA/FROM_FA 
         * encodings in capwap headers for AC2AC tunnel
         */
        if ((wlan_port->flags & BCM_WLAN_PORT_ROAM_ENABLE) && 
            (WLAN_TUNNEL_TYPE_AC2AC == soc_AXP_WTX_TUNNELm_field32_get(unit, 
                                 &axp_wtx_tunnel[TNL_ENTRY_0], TUNNEL_TYPEf))) {
            soc_AXP_WTX_DVP_PROFILEm_field32_set(unit, &axp_wtx_dvp, 
                                                    HA_FA_ENABLEf, 1);
        }

        /* Program the tunnel VLAN from the cached state */
        soc_AXP_WTX_DVP_PROFILEm_field32_set(unit, &axp_wtx_dvp, TUNNEL_VLANf,
                                      TUNNEL_VLAN(unit, tunnel));
        soc_AXP_WTX_DVP_PROFILEm_field32_set(unit, &axp_wtx_dvp, TUNNEL_PRIf,
                                      TUNNEL_PRI(unit, tunnel));
        soc_AXP_WTX_DVP_PROFILEm_field32_set(unit, &axp_wtx_dvp, TUNNEL_CFIf,
                                      TUNNEL_CFI(unit, tunnel));
       
        /* set wlan tunnel index */
        soc_AXP_WTX_DVP_PROFILEm_field32_set(unit, &axp_wtx_dvp, 
                                    WLAN_TUNNEL_INDEXf, tunnel);
        /*
         * Allocate new WLAN DVP profile index and link to EGR DVP
         * [EGR_DVP_ATTRIBUTE.WLAN_DVP_PROFILE] ->  [AXP_WTX_DVP_PROFILE]
         */

        /* Allocate new wlan DVP index */
        entries[0] = &axp_wtx_dvp;
        rv = soc_profile_mem_add(unit, WLAN_DVP_PROFILE(unit), entries, 1,
                            &wlan_dvp_idx);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        }
        soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &wlan_dvp, WLAN_DVP_PROFILEf,
                                           wlan_dvp_idx );
        /* Set WLAN VP Type */
        soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &wlan_dvp, VP_TYPEf,
                                           0x2 );

        /* Write into EGR_EVP_ATTRIBUTEm */
        rv = WRITE_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ALL, vp, &wlan_dvp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        }

        /* Initialize the SVP parameters */
        soc_SOURCE_VP_ATTRIBUTES_2m_field32_set(unit, &svp, DEFAULT_VLAN_IDf, 0x1);
        rv = WRITE_SOURCE_VP_ATTRIBUTES_2m(unit, MEM_BLOCK_ALL, vp, &svp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        }

        /*
         * Match entries cannot be replaced, instead, callers
         * need to delete the existing entry and re-add with the
         * new match parameters.
         */
        rv = _bcm_tr3_wlan_match_add(unit, wlan_port, vp);
        if (rv < 0) {
            WLAN_UNLOCK(unit);
            goto port_cleanup;
        } else {
            /* Set the WLAN port ID */
            BCM_GPORT_WLAN_PORT_ID_SET(wlan_port->wlan_port_id, vp);
            /* Set IPMC next hop interface id */
            wlan_port->encap_id = nh_index;
        }
    }
    WLAN_UNLOCK(unit);
port_cleanup:
    if (rv < 0) {
        if (tpid_enable) {
            (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
        }
        if (!(wlan_port->flags & BCM_WLAN_PORT_REPLACE)) {
            (void) _bcm_vp_free(unit, _bcmVpTypeWlan, 1, vp);
            _bcm_tr3_wlan_nh_info_delete(unit, nh_index);
        }
        if (lport_ptr != -1) {
            (void) _bcm_lport_profile_entry_delete(unit, lport_ptr);
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_wlan_port_delete
 * Purpose: 
 *      This function deletes wlan port
 * Parameters: 
 *      unit       - (IN) bcm device
 *      vp         - (IN) virtual port
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_tr3_wlan_port_delete(int unit, int vp)
{
    int rv = BCM_E_NONE;
    int nh_index, wlan_dvp_index;
    int lport_ptr = -1;
    int is_local, i, tpid_enable = 0;
    source_vp_attributes_2_entry_t svp;
    ing_dvp_table_entry_t dvp;
    egr_dvp_attribute_entry_t wlan_dvp;
    lport_tab_entry_t lport_profile;
    rtag7_port_based_hash_entry_t rtag7_entry;
    _bcm_port_info_t *info;
    void *entries[2];

    rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
    BCM_IF_ERROR_RETURN(rv);
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);

    rv = _bcm_tr3_wlan_match_delete(unit, vp);
    BCM_IF_ERROR_RETURN(rv);

    /* Delete LPORT profile and its associated TPID entry */
    rv = READ_SOURCE_VP_ATTRIBUTES_2m(unit, MEM_BLOCK_ANY, vp, &svp);
    BCM_IF_ERROR_RETURN(rv);
    lport_ptr = soc_SOURCE_VP_ATTRIBUTES_2m_field32_get(unit, &svp,
                                                        LPORT_PROFILE_INDEXf);
    entries[0] = &lport_profile;
    entries[1] = &rtag7_entry;
    rv = _bcm_lport_profile_entry_get(unit, lport_ptr, 1, entries);
    BCM_IF_ERROR_RETURN(rv);
    tpid_enable = soc_LPORT_TABm_field32_get(unit, &lport_profile,
                                             OUTER_TPID_ENABLEf);
    if (tpid_enable) {
        for (i = 0; i < 4; i++) {
            if (tpid_enable & (1 << i)) {
                (void) _bcm_fb2_outer_tpid_entry_delete(unit, i);
                break;
            }
        }
    }
    rv = _bcm_lport_profile_entry_delete(unit, lport_ptr);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear the SVP and DVP table entries */
    sal_memset(&svp, 0, sizeof(source_vp_attributes_2_entry_t));
    rv = WRITE_SOURCE_VP_ATTRIBUTES_2m(unit, MEM_BLOCK_ALL, vp, &svp);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
    rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);
    BCM_IF_ERROR_RETURN(rv);

    /* Delete wlan dvp profile */ 
    BCM_IF_ERROR_RETURN(
            READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, &wlan_dvp));
    wlan_dvp_index = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &wlan_dvp, WLAN_DVP_PROFILEf);
    BCM_IF_ERROR_RETURN(
            soc_profile_mem_delete(unit, WLAN_DVP_PROFILE(unit), wlan_dvp_index)); 
    
    sal_memset(&wlan_dvp, 0, sizeof(egr_dvp_attribute_entry_t));
    rv = WRITE_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ALL, vp, &wlan_dvp);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear the next-hop table entries */
    rv = _bcm_tr3_wlan_nh_info_delete(unit, nh_index);
    BCM_IF_ERROR_RETURN(rv);

    /* Update the physical port's SW state */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_modid_is_local(unit, WLAN_INFO(unit)->port_info[vp].modid,
                                &is_local));
    if (is_local && (WLAN_INFO(unit)->port_info[vp].tgid == -1)) {
        bcm_port_t phys_port = WLAN_INFO(unit)->port_info[vp].port;
        if (soc_feature(unit, soc_feature_sysport_remap)) {
            BCM_XLATE_SYSPORT_S2P(unit, &phys_port);
        }
        _bcm_port_info_access(unit, phys_port, &info);
        info->vp_count--;
    }

    /* If associated with a trunk, update each local physical port's SW state */
    if (WLAN_INFO(unit)->port_info[vp].tgid != -1) {
        int idx;
        bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
       int local_member_count;
        bcm_trunk_t trunk_id = WLAN_INFO(unit)->port_info[vp].tgid;

        BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, trunk_id,
                    SOC_MAX_NUM_PORTS, local_member_array, &local_member_count));
        for (idx = 0; idx < local_member_count; idx++) {
            _bcm_port_info_access(unit, local_member_array[idx], &info);
            info->vp_count--;
        }
    }

    /* Clear the SW state */
    sal_memset(&(WLAN_INFO(unit)->port_info[vp]), 0,
               sizeof(_bcm_tr3_wlan_port_info_t));

    /* Release Service counter, if any */
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        bcm_gport_t gport;

        BCM_GPORT_WLAN_PORT_ID_SET(gport, vp);
        _bcm_esw_flex_stat_handle_free(unit, _bcmFlexStatTypeGport, gport);
    }

    /* Free the VP */
    (void) _bcm_vp_free(unit, _bcmVpTypeWlan, 1, vp);
    return rv;
}

/*
 * Function:
 *      bcm_wlan_port_delete
 * Purpose:
 *      Delete a wlan port
 * Parameters:
 *      unit       - (IN) Device Number
 *      wlan_port_id - (IN) wlan port information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_wlan_port_delete(int unit, bcm_gport_t wlan_port_id)
{
    int vp, rv;

    WLAN_INIT(unit);

    vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
        return BCM_E_NOT_FOUND;
    }
    WLAN_LOCK(unit);
    rv = _bcm_tr3_wlan_port_delete(unit, vp);
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_wlan_port_delete_all
 * Purpose:
 *      Delete all wlan ports
 * Parameters:
 *      unit       - (IN) Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_wlan_port_delete_all(int unit)
{
    int rv = BCM_E_NONE;
    uint32 vp, num_vp;
    source_vp_entry_t svp, *svp_null;

    WLAN_INIT(unit);

    svp_null = soc_mem_entry_null(unit, SOURCE_VP_ATTRIBUTES_2m);
    num_vp = soc_mem_index_count(unit, SOURCE_VP_ATTRIBUTES_2m);
    sal_memset(&svp,0,sizeof(svp));
    for (vp = 0; vp < num_vp; vp++) {
        rv = READ_SOURCE_VP_ATTRIBUTES_2m(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            goto done;
        }
        if (sal_memcmp(&svp, svp_null, sizeof(source_vp_entry_t)) != 0) {
            WLAN_LOCK(unit);
            rv = _bcm_tr3_wlan_port_delete(unit, vp);
            WLAN_UNLOCK(unit);
            if (rv < 0) {
                goto done;
            }
        }
    }
done:
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_wlan_port_get
 * Purpose: 
 *      This function gets wlan port info.
 * Parameters: 
 *      unit       - (IN) bcm device
 *      vp         - (IN) wlan virtual port
 *      wlan_port  - (OUT) wlan port info
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_tr3_wlan_port_get(int unit, int vp, bcm_wlan_port_t *wlan_port)
{
    int egress_tunnel, nh_index, rv = BCM_E_NONE;
    int wlan_dvp_index;
    ing_dvp_table_entry_t           dvp;            /* ING DVP entry        */
    egr_dvp_attribute_entry_t       wlan_dvp;       /* EGR DVP entry        */
    axp_wtx_dvp_profile_entry_t     axp_wtx_dvp;    /* AXP DVP Profile      */
    axp_wtx_tunnel_entry_t          axp_wtx_tunnel[_BCM_WLAN_TNL_ENTRY_WIDTH]; 
                                                    /* AXP WTX Tunnel table   */
    void   *mem_entries[1];                         /* For profile mem entries */

    /* Initialize the structure */
    bcm_wlan_port_t_init(wlan_port);
    BCM_GPORT_WLAN_PORT_ID_SET(wlan_port->wlan_port_id, vp);

    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);

    /* Fill the IPMC next hop interface id(encap_id) */
    wlan_port->encap_id = nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

    /* Get the match parameters */
    rv = _bcm_tr3_wlan_match_get(unit, wlan_port, vp);
    BCM_IF_ERROR_RETURN(rv);

    /* Get the next-hop parameters */
    rv = _bcm_tr3_wlan_nh_info_get(unit, wlan_port, nh_index);
    BCM_IF_ERROR_RETURN(rv);

    /* Fill in egress parameters */
    BCM_IF_ERROR_RETURN(
            READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, &wlan_dvp));
    wlan_dvp_index = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &wlan_dvp, WLAN_DVP_PROFILEf);
    mem_entries[0] = &axp_wtx_dvp;
    rv = soc_profile_mem_get(unit, WLAN_DVP_PROFILE(unit), wlan_dvp_index, 1,
                            mem_entries);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (soc_AXP_WTX_DVP_PROFILEm_field32_get(unit, &axp_wtx_dvp, HA_FA_ENABLEf)) {
        wlan_port->flags |= BCM_WLAN_PORT_ROAM_ENABLE;
    }

    egress_tunnel = soc_AXP_WTX_DVP_PROFILEm_field32_get(unit, &axp_wtx_dvp,
                                                        WLAN_TUNNEL_INDEXf);
 
    BCM_GPORT_TUNNEL_ID_SET(wlan_port->egress_tunnel, egress_tunnel);
    rv = _bcm_tr3_wlan_tnl_read(unit, egress_tunnel, axp_wtx_tunnel);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    if (!soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &wlan_dvp, 
                                           WLAN__DISABLE_VP_PRUNINGf)) {
        wlan_port->flags |= BCM_WLAN_PORT_NETWORK;
    }

    if (soc_AXP_WTX_TUNNELm_field32_get(unit, &axp_wtx_tunnel[TNL_ENTRY_0], BSSID_ENf)) {
        wlan_port->flags |= BCM_WLAN_PORT_EGRESS_BSSID;
    }

    return rv;
}

/*
 * Function:
 *      bcm_wlan_port_get
 * Purpose:
 *      Get a wlan port
 * Parameters:
 *      unit       - (IN) Device Number
 *      wlan_port  - (IN/OUT) wlan port information
 */
int
bcm_tr3_wlan_port_get(int unit, bcm_gport_t wlan_port_id, bcm_wlan_port_t *wlan_port)
{
    int vp;

    WLAN_INIT(unit);

    vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
        return BCM_E_NOT_FOUND;
    }
    return _bcm_tr3_wlan_port_get(unit, vp, wlan_port);
}

/*
 * Function:
 *      bcm_wlan_port_traverse
 * Purpose:
 *      Walks through the valid WLAN port entries and calls
 *      the user supplied callback function for each entry.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      trav_fn    - (IN) Callback function.
 *      user_data  - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_tr3_wlan_port_traverse(int unit,
                           bcm_wlan_port_traverse_cb cb,
                           void *user_data)
{
    int i, index_min, index_max, rv = BCM_E_NONE;
    int wlan_port_id, *buffer = NULL;
    bcm_wlan_port_t info;

    WLAN_INIT(unit);

    index_min = soc_mem_index_min(unit, SOURCE_VP_ATTRIBUTES_2m);
    index_max = soc_mem_index_max(unit, SOURCE_VP_ATTRIBUTES_2m);

    WLAN_LOCK(unit);
    buffer = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, SOURCE_VP_ATTRIBUTES_2m),
                           "wlan port traverse");
    if (NULL == buffer) {
        WLAN_UNLOCK(unit);
        return BCM_E_MEMORY;
    }
    if ((rv = soc_mem_read_range(unit, SOURCE_VP_ATTRIBUTES_2m, MEM_BLOCK_ALL,
                                 index_min, index_max, buffer)) < 0 ) {
        soc_cm_sfree(unit, buffer);
        WLAN_UNLOCK(unit);
        return rv;
    }
    for (i = index_min; i <= index_max; i++) {
        bcm_wlan_port_t_init(&info);
        BCM_GPORT_WLAN_PORT_ID_SET(wlan_port_id, i);
        rv = bcm_tr3_wlan_port_get(unit, wlan_port_id, &info);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, buffer);
            WLAN_UNLOCK(unit);
            return rv;
        }
        rv = cb(unit, &info, user_data);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, buffer);
            WLAN_UNLOCK(unit);
            return rv;
        }
    }
    soc_cm_sfree(unit, buffer);
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_wlan_tunnel_init_add
 * Purpose:
 *      Add tunnel initiator entry to hw.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      info     - (IN)Tunnel initiator entry info.
 *      *idx      -(OUT)new tunnel Index in wtx tunnel table
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      WLAN Tunnel (AXP_WTX_TUNNEL) has 1k tunnels * 4 entries/tunnel
 *      
 *      tnl_id : tunnel id returns the value in 1k tunnels
 *      tnl_idex: tunnel index programmed in DVP profile table (4 * tnl_id)     
 *
 *      ENTRY0: TPID_SEL, UDP_TYPE .. TUNNEL_TYPE, BSSID_EN, IP_TYPE,DIP_LOW, SIP_LOW
 *      ENTRY1: L4_DST(111-96)     MACSA(95-48) MACDA(47-0)
 *      ENTRY2: L4_SRC(111-96)     DIP_HI(95-0)
 *      ENTRY3: FLOW_LABEL(115-96) SIP_HI(95-0)

 */
STATIC int
_bcm_tr3_wlan_tunnel_init_add(int unit, bcm_tunnel_initiator_t *info, int *tnl_id_p)
{
    soc_mem_t               mem;            /* Tunnel initiator table memory */
    axp_wtx_tunnel_entry_t  tnl_entry[_BCM_WLAN_TNL_ENTRY_WIDTH];   
                                            /* AXP_WTX_TUNNEL 4 hw entries   */
    int df_val;                             /* Don't fragment encoding       */
    int ipv6;                               /* IPv6 tunnel initiator         */
    int hw_type_code = 0;                   /* Tunnel type.                  */
    int old_tpid_index = -1;                /* Old TPID index                */
    uint32 tpid_index = 0;                  /* TPID index                    */
    int tnl_index = -1;                     /* Wlan tunnel Index             */
    int rv = BCM_E_NONE;                    /* Return value                  */
    uint32 ip6_fields[4];                   /* IPV6 addr fields              */
    uint64 rval64, *rval64s[1];             /* For the profile register      */

    /* Get IP Type */
    ipv6 = _BCM_TUNNEL_OUTER_HEADER_IPV6(info->type);

    mem = AXP_WTX_TUNNELm;

    /* Zero write buffer. */
    sal_memset(&tnl_entry, 0, sizeof(tnl_entry));

    /* Get table memory. */
    if (ipv6) {
        /* IP_TYPE: IPV6 (0x01) */
        soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], IP_TYPEf, WLAN_TUNNEL_TYPE_V6);
    } else {
        soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], IP_TYPEf, WLAN_TUNNEL_TYPE_V4);
    }

    /* If replacing existing entry, first read the entry to get old
       profile pointer and TPID */
    if (info->flags & BCM_TUNNEL_REPLACE) {
        tnl_index = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
        rv = _bcm_tr3_wlan_tnl_read(unit, tnl_index, tnl_entry);
        BCM_IF_ERROR_RETURN(rv);
        old_tpid_index = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_0],
                                             TPID_SELf);
        if (info->flags & BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED) {
            soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], 
                                VLAN_ASSIGN_POLICYf, 1);
        } else {
            soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0],
                                VLAN_ASSIGN_POLICYf, 0);
        }
    }

    /* Set Source and Destination address. */
    if (ipv6) {
        SAL_IP6_ADDR_TO_UINT32(info->dip6, ip6_fields);
        soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], DIP_LOWf, 
                            ip6_fields[0]);
        soc_mem_field_set(unit, mem, (uint32 *)&tnl_entry[TNL_ENTRY_2], DIP_HIf, 
                            (uint32 *)&ip6_fields[1]);
        SAL_IP6_ADDR_TO_UINT32(info->sip6, ip6_fields);
        soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], SIP_LOWf, 
                            ip6_fields[0]);
        soc_mem_field_set(unit, mem, (uint32 *)&tnl_entry[TNL_ENTRY_3], SIP_HIf, 
                            (uint32 *)&ip6_fields[1]);
    } else { /* ipv4 */
        soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], DIP_LOWf, info->dip);
        soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], SIP_LOWf, info->sip);
    }

    /* IP tunnel hdr don't fragment (DNF) bit */
    df_val = 0;
    if (info->flags & BCM_TUNNEL_INIT_USE_INNER_DF) {
        /* (0x02)Use INNER ipv4 DNF bit */
        df_val |= 0x2;
    } else if ((info->flags & BCM_TUNNEL_INIT_IPV4_SET_DF)
               || (info->flags & BCM_TUNNEL_INIT_IPV6_SET_DF)) {
        /* (0x01) Set DNF bit */
        df_val |= 0x1;
    }
    soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], DF_SELf, df_val);

    /* Set DSCP value.  */
    soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], DSCP_SRCf, info->dscp);

    /* Tunnel header DSCP select. */
    
    soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], DSCP_SELf, info->dscp_sel);

    /* Set TTL. */
    soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], TTL_HLIMITf, info->ttl);

    /* Set tunnel type. */
    if ( info->type == bcmTunnelTypeWlanWtpToAc) {
        hw_type_code =  WLAN_TUNNEL_TYPE_WTP2AC;
    } else if (info->type == bcmTunnelTypeWlanAcToAc) {
        hw_type_code =  WLAN_TUNNEL_TYPE_AC2AC;
    }
    soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], TUNNEL_TYPEf, 
                        hw_type_code);

    /* Set flow label. */
    if (ipv6) {
        soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_3], FLOW_LABELf,
                            info->flow_label);
    }


    /* L4 fields */
    soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_2], L4_SRCf,
                        info->udp_src_port);
    soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_1], L4_DSTf,
                        info->udp_dst_port);

    /* L2 fields */

    /* Set destination  and source mac address. */
    soc_mem_mac_addr_set(unit, mem, &tnl_entry[TNL_ENTRY_1], MACDAf, info->dmac);
    soc_mem_mac_addr_set(unit, mem, &tnl_entry[TNL_ENTRY_1], MACSAf, info->smac);

    if (info->flags & BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED) {
        /* VLAN Assign policy is single tagged */
        soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], 
                            VLAN_ASSIGN_POLICYf, 1);
        /* Use DVP Profile TUNNEL VLAN/PRI/CFI for vlan tag creation */
        soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0],        
                            DOT1P_REMAPf, 0);

        /* Add  and set tpid index to AXP_WTX_TUNNEL_TPID tpid values */
        COMPILER_64_ZERO(rval64);
        COMPILER_64_SET(rval64, 0, info->tpid);
        rval64s[0] = &rval64;
        rv = soc_profile_reg_add(unit, WLAN_TPID_PROFILE(unit), rval64s, 1,
                                 &tpid_index);
        soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], TPID_SELf, tpid_index);
        if (BCM_FAILURE(rv)) {
            goto tnl_cleanup;
        }

    }


    /* CAPWAP MTU */
    if (info->flags & BCM_TUNNEL_INIT_WLAN_MTU && info->mtu > 0) {
        uint32 encap_len = GET_CAPWAP_ENCAP_LEN(info->type);
        
        /* capwap payload length in 8 byte chunks = (mtu - max capwap encap len) */
        uint32 first = (((info->mtu - encap_len) / 8) > 240) ? 240 :
                       ((info->mtu - encap_len) / 8);
        /* Fragment MTU in 4 byte chunks */
        soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], FRAG_MTUf,
                            ((info->mtu) / 4 > 0xFFF) ? 0xFFF: (info->mtu / 4));
        soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], FRAG_LOCf, first);
        /* Fragmentation is enabled by default */
        soc_mem_field32_set(unit, mem, &tnl_entry[TNL_ENTRY_0], FRAG_ENf, 1);
    }

    if (info->flags & BCM_TUNNEL_INIT_WLAN_TUNNEL_WITH_ID) {
        /* Set Tunnel id */
        _BCM_WLAN_TNL_USED_SET(unit, *tnl_id_p);
    } else {
        /* Get Free tnl_entry  */
        rv = _bcm_tr3_wlan_tnl_create(unit,  tnl_id_p);
        if (BCM_FAILURE(rv)) {
            goto tnl_cleanup;
        }
    }

    /* Write TUNNEL entries in entry0, entry1, entry2, entry3 */
    rv = _bcm_tr3_wlan_tnl_write(unit, *tnl_id_p, tnl_entry);
    if (BCM_FAILURE(rv)) {
        goto tnl_cleanup;
    }

    /* Tunnel VLAN needs to be programmed in the WLAN_DVP_PROFILE table.
     * We need to cache it so that the bcm_wlan_port_add API can
     * program it in the correct location. It is always cached against
     * the index to WTX_TUNNEL for both V4 and V6 for simplicity. */
    TUNNEL_VLAN(unit, *tnl_id_p) = info->vlan;
    TUNNEL_PRI(unit, *tnl_id_p) = info->pkt_pri;
    TUNNEL_CFI(unit, *tnl_id_p) = info->pkt_cfi;

tnl_cleanup:
    if (old_tpid_index != -1) {
        BCM_IF_ERROR_RETURN (
            soc_profile_reg_delete(
                unit, WLAN_TPID_PROFILE(unit), old_tpid_index));
    }
    if (BCM_FAILURE(rv) && (!(info->flags & BCM_TUNNEL_REPLACE))) {
        if (tnl_id_p && (*tnl_id_p != -1)) {
            _BCM_WLAN_TNL_USED_CLR(unit, *tnl_id_p);
        }
    }
    return rv;
}

/*
 * Function:
 *      bcm_wlan_tunnel_initiator_create
 * Purpose:
 *      Create an outgong CAPWAP tunnel
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN/OUT) Tunnel initiator structure
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_tr3_wlan_tunnel_initiator_create(int unit, bcm_tunnel_initiator_t *info)
{
    int tnl_idx = 0, rv = BCM_E_NONE;
    uint32 flags = 0;

    WLAN_INIT(unit);

    /* Input parameters check. */
    if (NULL == info) {
        return (BCM_E_PARAM);
    }
    if ((info->type != bcmTunnelTypeWlanWtpToAc) &&
        (info->type != bcmTunnelTypeWlanAcToAc) &&
        (info->type != bcmTunnelTypeWlanWtpToAc6) &&
        (info->type != bcmTunnelTypeWlanAcToAc6)) {
        return BCM_E_PARAM;
    }
    if (!BCM_TTL_VALID(info->ttl)) {
        return BCM_E_PARAM;
    }
    if (info->dscp_sel > 2 || info->dscp_sel < 0) {
        return BCM_E_PARAM;
    }
    if (info->dscp > 63 || info->dscp < 0) {
        return BCM_E_PARAM;
    }
    if (_BCM_TUNNEL_OUTER_HEADER_IPV6(info->type)) {
        if (info->flow_label > (1 << 20)) {
            return BCM_E_PARAM;
        }
    }
    if (!BCM_VLAN_VALID(info->vlan)) {
        return BCM_E_PARAM;
    }
    if (BCM_MAC_IS_MCAST(info->smac) || BCM_MAC_IS_ZERO(info->smac)) {
        return BCM_E_PARAM;
    }
    if (info->flags & BCM_TUNNEL_INIT_WLAN_TUNNEL_WITH_ID) {
        if (!BCM_GPORT_IS_TUNNEL(info->tunnel_id)) {
            return BCM_E_PARAM;
        }
        flags |= _BCM_L3_SHR_WITH_ID;
        tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
        if (info->flags & BCM_TUNNEL_REPLACE) {
            if (!_BCM_WLAN_TNL_USED_GET(unit, tnl_idx)) {
                return BCM_E_PARAM;
            }
            flags |= _BCM_L3_SHR_UPDATE;
        }
    }
    if (info->flags & BCM_TUNNEL_INIT_WLAN_MTU) {
        if (info->mtu < 0) {
            return BCM_E_PARAM;
        }
    }

    WLAN_LOCK(unit);
    /* Allocate either existing or new tunnel initiator entry */
    rv = _bcm_tr3_wlan_tunnel_init_add(unit, info, &tnl_idx);
    if (BCM_FAILURE(rv)) {
        WLAN_UNLOCK(unit);
        return rv;
    }

    if (!(info->flags & BCM_TUNNEL_INIT_WLAN_TUNNEL_WITH_ID)) {
        BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, tnl_idx);
    }

    /* Update usage bitmaps appropriately */
    /* tnl_idx is always the index to WTX_TUNNEL */
    /* If writing to IPV6 Type Tunnel, need to divide by 2 */
    _BCM_WLAN_TNL_USED_SET(unit, tnl_idx);
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_wlan_tunnel_init_get
 * Purpose:
 *      Get a tunnel initiator entry from hw.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      idx      - (IN)Index to read tunnel initiator.
 *      info     - (OUT)Tunnel initiator entry info.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_wlan_tunnel_init_get(int unit, int idx, bcm_tunnel_initiator_t *info,
                              int *pri_index, int *tpid_index)
{
    soc_mem_t               mem;           /* Tunnel initiator table memory.*/
    axp_wtx_tunnel_entry_t     tnl_entry[_BCM_WLAN_TNL_ENTRY_WIDTH];  
                                           /* AXP_WTX_TUNNEL hw entry       */
    int df_val;                            /* Don't fragment encoded value. */
    int tnl_type;                          /* Tunnel type.                  */
    uint32 entry_type = WLAN_TUNNEL_TYPE_V4;/* Entry type (IPv4/IPv6/MPLS)  */
    uint32 rval;                           /* Generic register value        */
    uint32 ip6_fields[4];                  /* IPV6 addr fields              */
    uint64 rval64, *rval64s[1];            /* For the profile register      */

    /* Get table memory. */
    mem = AXP_WTX_TUNNELm;

    /* Initialize the buffer. */
    sal_memset(&tnl_entry, 0, sizeof(tnl_entry));

    /* Get tunnel VLAN from cache */
    if (TUNNEL_VLAN(unit, idx) != 0) {
        info->vlan = TUNNEL_VLAN(unit, idx);
    }

    /* First read the entry to get the type */
    BCM_IF_ERROR_RETURN(_bcm_tr3_wlan_tnl_read(unit, idx, tnl_entry));

    /* Get entry_type. */
    entry_type = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_0], IP_TYPEf);

    /* VLAN tag added or not */
    if (soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_0], 
                            VLAN_ASSIGN_POLICYf)) {
        info->flags |= BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED;
    }

    /* Get profiled data */
    *pri_index  = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_0],
                                      DOT1P_MAP_PTRf);
    *tpid_index = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_0], TPID_SELf);

    /* Parse hw buffer. */
    if (WLAN_TUNNEL_TYPE_V4 == entry_type) {
        /* Get destination ip. */
        info->dip = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_0],
                                        DIP_LOWf);

        /* Get source ip. */
        info->sip = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_0],
                                        SIP_LOWf);
    } else if (WLAN_TUNNEL_TYPE_V6 == entry_type) {
        /* Get destination ip. */
        soc_mem_field_get(unit, mem, (uint32 *)&tnl_entry[TNL_ENTRY_0], DIP_LOWf,
                          (uint32 *)&ip6_fields[0]);
        soc_mem_field_get(unit, mem, (uint32 *)&tnl_entry[TNL_ENTRY_2], DIP_HIf,
                          (uint32 *)&ip6_fields[1]);
        SAL_IP6_ADDR_FROM_UINT32(info->dip6, ip6_fields);

        /* Get source ip. */
        soc_mem_field_get(unit, mem, (uint32 *)&tnl_entry[TNL_ENTRY_0], SIP_LOWf,
                          (uint32 *)&ip6_fields[0]);
        soc_mem_field_get(unit, mem, (uint32 *)&tnl_entry[TNL_ENTRY_1], SIP_HIf,
                          (uint32 *)&ip6_fields[1]);
        SAL_IP6_ADDR_FROM_UINT32(info->sip6, ip6_fields);
    }

    /* Tunnel header DSCP select. */
    info->dscp_sel = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_0],
                                         DSCP_SELf);

    /* Tunnel header DSCP value. */
    info->dscp = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_0], DSCP_SRCf);

    /* IP tunnel hdr DF bit for IPv4 */
    df_val = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_0], DF_SELf);
    if (0x2 == df_val) {
        info->flags |= BCM_TUNNEL_INIT_USE_INNER_DF;
    } else if (0x1 == df_val) {
        if (WLAN_TUNNEL_TYPE_V4 == entry_type) {
            info->flags |= BCM_TUNNEL_INIT_IPV4_SET_DF;
        } else if (WLAN_TUNNEL_TYPE_V6 == entry_type) {
            info->flags |= BCM_TUNNEL_INIT_IPV6_SET_DF;
        }
    }

    /* Get TTL. */
    info->ttl = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_0],
                                    TTL_HLIMITf);

    /* Get tunnel type. */
    tnl_type = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_0], TUNNEL_TYPEf);

    /* Translate hw tunnel type into API encoding. */
    if (tnl_type == WLAN_TUNNEL_TYPE_WTP2AC) {
        info->type = bcmTunnelTypeWlanWtpToAc;
    } else if (tnl_type ==  WLAN_TUNNEL_TYPE_AC2AC) {
        info->type = bcmTunnelTypeWlanAcToAc;
    }

    /* Get flow label for IPv6 */
    if (WLAN_TUNNEL_TYPE_V6 == entry_type) {
        info->flow_label =
            soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_3], FLOW_LABELf);
    }

    /* Get L4 fields */
    info->udp_dst_port = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_1],
                                                 L4_DSTf);
    info->udp_src_port = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_2],
                                                 L4_SRCf);

    /* Get mac addresses */
    soc_mem_mac_addr_get(unit, mem, &tnl_entry[TNL_ENTRY_1], MACDAf, info->dmac);
    soc_mem_mac_addr_get(unit, mem, &tnl_entry[TNL_ENTRY_1], MACSAf, info->smac);

    /* Get other L2 fields */
    if (info->flags & BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED) {
        info->pkt_pri = TUNNEL_PRI(unit, idx);
        info->pkt_cfi = TUNNEL_CFI(unit, idx);

        *tpid_index = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_0],
                                          TPID_SELf);
        COMPILER_64_ZERO(rval64);
        rval64s[0] = &rval64;
        BCM_IF_ERROR_RETURN(soc_profile_reg_get(unit, WLAN_TPID_PROFILE(unit),
                                                *tpid_index, 1, rval64s));
        info->tpid = COMPILER_64_LO(rval64);
    }

    /* Get the MTU */
    rval = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_0], FRAG_MTUf);
    info->mtu = rval == 0xFFF ? 0x3FFF :  (rval * 4); /* 4 byte chunks */
    if (info->mtu > 0) {
        info->flags |= BCM_TUNNEL_INIT_WLAN_MTU;
        rval = soc_mem_field32_get(unit, mem, &tnl_entry[TNL_ENTRY_0], 
                                   FRAG_ENf);
        if(rval) {
            info->flags |= BCM_TUNNEL_INIT_WLAN_FRAG_ENABLE;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_wlan_tunnel_initiator_destroy
 * Purpose:
 *      Destroy an outgong CAPWAP tunnel
 * Parameters:
 *      unit           - (IN) Device Number
 *      wlan_tunnel_id - (IN) Tunnel ID (Gport)
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_wlan_tunnel_initiator_destroy(int unit, bcm_gport_t wlan_tunnel_id)
{
    int rv = BCM_E_NONE;
    uint32 tpid_index;                     /* TPID index */
    int pri_index;                         /* priority index */
    int tnl_index;                         /* Tunnel index */
    bcm_tunnel_initiator_t info;           /* Tunnel initiator structure */

    WLAN_INIT(unit);

    /* Input parameters check. */
    if (!BCM_GPORT_IS_TUNNEL(wlan_tunnel_id)) {
        return BCM_E_PARAM;
    }
    tnl_index = BCM_GPORT_TUNNEL_ID_GET(wlan_tunnel_id);
    if (!_BCM_WLAN_TNL_USED_GET(unit, tnl_index)) {
        return BCM_E_PARAM;
    }

    bcm_tunnel_initiator_t_init(&info);

    WLAN_LOCK(unit);
    /* Get the entry first */
    rv = _bcm_tr3_wlan_tunnel_init_get(unit, tnl_index, &info,
                                       &pri_index, (int*)&tpid_index);
    if (BCM_FAILURE(rv)) {
        WLAN_UNLOCK(unit);
        return rv;
    }

    if (info.flags & BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED) {
        /* Destroy the profiles */
        rv = soc_profile_reg_delete(unit, WLAN_TPID_PROFILE(unit), tpid_index);
        if (BCM_FAILURE(rv)) {
            WLAN_UNLOCK(unit);
            return rv;
        }
    }

    /* Destroy the tunnel entry */
    rv = _bcm_tr3_wlan_tnl_delete(unit, tnl_index);
    if (BCM_FAILURE(rv)) {
        WLAN_UNLOCK(unit);
        return rv;
    }

    /* Update usage bitmaps appropriately */
    /* tnl_index is always the index to WTX Tunnel */
    /* If writing to IPV6 type, need to divide by 2 */
    _BCM_WLAN_TNL_USED_CLR(unit, tnl_index);
    TUNNEL_VLAN(unit, tnl_index) = 0;
    TUNNEL_PRI(unit, tnl_index) = 0;
    TUNNEL_CFI(unit, tnl_index) = 0;
    WLAN_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_wlan_tunnel_initiator_get
 * Purpose:
 *      Get an outgong CAPWAP tunnel
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN/OUT) Tunnel initiator structure
 */
int
bcm_tr3_wlan_tunnel_initiator_get(int unit, bcm_tunnel_initiator_t *info)
{
    int tnl_idx, rv = BCM_E_NONE;
    int pri_index;                         /* priority index */
    int tpid_index;                        /* TPID index */

    WLAN_INIT(unit);

    /* Input parameters check. */
    if (info == NULL) {
        return BCM_E_PARAM;
    }
    if (!BCM_GPORT_IS_TUNNEL(info->tunnel_id)) {
        return BCM_E_PARAM;
    }
    tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
    if (!_BCM_WLAN_TNL_USED_GET(unit, tnl_idx)) {
        return BCM_E_NOT_FOUND;
    }

    /* Get the entry */
    rv = _bcm_tr3_wlan_tunnel_init_get(unit, tnl_idx, info,
                                       &pri_index, (int*)&tpid_index);
    return rv;
}

/*
 * Function:
 *      bcm_tr3_wlan_tunnel_terminator_vlan_get
 * Purpose:
 *      Get the allowed VLANs for a WLAN tunnel (AC2AC only)
 * Parameters:
 *      unit    - (IN)SOC unit number.
 *      tunnel  - (IN)Tunnel ID.
 *      vlan_vec - (OUT)VLAN vector
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *   1. Get assigned svp for the matched tunnelid
 *   2. Get SVP vlan membership group and VP Group bitmap
 *   3. Get all allowed vlans for the vp group bitmap
 */
int
bcm_tr3_wlan_tunnel_terminator_vlan_get(int unit, bcm_gport_t tunnel,
                                   bcm_vlan_vector_t *vlan_vec)
{
    int i, tnl_id, rv = BCM_E_NONE;
    vlan_tab_entry_t *vtab;
    uint32 grp_vec[2];
    uint64 grp_bmp;
    uint32 *buf;
    int num_vp = 0, vp = -1, vvp = -1;
    source_vp_entry_t svp;

    WLAN_INIT(unit);

    /* Input parameters check. */
    if (!BCM_GPORT_IS_TUNNEL(tunnel)) {
        return BCM_E_PARAM;
    }
    tnl_id = BCM_GPORT_TUNNEL_ID_GET(tunnel);
    if (!_BCM_WLAN_TNL_USED_GET(unit, tnl_id)) {
        return BCM_E_PARAM;
    }
    if (vlan_vec == NULL) {
        return BCM_E_PARAM;
    }

    /* Find the VP that corresponds to this tunnel */
    
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    for (vp = 0; vp < num_vp; vp++) {
        if ((WLAN_INFO(unit)->port_info[vp].match_tunnel == tunnel) &&
            (WLAN_INFO(unit)->port_info[vp].flags &
             _BCM_WLAN_PORT_MATCH_TUNNEL)) {
            /* Found the matching VP  - get the VLAN_MEMBERSHIP_PROFILE */
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, SOURCE_VPm, MEM_BLOCK_ANY, vp, &svp));
            /* Get VP Vlan group membership profile (mapped to 64 vp groups) */
            vvp = soc_SOURCE_VPm_field32_get(unit, &svp,
                                                  VLAN_MEMBERSHIP_PROFILEf);
            break;
        }
    }
    if ((vp == num_vp) || (vvp < 0)) {
        return BCM_E_NOT_FOUND;
    }

    /* Compare vlan membership against the VP_GROUP_BITMAP in vlan table */
    buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, VLAN_TABLE(unit)),
                        "vlan_table");
    if (buf == NULL) {
        return BCM_E_MEMORY;
    }
    
    if ((rv = soc_mem_read_range(unit, VLAN_TABLE(unit), MEM_BLOCK_ANY,
                                 BCM_VLAN_MIN, BCM_VLAN_MAX, buf)) < 0) {
        soc_cm_sfree(unit, buf);
        return rv;
    }

    /* Get all allowed vlans for the VP_GROUP_BITMAP */
    for (i = BCM_VLAN_MIN; i < BCM_VLAN_MAX; i++) {
        vtab = soc_mem_table_idx_to_pointer(unit, VLAN_TABLE(unit), vlan_tab_entry_t *,
                                            buf, i);
        if (!soc_mem_field32_get(unit, VLAN_TABLE(unit), vtab, VALIDf)) {
            continue;                                     
        }
        grp_vec[0] = grp_vec[1] = 0;
        soc_mem_field_get(unit, VLAN_TABLE(unit), (uint32*)vtab,
                          VP_GROUP_BITMAPf, grp_vec);
        
        COMPILER_64_ZERO(grp_bmp);
        COMPILER_64_SET(grp_bmp, grp_vec[1], grp_vec[0]);

        if (COMPILER_64_BITTEST(grp_bmp, vvp)) {
            /* VLAN is allowed to come in on this AC 2 AC tunnel */
            BCM_VLAN_VEC_SET((*vlan_vec), i);
        }
    }

    soc_cm_sfree(unit, buf);
    return rv;
}


/*
 * Function:
 *      bcm_tr3_wlan_tunnel_terminator_vlan_set
 * Purpose:
 *      Set the allowed VLANs for a WLAN tunnel (AC2AC only)
 * Parameters:
 *      unit    -   (IN)SOC unit number.
 *      tunnel  -   (IN)Tunnel ID.
 *      vlan_vec -  (IN)VLAN vector
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *
 *   [Tunnel Id]  +----------+SVP             (map to 64 vp groups)
 *       |        |AXP_SVP_  |--->+-----------+ vlan_membership-----+
 *     [SVP]      |ASSIGNMENT|    | SOURCE_VP |     +-----------+    \
 *       |        +----------+    |           |     |VLAN_TAB   |vp_bitmap
 * [Allowed Vlans]                +-----------+     |           |
 *                                                  +-----------+
 */

int
bcm_tr3_wlan_tunnel_terminator_vlan_set(int unit, bcm_gport_t tunnel,
                                   bcm_vlan_vector_t vlan_vec)
{
    int i, tnl_idx, rv = BCM_E_NONE;
    uint32 count = 0, diff = 0;
    uint64 grp_bmp, grp_bmp_union, val64;
    uint32 grp_vec[2];
    uint32 *buf;
    vlan_tab_entry_t *vtab;
    bcm_vlan_vector_t vlan_vec_old;
    int num_vp = 0, vp = -1, vvp = -1;
    source_vp_entry_t svp;

    WLAN_INIT(unit);

    /* Input parameters check. */
    if (!BCM_GPORT_IS_TUNNEL(tunnel)) {
        return BCM_E_PARAM;
    }
    tnl_idx = BCM_GPORT_TUNNEL_ID_GET(tunnel);
    if (!_BCM_WLAN_TNL_USED_GET(unit, tnl_idx)) {
        return BCM_E_PARAM;
    }
    if (BCM_VLAN_VEC_GET(vlan_vec, BCM_VLAN_MAX) ||
        BCM_VLAN_VEC_GET(vlan_vec, BCM_VLAN_MIN)) {
        return BCM_E_PARAM;
    }

    WLAN_LOCK(unit);
    
    /* First get existing VLAN vector, all allowed vlans for the tunnel */
    BCM_VLAN_VEC_ZERO(vlan_vec_old);
    rv = bcm_tr3_wlan_tunnel_terminator_vlan_get(unit, tunnel, &vlan_vec_old);
    if (BCM_FAILURE(rv)) {
        WLAN_UNLOCK(unit);
        return rv;
    }

    /* Check if old == new and if old vector is empty */
    for (i = BCM_VLAN_MIN; i < BCM_VLAN_MAX; i++) {
        if (BCM_VLAN_VEC_GET(vlan_vec_old, i) !=
            BCM_VLAN_VEC_GET(vlan_vec, i)) {
            diff = 1;
            if (BCM_VLAN_VEC_GET(vlan_vec_old, i)) {
                count++;
                break;
            }
        }
    }
    if (!diff) {
        WLAN_UNLOCK(unit);
        return rv;
    }

    /* Find the SVP corresponding to this tunnel */
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    for (vp = 0; vp < num_vp; vp++) {
        if ((WLAN_INFO(unit)->port_info[vp].match_tunnel == tunnel) &&
            (WLAN_INFO(unit)->port_info[vp].flags &
             _BCM_WLAN_PORT_MATCH_TUNNEL)) {
            /* Found the matching VP  -  get the VLAN_MEMBERSHIP_PROFILE */
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, SOURCE_VPm, MEM_BLOCK_ANY, vp, &svp));
            vvp = soc_SOURCE_VPm_field32_get(unit, &svp,
                                                  VLAN_MEMBERSHIP_PROFILEf);
            break;
        }
    }
    if ((vp == num_vp) || (vvp < 0)) {
        WLAN_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    /* DMA the VLAN table */
    buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, VLAN_TABLE(unit)),
                        "vlan_table");
    if (buf == NULL) {
        WLAN_UNLOCK(unit);
        return BCM_E_MEMORY;
    }
    soc_mem_lock(unit, VLAN_TABLE(unit));
    if ((rv = soc_mem_read_range(unit, VLAN_TABLE(unit), MEM_BLOCK_ANY,
                                 BCM_VLAN_MIN, BCM_VLAN_MAX, buf)) < 0) {
        goto vlan_set_end;
    }

    /* Initialize values */
    COMPILER_64_ZERO(grp_bmp);
    COMPILER_64_ZERO(grp_bmp_union);

    if (count == 0) {
        /* Old vector is empty, allocate a new bit in VLAN_GROUP_BITMAP */
        /* Otherwise use the existing VLAN_MEMBERSHIP_PROFILE from the SVP */
        for (i = BCM_VLAN_MIN; i < BCM_VLAN_MAX; i++) {
            vtab = soc_mem_table_idx_to_pointer(unit, VLAN_TABLE(unit),
                                                vlan_tab_entry_t *, buf, i);
            if (!soc_mem_field32_get(unit, VLAN_TABLE(unit), vtab, VALIDf)) {
                if (BCM_VLAN_VEC_GET(vlan_vec, i)) {
                    rv = BCM_E_NOT_FOUND;
                    goto vlan_set_end;
                } else {
                    continue;
                }
            }
            grp_vec[0] = grp_vec[1] = 0;
            soc_mem_field_get(unit, VLAN_TABLE(unit), (uint32*)vtab, VP_GROUP_BITMAPf, grp_vec);
            COMPILER_64_ZERO(grp_bmp);
            COMPILER_64_ZERO(grp_bmp_union);
            COMPILER_64_SET(grp_bmp, grp_vec[1], grp_vec[0]);

            COMPILER_64_OR(grp_bmp_union, grp_bmp);
        }

        COMPILER_64_SET(val64, 0xFFF, 0xFFFFF);

        if (COMPILER_64_EQ(grp_bmp_union, val64)) {
            rv = BCM_E_RESOURCE;
            goto vlan_set_end;
        }
       
        /* Use the first available zero bit - skip vvp == 0 */
        for (i = 1; i < soc_mem_field_length(unit, VLAN_TABLE(unit),
                                             VP_GROUP_BITMAPf); i++) {
            if (COMPILER_64_BITTEST(grp_bmp_union, i)) {
                vvp = i;
                break;
            }
        }
    }

    /* Write the VP_GROUP_BITMAP for all applicable VLANs */
    for (i = BCM_VLAN_MIN; i < BCM_VLAN_MAX; i++) {
        vtab = soc_mem_table_idx_to_pointer(unit, VLAN_TABLE(unit), vlan_tab_entry_t *,
                                            buf, i);
        grp_vec[0] = grp_vec[1] = 0;
        soc_mem_field_get(unit, VLAN_TABLE(unit), (uint32*)vtab, VP_GROUP_BITMAPf, grp_vec);
        COMPILER_64_ZERO(grp_bmp);
        COMPILER_64_SET(grp_bmp, grp_vec[1], grp_vec[0]);

        /* Prepare one bit (1 << vvp) in uint64 */
        COMPILER_64_SET(val64, 0, 1);
        COMPILER_64_SHL(val64, vvp);

        if (BCM_VLAN_VEC_GET(vlan_vec, i)) {

            COMPILER_64_OR(grp_bmp, val64);
            /* Increase use-count for this bit for this VLAN */
            WLAN_INFO(unit)->vlan_grp_bmp_use_cnt[i][vvp]++;
        } else {
            /* Decrease use-count for this bit for this VLAN */
            if (BCM_VLAN_VEC_GET(vlan_vec_old, i)) {
                WLAN_INFO(unit)->vlan_grp_bmp_use_cnt[i][vvp]--;
            }
            if (WLAN_INFO(unit)->vlan_grp_bmp_use_cnt[i][vvp] == 0) {
                COMPILER_64_NOT(val64); /* Invert one bit to mask */
                COMPILER_64_AND(grp_bmp, val64);
            }
        }
        
        grp_vec[0] = grp_vec[1] = 0;
        COMPILER_64_TO_32_LO(grp_vec[0], grp_bmp);
        COMPILER_64_TO_32_HI(grp_vec[1], grp_bmp);
        soc_mem_field_set(unit, VLAN_TABLE(unit), (uint32 *)vtab, VP_GROUP_BITMAPf, grp_vec);
    }
    if ((rv = soc_mem_write_range(unit, VLAN_TABLE(unit), MEM_BLOCK_ALL,
                                  BCM_VLAN_MIN, BCM_VLAN_MAX, buf)) < 0) {
        goto vlan_set_end;
    }
    soc_SOURCE_VPm_field32_set(unit, &svp, VLAN_MEMBERSHIP_PROFILEf, vvp);
    /* Enable Vlan-Vp Membership check */
    soc_SOURCE_VPm_field32_set(unit, &svp, ENABLE_IFILTERf, 1);
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
vlan_set_end:
    soc_mem_unlock(unit, VLAN_TABLE(unit));
    WLAN_UNLOCK(unit);
    soc_cm_sfree(unit, buf);
    return rv;
}


/*
 * Helper functions for setting/getting a field in the LPORT table and
 * update corresponding reference in the WLAN_SVP_TABLE.
 * Assumes locks are taken outside.
 */
int
bcm_tr3_wlan_lport_field_set(int unit, bcm_gport_t wlan_port_id,
                             soc_field_t field, int value)
{
    int value_old = 0, rv = BCM_E_NONE;
    uint32 vp;
    source_vp_attributes_2_entry_t svp;
    lport_tab_entry_t lport_profile;
    rtag7_port_based_hash_entry_t rtag7_entry;
    int lport_ptr, old_lport_ptr = -1;
    void *entries[2];

    /* Check valid LPORT field */
    if (!SOC_MEM_FIELD_VALID(unit, LPORT_TABm, field)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the VP index from the gport */
    vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port_id);

    /* Be sure the entry is used and is set for WLAN */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
        return BCM_E_BADID;
    }
    BCM_IF_ERROR_RETURN(READ_SOURCE_VP_ATTRIBUTES_2m(unit, MEM_BLOCK_ANY, vp, &svp));

    /* Get old lport_profile */
    old_lport_ptr = soc_SOURCE_VP_ATTRIBUTES_2m_field32_get(unit, &svp, 
                                                    LPORT_PROFILE_INDEXf);
    entries[0] = &lport_profile;
    entries[1] = &rtag7_entry;
    BCM_IF_ERROR_RETURN
        (_bcm_lport_profile_entry_get(unit, old_lport_ptr, 1, entries));
    value_old = soc_LPORT_TABm_field32_get(unit, &lport_profile, field);
    if (value != value_old) {
        soc_LPORT_TABm_field32_set(unit, &lport_profile, field, value);
        BCM_IF_ERROR_RETURN(_bcm_lport_profile_entry_add(unit, entries, 1, 
                                                        (uint32 *) &lport_ptr));
        soc_SOURCE_VP_ATTRIBUTES_2m_field32_set(unit, &svp, LPORT_PROFILE_INDEXf, 
                                        lport_ptr);
        /* coverity[negative_returns : FALSE] */
        BCM_IF_ERROR_RETURN
            (WRITE_SOURCE_VP_ATTRIBUTES_2m(unit, MEM_BLOCK_ALL, vp, &svp));
        BCM_IF_ERROR_RETURN
            (_bcm_lport_profile_entry_delete(unit, old_lport_ptr));
    }
    return rv;
}

int
bcm_tr3_wlan_lport_field_get(int unit, bcm_gport_t wlan_port_id, 
                             soc_field_t field, int *value)
{
    int rv = BCM_E_NONE;
    uint32 vp;
    source_vp_attributes_2_entry_t svp;
    lport_tab_entry_t lport_profile;
    rtag7_port_based_hash_entry_t rtag7_entry;
    int lport_ptr;
    void *entries[2];

    /* Check valid LPORT field */
    if (!SOC_MEM_FIELD_VALID(unit, LPORT_TABm, field)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the VP index from the gport */
    vp = BCM_GPORT_WLAN_PORT_ID_GET(wlan_port_id);

    /* Be sure the entry is used and is set for WLAN */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
        return BCM_E_BADID;
    }
    BCM_IF_ERROR_RETURN(READ_SOURCE_VP_ATTRIBUTES_2m(unit, MEM_BLOCK_ANY, vp, &svp));

    /* Get old lport_profile */
    lport_ptr = soc_SOURCE_VP_ATTRIBUTES_2m_field32_get(unit, &svp,
                                                        LPORT_PROFILE_INDEXf);
    entries[0] = &lport_profile;
    entries[1] = &rtag7_entry;
    BCM_IF_ERROR_RETURN
        (_bcm_lport_profile_entry_get(unit, lport_ptr, 1, entries));
    *value = soc_LPORT_TABm_field32_get(unit, &lport_profile, field);
    return rv;
}


int
_bcm_tr3_wlan_port_set(int unit, bcm_gport_t wlan_port_id, soc_field_t field,
                       uint32 value)
{
    int rv = BCM_E_NONE;
    WLAN_LOCK(unit);

    rv = bcm_tr3_wlan_lport_field_set(unit, wlan_port_id, field, value);

    WLAN_UNLOCK(unit);
    return rv;
}

int
bcm_tr3_wlan_port_untagged_vlan_set(int unit, bcm_gport_t port, bcm_vlan_t vid)
{
    int old_profile = 0, profile, rv = BCM_E_NONE;
    bcm_vlan_action_set_t action;
    WLAN_LOCK(unit);

    /* Get current default tag action entry */
    rv = bcm_tr3_wlan_lport_field_get(unit, port, TAG_ACTION_PROFILE_PTRf, 
                                      &old_profile);
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        return rv;
    }
    bcm_vlan_action_set_t_init(&action);
    _bcm_trx_vlan_action_profile_entry_get(unit, &action, old_profile);

    /* Add new tag action entry */
    action.new_outer_vlan = vid;
    rv = bcm_tr3_wlan_lport_field_get(unit, port, PORT_PRIf,
                                      &(action.priority));
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        return rv;
    }

    rv = _bcm_trx_vlan_action_profile_entry_add(unit, &action, (uint32*) &profile);
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        return rv;
    }

    /* Set the port VID value */
    rv = bcm_tr3_wlan_lport_field_set(unit, port, PORT_VIDf, vid);
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        return rv;
    }
    /* Point to the new profile */
    rv = bcm_tr3_wlan_lport_field_set(unit, port,
                                      TAG_ACTION_PROFILE_PTRf, profile);
    if (rv < 0) {
        WLAN_UNLOCK(unit);
        return rv;
    }
    /* Delete the old profile */
    rv = _bcm_trx_vlan_action_profile_entry_delete(unit, old_profile);
    WLAN_UNLOCK(unit);
    return rv;
}
/*
 * Function:
 *      bcm_tr3_wlan_port_untagged_vlan_get
 * Purpose:
 *      This function gets port vid for untagged tag action profile
 * Parameters:
 *      unit       - (IN) bcm device
 *      port       - (IN) gport
 *      vid_ptr    - (OUT) port vid
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_tr3_wlan_port_untagged_vlan_get(int unit, bcm_gport_t port,
                                    bcm_vlan_t *vid_ptr)
{
    int value = 0, rv = BCM_E_NONE;

    rv = bcm_tr3_wlan_lport_field_get(unit, port, PORT_VIDf, &value);
    *vid_ptr = (bcm_vlan_t)value;

    return rv;
}

/*
 * Function:
 *      bcm_tr3_wlan_port_untagged_prio_set
 * Purpose:
 *      This function sets port default priority in lport_table
 * Parameters:
 *      unit       - (IN) bcm device
 *      port       - (IN) gport
 *      prio       - (IN) Port priority
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_tr3_wlan_port_untagged_prio_set(int unit, bcm_gport_t port, int prio)
{
    int rv = BCM_E_NONE;
    if (prio > 7) {
        return BCM_E_PARAM;
    }
    rv = _bcm_tr3_wlan_port_set(unit, port, PORT_PRIf, prio);
    return rv;
}

/*
 * Function:
 *      bcm_tr3_wlan_port_untagged_prio_get
 * Purpose:
 *      This function gets port default priority from lport_table
 * Parameters:
 *      unit       - (IN) bcm device
 *      port       - (IN) gport
 *      *prio_ptr  - (OUT) Port priority
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_tr3_wlan_port_untagged_prio_get(int unit, bcm_gport_t port, int *prio_ptr)
{
    int value = 0, rv = BCM_E_NONE;

    rv = bcm_tr3_wlan_lport_field_get(unit, port, PORT_PRIf, &value);
    *prio_ptr = value;

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_qos_wlan_port_map_set
 * Purpose: 
 *      Function sets the egress priority maps in AXP for wlan port
 *
 * Parameters: 
 *      unit       - (IN) bcm device
 *      port       - (IN) wlan port
 *      egr_map    - (IN) egr map id
 *      pri_index  - (IN) egr priority index
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_tr3_qos_wlan_port_map_set(int unit, bcm_gport_t port, int egr_map, 
                                 int pri_index)
{
    axp_wtx_pri_map_entry_t         *wlan_pri_map;  /* Wlan Priority Map     */
    egr_dvp_attribute_entry_t       wlan_dvp;       /* Egress wlan dvp        */
    axp_wtx_dvp_profile_entry_t     axp_wtx_dvp;    /* AXP DVP Profile        */
    axp_wtx_tunnel_entry_t          axp_wtx_tunnel[_BCM_WLAN_TNL_ENTRY_WIDTH]; 
    void   *entry, *mem_entries[1];                 /* For profile mem entries       */
    int wlan_dvp_index, egress_tunnel;
    uint32 vp;
    int alloc_size, index;
    int idx, new_pri, new_cfi;
    soc_mem_t             mem;
    int rv = BCM_E_NONE;
    
 
    /* Deal with WLAN ports */
    vp = BCM_GPORT_WLAN_PORT_ID_GET(port);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
        return BCM_E_BADID;
    }

    if (egr_map != -1) { /* -1 means no change */
            
        /* Get AXP_WTX_TUNNEL index and set the dot1p pointer for WLAN AXP */
            
        /* Get EGR DVP Attribute for the vp */
        BCM_IF_ERROR_RETURN(
                READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, &wlan_dvp));
        wlan_dvp_index = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &wlan_dvp, WLAN_DVP_PROFILEf);
        mem_entries[0] = &axp_wtx_dvp;
        rv = soc_profile_mem_get(unit, WLAN_DVP_PROFILE(unit), wlan_dvp_index, 1,
                                mem_entries);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        /* Get Egress Tunnel Attributes */
        egress_tunnel = soc_AXP_WTX_DVP_PROFILEm_field32_get(unit, &axp_wtx_dvp,
                                                            WLAN_TUNNEL_INDEXf);

        rv = _bcm_tr3_wlan_tnl_read(unit, egress_tunnel, axp_wtx_tunnel);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        mem = AXP_WTX_TUNNELm;
        if (egr_map == 0) {
            /* Clear WTX Tunnel DOT1P priority map, 
             * the priorities are got from DVP tunnel pri/cfi
             */
            soc_mem_field32_set(unit, mem, &axp_wtx_tunnel[TNL_ENTRY_0],        
                                DOT1P_REMAPf, 0);
        } else {
            /* Use the provided map */
            soc_mem_field32_set(unit, mem, &axp_wtx_tunnel[TNL_ENTRY_0],        
                                DOT1P_REMAPf, 1);
            soc_mem_field32_set(unit, mem, &axp_wtx_tunnel[TNL_ENTRY_0],
                                DOT1P_MAP_PTRf, pri_index);

            /* Read from Egress "EGR_MPLS_PRI_MAPPING" and write to WLAN AXP
             * "AXP_WTX_PRI_MAP" table for the egr priority profile index. 
             */

            alloc_size = (_BCM_AXP_PRI_MAP_CHUNK * sizeof(egr_mpls_pri_mapping_entry_t));
            wlan_pri_map = soc_cm_salloc(unit, alloc_size,
                                                 "wlan pri map");
            if (NULL == wlan_pri_map) {
                return (BCM_E_MEMORY);
            }
            sal_memset(wlan_pri_map, 0, alloc_size);

            /* Priority Map index */
            index = pri_index * _BCM_AXP_PRI_MAP_CHUNK; 
                                   
            rv = soc_mem_read_range(unit, EGR_MPLS_PRI_MAPPINGm, MEM_BLOCK_ANY, 
                                        index, 
                                        (index + (_BCM_AXP_PRI_MAP_CHUNK -1)),
                                        wlan_pri_map);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, wlan_pri_map);
                return (rv);
            }

            for (idx = 0; idx < _BCM_AXP_PRI_MAP_CHUNK; idx++ )
            {
                new_pri = new_cfi = 0;
                mem = EGR_MPLS_PRI_MAPPINGm;
                entry = soc_mem_table_idx_to_pointer(unit, mem, void *, 
                                                 wlan_pri_map, idx);
                new_pri = soc_mem_field32_get(unit, mem, entry, NEW_PRIf);
                new_cfi = soc_mem_field32_get(unit, mem, entry, NEW_CFIf);

                mem = AXP_WTX_PRI_MAPm;
                entry = soc_mem_table_idx_to_pointer(unit, mem, void *, 
                                                 wlan_pri_map, idx);
                soc_mem_field32_set(unit, mem, entry, PRIf, new_pri);
                soc_mem_field32_set(unit, mem, entry, CFIf, new_cfi);
            }
       
             /* Write Priority maps to AXP_WTX_PRI_MAP */ 
             rv = soc_mem_write_range(unit, AXP_WTX_PRI_MAPm, MEM_BLOCK_ALL, 
                                        index, 
                                        (index + (_BCM_AXP_PRI_MAP_CHUNK -1)),
                                        wlan_pri_map);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, wlan_pri_map);
                return (rv);
            }
            soc_cm_sfree(unit, wlan_pri_map);
        }
        
        /* Write to axp_wtx_tunnel table */
        rv = _bcm_tr3_wlan_tnl_write(unit, egress_tunnel, axp_wtx_tunnel);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

    }
    return rv;
}

int
_bcm_tr3_wlan_tunnel_set_roam(int unit, bcm_gport_t tunnel_id)
{
    int rv = BCM_E_NONE;
    uint32 tunnel;
    egr_wlan_attributes_entry_t wlan_attr;

    WLAN_INIT(unit);

    /* Get Tunnel */
    tunnel = BCM_GPORT_TUNNEL_ID_GET(tunnel_id);

    WLAN_LOCK(unit);

    /* Enable HA/FA roam model */ 
    rv = READ_EGR_WLAN_ATTRIBUTESm(unit, MEM_BLOCK_ANY, tunnel, &wlan_attr);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    soc_EGR_WLAN_ATTRIBUTESm_field32_set(unit, &wlan_attr, HA_FA_ENABLEf, 1);
    rv = WRITE_EGR_WLAN_ATTRIBUTESm(unit, MEM_BLOCK_ANY, tunnel, &wlan_attr);

cleanup:
    WLAN_UNLOCK(unit);
    return rv;
}

int
_bcm_tr3_wlan_tunnel_get_roam(int unit, bcm_gport_t tunnel_id)
{
    int rv = BCM_E_NONE;
    uint32 tunnel;
    egr_wlan_attributes_entry_t wlan_attr;

    WLAN_INIT(unit);

    if (tunnel_id <= 0) {
        return BCM_E_PARAM;
    } 

    /* Get Tunnel */
    tunnel = BCM_GPORT_TUNNEL_ID_GET(tunnel_id);

    WLAN_LOCK(unit);

    /* Enable HA/FA roam model */ 
    rv = READ_EGR_WLAN_ATTRIBUTESm(unit, MEM_BLOCK_ANY, tunnel, &wlan_attr);
    if (BCM_FAILURE(rv))  {
        goto cleanup;
    }

    rv = soc_EGR_WLAN_ATTRIBUTESm_field32_get(unit, &wlan_attr, HA_FA_ENABLEf);

cleanup:
    
    WLAN_UNLOCK(unit);
    return rv;
}
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

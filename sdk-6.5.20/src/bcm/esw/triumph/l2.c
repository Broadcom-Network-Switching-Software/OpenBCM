/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l2.c
 * Purpose:     Triumph L2 function implementations
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#if defined(BCM_TRX_SUPPORT)

#include <assert.h>

#include <sal/core/libc.h>

#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/hash.h>
#include <soc/l2x.h>
#include <soc/triumph.h>
#ifdef BCM_TRIDENT2_SUPPORT
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
#include <soc/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_APACHE_SUPPORT)
#include <soc/apache.h>
#endif /* BCM_APACHE_SUPPORT */
#if defined(BCM_MONTEREY_SUPPORT)
#include <soc/monterey.h>
#endif /* BCM_MONTEREY_SUPPORT */

#include <bcm/l2.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/mim.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/switch.h>
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <bcm_int/esw/greyhound2.h>
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#endif /* BCM_SABER2_SUPPORT */
#if defined(BCM_METROLITE_SUPPORT)
#include <soc/metrolite.h>
#endif /* BCM_METROLITE_SUPPORT */
#if defined (BCM_HGPROXY_COE_SUPPORT)
#include <bcm_int/esw/subport.h>
#endif /* BCM_HGPROXY_COE_SUPPORT */
#if defined(BCM_TSN_SUPPORT)
#include <bcm_int/esw/tsn.h>
#endif /* BCM_TSN_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#define _BCM_PORT_MAC_MAX  170
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_GLOBAL_METER_SUPPORT)
#include <bcm_int/esw/policer.h>
#endif /* BCM_GLOBAL_METER_SUPPORT */

#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/esw/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT  */

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/subport.h>
#define _BCM_OLP_MAC_MAX   16

#ifdef BCM_TRIDENT3_SUPPORT
#include <soc/format.h>
#include <soc/esw/flow_db.h>
#include <soc/trident3.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/flow.h>
#endif /* BCM_TRIDENT3_SUPPORT */

#ifdef BCM_HELIX5_SUPPORT
#include <soc/helix5.h>
#endif /* BCM_HELIX5_SUPPORT  */

#ifdef BCM_HURRICANE4_SUPPORT
#include <soc/hurricane4.h>
#endif /* BCM_HURRICANE4_SUPPORT  */

#define _BCM_XGS_MAC_MAX   1

#define DGLP_MODULE_ID_MASK                   0x7f80
#define DGLP_PORT_NO_MASK                     0x7f
#define DGLP_MODULE_ID_SHIFT_BITS             7
#define DGLP_LAG_ID_INDICATOR_SHIFT_BITS      15
#define MAC_MASK_HIGH                         0xffff
#define MAC_MASK_LOW                          0xffffffff
#define OLP_MAC_MASK                          0x1f
#define OLP_LOWEST_MAC                        0
#define OLP_HIGHEST_MAC                       1
#define OLP_MAX_MAC_DIFF                      16
#define OLP_ACTION_ADD                        1
#define OLP_ACTION_UPDATE                     2
#define OLP_ACTION_DELETE                     3
#endif /* BCM_KATANA2_SUPPORT OR TD2+*/

#if defined(BCM_TRIUMPH_SUPPORT) /* BCM_TRIUMPH_SUPPORT */

#define GPORT_TYPE(_gport) (((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK)

#define _BCM_L2_STATION_OVERLAY(_station_) \
        ((soc_feature(unit, soc_feature_riot) && \
        !(_station_->flags & BCM_L2_STATION_UNDERLAY)))

#define _BCM_L2_STATION_ID_OVERLAY(_sid_) \
        ((soc_feature(unit, soc_feature_riot) && \
         (_sid_ >= _BCM_L2_STATION_ID_2_BASE)))

#define _BCM_L2_STATION_2(_station_) \
	(soc_feature(unit, soc_feature_pt2pt_access_vlan_check) && \
         (_station_->forward_domain_type == bcmL2ForwardDomainVlanClassId))

#define _BCM_L2_STATION_ID_2(_sid_) \
	(soc_feature(unit, soc_feature_pt2pt_access_vlan_check) && \
         (_sid_ >= _BCM_L2_STATION_ID_2_BASE))

/* To differentiate between overlay and underlay tcams */
#define _BCM_L2_STATION_ENTRY_UNDERLAY (0)
#define _BCM_L2_STATION_ENTRY_OVERLAY  (1)
#define _BCM_L2_STATION_ENTRY_LEGACY   _BCM_L2_STATION_ENTRY_UNDERLAY


#define _BCM_L2_STATION_ID_INVALID (-1)
#define _BCM_L2_STATION_ID_BASE (1)
#define _BCM_L2_STATION_ID_MAX  (0x1000000)
#define _BCM_L2_STATION_ID_2_BASE (0x2000001)
#define _BCM_L2_STATION_ID_2_MAX  (0x3000000)

#define _BCM_L2_STATION_ENTRY_INSTALLED      (1 << 0)

#define _BCM_L2_STATION_ENTRY_PRIO_NO_CHANGE (1 << 1)

#define _BCM_L2_STATION_ENTRY_TYPE_NON_TCAM  (1 << 2)

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)

#define _BCM_L2_STATION_ENTRY_TYPE_OAM_MAC  (1 << 3)

#define _BCM_L2_STATION_ENTRY_TYPE_OLP_MAC  (1 << 4)

#define _BCM_L2_STATION_ENTRY_TYPE_XGS_MAC  (1 << 5)

#endif /* BCM_KATANA2_SUPPORT OR TD2+ */

#define _BCM_L2_STATION_ENTRY_TYPE_TCAM_2  (1 << 6)

#define _BCM_L2_STATION_TERM_FLAGS_MASK    (BCM_L2_STATION_IPV4       \
                                            | BCM_L2_STATION_IPV6     \
                                            | BCM_L2_STATION_ARP_RARP \
                                            | BCM_L2_STATION_MPLS     \
                                            | BCM_L2_STATION_MIM      \
                                            | BCM_L2_STATION_TRILL    \
                                            | BCM_L2_STATION_FCOE     \
                                            | BCM_L2_STATION_OAM)

#define _BCM_GH2_L2_STATION_FLAGS_MASK    (BCM_L2_STATION_IPV4   \
                                            | BCM_L2_STATION_IPV6     \
                                            | BCM_L2_STATION_ARP_RARP \
                                            | BCM_L2_STATION_OAM \
                                            | BCM_L2_STATION_COPY_TO_CPU \
                                            | BCM_L2_STATION_DISCARD \
                                            | BCM_L2_STATION_WITH_ID \
                                            | BCM_L2_STATION_REPLACE)

/*
 * Macro:
 *     ParamMax
 * Purpose:
 *     Determine maximum value that can be stored in the field.
 */
#define ParamMax(_unit_, _mem_, _field_)                                      \
            ((soc_mem_field_length((_unit_), (_mem_) , (_field_)) < 32) ?     \
            ((1 << soc_mem_field_length((_unit_), (_mem_), (_field_))) - 1) : \
            (0xFFFFFFFFUL))                             
/*
 * Macro:
 *     ParamCheck
 * Purpose:
 *     Check if value can fit in the field and return error if it exceeds
 *     the range. 
 */
#define ParamCheck(_unit_, _mem_, _field_, _value_)                          \
        do {                                                                 \
            if (0 == ((uint32)(_value_) <=                                   \
                (uint32)ParamMax((_unit_), (_mem_), (_field_)))) {           \
                    LOG_ERROR(BSL_LS_BCM_L2, \
                              (BSL_META("L2(unit %d) Error: _value_ %d > %d (max)" \
                               " mem (%d) field (%d).\n"), _unit_, (_value_),    \
                               (uint32)ParamMax((_unit_), (_mem_), (_field_)),  \
                               (_mem_), (_field_)));                            \
                    return (BCM_E_PARAM);                                    \
            }                                                                \
        } while(0)

/*
 * Macro:
 *     SC_LOCK
 * Purpose:
 *     Lock take the Field control mutex
 */
#define SC_LOCK(control) \
            sal_mutex_take((control)->sc_lock, sal_mutex_FOREVER)

/*
 * Macro:
 *     FP_UNLOCK
 * Purpose:
 *     Lock take the Field control mutex
 */
#define SC_UNLOCK(control) \
            sal_mutex_give((control)->sc_lock);

STATIC _bcm_l2_station_control_t *_station_control[BCM_MAX_NUM_UNITS] = {NULL};
int prio_with_no_free_entries = FALSE;

#endif /* !BCM_TRIUMPH_SUPPORT */
#define DEFAULT_L2DELETE_CHUNKS		64	/* 16k entries / 64 = 256 */

typedef struct _bcm_mac_block_info_s {
    bcm_pbmp_t mb_pbmp;
    int ref_count;
} _bcm_mac_block_info_t;

static _bcm_mac_block_info_t *_mbi_entries[BCM_MAX_NUM_UNITS];
static int _mbi_num[BCM_MAX_NUM_UNITS];

#if defined(BCM_SCORPION_SUPPORT) && defined(INCLUDE_L3)
extern int16 * _sc_subport_group_index[BCM_MAX_NUM_UNITS];
#define _SC_SUBPORT_NUM_PORT  (4096)
#define _SC_SUBPORT_NUM_GROUP (4096/8)
#define _SC_SUBPORT_VPG_FIND(unit, vp, grp) \
    do { \
         int ix; \
         grp = -1; \
         for (ix = 0; ix < _SC_SUBPORT_NUM_GROUP; ix++) { \
              if (_sc_subport_group_index[unit][ix] == vp) { \
                  grp = ix * 8; \
                  break;  \
              } \
         } \
       } while ((0))
#endif /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */
#ifdef PLISIM
#ifdef BCM_KATANA_SUPPORT
extern void _bcm_kt_enable_port_age_simulation(uint32 flags, _bcm_l2_replace_t *rep_st);
#endif
#endif

#if defined(BCM_TRIUMPH_SUPPORT)

#define L2_MYSTA_PROFILE_DEFAULT  0
STATIC soc_profile_mem_t *l2_mysta_profile[BCM_MAX_NUM_UNITS][2];

STATIC int
_bcm_l2_p2p_access_vlan_check_enabled(int unit)
{
    int enable;
    int rv;

    if (soc_feature(unit, soc_feature_pt2pt_access_vlan_check)) {
        rv = bcm_esw_switch_control_get(unit,
               bcmSwitchPointToPointAccessVlanCheckEnable, &enable);
        if (BCM_SUCCESS(rv)) {
            if (enable) {
                return (TRUE);
            }
        }
    }
    return FALSE;
}

/*
 * Function:
 *     _bcm_l2_station_tcam_mem_get
 * Purpose:
 *     Get device specific station TCAM memory name.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     overlay   - (IN) Overlay termination indication.
 *     tcam_mem  - (OUT) Station TCAM memory name.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_l2_station_tcam_mem_get(int unit, int overlay, soc_mem_t *tcam_mem)
{
    if (NULL == tcam_mem) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_RIOT_SUPPORT
    /* Tcam memory for overlay termination */
    if (overlay == _BCM_L2_STATION_ENTRY_OVERLAY) {
        if (soc_feature(unit, soc_feature_my_station_2)) {
            *tcam_mem = MY_STATION_TCAM_2m;
            return (BCM_E_NONE);
        } else {
            return (BCM_E_UNAVAIL);
        }
    }
#endif /* BCM_RIOT_SUPPORT */
    if (SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH(unit)
        || SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit)
        || SOC_IS_VALKYRIE(unit) || SOC_IS_VALKYRIE2(unit)
        || SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit)) {
        
        *tcam_mem = MPLS_STATION_TCAMm;

    } else if (SOC_IS_KATANAX(unit) || SOC_IS_TRIDENT(unit)
               || SOC_IS_TD2_TT2(unit) || SOC_IS_TRIUMPH3(unit)
               || soc_feature(unit, soc_feature_gh2_my_station)) {

        *tcam_mem = MY_STATION_TCAMm;

    } else {

        *tcam_mem = INVALIDm;

        return (BCM_E_UNAVAIL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_l2_station_control_deinit
 * Purpose:
 *     Uninitialize device station control information.
 * Parameters:
 *     unit  - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
int
_bcm_tr_l2_station_control_deinit(int unit)
{
    _bcm_l2_station_control_t  *sc; /* Station control structure pointer. */

    sc = _station_control[unit];
    if (NULL == sc) {
        return (BCM_E_NONE);
    }

    BCM_IF_ERROR_RETURN
        (bcm_tr_l2_station_delete_all(unit));

    if (NULL != sc->entry_arr) {
        sal_free(sc->entry_arr);
        sc->entry_arr = NULL;
    }


    if (NULL != sc->entry_arr_2) {
        sal_free(sc->entry_arr_2);
        sc->entry_arr_2 = NULL;
    }


    if (NULL != sc->sc_lock) {
        sal_mutex_destroy(sc->sc_lock);
    }

    _station_control[unit] = NULL;

    sal_free(sc);

    return (BCM_E_NONE);

}

/*
 * Function:
 *     _bcm_l2_station_control_init
 * Purpose:
 *     Initialize device station control information.
 * Parameters:
 *     unit  - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
int
_bcm_tr_l2_station_control_init(int unit)
{
    int                        rv;          /* Operation return status.     */
    _bcm_l2_station_control_t  *sc = NULL;  /* Station control structure.   */
    soc_mem_t                  tcam_mem;    /* TCAM memory name.            */
    uint32                     mem_size;    /* Size of entry buffer.        */
    int                        max_entries; /* Max no. of entries in table. */
    int                        i;           /* Temporary iterator variable. */

    /* Deinit if control has already been initialized. */
    if (NULL != _station_control[unit]) {
        rv = _bcm_tr_l2_station_control_deinit(unit);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
    }

    /* Allocate the station control memory for this unit. */
    sc = (_bcm_l2_station_control_t *) sal_alloc
            (sizeof(_bcm_l2_station_control_t), "L2 station control");
    if (NULL == sc) {
        return (BCM_E_MEMORY);
    }

    sal_memset(sc, 0, sizeof(_bcm_l2_station_control_t));

    rv = _bcm_l2_station_tcam_mem_get(unit, 0, &tcam_mem);
    if (BCM_FAILURE(rv)) {
        sal_free(sc);
        return (rv);
    }

    sc->last_allocated_sid = (_BCM_L2_STATION_ID_BASE - 1);

    max_entries = soc_mem_index_count(unit, tcam_mem);

    sc->entries_total = max_entries;

    sc->entries_free = max_entries;

#ifdef BCM_METROLITE_SUPPORT
    if (SOC_IS_METROLITE(unit)) {
        sc->port_entries_total = ML_PORT_MAC_MAX;

        sc->port_entries_free = ML_PORT_MAC_MAX;

        sc->olp_entries_total = _BCM_OLP_MAC_MAX;

        sc->olp_entries_free = _BCM_OLP_MAC_MAX;
    } else
#endif
#ifdef BCM_SABER2_SUPPORT
    if (SOC_IS_SABER2(unit)) {
        sc->port_entries_total =SB2_PORT_MAC_MAX;

        sc->port_entries_free =SB2_PORT_MAC_MAX;

        sc->olp_entries_total = _BCM_OLP_MAC_MAX; 

        sc->olp_entries_free = _BCM_OLP_MAC_MAX; 
    } else
#endif
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {

        sc->port_entries_total =_BCM_PORT_MAC_MAX;

        sc->port_entries_free =_BCM_PORT_MAC_MAX;
    }
#endif
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (SOC_IS_KATANA2(unit) || soc_feature(unit, soc_feature_fp_based_oam)) {
        sc->olp_entries_total = _BCM_OLP_MAC_MAX; 
        sc->olp_entries_free = _BCM_OLP_MAC_MAX; 
    }
#endif

    sc->entry_count = 0;
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        max_entries = sc->entries_total + sc->port_entries_total +
                      sc->olp_entries_total + 1;
    }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam)) {
        max_entries = sc->entries_total + sc->olp_entries_total + 1;
    }
#endif
    mem_size = (max_entries * sizeof(_bcm_l2_station_entry_t *));

    sc->entry_arr = (_bcm_l2_station_entry_t **)
                        sal_alloc(mem_size, "L2 station entry pointers");
    if (NULL == sc->entry_arr) {
        sal_free(sc);
        sc = NULL;
        return (BCM_E_MEMORY);
    }

    for (i = 0; i < max_entries; i++) {
        sc->entry_arr[i] = NULL;
    }

#ifdef BCM_RIOT_SUPPORT
    if (soc_feature(unit, soc_feature_riot) ||
        soc_feature(unit, soc_feature_pt2pt_access_vlan_check)) {
        rv = _bcm_l2_station_tcam_mem_get(unit,
                 _BCM_L2_STATION_ENTRY_OVERLAY, &tcam_mem);
        if (BCM_FAILURE(rv)) {
            sal_free(sc);
            sc = NULL;
            return (rv);
        }

        sc->last_allocated_sid_2 = (_BCM_L2_STATION_ID_2_BASE - 1);
        max_entries = soc_mem_index_count(unit, tcam_mem);
        if (max_entries > 0) {
        sc->entries_total_2 = max_entries;
        sc->entries_free_2 = max_entries;

        mem_size = (max_entries * sizeof(_bcm_l2_station_entry_t *));

        sc->entry_arr_2 = (_bcm_l2_station_entry_t **)
                            sal_alloc(mem_size, "L2 station entry pointers");
        if (NULL == sc->entry_arr_2) {
            sal_free(sc);
            sc = NULL;
            return (BCM_E_MEMORY);
        }

        for (i = 0; i < max_entries; i++) {
            sc->entry_arr_2[i] = NULL;
        }
        }

    }
#endif /* BCM_RIOT_SUPPORT */

    sc->sc_lock = sal_mutex_create("L2 station control.lock");
    if (NULL == sc->sc_lock) {
        sal_free(sc->entry_arr);
        sc->entry_arr = NULL;
        if (soc_feature(unit, soc_feature_riot)) {
            sal_free(sc->entry_arr_2);
            sc->entry_arr_2 = NULL;
        }
        sal_free(sc);
        sc = NULL;
        return (BCM_E_MEMORY);
    }

    _station_control[unit] = sc;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_l2_station_control_get
 * Purpose:
 *     Get device station control structure pointer.
 * Parameters:
 *     unit  - (IN) BCM device number
 *     sc    - (OUT) Device station structure pointer. 
 * Retruns:
 *     BCM_E_XXX
 */
int
_bcm_l2_station_control_get(int unit, _bcm_l2_station_control_t **sc)
{

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        return _bcm_th3_l2_station_control_get(unit, sc);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    /* Input parameter check. */
    if (NULL == sc) {
        return (BCM_E_PARAM);
    }

    if (NULL == _station_control[unit]) {
        return (BCM_E_INIT);
    }

    *sc = _station_control[unit];

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_l2_station_entry_get
 * Purpose:
 *     Get station entry details by performing
 *     lookup using station ID.
 * Parameters:
 *     unit  - (IN) BCM device number
 *     sid   - (IN) Station ID.
 *     ent_p - (OUT) Station look up result.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_l2_station_entry_get(int unit,
                          int sid,
                          _bcm_l2_station_entry_t **ent_p)
{
    _bcm_l2_station_control_t *sc;   /* Station control structure pointer. */
    _bcm_l2_station_entry_t **entry_arr;
    int                       index; /* Entry index.                       */
    int                       count;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        return _bcm_th3_l2_station_entry_get(unit, sid, ent_p);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    /* Input parameter check. */
    if (NULL == ent_p) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

#ifdef BCM_RIOT_SUPPORT
    if (_BCM_L2_STATION_ID_OVERLAY(sid) || _BCM_L2_STATION_ID_2(sid)) {
        count = sc->entries_total_2;
        entry_arr = (sc->entry_arr_2);
    } else
#endif /* BCM_RIOT_SUPPORT */
    {
        count = sc->entries_total;
        entry_arr = (sc->entry_arr);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_fp_based_oam)) {
            count = sc->entries_total + sc->olp_entries_total + 1;
        }
#endif
    }
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        count = sc->entries_total + sc->port_entries_total + 
                                          sc->olp_entries_total + 1;
    }
#endif

    for (index = 0; index < count; index++) {

        if (NULL == entry_arr[index]) {
            continue;
        }

        if (sid == entry_arr[index]->sid) {
            *ent_p = entry_arr[index];
            LOG_DEBUG(BSL_LS_BCM_L2,
                      (BSL_META_U(unit,
                                  "L2(unit %d) Info: (SID=%d) - found (idx=%d).\n"),
                       unit, sid, index));
            return (BCM_E_NONE);
        }
    }

    LOG_DEBUG(BSL_LS_BCM_L2,
              (BSL_META_U(unit,
                          "L2(unit %d) Info: (SID=%d) - not found (idx=%d).\n"),
               unit, sid, index));

    /* L2 station entry with ID == sid not found. */
    return (BCM_E_NOT_FOUND);
}

int
_bcm_l2_mysta_profile_detach(int unit)
{
    int i, rv, t_count = 2;
    soc_mem_t mem[2] = {MY_STATION_PROFILE_1m, MY_STATION_PROFILE_2m};

    if (l2_mysta_profile[unit][1] == NULL) {
        t_count = 1;
    }

    for (i = 0; i < t_count; ++i) {
        if (SOC_MEM_IS_VALID(unit, mem[i])) {
            /* De-initialize the MY_STATION_PROFILE_1 table */
            rv = soc_profile_mem_destroy(unit, l2_mysta_profile[unit][i]);
            BCM_IF_ERROR_RETURN(rv);

            sal_free(l2_mysta_profile[unit][i]);
            l2_mysta_profile[unit][i] = NULL;
        }
    }
    return BCM_E_NONE;
}

int
_bcm_l2_mysta_profile_init(int unit)
{
    int i, idx, j, t_count = 2;
    my_station_tcam_entry_t mysta_entry;
    uint32 temp_index;
    soc_mem_t mem[2];
    int entry_words[2];
    void *entries[2];
    my_station_profile_1_entry_t mysta_profile_1;
    my_station_profile_2_entry_t mysta_profile_2;
    void *prof_entry;
    uint32 ent_sz;
    uint32 dest_type = 0;
    /* MY_STATION_PROFILE_2m is for MY_STATION_TCAM_2m, but we don't know if
     * they are in sync, so set table_count=1 */
    int table_count = 1;
    soc_mem_t tcam_mem[2] = {MY_STATION_TCAMm, MY_STATION_TCAM_2m};

    /* Create profile table cache (or re-init if it already exists) */
    mem[0] = MY_STATION_PROFILE_1m;
    mem[1] = MY_STATION_PROFILE_2m;
    ent_sz = sizeof(my_station_profile_1_entry_t);
    prof_entry = &mysta_profile_1;

    for(j = 0; j < t_count; ++j) {
        if (SOC_MEM_IS_VALID(unit, mem[j])) {
            if (j == 1) {
                prof_entry = &mysta_profile_2;
                ent_sz = sizeof(my_station_profile_2_entry_t);
            }
            /* Initialize the ING_VLAN_TAG_ACTION_PROFILE table */

            if (l2_mysta_profile[unit][j] == NULL) {
                l2_mysta_profile[unit][j] = sal_alloc(sizeof(soc_profile_mem_t),
                                                     "L2 MYSTA Profile Mem");
                if (l2_mysta_profile[unit][j] == NULL) {
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(l2_mysta_profile[unit][j]);
            }

            /* Create profile table cache (or re-init if it already exists) */
            entry_words[0] = entry_words[1] = ent_sz / sizeof(uint32);
            SOC_IF_ERROR_RETURN(
                soc_profile_mem_create(unit, &mem[j], entry_words, table_count,
                                   l2_mysta_profile[unit][j]));

            if (SOC_WARM_BOOT(unit)) {
                /* Increment the ref count for all ports */
                for (i = 0; i < soc_mem_index_count(unit, tcam_mem[j]); i++) {
                    if (j == 1) {
                        SOC_IF_ERROR_RETURN
                            (READ_MY_STATION_TCAM_2m(unit, MEM_BLOCK_ANY, i, &mysta_entry));
                    } else {
                        SOC_IF_ERROR_RETURN
                            (READ_MY_STATION_TCAMm(unit, MEM_BLOCK_ANY, i, &mysta_entry));
                    }
                    idx = soc_mem_field32_get(unit, tcam_mem[j],
                                              &mysta_entry, VALIDf);
                    if (idx == 0) {
                        continue;
                    }
                    idx = soc_mem_field32_dest_get(unit, tcam_mem[j],
                                &mysta_entry, DESTINATIONf, &dest_type);
                    if (dest_type != SOC_MEM_FIF_DEST_MYSTA) {
                        continue;
                    }

                    SOC_PROFILE_MEM_REFERENCE(unit, l2_mysta_profile[unit][j], idx, 1);
                    SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, l2_mysta_profile[unit][j], idx, 1);
                }

                /* One extra increment to preserve location
                 * ING_ACTION_PROFILE_DEFAULT */
                SOC_PROFILE_MEM_REFERENCE(unit, l2_mysta_profile[unit][j],
                    L2_MYSTA_PROFILE_DEFAULT,
                    soc_mem_index_count(unit, tcam_mem[j]) + 1);

            } else {
                /* Initialize the ING_ACTION_PROFILE_DEFAULT. For untagged and
                 * inner-tagged packets, always add an outer tag.
                 */
                sal_memset(prof_entry, 0, ent_sz);
                entries[0] = prof_entry;
                entries[1] = prof_entry;
                SOC_IF_ERROR_RETURN
                    (soc_profile_mem_add(unit, l2_mysta_profile[unit][j],
                                         (void *) &entries, 1, &temp_index));

                /* Increment the ref count for MY_STATION_TCAM */
                SOC_PROFILE_MEM_REFERENCE(unit, l2_mysta_profile[unit][j],
                    L2_MYSTA_PROFILE_DEFAULT,
                    soc_mem_index_count(unit, tcam_mem[j]));
             }
        }
    }

    return BCM_E_NONE;
}

void
_bcm_l2_mysta_station_to_entry(int unit, bcm_l2_station_t *station, soc_mem_t mysp_mem, void *entry)
{
    soc_mem_field32_set(unit, mysp_mem, entry, MIM_TERMINATION_ALLOWEDf,
                        ((station->flags & BCM_L2_STATION_MIM) ? 1 : 0));
    soc_mem_field32_set(unit, mysp_mem, entry, MPLS_TERMINATION_ALLOWEDf,
                        ((station->flags & BCM_L2_STATION_MPLS) ? 1 : 0));
    soc_mem_field32_set(unit, mysp_mem, entry, IPV4_TERMINATION_ALLOWEDf,
                        ((station->flags & BCM_L2_STATION_IPV4) ? 1 : 0));
    soc_mem_field32_set(unit, mysp_mem, entry, IPV6_TERMINATION_ALLOWEDf,
                        ((station->flags & BCM_L2_STATION_IPV6) ? 1 : 0));
    soc_mem_field32_set(unit, mysp_mem, entry, ARP_RARP_TERMINATION_ALLOWEDf,
                        ((station->flags & BCM_L2_STATION_ARP_RARP) ? 1 : 0));
    if (soc_mem_field_valid(unit, mysp_mem, TRILL_TERMINATION_ALLOWEDf)) {
        soc_mem_field32_set(unit, mysp_mem, entry, TRILL_TERMINATION_ALLOWEDf,
                            ((station->flags & BCM_L2_STATION_TRILL) ? 1 : 0));
    }
    if (soc_mem_field_valid(unit, mysp_mem, OAM_TERMINATION_ALLOWEDf)) {
        soc_mem_field32_set(unit, mysp_mem, entry, OAM_TERMINATION_ALLOWEDf,
                            ((station->flags & BCM_L2_STATION_OAM) ? 1 : 0));
    }
    soc_mem_field32_set(unit, mysp_mem, entry, FCOE_TERMINATION_ALLOWEDf,
                        ((station->flags & BCM_L2_STATION_FCOE) ? 1 : 0));
    soc_mem_field32_set(unit, mysp_mem, entry, IPV4_MULTICAST_TERMINATION_ALLOWEDf,
                        ((station->flags & BCM_L2_STATION_IPV4_MCAST) ? 1 : 0));
    soc_mem_field32_set(unit, mysp_mem, entry, IPV6_MULTICAST_TERMINATION_ALLOWEDf,
                        ((station->flags & BCM_L2_STATION_IPV6_MCAST) ? 1 : 0));
    if (soc_mem_field_valid(unit, mysp_mem, NSH_OVER_L2_TERMINATION_ALLOWEDf)) {
        soc_mem_field32_set(unit, mysp_mem, entry, NSH_OVER_L2_TERMINATION_ALLOWEDf,
                            ((station->flags & BCM_L2_STATION_NSH_OVER_L2) ? 1 : 0));
    }
}

int
_bcm_l2_mysta_profile_entry_add(int unit,
                                void *entry,
                                int mem_no,
                                uint32 *mysta_prof_id)
{
    void *entries[2];
    entries[0] = entry;
    entries[1] = entry;

    return (soc_profile_mem_add(unit, l2_mysta_profile[unit][mem_no],
                                (void *) &entries, 1, mysta_prof_id));
}

void
_bcm_l2_mysta_profile_entry_increment(int unit, int mem_no, uint32 mysta_prof_id)
{
    SOC_PROFILE_MEM_REFERENCE(unit, l2_mysta_profile[unit][mem_no], mysta_prof_id, 1);
}

void
_bcm_l2_mysta_entry_to_station(int unit, void *entry, soc_mem_t mem, bcm_l2_station_t *station)
{
    uint32 val;

    val = soc_mem_field32_get(unit, mem, entry, MIM_TERMINATION_ALLOWEDf);
    station->flags |= val ? BCM_L2_STATION_MIM : 0;

    val = soc_mem_field32_get(unit, mem, entry, MPLS_TERMINATION_ALLOWEDf);
    station->flags |= val ? BCM_L2_STATION_MPLS : 0;

    val = soc_mem_field32_get(unit, mem, entry, IPV4_TERMINATION_ALLOWEDf);
    station->flags |= val ? BCM_L2_STATION_IPV4 : 0;

    val = soc_mem_field32_get(unit, mem, entry, IPV6_TERMINATION_ALLOWEDf);
    station->flags |= val ? BCM_L2_STATION_IPV6 : 0;

    val = soc_mem_field32_get(unit, mem, entry, ARP_RARP_TERMINATION_ALLOWEDf);
    station->flags |= val ? BCM_L2_STATION_ARP_RARP : 0;

    if (soc_mem_field_valid(unit, mem, TRILL_TERMINATION_ALLOWEDf)) {
        val = soc_mem_field32_get(unit, mem, entry, TRILL_TERMINATION_ALLOWEDf);
        station->flags |= val ? BCM_L2_STATION_TRILL : 0;
    }

    if (soc_mem_field_valid(unit, mem, OAM_TERMINATION_ALLOWEDf)) {
        val = soc_mem_field32_get(unit, mem, entry, OAM_TERMINATION_ALLOWEDf);
        station->flags |= val ? BCM_L2_STATION_OAM : 0;
    }

    val = soc_mem_field32_get(unit, mem, entry, FCOE_TERMINATION_ALLOWEDf);
    station->flags |= val ? BCM_L2_STATION_FCOE : 0;

    val = soc_mem_field32_get(unit, mem, entry, IPV4_MULTICAST_TERMINATION_ALLOWEDf);
    station->flags |= val ? BCM_L2_STATION_IPV4_MCAST : 0;

    val = soc_mem_field32_get(unit, mem, entry, IPV6_MULTICAST_TERMINATION_ALLOWEDf);
    station->flags |= val ? BCM_L2_STATION_IPV6_MCAST : 0;

    if (soc_mem_field_valid(unit, mem, NSH_OVER_L2_TERMINATION_ALLOWEDf)) {
        val = soc_mem_field32_get(unit, mem, entry, NSH_OVER_L2_TERMINATION_ALLOWEDf);
        station->flags |= val ? BCM_L2_STATION_NSH_OVER_L2 : 0;
    }
}

int
_bcm_l2_mysta_profile_entry_get(int unit,
                                void *entry,
                                int mem_no,
                                uint32 mysta_prof_id)
{
    int rv = 0;
    void *entries[2];
    entries[0] = entry;
    entries[1] = entry;
    rv = soc_profile_mem_get(unit, l2_mysta_profile[unit][mem_no],
                             mysta_prof_id, 1, entries);
    return rv;
}

/*
 * Function : _bcm_l2_mysta_profile_entry_delete
 *
 * Purpose  : remove an entry from MY_STATION_PROFILE_1 profile table
 */
int
_bcm_l2_mysta_profile_entry_delete(int unit, int mem_no, uint32 mysta_prof_id)
{
    return soc_profile_mem_delete(unit, l2_mysta_profile[unit][mem_no], mysta_prof_id);
}

#endif

typedef enum _bcm_tr_l2x_memacc_field_e {
    _BCM_TR_L2X_MEMACC_VALID_f,
    _BCM_TR_L2X_MEMACC_KEY_TYPE_f,
    _BCM_TR_L2X_MEMACC_MAC_ADDR_f,
    _BCM_TR_L2X_MEMACC_VFI_f,
    _BCM_TR_L2X_MEMACC_VLAN_ID_f,
    _BCM_TR_L2X_MEMACC_L2MC_PTR_f,
    _BCM_TR_L2X_MEMACC_VPG_TYPE_f,
    _BCM_TR_L2X_MEMACC_DEST_TYPE_f,
    _BCM_TR_L2X_MEMACC_TGID_f,
    _BCM_TR_L2X_MEMACC_REMOTE_TRUNK_f,
    _BCM_TR_L2X_MEMACC_PORT_NUM_f,
    _BCM_TR_L2X_MEMACC_MODULE_ID_f,
    _BCM_TR_L2X_MEMACC_CLASS_ID_f,
    _BCM_TR_L2X_MEMACC_MAC_BLOCK_INDEX_f,
    _BCM_TR_L2X_MEMACC_PRI_f,
    _BCM_TR_L2X_MEMACC_CPU_f,
    _BCM_TR_L2X_MEMACC_DST_DISCARD_f,
    _BCM_TR_L2X_MEMACC_SRC_DISCARD_f,
    _BCM_TR_L2X_MEMACC_SCP_f,
    _BCM_TR_L2X_MEMACC_STATIC_BIT_f,
    _BCM_TR_L2X_MEMACC_LIMIT_COUNTED_f,
    _BCM_TR_L2X_MEMACC_PENDING_f,
    _BCM_TR_L2X_MEMACC_HITDA_f,
    _BCM_TR_L2X_MEMACC_HITSA_f,
    _BCM_TR_L2X_MEMACC_LOCAL_SA_f,
    _BCM_TR_L2X_MEMACC_REMOTE_f,
    _BCM_TR_L2X_MEMACC_DESTINATION_f,
    _BCM_TR_L2X_MEMACC_T_f,
    _BCM_TR_L2X_MEMACC_L3_f,
    _BCM_TR_L2X_MEMACC_RPE_f,
    _BCM_TR_L2X_MEMACC_OPAQUE_USAGE_TYPE_f,
    _BCM_TR_L2X_MEMACC_GBP_SID_f,
    _BCM_TR_L2X_MEMACC_MESH_f,
    _BCM_TR_L2X_MEMACC_NUM
} _bcm_tr_l2x_memacc_field_t;

static soc_memacc_t *_bcm_tr_l2x_memacc[BCM_UNITS_MAX];
static soc_field_t   _bcm_tr_l2x_fields[] = {
    VALIDf,
    KEY_TYPEf,
    MAC_ADDRf,
    VFIf,
    VLAN_IDf,
    L2MC_PTRf,
    VPG_TYPEf,
    DEST_TYPEf,
    TGIDf,
    REMOTE_TRUNKf,
    PORT_NUMf,
    MODULE_IDf,
    CLASS_IDf,
    MAC_BLOCK_INDEXf,
    PRIf,
    CPUf,
    DST_DISCARDf,
    SRC_DISCARDf,
    SCPf,
    STATIC_BITf,
    LIMIT_COUNTEDf,
    PENDINGf,
    HITDAf,
    HITSAf,
    LOCAL_SAf,
    REMOTEf,
    DESTINATIONf,
    Tf,
    L3f,
    RPEf,
    OPAQUE_USAGE_TYPEf,
    GBP_SIDf,
    MESHf
};

#define _BCM_L2X_MEMACC_FIELD(_u_, _f_) \
            (&_bcm_tr_l2x_memacc[_u_][_BCM_TR_L2X_MEMACC_##_f_])

#define _BCM_L2X_MEMACC_FIELD_VALID(_u_, _f_) \
            SOC_MEMACC_VALID(_BCM_L2X_MEMACC_FIELD(_u_, _f_))

#define _BCM_L2X_MEMACC_FIELD32_GET(_u_, _entBuf_, _f_) \
            soc_memacc_field32_get(_BCM_L2X_MEMACC_FIELD(_u_, _f_), _entBuf_)

#define _BCM_L2X_MEMACC_FIELD32_SET(_u_, _entBuf_, _f_, _v_) \
            soc_memacc_field32_set(_BCM_L2X_MEMACC_FIELD(_u_, _f_), _entBuf_, \
                                   _v_)

#if defined(BCM_TRIDENT2_SUPPORT)

typedef enum _bcm_td2_l2hit_mem_e {
    _BCM_TD2_L2HITDA_X,
    _BCM_TD2_L2HITDA_Y,
    _BCM_TD2_L2HITSA_X,
    _BCM_TD2_L2HITSA_Y,
    _BCM_TD2_L2HIT_MEM_NUM
} _bcm_td2_l2hit_mem_t;

typedef enum _bcm_td2_l2hit_field_e {
    _BCM_TD2_L2HITDA_HITDA_f_0,
    _BCM_TD2_L2HITDA_HITDA_f_1,
    _BCM_TD2_L2HITDA_HITDA_f_2,
    _BCM_TD2_L2HITDA_HITDA_f_3,
    _BCM_TD2_L2HITSA_HITSA_f_0,
    _BCM_TD2_L2HITSA_HITSA_f_1,
    _BCM_TD2_L2HITSA_HITSA_f_2,
    _BCM_TD2_L2HITSA_HITSA_f_3,
    _BCM_TD2_L2HITSA_LOCAL_SA_f_0,
    _BCM_TD2_L2HITSA_LOCAL_SA_f_1,
    _BCM_TD2_L2HITSA_LOCAL_SA_f_2,
    _BCM_TD2_L2HITSA_LOCAL_SA_f_3,
    _BCM_TD2_L2HIT_FIELD_NUM
} _bcm_td2_l2hit_field_t;


static soc_memacc_t *_td2_l2hit_memacc[BCM_UNITS_MAX][_BCM_TD2_L2HIT_MEM_NUM];

#define _TD2_L2HIT_MEM(_u_, _m_, _p_) \
            (_BCM_TD2_##_m_##_X + (_p_))

#define _TD2_L2HIT_FIELD(_u_, _m_,_f_, _i_) \
            (_BCM_TD2_##_m_##_##_f_##_0 + (_i_))

#define _TD2_L2HIT_MEMACC_FIELD(_u_, _mem_, _fld_) \
            (&_td2_l2hit_memacc[_u_][_mem_][_fld_])

#define _TD2_L2HIT_MEMACC_FIELD32_GET(_u_, _mem_, _entBuf_, _fld_) \
            soc_memacc_field32_get(_TD2_L2HIT_MEMACC_FIELD(_u_, _mem_, _fld_), \
                                   _entBuf_)

#endif /* BCM_TRIDENT2_SUPPORT */

STATIC int
_bcm_tr_l2_memacc_init(int unit)
{
    int rv, i;
    int alloc_size;

    if (_bcm_tr_l2x_memacc[unit] != NULL) {
        sal_free(_bcm_tr_l2x_memacc[unit]);
    }
    alloc_size = sizeof(soc_memacc_t) * _BCM_TR_L2X_MEMACC_NUM;
    _bcm_tr_l2x_memacc[unit] = sal_alloc(alloc_size, "L2 memacc");
    if (_bcm_tr_l2x_memacc[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(_bcm_tr_l2x_memacc[unit], 0, alloc_size);

    for (i = 0; i < _BCM_TR_L2X_MEMACC_NUM; i++) {
        rv = soc_memacc_init(unit, L2Xm, _bcm_tr_l2x_fields[i], 
                             &_bcm_tr_l2x_memacc[unit][i]);
        if (BCM_FAILURE(rv)) {
            SOC_MEMACC_INVALID_SET(&_bcm_tr_l2x_memacc[unit][i]);
        }
    }

#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TRIDENT2(unit) || SOC_IS_TITAN2(unit)
        || SOC_IS_TD2P_TT2P(unit)) {
        int m, f;
        static soc_mem_t   _bcm_td2_l2hit_mems[] = {
            L2_HITDA_ONLY_Xm,
            L2_HITDA_ONLY_Ym,
            L2_HITSA_ONLY_Xm,
            L2_HITSA_ONLY_Ym,
        };
        static soc_field_t   _bcm_td2_l2hit_fields[] = {
            HITDA_0f, HITDA_1f, HITDA_2f, HITDA_3f,
            HITSA_0f, HITSA_1f, HITSA_2f, HITSA_3f,
            LOCAL_SA_0f, LOCAL_SA_1f, LOCAL_SA_2f, LOCAL_SA_3f,
        };

        for (m = 0; m < _BCM_TD2_L2HIT_MEM_NUM; m++) {
            if (_td2_l2hit_memacc[unit][m] != NULL) {
                sal_free(_td2_l2hit_memacc[unit][m]);
            }
            alloc_size = sizeof(soc_memacc_t) * _BCM_TD2_L2HIT_FIELD_NUM;
            _td2_l2hit_memacc[unit][m] = sal_alloc(alloc_size, "L2hit memacc");
            if (_td2_l2hit_memacc[unit][m] == NULL) {
                sal_free(_bcm_tr_l2x_memacc[unit]);
                _bcm_tr_l2x_memacc[unit] = NULL;
                return BCM_E_MEMORY;
            }
            
            for (f = 0; f < _BCM_TD2_L2HIT_FIELD_NUM; f++) {
                rv = soc_memacc_init(unit, _bcm_td2_l2hit_mems[m], 
                                     _bcm_td2_l2hit_fields[f], 
                                     &_td2_l2hit_memacc[unit][m][f]);
                if (BCM_FAILURE(rv)) {
                    SOC_MEMACC_INVALID_SET(&_td2_l2hit_memacc[unit][m][f]);
                }
            }
        }
    }
#endif

    return BCM_E_NONE;
}

STATIC int
_bcm_tr_l2_memacc_deinit(int unit)
{
    if (_bcm_tr_l2x_memacc[unit] != NULL) {
        sal_free(_bcm_tr_l2x_memacc[unit]);
        _bcm_tr_l2x_memacc[unit] = NULL;
    }

#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TRIDENT2(unit) || SOC_IS_TITAN2(unit)
        || SOC_IS_TD2P_TT2P(unit)) {
        int m;
        for (m = 0; m < _BCM_TD2_L2HIT_MEM_NUM; m++) {
            if (_td2_l2hit_memacc[unit][m] != NULL) {
                sal_free(_td2_l2hit_memacc[unit][m]);
                _td2_l2hit_memacc[unit][m] = NULL;
            }
        }
    }
#endif
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_l2_from_l2x
 * Purpose:
 *      Convert a Triumph L2X entry to an L2 API data structure
 * Parameters:
 *      unit        Unit number
 *      l2addr      (OUT) L2 API data structure
 *      l2x_entry   Triumph L2X entry
 */
int
_bcm_tr_l2_from_l2x(int unit, bcm_l2_addr_t *l2addr, l2x_entry_t *l2x_entry)
{
    int l2mc_index, mb_index, vfi, rval;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    int my_station_config = 0;
#endif
    soc_memacc_t *memacc;
#if defined(INCLUDE_L3)
    int vp = -1;
#endif /* INCLUDE_L3 */
    int vfi_multicast = 0;
#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
    int dest_type_is_nhi = 0;
    int ecmp = 0, nh_ecmp_index = 0;
#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TSN_SUPPORT)
    uint32 hw_id = 0;
    bcm_tsn_flow_t tsn_flow_id = 0;
#if defined(BCM_TSN_SR_SUPPORT)
    bcm_tsn_sr_flow_t sr_flow_id = 0;
    int mp_index = 0; /* TSN SR MAC Proxy profile index */
    bcm_pbmp_t mp_pbmp;
#endif /* BCM_TSN_SR_SUPPORT */
#endif /* BCM_TSN_SUPPORT */
    int vfi_fid_used = 0;
#ifdef BCM_TRIDENT3_SUPPORT
    int key_type = 0;
#endif
    uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;

    sal_memset(l2addr, 0, sizeof(*l2addr));

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow)) {
       key_type = soc_mem_field32_get(unit, L2Xm, l2x_entry, KEY_TYPEf);
       if ((key_type >= TD3_L2_HASH_KEY_TYPE_COUNT)
#ifdef BCM_HURRICANE4_SUPPORT
           /* SOBMH FLEX is a fixed view */
           && (!(soc_feature(unit, soc_feature_hr4_a0_sobmh_war) &&
                 (key_type == HR4_WAR_L2X_SOBMH_KEY_TYPE)))
#endif
          ) {
           rval = _bcm_td3_flex_l2_from_l2x(unit, l2addr, l2x_entry, &mb_index);

           if (mb_index) {
               BCM_PBMP_ASSIGN(l2addr->block_bitmap,
                               _mbi_entries[unit][mb_index].mb_pbmp);
           }
           return rval;
       }
    }
#endif

#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vxlan_lite_riot) ||
        SOC_IS_FIRELIGHT(unit)) {
        if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, KEY_TYPE_f) ==
            GH2_L2_HASH_KEY_TYPE_VFI_MULTICAST) {
            vfi_multicast = 1;
        }
    }
#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3 */

    memacc = _BCM_L2X_MEMACC_FIELD(unit, MAC_ADDR_f);
    soc_memacc_mac_addr_get(memacc, l2x_entry, l2addr->mac);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_pvlan_on_vfi)) {
        int enable;

        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchSharedVlanEnable, &enable));
        if (enable) {
            vfi_fid_used = 1;
        }
    }
#endif

    if (((_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, KEY_TYPE_f) ==
        TR_L2_HASH_KEY_TYPE_VFI) || (vfi_multicast)) && !vfi_fid_used) {
        vfi = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, VFI_f);
        COMPILER_REFERENCE(vfi);
        /* VPLS or MIM VPN */
#if defined(INCLUDE_L3)
        if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
            _BCM_MPLS_VPN_SET(l2addr->vid, _BCM_MPLS_VPN_TYPE_VPLS, vfi);
        } else if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
            _BCM_MIM_VPN_SET(l2addr->vid, _BCM_MIM_VPN_TYPE_MIM, vfi);
        } else  if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeL2Gre)) {
#if defined(BCM_TRIUMPH3_SUPPORT)
                if (soc_feature(unit, soc_feature_l2gre)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_tr3_l2gre_vpn_get(unit, vfi, &l2addr->vid));
                }
#endif
        } else if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeVxlan)) {
#if defined(BCM_TRIDENT2_SUPPORT)
                if (soc_feature(unit, soc_feature_vxlan)) {
                    BCM_IF_ERROR_RETURN(
                      _bcm_td2_vxlan_vpn_get(unit, vfi, &l2addr->vid));
                }
#endif
        } else if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeFlow)) {
#if defined(BCM_TRIDENT3_SUPPORT)
                if (soc_feature(unit, soc_feature_flex_flow)) {
                    BCM_IF_ERROR_RETURN(
                      bcmi_esw_flow_vpnid_get(unit, vfi, &l2addr->vid));
                }
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
        } else if (soc_feature(unit, soc_feature_vxlan_lite)) {
            if (bcmi_gh2_vxlan_vfi_used_get(unit, vfi)) {
                BCM_IF_ERROR_RETURN(
                    bcmi_gh2_vxlan_vfi_to_vpn_get(unit, vfi, &l2addr->vid));
                /* Get DEST_TYPEf value for VFI view */
                dest_type_is_nhi = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry,
                                                               DEST_TYPE_f);
            }
#endif /* BCM_GREYHOUND2_SUPPORT */
        }
#endif /* INCLUDE_L3 */
    } else {  
        l2addr->vid = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, VLAN_ID_f); 
    }

    if (BCM_MAC_IS_MCAST(l2addr->mac)) {
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_L3)
        int dest_ipmc_index = 0;
        dest_ipmc_index = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, DESTINATION_f);
        if ((soc_feature(unit,soc_feature_roe) ||
             soc_feature(unit,soc_feature_roe_custom)) &&
            (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, DEST_TYPE_f) == 3)) {
            l2addr->flags2 |= BCM_L2_FLAGS2_L3_MULTICAST;
            _BCM_MULTICAST_GROUP_SET(l2addr->l3mc_group,
                                     _BCM_MULTICAST_TYPE_L3,
                                      dest_ipmc_index);
        } else
#endif /* BCM_MONTEREY_SUPPORT && INCLUDE_L3 */
        {
        l2addr->flags |= BCM_L2_MCAST;
        if (soc_feature(unit, soc_feature_generic_dest)) {
            l2mc_index = soc_mem_field32_dest_get(unit, L2Xm, l2x_entry,
                            DESTINATIONf, &dest_type);
            if ((dest_type != SOC_MEM_FIF_DEST_L2MC) &&
                (dest_type != SOC_MEM_FIF_DEST_IPMC)) {
                l2mc_index = 0;
            }
        } else {
        l2mc_index = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, L2MC_PTR_f);
        }
        if ((_BCM_L2X_MEMACC_FIELD_VALID(unit, VPG_TYPE_f) &&
            _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, VPG_TYPE_f)) ||
            (dest_type == SOC_MEM_FIF_DEST_IPMC)) {
            if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, KEY_TYPE_f) ==
                                               TR_L2_HASH_KEY_TYPE_VFI) {
                vfi = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, VFI_f);
                /* VPLS, MIM, L2GRE, VXLAN multicast */
#if defined(INCLUDE_L3)
                if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
                    _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group,
                            _BCM_MULTICAST_TYPE_VPLS, l2mc_index);
                } else if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
                    _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group,
                            _BCM_MULTICAST_TYPE_MIM, l2mc_index);
                } else  if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeFlow)) {
                    _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group,
                            _BCM_MULTICAST_TYPE_FLOW, l2mc_index);
                } else  if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeVxlan)) {
                    _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group,
                            _BCM_MULTICAST_TYPE_VXLAN, l2mc_index);
                } else  if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeL2Gre)) {
                    _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group,
                            _BCM_MULTICAST_TYPE_L2GRE, l2mc_index);
                }
#endif
            } else {
#if defined(INCLUDE_L3)
                int rv;
                rv = _bcm_tr_multicast_ipmc_group_type_get(unit,
                            l2mc_index, &l2addr->l2mc_group);
                if (BCM_E_NOT_FOUND == rv) {
                    /* Assume subport multicast */
                    _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group,
                            _BCM_MULTICAST_TYPE_SUBPORT, l2mc_index);
                } else if (BCM_FAILURE(rv)) {
                    return rv;
                }
#endif
            }
#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
        } else if (soc_feature(unit, soc_feature_vxlan_lite)) {
            if ((dest_type_is_nhi == 0x4) || (dest_type_is_nhi == 0x5)) {
                if (dest_type_is_nhi == 0x5) {
                    ecmp = 1; /* ECMP */
                } else {
                    ecmp = 0; /* NHI */
                }

                nh_ecmp_index = soc_L2Xm_field32_get(unit, l2x_entry,
                                                     NHI_ECMPf);

                BCM_IF_ERROR_RETURN(
                    bcmi_gh2_vxlan_port_from_nh_ecmp_index(unit, ecmp,
                                                           nh_ecmp_index,
                                                           &vp));
                BCM_GPORT_VXLAN_PORT_ID_SET(l2addr->port, vp);
            } else if (dest_type_is_nhi == 0x3) {
                _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group,
                                         _BCM_MULTICAST_TYPE_VXLAN,
                                         l2mc_index);
            } else {
                l2addr->l2mc_group = l2mc_index;
                /* Translate l2mc index */
                BCM_IF_ERROR_RETURN(
                    bcm_esw_switch_control_get(unit, bcmSwitchL2McIdxRetType,
                                               &rval));
                if (rval) {
                   _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group,
                                            _BCM_MULTICAST_TYPE_L2,
                                            l2addr->l2mc_group);
                }
            }
#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3 */
        } else {
            l2addr->l2mc_group = l2mc_index;
            /* Translate l2mc index */
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit, bcmSwitchL2McIdxRetType, &rval));
            if (rval) {
               _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group, _BCM_MULTICAST_TYPE_L2,
                                                                    l2addr->l2mc_group);
            }
        }
        }
    } else {
        _bcm_gport_dest_t       dest;
        int                     dest_type_is_trunk = 0;
        int                     isGport = 0;
#if defined(INCLUDE_L3)
        int                     dest_type_is_dvp = 0;
        uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
#endif
        uint32 dest_value = 0;
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_L3)
        int                     dest_type_is_roe_nhi = 0;
        uint32                  dest_nhi = 0;
        int                     nh_roe_idx = -1;
#endif /* BCM_MONTEREY_SUPPORT && INCLUDE_L3 */

        _bcm_gport_dest_t_init(&dest);
#if defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_generic_dest)) {
            dest_value = soc_mem_field32_dest_get(unit, L2Xm, l2x_entry, 
                            DESTINATIONf, &dest_type);
            if (dest_type == SOC_MEM_FIF_DEST_DVP) {
                dest_type_is_dvp = 1;
            } else if (dest_type == SOC_MEM_FIF_DEST_LAG) {
                dest_type_is_trunk = 1;
            }
        } else
#endif /* INCLUDE_L3 */
        {
#if defined(INCLUDE_L3)
            if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry,
                                            DEST_TYPE_f) == 2) {
                dest_type_is_dvp = 1;
            }
#endif /* INCLUDE_L3 */

            dest_type_is_trunk =
                _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, T_f);
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_L3)
            if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, DEST_TYPE_f) == 1) {
                dest_nhi = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, DESTINATION_f);
                if ((dest_nhi & _BCM_L2_DEST_TYPE_MASK) == _BCM_L2_DEST_TYPE_NHI) {
                    dest_type_is_roe_nhi = 1;
                    dest_type_is_trunk = 0;
                }
            }
#endif
        }

#if defined(INCLUDE_L3)
        if (dest_type_is_dvp) {
            if (soc_feature(unit, soc_feature_generic_dest)) {
                vp = dest_value;
            } else {
            vp = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, DESTINATION_f);
            }
            if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, KEY_TYPE_f) ==
                    TR_L2_HASH_KEY_TYPE_VFI) {
                /* MPLS/MiM virtual port unicast */
                if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                    dest.mpls_id = vp;
                    dest.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
                    isGport=1;
                } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                    dest.mim_id = vp;
                    dest.gport_type = _SHR_GPORT_TYPE_MIM_PORT;
                    isGport=1;
                } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
                    dest.vxlan_id  = vp;
                    dest.gport_type = _SHR_GPORT_TYPE_VXLAN_PORT;
                    isGport=1;
                } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
                    dest.l2gre_id  = vp;
                    dest.gport_type = _SHR_GPORT_TYPE_L2GRE_PORT;
                    isGport=1;
                } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
                    dest.flow_id  = vp;
                    dest.gport_type = _SHR_GPORT_TYPE_FLOW_PORT;
                    isGport=1;
                } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVpLag)) {
                    l2addr->flags |= BCM_L2_TRUNK_MEMBER;
                    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_vp_lag_vp_to_tid(unit,
                                vp, &l2addr->tgid));
                    dest.tgid = l2addr->tgid;
                    dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                                bcmSwitchUseGport, &isGport));
                } else {
                    /* L2 entries with Stale VPN */
                    dest.gport_type = BCM_GPORT_INVALID;
                    isGport=0;
                }
            } else {
                /* Subport/WLAN unicast */
#if defined(BCM_SCORPION_SUPPORT) && defined(INCLUDE_L3)
                if (SOC_IS_SC_CQ(unit)) {
                    /* Scorpion uses index to L3_NEXT_HOP as VPG */
                    int grp;

                    _SC_SUBPORT_VPG_FIND(unit, vp, grp);
                    if ((vp = grp) == -1) {
                        LOG_ERROR(BSL_LS_BCM_L2,
                                  (BSL_META_U(unit,
                                              "Can not find entry for VPG\n")));
                    }
                    dest.subport_id = vp;
                    dest.gport_type = _SHR_GPORT_TYPE_SUBPORT_GROUP;
                    isGport=1;
                } else
#endif /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */
                {
                    if (_bcm_vp_used_get(unit, vp, _bcmVpTypeSubport)) {
                        dest.subport_id = vp;
                        dest.gport_type = _SHR_GPORT_TYPE_SUBPORT_GROUP;
                        isGport=1;
                    } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
                        dest.wlan_id = vp;
                        dest.gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
                        isGport=1;
                    } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                        dest.vlan_vp_id = vp;
                        dest.gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
                        isGport=1;
                    } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
                        dest.niv_id = vp;
                        dest.gport_type = _SHR_GPORT_TYPE_NIV_PORT;
                        isGport=1;
                    } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeTrill)) {
                        dest.trill_id  = vp;
                        dest.gport_type = _SHR_GPORT_TYPE_TRILL_PORT;
                        isGport=1;
                    } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                        dest.extender_id = vp;
                        dest.gport_type = _SHR_GPORT_TYPE_EXTENDER_PORT;
                        isGport=1;
                    } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVpLag)) {
                        l2addr->flags |= BCM_L2_TRUNK_MEMBER;
                        BCM_IF_ERROR_RETURN(_bcm_esw_trunk_vp_lag_vp_to_tid(unit,
                                    vp, &l2addr->tgid));
                        dest.tgid = l2addr->tgid;
                        dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                        BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                                    bcmSwitchUseGport, &isGport));
                    } else {
                        /* L2 entries with Stale Gport */
                        dest.gport_type = BCM_GPORT_INVALID;
                        isGport=0;
                    }
                }
            }
        } else
#if defined(BCM_GREYHOUND2_SUPPORT)
        if (((dest_type_is_nhi == 0x4) || (dest_type_is_nhi == 0x5)) &&
            (soc_feature(unit, soc_feature_vxlan_lite))) {
            if (dest_type_is_nhi == 0x5) {
                ecmp = 1; /* ECMP */
            } else {
                ecmp = 0; /* NHI */
            }

            nh_ecmp_index = soc_L2Xm_field32_get(unit, l2x_entry, NHI_ECMPf);

            BCM_IF_ERROR_RETURN(
                bcmi_gh2_vxlan_port_from_nh_ecmp_index(unit, ecmp,
                                                       nh_ecmp_index, &vp));
            dest.vxlan_id = vp;
            dest.gport_type = _SHR_GPORT_TYPE_VXLAN_PORT;
            isGport = 1;
        } else
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_L3)
        if (dest_type_is_roe_nhi) {
            l2addr->flags2 |= BCM_L2_FLAGS2_ROE_NHI;
            nh_roe_idx = (dest_nhi & (~_BCM_L2_DEST_TYPE_MASK));
            l2addr->egress_if = BCM_XGS3_EGRESS_IDX_MIN(unit) + nh_roe_idx;
        } else
#endif /* BCM_MONTEREY_SUPPORT && INCLUDE_L3 */
#endif /* INCLUDE_L3 */
        if (dest_type_is_trunk) {
            /* Trunk group */
            int psc = 0; /* psc is dummy, not used */
            l2addr->flags |= BCM_L2_TRUNK_MEMBER;
            if (soc_feature(unit, soc_feature_generic_dest)) {
                l2addr->tgid = dest_value & SOC_MEM_FIF_DGPP_TGID_MASK;
            } else {
            l2addr->tgid = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, TGID_f);
            if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, REMOTE_TRUNK_f)) {
                l2addr->flags |= BCM_L2_REMOTE_TRUNK;
            }
            }

            bcm_esw_trunk_psc_get(unit, l2addr->tgid, &psc);
            dest.tgid = l2addr->tgid;
            dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
#if defined(BCM_TSN_SR_SUPPORT)
        } else if (soc_feature(unit, soc_feature_tsn_sr) &&
            soc_L2Xm_field32_get(unit, l2x_entry, SR_DUPLICATE_FORWARDINGf)) {
            /*
             * Indicates that the destination is a
             * Seamless Redundancy Duplicate port and Unicast packets must be
             * forwarded as Multicast (DEST_TYPE=L2MC)
             */
            l2addr->flags |= BCM_L2_MCAST;
            l2mc_index = _BCM_L2X_MEMACC_FIELD32_GET(unit,
                                                     l2x_entry, L2MC_PTR_f);

            l2addr->l2mc_group = l2mc_index;
            /* Translate l2mc index */
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit,
                                           bcmSwitchL2McIdxRetType, &rval));
            if (rval) {
                _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group,
                                         _BCM_MULTICAST_TYPE_L2,
                                         l2addr->l2mc_group);
            }
#endif /* BCM_TSN_SR_SUPPORT */
        } else {
            bcm_module_t    mod_in, mod_out;
            bcm_port_t      port_in, port_out;

            if (soc_feature(unit, soc_feature_generic_dest)) {
                port_in = dest_value & SOC_MEM_FIF_DGPP_PORT_MASK;
                mod_in = (dest_value & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                            SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
            } else {
            port_in = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, PORT_NUM_f);
            mod_in  = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, MODULE_ID_f);
            }
            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                        mod_in, port_in, &mod_out, &port_out));
            l2addr->modid = mod_out;
            l2addr->port = port_out;
            dest.port = l2addr->port;
            dest.modid = l2addr->modid;
            dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
        }

        if (isGport) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_construct(unit, &dest, &(l2addr->port)));
        }
    }

    if (_BCM_L2X_MEMACC_FIELD_VALID(unit, L3_f)) {
        if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, L3_f)) {
            l2addr->flags |= BCM_L2_L3LOOKUP;
        }
    }

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        l2addr->group = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry,
                                                    CLASS_ID_f);
    } else {
        mb_index = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, 
                                               MAC_BLOCK_INDEX_f);
        if (mb_index) {
            BCM_PBMP_ASSIGN(l2addr->block_bitmap,
                            _mbi_entries[unit][mb_index].mb_pbmp);
        }
    }

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, RPE_f)) {
        l2addr->flags |= BCM_L2_SETPRI;
    }

    l2addr->cos_dst = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, PRI_f);
    l2addr->cos_src = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, PRI_f);

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        if (soc_feature(unit, soc_feature_overlaid_address_class) &&
            !BCM_L2_PRE_SET(l2addr->flags)) {
            l2addr->group |= (l2addr->cos_dst & 0xF) << 6;
        }
    } 

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, CPU_f)) {
        l2addr->flags |= BCM_L2_COPY_TO_CPU;
    }

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, DST_DISCARD_f)) {
        l2addr->flags |= BCM_L2_DISCARD_DST;
    }

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, SRC_DISCARD_f)) {
        l2addr->flags |= BCM_L2_DISCARD_SRC;
    }

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, SCP_f)) {
        l2addr->flags |= BCM_L2_COS_SRC_PRI;
    }


    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, STATIC_BIT_f)) {
        l2addr->flags |= BCM_L2_STATIC;
    }

    if (_BCM_L2X_MEMACC_FIELD_VALID(unit, LIMIT_COUNTED_f)) {
        if (!(l2addr->flags & (BCM_L2_L3LOOKUP | BCM_L2_MCAST |
                               BCM_L2_STATIC | BCM_L2_LEARN_LIMIT))) {
            if (!_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry,
                                             LIMIT_COUNTED_f)) {
                l2addr->flags |= BCM_L2_LEARN_LIMIT_EXEMPT;
            }
        }
    }

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, PENDING_f)) {
        l2addr->flags |= BCM_L2_PENDING;
    }

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, HITDA_f)) {
        l2addr->flags |= BCM_L2_DES_HIT;
        l2addr->flags |= BCM_L2_HIT;
    }

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, HITSA_f)) {
        l2addr->flags |= BCM_L2_SRC_HIT;
        l2addr->flags |= BCM_L2_HIT;
    }

    if (_BCM_L2X_MEMACC_FIELD_VALID(unit, LOCAL_SA_f)) {
        if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, LOCAL_SA_f)) {
            l2addr->flags |= BCM_L2_NATIVE;
        }
    }

    if (soc_feature(unit, soc_feature_mac_learn_limit)) {
        if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, REMOTE_f)) {
            l2addr->flags |= BCM_L2_LEARN_LIMIT_EXEMPT_LOCAL;
        }
    }

#ifdef CANCUN_SUPPORT
    if (soc_feature(unit, soc_feature_l2x_gbp_support) &&
        soc_feature(unit, soc_feature_cancun) &&
        SOC_MEM_FIELD_VALID(unit, L2Xm, OPAQUE_USAGE_TYPEf) &&
        SOC_MEM_FIELD_VALID(unit, L2Xm, GBP_SIDf) &&
        SOC_MEM_FIELD_VALID(unit, L2Xm, OPAQUE_OBJECT_IDf)) {

        if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, OPAQUE_USAGE_TYPE_f) == 1) {
            l2addr->gbp_src_id =
                _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, GBP_SID_f);
        }
    }
#endif

    if (SOC_MEM_FIELD_VALID(unit, L2Xm, MESHf)) {
        if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, MESH_f)) {
            l2addr->flags2 |= BCM_L2_FLAGS2_MESH;
        }
    } else if (SOC_MEM_FIELD_VALID(unit, L2Xm, EVPN_AGE_DISABLEf)) {
        if (soc_L2Xm_field32_get(unit, l2x_entry, EVPN_AGE_DISABLEf)) {
            l2addr->flags2 |= BCM_L2_FLAGS2_MESH;
        }
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    my_station_config = soc_property_get(unit, spn_L2_ENTRY_USED_AS_MY_STATION, 0);
    if (soc_feature(unit, soc_feature_l2_entry_used_as_my_station)
        && my_station_config) {
#if defined(BCM_TRIDENT3_SUPPORT)
         if (soc_feature(unit, soc_feature_mysta_profile)) {
              bcm_l2_station_t station;
              uint32 mysta_prof_id = 0;
              uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
              my_station_profile_2_entry_t entry_prof;

              sal_memset(&station, 0, sizeof(bcm_l2_station_t));
              mysta_prof_id = soc_mem_field32_dest_get(unit, L2Xm, l2x_entry,
                              DESTINATIONf, &dest_type);
              if (dest_type == SOC_MEM_FIF_DEST_MYSTA) {
                  BCM_IF_ERROR_RETURN(_bcm_l2_mysta_profile_entry_get(unit,
                                      &entry_prof, 1, mysta_prof_id));
                  _bcm_l2_mysta_entry_to_station(unit, &entry_prof, MY_STATION_PROFILE_2m, &station);
                  l2addr->station_flags = station.flags;
              } else {
                  l2addr->station_flags = 0;
              }
          } else
#endif
         {
            if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, DEST_TYPE_f) == 1) {
                uint32 dst = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry,
                                                         DESTINATION_f);

        if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, DESTINATION_f)
            & _BCM_L2_DEST_STATION_IPV4) {
            l2addr->station_flags |= BCM_L2_STATION_IPV4;
        }

        if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, DESTINATION_f)
            & _BCM_L2_DEST_STATION_IPV6) {
            l2addr->station_flags |= BCM_L2_STATION_IPV6;
        }

        if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, DESTINATION_f)
            & _BCM_L2_DEST_STATION_ARP_RARP) {
            l2addr->station_flags |= BCM_L2_STATION_ARP_RARP;
        }

        if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, DESTINATION_f)
            & _BCM_L2_DEST_STATION_OAM) {
            l2addr->station_flags |= BCM_L2_STATION_OAM;
        }

        if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, DESTINATION_f)
            & _BCM_L2_DEST_STATION_FCOE) {
            l2addr->station_flags |= BCM_L2_STATION_FCOE;
        }

        if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, DESTINATION_f)
            & _BCM_L2_DEST_STATION_IPV4_MCAST) {
            l2addr->station_flags |= BCM_L2_STATION_IPV4_MCAST;
        }

        if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, DESTINATION_f)
            & _BCM_L2_DEST_STATION_IPV6_MCAST) {
            l2addr->station_flags |= BCM_L2_STATION_IPV6_MCAST;
        }

                if ((dst & _BCM_L2_DEST_TYPE_MASK) != _BCM_L2_DEST_TYPE_TRUNK) {
                    l2addr->flags &= (~BCM_L2_TRUNK_MEMBER);
                    l2addr->tgid = 0;
                }
            }
        }
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        /*
         * Provides TSN circuit ID for this packet:
         *  - 0 = Invalid TSN_CIRCUIT_ID, Valid only for MAC-DA lookup
         */
        hw_id = soc_L2Xm_field32_get(unit, l2x_entry, TSN_CIRCUIT_IDf);
        if (hw_id != 0) {
            /* Convert hardware TSN flow ID to software TSN flow ID */
            BCM_IF_ERROR_RETURN(
                bcmi_esw_tsn_sw_flow_id_get(unit, hw_id, &tsn_flow_id));

            /* Convert software TSN flow ID to flowset */
            BCM_IF_ERROR_RETURN(
                bcmi_esw_tsn_flowset_identify(unit, tsn_flow_id,
                                              &l2addr->tsn_flowset));
        }
    }

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        /* Indicates this MAC address belongs to DAN or SAN.(0: DAN, 1: SAN) */
        if (soc_L2Xm_field32_get(unit, l2x_entry, SANf)) {
            l2addr->flags |= BCM_L2_SR_SAN_DEST;
        }

        /*
         * Provides the Flow ID for Seamless Redundancy:
         * - SR_Flow_ID=0 is not valid. Valid for DA or SA lookup depending on
         *   PORT.SR_FLOW_ID_SOURCE_PRIORITY.
         */
        hw_id = soc_L2Xm_field32_get(unit, l2x_entry, SR_FLOW_IDf);
        if (hw_id != 0) {
            /* Convert hardware SR flow ID to software SR flow ID */
            BCM_IF_ERROR_RETURN(
                bcmi_esw_tsn_sr_sw_flow_id_get(unit, hw_id, &sr_flow_id));

            /* Convert software SR flow ID to flowset */
            BCM_IF_ERROR_RETURN(
                bcmi_esw_tsn_sr_flowset_identify(unit, sr_flow_id,
                                                 &l2addr->sr_flowset));
        }

        /* Provides the Source port filter bitmap for this SA */
        mp_index = soc_L2Xm_field32_get(unit, l2x_entry,
                                        SR_MAC_PROXY_PROFILE_PTRf);
        if (mp_index != 0) {
            BCM_PBMP_CLEAR(mp_pbmp);
            BCM_IF_ERROR_RETURN(
                bcmi_esw_tsn_sr_mac_proxy_get(unit, mp_index, &mp_pbmp));
            BCM_PBMP_NEGATE(l2addr->sa_source_filter_pbmp, mp_pbmp);
            BCM_PBMP_AND(l2addr->sa_source_filter_pbmp, PBMP_ALL(unit));
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */

    if (soc_feature(unit, soc_feature_tsn_taf)) {
        uint32 hw_profile_id = soc_L2Xm_field32_get(unit, l2x_entry,
                                                    TAF_GATE_ID_PROFILEf);
        if (0 == hw_profile_id) {
            l2addr->taf_gate_primap = BCM_TSN_PRI_MAP_INVALID;
        } else {
            bcm_tsn_pri_map_t sw_index;

            BCM_IF_ERROR_RETURN(
                bcmi_esw_tsn_pri_map_map_id_get(unit, bcmTsnPriMapTypeTafGate,
                                                hw_profile_id, &sw_index));
            l2addr->taf_gate_primap = sw_index;
        }
    } else
#endif /* BCM_TSN_SUPPORT */
    {
        l2addr->taf_gate_primap = BCM_TSN_PRI_MAP_INVALID;
    }

#if defined(BCM_GLOBAL_METER_SUPPORT)
    /* Global Meter Policer Configuration */
    if (soc_feature(unit, soc_feature_global_meter) &&
        soc_feature(unit, soc_feature_global_meter_source_l2)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_get_policer_from_table(
                unit, L2Xm, 0, (void *)l2x_entry, &l2addr->policer_id, TRUE));
    }
#endif /* BCM_GLOBAL_METER_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_l2_to_l2x
 * Purpose:
 *      Convert an L2 API data structure to a Triumph L2X entry
 * Parameters:
 *      unit        Unit number
 *      l2x_entry   (OUT) Triumph L2X entry
 *      l2addr      L2 API data structure
 *      key_only    Only construct key portion
 */
int
_bcm_tr_l2_to_l2x(int unit, l2x_entry_t *l2x_entry, bcm_l2_addr_t *l2addr,
                  int key_only)
{
    int rv = BCM_E_NONE;
    int vfi;
    int tid_is_vp_lag;
    bcm_cos_t cos_dst = l2addr->cos_dst;
    int group = l2addr->group;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    int my_station_config = 0;
    uint32 dest_value = 0;
#endif
    int pri_field_len;
    int max_pri;
#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
    int vp = -1;
    int ecmp = 0, nh_ecmp_index = 0;
    bcm_vxlan_port_t vxlan_port;
    int field_len = 0;
    vfi_entry_t vfi_entry;
#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_L3)
    uint32 mpath_flag = 0;
    int nh_index = 0;
    bcm_if_t egr_intf = -1;
    int l3mc_index = -1;
    uint32 egr_dest = 0;
#endif /* BCM_MONTEREY_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TSN_SUPPORT)
    uint32 hw_id = 0;
    bcm_tsn_pri_map_t pri_map = BCM_TSN_PRI_MAP_INVALID;
    bcm_tsn_flow_t tsn_flow_id = 0;
    bcm_tsn_flow_config_t tsn_config;
    bcm_tsn_pri_map_type_t map_type;
#if defined(BCM_TSN_SR_SUPPORT)
    bcm_tsn_sr_flow_t sr_flow_id = 0;
    bcm_tsn_sr_tx_flow_config_t sr_tx_config;
    bcm_tsn_sr_rx_flow_config_t sr_rx_config;
    l2mc_entry_t l2mc_entry;
    int l2mc_index;
    bcm_pbmp_t l2mc_pbmp;
    bcm_port_t l2mc_port;
    sr_port_table_entry_t sr_port_entry;
    sr_lport_tab_entry_t sr_lport_entry;
#endif /* BCM_TSN_SR_SUPPORT */
#endif /* BCM_TSN_SUPPORT */

    pri_field_len = soc_mem_field_length(unit, L2Xm, PRIf);
    max_pri = (1 << pri_field_len) - 1;

    /* Allow the maximum priority as per the device table support */
    if ((l2addr->cos_dst < BCM_PRIO_MIN) || (l2addr->cos_dst > max_pri)) {
        return BCM_E_PARAM;
    }

    /*  BCM_L2_MIRROR is not supported starting from Triumph */
    if (l2addr->flags & BCM_L2_MIRROR) {
        return BCM_E_PARAM;
    }

    /* Setting both flags is not a valid configuration. */
    if (l2addr->flags & BCM_L2_LEARN_LIMIT_EXEMPT && 
        l2addr->flags & BCM_L2_LEARN_LIMIT_EXEMPT_LOCAL) {
        return BCM_E_PARAM;
    }

    sal_memset(l2x_entry, 0, sizeof (*l2x_entry));

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow) &&
        (l2addr->flow_handle != 0)) {

        return _bcm_td3_flex_l2_to_l2x(unit, l2x_entry, l2addr, key_only);
    }
#endif

    if (_BCM_VPN_VFI_IS_SET(l2addr->vid)) {
        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, l2addr->vid);

#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_mem_is_valid(unit, VFI_ATTRS_1m)) {
            if (vfi > soc_mem_index_max(unit, VFI_ATTRS_1m)) {
                return BCM_E_PARAM;
            }
        }
#endif
        soc_L2Xm_field32_set(unit, l2x_entry, VFIf, vfi);
        soc_L2Xm_field32_set(unit, l2x_entry, KEY_TYPEf,
                             TR_L2_HASH_KEY_TYPE_VFI);
    } else {
         if (!_BCM_MPLS_VPN_IS_VPWS(l2addr->vid)) {
              VLAN_CHK_ID(unit, l2addr->vid);
              soc_L2Xm_field32_set(unit, l2x_entry, VLAN_IDf, l2addr->vid);
              soc_L2Xm_field32_set(unit, l2x_entry, KEY_TYPEf,
                                   TR_L2_HASH_KEY_TYPE_BRIDGE);
#ifdef BCM_TRIDENT3_SUPPORT
              if (soc_feature(unit, soc_feature_pvlan_on_vfi)) {
                  if (l2addr->flags2 & BCM_L2_FLAGS2_ADD_VPN_TYPE) {
                      soc_L2Xm_field32_set(unit, l2x_entry, KEY_TYPEf,
                                           TR_L2_HASH_KEY_TYPE_VFI);
                  }
              }
#endif
         }
    }

    soc_L2Xm_mac_addr_set(unit, l2x_entry, MAC_ADDRf, l2addr->mac);

    if (key_only) {
        return BCM_E_NONE;
    }

    if (BCM_MAC_IS_MCAST(l2addr->mac)) {
#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
        if ((BCM_GPORT_IS_VXLAN_PORT(l2addr->port)) &&
            (soc_feature(unit, soc_feature_vxlan_lite))) {
            vp = BCM_GPORT_VXLAN_PORT_ID_GET(l2addr->port);
            if (vp == -1) {
                return BCM_E_PARAM;
            }

            BCM_IF_ERROR_RETURN(
                bcmi_gh2_vxlan_port_to_nh_ecmp_index(unit, vp,
                                                     &ecmp, &nh_ecmp_index));
            if (ecmp) {
                /* ECMP */
                soc_L2Xm_field32_set(unit, l2x_entry, DEST_TYPEf, 0x5);
            } else {
                /* NHI */
                soc_L2Xm_field32_set(unit, l2x_entry, DEST_TYPEf, 0x4);
            }

            soc_L2Xm_field32_set(unit, l2x_entry, NHI_ECMPf, nh_ecmp_index);
        } else
#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_L3)
        if ((soc_feature(unit,soc_feature_roe) ||
             soc_feature(unit,soc_feature_roe_custom)) &&
            (l2addr->flags2 & BCM_L2_FLAGS2_L3_MULTICAST)) {

            if (_BCM_MULTICAST_IS_SET(l2addr->l3mc_group)) {
                if (_BCM_MULTICAST_IS_L3(l2addr->l3mc_group)) {
                    l3mc_index = _BCM_MULTICAST_ID_GET(l2addr->l3mc_group);
                } else {
                    return BCM_E_PARAM;
                }
            } else {
                return BCM_E_PARAM;
            }

            if (-1 != l3mc_index) {
                soc_L2Xm_field32_set(unit, l2x_entry, DEST_TYPEf, 0x3);
                soc_L2Xm_field32_set(unit, l2x_entry, DESTINATIONf, l3mc_index);
            }
        } else
#endif /* BCM_MONTEREY_SUPPORT && INCLUDE_L3 */
        {
            if (l2addr->l2mc_group < 0) {
                return BCM_E_PARAM;
            }

            if (soc_feature(unit, soc_feature_generic_dest)) {
                uint32 dest_type = SOC_MEM_FIF_DEST_L2MC;
                if (_BCM_MULTICAST_IS_VPLS(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_MIM(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_WLAN(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_VLAN(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_NIV(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_L2GRE(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_VXLAN(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_FLOW(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_SUBPORT(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_EXTENDER(l2addr->l2mc_group)) {
                    dest_type = SOC_MEM_FIF_DEST_IPMC;
                }
                soc_mem_field32_dest_set(unit, L2Xm, l2x_entry, DESTINATIONf,
                    dest_type, _BCM_MULTICAST_ID_GET(l2addr->l2mc_group));
            } else {
                if (_BCM_MULTICAST_IS_VPLS(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_MIM(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_WLAN(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_VLAN(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_NIV(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_L2GRE(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_VXLAN(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_FLOW(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_SUBPORT(l2addr->l2mc_group) ||
                    _BCM_MULTICAST_IS_EXTENDER(l2addr->l2mc_group)) {
                    soc_L2Xm_field32_set(unit, l2x_entry, DEST_TYPEf, 0x3);
#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
                    /* Only support for GH2-B0 */
                    if (soc_feature(unit, soc_feature_vxlan_lite_riot) ||
                        SOC_IS_FIRELIGHT(unit)) {
                        if (_BCM_MULTICAST_IS_VXLAN(l2addr->l2mc_group) &&
                            _BCM_VPN_VFI_IS_SET(l2addr->vid)) {
                            _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, l2addr->vid);
                            SOC_IF_ERROR_RETURN(
                                soc_mem_read(unit, VFIm, MEM_BLOCK_ANY,
                                             vfi, &vfi_entry));
                            /*
                             * For Multicast ENCAP flow, the KEY_TYPEf = 0x3.
                             * - to specify VFI is only MAC based forwarding.
                             * - IPMCV4_L2_ENABLE = 1 && IPMCV6_L2_ENABLE = 1.
                             * For Multicast DECAP flow, the KEY_TYPEf = 0x7.
                             * - IPMCV4_L2_ENABLE = 0 && IPMCV6_L2_ENABLE = 0.
                             */
                            if (!(soc_VFIm_field32_get(unit, &vfi_entry,
                                                       IPMCV4_L2_ENABLEf)) &&
                                !(soc_VFIm_field32_get(unit, &vfi_entry,
                                                       IPMCV6_L2_ENABLEf))) {
                                soc_L2Xm_field32_set
                                    (unit, l2x_entry, KEY_TYPEf,
                                     GH2_L2_HASH_KEY_TYPE_VFI_MULTICAST);
                            }
                        }
                    }
#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3  */
                }
#if defined(BCM_GREYHOUND2_SUPPORT)
                else if ((soc_feature(unit, soc_feature_vxlan_lite)) &&
                        (_BCM_VPN_VFI_IS_SET(l2addr->vid))) {
                    /*
                     * GH2 VXLAN-Lite doesn't support
                     * DEST_TYPE = 0 (L2MC) for VXLAN-based (VFI) keys.
                     */
                    return BCM_E_PARAM;
                }
#endif /* BCM_GREYHOUND2_SUPPORT */

                if (_BCM_MULTICAST_IS_SET(l2addr->l2mc_group)) {
                    soc_L2Xm_field32_set
                        (unit, l2x_entry, DESTINATIONf,
                         _BCM_MULTICAST_ID_GET(l2addr->l2mc_group));
                } else {
                        soc_L2Xm_field32_set(unit, l2x_entry, L2MC_PTRf,
                                             l2addr->l2mc_group);
                }
            }
        }
#if defined(BCM_TSN_SR_SUPPORT)
    } else if (soc_feature(unit, soc_feature_tsn_sr) &&
        (l2addr->flags & BCM_L2_MCAST)) {
        int sr_lport_idx;
        bcm_module_t my_modid = -1;
        int src_trk_idx = 0; /* Source Trunk table index.*/
        uint32 mem_entry[SOC_MAX_MEM_WORDS];

        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &my_modid));

        /*
         * Indicates that the destination is a
         * Seamless Redundancy Duplicate port and Unicast packets must be
         * forwarded as Multicast (DEST_TYPE=L2MC)
         */
        if (l2addr->l2mc_group < 0) {
            return BCM_E_PARAM;
        }

        if (_BCM_MULTICAST_IS_SET(l2addr->l2mc_group)) {
            if (_BCM_MULTICAST_IS_L2(l2addr->l2mc_group)) {
                l2mc_index = _BCM_MULTICAST_ID_GET(l2addr->l2mc_group);
            } else {
                return BCM_E_PARAM;
            }
        } else {
            l2mc_index = l2addr->l2mc_group;
        }
        soc_L2Xm_field32_set(unit, l2x_entry,
                             L2MC_PTRf, l2mc_index);
        soc_L2Xm_field32_set(unit, l2x_entry,
                             SR_DUPLICATE_FORWARDINGf, 0x1);

        /* Update l2mc_index for related SR ports associated L2MC table */
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, L2MCm, MEM_BLOCK_ANY,
                         l2mc_index, &l2mc_entry));
        BCM_PBMP_CLEAR(l2mc_pbmp);
        soc_mem_pbmp_field_get(unit, L2MCm, &l2mc_entry,
                               PORT_BITMAPf, &l2mc_pbmp);
        BCM_PBMP_ITER(l2mc_pbmp, l2mc_port) {
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, SR_PORT_TABLEm, MEM_BLOCK_ANY,
                             l2mc_port, &sr_port_entry));
            soc_mem_field32_set(unit, SR_PORT_TABLEm, &sr_port_entry,
                                L2MCf, l2mc_index);
            BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, SR_PORT_TABLEm, MEM_BLOCK_ALL,
                              l2mc_port, &sr_port_entry));

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
            soc_mem_field32_set(unit, SR_LPORT_TABm, &sr_lport_entry,
                                L2MCf, l2mc_index);
            BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, SR_LPORT_TABm, MEM_BLOCK_ALL,
                              sr_lport_idx, &sr_lport_entry));
        }
#endif /* BCM_TSN_SR_SUPPORT */
    } else {
        bcm_port_t      port = -1;
        bcm_trunk_t     tgid = BCM_TRUNK_INVALID;
        bcm_module_t    modid = -1;
        int             gport_id = -1;
        int             vpg_type = 0;

        if (BCM_GPORT_IS_SET(l2addr->port)) {
            _bcm_l2_gport_params_t  g_params;

            if (BCM_GPORT_IS_BLACK_HOLE(l2addr->port)) {
                soc_L2Xm_field32_set(unit, l2x_entry, SRC_DISCARDf, 1);
#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
            } else if ((BCM_GPORT_IS_VXLAN_PORT(l2addr->port)) &&
                       (soc_feature(unit, soc_feature_vxlan_lite))) {
                soc_L2Xm_field32_set(unit, l2x_entry, SRC_DISCARDf, 0);

                vp = BCM_GPORT_VXLAN_PORT_ID_GET(l2addr->port);
                if (vp == -1) {
                    return BCM_E_PARAM;
                }
#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3 */
            } else {
                soc_L2Xm_field32_set(unit, l2x_entry, SRC_DISCARDf, 0);
                BCM_IF_ERROR_RETURN(
                        _bcm_esw_l2_gport_parse(unit, l2addr, &g_params));

                switch (g_params.type) {
                    case _SHR_GPORT_TYPE_TRUNK:
                        tgid = g_params.param0;
                        break;
                    case  _SHR_GPORT_TYPE_MODPORT:
                        port = g_params.param0;
                        modid = g_params.param1;
                        break;
                    case _SHR_GPORT_TYPE_LOCAL_CPU:
                        port = g_params.param0;
                        BCM_IF_ERROR_RETURN(
                                bcm_esw_stk_my_modid_get(unit, &modid));
                        break;
                    case _SHR_GPORT_TYPE_SUBPORT_GROUP:
                        gport_id = g_params.param0;
#if defined(BCM_SCORPION_SUPPORT) && defined(INCLUDE_L3)
                        if (SOC_IS_SC_CQ(unit)) {
                            /* Map the sub_port to index to L3_NEXT_HOP */
                            gport_id = (int) _sc_subport_group_index[unit][gport_id/8];
                        }
#endif /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */
                        vpg_type = 1;
                        break;
                case _SHR_GPORT_TYPE_SUBPORT_PORT:

#if defined(BCM_HGPROXY_COE_SUPPORT)
                    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                        (soc_feature(unit, soc_feature_channelized_switching))) &&
                        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, l2addr->port)) {
                        port = g_params.param0;
                        modid = g_params.param1;
                    } else
#endif
#if defined(BCM_KATANA2_SUPPORT)
                    if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit,
                        l2addr->port)) {
                        port = g_params.param0;
                        modid = g_params.param1;
                    } else
#endif
                    {
                        gport_id = g_params.param0;
                        vpg_type = 1;
                    }
                    break;
                    case _SHR_GPORT_TYPE_MPLS_PORT:
                    gport_id = g_params.param0;
                    vpg_type = 1;
                    break;
                    case _SHR_GPORT_TYPE_MIM_PORT:
                    case _SHR_GPORT_TYPE_WLAN_PORT:
                    case _SHR_GPORT_TYPE_VLAN_PORT:
                    case _SHR_GPORT_TYPE_NIV_PORT:
                    case _SHR_GPORT_TYPE_EXTENDER_PORT:
                        gport_id = g_params.param0;
                        vpg_type = 1;
                        break;
                    case _SHR_GPORT_TYPE_TRILL_PORT:
                    case _SHR_GPORT_TYPE_L2GRE_PORT:
                    case _SHR_GPORT_TYPE_VXLAN_PORT:
                    case _SHR_GPORT_TYPE_FLOW_PORT:

                        gport_id = g_params.param0;
                        vpg_type = 0;
                        break;
                    default:
                        return BCM_E_PORT;
                }
            }
        } else if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
            tgid = l2addr->tgid;
 #if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_L3)
        } else if ((soc_feature(unit,soc_feature_roe) ||
                    soc_feature(unit,soc_feature_roe_custom)) && 
                   (l2addr->flags2 & BCM_L2_FLAGS2_ROE_NHI)) {
            egr_intf = l2addr->egress_if;
 #endif /* BCM_MONTEREY_SUPPORT && INCLUDE_L3 */
        } else {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                       l2addr->modid, l2addr->port,
                                       &modid, &port));
            /* Check parameters */
            if (!SOC_MODID_ADDRESSABLE(unit, modid)) {
                return BCM_E_BADID;
            }
            if (!SOC_PORT_ADDRESSABLE(unit, port)) {
                return BCM_E_PORT;
            }
        }

        /* Setting l2x_entry fields according to parameters */
        if ( BCM_TRUNK_INVALID != tgid) {
            BCM_IF_ERROR_RETURN(_bcm_esw_trunk_id_is_vp_lag(unit, tgid,
                            &tid_is_vp_lag));
            if (tid_is_vp_lag) {
                if (soc_feature(unit, soc_feature_vp_lag)) {
                    int vp_lag_vp;
                    /* Get the VP value representing VP LAG */
                    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_tid_to_vp_lag_vp(unit,
                                tgid, &vp_lag_vp));
                    if (soc_feature(unit, soc_feature_generic_dest)) {
                        soc_mem_field32_dest_set(unit, L2Xm, l2x_entry,
                            DESTINATIONf, SOC_MEM_FIF_DEST_DVP, vp_lag_vp);
                    } else {
                    soc_L2Xm_field32_set(unit, l2x_entry, DEST_TYPEf, 0x2);
                    soc_L2Xm_field32_set(unit, l2x_entry, DESTINATIONf,
                            vp_lag_vp);
                    }
                } else {
                    return BCM_E_PORT;
                }
            } else {
                if (soc_feature(unit, soc_feature_generic_dest)) {
                    if (l2addr->flags & BCM_L2_REMOTE_TRUNK) {
                        tgid |= 1 << SOC_MEM_FIF_DGPP_RT_SHIFT_BITS;
                    }
                    soc_mem_field32_dest_set(unit, L2Xm, l2x_entry,
                        DESTINATIONf, SOC_MEM_FIF_DEST_LAG, tgid);
                } else {
                soc_L2Xm_field32_set(unit, l2x_entry, Tf, 1);
                soc_L2Xm_field32_set(unit, l2x_entry, TGIDf, tgid);
                /*
                 * Note:  RTAG is ignored here.  Use bcm_trunk_psc_set to
                 * to set for a given trunk.
                 */
                if (l2addr->flags & BCM_L2_REMOTE_TRUNK) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_APACHE_SUPPORT)
                    if (soc_feature(unit, soc_feature_no_l2_remote_trunk)) {
                    } else
#endif
                    {
                        soc_L2Xm_field32_set(unit, l2x_entry, REMOTE_TRUNKf, 1);
                    }
                }
            }
            }
        } else if (-1 != port) {
            if (soc_feature(unit, soc_feature_generic_dest)) {
                soc_mem_field32_dest_set(unit, L2Xm, l2x_entry,
                    DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                    modid << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | port);
            } else {
            soc_L2Xm_field32_set(unit, l2x_entry, MODULE_IDf, modid);
            soc_L2Xm_field32_set(unit, l2x_entry, PORT_NUMf, port);
            }
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_L3)
        } else if (-1 != egr_intf) {
                BCM_IF_ERROR_RETURN(
                    bcm_xgs3_get_nh_from_egress_object(unit, egr_intf, &mpath_flag,
                                                                0, &nh_index));
                soc_L2Xm_field32_set(unit, l2x_entry, DEST_TYPEf, 0x1);
                egr_dest = _BCM_L2_DEST_TYPE_NHI;
                egr_dest |= nh_index;
                soc_L2Xm_field32_set(unit, l2x_entry, DESTINATIONf, egr_dest);
#endif /* BCM_MONTEREY_SUPPORT && INCLUDE_L3 */
#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
        } else if (-1 != vp) {
            if (soc_feature(unit, soc_feature_vxlan_lite)) {
                BCM_IF_ERROR_RETURN(
                    bcmi_gh2_vxlan_port_to_nh_ecmp_index(unit, vp, &ecmp,
                                                         &nh_ecmp_index));
                if (ecmp) {
                    /* ECMP */
                    soc_L2Xm_field32_set(unit, l2x_entry, DEST_TYPEf, 0x5);
                } else {
                    /* NHI */
                    soc_L2Xm_field32_set(unit, l2x_entry, DEST_TYPEf, 0x4);
                }
                soc_L2Xm_field32_set(unit, l2x_entry,
                                     NHI_ECMPf, nh_ecmp_index);

                /* Only support for GH2-B0 */
                if (soc_feature(unit, soc_feature_vxlan_lite_riot) || SOC_IS_FIRELIGHT(unit)) {
                    bcm_vxlan_port_t_init(&vxlan_port);

                    vxlan_port.vxlan_port_id = l2addr->port;
                    BCM_IF_ERROR_RETURN(
                        bcm_esw_vxlan_port_get(unit, (bcm_vpn_t)l2addr->vid,
                                               &vxlan_port));

                    soc_L2Xm_field32_set(unit, l2x_entry,
                                         ASSOCIATED_DATA_TYPEf, 0x1);

                    /* Set SGLP */
                    if (soc_property_get(unit, spn_RIOT_ENABLE, 0)) {
                        BCM_IF_ERROR_RETURN(
                            _bcm_esw_gport_resolve(unit, vxlan_port.match_port,
                                                   &modid, &port, &tgid,
                                                   &gport_id));
                        if (BCM_GPORT_IS_TRUNK(vxlan_port.match_port) &&
                            tgid != BCM_TRUNK_INVALID) {
                            /* Set LAG ID indicator bit */
                            field_len = soc_mem_field_length(unit,
                                                             L2Xm, SGLPf);
                            gport_id = ((1 << (field_len - 1)) + tgid);
                        } else {
                            field_len = soc_mem_field_length(unit,
                                                             L2Xm, PORT_NUMf);
                            gport_id = ((modid << field_len) + port);
                        }
                        soc_L2Xm_field32_set(unit, l2x_entry, SGLPf, gport_id);
                    }
                }
            }
#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3 */
        } else if (-1 != gport_id) {
            if (soc_feature(unit, soc_feature_generic_dest)) {
                soc_mem_field32_dest_set(unit, L2Xm, l2x_entry,
                    DESTINATIONf, SOC_MEM_FIF_DEST_DVP, gport_id);
            } else {
            soc_L2Xm_field32_set(unit, l2x_entry, DEST_TYPEf, 0x2);
            if (vpg_type) {
                soc_L2Xm_field32_set(unit, l2x_entry, VPGf, gport_id);
                soc_L2Xm_field32_set(unit, l2x_entry, VPG_TYPEf, vpg_type);
            } else {
                soc_L2Xm_field32_set(unit, l2x_entry, DESTINATIONf, gport_id);
            }
        }
    }
    }

    if (SOC_MEM_FIELD_VALID(unit, L2Xm, L3f)) {
        if (l2addr->flags & BCM_L2_L3LOOKUP) {
            soc_L2Xm_field32_set(unit, l2x_entry, L3f, 1);
        }
    }

    if (SOC_L2X_GROUP_ENABLE_GET(unit)) {
        if (soc_feature(unit, soc_feature_overlaid_address_class) &&
            !BCM_L2_PRE_SET(l2addr->flags)) {
            if ((l2addr->group > SOC_OVERLAID_ADDR_CLASS_MAX(unit)) ||
                (l2addr->group < 0)) {
                return BCM_E_PARAM;
            }
            cos_dst = (l2addr->group & 0x3C0) >> 6;
            group = l2addr->group & 0x3F;
        }
    }

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        soc_L2Xm_field32_set(unit, l2x_entry, CLASS_IDf, group);
    } /* else MAC_BLOCK_INDEXf is handled in the add/remove functions below */

    if (l2addr->flags & BCM_L2_SETPRI) {
        soc_L2Xm_field32_set(unit, l2x_entry, RPEf, 1);
    }

    if (!SOC_IS_TRIUMPH3(unit)) {
        soc_L2Xm_field32_set(unit, l2x_entry, PRIf, cos_dst);
    }

    if (l2addr->flags & BCM_L2_COPY_TO_CPU) {
        soc_L2Xm_field32_set(unit, l2x_entry, CPUf, 1);
    }

    if (l2addr->flags & BCM_L2_DISCARD_DST) {
        soc_L2Xm_field32_set(unit, l2x_entry, DST_DISCARDf, 1);
    }

    if (l2addr->flags & BCM_L2_DISCARD_SRC) {
        soc_L2Xm_field32_set(unit, l2x_entry, SRC_DISCARDf, 1);
    }

    if (l2addr->flags & BCM_L2_COS_SRC_PRI) {
        soc_L2Xm_field32_set(unit, l2x_entry, SCPf, 1);
    }

    if (SOC_MEM_FIELD_VALID(unit, L2Xm, LIMIT_COUNTEDf)) {
        if ((!(l2addr->flags & (BCM_L2_L3LOOKUP | BCM_L2_MCAST | BCM_L2_STATIC |
                               BCM_L2_LEARN_LIMIT_EXEMPT))) || 
            l2addr->flags & BCM_L2_LEARN_LIMIT) {
            soc_L2Xm_field32_set(unit, l2x_entry, LIMIT_COUNTEDf, 1);
        }
    }

    if (l2addr->flags & BCM_L2_PENDING) {
        soc_L2Xm_field32_set(unit, l2x_entry, PENDINGf, 1);
    }

    if (l2addr->flags & BCM_L2_STATIC) {
        soc_L2Xm_field32_set(unit, l2x_entry, STATIC_BITf, 1);
#ifdef BCM_HURRICANE4_SUPPORT
        if (soc_feature(unit, soc_feature_hr4_l2x_static_bit_war)) {
            soc_L2Xm_field32_set(unit, l2x_entry, STATIC_DUPLICATEf, 1);
        }
#endif /* BCM_HURRICANE4_SUPPORT */
    }

    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_L2Xm_field32_set(unit, l2x_entry, BASE_VALIDf, 1);
    } else {
    soc_L2Xm_field32_set(unit, l2x_entry, VALIDf, 1);
    }

    if ((l2addr->flags & BCM_L2_DES_HIT) ||
        (l2addr->flags & BCM_L2_HIT)) {
        soc_L2Xm_field32_set(unit, l2x_entry, HITDAf, 1);
    }

    if ((l2addr->flags & BCM_L2_SRC_HIT) ||
        (l2addr->flags & BCM_L2_HIT)) {
        soc_L2Xm_field32_set(unit, l2x_entry, HITSAf, 1);
    }

    if (SOC_MEM_FIELD_VALID(unit, L2Xm, LOCAL_SAf)) {
        if (l2addr->flags & BCM_L2_NATIVE) {
            soc_L2Xm_field32_set(unit, l2x_entry, LOCAL_SAf, 1);
        }
    }

    
    if (l2addr->flags & BCM_L2_LEARN_LIMIT_EXEMPT_LOCAL) {
        if (!soc_feature(unit, soc_feature_mac_learn_limit)) {
            return BCM_E_PARAM;
        }
        soc_L2Xm_field32_set(unit, l2x_entry, REMOTEf, 1);
        soc_L2Xm_field32_set(unit, l2x_entry, LIMIT_COUNTEDf, 1);
    }

#ifdef CANCUN_SUPPORT
    if (soc_feature(unit, soc_feature_l2x_gbp_support) &&
        soc_feature(unit, soc_feature_cancun) &&
        SOC_MEM_FIELD_VALID(unit, L2Xm, OPAQUE_USAGE_TYPEf) &&
        SOC_MEM_FIELD_VALID(unit, L2Xm, GBP_SIDf) &&
        SOC_MEM_FIELD_VALID(unit, L2Xm, OPAQUE_OBJECT_IDf) &&
        l2addr->gbp_src_id != 0) {
        soc_L2Xm_field32_set(unit, l2x_entry, OPAQUE_USAGE_TYPEf, 1);
        soc_L2Xm_field32_set(unit, l2x_entry, OPAQUE_OBJECT_IDf, 1);
        soc_L2Xm_field32_set(unit, l2x_entry, GBP_SIDf, l2addr->gbp_src_id);
    }
#endif

    if (SOC_MEM_FIELD_VALID(unit, L2Xm, MESHf)) {
        if (l2addr->flags2 & BCM_L2_FLAGS2_MESH) {
            soc_L2Xm_field32_set(unit, l2x_entry, MESHf, 1);
        }
    } else if (SOC_MEM_FIELD_VALID(unit, L2Xm, EVPN_AGE_DISABLEf)) {
        if (l2addr->flags2 & BCM_L2_FLAGS2_MESH) {
            soc_L2Xm_field32_set(unit, l2x_entry, EVPN_AGE_DISABLEf, 1);
        }
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    my_station_config = soc_property_get(unit, spn_L2_ENTRY_USED_AS_MY_STATION, 0);
    if (soc_feature(unit, soc_feature_l2_entry_used_as_my_station)
        && my_station_config) {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            uint32 mysta_profile_id;
            bcm_l2_station_t station;
            my_station_profile_2_entry_t entry;
            sal_memset(&station, 0, sizeof(bcm_l2_station_t));
            sal_memset(&entry, 0, sizeof(entry));
            station.flags = l2addr->station_flags;
            if (l2addr->station_flags != 0) {
                _bcm_l2_mysta_station_to_entry(unit, &station, MY_STATION_PROFILE_2m, &entry);
                BCM_IF_ERROR_RETURN(
                    _bcm_l2_mysta_profile_entry_add(unit, &entry, 1,
                    &mysta_profile_id));
                soc_mem_field32_dest_set(unit, L2Xm, l2x_entry, DESTINATIONf,
                                         SOC_MEM_FIF_DEST_MYSTA, mysta_profile_id);
            }
        } else
#endif
        {
            /* Program DESTINATION only if station flags are specified */
            if (l2addr->station_flags != 0) {

                if ((l2addr->flags & BCM_L2_TRUNK_MEMBER) != 0) {
                    return BCM_E_UNAVAIL;
                }

        soc_L2Xm_field32_set(unit, l2x_entry, DEST_TYPEf, 1);

        dest_value = _BCM_L2_DEST_STATION_HIT;

        if (l2addr->station_flags & BCM_L2_STATION_IPV4) {
            dest_value |= _BCM_L2_DEST_STATION_IPV4;
        }

        if (l2addr->station_flags & BCM_L2_STATION_IPV6) {
            dest_value |= _BCM_L2_DEST_STATION_IPV6;
        } 

        if (l2addr->station_flags & BCM_L2_STATION_ARP_RARP) {
            dest_value |= _BCM_L2_DEST_STATION_ARP_RARP;
        }

        if (l2addr->station_flags & BCM_L2_STATION_OAM) {
            dest_value |= _BCM_L2_DEST_STATION_OAM;
        }

        if (l2addr->station_flags & BCM_L2_STATION_FCOE) {
             dest_value |= _BCM_L2_DEST_STATION_FCOE;
        }

        if (l2addr->station_flags & BCM_L2_STATION_IPV4_MCAST) {
            dest_value |= _BCM_L2_DEST_STATION_IPV4_MCAST;
        }

        if (l2addr->station_flags & BCM_L2_STATION_IPV6_MCAST) {
            dest_value |= _BCM_L2_DEST_STATION_IPV6_MCAST;
        }

        soc_L2Xm_field32_set(unit, l2x_entry, DESTINATIONf, dest_value);
        }
        }
    } else {
        if (l2addr->station_flags & BCM_L2_STATION_IPV4
           || l2addr->station_flags & BCM_L2_STATION_IPV6
           || l2addr->station_flags & BCM_L2_STATION_ARP_RARP
           || l2addr->station_flags & BCM_L2_STATION_OAM
           || l2addr->station_flags & BCM_L2_STATION_FCOE
           || l2addr->station_flags & BCM_L2_STATION_IPV4_MCAST
           || l2addr->station_flags & BCM_L2_STATION_IPV6_MCAST) {
           return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        /*
         * Provides TSN circuit ID for this packet:
         *  - 0 = Invalid TSN_CIRCUIT_ID, Valid only for MAC-DA lookup
         */
        if (l2addr->tsn_flowset != 0) {
            /*
             * To get base flow ID from
             * TSN flow set (l2addr->tsn_flowset)
             */
            BCM_IF_ERROR_RETURN(
                bcm_esw_tsn_flowset_flow_get(unit, l2addr->tsn_flowset, 0,
                                             &tsn_flow_id));

            /* Convert software TSN flow ID to hardware TSN flow ID */
            BCM_IF_ERROR_RETURN(
                bcmi_esw_tsn_hw_flow_id_get(unit, tsn_flow_id, &hw_id));
            soc_L2Xm_field32_set(unit, l2x_entry, TSN_CIRCUIT_IDf, hw_id);

            /*
             * To get priority map id from
             * TSN flow set (l2addr->tsn_flowset)
             */
            bcm_tsn_flow_config_t_init(&tsn_config);
            BCM_IF_ERROR_RETURN(
                bcm_esw_tsn_flowset_config_get(unit, l2addr->tsn_flowset,
                                               &pri_map, &tsn_config));

            if (pri_map != BCM_TSN_PRI_MAP_INVALID) {
                /* Get HW memory base index with given pri_map */
                BCM_IF_ERROR_RETURN(
                    bcmi_esw_tsn_pri_map_hw_index_get(unit, pri_map,
                                                      &map_type, &hw_id));
                if (map_type == bcmTsnPriMapTypeTsnFlow) {
                    soc_L2Xm_field32_set(unit, l2x_entry,
                                         TSN_PRI_OFFSET_PROFILEf, hw_id);
                } else {
                    return BCM_E_CONFIG;
                }
            }
        }
    }

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        /* Indicates this MAC address belongs to DAN or SAN (0: DAN, 1: SAN) */
        if (l2addr->flags & BCM_L2_SR_SAN_DEST) {
            soc_L2Xm_field32_set(unit, l2x_entry, SANf, 1);
        }

        /*
         * Provides the Flow ID for Seamless Redundancy:
         * - SR_Flow_ID=0 is not valid. Valid for DA or SA lookup depending on
         *   PORT.SR_FLOW_ID_SOURCE_PRIORITY.
         */
        if (l2addr->sr_flowset != 0) {
            /*
             * To get base flow ID from
             * SR flow set (l2addr->sr_flowset)
             */
            BCM_IF_ERROR_RETURN(
                bcm_esw_tsn_sr_flowset_flow_get(unit, l2addr->sr_flowset, 0,
                                                &sr_flow_id));

            /* Convert software SR flow ID to hardware SR flow ID */
            BCM_IF_ERROR_RETURN(
                bcmi_esw_tsn_sr_hw_flow_id_get(unit, sr_flow_id, &hw_id));
            soc_L2Xm_field32_set(unit, l2x_entry, SR_FLOW_IDf, hw_id);

            /*
             * To get priority map id from
             * SR flow set (l2addr->sr_flowset)
             */
            bcm_tsn_sr_tx_flow_config_t_init(&sr_tx_config);
            rv = bcm_esw_tsn_sr_tx_flowset_config_get(unit,
                                                      l2addr->sr_flowset,
                                                      &pri_map,
                                                      &sr_tx_config);

            if ((rv == BCM_E_NOT_FOUND) || (rv == BCM_E_BADID)) {
                bcm_tsn_sr_rx_flow_config_t_init(&sr_rx_config);
                rv = bcm_esw_tsn_sr_rx_flowset_config_get(unit,
                                                          l2addr->sr_flowset,
                                                          &pri_map,
                                                          &sr_rx_config);

                if (BCM_FAILURE(rv)) {
                    return rv;
                }
            } else if (BCM_FAILURE(rv)) {
                return rv;
            }

            if (pri_map != BCM_TSN_PRI_MAP_INVALID) {
                /* Get HW memory base index with given pri_map */
                BCM_IF_ERROR_RETURN(
                    bcmi_esw_tsn_pri_map_hw_index_get(unit, pri_map,
                                                      &map_type, &hw_id));
                if (map_type == bcmTsnPriMapTypeSrFlow) {
                    soc_L2Xm_field32_set(unit, l2x_entry,
                                         SR_PRI_OFFSET_PROFILEf, hw_id);
                } else {
                    return BCM_E_CONFIG;
                }
            }
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */

    if (soc_feature(unit, soc_feature_tsn_taf)) {
        if (BCM_TSN_PRI_MAP_INVALID != l2addr->taf_gate_primap) {
            uint32 hw_index;
            bcm_tsn_pri_map_type_t map;

            BCM_IF_ERROR_RETURN(
                bcmi_esw_tsn_pri_map_hw_index_get(
                    unit, l2addr->taf_gate_primap, &map, &hw_index));

            if (map != bcmTsnPriMapTypeTafGate) {
                return BCM_E_PARAM;
            }

            soc_L2Xm_field32_set(unit, l2x_entry,
                                 TAF_GATE_ID_PROFILEf,
                                 hw_index);
        } else {
            soc_L2Xm_field32_set(unit, l2x_entry,
                                 TAF_GATE_ID_PROFILEf,
                                 0);
        }
    }
#endif /* BCM_TSN_SUPPORT */
    return rv;
}

/*
 * Function:
 *      _bcm_tr_l2_from_ext_l2
 * Purpose:
 *      Convert an EXT_L2_ENTRY to an L2 API data structure

 * Parameters:
 *      unit         Unit number
 *      l2addr       (OUT) L2 API data structure
 *      ext_l2_entry EXT_L2_ENTRY hardware entry
 */
int
_bcm_tr_l2_from_ext_l2(int unit, bcm_l2_addr_t *l2addr,
                       ext_l2_entry_entry_t *ext_l2_entry)
{
    _bcm_gport_dest_t       dest;
    int                     mb_index, vfi;
    bcm_module_t            mod;
    bcm_port_t              port;
    int  isGport = 0;

    sal_memset(l2addr, 0, sizeof(*l2addr));

     if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                             KEY_TYPE_VFIf) == 1) {
         vfi = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, VFIf);
         COMPILER_REFERENCE(vfi);
         /* VPLS or MIM VPN */
#if defined(INCLUDE_L3)
         if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
              _BCM_MPLS_VPN_SET(l2addr->vid, _BCM_MPLS_VPN_TYPE_VPLS, vfi);
         } else {
              _BCM_MIM_VPN_SET(l2addr->vid, _BCM_MIM_VPN_TYPE_MIM, vfi);
         }
#endif
    } else {
         l2addr->vid = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                           VLAN_IDf);
    }

    soc_mem_mac_addr_get(unit, EXT_L2_ENTRYm, ext_l2_entry, MAC_ADDRf,
                         l2addr->mac);

    _bcm_gport_dest_t_init(&dest);
#if defined(INCLUDE_L3)
    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, DEST_TYPEf) == 0x2) {
         int vp;

         vp = soc_mem_field32_get(unit, EXT_L2_ENTRYm,ext_l2_entry,  DESTINATIONf);
         if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, KEY_TYPE_VFIf) == 0x1) {
              /* MPLS/MiM virtual port unicast */
              if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                   dest.mpls_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                   dest.mim_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_MIM_PORT;
                   isGport=1;
              } else {
                   /* The VP is deleted before clearing the L2 entry. 
                   SDK will report this entry with gport invalid during
                   traverse so the entry can be removed from the L2 table */
                   dest.gport_type = BCM_GPORT_INVALID;
                   isGport=0;
              }
         } else {
             /* Subport/WLAN unicast */
             if (_bcm_vp_used_get(unit, vp, _bcmVpTypeSubport)) {
                 dest.subport_id = vp;
                 dest.gport_type = _SHR_GPORT_TYPE_SUBPORT_GROUP;
                 isGport=1;
             } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
                 dest.wlan_id = vp;
                 dest.gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
                 isGport=1;
             } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                 dest.vlan_vp_id = vp;
                 dest.gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
                 isGport=1;
             } else {
                   /* The VP is deleted before clearing the L2 entry. 
                   SDK will report this entry with gport invalid during
                   traverse so the entry can be removed from the L2 table */
                   dest.gport_type = BCM_GPORT_INVALID;
                   isGport=0;
             }
         }
    } else {
#endif /* INCLUDE_L3 */
        if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, Tf)) {
            int psc = 0;
            l2addr->tgid = soc_mem_field32_get(unit, EXT_L2_ENTRYm,
                                               ext_l2_entry, TGIDf);
            bcm_esw_trunk_psc_get(unit, l2addr->tgid, &psc);
            dest.tgid = l2addr->tgid;
            dest.gport_type = _SHR_GPORT_TYPE_TRUNK;

            l2addr->flags |= BCM_L2_TRUNK_MEMBER;
            if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                    REMOTE_TRUNKf)) {
                l2addr->flags |= BCM_L2_REMOTE_TRUNK;
            }
        } else {
            mod = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                      MODULE_IDf);
            port = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                       PORT_NUMf);
            BCM_IF_ERROR_RETURN(_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                                       mod, port, &mod, &port));
            l2addr->modid = mod;
            l2addr->port = port;
            dest.port = l2addr->port;
            dest.modid = l2addr->modid;
            dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
        }
        BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit, bcmSwitchUseGport,
                                                      &isGport));
#if defined(INCLUDE_L3)
    }
#endif /* INCLUDE_L3 */
	
    if (isGport) {
         BCM_IF_ERROR_RETURN
             (_bcm_esw_gport_construct(unit, &dest, &l2addr->port));
    }

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        l2addr->group = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                            CLASS_IDf);
    } else {
        mb_index = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                       MAC_BLOCK_INDEXf);
        if (mb_index) {
            BCM_PBMP_ASSIGN(l2addr->block_bitmap,
                            _mbi_entries[unit][mb_index].mb_pbmp);
        }
    }

    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, RPEf)) {
        l2addr->flags |= BCM_L2_SETPRI;
    }

    l2addr->cos_dst = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                          PRIf);
    l2addr->cos_src = l2addr->cos_dst;

    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, CPUf)) {
        l2addr->flags |= BCM_L2_COPY_TO_CPU;
    }

    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, DST_DISCARDf)) {
        l2addr->flags |= BCM_L2_DISCARD_DST;
    }

    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, SRC_DISCARDf)) {
        l2addr->flags |= BCM_L2_DISCARD_SRC;
    }

    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, SCPf)) {
        l2addr->flags |= BCM_L2_COS_SRC_PRI;
    }

    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, STATIC_BITf)) {
        l2addr->flags |= BCM_L2_STATIC;
    }

    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, DST_HITf)) {
        l2addr->flags |= BCM_L2_DES_HIT;
        l2addr->flags |= BCM_L2_HIT;
    }

    if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, SRC_HITf)) {
        l2addr->flags |= BCM_L2_SRC_HIT;
        l2addr->flags |= BCM_L2_HIT;
    }

    if (SOC_MEM_FIELD_VALID(unit, EXT_L2_ENTRYm, LIMIT_COUNTEDf)) {
        if ((!(l2addr->flags & BCM_L2_STATIC)) || 
            l2addr->flags & BCM_L2_LEARN_LIMIT ) {
            if (!soc_L2Xm_field32_get(unit, ext_l2_entry, LIMIT_COUNTEDf)) {
                l2addr->flags |= BCM_L2_LEARN_LIMIT_EXEMPT;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_l2_to_ext_l2
 * Purpose:
 *      Convert an L2 API data structure to an EXT_L2_ENTRY
 * Parameters:
 *      unit         Unit number
 *      ext_l2_entry (OUT) EXT_L2_ENTRY hardware entry
 *      l2addr       L2 API data structure
 *      key_only     Only construct key portion
 */
int
_bcm_tr_l2_to_ext_l2(int unit, ext_l2_entry_entry_t *ext_l2_entry,
                     bcm_l2_addr_t *l2addr, int key_only)
{
    _bcm_l2_gport_params_t  g_params;
    bcm_module_t            mod;
    bcm_port_t              port;
    uint32                  fval;

    /*  BCM_L2_MIRROR is not supported starting from Triumph */
    if (l2addr->flags & BCM_L2_MIRROR) {
        return BCM_E_PARAM;
    }

    sal_memset(ext_l2_entry, 0, sizeof(*ext_l2_entry));

    if (_BCM_MPLS_VPN_IS_VPLS(l2addr->vid)) {
        _BCM_MPLS_VPN_GET(fval, _BCM_MPLS_VPN_TYPE_VPLS, l2addr->vid);
        if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, VFIf, fval)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VFIf, fval);
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, KEY_TYPE_VFIf,
                            1);
    } else if (_BCM_IS_MIM_VPN(l2addr->vid)) {
        _BCM_MIM_VPN_GET(fval, _BCM_MIM_VPN_TYPE_MIM,  l2addr->vid);
        if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, VFIf, fval)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VFIf, fval);
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, KEY_TYPE_VFIf,
                            1);
    } else {
        if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, VLAN_IDf,
                                       l2addr->vid)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VLAN_IDf,
                            l2addr->vid);
    }

    soc_mem_mac_addr_set(unit, EXT_L2_ENTRYm, ext_l2_entry, MAC_ADDRf,
                         l2addr->mac);

    if (key_only) {
        return BCM_E_NONE;
    }

    if (!BCM_GPORT_IS_SET(l2addr->port)) {
        if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
            g_params.param0 = l2addr->tgid;
            g_params.type = _SHR_GPORT_TYPE_TRUNK;
        } else {
            PORT_DUALMODID_VALID(unit, l2addr->port);
            BCM_IF_ERROR_RETURN(_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                                       l2addr->modid, l2addr->port,
                                                       &mod, &port));
            g_params.param0 = port;
            g_params.param1 = mod;
            g_params.type = _SHR_GPORT_TYPE_MODPORT;
        }
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_l2_gport_parse(unit, l2addr, &g_params));
    }

    switch (g_params.type) {
    case _SHR_GPORT_TYPE_TRUNK:
        if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, TGIDf,
                                       g_params.param0)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, Tf, 1);
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, TGIDf,
                            g_params.param0);
        if (l2addr->flags & BCM_L2_REMOTE_TRUNK) {
            soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                REMOTE_TRUNKf, 1);
        }
        break;
    case _SHR_GPORT_TYPE_MODPORT:
        if (!SOC_MODID_ADDRESSABLE(unit, g_params.param1)) {
            return BCM_E_BADID;
        }
        if (!SOC_PORT_ADDRESSABLE(unit, g_params.param0)) {
            return BCM_E_PORT;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, MODULE_IDf,
                            g_params.param1);
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, PORT_NUMf,
                            g_params.param0);
        break;
    case _SHR_GPORT_TYPE_LOCAL_CPU:
        if (!SOC_PORT_ADDRESSABLE(unit, g_params.param0)) {
            return BCM_E_PORT;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, PORT_NUMf,
                            g_params.param0);
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &mod));
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, MODULE_IDf,
                            mod);
        break;
    case _SHR_GPORT_TYPE_MPLS_PORT:
        if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, DESTINATIONf,
                                       g_params.param0)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, DESTINATIONf,
                            g_params.param0);
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, DEST_TYPEf, 0x2);
        break;
    case _SHR_GPORT_TYPE_MIM_PORT:
    case _SHR_GPORT_TYPE_SUBPORT_GROUP:
    case _SHR_GPORT_TYPE_SUBPORT_PORT:
    case _SHR_GPORT_TYPE_VLAN_PORT:
        if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, VPGf,
                                       g_params.param0)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VPGf,
                            g_params.param0);
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VPG_TYPEf, 1);
        break;
    default:
        return BCM_E_PORT;
    }

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, CLASS_IDf,
                            l2addr->group);
    } /* else MAC_BLOCK_INDEXf is handled in the add/remove functions */

    if (l2addr->flags & BCM_L2_SETPRI) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, RPEf, 1);
    }
    /* coverity[large_shift : FALSE] */
    if (!SOC_MEM_FIELD32_VALUE_FIT(unit, EXT_L2_ENTRYm, PRIf,
                                   l2addr->cos_dst)) {
        return BCM_E_PARAM;
    }
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, PRIf,
                        l2addr->cos_dst);

    if (l2addr->flags & BCM_L2_COPY_TO_CPU) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, CPUf, 1);
    }

    if (l2addr->flags & BCM_L2_DISCARD_DST) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, DST_DISCARDf,
                            1);
    }

    if (l2addr->flags & BCM_L2_DISCARD_SRC) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, SRC_DISCARDf,
                            1);
    }

    if (l2addr->flags & BCM_L2_COS_SRC_PRI) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, SCPf, 1);
    }

    if (l2addr->flags & BCM_L2_STATIC) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, STATIC_BITf, 1);
    }

    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VALIDf, 1);

    if ((l2addr->flags & BCM_L2_DES_HIT) ||
        (l2addr->flags & BCM_L2_HIT)) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, DST_HITf, 1);
    }

    if ((l2addr->flags & BCM_L2_SRC_HIT) ||
        (l2addr->flags & BCM_L2_HIT)) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, SRC_HITf, 1);
    }

    if (SOC_MEM_FIELD_VALID(unit, EXT_L2_ENTRYm, LIMIT_COUNTEDf)) {
        if ((!(l2addr->flags & (BCM_L2_L3LOOKUP | BCM_L2_MCAST | BCM_L2_STATIC |
                               BCM_L2_LEARN_LIMIT_EXEMPT))) || 
            l2addr->flags & BCM_L2_LEARN_LIMIT) {
            soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                LIMIT_COUNTEDf, 1);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_tr_compose_ext_l2_entry
 * Description:
 *      Compose ext_l2_entry from its tcam portion, data portion, and hit bit
 *      Hardware does not support read and write to ext_l2_entry view.
 * Parameters:
 *      unit         Device number
 *      tcam_entry   TCAM portion of ESM L2 entry (ext_l2_entry_tcam_entry_t)
 *      data_entry   DATA portion of ESM L2 entry (ext_l2_entry_data_entry_t)
 *      src_hit      SRC_HIT field value
 *      dst_hit      DST_HIT field value
 *      ext_l2_entry (OUT) Buffer to store the composed ext_l2_entry_entry_t
 *                   result
 * Return:
 *      BCM_E_XXX.
 */
int
_bcm_tr_compose_ext_l2_entry(int unit,
                             ext_l2_entry_tcam_entry_t *tcam_entry,
                             ext_l2_entry_data_entry_t *data_entry,
                             int src_hit,
                             int dst_hit,
                             ext_l2_entry_entry_t *ext_l2_entry)
{
    sal_mac_addr_t      mac;
    uint32              fval;
    uint32              fbuf[2];

    if (tcam_entry == NULL || data_entry == NULL || ext_l2_entry == NULL) {
        return BCM_E_PARAM;
    }

    sal_memset(ext_l2_entry, 0, sizeof(ext_l2_entry_entry_t));

    /******************** Values from TCAM *******************************/
    fval = soc_mem_field32_get(unit, EXT_L2_ENTRY_TCAMm, tcam_entry, VLAN_IDf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VLAN_IDf, fval);

    soc_mem_mac_addr_get(unit, EXT_L2_ENTRY_TCAMm, tcam_entry, MAC_ADDRf, mac);
    soc_mem_mac_addr_set(unit, EXT_L2_ENTRYm, ext_l2_entry, MAC_ADDRf, mac);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRY_TCAMm, tcam_entry,
                               KEY_TYPE_VFIf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, KEY_TYPE_VFIf,
                        fval);

    /******************** Values from DATA *******************************/
    soc_mem_field_get(unit, EXT_L2_ENTRY_DATAm, (uint32 *)data_entry,
                      AD_EXT_L2f, fbuf);
    soc_mem_field_set(unit, EXT_L2_ENTRYm, (uint32 *)ext_l2_entry, AD_EXT_L2f,
                      fbuf);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRY_DATAm, data_entry, VALIDf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VALIDf, fval);

    /******************** Hit Bits *******************************/
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, DST_HITf, dst_hit);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, SRC_HITf, src_hit);

    return BCM_E_NONE;
}

/*
 * function:
 *     _bcm_tr_l2_traverse_mem
 * Description:
 *      Helper function to _bcm_esw_l2_traverse to itterate over given memory
 *      and actually read the table and parse entries for Triumph external
 *      memory
 * Parameters:
 *     unit         device number
 *      mem         External L2 memory to read
 *     trav_st      Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
#ifdef BCM_TRIUMPH_SUPPORT
int
_bcm_tr_l2_traverse_mem(int unit, soc_mem_t mem, _bcm_l2_traverse_t *trav_st)
{
    _soc_tr_l2e_ppa_info_t    *ppa_info;
    ext_l2_entry_entry_t      ext_l2_entry;
    ext_l2_entry_tcam_entry_t tcam_entry;
    ext_l2_entry_data_entry_t data_entry;
    ext_src_hit_bits_l2_entry_t src_hit_entry;
    ext_dst_hit_bits_l2_entry_t dst_hit_entry;
    int                       src_hit, dst_hit;
    int                       idx, idx_max;

    if (mem != EXT_L2_ENTRYm) {
        return BCM_E_UNAVAIL;
    }

    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    ppa_info = SOC_CONTROL(unit)->ext_l2_ppa_info;
    if (ppa_info == NULL) {
        return BCM_E_NONE;
    }

    idx_max = soc_mem_index_max(unit, mem);
    for (idx = soc_mem_index_min(unit, mem); idx <= idx_max; idx++ ) {
        if (!(ppa_info[idx].data & _SOC_TR_L2E_VALID)) {
            continue;
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, EXT_L2_ENTRY_TCAMm, MEM_BLOCK_ANY, idx,
                          &tcam_entry));
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, EXT_L2_ENTRY_DATAm, MEM_BLOCK_ANY, idx,
                          &data_entry));
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, EXT_SRC_HIT_BITS_L2m, MEM_BLOCK_ANY, idx >> 5,
                          &src_hit_entry));
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, EXT_DST_HIT_BITS_L2m, MEM_BLOCK_ANY, idx >> 5,
                          &dst_hit_entry));
        src_hit = (soc_mem_field32_get
                   (unit, EXT_SRC_HIT_BITS_L2m, &src_hit_entry, SRC_HITf) >>
                   (idx & 0x1f)) & 1;
        dst_hit = (soc_mem_field32_get
                   (unit, EXT_DST_HIT_BITS_L2m, &dst_hit_entry, DST_HITf) >>
                   (idx & 0x1f)) & 1;
        BCM_IF_ERROR_RETURN
            (_bcm_tr_compose_ext_l2_entry(unit, &tcam_entry, &data_entry,
                                          src_hit, dst_hit, &ext_l2_entry));
        trav_st->data = (uint32 *)&ext_l2_entry;
        trav_st->mem = mem;

        BCM_IF_ERROR_RETURN(trav_st->int_cb(unit, trav_st));
    }

    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH_SUPPORT */

/*
 * Function:
 *      _bcm_mac_block_insert
 * Purpose:
 *      Find or create a MAC_BLOCK table entry matching the given bitmap.
 * Parameters:
 *      unit - Unit number
 *      mb_pbmp - egress port bitmap for source MAC blocking
 *      mb_index - (OUT) Index of MAC_BLOCK table with bitmap.
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_RESOURCE          No more MAC_BLOCK entries available
 *      BCM_E_PARAM             Bad bitmap supplied
 */
static int
_bcm_mac_block_insert(int unit, bcm_pbmp_t mb_pbmp, int *mb_index)
{
    int cur_index = 0;
    _bcm_mac_block_info_t *mbi = _mbi_entries[unit];
    mac_block_entry_t mbe;
    bcm_pbmp_t temp_pbmp;
#if defined(BCM_KATANA2_SUPPORT)
    uint32     fldbuf[SOC_PBMP_WORD_MAX];
    bcm_pbmp_t mb_pbmp0, mb_pbmp1;
    int        mask_w0_width = 0;
    int        mask_w1_width = 0;
    int        i = 0;

    SOC_PBMP_CLEAR(mb_pbmp0);
    SOC_PBMP_CLEAR(mb_pbmp1);
#endif

    /* Check for reasonable pbmp */
    BCM_PBMP_ASSIGN(temp_pbmp, mb_pbmp);
    BCM_PBMP_AND(temp_pbmp, PBMP_ALL(unit));
    if (BCM_PBMP_NEQ(mb_pbmp, temp_pbmp)) {
        return BCM_E_PARAM;
    }

    for (cur_index = 0; cur_index < _mbi_num[unit]; cur_index++) {
        if (BCM_PBMP_EQ(mbi[cur_index].mb_pbmp, mb_pbmp)) {
            mbi[cur_index].ref_count++;
            *mb_index = cur_index;
            return BCM_E_NONE;
        }
    }

    /* Not in table already, see if any space free */
    for (cur_index = 1; cur_index < _mbi_num[unit]; cur_index++) {
        if (mbi[cur_index].ref_count == 0) {
            /* Attempt insert */
            sal_memset(&mbe, 0, sizeof(mac_block_entry_t));

            if (soc_mem_field_valid(unit, MAC_BLOCKm, MAC_BLOCK_MASK_LOf)) {
#if defined(BCM_GREYHOUND2_SUPPORT)
                if (SOC_IS_GREYHOUND2(unit)) {
                    soc_mem_pbmp_field_set(unit, MAC_BLOCKm, &mbe, MAC_BLOCK_MASKf,
                                           &mb_pbmp);
                } else
#endif /* BCM_GREYHOUND2_SUPPORT */
                {
                soc_MAC_BLOCKm_field32_set(unit, &mbe, MAC_BLOCK_MASK_LOf,
                                           SOC_PBMP_WORD_GET(mb_pbmp, 0));
                }
            } else if (soc_mem_field_valid(unit, MAC_BLOCKm,
                                           MAC_BLOCK_MASK_W0f)) {
#if defined(BCM_KATANA2_SUPPORT)
                if (SOC_IS_KATANA2(unit)) {
                    mask_w0_width = soc_mem_field_length(unit, MAC_BLOCKm,
                                                         MAC_BLOCK_MASK_W0f);
                    for (i = 0; i < mask_w0_width; i++) {
                        if (SOC_PBMP_MEMBER(mb_pbmp, i)) {
                            SOC_PBMP_PORT_ADD(mb_pbmp0, i);
                        }
                    }

                    sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
                    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                        fldbuf[i] = SOC_PBMP_WORD_GET(mb_pbmp0, i);
                    }
                    soc_MAC_BLOCKm_field_set(unit, &mbe,
                                             MAC_BLOCK_MASK_W0f, fldbuf);
                } else
#endif /* BCM_KATANA2_SUPPORT */
                {
                    soc_MAC_BLOCKm_field32_set(unit, &mbe, MAC_BLOCK_MASK_W0f,
                                           SOC_PBMP_WORD_GET(mb_pbmp, 0));
                }
            } else if (soc_mem_field_valid(unit, MAC_BLOCKm,
                                           MAC_BLOCK_MASKf)) {
                soc_mem_pbmp_field_set(unit, MAC_BLOCKm, &mbe, MAC_BLOCK_MASKf,
                                       &mb_pbmp); 
            } else {
                return BCM_E_INTERNAL;
            }
            if (soc_mem_field_valid(unit, MAC_BLOCKm, MAC_BLOCK_MASK_HIf)) {
                soc_MAC_BLOCKm_field32_set(unit, &mbe, MAC_BLOCK_MASK_HIf,
                                           SOC_PBMP_WORD_GET(mb_pbmp, 1));
            } else if (soc_mem_field_valid(unit, MAC_BLOCKm, MAC_BLOCK_MASK_W1f)) {
#if defined(BCM_KATANA2_SUPPORT)
                if (SOC_IS_KATANA2(unit)) {
                    mask_w1_width = soc_mem_field_length(unit, MAC_BLOCKm,
                                                         MAC_BLOCK_MASK_W1f);

                    for (i = mask_w0_width;
                         i < (mask_w0_width + mask_w1_width); i++) {
                        if (SOC_PBMP_MEMBER(mb_pbmp, i)) {
                            SOC_PBMP_PORT_ADD(mb_pbmp1, (i - mask_w0_width));
                        }
                    }

                    sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
                    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                        fldbuf[i] = SOC_PBMP_WORD_GET(mb_pbmp1, i);
                    }
                    soc_MAC_BLOCKm_field_set(unit, &mbe,
                                             MAC_BLOCK_MASK_W1f, fldbuf);
                } else
#endif /* BCM_KATANA2_SUPPORT */
                {
                    soc_MAC_BLOCKm_field32_set(unit, &mbe, MAC_BLOCK_MASK_W1f,
                                           SOC_PBMP_WORD_GET(mb_pbmp, 1));
                }
            }
            if (soc_mem_field_valid(unit, MAC_BLOCKm, MAC_BLOCK_MASK_W2f)) {
#if defined(BCM_TRIDENT_SUPPORT)
                if (SOC_IS_TRIDENT(unit)) {
                    soc_MAC_BLOCKm_field32_set(unit, &mbe, MAC_BLOCK_MASK_W2f,
                                               SOC_PBMP_WORD_GET(mb_pbmp, 2));
                }
#endif
            }

            SOC_IF_ERROR_RETURN(WRITE_MAC_BLOCKm(unit, MEM_BLOCK_ALL,
                                                 cur_index, &mbe));
            mbi[cur_index].ref_count++;
            BCM_PBMP_ASSIGN(mbi[cur_index].mb_pbmp, mb_pbmp);
            *mb_index = cur_index;
            return BCM_E_NONE;
        }
    }

    /* Didn't find a free slot, out of table space */
    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_mac_block_delete
 * Purpose:
 *      Remove reference to MAC_BLOCK table entry matching the given bitmap.
 * Parameters:
 *      unit - Unit number
 *      mb_index - Index of MAC_BLOCK table with bitmap.
 */
static void
_bcm_mac_block_delete(int unit, int mb_index)
{
    if (_mbi_entries[unit][mb_index].ref_count > 0) {
        _mbi_entries[unit][mb_index].ref_count--;
    } else if (mb_index) {
        
        /* Someone reran init without flushing the L2 table */
    } /* else mb_index = 0, as expected for learning */
}

#ifdef BCM_TRIUMPH_SUPPORT
STATIC int
_bcm_tr_l2e_ppa_match(int unit, _bcm_l2_replace_t *rep_st)
{
    _soc_tr_l2e_ppa_info_t      *ppa_info;
    _soc_tr_l2e_ppa_vlan_t      *ppa_vlan;
    int                         i, imin, imax, rv, nmatches;
    soc_mem_t                   mem;
    uint32                      entdata, entmask, entvalue, newdata, newmask;
    ext_l2_entry_entry_t        l2entry, old_l2entry;
    int                         same_dest;

    ppa_info = SOC_CONTROL(unit)->ext_l2_ppa_info;
    ppa_vlan = SOC_CONTROL(unit)->ext_l2_ppa_vlan;
    if (ppa_info == NULL) {
        return BCM_E_NONE;
    }

    mem = EXT_L2_ENTRYm;
    imin = soc_mem_index_min(unit, mem);
    imax = soc_mem_index_max(unit, mem);

    /* convert match data */
    entdata = _SOC_TR_L2E_VALID;
    entmask = _SOC_TR_L2E_VALID;
    if (!(rep_st->flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        entdata |= 0x00000000;
        entmask |= _SOC_TR_L2E_STATIC;
    }
    if (rep_st->flags & BCM_L2_REPLACE_MATCH_VLAN) {
        entdata |= (rep_st->key_vlan & _SOC_TR_L2E_VLAN_MASK) <<
            _SOC_TR_L2E_VLAN_SHIFT;
        entmask |= _SOC_TR_L2E_VLAN_MASK << _SOC_TR_L2E_VLAN_SHIFT;
        imin = ppa_vlan->vlan_min[rep_st->key_vlan];
        imax = ppa_vlan->vlan_max[rep_st->key_vlan];
    }
    if (rep_st->flags & BCM_L2_REPLACE_MATCH_DEST) {
        if (rep_st->match_dest.trunk != -1) {
            entdata |= _SOC_TR_L2E_T |
                ((rep_st->match_dest.trunk & _SOC_TR_L2E_TRUNK_MASK)
                 << _SOC_TR_L2E_TRUNK_SHIFT);
            entmask |= _SOC_TR_L2E_T |
                (_SOC_TR_L2E_TRUNK_MASK << _SOC_TR_L2E_TRUNK_SHIFT);
        } else {
            entdata |= 
                ((rep_st->match_dest.module & _SOC_TR_L2E_MOD_MASK) <<
                 _SOC_TR_L2E_MOD_SHIFT) |
                ((rep_st->match_dest.port & _SOC_TR_L2E_PORT_MASK) <<
                 _SOC_TR_L2E_PORT_SHIFT);
            entmask |= _SOC_TR_L2E_T |
                (_SOC_TR_L2E_MOD_MASK << _SOC_TR_L2E_MOD_SHIFT) |
                (_SOC_TR_L2E_PORT_MASK << _SOC_TR_L2E_PORT_SHIFT);
        }
    }

    nmatches = 0;

    if (imin >= 0) {
        soc_mem_lock(unit, mem);
        for (i = imin; i <= imax; i++) {
            entvalue = ppa_info[i].data;
            if ((entvalue & entmask) != entdata) {
                continue;
            }
            if (rep_st->flags & BCM_L2_REPLACE_MATCH_MAC) {
                if (ENET_CMP_MACADDR(rep_st->key_mac, ppa_info[i].mac)) {
                    continue;
                }
            }
            nmatches += 1;

            /* lookup the matched entry */
            sal_memset(&l2entry, 0, sizeof(l2entry));
            soc_mem_field32_set(unit, mem, &l2entry, VLAN_IDf,
                                (entvalue >> 16) & 0xfff);
            soc_mem_mac_addr_set(unit, mem, &l2entry, MAC_ADDRf,
                                 ppa_info[i].mac);

            /* operate on matched entry */
            if (rep_st->flags & BCM_L2_REPLACE_DELETE) {
                int             mb_index;

                /* coverity[value_overwrite] */
                rv = soc_mem_generic_delete(unit, mem, MEM_BLOCK_ANY, 0,
                                            &l2entry, &l2entry, NULL);
                if (rv < 0) {
                    soc_mem_unlock(unit, mem);
                    return rv;
                }
                if (!SOC_CONTROL(unit)->l2x_group_enable) {
                    mb_index = soc_mem_field32_get(unit, mem, &l2entry,
                                                   MAC_BLOCK_INDEXf);
                    _bcm_mac_block_delete(unit, mb_index);
                }
                if (entvalue & _SOC_TR_L2E_LIMIT_COUNTED) {
                    /* coverity[returned_value] */
                    rv = soc_triumph_learn_count_update(unit, &l2entry,
                                                        TRUE, -1);
                }
            } else {
                /* replace destination fields */
                /* coverity[value_overwrite] */
                rv = soc_mem_generic_lookup(unit, mem, MEM_BLOCK_ANY, 0,
                                            &l2entry, &l2entry, NULL);
                if (rep_st->flags & BCM_L2_REPLACE_NEW_TRUNK) {
                    newdata = _SOC_TR_L2E_T |
                        ((rep_st->new_dest.trunk & _SOC_TR_L2E_TRUNK_MASK) <<
                         _SOC_TR_L2E_TRUNK_SHIFT);
                    newmask = _SOC_TR_L2E_T |
                        (_SOC_TR_L2E_TRUNK_MASK << _SOC_TR_L2E_TRUNK_SHIFT);
                    soc_mem_field32_set(unit, mem, &l2entry, Tf, 1);
                    soc_mem_field32_set(unit, mem, &l2entry, TGIDf,
                                        rep_st->new_dest.trunk);
                } else {
                    newdata =
                        (rep_st->new_dest.module << _SOC_TR_L2E_MOD_SHIFT) |
                        (rep_st->new_dest.port << _SOC_TR_L2E_PORT_SHIFT);
                    newmask = _SOC_TR_L2E_T |
                        (_SOC_TR_L2E_MOD_MASK << _SOC_TR_L2E_MOD_SHIFT) |
                        (_SOC_TR_L2E_PORT_MASK << _SOC_TR_L2E_PORT_SHIFT);
                    soc_mem_field32_set(unit, mem, &l2entry, MODULE_IDf,
                                        rep_st->new_dest.module);
                    soc_mem_field32_set(unit, mem, &l2entry, PORT_NUMf,
                                        rep_st->new_dest.port);
                }
                same_dest = !((entvalue ^ newdata) & newmask);

                if ((entvalue & _SOC_TR_L2E_LIMIT_COUNTED) && !same_dest) {
                    rv = soc_triumph_learn_count_update(unit, &l2entry, FALSE,
                                                        1);
                    if (SOC_FAILURE(rv)) {
                        soc_mem_unlock(unit, mem);
                        return rv;
                    }
                }

                /* re-insert entry */
                rv = soc_mem_generic_insert(unit, mem, MEM_BLOCK_ANY, 0,
                                            &l2entry, &old_l2entry, NULL);
                if (rv == BCM_E_EXISTS) {
                    rv = BCM_E_NONE;
                }
                if (rv < 0) {
                    soc_mem_unlock(unit, mem);
                    return rv;
                }
                if ((entvalue & _SOC_TR_L2E_LIMIT_COUNTED) && !same_dest) {
                    rv = soc_triumph_learn_count_update(unit, &old_l2entry,
                                                        FALSE, -1);
                    if (SOC_FAILURE(rv)) {
                        soc_mem_unlock(unit, mem);
                        return rv;
                    }
                }
            }
        }
        soc_mem_unlock(unit, EXT_L2_ENTRYm);
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "tr_l2e_ppa_match: imin=%d imax=%d nmatches=%d flags=0x%x\n"),
                 imin, imax, nmatches, rep_st->flags));
    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH_SUPPORT */

/*
 * Function:
 *      _bcm_l2_hash_dynamic_replace
 * Purpose:
 *      Replace dynamic L2 entries in a dual hash.
 * Parameters:
 *      unit - Unit number
 *      l2x_entry - Entry to insert instead of dynamic entry.
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX          		Error 
 */

int _bcm_l2_hash_dynamic_replace(int unit, l2x_entry_t *l2x_entry)
{
    l2x_entry_t     l2ent;
    int             rv;
    uint32          fval;
    bcm_mac_t       mac;
    int             cf_hit, cf_unhit;
    int             bank, bucket, slot, index;
    int             num_banks;
    int             entries_per_bank, entries_per_row, entries_per_bucket;
    int             bank_base, bucket_offset;

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_apache_hash_bank_count_get(unit, L2Xm, &num_banks));
    } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
         BCM_IF_ERROR_RETURN
            (soc_helix5_hash_bank_count_get(unit, L2Xm, &num_banks));
    } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_trident3_hash_bank_count_get(unit, L2Xm, &num_banks));
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (!SOC_IS_TOMAHAWKX(unit) && SOC_IS_TD2_TT2(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_trident2_hash_bank_count_get(unit, L2Xm, &num_banks));
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_tomahawk_hash_bank_count_get(unit, L2Xm, &num_banks));
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
    {
        num_banks = 2;
    }

    BCM_IF_ERROR_RETURN(soc_l2x_freeze(unit));

    cf_hit = cf_unhit = -1;
    for (bank = 0; bank < num_banks; bank++) {
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            int phy_bank;
            rv = soc_apache_hash_bank_number_get(unit, L2Xm, bank, &phy_bank);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }

            rv = soc_apache_hash_bank_info_get(unit, L2Xm, phy_bank,
                                                 &entries_per_bank,
                                                 &entries_per_row,
                                                 &entries_per_bucket,
                                                 &bank_base, &bucket_offset);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            bucket = soc_ap_l2x_bank_entry_hash(unit, phy_bank,
                                                (uint32 *)l2x_entry);
        } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
            int phy_bank;
            rv = soc_hx5_hash_bank_number_get(unit, L2Xm, bank, &phy_bank);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            rv = soc_hx5_hash_bank_info_get(unit, L2Xm, phy_bank,
                                            &entries_per_bank,
                                            &entries_per_row,
                                            &entries_per_bucket,
                                            &bank_base, &bucket_offset);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            bucket = soc_hx5_l2x_bank_entry_hash(unit, phy_bank,
                                                 (uint32 *)l2x_entry);
        } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            int phy_bank;
            rv = soc_td3_hash_bank_number_get(unit, L2Xm, bank, &phy_bank);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            rv = soc_td3_hash_bank_info_get(unit, L2Xm, phy_bank,
                                            &entries_per_bank,
                                            &entries_per_row,
                                            &entries_per_bucket,
                                            &bank_base, &bucket_offset);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            bucket = soc_td3_l2x_bank_entry_hash(unit, phy_bank,
                                                 (uint32 *)l2x_entry);
        } else
#endif
#ifdef BCM_TRIDENT2_SUPPORT
        if (!SOC_IS_TOMAHAWKX(unit) && SOC_IS_TD2_TT2(unit)) {
            int phy_bank;
            rv = soc_trident2_hash_bank_number_get(unit, L2Xm, bank, &phy_bank);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            rv = soc_trident2_hash_bank_info_get(unit, L2Xm, phy_bank,
                                                 &entries_per_bank,
                                                 &entries_per_row,
                                                 &entries_per_bucket,
                                                 &bank_base, &bucket_offset);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            bucket = soc_td2_l2x_bank_entry_hash(unit, phy_bank,
                                                 (uint32 *)l2x_entry);
        } else
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            int phy_bank;
            rv = soc_tomahawk_hash_bank_number_get(unit, L2Xm, bank, &phy_bank);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            rv = soc_tomahawk_hash_bank_info_get(unit, L2Xm, phy_bank,
                                                 &entries_per_bank,
                                                 &entries_per_row,
                                                 &entries_per_bucket,
                                                 &bank_base, &bucket_offset);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            bucket = soc_th_l2x_bank_entry_hash(unit, phy_bank,
                                                 (uint32 *)l2x_entry);
        } else
#endif
        {
            entries_per_bank = soc_mem_index_count(unit, L2Xm) / 2;
            entries_per_row = SOC_L2X_BUCKET_SIZE;
            entries_per_bucket = entries_per_row / 2;
            bank_base = 0;
            bucket_offset = bank * entries_per_bucket;
            bucket = soc_tr_l2x_bank_entry_hash(unit, bank,
                                                (uint32 *)l2x_entry);
        }

        for (slot = 0; slot < entries_per_bucket; slot++) {
            uint32 valid = 0;
            index = bank_base + bucket * entries_per_row + bucket_offset +
                slot;
            rv = soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY, index, &l2ent);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }

            if (soc_feature(unit, soc_feature_base_valid)) {
                valid = soc_L2Xm_field32_get(unit, &l2ent, BASE_VALIDf);
            } else {
                valid = soc_L2Xm_field32_get(unit, &l2ent, VALIDf);
            }
            if (!valid) {
                /* Found invalid entry - stop the search victim found */
                cf_unhit = index; 
                break;
            }

            fval = soc_mem_field32_get(unit, L2Xm, &l2ent, KEY_TYPEf);
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TD2_TT2(unit)) {
                if (fval != TD2_L2_HASH_KEY_TYPE_BRIDGE &&
                    fval != TD2_L2_HASH_KEY_TYPE_VFI) {
                    continue;
                }
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            if (fval != TR_L2_HASH_KEY_TYPE_BRIDGE &&
                fval != TR_L2_HASH_KEY_TYPE_VFI) {
                continue;
            }

            soc_L2Xm_mac_addr_get(unit, &l2ent, MAC_ADDRf, mac);
            /* Skip static entries */
            if ((soc_L2Xm_field32_get(unit, &l2ent, STATIC_BITf)) ||
                (BCM_MAC_IS_MCAST(mac)) ||
                (soc_mem_field_valid(unit, L2Xm, L3f) && 
                 soc_L2Xm_field32_get(unit, &l2ent, L3f))) {
                continue;
            }

            if (soc_L2Xm_field32_get(unit, &l2ent, HITDAf) || 
                soc_L2Xm_field32_get(unit, &l2ent, HITSAf) ) {
                cf_hit =  index;
            } else {
                /* Found unhit entry - stop search victim found */
                cf_unhit = index;
                break;
            }
        }
        if (cf_unhit != -1) {
            break;
        }
    }

    COMPILER_REFERENCE(entries_per_bank);

    if (cf_unhit >= 0) {
        index = cf_unhit;   /* take last unhit dynamic */
    } else if (cf_hit >= 0) {
        index = cf_hit;     /* or last hit dynamic */
    } else {
        rv = BCM_E_FULL;     /* no dynamics to delete */
        (void) soc_l2x_thaw(unit);
         return rv;
    }

    rv = soc_mem_delete_index(unit, L2Xm, MEM_BLOCK_ALL, index);
    if (SOC_SUCCESS(rv)) {
        rv = soc_mem_generic_insert(unit, L2Xm, MEM_BLOCK_ANY, 0,
                                    l2x_entry, NULL, NULL);
    }
    if (SOC_FAILURE(rv)) {
        (void) soc_l2x_thaw(unit);
        return rv;
    }

    return soc_l2x_thaw(unit);
}

/*
 * Function:
 *      _bcm_l2_hash_pending_override
 * Purpose:
 *      Override pending L2 entries in a dual hash.
 * Parameters:
 *      unit - Unit number
 *      l2x_entry - Entry to insert instead of pending entry.
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX          		Error
 */

int _bcm_l2_hash_pending_override(int unit, l2x_entry_t *l2x_entry)
{
    l2x_entry_t     l2ent;
    int             rv;
    uint32          fval;
    bcm_mac_t       mac;
    int             p_hit;
    int             bank, bucket, slot, index;
    int             num_banks;
    int             entries_per_bank, entries_per_row, entries_per_bucket;
    int             bank_base, bucket_offset;

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_apache_hash_bank_count_get(unit, L2Xm, &num_banks));
    } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_helix5_hash_bank_count_get(unit, L2Xm, &num_banks));
    } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_trident3_hash_bank_count_get(unit, L2Xm, &num_banks));
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (!SOC_IS_TOMAHAWKX(unit) && SOC_IS_TD2_TT2(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_trident2_hash_bank_count_get(unit, L2Xm, &num_banks));
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_tomahawk_hash_bank_count_get(unit, L2Xm, &num_banks));
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
    {
        num_banks = 2;
    }

    BCM_IF_ERROR_RETURN(soc_l2x_freeze(unit));

    p_hit = -1;
    for (bank = 0; bank < num_banks; bank++) {
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            int phy_bank;
            rv = soc_apache_hash_bank_number_get(unit, L2Xm, bank, &phy_bank);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }

            rv = soc_apache_hash_bank_info_get(unit, L2Xm, phy_bank,
                                               &entries_per_bank,
                                               &entries_per_row,
                                               &entries_per_bucket,
                                               &bank_base, &bucket_offset);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            bucket = soc_ap_l2x_bank_entry_hash(unit, phy_bank,
                                                (uint32 *)l2x_entry);
        } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
            int phy_bank;
            rv = soc_hx5_hash_bank_number_get(unit, L2Xm, bank, &phy_bank);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            rv = soc_hx5_hash_bank_info_get(unit, L2Xm, phy_bank,
                                            &entries_per_bank,
                                            &entries_per_row,
                                            &entries_per_bucket,
                                            &bank_base, &bucket_offset);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            bucket = soc_hx5_l2x_bank_entry_hash(unit, phy_bank,
                                                 (uint32 *)l2x_entry);
        } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            int phy_bank;
            rv = soc_td3_hash_bank_number_get(unit, L2Xm, bank, &phy_bank);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            rv = soc_td3_hash_bank_info_get(unit, L2Xm, phy_bank,
                                            &entries_per_bank,
                                            &entries_per_row,
                                            &entries_per_bucket,
                                            &bank_base, &bucket_offset);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            bucket = soc_td3_l2x_bank_entry_hash(unit, phy_bank,
                                                 (uint32 *)l2x_entry);
        } else
#endif
#ifdef BCM_TRIDENT2_SUPPORT
        if (!SOC_IS_TOMAHAWKX(unit) && SOC_IS_TD2_TT2(unit)) {
            int phy_bank;
            rv = soc_trident2_hash_bank_number_get(unit, L2Xm, bank, &phy_bank);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            rv = soc_trident2_hash_bank_info_get(unit, L2Xm, phy_bank,
                                                 &entries_per_bank,
                                                 &entries_per_row,
                                                 &entries_per_bucket,
                                                 &bank_base, &bucket_offset);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            bucket = soc_td2_l2x_bank_entry_hash(unit, phy_bank,
                                                 (uint32 *)l2x_entry);
        } else
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            int phy_bank;
            rv = soc_tomahawk_hash_bank_number_get(unit, L2Xm, bank, &phy_bank);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            rv = soc_tomahawk_hash_bank_info_get(unit, L2Xm, phy_bank,
                                                 &entries_per_bank,
                                                 &entries_per_row,
                                                 &entries_per_bucket,
                                                 &bank_base, &bucket_offset);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            bucket = soc_th_l2x_bank_entry_hash(unit, phy_bank,
                                                (uint32 *)l2x_entry);
        } else
#endif
        {
            entries_per_bank = soc_mem_index_count(unit, L2Xm) / 2;
            entries_per_row = SOC_L2X_BUCKET_SIZE;
            entries_per_bucket = entries_per_row / 2;
            bank_base = 0;
            bucket_offset = bank * entries_per_bucket;
            bucket = soc_tr_l2x_bank_entry_hash(unit, bank,
                                                (uint32 *)l2x_entry);
        }

        for (slot = 0; slot < entries_per_bucket; slot++) {
            uint32 valid = 0;
            index = bank_base + bucket * entries_per_row + bucket_offset +
                slot;
            rv = soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY, index, &l2ent);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }

            if (soc_feature(unit, soc_feature_base_valid)) {
                valid = soc_L2Xm_field32_get(unit, &l2ent, BASE_VALIDf);
            } else {
                valid = soc_L2Xm_field32_get(unit, &l2ent, VALIDf);
            }
            if (!valid) {
                /* Found invalid entry - stop the search victim found */
                p_hit = index;
                break;
            }

            fval = soc_mem_field32_get(unit, L2Xm, &l2ent, KEY_TYPEf);
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TD2_TT2(unit)) {
                if (fval != TD2_L2_HASH_KEY_TYPE_BRIDGE &&
                    fval != TD2_L2_HASH_KEY_TYPE_VFI) {
                    continue;
                }
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            if (fval != TR_L2_HASH_KEY_TYPE_BRIDGE &&
                fval != TR_L2_HASH_KEY_TYPE_VFI) {
                continue;
            }

            soc_L2Xm_mac_addr_get(unit, &l2ent, MAC_ADDRf, mac);
            /* Skip static entries */
            if ((soc_L2Xm_field32_get(unit, &l2ent, STATIC_BITf)) ||
                (BCM_MAC_IS_MCAST(mac)) ||
                (soc_mem_field_valid(unit, L2Xm, L3f) &&
                 soc_L2Xm_field32_get(unit, &l2ent, L3f))) {
                continue;
            }

            if (soc_L2Xm_field32_get(unit, &l2ent, L2__PENDINGf)) {
                p_hit =  index;
                break;
            }
        }
        if (p_hit != -1) {
            break;
        }
    }

    COMPILER_REFERENCE(entries_per_bank);

    if (p_hit >= 0) {
        index = p_hit;   /* take last hit pending */
    } else {
        rv = BCM_E_FULL; /* no pending entry to delete */
        (void) soc_l2x_thaw(unit);
         return rv;
    }

    rv = soc_mem_delete_index(unit, L2Xm, MEM_BLOCK_ALL, index);
    if (SOC_SUCCESS(rv)) {
        rv = soc_mem_generic_insert(unit, L2Xm, MEM_BLOCK_ANY, 0,
                                    l2x_entry, NULL, NULL);
    }
    if (SOC_FAILURE(rv)) {
        (void) soc_l2x_thaw(unit);
        return rv;
    }

    return soc_l2x_thaw(unit);
}


/*
 * Function:
 *      bcm_bfd_l2_hash_dynamic_replace
 * Purpose:
 *      Replace dynamic L2 entries in a dual hash.
 * Parameters:
 *      unit - Unit number
 *      l2x_entry - Entry to insert instead of dynamic entry.
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               Error
 */

int bcm_bfd_l2_hash_dynamic_replace(int unit, l2x_entry_t *l2x_entry)
{
    int rv;

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        rv = bcm_tr3_bfd_l2_hash_dynamic_replace(unit, (void *)l2x_entry);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        rv = _bcm_l2_hash_dynamic_replace(unit, l2x_entry);
    }

    return rv;
}

#if defined(BCM_TRIDENT2_SUPPORT)
/*
 * Function:
 *      _bcm_td2_l2_hit_retrieve
 * Purpose:
 *      Retrieve the hit bit from both x and y pipe, then "or" the hitbit values and assign it to 
 *      L2_ENTRY data structure.
 * Parameters:
 *      unit        Unit number
 *      l2addr      (OUT) L2 API data structure
 *      l2x_entry   L2X entry
 *      l2_hw_index   L2X entry hardware index
 */
int
_bcm_td2_l2_hit_retrieve(int unit, l2x_entry_t *l2x_entry,
                                 int l2_hw_index)
{ 
    uint32 hit_sa, hit_da, hit_local_sa;          /* composite hit = hit_x|hit_y */    
    int idx_offset, hit_idx_shift;
    l2_hitsa_only_x_entry_t hit_sa_x;
    l2_hitsa_only_y_entry_t hit_sa_y;
    l2_hitda_only_x_entry_t hit_da_x;
    l2_hitda_only_y_entry_t hit_da_y;  
    soc_field_t hit_saf[] = { HITSA_0f, HITSA_1f, HITSA_2f, HITSA_3f};
    soc_field_t hit_daf[] = { HITDA_0f, HITDA_1f, HITDA_2f, HITDA_3f};   
    soc_field_t hit_local_saf[] = { LOCAL_SA_0f, LOCAL_SA_1f, 
                                    LOCAL_SA_2f, LOCAL_SA_3f};     

    idx_offset = (l2_hw_index & 0x3);
    hit_idx_shift = 2;

    /* Retrieve DA HIT */
    BCM_IF_ERROR_RETURN(
        BCM_XGS3_MEM_READ(unit, L2_HITDA_ONLY_Xm,
          (l2_hw_index >> hit_idx_shift), &hit_da_x));
    
    BCM_IF_ERROR_RETURN(
        BCM_XGS3_MEM_READ(unit, L2_HITDA_ONLY_Ym,
          (l2_hw_index >> hit_idx_shift), &hit_da_y)); 

    hit_da = 0;
    hit_da |= soc_mem_field32_get(unit, L2_HITDA_ONLY_Xm,
                                   &hit_da_x, hit_daf[idx_offset]);
    
    hit_da |= soc_mem_field32_get(unit, L2_HITDA_ONLY_Ym,
                                   &hit_da_y, hit_daf[idx_offset]);
    
    _BCM_L2X_MEMACC_FIELD32_SET(unit, l2x_entry, HITDA_f, hit_da);  

    /* Retrieve SA HIT */
    BCM_IF_ERROR_RETURN(
        BCM_XGS3_MEM_READ(unit, L2_HITSA_ONLY_Xm,
          (l2_hw_index >> hit_idx_shift), &hit_sa_x));
    
    BCM_IF_ERROR_RETURN(
        BCM_XGS3_MEM_READ(unit, L2_HITSA_ONLY_Ym,
          (l2_hw_index >> hit_idx_shift), &hit_sa_y)); 

    hit_sa = 0;
    hit_sa |= soc_mem_field32_get(unit, L2_HITSA_ONLY_Xm,
                                   &hit_sa_x, hit_saf[idx_offset]);
    
    hit_sa |= soc_mem_field32_get(unit, L2_HITSA_ONLY_Ym,
                                   &hit_sa_y, hit_saf[idx_offset]);
    
    _BCM_L2X_MEMACC_FIELD32_SET(unit, l2x_entry, HITSA_f, hit_sa);        
    
    /* Retrieve LOCAL_SA HIT */
    hit_local_sa = 0;
    hit_local_sa |= soc_mem_field32_get(unit, L2_HITSA_ONLY_Xm,
                                   &hit_sa_x, hit_local_saf[idx_offset]);
    
    hit_local_sa |= soc_mem_field32_get(unit, L2_HITSA_ONLY_Ym,
                                   &hit_sa_y, hit_local_saf[idx_offset]);
    
    _BCM_L2X_MEMACC_FIELD32_SET(unit, l2x_entry, LOCAL_SA_f, hit_local_sa);        
    
    return BCM_E_NONE;    
}

/*
 * Function:
 *      _bcm_td2_l2_hit_range_retrieve
 * Purpose:
 *      Range retrieve the hit bit from both x and y pipe
 * Parameters:
 *      unit         (IN) Unit number
 *      mem          (IN) Unit number
 *      idx_min      (IN) L2 API data structure
 *      idx_max      (IN) L2X entry
 *      l2_tbl_chnk  (OUT) L2X entry chunck
 */
int
_bcm_td2_l2_hit_range_retrieve(int unit, soc_mem_t mem, int idx_min,
                               int idx_max, uint32 *l2_tbl_chnk)
{
    int rv = BCM_E_NONE;
    int i, ent_idx, ent_idx_end;
    int buf_size, offset, hit_idx_shift;
    int valid, key_type;
    uint32 *l2_entry;
    uint32 *l2_hit_ptr;
    struct _l2_hit_chnk_t {
        soc_mem_t mem;
        uint32 *_hit_chnk;
    } _td2_l2_hitda[] = {{L2_HITDA_ONLY_Xm, NULL},{L2_HITDA_ONLY_Ym, NULL}},
      _td2_l2_hitsa[] = {{L2_HITSA_ONLY_Xm, NULL},{L2_HITSA_ONLY_Ym, NULL}},
      *_l2_hitda = NULL,
      *_l2_hitsa = NULL;
    int _l2_hitda_cnt;
    int _l2_hitsa_cnt;
    int hit_da, hit_sa, hit_local_sa;

    _l2_hitda = &_td2_l2_hitda[0];
    _l2_hitsa = &_td2_l2_hitsa[0];
    _l2_hitda_cnt = COUNTOF(_td2_l2_hitda);
    _l2_hitsa_cnt = COUNTOF(_td2_l2_hitsa);


    hit_idx_shift = 2;

    /* HITDA */
    for (i = 0; i < _l2_hitda_cnt; i++) {
        buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * ((idx_max - idx_min + 3) / 4);
        _l2_hitda[i]._hit_chnk = soc_cm_salloc(unit, buf_size, "l2 hit range");
        if (NULL == _l2_hitda[i]._hit_chnk) {
            rv = BCM_E_MEMORY;
            goto clean_up;
        }
        rv = soc_mem_read_range(unit, _l2_hitda[i].mem, MEM_BLOCK_ANY,
                                (idx_min >> hit_idx_shift), 
                                (idx_max >> hit_idx_shift), 
                                _l2_hitda[i]._hit_chnk);
        if (SOC_FAILURE(rv)) {
            goto clean_up;
        }
    }

    /* HITSA */
    for (i = 0; i < _l2_hitsa_cnt; i++) {
        buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * ((idx_max - idx_min + 3) / 4);
        _l2_hitsa[i]._hit_chnk = soc_cm_salloc(unit, buf_size, "l2 hit range");
        if (NULL == _l2_hitsa[i]._hit_chnk) {
            rv = BCM_E_MEMORY;
            goto clean_up;
        }
        rv = soc_mem_read_range(unit, _l2_hitsa[i].mem, MEM_BLOCK_ANY,
                                (idx_min >> hit_idx_shift), 
                                (idx_max >> hit_idx_shift), 
                                _l2_hitsa[i]._hit_chnk);
        if (SOC_FAILURE(rv)) {
            goto clean_up;
        }
    }

    ent_idx_end = idx_max - idx_min;
    for (ent_idx = 0; ent_idx <= ent_idx_end; ent_idx++) {
        l2_entry = soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                                                l2_tbl_chnk, ent_idx);
        valid = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2_entry, VALID_f);
        key_type = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2_entry, KEY_TYPE_f);

        if (!valid || (key_type != TR_L2_HASH_KEY_TYPE_BRIDGE && 
                       key_type != TR_L2_HASH_KEY_TYPE_VFI)) {
            continue;
        }
        offset = (ent_idx & 0x3);
        /* Retrieve DA HIT */
        hit_da = 0;
        for (i = 0; i < _l2_hitda_cnt; i++) {
            l2_hit_ptr = soc_mem_table_idx_to_pointer(unit, _l2_hitda[i].mem,
                                            uint32 *, _l2_hitda[i]._hit_chnk,
                                            (ent_idx >> hit_idx_shift));
            if (!hit_da) {
                hit_da |= _TD2_L2HIT_MEMACC_FIELD32_GET(unit, 
                                _TD2_L2HIT_MEM(unit, L2HITDA, i), l2_hit_ptr, 
                                _TD2_L2HIT_FIELD(unit, L2HITDA,
                                                 HITDA_f, offset));
            }
        }
        _BCM_L2X_MEMACC_FIELD32_SET(unit, l2_entry, HITDA_f, hit_da);  

        /* Retrieve SA and LOCAL_SA HIT */
        hit_sa = 0;
        hit_local_sa = 0;
        for (i = 0; i < _l2_hitsa_cnt; i++) {
            l2_hit_ptr = soc_mem_table_idx_to_pointer(unit, _l2_hitsa[i].mem,
                                            uint32 *, _l2_hitsa[i]._hit_chnk,
                                            (ent_idx >> hit_idx_shift));
            if (!hit_sa) {
                hit_sa |= _TD2_L2HIT_MEMACC_FIELD32_GET(unit, 
                                _TD2_L2HIT_MEM(unit, L2HITSA, i), l2_hit_ptr,
                                _TD2_L2HIT_FIELD(unit, L2HITSA,
                                                 HITSA_f, offset));
            }
            if (!hit_local_sa) {
                hit_local_sa |= _TD2_L2HIT_MEMACC_FIELD32_GET(unit,
                                _TD2_L2HIT_MEM(unit, L2HITSA, i), l2_hit_ptr,
                                _TD2_L2HIT_FIELD(unit, L2HITSA, 
                                                 LOCAL_SA_f, offset));
            }
        }
        
        _BCM_L2X_MEMACC_FIELD32_SET(unit, l2_entry, HITSA_f, hit_sa);        
        _BCM_L2X_MEMACC_FIELD32_SET(unit, l2_entry, LOCAL_SA_f, hit_local_sa);        
    }

clean_up:
    for (i = 0; i < _l2_hitda_cnt; i++) {
        if (_l2_hitda[i]._hit_chnk) {
            soc_cm_sfree(unit, _l2_hitda[i]._hit_chnk);
        }
    }
    for (i = 0; i < _l2_hitsa_cnt; i++) {
        if (_l2_hitsa[i]._hit_chnk) {
            soc_cm_sfree(unit, _l2_hitsa[i]._hit_chnk);
        }
    }
    
    return (rv);    
}
#endif /* BCM_TRIDENT2_SUPPORT */

/*
 * Function:
 *      bcm_tr_l2_addr_add
 * Description:
 *      Add a MAC address to the Switch Address Resolution Logic (ARL)
 *      port with the given VLAN ID and parameters.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      l2addr - Pointer to bcm_l2_addr_t containing all valid fields
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_RESOURCE          No MAC_BLOCK entries available
 * Notes:
 *      Use CMIC_PORT(unit) to associate the entry with the CPU.
 *      Use flag of BCM_L2_COPY_TO_CPU to send a copy to the CPU.
 *      Use flag of BCM_L2_TRUNK_MEMBER to set trunking (TGID must be
 *      passed as well with non-zero trunk group ID)
 *      In case the L2X table is full (e.g. bucket full), an attempt
 *      will be made to store the entry in the L2_USER_ENTRY table.
 */
int
bcm_tr_l2_addr_add(int unit, bcm_l2_addr_t *l2addr)
{
    l2x_entry_t  l2x_entry, l2x_lookup, l2x_old;
#ifdef BCM_TRIUMPH_SUPPORT
    _soc_tr_l2e_ppa_info_t *ppa_info;
    ext_l2_entry_entry_t ext_l2_entry, ext_l2_lookup;
    int          exist_in_ext_l2, same_dest, update_limit, limit_counted;
    int          rv1, ext_l2_index;
#endif /* BCM_TRIUMPH_SUPPORT */
    int          enable_ppa_war;
    int          rv, l2_index, mb_index = 0;
#if defined(BCM_TSN_SUPPORT)
    bcm_l2_addr_t old_l2addr;
    int is_tsn_or_sr_flow = FALSE; /* TSN or SR related flow */
#if defined(BCM_TSN_SR_SUPPORT)
    int mp_index = 0; /* TSN SR MAC Proxy profile index */
    bcm_pbmp_t mp_pbmp;
#endif /* BCM_TSN_SR_SUPPORT */
#endif /* BCM_TSN_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    int         l2_flex_view = FALSE;
    uint32      view_id;
    uint32      action_set;
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_GLOBAL_METER_SUPPORT)
    uint32 associated_data_type = 0;
#endif /* BCM_GLOBAL_METER_SUPPORT */
    int rv2 = 0;
    int goto_done = 0;
    int rv_exists = 0;
    int fifo_modified = 0;
    uint32 rval = 0;
    uint32 l2_insert = 0;
    uint32 l2_delete = 0;

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_KATANAX(unit) ||
         SOC_IS_TD2_TT2(unit) || soc_feature(unit, soc_feature_gh2_my_station)) {
        if (soc_mem_is_valid(unit, MY_STATION_TCAMm) &&
            (l2addr->flags & BCM_L2_L3LOOKUP)) {
            BCM_IF_ERROR_RETURN(bcm_td_l2_myStation_add(unit, l2addr));
        }
        else if (soc_mem_is_valid(unit, MY_STATION_TCAMm) &&
                !(l2addr->flags & BCM_L2_L3LOOKUP)) {
            /* If VPN specified do not perform myStation delete */
            if (!_BCM_VPN_VFI_IS_SET(l2addr->vid)) {
                rv = bcm_td_l2_myStation_delete(unit, 
                            l2addr->mac, l2addr->vid, &l2_index);
                if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
                    return rv;
                }
            }
        }
    }
#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND2_SUPPORT */

    BCM_IF_ERROR_RETURN(_bcm_tr_l2_to_l2x(unit, &l2x_entry, l2addr, FALSE));

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow) &&
        (l2addr->flow_handle != 0)) {

        l2_flex_view = TRUE;
       /* Get view id corresponding to the specified flow. */
        rv = soc_flow_db_ffo_to_mem_view_id_get(unit,
                              l2addr->flow_handle,
                              l2addr->flow_option_handle,
                              SOC_FLOW_DB_FUNC_L2_SWITCH_ID,
                              &view_id);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_TRIDENT3_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
    exist_in_ext_l2 = FALSE;
    ppa_info = SOC_CONTROL(unit)->ext_l2_ppa_info;
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm) &&
        soc_mem_index_count(unit, EXT_L2_ENTRYm) > 0) {
        same_dest = FALSE;
        update_limit =
            (l2addr->flags & (BCM_L2_STATIC | BCM_L2_LEARN_LIMIT_EXEMPT) && 
             !(l2addr->flags & BCM_L2_LEARN_LIMIT)) ? FALSE : TRUE;
        limit_counted = FALSE;
        BCM_IF_ERROR_RETURN
            (_bcm_tr_l2_to_ext_l2(unit, &ext_l2_entry, l2addr, FALSE));
        soc_mem_lock(unit, EXT_L2_ENTRYm);
        rv1 = soc_mem_generic_lookup(unit, EXT_L2_ENTRYm, MEM_BLOCK_ANY, 0,
                                     &ext_l2_entry, &ext_l2_lookup,
                                     &ext_l2_index);
        if (SOC_SUCCESS(rv1)) {
            exist_in_ext_l2 = TRUE;
        } else if (rv1 != SOC_E_NOT_FOUND) {
            soc_mem_unlock(unit, EXT_L2_ENTRYm);
            return rv1;
        }

        if (!(l2addr->flags & (BCM_L2_L3LOOKUP | BCM_L2_MCAST |
                               BCM_L2_PENDING | BCM_L2_STATIC))) {
            if (exist_in_ext_l2) {
                if (soc_mem_field32_get(unit, EXT_L2_ENTRYm, &ext_l2_entry,
                                        DEST_TYPEf) ==
                    soc_mem_field32_get(unit, EXT_L2_ENTRYm, &ext_l2_lookup,
                                        DEST_TYPEf) &&
                    soc_mem_field32_get(unit, EXT_L2_ENTRYm, &ext_l2_entry,
                                        DESTINATIONf) ==
                    soc_mem_field32_get(unit, EXT_L2_ENTRYm, &ext_l2_lookup,
                                        DESTINATIONf)) {
                    same_dest = TRUE;
                }
                limit_counted =
                    ppa_info[ext_l2_index].data & _SOC_TR_L2E_LIMIT_COUNTED;
            }
            if (update_limit) {
                rv1 = SOC_E_NONE;
                if (!limit_counted) {
                    rv1 = soc_triumph_learn_count_update(unit, &ext_l2_entry,
                                                         TRUE, 1);
                } else if (!same_dest) {
                    rv1 = soc_triumph_learn_count_update(unit, &ext_l2_entry,
                                                         FALSE, 1);
                }
                if (SOC_FAILURE(rv1)) {
                    soc_mem_unlock(unit, EXT_L2_ENTRYm);
                    return rv1;
                }
            }
            if (!SOC_CONTROL(unit)->l2x_group_enable) {
                /* Mac blocking, attempt to associate with bitmap entry */
                rv1 = _bcm_mac_block_insert(unit, l2addr->block_bitmap,
                                           &mb_index);
                if (SOC_FAILURE(rv1)) {
                    soc_mem_unlock(unit, EXT_L2_ENTRYm);
                    return rv1;
                }
                soc_mem_field32_set(unit, EXT_L2_ENTRYm, &ext_l2_entry,
                                    MAC_BLOCK_INDEXf, mb_index);
            }
            rv = soc_mem_generic_insert(unit, EXT_L2_ENTRYm, MEM_BLOCK_ANY, 0,
                                        &ext_l2_entry, &ext_l2_lookup, NULL);
            if (rv == SOC_E_EXISTS) {
                /* entry exists, clear setting for old entry */
                if (!SOC_CONTROL(unit)->l2x_group_enable) {
                    mb_index = soc_mem_field32_get(unit, EXT_L2_ENTRYm,
                                                   &ext_l2_lookup,
                                                   MAC_BLOCK_INDEXf);
                    _bcm_mac_block_delete(unit, mb_index);
                }
                if (limit_counted) {
                    if (!update_limit) {
                        (void)soc_triumph_learn_count_update(unit,
                                                             &ext_l2_lookup,
                                                             TRUE, -1);
                    } else if (!same_dest) {
                        (void)soc_triumph_learn_count_update(unit,
                                                             &ext_l2_lookup,
                                                             FALSE, -1);
                    }
                }
                rv = BCM_E_NONE;
            } else if (SOC_FAILURE(rv)) {
                /* insert fail, undo setting for new entry */
                if (update_limit) {
                    if (!limit_counted) {
                        (void)soc_triumph_learn_count_update(unit,
                                                             &ext_l2_entry,
                                                             TRUE, -1);
                    } else if (!same_dest) {
                        (void)soc_triumph_learn_count_update(unit,
                                                             &ext_l2_entry,
                                                             FALSE, -1);
                    }
                }
            }
            soc_mem_unlock(unit, EXT_L2_ENTRYm);
            if (SOC_SUCCESS(rv)) {
                /* insert to ext_l2_entry OK, delete from l2x if present */
                soc_mem_lock(unit, L2Xm);
                if (SOC_L2_DEL_SYNC_LOCK(SOC_CONTROL(unit)) >= 0) {
                    rv1 = soc_mem_generic_delete(unit, L2Xm, MEM_BLOCK_ANY, 0,
                                                 &l2x_entry, &l2x_lookup,
                                                 &l2_index);
                    if (SOC_SUCCESS(rv1)) {
                        if (!SOC_CONTROL(unit)->l2x_group_enable) {
                            mb_index =
                                soc_mem_field32_get(unit, L2Xm, &l2x_lookup,
                                                    MAC_BLOCK_INDEXf);
                            _bcm_mac_block_delete(unit, mb_index);
                        }
                        rv1 = soc_l2x_sync_delete(unit, (uint32 *)&l2x_lookup,
                                                  l2_index, 0);
                    } else if (rv1 == SOC_E_NOT_FOUND) {
                        rv1 = BCM_E_NONE;
                    }
                    SOC_L2_DEL_SYNC_UNLOCK(SOC_CONTROL(unit));
                } else {
                    rv1 = BCM_E_INTERNAL;
                }
                soc_mem_unlock(unit, L2Xm);
                return rv1;
            }
            if (rv != SOC_E_FULL) {
                goto done;
            }
        } else {
            soc_mem_unlock(unit, EXT_L2_ENTRYm);
        }
    }
#endif

    rv = soc_mem_generic_lookup(unit, L2Xm, MEM_BLOCK_ANY, 0, &l2x_entry,
                                &l2x_lookup, &l2_index);
    if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
        return rv;
    }

    if (!SOC_CONTROL(unit)->l2x_group_enable) {
        /* Mac blocking, attempt to associate with bitmap entry */
        BCM_IF_ERROR_RETURN
            (_bcm_mac_block_insert(unit, l2addr->block_bitmap, &mb_index));
#ifdef BCM_TRIDENT3_SUPPORT
        if (l2_flex_view) {
            if (soc_mem_field_valid(unit, view_id, MAC_BLOCK_ACTION_SETf)) {
                action_set = 0;
                soc_format_field32_set(unit, MAC_BLOCK_ACTION_SETfmt,
                                    &action_set, MAC_BLOCK_INDEXf, mb_index);
                soc_mem_field32_set(unit, view_id, &l2x_entry,
                                   MAC_BLOCK_ACTION_SETf, action_set);
            }
        } else
#endif
        {
        soc_mem_field32_set(unit, L2Xm, &l2x_entry, MAC_BLOCK_INDEXf,
                            mb_index);
    }
    }

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        /* SR Mac Proxy profile, attempt to associate with bitmap entry */
        if (!BCM_PBMP_IS_NULL(l2addr->sa_source_filter_pbmp)) {
            /* The value should be inverted between SW and HW view */
            BCM_PBMP_NEGATE(mp_pbmp, l2addr->sa_source_filter_pbmp);
            BCM_PBMP_AND(mp_pbmp, PBMP_ALL(unit));
            rv = bcmi_esw_tsn_sr_mac_proxy_insert(unit, mp_pbmp, &mp_index);
            if (BCM_FAILURE(rv)) {
                if (!SOC_CONTROL(unit)->l2x_group_enable) {
                    _bcm_mac_block_delete(unit, mb_index);
                }
                return rv;
            }
            soc_mem_field32_set(unit, L2Xm, &l2x_entry,
                                SR_MAC_PROXY_PROFILE_PTRf, mp_index);
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */

#if defined(BCM_GLOBAL_METER_SUPPORT)
    /* Global Meter Policer Configuration */
    if (soc_feature(unit, soc_feature_global_meter) &&
        soc_feature(unit, soc_feature_global_meter_source_l2)) {
        /* The SGLPf (used by VXLAN) is overlay with Global Meter Policer */
        if (soc_feature(unit, soc_feature_vxlan_lite_riot) || SOC_IS_FIRELIGHT(unit)) {
            associated_data_type =
                soc_L2Xm_field32_get(unit, &l2x_entry, ASSOCIATED_DATA_TYPEf);
        }
        if (associated_data_type != 0x1) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_add_policer_to_table(
                    unit, l2addr->policer_id, L2Xm, 0, (void *)(&l2x_entry)));
        }
    }
#endif /* BCM_GLOBAL_METER_SUPPORT */

    enable_ppa_war = FALSE;
    if (SOC_CONTROL(unit)->l2x_ppa_bypass == FALSE &&
        soc_feature(unit, soc_feature_ppa_bypass) &&
        soc_mem_field32_get(unit, L2Xm, &l2x_entry, KEY_TYPEf) !=
        TR_L2_HASH_KEY_TYPE_BRIDGE) {
        enable_ppa_war = TRUE;
    }

    soc_mem_lock(unit, L2Xm);
    SOC_L2_SR_LOCK(unit);
    if (l2addr->flags2 & BCM_L2_FLAGS2_ADD_NO_CALLBACKS) {
        if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO &&
            soc_l2mod_running(unit, NULL, NULL)) {
            if (SOC_REG_IS_VALID(unit, L2_MOD_FIFO_ENABLEr)) {
                rv = READ_L2_MOD_FIFO_ENABLEr(unit, &rval);
                if (rv < 0) {
                    SOC_L2_SR_UNLOCK(unit);
                    soc_mem_unlock(unit, L2Xm);
                    goto done;
                }
                l2_insert = soc_reg_field_get(
                    unit, L2_MOD_FIFO_ENABLEr, rval, L2_INSERTf);
                l2_delete = soc_reg_field_get(
                    unit, L2_MOD_FIFO_ENABLEr, rval, L2_DELETEf);
                soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval, L2_INSERTf, 0);
                soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval, L2_DELETEf, 0);
                rv = WRITE_L2_MOD_FIFO_ENABLEr(unit, rval);
                if (rv < 0) {
                    SOC_L2_SR_UNLOCK(unit);
                    soc_mem_unlock(unit, L2Xm);
                    goto done;
                }
            /* coverity[CONSTANT_EXPRESSION_RESULT] */
            } else if (SOC_REG_IS_VALID(unit, AUX_ARB_CONTROLr)) {
                rv = READ_AUX_ARB_CONTROLr(unit, &rval);
                if (rv < 0) {
                    SOC_L2_SR_UNLOCK(unit);
                    soc_mem_unlock(unit, L2Xm);
                    goto done;
                }
                if (soc_reg_field_valid(unit, AUX_ARB_CONTROLr,
                        L2_MOD_FIFO_ENABLE_L2_INSERTf)) {
                    l2_insert = soc_reg_field_get(unit, AUX_ARB_CONTROLr,
                        rval, L2_MOD_FIFO_ENABLE_L2_INSERTf);
                    soc_reg_field_set(unit, AUX_ARB_CONTROLr,
                        &rval, L2_MOD_FIFO_ENABLE_L2_INSERTf, 0);
                }
                if (soc_reg_field_valid(unit, AUX_ARB_CONTROLr,
                    L2_MOD_FIFO_ENABLE_L2_DELETEf)) {
                    l2_delete = soc_reg_field_get(unit, AUX_ARB_CONTROLr,
                        rval, L2_MOD_FIFO_ENABLE_L2_DELETEf);
                    soc_reg_field_set(unit, AUX_ARB_CONTROLr,
                        &rval, L2_MOD_FIFO_ENABLE_L2_DELETEf, 0);
                }
                rv = WRITE_AUX_ARB_CONTROLr(unit, rval);
                if (rv < 0) {
                    SOC_L2_SR_UNLOCK(unit);
                    soc_mem_unlock(unit, L2Xm);
                    goto done;
                }
            }
            fifo_modified = TRUE;
        }
    }
    rv = soc_mem_insert_return_old(unit, L2Xm, MEM_BLOCK_ANY,
                                   (void *)&l2x_entry, (void *)&l2x_old);
    if ((rv == BCM_E_FULL) && (l2addr->flags & BCM_L2_REPLACE_DYNAMIC)) {
        rv = _bcm_l2_hash_dynamic_replace( unit, &l2x_entry);
        if (rv < 0) {
            goto_done = TRUE;
        }
    } else if ((rv == BCM_E_FULL) &&
               (l2addr->flags & BCM_L2_ADD_OVERRIDE_PENDING)) {
        rv = _bcm_l2_hash_pending_override( unit, &l2x_entry);
        if (rv < 0) {
            goto_done = TRUE;
        }
    } else if (rv == BCM_E_EXISTS) {
        rv_exists = TRUE;
    }

    if (l2addr->flags2 & BCM_L2_FLAGS2_ADD_NO_CALLBACKS) {
        if (fifo_modified) {
            if (SOC_REG_IS_VALID(unit, L2_MOD_FIFO_ENABLEr)) {
                rv2 = READ_L2_MOD_FIFO_ENABLEr(unit, &rval);
                if (rv2 < 0) {
                    rv = rv2;
                    SOC_L2_SR_UNLOCK(unit);
                    soc_mem_unlock(unit, L2Xm);
                    goto done;
                }
                soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr,
                    &rval, L2_INSERTf, l2_insert);
                soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr,
                    &rval, L2_DELETEf, l2_delete);
                rv2 = WRITE_L2_MOD_FIFO_ENABLEr(unit, rval);
                if (rv2 < 0) {
                    rv = rv2;
                    SOC_L2_SR_UNLOCK(unit);
                    soc_mem_unlock(unit, L2Xm);
                    goto done;
                }
            /* coverity[CONSTANT_EXPRESSION_RESULT] */
            } else if (SOC_REG_IS_VALID(unit, AUX_ARB_CONTROLr)) {
                rv2 = READ_AUX_ARB_CONTROLr(unit, &rval);
                if (rv2 < 0) {
                    rv = rv2;
                    SOC_L2_SR_UNLOCK(unit);
                    soc_mem_unlock(unit, L2Xm);
                    goto done;
                }
                if (soc_reg_field_valid(unit, AUX_ARB_CONTROLr,
                        L2_MOD_FIFO_ENABLE_L2_INSERTf)) {
                    soc_reg_field_set(unit, AUX_ARB_CONTROLr,
                        &rval, L2_MOD_FIFO_ENABLE_L2_INSERTf, l2_insert);
                }
                if (soc_reg_field_valid(unit, AUX_ARB_CONTROLr,
                    L2_MOD_FIFO_ENABLE_L2_DELETEf)) {
                    soc_reg_field_set(unit, AUX_ARB_CONTROLr,
                        &rval, L2_MOD_FIFO_ENABLE_L2_DELETEf, l2_delete);
                }
                rv2 = WRITE_AUX_ARB_CONTROLr(unit, rval);
                if (rv2 < 0) {
                    rv = rv2;
                    SOC_L2_SR_UNLOCK(unit);
                    soc_mem_unlock(unit, L2Xm);
                    goto done;
                }
            }
        }
    }

    if (goto_done) {
        SOC_L2_SR_UNLOCK(unit);
        soc_mem_unlock(unit, L2Xm);
        goto done;
    } else if (rv_exists) {
        if (!SOC_CONTROL(unit)->l2x_group_enable) {
#ifdef BCM_TRIDENT3_SUPPORT
            if (l2_flex_view) {
                if (soc_mem_field_valid(unit, view_id, MAC_BLOCK_ACTION_SETf)) {
                    action_set = soc_mem_field32_get(unit, view_id, &l2x_entry,
                                       MAC_BLOCK_ACTION_SETf);
                    mb_index = soc_format_field32_get(unit,
                                         MAC_BLOCK_ACTION_SETfmt,
                                        &action_set, MAC_BLOCK_INDEXf);
                }
            } else
#endif
            {
            mb_index = soc_mem_field32_get(unit, L2Xm, &l2x_lookup,
                                           MAC_BLOCK_INDEXf);
            }
            _bcm_mac_block_delete(unit, mb_index);
        }

#if defined(BCM_TSN_SR_SUPPORT)
        if (soc_feature(unit, soc_feature_tsn_sr)) {
            mp_index = soc_mem_field32_get(unit, L2Xm,
                                           &l2x_old,
                                           SR_MAC_PROXY_PROFILE_PTRf);
            if (mp_index != 0) {
                rv = bcmi_esw_tsn_sr_mac_proxy_delete(unit, mp_index);
                if (rv < 0) {
                    SOC_L2_SR_UNLOCK(unit);
                    soc_mem_unlock(unit, L2Xm);
                    return rv;
                }
            }
        }
#endif /* BCM_TSN_SR_SUPPORT */

#if defined(BCM_TSN_SUPPORT)
        if (soc_feature(unit, soc_feature_tsn)) {
            /* Decrease reference count for the existed TSN flowset */
            if (soc_L2Xm_field32_get(unit, &l2x_old,
                                     TSN_CIRCUIT_IDf) != 0) {
                rv = _bcm_tr_l2_from_l2x(unit, &old_l2addr, &l2x_old);
                if (rv < 0) {
                    SOC_L2_SR_UNLOCK(unit);
                    soc_mem_unlock(unit, L2Xm);
                    return rv;
                }
                rv = bcmi_esw_tsn_flowset_ref_dec(unit,
                                                  old_l2addr.tsn_flowset);
                if (rv < 0) {
                    SOC_L2_SR_UNLOCK(unit);
                    soc_mem_unlock(unit, L2Xm);
                    return rv;
                }
            }
        }
#if defined(BCM_TSN_SR_SUPPORT)
        if (soc_feature(unit, soc_feature_tsn_sr)) {
            /* Decrease reference count for the existed SR flowset */
            if (soc_L2Xm_field32_get(unit, &l2x_old,
                                     SR_FLOW_IDf) != 0) {
                rv = _bcm_tr_l2_from_l2x(unit, &old_l2addr, &l2x_old);
                if (rv < 0) {
                    SOC_L2_SR_UNLOCK(unit);
                    soc_mem_unlock(unit, L2Xm);
                    return rv;
                }
                rv = bcmi_esw_tsn_sr_flowset_ref_dec(unit,
                                                     old_l2addr.sr_flowset);
                if (rv < 0) {
                    SOC_L2_SR_UNLOCK(unit);
                    soc_mem_unlock(unit, L2Xm);
                    return rv;
                }
            }
        }
#endif /* BCM_TSN_SR_SUPPORT */
#endif /* BCM_TSN_SUPPORT */

#if defined(BCM_GLOBAL_METER_SUPPORT)
        if (soc_feature(unit, soc_feature_global_meter) &&
            soc_feature(unit, soc_feature_global_meter_source_l2)) {
            bcm_policer_t policer_id;

            rv = _bcm_esw_get_policer_from_table(
                    unit, L2Xm, 0, &l2x_lookup, &policer_id, TRUE);
            if (rv < 0) {
                SOC_L2_SR_UNLOCK(unit);
                soc_mem_unlock(unit, L2Xm);
                return rv;
            }
            _bcm_esw_policer_decrement_ref_count(unit, policer_id);
        }
#endif /* BCM_GLOBAL_METER_SUPPORT */
        rv = BCM_E_NONE;
    }

#if defined(BCM_TSN_SUPPORT)
    if (BCM_SUCCESS(rv)) {
        /*
         * For manual L2 add and L2 delete:
         * - Update the TSN/SR flowset reference count bitmap for L2X reference.
         * (this is for L2MODE_POLL mode, not required for L2MOD_FIFO mode).
         * For packet learn and age (including ppa):
         * - the TSN/SR flowset reference count will be handled in
         * soc_l2x_callback with SOC_L2X_ENTRY_TSN_SR_FLOWSET_REF_CNT flag.
         */
        if (soc_feature(unit, soc_feature_tsn)) {
            /* Increase reference count for the specified TSN flowset */
            if (l2addr->tsn_flowset != 0) {
                rv = bcmi_esw_tsn_flowset_ref_inc(unit, l2addr->tsn_flowset);
                if (rv < 0) {
                    SOC_L2_SR_UNLOCK(unit);
                    soc_mem_unlock(unit, L2Xm);
                    return rv;
                }
                is_tsn_or_sr_flow = TRUE;
            }
        }
#if defined(BCM_TSN_SR_SUPPORT)
        if (soc_feature(unit, soc_feature_tsn_sr)) {
            /* Increase reference count for the specified SR flowset */
            if (l2addr->sr_flowset != 0) {
                rv = bcmi_esw_tsn_sr_flowset_ref_inc(unit, l2addr->sr_flowset);
                if (rv < 0) {
                    SOC_L2_SR_UNLOCK(unit);
                    soc_mem_unlock(unit, L2Xm);
                    return rv;
                }
                is_tsn_or_sr_flow = TRUE;
            }
        }
#endif /* BCM_TSN_SR_SUPPORT */
        if (is_tsn_or_sr_flow == TRUE) {
            /* Update the TSN/SR flowset ref cnt bitmap for L2X reference. */
            /* Get L2 index */
            rv = soc_mem_generic_lookup(unit, L2Xm, MEM_BLOCK_ANY, 0,
                                        &l2x_entry, &l2x_lookup, &l2_index);
            if (rv < 0) {
                SOC_L2_SR_UNLOCK(unit);
                soc_mem_unlock(unit, L2Xm);
                return rv;
            }
            rv = soc_l2x_sr_ref_sync(unit, l2_index);
            if (rv < 0) {
                SOC_L2_SR_UNLOCK(unit);
                soc_mem_unlock(unit, L2Xm);
                return rv;
            }
        }
    }
#endif /* BCM_TSN_SUPPORT */

    SOC_L2_SR_UNLOCK(unit);
    soc_mem_unlock(unit, L2Xm);

    if (BCM_SUCCESS(rv) && enable_ppa_war) {
        SOC_CONTROL(unit)->l2x_ppa_bypass = TRUE;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_SUCCESS(rv)) {
        if (exist_in_ext_l2) {
            soc_mem_lock(unit, EXT_L2_ENTRYm);
            limit_counted =
                ppa_info[ext_l2_index].data & _SOC_TR_L2E_LIMIT_COUNTED;
            rv1 = soc_mem_generic_delete(unit, EXT_L2_ENTRYm, MEM_BLOCK_ANY,
                                         0, &ext_l2_entry, &ext_l2_lookup,
                                         &ext_l2_index);
            if (SOC_SUCCESS(rv1)) {
                if (!SOC_CONTROL(unit)->l2x_group_enable) {
                    mb_index =
                        soc_mem_field32_get(unit, EXT_L2_ENTRYm,
                                            &ext_l2_lookup, MAC_BLOCK_INDEXf);
                    _bcm_mac_block_delete(unit, mb_index);
                }
                if (limit_counted) {
                    (void)soc_triumph_learn_count_update(unit, &ext_l2_lookup,
                                                         TRUE, -1);
                }
            }
            soc_mem_unlock(unit, EXT_L2_ENTRYm);
        }
    }
#endif

done:
    if (rv < 0) {
        _bcm_mac_block_delete(unit, mb_index);

#if defined(BCM_TSN_SR_SUPPORT)
        if (soc_feature(unit, soc_feature_tsn_sr)) {
            if (mp_index != 0) {
                BCM_IF_ERROR_RETURN(
                    bcmi_esw_tsn_sr_mac_proxy_delete(unit, mp_index));
            }
        }
#endif /* BCM_TSN_SR_SUPPORT */
#if defined(BCM_GLOBAL_METER_SUPPORT)
        if (soc_feature(unit, soc_feature_global_meter) &&
            soc_feature(unit, soc_feature_global_meter_source_l2)) {
            _bcm_esw_policer_decrement_ref_count(unit, l2addr->policer_id);
        }
#endif /* BCM_GLOBAL_METER_SUPPORT */
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr_l2_addr_delete
 * Description:
 *      Delete an L2 address (MAC+VLAN) from the device
 * Parameters:
 *      unit - device unit
 *      mac  - MAC address to delete
 *      vid  - VLAN id
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_l2_addr_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid)
{
    bcm_l2_addr_t  l2addr;
    l2x_entry_t    l2x_entry, l2x_lookup;
#ifdef BCM_TRIUMPH_SUPPORT
    _soc_tr_l2e_ppa_info_t *ppa_info;
    ext_l2_entry_entry_t ext_l2_entry, ext_l2_lookup;
    int limit_counted;
#endif /* BCM_TRIUMPH_SUPPORT */
    int            l2_index, mb_index;
    int            rv;
#if defined(BCM_TSN_SUPPORT)
    int is_tsn_or_sr_flow = FALSE; /* TSN or SR related flow */
#if defined(BCM_TSN_SR_SUPPORT)
    int mp_index = 0; /* TSN SR MAC Proxy profile index */
#endif /* BCM_TSN_SR_SUPPORT */
#endif /* BCM_TSN_SUPPORT */
    soc_control_t  *soc = SOC_CONTROL(unit);

    bcm_l2_addr_t_init(&l2addr, mac, vid);

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || (SOC_IS_KATANAX(unit)) ||
        SOC_IS_TD2_TT2(unit) || soc_feature(unit, soc_feature_gh2_my_station)) {
        if (soc_mem_is_valid(unit, MY_STATION_TCAMm) && BCM_VLAN_VALID(vid)) {
              rv = bcm_td_l2_myStation_delete (unit, mac, vid, &l2_index);
            if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_FULL)) {
                   if (rv != BCM_E_NONE) {
                        return rv;
                   }
              }
         }
    }
#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND2_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm) &&
        soc_mem_index_count(unit, EXT_L2_ENTRYm)) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr_l2_to_ext_l2(unit, &ext_l2_entry, &l2addr, TRUE));
        soc_mem_lock(unit, EXT_L2_ENTRYm);
        rv = soc_mem_generic_lookup(unit, EXT_L2_ENTRYm, MEM_BLOCK_ANY, 0,
                                    &ext_l2_entry, NULL, &l2_index);
        if (BCM_SUCCESS(rv)) {
            ppa_info = SOC_CONTROL(unit)->ext_l2_ppa_info;
            limit_counted =
                ppa_info[l2_index].data & _SOC_TR_L2E_LIMIT_COUNTED;
            rv = soc_mem_generic_delete(unit, EXT_L2_ENTRYm, MEM_BLOCK_ANY, 0,
                                        &ext_l2_entry, &ext_l2_lookup, NULL);
            if (BCM_SUCCESS(rv)) {
                if (!SOC_CONTROL(unit)->l2x_group_enable) {
                    mb_index =
                        soc_mem_field32_get(unit, EXT_L2_ENTRYm,
                                            &ext_l2_lookup, MAC_BLOCK_INDEXf);
                    _bcm_mac_block_delete(unit, mb_index);
                }
                if (limit_counted) {
                    rv = soc_triumph_learn_count_update(unit, &ext_l2_lookup,
                                                        TRUE, -1);
                }
            }
        }
        if (rv != BCM_E_NOT_FOUND) {
            soc_mem_unlock(unit, EXT_L2_ENTRYm);
            return rv;
        }
        soc_mem_unlock(unit, EXT_L2_ENTRYm);
    }
#endif

    BCM_IF_ERROR_RETURN(_bcm_tr_l2_to_l2x(unit, &l2x_entry, &l2addr, TRUE));

    soc_mem_lock(unit, L2Xm);

#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
    /* Only support for GH2-B0 */
    if (soc_feature(unit, soc_feature_vxlan_lite_riot) ||
        SOC_IS_FIRELIGHT(unit)) {
        rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, &l2_index,
                            (void *)&l2x_entry, (void *)&l2x_lookup, 0);
        if (rv == BCM_E_NOT_FOUND) {
            if (_BCM_VPN_VFI_IS_SET(l2addr.vid)) {
                soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                                     GH2_L2_HASH_KEY_TYPE_VFI_MULTICAST);
                rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, &l2_index,
                                    (void *)&l2x_entry,
                                    (void *)&l2x_lookup, 0);
            }
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3  */
    {
        rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, &l2_index,
                            (void *)&l2x_entry, (void *)&l2x_lookup, 0);
#ifdef BCM_TRIDENT3_SUPPORT
        if (rv == BCM_E_NOT_FOUND) {
            if (soc_feature(unit, soc_feature_pvlan_on_vfi)) {
                soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                                     TR_L2_HASH_KEY_TYPE_VFI);
                rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, &l2_index,
                                    (void *)&l2x_entry,
                                    (void *)&l2x_lookup, 0);
            }
        }
#endif
    }
    if (BCM_E_NONE != rv) {
        soc_mem_unlock(unit, L2Xm);
        return rv;
    }

    if (!SOC_CONTROL(unit)->l2x_group_enable) {
        mb_index = soc_L2Xm_field32_get(unit, &l2x_lookup, MAC_BLOCK_INDEXf);
        _bcm_mac_block_delete(unit, mb_index);
    }

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        mp_index = soc_L2Xm_field32_get(unit, &l2x_lookup,
                                        SR_MAC_PROXY_PROFILE_PTRf);
        if (mp_index != 0) {
            rv = bcmi_esw_tsn_sr_mac_proxy_delete(unit, mp_index);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, L2Xm);
                return rv;
            }
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */

#if defined(BCM_GLOBAL_METER_SUPPORT)
    if (soc_feature(unit, soc_feature_global_meter) &&
        soc_feature(unit, soc_feature_global_meter_source_l2)) {
        bcm_policer_t policer_id;
        rv = _bcm_esw_get_policer_from_table(unit, L2Xm, 0, &l2x_lookup,
                                             &policer_id, TRUE);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
        _bcm_esw_policer_decrement_ref_count(unit, policer_id);
    }
#endif /* BCM_GLOBAL_METER_SUPPORT */

    if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
        soc_mem_unlock(unit, L2Xm);
        return BCM_E_RESOURCE;
    }
    SOC_L2_SR_LOCK(unit);
    rv = soc_mem_delete_return_old(unit, L2Xm, MEM_BLOCK_ANY,
                                   (void *)&l2x_entry, (void *)&l2x_entry);
    if (rv >= 0) {
        rv = soc_l2x_sync_delete(unit, (uint32 *) &l2x_lookup, l2_index, 0);
    }

#if defined(BCM_TSN_SUPPORT)
    /*
     * For manual L2 add and L2 delete:
     * - Update the TSN/SR flowset reference count bitmap for L2X reference.
     * (this is for L2MODE_POLL mode, not required for L2MOD_FIFO mode).
     * For packet learn and age (including ppa):
     * - the TSN/SR flowset reference count will be handled in
     * soc_l2x_callback with SOC_L2X_ENTRY_TSN_SR_FLOWSET_REF_CNT flag.
     */
    if (soc_feature(unit, soc_feature_tsn)) {
        /* Decrease reference count for the associated TSN flowset */
        if (soc_L2Xm_field32_get(unit, &l2x_entry, TSN_CIRCUIT_IDf) != 0) {
            rv = _bcm_tr_l2_from_l2x(unit, &l2addr, &l2x_entry);
            if (BCM_FAILURE(rv)) {
                SOC_L2_SR_UNLOCK(unit);
                SOC_L2_DEL_SYNC_UNLOCK(soc);
                soc_mem_unlock(unit, L2Xm);
                return rv;
            }
            rv = bcmi_esw_tsn_flowset_ref_dec(unit, l2addr.tsn_flowset);
            if (BCM_FAILURE(rv)) {
                SOC_L2_SR_UNLOCK(unit);
                SOC_L2_DEL_SYNC_UNLOCK(soc);
                soc_mem_unlock(unit, L2Xm);
                return rv;
            }
            is_tsn_or_sr_flow = TRUE;
        }
    }
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        /* Decrease reference count for the associated SR flowset */
        if (soc_L2Xm_field32_get(unit, &l2x_entry, SR_FLOW_IDf) != 0) {
            rv = _bcm_tr_l2_from_l2x(unit, &l2addr, &l2x_entry);
            if (BCM_FAILURE(rv)) {
                SOC_L2_SR_UNLOCK(unit);
                SOC_L2_DEL_SYNC_UNLOCK(soc);
                soc_mem_unlock(unit, L2Xm);
                return rv;
            }
            rv = bcmi_esw_tsn_sr_flowset_ref_dec(unit, l2addr.sr_flowset);
            if (BCM_FAILURE(rv)) {
                SOC_L2_SR_UNLOCK(unit);
                SOC_L2_DEL_SYNC_UNLOCK(soc);
                soc_mem_unlock(unit, L2Xm);
                return rv;
            }
            is_tsn_or_sr_flow = TRUE;
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */

    if (is_tsn_or_sr_flow == TRUE) {
        /* Update the TSN/SR flowset ref cnt bitmap for L2X reference. */
        rv = soc_l2x_sr_ref_sync(unit, l2_index);
    }
#endif /* BCM_TSN_SUPPORT */

    SOC_L2_SR_UNLOCK(unit);
    SOC_L2_DEL_SYNC_UNLOCK(soc);
    soc_mem_unlock(unit, L2Xm);

    return rv;
}

#ifdef BCM_TRIUMPH_SUPPORT
STATIC int
bcm_tr_l2_addr_ext_get(int unit, sal_mac_addr_t mac, bcm_vlan_t vid,
                   bcm_l2_addr_t *l2addr)
{
    bcm_l2_addr_t           l2addr_key;
    ext_l2_entry_entry_t    ext_l2_entry, ext_l2_lookup;
    int                     rv;

    bcm_l2_addr_t_init(&l2addr_key, mac, vid);

    BCM_IF_ERROR_RETURN(
        _bcm_tr_l2_to_ext_l2(unit, &ext_l2_entry, &l2addr_key, TRUE));

    soc_mem_lock(unit, EXT_L2_ENTRYm);
    rv = soc_mem_generic_lookup(unit, EXT_L2_ENTRYm, MEM_BLOCK_ANY, 0,
                               &ext_l2_entry, &ext_l2_lookup, NULL);
    soc_mem_unlock(unit, EXT_L2_ENTRYm);

    if (BCM_SUCCESS(rv)) {
        rv = _bcm_tr_l2_from_ext_l2(unit, l2addr, &ext_l2_lookup);
    }

    return rv;
}

#endif /* BCM_TRIUMPH_SUPPORT */

/*
 * Function:
 *      bcm_tr_l2_addr_get
 * Description:
 *      Given a MAC address and VLAN ID, check if the entry is present
 *      in the L2 table, and if so, return all associated information.
 * Parameters:
 *      unit - Device unit number
 *      mac - input MAC address to search
 *      vid - input VLAN ID to search
 *      check_l2_only - only check if the target exist in L2 table.
 *      l2addr - Pointer to bcm_l2_addr_t structure to receive results
 * Returns:
 *      BCM_E_NONE              Success (l2addr filled in)
 *      BCM_E_PARAM             Illegal parameter (NULL pointer)
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_NOT_FOUND Address not found (l2addr not filled in)
 */

int
_bcm_tr_l2_addr_get(int unit, sal_mac_addr_t mac, bcm_vlan_t vid,
    int check_l2_only, bcm_l2_addr_t *l2addr)
{
    bcm_l2_addr_t l2addr_key;
    l2x_entry_t  l2x_entry, l2x_lookup;
    int          l2_hw_index;
    int          rv;

    bcm_l2_addr_t_init(&l2addr_key, mac, vid);

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_KATANAX(unit) ||       
        SOC_IS_TD2_TT2(unit) || soc_feature(unit, soc_feature_gh2_my_station)) {
        if (soc_mem_is_valid(unit, MY_STATION_TCAMm) && (!check_l2_only)) {
              rv = bcm_td_l2_myStation_get (unit, mac, vid, l2addr);
              if (BCM_SUCCESS(rv)) {
                   return rv;
              }
         }
    }
#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND2_SUPPORT */

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow) &&
        (l2addr->flow_handle != 0)) {
        l2addr_key.flow_handle = l2addr->flow_handle;
        l2addr_key.flow_option_handle = l2addr->flow_option_handle;
        l2addr_key.num_of_fields = l2addr->num_of_fields;
        sal_memcpy(l2addr_key.logical_fields, l2addr->logical_fields,
                   l2addr->num_of_fields*sizeof(bcm_flow_logical_field_t));
    }
#endif

    BCM_IF_ERROR_RETURN(
        _bcm_tr_l2_to_l2x(unit, &l2x_entry, &l2addr_key, TRUE));

    soc_mem_lock(unit, L2Xm);

#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
    /* Only support for GH2-B0 */
    if (soc_feature(unit, soc_feature_vxlan_lite_riot) ||
        SOC_IS_FIRELIGHT(unit)) {
        rv = soc_mem_generic_lookup(unit, L2Xm, MEM_BLOCK_ANY, 0, &l2x_entry,
                                    &l2x_lookup, &l2_hw_index);
        if (rv == BCM_E_NOT_FOUND) {
            if (_BCM_VPN_VFI_IS_SET(vid)) {
                soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                                     GH2_L2_HASH_KEY_TYPE_VFI_MULTICAST);
                rv = soc_mem_generic_lookup(unit, L2Xm, MEM_BLOCK_ANY, 0,
                                            &l2x_entry, &l2x_lookup,
                                            &l2_hw_index);
            }
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3  */
    {
        rv = soc_mem_generic_lookup(unit, L2Xm, MEM_BLOCK_ANY, 0, &l2x_entry,
                                    &l2x_lookup, &l2_hw_index);
#ifdef BCM_TRIDENT3_SUPPORT
        if (rv == BCM_E_NOT_FOUND) {
            if (soc_feature(unit, soc_feature_pvlan_on_vfi)) {
                soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                                     TR_L2_HASH_KEY_TYPE_VFI);
                rv = soc_mem_generic_lookup(unit, L2Xm, MEM_BLOCK_ANY, 0,
                                            &l2x_entry, &l2x_lookup,
                                            &l2_hw_index);
            }
        }
#endif
    }

    /* If not found in Internal memory and external is available serach there */
#ifdef BCM_TRIUMPH_SUPPORT
    if (rv == BCM_E_NOT_FOUND && soc_mem_is_valid(unit, EXT_L2_ENTRYm) &&
        soc_mem_index_count(unit, EXT_L2_ENTRYm) > 0) {
        rv = bcm_tr_l2_addr_ext_get(unit, mac, vid, l2addr);
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
#endif /* BCM_TRIUMPH_SUPPORT */

    soc_mem_unlock(unit, L2Xm);

    if (check_l2_only) {
        return rv;
    }

    if (SOC_SUCCESS(rv)) {
        /* Retrieve the hit bit for TD2/TT2 */
#ifdef BCM_TRIDENT2_SUPPORT
        if (!SOC_IS_TOMAHAWKX(unit) && !SOC_IS_APACHE(unit) &&
            !SOC_IS_TRIDENT3X(unit) &&
                SOC_IS_TD2_TT2(unit)) {
            BCM_IF_ERROR_RETURN(
                _bcm_td2_l2_hit_retrieve(unit, &l2x_lookup, l2_hw_index));
        }
#endif /* BCM_TRIDENT2_SUPPORT */
        rv = _bcm_tr_l2_from_l2x(unit, l2addr, &l2x_lookup);
    }

    return rv;
}

int
bcm_tr_l2_addr_get(int unit, sal_mac_addr_t mac, bcm_vlan_t vid,
                   bcm_l2_addr_t *l2addr)
{
    return _bcm_tr_l2_addr_get(unit, mac, vid, FALSE, l2addr);
}

#ifdef BCM_WARM_BOOT_SUPPORT
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
STATIC int
_bcm_l2_station_non_tcam_reload(int unit, _bcm_l2_station_control_t *sc) 
{
    int index = 0, entry_index=0;
    int rv  = BCM_E_NONE; 
    _bcm_l2_station_entry_t   *s_ent_p;          /* Station table entry.      */
    uint32              mac_field[2];
    egr_olp_dgpp_config_entry_t *eolp_dgpp_cfg_buf;
    egr_olp_dgpp_config_entry_t *eolp_dgpp_cfg = NULL;
    int                       entry_mem_size;    /* Size of table entry. */
    egr_olp_config_entry_t    eolp_cfg;
    bcm_mac_t                 mac_addr; 
    uint32                       non_tcam_count = 0;
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        port_tab_entry_t          *ptab_buf;        
        port_tab_entry_t          *ptab;        

        entry_mem_size = sizeof(port_tab_entry_t);
        /* Allocate buffer to store the DMAed table entries. */
        ptab_buf = soc_cm_salloc(unit, entry_mem_size * sc->port_entries_total,
                "Port table entry buffer");
        if (NULL == ptab_buf) {
            return (BCM_E_MEMORY);
        }
        /* Initialize the entry buffer. */
        sal_memset(ptab_buf, 0, sizeof(entry_mem_size) * sc->port_entries_total);

        /* Read the table entries into the buffer. */
        rv = soc_mem_read_range(unit, PORT_TABm, MEM_BLOCK_ALL,
                0, (sc->port_entries_total - 1), ptab_buf);
        if (BCM_FAILURE(rv)) {
            if (ptab_buf) {
                soc_cm_sfree(unit, ptab_buf);
            }
            return rv;
        }

        /* Iterate over the port table entries. */
        for (index = 0; index < sc->port_entries_total; index++) {
            ptab = soc_mem_table_idx_to_pointer
                (unit, PORT_TABm, port_tab_entry_t *,
                 ptab_buf, index);

            soc_mem_field_get(unit, PORT_TABm, (uint32 *)ptab, 
                    MAC_ADDRESSf, mac_field);

            if ((mac_field[0] > 0) || (mac_field[1] > 0)) {
                s_ent_p = (_bcm_l2_station_entry_t *)
                    sal_alloc(sizeof(_bcm_l2_station_entry_t),
                            "Sw L2 station entry");
                if (NULL == s_ent_p) {
                    if (ptab_buf) {
                        soc_cm_sfree(unit, ptab_buf);
                    }
                    return (BCM_E_MEMORY);
                }
                sal_memset(s_ent_p, 0, sizeof(_bcm_l2_station_entry_t));

                s_ent_p->hw_index = index;
                soc_mem_mac_addr_get(unit, PORT_TABm, &ptab,
                        MAC_ADDRESSf, mac_addr);
                s_ent_p->flags = _BCM_L2_STATION_ENTRY_INSTALLED;
                s_ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_NON_TCAM;
                s_ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_OAM_MAC;
                s_ent_p->prio = 0;
                s_ent_p->sid = ++sc->last_allocated_sid;
                sc->entry_arr[sc->entries_total + index] = s_ent_p;
                sc->port_entries_free--;
                non_tcam_count++;
                LOG_DEBUG(BSL_LS_BCM_L2,
                        (BSL_META_U(unit,
                                    "port station, sid %d, hw_index %d, entry_index %d,port_station_max %d\n"),
                         s_ent_p->sid, s_ent_p->hw_index, sc->entries_total + index,sc->port_entries_total));

            }
        }
        if (ptab_buf) {
            soc_cm_sfree(unit, ptab_buf);
        }
    }
#endif
    entry_mem_size = sizeof(egr_olp_dgpp_config_entry_t);

    /* Allocate buffer to store the DMAed table entries. */
    eolp_dgpp_cfg_buf = soc_cm_salloc(unit, entry_mem_size * sc->olp_entries_total,
                              "Port table entry buffer");
    if (NULL == eolp_dgpp_cfg_buf) {
        return (BCM_E_MEMORY);
    }
    /* Initialize the entry buffer. */
    sal_memset(eolp_dgpp_cfg_buf, 0, 
               sizeof(entry_mem_size) * sc->olp_entries_total);

    /* Read the table entries into the buffer. */
    rv = soc_mem_read_range(unit, EGR_OLP_DGPP_CONFIGm, MEM_BLOCK_ALL,
                            0, (sc->olp_entries_total - 1), eolp_dgpp_cfg_buf);
    if (BCM_FAILURE(rv)) {
        if (eolp_dgpp_cfg_buf) {
            soc_cm_sfree(unit, eolp_dgpp_cfg_buf);
        }
        return rv;
    }
    /* Iterate over the olp mac entries. */
    for (index = 0; index < sc->olp_entries_total; index++) {
        eolp_dgpp_cfg = soc_mem_table_idx_to_pointer
                    (unit, EGR_OLP_DGPP_CONFIGm, egr_olp_dgpp_config_entry_t *,
                     eolp_dgpp_cfg_buf, index);

        soc_mem_field_get(unit, EGR_OLP_DGPP_CONFIGm, (uint32 *)eolp_dgpp_cfg, 
                                                  MACDAf, mac_field);

        if ((mac_field[0] > 0) || (mac_field[1] > 0)) {
            s_ent_p = (_bcm_l2_station_entry_t *)
                sal_alloc(sizeof(_bcm_l2_station_entry_t),
                                    "Sw L2 station entry");
            if (NULL == s_ent_p) {
                if (eolp_dgpp_cfg_buf) {
                    soc_cm_sfree(unit, eolp_dgpp_cfg_buf);
                }
                return (BCM_E_MEMORY);
            }
            sal_memset(s_ent_p, 0, sizeof(_bcm_l2_station_entry_t));

            s_ent_p->flags = _BCM_L2_STATION_ENTRY_INSTALLED;
            s_ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_NON_TCAM;
            s_ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_OLP_MAC;
            s_ent_p->prio = 0;
            s_ent_p->sid = ++sc->last_allocated_sid;
            s_ent_p->hw_index = index;
#if defined(BCM_KATANA2_SUPPORT) 
            if (SOC_IS_KATANA2(unit)) {
                entry_index = sc->entries_total + sc->port_entries_total + 
                                index;
            }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_fp_based_oam)) {
                entry_index = sc->entries_total + index;                
            }
#endif
            sc->entry_arr[entry_index] = s_ent_p;

            sc->olp_entries_free--;
            non_tcam_count++;
            LOG_DEBUG(BSL_LS_BCM_L2,
                    (BSL_META_U(unit,
                                "OLP station, sid %d, hw_index %d, entry_index %d, olp_station_max %d\n"),
                     s_ent_p->sid, s_ent_p->hw_index, entry_index, sc->olp_entries_total));

        }
    }

    if (eolp_dgpp_cfg_buf) {
        soc_cm_sfree(unit, eolp_dgpp_cfg_buf);
    }

    SOC_IF_ERROR_RETURN(
             READ_EGR_OLP_CONFIGm(unit, MEM_BLOCK_ANY, 0, &eolp_cfg));

    soc_mem_field_get(unit, EGR_OLP_CONFIGm, (uint32 *)&eolp_cfg, 
                                                  MACSAf, mac_field);
    if ((mac_field[0] > 0) || (mac_field[1] > 0)) {
        s_ent_p = (_bcm_l2_station_entry_t *)
                sal_alloc(sizeof(_bcm_l2_station_entry_t),
                                    "Sw L2 station entry");
        if (NULL == s_ent_p) {
            return (BCM_E_MEMORY);
        }
        sal_memset(s_ent_p, 0, sizeof(_bcm_l2_station_entry_t));
        soc_mem_mac_addr_get(unit, EGR_OLP_CONFIGm, &eolp_cfg,
                                             MACSAf, mac_addr);
        s_ent_p->flags = _BCM_L2_STATION_ENTRY_INSTALLED;
        s_ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_NON_TCAM;
        s_ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_XGS_MAC;
        s_ent_p->prio = 0;
        s_ent_p->sid = ++sc->last_allocated_sid;
        s_ent_p->hw_index = 0;
#if defined(BCM_KATANA2_SUPPORT) 
        if (SOC_IS_KATANA2(unit)) {
            entry_index = sc->entries_total + sc->port_entries_total +
                            sc->olp_entries_total + s_ent_p->hw_index;
        }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_fp_based_oam)) {
            entry_index = sc->entries_total + sc->olp_entries_total + s_ent_p->hw_index;
        }
#endif
        sc->entry_arr[entry_index] = s_ent_p;
        non_tcam_count++;
        LOG_DEBUG(BSL_LS_BCM_L2,
                (BSL_META_U(unit,
                            "XGS station, sid %d, hw_index %d, entry_index %d\n"),
                 s_ent_p->sid, s_ent_p->hw_index, entry_index));

    }
    if (non_tcam_count > 0) {
        /* Found NON TCAM valid entry, set OLP API ver */
        _BCM_SWITCH_OLP_APIV_SET(unit, _BCM_SWITCH_OLP_L2_STATION_API);
        LOG_DEBUG(BSL_LS_BCM_L2,
                (BSL_META_U(unit,
                            "setting _BCM_SWITCH_OLP_L2_STATION_API OLP API ver\n")));
    }
    return (BCM_E_NONE);
}
#endif /* BCM_KATANA2_SUPPORT OR BCM_TRIDENT2PLUS_SUPPORT*/
/*
 * Function:
 *      _bcm_tr_l2_reload_mbi
 * Description:
 *      Load MAC block info from hardware into software data structures.
 * Parameters:
 *      unit - StrataSwitch unit number.
 */
int
_bcm_tr_l2_reload_mbi(int unit)
{
    _bcm_mac_block_info_t *mbi = _mbi_entries[unit];
    l2x_entry_t         *l2x_entry, *l2x_table;
    mac_block_entry_t   mbe;
    int                 index, mb_index, l2x_size;
    pbmp_t              mb_pbmp;
#if defined(BCM_KATANA2_SUPPORT)
    uint32              fldbuf[SOC_PBMP_WORD_MAX];
    int                 i = 0;
    pbmp_t              mb_pbmp0, mb_pbmp1;
    int                 mask_w0_width = 0;
    int                 member;

    SOC_PBMP_CLEAR(mb_pbmp0);
    SOC_PBMP_CLEAR(mb_pbmp1);
#endif /* BCM_KATANA2_SUPPORT */

    /*
     * Refresh MAC Block information from the hardware tables.
     */

    for (mb_index = 0; mb_index < _mbi_num[unit]; mb_index++) {
        SOC_IF_ERROR_RETURN
            (READ_MAC_BLOCKm(unit, MEM_BLOCK_ANY, mb_index, &mbe));

        SOC_PBMP_CLEAR(mb_pbmp);

        if (soc_mem_field_valid(unit, MAC_BLOCKm, MAC_BLOCK_MASK_LOf)) {
#if defined(BCM_GREYHOUND2_SUPPORT)
                if (SOC_IS_GREYHOUND2(unit)) {
                    soc_mem_pbmp_field_get(unit, MAC_BLOCKm, &mbe, MAC_BLOCK_MASKf,
                                           &mb_pbmp);
                } else
#endif /* BCM_GREYHOUND2_SUPPORT */
                {
            SOC_PBMP_WORD_SET(mb_pbmp, 0,
                              soc_MAC_BLOCKm_field32_get(unit, &mbe, 
                                                         MAC_BLOCK_MASK_LOf));
                }
        } else if (soc_mem_field_valid(unit, MAC_BLOCKm, MAC_BLOCK_MASK_W0f)) {
#if defined(BCM_KATANA2_SUPPORT)
            if (SOC_IS_KATANA2(unit)) {
                mask_w0_width = soc_mem_field_length(unit, MAC_BLOCKm,
                                                     MAC_BLOCK_MASK_W0f);
                sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
                soc_MAC_BLOCKm_field_get(unit, &mbe,
                                         MAC_BLOCK_MASK_W0f, fldbuf);
                for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                    SOC_PBMP_WORD_SET(mb_pbmp0, i, fldbuf[i]);
                }
            } else
#endif /* BCM_KATANA2_SUPPORT */
            {
                SOC_PBMP_WORD_SET(mb_pbmp, 0,
                              soc_MAC_BLOCKm_field32_get(unit, &mbe, 
                                                         MAC_BLOCK_MASK_W0f));
            }
        }else {
            soc_mem_pbmp_field_set(unit, MAC_BLOCKm, &mbe, MAC_BLOCK_MASKf,
                                       &mb_pbmp); 
        }
        if (soc_mem_field_valid(unit, MAC_BLOCKm, MAC_BLOCK_MASK_HIf)) {
            SOC_PBMP_WORD_SET(mb_pbmp, 1,
                          soc_MAC_BLOCKm_field32_get(unit, &mbe, 
                                                     MAC_BLOCK_MASK_HIf));
        } else if (soc_mem_field_valid(unit, MAC_BLOCKm, MAC_BLOCK_MASK_W1f)) {
#if defined(BCM_KATANA2_SUPPORT)
            if (SOC_IS_KATANA2(unit)) {
                sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
                soc_MAC_BLOCKm_field_get(unit, &mbe,
                                         MAC_BLOCK_MASK_W0f, fldbuf);
                for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                    SOC_PBMP_WORD_SET(mb_pbmp1, i, fldbuf[i]);
                }
            } else
#endif /* BCM_KATANA2_SUPPORT */
            {
                SOC_PBMP_WORD_SET(mb_pbmp, 1,
                          soc_MAC_BLOCKm_field32_get(unit, &mbe, 
                                                     MAC_BLOCK_MASK_W1f));
            }
        }
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            SOC_PBMP_OR(mb_pbmp, mb_pbmp0);
            SOC_PBMP_ITER(mb_pbmp1, member) {
               SOC_PBMP_PORT_ADD(mb_pbmp, (member + mask_w0_width));
            }
        }
#endif /* BCM_KATANA2_SUPPORT */

        BCM_PBMP_ASSIGN(mbi[mb_index].mb_pbmp, mb_pbmp);
    }

    if (!SOC_CONTROL(unit)->l2x_group_enable) {
        l2x_size = sizeof(l2x_entry_t) * soc_mem_index_count(unit, L2Xm);
        l2x_table = soc_cm_salloc(unit, l2x_size, "l2 reload");
        if (l2x_table == NULL) {
            return BCM_E_MEMORY;
        }

        memset((void *)l2x_table, 0, l2x_size);
        if (soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY,
                               soc_mem_index_min(unit, L2Xm),
                               soc_mem_index_max(unit, L2Xm),
                               l2x_table) < 0) {
            soc_cm_sfree(unit, l2x_table);
            return SOC_E_INTERNAL;
        }

        for (index = soc_mem_index_min(unit, L2Xm);
             index <= soc_mem_index_max(unit, L2Xm); index++) {

             l2x_entry = soc_mem_table_idx_to_pointer(unit, L2Xm,
                                                      l2x_entry_t *,
                                                      l2x_table, index);
             if (soc_feature(unit, soc_feature_base_valid)) {
                 if (!soc_L2Xm_field32_get(unit, l2x_entry, BASE_VALIDf)) {
                     continue;
                 }
             } else {
             if (!soc_L2Xm_field32_get(unit, l2x_entry, VALIDf)) {
                 continue;
             }
             }
  
             mb_index = soc_L2Xm_field32_get(unit, l2x_entry, MAC_BLOCK_INDEXf);
             mbi[mb_index].ref_count++;
        }
        soc_cm_sfree(unit, l2x_table);
    }

    return BCM_E_NONE;
}

#if defined(BCM_TRIUMPH_SUPPORT) /* BCM_TRIUMPH_SUPPORT */
 
/*
 * Function:
 *     _bcm_tr_l2_station_entry_reload
 * Purpose:
 *     Re-construct L2 station control structure software state
 *     from installed hardware entries
 * Parameters:
 *     unit       - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_tr_l2_station_entry_reload(int unit, int overlay)
{
    _bcm_l2_station_control_t *sc;               /* Pointer to station        */
                                                 /* control structure.        */
    int                       rv;                /* Operation return status.  */
    uint32                    *tcam_buf = NULL;  /* Buffer to store TCAM      */
                                                 /* table entries.            */
    soc_mem_t                 tcam_mem;          /* Memory name.              */
    int                       entry_mem_size;    /* Size of TCAM table entry. */
    int                       index;             /* Table index iterator.     */
    _bcm_l2_station_entry_t   *s_ent_p;          /* Station table entry.      */
    mpls_station_tcam_entry_t *mpls_station_ent; /* Table entry pointer.      */
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    my_station_tcam_entry_t   *my_station_ent = NULL; /* Table entry pointer. */
#endif

#ifdef BCM_RIOT_SUPPORT
    if (overlay == _BCM_L2_STATION_ENTRY_OVERLAY) {
        entry_mem_size = sizeof(my_station_tcam_2_entry_t);
    } else
#endif /* BCM_RIOT_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_KATANAX(unit)
        || SOC_IS_TRIDENT(unit)
        || SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit)
        || soc_feature(unit, soc_feature_gh2_my_station)) {

        entry_mem_size = sizeof(my_station_tcam_entry_t);

    } else 
#endif

    if (SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH(unit)
        || SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit)
        || SOC_IS_VALKYRIE(unit) || SOC_IS_VALKYRIE2(unit)
        || SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit)) {

        entry_mem_size = sizeof(mpls_station_tcam_entry_t);

    } else {
        entry_mem_size = 0;
    }

    if (0 == entry_mem_size) {
        return (BCM_E_INTERNAL);
    }

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

    SC_LOCK(sc);

    rv = _bcm_l2_station_tcam_mem_get(unit, overlay, &tcam_mem);
    if (BCM_FAILURE(rv)) {
        SC_UNLOCK(sc);
        return (rv);
    }

    /* Allocate buffer to store the DMAed table entries. */
    tcam_buf = soc_cm_salloc(unit, entry_mem_size * sc->entries_total,
                              "STATION TCAM buffer");
    if (NULL == tcam_buf) {
        SC_UNLOCK(sc);
        return (BCM_E_MEMORY);
    }
    /* Initialize the entry buffer. */
    sal_memset(tcam_buf, 0, sizeof(entry_mem_size) * sc->entries_total);

    if (!SOC_MEM_IS_VALID(unit, tcam_mem)) {
        /* Added in for low latency mode since tcam_mem is not valid */
        goto cleanup;
    } 

    /* Read the table entries into the buffer. */
    rv = soc_mem_read_range(unit, tcam_mem, MEM_BLOCK_ALL,
                            0, (sc->entries_total - 1), tcam_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Iterate over the table entries. */
    for (index = 0; index < sc->entries_total; index++) {
#ifdef BCM_RIOT_SUPPORT
        if (overlay == _BCM_L2_STATION_ENTRY_OVERLAY) {
            my_station_ent = soc_mem_table_idx_to_pointer(unit,
                                 tcam_mem, my_station_tcam_entry_t *,
                                 tcam_buf, index);
            if (0 == soc_MY_STATION_TCAM_2m_field32_get
                        (unit, my_station_ent, VALIDf)) {
                continue;
            }
        } else
#endif /* BCM_RIOT_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        if (SOC_IS_KATANAX(unit)
            || SOC_IS_TRIDENT(unit)
            || SOC_IS_TRIUMPH3(unit)
            || SOC_IS_TD2_TT2(unit)
            || soc_feature(unit, soc_feature_gh2_my_station)) {

            my_station_ent
                = soc_mem_table_idx_to_pointer
                    (unit, tcam_mem, my_station_tcam_entry_t *,
                     tcam_buf, index);

            if (0 == soc_MY_STATION_TCAMm_field32_get
                        (unit, my_station_ent, VALIDf)) {
                continue;
            }

        } else
#endif
        if (SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH(unit)
            || SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit)
            || SOC_IS_VALKYRIE(unit) || SOC_IS_VALKYRIE2(unit)
            || SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit)) {

            mpls_station_ent
                = soc_mem_table_idx_to_pointer
                    (unit, tcam_mem, mpls_station_tcam_entry_t *,
                     tcam_buf, index);

            if (0 == soc_MPLS_STATION_TCAMm_field32_get
                        (unit, mpls_station_ent, VALIDf)) {
                continue;
            }
        }

        s_ent_p
            = (_bcm_l2_station_entry_t *)
                sal_alloc(sizeof(_bcm_l2_station_entry_t),
                          "Sw L2 station entry");
        if (NULL == s_ent_p) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        sal_memset(s_ent_p, 0, sizeof(_bcm_l2_station_entry_t));

#ifdef BCM_RIOT_SUPPORT
        if (overlay == _BCM_L2_STATION_ENTRY_OVERLAY) {
            s_ent_p->sid = ++sc->last_allocated_sid_2;
            s_ent_p->prio = (sc->entries_total_2 - index);
            sc->entries_free_2--;
            sc->entry_count_2++;
            sc->entry_arr_2[index] = s_ent_p;
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_pt2pt_access_vlan_check)) {
                if (my_station_ent != NULL) {
                    if (soc_mem_field32_get(unit, tcam_mem, my_station_ent,
                               MY_STATION_FORWARDING_FIELD_OVERLAY_TYPEf)) {
                        s_ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_TCAM_2;
                    }
                }
            }
#endif /* BCM_TRIDENT3_SUPPORT */
        } else
#endif /* BCM_RIOT_SUPPORT */
        {
            s_ent_p->sid = ++sc->last_allocated_sid;
            s_ent_p->prio = (sc->entries_total - index);
            sc->entries_free--;
            sc->entry_count++;
            sc->entry_arr[index] = s_ent_p;
        }
        s_ent_p->hw_index = index;
        s_ent_p->flags |= _BCM_L2_STATION_ENTRY_INSTALLED;
    }
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (SOC_IS_KATANA2(unit) || 
            soc_feature(unit, soc_feature_fp_based_oam)) {

        /* 
         * In case of KT2,SB2 and TD2+ NONTCAM STATION related to OAM, OLP, XGS can be programmed
         * by either of bcm_l2_station_xxx API or  bcm_switch_olp_l2_addr_xxx
         *
         * Do not process NONTCAM entry if these were not programmed by bcm_l2_station_xxx
         */

        if (_BCM_SWITCH_OLP_APIV(unit) == _BCM_SWITCH_OLP_TRUE_API) {
            LOG_DEBUG(BSL_LS_BCM_L2,
                    (BSL_META_U(unit,
                                "bcm_switch_olp_l2_addr_xxx are used already,DO NOT recover NONTCAM ENTRY\n")));
            /* Fall thru */
        } else {
            /* Go ahead to recover NON TCAM entry */
            rv = _bcm_l2_station_non_tcam_reload(unit, sc); 
        }
    }
#endif 

cleanup:

    SC_UNLOCK(sc);

    if (tcam_buf) {
        soc_cm_sfree(unit, tcam_buf);
    }

    return (rv);
}

/*
 * Function:
 *     _bcm_tr_l2_station_reload
 * Purpose:
 *     Re-construct L2 station control structure software state
 *     from installed hardware entries
 * Parameters:
 *     unit       - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_tr_l2_station_reload(int unit)
{
    int rv;

    rv = _bcm_tr_l2_station_entry_reload(unit,
             _BCM_L2_STATION_ENTRY_UNDERLAY);
    BCM_IF_ERROR_RETURN(rv);

#ifdef BCM_RIOT_SUPPORT
    if (soc_feature(unit, soc_feature_riot) ||
        _bcm_l2_p2p_access_vlan_check_enabled(unit)) {
        rv = _bcm_tr_l2_station_entry_reload(unit,
                 _BCM_L2_STATION_ENTRY_OVERLAY);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_RIOT_SUPPORT */

   return BCM_E_NONE;
}


#endif /* !BCM_TRIUMPH_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */



/*
 * Function:
 *      _tr_l2x_delete_all
 * Purpose:
 *      Clear the L2 table by invalidating entries.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      static_too - if TRUE, delete static and non-static entries;
 *                   if FALSE, delete only non-static entries
 * Returns:
 *      SOC_E_XXX
 */

static int
_tr_l2x_delete_all(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int copyno;
    int index_min, index_max, index, mem_max;
    l2_entry_only_entry_t *l2x_entry;
    int rv = SOC_E_NONE;
    int *buffer = NULL;
    int mem_size, idx;
    int modified;
    uint32 key_type;

#if defined(BCM_TRIUMPH_SUPPORT)
    _soc_tr_l2e_ppa_info_t    *ppa_info = NULL;
#endif

#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_l2_bulk_control)) {
#ifdef BCM_TOMAHAWK_SUPPORT
        if (soc_feature(unit, soc_feature_l2_bulk_unified_table)) {
            l2_bulk_entry_t match_mask;
            l2_bulk_entry_t match_data;
            int field_len;
    
            sal_memset(&match_mask, 0, sizeof(match_mask));
            sal_memset(&match_data, 0, sizeof(match_data));
    
            if (soc_feature(unit, soc_feature_base_valid)) {
                soc_mem_field32_set(unit, L2_BULKm, &match_mask, BASE_VALIDf, 1);
                soc_mem_field32_set(unit, L2_BULKm, &match_data, BASE_VALIDf, 1);
            } else {
            soc_mem_field32_set(unit, L2_BULKm, &match_mask, VALIDf, 1);
            soc_mem_field32_set(unit, L2_BULKm, &match_data, VALIDf, 1);
            }
    
            field_len = soc_mem_field_length(unit, L2_BULKm, KEY_TYPEf);
            soc_mem_field32_set(unit, L2_BULKm, &match_mask, KEY_TYPEf,
                                (1 << field_len) - 1);
    
            soc_mem_lock(unit, L2Xm);
            rv = soc_reg_field32_modify(unit, L2_BULK_CONTROLr, REG_PORT_ANY,
                                        ACTIONf, 1);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, L2Xm);
                return rv;
            }
            rv = soc_reg_field32_modify(unit, L2_BULK_CONTROLr, REG_PORT_ANY,
                               NUM_ENTRIESf, soc_mem_index_count(unit, L2Xm));
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, L2Xm);
                return rv;
            }
            if (BCM_SUCCESS(rv)) {
                rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL,
                         _BCM_L2_BULK_MATCH_MASK_INX, &match_mask);
            }
    
            /* Remove all KEY_TYPE 0 entries */
            if (BCM_SUCCESS(rv)) {
                rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 
                      _BCM_L2_BULK_MATCH_DATA_INX, &match_data);
                if (BCM_SUCCESS(rv)) {
                    rv = soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
                }
            }
    
            /* Remove all KEY_TYPE 3 entries */
            if (BCM_SUCCESS(rv)) {
                soc_mem_field32_set(unit, L2_BULKm, &match_data,
                                    KEY_TYPEf, 3);
                rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 
                      _BCM_L2_BULK_MATCH_DATA_INX, &match_data);
                if (BCM_SUCCESS(rv)) {
                    rv = soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
                }
            }
        } else
#endif /* BCM_TOMAHAWK_SUPPORT */
        {
            l2_bulk_match_mask_entry_t match_mask;
            l2_bulk_match_data_entry_t match_data;
            int field_len;
    
            sal_memset(&match_mask, 0, sizeof(match_mask));
            sal_memset(&match_data, 0, sizeof(match_data));
    
            soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, 
                                &match_mask, VALIDf, 1);
            soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, 
                                &match_data, VALIDf, 1);
    
            field_len = soc_mem_field_length(unit, L2_BULK_MATCH_MASKm, 
                            KEY_TYPEf);
            soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask, 
                                KEY_TYPEf, (1 << field_len) - 1);
    
            soc_mem_lock(unit, L2Xm);
            rv = soc_reg_field32_modify(unit, L2_BULK_CONTROLr, REG_PORT_ANY,
                                        ACTIONf, 1);
            if (BCM_SUCCESS(rv)) {
                rv = WRITE_L2_BULK_MATCH_MASKm(unit, MEM_BLOCK_ALL, 0,
                                               &match_mask);
            }
    
            /* Remove all KEY_TYPE 0 entries */
            if (BCM_SUCCESS(rv)) {
                rv = WRITE_L2_BULK_MATCH_DATAm(unit, MEM_BLOCK_ALL, 0,
                                               &match_data);
                if (BCM_SUCCESS(rv)) {
                    rv = soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
                }
            }
    
            /* Remove all KEY_TYPE 3 entries */
            if (BCM_SUCCESS(rv)) {
                soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, &match_data,
                                    KEY_TYPEf, 3);
                rv = WRITE_L2_BULK_MATCH_DATAm(unit, MEM_BLOCK_ALL, 0,
                                               &match_data);
                if (BCM_SUCCESS(rv)) {
                    rv = soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
                }
            }
        }
    } else
#endif /* BCM_TRIDENT_SUPPORT */
    {
        index_min = soc_mem_index_min(unit, L2_ENTRY_ONLYm);
        mem_max = soc_mem_index_max(unit, L2_ENTRY_ONLYm);
        mem_size =  DEFAULT_L2DELETE_CHUNKS * sizeof(l2_entry_only_entry_t);
    
        buffer = soc_cm_salloc(unit, mem_size, "L2_ENTRY_ONLY_delete");
        if (NULL == buffer) {
            return SOC_E_MEMORY;
        }

        soc_mem_lock(unit, L2Xm);
        for (idx = index_min; idx < mem_max; idx += DEFAULT_L2DELETE_CHUNKS) {
            index_max = idx + DEFAULT_L2DELETE_CHUNKS - 1;
            if ( index_max > mem_max) {
                index_max = mem_max;
            }
            if ((rv = soc_mem_read_range(unit, L2_ENTRY_ONLYm, MEM_BLOCK_ALL,
                                         idx, index_max, buffer)) < 0 ) {
                soc_cm_sfree(unit, buffer);
                soc_mem_unlock(unit, L2Xm);
                return rv;
            }
            modified = FALSE;
            for (index = 0; index < DEFAULT_L2DELETE_CHUNKS; index++) {
                l2x_entry =
                    soc_mem_table_idx_to_pointer(unit, L2_ENTRY_ONLYm,
                                                 l2_entry_only_entry_t *,
                                                 buffer, index);
                if (!soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry, VALIDf)) {
                    continue;
                }
                key_type = soc_L2_ENTRY_ONLYm_field32_get(unit, l2x_entry,
                                                          KEY_TYPEf);
                if (key_type ==  TR_L2_HASH_KEY_TYPE_BRIDGE ||
                    key_type == TR_L2_HASH_KEY_TYPE_VFI) {
                    sal_memcpy(l2x_entry,
                               soc_mem_entry_null(unit, L2_ENTRY_ONLYm),
                               sizeof(l2_entry_only_entry_t));
                    modified = TRUE;
                }
            }
            if (!modified) {
                continue;
            }
            if ((rv = soc_mem_write_range(unit, L2_ENTRY_ONLYm, MEM_BLOCK_ALL, 
                                          idx, index_max, buffer)) < 0) {
                soc_cm_sfree(unit, buffer);
                soc_mem_unlock(unit, L2Xm);
                return rv;
            }
        }
        soc_cm_sfree(unit, buffer);
    }

    if (soc->arlShadow != NULL) {
        sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
        (void) shr_avl_delete_all(soc->arlShadow);
        sal_mutex_give(soc->arlShadowMutex);
    }

    copyno = SOC_MEM_BLOCK_ANY(unit, L2Xm);
    if (SOC_MEM_BLOCK_VALID(unit, L2Xm, copyno)) {
        SOP_MEM_STATE(unit, L2Xm).count[copyno] = 0;
    }

    /* Clear external L2 table if it exists */
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm) &&
        soc_mem_index_count(unit, EXT_L2_ENTRYm)) {
        rv = soc_mem_clear(unit, EXT_L2_ENTRY_TCAMm,
                                          MEM_BLOCK_ALL, TRUE);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2Xm);
            return (rv);
        }
        rv = soc_mem_clear(unit, EXT_L2_ENTRY_DATAm,
                                          MEM_BLOCK_ALL, TRUE);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2Xm);
            return (rv);
        }

#if defined(BCM_TRIUMPH_SUPPORT)
        ppa_info = SOC_CONTROL(unit)->ext_l2_ppa_info;
        if (NULL != ppa_info) {
            rv = _soc_tr_l2e_ppa_init(unit);
            if (SOC_FAILURE(rv)) {
                soc_mem_unlock(unit, L2Xm);
                return (rv);
            }
        }
#endif
    }
#if defined(BCM_HURRICANE3_SUPPORT)
    /* Clear L2 overflow table if it exists */
    if (soc_mem_is_valid(unit, L2_ENTRY_OVERFLOWm) &&
        soc_mem_index_count(unit, L2_ENTRY_OVERFLOWm)) {
        rv = soc_mem_clear(unit, L2_ENTRY_OVERFLOWm,
                                          MEM_BLOCK_ALL, TRUE);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2Xm);
            return (rv);
        }
    }
#endif /* BCM_HURRICANE3_SUPPORT */

    soc_mem_unlock(unit, L2Xm);

    return rv;
}

#if defined(BCM_TRIDENT3_SUPPORT)
/*
 * Function:
 *      _bcm_td3_lport_cml_config
 * Description:
 *      save/restore the fields in the LPORT Profile Table
 * Parameters:
 *      unit          - (IN) Device number
 *      port          - (IN) Port number (NON gport)
 *      save          - (IN) True for save and false for resetore purpose
 *      save_cml      - (IN/OUT) CML_FLAGS_NEW
 *      save_cml_move - (IN/OUT) CML_FLAGS_MOVE
 */
 int
_bcm_td3_lport_cml_config(int unit, bcm_port_t port, int save,
                           int *save_cml, int *save_cml_move)
{
    int rv;
    int src_index;
    uint32 src_ent[SOC_MAX_MEM_FIELD_WORDS];
    lport_tab_entry_t lport_entry;
    uint32 lport_index, old_lport_index;
    soc_mem_t mem = LPORT_TABm;
    void *entries[2], *entry;
    soc_mem_t src_mem = ING_DEVICE_PORTm;

    /* Make sure port module is initialized. */
    soc_mem_lock(unit, src_mem);
    src_index = port;

    rv = soc_mem_read(unit, src_mem, MEM_BLOCK_ANY, src_index, src_ent);
    if(BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, src_mem);
        return rv;
    }

    old_lport_index =
        soc_mem_field32_get(unit, src_mem, src_ent, LPORT_PROFILE_IDXf);

    entries[0] = &lport_entry;
    rv = _bcm_lport_ind_profile_entry_get(unit, old_lport_index, 1, entries);
    if(BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, src_mem);
        return rv;
    }

    /* Update fields */
    entry = entries[0];
    if(save) {
        *save_cml=
            soc_mem_field32_get(unit, mem, entry, CML_FLAGS_NEWf);
        *save_cml_move=
            soc_mem_field32_get(unit, mem, entry, CML_FLAGS_MOVEf);
        soc_mem_field32_set(unit, mem, entry, CML_FLAGS_NEWf, *save_cml &0x03);
        soc_mem_field32_set(unit, mem, entry, CML_FLAGS_MOVEf, *save_cml_move & 0x03);
    } else { /*restore*/
        soc_mem_field32_set(unit, mem, entry, CML_FLAGS_NEWf, *save_cml);
        soc_mem_field32_set(unit, mem, entry, CML_FLAGS_MOVEf, *save_cml_move);
    }

    rv = _bcm_lport_ind_profile_entry_add(unit, entries, 1, &lport_index);
    if(BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, src_mem);
        return rv;
    }

    /* Update Source Table LPORT profile index  */
    soc_mem_field32_set(unit, src_mem, src_ent, LPORT_PROFILE_IDXf,lport_index);
    rv = soc_mem_write(unit, src_mem, MEM_BLOCK_ALL, src_index, src_ent);
    soc_mem_unlock(unit, src_mem);
    if(BCM_FAILURE(rv)) {
        return rv;
    }

    /*
     * Entry must be 'deleted' because the mem profile 'add' routine
     * always increments count even though the same entry is used.
     */
    rv = _bcm_lport_ind_profile_entry_delete(unit, old_lport_index);

    return rv;
}
#endif /*BCM_TRIDENT3_SUPPORT*/
/*
 * Function:
 *      bcm_tr_l2_init
 * Description:
 *      Initialize chip-dependent parts of L2 module
 * Parameters:
 *      unit - StrataSwitch unit number.
 */

int
bcm_tr_l2_init(int unit)
{
    int         was_running = FALSE;
    int         rv = BCM_E_NONE, i = 0;
    uint32      flags;
    sal_usecs_t interval;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    uint64      regval64;
    int         my_station_config = 0;
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    COMPILER_64_ZERO(regval64);
    /* Enable L2 entry used as my station hit */
    my_station_config = soc_property_get(unit,
                                   spn_L2_ENTRY_USED_AS_MY_STATION, 0);
    if (soc_feature(unit, soc_feature_l2_entry_used_as_my_station)) {
        SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &regval64));
        if (my_station_config != soc_reg64_field32_get(unit, ING_CONFIG_64r,
            regval64, L2_ENTRY_USED_AS_MY_STATIONf)) {
                BCM_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, ING_CONFIG_64r, REG_PORT_ANY,
                             L2_ENTRY_USED_AS_MY_STATIONf, my_station_config));
        }
    }
#endif

    if (soc_l2x_running(unit, &flags, &interval)) { 	 
        was_running = TRUE; 	 
        BCM_IF_ERROR_RETURN(soc_l2x_stop(unit)); 	 
    }

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RCPU_ONLY(unit) &&
        !SOC_HW_RESET(unit)) {
        if (!(SAL_BOOT_QUICKTURN || 
              ((SAL_BOOT_SIMULATION || SAL_BOOT_BCMSIM || SAL_BOOT_XGSSIM) 
               && !SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TRIDENT3X(unit))
             )) {
            _tr_l2x_delete_all(unit);
        }
    }

    if (_mbi_entries[unit] != NULL) {
        sal_free(_mbi_entries[unit]);
        _mbi_entries[unit] = NULL;
    }

    _mbi_num[unit] = (SOC_MEM_INFO(unit, MAC_BLOCKm).index_max -
                      SOC_MEM_INFO(unit, MAC_BLOCKm).index_min + 1);
    _mbi_entries[unit] = sal_alloc(_mbi_num[unit] *
                                   sizeof(_bcm_mac_block_info_t),
                                   "BCM L2X MAC blocking info");
    if (!_mbi_entries[unit]) {
        return BCM_E_MEMORY;
    }
    sal_memset(_mbi_entries[unit], 0, _mbi_num[unit] * sizeof(_bcm_mac_block_info_t));

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)
        && !SOC_IS_HURRICANEX(unit) && !SOC_IS_GREYHOUND(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_tr_l2_station_control_init(unit));
    }
#endif

#if defined(BCM_TRIUMPH_SUPPORT)
    if (soc_feature(unit, soc_feature_mysta_profile)) {
        BCM_IF_ERROR_RETURN(_bcm_l2_mysta_profile_init(unit));
    }
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_tr_l2_reload_mbi(unit));

#if defined(BCM_TRIUMPH_SUPPORT)
        if (SOC_IS_TR_VL(unit)
            && !SOC_IS_HURRICANEX(unit) && !SOC_IS_GREYHOUND(unit)) {
            BCM_IF_ERROR_RETURN(_bcm_tr_l2_station_reload(unit));
        }
#endif
    }
#endif

     /* Control structure for L2_BULK matched traverse */
    if (soc_feature(unit, soc_feature_l2_bulk_control)) {
        sal_sem_t sem;
        if (_bcm_l2_match_ctrl[unit] == NULL) {
            _bcm_l2_match_ctrl[unit] = sal_alloc(sizeof(_bcm_l2_match_ctrl_t),
                                                 "matched_traverse control");
            if (_bcm_l2_match_ctrl[unit] == NULL) {
                return BCM_E_MEMORY;
            }
            sem = sal_sem_create("L2_matched", sal_sem_BINARY, 0);
            if (sem == NULL) {
                sal_free(_bcm_l2_match_ctrl[unit]);
                _bcm_l2_match_ctrl[unit] = NULL;
                return BCM_E_MEMORY;
            }
        } else {
            sem = _bcm_l2_match_ctrl[unit]->sem;
        }

        sal_memset(_bcm_l2_match_ctrl[unit], 0, sizeof(_bcm_l2_match_ctrl_t));
        _bcm_l2_match_ctrl[unit]->sem = sem;
    }

    rv = _bcm_tr_l2_memacc_init(unit);
    if (BCM_FAILURE(rv)) {
        if (soc_feature(unit, soc_feature_l2_bulk_control)) {
            if (_bcm_l2_match_ctrl[unit] != NULL) {
                if (_bcm_l2_match_ctrl[unit]->sem != NULL) {
                    sal_sem_destroy(_bcm_l2_match_ctrl[unit]->sem);
                }
                for (i = 0; i < _BCM_L2_MATCH_ENTRY_BUF_COUNT; i++) {
                    if (_bcm_l2_match_ctrl[unit]->l2x_entry_buf[i] != NULL) {
                        sal_free(_bcm_l2_match_ctrl[unit]->l2x_entry_buf[i]);
                        _bcm_l2_match_ctrl[unit]->l2x_entry_buf[i] = NULL;
                    }
                }
                sal_free(_bcm_l2_match_ctrl[unit]);
                _bcm_l2_match_ctrl[unit] = NULL;
            }
        }
        return rv;
    }

    /* bcm_l2_register clients */
    
    soc_l2x_register(unit, _bcm_l2_register_callback, NULL);
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_l2x_cml_register(unit, _bcm_td3_lport_cml_config);
    }
#endif /*BCM_TRIDENT3_SUPPORT*/
    if (was_running) {
        interval = (SAL_BOOT_BCMSIM && !SOC_IS_TRIDENT3X(unit)) ? \
                    BCMSIM_L2XMSG_INTERVAL : interval;
        soc_l2x_start(unit, flags, interval);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr_l2_term
 * Description:
 *      Finalize chip-dependent parts of L2 module
 * Parameters:
 *      unit - StrataSwitch unit number.
 */

int
bcm_tr_l2_term(int unit)
{
    int i = 0;
    if (_mbi_entries[unit] != NULL) {
        sal_free(_mbi_entries[unit]);
        _mbi_entries[unit] = NULL;
    }

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_tr_l2_station_control_deinit(unit));
    }

    if (soc_feature(unit, soc_feature_mysta_profile)) {
        BCM_IF_ERROR_RETURN(_bcm_l2_mysta_profile_detach(unit));
    }
#endif

    if (_bcm_l2_match_ctrl[unit] != NULL) {
       if (_bcm_l2_match_ctrl[unit]->sem != NULL) {
          sal_sem_destroy(_bcm_l2_match_ctrl[unit]->sem);
       }
       for (i = 0; i < _BCM_L2_MATCH_ENTRY_BUF_COUNT; i++) {
           if (_bcm_l2_match_ctrl[unit]->l2x_entry_buf[i] != NULL) {
               sal_free(_bcm_l2_match_ctrl[unit]->l2x_entry_buf[i]);
               _bcm_l2_match_ctrl[unit]->l2x_entry_buf[i] = NULL;
           }
       }
       sal_free(_bcm_l2_match_ctrl[unit]);
      _bcm_l2_match_ctrl[unit] = NULL;
    }

    BCM_IF_ERROR_RETURN(_bcm_tr_l2_memacc_deinit(unit));

    return BCM_E_NONE;
}

STATIC int
_bcm_tr_dual_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
                             bcm_l2_addr_t *cf_array, int cf_max,
                             int *cf_count)
{
    l2x_entry_t     match_entry, entry;
    uint32          fval;
    int             bank, bucket, slot, index;
    int             num_banks;
    int             entries_per_bank, entries_per_row, entries_per_bucket;
    int             bank_base, bucket_offset;

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_apache_hash_bank_count_get(unit, L2Xm, &num_banks));
    } else
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_helix5_hash_bank_count_get(unit, L2Xm, &num_banks));
    } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_trident3_hash_bank_count_get(unit, L2Xm, &num_banks));
    } else
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (!SOC_IS_TOMAHAWKX(unit) && SOC_IS_TD2_TT2(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_trident2_hash_bank_count_get(unit, L2Xm, &num_banks));
    } else
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_tomahawk_hash_bank_count_get(unit, L2Xm, &num_banks));
    } else
#endif
    {
        num_banks = 2;
    }

    *cf_count = 0;
    BCM_IF_ERROR_RETURN(_bcm_tr_l2_to_l2x(unit, &match_entry, addr, TRUE));
    for (bank = 0; bank < num_banks; bank++) {
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            int phy_bank;
            BCM_IF_ERROR_RETURN
                (soc_apache_hash_bank_number_get(unit, L2Xm, bank, &phy_bank));

            BCM_IF_ERROR_RETURN
                (soc_apache_hash_bank_info_get(unit, L2Xm, phy_bank,
                                                 &entries_per_bank,
                                                 &entries_per_row,
                                                 &entries_per_bucket,
                                                 &bank_base, &bucket_offset));
            bucket = soc_ap_l2x_bank_entry_hash(unit, phy_bank,
                                                 (uint32 *)&match_entry);
        } else
#endif
#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
            int phy_bank;
            BCM_IF_ERROR_RETURN
                (soc_hx5_hash_bank_number_get(unit, L2Xm, bank, &phy_bank));
            BCM_IF_ERROR_RETURN
                (soc_hx5_hash_bank_info_get(unit, L2Xm, phy_bank,
                                            &entries_per_bank,
                                            &entries_per_row,
                                            &entries_per_bucket,
                                            &bank_base, &bucket_offset));
            bucket = soc_hx5_l2x_bank_entry_hash(unit, phy_bank,
                                                 (uint32 *)&match_entry);
        } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            int phy_bank;
            BCM_IF_ERROR_RETURN
                (soc_td3_hash_bank_number_get(unit, L2Xm, bank, &phy_bank));
            BCM_IF_ERROR_RETURN
                (soc_td3_hash_bank_info_get(unit, L2Xm, phy_bank,
                                            &entries_per_bank,
                                            &entries_per_row,
                                            &entries_per_bucket,
                                            &bank_base, &bucket_offset));
            bucket = soc_td3_l2x_bank_entry_hash(unit, phy_bank,
                                                 (uint32 *)&match_entry);
        } else
#endif
#ifdef BCM_TRIDENT2_SUPPORT
        if (!SOC_IS_TOMAHAWKX(unit) && SOC_IS_TD2_TT2(unit)) {
            int phy_bank;
            BCM_IF_ERROR_RETURN
                (soc_trident2_hash_bank_number_get(unit, L2Xm, bank, &phy_bank));
            BCM_IF_ERROR_RETURN
                (soc_trident2_hash_bank_info_get(unit, L2Xm, phy_bank,
                                                 &entries_per_bank,
                                                 &entries_per_row,
                                                 &entries_per_bucket,
                                                 &bank_base, &bucket_offset));
            bucket = soc_td2_l2x_bank_entry_hash(unit, phy_bank,
                                                 (uint32 *)&match_entry);
        } else
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            int phy_bank;
            BCM_IF_ERROR_RETURN
                (soc_tomahawk_hash_bank_number_get(unit, L2Xm, bank, &phy_bank));
            BCM_IF_ERROR_RETURN
                (soc_tomahawk_hash_bank_info_get(unit, L2Xm, phy_bank,
                                                 &entries_per_bank,
                                                 &entries_per_row,
                                                 &entries_per_bucket,
                                                 &bank_base, &bucket_offset));
            bucket = soc_th_l2x_bank_entry_hash(unit, phy_bank,
                                                 (uint32 *)&match_entry);
        } else
#endif
        {
            entries_per_bank = soc_mem_index_count(unit, L2Xm) / 2;
            entries_per_row = 8;
            entries_per_bucket = entries_per_row / 2;
            bank_base = 0;
            bucket_offset = bank * entries_per_bucket;
            bucket = soc_tr_l2x_bank_entry_hash(unit, bank,
                                                (uint32 *)&match_entry);
        }

        for (slot = 0; slot < entries_per_bucket; slot++) {
            index = bank_base + bucket * entries_per_row + bucket_offset +
                slot;
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY, index, &entry));
            if (soc_feature(unit, soc_feature_base_valid)) {
                if (!soc_L2Xm_field32_get(unit, &entry, BASE_VALIDf)) {
                    continue;
                }
            } else {
            if (!soc_L2Xm_field32_get(unit, &entry, VALIDf)) {
                continue;
            }
            }

            fval = soc_mem_field32_get(unit, L2Xm, &entry, KEY_TYPEf);
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TD2_TT2(unit)) {
                if (fval != TD2_L2_HASH_KEY_TYPE_BRIDGE &&
                    fval != TD2_L2_HASH_KEY_TYPE_VFI) {
                    continue;
                }
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            if (fval != TR_L2_HASH_KEY_TYPE_BRIDGE &&
                fval != TR_L2_HASH_KEY_TYPE_VFI) {
                continue;
            }

            /* Retrieve the hit bit for TD2/TT2 */
#ifdef BCM_TRIDENT2_SUPPORT
            /* Apache is a single pipe device - L2X memory will have hit bit info */
            if (!SOC_IS_TOMAHAWKX(unit) && !SOC_IS_APACHE(unit) &&
                SOC_IS_TD2_TT2(unit) && !SOC_IS_TRIDENT3X(unit)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_l2_hit_retrieve(unit, &entry, index));
            }
#endif /* BCM_TRIDENT2_SUPPORT */   
            BCM_IF_ERROR_RETURN
                (_bcm_tr_l2_from_l2x(unit, &cf_array[*cf_count], &entry));

            *cf_count += 1;
            if (*cf_count >= cf_max) {
                return BCM_E_NONE;
            }
        }
    }

    COMPILER_REFERENCE(entries_per_bank);

    return BCM_E_NONE;
}

int
bcm_tr_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
                          bcm_l2_addr_t *cf_array, int cf_max,
                          int *cf_count)
{
    l2x_entry_t         l2ent;
    uint8               key[XGS_HASH_KEY_SIZE];
    int                 hash_sel, bucket, slot, num_bits;
    uint32              hash_control;

    if (soc_feature(unit, soc_feature_dual_hash)) {
        return _bcm_tr_dual_l2_conflict_get(unit, addr, cf_array,
                                            cf_max, cf_count);
    }

    *cf_count = 0;

    BCM_IF_ERROR_RETURN(_bcm_tr_l2_to_l2x(unit, &l2ent, addr, TRUE));

    /* Get L2 hash select */
    SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));
    hash_sel = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                    L2_AND_VLAN_MAC_HASH_SELECTf);
    num_bits = soc_tr_l2x_base_entry_to_key(unit, &l2ent, key);
    bucket = soc_tr_l2x_hash(unit, hash_sel, num_bits, &l2ent, key);

    for (slot = 0;
         slot < SOC_L2X_BUCKET_SIZE && *cf_count < cf_max;
         slot++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY,
                          bucket * SOC_L2X_BUCKET_SIZE + slot,
                          &l2ent));
        if (soc_feature(unit, soc_feature_base_valid)) {
            if (!soc_L2Xm_field32_get(unit, &l2ent, BASE_VALIDf)) {
                continue;
            }
        } else {
        if (!soc_L2Xm_field32_get(unit, &l2ent, VALIDf)) {
            continue;
        }
        }

        if ((soc_L2Xm_field32_get(unit, &l2ent, KEY_TYPEf) == 
                                  TR_L2_HASH_KEY_TYPE_BRIDGE) ||
            (soc_L2Xm_field32_get(unit, &l2ent, KEY_TYPEf) ==
                                  TR_L2_HASH_KEY_TYPE_VFI)) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr_l2_from_l2x(unit, &cf_array[*cf_count], &l2ent));
            *cf_count += 1;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_add
 * Purpose:
 *      Add a VLAN cross connect entry
 * Parameters:
 *      unit       - Device unit number
 *      outer_vlan - Outer vlan ID
 *      inner_vlan - Inner vlan ID
 *      port_1     - First port in the cross-connect
 *      port_2     - Second port in the cross-connect
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int 
bcm_tr_l2_cross_connect_add(int unit, bcm_vlan_t outer_vlan, 
                            bcm_vlan_t inner_vlan, bcm_gport_t port_1, 
                            bcm_gport_t port_2)
{
    l2x_entry_t  l2x_entry, l2x_lookup;
    int rv, gport_id, l2_index;
    bcm_port_t port_out;
    bcm_module_t mod_out;
    bcm_trunk_t trunk_id;
    soc_field_t vlan_ivid = IVIDf;

    /* Just a safety Check */
    if (!SOC_MEM_IS_VALID(unit,L2Xm)) {
        /* Control should not reach ! */
        return (BCM_E_INTERNAL);
    }
    sal_memset(&l2x_entry, 0, sizeof (l2x_entry));
    if ((outer_vlan < BCM_VLAN_DEFAULT) || (outer_vlan > BCM_VLAN_MAX)) {
        return BCM_E_PARAM;
    } else if (inner_vlan == BCM_VLAN_INVALID) {
        /* Single cross-connect (use only outer_vid) */
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                             TR_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT);
    } else {
        if ((inner_vlan < BCM_VLAN_DEFAULT) || (inner_vlan > BCM_VLAN_MAX)) {
            return BCM_E_PARAM;
        }
        /* Double cross-connect (use both outer_vid and inner_vid) */
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                             TR_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT);
        if (SOC_IS_TRIDENT3X(unit)) {
            vlan_ivid = VLAN__IVIDf;
        }

        soc_L2Xm_field32_set(unit, &l2x_entry, vlan_ivid, inner_vlan);
    }
    soc_L2Xm_field32_set(unit, &l2x_entry, STATIC_BITf, 1);
#ifdef BCM_HURRICANE4_SUPPORT
    if (soc_feature(unit, soc_feature_hr4_l2x_static_bit_war)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, STATIC_DUPLICATEf, 1);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, BASE_VALIDf, 1);
    } else {
    soc_L2Xm_field32_set(unit, &l2x_entry, VALIDf, 1);
    }
    soc_L2Xm_field32_set(unit, &l2x_entry, VLAN_IDf, outer_vlan);

    /* See if the entry already exists */
    rv = soc_mem_generic_lookup(unit, L2Xm, MEM_BLOCK_ANY, 0, &l2x_entry,
                                &l2x_lookup, &l2_index);
                
    if ((rv < 0) && (rv != BCM_E_NOT_FOUND)) {
         return rv;
    } 

    /* Resolve first port */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port_1, &mod_out, &port_out, &trunk_id,
                                &gport_id));
    if (BCM_GPORT_IS_TRUNK(port_1)) {
        if (soc_feature(unit, soc_feature_generic_dest)) {
            soc_mem_field32_dest_set(unit, L2Xm, &l2x_entry,
                DESTINATIONf, SOC_MEM_FIF_DEST_LAG,
                trunk_id);
        } else {
        soc_L2Xm_field32_set(unit, &l2x_entry, Tf, 1);
        soc_L2Xm_field32_set(unit, &l2x_entry, TGIDf, trunk_id);
        }
    } else if (BCM_GPORT_IS_SUBPORT_GROUP(port_1)
               || BCM_GPORT_IS_VLAN_PORT(port_1)) {
        if (soc_feature(unit, soc_feature_generic_dest)) {
            soc_mem_field32_dest_set(unit, L2Xm, &l2x_entry,
                DESTINATIONf, SOC_MEM_FIF_DEST_DVP,
                gport_id);
        } else {
        soc_L2Xm_field32_set(unit, &l2x_entry, VPG_TYPEf, 1);
#if defined(BCM_SCORPION_SUPPORT) && defined(INCLUDE_L3)
        if (SOC_IS_SC_CQ(unit) && BCM_GPORT_IS_SUBPORT_GROUP(port_1)) {
            /* Map the gport_id to index to L3_NEXT_HOP */
            gport_id = (int) _sc_subport_group_index[unit][gport_id/8];
        }
#endif /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */
        soc_L2Xm_field32_set(unit, &l2x_entry, VPGf, gport_id);
        }
    } else {
        if ((mod_out == -1) ||(port_out == -1)) {
            return BCM_E_PORT;
        }
        if (soc_feature(unit, soc_feature_generic_dest)) {
            soc_mem_field32_dest_set(unit, L2Xm, &l2x_entry,
                DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                mod_out << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | port_out);
        } else {
        soc_L2Xm_field32_set(unit, &l2x_entry, MODULE_IDf, mod_out);
        soc_L2Xm_field32_set(unit, &l2x_entry, PORT_NUMf, port_out);
    }
    }

    /* Resolve second port */
    BCM_IF_ERROR_RETURN 
        (_bcm_esw_gport_resolve(unit, port_2, &mod_out, &port_out, &trunk_id,
                                &gport_id));
    if (BCM_GPORT_IS_TRUNK(port_2)) {
        if (soc_feature(unit, soc_feature_generic_dest)) {
            soc_mem_field32_dest_set(unit, L2Xm, &l2x_entry,
                VLAN__DESTINATION_1f, SOC_MEM_FIF_DEST_LAG,
                trunk_id);
        } else {
        soc_L2Xm_field32_set(unit, &l2x_entry, T_1f, 1);
        soc_L2Xm_field32_set(unit, &l2x_entry, TGID_1f, trunk_id);
        }
    } else if (BCM_GPORT_IS_SUBPORT_GROUP(port_2)
               || BCM_GPORT_IS_VLAN_PORT(port_2)) {
        if (soc_feature(unit, soc_feature_generic_dest)) {
            soc_mem_field32_dest_set(unit, L2Xm, &l2x_entry,
                VLAN__DESTINATION_1f, SOC_MEM_FIF_DEST_DVP,
                gport_id);
        } else {
        soc_L2Xm_field32_set(unit, &l2x_entry, VPG_TYPE_1f, 1);
#if defined(BCM_SCORPION_SUPPORT) && defined(INCLUDE_L3)
        if (SOC_IS_SC_CQ(unit) && BCM_GPORT_IS_SUBPORT_GROUP(port_2)) {
            /* Map the gport_id to index to L3_NEXT_HOP */
            gport_id = (int) _sc_subport_group_index[unit][gport_id/8];
        }
#endif /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */
        soc_L2Xm_field32_set(unit, &l2x_entry, VPG_1f, gport_id);
        }
    } else {
        if ((mod_out == -1) ||(port_out == -1)) {
            return BCM_E_PORT;
        }
        if (soc_feature(unit, soc_feature_generic_dest)) {
            soc_mem_field32_dest_set(unit, L2Xm, &l2x_entry,
                VLAN__DESTINATION_1f, SOC_MEM_FIF_DEST_DGPP,
                mod_out << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | port_out);
        } else {
        soc_L2Xm_field32_set(unit, &l2x_entry, MODULE_ID_1f, mod_out);
        soc_L2Xm_field32_set(unit, &l2x_entry, PORT_NUM_1f, port_out);
    }
    }

    rv = soc_mem_insert_return_old(unit, L2Xm, MEM_BLOCK_ANY, 
                                   (void *)&l2x_entry, (void *)&l2x_entry);
    if (rv == BCM_E_FULL) {
        rv = _bcm_l2_hash_dynamic_replace( unit, &l2x_entry);
    } 
    if (BCM_SUCCESS(rv)) {
        if (soc_feature(unit, soc_feature_ppa_bypass)) {
            SOC_CONTROL(unit)->l2x_ppa_bypass = TRUE;
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_delete
 * Purpose:
 *      Delete a VLAN cross connect entry
 * Parameters:
 *      unit       - Device unit number
 *      outer_vlan - Outer vlan ID
 *      inner_vlan - Inner vlan ID
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int 
bcm_tr_l2_cross_connect_delete(int unit, bcm_vlan_t outer_vlan, 
                               bcm_vlan_t inner_vlan)
{
    l2x_entry_t  l2x_entry, l2x_lookup;
    int rv, l2_index;
    soc_field_t vlan_ividf = IVIDf;

    sal_memset(&l2x_entry, 0, sizeof (l2x_entry));
    if ((outer_vlan < BCM_VLAN_DEFAULT) || (outer_vlan > BCM_VLAN_MAX)) {
        return BCM_E_PARAM;
    } else if (inner_vlan == BCM_VLAN_INVALID) {
        /* Single cross-connect (use only outer_vid) */
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                             TR_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT);
    } else {
        if ((inner_vlan < BCM_VLAN_DEFAULT) || (inner_vlan > BCM_VLAN_MAX)) {
            return BCM_E_PARAM;
        }
        /* Double cross-connect (use both outer_vid and inner_vid) */
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                             TR_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT);
        if (SOC_IS_TRIDENT3X(unit)) {
            vlan_ividf = VLAN__IVIDf;
        }
        soc_L2Xm_field32_set(unit, &l2x_entry, vlan_ividf, inner_vlan);
    }
    soc_L2Xm_field32_set(unit, &l2x_entry, STATIC_BITf, 1);
#ifdef BCM_HURRICANE4_SUPPORT
    if (soc_feature(unit, soc_feature_hr4_l2x_static_bit_war)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, STATIC_DUPLICATEf, 1);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, BASE_VALIDf, 1);
    } else {
    soc_L2Xm_field32_set(unit, &l2x_entry, VALIDf, 1);
    }
    soc_L2Xm_field32_set(unit, &l2x_entry, VLAN_IDf, outer_vlan);

    rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, &l2_index, 
                        (void *)&l2x_entry, (void *)&l2x_lookup, 0);
                 
    if ((rv < 0) && (rv != BCM_E_NOT_FOUND)) {
         return rv;
    } 

    rv = soc_mem_delete(unit, L2Xm, MEM_BLOCK_ANY, (void *)&l2x_entry);
    return rv;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_delete_all
 * Purpose:
 *      Delete all VLAN cross connect entries
 * Parameters:
 *      unit       - Device unit number
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_tr_l2_cross_connect_delete_all(int unit)
{
    soc_control_t  *soc = SOC_CONTROL(unit);
    int index_min, index_max, index, mem_max;
    l2x_entry_t *l2x_entry;
    int rv = SOC_E_NONE;
    int *buffer = NULL;
    int mem_size, idx;
    soc_mem_t mem = L2_ENTRY_ONLYm;
    int valid = 0;
    int l2delete_chunks = DEFAULT_L2DELETE_CHUNKS;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        l2delete_chunks = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                           l2delete_chunks);
    }
#endif

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        mem = L2Xm;
    }
#endif

#ifdef BCM_HURRICANE2_SUPPORT
    if (SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit) ||
        SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) {
       mem = L2Xm;
    }
#endif 
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
       mem = L2Xm;
    }

    index_min = soc_mem_index_min(unit, mem);
    mem_max = soc_mem_index_max(unit, mem);
    mem_size =  l2delete_chunks * sizeof(l2x_entry_t);
    
    buffer = soc_cm_salloc(unit, mem_size, "L2X_delete");
    if (NULL == buffer) {
        return SOC_E_MEMORY;
    }

    soc_mem_lock(unit, L2Xm);
    for (idx = index_min; idx < mem_max; idx += l2delete_chunks) {
        index_max = idx + l2delete_chunks - 1;
        if ( index_max > mem_max) {
            index_max = mem_max;
        }
        if ((rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                                     idx, index_max, buffer)) < 0 ) {
            soc_cm_sfree(unit, buffer);
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
        for (index = 0; index < l2delete_chunks; index++) {
            l2x_entry =
                soc_mem_table_idx_to_pointer(unit, mem,
                                             l2x_entry_t *, buffer, index);
            if (soc_feature(unit, soc_feature_base_valid)) {
                valid = soc_L2Xm_field32_get(unit, l2x_entry, BASE_VALIDf);
            } else {
                valid = soc_L2Xm_field32_get(unit, l2x_entry, VALIDf);
            }
            if (valid &&
                ((soc_L2Xm_field32_get(unit, l2x_entry, KEY_TYPEf) ==
                                      TR_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT) ||
                 (soc_L2Xm_field32_get(unit, l2x_entry, KEY_TYPEf) ==
                                      TR_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT))) {
                sal_memcpy(l2x_entry, soc_mem_entry_null(unit, mem),
                           sizeof(l2x_entry_t));
            }
        }
        if ((rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ALL,
                                     idx, index_max, buffer)) < 0) {
            soc_cm_sfree(unit, buffer);
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
    }

    if (soc->arlShadow != NULL) {
        sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
        (void) shr_avl_delete_all(soc->arlShadow);
        sal_mutex_give(soc->arlShadowMutex);
    }
    soc_cm_sfree(unit, buffer);
    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_traverse
 * Purpose:
 *      Walks through the valid cross connect entries and calls
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
bcm_tr_l2_cross_connect_traverse(int unit,
                                 bcm_vlan_cross_connect_traverse_cb cb,
                                 void *user_data)
{
    int index_min, index_max, index, mem_max;
    l2x_entry_t *l2x_entry;
    int rv = SOC_E_NONE;
    int *buffer = NULL;
    int mem_size, idx;
    bcm_gport_t port_1, port_2;
    bcm_vlan_t outer_vlan, inner_vlan;
    bcm_port_t port_in = 0, port_out;
    bcm_module_t mod_in = 0, mod_out;
    soc_mem_t mem = L2Xm;
    soc_field_t vlan_ividf = IVIDf;
    uint32 destv = 0;
    uint32 destt = SOC_MEM_FIF_DEST_INVALID;
    int valid = 0;
    int l2delete_chunks = DEFAULT_L2DELETE_CHUNKS;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        l2delete_chunks = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                           l2delete_chunks);
    }
#endif

#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
        SOC_IS_GREYHOUND2(unit)){
       mem = L2Xm;
    }
#endif
    index_min = soc_mem_index_min(unit, mem);
    mem_max = soc_mem_index_max(unit, mem);
    mem_size =  l2delete_chunks * sizeof(l2x_entry_t);
    
    buffer = soc_cm_salloc(unit, mem_size, "cross connect traverse");
    if (NULL == buffer) {
        return SOC_E_MEMORY;
    }
    
    soc_mem_lock(unit, L2Xm);
    for (idx = index_min; idx < mem_max; idx += l2delete_chunks) {
        index_max = idx + l2delete_chunks - 1;
        if ( index_max > mem_max) {
            index_max = mem_max;
        }
        if ((rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                                     idx, index_max, buffer)) < 0 ) {
            soc_cm_sfree(unit, buffer);
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
        for (index = 0; index < l2delete_chunks; index++) {
            l2x_entry = 
                soc_mem_table_idx_to_pointer(unit, mem,
                                             l2x_entry_t *, buffer, index);
            if (soc_feature(unit, soc_feature_base_valid)) {
                valid = soc_L2Xm_field32_get(unit, l2x_entry, BASE_VALIDf);
            } else {
                valid = soc_L2Xm_field32_get(unit, l2x_entry, VALIDf);
            }
            if (valid) {
                if (soc_L2Xm_field32_get(unit, l2x_entry, KEY_TYPEf) ==
                                     TR_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT) {
                    /* Double cross-connect entry */
                    if (SOC_IS_TRIDENT3X(unit)) {
                        vlan_ividf = VLAN__IVIDf;
                    }
                    inner_vlan = soc_L2Xm_field32_get(unit, l2x_entry, vlan_ividf);
                } else if (soc_L2Xm_field32_get(unit, l2x_entry, KEY_TYPEf) ==
                                     TR_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT) {
                    /* Single cross-connect entry */
                    inner_vlan = BCM_VLAN_INVALID;
                } else {
                    /* Not a cross-connect entry, ignore */
                    continue;
                }
                if (SOC_IS_TRIDENT3X(unit)) {
                    outer_vlan = soc_L2Xm_field32_get(unit, l2x_entry, 
                                                      VLAN__OVIDf);
                } else {
                outer_vlan = soc_L2Xm_field32_get(unit, l2x_entry, OVIDf);
                }
                if (soc_feature(unit, soc_feature_generic_dest)) {
                    destv = soc_mem_field32_dest_get(unit, L2Xm, l2x_entry, 
                                DESTINATIONf, &destt);
                    if (destt == SOC_MEM_FIF_DEST_DGPP) {
                        port_in = destv & SOC_MEM_FIF_DGPP_PORT_MASK;
                        mod_in = (destv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >> 
                                 SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
                    } else {
                        port_in = 0;
                        mod_in = 0;
                        destv = destv & SOC_MEM_FIF_DGPP_TGID_MASK;
                    }
                } else if (soc_L2Xm_field32_get(unit, l2x_entry, Tf)) {
                    destt = SOC_MEM_FIF_DEST_LAG;
                    destv = soc_L2Xm_field32_get(unit, l2x_entry, TGIDf);
                } else {
                    port_in = soc_L2Xm_field32_get(unit, l2x_entry, PORT_NUMf);
                    mod_in = soc_L2Xm_field32_get(unit, l2x_entry, MODULE_IDf);
                }

                /* Get first port params */
                if (SOC_MEM_FIELD_VALID(unit, mem, VPG_TYPEf) && 
                    soc_L2Xm_field32_get(unit, l2x_entry, VPG_TYPEf)) {
                    int vpg;
                    vpg = soc_L2Xm_field32_get(unit, l2x_entry, VPGf);
#if defined(BCM_SCORPION_SUPPORT) && defined(INCLUDE_L3)
                /* Scorpion uses index to L3_NEXT_HOP as VPG */
                    if (SOC_IS_SC_CQ(unit)) {
                        int grp;
                        _SC_SUBPORT_VPG_FIND(unit, vpg, grp);
                        if ((vpg = grp) == -1) {
                            LOG_ERROR(BSL_LS_BCM_L2,
                                      (BSL_META_U(unit,
                                                  "Can not find entry for VPG\n")));
                        }
                    }             
#endif  /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */     
                    BCM_GPORT_SUBPORT_GROUP_SET(port_1, vpg);
                } else if (destt == SOC_MEM_FIF_DEST_DVP) {
                    BCM_GPORT_SUBPORT_GROUP_SET(port_1, destv);
                } else if (destt == SOC_MEM_FIF_DEST_LAG) {
                    BCM_GPORT_TRUNK_SET(port_1, destv);
                } else {
                    rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                                mod_in, port_in, &mod_out, &port_out);
                    if (BCM_FAILURE(rv)) {
                        soc_cm_sfree(unit, buffer);
                        soc_mem_unlock(unit, L2Xm);
                        return (rv);
                    }
                    BCM_GPORT_MODPORT_SET(port_1, mod_out, port_out);
                }

                if (soc_feature(unit, soc_feature_generic_dest)) {
                    destv = soc_mem_field32_dest_get(unit, L2Xm, l2x_entry, 
                                VLAN__DESTINATION_1f, &destt);
                    if (destt == SOC_MEM_FIF_DEST_DGPP) {
                        port_in = destv & SOC_MEM_FIF_DGPP_PORT_MASK;
                        mod_in = (destv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >> 
                                 SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
                    } else {
                        port_in = 0;
                        mod_in = 0;
                        destv = destv & SOC_MEM_FIF_DGPP_TGID_MASK;
                    }
                } else if (soc_L2Xm_field32_get(unit, l2x_entry, T_1f)) {
                    destt = SOC_MEM_FIF_DEST_LAG;
                    destv = soc_L2Xm_field32_get(unit, l2x_entry, TGID_1f);
                } else {
                    port_in = soc_L2Xm_field32_get(unit, l2x_entry, PORT_NUM_1f);
                    mod_in = soc_L2Xm_field32_get(unit, l2x_entry, MODULE_ID_1f);
                }

                /* Get second port params */
                if (SOC_MEM_FIELD_VALID(unit, mem, VPG_TYPE_1f) && 
                    soc_L2Xm_field32_get(unit, l2x_entry, VPG_TYPE_1f)) {
                    int vpg;
                    vpg = soc_L2Xm_field32_get(unit, l2x_entry, VPG_1f);
#if defined(BCM_SCORPION_SUPPORT) && defined(INCLUDE_L3)
                    if (SOC_IS_SC_CQ(unit)) {
                        int grp;
                        _SC_SUBPORT_VPG_FIND(unit, vpg, grp);
                        if ((vpg = grp) == -1) {
                            LOG_ERROR(BSL_LS_BCM_L2,
                                      (BSL_META_U(unit,
                                                  "Can not find entry for VPG\n")));
                        }
                    }             
#endif  /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */     
                    BCM_GPORT_SUBPORT_GROUP_SET(port_2, vpg);
                } else if (destt == SOC_MEM_FIF_DEST_DVP) {
                    BCM_GPORT_SUBPORT_GROUP_SET(port_2, destv);
                } else if (destt == SOC_MEM_FIF_DEST_LAG) {
                    BCM_GPORT_TRUNK_SET(port_2, destv);
                } else {
                    rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                                mod_in, port_in, &mod_out, &port_out);
                    if (BCM_FAILURE(rv)) {
                        soc_cm_sfree(unit, buffer);
                        soc_mem_unlock(unit, L2Xm);
                        return (rv);
                    }
                    BCM_GPORT_MODPORT_SET(port_2, mod_out, port_out);
                }

                /* Call application call-back */
                rv = cb(unit, outer_vlan, inner_vlan, port_1, port_2, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
                if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                    soc_cm_sfree(unit, buffer);
                    soc_mem_unlock(unit, L2Xm);
                    return rv;
                }
#endif
            }
        }
    }
    soc_cm_sfree(unit, buffer);
    soc_mem_unlock(unit, L2Xm);

    return BCM_E_NONE;
}


#if defined(BCM_TRIUMPH_SUPPORT) /* BCM_TRIUMPH_SUPPORT */
STATIC int
_l2_port_mask_gport_resolve(int unit, bcm_gport_t gport,
                            bcm_module_t *modid, bcm_port_t *port,
                            bcm_trunk_t *trunk_id, int *id)
{
    int rv = SOC_E_NONE;

    *modid = -1;
    *port = -1;
    *trunk_id = BCM_TRUNK_INVALID;
    *id = -1;

    if (SOC_GPORT_IS_TRUNK(gport)) {
        *trunk_id = SOC_GPORT_TRUNK_GET(gport);
    } else if (SOC_GPORT_IS_LOCAL(gport)) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_stk_my_modid_get(unit, modid));
        *port = SOC_GPORT_LOCAL_GET(gport);
    } else if (SOC_GPORT_IS_MODPORT(gport)) {
        *modid = SOC_GPORT_MODPORT_MODID_GET(gport);
        *port = SOC_GPORT_MODPORT_PORT_GET(gport);
    } else if (SOC_GPORT_IS_SUBPORT_PORT(gport)) {

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
            *modid = _BCM_SUBPORT_COE_PORT_ID_MOD_GET(gport);
            *port  = _BCM_SUBPORT_COE_PORT_ID_PORT_GET(gport);
        } else
#endif
#if defined(BCM_KATANA2_SUPPORT)
        if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, gport)) {
            BCM_IF_ERROR_RETURN(_bcm_kt2_pp_port_to_modport_get(
                                    unit, gport, modid, port));
        } else
#endif
        {
            rv =SOC_E_PORT;
        }
    }
#if defined(BCM_RIOT_SUPPORT) && defined(INCLUDE_L3)
#ifdef BCM_MPLS_SUPPORT
    else if (SOC_IS_TR_VL(unit) && BCM_GPORT_IS_MPLS_PORT(gport)) {
        *id = BCM_GPORT_MPLS_PORT_ID_GET(gport);
    }
#endif /* BCM_MPLS_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
    else if (BCM_GPORT_IS_MIM_PORT(gport)) {
        *id = BCM_GPORT_MIM_PORT_ID_GET(gport);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    else if (BCM_GPORT_IS_L2GRE_PORT(gport)) {
        *id = BCM_GPORT_L2GRE_PORT_ID_GET(gport);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    else if (BCM_GPORT_IS_VXLAN_PORT(gport)) {
        *id = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    else if (BCM_GPORT_IS_TRILL_PORT(gport)) {
        *id = BCM_GPORT_TRILL_PORT_ID_GET(gport);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#endif /* BCM_RIOT_SUPPORT && INCLUDE_L3 */
    else {
        rv = SOC_E_PORT;
    }
    return rv;
}

/*
 * Function:
 *     _bcm_l2_station_entry_set
 * Purpose:
 *     Set a station entrie's parameters to entry buffer for hardware table
 *     write operation.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     tcam_mem   - (IN) Hardware memory name.
 *     station    - (IN) Station parameters to be set in hardware.
 *     ent_p      - (IN) Software station entry pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_l2_station_entry_set(int unit,
                          soc_mem_t tcam_mem,
                          bcm_l2_station_t *station,
                          _bcm_l2_station_entry_t *ent_p)
{

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    soc_mem_t mysp_mem = MY_STATION_PROFILE_1m;
    int mem_no = 0;

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit) && (station->flags & BCM_L2_STATION_FCOE)) {
        mysp_mem = MY_STATION_PROFILE_2m;
        mem_no = 1;
    }

    if (soc_feature(unit, soc_feature_mysta_profile) && (tcam_mem == MY_STATION_TCAM_2m)) {
        mysp_mem = MY_STATION_PROFILE_2m;
        mem_no = 1;
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#endif

    soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent, VALIDf,
        (1 << soc_mem_field_length(unit, tcam_mem, VALIDf)) - 1);

    soc_mem_mac_addr_set(unit, tcam_mem, ent_p->tcam_ent,
                         MAC_ADDRf, station->dst_mac);

    soc_mem_mac_addr_set(unit, tcam_mem, ent_p->tcam_ent,
                         MAC_ADDR_MASKf, station->dst_mac_mask);

#ifdef BCM_RIOT_SUPPORT
    if (_BCM_L2_STATION_OVERLAY(station) &&
        _BCM_VPN_VFI_IS_SET(station->vlan)) {
        int vfi = 0, vfi_mask = 0;

        if (station->vlan != 0) {
            _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, station->vlan);
        }
        if (station->vlan_mask != 0) {
            _BCM_VPN_GET(vfi_mask, _BCM_VPN_TYPE_VFI, station->vlan_mask);
        }

        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            VFI_IDf, vfi);
        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            VFI_ID_MASKf,vfi_mask);
        if (soc_property_get(unit,
                             spn_MY_STATION_FORWARD_DOMAIN_TYPE_DECOUPLED,
                             0)) {
            /* Forwarding domain type specified by user explicitly. */
            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                FORWARDING_FIELD_TYPEf,
                                station->forward_domain_type);
            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                FORWARDING_FIELD_TYPE_MASKf,
                                (station->forward_domain_type_mask) ? 1 : 0);
        } else {
            /* Forwarding domain type determined by SDK internally. */
            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                FORWARDING_FIELD_TYPEf, 1);
            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                FORWARDING_FIELD_TYPE_MASKf, 1);
        }
    } else
#endif /* BCM_RIOT_SUPPORT */
    {
        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            VLAN_IDf, station->vlan);

        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            VLAN_ID_MASKf, station->vlan_mask);
        if (soc_feature(unit, soc_feature_my_station_2)) {
            if (soc_property_get(unit,
                                 spn_MY_STATION_FORWARD_DOMAIN_TYPE_DECOUPLED,
                                 0)) {
                /* Forwarding domain type specified by user explicitly. */
                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                    FORWARDING_FIELD_TYPEf,
                                    station->forward_domain_type);
                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                    FORWARDING_FIELD_TYPE_MASKf,
                                    (station->forward_domain_type_mask) ? 1 : 0);
            } else {
                /* Forwarding domain type determined by SDK internally. */
                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                    FORWARDING_FIELD_TYPEf, 0);
                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                    FORWARDING_FIELD_TYPE_MASKf, 1);
            }
        }
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (_bcm_l2_p2p_access_vlan_check_enabled(unit)) {
        if (tcam_mem == MY_STATION_TCAM_2m) {
            if (station->forward_domain_type == bcmL2ForwardDomainVlanClassId) {
                ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_TCAM_2;
                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                    FORWARDING_FIELD_TYPEf, 0);
                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                    FORWARDING_FIELD_TYPE_MASKf, 1);
                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                       MY_STATION_FORWARDING_FIELD_OVERLAY_TYPEf, 1);
                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                       MY_STATION_FORWARDING_FIELD_OVERLAY_TYPE_MASKf, 1);
            } else if (BCM_VLAN_VALID(station->vlan)) {
                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                       MY_STATION_FORWARDING_FIELD_OVERLAY_TYPEf, 0);
                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                       MY_STATION_FORWARDING_FIELD_OVERLAY_TYPE_MASKf, 1);
            }
        }
    }
#endif /* BCM_TRIDENT3_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_KATANAX(unit)
        || SOC_IS_TRIDENT(unit)
        || SOC_IS_TD2_TT2(unit)
        || SOC_IS_TRIUMPH3(unit)
        || soc_feature(unit, soc_feature_gh2_my_station)) {
        int gport_id = -1;
        bcm_port_t port_out = 0;
        bcm_module_t mod_out = 0;
        bcm_trunk_t trunk_id = BCM_TRUNK_INVALID;
        uint32 mod_port_data = 0; /* concatenated modid and port */
        bcm_port_t port_out_mask;
        bcm_module_t mod_out_mask;
        bcm_trunk_t trunk_id_mask = -1;  
        int gport_id_mask = -1;
        uint32 mod_port_mask = 0; /* concatenated modid and port */
        int rv=0;

        /* validate both src_port and src_port_mask has the same type */
        if (GPORT_TYPE(station->src_port) != 
                                   GPORT_TYPE(station->src_port_mask)) {
            return BCM_E_PARAM;
        }
 
        if (BCM_GPORT_IS_SET(station->src_port)) {
            rv = _bcm_esw_gport_resolve(unit, station->src_port, 
                          &mod_out, 
                          &port_out, &trunk_id,
                          &gport_id);
            BCM_IF_ERROR_RETURN(rv);

            if (BCM_GPORT_IS_TRUNK(station->src_port)) {
                if (BCM_TRUNK_INVALID == trunk_id) {
                    return BCM_E_PORT;
                }

            } else {
                if ((-1 == mod_out) || (-1 == port_out)) {
                    return BCM_E_PORT;
                }
#ifdef BCM_RIOT_SUPPORT
                if (!_BCM_L2_STATION_OVERLAY(station)) {
                    if (-1 != gport_id) {
                        return BCM_E_PORT;
                    }
                }
#endif /* BCM_RIOT_SUPPORT */
            }

            /* retrieve the port mask */
            rv = _l2_port_mask_gport_resolve(unit,station->src_port_mask,
                                             &mod_out_mask, &port_out_mask,
                                             &trunk_id_mask, &gport_id_mask);
            BCM_IF_ERROR_RETURN(rv);
        } else {
            port_out = station->src_port;
            port_out_mask = station->src_port_mask;
            mod_out = 0;
            mod_out_mask = 0;
        }

        if (trunk_id != BCM_TRUNK_INVALID) {
            /* require hardware with valid SOURCE_FIELDf */
            if (!SOC_MEM_FIELD_VALID(unit, tcam_mem, SOURCE_FIELDf)) {
                return BCM_E_PORT;
            }
            if (SOC_IS_KATANAX(unit)) {
                SOC_IF_ERROR_RETURN(soc_mem_field32_fit(unit, tcam_mem, 
                            SOURCE_FIELD_MASKf, 
                            1 << (SOC_TRUNK_BIT_POS(unit) - 1 )));
                mod_port_data = trunk_id;
                mod_port_mask = trunk_id_mask & 
                    ((1 << SOC_TRUNK_BIT_POS(unit)) - 1);
                if (SOC_MEM_FIELD_VALID(unit, tcam_mem, SOURCE_TYPEf)) {
                    /* If src_port_mask has been reset by caller it means that
                     * the caller does not care about matching against
                     * SOURCE_FIELD at all, in such case it does not really
                     * mater whether the packet is coming from a real port or
                     * a trunk, so it is best to keep the SOURCE_TYPE_MASK
                     * reset also */
                    soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            SOURCE_TYPEf, 1);
                    if (BCM_GPORT_IS_SET(station->src_port)) {
                        if (port_out_mask) {
                            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                    SOURCE_TYPE_MASKf, 1);
                        } else {
                            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                    SOURCE_TYPE_MASKf, 0);
                        }
                    } else {
                        if (station->src_port_mask) {
                            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                    SOURCE_TYPE_MASKf, 1);
                        } else {
                            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                    SOURCE_TYPE_MASKf, 0);
                        }
                    }
                }
            } else if (soc_feature(unit, soc_feature_gh2_my_station)) {
                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            SOURCE_TYPEf, 1);
                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            SOURCE_TYPE_MASKf, 1);
                mod_port_data = trunk_id;
                mod_port_mask = trunk_id_mask; 
            } else {
                /* and the SOURCE_FIELDf supports the trunk operation */
                SOC_IF_ERROR_RETURN(soc_mem_field32_fit(unit, tcam_mem, 
                            SOURCE_FIELD_MASKf, 
                            1 << SOC_TRUNK_BIT_POS(unit)));

                mod_port_data = ((1 << SOC_TRUNK_BIT_POS(unit)) | trunk_id);
                mod_port_mask = trunk_id_mask & 
                    ((1 << SOC_TRUNK_BIT_POS(unit)) - 1);
                mod_port_mask |= (1 << SOC_TRUNK_BIT_POS(unit));
            }
            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                       SOURCE_FIELDf, mod_port_data);
            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                       SOURCE_FIELD_MASKf, mod_port_mask);
#ifdef BCM_RIOT_SUPPORT
        } else if((gport_id != BCM_GPORT_INVALID) &&
                  (_BCM_L2_STATION_OVERLAY(station))) {

            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                SOURCE_TYPEf, 1);
            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                SOURCE_TYPE_MASKf, 1);
            mod_port_data = gport_id;
            mod_port_mask = gport_id_mask;
            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                SOURCE_FIELDf, mod_port_data);
            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                SOURCE_FIELD_MASKf, mod_port_mask);
#endif /* BCM_RIOT_SUPPORT */
        } else {
            if (SOC_MEM_FIELD_VALID(unit, tcam_mem, SOURCE_FIELDf)) {
                int num_bits_for_port;
                num_bits_for_port = _shr_popcount(
                                 (unsigned int)SOC_PORT_ADDR_MAX(unit));

                mod_port_data = (mod_out << num_bits_for_port) | port_out;
                mod_port_mask = mod_out_mask & SOC_MODID_MAX(unit);
                mod_port_mask <<= num_bits_for_port;
                mod_port_mask |= (port_out_mask & SOC_PORT_ADDR_MAX(unit));

                if (soc_feature(unit, soc_feature_gh2_my_station)) {
                    soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            SOURCE_TYPEf, 0);
                    soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            SOURCE_TYPE_MASKf, 1);
                } else {
                if (!SOC_IS_KATANAX(unit)) {
                    /* Clear the trunk ID bit. */
                    mod_port_data &= ~(1 << SOC_TRUNK_BIT_POS(unit));

                    /* Must match on the T bit (which should be 0) for the devices
                     * supporting the trunk operation with this field
                     */
                    if (BCM_GPORT_IS_SET(station->src_port)) {
                        if (soc_mem_field32_fit(unit, tcam_mem, 
                                    SOURCE_FIELD_MASKf, 
                                    1 << SOC_TRUNK_BIT_POS(unit)) == SOC_E_NONE) {
                            mod_port_mask |= (1 << SOC_TRUNK_BIT_POS(unit));
                        }
                    }
                }
                if (SOC_IS_KATANAX(unit)) {
                    if (SOC_MEM_FIELD_VALID(unit, tcam_mem, SOURCE_TYPEf)) {
                        /* If src_port_mask has been reset by caller it means
                         * that the caller does not care about matching against
                         * SOURCE_FIELD at all, in such case it does not really
                         * mater whether the packet is coming from a real port
                         * or a trunk, so it is best to keep the
                         * SOURCE_TYPE_MASK * reset also */
                        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                SOURCE_TYPEf, 0);
                        if (BCM_GPORT_IS_SET(station->src_port)) {
                            if (port_out_mask) {
                                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                        SOURCE_TYPE_MASKf, 1);
                            } else {
                                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                        SOURCE_TYPE_MASKf, 0);
                            }

                        } else {
                            if (station->src_port_mask) {
                                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                        SOURCE_TYPE_MASKf, 1);
                            } else {
                                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                        SOURCE_TYPE_MASKf, 0);
                            }
                        }
                    }
                }
                }
                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                    SOURCE_FIELDf, mod_port_data);

                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                    SOURCE_FIELD_MASKf, mod_port_mask);
            } else {
                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                    ING_PORT_NUMf, port_out);

                soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            ING_PORT_NUM_MASKf, 
                            port_out_mask & SOC_PORT_ADDR_MAX(unit));
            }
        } 

        if (soc_feature(unit, soc_feature_mysta_profile)) {
            uint32 mysta_profile_id;
            my_station_profile_1_entry_t entry;
            sal_memset(&entry, 0, sizeof(entry));
            _bcm_l2_mysta_station_to_entry(unit, station, mysp_mem, &entry);
            rv = _bcm_l2_mysta_profile_entry_add(unit, &entry, mem_no,
                                                 &mysta_profile_id);
            BCM_IF_ERROR_RETURN(rv);

            soc_mem_field32_dest_set(unit, tcam_mem, ent_p->tcam_ent,
                DESTINATIONf, SOC_MEM_FIF_DEST_MYSTA, mysta_profile_id);
        } else {

        if (!soc_feature(unit, soc_feature_gh2_my_station)) {
        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            MIM_TERMINATION_ALLOWEDf,
                            ((station->flags & BCM_L2_STATION_MIM) ? 1 : 0));

        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            MPLS_TERMINATION_ALLOWEDf,
                            ((station->flags & BCM_L2_STATION_MPLS) ? 1 : 0));
        }
        
        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            IPV4_TERMINATION_ALLOWEDf,
                            ((station->flags & BCM_L2_STATION_IPV4) ? 1 : 0));

        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            IPV6_TERMINATION_ALLOWEDf,
                            ((station->flags & BCM_L2_STATION_IPV6) ? 1 : 0));
        
        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            ARP_RARP_TERMINATION_ALLOWEDf,
                            ((station->flags & BCM_L2_STATION_ARP_RARP) ? 1 : 0));
        }
        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            COPY_TO_CPUf,
                            ((station->flags & BCM_L2_STATION_COPY_TO_CPU)
                            ? 1 : 0));
    }
#endif

#if defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TD2_TT2(unit)) {
        if (!soc_feature(unit, soc_feature_mysta_profile)) {
        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            TRILL_TERMINATION_ALLOWEDf,
                            ((station->flags & BCM_L2_STATION_TRILL) ? 1 : 0));

        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            FCOE_TERMINATION_ALLOWEDf,
                            ((station->flags & BCM_L2_STATION_FCOE) ? 1 : 0));
    }
    }
#endif

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_HELIX5_SUPPORT)
    if ((SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
        soc_feature(unit, soc_feature_gh2_my_station)) ||
        soc_feature(unit, soc_feature_hx5_oam_support)) {

        if (soc_mem_field_valid(unit, tcam_mem, OAM_TERMINATION_ALLOWEDf)) { 
            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                OAM_TERMINATION_ALLOWEDf,
                                ((station->flags & BCM_L2_STATION_OAM) ? 1 :0));
        }
    }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)

    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIDENT(unit) ||
        SOC_IS_TD2_TT2(unit) || SOC_IS_TRIUMPH3(unit) ||
        soc_feature(unit, soc_feature_gh2_my_station)) {
        
        if (soc_mem_field_valid(unit, tcam_mem, DISCARDf)) { 
            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                DISCARDf,
                                ((station->flags & BCM_L2_STATION_DISCARD) ? 1 : 0));
        }
    }
#endif
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_taf)) {
        if (soc_mem_field_valid(unit, tcam_mem, TAF_GATE_ID_PROFILEf)) {
            if (BCM_TSN_PRI_MAP_INVALID != station->taf_gate_primap) {
                uint32 hw_index;
                bcm_tsn_pri_map_type_t map;

                BCM_IF_ERROR_RETURN(
                    bcmi_esw_tsn_pri_map_hw_index_get(
                        unit, station->taf_gate_primap, &map, &hw_index));

                if (map != bcmTsnPriMapTypeTafGate) {
                    return BCM_E_PARAM;
                }
                soc_mem_field32_set(
                    unit, tcam_mem, ent_p->tcam_ent,
                    TAF_GATE_ID_PROFILEf, hw_index);
            } else {
                soc_mem_field32_set(
                    unit, tcam_mem, ent_p->tcam_ent,
                    TAF_GATE_ID_PROFILEf, 0);
            }
        }
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_NONE;
}

#if defined(BCM_KATANA2_SUPPORT)

/* Obsolete now, check _bcm_kt2_port_mac_set() */
STATIC int
_bcm_kt2_l2_port_mac_set(int unit, int index, bcm_mac_t port_mac) 
{
    port_tab_entry_t          ptab;        
    egr_port_entry_t          egr_port_entry;
    /* Set MAC address in Port table */
    SOC_IF_ERROR_RETURN(
                READ_PORT_TABm(unit, MEM_BLOCK_ANY, index, &ptab));

    soc_mem_mac_addr_set(unit, PORT_TABm, &ptab, MAC_ADDRESSf, port_mac);

    SOC_IF_ERROR_RETURN( WRITE_PORT_TABm(unit,MEM_BLOCK_ALL, index, &ptab));

    SOC_IF_ERROR_RETURN(READ_EGR_PORTm(unit, MEM_BLOCK_ANY, index,
                                           &egr_port_entry));

    soc_mem_mac_addr_set(unit, EGR_PORTm, &egr_port_entry, 
                                   MAC_ADDRESSf, port_mac);
    SOC_IF_ERROR_RETURN(
            WRITE_EGR_PORTm(unit, MEM_BLOCK_ALL, index, &egr_port_entry));

    return BCM_E_NONE; 
}
#endif

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
/* Obsolete */
STATIC int
_bcm_check_olp_dglp(int unit, int dglp, bcm_mac_t *mac)
{
    _bcm_l2_station_control_t *sc;           /* Station control structure. */
    egr_olp_dgpp_config_entry_t eolp_dgpp_cfg;
    int        index = 0;
    int        max_entries = 0;
    soc_field_t field;

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

    max_entries = soc_mem_index_count(unit, EGR_OLP_DGPP_CONFIGm);

    for (; index < max_entries; index++) {
        SOC_IF_ERROR_RETURN(
              READ_EGR_OLP_DGPP_CONFIGm(unit, MEM_BLOCK_ANY, 
                                           index, &eolp_dgpp_cfg));
        if (SOC_MEM_FIELD_VALID(unit, EGR_OLP_DGPP_CONFIGm, VALIDf)) {
            field = soc_mem_field32_get(unit, EGR_OLP_DGPP_CONFIGm, 
                    &eolp_dgpp_cfg, VALIDf);
            if(!field) {
                continue;
            }
        }
        field = soc_mem_field32_get(unit, EGR_OLP_DGPP_CONFIGm, 
                                  &eolp_dgpp_cfg, DGLPf);
        if (field == dglp) {
            soc_mem_mac_addr_get(unit, EGR_OLP_DGPP_CONFIGm, &eolp_dgpp_cfg, 
                                                    MACDAf, *mac);
            if (dglp == 0) {
                /* CPU port
                   Check if its a zero mac. If yes, dont return BCM_E_EXISTS*/
                    if (!BCM_MAC_IS_ZERO(*mac)) {
                        return BCM_E_EXISTS;
                    }
            } else {
                return BCM_E_EXISTS;
            }
        }
    } /* end of for */
    return BCM_E_NOT_FOUND;
}

/* Soon to become Obsolete, check bcm_switch_olp_l2_addr_xxx */
int
_bcm_l2_olp_mac_set(int unit, int index, bcm_port_t port, bcm_mac_t olp_mac, int action)
{
    egr_olp_dgpp_config_entry_t eolp_dgpp_cfg;
    ing_en_efilter_bitmap_entry_t efilter_entry;
    bcm_module_t module_id = 0;
    bcm_module_t my_modid;
    bcm_port_t port_id;
    bcm_trunk_t trunk_id = BCM_TRUNK_INVALID;
    int         local_id;
    int         dglp = 0;
    uint32      olp_enable = 1;
    pbmp_t      pbmp;
    bcm_l2_addr_t l2addr;
    bcm_vlan_t  vlan;
    uint32      rval;
    bcm_mac_t   zero_mac = { 0, 0, 0, 0, 0, 0};
    bcm_mac_t   old_mac = { 0, 0, 0, 0, 0, 0};
    _bcm_l2_station_control_t *sc;           /* Station control structure. */

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    /* Add/delete static MAC entry with OLP MAC and EGR_OLP_VLAN */
    SOC_IF_ERROR_RETURN(READ_EGR_OLP_VLANr(unit, &rval));
    vlan = soc_reg_field_get(unit, EGR_OLP_VLANr, rval, VLAN_TAGf);

    if ((vlan == 0) && (action != OLP_ACTION_DELETE)) {
        LOG_ERROR(BSL_LS_BCM_L2,
                  (BSL_META_U(unit,
                              "_bcm_l2_olp_mac_set: OLP vlan tag is not configured. \
                              So can't add static MAC entry for OLP mac\n")));
        return BCM_E_PARAM;
    }

    /* Set MAC address and DGLP in EGR_OLP_DGLP_CONFIG */
    SOC_IF_ERROR_RETURN(
              READ_EGR_OLP_DGPP_CONFIGm(unit, MEM_BLOCK_ANY, 
                                           index, &eolp_dgpp_cfg));
    if (port == 0) {
        port = soc_mem_field32_get(unit, EGR_OLP_DGPP_CONFIGm, 
                                  &eolp_dgpp_cfg, DGLPf);
        module_id = (port & DGLP_MODULE_ID_MASK) >> DGLP_MODULE_ID_SHIFT_BITS;
        port_id = port & DGLP_PORT_NO_MASK;
        dglp = 0;
        olp_enable = 0;
        soc_mem_mac_addr_get(unit, EGR_OLP_DGPP_CONFIGm, &eolp_dgpp_cfg, 
                                                    MACDAf, olp_mac);
    } else {
        if (BCM_GPORT_IS_SET(port)) {
            SOC_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &module_id, 
                                               &port_id, &trunk_id, &local_id));
            if (BCM_GPORT_IS_TRUNK(port) && (trunk_id != BCM_TRUNK_INVALID)) {
                /* Set LAG ID indicator bit */
                dglp |= (1 << DGLP_LAG_ID_INDICATOR_SHIFT_BITS);  
                dglp |= trunk_id;
#if defined(BCM_KATANA2_SUPPORT) 
            } else if (SOC_IS_KATANA2(unit) && 
                      (BCM_GPORT_IS_SUBPORT_PORT(port)) &&
                  ((_BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) ||
                  (_BCM_KT2_GPORT_IS_LINKPHY_SUBPORT_PORT(unit, port)))) {
                LOG_ERROR(BSL_LS_BCM_L2,
                          (BSL_META_U(unit,
                                      "_bcm_l2_olp_mac_set: OLP is not allowed on SUBPORT. \
                                      \n")));
                return BCM_E_PARAM;
#endif                
            } else {
                dglp |= ((module_id << DGLP_MODULE_ID_SHIFT_BITS) + port_id); 
            }
        } else {
            port_id = port;
            dglp |= ((my_modid << DGLP_MODULE_ID_SHIFT_BITS) + port); 
        }
        if (!SOC_IS_SABER2(unit) && 
                !soc_feature(unit, soc_feature_fp_based_oam)) {
            if (IS_CPU_PORT(unit, port_id)) {
                LOG_ERROR(BSL_LS_BCM_L2,
                        (BSL_META_U(unit,
                                    "_bcm_l2_olp_mac_set: OLP is not allowed on CPU port. \n")));
                return BCM_E_PARAM;
            }
        }
    }
    /* check if an entry already exists for this dglp */
    if (BCM_E_EXISTS == _bcm_check_olp_dglp(unit, dglp, &old_mac)) {
        if (action == OLP_ACTION_UPDATE) {
            /* delete the static l2 entry added */
            BCM_IF_ERROR_RETURN(bcm_esw_l2_addr_delete(unit, old_mac, vlan)); 
        } else if (action == OLP_ACTION_ADD) {
            LOG_ERROR(BSL_LS_BCM_L2,
                      (BSL_META_U(unit,
                                  "_bcm_l2_olp_mac_set: OLP MAC is already configured. \
                                  for this DGLP\n")));
            return BCM_E_PARAM;
        } 
    }

    soc_mem_mac_addr_set(unit, EGR_OLP_DGPP_CONFIGm, &eolp_dgpp_cfg, 
                         MACDAf, (olp_enable ? olp_mac : zero_mac));

    soc_mem_field32_set(unit, EGR_OLP_DGPP_CONFIGm, &eolp_dgpp_cfg, DGLPf, dglp);
    if (SOC_MEM_FIELD_VALID(unit, EGR_OLP_DGPP_CONFIGm, VALIDf)) {
        soc_mem_field32_set(unit, EGR_OLP_DGPP_CONFIGm, &eolp_dgpp_cfg, 
                            VALIDf, olp_enable);
    }
    SOC_IF_ERROR_RETURN(WRITE_EGR_OLP_DGPP_CONFIGm(unit, MEM_BLOCK_ALL, 
                                                   index, &eolp_dgpp_cfg));
    if ((module_id == 0) || (module_id == my_modid)) {
        /* Local port - Enable OLP on this port */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if(SOC_IS_TRIDENT2PLUS(unit)) {    
            iarb_ing_port_table_entry_t entry;
            port_tab_entry_t            port_entry;
            uint64                      mac;
            int                         prefix_check = 0;

            SOC_IF_ERROR_RETURN(soc_mem_read(unit, IARB_ING_PORT_TABLEm, 
                        MEM_BLOCK_ANY, port_id, &entry));
            soc_IARB_ING_PORT_TABLEm_field32_set(unit, &entry, 
                    OLP_ENABLEf, olp_enable);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, IARB_ING_PORT_TABLEm, 
                        MEM_BLOCK_ALL, port_id, &entry));

            /* Enable/Disable the prefix checking on the OLP port
             * If OLP is being enabled  - Disable prefix checking
             * If OLP is being disabled - Enable prefix checking if it
             *                            is configured.
             */
            prefix_check = 0;
            if (0 == olp_enable) {
                SOC_IF_ERROR_RETURN(READ_ING_OLP_CONFIG_0_64r(unit, &mac));
                if (!COMPILER_64_IS_ZERO(mac)) {
                    prefix_check = 1;
                }
            }
            SOC_IF_ERROR_RETURN(READ_PORT_TABm(unit,
                                               MEM_BLOCK_ANY,
                                               port_id,
                                               &port_entry));
            soc_PORT_TABm_field32_set(unit,
                                      &port_entry,
                                      OLP_MAC_DA_PREFIX_CHECK_ENABLEf,
                                      prefix_check);

            SOC_IF_ERROR_RETURN(WRITE_PORT_TABm(unit,
                                                MEM_BLOCK_ALL,
                                                port_id,
                                                &port_entry));
        } else 
#endif
        {
            iarb_ing_physical_port_entry_t  entry;
            ing_physical_port_table_entry_t port_entry;
            uint64                          mac;
            int                             prefix_check = 0;

            SOC_IF_ERROR_RETURN(soc_mem_read(unit, IARB_ING_PHYSICAL_PORTm, 
                        MEM_BLOCK_ANY, port_id, &entry));
            soc_IARB_ING_PHYSICAL_PORTm_field32_set(unit, &entry, 
                    OLP_ENABLEf, olp_enable);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, IARB_ING_PHYSICAL_PORTm, 
                        MEM_BLOCK_ALL, port_id, &entry));

            /* Enable/Disable the prefix checking on the OLP port
             * If OLP is being enabled  - Disable prefix checking
             * If OLP is being disabled - Enable prefix checking if it
             *                            is configured.
             */
            prefix_check = 0;
            if (0 == olp_enable) {
                SOC_IF_ERROR_RETURN(READ_ING_OLP_CONFIG_0_64r(unit, &mac));
                if (!COMPILER_64_IS_ZERO(mac)) {
                    prefix_check = 1;
                }
            }
            SOC_IF_ERROR_RETURN(
                                READ_ING_PHYSICAL_PORT_TABLEm(unit,
                                                              MEM_BLOCK_ANY,
                                                              port_id,
                                                              &port_entry));

            soc_ING_PHYSICAL_PORT_TABLEm_field32_set(unit,
                                                     &port_entry,
                                                     OLP_MAC_DA_PREFIX_CHECK_ENABLEf,
                                                     prefix_check);
            SOC_IF_ERROR_RETURN(
                                WRITE_ING_PHYSICAL_PORT_TABLEm(unit,
                                                               MEM_BLOCK_ALL,
                                                               port_id,
                                                               &port_entry));

        }
        /* Disable EFILTER */
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, ING_EN_EFILTER_BITMAPm, 
                    MEM_BLOCK_ANY, 0, &efilter_entry));
        soc_mem_pbmp_field_get(unit, ING_EN_EFILTER_BITMAPm,
                &efilter_entry, BITMAPf, &pbmp);
        if (olp_enable) {
            SOC_PBMP_PORT_REMOVE(pbmp, port_id);
        } else {
            SOC_PBMP_PORT_ADD(pbmp, port_id);
        }
        soc_mem_pbmp_field_set(unit, ING_EN_EFILTER_BITMAPm,
                                          &efilter_entry, BITMAPf, &pbmp);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_EN_EFILTER_BITMAPm, 
                                              MEM_BLOCK_ALL, 0, &efilter_entry));
    }

    if(BCM_VLAN_VALID(vlan) && !BCM_MAC_IS_ZERO(olp_mac)) {
        /* Set l2 address info. */
        bcm_l2_addr_t_init(&l2addr, olp_mac, vlan);

        if (olp_enable) {
            /* Set l2 entry flags. */
            l2addr.flags = BCM_L2_STATIC;
            /* Set l2 entry port dglp*/
            l2addr.port = port;
            bcm_esw_l2_addr_add(unit, &l2addr); 
        } else {
            BCM_IF_ERROR_RETURN(bcm_esw_l2_addr_delete(unit, olp_mac, vlan)); 
        }
    }
    return BCM_E_NONE; 
}

/* Obsolete, check bcmSwitchOamOlpStationMacOui */
STATIC int
_bcm_l2_xgs_mac_set(int unit, int index, bcm_mac_t xgs_mac)
{
    egr_olp_config_entry_t    eolp_cfg;
    uint64                    iarb_olp_cfg;
    uint64                    mac_field;
    uint64                    mask_field;
    int                       set;
    bcm_port_t                port;
    bcm_pbmp_t                ports;
    int                       olp_port;

    /* Set MAC address in IARB_OLP_CONFIG */
    SAL_MAC_ADDR_TO_UINT64(xgs_mac, mac_field);

    COMPILER_64_ZERO(iarb_olp_cfg); 

    SOC_IF_ERROR_RETURN(READ_IARB_OLP_CONFIGr(unit, &iarb_olp_cfg));

    soc_reg64_field_set(unit, IARB_OLP_CONFIGr, &iarb_olp_cfg, MACDAf, mac_field);

    SOC_IF_ERROR_RETURN(WRITE_IARB_OLP_CONFIGr(unit, iarb_olp_cfg));
  
    /* Set MAC address in EGR_OLP_CONFIG */
    SOC_IF_ERROR_RETURN(
              READ_EGR_OLP_CONFIGm(unit, MEM_BLOCK_ANY, index, &eolp_cfg));

    soc_mem_mac_addr_set(unit, EGR_OLP_CONFIGm, &eolp_cfg, 
                                   MACSAf, xgs_mac);
    SOC_IF_ERROR_RETURN(
                WRITE_EGR_OLP_CONFIGm(unit, MEM_BLOCK_ALL, 
                                           index, &eolp_cfg));

    /* Set mask and data values in ING_OLP_CONFIG_ register */
    if (COMPILER_64_IS_ZERO(mac_field)) {
        set = FALSE;
        COMPILER_64_SET(mask_field, 0x0, 0x0);
    } else {
        set = TRUE;
        COMPILER_64_SET(mask_field, 0XFFFF, 0xFFFFFFFF);
    }

    SOC_IF_ERROR_RETURN(WRITE_ING_OLP_CONFIG_0_64r(unit, mac_field));

    SOC_IF_ERROR_RETURN(WRITE_ING_OLP_CONFIG_1_64r(unit, mask_field));

    /* Enable/Disable the OLP MACDA prefix checking for front panel ports */
    BCM_PBMP_ASSIGN(ports, PBMP_PORT_ALL(unit));
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if(SOC_IS_TRIDENT2PLUS(unit)) {
        port_tab_entry_t            port_entry;
        iarb_ing_port_table_entry_t iarb_ing_port_entry;

        PBMP_ITER(ports, port) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm,
                                             MEM_BLOCK_ANY, port, &port_entry));

            SOC_IF_ERROR_RETURN(soc_mem_read(unit, IARB_ING_PORT_TABLEm, 
                                    MEM_BLOCK_ANY, port, &iarb_ing_port_entry));
            if (soc_IARB_ING_PORT_TABLEm_field32_get(unit, &iarb_ing_port_entry,
                                                     OLP_ENABLEf)) {
                olp_port = TRUE;
            } else {
                olp_port = FALSE;
            }

            if (!IS_HG_PORT(unit, port) && (TRUE == set) && (FALSE == olp_port)) {
                soc_PORT_TABm_field32_set(unit, &port_entry,
                                          OLP_MAC_DA_PREFIX_CHECK_ENABLEf, 1);
            } else {
                soc_PORT_TABm_field32_set(unit, &port_entry,
                                          OLP_MAC_DA_PREFIX_CHECK_ENABLEf, 0);
            }
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, PORT_TABm,
                                             MEM_BLOCK_ALL, port, &port_entry));
        }

    } else
#endif
    {
        ing_physical_port_table_entry_t port_entry;
        iarb_ing_physical_port_entry_t iarb_entry;

        PBMP_ITER(ports, port) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, ING_PHYSICAL_PORT_TABLEm,
                                             MEM_BLOCK_ANY, port, &port_entry));

            SOC_IF_ERROR_RETURN(soc_mem_read(unit, IARB_ING_PHYSICAL_PORTm, 
                                   MEM_BLOCK_ANY, port, &iarb_entry));

            if (soc_IARB_ING_PHYSICAL_PORTm_field32_get(unit, &iarb_entry,
                                                        OLP_ENABLEf)) {
                olp_port = TRUE;
            } else {
                olp_port = FALSE;
            }
            if (!IS_HG_PORT(unit, port) && (TRUE == set) && (FALSE == olp_port)) {
                soc_ING_PHYSICAL_PORT_TABLEm_field32_set(unit, &port_entry,
                                            OLP_MAC_DA_PREFIX_CHECK_ENABLEf, 1);
            } else {
                soc_ING_PHYSICAL_PORT_TABLEm_field32_set(unit, &port_entry,
                                            OLP_MAC_DA_PREFIX_CHECK_ENABLEf, 0);
            }
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_PHYSICAL_PORT_TABLEm,
                                             MEM_BLOCK_ALL, port, &port_entry));
        }
    }
    return BCM_E_NONE; 
}
/* Soon to become Obsolete, check bcm_switch_olp_l2_addr_xxx */
STATIC int
_bcm_l2_station_non_tcam_entry_delete(int unit, 
                                 _bcm_l2_station_control_t *sc, 
                                 _bcm_l2_station_entry_t *s_ent,
                                 int station_id) 
{
    int                         rv = BCM_E_NONE;  /* Operation return status. */
    int                         port_entries_total=0;
    bcm_mac_t                   mac_addr = {0,0,0,0,0,0};

#if defined(BCM_KATANA2_SUPPORT) 
    if (SOC_IS_KATANA2(unit)) {
        port_entries_total = sc->port_entries_total;
    }
#endif
#if defined(BCM_KATANA2_SUPPORT)
    if (s_ent->flags & _BCM_L2_STATION_ENTRY_TYPE_OAM_MAC) {
        rv = _bcm_kt2_l2_port_mac_set(unit, s_ent->hw_index, mac_addr);
        sc->port_entries_free++;
        /* Remove the entry pointer from the list. */
        sc->entry_arr[s_ent->hw_index + sc->entries_total] = NULL;
    } else 
#endif    
    if (s_ent->flags & _BCM_L2_STATION_ENTRY_TYPE_OLP_MAC) {
        rv = _bcm_l2_olp_mac_set(unit, s_ent->hw_index, 0, 
                                     mac_addr, OLP_ACTION_DELETE);
        sc->entry_arr[s_ent->hw_index + 
                      sc->entries_total + port_entries_total] = NULL;
        sc->olp_entries_free++;
    } else if (s_ent->flags & _BCM_L2_STATION_ENTRY_TYPE_XGS_MAC) {
        rv = _bcm_l2_xgs_mac_set(unit, 0, mac_addr);
        sc->entry_arr[s_ent->hw_index + sc->entries_total + 
                      port_entries_total + sc->olp_entries_total] = NULL;
    }
    LOG_DEBUG(BSL_LS_BCM_L2,
            (BSL_META_U(unit,
                        "rv %d, flag 0x%x, station %d\n"), 
             rv, s_ent->flags, station_id));
    return (rv);
}
/* Soon to become Obsolete, check bcm_switch_olp_l2_addr_xxx */
STATIC int
_bcm_l2_station_non_tcam_entry_get(int unit, _bcm_l2_station_control_t *sc, 
                              _bcm_l2_station_entry_t *s_ent,
                              int station_id, bcm_l2_station_t *station) 
{
    int         rv = BCM_E_NONE;  /* Operation return status. */
    egr_olp_config_entry_t    eolp_cfg;
    egr_olp_dgpp_config_entry_t eolp_dgpp_cfg;

#if defined(BCM_KATANA2_SUPPORT)
    if (s_ent->flags & _BCM_L2_STATION_ENTRY_TYPE_OAM_MAC) {
        port_tab_entry_t          ptab;        
        SOC_IF_ERROR_RETURN(
                READ_PORT_TABm(unit, MEM_BLOCK_ANY, s_ent->hw_index, &ptab));
        station->src_port = s_ent->hw_index;
        soc_mem_mac_addr_get(unit, PORT_TABm, &ptab, 
                             MAC_ADDRESSf, station->dst_mac);
        station->flags = BCM_L2_STATION_OAM; 
    } else 
#endif    
    if (s_ent->flags & _BCM_L2_STATION_ENTRY_TYPE_OLP_MAC) {
        SOC_IF_ERROR_RETURN(
              READ_EGR_OLP_DGPP_CONFIGm(unit, MEM_BLOCK_ANY, 
                                         s_ent->hw_index, &eolp_dgpp_cfg));
        station->src_port = soc_mem_field32_get(unit, EGR_OLP_DGPP_CONFIGm, 
                                  &eolp_dgpp_cfg, DGLPf);
        soc_mem_mac_addr_get(unit, EGR_OLP_DGPP_CONFIGm, &eolp_dgpp_cfg, 
                             MACDAf, station->dst_mac);
        station->flags = BCM_L2_STATION_OLP; 
    } else if (s_ent->flags & _BCM_L2_STATION_ENTRY_TYPE_XGS_MAC) {
        SOC_IF_ERROR_RETURN(
              READ_EGR_OLP_CONFIGm(unit, MEM_BLOCK_ANY, 0, &eolp_cfg));
         soc_mem_mac_addr_get(unit, EGR_OLP_CONFIGm, &eolp_cfg, 
                                            MACSAf, station->dst_mac);
        station->flags = BCM_L2_STATION_XGS_MAC; 
    }
    return rv;
}

#endif /* BCM_KATANA2_SUPPORT OR TD2+*/

#if defined(BCM_KATANA2_SUPPORT)

/*
 * Function:
 *     _bcm_kt2_l2_station_entry_create
 * Purpose:
 *     Allocate an entry buffer and setup entry parameters for hardware write
 *     operation.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     sid        - (IN) Station ID.
 *     station    - (IN) Station parameters to be set in hardware.
 *     ent_p      - (OUT) Pointer to Station entry type data.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_l2_station_entry_create(int unit,
                             int sid,
                             bcm_l2_station_t *station,
                             _bcm_l2_station_entry_t **ent_pp)
{
    _bcm_l2_station_control_t *sc;           /* Station control structure. */
    int                       index;         /* Entry index.               */
    _bcm_l2_station_entry_t   *ent_p = NULL; /* L2 station entry pointer.  */
    soc_mem_t                 tcam_mem;      /* TCAM memory name.          */ 
    int                       mem_size = 0;  /* Entry buffer size.         */
    int                       rv;            /* Operation return status.   */
    int                       index_max;     /* Max Entry index.           */
    bcm_module_t       module_id;            /* Module ID           */
    bcm_port_t         port_id;              /* Port ID.            */
    bcm_trunk_t        trunk_id;             /* Trunk ID.           */
    int                local_id;             /* Hardware ID.        */
    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

    if (station->flags & BCM_L2_STATION_OAM) {
        if (0 == sc->port_entries_free) {
            return (BCM_E_RESOURCE);
        }
    } else if (station->flags & BCM_L2_STATION_OLP) { 
        if (0 == sc->olp_entries_free) {
            return (BCM_E_RESOURCE);
        }
    } else if (station->flags & BCM_L2_STATION_XGS_MAC) { 
        /* do nothing */
    } else if (0 == sc->entries_free)  {
        return (BCM_E_RESOURCE);
    }

    ent_p = (_bcm_l2_station_entry_t *)
            sal_alloc(sizeof(_bcm_l2_station_entry_t), "Sw L2 station entry");
    if (NULL == ent_p) {
        return (BCM_E_MEMORY);
    }

    sal_memset(ent_p, 0, sizeof(_bcm_l2_station_entry_t));


    ent_p->sid = sid;

    ent_p->prio = station->priority;

    if (station->flags & BCM_L2_STATION_OAM) {
        /* Get index to PORT table from source port */ 
        if (BCM_GPORT_IS_SET(station->src_port)) {
            rv = _bcm_esw_gport_resolve(unit, station->src_port,  &module_id,
                                        &port_id, &trunk_id, &local_id);
            if (BCM_FAILURE(rv)) {
                sal_free(ent_p);
                return (rv);
            }
            rv = _bcm_kt2_modport_to_pp_port_get(unit, module_id, port_id,
                                                &index);
            if (BCM_FAILURE(rv)) {
                sal_free(ent_p);
                return (rv);
            }
        } else {
            index = station->src_port;
        }
        if (NULL == sc->entry_arr[index + sc->entries_total]) {
            ent_p->hw_index = index;
        } else if (!(station->flags & BCM_L2_STATION_REPLACE)) {
            sal_free(ent_p);
            return BCM_E_PARAM;
        }
        ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_NON_TCAM;
        ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_OAM_MAC;
        /* Set MAC address in Port table */
        rv = _bcm_kt2_l2_port_mac_set(unit, index, station->dst_mac);
        if (BCM_FAILURE(rv)) {
            sal_free(ent_p);
            return (rv);
        }
        index += sc->entries_total;
        sc->port_entries_free--;
    } else if (station->flags & BCM_L2_STATION_OLP) {
        /* Get the first free slot. */
        index = sc->entries_total + sc->port_entries_total;
        index_max = index + sc->olp_entries_total;
        for (; index < index_max; index++) {
            if (NULL == sc->entry_arr[index]) {
                ent_p->hw_index = index - 
                                 (sc->entries_total + sc->port_entries_total);
                break;
            }
        }
        ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_NON_TCAM;
        ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_OLP_MAC;
        sc->olp_entries_free--;
        /* Set MAC address and DGLP in EGR_OLP_DGLP_CONFIG */
        rv = _bcm_l2_olp_mac_set(unit, ent_p->hw_index, station->src_port,
                                     station->dst_mac, OLP_ACTION_ADD);
        if (BCM_FAILURE(rv)) {
            sal_free(ent_p);
            sc->olp_entries_free++;
            return (rv);
        }
    } else if (station->flags & BCM_L2_STATION_XGS_MAC) {
        index = sc->entries_total + sc->port_entries_total + 
                                    sc->olp_entries_total;
        ent_p->hw_index = 0;
        ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_NON_TCAM;
        ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_XGS_MAC;
        /* Set MAC address in IARB_OLP_CONFIG and EGR_OLP_CONFIG */
        rv = _bcm_l2_xgs_mac_set(unit, 0, station->dst_mac);
        if (BCM_FAILURE(rv)) {
            sal_free(ent_p);
            return (rv);
        }
    } else {

        /* Get the first free slot. */
        for (index = 0; index < sc->entries_total; index++) {
            if (NULL == sc->entry_arr[index]) {
                ent_p->hw_index = index;
                break;
            }
        }

        rv = _bcm_l2_station_tcam_mem_get(unit,
             _BCM_L2_STATION_ENTRY_UNDERLAY, &tcam_mem);
        if (BCM_FAILURE(rv)) {
            sal_free(ent_p);
            return (rv);
        }

        mem_size = sizeof(my_station_tcam_entry_t);

        ent_p->tcam_ent = sal_alloc(mem_size, "L2 station entry buffer");
        if (NULL == ent_p->tcam_ent) {
            sal_free(ent_p);
            return (BCM_E_MEMORY);
        }

        sal_memset(ent_p->tcam_ent, 0, mem_size);

        rv = _bcm_l2_station_entry_set(unit, tcam_mem, station, ent_p);
        if (BCM_FAILURE(rv)) {
            sal_free(ent_p);
            return (rv);
        }

        sc->entries_free--;
        sc->entry_count++;
    }
    sc->entry_arr[index] = ent_p;
    *ent_pp = ent_p;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_l2_station_entry_update
 * Purpose:
 *     Allocate an entry buffer and setup entry parameters for hardware write
 *     operation.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     sid        - (IN) Station ID.
 *     station    - (IN) Station parameters to be set in hardware.
 *     ent_p      - (OUT) Pointer to Station entry type data.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_l2_station_entry_update(int unit,
                             int sid,
                             bcm_l2_station_t *station,
                             _bcm_l2_station_entry_t *ent_p)
{
    soc_mem_t   tcam_mem;    /* TCAM memory name.         */
    int         rv;           /* Operation return status. */
    int         mem_size = 0; /* Entry buffer size.       */

    if (station->flags & BCM_L2_STATION_OAM) {
        if (ent_p->flags & _BCM_L2_STATION_ENTRY_TYPE_OAM_MAC) {
            rv = _bcm_kt2_l2_port_mac_set(unit, ent_p->hw_index, 
                                          station->dst_mac);
        } else {
            rv = BCM_E_PARAM; 
        }
        return rv;
    } else if (station->flags & BCM_L2_STATION_OLP) {
        if (ent_p->flags & _BCM_L2_STATION_ENTRY_TYPE_OLP_MAC) {
            rv = _bcm_l2_olp_mac_set(unit, ent_p->hw_index, 
                                         station->src_port, 
                                         station->dst_mac, OLP_ACTION_UPDATE);
        } else {
            rv = BCM_E_PARAM; 
        }
        return rv;
    } else if (station->flags & BCM_L2_STATION_XGS_MAC) {
        if (ent_p->flags & _BCM_L2_STATION_ENTRY_TYPE_XGS_MAC) {
            rv = _bcm_l2_xgs_mac_set(unit, 0, station->dst_mac);
        } else {
            rv = BCM_E_PARAM; 
        }
        return rv;
    } else {
        rv = _bcm_l2_station_tcam_mem_get(unit,
                                          _BCM_L2_STATION_ENTRY_LEGACY,
                                          &tcam_mem);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }

        mem_size = sizeof(my_station_tcam_entry_t);

        ent_p->tcam_ent = sal_alloc(mem_size, "L2 station entry buffer");
        if (NULL == ent_p->tcam_ent) {
            return (BCM_E_MEMORY);
        }

        sal_memset(ent_p->tcam_ent, 0, mem_size);
    
        BCM_IF_ERROR_RETURN(_bcm_l2_station_entry_set(unit, tcam_mem, 
                               station, ent_p));
    }
    return (BCM_E_NONE);
}


#endif

/*
 * Function:
 *     _bcm_l2_station_entry_update
 * Purpose:
 *     Allocate an entry buffer and setup entry parameters for hardware write
 *     operation.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     sid        - (IN) Station ID.
 *     station    - (IN) Station parameters to be set in hardware.
 *     ent_p      - (OUT) Pointer to Station entry type data.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_l2_station_entry_update(int unit,
                             int sid,
                             bcm_l2_station_t *station,
                             _bcm_l2_station_entry_t *ent_p)
{
    soc_mem_t   tcam_mem;    /* TCAM memory name.         */
    int         rv;           /* Operation return status. */
    int         mem_size = 0; /* Entry buffer size.       */

    /* Input parameter check. */
    if (NULL == ent_p || NULL == station) {
        return (BCM_E_PARAM);
    }

    LOG_DEBUG(BSL_LS_BCM_L2,
              (BSL_META_U(unit,
                          "L2(unit %d) Info: (SID=%d) (idx=%d) (prio: o=%d n=%d) "
                           "- update.\n"), unit, sid, ent_p->hw_index, ent_p->prio,
               station->priority));

    if (ent_p->prio == station->priority) {
        ent_p->flags |= _BCM_L2_STATION_ENTRY_PRIO_NO_CHANGE;
    } else {
        ent_p->prio = station->priority;
    }

#if defined(BCM_KATANA2_SUPPORT) 
    if (SOC_IS_KATANA2(unit)) {
        rv = _bcm_kt2_l2_station_entry_update(unit, sid, station, ent_p);
        return (rv);
    }
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam) &&
            ((station->flags & BCM_L2_STATION_OLP) ||
             (station->flags & BCM_L2_STATION_XGS_MAC))) {
        
        if (station->flags & BCM_L2_STATION_OLP) {
            if (ent_p->flags & _BCM_L2_STATION_ENTRY_TYPE_OLP_MAC) {
                rv = _bcm_l2_olp_mac_set(unit, ent_p->hw_index, 
                        station->src_port, 
                        station->dst_mac, OLP_ACTION_UPDATE);
            } else {
                rv = BCM_E_PARAM; 
            }
        } else {
            /* BCM_L2_STATION_XGS_MAC */
            if (ent_p->flags & _BCM_L2_STATION_ENTRY_TYPE_XGS_MAC) {
                rv = _bcm_l2_xgs_mac_set(unit, 0, station->dst_mac);
            } else {
                rv = BCM_E_PARAM; 
            }
        } 
        return rv;
    }
#endif                             

    rv = _bcm_l2_station_tcam_mem_get(unit, _BCM_L2_STATION_OVERLAY(station) ||
              _BCM_L2_STATION_2(station), &tcam_mem);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

#ifdef BCM_RIOT_SUPPORT
    if (_BCM_L2_STATION_OVERLAY(station) || _BCM_L2_STATION_2(station)) {
        mem_size = sizeof(my_station_tcam_2_entry_t);
    } else
#endif /* BCM_RIOT_SUPPORT */
    if (SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH(unit)
        || SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit)
        || SOC_IS_VALKYRIE(unit) || SOC_IS_VALKYRIE2(unit)
        || SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit)) {

        mem_size = sizeof(mpls_station_tcam_entry_t);

    } else if (SOC_IS_KATANAX(unit) || SOC_IS_TRIDENT(unit)
               || SOC_IS_TD2_TT2(unit) || SOC_IS_TRIUMPH3(unit)
               || soc_feature(unit, soc_feature_gh2_my_station)) {

        mem_size = sizeof(my_station_tcam_entry_t);

    }

    if (0 == mem_size) {
        /* Must be a valid mem_size value. */
        return (BCM_E_INTERNAL);
    }

    ent_p->tcam_ent = sal_alloc(mem_size, "L2 station entry buffer");
    if (NULL == ent_p->tcam_ent) {
        return (BCM_E_MEMORY);
    }

    sal_memset(ent_p->tcam_ent, 0, mem_size);
    
    BCM_IF_ERROR_RETURN(_bcm_l2_station_entry_set(unit, tcam_mem, 
                           station, ent_p));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_l2_station_entry_create
 * Purpose:
 *     Allocate an entry buffer and setup entry parameters for hardware write
 *     operation.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     sid        - (IN) Station ID.
 *     station    - (IN) Station parameters to be set in hardware.
 *     ent_p      - (OUT) Pointer to Station entry type data.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_l2_station_entry_create(int unit,
                             int sid,
                             bcm_l2_station_t *station,
                             _bcm_l2_station_entry_t **ent_pp)
{
    _bcm_l2_station_control_t *sc;           /* Station control structure. */
    int                       index;         /* Entry index.               */
    _bcm_l2_station_entry_t   *ent_p = NULL; /* L2 station entry pointer.  */
    soc_mem_t                 tcam_mem;      /* TCAM memory name.          */ 
    int                       mem_size = 0;  /* Entry buffer size.         */
    int                       rv;            /* Operation return status.   */
    int                       entries_total;
    int                       overlay;
    _bcm_l2_station_entry_t   **entry_arr;

    /* Input parameter check. */
    if (NULL == station || NULL == ent_pp) {
        return (BCM_E_PARAM);
    }
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        rv = _bcm_kt2_l2_station_entry_create(unit, sid, station, ent_pp);
        return rv;
    } 
#endif    
    
    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

#ifdef BCM_RIOT_SUPPORT
    if (_BCM_L2_STATION_ID_OVERLAY(sid) || _BCM_L2_STATION_ID_2(sid)) {
        if (0 == sc->entries_free_2) {
            return (BCM_E_RESOURCE);
        }
        overlay = _BCM_L2_STATION_ENTRY_OVERLAY;
        entry_arr = (sc->entry_arr_2);
        entries_total = sc->entries_total_2;
    } else
#endif /* BCM_RIOT_SUPPORT */
    {
        if (0 == sc->entries_free) {
            return (BCM_E_RESOURCE);
        }
        overlay = _BCM_L2_STATION_ENTRY_UNDERLAY;
        entry_arr = (sc->entry_arr);
        entries_total = sc->entries_total;
    }

    ent_p = (_bcm_l2_station_entry_t *)
        sal_alloc(sizeof(_bcm_l2_station_entry_t), "Sw L2 station entry");
    if (NULL == ent_p) {
        return (BCM_E_MEMORY);
    }

    sal_memset(ent_p, 0, sizeof(_bcm_l2_station_entry_t));
    ent_p->sid = sid;
    ent_p->prio = station->priority;

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam) &&
            ((station->flags & BCM_L2_STATION_OLP) || 
             (station->flags & BCM_L2_STATION_XGS_MAC)) &&
             (overlay == _BCM_L2_STATION_ENTRY_UNDERLAY)) {
        int index_max;

        if ((station->flags & BCM_L2_STATION_OLP) &&  
                (0 == sc->olp_entries_free)) {
            sal_free(ent_p);
            return (BCM_E_RESOURCE);
        }
        if (station->flags & BCM_L2_STATION_OLP) {
            index = sc->entries_total;
            index_max = index + sc->olp_entries_total;
            for (; index < index_max; index++) {
                if (NULL == sc->entry_arr[index]) {
                    ent_p->hw_index = index - sc->entries_total; 
                    break;
                }
            }
            ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_NON_TCAM;
            ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_OLP_MAC;
            sc->olp_entries_free--;
            /* Set MAC address and DGLP in EGR_OLP_DGLP_CONFIG */
            rv = _bcm_l2_olp_mac_set(unit, ent_p->hw_index, station->src_port,
                    station->dst_mac, OLP_ACTION_ADD);
            if (BCM_FAILURE(rv)) {
                sal_free(ent_p);
                sc->olp_entries_free++;
                return (rv);
            }
        } else {
            /* BCM_L2_STATION_XGS_MAC */
            index = sc->entries_total + sc->olp_entries_total;
            ent_p->hw_index = 0;
            ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_NON_TCAM;
            ent_p->flags |= _BCM_L2_STATION_ENTRY_TYPE_XGS_MAC;
            /* Set MAC address in IARB_OLP_CONFIG and EGR_OLP_CONFIG */
            rv = _bcm_l2_xgs_mac_set(unit, 0, station->dst_mac);
            if (BCM_FAILURE(rv)) {
                sal_free(ent_p);
                return (rv);
            }
        }
        sc->entry_arr[index] = ent_p;
        *ent_pp = ent_p;
        return (BCM_E_NONE);
    }
#endif

    if (0 == sc->entries_free) {
        sal_free(ent_p);
        return (BCM_E_RESOURCE);
    }

    /* Get the first free slot. */
    for (index = 0; index < entries_total; index++) {
        if (NULL == entry_arr[index]) {
            ent_p->hw_index = index;
            break;
        }
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit) && (station->flags & BCM_L2_STATION_FCOE)) {
        tcam_mem = MY_STATION_TCAM_2m;
    } else 
#endif /* BCM_TRIDENT3_SUPPORT */
    {
    rv = _bcm_l2_station_tcam_mem_get(unit, overlay, &tcam_mem);
    if (BCM_FAILURE(rv)) {
        sal_free(ent_p);
        return (rv);
    }
    }

#ifdef BCM_RIOT_SUPPORT
    if (_BCM_L2_STATION_OVERLAY(station) || _BCM_L2_STATION_2(station)) {
        mem_size = sizeof(my_station_tcam_2_entry_t);
    } else
#endif /* BCM_RIOT_SUPPORT */
    if (SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH(unit)
        || SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit)
        || SOC_IS_VALKYRIE(unit) || SOC_IS_VALKYRIE2(unit)
        || SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit)) {

        mem_size = sizeof(mpls_station_tcam_entry_t);

    } else if (SOC_IS_KATANAX(unit) || SOC_IS_TRIDENT(unit)
               || SOC_IS_TD2_TT2(unit) || SOC_IS_TRIUMPH3(unit)
               || soc_feature(unit, soc_feature_gh2_my_station)) {
        mem_size = sizeof(my_station_tcam_entry_t);
    }

    if (0 == mem_size) {
        /* mem_size must be greater than zero. */
        sal_free(ent_p);
        return (BCM_E_INTERNAL);
    }

    ent_p->tcam_ent = sal_alloc(mem_size, "L2 station entry buffer");
    if (NULL == ent_p->tcam_ent) {
        sal_free(ent_p);
        return (BCM_E_MEMORY);
    }

    sal_memset(ent_p->tcam_ent, 0, mem_size);

    rv = _bcm_l2_station_entry_set(unit, tcam_mem, station, ent_p);
    if (BCM_FAILURE(rv)) {
        sal_free(ent_p);
        return (rv);
    }

    /* Decrement free entries count. */
#ifdef BCM_RIOT_SUPPORT
    if (_BCM_L2_STATION_OVERLAY(station) || _BCM_L2_STATION_2(station)) {
        sc->entries_free_2--;
        sc->entry_count_2++;
        sc->entry_arr_2[index] = ent_p;
    } else
#endif /* BCM_RIOT_SUPPORT */
    {
        sc->entries_free--;
        sc->entry_count++;
        sc->entry_arr[index] = ent_p;
    }

    *ent_pp = ent_p;

    return (BCM_E_NONE);

}

/*
 * Function:
 *     _bcm_l2_station_entry_prio_cmp
 * Purpose:
 *     Compare station entry priority values.
 * Parameters:
 *     prio_first   - (IN) First entry priority value.
 *     prio_second  - (IN) Second entry priority value.
 * Returns:
 *     0    - if both priority values are equal.
 *     -1   - if priority value of first entry is lower than second.
 *     1    - if priority value of first entry is greater than second.
 */
STATIC int
_bcm_l2_station_entry_prio_cmp(int prio_first, int prio_second)
{
    int rv; /* Comparison result. */

    if (prio_first == prio_second) {
        rv = 0;
    } else if (prio_first < prio_second) {
        rv = -1;
    } else {
        rv = 1;
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_l2_station_prio_move_required
 * Purpose:
 *     Check if entry move is required based on priority value.
 * Parameters:
 *     unit   - (IN) BCM device number
 *     ent_p  - (IN) Station entry for priority comparison.
 * Returns:
 *     TRUE   - Entry move is required based on priority comparison.
 *     FALSE  - Entry move is not required based on priority comparison.
 */
STATIC int
_bcm_l2_station_prio_move_required(int unit, _bcm_l2_station_entry_t *ent_p)
{
    _bcm_l2_station_control_t *sc;      /* station control structure.   */
    _bcm_l2_station_entry_t **entry_arr;/* station entry array pointer  */
    uint32                    count;    /* Maximum number of entries.   */
    int                       i;        /* Temporary iterator variable. */
    int                       ent_prio; /* Entry priority.              */
    int                       flag;     /* Flag value denotes if we are */
                                        /* before or after s_ent.       */

    /* Input parameter check. */
    if (NULL == ent_p) {
        return (BCM_E_INTERNAL);
    }

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

    /* Get total number for entries supported. */
#ifdef BCM_RIOT_SUPPORT
    if (_BCM_L2_STATION_ID_OVERLAY(ent_p->sid) ||
         _BCM_L2_STATION_ID_2(ent_p->sid)) {
        count = sc->entries_total_2;
        entry_arr = (sc->entry_arr_2);
    } else
#endif /* BCM_RIOT_SUPPORT */
    {
        count = sc->entries_total;
        entry_arr = (sc->entry_arr);
    }

    /* Get the entry priority value. */
    ent_prio = ent_p->prio;

    /* Init flag value, we are before s_ent. */
    flag = -1;

    for (i = 0; i < count; i++) {

        if (entry_arr[i] == ent_p) {
            /* Now we are after s_ent. */
            flag = 1;
            continue;
        }

        if (NULL == entry_arr[i]) {
            continue;
        }

        if (-1 == flag) {
            if (_bcm_l2_station_entry_prio_cmp
                    (entry_arr[i]->prio, ent_prio) < 0) {

                LOG_DEBUG(BSL_LS_BCM_L2,
                          (BSL_META_U(unit,
                                      "L2(unit %d) Info: (SID=%d) found lower prio than "
                                       "(prio=%d).\n"), unit, ent_p->sid, ent_p->prio));

                /*
                 * An entry  before s_ent has lower priority.
                 * So, move is required.
                 */
                return (TRUE);
            }
        } else {
            if (_bcm_l2_station_entry_prio_cmp
                    (entry_arr[i]->prio, ent_prio) > 0) {
                LOG_DEBUG(BSL_LS_BCM_L2,
                          (BSL_META_U(unit,
                                      "L2(unit %d) Info: (SID=%d) found higher prio than "
                                       "(prio=%d).\n"), unit, ent_p->sid, ent_p->prio));
                /*
                 * An entry after s_ent has higher priority than s_ent prio.
                 * So, move is required.
                 */
                return (TRUE);
            }
        }
    }

    LOG_DEBUG(BSL_LS_BCM_L2,
              (BSL_META_U(unit,
                          "L2(unit %d) Info: (SID=%d) (prio=%d) no move.\n"),
               unit, ent_p->sid, ent_p->prio));
    return (FALSE);
}

/*
 * Function:
 *     _bcm_l2_station_entry_move_check
 * Purpose:
 *     Calculate number of entries that need to be moved for installing the
 *     new station entry.
 * Parameters:
 *     unit                     - (IN) BCM device number
 *     null_index               - (IN) Free null entry hardware index.
 *     target_index             - (IN) Target hardware index for the entry.
 *     direction                - (IN) Direction of Move.
 *     shifted_entries_count    - (OUT) No. of entries to be moved.
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_bcm_l2_station_entry_move_check(int unit, int null_index,
                                 int target_index, int direction,
                                 int *shifted_entries_count)
{
    *shifted_entries_count += direction * (null_index - target_index);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_l2_station_entry_dir_get
 * Purpose:
 *     Find out direction in which entries need to be moved for installing
 *     new station entry in hardware.
 * Parameters:
 *     unit             - (IN) BCM device number
 *     ent_p            - (IN) Station entry pointer.
 *     prev_null_index  - (IN) Location of previous null entry in hardware.
 *     next_null_index  - (IN) Location of next null entry in hardware.
 *     target_index     - (IN) Target index where entry needs to be installed.
 *     dir              - (OUT) Direction in which entries need to be moved.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_l2_station_entry_dir_get(int unit,
                              _bcm_l2_station_entry_t *ent_p,
                              int prev_null_index,
                              int next_null_index,
                              int target_index,
                              int *dir)
{
    _bcm_l2_station_control_t  *sc;                   /* Station control    */
                                                      /* strucutre.         */
    int                        shift_up = FALSE;      /* Shift UP status.   */
    int                        shift_down = FALSE;    /* Shift DOWN status. */
    int                        shift_up_amount = 0;   /* Shift UP amount.   */
    int                        shift_down_amount = 0; /* Shift DOWN amount. */
    int                        rv;                    /* Operation return   */
                                                      /* status.            */

    /* Input parameter check. */
    if (NULL == ent_p || NULL == dir) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

    if (-1 != prev_null_index) {
        rv = _bcm_l2_station_entry_move_check(unit, prev_null_index,
                                              target_index, -1,
                                              &shift_up_amount);
        shift_up = BCM_SUCCESS(rv);
    } else {
        shift_up = FALSE;
    }

    if (-1 != next_null_index) {
        rv = _bcm_l2_station_entry_move_check(unit, next_null_index,
                                              target_index, 1,
                                              &shift_down_amount);
        shift_down = BCM_SUCCESS(rv);
    } else {
        shift_down = FALSE;
    }

    if (TRUE == shift_up) {
        if (TRUE == shift_down) {
            if (shift_up_amount < shift_down_amount) {
                *dir = -1;
            } else {
                *dir = 1;
            }
        } else {
            *dir = -1;
        }
    } else {
        if (TRUE == shift_down) {
            *dir = 1;
        } else {
            return FALSE;
        }
    }
    return TRUE;
}

/*
 * Function:
 *     _bcm_l2_station_entry_move
 * Purpose:
 *     Move a station entry by a specified amount.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     s_ent    - (IN) Station entry pointer.
 *     amount   - (IN) Direction in which entries need to be moved.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_l2_station_entry_move(int unit,
                           _bcm_l2_station_entry_t *s_ent,
                           int amount)
{
    _bcm_l2_station_control_t   *sc;          /* Station control structure. */
    _bcm_l2_station_entry_t   **entry_arr;    /* Station entry pointer . */
    int                         tcam_idx_old; /* Original entry TCAM index. */
    int                         tcam_idx_new; /* Entry new TCAM index.      */
    int                         rv;           /* Operation return status.   */
    soc_mem_t                   tcam_mem;     /* TCAM memory name.          */
    uint32                      entry[SOC_MAX_MEM_FIELD_WORDS]; /* Entry    */
                                                                /* buffer.  */
    int                         count;
    int                         overlay;
    /* Input parameter check. */
    if (NULL == s_ent) {
        return (BCM_E_PARAM);
    }

    /* Input parameter check. */
    if (0 == amount) {
        return (BCM_E_NONE);
    }

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

    tcam_idx_old =  s_ent->hw_index;

    tcam_idx_new = tcam_idx_old + (amount);

    LOG_DEBUG(BSL_LS_BCM_L2,
              (BSL_META_U(unit,
                          "L2(unit %d) Info: (SID=%d) move (oidx=%d nidx=%d) (amt=%d).\n"),
               unit, s_ent->sid, s_ent->hw_index, tcam_idx_new, amount));

#ifdef BCM_RIOT_SUPPORT
    if (_BCM_L2_STATION_ID_OVERLAY(s_ent->sid) ||
        _BCM_L2_STATION_ID_2(s_ent->sid)) {
        overlay = _BCM_L2_STATION_ENTRY_OVERLAY;
        count = sc->entries_total_2;
        entry_arr = (sc->entry_arr_2);
    } else
#endif /* BCM_RIOT_SUPPORT */
    {
        overlay = _BCM_L2_STATION_ENTRY_UNDERLAY;
        count = sc->entries_total;
        entry_arr = (sc->entry_arr);
    }

    if (tcam_idx_old < 0 || tcam_idx_old >= count) {
        return (BCM_E_PARAM);
    }

    rv = _bcm_l2_station_tcam_mem_get(unit, overlay, &tcam_mem);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    if (s_ent->flags & _BCM_L2_STATION_ENTRY_INSTALLED) {
        /* Clear entry buffer. */
        sal_memset(entry, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

        rv = soc_mem_read(unit, tcam_mem, MEM_BLOCK_ANY, tcam_idx_old, entry);
        BCM_IF_ERROR_RETURN(rv);

        rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx_new, entry);
        BCM_IF_ERROR_RETURN(rv);

        rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx_old,
                           soc_mem_entry_null(unit, tcam_mem));
        BCM_IF_ERROR_RETURN(rv);
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit) && (!soc_feature(unit, soc_feature_riot))) {
            if (tcam_mem == MY_STATION_TCAMm) {
                tcam_mem = MY_STATION_TCAM_2m;
                rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx_new, entry);
                BCM_IF_ERROR_RETURN(rv);

                rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx_old,
                                   soc_mem_entry_null(unit, tcam_mem));
                BCM_IF_ERROR_RETURN(rv);
            }
        }
#endif
    }

    if (FALSE == prio_with_no_free_entries) {
        entry_arr[s_ent->hw_index] = NULL;
    }

    entry_arr[tcam_idx_new] = s_ent;

    s_ent->hw_index = tcam_idx_new;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_l2_station_entry_shift_down
 * Purpose:
 *     Shift entries down by one index to create a free entry at target
 *     index location.
 * Parameters:
 *     unit             - (IN) BCM device number
 *     target_index     - (IN) Target index for entry install operation.
 *     next_null_index  - (IN) Location of free entry after target index.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_l2_station_entry_shift_down(int unit,
                                 int overlay,
                                 int target_index,
                                 int next_null_index)
{
    uint16                      empty_idx;   /* Empty index.               */
    _bcm_l2_station_control_t   *sc;         /* Station control structure. */
    _bcm_l2_station_entry_t     **entry_arr; /* Station entry pointer */
    int                         tmp_idx1;    /* Temporary index.           */
    int                         tmp_idx2;    /* Temporary index.           */ 
    int                         max_entries; /* Maximum entries supported. */
    int                         rv;          /* Operation return status.   */

    LOG_DEBUG(BSL_LS_BCM_L2,
              (BSL_META_U(unit,
                          "L2(unit %d) Info: Shift UP (tidx=%d null-idx=%d).\n"),
               unit, target_index, next_null_index));

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

    empty_idx = next_null_index;

#ifdef BCM_RIOT_SUPPORT
    if (overlay == _BCM_L2_STATION_ENTRY_OVERLAY) {
        max_entries = sc->entries_total_2;
        entry_arr = (sc->entry_arr_2);
    } else
#endif /* BCM_RIOT_SUPPORT */
    {
        max_entries = sc->entries_total;
        entry_arr = (sc->entry_arr);
    }

    /*
     * Move entries one step down
     *     starting from the last entry
     * IDEA: 
     *     Move till empty_idx > target_index  
     */
    while (empty_idx > target_index) {
        if (empty_idx == 0) {
            tmp_idx1 = 0;
            tmp_idx2 = max_entries - 1;
            rv = _bcm_l2_station_entry_move(unit,
                                            entry_arr[max_entries - 1],
                                            (tmp_idx1 - tmp_idx2));
            BCM_IF_ERROR_RETURN(rv);
        } else {
            tmp_idx1 = empty_idx;
            tmp_idx2 = (empty_idx - 1);
            rv = _bcm_l2_station_entry_move(unit,
                                            entry_arr[empty_idx - 1],
                                            (tmp_idx1 - tmp_idx2));
            BCM_IF_ERROR_RETURN(rv);

            empty_idx--;
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_l2_station_entry_shift_up
 * Purpose:
 *     Shift entries up by one index to create a free entry at target
 *     index location.
 * Parameters:
 *     unit             - (IN) BCM device number
 *     target_index     - (IN) Target index for entry install operation.
 *     prev_null_index  - (IN) Location of free entry before target index.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_l2_station_entry_shift_up(int unit,
                               int overlay,
                               int target_index,
                               int prev_null_index)
{
    uint16                      empty_idx;   /* Empty index.               */
    _bcm_l2_station_control_t   *sc;         /* Station control structure. */
    _bcm_l2_station_entry_t     **entry_arr; /* Station entry pointer */
    int                         tmp_idx1;    /* Temporary index.           */
    int                         tmp_idx2;    /* Temporary index.           */ 
    int                         max_entries; /* Maximum entries supported. */
    int                         rv;          /* Operation return status.   */

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

    LOG_DEBUG(BSL_LS_BCM_L2,
              (BSL_META_U(unit,
                          "L2(unit %d) Info: Shift UP (tidx=%d null-idx=%d).\n"),
               unit, target_index, prev_null_index));

    empty_idx = prev_null_index;

#ifdef BCM_RIOT_SUPPORT
    if (overlay == _BCM_L2_STATION_ENTRY_OVERLAY) {
        max_entries = sc->entries_total_2;
        entry_arr = (sc->entry_arr_2);
    } else
#endif /* BCM_RIOT_SUPPORT */
    {
        max_entries = sc->entries_total;
        entry_arr = (sc->entry_arr);
    }


    /*
     * Move entries one step up
     *     starting from the first entry
     * IDEA: 
     *     Move till empty_idx < target_index  
     */
    while (empty_idx < target_index) {
        if (empty_idx == (max_entries - 1)) {
            tmp_idx1 = max_entries - 1;
            tmp_idx2 = 0;
            rv = _bcm_l2_station_entry_move(unit,
                                            entry_arr[0],
                                            (tmp_idx1 - tmp_idx2));
            BCM_IF_ERROR_RETURN(rv);
            empty_idx = 0;
        } else {
            tmp_idx1 = empty_idx;
            tmp_idx2 = (empty_idx + 1);
            rv = _bcm_l2_station_entry_move(unit,
                                            entry_arr[empty_idx + 1],
                                            (tmp_idx1 - tmp_idx2));
            BCM_IF_ERROR_RETURN(rv);

            empty_idx++;
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_l2_station_entry_install
 * Purpose:
 *     Write a station entry to hardware
 * Parameters:
 *     unit     - (IN) BCM device number
 *     ent_p    - (IN) Pointer to station entry.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_l2_station_entry_install(int unit, _bcm_l2_station_entry_t *ent_p, bcm_l2_station_t *station)
{
    int     rv;         /* Operation return status. */ 
    soc_mem_t tcam_mem; /* TCAM memory name.        */

    if (NULL == ent_p) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit) && (station->flags & BCM_L2_STATION_FCOE)) {
        tcam_mem = MY_STATION_TCAM_2m;
    } else 
#endif /* BCM_TRIDENT3_SUPPORT */
    {
    rv = _bcm_l2_station_tcam_mem_get(unit,
              _BCM_L2_STATION_ID_OVERLAY(ent_p->sid) ||
              _BCM_L2_STATION_ID_2(ent_p->sid), &tcam_mem);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }
    }

    if (NULL == ent_p->tcam_ent) {
        return (BCM_E_INTERNAL);
    }

    LOG_DEBUG(BSL_LS_BCM_L2,
              (BSL_META_U(unit,
                          "L2(unit %d) Info: (SID=%d) - install (idx=%d).\n"),
               unit, ent_p->sid, ent_p->hw_index));

    rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, ent_p->hw_index,
                       ent_p->tcam_ent);
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit) && (!(soc_feature(unit, soc_feature_riot) ||
                              _bcm_l2_p2p_access_vlan_check_enabled(unit)))) {
        BCM_IF_ERROR_RETURN(rv);
        if (tcam_mem == MY_STATION_TCAMm) {
            tcam_mem = MY_STATION_TCAM_2m;
            if (soc_feature(unit, soc_feature_mysta_profile)) {
                uint32 mem_no = 1;
                uint32 mysta_profile_id;
                my_station_profile_2_entry_t entry;
                soc_mem_t mysp_mem = MY_STATION_PROFILE_2m;

                sal_memset(&entry, 0, sizeof(entry));

                _bcm_l2_mysta_station_to_entry(unit, station, mysp_mem, &entry);
                rv = _bcm_l2_mysta_profile_entry_add(unit, &entry, mem_no,
                                                    &mysta_profile_id);
                BCM_IF_ERROR_RETURN(rv);

                soc_mem_field32_dest_set(unit, tcam_mem, ent_p->tcam_ent,
                    DESTINATIONf, SOC_MEM_FIF_DEST_MYSTA, mysta_profile_id);
            }

            rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, ent_p->hw_index,
                       ent_p->tcam_ent);
        }
    }
#endif
    sal_free(ent_p->tcam_ent);

    ent_p->tcam_ent = NULL;

    return (rv);
}

/*
 * Function:
 *     _bcm_l2_station_entry_prio_install
 * Purpose:
 *     Install an entry based on priority value.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     ent_p    - (IN) Pointer to station entry.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_l2_station_entry_prio_install(int unit, _bcm_l2_station_entry_t *ent_p, bcm_l2_station_t *station)
{
    _bcm_l2_station_control_t  *sc;                /* Control structure.     */
    _bcm_l2_station_entry_t  **entry_arr;          /* L2 station entry       */
    int                        idx_old;            /* Entry old TCAM index.  */
    int                        idx_target;         /* Entry target index.    */
    int                        prev_null_idx = -1; /* Prev NULL entry index. */
    int                        next_null_idx = -1; /* Next NULL entry index. */
    int                        temp;               /* Temporary variable.    */
    int                        dir = -1;           /* -1 = UP, 1 = DOWN      */
    int                        decr_on_shift_up = TRUE; /* Shift up status   */
    int                        rv;                 /* Return status.         */
    int                        flag_no_free_entries = FALSE; /* No free      */
                                                   /* entries status         */
    int                        max_entries; 
    int                        free_entries; 
    int                        overlay;
    prio_with_no_free_entries = FALSE;

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    if ((SOC_IS_KATANA2(unit) || 
            soc_feature(unit, soc_feature_fp_based_oam)) && 
        (ent_p->flags & _BCM_L2_STATION_ENTRY_TYPE_NON_TCAM)) {
        return BCM_E_NONE;  
    }
#endif

    if ((ent_p->flags & _BCM_L2_STATION_ENTRY_PRIO_NO_CHANGE)
        || (FALSE == _bcm_l2_station_prio_move_required(unit, ent_p))) {
        goto end;
    }

    idx_old = ent_p->hw_index;

#ifdef BCM_RIOT_SUPPORT
    if (_BCM_L2_STATION_ID_OVERLAY(ent_p->sid) ||
        _BCM_L2_STATION_ID_2(ent_p->sid)) {
        overlay = _BCM_L2_STATION_ENTRY_OVERLAY;
        max_entries = sc->entries_total_2;
        entry_arr = (sc->entry_arr_2);
        free_entries = sc->entries_free_2;
    } else
#endif /* BCM_RIOT_SUPPORT */
    {
        overlay = _BCM_L2_STATION_ENTRY_UNDERLAY;
        max_entries = sc->entries_total;
        entry_arr = (sc->entry_arr);
        free_entries  = sc->entries_free;
    }

    if (idx_old >= max_entries) {
        return (BCM_E_INTERNAL);
    }

    if (0 == free_entries) {
        if (0 == (ent_p->flags & _BCM_L2_STATION_ENTRY_INSTALLED)) {
            entry_arr[ent_p->hw_index] = NULL;
            flag_no_free_entries = TRUE;
        } else {
            return (BCM_E_CONFIG);
        }
    }

    for (idx_target = 0; idx_target < max_entries; idx_target++) {

        /* Skip the entry itself. */
        if (ent_p == entry_arr[idx_target]) {
            continue;
        }

        if (NULL == entry_arr[idx_target]) {
            /* Store the null slot index. */
            prev_null_idx = idx_target;
            continue;
        }

        if (_bcm_l2_station_entry_prio_cmp
            (ent_p->prio, entry_arr[idx_target]->prio) > 0) {
            /* Found the entry with priority lower than ent_p prio. */
            break;
        }
    }

    temp = idx_target;

    if (idx_target != (max_entries - 1)) {
        /*  Find next empty slot after ent_p. */
        for (; temp < max_entries; temp++) {
            if (NULL == entry_arr[temp]) {
                next_null_idx = temp;
                break;
            }
        }
    }

    /* Put the entry back. */
    if (TRUE == flag_no_free_entries) {
        entry_arr[ent_p->hw_index] = ent_p;
    }

    /* No empty slot found. */
    if (-1 == prev_null_idx && -1 == next_null_idx) {
        return (BCM_E_CONFIG);
    }

    if (idx_target == max_entries) {
        /* Target location is after the last installed entry. */
        if (prev_null_idx == (max_entries - 1)) {
            idx_target = prev_null_idx;
            goto only_move;
        } else {
            idx_target = (max_entries - 1);
            decr_on_shift_up = FALSE;
        }
    }

    if (FALSE
        == _bcm_l2_station_entry_dir_get(unit,
                                         ent_p,
                                         prev_null_idx,
                                         next_null_idx,
                                         idx_target,
                                         &dir)) {
        return (BCM_E_PARAM);
    }

    if (1 == dir) {
        /* 
         * Move the entry at the target index to target_index+1. This may
         * mean shifting more entries down to make room. In other words,
         * shift the target index and any that follow it down 1 as far as the
         * next empty index.
         */
        if (NULL != entry_arr[idx_target]) {
            BCM_IF_ERROR_RETURN
                (_bcm_l2_station_entry_shift_down(unit,
                                                  overlay,
                                                  idx_target,
                                                  next_null_idx));
        }
    } else {
        if (TRUE == decr_on_shift_up) {
            idx_target--;
            LOG_DEBUG(BSL_LS_BCM_L2,
                      (BSL_META_U(unit,
                                  "L2(unit %d) Info: Decr. on Shift UP (idx_tgt=%d).\n"),
                       unit, idx_target));
        }
        if (NULL != entry_arr[idx_target]) {
            /* coverity[negative_returns] */
            BCM_IF_ERROR_RETURN
                (_bcm_l2_station_entry_shift_up(unit,
                                                overlay,
                                                idx_target,
                                                prev_null_idx));
        }
    }

    /* Move the entry from old index to new index. */
only_move:
    if (0 != (idx_target - ent_p->hw_index)) {

        if (TRUE == flag_no_free_entries) {
            prio_with_no_free_entries = TRUE;
        }

        rv = _bcm_l2_station_entry_move(unit,
                                        ent_p,
                                        (idx_target - ent_p->hw_index));
        if (BCM_FAILURE(rv)) {
            prio_with_no_free_entries = FALSE;
            return (rv);
        }

        prio_with_no_free_entries = FALSE;
    }

end:

    ent_p->flags &= ~_BCM_L2_STATION_ENTRY_PRIO_NO_CHANGE;

    rv = _bcm_l2_station_entry_install(unit, ent_p, station);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_tr_l2_station_entry_parse
 * Purpose:
 *     Get station information from H/W format.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     tcam_mem     - (IN) TCAM memory name.
 *     station      - (IN/OUT) Station entry lookup information.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_tr_l2_station_tcam_entry_parse(int unit, soc_mem_t tcam_mem, uint32 *entry,
                                        bcm_l2_station_t *station)
{
#ifdef BCM_RIOT_SUPPORT
    int                         overlay = 0; /* Overlay/Underlay indicator */
#endif /* BCM_RIOT_SUPPORT */

    /* Input parameter check. */
    if (tcam_mem != MY_STATION_TCAMm
        && tcam_mem != MY_STATION_TCAM_2m
        && tcam_mem != MPLS_STATION_TCAMm) {
        return (BCM_E_PARAM);
    }

    if (NULL == entry) {
        return (BCM_E_PARAM);
    }

    if (NULL == station) {
        return (BCM_E_PARAM);
    }

    soc_mem_mac_addr_get(unit, tcam_mem, entry,
                         MAC_ADDRf, station->dst_mac);

    soc_mem_mac_addr_get(unit, tcam_mem, entry,
                         MAC_ADDR_MASKf, station->dst_mac_mask);

#ifdef BCM_RIOT_SUPPORT
    if (tcam_mem == MY_STATION_TCAM_2m) {
        overlay = soc_mem_field32_get(unit, tcam_mem, entry,
                                      FORWARDING_FIELD_TYPEf);
    }

    if (overlay) {
        int vfi = 0;
        int vfi_mask = 0;

        vfi = soc_mem_field32_get(unit, tcam_mem, entry, VFI_IDf);
        _BCM_VPN_SET(station->vlan, _BCM_VPN_TYPE_VFI, vfi);

        vfi_mask = soc_mem_field32_get(unit, tcam_mem, entry, VFI_ID_MASKf);
        if (vfi_mask != 0) {
            _BCM_VPN_SET(station->vlan_mask, _BCM_VPN_TYPE_VFI, vfi_mask);
        }
    } else
#endif /* BCM_RIOT_SUPPORT */
    {
#ifdef BCM_RIOT_SUPPORT
        if (soc_feature(unit, soc_feature_riot)) {
            station->flags |= BCM_L2_STATION_UNDERLAY;
        }
#endif
        station->vlan
            = soc_mem_field32_get(unit, tcam_mem, entry, VLAN_IDf);

        station->vlan_mask
            = soc_mem_field32_get(unit, tcam_mem, entry, VLAN_ID_MASKf);
    }

    if (soc_feature(unit, soc_feature_my_station_2)) {
        if (soc_property_get(unit,
                             spn_MY_STATION_FORWARD_DOMAIN_TYPE_DECOUPLED,
                             0)) {
            station->forward_domain_type
                = soc_mem_field32_get(unit, tcam_mem, entry,
                                      FORWARDING_FIELD_TYPEf);

            station->forward_domain_type_mask
                = soc_mem_field32_get(unit, tcam_mem, entry,
                                      FORWARDING_FIELD_TYPE_MASKf);
        }
    }

#if defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_KATANAX(unit)
        || SOC_IS_TRIDENT(unit)
        || SOC_IS_TD2_TT2(unit)
        || SOC_IS_TRIUMPH3(unit)) {
        int src_field;
        int src_mask_field;
        int src_type = 0;
        _bcm_gport_dest_t src;
        _bcm_gport_dest_t src_mask;
        int num_bits_for_port;

        _bcm_gport_dest_t_init(&src);
        _bcm_gport_dest_t_init(&src_mask);

        if (SOC_MEM_FIELD_VALID(unit, tcam_mem, SOURCE_FIELDf)) {
            src_field= soc_mem_field32_get(unit, tcam_mem, entry,
                            SOURCE_FIELDf);
            src_mask_field = soc_mem_field32_get(unit, tcam_mem, entry,
                                  SOURCE_FIELD_MASKf);

            if (SOC_MEM_FIELD_VALID(unit, tcam_mem, SOURCE_TYPEf)) {
                src_type = soc_mem_field32_get(unit, tcam_mem, entry,
                                               SOURCE_TYPEf);
            }

#ifdef BCM_RIOT_SUPPORT
            if (soc_feature(unit, soc_feature_riot) && (src_type & 0x1)) {
                _bcm_vp_gport_dest_fill(unit, src_field, &src);
                src_mask.gport_type = src.gport_type;
                switch (src_mask.gport_type) {
                    case _SHR_GPORT_TYPE_MPLS_PORT:
                        src_mask.mpls_id = src_mask_field;
                        break;
                    case _SHR_GPORT_TYPE_MIM_PORT:
                        src_mask.mim_id = src_mask_field;
                        break;
                    case _SHR_GPORT_TYPE_TRILL_PORT:
                        src_mask.trill_id = src_mask_field;
                        break;
                    case _SHR_GPORT_TYPE_L2GRE_PORT:
                        src_mask.l2gre_id = src_mask_field;
                        break;
                    case _SHR_GPORT_TYPE_VXLAN_PORT:
                        src_mask.vxlan_id = src_mask_field;
                        break;
                    default:
                        return BCM_E_PARAM;
                }
            } else
#endif /* BCM_RIOT_SUPPORT */
            if ((SOC_IS_KATANAX(unit) && (src_type & 0x1)) ||
                    (src_field >> SOC_TRUNK_BIT_POS(unit)) & 0x1) {
                src.tgid = src_field & ((1 << SOC_TRUNK_BIT_POS(unit)) - 1);
                src.gport_type = _SHR_GPORT_TYPE_TRUNK;
                src_mask.tgid = src_mask_field &
                    ((1 << SOC_TRUNK_BIT_POS(unit)) - 1);
                src_mask.gport_type = _SHR_GPORT_TYPE_TRUNK;
            } else {
                src.port = src_field & SOC_PORT_ADDR_MAX(unit);
                src.modid = (src_field - src.port) /
                               (SOC_PORT_ADDR_MAX(unit) + 1);
                src.gport_type = _SHR_GPORT_TYPE_MODPORT;
                num_bits_for_port = _shr_popcount(
                                 (unsigned int)SOC_PORT_ADDR_MAX(unit));
                src_mask.port = src_mask_field & SOC_PORT_ADDR_MAX(unit);
                src_mask.gport_type = _SHR_GPORT_TYPE_MODPORT;
                src_mask.modid = (src_mask_field >> num_bits_for_port) &
                                 SOC_MODID_MAX(unit);
            }
            if (BCM_GPORT_IS_SET(station->src_port)) {
                /* caller is expecting gport format*/
                BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &src,
                                    &(station->src_port)));
                BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &src_mask,
                                    &(station->src_port_mask)));
            } else {
                /* only port and port_mask can be conveyed*/
                station->src_port      = src.port;
                station->src_port_mask = src_mask.port;
            }
        } else {
            station->src_port
                = soc_mem_field32_get(unit, tcam_mem, entry,
                                  ING_PORT_NUMf);
            station->src_port_mask
                = soc_mem_field32_get(unit, tcam_mem, entry,
                                  ING_PORT_NUM_MASKf);
        }

        if (soc_mem_field_valid(unit, tcam_mem, MIM_TERMINATION_ALLOWEDf)) {
            if (soc_mem_field32_get(unit, tcam_mem, entry,
                                    MIM_TERMINATION_ALLOWEDf)) {
                station->flags |= BCM_L2_STATION_MIM;
            }
        }

        if (soc_mem_field_valid(unit, tcam_mem, MPLS_TERMINATION_ALLOWEDf)) {
            if (soc_mem_field32_get(unit, tcam_mem, entry,
                                    MPLS_TERMINATION_ALLOWEDf)) {
                station->flags |= BCM_L2_STATION_MPLS;
            }
        }

        if (soc_mem_field_valid(unit, tcam_mem, IPV4_TERMINATION_ALLOWEDf)) {
            if (soc_mem_field32_get(unit, tcam_mem, entry,
                                    IPV4_TERMINATION_ALLOWEDf)) {
                station->flags |= BCM_L2_STATION_IPV4;
            }
        }

        if (soc_mem_field_valid(unit, tcam_mem, IPV6_TERMINATION_ALLOWEDf)) {
            if (soc_mem_field32_get(unit, tcam_mem, entry,
                                    IPV6_TERMINATION_ALLOWEDf)) {
                station->flags |= BCM_L2_STATION_IPV6;
            }
        }

        if (soc_mem_field_valid(unit, tcam_mem, ARP_RARP_TERMINATION_ALLOWEDf)) {
            if (soc_mem_field32_get(unit, tcam_mem, entry,
                                    ARP_RARP_TERMINATION_ALLOWEDf)) {
                station->flags |= BCM_L2_STATION_ARP_RARP;
            }
        }

        if (soc_mem_field32_get(unit, tcam_mem, entry,
                                COPY_TO_CPUf)) {
            station->flags |= BCM_L2_STATION_COPY_TO_CPU;
        }
    }
#endif

#if defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TRIDENT(unit)
        || SOC_IS_TD2_TT2(unit)) {
        if (soc_mem_field_valid(unit, tcam_mem, TRILL_TERMINATION_ALLOWEDf)) {
            if (soc_mem_field32_get(unit, tcam_mem, entry,
                                    TRILL_TERMINATION_ALLOWEDf)) {
                station->flags |= BCM_L2_STATION_TRILL;
            }
        }

        if (soc_mem_field_valid(unit, tcam_mem, FCOE_TERMINATION_ALLOWEDf)) {
            if (soc_mem_field32_get(unit, tcam_mem, entry,
                                    FCOE_TERMINATION_ALLOWEDf)) {
                station->flags |= BCM_L2_STATION_FCOE;
            }
        }
    }
#endif

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
        soc_feature(unit, soc_feature_hx5_oam_support)) {
        if (soc_mem_field_valid(unit, tcam_mem, OAM_TERMINATION_ALLOWEDf)) {
            if (soc_mem_field32_get(unit, tcam_mem, entry,
                                    OAM_TERMINATION_ALLOWEDf)) {
                station->flags |= BCM_L2_STATION_OAM;
            }
        }
    }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIDENT(unit)
        || SOC_IS_TD2_TT2(unit) || SOC_IS_TRIUMPH3(unit)) {

        if (soc_mem_field_valid(unit, tcam_mem, DISCARDf)) {
            if (soc_mem_field32_get(unit, tcam_mem, entry, DISCARDf)) {
                station->flags |= BCM_L2_STATION_DISCARD;
            }
        }

    }
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_mysta_profile)) {
        uint32 mysta_profile_id = 0;
        uint32 dest_type = 0;
        my_station_profile_1_entry_t entry_prof;
        soc_mem_t prof_mem =  INVALIDm;
        int mem_no = 0;

        if (tcam_mem == MY_STATION_TCAM_2m) {
            prof_mem = MY_STATION_PROFILE_2m;
            mem_no = 1;
        } else {
            prof_mem = MY_STATION_PROFILE_1m;
        }
        mysta_profile_id = soc_mem_field32_dest_get(unit, tcam_mem, entry,
            DESTINATIONf, &dest_type);
        if (dest_type == SOC_MEM_FIF_DEST_MYSTA) {
            if (BCM_E_NONE == _bcm_l2_mysta_profile_entry_get(unit, &entry_prof,
                                    mem_no, mysta_profile_id)){
                _bcm_l2_mysta_entry_to_station(unit, &entry_prof, prof_mem,
                    station);
            }
        }
    }
#endif

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_taf)) {
        if (soc_mem_field_valid(unit, tcam_mem, TAF_GATE_ID_PROFILEf)) {
            uint32 hw_profile_id;

            hw_profile_id = soc_mem_field32_get(unit, tcam_mem,
                                                entry, TAF_GATE_ID_PROFILEf);
            if (0 != hw_profile_id) {
                bcm_tsn_pri_map_t sw_index;

                BCM_IF_ERROR_RETURN(
                    bcmi_esw_tsn_pri_map_map_id_get(
                        unit, bcmTsnPriMapTypeTafGate,
                        hw_profile_id, &sw_index));

                station->taf_gate_primap = sw_index;
            } else {
                station->taf_gate_primap = BCM_TSN_PRI_MAP_INVALID;
            }
        }
    } else
#endif /* BCM_TSN_SUPPORT */
    {
        station->taf_gate_primap = BCM_TSN_PRI_MAP_INVALID;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_l2_station_tcam_replica_find
 * Purpose:
 *     Find a replica entry from my station table. Assuming SC_LOCK has been taken.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station      - (IN) Station information for lookup by hardware.
 *     station_id   - (IN/OUT) Station entry ID / the id of existing station.
 * Returns:
 *      BCM_E_NONE            entry found
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_station_tcam_replica_find(int unit, bcm_l2_station_t *station, int *station_id)
{
    _bcm_l2_station_control_t *sc;               /* Pointer to station        */
                                                 /* control structure.        */
    int                       rv = BCM_E_FAIL;   /* Operation return status.  */
    uint32                    *tcam_buf = NULL;  /* Buffer to store TCAM      */
                                                 /* table entries.            */
    soc_mem_t                 tcam_mem;          /* Memory name.              */
    int                       count;
    _bcm_l2_station_entry_t   **entry_arr;
    int                       index;             /* Table index iterator.     */
    void                      *my_station_hw;   /* Table entry pointer.      */
    uint32                    flags_care = BCM_L2_STATION_IPV4
                                            | BCM_L2_STATION_IPV6
                                            | BCM_L2_STATION_ARP_RARP
                                            | BCM_L2_STATION_MPLS
                                            | BCM_L2_STATION_MIM
                                            | BCM_L2_STATION_TRILL
                                            | BCM_L2_STATION_FCOE
                                            | BCM_L2_STATION_OAM
                                            | BCM_L2_STATION_COPY_TO_CPU
                                            | BCM_L2_STATION_IPV4_MCAST
                                            | BCM_L2_STATION_IPV6_MCAST
                                            | BCM_L2_STATION_UNDERLAY
                                            | BCM_L2_STATION_DISCARD
                                            | BCM_L2_STATION_NSH_OVER_L2;
    int i;
    bcm_l2_station_t station_entry;
#if defined(BCM_RIOT_SUPPORT)
    int vfi_mask_zero = 0;
#endif

    if (station == NULL)
    {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

    rv = _bcm_l2_station_tcam_mem_get(unit, _BCM_L2_STATION_OVERLAY(station) ||
                _BCM_L2_STATION_2(station), &tcam_mem);
    BCM_IF_ERROR_RETURN(rv);

#ifdef BCM_RIOT_SUPPORT
        if (_BCM_L2_STATION_OVERLAY(station) || _BCM_L2_STATION_2(station)) {
            count = sc->entries_total_2;
            entry_arr = (sc->entry_arr_2);
        } else
#endif /* BCM_RIOT_SUPPORT */
        {
            count = sc->entries_total;
            entry_arr = (sc->entry_arr);
        }

    /* Allocate buffer to store the DMAed table entries. */
    tcam_buf = soc_cm_salloc(unit, SOC_MAX_MEM_BYTES * count,
                              "MY STATION TCAM buffer");
    if (NULL == tcam_buf) {
        return (BCM_E_MEMORY);
    }
    /* Initialize the entry buffer. */
    sal_memset(tcam_buf, 0, SOC_MAX_MEM_BYTES * count);

    /* Read the table entries into the buffer. */
    rv = soc_mem_read_range(unit, tcam_mem, MEM_BLOCK_ALL,
                            0, (count - 1), tcam_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

#if defined(BCM_RIOT_SUPPORT)
    _BCM_VPN_SET(vfi_mask_zero, _BCM_VPN_TYPE_VFI, 0);
#endif
    /* Iterate over the table entries. */
    for (index = 0; index < count; index++) {
        if (entry_arr[index] == NULL) {
            continue;
        }

        my_station_hw = soc_mem_table_idx_to_pointer(unit,
                            tcam_mem, void*,
                            tcam_buf, entry_arr[index]->hw_index);

        sal_memset(&station_entry, 0, sizeof(station_entry));
        station_entry.src_port = station->src_port;
        rv = _bcm_tr_l2_station_tcam_entry_parse(unit, tcam_mem, my_station_hw,
                                                 &station_entry);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        /* check keys */
        for (i = 0; i < sizeof(bcm_mac_t); i++) {
            if ((station_entry.dst_mac[i] & station_entry.dst_mac_mask[i]) !=
                (station->dst_mac[i] & station->dst_mac_mask[i])) {
                break;
            }
        }
        if (i < sizeof(bcm_mac_t)) {
            continue;
        }

        if (((station_entry.vlan & station_entry.vlan_mask) !=
            (station->vlan & station->vlan_mask)
#if defined(BCM_RIOT_SUPPORT)
            && !((station_entry.vlan_mask == 0) &&
            (station->vlan_mask == vfi_mask_zero))) ||
            ((station_entry.vlan_mask == 0) &&
            ((station->vlan_mask == 0)
            || (station->vlan_mask == vfi_mask_zero)) &&
            (_BCM_VPN_VFI_IS_SET(station_entry.vlan)
            != _BCM_VPN_VFI_IS_SET(station->vlan))
#endif
            )) {
            continue;
        }
        if ((station_entry.src_port & station_entry.src_port_mask) !=
            (station->src_port & station->src_port_mask)) {
            continue;
        }

        /* check actions */
        if ((station_entry.flags ^ station->flags) & flags_care) {
            continue;
        }

         /* entry exists */
        if (station_id != NULL) {
            *station_id = entry_arr[index]->sid;
        }
        rv = BCM_E_NONE;
        break;
    }

    if (index == count) {
        rv = BCM_E_FAIL;
    }

cleanup:
    if (tcam_buf) {
        soc_cm_sfree(unit, tcam_buf);
    }

    return (rv);
}


/*
 * Function:
 *     _bcm_l2_station_param_check
 * Purpose:
 *     Validate L2 station user input parameters.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station      - (IN) Station information for lookup by hardware.
 *     station_id   - (IN) Station entry ID.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_l2_station_param_check(int unit, bcm_l2_station_t *station, int station_id)
{
    soc_mem_t tcam_mem; /* Hardware table name.     */

    /* Input parameter check. */
    if (NULL == station) {
        return (BCM_E_PARAM);
    }

    if (station->forward_domain_type == bcmL2ForwardDomainVlanClassId) {
        if (!_bcm_l2_p2p_access_vlan_check_enabled(unit)) {
            return (BCM_E_UNAVAIL);
        }
    }

    if (station->flags & BCM_L2_STATION_NSH_OVER_L2) {
        if (!soc_feature(unit, soc_feature_nsh_over_l2)) {
            return (BCM_E_UNAVAIL);
        }
    }

#ifdef BCM_RIOT_SUPPORT
    if (!soc_feature(unit, soc_feature_riot)) {
        if ((_BCM_VPN_VFI_IS_SET(station->vlan))) {
            return (BCM_E_PARAM);
        }
    }
#endif /* BCM_RIOT_SUPPORT */

    if ((SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH(unit)
        || SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit)
        || SOC_IS_VALKYRIE(unit) || SOC_IS_VALKYRIE2(unit)
        || SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit))
        && (_BCM_L2_STATION_TERM_FLAGS_MASK & station->flags)) {
        return (BCM_E_UNAVAIL);
    }

    /* Check allowed L2 station flag options */
    if (soc_feature(unit, soc_feature_gh2_my_station) && 
        (~_BCM_GH2_L2_STATION_FLAGS_MASK & station->flags)) {
        return (BCM_E_UNAVAIL);
    }

    if ((station->flags & BCM_L2_STATION_FCOE)
        && (!SOC_IS_TRIDENT(unit) && !SOC_IS_TD2_TT2(unit))) {
        /* FCoE Termination is support only on Trident family. */
        return (BCM_E_UNAVAIL);
    }

    if ((station->flags & BCM_L2_STATION_COPY_TO_CPU)
        && (!SOC_IS_KATANAX(unit) && !SOC_IS_TRIUMPH3(unit) &&
            !SOC_IS_TRIDENT(unit) && !SOC_IS_TD2_TT2(unit) &&
            !soc_feature(unit, soc_feature_gh2_my_station))) {
        return (BCM_E_UNAVAIL);
    }
#if defined (BCM_OLP_SUPPORT)
    if(soc_feature(unit, soc_feature_olp) &&
       ((station->flags & BCM_L2_STATION_OAM) ||
        (station->flags & BCM_L2_STATION_OLP) ||
        (station->flags & BCM_L2_STATION_XGS_MAC))) {
        if (_BCM_SWITCH_OLP_APIV(unit) == _BCM_SWITCH_OLP_TRUE_API) {
            if (station->flags & BCM_L2_STATION_OAM) {
                LOG_VERBOSE(BSL_LS_BCM_L2,
                        (BSL_META_U(unit,
                                    "use bcmPortControlPortMacOui \n")));
            } else {
                LOG_VERBOSE(BSL_LS_BCM_L2,
                        (BSL_META_U(unit,
                                    "use bcm_switch_olp_l2_addr_xxx API\n")));
            }
            return BCM_E_UNAVAIL;
        }
        _BCM_SWITCH_OLP_APIV_SET(unit, _BCM_SWITCH_OLP_L2_STATION_API);
    }
#endif

    if ((station->flags & BCM_L2_STATION_OAM)
        && (!SOC_IS_KATANAX(unit) && !SOC_IS_TRIUMPH3(unit) &&
        !soc_feature(unit, soc_feature_fp_based_oam) &&
        !soc_feature(unit, soc_feature_gh2_my_station))
#if defined(BCM_HELIX5_SUPPORT)
        && (!soc_feature(unit, soc_feature_hx5_oam_support))
#endif
       ) {

        return (BCM_E_UNAVAIL);
    }
    /* OLP/XGS STATION should not have overlay flag set */
    if (soc_feature(unit, soc_feature_fp_based_oam) &&
        ((station->flags & BCM_L2_STATION_OLP) ||
            (station->flags & BCM_L2_STATION_XGS_MAC)) &&
            _BCM_L2_STATION_OVERLAY(station)) {
        return (BCM_E_PARAM);
    }                   

#ifdef BCM_RIOT_SUPPORT
    if (station->flags & BCM_L2_STATION_WITH_ID) {
        if (_BCM_L2_STATION_OVERLAY(station) || _BCM_L2_STATION_2(station)) {
            if ((station_id < _BCM_L2_STATION_ID_2_BASE) ||
                (station_id > _BCM_L2_STATION_ID_2_MAX)) {
                return (BCM_E_PARAM);
            }
        } else {
            if ((station_id < _BCM_L2_STATION_ID_BASE) ||
                (station_id > _BCM_L2_STATION_ID_MAX)) {
                return (BCM_E_PARAM);
            }
        }
    }
#endif /* BCM_RIOT_SUPPORT */

    if (((0 != station->src_port) || (0 != station->src_port_mask))
        && (!SOC_IS_KATANAX(unit) && !SOC_IS_TRIDENT(unit)
            && !SOC_IS_TD2_TT2(unit) && !SOC_IS_TRIUMPH3(unit)
            && !soc_feature(unit, soc_feature_gh2_my_station))) {
        /* Ingress Port match lookup not supported. */
        return (BCM_E_UNAVAIL);
    }

    /* In case of KT2  OAM, OLP and XGS MAC are not part of TCAM */
    if ((SOC_IS_KATANA2(unit) || 
         soc_feature(unit, soc_feature_fp_based_oam)) && 
       ((station->flags & BCM_L2_STATION_OAM) || 
          (station->flags & BCM_L2_STATION_OLP) || 
          (station->flags & BCM_L2_STATION_XGS_MAC))) { 
          return (BCM_E_NONE);
    } else {
#ifdef BCM_RIOT_SUPPORT
        if (_BCM_L2_STATION_OVERLAY(station)) {
            int vfi;
            int vfi_mask = 0;

            /* vlan/vlan_mask is VFI or source_port/source_port_mask is vp
             * are not possible because they can 'masked out' */
            BCM_IF_ERROR_RETURN(_bcm_l2_station_tcam_mem_get(unit, 1, &tcam_mem));

            if (soc_property_get(unit,
                                 spn_MY_STATION_FORWARD_DOMAIN_TYPE_DECOUPLED,
                                 0)) {

                if (_BCM_VPN_VFI_IS_SET(station->vlan)
                    != _BCM_VPN_VFI_IS_SET(station->vlan_mask)) {
                    /* VLAN/VFI and its mask MUST be of the same type. */
                    return (BCM_E_PARAM);
                }

                if (_BCM_VPN_VFI_IS_SET(station->vlan)) {
                    _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, station->vlan);
                    ParamCheck(unit, tcam_mem, VFI_IDf, vfi);
                    _BCM_VPN_GET(vfi_mask, _BCM_VPN_TYPE_VFI, station->vlan_mask);
                    ParamCheck(unit, tcam_mem, VFI_ID_MASKf, vfi_mask);
                } else {
                    ParamCheck(unit, tcam_mem, VLAN_IDf, station->vlan);
                    ParamCheck(unit, tcam_mem, VLAN_ID_MASKf, station->vlan_mask);
                }

                /* Validate L2 forwarding domain type. */
                if ((station->forward_domain_type_mask == 0) &&
                    (station->forward_domain_type != 0)) {
                    return (BCM_E_PARAM);
                }
                if (_BCM_VPN_VFI_IS_SET(station->vlan_mask)) {
                    if (station->forward_domain_type_mask == 0) {
                        /* Not allowed. */
                        return (BCM_E_PARAM);
                    }
                    if (station->forward_domain_type != bcmL2ForwardDomainVpn) {
                        return (BCM_E_PARAM);
                    }
                } else {
                    if ((station->vlan_mask != 0) &&
                        (station->forward_domain_type_mask == 0)) {
                        /*
                         * Forwarding domain type can be ignored only when VLAN
                         * is ignored.
                         */
                        return (BCM_E_PARAM);
                    }
                    if ((station->forward_domain_type_mask != 0) &&
                        (station->forward_domain_type != bcmL2ForwardDomainVlan)) {
                        return (BCM_E_PARAM);
                    }
                }
            } else {
                /*
                 * In this legacy mode, forwarding domain type are determined by
                 * SDK internally, thus make sure L2 forwarding domain fields
                 * are NOT configured by user.
                 */
                if ((station->forward_domain_type_mask != 0) ||
                    (station->forward_domain_type != 0)) {

                    /* make exception for this type */
                    if (station->forward_domain_type != bcmL2ForwardDomainVlanClassId) {
                        return (BCM_E_PARAM);
                    }
                }

                if ((_BCM_VPN_VFI_IS_SET(station->vlan)
                    != _BCM_VPN_VFI_IS_SET(station->vlan_mask)) && (station->vlan_mask)) {
                    return (BCM_E_PARAM);
                }

                if ((_BCM_VPN_VFI_IS_SET(station->vlan)) && (station->vlan)) {
                    _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, station->vlan);
                    ParamCheck(unit, tcam_mem, VFI_IDf, vfi);
                } else {
                    ParamCheck(unit, tcam_mem, VLAN_IDf, station->vlan);
                }

                if ((_BCM_VPN_VFI_IS_SET(station->vlan_mask)) && (station->vlan_mask)) {
                    _BCM_VPN_GET(vfi_mask, _BCM_VPN_TYPE_VFI, station->vlan_mask);
                    ParamCheck(unit, tcam_mem, VFI_ID_MASKf, vfi_mask);
                } else {
                    ParamCheck(unit, tcam_mem, VLAN_ID_MASKf, station->vlan_mask);
                }
            }
        } else
#endif /* BCM_RIOT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(_bcm_l2_station_tcam_mem_get(unit, 0, &tcam_mem));
            ParamCheck(unit, tcam_mem, VLAN_IDf, station->vlan);
            ParamCheck(unit, tcam_mem, VLAN_ID_MASKf, station->vlan_mask);

            if (soc_property_get(unit,
                                 spn_MY_STATION_FORWARD_DOMAIN_TYPE_DECOUPLED,
                                 0)) {

                /* Validate L2 forwarding domain type. */
                if ((station->forward_domain_type_mask == 0) &&
                    (station->forward_domain_type != 0)) {
                    return (BCM_E_PARAM);
                }
                if ((station->vlan_mask != 0) &&
                    (station->forward_domain_type_mask == 0)) {
                    /*
                     * Forwarding domain type can be ignored only when VLAN is
                     * ignored.
                     */
                    return (BCM_E_PARAM);
                }
                if ((station->forward_domain_type_mask != 0) &&
                    (station->forward_domain_type != bcmL2ForwardDomainVlan)) {
                    return (BCM_E_PARAM);
                }
            }
        }

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
       if (SOC_IS_KATANAX(unit)
            || SOC_IS_TRIDENT(unit)
            || SOC_IS_TD2_TT2(unit)
            || SOC_IS_TRIUMPH3(unit)
            || soc_feature(unit, soc_feature_gh2_my_station)) {
           bcm_gport_t gport_id;
           bcm_port_t port_out;
           bcm_module_t mod_out;
           bcm_trunk_t trunk_id;
           uint32 mod_port_data = 0; /* concatenated modid and port */
           int num_bits_for_port;

           if (GPORT_TYPE(station->src_port) != 
                                  GPORT_TYPE(station->src_port_mask)) {
               return BCM_E_PARAM;
           }
           num_bits_for_port = _shr_popcount(
                                  (unsigned int)SOC_PORT_ADDR_MAX(unit));
           if (BCM_GPORT_IS_SET(station->src_port)) {
               BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, 
                          station->src_port,
                          &mod_out,
                          &port_out, &trunk_id,
                          &gport_id));
               if (BCM_GPORT_IS_TRUNK(station->src_port)) {
                   if (BCM_TRUNK_INVALID == trunk_id) {
                       return BCM_E_PORT;
                   }
                   if (SOC_IS_KATANAX(unit) || 
                       soc_feature(unit, soc_feature_gh2_my_station)) {
                       mod_port_data = trunk_id;
                   } else {
                       mod_port_data = ((1 << SOC_TRUNK_BIT_POS(unit)) | trunk_id);
                   }
               }
#ifdef BCM_RIOT_SUPPORT
               else if((gport_id != BCM_GPORT_INVALID) &&
                       (_BCM_L2_STATION_OVERLAY(station))) {
                   bcm_port_t port_mask = -1;
                   bcm_module_t mod_mask = -1;
                   bcm_trunk_t trunk_mask = -1;
                   int gport_mask = -1;
                   BCM_IF_ERROR_RETURN(
                       _l2_port_mask_gport_resolve(unit, station->src_port_mask,
                                                   &mod_mask, &port_mask,
                                                   &trunk_mask, &gport_mask));
                   ParamCheck(unit, tcam_mem, SOURCE_FIELD_MASKf, gport_mask);
                   mod_port_data = gport_id;
               }
#endif /* BCM_RIOT_SUPPORT */
               else {
                   if ((-1 == mod_out) || (-1 == port_out)) {
                       return BCM_E_PORT;
                   }
                   mod_port_data = (mod_out << num_bits_for_port) | port_out;
               }
           } else {
               int my_modid;
               BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit,&my_modid));
               port_out = station->src_port;
               mod_port_data = station->src_port | 
                               (my_modid << num_bits_for_port);
           } 
           if (SOC_MEM_FIELD_VALID(unit, tcam_mem, SOURCE_FIELDf)) {
               ParamCheck(unit, tcam_mem, SOURCE_FIELDf, mod_port_data);
           } else {
               ParamCheck(unit, tcam_mem, ING_PORT_NUMf, port_out);
           }
        }
#endif
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_tr_l2_station_add
 * Purpose:
 *     Create an entry with specified ID or generated ID and install
 *     the entry in hardware.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station_id   - (IN/OUT) Station entry ID.
 *     station      - (IN) Station information for lookup by hardware.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tr_l2_station_add(int unit,
                      int *station_id,
                      bcm_l2_station_t *station)
{
    _bcm_l2_station_control_t *sc;         /* Control structure.       */
    _bcm_l2_station_entry_t   *ent = NULL; /* Entry pointer.           */
    int                       rv;          /* Operation return status. */
    int                       sid = -1;    /* Entry ID.                */
    /* Input parameter check. */
    if (NULL == station || NULL == station_id)  {
        return (BCM_E_PARAM);
    }

    /* ID needs to be passed for replace operation. */
    if ((station->flags & BCM_L2_STATION_REPLACE)
        && !(station->flags & BCM_L2_STATION_WITH_ID)) {
        return (BCM_E_PARAM);
    }
#ifdef BCM_RIOT_SUPPORT
    if(soc_feature(unit, soc_feature_riot) &&
            soc_feature(unit, soc_feature_fp_based_oam) &&
            ((station->flags & BCM_L2_STATION_OLP) ||
             (station->flags & BCM_L2_STATION_XGS_MAC))) {
        /* OLP/XGS STATION should go to tcam1 software database */
        station->flags |= BCM_L2_STATION_UNDERLAY;
     }               
#endif
    rv = _bcm_l2_station_param_check(unit, station, *station_id);
    if (BCM_FAILURE(rv)) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

    SC_LOCK(sc);

    if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit) || SOC_IS_TRIDENT3X(unit)) {
        rv = _bcm_l2_station_tcam_replica_find(unit, station, &sid);
        if (BCM_SUCCESS(rv)) {
            if (!((station->flags & BCM_L2_STATION_WITH_ID) &&
                  (station->flags & BCM_L2_STATION_REPLACE) &&
                  (sid == *station_id))) {
                SC_UNLOCK(sc);
                rv = BCM_E_EXISTS;
                return (rv);
            }
        }
    }

    if (station->flags & BCM_L2_STATION_WITH_ID) {
        if ((sc->last_allocated_sid < *station_id) &&
            (*station_id <= _BCM_L2_STATION_ID_MAX)) {
            sc->last_allocated_sid = *station_id;
        }
        sid = *station_id;
    } else {

#ifdef BCM_RIOT_SUPPORT
        if (_BCM_L2_STATION_OVERLAY(station) || _BCM_L2_STATION_2(station)) {
            sid = ++sc->last_allocated_sid_2;

            if (_BCM_L2_STATION_ID_2_MAX < sid) {
                sc->last_allocated_sid_2 = _BCM_L2_STATION_ID_2_MAX;

                for (sid = _BCM_L2_STATION_ID_2_BASE;
                     sid <= _BCM_L2_STATION_ID_2_MAX; sid++) {
                    if (BCM_E_NOT_FOUND == 
                            _bcm_l2_station_entry_get(unit, sid, &ent)) {
                        break;
                    }
                }
            }
        } else
#endif /* BCM_RIOT_SUPPORT */
        {
            sid = ++sc->last_allocated_sid;

            if (_BCM_L2_STATION_ID_MAX < sid) {
                sc->last_allocated_sid = _BCM_L2_STATION_ID_MAX;

                for (sid = _BCM_L2_STATION_ID_BASE;
                     sid <= _BCM_L2_STATION_ID_MAX; sid++) {
                    if (BCM_E_NOT_FOUND == 
                            _bcm_l2_station_entry_get(unit, sid, &ent)) {
                        break;
                    }
                }
            }
        }

        *station_id = sid;
    }

    rv = _bcm_l2_station_entry_get(unit, sid, &ent);

    /* For replace operation, entry must exist. */
    if (BCM_FAILURE(rv)
        && (station->flags & BCM_L2_STATION_REPLACE)) {
        LOG_ERROR(BSL_LS_BCM_L2,
                  (BSL_META_U(unit,
                              "L2(unit %d) Error: Replace (SID=%d) - Invalid (%s).\n"),
                   unit, sid, bcm_errmsg(rv)));
        SC_UNLOCK(sc);
        return (rv);
    }

    /* For non-replace operations, entry must not exist. */
    if (BCM_SUCCESS(rv)
        && (0 == (station->flags & BCM_L2_STATION_REPLACE))) {
        LOG_INFO(BSL_LS_BCM_L2,
                  (BSL_META_U(unit,
                              "L2(unit %d) Error: (SID=%d) add - failed (%s).\n"),
                   unit, sid, bcm_errmsg(rv)));
        SC_UNLOCK(sc);
        return (BCM_E_EXISTS);
    }

    if (0 == (station->flags & BCM_L2_STATION_REPLACE)) {
        rv = _bcm_l2_station_entry_create(unit, sid, station, &ent);
        if (BCM_FAILURE(rv)) {
            LOG_INFO(BSL_LS_BCM_L2,
                      (BSL_META_U(unit,
                                  "L2(unit %d) Error: Station (SID=%d) add - failed (%s).\n"),
                       unit, sid, bcm_errmsg(rv)));
            SC_UNLOCK(sc);
            return (rv);
        }
    } else {
        rv = _bcm_l2_station_entry_update(unit, sid, station, ent);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_L2,
                      (BSL_META_U(unit,
                                  "L2(unit %d) Error: Station (SID=%d) update - failed (%s).\n"),
                       unit, sid, bcm_errmsg(rv)));
            SC_UNLOCK(sc);
            return (rv);
        }
    }

    rv = _bcm_l2_station_entry_prio_install(unit, ent, station);
    if (BCM_FAILURE(rv)) {
        if (NULL != ent->tcam_ent) {
            sal_free(ent->tcam_ent);
        }
        LOG_ERROR(BSL_LS_BCM_L2,
                  (BSL_META_U(unit,
                              "L2(unit %d) Error: (SID=%d) install - failed (%s).\n"),
                   unit, sid, bcm_errmsg(rv)));
        SC_UNLOCK(sc);
        return (rv);
    }

    /* Mark the entry as installed. */
    ent->flags |= _BCM_L2_STATION_ENTRY_INSTALLED;

    SC_UNLOCK(sc);
    return (rv);
}

/*
 * Function:
 *     bcm_tr_l2_station_delete
 * Purpose:
 *     Deallocate the memory used to track an entry and remove the entry
 *     from hardware.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station_id   - (IN) Station entry ID.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tr_l2_station_delete(int unit, int station_id)
{
    _bcm_l2_station_entry_t     *s_ent = NULL; /* Entry pointer.           */
    _bcm_l2_station_entry_t     **entry_arr = NULL; /* Entry pointer.      */
    _bcm_l2_station_control_t   *sc = NULL;    /* Control structure.       */
    int                         rv;            /* Operation return status. */
    int                         overlay;       /* MY_STATION_2 entry.      */
    soc_mem_t                   tcam_mem;      /* TCMA memory name.        */

#if defined(BCM_TRIUMPH_SUPPORT)
    my_station_tcam_entry_t     entry;
#endif

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

    SC_LOCK(sc);

    /* Get the entry by ID. */
    rv = _bcm_l2_station_entry_get(unit, station_id, &s_ent);
    if (BCM_FAILURE(rv)) {
        SC_UNLOCK(sc);
        return (rv);
    }
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    if ((SOC_IS_KATANA2(unit) || 
                soc_feature(unit, soc_feature_fp_based_oam)) &&
            (s_ent->flags & _BCM_L2_STATION_ENTRY_TYPE_NON_TCAM)) {
        rv = _bcm_l2_station_non_tcam_entry_delete(unit, sc, s_ent, station_id);
        /* Free the entry. */
        sal_free(s_ent);
        SC_UNLOCK(sc);
        return rv;
    }  
#endif

#ifdef BCM_RIOT_SUPPORT
    if (_BCM_L2_STATION_ID_OVERLAY(s_ent->sid) ||
         _BCM_L2_STATION_ID_2(s_ent->sid)) {
        overlay = _BCM_L2_STATION_ENTRY_OVERLAY;
        entry_arr = (sc->entry_arr_2);
    } else
#endif /* BCM_RIOT_SUPPORT */
    {
        overlay = _BCM_L2_STATION_ENTRY_UNDERLAY;
        entry_arr = (sc->entry_arr);
    }

    /* Get the TCAM memory name. */
    rv = _bcm_l2_station_tcam_mem_get(unit, overlay, &tcam_mem);
    if (BCM_FAILURE(rv)) {
        SC_UNLOCK(sc);
        return (rv);
    }

#if defined(BCM_TRIUMPH_SUPPORT)
    if (soc_feature(unit, soc_feature_mysta_profile)) {
        uint32 mysta_profile_id = 0;
        uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;

        rv = soc_mem_read(unit, tcam_mem, MEM_BLOCK_ANY, s_ent->hw_index,
                          &entry);
        if (BCM_FAILURE(rv)) {
            SC_UNLOCK(sc);
            return (rv);
        }
        if (soc_mem_field32_get(unit, tcam_mem, &entry, VALIDf) != 0) {
            mysta_profile_id = soc_mem_field32_dest_get(unit, tcam_mem, &entry,
                                    DESTINATIONf, &dest_type);
            if (dest_type != SOC_MEM_FIF_DEST_MYSTA) {
                SC_UNLOCK(sc);
                return BCM_E_FAIL;
            }
            rv = _bcm_l2_mysta_profile_entry_delete(unit,
                                 (tcam_mem == MY_STATION_TCAM_2m)?1:0,
                                mysta_profile_id);
            if (BCM_FAILURE(rv)) {
                SC_UNLOCK(sc);
                return (rv);
            }
#ifdef BCM_TRIDENT3_SUPPORT
            if (!(soc_feature(unit, soc_feature_riot) ||
                  _bcm_l2_p2p_access_vlan_check_enabled(unit))) {
                if (tcam_mem == MY_STATION_TCAMm) {
                    uint32 mem_no = 1;
                    rv = _bcm_l2_mysta_profile_entry_delete(unit, mem_no,
                                    mysta_profile_id);
                    if (BCM_FAILURE(rv)) {
                        SC_UNLOCK(sc);
                        return (rv);
                    }
                }
            }
#endif
        }
    }
#endif

    /* Remove the entry from hardware by clearing the index. */
    rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, s_ent->hw_index,
                       soc_mem_entry_null(unit, tcam_mem));
    if (BCM_FAILURE(rv)) {
        SC_UNLOCK(sc);
        return (rv);
    }
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit) && (!(soc_feature(unit, soc_feature_riot) ||
                         _bcm_l2_p2p_access_vlan_check_enabled(unit)))) {
        if (tcam_mem == MY_STATION_TCAMm) {
            tcam_mem = MY_STATION_TCAM_2m;
            rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, s_ent->hw_index,
                           soc_mem_entry_null(unit, tcam_mem));
            if (BCM_FAILURE(rv)) {
                SC_UNLOCK(sc);
                return (rv);
            }
        }
    }
#endif
    /* Free tcam buffer. */
    if (NULL != s_ent->tcam_ent) {
        sal_free(s_ent->tcam_ent);
    }

    /* Remove the entry pointer from the list. */
    entry_arr[s_ent->hw_index] = NULL;

    /* Increment free entries count. */
#ifdef BCM_RIOT_SUPPORT
    if (_BCM_L2_STATION_ID_OVERLAY(s_ent->sid) ||
        _BCM_L2_STATION_ID_2(s_ent->sid)) {
        sc->entries_free_2++;
        sc->entry_count_2--;
    } else 
#endif /* BCM_RIOT_SUPPORT */
    {
        sc->entries_free++;
        sc->entry_count--;
    }

    /* Free the entry. */
    sal_free(s_ent);
    
    /* Rset the last_allocated_sid */
    if (station_id == sc->last_allocated_sid) {
        --sc->last_allocated_sid;
    }

    SC_UNLOCK(sc);

    return (rv);
}

/*
 * Function:
 *     _bcm_tr_l2_station_entry_delete_all
 * Purpose:
 *     Deallocate all memory used to track entries and remove entries from
 *     hardware.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     overlay  - (IN) Overlay/Underlay identification
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_tr_l2_station_entry_delete_all(int unit, int overlay)
{
    _bcm_l2_station_control_t   *sc = NULL; /* Control strucutre.       */
    _bcm_l2_station_entry_t    **entry_arr = NULL; /* Entry array.      */
    int                         rv;         /* Operation return status. */
    int                         index;      /* Entry index.             */
    int                         max_entries;
    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

    SC_LOCK(sc);


    max_entries = (overlay == _BCM_L2_STATION_ENTRY_OVERLAY) ?
                      sc->entries_total_2 : sc->entries_total;
    entry_arr = (overlay == _BCM_L2_STATION_ENTRY_OVERLAY) ?
                    (sc->entry_arr_2) : (sc->entry_arr);

    if (NULL == entry_arr) {
        SC_UNLOCK(sc);
        return (BCM_E_NONE);
    }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam) &&
        (overlay == _BCM_L2_STATION_ENTRY_UNDERLAY)) {
        max_entries = sc->entries_total + sc->olp_entries_total + 1;
    }
#endif

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        max_entries = sc->entries_total + sc->port_entries_total + 
                                          sc->olp_entries_total + 1;
    }
#endif 

    for (index = 0; index < max_entries; index++) {

        if (NULL == entry_arr[index]) {
            continue;
        }

        rv = bcm_tr_l2_station_delete(unit, entry_arr[index]->sid);
        if (BCM_FAILURE(rv)) {
            SC_UNLOCK(sc);
            return (rv);
        }
    }

    SC_UNLOCK(sc);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_tr_l2_station_delete_all
 * Purpose:
 *     Deallocate all memory used to track entries and remove entries from
 *     hardware.
 * Parameters:
 *     unit     - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tr_l2_station_delete_all(int unit)
{
    int rv;

    rv = _bcm_tr_l2_station_entry_delete_all(unit,
             _BCM_L2_STATION_ENTRY_UNDERLAY);
    BCM_IF_ERROR_RETURN(rv);

#ifdef BCM_RIOT_SUPPORT
    if (soc_feature(unit, soc_feature_riot) ||
        _bcm_l2_p2p_access_vlan_check_enabled(unit)) {
        rv = _bcm_tr_l2_station_entry_delete_all(unit,
                 _BCM_L2_STATION_ENTRY_OVERLAY);

        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_RIOT_SUPPORT */

   return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_tr_l2_station_get
 * Purpose:
 *     Get station information for a given station ID.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station_id   - (IN) Station entry ID.
 *     station      - (OUT) Station entry lookup information.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tr_l2_station_get(int unit, int station_id, bcm_l2_station_t *station)
{
    _bcm_l2_station_entry_t     *s_ent = NULL; /* Entry pointer.           */
    _bcm_l2_station_control_t   *sc = NULL;    /* Control structure.       */
    int                         rv;            /* Operation return status. */
    soc_mem_t                   tcam_mem;      /* TCAM memory name.        */
    uint32                      entry[SOC_MAX_MEM_FIELD_WORDS]; /* Entry   */
                                                                /* buffer. */
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    soc_mem_t prof_mem = MY_STATION_PROFILE_1m;
#endif
#ifdef BCM_RIOT_SUPPORT
    int                         overlay = 0; /* Overlay/Underlay indicator */
#endif /* BCM_RIOT_SUPPORT */

    /* Input parameter check. */
    if (NULL == station) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));
    SC_LOCK(sc);

    /* Get the entry by ID. */
    rv = _bcm_l2_station_entry_get(unit, station_id, &s_ent);
    if (BCM_FAILURE(rv)) {
        SC_UNLOCK(sc);
        return (rv);
    }
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    if ((SOC_IS_KATANA2(unit) || 
                soc_feature(unit, soc_feature_fp_based_oam)) &&
            (s_ent->flags & _BCM_L2_STATION_ENTRY_TYPE_NON_TCAM)) {
        rv = _bcm_l2_station_non_tcam_entry_get(unit, sc, s_ent, 
                station_id, station);
        SC_UNLOCK(sc);
        return rv;
    }  
#endif
    rv = _bcm_l2_station_tcam_mem_get(unit,
             _BCM_L2_STATION_ID_OVERLAY(station_id) ||
              _BCM_L2_STATION_ID_2(station_id), &tcam_mem);
    if (BCM_FAILURE(rv)) {
        SC_UNLOCK(sc);
        return (rv);
    }

    if (s_ent->flags & _BCM_L2_STATION_ENTRY_TYPE_TCAM_2) {
        station->forward_domain_type = bcmL2ForwardDomainVlanClassId;
    }

    /* Clear entry buffer. */
    sal_memset(entry, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    rv = soc_mem_read(unit, tcam_mem, MEM_BLOCK_ANY, s_ent->hw_index, entry);
    if (BCM_FAILURE(rv)) {
        SC_UNLOCK(sc);
        return (rv);
    }

    station->priority = s_ent->prio;

    soc_mem_mac_addr_get(unit, tcam_mem, entry,
                         MAC_ADDRf, station->dst_mac);

    soc_mem_mac_addr_get(unit, tcam_mem, entry,
                         MAC_ADDR_MASKf, station->dst_mac_mask);

#ifdef BCM_RIOT_SUPPORT
    if (_BCM_L2_STATION_ID_OVERLAY(station_id)) {
        overlay = soc_mem_field32_get(unit, tcam_mem, entry,
                                      FORWARDING_FIELD_TYPEf);
    }

    if (overlay) {
        int vfi = 0;
        int vfi_mask = 0;

        vfi = soc_mem_field32_get(unit, tcam_mem, entry, VFI_IDf);
        _BCM_VPN_SET(station->vlan, _BCM_VPN_TYPE_VFI, vfi);

        vfi_mask = soc_mem_field32_get(unit, tcam_mem, entry, VFI_ID_MASKf);
        _BCM_VPN_SET(station->vlan_mask, _BCM_VPN_TYPE_VFI, vfi_mask);
    } else
#endif /* BCM_RIOT_SUPPORT */
    {
#ifdef BCM_RIOT_SUPPORT
        if (soc_feature(unit, soc_feature_riot)) {
            station->flags |= BCM_L2_STATION_UNDERLAY;
        }
#endif
        station->vlan
            = soc_mem_field32_get(unit, tcam_mem, entry, VLAN_IDf);

        station->vlan_mask
            = soc_mem_field32_get(unit, tcam_mem, entry, VLAN_ID_MASKf);
    }

    if (soc_feature(unit, soc_feature_my_station_2)) {
        if (soc_property_get(unit,
                             spn_MY_STATION_FORWARD_DOMAIN_TYPE_DECOUPLED,
                             0)) {
            station->forward_domain_type
                = soc_mem_field32_get(unit, tcam_mem, entry,
                                      FORWARDING_FIELD_TYPEf);

            station->forward_domain_type_mask
                = soc_mem_field32_get(unit, tcam_mem, entry,
                                      FORWARDING_FIELD_TYPE_MASKf);
        }
    }

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_KATANAX(unit)
        || SOC_IS_TRIDENT(unit)
        || SOC_IS_TD2_TT2(unit)
        || SOC_IS_TRIUMPH3(unit)
        || soc_feature(unit, soc_feature_gh2_my_station)) {
        int src_field;
        int src_mask_field;
        int src_type = 0;
        _bcm_gport_dest_t src;
        _bcm_gport_dest_t src_mask;
        int num_bits_for_port;

        _bcm_gport_dest_t_init(&src);
        _bcm_gport_dest_t_init(&src_mask);

        if (tcam_mem == MY_STATION_TCAM_2m) {
            prof_mem = MY_STATION_PROFILE_2m;
        }

        if (SOC_MEM_FIELD_VALID(unit, tcam_mem, SOURCE_FIELDf)) {
            src_field= soc_mem_field32_get(unit, tcam_mem, entry,
                            SOURCE_FIELDf);
            src_mask_field = soc_mem_field32_get(unit, tcam_mem, entry,
                                  SOURCE_FIELD_MASKf);

            if (SOC_MEM_FIELD_VALID(unit, tcam_mem, SOURCE_TYPEf)) {
                src_type = soc_mem_field32_get(unit, tcam_mem, entry,
                                               SOURCE_TYPEf);
            }

#ifdef BCM_RIOT_SUPPORT
            if (soc_feature(unit, soc_feature_riot) && (src_type & 0x1)) {
                _bcm_vp_gport_dest_fill(unit, src_field, &src);
                src_mask.gport_type = src.gport_type;
                switch (src_mask.gport_type) {
                    case _SHR_GPORT_TYPE_MPLS_PORT:
                        src_mask.mpls_id = src_mask_field;
                        break;
                    case _SHR_GPORT_TYPE_MIM_PORT:
                        src_mask.mim_id = src_mask_field;
                        break;
                    case _SHR_GPORT_TYPE_TRILL_PORT:
                        src_mask.trill_id = src_mask_field;
                        break;
                    case _SHR_GPORT_TYPE_L2GRE_PORT:
                        src_mask.l2gre_id = src_mask_field;
                        break;
                    case _SHR_GPORT_TYPE_VXLAN_PORT:
                        src_mask.vxlan_id = src_mask_field;
                        break;
                    default:
                        return BCM_E_PARAM;
                }
            } else
#endif /* BCM_RIOT_SUPPORT */
            if ((SOC_IS_KATANAX(unit) && (src_type & 0x1)) || 
                    (src_field >> SOC_TRUNK_BIT_POS(unit)) & 0x1) {
                src.tgid = src_field & ((1 << SOC_TRUNK_BIT_POS(unit)) - 1);
                src.gport_type = _SHR_GPORT_TYPE_TRUNK;
                src_mask.tgid = src_mask_field & 
                    ((1 << SOC_TRUNK_BIT_POS(unit)) - 1);
                src_mask.gport_type = _SHR_GPORT_TYPE_TRUNK;
            } else if (soc_feature(unit, soc_feature_gh2_my_station) &&
                       (src_type & 0x1)) {
                src.tgid = src_field;
                src.gport_type = _SHR_GPORT_TYPE_TRUNK;
                src_mask.tgid = src_mask_field;
                src_mask.gport_type = _SHR_GPORT_TYPE_TRUNK;
            } else {
                src.port = src_field & SOC_PORT_ADDR_MAX(unit);
                src.modid = (src_field - src.port) /
                               (SOC_PORT_ADDR_MAX(unit) + 1);
                src.gport_type = _SHR_GPORT_TYPE_MODPORT;
                num_bits_for_port = _shr_popcount(
                                 (unsigned int)SOC_PORT_ADDR_MAX(unit));
                src_mask.port = src_mask_field & SOC_PORT_ADDR_MAX(unit);
                src_mask.gport_type = _SHR_GPORT_TYPE_MODPORT;
                src_mask.modid = (src_mask_field >> num_bits_for_port) & 
                                 SOC_MODID_MAX(unit);
            }
            BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &src, 
                                &(station->src_port)));
            BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &src_mask, 
                                &(station->src_port_mask)));
        } else {
            station->src_port
                = soc_mem_field32_get(unit, tcam_mem, entry,
                                  ING_PORT_NUMf);
            station->src_port_mask
                = soc_mem_field32_get(unit, tcam_mem, entry,
                                  ING_PORT_NUM_MASKf);
        }
        
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            uint32 mysta_profile_id = 0;
            uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
            my_station_profile_1_entry_t entry_prof;
            mysta_profile_id = soc_mem_field32_dest_get(unit, tcam_mem, entry,
                                    DESTINATIONf, &dest_type);
            if (dest_type != SOC_MEM_FIF_DEST_MYSTA) {
                SC_UNLOCK(sc);
                return BCM_E_FAIL;
            }

            rv = _bcm_l2_mysta_profile_entry_get(unit, &entry_prof,
                                                 (prof_mem == MY_STATION_PROFILE_2m)?1:0,
                                                 mysta_profile_id);
            if (BCM_FAILURE(rv)) {
                SC_UNLOCK(sc);
                return rv;
            }
            _bcm_l2_mysta_entry_to_station(unit, &entry_prof, prof_mem, station);
        } else {
        if (soc_mem_field_valid(unit, tcam_mem, MIM_TERMINATION_ALLOWEDf)) { 
        if (soc_mem_field32_get(unit, tcam_mem, entry,
                                MIM_TERMINATION_ALLOWEDf)) {
            station->flags |= BCM_L2_STATION_MIM;
        }
        }

        if (soc_mem_field_valid(unit, tcam_mem, MPLS_TERMINATION_ALLOWEDf)) { 
        if (soc_mem_field32_get(unit, tcam_mem, entry,
                                MPLS_TERMINATION_ALLOWEDf)) {
            station->flags |= BCM_L2_STATION_MPLS;
        }
        }   
        
        if (soc_mem_field32_get(unit, tcam_mem, entry,
                                IPV4_TERMINATION_ALLOWEDf)) {
            station->flags |= BCM_L2_STATION_IPV4;
        }

        if (soc_mem_field32_get(unit, tcam_mem, entry,
                                IPV6_TERMINATION_ALLOWEDf)) {
            station->flags |= BCM_L2_STATION_IPV6;
        }
        
        if (soc_mem_field32_get(unit, tcam_mem, entry,
                                ARP_RARP_TERMINATION_ALLOWEDf)) {
            station->flags |= BCM_L2_STATION_ARP_RARP;
        }
        }

        if (soc_mem_field32_get(unit, tcam_mem, entry,
                                COPY_TO_CPUf)) {
            station->flags |= BCM_L2_STATION_COPY_TO_CPU;
        }
    }
#endif

#if defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TRIDENT(unit)
        || SOC_IS_TD2_TT2(unit)) {

        if (soc_feature(unit, soc_feature_mysta_profile)) {
            uint32 mysta_profile_id = 0;
            uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
            my_station_profile_1_entry_t entry_prof;
            mysta_profile_id = soc_mem_field32_dest_get(unit, tcam_mem, entry,
                                    DESTINATIONf, &dest_type);
            if (dest_type != SOC_MEM_FIF_DEST_MYSTA) {
                SC_UNLOCK(sc);
                return BCM_E_FAIL;
            }

            rv = _bcm_l2_mysta_profile_entry_get(unit, &entry_prof,
                                                 (prof_mem == MY_STATION_PROFILE_2m)?1:0,
                                                 mysta_profile_id);
            if (BCM_FAILURE(rv)) {
                SC_UNLOCK(sc);
                return rv;
            }
            _bcm_l2_mysta_entry_to_station(unit, &entry_prof, prof_mem, station);
        } else {
        if (soc_mem_field32_get(unit, tcam_mem, entry,
                                TRILL_TERMINATION_ALLOWEDf)) {
            station->flags |= BCM_L2_STATION_TRILL;
        }

        if (soc_mem_field32_get(unit, tcam_mem, entry,
                                FCOE_TERMINATION_ALLOWEDf)) {
            station->flags |= BCM_L2_STATION_FCOE;
        }
    }
    }
#endif

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
        soc_feature(unit, soc_feature_gh2_my_station) ||
        soc_feature(unit, soc_feature_hx5_oam_support)) {

        if (soc_mem_field_valid(unit, tcam_mem, OAM_TERMINATION_ALLOWEDf)) { 
            if (soc_mem_field32_get(unit, tcam_mem, entry,
                                    OAM_TERMINATION_ALLOWEDf)) {
                station->flags |= BCM_L2_STATION_OAM;
            }
        }
    }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIDENT(unit) ||
        SOC_IS_TD2_TT2(unit) || SOC_IS_TRIUMPH3(unit) ||
         soc_feature(unit, soc_feature_gh2_my_station)) {

        if (soc_mem_field_valid(unit, tcam_mem, DISCARDf)) { 
            if (soc_mem_field32_get(unit, tcam_mem, entry, DISCARDf)) {
                station->flags |= BCM_L2_STATION_DISCARD;
            }
        }

    }
#endif
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_taf)) {
        if (soc_mem_field_valid(unit, tcam_mem, TAF_GATE_ID_PROFILEf)) {
            uint32 hw_profile_id;

            hw_profile_id = soc_mem_field32_get(unit, tcam_mem,
                                                entry, TAF_GATE_ID_PROFILEf);
            if (0 != hw_profile_id) {
                bcm_tsn_pri_map_t sw_index;

                BCM_IF_ERROR_RETURN(
                    bcmi_esw_tsn_pri_map_map_id_get(
                        unit, bcmTsnPriMapTypeTafGate,
                        hw_profile_id, &sw_index));
                station->taf_gate_primap = sw_index;
            } else {
                station->taf_gate_primap = BCM_TSN_PRI_MAP_INVALID;
            }
        }
    } else
#endif /* BCM_TSN_SUPPORT */
    {
        station->taf_gate_primap = BCM_TSN_PRI_MAP_INVALID;
    }

    SC_UNLOCK(sc);

    return (rv);
}

/*
 * Function:
 *     bcm_tr_l2_station_size_get
 * Purpose:
 *     Get number of entries supported by L2 station API.
 * Parameters:
 *     unit   - (IN) BCM device number
 *     size   - (OUT) Station entry ID.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tr_l2_station_size_get(int unit, int *size)       
{
    _bcm_l2_station_control_t   *sc = NULL; /* Station control structure. */
    
    /* Input parameter check. */
    if (NULL == size) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));
    SC_LOCK(sc);

    *size = sc->entries_total;

    SC_UNLOCK(sc);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_tr_l2_station_entry_max_size_get
 * Purpose:
 *     Get number of max entries for L2 station.
 * Parameters:
 *     unit    - (IN) BCM device number
 *     overlay - (IN) Overlay/Underlay identification
 *     size    - (OUT)Max size of l2 station.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_tr_l2_station_entry_max_size_get(int unit, int overlay, int *size)
{
    _bcm_l2_station_control_t   *sc = NULL;

    /* Input parameter check. */
    if (NULL == size) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

    if (overlay) {
        *size = sc->entries_total_2;
    } else {
        *size = sc->entries_total;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        if (soc_feature(unit, soc_feature_fp_based_oam)) {
            *size = sc->entries_total + sc->olp_entries_total + 1;
        }
#endif
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            *size = sc->entries_total + sc->port_entries_total +
                    sc->olp_entries_total + 1;
        }
#endif
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_tr_l2_station_entry_last_sid_set
 * Purpose:
 *     Set the current last station id for L2 station.
 * Parameters:
 *     unit    - (IN) BCM device number
 *     overlay - (IN) Overlay/Underlay identification
 *     sid     - (IN) Station ID.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_tr_l2_station_entry_last_sid_set(int unit, int overlay, int sid)
{
    _bcm_l2_station_control_t *sc = NULL;

    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

    if (overlay) {
        if (sid) {
            sc->last_allocated_sid_2 = sid;
        } else {
            sc->last_allocated_sid_2 = _BCM_L2_STATION_ID_2_BASE - 1;
        }
    } else {
        if (sid) {
            sc->last_allocated_sid = sid;
        } else {
            sc->last_allocated_sid = _BCM_L2_STATION_ID_BASE - 1;
        }
    }
    return BCM_E_NONE;
}

int
bcm_tr_l2_station_traverse(int unit, bcm_l2_station_traverse_cb trav_fn, void *user_data) 
{
    _bcm_l2_station_control_t *sc = NULL;           /* Station control structure. */
    int                       index; /* Entry index.                       */
    int                       count;
    int                       rv = BCM_E_NONE;
    _bcm_l2_station_entry_t   *s_ent = NULL;
    bcm_l2_station_t          l2_station;
    int                       station_id;
    /* Input parameter check. */
    if (!trav_fn) {
        return BCM_E_PARAM;
    }
    
    memset((void *)(&l2_station), 0, sizeof(bcm_l2_station_t));
    
    BCM_IF_ERROR_RETURN(_bcm_l2_station_control_get(unit, &sc));

    count = sc->entries_total;
    
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        count = sc->entries_total + sc->port_entries_total + sc->olp_entries_total + 1;
    }
#endif 
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam)) {
        count = sc->entries_total + sc->olp_entries_total + 1;
    }
#endif

    for (index = 0; index < count; index++) {

        s_ent = sc->entry_arr[index]; /* H/W index */
        
        if (NULL == s_ent) {
            continue;
        }
        station_id = sc->entry_arr[index]->sid; /* Get S/W handle */
            
        rv = bcm_tr_l2_station_get(unit, station_id, &l2_station);
        if (BCM_FAILURE(rv)) {
            break;
        }
        
        rv = trav_fn(unit, &l2_station, user_data);
        if (BCM_FAILURE(rv)) {
            LOG_DEBUG(BSL_LS_BCM_L2,
                      (BSL_META_U(unit,
                                  "L2(unit %d) Info: Traverse (SID=%d)/(idx=%d) entry failed.\n"),
                       unit, station_id, index));
            break;
        }
    }

    BCM_IF_ERROR_RETURN(rv);

#ifdef BCM_RIOT_SUPPORT
    if (soc_feature(unit, soc_feature_riot)) {
        count = sc->entries_total_2;

        for (index = 0; index < count; index++) {
            s_ent = sc->entry_arr_2[index];

            if (s_ent == NULL) {
                continue;
            }

            station_id = s_ent->sid;
            sal_memset((void*)&l2_station, 0, sizeof(bcm_l2_station_t));
            rv = bcm_tr_l2_station_get(unit, station_id, &l2_station);
            if (BCM_FAILURE(rv)) {
                break;
            }

            rv = trav_fn(unit, &l2_station, user_data);
            if (BCM_FAILURE(rv)) {
                LOG_DEBUG(BSL_LS_BCM_L2,
                          (BSL_META_U(unit,
                                      "L2(unit %d) Info: Traverse (SID=%d)/(idx=%d) entry failed.\n"),
                           unit, station_id, index));
                break;
            }
        }
    }
#endif /* BCM_RIOT_SUPPORT */

    return rv;
}

#endif /* !BCM_TRIUMPH_SUPPORT */

/*
 * Function:
 *     _bcm_tr_l2_ppa_setup
 * Description:
 *     Setup hardware L2 PPA registers
 * Parameters:
 *     unit         device number
 *     flags        flags BCM_L2_REPLACE_*
 *     rep_st       structure with information of what to replace
 * Return:
 *     BCM_E_XXX
 */
STATIC int
_bcm_tr_l2_ppa_setup(int unit, _bcm_l2_replace_t *rep_st)
{
    soc_field_t field;
    uint32  rval, ppa_mode;
    uint32  rval_limit, limit_en;
    int field_len;
#ifdef BCM_HURRICANE3_SUPPORT
    uint64  rval64;
#endif

    switch (rep_st->flags &
            (BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_MATCH_VLAN |
             BCM_L2_REPLACE_DELETE)) {
    case BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_DELETE:
        ppa_mode = 0;
        break;
    case BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_DELETE:
        ppa_mode = 1;
        break;
    case BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_MATCH_VLAN |
        BCM_L2_REPLACE_DELETE:
        ppa_mode = 2;
        break;
    case BCM_L2_REPLACE_DELETE:
        ppa_mode = 3;
        break;
    case BCM_L2_REPLACE_MATCH_DEST:
        ppa_mode = 4;
        break;
    case BCM_L2_REPLACE_MATCH_VLAN:
        ppa_mode = 5;
        break;
    case BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_MATCH_VLAN :
        ppa_mode = 6;
        break;
    default:
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_PER_PORT_REPL_CONTROLr(unit, &rval));
    if ((soc_feature(unit, soc_feature_mac_learn_limit))) {

        SOC_IF_ERROR_RETURN(READ_SYS_MAC_LIMIT_CONTROLr(unit, &rval_limit));
        limit_en = soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, 
                                 rval_limit, ENABLEf);
        soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &rval, LIMIT_COUNTEDf, 
                          limit_en);
        soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &rval, 
                      USE_OLD_LIMIT_COUNTEDf, limit_en);
    }
    soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &rval, KEY_TYPEf,
                      rep_st->key_type);
    soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &rval, EXCL_STATICf,
                      rep_st->flags & BCM_L2_REPLACE_MATCH_STATIC ? 0 : 1);
    soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &rval, EXCL_NON_PENDINGf,
                      rep_st->flags & BCM_L2_REPLACE_PENDING ? 1 : 0);
    soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &rval, EXCL_PENDINGf,
                      rep_st->flags & BCM_L2_REPLACE_PENDING ? 0 : 1);
    if (!(rep_st->flags & BCM_L2_REPLACE_DELETE)) {
        if (rep_st->new_dest.vp != -1) {
            /* For virtual ports PORT_NUMf provides new VPG,
             * Reset MODULE_IDf and PORT_NUMf as it is overlayed with VPG
             */
            soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &rval, Tf, 0);
            soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &rval, MODULE_IDf, 
                              (rep_st->new_dest.vp >> 6));
            soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &rval, PORT_NUMf, 
                              (rep_st->new_dest.vp & 0x3F));
            if (soc_reg_field_valid(unit, PER_PORT_REPL_CONTROLr, DEST_TYPEf)) {
                soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &rval,
                                  DEST_TYPEf, 2);
            }
        } else if (rep_st->new_dest.trunk != -1) {
            soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &rval, Tf, 1);
            soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &rval, TGIDf,
                              rep_st->new_dest.trunk);
        } else {
            soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &rval, Tf, 0);
            soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &rval, MODULE_IDf,
                              rep_st->new_dest.module);
            soc_reg_field_set(unit, PER_PORT_REPL_CONTROLr, &rval, PORT_NUMf,
                              rep_st->new_dest.port);
        }
    }
    SOC_IF_ERROR_RETURN(WRITE_PER_PORT_REPL_CONTROLr(unit, rval));

#ifdef BCM_HURRICANE3_SUPPORT
    if (SOC_REG_IS_VALID(unit, PER_PORT_AGE_CONTROL_64r)) {
        COMPILER_64_ZERO(rval64); 
        soc_reg64_field32_set(unit, PER_PORT_AGE_CONTROL_64r, &rval64, 
                              PPA_MODEf, ppa_mode);
        if (rep_st->flags & BCM_L2_REPLACE_MATCH_VLAN) {
            soc_reg64_field32_set(unit, PER_PORT_AGE_CONTROL_64r, &rval64, 
                                  VLAN_IDf, 
                                  rep_st->key_vfi != -1 ?
                                  rep_st->key_vfi : rep_st->key_vlan);
        }
        if (rep_st->flags & BCM_L2_REPLACE_MATCH_DEST) {
            if (rep_st->match_dest.vp != -1) {
                if (soc_reg_field_valid(unit, PER_PORT_AGE_CONTROL_64r, DEST_TYPEf)) {
                    soc_reg64_field32_set(unit, PER_PORT_AGE_CONTROL_64r, &rval64, 
                                          DEST_TYPEf, 2);
                    soc_reg64_field32_set(unit, PER_PORT_AGE_CONTROL_64r, &rval64, 
                                          VPGf, rep_st->match_dest.vp);
                } else {
                    BCM_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, ING_Q_BEGINr, REG_PORT_ANY,
                                                VPG_TYPEf, 1));
                    field_len = soc_reg_field_length(unit, PER_PORT_AGE_CONTROL_64r,
                                                     PORT_NUMf);
                    soc_reg64_field32_set(unit, PER_PORT_AGE_CONTROL_64r, &rval64, 
                                          MODULE_IDf, rep_st->match_dest.vp >> field_len);
                    soc_reg64_field32_set(unit, PER_PORT_AGE_CONTROL_64r, &rval64, 
                                          PORT_NUMf, 
                                          rep_st->match_dest.vp & ((1 << field_len) - 1));
                }
            } else if (rep_st->match_dest.trunk != -1) {
                soc_reg64_field32_set(unit, PER_PORT_AGE_CONTROL_64r, &rval64, 
                                      Tf, 1);
                soc_reg64_field32_set(unit, PER_PORT_AGE_CONTROL_64r, &rval64, 
                                      TGIDf, rep_st->match_dest.trunk);
            } else {
                soc_reg64_field32_set(unit, PER_PORT_AGE_CONTROL_64r, &rval64, 
                                      MODULE_IDf, rep_st->match_dest.module);
                soc_reg64_field32_set(unit, PER_PORT_AGE_CONTROL_64r, &rval64, 
                                      PORT_NUMf, rep_st->match_dest.port);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_PER_PORT_AGE_CONTROL_64r(unit, rval64));
    } else 
#endif
    {
        rval = 0;
        soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &rval, PPA_MODEf,
                          ppa_mode);
        if (rep_st->flags & BCM_L2_REPLACE_MATCH_VLAN) {
            soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &rval, VLAN_IDf,
                              rep_st->key_vfi != -1 ?
                              rep_st->key_vfi : rep_st->key_vlan);
        }
        if (rep_st->flags & BCM_L2_REPLACE_MATCH_DEST) {
            if (rep_st->match_dest.vp != -1) {
                if (soc_reg_field_valid(unit, PER_PORT_AGE_CONTROLr, DEST_TYPEf)) {
                    soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &rval,
                                      DEST_TYPEf, 2);
                    soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &rval, VPGf,
                                      rep_st->match_dest.vp);
                } else {
                    BCM_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, ING_Q_BEGINr, REG_PORT_ANY,
                                                VPG_TYPEf, 1));
                    field_len = soc_reg_field_length(unit, PER_PORT_AGE_CONTROLr,
                                                     PORT_NUMf);
                    soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &rval,
                                      MODULE_IDf,
                                      rep_st->match_dest.vp >> field_len);
                    soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &rval,
                                      PORT_NUMf,
                                      rep_st->match_dest.vp &
                                      ((1 << field_len) - 1));
                }
            } else if (rep_st->match_dest.trunk != -1) {
                soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &rval, Tf, 1);
                soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &rval, TGIDf,
                                  rep_st->match_dest.trunk);
            } else {
                soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &rval, MODULE_IDf,
                                  rep_st->match_dest.module);
                soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &rval, PORT_NUMf,
                                  rep_st->match_dest.port);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_PER_PORT_AGE_CONTROLr(unit, rval));
    }

    if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
        field = rep_st->flags & BCM_L2_REPLACE_DELETE ?
            L2_MOD_FIFO_ENABLE_PPA_DELETEf : L2_MOD_FIFO_ENABLE_PPA_REPLACEf;
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify
                            (unit, AUX_ARB_CONTROLr, REG_PORT_ANY, field,
                             rep_st->flags & BCM_L2_REPLACE_NO_CALLBACKS ?
                             0 : soc_l2mod_running(unit, NULL, NULL)));
    }

    return BCM_E_NONE;
}

#ifdef BCM_TRIUMPH_SUPPORT
/*
 * Function:
 *     _bcm_tr_ext_l2_ppa_setup
 * Description:
 *     Setup hardware external L2 PPA registers
 * Parameters:
 *     unit         device number
 *     rep_st       structure with information of what to replace
 * Return:
 *     BCM_E_XXX
 */
STATIC int
_bcm_tr_ext_l2_ppa_setup(int unit, _bcm_l2_replace_t *rep_st)
{
    uint32 rval, ppa_mode;
    ext_l2_mod_fifo_entry_t ext_l2_mod_entry;
    ext_l2_entry_entry_t ext_l2_entry;

    switch (rep_st->flags &
            (BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_MATCH_VLAN |
             BCM_L2_REPLACE_DELETE)) {
    case BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_DELETE:
        ppa_mode = 0;
        break;
    case BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_DELETE:
        ppa_mode = 1;
        break;
    case BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_MATCH_VLAN |
        BCM_L2_REPLACE_DELETE:
        ppa_mode = 2;
        break;
    case BCM_L2_REPLACE_DELETE:
        ppa_mode = 3;
        break;
    case BCM_L2_REPLACE_MATCH_DEST:
        ppa_mode = 4;
        break;
    case BCM_L2_REPLACE_MATCH_VLAN:
        ppa_mode = 5;
        break;
    case BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_MATCH_VLAN :
        ppa_mode = 6;
        break;
    default:
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_ESM_PER_PORT_REPL_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, ESM_PER_PORT_REPL_CONTROLr, &rval, KEY_TYPE_VFIf,
                      rep_st->key_vfi != -1 ? 1 : 0);
    soc_reg_field_set(unit, ESM_PER_PORT_REPL_CONTROLr, &rval, EXCL_STATICf,
                      rep_st->flags & BCM_L2_REPLACE_MATCH_STATIC ? 0 : 1);
    if (!(rep_st->flags & BCM_L2_REPLACE_DELETE)) {
        if(rep_st->new_dest.vp != -1)
        {
            soc_reg_field_set(unit, ESM_PER_PORT_REPL_CONTROLr, &rval, Tf, 0);
            soc_reg_field_set(unit, ESM_PER_PORT_REPL_CONTROLr, &rval, MODULE_IDf, 
                              (rep_st->new_dest.vp >> 6));
            soc_reg_field_set(unit, ESM_PER_PORT_REPL_CONTROLr, &rval, PORT_NUMf, 
                              (rep_st->new_dest.vp & 0x3F));
        }
        else if (rep_st->new_dest.trunk != -1) {
            soc_reg_field_set(unit, ESM_PER_PORT_REPL_CONTROLr, &rval, Tf, 1);
            soc_reg_field_set(unit, ESM_PER_PORT_REPL_CONTROLr, &rval, TGIDf,
                              rep_st->new_dest.trunk);
        } else {
            soc_reg_field_set(unit, ESM_PER_PORT_REPL_CONTROLr, &rval, Tf, 0);
            soc_reg_field_set(unit, ESM_PER_PORT_REPL_CONTROLr, &rval,
                              MODULE_IDf, rep_st->new_dest.module);
            soc_reg_field_set(unit, ESM_PER_PORT_REPL_CONTROLr, &rval,
                              PORT_NUMf, rep_st->new_dest.port);
        }
    }
    if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
        soc_reg_field_set(unit, ESM_PER_PORT_REPL_CONTROLr, &rval, CPU_NOTIFYf,
                          rep_st->flags & BCM_L2_REPLACE_NO_CALLBACKS ? 0 : 1);
    }
    SOC_IF_ERROR_RETURN(WRITE_ESM_PER_PORT_REPL_CONTROLr(unit, rval));

    /*
     * Unlike L2_MOD_FIFO, EXT_L2_MOD_FIFO does not report both new and
     * replaced L2 destination for PPA replace command. To workaround
     * the problem, we add an special entry to EXT_L2_MOD_FIFO before
     * issuing the PPA replace command. The special entry has the new
     * destination and a special "type" value. L2 mod fifo processing
     * thread knows all entries after this special entry are associated
     * with this new destination.
     */
    sal_memset(&ext_l2_mod_entry, 0, sizeof(ext_l2_mod_entry));
    sal_memset(&ext_l2_entry, 0, sizeof(ext_l2_entry));

    if (rep_st->new_dest.trunk != -1) {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, &ext_l2_entry, Tf, 1);
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, &ext_l2_entry, TGIDf,
                            rep_st->new_dest.trunk);
    } else {
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, &ext_l2_entry, MODULE_IDf,
                            rep_st->new_dest.module);
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, &ext_l2_entry, PORT_NUMf,
                            rep_st->new_dest.port);
    }
    soc_mem_field_set(unit, EXT_L2_MOD_FIFOm, (uint32 *)&ext_l2_mod_entry,
                      WR_DATAf, (uint32 *)&ext_l2_entry);
    /* borrow INSERTED type (value 3) as special mark */
    soc_mem_field32_set(unit, EXT_L2_MOD_FIFOm, &ext_l2_mod_entry, TYPf, 3);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, EXT_L2_MOD_FIFOm, MEM_BLOCK_ANY,
                                      0, &ext_l2_mod_entry));

    rval = 0;
    soc_reg_field_set(unit, ESM_PER_PORT_AGE_CONTROLr, &rval, PPA_MODEf,
                      ppa_mode);
    if (rep_st->flags & BCM_L2_REPLACE_MATCH_VLAN) {
        soc_reg_field_set(unit, ESM_PER_PORT_AGE_CONTROLr, &rval, VLAN_IDf,
                          rep_st->key_vfi != -1 ?
                          rep_st->key_vfi : rep_st->key_vlan);
    }
    if (rep_st->flags & BCM_L2_REPLACE_MATCH_DEST) {
        if (rep_st->match_dest.trunk != -1) {
            soc_reg_field_set(unit, ESM_PER_PORT_AGE_CONTROLr, &rval, Tf, 1);
            soc_reg_field_set(unit, ESM_PER_PORT_AGE_CONTROLr, &rval, TGIDf,
                              rep_st->match_dest.trunk);
        } else {
            soc_reg_field_set(unit, ESM_PER_PORT_AGE_CONTROLr, &rval,
                              MODULE_IDf, rep_st->match_dest.module);
            soc_reg_field_set(unit, ESM_PER_PORT_AGE_CONTROLr, &rval,
                              PORT_NUMf, rep_st->match_dest.port);
        }
    }
    SOC_IF_ERROR_RETURN(WRITE_ESM_PER_PORT_AGE_CONTROLr(unit, rval));

    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH_SUPPORT */

/*
 * Function:
 *     _bcm_td_l2_bulk_control_setup
 * Description:
 *     Setup hardware L2 bulk control registers
 * Parameters:
 *     unit         device number
 *     rep_st       structure with information of what to replace
 * Return:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td_l2_bulk_control_setup(int unit, _bcm_l2_replace_t *rep_st)
{
    uint32 rval;
    int action;

    SOC_IF_ERROR_RETURN(READ_L2_BULK_CONTROLr(unit, &rval)); 
    if (rep_st->int_flags & _BCM_L2_REPLACE_INT_NO_ACTION) {
        action = 0;
    } else if (rep_st->flags & BCM_L2_REPLACE_DELETE) {
        action = 1;
    } else if (rep_st->flags & BCM_L2_REPLACE_AGE) {
        action = 3;
    } else {
        action = 2;
    }
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, action);
    if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
        int fifo_record = rep_st->flags & BCM_L2_REPLACE_NO_CALLBACKS ? 0 : 1;
        if (!soc_l2mod_running(unit, NULL, NULL)) {
            fifo_record = 0;
        }
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 
                          fifo_record); 
    }
    SOC_IF_ERROR_RETURN(WRITE_L2_BULK_CONTROLr(unit, rval)); 

    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULK_MATCH_MASKm(unit, MEM_BLOCK_ALL, 0,
                                   &rep_st->match_mask));
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULK_MATCH_DATAm(unit, MEM_BLOCK_ALL, 0,
                                   &rep_st->match_data));

    if (action == 2) {
        BCM_IF_ERROR_RETURN
            (WRITE_L2_BULK_REPLACE_MASKm(unit, MEM_BLOCK_ALL, 0,
                                         &rep_st->new_mask));
        BCM_IF_ERROR_RETURN
            (WRITE_L2_BULK_REPLACE_DATAm(unit, MEM_BLOCK_ALL, 0,
                                         &rep_st->new_data));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_l2_bulk_control_setup
 * Description:
 *     Setup hardware L2 bulk control registers for Tomahawk chip
 * Parameters:
 *     unit         device number
 *     rep_st       structure with information of what to replace
 * Return:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_l2_bulk_control_setup(int unit, _bcm_l2_replace_t *rep_st)
{
#if defined(BCM_TOMAHAWK_SUPPORT)
    uint32 rval;
    uint32 action;

    SOC_IF_ERROR_RETURN(READ_L2_BULK_CONTROLr(unit, &rval));

    if (rep_st->int_flags & _BCM_L2_REPLACE_INT_NO_ACTION) {
        action = 0;
    } else if (rep_st->flags & BCM_L2_REPLACE_DELETE) {
        action = 1;
    } else if (rep_st->flags & BCM_L2_REPLACE_AGE) {
        action = 3;
    } else {
        action = 2;
    }
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, action);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                      soc_mem_index_count(unit, L2Xm));
    if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
        int fifo_record = rep_st->flags & BCM_L2_REPLACE_NO_CALLBACKS ? 0 : 1;
        if (!soc_l2mod_running(unit, NULL, NULL)) {
            fifo_record = 0;
        }

        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf,
                          fifo_record);
    }
    if (soc_feature(unit, soc_feature_l2x_bulk_improvement)) {
        if (soc_reg_field_valid(unit, L2_BULK_CONTROLr, BURST_ENTRIESf)) {
            int burst_length = rep_st->flags & BCM_L2_REPLACE_NO_CALLBACKS ?
                               _SOC_TR3_L2_BULK_BURST_MAX : _SOC_TR3_L2_BULK_BURST_MIN;

            soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, BURST_ENTRIESf,
                              burst_length);
        }
    }
    SOC_IF_ERROR_RETURN(WRITE_L2_BULK_CONTROLr(unit, rval));

    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_MATCH_MASK_INX,
                                   &rep_st->match_mask));
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_MATCH_DATA_INX,
                                   &rep_st->match_data));

    if (!(rep_st->flags & BCM_L2_REPLACE_DELETE)) {
        BCM_IF_ERROR_RETURN
            (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_REPLACE_MASK_INX,
                                         &rep_st->new_mask));
        BCM_IF_ERROR_RETURN
            (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_REPLACE_DATA_INX,
                                         &rep_st->new_data));
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Match DEST_TYPE + MODULE_ID + PORT_NUM and replace with one of following:
 * DEST_TYPE = 0, new MODULE_ID, new PORT_NUM
 * DEST_TYPE = 1, new TGID
 * Notes:
 *     Need to match all bits of DEST_TYPE + DESTINATION, therefore can not
 *     match on trunk destination because the unused overlay bits can have any
 *     data.
 */
STATIC int
_bcm_td_l2_bulk_replace_modport(int unit, _bcm_l2_replace_t *rep_st)
{
    l2_bulk_match_mask_entry_t match_mask;
    l2_bulk_match_data_entry_t match_data;
    l2_bulk_replace_mask_entry_t repl_mask;
    l2_bulk_replace_data_entry_t repl_data;
    int field_len;

    sal_memset(&match_mask, 0, sizeof(match_mask));
    sal_memset(&match_data, 0, sizeof(match_data));
    sal_memset(&repl_mask, 0, sizeof(repl_mask));
    sal_memset(&repl_data, 0, sizeof(repl_data));

    soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask, VALIDf, 1);
    soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, &match_data, VALIDf, 1);

    field_len = soc_mem_field_length(unit, L2_BULK_MATCH_MASKm, KEY_TYPEf);
    soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask, KEY_TYPEf,
                        (1 << field_len) - 1);
    /* KEY_TYPE field in data is 0 */

    field_len = soc_mem_field_length(unit, L2_BULK_MATCH_MASKm, DEST_TYPEf);
    soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask, DEST_TYPEf,
                        (1 << field_len) - 1);
    /* DEST_TYPE field in data is 0 */

    field_len = soc_mem_field_length(unit, L2_BULK_MATCH_MASKm, MODULE_IDf);
    soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask, MODULE_IDf,
                        (1 << field_len) - 1);
    soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, &match_data, MODULE_IDf,
                        rep_st->match_dest.module);

    field_len = soc_mem_field_length(unit, L2_BULK_MATCH_MASKm, PORT_NUMf);
    soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask, PORT_NUMf,
                        (1 << field_len) - 1);
    soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, &match_data, PORT_NUMf,
                        rep_st->match_dest.port);

    if (!(rep_st->flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask,
                            STATIC_BITf, 1);
        /* STATIC_BIT field in data is 0 */
    }

    soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask, PENDINGf, 1);
    if (rep_st->flags & BCM_L2_REPLACE_PENDING) {
        soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, &match_data, PENDINGf,
                            1);
    }

    soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask, EVEN_PARITYf,
                        1);

    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULK_MATCH_MASKm(unit, MEM_BLOCK_ALL, 0, &match_mask));

    field_len = soc_mem_field_length(unit, L2_BULK_REPLACE_MASKm, DEST_TYPEf);
    soc_mem_field32_set(unit, L2_BULK_REPLACE_MASKm, &repl_mask, DEST_TYPEf,
                        (1 << field_len) - 1);

    field_len = soc_mem_field_length(unit, L2_BULK_REPLACE_MASKm, MODULE_IDf);
    soc_mem_field32_set(unit, L2_BULK_REPLACE_MASKm, &repl_mask, MODULE_IDf,
                        (1 << field_len) - 1);

    field_len = soc_mem_field_length(unit, L2_BULK_REPLACE_MASKm, PORT_NUMf);
    soc_mem_field32_set(unit, L2_BULK_REPLACE_MASKm, &repl_mask, PORT_NUMf,
                        (1 << field_len) - 1);

    if (rep_st->new_dest.trunk != -1) {
        soc_mem_field32_set(unit, L2_BULK_REPLACE_DATAm, &repl_data, Tf, 1);
        soc_mem_field32_set(unit, L2_BULK_REPLACE_DATAm, &repl_data, TGIDf,
                            rep_st->new_dest.trunk);
    } else {
        /* T field in data is 0 */
        soc_mem_field32_set(unit, L2_BULK_REPLACE_DATAm, &repl_data,
                            MODULE_IDf, rep_st->new_dest.module);
        soc_mem_field32_set(unit, L2_BULK_REPLACE_DATAm, &repl_data, PORT_NUMf,
                            rep_st->new_dest.port);
    }

    soc_mem_field32_set(unit, L2_BULK_REPLACE_MASKm, &repl_mask, EVEN_PARITYf,
                        1);
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULK_REPLACE_MASKm(unit, MEM_BLOCK_ALL, 0, &repl_mask));

    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, L2_BULK_CONTROLr, REG_PORT_ANY, ACTIONf,
                                2));

    /* Replace all entries having EVEN_PARITY == 0 */
    /* EVEN_PARITY field in data is 0 */
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULK_MATCH_DATAm(unit, MEM_BLOCK_ALL, 0, &match_data));

    soc_mem_field32_set(unit, L2_BULK_REPLACE_DATAm, &repl_data, EVEN_PARITYf,
                        1);
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULK_REPLACE_DATAm(unit, MEM_BLOCK_ALL, 0, &repl_data));

    BCM_IF_ERROR_RETURN(soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr));

    /* Replace all entries having EVEN_PARITY == 1 */
    soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, &match_data, EVEN_PARITYf,
                        1);
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULK_MATCH_DATAm(unit, MEM_BLOCK_ALL, 0, &match_data));

    soc_mem_field32_set(unit, L2_BULK_REPLACE_DATAm, &repl_data, EVEN_PARITYf,
                        0);
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULK_REPLACE_DATAm(unit, MEM_BLOCK_ALL, 0, &repl_data));

    BCM_IF_ERROR_RETURN(soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr));

    return BCM_E_NONE;
}

#ifdef BCM_TRIDENT2_SUPPORT
/*
 * Function:
 *     _bcm_td_l2_bulk_replace_delete_age
 * Description:
 *     Use replace operation to replace the delete or age operation
 * Parameters:
 *     unit         device number
 *     rep_st       structure with information of what to replace
 * Return:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td_l2_bulk_replace_delete_age(int unit, _bcm_l2_replace_t *rep_st)
{
    int rv = BCM_E_NONE;
    _bcm_l2_replace_t   rep_st_replace;
    soc_control_t *soc = SOC_CONTROL(unit);

    sal_memcpy(&rep_st_replace, rep_st, 
                            sizeof(_bcm_l2_replace_t));

    rep_st_replace.flags &= ~(BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_AGE);

    if (rep_st->flags & BCM_L2_REPLACE_DELETE) {
        if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
            /* Only Clear VALIDf */
            if (soc_feature(unit, soc_feature_base_valid)) {
                soc_mem_field32_set(unit, L2Xm, 
                                &rep_st_replace.new_mask, BASE_VALIDf, 1);
            } else {
            soc_mem_field32_set(unit, L2Xm, 
                                &rep_st_replace.new_mask, VALIDf, 1);
            }
        } else {
            /* Clear All Fields */
            sal_memset(&rep_st_replace.new_mask, 0xff, 
                                       sizeof(l2x_entry_t));
        }
    } else if (rep_st->flags & BCM_L2_REPLACE_AGE) {
        _bcm_l2_replace_t   rep_st_age;

        soc_mem_field32_set(unit, L2Xm, 
                                &rep_st_replace.new_mask, HITSAf, 1);
        soc_mem_field32_set(unit, L2Xm, 
                                &rep_st_replace.new_mask, HITDAf, 1);
        soc_mem_field32_set(unit, L2Xm, 
                                &rep_st_replace.new_mask, LOCAL_SAf, 1);

        sal_memcpy(&rep_st_age, &rep_st_replace, 
                                sizeof(_bcm_l2_replace_t));

        /* Iteration 1: Match VALIDf=1, STATICf=0, HITSAf=0, HITDAf=0 */
        soc_mem_field32_set(unit, L2Xm, 
                                &rep_st_age.match_mask, HITSAf, 1);
        soc_mem_field32_set(unit, L2Xm, 
                                &rep_st_age.match_mask, HITDAf, 1);

        if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
            /* Iteration 1: Only Clear VALIDf */
            if (soc_feature(unit, soc_feature_base_valid)) {
                soc_mem_field32_set(unit, L2Xm, 
                                    &rep_st_age.new_mask, BASE_VALIDf, 1);
            } else {
            soc_mem_field32_set(unit, L2Xm, 
                                &rep_st_age.new_mask, VALIDf, 1);
            }

            /* Iteration 2: L2_BULK_CONTROLr_L2_MOD_FIFO_RECORDf = 0 */
            rep_st_replace.flags |= BCM_L2_REPLACE_NO_CALLBACKS;

        } else {
            /* Iteration 1: Clear All Fields */
            sal_memset(&rep_st_age.new_mask, 0xff, 
                                       sizeof(l2x_entry_t));
        }

        BCM_IF_ERROR_RETURN(_bcm_td_l2_bulk_control_setup(unit, &rep_st_age));

        BCM_IF_ERROR_RETURN(soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr));
    }

    BCM_IF_ERROR_RETURN(_bcm_td_l2_bulk_control_setup(unit, &rep_st_replace));

    BCM_IF_ERROR_RETURN(soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr));

    if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
        return BCM_E_RESOURCE;
    }

    rv = _soc_l2x_sync_replace
                (unit, &rep_st->match_data, &rep_st->match_mask,
                 rep_st->flags & BCM_L2_REPLACE_NO_CALLBACKS ?
                 SOC_L2X_NO_CALLBACKS : 0);
    
    SOC_L2_DEL_SYNC_UNLOCK(soc);

    return rv;
}
#endif

/*
 *     _bcm_th_l2_bulk_replace_modport
 * Description:
 *     Match L2 entries with these fields: DEST_TYPE + MODULE_ID + PORT_NUM 
 *     Replace with one of following:
 *         DEST_TYPE = 0, new MODULE_ID, new PORT_NUM 
 *         DEST_TYPE = 1, new TGID
 * Parameters:
 *     unit         device number
 *     rep_st       structure with match/replace information
 * Return:
 *     BCM_E_XXX
 */
STATIC int 
_bcm_th_l2_bulk_replace_modport(int unit, _bcm_l2_replace_t *rep_st)
{
#if defined(BCM_TOMAHAWK_SUPPORT)
    l2_bulk_entry_t match_mask;
    l2_bulk_entry_t match_data;
    l2_bulk_entry_t repl_mask;
    l2_bulk_entry_t repl_data;
    int field_len;

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow) &&
        (rep_st->view_id != 0)) {
        return _bcm_td3_flex_l2_bulk_replace_modport(unit, rep_st);
    }
#endif

    sal_memset(&match_mask, 0, sizeof(match_mask));
    sal_memset(&match_data, 0, sizeof(match_data));
    sal_memset(&repl_mask, 0, sizeof(repl_mask));
    sal_memset(&repl_data, 0, sizeof(repl_data));

    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, L2_BULKm, &match_mask, BASE_VALIDf, 1);
        soc_mem_field32_set(unit, L2_BULKm, &match_data, BASE_VALIDf, 1);
    } else {
    soc_mem_field32_set(unit, L2_BULKm, &match_mask, VALIDf, 1);
    soc_mem_field32_set(unit, L2_BULKm, &match_data, VALIDf, 1);
    }

    field_len = soc_mem_field_length(unit, L2_BULKm, KEY_TYPEf);
    soc_mem_field32_set(unit, L2_BULKm, &match_mask, KEY_TYPEf,
                        (1 << field_len) - 1);
    /* KEY_TYPE field in data is 0 */

    field_len = soc_mem_field_length(unit, L2_BULKm, DEST_TYPEf);
    soc_mem_field32_set(unit, L2_BULKm, &match_mask, DEST_TYPEf,
                        (1 << field_len) - 1);
    /* DEST_TYPE field in data is 0 */
    if (soc_feature(unit, soc_feature_generic_dest)) {
        soc_mem_field32_dest_set(unit, L2_BULKm, &match_mask,
            DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
            SOC_MEM_FIF_DGPP_MOD_ID_MASK | SOC_MEM_FIF_DGPP_PORT_MASK);
        soc_mem_field32_dest_set(unit, L2_BULKm, &match_data,
            DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
            rep_st->match_dest.module | rep_st->match_dest.port);
    } else {
    field_len = soc_mem_field_length(unit, L2_BULKm, MODULE_IDf);
    soc_mem_field32_set(unit, L2_BULKm, &match_mask, MODULE_IDf,
                        (1 << field_len) - 1);
    soc_mem_field32_set(unit, L2_BULKm, &match_data, MODULE_IDf,
                        rep_st->match_dest.module);

    field_len = soc_mem_field_length(unit, L2_BULKm, PORT_NUMf);
    soc_mem_field32_set(unit, L2_BULKm, &match_mask, PORT_NUMf,
                        (1 << field_len) - 1);
    soc_mem_field32_set(unit, L2_BULKm, &match_data, PORT_NUMf,
                        rep_st->match_dest.port);
    }

    if (!(rep_st->flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        soc_mem_field32_set(unit, L2_BULKm, &match_mask,
                            STATIC_BITf, 1);
        /* STATIC_BIT field in data is 0 */
    }

    soc_mem_field32_set(unit, L2_BULKm, &match_mask, PENDINGf, 1);
    if (rep_st->flags & BCM_L2_REPLACE_PENDING) {
        soc_mem_field32_set(unit, L2_BULKm, &match_data, PENDINGf,
                            1);
    }

    soc_mem_field32_set(unit, L2_BULKm, &match_mask, EVEN_PARITYf,
                        1);

    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_MATCH_MASK_INX, 
                        &match_mask));

    field_len = soc_mem_field_length(unit, L2_BULKm, DEST_TYPEf);
    soc_mem_field32_set(unit, L2_BULKm, &repl_mask, DEST_TYPEf,
                        (1 << field_len) - 1);

    if (soc_feature(unit, soc_feature_generic_dest)) {
        soc_mem_field32_dest_set(unit, L2_BULKm, &repl_mask,
            DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
            SOC_MEM_FIF_DGPP_MOD_ID_MASK | SOC_MEM_FIF_DGPP_PORT_MASK);
        if (rep_st->new_dest.trunk != -1) {
            soc_mem_field32_dest_set(unit, L2_BULKm, &repl_data,
                DESTINATIONf, SOC_MEM_FIF_DEST_LAG, rep_st->new_dest.trunk);
        } else {
            soc_mem_field32_dest_set(unit, L2_BULKm, &repl_data,
                DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                rep_st->new_dest.module | rep_st->new_dest.port);
        }
    } else {
    field_len = soc_mem_field_length(unit, L2_BULKm, MODULE_IDf);
    soc_mem_field32_set(unit, L2_BULKm, &repl_mask, MODULE_IDf,
                        (1 << field_len) - 1);

    field_len = soc_mem_field_length(unit, L2_BULKm, PORT_NUMf);
    soc_mem_field32_set(unit, L2_BULKm, &repl_mask, PORT_NUMf,
                        (1 << field_len) - 1);

    if (rep_st->new_dest.trunk != -1) {
        soc_mem_field32_set(unit, L2_BULKm, &repl_data, Tf, 1);
        soc_mem_field32_set(unit, L2_BULKm, &repl_data, TGIDf,
                            rep_st->new_dest.trunk);
    } else {
        /* T field in data is 0 */
        soc_mem_field32_set(unit, L2_BULKm, &repl_data,
                            MODULE_IDf, rep_st->new_dest.module);
        soc_mem_field32_set(unit, L2_BULKm, &repl_data, PORT_NUMf,
                            rep_st->new_dest.port);
    }
    }

    soc_mem_field32_set(unit, L2_BULKm, &repl_mask, EVEN_PARITYf,
                        1);
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_REPLACE_MASK_INX, 
                        &repl_mask));

    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, L2_BULK_CONTROLr, REG_PORT_ANY, ACTIONf,
                                2));
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, L2_BULK_CONTROLr, REG_PORT_ANY,
                            NUM_ENTRIESf, soc_mem_index_count(unit, L2Xm)));

    /* Replace all entries having EVEN_PARITY == 0 */
    /* EVEN_PARITY field in data is 0 */
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_MATCH_DATA_INX, 
                        &match_data));

    soc_mem_field32_set(unit, L2_BULKm, &repl_data, EVEN_PARITYf,
                        1);
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_REPLACE_DATA_INX, 
                        &repl_data));

    BCM_IF_ERROR_RETURN(soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr));

    /* Replace all entries having EVEN_PARITY == 1 */
    soc_mem_field32_set(unit, L2_BULKm, &match_data, EVEN_PARITYf,
                        1);
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_MATCH_DATA_INX, 
                        &match_data));

    soc_mem_field32_set(unit, L2_BULKm, &repl_data, EVEN_PARITYf,
                        0);
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_REPLACE_DATA_INX,
                        &repl_data));

    BCM_IF_ERROR_RETURN(soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *     _bcm_tr_l2_replace_by_hw
 * Description:
 *     Helper function to _bcm_l2_replace_by_hw
 * Parameters:
 *     unit         device number
 *     rep_st       structure with information of what to replace
 * Return:
 *     BCM_E_XXX
 */
int
_bcm_tr_l2_replace_by_hw(int unit, _bcm_l2_replace_t *rep_st)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_reg_t int_reg, ext_reg;
    int rv;
    int bulk_replace_modport;
    uint32 parity_diff;
#if defined(BCM_TRIUMPH_SUPPORT)
    int do_l2e_ppa_match = 1;
#endif	/* BCM_TRIUMPH_SUPPORT*/
#ifdef BCM_TRIDENT3_SUPPORT
    soc_flow_db_mem_view_info_t mem_view_info;
#endif	/* BCM_TRIDENT3_SUPPORT*/
#ifdef BCM_TRIDENT2_SUPPORT
    soc_mem_t L2mem = L2Xm;
#endif

    if (NULL == rep_st) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow) &&
        (rep_st->view_id != 0)) {

        BCM_IF_ERROR_RETURN(
            soc_flow_db_mem_view_info_get(unit,
                              rep_st->view_id,
                              &mem_view_info));
        L2mem = mem_view_info.mem;
    }
#endif

    bulk_replace_modport = FALSE;
    if (soc_feature(unit, soc_feature_l2_bulk_bypass_replace) &&
        !(rep_st->int_flags & _BCM_L2_REPLACE_INT_NO_ACTION)) {
        if ((rep_st->flags &
             (BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_MATCH_VLAN |
              BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_AGE)) == BCM_L2_REPLACE_MATCH_DEST) {
            if (rep_st->match_dest.trunk == -1) {
                /* Find out if the new data will cause parity bit change */
                if (rep_st->new_dest.trunk != -1) {
                    parity_diff =
                        rep_st->match_dest.module ^ rep_st->match_dest.port ^
                        rep_st->new_dest.trunk ^ 1;
                } else {
                    parity_diff = 
                        rep_st->match_dest.module ^ rep_st->match_dest.port ^
                        rep_st->new_dest.module ^ rep_st->new_dest.port;
                }
                parity_diff ^= parity_diff >> 4;
                parity_diff ^= parity_diff >> 2;
                parity_diff ^= parity_diff >> 1;
                bulk_replace_modport = parity_diff & 1 ? TRUE : FALSE;
            }
        }
    }

#ifdef BCM_TRIDENT2_SUPPORT
        if (SOC_IS_TD2_TT2(unit)) {
            soc_mem_lock(unit, L2mem);
        }
#endif
    if (bulk_replace_modport) {
        if (soc_feature(unit, soc_feature_l2_bulk_unified_table)) {
            rv =  _bcm_th_l2_bulk_replace_modport(unit, rep_st);
        } else {
            rv =  _bcm_td_l2_bulk_replace_modport(unit, rep_st);
        }
#ifdef BCM_TRIDENT2_SUPPORT
        if (SOC_IS_TD2_TT2(unit)) {
            soc_mem_unlock(unit, L2mem);
        }
#endif
        return rv;
    } else {
        ext_reg = INVALIDr;
        if (soc_feature(unit, soc_feature_l2_bulk_control)) {
            if (soc_feature(unit, soc_feature_l2_bulk_unified_table)) {
                rv = _bcm_th_l2_bulk_control_setup(unit, rep_st);
            } else {
#ifdef BCM_TRIDENT2_SUPPORT
                if (SOC_IS_TD2_TT2(unit) && 
                    (rep_st->flags & (BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_AGE))) {
                    rv = _bcm_td_l2_bulk_replace_delete_age(unit, rep_st);
                    soc_mem_unlock(unit, L2mem);
                    return rv;
                }
#endif
                rv = _bcm_td_l2_bulk_control_setup(unit, rep_st);
            }
            if (BCM_FAILURE(rv)) {
#ifdef BCM_TRIDENT2_SUPPORT
                if (SOC_IS_TD2_TT2(unit)) {
                    soc_mem_unlock(unit, L2mem);
                }
#endif
                return rv;
            }
            int_reg = L2_BULK_CONTROLr;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_tr_l2_ppa_setup(unit, rep_st));
#ifdef BCM_HURRICANE3_SUPPORT
            if (SOC_REG_IS_VALID(unit, PER_PORT_AGE_CONTROL_64r)) {
                int_reg = PER_PORT_AGE_CONTROL_64r;
            } else 
#endif
            {
                int_reg = PER_PORT_AGE_CONTROLr;
            }

#if defined(BCM_TRIUMPH_SUPPORT)
            if (soc_mem_is_valid(unit, EXT_L2_ENTRYm) &&
                soc_mem_index_count(unit, EXT_L2_ENTRYm) > 0) {
                /* s/w based ppa match is supported for non vfi entries. */
                if ((do_l2e_ppa_match) && (rep_st->key_type != 3)) {
                    BCM_IF_ERROR_RETURN(_bcm_tr_l2e_ppa_match(unit, rep_st));
                } else {
                    BCM_IF_ERROR_RETURN(_bcm_tr_ext_l2_ppa_setup(unit, rep_st));
                    ext_reg = ESM_PER_PORT_AGE_CONTROLr;
                }
            }
#endif	/* BCM_TRIUMPH_SUPPORT*/
        }

#ifdef PLISIM
#ifdef _KATANA_DEBUG /* BCM_KATANA_SUPPORT */
    if (SAL_BOOT_PLISIM) {
        _bcm_kt_enable_port_age_simulation(flags,rep_st);
    }
#endif
#endif
        rv = soc_l2x_port_age(unit, int_reg, ext_reg);
#ifdef BCM_TRIDENT2_SUPPORT
        if (SOC_IS_TD2_TT2(unit)) {
            soc_mem_unlock(unit, L2mem);
        }
#endif
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (rep_st->int_flags & _BCM_L2_REPLACE_INT_NO_ACTION) {
            return BCM_E_NONE;
        }

        if (!(rep_st->flags & BCM_L2_REPLACE_DELETE)) {
            return BCM_E_NONE;
        }

        if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
            return BCM_E_RESOURCE;
        }
        rv = _soc_l2x_sync_replace
            (unit, &rep_st->match_data, &rep_st->match_mask,
             rep_st->flags & BCM_L2_REPLACE_NO_CALLBACKS ?
             SOC_L2X_NO_CALLBACKS : 0);

        SOC_L2_DEL_SYNC_UNLOCK(soc);
    }

    return rv;
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_tr_l2_sw_dump
 * Purpose:
 *     Displays L2 information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_tr_l2_sw_dump(int unit)
{
    _bcm_mac_block_info_t *mbi;
     char                 pfmt[SOC_PBMP_FMT_LEN];
     int                  i;
#if defined(BCM_TRIUMPH_SUPPORT) /* BCM_TRIUMPH_SUPPORT */
    _bcm_l2_station_control_t *sc; /* Station control structure. */
    int rv;
#endif /* BCM_TRIUMPH_SUPPORT */

    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    LOG_CLI((BSL_META_U(unit,
                        "  TR L2 MAC Blocking Info -\n")));
    LOG_CLI((BSL_META_U(unit,
                        "      Number : %d\n"), _mbi_num[unit]));

    mbi = _mbi_entries[unit];
    LOG_CLI((BSL_META_U(unit,
                        "      Entries (index: pbmp-count) :\n")));
    if (mbi != NULL) {
        for (i = 0; i < _mbi_num[unit]; i++) {
            SOC_PBMP_FMT(mbi[i].mb_pbmp, pfmt);
            LOG_CLI((BSL_META_U(unit,
                                "          %5d: %s-%d\n"), i, pfmt, mbi[i].ref_count));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    LOG_CLI((BSL_META_U(unit,
                        "\n  TR L2 PPA bypass - %s\n"),
             SOC_CONTROL(unit)->l2x_ppa_bypass ? "TRUE" : "FALSE"));
#if defined(BCM_TRIUMPH_SUPPORT) /* BCM_TRIUMPH_SUPPORT */
    rv = _bcm_l2_station_control_get(unit, &sc);
    if (BCM_SUCCESS(rv)) {
        LOG_CLI((BSL_META_U(unit,
                            "\n  Station Control Info:\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    entries_free = %d, entries_free_2 = %d\n"),
                sc->entries_free, sc->entries_free_2));
        LOG_CLI((BSL_META_U(unit,
                            "    entry_count  = %d, entry_count_2  = %d\n"),
                sc->entry_count, sc->entry_count_2));
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#else /* BCM_TRX_SUPPORT */
typedef int bcm_esw_triumph_l2_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_TRX_SUPPORT */


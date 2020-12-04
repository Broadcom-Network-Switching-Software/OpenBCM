/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains VLAN definitions internal to the BCM library.
 */

#ifndef _BCM_INT_VLAN_H
#define _BCM_INT_VLAN_H

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/vlan.h>

/*
 * Define:
 *	VLAN_CHK_ID
 * Purpose:
 *	Causes a routine to return BCM_E_PARAM if the specified
 *	VLAN ID is out of range.
 */
#define VLAN_CHK_ID(unit, vid) do { \
	if (vid > BCM_VLAN_MAX) return BCM_E_PARAM; \
	} while (0)

/*
 * Define:
 *	VLAN_CHK_PRIO
 * Purpose:
 *	Causes a routine to return BCM_E_PARAM if the specified
 *	priority (802.1p CoS) is out of range.
 */

#define VLAN_CHK_PRIO(unit, prio) do { \
	if ((prio < BCM_PRIO_MIN || prio > BCM_PRIO_MAX)) return BCM_E_PARAM; \
	} while (0);

#define VLAN_CHK_ACTION(unit, action) do { \
	if (action < bcmVlanActionNone || action > bcmVlanActionCopy) return BCM_E_PARAM; \
	} while (0);
/*
  *In TH2/TH3, some fields in VLAN_TAB are moved to VLAN_2_TAB.
  *This macro is to distinguish the two tables where these fields are used.
  */
#define VLAN_TABLE(unit) \
        (SOC_MEM_IS_VALID((unit), VLAN_2_TABm) ? VLAN_2_TABm : VLAN_TABm)

/*
 * XGS3 Vlan Translate macros
 */
#ifdef BCM_XGS3_SWITCH_SUPPORT
#define BCM_VLAN_XLATE_PRIO_MASK        0x007
#define BCM_VLAN_XLATE_MODID_MASK       0x3f0
#define BCM_VLAN_XLATE_INGRESS_MASK     0x400
#define BCM_VLAN_XLATE_ADDVID_MASK      0x800

#define BCM_VLAN_XLATE_MODID_SHIFT      4

#define BCM_VLAN_XLATE_ING              0
#define BCM_VLAN_XLATE_EGR              1
#define BCM_VLAN_XLATE_DTAG             2
#endif

#define _BCM_VLAN_XLATE_DUMMY_PORT     (2)
#define BCM_VLAN_RANGE_NUM              8

/*
 * VLAN internal struct used for book keeping
 */
typedef struct vbmp_s {
    SHR_BITDCL  *w;
} vbmp_t;

/*
 * Macro :
 *      _BCM_VBMP_LOOKUP
 * Purpose:
 *      Return TRUE if a VLAN ID exists in a vbmp, FALSE otherwise
 */
#define _BCM_VBMP_LOOKUP(_bmp_, _vid_)    SHR_BITGET(((_bmp_).w), (_vid_))

/*
 * MACRO:
 *      _BCM_VBMP_INSERT
 * Purpose:
 *      Add a VLAN ID to a vbmap
 */
#define _BCM_VBMP_INSERT(_bmp_, _vid_)    SHR_BITSET(((_bmp_).w), (_vid_))

/*
 * Macro:
 *      _BCM_VBMP_REMOVE
 * Purpose:
 *      Delete a VLAN ID from a vbmp
 */
#define _BCM_VBMP_REMOVE(_bmp_, _vid_)    SHR_BITCLR(((_bmp_).w), (_vid_))

extern int _bcm_esw_vlan_stk_update(int unit, uint32 flags);
extern int _bcm_esw_vlan_flood_default_set(int unit, bcm_vlan_mcast_flood_t mode);
extern int _bcm_esw_vlan_flood_default_get(int unit, bcm_vlan_mcast_flood_t *mode);
extern int _bcm_esw_higig_flood_l2_set(int unit, bcm_vlan_mcast_flood_t mode);
extern int _bcm_esw_higig_flood_l2_get(int unit, bcm_vlan_mcast_flood_t *mode);
extern int _bcm_esw_higig_flood_l3_set(int unit, bcm_vlan_mcast_flood_t mode);
extern int _bcm_esw_higig_flood_l3_get(int unit, bcm_vlan_mcast_flood_t *mode);
extern int bcm_esw_vlan_detach(int unit);
extern int _bcm_esw_vlan_xlate_key_type_value_get(int unit, int key_type,
                                                  int *key_value);
extern int _bcm_esw_vlan_xlate_key_type_get(int unit, int key_type_value, 
                                            int *key_type);
extern int _bcm_esw_pt_vtkey_type_value_get(int unit,
                     int key_type, int *key_type_value);
extern int _bcm_esw_pt_vtkey_type_get(int unit,
                     int key_type_value, int *key_type);
extern int _bcm_esw_dvp_vtkey_type_get(int unit,
                     int key_type_value, int *key_type);
extern int _bcm_esw_dvp_vtkey_type_value_get(int unit,
                     int key_type, int *key_type_value);

#if defined(BCM_TRIUMPH3_SUPPORT)
extern int _bcm_tr3_vxlate_extd2vxlate(int unit,vlan_xlate_extd_entry_t *vxxent,
                vlan_xlate_entry_t *vxent, int use_svp);
extern int _bcm_tr3_vxlate2vxlate_extd(int unit, vlan_xlate_entry_t *vxent,
                vlan_xlate_extd_entry_t *vxxent);
#endif

#ifdef BCM_TRX_SUPPORT
/*
 * This structure is used to bookkeeping for vlan ranges
 * which are added with no association to vlan translate action.
 * This case happens when using bcm_vlan_translate_action_range_add API
 * and assigns NULL to 'action' parameter.
 */
typedef struct vlan_range_bk_s {
    bcm_vlan_t vlan_low[BCM_VLAN_RANGE_NUM]; /* vlan_low of profile entry */
} vlan_range_bk_t;

/*
 * This structure is used to keep track of ref counts for the
 * vlan_range_bk_t defined above (Vlan ranges added with no assosiaction
 * to vlan translate action).
 */
typedef struct vlan_range_bk_ref_cnt_s {
    int ref_cnt; /* Ref counts for profiles */
} vlan_range_bk_ref_cnt_t;

#endif /* BCM_TRX_SUPPORT */

/*
 * The entire vlan_info structure is protected by BCM_LOCK.
 */

typedef struct bcm_vlan_info_s {
    int                    init;      /* TRUE if VLAN module has been inited */
    bcm_vlan_t             defl;     /* Default VLAN */
    vbmp_t                 bmp;       /* Bitmap of existing VLANs */
    int                    count;     /* Number of existing VLANs */
    /* Number of vlan egress xlate entires used to blok port_class settings */
    uint16                 old_egr_xlate_cnt; 
    bcm_vlan_mcast_flood_t flood_mode;/* Default flood mode */
    uint32                 *ing_trans;  /* Ingress Vlan Translate info */
    uint32                 *egr_trans;  /* Egress Vlan Translate info */
#if defined(BCM_EASY_RELOAD_SUPPORT)
    vbmp_t                 egr_vlan_bmp; /* Bitmap of valid EGR_VLAN */
    vbmp_t                 vlan_tab_bmp; /* Bitmap of valid VLAN_TAB */
#endif
    SHR_BITDCL             *qm_bmp;   /* Bitmap of allocated queue map */
    SHR_BITDCL             *qm_it_bmp;/* Bitmap of queue map entry using inner
                                         tag */
    vbmp_t                 pre_cfg_bmp; /* per vlan pre-configuration flag */ 
    SHR_BITDCL vp_mode[_SHR_BITDCLSIZE(BCM_VLAN_COUNT)]; /* VP control mode */
    int                    vlan_auto_stack; /* True if turn on auto stack */
#ifdef BCM_TRIDENT3_SUPPORT
    int                    shared_vlan_enable; /* Shared vlan enable */
#endif
#ifdef BCM_TRX_SUPPORT
    /* VLAN ranges which are added with no association to vlan xlate action */
    vlan_range_bk_t *vlan_range_no_act_array;
    vlan_range_bk_t *vlan_range_inner_no_act_array;
    /* Ref counts assosiated with the VLAN ranges */
    vlan_range_bk_ref_cnt_t *vlan_range_no_act_ref_cnt_array;
    vlan_range_bk_ref_cnt_t *vlan_range_inner_no_act_ref_cnt_array;
#endif /* BCM_TRX_SUPPORT */
} bcm_vlan_info_t;

extern bcm_vlan_info_t vlan_info[BCM_MAX_NUM_UNITS];

typedef struct _bcm_vlan_translate_data_s{
    bcm_port_t      port;
    bcm_vlan_t      old_vlan;
    bcm_vlan_t      *new_vlan;
    int             *prio;
}_bcm_vlan_translate_data_t;

/* Key types for VP VLAN/VFI Mermbership */
typedef enum _bcm_vp_vlan_mbrship_key_type_t {
    _bcm_vp_vlan_mbrship_vp_vlan_type = 0,
    _bcm_vp_vlan_mbrship_glp_vlan_type,
    _bcm_vp_vlan_mbrship_vp_vfi_type,
    _bcm_vp_vlan_mbrship_glp_vfi_type
} _bcm_vp_vlan_mbrship_key_type_e;

/*
 * Internal key type for egr vxlate lookup.
 * Maps to HW encoding of VT_ENABLE field in EGR_VLAN_CONTROL mem.
 * Not applicable for virtual port (VP) based lookups
 */
typedef enum _bcm_evxlt_lookup_key_e {
    EVXLT_KEY_TYPE_NOOP = 0,
    EVXLT_KEY_TYPE_PORT_GROUP_VLAN_DOUBLE = 1,
    EVXLT_KEY_TYPE_PORT_VLAN_DOUBLE = 2
} _bcm_evxlt_lookup_key_t;

/*
 * Vlan Translate support routines
 */

#define BCM_VTCACHE_VALID_SET(_c, _valid)       _c |= ((_valid & 1) << 31)
#define BCM_VTCACHE_ADD_SET(_c, _add)           _c |= ((_add & 1) << 30)
#define BCM_VTCACHE_PORT_SET(_c, _port)         _c |= ((_port & 0xff) << 16)
#define BCM_VTCACHE_VID_SET(_c, _vid)           _c |= ((_vid & 0xffff) << 0)

#define BCM_VTCACHE_VALID_GET(_c)               ((_c >> 31) & 1)
#define BCM_VTCACHE_ADD_GET(_c)                 ((_c >> 30) & 1)
#define BCM_VTCACHE_PORT_GET(_c)                ((_c >> 16) & 0xff)
#define BCM_VTCACHE_VID_GET(_c)                 ((_c >> 0) & 0xffff)

#define VLAN_MEM_CHUNKS_DEFAULT                 256
#define ING_ACTION_PROFILE_DEFAULT                0

typedef struct _translate_action_range_traverse_cb_s {
    bcm_vlan_translate_action_range_traverse_cb usr_cb;
} _translate_action_range_traverse_cb_t;

typedef struct _translate_range_traverse_cb_s {
    bcm_vlan_translate_range_traverse_cb usr_cb;
} _translate_range_traverse_cb_t;

typedef struct _dtag_range_traverse_cb_s {
    bcm_vlan_dtag_range_traverse_cb usr_cb;
} _dtag_range_traverse_cb_t;

typedef struct _translate_action_traverse_cb_s {
    bcm_vlan_translate_action_traverse_cb usr_cb;
} _translate_action_traverse_cb_t;

typedef struct _translate_egress_action_traverse_cb_s {
    bcm_vlan_translate_egress_action_traverse_cb usr_cb;
} _translate_egress_action_traverse_cb_t;

typedef struct _translate_egress_action_extended_traverse_cb_s {
    bcm_vlan_translate_egress_action_extended_traverse_cb usr_cb;
} _translate_egress_action_extended_traverse_cb_t;

typedef struct _translate_traverse_cb_s {
    bcm_vlan_translate_traverse_cb usr_cb;
} _translate_traverse_cb_t;

typedef struct _translate_egress_traverse_cb_s {
    bcm_vlan_translate_egress_traverse_cb usr_cb;
} _translate_egress_traverse_cb_t;

typedef struct _dtag_traverse_cb_s {
    bcm_vlan_dtag_traverse_cb usr_cb;
} _dtag_traverse_cb_t;


typedef int (*_bcm_vlan_translate_traverse_int_cb)( int unit, void *trv_info, 
                                                    int *stop);

typedef struct _bcm_vlan_translate_traverse_s {
    _bcm_vlan_translate_traverse_int_cb int_cb;
    void                                *user_cb_st;
    void                                *user_data;
    bcm_vlan_action_set_t               *action;
    bcm_gport_t                         gport;
    uint32                              key_type;
    int                                 port_class;
    bcm_vlan_t                          outer_vlan;
    bcm_vlan_t                          inner_vlan;
    bcm_vlan_t                          outer_vlan_high;
    bcm_vlan_t                          inner_vlan_high;
}_bcm_vlan_translate_traverse_t;

extern int _bcm_esw_vlan_translate_action_traverse_int_cb(int unit, 
                                                         void* trv_info,
                                                          int *stop);
extern int _bcm_esw_vlan_translate_egress_action_traverse_int_cb(int unit, 
                                                                void* trv_info,
                                                                 int *stop);
extern int _bcm_esw_vlan_translate_traverse_int_cb(int unit, void* trv_info, 
                                                   int *stop);
extern int _bcm_esw_vlan_translate_egress_traverse_int_cb(int unit, 
                                                          void* trv_info,
                                                          int *stop);
extern int _bcm_esw_vlan_dtag_traverse_int_cb(int unit, void* trv_info, 
                                              int *stop);
extern int _bcm_esw_vlan_translate_traverse_mem(int unit, soc_mem_t mem, 
                                   _bcm_vlan_translate_traverse_t *trvs_info);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_vlan_cleanup(int unit);
#else
#define _bcm_vlan_cleanup(u)   (BCM_E_NONE)
#endif /* BCM_WARM_BOOT_SUPPORT */


extern int _bcm_esw_vlan_untag_update(int unit, bcm_port_t port, int untag);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_vlan_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_vlan_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
extern int _bcm_vlan_valid_check(int unit, int table, vlan_tab_entry_t *vt, 
                                 bcm_vlan_t vid); 
#endif

#if defined(BCM_EASY_RELOAD_SUPPORT)
int _bcm_vlan_valid_set(int unit, int table, bcm_vlan_t vid, int val);
#endif

extern int _bcm_esw_vlan_port_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int _bcm_esw_vlan_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int _bcm_esw_vlan_port_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp);

extern int _bcm_esw_vlan_system_reserved_get(int unit, int *arg);
extern int _bcm_esw_vlan_system_reserved_set(int unit, int arg);

extern int _bcm_vlan_count_get(int unit, int *arg);
extern int _bcm_vbmp_init(vbmp_t *bmp);
extern int _bcm_vbmp_destroy(vbmp_t *bmp);

#if defined(BCM_TRX_SUPPORT)
extern int _bcm_trx_vlan_action_profile_ref_count_get(int unit, int index, int *ref_count);
extern int _bcm_trx_vlan_action_profile_entry_add(int unit,
                                      bcm_vlan_action_set_t *action,
                                      uint32 *index);
extern int _bcm_trx_vlan_action_profile_entry_delete(int unit, uint32 index);
extern int _bcm_trx_vlan_action_verify(int unit, bcm_vlan_action_set_t *action);
#endif /* BCM_TRX_SUPPORT */

#endif	/* !_BCM_INT_VLAN_H */

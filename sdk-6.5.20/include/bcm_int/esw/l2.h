/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_INT_L2_H
#define _BCM_INT_L2_H

#include <bcm/types.h>
#include <bcm/l2.h>

#include <sal/core/sync.h>

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_l2_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */


#define     L2_MEM_CHUNKS_DEFAULT   100
typedef int (*_bcm_l2_traverse_int_cb)(int unit, void *trav_st);

typedef struct _bcm_l2_traverse_s {
    uint32                      *data;      /* L2 Entry */
    soc_mem_t                   mem;        /* Traversed memory */
    int                         mem_idx;    /* Index of currently read entry */
    bcm_l2_traverse_cb          user_cb;    /* User callback function */
    void                        *user_data; /* Data provided by the user, cookie */
    _bcm_l2_traverse_int_cb     int_cb;     /* Internal callback function */
}_bcm_l2_traverse_t;

extern int _bcm_esw_l2_traverse(int unit, _bcm_l2_traverse_t *trav_st);

typedef struct _bcm_l2_replace_dest_s {
    bcm_module_t        module;
    bcm_port_t          port;
    bcm_trunk_t         trunk;
    int                 vp;
} _bcm_l2_replace_dest_t;

/* _bcm_l2_replaces_t.int_flags (internal flags) */
#define _BCM_L2_REPLACE_INT_NO_ACTION               0x0001
#define _BCM_L2_REPLACE_INT_MATCH_DISCARD_SRC       0x0002
#define _BCM_L2_REPLACE_INT_MATCH_SRC_HIT           0x0004
#define _BCM_L2_REPLACE_INT_MATCH_DES_HIT           0x0008
#define _BCM_L2_REPLACE_INT_MATCH_NATIVE            0x0010
#define _BCM_L2_REPLACE_INT_MATCH_ONLY_STATIC       0x0020
#define _BCM_L2_REPLACE_IGNORE_PORTID               0x0040
#define _BCM_L2_REPLACE_INT_MATCH_MESH              0x0080

typedef struct _bcm_l2_replace_s {
    uint32              flags;           /* BCM_L2_REPLACE_XXX */
    uint32              int_flags;       /* _BCM_L2_REPLACE_INT_XXX */
    uint32              key_l2_flags;    /* bcm_l2_addr_t.flags */
    uint32              key_l2_flags2;   /* bcm_l2_addr_t.flags2 */
    int                 key_type;
    bcm_mac_t           key_mac;
    bcm_vlan_t          key_vlan;
    int                 key_vfi;
    int                 key_class_id;
    int                 key_int_pri;
    _bcm_l2_replace_dest_t match_dest;
    _bcm_l2_replace_dest_t new_dest;
    l2x_entry_t         match_data;
    l2x_entry_t         match_mask;
    l2x_entry_t         new_data;
    l2x_entry_t         new_mask;
    bcm_l2_traverse_cb  trav_fn;
    void               *user_data;
    uint32              view_id;         /* flex view identifier */
} _bcm_l2_replace_t;

typedef struct _bcm_l2_match_s {
    bcm_l2_addr_t *match_addr; 
    bcm_l2_addr_t *addr_mask; 
    bcm_l2_traverse_cb trav_fn; 
    void *user_data;
} _bcm_l2_match_t;


#define _BCM_L2_MATCH_ENTRY_BUF_SIZE      (16 * 1024)
#define _BCM_L2_MATCH_ENTRY_BUF_COUNT     32
typedef struct _bcm_l2_match_ctrl_s {
    sal_sem_t sem;
    int entry_wr_idx;
    l2x_entry_t *l2x_entry_buf[_BCM_L2_MATCH_ENTRY_BUF_COUNT];
    int preserved;
} _bcm_l2_match_ctrl_t;

typedef struct  _bcm_l2_gport_params_s {
    int     param0;
    int     param1;
    uint32  type;
}_bcm_l2_gport_params_t;

extern int _l2_init[BCM_MAX_NUM_UNITS];
extern  _bcm_l2_match_ctrl_t *_bcm_l2_match_ctrl[BCM_MAX_NUM_UNITS];
/*
 * Define:
 *	L2_INIT
 * Purpose:
 *	Causes a routine to return BCM_E_INIT (or some other
 *	error) if L2 software module is not yet initialized.
 */

#define L2_INIT(unit) do { \
	if (_l2_init[unit] < 0) return _l2_init[unit]; \
	if (_l2_init[unit] == 0) return BCM_E_INIT; \
	} while (0);

extern int _bcm_esw_l2_gport_parse(int unit, bcm_l2_addr_t *l2addr, 
                                   _bcm_l2_gport_params_t *params);
extern int _bcm_esw_l2_gport_construct(int unit, bcm_l2_addr_t *l2addr, 
                                       _bcm_l2_gport_params_t *params);

extern int _bcm_get_op_from_flags(uint32 flags, uint32 *op, uint32 *sync_op);
extern int _bcm_esw_l2_from_l2x(int unit, soc_mem_t mem, bcm_l2_addr_t *l2addr,
                                uint32 *l2_entry);
extern void _bcm_l2_register_callback(int unit, int flags, l2x_entry_t *entry_del,
                                      l2x_entry_t *entry_add, void *fn_data);

/* extended aging per_port_age_control mode field values */
#define	XGS_PPAMODE_PORTMOD		            0x0
#define	XGS_PPAMODE_VLAN		            0x1
#define	XGS_PPAMODE_PORTMOD_VLAN	        0x2
#define	XGS_PPAMODE_RSVD		            0x3
#define	XGS_PPAMODE_PORTMOD_REPLACE		    0x4	
#define	XGS_PPAMODE_VLAN_REPLACE		    0x5	
#define	XGS_PPAMODE_PORTMOD_VLAN_REPLACE	0x6	

#define BCMSIM_L2XMSG_INTERVAL 60000000
#ifdef BCM_TOMAHAWK3_SUPPORT
#define BCMSIM_L2XLRN_INTERVAL 1000000
#define BCM_L2XLRN_INTERVAL_DEFAULT 250000
#endif /* BCM_TOMAHAWK3_SUPPORT */

/* type field [14:13] for destination type in L2_ENTRY/L2_USER_ENTRY*/
#define _BCM_L2_DEST_TYPE_MASK          (3U << 13)
#define _BCM_L2_DEST_TYPE_TRUNK         0        /* type field [14:13] = 0 */

#define _BCM_L2_DEST_TYPE_NHI           (3U << 13)

#define _BCM_L2_DEST_STATION_HIT        (1U << 13)
#define _BCM_L2_DEST_STATION_IPV4       (1U << 3)
#define _BCM_L2_DEST_STATION_IPV6       (1U << 4)
#define _BCM_L2_DEST_STATION_ARP_RARP   (1U << 5)
#define _BCM_L2_DEST_STATION_OAM        (1U << 6)
#define _BCM_L2_DEST_STATION_FCOE       (1U << 7)
#define _BCM_L2_DEST_STATION_IPV4_MCAST (1U << 8)
#define _BCM_L2_DEST_STATION_IPV6_MCAST (1U << 9)

/* L2 Learn class defines */
#define _BCM_L2_LEARN_CLASSID_MIN      0
#define _BCM_L2_LEARN_CLASSID_MAX      3
#define BCM_L2_LEARN_CLASSID_VALID(_cid_) do { \
        if ((_cid_ < _BCM_L2_LEARN_CLASSID_MIN) || \
        (_cid_ > _BCM_L2_LEARN_CLASSID_MAX)) { return BCM_E_PARAM; } \
        } while(0);

#define BCM_L2_PRE_SET(_flags_) \
    (((_flags_) & BCM_L2_SETPRI) ? TRUE : FALSE)

#if defined(BCM_TRIUMPH_SUPPORT) /* BCM_TRIUMPH_SUPPORT */
typedef struct _bcm_l2_station_entry_s {
    int                     sid;        /* Entry identifier.             */
    int                     prio;       /* Entry priority.               */
    uint32                  hw_index;   /* Entry hardware index.         */
    uint32                  flags;      /* Entry flags.                  */
    uint32                  *tcam_ent;  /* TCAM entry.                   */
} _bcm_l2_station_entry_t;

typedef struct _bcm_l2_station_control_s {
    sal_mutex_t             sc_lock;
    _bcm_l2_station_entry_t **entry_arr;    /* L2 station entry array.  */
    int                     entries_total;  /* Total number of entries. */
    int                     entries_free;   /* Count of unused entries. */
    int                     entry_count;    /* Count of used entries.   */
    _bcm_l2_station_entry_t **entry_arr_2;  /* L2 station entry array.  */
    int                     entries_total_2;/* Total number of entries. */
    int                     entries_free_2; /* Count of unused entries. */
    int                     entry_count_2;  /* Count of used entries.   */
#ifdef BCM_KATANA2_SUPPORT
    int                     port_entries_total; /* Count of total Port MAC 
                                                                entries */
    int                     port_entries_free; /* Count of total PORT MAC
                                                                 entries */
#endif    
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    int                     olp_entries_total;  /* Count of total OLP MAC 
                                                                entries */
    int                     olp_entries_free;   /* Count of total OLP MAC 
                                                                entries */
#endif
    uint32                  last_allocated_sid;
    uint32                  last_allocated_sid_2; /* for MY_STATION_TCAM_2 */
} _bcm_l2_station_control_t;

/* L2 BULK unified table indexes */
#define _BCM_L2_BULK_MATCH_DATA_INX   0
#define _BCM_L2_BULK_MATCH_MASK_INX   1
#define _BCM_L2_BULK_REPLACE_DATA_INX 2
#define _BCM_L2_BULK_REPLACE_MASK_INX 3

#endif /* !BCM_TRIUMPH_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT)

    typedef struct l2_count_s {
        int l2_entry_1;
        int l2_entry_2;
        int ext_l2_entry_1;
        int ext_l2_entry_2;
    } l2_count_t;

    typedef struct l2_count_data_s {
        uint32     flags;
        l2_count_t l2_count;
    } l2_count_data_t;

#define L2_DUMP_COUNT 1<<0

extern l2_count_data_t _l2_addr_counts;

#endif

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_l2_wb_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(INCLUDE_L3)
extern int _bcm_td3_l2_change_fields_add(int unit,bcm_l2_change_fields_t *l2_info);
extern int _bcm_td3_l2_change_fields_egress_key_set(int unit, soc_mem_t mem, bcm_l2_change_fields_t *l2_info, uint32 *entry);
extern int _bcm_td3_l2_change_fields_egress_entry_get(int unit, soc_mem_t mem, uint32* entry, bcm_l2_change_fields_t *l2_info);
extern int _bcm_td3_change_l2_fields_entry_traverse(int unit, soc_mem_t mem, bcm_l2_change_fields_traverse_cb cb, void *user_data);
#endif  /* INCLUDE_L3 */
#endif	/* !_BCM_INT_L2_H */

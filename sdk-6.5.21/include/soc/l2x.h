/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l2x.h
 * Purpose:     Draco L2X hardware table manipulation support
 */

#ifndef _L2X_H_
#define _L2X_H_

#include <sal/core/time.h>
#include <shared/avl.h>
#include <shared/bitop.h>
#include <soc/macipadr.h>
#include <soc/hash.h>

#ifdef BCM_XGS_SWITCH_SUPPORT

extern int _soc_l2x_cml_struct_alloc(int unit);
extern void _soc_l2x_cml_struct_free(int unit);
extern int soc_l2x_attach(int unit);
extern int soc_l2x_detach(int unit);
extern int soc_l2x_init(int unit);

extern int soc_l2x_entry_valid(int unit, l2x_entry_t *entry);

extern int soc_l2x_entry_compare_key(void *user_data,
                                     shr_avl_datum_t *datum1,
                                     shr_avl_datum_t *datum2);
extern int soc_l2x_entry_compare_all(void *user_data,
                                     shr_avl_datum_t *datum1,
                                     shr_avl_datum_t *datum2);

extern int soc_l2x_entry_dump(void *user_data,
                              shr_avl_datum_t *datum,
                              void *extra_data);

/*
 * L2 hardware insert, delete, and lookup
 */

extern int soc_l2x_insert(int unit, l2x_entry_t *entry);
extern int soc_l2x_delete(int unit, l2x_entry_t *entry);
extern int soc_l2x_delete_all(int unit, int static_too);
extern int soc_l2x_port_age(int unit, soc_reg_t reg0, soc_reg_t reg1);
extern int soc_l2x_lookup(int unit,
                          l2x_entry_t *key, l2x_entry_t *result,
                          int *index);

#define SOC_L2X_INC_STATIC          0x00000001
#define SOC_L2X_NO_CALLBACKS        0x00000002
#define SOC_L2X_EXT_MEM             0x00000004 

/* Modes by which delete can take place */
#define SOC_L2X_NO_DEL                0  
#define SOC_L2X_PORTMOD_DEL           1
#define SOC_L2X_VLAN_DEL              2
#define SOC_L2X_MAC_DEL               3
#define SOC_L2X_PORTMOD_VLAN_DEL      4
#define SOC_L2X_ALL_DEL               5
#define SOC_L2X_TRUNK_DEL             6
#define SOC_L2X_VFI_DEL               7
#define SOC_L2X_TRUNK_VLAN_DEL        8

#define SOC_L2X_FROZEN_WITH_LOCK      0
#define SOC_L2X_FROZEN_WITHOUT_LOCK   1
#define SOC_L2X_FROZEN                2

#define SOC_L2X_FREEZE_MODE_PORT_RECONFIGURE    0
#define SOC_L2X_FREEZE_MODE_GLOBAL_OVERRIDE     1

extern int soc_l2x_freeze_mode_set(int unit, int mode);
extern int soc_l2x_freeze_mode_get(int unit, int *mode);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_l2x_wb_scache_size_get(int unit, int *req_scache_size);
extern int soc_l2x_scache_sync(int unit, uint8 **scache_ptr);
extern int soc_l2x_reinit(int unit, uint8 **scache_ptr);
#endif /* BCM_WARM_BOOT_SUPPORT */
extern int soc_l2x_sync_delete(int unit, uint32 *del_entry, int index, uint32 flags);
extern int _soc_l2x_sync_delete_by(int, uint32, uint32, uint16, uint32, int,
                                   uint32, uint32);
extern int _soc_l2x_sync_replace(int unit, l2x_entry_t *l2x_match_data,
                                 l2x_entry_t *l2x_match_mask, uint32 flags);
#ifdef BCM_TSN_SUPPORT
extern int soc_l2x_sr_ref_sync(int unit, int l2_index);
#endif /* BCM_TSN_SUPPORT */

#define SOC_L2X_ENTRY_OVERFLOW                  0x00000001
#define SOC_L2X_ENTRY_DUMMY                     0x00000002
#define SOC_L2X_ENTRY_NO_ACTION                 0x00000004
#define SOC_L2X_ENTRY_TSN_SR_FLOWSET_REF_CNT    0x00000008
#define SOC_L2X_ENTRY_DELETE                    0x00000010
#define SOC_L2X_ENTRY_AGE                       0x00000020
#define SOC_L2X_ENTRY_INSERT                    0x00000040
#define SOC_L2X_ENTRY_LEARN                     0x00000080

#ifdef BCM_TRIUMPH3_SUPPORT
/****************************************************************
 *
 * L2 replace structure
 */
typedef struct _soc_tr3_l2_replace_dest_s {
    soc_module_t module;
    soc_port_t   port;
    trunk_id_t   trunk;
    int          vp;
} _soc_tr3_l2_replace_dest_t;

typedef struct _soc_tr3_l2_replace_s {
    uint32          flags; /* BCM_L2_REPLACE_XXX */
    int             key_type;
    int             ext_key_type;
    sal_mac_addr_t  key_mac;
    vlan_id_t       key_vlan;
    int             key_vfi;
    _soc_tr3_l2_replace_dest_t match_dest;
    _soc_tr3_l2_replace_dest_t new_dest;
    l2_bulk_entry_t match_data1;
    l2_bulk_entry_t match_mask1;
    l2_bulk_entry_t new_data1;
    l2_bulk_entry_t new_mask1;
    l2_bulk_entry_t match_data2;
    l2_bulk_entry_t match_mask2;
    l2_bulk_entry_t new_data2;
    l2_bulk_entry_t new_mask2;
    l2_bulk_entry_t ext_match_data1;
    l2_bulk_entry_t ext_match_mask1;
    l2_bulk_entry_t ext_new_data1;
    l2_bulk_entry_t ext_new_mask1;
    l2_bulk_entry_t ext_match_data2;
    l2_bulk_entry_t ext_match_mask2;
    l2_bulk_entry_t ext_new_data2;
    l2_bulk_entry_t ext_new_mask2;
} _soc_tr3_l2_replace_t;

extern int soc_tr3_l2x_sync_delete(int unit, soc_mem_t mem, uint32 *del_entry, 
                                   int index, uint32 flags);
extern int _soc_tr3_l2x_sync_delete_by(int, uint32, uint32, uint16, uint32, int,
                                       uint32, uint32);
extern int _soc_tr3_l2x_sync_replace(int unit, _soc_tr3_l2_replace_t *rep_st,
                                     uint32 flags);
#endif
extern int soc_l2x_sync_delete_by_port(int unit, int mod, 
                                       soc_port_t port, uint32 flags);
extern int soc_l2x_sync_delete_by_trunk(int unit, 
                                        int tid, uint32 flags);
extern int soc_er_l2x_entries_external(int unit);
extern int soc_er_l2x_entries_internal(int unit);
extern int soc_er_l2x_entries_overflow(int unit);

extern int soc_l2x_mode_cfg_get(int unit);
/*
 * Registration to receive inserts, deletes, and updates.
 *	For inserts, entry_del is NULL.
 *	For deletes, entry_ins is NULL.
 *	For updates, neither is NULL.
 */

typedef void (*soc_l2x_cb_fn)(int unit,
                              int flags,
                              l2x_entry_t *entry_del,
                              l2x_entry_t *entry_add,
                              void *fn_data);
#ifdef BCM_TRIDENT3_SUPPORT
typedef int (*soc_l2x_cml_cb_fn)(int unit, soc_port_t port, int save,
                                 int *save_cml, int *save_cml_move);
extern int soc_l2x_cml_register(int unit, soc_l2x_cml_cb_fn fn);
#endif
extern int soc_l2x_register(int unit, soc_l2x_cb_fn fn, void *fn_data);
extern int soc_l2x_unregister(int unit, soc_l2x_cb_fn fn, void *fn_data);

#define soc_l2x_unregister_all(unit) \
        soc_l2x_unregister((unit), NULL, NULL)

extern void soc_l2x_callback(int unit,
                             int flags,
                             l2x_entry_t *entry_del,
                             l2x_entry_t *entry_add);

/* 
 * TR3 style l2 entry update callback structure and routine
 */
typedef union l2_entry_u {
    uint32 words[SOC_MAX_MEM_WORDS];
    ext_l2_entry_1_entry_t ext_l2_entry_1;
    ext_l2_entry_2_entry_t ext_l2_entry_2;
    l2_entry_1_entry_t l2_entry_1;
    l2_entry_2_entry_t l2_entry_2;
} l2_combo_entry_t;

typedef void (*soc_l2_entry_cb_fn)(int unit, uint32 flags,
                                   soc_mem_t mem_type,
                                   l2_combo_entry_t *entry_del,
                                   l2_combo_entry_t *entry_add, 
                                   void *fn_data);
extern int soc_l2_entry_register(int unit, soc_l2_entry_cb_fn, void* fn_data);
extern int soc_l2_entry_unregister(int unit, soc_l2_entry_cb_fn, void* fn_data);

/*
 * L2 miscellaneous functions
 */
extern void soc_age_timer_cache_set(int unit, int age_seconds, int enabled);
extern void soc_age_timer_cache_get(int unit,int *age_seconds,int *enabled);
extern int  soc_l2x_selective_freeze(int unit, int hw_freeze);
extern int  soc_l2x_selective_thaw(int unit, int hw_thaw);
extern int soc_l2x_freeze(int unit);
extern int soc_l2x_is_frozen(int unit, int hw_frozen, int *frozen);
extern int soc_l2x_thaw(int unit);
extern int soc_l2x_entries(int unit);	/* Warning: very slow */
extern int soc_l2x_hash(int unit, l2x_entry_t *entry);

extern int soc_l2x_frozen_cml_set(int unit, soc_port_t port, int cml, int cml_move,
				  int *repl_cml, int *repl_cml_move);
extern int soc_l2x_frozen_cml_get(int unit, soc_port_t port, int *cml, int *cml_move);
extern int _soc_l2x_frozen_cml_save(int unit);
extern int _soc_l2x_frozen_cml_restore(int unit);
extern void soc_l2x_cml_vp_bitmap_set(int unit, SHR_BITDCL *vp_bitmap);

extern void soc_tr3_l2_overflow_interrupt_handler(int unit);
extern int soc_tr3_l2_overflow_start(int unit);
extern int soc_tr3_l2_overflow_stop(int unit);
extern int soc_tr3_l2_overflow_entry_get(int unit, l2_combo_entry_t *entry,
                                         soc_mem_t *mem_type);

extern void soc_l2_entry_callback(int unit, uint32 flags, soc_mem_t mem_type, 
                      l2_combo_entry_t *entry_del, l2_combo_entry_t *entry_add);

extern void soc_td2_l2_overflow_interrupt_handler(int unit);
extern int soc_td2_l2_overflow_start(int unit);
extern int soc_td2_l2_overflow_stop(int unit);

/*
 * L2X Thread
 */

#define SOC_L2X_INTERVAL_DEFAULT 3000000

extern int soc_l2x_start(int unit, uint32 flags, sal_usecs_t interval);
extern int soc_l2x_running(int unit, uint32 *flags, sal_usecs_t *interval);
extern int soc_l2x_stop(int unit);
extern int soc_td2_l2_bulk_age_start(int unit, int interval);
extern int soc_td2_l2_bulk_age_stop(int unit);

/*
 * L2 software-based sanity routines
 */

#define SOC_L2X_BUCKET_SIZE	8
extern int soc_l2x_software_hash(int unit, int hash_select, 
                                 l2x_entry_t *entry);

/*
 * Debugging features - for diagnostic purposes only
 */

extern void soc_l2x_key_dump(int unit, char *pfx,
                             l2x_entry_t *entry, char *sfx);

#endif	/* BCM_XGS_SWITCH_SUPPORT */

#ifdef	BCM_FIREBOLT_SUPPORT

extern int soc_fb_l2x_bank_insert(int unit, uint8 banks, l2x_entry_t *entry);
extern int soc_fb_l2x_bank_delete(int unit, uint8 banks, l2x_entry_t *entry);
extern int soc_fb_l2x_bank_lookup(int unit, uint8 banks, l2x_entry_t *key,
                                  l2x_entry_t *result, int *index_ptr);

extern int soc_fb_l2x_insert(int unit, l2x_entry_t *entry) ;
extern int soc_fb_l2x_delete(int unit, l2x_entry_t *entry);
extern int soc_fb_l2x_lookup(int unit, l2x_entry_t *key,
                             l2x_entry_t *result, int *index_ptr);
extern int soc_fb_l2x_delete_all(int unit);

/*
 * We must use SOC_MEM_INFO here, because we may redefine the
 * index_max due to configuration, but the SCHAN msg uses the maximum size.
 */
#define SOC_L2X_OP_FAIL_POS(unit) \
        ((_shr_popcount(SOC_MEM_INFO(unit, L2Xm).index_max) + \
            soc_mem_entry_bits(unit, L2Xm)) %32)

#endif	/* BCM_FIREBOLT_SUPPORT */


#define SOC_L2X_MEM_LOCK(unit)\
    if (soc_feature(unit, soc_feature_ism_memory)) {\
         soc_mem_lock(unit, L2_ENTRY_1m);\
         soc_mem_lock(unit, L2_ENTRY_2m);\
    } else { soc_mem_lock(unit, L2Xm); }
        
#define SOC_L2X_MEM_UNLOCK(unit)\
    if (soc_feature(unit, soc_feature_ism_memory)) {\
         soc_mem_unlock(unit, L2_ENTRY_2m);\
         soc_mem_unlock(unit, L2_ENTRY_1m);\
    } else { soc_mem_unlock(unit, L2Xm); }

#define SOC_EXT_L2X_MEM_LOCK(unit)\
    if (soc_feature(unit, soc_feature_esm_support)) {\
        if (soc_feature(unit, soc_feature_ism_memory)) {\
            soc_mem_lock(unit, EXT_L2_ENTRY_1m);\
            soc_mem_lock(unit, EXT_L2_ENTRY_2m);\
        }\
    }

#define SOC_EXT_L2X_MEM_UNLOCK(unit)\
    if (soc_feature(unit, soc_feature_esm_support)) {\
        if (soc_feature(unit, soc_feature_ism_memory)) {\
            soc_mem_unlock(unit, EXT_L2_ENTRY_2m);\
            soc_mem_unlock(unit, EXT_L2_ENTRY_1m);\
        }\
    }

/* The overflow indicator of the index of L2_ENTRY */
#define SOC_L2X_GEN_RESP_INDEX_L2_OVERFLOW(unit) \
    ((SOC_IS_GREYHOUND2(unit)) ? ((soc_feature(unit, soc_feature_fl)) ? 0x4000 : 0x8000) : 0x4000)


/*
 * L2 Mod Thread
 */

typedef enum l2x_mode_e {
    L2MODE_POLL,
    L2MODE_FIFO
} l2x_mode_t;

#define L2MOD_PASS_CPU  0x01

#if defined(BCM_TOMAHAWK3_SUPPORT)
/* Defines if L2 learning happens through interrupt mechanism provided by learn
 * cache, or polling is used to get entries from learn cache (to learn)
 */
typedef enum soc_th3_l2x_lrn_mode_e {
    L2_LRN_MODE_POLL = 1,
    L2_LRN_MODE_INTR = 2
} soc_th3_l2x_lrn_mode_t;
#endif /* BCM_TOMAHAWK3_SUPPORT */

extern int soc_l2mod_start(int unit, uint32 flags, sal_usecs_t interval);
extern int soc_l2mod_running(int unit, uint32 *flags, sal_usecs_t *interval);
extern int soc_l2mod_stop(int unit);

#ifdef BCM_TRIDENT2_SUPPORT
extern int soc_l2x_cml_override_is_enabled(int unit);
extern void _soc_td2_l2mod_fifo_process(int unit, uint32 flags,
                                        l2_mod_fifo_entry_t *entry);
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
extern void _soc_kt_l2mod_fifo_process(int unit, uint32 flags,
        l2_mod_fifo_entry_t *entry);
#endif
#ifdef BCM_HURRICANE2_SUPPORT
extern void _soc_hu2_l2mod_fifo_process(int unit, uint32 flags,
        l2_mod_fifo_entry_t *entry);
#endif /* BCM_HURRICANE2_SUPPORT */
#if defined(BCM_TOMAHAWK3_SUPPORT)
void soc_th3_l2x_reset_freeze_state(int unit);
#endif /* BCM_TOMAHAWK3_SUPPORT */

#endif	/* !_L2X_H_ */

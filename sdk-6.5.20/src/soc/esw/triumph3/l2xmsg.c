/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:	l2xmsg.c
 * Purpose:	Provide a reliable stream of L2 insert/delete messages.
 *
 * This module monitors the L2X table for changes and performs callbacks
 * for each insert, delete, or port movement that is detected.
 *
 * There is a time lag from the actual table change to the callback
 * because the l2xmsg task scans the L2X table only periodically.
 */

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/time.h>
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/l2x.h>
#include <soc/ism_hash.h>
#include <soc/triumph3.h>

#ifdef BCM_TRIUMPH3_SUPPORT

#define L2X_ENTRY_EQL(unit, e1, e2, hit) \
    (!_soc_mem_cmp_tr3_l2x_sync(unit, (void *) e1, (void *) e2, hit))
#define EXT_L2X_1_ENTRY_EQL(unit, e1, e2, hit) \
    (!_soc_mem_cmp_tr3_ext_l2x_1_sync(unit, (void *) e1, (void *) e2, hit))
#define EXT_L2X_2_ENTRY_EQL(unit, e1, e2, hit) \
    (!_soc_mem_cmp_tr3_ext_l2x_2_sync(unit, (void *) e1, (void *) e2, hit))

#define L2X_IS_VALID_ENTRY(_u, _mem, _e, bitf)  \
           soc_mem_field32_get((_u), (_mem), (_e), bitf)


STATIC void _soc_tr3_l2x_thread(void *unit_vp);

#define SOC_MEM_L2_INT_LOCK(unit) \
    soc_mem_lock(unit, L2_ENTRY_1m); \
    soc_mem_lock(unit, L2_ENTRY_2m);

#define SOC_MEM_L2_INT_UNLOCK(unit) \
    soc_mem_unlock(unit, L2_ENTRY_2m); \
    soc_mem_unlock(unit, L2_ENTRY_1m);

#define SOC_MEM_L2_EXT_1_LOCK(unit) \
    soc_mem_lock(unit, EXT_L2_ENTRY_1m);

#define SOC_MEM_L2_EXT_1_UNLOCK(unit) \
    soc_mem_unlock(unit, EXT_L2_ENTRY_1m);

#define SOC_MEM_L2_EXT_2_LOCK(unit) \
    soc_mem_lock(unit, EXT_L2_ENTRY_2m);

#define SOC_MEM_L2_EXT_2_UNLOCK(unit) \
    soc_mem_unlock(unit, EXT_L2_ENTRY_2m);

#define SOC_TR3_L2X_BUCKET_SIZE 4

/****************************************************************************
 *
 * L2X Message Registration
 */

#define L2X_CB_MAX 3

typedef struct l2x_cb_entry_s {
    soc_l2_entry_cb_fn fn;
    void               *fn_data;
} l2_entry_cb_entry_t;

typedef struct l2x_data_s {
    uint8          enabled; /* Set to true if a L2 data store is being used, 
                               applicable for POLL mode */
    uint8          int_avail;
    uint8          ext1_avail;
    uint8          ext2_avail;
    l2_entry_cb_entry_t cb[L2X_CB_MAX];
    int            cb_count;
    /* Internal ISM based memory: We manage type 1 and 2 using a single set */
    int            entry_bytes_int;
    int            entry_words_int;
    uint32         *shadow_tab_int;
    SHR_BITDCL     *del_map_int;
    SHR_BITDCL     *cb_map_int;
    /* External ESM based memories: Two sets for type 1 and type 2 respectively */
    int            entry_bytes_ext1;
    int            entry_words_ext1;
    uint32         *shadow_tab_ext1;
    SHR_BITDCL     *del_map_ext1;
    SHR_BITDCL     *cb_map_ext1;
    int            entry_bytes_ext2;
    int            entry_words_ext2;
    uint32         *shadow_tab_ext2;
    SHR_BITDCL     *del_map_ext2;
    SHR_BITDCL     *cb_map_ext2;
} l2_entry_data_t;

static l2_entry_data_t tr3_l2x_data[SOC_MAX_NUM_DEVICES];

#define L2X_ENTRY_CALLBACK_SET(_u_, _index_)                     \
{                                                                \
    SHR_BITSET(tr3_l2x_data[(_u_)].cb_map_int, (_index_));       \
    LOG_INFO(BSL_LS_SOC_ARL, \
             (BSL_META_U(unit, \
                         "set_entry_callback: u:%d i=%d\n"),      \
                         _u_, _index_));                                   \
}                                                                
                                                                 
#define L2X_ENTRY_DELETED_SET(_u_, _index_)                      \
{                                                                \
    SHR_BITSET(tr3_l2x_data[(_u_)].del_map_int, (_index_));      \
    LOG_VERBOSE(BSL_LS_SOC_ARL, \
                (BSL_META_U(unit, \
                            "set_entry_deleted: u:%d i=%d\n"),       \
                            _u_, _index_));                                   \
}                                                                

#define EXT_L2X_1_ENTRY_CALLBACK_SET(_u_, _index_)               \
{                                                                \
    SHR_BITSET(tr3_l2x_data[(_u_)].cb_map_ext1, (_index_));      \
    LOG_INFO(BSL_LS_SOC_ARL, \
             (BSL_META_U(unit, \
                         "set_entry_callback: u:%d i=%d\n"),      \
                         _u_, _index_));                                   \
}                                                                
                                                                 
#define EXT_L2X_1_ENTRY_DELETED_SET(_u_, _index_)                \
{                                                                \
    SHR_BITSET(tr3_l2x_data[(_u_)].del_map_ext1, (_index_));     \
    LOG_VERBOSE(BSL_LS_SOC_ARL, \
                (BSL_META_U(unit, \
                            "set_entry_deleted: u:%d i=%d\n"),       \
                            _u_, _index_));                                   \
}                                                                

#define EXT_L2X_2_ENTRY_CALLBACK_SET(_u_, _index_)               \
{                                                                \
    SHR_BITSET(tr3_l2x_data[(_u_)].cb_map_ext2, (_index_));      \
    LOG_INFO(BSL_LS_SOC_ARL, \
             (BSL_META_U(unit, \
                         "set_entry_callback: u:%d i=%d\n"),      \
                         _u_, _index_));                                   \
}                                                                
                                                                 
#define EXT_L2X_2_ENTRY_DELETED_SET(_u_, _index_)                \
{                                                                \
    SHR_BITSET(tr3_l2x_data[(_u_)].del_map_ext2, (_index_));     \
    LOG_VERBOSE(BSL_LS_SOC_ARL, \
                (BSL_META_U(unit, \
                            "set_entry_deleted: u:%d i=%d\n"),       \
                            _u_, _index_));                                   \
}                                                                
                                                                 
#define L2X_IS_CPU_DELETED(_u_, _index_)                         \
            SHR_BITGET(tr3_l2x_data[(_u_)].del_map_int, (_index_))

#define SOC_L2X_TGID_TRUNK_INDICATOR(unit) \
        SOC_IS_RAVEN(unit) ? 0x40 : 0x20
#define SOC_L2X_TGID_PORT_TRUNK_MASK           0x1f
#define SOC_L2X_TGID_PORT_TRUNK_MASK_HI        0x60

/*
 * Function:
 *	soc_l2_entry_register
 * Purpose:
 *	Register a callback routine to be notified of all inserts,
 *	deletes, and updates to the L2X table.
 * Parameters:
 *	unit - StrataSwitch unit number
 *	fn - Callback function to register
 *	fn_data - Extra data passed to callback function
 * Returns:
 *	SOC_E_NONE - Success
 *	SOC_E_MEMORY - Too many callbacks registered
 */

int
soc_l2_entry_register(int unit, soc_l2_entry_cb_fn fn, void *fn_data)
{
    l2_entry_data_t *ad = &tr3_l2x_data[unit];
    int             i;
#ifdef BCM_XGS3_SWITCH_SUPPORT
    soc_control_t *soc = SOC_CONTROL(unit);
    int           mode;

    mode = soc_property_get(unit, spn_L2XMSG_MODE, L2MODE_POLL);

    if ((mode == L2MODE_FIFO) && !soc_feature(unit, soc_feature_l2_modfifo)) {
        mode = L2MODE_POLL;
    }

    if ((mode == L2MODE_POLL) && soc->l2x_external) {
        /* Not supported for external L2 memory configuration. */
        return SOC_E_UNAVAIL;
    }
#endif

    if (!soc_feature(unit, soc_feature_arl_hashed)) {
        return SOC_E_UNAVAIL;
    }

    if (ad->cb_count >= L2X_CB_MAX) {
        return SOC_E_MEMORY;
    }

    for (i = 0; i < ad->cb_count; i++) {
        if ((ad->cb[i].fn == fn &&
             ad->cb[i].fn_data == fn_data)) {
            return SOC_E_NONE; /* Already registered */
        }
    }

    ad->cb[ad->cb_count].fn = fn;
    ad->cb[ad->cb_count].fn_data = fn_data;

    ad->cb_count++;

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2_entry_unregister
 * Purpose:
 *  Unregister a callback routine; requires same args as when registered
 * Parameters:
 *  unit - StrataSwitch unit number
 *  fn - Callback function to unregister; NULL to unregister all
 *  fn_data - Extra data passed to callback function;
 *  must match registered value unless fn is NULL
 * Returns:
 *  SOC_E_NONE - Success
 *  SOC_E_NOT_FOUND - Matching registered routine not found
 */

int
soc_l2_entry_unregister(int unit, soc_l2_entry_cb_fn fn, void *fn_data)
{
    l2_entry_data_t *ad = &tr3_l2x_data[unit];
    int             i;

    if (fn == NULL) {
        ad->cb_count = 0;
        return SOC_E_NONE;
    }

    for (i = 0; i < ad->cb_count; i++) {
        if ((ad->cb[i].fn == fn &&
             ad->cb[i].fn_data == fn_data)) {

            for (ad->cb_count--; i < ad->cb_count; i++) {
                sal_memcpy(&ad->cb[i], &ad->cb[i + 1],
                           sizeof (l2_entry_cb_entry_t));
            }

            return SOC_E_NONE;
        }
    }

    return SOC_E_NOT_FOUND;
}

/*
 * Function:
 *	soc_l2_entry_callback
 * Purpose:
 *	Routine to execute all callbacks on the list.
 * Parameters:
 *	unit - unit number.
 *	entry_del - deleted or updated entry, NULL if none.
 *	entry_add - added or updated entry, NULL if none.
 */

void
soc_l2_entry_callback(int unit, uint32 flags, soc_mem_t mem_type, 
                      l2_combo_entry_t *entry_del, l2_combo_entry_t *entry_add)
{
    l2_entry_data_t *ad = &tr3_l2x_data[unit];
    int        i;
    
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%s %s %s\n"), FUNCTION_NAME(), entry_del ? "DEL" : "",
                 entry_add ? "ADD" : ""));
    for (i = 0; i < ad->cb_count; i++) {
        (*ad->cb[i].fn)(unit, flags, mem_type, entry_del, entry_add, ad->cb[i].fn_data);
    }
}

/*
 * Function:
 * 	soc_l2x_running
 * Purpose:
 *	Determine the L2X sync thread running parameters
 * Parameters:
 *	unit - unit number.
 *	flags (OUT) - if non-NULL, receives the current flag settings
 *	interval (OUT) - if non-NULL, receives the current pass interval
 * Returns:
 *   	Boolean; TRUE if L2X sync thread is running
 */

int
soc_tr3_l2x_running(int unit, uint32 *flags, sal_usecs_t *interval)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    if (soc->l2x_pid != SAL_THREAD_ERROR) {
        if (flags != NULL) {
            *flags = soc->l2x_flags;
        }
        if (interval != NULL) {
            *interval = soc->l2x_interval;
        }
    }

    return(soc->l2x_pid != SAL_THREAD_ERROR);
}

/* It can only handle delete so far */
int
_soc_tr3_l2x_sync_replace(int unit, _soc_tr3_l2_replace_t *rep_st,
                          uint32 flags)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 kt, *entry, *match_data, *match_mask;
    int index_max, entry_words, idx, i;
    soc_memacc_t l21_key_type;
    soc_mem_t mem_type;

    if (soc->l2x_pid == SAL_THREAD_ERROR) {
        /* thread not running */
        return SOC_E_NONE;
    }
    
    /* First work on Internal L2 */
    entry = tr3_l2x_data[unit].shadow_tab_int;
    if (entry == NULL) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (soc_memacc_init(unit, L2_ENTRY_1m, KEY_TYPEf, &l21_key_type));

    index_max = soc_mem_index_max(unit, L2_ENTRY_1m);
    (void)sal_sem_take(soc->l2x_lock, sal_sem_FOREVER);
    for (idx = 0; idx <= index_max; ) {
        mem_type = L2_ENTRY_1m;
        match_data = (uint32 *)&rep_st->match_data1;
        match_mask = (uint32 *)&rep_st->match_mask1;
        kt = soc_memacc_field32_get(&l21_key_type, entry);
        if ((kt == SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE) || 
            (kt == SOC_MEM_KEY_L2_ENTRY_2_L2_VFI) ||
            (kt == SOC_MEM_KEY_L2_ENTRY_2_L2_TRILL_NONUC_ACCESS)) {
            mem_type = L2_ENTRY_2m;
            match_data = (uint32 *)&rep_st->match_data2;
            match_mask = (uint32 *)&rep_st->match_mask2;
        }
        entry_words = soc_mem_entry_words(unit, mem_type);
        for (i = 0; i < entry_words; i++) {
            if ((entry[i] ^ match_data[i]) & match_mask[i]) {
                break;
            }
        }
        if (i != entry_words) {
            entry += entry_words;
            if (mem_type == L2_ENTRY_1m) {
                idx++;
            } else {
                idx += 2;
            }
            continue;
        }
        soc_tr3_l2x_sync_delete(unit, mem_type, entry, idx, flags);
        if (mem_type == L2_ENTRY_1m) {
            idx++;
        } else {
            idx += 2;
        }
        entry += entry_words;
    }
    (void)sal_sem_give(soc->l2x_lock);
    
    /* Then work on external L2 */
    if (!soc_feature(unit, soc_feature_esm_support)) { 
        return SOC_E_NONE;
    }
    /* First work on EXT_L2_ENTRY_1 */
    entry = tr3_l2x_data[unit].shadow_tab_ext1;
    if (entry == NULL) {
        return SOC_E_NONE;
    }
    index_max = soc_mem_index_max(unit, EXT_L2_ENTRY_1m);    
    entry_words = soc_mem_entry_words(unit, EXT_L2_ENTRY_1m);
    (void)sal_sem_take(soc->l2x_lock, sal_sem_FOREVER);
    for (idx = 0; idx <= index_max; idx++, entry += entry_words) {
        match_data = (uint32 *)&rep_st->ext_match_data1;
        match_mask = (uint32 *)&rep_st->ext_match_mask1;        
        for (i = 0; i < entry_words; i++) {
            if ((entry[i] ^ match_data[i]) & match_mask[i]) {
                break;
            }
        }
        if (i != entry_words) {
            continue;
        }
        soc_tr3_l2x_sync_delete(unit, EXT_L2_ENTRY_1m, entry, idx, flags);
    }
    (void)sal_sem_give(soc->l2x_lock);
    
    /* Then work on EXT_L2_ENTRY_2 */
    entry = tr3_l2x_data[unit].shadow_tab_ext2;
    if (entry == NULL) {
        return SOC_E_NONE;
    }
    index_max = soc_mem_index_max(unit, EXT_L2_ENTRY_2m);    
    entry_words = soc_mem_entry_words(unit, EXT_L2_ENTRY_2m);
    (void)sal_sem_take(soc->l2x_lock, sal_sem_FOREVER);
    for (idx = 0; idx <= index_max; idx++, entry += entry_words) {
        match_data = (uint32 *)&rep_st->ext_match_data2;
        match_mask = (uint32 *)&rep_st->ext_match_mask2;        
        for (i = 0; i < entry_words; i++) {
            if ((entry[i] ^ match_data[i]) & match_mask[i]) {
                break;
            }
        }
        if (i != entry_words) {
            continue;
        }
        soc_tr3_l2x_sync_delete(unit, EXT_L2_ENTRY_2m, entry, idx, flags);
    }
    (void)sal_sem_give(soc->l2x_lock);
    
    return SOC_E_NONE;
}

/*
 * Function:
 * 	_soc_tr3_l2x_sync_delete_by
 * Purpose:
 *	Search all the entries in the shadow table based on the 
 *	criteria and mark them as deleted.
 * Parameters:
 * Returns:
 *      SOC_E_XX
 */
int
_soc_tr3_l2x_sync_delete_by(int unit, uint32 mod, uint32 port,
                            uint16 vid, uint32 tid, int vfi,
                            uint32 flags, uint32 search_key)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int           rv = SOC_E_NONE, static_bit_val;
    int           index, max_index, stl2;
    uint32        *entry = NULL;
    uint32        port_val, mod_id;
    vlan_id_t     vlan_id;


    if (soc->l2x_pid == SAL_THREAD_ERROR) {
        /* thread not running */
        return SOC_E_NONE;
    }
    stl2 = (flags & SOC_L2X_INC_STATIC) ? 1 : 0;
    (void)sal_sem_take(soc->l2x_lock, sal_sem_FOREVER);
    entry = tr3_l2x_data[unit].shadow_tab_int;
    if (!entry) {
        goto exit;
    }

    max_index = soc_mem_index_max(unit, L2_ENTRY_1m);

    if (search_key == SOC_L2X_PORTMOD_DEL) {
        /* First work through L2_ENTRY_1 */
        for (index = 0; index < max_index; index++,
            entry += tr3_l2x_data[unit].entry_words_int) {
            if (!L2X_IS_VALID_ENTRY(unit, L2_ENTRY_1m, 
                                    entry, VALIDf)) {
                continue;
            }
            if (soc_mem_field32_get(unit, L2_ENTRY_1m, entry,
                                    L2__DEST_TYPEf) == 1) {
                continue;
            }
            port_val = soc_mem_field32_get(unit, L2_ENTRY_1m,
                                           entry, L2__PORT_NUMf);
            mod_id = soc_mem_field32_get(unit, L2_ENTRY_1m,
                                         entry, L2__MODULE_IDf);
            if ((port != port_val) || (mod != mod_id)) {
                continue;
            }
            static_bit_val = soc_mem_field32_get(unit, L2_ENTRY_1m,
                                                 entry, STATIC_BITf);
            if (stl2 || (stl2 == static_bit_val)) {
                soc_tr3_l2x_sync_delete(unit, L2_ENTRY_1m, entry, index, flags);
            }
        }

        /* Then work through L2_ENTRY_2 */
        entry = tr3_l2x_data[unit].shadow_tab_int;
        max_index = soc_mem_index_max(unit, L2_ENTRY_2m);
        for (index = 0; index < max_index; index+=2,
            entry += tr3_l2x_data[unit].entry_words_int * 2) {
            if (!L2X_IS_VALID_ENTRY(unit, L2_ENTRY_2m, 
                                    entry, VALID_0f)) {
                continue;
            }
            if (soc_mem_field32_get(unit, L2_ENTRY_2m, entry,
                                    L2__DEST_TYPEf) == 1) {
                continue;
            }
            port_val = soc_mem_field32_get(unit, L2_ENTRY_2m,
                                           entry, L2__PORT_NUMf);
            mod_id = soc_mem_field32_get(unit, L2_ENTRY_2m,
                                         entry, L2__MODULE_IDf);
            if ((port != port_val) || (mod != mod_id)) {
                continue;
            }
            static_bit_val = soc_mem_field32_get(unit, L2_ENTRY_2m,
                                                 entry, STATIC_BITf);
            if (stl2 || (stl2 == static_bit_val)) {
                soc_tr3_l2x_sync_delete(unit, L2_ENTRY_2m, entry, index, flags);
            }
        }

        if (soc_feature(unit, soc_feature_esm_support)) {
            /* First work through EXT_L2_ENTRY_1 */
            max_index = soc_mem_index_max(unit, EXT_L2_ENTRY_1m);
            entry = tr3_l2x_data[unit].shadow_tab_ext1;
            if (entry) {
                for (index = 0; index < max_index; index++,
                    entry += tr3_l2x_data[unit].entry_words_ext1) {
                    if (L2X_IS_VALID_ENTRY(unit, EXT_L2_ENTRY_1m, 
                                            entry, FREEf)) {
                        continue;
                    }
                    if (soc_mem_field32_get(unit, EXT_L2_ENTRY_1m, entry,
                                            DEST_TYPEf) == 1) {
                        continue;
                    }
                    port_val = soc_mem_field32_get(unit, EXT_L2_ENTRY_1m,
                                                   entry, PORT_NUMf);
                    mod_id = soc_mem_field32_get(unit, EXT_L2_ENTRY_1m,
                                                 entry, MODULE_IDf);
                    if ((port != port_val) || (mod != mod_id)) {
                        continue;
                    }
                    static_bit_val = soc_mem_field32_get(unit, EXT_L2_ENTRY_1m,
                                                         entry, STATIC_BITf);
                    if (stl2 || (stl2 == static_bit_val)) {
                        soc_tr3_l2x_sync_delete(unit, EXT_L2_ENTRY_1m, entry, 
                                                index, flags);
                    }
                }
            }
            
            /* Then work through EXT_L2_ENTRY_2 */
            max_index = soc_mem_index_max(unit, EXT_L2_ENTRY_2m);
            entry = tr3_l2x_data[unit].shadow_tab_ext2;
            if (entry) {
                for (index = 0; index < max_index; index++,
                    entry += tr3_l2x_data[unit].entry_words_ext2) {
                    if (L2X_IS_VALID_ENTRY(unit, EXT_L2_ENTRY_2m, 
                                            entry, FREEf)) {
                        continue;
                    }
                    if (soc_mem_field32_get(unit, EXT_L2_ENTRY_2m, entry,
                                            DEST_TYPEf) == 1) {
                        continue;
                    }
                    port_val = soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                                                   entry, PORT_NUMf);
                    mod_id = soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                                                 entry, MODULE_IDf);
                    if ((port != port_val) || (mod != mod_id)) {
                        continue;
                    }
                
                    static_bit_val = soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                                                         entry, STATIC_BITf);
                    if (stl2 || (stl2 == static_bit_val)) {
                        soc_tr3_l2x_sync_delete(unit, EXT_L2_ENTRY_2m, entry, 
                                                index, flags);
                    }
                }
            }
        }
        goto exit;
    }

    if (search_key == SOC_L2X_VLAN_DEL) {
        /* First work through L2_ENTRY_1 */
        for (index = 0; index < max_index; index++,
            entry += tr3_l2x_data[unit].entry_words_int) {
            if (!L2X_IS_VALID_ENTRY(unit, L2_ENTRY_1m, 
                                    entry, VALIDf)) {
                continue;
            }
            vlan_id = soc_mem_field32_get(unit, L2_ENTRY_1m,
                                          entry, L2__VLAN_IDf);
            if (vid != vlan_id) {
                continue;
            }
            static_bit_val = soc_mem_field32_get(unit, L2_ENTRY_1m,
                                                 entry, STATIC_BITf);
            if (stl2 || (stl2 == static_bit_val)) {
                soc_tr3_l2x_sync_delete(unit, L2_ENTRY_1m, entry, index, flags);
            }
        }

        /* Then work through L2_ENTRY_2 */
        entry = tr3_l2x_data[unit].shadow_tab_int;
        max_index = soc_mem_index_max(unit, L2_ENTRY_2m);
        for (index = 0; index < max_index; index+=2,
            entry += tr3_l2x_data[unit].entry_words_int * 2) {
            if (!L2X_IS_VALID_ENTRY(unit, L2_ENTRY_2m, 
                                    entry, VALID_0f)) {
                continue;
            }
            vlan_id = soc_mem_field32_get(unit, L2_ENTRY_2m,
                                          entry, L2__VLAN_IDf);
            if (vid != vlan_id) {
                continue;
            }
            static_bit_val = soc_mem_field32_get(unit, L2_ENTRY_2m,
                                                 entry, STATIC_BITf);
            if (stl2 || (stl2 == static_bit_val)) {
                soc_tr3_l2x_sync_delete(unit, L2_ENTRY_2m, entry, index, flags);
            }
        }

        if (soc_feature(unit, soc_feature_esm_support)) {
            /* First work through EXT_L2_ENTRY_1 */
            max_index = soc_mem_index_max(unit, EXT_L2_ENTRY_1m);
            entry = tr3_l2x_data[unit].shadow_tab_ext1;
            if (entry) {
                for (index = 0; index < max_index; index++,
                    entry += tr3_l2x_data[unit].entry_words_ext1) {
                    if (L2X_IS_VALID_ENTRY(unit, EXT_L2_ENTRY_1m, 
                                            entry, FREEf)) {
                        continue;
                    }
                    vlan_id = soc_mem_field32_get(unit, EXT_L2_ENTRY_1m,
                                                  entry, VLAN_IDf);
                    if (vid != vlan_id) {
                        continue;
                    }
                    static_bit_val = soc_mem_field32_get(unit, EXT_L2_ENTRY_1m,
                                                         entry, STATIC_BITf);
                    if (stl2 || (stl2 == static_bit_val)) {
                        soc_tr3_l2x_sync_delete(unit, EXT_L2_ENTRY_1m, entry, 
                                                index, flags);
                    }
                }
            }
            
            /* Then work through EXT_L2_ENTRY_2 */
            max_index = soc_mem_index_max(unit, EXT_L2_ENTRY_2m);
            entry = tr3_l2x_data[unit].shadow_tab_ext2;
            if (entry) {
                for (index = 0; index < max_index; index++,
                    entry += tr3_l2x_data[unit].entry_words_ext2) {
                    if (L2X_IS_VALID_ENTRY(unit, EXT_L2_ENTRY_2m, 
                                            entry, FREEf)) {
                        continue;
                    }
                    vlan_id = soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                                                  entry, VLAN_IDf);
                    if (vid != vlan_id) {
                        continue;
                    }
                    static_bit_val = soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                                                         entry, STATIC_BITf);
                    if (stl2 || (stl2 == static_bit_val)) {
                        soc_tr3_l2x_sync_delete(unit, EXT_L2_ENTRY_2m, entry, 
                                                index, flags);
                    }
                }
            }
        }
        goto exit;
    }

    if (search_key == SOC_L2X_VFI_DEL) {
        int vfi_id;
        /* First work through L2_ENTRY_1 */
        for (index = 0; index < max_index; index++,
            entry += tr3_l2x_data[unit].entry_words_int) {
            if (!L2X_IS_VALID_ENTRY(unit, L2_ENTRY_1m,
                                    entry, VALIDf)) {
                continue;
            }
            vfi_id = soc_mem_field32_get(unit, L2_ENTRY_1m,
                                         entry, L2__VFIf);
            if (vfi != vfi_id) {
                continue;
            }
            static_bit_val = soc_mem_field32_get(unit, L2_ENTRY_1m,
                                                 entry, STATIC_BITf);
            if (stl2 || (stl2 == static_bit_val)) {
                soc_tr3_l2x_sync_delete(unit, L2_ENTRY_1m, entry, index, flags);
            }
        }

        /* Then work through L2_ENTRY_2 */
        entry = tr3_l2x_data[unit].shadow_tab_int;
        max_index = soc_mem_index_max(unit, L2_ENTRY_2m);
        for (index = 0; index < max_index; index+=2,
            entry += tr3_l2x_data[unit].entry_words_int * 2) {
            if (!L2X_IS_VALID_ENTRY(unit, L2_ENTRY_2m,
                                    entry, VALID_0f)) {
                continue;
            }
            vfi_id = soc_mem_field32_get(unit, L2_ENTRY_2m,
                                         entry, L2__VFIf);
            if (vfi != vfi_id) {
                continue;
            }
            static_bit_val = soc_mem_field32_get(unit, L2_ENTRY_2m,
                                                 entry, STATIC_BITf);
            if (stl2 || (stl2 == static_bit_val)) {
                soc_tr3_l2x_sync_delete(unit, L2_ENTRY_2m, entry, index, flags);
            }
        }

        if (soc_feature(unit, soc_feature_esm_support)) {
            /* First work through EXT_L2_ENTRY_1 */
            max_index = soc_mem_index_max(unit, EXT_L2_ENTRY_1m);
            entry = tr3_l2x_data[unit].shadow_tab_ext1;
            if (entry) {
                for (index = 0; index < max_index; index++,
                    entry += tr3_l2x_data[unit].entry_words_ext1) {
                    if (L2X_IS_VALID_ENTRY(unit, EXT_L2_ENTRY_1m, 
                                            entry, FREEf)) {
                        continue;
                    }
                    vfi_id = soc_mem_field32_get(unit, EXT_L2_ENTRY_1m,
                                                 entry, VFIf);
                    if (vfi != vfi_id) {
                        continue;
                    }
                    static_bit_val = soc_mem_field32_get(unit, EXT_L2_ENTRY_1m,
                                                         entry, STATIC_BITf);
                    if (stl2 || (stl2 == static_bit_val)) {
                        soc_tr3_l2x_sync_delete(unit, EXT_L2_ENTRY_1m, entry, 
                                                index, flags);
                    }
                }
            }
            
            /* Then work through EXT_L2_ENTRY_2 */
            max_index = soc_mem_index_max(unit, EXT_L2_ENTRY_2m);
            entry = tr3_l2x_data[unit].shadow_tab_ext2;
            if (entry) {
                for (index = 0; index < max_index; index++,
                    entry += tr3_l2x_data[unit].entry_words_ext2) {
                    if (L2X_IS_VALID_ENTRY(unit, EXT_L2_ENTRY_2m, 
                                            entry, FREEf)) {
                        continue;
                    }
                    vfi_id = soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                                                 entry, VFIf);
                    if (vfi != vfi_id) {
                        continue;
                    }
                    static_bit_val = soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                                                         entry, STATIC_BITf);
                    if (stl2 || (stl2 == static_bit_val)) {
                        soc_tr3_l2x_sync_delete(unit, EXT_L2_ENTRY_2m, entry, 
                                                index, flags);
                    }
                }
            }
        }
        goto exit;
    }

    if (search_key == SOC_L2X_PORTMOD_VLAN_DEL) {
        /* First work through L2_ENTRY_1 */
        for (index = 0; index < max_index; index++,
            entry += tr3_l2x_data[unit].entry_words_int) {
            if (!L2X_IS_VALID_ENTRY(unit, L2_ENTRY_1m, 
                                    entry, VALIDf)) {
                continue;
            }
            if (soc_mem_field32_get(unit, L2_ENTRY_1m, entry,
                                    L2__DEST_TYPEf) == 1) {
                continue;
            }
            port_val = soc_mem_field32_get(unit, L2_ENTRY_1m,
                                           entry, L2__PORT_NUMf);
            mod_id = soc_mem_field32_get(unit, L2_ENTRY_1m,
                                         entry, L2__MODULE_IDf);
            vlan_id = soc_mem_field32_get(unit, L2_ENTRY_1m,
                                          entry, L2__VLAN_IDf);
            if ((vid != vlan_id) ||(port != port_val) ||(mod != mod_id)) {
                continue;
            }
            static_bit_val = soc_mem_field32_get(unit, L2_ENTRY_1m,
                                                 entry, STATIC_BITf);
            if (stl2 || (stl2 == static_bit_val)) {
                soc_tr3_l2x_sync_delete(unit, L2_ENTRY_1m, entry, index, flags);
            }
        }

        /* Then work through L2_ENTRY_2 */
        entry = tr3_l2x_data[unit].shadow_tab_int;
        max_index = soc_mem_index_max(unit, L2_ENTRY_2m);
        for (index = 0; index < max_index; index+=2,
            entry += tr3_l2x_data[unit].entry_words_int * 2) {
            if (!L2X_IS_VALID_ENTRY(unit, L2_ENTRY_2m, 
                                    entry, VALID_0f)) {
                continue;
            }
            if (soc_mem_field32_get(unit, L2_ENTRY_2m, entry,
                                    L2__DEST_TYPEf) == 1) {
                continue;
            }
            port_val = soc_mem_field32_get(unit, L2_ENTRY_2m,
                                           entry, L2__PORT_NUMf);
            mod_id = soc_mem_field32_get(unit, L2_ENTRY_2m,
                                         entry, L2__MODULE_IDf);
            vlan_id = soc_mem_field32_get(unit, L2_ENTRY_2m,
                                          entry, L2__VLAN_IDf);
            if ((vid != vlan_id) ||(port != port_val) ||(mod != mod_id)) {
                continue;
            }
            static_bit_val = soc_mem_field32_get(unit, L2_ENTRY_2m,
                                                 entry, STATIC_BITf);
            if (stl2 || (stl2 == static_bit_val)) {
                soc_tr3_l2x_sync_delete(unit, L2_ENTRY_2m, entry, index, flags);
            }
        }
        
        if (soc_feature(unit, soc_feature_esm_support)) {
            /* First work through EXT_L2_ENTRY_1 */
            max_index = soc_mem_index_max(unit, EXT_L2_ENTRY_1m);
            entry = tr3_l2x_data[unit].shadow_tab_ext1;
            if (entry) {
                for (index = 0; index < max_index; index++,
                    entry += tr3_l2x_data[unit].entry_words_ext1) {
                    if (L2X_IS_VALID_ENTRY(unit, EXT_L2_ENTRY_1m, 
                                            entry, FREEf)) {
                        continue;
                    }
                    port_val = soc_mem_field32_get(unit, EXT_L2_ENTRY_1m,
                                                   entry, PORT_NUMf);
                    mod_id = soc_mem_field32_get(unit, EXT_L2_ENTRY_1m,
                                                 entry, MODULE_IDf);
                    vlan_id = soc_mem_field32_get(unit, EXT_L2_ENTRY_1m,
                                                  entry, VLAN_IDf);
                    if ((vid != vlan_id) ||(port != port_val) ||(mod != mod_id)) {
                        continue;
                    }
                    static_bit_val = soc_mem_field32_get(unit, EXT_L2_ENTRY_1m,
                                                         entry, STATIC_BITf);
                    if (stl2 || (stl2 == static_bit_val)) {
                        soc_tr3_l2x_sync_delete(unit, EXT_L2_ENTRY_1m, entry, 
                                                index, flags);
                    }
                }
            }
            
            /* Then work through EXT_L2_ENTRY_2 */
            max_index = soc_mem_index_max(unit, EXT_L2_ENTRY_2m);
            entry = tr3_l2x_data[unit].shadow_tab_ext2;
            if (entry) {
                for (index = 0; index < max_index; index++,
                    entry += tr3_l2x_data[unit].entry_words_ext2) {
                    if (L2X_IS_VALID_ENTRY(unit, EXT_L2_ENTRY_2m, 
                                            entry, FREEf)) {
                        continue;
                    }
                    port_val = soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                                                   entry, PORT_NUMf);
                    mod_id = soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                                                 entry, MODULE_IDf);
                    vlan_id = soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                                                  entry, VLAN_IDf);
                    if ((vid != vlan_id) ||(port != port_val) ||(mod != mod_id)) {
                        continue;
                    }
                    static_bit_val = soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                                                         entry, STATIC_BITf);
                    if (stl2 || (stl2 == static_bit_val)) {
                        soc_tr3_l2x_sync_delete(unit, EXT_L2_ENTRY_2m, entry, 
                                                index, flags);
                    }
                }
            }
        }
        goto exit;
    }

    if ((search_key == SOC_L2X_TRUNK_DEL) || 
        (search_key == SOC_L2X_TRUNK_VLAN_DEL)) {
        /* First work through L2_ENTRY_1 */
        for (index = 0; index < max_index; index++,
            entry += tr3_l2x_data[unit].entry_words_int) {
            if (!L2X_IS_VALID_ENTRY(unit, L2_ENTRY_1m, 
                                    entry, VALIDf)) {
                continue;
            }
            if ((soc_mem_field32_get(unit, L2_ENTRY_1m,
                                     entry, L2__DEST_TYPEf) != 1) ||
                (soc_mem_field32_get(unit, L2_ENTRY_1m,
                                     entry, L2__TGIDf) != tid)) {
                continue;
            }
            if ((search_key == SOC_L2X_TRUNK_VLAN_DEL) && 
                (vid != soc_mem_field32_get(unit, L2_ENTRY_1m,
                                            entry, L2__VLAN_IDf))) {
                continue;
            }
            static_bit_val = soc_mem_field32_get(unit, L2_ENTRY_1m,
                                                 entry, STATIC_BITf);
            if (stl2 || (stl2 == static_bit_val)) {
                soc_tr3_l2x_sync_delete(unit, L2_ENTRY_1m, entry, index, flags);
            }
        }

        /* Then work through L2_ENTRY_2 */
        entry = tr3_l2x_data[unit].shadow_tab_int;
        max_index = soc_mem_index_max(unit, L2_ENTRY_2m);
        for (index = 0; index < max_index; index+=2,
            entry += tr3_l2x_data[unit].entry_words_int * 2) {
            if (!L2X_IS_VALID_ENTRY(unit, L2_ENTRY_2m, 
                                    entry, VALID_0f)) {
                continue;
            }
            if ((soc_mem_field32_get(unit, L2_ENTRY_2m,
                                     entry, L2__DEST_TYPEf) != 1) ||
                (soc_mem_field32_get(unit, L2_ENTRY_2m,
                                     entry, L2__TGIDf) != tid)) {
                continue;
            }
            if ((search_key == SOC_L2X_TRUNK_VLAN_DEL) && 
                (vid != soc_mem_field32_get(unit, L2_ENTRY_2m,
                                            entry, L2__VLAN_IDf))) {
                continue;
            }
            static_bit_val = soc_mem_field32_get(unit, L2_ENTRY_2m,
                                                 entry, STATIC_BITf);
            if (stl2 || (stl2 == static_bit_val)) {
                soc_tr3_l2x_sync_delete(unit, L2_ENTRY_2m, entry, index, flags);
            }
        }
        
        if (soc_feature(unit, soc_feature_esm_support)) {
            /* First work through EXT_L2_ENTRY_1 */
            max_index = soc_mem_index_max(unit, EXT_L2_ENTRY_1m);
            entry = tr3_l2x_data[unit].shadow_tab_ext1;
            if (entry) {
                for (index = 0; index < max_index; index++,
                    entry += tr3_l2x_data[unit].entry_words_ext1) {
                    if (L2X_IS_VALID_ENTRY(unit, EXT_L2_ENTRY_1m, 
                                            entry, FREEf)) {
                        continue;
                    }
                    if ((soc_mem_field32_get(unit, EXT_L2_ENTRY_1m,
                                             entry, DEST_TYPEf) != 1) ||
                        (soc_mem_field32_get(unit, EXT_L2_ENTRY_1m,
                                             entry, TGIDf) != tid)) {
                        continue;
                    }
                    if ((search_key == SOC_L2X_TRUNK_VLAN_DEL) && 
                        (vid != soc_mem_field32_get(unit, EXT_L2_ENTRY_1m,
                                                    entry, VLAN_IDf))) {
                        continue;
                    }
                    static_bit_val = soc_mem_field32_get(unit, EXT_L2_ENTRY_1m,
                                                         entry, STATIC_BITf);
                    if (stl2 || (stl2 == static_bit_val)) {
                        soc_tr3_l2x_sync_delete(unit, EXT_L2_ENTRY_1m, entry, 
                                                index, flags);
                    }
                }
            }
            
            /* Then work through EXT_L2_ENTRY_2 */
            max_index = soc_mem_index_max(unit, EXT_L2_ENTRY_2m);
            entry = tr3_l2x_data[unit].shadow_tab_ext2;
            if (entry) {
                for (index = 0; index < max_index; index++,
                    entry += tr3_l2x_data[unit].entry_words_ext2) {
                    if (L2X_IS_VALID_ENTRY(unit, EXT_L2_ENTRY_2m, 
                                            entry, FREEf)) {
                        continue;
                    }
                    if ((soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                                             entry, DEST_TYPEf) != 1) ||
                        (soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                                             entry, TGIDf) != tid)) {
                        continue;
                    }
                    if ((search_key == SOC_L2X_TRUNK_VLAN_DEL) && 
                        (vid != soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                                                    entry, VLAN_IDf))) {
                        continue;
                    }
                    static_bit_val = soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                                                         entry, STATIC_BITf);
                    if (stl2 || (stl2 == static_bit_val)) {
                        soc_tr3_l2x_sync_delete(unit, EXT_L2_ENTRY_2m, entry, 
                                                index, flags);
                    }
                }
            }
        }
        goto exit;
    }

    rv = SOC_E_PARAM;

exit:
    (void)sal_sem_give(soc->l2x_lock);
    return rv;
}

/*
 * Function:
 * 	soc_tr3_l2x_sync_delete
 * Purpose:
 *	sync the SW shadow copy for the deleted entry from HW.
 * Parameters:
 *	unit - unit number.
 *	del_entry - entry deleted from the HW l2X memory
 * Returns:
 *      SOC_E_XX
 */

int
soc_tr3_l2x_sync_delete(int unit, soc_mem_t mem, uint32 *del_entry, int index, 
                        uint32 flags)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int           max_index;
    uint32        *tab_p, kt;
    soc_mem_t     mem_type;   

    LOG_INFO(BSL_LS_SOC_ARL,
             (BSL_META_U(unit,
                         "soc_tr3_l2x_sync_delete: unit=%d index=%d\n"),
                         unit, index));

    if (soc->l2x_pid == SAL_THREAD_ERROR) {
        /* thread not running */
        return SOC_E_NONE;
    }

    if (!tr3_l2x_data[unit].enabled) {
        return SOC_E_NONE;
    }
    
    if (mem == L2_ENTRY_1m || mem == L2_ENTRY_2m) {
        /* Determine entry type/size */
        mem_type = L2_ENTRY_1m;
        kt = soc_mem_field32_get(unit, L2_ENTRY_1m, del_entry, KEY_TYPEf);
        if ((kt == SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE) || 
            (kt == SOC_MEM_KEY_L2_ENTRY_2_L2_VFI) ||
            (kt == SOC_MEM_KEY_L2_ENTRY_2_L2_TRILL_NONUC_ACCESS)) {
            mem_type = L2_ENTRY_2m;
        }
        if (mem_type == L2_ENTRY_1m) {
            max_index = soc_mem_index_max(unit, L2_ENTRY_1m);
        } else {
            max_index = soc_mem_index_max(unit, L2_ENTRY_2m);
        }
        if (index > max_index) {
            return SOC_E_PARAM;
        }
        
        /*
         * validate that the entry in the shadow copy is same as the entry
         * deleted.
         */
        if (mem_type == L2_ENTRY_1m) {
            tab_p = tr3_l2x_data[unit].shadow_tab_int + 
                    (index * tr3_l2x_data[unit].entry_words_int);
        } else {
            tab_p = tr3_l2x_data[unit].shadow_tab_int + 
                    (index * tr3_l2x_data[unit].entry_words_int * 2);
        }    
        if (L2X_ENTRY_EQL(unit, del_entry, tab_p, 0)) {
            L2X_ENTRY_DELETED_SET(unit, index);
            /* Bitmap that indicates to suppress callback */
            if ((flags & SOC_L2X_NO_CALLBACKS)) {
                L2X_ENTRY_CALLBACK_SET(unit, index);
            }
        }
    } else if (mem == EXT_L2_ENTRY_1m) {
        if (index > soc_mem_index_max(unit, EXT_L2_ENTRY_1m)) {
            return SOC_E_PARAM;
        }        
        /*
         * validate that the entry in the shadow copy is same as the entry
         * deleted.
         */
        tab_p = tr3_l2x_data[unit].shadow_tab_ext1 + 
                    (index * tr3_l2x_data[unit].entry_words_ext1);
        if (EXT_L2X_1_ENTRY_EQL(unit, del_entry, tab_p, 0)) {
            EXT_L2X_1_ENTRY_DELETED_SET(unit, index);
            /* Bitmap that indicates to suppress callback */
            if ((flags & SOC_L2X_NO_CALLBACKS)) {
                EXT_L2X_1_ENTRY_CALLBACK_SET(unit, index);
            }
        }
    } else if (mem == EXT_L2_ENTRY_2m) {
        if (index > soc_mem_index_max(unit, EXT_L2_ENTRY_2m)) {
            return SOC_E_PARAM;
        }        
        /*
         * validate that the entry in the shadow copy is same as the entry
         * deleted.
         */
        tab_p = tr3_l2x_data[unit].shadow_tab_ext2 + 
                    (index * tr3_l2x_data[unit].entry_words_ext2);
        if (EXT_L2X_2_ENTRY_EQL(unit, del_entry, tab_p, 0)) {
            EXT_L2X_2_ENTRY_DELETED_SET(unit, index);
            /* Bitmap that indicates to suppress callback */
            if ((flags & SOC_L2X_NO_CALLBACKS)) {
                EXT_L2X_2_ENTRY_CALLBACK_SET(unit, index);
            }
        }
    } else {
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_tr3_l2x_stop
 * Purpose:
 *   	Stop L2X-related thread
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX
 */

int
soc_tr3_l2x_stop(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int           rv = SOC_E_NONE;
    soc_timeout_t to;
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(BCM_CMICM_SUPPORT)
    int           mode;
#endif

    LOG_INFO(BSL_LS_SOC_ARL,
             (BSL_META_U(unit,
                         "soc_tr3_l2x_stop: unit=%d\n"), unit));

    tr3_l2x_data[unit].enabled = 0;
    SOC_CONTROL_LOCK(unit);
    soc->l2x_interval = 0;  /* Request exit */
    SOC_CONTROL_UNLOCK(unit);

    if (soc->l2x_pid != SAL_THREAD_ERROR) {
        /* Wake up thread so it will check the exit flag */
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(BCM_CMICM_SUPPORT)
        mode = soc_property_get(unit, spn_L2XMSG_MODE, L2MODE_POLL);
        if (soc_feature(unit, soc_feature_l2_modfifo) &&
            (mode == L2MODE_FIFO)) {
            _soc_l2mod_stop(unit);
        } else
#endif
        {
            sal_sem_give(soc->l2x_notify);
        }

        /* Give thread a few seconds to wake up and exit */
        if (SAL_BOOT_SIMULATION) {
            soc_timeout_init(&to, 30 * 1000000, 0);
        } else {
            soc_timeout_init(&to, 10 * 1000000, 0);
        }

        while (soc->l2x_pid != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_L2,
                          (BSL_META_U(unit,
                                      "soc_tr3_l2x_stop: thread will not exit\n")));
                rv = SOC_E_INTERNAL;
                break;
            }
        }
    }

    return rv;
}

/*
 * Function:
 * 	soc_tr3_l2x_start
 * Purpose:
 *   	Initialize shadow table and start L2X thread to maintain it
 * Parameters:
 *	unit - unit number.
 *	flags - SOC_L2X_FLAG_xxx
 *	interval - time between resynchronization passes
 * Returns:
 *	SOC_E_MEMORY if can't create thread.
 */

int
soc_tr3_l2x_start(int unit, uint32 flags, sal_usecs_t interval)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int           pri;
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(BCM_CMICM_SUPPORT)
    int           mode;
#endif

    LOG_INFO(BSL_LS_SOC_ARL,
             (BSL_META_U(unit,
                         "soc_tr3_l2x_start: unit=%d flags=0x%x interval=%d\n"),
              unit, flags, interval));

    if (!soc_feature(unit, soc_feature_arl_hashed)) {
        return SOC_E_UNAVAIL;
    }

    if (soc->l2x_interval != 0) {
        SOC_IF_ERROR_RETURN(soc_tr3_l2x_stop(unit));
    }

    sal_snprintf(soc->l2x_name, sizeof (soc->l2x_name),
                 "bcmL2X.%d", unit);

    if (soc->l2x_pid == SAL_THREAD_ERROR) {
        pri = soc_property_get(unit, spn_L2XMSG_THREAD_PRI, 50);
        soc->l2x_age_hitsa_only = soc_property_get(unit,
                                                   spn_L2X_AGE_ONLY_ON_HITSA,
                                                   0);
#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(BCM_CMICM_SUPPORT)
        mode = soc_property_get(unit, spn_L2XMSG_MODE, L2MODE_POLL);

        if (mode == L2MODE_FIFO) {

            SOC_CONTROL_LOCK(unit);
            soc->l2x_mode = mode;
            soc->l2x_flags = flags;
            soc->l2x_interval = interval;
    
            if (interval == 0) {
                SOC_CONTROL_UNLOCK(unit);
                return SOC_E_NONE;
            }
            _soc_l2mod_start(unit, flags, interval);
            SOC_CONTROL_UNLOCK(unit);
            /* For 'FIFO' mode, set the 'enabled' to '0',
               to indicate NO L2 data store. */
            tr3_l2x_data[unit].enabled = 0;
        } else
#endif
        {
            SOC_CONTROL_LOCK(unit);
            soc->l2x_mode = L2MODE_POLL;
            soc->l2x_flags = flags;
            soc->l2x_interval = interval;

            if (interval == 0) {
                SOC_CONTROL_UNLOCK(unit);
                return SOC_E_NONE;
            }
            soc->l2x_pid = sal_thread_create(soc->l2x_name,
                                             SAL_THREAD_STKSZ,
                                             pri,
                                             _soc_tr3_l2x_thread,
                                             INT_TO_PTR(unit));
            if (soc->l2x_pid == SAL_THREAD_ERROR) {
                LOG_ERROR(BSL_LS_SOC_L2,
                          (BSL_META_U(unit,
                                      "soc_tr3_l2x_start: Could not start L2X thread\n")));
                SOC_CONTROL_UNLOCK(unit);
                return SOC_E_MEMORY;
            }
            SOC_CONTROL_UNLOCK(unit);
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	_soc_l2x_sync_bucket
 * Purpose:
 *	Compare the old contents of a hash bucket to the
 *	next contents, make any ARL callbacks that are necessary,
 *	and sync that bucket in the shadow table.
 * Parameters:
 *	unit - StrataSwitch unit #
 *	old_bucket - Previous bucket contents (SOC_TR3_L2X_BUCKET_SIZE entries)
 *	new_bucket - New bucket contents (SOC_TR3_L2X_BUCKET_SIZE entries)
 *	shadow_hit_bits - perform the notify callback even if
 *			only hit bits change.
 *	bucket_index - start index of bucket within chunk
 *	p_chunk_del_map - pointer to chunk's delete map
 * Notes:
 *	It's necessary to process deletions first, then insertions.
 *	If an entry moves within the hash bucket, or some field such
 *	as PORT changes, the application first receives a callback
 *	for a deletion, and then for an insertion.
 */

STATIC void
_soc_tr3_l2x_sync_bucket(int unit, uint32 *old_bucket, uint32 *new_bucket,
                         uint8 shadow_hit_bits, int bucket_index,
                         SHR_BITDCL *p_chunk_del_map, 
                         SHR_BITDCL *p_chunk_cb_map)
{
    int       io, in, delete_marked, callback_suppress;
    uint32    *old_p, *old_ptr, *new_p, *new_ptr;
    uint32    kt, kt_new, key_val;
    soc_mem_t mem_type_new, mem_type;
    soc_field_t field;
    
    /* Since we read everything using a type 1 view only thus, if valid and 
       present then convert the single type 2 entry split across 2 raw type 1
       entries back into a single type 2 entry */
    new_p = new_bucket;
    for (in = 0; in < SOC_TR3_L2X_BUCKET_SIZE; in+=2, 
         new_p += tr3_l2x_data[unit].entry_words_int * 2) {
        if (!L2X_IS_VALID_ENTRY(unit, L2_ENTRY_2m, 
                                new_p, VALID_0f)) {
            continue;
        }
        kt_new = soc_mem_field32_get(unit, L2_ENTRY_1m, new_p, KEY_TYPEf);
        if ((kt_new == SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE) || 
            (kt_new == SOC_MEM_KEY_L2_ENTRY_2_L2_VFI) ||
            (kt_new == SOC_MEM_KEY_L2_ENTRY_2_L2_TRILL_NONUC_ACCESS)) {
            uint32 tmp[SOC_MAX_MEM_WORDS];
            uint32 *src[4] = {0};
            src[0] = new_p;
            src[1] = new_p + tr3_l2x_data[unit].entry_words_int;
            soc_mem_base_to_wide_entry_conv(unit, L2_ENTRY_2m, L2_ENTRY_1m, 
                                            tmp, src, TYPE_1_TO_TYPE_2);
            sal_memcpy(new_p, tmp, soc_mem_entry_bytes(unit, L2_ENTRY_2m));
        }
    }
    
    /*
     * Process deletions (which will be any entry that was in the bucket
     * before but whose key is no longer found anywhere in the bucket)
     * and changes (which will be any entry whose key is still found but
     * whose associated data (port, etc.) has changed).
     */

    old_p = old_bucket;
    new_ptr = new_bucket;
    for (io = 0; io < SOC_TR3_L2X_BUCKET_SIZE; ) {
        /*
         * No deletion or change is needed for invalid entries.
         */
        if (!soc_mem_field32_get(unit, L2_ENTRY_1m, old_p, VALIDf)) {
            io++;
            old_p += tr3_l2x_data[unit].entry_words_int;
            new_ptr += tr3_l2x_data[unit].entry_words_int;
            continue;
        }
        
        /* Determine entry type/size */
        mem_type = L2_ENTRY_1m;
        kt = soc_mem_field32_get(unit, L2_ENTRY_1m, old_p, KEY_TYPEf);
        if ((kt == SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE) || 
            (kt == SOC_MEM_KEY_L2_ENTRY_2_L2_VFI) ||
            (kt == SOC_MEM_KEY_L2_ENTRY_2_L2_TRILL_NONUC_ACCESS)) {
            mem_type = L2_ENTRY_2m;
        }
        
        /*
         * Check if the entry has been deleted and is marked for
         * delete.
         */
        delete_marked = SHR_BITGET(p_chunk_del_map, bucket_index + io);
        callback_suppress = SHR_BITGET(p_chunk_cb_map, bucket_index + io);

        /*
         * Quick check: if the entry is identical, it does not need any
         * deletion or change processing.  It may be valid or not.
         */
        if (L2X_ENTRY_EQL(unit, old_p, new_ptr, 0)) {
            /* Entry got learnt again after delete */
            if (delete_marked) {
                if (callback_suppress) {
                    /* Indicate re-learn */
                    soc_l2_entry_callback(unit, 0, mem_type, NULL,
                                         (l2_combo_entry_t *) new_ptr);
                } else {
                    /* Indicate delete and re-learn */
                    soc_l2_entry_callback(unit, 0, mem_type, 
                                          (l2_combo_entry_t *) old_p, NULL);
                    soc_l2_entry_callback(unit, 0, mem_type, NULL,
                                          (l2_combo_entry_t *) new_ptr);
                }
            }
            goto adv_del_process;
        }

        /*
         * See if the old key still exists anywhere within the bucket.
         */
        new_p = new_bucket;
        for (in = 0; in < SOC_TR3_L2X_BUCKET_SIZE; ) {
            mem_type_new = L2_ENTRY_1m;
            field = VALIDf;
            kt_new = soc_mem_field32_get(unit, L2_ENTRY_1m, new_p, KEY_TYPEf);
            if ((kt_new == SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE) || 
                (kt_new == SOC_MEM_KEY_L2_ENTRY_2_L2_VFI) ||
                (kt_new == SOC_MEM_KEY_L2_ENTRY_2_L2_TRILL_NONUC_ACCESS)) {
                assert(in % 2 == 0); /* wide entry should never be on odd boundary */
                mem_type_new = L2_ENTRY_2m;
                field = VALID_0f;
            }
            if (soc_mem_field32_get(unit, mem_type_new,
                                    new_p, field) &&
                (soc_mem_compare_key(unit, mem_type_new,
                                     old_p, new_p) == 0)) {
                break;
            }
            if (mem_type == L2_ENTRY_1m) {
                if (mem_type_new == L2_ENTRY_1m) {
                    in++;
                    new_p += tr3_l2x_data[unit].entry_words_int;
                } else {
                    in += 2;
                    new_p += tr3_l2x_data[unit].entry_words_int * 2;
                }
            } else {
                in += 2;
                new_p += tr3_l2x_data[unit].entry_words_int * 2;
            }
        }

        if (in == SOC_TR3_L2X_BUCKET_SIZE) {
            /* It doesn't, so issue a delete. */
            if (!callback_suppress) {
                soc_l2_entry_callback(unit, 0, mem_type, (l2_combo_entry_t *)old_p, 
                                      NULL);
            }
            goto adv_del_process;
        }

        /*
         * Check if everything at the new location is the same (ignoring
         * hit bit).  If so, there is no delete or change processing.
         */
        if (!(shadow_hit_bits & L2X_SHADOW_HIT_BITS)) {
            if (!(shadow_hit_bits & L2X_SHADOW_HIT_DST)) { 
                soc_mem_field32_set(unit, mem_type, old_p, HITDAf,
                                    soc_mem_field32_get(unit, mem_type,
                                                        new_p, HITDAf));
            }
            if (!(shadow_hit_bits & L2X_SHADOW_HIT_SRC)) {
                soc_mem_field32_set(unit, mem_type, old_p, HITSAf,
                                    soc_mem_field32_get(unit, mem_type,
                                                        new_p, HITSAf));
            }
        }

        if (L2X_ENTRY_EQL(unit, old_p, new_p, shadow_hit_bits)) {
            /* Entry got learnt again after delete */
            if (delete_marked) {
                if (callback_suppress) {
                    /* Indicate re-learn */
                    soc_l2_entry_callback(unit, 0, mem_type, NULL, 
                                         (l2_combo_entry_t *) new_ptr);
                } else {
                    /* Indicate delete and re-learn */
                    soc_l2_entry_callback(unit, 0, mem_type, 
                                          (l2_combo_entry_t *) old_p, NULL);
                    soc_l2_entry_callback(unit, 0, mem_type, NULL,
                                          (l2_combo_entry_t *) new_ptr);
                }
            }
            goto adv_del_process;
        }

        /* Issue change (delete & insert) */
        if (!callback_suppress) {
            soc_l2_entry_callback(unit, 0, mem_type,
                                  (l2_combo_entry_t *) old_p,
                                  (l2_combo_entry_t *) new_p);
        }

adv_del_process:        
        if (mem_type == L2_ENTRY_1m) {
            io++;
            old_p += tr3_l2x_data[unit].entry_words_int;
            new_ptr += tr3_l2x_data[unit].entry_words_int;
        } else {
            io += 2;
            old_p += tr3_l2x_data[unit].entry_words_int * 2;
            new_ptr += tr3_l2x_data[unit].entry_words_int * 2;
        }        
    }

    /*
     * Process insertions, which will be any entry whose key was not in
     * the bucket before, but is now.
     */

    new_p = new_bucket;
    old_ptr = old_bucket;
    for (in = 0; in < SOC_TR3_L2X_BUCKET_SIZE; ) {
        /* Determine entry type/size */
        mem_type = L2_ENTRY_1m;
        kt = soc_mem_field32_get(unit, L2_ENTRY_1m, new_p, KEY_TYPEf);
        if ((kt == SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE) || 
            (kt == SOC_MEM_KEY_L2_ENTRY_2_L2_VFI) ||
            (kt == SOC_MEM_KEY_L2_ENTRY_2_L2_TRILL_NONUC_ACCESS)) {
            mem_type = L2_ENTRY_2m;
        }
        
        /*
         * Quick check: if the entry is identical, it does not need any
         * insertion processing.  It may be valid or not.
         */
        if (L2X_ENTRY_EQL(unit, new_p, old_ptr, 0)) {
            goto adv_ins_process;
        }

        /*
         * No insert needed for invalid entries.
         */
        if (!soc_mem_field32_get(unit, L2_ENTRY_1m, new_p, VALIDf)) {
            in++;
            new_p += tr3_l2x_data[unit].entry_words_int;
            old_ptr += tr3_l2x_data[unit].entry_words_int;
            continue;
        }

        /*
         * If the same key already existed elsewhere within the bucket,
         * it has already been taken care of by the change processing.
         */

        old_p = old_bucket;
        for (io = 0; io < SOC_TR3_L2X_BUCKET_SIZE; ) {
            mem_type_new = L2_ENTRY_1m;
            field = VALIDf;
            kt_new = soc_mem_field32_get(unit, L2_ENTRY_1m, old_p, KEY_TYPEf);
            if ((kt_new == SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE) || 
                (kt_new == SOC_MEM_KEY_L2_ENTRY_2_L2_VFI) ||
                (kt_new == SOC_MEM_KEY_L2_ENTRY_2_L2_TRILL_NONUC_ACCESS)) {
                assert(io % 2 == 0); /* wide entry should never be on odd boundary */
                mem_type_new = L2_ENTRY_2m;
                field = VALID_0f;
            }
            if (soc_mem_field32_get(unit, mem_type_new,
                                    old_p, field) &&
                soc_mem_compare_key(unit, mem_type_new,
                                    new_p, old_p) == 0) {
                break;
            }
            if (mem_type == L2_ENTRY_1m) {
                if (mem_type_new == L2_ENTRY_1m) {
                    io++;
                    old_p += tr3_l2x_data[unit].entry_words_int;
                } else {
                    io += 2;
                    old_p += tr3_l2x_data[unit].entry_words_int * 2;
                }
            } else {
                io += 2;
                old_p += tr3_l2x_data[unit].entry_words_int * 2;
            }
        }

        if (io < SOC_TR3_L2X_BUCKET_SIZE) {
            goto adv_ins_process;
        }

        if (soc_feature(unit, soc_feature_ppa_bypass)) {
            if (SOC_CONTROL(unit)->l2x_ppa_bypass == FALSE) {
                if (mem_type == L2_ENTRY_1m) {
                    key_val = SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE;
                } else {
                    key_val = SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE;
                } 
                if (soc_mem_field32_get(unit, mem_type, new_p, KEY_TYPEf) !=
                    key_val) {
                    SOC_CONTROL(unit)->l2x_ppa_bypass = TRUE;
                }
            }
        }

        /* Issue insert */
        delete_marked = SHR_BITGET(p_chunk_del_map, bucket_index + in);
        if (!delete_marked) {
            soc_l2_entry_callback(unit, 0, mem_type, NULL, 
                                  (l2_combo_entry_t *)new_p);
        } else {
            /* Special case - New entry but marked for deletion by now */
            soc_l2_entry_callback(unit, 0, mem_type, (l2_combo_entry_t *)new_p,
                                  (l2_combo_entry_t *)new_p);
        }

adv_ins_process:        
        if (mem_type == L2_ENTRY_1m) {        
            in++;
            new_p += tr3_l2x_data[unit].entry_words_int;
            old_ptr += tr3_l2x_data[unit].entry_words_int;
        } else {
            in += 2;
            new_p += tr3_l2x_data[unit].entry_words_int * 2;
            old_ptr += tr3_l2x_data[unit].entry_words_int *2;
        }
    }

    /* Sync shadow copy */

    sal_memcpy(old_bucket, new_bucket,
               SOC_TR3_L2X_BUCKET_SIZE * tr3_l2x_data[unit].entry_words_int*4);
}

/*
 * Function:
 * 	_soc_tr3_l2x_thread
 * Purpose:
 *   	Thread control for L2 shadow table maintenance
 * Parameters:
 *	unit_vp - StrataSwitch unit # (as a void *).
 * Returns:
 *	Nothing
 * Notes:
 *	Exits when l2x_rate is set to zero and semaphore is given.  The
 *	table is processed one chunk at a time (spn_L2XMSG_CHUNKS chunks
 *	total) in order to reduce memory requirements for the temporary
 *	DMA buffer and to give the CPU a break from bursts of activity.
 */

STATIC void
_soc_tr3_l2x_thread(void *unit_vp)
{
    int           unit = PTR_TO_INT(unit_vp);
    soc_control_t *soc = SOC_CONTROL(unit);
    int           rv;
    uint32        *shadow_tab = NULL;
    uint32        *chunk_buf_int = NULL;
    uint32        *chunk_buf_ext1 = NULL, *chunk_buf_ext2 = NULL;
    SHR_BITDCL    *del_map_int = NULL;
    SHR_BITDCL    *del_map_ext1 = NULL, *del_map_ext2 = NULL;
    SHR_BITDCL    *chunk_del_map_int = NULL;
    SHR_BITDCL    *chunk_del_map_ext1 = NULL, *chunk_del_map_ext2 = NULL;
    SHR_BITDCL    *cb_map_int = NULL, *cb_map_ext1 = NULL, *cb_map_ext2 = NULL;
    SHR_BITDCL    *chunk_cb_map_int = NULL;
    SHR_BITDCL    *chunk_cb_map_ext1 = NULL, *chunk_cb_map_ext2 = NULL;
    int           index_count_int, index_count_ext1 = 0, index_count_ext2 = 0;
    int           chunk_count_int = 0, chunk_size_int = 0;
    int           chunk_count_ext1 = 0, chunk_size_ext1 = 0;
    int           chunk_count_ext2 = 0, chunk_size_ext2 = 0;
    int           chunk_index_int = 0, bucket_index, index;
    int           chunk_index_ext1 = 0, chunk_index_ext2 = 0;
    uint32        *tab_p_int = NULL, *buf_p;
    uint32        *tab_p_ext1 = NULL, *tab_p_ext2 = NULL;
    int           interval, delete_marked, callback_suppress;
    sal_usecs_t   stime, etime;
    int           iter = 0;

    tr3_l2x_data[unit].shadow_tab_int = NULL;
    tr3_l2x_data[unit].shadow_tab_ext1 = NULL;
    tr3_l2x_data[unit].shadow_tab_ext2 = NULL;
    
    tr3_l2x_data[unit].entry_bytes_int = soc_mem_entry_bytes(unit, L2_ENTRY_1m);
    tr3_l2x_data[unit].entry_words_int = soc_mem_entry_words(unit, L2_ENTRY_1m);

    index_count_int = soc_mem_index_count(unit, L2_ENTRY_1m);
    if (index_count_int <= 0) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "soc_tr3_l2x_thread: Internal L2 table size is 0 \n")));
        tr3_l2x_data[unit].int_avail = FALSE;
        goto skip_l2_int;
    } else {
        tr3_l2x_data[unit].int_avail = TRUE;
    }
    
    chunk_count_int = soc_property_get(unit, spn_L2XMSG_CHUNKS, 4);
    chunk_size_int = index_count_int / chunk_count_int;

    assert(chunk_size_int > 0);
    assert(chunk_size_int % SOC_TR3_L2X_BUCKET_SIZE == 0);

    shadow_tab = sal_alloc(index_count_int * tr3_l2x_data[unit].entry_words_int * 4,
                           "l2x_old");

    chunk_buf_int = soc_cm_salloc(unit,
                              chunk_size_int * tr3_l2x_data[unit].entry_words_int * 4,
                              "l2x_new");

    del_map_int = sal_alloc(SHR_BITALLOCSIZE(index_count_int), "l2x_delete_map");
    cb_map_int = sal_alloc(SHR_BITALLOCSIZE(index_count_int), "l2x_callback_map");

    chunk_del_map_int = 
    sal_alloc(SHR_BITALLOCSIZE(chunk_size_int), "l2x_chunk_delete_map");
    chunk_cb_map_int = 
    sal_alloc(SHR_BITALLOCSIZE(chunk_size_int), "l2x_chunk_callback_map");

    if (shadow_tab == NULL || chunk_buf_int == NULL || 
        del_map_int == NULL || chunk_del_map_int == NULL || 
        cb_map_int == NULL || chunk_cb_map_int == NULL) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "AbnormalThreadExit:soc_l2x_thread: not enough memory \n")));
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                           SOC_SWITCH_EVENT_THREAD_L2X, __LINE__, SOC_E_MEMORY);
        goto cleanup_exit;
    }
    /*
     * Start with initially empty shadow table.
     */

    sal_memset(shadow_tab, 0, index_count_int * tr3_l2x_data[unit].entry_words_int * 4);

    /*
     * Clear the delete and callback map.
     */
    SHR_BITCLR_RANGE(del_map_int, 0, index_count_int);
    SHR_BITCLR_RANGE(cb_map_int, 0, index_count_int);

    tr3_l2x_data[unit].shadow_tab_int = shadow_tab;
    tr3_l2x_data[unit].del_map_int = del_map_int;
    tr3_l2x_data[unit].cb_map_int = cb_map_int;

    tab_p_int = shadow_tab;

    shadow_tab = NULL;

skip_l2_int:
    if (!soc_feature(unit, soc_feature_esm_support)) {
        goto skip_l2_ext2;
    }
    
    tr3_l2x_data[unit].entry_bytes_ext1 = soc_mem_entry_bytes(unit, EXT_L2_ENTRY_1m);
    tr3_l2x_data[unit].entry_words_ext1 = soc_mem_entry_words(unit, EXT_L2_ENTRY_1m);

    index_count_ext1 = soc_mem_index_count(unit, EXT_L2_ENTRY_1m);
    if (index_count_ext1 <= 0) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "soc_tr3_l2x_thread: External L2_1 table size is 0 \n")));
        tr3_l2x_data[unit].ext1_avail = FALSE;
        goto skip_l2_ext1;
    } else {
        tr3_l2x_data[unit].ext1_avail = TRUE;
    }
    
    chunk_count_ext1 = soc_property_get(unit, spn_L2XMSG_CHUNKS, 4);
    chunk_size_ext1 = index_count_ext1 / chunk_count_ext1;

    assert(chunk_size_ext1 > 0);
    assert(chunk_size_ext1 % SOC_TR3_L2X_BUCKET_SIZE == 0);

    shadow_tab = sal_alloc(index_count_ext1 * tr3_l2x_data[unit].entry_words_ext1 * 4,
                           "l2x_old");

    chunk_buf_ext1 = soc_cm_salloc(unit, chunk_size_ext1 * 
                                   tr3_l2x_data[unit].entry_words_ext1 * 4,
                                   "l2x_new");

    del_map_ext1 = sal_alloc(SHR_BITALLOCSIZE(index_count_ext1), "l2x_delete_map");
    cb_map_ext1 = sal_alloc(SHR_BITALLOCSIZE(index_count_ext1), "l2x_callback_map");

    chunk_del_map_ext1 = 
    sal_alloc(SHR_BITALLOCSIZE(chunk_size_ext1), "l2x_chunk_delete_map");
    chunk_cb_map_ext1 = 
    sal_alloc(SHR_BITALLOCSIZE(chunk_size_ext1), "l2x_chunk_callback_map");

    if (shadow_tab == NULL || chunk_buf_ext1 == NULL || 
        del_map_ext1 == NULL || chunk_del_map_ext1 == NULL || 
        cb_map_ext1 == NULL || chunk_cb_map_ext1 == NULL) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "AbnormalThreadExit:soc_l2x_thread: not enough memory \n")));
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                           SOC_SWITCH_EVENT_THREAD_L2X, __LINE__, SOC_E_MEMORY);
        goto cleanup_exit;
    }
    /*
     * Start with initially empty shadow table.
     */
    sal_memset(shadow_tab, 0, index_count_ext1 * tr3_l2x_data[unit].entry_words_ext1 * 4);
    buf_p = shadow_tab;
    for (index = 0; index < index_count_ext1; index ++) {
        soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, buf_p, FREEf, 1);
        buf_p += tr3_l2x_data[unit].entry_words_ext1;
    }

    /*
     * Clear the delete and callback map.
     */
    SHR_BITCLR_RANGE(del_map_ext1, 0, index_count_ext1);
    SHR_BITCLR_RANGE(cb_map_ext1, 0, index_count_ext1);

    tr3_l2x_data[unit].shadow_tab_ext1 = shadow_tab;
    tr3_l2x_data[unit].del_map_ext1 = del_map_ext1;
    tr3_l2x_data[unit].cb_map_ext1 = cb_map_ext1;

    tab_p_ext1 = shadow_tab;    

    shadow_tab = NULL;

skip_l2_ext1:
    tr3_l2x_data[unit].entry_bytes_ext2 = soc_mem_entry_bytes(unit, EXT_L2_ENTRY_2m);
    tr3_l2x_data[unit].entry_words_ext2 = soc_mem_entry_words(unit, EXT_L2_ENTRY_2m);

    index_count_ext2 = soc_mem_index_count(unit, EXT_L2_ENTRY_2m);
    if (index_count_ext2 <= 0) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "soc_tr3_l2x_thread: External L2_2 table size is 0 \n")));
        tr3_l2x_data[unit].ext2_avail = FALSE;
        goto skip_l2_ext2;
    } else {
        tr3_l2x_data[unit].ext2_avail = TRUE;
    }
    
    chunk_count_ext2 = soc_property_get(unit, spn_L2XMSG_CHUNKS, 4);
    chunk_size_ext2 = index_count_ext2 / chunk_count_ext2;

    assert(chunk_size_ext2 > 0);
    assert(chunk_size_ext2 % SOC_TR3_L2X_BUCKET_SIZE == 0);

    shadow_tab = sal_alloc(index_count_ext2 * tr3_l2x_data[unit].entry_words_ext2 * 4,
                           "l2x_old");

    chunk_buf_ext2 = soc_cm_salloc(unit, chunk_size_ext2 * 
                                   tr3_l2x_data[unit].entry_words_ext2 * 4,
                                   "l2x_new");

    del_map_ext2 = sal_alloc(SHR_BITALLOCSIZE(index_count_ext2), "l2x_delete_map");
    cb_map_ext2 = sal_alloc(SHR_BITALLOCSIZE(index_count_ext2), "l2x_callback_map");

    chunk_del_map_ext2 = 
    sal_alloc(SHR_BITALLOCSIZE(chunk_size_ext2), "l2x_chunk_delete_map");
    chunk_cb_map_ext2 = 
    sal_alloc(SHR_BITALLOCSIZE(chunk_size_ext2), "l2x_chunk_callback_map");

    if (shadow_tab == NULL || chunk_buf_ext2 == NULL || 
        del_map_ext2 == NULL || chunk_del_map_ext2 == NULL || 
        cb_map_ext2 == NULL || chunk_cb_map_ext2 == NULL) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "AbnormalThreadExit:soc_l2x_thread: not enough memory \n")));
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                           SOC_SWITCH_EVENT_THREAD_L2X, __LINE__, SOC_E_MEMORY);
        goto cleanup_exit;
    }
    /*
     * Start with initially empty shadow table.
     */
    sal_memset(shadow_tab, 0, index_count_ext2 * tr3_l2x_data[unit].entry_words_ext2 * 4);
    buf_p = shadow_tab;
    for (index = 0; index < index_count_ext2; index ++) {
        soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, buf_p, FREEf, 1);
        buf_p += tr3_l2x_data[unit].entry_words_ext2;
    }

    /*
     * Clear the delete and callback map.
     */
    SHR_BITCLR_RANGE(del_map_ext2, 0, index_count_ext2);
    SHR_BITCLR_RANGE(cb_map_ext2, 0, index_count_ext2);

    tr3_l2x_data[unit].shadow_tab_ext2 = shadow_tab;
    tr3_l2x_data[unit].del_map_ext2 = del_map_ext2;
    tr3_l2x_data[unit].cb_map_ext2 = cb_map_ext2;

    tab_p_ext2 = shadow_tab;    
    
    shadow_tab = NULL;

skip_l2_ext2:
    if (tr3_l2x_data[unit].int_avail == FALSE && 
        tr3_l2x_data[unit].ext1_avail == FALSE && 
        tr3_l2x_data[unit].ext2_avail == FALSE) {
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                           SOC_SWITCH_EVENT_THREAD_L2X, __LINE__, SOC_E_EMPTY);
        goto no_cleanup_exit;
    }
    /*
     * For 'POLL' mode, set the 'enabled' to '1',
     * to indicate L2 data store.
     */
    tr3_l2x_data[unit].enabled = 1;

    while ((interval = soc->l2x_interval) != 0) {
        /*
         * Read the next chunk of the L2 table using Table DMA.
         */
        stime = sal_time_usecs();

        /*
         * If running for the first time wait for
         * duration = interval
         */
        if (!iter) {
            goto continue_loop;
        }

        if (tr3_l2x_data[unit].int_avail == FALSE) {
            goto skip_l2_int_loop;
        }
        LOG_VERBOSE(BSL_LS_SOC_ARL,
                    (BSL_META_U(unit,
                                "soc_l2x_thread: Process INT %d-%d\n"),
                     chunk_index_int, chunk_index_int + chunk_size_int - 1));
        SOC_MEM_L2_INT_LOCK(unit);
        if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                                  "AbnormalThreadExit:soc_l2x_thread: unable to take mutex\n")));
            SOC_MEM_L2_INT_UNLOCK(unit);
            soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                               SOC_SWITCH_EVENT_THREAD_L2X, __LINE__,
                               SOC_E_RESOURCE);
            goto cleanup_exit;
        }

        if ((rv = soc_mem_read_range(unit, L2_ENTRY_1m,
                                     MEM_BLOCK_ANY, chunk_index_int,
                                     chunk_index_int + chunk_size_int - 1,
                                     chunk_buf_int)) < 0) {
            SOC_L2_DEL_SYNC_UNLOCK(soc);
            SOC_MEM_L2_INT_UNLOCK(unit);
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                                  "AbnormalThreadExit:soc_l2x_thread: DMA failed: %s\n"),
                                  soc_errmsg(rv)));

            soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                               SOC_SWITCH_EVENT_THREAD_L2X, __LINE__, rv);

            goto cleanup_exit;
        } else {
            SHR_BITCOPY_RANGE(chunk_del_map_int, 0, del_map_int, chunk_index_int, 
                              chunk_size_int);
            SHR_BITCOPY_RANGE(chunk_cb_map_int, 0, cb_map_int, chunk_index_int, 
                              chunk_size_int);

            SHR_BITCLR_RANGE(del_map_int, chunk_index_int, chunk_size_int);
            SHR_BITCLR_RANGE(cb_map_int, chunk_index_int, chunk_size_int);

            SOC_L2_DEL_SYNC_UNLOCK(soc);
            SOC_MEM_L2_INT_UNLOCK(unit);

            /*
             * Scan, compare, and sync the old and new chunks one bucket
             * at a time.
             */
            buf_p = chunk_buf_int;
            for (bucket_index = 0; bucket_index < chunk_size_int; 
                bucket_index += SOC_TR3_L2X_BUCKET_SIZE) {
                _soc_tr3_l2x_sync_bucket(unit, tab_p_int, buf_p, 
                                         soc->l2x_shadow_hit_bits,
                                         bucket_index, chunk_del_map_int, 
                                         chunk_cb_map_int);
                tab_p_int += tr3_l2x_data[unit].entry_words_int * SOC_TR3_L2X_BUCKET_SIZE;
                buf_p += tr3_l2x_data[unit].entry_words_int * SOC_TR3_L2X_BUCKET_SIZE;
            }

            if ((chunk_index_int += chunk_size_int) >= index_count_int) {
                chunk_index_int = 0;
                tab_p_int = tr3_l2x_data[unit].shadow_tab_int;
            }
        }

skip_l2_int_loop:
        if (!soc_feature(unit, soc_feature_esm_support)) {
            goto continue_loop;
        }        
        if (tr3_l2x_data[unit].ext1_avail == FALSE) {
            goto skip_l2_ext1_loop;
        }
        LOG_VERBOSE(BSL_LS_SOC_ARL,
                    (BSL_META_U(unit,
                                "soc_l2x_thread: Process EXT1 %d-%d\n"),
                     chunk_index_ext1, chunk_index_ext1 + chunk_size_ext1 - 1));

        /* First work on EXT_L2_ENTRY_1 */
        SOC_MEM_L2_EXT_1_LOCK(unit);
        if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                                  "AbnormalThreadExit:soc_l2x_thread: unable to take mutex\n")));
            SOC_MEM_L2_INT_UNLOCK(unit);
            soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                               SOC_SWITCH_EVENT_THREAD_L2X, __LINE__,
                               SOC_E_RESOURCE);
            goto cleanup_exit;
        }

        if ((rv = soc_mem_read_range(unit, EXT_L2_ENTRY_1m,
                                     MEM_BLOCK_ANY, chunk_index_ext1,
                                     chunk_index_ext1 + chunk_size_ext1 - 1,
                                     chunk_buf_ext1)) < 0) {
            SOC_L2_DEL_SYNC_UNLOCK(soc);
            SOC_MEM_L2_EXT_1_UNLOCK(unit);
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                                  "AbnormalThreadExit:soc_l2x_thread: DMA failed: %s\n"),
                                  soc_errmsg(rv)));

            soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                               SOC_SWITCH_EVENT_THREAD_L2X, __LINE__, rv);

            goto cleanup_exit;
        } else {
            SHR_BITCOPY_RANGE(chunk_del_map_ext1, 0, del_map_ext1, chunk_index_ext1, 
                              chunk_size_ext1);
            SHR_BITCOPY_RANGE(chunk_cb_map_ext1, 0, cb_map_ext1, chunk_index_ext1, 
                              chunk_size_ext1);

            SHR_BITCLR_RANGE(del_map_ext1, chunk_index_ext1, chunk_size_ext1);
            SHR_BITCLR_RANGE(cb_map_ext1, chunk_index_ext1, chunk_size_ext1);

            SOC_L2_DEL_SYNC_UNLOCK(soc);
            SOC_MEM_L2_EXT_1_UNLOCK(unit);

            /*
             * Scan, compare, and sync the old and new chunks.
             */
            buf_p = chunk_buf_ext1;
            for (index = 0; index < chunk_size_ext1; index ++) {
                /* Check if old entry was free */
                if (soc_mem_field32_get(unit, EXT_L2_ENTRY_1m, tab_p_ext1, FREEf)) {
                    /* if new entry is used then issue insert cb */
                    if (!soc_mem_field32_get(unit, EXT_L2_ENTRY_1m, buf_p, FREEf)) {
                        delete_marked = SHR_BITGET(chunk_del_map_ext1, index);
                        if (!delete_marked) {
                            soc_l2_entry_callback(unit, 0, EXT_L2_ENTRY_1m, NULL, 
                                                  (l2_combo_entry_t *)buf_p);
                        } else {
                            /* Special case - New entry but marked for deletion by now */
                            soc_l2_entry_callback(unit, 0, EXT_L2_ENTRY_1m, 
                                                  (l2_combo_entry_t *)buf_p, 
                                                  (l2_combo_entry_t *)buf_p);
                        }
                    }
                } else {
                    /* If the new entry is free then issue del cb */
                    if (soc_mem_field32_get(unit, EXT_L2_ENTRY_1m, buf_p, FREEf)) {
                        callback_suppress = SHR_BITGET(chunk_cb_map_ext1, index);
                        if (!callback_suppress) {
                            soc_l2_entry_callback(unit, 0, EXT_L2_ENTRY_1m, 
                                                  (l2_combo_entry_t *)tab_p_ext1, NULL);
                        }
                    } else {
                        /* If new entry is different then issue a replace (del+ins) cb */
                        delete_marked = SHR_BITGET(chunk_del_map_ext1, index);
                        callback_suppress = SHR_BITGET(chunk_cb_map_ext1, index);
                        if (!(soc->l2x_shadow_hit_bits & L2X_SHADOW_HIT_BITS)) {
                            if (!(soc->l2x_shadow_hit_bits & L2X_SHADOW_HIT_DST)) { 
                                soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, tab_p_ext1, 
                                    HITDAf, soc_mem_field32_get(unit, 
                                                                EXT_L2_ENTRY_1m,
                                                                buf_p, HITDAf));
                            }
                            if (!(soc->l2x_shadow_hit_bits & L2X_SHADOW_HIT_SRC)) {
                                soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, tab_p_ext1, 
                                    HITSAf, soc_mem_field32_get(unit, 
                                                                EXT_L2_ENTRY_1m,
                                                                buf_p, HITSAf));
                            }
                        }
                        if (EXT_L2X_1_ENTRY_EQL(unit, tab_p_ext1, buf_p, 
                                                soc->l2x_shadow_hit_bits)) {
                            if (delete_marked && !callback_suppress) {
                                soc_l2_entry_callback(unit, 0, EXT_L2_ENTRY_1m,
                                    (l2_combo_entry_t *)tab_p_ext1, NULL);
                            }
                        } else {
                            if (delete_marked && !callback_suppress) {
                                soc_l2_entry_callback(unit, 0, EXT_L2_ENTRY_1m,
                                    (l2_combo_entry_t *)tab_p_ext1,
                                    (l2_combo_entry_t *)buf_p);
                            }
                        }
                    }
                }
                sal_memcpy(tab_p_ext1, buf_p, tr3_l2x_data[unit].entry_words_ext1 * 4);
                tab_p_ext1 += tr3_l2x_data[unit].entry_words_ext1;
                buf_p += tr3_l2x_data[unit].entry_words_ext1;
            }

            if ((chunk_index_ext1 += chunk_size_ext1) >= index_count_ext1) {
                chunk_index_ext1 = 0;
                tab_p_ext1 = tr3_l2x_data[unit].shadow_tab_ext1;
            }
        }
        
skip_l2_ext1_loop:
        if (tr3_l2x_data[unit].ext2_avail == FALSE) {
            goto continue_loop;
        }
        LOG_VERBOSE(BSL_LS_SOC_ARL,
                    (BSL_META_U(unit,
                                "soc_l2x_thread: Process EXT2 %d-%d\n"),
                     chunk_index_ext2, chunk_index_ext2 + chunk_size_ext2 - 1));
        /* Then work on EXT_L2_ENTRY_2 */
        SOC_MEM_L2_EXT_2_LOCK(unit);
        if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                                  "AbnormalThreadExit:soc_l2x_thread: unable to take mutex\n")));
            SOC_MEM_L2_INT_UNLOCK(unit);
            soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                               SOC_SWITCH_EVENT_THREAD_L2X, __LINE__,
                               SOC_E_RESOURCE);
            goto cleanup_exit;
        }

        if ((rv = soc_mem_read_range(unit, EXT_L2_ENTRY_2m,
                                     MEM_BLOCK_ANY, chunk_index_ext2,
                                     chunk_index_ext2 + chunk_size_ext2 - 1,
                                     chunk_buf_ext2)) < 0) {
            SOC_L2_DEL_SYNC_UNLOCK(soc);
            SOC_MEM_L2_EXT_2_UNLOCK(unit);
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                                  "AbnormalThreadExit:soc_l2x_thread: DMA failed: %s\n"),
                                  soc_errmsg(rv)));

            soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                               SOC_SWITCH_EVENT_THREAD_L2X, __LINE__, rv);

            goto cleanup_exit;
        } else {
            SHR_BITCOPY_RANGE(chunk_del_map_ext2, 0, del_map_ext2, chunk_index_ext2, 
                              chunk_size_ext2);
            SHR_BITCOPY_RANGE(chunk_cb_map_ext2, 0, cb_map_ext2, chunk_index_ext2, 
                              chunk_size_ext2);

            SHR_BITCLR_RANGE(del_map_ext2, chunk_index_ext2, chunk_size_ext2);
            SHR_BITCLR_RANGE(cb_map_ext2, chunk_index_ext2, chunk_size_ext2);

            SOC_L2_DEL_SYNC_UNLOCK(soc);
            SOC_MEM_L2_EXT_2_UNLOCK(unit);

            /*
             * Scan, compare, and sync the old and new chunks.
             */
            buf_p = chunk_buf_ext2;
            for (index = 0; index < chunk_size_ext2; index ++) {
                /* Check if old entry was free */
                if (soc_mem_field32_get(unit, EXT_L2_ENTRY_2m, tab_p_ext2, FREEf)) {
                    /* if new entry is used then issue insert cb */
                    if (!soc_mem_field32_get(unit, EXT_L2_ENTRY_2m, buf_p, FREEf)) {
                        delete_marked = SHR_BITGET(chunk_del_map_ext2, index);
                        if (!delete_marked) {
                            soc_l2_entry_callback(unit, 0, EXT_L2_ENTRY_2m, NULL, 
                                                  (l2_combo_entry_t *)buf_p);
                        }
                    }
                } else {
                    /* If the new entry is free then issue del cb */
                    if (soc_mem_field32_get(unit, EXT_L2_ENTRY_2m, buf_p, FREEf)) {
                        callback_suppress = SHR_BITGET(chunk_cb_map_ext2, index);
                        if (!callback_suppress) {
                            soc_l2_entry_callback(unit, 0, EXT_L2_ENTRY_2m, 
                                                  (l2_combo_entry_t *)tab_p_ext2, NULL);
                        }
                    } else {
                        /* If new entry is different then issue a replace (del+ins) cb */
                        delete_marked = SHR_BITGET(chunk_del_map_ext2, index);
                        callback_suppress = SHR_BITGET(chunk_cb_map_ext2, index);
                        if (!(soc->l2x_shadow_hit_bits & L2X_SHADOW_HIT_BITS)) {
                            if (!(soc->l2x_shadow_hit_bits & L2X_SHADOW_HIT_DST)) { 
                                soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, tab_p_ext2, 
                                    HITDAf, soc_mem_field32_get(unit, 
                                                                EXT_L2_ENTRY_2m,
                                                                buf_p, HITDAf));
                            }
                            if (!(soc->l2x_shadow_hit_bits & L2X_SHADOW_HIT_SRC)) {
                                soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, tab_p_ext2, 
                                    HITSAf, soc_mem_field32_get(unit, 
                                                                EXT_L2_ENTRY_2m,
                                                                buf_p, HITSAf));
                            }
                        }
                        if (EXT_L2X_2_ENTRY_EQL(unit, tab_p_ext2, buf_p,
                                                soc->l2x_shadow_hit_bits)) {
                            if (delete_marked && !callback_suppress) {
                                soc_l2_entry_callback(unit, 0, EXT_L2_ENTRY_2m,
                                    (l2_combo_entry_t *)tab_p_ext2, NULL);
                            }
                        } else {
                            if (delete_marked && !callback_suppress) {
                                soc_l2_entry_callback(unit, 0, EXT_L2_ENTRY_2m,
                                    (l2_combo_entry_t *)tab_p_ext2, 
                                    (l2_combo_entry_t *)buf_p);
                            }
                        }
                    }
                }
                sal_memcpy(tab_p_ext2, buf_p, tr3_l2x_data[unit].entry_words_ext2 * 4);
                tab_p_ext2 += tr3_l2x_data[unit].entry_words_ext2;
                buf_p += tr3_l2x_data[unit].entry_words_ext2;
            }

            if ((chunk_index_ext2 += chunk_size_ext2) >= index_count_ext2) {
                chunk_index_ext2 = 0;
                tab_p_ext2 = tr3_l2x_data[unit].shadow_tab_ext2;
            }
        }
continue_loop:
        etime = sal_time_usecs();

        /*
         * Implement the sleep using a semaphore timeout so if the task
         * is requested to exit, it can do so immediately.
         */

        LOG_VERBOSE(BSL_LS_SOC_ARL,
                    (BSL_META_U(unit,
                                "soc_l2x_thread: unit=%d: done in %d usec\n"), unit,
                     SAL_USECS_SUB(etime, stime)));

        sal_sem_take(soc->l2x_notify, interval / (chunk_count_int ? 
                     chunk_count_int : chunk_count_ext1 ? chunk_count_ext1 :
                     chunk_count_ext2 ? chunk_count_ext2 : 4));

        if (!iter) {
            iter++;
        }
    }

cleanup_exit:
    (void)sal_sem_take(soc->l2x_lock, sal_sem_FOREVER);

    if (chunk_buf_int != NULL) {
        soc_cm_sfree(unit, chunk_buf_int);
    }

    if (shadow_tab != NULL) {
        soc_cm_sfree(unit, shadow_tab);
        shadow_tab = NULL;
    }

    if (tr3_l2x_data[unit].shadow_tab_int != NULL) {
        sal_free(tr3_l2x_data[unit].shadow_tab_int);
        tr3_l2x_data[unit].shadow_tab_int = NULL;
    }

    if (del_map_int != NULL) {
        sal_free(del_map_int);
        tr3_l2x_data[unit].del_map_int = NULL;
    }

    if (chunk_del_map_int != NULL) {
        sal_free(chunk_del_map_int);
    }

    if (cb_map_int != NULL) {
        sal_free(cb_map_int);
        tr3_l2x_data[unit].cb_map_int = NULL;
    }

    if (chunk_cb_map_int != NULL) {
        sal_free(chunk_cb_map_int);
    }
    
    if (!soc_feature(unit, soc_feature_esm_support)) {
        goto skip_cleanup_l2_ext;
    }
    
    if (chunk_buf_ext1 != NULL) {
        soc_cm_sfree(unit, chunk_buf_ext1);
    }

    if (tr3_l2x_data[unit].shadow_tab_ext1 != NULL) {
        sal_free(tr3_l2x_data[unit].shadow_tab_ext1);
        tr3_l2x_data[unit].shadow_tab_ext1 = NULL;
    }

    if (del_map_ext1 != NULL) {
        sal_free(del_map_ext1);
        tr3_l2x_data[unit].del_map_ext1 = NULL;
    }

    if (chunk_del_map_ext1 != NULL) {
        sal_free(chunk_del_map_ext1);
    }

    if (cb_map_ext1 != NULL) {
        sal_free(cb_map_ext1);
        tr3_l2x_data[unit].cb_map_ext1 = NULL;
    }

    if (chunk_cb_map_ext1 != NULL) {
        sal_free(chunk_cb_map_ext1);
    }

    if (chunk_buf_ext2 != NULL) {
        soc_cm_sfree(unit, chunk_buf_ext2);
    }

    if (tr3_l2x_data[unit].shadow_tab_ext2 != NULL) {
        sal_free(tr3_l2x_data[unit].shadow_tab_ext2);
        tr3_l2x_data[unit].shadow_tab_ext2 = NULL;
    }

    if (del_map_ext2 != NULL) {
        sal_free(del_map_ext2);
        tr3_l2x_data[unit].del_map_ext2 = NULL;
    }

    if (chunk_del_map_ext2 != NULL) {
        sal_free(chunk_del_map_ext2);
    }

    if (cb_map_ext2 != NULL) {
        sal_free(cb_map_ext2);
        tr3_l2x_data[unit].cb_map_ext2 = NULL;
    }

    if (chunk_cb_map_ext2 != NULL) {
        sal_free(chunk_cb_map_ext2);
    }

skip_cleanup_l2_ext:
    (void)sal_sem_give(soc->l2x_lock);

no_cleanup_exit:
    LOG_INFO(BSL_LS_SOC_ARL,
             (BSL_META_U(unit,
                         "soc_l2x_thread: exiting\n")));

    soc->l2x_pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

void
soc_tr3_l2mod_fifo_process(int unit, uint32 flags, l2_mod_fifo_entry_t *entry)
{
    uint32 op;
    soc_mem_t mem_type = INVALIDm;
    uint32 fval[SOC_MAX_MEM_FIELD_WORDS], fval1[SOC_MAX_MEM_FIELD_WORDS];
    l2_combo_entry_t l2_entry, old_l2_entry, temp_l2_entry;
    uint32 key_type;
    int station_move = FALSE;
    int assoc_data_length = 0;
    LOG_VERBOSE(BSL_LS_SOC_ARL,
                (BSL_META_U(unit,
                            "Processing L2 MOD FIFO message..\n")));
   
    if (soc_L2_MOD_FIFOm_field32_get(unit, entry, EXTERNAL_L2_ENTRYf)) {
        if (soc_feature(unit, soc_feature_esm_support)) {
            if (soc_L2_MOD_FIFOm_field32_get(unit, entry, ENTRY_WIDTHf)) {
                mem_type = EXT_L2_ENTRY_2m;
                 soc_L2_MOD_FIFOm_field_get(unit, entry, L2_ENTRYf,
                                            l2_entry.ext_l2_entry_2.entry_data);

                LOG_INFO(BSL_LS_SOC_ARL,
                         (BSL_META_U(unit,
                                     "External L2 entry type 2.\n")));
            } else {
                mem_type = EXT_L2_ENTRY_1m;
                soc_L2_MOD_FIFOm_field_get(unit, entry, L2_ENTRYf,
                                           l2_entry.ext_l2_entry_1.entry_data);

                LOG_INFO(BSL_LS_SOC_ARL,
                         (BSL_META_U(unit,
                                     "External L2 entry type 1.\n")));
            }            
        } else {
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                                  "Unexpected external L2 entry data in L2 MOD FIFO !!\n")));
        }
    } else {
        if (soc_L2_MOD_FIFOm_field32_get(unit, entry, ENTRY_WIDTHf)) {
            soc_L2_MOD_FIFOm_field_get(unit, entry, L2_ENTRYf,
                                       l2_entry.l2_entry_2.entry_data);
            mem_type = L2_ENTRY_2m;
            LOG_INFO(BSL_LS_SOC_ARL,
                     (BSL_META_U(unit,
                                 "Internal L2 entry type 2.\n")));
        } else {
            soc_L2_MOD_FIFOm_field_get(unit, entry, L2_ENTRYf,
                                       l2_entry.l2_entry_1.entry_data);
            mem_type = L2_ENTRY_1m;
            LOG_INFO(BSL_LS_SOC_ARL,
                     (BSL_META_U(unit,
                                 "Internal L2 entry type 1.\n")));
        }
    }

    if (soc_L2_MOD_FIFOm_field32_get(unit, entry, STATION_MOVEf)) {
        if (mem_type == L2_ENTRY_1m) {
            /* following is to handle the reserved bit in the actual l2 entry 
            that does not exist in the mod fifo data so we convert/split the 
            fifo data into l2 entry by copying over parts of fifo data */
            soc_mem_field_get(unit, L2_ENTRY_1m, l2_entry.l2_entry_1.entry_data, KEY_TYPEf,
                              &key_type);
            if (key_type == 0 || key_type == 2) {
                /*It is only acceptable for Bridge and VFI mode*/
                sal_memcpy(&old_l2_entry.l2_entry_1, &l2_entry.l2_entry_1, 
                           sizeof(l2_entry.l2_entry_1));
                soc_L2_MOD_FIFOm_field_get(unit, (uint32 *)entry,
                                           REPLACED_ASSOC_DATAf, fval);
                soc_mem_field_set(unit, L2_ENTRY_1m, temp_l2_entry.l2_entry_1.entry_data,
                                  L2__DATAf, fval);
                soc_mem_field_set(unit, L2_ENTRY_1m, old_l2_entry.l2_entry_1.entry_data, 
                                  L2__DATA_Af,
                                  soc_mem_field_get(unit, L2_ENTRY_1m, 
                                                    temp_l2_entry.l2_entry_1.entry_data,
                                                    L2__DATA_Af, fval));
                soc_mem_field_set(unit, L2_ENTRY_1m, old_l2_entry.l2_entry_1.entry_data, 
                                  L2__DATA_Bf,
                                  soc_mem_field_get(unit, L2_ENTRY_1m, 
                                                    temp_l2_entry.l2_entry_1.entry_data,
                                                    L2__DUMMYf, fval));
            }
        } else if (mem_type == L2_ENTRY_2m) {
            /* following is needed because associated data is not contiguous in 
               l2 double wide entry but it is contiguous in the mod fifo data so 
               we convert/split the fifo data into l2 entry by copying over parts 
               of fifo data */
            sal_memcpy(&old_l2_entry.l2_entry_2, &l2_entry.l2_entry_2, 
                       sizeof(l2_entry.l2_entry_2));
            soc_L2_MOD_FIFOm_field_get(unit, (uint32 *)entry,
                                       REPLACED_ASSOC_DATAf, fval);
            soc_mem_field_set(unit, L2_ENTRY_2m, temp_l2_entry.l2_entry_2.entry_data,
                              L2__DATAf, fval);
            soc_mem_field_set(unit, L2_ENTRY_2m, old_l2_entry.l2_entry_2.entry_data, 
                              L2__DATA_0f,
                              soc_mem_field_get(unit, L2_ENTRY_2m, 
                                                temp_l2_entry.l2_entry_2.entry_data,
                                                L2__DATA_0f, fval));
            soc_mem_field_set(unit, L2_ENTRY_2m, old_l2_entry.l2_entry_2.entry_data, 
                              L2__DATA_1_Af,
                              soc_mem_field_get(unit, L2_ENTRY_2m, 
                                                temp_l2_entry.l2_entry_2.entry_data,
                                                L2__DUMMY0f, fval));
            soc_mem_field_set(unit, L2_ENTRY_2m, old_l2_entry.l2_entry_2.entry_data, 
                              L2__DATA_1_Bf,
                              soc_mem_field_get(unit, L2_ENTRY_2m, 
                                                temp_l2_entry.l2_entry_2.entry_data,
                                                L2__DUMMY1f, fval));
        }
    }
    if (bsl_check(bslLayerSoc, bslSourceDma, bslSeverityVerbose, unit)) {
        soc_mem_entry_dump(unit, L2_MOD_FIFOm, entry, BSL_VERBOSE|BSL_LS_SOC_DMA);
        LOG_VERBOSE(BSL_LS_SOC_DMA, (BSL_META_U(unit, "\n")));
    }
    if (mem_type == INVALIDm) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "Unable to determine L2 mem type !!\n")));
        return;
    }
    op = soc_L2_MOD_FIFOm_field32_get(unit, entry, OPERATIONf);
    if ((op == 2 || op == 6 || op == 8) &&
        (mem_type == EXT_L2_ENTRY_1m || mem_type == EXT_L2_ENTRY_2m)) {
        (void)soc_triumph3_ext_l2_entry_update(unit, mem_type,
                  soc_L2_MOD_FIFOm_field32_get(unit, entry, ENTRY_ADRf),
                  (op == 2) ? &l2_entry : NULL);
    }

    switch (op) {
    case 0: /* L2_DELETE */
    case 6: /* L2_BULK_DELETE */
    case 8: /* L2_BULK_AGE */
        soc_l2_entry_callback(unit, 0, mem_type, &l2_entry, NULL);
        break;
    case 7: /* L2_BULK_REPLACE */
        soc_l2_entry_callback(unit, 0, mem_type, &l2_entry, &l2_entry);
        break;
    case 1: /* L2_INSERT */
    case 2: /* LEARN */
        if (soc_L2_MOD_FIFOm_field32_get(unit, entry, STATION_MOVEf)) {
            /* The following is a workaround for L2_MOD_FIFO. The false callback caused 
               by false staion move is avoided. */
            if (mem_type == L2_ENTRY_1m) {
                assoc_data_length = BITS2BYTES(soc_mem_field_length(unit, L2_ENTRY_1m, 
                                                         L2__DATAf));
                soc_L2_MOD_FIFOm_field_get(unit, (uint32 *)entry,
                                           REPLACED_ASSOC_DATAf, fval);
                soc_mem_field_get(unit, L2_ENTRY_1m, l2_entry.l2_entry_1.entry_data,
                                  L2__DATAf, fval1);
                station_move = sal_memcmp(fval1, fval, assoc_data_length);
                    
            } else if (mem_type == L2_ENTRY_2m) {
                assoc_data_length = BITS2BYTES(soc_mem_field_length(unit, L2_ENTRY_2m, 
                                                         L2__DATAf));
                soc_L2_MOD_FIFOm_field_get(unit, (uint32 *)entry,
                                           REPLACED_ASSOC_DATAf, fval);
                soc_mem_field_get(unit, L2_ENTRY_2m, l2_entry.l2_entry_2.entry_data,
                                  L2__DATAf, fval1);
                station_move = sal_memcmp(fval1, fval, assoc_data_length);

            } else if (mem_type == EXT_L2_ENTRY_1m) {
                assoc_data_length = BITS2BYTES(soc_mem_field_length(unit, EXT_L2_ENTRY_1m, 
                                                                    DATAf));
                soc_L2_MOD_FIFOm_field_get(unit, (uint32 *)entry,
                                           REPLACED_ASSOC_DATAf, fval);
                soc_mem_field_get(unit, EXT_L2_ENTRY_1m, l2_entry.ext_l2_entry_1.entry_data,
                                  DATAf, fval1);
                station_move = sal_memcmp(fval1, fval, assoc_data_length);               
            } else if (mem_type == EXT_L2_ENTRY_2m) {
                assoc_data_length = BITS2BYTES(soc_mem_field_length(unit, EXT_L2_ENTRY_2m, 
                                                                    DATAf));
                soc_L2_MOD_FIFOm_field_get(unit, (uint32 *)entry,
                                           REPLACED_ASSOC_DATAf, fval);
                soc_mem_field_get(unit, EXT_L2_ENTRY_2m, l2_entry.ext_l2_entry_2.entry_data,
                                  DATAf, fval1);
                station_move = sal_memcmp(fval1, fval, assoc_data_length);                
            }
            if (station_move) {
                soc_l2_entry_callback(unit, 0, mem_type, &old_l2_entry, &l2_entry);
            }
	} else {
            soc_l2_entry_callback(unit, 0, mem_type, NULL, &l2_entry);
        }
        break;
    default:
        break;
    }
    if (bsl_check(bslLayerSoc, bslSourceDma, bslSeverityVerbose, unit)) {
        soc_mem_entry_dump(unit, L2_MOD_FIFOm, entry, BSL_VERBOSE|BSL_LS_SOC_DMA);
        LOG_VERBOSE(BSL_LS_SOC_DMA, (BSL_META_U(unit, "\n")));
    }
}

int
soc_tr3_l2_overflow_entry_get(int unit, l2_combo_entry_t *entry, soc_mem_t *mem_type)
{
    SOC_IF_ERROR_RETURN
        (READ_L2_LEARN_INSERT_FAILUREm(unit, COPYNO_ALL, 0, entry->words));
    if (soc_L2_ENTRY_1m_field32_get(unit, entry->words, WIDEf)) {
        *mem_type = L2_ENTRY_2m;
    } else {
        *mem_type = L2_ENTRY_1m;
    }
    return SOC_E_NONE;
}

int
soc_tr3_l2_overflow_disable(int unit)
{
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ISM_INTR_MASKr, REG_PORT_ANY,
                                L2_LEARN_INSERT_FAILUREf, 0));
    /* Mark as inactive */
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->l2_overflow_active = FALSE;
    SOC_CONTROL_UNLOCK(unit);
    /* But don't disable interrupt as its shared by various ISM events */
    return SOC_E_NONE;
}

int
soc_tr3_l2_overflow_enable(int unit)
{
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ISM_INTR_MASKr, REG_PORT_ANY,
                                L2_LEARN_INSERT_FAILUREf, 1));
    /* Mark as active */
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->l2_overflow_active = TRUE;
    SOC_CONTROL_UNLOCK(unit);
    /* Enable interrupt */
#ifdef BCM_CMICM_SUPPORT
    (void)soc_cmicm_intr1_enable(unit, _SOC_TR3_ISM_INTR_MASK);
#endif
    return SOC_E_NONE;
}

int
soc_tr3_l2_overflow_fill(int unit, uint8 zeros_or_ones)
{
    l2_learn_insert_failure_entry_t entry;
    
    if (zeros_or_ones) {
        sal_memset(&entry, 0xffffffff, sizeof(entry));
        SOC_IF_ERROR_RETURN
            (WRITE_L2_LEARN_INSERT_FAILUREm(unit, COPYNO_ALL, 0, &entry));
    } else {
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, L2_LEARN_INSERT_FAILUREm, COPYNO_ALL, FALSE));
    }    
    return SOC_E_NONE;
}

void
soc_tr3_l2_overflow_interrupt_handler(int unit)
{
    l2_combo_entry_t entry;
    soc_mem_t mem_type = INVALIDm;

    if (!SOC_CONTROL(unit)->l2_overflow_active) {
        LOG_ERROR(BSL_LS_SOC_L2, \
                  (BSL_META_U(unit, \
                              "Received L2 overflow event with no app handler or"\
                              " processing inactive !!\n")));
    }
    if (soc_tr3_l2_overflow_disable(unit)) {
        return;
    }
    if (soc_tr3_l2_overflow_entry_get(unit, &entry, &mem_type)) {
        return;
    }
    if (mem_type == INVALIDm) {
        return;
    }
    /* Callback */
    soc_l2_entry_callback(unit, SOC_L2X_ENTRY_OVERFLOW, mem_type, NULL, &entry);
}

int
soc_tr3_l2_overflow_stop(int unit)
{
    if (!SOC_CONTROL(unit)->l2_overflow_enable) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(soc_tr3_l2_overflow_fill(unit, 1));
    SOC_IF_ERROR_RETURN(soc_tr3_l2_overflow_disable(unit));
    return SOC_E_NONE;
}

int
soc_tr3_l2_overflow_start(int unit)
{
    if (!SOC_CONTROL(unit)->l2_overflow_enable) {
        return SOC_E_NONE;
    }
    if (SOC_CONTROL(unit)->l2_overflow_active) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(soc_tr3_l2_overflow_fill(unit, 0));
    SOC_IF_ERROR_RETURN(soc_tr3_l2_overflow_enable(unit));
    return SOC_E_NONE;
}

/*
 * External L2 Bulk Delete/Replace Acceleration
 *
 * Hardware scanning of the external L2 table for bulk delete and
 * bulk replace can be a little slow with large L2 tables.  This
 * code caches enough information on each entry in order to do a
 * quick software walk of the table looking for matches.
 *
 * There are per-vlan min and max indices that are kept so that for
 * per-vlan or per-vlan/port scans we only need to look at a subset of
 * the entries.
 */
STATIC int
_soc_tr3_l2e_ppa_init(int unit)
{
    _soc_tr3_l2e_ppa_info_t *einfo;
    _soc_tr3_l2e_ppa_vlan_t *evlan;
    int                     vlan, esize;

    if (SOC_CONTROL(unit)->ext_l2_ppa_info != NULL) {
        sal_free(SOC_CONTROL(unit)->ext_l2_ppa_info);
        SOC_CONTROL(unit)->ext_l2_ppa_info = NULL;
    }
    if (SOC_CONTROL(unit)->ext_l2_ppa_vlan != NULL) {
        sal_free(SOC_CONTROL(unit)->ext_l2_ppa_vlan);
        SOC_CONTROL(unit)->ext_l2_ppa_vlan = NULL;
    }
    if (SOC_CONTROL(unit)->ext_l2_ppa_info_2 != NULL) {
        sal_free(SOC_CONTROL(unit)->ext_l2_ppa_info_2);
        SOC_CONTROL(unit)->ext_l2_ppa_info_2 = NULL;
    }
    if (SOC_CONTROL(unit)->ext_l2_ppa_vlan_2 != NULL) {
        sal_free(SOC_CONTROL(unit)->ext_l2_ppa_vlan_2);
        SOC_CONTROL(unit)->ext_l2_ppa_vlan_2 = NULL;
    }

    /* EXT_L2_ENTRY_1 */
    esize = soc_mem_index_count(unit, EXT_L2_ENTRY_1m) *
                sizeof(_soc_tr3_l2e_ppa_info_t);
    einfo = sal_alloc(esize, "_soc_tr3_l2e_ppa_info 1");
    if (einfo == NULL) {
        return SOC_E_MEMORY;
    }

    evlan = sal_alloc(sizeof(_soc_tr3_l2e_ppa_vlan_t),
                      "_soc_tr3_l2e_ppa_vlan 1");
    if (evlan == NULL) {
        sal_free(einfo);
        return SOC_E_MEMORY;
    }
    sal_memset(einfo, 0, esize);
    for (vlan = 0; vlan <= VLAN_ID_MAX; vlan++) {
        evlan->vlan_min[vlan] = -1;
        evlan->vlan_max[vlan] = -1;
    }
    SOC_CONTROL(unit)->ext_l2_ppa_info = einfo;
    SOC_CONTROL(unit)->ext_l2_ppa_vlan = evlan;
    
    /* EXT_L2_ENTRY_2 */
    esize = soc_mem_index_count(unit, EXT_L2_ENTRY_2m) *
                sizeof(_soc_tr3_l2e_ppa_info_t);
    einfo = sal_alloc(esize, "_soc_tr3_l2e_ppa_info 2");
    if (einfo == NULL) {
        return SOC_E_MEMORY;
    }
    evlan = sal_alloc(sizeof(_soc_tr3_l2e_ppa_vlan_t),
                      "_soc_tr3_l2e_ppa_vlan 2");
    if (evlan == NULL) {
        sal_free(einfo);
        return SOC_E_MEMORY;
    }
    sal_memset(einfo, 0, esize);
    for (vlan = 0; vlan <= VLAN_ID_MAX; vlan++) {
        evlan->vlan_min[vlan] = -1;
        evlan->vlan_max[vlan] = -1;
    }
    SOC_CONTROL(unit)->ext_l2_ppa_info_2 = einfo;
    SOC_CONTROL(unit)->ext_l2_ppa_vlan_2 = evlan;
    
    SOC_CONTROL(unit)->ext_l2_ppa_threshold = 
        soc_property_get(unit, spn_EXT_L2_USE_HARDWARE_REPLACE_THRESHOLD, 
                         10000);
    
    return SOC_E_NONE;
}

STATIC int
_soc_tr3_l2e_ppa_update(int unit, soc_mem_t mem, int index, uint32 *entry)
{
    _soc_tr3_l2e_ppa_info_t *ppa_info;
    _soc_tr3_l2e_ppa_vlan_t *ppa_vlan;
    uint32      entvalue;
    uint32      l2data;
    sal_mac_addr_t   l2mac;
    vlan_id_t   vlan, oldvlan;
    int         vlan_min, vlan_max;
    uint32      vdata, vmask;

    if (SOC_CONTROL(unit)->ext_l2_ppa_info == NULL) {
        if (!soc_mem_is_valid(unit, mem) ||
            soc_mem_index_count(unit, mem) <= 0) {
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_soc_tr3_l2e_ppa_init(unit));
    }

    ppa_info = SOC_CONTROL(unit)->ext_l2_ppa_info;
    ppa_vlan = SOC_CONTROL(unit)->ext_l2_ppa_vlan;

    vlan = 0;
    entvalue = 0;
    if (entry != NULL) {
        l2data = soc_mem_field32_get(unit, mem, entry, FREEf);
        if (l2data != 1) {
            entvalue |= _SOC_TR3_L2E_VALID;
        }
        l2data = soc_mem_field32_get(unit, mem, entry, STATIC_BITf);
        if (l2data != 0) {
            entvalue |= _SOC_TR3_L2E_STATIC;
        }
        l2data = soc_mem_field32_get(unit, mem, entry, VLAN_IDf);
        vlan = l2data;
        entvalue |= (l2data & _SOC_TR3_L2E_VLAN_MASK) << _SOC_TR3_L2E_VLAN_SHIFT;
        if (soc_mem_field32_get(unit, mem, entry, DEST_TYPEf) == 0x1) {
            l2data = soc_mem_field32_get(unit, mem, entry, TGIDf);
            entvalue |= _SOC_TR3_L2E_T |
                ((l2data & _SOC_TR3_L2E_TRUNK_MASK) << _SOC_TR3_L2E_TRUNK_SHIFT);
        } else if (soc_mem_field32_get(unit, mem, entry, DEST_TYPEf) == 0x2) {
            l2data = soc_mem_field32_get(unit, mem, entry, DESTINATIONf);
            entvalue |= _SOC_TR3_L2E_VP |
                ((l2data & _SOC_TR3_L2E_DESTINATION_MASK) << 
                 _SOC_TR3_L2E_DESTINATION_SHIFT);
        } else {
            l2data = soc_mem_field32_get(unit, mem, entry, MODULE_IDf);
            entvalue |=
                (l2data & _SOC_TR3_L2E_MOD_MASK) << _SOC_TR3_L2E_MOD_SHIFT;
            l2data = soc_mem_field32_get(unit, mem, entry, PORT_NUMf);
            entvalue |=
                (l2data & _SOC_TR3_L2E_PORT_MASK) << _SOC_TR3_L2E_PORT_SHIFT;
        }
        soc_mem_mac_addr_get(unit, mem, entry, MAC_ADDRf, l2mac);
    } else {
        sal_memset(l2mac, 0, sizeof(l2mac));
    }
    oldvlan = (ppa_info[index].data >> 16) & 0xfff;
    ppa_info[index].data = entvalue;
    sal_memcpy(ppa_info[index].mac, l2mac, sizeof(sal_mac_addr_t));

    if (vlan != oldvlan) {      /* update vlan_min/max indices */
        if (oldvlan != 0) {
            vlan_min = ppa_vlan->vlan_min[oldvlan];
            vlan_max = ppa_vlan->vlan_max[oldvlan];
            vdata = oldvlan << _SOC_TR3_L2E_VLAN_SHIFT;
            vmask = _SOC_TR3_L2E_VLAN_MASK << _SOC_TR3_L2E_VLAN_SHIFT;

            /* deleted last one? */
            if (index == vlan_min && index == vlan_max) {
                ppa_vlan->vlan_min[oldvlan] = -1;
                ppa_vlan->vlan_max[oldvlan] = -1;
            } else if (index == vlan_min) {
                for (; vlan_min <= vlan_max; vlan_min++) {
                    entvalue = ppa_info[vlan_min].data;
                    if ((entvalue & vmask) == vdata) {
                        break;
                    }
                }
                ppa_vlan->vlan_min[oldvlan] = vlan_min;
            } else if (index == vlan_max) {
                for (; vlan_min <= vlan_max; vlan_max--) {
                    entvalue = ppa_info[vlan_max].data;
                    if ((entvalue & vmask) == vdata) {
                        break;
                    }
                }
                ppa_vlan->vlan_max[oldvlan] = vlan_max;
            }

        }
        if (vlan != 0) {
            vlan_min = ppa_vlan->vlan_min[vlan];
            vlan_max = ppa_vlan->vlan_max[vlan];
            if (vlan_min < 0 || index < vlan_min) {
                ppa_vlan->vlan_min[vlan] = index;
            }
            if (vlan_max < 0 || index > vlan_max) {
                ppa_vlan->vlan_max[vlan] = index;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "tr3_l2e_ppa %d: index=%d oldvlan=%d min:max %d:%d\n"),
                 mem == EXT_L2_ENTRY_1m ? 1 : 2,
                 index, oldvlan, ppa_vlan->vlan_min[oldvlan],
                 ppa_vlan->vlan_max[oldvlan]));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "tr3_l2e_ppa %d: ent=0x%x mac=%x:%x:%x:%x:%x:%x "
                            "vlan=%d min:max %d:%d\n"),
                 mem == EXT_L2_ENTRY_1m ? 1 : 2,
                 ppa_info[index].data,
                 l2mac[0], l2mac[1], l2mac[2], l2mac[3], l2mac[4], l2mac[5],
                 vlan, ppa_vlan->vlan_min[vlan],
                 ppa_vlan->vlan_max[vlan]));
    return SOC_E_NONE;
}

int
soc_triumph3_ext_l2_entry_update(int unit, soc_mem_t mem, int index, void *entry)
{
    return _soc_tr3_l2e_ppa_update(unit, mem, index, entry);
}

#endif /* BCM_TRIUMPH3_SUPPORT */

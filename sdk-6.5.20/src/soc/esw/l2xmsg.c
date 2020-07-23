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
#include <shared/bsl.h>
#include <sal/core/spl.h>
#include <sal/core/time.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/l2x.h>
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <soc/tomahawk.h>
#endif
#if defined(BCM_MONTEREY_SUPPORT)
#include <soc/monterey.h>
#endif
#if defined(BCM_APACHE_SUPPORT)
#include <soc/apache.h>
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
#include <soc/hurricane3.h>
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>
#endif
#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/hurricane4.h>
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_XGS_SWITCH_SUPPORT

#define L2X_ENTRY_EQL(unit, e1, e2, sz, bits) \
    (!_soc_mem_cmp_l2x_sync(unit, (void *) e1, (void *) e2, sz, bits))

#define L2X_IS_VALID_ENTRY(_u, _mem, _e, bitf)  \
           soc_mem_field32_get((_u), (_mem), (_e), bitf)

#ifdef BCM_TRIUMPH3_SUPPORT
extern int
soc_tr3_l2x_running(int unit, uint32 *flags, sal_usecs_t *interval);
extern int
soc_tr3_l2x_start(int unit, uint32 flags, sal_usecs_t interval);
extern int
soc_tr3_l2x_stop(int unit);
#endif

STATIC void _soc_l2x_thread(void *unit_vp);

/****************************************************************************
 *
 * L2X Message Registration
 */

#define L2X_CB_MAX      3

typedef struct l2x_cb_entry_s {
    soc_l2x_cb_fn       fn;
    void                *fn_data;
} l2x_cb_entry_t;

typedef struct l2x_data_s {
    l2x_cb_entry_t              cb[L2X_CB_MAX];
    int                         cb_count;
    soc_mem_t                   l2mem;
    int                         entry_bytes;
    int                         entry_words;
    uint32                      *shadow_tab;
    uint32                      *chunk_buf;
    SHR_BITDCL                  *delete_map;
    SHR_BITDCL                  *callback_map;
    SHR_BITDCL                  *sr_ref_map;
} l2x_data_t;

STATIC l2x_data_t l2x_data[SOC_MAX_NUM_DEVICES];

#define L2X_ENTRY_CALLBACK_SET(_u_, _index_)                \
{                                                           \
    SHR_BITSET(l2x_data[(_u_)].callback_map, (_index_));    \
    LOG_VERBOSE(BSL_LS_SOC_ARL, \
                (BSL_META_U(unit, \
                            "set_entry_callback: u:%d i=%d\n"), \
                            _u_, _index_));                             \
}

#define L2X_ENTRY_DELETED_SET(_u_, _index_)                 \
{                                                           \
    SHR_BITSET(l2x_data[(_u_)].delete_map, (_index_));      \
    LOG_VERBOSE(BSL_LS_SOC_ARL, \
                (BSL_META_U(unit, \
                            "set_entry_deleted: u:%d i=%d\n"),  \
                            _u_, _index_));                             \
}

#define L2X_IS_CPU_DELETED(_u_, _index_)                    \
            SHR_BITGET(l2x_data[(_u_)].delete_map, (_index_))

#define SOC_L2X_TGID_TRUNK_INDICATOR(unit) \
        SOC_IS_RAVEN(unit) ? 0x40 : 0x20
#define SOC_L2X_TGID_PORT_TRUNK_MASK           0x1f
#define SOC_L2X_TGID_PORT_TRUNK_MASK_HI        0x60

/* Get L2x mode from soc property */
int
soc_l2x_mode_cfg_get(int unit)
{
    int mode, def_mode = L2MODE_POLL;

    if (!(SAL_BOOT_BCMSIM || SAL_BOOT_XGSSIM) &&
        soc_feature(unit, soc_feature_l2_modfifo) &&
        soc_feature(unit, soc_feature_l2_modfifo_def)) {
        def_mode = L2MODE_FIFO;
    }

    mode = soc_property_get(unit, spn_L2XMSG_MODE, def_mode);

    return mode;
}

/*
 * Function:
 *	soc_l2x_register
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
soc_l2x_register(int unit, soc_l2x_cb_fn fn, void *fn_data)
{
    l2x_data_t      *ad = &l2x_data[unit];
    int                 i;
#ifdef BCM_XGS3_SWITCH_SUPPORT
    soc_control_t	*soc = SOC_CONTROL(unit);
    int                 mode;

    mode = soc_l2x_mode_cfg_get(unit);

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
 *	soc_l2x_unregister
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
soc_l2x_unregister(int unit, soc_l2x_cb_fn fn, void *fn_data)
{
    l2x_data_t  *ad = &l2x_data[unit];
    int         i;
    if (fn == NULL) {
        ad->cb_count = 0;
        return SOC_E_NONE;
    }

    for (i = 0; i < ad->cb_count; i++) {
        if ((ad->cb[i].fn == fn &&
             ad->cb[i].fn_data == fn_data)) {

            for (ad->cb_count--; i < ad->cb_count; i++) {
                sal_memcpy(&ad->cb[i], &ad->cb[i + 1],
                           sizeof (l2x_cb_entry_t));
            }

            return SOC_E_NONE;
        }
    }

    return SOC_E_NOT_FOUND;
}

/*
 * Function:
 *	soc_l2x_callback
 * Purpose:
 *	Routine to execute all callbacks on the list.
 * Parameters:
 *	unit - unit number.
 *	entry_del - deleted or updated entry, NULL if none.
 *	entry_add - added or updated entry, NULL if none.
 */

void
soc_l2x_callback(int unit, int flags, l2x_entry_t *entry_del, l2x_entry_t *entry_add)
{
    l2x_data_t      *ad = &l2x_data[unit];
    int         i;
    for (i = 0; i < ad->cb_count; i++) {
        (*ad->cb[i].fn)(unit, flags, entry_del, entry_add, ad->cb[i].fn_data);
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
soc_l2x_running(int unit, uint32 *flags, sal_usecs_t *interval)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
#ifdef BCM_TRIUMPH3_SUPPORT
    if SOC_IS_TRIUMPH3(unit) {
        return soc_tr3_l2x_running(unit, flags, interval);
    }
#endif
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

/* It only can handle delete so far */
int
_soc_l2x_sync_replace(int unit, l2x_entry_t *l2x_match_data,
                      l2x_entry_t *l2x_match_mask, uint32 flags)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 *entry, *match_data, *match_mask;
    int index_max, entry_words, idx, i;

    if (soc->l2x_pid == SAL_THREAD_ERROR) {
        /* thread not running */
        return SOC_E_NONE;
    }

    entry = l2x_data[unit].shadow_tab;
    if (entry == NULL) {
        return SOC_E_NONE;
    }
    match_data = (uint32 *)l2x_match_data;
    match_mask = (uint32 *)l2x_match_mask;

    index_max = soc_mem_index_max(unit, L2Xm);
    entry_words = soc_mem_entry_words(unit, L2Xm);

    (void)sal_sem_take(soc->l2x_lock, sal_sem_FOREVER);

    for (idx = 0; idx <= index_max; idx++, entry += entry_words) {
        for (i = 0; i < entry_words; i++) {
            if ((entry[i] ^ match_data[i]) & match_mask[i]) {
                break;
            }
        }
        if (i != entry_words) {
            continue;
        }
        soc_l2x_sync_delete(unit, entry, idx, flags);
    }

    (void)sal_sem_give(soc->l2x_lock);

    return SOC_E_NONE;
}
 
/*
 * Function:
 * 	_soc_l2x_sync_delete_by
 * Purpose:
 *	Search all the entries in the shadow table based on the 
 *	criteria and mark them as deleted.
 * Parameters:
 * Returns:
 *      SOC_E_XX
 */
int
_soc_l2x_sync_delete_by(int unit, uint32 mod, uint32 port,
                        uint16 vid, uint32 tid, int vfi,
                        uint32 flags, uint32 search_key)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    int             index, max_index, stl2;
    uint32          *entry = NULL;
    soc_field_t     valid_bit = VALIDf;
    int             trunk128, rv = SOC_E_NONE;
    int             static_bit_val;
    uint32          tgid_hi = 0, tgid_lo = 0;
    uint32          tgid_val, port_val, mod_id;
    vlan_id_t       vlan_id;

#ifdef BCM_TRIUMPH3_SUPPORT
    if SOC_IS_TRIUMPH3(unit) {
        return _soc_tr3_l2x_sync_delete_by(unit, mod, port, vid, tid, vfi,
                                           flags, search_key);
    }
#endif 


    if (soc->l2x_pid == SAL_THREAD_ERROR) {
        /* thread not running */
        return SOC_E_NONE;
    }

    max_index = soc_mem_index_max(unit, l2x_data[unit].l2mem);

    stl2 = (flags & SOC_L2X_INC_STATIC) ? 1 : 0;

    (void)sal_sem_take(soc->l2x_lock, sal_sem_FOREVER);

    entry = l2x_data[unit].shadow_tab;
    if (!entry) {
        goto exit;
    }

    if (search_key == SOC_L2X_PORTMOD_DEL) {
        for (index = 0; index < max_index; index++,
            entry += l2x_data[unit].entry_words) {
            if (!L2X_IS_VALID_ENTRY(unit, l2x_data[unit].l2mem, 
                                    entry, valid_bit)) {
                continue;
            }
            if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
                if (soc_mem_field32_get(unit, l2x_data[unit].l2mem, entry,
                                        Tf)) {
                    continue;
                }
                port_val = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                               entry, PORT_NUMf);
            } else {
                port_val = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                               entry, TGID_PORTf);
            }
            mod_id = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                         entry, MODULE_IDf);
            if ((port != port_val) || (mod != mod_id)) {
                continue;
            }

            static_bit_val = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                                 entry, STATIC_BITf);
            if (stl2 || (stl2 == static_bit_val)) {
                soc_l2x_sync_delete(unit, entry, index, flags);
            }
        }
        goto exit;
    }

    if (search_key == SOC_L2X_VLAN_DEL) {
        for (index = 0; index < max_index; index++,
            entry += l2x_data[unit].entry_words) {
            if (!L2X_IS_VALID_ENTRY(unit, l2x_data[unit].l2mem, 
                                    entry, valid_bit)) {
                continue;
            }
            vlan_id = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                          entry, VLAN_IDf);
            if (vid != vlan_id) {
                continue;
            }

            static_bit_val = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                                 entry, STATIC_BITf);
            if (stl2 || (stl2 == static_bit_val)) {
                soc_l2x_sync_delete(unit, entry, index, flags);
            }
        }
        goto exit;
    }

    if (search_key == SOC_L2X_VFI_DEL) {
#ifdef BCM_TRIUMPH_SUPPORT
        int vfi_id;
        for (index = 0; index < max_index; index++,
            entry += l2x_data[unit].entry_words) {
            if (!L2X_IS_VALID_ENTRY(unit, l2x_data[unit].l2mem,
                                    entry, valid_bit)) {
                continue;
            }
            vfi_id = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                         entry, VFIf);
            if (vfi != vfi_id) {
                continue;
            }

            static_bit_val = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                                 entry, STATIC_BITf);
            if (stl2 || (stl2 == static_bit_val)) {
                soc_l2x_sync_delete(unit, entry, index, flags);
            }
        }
#endif
        goto exit;
    }

    if (search_key == SOC_L2X_PORTMOD_VLAN_DEL) {
        for (index = 0; index < max_index; index++,
            entry += l2x_data[unit].entry_words) {
            if (!L2X_IS_VALID_ENTRY(unit, l2x_data[unit].l2mem, 
                                    entry, valid_bit)) {
                continue;
            }
            if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
                if (soc_mem_field32_get(unit, l2x_data[unit].l2mem, entry,
                                        Tf)) {
                    continue;
                }
                port_val = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                               entry, PORT_NUMf);
            } else {
                port_val = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                               entry, TGID_PORTf);
            }
            mod_id = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                         entry, MODULE_IDf);
            vlan_id = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                          entry, VLAN_IDf);
            if ((vid != vlan_id) ||(port != port_val) ||(mod != mod_id)) {
                continue;
            }

            static_bit_val = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                                 entry, STATIC_BITf);
            if (stl2 || (stl2 == static_bit_val)) {
                soc_l2x_sync_delete(unit, entry, index, flags);
            }
        }
        goto exit;
    }

    if ((search_key == SOC_L2X_TRUNK_DEL) || 
        (search_key == SOC_L2X_TRUNK_VLAN_DEL)) {
        trunk128 = soc_feature(unit, soc_feature_trunk_extended);

        if (trunk128) {
            if (!soc_feature(unit, soc_feature_trunk_group_overlay)) {
                tgid_hi = (tid & SOC_L2X_TGID_PORT_TRUNK_MASK_HI) >>
                          SOC_TRUNK_BIT_POS(unit);
                tgid_lo = tid & SOC_L2X_TGID_PORT_TRUNK_MASK;
                tgid_lo |= SOC_L2X_TGID_TRUNK_INDICATOR(unit);
            }

            for (index = 0; index < max_index; index++,
                entry += l2x_data[unit].entry_words) {
                if (!L2X_IS_VALID_ENTRY(unit, l2x_data[unit].l2mem, 
                                        entry, valid_bit)) {
                    continue;
                }
                if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
                    if (!soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                              entry, Tf) ||
                        soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                             entry, TGIDf) != tid) {
                        continue;
                    }
                    if ((search_key == SOC_L2X_TRUNK_VLAN_DEL) && 
                        (vid != soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                                    entry, VLAN_IDf))) {
                        continue;

                    }
                } else {
                    port_val = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                                    entry, TGID_PORTf);
                    tgid_val = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                                    entry, MODULE_IDf);

                    if ((tgid_lo != port_val) ||(tgid_hi != tgid_val)) {
                        continue;
                    }
                    if ((search_key == SOC_L2X_TRUNK_VLAN_DEL) && 
                        (vid != soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                                    entry, VLAN_IDf))) {
                        continue;

                    }
                }

                static_bit_val = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                                     entry, STATIC_BITf);
                if (stl2 || (stl2 == static_bit_val)) {
                    soc_l2x_sync_delete(unit, entry, index, flags);
                }
            }
        } else {
            tid &= SOC_L2X_TGID_PORT_TRUNK_MASK;
            tid |= SOC_L2X_TGID_TRUNK_INDICATOR(unit);

            for (index = 0; index < max_index; index++,
                entry += l2x_data[unit].entry_words) {
                if (!L2X_IS_VALID_ENTRY(unit, l2x_data[unit].l2mem, 
                                        entry, valid_bit)) {
                    continue;
                }
                port_val = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                               entry, TGID_PORTf);
                if (tid != port_val ) {
                    continue;
                }
                if ((search_key == SOC_L2X_TRUNK_VLAN_DEL) && 
                    (vid != soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                                entry, VLAN_IDf))) {
                    continue;

                }
                static_bit_val = soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                                     entry, STATIC_BITf);
                if (stl2 || (stl2 == static_bit_val)) {
                    soc_l2x_sync_delete(unit, entry, index, flags);
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
 * 	soc_l2x_sync_delete
 * Purpose:
 *	sync the SW shadow copy for the deleted entry from HW.
 * Parameters:
 *	unit - unit number.
 *	del_entry - entry deleted from the HW l2X memory
 * Returns:
 *      SOC_E_XX
 */

int
soc_l2x_sync_delete(int unit, uint32 *del_entry, int index, uint32 flags)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    int                 max_index;
    uint32              *tab_p;
    int                  io = 0;
    soc_field_t         valid_bit = VALIDf;

    LOG_VERBOSE(BSL_LS_SOC_ARL,
                (BSL_META_U(unit,
                            "soc_l2x_sync_delete: unit=%d index=%d\n"),
                 unit, index));

    if (soc->l2x_pid == SAL_THREAD_ERROR) {
        /* thread not running */
        return SOC_E_NONE;
    }

    if (l2x_data[unit].l2mem == INVALIDm) {
        return SOC_E_NONE;
    }

    if (soc_feature(unit, soc_feature_base_valid)) {
        valid_bit = BASE_VALIDf;
    }

    max_index = soc_mem_index_max(unit, l2x_data[unit].l2mem);
    if (index > max_index) {
#ifdef BCM_HURRICANE3_SUPPORT
        if ((SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) && 
            (index & SOC_L2X_GEN_RESP_INDEX_L2_OVERFLOW(unit))) {
            /* Entry learnt in L2_ENTRY_OVERFLOW */
            return SOC_E_NONE;
        }
#endif /* BCM_HURRICANE3_SUPPORT */         
        return SOC_E_PARAM;
    }
    if (l2x_data[unit].shadow_tab == NULL) {
        LOG_WARN(BSL_LS_SOC_ARL,
                    (BSL_META_U(unit,
                                "soc_l2x_sync_delete: l2x_data[%d].shadow_tab is NULL!!!\n"),
                     unit));
        return SOC_E_NONE;
    }
    /*
     * validate that the entry in the shadow copy is same as the entry
     * deleted.
     */
    tab_p = l2x_data[unit].shadow_tab + (index * l2x_data[unit].entry_words);
    if (L2X_ENTRY_EQL(unit, tab_p, del_entry,
                WORDS2BYTES(l2x_data[unit].entry_words), 0)) {
        L2X_ENTRY_DELETED_SET(unit, index);
        /* Bitmap that indicates to suppress callback */
        if ((flags & SOC_L2X_NO_CALLBACKS)) {
            L2X_ENTRY_CALLBACK_SET(unit, index);
        }
    }

    if(!(soc_feature(unit, soc_feature_no_l2_del_witk_key_only_match))) {
        /* During a port-move, the data associated with the HW entry and data
           saved in the SW shadow copy can be different, in such a case when
           the user is attempting a delete of the MAC entry, we check the key
           and mark the L2 entry for delete and on the next pass of the L2 sync
           thread, the new entry gets added and shadow copy updated */
        tab_p = l2x_data[unit].shadow_tab +
            (((index/SOC_L2X_BUCKET_SIZE) * SOC_L2X_BUCKET_SIZE) * l2x_data[unit].entry_words);
        for (io = 0; io < SOC_L2X_BUCKET_SIZE; io++) {
            if (soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                    tab_p, valid_bit) &&
                soc_mem_compare_key(unit, l2x_data[unit].l2mem,
                                    tab_p, del_entry) == 0) {
                break;
            }
            tab_p += l2x_data[unit].entry_words;
        }

        if (io < SOC_L2X_BUCKET_SIZE) {
            L2X_ENTRY_DELETED_SET(unit, ((index/SOC_L2X_BUCKET_SIZE) * SOC_L2X_BUCKET_SIZE) + io);
            /* Bitmap that indicates to suppress callback */
            if ((flags & SOC_L2X_NO_CALLBACKS)) {
                L2X_ENTRY_CALLBACK_SET(unit, ((index/SOC_L2X_BUCKET_SIZE) * SOC_L2X_BUCKET_SIZE) + io);
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_l2x_sync_delete_by_port
 * Purpose:
 *	sync the SW shadow copy for the deleted entryies based on port.
 * Parameters:
 *	unit - unit number.
 *	port - port number.
 * Returns:
 *      SOC_E_XX
 */

int
soc_l2x_sync_delete_by_port(int unit, int mod, soc_port_t port,
                            uint32 flags)
{
    return _soc_l2x_sync_delete_by(unit, mod, port, 0, 0, 0,
                                   flags, SOC_L2X_PORTMOD_DEL);
}

/*
 * Function:
 * 	soc_l2x_sync_delete_by_trunk
 * Purpose:
 *	sync the SW shadow copy for the deleted entryies based on trunk id.
 * Parameters:
 *	unit - unit number.
 *	port - port number.
 * Returns:
 *      SOC_E_XX
 */

int
soc_l2x_sync_delete_by_trunk(int unit, int tid, uint32 flags)
{
    return _soc_l2x_sync_delete_by(unit, 0, 0, 0, tid, 0,
                                   flags, SOC_L2X_TRUNK_DEL);
}

/*
 * Function:
 * 	soc_l2x_start
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
soc_l2x_start(int unit, uint32 flags, sal_usecs_t interval)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    int         pri;
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int                 mode;
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if SOC_IS_TRIUMPH3(unit) {
        return soc_tr3_l2x_start(unit, flags, interval);
    }
#endif
    LOG_INFO(BSL_LS_SOC_ARL,
             (BSL_META_U(unit,
                         "soc_l2x_start: unit=%d flags=0x%x interval=%d\n"),
              unit, flags, interval));

    if (!soc_feature(unit, soc_feature_arl_hashed)) {
        return SOC_E_UNAVAIL;
    }

    if (soc->l2x_interval != 0) {
        SOC_IF_ERROR_RETURN(soc_l2x_stop(unit));
    }

    sal_snprintf(soc->l2x_name, sizeof (soc->l2x_name),
                 "bcmL2X.%d", unit);

    if (soc->l2x_pid == SAL_THREAD_ERROR) {
        pri = soc_property_get(unit, spn_L2XMSG_THREAD_PRI, 50);
#ifdef BCM_XGS3_SWITCH_SUPPORT
        mode = soc_l2x_mode_cfg_get(unit);

        if (soc_feature(unit, soc_feature_l2_modfifo) && (mode == L2MODE_FIFO)) {

            SOC_CONTROL_LOCK(unit);
            soc->l2x_mode = mode;
            soc->l2x_flags = flags;
            soc->l2x_interval = interval;

            if (interval == 0) {
                SOC_CONTROL_UNLOCK(unit);
                return SOC_E_NONE;
            }
#ifdef BCM_CMICX_SUPPORT
            if (SOC_IS_TRIDENT3X(unit) || SOC_IS_FIRELIGHT(unit)) {
                _soc_cmicx_l2mod_start(unit, flags, interval);
            } else
#endif
#ifdef BCM_CMICM_SUPPORT
            if (SOC_IS_TD2_TT2(unit) || SOC_IS_KATANA2(unit) || 
                soc_feature(unit, soc_feature_fifo_dma_hu2)) {
                _soc_l2mod_start(unit, flags, interval);
            } else 
#endif
            {
                soc_l2mod_start(unit, flags, interval);
            }
            SOC_CONTROL_UNLOCK(unit);
            l2x_data[unit].l2mem = INVALIDm;
        } else
#endif
        {
            if (soc->l2x_external) {
                /* Not supported for external L2 memory configuration. */
                return SOC_E_NONE;
            }

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
                                             _soc_l2x_thread,
                                             INT_TO_PTR(unit));
            if (soc->l2x_pid == SAL_THREAD_ERROR) {
                LOG_ERROR(BSL_LS_SOC_L2,
                          (BSL_META_U(unit,
                                      "soc_l2x_start: Could not start L2X thread\n")));
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
 * 	soc_l2x_stop
 * Purpose:
 *   	Stop L2X-related thread
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX
 */

int
soc_l2x_stop(int unit)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    int         rv = SOC_E_NONE;
    soc_timeout_t   to;
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int                 mode;
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if SOC_IS_TRIUMPH3(unit) {
        return soc_tr3_l2x_stop(unit);
    }
#endif
    LOG_INFO(BSL_LS_SOC_ARL,
             (BSL_META_U(unit,
                         "soc_l2x_stop: unit=%d\n"), unit));

    SOC_CONTROL_LOCK(unit);
    soc->l2x_interval = 0;  /* Request exit */
    SOC_CONTROL_UNLOCK(unit);

    if (soc->l2x_pid != SAL_THREAD_ERROR) {
        /* Wake up thread so it will check the exit flag */
#ifdef BCM_XGS3_SWITCH_SUPPORT
        mode = soc_l2x_mode_cfg_get(unit);
        if (soc_feature(unit, soc_feature_l2_modfifo) &&
            (mode == L2MODE_FIFO)) {
#ifdef BCM_CMICX_SUPPORT
            if (SOC_IS_TRIDENT3X(unit) || SOC_IS_FIRELIGHT(unit)) {
                _soc_cmicx_l2mod_stop(unit);
            } else
#endif
#ifdef BCM_CMICM_SUPPORT
            if (SOC_IS_TD2_TT2(unit) || SOC_IS_KATANA2(unit) ||
                soc_feature(unit, soc_feature_fifo_dma_hu2)) {
                _soc_l2mod_stop(unit);
            } else 
#endif
            {
                soc_l2mod_stop(unit);
            }
        } else
#endif
        {
            if (NULL != soc->l2x_notify) {
                sal_sem_give(soc->l2x_notify);
            }
        }

        /* Give thread a few seconds to wake up and exit */
        if (SAL_BOOT_SIMULATION) {
            soc_timeout_init(&to, 30 * 1000000, 0);
        } else {
            soc_timeout_init(&to, 15 * 1000000, 0);
        }

        while (soc->l2x_pid != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_L2,
                          (BSL_META_U(unit,
                                      "soc_l2x_stop: thread will not exit\n")));
                rv = SOC_E_INTERNAL;
                break;
            }
        }
    }

    return rv;
}

#if defined(BCM_TRIDENT2_SUPPORT)
STATIC int
_soc_trident2_l2x_sync_multi_buckets(int unit, uint32 *new_entry, 
                                     uint8 shadow_hit_bits, int *mac_moved)
{
    int num_banks = 0;
    int bank_idx = 0;
    int hw_bank = 0;
    int bucket_base = 0;
    int entry_idx = 0;
    int delete_marked, callback_suppress;
    uint32 *shadow_entry = NULL;
    uint32 bucket;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_mem_t l2mem = l2x_data[unit].l2mem;
    int entry_words = l2x_data[unit].entry_words;
    SHR_BITDCL *delete_map = l2x_data[unit].delete_map;
    SHR_BITDCL *callback_map = l2x_data[unit].callback_map;
    soc_field_t l2x_vld = VALIDf;
    int bucket_size = 4; /* 4 entries per bucket */

    if (soc_feature(unit, soc_feature_base_valid)) {
        l2x_vld = BASE_VALIDf;
    }

    SOC_IF_ERROR_RETURN(
        soc_trident2_hash_bank_count_get(unit, l2mem, &num_banks));
    for (bank_idx = 0; bank_idx < num_banks; bank_idx++) {
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_apache_hash_bank_number_get(unit, l2mem, bank_idx,
                                                   &hw_bank));
            SOC_IF_ERROR_RETURN
                (soc_ap_hash_bucket_get(unit, l2mem, hw_bank, new_entry,
                                        &bucket));
            bucket_base = soc_ap_hash_index_get(unit, l2mem, hw_bank, bucket);
        } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_IS_TOMAHAWK3(unit)) {
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk3_hash_bank_number_get(unit, l2mem, bank_idx,
                                                        &hw_bank));
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk3_hash_bucket_get(unit, l2mem, hw_bank,
                                                   new_entry, &bucket));
                bucket_base = soc_tomahawk3_hash_index_get(unit, l2mem, hw_bank,
                                                           bucket);
            } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
            {
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk_hash_bank_number_get(unit, l2mem, bank_idx,
                                                       &hw_bank));
                SOC_IF_ERROR_RETURN
                    (soc_th_hash_bucket_get(unit, l2mem, hw_bank, new_entry,
                                            &bucket));
                bucket_base = soc_th_hash_index_get(unit, l2mem, hw_bank,
                                                    bucket);
            }
        } else
#endif /* BCM_TOMAHAWK_SUPPORT */
        {
            SOC_IF_ERROR_RETURN
                (soc_trident2_hash_bank_number_get(unit, l2mem, bank_idx,
                                                   &hw_bank));
            SOC_IF_ERROR_RETURN
                (soc_hash_bucket_get(unit, l2mem, hw_bank, new_entry,
                                     &bucket));
            bucket_base = soc_hash_index_get(unit, l2mem, hw_bank, bucket);
        }
        shadow_entry = l2x_data[unit].shadow_tab + (bucket_base * entry_words);
        for (entry_idx = 0; entry_idx < bucket_size; entry_idx++) {
            if (soc_mem_field32_get(unit, l2mem, shadow_entry, l2x_vld) &&
                soc_mem_compare_key(unit, l2mem, new_entry, shadow_entry) == 0) {
                break;
            }
            shadow_entry += entry_words;
        }
        if (entry_idx < bucket_size) {
            break;
        }
    }

    if (bank_idx < num_banks) {
        /* This MAC existed previously in the bucket of another bank.
         * Check if everything at the new location is the same (ignoring
         * hit bit).  If so, there is no delete or change processing.
         */
         if (!(shadow_hit_bits & L2X_SHADOW_HIT_BITS)) {
             if (!(shadow_hit_bits & L2X_SHADOW_HIT_DST)) {
                 soc_L2Xm_field32_set(unit, shadow_entry, HITDAf,
                                      soc_L2Xm_field32_get(unit, new_entry,
                                                           HITDAf));
             }
             if (!(shadow_hit_bits & L2X_SHADOW_HIT_SRC)) {
                 soc_L2Xm_field32_set(unit, shadow_entry, HITSAf,
                                      soc_L2Xm_field32_get(unit, new_entry,
                                                           HITSAf));
             }
         }
        /* Check if the entry has been deleted and is marked for
         * delete.
         */
        if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
            return SOC_E_RESOURCE;
        }
        delete_marked = SHR_BITGET(delete_map, bucket_base + entry_idx);
        callback_suppress = SHR_BITGET(callback_map, bucket_base + entry_idx);
        SOC_L2_DEL_SYNC_UNLOCK(soc);
        if (L2X_ENTRY_EQL(unit, shadow_entry, new_entry,
                             WORDS2BYTES(l2x_data[unit].entry_words), 0)) {
            if (delete_marked) {
                /* Entry got learnt again after delete */
                if (callback_suppress) {
                    /* Indicate re-learn */
                    soc_l2x_callback(unit, 0, NULL, (l2x_entry_t *)new_entry);
                } else {
                    /* Indicate delete and re-learn */
                    soc_l2x_callback(unit, 0, (l2x_entry_t *)shadow_entry, NULL);
                    soc_l2x_callback(unit, 0, NULL, (l2x_entry_t *)new_entry);
                }
            }
            /* Invalid entry in shadow table */
            soc_L2Xm_field32_set(unit, shadow_entry, l2x_vld, 0);
            *mac_moved = TRUE;
            return SOC_E_NONE;
        }
        /* Issue change (delete & insert) */
        if (!callback_suppress) {
            soc_l2x_callback(unit, 0, (l2x_entry_t *)shadow_entry,
                             (l2x_entry_t *)new_entry);
        }
        /* Invalid entry in the shadow table */
        soc_L2Xm_field32_set(unit, shadow_entry, l2x_vld, 0);
        *mac_moved = TRUE;
        return SOC_E_NONE;
    }
    *mac_moved = FALSE;
    return SOC_E_NONE;
}
#endif

/*
 * Function:
 *	_soc_l2x_sync_bucket
 * Purpose:
 *	Compare the old contents of a hash bucket (8 entries) to the
 *	next contents, make any ARL callbacks that are necessary,
 *	and sync that bucket in the shadow table.
 * Parameters:
 *	unit - StrataSwitch unit #
 *	old_bucket - Previous bucket contents (SOC_L2X_BUCKET_SIZE entries)
 *	new_bucket - New bucket contents (SOC_L2X_BUCKET_SIZE entries)
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
_soc_l2x_sync_bucket(int unit,
                     uint32 *old_bucket,
                     uint32 *new_bucket,
                     uint8 shadow_hit_bits, int bucket_index,
                     SHR_BITDCL *p_chunk_del_map, 
                     SHR_BITDCL *p_chunk_cb_map,
                     SHR_BITDCL *p_bucket_sr_map)
{
    int         io, in, delete_marked, callback_suppress;
    soc_field_t         valid_bit = VALIDf;
    uint32              *old_p, *new_p;
#if defined(BCM_TRIDENT2_SUPPORT)
    int rv = SOC_E_NONE;
    int mac_moved = 0;
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    int key_type = 0;
    int remote_trunk = 0;
    int trunk = 0;
#endif
#ifdef BCM_TSN_SUPPORT
    int sr_ref_updated = 0;
#endif

    /*
     * Process deletions, which will be any entry that was in the bucket
     * before but whose key is no longer found anywhere in the bucket,
     * and changes, which will be any entry whose key is still found but
     * whose associated data (port, etc.) has changed.
     */

    if (soc_feature(unit, soc_feature_base_valid)) {
        valid_bit = BASE_VALIDf;
    }

    old_p = old_bucket;
    for (io = 0; io < SOC_L2X_BUCKET_SIZE; io++,
        old_p += l2x_data[unit].entry_words) {

        /*
         * No deletion or change is needed for invalid entries.
         */
        if (!soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                 old_p, valid_bit)) {
            continue;
        }

#ifdef BCM_HURRICANE4_SUPPORT
        if (soc_feature(unit, soc_feature_hr4_a0_sobmh_war)) {
            if (HR4_WAR_L2X_SOBMH_KEY_TYPE ==
                soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                    old_p, KEY_TYPEf)) {
                continue;
            }
        }
#endif

        /*
         * Check if the entry has been deleted and is marked for
         * delete.
         */
        delete_marked = SHR_BITGET(p_chunk_del_map, bucket_index + io);
        callback_suppress = SHR_BITGET(p_chunk_cb_map, bucket_index + io);
#ifdef BCM_TSN_SUPPORT
        if (p_bucket_sr_map != NULL) {
            sr_ref_updated = SHR_BITGET(p_bucket_sr_map, io);
        }
#endif /* BCM_TSN_SUPPORT */
        /*
         * Quick check: if the entry is identical, it does not need any
         * deletion or change processing.  It may be valid or not.
         */

        new_p = new_bucket + (io * l2x_data[unit].entry_words);
        if (L2X_ENTRY_EQL(unit, old_p, new_p, 
                    WORDS2BYTES(l2x_data[unit].entry_words), shadow_hit_bits)) {
            if (delete_marked) {
                /* Entry got learnt again after delete */
                if (callback_suppress) {
                    /* Indicate re-learn */
                    soc_l2x_callback(unit, 0, NULL, (l2x_entry_t *) new_p);
                } else {
                    /* Indicate delete and re-learn */
                    soc_l2x_callback(unit, 0, (l2x_entry_t *) old_p, NULL);
                    soc_l2x_callback(unit, 0, NULL, (l2x_entry_t *) new_p);
                }
            }
            continue;
        }

        /*
         * See if the old key still exists anywhere within the bucket.
         */

        new_p = new_bucket;
        for (in = 0; in < SOC_L2X_BUCKET_SIZE; in++) {
            if (soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                    new_p, valid_bit) &&
                soc_mem_compare_key(unit, l2x_data[unit].l2mem,
                                    old_p, new_p) == 0) {
                break;
            }
            new_p += l2x_data[unit].entry_words;
        }

        if (in == SOC_L2X_BUCKET_SIZE) {
            /* It doesn't, so issue a delete. */
            if (!callback_suppress) {
#if defined(BCM_TSN_SUPPORT)
                /* Decrease TSN/SR flowset reference count for L2 deletion */
                if (soc_feature(unit, soc_feature_tsn)) {
                    if (!sr_ref_updated) {
                        soc_l2x_callback(unit,
                                         SOC_L2X_ENTRY_TSN_SR_FLOWSET_REF_CNT,
                                         (l2x_entry_t *)old_p, NULL);
                        if (p_bucket_sr_map != NULL) {
                            SHR_BITCLR(p_bucket_sr_map, io);
                        }
                    }
                }
#endif /* BCM_TSN_SUPPORT */
#ifdef BCM_XGS_SWITCH_SUPPORT
#if defined(BCM_TOMAHAWK3_SUPPORT)
                if (SOC_IS_TOMAHAWK3(unit)) {
                    int rv1 = SOC_E_NONE;
                    int idx = -1;

                    /* Entry not found in this bucket. Search in L2 table to see
                     * if it was shuffled. It it was shuffled, the same entry
                     * will be present at some other location. If the search
                     * did not find it, it means the entry was deleted, so  we
                     * can issue a 'delete' callback. This code is required
                     * because TH3 does not have mod fifo; in legacy devices,
                     * mod fifo is configured to supress callbacks due to
                     * internal (within SDK) shuffling of L2 entries
                     */
                    rv1 = soc_mem_generic_lookup(unit, L2Xm, MEM_BLOCK_ANY, 0,
                                                 old_p, NULL, &idx);
                    if (rv1 == SOC_E_NOT_FOUND) {
                        /* Check if application callback feature is enabled.
                         * If so, do additional processing, otherwise issue a
                         * delete callback with flags
                         */
                        if (soc_th3_l2x_appl_callback_ready(unit)) {
                            int found = FALSE, enable = 0;
                            sal_mac_addr_t mac;
                            bcm_vlan_t vlan;

                            soc_L2Xm_mac_addr_get(unit, (l2x_entry_t *)old_p,
                                MAC_ADDRf, mac);
                            vlan = soc_L2Xm_field32_get(unit,
                                       (l2x_entry_t *)old_p, VLAN_IDf);

                           /* Check if this entry is in callback db */
                           (void)soc_th3_l2x_appl_callback_entry_lookup(unit,
                                     mac, vlan, &found, &enable);

                           if (found == TRUE) {
                               /* Delete entry from application callback db */
                               (void)soc_th3_l2x_appl_callback_entry_delete(
                                         unit, mac, vlan);

                                soc_l2x_callback(unit,
                                    SOC_L2X_ENTRY_DELETE|
                                    SOC_L2X_ENTRY_NO_ACTION,
                                    (l2x_entry_t *) old_p,
                                    NULL);
                            } else {
                                /* Issue callback since entry was added by other
                                 * source (example, learning)
                                 */
                                soc_l2x_callback(unit, 0, (l2x_entry_t *) old_p,
                                    NULL);
                            }
                        } else {
                            /* Issue callback if the callback feature is not
                             * enabled
                             */
                            soc_l2x_callback(unit, 0, (l2x_entry_t *) old_p,
                                NULL);
                        }
                    }

                } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
#endif /* BCM_XGS_SWITCH_SUPPORT */
                {
                    soc_l2x_callback(unit, 0, (l2x_entry_t *) old_p, NULL);
                }
            }
            continue;
        }

        /*
         * Check if everything at the new location is the same (ignoring
         * hit bit).  If so, there is no delete or change processing.
         */

        if (!(shadow_hit_bits & L2X_SHADOW_HIT_BITS)) {
            if (!(shadow_hit_bits & L2X_SHADOW_HIT_DST)) { 
                soc_L2Xm_field32_set(unit, old_p, HITDAf,
                                     soc_L2Xm_field32_get(unit, new_p,
                                                          HITDAf));
            }
            if (!(shadow_hit_bits & L2X_SHADOW_HIT_SRC)) {
                soc_L2Xm_field32_set(unit, old_p, HITSAf,
                                     soc_L2Xm_field32_get(unit, new_p,
                                                          HITSAf));
            }
        }

        if (L2X_ENTRY_EQL(unit, old_p, new_p, 
                 WORDS2BYTES(l2x_data[unit].entry_words), shadow_hit_bits)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_APACHE_SUPPORT)
            if (soc_feature(unit, soc_feature_no_l2_remote_trunk)) {
                key_type = soc_L2Xm_field32_get(unit, new_p, KEY_TYPEf);
                trunk = soc_L2Xm_field32_get(unit, new_p, Tf);
                remote_trunk = soc_L2Xm_field32_get(unit, new_p, REMOTE_TRUNKf);

                if (remote_trunk && trunk && (key_type == 0)) {
                    /* Clear REMOTE_TRUNK in S/W new chunk */
                    soc_L2Xm_field32_set(unit, new_p, REMOTE_TRUNKf, 0);

                    /* Clear REMOTE_TRUNK in H/W table */
                    soc_mem_generic_insert(unit, L2Xm, MEM_BLOCK_ANY,
                                           0, new_p, NULL, NULL);
                    remote_trunk = 0;
                    trunk = 0;
                }
            }
#endif
            if (delete_marked) {
                /* Entry got learnt again after delete */
                if (callback_suppress) {
                    /* Indicate re-learn */
                    soc_l2x_callback(unit, 0, NULL, (l2x_entry_t *) new_p);
                } else {
                    /* Indicate delete and re-learn */
                    soc_l2x_callback(unit, 0, (l2x_entry_t *) old_p, NULL);
                    soc_l2x_callback(unit, 0, NULL, (l2x_entry_t *) new_p);
                }
            }
            continue;
        }

#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_APACHE_SUPPORT)
        if (soc_feature(unit, soc_feature_no_l2_remote_trunk)) {
            key_type = soc_L2Xm_field32_get(unit, new_p, KEY_TYPEf);
            trunk = soc_L2Xm_field32_get(unit, new_p, Tf);
            remote_trunk = soc_L2Xm_field32_get(unit, new_p, REMOTE_TRUNKf);

            if (remote_trunk && trunk && (key_type == 0)) {
                /* Clear REMOTE_TRUNK in S/W new chunk */
                soc_L2Xm_field32_set(unit, new_p, REMOTE_TRUNKf, 0);

                /* Clear REMOTE_TRUNK in H/W table */
                soc_mem_generic_insert(unit, L2Xm, MEM_BLOCK_ANY,
                                       0, new_p, NULL, NULL);
                remote_trunk = 0;
                trunk = 0;
            }
        }
#endif
        if (delete_marked) {
            /* Entry got learnt again at another port after delete */
            if (callback_suppress) {
                /* Indicate re-learn */
                soc_l2x_callback(unit, 0, NULL, (l2x_entry_t *) new_p);
            } else {
                /* Indicate delete and re-learn */
                soc_l2x_callback(unit, 0, (l2x_entry_t *) old_p, NULL);
                soc_l2x_callback(unit, 0, NULL, (l2x_entry_t *) new_p);
            }
        } else {
            /* Issue change (delete & insert) */
            if (!callback_suppress) {
#if defined(BCM_TSN_SUPPORT)
                /* Update TSN/SR flowset reference count for L2 changes */
                if (soc_feature(unit, soc_feature_tsn)) {
                    if (!sr_ref_updated) {
                        soc_l2x_callback(unit,
                                         SOC_L2X_ENTRY_TSN_SR_FLOWSET_REF_CNT,
                                         (l2x_entry_t *)old_p,
                                         (l2x_entry_t *)new_p);
                        if (p_bucket_sr_map != NULL) {
                            SHR_BITCLR(p_bucket_sr_map, io);
                        }
                    }
                }
#endif /* BCM_TSN_SUPPORT */
                soc_l2x_callback(unit, 0, (l2x_entry_t *) old_p,
                                 (l2x_entry_t *) new_p);
            }
        }
    }

    /*
     * Process insertions, which will be any entry whose key was not in
     * the bucket before, but is now.
     */

    new_p = new_bucket;
    for (in = 0; in < SOC_L2X_BUCKET_SIZE; in++,
        new_p += l2x_data[unit].entry_words) {

#ifdef BCM_TSN_SUPPORT
        if (p_bucket_sr_map != NULL) {
            sr_ref_updated = SHR_BITGET(p_bucket_sr_map, in);
        }
#endif
        /*
         * Quick check: if the entry is identical, it does not need any
         * insertion processing.  It may be valid or not.
         */
        old_p = old_bucket + (in * l2x_data[unit].entry_words);
        if (L2X_ENTRY_EQL(unit, old_p, new_p,
                 WORDS2BYTES(l2x_data[unit].entry_words), 0)) {
            continue;
        }

        /*
         * No insert needed for invalid entries.
         */

        if (!soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                 new_p, valid_bit)) {
            continue;
        }

#ifdef BCM_HURRICANE4_SUPPORT
        if (soc_feature(unit, soc_feature_hr4_a0_sobmh_war)) {
            if (HR4_WAR_L2X_SOBMH_KEY_TYPE ==
                soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                    new_p, KEY_TYPEf)) {
                continue;
            }
        }
#endif

        /*
         * If the same key already existed elsewhere within the bucket,
         * it has already been taken care of by the change processing above.
         */

        old_p = old_bucket;
        for (io = 0; io < SOC_L2X_BUCKET_SIZE; io++) {
            if (soc_mem_field32_get(unit, l2x_data[unit].l2mem,
                                    old_p, valid_bit) &&
                soc_mem_compare_key(unit, l2x_data[unit].l2mem,
                                    new_p, old_p) == 0) {
                break;
            }
            old_p += l2x_data[unit].entry_words;
        }

        if (io < SOC_L2X_BUCKET_SIZE) {
            continue;
        }

        if (soc_feature(unit, soc_feature_ppa_bypass)) {
            if (SOC_CONTROL(unit)->l2x_ppa_bypass == FALSE &&
                soc_mem_field32_get(unit, L2Xm, new_p, KEY_TYPEf) !=
                TR_L2_HASH_KEY_TYPE_BRIDGE) {
                SOC_CONTROL(unit)->l2x_ppa_bypass = TRUE;
            }
        }

#if defined(BCM_TRIDENT2_SUPPORT)
        /* Check whether this entry in the new bucket existed in 
         * other buckets in another banks previously.
         */
        if (SOC_IS_TD2_TT2(unit) &&
        	  soc_feature(unit, soc_feature_shared_hash_mem)) {
            rv = _soc_trident2_l2x_sync_multi_buckets(unit, new_p, 
                                                      shadow_hit_bits, 
                                                      &mac_moved);
            if (SOC_SUCCESS(rv)) {
                if (mac_moved == TRUE) {
                    continue;
                }
            }
        }
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_APACHE_SUPPORT)
        if (soc_feature(unit, soc_feature_no_l2_remote_trunk)) {
            key_type = soc_L2Xm_field32_get(unit, new_p, KEY_TYPEf);
            trunk = soc_L2Xm_field32_get(unit, new_p, Tf);
            remote_trunk = soc_L2Xm_field32_get(unit, new_p, REMOTE_TRUNKf);

            if (remote_trunk && trunk && (key_type == 0)) {
                /* Clear REMOTE_TRUNK in S/W new chunk */
                soc_L2Xm_field32_set(unit, new_p, REMOTE_TRUNKf, 0);

                /* Clear REMOTE_TRUNK in H/W table */
                soc_mem_generic_insert(unit, L2Xm, MEM_BLOCK_ANY,
                                       0, new_p, NULL, NULL);
                remote_trunk = 0;
                trunk = 0;
            }
        }
#endif
#if defined(BCM_TSN_SUPPORT)
        /* Increase TSN/SR flowset reference count for L2 insertions */
        if (soc_feature(unit, soc_feature_tsn)) {
            if (!sr_ref_updated) {
                soc_l2x_callback(unit,
                                 SOC_L2X_ENTRY_TSN_SR_FLOWSET_REF_CNT,
                                 NULL, (l2x_entry_t *)new_p);
                if (p_bucket_sr_map != NULL) {
                    SHR_BITCLR(p_bucket_sr_map, in);
                }
            }
        }
#endif /* BCM_TSN_SUPPORT */

#ifdef BCM_XGS_SWITCH_SUPPORT
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(unit)) {
            /* If callback needs to be supressed for entries added
             * through l2 address add API, check in the internal
             * database if mac-vlan was recorded. If so, do not issue
             * callback
             */
            if (soc_th3_l2x_appl_callback_ready(unit)) {
                sal_mac_addr_t mac;
                bcm_vlan_t vlan;
                int found = 0;
                int enable = -1;

                soc_L2Xm_mac_addr_get(unit, (l2x_entry_t *)new_p, MAC_ADDRf,
                                      mac);
                vlan = soc_L2Xm_field32_get(unit, (l2x_entry_t *)new_p,
                                            VLAN_IDf);
                (void)soc_th3_l2x_appl_callback_entry_lookup(unit, mac, vlan,
                                                             &found, &enable);

                /* Entry not found means the entry was added by another
                 * source, for example, through learning. We issue callback in
                 * such a case
                 */
                if (found == FALSE) {
                    soc_l2x_callback(unit, 0, NULL, (l2x_entry_t *)new_p);
                } else {
                    /* If entry is found in internal database, stop issuing
                     * callback
                     */
                    soc_l2x_callback(unit,
                        SOC_L2X_ENTRY_INSERT|SOC_L2X_ENTRY_NO_ACTION,
                        NULL, (l2x_entry_t *)new_p);
                }
            } else {
                soc_l2x_callback(unit, 0, NULL, (l2x_entry_t *) new_p);
            }
        } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
#endif /* BCM_XGS_SWITCH_SUPPORT */
        {
            soc_l2x_callback(unit, 0, NULL, (l2x_entry_t *) new_p);
        }
    }

    /* Sync shadow copy */

    sal_memcpy(old_bucket, new_bucket,
               SOC_L2X_BUCKET_SIZE * l2x_data[unit].entry_words * 4);
}

/*
 * Function:
 * 	_soc_l2x_thread
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
_soc_l2x_thread(void *unit_vp)
{
    int         unit = PTR_TO_INT(unit_vp);
    soc_control_t   *soc = SOC_CONTROL(unit);
    int         rv;
    uint32      *shadow_tab = NULL, *chunk_buf = NULL;
    SHR_BITDCL          *delete_map = NULL;
    SHR_BITDCL          *chunk_delete_map = NULL;
    SHR_BITDCL          *callback_map = NULL;
    SHR_BITDCL          *chunk_callback_map = NULL;
#ifdef BCM_TSN_SUPPORT
    SHR_BITDCL          *sr_ref_map = NULL;
    SHR_BITDCL          *bucket_sr_ref_map = NULL;
    int         is_tsn_or_sr = FALSE; /* TSN or SR support */
#endif
    int         index_count;
    int         chunk_count, chunk_size;
    int         chunk_index, bucket_index;
    uint32      *tab_p, *buf_p;
    int         interval;
    sal_usecs_t     stime, etime;

    l2x_data[unit].l2mem = L2Xm;
    l2x_data[unit].entry_bytes = soc_mem_entry_bytes(unit, l2x_data[unit].l2mem);
    l2x_data[unit].entry_words = soc_mem_entry_words(unit, l2x_data[unit].l2mem);

    assert(soc_mem_index_min(unit, l2x_data[unit].l2mem) == 0);
    index_count = soc_mem_index_count(unit, l2x_data[unit].l2mem);
    if (index_count <= 0) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "soc_l2x_thread: table size is 0 \n")));
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                           SOC_SWITCH_EVENT_THREAD_L2X, __LINE__, SOC_E_EMPTY);
        goto cleanup_exit;
    }

    chunk_count = soc_property_get(unit, spn_L2XMSG_CHUNKS, 8);
    chunk_size = index_count / chunk_count;

    assert(chunk_size > 0);
    assert(chunk_size % SOC_L2X_BUCKET_SIZE == 0);

    shadow_tab = sal_alloc(index_count * l2x_data[unit].entry_words * 4,
                           "l2x_old");

    chunk_buf = soc_cm_salloc(unit,
                              chunk_size * l2x_data[unit].entry_words * 4,
                              "l2x_new");

    delete_map = sal_alloc(SHR_BITALLOCSIZE(index_count), "l2x_delete_map");
    callback_map = sal_alloc(SHR_BITALLOCSIZE(index_count), "l2x_callback_map");

    chunk_delete_map = 
    sal_alloc(SHR_BITALLOCSIZE(chunk_size), "l2x_chunk_delete_map");
    chunk_callback_map = 
    sal_alloc(SHR_BITALLOCSIZE(chunk_size), "l2x_chunk_callback_map");
#ifdef BCM_TSN_SUPPORT
    if (soc_feature(unit, soc_feature_tsn)) {
        sr_ref_map =
            sal_alloc(SHR_BITALLOCSIZE(index_count), "l2x_sr_ref_map");
        bucket_sr_ref_map =
            sal_alloc(SHR_BITALLOCSIZE(SOC_L2X_BUCKET_SIZE),
                      "l2x_bucket_sr_ref_map");
        is_tsn_or_sr = TRUE;
    }
#endif

    if (shadow_tab == NULL || chunk_buf == NULL || 
        delete_map == NULL || chunk_delete_map == NULL || 
#ifdef BCM_TSN_SUPPORT
        ((is_tsn_or_sr) &&
         (sr_ref_map == NULL || bucket_sr_ref_map == NULL)) ||
#endif
        callback_map == NULL || chunk_callback_map == NULL) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "AbnormalThreadExit:soc_l2x_thread: not enough memory \n")));
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                           SOC_SWITCH_EVENT_THREAD_L2X, __LINE__, SOC_E_MEMORY);
        goto cleanup_exit;
    }
    
#ifdef BCM_HURRICANE3_SUPPORT
    /* Prepare for sync L2 overflow table */
    if ((soc_feature(unit, soc_feature_l2_overflow_bucket)) &&
            (soc->l2_overflow_bucket_enable)) {
        if (soc_hr3_l2_overflow_sync_init(unit) < 0) {
            LOG_ERROR(BSL_LS_SOC_L2,
                (BSL_META_U(unit,
                "Error:soc_hr3_l2_overflow_sync_init: not enough memory \n")));
            soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                           SOC_SWITCH_EVENT_THREAD_L2X, __LINE__, SOC_E_MEMORY);
            goto cleanup_exit;
        }
    }
#endif /* BCM_HURRICANE3_SUPPORT */    
    
    /*
     * Start with initially empty shadow table.
     */

    sal_memset(shadow_tab, 0, index_count * l2x_data[unit].entry_words * 4);

    /*
     * Clear the delete and callback map.
     */
    SHR_BITCLR_RANGE(delete_map, 0, index_count);
    SHR_BITCLR_RANGE(callback_map, 0, index_count);

    l2x_data[unit].shadow_tab = shadow_tab;
    l2x_data[unit].chunk_buf = chunk_buf;
    l2x_data[unit].delete_map = delete_map;
    l2x_data[unit].callback_map = callback_map;
#ifdef BCM_TSN_SUPPORT
    if (is_tsn_or_sr) {
        SHR_BITCLR_RANGE(sr_ref_map, 0, index_count);
        l2x_data[unit].sr_ref_map = sr_ref_map;
    }
#endif /* BCM_TSN_SUPPORT */
    chunk_index = 0;
    tab_p = shadow_tab;

    while ((interval = soc->l2x_interval) != 0) {
        /*
         * Read the next chunk of the L2 table using Table DMA.
         */

        LOG_VERBOSE(BSL_LS_SOC_ARL,
                    (BSL_META_U(unit,
                                "soc_l2x_thread: Process %d-%d\n"),
                     chunk_index, chunk_index + chunk_size - 1));

        stime = sal_time_usecs();

        soc_mem_lock(unit, l2x_data[unit].l2mem);
        if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                                  "soc_l2x_thread: unable to take mutex\n")));
            soc_mem_unlock(unit, l2x_data[unit].l2mem);
            soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                               SOC_SWITCH_EVENT_THREAD_L2X, __LINE__,
                                SOC_E_RESOURCE);
            goto cleanup_exit;
        }

        if ((rv = soc_mem_read_range(unit, l2x_data[unit].l2mem,
                                     MEM_BLOCK_ANY,chunk_index,
                                     chunk_index + chunk_size - 1,
                                     chunk_buf)) < 0) {
            SOC_L2_DEL_SYNC_UNLOCK(soc);
            soc_mem_unlock(unit, l2x_data[unit].l2mem);
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                                  "soc_l2x_thread: DMA failed: %s\n"),
                                  soc_errmsg(rv)));

            soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                               SOC_SWITCH_EVENT_THREAD_L2X, __LINE__, rv);

            goto cleanup_exit;
        } else {
            SHR_BITCOPY_RANGE(chunk_delete_map, 0, delete_map, chunk_index, 
                              chunk_size);
            SHR_BITCOPY_RANGE(chunk_callback_map, 0, callback_map, chunk_index, 
                              chunk_size);

            SHR_BITCLR_RANGE(delete_map, chunk_index, chunk_size);
            SHR_BITCLR_RANGE(callback_map, chunk_index, chunk_size);

            SOC_L2_DEL_SYNC_UNLOCK(soc);
            soc_mem_unlock(unit, l2x_data[unit].l2mem);

            /*
             * Scan, compare, and sync the old and new chunks one bucket
             * at a time.
             */

            buf_p = chunk_buf;
            for (bucket_index = 0; bucket_index < chunk_size; 
                bucket_index += SOC_L2X_BUCKET_SIZE) {
#ifdef BCM_TSN_SUPPORT
                if (is_tsn_or_sr) {
                     soc_mem_lock(unit, l2x_data[unit].l2mem);
                     SOC_L2_SR_LOCK(unit);
                     SHR_BITCOPY_RANGE(bucket_sr_ref_map, 0, sr_ref_map,
                                       chunk_index + bucket_index,
                                       SOC_L2X_BUCKET_SIZE);

                     _soc_l2x_sync_bucket(unit, tab_p, buf_p,
                                          soc->l2x_shadow_hit_bits,
                                          bucket_index, chunk_delete_map,
                                          chunk_callback_map,
                                          bucket_sr_ref_map);
                     SHR_BITCOPY_RANGE(sr_ref_map, chunk_index + bucket_index,
                                       bucket_sr_ref_map,
                                       0,
                                       SOC_L2X_BUCKET_SIZE);
                     SOC_L2_SR_UNLOCK(unit);
                     soc_mem_unlock(unit, l2x_data[unit].l2mem);
                } else
#endif
                {
                    _soc_l2x_sync_bucket(unit, tab_p, buf_p,
                                         soc->l2x_shadow_hit_bits,
                                         bucket_index, chunk_delete_map,
                                         chunk_callback_map, NULL);
                }

                tab_p += l2x_data[unit].entry_words * SOC_L2X_BUCKET_SIZE;
                buf_p += l2x_data[unit].entry_words * SOC_L2X_BUCKET_SIZE;
            }

            if ((chunk_index += chunk_size) >= index_count) {
                chunk_index = 0;
                tab_p = shadow_tab;
            }
        }

        etime = sal_time_usecs();

        /*
         * Implement the sleep using a semaphore timeout so if the task
         * is requested to exit, it can do so immediately.
         */

        LOG_VERBOSE(BSL_LS_SOC_ARL,
                    (BSL_META_U(unit,
                                "soc_l2x_thread: unit=%d: done in %d usec\n"),
                     unit,
                     SAL_USECS_SUB(etime, stime)));
#if defined(BCM_HURRICANE3_SUPPORT)
        /* Sync the L2 overflow table */
        if ((chunk_index == 0) && 
            (soc_feature(unit, soc_feature_l2_overflow_bucket)) &&
            (soc->l2_overflow_bucket_enable)) {
            soc_hr3_l2_overflow_sync(unit);
            
        }

#endif /* BCM_HURRICANE3_SUPPORT */

        sal_sem_take(soc->l2x_notify, interval / chunk_count);
    }

cleanup_exit:

    (void)sal_sem_take(soc->l2x_lock, sal_sem_FOREVER);

    if (chunk_buf != NULL) {
        soc_cm_sfree(unit, chunk_buf);
        l2x_data[unit].chunk_buf = NULL;
    }

    if (shadow_tab != NULL) {
        sal_free(shadow_tab);
        l2x_data[unit].shadow_tab = NULL;
    }

    if (delete_map != NULL) {
        sal_free(delete_map);
        l2x_data[unit].delete_map = NULL;
    }

    if (chunk_delete_map != NULL) {
        sal_free(chunk_delete_map);
    }

    if (callback_map != NULL) {
        sal_free(callback_map);
        l2x_data[unit].callback_map = NULL;
    }

    if (chunk_callback_map != NULL) {
        sal_free(chunk_callback_map);
    }

#ifdef BCM_TSN_SUPPORT
    if (sr_ref_map != NULL) {
        sal_free(sr_ref_map);
        l2x_data[unit].sr_ref_map = NULL;
    }
    if (bucket_sr_ref_map != NULL) {
        sal_free(bucket_sr_ref_map);
    }
#endif

#ifdef BCM_HURRICANE3_SUPPORT
    /* Prepare for sync L2 overflow table */
    if ((soc_feature(unit, soc_feature_l2_overflow_bucket)) &&
            (soc->l2_overflow_bucket_enable)) {
        soc_hr3_l2_overflow_sync_cleanup(unit);
    }
#endif /* BCM_HURRICANE3_SUPPORT */    

    (void)sal_sem_give(soc->l2x_lock);

    LOG_INFO(BSL_LS_SOC_ARL,
             (BSL_META_U(unit,
                         "soc_l2x_thread: exiting\n")));

    soc->l2x_pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

#if defined(BCM_TSN_SUPPORT)
/*
 * Function:
 *    soc_l2x_sr_ref_sync
 * Purpose:
 *    Sync the SW shadow copy for SR flow reference count
 * Parameters:
 *    unit  - unit number.
 *    index - L2 entry index.
 * Returns:
 *    SOC_E_XXX
 * Notes:
 */
int soc_l2x_sr_ref_sync(int unit, int l2_index)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    int                 max_index;

    LOG_VERBOSE(BSL_LS_SOC_ARL,
                (BSL_META_U(unit,
                            "soc_l2x_sr_ref_sync: unit=%d index=%d\n"),
                 unit, l2_index));

    if (soc->l2x_pid == SAL_THREAD_ERROR) {
        /* thread not running */
        return SOC_E_NONE;
    }

    if (l2x_data[unit].l2mem == INVALIDm) {
        return SOC_E_NONE;
    }

    max_index = soc_mem_index_max(unit, l2x_data[unit].l2mem);
    if (l2_index > max_index) {
#ifdef BCM_HURRICANE3_SUPPORT
        if ((SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) &&
            (l2_index & SOC_L2X_GEN_RESP_INDEX_L2_OVERFLOW(unit))) {
            /* Entry learnt in L2_ENTRY_OVERFLOW */
            return SOC_E_NONE;
        }
#endif /* BCM_HURRICANE3_SUPPORT */
        return SOC_E_PARAM;
    }
    if (l2x_data[unit].shadow_tab == NULL) {
        LOG_WARN(BSL_LS_SOC_ARL,
                    (BSL_META_U(unit,
                                "soc_l2x_sr_ref_sync: "
                                "l2x_data[%d].shadow_tab is NULL!!!\n"),
                     unit));
        return SOC_E_NONE;
    }

    SHR_BITSET(l2x_data[unit].sr_ref_map, l2_index);

    return SOC_E_NONE;
}

#endif /* BCM_TSN_SUPPORT */

#endif /* BCM_XGS_SWITCH_SUPPORT */

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * L2 - Broadcom StrataSwitch Layer-2 switch API.
 */

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/l2x.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#include <bcm/l2.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/trunk.h>
#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/trident.h>
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#include <bcm_int/esw/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#include <bcm_int/esw_dispatch.h>

#ifdef BCM_TOMAHAWK3_SUPPORT                                                    
#define BSL_LSS_SCACHE (BSL_LS_SHARED_SCACHE | BSL_VERBOSE)

/****************************************************************
 *
 * L2 software tables, per unit.
 */

int _th3_l2_init[BCM_MAX_NUM_UNITS];

static _bcm_l2_match_ctrl_t *_bcm_th3_l2_match_ctrl[BCM_MAX_NUM_UNITS];
static sal_sem_t _bcm_th3_l2_cb_check[BCM_MAX_NUM_UNITS];

/****************************************************************************
 *
 * L2X Message Registration
 */

#define L2_CB_MAX		3

typedef struct th3_l2x_cb_entry_s {
    bcm_l2_addr_callback_t	fn;
    void			*fn_data;
} th3_l2_cb_entry_t;

#define _BCM_TH3_L2X_THREAD_STOP      (1 << 0)
typedef struct th3_l2_data_s {
    th3_l2_cb_entry_t cb[L2_CB_MAX];
    int               cb_count;
    int               flags;
    sal_mutex_t       l2_mutex;
} th3_l2_data_t;

static th3_l2_data_t th3_l2_data[SOC_MAX_NUM_DEVICES];


#define L2_LOCK(unit) \
    sal_mutex_take(th3_l2_data[unit].l2_mutex, sal_mutex_FOREVER)

#define L2_UNLOCK(unit) sal_mutex_give(th3_l2_data[unit].l2_mutex)

#define L2_MUTEX(unit) if (th3_l2_data[unit].l2_mutex == NULL && \
       (th3_l2_data[unit].l2_mutex = sal_mutex_create("bcm_l2_lock")) == NULL) \
            return BCM_E_MEMORY

/****************************************************************************
 *
 * L2 Callbacks Registration
 */
static bcm_l2_addr_callback_t _bcm_th3_l2_cbs[SOC_MAX_NUM_DEVICES];
static void *_bcm_th3_l2_cb_data[SOC_MAX_NUM_DEVICES];

/* forward declaration */
#if 0

static int _bcm_l2_bpdu_init(int unit);
#endif
STATIC int _bcm_th3_l2_replace(int unit, uint32 flags, uint32 int_flags,
                               bcm_l2_addr_t *match_addr,
                               bcm_module_t new_module, bcm_port_t new_port,
                               bcm_trunk_t new_trunk,
                               bcm_l2_traverse_cb trav_fn, void *user_data);

/* L2 (vlan, port) tuple structure */
typedef struct _bcm_l2_vlan_port_pair_s {
    /* Array of PBMP with max VLAN size, the first 0-4095 PBMP entries 
     in the array are for max number of VLANs supported, the 4096th
     i.e pbm[BCM_VLAN_MAX+1]th entry in the array is to maintain a list
     of PBMPs for invalid VLANs (used for delete by gport operation). */
    bcm_pbmp_t        pbm[BCM_VLAN_MAX+2];
} _bcm_l2_vlan_port_pair_t;

typedef struct _bcm_l2_vlan_trunk_pair_s {
    SHR_BITDCL        *tbm[BCM_VLAN_MAX+2];
} _bcm_l2_vlan_trunk_pair_t;

typedef struct _bcm_l2_del_vlan_port_pair_s {
    uint32                    flags;            /* BCM_L2_AUTH_XXX flags. */
    uint32                    replace_flags;        /* BCM_L2_REPLACE_XXX flags */
    bcm_vlan_vector_t         del_by_vlan_vec;  /* Vlan vector represents 
                                                   delete by vlan */
    _bcm_l2_vlan_port_pair_t  *mod_pair_arr;    /* Reference to Array of
                                                   (vlan,port) for max modules */
    _bcm_l2_vlan_trunk_pair_t *unit_pair_arr;   /* Reference to Array of
                                                   (vlan,tid) for each unit */
} _bcm_l2_del_vlan_port_pair_t;

typedef enum l2x_memacc_field_e {
    _BCM_L2_MEMACC_VALID,
    _BCM_L2_MEMACC_KEY_TYPE,
    _BCM_L2_MEMACC_NUM
} l2x_memacc_field_t;

static uint32 _l2_dbg_matched_entries[SOC_MAX_NUM_DEVICES],
              _l2_dbg_unmatched_entries[SOC_MAX_NUM_DEVICES];

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int _bcm_th3_l2_wb_alloc(int unit);
STATIC int _bcm_th3_l2_wb_recover(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

/* just need to clear SRC or DES hit bit. Customers may call API bcm_esw_l2_replace
*  with parameter *match_addr which value is NULL */
#define _CLEAR_ALL_ENTRIES_SRC_OR_DES_HIT_BIT(flag) \
                  (!(flag & (BCM_L2_REPLACE_MATCH_VLAN |\
                             BCM_L2_REPLACE_MATCH_MAC |\
                             BCM_L2_REPLACE_MATCH_DEST)) &&\
                   (flag & (BCM_L2_REPLACE_DES_HIT_CLEAR |\
                            BCM_L2_REPLACE_SRC_HIT_CLEAR)))

/*
 * Function:
 *	bcm_th3_l2_learn_start
 * Description:
 *	This function creates learn thread. It is called after L2 learn cache has
 *  and CMICx have been initialized
 * Parameters:
 *	unit - StrataSwitch device unit number
 * Returns:
 *	BCM_E_XXX
 */
int
bcm_th3_l2_learn_start(int unit, int interval)
{
    sal_usecs_t cfg_interval = -1;
    int rv = SOC_E_NONE;

    /* Stop learn thread if it is runnng, and apply new interval provided
     * by application
     */
    if (soc_th3_l2_learn_thread_running(unit, &cfg_interval)) {
        LOG_ERROR(BSL_LS_BCM_L2,
                 (BSL_META_U(unit, "Learn thread running, interval=%d\n"),
                 cfg_interval));

        BCM_IF_ERROR_RETURN(soc_th3_l2_learn_thread_stop(unit));
        /* Check if sleep is reqd */
        sal_usleep(10000);
    }

    LOG_INFO(BSL_LS_BCM_L2, (BSL_META_U(unit, "======: Starting learn"
                                              " thread\n")));

    rv = soc_th3_l2_learn_alloc_resources(unit);

    if (rv == SOC_E_NONE) {
        rv = soc_th3_l2_learn_thread_start(unit, interval);
    }

    return rv;
}

/*
 * Function:
 *	bcm_tomahawk3_l2_init
 * Description:
 *	Perform required initializations to L2 table.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_tomahawk3_l2_init(int unit)
{
    int rv; 
    sal_usecs_t lrn_interval;
    sal_usecs_t l2x_interval;
    uint32 flags = 0;
#ifdef BCM_XGS_SWITCH_SUPPORT
    int frozen;

    BCM_IF_ERROR_RETURN(
        soc_l2x_is_frozen(unit, SOC_L2X_FROZEN_WITH_LOCK, &frozen));
    if (TRUE == frozen) {
        return BCM_E_BUSY;
    }
#endif /* BCM_XGS_SWITCH_SUPPORT */
    _th3_l2_init[unit] = 0;

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RCPU_ONLY(unit)) {
        /* Turn off arl aging */
        BCM_IF_ERROR_RETURN
            (SOC_FUNCTIONS(unit)->soc_age_timer_set(unit, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_tomahawk3_l2_detach(unit));
    }

    /* Proposal: during warm boot, customer wants to freeze l2x.
     * After warm boot, customer will restore l2x.
     */
    if (SOC_WARM_BOOT(unit) && soc_l2x_cml_override_is_enabled(unit)) {
        /* In this condition, soc_l2x_freeze does not change ING_MISC_CONFIG2r.
         * It just wants to Increment  l2 frozen indicator "f_l2->hw_frozen++".
         * When we call bcm_tomahawk3_l2_age_timer_set, we won't enable SW aging
         * by mistake
         */
        BCM_IF_ERROR_RETURN(soc_l2x_selective_freeze(unit,
                                SOC_L2X_FROZEN_WITHOUT_LOCK));
    }

    /*
     * Call chip-dependent initialization
     */
    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_l2_init(unit));

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RCPU_ONLY(unit)) {
        /*
         * Init L2 cache
         */
        rv = bcm_th3_l2_cache_init(unit);
        if (BCM_FAILURE(rv) && (rv != BCM_E_UNAVAIL)) {
            return rv;
        }
    }

    /* If learn thread is not running, start it */
    if (!(soc_th3_l2_learn_thread_running(unit, &lrn_interval))) {
         int cfg_interval;

        /* Start learn thread. Make sure learn cache and DMA infrastructure
         * have been initialized at this point
         */
        cfg_interval = soc_property_get(unit, spn_L2XLRN_THREAD_INTERVAL,
                                        SAL_BOOT_BCMSIM ?
                                        BCMSIM_L2XLRN_INTERVAL :
                                        BCM_L2XLRN_INTERVAL_DEFAULT);

        BCM_IF_ERROR_RETURN(bcm_th3_l2_learn_start(unit, cfg_interval));
    }

    /* If l2x thread is not running, start it */
    if (!soc_l2x_running(unit, &flags, &l2x_interval)) {
        l2x_interval = soc_property_get(unit, spn_L2XMSG_THREAD_USEC,
                                        SOC_L2X_INTERVAL_DEFAULT);
        rv = soc_l2x_start(unit, 0, l2x_interval);

        if (BCM_FAILURE(rv)) {

            LOG_ERROR(BSL_LS_BCM_L2,
                     (BSL_META_U(unit, "Could not start l2x thread,"
                      "rv = %d, interval=%d\n"), rv, l2x_interval));

            return rv;
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_th3_l2_wb_recover(unit));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_th3_l2_wb_alloc(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (_bcm_th3_l2_cb_check[unit] == NULL) {
        _bcm_th3_l2_cb_check[unit] = sal_sem_create("l2 callback check",
                                                    sal_sem_BINARY, 1);
        if (_bcm_th3_l2_cb_check[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }

    _th3_l2_init[unit] = 1;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tomahawk3_l2_detach
 * Purpose:
 *      Clean up L2 bcm layer when unit is detached
 * Parameters:
 *      unit - Unit being detached
 * Returns:
 *	BCM_E_XXX
 */
int
bcm_tomahawk3_l2_detach(int unit)
{
    int frozen;

    /* Verify the table is not frozen */
    BCM_IF_ERROR_RETURN(soc_l2x_is_frozen(unit, SOC_L2X_FROZEN_WITH_LOCK,
                                          &frozen));
    if (TRUE == frozen) {
        return BCM_E_BUSY;
    }

    if (_bcm_th3_l2_match_ctrl[unit] &&
        _bcm_th3_l2_match_ctrl[unit]->preserved) {
        _th3_l2_init[unit] = 0;
        return BCM_E_NONE;
    }

    /*
     * Call chip-dependent finalization
     */
    SOC_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_l2_term(unit));

    if (_bcm_th3_l2_cb_check[unit] != NULL) {
        sal_sem_destroy(_bcm_th3_l2_cb_check[unit]);
        _bcm_th3_l2_cb_check[unit] = NULL;
    }

    _th3_l2_init[unit] = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_l2_match_ctrl_init
 * Purpose:
 *     Allocate resources for match control data (used by callback
 *     registration, traverse and replace APIs)
 * Parameters:
 *     unit  - (IN) BCM device number
 * Retruns:
 *     BCM_E_NONE on success
 *     Other BCM_E_XXX codes on error
 */
int
_bcm_l2_match_ctrl_init(int unit)
{
    sal_sem_t sem = NULL;

    if (_bcm_th3_l2_match_ctrl[unit] == NULL) {
        _bcm_th3_l2_match_ctrl[unit] = sal_alloc(sizeof(_bcm_l2_match_ctrl_t),
                                             "matched_traverse control");
        if (_bcm_th3_l2_match_ctrl[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        sem = sal_sem_create("L2_matched", sal_sem_BINARY, 0);
        if (sem == NULL) {
            sal_free(_bcm_th3_l2_match_ctrl[unit]);
            _bcm_th3_l2_match_ctrl[unit] = NULL;
            return BCM_E_RESOURCE;
        }
    } else {
        sem = _bcm_th3_l2_match_ctrl[unit]->sem;
    }

    sal_memset(_bcm_th3_l2_match_ctrl[unit], 0, sizeof(_bcm_l2_match_ctrl_t));
    _bcm_th3_l2_match_ctrl[unit]->sem = sem;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_l2_match_ctrl_deinit
 * Purpose:
 *     Free resources allocated for match control data (used by callback
 *     registration, traverse and replace APIs)
 * Parameters:
 *     unit  - (IN) BCM device number
 * Retruns:
 *     No value
 */
void
_bcm_l2_match_ctrl_deinit(int unit)
{

    if (_bcm_th3_l2_match_ctrl[unit] != NULL) {
       if (_bcm_th3_l2_match_ctrl[unit]->sem != NULL) {
          sal_sem_destroy(_bcm_th3_l2_match_ctrl[unit]->sem);
          _bcm_th3_l2_match_ctrl[unit]->sem = NULL;
       }
       sal_free(_bcm_th3_l2_match_ctrl[unit]);
      _bcm_th3_l2_match_ctrl[unit] = NULL;
    }

    return;
}

int _bcm_tomahawk3_l2_age_timer_set(int unit, int age_seconds, int enabled)
{
    int rv;
#ifdef BCM_XGS_SWITCH_SUPPORT
    int         frozen = 0;
#endif

    SOC_L2X_MEM_LOCK(unit);
#ifdef BCM_XGS_SWITCH_SUPPORT
    rv = soc_l2x_is_frozen(unit, SOC_L2X_FROZEN, &frozen);

    if (frozen) {
        soc_age_timer_cache_set(unit, age_seconds, enabled);
        enabled = 0; /* L2x is frozen, so we can't enable aging function */
    } 
#endif

    rv = SOC_FUNCTIONS(unit)->soc_age_timer_set(unit, age_seconds, enabled);
    SOC_L2X_MEM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *	bcm_tomahawk3_l2_age_timer_set
 * Description:
 *	Set the age timer for all blocks.
 *	Setting the value to 0 disables the age timer.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	age_seconds - Age timer value in seconds
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_tomahawk3_l2_age_timer_set(int unit, int age_seconds)
{
    int			max_value;
    int			enabled;
    int			rv;

    TH3_L2_INIT(unit);

    BCM_IF_ERROR_RETURN
	(SOC_FUNCTIONS(unit)->soc_age_timer_max_get(unit, &max_value));

    if (age_seconds < 0 || age_seconds > max_value) {
        return BCM_E_PARAM;
    }

    enabled = age_seconds ? 1 : 0;

    rv = _bcm_tomahawk3_l2_age_timer_set(unit, age_seconds, enabled);

    return rv;
}

int
_bcm_tomahawk3_l2_age_timer_get(int unit, int *age_seconds)
{
    int         rv = BCM_E_NONE;
#ifdef BCM_XGS_SWITCH_SUPPORT
    int         frozen = 0;
#endif
    int         enabled;

    SOC_L2X_MEM_LOCK(unit);

#ifdef BCM_XGS_SWITCH_SUPPORT
    rv = soc_l2x_is_frozen(unit, SOC_L2X_FROZEN, &frozen);

    if (BCM_FAILURE(rv)) {
        frozen = 0;
    }
    if (frozen) {
        soc_age_timer_cache_get(unit, age_seconds, &enabled);

        SOC_L2X_MEM_UNLOCK(unit);
        return rv;
    }
#endif
    rv = SOC_FUNCTIONS(unit)->soc_age_timer_get(unit, age_seconds, &enabled);

    SOC_L2X_MEM_UNLOCK(unit);
    return rv;
}

/*
 * Function: bcm_tomahawk3_l2_age_timer_get
 * Description:
 *	Returns the current age timer value
 *	The value is 0 if aging is not enabled
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal)
 *	age_seconds - Place to store returned age timer value in seconds
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_tomahawk3_l2_age_timer_get(int unit, int *age_seconds)
{
    int			seconds, enabled;
    int			rv;

    TH3_L2_INIT(unit);

    if (age_seconds == NULL) {
        return BCM_E_PARAM;
    }

    SOC_L2X_MEM_LOCK(unit);
    rv = SOC_FUNCTIONS(unit)->soc_age_timer_get(unit, &seconds, &enabled);
    SOC_L2X_MEM_UNLOCK(unit);

    BCM_IF_ERROR_RETURN(rv);

    if (enabled) {
        *age_seconds = seconds;
    } else {
        *age_seconds = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_l2_register_callback
 * Description:
 *      Call back to handle bcm_l2_addr_register clients
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      entry_del - Entry to be deleted or updated, NULL if none
 *      entry_add - Entry to be inserted or updated, NULL if none
 *      fn_data - unused
 * Notes:
 *      Only one callback per unit to the bcm layer is supported here.
 *      Multiple bcm client callbacks per unit are supported in the bcm layer
 */
void
_bcm_th3_l2_register_callback(int unit,
                              int pflags,
                              l2x_entry_t *entry_del,
                              l2x_entry_t *entry_add,
                              void *fn_data)
{
    bcm_l2_addr_t l2addr_del, l2addr_add;
    _bcm_l2_match_ctrl_t *match_ctrl;
    l2x_entry_t *l2x_entry_buf;
    int buf_index, entry_index;

    if (soc_th3_l2x_appl_callback_ready(unit)) {
        /* We use insert/delete and 'no action' flag combinations to stop
         * issuing callback
         */
        if (pflags & SOC_L2X_ENTRY_NO_ACTION) {
            if ((pflags & SOC_L2X_ENTRY_INSERT) ||
                (pflags & SOC_L2X_ENTRY_DELETE)) {
                return;
            }
        }
    }

    if (pflags & (SOC_L2X_ENTRY_DUMMY | SOC_L2X_ENTRY_NO_ACTION)) {
        /* pass l2addr_add info match */
        match_ctrl = _bcm_th3_l2_match_ctrl[unit];
        buf_index = match_ctrl->entry_wr_idx / _BCM_L2_MATCH_ENTRY_BUF_SIZE;
        entry_index = match_ctrl->entry_wr_idx % _BCM_L2_MATCH_ENTRY_BUF_SIZE;
        if (buf_index >= _BCM_L2_MATCH_ENTRY_BUF_COUNT) {
            LOG_WARN(BSL_LS_BCM_L2,
                     (BSL_META_U(unit,
                                 "Buf_index is overflow!!!\n")));
            return;
        }
        if (match_ctrl->l2x_entry_buf[buf_index] == NULL) {
            match_ctrl->l2x_entry_buf[buf_index] =
                sal_alloc(sizeof(l2x_entry_t) * _BCM_L2_MATCH_ENTRY_BUF_SIZE,
                          "l2_matched_traverse");
            if (match_ctrl->l2x_entry_buf[buf_index] == NULL) {
                LOG_WARN(BSL_LS_BCM_L2,
                         (BSL_META_U(unit,
                                     "Fail to allocate space "
                                     "for L2 matched traverse!!!\n")));
                return;
            }
        }
        l2x_entry_buf = match_ctrl->l2x_entry_buf[buf_index];

        if (pflags & SOC_L2X_ENTRY_NO_ACTION) {
            l2x_entry_buf[entry_index] = *entry_add;
        } else {
            sal_memcpy(&l2x_entry_buf[entry_index],
                       soc_mem_entry_null(unit, L2Xm), sizeof(l2x_entry_t));
        }
        match_ctrl->entry_wr_idx++;
        sal_sem_give(match_ctrl->sem);
        sal_thread_yield();
    } else if (_bcm_th3_l2_cbs[unit] != NULL) {
         uint32 flags = 0; /* Common flags: Move, From/to native */

        sal_memset(&l2addr_del, 0, sizeof(bcm_l2_addr_t));
        sal_memset(&l2addr_add, 0, sizeof(bcm_l2_addr_t));

        /* First, set up the entries:  decode HW entries and set flags */
        if (entry_del != NULL) {
            (void)_bcm_th3_l2_from_l2x(unit, &l2addr_del, entry_del);
        }
        if (entry_add != NULL) {
            (void)_bcm_th3_l2_from_l2x(unit, &l2addr_add, entry_add);
        }

        if ((entry_del != NULL) && (entry_add != NULL)) { /* It's a move */
            flags |= BCM_L2_MOVE;
            if (SOC_USE_GPORT(unit)) {
                if (l2addr_del.port != l2addr_add.port) {
                    flags |= BCM_L2_MOVE_PORT;
                }
            } else {
                if (l2addr_del.modid != l2addr_add.modid ||
                    l2addr_del.tgid != l2addr_add.tgid ||
                    l2addr_del.port != l2addr_add.port) {
                    flags |= BCM_L2_MOVE_PORT;
                }
            }
            if (!(l2addr_del.flags & BCM_L2_TRUNK_MEMBER)) {
                if (bcm_tomahawk3_l2_port_native(unit, l2addr_del.modid,
                                                 l2addr_del.port) > 0) {
                    flags |= BCM_L2_FROM_NATIVE;
                    l2addr_del.flags |= BCM_L2_NATIVE;
                }
            }
            if (!(l2addr_add.flags & BCM_L2_TRUNK_MEMBER)) {
                if (bcm_tomahawk3_l2_port_native(unit, l2addr_add.modid,
                                                 l2addr_add.port) > 0) {
                    flags |= BCM_L2_TO_NATIVE;
                    l2addr_add.flags |= BCM_L2_NATIVE;
                }
            }
            l2addr_del.flags |= flags;
            l2addr_add.flags |= flags;
        } else if (entry_del != NULL) { /* Age out or simple delete */
            if (!(l2addr_del.flags & BCM_L2_TRUNK_MEMBER)) {
                if (bcm_tomahawk3_l2_port_native(unit, l2addr_del.modid,
                                                 l2addr_del.port) > 0) {
                    l2addr_del.flags |= BCM_L2_NATIVE;
                }
            }
        } else if (entry_add != NULL) { /* Insert or learn */
            if (!(l2addr_add.flags & BCM_L2_TRUNK_MEMBER)) {
                if (bcm_tomahawk3_l2_port_native(unit, l2addr_add.modid,
                                                 l2addr_add.port) > 0) {
                    l2addr_add.flags |= BCM_L2_NATIVE;
                }
            }
        }

        if (_bcm_th3_l2_cb_check[unit] != NULL) {
            sal_sem_take(_bcm_th3_l2_cb_check[unit], sal_sem_FOREVER);
        }

        /* The entries are now set up. Make the callbacks */
        if (entry_del != NULL) {
            if (_bcm_th3_l2_cbs[unit] != NULL) {
                _bcm_th3_l2_cbs[unit](unit, &l2addr_del, 0,
                                      _bcm_th3_l2_cb_data[unit]);
            }
        }

        if (pflags & SOC_L2X_ENTRY_OVERFLOW) {
            l2addr_add.flags |= BCM_L2_ENTRY_OVERFLOW;
        }

        if (entry_add != NULL) {
            if (_bcm_th3_l2_cbs[unit] != NULL) {
                _bcm_th3_l2_cbs[unit](unit, &l2addr_add, 1,
                                      _bcm_th3_l2_cb_data[unit]);
            }
        }

        if (_bcm_th3_l2_cb_check[unit] != NULL) {
            sal_sem_give(_bcm_th3_l2_cb_check[unit]);
        }
    }
}

/*
 * Function:
 *	bcm_tomahawk3_l2_addr_freeze
 * Description:
 *	Temporarily quiesce ARL from all activity (learning, aging, inserting)
 * include SW operations
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_tomahawk3_l2_addr_freeze(int unit)
{
    TH3_L2_INIT(unit);

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_arl_hashed)) {
        return soc_l2x_freeze(unit);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_tomahawk3_l2_addr_thaw
 * Description:
 *	Restore normal ARL activity including S/W and H/W activity.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_tomahawk3_l2_addr_thaw(int unit)
{
    TH3_L2_INIT(unit);

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_arl_hashed)) {
        return soc_l2x_thaw(unit);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_tomahawk3_l2_clear
 * Purpose:
 *      Clear the L2 layer
 * Parameters:
 *      unit  - BCM unit number
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tomahawk3_l2_clear(int unit)
{
    TH3_L2_INIT(unit);

    if (_bcm_th3_l2_match_ctrl[unit]) {
        _bcm_th3_l2_match_ctrl[unit]->preserved = 1;
    }

    BCM_IF_ERROR_RETURN(bcm_tomahawk3_l2_detach(unit));

    if (_bcm_th3_l2_match_ctrl[unit]) {
        _bcm_th3_l2_match_ctrl[unit]->preserved = 0;
    }

    /*
     * Call chip-dependent initialization
     */
    BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_l2_init(unit));

#if defined(BCM_XGS_SWITCH_SUPPORT)
    /* Stop l2x thread if callback registration started it. */
    if (th3_l2_data[unit].flags & _BCM_TH3_L2X_THREAD_STOP) {
        BCM_IF_ERROR_RETURN(soc_l2x_stop(unit));
        /* Stop the learn thread */
        BCM_IF_ERROR_RETURN(soc_th3_l2_learn_thread_stop(unit));
    }
#endif /* BCM_XGS_SWITCH_SUPPORT */

    /* Clear l2_data structure */
    th3_l2_data[unit].cb_count = 0;
    th3_l2_data[unit].flags = 0;
    sal_memset(&th3_l2_data[unit].cb, 0, sizeof(th3_l2_data[unit].cb));

    _th3_l2_init[unit] = 1; /* some positive value */

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_tomahawk3_l2_key_dump
 * Purpose:
 *	Dump the key (VLAN+MAC) portion of a hardware-independent
 *	L2 address for debugging
 * Parameters:
 *	unit - Unit number
 *	pfx - String to print before output
 *	entry - Hardware-independent L2 entry to dump
 *	sfx - String to print after output
 */

int
bcm_tomahawk3_l2_key_dump(int unit, char *pfx, bcm_l2_addr_t *entry, char *sfx)
{
    LOG_CLI((BSL_META_U(unit,
                        "l2: %sVLAN=0x%03x MAC=0x%02x%02x%02x"
             "%02x%02x%02x%s"), pfx, entry->vid,
             entry->mac[0], entry->mac[1], entry->mac[2],
             entry->mac[3], entry->mac[4], entry->mac[5], sfx));

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_l2_conflict_get
 * Purpose:
 *	Given an L2 address, return existing addresses which could conflict.
 * Parameters:
 *	unit		- switch device
 *	addr		- l2 address to search for conflicts
 *	cf_array	- (OUT) list of l2 addresses conflicting with addr
 *	cf_max		- number of entries allocated to cf_array
 *	cf_count	- (OUT) actual number of cf_array entries filled
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *	Given an L2 or L2 multicast address, return any existing L2 or
 *	L2 multicast addresses which might prevent it from being
 *	inserted because a chip resource (like a hash bucket) is full.
 *	This routine could be used if bcm_l2_addr_add or bcm_l2_mcast_add
 *	returns BCM_E_FULL.
 */

int
bcm_tomahawk3_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
                              bcm_l2_addr_t *cf_array, int cf_max,
                              int *cf_count)
{
    TH3_L2_INIT(unit);
    /*
     * Call chip-dependent handler.
     */
    SOC_IF_ERROR_RETURN
       (mbcm_driver[unit]->mbcm_l2_conflict_get(unit, addr,
                                       cf_array, cf_max, cf_count));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_tomahawk3_l2_learn_limit_set
 * Description:
 *     Set the L2 MAC learning limit
 * Parameters:
 *     unit        device number
 *     limit       learn limit control info
 *                 limit->flags - qualifiers bits and action bits
 *                 limit->vlan - vlan identifier
 *                 limit->port - port number
 *                 limit->trunk - trunk identifier
 *                 limit->limit - max number of learned entry, -1 for unlimit
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_l2_learn_limit_set(int unit, bcm_l2_learn_limit_t *limit)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *     bcm_tomahawk3_l2_learn_limit_get
 * Description:
 *     Get the L2 MAC learning limit
 * Parameters:
 *     unit        device number
 *     limit       learn limit control info
 *                 limit->flags - qualifiers bits and action bits
 *                 limit->vlan - vlan identifier
 *                 limit->port - port number
 *                 limit->trunk - trunk identifier
 *                 limit->limit - max number of learned entry, -1 for unlimit
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_l2_learn_limit_get(int unit, bcm_l2_learn_limit_t *limit)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *     bcm_tomahawk3_l2_learn_class_set
 * Description:
 *     To set the attributes of L2 learning class
 * Parameters:
 *     unit        device number
 *     lclass      learning class id (0 - 3)
 *     lclass_prio learning class priority (0 - 3, 3 is the highest)
 *     flags       flags (BCM_L2_LEARN_CLASS_*)
 * Return:
 *     BCM_E_XXX
 * Note:
 *     This feature is disabled by default (see bcmPortControlLearnClassEnable).
 *     By default, each learning class is assigned priority 0 (lowest)
 *     and the attribute, BCM_L2_LEARN_CLASS_MOVE, is not set. By default,
 *     each port is assigned to learning class 0.
 */
int
bcm_tomahawk3_l2_learn_class_set(int unit, int lclass, int lclass_prio,
                                 uint32 flags)
{
    return (BCM_E_UNAVAIL);
}
/*
 * Function:
 *     bcm_tomahawk3_l2_learn_class_get
 * Description:
 *     To get the attributes of L2 learning class
 * Parameters:
 *     unit        device number
 *     lclass      learning class id (0 - 3)
 *     lclass_prio learning class priority (0 - 3, 3 is the highest)
 *     flags       flags (BCM_L2_LEARN_CLASS_*)
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_l2_learn_class_get(int unit, int lclass, int *lclass_prio,
                                 uint32 *flags)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *     bcm_tomahawk3_l2_learn_stat_set
 * Description:
 *     To set or clear the specified L2 hash counter to the indicated value.
 * Parameters:
 *      unit - (IN) Unit number.
 *      learn_stat - (IN) Pointer to the L2 learned statistic structure.
 * Return:
 *     BCM_E_XXX
 * Note:
 *     The purposes of these counters are used to monitor the L2 hash condition.
 *     When the counter counts to 32'hffff_ffff (maximum value), it will keep this value till
 *     software clear it. Currently these L2 hash counters are system-wise.
 */
int
bcm_tomahawk3_l2_learn_stat_set(int unit, bcm_l2_learn_stat_t *learn_stat)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *     bcm_tomahawk3_l2_learn_stat_get
 * Description:
 *     To get the statistic of the specified L2 hash counter from chip.
 * Parameters:
 *      unit - (IN) Unit number.
 *      learn_stat - (INOUT) Pointer to the L2 learned statistic structure.
 * Return:
 *     BCM_E_XXX
 * Note:
 *     The purposes of these counters are used to monitor the L2 hash condition.
 *     When the counter counts to 32'hffff_ffff (maximum value), it will keep this value till
 *     software clear it. Currently these L2 hash counters are system-wise.
 */
int
bcm_tomahawk3_l2_learn_stat_get(int unit, bcm_l2_learn_stat_t *learn_stat)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *	_bcm_th3_delete_replace_flags_convert
 * Description:
 *	A helper function to all delete_by APIs to use bcm_l2_replace
 *  Converts L2 flags to L2 replace flags compatable to use with bcm_l2_replace
 * Parameters:
 *	unit        [IN]- device unit
 *	flags       [IN] - BCM_L2_DELETE_XXX
 *	repl_flags  [OUT]- Vflags BCM_L2_REPLACE_XXX
 * Returns:
 *	BCM_E_XXX
 */

STATIC int
_bcm_th3_delete_replace_flags_convert(int unit, uint32 flags,
                                      uint32 *repl_flags)
{
    uint32 tmp_flags = BCM_L2_REPLACE_DELETE;

    /* Age feature only supported by bulk Control */
    if (flags & BCM_L2_REPLACE_AGE){
        return BCM_E_UNAVAIL;
    }

    if (flags & BCM_L2_DELETE_PENDING) {
        tmp_flags |= BCM_L2_REPLACE_PENDING;
    }

    if (flags & BCM_L2_DELETE_STATIC) {
        tmp_flags |= BCM_L2_REPLACE_MATCH_STATIC;
    }

    if (flags & BCM_L2_DELETE_NO_CALLBACKS) {
        tmp_flags |= BCM_L2_REPLACE_NO_CALLBACKS;
    }

    *repl_flags = tmp_flags;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tomahawk3_l2_port_native
 * Purpose:
 *      Determine if the given port is "native" from the point
 *      of view of L2.
 * Parameters:
 *      unit       - The unit
 *      modid      - Module ID of L2 entry being queried
 *      port       - Module port of L2 entry being queried
 * Returns:
 *      TRUE (> 0) if (modid, port) is front panel/CPU port for unit.
 *      FALSE (0) otherwise.
 *      < 0 on error.
 *
 *      Native means front panel, but also includes the CPU port.
 *      HG ports are always excluded as are ports marked as internal or
 *      external stacking
 */

int
bcm_tomahawk3_l2_port_native(int unit, int modid, int port)
{
    bcm_trunk_t     tgid;
    int             id;

    TH3_L2_INIT(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &modid,
            &port, &tgid, &id));

        if (-1 != id || BCM_TRUNK_INVALID != tgid) {
            return FALSE;
        }
    }

    return TRUE;
}

/*
 * Function:
 *     bcm_tomahawk3_l2_matched_traverse
 * Description:
 *     This function iterates over L2 table entries, find matching entries based
 *     on criteria provided by the caller and executes user provided callback
 *     function for each of the matching entries
 * Parameters:
 *      unit       - (IN) Unit number.
 *      flags      - (IN) BCM_L2_TRAVERSE_* flags
 *      match_addr - (IN) L2 address to match
 *      trav_fn    - (IN) Callback function used for entries matching the
 *                        address and the flags
 *      user_data  - (IN) Pointer to data passed by caller, used with trav_fn
 * Return:
 *     BCM_E_XXX
 * Note:
 *     None
 */
int
bcm_tomahawk3_l2_matched_traverse(int unit, uint32 flags,
                                  bcm_l2_addr_t *match_addr,
                                  bcm_l2_traverse_cb trav_fn, void *user_data)
{
    uint32 replace_flags, int_flags;

    if (!trav_fn) {
        return BCM_E_PARAM;
    }

    if (!match_addr) {
        return BCM_E_PARAM;
    }

    replace_flags = 0;
    int_flags = _BCM_L2_REPLACE_INT_NO_ACTION;
    /*
    * 1/ If BCM_L2_TRAVERSE_MATCH_DYNAMIC and BCM_L2_TRAVERSE_MATCH_STATIC 
    *     are passed simultaneously to API bcm_l2_matched_traverse, 
    *     this API will match all entries.
    * 2/ If both BCM_L2_TRAVERSE_MATCH_DYNAMIC and BCM_L2_TRAVERSE_MATCH_STATIC 
    *     are not passed to API bcm_l2_matched_traverse,
    *     this API will also match all entries as before. This is the legacy behavior.
    * 3/  If only BCM_L2_TRAVERSE_MATCH_DYNAMIC is passed to 
    *     API bcm_l2_matched_traverse, this API will only match dynamic entries.
    * 4/ If only BCM_L2_TRAVERSE_MATCH_STATIC is passed to 
    *     API bcm_l2_matched_traverse, this API will only match static entries.  
    */
    if (((flags & BCM_L2_TRAVERSE_MATCH_STATIC) ? 1 : 0) ==
        ((flags & BCM_L2_TRAVERSE_MATCH_DYNAMIC) ? 1 : 0)) {
        /* Replace static as well as non-static entries */
        replace_flags |= BCM_L2_REPLACE_MATCH_STATIC;
    }
    if (flags & BCM_L2_TRAVERSE_MATCH_MAC) {
        replace_flags |= BCM_L2_REPLACE_MATCH_MAC;
    }
    if (flags & BCM_L2_TRAVERSE_MATCH_VLAN) {
        replace_flags |= BCM_L2_REPLACE_MATCH_VLAN;
    }
    if (flags & BCM_L2_TRAVERSE_MATCH_DEST) {
        replace_flags |= BCM_L2_REPLACE_MATCH_DEST;
    }
    if (!(flags & BCM_L2_TRAVERSE_IGNORE_DISCARD_SRC)) {
        int_flags |= _BCM_L2_REPLACE_INT_MATCH_DISCARD_SRC;
    }
    if (!(flags & BCM_L2_TRAVERSE_IGNORE_DES_HIT)) {
        int_flags |= _BCM_L2_REPLACE_INT_MATCH_DES_HIT;
    }
    if (!(flags & BCM_L2_TRAVERSE_IGNORE_SRC_HIT)) {
        int_flags |= _BCM_L2_REPLACE_INT_MATCH_SRC_HIT;
    }
    if (flags & BCM_L2_TRAVERSE_MATCH_NATIVE) {
        int_flags |= _BCM_L2_REPLACE_INT_MATCH_NATIVE;
    }

    if ((flags & BCM_L2_TRAVERSE_MATCH_STATIC) &&
         !(flags & BCM_L2_TRAVERSE_MATCH_DYNAMIC)){
        int_flags |= _BCM_L2_REPLACE_INT_MATCH_ONLY_STATIC;
    }

    return _bcm_th3_l2_replace(unit, replace_flags, int_flags, match_addr, 0,
                               0, 0, trav_fn, user_data);
}

/*
 * Function:
 *     _bcm_th3_l2_entry_valid
 * Description:
 *      Check if given L2 entry is valid
 * Parameters:
 *      unit         device number
 *      mem         L2 memory to operate on
 *      l2_entry    L2X entry read from HW
 * Return:
 *     BCM_E_NONE
 *     BCM_E_NOT_FOUND
 */
STATIC int
_bcm_th3_l2_entry_valid(int unit, soc_memacc_t *l2x_memacc, uint32 *l2_entry)
{
    int valid;
    int key_type;

    /* Value below can be read from l2_entry's BASE_VALID field (if reqd) */
    valid = soc_memacc_field32_get(&l2x_memacc[_BCM_L2_MEMACC_VALID],
                                           l2_entry);

    if (!valid) {
        return BCM_E_NOT_FOUND;
    }

    key_type = soc_memacc_field32_get(&l2x_memacc[_BCM_L2_MEMACC_KEY_TYPE],
                                      l2_entry);

    if (key_type != TH3_L2_HASH_KEY_TYPE_BRIDGE) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_l2_traverse_mem
 * Description:
 *      Helper function to _bcm_th3_l2_traverse to iterate over L2X memory 
 *      and parse entries
 * Parameters:
 *     unit         device number
 *     trav_st      Traverse structure with initialized data
 * Return:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_traverse_mem(int unit, _bcm_l2_traverse_t *trav_st)
{
    /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int             buf_size, chunksize, chnk_end;
    /* Buffer to store chunk of L2 table we currently work on */
    uint32          *l2_tbl_chnk;
    uint32          *l2_entry;
    int             rv = BCM_E_NONE;
    int             i;
    soc_memacc_t    l2x_memacc_list[_BCM_L2_MEMACC_NUM];
    soc_field_t     l2x_fields[_BCM_L2_MEMACC_NUM] = {
        BASE_VALIDf,
        KEY_TYPEf
    };
    _bcm_l2_replace_t *rep_st;
    soc_mem_t mem = L2Xm;
    
    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    for (i = 0; i < _BCM_L2_MEMACC_NUM; i++) {
        rv = soc_memacc_init(unit, mem, l2x_fields[i], &l2x_memacc_list[i]);
        if (BCM_FAILURE(rv)) {
            SOC_MEMACC_INVALID_SET(&l2x_memacc_list[i]);
        }
    }

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    l2_tbl_chnk = soc_cm_salloc(unit, buf_size, "l2 traverse");
    if (NULL == l2_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
         chnk_idx <= mem_idx_max; 
         chnk_idx += chunksize) {
        sal_memset((void *)l2_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) < mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, l2_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }

        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            l2_entry = 
                soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                                             l2_tbl_chnk, ent_idx);
            if (BCM_FAILURE(_bcm_th3_l2_entry_valid(unit, l2x_memacc_list,
                                                    l2_entry))) {
                continue;
            }

            trav_st->data = l2_entry;
            trav_st->mem = mem;
            trav_st->mem_idx = chnk_idx + ent_idx;
            rv = trav_st->int_cb(unit, (void *)trav_st);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            break;
        }

        if (!trav_st->user_cb) {
            /* Enhancement: use memory write range (per chunk) for replacing
             * l2 entry data
             */
            rep_st = ((_bcm_l2_traverse_t *)trav_st)->user_data;
            if (!(rep_st->flags & BCM_L2_REPLACE_DELETE)) {
                rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ANY,
                                         chnk_idx, chnk_idx_max, l2_tbl_chnk);
                if (SOC_FAILURE(rv)) {
                    break;
                }
            }
        }
    }
    soc_cm_sfree(unit, l2_tbl_chnk);
    return rv;        
}

/*
 * Function:
 *     _bcm_th3_l2_traverse
 * Description:
 *      Wrapper function for bcm_th3 to iterate over table and read L2X momery
 * Parameters:
 *     unit         Device number
 *     trav_st      Traverse structure
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_th3_l2_traverse(int unit, _bcm_l2_traverse_t *trav_st)
{
    int rv = BCM_E_UNAVAIL; 

    rv = _bcm_th3_l2_traverse_mem(unit, trav_st);

    return rv;
}

/*
 * Function:
 *     _bcm_th3_l2_traverse_int_cb
 * Description:
 *      Simple internal callback function for bcm_l2_traverse API to call
 *      a user provided callback on a given entry.
 * Parameters:
 *      unit         device number
 *      trav_st      traverse structure
 * Return:
 *     BCM_E_XXX
 */
int
_bcm_th3_l2_traverse_int_cb(int unit, void *trav_st)
{
    _bcm_l2_traverse_t *trv;
    bcm_l2_addr_t      l2_addr; 

    trv = (_bcm_l2_traverse_t *)trav_st;

    BCM_IF_ERROR_RETURN(
        _bcm_th3_l2_from_l2x(unit, &l2_addr, (l2x_entry_t *)(trv->data)));

    return trv->user_cb(unit, &l2_addr, trv->user_data);
}

/*
 * Function:
 *     bcm_tomahawk3_l2_traverse
 * Description:
 *     Traverse L2 table and call provided callback function with matched entry
 * Parameters:
 *     unit         device number
 *     trav_fn      User specified callback function 
 *     user_data    User specified cookie
 * Return:
 *     BCM_E_XXX
 */
int 
bcm_tomahawk3_l2_traverse(int unit, bcm_l2_traverse_cb trav_fn, void *user_data)
{
    _bcm_l2_traverse_t  trav_st;
    
    TH3_L2_INIT(unit);

    if (!trav_fn) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trav_st, 0, sizeof(_bcm_l2_traverse_t));
    
    trav_st.user_cb = trav_fn;
    trav_st.user_data = user_data;
    trav_st.int_cb = _bcm_th3_l2_traverse_int_cb;

    return (_bcm_th3_l2_traverse(unit, &trav_st));
}

/*
 * Function:
 *     _bcm_th3_l2_replace_dest_setup
 * Description:
 *     Helper function to bcm_l2_replace API to setup a destination
  * Parameters:
 *      unit     - (IN) Unit number.
 *      module   - (IN) Module number
 *      port     - (IN) Port number for matching L2 entry
 *      trunk    - (IN) Trunkid for matching L2 entry. Valid if is_trunk is TRUE
 *      is_trunk - (IN) To specify if 'trunk' is valid
 * Return:
 *     BCM_E_XXX
 * Note:
 * This function is called replace APIs, and indirectly, by delete_by APIs
 */
STATIC int
_bcm_th3_l2_replace_dest_setup(int unit, bcm_module_t module, bcm_port_t port,
                               bcm_trunk_t trunk, int is_trunk,
                               _bcm_l2_replace_dest_t *dest)
{
    /* If all are -1, means ignore the replacement with new destination */
    if (port == -1 && trunk == -1) {
        dest->module = dest->port = dest->trunk = dest->vp = -1;
        return BCM_E_NONE;
    }

    if (BCM_GPORT_IS_SET(port)) {

        
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &dest->module,
                                                   &dest->port, &dest->trunk,
                                                   &dest->vp));
    } else if (is_trunk) {
        
        BCM_IF_ERROR_RETURN(_bcm_trunk_id_validate(unit, trunk));

        dest->trunk = trunk;
        dest->vp = -1;
    } else {
        /* Check to maintain backward compatibility for
         *  replace/traverse/delete_by APIs
         */
        if (!SOC_MODID_ADDRESSABLE(unit, module)) {
            return BCM_E_BADID;
        }
        dest->module =  0; /* Not used */

        if (!SOC_PORT_ADDRESSABLE(unit, port)) {
            return BCM_E_PORT;
        }
        dest->port = port;
        dest->trunk = -1;
        dest->vp = -1;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_l2_replace_data_mask_setup(int unit, _bcm_l2_replace_t *rep)
{
    bcm_mac_t mac_mask;
    bcm_mac_t mac_data;
    int field_len;

    sal_memset(&rep->match_mask, 0, sizeof(l2x_entry_t));
    sal_memset(&rep->match_data, 0, sizeof(l2x_entry_t));
    sal_memset(&rep->new_mask, 0, sizeof(l2x_entry_t));
    sal_memset(&rep->new_data, 0, sizeof(l2x_entry_t));

    soc_mem_field32_set(unit, L2Xm, &rep->match_mask, BASE_VALIDf, 1);
    soc_mem_field32_set(unit, L2Xm, &rep->match_data, BASE_VALIDf, 1);

    if (rep->key_type != -1) {
        field_len = soc_mem_field_length(unit, L2Xm, KEY_TYPEf);
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, KEY_TYPEf,
                            (1 << field_len) - 1);
        soc_mem_field32_set(unit, L2Xm, &rep->match_data, KEY_TYPEf,
                            rep->key_type);
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_VLAN) {
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, VLAN_IDf, 0xfff);
        soc_mem_field32_set(unit, L2Xm, &rep->match_data, VLAN_IDf,
                            rep->key_vlan);
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_MAC) {
        sal_memset(&mac_mask, 0xFF, sizeof(mac_mask));
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_mask, MAC_ADDRf,
                             mac_mask);
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_data, MAC_ADDRf,
                             rep->key_mac);
    }

    if ((rep->flags & BCM_L2_REPLACE_MATCH_UC) &&
        !(rep->flags & BCM_L2_REPLACE_MATCH_MC)) {
        /* The 40th bit of Unicast must be 0 */
        sal_memset(&mac_mask, 0x00, sizeof(mac_mask));
        mac_mask[0] = 0x01;
        sal_memset(&mac_data, 0x00, sizeof(mac_data));
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_mask, MAC_ADDRf,
                             mac_mask);
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_data, MAC_ADDRf,
                             mac_data);
    } else if ((rep->flags & BCM_L2_REPLACE_MATCH_MC) &&
        !(rep->flags & BCM_L2_REPLACE_MATCH_UC)) {
        /* The 40th bit of Multicast must be 1 */
        sal_memset(&mac_mask, 0x00, sizeof(mac_mask));
        mac_mask[0] = 0x01;
        sal_memset(&mac_data, 0x00, sizeof(mac_data));
        mac_data[0] = 0x01;
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_mask, MAC_ADDRf,
                             mac_mask);
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_data, MAC_ADDRf,
                             mac_data);
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_DEST) {
        field_len = soc_mem_field_length(unit, L2Xm, DEST_TYPEf);
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, DEST_TYPEf,
                            (1 << field_len) - 1);

        if (rep->match_dest.trunk != -1) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_data,
                                DEST_TYPEf, 1);

            field_len = soc_mem_field_length(unit, L2Xm, TGIDf);
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, TGIDf,
                                (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, TGIDf,
                                rep->match_dest.trunk);
        } else {
            /* DEST_TYPE field in data is 0 */

            if (rep->match_dest.port != -1) {
                field_len = soc_mem_field_length(unit, L2Xm, PORT_NUMf);
                soc_mem_field32_set(unit, L2Xm, &rep->match_mask,
                                    PORT_NUMf, (1 << field_len) - 1);
                soc_mem_field32_set(unit, L2Xm, &rep->match_data,
                                    PORT_NUMf, rep->match_dest.port);
            }

            
        }
    }

    /* BCM_L2_REPLACE_MATCH_STATIC means to replace static as well as
     * non-static entries
     */
    if (!(rep->flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        if (rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_ONLY_STATIC) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, STATIC_BITf, 1);
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, STATIC_BITf, 1);
        } else { 
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, STATIC_BITf, 1);
            /* STATIC_BIT field in data is 0 */
        }
    }
    /* STATIC_BIT field in mask is 0 */

    if (rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_DISCARD_SRC) {
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, SRC_DISCARDf, 1);
        if (rep->key_l2_flags & BCM_L2_DISCARD_SRC) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, SRC_DISCARDf, 1);
        }
    }

    if (rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_SRC_HIT) {
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, HITSAf, 1);
        if (rep->key_l2_flags & BCM_L2_SRC_HIT) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, HITSAf, 1);
        }
    }
    if (rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_DES_HIT) {
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, HITDAf, 1);
        if (rep->key_l2_flags & BCM_L2_DES_HIT) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, HITDAf, 1);
        }
    }

    if (rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_NATIVE) {
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, LOCAL_SAf, 1);
        if (rep->key_l2_flags & BCM_L2_NATIVE) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, LOCAL_SAf, 1);
        }
    }

    if (_CLEAR_ALL_ENTRIES_SRC_OR_DES_HIT_BIT(rep->flags)) {
        if (rep->flags & BCM_L2_REPLACE_DES_HIT_CLEAR) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, HITDAf, 1);
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, HITDAf, 1);
        }
        if (rep->flags & BCM_L2_REPLACE_SRC_HIT_CLEAR) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, HITSAf, 1);
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, HITSAf, 1);
        }
    }

    if (!(rep->flags & BCM_L2_REPLACE_DELETE)) {
        if (!_CLEAR_ALL_ENTRIES_SRC_OR_DES_HIT_BIT(rep->flags)) {
            if (rep->new_dest.trunk != -1) {
                field_len = soc_mem_field_length(unit, L2Xm, DEST_TYPEf);
                soc_mem_field32_set(unit, L2Xm, &rep->new_mask, DEST_TYPEf,
                                    (1 << field_len) - 1);

                soc_mem_field32_set(unit, L2Xm, &rep->new_data, DEST_TYPEf, 1);

                /*
                 * Writing to DESTINATION field instead of TGIDf
                 * to ensure the MSB bits of DESTINATION are cleared.
                 */
                field_len = soc_mem_field_length(unit, L2Xm, DESTINATIONf);
                soc_mem_field32_set(unit, L2Xm, &rep->new_mask,
                                    DESTINATIONf, (1 << field_len) - 1);
                soc_mem_field32_set(unit, L2Xm, &rep->new_data,
                                    DESTINATIONf, rep->new_dest.trunk);
            } else if (rep->new_dest.port != -1 && rep->new_dest.module != -1) {
                    /* DEST_TYPE field in data is 0 */
                    field_len = soc_mem_field_length(unit, L2Xm, DEST_TYPEf);
                    soc_mem_field32_set(unit, L2Xm, &rep->new_mask, DEST_TYPEf,
                                        (1 << field_len) - 1);

                    field_len = soc_mem_field_length(unit, L2Xm, PORT_NUMf);
                    soc_mem_field32_set(unit, L2Xm, &rep->new_mask,
                                        PORT_NUMf, (1 << field_len) - 1);
                    soc_mem_field32_set(unit, L2Xm, &rep->new_data,
                                        PORT_NUMf, rep->new_dest.port);
            }
        }

        if (rep->flags & BCM_L2_REPLACE_DES_HIT_CLEAR) {
            soc_mem_field32_set(unit, L2Xm, &rep->new_mask, HITDAf, 1);
        }
        if (rep->flags & BCM_L2_REPLACE_SRC_HIT_CLEAR) {
            soc_mem_field32_set(unit, L2Xm, &rep->new_mask, HITSAf, 1);
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_l2_is_soc_l2_mem_cache_enabled(int unit)
{
    soc_mem_t mem = L2Xm;
    uint32 *cache;
    int copyno = SOC_MEM_BLOCK_ANY(unit, mem);

    cache = SOC_MEM_STATE(unit, mem).cache[copyno];

    if (cache == NULL) {
        return FALSE;
    }

    return TRUE;
}

/*
 * Function:
 *     _bcm_th3_l2_entry_replace
 * Description:
 *     Helper function to bcm_l2_replace API to be used as a call back for
 *     bcm_l2_traverse API and replace given entry if MAC matches
 * Parameters:
 *     unit         device number
 *     trav_st      structure with information from traverse function
 * Return:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_entry_replace(int unit, void *trav_st)
{
    _bcm_l2_replace_t *rep_st;
    soc_mem_t   mem;
    uint32      *entry;
    uint32      *match_data, *match_mask;
    int         entry_words, i, idx, rv;
    bcm_mac_t   l2mac;
    int         port_val, trunk_val;
    soc_control_t   *soc = SOC_CONTROL(unit);
    bcm_l2_addr_t l2_addr; 

    if (trav_st == NULL) {
        return BCM_E_PARAM;
    }

    mem = ((_bcm_l2_traverse_t *)trav_st)->mem;
    entry = ((_bcm_l2_traverse_t *)trav_st)->data;
    idx = ((_bcm_l2_traverse_t *)trav_st)->mem_idx;
    rep_st = ((_bcm_l2_traverse_t *)trav_st)->user_data;

    if (rep_st == NULL || entry == NULL || mem == INVALIDm) {
        return BCM_E_PARAM;
    }

    match_data = (uint32 *)&rep_st->match_data;
    match_mask = (uint32 *)&rep_st->match_mask;
    entry_words = soc_mem_entry_words(unit, mem);

    for (i = 0; i < entry_words; i++) {
        if ((entry[i] ^ match_data[i]) & match_mask[i]) {
            return BCM_E_NONE;
        }
    }
    if (rep_st->int_flags & _BCM_L2_REPLACE_INT_NO_ACTION) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_l2_from_l2x(unit, &l2_addr, (l2x_entry_t *)entry));
        return rep_st->trav_fn(unit, &l2_addr, rep_st->user_data);
    }
    if (rep_st->flags & BCM_L2_REPLACE_DELETE) {
        MEM_LOCK(unit, mem);
        if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
            MEM_UNLOCK(unit, mem);
            return BCM_E_RESOURCE;
        }
        rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, entry);
#ifdef BCM_XGS_SWITCH_SUPPORT
        if (BCM_SUCCESS(rv)) {
            rv = soc_l2x_sync_delete(unit, entry, idx,
                     rep_st->flags & BCM_L2_REPLACE_NO_CALLBACKS ?
                     SOC_L2X_NO_CALLBACKS : 0);

            if (rep_st->flags & BCM_L2_REPLACE_NO_CALLBACKS) {
                sal_mutex_take(SOC_CONTROL(unit)->l2x_lrn_shadow_mutex,
                               sal_mutex_FOREVER);
                soc_th3_lrn_shadow_delete(unit, (l2x_entry_t *)entry);
                sal_mutex_give(SOC_CONTROL(unit)->l2x_lrn_shadow_mutex);

                if (soc_th3_l2x_appl_callback_ready(unit)) {
                    sal_mac_addr_t mac;
                    bcm_vlan_t vlan;

                    soc_L2Xm_mac_addr_get(unit, (l2x_entry_t *)entry, MAC_ADDRf,
                                          mac);
                    vlan = soc_L2Xm_field32_get(unit, (l2x_entry_t *)entry,
                                                VLAN_IDf);

                    /* Delete entry from application callback db */
                    (void)soc_th3_l2x_appl_callback_entry_delete(unit, mac,
                                                                 vlan);
                }
            }
        }
#endif /* BCM_XGS_SWITCH_SUPPORT */
        SOC_L2_DEL_SYNC_UNLOCK(soc);
        MEM_UNLOCK(unit, mem);

        return rv;
    }

    if (rep_st->flags & BCM_L2_REPLACE_MATCH_VLAN) {
        if (soc_mem_field32_get(unit, mem, entry, VLAN_IDf) !=
                rep_st->key_vlan) {
                return BCM_E_NONE;
        }
    }

    if (rep_st->flags & BCM_L2_REPLACE_MATCH_MAC) {
        soc_mem_mac_addr_get(unit, mem, entry, MAC_ADDRf, l2mac);
        if (ENET_CMP_MACADDR(rep_st->key_mac, l2mac)) {
            return BCM_E_NONE;
        }
    }

    if (rep_st->flags & BCM_L2_REPLACE_MATCH_DEST) {
        trunk_val = -1;
        port_val = -1;

        if (soc_mem_field32_get(unit, mem, entry, Tf)) {
            trunk_val = soc_mem_field32_get(unit, mem, entry, TGIDf);
        } else {
            port_val = soc_mem_field32_get(unit, mem, entry, PORT_NUMf);
        }

        if (rep_st->match_dest.trunk != -1) {
            if (rep_st->match_dest.trunk != trunk_val) {
                return BCM_E_NONE;
            }
        } else {
            if (rep_st->match_dest.port != port_val) {
                return BCM_E_NONE;
            }
        }
    }

    /* Check if user wants static field matched with the entry's static bit */
    if (!(rep_st->flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        /* BCM_L2_REPLACE_MATCH_STATIC means to replace static as well as
         * non-static entries */
        if (soc_mem_field32_get(unit, mem, entry, STATIC_BITf)) {
            return BCM_E_NONE;
        }
    }

    /* Perform replace action on matching entry */
    /* Note: replace delete flag has been handled at top of this function */
    if (rep_st->new_dest.trunk != -1) { /* New destination is trunk */
        soc_mem_field32_set(unit, mem, entry, DEST_TYPEf, 1);
        soc_mem_field32_set(unit, mem, entry, TGIDf,
                            rep_st->new_dest.trunk);
    } else { /* New destination is mod port */
        soc_mem_field32_set(unit, mem, entry, DEST_TYPEf, 0);
        soc_mem_field32_set(unit, mem, entry, PORT_NUMf,
                            rep_st->new_dest.port);
    }

    return soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, entry);
}

/*
 * Function:
 *     _bcm_th3_l2_replace
 * Description:
 *     Helper function called by main replace API implementation
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_L2_REPLACE_* flags
 *      match_addr - (IN) L2 information used to match one/more L2 addresses
 *      new_module - (IN) Module number. Not used, see Note below 
 *      new_port   - (IN) Port number to replace in the matching entries
 *      new_trunk  - (IN) Port number to replace in the matching entries
 *      trav_fn    - (IN) Callback function (optional) to process matchin
 *                        entries
 *      user_data  - (IN) Arguments to pass to callback function (trav_fn)
 *                        (optional)
 * Return:
 *     BCM_E_XXX
 * Note:
 * new_module is not used. We accept it here for maintaining compatibility of
 * various APIs' behavior. Invalid module id will be rejected inside the call
 * sequence below
 */
STATIC int
_bcm_th3_l2_replace(int unit, uint32 flags, uint32 int_flags,
                    bcm_l2_addr_t *match_addr, bcm_module_t new_module,
                    bcm_port_t new_port, bcm_trunk_t new_trunk,
                    bcm_l2_traverse_cb trav_fn,
                    void *user_data)
{
    _bcm_l2_replace_t   rep_st;
    _bcm_l2_traverse_t  trav_st;
    int                 rv = BCM_E_UNAVAIL;
    int                 do_freeze;

    sal_memset(&rep_st, 0, sizeof(_bcm_l2_replace_t));
    sal_memset(&trav_st, 0, sizeof(_bcm_l2_traverse_t));

    rep_st.trav_fn = trav_fn;
    rep_st.user_data = user_data;
    rep_st.flags = flags;
    rep_st.int_flags = int_flags;
    if ((int_flags & _BCM_L2_REPLACE_IGNORE_PORTID) &&
        !(flags & BCM_L2_REPLACE_DELETE) &&
        !(flags & BCM_L2_REPLACE_MATCH_DEST)) {
        return BCM_E_PARAM;
    }

    if (!(flags & BCM_L2_REPLACE_DELETE) &&
        !(int_flags & _BCM_L2_REPLACE_INT_NO_ACTION)) {
        BCM_IF_ERROR_RETURN(_bcm_th3_l2_replace_dest_setup
                            (unit, new_module, new_port, new_trunk,
                             flags & BCM_L2_REPLACE_NEW_TRUNK,
                             &rep_st.new_dest));
    }

    /* Note rep_st.key_type is 0 */

    if (match_addr != NULL) {
        rep_st.key_l2_flags = match_addr->flags;

        if (flags & BCM_L2_REPLACE_MATCH_DEST) {
            if (int_flags & _BCM_L2_REPLACE_IGNORE_PORTID) {
                if (!SOC_MODID_ADDRESSABLE(unit, match_addr->modid)) {
                    return BCM_E_BADID;
                }
                rep_st.match_dest.vp = -1;
                rep_st.match_dest.trunk = -1;
                rep_st.match_dest.port = -1;
                rep_st.match_dest.module = 0; /* match_addr->modid; ignored */
            } else {
                BCM_IF_ERROR_RETURN(_bcm_th3_l2_replace_dest_setup
                                    (unit, match_addr->modid, match_addr->port,
                                     match_addr->tgid,
                                     match_addr->flags & BCM_L2_TRUNK_MEMBER,
                                     &rep_st.match_dest));
            }
        }

        if (flags & BCM_L2_REPLACE_MATCH_VLAN) {
            rep_st.key_vfi = -1;
            VLAN_CHK_ID(unit, match_addr->vid);
            rep_st.key_vlan = match_addr->vid;
            rep_st.key_type = 0;  /* L2 Bridge */
        }

        if (flags & BCM_L2_REPLACE_MATCH_MAC) {
            sal_memcpy(&rep_st.key_mac, match_addr->mac, sizeof(bcm_mac_t));
            /* When replace L2 by MAC, key type must be provided. */
            /* Key type is 0 by default for L2 Bridge. */
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_replace_data_mask_setup(unit, &rep_st));

    /* Delete operation don't require freeze */
    do_freeze = FALSE;
    if (!(flags & BCM_L2_REPLACE_DELETE) ||
        ((flags & BCM_L2_REPLACE_MATCH_STATIC) &&
          _bcm_th3_l2_is_soc_l2_mem_cache_enabled(unit))) {
        do_freeze = TRUE;
    }

    if (do_freeze) {
        BCM_IF_ERROR_RETURN(bcm_esw_l2_addr_freeze(unit));
    }

    trav_st.user_data = (void *)&rep_st;
    trav_st.int_cb = _bcm_th3_l2_entry_replace;
    rv = _bcm_th3_l2_traverse(unit, &trav_st);

    /* Check with Michael what this does    rv = _soc_l2_sync_mem_cache(unit, &rep_st); */

    if (do_freeze) {
        BCM_IF_ERROR_RETURN(bcm_esw_l2_addr_thaw(unit));
    }

    return rv;
}

/*
 * Function:
 *	bcm_tomahawk3_l2_addr_delete_by_mac
 * Description:
 *	Delete L2 entries associated with a MAC address
 * Parameters:
 *	unit  - device unit
 *	mac   - MAC address
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation
 */
int
bcm_tomahawk3_l2_addr_delete_by_mac(int unit, bcm_mac_t mac, uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    TH3_L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    sal_memcpy(match_addr.mac, mac, sizeof(bcm_mac_t));

    BCM_IF_ERROR_RETURN
        (_bcm_th3_delete_replace_flags_convert(unit, flags, &repl_flags));

    /* Traverse l2 entry contains MAC address field by key type */
    repl_flags |= BCM_L2_REPLACE_MATCH_MAC;

    return _bcm_th3_l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                               NULL, NULL);
}

/*
 * Function:
 *	bcm_tomahawk3_l2_addr_delete_by_vlan
 * Description:
 *	Delete L2 entries associated with a VLAN
 * Parameters:
 *	unit  - device unit
 *	vid   - VLAN id
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_tomahawk3_l2_addr_delete_by_vlan(int unit, bcm_vlan_t vid, uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    TH3_L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.vid = vid;
    BCM_IF_ERROR_RETURN
        (_bcm_th3_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_VLAN;
    return _bcm_th3_l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                               NULL, NULL);
}

/*
 * Function:
 *	bcm_tomahawk3_l2_addr_delete_by_port
 * Description:
 *	Delete L2 addresses associated with a destination module/port.
 * Parameters:
 *	unit  - device unit
 *	mod   - module id (or -1 for local unit)
 *	port  - port
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */

int
bcm_tomahawk3_l2_addr_delete_by_port(int unit,
                                     bcm_module_t mod, bcm_port_t port,
                                     uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;
    uint32          int_flags = 0;

    TH3_L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    if (port == -1) {
        /* non zero mod id is not used, replace will return error */
        match_addr.modid = mod;
        match_addr.port  = -1;
        int_flags = _BCM_L2_REPLACE_IGNORE_PORTID;
    } else if (!BCM_GPORT_IS_SET(port) && mod == -1) {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }

        match_addr.modid = SOC_BASE_MODID(unit);
    } else {
        match_addr.modid = mod;
    }

    match_addr.port = port;

    BCM_IF_ERROR_RETURN
        (_bcm_th3_delete_replace_flags_convert(unit, flags, &repl_flags));

    repl_flags |= BCM_L2_REPLACE_MATCH_DEST;

    return _bcm_th3_l2_replace(unit, repl_flags, int_flags, &match_addr, 0, 0,
                               0, NULL, NULL);
}

/*
 * Function:
 *	bcm_tomahawk3_l2_addr_delete_by_trunk
 * Description:
 *	Delete L2 entries associated with a trunk
 * Parameters:
 *	unit  - device unit
 *	tid   - trunk id
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_tomahawk3_l2_addr_delete_by_trunk(int unit, bcm_trunk_t tid, uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    TH3_L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.tgid = tid;
    match_addr.flags = BCM_L2_TRUNK_MEMBER;

    BCM_IF_ERROR_RETURN
        (_bcm_th3_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_DEST;

    return _bcm_th3_l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                               NULL, NULL);
}

/*
 * Function:
 *	bcm_tomahawk3_l2_addr_delete_by_mac_port
 * Description:
 *	Delete L2 entries associated with a MAC address and
 *	a destination module/port
 * Parameters:
 *	unit  - device unit
 *	mac   - MAC address
 *	mod   - module id
 *	port  - port
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_tomahawk3_l2_addr_delete_by_mac_port(int unit, bcm_mac_t mac,
                                         bcm_module_t mod, bcm_port_t port,
                                         uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    TH3_L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    sal_memcpy(match_addr.mac, mac, sizeof(bcm_mac_t));

    if (!BCM_GPORT_IS_SET(port) && mod == -1) {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        match_addr.modid = SOC_BASE_MODID(unit);
    } else {
        match_addr.modid = mod;
    }
    match_addr.port = port;
    BCM_IF_ERROR_RETURN
        (_bcm_th3_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_MAC | BCM_L2_REPLACE_MATCH_DEST;
    return _bcm_th3_l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                               NULL, NULL);
}

/*
 * Function:
 *	bcm_tomahawk3_l2_addr_delete_by_vlan_port
 * Description:
 *	Delete L2 entries associated with a VLAN and
 *	a destination module/port
 * Parameters:
 *	unit  - device unit
 *	vid   - VLAN id
 *	mod   - module id
 *	port  - port
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_tomahawk3_l2_addr_delete_by_vlan_port(int unit, bcm_vlan_t vid,
                                          bcm_module_t mod, bcm_port_t port,
                                          uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    TH3_L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.vid = vid;

    if (!BCM_GPORT_IS_SET(port) && mod == -1) {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        match_addr.modid = SOC_BASE_MODID(unit);
    } else {
        match_addr.modid = mod;
    }
    match_addr.port = port;
    BCM_IF_ERROR_RETURN
        (_bcm_th3_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_MATCH_DEST;
    return _bcm_th3_l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                               NULL, NULL);
}

/*
 * Function:
 *	bcm_tomahawk3_l2_addr_delete_by_vlan_trunk
 * Description:
 *	Delete L2 entries associated with a VLAN and a
 *      destination trunk.
 * Parameters:
 *	unit  - device unit
 *	vid   - VLAN id
 *	tid   - trunk group id
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_tomahawk3_l2_addr_delete_by_vlan_trunk(int unit, bcm_vlan_t vid,
                                           bcm_trunk_t tid, uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    TH3_L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.vid = vid;
    match_addr.tgid = tid;
    match_addr.flags = BCM_L2_TRUNK_MEMBER;

    BCM_IF_ERROR_RETURN
        (_bcm_th3_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_MATCH_DEST;
    return _bcm_th3_l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                               NULL, NULL);
}

/*
 * Function:
 *     bcm_tomahawk3_l2_replace
 * Description:
 *     This function replaces, or deletes destination for multiple L2 entries
 * Parameters:
 *     unit         device number
 *     flags        flags BCM_L2_REPLACE_*
 *     match_addr   L2 parameters to match on delete/replace
 *     new_module   new module ID for a replace (not used)
 *     new_port     new port for a replace
 *     new_trunk    new trunk for a replace
 * Return:
 *     BCM_E_XXX
 * Notes:
 * The following replace flags are supported on Tomahawk3 since there are no
 * requirements in this area (refer PRD), and/or there is no h/w support for
 * these flags (Example, flags related to VPN, pending flag and so on)
 */
int
bcm_tomahawk3_l2_replace(int unit, uint32 flags, bcm_l2_addr_t *match_addr,
                         bcm_module_t new_module, bcm_port_t new_port,
                         bcm_trunk_t new_trunk)
{
    TH3_L2_INIT(unit);

    if (!flags) {
        return BCM_E_PARAM;
    }

    if ((flags & (BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_AGE)) &&
        (flags & BCM_L2_REPLACE_NEW_TRUNK)) {
        return BCM_E_PARAM;
    }

    /* The following flags are not supported */
    if ((flags & BCM_L2_REPLACE_VPN_TYPE) ||
        (flags & BCM_L2_REPLACE_VLAN_AND_VPN_TYPE) ||
        (flags & BCM_L2_REPLACE_PENDING) ||
        (flags & BCM_L2_REPLACE_IGNORE_PENDING) ||
        (flags & BCM_L2_REPLACE_LEARN_LIMIT) ||
        (flags & BCM_L2_REPLACE_AGE)) {
        return BCM_E_UNAVAIL;
    }

    if ((flags & BCM_L2_REPLACE_DELETE) && (flags & BCM_L2_REPLACE_MATCH_MAC) &&
        ((flags & BCM_L2_REPLACE_MATCH_UC) ||
         (flags & BCM_L2_REPLACE_MATCH_MC))) {
        return BCM_E_PARAM;
    }

    /* matching L2 address allowed to be NULL together with */
    /* BCM_L2_REPLACE_DELETE flag indicating to delete all entries */
    if (NULL == match_addr) {
        if (!(flags & BCM_L2_REPLACE_DELETE) &&
            !(flags & (BCM_L2_REPLACE_SRC_HIT_CLEAR |
                       BCM_L2_REPLACE_DES_HIT_CLEAR))) {
            return BCM_E_PARAM;
        }
        flags &= ~(BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_MATCH_MAC |
                   BCM_L2_REPLACE_MATCH_DEST);
    }
    /* coverity[var_deref_op : FALSE] */
    /* coverity[var_deref_model : FALSE] */
    return _bcm_th3_l2_replace(unit, flags, 0, match_addr, new_module,
                                     new_port, new_trunk, NULL, NULL);
}


#if 0
#ifdef BCM_XGS_SWITCH_SUPPORT
/*
 * Function:
 *     _bcm_get_op_from_flags
 * Description:
 *     Helper function to _bcm_fb_age_reg_config to 
 *     decide on a PPA command according to flags
 * Parameters:
 *     flags        flags BCM_L2_REPLACE_* 
 *     op           PPA command to be programmed
 *     sync_op      Search Key for _soc_l2x_sync_delete_by
 * Return:
 *     BCM_E_NONE   - Success
 *     BCM_E_PARAM  - Failure
 */
int 
_bcm_get_op_from_flags(uint32 flags, uint32 *op, uint32 *sync_op)
{
    uint32 cmp_flags = 0, int_op = 0, int_sync_op = 0;
    int    rv = BCM_E_NONE;

    if (NULL == op) {
        return BCM_E_PARAM;
    }
    cmp_flags = flags & (BCM_L2_REPLACE_MATCH_VLAN |
                         BCM_L2_REPLACE_MATCH_DEST);
    switch (cmp_flags) {
        case BCM_L2_REPLACE_MATCH_VLAN: 
        {
            int_op = (flags & BCM_L2_REPLACE_DELETE ? XGS_PPAMODE_VLAN : 
                   XGS_PPAMODE_VLAN_REPLACE);
            int_sync_op = (flags & BCM_L2_REPLACE_DELETE ? SOC_L2X_VLAN_DEL : 
                           SOC_L2X_NO_DEL);
         break;
        }
        case BCM_L2_REPLACE_MATCH_DEST :
        {
            int_op = (flags & BCM_L2_REPLACE_DELETE ? XGS_PPAMODE_PORTMOD :
                   XGS_PPAMODE_PORTMOD_REPLACE);
            int_sync_op = (flags & BCM_L2_REPLACE_DELETE ? SOC_L2X_PORTMOD_DEL : 
                           SOC_L2X_NO_DEL);
            break;
        }
        case BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_MATCH_DEST:
        {
            int_op = (flags & BCM_L2_REPLACE_DELETE ? XGS_PPAMODE_PORTMOD_VLAN :
                   XGS_PPAMODE_PORTMOD_VLAN_REPLACE);
            int_sync_op = (flags & BCM_L2_REPLACE_DELETE ? SOC_L2X_PORTMOD_VLAN_DEL : 
                           SOC_L2X_NO_DEL);

            break;
        }
        default:
            rv = BCM_E_PARAM;
            break;
    }

    *op = int_op;
    *sync_op = int_sync_op;
    return rv;
}

#endif /* BCM_XGS_SWITCH_SUPPORT */

STATIC int
_soc_l2_sync_mem_cache(int unit, _bcm_l2_replace_t *rep_st)
{
    uint8 *vmap;
    soc_mem_t mem = L2Xm;
    uint32 *cache, *entry, *match_data, *match_mask;
    uint32 *new_data, *new_mask;
    int index_max, idx, i;
    int entry_dw = soc_mem_entry_words(unit, mem);
    int entry_words = soc_mem_entry_words(unit, mem);
    int copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    soc_field_t vld_fld = VALIDf;

    cache = SOC_MEM_STATE(unit, mem).cache[copyno];
    if (cache == NULL) {
        return BCM_E_NONE;
    }

    if (soc_feature(unit, soc_feature_base_valid)) {
        vld_fld = BASE_VALIDf;
    }
    vmap = SOC_MEM_STATE(unit, mem).vmap[copyno];
    index_max = soc_mem_index_max(unit, mem);
    for (idx = 0; idx <= index_max; idx++) {
        if (!CACHE_VMAP_TST(vmap, idx)) {
            continue;
        }
        entry = cache + (idx * entry_dw);
        if (!soc_mem_field32_get(unit, mem, entry, vld_fld)) {
            continue;
        }
        if (!soc_mem_field32_get(unit, mem, entry, STATIC_BITf)) {
            continue;
        }
        match_data = (uint32 *)&rep_st->match_data;
        match_mask = (uint32 *)&rep_st->match_mask;
        for (i = 0; i < entry_words; i++) {
            if ((entry[i] ^ match_data[i]) & match_mask[i]) {
                break;
            }
        }
        if (i != entry_words) {
            continue;
        }
        /* Match found, delete or replace entry */
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Match found in L2 bulk cache op: %d\n"), idx));
        if (rep_st->flags & BCM_L2_REPLACE_DELETE) {
            /* Delete op - invalidate the entry */
            CACHE_VMAP_CLR(vmap, idx);
        } else {
            /* Replace op - update entry */
            new_data = (uint32 *)&rep_st->new_data;
            new_mask = (uint32 *)&rep_st->new_mask;
            for (i = 0; i < entry_words; i++) {
                entry[i] ^= (entry[i] ^ new_data[i]) & new_mask[i];
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_esw_l2_station_add
 * Description:
 *     Add an entry to L2 Station Table      
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station_id   - (IN/OUT) Station ID
 *     station      - (IN) Pointer to station address information
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_station_add(int unit, int *station_id, bcm_l2_station_t *station)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWK(unit)
        || SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT3(unit)) {
        rv = bcm_tr_l2_station_add(unit, station_id, station);
    }
#endif

    return (rv);
}

/*
 * Function:
 *     bcm_esw_l2_station_delete
 * Description:
 *     Delete an entry from L2 Station Table
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station_id   - (IN) Station ID
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_station_delete(int unit, int station_id)
{

    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWK(unit)
        || SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT3(unit)) {
        rv = bcm_tr_l2_station_delete(unit, station_id);
    }
#endif
    return (rv);
}

/*
 * Function:
 *     bcm_esw_l2_station_delete_all
 * Description:
 *     Clear all L2 Station Table entries
 * Parameters:
 *     unit - (IN) BCM device number
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_station_delete_all(int unit)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWK(unit)
        || SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT3(unit)) {
        rv = bcm_tr_l2_station_delete_all(unit);
    }
#endif

    return (rv);
}

/*
 * Function:
 *     bcm_esw_l2_station_get
 * Description:
 *     Get L2 station entry detail from Station Table
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station_id   - (IN) Station ID
 *     station      - (OUT) Pointer to station address information.
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_station_get(int unit, int station_id, bcm_l2_station_t *station)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWK(unit)
        || SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT3(unit)) {
        rv = bcm_tr_l2_station_get(unit, station_id, station);
    }
#endif

    return (rv);
}

/*
 * Function:
 *     bcm_esw_l2_station_size_get
 * Description:
 *     Get size of L2 Station Table
 * Parameters:
 *     unit - (IN) BCM device number
 *     size - (OUT) L2 Station table size
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_station_size_get(int unit, int *size)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWK(unit)
        || SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT3(unit)) {
        rv = bcm_tr_l2_station_size_get(unit, size);
    }
#endif

    return (rv);
}

/*
 * Function:
 *     bcm_esw_l2_station_traverse
 * Description:
 *     Traverse L2 station entry from Station Table
 * Parameters:
 *     unit         - (IN) BCM device number
 *     trav_fn   -  (IN) Customer defines function is used to traverse
 *     user_data - (IN) Pointer to information of customer.
 * Return:
 *     BCM_E_XXX
 */
int 
bcm_esw_l2_station_traverse(int unit, bcm_l2_station_traverse_cb trav_fn, void *user_data)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TRIDENT2X(unit) ||
        SOC_IS_TOMAHAWK(unit) || SOC_IS_KATANAX(unit) ||
        SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT3(unit)) {
        rv = bcm_tr_l2_station_traverse(unit, trav_fn, user_data);
    }
#endif

    return (rv);
}

#endif

int
bcm_tomahawk3_l2_ring_replace( int unit, bcm_l2_ring_t *l2_ring)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_tomahawk3_l2_mac_port_create
 * Description:
 *     Create a MAC port.
 * Parameters:
 *     unit - (IN) BCM device number
 *     mac_port - (IN/OUT) MAC port info
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_l2_mac_port_create(int unit, bcm_l2_mac_port_t *mac_port)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_tomahawk3_l2_mac_port_destroy
 * Description:
 *     Destroy a MAC port.
 * Parameters:
 *     unit - (IN) BCM device number
 *     mac_port_id - (IN) MAC port GPORT ID
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_l2_mac_port_destroy(int unit, bcm_gport_t mac_port_id)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_tomahawk3_l2_mac_port_destroy_all
 * Description:
 *     Destroy all MAC ports.
 * Parameters:
 *     unit - (IN) BCM device number
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_l2_mac_port_destroy_all(int unit)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_tomahawk3_l2_mac_port_get
 * Description:
 *     Get info about a MAC port.
 * Parameters:
 *     unit - (IN) BCM device number
 *     mac_port - (IN/OUT) MAC port info
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_l2_mac_port_get(int unit, bcm_l2_mac_port_t *mac_port)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_tomahawk3_l2_mac_port_traverse
 * Description:
 *     Traverse MAC ports.
 * Parameters:
 *     unit - (IN) BCM device number
 *     cb   - (IN) Callback
 *     user_data - (IN) User data
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_l2_mac_port_traverse(int unit, bcm_l2_mac_port_traverse_cb cb,
        void *user_data)
{
    return BCM_E_UNAVAIL;
}

STATIC void
_bcm_th3_l2_vlan_gport_free(int unit, _bcm_l2_del_vlan_port_pair_t *l2_pair)
{
    int i;
    
    if (l2_pair) {
        if (l2_pair->mod_pair_arr) {
            sal_free(l2_pair->mod_pair_arr);
            l2_pair->mod_pair_arr = NULL;
        } 

        if (l2_pair->unit_pair_arr) {
            for (i = 0; i < BCM_VLAN_MAX + 2; i++) {
                sal_free(l2_pair->unit_pair_arr->tbm[i]);
                l2_pair->unit_pair_arr->tbm[i] = NULL;
            }
            sal_free(l2_pair->unit_pair_arr);
            l2_pair->unit_pair_arr = NULL;
        }
    }

    return;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *     _bcm_th3_is_gport_valid_type
 * Purpose:
 *     Returns TRUE if supplied gport is a valid VP type
 * Parameters:
 *     gport_type    -  (IN) Gport type to be validated
 * Returns:
 *     TRUE if 'gport' is a valid VP type, FALSE otherwise
 */

STATIC int
_bcm_th3_is_gport_valid_type(int gport_type)
{
    switch(gport_type) {
        case BCM_GPORT_MPLS_PORT:
        case BCM_GPORT_VLAN_PORT:
            return 1;
        default:
            return 0;
    }
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *     _bcm_th3_l2_vlan_gport_matched_delete
 * Purpose:
 *     L2 Traverse callback function to Purge L2 entry belongs
 *     to set of (vlan,port) tuple.
 * Parameters:
 *     unit       -  (IN) Device unit number
 *     info       -  (IN) Reference to L2 entry, delete on match
 *     user_data  -  (IN) User specified cookie
 * Returns:
 *     BCM_E_XXX
 * Note:
 *     If one of the elements of the (vlan,gport) pair is invalid
 *     (if vlan value is BCM_VLAN_INVALID or port value is BCM_GPORT_INVALID),
 *     it is a wildcard. 
 */
STATIC int
_bcm_th3_l2_vlan_gport_matched_delete(int unit, bcm_l2_addr_t *info,
                                      void *user_data)
{
    int                            rv = BCM_E_NONE;
    int                            entry_found = FALSE;
    _bcm_l2_del_vlan_port_pair_t  *l2_pair;
    int                            useGport;
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    int                            gport_type;
    bcm_port_t                     local_port;
#endif /* defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3) */

    l2_pair = (void *)user_data;

    if (l2_pair == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if ((info->flags & BCM_L2_STATIC) &&  
        (!(l2_pair->replace_flags & BCM_L2_REPLACE_MATCH_STATIC))) {
        _l2_dbg_unmatched_entries[unit]++;  
        return BCM_E_NONE;
    }

    if ((info->flags & BCM_L2_PENDING) &&  
        (!(l2_pair->replace_flags & BCM_L2_REPLACE_PENDING))) {
        _l2_dbg_unmatched_entries[unit]++;
        return BCM_E_NONE;
    }

    /* Assumption is that esw's version of switch control will be used on TH3 */
    BCM_IF_ERROR_RETURN
        (bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &useGport));

#if defined(INCLUDE_L3)
    /* Extract the gport_type */
    gport_type = (((info->port) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK);
    if (_bcm_th3_is_gport_valid_type(gport_type)) {
        /* Assumption is that esw's version of switch control will be used on
         * TH3
         */
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, info->port, &local_port));
    }
#endif /* defined(INCLUDE_L3) */

    if (BCM_VLAN_VEC_GET(l2_pair->del_by_vlan_vec, info->vid)) {
        /* Delete by vlan operation */
        entry_found = TRUE;
    } else if ((useGport) && (BCM_GPORT_IS_MODPORT(info->port)) &&
               BCM_PBMP_MEMBER (l2_pair->mod_pair_arr[info->modid].pbm[BCM_VLAN_MAX+1],
                                _SHR_GPORT_LOCAL_GET(info->port))) {
        /* Delete by (mod, gport) operation */
        entry_found = TRUE;
    } else if ((!useGport) && (!BCM_GPORT_IS_SET(info->port)) &&
               BCM_PBMP_MEMBER (l2_pair->mod_pair_arr[info->modid].pbm[BCM_VLAN_MAX+1],
                                info->port)) {
        /* Delete by (mod, port) operation */
        entry_found = TRUE;
    } else if ((useGport) && BCM_GPORT_IS_MODPORT(info->port) &&
               BCM_PBMP_MEMBER(l2_pair->mod_pair_arr[info->modid].pbm[info->vid],
                _SHR_GPORT_LOCAL_GET(info->port))) {
        /* Delete by (vlan, (mod, gport)) operation */
        entry_found = TRUE;
    } else if (((!useGport) && (!BCM_GPORT_IS_SET(info->port)) &&
                BCM_PBMP_MEMBER(l2_pair->mod_pair_arr[info->modid].pbm[info->vid],
                info->port))) {
        /* Delete by (vlan, (mod, port)) operation */
        entry_found = TRUE;
    } else if (info->flags & BCM_L2_TRUNK_MEMBER) {
        if (SHR_BITGET(l2_pair->unit_pair_arr[0].tbm[BCM_VLAN_MAX+1], info->tgid)) {
            /* Delete by (tid) operation */
            entry_found = TRUE;
        } else if (SHR_BITGET(l2_pair->unit_pair_arr[0].tbm[info->vid], info->tgid)) {
            /* Delete by (vlan, tid) operation */
            entry_found = TRUE;
        }
    }

    if (entry_found == TRUE) {
       /* Entry found - Delete the entry */

       rv = mbcm_driver[unit]->mbcm_l2_addr_delete(unit, info->mac,
                                                   info->vid);
       if (rv == BCM_E_NONE) {
           _l2_dbg_matched_entries[unit]++;

           /* When flag BCM_L2_REPLACE_NO_CALLBACKS is used by caller, the sync
            * (bcmL2X) thread has been stopped before this function is called,
            * thats why we explicitly delete entry from learn shadow table
            * below
            */
           if (l2_pair->replace_flags & BCM_L2_REPLACE_NO_CALLBACKS) {
               l2x_entry_t l2x_entry;

               sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
               soc_L2Xm_field32_set(unit, &l2x_entry, BASE_VALIDf, 0x1);
               soc_L2Xm_field32_set(unit, &l2x_entry, VLAN_IDf, info->vid);
               soc_L2Xm_mac_addr_set(unit, &l2x_entry, MAC_ADDRf, info->mac);
               soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                                    TH3_L2_HASH_KEY_TYPE_BRIDGE);
               if (info->flags & BCM_L2_TRUNK_MEMBER) {
                   soc_L2Xm_field32_set(unit, &l2x_entry, Tf, 0x1);
                   soc_L2Xm_field32_set(unit, &l2x_entry, TGIDf, info->tgid);
               } else {
                   soc_L2Xm_field32_set(unit, &l2x_entry, PORT_NUMf,
                                        info->port);
               }

                /* Remove the matching entry from learn shadow table */
                sal_mutex_take(SOC_CONTROL(unit)->l2x_lrn_shadow_mutex,
                               sal_mutex_FOREVER);
                soc_th3_lrn_shadow_delete(unit, &l2x_entry);
                sal_mutex_give(SOC_CONTROL(unit)->l2x_lrn_shadow_mutex);
           }

           if ((l2_pair->replace_flags & BCM_L2_REPLACE_NO_CALLBACKS) &&
               (soc_th3_l2x_appl_callback_ready(unit))) {

               /* Delete entry from application callback db */
               (void)soc_th3_l2x_appl_callback_entry_delete(unit, info->mac,
                                                            info->vid);
           }
       }
   } else {
       _l2_dbg_unmatched_entries[unit]++;
   }

   return rv;
}

/*
 * Function:
 *     bcm_tomahawk3_l2_addr_delete_by_vlan_gport_multi
 * Purpose:
 *     Purge L2 entries belong to set of (vlan,port) tuple.
 * Parameters:
 *     unit       -  (IN) Device unit number
 *     flags      -  (IN) Only accept BCM_L2_REPLACE_DELETE, BCM_L2_REPLACE_NO_CALLBACKS.
 *     num_pairs  -  (IN) Size of (vlan,port) pair array.
 *     vlan       -  (IN) Reference to array of VLAN list.
 *     gport      -  (IN) Reference to array of gport list.
 * Returns:
 *     BCM_E_XXX
 * Note:
 *     If one of the elements of the (vlan,gport) pair is invalid
 *     (if vlan value is BCM_VLAN_INVALID or port value is BCM_GPORT_INVALID),
 *     it is a wildcard.
 *     Entries are removed only if BCM_L2_REPLACE_DELETE flag is used. With
 *     flag BCM_L2_REPLACE_NO_CALLBACKS, l2 aging and learning are disabled 
 *     during this operation.
 */

int 
bcm_tomahawk3_l2_addr_delete_by_vlan_gport_multi(int unit, uint32 flags,
                                                 int num_pairs,
                                                 bcm_vlan_t *vlan,
                                                 bcm_gport_t *gport)
{
    sal_usecs_t                   interval=0;
    bcm_module_t                  mod_in, modid = 0;
    bcm_port_t                    port_in, port = 0;
    bcm_trunk_t                   trunk_id = 0;
    int                           id = 0, ct, i;
    int                           rv2 = 0, rv = BCM_E_UNAVAIL;
    soc_control_t                 *soc = SOC_CONTROL(unit);
    uint32                        max_modid;
    _bcm_l2_vlan_port_pair_t      *mod_l2_pair_arr = NULL;
    _bcm_l2_vlan_trunk_pair_t     *l2_trunk_pair_arr = NULL;
    _bcm_l2_del_vlan_port_pair_t  l2_pair;
    int                           alloc_size;

    /* Validate the input parameters */
    if (vlan == NULL || gport == NULL || num_pairs == 0) {
        return BCM_E_PARAM;
    }

    /* Validate flag parameter, no action will be proceeded with extra flags */
    if ((flags & ~(BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_NO_CALLBACKS | 
                   BCM_L2_REPLACE_MATCH_STATIC |
                   BCM_L2_REPLACE_PENDING )) != 0) {
        return BCM_E_NONE;
    }

    sal_memset(&l2_pair, 0x0, sizeof(l2_pair));
    
    /* Prepare (vlan, (mod, port)) pair */
    max_modid = SOC_MODID_MAX(unit) + 1;  
    alloc_size = sizeof(_bcm_l2_vlan_port_pair_t) * max_modid;
    mod_l2_pair_arr = sal_alloc(alloc_size, "l2_addr_vlan_gport_multi_delete");
    if (mod_l2_pair_arr == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(mod_l2_pair_arr, 0x0, alloc_size);

    /* Prepare (vlan, tid) pair */
    l2_pair.mod_pair_arr = mod_l2_pair_arr;
    alloc_size = sizeof(_bcm_l2_vlan_trunk_pair_t);
    l2_trunk_pair_arr = sal_alloc(alloc_size, "l2_addr_vlan_gport_multi_delete");
    if (l2_trunk_pair_arr == NULL) {
        rv = BCM_E_MEMORY;
        goto _free_exit;
    }
    sal_memset(l2_trunk_pair_arr, 0x0, alloc_size);
    l2_pair.unit_pair_arr = l2_trunk_pair_arr;
    
    /* Assumption is that the trunk module will have the same function as below
     * for getting trunk information
    */
    alloc_size = SHR_BITALLOCSIZE(_bcm_esw_trunk_group_num(unit));
    for (i = 0; i < BCM_VLAN_MAX + 2; i++) {
        l2_trunk_pair_arr->tbm[i] = sal_alloc(alloc_size, \
            "l2_addr_vlan_gport_multi_delete");
        if (l2_trunk_pair_arr->tbm[i] == NULL) {
            rv = BCM_E_MEMORY;
            goto _free_exit;
        }
        sal_memset(l2_trunk_pair_arr->tbm[i], 0x0, alloc_size);
    }

    /* Validate vlan & gport */
    for (ct = 0; ct < num_pairs; ct++) {
        if (vlan[ct] == BCM_VLAN_INVALID &&
            gport[ct] == BCM_GPORT_INVALID) {
            rv = BCM_E_PARAM;
            goto _free_exit;
        }

        if (vlan[ct] != BCM_VLAN_INVALID) {
            if (vlan[ct] > BCM_VLAN_MAX) {
                rv = BCM_E_PARAM;
                goto _free_exit;
            }
        }

        if (gport[ct] != BCM_GPORT_INVALID) {

            if (BCM_GPORT_IS_MODPORT(gport[ct])) {
                mod_in = BCM_GPORT_MODPORT_MODID_GET(gport[ct]);
                port_in = BCM_GPORT_MODPORT_PORT_GET(gport[ct]);
                modid = mod_in;
                port = port_in;
            } else {
                rv = _bcm_esw_gport_resolve(unit, gport[ct],
                                       &modid, &port, &trunk_id, &id);
                if (BCM_FAILURE(rv)) {
                    goto _free_exit;
                }
            
                if (BCM_GPORT_IS_TRUNK(gport[ct])) {
                    rv = _bcm_trunk_id_validate(unit, trunk_id);
                    if (BCM_FAILURE(rv)) {
                        rv = BCM_E_PARAM;
                        goto _free_exit;
                    }
                } else {
                    rv = BCM_E_PARAM;
                    goto _free_exit;
                }
            }
        }

        if (vlan[ct] == BCM_VLAN_INVALID) {
            if (BCM_GPORT_IS_TRUNK(gport[ct])) {
                SHR_BITSET(l2_pair.unit_pair_arr[0].tbm[BCM_VLAN_MAX+1], \
                    trunk_id);
            } else if (BCM_GPORT_IS_MODPORT(gport[ct])){
                BCM_PBMP_PORT_ADD( \
                    l2_pair.mod_pair_arr[modid].pbm[BCM_VLAN_MAX+1], port);
            }
        } else if (gport[ct] == BCM_GPORT_INVALID) {
            BCM_VLAN_VEC_SET(l2_pair.del_by_vlan_vec, vlan[ct]);
        } else {
            if (BCM_GPORT_IS_TRUNK(gport[ct])) {
                SHR_BITSET(l2_pair.unit_pair_arr[0].tbm[vlan[ct]], \
                    trunk_id);                
            } else /* if (BCM_GPORT_IS_MODPORT(gport[ct])) */{
                BCM_PBMP_PORT_ADD(l2_pair.mod_pair_arr[modid].pbm[vlan[ct]], \
                    port);
            }
        }
    }

    if (flags & BCM_L2_REPLACE_DELETE) {
        _l2_dbg_matched_entries[unit] = 0;
        _l2_dbg_unmatched_entries[unit] = 0;

        if (flags & BCM_L2_REPLACE_NO_CALLBACKS) {
            interval = soc->l2x_interval;
            rv = bcm_esw_l2_addr_freeze(unit);
            if (BCM_FAILURE(rv)) {
                goto _free_exit;
            }

            rv2 = soc_l2x_stop(unit);
            if (rv2 < 0) {
                goto _free_exit;
            }

            LOG_INFO(BSL_LS_BCM_ARL,
                     (BSL_META_U(unit, "L2 Freeze and stop L2 thread\n")));
        }

        l2_pair.replace_flags   = flags;

        rv = bcm_tomahawk3_l2_traverse(unit,
                                       _bcm_th3_l2_vlan_gport_matched_delete,
                                       &l2_pair);
        if (rv == BCM_E_NONE) {
            LOG_INFO(BSL_LS_BCM_ARL,
                     (BSL_META_U(unit,
                                 "Total number of L2 Entries: %d [Deleted:%d Left:%d]\n\r"),
                      _l2_dbg_matched_entries[unit] + _l2_dbg_unmatched_entries[unit],
                      _l2_dbg_matched_entries[unit], _l2_dbg_unmatched_entries[unit]));
        }

        if (flags & BCM_L2_REPLACE_NO_CALLBACKS) {
            LOG_INFO(BSL_LS_BCM_ARL,
                     (BSL_META_U(unit, "L2 Thaw and start L2 thread\n")));

            rv2 = soc_l2x_start(unit, soc->l2x_flags, interval);
            if (rv2 < 0) {
                goto _free_exit;
            }

            rv2 = bcm_esw_l2_addr_thaw(unit);
            if (rv2 < 0) {
                goto _free_exit;
            }
        }
    }

_free_exit:
    _bcm_th3_l2_vlan_gport_free(unit, &l2_pair);

    if (rv < 0) {
        return rv;
    } else if (rv2 < 0) {
        return rv2;
    } else {
        return rv;
    }
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
#if 0

/*
 * Function:
 *     _bcm_l2_sw_dump
 * Purpose:
 *     Displays L2 information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 * Note:
 *    The l2_data structure maintains registration of callbacks routines.
 *    Dump of l2_data is skipped.
 */
void
_bcm_l2_sw_dump(int unit)
{

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information L2 - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "  Initialized : %d\n"), _th3_l2_init[unit]));

    _bcm_th3_l2_sw_dump(unit);

    return;
}
#endif /* 0 */
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#ifdef BCM_WARM_BOOT_SUPPORT
/* Warmboot versions below must be as defined in src/bcm/esw/l2.c */
#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_2

/*
 * Function:
 *      _bcm_th3_l2_wb_alloc
 * Purpose:
 *      Allocated L2 module persistent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_wb_alloc(int unit)
{
    int                         sz = 0;
    soc_scache_handle_t         scache_handle;
    uint8                       *scache_ptr;
    int                         rv = BCM_E_NONE;
    int                         stable_size = 0;
    _bcm_l2_station_control_t   *station_control = NULL;
    int                         sw_aging_support;
    int                         underlay_max_entries = 0;
    int                         num_entries;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    /* Requires extended scache support warmboot */
    if (stable_size == 0) {
        return BCM_E_NONE;
    }

    rv = _bcm_th3_l2_station_control_get(unit, &station_control);
    if (BCM_FAILURE(rv) && (rv != BCM_E_INIT)) {
        return rv;
    }

    /* TH3 does not support SW aging. Added only for warmboot upgrade purpose.
     * See _bcm_esw_l2_wb_alloc
     */
    sw_aging_support = 1;

    if (sw_aging_support ||
        ((!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) && station_control)) {

        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_L2, 0);

        if (sw_aging_support) {
            /* aging time */
            sz += sizeof(int);
        }

        if ((!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) && station_control) {
            /* Underlay L2 station entry - sid/prio */
            /* For TH3, l2_station_entry_max_size_get and l2_station_size_get
             * would return same value, so we use l2_station_size_get
             * (we have not defined bcm_th3_l2_station_max_size_get)
             */
            BCM_IF_ERROR_RETURN(
                bcm_th3_l2_station_size_get(unit, &underlay_max_entries));
            sz += (underlay_max_entries * (sizeof(int) * 2));

        }

       /* Ver 1_2 introduces application callback supress feature for
        * entries added through L2 address add API. The setting of this
        * feature and * its related information is stored in scache
        */
        if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
            /* Setting of application callback suppress feature
             * (enable/disable)
             */
            sz += sizeof(int);

            /* Count of the number of entries in the internal database. We
             * store this because even if the feature is enabled, the database
             * may be empty due to multiple add-delete sequences
             */
            sz += sizeof(int);

            num_entries = soc_mem_index_count(unit, L2Xm);

            /* Bitmap of indices which were added by application */
            sz += SHR_BITALLOCSIZE(num_entries);

            /* Bitmap of callback state (enabled/disabled) corresponding to each
             * index in the above bitmap
             */
            sz += SHR_BITALLOCSIZE(num_entries);
        }

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                     sz, &scache_ptr,
                                     BCM_WB_DEFAULT_VERSION, NULL);

        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_l2_wb_sync
 * Purpose:
 *      Record L2 module persistent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th3_l2_wb_sync(int unit)
{
    int                         sz = 0;
    soc_scache_handle_t         scache_handle;
    uint8                       *scache_ptr = NULL;
    int                         aging_time = 0;
    _bcm_l2_station_entry_t     **entry_arr;
    _bcm_l2_station_control_t   *station_control = NULL;
    int                         stable_size = 0;
    int                         rv = BCM_E_NONE;
    int                         index = 0;
    int                         init_value = 0;
    int                         sw_aging_support; /* SW Learning supported */
    int                         underlay_max_entries = 0;
    SOC_SCACHE_DUMP_DECL;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    /* Requires extended scache support warmboot */
    if (stable_size == 0) {
        return BCM_E_NONE;
    }

    rv = _bcm_th3_l2_station_control_get(unit, &station_control);
    if (BCM_FAILURE(rv) && (rv != BCM_E_INIT)) {
        return rv;
    }

    /* TH3 does not support SW aging. Added only for warmboot upgrade purpose.
     * See _bcm_esw_l2_wb_sync
     */
    sw_aging_support = 1;

    if (sw_aging_support ||
        ((!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) && station_control)) {

        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_L2, 0);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                     0, &scache_ptr,
                                     BCM_WB_DEFAULT_VERSION, NULL);

        if (BCM_FAILURE(rv) && (rv != BCM_E_INTERNAL)) {
            return rv;
        }

        if (sw_aging_support) {
            /* aging time */
            BCM_IF_ERROR_RETURN(
                _bcm_tomahawk3_l2_age_timer_get(unit, &aging_time));

            SOC_SCACHE_DUMP_START(unit, bcm_module_name(unit, BCM_MODULE_L2),
                                  scache_ptr);

            sz = sizeof(int);
            SOC_SCACHE_DUMP_DATA_BEGIN("aging_time");
            sal_memcpy(scache_ptr, &aging_time, sz);
            SOC_SCACHE_DUMP_DATA_INT32d_V(scache_ptr);
            scache_ptr += sz;
            SOC_SCACHE_DUMP_DATA_END();
            SOC_SCACHE_DUMP_STOP();
        }

        if ((!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) && station_control) {
            BCM_IF_ERROR_RETURN(
                bcm_th3_l2_station_size_get(unit, &underlay_max_entries));
            /* Underlay L2 station entry - sid/prio */
            entry_arr = (station_control->entry_arr);

            SOC_SCACHE_DUMP_START(unit, bcm_module_name(unit, BCM_MODULE_L2),
                                  scache_ptr);
            SOC_SCACHE_DUMP_DATA_BEGIN("underlay_entry_idx;sid;prio");
            for (index = 0; index < underlay_max_entries; index++) {
                uint8 *scache_ptr_tmp = scache_ptr;
                if (entry_arr[index]) {
                    sal_memcpy(scache_ptr,
                               &(entry_arr[index]->sid), sizeof(int));
                    scache_ptr += sizeof(int);

                    sal_memcpy(scache_ptr,
                               &(entry_arr[index]->prio), sizeof(int));
                    scache_ptr += sizeof(int);
                } else {
                    sal_memcpy(scache_ptr, &init_value, sizeof(int));
                    scache_ptr += sizeof(int);

                    sal_memcpy(scache_ptr, &init_value, sizeof(int));
                    scache_ptr += sizeof(int);
                }
                SOC_SCACHE_DUMP_DATA_INT32d_TVV(scache_ptr_tmp, index);
            }
            SOC_SCACHE_DUMP_DATA_END();
            SOC_SCACHE_DUMP_STOP();
        }
    }

    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {

        /* Ver 1_2: Store state of application callback suppress feature and
         * the number of entries in the internal database. Next, store 2 bitmaps
         * - first is bitmap of indices of entries added by application, second
         * is the callback state (enable/disable) corresponding to each entry
         * (index).
         * Note this function modifies scache_ptr
         */
        BCM_IF_ERROR_RETURN(soc_th3_l2x_appl_callback_info_sync(unit,
                                                                &scache_ptr));

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_l2_wb_recover
 * Purpose:
 *      Recover L2 module persistent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_wb_recover(int unit)
{
    soc_scache_handle_t         scache_handle;
    uint8                       *scache_ptr = NULL;
    uint16                      recovered_ver = 0;
    int                         aging_time = 0;
    int                         rv = BCM_E_NONE;
    int                         additional_scache_size = 0;
    int                         sw_aging_support;
    int                         underlay_max_entries = 0;
    _bcm_l2_station_entry_t     **entry_arr;
    _bcm_l2_station_control_t   *station_control = NULL;
    int                         index = 0;
    int                         sid = 0;
    int                         pri = 0;
    int                         cur_last_sid = 0;
    int                         stable_size = 0;
    SOC_SCACHE_DUMP_DECL;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    /* Requires extended scache support warmboot */
    if (stable_size == 0) {
        return BCM_E_NONE;
    }

    rv = _bcm_th3_l2_station_control_get(unit, &station_control);
    if (BCM_FAILURE(rv) && (rv != BCM_E_INIT)) {
        return rv;
    }

    /* TH3 does not support SW aging. Added only for warmboot upgrade purpose.
     * See _bcm_esw_l2_wb_recover
     */
    sw_aging_support = 1;

    if (sw_aging_support ||
        ((!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) && station_control)) {

        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_L2, 0);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                     0, &scache_ptr,
                                     BCM_WB_DEFAULT_VERSION, &recovered_ver);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        } else if (BCM_E_NOT_FOUND == rv) {
            return _bcm_th3_l2_wb_alloc(unit);
        }

        if (scache_ptr != NULL) {
            if (sw_aging_support) {
                if (recovered_ver >= BCM_WB_VERSION_1_0) {
                    SOC_SCACHE_DUMP_START(unit, bcm_module_name(unit,
                                          BCM_MODULE_L2), scache_ptr);
                    /* aging time */
                    SOC_SCACHE_DUMP_DATA_BEGIN("aging_time");
                    sal_memcpy(&aging_time, scache_ptr, sizeof(int));
                    SOC_SCACHE_DUMP_DATA_INT32d_V(scache_ptr);
                    rv = _bcm_tomahawk3_l2_age_timer_set(unit, aging_time,
                                                         aging_time > 0);
                    if (BCM_FAILURE(rv)) {
                        SOC_SCACHE_DUMP_STOP();
                        return rv;
                    }
                    scache_ptr += sizeof(int);
                    SOC_SCACHE_DUMP_DATA_END();
                    SOC_SCACHE_DUMP_STOP();
                } else {
                    additional_scache_size += sizeof(int);
                }
            }

            if ((!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) && station_control) {
                /* We use bcm_th3_l2_station_size_get since the max_size_get
                 * version will be the same as size_get
                 */
                BCM_IF_ERROR_RETURN(
                    bcm_th3_l2_station_size_get(unit, &underlay_max_entries));

                /* Recover overlay/underlay L2 station entry */
                if (recovered_ver >= BCM_WB_VERSION_1_1) {
                    entry_arr = (station_control->entry_arr);

                    SOC_SCACHE_DUMP_START(unit, bcm_module_name(unit,
                                          BCM_MODULE_L2), scache_ptr);
                    SOC_SCACHE_DUMP_DATA_BEGIN("underlay_entry_idx;sid;prio");
                    for (index = 0; index < underlay_max_entries; index++) {
                        SOC_SCACHE_DUMP_DATA_INT32d_TVV(scache_ptr, index);
                        sal_memcpy(&sid, scache_ptr, sizeof(int));
                        scache_ptr += sizeof(int);

                        sal_memcpy(&pri, scache_ptr, sizeof(int));
                        scache_ptr += sizeof(int);

                        if (sid > cur_last_sid) {
                            cur_last_sid = sid;
                        }

                        if (entry_arr[index]) {
                            if (0 == sid) {
                                /*
                                 * Only recover the L2 station entries created
                                 * by bcm_l2_station APIs
                                 */
                                if (index < station_control->entries_total) {
                                    if (NULL != entry_arr[index]->tcam_ent) {
                                        sal_free(entry_arr[index]->tcam_ent);
                                    }

                                    sal_free(entry_arr[index]);

                                    entry_arr[index] = NULL;
                                    station_control->entry_count--;
                                    station_control->entries_free++;
                                }
                            } else {
                                entry_arr[index]->sid = sid;
                                entry_arr[index]->prio = pri;
                            }
                        }
                    }
                    SOC_SCACHE_DUMP_DATA_END();
                    SOC_SCACHE_DUMP_STOP();

                    rv = _bcm_th3_l2_station_entry_last_sid_set(unit, FALSE,
                                                          cur_last_sid);
                    if (BCM_FAILURE(rv)) {
                       return rv;
                    }
                } else {
                    /* Underlay L2 station entry - sid/prio */
                    additional_scache_size +=
                        (underlay_max_entries *(sizeof(int) * 2));
                }
            }

            if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
                if (recovered_ver >= BCM_WB_VERSION_1_2) {
                    /* Recreate callback database from scache contents.
                     * Note scache_ptr is modified by the function below
                     */
                    SOC_IF_ERROR_RETURN(
                        soc_th3_l2x_appl_callback_info_restore(unit,
                                                               &scache_ptr));
                } else {
                      int num_entries;
                      int sz;

                      num_entries = soc_mem_index_count(unit, L2Xm);
                      sz = SHR_BITALLOCSIZE(num_entries);

                      /* Enable setting */
                      additional_scache_size += sizeof(int);
                      /* Count setting */
                      additional_scache_size += sizeof(int);
                      /* Adjust pointer for two bitmaps */
                      additional_scache_size += sz;
                      additional_scache_size += sz;
                }
            }

            if (additional_scache_size > 0) {
                rv = soc_scache_realloc(unit, scache_handle,
                                        additional_scache_size);
                if(BCM_FAILURE(rv)) {
                   return rv;
                }
            }
        }
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *	bcm_tomahawk3_l2_addr_add
 * Description:
 *	Add a MAC address to the Switch Address Resolution Logic (ARL)
 *	port with the given VLAN ID and parameters.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	l2addr - Pointer to bcm_l2_addr_t containing all valid fields
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 * Notes:
 *	Use CMIC_PORT(unit) to associate the entry with the CPU.
 *	Use flag of BCM_L2_COPY_TO_CPU to send a copy to the CPU.
 *      Use flag of BCM_L2_TRUNK_MEMBER to set trunking (TGID must be
 *      passed as well with non-zero trunk group ID)
 */
int
bcm_tomahawk3_l2_addr_add(int unit, bcm_l2_addr_t *l2addr)
{
    TH3_L2_INIT(unit);


    /* Input parameters check. */
    if (NULL == l2addr) {
        return (BCM_E_PARAM);
    }

    if (l2addr->flags & BCM_L2_DISCARD_DST) {
        if (MAC_IS_MCAST(l2addr->mac)) {
            return (BCM_E_UNAVAIL);
        }
    }

    if (l2addr->flags & BCM_L2_LOCAL_CPU) {
        l2addr->port = CMIC_PORT(unit);
        l2addr->modid = 0;
    }

    if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
        BCM_IF_ERROR_RETURN(_bcm_trunk_id_validate(unit, l2addr->tgid));
    }

    if (SOC_L2X_GROUP_ENABLE_GET(unit)) {
        /* soc_feature_overlaid_address_class should be enabled */
        if (soc_feature(unit, soc_feature_overlaid_address_class) &&
            !BCM_L2_PRE_SET(l2addr->flags)) {
            if ((l2addr->group > SOC_OVERLAID_ADDR_CLASS_MAX(unit)) ||
                (l2addr->group < 0)) {
                return (BCM_E_PARAM);
            }
        } else if ((l2addr->group > SOC_ADDR_CLASS_MAX(unit)) ||
                   (l2addr->group < 0)) {
            return (BCM_E_PARAM);
        }
        if (!BCM_PBMP_IS_NULL(l2addr->block_bitmap)) {
            return (BCM_E_PARAM);
        }
    } else {
        if (l2addr->group)  {
            return (BCM_E_PARAM);
        }
    }

    SOC_IF_ERROR_RETURN
	(mbcm_driver[unit]->mbcm_l2_addr_add(unit, l2addr));

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_tomahawk3_l2_addr_delete
 * Description:
 *	Delete an L2 address (MAC+VLAN) from the device
 * Parameters:
 *	unit - device unit
 *	mac  - MAC address to delete
 *	vid  - VLAN id
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_tomahawk3_l2_addr_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid)
{
    TH3_L2_INIT(unit);

    /* Calls bcm_th3_l2_addr_delete defined above */
    return mbcm_driver[unit]->mbcm_l2_addr_delete(unit, mac, vid);
}

/*
 * Function:
 *	bcm_tomahawk3_l2_addr_get
 * Description:
 *	Given a MAC address and VLAN ID, check if the entry is present
 *	in the L2 table, and if so, return all associated information.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	mac - input MAC address to search
 *	vid - input VLAN ID to search
 *	l2addr - Pointer to bcm_l2_addr_t structure to receive results
 * Returns:
 *	BCM_E_NONE		Success (l2addr filled in)
 *	BCM_E_PARAM		Illegal parameter (NULL pointer)
 *	BCM_E_INTERNAL		Chip access failure
 *	BCM_E_NOT_FOUND	        Address not found (l2addr not filled in)
 */

int
bcm_tomahawk3_l2_addr_get(int unit, sal_mac_addr_t mac,
		                  bcm_vlan_t vid, bcm_l2_addr_t *l2addr)
{
    TH3_L2_INIT(unit);

    /* Calls bcm_th3_l2_addr_get defined above */
    SOC_IF_ERROR_RETURN(
        mbcm_driver[unit]->mbcm_l2_addr_get(unit, mac, vid, l2addr));

    return BCM_E_NONE;
}

/*
 * Function:
 * 	_bcm_th3_l2_addr_callback
 * Description:
 *	Callback used with chip addr registration functions.
 *	This callback calls all the top level client callbacks.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	l2addr
 *	insert
 *	userdata
 * Returns:
 */
STATIC void
_bcm_th3_l2_addr_callback(int unit,
		                  bcm_l2_addr_t *l2addr,
		                  int insert,
		                  void *userdata)
{
    th3_l2_data_t *ad = &th3_l2_data[unit];
    int i;

    if (ad->l2_mutex == NULL) {
        return;
    }

    L2_LOCK(unit);
    for(i = 0; i < L2_CB_MAX; i++) {
        if(ad->cb[i].fn) {
            ad->cb[i].fn(unit, l2addr, insert, ad->cb[i].fn_data);
        }
    }
    L2_UNLOCK(unit);
}

/*
 * Function:
 *	bcm_tomahawk3_l2_addr_register
 * Description:
 *	Register a callback routine that will be called whenever
 *	an entry is inserted into or deleted from the L2 address table
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal)
 *	fn - Callback function of type bcm_l2_addr_callback_t
 *	fn_data - Arbitrary value passed to callback along with messages
 * Returns:
 *	BCM_E_NONE		Success, handle valid
 *	BCM_E_MEMORY		Out of memory
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_tomahawk3_l2_addr_register(int unit,
                               bcm_l2_addr_callback_t fn,
                               void *fn_data)
{
    th3_l2_data_t *ad = &th3_l2_data[unit];
    int i;
    int rv = BCM_E_NONE;
    int usec;

    TH3_L2_INIT(unit);

    _bcm_th3_l2_cbs[unit] = _bcm_th3_l2_addr_callback;
    _bcm_th3_l2_cb_data[unit] = NULL;

    L2_MUTEX(unit);
    L2_LOCK(unit);

    /* Start L2x thread if it isn't running already. */
    if (!soc_l2x_running(unit, NULL, NULL)) {
        /* Use default value. Even for bcmsim we need interval to be
         * sufficiently small to track L2X table updates
         */
        usec = SOC_L2X_INTERVAL_DEFAULT;
        usec = soc_property_get(unit, spn_L2XMSG_THREAD_USEC, usec);
        rv = soc_l2x_start(unit, 0, usec);
        if ((BCM_FAILURE(rv)) && (rv != BCM_E_UNAVAIL)) {
            _bcm_th3_l2_cbs[unit] = NULL;
            _bcm_th3_l2_cb_data[unit] = NULL;
            L2_UNLOCK(unit);
            return(rv);
        }
        ad->flags |= _BCM_TH3_L2X_THREAD_STOP;
    }

    /* See if the function is already registered (with same data) */
    for (i = 0; i < L2_CB_MAX; i++) {
        if (ad->cb[i].fn == fn && ad->cb[i].fn_data == fn_data) {
            L2_UNLOCK(unit);
            return BCM_E_NONE;
        }
    }

    /* Not found; add to list. */
    for (i = 0; i < L2_CB_MAX; i++) {
        if (ad->cb[i].fn == NULL) {
            ad->cb[i].fn = fn;
            ad->cb[i].fn_data = fn_data;
            ad->cb_count++;
            break;
	    }
    }

    L2_UNLOCK(unit);
    return i >= L2_CB_MAX ? BCM_E_RESOURCE : BCM_E_NONE;
}

/*
 * Function:
 *	bcm_tomahawk3_l2_addr_unregister
 * Description:
 *	Unregister a previously registered callback routine.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	fn - Same callback function used to register callback
 *	fn_data - Same arbitrary value used to register callback
 * Returns:
 *	BCM_E_NONE		Success, handle valid
 *	BCM_E_MEMORY		Out of memory
 *	BCM_E_INTERNAL		Chip access failure
 * Notes:
 *	Both callback and userdata must match from original register call.
 */
int
bcm_tomahawk3_l2_addr_unregister(int unit,
                                 bcm_l2_addr_callback_t fn,
                                 void *fn_data)
{
    th3_l2_data_t *ad = &th3_l2_data[unit];
    int rv = BCM_E_NOT_FOUND;
    int	i;

    TH3_L2_INIT(unit);
    L2_MUTEX(unit);
    if (_bcm_th3_l2_cb_check[unit] != NULL) {
        sal_sem_take(_bcm_th3_l2_cb_check[unit], sal_sem_FOREVER);
    }
    L2_LOCK(unit);

    for (i = 0; i < L2_CB_MAX; i++) {
        if((ad->cb[i].fn == fn) && (ad->cb[i].fn_data == fn_data)) {
            rv = BCM_E_NONE;
            ad->cb[i].fn = NULL;
            ad->cb[i].fn_data = NULL;
            ad->cb_count--;
            if (ad->cb_count == 0) {
                _bcm_th3_l2_cbs[unit] = NULL;
                _bcm_th3_l2_cb_data[unit] = NULL;

                /* Stop l2x thread if callback registration started it. */
                if (ad->flags & _BCM_TH3_L2X_THREAD_STOP) {
                    rv = soc_l2x_stop(unit);
                    ad->flags &= ~_BCM_TH3_L2X_THREAD_STOP;
                }
            }
        }
    }

    L2_UNLOCK(unit);
    if (_bcm_th3_l2_cb_check[unit] != NULL) {
        sal_sem_give(_bcm_th3_l2_cb_check[unit]);
    }
    return (rv);
}

/*
 * Function:
 *     bcm_tomahawk3_l2_station_add
 * Description:
 *     Add an entry to L2 Station Table      
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station_id   - (IN/OUT) Station ID
 *     station      - (IN) Pointer to station address information
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_l2_station_add(int unit, int *station_id,
                             bcm_l2_station_t *station)
{
    int rv = BCM_E_UNAVAIL;

    rv = bcm_th3_l2_station_add(unit, station_id, station);

    return rv;
}

/*
 * Function:
 *     bcm_tomahawk3_l2_station_delete
 * Description:
 *     Delete an entry from L2 Station Table
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station_id   - (IN) Station ID
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_l2_station_delete(int unit, int station_id)
{

    int rv = BCM_E_UNAVAIL;

    rv = bcm_th3_l2_station_delete(unit, station_id);

    return (rv);
}

/*
 * Function:
 *     bcm_tomahawk3_l2_station_delete_all
 * Description:
 *     Clear all L2 Station Table entries
 * Parameters:
 *     unit - (IN) BCM device number
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_l2_station_delete_all(int unit)
{
    int rv = BCM_E_UNAVAIL;

    rv = bcm_th3_l2_station_delete_all(unit);

    return (rv);
}

/*
 * Function:
 *     bcm_tomahawk3_l2_station_get
 * Description:
 *     Get L2 station entry detail from Station Table
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station_id   - (IN) Station ID
 *     station      - (OUT) Pointer to station address information
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_l2_station_get(int unit, int station_id,
                             bcm_l2_station_t *station)
{
    int rv = BCM_E_UNAVAIL;

    rv = bcm_th3_l2_station_get(unit, station_id, station);

    return (rv);
}

/*
 * Function:
 *     bcm_tomahawk3_l2_station_size_get
 * Description:
 *     Get size of L2 Station Table
 * Parameters:
 *     unit - (IN) BCM device number
 *     size - (OUT) L2 Station table size
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_l2_station_size_get(int unit, int *size)
{
    int rv = BCM_E_UNAVAIL;

    rv = bcm_th3_l2_station_size_get(unit, size);

    return (rv);
}

/*
 * Function:
 *     bcm_tomahawk3_l2_station_traverse
 * Description:
 *     Traverse L2 station entry from Station Table
 * Parameters:
 *     unit         - (IN) BCM device number
 *     trav_fn   -  (IN) Customer defines function is used to traverse
 *     user_data - (IN) Pointer to information of customer.
 * Return:
 *     BCM_E_XXX
 */
int 
bcm_tomahawk3_l2_station_traverse(int unit, bcm_l2_station_traverse_cb trav_fn,
                                  void *user_data)
{
    int rv = BCM_E_UNAVAIL;

    rv = bcm_th3_l2_station_traverse(unit, trav_fn, user_data);

    return (rv);
}

/*
 * Function:
 *      bcm_tomahawk3_l2_tunnel_add
 * Purpose:
 *    Add a (MAC, VLAN) for tunnel/MPLS processing, frames
 *    destined to (MAC, VLAN) is subjected to TUNNEL/MPLS processing.
 * Parameters:
 *      unit - StrataXGS unit number
 *      mac  - MAC address
 *      vlan - VLAN ID
 */
int
bcm_tomahawk3_l2_tunnel_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
    TH3_L2_INIT(unit);

#if defined(BCM_MPLS_SUPPORT) && defined(INCLUDE_L3)
    VLAN_CHK_ID(unit, vlan);

    return bcm_td_metro_myStation_add(unit, mac, vlan, -1, 1);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_tomahawk3_l2_tunnel_delete
 * Purpose:
 *      Remove a tunnel processing indicator for an L2 address
 * Parameters:
 *      unit - StrataXGS unit number
 *      mac  - MAC address
      vlan - VLAN ID
 */

int
bcm_tomahawk3_l2_tunnel_delete(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
    TH3_L2_INIT(unit);

#if defined(BCM_MPLS_SUPPORT) && defined(INCLUDE_L3)
    VLAN_CHK_ID(unit, vlan);
    return bcm_td_metro_myStation_delete(unit, mac, vlan, -1, 1);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_tomahawk3_l2_tunnel_delete_all
 * Purpose:
 *      Remove all tunnel processing indicating L2 addresses
 * Parameters:
 *      unit - StrataXGS unit number
 */

int
bcm_tomahawk3_l2_tunnel_delete_all(int unit)
{
    TH3_L2_INIT(unit);

#if defined(BCM_MPLS_SUPPORT) && defined(INCLUDE_L3)
    return bcm_td_metro_myStation_delete_all(unit);
#else
    return BCM_E_UNAVAIL;
#endif
}
#endif /* BCM_TOMAHAWK3_SUPPORT */

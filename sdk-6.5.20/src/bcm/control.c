/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM Dispatch Control Operations (eg: attach, detach)
 */

#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <bcm/debug.h>

#include <soc/drv.h>
#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/drv.h>
#endif
#if defined(BCM_DFE_SUPPORT)
#include <soc/dfe/cmn/dfe_drv.h>
#endif
#if defined(BCM_DNXF_SUPPORT)
#include <soc/dnxf/cmn/dnxf_drv.h>
#endif

#include <bcm/error.h>

#include <bcm_int/dispatch.h>
#include <bcm_int/control.h>
#include <bcm_int/api_ref.h>
#include <bcm_int/api_xlate_port.h>


bcm_control_t        *bcm_control[BCM_CONTROL_MAX] = {0};
static uint32        bcm_control_available[BCM_CONTROL_MAX] = {0};

/* Attach/detach callback */
typedef struct bcm_control_cb_data_s {
    bcm_attach_cb_t cb;
    void *user_data;
} bcm_control_cb_data_t;

static bcm_control_cb_data_t bcm_control_cb[BCM_CONTROL_MAX];

/* Detach retry information */
static int bcm_control_detach_retry_set[BCM_CONTROL_MAX];
static bcm_detach_retry_t bcm_control_detach_retry[BCM_CONTROL_MAX];

/* Default detach retry 5 minutes, unless overridden by flags */
#ifdef BCM_DETACH_POLL_INTERVAL_USECS_DEFAULT
#define _DETACH_POLL_INTERVAL_USECS BCM_DETACH_POLL_INTERVAL_USECS_DEFAULT
#else
#define _DETACH_POLL_INTERVAL_USECS 100000 /* 100 msecs */
#endif
#ifdef BCM_DETACH_NUM_RETRIES_DEFAULT
#define _DETACH_NUM_RETRIES BCM_DETACH_NUM_RETRIES_DEFAULT
#else
#define _DETACH_NUM_RETRIES 3000
#endif
static bcm_detach_retry_t bcm_control_detach_retry_default = {
    _DETACH_POLL_INTERVAL_USECS,  /* Poll interval    */
    _DETACH_NUM_RETRIES           /* Number retries */
};

static sal_mutex_t    _bcm_control_lock[BCM_CONTROL_MAX];

#define CONTROL_LOCK(unit) \
    sal_mutex_take(_bcm_control_lock[unit], sal_mutex_FOREVER)
#define CONTROL_UNLOCK(unit) \
    sal_mutex_give(_bcm_control_lock[unit])

#ifdef BCM_CONTROL_API_TRACKING
/* true if unit valid */
uint32               bcm_control_unit_valid[BCM_CONTROL_MAX] = {0};

/* API reference count */
static uint32 bcm_control_unit_busy[BCM_CONTROL_MAX] = {0};

/* lock for bcm_control_unit_valid/busy */
static sal_mutex_t    _bcm_busy_lock[BCM_CONTROL_MAX];

#define BUSY_LOCK(unit) \
    sal_mutex_take(_bcm_busy_lock[unit], sal_mutex_FOREVER)
#define BUSY_UNLOCK(unit) \
    sal_mutex_give(_bcm_busy_lock[unit])

#endif /* BCM_CONTROL_API_TRACKING */


#define BCM_DEVICE_CONTROL_DESTROY(_unit_)                  \
        if (BCM_CONTROL(_unit_)->subtype != NULL) {       \
            sal_free(BCM_CONTROL(_unit_)->subtype);       \
        }                                                 \
        sal_free(BCM_CONTROL(_unit_));                    \
        BCM_CONTROL(_unit_) = NULL; 

/*
 * BCM Dispatch Type name table
 */
#define BCM_DLIST_ENTRY(_dtype)\
#_dtype, 

static const char* _bcm_dtype_names[] = {
#include <bcm_int/bcm_dlist.h>
    NULL, 
}; 

/*
 * Name to dispatch type
 */
static bcm_dtype_t
_bcm_type_find(const char* name)
{
    int i; 
    for(i = 0; i < bcmTypeCount; i++) {
        if(!sal_strcmp(_bcm_dtype_names[i], name)) {
            return i; 
        }       
    }
    return bcmTypeNone; 
}


/*
 * The following are functions need to be dispatchable by type
 * within this module only. 
 */

/* _attach */
#define BCM_DLIST_ENTRY(_dtype)\
extern int _bcm_##_dtype##_attach(int, char*); 
#include <bcm_int/bcm_dlist.h>

#define BCM_DLIST_ENTRY(_dtype)\
_bcm_##_dtype##_attach,

static int (*_dispatch_attach[])(int, char*) = {
#include <bcm_int/bcm_dlist.h>
};

/* _init */
#define BCM_DLIST_ENTRY(_dtype)\
extern int bcm_##_dtype##_init(int); 
#include <bcm_int/bcm_dlist.h>

#define BCM_DLIST_ENTRY(_dtype)\
bcm_##_dtype##_init,

static int (*_dispatch_init[])(int) = {
#include <bcm_int/bcm_dlist.h>
};

/* _detach */
#define BCM_DLIST_ENTRY(_dtype)\
extern int _bcm_##_dtype##_detach(int); 
#include <bcm_int/bcm_dlist.h>

#define BCM_DLIST_ENTRY(_dtype)\
_bcm_##_dtype##_detach,

static int (*_dispatch_detach[])(int) = {
#include <bcm_int/bcm_dlist.h>
};

/* run attach/detach callback for unit (or all units if unit is -1 */
STATIC int
bcm_attach_run(int unit, bcm_device_state_t state, bcm_attach_info_t *data)
{
    int rv = BCM_E_NONE;
    bcm_control_cb_data_t *cb_data = &bcm_control_cb[unit];

    if (cb_data->cb != NULL) {
        rv = cb_data->cb(unit, state, data, cb_data->user_data);
    }

    return rv;
}
/*
 * Function:
 *   _bcm_attach
 * Purpose:
 *   Creates control structure, related locks and calls the corresponding
 *   dispatch function.
 *
 * Parameters:
 *      unit    - BCM device number or -1.
 *      type    - Type of BCM API dispatch driver
 *      subtype - Argument to dispatch driver
 *      remunit - underlying remote unit
 *      early_attach  - Set, if doing an attach_early_txrx
 * Returns:
 *  BCM_E_XXX
 */
STATIC int
_bcm_attach(int unit, char *type, char *subtype, int remunit, int early_attach)

{
    int            rv;
    bcm_attach_info_t data;
    int len_subtype = 0;

    LOG_INFO(BSL_LS_BCM_ATTACH,
             (BSL_META_U(unit,
                         "STK %d: attach %s subtype %s as %d\n"),
              unit, (NULL !=type) ? type: "N/A", 
              (NULL != subtype) ? subtype:"N/A",
              remunit));

    /* Control lock initialization. */
    SAL_GLOBAL_LOCK;
    if (unit < 0) {    /* find a free unit */
        for (unit = 0; unit < BCM_CONTROL_MAX; unit++) {
            if (bcm_control_available[unit] == FALSE) {
                break;
            }
        }
        if (unit >= BCM_CONTROL_MAX) {
            SAL_GLOBAL_UNLOCK;
            return (BCM_E_FULL);
        }
    }

    if (_bcm_control_lock[unit] == NULL) {
        _bcm_control_lock[unit] = sal_mutex_create("bcm_control");
    }

    if (_bcm_control_lock[unit] == NULL) {
        SAL_GLOBAL_UNLOCK;
        return (BCM_E_MEMORY);
    }

    bcm_control_available[unit] = TRUE;
    SAL_GLOBAL_UNLOCK;


    CONTROL_LOCK(unit);
    /*
     *  Device attach can be done in two phases.
     *
     *    bcm_attach_early_txrx() can be called to attach just TX and RX
     *      modules so the application can continue CPU TX/RX while complete
     *      BCM initialization is in progress too.
     *
     *   bcm_attach() should follow any early attach operations so the SDK
     *     will be fully initialized and functional for further API calls.
     *
     *  The regular attach sequence can be performed with bcm_attach(). This
     *      will initialize all the modules before returning.
     *
     *  Similarly for detach also, bcm_detach_late_txrx() can be called to
     *    deinit all the modules except TX and RX so the application can
     *    continue packet TX/RX even during other modules are initialized.
     */



    /* Check if unit is already attached. */
    if (BCM_CONTROL(unit) != NULL) {
        /* Control structure is already intied when attach_early_txrx is called */
        if(!(BCM_CONTROL(unit)->attach_state == _bcmControlStateTxRxInited)) {
            CONTROL_UNLOCK(unit);
            return (BCM_E_EXISTS);
        }

    }

#ifdef BCM_CONTROL_API_TRACKING
    {
        int old_busy;
        
        /* Create BUSY lock */
        if (_bcm_busy_lock[unit] == NULL) {
            _bcm_busy_lock[unit] = sal_mutex_create("bcm_busy");
        }
        if (_bcm_busy_lock[unit] == NULL) {
            CONTROL_UNLOCK(unit);
            return (BCM_E_MEMORY);
        }

        /* clear busy flag */
        BUSY_LOCK(unit);
        old_busy = bcm_control_unit_busy[unit];
        bcm_control_unit_busy[unit] = 0;
        BUSY_UNLOCK(unit);
        if (old_busy != 0) {

            LOG_ERROR(BSL_LS_BCM_ATTACH,
                      (BSL_META_U(unit,
                                  "Unit %d busy at attach()\n"),
                       unit));

        }
    }
#endif

    data.unit = unit;
    data.type = type;
    data.subtype = subtype;
    data.remunit = remunit;

    if (!early_attach) {
        rv = bcm_attach_run(unit, bcmDeviceStateAttach, &data);
        if (BCM_FAILURE(rv)) {
            CONTROL_UNLOCK(unit);
            return (rv);
        }
    }

    if (BCM_CONTROL(unit) == NULL) {
        /* Allocate unit control structure. */
        BCM_CONTROL(unit) = sal_alloc(sizeof(bcm_control_t), "bcm_control");
        if (BCM_CONTROL(unit) == NULL) {
            CONTROL_UNLOCK(unit);
            return (BCM_E_MEMORY);
        }
        sal_memset(BCM_CONTROL(unit), 0, sizeof(bcm_control_t));
    }
    if (subtype != NULL) {
        len_subtype = sal_strlen(subtype);
        BCM_CONTROL(unit)->subtype = sal_alloc(len_subtype+1,
                                               "bcm_control subtype");
        if (BCM_CONTROL(unit)->subtype == NULL) {
            BCM_DEVICE_CONTROL_DESTROY(unit);
            CONTROL_UNLOCK(unit);                                   
            return (BCM_E_MEMORY);
        }
        sal_strncpy(BCM_CONTROL(unit)->subtype, subtype, len_subtype);
        if (len_subtype)
            BCM_CONTROL(unit)->subtype[len_subtype] = '\0';
    }

    /* 
     * Assign dtype based on type string. The string was used for the dispatch table search, 
     * so we leave this alone for backwards compatibility with the rest of the code that calls bcm_attach(). 
     */
    if((BCM_CONTROL(unit)->dtype = _bcm_type_find(type)) == bcmTypeNone) {
        BCM_DEVICE_CONTROL_DESTROY(unit); 
        CONTROL_UNLOCK(unit); 
        return (BCM_E_CONFIG); 
    }

    BCM_CONTROL(unit)->unit = remunit;
    BCM_CONTROL(unit)->name = type; 
    
    if (early_attach) {
        BCM_CONTROL(unit)->attach_state = _bcmControlStateTxRxInit;
    }
    rv = _dispatch_attach[BCM_DTYPE(unit)](unit, subtype);
    if (early_attach && BCM_SUCCESS(rv)) {
        BCM_CONTROL(unit)->attach_state = _bcmControlStateTxRxInited;
    }

    if (BCM_FAILURE(rv)) {
        BCM_DEVICE_CONTROL_DESTROY(unit);
        CONTROL_UNLOCK(unit);                                   
        return (rv);
    }
#ifdef BCM_CONTROL_API_TRACKING
    BUSY_LOCK(unit);
    bcm_control_unit_valid[unit] = TRUE;
    BUSY_UNLOCK(unit);
#endif

    CONTROL_UNLOCK(unit);
    return (unit);
}
/*
 * Function:
 *   bcm_attach_early_txrx
 * Purpose:
 *   Create a BCM unit instance and initialise tx and rx modules only.
 *   Can be used only during warmboot to reduce tx and rx inactivity time,
 *   for strataXGS devices.
 *
 * Parameters:
 *      unit    - BCM device number or -1.
 *      type    - Type of BCM API dispatch driver
 *      subtype - Argument to dispatch driver
 *      remunit - underlying remote unit
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_attach_early_txrx(int unit, char *type, char *subtype, int remunit)
{
#ifdef BCM_ESW_SUPPORT
    if ((type == NULL) && (SOC_IS_ESW(unit))) {
        type = "esw";
    } else if (sal_strncmp(type, "esw", strlen(type))) {
        return BCM_E_UNAVAIL;
    }
    return _bcm_attach(unit, type, subtype, remunit, 1);
#endif /* BCM_ESW_SUPPORT */

    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *   bcm_attach
 * Purpose:
 *   Create a BCM unit instance.
 * Parameters:
 *      unit    - BCM device number or -1.
 *      type    - Type of BCM API dispatch driver
 *      subtype - Argument to dispatch driver
 *      remunit - underlying remote unit
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_attach(int unit, char *type, char *subtype, int remunit)
{
    if ((NULL == type) && (-1 == unit)) {
#ifdef BCM_ESW_SUPPORT
        type = "esw";
#else
        return (BCM_E_CONFIG);
#endif
    }

    if (type == NULL) {
        if (SOC_IS_TOMAHAWK3(unit)) {
#if defined(BCM_TOMAHAWK3X_SUPPORT) && defined(INCLUDE_PKTIO)
            if (soc_feature(unit, soc_feature_sdklt_pktio)) {
                type = "tomahawk3x";
            } else
#endif
#ifdef    BCM_TOMAHAWK3_SUPPORT
            {
                type = "tomahawk3";
            }
#endif
        }

#ifdef BCM_SHADOW_SUPPORT
        else if (SOC_IS_SHADOW(unit)) {
            type = "shadow";
        }
#endif

#ifdef BCM_PETRA_SUPPORT
        else if (SOC_IS_ARAD(unit)) {
            type = "petra";
        }
#endif

#ifdef BCM_DNX_SUPPORT
        else if (SOC_IS_DNX(unit)) {
            type = "dnx";
        }
#endif

#ifdef BCM_DFE_SUPPORT
        else if (SOC_IS_DFE(unit)) {
            type = "dfe";
        }
#endif

#ifdef BCM_DNXF_SUPPORT
        else if (SOC_IS_DNXF(unit)) {
            type = "dnxf";
        }
#endif
#ifdef BCM_LTSW_SUPPORT
        else if (SOC_IS_LTSW(unit)) {
            type = "ltsw";
        }
#endif
#ifdef BCM_ESW_SUPPORT
        else {
            type = "esw";
        }
#endif
    }

    if (NULL == type) {
        BCM_DEVICE_CONTROL_DESTROY(unit);
        CONTROL_UNLOCK(unit);
        return (BCM_E_CONFIG);
    }

    return _bcm_attach(unit, type, subtype, remunit, 0);
}

#ifdef BCM_CONTROL_API_TRACKING
/* Wait for BCM units to become idle */
STATIC int
_bcm_detach_wait(int unit)
{
    int rv;
    int i;
    int busy;
    bcm_detach_retry_t retry;

    rv = bcm_detach_retry_get(unit, &retry);

    if (BCM_SUCCESS(rv)) {
        for (i = retry.num_retries; i > 0; i--) {
            BUSY_LOCK(unit);
            busy = bcm_control_unit_busy[unit];
            BUSY_UNLOCK(unit);
            if (busy != 0) {
                sal_usleep(retry.poll_usecs);
            } else {
                break;
            }
        }
        sal_usleep(retry.poll_usecs);
        rv = (i == 0) ? BCM_E_TIMEOUT : BCM_E_NONE;
    }

    return rv;
}
#endif
int
bcm_detach_late_txrx(int unit) {
    int rv = 0;
    if (!SOC_IS_ESW(unit)) {
        rv = BCM_E_UNAVAIL;
        return rv;
    }
#ifdef BCM_ESW_SUPPORT
    BCM_CONTROL(unit)->attach_state = _bcmControlStateDeinitLateTxRx;
    rv = _dispatch_detach[BCM_DTYPE(unit)](unit);
    if (BCM_SUCCESS(rv)) {
         BCM_CONTROL(unit)->attach_state = _bcmControlStateDeinitAll;
    }
#endif
    return rv;
}

int
bcm_detach(int unit)
{
    int        rv = BCM_E_NONE;
    bcm_attach_info_t data;

    LOG_INFO(BSL_LS_BCM_ATTACH,
             (BSL_META_U(unit,
                         "STK %d: unit being detached\n"),
              unit));

    BCM_IF_ERROR_RETURN(bcm_attach_check(unit));

    CONTROL_LOCK(unit);

#ifdef BCM_CONTROL_API_TRACKING
    /* any new calls on other threads will return BCM_E_UNIT */
    BUSY_LOCK(unit);
    bcm_control_unit_valid[unit] = FALSE;
    BUSY_UNLOCK(unit);

    /* wait for other threads to complete BCM calls */
    rv = _bcm_detach_wait(unit);
    if (BCM_FAILURE(rv)) {
        CONTROL_UNLOCK(unit);

        return rv;
    }
#endif
    rv = _dispatch_detach[BCM_DTYPE(unit)](unit); 

    /* Clean up port mappings */
    _bcm_api_xlate_port_cleanup(unit);

    data.unit = unit;
    data.type = BCM_CONTROL(unit)->name;
    data.subtype = BCM_CONTROL(unit)->subtype;
    data.remunit = BCM_CONTROL(unit)->unit;

    if (BCM_SUCCESS(rv)) {
        rv = bcm_attach_run(unit, bcmDeviceStateDetach, &data);
    }

    BCM_DEVICE_CONTROL_DESTROY(unit);

    CONTROL_UNLOCK(unit);
#ifdef BCM_CONTROL_API_TRACKING
    sal_mutex_destroy(_bcm_busy_lock[unit]);
    _bcm_busy_lock[unit] = NULL;
#endif
    sal_mutex_destroy(_bcm_control_lock[unit]);
    _bcm_control_lock[unit] = NULL;

    SAL_GLOBAL_LOCK;
    bcm_control_available[unit] = FALSE;
    SAL_GLOBAL_UNLOCK;

    return rv;
}

/* _match */
#define BCM_DLIST_ENTRY(_dtype)\
extern int _bcm_##_dtype##_match(int, char *, char *); 
#include <bcm_int/bcm_dlist.h>

#define BCM_DLIST_ENTRY(_dtype)\
_bcm_##_dtype##_match,

static int (*_dispatch_match[])(int, char *, char *) = {
#include <bcm_int/bcm_dlist.h>
};

int
bcm_find(char *type, char *subtype, int remunit)
{
    int    unit;

    for (unit = 0; unit < BCM_CONTROL_MAX; unit++) {
    if (BCM_CONTROL(unit) == NULL) {
        continue;
    }
    if (remunit != BCM_CONTROL(unit)->unit) {
        continue;
    }
    if (type != NULL &&
        sal_strcmp(type, BCM_TYPE_NAME(unit)) != 0) {
        continue;
    }
    if (subtype == NULL && BCM_CONTROL(unit)->subtype != NULL) {
        continue;
    }
    if (subtype != NULL && BCM_CONTROL(unit)->subtype == NULL) {
        continue;
    }
        if (subtype != NULL &&
            _dispatch_match[BCM_DTYPE(unit)]
            (unit, subtype, BCM_CONTROL(unit)->subtype) != 0) {
            continue;
        }
    return unit;
    }
    return BCM_E_NOT_FOUND;
}

int
bcm_attach_check(int unit)
{
    if (!BCM_CONTROL_UNIT_LEGAL(unit)) {
    return BCM_E_UNIT;
    }
    if (BCM_CONTROL(unit) == NULL) {
    return BCM_E_UNIT;
    }
    return BCM_E_NONE;
}

int
bcm_attach_max(int *max_units)
{
    int    unit;

    *max_units = -1;
    for (unit = 0; unit < BCM_CONTROL_MAX; unit++) {
    if (BCM_CONTROL(unit) != NULL) {
        *max_units = unit;
    }
    }
    return BCM_E_NONE;
}

int
bcm_init(int unit)
{
    int        rv = BCM_E_NONE;
    void    *api_ptr = (void *)bcm_api_tbl;
    int     init_done = FALSE;

    /* This should never be true; done to include api table */
    /* coverity[dead_error_condition] */
    /* coverity[dead_error_line] */
    if (api_ptr == NULL) {
        return 0;
    }

    if (!BCM_CONTROL_UNIT_LEGAL(unit)) {
        return BCM_E_UNIT;
    }
    if (BCM_CONTROL(unit) == NULL) {

        if (!SOC_UNIT_VALID(unit)) {
            return BCM_E_UNIT;
        }
        rv = bcm_attach(unit, NULL, NULL, unit);
        if (rv < 0) {
            return rv;
        }
        if (SOC_IS_XGS(unit) ||
            SOC_IS_DPP(unit) ||
            SOC_IS_DFE(unit) ||
            SOC_IS_DNXF(unit) ||
            SOC_IS_LTSW(unit)) {
            init_done = TRUE;
        }
    }

#if defined(BCM_TOMAHAWK3X_SUPPORT) && defined(INCLUDE_PKTIO)
    if (BCM_CONTROL(unit)) {
        if (SOC_IS_TOMAHAWK3(unit)) {
            char *type;

            if (soc_feature(unit, soc_feature_sdklt_pktio)) {
                type = "tomahawk3x";
            } else {
                type = "tomahawk3";
            }
            if (sal_strncmp(type, BCM_TYPE_NAME(unit), strlen(type) + 1) != 0) {
                rv = bcm_detach(unit);
                if (rv < 0) {
                    return rv;
                }
                /*
                 * Considering the case of config change of
                 * pktio_driver_type from streamlined to classic,
                 * DMA will be clean up in SDKLT from bcm_pktio_cleanup
                 * after soc dma init.
                 * Hence, need to do the dma initialization again.
                 */
                if (!soc_feature(unit, soc_feature_sdklt_pktio)) {
                    soc_dma_init(unit);
                }
                rv = bcm_attach(unit, NULL, NULL, unit);
                if (rv < 0) {
                    return rv;
                }
            }
        }
    }
#endif
    if (FALSE == init_done) {
        /* Initialize port mappings */
        _bcm_api_xlate_port_init(unit);

        rv = _dispatch_init[BCM_DTYPE(unit)](unit);
    }

#ifdef BCM_CUSTOM_INIT_F
    if(BCM_SUCCESS(rv)) {
        extern int BCM_CUSTOM_INIT_F (int unit); 
        rv = BCM_CUSTOM_INIT_F (unit); 
    }
#endif /* BCM_CUSTOM_INIT_F */

    return rv; 
}


/*
 * Attach and detach dispatchable routines.
 */
int
_bcm_null_attach(int unit, char *subtype)
{
    COMPILER_REFERENCE(subtype);

    BCM_CONTROL(unit)->capability |= BCM_CAPA_LOCAL;

    return BCM_E_NONE;
}

int
_bcm_null_detach(int unit)
{
    return BCM_E_NONE;
}


#ifdef    BCM_LOOP_SUPPORT

STATIC int
_bcm_loop_match(int unit, char *subtype_a, char *subtype_b)
{
    COMPILER_REFERENCE(unit);
    return sal_strcmp(subtype_a, subtype_b);
}

int
_bcm_loop_attach(int unit, char *subtype)
{
    int        dunit;
    uint32    dcap;

    COMPILER_REFERENCE(subtype);

    dunit = BCM_CONTROL(unit)->unit;

    BCM_CONTROL(unit)->chip_vendor = BCM_CONTROL(dunit)->chip_vendor;
    BCM_CONTROL(unit)->chip_device = BCM_CONTROL(dunit)->chip_device;
    BCM_CONTROL(unit)->chip_revision = BCM_CONTROL(dunit)->chip_revision;
    dcap = BCM_CONTROL(dunit)->capability;
    dcap &= ~(BCM_CAPA_REMOTE|BCM_CAPA_COMPOSITE);
    BCM_CONTROL(unit)->capability |= BCM_CAPA_LOCAL | dcap;
    return BCM_E_NONE;
}

int
_bcm_loop_detach(int unit)
{
    return BCM_E_NONE;
}
#endif    /* BCM_LOOP_SUPPORT */

int
bcm_unit_valid(int unit)
{
    return BCM_UNIT_VALID(unit);
}

int
bcm_unit_local(int unit)
{
    return (BCM_UNIT_VALID(unit) && BCM_IS_LOCAL(unit));
}

int
bcm_unit_remote(int unit)
{
    return (BCM_UNIT_VALID(unit) && BCM_IS_REMOTE(unit));
}

int
bcm_unit_max(void)
{
    return BCM_CONTROL_MAX;
}

/*
 * Get the local reference of a remote device for its controlling
 * CPU.
 */

int
bcm_unit_remote_unit_get(int unit, int *remunit)
{
    if (!BCM_UNIT_VALID(unit)) {
        return BCM_E_NOT_FOUND;
    }

    if (remunit == NULL) {
        return BCM_E_PARAM;
    }

    *remunit = BCM_CONTROL(unit)->unit;

    return BCM_E_NONE;
}

/*
 * Get the subtype string that identifies the CPU controlling
 * the given unit.  subtype must point to a preallocated buffer;
 * maxlen is the maximum number of bytes that will be copied.
 *
 * returns the number of bytes copied or < 0 if an error occurs.
 *
 * NOTE:  This is currently the CPU key (mac address) as a
 * formatted string like 00:11:22:33:44:55.  It should be
 * converted to a CPU DB key with cpudb_key_parse.
 */

int
bcm_unit_subtype_get(int unit, char *subtype, int maxlen)
{
    int minlen;
    int stlen;

    if (!BCM_UNIT_VALID(unit)) {
        return BCM_E_NOT_FOUND;
    }

    if ((subtype == NULL) || (maxlen <= 0)) {
        return BCM_E_PARAM;
    }

    if(BCM_CONTROL(unit)->subtype == NULL) {
        return BCM_E_NOT_FOUND;
    }

    stlen = sal_strlen(BCM_CONTROL(unit)->subtype) + 1;
    minlen = maxlen < stlen ? maxlen : stlen;

    sal_memcpy(subtype, BCM_CONTROL(unit)->subtype, minlen);

    return minlen;
}

/*
 * Function:
 *      bcm_attach_register
 * Purpose:
 *      Register a callback that will be called before
 *      BCM device attachment and after BCM device detachment.
 *
 *      When bcm_attach() is called, the registered callback routine is
 *      called before BCM device attachment. When bcm_detach() is called,
 *      the registered callback routine is called after BCM device detachment.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      cb          - (IN) Callback function to register.
 *      user_data   - (IN) Arbitrary value passed to callback function.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Registering for a unit less than zero will register the
 *      callback for all BCM units, otherwise the registration will be
 *      for a specific BCM unit.
 */
int
bcm_attach_register(int unit, bcm_attach_cb_t cb, void *user_data)
{
    int i;
    int first_index, last_index;

    if (unit >= BCM_CONTROL_MAX) {
        return BCM_E_UNIT;
    }

    if (unit < 0) {
        /* Register for all units */
        first_index = 0;
        last_index  = COUNTOF(bcm_control_cb) - 1;
    } else {
        first_index = unit;
        last_index  = unit;
    }

    SAL_GLOBAL_LOCK;
    for (i = first_index; i <= last_index; i++) {
        bcm_control_cb[i].cb        = cb;
        bcm_control_cb[i].user_data = user_data;
    }
    SAL_GLOBAL_UNLOCK;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_attach_unregister
 * Purpose:
 *      Unregister a previously registered attach callback.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      cb          - (IN) Callback function to unregister.
 *      user_data   - (IN) Arbitrary value passed to callback function.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Unregistering for a unit less than zero will unregister the
 *      callback for all BCM units, otherwise the operation will be done
 *      for a specific BCM unit.
 */
int
bcm_attach_unregister(int unit, bcm_attach_cb_t cb, void *user_data)
{
    int i;
    int first_index, last_index;
    int found = 0;

    if (unit >= BCM_CONTROL_MAX) {
        return BCM_E_UNIT;
    }

    if (unit < 0) {
        /* Unregister for all units */
        first_index = 0;
        last_index  = COUNTOF(bcm_control_cb) - 1;
    } else {
        first_index = unit;
        last_index  = unit;
    }

    SAL_GLOBAL_LOCK;
    for (i = first_index; i <= last_index; i++) {
        if ((bcm_control_cb[i].cb == cb) &&
            (bcm_control_cb[i].user_data == user_data)) {
            bcm_control_cb[i].cb        = NULL;
            bcm_control_cb[i].user_data = NULL;
            found = 1;
        }
    }
    SAL_GLOBAL_UNLOCK;

    if (!found) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_detach_retry_set
 * Purpose:
 *      Set the poll interval time between retries and the number of retries
 *      before bcm_detach() exits.
 *
 *      The detach process checks whether there is any thread executing
 *      an API on the unit before it proceeds to unit detachment.
 *      If there is any API currently being executed, it tries
 *      again after waiting for the specified poll interval time.
 *
 *     The default number of attempts for detach is 1 (num_retries = 1)
 * Parameters:
 *      unit  - (IN) Unit number
 *      retry - (IN) Poll interval and number of retries
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      A unit with a value less than zero will apply to all BCM units.
 *      Otherwise it will apply for a specific BCM unit.
 *      A NULL parameter will set back the default value.
 */
int
bcm_detach_retry_set(int unit, bcm_detach_retry_t *retry)
{
    int i;
    int first_index, last_index;
    bcm_detach_retry_t *detach_ptr = &bcm_control_detach_retry_default;

    if (unit >= BCM_CONTROL_MAX) {
        return BCM_E_UNIT;
    }

    if (unit < 0) {
        /* Register for all units */
        first_index = 0;
        last_index  = COUNTOF(bcm_control_detach_retry) - 1;
    } else {
        first_index = unit;
        last_index  = unit;
    }

    if (retry != NULL) {
        detach_ptr = retry;
    }

    SAL_GLOBAL_LOCK;
    for (i = first_index; i <= last_index; i++) {
        bcm_control_detach_retry[i]     = *detach_ptr;
        bcm_control_detach_retry_set[i] = TRUE;
    }
    SAL_GLOBAL_UNLOCK;

    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_detach_retry_get
 * Purpose:
 *      Get the poll interval time between retries and the number of retries
 *      before bcm_detach() exits.
 * Parameters:
 *      unit  - (IN) Unit number
 *      retry - (OUT) Poll interval and number of retries
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_detach_retry_get(int unit, bcm_detach_retry_t *retry)
{
    if ((unit < 0) || (unit >= BCM_CONTROL_MAX)) {
        return BCM_E_UNIT;
    }

    if (retry == NULL) {
        return BCM_E_PARAM;
    }

    SAL_GLOBAL_LOCK;
    /* If detach retry information was never set, then use default */
    if (bcm_control_detach_retry_set[unit]) {
        *retry = bcm_control_detach_retry[unit];
    } else {
        *retry = bcm_control_detach_retry_default;
    }
    SAL_GLOBAL_UNLOCK;

    return BCM_E_NONE;
}

#ifdef BCM_CONTROL_API_TRACKING
/*
 * Function:
 *      bcm_unit_refcount
 * Purpose:
 *      Modify the reference count for a unit, and return a validity flag
 * Parameters:
 *      unit  - (IN) Unit number
 *      delta - (IN) Change in reference count (typically 1 or -1)
 * Returns:
 *      TRUE is the unit is valid; FALSE if not
 */
int
bcm_unit_refcount(int unit, int delta)
{
    int flag = FALSE;

    if (BCM_CONTROL_UNIT_LEGAL(unit) && _bcm_busy_lock[unit] != NULL) {
        BUSY_LOCK(unit);
        flag = bcm_control_unit_valid[unit];
        bcm_control_unit_busy[unit] += delta;
        BUSY_UNLOCK(unit);
    }

    return flag;
}
#endif

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/cpri.h>

#ifdef CPRIMOD_SUPPORT
#include <soc/cprimod/cprimod.h>
#endif /* CPRIMOD_SUPPORT */

#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/esw/port.h>

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#endif /* PORTMOD_SUPPORT */

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/cpri_speed_scan.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#include <soc/scache.h>
#include <soc/types.h>

#define _BCM_PBMP_COPY(_dest_pbmp, _src_pbmp) \
    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) { \
        SOC_PBMP_WORD_SET(_dest_pbmp,  i, SOC_PBMP_WORD_GET(_src_pbmp, i)); \
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Define:
 *     SS_LOCK/SS_UNLOCK
 * Purpose:
 *     Serialization Macros for access to lc_cntl structure.
 */

#define SS_LOCK(unit) \
        if (speed_scan_control[unit]->lock) { \
            sal_mutex_take(speed_scan_control[unit]->lock, sal_mutex_FOREVER); \
        }

#define SS_UNLOCK(unit) \
        if (speed_scan_control[unit]->lock) { \
            sal_mutex_give(speed_scan_control[unit]->lock); \
        }

typedef struct port_scan_param_s {
    bcm_cpri_port_speed_t       speed_list[SPD_SCAN_MAX_SPEED_LIST]; /* speed ids */
    int                         last_status[SPD_SCAN_MAX_SPEED_LIST]; /*  last status, dbg only */
    uint32                      num_valid_speed;
    uint32                      curr_speed_index;
    _cpri_spd_scan_state_t      spd_scan_state;
 } port_scan_param_t ;

typedef struct speed_scan_handler_info_s {
    struct speed_scan_handler_info_s *next;
    void *user_data;
    bcm_cpri_speed_scan_handler_t handler;
} speed_scan_handler_info_t ;

/*
 * Speed Scan Module control structure
 */
typedef struct spd_scan_control_s {
    int               flags;         /* keep track of stuff.                */
    sal_mutex_t       lock;          /* Synchronization.                    */
    char              taskname[16];  /* Linkscan thread name.               */
    sal_thread_t      thread_id;     /* Linkscan thread id.                 */
    int               interval_us;   /* Time between scans (us).            */
    sal_sem_t         spd_scan_sema; /* Speed Scan semaphore.               */
    bcm_pbmp_t        pbmp_enabled;  /* Ports that are speed scan enabled. */
    bcm_pbmp_t        pbmp_active;   /* Ports that are in scanning state.  */
    port_scan_param_t port_info[SPD_SCAN_NUM_PORTS];
    int               linkscan_mode[SPD_SCAN_NUM_PORTS];    
    speed_scan_handler_info_t* handler_info;  /* Handler to list of callbacks */
} spd_scan_control_t;

STATIC spd_scan_control_t *speed_scan_control[SPD_SCAN_NUM_UNITS];


/*
 *  WARMBOOT related defines.
 */

#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_SPEED_SCAN_WB_VERSION_1_0               SOC_SCACHE_VERSION(1,0)
#define BCM_SPEED_SCAN_WB_CURRENT_VERSION           BCM_SPEED_SCAN_WB_VERSION_1_0

#define _SPEED_SCAN_PBMP_BYTE_MAX                   (SOC_PBMP_WORD_MAX * sizeof(uint32))

/*
 * Data format that saved in Scache.  Any change in this structure
 * will need extra handling.
 */
typedef struct bcm_cpri_speed_scan_scache_s {
    int               flags;                                /* WB_VERSION_1_0 */
    int               interval_us;                          /* WB_VERSION_1_0 */
    bcm_pbmp_t        pbmp_enabled;                         /* WB_VERSION_1_0 */
    bcm_pbmp_t        pbmp_active;                          /* WB_VERSION_1_0 */
    port_scan_param_t port_info[SPD_SCAN_NUM_PORTS];        /* WB_VERSION_1_0 */
    int               linkscan_mode[SPD_SCAN_NUM_PORTS];    /* WB_VERSION_1_0 */
} bcm_cpri_speed_scan_scache_t;


/*
 * Function:
 *      _bcm_esw_cpri_speed_scan_sync
 * Purpose:
 *      Record CPRI speed scan module persisitent info for  Warm Boot
 *
 * Warm Boot Version Map:
 *      Fields for previous version to be documented
 *  BCM_WB_VERSION_1_0
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_cpri_speed_scan_sync (int unit)
{
    int                 alloc_size;
    soc_scache_handle_t scache_handle;
    uint8               *speed_scan_scache;
    spd_scan_control_t  *spd_scan = speed_scan_control[unit];
    bcm_cpri_speed_scan_scache_t *speed_scan_scache_buffer;
    int                 i; /* needed for _BCM_PBMP_COPY */
    int                 port;
    int                 rv;

    SOC_SCACHE_HANDLE_SET(scache_handle,
                          unit, BCM_MODULE_CPRI, 0);

    alloc_size = sizeof(bcm_cpri_speed_scan_scache_t);

    rv = (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 alloc_size,
                                 &speed_scan_scache, BCM_SPEED_SCAN_WB_CURRENT_VERSION, NULL));

    if (BCM_SUCCESS(rv)) {

        if ((speed_scan_scache_buffer = sal_alloc(sizeof (bcm_cpri_speed_scan_scache_t), "speed_scan")) == NULL) {
            return BCM_E_MEMORY;
        }

        speed_scan_scache_buffer->flags          = spd_scan->flags;
        speed_scan_scache_buffer->interval_us    = spd_scan->interval_us;
        _BCM_PBMP_COPY(speed_scan_scache_buffer->pbmp_enabled, spd_scan->pbmp_enabled);
        _BCM_PBMP_COPY(speed_scan_scache_buffer->pbmp_active, spd_scan->pbmp_active);
        for (port = 0; port < SPD_SCAN_NUM_PORTS ; port++) {
            sal_memcpy(&(speed_scan_scache_buffer->port_info[port]),
                       &(spd_scan->port_info[port]),
                       sizeof(port_scan_param_t));

            speed_scan_scache_buffer->linkscan_mode[port] = spd_scan->linkscan_mode[port];
        }
        sal_memcpy(speed_scan_scache, speed_scan_scache_buffer,
                   sizeof(bcm_cpri_speed_scan_scache_t));
        sal_free(speed_scan_scache_buffer);
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_cpri_speed_scan_reload_
 * Purpose:
 *      Reload CPRI speed scan module persisitent info for  Warm Boot
 *
 * Warm Boot Version Map:
 *      Fields for previous version to be documented
 *  BCM_WB_VERSION_1_0
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_cpri_speed_scan_reload (int unit)
{
    int                 alloc_size;
    int                 realloc_size = 0;
    soc_scache_handle_t scache_handle;
    uint8               *speed_scan_scache;
    spd_scan_control_t  *spd_scan = speed_scan_control[unit];
    bcm_cpri_speed_scan_scache_t *speed_scan_scache_buffer;
    int                 i; /* needed for _BCM_PBMP_COPY */
    int                 port;
    uint16              recovered_ver;
    int                 rv;

    SOC_SCACHE_HANDLE_SET(scache_handle,
                          unit, BCM_MODULE_CPRI, 0);

    alloc_size = sizeof(bcm_cpri_speed_scan_scache_t);

    rv =  _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 alloc_size,
                                 &speed_scan_scache, BCM_SPEED_SCAN_WB_CURRENT_VERSION, &recovered_ver);

    if (BCM_SUCCESS(rv)) {

        if ((speed_scan_scache_buffer = sal_alloc(sizeof (bcm_cpri_speed_scan_scache_t), "speed_scan")) == NULL) {
            return BCM_E_MEMORY;
        }

        sal_memcpy(speed_scan_scache_buffer, speed_scan_scache, sizeof(bcm_cpri_speed_scan_scache_t));

        /* WB_VERSION_1_0 */
        spd_scan->flags         = speed_scan_scache_buffer->flags;
        spd_scan->interval_us   = speed_scan_scache_buffer->interval_us;
        _BCM_PBMP_COPY(spd_scan->pbmp_enabled, speed_scan_scache_buffer->pbmp_enabled);
        _BCM_PBMP_COPY(spd_scan->pbmp_active, speed_scan_scache_buffer->pbmp_active);
        for (port = 0; port < SPD_SCAN_NUM_PORTS ; port++) {
            sal_memcpy(&spd_scan->port_info[port],
                       &(speed_scan_scache_buffer->port_info[port]),
                       sizeof(port_scan_param_t));
            spd_scan->linkscan_mode[port] = speed_scan_scache_buffer->linkscan_mode[port];
        }
        sal_free(speed_scan_scache_buffer);
        /*
         * Allocate extra scache space/Future support
         */
        if (realloc_size > 0) {
            SOC_IF_ERROR_RETURN
                (soc_scache_realloc(unit, scache_handle, realloc_size));
        }
    } else if (BCM_E_NOT_FOUND == rv) {
        /*
         * No warmboot data is available, initialized those information.
         */
        spd_scan->flags         = 0;
        spd_scan->interval_us   = 0;
        BCM_PBMP_CLEAR(spd_scan->pbmp_enabled);
        BCM_PBMP_CLEAR(spd_scan->pbmp_active);
        for (port = 0; port < SPD_SCAN_NUM_PORTS ; port++) {
            sal_memset( &spd_scan->port_info[port], 0, sizeof(port_scan_param_t));
            spd_scan->linkscan_mode[port] = 0;
        }
    } else {
        return (rv);
    }
    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef CPRIMOD_SUPPORT
static int
_bcm_cpri_port_speed_t_to_speed (
    bcm_cpri_port_speed_t speed)
{
    int speed_i;

    switch(speed) {
    case BCM_CPRI_PORT_SPEED_1228P8:
        speed_i = 1228;
        break;
    case BCM_CPRI_PORT_SPEED_2457P6:
        speed_i = 2457;
        break;
    case BCM_CPRI_PORT_SPEED_3072P0:
        speed_i = 3072;
        break;
    case BCM_CPRI_PORT_SPEED_4915P2:
        speed_i = 4915;
        break;
    case BCM_CPRI_PORT_SPEED_6144P0:
        speed_i = 6144;
        break;
    case BCM_CPRI_PORT_SPEED_9830P4:
        speed_i = 9830;
        break;
    case BCM_CPRI_PORT_SPEED_10137P6:
        speed_i = 10137;
        break;
    case BCM_CPRI_PORT_SPEED_12165P12:
        speed_i = 12165;
        break;
    case BCM_CPRI_PORT_SPEED_24330P24:
        speed_i = 24330;
        break;
    default:
        speed_i = 1228;
        break;
    }

    return speed_i;
}


STATIC int
_speed_scan_speed_set(int unit, bcm_port_t port, bcm_cpri_port_speed_t speed_id)
{
    bcm_port_resource_t r[2];
    int rv;
    r[0].port = port;
    r[0].physical_port = -1;
    r[0].encap = BCM_PORT_ENCAP_CPRI;    /* port validate will check this.. */

    BCM_IF_ERROR_RETURN(bcm_esw_port_resource_get(unit, port, &(r[1])));
    /*
     * Convert the speed_id to speed first.
     */
    r[1].speed = _bcm_cpri_port_speed_t_to_speed(speed_id);
    r[1].encap = BCM_PORT_ENCAP_CPRI;
    r[1].flags = SOC_PORT_RESOURCE_REMAP; /* forced speed change. */

    BCM_IF_ERROR_RETURN(bcm_esw_port_enable_set(unit, port, FALSE));

    rv = bcm_esw_port_resource_multi_set(unit, 2, &(r[0]));
    BCM_IF_ERROR_RETURN(rv);

    BCM_IF_ERROR_RETURN(bcm_esw_port_enable_set(unit, port, TRUE));

    return BCM_E_NONE;
}

STATIC void
_speed_scan_link_update(int unit, bcm_port_t port, int link)
{
    bcm_cpri_port_speed_t speed=0;
    speed_scan_handler_info_t* handler_info;
    bcm_cpri_speed_scan_port_info_t port_info;
    spd_scan_control_t *spd_scan = speed_scan_control[unit];
    uint32 curr_speed_index=0;
    int rv = 0;


    curr_speed_index = spd_scan->port_info[port].curr_speed_index;
    speed = spd_scan->port_info[port].speed_list[curr_speed_index];

    if (link == 0) { /* link is down. */

        switch (spd_scan->port_info[port].spd_scan_state) {
            case _CPRI_SPD_SCAN_STATE_FOUND:
            case _CPRI_SPD_SCAN_STATE_RESOLVED:
                LOG_DEBUG(BSL_LS_BCM_CPRI,
                         (BSL_META("%u: Speed Lost %d %d \n"), sal_time_usecs(),curr_speed_index, speed));

                /*
                 * save current linkscan mode and disable the link scan to start
                 * the speed scan.
                 */
                rv = bcm_esw_port_linkscan_get(unit, port, &(spd_scan->linkscan_mode[port]));
                if (BCM_FAILURE(rv)) {
                    LOG_DEBUG(BSL_LS_BCM_CPRI,
                              (BSL_META_U(unit,"bcm_esw_port_linkscan_get failed \n")));
                } else {
                    rv = bcm_esw_port_linkscan_set(unit, port, 0);
                    if (BCM_FAILURE(rv)) {
                        LOG_DEBUG(BSL_LS_BCM_CPRI,
                                (BSL_META_U(unit,"bcm_esw_port_linkscan_set failed \n")));
                    }
                }
                spd_scan->port_info[port].spd_scan_state = _CPRI_SPD_SCAN_STATE_SCANNING;
                SS_LOCK(unit);
                BCM_PBMP_PORT_ADD(spd_scan->pbmp_active, port);
                SS_UNLOCK(unit);

                /* 
                 * Deliver callback in this thread context.
                 */
                port_info.link_status = FALSE;
                /* speed is irrelevant in link down case */
                port_info.speed       = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_INVALID;

                handler_info = spd_scan->handler_info;

                while (handler_info != NULL){
                    handler_info->handler(unit, port, &port_info, handler_info->user_data);
                    handler_info = handler_info->next;
                }

                break;

            case _CPRI_SPD_SCAN_STATE_SCANNING:
                /*
                 * current speed didn't come up, try next speed.
                 */
                spd_scan->port_info[port].curr_speed_index += 1;
                if ( spd_scan->port_info[port].curr_speed_index >= spd_scan->port_info[port].num_valid_speed){
                    spd_scan->port_info[port].curr_speed_index = 0;
                }

                break;
            default:
                /* shouldn't be here. */
                break;
        }
    } else { /* Link  is Up. */

        switch (spd_scan->port_info[port].spd_scan_state) {

            case _CPRI_SPD_SCAN_STATE_SCANNING:

                LOG_DEBUG(BSL_LS_BCM_CPRI,
                         (BSL_META("%u: Speed found %d %d \n"), sal_time_usecs(),curr_speed_index, speed));

                /*
                 * Configure the  newly found speed.
                 */
                rv = _speed_scan_speed_set(unit, port, speed);
                if (BCM_FAILURE(rv)) {
                    /*
                     * If the speed configuration failed, try next speed.
                     * Do NOT transition to next state.
                     */
                    LOG_DEBUG(BSL_LS_BCM_CPRI,
                            (BSL_META("Speed Configuration Failed. %d %d \n"), curr_speed_index, speed));
                    spd_scan->port_info[port].curr_speed_index += 1;
                    if ( spd_scan->port_info[port].curr_speed_index >= spd_scan->port_info[port].num_valid_speed){
                        spd_scan->port_info[port].curr_speed_index = 0;
                    }
                } else {
                    spd_scan->port_info[port].spd_scan_state = _CPRI_SPD_SCAN_STATE_FOUND;
                    SS_LOCK(unit);
                    BCM_PBMP_PORT_REMOVE(spd_scan->pbmp_active, port);
                    SS_UNLOCK(unit);
                }
                break;

            case _CPRI_SPD_SCAN_STATE_FOUND:
                spd_scan->port_info[port].spd_scan_state = _CPRI_SPD_SCAN_STATE_RESOLVED;
                rv = bcm_esw_port_linkscan_set(unit, port, spd_scan->linkscan_mode[port]);
                if (BCM_FAILURE(rv)) {
                    LOG_DEBUG(BSL_LS_BCM_CPRI,
                              (BSL_META_U(unit,"bcm_esw_port_linkscan_set failed \n")));
                }

                /* Deliver callback.... in this thread context */

                LOG_DEBUG(BSL_LS_BCM_CPRI,
                         (BSL_META("%u: Speed Resolved %d\n"), sal_time_usecs(),speed));
                port_info.link_status = TRUE;
                port_info.speed       = speed;

                handler_info = spd_scan->handler_info;

                while (handler_info != NULL){
                    handler_info->handler(unit, port, &port_info, handler_info->user_data);
                    handler_info = handler_info->next;
                }

                break;
            case _CPRI_SPD_SCAN_STATE_RESOLVED:
                /* nothing to do here. */

                break;
            default:
                /* shouldn't be here. */
                break;

        }
    }
}

STATIC void
speed_scan_thread (int unit)
{
    int rv;
    int link;
    bcm_port_t port;
    spd_scan_control_t *spd_scan = speed_scan_control[unit];
    bcm_cpri_port_speed_t speed;
    uint32 index;

    while (SPD_SCAN_F_ENABLE_GET(spd_scan->flags)) {
        BCM_PBMP_ITER(spd_scan->pbmp_active, port) {

            index = spd_scan->port_info[port].curr_speed_index;
            speed = spd_scan->port_info[port].speed_list[index];

            /*
             * Disable the port.
             * Lock the counter
             * Change Speed
             * ReEnable the port.
             */
            rv = bcm_esw_port_enable_set(unit, port, FALSE);
            if (BCM_FAILURE(rv)) {
                /*
                 * skip the speed set if failed to enabled.
                 */
                LOG_ERROR( BSL_LS_BCM_CPRI,
                         ( BSL_META_U(unit,
                           "Failed to Disable port=%d : %s\n"), port,
                             bcm_errmsg(rv)));
                continue;
            }

            COUNTER_LOCK(unit);

            rv = portmod_cpri_port_speed_set(unit, port, speed);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR( BSL_LS_BCM_CPRI,
                        ( BSL_META_U(unit,
                        "Failed setting rate for port=%d, rate=%d: %s\n"), port,
                        spd_scan->port_info[port].speed_list[index], bcm_errmsg(rv)));
            }
            COUNTER_UNLOCK(unit);

            rv = bcm_esw_port_enable_set(unit, port, TRUE);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR( BSL_LS_BCM_CPRI,
                     ( BSL_META_U(unit,
                       "Failed to Enable port=%d : %s\n"), port,
                         bcm_errmsg(rv)));
            }
        }

        sal_sem_take(spd_scan->spd_scan_sema, spd_scan->interval_us);

        if (!(SPD_SCAN_F_ENABLE_GET(spd_scan->flags))) {
            /* Mode has been reconfigured, stop the thread */
            break;
        }

        BCM_PBMP_ITER(spd_scan->pbmp_enabled, port) {
            rv = bcm_esw_port_link_status_get(unit, port, &link);
            if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_CPRI,
                      (BSL_META_U(unit,
                                  "Failed getting link status: port=%d\n"), port));
            } else {
                /*
                 * Run the link update only when status get is successful.
                 */
                _speed_scan_link_update(unit, port, link);
            }
            if (!(SPD_SCAN_F_ENABLE_GET(spd_scan->flags))) {
                /* Mode has been reconfigured, stop the thread */
                break;
            }
        }
    }
    LOG_DEBUG(BSL_LS_BCM_CPRI,
             (BSL_META_U(unit, "CPRI rate sel thread exiting\n")));
    spd_scan->thread_id = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}
#endif
/*
 * Start of exposed APIs
 */

/* Initialize the CPRI Speed Scan. */
int bcm_esw_cpri_speed_scan_init (int unit)
{
#ifdef CPRIMOD_SUPPORT
    spd_scan_control_t *spd_scan  = speed_scan_control[unit];

#ifdef BCM_WARM_BOOT_SUPPORT
    int                 alloc_size;
    soc_scache_handle_t scache_handle;
    uint8               *speed_scan_scache;
    int                 rv = 0;
#endif

    if (spd_scan != NULL) {
        /* Already been initailized.  */
        return BCM_E_NONE;
    }
    spd_scan = sal_alloc(sizeof(spd_scan_control_t), "CPRI speed scan control");
    if (spd_scan == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(spd_scan, 0, sizeof(spd_scan_control_t));

    spd_scan->spd_scan_sema = sal_sem_create("CPRI speed scan", sal_sem_BINARY, 0);
    if (spd_scan->spd_scan_sema == NULL) {
        sal_free(spd_scan);
        return BCM_E_MEMORY;
    }

    
    speed_scan_control[unit] = spd_scan;
    SPD_SCAN_F_INIT_SET(spd_scan->flags);
    spd_scan->thread_id = SAL_THREAD_ERROR;

#ifdef BCM_WARM_BOOT_SUPPORT

    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_cpri_speed_scan_reload(unit));
    } else {
        SOC_SCACHE_HANDLE_SET(scache_handle,
                              unit, BCM_MODULE_CPRI, 0);

        alloc_size = sizeof(bcm_cpri_speed_scan_scache_t);

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                     alloc_size,
                                     &speed_scan_scache, BCM_SPEED_SCAN_WB_CURRENT_VERSION, NULL);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* CPRI Speed Scan unit level configuration. */
int bcm_esw_cpri_speed_scan_config_set(
    int unit,
    int interval_in_us,
    int enable)
{
#ifdef CPRIMOD_SUPPORT
    spd_scan_control_t *spd_scan  = speed_scan_control[unit];

    if (spd_scan == NULL) {
        return BCM_E_INIT;
    }

    if (enable) {
        if (interval_in_us < BCM_SPEED_SCAN_INTERVAL_MIN) {
            LOG_WARN(BSL_LS_BCM_CPRI,
                (BSL_META_U(unit, "Interval is less than minumumi %d, setting to minimum\n"),
                 BCM_SPEED_SCAN_INTERVAL_MIN));
            spd_scan->interval_us = BCM_SPEED_SCAN_INTERVAL_MIN;
        } else {
            spd_scan->interval_us = interval_in_us;
        }

        if (spd_scan->thread_id != SAL_THREAD_ERROR) {
            SPD_SCAN_F_ENABLE_SET(spd_scan->flags);
            sal_sem_give(spd_scan->spd_scan_sema);
            return (BCM_E_NONE);
        }

        sal_snprintf(spd_scan->taskname,
                     sizeof (spd_scan->taskname),
                    "bcmSpeedScan.%d",
                    unit);

        SPD_SCAN_F_ENABLE_SET(spd_scan->flags);
        spd_scan->thread_id = sal_thread_create(spd_scan->taskname,
                                                SAL_THREAD_STKSZ,
                                                SPD_SCAN_THREAD_PRI,   /* thread priority */
                                                (void (*)(void*))speed_scan_thread,
                                                INT_TO_PTR(unit));

        if (spd_scan->thread_id == SAL_THREAD_ERROR) {
            LOG_ERROR(BSL_LS_BCM_CPRI,
                (BSL_META_U(unit, "Could not create CPRI speed_scan_thread thread\n")));
            return BCM_E_MEMORY;
        }

    } else { /* Disable */
        SPD_SCAN_F_ENABLE_CLR(spd_scan->flags);
        sal_sem_give(spd_scan->spd_scan_sema);
        /*
         * sleep to give the thread enough time
         * to properly shut down.
         */
        sal_usleep(100000);
        if (spd_scan->thread_id != SAL_THREAD_ERROR) {
               LOG_ERROR(BSL_LS_BCM_CPRI,
                          (BSL_META_U(unit,
                                      "bcm_esw_cpri_speed_scan_config_set : thread did not exit\n")));
        }

    }
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* CPRI Speed Scan unit level configuration. */
int bcm_esw_cpri_speed_scan_config_get(
    int unit,
    int *interval_in_us,
    int *enable)
{
#ifdef CPRIMOD_SUPPORT
    spd_scan_control_t *spd_scan  = speed_scan_control[unit];

    if (spd_scan == NULL) {
        return BCM_E_INIT;
    }

    if (SPD_SCAN_F_ENABLE_GET(spd_scan->flags)) {
        *enable         = TRUE;
        *interval_in_us = spd_scan->interval_us;
    } else {
        *enable         = FALSE;
        *interval_in_us = 0;
    }
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* CPRI Adding port to speed scan list. */
int bcm_esw_cpri_speed_scan_port_config_add(
    int unit,
    bcm_port_t port,
    bcm_cpri_port_speed_t *speed_list,
    int num_speed)
{
#ifdef CPRIMOD_SUPPORT
    spd_scan_control_t *spd_scan  = speed_scan_control[unit];
    int spd_index;

    if (spd_scan == NULL) {
        return BCM_E_INIT;
    }

    if (num_speed > SPD_SCAN_MAX_SPEED_LIST) {
        LOG_ERROR(BSL_LS_BCM_CPRI,
                (BSL_META_U(unit, " The number of speed it can support is  %d.\n"),
                 SPD_SCAN_MAX_SPEED_LIST));
        return BCM_E_PARAM;

    }

    if (BCM_PBMP_MEMBER(spd_scan->pbmp_enabled, port)) {
        /* Already configured */
        return BCM_E_NONE;
    }

    if (num_speed > 0) {
        spd_scan->port_info[port].num_valid_speed = num_speed;
        for( spd_index = 0; spd_index < num_speed ; spd_index++) {
            spd_scan->port_info[port].speed_list[spd_index] = speed_list[spd_index];
        }
    }

    /* Save linkscan mode */
    BCM_IF_ERROR_RETURN(bcm_esw_port_linkscan_get(unit, port, &(spd_scan->linkscan_mode[port])));
    /* Remove port from linkscan as port will be scanned */
    BCM_IF_ERROR_RETURN(bcm_esw_port_linkscan_set(unit, port, 0));

    SS_LOCK(unit);
    BCM_PBMP_PORT_ADD(spd_scan->pbmp_enabled, port);
    BCM_PBMP_PORT_ADD(spd_scan->pbmp_active, port);
    SS_UNLOCK(unit);

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* CPRI Delete port from speed scan list. */
int bcm_esw_cpri_speed_scan_port_config_delete(
    int unit,
    bcm_port_t port)
{
#ifdef CPRIMOD_SUPPORT
    spd_scan_control_t *spd_scan  = speed_scan_control[unit];

    if (spd_scan == NULL) {
        return BCM_E_INIT;
    }

    /* Restore linkscan mode */
    BCM_IF_ERROR_RETURN(bcm_esw_port_linkscan_set(unit, port, spd_scan->linkscan_mode[port]));
    SS_LOCK(unit);
    BCM_PBMP_PORT_REMOVE(spd_scan->pbmp_enabled, port);
    BCM_PBMP_PORT_REMOVE(spd_scan->pbmp_active, port);
    SS_UNLOCK(unit);

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* CPRI Speed Scan Port configuration get. */
int bcm_esw_cpri_speed_scan_port_config_get(
    int unit,
    bcm_port_t port,
    bcm_cpri_port_speed_t *speed_list,
    int num_element,
    int *num_speed)
{
#ifdef CPRIMOD_SUPPORT
    int spd_index;
    spd_scan_control_t *spd_scan  = speed_scan_control[unit];

    if (spd_scan == NULL) {
        return BCM_E_INIT;
    }

    if (BCM_PBMP_MEMBER(spd_scan->pbmp_enabled, port)){
        if(num_element < spd_scan->port_info[port].num_valid_speed) {
            *num_speed = num_element;
        } else {
            *num_speed = spd_scan->port_info[port].num_valid_speed;
        }
        if (*num_speed > 0) {
            for( spd_index = 0; spd_index < *num_speed ; spd_index++) {
                speed_list[spd_index] = spd_scan->port_info[port].speed_list[spd_index];
            }
        }
    } else { /* Port is disabled for Speed Scan */
        *num_speed = 0;
    }
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Register handler function for CPRI speed scanning. */
int bcm_esw_cpri_speed_scan_callback_register(
    int unit,
    bcm_cpri_speed_scan_handler_t callback,
    void *user_data)
{
#ifdef CPRIMOD_SUPPORT
    spd_scan_control_t *spd_scan  = speed_scan_control[unit];
    speed_scan_handler_info_t *handler_info;
    int found = FALSE;


    if (spd_scan == NULL) {
        return BCM_E_INIT;
    }

    for (handler_info = spd_scan->handler_info; handler_info != NULL; handler_info = handler_info->next) {
        if (handler_info->handler == callback) {
            found = TRUE;
            break;
        }
    }

    if (found) {
        return BCM_E_NONE;
    }

    if ((handler_info = sal_alloc(sizeof(speed_scan_handler_info_t), "bcm_speed_scan_register")) == NULL) {
        return(BCM_E_MEMORY);
    }

    handler_info->next = spd_scan->handler_info;
    handler_info->handler = callback;
    handler_info->user_data = user_data;
    spd_scan->handler_info = handler_info;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* UnRegister handler function for CPRI speed scanning. */
int bcm_esw_cpri_speed_scan_callback_unregister(
    int unit,
    bcm_cpri_speed_scan_handler_t callback)
{
#ifdef CPRIMOD_SUPPORT
    spd_scan_control_t *spd_scan  = speed_scan_control[unit];
    speed_scan_handler_info_t *curr_handler_info;
    speed_scan_handler_info_t *prev_handler_info;

    if (spd_scan == NULL) {
        return BCM_E_INIT;
    }
    for (prev_handler_info = NULL, curr_handler_info = spd_scan->handler_info;
         curr_handler_info != NULL;
         prev_handler_info = curr_handler_info, curr_handler_info = curr_handler_info->next) {

        if (curr_handler_info->handler == callback) {
            if (prev_handler_info == NULL) {
                spd_scan->handler_info = curr_handler_info->next;
            } else {
                prev_handler_info->next = curr_handler_info->next;
            }
            break;
        }
    }
    if (curr_handler_info != NULL) {
        sal_free(curr_handler_info);
        return BCM_E_NONE;
    } else {
        return BCM_E_NOT_FOUND;
    }

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

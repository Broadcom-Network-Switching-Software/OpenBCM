/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        macmove_mon.c
 * Purpose:     This file contains implementation of MAC move dampening feature.
 *              It contains implementation of the 'consumer' portion of
 *              communication with the 'producer' thread. On Tomahawk3, the
 *              producer is the learn thread; it detects the station move and
 *              informs the consumer (consumer is the mac move monitoring thread
 *              implemented in this module). The consumer portion can be reused
 *              on other devices which support mod fifo; the producer portion
 *              should be implemented for mod fifo based devices. 
 */
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/l2x.h>
#include <soc/debug.h>
#include <soc/util.h>
#include <soc/esw/macmove_mon.h>
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>
#endif
#include <soc/scache.h>

#ifdef BCM_XGS_SWITCH_SUPPORT

#define SOC_MEM_COMPARE_RETURN(a, b) {		\
        if ((a) < (b)) { return -1; }		\
        if ((a) > (b)) { return  1; }		\
}

/* Default interval for Mac move monitoring thread (250ms) */
#define SOC_L2_MAC_MOVE_MON_THREAD_DEFAULT_INTERVAL 250000
#define SOC_L2_MAC_MOVE_MON_THREAD_DEFAULT_PRI      50

/* Time to sleep for before checking the queue for new entries */
#define SOC_L2_MAC_MOVE_MON_THREAD_SLEEP_INTERVAL (500 * MILLISECOND_USEC)

/* Invalid value for duration and threshold */
#define SOC_L2_MAC_MOVE_MON_PARAM_INVALID 0

/* Database implemented as AVL tree. AVL tree datum format */
typedef struct soc_l2_mac_move_mon_db_entry_s {
    soc_l2_mac_move_mon_entry_info_t entry_info; /* Info from learn thr */

    int moves_this_cycle; /* Count of the number of times station move
                             occurred in the current polling cycle */

    uint32 flags;       /* See values below */
/* Entry is valid */
#define _SOC_L2_MAC_MOVE_MON_DATUM_VALID 0x1
/* Registered callback function was called for this entry */
#define _SOC_L2_MAC_MOVE_MON_CB_ISSUED   0x2
} soc_l2_mac_move_mon_db_entry_t;

/* Structure used to pass additional info with the AVL tree traverse function */
typedef struct soc_l2_mac_move_mon_db_entry_modify_s {
    uint32 field_to_modify; /* See values below */
#define _SOC_L2_MAC_MOVE_MON_MODIFY_FLAGS     0x1
#define _SOC_L2_MAC_MOVE_MON_MODIFY_NUM_MOVES 0x2

    int      move_count; /* Used to modify 'moves_this_cycle' */
    uint32 flags;      /* Used to modify 'flags' */
} soc_l2_mac_move_mon_db_entry_modify_t;

/* Per device configuration parameters for Mac move monitoring feature */
typedef struct soc_l2_mac_move_mon_config_s {
    char thread_name[16];                   /* Name for the monitoring thread */

    volatile sal_thread_t mac_move_mon_tid; /* Thread id of Mac move monitoring
                                               thread */

    sal_mutex_t mac_move_mon_cfg_mutex;     /* Mutex to control access to this
                                              (monitoring params) information */

    volatile sal_usecs_t thread_interval;   /* Interval for periodic processing
                                               of mac move entries. Interval is
                                               in microsec */

    sal_mutex_t mac_move_mon_db_mutex;      /* Mutex to control access to Mac
                                               move mon db */

    shr_avl_t *mac_move_mon_db;             /* Station move database */

    soc_l2_mac_move_mon_cb int_cb_fn;       /* Internal cb function registered
                                               by the BCM layer */

    void *int_cb_fn_data;                   /* Data to be passed to the int cb
                                               function */ 

    volatile uint32 duration;               /* Duration in seconds to accumulate
                                               moves for (M in the requirements
                                               doc) */

    volatile uint32 threshold;              /* Number of moves allowed over the
                                               move duration (N in the
                                               requirements doc) */

    uint32 feature_state;                   /* Set by BCM layer to indicate all
                                               feature's operational state
                                               (initialized, started/stopped) */
} soc_l2_mac_move_mon_config_t;

static soc_l2_mac_move_mon_config_t *mac_move_mon_cfg[SOC_MAX_NUM_DEVICES];

/* Internal, shared queue to pass information from the producer thread to the
 * mac move monitoring thread
 */
soc_l2_mac_move_mon_queue_t *mac_move_mon_queue[SOC_MAX_NUM_DEVICES];

/* Used to signal monitoring thread start/stop condition to producer thread
 * Note this semaphore is not part of mac_move_mon_cfg[] because producer thread
 * will also use it. So when mac move monitoring feature is
 * disabled, or not enabled at all, in both cases, the producer thread needs to
 * check for existence of a valid semaphore before proceeding ahead
 * (mac_move_mon_cfg[] for the unit is deleted when this thread is stopped or
 *  it is not instantiated at all)
 */ 
static sal_sem_t mmm_thread_signal[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *	soc_l2_mac_move_mon_params_set
 * Purpose:
 *	This function sets values of duration and threshold passed by the
 *	caller/upper layer is done
 * Parameters:
 *	unit    - StrataSwitch unit number
 *	threshold - Threshold value (values 5-50)
 *	duration -  Duration value (values 1-60 seconds)
 * Returns:
 *	SOC_E_NONE - Success
 *	SOC_E_INIT - If config db is not yet created
 */
int
soc_l2_mac_move_mon_params_set(int unit, uint32 threshold, uint32 duration)
{
    if ((mac_move_mon_cfg[unit] == NULL) ||
        (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex == NULL)) {
        return SOC_E_INIT;
    }

    sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex,
                   sal_mutex_FOREVER);
    mac_move_mon_cfg[unit]->threshold = threshold;
    mac_move_mon_cfg[unit]->duration = duration;
    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2_mac_move_mon_params_get
 * Purpose:
 *	This function sets values of duration and threshold passed by the
 *	caller/upper layer is done
 * Parameters:
 *	unit      - StrataSwitch unit number
 *	threshold - (OUT)Threshold value returned, if feature is initialized
 *	duration  - (OUT)Duration value returned (in seconds), if feature is
 *	                 initialized
 * Returns:
 *	SOC_E_NONE - Success
 *	SOC_E_INIT - If config db is not yet created
 */
int
soc_l2_mac_move_mon_params_get(int unit, uint32 *threshold, uint32 *duration)
{
    if ((mac_move_mon_cfg[unit] == NULL) ||
        (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex == NULL)) {
        return SOC_E_INIT;
    }

    if ((threshold == NULL) || (duration == NULL)) {
        return SOC_E_PARAM;
    }

	sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex,
                   sal_mutex_FOREVER);
    *threshold = mac_move_mon_cfg[unit]->threshold;
    /* Convert microseconds to seconds */
    *duration = mac_move_mon_cfg[unit]->duration;
    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2_mac_move_mon_thread_intvl_set
 * Purpose:
 *	This function sets value of thread interval
 * Parameters:
 *	unit    - StrataSwitch unit number
 * Returns:
 *	SOC_E_NONE    - Success
 *	SOC_E_INIT - If config db is not yet created
 */
int
soc_l2_mac_move_mon_thread_intvl_set(int unit, sal_usecs_t interval)
{
    if ((mac_move_mon_cfg[unit] == NULL) ||
        (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex == NULL)) {
        return SOC_E_INIT;
    }

	sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex,
                   sal_mutex_FOREVER);
    mac_move_mon_cfg[unit]->thread_interval = interval;
    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2_mac_move_mon_thread_intvl_get
 * Purpose:
 *	This function returns value of configured thread interval
 * Parameters:
 *	unit     - (IN)StrataSwitch unit number
 *	interval - (OUT)Interval in microseconds
 * Returns:
 *	SOC_E_NONE    - Success
 *	SOC_E_INIT - If config db is not yet created
 */
int
soc_l2_mac_move_mon_thread_intvl_get(int unit, sal_usecs_t *interval)
{
    if ((mac_move_mon_cfg[unit] == NULL) ||
        (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex == NULL)) {
        return SOC_E_INIT;
    }

	sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex,
                   sal_mutex_FOREVER);
    *interval = mac_move_mon_cfg[unit]->thread_interval;
    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2_mac_move_mon_feature_init_set
 * Purpose:
 *	This function sets value of initialization flag to indicate feature
 *	initialization by the caller/upper layer is done
 * Parameters:
 *	unit    - StrataSwitch unit number
 * Returns:
 *	SOC_E_NONE    - Success
 *	SOC_E_INIT - If config db is not yet created
 */
int
soc_l2_mac_move_mon_feature_init_set(int unit)
{
    if ((mac_move_mon_cfg[unit] == NULL) ||
        (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex == NULL)) {
        return SOC_E_INIT;
    }

	sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex,
                   sal_mutex_FOREVER);
    mac_move_mon_cfg[unit]->feature_state |= SOC_L2_MAC_MOVE_MON_STATE_INITED;
    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);

    return SOC_E_NONE;
}
 
/*
 * Function:
 *	soc_l2_mac_move_mon_feature_init_clear
 * Purpose:
 *	This function resets value of initialization flag to indicate feature
 *	was stopped by the caller/upper layer and allocated resources were freed
 * Parameters:
 *	unit - StrataSwitch unit number
 * Returns:
 *	SOC_E_NONE - Success
 *	SOC_E_INIT - If config db is not yet created
 */
int
soc_l2_mac_move_mon_feature_init_clear(int unit)
{
    if ((mac_move_mon_cfg[unit] == NULL) ||
        (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex == NULL)) {
        return SOC_E_INIT;
    }

	sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex,
                   sal_mutex_FOREVER);
    mac_move_mon_cfg[unit]->feature_state &= ~SOC_L2_MAC_MOVE_MON_STATE_INITED;
    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2_mac_move_mon_feature_start_set
 * Purpose:
 *	This function sets value of start flag to indicate feature
 *	is fully operational (following initialization)
 * Parameters:
 *	unit    - StrataSwitch unit number
 * Returns:
 *	SOC_E_NONE    - Success
 *	SOC_E_INIT - If config db is not yet created
 */
int
soc_l2_mac_move_mon_feature_start_set(int unit)
{
    if ((mac_move_mon_cfg[unit] == NULL) ||
        (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex == NULL)) {
        return SOC_E_INIT;
    }

	sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex,
                   sal_mutex_FOREVER);
    mac_move_mon_cfg[unit]->feature_state |= SOC_L2_MAC_MOVE_MON_STATE_STARTED;
    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2_mac_move_mon_feature_start_clear
 * Purpose:
 *	This function resets value of the start flag to indicate the feature
 *	was stopped by the caller/upper layer
 * Parameters:
 *	unit - StrataSwitch unit number
 * Returns:
 *	SOC_E_NONE - Success
 *	SOC_E_INIT - If config db is not yet created
 */
int
soc_l2_mac_move_mon_feature_start_clear(int unit)
{
    if ((mac_move_mon_cfg[unit] == NULL) ||
        (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex == NULL)) {
        return SOC_E_INIT;
    }

	sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex,
                   sal_mutex_FOREVER);
    mac_move_mon_cfg[unit]->feature_state &= ~SOC_L2_MAC_MOVE_MON_STATE_STARTED;
    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2_mac_move_mon_feature_state_get
 * Purpose:
 *	This function returns operational state of the mac move monitoring feature
 * Parameters:
 *	unit    - (IN)StrataSwitch unit number
 *	state   - (OUT)Flags indicating if the feature has been initialized, started
 * Returns:
 *	SOC_E_NONE    - Success
 *	SOC_E_INIT    - If config db is not yet created
 */
int
soc_l2_mac_move_mon_feature_state_get(int unit, uint32 *state)
{
    if ((mac_move_mon_cfg[unit] == NULL) ||
        (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex == NULL)) {
        return SOC_E_INIT;
    }

	sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex,
                   sal_mutex_FOREVER);
    *state = mac_move_mon_cfg[unit]->feature_state;
    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2_mac_move_mon_cb_register
 * Purpose:
 *	This function registers a callback function to notify the upper layer
 *	the the threshold for MAC moves is exceeded for a given MAC
 * Parameters:
 *	unit    - StrataSwitch unit number
 *	fn      - Callback function to register
 *	fn_data - Extra data passed to callback function
 * Returns:
 *	SOC_E_NONE    - Success
 *	SOC_E_INTERNAL - If config db is not yet created
 *  SOC_E_UNAVAIL - If a device does not support this feature
 */
int
soc_l2_mac_move_mon_cb_register(int unit, soc_l2_mac_move_mon_cb fn,
                                void *fn_data)
{
    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_UNAVAIL;
    }

    if (mac_move_mon_cfg[unit] != NULL) {
        mac_move_mon_cfg[unit]->int_cb_fn = fn;

        mac_move_mon_cfg[unit]->int_cb_fn_data = fn_data;
    } else {
        LOG_INFO(BSL_LS_SOC_L2, 
             (BSL_META_U(unit, "Debug: Config db not yet created, exiting\n")));
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2_mac_move_mon_cb_unregister
 * Purpose:
 *	This function unregisters a previously registered callback function.
 *	Currently, we plan to support only one callback function, so the supplied
 *	arguments are not used
 * Parameters:
 *	unit    - StrataSwitch unit number
 *	fn      - Callback function to register - currently not used
 *	fn_data - Extra data passed to callback function - currently not used
 * Returns:
 *	SOC_E_NONE - Success
 *  SOC_E_UNAVAIL - If a device does not support this feature
 */
int
soc_l2_mac_move_mon_cb_unregister(int unit, soc_l2_mac_move_mon_cb fn,
                                  void *fn_data)
{
    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_UNAVAIL;
    }

    if (mac_move_mon_cfg[unit] != NULL) {
        mac_move_mon_cfg[unit]->int_cb_fn = NULL;

        mac_move_mon_cfg[unit]->int_cb_fn_data = NULL;
    } else {
        LOG_INFO(BSL_LS_SOC_L2, 
             (BSL_META_U(unit, "Debug: Config db does not exist, exiting\n")));
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_l2_mac_move_mon_signalling_sem_create
 * Purpose:
 *  This function create a binary semaphore which is used to communicate if a
 *  queue update is in progress
 * Parameters:
 *	unit - StrataSwitch unit number
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *  None 
 */
int soc_l2_mac_move_mon_signalling_sem_create(int unit)
{
    if ((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES)) {
        return SOC_E_UNIT;
    }

    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_UNAVAIL;
    }

    if (mmm_thread_signal[unit] != NULL) {
        sal_sem_destroy(mmm_thread_signal[unit]);
        mmm_thread_signal[unit] = NULL;
    }

    mmm_thread_signal[unit] = sal_sem_create("mmm_thr_sig", sal_sem_BINARY, 1);

    if (mmm_thread_signal[unit] == NULL) {
        return SOC_E_RESOURCE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_l2_mac_move_mon_signalling_sem_destroy
 * Purpose:
 *  This function removes the signalling semaphore. It will be called during
 *  soc shutdown/detach/init stages
 * Parameters:
 *	unit - StrataSwitch unit number
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *  None 
 */
int soc_l2_mac_move_mon_signalling_sem_destroy(int unit)
{
    if ((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES)) {
        return SOC_E_UNIT;
    }

    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_UNAVAIL;
    }

    if (mmm_thread_signal[unit] != NULL) {
        sal_sem_destroy(mmm_thread_signal[unit]);
        mmm_thread_signal[unit] = NULL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_l2_mac_move_mon_signalling_sem_take
 * Purpose:
 *  Wrapper function to acquire the signalling semaphore
 * Parameters:
 *	unit - StrataSwitch unit number
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *  None 
 */
int soc_l2_mac_move_mon_signalling_sem_take(int unit)
{
    if ((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES)) {
        return SOC_E_UNIT;
    }

    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_UNAVAIL;
    }

    if (mmm_thread_signal[unit] != NULL) {
        sal_sem_take(mmm_thread_signal[unit], sal_sem_FOREVER);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_l2_mac_move_mon_signalling_sem_give
 * Purpose:
 *  Wrapper function to release the signalling semaphore
 * Parameters:
 *	unit - StrataSwitch unit number
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *  None 
 */
int soc_l2_mac_move_mon_signalling_sem_give(int unit)
{
    if ((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES)) {
        return SOC_E_UNIT;
    }

    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_UNAVAIL;
    }

    if (mmm_thread_signal[unit] != NULL) {
        sal_sem_give(mmm_thread_signal[unit]);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * 	_soc_l2_mac_move_mon_queue_destroy
 * Purpose:
 *  This function is called to delete the queue used for mac move monitoring
 *  feature. All associated memory is freed. See notes
 * Parameters:
 *  unit - StrataSwitch unit number
 *  q    - Pointer to the queue information of the queue being deleted/removed
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *  Make sure producer thread does not access the queue while or after its
 *  destroyed. Ideally the producer thread should be stopped when this function is
 *  invoked
 */
STATIC void
_soc_l2_mac_move_mon_queue_destroy(int unit, soc_l2_mac_move_mon_queue_t **q)
{
    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return;
    }

    if ((q != NULL) && (*q != NULL)) {
        if ((*q)->list != NULL) {
            sal_free((*q)->list);
            (*q)->list = NULL;
        }

        sal_free(*q);
        *q = NULL;
    }
}

/*
 * Function:
 * 	_soc_l2_mac_move_mon_queue_clear
 * Purpose:
 *  This function clears all entries in the queue. This is required when the 
 *  feature is disabled, and also during initialization
 * Parameters:
 *  unit - StrataSwitch unit number
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *  None 
 */
STATIC void
_soc_l2_mac_move_mon_queue_clear(int unit)
{
    soc_l2_mac_move_mon_entry_info_t *e = NULL;

    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return;
    }

    /* If the queue is not initalized when this function is called, simply 
     * return
     */
    if (mac_move_mon_queue[unit] == NULL) {
        return;
    }

    e = mac_move_mon_queue[unit]->list;

    sal_memset((uint8 *)e, 0x0,
               MAC_MOVE_MON_QSIZE * sizeof(soc_l2_mac_move_mon_entry_info_t));

    /* Mark queue as empty */
    mac_move_mon_queue[unit]->in = 0;
    mac_move_mon_queue[unit]->out = 0;
}

/*
 * Function:
 * 	_soc_l2_mac_move_mon_queue_init
 * Purpose:
 *  This function allocates memory for the queue when mac move monitoring
 *  feature is enabled
 * Parameters:
 *	unit - StrataSwitch unit number
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *  None 
 */
STATIC int
_soc_l2_mac_move_mon_queue_init(int unit)
{
    int size;

    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_UNAVAIL;
    }

    if (mac_move_mon_queue[unit] != NULL) {
        _soc_l2_mac_move_mon_queue_destroy(unit, &mac_move_mon_queue[unit]);
        mac_move_mon_queue[unit] = NULL;
    }

    size = sizeof(soc_l2_mac_move_mon_queue_t);

    mac_move_mon_queue[unit] = (soc_l2_mac_move_mon_queue_t *)
                               sal_alloc(size, "MacMoveMonQStruct");

    if (mac_move_mon_queue[unit] == NULL) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit, "Error: alloc failed for queue memory,"
                  " size %d\n"), size));
        return SOC_E_MEMORY;
    }

    size = sizeof(soc_l2_mac_move_mon_entry_info_t) * MAC_MOVE_MON_QSIZE;

    mac_move_mon_queue[unit]->list = (soc_l2_mac_move_mon_entry_info_t *)
                                     sal_alloc(size, "MacMoveMonQEntries");

    if (mac_move_mon_queue[unit]->list == NULL) {
        sal_free(mac_move_mon_queue[unit]);
        mac_move_mon_queue[unit] = NULL;

        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit, "Error: alloc failed for queue entries,"
                  " size %d\n"), size));
        return SOC_E_MEMORY;
    }

    _soc_l2_mac_move_mon_queue_clear(unit);

    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_l2_mac_move_mon_queue_add
 * Purpose:
 *  This function enqueue an element at the end of the queue when mac move monitoring
 *  feature is enabled
 * Parameters:
 *	unit - StrataSwitch unit number
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *  None
 */
int
soc_l2_mac_move_mon_queue_add(int unit,
    soc_l2_mac_move_mon_entry_info_t *l2_mmm_entry)
{
    int in, out, next, full;
    soc_l2_mac_move_mon_entry_info_t *elem = NULL;

    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_UNAVAIL;
    }

    if (l2_mmm_entry == NULL) {
        return SOC_E_PARAM;
    }

    if (soc_l2_mac_move_mon_thread_running(unit)) {
        if ((mac_move_mon_queue[unit] != NULL) &&
            (mac_move_mon_queue[unit]->list != NULL)) {
            (void)soc_l2_mac_move_mon_signalling_sem_take(unit);
            /* If monitoring thread was turned off, do not enqueue */
            if (soc_l2_mac_move_mon_thread_running(unit)) {
                in = mac_move_mon_queue[unit]->in;
                out = mac_move_mon_queue[unit]->out;
                next = ((in + 1) % MAC_MOVE_MON_QSIZE);
                /* Point to the entry being written */
                elem = mac_move_mon_queue[unit]->list + in;

                full = (out == next);
                if (!full) {
                    sal_memcpy(elem->mac, l2_mmm_entry->mac,
                               sizeof(elem->mac));
                    elem->vlan              = l2_mmm_entry->vlan;
                    elem->dst_type_prev     = l2_mmm_entry->dst_type_prev;
                    elem->modid_prev        = l2_mmm_entry->modid_prev;
                    elem->portnum_or_trunk_id_prev =
                                    l2_mmm_entry->portnum_or_trunk_id_prev;
                    elem->dst_type_cur      = l2_mmm_entry->dst_type_cur;
                    elem->modid_cur         = l2_mmm_entry->modid_cur;
                    elem->portnum_or_trunk_id_cur =
                                    l2_mmm_entry->portnum_or_trunk_id_cur;
                    elem->flags = _SOC_L2_MAC_MOVE_MON_ENTRY_VALID;

                    /* Point to the next (free) location */
                    mac_move_mon_queue[unit]->in = next;
                }
            }
            (void)soc_l2_mac_move_mon_signalling_sem_give(unit);
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2_mac_move_mon_delete_all
 * Purpose:
 *	This function removes all entries from mac move monitoring database
 * Parameters:
 *	unit - StrataSwitch unit number
 * Returns:
 *  SOC_E_XXX
 * Notes:
 *	None
 */
int
soc_l2_mac_move_mon_delete_all(int unit)
{
    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_UNAVAIL;
    }

	if (mac_move_mon_cfg[unit] != NULL &&
        (mac_move_mon_cfg[unit]->mac_move_mon_db != NULL) &&
        (mac_move_mon_cfg[unit]->mac_move_mon_db_mutex != NULL)) {
	    sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex,
                       sal_mutex_FOREVER);
	    (void)shr_avl_delete_all(mac_move_mon_cfg[unit]->mac_move_mon_db);
	    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex);
	}

    /* Clear all queue entries */
    _soc_l2_mac_move_mon_queue_clear(unit);

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2_move_mac_mon_entry_from_l2x
 * Purpose:
 *	This function contructs an L2 MMM info entry from L2X entries.
 * Parameters:
 *	unit - StrataSwitch unit number
 *	old_l2x_entry - old L2X entry, contains information before station move
 *	l2x_entry - L2X entry, contains information after station move
 *	l2_mmm_entry - output L2 MAC move info entry
 * Returns:
 *  SOC_E_XXX
 * Notes:
 *	None
 */
int
soc_l2_move_mac_mon_entry_from_l2x(int unit,
                                   l2x_entry_t *old_l2x_entry,
                                   l2x_entry_t *l2x_entry,
                                soc_l2_mac_move_mon_entry_info_t *l2_mmm_entry)
{
    uint32 dest_value = 0;
    uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;

    if (old_l2x_entry == NULL || l2x_entry == NULL || l2_mmm_entry == NULL) {
        return SOC_E_PARAM;
    }

    sal_memset(l2_mmm_entry, 0, sizeof(*l2_mmm_entry));

    soc_L2Xm_mac_addr_get(unit, old_l2x_entry,
                         MAC_ADDRf, l2_mmm_entry->mac);
    l2_mmm_entry->vlan = soc_L2Xm_field32_get(unit,
                            old_l2x_entry, VLAN_IDf);
    l2_mmm_entry->flags = _SOC_L2_MAC_MOVE_MON_ENTRY_VALID;

    if (soc_feature(unit, soc_feature_generic_dest)) {
        dest_value = soc_mem_field32_dest_get(unit, L2Xm,
                         old_l2x_entry, DESTINATIONf, &dest_type);
        if (dest_type == SOC_MEM_FIF_DEST_DGPP) {
            /* MODID + PORT */
            l2_mmm_entry->dst_type_prev = 0;
            l2_mmm_entry->modid_prev =
                                (dest_value & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                                SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
            l2_mmm_entry->portnum_or_trunk_id_prev =
                                dest_value & SOC_MEM_FIF_DGPP_PORT_MASK;
        } else if (dest_type == SOC_MEM_FIF_DEST_LAG) {
            /* Trunk */
            l2_mmm_entry->dst_type_prev = 1;
            l2_mmm_entry->modid_prev = 0;
            l2_mmm_entry->portnum_or_trunk_id_prev =
                                dest_value & SOC_MEM_FIF_DGPP_TGID_MASK;
        } else {
            l2_mmm_entry->flags = 0;
        }

        dest_value = soc_mem_field32_dest_get(unit, L2Xm,
                         l2x_entry, DESTINATIONf, &dest_type);
        if (dest_type == SOC_MEM_FIF_DEST_DGPP) {
            /* MODID + PORT */
            l2_mmm_entry->dst_type_cur = 0;
            l2_mmm_entry->modid_cur =
                                (dest_value & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                                SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
            l2_mmm_entry->portnum_or_trunk_id_cur =
                                dest_value & SOC_MEM_FIF_DGPP_PORT_MASK;
        } else if (dest_type == SOC_MEM_FIF_DEST_LAG) {
            /* Trunk */
            l2_mmm_entry->dst_type_cur = 1;
            l2_mmm_entry->modid_cur = 0;
            l2_mmm_entry->portnum_or_trunk_id_cur =
                                dest_value & SOC_MEM_FIF_DGPP_TGID_MASK;
        } else {
            l2_mmm_entry->flags = 0;
        }
   } else {
        l2_mmm_entry->dst_type_prev = soc_L2Xm_field32_get(unit,
                                         old_l2x_entry, DEST_TYPEf);
        if (l2_mmm_entry->dst_type_prev == 0) {
            /* MODID + PORT */
            l2_mmm_entry->modid_prev = soc_L2Xm_field32_get(unit,
                                           old_l2x_entry, MODULE_IDf);
            l2_mmm_entry->portnum_or_trunk_id_prev = soc_L2Xm_field32_get(unit,
                                           old_l2x_entry, PORT_NUMf);
        } else if (l2_mmm_entry->dst_type_prev == 1) {
            /* Trunk */
            l2_mmm_entry->modid_prev = 0;
            l2_mmm_entry->portnum_or_trunk_id_prev = soc_L2Xm_field32_get(unit,
                                                        old_l2x_entry, TGIDf);
        } else {
            l2_mmm_entry->flags = 0;
        }

        l2_mmm_entry->dst_type_cur = soc_L2Xm_field32_get(unit,
                                        l2x_entry, DEST_TYPEf);
        if (l2_mmm_entry->dst_type_cur == 0) {
            /* MODID + PORT */
            l2_mmm_entry->modid_cur = soc_L2Xm_field32_get(unit,
                                         l2x_entry, MODULE_IDf);
            l2_mmm_entry->portnum_or_trunk_id_cur = soc_L2Xm_field32_get(unit,
                                                       l2x_entry, PORT_NUMf);
        } else if (l2_mmm_entry->dst_type_cur == 1) {
            /* Trunk */
            l2_mmm_entry->modid_cur = 0;
            l2_mmm_entry->portnum_or_trunk_id_cur = soc_L2Xm_field32_get(unit,
                                                       l2x_entry, TGIDf);
        } else {
            l2_mmm_entry->flags = 0;
        }
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *	soc_l2_mac_move_mon_resource_destroy
 * Purpose:
 *	This function is called during soc init, detach, shutdown stages, and when
 *	this thread is stopped
 * Parameters:
 *	unit - StrataSwitch unit number
 *	cleanup_all - If true, delete complete configuration. This is used during
 *	              detach/shutdown/init stages
 * Returns:
 *  SOC_E_XXX
 * Notes:
 *	None
 */
int
soc_l2_mac_move_mon_resource_destroy(int unit, int cleanup_all)
{
    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_UNAVAIL;
    }

    /* If the configuration is not initialized, or if it was destroyed earlier,
     * do nothing
     */
    if (mac_move_mon_cfg[unit] == NULL) {
        return SOC_E_NONE;
    }

    if (cleanup_all == TRUE) {
        if (mac_move_mon_cfg[unit]->mac_move_mon_db != NULL) {
            if (mac_move_mon_cfg[unit]->mac_move_mon_db_mutex != NULL) {
	            sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex,
                               sal_mutex_FOREVER);
                shr_avl_destroy(mac_move_mon_cfg[unit]->mac_move_mon_db);
                mac_move_mon_cfg[unit]->mac_move_mon_db = NULL;
                sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex);
            }
        }

        if (mac_move_mon_cfg[unit]->mac_move_mon_db_mutex != NULL) {
            sal_mutex_destroy(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex);
            mac_move_mon_cfg[unit]->mac_move_mon_db_mutex = NULL;
        }

        /* Destroy signalling semaphore */
        (void)soc_l2_mac_move_mon_signalling_sem_destroy(unit);

        /* Delete resources used for mac move monitoring configuration */
        if (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex != NULL) {
            sal_mutex_destroy(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);
            mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex = NULL;
        }

        if (mac_move_mon_cfg[unit] != NULL) {
            sal_free(mac_move_mon_cfg[unit]);
            mac_move_mon_cfg[unit] = NULL;
        }
    } else {
        if (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex != NULL) {
            sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex,
                           sal_mutex_FOREVER);
            mac_move_mon_cfg[unit]->mac_move_mon_tid = SAL_THREAD_ERROR;
            mac_move_mon_cfg[unit]->thread_interval = 0;
            mac_move_mon_cfg[unit]->duration =
                SOC_L2_MAC_MOVE_MON_STNMOVE_DURATION_DEFAULT;
            mac_move_mon_cfg[unit]->threshold =
                SOC_L2_MAC_MOVE_MON_STNMOVE_THRESHOLD_DEFAULT;
        	sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);
        }
    }

    /* Destroy queue. Destroyed after mac_move_mon_tid is marked invalid,
     * so producer thread can check the thread state first before accessing the
     * queue
     */
    if (mac_move_mon_queue[unit] != NULL) {
        if (cleanup_all == TRUE) {
            _soc_l2_mac_move_mon_queue_destroy(unit, &mac_move_mon_queue[unit]);
            mac_move_mon_queue[unit] = NULL;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * 	_soc_l2_mac_move_mon_compare_key
 * Purpose:
 *  Comparison function for AVL tree used for mac move monitoring operations
 * Parameters:
 *	user_data - User supplied data reuired by AVL library
 *	datum1    - First data item to compare
 *	datum2    - Second data item to compare
 * Returns:
 *	0 : If comparison items are equal
 *	1 : If comparison items are different
 * Notes:
 *	None
 */
STATIC int
_soc_l2_mac_move_mon_compare_key(void *user_data,
                                     shr_avl_datum_t *datum1,
                                     shr_avl_datum_t *datum2)
{
     soc_l2_mac_move_mon_db_entry_t *k1, *k2;

    /* COMPILER_REFERENCE(user_data);*/
    k1 = (soc_l2_mac_move_mon_db_entry_t *)datum1;
    k2 = (soc_l2_mac_move_mon_db_entry_t *)datum2;

    SOC_MEM_COMPARE_RETURN(k1->entry_info.vlan, k2->entry_info.vlan);

    return ENET_CMP_MACADDR(k1->entry_info.mac, k2->entry_info.mac);
}

/*
 * Function:
 * 	_soc_l2_mac_mon_param_clear
 * Purpose:
 *  Tree traverse cb function to clear either the move count accumulated at the
 *  end of 'duration' cycle provided by the application, or the entry flags, or
 *  both
 * Parameters:
 *  None
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *  Make sure mutex is acquired before the first invocation of this function
 */
STATIC int
_soc_l2_mac_mon_param_clear(void *user_data, shr_avl_datum_t *datum,
                                void *extra_data)
{
    /*int unit = PTR_TO_INT(user_data);*/
    soc_l2_mac_move_mon_db_entry_t *entry = NULL;
    soc_l2_mac_move_mon_db_entry_modify_t *m;

    if (extra_data != NULL) {
        entry = (soc_l2_mac_move_mon_db_entry_t *)datum;

        m = (soc_l2_mac_move_mon_db_entry_modify_t *)extra_data;

        if (m->field_to_modify & _SOC_L2_MAC_MOVE_MON_MODIFY_NUM_MOVES) {
            entry->moves_this_cycle = m->move_count;
        }

        if (m->field_to_modify & _SOC_L2_MAC_MOVE_MON_MODIFY_FLAGS) {
            entry->flags = m->flags;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * 	_soc_l2_mac_move_mon_info_clear
 * Purpose:
 *  This function clears information at the end of 'duration' cycle provided by
 *  the application. The information may be the number of moves, ot flags or
 *  both
 * Parameters:
 *  info - Pointer containing info about the data to be modified
 * Returns:
 *	SOC_E_XXX
 * Notes:
 */
STATIC void
_soc_l2_mac_move_mon_info_clear(int unit,
    soc_l2_mac_move_mon_db_entry_modify_t *info)
{
    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return;
    }

	sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex,
                   sal_mutex_FOREVER);

    if (shr_avl_count(mac_move_mon_cfg[unit]->mac_move_mon_db) > 0) {

        shr_avl_traverse(mac_move_mon_cfg[unit]->mac_move_mon_db,
                         _soc_l2_mac_mon_param_clear, info);
    }

    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex);
}

/*
 * Function:
 *	soc_l2_mac_move_mon_entry_delete
 * Purpose:
 *	This function removes an entry from mac move monitoring database, if mac
 *	dampening feature is enabled. It is called as part of L2 delete/replace
 *	APIs, and the aging thread when one/more entries are deleted. If L2 entry is
 *	not present, the function simply returns
 * Parameters:
 *	unit - StrataSwitch unit number
 *	mac  - Mac address. MAC-VLAN pair is used as key for locating corresponding
 *	       entry
 *	vlan - Vlan id
 * Returns:
 *  SOC_E_xxx
 * Notes:
 *	None
 */
int
soc_l2_mac_move_mon_entry_delete(int unit, sal_mac_addr_t mac, vlan_id_t vlan)
{
    int rv = SOC_E_NONE;
    soc_l2_mac_move_mon_db_entry_t entry;

    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_NONE;
    }

    /* Feature not initialized */
    if ((mac_move_mon_cfg[unit] == NULL) ||
        (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex == NULL)) {
        return SOC_E_NONE;
    }

	if ((mac_move_mon_cfg[unit]->mac_move_mon_db != NULL) &&
        (mac_move_mon_cfg[unit]->mac_move_mon_db_mutex != NULL)) {
	    sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex,
                       sal_mutex_FOREVER);
        if (shr_avl_count(mac_move_mon_cfg[unit]->mac_move_mon_db) > 0) {
            sal_memset(&entry, 0x0, sizeof(entry));
            sal_memcpy(entry.entry_info.mac, mac, sizeof(entry.entry_info.mac));
            entry.entry_info.vlan = vlan;
            /* Entry may or may not be found depending on whether it experienced
             * station move condition
             */
            rv = shr_avl_delete(mac_move_mon_cfg[unit]->mac_move_mon_db,
                     _soc_l2_mac_move_mon_compare_key,
                     (shr_avl_datum_t *)(&entry));
            if (rv == 0) {
                LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "DEBUG: soc_th3_l2x_appl_callback_entry_delete:"
                    "shr_avl_delete: Did not find datum\n")));
            }
        }
	    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex);
	}

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2_mac_move_mon_entry_lookup
 * Purpose:
 *	This function is used to lookup an item in the monitoring database. If an
 *	entry experienced station move condition, an entry will be present in the
 *	database
 * Parameters:
 *	unit - StrataSwitch unit number
 *	mac  -  Mac address. MAC-VLAN pair is used as key for locating corresponding
 *	        entry
 *	vlan -  Vlan id
 *	found - TRUE if entry was found,
 *          FALSE otherwise
 * Returns:
 *  SOC_E_xxx
 * Notes:
 *	None
 */
int
soc_l2_mac_move_mon_entry_lookup(int unit, sal_mac_addr_t mac, vlan_id_t vlan,
                                 int *found)
{
    soc_l2_mac_move_mon_db_entry_t entry;
    int present = 0;

    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_NONE;
    }

    /* Feature not initialized */
    if ((mac_move_mon_cfg[unit] == NULL) ||
        (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex == NULL)) {
        *found = FALSE;
        return SOC_E_NONE;
    }

    sal_memset(&entry, 0x0, sizeof(entry));
    sal_memcpy(entry.entry_info.mac, mac, sizeof(entry.entry_info.mac));
    entry.entry_info.vlan = vlan;

    *found = FALSE;
	if ((mac_move_mon_cfg[unit]->mac_move_mon_db != NULL) &&
        (mac_move_mon_cfg[unit]->mac_move_mon_db_mutex != NULL)) {
	    sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex,
                       sal_mutex_FOREVER);
        present = shr_avl_lookup(mac_move_mon_cfg[unit]->mac_move_mon_db,
                      _soc_l2_mac_move_mon_compare_key,
                      (shr_avl_datum_t *)(&entry));
	    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex);

        /* Entry found */
        *found = (present == 1) ? TRUE : FALSE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * 	_soc_l2_mac_move_mon_process
 * Purpose:
 *  Entry point for monitoring mac moves and issuing callbacks to the
 *  application
 * Parameters:
 *  u - Pointer containing unit number
 * Returns:
 *	SOC_E_XXX
 * Notes:
 */
STATIC void
_soc_l2_mac_move_mon_process(void *u)
{
    sal_usecs_t interval;
    uint32 poll_count, poll_num;
    uint32 duration, duration_usec, duration_prev;
    uint32 threshold, threshold_prev;
    int unit = PTR_TO_INT(u);
    soc_l2_mac_move_mon_db_entry_t datum;
    soc_l2_mac_move_mon_entry_info_t *move_info_ptr;
    soc_l2_mac_move_mon_db_entry_modify_t info;
    int result, issue_cb = FALSE;
    int max_db_entries;
    int in, out;
    int next, empty;
    int datum_changed = 0;

    poll_num = 1;
    duration_prev = SOC_L2_MAC_MOVE_MON_PARAM_INVALID;
    threshold_prev = SOC_L2_MAC_MOVE_MON_PARAM_INVALID;
    /* Max number of macs in the db can be up to the size of L2 table */
    max_db_entries = soc_mem_index_count(unit, L2Xm);

    while ((interval = mac_move_mon_cfg[unit]->thread_interval) != 0) {

        /* If params changed, mon thread will first process the current entries
         * because they (roughly) belong to the time 'before' the application
         * changed the mon params. Callbacks may be issued during this
         * processing. Then the mon thread will reset polling and monitoring
         * variables to get in line with the newly configured params
         */

        if (SOC_WARM_BOOT(unit)) {
            /* Do not process entries during Warmboot stage */
            sal_usleep(interval);
            continue;
        }

        /* Process entries from producer thread, if any. Then move on to check
         * end of current interval
         */
        in = mac_move_mon_queue[unit]->in;
        out = mac_move_mon_queue[unit]->out;

        empty = (in == out);

        while (!empty) {
            int node_count;

            node_count = 0;

            /* If warmboot is in progress, do not process any entries */
            if (SOC_WARM_BOOT(unit)) {
                break;
            }

            next = (out + 1) % MAC_MOVE_MON_QSIZE;

            move_info_ptr = mac_move_mon_queue[unit]->list + out;

            /* Check if this entry is already in the db. If not, insert it.
             * If its in the db, check for threshold exceeded condition,
             * issue cb (if not issued). 
             * Update 'moves_this_cycle' if callback was not issued yet.
             * If callback was already issued, we will only check whether the
             * current monitoring duration is over, for this mac
             */
            sal_memcpy(&datum.entry_info, move_info_ptr,
                       sizeof(*move_info_ptr));
            datum.flags = datum.moves_this_cycle = 0;

            /* mac, vlan info from the entry will be used for searching the db
             * entry. Note, if entry is found, 'datum' contains
             * the db entry's contents
             */
	        sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex,
                           sal_mutex_FOREVER);
            result = shr_avl_lookup(mac_move_mon_cfg[unit]->mac_move_mon_db,
                                    _soc_l2_mac_move_mon_compare_key,
                                    (shr_avl_datum_t *)(&datum));

            /* Entry found. We deal with 2 cases: entries for which threshold
             * is not exceeded, and entries for which threshold has been
             * exceeded
             */
            datum_changed = 0;
            if (result) {
                issue_cb = FALSE;
                if (datum.flags & _SOC_L2_MAC_MOVE_MON_DATUM_VALID) {
                    if (!(datum.flags & _SOC_L2_MAC_MOVE_MON_CB_ISSUED)) {
                        datum.moves_this_cycle += 1;
                        if (datum.moves_this_cycle >=
                            mac_move_mon_cfg[unit]->threshold) {
                            issue_cb = TRUE;
                            /* Set cb issued flag. Note we are doing this before
                             * issuing actual cb, so that we release mutex
                             * before the cb is invoked
                             */
                            datum.flags |= _SOC_L2_MAC_MOVE_MON_CB_ISSUED;
                        }
                        datum_changed = 1;
                    }
                }
            } else {

                if ((soc_mem_is_valid(unit, L2Xm)) &&
                    (SOC_MEM_FIELD_VALID(unit, L2Xm, MAC_ADDRf)) &&
                    ((SOC_MEM_FIELD_VALID(unit, L2Xm, VLAN_IDf)))) {
                    l2x_entry_t l2x_entry, l2x_lookup;
                    int ent_idx;
                    int retval = 0;

                    sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
                    soc_L2Xm_field32_set(unit, &l2x_entry, VLAN_IDf,
                        move_info_ptr->vlan);

                    soc_L2Xm_mac_addr_set(unit, &l2x_entry, MAC_ADDRf,
                        move_info_ptr->mac);

                    soc_mem_lock(unit, L2Xm);
                    retval = soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, &ent_idx,
                                 (void *)&l2x_entry, (void *)&l2x_lookup, 0);
                    soc_mem_unlock(unit, L2Xm);

                     /* This is either a very first stn move (for this entry),
                      * or the entry was deleted.
                      */
                     if (retval == SOC_E_NOT_FOUND) {
                         sal_mutex_give(
                             mac_move_mon_cfg[unit]->mac_move_mon_db_mutex);
                         /* If L2 entry was deleted, we skip this
                          * queue entry (this queue entry will be subsequently
                          * reused by the producer thread)
                          */
                         goto skip_processing;
                     } else {
                         if (retval != SOC_E_NONE) {
                             sal_mutex_give(
                                 mac_move_mon_cfg[unit]->mac_move_mon_db_mutex);

                             LOG_INFO(BSL_LS_SOC_L2, (BSL_META_U(unit,
                                 "soc_mem_search returned error %d\n"),
                                 retval));

                             /* We assume the error may be transitory, so we
                              * continue without stopping the thread; it is more
                              * likely that the subsequent mac-move search will
                              * be error free, unless the system has a severe
                              * problem
                              */
                             goto skip_processing;
                         }
                     }
                }

                /* Entry not found in internal db */
                issue_cb = FALSE;
                datum.moves_this_cycle = 1;
                datum.flags = _SOC_L2_MAC_MOVE_MON_DATUM_VALID;
                datum.flags &= ~_SOC_L2_MAC_MOVE_MON_CB_ISSUED;
                datum_changed = 1;
                /* Note, per requirement, the minimum mac moves is 5. So we
                 * do not check for threshold exceeded condition here
                 */
            }

            /* Check if the db is full */
            node_count = shr_avl_count(mac_move_mon_cfg[unit]->mac_move_mon_db);

            if ((datum_changed) && (node_count < max_db_entries)) {
                /* Insert/update the current node */
                shr_avl_insert(mac_move_mon_cfg[unit]->mac_move_mon_db,
                    _soc_l2_mac_move_mon_compare_key,
                    (shr_avl_datum_t *)(&datum));
            } else {
                if (datum_changed) {
                    LOG_INFO(BSL_LS_SOC_L2, (BSL_META_U(unit,
                             "DEBUG: Tree full. Entry not added in db\n")));
                }
            }

            sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex);

            if (issue_cb == TRUE) {
                /* Call registered bcm layer cb function */
                if ((mac_move_mon_cfg[unit]) &&
                    (mac_move_mon_cfg[unit]->int_cb_fn)) {

                    mac_move_mon_cfg[unit]->int_cb_fn(unit, move_info_ptr,
                        mac_move_mon_cfg[unit]->int_cb_fn_data);
                }
            }

skip_processing:
            /* Mark queue entry as not valid (means processed) */
            move_info_ptr->flags &= ~_SOC_L2_MAC_MOVE_MON_ENTRY_VALID;

            /* Update 'out' ptr to notifiy producer thread that the entry has
             * been freed up
             */
            mac_move_mon_queue[unit]->out = next;
            out = mac_move_mon_queue[unit]->out;
            empty = (in == out);
        }

        /* Read current param values */
        sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex,
                       sal_mutex_FOREVER);
        duration = mac_move_mon_cfg[unit]->duration;
        threshold = mac_move_mon_cfg[unit]->threshold;
    	sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);

        if ((duration_prev != duration) || (threshold_prev != threshold)) {

            /* Reset db fields for cycles other than the very first cycyle */
            if ((duration_prev != SOC_L2_MAC_MOVE_MON_PARAM_INVALID) &&
                (threshold_prev != SOC_L2_MAC_MOVE_MON_PARAM_INVALID)) {
                /* Reset 'moves_this_cycle', 'flags' for each entry in the db */
                sal_memset(&info, 0x0, sizeof(info));
                info.field_to_modify = _SOC_L2_MAC_MOVE_MON_MODIFY_NUM_MOVES;
                info.field_to_modify |= _SOC_L2_MAC_MOVE_MON_MODIFY_FLAGS;
                info.move_count = 0;
                /* Note cb issued bit is set to '0' in 'flags' below */
                info.flags = _SOC_L2_MAC_MOVE_MON_DATUM_VALID;
                _soc_l2_mac_move_mon_info_clear(unit, &info);
             }

            /* Restart polling */
            poll_num = 1;

            /* Save the latest values of monitoring params */
            duration_prev = duration;
            threshold_prev = threshold;
        }

        /* Convert seconds to microsec. Note max value of duration is 60sec,
         * as per the requirement
         */
        duration_usec = duration * SECOND_USEC;

        poll_count = duration_usec / interval;

        

        if (poll_num > poll_count) {

            /* Reset polling cycle number */
            poll_num = 1;

            /* Set 'moves_this_cycle' for each entry in the db to 0 */
            sal_memset(&info, 0x0, sizeof(info));
            info.field_to_modify = _SOC_L2_MAC_MOVE_MON_MODIFY_NUM_MOVES;
            info.field_to_modify |= _SOC_L2_MAC_MOVE_MON_MODIFY_FLAGS;
            info.move_count = 0;
            /* Note cb issued bit is set to '0' in 'flags' below */
            info.flags = _SOC_L2_MAC_MOVE_MON_DATUM_VALID;
            _soc_l2_mac_move_mon_info_clear(unit, &info);
        } else {
            poll_num++;
        }

        sal_usleep(interval);
    }

    LOG_INFO(BSL_LS_SOC_L2, 
             (BSL_META_U(unit, "Mac move monitoring thread exiting\n")));

    /* Wait for producer thread to signal that this thread can proceed */
    (void)soc_l2_mac_move_mon_signalling_sem_take(unit);
    (void)soc_l2_mac_move_mon_delete_all(unit);
    (void)soc_l2_mac_move_mon_resource_destroy(unit, FALSE);
    (void)soc_l2_mac_move_mon_signalling_sem_give(unit);

    /* thread id set to SAL_THREAD_ERROR in resource destroy function above */
    sal_thread_exit(0);
}

int
soc_l2_mac_move_mon_resource_init(int unit)
{
    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_UNAVAIL;
    }

    if ((mac_move_mon_cfg[unit] != NULL) &&
        (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex != NULL)) {
        sal_mutex_destroy(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);
        mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex = NULL;
    }

    if (mac_move_mon_cfg[unit] != NULL) {
        sal_free(mac_move_mon_cfg[unit]);
        mac_move_mon_cfg[unit] = NULL;
    }

    mac_move_mon_cfg[unit] = sal_alloc(sizeof(soc_l2_mac_move_mon_config_t),
                                       "mac_move_mon_cfg");

    if (mac_move_mon_cfg[unit] == NULL) {
        return(SOC_E_MEMORY);
    }

    sal_memset(mac_move_mon_cfg[unit], 0,
               sizeof(soc_l2_mac_move_mon_config_t));

    if ((mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex =
        sal_mutex_create("MacMoveMonCfgMutex")) == NULL) {

        return SOC_E_RESOURCE;
    }

    mac_move_mon_cfg[unit]->mac_move_mon_tid = SAL_THREAD_ERROR;
    mac_move_mon_cfg[unit]->mac_move_mon_db = NULL;
    mac_move_mon_cfg[unit]->mac_move_mon_db_mutex = NULL;

    return SOC_E_NONE;
}

int
soc_l2_mac_move_mon_resource_alloc(int unit)
{
    int rv = SOC_E_NONE;

    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_UNAVAIL;
    }

    if (mac_move_mon_cfg[unit] == NULL) {

        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                  "mac_move_mon_cfg not created for this unit\n")));

        return(SOC_E_INTERNAL);
    }

    if (mac_move_mon_cfg[unit]->mac_move_mon_db_mutex != NULL) {
        sal_mutex_destroy(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex);
        mac_move_mon_cfg[unit]->mac_move_mon_db_mutex = NULL;
    }

    if (mac_move_mon_cfg[unit]->mac_move_mon_db != NULL) {
         /* Delete all nodes */
        (void)shr_avl_delete_all(mac_move_mon_cfg[unit]->mac_move_mon_db);
         /* Delete tree's config data */
        (void)shr_avl_destroy(mac_move_mon_cfg[unit]->mac_move_mon_db);
        mac_move_mon_cfg[unit]->mac_move_mon_db = NULL;
    }

    /* Delete queue */
    if (mac_move_mon_queue[unit] != NULL) {
        _soc_l2_mac_move_mon_queue_destroy(unit, &mac_move_mon_queue[unit]);
        mac_move_mon_queue[unit] = NULL;
    }

    rv = soc_l2_mac_move_mon_signalling_sem_destroy(unit);
    if (rv != SOC_E_NONE) {
        LOG_INFO(BSL_LS_SOC_L2,
                  (BSL_META_U(unit, "%d: Error calling"
                  " soc_l2_mac_move_mon_signalling_sem_destroy: %s\n"),
                  unit, soc_errmsg(rv)));
        return rv;
    }

    /* Create queue */
    rv = _soc_l2_mac_move_mon_queue_init(unit);

    if (rv != SOC_E_NONE) {
        return rv;
    }

    rv = soc_l2_mac_move_mon_signalling_sem_create(unit);

    if (rv != SOC_E_NONE) {
        LOG_INFO(BSL_LS_SOC_L2,
                 (BSL_META_U(unit, "%d: Error calling"
                  " soc_l2_mac_move_mon_signalling_sem_create: %s\n"),
                  unit, soc_errmsg(rv)));
        goto cleanup_exit3;
    }

    if ((mac_move_mon_cfg[unit]->mac_move_mon_db_mutex =
        sal_mutex_create("MacMoveMonMutex")) == NULL) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit, "%d: Error calling sal_mutex_create for"
                  "Mac move monitoring mutex\n"), unit));

        rv = SOC_E_RESOURCE;
        goto cleanup_exit2;
    }

    if (shr_avl_create(&mac_move_mon_cfg[unit]->mac_move_mon_db,
                       INT_TO_PTR(unit),
                       sizeof(soc_l2_mac_move_mon_db_entry_t),
                       soc_mem_index_count(unit, L2Xm)) < 0) {

        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                  "%d: shr_avl_create(mac move mon db) error\n"),
                  unit));

        rv = SOC_E_RESOURCE;
        goto cleanup_exit1;
    }

    if (rv == SOC_E_NONE) {
        return rv;
    }

cleanup_exit1:
    if (mac_move_mon_cfg[unit]->mac_move_mon_db_mutex != NULL) {
        sal_mutex_destroy(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex);
        mac_move_mon_cfg[unit]->mac_move_mon_db_mutex = NULL;
    }
cleanup_exit2:
    (void)soc_l2_mac_move_mon_signalling_sem_destroy(unit);
cleanup_exit3:
    /* Destroy queue in case of error */
    _soc_l2_mac_move_mon_queue_destroy(unit, &mac_move_mon_queue[unit]);

    return rv;
}

/*
 * Function:
 *	soc_l2_mac_move_mon_disable
 * Purpose:
 *	This function clears whole internal database which maintains mac entries for
 *  monitoring
 * Parameters:
 *	unit - Switch unit number
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	None
 */
int
soc_l2_mac_move_mon_disable(int unit)
{
    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_UNAVAIL;
    }

    /* Clear all entries recorded so far. When the feature is enabled again,
     * the db will be re-used
     */
	if ((mac_move_mon_cfg[unit]->mac_move_mon_db != NULL) &&
        (mac_move_mon_cfg[unit]->mac_move_mon_db_mutex != NULL)) {
	    sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex,
                       sal_mutex_FOREVER);
	    (void)shr_avl_delete_all(mac_move_mon_cfg[unit]->mac_move_mon_db);
	    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_db_mutex);
	}

    if (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex != NULL) {
        sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex,
                       sal_mutex_FOREVER);
        mac_move_mon_cfg[unit]->duration =
            SOC_L2_MAC_MOVE_MON_STNMOVE_DURATION_DEFAULT;
        mac_move_mon_cfg[unit]->threshold =
            SOC_L2_MAC_MOVE_MON_STNMOVE_THRESHOLD_DEFAULT;
    	sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);
    }

    /* Clear all queue entries */
    _soc_l2_mac_move_mon_queue_clear(unit);

    return SOC_E_NONE;
}

/*
 * Function:
 *   soc_l2_mac_move_mon_thread_running
 * Purpose:
 *   This function returns execution state of the mac move monitoring thread
 * Parameters:
 *   unit - unit number
 * Returns:
 *	  state - TRUE if thread is running. FALSE if thread is not running
 * Notes:
 */
int
soc_l2_mac_move_mon_thread_running(int unit)
{
    int state;

    if (mac_move_mon_cfg[unit] == NULL) {
        /* If the configuration is not set up, the thread has not yet started */
        state = FALSE;
    } else {
        state = (mac_move_mon_cfg[unit]->mac_move_mon_tid == SAL_THREAD_ERROR) ?
                    FALSE : TRUE;
    }

    return state;
}

/*
 * Function:
 *   soc_l2_mac_move_mon_thread_start
 * Purpose:
 *   Start L2 station move monotoring thread to be used for MAC dampening
 *   feature. See Notes
 * Parameters:
 *   unit - unit number
 * Returns:
 *	 SOC_E_XXX
 * Notes:
 *   soc_l2_mac_move_mon_resource_init and
 *   soc_l2_mac_move_mon_resource_alloc must be called before invoking this
 *   function
 *   MAC dampening feature must be enabled for this thread to be used
 *   User must configure valid global station move related parameters for
 *   station move monitoring mechanism to work as expected by the application
 */
int
soc_l2_mac_move_mon_thread_start(int unit)
{
    int pri = 0;
    sal_usecs_t interval;

    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_UNAVAIL;
    }

    /* Stop the monitoring thread if it was running */
    if (soc_l2_mac_move_mon_thread_running(unit)) {
        soc_l2_mac_move_mon_thread_stop(unit);
    }

    sal_snprintf(mac_move_mon_cfg[unit]->thread_name,
                 sizeof(mac_move_mon_cfg[unit]->thread_name),
                 "MacMoveMon.%d", unit);

    if ((mac_move_mon_cfg[unit] != NULL) &&
        (mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex != NULL)) {
        sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex,
                       sal_mutex_FOREVER);

        /* If interval is not configured, use default value */
        (void)soc_l2_mac_move_mon_thread_intvl_get(unit, &interval);
        if (interval == 0) {
            int rv1;

            rv1 = soc_l2_mac_move_mon_thread_intvl_set(unit,
                      SOC_L2_MAC_MOVE_MON_THREAD_DEFAULT_INTERVAL);

            if (rv1 == SOC_E_NONE) {
                 LOG_INFO(BSL_LS_SOC_L2, (BSL_META_U(unit,
                     "soc_l2_mac_move_mon_thread_start:unit %d, using default"
                     " thread interval %u Usec\n"), unit,
                     SOC_L2_MAC_MOVE_MON_THREAD_DEFAULT_INTERVAL));
            } else {
                 LOG_WARN(BSL_LS_SOC_L2, (BSL_META_U(unit,
                     "soc_l2_mac_move_mon_thread_start: unit %d, default thread"
                     " interval could not be set\n"), unit));
                return SOC_E_INTERNAL;
            }
        }

        if (mac_move_mon_cfg[unit]->mac_move_mon_tid == SAL_THREAD_ERROR) {

            /* NOTE: We are using the same property as producer thread */
            pri = SOC_L2_MAC_MOVE_MON_THREAD_DEFAULT_PRI;

            mac_move_mon_cfg[unit]->mac_move_mon_tid =
                sal_thread_create(mac_move_mon_cfg[unit]->thread_name,
                                  SAL_THREAD_STKSZ, pri,
                                  _soc_l2_mac_move_mon_process,
                                  INT_TO_PTR(unit));
        }

        if (mac_move_mon_cfg[unit]->mac_move_mon_tid == SAL_THREAD_ERROR) {
    	    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                       "Mac Move Monitoring: Could not start thread\n")));

            return SOC_E_MEMORY;
        }

    	sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *   soc_l2_mac_move_mon_thread_stop
 * Purpose:
 *   This function stops the mac move monitoring thread, if it was running
 * Parameters:
 *   unit - unit number
 * Returns:
 *	  SOC_E_XXX
 * Notes:
 */
int
soc_l2_mac_move_mon_thread_stop(int unit)
{
    int rv = SOC_E_NONE;
    soc_timeout_t to;

    if (!soc_feature(unit, soc_feature_l2_mac_move_monitoring)) {
        return SOC_E_UNAVAIL;
    }

    /* If the thread is not running, simply return */
    if (!(soc_l2_mac_move_mon_thread_running(unit))) {
        return rv;
    }

    LOG_INFO(BSL_LS_SOC_L2,
             (BSL_META_U(unit,
              "soc_l2_mac_move_mon_thread_stop:unit %d\n"), unit));

    sal_mutex_take(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex,
                   sal_mutex_FOREVER);

    mac_move_mon_cfg[unit]->thread_interval = 0; /* Set up exit condition */

    sal_mutex_give(mac_move_mon_cfg[unit]->mac_move_mon_cfg_mutex);

    if (mac_move_mon_cfg[unit]->mac_move_mon_tid != SAL_THREAD_ERROR) {
        /* Give thread some time to wake up and exit */
        if (SAL_BOOT_SIMULATION) {
            soc_timeout_init(&to, 30 * 1000000, 0);
        } else {
            soc_timeout_init(&to, 20 * 1000000, 0);
        }

        while (mac_move_mon_cfg[unit]->mac_move_mon_tid != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_L2,
                          (BSL_META_U(unit,
                          "soc_l2_mac_move_mon_thread_stop:"
                          " thread did not stop\n")));
                rv = SOC_E_INTERNAL;
                break;
            }
        }
    }

    return rv;
}


#endif /* BCM_XGS_SWITCH_SUPPORT */

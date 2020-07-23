/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#include <soc/drv.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm_int/esw/preemption_cnt.h>
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <bcm_int/esw/greyhound2.h>
#endif

#if defined(BCM_PREEMPTION_SUPPORT)
/* preemption counter control data for each device */
typedef struct bcmi_preemption_counter_control_s {
    sal_mutex_t counter_mutex;
    bcmi_preemption_counte_func_t dev_func;
} bcmi_preemption_counter_control_t;
STATIC bcmi_preemption_counter_control_t *preemption_counter_control
                                          [SOC_MAX_NUM_DEVICES];

/* lock/uplock helper function */
#define PREEMPTION_COUNTER_LOCK(_pc_) \
        sal_mutex_take((_pc_)->counter_mutex, sal_mutex_FOREVER);
#define PREEMPTION_COUNTER_UNLOCK(_pc_) \
        sal_mutex_give((_pc_)->counter_mutex);

/*
 * Function:
 *      bcmi_esw_preemption_counter_multi_get32
 * Purpose:
 *      Get 32-bit counter value for multiple port statistic types.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      sync_mode  - (IN) hwcount is to be synced to sw count
 *      local_port - (IN) GPORT ID
 *      nstat      - (IN) Number of elements in stat array
 *      stat_arr   - (IN) New statistics descriptors array
 *      value_arr  - (OUT)New counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_counter_multi_get32(int unit, int sync_mode,
                                        bcm_port_t local_port, int nstat,
                                        bcm_port_stat_t *stat_arr,
                                        uint32 *value_arr)
{
    int i;

    if (NULL == stat_arr || NULL == value_arr) {
        return BCM_E_PARAM;
    }
    for (i = 0; i < nstat; i++) {
        BCM_IF_ERROR_RETURN(bcmi_esw_preemption_counter_get32(unit, sync_mode,
                                                              local_port,
                                                              stat_arr[i],
                                                              &(value_arr[i])));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_counter_multi_set32
 * Purpose:
 *      Set 32-bit counter value for multiple port statistic types.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      local_port - (IN) GPORT ID
 *      nstat      - (IN) Number of elements in stat array
 *      stat_arr   - (IN) New statistics descriptors array
 *      value_arr  - (IN) New counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_counter_multi_set32(int unit,
                                        bcm_port_t local_port, int nstat,
                                        bcm_port_stat_t *stat_arr,
                                        uint32 *value_arr)
{
    int i;

    if (NULL == stat_arr || NULL == value_arr) {
        return BCM_E_PARAM;
    }
    for (i = 0; i < nstat; i++) {
        BCM_IF_ERROR_RETURN(bcmi_esw_preemption_counter_set32(unit,
                                                              local_port,
                                                              stat_arr[i],
                                                              value_arr[i]));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_counter_multi_get
 * Purpose:
 *      Get 64-bit counter value for multiple port statistic types.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      sync_mode   - (IN) hwcount is to be synced to sw count
 *      local_port - (IN) GPORT ID
 *      nstat      - (IN) Number of elements in stat array
 *      stat_arr   - (IN) New statistics descriptors array
 *      value_arr  - (OUT)New counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_counter_multi_get(int unit, int sync_mode,
                                      bcm_port_t local_port, int nstat,
                                      bcm_port_stat_t *stat_arr,
                                      uint64 *value_arr)
{
    int i;

    if (NULL == stat_arr || NULL == value_arr) {
        return BCM_E_PARAM;
    }
    for (i = 0; i < nstat; i++) {
        BCM_IF_ERROR_RETURN(bcmi_esw_preemption_counter_get(unit, sync_mode,
                                                            local_port,
                                                            stat_arr[i],
                                                            &(value_arr[i])));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_counter_multi_set
 * Purpose:
 *      Set 64-bit counter value for multiple port statistic types.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      local_port - (IN) GPORT ID
 *      nstat      - (IN) Number of elements in stat array
 *      stat_arr   - (IN) New statistics descriptors array
 *      value_arr  - (IN) New counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_counter_multi_set(int unit,
                                      bcm_port_t local_port, int nstat,
                                      bcm_port_stat_t *stat_arr,
                                      uint64 *value_arr)
{
    int i;

    if (NULL == stat_arr || NULL == value_arr) {
        return BCM_E_PARAM;
    }
    for (i = 0; i < nstat; i++) {
        BCM_IF_ERROR_RETURN(bcmi_esw_preemption_counter_set(unit,
                                                            local_port,
                                                            stat_arr[i],
                                                            value_arr[i]));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_counter_get32
 * Purpose:
 *      Get lower 32-bit counter value for specified port statistic
 *      type.
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      sync_mode   - (IN) hwcount is to be synced to sw count
 *      local_port  - (IN) GPORT ID
 *      stat        - (IN) Type of the counter to retrieve.
 *      val         - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_counter_get32(int unit, int sync_mode, bcm_port_t local_port,
                                  bcm_port_stat_t stat, uint32 *val)
{
    uint64 val64;

    if (NULL == val) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(bcmi_esw_preemption_counter_get(unit, sync_mode,
                                                        local_port,
                                                        stat, &val64));
    *val = COMPILER_64_LO(val64);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_counter_set32
 * Purpose:
 *      Set lower 32-bit counter value for specified port statistic
 *      type.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      local_port  - (IN) GPORT ID
 *      stat        - (IN) Type of the counter to retrieve.
 *      val         - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_counter_set32(int unit, bcm_port_t local_port,
                                  bcm_port_stat_t stat, uint32 val)
{
    uint64 val64;

    COMPILER_64_SET(val64, 0, val);
    BCM_IF_ERROR_RETURN(bcmi_esw_preemption_counter_set(unit, local_port,
                                                        stat, val64));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_counter_get
 * Purpose:
 *      Get 64-bit counter value for specified port statistic type.
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      sync_mode   - (IN) hwcount is to be synced to sw count
 *      local_port  - (IN) GPORT ID
 *      stat        - (IN) Type of the counter to retrieve.
 *      val         - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_counter_get(int unit, int sync_mode, bcm_port_t local_port,
                                bcm_port_stat_t stat, uint64 *val)
{
    bcmi_preemption_counter_control_t *pc = preemption_counter_control[unit];
    int rv;

    if (NULL == pc) {
        return BCM_E_INIT;
    }

    PREEMPTION_COUNTER_LOCK(pc);
    rv = pc->dev_func.preemption_counter_get(unit, sync_mode, local_port,
                                             stat, val);
    PREEMPTION_COUNTER_UNLOCK(pc);

    return rv;
}

/*
 * Function:
 *      bcmi_esw_preemption_counter_set32
 * Purpose:
 *      Set 64-bit counter value for specified port statistic type.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      local_port  - (IN) GPORT ID
 *      stat        - (IN) Type of the counter to retrieve.
 *      val         - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_counter_set(int unit, bcm_port_t local_port,
                                bcm_port_stat_t stat, uint64 val)
{
    bcmi_preemption_counter_control_t *pc = preemption_counter_control[unit];
    int rv;

    if (NULL == pc) {
        return BCM_E_INIT;
    }

    PREEMPTION_COUNTER_LOCK(pc);
    rv = pc->dev_func.preemption_counter_set(unit, local_port, stat, val);
    PREEMPTION_COUNTER_UNLOCK(pc);

    return rv;
}

/*
 * Function:
 *      bcmi_esw_preemption_counter_cleanup
 * Purpose:
 *      cleanup resource after detach
 * Parameters:
 *      unit        - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_counter_cleanup(int unit)
{
    if (NULL != preemption_counter_control[unit]) {
        preemption_counter_control[unit]->dev_func.preemption_counter_clean(unit);
        if (NULL != preemption_counter_control[unit]->counter_mutex) {
            sal_mutex_destroy(preemption_counter_control[unit]->counter_mutex);
            preemption_counter_control[unit]->counter_mutex = NULL;
        }
        sal_free(preemption_counter_control[unit]);
        preemption_counter_control[unit] = NULL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_preemption_counter_init
 * Purpose:
 *      init premption counter module
 * Parameters:
 *      unit        - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_preemption_counter_init(int unit)
{
    bcmi_preemption_counter_control_t *pc = NULL;
    int rv;

    if (NULL != preemption_counter_control[unit]) {
        bcmi_esw_preemption_counter_cleanup(unit);
    }
    preemption_counter_control[unit] =
        sal_alloc(sizeof(bcmi_preemption_counter_control_t),
                  "preemption counter control");
    pc = preemption_counter_control[unit];
    if (NULL == pc) {
        return BCM_E_MEMORY;
    }
    sal_memset(pc, 0, sizeof(bcmi_preemption_counter_control_t));
    pc->counter_mutex = sal_mutex_create("counter_mutex");
    if (NULL == pc->counter_mutex) {
        bcmi_esw_preemption_counter_cleanup(unit);
        return BCM_E_INTERNAL;
    }

    /* init chip-specific func */
    rv = BCM_E_UNAVAIL;
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        rv = bcmi_gh2_preemption_counter_dev_func_init(&(pc->dev_func));
    }
#endif
    if (BCM_FAILURE(rv)) {
        bcmi_esw_preemption_counter_cleanup(unit);
        return rv;
    }

    rv = pc->dev_func.preemption_counter_init(unit);
    if (BCM_FAILURE(rv)) {
        bcmi_esw_preemption_counter_cleanup(unit);
        return rv;
    }

    return BCM_E_NONE;
}
#endif /* BCM_PREEMPTION_SUPPORT */

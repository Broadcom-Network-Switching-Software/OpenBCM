/* \file rx.c
 *
 * RX Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/rx.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/mbcm/rx.h>

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_RX

/*
 * \brief RX information data structure.
 */
typedef struct rx_info_s {

    /* Module is initialized. */
    bool inited;

    /* Module lock. */
    sal_mutex_t mutex;

} rx_info_t;

/*
 * Static global variable to hold RX info.
 */
static rx_info_t rx_info[BCM_MAX_NUM_UNITS] = {{0}};

/* RX info. */
#define RX_INFO(unit) \
    (&rx_info[unit])

/* Check if RX module is initialized. */
#define RX_INIT_CHECK(unit) \
    do { \
        rx_info_t *rx = RX_INFO(unit); \
        if (rx->inited == false) { \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT); \
        } \
    } while(0)

/* Take RX lock. */
#define RX_LOCK(unit) \
    sal_mutex_take(RX_INFO(unit)->mutex, SAL_MUTEX_FOREVER)

/* Give RX lock. */
#define RX_UNLOCK(unit) \
    sal_mutex_give(RX_INFO(unit)->mutex)

/******************************************************************************
 * Private functions
 */


/******************************************************************************
 * Public functions
 */

int
bcmi_ltsw_rx_detach(int unit)
{
    bool locked = false;
    SHR_FUNC_ENTER(unit);

    /* If not initialized, return success. */
    if (RX_INFO(unit)->inited == false) {
        SHR_EXIT();
    }

    RX_INFO(unit)->inited = false;

    RX_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_rx_detach(unit));

    RX_UNLOCK(unit);
    locked = false;

    if (RX_INFO(unit)->mutex) {
        sal_mutex_destroy(RX_INFO(unit)->mutex);
        RX_INFO(unit)->mutex = NULL;
    }

exit:
    if (locked) {
        RX_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_rx_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (RX_INFO(unit)->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_rx_detach(unit));
    }

    if (RX_INFO(unit)->mutex == NULL) {
        RX_INFO(unit)->mutex = sal_mutex_create("RX mutex");
        SHR_NULL_CHECK(RX_INFO(unit)->mutex, SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_rx_init(unit));
    RX_INFO(unit)->inited = true;
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_rx_cosq_mapping_extended_add(
    int unit,
    uint32 options,
    bcm_rx_cosq_mapping_t *rx_cosq_mapping)
{
    SHR_FUNC_ENTER(unit);
    RX_INIT_CHECK(unit);

    SHR_NULL_CHECK(rx_cosq_mapping, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_rx_cosq_mapping_extended_add(unit, options, rx_cosq_mapping));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_rx_cosq_mapping_extended_delete(
    int unit,
    bcm_rx_cosq_mapping_t *rx_cosq_mapping)
{
    SHR_FUNC_ENTER(unit);
    RX_INIT_CHECK(unit);

    SHR_NULL_CHECK(rx_cosq_mapping, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_rx_cosq_mapping_extended_delete(unit, rx_cosq_mapping));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_rx_cosq_mapping_extended_delete_all(int unit)
{
    SHR_FUNC_ENTER(unit);
    RX_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_rx_cosq_mapping_extended_delete_all(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_rx_cosq_mapping_extended_get(
    int unit,
    bcm_rx_cosq_mapping_t *rx_cosq_mapping)
{
    SHR_FUNC_ENTER(unit);
    RX_INIT_CHECK(unit);

    SHR_NULL_CHECK(rx_cosq_mapping, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_rx_cosq_mapping_extended_get(unit, rx_cosq_mapping));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_rx_queue_channel_get(
    int unit,
    bcm_cos_queue_t queue_id,
    bcm_rx_chan_t *chan_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_rx_queue_channel_get(unit, queue_id, chan_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_rx_queue_channel_set(
    int unit,
    bcm_cos_queue_t queue_id,
    bcm_rx_chan_t chan_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_rx_queue_channel_set(unit, queue_id, chan_id));

exit:
    SHR_FUNC_EXIT();
}


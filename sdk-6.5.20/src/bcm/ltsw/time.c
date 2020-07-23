/*! \file time.c
 *
 * BCM level APIs for time.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* SDK6 API header file */
#include <bcm/error.h>
#include <bcm/time.h>

/* Mixed header file */
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/time_int.h>
#include <bcm_int/ltsw/mbcm/time.h>

/* SDKLT header file */
#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <sal/sal_assert.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd_ts.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_TIME

/* Max time interface instance */
#define TIME_MAX_INTF_INSTANCE 1

/*!
 * \brief Time interface database.
 */
typedef struct ltsw_time_intf_db_s {
    /* interface. */
    bcm_time_interface_t intf;

    /* Reference cnt. */
    int cnt;
} ltsw_time_intf_db_t;

static ltsw_time_intf_db_t ltsw_time_intf_db[BCM_MAX_NUM_UNITS][TIME_MAX_INTF_INSTANCE];

/*!
 * \brief Time database.
 */
typedef struct ltsw_time_db_s {

    /*! Time initialized flag. */
    int         initialized;

    /*! Time module lock. */
    sal_mutex_t lock;

    /*! Flag to indicate that time module has been locked. */
    bool locked;

} ltsw_time_db_t;

/*!
 * \brief One time control entry for each SOC device containing time
 * book keeping information for that device.
 */
static ltsw_time_db_t ltsw_time_db[BCM_MAX_NUM_UNITS];

/*! Cause a routine to return SHR_E_INIT if timeing subsystem is not initialized. */
#define TIME_INIT(unit) \
    do { \
        if (ltsw_time_db[unit].initialized == FALSE) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while (0)

/*! Time mutex lock. */
#define TIME_LOCK(unit) \
    do { \
        if (ltsw_time_db[unit].lock && !ltsw_time_db[unit].locked) { \
            sal_mutex_take(ltsw_time_db[unit].lock, SAL_MUTEX_FOREVER); \
            ltsw_time_db[unit].locked = true; \
        } \
    } while (0)

/*! Time mutex unlock. */
#define TIME_UNLOCK(unit) \
    do { \
        if (ltsw_time_db[unit].lock && ltsw_time_db[unit].locked) { \
            sal_mutex_give(ltsw_time_db[unit].lock); \
            ltsw_time_db[unit].locked = false; \
        } \
    } while (0)

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Create protection mutex for time module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_time_lock_create(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_time_db[unit].lock == NULL) {
        ltsw_time_db[unit].lock = sal_mutex_create("bcmTimeMutex");
        SHR_NULL_CHECK(ltsw_time_db[unit].lock, SHR_E_MEMORY);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy protection mutex for time module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
static void
ltsw_time_lock_destroy(int unit)
{
    if (ltsw_time_db[unit].lock != NULL) {
        sal_mutex_destroy(ltsw_time_db[unit].lock);
        ltsw_time_db[unit].lock = NULL;
    }
}

/*!
 * \brief Initialize the time module.
 *
 * Initial time database data structure and
 * clear the time related Logical Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_MEMORY No Memory Resource.
 */
static int
ltsw_time_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ltsw_time_lock_create(unit));

    /*! Init Time module. */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_time_init(unit));

    ltsw_time_db[unit].initialized = TRUE;
    ltsw_time_db[unit].locked = false;
exit:
    if (SHR_FUNC_ERR()) {
        (void)ltsw_time_lock_destroy(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the time module.
 *
 * This function is used to de-initialize the time module.
 *
 * \param [in] unit Unit Number.
 */
static int
ltsw_time_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);
    TIME_INIT(unit);

    if (ltsw_time_db[unit].initialized == TRUE) {
        (void)ltsw_time_lock_destroy(unit);
        (void)mbcm_ltsw_time_deinit(unit);
    }
    ltsw_time_db[unit].initialized = FALSE;
    ltsw_time_db[unit].locked = false;
exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_time_synce_clock_set(
    int unit,
    bcm_time_synce_clock_src_type_t clk_src,
    bcm_time_synce_divisor_setting_t *divisor_setting)
{
    SHR_FUNC_ENTER(unit);
    TIME_INIT(unit);

    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_time_synce_clock_set
            (unit, clk_src, divisor_setting));
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_synce_clock_get(
    int unit,
    bcm_time_synce_clock_src_type_t clk_src,
    bcm_time_synce_divisor_setting_t *divisor_setting)
{
    SHR_FUNC_ENTER(unit);
    TIME_INIT(unit);

    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_time_synce_clock_get(
            unit, clk_src, divisor_setting));
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_synce_clock_source_control_set(
    int unit,
    bcm_time_synce_clock_source_config_t *clk_src_config,
    bcm_time_synce_clock_source_control_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);
    TIME_INIT(unit);

    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_time_synce_clock_source_control_set
            (unit, clk_src_config, control, value));
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_synce_clock_source_control_get(
    int unit,
    bcm_time_synce_clock_source_config_t *clk_src_config,
    bcm_time_synce_clock_source_control_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);
    TIME_INIT(unit);

    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_time_synce_clock_source_control_get
            (unit, clk_src_config, control, value));
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_tod_set(
    int unit,
    uint32 stages,
    bcm_time_tod_t *tod)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    TIME_INIT(unit);

    TIME_LOCK(unit);

    if (stages & BCM_TIME_STAGE_ALL) {
        uint8 stage, set_cnt = 0;
        for (stage = BCM_TIME_STAGE_INGRESS_FLOWTRACKER;
             stage <= BCM_TIME_STAGE_MMU;
             stage++) {

            rv = mbcm_ltsw_time_tod_set(unit, stages, tod);
            if (BCM_SUCCESS(rv)) {
                set_cnt++;
            }
        }
        if (set_cnt == 0) {
            /* Loop all and cannot set a successful one. */
            SHR_ERR_EXIT(BCM_E_UNAVAIL);
        } else {
            SHR_EXIT();
        }
    } else {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_time_tod_set(unit, stages, tod));
    }
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_tod_get(
    int unit,
    uint32 stages,
    bcm_time_tod_t *tod)
{
    int rv = BCM_E_UNAVAIL;

    SHR_FUNC_ENTER(unit);
    TIME_INIT(unit);

    TIME_LOCK(unit);

    if (stages & BCM_TIME_STAGE_ALL) {
        uint32 stage;
        for (stage = BCM_TIME_STAGE_INGRESS_FLOWTRACKER;
             stage <= BCM_TIME_STAGE_MMU;
             stage++) {
            rv = mbcm_ltsw_time_tod_get(unit, stages, tod);
            if (BCM_SUCCESS(rv)) {
                SHR_EXIT();
            }
        }
        /* Loop all and cannot get a successful one. */
        SHR_ERR_EXIT(BCM_E_UNAVAIL);
    } else {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_time_tod_get(unit, stages, tod));
    }
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_port_timesync_config_set(
    int unit,
    bcm_port_t port,
    int config_count,
    bcm_port_timesync_config_t *config_array)
{
    SHR_FUNC_ENTER(unit);
    TIME_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (((config_count > 0) && (NULL == config_array)) ||
        ((config_count == 0) && (NULL != config_array))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_time_port_timesync_config_set
            (unit, port, config_count, config_array));
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_port_timesync_config_get(
    int unit,
    bcm_port_t port,
    int array_size,
    bcm_port_timesync_config_t *config_array,
    int *array_count)
{
    SHR_FUNC_ENTER(unit);
    TIME_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_time_port_timesync_config_get
            (unit, port, array_size, config_array, array_count));
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_switch_control_set(
    int unit,
    bcm_switch_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);
    TIME_INIT(unit);

    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_time_switch_control_set(unit, type, arg));
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_switch_control_get(
    int unit,
    bcm_switch_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);
    TIME_INIT(unit);

    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_time_switch_control_get(unit, type, arg));
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_switch_control_port_set(
    int unit,
    bcm_port_t port,
    bcm_switch_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);
    TIME_INIT(unit);

    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_time_switch_control_port_set(unit, port, type, arg));
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_switch_control_port_get(
    int unit,
    bcm_port_t port,
    bcm_switch_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);
    TIME_INIT(unit);

    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_time_switch_control_port_get(unit, port, type, arg));
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

STATIC int
ltsw_time_interface_id_validate(int unit, bcm_time_if_t id)
{
    SHR_FUNC_ENTER(unit);

    TIME_INIT(unit);
    if (id < 0 || id > TIME_MAX_INTF_INSTANCE) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (0 == ltsw_time_intf_db[unit][id].cnt) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
ltsw_time_interface_allocate_id(int unit, bcm_time_if_t *id)
{
    int idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(id, SHR_E_PARAM);

    /* Find & allocate time interface. */
    for (idx = 0; idx < TIME_MAX_INTF_INSTANCE; idx++) {
        if (ltsw_time_intf_db[unit][idx].cnt) {
            /* In use interface. */
            continue;
        }
         /* If found and then mark interface as in used. */
        ltsw_time_intf_db[unit][idx].cnt = 1;
        /* Assign ID. */
        *id = idx;
        SHR_EXIT();
    }
    /* No available interfaces. */
    SHR_ERR_EXIT(SHR_E_FULL);
exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_time_interface_add(
    int unit,
    bcm_time_interface_t *intf)
{
    int dunit = 0;
    bcm_time_if_t intf_id;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(intf, SHR_E_PARAM);

    TIME_INIT(unit);

    if (!(intf->flags & BCM_TIME_ENABLE ||
          intf->flags & BCM_TIME_WITH_ID)) {
         SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (intf->flags & BCM_TIME_WITH_ID) {
        intf_id = intf->id;
        rv = ltsw_time_interface_id_validate(unit, intf_id);
        if (rv == SHR_E_NONE) {
            if (0 == (intf->flags & BCM_TIME_REPLACE)) {
                SHR_IF_ERR_EXIT(SHR_E_EXISTS);
            }
        } else if (rv != SHR_E_NOT_FOUND){
            SHR_IF_ERR_EXIT(rv);
        }

        /* If interface is created WITH_ID, set 'cnt' here. */
        ltsw_time_intf_db[unit][intf_id].cnt = 1;
    } else {
        SHR_IF_ERR_EXIT
            (ltsw_time_interface_allocate_id(unit, &intf_id));
        intf->id = intf_id;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    TIME_LOCK(unit);

    if (intf->flags & BCM_TIME_ENABLE) {
        SHR_IF_ERR_EXIT
            (bcmbd_ts_timestamp_enable_set(dunit, intf_id, true));
        ltsw_time_intf_db[unit][intf_id].intf.flags |= BCM_TIME_ENABLE;
    } else {
        SHR_IF_ERR_EXIT
            (bcmbd_ts_timestamp_enable_set(dunit, intf_id, false));
        ltsw_time_intf_db[unit][intf_id].intf.flags &= ~BCM_TIME_ENABLE;
    }

    ltsw_time_intf_db[unit][intf_id].intf.id = intf_id;

exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_interface_delete(
    int unit,
    bcm_time_if_t intf_id)
{
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    TIME_INIT(unit);

    SHR_IF_ERR_EXIT
        (ltsw_time_interface_id_validate(unit, intf_id));

    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_ts_timestamp_enable_set(dunit, intf_id, false));
    ltsw_time_intf_db[unit][intf_id].cnt = 0;
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_interface_get(
    int unit,
    bcm_time_interface_t *intf)
{
    bool enable;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(intf, SHR_E_PARAM);

    TIME_INIT(unit);

    SHR_IF_ERR_EXIT
        (ltsw_time_interface_id_validate(unit, intf->id));

    SHR_IF_ERR_EXIT
        (bcmbd_ts_timestamp_enable_get(unit, intf->id, &enable));

    if (enable) {
        intf->flags |= BCM_TIME_ENABLE;
    } else {
        intf->flags &= ~BCM_TIME_ENABLE;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_time_interface_delete_all(
    int unit)
{
    bcm_time_if_t intf_id;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    TIME_INIT(unit);

    TIME_LOCK(unit);

    for (intf_id = 0; intf_id < TIME_MAX_INTF_INSTANCE; intf_id++ ) {
        rv = ltsw_time_interface_id_validate(unit, intf_id);
        if (SHR_SUCCESS(rv)) {
            SHR_IF_ERR_EXIT
                (ltsw_time_interface_delete(unit, intf_id));
        }
    }
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_interface_traverse(
    int unit,
    bcm_time_interface_traverse_cb cb,
    void *user_data)
{
    bcm_time_if_t intf_id;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    TIME_INIT(unit);

    TIME_LOCK(unit);

    for (intf_id = 0; intf_id < TIME_MAX_INTF_INSTANCE; intf_id++) {
        rv = ltsw_time_interface_id_validate(unit, intf_id);
        if (SHR_SUCCESS(rv)) {
            rv = (cb(unit, &(ltsw_time_intf_db[unit][intf_id].intf), user_data));
            if (SHR_SUCCESS(rv)) {
                break;
            }
        }
    }
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_ts_counter_get(
    int unit,
    bcm_time_ts_counter_t *counter)
{
    bool enable;
    uint64_t timestamp;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(counter, SHR_E_PARAM);

    TIME_INIT(unit);

    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_ts_timestamp_enable_get(unit, counter->ts_counter, &enable));

    rv = bcmbd_ts_timestamp_nanosec_get(unit, counter->ts_counter, &timestamp);
    if (rv == SHR_E_DISABLED) {
        rv = SHR_E_NONE;
    }

    SHR_IF_ERR_EXIT(rv);

    counter->enable = enable;
    counter->ts_counter_ns = timestamp;

exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_ts_counter_set(
    int unit,
    bcm_time_ts_counter_t *counter)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(counter, SHR_E_PARAM);

    TIME_INIT(unit);

    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_ts_timestamp_enable_set(unit, counter->ts_counter,
                                       counter->enable));

    SHR_IF_ERR_EXIT
        (bcmbd_ts_timestamp_nanosec_set(unit, counter->ts_counter,
                                        counter->ts_counter_ns));

exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_ts_offset_set(
    int unit,
    int64_t ts_offset,
    uint64_t ts_update_at,
    int ts_counter)
{
    bool enable;
    uint64_t cur_offset, new_offset, mask;
    uint32_t valid_bits, frac_bits;

    SHR_FUNC_ENTER(unit);

    TIME_INIT(unit);

    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_ts_timestamp_enable_get(unit, ts_counter, &enable));
    if (!enable) {
        SHR_ERR_EXIT(SHR_E_DISABLED);
    }

    SHR_IF_ERR_EXIT
        (bcmbd_ts_timestamp_raw_get(unit, ts_counter,
                                    NULL, &valid_bits, &frac_bits));
    /* Bit mask for timestamp in nanoseconds */
    mask = (1ULL << (valid_bits - frac_bits)) - 1;
    /* Check the input parameter (in nanoseconds) is not out of range */
    if ((ts_update_at & ~mask) != 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Calculate new offset value */
    SHR_IF_ERR_EXIT
        (bcmbd_ts_timestamp_raw_offset_get(unit, ts_counter, &cur_offset));
    /* Bit mask for timestamp raw data */
    mask = (1ULL << valid_bits) - 1;
    new_offset = (cur_offset + ts_offset) & mask;

    /* Update nanoseconds to TimeSync hardware timestamp format */
    ts_update_at <<= frac_bits;

    SHR_IF_ERR_EXIT
        (bcmbd_ts_timestamp_raw_offset_set(unit, ts_counter,
                                           new_offset, ts_update_at));

exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_time_capture_get(
    int unit,
    bcm_time_if_t id,
    bcm_time_capture_t *time)
{
    uint64_t timestamp;
    uint64 secs, nano_secs;
    uint64 div;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    TIME_INIT(unit);

    TIME_LOCK(unit);

    if (!(time->flags & BCM_TIME_CAPTURE_IMMEDIATE)) {
         SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (ltsw_time_interface_id_validate(unit, id));

    rv = bcmbd_ts_timestamp_nanosec_get(unit, id, &timestamp);
    if (rv == SHR_E_DISABLED) {
        rv = SHR_E_NONE;
    }

    SHR_IF_ERR_EXIT(rv);

    /* Get seconds from the timestamp value */
    secs = timestamp;
    COMPILER_64_SET(div, 0, 1000000000);
    COMPILER_64_UDIV_64(secs, div);
    time->synchronous.seconds = secs;

    /* Get nanoseconds from the timestamp value */
    nano_secs = timestamp;
    COMPILER_64_UMUL_32(secs, 1000000000);
    COMPILER_64_SUB_64(nano_secs, secs);
    time->synchronous.nanoseconds = COMPILER_64_LO(nano_secs);

    /* For backward compatibility */
    time->free = time->synchronous;
    time->syntonous = time->synchronous;
    time->flags |= BCM_TIME_CAPTURE_IMMEDIATE;

exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Specify time general switch behaviors.
 *
 * \param [in] unit Unit Number.
 * \param [in] type The desired configuration parameter to modify.
 * \param [in] arg he value with which to set the parameter.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_time_switch_control_set(
    int unit,
    bcm_switch_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_switch_control_set(unit, type, arg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve time general switch behaviors.
 *
 * \param [in] unit Unit Number.
 * \param [in] type The desired configuration parameter to modify.
 * \param [out] arg he value with which to set the parameter.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_time_switch_control_get(
    int unit,
    bcm_switch_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_switch_control_get(unit, type, arg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Specify time port-specific and device-wide operating configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] type Switch control type.
 * \param [in] arg Value to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_time_switch_control_port_set(
    int unit,
    bcm_port_t port,
    bcm_switch_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_switch_control_port_set(unit, port, type, arg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve time port-specific and device-wide operating configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] type Switch control type.
 * \param [out] arg Value to get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_time_switch_control_port_get(
    int unit,
    bcm_port_t port,
    bcm_switch_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_switch_control_port_get(unit, port, type, arg));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_esw_time_init
 * Purpose:
 *      Initialize time module
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_ltsw_time_init (int unit)
{

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_init(unit));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_esw_time_deinit
 * Purpose:
 *      Uninitialize time module
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 */
int
bcm_ltsw_time_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
    (ltsw_time_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set syncE clock output frequency beased on the input divisor settings.
 *
 * \param [in] unit Unit Number.
 * \param [in] clk_src SyncE clock source type.
 * \param [in] divisor_setting SyncE divisor setting.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_time_synce_clock_set(
    int unit,
    bcm_time_synce_clock_src_type_t clk_src,
    bcm_time_synce_divisor_setting_t *divisor)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_synce_clock_set(
            unit, clk_src, divisor));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get current syncE input divisor settings.
 *
 * \param [in] unit Unit Number.
 * \param [in] clk_src SyncE clock source type.
 * \param [out] divisor_setting SyncE divisor setting.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_time_synce_clock_get(
    int unit,
    bcm_time_synce_clock_src_type_t clk_src,
    bcm_time_synce_divisor_setting_t *divisor)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_synce_clock_get(
            unit, clk_src, divisor));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set Primary or Secondary SyncE Recovered clock.
 *
 * \param [in] unit Unit Number.
 * \param [in] clk_src_config SyncE clock source type.
 * \param [in] control SyncE divisor setting.
 * \param [in] value Value to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_time_synce_clock_source_control_set(
    int unit,
    bcm_time_synce_clock_source_config_t *clk_src_config,
    bcm_time_synce_clock_source_control_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_synce_clock_source_control_set(
            unit, clk_src_config, control, value));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get Primary or Secondary SyncE Recovered clock.
 *
 * \param [in] unit Unit Number.
 * \param [in] clk_src_config SyncE clock source type.
 * \param [in] control SyncE divisor setting.
 * \param [out] value Value to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_time_synce_clock_source_control_get(
    int unit,
    bcm_time_synce_clock_source_config_t *clk_src_config,
    bcm_time_synce_clock_source_control_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_synce_clock_source_control_get(
            unit, clk_src_config, control, value));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set time of the day values at various stages in ASIC.
 *
 * \param [in] unit Unit Number.
 * \param [in] stages Time stamping stages.
 * \param [in] tod Structure for Time of the day values.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_time_tod_set(int unit, uint32 stages, bcm_time_tod_t *tod)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_tod_set(unit, stages, tod));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get time of the day values at various stages in ASIC.
 *
 * \param [in] unit Unit Number.
 * \param [in] stages Time stamping stages.
 * \param [out] tod Structure for Time of the day values.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_time_tod_get(int unit, uint32 stages, bcm_time_tod_t *tod)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_tod_get(unit, stages, tod));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get instantaneous ts counter value.
 *
 * \param [in] unit Unit Number.
 * \param [in] counter ts counter structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_time_ts_counter_get(int unit, bcm_time_ts_counter_t *counter)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_ts_counter_get(unit, counter));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set instantaneous ts counter value.
 *
 * \param [in] unit Unit Number.
 * \param [in] counter ts counter structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_time_ts_counter_set(int unit, bcm_time_ts_counter_t *counter)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_ts_counter_set(unit, counter));
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set offset to ts counter value.
 *
 * \param [in] unit Unit Number.
 * \param [in] counter ts counter structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_time_ts_offset_set(int unit, int64_t ts_offset, uint64_t ts_update_at, int ts_counter)

{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_ts_offset_set(unit, ts_offset, ts_update_at, ts_counter));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set timesync configurations for the port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] config_count Input configuration size.
 * \param [in] config_array Timesync configuration array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_port_timesync_config_set(
    int unit,
    bcm_port_t port,
    int config_count,
    bcm_port_timesync_config_t *config_array)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_port_timesync_config_set(
            unit, port, config_count, config_array));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get timesync configurations for the port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] array_size Input array size.
 * \param [out] config_array Timesync configuration array.
 * \param [out] array_count Fetched array size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_port_timesync_config_get(
    int unit,
    bcm_port_t port,
    int array_size,
    bcm_port_timesync_config_t *config_array,
    int *array_count)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_port_timesync_config_get(
            unit, port, array_size, config_array, array_count));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Gets a time captured by HW clock.
 *
 * \param [in] unit Unit Number.
 * \param [in] Time Sync Interface id.
 * \param [out] time Structure to contain HW clocks values.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_time_capture_get(
    int unit,
    bcm_time_if_t id,
    bcm_time_capture_t *time)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_capture_get(unit, id, time));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Adding a time sync interface to a specified unit.
 *
 * \param [in] unit Unit Number.
 * \param [in/out] Time Sync Interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_time_interface_add(
    int unit,
    bcm_time_interface_t *intf)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_interface_add(unit, intf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Adding a time sync interface to a specified unit.
 *
 * \param [in] unit Unit Number.
 * \param [in] Time Sync Interface id to remove.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_time_interface_delete(
    int unit,
    bcm_time_if_t intf_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_interface_delete(unit, intf_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Get a time sync interface on a specified unit.
 *
 * \param [in] unit Unit Number.
 * \param [in] Time Sync Interface id to remove.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_time_interface_get(
    int unit,
    bcm_time_interface_t *intf)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_interface_get(unit, intf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Deleting all time sync interfaces on a unit.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_time_interface_delete_all(
    int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_interface_delete_all(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Iterates over all time sync interfaces and calls given callback.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_time_interface_traverse(
    int unit,
    bcm_time_interface_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_interface_traverse(unit, cb, user_data));
exit:
    SHR_FUNC_EXIT();
}


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
#include <bcm_int/control.h>

/* Mixed header file */
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/time_int.h>
#include <bcm_int/ltsw/mbcm/time.h>
#include <bcm_int/ltsw/generated/time_ha.h>

/* SDKLT header file */
#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <sal/sal_assert.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd_ts.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_TIME

#define BCMINT_TIME_SUB_COMP_ID_INTF 0

/* Number of interface instance. */
#define TIME_INTF_INSTANCE_NUM 2

/*!
 * \brief Time interface database.
 */
typedef struct ltsw_time_intf_db_s {
    /* interface. */
    bcm_time_interface_t intf;
} ltsw_time_intf_db_t;

static ltsw_time_intf_db_t ltsw_time_intf_db[BCM_MAX_NUM_UNITS][TIME_INTF_INSTANCE_NUM];

static bcmint_time_intf_ha_db_t *ltsw_time_intf_ha_db[BCM_MAX_NUM_UNITS];

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

/*!
 * \brief Compare the field value in two same-type variables.
 *
 * _var1 and _var2 are the points to two variables with the same data type.
 * _f is a field name in the data structure.
 *
 * \param [in] _var1 The first variable.
 * \param [in] _var2 The second variable.
 * \param [in] _f field name.
 *
 * \retval 1 The value of field \c _f is same in \c _var1 and \c _var2.
 * \retval 0 The value of \c _f is different.
 */
#define TIME_FIELD_VAL_EQUAL(_var1, _var2, _f) \
    (sal_memcmp(&((_var1)->_f), &((_var2)->_f), sizeof((_var1)->_f)) == 0)

/******************************************************************************
 * Private functions
 */
static int
bcm_ltsw_bs_clear_tables(int unit)
{
    int i, num;
    const char *ltname[] = {
        DEVICE_TS_BROADSYNC_INTERFACEs,
        DEVICE_TS_BROADSYNC_LOGs,
        DEVICE_TS_BROADSYNC_LOG_CONTROLs,
        DEVICE_TS_BROADSYNC_STATUSs,
        DEVICE_TS_BROADSYNC_LOG_STATUSs,
        DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTs,
        DEVICE_TS_BROADSYNC_CONTROLs,
    };

    SHR_FUNC_ENTER(unit);

    num = COUNTOF(ltname);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                        "BS(unit %d) num_tables(%d).\n"),
                    unit, num));

    for (i = 0; i < num; i++) {
        SHR_IF_ERR_CONT
            (bcmi_lt_clear(unit, ltname[i]));
    }
    SHR_VERBOSE_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

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
             stage = (stage << 1)) {
            rv = mbcm_ltsw_time_tod_set(unit, stage, tod);
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
             stage = (stage << 1)) {
            rv = mbcm_ltsw_time_tod_get(unit, stage, tod);
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

int
bcmi_ltsw_bs_control_entry_set(int unit,
        bool update,
        bcm_time_interface_t *intf)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DEVICE_TS_BROADSYNC_CONTROLs, &entry_hdl));

    /* BS will be enabled by default */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, BROADSYNCs, 1));

    if (update == false) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_bs_control_entry_get(int unit,
        bcm_time_interface_t *intf)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DEVICE_TS_BROADSYNC_CONTROLs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, OPERATIONAL_STATEs, &val));

    if (val != 0) {
        _func_rv = SHR_E_INTERNAL;
    } else {
        _func_rv = SHR_E_NONE;
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_bs_interface_entry_get(int unit,
        bcm_time_interface_t *intf)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, DEVICE_TS_BROADSYNC_INTERFACEs, &entry_hdl));

    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, DEVICE_TS_BROADSYNC_INTERFACE_IDs, intf->id));

    SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL));

    intf->flags |= BCM_TIME_SYNC_STAMPER;
    intf->flags |= BCM_TIME_WITH_ID;
    intf->flags |= BCM_TIME_HEARTBEAT;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, BITCLOCK_HZs, &val));
    intf->bitclock_hz = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, HEARTBEAT_HZs, &val));
    intf->heartbeat_hz = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TIME_SIGNs, &val));
    intf->bs_time.isnegative = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TIME_SECs, &val));
    intf->bs_time.seconds = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TIME_NSECs, &val));
    intf->bs_time.nanoseconds = val;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_bs_interface_entry_set(int unit,
        bool update,
        bcm_time_interface_t *intf)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *mode;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, DEVICE_TS_BROADSYNC_INTERFACEs, &entry_hdl));

    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, DEVICE_TS_BROADSYNC_INTERFACE_IDs, intf->id));

    if (!(intf->flags & BCM_TIME_REPLACE)) {
        if (intf->flags & BCM_TIME_INPUT) {
            mode = INPUT_MODEs;
        } else {
            mode = OUTPUT_MODEs;
        }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl, MODEs, mode));

        if (intf->flags & BCM_TIME_HEARTBEAT) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, BITCLOCK_HZs, (uint64_t)intf->bitclock_hz));

            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, HEARTBEAT_HZs, intf->heartbeat_hz));
        }
    } else {
        if ((intf->flags & BCM_TIME_INPUT) !=
            (ltsw_time_intf_db[unit][intf->id].intf.flags & BCM_TIME_INPUT)) {
            if (intf->flags & BCM_TIME_INPUT) {
                mode = INPUT_MODEs;
            } else {
                mode = OUTPUT_MODEs;
            }
            SHR_IF_ERR_EXIT
                  (bcmlt_entry_field_symbol_add(entry_hdl, MODEs, mode));
        }

        if ((intf->flags & BCM_TIME_HEARTBEAT) &&
            ((intf->bitclock_hz != ltsw_time_intf_db[unit][intf->id].intf.bitclock_hz)||
             (intf->heartbeat_hz != ltsw_time_intf_db[unit][intf->id].intf.heartbeat_hz))){
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, BITCLOCK_HZs, intf->bitclock_hz));

            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, HEARTBEAT_HZs, intf->heartbeat_hz));
        }
    }

    if (intf->flags & BCM_TIME_DRIFT) {
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(entry_hdl,
                                                      FREQ_ADJUST_SECs,
                                                      intf->drift.seconds));
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(entry_hdl,
                                                      FREQ_ADJUST_NSECs,
                                                      intf->drift.nanoseconds));
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(entry_hdl,
                                                      FREQ_ADJUST_SIGNs,
                                                      intf->drift.isnegative));
    }

    if (intf->flags & BCM_TIME_OFFSET) {
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(entry_hdl,
                                                      PHASE_ADJUST_SECs,
                                                      intf->offset.seconds));
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(entry_hdl,
                                                      PHASE_ADJUST_NSECs,
                                                      intf->offset.nanoseconds));
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(entry_hdl,
                                                      PHASE_ADJUST_SIGNs,
                                                      intf->offset.isnegative));
    }

    if (intf->flags & BCM_TIME_BS_TIME) {
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(entry_hdl,
                                                      TIME_SECs,
                                                      intf->bs_time.seconds));
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(entry_hdl,
                                                      TIME_NSECs,
                                                      intf->bs_time.nanoseconds));
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(entry_hdl,
                                                      TIME_SIGNs,
                                                      intf->bs_time.isnegative));
    }

    if (update == false) {
        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_bs_status_entry_get(int unit,
                             bcm_time_interface_t *intf)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, DEVICE_TS_BROADSYNC_STATUSs, &entry_hdl));

    val = intf->id;
    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, DEVICE_TS_BROADSYNC_INTERFACE_IDs, val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, STATEs, &val));

    intf->status = val;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_bs_log_control_set(int unit,
                             bcm_time_bs_log_cfg_t bs_log_cfg)
{
    uint64_t val = 0;
    int dunit;
    int rv = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DEVICE_TS_BROADSYNC_LOG_CONTROLs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, DEBUG_MASKs, bs_log_cfg.debug_mask));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, UDP_LOG_MASKs, bs_log_cfg.udp_log_mask));

    bcmi_ltsw_util_mac_to_uint64(&val, bs_log_cfg.src_mac);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, SRC_MACs, val));

    bcmi_ltsw_util_mac_to_uint64(&val, bs_log_cfg.dest_mac);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, DST_MACs, val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TPIDs, bs_log_cfg.tpid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_IDs, bs_log_cfg.vid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TTLs, bs_log_cfg.ttl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, SRC_IPV4s, bs_log_cfg.src_addr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, DST_IPV4s, bs_log_cfg.dest_addr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, UDP_SRC_PORTs, bs_log_cfg.udp_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, UDP_DST_PORTs, bs_log_cfg.udp_port));

    rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_EXISTS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_bs_log_control_get(int unit,
                             bcm_time_bs_log_cfg_t *bs_log_cfg)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DEVICE_TS_BROADSYNC_LOG_CONTROLs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DEBUG_MASKs, &val));
    bs_log_cfg->debug_mask = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, UDP_LOG_MASKs, &val));
    bs_log_cfg->udp_log_mask = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, SRC_MACs, &val));
    bcmi_ltsw_util_uint64_to_mac(bs_log_cfg->src_mac, &val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DST_MACs, &val));
    bcmi_ltsw_util_uint64_to_mac(bs_log_cfg->dest_mac, &val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TPIDs, &val));
    bs_log_cfg->tpid = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, VLAN_IDs, &val));
    bs_log_cfg->vid = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TTLs, &val));
    bs_log_cfg->ttl = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, SRC_IPV4s, &val));
    bs_log_cfg->src_addr = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DST_IPV4s, &val));
    bs_log_cfg->dest_addr = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, UDP_SRC_PORTs, &val));
    bs_log_cfg->udp_port = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, UDP_DST_PORTs, &val));
    bs_log_cfg->udp_port = val;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_bs_hb_enable_set(int unit, bcm_time_if_t id, int enable)
{
    int dunit;
    int rv = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, DEVICE_TS_BROADSYNC_SIGNAL_OUTPUT_IDs, GPIO_1s));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FREQUENCYs, (enable?1:0) ));

    rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_EXISTS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_time_hb_enable_get(int unit,
                             bcm_time_if_t id, int *enable)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DEVICE_TS_BROADSYNC_SIGNAL_OUTPUTs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, DEVICE_TS_BROADSYNC_SIGNAL_OUTPUT_IDs, GPIO_1s));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, FREQUENCYs, &val));
    *enable = (val ? 1 : 0);

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_bs_log_entry_get(int unit, bcmlt_entry_handle_t entry_hdl)
{

    uint64_t log_id = 0;
    uint64_t log_data_cnt = 0;
    int i;
    uint32_t r_cnt;
    uint64_t log_data[101];
    char data[101];
    bcmlt_entry_handle_t stat_entry_hdl = BCMLT_INVALID_HDL;
    uint64_t status_log_id = 0;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DEVICE_TS_BROADSYNC_LOG_STATUSs, &stat_entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, stat_entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(stat_entry_hdl, DEVICE_TS_BROADSYNC_LOG_IDs, &status_log_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DEVICE_TS_BROADSYNC_LOG_IDs, &log_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, LOG_DATA_CNTs, &log_data_cnt));

     SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(entry_hdl,
                                         LOG_DATAs,
                                         0,
                                         &log_data[0],
                                         log_data_cnt,
                                         &r_cnt));
    for ( i = 0; i < log_data_cnt; i++) {
       data[i] = (char)log_data[i];
    }
    data[i]=0;
    LOG_CLI((BSL_META(" status_log_id[%lu] log_id:%lu log_data_cnt:%lu \n %s \n"),
                 (long unsigned int)status_log_id,
                 (long unsigned int)log_id,
                 (long unsigned int)log_data_cnt, data));

exit:
    if (stat_entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(stat_entry_hdl);
        stat_entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();

}

static void
bcm_ltsw_bs_log_callback(bcmlt_table_notif_info_t *notify_info, void *user_data)
{
    int unit;
    bcmlt_entry_handle_t ent_hdl;
    uint32_t rv = 0;

    unit = notify_info->unit;
    ent_hdl = notify_info->entry_hdl;

    rv = bcmi_ltsw_bs_log_entry_get(unit, ent_hdl);
    if (SHR_FAILURE(rv)) {
        return;
    }
}

STATIC int
ltsw_time_interface_id_validate(int unit, bcm_time_if_t id)
{
    SHR_FUNC_ENTER(unit);

    TIME_INIT(unit);
    if (id < 0 || id >= TIME_INTF_INSTANCE_NUM) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (ltsw_time_intf_ha_db[unit][id].cnt == 0) {
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
    for (idx = 0; idx < TIME_INTF_INSTANCE_NUM; idx++) {
        if (ltsw_time_intf_ha_db[unit][idx].cnt) {
            /* In use interface. */
            continue;
        }
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
    int rv = SHR_E_NONE;
    uint32_t valid_flags;
    bcm_time_if_t intf_id;
    int i=0;
    int init_done=0;
    bool update=false;
    bcm_time_interface_t intf_data, *intf_def = &intf_data;

    SHR_FUNC_ENTER(unit);

    /* Sanitize first */
    SHR_NULL_CHECK(intf, SHR_E_PARAM);

    if (!(intf->flags & BCM_TIME_SYNC_STAMPER)) {
        /* skip the below checks for broadsync */
        valid_flags = BCM_TIME_ENABLE | BCM_TIME_WITH_ID | BCM_TIME_REPLACE;
        if (intf->flags & ~valid_flags) {
            /* The given flags are not support */
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }

        bcm_time_interface_t_init(intf_def);
        if (!TIME_FIELD_VAL_EQUAL(intf, intf_def, drift) ||
                !TIME_FIELD_VAL_EQUAL(intf, intf_def, offset) ||
                !TIME_FIELD_VAL_EQUAL(intf, intf_def, accuracy) ||
                !TIME_FIELD_VAL_EQUAL(intf, intf_def, heartbeat_hz) ||
                !TIME_FIELD_VAL_EQUAL(intf, intf_def, clk_resolution) ||
                !TIME_FIELD_VAL_EQUAL(intf, intf_def, bitclock_hz) ||
                !TIME_FIELD_VAL_EQUAL(intf, intf_def, status) ||
                !TIME_FIELD_VAL_EQUAL(intf, intf_def, ntp_offset) ||
                !TIME_FIELD_VAL_EQUAL(intf, intf_def, bs_time)) {
            /* The given fields are not support */
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    }

    TIME_INIT(unit);

    TIME_LOCK(unit);

    if (intf->flags & BCM_TIME_WITH_ID) {
        intf_id = intf->id;
        rv = ltsw_time_interface_id_validate(unit, intf_id);
        if (rv == SHR_E_NONE) {
            if ((intf->flags & BCM_TIME_REPLACE) == 0) {
                SHR_IF_ERR_EXIT(SHR_E_EXISTS);
            }
        } else if (rv != SHR_E_NOT_FOUND){
            SHR_IF_ERR_EXIT(rv);
        }
    } else {
        SHR_IF_ERR_EXIT
            (ltsw_time_interface_allocate_id(unit, &intf_id));
        intf->id = intf_id;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    if (intf->flags & BCM_TIME_SYNC_STAMPER) {
        init_done=0;
        for (i=0; i < TIME_INTF_INSTANCE_NUM; i++) {
            if (ltsw_time_intf_ha_db[unit][i].cnt) {
                init_done = 1;
            }
        }

        if (!init_done) {
            rv = bcmi_ltsw_bs_control_entry_set(unit, false, intf);
            if ((rv != SHR_E_NONE)) {
                LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit,
                                "BS(unit %d) control entry insert fail.\n"),
                            unit));
                SHR_EXIT();
            }

            SHR_IF_ERR_EXIT
                (bcmlt_table_subscribe(unit,
                                       DEVICE_TS_BROADSYNC_LOGs,
                                       &bcm_ltsw_bs_log_callback,
                                       NULL));
        }

        rv = bcmi_ltsw_bs_control_entry_get(unit, intf);
        if ((rv != SHR_E_NONE)) {
            LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit,
                            "BS(unit %d) Warning: BS app not initialized.\n"),
                        unit));
            SHR_EXIT();
        }
    }

    /* If found and then mark interface as in used. */
    ltsw_time_intf_ha_db[unit][intf_id].cnt = 1;

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

    if ((intf->flags & BCM_TIME_SYNC_STAMPER)) {
        if ((intf->flags & BCM_TIME_REPLACE)) {
            update = true;
        } else {
            update = false;
        }

        SHR_IF_ERR_VERBOSE_EXIT(
                bcmi_ltsw_bs_interface_entry_set(unit, update, intf));

        if (!(intf->flags & BCM_TIME_REPLACE)) {
            ltsw_time_intf_db[unit][intf_id].intf = *intf;
        }
    }

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
    ltsw_time_intf_ha_db[unit][intf_id].cnt = 0;
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

    bcmi_ltsw_bs_status_entry_get(unit, intf);
    bcmi_ltsw_bs_interface_entry_get(unit, intf);

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

    for (intf_id = 0; intf_id < TIME_INTF_INSTANCE_NUM; intf_id++) {
        rv = ltsw_time_interface_id_validate(unit, intf_id);
        if (SHR_SUCCESS(rv)) {
            SHR_IF_ERR_EXIT
                (ltsw_time_interface_delete(unit, intf_id));
        }
    }
exit:
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

    for (intf_id = 0; intf_id < TIME_INTF_INSTANCE_NUM; intf_id++) {
        rv = ltsw_time_interface_id_validate(unit, intf_id);
        if (SHR_SUCCESS(rv)) {
            rv = (cb(unit, &(ltsw_time_intf_db[unit][intf_id].intf), user_data));
            if (rv == BCM_E_NONE) {
                LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit, "Found intf[%d] \n"), intf_id));
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
    uint32_t valid_flags;

    SHR_FUNC_ENTER(unit);

    valid_flags = BCM_TIME_CAPTURE_IMMEDIATE;
    if (time->flags & ~valid_flags) {
        /* The given flags are not supported */
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    TIME_INIT(unit);

    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (ltsw_time_interface_id_validate(unit, id));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmbd_ts_timestamp_nanosec_get(unit, id, &timestamp), SHR_E_DISABLED);

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

int
ltsw_time_timesync_config_profile_create(
    int unit,
    bcm_time_timesync_config_profile_t *profile,
    int *profile_id)
{
    SHR_FUNC_ENTER(unit);

    TIME_INIT(unit);
    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_time_timesync_config_profile_create(unit,
                                                       profile,
                                                       profile_id));
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
ltsw_time_timesync_config_profile_destroy(
    int unit,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    TIME_INIT(unit);
    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_time_timesync_config_profile_destroy(unit,
                                                        profile_id));
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
ltsw_time_timesync_config_profile_get(
    int unit,
    int profile_id,
    bcm_time_timesync_config_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);

    TIME_INIT(unit);
    TIME_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_time_timesync_config_profile_get(unit,
                                                    profile_id,
                                                    profile));
exit:
    TIME_UNLOCK(unit);
    SHR_FUNC_EXIT();
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
    int warm, rv;
    uint32_t ha_instance_size = 0, ha_array_size = 0;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    void *alloc_ptr = NULL;
    bcm_time_if_t intf_id;

    SHR_FUNC_ENTER(unit);

    /* Cleanup software database first. */
    warm = bcmi_warmboot_get(unit);
    if (!warm && ltsw_time_db[unit].initialized) {
        (void)ltsw_time_interface_delete_all(unit);
    }

    /*! Init time module. */
    SHR_IF_ERR_EXIT
        (ltsw_time_lock_create(unit));
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_time_init(unit));

    /* Ha allocate */
    ha_instance_size = sizeof(bcmint_time_intf_ha_db_t);
    ha_array_size = TIME_INTF_INSTANCE_NUM;
    ha_req_size = ha_instance_size * ha_array_size;
    ha_alloc_size = ha_req_size;
    alloc_ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                       BCMI_HA_COMP_ID_TIME,
                                       BCMINT_TIME_SUB_COMP_ID_INTF,
                                       "bcmTimeIntfInfo",
                                       &ha_alloc_size);
    SHR_NULL_CHECK(alloc_ptr, SHR_E_MEMORY);

    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    if (!warm) {
        sal_memset(alloc_ptr, 0, ha_alloc_size);
    }
    ltsw_time_intf_ha_db[unit] = (bcmint_time_intf_ha_db_t *)alloc_ptr;

    rv = bcmi_ltsw_issu_struct_info_report(unit,
             BCMI_HA_COMP_ID_TIME,
             BCMINT_TIME_SUB_COMP_ID_INTF,
             0, ha_instance_size, ha_array_size,
             BCMINT_TIME_INTF_HA_DB_T_ID);
    if (rv != SHR_E_EXISTS) {
       SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    ltsw_time_db[unit].initialized = TRUE;
    ltsw_time_db[unit].locked = false;

    if (warm) {
        for (intf_id = 0; intf_id < TIME_INTF_INSTANCE_NUM; intf_id++) {
            bcm_time_interface_t *intf;
            intf = &(ltsw_time_intf_db[unit][intf_id].intf);
            rv = ltsw_time_interface_get(unit, intf);
            if (rv == SHR_E_NONE) {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                        "TIME(unit %d) wb[%d]. interface get\n"), unit, warm));
            }
        }

        bcmlt_table_subscribe(unit,
                               DEVICE_TS_BROADSYNC_LOGs,
                               &bcm_ltsw_bs_log_callback,
                               NULL);
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (alloc_ptr && !warm) {
            (void)bcmi_ltsw_ha_mem_free(unit, alloc_ptr);
        }

        (void)ltsw_time_lock_destroy(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bs_deinit(int unit)
{
    int warmboot = false;
    SHR_FUNC_ENTER(unit);

    warmboot = bcmi_warmboot_get(unit);

    if (!warmboot) {
        /* Clear all LTs */
        SHR_IF_ERR_VERBOSE_EXIT(
                bcm_ltsw_bs_clear_tables(unit));
    }

exit:
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

    bcm_ltsw_bs_deinit(unit);

    ltsw_time_db[unit].initialized = FALSE;
    ltsw_time_db[unit].locked = false;

exit:
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

/*
 * Function:
 *      bcm_ltsw_time_bs_log_configure_set
 * Purpose:
 *      Configure Broadsync logging
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_ltsw_time_bs_log_configure_set (int unit,
        bcm_time_bs_log_cfg_t bs_log_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_bs_log_control_set(unit, bs_log_cfg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_time_bs_log_configure_get
 * Purpose:
 *      Retrieve Broadsync logging configuration
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_ltsw_time_bs_log_configure_get (int unit,
        bcm_time_bs_log_cfg_t *bs_log_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_bs_log_control_get(unit, bs_log_cfg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_time_heartbeat_enable_set
 * Purpose:
 *      Configure Broadsync signal
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_ltsw_time_heartbeat_enable_set (int unit,
        bcm_time_if_t id,
        int enable)
{

    SHR_FUNC_ENTER(unit);

    if (!(ltsw_time_intf_db[unit][id].intf.flags & BCM_TIME_SYNC_STAMPER)) {
        return BCM_E_UNAVAIL;
    }

    bcmi_ltsw_bs_hb_enable_set(unit, id, enable);

    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_time_heartbeat_enable_get
 * Purpose:
 *      Retrieve Broadsync signal config
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_ltsw_time_heartbeat_enable_get (int unit,
        bcm_time_if_t id,
        int *enable)
{

    SHR_FUNC_ENTER(unit);

    if (!(ltsw_time_intf_db[unit][id].intf.flags & BCM_TIME_SYNC_STAMPER)) {
        return BCM_E_UNAVAIL;
    }

    bcmi_ltsw_time_hb_enable_get(unit, id, enable);

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create a timesync profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile Profile entry.
 * \param [inout] profile_id Profile id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_time_timesync_config_profile_create(
    int unit,
    bcm_time_timesync_config_profile_t *profile,
    int *profile_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_timesync_config_profile_create(unit, profile, profile_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a timesync profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_id Profile id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_time_timesync_config_profile_destroy(
    int unit,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_timesync_config_profile_destroy(unit, profile_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a timesync profile with given profile id.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_id Profile id.
 * \param [out] profile Profile entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_time_timesync_config_profile_get(
    int unit,
    int profile_id,
    bcm_time_timesync_config_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_time_timesync_config_profile_get(unit, profile_id, profile));
exit:
    SHR_FUNC_EXIT();
}


/*! \file rate.c
 *
 * BCM level APIs for rate.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* SDK6 API header file */
#include <bcm/error.h>
#include <bcm/rate.h>
#include <bcm_int/control.h>

/* Mixed header file */
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/rate_int.h>
#include <bcm_int/ltsw/mbcm/rate.h>

/* SDKLT header file */
#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <sal/sal_assert.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_RATE

/*!
 * \brief Rate API enum.
 */
typedef enum bcmi_rate_api_s {

    /*! Rate API for Broadcast packet. */
    LT_RATE_API_BC = 0,

    /*! Rate API for Multicast packet. */
    LT_RATE_API_MC = 1,

    /*! Rate API for Destination lookup failure packet. */
    LT_RATE_API_DLF = 2,

    /*! Rate API for meter adjust. */
    LT_RATE_API_METER = 3,

    /*! Rate API for shaper adjust. */
    LT_RATE_API_SHAPER = 4,

    /*! Number of Rate APIs. */
    LT_RATE_API_NUM = 5
} bcmi_rate_api_t;

/*!
 * \brief Rate database.
 */
typedef struct ltsw_rate_db_s {

    /*! Rate initialized flag. */
    int         initialized;

    /*! Rate module lock. */
    sal_mutex_t lock;

    /*! Rate API supporting status. */
    bool api_supported[LT_RATE_API_NUM];
} ltsw_rate_db_t;

/*!
 * \brief One rate control entry for each SOC device containing rate
 * book keeping information for that device.
 */
static ltsw_rate_db_t ltsw_rate_db[BCM_MAX_NUM_UNITS];

/*! Cause a routine to return SHR_E_INIT if rateing subsystem is not initialized. */
#define RATE_INIT(unit) \
    do { \
        if (ltsw_rate_db[unit].initialized == FALSE) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while (0)

/*! Rate mutex lock. */
#define RATE_LOCK(unit) \
    do { \
        if (ltsw_rate_db[unit].lock) { \
            sal_mutex_take(ltsw_rate_db[unit].lock, SAL_MUTEX_FOREVER); \
        } \
    } while (0)

/*! Rate mutex unlock. */
#define RATE_UNLOCK(unit) \
    do { \
        if (ltsw_rate_db[unit].lock) { \
            sal_mutex_give(ltsw_rate_db[unit].lock); \
        } \
    } while (0)

/*! Rate API supporting status. */
#define RATE_API_SUPPORT(unit, api) \
    do { \
        if (ltsw_rate_db[unit].api_supported[api] == FALSE) { \
            SHR_ERR_EXIT(SHR_E_UNAVAIL); \
        } \
    } while (0)

/*! Rate Storm Control supporting status. */
#define RATE_API_SC_SUPPORT(unit) \
    do { \
        if (ltsw_rate_db[unit].api_supported[LT_RATE_API_BC] == FALSE && \
            ltsw_rate_db[unit].api_supported[LT_RATE_API_MC] == FALSE && \
            ltsw_rate_db[unit].api_supported[LT_RATE_API_DLF] == FALSE) { \
            SHR_ERR_EXIT(SHR_E_UNAVAIL); \
        } \
    } while (0)

/* Default storm control meter mapping index. */
#define BCMI_LTSW_RATE_BCAST_INDEX           (0)
#define BCMI_LTSW_RATE_MCAST_INDEX           (1)
#define BCMI_LTSW_RATE_UNKNOWN_MCAST_INDEX   (2)
#define BCMI_LTSW_RATE_DLF_INDEX             (3)

#define BCMI_LTSW_RATE_DEFAULT_BURST_PKTS    (0)

/******************************************************************************
 * Private functions
 */

/* init bcmint_rate_info_t */
static void ltsw_rate_info_t_init(bcmint_rate_info_t *info)
{
    /* NULL error handling */
    if (NULL == info) {
        return;
    }
    sal_memset(info, 0, sizeof(bcmint_rate_info_t));
}

/* init bcmint_rate_dev_info_t */
static void ltsw_rate_dev_info_t_init(bcmint_rate_dev_info_t *info)
{
    /* NULL error handling */
    if (NULL == info) {
        return;
    }
    sal_memset(info, 0, sizeof(bcmint_rate_dev_info_t));
}

/*!
 * \brief Create protection mutex for rate module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_rate_lock_create(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_rate_db[unit].lock == NULL) {
        ltsw_rate_db[unit].lock = sal_mutex_create("bcmRateMutex");
        SHR_NULL_CHECK(ltsw_rate_db[unit].lock, SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy protection mutex for rate module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
static void
ltsw_rate_lock_destroy(int unit)
{
    if (ltsw_rate_db[unit].lock != NULL) {
        sal_mutex_destroy(ltsw_rate_db[unit].lock);
        ltsw_rate_db[unit].lock = NULL;
    }
}

/*!
 * \brief Initialize the rate module.
 *
 * Initial rate database data structure and
 * clear the rate related Logical Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_MEMORY No Memory Resource.
 */
static int
ltsw_rate_init(int unit)
{
    bcmint_rate_dev_info_t dev_info;
    int i;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    RATE_LOCK(unit);

    /*! Init Rate module. */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_rate_init(unit));

    (void)ltsw_rate_dev_info_t_init(&dev_info);

    /* Get API supporting status from hardware */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_rate_dev_get(unit, &dev_info));

    ltsw_rate_db[unit].api_supported[LT_RATE_API_BC] =
        dev_info.map_supported[LT_RATE_MAP_BCAST];

    ltsw_rate_db[unit].api_supported[LT_RATE_API_MC] =
        dev_info.map_supported[LT_RATE_MAP_KNOWN_L2MC] |
        dev_info.map_supported[LT_RATE_MAP_KNOWN_IPMC] |
        dev_info.map_supported[LT_RATE_MAP_UNKNOWN_L2MC] |
        dev_info.map_supported[LT_RATE_MAP_UNKNOWN_IPMC];

    ltsw_rate_db[unit].api_supported[LT_RATE_API_DLF] =
        dev_info.map_supported[LT_RATE_MAP_DLF];

    ltsw_rate_db[unit].api_supported[LT_RATE_API_METER] =
        dev_info.meter_adjust;

    ltsw_rate_db[unit].api_supported[LT_RATE_API_SHAPER] =
        dev_info.shaper_adjust;

    /* Check if any meter map index is supported on the device */
    if (!warm) {
        for (i = LT_RATE_MAP_BCAST;
             i < LT_RATE_MAP_NUM;
             i++) {
            if (dev_info.map_supported[i]) {
                /* Set the default storm control meter mapping index. */
                dev_info.map_idx[LT_RATE_MAP_BCAST] =
                    BCMI_LTSW_RATE_BCAST_INDEX;
                dev_info.map_idx[LT_RATE_MAP_KNOWN_L2MC] =
                    BCMI_LTSW_RATE_MCAST_INDEX;
                dev_info.map_idx[LT_RATE_MAP_KNOWN_IPMC] =
                    BCMI_LTSW_RATE_MCAST_INDEX;
                dev_info.map_idx[LT_RATE_MAP_UNKNOWN_L2MC] =
                    BCMI_LTSW_RATE_UNKNOWN_MCAST_INDEX;
                dev_info.map_idx[LT_RATE_MAP_UNKNOWN_IPMC] =
                    BCMI_LTSW_RATE_UNKNOWN_MCAST_INDEX;
                dev_info.map_idx[LT_RATE_MAP_DLF] =
                    BCMI_LTSW_RATE_DLF_INDEX;
                SHR_IF_ERR_EXIT
                    (mbcm_ltsw_rate_dev_set(unit, &dev_info));
                break;
            }
        }
    }

exit:
    RATE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the rate module.
 *
 * Initial rate database data structure and
 * clear the rate related Logical Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_MEMORY No Memory Resource.
 */
static void
ltsw_rate_deinit(int unit)
{
    RATE_LOCK(unit);

    /*! Clear Rate related LT tables. */
    (void)mbcm_ltsw_rate_clear(unit);

    RATE_UNLOCK(unit);
}

/* Check if device supports the flags of storm control. */
static int
ltsw_rate_sc_flag_supported(
    int unit,
    int flags)
{
    SHR_FUNC_ENTER(unit);

    if (flags & BCM_RATE_BCAST) {
        RATE_API_SUPPORT(unit, LT_RATE_API_BC);
    }
    if (flags & (BCM_RATE_MCAST |
                 BCM_RATE_KNOWN_MCAST |
                 BCM_RATE_UNKNOWN_MCAST)) {
        RATE_API_SUPPORT(unit, LT_RATE_API_MC);
    }
    if (flags & BCM_RATE_DLF) {
        RATE_API_SUPPORT(unit, LT_RATE_API_DLF);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the rate configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] rate_info Rate Information.
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_rate_sc_set(
    int unit,
    bcm_port_t port,
    bcmint_rate_info_t *rate_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(rate_info, SHR_E_PARAM);

    RATE_INIT(unit);

    RATE_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_rate_set(unit, port, rate_info));
    RATE_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the rate configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [out] rate_info Rate Information.
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_rate_sc_get(
    int unit,
    bcm_port_t port,
    bcmint_rate_info_t *rate_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(rate_info, SHR_E_PARAM);

    RATE_INIT(unit);

    RATE_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_rate_get(unit, port, rate_info));
    RATE_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the rate configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] rate_info Rate Information.
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_rate_set(
    int unit,
    int port,
    bcmint_rate_info_t *info)
{
    bcmint_rate_dev_info_t dev_info;
    bcm_pbmp_t pbmp;
    bcm_port_t p;

    SHR_FUNC_ENTER(unit);
    RATE_INIT(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    if (!(info->fmask & BCM_RATE_ALL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    (void)ltsw_rate_dev_info_t_init(&dev_info);
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_rate_dev_get(unit, &dev_info));

    /* Loop all Ethernet ports */
    if (info->all_port) {
        SHR_IF_ERR_EXIT(
            bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ETH, &pbmp));
        BCM_PBMP_ITER(pbmp, p) {
            port = p;
            if (bcmi_ltsw_port_gport_validate(unit, port, &port) != SHR_E_NONE) {
                continue;
            }
            if ((info->fmask) & BCM_RATE_BCAST) {
                info->map_idx = dev_info.map_idx[LT_RATE_MAP_BCAST];
                SHR_IF_ERR_EXIT
                    (ltsw_rate_sc_set(unit, port, info));
            }
            if ((info->fmask) & (BCM_RATE_MCAST | BCM_RATE_KNOWN_MCAST)) {
                info->map_idx = dev_info.map_idx[LT_RATE_MAP_KNOWN_L2MC];
                SHR_IF_ERR_EXIT
                    (ltsw_rate_sc_set(unit, port, info));
            }
            if ((info->fmask) & (BCM_RATE_MCAST | BCM_RATE_UNKNOWN_MCAST)) {
                info->map_idx = dev_info.map_idx[LT_RATE_MAP_UNKNOWN_L2MC];
                SHR_IF_ERR_EXIT
                    (ltsw_rate_sc_set(unit, port, info));
            }
            if ((info->fmask) & BCM_RATE_DLF) {
                info->map_idx = dev_info.map_idx[LT_RATE_MAP_DLF];
                SHR_IF_ERR_EXIT
                    (ltsw_rate_sc_set(unit, port, info));
            }
        }
    }else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));

        if ((info->fmask) & BCM_RATE_BCAST) {
            info->map_idx = dev_info.map_idx[LT_RATE_MAP_BCAST];
            SHR_IF_ERR_EXIT
                (ltsw_rate_sc_set(unit, port, info));
        }
        if ((info->fmask) & (BCM_RATE_MCAST | BCM_RATE_KNOWN_MCAST)) {
            info->map_idx = dev_info.map_idx[LT_RATE_MAP_KNOWN_L2MC];
            SHR_IF_ERR_EXIT
                (ltsw_rate_sc_set(unit, port, info));
        }
        if ((info->fmask) & (BCM_RATE_MCAST | BCM_RATE_UNKNOWN_MCAST)) {
            info->map_idx = dev_info.map_idx[LT_RATE_MAP_UNKNOWN_L2MC];
            SHR_IF_ERR_EXIT
                (ltsw_rate_sc_set(unit, port, info));
        }
        if ((info->fmask) & BCM_RATE_DLF) {
            info->map_idx = dev_info.map_idx[LT_RATE_MAP_DLF];
            SHR_IF_ERR_EXIT
                (ltsw_rate_sc_set(unit, port, info));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the rate configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [out] rate_info Rate Information.
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_rate_get(
    int unit, int port, bcmint_rate_info_t *info)
{
    bcmint_rate_dev_info_t dev_info;
    bcm_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    RATE_INIT(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    if (!(info->fmask & BCM_RATE_ALL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    (void)ltsw_rate_dev_info_t_init(&dev_info);
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_rate_dev_get(unit, &dev_info));

    /* Get the 1st enabled Ethernet port */
    if (info->all_port) {
        SHR_IF_ERR_EXIT(
            bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ETH, &pbmp));
        BCM_PBMP_ITER(pbmp, port) {
            break;
        }
    }

    if ((info->fmask) & BCM_RATE_BCAST) {
        info->map_idx = dev_info.map_idx[LT_RATE_MAP_BCAST];
        SHR_IF_ERR_EXIT
            (ltsw_rate_sc_get(unit, port, info));
        if (info->flags && (info->meter_kbps || info->meter_pps)) SHR_EXIT();
    }
    if ((info->fmask) & (BCM_RATE_MCAST | BCM_RATE_KNOWN_MCAST)) {
        info->map_idx = dev_info.map_idx[LT_RATE_MAP_KNOWN_L2MC];
        SHR_IF_ERR_EXIT
            (ltsw_rate_sc_get(unit, port, info));
        if (info->flags && (info->meter_kbps || info->meter_pps)) SHR_EXIT();
    }
    if ((info->fmask) & (BCM_RATE_MCAST | BCM_RATE_UNKNOWN_MCAST)) {
        info->map_idx = dev_info.map_idx[LT_RATE_MAP_UNKNOWN_L2MC];
        SHR_IF_ERR_EXIT
            (ltsw_rate_sc_get(unit, port, info));
        if (info->flags && (info->meter_kbps || info->meter_pps)) SHR_EXIT();
    }
    if ((info->fmask) & BCM_RATE_DLF) {
        info->map_idx = dev_info.map_idx[LT_RATE_MAP_DLF];
        SHR_IF_ERR_EXIT
            (ltsw_rate_sc_get(unit, port, info));
        if (info->flags && (info->meter_kbps || info->meter_pps)) SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_rate_type_set(int unit, bcm_rate_limit_t *rl)
{
    bcmint_rate_info_t info;

    SHR_FUNC_ENTER(unit);
    RATE_INIT(unit);

    SHR_NULL_CHECK(rl, SHR_E_PARAM);

    /* set BCAST */
    (void)ltsw_rate_info_t_init(&info);
    info.byte_mode = FALSE;
    /* rate == 0 means to disable storm control. */
    info.flags = (rl->br_bcast_rate == BCM_RATE_DISABLE) ? 0 : rl->flags;
    info.fmask = BCM_RATE_BCAST;
    info.meter_pps = rl->br_bcast_rate;
    info.burst_pkts = BCMI_LTSW_RATE_DEFAULT_BURST_PKTS;
    info.all_port = TRUE;
    SHR_IF_ERR_EXIT
        (ltsw_rate_set(unit, 0, &info));

    /* set MCAST */
    (void)ltsw_rate_info_t_init(&info);
    info.byte_mode = FALSE;
    /* rate == 0 means to disable storm control. */
    info.flags = (rl->br_mcast_rate == BCM_RATE_DISABLE) ? 0 : rl->flags;
    info.fmask =
        BCM_RATE_MCAST | BCM_RATE_KNOWN_MCAST | BCM_RATE_UNKNOWN_MCAST;
    info.meter_pps = rl->br_mcast_rate;
    info.burst_pkts = BCMI_LTSW_RATE_DEFAULT_BURST_PKTS;
    info.all_port = TRUE;
    SHR_IF_ERR_EXIT
        (ltsw_rate_set(unit, 0, &info));

    /* set DLF */
    (void)ltsw_rate_info_t_init(&info);
    info.byte_mode = FALSE;
    /* rate == 0 means to disable storm control. */
    info.flags = (rl->br_dlfbc_rate == BCM_RATE_DISABLE) ? 0 : rl->flags;
    info.fmask = BCM_RATE_DLF;
    info.meter_pps = rl->br_dlfbc_rate;
    info.burst_pkts = BCMI_LTSW_RATE_DEFAULT_BURST_PKTS;
    info.all_port = TRUE;
    SHR_IF_ERR_EXIT
        (ltsw_rate_set(unit, 0, &info));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_rate_type_get(int unit, bcm_rate_limit_t *rl)
{
    bcmint_rate_info_t info;

    SHR_FUNC_ENTER(unit);
    RATE_INIT(unit);

    SHR_NULL_CHECK(rl, SHR_E_PARAM);

    bcm_rate_limit_t_init(rl);

    /* Get BCAST */
    (void)ltsw_rate_info_t_init(&info);
    info.byte_mode = FALSE;
    info.fmask = BCM_RATE_BCAST;
    info.all_port = TRUE;
    SHR_IF_ERR_EXIT
        (ltsw_rate_get(unit, 0, &info));
    rl->br_bcast_rate = info.meter_pps;
    rl->flags |= info.flags;

    /* Get MCAST */
    (void)ltsw_rate_info_t_init(&info);
    info.byte_mode = FALSE;
    info.fmask =
        BCM_RATE_MCAST | BCM_RATE_KNOWN_MCAST | BCM_RATE_UNKNOWN_MCAST;
    info.all_port = TRUE;
    SHR_IF_ERR_EXIT
        (ltsw_rate_get(unit, 0, &info));
    rl->br_mcast_rate = info.meter_pps;
    rl->flags |= info.flags;

    /* Get DLF */
    (void)ltsw_rate_info_t_init(&info);
    info.byte_mode = FALSE;
    info.fmask = BCM_RATE_DLF;
    info.all_port = TRUE;
    SHR_IF_ERR_EXIT
        (ltsw_rate_get(unit, 0, &info));
    rl->br_dlfbc_rate = info.meter_pps;
    rl->flags |= info.flags;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_rate_meter_adjust_set(
    int unit,
    bcm_port_t port,
    int value)
{
    SHR_FUNC_ENTER(unit);
    RATE_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    RATE_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_rate_meter_adjust_set(unit, port, value));
    RATE_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_rate_meter_adjust_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    SHR_FUNC_ENTER(unit);
    RATE_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    RATE_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_rate_meter_adjust_get(unit, port, value));
    RATE_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_rate_shaper_adjust_set(
    int unit,
    bcm_port_t port,
    int value)
{
    SHR_FUNC_ENTER(unit);
    RATE_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    RATE_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_rate_shaper_adjust_set(unit, port, value));
    RATE_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_rate_shaper_adjust_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    SHR_FUNC_ENTER(unit);
    RATE_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    RATE_LOCK(unit);
    SHR_IF_ERR_CONT
        (mbcm_ltsw_rate_shaper_adjust_get(unit, port, value));
    RATE_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Initialize the rate module.
 *
 * Initial rate database data structure and
 * clear the rate related Logical Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_rate_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ltsw_rate_lock_create(unit));

    SHR_IF_ERR_EXIT
        (ltsw_rate_init(unit));

    ltsw_rate_db[unit].initialized = TRUE;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the RATE module.
 *
 * This function is used to de-initialize the RATE module.
 *
 * \param [in]  unit      Unit Number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcmi_ltsw_rate_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_rate_db[unit].initialized == TRUE) {
        (void)ltsw_rate_deinit(unit);

        (void)ltsw_rate_lock_destroy(unit);
    }
    ltsw_rate_db[unit].initialized = FALSE;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Disable Rate limit on the port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_rate_clear_per_port(
    int unit,
    bcm_port_t port)
{
    bcmint_rate_info_t info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    (void)ltsw_rate_info_t_init(&info);
    /* Disable rate limiting. */
    info.flags = 0;
    info.fmask = BCM_RATE_BCAST | BCM_RATE_MCAST | BCM_RATE_DLF;
    info.byte_mode = TRUE;
    info.meter_kbps = 0;
    info.burst_kbits = 0;
    (void)ltsw_rate_set(unit, port, &info);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set Meter/Rate control with overhead on wire accounted for per packet.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] value Number of bytes to adjust.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_rate_meter_adjust_set(
    int unit,
    bcm_port_t port,
    int value)
{
    SHR_FUNC_ENTER(unit);
    RATE_API_SUPPORT(unit, LT_RATE_API_METER);
    SHR_IF_ERR_EXIT
        (ltsw_rate_meter_adjust_set(unit, port, value));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get Meter/Rate control with overhead on wire accounted for per packet.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] value Number of bytes to adjust.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_rate_meter_adjust_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    SHR_FUNC_ENTER(unit);
    RATE_API_SUPPORT(unit, LT_RATE_API_METER);
    SHR_IF_ERR_EXIT
        (ltsw_rate_meter_adjust_get(unit, port, value));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set Meter/Rate control with overhead on wire accounted for per packet for EFP.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] value Number of bytes to adjust.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_rate_shaper_adjust_set(
    int unit,
    bcm_port_t port,
    int value)
{
    SHR_FUNC_ENTER(unit);
    RATE_API_SUPPORT(unit, LT_RATE_API_SHAPER);
    SHR_IF_ERR_EXIT
        (ltsw_rate_shaper_adjust_set(unit, port, value));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get Meter/Rate control with overhead on wire accounted for per packet for EFP.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] value Number of bytes to adjust.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_rate_shaper_adjust_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    SHR_FUNC_ENTER(unit);
    RATE_API_SUPPORT(unit, LT_RATE_API_SHAPER);
    SHR_IF_ERR_EXIT
        (ltsw_rate_shaper_adjust_get(unit, port, value));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Front end to bcm_*cast_rate_set functions.
 *
 * Uses a single data structure to write into
 * all the 3 rate control registers.
 *
 * \param [in]  unit      Unit Number.
 * \param [in]  rl        Data structure containing info to be
 *                        written to the rate control registers.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_rate_type_set(int unit, bcm_rate_limit_t *rl)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(rl, SHR_E_PARAM);
    RATE_API_SC_SUPPORT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_rate_sc_flag_supported(unit, rl->flags));

    SHR_IF_ERR_EXIT
        (ltsw_rate_type_set(unit, rl));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Front end to bcm_*cast_rate_get functions.
 *
 * Uses a single data structure to read from
 * all the 3 rate control registers.
 *
 * \param [in]  unit      Unit Number.
 * \param [out] rl        data structure containing info to be
 *                        acquired from the rate control registers.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_rate_type_get(int unit, bcm_rate_limit_t *rl)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(rl, SHR_E_PARAM);
    RATE_API_SC_SUPPORT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_rate_sc_flag_supported(unit, rl->flags));

    SHR_IF_ERR_EXIT
        (ltsw_rate_type_get(unit, rl));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure rate limit.
 *
 * Configure rate limit and on/off state of
 * DLF, MCAST, and BCAST limiting
 *
 * \param [in] unit       Unit Number.
 * \param [in] pps        Rate limit value in packets/second.
 * \param [in] flags      Bitmask with one or more of BCM_RATE_*.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_rate_set(int unit, int pps, int flags)
{
    bcmint_rate_info_t info;

    SHR_FUNC_ENTER(unit);
    RATE_API_SC_SUPPORT(unit);

    (void)ltsw_rate_info_t_init(&info);
    /* rate == 0 means to disable storm control. */
    info.flags = (pps == BCM_RATE_DISABLE) ? 0 : flags;
    info.fmask = BCM_RATE_BCAST | BCM_RATE_MCAST | BCM_RATE_DLF;
    info.byte_mode = FALSE;
    info.meter_pps = pps;
    info.burst_pkts = BCMI_LTSW_RATE_DEFAULT_BURST_PKTS;
    info.all_port = TRUE;

    SHR_IF_ERR_EXIT
        (ltsw_rate_set(unit, 0, &info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get rate limit.
 *
 * Get rate limit and on/off state of
 * DLF, MCAST, and BCAST limiting
 * Actually returns the rate for BCAST only, but assumes the
 * bcm_rate_set call was used so DLF, MCAST and BCAST should be equal.
 *
 * \param [in] unit       Unit Number.
 * \param [out] pps       Rate limit value in packets/second.
 * \param [out] flags     Bitmask with one or more of BCM_RATE_*.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcm_ltsw_rate_get(int unit, int *pps, int *flags)
{
    bcmint_rate_info_t info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pps, SHR_E_PARAM);
    SHR_NULL_CHECK(flags, SHR_E_PARAM);
    RATE_API_SC_SUPPORT(unit);

    (void)ltsw_rate_info_t_init(&info);
    info.fmask = BCM_RATE_BCAST | BCM_RATE_MCAST | BCM_RATE_DLF;
    info.byte_mode = FALSE;
    info.all_port = TRUE;
    SHR_IF_ERR_EXIT
        (ltsw_rate_get(unit, 0, &info));

    *flags = info.flags;
    *pps = info.meter_pps;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure rate limit for MCAST packets.
 *
 * \param [in] unit         Unit Number.
 * \param [in] port         Port number.
 * \param [in] pps          Rate limit value in packets/second.
 * \param [in] flags        Bitmask with one or more of BCM_RATE_*.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
int
bcm_ltsw_rate_mcast_set(int unit, int pps, int flags, int port)
{
    bcmint_rate_info_t info;

    SHR_FUNC_ENTER(unit);

    RATE_API_SUPPORT(unit, LT_RATE_API_MC);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_rate_sc_flag_supported(unit, flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    (void)ltsw_rate_info_t_init(&info);
    /* rate == 0 means to disable storm control. */
    info.flags = (pps == BCM_RATE_DISABLE) ? 0 : flags;
    info.fmask = BCM_RATE_MCAST;
    info.byte_mode = FALSE;
    info.meter_pps = (flags == 0) ? 0 : pps;
    info.burst_pkts = BCMI_LTSW_RATE_DEFAULT_BURST_PKTS;
    info.all_port = FALSE;

    if (info.fmask & flags || flags == 0) {
        SHR_IF_ERR_EXIT
            (ltsw_rate_set(unit, port, &info));
    }

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Configure rate limit for BCAST packets.
 *
 * \param [in] unit         Unit Number.
 * \param [in] port         Port number.
 * \param [in] pps          Rate limit value in packets/second.
 * \param [in] flags        Bitmask with one or more of BCM_RATE_*.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
int
bcm_ltsw_rate_bcast_set(int unit, int pps, int flags, int port)
{
    bcmint_rate_info_t info;

    SHR_FUNC_ENTER(unit);

    RATE_API_SUPPORT(unit, LT_RATE_API_BC);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_rate_sc_flag_supported(unit, flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    (void)ltsw_rate_info_t_init(&info);
    /* rate == 0 means to disable storm control. */
    info.flags = (pps == BCM_RATE_DISABLE) ? 0 : flags;
    info.fmask = BCM_RATE_BCAST;
    info.byte_mode = FALSE;
    info.meter_pps = (flags == 0) ? 0 : pps;
    info.burst_pkts = BCMI_LTSW_RATE_DEFAULT_BURST_PKTS;
    info.all_port = FALSE;

    if (info.fmask & flags || flags == 0) {
        SHR_IF_ERR_EXIT
            (ltsw_rate_set(unit, port, &info));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure rate limit for DLFBC packets.
 *
 * DLFBC: Destination Lookup Failure Broadcast
 *
 * \param [in] unit         Unit Number.
 * \param [in] port         Port number.
 * \param [in] pps          Rate limit value in packets/second.
 * \param [in] flags        Bitmask with one or more of BCM_RATE_*.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
int
bcm_ltsw_rate_dlfbc_set(int unit, int pps, int flags, int port)
{
    bcmint_rate_info_t info;

    SHR_FUNC_ENTER(unit);

    RATE_API_SUPPORT(unit, LT_RATE_API_DLF);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_rate_sc_flag_supported(unit, flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    (void)ltsw_rate_info_t_init(&info);
    /* rate == 0 means to disable storm control. */
    info.flags = (pps == BCM_RATE_DISABLE) ? 0 : flags;
    info.fmask = BCM_RATE_DLF;
    info.byte_mode = FALSE;
    info.meter_pps = (flags == 0) ? 0 : pps;
    info.burst_pkts = BCMI_LTSW_RATE_DEFAULT_BURST_PKTS;
    info.all_port = FALSE;

    if (info.fmask & flags || flags == 0) {
        SHR_IF_ERR_EXIT
            (ltsw_rate_set(unit, port, &info));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get rate limit for MCAST packets.
 *
 * \param [in] unit         Unit Number.
 * \param [in] port         Port number.
 * \param [out] pps         Rate limit value in packets/second.
 * \param [out] flags       Bitmask with one or more of BCM_RATE_*.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
int
bcm_ltsw_rate_mcast_get(int unit, int *pps, int *flags, int port)
{
    bcmint_rate_info_t info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pps, SHR_E_PARAM);
    SHR_NULL_CHECK(flags, SHR_E_PARAM);

    RATE_API_SUPPORT(unit, LT_RATE_API_MC);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    (void)ltsw_rate_info_t_init(&info);
    info.fmask = BCM_RATE_MCAST;
    info.byte_mode = FALSE;
    info.all_port = FALSE;
    SHR_IF_ERR_EXIT
        (ltsw_rate_get(unit, port, &info));

    *flags = info.flags;
    *pps = info.meter_pps;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get rate limit for BCAST packets.
 *
 * \param [in] unit         Unit Number.
 * \param [in] port         Port number.
 * \param [out] pps         Rate limit value in packets/second.
 * \param [out] flags       Bitmask with one or more of BCM_RATE_*.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
int
bcm_ltsw_rate_bcast_get(int unit, int *pps, int *flags, int port)
{
    bcmint_rate_info_t info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pps, SHR_E_PARAM);
    SHR_NULL_CHECK(flags, SHR_E_PARAM);

    RATE_API_SUPPORT(unit, LT_RATE_API_BC);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    (void)ltsw_rate_info_t_init(&info);
    info.fmask = BCM_RATE_BCAST;
    info.byte_mode = FALSE;
    info.all_port = FALSE;
    SHR_IF_ERR_EXIT
        (ltsw_rate_get(unit, port, &info));

    *flags = info.flags;
    *pps = info.meter_pps;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get rate limit for DLFBC packets.
 *
 * DLFBC: Destination Lookup Failure Broadcast
 *
 * \param [in] unit         Unit Number.
 * \param [in] port         Port number.
 * \param [out] pps         Rate limit value in packets/second.
 * \param [out] flags       Bitmask with one or more of BCM_RATE_*.
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
int
bcm_ltsw_rate_dlfbc_get(int unit, int *pps, int *flags, int port)
{
    bcmint_rate_info_t info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pps, SHR_E_PARAM);
    SHR_NULL_CHECK(flags, SHR_E_PARAM);

    RATE_API_SUPPORT(unit, LT_RATE_API_DLF);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    (void)ltsw_rate_info_t_init(&info);
    info.fmask = BCM_RATE_DLF;
    info.byte_mode = FALSE;
    info.all_port = FALSE;
    SHR_IF_ERR_EXIT
        (ltsw_rate_get(unit, port, &info));

    *flags = info.flags;
    *pps = info.meter_pps;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set rate bandwidth limiting parameters.
 *
 * \param [in] unit         Unit Number.
 * \param [in] port         Port number.
 * \param [in] flags        Bitmask with one of the following:
 *                            BCM_RATE_BCAST
 *                            BCM_RATE_MCAST
 *                            BCM_RATE_DLF
 * \param [in] meter_kbps   Rate in kilobits (1000 bits) per second.
 *                            Rate of 0 disables rate limiting.
 * \param [in] burst_kbits  Maximum burst size in kilobits(1000 bits).
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
int
bcm_ltsw_rate_bandwidth_set(
    int unit,
    bcm_port_t port,
    int flags,
    uint32 meter_kbps,
    uint32 burst_kbits)
{
    bcmint_rate_info_t info;

    SHR_FUNC_ENTER(unit);
    RATE_API_SC_SUPPORT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_rate_sc_flag_supported(unit, flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (!(flags & BCM_RATE_ALL)) {
        return BCM_E_PARAM;
    }

    (void)ltsw_rate_info_t_init(&info);
    /* rate == 0 means to disable storm control. */
    info.flags = (meter_kbps == BCM_RATE_DISABLE) ? 0 : flags;

    /* flags is input, hence only set pkt types defined in flags. */
    info.fmask = flags;
    info.byte_mode = (flags & BCM_RATE_MODE_PACKETS) ? FALSE : TRUE;
    info.all_port = FALSE;

    /* Block packets. */
    if (meter_kbps == BCM_RATE_BLOCK) {
        meter_kbps = 0;
        burst_kbits = 0;
    }

    if (info.byte_mode) {
        info.meter_kbps = meter_kbps;
        info.burst_kbits = burst_kbits;
    } else {
        info.meter_pps = meter_kbps;
        info.burst_pkts = burst_kbits;
    }

    SHR_IF_ERR_EXIT
        (ltsw_rate_set(unit, port, &info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get rate bandwidth limiting parameters.
 *
 * \param [in] unit         Unit Number.
 * \param [in] port         Port number.
 * \param [in] flags        Bitmask with one of the following:
 *                            BCM_RATE_BCAST
 *                            BCM_RATE_MCAST
 *                            BCM_RATE_DLF
 * \param [out] meter_kbps  Rate in kilobits (1000 bits) per second.
 *                            Rate of 0 disables rate limiting.
 * \param [out] burst_kbits Maximum burst size in kilobits(1000 bits).
 *
 * \retval SHR_E_NONE       No errors.
 * \retval !SHR_E_NONE      Failure.
 */
int
bcm_ltsw_rate_bandwidth_get(
    int unit,
    bcm_port_t port,
    int flags,
    uint32 *meter_kbps,
    uint32 *burst_kbits)
{
    bcmint_rate_info_t info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(meter_kbps, SHR_E_PARAM);
    SHR_NULL_CHECK(burst_kbits, SHR_E_PARAM);
    RATE_API_SC_SUPPORT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_rate_sc_flag_supported(unit, flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (!(flags & BCM_RATE_ALL)) {
        return BCM_E_PARAM;
    }

    (void)ltsw_rate_info_t_init(&info);
    info.flags = flags;
    /* flags is input, hence only get pkt types defined in flags. */
    info.fmask = flags;
    info.byte_mode = (flags & BCM_RATE_MODE_PACKETS) ? FALSE : TRUE;
    info.all_port = FALSE;
    SHR_IF_ERR_EXIT
        (ltsw_rate_get(unit, port, &info));

    if (info.byte_mode) {
        *meter_kbps = info.meter_kbps;
        *burst_kbits = info.burst_kbits;
    } else {
        *meter_kbps = info.meter_pps;
        *burst_kbits = info.burst_pkts;
    }

exit:
    SHR_FUNC_EXIT();
}


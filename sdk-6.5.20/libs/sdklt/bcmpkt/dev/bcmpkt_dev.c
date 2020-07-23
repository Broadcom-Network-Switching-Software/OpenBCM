/*! \file bcmpkt_dev.c
 *
 * Packet device API functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd.h>

#define BSL_LOG_MODULE          BSL_LS_BCMPKT_DEV

/*! This sequence should be same as bcmdrd_cm_dev_type_t */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    {#_bd, BCMDRD_DEV_T_##_bd},
static const shr_enum_map_t device_types[] = {
    {"device_none", BCMDRD_DEV_T_NONE},
#include <bcmdrd/bcmdrd_devlist.h>
    {"device_count", BCMDRD_DEV_T_COUNT}
};

/*! This sequence should be same as bcmdrd_cm_dev_type_t */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    {#_bd, _dv},
static const shr_enum_map_t device_ids[] = {
    {"device_none", 0},
#include <bcmdrd/bcmdrd_devlist.h>
    {"device_count", 0xffff}
};

static bcmpkt_dev_t dvect[BCMPKT_DEV_DRV_T_COUNT];
/* Per unit driver handler */
static bcmpkt_dev_t *ddrv[BCMDRD_CONFIG_MAX_UNITS];
/* Per unit device type save. */
static uint32_t dev_types[BCMDRD_CONFIG_MAX_UNITS];

static bcmpkt_rcpu_hdr_t rcpu_hdrs[BCMDRD_CONFIG_MAX_UNITS];

static int
bcmpkt_dev_drv_get(int unit, bcmpkt_dev_t **dev)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    *dev = ddrv[unit];

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_dev_knet_mode_update(int unit)
{
    bool enable = false;

    SHR_FUNC_ENTER(unit);

    if (ddrv[unit]->driver_type == BCMPKT_DEV_DRV_T_KNET) {
        enable = true;
    }
    SHR_IF_ERR_EXIT
        (bcmbd_knet_enable_set(unit, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_type_get(int unit, bcmdrd_dev_type_t *type)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    SHR_NULL_CHECK(type, SHR_E_PARAM);

    if (!dev_types[unit]) {
        int i;
        const char *dev_name = bcmdrd_dev_type_str(unit);

        for (i = BCMDRD_DEV_T_NONE + 1; i < BCMDRD_DEV_T_COUNT; i++) {
            if (!sal_strcasecmp(dev_name, device_types[i].name)) {
                dev_types[unit] = device_types[i].val;
                break;
            }
        }
        if (!dev_types[unit]) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }
    *type = dev_types[unit];

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_id_get(int unit, uint32_t *id)
{
    int dev_type;
    const char *dev_name;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    SHR_NULL_CHECK(id, SHR_E_PARAM);

    dev_name = bcmdrd_dev_type_str(unit);
    SHR_NULL_CHECK(id, SHR_E_NOT_FOUND);

    for (dev_type = BCMDRD_DEV_T_NONE + 1; dev_type < BCMDRD_DEV_T_COUNT;
         dev_type++) {
        if (!sal_strcasecmp(dev_name, device_ids[dev_type].name)) {
            *id = device_ids[dev_type].val;
            SHR_EXIT();
        }
    }

    LOG_ERROR(BSL_LS_BCMPKT_DEV,
              (BSL_META_U(unit, "Not found device %s\n"), dev_name));

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_drv_register(bcmpkt_dev_t *dev_drv)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(dev_drv, SHR_E_PARAM);

    if (dev_drv->driver_type >= BCMPKT_DEV_DRV_T_COUNT ||
        dev_drv->driver_type <= BCMPKT_DEV_DRV_T_AUTO) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (dev_drv->initialized != 1) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if (dvect[dev_drv->driver_type].initialized) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    dvect[dev_drv->driver_type] = *dev_drv;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_drv_unregister(bcmpkt_dev_drv_types_t type)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (type >= BCMPKT_DEV_DRV_T_COUNT ||
        type <= BCMPKT_DEV_DRV_T_AUTO) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (bcmpkt_dev_drv_inuse(type)) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    if (dvect[type].initialized) {
        sal_memset(&dvect[type], 0, sizeof(bcmpkt_dev_t));
    }

exit:
    SHR_FUNC_EXIT();
}

bool
bcmpkt_dev_drv_attached(int unit)
{
    if (!bcmdrd_dev_exists(unit)) {
        return false;
    }

    if (ddrv[unit] != NULL) {
        return true;
    }

    return false;
}

int
bcmpkt_dev_drv_attach(int unit, bcmpkt_dev_drv_types_t type)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (ddrv[unit] != NULL) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    switch (type) {
    case BCMPKT_DEV_DRV_T_AUTO:
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_KNET) &&
            dvect[BCMPKT_DEV_DRV_T_KNET].initialized) {
            ddrv[unit] = &dvect[BCMPKT_DEV_DRV_T_KNET];
            break;
        }

        if (dvect[BCMPKT_DEV_DRV_T_UNET].initialized) {
            ddrv[unit] = &dvect[BCMPKT_DEV_DRV_T_UNET];
            break;
        }

        SHR_ERR_EXIT(SHR_E_FAIL);
    case BCMPKT_DEV_DRV_T_KNET:
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_KNET) &&
            dvect[type].initialized) {
            ddrv[unit] = &dvect[type];
            break;
        }
        SHR_ERR_EXIT(SHR_E_FAIL);
    case BCMPKT_DEV_DRV_T_UNET:
        if (dvect[type].initialized) {
            ddrv[unit] = &dvect[type];
            break;
        }
        SHR_ERR_EXIT(SHR_E_FAIL);
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_knet_mode_update(unit));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Attach device driver succeed\n")));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_drv_type_get(int unit, bcmpkt_dev_drv_types_t *type)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(type, SHR_E_PARAM);

    *type = BCMPKT_DEV_DRV_T_NONE;
    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));

    if(dev) {
        *type = dev->driver_type;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_drv_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (ddrv[unit] != NULL) {
        ddrv[unit] = NULL;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Detach device driver succeed\n")));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dma_chan_set(int unit, bcmpkt_dma_chan_t *chan)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);

    SHR_NULL_CHECK(dev->dma_chan_ops.set, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->dma_chan_ops.set(unit, chan));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dma_chan_get(int unit, int chan_id, bcmpkt_dma_chan_t *chan)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);

    SHR_NULL_CHECK(dev->dma_chan_ops.get, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->dma_chan_ops.get(unit, chan_id, chan));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dma_chan_get_list(int unit, uint32_t size, bcmpkt_dma_chan_t *chans,
                         uint32_t *num_elements)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);

    SHR_NULL_CHECK(dev->dma_chan_ops.get_list, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->dma_chan_ops.get_list(unit, size, chans, num_elements));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_init(int unit, const bcmpkt_dev_init_t *init)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);

    SHR_NULL_CHECK(dev->init, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->init(unit, init));

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_knet_mode_update(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_initialized(int unit, bool *initialized)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(initialized, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    *initialized = false;
    if (dev != NULL && dev->dev_initialized) {
        SHR_IF_ERR_EXIT
            (dev->dev_initialized(unit, initialized));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_cleanup(int unit)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);

    SHR_NULL_CHECK(dev->cleanup, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->cleanup(unit));

    SHR_IF_ERR_EXIT
        (bcmbd_knet_enable_set(unit, false));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_enable(int unit)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(dev->enable, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->enable(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_disable(int unit)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(dev->disable, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->disable(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_enabled(int unit, bool *enabled)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(enabled, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    *enabled = false;
    if (dev != NULL && dev->enabled) {
        SHR_IF_ERR_EXIT
            (dev->enabled(unit, enabled));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_info_dump(int unit, shr_pb_t *pb)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(dev->info_dump, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->info_dump(unit, pb));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_stats_get(int unit, bcmpkt_dev_stat_t *stats)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(dev->stats_get, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->stats_get(unit, stats));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_stats_dump(int unit, shr_pb_t *pb)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(dev->stats_dump, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->stats_dump(unit, pb));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_stats_clear(int unit)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(dev->stats_clear, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (dev->stats_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rcpu_hdr_set(int unit, bcmpkt_rcpu_hdr_t *hdr)
{
    bcmpkt_dev_t *dev;
    bcmpkt_dev_drv_types_t drv_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(hdr, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_type_get(unit, &drv_type));

    if (drv_type == BCMPKT_DEV_DRV_T_KNET) {
        SHR_IF_ERR_EXIT
            (bcmpkt_dev_drv_get(unit, &dev));
        SHR_NULL_CHECK(dev, SHR_E_CONFIG);
        SHR_NULL_CHECK(dev->rcpu_hdr_set, SHR_E_CONFIG);
        SHR_IF_ERR_EXIT
            (dev->rcpu_hdr_set(unit, hdr));
    }

    rcpu_hdrs[unit] = *hdr;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rcpu_hdr_get(int unit, bcmpkt_rcpu_hdr_t *hdr)
{
    bcmpkt_dev_t *dev;
    bcmpkt_dev_drv_types_t drv_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(hdr, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_type_get(unit, &drv_type));

    if (drv_type == BCMPKT_DEV_DRV_T_KNET) {
        SHR_IF_ERR_EXIT
            (bcmpkt_dev_drv_get(unit, &dev));
        SHR_NULL_CHECK(dev, SHR_E_CONFIG);
        SHR_NULL_CHECK(dev->rcpu_hdr_get, SHR_E_CONFIG);
        SHR_IF_ERR_EXIT
            (dev->rcpu_hdr_get(unit, hdr));

        if (sal_memcmp(hdr, &rcpu_hdrs[unit], sizeof(bcmpkt_rcpu_hdr_t))) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "RCPU configuration not sync\n")));
        }
    } else {
        *hdr = rcpu_hdrs[unit];
    }

exit:
    SHR_FUNC_EXIT();
}

/* API internal functions. */
/* Internal use only.
 * This function could be used by SOCKET or UNET.
 */
bcmpkt_rcpu_hdr_t *
bcmpkt_rcpu_hdr(int unit)
{
    if (bcmdrd_dev_exists(unit)) {
        return &rcpu_hdrs[unit];
    }
    return NULL;
}

int
bcmpkt_dev_drv_inuse(bcmpkt_dev_drv_types_t type)
{
    int i = 0;

    if (type < BCMPKT_DEV_DRV_T_COUNT &&
        type > BCMPKT_DEV_DRV_T_AUTO) {
        for (i = 0; i < BCMDRD_CONFIG_MAX_UNITS; i++) {
            if (ddrv[i] == &dvect[type]) {
                return 1;
            }
        }
    }

    return 0;
}

int
bcmpkt_rx_rate_limit_set(int unit, int rate)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(dev->rx_rate_limit_ops.set, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT
        (dev->rx_rate_limit_ops.set(unit, rate));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rx_rate_limit_get(int unit, int *rate)
{
    bcmpkt_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_CONFIG);
    SHR_NULL_CHECK(rate, SHR_E_PARAM);
    SHR_NULL_CHECK(dev->rx_rate_limit_ops.get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT
        (dev->rx_rate_limit_ops.get(unit, rate));

exit:
    SHR_FUNC_EXIT();
}


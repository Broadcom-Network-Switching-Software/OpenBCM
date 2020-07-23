/*! \file bcmlu_ngknet.c
 *
 * NGKNET user interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <bcmcnet/bcmcnet_core.h>
#include <lkm/ngknet_dev.h>
#include <lkm/ngknet_ioctl.h>
#include <bcmlu/bcmlu_ngknet.h>

struct bkn_ctrl {
    int unit;
    char *name;
    int iofd;
    int inited;
};

static struct bkn_ctrl bkn_ctrl[BCMDRD_CONFIG_MAX_UNITS];
static int devnum = sizeof(bkn_ctrl) / sizeof(bkn_ctrl[0]);
static int devfd = -1;
static const char *devname = "/dev/" NGKNET_MODULE_NAME;
static const char *modname = NGKNET_MODULE_NAME ".ko";

static int
ngknet_dev_open(void)
{
    if (devfd >= 0) {
        return SHR_E_NONE;
    }

    if ((devfd = open(devname, O_RDWR | O_SYNC)) >= 0) {
        return SHR_E_NONE;
    }

    return SHR_E_FAIL;
}

static int
ngknet_dev_load(void)
{
    char cmd[128];

    snprintf(cmd, sizeof(cmd),
             "dev=%s; if [ ! -e $dev ]; then mknod $dev c %d 0; fi",
             devname, NGKNET_MODULE_MAJOR);
    if (system(cmd) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("Unable to create device node %s\n"), devname));
        return SHR_E_FAIL;
    }

    if (SHR_SUCCESS(ngknet_dev_open())) {
        return SHR_E_NONE;
    }

    if (access(modname, F_OK) == F_OK) {
        snprintf(cmd, sizeof(cmd), "/sbin/insmod %s", modname);
        if (system(cmd) < 0) {
            LOG_ERROR(BSL_LS_SYS_DMA,
                      (BSL_META("Unable to load kernel module %s\n"), modname));
            return SHR_E_FAIL;
        }

        if (SHR_SUCCESS(ngknet_dev_open())) {
            return SHR_E_NONE;
        }
    }

    LOG_INFO(BSL_LS_SYS_DMA,
             (BSL_META("Unable to open KNET device\n")));

    return SHR_E_UNAVAIL;
}

static int
ngknet_dev_close(void)
{
    int all_closed = 1;
    int i;

    if (devfd >= 0) {
        for (i = 0; i < devnum; i++) {
            if (bkn_ctrl[i].inited) {
                all_closed = 0;
                break;
            }
        }

        if (all_closed) {
            close(devfd);
            devfd = -1;
        }
    }

    return SHR_E_NONE;
}

static int
ngknet_vnet_wait(int unit)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;

    if (ioctl(bc->iofd, NGKNET_DEV_VNET_WAIT, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

static int
ngknet_hnet_wake(int unit)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;

    if (ioctl(bc->iofd, NGKNET_DEV_HNET_WAKE, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

static int
ngknet_vnet_dock(int unit, vnet_sync_t *vsync)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)vsync;
    ioc.op.data.len = sizeof(*vsync);

    if (ioctl(bc->iofd, NGKNET_DEV_VNET_DOCK, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

static int
ngknet_vnet_undock(int unit)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;

    if (ioctl(bc->iofd, NGKNET_DEV_VNET_UNDOCK, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

static int
ngknet_dev_ver_check(void)
{
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    if (ioctl(devfd, NGKNET_VERSION_GET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s\n"), devname));
        return SHR_E_FAIL;
    }

    if (ioc.rc) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("Unable to get KNET version\n")));
        return SHR_E_INTERNAL;
    }

    if (ioc.op.info.version != NGKNET_IOC_VERSION) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("KNET version mismatch, expect: %d, current: %d\n"),
                   NGKNET_IOC_VERSION, ioc.op.info.version));
        return SHR_E_FAIL;
    }

    return SHR_E_NONE;
}

int
bcmlu_ngknet_dev_check(void)
{
    int rv = SHR_E_NONE;

    if (SHR_FAILURE(ngknet_dev_open())) {
        rv = ngknet_dev_load();
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_UNAVAIL) {
                return rv;
            } else {
                return SHR_E_FAIL;
            }
        }
    }

    if (SHR_FAILURE(ngknet_dev_ver_check())) {
        ngknet_dev_close();
        return SHR_E_CONFIG;
    }

    return SHR_E_NONE;
}

int
bcmlu_ngknet_rx_rate_limit(int *rate_max, int set)
{
    struct ngknet_ioctl ioc;

    if (devfd < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("KNET device not ready\n")));
        return SHR_E_FAIL;
    }

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.iarg[0] = set;
    ioc.iarg[1] = *rate_max;

    if (ioctl(devfd, NGKNET_RX_RATE_LIMIT, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s\n"), devname));
        return SHR_E_FAIL;
    }

    if (!set) {
        *rate_max = ioc.iarg[1];
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_init(int unit, ngknet_dev_cfg_t *dev_cfg)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;
    bcmcnet_vnet_ops_t vnet_ops;

    if (bc->inited) {
        return SHR_E_NONE;
    }

    if (SHR_FAILURE(bcmlu_ngknet_dev_check())) {
        return SHR_E_INTERNAL;
    }

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)dev_cfg;
    ioc.op.data.len = sizeof(*dev_cfg);

    if (ioctl(devfd, NGKNET_DEV_INIT, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), devname, unit));
        return SHR_E_FAIL;
    }

    if (SHR_SUCCESS((int)ioc.rc) && dev_cfg->mode == DEV_MODE_HNET) {
        vnet_ops.vnet_wait = ngknet_vnet_wait;
        vnet_ops.hnet_wake = ngknet_hnet_wake;
        vnet_ops.vnet_dock = ngknet_vnet_dock;
        vnet_ops.vnet_undock = ngknet_vnet_undock;
        if (SHR_FAILURE(bcmcnet_vnet_ops_register(unit, &vnet_ops))) {
            return SHR_E_INTERNAL;
        }
    }

    bc->unit = unit;
    bc->iofd = devfd;
    bc->name = (char *)devname;
    bc->inited = 1;

    return ioc.rc;
}

int
bcmlu_ngknet_dev_cleanup(int unit)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    if (!bc->inited) {
        return SHR_E_NONE;
    }

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;

    if (ioctl(bc->iofd, NGKNET_DEV_DEINIT, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    bc->iofd = -1;
    bc->inited = 0;

    if (SHR_FAILURE(ngknet_dev_close())) {
        return SHR_E_INTERNAL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_close(void)
{
    return ngknet_dev_close();
}

int
bcmlu_ngknet_dev_chan_config(int unit, ngknet_chan_cfg_t *chan_cfg)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)chan_cfg;
    ioc.op.data.len = sizeof(*chan_cfg);

    if (ioctl(bc->iofd, NGKNET_QUEUE_CONFIG, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_chan_query(int unit, ngknet_chan_cfg_t *chan_cfg)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)chan_cfg;
    ioc.op.data.len = sizeof(*chan_cfg);

    if (ioctl(bc->iofd, NGKNET_QUEUE_QUERY, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_suspend(int unit)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;

    if (ioctl(bc->iofd, NGKNET_DEV_SUSPEND, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_resume(int unit)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;

    if (ioctl(bc->iofd, NGKNET_DEV_RESUME, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_rcpu_config(int unit, struct ngknet_rcpu_hdr *rcpu)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)rcpu;
    ioc.op.data.len = sizeof(*rcpu);

    if (ioctl(bc->iofd, NGKNET_RCPU_CONFIG, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_rcpu_get(int unit, struct ngknet_rcpu_hdr *rcpu)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)rcpu;
    ioc.op.data.len = sizeof(*rcpu);

    if (ioctl(bc->iofd, NGKNET_RCPU_GET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_info_get(int unit, struct bcmcnet_dev_info *info)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)info;
    ioc.op.data.len = sizeof(*info);

    if (ioctl(bc->iofd, NGKNET_INFO_GET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_stats_get(int unit, struct bcmcnet_dev_stats *stats)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)stats;
    ioc.op.data.len = sizeof(*stats);

    if (ioctl(bc->iofd, NGKNET_STATS_GET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_dev_stats_reset(int unit)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;

    if (ioctl(bc->iofd, NGKNET_STATS_RESET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_netif_create(int unit, ngknet_netif_t *netif)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)netif;
    ioc.op.data.len = sizeof(*netif);

    if (ioctl(bc->iofd, NGKNET_NETIF_CREATE, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_netif_destroy(int unit, int netif_id)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.iarg[0] = netif_id;

    if (ioctl(bc->iofd, NGKNET_NETIF_DESTROY, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_netif_get(int unit, int netif_id, ngknet_netif_t *netif)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.iarg[0] = netif_id;
    ioc.op.data.buf = (unsigned long)netif;
    ioc.op.data.len = sizeof(*netif);

    if (ioctl(bc->iofd, NGKNET_NETIF_GET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_netif_get_next(int unit, ngknet_netif_t *netif)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)netif;
    ioc.op.data.len = sizeof(*netif);

    if (ioctl(bc->iofd, NGKNET_NETIF_NEXT, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_netif_link_set(int unit, int netif_id, int up)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.iarg[0] = netif_id;
    ioc.iarg[1] = up;

    if (ioctl(bc->iofd, NGKNET_NETIF_LINK_SET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_filter_create(int unit, ngknet_filter_t *filter)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)filter;
    ioc.op.data.len = sizeof(*filter);

    if (ioctl(bc->iofd, NGKNET_FILT_CREATE, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_filter_destroy(int unit, int filter_id)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.iarg[0] = filter_id;

    if (ioctl(bc->iofd, NGKNET_FILT_DESTROY, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_filter_get(int unit, int filter_id, ngknet_filter_t *filter)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.iarg[0] = filter_id;
    ioc.op.data.buf = (unsigned long)filter;
    ioc.op.data.len = sizeof(*filter);

    if (ioctl(bc->iofd, NGKNET_FILT_GET, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}

int
bcmlu_ngknet_filter_get_next(int unit, ngknet_filter_t *filter)
{
    struct bkn_ctrl *bc = &bkn_ctrl[unit];
    struct ngknet_ioctl ioc;

    sal_memset(&ioc, 0, sizeof(ioc));

    ioc.unit = unit;
    ioc.op.data.buf = (unsigned long)filter;
    ioc.op.data.len = sizeof(*filter);

    if (ioctl(bc->iofd, NGKNET_FILT_NEXT, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_DMA,
                  (BSL_META("IOCTL failed on %s @ unit%d\n"), bc->name, unit));
        return SHR_E_FAIL;
    }

    return ioc.rc;
}


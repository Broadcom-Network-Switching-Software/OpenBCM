/*! \file bcma_sys_probe_xgssim.c
 *
 * Probe function for dumb simulator.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bsl/bsl.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <bcma/sys/bcma_sys_sim.h>
#include <bcma/sys/bcma_sys_probe_xgssim.h>

#include <bcmbd/bcmbd_simhook.h>

#include <sim/chelem/chelem.h>

#if BCMDRD_CONFIG_MEMMAP_DIRECT == 1
#error Simulation environment does not support direct register access
#endif

static int
io_read(void *dvc, bcmdrd_hal_io_space_t io_space,
        uint32_t addr, void *data, size_t size)
{
    int unit = *((int*)dvc);

    return chelem_read(unit, 0, addr, data, size);
}

static int
io_write(void *dvc, bcmdrd_hal_io_space_t io_space,
         uint32_t addr, void *data, size_t size)
{
    int unit = *((int*)dvc);

    return chelem_write(unit, 0, addr, data, size);
}

static int
sim_read(int unit, uint32_t addrx, uint32_t addr, void *data, int size)
{
    return chelem_read(unit, addrx, addr, data, size);
}

static int
sim_write(int unit, uint32_t addrx, uint32_t addr, void *data, int size)
{
    return chelem_write(unit, addrx, addr, data, size);
}

/*
 * These are the successfully create devices
 */
typedef struct sys_dev_s {

    /*! Device created by this probe function. */
    bool owned;

    /*! Unit number. */
    int unit;

} sys_dev_t;

/*! Actual devices in our system. */
static sys_dev_t sys_devs[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_probe_xgssim_cleanup(void)
{
    int unit;
    sys_dev_t *sd;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        sd = &sys_devs[unit];
        if (sd->owned) {
            if (bcmdrd_dev_exists(unit)) {
                bcmdrd_dev_destroy(unit);
            }
            bcmbd_simhook_read[unit] = NULL;
            bcmbd_simhook_write[unit] = NULL;
        }
        sal_memset(sd, 0, sizeof(*sd));
    }
    return 0;
}

int
bcma_sys_probe_xgssim(void)
{
    int ndevs = 0;
    int edx, unit;
    sys_dev_t *sd;
    bcmdrd_dev_id_t id;
    bcmdrd_hal_io_t io;
    const char *name;

    for (edx = 0; edx < BCMDRD_CONFIG_MAX_UNITS; edx++) {

        /* Set up ID structure for next simulated device */
        sal_memset(&id, 0, sizeof(id));
        if (bcma_sys_sim_dev_get(edx,
                                 &id.vendor_id, &id.device_id,
                                 &id.revision, NULL, &name) < 0) {
            /* No more devices */
            break;
        }

        /* Create device */
        if ((unit = bcmdrd_dev_create(-1, &id)) < 0) {
            LOG_WARN(BSL_LS_SYS_PCI,
                     (BSL_META("Failed to create device "
                               "(%04x:%04x:%02x)\n"),
                      id.vendor_id, id.device_id, id.revision));
            continue;
        }

        /* Convenience pointer */
        sd = &sys_devs[unit];

        /* Mark as successfully created */
        sd->owned = true;

        /* Install generic simulation hooks */
        bcmbd_simhook_read[unit] = sim_read;
        bcmbd_simhook_write[unit] = sim_write;

        /* Allow driver to account for simulation environment */
        bcmdrd_feature_enable(unit, BCMDRD_FT_PASSIVE_SIM);

        /* Initialize I/O structure */
        sal_memset(&io, 0, sizeof(io));

        /* Callback context is unit number */
        sys_devs[ndevs].unit = unit;
        io.devh = &sys_devs[ndevs].unit;

        io.read = io_read;
        io.write = io_write;

        if (bcmdrd_dev_hal_io_init(unit, &io) < 0) {
            LOG_ERROR(BSL_LS_SYS_PCI,
                      (BSL_META_U(unit,
                                  "Failed to initialize I/O\n")));
            continue;
        }

        if (++ndevs >= COUNTOF(sys_devs)) {
            LOG_WARN(BSL_LS_SYS_PCI,
                     (BSL_META("Too many XGSSIM devices\n")));
            break;
        }
    }

    return ndevs;
}

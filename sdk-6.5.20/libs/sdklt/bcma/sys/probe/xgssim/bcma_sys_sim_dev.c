/*! \file bcma_sys_sim_dev.c
 *
 * Default list of simulated deviced.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>

#include <bcma/sys/bcma_sys_sim.h>
#include <bcma/sys/bcma_sys_probe.h>

/*
 * These are the devices we pretend are present in our system
 */
/* Create device table */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _vn, _dv, _rv, _md, #_nm, #_fn },
#define BCMDRD_DEVLIST_INCLUDE_ALL
static struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t revision;
    uint16_t model;
    const char *name;
    const char *full_name;
} dev_table[] = {
#include <bcmdrd/bcmdrd_devlist.h>
    { 0, 0 } /* end-of-list */
};

static unsigned int override_devs = 0;
static unsigned int override_dev_idx[BCMDRD_CONFIG_MAX_UNITS];

int
bcma_sys_sim_dev_get(unsigned int dev_idx,
                     uint16_t *vendor_id, uint16_t *device_id,
                     uint16_t *revision, uint16_t *model, const char **name)
{
    int override_only =  BCMDRD_CONFIG_INCLUDE_CHIP_DEFAULT;

    if (override_only || override_devs > 0) {
        if (dev_idx >= override_devs) {
            /* Invalid device */
            return -1;
        }
        if (dev_idx >= COUNTOF(override_dev_idx)) {
            /* Should never get here */
            return -1;
        }
        /* Get override index */
        dev_idx = override_dev_idx[dev_idx];
    }

    if (dev_idx >= (COUNTOF(dev_table) - 1)) {
        return -1;
    }

    /* Return requested device information */
    if (vendor_id) {
        *vendor_id = dev_table[dev_idx].vendor_id;
    }
    if (device_id) {
        *device_id = dev_table[dev_idx].device_id;
    }
    if (revision) {
        *revision = dev_table[dev_idx].revision;
    }
    if (model) {
        *model = dev_table[dev_idx].model;
    }
    if (name) {
        *name = dev_table[dev_idx].name;
    }
    return 0;
}

int
bcma_sys_probe_override(int unit, const char *dev_name)
{
    int idx;

    if (override_devs >= COUNTOF(override_dev_idx)) {
        /* Too many override devices */
        return -1;
    }

    /* Try full name first */
    for (idx = 0; idx < (COUNTOF(dev_table) - 1); idx++) {
        if (sal_strcasecmp(dev_name, dev_table[idx].full_name) == 0) {
            override_dev_idx[override_devs++] = idx;
            return 0;
        }
    }

    /* Try name w/o revision suffix */
    for (idx = 0; idx < (COUNTOF(dev_table) - 1); idx++) {
        if (sal_strcasecmp(dev_name, dev_table[idx].name) == 0) {
            override_dev_idx[override_devs++] = idx;
            return 0;
        }
    }

    /* Unsupported device name */
    return -1;
}

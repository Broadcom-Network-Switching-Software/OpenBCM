/*! \file bcma_pcie_drv.c
 *
 * Common interface to the chip-specific PCIe driver functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcma/pcie/bcma_pcie.h>


/*******************************************************************************
 * Local defines
 */

/*! Device specific attach function type. */
typedef bcma_pcie_drv_t *(*bcma_pcie_drv_get_f)(int unit);

/*! Array of device specific driver retrieval functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bd##_bcma_pcie_drv_get },

static struct {
    bcma_pcie_drv_get_f get;
} list_dev_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/*! PCIe driver for each device */
bcma_pcie_drv_t *bcma_pcie_drv[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private functions
 */

static int
pcie_dev_type_get(int unit, bcmdrd_dev_type_t *dev_type)
{
    if (!dev_type) {
        return SHR_E_PARAM;
    }

    *dev_type = bcmdrd_dev_type(unit);
    if (*dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    return SHR_E_NONE;
}


/*******************************************************************************
 * Public functions
 */

int
bcma_pcie_drv_init(int unit)
{
    int rv;
    bcmdrd_dev_type_t dev_type;

    rv = pcie_dev_type_get(unit, &dev_type);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    bcma_pcie_drv[unit] = list_dev_get[dev_type].get(unit);

    return SHR_E_NONE;
}

int
bcma_pcie_phy_type_get(int unit, pciephy_driver_type_t *type)
{
    int rv;
    bcmdrd_dev_type_t dev_type;
    bcma_pcie_drv_t *drv;

    rv = pcie_dev_type_get(unit, &dev_type);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    drv = bcma_pcie_drv[unit];
    if (drv && drv->phy_type_get) {
        return drv->phy_type_get(unit, type);
    }

    return SHR_E_UNAVAIL;
}

int
bcma_pcie_fw_ver_get(pciephy_access_t *pa, uint16_t *ver)
{
    int rv, unit;
    bcmdrd_dev_type_t dev_type;
    bcma_pcie_drv_t *drv;

    unit = pa->unit;
    rv = pcie_dev_type_get(unit, &dev_type);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    drv = bcma_pcie_drv[unit];
    if (drv && drv->fw_ver_get) {
        return drv->fw_ver_get(pa, ver);
    }

    return SHR_E_UNAVAIL;
}

int
bcma_pcie_trace_get(int unit, uint32_t idx, uint32_t *val)
{
    bcma_pcie_drv_t *drv;

    drv = bcma_pcie_drv[unit];
    if (drv && drv->trace_get) {
        return drv->trace_get(unit, idx, val);
    }

    return SHR_E_UNAVAIL;
}

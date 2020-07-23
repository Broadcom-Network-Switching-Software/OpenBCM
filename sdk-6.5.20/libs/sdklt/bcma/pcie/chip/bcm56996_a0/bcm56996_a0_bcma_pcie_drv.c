/*! \file bcm56996_a0_bcma_pcie_drv.c
 *
 * Chip specific implementation for BCMA PCIe driver attach.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <bcmdrd/bcmdrd_hal.h>
#include <bcma/pcie/bcma_pcie.h>

/*******************************************************************************
 * Local definitions
 */

/* FW version register. */
#define PCIE_PHY_REG_FWVER 0xd230

/*! Trace buffer memory address. */
#define TRACE_ADDR         0x01327e00

/*! Number of trace buffer. */
#define TRACE_BUF_COUNT    128

/*******************************************************************************
 * PCIe driver
 */

static int
bcm56996_a0_pcie_phy_type_get(int unit, pciephy_driver_type_t *type)
{
    *type = PCIEPHY_DRIVER_TYPE_MERLIN7;
    return SHR_E_NONE;
}

static int
bcm56996_a0_pcie_fw_ver_get(pciephy_access_t *pa, uint16_t *ver)
{
    return pciephy_diag_reg_read(pa, PCIE_PHY_REG_FWVER, ver);
}

static int
bcm56996_a0_pcie_trace_get(int unit, uint32_t idx, uint32_t *val)
{
    if (idx >= TRACE_BUF_COUNT) {
        return SHR_E_PARAM;
    }
    return bcmdrd_hal_iproc_read(unit, TRACE_ADDR + idx *4, val);
}

static bcma_pcie_drv_t bcm56996_a0_bcma_pcie_drv = {
    .phy_type_get = bcm56996_a0_pcie_phy_type_get,
    .fw_ver_get = bcm56996_a0_pcie_fw_ver_get,
    .trace_get = bcm56996_a0_pcie_trace_get,
};

/*******************************************************************************
 * Public functions
 */

bcma_pcie_drv_t *
bcm56996_a0_bcma_pcie_drv_get(int unit)
{
    return &bcm56996_a0_bcma_pcie_drv;
}

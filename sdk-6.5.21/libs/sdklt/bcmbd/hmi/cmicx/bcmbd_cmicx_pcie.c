/*! \file bcmbd_cmicx_pcie.c
 *
 * CMICx PCIE driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_udelay.h>

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd_pcie_internal.h>
#include <bcmbd/bcmbd_cmicx_pcie.h>


/*******************************************************************************
 * Local definitions
 */

/* Delay in usec for PCIe indirect register access */
#define PCIE_IND_ACC_DELAY_USEC 100


/*******************************************************************************
 * Driver functions
 */

static int
cmicx_pcie_core_reg_read(int unit, uint32_t addr, uint32_t *data)
{
    int ioerr = 0;
    PAXB_0_CONFIG_IND_ADDRr_t cfg_ind_addr;
    PAXB_0_CONFIG_IND_DATAr_t cfg_ind_data;

    PAXB_0_CONFIG_IND_ADDRr_SET(cfg_ind_addr, addr);
    ioerr += WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, cfg_ind_addr);
    sal_udelay(PCIE_IND_ACC_DELAY_USEC);
    ioerr += READ_PAXB_0_CONFIG_IND_DATAr(unit, &cfg_ind_data);
    *data = PAXB_0_CONFIG_IND_DATAr_DATAf_GET(cfg_ind_data);
    sal_udelay(PCIE_IND_ACC_DELAY_USEC);

    return (ioerr == 0) ? SHR_E_NONE : SHR_E_FAIL;
}

static int
cmicx_pcie_core_reg_write(int unit, uint32_t addr, uint32_t data)
{
    int ioerr = 0;
    PAXB_0_CONFIG_IND_ADDRr_t cfg_ind_addr;
    PAXB_0_CONFIG_IND_DATAr_t cfg_ind_data;

    PAXB_0_CONFIG_IND_ADDRr_SET(cfg_ind_addr, addr);
    ioerr += WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, cfg_ind_addr);
    sal_udelay(PCIE_IND_ACC_DELAY_USEC);
    PAXB_0_CONFIG_IND_DATAr_SET(cfg_ind_data, data);
    ioerr += WRITE_PAXB_0_CONFIG_IND_DATAr(unit, cfg_ind_data);
    sal_udelay(PCIE_IND_ACC_DELAY_USEC);

    return (ioerr == 0) ? SHR_E_NONE : SHR_E_FAIL;
}

static int
cmicx_pcie_fw_prog_done(int unit)
{
    int ioerr = 0;
    PAXB_0_GEN3_UC_LOADER_STATUSr_t ldst;

    /*
     * Clear UC_PROG_DONE bit so the new firmware can take effect after device
     * reset.
     */
    PAXB_0_GEN3_UC_LOADER_STATUSr_CLR(ldst);
    ioerr += WRITE_PAXB_0_GEN3_UC_LOADER_STATUSr(unit, ldst);

    return (ioerr == 0) ? SHR_E_NONE : SHR_E_FAIL;
}


/*******************************************************************************
 * Driver object
 */

static bcmbd_pcie_drv_t cmicx_pcie_drv = {
    .read = cmicx_pcie_core_reg_read,
    .write = cmicx_pcie_core_reg_write,
    .fw_prog_done = cmicx_pcie_fw_prog_done,
};


/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicx_pcie_drv_init(int unit)
{
    return bcmbd_pcie_drv_init(unit, &cmicx_pcie_drv);
}

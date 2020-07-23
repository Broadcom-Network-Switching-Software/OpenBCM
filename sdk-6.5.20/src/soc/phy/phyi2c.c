/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    phyi2c.c
 * Purpose: I2C bus read/write routines for SFP copper PHY devices 
 */

#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_I2C)
#include <sal/types.h>
#include <soc/drv.h>
#include <soc/phy.h>
#include <soc/i2c.h>
#include <soc/debug.h>

#ifndef BCMSWAP16
#define BCMSWAP16(val) \
        ((uint16)( \
                (((uint16)(val) & (uint16)0x00ffU) << 8) | \
                (((uint16)(val) & (uint16)0xff00U) >> 8) ))
#endif

/* customer specific phy bus read/write hook pointers */
typedef struct {
    soc_phy_bus_rd_t rd;
    soc_phy_bus_wr_t wr;
} soc_phy_bus_hook_t;

STATIC soc_phy_bus_hook_t _soc_phy_bus_hook[SOC_MAX_NUM_DEVICES];
 

/*
 * Function:
 *  phy_i2c_miireg_read
 * Purpose:
 *  Read a MII register of the given PHY device on the I2C bus. Typically it is
 *  a copper SFP PHY device.
 * Parameters:
 *  unit         - StrataSwitch unit #.
 *  phy_id       - the address on the I2C bus.
 *  phy_reg_addr - MII register address.
 *  phy_rd_data  - pointer to a 16bit storage to hold read data.
 * Returns:
 *  SOC_E_XXX
 */

int
phy_i2c_miireg_read (int unit,uint32 phy_id,uint32 phy_reg_addr,
                                             uint16 *phy_rd_data)
{
    int rv = SOC_E_NONE;

    if (_soc_phy_bus_hook[unit].rd) {
        return (*_soc_phy_bus_hook[unit].rd)(unit,phy_id,phy_reg_addr,phy_rd_data);
    }

    /* Make sure that we're already attached, or go get attached */
    if (!soc_i2c_is_attached(unit)) {
        if ((rv = soc_i2c_attach(unit, 0, 0)) <= 0) {
            return rv;
        }
    }
    rv = soc_i2c_read_word_data(unit,phy_id,phy_reg_addr,phy_rd_data);

    if (rv == SOC_E_NONE) {
        *phy_rd_data = BCMSWAP16(*phy_rd_data);
    }
    return rv;
}

/*
 * Function:
 *  phy_i2c_miireg_write
 * Purpose:
 *  Write to a MII register of the given PHY device on the I2C bus. Typically it is
 *  a copper SFP PHY device.
 * Parameters:
 *  unit         - StrataSwitch unit #.
 *  phy_id       - the address on the I2C bus.
 *  phy_reg_addr - MII register address.
 *  phy_wr_data  - 16bit value to write to the MII register.
 * Returns:
 *  SOC_E_XXX
 */

int 
phy_i2c_miireg_write (int unit,uint32 phy_id,uint32 phy_reg_addr, 
                                                uint16 phy_wr_data)
{
    int rv = SOC_E_NONE;

    if (_soc_phy_bus_hook[unit].wr) {
        return (*_soc_phy_bus_hook[unit].wr)(unit,phy_id,phy_reg_addr,phy_wr_data);
    }

    /* Make sure that we're already attached, or go get attached */
    if (!soc_i2c_is_attached(unit)) {
        if ((rv = soc_i2c_attach(unit, 0, 0)) <= 0) {
            return rv;
        }
    }

    rv = soc_i2c_write_word_data(unit,phy_id,phy_reg_addr,BCMSWAP16(phy_wr_data));
    return rv;
}

/*
 * Function:
 *  phy_i2c_bus_func_hook_set
 * Purpose:
 *  Install customer specific I2C bus read/write routines 
 *  
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  rd   - I2C bus read routine.
 *  wr   - I2C bus write routine.
 * Returns:
 *  SOC_E_XXX
 */

int phy_i2c_bus_func_hook_set (int unit, soc_phy_bus_rd_t rd, soc_phy_bus_wr_t wr)
{
    _soc_phy_bus_hook[unit].rd = rd;
    _soc_phy_bus_hook[unit].wr = wr;
    return SOC_E_NONE;
}
#else
typedef int _soc_phy_phyi2c_not_empty; /* Make ISO compilers happy. */
#endif

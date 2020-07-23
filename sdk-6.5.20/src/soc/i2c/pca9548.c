/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM56xx I2C Device Driver for Phillips PCA9548 i2c switch
 */

#include <sal/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>

STATIC int
pca9548_read(int unit, int devno, uint16 addr, uint8* data, uint32* len)
{
    
    int rv;
    uint8 saddr = soc_i2c_addr(unit, devno);

    rv = soc_i2c_read_byte(unit, saddr, data);
    *len = 1; /* Byte device */
    /* LOG_CLI((BSL_META_U(unit,
                           "sw%d: read 0x%x from sw at 0x%x\n"), devno, *data, saddr)); */
    soc_i2c_device(unit, devno)->rbyte++;   
    return rv;
}

/*
 * The data is what gets actually written to the channel selection
 * register. The channel must be converted to the appropriate bit
 * by the caller.
 */
STATIC int
pca9548_write(int unit, int devno, uint16 addr, uint8* data, uint32 len)
{
    int rv;
    uint8 saddr = soc_i2c_addr(unit, devno);

    rv = soc_i2c_write_byte(unit, saddr, *data);
    /* LOG_CLI((BSL_META_U(unit,
                           "sw%d: wrote 0x%x to sw at 0x%x\n"), devno, *data, saddr)); */
    soc_i2c_device(unit, devno)->tbyte++;   
    return rv;
}



STATIC int
pca9548_ioctl(int unit, int devno, int opcode, void* data, int len)
{
    return SOC_E_NONE;
}

STATIC int
pca9548_init(int unit, int devno, void* data, int len)
{

    soc_i2c_devdesc_set(unit, devno, "PCA9548 i2c Switch");
    if (!SOC_IS_SHADOW(unit)) {
        /* enable channel 0 by default */
        return soc_i2c_write_byte(unit, soc_i2c_addr(unit, devno), 1);
    }
    return SOC_E_NONE;
}


/* PCA9548 Clock Chip Driver callout */
i2c_driver_t _soc_i2c_pca9548_driver = {
    0x0, 0x0, /* System assigned bytes */
    PCA9548_DEVICE_TYPE,
    pca9548_read,
    pca9548_write,
    pca9548_ioctl,
    pca9548_init,
    NULL,
};


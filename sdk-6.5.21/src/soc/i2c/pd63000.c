/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM56xx I2C Device Driver for PD63000 8-bit PoE Microcontroller Unit.
 * The MCU communicates with message types of 15-byte long packet.
 */

#include <shared/bsl.h>

#include <sal/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>

#define PD63000_CTN     15

STATIC int
pd63000_read(int unit, int devno,
             uint16 addr, uint8* data, uint32* len)
{
    
    int i, rv = SOC_E_NONE;
    uint8 saddr = soc_i2c_addr(unit, devno);
    uint8 *ptr;
    
    if (!data) {
        return SOC_E_PARAM;
    }

    ptr = data;
    for (i = 0; i < PD63000_CTN; i++, ptr++) {
        if ((rv = soc_i2c_read_byte(unit, saddr, ptr)) < 0) {
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "i2c%d: soc_i2c_pd63000_read: "
                                 "failed to read data byte %d.\n"),
                      unit, i));
            break;
        }
        soc_i2c_device(unit, devno)->rbyte++;   
    }
    *len = i;
          
    return rv;
}

STATIC int
pd63000_write(int unit, int devno,
              uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    uint8 saddr = soc_i2c_addr(unit, devno);
    uint8 *ptr;
    uint32 i;

    if (!data) {
        return SOC_E_PARAM;
    }
    assert((PD63000_CTN == len));

    I2C_LOCK(unit);

    if ((rv = soc_i2c_start(unit, SOC_I2C_TX_ADDR(saddr))) < 0) {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "i2c%d: soc_i2c_pd63000_write: "
                             "failed to generate start.\n"),
                  unit));
        I2C_UNLOCK(unit);
        return rv;
    }

    ptr = data;
    for (i = 0; i < len; i++, ptr++) {
        if ((rv = soc_i2c_write_one_byte(unit, *ptr) ) < 0 ) {
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "i2c%d: soc_i2c_pd63000_write: "
                                 "failed to send byte %d.\n"),
                      unit, i ));
            break;
        }
        soc_i2c_device(unit, devno)->tbyte++;
    }

    soc_i2c_stop(unit);
    I2C_UNLOCK(unit);

    return rv;
}



STATIC int
pd63000_ioctl(int unit, int devno,
              int opcode, void* data, int len)
{
    return SOC_E_NONE;
}

STATIC int
pd63000_init(int unit, int devno,
             void* data, int len)
{
    uint8 *init_vector = (uint8*)data;
    uint8 pkt[PD63000_CTN];
    int rv = SOC_E_NONE;
    uint32 l;
    soc_timeout_t to;

    soc_i2c_devdesc_set(unit, devno, "PD63000 PoE MCU");

    if ((rv = pd63000_read(unit, devno, 0, pkt, &l)) < 0) {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "i2c%d: soc_i2c_pd63000_init: "
                             "failed to read init packet.\n"), unit));
        return rv;
    }

    if ((rv = pd63000_write(unit, devno, 0, 
                            init_vector, PD63000_CTN)) < 0) {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "i2c%d: soc_i2c_pd63000_init: "
                             "failed to send power init packet.\n"), unit));
        return rv;
    }

    soc_timeout_init(&to, 100000, 0);
    for (;;) {
         if (soc_timeout_check(&to)) {
             break;
         }
    }

    if ((rv = pd63000_read(unit, devno, 0, pkt, &l)) < 0) {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "i2c%d: soc_i2c_pd63000_init: "
                             "failed to read power init packet.\n"), unit));
        return rv;
    }

    return rv;
}


/* PD63000 Chip Driver callout */
i2c_driver_t _soc_i2c_pd63000_driver = {
    0x0, 0x0, /* System assigned bytes */
    PD63000_DEVICE_TYPE,
    pd63000_read,
    pd63000_write,
    pd63000_ioctl,
    pd63000_init,
    NULL,
};


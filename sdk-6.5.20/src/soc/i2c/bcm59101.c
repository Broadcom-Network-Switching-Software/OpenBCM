/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM56xx I2C Device Driver for BCM59101 8-bit PoE Microcontroller Unit.
 * The MCU communicates with message types of 12-byte long packet.
 */

#include <sal/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>
#define BCM59101_CTN     12

STATIC int
bcm59101_read(int unit, int devno,
             uint16 addr, uint8* data, uint32* len)
{
    int rv = SOC_E_NONE;
    uint8 saddr_r;
    uint32 nbytes = 0;


    /* Valid address, memory and size must be provided */
    if ( ! len || ! data )
        return SOC_E_PARAM;

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "bcm59101_read: unit: %d devno: %d addr: %d data: %p len: %u\n"),
              unit, devno, addr, (void *)data, *len));

    I2C_LOCK(unit);

    saddr_r = SOC_I2C_RX_ADDR(soc_i2c_addr(unit, devno));

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "bcm59101_read: devno=0x%x (data=%p) len=%d\n"),
              devno, (void *)data, (int)*len));

    /* Now, we have sent the first and second word addresses,
     * we then issue a repeated start condition, followed by
     * the device's read address (note: saddr_r)
     */
    /* Generate Start, for Write address */
    if( (rv = soc_i2c_start(unit, saddr_r)) < 0){
      LOG_INFO(BSL_LS_SOC_I2C,
               (BSL_META_U(unit,
                           "bcm59101_write(%d,%d,0x%x,%p,%d): "
                           "failed to gen start\n"),
                unit, devno, addr, (void *)data, *len));
      goto error;
    }

    nbytes = *len;
    if ( (rv = soc_i2c_read_bytes(unit, data, (int *)&nbytes, 0) ) < 0 ) {
        goto error;
    }
    *len = nbytes;

 error:

    soc_i2c_stop(unit);

    I2C_UNLOCK(unit);
    return rv ;

}

STATIC int
bcm59101_write(int unit, int devno,
              uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    uint8 saddr = soc_i2c_addr(unit, devno);
    uint8 *ptr;
    uint32 i;

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "bcm59101_write: unit: %d devno: %d addr: %d data: %p len: %d\n"),
              unit, devno, addr, (void *)data, len));

    if (!data) {
        return SOC_E_PARAM;
    }

    I2C_LOCK(unit);

    if ((rv = soc_i2c_start(unit, SOC_I2C_TX_ADDR(saddr))) < 0) {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "i2c%d: soc_i2c_bcm59101_write: "
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
                                 "i2c%d: soc_i2c_bcm59101_write: "
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
bcm59101_ioctl(int unit, int devno,
              int opcode, void* data, int len)
{
    return SOC_E_NONE;
}

STATIC int
bcm59101_init(int unit, int devno,
             void* data, int len)
{
    uint8 pkt[BCM59101_CTN];
    uint32 l = 2;
    soc_timeout_t to;
    int rv = SOC_E_NONE;

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "bcm59101_init: unit: %d devno: %d data: %p len: %d\n"),
              unit, devno, data, len)); 
    /*
    Request :
    0x20 0x01 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0x18

    Response:
    0x20 0x01   Rest dont care ..
    0xAF   Rest dont care ..
     */

    soc_i2c_devdesc_set(unit, devno, "BCM59101 POE controller");

    if ((rv = bcm59101_write(unit, devno, 0, data, len)) < 0) {
                            
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "i2c%d: soc_i2c_bcm59101_init: "
                             "failed to send power init packet.\n"), unit));
        return rv;
    }

    soc_timeout_init(&to, 100000, 0);
    for (;;) {
         if (soc_timeout_check(&to)) {
             break;
         }
    }

    l = BCM59101_CTN;
    if ((rv = bcm59101_read(unit, devno, 0, pkt, &l)) < 0) {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "i2c%d: soc_i2c_bcm59101_init: "
                             "failed to read power init packet.\n"), unit));
        return rv;
    }

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "bcm59101_init: Signature read %x %x\n"), *pkt, *(pkt+1)));

    if ((*pkt != 0x20 || *(pkt+1) != 0x01) && *pkt != 0xaf) {

        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "i2c%d: soc_i2c_bcm59101_init: "
                             "bcm5910 not found.\n"), unit));

        return SOC_E_UNAVAIL;
    }

    return rv;
}


/* BCM59101 Chip Driver callout */
i2c_driver_t _soc_i2c_bcm59101_driver = {
    0x0, 0x0, /* System assigned bytes */
    BCM59101_DEVICE_TYPE,
    bcm59101_read,
    bcm59101_write,
    bcm59101_ioctl,
    bcm59101_init,
    NULL,
};

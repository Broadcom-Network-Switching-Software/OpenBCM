/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM56xx I2C Device Driver for Phillips PCF8574 Parallel port.
 *
 * On P48 systems, the parallel port is typically connected to two
 * MUX's used to control I2C slave addressing on the common I2C bus.
 *
 *
 *       o-------[MUX1]--[MUX2]
 *       |         |       |
 *       |         |       |
 *       |         |       |
 *   [PCF8574]   (SxA)   (SyB)
 *       |         |       |
 *       0=========O=======0================I2C
 *
 *
 * SxA = Slave X with Address Bits A
 * SxB = Slave Y with Address Bits B
 */

#include <shared/bsl.h>

#include <sal/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>

STATIC int
pcf8574_read(int unit, int devno,
	  uint16 addr, uint8* data, uint32* len)
{
    
    int rv;
    uint8 saddr = soc_i2c_addr(unit, devno);

    rv = soc_i2c_read_byte(unit, saddr, data);
    *len = 1; /* Byte device */
    /* LOG_CLI((BSL_META_U(unit,
                           "lpt0: read 0x%x from HW\n"),*data)); */
    soc_i2c_device(unit, devno)->rbyte++;   
    return rv;
}

STATIC int
pcf8574_write(int unit, int devno,
	   uint16 addr, uint8* data, uint32 len)
{
    int rv;
    uint8 saddr = soc_i2c_addr(unit, devno);

    rv = soc_i2c_write_byte(unit, saddr, *data);
    /* LOG_CLI((BSL_META_U(unit,
                           "lpt0: wrote 0x%x to HW\n"),*data)); */
    soc_i2c_device(unit, devno)->tbyte++;   
    return rv;
}



STATIC int
pcf8574_ioctl(int unit, int devno,
	   int opcode, void* data, int len)
{
    return SOC_E_NONE;
}

STATIC int
pcf8574_init(int unit, int devno,
	  void* data, int len)
{
    uint32 bytes;
    uint8 lpt_val = 0;
    uint8 saddr = soc_i2c_addr(unit, devno);        

    if (saddr == I2C_LPT_SADDR0) {
        pcf8574_read(unit, devno, 0, &lpt_val, &bytes);
	if (lpt_val == 0xFF) {
	    /* This is probably immediately after power-on; reset to zero. */
	    lpt_val = 0;
	    pcf8574_write(unit, devno, 0, &lpt_val, 1);
	}
	soc_i2c_devdesc_set(unit, devno, "PCF8574 MUX control");
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "%s: mux control 0x%x\n"),
                     soc_i2c_devname(unit, devno), lpt_val));
    } else if (saddr == I2C_LPT_SADDR1) {
        /* This device used as input; write 0xFF to disable quasi-outputs */
        lpt_val = 0xFF;
        pcf8574_write(unit, devno, 0, &lpt_val, 1);

        /* Now read what is being driven onto the quasi-inputs */
        /* coverity[callee_ptr_arith: FALSE] */
        pcf8574_read(unit, devno, 0, &lpt_val, &bytes);

        /* Try to read from the PCA9554 device */
        if (lpt_val == 0xFF || lpt_val == 0x0) {
            lpt_val = 0; /* Write 0x00 to command register first */
            pcf8574_write(unit, devno, 0, &lpt_val, 1);
            pcf8574_read(unit, devno, 0, &lpt_val, &bytes);
        }

        soc_i2c_devdesc_set(unit, devno, "PCF8574 or PCA9554 Baseboard ID");
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "%s: baseboard id 0x%x\n"),
                     soc_i2c_devname(unit, devno), lpt_val));
        SOC_CONTROL(unit)->board_type = lpt_val;
    } else if (saddr == I2C_LPT_SADDR2) {
        /* 
	 * The power-up state is the appropriate default; 
	 * no need to manually initialize the HCLK PCF8574.
	 */
	soc_i2c_devdesc_set(unit, devno, "PCF8574 HCLK control");
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "%s: hclk control 0x%x\n"),
                     soc_i2c_devname(unit, devno), lpt_val));
    } else if (saddr == I2C_LPT_SADDR3) {
        pcf8574_read(unit, devno, 0, &lpt_val, &bytes);
	if (lpt_val == 0xFF) {
	    /* This is probably immediately after power-on; reset to zero. */
	    lpt_val = 0x0;
	    pcf8574_write(unit, devno, 0, &lpt_val, 1);
	}
	pcf8574_write(unit, devno, 0, &lpt_val, 1);
	soc_i2c_devdesc_set(unit, devno, "PCF8574 POE control");
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "%s: poe control 0x%x\n"),
                     soc_i2c_devname(unit, devno), lpt_val));
    } else if (saddr == I2C_LPT_SADDR4) {
	soc_i2c_devdesc_set(unit, devno, "PCF8574 synthesizer frequency selector M");
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "%s: synth freq select B 0x%x\n"),
                     soc_i2c_devname(unit, devno), lpt_val));
    } else if (saddr == I2C_LPT_SADDR5) {
	soc_i2c_devdesc_set(unit, devno, "PCF8574 synthesizer frequency selector N");
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "%s: synth freq select A 0x%x\n"),
                     soc_i2c_devname(unit, devno), lpt_val));
    } else if (saddr == I2C_LPT_SADDR6) {
	soc_i2c_devdesc_set(unit, devno, "PCF8574 PPD clock delay");
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "%s: clock selector 0x%x\n"),
                     soc_i2c_devname(unit, devno), lpt_val));
    } else if (saddr == I2C_LPT_SADDR7) {
	soc_i2c_devdesc_set(unit, devno, "PCF8574 PPD clock divider");
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "%s: clock selector 0x%x\n"),
                     soc_i2c_devname(unit, devno), lpt_val));
    } else {
	soc_i2c_devdesc_set(unit, devno, "PCF8574 Parallel Port");
    }
    return SOC_E_NONE;
}


/* PCF8574 Clock Chip Driver callout */
i2c_driver_t _soc_i2c_pcf8574_driver = {
    0x0, 0x0, /* System assigned bytes */
    PCF8574_DEVICE_TYPE,
    pcf8574_read,
    pcf8574_write,
    pcf8574_ioctl,
    pcf8574_init,
    NULL,
};


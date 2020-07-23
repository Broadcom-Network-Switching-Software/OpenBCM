/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * I2C driver for the CPU Management Interface Controller (CMIC). This
 * module provides for discovery and loading of I2C device drivers and
 * manipulation of I2C device state.
 *
 * See also: bus.c
 */

#include <sal/types.h>
#include <sal/core/boot.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/cmic.h>
#include <soc/i2c.h>
#include <shared/bsl.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

/*
 * SD9560x Baseboard components (from UM)
 *
 * Device   AddrLines NDevices   Base Address     Board0     Description
 * LM75     3      2          1001XXXY        1001000R   I2C TempSensor
 * LM75     3      2          1001XXXY        1001100R   I2C TempSensor
 * 24LC64   3      2          1010XXXY        1010000R   I2C EEPROM
 * 24LC64   3      2          1010XXXY        1010100R   I2C EEPROM
 * W229b    n/a    n/a        1101001Y        1101001Y   Clock chip
 * MAX127   n/a    n/a        1010000Y        1010000Y   A/D Converter
 *
 * X=Don't Care, Y= Read/Write
 */

/* LM63 driver - lm63.c */
extern i2c_driver_t _soc_i2c_lm63_driver;
/* LM75 driver - lm75.c */
extern i2c_driver_t _soc_i2c_lm75_driver;
/* MAX664X driver - max664x.c */
extern i2c_driver_t _soc_i2c_max664x_driver;
/* MAX127 Driver - max127.c */
extern i2c_driver_t _soc_i2c_max127_driver;
/* W229B Driver - w229b.c */
extern i2c_driver_t _soc_i2c_w229b_driver;
/* AT24C64 EEPROM driver - 24c64.c */
extern i2c_driver_t _soc_i2c_eep24c64_driver;
/* XFP driver - xfp.c */
extern i2c_driver_t _soc_i2c_xfp_driver;
/* PCIE config space register access driver - pcie.c */
extern i2c_driver_t _soc_i2c_pcie_driver;
/* Phillips PCF8574 Parallel Port - pcf8574.c */
/* Phillips PCF8574 Parallel Port - pcf8574.c */
extern i2c_driver_t _soc_i2c_pcf8574_driver;
/* LTC 1427 DAC */
extern i2c_driver_t _soc_i2c_ltc1427_driver ;
/* Matrix Orbital LCD Display */
extern i2c_driver_t _soc_i2c_lcd_driver;
/* Cypress 22393 clock chip driver */
extern i2c_driver_t _soc_i2c_cy2239x_driver;
/* Cypress 22150 clock chip driver */
extern i2c_driver_t _soc_i2c_cy22150_driver;
/* LTC4258 Powered Ethernet chip driver */
extern i2c_driver_t _soc_i2c_ltc4258_driver;
/* PD63000 Powered Ethernet chip driver */
extern i2c_driver_t _soc_i2c_pd63000_driver;
/* MAX5478 Digital Potentiometer chip driver */
extern i2c_driver_t _soc_i2c_max5478_driver;
/* BCM59101 Power Over Ethernet chip driver */
extern i2c_driver_t _soc_i2c_bcm59101_driver;
/* PCA9548 8-channel i2c mux chip driver */
extern i2c_driver_t _soc_i2c_pca9548_driver;
/* ADP4000 an integrated power control IC */
extern i2c_driver_t _soc_i2c_adp4000_driver;
/* CxP driver */
extern i2c_driver_t _soc_i2c_cxp_driver;
/* LTC3880 an integrated power control IC */
extern i2c_driver_t _soc_i2c_ltc3880_driver;
/* LTC3882 an integrated power control IC */
extern i2c_driver_t _soc_i2c_ltc3882_driver;
/* LTC3884 an integrated power control IC */
extern i2c_driver_t _soc_i2c_ltc3884_driver;
/* LTC2974 an integrated power monitoring  IC */
extern i2c_driver_t _soc_i2c_ltc2974_driver;
/* LTM4676 an integrated power control IC */
extern i2c_driver_t _soc_i2c_ltm4676_driver;
/* PMBUS an integrated power control IC */
extern i2c_driver_t _soc_i2c_pmbus_driver;

extern i2c_driver_t _soc_i2c_max669x_driver;

extern i2c_driver_t _soc_i2c_ltm4678_driver;

extern i2c_driver_t _soc_i2c_isl68127_driver;

#if defined(SHADOW_SVK)
/* PCA9505 40-bit i2c bus IO port */
extern i2c_driver_t _soc_i2c_pca9505_driver;
#endif

/*
 * AT24C64 init (test) data - one page, and one word, used
 * for page boundary write tests
 */
 unsigned char
eep24c64_test_data[] = {
    0x4A,0x46,0x44,0xff,
    0x01,0x02,0x03,0x04,
    0x05,0x06,0x07,0x08,
    0x09,0x0a,0x0b,0x0c,
    0x0d,0x0e,0x0f,0x10,
    0x11,0x12,0x13,0x14,
    0x15,0x16,0x17,0x18,
    0x19,0x1a,0x1b,0x1c,
    0xba,0xbe,0xba,0xbe
};
#define eep24c64_test_data_len COUNTOF(eep24c64_test_data)

/*
 * PD63000 init (test) data
 */
STATIC unsigned char
pd63000_test_data[] = {
    0x00,0xfe,0x07,0x0b,0x15, /* init power source 1 to min 37W */
    0x00,0x25,0x02,0x49,0x01,
    0xb8,0x01,0x4e,0x02,0x9d
};
#define pd63000_test_data_len COUNTOF(pd63000_test_data)

/*
 * BCM59101 init (test) data
 */
/* STATIC */ unsigned char
bcm59101_test_data[] = {
    0x20,0x01,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0x18

};

#define bcm59101_test_data_len COUNTOF(bcm59101_test_data)

/* Data: i2c_devices
 * Purpose: I2C device descriptor table.
 *          Devices are added to this table, along with their slave
 *          address, a string description, and their driver (if available).
 *
 * Note: The entry for the "MUX" (LPT0) must be the first entry in the list
 */
#define	I2CDEV(_name, _num, _drv) {	\
	I2C_##_name##_##_num,		\
	I2C_##_name##_SADDR##_num,	\
	&_soc_i2c_##_drv##_driver,	\
	}
#define	I2CDEVTD(_name, _num, _drv) {	\
	I2C_##_name##_##_num,		\
	I2C_##_name##_SADDR##_num,	\
	&_soc_i2c_##_drv##_driver,	\
	_drv##_test_data,		\
	_drv##_test_data_len,		\
	}

STATIC i2c_device_t i2c_common_devices[] = {
    I2CDEV(LPT, 0, pcf8574),		/* MUX register, must be first */
    I2CDEVTD(NVRAM, 0, eep24c64),
    I2CDEV(XFP, 0, xfp),
    I2CDEV(PCIE, 0, pcie),
    I2CDEVTD(NVRAM, 1, eep24c64),
    I2CDEV(LM75, 4, lm75),
    I2CDEV(MAX664X, 0, max664x),
    I2CDEV(LM75, 0, lm75),
    I2CDEV(LM75, 1, lm75),
    I2CDEV(LM75, 2, lm75),
    I2CDEV(LM75, 3, lm75),
    I2CDEV(LM75, 5, lm75),
    I2CDEV(LM75, 6, lm75),
    I2CDEV(LM75, 7, lm75),
    I2CDEV(ADC, 0, max127),
    I2CDEV(ADC, 1, max127),
    I2CDEV(ADC, 2, max127),
    I2CDEV(ADC, 3, max127),
    I2CDEV(DAC, 0, ltc1427),
#if 0
    I2CDEV(PLL, 0, w229b),
#endif
    I2CDEV(LPT, 1, pcf8574),
    I2CDEV(LPT, 2, pcf8574),
    I2CDEV(LPT, 3, pcf8574),
    I2CDEV(LPT, 4, pcf8574),
#ifdef SHADOW_SVK  /* Shares same IO addr */
    I2CDEV(IOP, 0, pca9505),
    I2CDEV(IOP, 1, pca9505),
#else
    I2CDEV(LPT, 5, pcf8574),
    I2CDEV(LPT, 6, pcf8574),
#endif
    I2CDEV(LPT, 7, pcf8574),
    I2CDEV(LCD, 0, lcd),
    I2CDEV(XPLL, 0, cy2239x),
    I2CDEV(XPLL, 1, cy2239x),
    I2CDEV(XPLL1, 0, cy22150),		/* should this be xpll, 1? */
    I2CDEVTD(POE, 0, pd63000),
    I2CDEV(POE, 1, ltc4258),
    I2CDEV(POE, 2, ltc4258),
    I2CDEV(POE, 3, ltc4258),
    I2CDEV(POE, 4, ltc4258),
    I2CDEV(POE, 5, ltc4258),
    I2CDEV(POE, 6, ltc4258),
    I2CDEVTD(POE, 7, bcm59101),
    I2CDEVTD(POE, 8, bcm59101),
    I2CDEVTD(POE, 9, bcm59101),
    I2CDEVTD(POE, 10, bcm59101),
    I2CDEV(MUX, 0, pca9548),             /* MUX device */
    I2CDEV(MUX, 3, pca9548),             /* MUX device */
    I2CDEV(MUX, 4, pca9548),             /* MUX device */
    I2CDEV(MUX, 5, pca9548),             /* MUX device */
    I2CDEV(MUX, 6, pca9548),             /* MUX device */
#ifdef BCM_TOMAHAWK_SUPPORT
    I2CDEV(MUX, 70, pca9548),            /* MUX device */
#endif /* BCM_TOMAHAWK_SUPPORT */
    I2CDEV(POT, 0, max5478),
    I2CDEV(PWCTRL, 0, adp4000),
    I2CDEV(PWCTRL, 2, adp4000),
    I2CDEV(LTC3880, 45, ltc3880),
    I2CDEV(LTC3882, 0, ltc3882),
    I2CDEV(LTC3882, 1, ltc3882),
    I2CDEV(LTC2974, 0, ltc2974),
    I2CDEV(LTC3880, 60, ltc3880),
    I2CDEV(LTC3880, 61, ltc3880),
    I2CDEV(LTC3880, 62, ltc3880),
    I2CDEV(LTC3880, 64, ltc3880),
    I2CDEV(LTC3880, 65, ltc3880),
    I2CDEV(LTM4676, 43, ltm4676),
#ifdef BCM_TOMAHAWK_SUPPORT
    I2CDEV(LTC3880, 41, ltc3880),
    I2CDEV(LTC3880, 42, ltc3880),
    I2CDEV(LTC3880, 43, ltc3880),
    I2CDEV(LTC3880, 47, ltc3880),
    I2CDEV(LTC3882, 55, ltc3882),
    I2CDEV(LTC3882, 66, ltc3882),
    I2CDEV(LTC3882, 77, ltc3882),
#endif /* BCM_TOMAHAWK_SUPPORT */
};

STATIC i2c_device_t i2c_56565_devices[] = {
    I2CDEV(LPT, 1, pcf8574),
    I2CDEVTD(NVRAM, 56, eep24c64),
    I2CDEV(LM63, 0, lm63),
    I2CDEV(MUX, 3, pca9548),
    I2CDEV(LTC3884, 42, ltc3884),
    I2CDEV(LTC3884, 43, ltc3884),
    I2CDEV(LTC3884, 46, ltc3884),
    I2CDEV(LTM4676, 47, ltm4676),
};

STATIC i2c_device_t i2c_56760_devices[] = {
    I2CDEV(LPT, 1, pcf8574),
    I2CDEV(LPT, 3, pcf8574),
    I2CDEVTD(NVRAM, 56, eep24c64),
    I2CDEV(LM63, 0, lm63),
    I2CDEV(MUX, 3, pca9548),
    I2CDEV(LTC3884, 42, ltc3884),
    I2CDEV(LTC3884, 43, ltc3884),
    I2CDEV(LTC3880, 45, ltc3880),
};

STATIC i2c_device_t i2c_56270_devices[] = {
    I2CDEV(LPT, 1, pcf8574),
    I2CDEV(MUX, 3, pca9548),
    I2CDEV(MUX, 4, pca9548),
    I2CDEVTD(NVRAM, 1, eep24c64),
    I2CDEV(LTC2974, 0, ltc2974),
};

STATIC i2c_device_t i2c_56272_devices[] = {
    I2CDEV(LPT, 1, pcf8574),
    I2CDEVTD(NVRAM, 1, eep24c64),
};

STATIC i2c_device_t i2c_56970_32_devices[] = {
    I2CDEV(LPT, 1, pcf8574),
    I2CDEV(MUX, 0, pca9548),
    I2CDEV(MUX, 3, pca9548),
    I2CDEV(MUX, 4, pca9548),
    I2CDEVTD(NVRAM, 1, eep24c64),
    I2CDEV(NCP81233, 0, pmbus),
    I2CDEV(LTC2974, 5C, ltc2974),
    I2CDEV(LTC3880, 62, ltc3880),
};

STATIC i2c_device_t i2c_56970_64_devices[] = {
    I2CDEV(LPT, 1, pcf8574),
    I2CDEV(MUX, 0, pca9548),
    I2CDEV(MUX, 3, pca9548),
    I2CDEV(MUX, 4, pca9548),
    I2CDEVTD(NVRAM, 1, eep24c64),
    I2CDEV(NCP81233, 0, pmbus),
    I2CDEV(LTC2974, 5C, ltc2974),
    I2CDEV(LTC3880, 62, ltc3880),
    I2CDEV(LTC3880, 61, ltc3880),
};

STATIC i2c_device_t i2c_56870_devices[] = {
    I2CDEV(LPT, 1, pcf8574), /* I2C address 0x27 */
    I2CDEV(MUX, 3, pca9548), /* I2C address 0x73 */
    I2CDEV(MUX, 4, pca9548), /* I2C address 0x74 */
    I2CDEVTD(NVRAM, 1, eep24c64), /* I2C address 0x54 */
    I2CDEV(NCP81233, 0, pmbus), /* I2C address 0x70 */
    I2CDEV(LTC2974, 5C, ltc2974),  /* I2C address 0x5C */
    I2CDEV(LTC3880, 62, ltc3880),  /* I2C address 0x62 */
};

STATIC i2c_device_t i2c_56670_devices[] = {
    I2CDEV(LPT, 1, pcf8574),/* I2C address 0x27 */
    I2CDEV(LPT, 3, pcf8574),/* I2C address 0x26 */
    I2CDEVTD(NVRAM, 56, eep24c64), /* I2C address 0x54 */
    I2CDEV(LM63, 0, lm63),
    I2CDEV(MUX, 3, pca9548),/* I2C address 0x73 */
    I2CDEV(LTC3884, 42, ltc3884),/* I2C address 0x42 */
    I2CDEV(LTC3884, 43, ltc3884), /* I2C address 0x43 */
    I2CDEV(LTC2974, 64, ltc2974),  /* I2C address 0x64 */ 
};

STATIC i2c_device_t i2c_56980_devices[] = {
    I2CDEV(LPT, 1, pcf8574), /* I2C address 0x27 (board ID) */
    I2CDEV(MUX, 3, pca9548), /* I2C address 0x73 (mux 3) */
    I2CDEV(MUX, 4, pca9548), /* I2C address 0x74 (mux 4) */
    I2CDEVTD(NVRAM, 1, eep24c64), /* I2C address 0x54 (EEPROM) */
    I2CDEV(MAX669X, 18, max669x),
    I2CDEV(MAX669X, 19, max669x),
    I2CDEV(NCP81233, 0, pmbus),
    I2CDEV(LTM4678, 40, ltm4678),
    I2CDEV(LTM4678, 41, ltm4678),
    I2CDEV(LTC2974, 5C, ltc2974),
    I2CDEV(ISL68127, 60, isl68127),
    I2CDEV(ISL68127, 61, isl68127),
};

STATIC i2c_device_t i2c_56370_devices[] = {
    I2CDEV(LPT, 1, pcf8574),
    I2CDEVTD(NVRAM, 1, eep24c64),
    I2CDEV(MUX, 3, pca9548),
    I2CDEV(LTC3884, 42, ltc3884),
    I2CDEV(LTC3884, 43, ltc3884),
    I2CDEV(LTC2974, 63, ltc2974),   
    I2CDEV(LTC2974, 64, ltc2974),      
};

STATIC i2c_device_t i2c_56275_devices[] = {
    I2CDEV(LPT, 1, pcf8574),      /* I2C address 0x27 (board ID) */
    I2CDEVTD(NVRAM, 1, eep24c64), /* I2C address 0x54 (EEPROM) */
    I2CDEV(MUX, 3, pca9548),      /* I2C address 0x73 (mux 3) */
    I2CDEV(LTM4678, 40, ltm4678),
    I2CDEV(LTC2974, 63, ltc2974),
    I2CDEV(LTC2974, 64, ltc2974),
};

STATIC i2c_device_t i2c_56070_devices[] = {
    I2CDEV(LPT, 1, pcf8574),      /* I2C address 0x27 (board ID) */
    I2CDEVTD(NVRAM, 1, eep24c64), /* I2C address 0x54 (EEPROM) */
    I2CDEV(MUX, 3, pca9548),      /* I2C address 0x73 (mux 3) */
    I2CDEV(MUX, 4, pca9548),      /* I2C address 0x74 (mux 4) */
    I2CDEV(LTC2974, 63, ltc2974),
    I2CDEV(LTC2974, 64, ltc2974),
};

STATIC i2c_device_t i2c_56470_devices[] = {
    I2CDEV(LPT, 1, pcf8574),
    I2CDEVTD(NVRAM, 1, eep24c64),
    I2CDEV(MUX, 3, pca9548),
    I2CDEV(LTC3884, 41, ltc3884),
    I2CDEV(LTC3884, 42, ltc3884),
    I2CDEV(LTC3884, 43, ltc3884),
    I2CDEV(LTC2974, 63, ltc2974),
};
STATIC i2c_device_t *i2c_devices = NULL;

STATIC int num_i2c_devices = 0;

#undef	I2CDEV
#undef	I2CDEVTD

static char *soc_i2c_saddr_to_string_table[0x80] = {
    /* 0X00 */ "Unknown",
    /* 0X01 */ "Unknown",
    /* 0X02 */ "Unknown",
    /* 0X03 */ "Unknown",
    /* 0X04 */ "Unknown",
    /* 0X05 */ "Unknown",
    /* 0X06 */ "Unknown",
    /* 0X07 */ "Unknown",
    /* 0X08 */ "Unknown",
    /* 0X09 */ "Unknown",
    /* 0X0A */ "Unknown",
    /* 0X0B */ "Unknown",
    /* 0X0C */ "Unknown",
    /* 0X0D */ "Unknown",
    /* 0X0E */ "Unknown",
    /* 0X0F */ "Unknown",
    /* 0X10 */ "Unknown",
    /* 0X11 */ "Unknown",
    /* 0X12 */ "Unknown",
    /* 0X13 */ "Unknown",
    /* 0X14 */ "Unknown",
    /* 0X15 */ "Unknown",
    /* 0X16 */ "Unknown",
    /* 0X17 */ "Unknown",
    /* 0X18 */ "MAX6695/MAX6696",
    /* 0X19 */ "MAX6695/MAX6696",
    /* 0X1A */ "Unknown",
    /* 0X1B */ "Unknown",
    /* 0X1C */ "Unknown",
    /* 0X1D */ "Unknown",
    /* 0X1E */ "Unknown",
    /* 0X1F */ "Unknown",
    /* 0X20 */ "lpt0: PCF8574 MUX control",
    /* 0X21 */ "lpt4: PCF8574/Linear Tech 4258 Powered Ethernet (POE)",
#ifdef SHADOW_SVK
    /* 0X22 */ "iop1: PCA9505 I2C to 10Bit IO CNTL",
#else
    /* 0X22 */ "lpt5: PCF8574/Linear Tech 4258 Powered Ethernet (POE)",
#endif
    /* 0X23 */ "lpt2: PCF8574/Linear Tech 4258 Powered Ethernet (POE)",
#ifdef SHADOW_SVK
    /* 0X24 */ "iop0: PCA9505 I2C to 10Bit IO CNTL",
#else
    /* 0X24 */ "lpt6: PCF8574/Linear Tech 4258 Powered Ethernet (POE)",
#endif
    /* 0X25 */ "lpt7: PCF8574/Linear Tech 4258 Powered Ethernet (POE)",
    /* 0X26 */ "lpt3: PCF8574/Linear Tech 4258 Powered Ethernet (POE)",
    /* 0X27 */ "lpt1: PCF8574 or PCA9554 Baseboard ID",
    /* 0X28 */ "adc0: MAX127 A/D Converter",
    /* 0X29 */ "adc1: MAX127 A/D Converter",
    /* 0X2A */ "adc2: MAX127 A/D Converter",
    /* 0X2B */ "adc3: MAX127 A/D Converter",
    /* 0X2C */ "dac0: Linear Technology LTC1427 DAC",
    /* 0X2D */ "pot0: Maxim 5478 Potentiometer",
    /* 0X2E */ "lcd0: Matrix Orbital LCD/MAX6653",
    /* 0X2F */ "ZM7XXX",
    /* 0X30 */ "Unknown",
    /* 0X31 */ "Unknown",
    /* 0X32 */ "Unknown",
    /* 0X33 */ "Unknown",
    /* 0X34 */ "Unknown",
    /* 0X35 */ "Unknown",
    /* 0X36 */ "Unknown",
    /* 0X37 */ "Unknown",
    /* 0X38 */ "poe0: PowerDsine 63000 PoE Microcontroller",
    /* 0X39 */ "Unknown",
    /* 0X3A */ "Unknown",
    /* 0X3B */ "Unknown",
    /* 0X3C */ "Unknown",
    /* 0X3D */ "Unknown",
    /* 0X3E */ "Unknown",
    /* 0X3F */ "Unknown",
    /* 0X40 */ "Unknown",
    /* 0X41 */ I2C_LTC3880_41": LTC 3880 Voltage Controller",
    /* 0X42 */ I2C_LTC3880_42" or I2C_LTC3884_42: LTC 3880/3884 Voltage Controller",
    /* 0X43 */ "I2C_LTM4676_43 or I2C_LTC3880_43 or I2C_LTC3884_43: LTC 3880/3884 Voltage Controller",
    /* 0X44 */ "pcie0:",
    /* 0X45 */ "LTC 3880 Voltage Controller",
    /* 0X46 */ "LTC 3882-0 Voltage Controller or LTC 3884 Voltage Controller",
    /* 0X47 */ "LTC 3882-1 or I2C_LTC3880_47 or I2C_LTM4676_47 Voltage Controller",
    /* 0X48 */ "temp0: LM75 Temperature Sensor",
    /* 0X49 */ "temp1: LM75 Temperature Sensor",
    /* 0X4A */ "temp2: LM75 Temperature Sensor",
    /* 0X4B */ "temp3: LM75 Temperature Sensor",
    /* 0X4C */ "temp0: LM63 Temperature Sensor or temp4: LM75 or MAX664x Temperature Sensor",
    /* 0X4D */ "temp5: LM75 Temperature Sensor",
    /* 0X4E */ "temp6: LM75 Temperature Sensor",
    /* 0X4F */ "temp7: LM75 Temperature Sensor",
    /* 0X50 */ "EEPROM/nvram0/XFP/POE_DB0/CXP/QSFP",
    /* 0X51 */ "Unknown",
    /* 0X52 */ "Unknown",
    /* 0X53 */ "Unknown",
    /* 0X54 */ "nvram1:/CXP ",
    /* 0X55 */ "I2C_LTC3882_55: LTC 3882 Voltage Controller",
    /* 0X56 */ "nvram_56",
    /* 0X57 */ "Unknown",
    /* 0X58 */ "POE_DB1",
    /* 0X59 */ "Unknown",
    /* 0X5A */ "Unknown",
    /* 0X5B */ "Unknown",
    /* 0X5C */ "LTC 2974",
    /* 0X5D */ "Unknown",
    /* 0X5E */ "Unknown",
    /* 0X5F */ "Unknown",
    /* 0X60 */ I2C_LTC3880_60": LTC3880 DC/DC Controller",
    /* 0X61 */ I2C_LTC3880_61": LTC3880 DC/DC Controller",
    /* 0X62 */ "ADP4200-1 or "I2C_LTC3880_62": LTC3880 DC/DC Controller",
    /* 0X63 */ "Unknown",
    /* 0X64 */ "LTC 2974 or "I2C_LTC3880_64": LTC3880 DC/DC Controller",
    /* 0X65 */ "ADP4000-0 or "I2C_LTC3880_65": LTC3880 DC/DC Controller",
    /* 0X66 */ "I2C_LTC3882_66: LTC 3882 Voltage Controller",
    /* 0X67 */ "Unknown",
    /* 0X68 */ "Unknown",
    /* 0X69 */ "pll0: Cypress W229B/W311 Clock Chip",
    /* 0X6A */ "clk0: Cypress W2239x clock Chip",
    /* 0X6B */ "clk0: Cypress W22150 clock Chip",
    /* 0X6C */ "Unknown",
    /* 0X6D */ "Unknown",
    /* 0X6E */ "Unknown",
    /* 0X6F */ "Unknown",
    /* 0X70 */ "PCA9548 MUX or NCP81233-0",
    /* 0X71 */ "PCA9548 MUX",
    /* 0X72 */ "PCA9548 MUX",
    /* 0X73 */ "PCA9548 MUX",
    /* 0X74 */ "PCA9548 MUX",
    /* 0X75 */ "PCA9548 MUX",
#ifdef SHADOW_SVK
    /* 0X76 */ "mux6: PCA9548 i2c Switch",
    /* 0X77 */ "mux7: PCA9548 i2c Switch",
#else
    /* 0X76 */ "PCA9548 MUX",
    /* 0X77 */ "PCA9548 MUX or I2C_LTC3882_77: LTC 3882 Voltage Controller",
#endif
    /* 0X78 */ "Unknown",
    /* 0X79 */ "Unknown",
    /* 0X7A */ "Unknown",
    /* 0X7B */ "Unknown",
    /* 0X7C */ "Unknown",
    /* 0X7D */ "Unknown",
    /* 0X7E */ "Unknown",
    /* 0X7F */ "Unknown"
};

char *soc_i2c_saddr_to_string(int unit, i2c_saddr_t saddr)
{
    return ((unit < 0) ? "" : soc_i2c_saddr_to_string_table[saddr]);
}

/*
 * Function: soc_i2c_device_present
 * Purpose: Probe the I2C bus using saddr, report if a device responds.
 *          The I2C bus is released upon return.
 *          No further action is taken.
 *
 * Parameters:
 *    unit  - StrataSwitch device number or I2C bus number
 *    saddr - I2C slave address
 *
 * Return:
 *     SOC_E_NONE - An I2C device responds at the indicated saddr.
 *     otherwise  - No I2C response at the indicated saddr, or I/O error.
 */
int
soc_i2c_device_present(int unit, i2c_saddr_t saddr)
{
  int rv;

#ifdef BCM_IPROC_SUPPORT
    if(soc_feature(unit, soc_feature_eeprom_iproc) &&
       ((saddr == I2C_NVRAM_SADDR0) || (saddr == I2C_NVRAM_SADDR1))) {
        if ((rv = iproc_smbus_quick_command(unit, saddr)) == SOC_E_NONE) {
            /* An ACK was received, there's an I2C device at saddr */
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "unit %d i2c 0x%x: detected device\n"),
                      unit, saddr));
        }
        return rv;
    }
#endif
#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm)) {
        if ((rv = smbus_quick_command(unit, saddr)) == SOC_E_NONE) {
            /* An ACK was received, there's an I2C device at saddr */
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "unit %d i2c 0x%x: detected device\n"),
                      unit, saddr));
        }
        return rv;
    }
#endif
#ifdef BCM_CMICX_SUPPORT
    if(soc_feature(unit, soc_feature_cmicx)) {
        if ((rv = cmicx_smbus_quick_command(unit, saddr)) == SOC_E_NONE) {
            /* An ACK was received, there's an I2C device at saddr */
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "unit %d i2c 0x%x: detected device\n"),
                      unit, saddr));
        }
        return rv;
    }
#endif
  if ((rv=soc_i2c_start(unit, SOC_I2C_TX_ADDR(saddr))) == SOC_E_NONE) {
    /* An ACK was received, there's an I2C device at saddr */
      LOG_INFO(BSL_LS_SOC_I2C,
               (BSL_META_U(unit,
                           "unit %d i2c 0x%x: detected device\n"),
                unit, saddr));
      /* Release the I2C bus */
      soc_i2c_stop(unit);
  }
  return rv;
}


static i2c_device_t *i2c_custom_devices[SOC_MAX_NUM_DEVICES][MAX_I2C_DEVICES];
static uint8 i2c_custome_dev_tbl_init = 0;
static uint8 switch_to_i2c_custom_dev_tbl[SOC_MAX_NUM_DEVICES];
static int num_custom_i2c_devices[SOC_MAX_NUM_DEVICES];
#define I2C_DEV_DRV(_drv_name)   &_soc_i2c_##_drv_name##_driver

typedef struct i2c_driver_map_info_s {
    char *drv_name;
    i2c_driver_t *drv;
} i2c_driver_map_info_t;

static const i2c_driver_map_info_t driver_mapping_info[] =
{
    {"lm63", I2C_DEV_DRV(lm63)},
    {"lm75", I2C_DEV_DRV(lm75)},
    {"max664x", I2C_DEV_DRV(max664x)},
    {"max127", I2C_DEV_DRV(max127)},
    {"w229b", I2C_DEV_DRV(w229b)},
    {"eep24c64", I2C_DEV_DRV(eep24c64)},
    {"xfp", I2C_DEV_DRV(xfp)},
    {"pcie", I2C_DEV_DRV(pcie)},
    {"pcf8574", I2C_DEV_DRV(pcf8574)},
    {"ltc1427", I2C_DEV_DRV(ltc1427)},
    {"lcd", I2C_DEV_DRV(lcd)},
    {"cy2239x", I2C_DEV_DRV(cy2239x)},
    {"cy22150", I2C_DEV_DRV(cy22150)},
    {"ltc4258", I2C_DEV_DRV(ltc4258)},
    {"pd63000", I2C_DEV_DRV(pd63000)},
    {"max5478", I2C_DEV_DRV(max5478)},
    {"bcm59101", I2C_DEV_DRV(bcm59101)},
    {"pca9548", I2C_DEV_DRV(pca9548)},
    {"adp4000", I2C_DEV_DRV(adp4000)},
    {"cxp", I2C_DEV_DRV(cxp)},
    {"ltc3880", I2C_DEV_DRV(ltc3880)},
    {"ltc3884", I2C_DEV_DRV(ltc3884)},
    {"ltc2974", I2C_DEV_DRV(ltc2974)},
    {"ltm4676", I2C_DEV_DRV(ltm4676)},
    {"pmbus", I2C_DEV_DRV(pmbus)},
};

int
soc_i2c_dev_driver_get(int unit, char *drv_name, i2c_driver_t **drv)
{
    int i;

    for (i = 0; COUNTOF(driver_mapping_info); i++) {
        if (!sal_strcmp(drv_name, driver_mapping_info[i].drv_name)) {
            *drv = driver_mapping_info[i].drv;
            return SOC_E_NONE;
        }
    }
    return SOC_E_NOT_FOUND;
}

/*
 * Check if the device is ever added in custom i2c device descriptor -
 * i2c_custom_devices.
 */
static int
soc_i2c_custom_device_search(int unit, i2c_device_t *device, int *tbl_idx)
{
    i2c_device_t *i2c_dev;
    int i;

    if ((device == NULL) || (tbl_idx == NULL)) {
        return SOC_E_PARAM;
    }
    for (i = 0; i < MAX_I2C_DEVICES; i++) {
        i2c_dev = i2c_custom_devices[unit][i];
        if ((i2c_dev != NULL) && (i2c_dev->saddr == device->saddr)){
            /*
             * Probably have multiple same devices on the system.
             * Check saddr since this should be unique.
             */
            *tbl_idx = i;
            return SOC_E_NONE;
        }
    }
    return SOC_E_NOT_FOUND;
}

/* Add device to custom i2c device descriptor */
int
soc_i2c_custom_device_add(int unit, i2c_device_t *device)
{
    int u, i, tbl_idx;
    int rv = SOC_E_UNAVAIL;

    if (!i2c_custome_dev_tbl_init) {
        for (u = 0; u < SOC_MAX_NUM_DEVICES; u++) {
            for (i = 0; i < MAX_I2C_DEVICES; i++) {
                i2c_custom_devices[u][i] = NULL;
            }
            switch_to_i2c_custom_dev_tbl[u] = 0;
            num_custom_i2c_devices[u] = 0;
        }
        i2c_custome_dev_tbl_init = 1;
    }
    tbl_idx = 0;
    rv = soc_i2c_custom_device_search(unit, device, &tbl_idx);
    if (rv != SOC_E_NOT_FOUND) {
        return rv;
    }

    for (i = 0; i < MAX_I2C_DEVICES; i++) {
        if (i2c_custom_devices[unit][i] == NULL) {
            i2c_custom_devices[unit][i] =
                sal_alloc(sizeof(i2c_device_t),"custom i2c dev");
            if (i2c_custom_devices[unit][i] == NULL) {
                return SOC_E_MEMORY;
            }
            sal_memcpy(i2c_custom_devices[unit][i], device,
                       sizeof(i2c_device_t));
            rv = SOC_E_NONE;
            num_custom_i2c_devices[unit]++;
            break;
        }
    }
    return rv;
}

/* Remove device from custom i2c device descriptor. */
int
soc_i2c_custom_device_remove(int unit, i2c_device_t *device)
{
    int tbl_idx, i;
    i2c_device_t *i2c_dev;

    SOC_IF_ERROR_RETURN(soc_i2c_custom_device_search(unit, device, &tbl_idx));

    i2c_dev = i2c_custom_devices[unit][tbl_idx];
    if (i2c_dev->devname != NULL) {
        sal_free(i2c_dev->devname);
        i2c_dev->devname = NULL;
    }
    sal_free(i2c_dev);
    i2c_custom_devices[unit][tbl_idx] = NULL;

    for (i = tbl_idx; i < (MAX_I2C_DEVICES - 1); i++) {
        if (i2c_custom_devices[unit][i + 1] != NULL) {
            i2c_custom_devices[unit][i] = i2c_custom_devices[unit][i + 1];
        }
    }
    num_custom_i2c_devices[unit]--;
    if (num_custom_i2c_devices[unit] == 0) {
        if (i2c_devices != NULL) {
            sal_free(i2c_devices);
            i2c_devices = NULL;
        }
        switch_to_i2c_custom_dev_tbl[unit] = 0;
    }
    return SOC_E_NONE;
}

typedef struct _soc_i2c_probe_info_s {
    int			devid;
    int			devid_found;
    int			devices_found;
    soc_i2c_bus_t	*i2cbus;
    int                 i2c_nvram_skip;
    int                 i2c_hclk_skip;
    int                 i2c_poe_power;
    int                 i2c_muxed_devid_count[MAX_I2C_DEVICES];
    int                 i2c_mux_stack[MAX_I2C_DEVICES];
    int                 i2c_mux_stack_depth;
} _soc_i2c_probe_info_t;

/*
 * Function: _soc_i2c_probe_device
 * Purpose: Internal probing of a particular I2C device.
 *
 * Parameters:
 *	unit - StrataSwitch device number or I2C bus number
 *	muxed - I2C device search beyond one or more MUX's
 *      i2c_probe_info - data structure of I2C probe parameters
 *
 * Return:
 *	SOC_E_***
 */
int
_soc_i2c_probe_device(int unit, int muxed,
                      _soc_i2c_probe_info_t *i2c_probe_info)
{
    int	devid = i2c_probe_info->devid;
    int rv = SOC_E_NOT_FOUND;
    int mux_stack_index;
    i2c_dev_init_func_t	load;
    char		*devdesc;

    for (mux_stack_index = 0;
         mux_stack_index < i2c_probe_info->i2c_mux_stack_depth;
         mux_stack_index++) {
        if (i2c_probe_info->i2c_mux_stack[mux_stack_index] == devid) {
            /* This is a previous MUX in our tree, skip */
            return rv;
        }
    }

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWK(unit)) {
        if (sal_strcmp(i2c_devices[devid].devname, "pwctrl2") == 0) {
            i2c_devices[devid].saddr = I2C_PWCTRL_2_SADDR60;
        }
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    if (muxed &&
        (NULL != i2c_probe_info->i2cbus->devs[devid]) &&
        (0 == i2c_probe_info->i2c_muxed_devid_count[devid])) {
        /* Top level device, don't probe again during MUX scan. */
        return rv;
    }
    if ((!i2c_probe_info->i2c_poe_power) && /* Not POE */
            ((i2c_devices[devid].driver == &_soc_i2c_pd63000_driver) ||
             (i2c_devices[devid].driver == &_soc_i2c_bcm59101_driver))) {
            return rv;
    } else if (((i2c_probe_info->i2c_poe_power) ||
             (i2c_probe_info->i2c_nvram_skip)) &&
            (i2c_devices[devid].saddr == I2C_NVRAM_SADDR0) &&
            (i2c_devices[devid].driver == &_soc_i2c_eep24c64_driver)) {
            /* check property and skip NVRAM */
            return rv;
    } else if (((i2c_probe_info->i2c_poe_power) ||
                (!i2c_probe_info->i2c_nvram_skip)) &&
               (i2c_devices[devid].saddr == I2C_XFP_SADDR0) &&
               (i2c_devices[devid].driver == &_soc_i2c_xfp_driver)) {
        /* check property and skip XFP */
        return rv;
    } else if ((i2c_probe_info->i2c_hclk_skip) &&
               ((i2c_devices[devid].saddr == I2C_LPT_SADDR2) ||
                (i2c_devices[devid].saddr == I2C_LPT_SADDR3)) &&
               (i2c_devices[devid].driver == &_soc_i2c_pcf8574_driver)) {
        /* check property and skip HCLK */
        return rv;
    } else if ((!i2c_probe_info->i2c_hclk_skip) &&
               ((i2c_devices[devid].saddr == I2C_POE_SADDR2) ||
                (i2c_devices[devid].saddr == I2C_POE_SADDR3) ||
                (i2c_devices[devid].saddr == I2C_POE_SADDR4) ||
                (i2c_devices[devid].saddr == I2C_POE_SADDR6)) &&
               (i2c_devices[devid].driver == &_soc_i2c_ltc4258_driver)) {
        /* check property and skip POE */
        return rv;
    } else if ((i2c_probe_info->i2c_poe_power) &&
               (i2c_devices[devid].saddr == I2C_POE_SADDR0) &&
               (i2c_devices[devid].driver == &_soc_i2c_pd63000_driver)) {
        /* check property and set init power level */
        pd63000_test_data[6] = 0x64;   /* set power level to 100W */
        pd63000_test_data[8] = 0x39;   /* set max volt to 56.9V */
        pd63000_test_data[10] = 0xb7;  /* set min volt to 43.9V */
        pd63000_test_data[11] = 0x13;  /* set delta, guard band */
        pd63000_test_data[14] = 0xdd;  /* update checksum */
    }

    if (SOC_E_NONE ==
        soc_i2c_device_present(unit, i2c_devices[devid].saddr)) {
#ifdef BCM_GREYHOUND_SUPPORT
        if (SOC_IS_GREYHOUND(unit)) {
            int check_devs = 0;
            char *board_name = soc_property_get_str(unit, spn_BOARD_NAME);

            if (board_name != NULL) {
                if ((sal_strcmp(board_name, "BCM953411K") == 0) ||
                        (sal_strcmp(board_name, "BCM953416K") == 0)) {
                    check_devs = 5;
                }
                else if (sal_strcmp(board_name, "BCM953406K") == 0) {
                    check_devs = 4;
                }

                /* filter the pre-defined devices which addr are confict to */
                /* the ones on GH SVK board */
                if (check_devs > 0) {
                    if (i2c_devices[devid].driver != &_soc_i2c_ltc3880_driver) {
                        if ((i2c_devices[devid].saddr == I2C_LTC3880_SADDR60) ||
                                (i2c_devices[devid].saddr == I2C_LTC3880_SADDR61) ||
                                (i2c_devices[devid].saddr == I2C_LTC3880_SADDR62) ||
                                (i2c_devices[devid].saddr == I2C_LTC3880_SADDR64)) {
                            return SOC_E_NOT_FOUND;
                        }
                        if (check_devs > 4) {
                            if (i2c_devices[devid].saddr == I2C_LTC3880_SADDR65) {
                                return SOC_E_NOT_FOUND;
                            }
                        }
                    }
                }
            }
        }
#endif

        /* An ACK was received, there's a device at this SADDR.      */
        /* We infer the device type solely by its I2C slave address. */

        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "unit %d i2c 0x%x: found %s: %s\n"),
                  unit,
                  i2c_devices[devid].saddr,
                  i2c_devices[devid].devname,
                  i2c_devices[devid].desc ?
                  i2c_devices[devid].desc : ""));

        /* Load the device driver */
        i2c_probe_info->i2cbus->devs[devid] = &i2c_devices[devid];

        /* Else, if the device is beyond one or more MUX's, then we
         * need to check it is present and initialize each time
         * it is found.
         */

        /* set flags, device id, hook up bus, etc  */
        if (i2c_devices[devid].driver) {
            i2c_devices[devid].driver->flags |= I2C_DEV_OK;
            i2c_devices[devid].driver->flags |= I2C_REG_STATIC;
            i2c_devices[devid].driver->devno = devid;
        }

        /* Load device if init function exists */

        load = i2c_devices[devid].driver->load;
        if (load) {
            if ((rv = load(unit,
                           devid,
                           i2c_devices[devid].testdata,
                           i2c_devices[devid].testlen)) < 0) {
                LOG_INFO(BSL_LS_SOC_I2C,
                         (BSL_META_U(unit,
                                     "unit %d i2c 0x%x: init failed %s - %s\n"),
                          unit,
                          i2c_devices[devid].saddr,
                          i2c_devices[devid].devname,
                          soc_errmsg(rv)));
            } else {
                LOG_INFO(BSL_LS_SOC_I2C,
                         (BSL_META_U(unit,
                                     "unit %d i2c: Loaded driver for 0x%02x - %s\n"),
                          unit, devid, i2c_devices[devid].devname));
           }
           if ((rv == SOC_E_NOT_FOUND) && 
                ((i2c_devices[devid].driver == &_soc_i2c_ltc3880_driver) ||
                (i2c_devices[devid].driver == &_soc_i2c_ltc3882_driver) ||
                (i2c_devices[devid].driver == &_soc_i2c_ltc3884_driver))) {
                /* Not loading the expected device driver, unset device driver */
                i2c_probe_info->i2cbus->devs[devid] = NULL;
                return rv;
           }
        }
        soc_i2c_devdesc_get(unit, devid, &devdesc);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d i2c 0x%x %s: %s\n"),
                     unit,
                     i2c_devices[devid].saddr,
                     i2c_devices[devid].devname,
                     devdesc));

        /* Only count a device type once, but each device instance */
        i2c_probe_info->devices_found += 1;
        if (!muxed){
            i2c_probe_info->devid_found += 1;
        } else {
            if (0 == i2c_probe_info->i2c_muxed_devid_count[devid]) {
                i2c_probe_info->devid_found += 1;
            }
            i2c_probe_info->i2c_muxed_devid_count[devid] += 1;
        }
        rv = SOC_E_EXISTS;
    } else if (!muxed) {  /* Don't clear info when scanning beyond MUX's */
        /* I2C device not present */
        i2c_probe_info->i2cbus->devs[devid] = NULL;
    }
    return rv;
}

/*
 * Function: _soc_i2c_probe_mux
 * Purpose: Internal probing of a given I2C MUX.
 *
 * Parameters:
 *	unit - StrataSwitch device number or I2C bus number
 *      mux_devid_range - I2C device ID for MUX to scan, -1 for all
 *      i2c_probe_info - data structre of I2C probe parameters
 *
 * Return:
 *	SOC_E_***
 */
int
_soc_i2c_probe_mux(int unit, int mux_devid_range,
                   _soc_i2c_probe_info_t *i2c_probe_info)
{
    int			devid, mux_devid;
    int			mux_channel;
    int                 rv = SOC_E_NONE;
    uint8               mux_data;
    int                 mux_devid_min, mux_devid_max;

    if (mux_devid_range < 0) {
        mux_devid_min = 0;
        mux_devid_max = num_i2c_devices - 1;
    } else {
        mux_devid_min = mux_devid_range;
        mux_devid_max = mux_devid_range;
    }

    /* Now scan down the MUX tree */
    for (mux_devid = mux_devid_min;
         mux_devid <= mux_devid_max;
         mux_devid++) {
        if (i2c_devices[mux_devid].driver != &_soc_i2c_pca9548_driver) {
            /* Only the MUX's are interesting here. */
            continue;
        }
        if (NULL == i2c_probe_info->i2cbus->devs[mux_devid]) {
            /* We didn't find a MUX of this type */
            continue;
        }

        if ((0 == i2c_probe_info->i2c_mux_stack_depth) && /* Top level */
            (0 != i2c_probe_info->i2c_muxed_devid_count[mux_devid])) {
            /* We found this MUX type at a lower level of the tree */
            continue;
        }
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "unit %d i2c: Detected MUX 0x%02x - %s\n"),
                  unit, mux_devid,
                  i2c_devices[mux_devid].devname));
        for (mux_channel = 0; mux_channel < PCA9548_CHANNEL_NUM;
             mux_channel++) {
            /* Set the channel so we can probe beyond it */
            mux_data = (1 << mux_channel);
            if ((rv = i2c_devices[mux_devid].driver->write(unit, mux_devid,
                                         0, &mux_data, 1)) < 0) {
                LOG_INFO(BSL_LS_SOC_I2C,
                         (BSL_META_U(unit,
                                     "unit %d i2c: Could not assign channel %d to %s\n"),
                          unit, mux_channel,
                          i2c_devices[mux_devid].devname));
                return rv;
            }
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "unit %d i2c: Set channel %d of MUX 0x%02x - %s\n"),
                      unit, mux_channel, mux_devid,
                      i2c_devices[mux_devid].devname));
            for (devid = 0; devid < num_i2c_devices; devid++) {
                i2c_probe_info->devid = devid;
                rv = _soc_i2c_probe_device(unit, TRUE, i2c_probe_info);
                if (SOC_E_EXISTS == rv) {
                    /* Is it a MUX? Recurse! */
                    if (i2c_devices[devid].driver ==
                        &_soc_i2c_pca9548_driver) {
                        /* Record MUX stack to get here */
                        i2c_probe_info->i2c_mux_stack[i2c_probe_info->i2c_mux_stack_depth] = devid;
                        i2c_probe_info->i2c_mux_stack_depth++;
                        rv = _soc_i2c_probe_mux(unit, devid,
                                                i2c_probe_info);
                        if (SOC_FAILURE(rv)) {
                            /* Access error */
                            return rv;
                        }
                    }
                    rv = SOC_E_NONE;
                } else if ((SOC_E_NOT_FOUND == rv) || (SOC_E_INIT == rv)) {
                    /* No problem */
                    rv = SOC_E_NONE;
                } else if (SOC_FAILURE(rv)) {
                    /* Access error */
                    return rv;
                }
            }
        }

        /* Restore default channel 0 on the MUX now that
         * the probe is complete */
        mux_channel = 0;
        mux_data = (1 << mux_channel);
        if ((rv = i2c_devices[mux_devid].driver->write(unit, mux_devid,
                                                       0, &mux_data, 1)) < 0) {
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "unit %d i2c: Could not assign channel %d to %s\n"),
                      unit, mux_channel,
                      i2c_devices[mux_devid].devname));
            return rv;
        }
    }

    i2c_probe_info->i2c_mux_stack_depth--;
    i2c_probe_info->i2c_mux_stack[i2c_probe_info->i2c_mux_stack_depth] = -1;
    return rv;
}

/*
 * Function: soc_i2c_probe
 * Purpose: Probe I2C devices on bus, report devices found.
 *          This routine will walk through our internal I2C device driver
 *          tables, attempt to find the device on the I2C bus, and if
 *          successful, register a device driver for that device.
 *
 *          This allows for the device to be used in an API context as
 *          when the devices are found, the device driver table is filled
 *          with the correct entries for that device (r/w function, etc).
 *
 * Parameters:
 *	unit - StrataSwitch device number or I2C bus number
 *
 * Return:
 *	count of found devices or SOC_E_***
 */
int
soc_i2c_probe(int unit)
{
    _soc_i2c_probe_info_t i2c_probe_info;
    int			devid;
    int         rv;
    int			mux_devid;
    uint8       mux_data, board_id;
    uint8       *data;

    /* Make sure that we're already attached, or go get attached */
    if (!soc_i2c_is_attached(unit)) {
	return soc_i2c_attach(unit, 0, 0);
    }

    if (num_custom_i2c_devices[unit]) {
        if ((i2c_devices != NULL) && switch_to_i2c_custom_dev_tbl[unit]) {
            sal_free(i2c_devices);
            i2c_devices = NULL;
        }
        i2c_devices = sal_alloc(sizeof(i2c_device_t) *
                                num_custom_i2c_devices[unit],
                                "custome i2c devices descriptor");
        if (i2c_devices == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(i2c_devices, 0,
                   sizeof(i2c_device_t) * num_custom_i2c_devices[unit]);
        for (devid = 0; devid < num_custom_i2c_devices[unit]; devid++) {
            sal_memcpy(&i2c_devices[devid], i2c_custom_devices[unit][devid],
                       sizeof(i2c_device_t));
        }
        switch_to_i2c_custom_dev_tbl[unit] = 1;
        num_i2c_devices = num_custom_i2c_devices[unit];
    } else {
        /* For 56560K, 56760K, 56565K read the Board ID directly before probing
         * and load their respective device descriptor tables.
         * This is a new approach to avoid conflicts with other boards
         * which may have different I2C devices at the same slave address.
         */
        data = &board_id;
        (void)soc_i2c_read_byte(unit, I2C_LPT_SADDR1, data);
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "I2C board ID 0x%x\n"),
                  board_id ));
        switch (board_id) {
            case 0x94: i2c_devices = i2c_56760_devices;
                       num_i2c_devices = COUNTOF(i2c_56760_devices);
                       break; /*56560K, 56760K */
            case 0x96: i2c_devices = i2c_56565_devices;
                       num_i2c_devices = COUNTOF(i2c_56565_devices);
                       break; /*56565K */
            case 0x98: i2c_devices = i2c_56270_devices;
                       num_i2c_devices = COUNTOF(i2c_56270_devices);
                       break; /* 56270K */
            case 0x99: i2c_devices = i2c_56272_devices;
                       num_i2c_devices = COUNTOF(i2c_56272_devices);
                       break; /* 56272K */
            case 0x9D: /* TH2 Simplified SVK board */
            case 0xA3: /* TH2 DVT board */
            case 0x9F: i2c_devices = i2c_56970_32_devices;
                       num_i2c_devices = COUNTOF(i2c_56970_32_devices);
                       break; /* 56970K */
            case 0x9E: i2c_devices = i2c_56970_64_devices;
                       num_i2c_devices = COUNTOF(i2c_56970_64_devices);
                       break; /* 56970K */
            case 0xA4: /* TD3 SVK */
            case 0xA5: /* TD3 simplified SVK */
            case 0xA6: /* TD3 56873K /MV2 56770K*/
            case 0xB5: /* MV2 DVT board*/
            case 0xA7: i2c_devices = i2c_56870_devices;
                       num_i2c_devices = COUNTOF(i2c_56870_devices);
                       break; /* TD3 DVT board */
            case 0xAA: i2c_devices = i2c_56670_devices;
                       num_i2c_devices = COUNTOF(i2c_56670_devices);
                       break; /* MONTEREY DVT board */
            case 0xB1:
            case 0xAD:
            case 0xAF:
            case 0xDE:
            case 0xAC: i2c_devices = i2c_56980_devices;
                       num_i2c_devices = COUNTOF(i2c_56980_devices);
                       break; /* 56980K */
            case 0xB0:
            case 0xB3:
            case 0xB6: i2c_devices = i2c_56370_devices;
                       num_i2c_devices = COUNTOF(i2c_56370_devices);
                       break; /* 56370K */
            case 0xC0: i2c_devices = i2c_56470_devices;
                       num_i2c_devices = COUNTOF(i2c_56470_devices);
                       break; /* 56470K */
            case 0xBB:
            case 0xBC:
            case 0xBF: i2c_devices = i2c_56275_devices;
                       num_i2c_devices = COUNTOF(i2c_56275_devices);
                       break; /* 56275K */
            case 0xC2: i2c_devices = i2c_56070_devices;
                       num_i2c_devices = COUNTOF(i2c_56070_devices);
                       break; /* 56070K */
            default:   i2c_devices = i2c_common_devices;
                       num_i2c_devices = COUNTOF(i2c_common_devices);
                       break;
        }
    }
    /* Check for supported I2c devices */
    if (num_i2c_devices > MAX_I2C_DEVICES) {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "ERROR: %d exceeds supported I2C devices\n"),
                  num_i2c_devices ));
        return SOC_E_INTERNAL;
    }

    i2c_probe_info.i2cbus = I2CBUS(unit);

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "unit %d i2c: probing %d I2C devices.\n"),
              unit, num_i2c_devices));

    /* Attempt to contact known slave devices, hookup driver and
     * run device init function if the device exists, otherwise
     * simply acknowledge the device can be found.
     */

    i2c_probe_info.i2c_nvram_skip =
        soc_property_get(unit, spn_I2C_NVRAM_SKIP, 0);
    i2c_probe_info.i2c_hclk_skip =
        soc_property_get(unit, spn_I2C_HCLK_SKIP, 0);
    i2c_probe_info.i2c_poe_power =
        soc_property_get(unit, spn_I2C_POE_POWER, 0);

    i2c_probe_info.devid_found = 0; /* Installed I2C device types */
    i2c_probe_info.devices_found = 0; /* Number of installed I2C devices */

    /* Initialize device table with none found. */
    for (devid = 0; devid < num_i2c_devices; devid++) {
        /* I2C device not present */
        i2c_probe_info.i2cbus->devs[devid] = NULL;
        i2c_probe_info.i2c_muxed_devid_count[devid] = 0;
        i2c_probe_info.i2c_mux_stack[devid] = -1;
    }
    i2c_probe_info.i2c_mux_stack_depth = 0;

    /* First, disable all visible MUX's. */
    for (mux_devid = 0; mux_devid < num_i2c_devices; mux_devid++) {
        if (i2c_devices[mux_devid].driver != &_soc_i2c_pca9548_driver) {
            /* Only the MUX's are interesting here. */
            continue;
        }
        i2c_probe_info.devid = mux_devid;
        rv = _soc_i2c_probe_device(unit, FALSE, &i2c_probe_info);
        if (SOC_E_EXISTS == rv) {
            mux_data = 0; /* Shut down ALL channels */
            if ((rv = i2c_devices[mux_devid].driver->write(unit, mux_devid,
                                             0, &mux_data, 1)) < 0) {
                LOG_INFO(BSL_LS_SOC_I2C,
                         (BSL_META_U(unit,
                                     "unit %d i2c: Could not disable channels on %s\n"),
                          unit, i2c_devices[mux_devid].devname));
                return rv;
            } else {
                /* MUX disabled */
                rv = SOC_E_NONE;
            }
        } else if ((SOC_E_NOT_FOUND == rv) || (SOC_E_INIT == rv)) {
            /* No error */
            rv = SOC_E_NONE;
        } else if (SOC_FAILURE(rv)) {
            /* Access error */
            return rv;
        }
    }

    /* Reset after MUX ops */
    i2c_probe_info.devid_found = 0;
    i2c_probe_info.devices_found = 0;

    /* Probe the currently configured (i.e. MUX'd) I2C bus. */
    for (devid = 0; devid < num_i2c_devices; devid++) {
        /* Detect device type */
        i2c_probe_info.devid = devid;
        rv = _soc_i2c_probe_device(unit, FALSE, &i2c_probe_info);
        if ((SOC_E_EXISTS == rv) || (SOC_E_NOT_FOUND == rv)
            || (SOC_E_INIT == rv)) {
            /* No problem */
            rv = SOC_E_NONE;
        } else if (SOC_FAILURE(rv)) {
            /* Access error */
            return rv;
        }
    }

    /* Now we know all of the devices that aren't behind a MUX.
     * Anything new we find now must be behind a MUX.
     * Recursively scan down the MUX tree.
     */
    if ((rv = _soc_i2c_probe_mux(unit, -1, &i2c_probe_info)) < 0) {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "unit %d i2c: Could not probe MUX's\n"), unit));
        return rv;
    }

    return i2c_probe_info.devid_found;
}

/*
 * Function: soc_i2c_device_count
 *
 * Purpose: Report the number of devices registered
 *          in the system. For now, this is the total number of devices
 *          we have added to the statically defined device descriptor
 *          array above.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *
 * Returns:
 *        number of devices registered in the system device table.
 *
 * Notes: Currently, we do not support dynamic device loading.
 *        Later, one will be able add a driver to the device table,
 *        without the STATIC attribute.
 */
int
soc_i2c_device_count(int unit)
{
    COMPILER_REFERENCE(unit);
    return num_i2c_devices;
}

/*
 * Function: soc_i2c_show
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *
 * Purpose: Show all valid devices and their attributes and
 *          statistics.
 * Returns:
 *    none
 * Notes:
 *    none
 */
void
soc_i2c_show(int unit)
{
    int dev;
    soc_i2c_bus_t	*i2cbus;
    i2c_device_t	*i2cdev;

#ifdef BCM_APACHE_SUPPORT
    /* Temporary Output for "i2c show" command in BCMSIM
       Shows info of primary mux
     */
    if (SOC_IS_APACHE(unit) && SAL_BOOT_BCMSIM) {
        LOG_CLI((BSL_META_U(unit,
                            "%15s: %s%s saddr=0x%02x \n"),
                 I2C_MUX_3,
                 " (detached) ",
                 "PCA9548 i2c switch",
                 I2C_MUX_SADDR3));
        return;
    }
#endif

    /* Make sure that we're already attached, or go get attached */
    if ( !soc_i2c_is_attached(unit) &&
	 (soc_i2c_attach(unit, 0, 0) < 0) ) {
        LOG_CLI((BSL_META_U(unit,
                            "unit %d soc_i2c_show: error attaching to I2C bus\n"),
                 unit));
        return;
    }

    i2cbus = I2CBUS(unit);

    i2cbus->txBytes = i2cbus->rxBytes = 0;

    LOG_CLI((BSL_META_U(unit,
                        "unit %d i2c  bus: mode=%s speed=%dkbps "
                        "SOC_address=0x%02X\n"),
             unit,
             i2cbus->flags&SOC_I2C_MODE_PIO?"PIO":"INTR",
             i2cbus->frequency/1000,
             i2cbus->master_addr));

    for (dev = 0; dev < num_i2c_devices; dev++) {
	i2cdev = i2cbus->devs[dev];
	if (i2cdev == NULL) {
	    continue;
	}
	LOG_CLI((BSL_META_U(unit,
                            "%15s: %s%s saddr=0x%02x \n"),
                 i2cdev->devname,
                 i2cdev->driver ? "" : " (detached)",
                 i2cdev->desc,
                 i2cdev->saddr));

	if (i2cdev->driver && (i2cdev->rbyte != 0 || i2cdev->tbyte != 0)) {
	    LOG_CLI((BSL_META_U(unit,
                                "                 received %d bytes, "
                                "transmitted %d bytes\n"),
                     i2cdev->rbyte, i2cdev->tbyte));
	    i2cbus->txBytes += i2cdev->tbyte;
	    i2cbus->rxBytes += i2cdev->rbyte;
	}
    }
    LOG_CLI((BSL_META_U(unit,
                        "unit %d i2c  bus: received %d bytes, "
                        "transmitted %d bytes\n"),
             unit, i2cbus->rxBytes, i2cbus->txBytes));
}

/*
 * Function: soc_i2c_addr
 *
 * Purpose: Return slave address of specified device.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devid - I2C device id returned from soc_i2c_devopen
 *
 * Returns:
 *    7-bit or 10-bit I2C slave address of device, in "datasheet-speak"
 *
 * Notes:
 *    none
 */
i2c_saddr_t
soc_i2c_addr(int unit, int devid)
{
    soc_i2c_bus_t	*i2cbus;

    i2cbus = I2CBUS(unit);
    if (i2cbus && i2cbus->devs[devid]) {
	return i2cbus->devs[devid]->saddr;
    }
    return 0;
}

/*
 * Function: soc_i2c_devname
 *
 * Purpose: Return device name of specified device.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devid - I2C device id returned from soc_i2c_devopen
 *
 * Returns:
 *    character string name of device
 *
 * Notes:
 *    none
 */
const char *
soc_i2c_devname(int unit, int devid)
{
    soc_i2c_bus_t	*i2cbus;

    i2cbus = I2CBUS(unit);
    if (i2cbus && i2cbus->devs[devid]) {
	return i2cbus->devs[devid]->devname;
    }
    return NULL;
}

int
soc_i2c_devdesc_set(int unit, int devid, char *desc)
{
    soc_i2c_bus_t	*i2cbus;

    i2cbus = I2CBUS(unit);
    if (i2cbus && i2cbus->devs[devid]) {
	i2cbus->devs[devid]->desc = desc;
    }
    return SOC_E_NONE;
}

int
soc_i2c_devdesc_get(int unit, int devid, char **desc)
{
    soc_i2c_bus_t	*i2cbus;

    i2cbus = I2CBUS(unit);
    if (i2cbus && i2cbus->devs[devid]) {
	*desc = i2cbus->devs[devid]->desc;
    } else {
	*desc = "";
    }
    return SOC_E_NONE;
}

/*
 * Function: soc_i2c_device
 *
 * Purpose: Returns the device structure associated with the bus and
 *          device identifier.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devid - I2C device id returned from soc_i2c_devopen
 *
 * Returns:
 *    I2C device descriptor
 *
 * Notes:
 *    none
 */
i2c_device_t *
soc_i2c_device(int unit, int devid)
{
    soc_i2c_bus_t	*i2cbus;

    i2cbus = I2CBUS(unit);
    if (i2cbus && i2cbus->devs[devid]) {
	return i2cbus->devs[devid];
    }
    return NULL;
}

/*
 * Function: soc_i2c_devtype
 *
 * Purpose: Return the device driver type, this is an integer value
 *          associated with the driver to determine ownership of the device.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devid - I2C device id returned from soc_i2c_devopen
 *
 * Returns:
 *    I2C device type code (from driver)
 *
 * Notes:
 *    Every device should have a unique Device type indentifier.
 */
int
soc_i2c_devtype(int unit, int devid)
{
    soc_i2c_bus_t	*i2cbus;

    i2cbus = I2CBUS(unit);
    if (i2cbus && i2cbus->devs[devid] && i2cbus->devs[devid]->driver) {
	return i2cbus->devs[devid]->driver->id;
    }
    return 0;
}

/*
 *
 * Function: soc_i2c_is_attached
 *
 * Purpose:  Return true if I2C bus driver is attached.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *
 * Returns:
 *    True if the bus driver for I2C is attached.
 *
 * Notes:
 *    None
 */
int
soc_i2c_is_attached(int unit)
{
    uint32 dev_idx;
    uint8 found = 0;
    soc_i2c_bus_t	*i2cbus;

    for(dev_idx = 0; dev_idx < soc_ndev ; ++dev_idx) {
        if(SOC_NDEV_IDX2DEV(dev_idx) == unit) {
            found = 1;
            break;
        }
    }
    if (!found) {
        return 0;
    }

    i2cbus = I2CBUS(unit);
    if (i2cbus == NULL) {
	    return 0;
    }
    return (i2cbus->flags & SOC_I2C_ATTACHED) != 0;
}

/*
 * Function: soc_i2c_devopen
 *
 * Purpose:  Open device, return valid file descriptor or -1 on error.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devname - I2C device name string
 *    flags - arguments to pass to attach, default value should be zero
 *    speed - I2C bus speed, if non-zero, this speed is configured, normally
 *            this argument should be zero unless a speed is desired.
 * Returns:
 *      device identifier for all I2C operations
 *
 * Notes:
 *      This routine should be called before attempting to communicate
 *      with an I2C device which has a registered driver.
 */
int
soc_i2c_devopen(int unit, char* devname, uint32 flags, int speed)
{
    int devid, rv;
    soc_i2c_bus_t	*i2cbus;

    /* Make sure that we're already attached, or go get attached */
    if ( !soc_i2c_is_attached(unit) &&
	 ((rv = soc_i2c_attach(unit, flags, speed)) < 0) ) {
        return rv;
    }

    i2cbus = I2CBUS(unit);
    for (devid = 0; devid < num_i2c_devices; devid++) {
    	if (i2cbus->devs[devid]) {
    	    if (sal_strcmp(i2cbus->devs[devid]->devname, devname) == 0) {
    		return devid;
    	    }
    	}
    }
    return SOC_E_NOT_FOUND;
}

/*
 * Function: soc_i2c_get_devices
 *
 * Purpose: Return an array of I2C devices; fill in the count.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    count - address where the count of devices should be stored.
 *
 * Returns:
 *      device descriptor array
 *
 * Notes:
 *      NYI
 */
i2c_device_t *
soc_i2c_get_devices(int unit, int* count)
{
    return NULL;
}

/*
 * Function: soc_i2c_register
 * Purpose : Register a driver with the systems internal table.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devname - device name
 *    device - I2C device descriptor
 *
 * Returns:
 *      SOC_E_NONE - device is registered
 *
 * Notes:
 *      NYI
 */
int
soc_i2c_register(int unit, char* devname, i2c_device_t* device)
{
    return SOC_E_UNAVAIL;
}

/*
 * Function: soc_i2c_unload_devices
 * Purpose: Unload I2C devices on bus, hook to I2C device driver uload function.
 *
 * Parameters:
 *          unit - StrataSwitch device number or I2C bus number
 *
 * Return:
 *          SOC_E_NONE
 */
int
soc_i2c_unload_devices(int unit)
{
    int dev;
    soc_i2c_bus_t *i2cbus;
    i2c_device_t *i2cdev;
    i2c_dev_deinit_func_t unload;

    i2cbus = I2CBUS(unit);

    if (i2cbus != NULL) {
        for (dev = 0; dev < num_i2c_devices; dev++) {
            i2cdev = i2cbus->devs[dev];
            if (i2cdev == NULL) {
                continue;
            }
            unload = i2cdev->driver->unload;
            if (unload) {
                (void)unload(unit, dev);
            }
        }
	}

    return SOC_E_NONE;
}

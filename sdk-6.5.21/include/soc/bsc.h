/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Constants and defines for the Broadcom GTO (BCM98548) I2C
 * Bus Driver interface controller operating in master mode.
 *
 * See also: Broadcom StrataSwitch (TM) Register Reference Guide
 */

#ifndef _SOC_BSC_H
#define _SOC_BSC_H

#ifdef	INCLUDE_I2C

#include <sal/compiler.h>
#include <sal/core/sync.h>

#define	BSCBUS(_u)	((soc_bsc_bus_t *)(SOC_CONTROL(_u)->bsc_bus))

#define BSC_WRITE_ONLY		0
#define BSC_READ_ONLY		1
#define BSC_READ_THEN_WRITE	2
#define BSC_WRITE_THEN_READ	3

#define BSC_DIV_CLK		(1 << 7)

#define BSC_SCL_SEL_375KHZ	(0 << 4)
#define BSC_SCL_SEL_390KHZ	(1 << 4)
#define BSC_SCL_SEL_187KHZ	(2 << 4)
#define BSC_SCL_SEL_200KHZ	(3 << 4)

#define BSC_LOCK(unit) \
	sal_mutex_take(BSCBUS(unit)->bsc_mutex, sal_mutex_FOREVER)
#define BSC_UNLOCK(unit) \
	sal_mutex_give(BSCBUS(unit)->bsc_mutex)

#define MAX_BSC_DEVICES     48

/* Temperature Sensor and Fan Controller */
#define MAX6653_DEVICE_TYPE	0x00000007 /* Device ID */
#define BSC_MAX6653_SADDR	0x2e

#define BSC_MAX6653_DEVID_OFF	0x3d	/* device id register offset */
#define BSC_MAX6653_MANUF_OFF	0x3e	/* manufacturer id register offset */

#define BSC_MAX6653_DEVID	0x38	/* device ID */
#define BSC_MAX6653_MANUF	0x4d	/* manufacturer ID */

/* Philips 8-channel I2C Mux */
#define BSC_PCA9548_SADDR	0x73
#define BSC_PCA9548_SADDR_1	0x71
#define BSC_PCA9548_SADDR_2	0x72
#define BSC_PCA9548_SADDR_3	BSC_PCA9548_SADDR
#define BSC_PCA9548_CHAN_MAX	8
#define BSC_PCA9548_CHAN_0	(1 << 0)
#define BSC_PCA9548_CHAN_1	(1 << 1)
#define BSC_PCA9548_CHAN_2	(1 << 2)
#define BSC_PCA9548_CHAN_3	(1 << 3)
#define BSC_PCA9548_CHAN_4	(1 << 4)
#define BSC_PCA9548_CHAN_5	(1 << 5)
#define BSC_PCA9548_CHAN_6	(1 << 6)
#define BSC_PCA9548_CHAN_7	(1 << 7)

/* Digital Power Manager */
#define ZM7300_DEVICE_TYPE	0x00000008 /* Device ID */
#define BSC_ZM73XX_SADDR	0x2f

/*
 * Atmel EEPROM At24C32
 *
 * The EEPROM device on board slave address depends on which
 * slot the board is inserted in.  There are total 4 slots
 * and they are refleted at the lowest 2-bit.
 */
#define BSC_AT24C32_SADDR	0x50
#define BSC_AT24C32_NAME	"at24c64"

#define AT24C32_DEVICE_TYPE	0x00000009 /* Device ID */


#define LC2464_NAME_LEN            10

/* Coverity :: 21386 */
typedef struct eep24c64_s{
        uint16 size;   /* Number of Bytes */
        uint16 type;   /* Reserved */
        char name[LC2464_NAME_LEN + 1];  /* Device name */
        uint16 chksum; /* 16 bit checksum of size, type, and name */
} eep24c64_t;

/*
 * Store EEPROM data at start of prom, make all
 * drivers read/write beyond that. This basically
 * serves as a way of signing the EEPROM with data
 * so that we know the device is good and has been
 * validated.
 */
#define AT24C64_DEVICE_SIZE	(8192) /* AT24C64 */
#define AT24C64_DEVICE_RW_SIZE	((AT24C64_DEVICE_SIZE * 3) / 4)
#define AT24C64_PARAMS_SIZE	(sizeof(eep24c64_t))
#define AT24C64_CONFIG_START	(AT24C64_DEVICE_RW_SIZE - AT24C64_PARAMS_SIZE)

/*
 * SFP Fiber module
 */
#define BSC_SFP_SADDR	0x50
#define BSC_SFP_NAME	"sfp"
#define BSC_SFP_00_NAME	"sfp0"
#define BSC_SFP_01_NAME	"sfp1"
#define BSC_SFP_02_NAME	"sfp2"
#define BSC_SFP_03_NAME	"sfp3"
#define BSC_SFP_04_NAME	"sfp4"
#define BSC_SFP_05_NAME	"sfp5"
#define BSC_SFP_06_NAME	"sfp6"
#define BSC_SFP_07_NAME	"sfp7"
#define BSC_SFP_08_NAME	"sfp8"
#define BSC_SFP_09_NAME	"sfp9"
#define BSC_SFP_10_NAME	"sfp10"
#define BSC_SFP_11_NAME	"sfp11"

#define SFP_DEVICE_TYPE	0x0000000A /* Device ID */
#define SFP_DEVICE_SIZE	(256) /* SFP */

typedef uint8 bsc_saddr_t;     /* Device address in datasheet-speak */
typedef uint8 bsc_bus_addr_t;  /* Address byte on the I2C SDA line  */

/*
 * BSC Driver i/f routines.
 */
typedef int (*bsc_read_func_t)(int unit, int devno, uint32 reg, uint32 *data);
typedef int (*bsc_write_func_t)(int unit, int devno, uint32 reg, uint32 data);
typedef int (*bsc_ioc_func_t)(int unit, int devno, int opcode, void* data, int len);
typedef int (*bsc_dev_init_func_t)(int unit, int devno);

/*
 * I2C Driver structure: definitions for managed device operations
 */
typedef struct bsc_driver_s {
	uint8 flags;			/* Device flags */
	int devno;			/* Index into device descriptor table */
	uint32 id;			/* Device Serial No, GUID, or identifier */
	bsc_read_func_t read;		/* Read routine */
	bsc_write_func_t write;		/* Write routine */
	bsc_ioc_func_t ioctl;		/* IO control routine */
	bsc_dev_init_func_t init;	/* Called once at startup */
} bsc_driver_t;

/*
 * Command definition for ioctl
 */
#define BSC_IOCTL_DEVICE_PRESENT	1

/*
 * I2C Device Descriptor: One for every known device on the bus
 */
typedef struct bsc_dev_s {
	char *devname;			/* Device name, eg. "eeprom0" */
	int mux;			/* i2c mux address */
	int chan;			/* attached to mux channel */
	bsc_saddr_t saddr;		/* Slave address */
	bsc_driver_t *driver;		/* Driver routines */
	char *desc;			/* Description */
	uint32 tbyte;			/* Bytes transmitted */
	uint32 rbyte;			/* Bytes received */
	void *sc;			/* device local data structure */
} bsc_device_t;

/*
 * I2C BUS configuration parameters, one per Switch on a Chip (SOC) device
 */
typedef struct soc_bsc_bus_s {
	uint32 flags;				/* Bitmask of state : see below */
	sal_mutex_t bsc_mutex;			/* BSC state mutual exclusion */
	int mon_delay;				/* Monitor or sensor period */
	bsc_device_t *devs[MAX_BSC_DEVICES];	/* devices on this bus */
} soc_bsc_bus_t;

/* Bus Controller flag bit values and their meanings */
#define SOC_BSC_DETACHED    0x00 /* No status */
#define SOC_BSC_MODE_PIO    0x01 /* PIO mode */
#define SOC_BSC_MODE_INTR   0x02 /* Interrupt mode */
#define SOC_BSC_ATTACHED    0x04 /* Driver attached */

extern int soc_bsc_addr(int unit, int devno);
extern const char * soc_bsc_devname(int unit, int devno);
extern int soc_bsc_device_count(int unit);
extern bsc_device_t *soc_bsc_device(int unit, int devno);
extern int soc_bsc_devtype(int unit, int devno);
extern int soc_bsc_devopen(int unit, char* devname);
extern int soc_bsc_devdesc_set(int unit, int devno, char *desc);
extern int soc_bsc_devdesc_get(int unit, int devno, char **desc);

/*
 * MAX6653 Device Driver Temperature and Fan probe and display
 * See: drv/i2c/max6653.c
 */
extern void soc_bsc_max6653_show(int unit);
extern void soc_bsc_max6653_watch(int unit, int enable, int nsecs);
extern int soc_bsc_max6653_set(int unit, int devno, int speed);

extern int soc_bsc_zm73xx_show(int unit);
extern void soc_bsc_zm73xx_watch(int unit, int enable, int nsecs);
extern int soc_bsc_zm73xx_set(int unit, int pol, float volt);

extern int soc_eep24c64_read(int unit, int devno,
			uint16 addr, uint8* data, uint32 *len);
extern int soc_eep24c64_write(int unit, int devno,
			uint16 addr, uint8* data, uint32 len);
extern void soc_eep24c64_test(int unit, int devno);

extern int soc_sfp_read(int unit, int devno,
			uint16 addr, uint8* data, uint32 *len);
extern int soc_sfp_write(int unit, int devno,
			uint16 addr, uint8* data, uint32 len);

#define MAX_POLS	32

struct zm73xx_sc {
	int numpols;	/* from ID reg */
	int fw_ver;	/* from ID reg */
	int fw_idx;	/* from ID reg */
	int dpm_gen;	/* from ID reg */
	char pids[MAX_POLS];	/* POL ids */
};

extern int soc_bsc_probe(int unit);
extern void soc_bsc_show(int unit);
extern void soc_bsc_setmux(int unit, int chan);
extern void soc_bsc_readmax(int unit);
extern void soc_bsc_readdpm(int unit);
extern int soc_bsc_is_attached(int unit);
extern int soc_bsc_attach(int unit);

#endif	/* INCLUDE_I2C */

#endif	/* !_SOC_BSC_H */


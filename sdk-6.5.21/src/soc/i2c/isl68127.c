/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * I2C Device Driver for ISL68127 an integrated power control IC.
 */

#include <sal/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>

/* Divided by 1000 for voltage conversion */
#define L16_TO_V(val)  ((val)*100000/1000/100000)
#define V_TO_L16(val)  ((val)*1000)
#define L16_TO_UV(val) ((val)*100000/1000*10)
#define UV_TO_L16(val) ((val)/10*1000/100000)

#define L16_RANGE_UPPER(L16, range) ((L16) + (int)(range)*L16/100000)
#define L16_RANGE_LOWER(L16, range) ((L16) - (int)(range)*L16/100000)

#define MAX_VS_CONFIG    2

typedef struct device_data_s {
    sal_mutex_t lock;   /* mutex lock for device operation */
    dac_calibrate_t *dac_params;  /*dac params for each device channel*/
    int dac_param_len;
    int flags;
    i2c_saddr_t rail_saddr;
} device_data_t;

#define DEV_DAC_PARAMS(dev) \
    (((device_data_t *)(((i2c_device_t *)(dev))->priv_data))->dac_params)
#define DEV_DAC_PARAM_LEN(dev) \
    (((device_data_t *)(((i2c_device_t *)(dev))->priv_data))->dac_param_len)   
#define DEV_FLAGS(dev) \
    (((device_data_t *)(((i2c_device_t *)(dev))->priv_data))->flags)   
#define DEV_RAIL_SADDR(dev) \
    (((device_data_t *)(((i2c_device_t *)(dev))->priv_data))->rail_saddr)   


#define DEV_CHECK_RETURN(dev) \
{ \
    if ((dev) == NULL) { \
        return SOC_E_INTERNAL; \
    } \
}

#define DEV_PRIVDATA_CHECK_RETURN(dev) \
{ \
    DEV_CHECK_RETURN(dev) \
    if ((dev)->priv_data == NULL) { \
        return SOC_E_INTERNAL; \
    } \
}


static sal_mutex_t ioctl_lock = NULL;

STATIC int
Write1(uint8 saddr, uint8 comm, uint8 arg0)
{

    return soc_i2c_write_byte_data(0, saddr, comm, arg0);

}

STATIC int
Write2(uint8 saddr, uint8 comm, uint8 arg0, uint8 arg1)
{
    uint16 value = (arg1 << 8) | arg0;

    return soc_i2c_write_word_data(0, saddr, comm, value);

}

STATIC int
Write4(uint8 saddr, uint8 comm, uint8 arg0, uint8 arg1, uint8 arg2, uint8 arg3)
{
    uint8 buf[4];

    buf[0] = arg0;
    buf[1] = arg1;
    buf[2] = arg2;
    buf[3] = arg3;

    return soc_i2c_multi_write(0, saddr, comm, 4, buf);

}

STATIC int
disable_load_line (int unit, int devno) {
    int rv;
    uint8 saddr;

    saddr = soc_i2c_addr(unit, devno);

    /* Disables load line */
    rv = Write2(saddr, 0xf7,0x9b,0xe6);
    if (rv < 0) {
        return rv;
    }
    rv = Write4(saddr, 0xf5,0x05,0x86,0,0);
    if (rv < 0) {
        return rv;
    }
    rv = Write2(saddr, 0xf7,0x85,0xe6);
    if (rv < 0) {
        return rv;
    }
    rv = Write4(saddr, 0xf5,0,0,0,0);
    if (rv < 0) {
        return rv;
    }
    rv = Write2(saddr, 0xf7,0xb4,0xe6);
    if (rv < 0) {
        return rv;
    }
    rv = Write4(saddr, 0xf5,0,0,0,0);
    if (rv < 0) {
        return rv;
    }
    rv = Write2(saddr, 0xf7,0xb2,0xe6);
    if (rv < 0) {
        return rv;
    }
    rv = Write4(saddr, 0xf5,0,0,0,0);
    if (rv < 0) {
        return rv;
    }
    rv = Write1(saddr, 0x00,0x01);
    if (rv < 0) {
        return rv;
    }
    rv = Write2(saddr, 0x28,0,0);
    if (rv < 0) {
        return rv;
    }
    rv = Write2(saddr, 0xf7,0xa3,0xe3);
    if (rv < 0) {
        return rv;
    }
    rv = Write4(saddr, 0xf5,0,0,0,0);
    if (rv < 0) {
        return rv;
    }
    rv = Write2(saddr, 0xe7,0x01,0x0);

    return rv;
}


STATIC int
isl68127_read(int unit, int devno, uint16 addr, uint8* data, uint32* len)
{
    int rv = SOC_E_NONE;
    uint8 saddr;

    saddr = soc_i2c_addr(unit, devno);

    if (*len == 0) {
        return SOC_E_NONE;
    }
    if (*len == 1) {
        /* reads a single byte from a device, from a designated register*/
        rv = soc_i2c_read_byte_data(unit, saddr, addr,data);
        soc_i2c_device(unit, devno)->rbyte++;
         LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "unit %d i2c %s: ISL68127_read: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                    "rv = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *data, *len, rv));
    } else if (*len == 2) {
        /* reads a single word from a device, from a designated register*/
        rv = soc_i2c_read_word_data(unit, saddr, addr,(uint16 *)data);
        soc_i2c_device(unit, devno)->rbyte +=2;
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "unit %d i2c %s: ISL68127_read: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                    "rv = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *(uint16 *)data, *len, rv));
    } else {
        /* not supported for now */
         LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "unit %d i2c %s: ISL68127_read fail: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *data, *len));
    }
    return rv;
}

STATIC int
_isl68127_write(int unit, int devno, uint8 saddr, uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    unsigned short val;

    if (len == 0) {
        /* simply writes command code to device */
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "i2c %s: ISL68127 write: "
                    "saddr = 0x%x, addr = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno), saddr, addr, len));
        rv = soc_i2c_write_byte(unit, saddr, addr);
    } else if (len == 1) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "i2c %s: ISL68127 write: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno), saddr, addr, *data, len));
        rv = soc_i2c_write_byte_data(unit, saddr, addr, *data);
        soc_i2c_device(unit, devno)->tbyte++;
    } else if (len == 2) {
         LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "i2c %s: ISL68127 write: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno),
                    saddr, addr, *(uint16 *)data, len));
        val = *(unsigned short *)data;
        rv = soc_i2c_write_word_data(unit, saddr, addr, val);
        soc_i2c_device(unit, devno)->tbyte += 2;
    }
    return rv;
}

STATIC int
isl68127_write(int unit, int devno, uint16 addr, uint8* data, uint32 len)
{
    uint8 saddr;

    saddr = soc_i2c_addr(unit, devno);

    return (_isl68127_write(unit, devno, saddr, addr, data, len));

}

STATIC int
isl68127_check_page(int unit, int devno, int ch)
{
     int rv;
     uint8 page;
     uint32 len;

     len = sizeof(char);
     rv = isl68127_read(unit, devno, PMBUS_CMD_PAGE, &page, &len);
     if (rv != SOC_E_NONE) {
         return rv;
     }

     if (page != ch) {
         page = ch;
         LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "ISL68127 %d set page to %d\n"),
                     soc_i2c_addr(unit, devno), page));
         rv = isl68127_write(unit, devno, PMBUS_CMD_PAGE, &page, sizeof(char));
     }
     return rv;
}

/*
 * NOTE NOTE NOTE:
 * All tables (dac_calibrate_t) passed to the ioctl() have size > 1
 * and the index is always within this range.
 */
STATIC int
isl68127_ioctl(int unit, int devno, int opcode,
              void* data, int len)
{
    int rv = SOC_E_NONE;
#ifdef COMPILER_HAS_DOUBLE
    double fval;
#else
    int fval;
#endif
    uint16 dac;
    uint32 datalen = 2;
    unsigned short voltage;
    /* Using mutex lock to ensure thread-safe for ioctl operations */
    sal_mutex_take(ioctl_lock, sal_mutex_FOREVER);

     /* length field is actually used as an index into the dac_params table*/
     switch (opcode) {
         case PMBUS_IOC_READ_VOUT:
             if ((rv=isl68127_check_page(unit, devno, len)) < 0) {
                 cli_out("Error: failed to set page %d in ISL68127 device.\n", len);
                 break;
             }
             datalen=2;
             if ((rv=isl68127_read(unit,devno, PMBUS_CMD_READ_VOUT, (void *)&dac, &datalen)) < 0) {
                 cli_out("Error: Failed to read VOUT of ISL68127 Device.\n");
                 break;
             }
             fval=dac;
#ifdef COMPILER_HAS_DOUBLE
             if (len == 0) {
                 fval=((fval)*120000/1000/100000);
             } else {
                 fval=L16_TO_V(fval );
             }
             *(double *)data=fval;
#else
             fval=L16_TO_UV(fval);
             *(int *)data=(fval);
#endif
             break;

         case PMBUS_IOC_READ_IOUT:
             if ((rv=isl68127_check_page(unit, devno, len)) < 0) {
                 cli_out("Error: failed to set page %d in ISL68127 device.\n", len);
                 break;
             }
             if ((rv=isl68127_read(unit, devno,PMBUS_CMD_READ_IOUT, (void *)&dac, &datalen)) < 0) {
                 cli_out("Error:Failed to read current in ISL68127 Device.\n");
                 break;
             }
#ifdef COMPILER_HAS_DOUBLE
             *(double *)data=dac*100; /* (dac/10)*1000 */
#else
             *(int *)data=dac;
#endif
             break;

         case PMBUS_IOC_READ_POUT:
            datalen=2;
            if ((rv=isl68127_check_page(unit, devno, len)) < 0) {
                cli_out("Error: failed to set page %d in ISL68127 device.\n", len);
                break;
            }

            if ((rv=isl68127_read(unit,devno, PMBUS_CMD_READ_POUT, (void *)&dac, &datalen)) < 0) {
                cli_out("Error: failed to read power in ISL68127 device.\n");
                break;
            }
#ifdef COMPILER_HAS_DOUBLE
            if (len == 0) {
                *(double *)data=dac*1200;
            } else {
                *(double *)data=dac*1000;
            }
#else
             *(int *)data=dac;
#endif
             break;

    case PMBUS_IOC_SET_VOUT:
            /* Conversion of output voltage */
#ifdef COMPILER_HAS_DOUBLE
            fval = *((double*)data);
            voltage = (fval*1000);
#else
            fval = *((int*)data);
            /*2^12 conversion and changing from uVolt */
            voltage = (fval*1000)/1000000;
#endif
            dac = voltage;

            /* Show what we are doing, for now ... */
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                        "unit %d i2c %s: ISL68127 ioctl "
                        "I2C_DAC_IOC_SET_VOUT : voltage = %d, len = %d\n"),
                        unit, soc_i2c_devname(unit,devno), voltage, len));

	        rv = isl68127_write(unit, devno, PMBUS_CMD_VOUT_COMMAND,(void *) &dac, 2);
             break;

         default:
              LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                 "unit %d i2c %s: isl68127_ioctl: invalid opcode (%d)\n"),
                 unit, soc_i2c_devname(unit,devno), opcode));
             break;
    }
    sal_mutex_give(ioctl_lock);
    return rv;
}

STATIC int
isl68127_init(int unit, int devno, void* data, int len)
{
    int rv = SOC_E_NONE;
    i2c_device_t *dev = soc_i2c_device(unit, devno);
    char *devname;
    uint8 data8;

    if (dev == NULL) {
        return SOC_E_INTERNAL;
    }
    devname = (char *)soc_i2c_devname(unit, devno);
    if (dev->priv_data == NULL) {
        dev->priv_data = sal_alloc(sizeof(device_data_t), devname);
        if (dev->priv_data == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "Fail to allocate private data fo dev %s\n"),
                        soc_i2c_devname(unit, devno)));
            return SOC_E_MEMORY;
        }
        sal_memset(dev->priv_data, 0, sizeof(device_data_t));
    }

    if (ioctl_lock == NULL) {
        ioctl_lock = sal_mutex_create("isl68127_ioctl_lock");
        if (ioctl_lock == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "Fail to create isl68127_ioctl_lock\n")));
            rv = SOC_E_MEMORY;
        }
    }

    /* Using mutex lock to ensure thread-safe for ioctl operations */
    sal_mutex_take(ioctl_lock, sal_mutex_FOREVER);
    /* bit3 1: control the ISL68127 output through VOUT command
     *      0: control the output via VID input pins which is controlled by
     *         a PCF8574 device
     */
    if ((rv=isl68127_write(unit, devno,
                    PMBUS_CMD_CLEAR_FAULTS,(void *) &len, 0)) < 0) {
        cli_out("Error: Failed to clear the faults of ISL68127 device.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }

    len = 1;
    if ((rv=isl68127_read(unit, devno, 0xFA, (uint8 *)&DEV_RAIL_SADDR(dev), (void *)&len)) < 0) {
        cli_out("Error: Failed to read 0xFA of ISL68127 device.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }

    soc_i2c_devdesc_set(unit, devno, "ISL68127 Voltage Control");
     LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "isl68127_init: %s, devNo=0x%x\n"),
             soc_i2c_devname(unit,devno), devno));

    /* Setting Fault response to zero to prevent shut-down of device */
    data8 =0x00;
    if ((rv=isl68127_write(unit, devno,
                    PMBUS_CMD_VOUT_OV_FAULT_RES, &data8, 1)) < 0) {
        cli_out("Error: failed to set OV fault response of ISL68127.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }

    data8 =0x0A;
    if ((rv=isl68127_write(unit, devno, PMBUS_CMD_ON_OFF_CONFIG, &data8, 1)) < 0) {
        cli_out("Error: failed to set CONFIG register of ISL68127 device.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }
    if ((rv=disable_load_line(unit, devno)) < 0) {
        cli_out("Error: Failed to disable ISL68127 load line.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }
    /* Switching on ISL68127 device */
    data8 =0x80;
    if ((rv=isl68127_write(unit, devno, PMBUS_CMD_OPERATION, &data8, 1)) < 0) {
        cli_out("Error: failed to set operation register of ISL68127 device.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }

    sal_mutex_give(ioctl_lock);
    return rv;
}

/* isl68127 voltage control Chip Driver callout */
i2c_driver_t _soc_i2c_isl68127_driver = {
    0x0, 0x0, /* System assigned bytes */
    ISL68127_DEVICE_TYPE,
    isl68127_read,
    isl68127_write,
    isl68127_ioctl,
    isl68127_init,
    NULL,
};

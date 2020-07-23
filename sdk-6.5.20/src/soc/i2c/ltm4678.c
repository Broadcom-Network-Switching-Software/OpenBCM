/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * I2C Device Driver for LTM4678 an integrated power control IC.
 */

#include <sal/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>
 /* Divided by 2^12 for voltage conversion */
#define L16_TO_V(val)  ((val)*100000/4096/100000)
#define V_TO_L16(val)  ((val)*4096)
#define L16_TO_UV(val) ((val)*100000/4096*10)
#define UV_TO_L16(val) ((val)/10*4096/100000)

#define L16_RANGE_UPPER(L16, range) ((L16) + (int)(range)*L16/100000)
#define L16_RANGE_LOWER(L16, range) ((L16) - (int)(range)*L16/100000)

#define MAX_VS_CONFIG    2

#define POWER(exponent, input_val)  exponent < 0 ? \
                            (int)((int) (input_val) << (exponent*(-1))): \
                            (int)((int) (input_val*1000000) >> \
			    exponent)/1000000

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

/*
 * Convert a floating point value into a
 * LinearFloat5_11 formatted word
 */
int
#ifdef  COMPILER_HAS_DOUBLE
ltm4678_float_to_L11(double input_val, uint16* data)
#else
ltm4678_float_to_L11(int input_val, uint16* data)
#endif
{
    uint16 uExponent, uMantissa;
    /* set exponent to -16 */
    int16 exponent = -16;
    /* extract mantissa from input value */
    int mantissa = POWER(exponent, input_val); 

    /* Search for an exponent that produces
     * a valid 11-bit mantissa */
    do
    {
        if((mantissa >= -1024) &&
                (mantissa <= +1023))
        {
            break; /* stop if mantissa valid */
        }
        exponent++;
        mantissa = POWER(exponent, input_val);
    } while (exponent < +15);

    /* Format the exponent of the L11 */
    uExponent = exponent << 11;
    /* Format the mantissa of the L11 */
    uMantissa = mantissa & 0x07FF;
    /* Compute value as exponent | mantissa */
    *(data) = uExponent | uMantissa;
    return SOC_E_NONE;
}

/*
 * Convert a LinearFloat5_11 formatted word
 * into a floating point value
 */
int
ltm4678_L11_to_float(uint16 input_val, void *data)
{
    /* extract exponent as MS 5 bits */
    int8 exponent = input_val >> 11;
    /* extract mantissa as LS 11 bits */
    int16 mantissa = input_val & 0x7ff;
    /* sign extend exponent from 5 to 8 bits */
    if( exponent > 0x0F ) exponent |= 0xE0;
    /* sign extend mantissa from 11 to 16 bits */
    if( mantissa > 0x03FF ) mantissa |= 0xF800;
#ifdef  COMPILER_HAS_DOUBLE
    /* compute value as mantissa * 2^(exponent) */
    *(double *)data= exponent < 0 ?
                      (double) ((mantissa*1000000) >>
                                (exponent*(-1)))/1000000:
                      (double) (mantissa << exponent);
#else
    *(int *)data= exponent < 0 ?
                   (((mantissa)*1000000) >> (exponent*(-1))):
                   ((mantissa) << exponent)*100000;
#endif
    return SOC_E_NONE;
}

STATIC int
ltm4678_wait_for_not_busy(int unit, int devno)
{
    int rv = SOC_E_NONE;
    uint8 mfr_status, saddr;
    uint32 usec, wait_usec;

    wait_usec = 0;
    usec = 10;

    saddr = soc_i2c_addr(unit, devno);

    while(wait_usec < 1000000) {
        SOC_IF_ERROR_RETURN
            (soc_i2c_read_byte_data(unit, saddr,PMBUS_CMD_MFR_COMMON,
                                    &mfr_status));
        soc_i2c_device(unit, devno)->rbyte++;
        if ((mfr_status & 0x70) == 0x70) {
            /* Bit 6 : Chip not busy */
            /* Bit 5 : calculations not pending */
            /* Bit 4 : OUTPUT not in transition */
            break;
        } else {
            sal_udelay(usec);
            wait_usec += usec;
        }
    }

    if ((mfr_status & 0x70) != 0x70) {
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "unit %d i2c %s :ltm4678 is busy !\n"),
                 unit, soc_i2c_devname(unit, devno)));
        rv = SOC_E_TIMEOUT;
    }
    return rv;
}


STATIC int
ltm4678_read(int unit, int devno, uint16 addr, uint8* data, uint32* len)
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
                    "unit %d i2c %s: LTM4678_read: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                    "rv = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *data, *len, rv));
    } else if (*len == 2) {
        /* reads a single word from a device, from a designated register*/
        rv = soc_i2c_read_word_data(unit, saddr, addr,(uint16 *)data);
        soc_i2c_device(unit, devno)->rbyte +=2;
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "unit %d i2c %s: LTM4678_read: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                    "rv = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *(uint16 *)data, *len, rv));
    } else {
        /* not supported for now */
         LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "unit %d i2c %s: LTM4678_read fail: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *data, *len));
    }
    return rv;
}

STATIC int
_ltm4678_write(int unit, int devno, uint8 saddr, uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    unsigned short val;

    if (len == 0) {
        /* simply writes command code to device */
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "i2c %s: LTM4678 write: "
                    "saddr = 0x%x, addr = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno), saddr, addr, len));
        rv = soc_i2c_write_byte(unit, saddr, addr);
    } else if (len == 1) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "i2c %s: LTM4678 write: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno), saddr, addr, *data, len));
        rv = soc_i2c_write_byte_data(unit, saddr, addr, *data);
        soc_i2c_device(unit, devno)->tbyte++;
    } else if (len == 2) {
         LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "i2c %s: LTM4678 write: "
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
ltm4678_write(int unit, int devno, uint16 addr, uint8* data, uint32 len)
{
    uint8 saddr;

    saddr = soc_i2c_addr(unit, devno);

    return (_ltm4678_write(unit, devno, saddr, addr, data, len));

}

STATIC int
ltm4678_rail_write(int unit, int devno, uint16 addr, uint8* data, uint32 len)
{
    uint8 rail_saddr;

    rail_saddr = DEV_RAIL_SADDR(soc_i2c_device(unit, devno));

    if (rail_saddr == 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "Rail saddr not set for dev %s\n"),
                    soc_i2c_devname(unit, devno)));
        return SOC_E_INTERNAL;
    }
    return (_ltm4678_write(unit, devno, rail_saddr, addr, data, len));

}

STATIC int
ltm4678_check_page(int unit, int devno, int ch)
{
     int rv;
     uint8 page;
     uint32 len;

     len = sizeof(char);
     rv = ltm4678_read(unit, devno, PMBUS_CMD_PAGE, &page, &len);
     if (rv != SOC_E_NONE) {
         return rv;
     }

     if (page != ch) {
         page = ch;
         LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "LTM4678 %d set page to %d\n"),
                     soc_i2c_addr(unit, devno), page));
         rv = ltm4678_write(unit, devno, PMBUS_CMD_PAGE, &page, sizeof(char));
     }
     return rv;
}

/*
 * NOTE NOTE NOTE:
 * All tables (dac_calibrate_t) passed to the ioctl() have size > 1
 * and the index is always within this range.
 */
STATIC int
ltm4678_ioctl(int unit, int devno, int opcode,
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
         case I2C_LTC_IOC_READ_VOUT:
             if ((rv=ltm4678_check_page(unit, devno, len)) < 0) {
                 cli_out("Error: failed to set page %d in LTM4678 device.\n", len);
                 break;
             }
             if ((rv=ltm4678_wait_for_not_busy(unit, devno)) < 0) {
                 cli_out("Error: Device LTM4678 is busy.\n");
                 break;
             }
             datalen=2;
             if ((rv=ltm4678_read(unit,devno, PMBUS_CMD_READ_VOUT, (void *)&dac, &datalen)) < 0) {
                 cli_out("Error: Failed to read VOUT of LTM4678 Device.\n");
                 break;
             }
             fval=dac;
#ifdef COMPILER_HAS_DOUBLE
             fval=L16_TO_V(fval );
             *(double *)data=fval;
#else
             fval=L16_TO_UV(fval);
             *(int *)data=(fval);
#endif
             break;

         case I2C_LTC_IOC_READ_IOUT:
             if ((rv=ltm4678_check_page(unit, devno, len)) < 0) {
                 cli_out("Error: failed to set page %d in LTM4678 device.\n", len);
                 break;
             }
             if ((rv=ltm4678_wait_for_not_busy(unit, devno)) < 0) {
                 cli_out("Error: LTM4678 Device is busy.\n");
                 break;
             }
             if ((rv=ltm4678_read(unit, devno,PMBUS_CMD_READ_IOUT, (void *)&dac, &datalen)) < 0) {
                 cli_out("Error:Failed to read current in LTM4678 Device.\n");
                 break;
             }
             rv=ltm4678_L11_to_float(dac, &fval);
#ifdef COMPILER_HAS_DOUBLE
             *(double *)data=fval*1000;
#else
             *(int *)data=fval;
#endif
             break;

         case I2C_LTC_IOC_READ_POUT:
            datalen=2;
            if ((rv=ltm4678_check_page(unit, devno, len)) < 0) {
                cli_out("Error: failed to set page %d in LTM4678 device.\n", len);
                break;
            }
            if ((rv=ltm4678_wait_for_not_busy(unit, devno)) < 0) {
                cli_out("Error: LTM4678 Device is busy.\n");
                sal_mutex_give(ioctl_lock);
                return rv;
            }

            if ((rv=ltm4678_read(unit,devno, PMBUS_CMD_READ_POUT, (void *)&dac, &datalen)) < 0) {
                cli_out("Error: failed to read power in LTM4678 device.\n");
                break;
            }
             rv=ltm4678_L11_to_float(dac, &fval);
#ifdef COMPILER_HAS_DOUBLE
             *(double *)data=fval*1000;
#else
             *(int *)data=fval;
#endif
             break;

    case I2C_LTC_IOC_SET_VOUT:
            /* Conversion of output voltage */
#ifdef COMPILER_HAS_DOUBLE
            fval = *((double*)data);
            voltage = (fval*4096);
#else
            fval = *((int*)data);
            /*2^12 conversion and changing from uVolt */
            voltage = (fval*4096)/1000000;
#endif
            dac = voltage;
            /* Show what we are doing, for now ... */
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                        "unit %d i2c %s: LTM4678 ioctl "
                        "I2C_DAC_IOC_SET_VOUT : voltage = %d, len = %d\n"),
                        unit, soc_i2c_devname(unit,devno), voltage, len));

	        rv = ltm4678_rail_write(unit, devno, PMBUS_CMD_VOUT_COMMAND,(void *) &dac, 2);
             break;

         default:
              LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                 "unit %d i2c %s: ltm4678_ioctl: invalid opcode (%d)\n"),
                 unit, soc_i2c_devname(unit,devno), opcode));
             break;
    }
    sal_mutex_give(ioctl_lock);
    return rv;
}

STATIC int
ltm4678_init(int unit, int devno, void* data, int len)
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
        ioctl_lock = sal_mutex_create("ltm4678_ioctl_lock");
        if (ioctl_lock == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "Fail to create ltm4678_ioctl_lock\n")));
            rv = SOC_E_MEMORY;
        }
    }

    /* Using mutex lock to ensure thread-safe for ioctl operations */
    sal_mutex_take(ioctl_lock, sal_mutex_FOREVER);
    /* bit3 1: control the LTM4678 output through VOUT command
     *      0: control the output via VID input pins which is controlled by
     *         a PCF8574 device
     */
    if ((rv=ltm4678_write(unit, devno,
                    PMBUS_CMD_CLEAR_FAULTS,(void *) &len, 0)) < 0) {
        cli_out("Error: Failed to clear the faults of LTM4678 device.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }
    if ((rv=ltm4678_wait_for_not_busy(unit, devno)) < 0) {
        cli_out("Error: LTM4678 Device is busy.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }

    len = 1;
    if ((rv=ltm4678_read(unit, devno, 0xFA, (uint8 *)&DEV_RAIL_SADDR(dev), (void *)&len)) < 0) {
        cli_out("Error: Failed to read 0xFA of LTM4678 device.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }

    /* bsl_printf("SADDR = 0x%02x DEV_RAIL_SADDR = 0x%02x\n", soc_i2c_addr(unit, devno), DEV_RAIL_SADDR(dev)); */
    soc_i2c_devdesc_set(unit, devno, "LTM4678 Voltage Control");
     LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "ltm4678_init: %s, devNo=0x%x\n"),
             soc_i2c_devname(unit,devno), devno));

    /* Setting Fault response to zero to prevent shut-down of device */
    data8 =0x00;
    if ((rv=ltm4678_write(unit, devno,
                    PMBUS_CMD_VOUT_OV_FAULT_RES, &data8, 1)) < 0) {
        cli_out("Error: failed to set OV fault response of LTM4678.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }
    if ((rv=ltm4678_wait_for_not_busy(unit, devno)) < 0) {
        cli_out("Error: LTM4678 device is busy.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }

    data8 =0x0A;
    if ((rv=ltm4678_write(unit, devno, PMBUS_CMD_ON_OFF_CONFIG, &data8, 1)) < 0) {
        cli_out("Error: failed to set CONFIG register of LTM4678 device.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }
    /* Switching on LTC4678 device */
    data8 =0x80;
    if ((rv=ltm4678_write(unit, devno, PMBUS_CMD_OPERATION, &data8, 1)) < 0) {
        cli_out("Error: failed to set operation register of LTM4678 device.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }
    if ((rv=ltm4678_wait_for_not_busy(unit, devno)) < 0) {
        cli_out("Error: LTM4678 Device is busy.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }

    sal_mutex_give(ioctl_lock);
    return rv;
}

/* ltm4678 voltage control Chip Driver callout */
i2c_driver_t _soc_i2c_ltm4678_driver = {
    0x0, 0x0, /* System assigned bytes */
    LTM4678_DEVICE_TYPE,
    ltm4678_read,
    ltm4678_write,
    ltm4678_ioctl,
    ltm4678_init,
    NULL,
};

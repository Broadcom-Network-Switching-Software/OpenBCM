/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * I2C Device Driver for pmbus an integrated power control IC.
 */

#include <sal/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>

#ifdef COMPILER_HAS_DOUBLE
#define I2C_DATA_T      double
#define I2C_DATA_F      "f"
#else
#define I2C_DATA_T      int
#define I2C_DATA_F      "d"
#endif

#define POWER(exponent, input_val)                          \
    exponent < 0 ?                                          \
    (int)((int) (input_val) << (exponent*(-1))):            \
    (int)((int) (input_val*1000000) >> exponent)/1000000

typedef struct device_data_s {
    sal_mutex_t lock;               /* mutex lock for device operation */
    int pmbus_io_flags;             /* pmbus io ctrl support flag */
#define PMBUS_VID    0x20
    int dac_vout_mode;              /* vout_command mode */
    dac_calibrate_t *dac_params;    /* dac params for each device channel */
    int dac_param_len;              /* number of dac params, for multi-channel device */
} device_data_t;

#define DEV_DAC_VOUTMODE(dev) \
    (((device_data_t *)(((i2c_device_t *)(dev))->priv_data))->dac_vout_mode)
#define DEV_DAC_PARAMS(dev) \
    (((device_data_t *)(((i2c_device_t *)(dev))->priv_data))->dac_params)
#define DEV_DAC_PARAM_LEN(dev) \
    (((device_data_t *)(((i2c_device_t *)(dev))->priv_data))->dac_param_len)

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

/*
 * Convert a floating point value into a
 * LinearFloat5_11 formatted word
 */
STATIC int
#ifdef  COMPILER_HAS_DOUBLE
pmbus_float_to_L11(double input_val, uint16* data)
#else
pmbus_float_to_L11(int input_val, uint16* data)
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
        if((mantissa >= -1024) && (mantissa <= +1023))
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
STATIC int
pmbus_L11_to_float(uint16 input_val, void *data)
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
pmbus_read(int unit, int devno, uint16 addr, uint8* data, uint32* len)
{

    int rv = SOC_E_NONE;
    uint8 saddr = soc_i2c_addr(unit, devno);

    if (!*len) {
        return SOC_E_NONE;
    }

    /* reads a single byte from a device, from a designated register*/
    if (*len == 1) {
        rv = soc_i2c_read_byte_data(unit, saddr, addr,data);
        soc_i2c_device(unit, devno)->rbyte++;
    } else if (*len == 2) {
        rv = soc_i2c_read_word_data(unit, saddr, addr,(uint16 *)data);
        soc_i2c_device(unit, devno)->rbyte +=2;
    } else {
        /* not supported for now */
    }

    return rv;
}

STATIC int
pmbus_write(int unit, int devno, uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    uint8 saddr = soc_i2c_addr(unit, devno);

    if (len == 0) {
        /* simply writes command code to device */
        rv = soc_i2c_write_byte(unit, saddr, addr);
    } else if (len == 1) {
        rv = soc_i2c_write_byte_data(unit, saddr, addr,*data);
        soc_i2c_device(unit, devno)->tbyte++;
    } else if (len == 2) {
        rv = soc_i2c_write_word_data(unit, saddr, addr, *(uint16 *)data);
        soc_i2c_device(unit, devno)->tbyte += 2;
    }
    return rv;
}

STATIC int
pmbus_ioctl(int unit, int devno, int opcode, void* data, int len)
{
    int rv = SOC_E_NONE;
    I2C_DATA_T fval = 0,tmp;
    uint8  data8;
    uint16 dac;
    uint16 time_ms;
    uint32 datalen = 0;
    dac_calibrate_t* dac_params = NULL;
    i2c_device_t *dev = soc_i2c_device(unit, devno);

    DEV_PRIVDATA_CHECK_RETURN(dev);

    dac_params = DEV_DAC_PARAMS(dev);
    /* length field is actually used as an index into the dac_params table*/
    if((!data) || ((dac_params != NULL) && (len > DEV_DAC_PARAM_LEN(dev)))) {
        return SOC_E_PARAM;
    }

    switch (opcode){
    case PMBUS_IOC_SET_DAC:
    /* Set special dac table */
    dac_params = (dac_calibrate_t*)data;
    DEV_DAC_PARAMS(dev) = dac_params;
    DEV_DAC_PARAM_LEN(dev) = len;

    /* Set vout command mode*/
    datalen = 1;
    if ((rv = pmbus_read(unit, devno, PMBUS_CMD_VOUT_MODE,
                         &data8, &datalen)) < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Error: failed to read VOUT MODE for %s.\n"),
                  soc_i2c_devname(unit, devno)));
        break;
    }
    DEV_DAC_VOUTMODE(dev) = data8;
    break;

    case PMBUS_IOC_SET_VOUT:
    /* Set output voltage */
    fval = *((I2C_DATA_T*)data);
    if (dac_params) {
        if ((fval < dac_params[len].min)||(fval > dac_params[len].max)){
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "unit %d i2c %s: calibration/range error :"
                                    "requested=%"I2C_DATA_F" (max=%"I2C_DATA_F","
                                    "min=%"I2C_DATA_F",step=%"I2C_DATA_F")\n"),
                         unit, soc_i2c_devname(unit,devno),
                         fval, dac_params[len].max,
                         dac_params[len].min,
                         dac_params[len].step));
            return SOC_E_PARAM;
        }

        if (DEV_DAC_VOUTMODE(dev) & PMBUS_VID) {
            /* VID mode */
            if(dac_params[len].use_max)  {
                tmp = ((dac_params[len].max - fval) / dac_params[len].step);
                tmp = dac_params[len].dac_max_hwval - tmp;
            } else {
                tmp = ((dac_params[len].max - fval) / dac_params[len].step);
                tmp = dac_params[len].dac_max_hwval + tmp;
            }
            dac = (uint8)tmp;
        } else {
            /* Linear mode */
            rv = pmbus_float_to_L11(fval, &dac);
        }

        /* Show what we are doing, for now ... */
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d i2c %s: Set V_%s:"
                                "request=%"I2C_DATA_F" dac=0x%x (max=%"I2C_DATA_F","
                                "min=%"I2C_DATA_F",step=%"I2C_DATA_F")\n"),
                     unit, soc_i2c_devname(unit, devno),
                     dac_params[len].name,
                     fval,
                     dac,
                     dac_params[len].max,
                     dac_params[len].min,
                     dac_params[len].step));
        if ((rv = pmbus_write(unit, devno,
                              PMBUS_CMD_VOUT_COMMAND, (uint8 *)&dac, 2)) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "Error: failed to set VOUT for %s.\n"),
                      soc_i2c_devname(unit, devno)));
            break;
        }
        /* Keep last value since DAC is write-only device */
        dac_params[len].dac_last_val = dac;
    }
    break;

    case PMBUS_IOC_READ_VOUT:
    /* Set output voltage */
    if (dac_params) {
        datalen = 2;
        if ((rv = pmbus_read(unit, devno, PMBUS_CMD_READ_VOUT,
                             (uint8 *)&dac, &datalen)) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "Error: failed to read VOUT for %s.\n"),
                      soc_i2c_devname(unit, devno)));
            break;
        }

        /* Show what we are doing, for now ... */
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d i2c %s: Get V_%s:"
                                " dac=0x%x (max=%"I2C_DATA_F","
                                "min=%"I2C_DATA_F",step=%"I2C_DATA_F")\n"),
                     unit, soc_i2c_devname(unit, devno),
                     dac_params[len].name,
                     dac,
                     dac_params[len].max,
                     dac_params[len].min,
                     dac_params[len].step));

        if (DEV_DAC_VOUTMODE(dev) & PMBUS_VID) {
            if(dac_params[len].use_max)  {
                tmp = dac_params[len].dac_max_hwval - dac;
                fval = dac_params[len].max - tmp * dac_params[len].step;
            } else {
                tmp = dac - dac_params[len].dac_max_hwval;
                fval = dac_params[len].max - tmp * dac_params[len].step;
            }
        } else {
            /* Linear mode */
            rv = pmbus_L11_to_float(dac, &fval);
        }
    }
    *((I2C_DATA_T*)data) = fval;
    break;

    case PMBUS_IOC_SET_SEQ:
    time_ms = *((uint16 *)data);
    time_ms &= ((1 << 4) - 1);
    if ((rv = pmbus_write(unit, devno, PMBUS_CMD_POWER_ON_DELAY,
                          (uint8 *)&time_ms, 2)) < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Error: failed to set TON_DELAY for %s.\n"),
                  soc_i2c_devname(unit, devno)));
        break;
    }
    break;

    case PMBUS_IOC_READ_SEQ:
    datalen = 2;
    if ((rv = pmbus_read(unit, devno, PMBUS_CMD_POWER_ON_DELAY,
                         (uint8 *)&time_ms, &datalen)) < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Error: failed to read TON_DELAY for %s.\n"),
                  soc_i2c_devname(unit, devno)));
        break;
    }
    *((uint16 *)data) = time_ms;
    break;

    case PMBUS_IOC_READ_IOUT:
    datalen = 2;
    if ((rv = pmbus_read(unit, devno, PMBUS_CMD_READ_IOUT,
                         (uint8 *)&dac, &datalen)) < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Error: failed to read current for %s.\n"),
                  soc_i2c_devname(unit, devno)));
        break;
    }
    rv = pmbus_L11_to_float(dac, &fval);
#ifdef COMPILER_HAS_DOUBLE
     *(double *)data=fval*1000;
#else
     *(int *)data=fval;
#endif
    break;

    default:
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "unit %d i2c %s: pmbus_ioctl: invalid opcode (%d)\n"),
                 unit, soc_i2c_devname(unit, devno), opcode));
    break;
    }

    return rv;
}

/* Calibration table in effect */
static dac_calibrate_t
ncp4200_dac_params = {0, "core",
                      NCP4200_MAX_VOL, NCP4200_MIN_VOL, NCP4200_DAC_STEP, 0,
                      NCP4200_MIN_DACVAL, NCP4200_MAX_DACVAL, NCP4200_MID_DACVAL, 0x0};

static dac_calibrate_t
ncp81233_dac_params = {0, "core",
                       NCP81233_MAX_VOL, NCP81233_MIN_VOL, NCP81233_DAC_STEP, 0,
                       NCP81233_MAX_DACVAL, NCP81233_MIN_DACVAL, NCP81233_MID_DACVAL, 0x1};

STATIC int
pmbus_init(int unit, int devno, void* data, int len)
{
    uint8 data8, mfr_id;
    uint16 mfr_model;
    uint32 datalen = 0;
    int rv = SOC_E_NONE, line = __LINE__;
    i2c_device_t *dev = soc_i2c_device(unit, devno);
    char *devname;

    if (dev == NULL) {
        return SOC_E_INTERNAL;
    }
    devname = (char *)soc_i2c_devname(unit, devno);
    if (dev->priv_data == NULL) {
        dev->priv_data = sal_alloc(sizeof(device_data_t), devname);
        if (dev->priv_data == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Fail to allocate private data fo dev %s\n"),
                      soc_i2c_devname(unit, devno)));
            rv = SOC_E_MEMORY;
            return rv;
        }
        DEV_DAC_PARAMS(dev) = NULL;
        DEV_DAC_PARAM_LEN(dev) = 0;
    }

    /* clear all faults */
    /* coverity[callee_ptr_arith: FALSE] */
    rv = pmbus_write(unit, devno, PMBUS_CMD_CLEAR_FAULTS, &data8, 0);
    if (SOC_FAILURE(rv)) {
        line = __LINE__;
        goto err_return;
    }

    /* enable operation commands over the PMBus */
    datalen = 1;
    rv =pmbus_read(unit, devno, PMBUS_CMD_ON_OFF_CONFIG, &data8, &datalen);
    if (SOC_FAILURE(rv)) {
        line = __LINE__;
        goto err_return;
    }

    data8 |= 0xf;
    rv = pmbus_write(unit, devno, PMBUS_CMD_ON_OFF_CONFIG, &data8, 1);
    if (SOC_FAILURE(rv)) {
        line = __LINE__;
        goto err_return;
    }

    /* get MFR ID and MFR MODEL */
    rv = pmbus_read(unit, devno, PMBUS_CMD_MFR_ID, &mfr_id, &datalen);
    if (SOC_FAILURE(rv)) {
        line = __LINE__;
        goto err_return;
    }

    datalen = 2;
    rv = pmbus_read(unit, devno, PMBUS_CMD_MFR_MODEL, (uint8*)&mfr_model, &datalen);
    if (SOC_FAILURE(rv)) {
        line = __LINE__;
        goto err_return;
    }

    /* MFR special init */
    if((mfr_id == 0x1) && (mfr_model == 0x2)) {
        /* NCP4200 device */
        data8 = NCP4200_MID_DACVAL;
        rv = pmbus_write(unit, devno, PMBUS_CMD_VOUT_COMMAND, &data8, 1);
        if (SOC_FAILURE(rv)) {
            line = __LINE__;
            goto err_return;
        }
        if (soc_property_get(unit, "voltage_control", 1)) {
            datalen = 1;
            rv = pmbus_read(unit, devno, I2C_ADP4000_CMD_CONFIG_1A, &data8, &datalen);
            if (SOC_FAILURE(rv)) {
                line = __LINE__;
                goto err_return;
            }
            data8 |= 1 << 3;
            rv = pmbus_write(unit, devno, I2C_ADP4000_CMD_CONFIG_1A, &data8, 1);
            if (SOC_FAILURE(rv)) {
                line = __LINE__;
                goto err_return;
            }
            datalen = 1;
            rv = pmbus_read(unit, devno, I2C_ADP4000_CMD_CONFIG_1B, &data8, &datalen);
            if (SOC_FAILURE(rv)) {
                line = __LINE__;
                goto err_return;
            }
            data8 |= 1 << 3;
            rv = pmbus_write(unit, devno, I2C_ADP4000_CMD_CONFIG_1B, &data8, 1);
            if (SOC_FAILURE(rv)) {
                line = __LINE__;
                goto err_return;
            }
        } else {
            datalen = 1;
            rv = pmbus_read(unit, devno, I2C_ADP4000_CMD_CONFIG_1A, &data8, &datalen);
            if (SOC_FAILURE(rv)) {
                line = __LINE__;
                goto err_return;
            }
            data8 &= ~(1 << 3);
            rv = pmbus_write(unit, devno, I2C_ADP4000_CMD_CONFIG_1A, &data8, 1);
            if (SOC_FAILURE(rv)) {
                line = __LINE__;
                goto err_return;
            }
            datalen = 1;
            rv = pmbus_read(unit, devno, I2C_ADP4000_CMD_CONFIG_1B, &data8, &datalen);
            if (SOC_FAILURE(rv)) {
                line = __LINE__;
                goto err_return;
            }
            data8 &= ~(1 << 3);
            rv = pmbus_write(unit, devno, I2C_ADP4000_CMD_CONFIG_1B, &data8, 1);
            if (SOC_FAILURE(rv)) {
                line = __LINE__;
                goto err_return;
            }
        }
        rv = pmbus_ioctl(unit, devno, PMBUS_IOC_SET_DAC, &ncp4200_dac_params, 1);
        if (SOC_FAILURE(rv)) {
            line = __LINE__;
            goto err_return;
        }
    } else if ((mfr_id == 0x1A) && (mfr_model == 0x1233)) {
        /* NCP81233 device */
        rv = pmbus_ioctl(unit, devno, PMBUS_IOC_SET_DAC, &ncp81233_dac_params, 1);
        if (SOC_FAILURE(rv)) {
            line = __LINE__;
            goto err_return;
        }
    }

    soc_i2c_devdesc_set(unit, devno, "Pmbus Voltage Control");

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "pmbus_init: %s, devNo=0x%x\n"),
                 soc_i2c_devname(unit, devno), devno));
    return rv;

err_return:
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit, "pmbus init error: %s, line = %d\n"),
              soc_i2c_devname(unit, devno), line));
    sal_free(dev->priv_data);
    dev->priv_data = NULL;
    return rv;
}

STATIC int
pmbus_deinit(int unit, int devno)
{
    int rv = SOC_E_NONE;
    i2c_device_t *dev = soc_i2c_device(unit, devno);

    if (dev != NULL) {
        if (dev->priv_data != NULL) {
            sal_free(dev->priv_data);
            dev->priv_data = NULL;
        }
    } else {
        rv = SOC_E_INTERNAL;
    }

    return rv;
}

/* pmbus voltage control Chip Driver callout */
i2c_driver_t _soc_i2c_pmbus_driver = {
    0x0, 0x0, /* System assigned bytes */
    PMBUS_DEVICE_TYPE,
    pmbus_read,
    pmbus_write,
    pmbus_ioctl,
    pmbus_init,
    pmbus_deinit,
};

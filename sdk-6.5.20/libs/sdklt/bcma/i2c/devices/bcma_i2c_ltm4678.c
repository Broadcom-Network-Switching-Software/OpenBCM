/*! \file bcma_i2c_ltm4678.c
 *
 *  Application to control power regulator ltm4678.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>
#include <bcma/i2c/bcma_i2ccmd_i2c.h>
#include <bcma/i2c/bcma_i2c_device.h>
#include <bcmbd/bcmbd_i2c.h>
#include <bcma/io/bcma_io_file.h>
#include <bcmbd/bcmbd.h>

#define BSL_LOG_MODULE  BSL_LS_APPL_I2C

/* Device specific operation */

/* Divided by 2^12 for voltage conversion */
#define L16_TO_V(val)  ((val)*100000/4096/100000)
#define V_TO_L16(val)  ((val)*4096)
#define L16_TO_UV(val) ((val)*100000/4096*10)
#define UV_TO_L16(val) ((val)/10*4096/100000)

#define POWER(exponent, input_val)  exponent < 0 ? \
                            (int)((int) (input_val) << (exponent*(-1))): \
                            (int)((int) (input_val*1000000) >> \
                            exponent)/1000000

/*!
 * \brief Local utility function.
 *
 * Convert a LinearFloat5_11 formatted word
 * into a floating point value
 */
static int
ltm4678_l11_to_float(uint16_t input_val, void *data)
{
    /* extract exponent as MS 5 bits */
    int8_t exponent = input_val >> 11;

    /* extract mantissa as LS 11 bits */
    int16_t mantissa = input_val & 0x7ff;

    /* sign extend exponent from 5 to 8 bits */
    if (exponent > 0x0F) {
        exponent |= 0xE0;
    }

    /* sign extend mantissa from 11 to 16 bits */
    if (mantissa > 0x03FF) {
        mantissa |= 0xF800;
    }

    /* compute value as mantissa * 2^(exponent) */
    *(double *)data = exponent < 0 ?
                      (double) ((mantissa*1000000) >>
                               (exponent*(-1)))/1000000:
                      (double) (mantissa << exponent);

    return SHR_E_NONE;
}

/*!
 * \brief Derive utility routine - wait.
 */
static int
ltm4678_wait_for_not_busy(int unit, int saddr)
{
    int rv = SHR_E_NONE;
    uint8_t mfr_status;
    uint32_t usec, wait_usec;

    wait_usec = 0;
    usec = 10;

    while (wait_usec < 1000000) {
        rv =  bcmbd_i2c_read_byte_data(unit,
                                       saddr,PMBUS_CMD_MFR_COMMON,
                                       &mfr_status);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        if ((mfr_status & 0x70) == 0x70) {
            /* Bit 6 : Chip not busy */
            /* Bit 5 : calculations not pending */
            /* Bit 4 : OUTPUT not in transition */
            break;
        } else {
            sal_usleep(usec);
            wait_usec += usec;
        }
    }

    if ((mfr_status & 0x70) != 0x70) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "i2c %s :ltm4678 is busy !\n"),
                  "LTM4678"));
        rv = SHR_E_TIMEOUT;
    }

    return rv;
}

/*!
 * \brief Device specific read function.
 */
static int
ltm4678_read(int unit,
             int saddr,
             uint16_t addr,
             uint8_t *data,
             uint32_t *len)
{
    int rv = SHR_E_NONE;

    if (*len == 0) {
        return SHR_E_NONE;
    }

    if (*len == 1) {
        /* reads a single byte from a device, from a designated register*/
        rv = bcmbd_i2c_read_byte_data(unit, saddr, addr,data);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: LTM4678_read: "
                                "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                                "rv = %d\n"),
                     "LTM4678",
                     saddr, addr, *data, *len, rv));
    } else if (*len == 2) {
        /* reads a single word from a device, from a designated register*/
        rv = bcmbd_i2c_read_word_data(unit, saddr, addr, (uint16_t *)data);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: LTM4678_read: "
                                "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                                "rv = %d\n"),
                     "LTM4678",
                     saddr, addr, *(uint16_t *)data, *len, rv));
    } else {
        /* not supported for now */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: LTM4678_read fail: "
                                "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                     "LTM4678",
                     saddr, addr, *data, *len));
    }

    return rv;
}

/*!
 * \brief Device specific write function.
 */
static int
ltm4678_write_op(int unit,
                 uint8_t saddr,
                 uint16_t addr,
                 uint8_t *data,
                 uint32_t len)
{
    int rv = SHR_E_NONE;
    unsigned short val;

    if (len == 0) {
        /* simply writes command code to device */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: LTM4678 write: "
                                "saddr = 0x%x, addr = 0x%x, len = %d\n"),
                     "LTM4678", saddr, addr, len));
        rv = bcmbd_i2c_write_byte(unit, saddr, addr);

    } else if (len == 1) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: LTM4678 write: "
                                "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                     "LTM4678", saddr, addr, *data, len));
        rv = bcmbd_i2c_write_byte_data(unit, saddr, addr, *data);
    } else if (len == 2) {
         LOG_VERBOSE(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "i2c %s: LTM4678 write: "
                                 "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                      "LTM4678", saddr, addr, *(uint16_t *)data, len));
        val = *(unsigned short *)data;
        rv = bcmbd_i2c_write_word_data(unit, saddr, addr, val);
    }

    return rv;
}

/*!
 * \brief Device specific write function wrapper.
 */
static int
ltm4678_write(int unit,
              int saddr,
              uint16_t addr,
              uint8_t *data,
              uint32_t len)
{
    return (ltm4678_write_op(unit, saddr, addr, data, len));
}

/*!
 * \brief Write to rail bus.
 */
static int
ltm4678_rail_write(int unit,
                   int saddr,
                   i2c_device_t *dev,
                   uint16_t addr,
                   uint8_t *data,
                   uint32_t len)
{
    uint8_t rail_saddr = 0;

    /* Rail address is read back from device from 0xFA read command */
    rail_saddr = DEV_RAIL_SADDR(dev);

    if (rail_saddr == 0) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                    "Rail saddr not set for dev %s\n"),
                    "LTM4678"));
        return SHR_E_INTERNAL;
    }

    return (ltm4678_write_op(unit, rail_saddr, addr, data, len));
}

/*!
 * \brief Check per page (sub-channel) operation.
 */
static int
ltm4678_check_page(int unit, int saddr, int ch)
{
     int rv;
     uint8_t page;
     uint32_t len;

     len = sizeof(char);
     rv = ltm4678_read(unit, saddr, PMBUS_CMD_PAGE, &page, &len);
     if (rv != SHR_E_NONE) {
         return rv;
     }

     if (page != ch) {
         page = ch;

         LOG_VERBOSE(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "LTM4678 %d set page to %d\n"),
                      saddr, page));

         rv = ltm4678_write(unit, saddr, PMBUS_CMD_PAGE, &page, sizeof(char));
     }

     return rv;
}

/*!
 * \brief Device specific ioctl function wrapper.
 *
 * NOTE NOTE NOTE:
 * All tables (dac_calibrate_t) passed to the ioctl() have size > 1
 * and the index is always within this range.
 */
static int
ltm4678_ioctl(int unit,
              int saddr,
              int opcode,
              void *data,
              int channel)
{
    int rv = SHR_E_NONE;
    double fval;
    uint16_t dac;
    uint32_t datalen = 2;
    unsigned short voltage;
    i2c_device_t *dev;

    dev = ((ioctl_op_data_t *)data)->input.dev;

    switch (opcode) {
        case I2C_LTC_IOC_VOUT_READ:
            if ((rv = ltm4678_check_page(unit, saddr, channel)) < 0) {
                cli_out("Error: failed to set page %d in LTM4678 device.\n", channel);
                break;
            }
            if ((rv = ltm4678_wait_for_not_busy(unit, saddr)) < 0) {
                cli_out("Error: Device LTM4678 is busy.\n");
                break;
            }

            datalen = 2;
            if ((rv = ltm4678_read(unit, saddr,
                                   PMBUS_CMD_READ_VOUT, (void *)&dac, &datalen)) < 0) {
                cli_out("Error: Failed to read VOUT of LTM4678 Device.\n");
                break;
            }
            fval = dac;
            fval = L16_TO_V(fval );
            ((ioctl_op_data_t *)data)->output.data[0] = fval;
            break;

        case I2C_LTC_IOC_IOUT_READ:
            if ((rv = ltm4678_check_page(unit, saddr, channel)) < 0) {
                cli_out("Error: failed to set page %d in LTM4678 device.\n", channel);
                break;
            }
            if ((rv = ltm4678_wait_for_not_busy(unit, saddr)) < 0) {
                cli_out("Error: LTM4678 Device is busy.\n");
                break;
            }
            if ((rv = ltm4678_read(unit, saddr,
                                   PMBUS_CMD_READ_IOUT, (void *)&dac, &datalen)) < 0) {
                cli_out("Error:Failed to read current in LTM4678 Device.\n");
                break;
            }
            rv = ltm4678_l11_to_float(dac, &fval);
            ((ioctl_op_data_t *)data)->output.data[0] = fval*1000;
            break;

        case I2C_LTC_IOC_POUT_READ:
            datalen = 2;
            if ((rv = ltm4678_check_page(unit, saddr, channel)) < 0) {
                cli_out("Error: failed to set page %d in LTM4678 device.\n", channel);
                break;
            }
            if ((rv = ltm4678_wait_for_not_busy(unit, saddr)) < 0) {
                cli_out("Error: LTM4678 Device is busy.\n");
                return rv;
            }

            if ((rv = ltm4678_read(unit, saddr,
                                   PMBUS_CMD_READ_POUT, (void *)&dac, &datalen)) < 0) {
                cli_out("Error: failed to read power in LTM4678 device.\n");
                break;
            }

            rv = ltm4678_l11_to_float(dac, &fval);
            ((ioctl_op_data_t *)data)->output.data[0] = fval*1000;

            break;

       case I2C_LTC_IOC_VOUT_SET:
           /* Conversion of output voltage */
           fval = ((ioctl_op_data_t *)data)->input.voltage;
           voltage = V_TO_L16(fval);
           dac = voltage;

           if ((voltage > DEV_DAC_PARAMS(dev, channel)->dac_max_hwval)  ||
               (voltage < DEV_DAC_PARAMS(dev, channel)->dac_min_hwval)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "i2c %d: LTM4678 given voltage %2.3f V "
                                      "beyond range( max=%2.3f V, min=%2.3f V) for "
                                      "voltage VDD_%s \n"),
                           saddr,
                           (double) L16_TO_V((double) voltage),
                           (double) L16_TO_V((double) DEV_DAC_PARAMS(dev, channel)->dac_max_hwval),
                           (double) L16_TO_V((double) DEV_DAC_PARAMS(dev, channel)->dac_min_hwval),
                           DEV_DAC_PARAMS(dev, channel)->name));

               rv = SHR_E_PARAM;
               break;
           }

           /* Show what we are doing, for now ... */
           LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                                   "i2c %s: LTM4678 ioctl "
                                   "I2C_DAC_IOC_VOUT_SET : voltage = %d, channel = %d\n"),
                        "LTM4678", voltage, channel));

           rv = ltm4678_rail_write(unit, saddr,
                                   dev, PMBUS_CMD_VOUT_COMMAND, (void *)&dac, 2);
           break;

       default:
           LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                                   "i2c %s: ltm4678_ioctl: invalid opcode (%d)\n"),
                        "LTM4678", opcode));
           break;
    }

    return rv;
}

/*!
 * \brief Device specific init function.
 */
static int
ltm4678_init(int unit,
             int saddr,
             void *data,
             int channel)
{
    int rv = SHR_E_NONE;
    i2c_device_t *dev;
    uint8_t data8;
    dac_calibrate_t *record;

    dev = ((ioctl_op_data_t *)data)->input.dev;
    record = ((ioctl_op_data_t *)data)->input.cali_record;

    if (dev->priv_data == NULL) {
        dev->priv_data = sal_alloc(sizeof(device_data_t), "bcmaI2cLtm4678");
        if (dev->priv_data == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to allocate private data fo dev %s\n"),
                       "LTM4678"));
            return SHR_E_MEMORY;
        }
        sal_memset(dev->priv_data, 0, sizeof(device_data_t));

        /* Attach calibration record to dev */
        DEV_DAC_PARAMS(dev, channel) = record;
        DEV_DAC_PARAM_LEN(dev) = LTC2974_TOTAL_CHANNEL;
    } else {
        DEV_DAC_PARAMS(dev, channel) = record;
    }

    /* bit3 1: control the LTM4678 output through VOUT command
     *      0: control the output via VID input pins which is controlled by
     *         a PCF8574 device
     */
    if ((rv = ltm4678_write(unit, saddr,
                            PMBUS_CMD_CLEAR_FAULTS, (void *) &channel, 0)) < 0) {
        cli_out("Error: Failed to clear the faults of LTM4678 device.\n");
        return rv;
    }

    if ((rv = ltm4678_wait_for_not_busy(unit, saddr)) < 0) {
        cli_out("Error: LTM4678 Device is busy.\n");
        return rv;
    }

    channel = 1;
    if ((rv=ltm4678_read(unit, saddr, 0xFA,
                         (uint8_t *)&DEV_RAIL_SADDR(dev), (void *)&channel)) < 0) {
        cli_out("Error: Failed to read 0xFA of LTM4678 device.\n");
        return rv;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "ltm4678_init: %s, devNo=0x%x\n"),
                 "LTM4678", saddr));

    /* Setting Fault response to zero to prevent shut-down of device */
    data8 = 0x00;
    if ((rv = ltm4678_write(unit, saddr,
                            PMBUS_CMD_VOUT_OV_FAULT_RES, &data8, 1)) < 0) {
        cli_out("Error: failed to set OV fault response of LTM4678.\n");
        return rv;
    }

    if ((rv = ltm4678_wait_for_not_busy(unit, saddr)) < 0) {
        cli_out("Error: LTM4678 device is busy.\n");
        return rv;
    }

    data8 = 0x0A;
    if ((rv = ltm4678_write(unit, saddr,
                            PMBUS_CMD_ON_OFF_CONFIG, &data8, 1)) < 0) {
        cli_out("Error: failed to set CONFIG register of LTM4678 device.\n");
        return rv;
    }

    /* Switching on LTC4678 device */
    data8 = 0x80;
    if ((rv = ltm4678_write(unit, saddr,
                            PMBUS_CMD_OPERATION, &data8, 1)) < 0) {
        cli_out("Error: failed to set operation register of LTM4678 device.\n");
        return rv;
    }

    if ((rv = ltm4678_wait_for_not_busy(unit, saddr)) < 0) {
        cli_out("Error: LTM4678 Device is busy.\n");
        return rv;
    }

    return rv;
}

/*!
 * \brief Device specific unload function.
 */
static int
ltm4678_cleanup(int unit,
                int saddr,
                void *data,
                int channel)
{
    i2c_device_t *dev;

    dev = ((ioctl_op_data_t *)data)->input.dev;

    if (dev == NULL) {
        return SHR_E_INTERNAL;
    }

    if (dev->priv_data) {
        sal_free(dev->priv_data);
        dev->priv_data = NULL;
    }

    return SHR_E_NONE;
}

/*!
 * \brief LTM4678 voltage control Chip Driver callout
 */
i2c_driver_t bcma_i2c_ltm4678_driver = {
    0x0,
    0x0,
    LTM4678_DEVICE_TYPE,
    ltm4678_read,
    ltm4678_write,
    ltm4678_ioctl,
    ltm4678_init,
    ltm4678_cleanup,
    NULL,
};

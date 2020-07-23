/*! \file bcma_i2c_ltc2794.c
 *
 *  Application to control power regulator LTC2794.
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
#include <sal/sal_sleep.h>
#include <bcma/i2c/bcma_i2ccmd_i2c.h>
#include <bcma/i2c/bcma_i2c_device.h>
#include <bcmbd/bcmbd_i2c.h>
#include <bcma/io/bcma_io_file.h>
#include <bcmbd/bcmbd.h>

#define BSL_LOG_MODULE  BSL_LS_APPL_I2C

/* Device specific macros */

/* Divided by 2^13 for voltage conversion */
#define L16_TO_V(val)  ((double)(val)*100000/8192/100000)
#define V_TO_L16(val)  ((val)*8192)
#define L16_TO_UV(val) ((val)*100000/8192*10)
#define UV_TO_L16(val) ((val)/10*8192/100000)

#define POWER(exponent, input_val)  exponent < 0 ? \
                            (int)((int) (input_val) << (exponent*(-1))): \
                            (int)((int) (input_val*1000000) >> \
                            exponent)/1000000

static i2c_ltc_t *sense_resistor_config[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Local utility function.
 *
 * Convert a floating point value into a
 * LinearFloat5_11 formatted word
 */
static int
ltc2974_float_to_l11(double input_val, uint16_t *data)
{
    uint16_t uexponent, umantissa;

    /* set exponent to -16 */
    int16_t exponent = -16;

    /* extract mantissa from input value */
    int mantissa = POWER(exponent, input_val);

    /* Search for an exponent that produces
     * a valid 11-bit mantissa */
    do {
        if ((mantissa >= -1024) && (mantissa <= +1023)) {
            /* stop if mantissa valid */
            break;
        }

        exponent++;
        mantissa = POWER(exponent, input_val);
    } while (exponent < +15);

    /* Format the exponent of the L11 */
    uexponent = exponent << 11;

    /* Format the mantissa of the L11 */
    umantissa = mantissa & 0x07FF;

    /* Compute value as exponent | mantissa */
    *(data) = uexponent | umantissa;

    return SHR_E_NONE;
}

/*!
 * \brief Local utility function.
 *
 * Convert a LinearFloat5_11 formatted word
 * into a floating point value
 */
static int
ltc2974_l11_to_float(uint16_t input_val, void *data)
{
    /* extract exponent as MS 5 bits */
    int8_t exponent;

    /* extract mantissa as LS 11 bits */
    int16_t mantissa;

    exponent = input_val >> 11;
    mantissa = input_val & 0x7ff;

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
ltc2974_wait_for_not_busy(int unit, int saddr)
{
    int rv = SHR_E_NONE;
    uint8_t mfr_status;
    uint32_t usec, wait_usec;

    wait_usec = 0;
    usec = 10;

    while (wait_usec < 1000000) {

        rv = bcmbd_i2c_read_byte_data(unit, saddr,
                                      PMBUS_CMD_MFR_COMMON, &mfr_status);
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
                             "i2c %s :ltc2974 is busy !\n"),
                  "LTC2794"));
        rv = SHR_E_TIMEOUT;
    }

    return rv;
}

/*!
 * \brief Device specific read function.
 */
static int
ltc2974_read(int unit,
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
        rv = bcmbd_i2c_read_byte_data(unit, saddr, addr, data);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: LTC2974_read: "
                                "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                                "rv = %d\n"),
                     "LTC2794",
                     saddr, addr, *data, *len, rv));
    } else if (*len == 2) {
        /* reads a single word from a device, from a designated register*/
        rv = bcmbd_i2c_read_word_data(unit, saddr, addr, (uint16_t *)data);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: LTC2974_read: "
                                "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                                "rv = %d\n"),
                     "LTC2794",
                     saddr, addr, *(uint16_t *)data, *len, rv));
    } else {
        /* not supported for now */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: LTC2974_read fail: "
                                "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                     "LTC2794",
                     saddr, addr, *data, *len));
    }

    return rv;
}

/*!
 * \brief Device specific write function.
 */
static int
ltc2974_write(int unit,
              int saddr,
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
                                "i2c %s: LTC2974 write: "
                                "saddr = 0x%x, addr = 0x%x, len = %d\n"),
                     "LTC2794", saddr, addr, len));

        rv = bcmbd_i2c_write_byte(unit, saddr, addr);
    } else if (len == 1) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: LTC2974 write: "
                                "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                     "LTC2794", saddr, addr, *data, len));

        rv = bcmbd_i2c_write_byte_data(unit, saddr, addr, *data);
    } else if (len == 2) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: LTC2974 write: "
                                "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                     "LTC2794",
                     saddr, addr, *(uint16_t *)data, len));

        val = *(unsigned short *)data;
        rv = bcmbd_i2c_write_word_data(unit, saddr, addr, val);
    }

    return rv;
}

/*!
 * \brief Check per page (sub-channel) operation.
 */
static int
ltc2974_check_page(int unit, int saddr, int ch)
{
     int rv;
     uint8_t page;
     uint32_t len;

     len = sizeof(char);
     rv = ltc2974_read(unit, saddr, PMBUS_CMD_PAGE, &page, &len);
     if (rv != SHR_E_NONE) {
         return rv;
     }

     if (page != ch) {
         page = ch;
         LOG_VERBOSE(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "LTC2974 %d set page to %d\n"),
                      saddr, page));
         rv = ltc2974_write(unit, saddr, PMBUS_CMD_PAGE, &page, sizeof(char));
     }

     return rv;
 }

/*!
 * \brief Derive sub-channel voltage value.
 */
static int
ltc2974_get_ch_voltage(int unit, int saddr, int ch, unsigned short *voltage)
{
    int rv;
    uint32_t len;

    rv = ltc2974_check_page(unit, saddr, ch);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    len = sizeof(short);
    rv = ltc2974_read(unit, saddr, PMBUS_CMD_READ_VOUT, (uint8_t*)voltage,
                        &len);

    return rv;
}

/*!
 * \brief Set upper voltage bond.
 */
static int
ltc2974_set_ch_voltage_upper_with_supervision(int unit, int saddr, int ch,
                                              unsigned short voltage)
{
    return (ltc2974_write(unit,
                          saddr,
                          PMBUS_CMD_VOUT_COMMAND,
                          (uint8_t*)&voltage,
                          sizeof(short)));
}

/*!
 * \brief Set lower voltage bond.
 */
static int
ltc2974_set_ch_voltage_lower_with_supervision(int unit,
                                              int saddr,
                                              int ch,
                                              unsigned short voltage)
{
    return (ltc2974_write(unit,
                          saddr,
                          PMBUS_CMD_VOUT_COMMAND,
                          (uint8_t *)&voltage,
                          sizeof(short)));
}

/*!
 * \brief Set ADC channel voltage output.
 */
static int
ltc2974_set_ch_voltage(int unit, int saddr, int ch, unsigned short voltage)
{
    int rv = SHR_E_NONE;
    unsigned short old_vout;

    rv = ltc2974_get_ch_voltage(unit, saddr, ch, &old_vout);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    if (voltage >= old_vout) {
        rv = ltc2974_set_ch_voltage_upper_with_supervision(unit,
                                                           saddr, ch, voltage);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    } else {
        rv = ltc2974_set_ch_voltage_lower_with_supervision(unit,
                                                           saddr, ch, voltage);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    return rv;
}

/*!
 * \brief Set DAC channel voltage output.
 */
static int
ltc2974_dac_set_ch_voltage(int unit,
                           int saddr,
                           int channel,
                           unsigned short voltage,
                           i2c_device_t *dev)
{
    int rv;

    DEV_PRIVDATA_CHECK_RETURN(dev);

    rv = ltc2974_set_ch_voltage(unit, saddr, channel, voltage);

    if (SHR_SUCCESS(rv)) {
        /* Keep last value since DAC is write-only device */
        DEV_DAC_PARAMS(dev, channel)->dac_last_val = voltage;
    }

    return rv;
}

/*!
 * \brief Set min and max value.
 */

static int
ltc2974_setmin_max(int unit, int saddr, int channel, i2c_device_t *dev)
{
    int rv = SHR_E_NONE;
    int data, ch;
    uint16_t dac;
    i2c_ltc_t *sensor_config;

    DEV_PRIVDATA_CHECK_RETURN(dev);

    /* We did not receive the config from application layer */
    if (sense_resistor_config[unit] == NULL) {
        return (SHR_E_INTERNAL);
    } else {
        sensor_config = sense_resistor_config[unit];
    }

    /* Channel out of bound */
    if (channel >= LTC2974_TOTAL_CHANNEL) {
        return (SHR_E_INTERNAL);
    } else {
        /* Advance pointer to the correct channel */
        sensor_config = sensor_config + channel;
        ch = sensor_config->ch;
    }

    if (sensor_config->flag == 0) {
        rv = ltc2974_check_page(unit, saddr, ch);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        data = DEV_DAC_PARAMS(dev, channel)->dac_max_hwval;
        dac = (uint16_t) L16_RANGE_UPPER(data, 10000);
        rv = ltc2974_write(unit, saddr, PMBUS_CMD_VOUT_MAX, (void *)&dac, 2);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        dac = (uint16_t) L16_RANGE_UPPER(data, 7000);
        rv = ltc2974_wait_for_not_busy(unit, saddr);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        rv =  ltc2974_write(unit, saddr, PMBUS_CMD_VOUT_OV_FAULT_LIMIT,
                            (void *)&dac, 2);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        dac = (uint16_t) L16_RANGE_UPPER(data, 5000);
        rv = ltc2974_wait_for_not_busy(unit, saddr);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        rv = ltc2974_write(unit, saddr, PMBUS_CMD_VOUT_OV_WARN_LIMIT,
                           (void *)&dac, 2);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        dac = (uint16_t) L16_RANGE_UPPER(data, 2000);
        rv = ltc2974_wait_for_not_busy(unit, saddr);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        rv = ltc2974_write(unit, saddr, PMBUS_CMD_VOUT_MARGIN_HIGH,
                           (void *)&dac, 2);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        data = DEV_DAC_PARAMS(dev, channel)->dac_min_hwval;
        rv = ltc2974_wait_for_not_busy(unit, saddr);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        dac = (uint16_t) L16_RANGE_LOWER(data, 5000);
        rv = ltc2974_write(unit, saddr, PMBUS_CMD_VOUT_UV_FAULT_LIMIT,
                            (void *)&dac, 2);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        rv = ltc2974_write(unit, saddr, PMBUS_CMD_POWER_GOOD_ON,
                           (void *)&dac, 2);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        dac = (uint16_t) L16_RANGE_LOWER(data, 7000);
        rv = ltc2974_write(unit, saddr, PMBUS_CMD_POWER_GOOD_OFF,
                            (void *)&dac, 2);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        dac = (uint16_t) L16_RANGE_LOWER(data, 2000);
        rv = ltc2974_wait_for_not_busy(unit, saddr);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        rv = ltc2974_write(unit, saddr, PMBUS_CMD_VOUT_UV_WARN_LIMIT,
                           (void *)&dac, 2);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        dac = (uint16_t) L16_RANGE_LOWER(data, 1000);
        rv = ltc2974_wait_for_not_busy(unit, saddr);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        rv = ltc2974_write(unit, saddr, PMBUS_CMD_VOUT_MARGIN_LOW,
                           (void *)&dac, 2);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        /* Use the per board data to write to hw */
        dac = sensor_config->res_value;
        rv = ltc2974_write(unit, saddr,
                           PMBUS_CMD_IOUT_CAL_GAIN, (void *)&dac, 2);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        sal_usleep(500);

        /* Device Configured */
        sensor_config->flag = 1;
    }
    return rv;
}

/*
 * NOTE NOTE NOTE:
 * All tables (dac_calibrate_t) passed to the ioctl() have size > 1
 * and the index is always within this range.
 */
static int
ltc2974_ioctl(int unit,
              int saddr,
              int opcode,
              void *data,
              int channel)
{
    int rv = SHR_E_NONE;
    double fval;
    uint16_t dac;
    uint32_t datalen = 2;
    uint8_t data8;
    unsigned short voltage;
    int ch;
    i2c_device_t *dev;
    uint32_t board_id;
    i2c_ltc_t *sensor_config;

    if (data == NULL) {
        return SHR_E_INTERNAL;
    }

    /* Derive data from application input */
    board_id = ((ioctl_op_data_t *)data)->input.board_id;
    dev = ((ioctl_op_data_t *)data)->input.dev;
    sense_resistor_config[unit] = ((ioctl_op_data_t *)data)->input.sensor_config;
    sensor_config = sense_resistor_config[unit];

    if ((dev == NULL) ||
        (sensor_config == NULL)) {
        return SHR_E_INTERNAL;
    }

    switch (opcode) {
        case I2C_LTC_IOC_VOUT_READ:
            if ((rv = ltc2974_check_page(unit, saddr, channel)) < 0) {
                cli_out("Error: failed to set page %d in LTC2974 device.\n", channel);
                break;
            }

            if ((rv = ltc2974_write(unit, saddr,
                                    PMBUS_CMD_CLEAR_FAULTS, &data8, 0)) < 0) {
                cli_out("Error: Failed to clear the faults of LTC2974 device.\n");
                break;
            }

            if ((rv = ltc2974_wait_for_not_busy(unit, saddr)) < 0) {
                cli_out("Error: LTC2974 device is busy.\n");
                break;
            }

           /* Setting Fault response to zero to prevent shut-down of device */
            data8 = 0x00;
            if ((rv = ltc2974_write(unit, saddr,
                                    PMBUS_CMD_VOUT_OV_FAULT_RES, &data8, 1)) < 0) {
                cli_out("Error: failed to set OV fault response of LTC2974.\n");
                break;
            }
            if ((rv = ltc2974_wait_for_not_busy(unit, saddr)) < 0) {
                cli_out("Error: LTC2974 device is busy.\n");
                break;
            }

            /* For future - if the board needs to turn on power later */
            if (board_id == 0xffff ) {
                /* Switching on LTC2974 device */
                data8 = 0x40;
                if ((rv = ltc2974_write(unit, saddr, PMBUS_CMD_OPERATION, &data8, 1)) < 0) {
                    cli_out("Error: failed to set operation register of LTC2974 device.\n");
                    break;
                }

                if ((rv = ltc2974_wait_for_not_busy(unit, saddr)) < 0) {
                    cli_out("Error: LTC2974 Device is busy.\n");
                    break;
                }
            }

            data8 = 0x0A;
            if ((rv = ltc2974_write(unit, saddr, PMBUS_CMD_ON_OFF_CONFIG, &data8, 1)) < 0) {
                cli_out("Error: failed to set CONFIG register of LTC2974 device.\n");
                break;
            }
            if ((rv = ltc2974_wait_for_not_busy(unit, saddr)) < 0) {
                cli_out("Error: Device LTC2974 is busy.\n");
                break;
            }

            datalen = 2;
            rv = ltc2974_read(unit, saddr, PMBUS_CMD_READ_VOUT, (void *)&dac, &datalen);
            if (rv < 0) {
                cli_out("Error: Failed to read VOUT of LTC2974 Device.\n");
                break;
            }
            fval = dac;
            fval = L16_TO_V(fval );
            ((ioctl_op_data_t *)data)->output.data[0] = fval;

           break;

         case I2C_LTC_IOC_IOUT_READ:
             if ((rv = ltc2974_check_page(unit, saddr, channel)) < 0) {
                 cli_out("Error: failed to set page %d in LTC2974 device.\n", channel);
                 break;
             }
             rv = ltc2974_float_to_l11(LTC2974_RES_CONFIG, &dac);

             sensor_config = sensor_config + channel;

             dac = sensor_config->res_value;
             if ((rv = ltc2974_write(unit, saddr,
                                     PMBUS_CMD_IOUT_CAL_GAIN, (void *)&dac, 2)) < 0) {
                 cli_out("Error: failed to set IOUT_CAL_GAIN of LTC2974 device.\n");
                 break;
             }

             sal_usleep(500);

             if ((rv = ltc2974_wait_for_not_busy(unit, saddr)) < 0) {
                 cli_out("Error: LTC2974 Device is busy.\n");
                 break;
             }

             rv = ltc2974_read(unit, saddr, PMBUS_CMD_READ_IOUT, (void *)&dac, &datalen);
             if (rv < 0) {
                 cli_out("Error:Failed to read current in LTC2974 Device.\n");
                 break;
             }

             rv = ltc2974_l11_to_float(dac, &fval);

             ((ioctl_op_data_t *)data)->output.data[0] = fval*1000;

             break;

         case I2C_LTC_IOC_RCONFIG_SET:
             /* For future use in different RCONFIGs */
             /* Initialising sense resistor */
             if (sensor_config == NULL) {
                     LOG_CLI((BSL_META_U(unit,
                                         "Error: Invalid chip: %d for "
                                         "LTC2974 device %s .\n"),
                              channel, "LTC2794"));
                     rv = SHR_E_NOT_FOUND;
             }
             break;

         case I2C_LTC_IOC_POUT_READ:
             datalen = 2;
             if ((rv = ltc2974_check_page(unit, saddr, channel)) < 0) {
                 cli_out("Error: failed to set page %d in LTC2974 device.\n", channel);
                 break;
             }

             if ((rv = ltc2974_read(unit, saddr,
                                    PMBUS_CMD_READ_POUT, (void *)&dac, &datalen)) < 0) {
                 cli_out("Error: failed to read power in LTC2974 device.\n");
                 break;
             }

             /* Furture board only - Switching off LTC2974  */
             if (board_id == 0xffff) {
                 data8 = 0x00;
                 if ((rv = ltc2974_write(unit, saddr,
                                         PMBUS_CMD_OPERATION, &data8, 1)) < 0) {
                     cli_out("Error: failed to set operation register of LTC2974 device.\n");
                     break;
                 }
                 if ((rv = ltc2974_wait_for_not_busy(unit, saddr)) < 0) {
                     cli_out("Error: LTC2974 Device is busy.\n");
                     break;
                 }
             }

             rv = ltc2974_l11_to_float(dac, &fval);
             ((ioctl_op_data_t *)data)->output.data[0] = fval*1000;

             break;

        /* Upload calibration table */
        case I2C_DAC_IOC_CALTAB_SET:
            /* dac_params  initialised */
            DEV_DAC_PARAMS(dev, channel) = ((ioctl_op_data_t *)data)->input.cali_record;
            DEV_DAC_PARAM_LEN(dev) = channel;
            break;

        case I2C_LTC_IOC_CONFIG_SET:
            /* Initialising sense resistor */
            if (DEV_DAC_PARAMS(dev, channel)) {
                /* Setting max and min values for dac */
                rv = ltc2974_setmin_max(unit, saddr, channel, dev);
            }
            break;

        case I2C_DAC_IOC_VOUT_SET:
            /* Set output voltage */
            if (DEV_DAC_PARAMS(dev, channel)) {
                fval = ((ioctl_op_data_t *)data)->input.voltage;
                voltage = V_TO_L16(fval);
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "i2c %s: LTC2974 ioctl I2C_DAC_IOC_VOUT_SET: "
                                        "voltage = %d, channel = %d\n"),
                             "LTC2794", voltage, channel));

                if ((voltage > DEV_DAC_PARAMS(dev, channel)->dac_max_hwval)  ||
                    (voltage < DEV_DAC_PARAMS(dev, channel)->dac_min_hwval)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "i2c %d: LTC2974 given voltage %d "
                                          "beyond range for ch %d, %d, %d\n"),
                                saddr, voltage, channel,
                                DEV_DAC_PARAMS(dev, channel)->dac_max_hwval,
                                DEV_DAC_PARAMS(dev, channel)->dac_min_hwval));
                    rv = SHR_E_PARAM;
                    break;
                }
                if ((rv = ltc2974_check_page(unit, saddr, channel)) < 0) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Error: failed to set page %d in "
                                        "LTC2974 device name: %s.\n"),
                             channel, "LTC2794"));
                }
                rv = ltc2974_dac_set_ch_voltage(unit, saddr, channel, voltage, dev);
            }
            break;

        case I2C_LTC_IOC_VOUT_SET:
            /* Finding and setting page numbers */
            if (DEV_DAC_PARAMS(dev, channel)) {
                /* Advance to the correct entry */
                sensor_config = sensor_config + channel;

                ch = sensor_config->ch;
                if ((rv=ltc2974_check_page(unit, saddr, ch)) < 0) {
                    LOG_CLI((BSL_META_U(unit, "Error: failed to set page %d in "
                            "LTC2974 device.\n"), ch));
                    break;
                }
                /* Conversion of output voltage */
                fval = ((ioctl_op_data_t *)data)->input.voltage;
                voltage = V_TO_L16(fval);

                if ((voltage < DEV_DAC_PARAMS(dev, channel)->dac_min_hwval)||
                    (voltage > DEV_DAC_PARAMS(dev, channel)->dac_max_hwval)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "i2c %d: LTC2974 given voltage %2.3f V "
                                          "beyond range( max=%2.3f V, min=%2.3f V) for "
                                          "voltage VDD_%s \n"),
                               saddr,
                               (double) L16_TO_V(voltage),
                               (double) L16_TO_V(DEV_DAC_PARAMS(dev, channel)->dac_max_hwval),
                               (double) L16_TO_V(DEV_DAC_PARAMS(dev, channel)->dac_min_hwval),
                               DEV_DAC_PARAMS(dev, channel)->name));

                    rv = SHR_E_PARAM;
                    break;
                }

                dac = voltage;

                /* Show what we are doing, for now ... */
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "i2c %s: LTC2974 ioctl "
                                        "I2C_DAC_IOC_VOUT_SET : voltage = %d, channel = %d\n"),
                             "LTC2794", voltage, channel));

                rv = ltc2974_write(unit, saddr, PMBUS_CMD_VOUT_COMMAND,(void *) &dac, 2);

                /* Keep last value since DAC is write-only device */
                DEV_DAC_PARAMS(dev, channel)[channel].dac_last_val = dac;
            }
            break;

        default:
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "i2c %s: ltc2974_ioctl: invalid opcode (%d)\n"),
                         "LTC2794", opcode));
            break;
    }

    return rv;
}

static int
ltc2974_init(int unit,
             int saddr,
             void *data,
             int channel)
{
    int rv = SHR_E_NONE;
    i2c_device_t *dev;
    dac_calibrate_t *record;

    if (data == NULL) {
        return SHR_E_INTERNAL;
    }

    /* Derive data from application input */
    dev = ((ioctl_op_data_t *)data)->input.dev;
    record = ((ioctl_op_data_t *)data)->input.cali_record;

    if (dev->priv_data == NULL) {
        dev->priv_data = sal_alloc(sizeof(device_data_t), "bcmaI2cLtc2794");
        if (dev->priv_data == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to allocate private data fo dev %s\n"),
                       "LTC2794"));
            return SHR_E_MEMORY;
        }

        /* Attach calibration record to dev */
        DEV_DAC_PARAMS(dev, channel) = record;
        DEV_DAC_PARAM_LEN(dev) = LTC2974_TOTAL_CHANNEL;
    } else {
        DEV_DAC_PARAMS(dev, channel) = record;
    }

    /* bit3 1: control the LTC2974 output through VOUT command
     *      0: control the output via VID input pins which is controlled by
     *         a PCF8574 device
     */
    if ((rv = ltc2974_write(unit, saddr,
                            PMBUS_CMD_CLEAR_FAULTS,(void *) &channel, 0)) < 0) {
        cli_out("Error: Failed to clear the faults of LTC2974 device.\n");
        return rv;
    }

    if ((rv = ltc2974_wait_for_not_busy(unit, saddr)) < 0) {
        cli_out("Error: LTC2974 Device is busy.\n");
        return rv;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "ltc2974_init: %s, devNo=0x%x\n"),
                 "LTC2794", saddr));

    return rv;
}

/*!
 * \brief Device specific unload function.
 */
static int
ltc2974_cleanup(int unit,
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
 * \brief LTC2974 voltage control Chip Driver callout
 */
i2c_driver_t bcma_i2c_ltc2974_driver = {
    0x0,
    0x0,
    LTC2974_DEVICE_TYPE,
    ltc2974_read,
    ltc2974_write,
    ltc2974_ioctl,
    ltc2974_init,
    ltc2974_cleanup,
    NULL,
};

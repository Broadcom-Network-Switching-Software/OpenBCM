/*! \file bcma_i2c_ltm4676.c
 *
 *  Application to control power regulator ltm4676.
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

/* Channel in independent (0) or combined (1) mode */
static bool combo_chan[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Local utility function.
 *
 * Convert a LinearFloat5_11 formatted word
 * into a floating point value
 */
static int
ltm4676_l11_to_float(uint16_t input_val, double *data)
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
    *data = exponent < 0 ?
                (double) ((mantissa*1000000) >> (exponent*(-1))) / 1000000 :
                (double) (mantissa << exponent);

    return SHR_E_NONE;
}

/*!
 * \brief Derive utility routine - wait.
 */
static int
ltm4676_wait_for_not_busy(int unit, int saddr)
{
    int rv = SHR_E_NONE;
    uint8_t mfr_status;
    uint32_t usec, wait_usec;

    wait_usec = 0;
    usec = 10;

    while (wait_usec < 1000000) {
        rv =  bcmbd_i2c_read_byte_data(unit, saddr, PMBUS_CMD_MFR_COMMON,
                                       &mfr_status);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        if ((mfr_status & 0x68) == 0x68) {
            /* Bit 6 : MODULE not busy */
            /* Bit 5 : CALCULATIONS not pending */
            /* Bit 4 : OUTPUT not in transition */
            /* Bit 3 : NVM initialized */
            break;
        } else {
            sal_usleep(usec);
            wait_usec += usec;
        }
    }

    if ((mfr_status & 0x68) != 0x68) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "i2c %s: device busy.\n"),
                 LTM4676));
        rv = SHR_E_TIMEOUT;
    }

    return rv;
}

/*!
 * \brief Device specific read function.
 */
static int
ltm4676_read(int unit, int saddr, uint16_t addr, uint8_t *data, uint32_t *len)
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
                                "i2c %s: read: saddr = 0x%x, addr = 0x%x, "
                                "data = 0x%x, len = %d, rv = %d\n"),
                    LTM4676, saddr, addr, *data, *len, rv));
    } else if (*len == 2) {
        /* reads a single word from a device, from a designated register*/
        rv = bcmbd_i2c_read_word_data(unit, saddr, addr, (uint16_t *)data);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: read word data: saddr = 0x%x, "
                                "addr = 0x%x, data = 0x%x, len = %d, "
                                "rv = %d\n"),
                    LTM4676, saddr, addr, *(uint16_t *)data, *len, rv));
    } else {
        /* not supported for now */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: read fail: saddr = 0x%x, "
                                "addr = 0x%x, data = 0x%x, len = %d\n"),
                    LTM4676, saddr, addr, *data, *len));
    }

    return rv;
}

/*!
 * \brief Device specific write function.
 */
static int
ltm4676_write_op(int unit, uint8_t saddr, uint16_t addr, uint8_t *data,
                 uint32_t len)
{
    int rv = SHR_E_NONE;
    unsigned short val;

    if (len == 0) {
        /* simply writes command code to device */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: write: saddr = 0x%x, "
                                "addr = 0x%x, len = %d\n"),
                    LTM4676, saddr, addr, len));
        rv = bcmbd_i2c_write_byte(unit, saddr, addr);
    } else if (len == 1) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: write: saddr = 0x%x, "
                                "addr = 0x%x, data = 0x%x, len = %d\n"),
                    LTM4676, saddr, addr, *data, len));
        rv = bcmbd_i2c_write_byte_data(unit, saddr, addr, *data);
    } else if (len == 2) {
         LOG_VERBOSE(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "i2c %s: write: saddr = 0x%x, "
                                 "addr = 0x%x, data = 0x%x, len = %d\n"),
                     LTM4676, saddr, addr, *(uint16_t *)data, len));
        val = *(unsigned short *)data;
        rv = bcmbd_i2c_write_word_data(unit, saddr, addr, val);
    }

    return rv;
}

/*!
 * \brief Device specific write function wrapper.
 */
static int
ltm4676_write(int unit, int saddr, uint16_t addr, uint8_t *data, uint32_t len)
{
    return (ltm4676_write_op(unit, saddr, addr, data, len));
}

/*!
 * \brief Write to rail bus.
 */
static int
ltm4676_rail_write(int unit, int saddr, i2c_device_t *dev, uint16_t addr,
                   uint8_t *data, uint32_t len)
{
    uint8_t rail_saddr = 0;

    /* Rail address is read back from device from 0xFA read command */
    rail_saddr = DEV_RAIL_SADDR(dev);
    if (rail_saddr == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "i2c %s: rail saddr not set.\n"),
                  LTM4676));
        return SHR_E_INTERNAL;
    }

    return (ltm4676_write_op(unit, rail_saddr, addr, data, len));
}

/*!
 * \brief Check per page (sub-channel) operation.
 */
static int
ltm4676_check_page(int unit, int saddr, int ch)
{
     int rv;
     uint8_t page;
     uint32_t len;

     len = sizeof(char);
     rv = ltm4676_read(unit, saddr, PMBUS_CMD_PAGE, &page, &len);
     if (rv != SHR_E_NONE) {
         return rv;
     }

     if (page != ch) {
         page = ch;

         LOG_VERBOSE(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "i2c %s: %d set page to %d\n"),
                     LTM4676, saddr, page));

         rv = ltm4676_write(unit, saddr, PMBUS_CMD_PAGE, &page, sizeof(char));
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
ltm4676_ioctl(int unit, int saddr, int opcode, void *data, int channel)
{
    int rv = SHR_E_NONE;
    double fval;
    uint16_t dac;
    uint32_t datalen = 2;
    unsigned short voltage;
    uint8_t other_chan = 0;
    i2c_device_t *dev;

    dev = ((ioctl_op_data_t *)data)->input.dev;

    switch (opcode) {
        case I2C_LTC_IOC_VOUT_READ:
            if ((rv = ltm4676_check_page(unit, saddr, channel)) < 0) {
                cli_out("Error: failed to set page %d in %s device.\n",
                        channel, LTM4676);
                break;
            }
            if ((rv = ltm4676_wait_for_not_busy(unit, saddr)) < 0) {
                cli_out("Error: Device %s is busy.\n", LTM4676);
                break;
            }

            datalen = 2;
            if ((rv = ltm4676_read(unit, saddr, PMBUS_CMD_READ_VOUT,
                                   (void *)&dac, &datalen)) < 0) {
                cli_out("Error: Failed to read VOUT of %s Device.\n", LTM4676);
                break;
            }
            fval = dac;
            fval = L16_TO_V(fval);
            ((ioctl_op_data_t *)data)->output.data[0] = fval;
            break;

        case I2C_LTC_IOC_IOUT_READ:
            if ((rv = ltm4676_check_page(unit, saddr, channel)) < 0) {
                cli_out("Error: failed to set page %d in %s device.\n",
                        channel, LTM4676);
                break;
            }
            if ((rv = ltm4676_wait_for_not_busy(unit, saddr)) < 0) {
                cli_out("Error: %s Device is busy.\n", LTM4676);
                break;
            }
            if ((rv = ltm4676_read(unit, saddr, PMBUS_CMD_READ_IOUT,
                                   (void *)&dac, &datalen)) < 0) {
                cli_out("Error: failed to read current in %s Device.\n",
                        LTM4676);
                break;
            }
            rv = ltm4676_l11_to_float(dac, &fval);
            ((ioctl_op_data_t *)data)->output.data[0] = fval*1000;

            /* When in combined mode, add current from the other channel */
            if (combo_chan[unit]) {
                if (channel == 0) {
                    other_chan = 1;
                } else if (channel == 1) {
                    other_chan = 0;
                } else {
                    cli_out("Error: %s invalid chan %d.\n", LTM4676, channel);
                    break;
                }

                if ((rv = ltm4676_check_page(unit, saddr, other_chan)) < 0) {
                    cli_out("Error: failed to set page %d in %s device.\n",
                            other_chan, LTM4676);
                    break;
                }
                if ((rv = ltm4676_wait_for_not_busy(unit, saddr)) < 0) {
                    cli_out("Error: %s Device is busy.\n", LTM4676);
                    break;
                }
                if ((rv = ltm4676_read(unit, saddr, PMBUS_CMD_READ_IOUT,
                                       (void *)&dac, &datalen)) < 0) {
                    cli_out("Error: failed to read current in %s Device.\n",
                            LTM4676);
                    break;
                }
                rv = ltm4676_l11_to_float(dac, &fval);
                ((ioctl_op_data_t *)data)->output.data[0] += fval*1000;
            }
            break;

        case I2C_LTC_IOC_POUT_READ:
            datalen = 2;
            if ((rv = ltm4676_check_page(unit, saddr, channel)) < 0) {
                cli_out("Error: failed to set page %d in %s device.\n",
                        channel, LTM4676);
                break;
            }
            if ((rv = ltm4676_wait_for_not_busy(unit, saddr)) < 0) {
                cli_out("Error: %s Device is busy.\n", LTM4676);
                return rv;
            }

            if ((rv = ltm4676_read(unit, saddr, PMBUS_CMD_READ_POUT,
                                   (void *)&dac, &datalen)) < 0) {
                cli_out("Error: failed to read power in %s device.\n", LTM4676);
                break;
            }

            rv = ltm4676_l11_to_float(dac, &fval);
            ((ioctl_op_data_t *)data)->output.data[0] = fval*1000;

            /* When in combined mode, add power from the other channel */
            if (combo_chan[unit]) {
                if (channel == 0) {
                    other_chan = 1;
                } else if (channel == 1) {
                    other_chan = 0;
                } else {
                    cli_out("Error: %s invalid chan %d.\n", LTM4676, channel);
                    break;
                }

                if ((rv = ltm4676_check_page(unit, saddr, other_chan)) < 0) {
                    cli_out("Error: failed to set page %d in %s device.\n",
                            other_chan, LTM4676);
                    break;
                }
                if ((rv = ltm4676_wait_for_not_busy(unit, saddr)) < 0) {
                    cli_out("Error: %s Device is busy.\n", LTM4676);
                    break;
                }
                if ((rv = ltm4676_read(unit, saddr, PMBUS_CMD_READ_POUT,
                                       (void *)&dac, &datalen)) < 0) {
                    cli_out("Error: failed to read power in %s device.\n",
                            LTM4676);
                    break;
                }
                rv = ltm4676_l11_to_float(dac, &fval);
                ((ioctl_op_data_t *)data)->output.data[0] += fval*1000;
            }

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
                                      "i2c %d: %s given voltage %2.3f V "
                                      "beyond range(max=%2.3fV, min=%2.3fV) "
                                      "for voltage VDD_%s \n"),
                           saddr, LTM4676,
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
                                   "i2c %s: ioctl I2C_DAC_IOC_VOUT_SET: "
                                   "voltage = %d, channel = %d\n"),
                       LTM4676, voltage, channel));

           rv = ltm4676_rail_write(unit, saddr, dev, PMBUS_CMD_VOUT_COMMAND,
                                   (void *)&dac, 2);
           break;

       default:
           LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                                   "i2c %s: ioctl: invalid opcode (%d)\n"),
                       LTM4676, opcode));
           break;
    }

    return rv;
}

/*!
 * \brief Device specific init function.
 */
static int
ltm4676_init(int unit, int saddr, void *data, int chan)
{
    i2c_device_t *dev;
    uint8_t data8, rail_addr;
    uint32_t len;
    dac_calibrate_t *record;

    SHR_FUNC_ENTER(unit);

    dev = ((ioctl_op_data_t *)data)->input.dev;
    record = ((ioctl_op_data_t *)data)->input.cali_record;

    if (dev->priv_data == NULL) {
        dev->priv_data = sal_alloc(sizeof(device_data_t), "bcmaI2cLtm4676");
        if (dev->priv_data == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to allocate private data fo dev %s\n"),
                      LTM4676));
            return SHR_E_MEMORY;
        }
        sal_memset(dev->priv_data, 0, sizeof(device_data_t));

        /* Attach calibration record to dev */
        DEV_DAC_PARAMS(dev, chan) = record;
        DEV_DAC_PARAM_LEN(dev) = LTM4676_TOTAL_CHANNEL;
    } else {
        DEV_DAC_PARAMS(dev, chan) = record;
    }

    /* bit3 1: control the LTM4676 output through VOUT command
     *      0: control the output via VID input pins which is controlled by
     *         a PCF8574 device
     */
    SHR_IF_ERR_EXIT
        (ltm4676_write(unit, saddr, PMBUS_CMD_CLEAR_FAULTS, (void *) &chan, 0));

    SHR_IF_ERR_EXIT
        (ltm4676_wait_for_not_busy(unit, saddr));

    /* Read back rail address */
    len = 1;
    SHR_IF_ERR_EXIT
        (ltm4676_read(unit, saddr, PMBUS_CMD_MFR_RAIL_ADDR,
                      (uint8_t *)&rail_addr, (void *)&len));

    SHR_IF_ERR_EXIT
        (ltm4676_wait_for_not_busy(unit, saddr));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "init: %s, devNo=0x%x, railAddr=0x%x\n"),
                LTM4676, saddr, rail_addr));

    DEV_RAIL_SADDR(dev) = rail_addr;
    if (rail_addr != 0x80) {
        combo_chan[unit] = true;
    } else {
        combo_chan[unit] = false;
    }

    /* Setting Fault response to zero to prevent shut-down of device */
    data8 = 0x00;
    SHR_IF_ERR_EXIT
        (ltm4676_write(unit, saddr, PMBUS_CMD_VOUT_OV_FAULT_RES, &data8, 1));

    SHR_IF_ERR_EXIT
        (ltm4676_wait_for_not_busy(unit, saddr));

    data8 = 0x0A;
    SHR_IF_ERR_EXIT
        (ltm4676_write(unit, saddr, PMBUS_CMD_ON_OFF_CONFIG, &data8, 1));

    SHR_IF_ERR_EXIT
        (ltm4676_wait_for_not_busy(unit, saddr));

    data8 = 0x80;
    SHR_IF_ERR_EXIT
        (ltm4676_write(unit, saddr, PMBUS_CMD_OPERATION, &data8, 1));

    SHR_IF_ERR_EXIT
        (ltm4676_wait_for_not_busy(unit, saddr));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Device specific unload function.
 */
static int
ltm4676_cleanup(int unit, int saddr, void *data, int channel)
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
 * \brief LTM4676 voltage control Chip Driver callout
 */
i2c_driver_t bcma_i2c_ltm4676_driver = {
    0x0,
    0x0,
    LTM4676_DEVICE_TYPE,
    ltm4676_read,
    ltm4676_write,
    ltm4676_ioctl,
    ltm4676_init,
    ltm4676_cleanup,
    NULL,
};

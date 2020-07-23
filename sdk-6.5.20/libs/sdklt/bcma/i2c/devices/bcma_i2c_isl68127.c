/*! \file bcma_i2c_isl68127.c
 *
 *  Application to control power regulator ISL68127.
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
#include <bcma/i2c/bcma_i2ccmd_i2c.h>
#include <bcma/i2c/bcma_i2c_device.h>
#include <bcmbd/bcmbd_i2c.h>
#include <bcma/io/bcma_io_file.h>
#include <bcmbd/bcmbd.h>

#define BSL_LOG_MODULE  BSL_LS_APPL_I2C

/* Device specific Macros */

/* Divided by 1000 for voltage conversion */
#define L16_TO_V(val)  ((val)*100000/1000/100000)
#define V_TO_L16(val)  ((val)*1000)
#define L16_TO_UV(val) ((val)*100000/1000*10)
#define UV_TO_L16(val) ((val)/10*1000/100000)


/*!
 * \brief Write one byte to device.
 */
static int
write_one_byte(int unit,
               uint8_t saddr,
               uint8_t command,
               uint8_t arg0)
{
    return bcmbd_i2c_write_byte_data(unit, saddr, command, arg0);
}

/*!
 * \brief Write two bytes to device.
 */
static int
write_two_bytes(int unit,
                uint8_t saddr,
                uint8_t command,
                uint8_t arg0,
                uint8_t arg1)
{
    uint16_t value = (arg1 << 8) | arg0;

    return bcmbd_i2c_write_word_data(unit, saddr, command, value);
}

/*!
 * \brief Write four bytes to device.
 */
static int
write_four_bytes(int unit,
                 uint8_t saddr,
                 uint8_t command,
                 uint8_t arg0,
                 uint8_t arg1,
                 uint8_t arg2,
                 uint8_t arg3)
{
    uint8_t buf[4];

    buf[0] = arg0;
    buf[1] = arg1;
    buf[2] = arg2;
    buf[3] = arg3;

    return bcmbd_i2c_multi_write(unit, saddr, command, 4, buf);
}

/*!
 * \brief Disable voltage output.
 */
static int
disable_load_line(int unit, int saddr) {
    int rv;

    /* Disables load line */
    rv = write_two_bytes(unit, saddr, 0xf7, 0x9b, 0xe6);
    if (rv < 0) {
        return rv;
    }

    rv = write_four_bytes(unit, saddr, 0xf5, 0x05, 0x86, 0, 0);
    if (rv < 0) {
        return rv;
    }

    rv = write_two_bytes(unit, saddr, 0xf7, 0x85, 0xe6);
    if (rv < 0) {
        return rv;
    }

    rv = write_four_bytes(unit,saddr, 0xf5, 0, 0, 0, 0);
    if (rv < 0) {
        return rv;
    }

    rv = write_two_bytes(unit, saddr, 0xf7, 0xb4, 0xe6);
    if (rv < 0) {
        return rv;
    }

    rv = write_four_bytes(unit, saddr, 0xf5, 0, 0, 0, 0);
    if (rv < 0) {
        return rv;
    }

    rv = write_two_bytes(unit, saddr, 0xf7, 0xb2, 0xe6);
    if (rv < 0) {
        return rv;
    }

    rv = write_four_bytes(unit, saddr, 0xf5, 0, 0, 0, 0);
    if (rv < 0) {
        return rv;
    }

    rv = write_one_byte(unit, saddr, 0x00, 0x01);
    if (rv < 0) {
        return rv;
    }

    rv = write_two_bytes(unit, saddr, 0x28, 0, 0);
    if (rv < 0) {
        return rv;
    }

    rv = write_two_bytes(unit, saddr, 0xf7, 0xa3, 0xe3);
    if (rv < 0) {
        return rv;
    }

    rv = write_four_bytes(unit, saddr, 0xf5, 0, 0, 0, 0);
    if (rv < 0) {
        return rv;
    }

    rv = write_two_bytes(unit,saddr, 0xe7, 0x01, 0x0);

    return rv;
}

/*!
 * \brief Device specific read function.
 */
static int
isl68127_read(int unit,
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
        /* Reads a single byte from a device, from a designated register */
        rv = bcmbd_i2c_read_byte_data(unit, saddr, addr,data);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: ISL68127_read: "
                                "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                                "rv = %d\n"),
                     "ISL68127",
                     saddr, addr, *data, *len, rv));
    } else if (*len == 2) {
        /* Reads a single word from a device, from a designated register */
        rv = bcmbd_i2c_read_word_data(unit, saddr, addr,(uint16_t *)data);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: ISL68127_read: "
                                "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                                "rv = %d\n"),
                     "ISL68127",
                     saddr, addr, *(uint16_t *)data, *len, rv));
    } else {
        /* Not supported for now */
         LOG_VERBOSE(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "i2c %s: ISL68127_read fail: "
                                 "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                      "ISL68127",
                      saddr, addr, *data, *len));
    }

    return rv;
}

/*!
 * \brief Device specific write function.
 */
static int
isl68127_write_op(int unit,
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
                                "i2c %s: ISL68127 write: "
                                "saddr = 0x%x, addr = 0x%x, len = %d\n"),
                     "ISL68127", saddr, addr, len));

        rv = bcmbd_i2c_write_byte(unit, saddr, addr);
    } else if (len == 1) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: ISL68127 write: "
                                "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                     "ISL68127", saddr, addr, *data, len));

        rv = bcmbd_i2c_write_byte_data(unit, saddr, addr, *data);
    } else if (len == 2) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "i2c %s: ISL68127 write: "
                                "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                     "ISL68127",
                     saddr, addr, *(uint16_t *)data, len));

        val = *(unsigned short *)data;
        rv = bcmbd_i2c_write_word_data(unit, saddr, addr, val);
    }
    return rv;
}

/*!
 * \brief Device specific write function wrapper.
 */
static int
isl68127_write(int unit,
               int saddr,
               uint16_t addr,
               uint8_t *data,
               uint32_t len)
{
    return (isl68127_write_op(unit, saddr, addr, data, len));
}

/*!
 * \brief Check per page (sub-channel) operation.
 */
static int
isl68127_check_page(int unit, int saddr, int ch)
{
     int rv;
     uint8_t page;
     uint32_t len;

     len = sizeof(char);
     rv = isl68127_read(unit, saddr, PMBUS_CMD_PAGE, &page, &len);
     if (rv != SHR_E_NONE) {
         return rv;
     }

     if (page != ch) {
         page = ch;
         LOG_VERBOSE(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "ISL68127 %d set page to %d\n"),
                      saddr, page));
         rv = isl68127_write(unit, saddr, PMBUS_CMD_PAGE, &page, sizeof(char));
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
isl68127_ioctl(int unit,
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
        case PMBUS_IOC_VOUT_READ:
            if ((rv = isl68127_check_page(unit, saddr, channel)) < 0) {
                cli_out("Error: failed to set page %d in ISL68127 device.\n", channel);
                break;
            }

            datalen = 2;
            rv = isl68127_read(unit, saddr, PMBUS_CMD_READ_VOUT, (void *)&dac, &datalen);
            if (rv < 0) {
                cli_out("Error: Failed to read VOUT of ISL68127 Device.\n");
                break;
            }

            fval = dac;
            if (channel == 0) {
                fval = ((fval)*120000/1000/100000);
            } else {
                fval = L16_TO_V(fval);
            }

            ((ioctl_op_data_t *)data)->output.data[0] = fval;
            break;

        case PMBUS_IOC_IOUT_READ:
            if ((rv = isl68127_check_page(unit, saddr, channel)) < 0) {
                cli_out("Error: failed to set page %d in ISL68127 device.\n", channel);
                break;
            }

            rv = isl68127_read(unit, saddr, PMBUS_CMD_READ_IOUT, (void *)&dac, &datalen);
            if (rv < 0) {
                cli_out("Error:Failed to read current in ISL68127 Device.\n");
                break;
            }

            /* (dac/10)*1000 */
            ((ioctl_op_data_t *)data)->output.data[0] = dac*100;
            break;

        case PMBUS_IOC_POUT_READ:
            datalen = 2;
            if ((rv = isl68127_check_page(unit, saddr, channel)) < 0) {
                cli_out("Error: failed to set page %d in ISL68127 device.\n", channel);
                break;
            }

            rv = isl68127_read(unit, saddr, PMBUS_CMD_READ_POUT, (void *)&dac, &datalen);
            if (rv < 0) {
                cli_out("Error: failed to read power in ISL68127 device.\n");
                break;
            }

            if (channel == 0) {
                ((ioctl_op_data_t *)data)->output.data[0] = dac*1200;
            } else {
                ((ioctl_op_data_t *)data)->output.data[0] = dac*1000;
            }

            break;

        case PMBUS_IOC_VOUT_SET:
            /* Conversion of output voltage */
            fval = ((ioctl_op_data_t *)data)->input.voltage;
            voltage = V_TO_L16(fval);
            dac = voltage;

            if ((voltage < DEV_DAC_PARAMS(dev, channel)->dac_min_hwval)||
                (voltage > DEV_DAC_PARAMS(dev, channel)->dac_max_hwval)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "i2c %d: ISL68127 given voltage %2.3f V "
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
                                    "i2c %s: ISL68127 ioctl "
                                    "I2C_DAC_IOC_SET_VOUT : voltage = %d, channel = %d\n"),
                         "ISL68127", voltage, channel));

            /* Write to ISL68127 needs to set channel first. */
            rv = isl68127_write(unit, saddr, PMBUS_CMD_PAGE, (void *) &channel, 1);
            if (rv < 0) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Failed to set ISL68127 channel.\n")));
                break;
            }

            rv = isl68127_write(unit, saddr, PMBUS_CMD_VOUT_COMMAND, (void *) &dac, 2);
            if (rv < 0) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Failed to set ISL68127 out voltage.\n")));
                break;
            }

            break;

        default:
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "i2c %s: isl68127_ioctl: invalid opcode (%d)\n"),
                         "ISL68127", opcode));
            break;
    }

    return rv;
}

/*!
 * \brief Device specific init function.
 */
static int
isl68127_init(int unit,
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

    if (dev == NULL) {
        return SHR_E_INTERNAL;
    }

    if (dev->priv_data == NULL) {
        dev->priv_data = sal_alloc(sizeof(device_data_t), "bcmaI2cISL68127");
        if (dev->priv_data == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to allocate private data fo dev %s\n"),
                                  "ISL68127"));
            return SHR_E_MEMORY;
        }
        sal_memset(dev->priv_data, 0, sizeof(device_data_t));

        /* Attach calibration record to dev */
        DEV_DAC_PARAMS(dev, channel) = record;
        DEV_DAC_PARAM_LEN(dev) = ISL68127_TOTAL_CHANNEL;
    } else {
        /* Attach calibration record to dev */
        DEV_DAC_PARAMS(dev, channel) = record;
    }

    /* bit3 1: control the ISL68127 output through VOUT command
     *      0: control the output via VID input pins which is controlled by
     *         a PCF8574 device
     */
    if ((rv = isl68127_write(unit, saddr,
                             PMBUS_CMD_CLEAR_FAULTS,(void *) &channel, 0)) < 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Failed to clear faults of ISL68127 device.\n")));
        return rv;
    }

    channel = 1;
    rv = isl68127_read(unit, saddr, 0xFA, (uint8_t *)&DEV_RAIL_SADDR(dev), (void *)&channel);
    if (rv < 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Failed to read address 0xFA of ISL68127 device.\n")));
        return rv;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "isl68127_init: %s, devNo=0x%x\n"),
                 "ISL68127", saddr));

    /* Setting Fault response to zero to prevent shut-down of device */
    data8 = 0x00;
    if ((rv = isl68127_write(unit, saddr,
                             PMBUS_CMD_VOUT_OV_FAULT_RES, &data8, 1)) < 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Failed to set OV fault response of ISL68127.\n")));
        return rv;
    }

    data8 = 0x0A;
    rv = isl68127_write(unit, saddr, PMBUS_CMD_ON_OFF_CONFIG, &data8, 1);
    if (rv < 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Failed to set ON/OFF CONFIG of ISL68127 device.\n")));
        return rv;
    }

    if ((rv = disable_load_line(unit, saddr)) < 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Failed to disable ISL68127 load line.\n")));
        return rv;
    }

    /* Switching on ISL68127 device */
    data8 = 0x80;
    rv = isl68127_write(unit, saddr, PMBUS_CMD_OPERATION, &data8, 1);
    if (rv < 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                             "Failed to set operation register of ISL68127 device.\n")));
        return rv;
    }

    return rv;
}

/*!
 * \brief Device specific unload function.
 */
static int
isl68127_cleanup(int unit,
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
 * \brief ISL68127 voltage control Chip Driver callout
 */
i2c_driver_t bcma_i2c_isl68127_driver = {
    0x0,
    0x0,
    ISL68127_DEVICE_TYPE,
    isl68127_read,
    isl68127_write,
    isl68127_ioctl,
    isl68127_init,
    isl68127_cleanup,
    NULL,
};

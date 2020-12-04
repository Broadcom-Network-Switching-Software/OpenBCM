/*! \file bcma_i2c_isl68239.c
 *
 *  Application to control power regulator ISL68239.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
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
 * \brief Device specific read function.
 */
static int
isl68239_read(int unit,
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
        rv = bcmbd_i2c_read_byte_data(unit, saddr, addr, data);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s read byte data: saddr=0x%x, addr=0x%x, "
                                "data=0x%x, len=%d, rv=%d.\n"),
                     ISL68239, saddr, addr, *data, *len, rv));
    } else if (*len == 2) {
        /* Reads a single word from a device, from a designated register */
        rv = bcmbd_i2c_read_word_data(unit, saddr, addr, (uint16_t *)data);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s read word data: saddr=0x%x, addr=0x%x, "
                                "data=0x%x, len=%d, rv=%d.\n"),
                     ISL68239, saddr, addr, *(uint16_t *)data, *len, rv));
    } else {
        /* Not supported for now */
         LOG_VERBOSE(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "%s read fail: saddr=0x%x, addr=0x%x, "
                                 "data=0x%x, len=%d\n"),
                      ISL68239, saddr, addr, *data, *len));
    }

    return rv;
}

/*!
 * \brief Device specific write function.
 */
static int
isl68239_write_op(int unit,
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
                                "%s write byte: saddr=0x%x, addr=0x%x, "
                                "len=%d.\n"),
                     ISL68239, saddr, addr, len));
        rv = bcmbd_i2c_write_byte(unit, saddr, addr);
    } else if (len == 1) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s write byte data: saddr=0x%x, addr=0x%x, "
                                "data=0x%x, len=%d.\n"),
                     ISL68239, saddr, addr, *data, len));
        rv = bcmbd_i2c_write_byte_data(unit, saddr, addr, *data);
    } else if (len == 2) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s write word data: saddr=0x%x, addr=0x%x, "
                                "data=0x%x, len=%d.\n"),
                     ISL68239, saddr, addr, *(uint16_t *)data, len));
        val = *(unsigned short *)data;
        rv = bcmbd_i2c_write_word_data(unit, saddr, addr, val);
    }
    return rv;
}

/*!
 * \brief Device specific write function wrapper.
 */
static int
isl68239_write(int unit,
               int saddr,
               uint16_t addr,
               uint8_t *data,
               uint32_t len)
{
    return (isl68239_write_op(unit, saddr, addr, data, len));
}

/*!
 * \brief Check per page (sub-channel) operation.
 */
static int
isl68239_check_page(int unit, int saddr, int ch)
{
     int rv;
     uint8_t page;
     uint32_t len;

     len = sizeof(char);
     rv = isl68239_read(unit, saddr, PMBUS_CMD_PAGE, &page, &len);
     if (rv != SHR_E_NONE) {
         return rv;
     }

     if (page != ch) {
         page = ch;
         LOG_VERBOSE(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "%s set page to %d\n"),
                      ISL68239, page));
         rv = isl68239_write(unit, saddr, PMBUS_CMD_PAGE, &page, sizeof(char));
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
isl68239_ioctl(int unit,
               int saddr,
               int opcode,
               void *data,
               int channel)
{
    double fval;
    uint16_t dac;
    uint32_t datalen = 2;
    unsigned short voltage;
    i2c_device_t *dev;

    SHR_FUNC_ENTER(unit);

    dev = ((ioctl_op_data_t *)data)->input.dev;

    switch (opcode) {
        case PMBUS_IOC_VOUT_READ:
            SHR_IF_ERR_EXIT
                (isl68239_check_page(unit, saddr, channel));
            datalen = 2;
            SHR_IF_ERR_EXIT
                (isl68239_read(unit, saddr, PMBUS_CMD_READ_VOUT, (void *)&dac,
                               &datalen));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s ioctl: read chan %d voltage %d.\n"),
                        ISL68239, channel, dac));

            fval = dac;
            fval = L16_TO_V(fval);
            ((ioctl_op_data_t *)data)->output.data[0] = fval;
            break;

        case PMBUS_IOC_IOUT_READ:
            SHR_IF_ERR_EXIT
                (isl68239_check_page(unit, saddr, channel));
            SHR_IF_ERR_EXIT
                (isl68239_read(unit, saddr, PMBUS_CMD_READ_IOUT, (void *)&dac,
                               &datalen));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s ioctl: read chan %d current %d.\n"),
                        ISL68239, channel, dac));

            /* (dac/10)*1000 */
            fval = (int16_t)dac * 100;
            ((ioctl_op_data_t *)data)->output.data[0] = fval;
            break;

        case PMBUS_IOC_POUT_READ:
            datalen = 2;
            SHR_IF_ERR_EXIT
                (isl68239_check_page(unit, saddr, channel));
            SHR_IF_ERR_EXIT
                (isl68239_read(unit, saddr, PMBUS_CMD_READ_POUT, (void *)&dac,
                               &datalen));

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s ioctl: read chan %d power %d.\n"),
                        ISL68239, channel, dac));

            fval = (int16_t)dac * 1000;
            ((ioctl_op_data_t *)data)->output.data[0] = fval;
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
                                      "%s ioctl: given voltage %2.3f V "
                                      "beyond range (max=%2.3f V, min=%2.3f V) "
                                      "for voltage VDD_%s.\n"),
                           ISL68239,
                           (double) L16_TO_V((double) voltage),
                           (double) L16_TO_V((double) DEV_DAC_PARAMS(dev, channel)->dac_max_hwval),
                           (double) L16_TO_V((double) DEV_DAC_PARAMS(dev, channel)->dac_min_hwval),
                           DEV_DAC_PARAMS(dev, channel)->name));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Show what we are doing, for now ... */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s ioctl: set chan %d voltage %d.\n"),
                         ISL68239, channel, voltage));

            /* Write to ISL68239 needs to set channel first. */
            SHR_IF_ERR_EXIT
                (isl68239_write(unit, saddr, PMBUS_CMD_PAGE,
                                (void *)&channel, 1));
            SHR_IF_ERR_EXIT
                (isl68239_write(unit, saddr, PMBUS_CMD_VOUT_COMMAND,
                                (void *)&dac, 2));
            break;

        default:
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s ioctl: invalid opcode (%d)\n"),
                        ISL68239, opcode));
            break;
    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Device specific init function.
 */
static int
isl68239_init(int unit,
              int saddr,
              void *data,
              int channel)
{
    i2c_device_t *dev;
    uint8_t data8;
    dac_calibrate_t *record;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);

    dev = ((ioctl_op_data_t *)data)->input.dev;
    SHR_NULL_CHECK(dev, SHR_E_INTERNAL);

    record = ((ioctl_op_data_t *)data)->input.cali_record;
    SHR_NULL_CHECK(record, SHR_E_INTERNAL);

    if (dev->priv_data == NULL) {
        dev->priv_data = sal_alloc(sizeof(device_data_t), "bcmaI2cISL68239");
        SHR_NULL_CHECK(dev->priv_data, SHR_E_MEMORY);

        sal_memset(dev->priv_data, 0, sizeof(device_data_t));

        /* Attach calibration record to dev */
        DEV_DAC_PARAMS(dev, channel) = record;
        DEV_DAC_PARAM_LEN(dev) = ISL68239_TOTAL_CHANNEL;
    } else {
        /* Attach calibration record to dev */
        DEV_DAC_PARAMS(dev, channel) = record;
    }

    SHR_IF_ERR_EXIT
        (isl68239_write(unit, saddr, PMBUS_CMD_CLEAR_FAULTS,
                        (void *)&channel, 0));

    channel = 1;
    SHR_IF_ERR_EXIT
        (isl68239_read(unit, saddr, 0xFA,
                       (uint8_t *)&DEV_RAIL_SADDR(dev), (void *)&channel));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s init: saddr=0x%x.\n"),
                 ISL68239, saddr));

    /* Setting Fault response to zero to prevent shut-down of device */
    data8 = 0x00;
    SHR_IF_ERR_EXIT
        (isl68239_write(unit, saddr, PMBUS_CMD_VOUT_OV_FAULT_RES, &data8, 1));

    /* bit3 1: control the ISL68239 output through VOUT command
     *      0: control the output via VID input pins which is controlled by
     *         a PCF8574 device
     */
    data8 = 0x0A;
    SHR_IF_ERR_EXIT
        (isl68239_write(unit, saddr, PMBUS_CMD_ON_OFF_CONFIG, &data8, 1));

    /* Switching on device, output source from VOUT */
    data8 = 0x80;
    SHR_IF_ERR_EXIT
        (isl68239_write(unit, saddr, PMBUS_CMD_OPERATION, &data8, 1));

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Device specific unload function.
 */
static int
isl68239_cleanup(int unit,
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
 * \brief ISL68239 voltage control Chip Driver callout
 */
i2c_driver_t bcma_i2c_isl68239_driver = {
    0x0,
    0x0,
    ISL68239_DEVICE_TYPE,
    isl68239_read,
    isl68239_write,
    isl68239_ioctl,
    isl68239_init,
    isl68239_cleanup,
    NULL,
};

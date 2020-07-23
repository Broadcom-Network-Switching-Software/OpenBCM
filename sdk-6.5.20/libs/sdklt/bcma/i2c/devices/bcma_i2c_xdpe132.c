/*! \file bcma_i2c_xdpe132.c
 *
 *  Application to control power regulator XDPE132.
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

static int8_t l16u_exponent[BCMDRD_CONFIG_MAX_UNITS];

/* Convert a floating point value into a 16-bit linear formatted word */
static void
float_to_l16u(int unit, double input_val, int8_t exponent, void *data)
{
    uint32_t uval;

    /* Sign extend exponent from 5 to 8 bits */
    if (exponent > 0x0F) {
        exponent |= 0xE0;
    }

    if (exponent < 0) {
        uval = ((uint32_t)(input_val * 100000) << (exponent * (-1))) / 100000;
    } else {
        uval = ((uint32_t)(input_val * 100000) >> exponent) / 100000;
    }

    /* Format the mantissa of the L16U */
    *(uint16_t *)data = uval & 0xFFFF;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s exponent: %d, %u\n"),
                 XDPE132, exponent, uval));

    return;
}

/* Convert a 16-bit linear formatted word into a floating point value */
static void
l16u_to_float(int unit, int8_t exponent, uint16_t mantissa, void *data)
{
    double fval;
    uint32_t uval = mantissa;

    /* Sign extend exponent from 5 to 8 bits */
    if (exponent > 0x0F) {
        exponent |= 0xE0;
    }

    /* Compute value as mantissa * 2^(exponent) */
    if (exponent < 0) {
        fval = (double)((uval * 100000) >> (exponent * (-1))) / 100000;
    } else {
        fval = (double)(uval << exponent);
    }

    *(double *)data = fval;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s exponent: %d, %f\n"),
                 XDPE132, exponent, *(double *)data));

    return;
}


/* Convert a 11-bit linear formatted word into a floating point value */
static void
l11_to_float(int unit, uint16_t input_val, void *data)
{
    int8_t exponent = input_val >> 11;
    int16_t mantissa = input_val & 0x7ff;

    /* Sign extend exponent from 5 to 8 bits */
    if (exponent > 0x0F) {
        exponent |= 0xE0;
    }

    /* Sign extend mantissa from 11 to 16 bits */
    if (mantissa > 0x03FF) {
        mantissa |= 0xF800;
    }

    /* Compute value as mantissa * 2^(exponent) */
    *(double *)data = exponent < 0 ?
        (double)((mantissa * 100000) >> (exponent * (-1))) / 100000 :
        (double)(mantissa << exponent);

    return;
}


/*!
 * \brief Device specific read function.
 */
static int
xdpe132_read(int unit,
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
                     XDPE132, saddr, addr, *data, *len, rv));
    } else if (*len == 2) {
        /* Reads a single word from a device, from a designated register */
        rv = bcmbd_i2c_read_word_data(unit, saddr, addr, (uint16_t *)data);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s read word data: saddr=0x%x, addr=0x%x, "
                                "data=0x%x, len=%d, rv=%d.\n"),
                     XDPE132, saddr, addr, *(uint16_t *)data, *len, rv));
    } else {
        /* Not supported for now */
         LOG_VERBOSE(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "%s read fail: saddr=0x%x, addr=0x%x, "
                                 "data=0x%x, len=%d\n"),
                      XDPE132, saddr, addr, *data, *len));
    }

    return rv;
}

/*!
 * \brief Device specific write function.
 */
static int
xdpe132_write(int unit,
              int saddr,
              uint16_t addr,
              uint8_t *data,
              uint32_t len)
{
    int rv = SHR_E_NONE;
    unsigned short val;

    if (len == 0) {
        /* Simply writes command code to device */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s write byte: saddr=0x%x, addr=0x%x, "
                                "len=%d.\n"),
                     XDPE132, saddr, addr, len));
        rv = bcmbd_i2c_write_byte(unit, saddr, addr);
    } else if (len == 1) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s write byte data: saddr=0x%x, addr=0x%x, "
                                "data=0x%x, len=%d.\n"),
                     XDPE132, saddr, addr, *data, len));
        rv = bcmbd_i2c_write_byte_data(unit, saddr, addr, *data);
    } else if (len == 2) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s write word data: saddr=0x%x, addr=0x%x, "
                                "data=0x%x, len=%d.\n"),
                     XDPE132, saddr, addr, *(uint16_t *)data, len));
        val = *(unsigned short *)data;
        rv = bcmbd_i2c_write_word_data(unit, saddr, addr, val);
    }
    return rv;
}

/*!
 * \brief Check per page (sub-channel) operation.
 */
static int
xdpe132_check_page(int unit, int saddr, int ch)
{
     int rv;
     uint8_t page;
     uint32_t len;

     len = sizeof(char);
     rv = xdpe132_read(unit, saddr, PMBUS_CMD_PAGE, &page, &len);
     if (rv != SHR_E_NONE) {
         return rv;
     }

     if (page != ch) {
         page = ch;
         LOG_VERBOSE(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "%s set page to %d\n"),
                      XDPE132, page));
         rv = xdpe132_write(unit, saddr, PMBUS_CMD_PAGE, &page, sizeof(char));
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
xdpe132_ioctl(int unit,
              int saddr,
              int opcode,
              void *data,
              int channel)
{
    double fval, max_v, min_v;
    uint16_t dac;
    uint32_t datalen = 2;
    unsigned short voltage;
    i2c_device_t *dev;

    SHR_FUNC_ENTER(unit);

    dev = ((ioctl_op_data_t *)data)->input.dev;

    switch (opcode) {
        case PMBUS_IOC_VOUT_READ:
            SHR_IF_ERR_EXIT
                (xdpe132_check_page(unit, saddr, channel));

            datalen = 2;
            SHR_IF_ERR_EXIT
                (xdpe132_read(unit, saddr, PMBUS_CMD_READ_VOUT, (void *)&dac,
                              &datalen));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s ioctl: read chan %d voltage %d.\n"),
                        XDPE132, channel, dac));

            l16u_to_float(unit, l16u_exponent[unit], dac, &fval);
            ((ioctl_op_data_t *)data)->output.data[0] = fval;
            break;

        case PMBUS_IOC_IOUT_READ:
            SHR_IF_ERR_EXIT
                (xdpe132_check_page(unit, saddr, channel));
            SHR_IF_ERR_EXIT
                (xdpe132_read(unit, saddr, PMBUS_CMD_READ_IOUT, (void *)&dac,
                               &datalen));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s ioctl: read chan %d current %d.\n"),
                        XDPE132, channel, dac));

            l11_to_float(unit, dac, &fval);
            ((ioctl_op_data_t *)data)->output.data[0] = fval * 1000;
            break;

        case PMBUS_IOC_POUT_READ:
            datalen = 2;
            SHR_IF_ERR_EXIT
                (xdpe132_check_page(unit, saddr, channel));
            SHR_IF_ERR_EXIT
                (xdpe132_read(unit, saddr, PMBUS_CMD_READ_POUT, (void *)&dac,
                               &datalen));

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s ioctl: read chan %d power %d.\n"),
                        XDPE132, channel, dac));

            l11_to_float(unit, dac, &fval);
            ((ioctl_op_data_t *)data)->output.data[0] = fval * 1000;
            break;

        case PMBUS_IOC_VOUT_SET:
            fval = ((ioctl_op_data_t *)data)->input.voltage;
            float_to_l16u(unit, fval, l16u_exponent[unit], &voltage);

            if ((voltage < DEV_DAC_PARAMS(dev, channel)->dac_min_hwval)||
                (voltage > DEV_DAC_PARAMS(dev, channel)->dac_max_hwval)) {
                l16u_to_float(unit, l16u_exponent[unit],
                              DEV_DAC_PARAMS(dev, channel)->dac_max_hwval,
                              &max_v);
                l16u_to_float(unit, l16u_exponent[unit],
                              DEV_DAC_PARAMS(dev, channel)->dac_min_hwval,
                              &min_v);

                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "%s ioctl: given voltage %2.3f "
                                      "beyond range (max=%2.3f, min=%2.3f) "
                                      "for voltage VDD_%s.\n"),
                           XDPE132,
                           fval, max_v, min_v,
                           DEV_DAC_PARAMS(dev, channel)->name));

                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Show what we are doing, for now ... */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s ioctl: set chan %d voltage %2.3f "
                                    "hwval %d.\n"),
                         XDPE132, channel, fval, voltage));

            /* Write to XDPE132 needs to set channel first. */
            SHR_IF_ERR_EXIT
                (xdpe132_write(unit, saddr, PMBUS_CMD_PAGE,
                               (void *)&channel, 1));
            SHR_IF_ERR_EXIT
                (xdpe132_write(unit, saddr, PMBUS_CMD_VOUT_COMMAND,
                               (void *)&voltage, 2));
            break;

        default:
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s ioctl: invalid opcode (%d)\n"),
                        XDPE132, opcode));
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Device specific init function.
 */
static int
xdpe132_init(int unit,
             int saddr,
             void *data,
             int channel)
{
    i2c_device_t *dev;
    dac_calibrate_t *record;
    uint32_t len;
    uint8_t data8;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);

    dev = ((ioctl_op_data_t *)data)->input.dev;
    SHR_NULL_CHECK(dev, SHR_E_INTERNAL);

    record = ((ioctl_op_data_t *)data)->input.cali_record;
    SHR_NULL_CHECK(record, SHR_E_INTERNAL);

    if (dev->priv_data == NULL) {
        dev->priv_data = sal_alloc(sizeof(device_data_t), "bcmaI2cXDPE132");
        SHR_NULL_CHECK(dev->priv_data, SHR_E_MEMORY);

        sal_memset(dev->priv_data, 0, sizeof(device_data_t));

        /* Attach calibration record to dev */
        DEV_DAC_PARAMS(dev, channel) = record;
        DEV_DAC_PARAM_LEN(dev) = XDPE132_TOTAL_CHANNEL;
    } else {
        /* Attach calibration record to dev */
        DEV_DAC_PARAMS(dev, channel) = record;
    }

    /* Clear previous fault */
    SHR_IF_ERR_EXIT
        (xdpe132_write(unit, saddr, PMBUS_CMD_CLEAR_FAULTS,
                        (void *)&channel, 0));

    /* Check vout_mode is linear */
    len = 1;
    SHR_IF_ERR_EXIT
        (xdpe132_read(unit, saddr, PMBUS_CMD_VOUT_MODE, &data8, &len));
    if ((data8 >> 5) != 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s init: vout_mode 0x%x, not linear.\n"),
                  XDPE132, data8));
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    l16u_exponent[unit] = data8 & 0x1F;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s init: saddr=0x%x mode=0x%x\n"),
                 XDPE132, saddr, data8));


    /* Setting fault response to zero to prevent shut-down of device */
    data8 = 0x00;
    SHR_IF_ERR_EXIT
        (xdpe132_write(unit, saddr, PMBUS_CMD_VOUT_OV_FAULT_RES, &data8, 1));

    /* Depends on OPERATION command to start */
    data8 = 0x0A;
    SHR_IF_ERR_EXIT
        (xdpe132_write(unit, saddr, PMBUS_CMD_ON_OFF_CONFIG, &data8, 1));

    /* Vout control from PMBus VOUT_COMMAND */
    data8 = 0x80;
    SHR_IF_ERR_EXIT
        (xdpe132_write(unit, saddr, PMBUS_CMD_OPERATION, &data8, 1));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Device specific unload function.
 */
static int
xdpe132_cleanup(int unit,
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
 * \brief XDPE132 voltage control Chip Driver callout
 */
i2c_driver_t bcma_i2c_xdpe132_driver = {
    0x0,
    0x0,
    XDPE132_DEVICE_TYPE,
    xdpe132_read,
    xdpe132_write,
    xdpe132_ioctl,
    xdpe132_init,
    xdpe132_cleanup,
    NULL,
};

/*! \file bcma_i2ccmd_i2c.c
 *
 *  CLI command for BB control user interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/i2c/bcma_i2ccmd_op.h>
#include <bcma/i2c/bcma_i2ccmd_bb.h>
#include <bcma/i2c/bcma_i2c_board_type.h>
#include <bcmbd/bcmbd_i2c.h>
#include <bcma/io/bcma_io_file.h>
#include <bcmbd/bcmbd.h>
#include <shr/shr_pb.h>

#define BSL_LOG_MODULE  BSL_LS_APPL_I2C

#define BCMA_BB_PROFILE(i)  &bcma_bb_voltage_profile[i][0]

static bb_i2c_device_t bcma_bb_voltage_profile[BCMDRD_CONFIG_MAX_UNITS];
static i2c_device_t bcma_temperature_dev[BCMDRD_CONFIG_MAX_UNITS][20];
static temperature_dev_info_t bcma_thermal_record[BCMDRD_CONFIG_MAX_UNITS][20];

/*!
 * \brief Show base board type based on board ID.
 *
 * \param[in] unit     unit
 * \param[in] board_id Board ID readout from I2C bus.
 *
 * \return No return needed.
 */
static void
bcma_show_base_board(int unit, uint32_t board_id)
{
    bool found = FALSE;
    bcma_i2c_base_board_t *info = NULL;

    if (!bcma_i2c_device_drv[unit]) {
        bcma_i2c_board_drv_init(unit);
    }

    if (bcma_i2c_device_drv[unit] &&
        bcma_i2c_device_drv[unit]->platform_get) {
        info = bcma_i2c_device_drv[unit]->platform_get(unit);
    }

    if (info) {
        do {
            if (info->id == board_id) {
                found = TRUE;
                break;
            }
            info++;
        } while ((info->id != 0) && (info->desc != NULL));
    }

    if (found) {
        cli_out("BaseBoard: %s(type 0x%x)\n", info->desc, board_id);
    } else {
        cli_out("BaseBoard (type 0x%x) not supported!\n", board_id);
    }

    return;
}


/*!
 * \brief Convert CLI input to double data type.
 *
 * \param[in] unit Unit number.
 *
 * \return converted value.
 */
static double
bcma_atof(const char *str)
{
    int len = 0, n = 0, i = 0;
    float f = 1.0;
    float val = 0.0;

    /* counting length of String */
    while (str[len]) {
        len++;
    }

    /* cheking for valid string */
    if (!len) {
        return 0;
    }

    /* Extracting integer part */
    while (i < len && (str[i] != '.')) {
        n = (10 * n) + (str[i++] - '0');
    }

    /* checking if only integer */
    if (i == len) {
        return n;
    }

    /* Now do decimal part */
    i++;
    while (i < len)
    {
        f *= 0.1;
        val += f * (str[i++] - '0');
    }

    return (val + n);
}

/*!
 * \brief CLI 'temperature' command for temperature show.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
static int
bcma_show_bb_temperature(int unit, int argc)
{
    int total = 0;
    int ix;
    i2c_addr_t addr;

    if (argc != 1) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (!bcma_i2c_device_drv[unit]) {
        bcma_i2c_board_drv_init(unit);
    }

    if (bcma_i2c_device_drv[unit] &&
        bcma_i2c_device_drv[unit]->temp_dev_get) {
        /* Query driver total device count */
        bcma_i2c_device_drv[unit]->temp_dev_get(unit,
                                                &total,
                                                &bcma_temperature_dev[unit][0]);

        /* Allocate required mem to hold dev info */
        if ((total != 0) && (total < BCMA_I2C_MAXIMUM_DEVICE)) {
            /* Call driver again to get dev info */
            bcma_i2c_device_drv[unit]->temp_dev_get(unit,
                                                    &total,
                                                    &bcma_temperature_dev[unit][0]);
            for (ix = 0; ix < total; ix++) {
                /* Call driver show command */
                addr = bcma_i2c_addr(unit, ix, bcma_temperature_dev[unit]);
                bcma_temperature_dev[unit][ix].driver->show(unit,
                                                       addr,
                                                       (void *)&bcma_thermal_record[unit][ix],
                                                       1);
                cli_out("Board Temperature Sensor %d (0x%x): \n",
                        ix, (int)bcma_temperature_dev[unit][ix].saddr);
                cli_out("Local temperature: %d C, %d F. \n",
                         bcma_thermal_record[unit][ix].local_temp,
                         C2F(bcma_thermal_record[unit][ix].local_temp));
                cli_out("Remote temperature 1: %d C, %d F. \n",
                         bcma_thermal_record[unit][ix].remote_temp_1,
                         C2F(bcma_thermal_record[unit][ix].remote_temp_1));
                cli_out("Remote temperature 2: %d C, %d F. \n\n",
                         bcma_thermal_record[unit][ix].remote_temp_2,
                         C2F(bcma_thermal_record[unit][ix].remote_temp_2));
            }
        }
    } else {
        return SHR_E_UNAVAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/*!
 * \brief CLI 'bb' command for Base board controlling.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
static i2c_device_t *
bcma_bb_locate_drv(int unit, i2c_device_t *dev, int total, char *name, char addr)
{
    int ix;
    i2c_device_t *d_ptr;

    d_ptr = dev;
    for (ix = 0; ix < total; ix++) {
        if ((sal_strcmp(d_ptr->devname, name) == 0) &&
            (d_ptr->saddr == addr)) {
            return d_ptr;
        }
        d_ptr++;
    }

    return NULL;
}

/*!
 * \brief CLI 'bb' command for Base board controlling.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
static dac_calibrate_t *
bcma_bb_locate_cali_record(int unit,
                           bb_i2c_device_t *dev,
                           char addr,
                           const char *rail_name)
{
    int ix;
    dac_calibrate_t *record_ptr;

    record_ptr = dev->cali_record;

    for (ix = 0; ix < dev->cali_cnt; ix++) {
        if ((addr == record_ptr->saddr) &&
            (sal_strcmp(record_ptr->name, rail_name) == 0)){
            return record_ptr;
        }
        record_ptr++;
    }

    return NULL;
}

/*!
 * \brief CLI 'bb' command for Base board controlling.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
static int
bcma_bb_board_id(int unit)
{
    i2c_device_t *id_dev;
    uint32_t board_id = 0;

    if (bcma_bb_voltage_profile[unit].initialized == FALSE) {
        if (!bcma_i2c_device_drv[unit]) {
            bcma_i2c_board_drv_init(unit);
        }

        if (bcma_i2c_device_drv[unit] &&
            bcma_i2c_device_drv[unit]->voltage_dev_get) {

            /* Derive board specific configuration */
            bcma_i2c_device_drv[unit]->voltage_dev_get(unit, &bcma_bb_voltage_profile[unit]);
        } else {
            return SHR_E_UNAVAIL;
        }
    }

    id_dev = bcma_bb_voltage_profile[unit].id_device;

    if (id_dev) {
        /* Run driver init function first */
        id_dev->driver->load(unit,
                             id_dev->saddr,
                             (void *)&board_id,
                             0);
    }

    return board_id;
}

/*!
 * \brief CLI 'bb' command for Base board controlling.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
int
bcma_bb_set_rail_voltage(int unit,
                         bb_i2c_device_t *dev_info,
                         const char *rail_name,
                         double *voltage)
{
    int ix;
    bb_voltage_profile_t *p_ptr;
    i2c_device_t *d_ptr;
    dac_calibrate_t *record;
    ioctl_op_data_t op_data;
    ioctl_in_t *op_in;
    i2c_driver_t *drv;

    sal_memset((char *)&op_data, 0, sizeof(op_data));
    p_ptr = dev_info->voltage_profile;

    /* Look for profile based on rail name */
    for (ix = 0; ix < dev_info->p_cnt; ix++) {
        if (sal_strcmp(p_ptr->function_name, rail_name) == 0) {
            break;
        }
        p_ptr++;
    }

    if (ix >= dev_info->p_cnt) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Locate calibration record for this rail voltage */
    record = bcma_bb_locate_cali_record(unit, dev_info, p_ptr->adc_addr, rail_name);
    if (record == NULL) {
        return SHR_E_INTERNAL;
    }

    /* If the voltage regulator is behind a MUX, enable it */
    if (p_ptr->behind_mux) {
        /* Locate the correct mux - name and address */
        d_ptr = bcma_bb_locate_drv(unit,
                                   dev_info->mux_device,
                                   dev_info->m_cnt,
                                   p_ptr->mux_name,
                                   p_ptr->mux_addr);

        if (d_ptr) {
            /* Run driver init function first */
            drv = d_ptr->driver;

            drv->load(unit, p_ptr->mux_addr, 0, 0);

            drv->write(unit, p_ptr->mux_addr,
                       0, &p_ptr->mux_channel, 1);
        }
    }

    /*
     * Now read out from the voltage regulator itself
     * We support two types of power regulator. flags as:
     * PWR_DAC_IO_SRC & PWR_DAC_IO_PMBUS respectively.
     */
    if (p_ptr->flags == PWR_DAC_IO_SRC) {
        /* Locate the correct adc device - name and address */
        d_ptr = bcma_bb_locate_drv(unit,
                                   dev_info->v_device,
                                   dev_info->v_cnt,
                                   p_ptr->adc_name,
                                   p_ptr->adc_addr);

        if (d_ptr) {
            /* Pass device and board specific info to driver */
            op_in = &op_data.input;
            drv = d_ptr->driver;

            op_in->board_id = bcma_bb_board_id(unit);
            op_in->dev = d_ptr;
            op_in->sensor_config = dev_info->sensor_config;
            op_in->cali_record = record;
            op_in->voltage = *voltage;

            /* Run driver init function first */
            drv->load(unit, p_ptr->adc_addr,
                      &op_data, p_ptr->adc_channel);

            drv->ioctl(unit, p_ptr->adc_addr, I2C_LTC_IOC_VOUT_SET,
                       &op_data, p_ptr->adc_channel);

            /* Clean up driver resource. */
            drv->unload(unit, p_ptr->adc_addr,
                        &op_data, p_ptr->adc_channel);

        }
    } else if (p_ptr->flags == PWR_DAC_IO_PMBUS) {
        /* Locate the correct adc device - name and address */
        d_ptr = bcma_bb_locate_drv(unit,
                                   dev_info->v_device,
                                   dev_info->v_cnt,
                                   p_ptr->adc_name,
                                   p_ptr->adc_addr);

        if (d_ptr) {
            /* Pass device and board specific info to driver */
            op_in = &op_data.input;
            drv = d_ptr->driver;

            op_in->board_id = bcma_bb_board_id(unit);
            op_in->dev = d_ptr;
            op_in->sensor_config = dev_info->sensor_config;
            op_in->cali_record = record;
            op_in->voltage = *voltage;

            /* Run driver init function first */
            drv->load(unit, p_ptr->adc_addr, &op_data, p_ptr->adc_channel);

            /* Set voltage output */
            drv->ioctl(unit, p_ptr->adc_addr, PMBUS_IOC_VOUT_SET,
                       &op_data, p_ptr->adc_channel);

            /* clean up driver resource */
            drv->unload(unit, p_ptr->adc_addr, &op_data, p_ptr->adc_channel);
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief CLI 'bb' command for Base board controlling.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
static int
bcma_bb_rail_voltage(int unit,
                     bb_i2c_device_t *dev_info,
                     const char *rail_name,
                     double *total_power)
{
    int ix;
    bb_voltage_profile_t *p_ptr;
    i2c_device_t *d_ptr;
    ioctl_op_data_t op_data;
    double adc_voltage = 0;
    double adc_current = 0;
    double adc_power = 0;
    ioctl_in_t *op_in;
    i2c_driver_t *drv;

    sal_memset((char *)&op_data, 0, sizeof(ioctl_op_data_t));
    p_ptr = dev_info->voltage_profile;

    for (ix = 0; ix < dev_info->p_cnt; ix++) {
        if (sal_strcmp(p_ptr->function_name, rail_name) == 0) {
            break;
        }
        p_ptr++;
    }

    if (ix >= dev_info->p_cnt) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* If the voltage regulator is hind a MUX, config it */
    if (p_ptr->behind_mux) {
        /* Locate the correct mux - name and address */
        d_ptr = bcma_bb_locate_drv(unit,
                                   dev_info->mux_device,
                                   dev_info->m_cnt,
                                   p_ptr->mux_name,
                                   p_ptr->mux_addr);

        if (d_ptr) {
            /* Run driver init function first */
            drv = d_ptr->driver;

            drv->load(unit, p_ptr->mux_addr, 0, 0);

            drv->write(unit, p_ptr->mux_addr, 0,
                       &p_ptr->mux_channel, 1);
        }

    }

    /*
     * Now read out from the voltage regulator itself
     * We support two types of power regulator. flags as:
     * PWR_DAC_IO_SRC & PWR_DAC_IO_PMBUS respectively.
     */
    if (p_ptr->flags == PWR_DAC_IO_SRC) {
        /* Locate the correct adc device - name and address */
        d_ptr = bcma_bb_locate_drv(unit,
                                   dev_info->v_device,
                                   dev_info->v_cnt,
                                   p_ptr->adc_name,
                                   p_ptr->adc_addr);

        if (d_ptr) {
            /* Pass device and board specific info to driver */
            op_in = &op_data.input;
            drv = d_ptr->driver;

            op_in->board_id = bcma_bb_board_id(unit);
            op_in->dev = d_ptr;
            op_in->sensor_config = dev_info->sensor_config;

            /* Locate calibration record for this rail voltage */
            op_in->cali_record = bcma_bb_locate_cali_record(unit,
                                                            dev_info,
                                                            p_ptr->adc_addr,
                                                            rail_name);

            /* Run driver init function first */
            drv->load(unit, p_ptr->adc_addr, &op_data, p_ptr->adc_channel);

            /* First read out voltage output */
            drv->ioctl(unit, p_ptr->adc_addr, I2C_LTC_IOC_VOUT_READ,
                       &op_data, p_ptr->adc_channel);
            adc_voltage = op_data.output.data[0];

            /* Second check resistor configuration */
            drv->ioctl(unit, p_ptr->adc_addr, I2C_LTC_IOC_RCONFIG_SET,
                       &op_data, p_ptr->adc_channel);

            /* Third read out current output */
            drv->ioctl(unit, p_ptr->adc_addr, I2C_LTC_IOC_IOUT_READ,
                       &op_data, p_ptr->adc_channel);
            adc_current = op_data.output.data[0];

            /* Fourth read out power output */
            drv->ioctl(unit, p_ptr->adc_addr, I2C_LTC_IOC_POUT_READ,
                       &op_data, p_ptr->adc_channel);
            adc_power = op_data.output.data[0];

            /* Cleanup driver resource. */
            drv->unload(unit, p_ptr->adc_addr, &op_data, p_ptr->adc_channel);

            /* Return power to caller. */
            *total_power = adc_power;

            cli_out("VDD_%s = %2.3f Volts "
                    " Current = %2.3f mA  Power = %2.3f mW \n",
                    p_ptr->function_name, adc_voltage,
                    adc_current, adc_power);
        }
    } else if (p_ptr->flags == PWR_DAC_IO_PMBUS) {
        /* Locate the correct adc device - name and address */
        d_ptr = bcma_bb_locate_drv(unit,
                                   dev_info->v_device,
                                   dev_info->v_cnt,
                                   p_ptr->adc_name,
                                   p_ptr->adc_addr);

        if (d_ptr) {
            /* Pass device and board specific info to driver */
            op_in = &op_data.input;
            drv = d_ptr->driver;

            op_in->board_id = bcma_bb_board_id(unit);
            op_in->dev = d_ptr;
            op_in->sensor_config = dev_info->sensor_config;

            /* Locate calibration record for this rail voltage */
            op_in->cali_record = bcma_bb_locate_cali_record(unit,
                                                            dev_info,
                                                            p_ptr->adc_addr,
                                                            rail_name);

            /* Run driver init function first */
            drv->load(unit, p_ptr->adc_addr, &op_data, p_ptr->adc_channel);

            /* First read out voltage output */
            drv->ioctl(unit, p_ptr->adc_addr, PMBUS_IOC_VOUT_READ,
                       &op_data, p_ptr->adc_channel);
            adc_voltage = op_data.output.data[0];

            /* Second read out current output */
            drv->ioctl(unit, p_ptr->adc_addr, PMBUS_IOC_IOUT_READ,
                       &op_data, p_ptr->adc_channel);
            adc_current = op_data.output.data[0];

            /* Third read out power output */
            drv->ioctl(unit, p_ptr->adc_addr, PMBUS_IOC_POUT_READ,
                       &op_data, p_ptr->adc_channel);

            /* clean up driver resource. */
            drv->unload(unit, p_ptr->adc_addr, &op_data, p_ptr->adc_channel);

            adc_power = op_data.output.data[0];
        }

        /* Return power to caller. */
        *total_power = adc_power;

        cli_out("VDD_%s = %2.3f Volts "
                " Current = %2.3f mA  Power = %2.3f mW \n",
                p_ptr->function_name, adc_voltage,
                adc_current, adc_power);
    }

    return SHR_E_NONE;
}

/*!
 * \brief CLI 'bb' command for Base board controlling.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
static int
bcma_bb_voltage_show(int unit, bb_i2c_device_t *dev_info)
{
    int ix;
    bb_voltage_profile_t *p_ptr;
    double power;
    double total_power = 0;

    p_ptr = dev_info->voltage_profile;

    for (ix = 0; ix < dev_info->p_cnt; ix++) {
        bcma_bb_rail_voltage(unit, dev_info, p_ptr->function_name, &power);
        total_power = total_power + power;
        p_ptr++;
    }

    cli_out("Total Device Power %2.3f Watt. \n", (total_power / 1000));

    return SHR_E_NONE;
}

/*!
 * \brief CLI 'bb' command for Base board controlling.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
int
bcma_i2ccmd_bb(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;
    const char *arg;
    const char *arg_buf[BB_MAX_ARG_COUNT];
    uint32_t argc;
    uint32_t ix;
    int board_id;
    double voltage;
    double power;
    int rv;

    /* Variables initialization */
    unit = cli->cmd_unit;

    sal_memset((char *)arg_buf, 0, sizeof(arg_buf));
    argc = 0;

    /* Sort CLI arguments into buffer. */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg) {
        while (arg) {
           arg_buf[argc]=arg;

           arg = BCMA_CLI_ARG_GET(args);
           argc++;
           if (argc >= BB_MAX_ARG_COUNT) {
               return BCMA_CLI_CMD_USAGE;
           }
        }
    }

    if (argc == 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Arg count: %d\n"),
                 argc));

    for (ix = 0; ix < argc; ix++) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "arg[%d]: %s\n"),
                     ix, arg_buf[ix]));
    }

    if (!bcma_i2c_device_drv[unit]) {
        bcma_i2c_board_drv_init(unit);
    }

    if (bcma_i2c_device_drv[unit] &&
        bcma_i2c_device_drv[unit]->voltage_dev_get) {
        /* Derive board specific configuration */
        bcma_i2c_device_drv[unit]->voltage_dev_get(unit,
                                                   &bcma_bb_voltage_profile[unit]);
    } else {
        return SHR_E_UNAVAIL;
    }

    if (sal_strncmp(arg_buf[0], "voltage", 4) == 0) {
        if (argc == 1) {
            /* Show voltage configuration for all channels */
            /* Keep here for debug usage */
            /* cli_out("Total voltage profile: %d\n", bcma_bb_voltage_profile[unit].p_cnt); */
            /* cli_out("Total Mux device: %d\n", bcma_bb_voltage_profile[unit].m_cnt); */
            /* cli_out("Total ID device: %d\n", bcma_bb_voltage_profile[unit].id_cnt); */
            /* cli_out("Total Voltage device: %d\n", bcma_bb_voltage_profile[unit].v_cnt); */

            return (bcma_bb_voltage_show(unit, &bcma_bb_voltage_profile[unit]));

        } else if (argc == 2) {

            /* Show specific voltage channel */
            return (bcma_bb_rail_voltage(unit, &bcma_bb_voltage_profile[unit], arg_buf[1], &power));

        } else if (argc == 3) {
            /* Set specific rail voltage value per user input */
            voltage = bcma_atof(arg_buf[2]);
            rv = bcma_bb_set_rail_voltage(unit,
                                          &bcma_bb_voltage_profile[unit],
                                          arg_buf[1],
                                          &voltage);
            if (SHR_FAILURE(rv)) {
                return rv;
            }
            return (bcma_bb_rail_voltage(unit, &bcma_bb_voltage_profile[unit], arg_buf[1], &power));
        } else {
            return BCMA_CLI_CMD_USAGE;
        }
    } else if (sal_strncmp(arg_buf[0], "board", 2) == 0) {
        board_id = bcma_bb_board_id(unit);
        bcma_show_base_board(unit, board_id);
    } else if (sal_strncmp(arg_buf[0], "temperature", 2) == 0) {
        bcma_show_bb_temperature(unit, argc);
    } else {
        return BCMA_CLI_CMD_USAGE;
    }

    return BCMA_CLI_CMD_OK;
}

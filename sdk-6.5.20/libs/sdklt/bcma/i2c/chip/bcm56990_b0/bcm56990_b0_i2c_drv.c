/*! \file bcm56990_b0_i2c_drv.c
 *
 * Chip stub for BCMA i2c device driver attach.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>

#include <bcma/i2c/bcma_i2ccmd_op.h>

/*! BCM56990_B0 base board ID. */
#define TOMAHAWK_4_B0   0xD5

/* Debug log target definition */
/*******************************************************************************
* Private functions
 */

/* BCM56990 supported base board type */
static bcma_i2c_base_board_t bcm56990_b0_base_board_type[] = {
    {"TOMAHAWK 4 B0 Platform",  TOMAHAWK_4_B0},
    {"\0", 0}
};

/* Board specific sense resistor configuration */
static i2c_ltc_t bcm56990_b0_sense_resistor_config[LTC2974_TOTAL_CHANNEL] = {
    { 0, "MDIO",   LTC2974, CHANNEL0, 0xF801/*0.5*/,  0 },
    { 1, "1.2",    LTC2974, CHANNEL1, 0x4 /*4*/  ,    0 },
    { 2, "1.8V1",  LTC2974, CHANNEL2, 0xC8/*200*/,    0 },
    { 3, "1.8V2",  LTC2974, CHANNEL3, 0xC8/*200*/,    0 }
};

/* Board specific voltage rail profile */
static bb_voltage_profile_t bcm56990_b0_bb_voltage_profile[] = {
    {0, "MDIO",     1, MUX9548, 0x73, 3, LTC2974,  0x5c, CHANNEL0, LTC2974,  PWR_DAC_IO_SRC},
    {1, "1.2",      1, MUX9548, 0x73, 3, LTC2974,  0x5c, CHANNEL1, LTC2974,  PWR_DAC_IO_SRC},
    {2, "1.8V1",    1, MUX9548, 0x73, 3, LTC2974,  0x5c, CHANNEL2, LTC2974,  PWR_DAC_IO_SRC},
    {3, "1.8V2",    1, MUX9548, 0x73, 3, LTC2974,  0x5c, CHANNEL3, LTC2974,  PWR_DAC_IO_SRC},
    {4, "Analog0",  1, MUX9548, 0x73, 3, ISL68239, 0x60, CHANNEL0, ISL68239, PWR_DAC_IO_PMBUS},
    {5, "Analog1",  1, MUX9548, 0x73, 3, ISL68239, 0x60, CHANNEL1, ISL68239, PWR_DAC_IO_PMBUS},
    {6, "3.3V1",    1, MUX9548, 0x73, 3, ISL68127, 0x61, CHANNEL0, ISL68127, PWR_DAC_IO_PMBUS},
    {7, "3.3V2",    1, MUX9548, 0x73, 3, ISL68127, 0x64, CHANNEL0, ISL68127, PWR_DAC_IO_PMBUS},
    {8, "Core",     1, MUX9548, 0x73, 3, XDPE132,  0x40, CHANNEL0, XDPE132,  PWR_DAC_IO_PMBUS}
};

/* Per voltage rail calibration data */
static dac_calibrate_t bcm56990_b0_adc_cali_data[] = {
    {0x5c, "MDIO",    -1, 1, 0, 0, LTC2974_1_35V,  LTC2974_0_6V,   LTC2974_1_2V,  1},
    {0x5c, "1.2",     -1, 1, 0, 0, LTC2974_1_35V,  LTC2974_0_6V,   LTC2974_1_2V,  1},
    {0x5c, "1.8V1",   -1, 1, 0, 0, LTC2974_1_95V,  LTC2974_0_6V,   LTC2974_1_8V,  1},
    {0x5c, "1.8V2",   -1, 1, 0, 0, LTC2974_1_95V,  LTC2974_0_6V,   LTC2974_1_8V,  1},
    {0x60, "Analog0", -1, 1, 0, 0, ISL68239_0_9V,  ISL68239_0_6V,  ISL68239_0_8V, 1},
    {0x60, "Analog1", -1, 1, 0, 0, ISL68239_0_9V,  ISL68239_0_6V,  ISL68239_0_8V, 1},
    {0x61, "3.3V1",   -1, 1, 0, 0, ISL68127_3_3V,  ISL68127_0_6V,  ISL68127_1_8V, 1},
    {0x64, "3.3V2",   -1, 1, 0, 0, ISL68127_3_3V,  ISL68127_0_6V,  ISL68127_1_8V, 1},
    {0x40, "Core",    -1, 1, 0, 0, XDPE132_1V,     XDPE132_0_6V,   XDPE132_0_88V, 1}
};

/* Board ID chip info */
static i2c_device_t bcm56990_b0_board_id[] = {
    I2CDEV(PCF8574, 0x27, pcf8574)
};

/* Mux info before voltage regulator */
static i2c_device_t bcm56990_b0_voltage_mux[] = {
    I2CDEV(MUX9548, 0x73, pca9548)
};

/* All voltage regulator devices */
static i2c_device_t bcm56990_b0_voltage_devices[] = {
    I2CDEV(LTC2974,  0x5C, ltc2974),
    I2CDEV(ISL68239, 0x60, isl68239),
    I2CDEV(ISL68127, 0x61, isl68127),
    I2CDEV(ISL68127, 0x64, isl68127),
    I2CDEV(XDPE132,  0x40, xdpe132),
};

static i2c_device_t bcm56990_b0_temperature_devices[] = {
    I2CDEV(MAX669X, 0x18, max669x),
    I2CDEV(MAX669X, 0x19, max669x)
};

/* Complete list of supported I2C devices. */
static i2c_device_t bcm56990_b0_i2c_devices[] = {
    I2CDEV(LPT,      0x27, pcf8574),
    I2CDEV(MUX,      0x73, pca9548),
    I2CDEV(MAX669X,  0x18, max669x),
    I2CDEV(MAX669X,  0x19, max669x),
    I2CDEV(LTC2974,  0x5C, ltc2974),
    I2CDEV(ISL68239, 0x60, isl68239),
    I2CDEV(ISL68127, 0x61, isl68127),
    I2CDEV(ISL68127, 0x64, isl68127),
    I2CDEV(XDPE132,  0x40, xdpe132),
};

static bcma_i2c_base_board_t *
bcm56990_b0_i2c_platform_info_get(int unit)
{
    return (bcm56990_b0_base_board_type);
}

static int
bcm56990_b0_bb_voltage_dev_get(int unit, bb_i2c_device_t *dev_info)
{
    if (dev_info) {
        dev_info->p_cnt = COUNTOF(bcm56990_b0_bb_voltage_profile);
        dev_info->voltage_profile = bcm56990_b0_bb_voltage_profile;

        dev_info->m_cnt = COUNTOF(bcm56990_b0_voltage_mux);
        dev_info->mux_device = bcm56990_b0_voltage_mux;

        dev_info->id_cnt = COUNTOF(bcm56990_b0_board_id);
        dev_info->id_device = bcm56990_b0_board_id;

        dev_info->v_cnt = COUNTOF(bcm56990_b0_voltage_devices);
        dev_info->v_device = bcm56990_b0_voltage_devices;

        dev_info->sensor_cnt = COUNTOF(bcm56990_b0_sense_resistor_config);
        dev_info->sensor_config = bcm56990_b0_sense_resistor_config;

        dev_info->cali_cnt = COUNTOF(bcm56990_b0_adc_cali_data);
        dev_info->cali_record = bcm56990_b0_adc_cali_data;

        dev_info->initialized = TRUE;
    }

    return SHR_E_NONE;
}

static int
bcm56990_b0_temperature_dev_get(int unit, int *total, i2c_device_t *dev_info)
{
    if (*total == 0) {
        *total = sizeof(bcm56990_b0_temperature_devices) / sizeof(i2c_device_t);
    }

    if (dev_info) {
        *total = sizeof(bcm56990_b0_temperature_devices) / sizeof(i2c_device_t);
        sal_memcpy(dev_info,
                   &bcm56990_b0_temperature_devices,
                   sizeof(bcm56990_b0_temperature_devices));
    }
    return SHR_E_NONE;
}

static int
bcm56990_b0_i2c_dev_get(int unit, int *total, i2c_device_t *dev_info)
{
    if (*total == 0) {
        *total = sizeof(bcm56990_b0_i2c_devices) / sizeof(i2c_device_t);
    }

    if (dev_info) {
        *total = sizeof(bcm56990_b0_i2c_devices) / sizeof(i2c_device_t);
        sal_memcpy(dev_info,
                   &bcm56990_b0_i2c_devices,
                   sizeof(bcm56990_b0_i2c_devices));
    }
    return SHR_E_NONE;
}

static bcma_i2c_device_drv_t bcm56990_b0_i2c_dev_drv = {
    .platform_get    = bcm56990_b0_i2c_platform_info_get,
    .dev_get         = bcm56990_b0_i2c_dev_get,
    .voltage_dev_get = bcm56990_b0_bb_voltage_dev_get,
    .temp_dev_get    = bcm56990_b0_temperature_dev_get
};

/*******************************************************************************
 * Public functions
 */
bcma_i2c_device_drv_t *
bcm56990_b0_i2c_drv_get(int unit)
{
   return &bcm56990_b0_i2c_dev_drv;
}

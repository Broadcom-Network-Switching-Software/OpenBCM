/*! \file bcma_i2c_device.h
 *
 * Data type for slave device access.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_I2C_DEVICE_H
#define BCMA_I2C_DEVICE_H

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <sal/sal_mutex.h>

#include <bcmbd/bcmbd_i2c.h>
#include <bcma/io/bcma_io_file.h>
#include <bcmbd/bcmbd.h>

/*! Upper byte range calculation */
#define L16_RANGE_UPPER(L16, range) ((L16) + (int)(range)*L16/100000)

/*! Lower byte range calculation */
#define L16_RANGE_LOWER(L16, range) ((L16) - (int)(range)*L16/100000)

/*! MAX config entry */
#define MAX_VS_CONFIG    2

/*! PM Bus Compliant ISL68127, LTC 3884 */

/*! PM Bus page command */
#define PMBUS_CMD_PAGE                   0x0
/*! PM Bus operation command */
#define PMBUS_CMD_OPERATION              0x1
/*! PM Bus on and off command */
#define PMBUS_CMD_ON_OFF_CONFIG          0x2
/*! PM Bus clear fault command */
#define PMBUS_CMD_CLEAR_FAULTS           0x3
/*! PM Bus write protect command */
#define PMBUS_CMD_WRITE_PROTECT          0x10
/*! PM Bus store user data command */
#define PMBUS_CMD_STORE_USER_ALL         0x15
/*! PM Bus capacity command */
#define PMBUS_CMD_CAPACITY               0x19
/*! PM Bus vout mode command */
#define PMBUS_CMD_VOUT_MODE              0x20
/*! PM Bus vout command */
#define PMBUS_CMD_VOUT_COMMAND           0x21
/*! PM Bus vout max command */
#define PMBUS_CMD_VOUT_MAX               0x24
/*! PM Bus vout margin high command */
#define PMBUS_CMD_VOUT_MARGIN_HIGH       0x25
/*! PM Bus vout margin low command */
#define PMBUS_CMD_VOUT_MARGIN_LOW        0x26
/*! PM Bus vin on command */
#define PMBUS_CMD_VIN_ON                 0x35
/*! PM Bus vin off command */
#define PMBUS_CMD_VIN_OFF                0x36
/*! PM Bus iout command */
#define PMBUS_CMD_IOUT_CAL_GAIN          0x38
/*! PM Bus vout fault command */
#define PMBUS_CMD_VOUT_OV_FAULT_LIMIT    0x40
/*! PM Bus vout over voltage fault command */
#define PMBUS_CMD_VOUT_OV_FAULT_RES      0x41
/*! PM Bus vout over voltage warn limit command */
#define PMBUS_CMD_VOUT_OV_WARN_LIMIT     0x42
/*! PM Bus vout under voltage limit command */
#define PMBUS_CMD_VOUT_UV_WARN_LIMIT     0x43
/*! PM Bus vout under voltage fault command */
#define PMBUS_CMD_VOUT_UV_FAULT_LIMIT    0x44
/*! PM Bus power good on command */
#define PMBUS_CMD_POWER_GOOD_ON          0x5E
/*! PM Bus power good off command */
#define PMBUS_CMD_POWER_GOOD_OFF         0x5F
/*! PM Bus power on delay command */
#define PMBUS_CMD_POWER_ON_DELAY         0x60
/*! PM Bus power on fault command */
#define PMBUS_CMD_POWER_ON_FAULT_LIMIT   0x62
/*! PM Bus status byte command */
#define PMBUS_CMD_STATUS_BYTE            0x78
/*! PM Bus status word command */
#define PMBUS_CMD_STATUS_WORD            0x79
/*! PM Bus vout read command */
#define PMBUS_CMD_READ_VOUT              0x8B
/*! PM Bus iout read command */
#define PMBUS_CMD_READ_IOUT              0x8C
/*! PM Bus pout read command */
#define PMBUS_CMD_READ_POUT              0x96
/*! PM Bus manufacture ID command */
#define PMBUS_CMD_MFR_ID                 0x99
/*! PM Bus manufacture model command */
#define PMBUS_CMD_MFR_MODEL              0x9A
/*! PM Bus manufacture common command */
#define PMBUS_CMD_MFR_COMMON             0xEF
/*! PM Bus manufacture reset command */
#define PMBUS_CMD_MFR_RESET              0xFD

/*! PM Bus io control set DAC opcode */
#define PMBUS_IOC_DAC_SET                0x1
/*! PM Bus io control set vout opcode */
#define PMBUS_IOC_VOUT_SET               0x2
/*! PM Bus io control read vout opcode */
#define PMBUS_IOC_VOUT_READ              0x3
/*! PM Bus io control set sequence opcode */
#define PMBUS_IOC_SEQ_SET                0x4
/*! PM Bus io control read sequence opcode */
#define PMBUS_IOC_SEQ_READ               0x5
/*! PM Bus io control read iout opcode */
#define PMBUS_IOC_IOUT_READ              0x6
/*! PM Bus io control read pout opcode */
#define PMBUS_IOC_POUT_READ              0x7

/*! PM Bus Compliant Device List */

/*! NXP 8574 8bit IO expander */
#define PCF8574_DEVICE_TYPE   0x00000005

/*! NXP 9548 8-ch I2C Switch */
#define PCA9548_DEVICE_TYPE   0x0000000E

/*! LTC 2974 Voltage Sensor */
#define LTC2974_DEVICE_TYPE   0x00000015

/*! LTM 4678 Voltage Regularor/Sensor */
#define LTM4678_DEVICE_TYPE   0x0000001A

/*! ISL 68127 Voltage Regularor/Sensor */
#define ISL68127_DEVICE_TYPE  0x49D22800

/*! ISL 68239 Voltage Regularor/Sensor */
#define ISL68239_DEVICE_TYPE  0x49D24B00

/*! Infineon XDPE132 Voltage Regularor/Sensor */
#define XDPE132_DEVICE_TYPE   0x00000076


/*! Derive device parameter block */
#define DEV_DAC_PARAMS(dev, chan) \
    (((device_data_t *)(((i2c_device_t *)(dev))->priv_data))->dac_params[chan])

/*! Derive device parameter block length */
#define DEV_DAC_PARAM_LEN(dev) \
    (((device_data_t *)(((i2c_device_t *)(dev))->priv_data))->dac_param_len)

/*! Derive device flag */
#define DEV_FLAGS(dev) \
    (((device_data_t *)(((i2c_device_t *)(dev))->priv_data))->flags)

/*! Derive device address for rail bus */
#define DEV_RAIL_SADDR(dev) \
    (((device_data_t *)(((i2c_device_t *)(dev))->priv_data))->rail_saddr)


/*! Check dev pointer return */
#define DEV_CHECK_RETURN(dev) \
{ \
    if ((dev) == NULL) { \
        return SHR_E_INTERNAL; \
    } \
}

/*! Check dev data pointer return */
#define DEV_PRIVDATA_CHECK_RETURN(dev) \
{ \
    DEV_CHECK_RETURN(dev) \
    if ((dev)->priv_data == NULL) { \
        return SHR_E_INTERNAL; \
    } \
}


/*! LTC 2974 resisitor config */
#define LTC2974_RES_CONFIG          0x2

/*! LTM 4678 resisitor config */
#define LTM4678_RES_CONFIG          0x2

/*! DAC name */
#define I2C_DAC_0                   "dac0"

/*! DAC address */
#define I2C_DAC_SADDR0              0x2C

/*! DAC ops set output to MIN */
#define I2C_DAC_IOC_SETDAC_MIN      0
/*! DAC ops set output to MAX */
#define I2C_DAC_IOC_SETDAC_MAX      1
/*! DAC set min calibration value */
#define I2C_DAC_IOC_CALIBRATE_MIN   2
/*! DAC set max calibration value */
#define I2C_DAC_IOC_CALIBRATE_MAX   3
/*! DAC set step calibration value */
#define I2C_DAC_IOC_CALIBRATE_STEP  4
/*! DAC set VOUT operation */
#define I2C_DAC_IOC_VOUT_SET        5
/*! Upload DAC calibration table */
#define I2C_DAC_IOC_CALTAB_SET      6
/*! Reset DAC to its H/W midrange */
#define I2C_DAC_IOC_SETDAC_MID      7

/*! Calibration index:VDD Core A */
#define I2C_DAC_CAL_A               0
/*! Calibration index:VDD Core B */
#define I2C_DAC_CAL_B               1
/*! Calibration index:VDD PHY */
#define I2C_DAC_CAL_P               2
/*! Calibration index:VDD Turbo */
#define I2C_DAC_CAL_T               3

/*! LTC read VOUT operation    */
#define I2C_LTC_IOC_VOUT_READ       8
/*! LTC set nominal voltage    */
#define I2C_LTC_IOC_NOMINAL         9
/*! LTC set vout operation     */
#define I2C_LTC_IOC_VOUT_SET        10
/*! LTC read current operation */
#define I2C_LTC_IOC_IOUT_READ       11
/*! LTC read power operation   */
#define I2C_LTC_IOC_POUT_READ       12
/*! LTC device calibration     */
#define I2C_LTC_IOC_CALTAB_SET      I2C_DAC_IOC_CALTAB_SET
/*! LTC device setting config  */
#define I2C_LTC_IOC_CONFIG_SET      13
/*! LTC setting Resistor config */
#define I2C_LTC_IOC_RCONFIG_SET     14
/*! LTC setting sequencing duration */
#define I2C_SEQ_SET                 15
/*! LTC reading sequencing duration */
#define I2C_SEQ_READ                16

/*! BoardID string name */
#define LPT      "BoardID"
/*! Mux string name */
#define PCF8574  "PCF8574"
/*! Mux string name */
#define MUX      "I2C MUX"
/*! Mux string name */
#define MUX9548  "MUX9548"
/*! Thermal chip string name */
#define MAX669X  "MAX669X"
/*! voltage regulator string name */
#define LTM4678  "LTM4678"
/*! voltage regulator string name */
#define LTC2974  "LTC2974"
/*! voltage regulator string name */
#define ISL68127 "ISL68127"
/*! voltage regulator string name */
#define ISL68239 "ISL68239"
/*! voltage regulator string name */
#define XDPE132  "XDPE132"

/*! TRUE */
#define TRUE  1
/*! FALSE */
#define FALSE 0

/*! Device channel 0 */
#define CHANNEL0        0
/*! Device channel 1 */
#define CHANNEL1        1
/*! Device channel 2 */
#define CHANNEL2        2
/*! Device channel 3 */
#define CHANNEL3        3

/*! LTC2974 Total device channel. */
#define LTC2974_TOTAL_CHANNEL   4
/*! LTM4678 Total device channel. */
#define LTM4678_TOTAL_CHANNEL   2
/*! ISL68127 Total device channel. */
#define ISL68127_TOTAL_CHANNEL  1
/*! ISL68239 Total device channel. */
#define ISL68239_TOTAL_CHANNEL  2
/*! XDPE132 Total device channel. */
#define XDPE132_TOTAL_CHANNEL   2

/*! Voltage configuration value for LTC2974 */
/*! 0.6V */
#define LTC2974_0_6V        4915
/*! 0.8V */
#define LTC2974_0_8V        6554
/*! 1.0V */
#define LTC2974_1V          8192
/*! 1.2V */
#define LTC2974_1_2V        9830
/*! 1.35V */
#define LTC2974_1_35V       11059
/*! 1.8V */
#define LTC2974_1_8V        14746
/*! 1.95V */
#define LTC2974_1_95V       15974
/*! 3.3V */
#define LTC2974_3_3V        27034
/*! 3.4V */
#define LTC2974_3_4V        27853

/*! Voltage configuration value for LTM4768 */
/*! 0.6V */
#define LTM4768_0_6V        2458
/*! 0.8V */
#define LTM4768_0_8V        3277
/*! 0.9V */
#define LTM4768_0_9V        3686

/*! Voltage configuration value for ISL68127 */
/*! 0.6V */
#define ISL68127_0_6V       600
/*! 0.76V */
#define ISL68127_0_76V      760
/*! 0.8V */
#define ISL68127_0_8V       800
/*! 0.84V */
#define ISL68127_0_84V      840
/*! 0.88V */
#define ISL68127_0_88V      880
/*! 1V */
#define ISL68127_1V         1000
/*! 1.8V */
#define ISL68127_1_8V       1800
/*! 3.3V */
#define ISL68127_3_3V       3300

/*! Voltage configuration value for ISL68239 */
/*! 0.6V */
#define ISL68239_0_6V       600
/*! 0.76V */
#define ISL68239_0_76V      760
/*! 0.8V */
#define ISL68239_0_8V       800
/*! 0.84V */
#define ISL68239_0_84V      840
/*! 0.88V */
#define ISL68239_0_88V      880
/*! 0.9V */
#define ISL68239_0_9V       900
/*! 1V */
#define ISL68239_1V         1000

/*! Voltage configuration value for XDPE132 */
/*! 0.6V */
#define XDPE132_0_6V        2458
/*! 0.8V */
#define XDPE132_0_8V        3277
/*! 0.88V */
#define XDPE132_0_88V       3604
/*! 0.9V */
#define XDPE132_0_9V        3686
/*! 1V */
#define XDPE132_1V          4096

/*!
 *! DAC Calibration table for various ADC inputs and DAC chips.
 *! The ioctl call indexes this table with the len parameter
 *! and store the associated DAC Volts/Step value after setting max/min
 *! via reads from a MAX127 A/D device.
 */
typedef struct dac_calibrate_s {
    /*! Device address as ID */
    int saddr;

    /*! Name */
    char *name;

    /*! Max value */
    double max;

    /*! Min value */
    double min;

    /*! Step information */
    double step;

    /*! Last calibration value */
    unsigned short dac_last_val;

    /*! Max hardware value */
    unsigned short dac_max_hwval;

    /*! Min hardware value */
    unsigned short dac_min_hwval;

    /*! Medium hardware value */
    unsigned short dac_mid_hwval;

    /*! Use max hardware value */
    unsigned short use_max;
} dac_calibrate_t;

/*! Maximum device channel */
#define DAC_MAX_CHANNEL 4

/*! Per voltage regulator control */
typedef struct device_data_s {
    /*! Mutex lock for device operation */
    sal_mutex_t lock;

    /*! DAC params for each device channel*/
    dac_calibrate_t *dac_params[DAC_MAX_CHANNEL];

    /*! DAC params length */
    int dac_param_len;

    /*! Config flags */
    int flags;

    /*! Rail address */
    i2c_addr_t rail_saddr;
} device_data_t;

/*! Device specific show function type. */
typedef void (*i2c_show_func_t)(int unit,
                                int saddr,
                                void *record,
                                int force);

/*! Device specific read function type. */
typedef int (*i2c_read_func_t)(int unit,
                               int saddr,
                               uint16_t addr,
                               uint8_t *data,
                               uint32_t *len);

/*! Device specific write function type. */
typedef int (*i2c_write_func_t)(int unit,
                                int saddr,
                                uint16_t addr,
                                uint8_t *data,
                                uint32_t len);

/*! Device specific ioctl function type. */
typedef int (*i2c_ioc_func_t)(int unit,
                              int saddr,
                              int opcode,
                              void *data,
                              int len);

/*! Device specific init function type. */
typedef int (*i2c_dev_init_func_t)(int unit,
                                   int saddr,
                                   void *data,
                                   int len);

/*! Device specific cleanup function type. */
typedef int (*i2c_dev_deinit_func_t)(int unit,
                                     int saddr,
                                     void *data,
                                     int len);

/*! I2C Driver structure. */
typedef struct i2c_driver_s {
    /*! Device flags */
    uint8_t flags;

    /*! Index into device descriptor table */
    int devno;

    /*! Device Serial No, GUID, or identifier */
    uint32_t id;

    /*! Read routine */
    i2c_read_func_t read;

    /*! Write routine */
    i2c_write_func_t write;

    /*! io control routine */
    i2c_ioc_func_t ioctl;

    /*! called once at startup */
    i2c_dev_init_func_t load;

    /*! called once at the end */
    i2c_dev_deinit_func_t unload;

    /*! Show routine */
    i2c_show_func_t show;
} i2c_driver_t;

/*! I2C device structure to hold driver and debug data */
typedef struct i2c_dev_s {
    /*! Device name, eg. "eeprom0" */
    char *devname;

    /*! Slave address */
    i2c_addr_t saddr;

    /*! Driver routines */
    i2c_driver_t *driver;

    /*! Test data */
    void* testdata;

    /*! Size of test data */
    int testlen;

    /*! Bytes transmitted */
    uint32_t tbyte;

    /*! Bytes received */
    uint32_t rbyte;

    /*! Description */
    char *desc;

    /*! private data per i2c device */
    void *priv_data;
} i2c_device_t;

/*! Generic Sense Resistor  data */
typedef struct i2c_ltc_s {
    /*! Index */
    int idx;

    /*! Voltage source to configure */
    char *function;

    /*! Name of LTCXXXX device */
    char *devname;

    /*! Device channel */
    int ch;

    /*! resistor value */
    uint16_t res_value;

     /*! 1:Device is configured */
    int flag;
} i2c_ltc_t;

/*! Generic ADC data */
typedef struct i2c_adc_s {
    /*! value */
    double val;

    /*! Min value */
    double min;

    /*! Max value */
    double max;

    /*! Delta value */
    double delta;

    /*! Calibration samples */
    int  nsamples;
} i2c_adc_t;

/*! I2C device name, address and driver assignment */
#define I2CDEV(_name, _num, _drv) {     \
        _name,                          \
        _num,                           \
        &bcma_i2c_##_drv##_driver,    \
        }

/*! Defines for the bb_vparams_t flags field */

/*! DAC not calibrated */
#define DAC_UNCALIBRATED 0x00000000

/*! DAC calibrated */
#define DAC_CALIBRATED   0x00000001

/*! LTC ADC/DAC Device */
#define PWR_DAC_IO_SRC   (0x40000|DAC_CALIBRATED)

/*! PMBUS ADC/DAC Device */
#define PWR_DAC_IO_PMBUS (0x80000|DAC_CALIBRATED)

/*! Structure to hold IOCTL input data */
typedef struct ioctl_in_s {
    /*! Board ID */
    uint32_t board_id;

    /*! Dev structure for this operation */
    i2c_device_t *dev;

    /*! Sense resistor config */
    i2c_ltc_t *sensor_config;

    /*! Calibration record */
    dac_calibrate_t *cali_record;

    /*! Voltage value to set */
    double voltage;
} ioctl_in_t;

/*! Structure to hold IOCTL output data */
typedef struct ioctl_out_s {
    /*! data to hold double type */
    double data[5];
} ioctl_out_t;

/*! Structure to hold both input and output */
typedef struct ioctl_op_data_s {
    /*! Entry to hold ioctl input parameter */
    ioctl_in_t input;

    /*! Entry to hold ioctl readback data */
    ioctl_out_t output;
} ioctl_op_data_t;

/*! Board temperature type */
typedef int bcma_temperature_t;

/*!
 * MAX669X control structure.
 */
typedef struct {
    /*! last temp value 1 */
    bcma_temperature_t remote_temp_1;

    /*! last temp value 2 */
    bcma_temperature_t remote_temp_2;

    /*! last local temp */
    bcma_temperature_t local_temp;
} temperature_dev_info_t;

/*! celsius to farenheit */
#define C2F(_d)         ((9*(_d)/5) + 32)

/*! PCA9548 driver */
extern i2c_driver_t bcma_i2c_pca9548_driver;

/*! LTC2974 driver */
extern i2c_driver_t bcma_i2c_ltc2974_driver;

/*! ISL68127 driver */
extern i2c_driver_t bcma_i2c_isl68127_driver;

/*! ISL68239 driver */
extern i2c_driver_t bcma_i2c_isl68239_driver;

/*! XDPE132 driver */
extern i2c_driver_t bcma_i2c_xdpe132_driver;

/*! LTM4678 driver */
extern i2c_driver_t bcma_i2c_ltm4678_driver;

/*! MAX669X driver */
extern i2c_driver_t bcma_i2c_max669x_driver;

/*! PCF8574 driver */
extern i2c_driver_t bcma_i2c_pcf8574_driver;

/*!
 * \brief Derive I2C device address.
 *
 * \param [in] unit  Unit number
 * \param [in] devid Device ID
 * \param [in] dev_info Device array
 *
 * \return I2C slave device address.
 */
extern i2c_addr_t
bcma_i2c_addr(int unit, int devid, i2c_device_t *dev_info);

/*!
 * \brief Derive I2C device name.
 *
 * \param [in] unit  Unit number
 * \param [in] devid Device ID
 * \param [in] dev_info Device array
 *
 * \return I2C slave device name.
 */
extern const char *
bcma_i2c_devname(int unit, int devid, i2c_device_t *dev_info);

#endif /* BCMA_I2C_DEVICE_H */

/*! \file bcma_i2ccmd_op.h
 *
 * Data types to support CLI i2c command operation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_I2CCMD_OP_H
#define BCMA_I2CCMD_OP_H

#include <bcma/cli/bcma_cli.h>
#include <bcma/i2c/bcma_i2c_device.h>
#include <bcma/i2c/bcma_i2c_board_type.h>

/*! CLI max key word length. */
#define BCMA_I2C_NAME_LEN             20

/*! CLI max argument count. */
#define I2C_MAX_ARG_COUNT             10

/*! CLI total length per entry input. */
#define BCMA_I2C_ENTRY_LEN            2048

/*! CLI total byte count per operaion. */
#define BCMA_I2C_DATA_INPUT_BYTE_CNT  256

/*! CLI total byte count per data value input. */
#define BCMA_I2C_PER_DATA_SIZE        10

/*! Types of bcma i2c operations code. */
typedef enum bcma_i2c_opcode_s {
    /*! Quick command */
    BCMA_I2C_QUICK_CMD = 0,

    /*! Write one byte data to slave. */
    BCMA_I2C_WRITE_BYTE,

    /*! Read one byte data from slave. */
    BCMA_I2C_READ_BYTE,

    /*! Write two bytes data to slave. */
    BCMA_I2C_WRITE_WORD,

    /*! Read two bytes data from slave. */
    BCMA_I2C_READ_WORD,

    /*! Write one byte data with command to slave. */
    BCMA_I2C_WRITE_BYTE_DATA,

    /*! Read one byte data with command from slave. */
    BCMA_I2C_READ_BYTE_DATA,

    /*! Write two bytes data with command to slave. */
    BCMA_I2C_WRITE_WORD_DATA,

    /*! Read two bytes data with command from slave. */
    BCMA_I2C_READ_WORD_DATA,

    /*! Write multi bytes data with command to slave. */
    BCMA_I2C_BLOCK_WRITE,

    /*! Read multi bytes data with command from slave. */
    BCMA_I2C_BLOCK_READ,

    /*! Write multi bytes data to slave. */
    BCMA_I2C_MULTI_WRITE,

    /*! Probe I2C to verify device list. */
    BCMA_I2C_PROBE,

    /*! Reset I2C bus. */
    BCMA_I2C_RESET
} bcma_i2c_opcode_t;

/*! Bcmbd i2c operations data holder. */
typedef struct bcma_i2c_cli_struct_s {
    /*! I2C opcode. */
    uint8_t op_code;

    /*! Slave device address. */
    uint8_t saddr;

    /*! Reset command. */
    uint8_t reset;

    /*! Probe command. */
    uint8_t probe;

    /*! Specific command or offset to send to slave device. */
    uint8_t  command;

    /*! Total data count for operation. */
    uint8_t  count;

    /*! Output debug message. */
    uint8_t  verbose;

    /*! Data buffer holds CLI data byte input. */
    uint8_t  data[256];

    /*! Data buffer holds CLI data word inpu. */
    uint16_t  data_word;
} bcma_i2c_cli_struct_t;

/*! Byte count to hold device name. */
#define BCMA_I2C_DEVICE_NAME_LEN 50

/*! Maximum number of devices supported. */
#define BCMA_I2C_MAXIMUM_DEVICE  20

/*! Structure to hold i2c device information. */
typedef struct bcma_i2c_device_s {
   /*! Slave device address. */
   uint8_t saddr;

   /*! Slave device name. */
   uint8_t name[BCMA_I2C_DEVICE_NAME_LEN];
} bcma_i2c_device_t;

/*!
 * BCMA i2c board device list.
 */
typedef struct bcma_i2c_device_list_s {
    /*! Total supported device. */
    uint32_t d_total;

    /*! Device list. */
    bcma_i2c_device_t d_list[BCMA_I2C_MAXIMUM_DEVICE];
} bcma_i2c_device_list_t;


/*! Max bb argument count. */
#define BB_MAX_ARG_COUNT 20

/*! Max temperature_ argument count. */
#define TEMPERATURE_MAX_ARG_COUNT 20

/*! Complete voltage regulator profile per board design */
typedef struct bb_voltage_profile_s {
    /*! DAC calibration table index: See ltc1427.c */
    int id;

    /*! Voltage source to configure */
    char *function_name;

    /*! Is it behind mux */
    uint8_t behind_mux;

    /*! Mux name. */
    char *mux_name;

    /*! Mux address */
    uint8_t mux_addr;

    /*! Mux channel to turn on access */
    uint8_t mux_channel;

    /*! Voltage regulator name */
    char *adc_name;

    /*! Voltage regulator address */
    uint8_t adc_addr;

    /*! Voltage regulator output channel */
    uint8_t adc_channel;

    /*! Voltage regulator name */
    char *dac_name;

    /*! Control Flag. */
    int flags;
} bb_voltage_profile_t;

/*! Complete base board i2C device list */
typedef struct bb_i2c_device_s {
    /*! Structure already linked with per board config */
    int initialized;

    /*! Total profile count */
    int p_cnt;

    /*! Voltage regulator profile */
    bb_voltage_profile_t *voltage_profile;

    /*! Mux count */
    int m_cnt;

    /*! Mux device */
    i2c_device_t *mux_device;

    /*! ID device count */
    int id_cnt;

    /*! ID device */
    i2c_device_t *id_device;

    /*! Voltage device count */
    int v_cnt;

    /*! Voltage device */
    i2c_device_t *v_device;

    /*! Sense resistor config count */
    int sensor_cnt;

    /*! Sense resistor config */
    i2c_ltc_t *sensor_config;

    /*! Calibration record count */
    int cali_cnt;

    /*! Calibration record */
    dac_calibrate_t *cali_record;
} bb_i2c_device_t;

/*! Derive chiped based base board type structure. */
typedef bcma_i2c_base_board_t *(*bcma_i2c_platform_info_get_f)(int unit);

/*! Derive complete I2C device list per board design. */
typedef int (*bcma_i2c_dev_get_f)(int unit,
                                  int *total,
                                  i2c_device_t *dev_info);

/*! Derive voltage regulator device list per board design. */
typedef int (*bcma_i2c_voltage_dev_get_f)(int unit,
                                          bb_i2c_device_t *dev_info);

/*! Derive temperature regulator device list per board design. */
typedef int (*bcma_i2c_temperature_dev_get_f)(int unit,
                                              int *total,
                                              i2c_device_t *dev_info);

/*!
 * \brief Board I2C related device driver object
 *
 * Driver is used to access I2C related devices.
 */
typedef struct bcma_i2c_device_drv_s {
    /*! Get base board type based on board_id */
    bcma_i2c_platform_info_get_f   platform_get;

    /*! Get complete I2C device list. */
    bcma_i2c_dev_get_f dev_get;

    /*! Get voltage regulator device list. */
    bcma_i2c_voltage_dev_get_f voltage_dev_get;

    /*! Get thermal device list. */
    bcma_i2c_temperature_dev_get_f temp_dev_get;
} bcma_i2c_device_drv_t;

/*! Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcma_i2c_device_drv_t *_bc##_i2c_drv_get(int unit);

/*! Include externs to header. */
#define BCMDRD_DEVLIST_OVERRIDE

#include <bcmdrd/bcmdrd_devlist.h>

/*! Board specific I2C device driver info. */
extern bcma_i2c_device_drv_t *bcma_i2c_device_drv[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Init per board I2C device driver per usage type.
 *
 * \param[in] unit Unit number.
 *
 * \return converted value.
 */
int bcma_i2c_board_drv_init(int unit);

#endif /* BCMA_I2CCMD_OP_H */

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * I2C Device Driver for LTC2974 an integrated power control IC.
 */

#include <sal/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>
 /* Divided by 2^13 for voltage conversion */
#define L16_TO_V(val)  ((val)*100000/8192/100000)
#define V_TO_L16(val)  ((val)*8192)
#define L16_TO_UV(val) ((val)*100000/8192*10)
#define UV_TO_L16(val) ((val)/10*8192/100000)

#define L16_RANGE_UPPER(L16, range) ((L16) + (int)(range)*L16/100000)
#define L16_RANGE_LOWER(L16, range) ((L16) - (int)(range)*L16/100000)

#define MAX_VS_CONFIG    2

#define POWER(exponent, input_val)  exponent < 0 ? \
                            (int)((int) (input_val) << (exponent*(-1))): \
                            (int)((int) (input_val*1000000) >> \
			    exponent)/1000000

typedef struct device_data_s {
    sal_mutex_t lock;   /* mutex lock for device operation */
    dac_calibrate_t *dac_params;  /*dac params for each device channel*/
    int dac_param_len;
} device_data_t;

#define DEV_DAC_PARAMS(dev) \
    (((device_data_t *)(((i2c_device_t *)(dev))->priv_data))->dac_params)
#define DEV_DAC_PARAM_LEN(dev) \
    (((device_data_t *)(((i2c_device_t *)(dev))->priv_data))->dac_param_len)   

#define DEV_CHECK_RETURN(dev) \
{ \
    if ((dev) == NULL) { \
        return SOC_E_INTERNAL; \
    } \
}

#define DEV_PRIVDATA_CHECK_RETURN(dev) \
{ \
    DEV_CHECK_RETURN(dev) \
    if ((dev)->priv_data == NULL) { \
        return SOC_E_INTERNAL; \
    } \
}

static i2c_ltc_t* sense_resistor_config;
typedef struct _ltc_board_resistor_map_s {
    int offset;
    i2c_ltc_t* sense_resistor_config;
} _ltc_board_resistor_map_t;

#define ML_MAX_VS_CONFIG     4
#define MONTEREY_MAX_VS_CONFIG     4
#if 0
static i2c_ltc_t sense_resistor_config_56270[ML_MAX_VS_CONFIG]={
    { 0, "Core",   I2C_LTC3880_41, I2C_LTC3880_CH0, LTC3880_CORE_RES_CONFIG,  0 },
    { 1, "Analog", I2C_LTC3880_42, I2C_LTC3880_CH1, LTC3880_ANLG_RES_CONFIG,  0 },
    { 2, "1.8",    I2C_LTM4676_43, I2C_LTC3880_CH1, LTC3880_1_8V_RES_CONFIG,  0 },
    { 3, "3.3",    I2C_LTM4676_43, I2C_LTC3880_CH0, LTC3880_3_3V_RES_CONFIG,  0 },
};
#endif

static i2c_ltc_t sense_resistor_config_56270[ML_MAX_VS_CONFIG]={
    { 0, "Core",   I2C_LTC2974_0, I2C_LTC2974_CH0, 0x1,  0 },
    { 1, "Analog", I2C_LTC2974_0, I2C_LTC2974_CH1, 0x2,  0 },
    { 2, "3.3",    I2C_LTC2974_0, I2C_LTC2974_CH2, 0x2,  0 },
    { 3, "1.8",    I2C_LTC2974_0, I2C_LTC2974_CH3, 0x2,  0 },
};
static i2c_ltc_t sense_resistor_config_56670[MONTEREY_MAX_VS_CONFIG]={
    { 0, "1.2V",   I2C_LTC2974_64, I2C_LTC2974_CH0, 0x4,  0 },
    { 1, "3.3dut", I2C_LTC2974_64, I2C_LTC2974_CH1, 0x44,  0 },
    { 2, "1.8V",    I2C_LTC2974_64, I2C_LTC2974_CH2, 0x14,  0 },
    { 3, "1.2mdio",    I2C_LTC2974_64, I2C_LTC2974_CH3, 0xC8,  0 },
 
};
#define TH2_MAX_VS_CONFIG     4
static i2c_ltc_t sense_resistor_config_56970[TH2_MAX_VS_CONFIG]={
    { 0, "Analog", I2C_LTC2974_5C, I2C_LTC2974_CH0, 0xF801/*0.5*/,  0 },
    { 1, "1.2",    I2C_LTC2974_5C, I2C_LTC2974_CH1, 0x4 /*4*/  ,  0 },
    { 2, "1.8",    I2C_LTC2974_5C, I2C_LTC2974_CH2, 0xC8/*200*/,  0 },
    { 3, "3.3",    I2C_LTC2974_5C, I2C_LTC2974_CH3, 0xC8/*200*/,  0 },
};

#define TD3_MAX_VS_CONFIG     4
static i2c_ltc_t sense_resistor_config_56870[TD3_MAX_VS_CONFIG]={
    { 0, "Analog", I2C_LTC2974_5C, I2C_LTC2974_CH0, 0xF801/*0.5*/,  0 },
    { 1, "1.2",    I2C_LTC2974_5C, I2C_LTC2974_CH1, 0x4 /*4*/  ,  0 },
    { 2, "1.8",    I2C_LTC2974_5C, I2C_LTC2974_CH2, 0xC8/*200*/,  0 },
    { 3, "3.3",    I2C_LTC2974_5C, I2C_LTC2974_CH3, 0xC8/*200*/,  0 },
};

#define TH3_MAX_VS_CONFIG     4
static i2c_ltc_t sense_resistor_config_56980[TH3_MAX_VS_CONFIG]={
    { 0, "MDIO",   I2C_LTC2974_5C, I2C_LTC2974_CH0, 0xF801/*0.5*/,  0 },
    { 1, "1.2",    I2C_LTC2974_5C, I2C_LTC2974_CH1, 0x4 /*4*/  ,  0 },
    { 2, "1.8",    I2C_LTC2974_5C, I2C_LTC2974_CH2, 0xC8/*200*/,  0 },
    { 3, "3.3",    I2C_LTC2974_5C, I2C_LTC2974_CH3, 0xC8/*200*/,  0 },
};


#define HX5_MAX_VS_CONFIG     8
static i2c_ltc_t sense_resistor_config_56370[HX5_MAX_VS_CONFIG]={
    { 0, "0.88V",    I2C_LTC2974_63, I2C_LTC2974_CH0, 0xEA20,  0 },
    { 1, "3.3V_Dut", I2C_LTC2974_63, I2C_LTC2974_CH1, 0xEA20,  0 },
    { 2, "2.5V",     I2C_LTC2974_63, I2C_LTC2974_CH2, 0xEA20,  0 },
    { 3, "VDD_MDIO", I2C_LTC2974_63, I2C_LTC2974_CH3, 0xF320,  0 },
    { 4, "1.8V",     I2C_LTC2974_64, I2C_LTC2974_CH0, 0xDA80,  0 },
    { 5, "1.2V",     I2C_LTC2974_64, I2C_LTC2974_CH1, 0xD280,  0 },
    { 6, "3.3V",     I2C_LTC2974_64, I2C_LTC2974_CH2, 0xBA00,  0 },    
    { 7, "5.0V",     I2C_LTC2974_64, I2C_LTC2974_CH3, 0xEA20,  0 }
};


#define HR4_MAX_VS_CONFIG     7
static i2c_ltc_t sense_resistor_config_56275[HR4_MAX_VS_CONFIG]={
    { 0, "0.88V",    I2C_LTC2974_63, I2C_LTC2974_CH1, 0xF320,  0 },
    { 1, "1.2V",     I2C_LTC2974_63, I2C_LTC2974_CH2, 0xEA20,  0 },
    { 2, "1.8V",     I2C_LTC2974_63, I2C_LTC2974_CH3, 0xEA20,  0 },
    { 3, "2.5V",     I2C_LTC2974_64, I2C_LTC2974_CH0, 0xF320,  0 },
    { 4, "3.3V_Dut", I2C_LTC2974_64, I2C_LTC2974_CH1, 0xD280,  0 },
    { 5, "3.3V",     I2C_LTC2974_64, I2C_LTC2974_CH2, 0xD100,  0 },
    { 6, "5.0V",     I2C_LTC2974_64, I2C_LTC2974_CH3, 0xEA20,  0 }
};

#define FL_MAX_VS_CONFIG     7
static i2c_ltc_t sense_resistor_config_56070[FL_MAX_VS_CONFIG]={
    { 0, "0.8V",     I2C_LTC2974_63, I2C_LTC2974_CH0, 0xCA80/*5*/,     0 },
    { 1, "0.88V",    I2C_LTC2974_63, I2C_LTC2974_CH1, 0xB200/*1*/,  0 },
    { 2, "1.2V",     I2C_LTC2974_63, I2C_LTC2974_CH2, 0xEA20/*68*/,     0 },
    { 3, "1.8V",     I2C_LTC2974_63, I2C_LTC2974_CH3, 0xEA20/*68*/,     0 },
    { 4, "VDD_MDIO", I2C_LTC2974_64, I2C_LTC2974_CH0, 0xF320/*200*/,     0 },
    { 5, "3.3V_Dut", I2C_LTC2974_64, I2C_LTC2974_CH1, 0xEA20/*68*/,     0 },
    { 6, "3.3V",     I2C_LTC2974_64, I2C_LTC2974_CH2, 0x03E8/*5*/,    0 },
};

/* 
 * This table is used to reflect the mapping between board type offset and 
 *  sens_register_config table. The board type offset is used since the IN 
 *  parameter on I2C_LTC_IOC_{SET_CONFIG,SET_RCONFIG,READ_IOUT} takes Metrolite SVK 
 *  board(G56) as basis to identify different board.
 *
 * Information for the board type offset :
 *  1. This is the delta value of board_type index and TH is the index basis.
 *     [Examples] :
 *      - Metrolite(board_type=G56), offset=14
 *  2. The board_type definition can be found in i2c_cmds.c
 */
static _ltc_board_resistor_map_t board_resistor_map[] = {
    {14, sense_resistor_config_56270},   /* Metrolite(G56) */
    {15, sense_resistor_config_56970},   /* TOMAHAWK2(G57) */
    {18, sense_resistor_config_56870},   /* TRIDENT3(G60) */
    {19, sense_resistor_config_56670},   /* MONTEREY(G61) */
    {22, sense_resistor_config_56980},   /* TOMAHAWK3(G64) */
    {23, sense_resistor_config_56370},   /* HELIX5(G65) */
    {24, sense_resistor_config_56275},   /* HURRICANE4(G66) */
    {25, sense_resistor_config_56070},   /* FIRELIGHT(G67) */
};

static sal_mutex_t ioctl_lock = NULL;

/*
 * Convert a floating point value into a
 * LinearFloat5_11 formatted word
 */
STATIC int
#ifdef  COMPILER_HAS_DOUBLE
ltc2974_float_to_L11(double input_val, uint16* data)
#else
ltc2974_float_to_L11(int input_val, uint16* data)
#endif
{
    uint16 uExponent, uMantissa;
    /* set exponent to -16 */
    int16 exponent = -16;
    /* extract mantissa from input value */
    int mantissa = POWER(exponent, input_val); 

    /* Search for an exponent that produces
     * a valid 11-bit mantissa */
    do
    {
        if((mantissa >= -1024) &&
                (mantissa <= +1023))
        {
            break; /* stop if mantissa valid */
        }
        exponent++;
        mantissa = POWER(exponent, input_val);
    } while (exponent < +15);

    /* Format the exponent of the L11 */
    uExponent = exponent << 11;
    /* Format the mantissa of the L11 */
    uMantissa = mantissa & 0x07FF;
    /* Compute value as exponent | mantissa */
    *(data) = uExponent | uMantissa;
    return SOC_E_NONE;
}

/*
 * Convert a LinearFloat5_11 formatted word
 * into a floating point value
 */
STATIC int
ltc2974_L11_to_float(uint16 input_val, void *data)
{
    /* extract exponent as MS 5 bits */
    int8 exponent = input_val >> 11;
    /* extract mantissa as LS 11 bits */
    int16 mantissa = input_val & 0x7ff;
    /* sign extend exponent from 5 to 8 bits */
    if( exponent > 0x0F ) exponent |= 0xE0;
    /* sign extend mantissa from 11 to 16 bits */
    if( mantissa > 0x03FF ) mantissa |= 0xF800;
#ifdef  COMPILER_HAS_DOUBLE
    /* compute value as mantissa * 2^(exponent) */
    *(double *)data= exponent < 0 ?
                      (double) ((mantissa*1000000) >>
                                (exponent*(-1)))/1000000:
                      (double) (mantissa << exponent);
#else
    *(int *)data= exponent < 0 ?
                   (((mantissa)*1000000) >> (exponent*(-1))):
                   ((mantissa) << exponent)*100000;
#endif
    return SOC_E_NONE;
}

STATIC int
ltc2974_wait_for_not_busy(int unit, int devno)
{
    int rv = SOC_E_NONE;
    uint8 mfr_status, saddr;
    uint32 usec, wait_usec;

    wait_usec = 0;
    usec = 10;

    saddr = soc_i2c_addr(unit, devno);

    while(wait_usec < 1000000) {
        SOC_IF_ERROR_RETURN
            (soc_i2c_read_byte_data(unit, saddr,PMBUS_CMD_MFR_COMMON,
                                    &mfr_status));
        soc_i2c_device(unit, devno)->rbyte++;
        if ((mfr_status & 0x70) == 0x70) {
            /* Bit 6 : Chip not busy */
            /* Bit 5 : calculations not pending */
            /* Bit 4 : OUTPUT not in transition */
            break;
        } else {
            sal_udelay(usec);
            wait_usec += usec;
        }
    }

    if ((mfr_status & 0x70) != 0x70) {
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "unit %d i2c %s :ltc2974 is busy !\n"),
                 unit, soc_i2c_devname(unit, devno)));
        rv = SOC_E_TIMEOUT;
    }
    return rv;
}


STATIC int
ltc2974_read(int unit, int devno, uint16 addr, uint8* data, uint32* len)
{
    int rv = SOC_E_NONE;
    uint8 saddr;

    saddr = soc_i2c_addr(unit, devno);

    if (*len == 0) {
        return SOC_E_NONE;
    }
    if (*len == 1) {
        /* reads a single byte from a device, from a designated register*/
        rv = soc_i2c_read_byte_data(unit, saddr, addr,data);
        soc_i2c_device(unit, devno)->rbyte++;
         LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "unit %d i2c %s: LTC2974_read: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                    "rv = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *data, *len, rv));
    } else if (*len == 2) {
        /* reads a single word from a device, from a designated register*/
        rv = soc_i2c_read_word_data(unit, saddr, addr,(uint16 *)data);
        soc_i2c_device(unit, devno)->rbyte +=2;
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "unit %d i2c %s: LTC2974_read: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                    "rv = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *(uint16 *)data, *len, rv));
    } else {
        /* not supported for now */
         LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "unit %d i2c %s: LTC2974_read fail: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *data, *len));
    }
    return rv;
}

STATIC int
ltc2974_write(int unit, int devno, uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    uint8 saddr;
    unsigned short val;

    saddr = soc_i2c_addr(unit, devno);

    if (len == 0) {
        /* simply writes command code to device */
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "i2c %s: LTC2974 write: "
                    "saddr = 0x%x, addr = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno), saddr, addr, len));
        rv = soc_i2c_write_byte(unit, saddr, addr);
    } else if (len == 1) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "i2c %s: LTC2974 write: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno), saddr, addr, *data, len));
        rv = soc_i2c_write_byte_data(unit, saddr, addr, *data);
        soc_i2c_device(unit, devno)->tbyte++;
    } else if (len == 2) {
         LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "i2c %s: LTC2974 write: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno),
                    saddr, addr, *(uint16 *)data, len));
        val = *(unsigned short *)data;
        rv = soc_i2c_write_word_data(unit, saddr, addr, val);
        soc_i2c_device(unit, devno)->tbyte += 2;
    }
    return rv;
}

STATIC int
ltc2974_check_page(int unit, int devno, int ch)
{
     int rv;
     uint8 page;
     uint32 len;

     len = sizeof(char);
     rv = ltc2974_read(unit, devno, PMBUS_CMD_PAGE, &page, &len);
     if (rv != SOC_E_NONE) {
         return rv;
     }

     if (page != ch) {
         page = ch;
         LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "LTC2974 %d set page to %d\n"),
                     soc_i2c_addr(unit, devno), page));
         rv = ltc2974_write(unit, devno, PMBUS_CMD_PAGE, &page, sizeof(char));
     }
     return rv;
 }

STATIC int
ltc2974_get_ch_voltage(int unit, int devno, int ch, unsigned short *voltage)
{
    int rv;
    uint32 len;

    SOC_IF_ERROR_RETURN(ltc2974_check_page(unit, devno, ch));

    len = sizeof(short);
    rv = ltc2974_read(unit, devno, PMBUS_CMD_READ_VOUT, (uint8*)voltage,
                        &len);

    return rv;
}

STATIC int
ltc2974_set_ch_voltage_upper_with_supervision(int unit, int devno, int ch,
                                                        unsigned short voltage)
{

    SOC_IF_ERROR_RETURN(ltc2974_write(unit, devno, PMBUS_CMD_VOUT_COMMAND,
                                        (uint8*)&voltage, sizeof(short)));
    return SOC_E_NONE;
}

STATIC int
ltc2974_set_ch_voltage_lower_with_supervision(int unit, int devno, int ch,
                                                        unsigned short voltage)
{
    SOC_IF_ERROR_RETURN(ltc2974_write(unit, devno, PMBUS_CMD_VOUT_COMMAND,
                                        (uint8*)&voltage, sizeof(short)));

    return SOC_E_NONE;
}
STATIC int
ltc2974_set_ch_voltage(int unit, int devno, int ch, unsigned short voltage)
{
    int rv = SOC_E_NONE;
    unsigned short old_vout;

    SOC_IF_ERROR_RETURN(ltc2974_get_ch_voltage(unit, devno, ch, &old_vout));

    if (voltage >= old_vout) {
        SOC_IF_ERROR_RETURN(ltc2974_set_ch_voltage_upper_with_supervision(unit,
                                                    devno, ch, voltage));
    } else {
        SOC_IF_ERROR_RETURN(ltc2974_set_ch_voltage_lower_with_supervision(unit,
                                                    devno, ch, voltage));
    }


/*
    if (SOC_SUCCESS(rv)) {
        sal_udelay(100000);
    }
*/

    return rv;
}
STATIC int
ltc2974_dac_set_ch_voltage(int unit, int devno, int ch, unsigned short voltage)
{
    int rv;
    i2c_device_t *dev = soc_i2c_device(unit, devno);

    DEV_PRIVDATA_CHECK_RETURN(dev);

    rv = ltc2974_set_ch_voltage(unit, devno, ch, voltage);

    if (SOC_SUCCESS(rv)) {
        /* Keep last value since DAC is write-only device */
        DEV_DAC_PARAMS(dev)[ch].dac_last_val = voltage;
    }

    return rv;
}

/* 
 * This function is used to match the comming offset (delta_index) with a 
 * predefined sense_resistor_configuration table.
 */
STATIC int 
ltc2974_board_resistor_match(int in_offset) 
{
    int i, map_size;

    map_size = sizeof(board_resistor_map) / sizeof(_ltc_board_resistor_map_t);

    for (i = 0; i < map_size; i++) {
        if (board_resistor_map[i].offset == in_offset) {
            sense_resistor_config = board_resistor_map[i].sense_resistor_config;
            return SOC_E_NONE;
        }
    }
    
    return SOC_E_NOT_FOUND;
}

STATIC int
ltc2974_setmin_max(int unit, int devno, int len)
{
    int rv=SOC_E_NONE;
    int data,i,ch;
    uint16 dac;
    i2c_device_t *dev = soc_i2c_device(unit, devno);
    int max_vs_config = 1;

    DEV_PRIVDATA_CHECK_RETURN(dev);

#ifdef BCM_METROLITE_SUPPORT
    if (SOC_CONTROL(unit)->board_type == 0x98) {
        max_vs_config = sizeof(sense_resistor_config_56270)/sizeof(i2c_ltc_t);
    }
#endif

#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_CONTROL(unit)->board_type == 0xAA) {
        sense_resistor_config  = sense_resistor_config_56670 ;
        max_vs_config = sizeof(sense_resistor_config_56670)/sizeof(i2c_ltc_t);
    }
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT)
    if ((SOC_CONTROL(unit)->board_type == 0x9D) ||
        (SOC_CONTROL(unit)->board_type == 0xA3) ||
        (SOC_CONTROL(unit)->board_type == 0x9F) ||
        (SOC_CONTROL(unit)->board_type == 0x9E)) {
        max_vs_config = sizeof(sense_resistor_config_56970)/sizeof(i2c_ltc_t);
    }
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
    if ((SOC_CONTROL(unit)->board_type == 0xA4) ||
        (SOC_CONTROL(unit)->board_type == 0xA5) ||
        (SOC_CONTROL(unit)->board_type == 0xA6) ||
        (SOC_CONTROL(unit)->board_type == 0xA7)) {
        max_vs_config = sizeof(sense_resistor_config_56870)/sizeof(i2c_ltc_t);
    }
#endif

#if defined(BCM_MAVERICK2_SUPPORT)
    if ((SOC_CONTROL(unit)->board_type == 0xA6) ||
        (SOC_CONTROL(unit)->board_type == 0xB5)) {
        max_vs_config = sizeof(sense_resistor_config_56870)/sizeof(i2c_ltc_t);
    }
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if ((SOC_CONTROL(unit)->board_type == 0xAC) || (SOC_CONTROL(unit)->board_type == 0xDE) || 
        (SOC_CONTROL(unit)->board_type == 0xB1) || (SOC_CONTROL(unit)->board_type == 0xAD) || 
        (SOC_CONTROL(unit)->board_type == 0xAF)) {
        max_vs_config = sizeof(sense_resistor_config_56980)/sizeof(i2c_ltc_t);
    }
#endif

#if defined(BCM_HELIX5_SUPPORT)
    if ((SOC_CONTROL(unit)->board_type == 0xB0) || (SOC_CONTROL(unit)->board_type == 0xB3) ||
        (SOC_CONTROL(unit)->board_type == 0xB6)) {
        sense_resistor_config  = sense_resistor_config_56370 ;
        max_vs_config = sizeof(sense_resistor_config_56370)/sizeof(i2c_ltc_t);
    }
#endif

#if defined(BCM_HURRICANE4_SUPPORT)
    if ((SOC_CONTROL(unit)->board_type == 0xBB) ||
        (SOC_CONTROL(unit)->board_type == 0xBC) ||
        (SOC_CONTROL(unit)->board_type == 0xBF)) {
        sense_resistor_config  = sense_resistor_config_56275 ;
        max_vs_config = sizeof(sense_resistor_config_56275)/sizeof(i2c_ltc_t);
    }
#endif


#if defined(BCM_FIRELIGHT_SUPPORT)
    if (SOC_CONTROL(unit)->board_type == 0xC2) {
        sense_resistor_config  = sense_resistor_config_56070 ;
        max_vs_config = sizeof(sense_resistor_config_56070)/sizeof(i2c_ltc_t);
    }
#endif

    for(i=0;i<max_vs_config;i++) {
         /*
          * COVERITY
          *
          * The sense_resistor_config is never overrun. It is guarded by a
          * device specific board-id and device type.
          */
         /* coverity[overrun-local : FALSE] */
         if (sal_strcmp(DEV_DAC_PARAMS(dev)[len].name, 
                        sense_resistor_config[i].function) == 0) {
             break;
         }
    }
    if (i == max_vs_config) {
        LOG_CLI((BSL_META_U(unit, "Error: failed to find sense resistor "
                    "for VDD_%s in  LTC 2974 device.\n"), 
                    DEV_DAC_PARAMS(dev)[len].name));
         return SOC_E_NOT_FOUND;
    }
    if (sense_resistor_config[i].flag == 0) {
        ch=sense_resistor_config[i].ch;
        SOC_IF_ERROR_RETURN
            (ltc2974_check_page(unit, devno, ch));
        data = DEV_DAC_PARAMS(dev)[len].dac_max_hwval;
        dac=(uint16) L16_RANGE_UPPER(data, 10000);
        SOC_IF_ERROR_RETURN
            (ltc2974_write(unit, devno, PMBUS_CMD_VOUT_MAX, (void *)&dac, 2));
        dac=(uint16) L16_RANGE_UPPER(data, 7000);
        SOC_IF_ERROR_RETURN
            (ltc2974_wait_for_not_busy(unit, devno));
        SOC_IF_ERROR_RETURN
            (ltc2974_write(unit, devno, PMBUS_CMD_VOUT_OV_FAULT_LIMIT, 
                            (void *)&dac, 2));
        dac=(uint16) L16_RANGE_UPPER(data, 5000);
        SOC_IF_ERROR_RETURN
            (ltc2974_wait_for_not_busy(unit, devno));
        SOC_IF_ERROR_RETURN
            (ltc2974_write(unit, devno, PMBUS_CMD_VOUT_OV_WARN_LIMIT, 
                            (void *)&dac, 2));
        dac=(uint16) L16_RANGE_UPPER(data, 2000);
        SOC_IF_ERROR_RETURN
            (ltc2974_wait_for_not_busy(unit, devno));
        SOC_IF_ERROR_RETURN
            (ltc2974_write(unit, devno, PMBUS_CMD_VOUT_MARGIN_HIGH, 
                            (void *)&dac, 2));

        data=DEV_DAC_PARAMS(dev)[len].dac_min_hwval;
        SOC_IF_ERROR_RETURN
            (ltc2974_wait_for_not_busy(unit, devno));

        dac=(uint16) L16_RANGE_LOWER(data, 5000);
        SOC_IF_ERROR_RETURN
            (ltc2974_write(unit, devno, PMBUS_CMD_VOUT_UV_FAULT_LIMIT, 
                            (void *)&dac, 2));

        if((sal_strcmp("ltc2974", sense_resistor_config[i].devname) == 0) || 
                (sal_strcmp(I2C_LTM4676_43, sense_resistor_config[i].devname) == 0)){
            SOC_IF_ERROR_RETURN
                (ltc2974_write(unit, devno, PMBUS_CMD_POWER_GOOD_ON, 
                                (void *)&dac, 2));

            dac=(uint16) L16_RANGE_LOWER(data, 7000);
            SOC_IF_ERROR_RETURN
                (ltc2974_write(unit, devno, PMBUS_CMD_POWER_GOOD_OFF, 
                                (void *)&dac, 2));
        }
        dac=(uint16) L16_RANGE_LOWER(data, 2000);
        SOC_IF_ERROR_RETURN
            (ltc2974_wait_for_not_busy(unit, devno));
        SOC_IF_ERROR_RETURN
            (ltc2974_write(unit, devno, PMBUS_CMD_VOUT_UV_WARN_LIMIT, 
                            (void *)&dac, 2));

        dac=(uint16) L16_RANGE_LOWER(data, 1000);
        SOC_IF_ERROR_RETURN
            ( ltc2974_wait_for_not_busy(unit, devno));
        SOC_IF_ERROR_RETURN
            (ltc2974_write(unit, devno, PMBUS_CMD_VOUT_MARGIN_LOW, 
                            (void *)&dac, 2));

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT) \
           || defined(BCM_MONTEREY_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) \
           || defined(BCM_FIRELIGHT_SUPPORT)
        if ((SOC_CONTROL(unit)->board_type == 0x9D) ||
            (SOC_CONTROL(unit)->board_type == 0xA3) ||
            (SOC_CONTROL(unit)->board_type == 0x9F) ||
            (SOC_CONTROL(unit)->board_type == 0x9E) ||
            (SOC_CONTROL(unit)->board_type == 0xA4) ||
            (SOC_CONTROL(unit)->board_type == 0xA5) ||
            (SOC_CONTROL(unit)->board_type == 0xA6) ||
            (SOC_CONTROL(unit)->board_type == 0xAA) ||
            (SOC_CONTROL(unit)->board_type == 0xA7) ||
            (SOC_CONTROL(unit)->board_type == 0xB1) ||
            (SOC_CONTROL(unit)->board_type == 0xAD) ||
            (SOC_CONTROL(unit)->board_type == 0xAF) ||
            (SOC_CONTROL(unit)->board_type == 0xDE) ||
            (SOC_CONTROL(unit)->board_type == 0xAC) ||
            (SOC_CONTROL(unit)->board_type == 0xB0) ||
            (SOC_CONTROL(unit)->board_type == 0xB3) ||
            (SOC_CONTROL(unit)->board_type == 0xB6) ||
            (SOC_CONTROL(unit)->board_type == 0xBB) ||
            (SOC_CONTROL(unit)->board_type == 0xBC) ||
            (SOC_CONTROL(unit)->board_type == 0xBF) ||
            (SOC_CONTROL(unit)->board_type == 0xC0) ||
            (SOC_CONTROL(unit)->board_type == 0xC2)) {
            dac = sense_resistor_config[i].res_value;
        } else
#endif
        {
            rv=ltc2974_float_to_L11(sense_resistor_config[i].res_value, &dac);
        }
            SOC_IF_ERROR_RETURN
                (ltc2974_write(unit, devno,
                                PMBUS_CMD_IOUT_CAL_GAIN, (void *)&dac, 2));
            sal_udelay(500);

       /* Device Configured */
        sense_resistor_config[i].flag=1;

    }
    return rv;
}

/*
 * NOTE NOTE NOTE:
 * All tables (dac_calibrate_t) passed to the ioctl() have size > 1
 * and the index is always within this range.
 */
STATIC int
ltc2974_ioctl(int unit, int devno, int opcode,
              void* data, int len)
{
    int rv = SOC_E_NONE;
#ifdef COMPILER_HAS_DOUBLE
    double fval;
#else
    int fval;
#endif
    uint16 dac;
    uint32 datalen = 2;
    uint8 data8;
    unsigned short voltage;
    dac_calibrate_t* params = NULL;
    int i, ch;
    int max_vs_config = MAX_VS_CONFIG;
    i2c_device_t *dev = soc_i2c_device(unit, devno);
    /* Using mutex lock to ensure thread-safe for ioctl operations */
    sal_mutex_take(ioctl_lock, sal_mutex_FOREVER);

     /* length field is actually used as an index into the dac_params table*/
     switch (opcode) {
         case I2C_LTC_IOC_READ_VOUT:
             if ((rv=ltc2974_check_page(unit, devno, len)) < 0) {
                 cli_out("Error: failed to set page %d in LTC2974 device.\n", len);
                 break;
             }
             if ((rv=ltc2974_write(unit, devno,
                             PMBUS_CMD_CLEAR_FAULTS,(void *) &data8, 0)) < 0) {
                 cli_out("Error: Failed to clear the faults of LTC2974 device.\n");
                 break;
             }
             if ((rv=ltc2974_wait_for_not_busy(unit, devno)) < 0) {
                 cli_out("Error: LTC2974 device is busy.\n");
                 break;
             }

            /* Setting Fault response to zero to prevent shut-down of device */
             data8 =0x00;
             if ((rv=ltc2974_write(unit, devno,
                             PMBUS_CMD_VOUT_OV_FAULT_RES, &data8, 1)) < 0) {
                 cli_out("Error: failed to set OV fault response of LTC2974.\n");
                 break;
             }
             if ((rv=ltc2974_wait_for_not_busy(unit, devno)) < 0) {
                 cli_out("Error: LTC2974 device is busy.\n");
                 break;
             }

	     if ((SOC_CONTROL(unit)->board_type != 0x98) && (SOC_CONTROL(unit)->board_type != 0xAA) &&
                 (SOC_CONTROL(unit)->board_type != 0xAC) && (SOC_CONTROL(unit)->board_type != 0xDE) && 
                 (SOC_CONTROL(unit)->board_type != 0xB1) && (SOC_CONTROL(unit)->board_type != 0xAD) && 
                 (SOC_CONTROL(unit)->board_type != 0xAF) && (SOC_CONTROL(unit)->board_type != 0xB0) &&
                 (SOC_CONTROL(unit)->board_type != 0xB3) && (SOC_CONTROL(unit)->board_type != 0xB6) &&
                 (SOC_CONTROL(unit)->board_type != 0xBB) && (SOC_CONTROL(unit)->board_type != 0xBC) &&
                 (SOC_CONTROL(unit)->board_type != 0xBF) && (SOC_CONTROL(unit)->board_type != 0xC2) &&
                 (SOC_CONTROL(unit)->board_type != 0xC0)) {
                 /* Switching on LTC2974 device */
                 data8 =0x40;
                 if ((rv=ltc2974_write(unit, devno, PMBUS_CMD_OPERATION, &data8, 1)) < 0) {
                     cli_out("Error: failed to set operation register of LTC2974 device.\n");
                     break;
                 }
                 if ((rv=ltc2974_wait_for_not_busy(unit, devno)) < 0) {
                     cli_out("Error: LTC2974 Device is busy.\n");
                     break;
                 }
	     }
             data8 =0x0A;
#if  defined(BCM_METROLITE_SUPPORT) || defined(BCM_MONTEREY_SUPPORT) || defined(BCM_HELIX5_SUPPORT)\
     || defined(BCM_FIRELIGHT_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT)
             if ((SOC_CONTROL(unit)->board_type == 0x98)  || (SOC_CONTROL(unit)->board_type == 0xAA) ||
                  (SOC_CONTROL(unit)->board_type == 0xB0) || (SOC_CONTROL(unit)->board_type == 0xB3) ||
                  (SOC_CONTROL(unit)->board_type == 0xB6) || (SOC_CONTROL(unit)->board_type == 0xBB) ||
                  (SOC_CONTROL(unit)->board_type == 0xBC) || (SOC_CONTROL(unit)->board_type == 0xBF) ||
                  (SOC_CONTROL(unit)->board_type == 0xC2) || (SOC_CONTROL(unit)->board_type == 0xC0)) {
                  data8 =0x1E;
             }
#endif
             if ((rv=ltc2974_write(unit, devno, PMBUS_CMD_ON_OFF_CONFIG, &data8, 1)) < 0) {
                 cli_out("Error: failed to set CONFIG register of LTC2974 device.\n");
                 break;
             }
             if ((rv=ltc2974_wait_for_not_busy(unit, devno)) < 0) {
                 cli_out("Error: Device LTC2974 is busy.\n");
                 break;
             }
             datalen=2;
             if ((rv=ltc2974_read(unit,devno, PMBUS_CMD_READ_VOUT, (void *)&dac, &datalen)) < 0) {
                 cli_out("Error: Failed to read VOUT of LTC2974 Device.\n");
                 break;
             }
             fval=dac;
#ifdef COMPILER_HAS_DOUBLE
             fval=L16_TO_V(fval );
             *(double *)data=fval;
#else
             fval=L16_TO_UV(fval);
             *(int *)data=(fval);
#endif
             ((i2c_adc_t*)data)->nsamples = 1;
             ((i2c_adc_t*)data)->max = fval;
             ((i2c_adc_t*)data)->min = fval;
             ((i2c_adc_t*)data)->delta = 0;
             break;

         case I2C_LTC_IOC_READ_IOUT:
             if ((rv=ltc2974_check_page(unit, devno, len)) < 0) {
                 cli_out("Error: failed to set page %d in LTC2974 device.\n", len);
                 break;
             }
             rv=ltc2974_float_to_L11(LTC2974_RES_CONFIG, &dac);
#if defined(BCM_METROLITE_SUPPORT) 
             if (SOC_CONTROL(unit)->board_type == 0x98 ) {
                 rv=ltc2974_float_to_L11(sense_resistor_config[len].res_value, &dac);
             }
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)\
    || defined(BCM_FIRELIGHT_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT)
             if ((SOC_CONTROL(unit)->board_type == 0x9D) ||
                 (SOC_CONTROL(unit)->board_type == 0xA3) ||
                 (SOC_CONTROL(unit)->board_type == 0x9F) ||
                 (SOC_CONTROL(unit)->board_type == 0x9E) ||
                 (SOC_CONTROL(unit)->board_type == 0xA4) ||
                 (SOC_CONTROL(unit)->board_type == 0xA5) ||
                 (SOC_CONTROL(unit)->board_type == 0xA6) ||
                 (SOC_CONTROL(unit)->board_type == 0xA7) ||
                 (SOC_CONTROL(unit)->board_type == 0xAA) ||
                 (SOC_CONTROL(unit)->board_type == 0xB1) ||
                 (SOC_CONTROL(unit)->board_type == 0xAD) ||
                 (SOC_CONTROL(unit)->board_type == 0xAF) ||
                 (SOC_CONTROL(unit)->board_type == 0xDE) ||
                 (SOC_CONTROL(unit)->board_type == 0xAC) ||
                 (SOC_CONTROL(unit)->board_type == 0xB0) ||
                 (SOC_CONTROL(unit)->board_type == 0xB3) ||
                 (SOC_CONTROL(unit)->board_type == 0xB6) ||
                 (SOC_CONTROL(unit)->board_type == 0xBB) ||
                 (SOC_CONTROL(unit)->board_type == 0xBC) ||
                 (SOC_CONTROL(unit)->board_type == 0xBF) ||
                 (SOC_CONTROL(unit)->board_type == 0xC2))
             {
                 i = len;
                 /* For HX5 boards since there are two 2974 devices, adjust the offset
                    based on the address */
                 if ((SOC_CONTROL(unit)->board_type == 0xB0) || (SOC_CONTROL(unit)->board_type == 0xB3) ||
                     (SOC_CONTROL(unit)->board_type == 0xB6) || (SOC_CONTROL(unit)->board_type == 0xBB) ||
                     (SOC_CONTROL(unit)->board_type == 0xBC) || (SOC_CONTROL(unit)->board_type == 0xBF) ||
                     (SOC_CONTROL(unit)->board_type == 0xC2)) {
                     i = ((dev->saddr == I2C_LTC2974_SADDR63) ? len: (len + 4));
                 }
                 dac = sense_resistor_config[i].res_value;
             }
#endif
             if ((rv=ltc2974_write(unit, devno,
                             PMBUS_CMD_IOUT_CAL_GAIN, (void *)&dac, 2)) < 0) {
                 cli_out("Error: failed to set IOUT_CAL_GAIN of LTC2974 device.\n");
                 break;
             }
             sal_udelay(500);
             if ((rv=ltc2974_wait_for_not_busy(unit, devno)) < 0) {
                 cli_out("Error: LTC2974 Device is busy.\n");
                 break;
             }
             if ((rv=ltc2974_read(unit, devno,PMBUS_CMD_READ_IOUT, (void *)&dac, &datalen)) < 0) {
                 cli_out("Error:Failed to read current in LTC2974 Device.\n");
                 break;
             }
             rv=ltc2974_L11_to_float(dac, &fval);
#ifdef COMPILER_HAS_DOUBLE
             *(double *)data=fval*1000;
             ((i2c_adc_t*)data)->max = fval*1000;
             ((i2c_adc_t*)data)->min = fval*1000;
#else
             *(int *)data=fval;
             ((i2c_adc_t*)data)->max = fval;
             ((i2c_adc_t*)data)->min = fval;

#endif
             ((i2c_adc_t*)data)->nsamples = 1;
             ((i2c_adc_t*)data)->delta = 0;

             break;

         case I2C_LTC_IOC_SET_RCONFIG:
             /* For future use in different RCONFIGs */
             /* Initialising sense resistor */
             i = len;

             /* For FB6 no need to reprograme this again
              * these registes are progmarmmed as part of LTC2974 image
              * its gets programmed before SVK powers on */

             if(SOC_CONTROL(unit)->board_type == 0xC0) {
                 return SOC_E_NONE;
             }

             /* 
              * ltc2974_board_resistor_match() as the general matching process
              * uses this offset index to retreive the board 
	      * based sense_resistor_configuration.
	      */
	     if (ltc2974_board_resistor_match(i) != SOC_E_NOT_FOUND) {
		     if ((i != 14) && (i != 15) && (i != 18) &&
                         (i != 22) && (i != 23) && (i != 24) && (i != 25)) {
			     LOG_CLI((BSL_META_U(unit, "Error: Invalid chip: %d for "
	                                         "LTC2974 device %s .\n"),
					         i, soc_i2c_devname(unit, devno)));
			     rv=SOC_E_NOT_FOUND;
			     break;
		     }
	     }
	     break;

         case I2C_LTC_IOC_READ_POUT:
             datalen=2;
             if ((rv=ltc2974_check_page(unit, devno, len)) < 0) {
                 cli_out("Error: failed to set page %d in LTC2974 device.\n", len);
                 break;
             }
             if ((rv=ltc2974_read(unit,devno, PMBUS_CMD_READ_POUT, (void *)&dac, &datalen)) < 0) {
                 cli_out("Error: failed to read power in LTC2974 device.\n");
                 break;
             }
             /* Switching off LTC2974  */
	     if ((SOC_CONTROL(unit)->board_type != 0x98) && (SOC_CONTROL(unit)->board_type != 0xAA) &&
                 (SOC_CONTROL(unit)->board_type != 0xAC) && (SOC_CONTROL(unit)->board_type != 0xDE) && 
                 (SOC_CONTROL(unit)->board_type != 0xB1) && (SOC_CONTROL(unit)->board_type != 0xAD) && 
                 (SOC_CONTROL(unit)->board_type != 0xAF) && (SOC_CONTROL(unit)->board_type != 0xB0) &&
                 (SOC_CONTROL(unit)->board_type != 0xB3) && (SOC_CONTROL(unit)->board_type != 0xB6) &&
                 (SOC_CONTROL(unit)->board_type != 0xBB) && (SOC_CONTROL(unit)->board_type != 0xBC) &&
                 (SOC_CONTROL(unit)->board_type != 0xBF) && (SOC_CONTROL(unit)->board_type != 0xC2) &&
                 (SOC_CONTROL(unit)->board_type != 0xC0)) {
                 data8 = 0x00;
                 if ((rv=ltc2974_write(unit, devno, PMBUS_CMD_OPERATION, &data8, 1)) < 0) {
                     cli_out("Error: failed to set operation register of LTC2974 device.\n");
                     break;
                 }
                 if ((rv=ltc2974_wait_for_not_busy(unit, devno)) < 0) {
                     cli_out("Error: LTC2974 Device is busy.\n");
                     break;
                 }
             }
             rv=ltc2974_L11_to_float(dac, &fval);
#ifdef COMPILER_HAS_DOUBLE
             *(double *)data=fval*1000;
             ((i2c_adc_t*)data)->max = fval*1000;
             ((i2c_adc_t*)data)->min = fval*1000;
#else
             *(int *)data=fval;
             ((i2c_adc_t*)data)->max = fval;
             ((i2c_adc_t*)data)->min = fval;
#endif
             ((i2c_adc_t*)data)->nsamples = 1;
             ((i2c_adc_t*)data)->delta = 0;
             break;

    /* Upload calibration table */
    case I2C_DAC_IOC_SET_CALTAB:
        params = (dac_calibrate_t*)data;
        /* dac_params  initialised */
        DEV_DAC_PARAMS(dev) = params;
        DEV_DAC_PARAM_LEN(dev) = len;
        break;

    case I2C_LTC_IOC_SET_CONFIG:
        /* Initialising sense resistor */
#ifdef COMPILER_HAS_DOUBLE
        i = *(double *)data;
#else
        i = *(int *)data;
#endif
        /* For FB6 no need to reprograme this again
         * these registes are progmarmmed as part of LTC2974 image
         * its gets programmed before SVK powers on */

        if(SOC_CONTROL(unit)->board_type == 0xC0) {
             return SOC_E_NONE;
        }
        /* 
         * ltc2974_board_resistor_match() as the general matching process
         * uses this offset index to retreive the board 
         * based sense_resistor_configuration.
         */
        if (ltc2974_board_resistor_match(i) != SOC_E_NOT_FOUND) {
            if ((i != 14) && (i != 15) && (i != 18) && (i != 22)) {
                LOG_CLI((BSL_META_U(unit, "Error: Invalid chip: %d for "
                        "LTC2974 device %s .\n"),
                        i, soc_i2c_devname(unit, devno)));
                rv=SOC_E_NOT_FOUND;
                break;
            }
        }
        if (DEV_DAC_PARAMS(dev)) {
            /* Setting max and min values for dac */
            rv= ltc2974_setmin_max(unit, devno, len);
        }
        break;

    case I2C_DAC_IOC_SET_VOUT:
        /* Set output voltage */
        if (DEV_DAC_PARAMS(dev)) {
#ifdef COMPILER_HAS_DOUBLE
            fval = *((double*)data); /* in V */
            voltage = V_TO_L16(fval);
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                        "unit %d i2c %s: LTC2974 ioctl I2C_DAC_IOC_SET_VOUT: "
                        "voltage = %d, len = %d\n"),
                        unit, soc_i2c_devname(unit,devno), voltage, len));
#else
            fval = *((int *)data); /* in uV */
            voltage = UV_TO_L16(fval);
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                        "unit %d i2c %s: LTC2974 ioctl I2C_DAC_IOC_SET_VOUT: "
                        "voltage = %d, len = %d\n"),
                        unit, soc_i2c_devname(unit,devno), voltage, len));
#endif
            if ((voltage > DEV_DAC_PARAMS(dev)[len].dac_max_hwval)  ||
                    (voltage < DEV_DAC_PARAMS(dev)[len].dac_min_hwval)) {
                LOG_ERROR(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                                "unit %d i2c %d: LTC2974 given voltage %d "
                                "beyond range for ch %d, %d, %d\n"),
                            unit, devno, voltage, len,
                            DEV_DAC_PARAMS(dev)[len].dac_max_hwval,
                            DEV_DAC_PARAMS(dev)[len].dac_min_hwval));
                rv=SOC_E_PARAM;
                break;
            }
            if ((rv=ltc2974_check_page(unit, devno, len)) < 0) {
                LOG_CLI((BSL_META_U(unit, "Error: failed to set page %d in "
                        "unit: %d LTC2974 device name: %s.\n"),
                        len, unit, soc_i2c_devname(unit, devno)));
            }
            rv = ltc2974_dac_set_ch_voltage(unit, devno, len, voltage);
        }
       break;

    case I2C_LTC_IOC_SET_VOUT:
        /* Finding and  setting page numbers */
        if (DEV_DAC_PARAMS(dev)) {

#ifdef BCM_METROLITE_SUPPORT
            if (SOC_CONTROL(unit)->board_type == 0x98) {
                max_vs_config = ML_MAX_VS_CONFIG;
            }
#endif
#ifdef BCM_MONTEREY_SUPPORT
            if (SOC_CONTROL(unit)->board_type == 0xAA) {
                max_vs_config = MONTEREY_MAX_VS_CONFIG;
            }
#endif

#if defined(BCM_TOMAHAWK2_SUPPORT)
            if ((SOC_CONTROL(unit)->board_type == 0x9D) ||
                (SOC_CONTROL(unit)->board_type == 0xA3) ||
                (SOC_CONTROL(unit)->board_type == 0x9F) ||
                (SOC_CONTROL(unit)->board_type == 0x9E)) {
                max_vs_config = TH2_MAX_VS_CONFIG;
            }
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
            if ((SOC_CONTROL(unit)->board_type == 0xA4) ||
                (SOC_CONTROL(unit)->board_type == 0xA5) ||
                (SOC_CONTROL(unit)->board_type == 0xA6) ||
                (SOC_CONTROL(unit)->board_type == 0xA7)) {
                max_vs_config = TD3_MAX_VS_CONFIG;
            }
#endif

#if defined(BCM_MAVERICK2_SUPPORT)
            if ((SOC_CONTROL(unit)->board_type == 0xA6) ||
                (SOC_CONTROL(unit)->board_type == 0xB5)) {
                max_vs_config = TD3_MAX_VS_CONFIG;
            }
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
            if ((SOC_CONTROL(unit)->board_type == 0xAC) || (SOC_CONTROL(unit)->board_type == 0xDE) ||
                (SOC_CONTROL(unit)->board_type == 0xB1) || (SOC_CONTROL(unit)->board_type == 0xAD) ||
                (SOC_CONTROL(unit)->board_type == 0xAF)) {
                max_vs_config = TH3_MAX_VS_CONFIG;
            }
#endif

#if defined(BCM_HELIX5_SUPPORT)
            if ((SOC_CONTROL(unit)->board_type == 0xB0) || (SOC_CONTROL(unit)->board_type == 0xB3) ||
                (SOC_CONTROL(unit)->board_type == 0xB6)) {
                max_vs_config = HX5_MAX_VS_CONFIG;
            }
#endif            


#if defined(BCM_HURRICANE4_SUPPORT)
            if ((SOC_CONTROL(unit)->board_type == 0xBB) ||
                (SOC_CONTROL(unit)->board_type == 0xBC) ||
                (SOC_CONTROL(unit)->board_type == 0xBF)) {
                max_vs_config = HR4_MAX_VS_CONFIG;
            }
#endif

#if defined(BCM_FIRELIGHT_SUPPORT)
            if (SOC_CONTROL(unit)->board_type == 0xC2) {
                max_vs_config = FL_MAX_VS_CONFIG;
            }
#endif
            for(i=0;i<max_vs_config;i++) {
                if(sal_strcmp(DEV_DAC_PARAMS(dev)[len].name, 
                            sense_resistor_config[i].function) == 0 ) {
                    break;
                }
            }
            if (i == max_vs_config) {
                LOG_CLI((BSL_META_U(unit, "Error: failed to find page number"
                        " for VDD_%s in sense resistor table.\n"), 
                        DEV_DAC_PARAMS(dev)[len].name));
                rv=SOC_E_NOT_FOUND;
                break;
            }
            ch = sense_resistor_config[i].ch;
            if ((rv=ltc2974_check_page(unit, devno, ch)) < 0) {
                LOG_CLI((BSL_META_U(unit, "Error: failed to set page %d in "
                        "LTC2974 device.\n"), ch));
                break;
            }
            /* Conversion of output voltage */
#ifdef COMPILER_HAS_DOUBLE
            fval = *((double*)data);
            voltage = (fval*8192);
#else
            fval = *((int*)data);
            /*2^12 conversion and changing from uVolt */
            voltage = (fval*8192)/1000000;
#endif
            if ((voltage < DEV_DAC_PARAMS(dev)[len].dac_min_hwval)||
                    (voltage > DEV_DAC_PARAMS(dev)[len].dac_max_hwval)) {
#ifdef  COMPILER_HAS_DOUBLE
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                          "unit %d i2c %d: LTC2974 given voltage %2.3f V "
                          "beyond range( max=%2.3f V, min=%2.3f V) for "
                          "voltage VDD_%s \n"), unit, devno, 
                          (double)voltage/8192, 
                          (double) DEV_DAC_PARAMS(dev)[len].dac_max_hwval/8192,
                          (double) DEV_DAC_PARAMS(dev)[len].dac_min_hwval/8192, 
                          DEV_DAC_PARAMS(dev)[len].name));
#else
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                          "unit %d i2c %d: LTC2974 given voltage %d mV "
                          "beyond range( max=%d mV, min=%d mV ) for " 
                          "voltage  VDD_%s \n"), unit, devno, 
                          voltage*1000/8192, 
                          DEV_DAC_PARAMS(dev)[len].dac_max_hwval*1000/8192,
                          DEV_DAC_PARAMS(dev)[len].dac_min_hwval*1000/8192,
                          DEV_DAC_PARAMS(dev)[len].name));

#endif
                rv = SOC_E_PARAM;
                break;
            }
            dac = voltage;
            /* Show what we are doing, for now ... */
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                        "unit %d i2c %s: LTC2974 ioctl "
                        "I2C_DAC_IOC_SET_VOUT : voltage = %d, len = %d\n"),
                        unit, soc_i2c_devname(unit,devno), voltage, len));
#ifdef BCM_METROLITE_SUPPORT
	    if (SOC_CONTROL(unit)->board_type == 0x98) {
                rv = ltc2974_dac_set_ch_voltage(unit, devno, len, voltage);
                /* Store voltage setting in NV memory to retain them after power cycling */
                rv = ltc2974_write(unit, devno, PMBUS_CMD_STORE_USER_ALL, (void *)&dac, 0);
                if ((rv=ltc2974_wait_for_not_busy(unit, devno)) < 0) {
                    cli_out("Error: LTC2974 device is busy.\n");
                    break;
		}
	    }else
#endif
	    {
	        rv = ltc2974_write(unit, devno, PMBUS_CMD_VOUT_COMMAND,(void *) &dac, 2);
            }
            /* Keep last value since DAC is write-only device */
            DEV_DAC_PARAMS(dev)[len].dac_last_val = dac;
        }
        break;

         default:
              LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                 "unit %d i2c %s: ltc2974_ioctl: invalid opcode (%d)\n"),
                 unit, soc_i2c_devname(unit,devno), opcode));
             break;
     }
    sal_mutex_give(ioctl_lock);
    return rv;
}

STATIC int
ltc2974_init(int unit, int devno, void* data, int len)
{
    int rv = SOC_E_NONE;
    i2c_device_t *dev = soc_i2c_device(unit, devno);
    char *devname;

    if (dev == NULL) {
        return SOC_E_INTERNAL;
    }
    devname = (char *)soc_i2c_devname(unit, devno);
    if (dev->priv_data == NULL) {
        dev->priv_data = sal_alloc(sizeof(device_data_t), devname);
        if (dev->priv_data == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "Fail to allocate private data fo dev %s\n"),
                        soc_i2c_devname(unit, devno)));
            return SOC_E_MEMORY;
        }
    }

    if (ioctl_lock == NULL) {
        ioctl_lock = sal_mutex_create("ltc2974_ioctl_lock");
        if (ioctl_lock == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "Fail to create ltc2974_ioctl_lock\n")));
            rv = SOC_E_MEMORY;
        }
    }

    /* Using mutex lock to ensure thread-safe for ioctl operations */
    sal_mutex_take(ioctl_lock, sal_mutex_FOREVER);
    /* bit3 1: control the LTC2974 output through VOUT command
     *      0: control the output via VID input pins which is controlled by
     *         a PCF8574 device
     */
    if ((rv=ltc2974_write(unit, devno,
                    PMBUS_CMD_CLEAR_FAULTS,(void *) &len, 0)) < 0) {
        cli_out("Error: Failed to clear the faults of LTC2974 device.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }
    if ((rv=ltc2974_wait_for_not_busy(unit, devno)) < 0) {
        cli_out("Error: LTC2974 Device is busy.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }
    soc_i2c_devdesc_set(unit, devno, "LTC2974 Voltage Control");
     LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "ltc2974_init: %s, devNo=0x%x\n"),
             soc_i2c_devname(unit,devno), devno));

    sal_mutex_give(ioctl_lock);
    return rv;
}

/* ltc2974 voltage control Chip Driver callout */
i2c_driver_t _soc_i2c_ltc2974_driver = {
    0x0, 0x0, /* System assigned bytes */
    LTC2974_DEVICE_TYPE,
    ltc2974_read,
    ltc2974_write,
    ltc2974_ioctl,
    ltc2974_init,
    NULL,
};

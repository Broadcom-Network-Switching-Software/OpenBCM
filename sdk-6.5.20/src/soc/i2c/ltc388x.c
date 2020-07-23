/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * I2C Device Driver for LTC388X an integrated power control IC.
 *
 * For new chip with Resistor configurations add new resistor
 * configuration array [ for ex sense_resistor_config_56960k
 * for Tomahawk chip] and initalise with the appropriate index
 * (chip number-G42) in IOCTL opcodes I2C_LTC_IOC_SET_CONFIG and
 * I2C_LTC_IOC_SET_RCONFIG.
*/

#include <sal/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>

typedef struct device_data_s {
    sal_mutex_t lock;   /* mutex lock for device operation */
    dac_calibrate_t *dac_params;  /*dac params for each device channel*/
    int dac_param_len;
} device_data_t;

#define DEV_MUTEX(dev) \
    (((device_data_t *)(((i2c_device_t *)(dev))->priv_data))->lock)
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

/* L16 format conversions */
#define L16_TO_V(val) ((val)*100000/4096/100000)
#define V_TO_L16(val) ((val)*4096)
#define L16_TO_UV(val) ((val)*100000/4096*10)
#define UV_TO_L16(val) ((val)/10*4096/100000)

/* The ranges are defined in unit of one part per 100000 */
#define FAULT_LIMIT_RANGE    10000
#define WARN_LIMIT_RANGE      7500
#define MARGIN_RANGE          5000
#define POWER_GOOD_ON_RANGE   7000
#define POWER_GOOD_OFF_RANGE  8000

#define MAX_VS_CONFIG         15
#define SB2_MAX_VS_CONFIG     6
#define AP_MAX_VS_CONFIG      4
#define FB5_MAX_VS_CONFIG     5
#define HR3_MAX_VS_CONFIG     6
#define GH2_MAX_VS_CONFIG     6
#define MN_MAX_VS_CONFIG      3  
#define WH2_MAX_VS_CONFIG     5
#define FB5_MAX_VS_CONFIG     5
#define HX5_MAX_VS_CONFIG     9


#define L16_RANGE_UPPER(L16, range) ((L16) + (int)(range)*L16/100000)
#define L16_RANGE_LOWER(L16, range)  ((L16) - (int)(range)*L16/100000)


#define POWER(exponent, input_val)  exponent < 0 ? \
                    (int)((int) (input_val) << (exponent*(-1))): \
                    (int)((int) (input_val*1000000) >> exponent)/1000000

static i2c_ltc_t* sense_resistor_config;

/* Sense Resistors Configuration Table for Tomahawk 56960k */
static i2c_ltc_t sense_resistor_config_56960k[MAX_VS_CONFIG]={
    { 0, "1.25",   "ltc3880",   I2C_LTC3880_CH0, LTC3880_1_25V_RES_CONFIG, 0 },
    { 1, "1.8",    "ltc3880",   I2C_LTC3880_CH1, LTC3880_1_8V_RES_CONFIG,  0 },
    { 2, "Analog", "ltc3882-1", I2C_BOTH_CH,     LTC3882_ANLG_RES_CONFIG,  0 },
    { 3, "3.3",    "ltc3882-0", I2C_BOTH_CH,     LTC3882_3_3V_RES_CONFIG,  0 },
    { 4, "Core",   I2C_LTC3882_55, I2C_BOTH_CH,     1,  0 },
    { 5, "ANA1",   I2C_LTC3880_41, I2C_BOTH_CH,     2,  0 },
    { 6, "ANA2",   I2C_LTC3880_42, I2C_LTC3880_CH0, 2,  0 },
    { 7, "3.3V",    I2C_LTC3880_42, I2C_LTC3880_CH1, 4,  0 },
    { 8, "DDR_15", I2C_LTC3880_43, I2C_BOTH_CH,     2, 0 },
    { 9, "SP1",    I2C_LTC3880_47, I2C_LTC3880_CH0, 8,  0 },
    { 10, "SP2",    I2C_LTC3880_47, I2C_LTC3880_CH1, 4,  0 },
    { 11, "DDR_Core",   I2C_LTC3880_45, I2C_LTC3880_CH0, 8,  0 },
    { 12, "SP3",    I2C_LTC3880_45, I2C_LTC3880_CH1, 4,  0 },
    { 13,  "Core",   I2C_LTC3882_66, I2C_BOTH_CH,     1,  0 },
    { 14, "Core",   I2C_LTC3882_77, I2C_BOTH_CH,     1,  0 }
};

static i2c_ltc_t sense_resistor_config_56260[SB2_MAX_VS_CONFIG]={
    { 0, "Core",   I2C_LTC3880_41, I2C_LTC3880_CH0, LTC3880_CORE_RES_CONFIG,  0 },
    { 1, "DDR",    I2C_LTC3880_41, I2C_LTC3880_CH1, LTC3880_DDR_RES_CONFIG,   0 },
    { 2, "IPROC_CORE", I2C_LTC3880_42, I2C_LTC3880_CH0, LTC3880_IPROC_CORE_RES_CONFIG,  0 },
    { 3, "Analog", I2C_LTC3880_42, I2C_LTC3880_CH1, LTC3880_ANLG_RES_CONFIG,  0 },
    { 4, "1.8",    I2C_LTM4676_43, I2C_LTC3880_CH1, LTC3880_1_8V_RES_CONFIG,  0 },
    { 5, "3.3",    I2C_LTM4676_43, I2C_LTC3880_CH0, LTC3880_3_3V_RES_CONFIG,  0 },
};
#ifdef BCM_APACHE_SUPPORT
/* Sense Resistors Configuration Table for Apache 56560K and Maverick 56760K*/
static i2c_ltc_t sense_resistor_config_56760[AP_MAX_VS_CONFIG]={
    { 0, "Core",     I2C_LTC3884_42, I2C_BOTH_CH,     1,  0 },
    { 1, "Core",     I2C_LTC3884_43, I2C_LTC3880_CH0, 1,  0 },
    { 2, "Analog",   I2C_LTC3884_43, I2C_LTC3880_CH1, 2,  0 },
    { 3, "3.3V",     I2C_LTC3880_45, I2C_BOTH_CH,     2,  0 },
};
/* Sense Resistors Configuration Table for FireBolt5 56565K */
static i2c_ltc_t sense_resistor_config_56565[FB5_MAX_VS_CONFIG]={
    { 0, "Core",     I2C_LTC3884_42, I2C_BOTH_CH,     1,  0 },
    { 1, "Core",     I2C_LTC3884_43, I2C_LTC3880_CH0, 1,  0 },
    { 2, "Analog",   I2C_LTC3884_43, I2C_LTC3880_CH1, 2,  0 },
    { 3, "1.0V_PHY", I2C_LTC3884_46, I2C_BOTH_CH,     1,  0 },
    { 4, "3.3V",     I2C_LTM4676_47, I2C_BOTH_CH,     0,  1 },
};
#endif

#ifdef BCM_MONTEREY_SUPPORT
/* Sense Resistors Configuration Table for MONTEREY 56670*/
static i2c_ltc_t sense_resistor_config_56670[MN_MAX_VS_CONFIG]={
    { 0, "Core",     I2C_LTC3884_42, I2C_BOTH_CH,     1,  0 },
    { 2, "Analog",   I2C_LTC3884_43, I2C_LTC3880_CH1, 2,  0 },
    { 3, "3.3V",     I2C_LTC3884_43, I2C_LTC3880_CH0,  1,  0 },
};
#endif 

#ifdef BCM_HELIX5_SUPPORT
/* Sense Resistors Configuration Table for FireBolt5 56565K */
static i2c_ltc_t sense_resistor_config_56370[HX5_MAX_VS_CONFIG]={
    { 0, "Core",     I2C_LTC3884_42, I2C_BOTH_CH,     1,  0 },
    { 1, "Core",     I2C_LTC3884_43, I2C_LTC3880_CH0, 1,  0 },
    { 2, "Analog",   I2C_LTC3884_43, I2C_LTC3880_CH1, 2,  0 },
};
#endif

static i2c_ltc_t sense_resistor_config_56160[HR3_MAX_VS_CONFIG]={
     {0, "3.3V", I2C_LTC3880_60, I2C_LTC3880_CH0, 2, 0},
     {1, "1.0V", I2C_LTC3880_60, I2C_LTC3880_CH1, 2, 0},
     {2, "1.8V", I2C_LTC3880_61, I2C_LTC3880_CH0, 2, 0},
     {3, "1.2V", I2C_LTC3880_61, I2C_LTC3880_CH1, 2, 0},
     {4, "Core", I2C_LTC3880_62, I2C_LTC3880_CH0, 2, 0},
     {5, "Analog", I2C_LTC3880_62, I2C_LTC3880_CH1, 2, 0},
};

static i2c_ltc_t sense_resistor_config_53570[GH2_MAX_VS_CONFIG]={
     {0, "3.3V", I2C_LTC3880_60, I2C_LTC3880_CH0, 2, 0},
     {1, "1.25V", I2C_LTC3880_60, I2C_LTC3880_CH1, 2, 0},
     {2, "1.8V", I2C_LTC3880_61, I2C_LTC3880_CH0, 2, 0},
     {3, "1.2V", I2C_LTC3880_61, I2C_LTC3880_CH1, 2, 0},
     {4, "Core", I2C_LTC3880_62, I2C_LTC3880_CH0, 2, 0},
     {5, "Analog", I2C_LTC3880_62, I2C_LTC3880_CH1, 2, 0},
};

static i2c_ltc_t sense_resistor_config_53570S[GH2_MAX_VS_CONFIG]={
     {0, "3.3V", I2C_LTC3880_60, I2C_LTC3880_CH0, 2, 0},
     {1, "1.25V", I2C_LTC3880_60, I2C_LTC3880_CH1, 2, 0},
     {2, "1.8V", I2C_LTC3880_61, I2C_LTC3880_CH0, 2, 0},
     {3, "1.5V", I2C_LTC3880_61, I2C_LTC3880_CH1, 2, 0},
     {4, "Core", I2C_LTC3880_62, I2C_LTC3880_CH0, 2, 0},
     {5, "Analog", I2C_LTC3880_62, I2C_LTC3880_CH1, 2, 0},
};

static i2c_ltc_t sense_resistor_config_53547[WH2_MAX_VS_CONFIG]={
     {0, "3.3V", I2C_LTC3880_60, I2C_LTC3880_CH0, 2, 0},
     {1, "1.8V", I2C_LTC3880_61, I2C_LTC3880_CH0, 2, 0},
     {2, "1.2V", I2C_LTC3880_61, I2C_LTC3880_CH1, 2, 0},
     {3, "Core", I2C_LTC3880_62, I2C_LTC3880_CH0, 2, 0},
     {4, "Analog", I2C_LTC3880_62, I2C_LTC3880_CH1, 2, 0},
};

static i2c_ltc_t sense_resistor_config_53549[WH2_MAX_VS_CONFIG]={
     {0, "3.3V", I2C_LTC3880_60, I2C_LTC3880_CH0, 2, 0},
     {1, "1.8V", I2C_LTC3880_61, I2C_LTC3880_CH0, 2, 0},
     {2, "1.5V", I2C_LTC3880_61, I2C_LTC3880_CH1, 2, 0},
     {3, "Core", I2C_LTC3880_62, I2C_LTC3880_CH0, 2, 0},
     {4, "Analog", I2C_LTC3880_62, I2C_LTC3880_CH1, 2, 0},
};

typedef struct _ltc_board_resistor_map_s {
    int offset;
    i2c_ltc_t* sense_resistor_config;
} _ltc_board_resistor_map_t;

/* 
 * This table is used to reflect the mapping between board type offset and 
 *  sens_register_config table. The board type offset is used since the IN 
 *  parameter on I2C_LTC_IOC_{SET_CONFIG,SET_RCONFIG,READ_IOUT} takes TH SVK 
 *  board(G42) as basis to identify different board.
 *
 * Information for the board type offset :
 *  1. This is the delta value of board_type index and TH is the index basis.
 *     [Examples] :
 *      - Tomahawk(board_type=G42), offset=0
 *      - HR3 (board_type=G52~G55, offset=10 (G42-G52)
 *  2. The board_type definition can be found in i2c_cmds.c
 */
static _ltc_board_resistor_map_t board_resistor_map[] = {
    {0, sense_resistor_config_56960k},  /* TH(G42) */
    {7, sense_resistor_config_56960k},  /* TH(G49) */
    {6, sense_resistor_config_56260},   /* SB2(G48) */
    {10, sense_resistor_config_56160},  /* HR3(G52) */
    {11, sense_resistor_config_56160},  /* HR3(G53) */
    {12, sense_resistor_config_56160},  /* HR3(G54) */
    {13, sense_resistor_config_56160},  /* HR3(G55) */
    {16, sense_resistor_config_53570},  /* GH2(G58) */
    {17, sense_resistor_config_53570S},  /* GH2(G59) */
    {20, sense_resistor_config_53547},  /* WH2(G62) */
    {21, sense_resistor_config_53549},  /* WH2(G63) */
};

/* 
 * This function is used to match the comming offset (delta_index) with a 
 * predefined sense_resistor_configuration table.
 */
STATIC int 
ltc388x_board_resistor_match(int unit, int in_offset)
{
    int i, map_size;

#ifdef BCM_APACHE_SUPPORT
    if ((SOC_CONTROL(unit)->board_type == 0x94) ||
        (SOC_CONTROL(unit)->board_type == 0x96)) {
        return SOC_E_NONE;
    }
#endif

#ifdef BCM_MONTEREY_SUPPORT
    if ((SOC_CONTROL(unit)->board_type == 0xAA)) { 
        return SOC_E_NONE;
    }
#endif

#ifdef BCM_HELIX5_SUPPORT
    if ((SOC_CONTROL(unit)->board_type == 0xB0) || (SOC_CONTROL(unit)->board_type == 0xB3) ||
        (SOC_CONTROL(unit)->board_type == 0xB6)) {
        return SOC_E_NONE;
    }
#endif

#ifdef BCM_FIREBOLT6_SUPPORT
    if ((SOC_CONTROL(unit)->board_type == 0xC0)) { 
        return SOC_E_NONE;
    }
#endif
    map_size = sizeof(board_resistor_map) / sizeof(_ltc_board_resistor_map_t);

    for (i = 0; i < map_size; i++) {
        if (board_resistor_map[i].offset == in_offset) {
            sense_resistor_config = board_resistor_map[i].sense_resistor_config;
            return SOC_E_NONE;
        }
    }
    return SOC_E_NOT_FOUND;
}

/*
 * Convert a floating point value into a
 * LinearFloat5_11 formatted word
 */

STATIC int
#ifdef  COMPILER_HAS_DOUBLE
ltc388x_float_to_L11(double input_val, uint16* data)
#else
ltc388x_float_to_L11(int input_val, uint16* data)
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
        mantissa= POWER(exponent, input_val);
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
ltc388x_L11_to_float(uint16 input_val, void *data)
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
                                 (exponent* (-1)))/1000000:
                      (double) (mantissa << exponent);
#else
    *(int *)data= exponent < 0 ?
                   (((mantissa)*1000000) >> (exponent*(-1))):
                   ((mantissa) << exponent)*100000;
#endif
    return SOC_E_NONE;
}

STATIC int
ltc388x_wait_for_not_busy(int unit, int devno)
{
    int rv = SOC_E_NONE;
    uint8 mfr_status, saddr;
    uint32 usec, wait_usec;
    i2c_device_t *dev = soc_i2c_device(unit, devno);

    DEV_CHECK_RETURN(dev);

    wait_usec = 0;
    usec = 10;

    saddr = soc_i2c_addr(unit, devno);

    while(wait_usec < 1000000) {
        SOC_IF_ERROR_RETURN(soc_i2c_read_byte_data(unit, saddr, 
                    PMBUS_CMD_MFR_COMMON, &mfr_status));
        dev->rbyte++;
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
         LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "unit %d i2c %s :"
                 "ltc388x is busy !\n"),
                 unit, soc_i2c_devname(unit, devno)));
        rv = SOC_E_TIMEOUT;
    }

    return rv;
}

STATIC int
ltc388x_read(int unit, int devno, uint16 addr, uint8* data, uint32* len)
{
    int rv = SOC_E_NONE;
    uint8 saddr;
    i2c_device_t *dev = soc_i2c_device(unit, devno);

    DEV_CHECK_RETURN(dev);

    saddr = soc_i2c_addr(unit, devno);

    if (*len == 0) {
        return SOC_E_NONE;
    }

    if (*len == 1) {
        /* reads a single byte from a device, from a designated register*/
        rv = soc_i2c_read_byte_data(unit, saddr, addr, data);
        dev->rbyte++;
        LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                    "unit %d i2c %s: LTC388X_read: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                    "rv = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *data, *len, rv));
    } else if (*len == 2) {
        /* reads a single word from a device, from a designated register*/
        rv = soc_i2c_read_word_data(unit, saddr, addr,(uint16 *)data);
        dev->rbyte +=2;
        LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                    "unit %d i2c %s: LTC388X_read: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                    "rv = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *(uint16 *)data, *len, rv));
    } else {
        /* not supported for now */
        LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                    "unit %d i2c %s: LTC388X_read fail: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *data, *len));
    }

    return rv;
}

STATIC int
ltc388x_write(int unit, int devno, uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    uint8 saddr;
    unsigned short val;
    i2c_device_t *dev = soc_i2c_device(unit, devno);

    DEV_CHECK_RETURN(dev);

    saddr = soc_i2c_addr(unit, devno);

    if (len == 0) {
        /* simply writes command code to device */
        LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit, "i2c %s: LTC388X write: "
                    "saddr = 0x%x, addr = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno), saddr, addr, len));
        rv = soc_i2c_write_byte(unit, saddr, addr);
    } else if (len == 1) {
        LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit, "i2c %s: LTC388X write: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno), saddr, addr, *data, len));
        rv = soc_i2c_write_byte_data(unit, saddr, addr,*data);
        dev->tbyte++;
    } else if (len == 2) {
        LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit, "i2c %s: LTC388X write: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno),
                    saddr, addr, *(uint16 *)data, len));
        val = *(unsigned short *)data;
        rv = soc_i2c_write_word_data(unit, saddr, addr, val);
        dev->tbyte += 2;
    }

    if (addr == PMBUS_CMD_VOUT_COMMAND) {
        /* Make sure the set voltage command is completed internally */
        rv = ltc388x_wait_for_not_busy(unit, devno);
        if (rv == SOC_E_NONE) {
            sal_usleep(120000);
        }
    }
    return rv;
}


STATIC int
ltc388x_check_page(int unit, int devno, int ch)
{
    int rv;
    uint8 page;
    uint32 len;

    len = sizeof(char);
    rv = ltc388x_read(unit, devno, PMBUS_CMD_PAGE, &page, &len);
    if (rv != SOC_E_NONE) {
        return rv;
    }

    if (page != ch) {
        page = ch;
        LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit, 
                    "LTC388X %d set page to %d\n"),
                    soc_i2c_addr(unit, devno), page));
        rv = ltc388x_write(unit, devno, PMBUS_CMD_PAGE, &page, sizeof(char));
    }

    return rv;
}



STATIC int
ltc388x_setmin_max(int unit, int devno, int len)
{
    int rv=SOC_E_NONE;
    int data,i,ch;
    uint16 dac;
    i2c_device_t *dev = soc_i2c_device(unit, devno);
    int max_vs_config = MAX_VS_CONFIG;

    DEV_PRIVDATA_CHECK_RETURN(dev);
#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
        max_vs_config = sizeof(sense_resistor_config_56260)/sizeof(i2c_ltc_t);
    }
#endif
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_CONTROL(unit)->board_type == 0x94) {
        max_vs_config = sizeof(sense_resistor_config_56760)/sizeof(i2c_ltc_t);
    } else if (SOC_CONTROL(unit)->board_type == 0x96) {
        max_vs_config = sizeof(sense_resistor_config_56565)/sizeof(i2c_ltc_t);
    }
#endif

#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_CONTROL(unit)->board_type == 0xAA) {
        max_vs_config = sizeof(sense_resistor_config_56670)/sizeof(i2c_ltc_t);
    } 
#endif

#if defined(BCM_HELIX5_SUPPORT)
    if ((SOC_CONTROL(unit)->board_type == 0xB0) || (SOC_CONTROL(unit)->board_type == 0xB3) ||
        (SOC_CONTROL(unit)->board_type == 0xB6)) {
        max_vs_config = sizeof(sense_resistor_config_56370)/sizeof(i2c_ltc_t);
    }
#endif 

    /* for HR3 boards */
    if ((SOC_CONTROL(unit)->board_type == 0x8F) || 
            (SOC_CONTROL(unit)->board_type == 0x90) || 
            (SOC_CONTROL(unit)->board_type == 0x91)) {
        max_vs_config = sizeof(sense_resistor_config_56160)/sizeof(i2c_ltc_t);
    }

    /* for GH2 boards */
    if ((SOC_CONTROL(unit)->board_type == 0xA0) ||
        (SOC_CONTROL(unit)->board_type == 0xA1)) {
        max_vs_config = sizeof(sense_resistor_config_56160)/sizeof(i2c_ltc_t);
    }

    /* for WH2 boards */
    if ((SOC_CONTROL(unit)->board_type == 0xA8) ||
        (SOC_CONTROL(unit)->board_type == 0xA9)) {
        max_vs_config = sizeof(sense_resistor_config_53547)/sizeof(i2c_ltc_t);
    }

    for(i=0;i<max_vs_config;i++) {
         if (sal_strcmp(DEV_DAC_PARAMS(dev)[len].name, 
                        sense_resistor_config[i].function) == 0) {
             break;
         }
    }
    if (i == max_vs_config) {
        LOG_CLI((BSL_META_U(unit, "Error: failed to find sense resistor "
                    "for VDD_%s in  LTC 3880 device.\n"), 
                    DEV_DAC_PARAMS(dev)[len].name));
         return SOC_E_NOT_FOUND;
    }
    if (sense_resistor_config[i].flag == 0) {
        ch=sense_resistor_config[i].ch;
        SOC_IF_ERROR_RETURN
            (ltc388x_check_page(unit, devno, ch));
        data = DEV_DAC_PARAMS(dev)[len].dac_max_hwval;
        dac=(uint16) L16_RANGE_UPPER(data, 10000);
        SOC_IF_ERROR_RETURN
            (ltc388x_write(unit, devno, PMBUS_CMD_VOUT_MAX, (void *)&dac, 2));
        dac=(uint16) L16_RANGE_UPPER(data, 7000);
        SOC_IF_ERROR_RETURN
            (ltc388x_wait_for_not_busy(unit, devno));
        SOC_IF_ERROR_RETURN
            (ltc388x_write(unit, devno, PMBUS_CMD_VOUT_OV_FAULT_LIMIT, 
                            (void *)&dac, 2));
        dac=(uint16) L16_RANGE_UPPER(data, 5000);
        SOC_IF_ERROR_RETURN
            (ltc388x_wait_for_not_busy(unit, devno));
        SOC_IF_ERROR_RETURN
            (ltc388x_write(unit, devno, PMBUS_CMD_VOUT_OV_WARN_LIMIT, 
                            (void *)&dac, 2));
        dac=(uint16) L16_RANGE_UPPER(data, 2000);
        SOC_IF_ERROR_RETURN
            (ltc388x_wait_for_not_busy(unit, devno));
        SOC_IF_ERROR_RETURN
            (ltc388x_write(unit, devno, PMBUS_CMD_VOUT_MARGIN_HIGH, 
                            (void *)&dac, 2));

        data=DEV_DAC_PARAMS(dev)[len].dac_min_hwval;
        SOC_IF_ERROR_RETURN
            (ltc388x_wait_for_not_busy(unit, devno));

        dac=(uint16) L16_RANGE_LOWER(data, 5000);
        SOC_IF_ERROR_RETURN
            (ltc388x_write(unit, devno, PMBUS_CMD_VOUT_UV_FAULT_LIMIT, 
                            (void *)&dac, 2));

        if((sal_strcmp("ltc3880", sense_resistor_config[i].devname) == 0) || 
                (sal_strcmp(I2C_LTM4676_43, sense_resistor_config[i].devname) == 0)){
            SOC_IF_ERROR_RETURN
                (ltc388x_write(unit, devno, PMBUS_CMD_POWER_GOOD_ON, 
                                (void *)&dac, 2));

            dac=(uint16) L16_RANGE_LOWER(data, 7000);
            SOC_IF_ERROR_RETURN
                (ltc388x_write(unit, devno, PMBUS_CMD_POWER_GOOD_OFF, 
                                (void *)&dac, 2));
        }
        dac=(uint16) L16_RANGE_LOWER(data, 2000);
        SOC_IF_ERROR_RETURN
            (ltc388x_wait_for_not_busy(unit, devno));
        SOC_IF_ERROR_RETURN
            (ltc388x_write(unit, devno, PMBUS_CMD_VOUT_UV_WARN_LIMIT, 
                            (void *)&dac, 2));

        dac=(uint16) L16_RANGE_LOWER(data, 1000);
        SOC_IF_ERROR_RETURN
            ( ltc388x_wait_for_not_busy(unit, devno));
        SOC_IF_ERROR_RETURN
            (ltc388x_write(unit, devno, PMBUS_CMD_VOUT_MARGIN_LOW, 
                            (void *)&dac, 2));

#if defined(BCM_SABER2_SUPPORT)
        if(!SOC_IS_SABER2(unit)) {
#endif
            rv=ltc388x_float_to_L11(sense_resistor_config[i].res_value, &dac);
            SOC_IF_ERROR_RETURN
                (ltc388x_write(unit, devno,
                                PMBUS_CMD_IOUT_CAL_GAIN, (void *)&dac, 2));
            sal_udelay(500);
#if defined(BCM_SABER2_SUPPORT)
        }
#endif

       /* Device Configured */
        sense_resistor_config[i].flag=1;

    }
    return rv;
}

STATIC int
ltc388x_get_ch_voltage(int unit, int devno, int ch, unsigned short *voltage)
{
    int rv;
    uint32 len;

    SOC_IF_ERROR_RETURN(ltc388x_check_page(unit, devno, ch));

    len = sizeof(short);
    rv = ltc388x_read(unit, devno, PMBUS_CMD_READ_VOUT, (uint8*)voltage,
                        &len);

    return rv;
}

STATIC int
ltc388x_set_ch_voltage_upper_with_supervision(int unit, int devno, int ch,
                                                        unsigned short voltage)
{
    unsigned short val;

    val = L16_RANGE_UPPER(voltage, FAULT_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_OV_FAULT_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_UPPER(voltage, WARN_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_OV_WARN_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_UPPER(voltage, MARGIN_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_MARGIN_HIGH,
                                        (uint8*)&val, sizeof(short)));


    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno, PMBUS_CMD_VOUT_COMMAND,
                                        (uint8*)&voltage, sizeof(short)));

    val = L16_RANGE_LOWER(voltage, MARGIN_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_MARGIN_LOW,
                                        (uint8*)&val, sizeof(short)));


    val = L16_RANGE_LOWER(voltage, WARN_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_UV_WARN_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_LOWER(voltage, FAULT_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_UV_FAULT_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    if (soc_i2c_devtype(unit, devno) != LTC3884_DEVICE_TYPE) {
        val = L16_RANGE_LOWER(voltage, POWER_GOOD_ON_RANGE);
        SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_POWER_GOOD_ON,
                                        (uint8*)&val, sizeof(short)));

        val = L16_RANGE_LOWER(voltage, POWER_GOOD_OFF_RANGE);
        SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_POWER_GOOD_OFF,
                                        (uint8*)&val, sizeof(short)));
    }

    return SOC_E_NONE;
}

STATIC int
ltc388x_set_ch_voltage_lower_with_supervision(int unit, int devno, int ch,
                                                        unsigned short voltage)
{
    unsigned short val;

    val = L16_RANGE_LOWER(voltage, FAULT_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_UV_FAULT_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_LOWER(voltage, WARN_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_UV_WARN_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_LOWER(voltage, MARGIN_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_MARGIN_LOW,
                                        (uint8*)&val, sizeof(short)));

    if (soc_i2c_devtype(unit, devno) != LTC3884_DEVICE_TYPE) {
        val = L16_RANGE_LOWER(voltage, POWER_GOOD_ON_RANGE);
        SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_POWER_GOOD_ON,
                                        (uint8*)&val, sizeof(short)));

        val = L16_RANGE_LOWER(voltage, POWER_GOOD_OFF_RANGE);
        SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_POWER_GOOD_OFF,
                                        (uint8*)&val, sizeof(short)));

    }

    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno, PMBUS_CMD_VOUT_COMMAND,
                                        (uint8*)&voltage, sizeof(short)));


    val = L16_RANGE_UPPER(voltage, MARGIN_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_MARGIN_HIGH,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_UPPER(voltage, WARN_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_OV_WARN_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_UPPER(voltage, FAULT_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_OV_FAULT_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    return SOC_E_NONE;
}

STATIC int
ltc388x_set_ch_voltage(int unit, int devno, int ch, unsigned short voltage)
{
    int rv = SOC_E_NONE;
    unsigned short old_vout;

    SOC_IF_ERROR_RETURN(ltc388x_get_ch_voltage(unit, devno, ch, &old_vout));

    if (voltage >= old_vout) {
        SOC_IF_ERROR_RETURN(ltc388x_set_ch_voltage_upper_with_supervision(unit,
                                                    devno, ch, voltage));
    } else {
        SOC_IF_ERROR_RETURN(ltc388x_set_ch_voltage_lower_with_supervision(unit,
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
ltc388x_dac_set_ch_voltage(int unit, int devno, int ch, unsigned short voltage)
{
    int rv;
    i2c_device_t *dev = soc_i2c_device(unit, devno);

    DEV_PRIVDATA_CHECK_RETURN(dev);

    rv = ltc388x_set_ch_voltage(unit, devno, ch, voltage);

#if defined (BCM_APACHE_SUPPORT) || defined (BCM_HELIX5_SUPPORT) || defined (BCM_FIREBOLT6_SUPPORT)
    if ((SOC_CONTROL(unit)->board_type == 0x94) ||
        (SOC_CONTROL(unit)->board_type == 0x96) ||
        (SOC_CONTROL(unit)->board_type == 0xB0) || 
        (SOC_CONTROL(unit)->board_type == 0xB3) ||
        (SOC_CONTROL(unit)->board_type == 0xB6) ||
        (SOC_CONTROL(unit)->board_type == 0xC0)) {
        /* For Apache/Maverick/FB5 Core voltage is on 3 phase so old voltage
         * needs to be saved only for the device connected on first two phases
         * which is done in ioctl()
         */
       return rv;
	}
#endif /* #if defined (BCM_APACHE_SUPPORT) || defined (BCM_HELIX5_SUPPORT) */

    if (SOC_SUCCESS(rv)) {
        /* Keep last value since DAC is write-only device */
        DEV_DAC_PARAMS(dev)[ch].dac_last_val = voltage;
    }

    return rv;
}

STATIC int
ltc388x_read_current(int unit, int devno, void* data, int ch)
{
    int rv = SOC_E_NONE;
#ifdef  COMPILER_HAS_DOUBLE
    double fval,fval1,fval0;
#else
    int fval,fval1,fval0;
#endif
    uint16 dac,dac0,dac1;
    uint32 datalen;
    soc_i2c_bus_t *i2cbus;
    int i;
    int max_vs_config = MAX_VS_CONFIG;

#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
        max_vs_config = sizeof(sense_resistor_config_56260)/sizeof(i2c_ltc_t);
    }
#endif
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_CONTROL(unit)->board_type == 0x94) {
        max_vs_config = sizeof(sense_resistor_config_56760)/sizeof(i2c_ltc_t);
    } else if (SOC_CONTROL(unit)->board_type == 0x96) {
        max_vs_config = sizeof(sense_resistor_config_56565)/sizeof(i2c_ltc_t);
    }
#endif

#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_CONTROL(unit)->board_type == 0xAA) {
        max_vs_config = sizeof(sense_resistor_config_56670)/sizeof(i2c_ltc_t);
    }
#endif

#if defined(BCM_HELIX5_SUPPORT)
    if ((SOC_CONTROL(unit)->board_type == 0xB0) || (SOC_CONTROL(unit)->board_type == 0xB3) ||
        (SOC_CONTROL(unit)->board_type == 0xB6)) {
        max_vs_config = sizeof(sense_resistor_config_56370)/sizeof(i2c_ltc_t);
	}
#endif

    /* for HR3 boards */
    if ((SOC_CONTROL(unit)->board_type == 0x8F) || 
            (SOC_CONTROL(unit)->board_type == 0x90) || 
            (SOC_CONTROL(unit)->board_type == 0x91)) {
        max_vs_config = sizeof(sense_resistor_config_56160)/sizeof(i2c_ltc_t);
    }

    /* for GH2 boards */
    if ((SOC_CONTROL(unit)->board_type == 0xA0) ||
        (SOC_CONTROL(unit)->board_type == 0xA1)) {
        max_vs_config = sizeof(sense_resistor_config_56160)/sizeof(i2c_ltc_t);
    }

    /* for WH2 boards */
    if ((SOC_CONTROL(unit)->board_type == 0xA8) ||
        (SOC_CONTROL(unit)->board_type == 0xA9)) {
        max_vs_config = sizeof(sense_resistor_config_53547)/sizeof(i2c_ltc_t);
    }

    i2cbus = I2CBUS(unit);
    for( i=0; i < max_vs_config; i++ ) {
#if defined(BCM_FIREBOLT6_SUPPORT)
        if(SOC_IS_FIREBOLT6(unit)) {
            continue;
        }
#endif
        if (sal_strcmp(i2cbus->devs[devno]->devname,
                            sense_resistor_config[i].devname) == 0) {
             if (sense_resistor_config[i].ch == ch ) {
                 if (sense_resistor_config[i].flag == 0) {
                     if ((rv = ltc388x_check_page(unit, devno, ch)) < 0) {
                         LOG_CLI((BSL_META_U(unit, "Error: failed to set page"
                                " %d in unit:%d LTC388X device name: %s.\n"),
                                ch, unit, soc_i2c_devname(unit, devno)));
                         return rv;
                     }

#if defined(BCM_SABER2_SUPPORT)
                    if(!SOC_IS_SABER2(unit)) {
#endif
                         rv = ltc388x_float_to_L11(
                                sense_resistor_config[i].res_value, &dac);
                         if ((rv = ltc388x_write(unit, devno,
                                PMBUS_CMD_IOUT_CAL_GAIN, (void *)&dac, 2)) < 0) {
                             LOG_CLI((BSL_META_U(unit, "Error: failed to set "
                                     "sense resistor in unit:%d"
                                     " LTC388X device name: %s.\n"),
                                     unit, soc_i2c_devname(unit, devno)));
                             return rv;
                         }
                         sal_udelay(500);
#if defined(BCM_SABER2_SUPPORT)
                    }
#endif
                 }
                 break;
             }
        }
    }
#if defined(BCM_FIREBOLT6_SUPPORT)
    if(!SOC_IS_FIREBOLT6(unit)) {
#endif
        if (i == max_vs_config) {
            LOG_CLI((BSL_META_U(unit, "Error: failed to find sense resistor value "
                            "for %s device .\n"), soc_i2c_devname(unit, devno)));
            return SOC_E_NOT_FOUND;
        }
#if defined(BCM_FIREBOLT6_SUPPORT)
    }
#endif

    if( ch == 0xFF ) {
        /* LTC3882/LTC3884 in dual mode hence current at both channels
         * are added up to measure total output current.......*/
        ch = 0x00;
        datalen = 2;
        if ((rv = ltc388x_check_page(unit, devno, ch)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: failed to set page %d in "
                    "unit: %d LTC388X device name: %s.\n"),
                    ch, unit, soc_i2c_devname(unit, devno)));
            return rv;
        }
        if ((rv = ltc388x_read(unit, devno, PMBUS_CMD_READ_IOUT, 
                    (void *)&dac0, &datalen)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: failed to read current in "
                    "LTC3882 device.\n")));
        }
        ch = 0x01;
        if ((rv = ltc388x_check_page(unit, devno, ch)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: failed to set page %d in "
                    "unit: %d LTC388X device name: %s.\n"),
                    ch, unit, soc_i2c_devname(unit, devno)));
            return rv;
        }
        if ((rv = ltc388x_read(unit, devno, PMBUS_CMD_READ_IOUT, 
                    (void *)&dac1, &datalen)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: failed to read current in "
                    "unit: %d LTC3882 device name: %s.\n"),
                    unit, soc_i2c_devname(unit, devno)));
            return rv;
        }
        rv = ltc388x_L11_to_float(dac1, &fval1);
        rv = ltc388x_L11_to_float(dac0, &fval0);
        fval = fval0 + fval1;
    } else {
        if ((rv = ltc388x_check_page(unit, devno, ch)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: failed to set page %d in "
                    "unit: %d LTC388X device name: %s.\n"),
                    ch, unit, soc_i2c_devname(unit, devno)));
            return rv;
        }
        datalen = 2;
        if ((rv = ltc388x_read(unit, devno,
                        PMBUS_CMD_READ_IOUT, (void *)&dac, &datalen)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: failed to read current in "
                    "unit: %d LTC3882 device name: %s.\n"),
                    unit, soc_i2c_devname(unit, devno)));
            return rv;
        }
        rv = ltc388x_L11_to_float(dac, &fval);
    }
#ifdef COMPILER_HAS_DOUBLE
    *(double *)data = fval * 1000;
#else
    *(int *)data = fval;
#endif
    return rv;
}

STATIC int
ltc388x_read_power(int unit, int devno, void* data, int ch)
{
    int rv = SOC_E_NONE;
#ifdef  COMPILER_HAS_DOUBLE
    double fval,fval1,fval0;
#else
    int fval,fval1,fval0;
#endif
    uint16 dac,dac0,dac1;
    uint32 datalen;
    if( ch == 0xFF) {
        /* LTC3882/LTC3884 in dual mode hence current at both channels
           are added up to measure totl output current...........*/
        ch = 0x00;
        datalen = 2;
        if ((rv = ltc388x_check_page(unit, devno, ch)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: failed to set page %d in "
                    "unit: %d LTC388X device name: %s.\n"),
                    ch, unit, soc_i2c_devname(unit, devno)));
            return rv;
        }
        if ((rv = ltc388x_read(unit, devno, PMBUS_CMD_READ_POUT, 
                    (void *)&dac0, &datalen)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: failed to read current in "
                    "LTC3882 device.\n")));
        }
        ch = 0x01;
        if ((rv = ltc388x_check_page(unit, devno, ch)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: failed to set page %d in "
                    "unit: %d LTC388X device name: %s.\n"),
                    ch, unit, soc_i2c_devname(unit, devno)));
            return rv;
        }
        if ((rv = ltc388x_read(unit, devno, PMBUS_CMD_READ_POUT, 
                    (void *)&dac1, &datalen)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: failed to read power in "
                    "unit: %d LTC3882 device name: %s.\n"),
                    unit, soc_i2c_devname(unit, devno)));
            return rv;
        }
        rv = ltc388x_L11_to_float(dac1,&fval1);
        rv = ltc388x_L11_to_float(dac0,&fval0);
        fval = fval0+fval1;
    } else {
        if ((rv = ltc388x_check_page(unit, devno, ch)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: failed to set page %d in "
                    "unit: %d LTC388X device name: %s.\n"),
                    ch, unit, soc_i2c_devname(unit, devno)));
            return rv;
        }
        datalen = 2;
        if ((rv = ltc388x_read(unit, devno,
                    PMBUS_CMD_READ_POUT, (void *)&dac, &datalen)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: failed to read power in "
                    "unit: %d LTC3882 device name: %s.\n"),
                    unit, soc_i2c_devname(unit, devno)));
            return rv;
        }
        rv = ltc388x_L11_to_float(dac,&fval);
    }
#ifdef COMPILER_HAS_DOUBLE
        *(double *)data = fval * 1000;
#else
        *(int *)data = fval;
#endif
        return rv;
}

/*
 * NOTE NOTE NOTE:
 * All tables (dac_calibrate_t) passed to the ioctl() have size > 1
 * and the index is always within this range.
 */
STATIC int
ltc388x_ioctl(int unit, int devno, int opcode, void* data, int len)
{
    int rv = SOC_E_NONE;
#ifdef COMPILER_HAS_DOUBLE
    double fval=0, fval1=0;
#else
    int fval=0, fval1=0;
#endif
    int uV,ch=0,i;
    int iter;
    uint16 dac,time_ms;
    unsigned short voltage;
    uint32 datalen;
    dac_calibrate_t* params = NULL;
#if defined BCM_APACHE_SUPPORT || defined BCM_HELIX5_SUPPORT || defined BCM_FIREBOLT6_SUPPORT
    int no_ch = 0;
#endif
    i2c_device_t *dev = soc_i2c_device(unit, devno);
    int max_vs_config = MAX_VS_CONFIG;

#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
        max_vs_config = sizeof(sense_resistor_config_56260)/sizeof(i2c_ltc_t);
    }
#endif
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_CONTROL(unit)->board_type == 0x94) {
        max_vs_config = sizeof(sense_resistor_config_56760)/sizeof(i2c_ltc_t);
    } else if (SOC_CONTROL(unit)->board_type == 0x96) {
        max_vs_config = sizeof(sense_resistor_config_56565)/sizeof(i2c_ltc_t);
    }
#endif
    
#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_CONTROL(unit)->board_type == 0xAA) {
        max_vs_config = sizeof(sense_resistor_config_56670)/sizeof(i2c_ltc_t);
   } 
#endif  
#if defined(BCM_HELIX5_SUPPORT)
    if ((SOC_CONTROL(unit)->board_type == 0xB0) || (SOC_CONTROL(unit)->board_type == 0xB3) ||
        (SOC_CONTROL(unit)->board_type == 0xB6)) {
        max_vs_config = sizeof(sense_resistor_config_56370)/sizeof(i2c_ltc_t);
   }
#endif
    /* for HR3 boards */
    if ((SOC_CONTROL(unit)->board_type == 0x8F) || 
            (SOC_CONTROL(unit)->board_type == 0x90) || 
            (SOC_CONTROL(unit)->board_type == 0x91)) {
        max_vs_config = sizeof(sense_resistor_config_56160)/sizeof(i2c_ltc_t);
    }

    /* for GH2 boards */
    if ((SOC_CONTROL(unit)->board_type == 0xA0) ||
        (SOC_CONTROL(unit)->board_type == 0xA1)) {
        max_vs_config = sizeof(sense_resistor_config_56160)/sizeof(i2c_ltc_t);
    }

    /* for WH2 boards */
    if ((SOC_CONTROL(unit)->board_type == 0xA8) ||
        (SOC_CONTROL(unit)->board_type == 0xA9)) {
        max_vs_config = sizeof(sense_resistor_config_53547)/sizeof(i2c_ltc_t);
    }

    DEV_PRIVDATA_CHECK_RETURN(dev);

    /* length field is actually used as an index into the dac_params table*/
    if( !data || ( (DEV_DAC_PARAMS(dev) != NULL) && 
        ((len > DEV_DAC_PARAM_LEN(dev)) && (len != I2C_BOTH_CH)))) {
        /* 
         * len for op = I2C_LTC_IOC_SET_RCONFIG is used as table offset 
         * instead of dac_param's length.
         */
        if (opcode != I2C_LTC_IOC_SET_RCONFIG) {
            return SOC_E_PARAM;
        }
    }

    /* Using mutex lock to ensure thread-safe for ioctl operations */
    sal_mutex_take(DEV_MUTEX(dev), sal_mutex_FOREVER);

    switch ( opcode ){
    case I2C_ADC_QUERY_CHANNEL:
        ch = len;
        rv = ltc388x_get_ch_voltage(unit, devno, ch, &voltage);
        fval = voltage;
#ifdef COMPILER_HAS_DOUBLE
        fval = L16_TO_V(fval);
#else
        fval = L16_TO_UV(fval);
#endif
        ((i2c_adc_t*)data)->max =
        ((i2c_adc_t*)data)->min =
        ((i2c_adc_t*)data)->val = fval;
        ((i2c_adc_t*)data)->delta = 0;
        ((i2c_adc_t*)data)->nsamples = 1;
        break;

    /* Upload calibration table */
    case I2C_DAC_IOC_SET_CALTAB:
        params = (dac_calibrate_t*)data;
        /* dac_params  initialised */
        DEV_DAC_PARAMS(dev) = params;
        DEV_DAC_PARAM_LEN(dev) = len;
        break;

    case I2C_DAC_IOC_SETDAC_MIN:
        /* Set MIN voltage */
        if (DEV_DAC_PARAMS(dev)) {
            rv = ltc388x_dac_set_ch_voltage(unit, devno, len,
                    DEV_DAC_PARAMS(dev)[len].dac_min_hwval);
        }
        break;

    case I2C_DAC_IOC_SETDAC_MAX:
        /* Set MAX voltage */
        if (DEV_DAC_PARAMS(dev)) {
            rv = ltc388x_dac_set_ch_voltage(unit, devno, len,
                    DEV_DAC_PARAMS(dev)[len].dac_max_hwval);
        }
        break;
    
    case I2C_DAC_IOC_SETDAC_MID:
        /* Set mid-range voltage */
        if (DEV_DAC_PARAMS(dev)) {
            rv = ltc388x_dac_set_ch_voltage(unit, devno, len,
                    DEV_DAC_PARAMS(dev)[len].dac_mid_hwval);
        }
        break;

    case I2C_DAC_IOC_CALIBRATE_MAX:
        /* Set MAX output value (from ADC) */
        if (DEV_DAC_PARAMS(dev)) {
#ifdef COMPILER_HAS_DOUBLE
            fval = *((double*)data);
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                            "CALIBRATE_MAX setting %f\n"), fval));
#else
            fval = *((int *)data);
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                            "CALIBRATE_MAX setting %d\n"), fval));
#endif
            DEV_DAC_PARAMS(dev)[len].max = fval;
        }
        break;

    case I2C_DAC_IOC_CALIBRATE_MIN:
        /* Set MIN output value (from ADC) */
        if (DEV_DAC_PARAMS(dev)) {
#ifdef COMPILER_HAS_DOUBLE
            fval = *((double*)data);
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                            "CALIBRATE_MIN setting %f\n"), fval));
#else
            fval = *((int *)data);
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                            "CALIBRATE_MIN setting %d\n"), fval));
#endif
            DEV_DAC_PARAMS(dev)[len].min = fval;
        }
        break;

    case I2C_DAC_IOC_CALIBRATE_STEP:
        /* Calibrate stepsize */
        if (DEV_DAC_PARAMS(dev)) {
            DEV_DAC_PARAMS(dev)[len].step =
                (DEV_DAC_PARAMS(dev)[len].use_max ? -1 : 1) *
                (DEV_DAC_PARAMS(dev)[len].max - DEV_DAC_PARAMS(dev)[len].min) /
                (DEV_DAC_PARAMS(dev)[len].dac_max_hwval - 
                    DEV_DAC_PARAMS(dev)[len].dac_min_hwval);
#ifdef COMPILER_HAS_DOUBLE
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                        "unit %d i2c %s: LTC388X calibration on function %s:"
                        "(max=%f,min=%f,step=%f)\n"),
                        unit, soc_i2c_devname(unit,devno),
                        DEV_DAC_PARAMS(dev)[len].name,
                        DEV_DAC_PARAMS(dev)[len].max,
                        DEV_DAC_PARAMS(dev)[len].min,
                        DEV_DAC_PARAMS(dev)[len].step));
#else
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                        "unit %d i2c %s: LTC388X calibration on function %s:"
                        "(max=%d,min=%d,step=%d)\n"),
                        unit, soc_i2c_devname(unit,devno),
                        DEV_DAC_PARAMS(dev)[len].name,
                        DEV_DAC_PARAMS(dev)[len].max,
                        DEV_DAC_PARAMS(dev)[len].min,
                        DEV_DAC_PARAMS(dev)[len].step));
#endif
        }
        break;
    case I2C_DAC_IOC_SET_VOUT:
        /* Set output voltage */
        if (DEV_DAC_PARAMS(dev)) {
#ifdef COMPILER_HAS_DOUBLE
            fval = *((double*)data); /* in V */
            voltage = V_TO_L16(fval);
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                        "unit %d i2c %s: LTC388x ioctl I2C_DAC_IOC_SET_VOUT: "
                        "voltage = %d, len = %d\n"),
                        unit, soc_i2c_devname(unit,devno), voltage, len));
#else
            fval = *((int *)data); /* in uV */
            voltage = UV_TO_L16(fval);
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                        "unit %d i2c %s: LTC388X ioctl I2C_DAC_IOC_SET_VOUT: "
                        "voltage = %d, len = %d\n"),
                        unit, soc_i2c_devname(unit,devno), voltage, len));
#endif
            if ((voltage > DEV_DAC_PARAMS(dev)[len].dac_max_hwval)  ||
                    (voltage < DEV_DAC_PARAMS(dev)[len].dac_min_hwval)) {
                LOG_ERROR(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                                "unit %d i2c %d: LTC388X given voltage %d "
                                "beyond range for ch %d, %d, %d\n"),
                            unit, devno, voltage, len,
                            DEV_DAC_PARAMS(dev)[len].dac_max_hwval,
                            DEV_DAC_PARAMS(dev)[len].dac_min_hwval));
                rv=SOC_E_PARAM;
                break;
            }
            if ((rv=ltc388x_check_page(unit, devno, len)) < 0) {
                LOG_CLI((BSL_META_U(unit, "Error: failed to set page %d in "
                        "unit: %d LTC388X device name: %s.\n"),
                        len, unit, soc_i2c_devname(unit, devno)));
            }
            rv = ltc388x_dac_set_ch_voltage(unit, devno, len, voltage);
        }
       break;

    case I2C_REGULATOR_IOC_VOLT_GET:
        rv = ltc388x_get_ch_voltage(unit, devno, len, &voltage);
        if (rv == SOC_E_NONE) {
            *((int *)data) = L16_TO_UV(voltage);
        }
        break;

    case I2C_REGULATOR_IOC_VOLT_SET:
        uV = *(int *)data;
        voltage = UV_TO_L16(uV);
        rv = ltc388x_set_ch_voltage(unit, devno, len, voltage);
        break;

    case I2C_LTC_IOC_SET_CONFIG:
        /* Initialising sense resistor */
#ifdef COMPILER_HAS_DOUBLE
        i = *(double *)data;
#else
        i = *(int *)data;
#endif
#ifdef BCM_APACHE_SUPPORT
        if (SOC_CONTROL(unit)->board_type == 0x94) {
            /* Initializing with Apache/Maverick chip resistor configuration */
            sense_resistor_config = sense_resistor_config_56760;
            rv = ltc388x_setmin_max(unit, devno, len);
            break;
        } else if (SOC_CONTROL(unit)->board_type == 0x96) {
            /* Initializing with FB5 chip resistor configuration */
            sense_resistor_config = sense_resistor_config_56565;
            rv = ltc388x_setmin_max(unit, devno, len);
            break;
        }
#endif

#ifdef BCM_MONTEREY_SUPPORT
        if (SOC_CONTROL(unit)->board_type == 0xAA) {
            /* Initializing with MONTEREY chip resistor configuration */
            sense_resistor_config = sense_resistor_config_56670;
            rv = ltc388x_setmin_max(unit, devno, len);
            break;
        }
#endif 

#ifdef BCM_FIREBOLT6_SUPPORT
        if (SOC_CONTROL(unit)->board_type == 0xC0) {
           return SOC_E_NONE;
        }
#endif

#ifdef BCM_HELIX5_SUPPORT
        if ((SOC_CONTROL(unit)->board_type == 0xB0) || (SOC_CONTROL(unit)->board_type == 0xB3) ||
            (SOC_CONTROL(unit)->board_type == 0xB6)) {
            /* Initializing with MONTEREY chip resistor configuration */
            sense_resistor_config = sense_resistor_config_56370;
            rv = ltc388x_setmin_max(unit, devno, len);
            break;
        }
#endif
        /* 
         * Initializing with Tomahawk chip resistor configuration
         *
         * "i" is retrieved from incoming parameter and it is referenced as 
         * board_type offset index. (offset from TH's board type) 
         * 
         * ltc388x_board_resistor_match() as the general matching process
         * uses this offset index to retreive the board 
         * based sense_resistor_configuration.
         */
        if (ltc388x_board_resistor_match(unit, i) == SOC_E_NOT_FOUND) {
            if (i != 4) {
                LOG_CLI((BSL_META_U(unit, "Error: Invalid chip: %d for "
                        "LTC388X device %s .\n"),
                        i, soc_i2c_devname(unit, devno)));
                rv=SOC_E_NOT_FOUND;
                break;
            }
        }
        if (DEV_DAC_PARAMS(dev)) {
            /* Setting max and min values for dac */
            rv= ltc388x_setmin_max(unit, devno, len);
        }
        break;
   case I2C_LTC_IOC_SET_RCONFIG:
        i = len;

        if(SOC_CONTROL(unit)->board_type == 0xC0) {
            return SOC_E_NONE;
        }
        /* 
         * Initializing with Tomahawk chip resistor configuration
         *
         * "i" is retrieved from incoming parameter and it is referenced as 
         * board_type offset index. (offset from TH's board type) 
         * 
         * ltc388x_board_resistor_match() as the general matching process
         * uses this offset index to retreive the board 
         * based sense_resistor_configuration.
         */
        if (ltc388x_board_resistor_match(unit, i) == SOC_E_NOT_FOUND) {
            if (i != 4) {
                LOG_CLI((BSL_META_U(unit, "Error: Invalid chip: %d for "
                        "LTC388X device %s .\n"),
                        i, soc_i2c_devname(unit, devno)));
                rv=SOC_E_NOT_FOUND;
                break;
            }
        }
        break;
   case I2C_LTC_IOC_NOMINAL:
        if ((rv=ltc388x_wait_for_not_busy(unit, devno)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: unit: %d  LTC388X device name"
                    " %s Device is busy.\n"), 
                    unit, soc_i2c_devname(unit, devno)));
            break;
        }
        ch=*((int *)data);
        if ((rv=ltc388x_check_page(unit, devno, ch)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: failed to set page %d in "
                        "unit: %d LTC388X device name: %s.\n"),
                        ch, unit, soc_i2c_devname(unit, devno)));
            break;
        }
        if (DEV_DAC_PARAMS(dev)) {
            dac= (uint16) DEV_DAC_PARAMS(dev)[len].dac_mid_hwval;
            rv = ltc388x_write(unit, devno, PMBUS_CMD_VOUT_COMMAND,(
                                    void *)&dac, 2);
            /* Keep last value since DAC is write-only device */
            DEV_DAC_PARAMS(dev)[len].dac_last_val = dac;
        }
        break;

    case I2C_LTC_IOC_READ_VOUT:
        ch = len;
        ((i2c_adc_t*)data)->max =
        ((i2c_adc_t*)data)->min =
        ((i2c_adc_t*)data)->val =
        ((i2c_adc_t*)data)->delta = 0;

#if defined (BCM_APACHE_SUPPORT) || defined (BCM_HELIX5_SUPPORT) || defined (BCM_FIREBOLT6_SUPPORT)
        if ((SOC_CONTROL(unit)->board_type == 0x94) ||
            (SOC_CONTROL(unit)->board_type == 0x96) ||
            (SOC_CONTROL(unit)->board_type == 0xB0) ||
            (SOC_CONTROL(unit)->board_type == 0xB3) ||
            (SOC_CONTROL(unit)->board_type == 0xB6) ||
            (SOC_CONTROL(unit)->board_type == 0xC0)) {
            ((i2c_adc_t*)data)->nsamples = 100;
        } else
#endif /* #if defined (BCM_APACHE_SUPPORT) || defined (BCM_HELIX5_SUPPORT) */
        {
            ((i2c_adc_t*)data)->nsamples = 1;
        }

        /* Taking Samples */
        for (iter = 0; iter < ((i2c_adc_t*)data)->nsamples; iter++) {
            if ((rv = ltc388x_check_page(unit, devno, ch)) < 0) {
                LOG_CLI((BSL_META_U(unit, "Error: failed to set page %d in "
                        "unit: %d LTC388X Device name: %s.\n"),
                        ch, unit, soc_i2c_devname(unit, devno)));
                break;
            }
            datalen = 2;
            if ((rv = ltc388x_read(unit, devno, PMBUS_CMD_READ_VOUT,
                                   (void *)&voltage, &datalen)) < 0) {
                LOG_CLI((BSL_META_U(unit, "Error: Failed to read voltage in "
                         "unit:%d LTC388x Device name:%s.\n"),
                         unit, soc_i2c_devname(unit, devno)));
                break;
            }
            fval = voltage;
#ifdef COMPILER_HAS_DOUBLE
            fval = L16_TO_V(fval);
#else
            fval = L16_TO_UV(fval);
#endif
            if (((i2c_adc_t*)data)->min == 0) {
                ((i2c_adc_t*)data)->min = fval;
            }
            if (fval > ((i2c_adc_t*)data)->max) {
                ((i2c_adc_t*)data)->max = fval;
            } else if (fval < ((i2c_adc_t*)data)->min) {
                ((i2c_adc_t*)data)->min = fval;
            }
        }
        ((i2c_adc_t*)data)->val =
            (((i2c_adc_t*)data)->max + ((i2c_adc_t*)data)->min) / 2;
        ((i2c_adc_t*)data)->delta =
            (((i2c_adc_t*)data)->max - ((i2c_adc_t*)data)->min);
        break;

    case I2C_LTC_IOC_READ_IOUT:
#ifdef COMPILER_HAS_DOUBLE
        i = *(double *)data;
#else
        i = *(int *)data;
#endif
#if defined BCM_APACHE_SUPPORT || defined BCM_HELIX5_SUPPORT || defined BCM_FIREBOLT6_SUPPORT
        ((i2c_adc_t*)data)->max =
        ((i2c_adc_t*)data)->min =
        ((i2c_adc_t*)data)->val =
        ((i2c_adc_t*)data)->delta = 0;
        /* For Apache/Maverick/FB5/HX5 Three phase support */
        if ((SOC_CONTROL(unit)->board_type == 0x94) ||
            (SOC_CONTROL(unit)->board_type == 0x96) || 
            (SOC_CONTROL(unit)->board_type == 0xAA) ||
            (SOC_CONTROL(unit)->board_type == 0xB0) ||
            (SOC_CONTROL(unit)->board_type == 0xB3) ||
            (SOC_CONTROL(unit)->board_type == 0xB6) ||
            (SOC_CONTROL(unit)->board_type == 0xC0)) {
#if defined BCM_APACHE_SUPPORT
            if (SOC_CONTROL(unit)->board_type == 0x94) {
                /* Initializing with Apache/Maverick sense resistor configuration */
                sense_resistor_config = sense_resistor_config_56760;
            } else if (SOC_CONTROL(unit)->board_type == 0x96) {
                /* Initializing with FB5 sense resistor configuration */
                sense_resistor_config = sense_resistor_config_56565;
            }
#endif
#ifdef BCM_MONTEREY_SUPPORT
           if (SOC_CONTROL(unit)->board_type == 0xAA) {
            /* Initializing with MONTEREY chip resistor configuration */
            sense_resistor_config = sense_resistor_config_56670;
            }
#endif 
#ifdef BCM_HELIX5_SUPPORT
           if ((SOC_CONTROL(unit)->board_type == 0xB0) ||
               (SOC_CONTROL(unit)->board_type == 0xB3) ||
               (SOC_CONTROL(unit)->board_type == 0xB6)) {
                /* Initializing with MONTEREY chip resistor configuration */
                sense_resistor_config = sense_resistor_config_56370;
            }
#endif
            /* Taking 100 Samples */
            for (iter = 0; iter < 100; iter++) {
                if (soc_i2c_addr(unit, devno) == I2C_LTC3884_SADDR42) {
                    if (SOC_CONTROL(unit)->board_type == 0xC0) {
                       /* For Firebolt6 4 phase support */
                       no_ch = I2C_BOTH_CH;
                    } else if(SOC_CONTROL(unit)->board_type != 0xAA) {
                       no_ch = I2C_LTC3880_CH0;
                    }
                    SOC_IF_ERROR_RETURN
                        (ltc388x_read_current(unit, devno, &fval, len));
                    rv = ltc388x_read_current(unit, devno + 1, &fval1, no_ch);
                    fval += fval1;
                } else {
                    rv = ltc388x_read_current(unit, devno, &fval, len);
                }
                if (((i2c_adc_t*)data)->min == 0) {
                    ((i2c_adc_t*)data)->min = fval;
                }
                if (fval > ((i2c_adc_t*)data)->max) {
                    ((i2c_adc_t*)data)->max = fval;
                } else if (fval < ((i2c_adc_t*)data)->min) {
                    ((i2c_adc_t*)data)->min = fval;
                }
            }
            ((i2c_adc_t*)data)->val =
                (((i2c_adc_t*)data)->max + ((i2c_adc_t*)data)->min) / 2;
            ((i2c_adc_t*)data)->delta =
                (((i2c_adc_t*)data)->max - ((i2c_adc_t*)data)->min);
            ((i2c_adc_t*)data)->nsamples = 100;
            break;
        }
#endif /* #if defined (BCM_APACHE_SUPPORT) || defined (BCM_HELIX5_SUPPORT) */

        if (ltc388x_board_resistor_match(unit, i) == SOC_E_NOT_FOUND) {
            if (i != 4) {
                LOG_CLI((BSL_META_U(unit, "Error: Invalid chip: %d for "
                        "LTC388X device %s .\n"),
                        i, soc_i2c_devname(unit, devno)));
                rv=SOC_E_NOT_FOUND;
                break;
            }
        }

        rv=ltc388x_read_current(unit, devno, &fval, len);

        /* For Tomahawk PVT Six phase support */
        if ((SOC_CONTROL(unit)->board_type == 0x88)
             && (soc_i2c_devtype(unit, devno) == LTC3882_DEVICE_TYPE)) {
            rv=ltc388x_read_current(unit, devno+1, &fval1, I2C_BOTH_CH);
            fval += fval1;
            fval1 = 0;
            rv=ltc388x_read_current(unit, devno+2, &fval1, I2C_BOTH_CH);
            fval += fval1;
        }

#ifdef COMPILER_HAS_DOUBLE
        *(double *)data=fval;
#else
        *(int *)data=fval;
#endif
        break;

    case I2C_LTC_IOC_READ_POUT:
#if defined (BCM_APACHE_SUPPORT) || defined (BCM_HELIX5_SUPPORT) || defined (BCM_FIREBOLT6_SUPPORT)
        ((i2c_adc_t*)data)->max =
        ((i2c_adc_t*)data)->min =
        ((i2c_adc_t*)data)->val =
        ((i2c_adc_t*)data)->delta = 0;
        /* For Apache/Maverick/FB5/HX5 Three phase support */
        if ((SOC_CONTROL(unit)->board_type == 0x94) ||
            (SOC_CONTROL(unit)->board_type == 0x96) ||
            (SOC_CONTROL(unit)->board_type == 0xAA) ||
            (SOC_CONTROL(unit)->board_type == 0xB0) ||
            (SOC_CONTROL(unit)->board_type == 0xB3) ||
            (SOC_CONTROL(unit)->board_type == 0xB6) ||
            (SOC_CONTROL(unit)->board_type == 0xC0)) {
            /* Taking 100 Samples */
            for (iter = 0; iter < 100; iter++) {
                if (soc_i2c_addr(unit, devno) == I2C_LTC3884_SADDR42) { 
                    /* For FB6 Four phase support */
                    if (SOC_CONTROL(unit)->board_type == 0xC0) {
                       no_ch = I2C_BOTH_CH;
                    } else if(SOC_CONTROL(unit)->board_type != 0xAA) {
                       no_ch = I2C_LTC3880_CH0;
                    }
                    SOC_IF_ERROR_RETURN
                        (ltc388x_read_power(unit, devno, &fval, len));
                    SOC_IF_ERROR_RETURN
                        (ltc388x_read_power(unit, devno + 1, &fval1, no_ch));
                    fval += fval1;
                } else {
                    rv = ltc388x_read_power(unit, devno, &fval, len);
                }
                if (((i2c_adc_t*)data)->min == 0) {
                    ((i2c_adc_t*)data)->min = fval;
                }
                if (fval > ((i2c_adc_t*)data)->max) {
                    ((i2c_adc_t*)data)->max = fval;
                } else if (fval < ((i2c_adc_t*)data)->min) {
                    ((i2c_adc_t*)data)->min = fval;
                }
            }
            ((i2c_adc_t*)data)->val =
                (((i2c_adc_t*)data)->max + ((i2c_adc_t*)data)->min) / 2;
            ((i2c_adc_t*)data)->delta =
                (((i2c_adc_t*)data)->max - ((i2c_adc_t*)data)->min);
            ((i2c_adc_t*)data)->nsamples = 100;
            break;
        }
#endif /* #if defined (BCM_APACHE_SUPPORT) || defined (BCM_HELIX5_SUPPORT) */

        rv=ltc388x_read_power(unit, devno, &fval, len);

        /* For Tomahawk PVT Six phase support */
        if ((SOC_CONTROL(unit)->board_type == 0x88)
            && (soc_i2c_devtype(unit, devno) == LTC3882_DEVICE_TYPE)) {
            rv=ltc388x_read_power(unit, devno+1, &fval1, I2C_BOTH_CH);
            fval += fval1;
            fval1 = 0;
            rv=ltc388x_read_power(unit, devno+2, &fval1, I2C_BOTH_CH);
            fval += fval1;
        }
#ifdef COMPILER_HAS_DOUBLE
        *(double *)data=fval;
#else
        *(int *)data=fval;
#endif
        break;

    case I2C_LTC_IOC_SET_VOUT:
        /* Finding and  setting page numbers */
        if (DEV_DAC_PARAMS(dev)) {

#if defined(BCM_FIREBOLT6_SUPPORT)
            if(!SOC_IS_FIREBOLT6(unit)) {
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
                if ((rv=ltc388x_check_page(unit, devno, ch)) < 0) {
                    LOG_CLI((BSL_META_U(unit, "Error: failed to set page %d in "
                                    "LTC388X device.\n"), ch));
                    break;
                }

#if defined(BCM_FIREBOLT6_SUPPORT)
            }
#endif
            /* Conversion of output voltage */
#ifdef COMPILER_HAS_DOUBLE
            fval = *((double*)data);
            voltage = (fval*4096);
#else
            fval = *((int*)data);
            /*2^12 conversion and changing from uVolt */
            voltage = (fval*4096)/1000000;
#endif
            if ((voltage < DEV_DAC_PARAMS(dev)[len].dac_min_hwval)||
                    (voltage > DEV_DAC_PARAMS(dev)[len].dac_max_hwval)) {
#ifdef  COMPILER_HAS_DOUBLE
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                          "unit %d i2c %d: LTC388X given voltage %2.3f V "
                          "beyond range( max=%2.3f V, min=%2.3f V) for "
                          "voltage VDD_%s \n"), unit, devno, 
                          (double)voltage/4096, 
                          (double) DEV_DAC_PARAMS(dev)[len].dac_max_hwval/4096,
                          (double) DEV_DAC_PARAMS(dev)[len].dac_min_hwval/4096, 
                          DEV_DAC_PARAMS(dev)[len].name));
#else
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                          "unit %d i2c %d: LTC388X given voltage %d mV "
                          "beyond range( max=%d mV, min=%d mV ) for " 
                          "voltage  VDD_%s \n"), unit, devno, 
                          voltage*1000/4096, 
                          DEV_DAC_PARAMS(dev)[len].dac_max_hwval*1000/4096,
                          DEV_DAC_PARAMS(dev)[len].dac_min_hwval*1000/4096,
                          DEV_DAC_PARAMS(dev)[len].name));

#endif
                rv = SOC_E_PARAM;
                break;
            }
            dac = voltage;
            /* Show what we are doing, for now ... */
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                        "unit %d i2c %s: LTC388X ioctl "
                        "I2C_DAC_IOC_SET_VOUT : voltage = %d, len = %d\n"),
                        unit, soc_i2c_devname(unit,devno), voltage, len));
#if defined (BCM_APACHE_SUPPORT) || defined (BCM_HELIX5_SUPPORT) || defined (BCM_FIREBOLT6_SUPPORT)
            /* For Apache/Maverick/FB5/HX5 Three Phase Support
             * LTC3884 at 0x42 : Channel 0,1 = VDD_Core
             * LTC3884 at 0x43 : Channel 0 = VDD_Core
             *                   Channel 1 = VDD_Analog
             *
             * For Firebolt6 4 phase support
             * LTC3884 at 0x42 : Channel 0,1 = VDD_core
             * LTC3884 at 0x43 : Channel 0,1 = VDD_core
             */
            if ((SOC_CONTROL(unit)->board_type == 0x94) ||
                (SOC_CONTROL(unit)->board_type == 0x96) ||
                (SOC_CONTROL(unit)->board_type == 0xB0) ||
                (SOC_CONTROL(unit)->board_type == 0xB3) ||
                (SOC_CONTROL(unit)->board_type == 0xB6) ||
                (SOC_CONTROL(unit)->board_type == 0xC0)) {
                if (soc_i2c_addr(unit, devno) == I2C_LTC3884_SADDR42) {
                    if (SOC_CONTROL(unit)->board_type == 0xC0) {
                        no_ch = I2C_BOTH_CH;
                    } else {
                        no_ch = I2C_LTC3880_CH0;
                    }
                    SOC_IF_ERROR_RETURN
                        (ltc388x_dac_set_ch_voltage(unit, devno, ch, voltage));
                    /* Set VDD_Core on both Channels of I2C_LTC3884_SADDR43/42 */
                    SOC_IF_ERROR_RETURN
                        (ltc388x_dac_set_ch_voltage(unit, devno + 1, no_ch, voltage));
                } else {
                    SOC_IF_ERROR_RETURN
                        (ltc388x_dac_set_ch_voltage(unit, devno, ch, voltage));
                }
                /* Store voltage setting in NV memory to retain them after power cycling */
                rv = ltc388x_write(unit, devno, PMBUS_CMD_STORE_USER_ALL, (void *)&dac, 0);
            } else
#endif /* #if defined (BCM_APACHE_SUPPORT) || defined (BCM_HELIX5_SUPPORT) */
            {
                rv = ltc388x_write(unit, devno, PMBUS_CMD_VOUT_COMMAND,(void *) &dac, 2);
            }
            /* Keep last value since DAC is write-only device */
            DEV_DAC_PARAMS(dev)[len].dac_last_val = dac;
        }
        break;

    case I2C_SET_SEQ:
        if ((rv = ltc388x_check_page(unit, devno, len)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: failed to set page %d in "
                        "unit: %d LTC388X device name: %s.\n"),
                        len, unit, soc_i2c_devname(unit, devno)));
            break;
        }
#ifdef  COMPILER_HAS_DOUBLE
        fval = *((double*)data);
        rv = ltc388x_float_to_L11(fval, &time_ms);
#else
        fval = *((int*)data);
        rv = ltc388x_float_to_L11(fval/1000000, &time_ms);
#endif
        if ((rv = ltc388x_write(unit, devno, PMBUS_CMD_POWER_ON_DELAY,
                    (void *) &time_ms, 2)) < 0) {
             LOG_CLI((BSL_META_U(unit, "Error: failed to set TON_DELAY for"
                     " %s.\n"), soc_i2c_devname(unit, devno)));
             break;
        }
        if ((rv = ltc388x_wait_for_not_busy(unit, devno)) < 0) {
             LOG_CLI((BSL_META_U(unit, "Error: unit: %d  LTC388X device name"
                     " %s Device is busy.\n"),
                     unit, soc_i2c_devname(unit, devno)));
             break;
        }
        dac = 0;
        if ((rv = ltc388x_write(unit, devno, PMBUS_CMD_STORE_USER_ALL, 
                    (void *)&dac, 0)) < 0) {
            LOG_CLI((BSL_META_U(unit, "Error: Failed to store the TON_DELAY"
                    " value to NVRAM in device: %s.\n"),
                    soc_i2c_devname(unit, devno)));
            break;
        }
        if ((rv = ltc388x_wait_for_not_busy(unit, devno)) < 0) {
             LOG_CLI((BSL_META_U(unit, "Error: unit: %d  LTC388X device name"
                    " %s Device is busy.\n"),
                    unit, soc_i2c_devname(unit, devno)));
             break;
        }
        break;

    case I2C_READ_SEQ:
         if ((rv = ltc388x_check_page(unit, devno, len)) < 0) {
             LOG_CLI((BSL_META_U(unit, "Error: failed to set page %d in "
                    "unit: %d LTC388X device name: %s.\n"),
                    len, unit, soc_i2c_devname(unit, devno)));
             break;
         }
         datalen = 2;
         if ((rv = ltc388x_read(unit, devno, PMBUS_CMD_POWER_ON_DELAY,
                    (void *) &time_ms, &datalen)) < 0) {
             LOG_CLI((BSL_META_U(unit, "Error: failed to read TON_DELAY "
                    "for %s.\n"), soc_i2c_devname(unit, devno)));
             break;
         }
         rv = ltc388x_L11_to_float(time_ms, &fval);
#ifdef  COMPILER_HAS_DOUBLE
        *(double *)data = fval;
#else
        *(int *)data = (fval);
#endif
        break;

    default:
        LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                    "unit %d i2c %s: LTC388X_ioctl: invalid opcode (%d)\n"),
                    unit, soc_i2c_devname(unit,devno), opcode));
        break;
    }
    sal_mutex_give(DEV_MUTEX(dev));
    return rv;
}

STATIC int
ltc388x_init(int unit, int devno, void* data, int len)
{
    int rv = SOC_E_NONE;
    uint16 dac;
    i2c_device_t *dev = soc_i2c_device(unit, devno);
    char *devname;
    int devtype;

    if (dev == NULL) {
        rv = SOC_E_INTERNAL;
        goto err_return;
    }

    devname = (char *)soc_i2c_devname(unit, devno);
    if (dev->priv_data == NULL) {
        dev->priv_data = sal_alloc(sizeof(device_data_t), devname);
        if (dev->priv_data == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, 
                        "Fail to allocate private data fo dev %s\n"), 
                        soc_i2c_devname(unit, devno)));
            rv = SOC_E_MEMORY;
            goto err_return;    
        }
        DEV_MUTEX(dev) = sal_mutex_create(devname);
        if (DEV_MUTEX(dev) == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, 
                        "Fail to create %s mutex lock\n"), 
                        soc_i2c_devname(unit, devno)));
            rv = SOC_E_MEMORY;
            goto err_free_mem;
        }
        DEV_DAC_PARAMS(dev) = NULL;
        DEV_DAC_PARAM_LEN(dev) = 0;
    }

    sal_mutex_take(DEV_MUTEX(dev), sal_mutex_FOREVER);
    if ((rv=ltc388x_write(unit, devno, PMBUS_CMD_CLEAR_FAULTS, 
            (void *)&dac, 0)) < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, 
            "Error: Failed to clear fault for LTC388X Device name:%s.\n"),
            soc_i2c_devname(unit, devno)));
        goto err_unlock;
    }
    if ((rv=ltc388x_wait_for_not_busy(unit, devno)) < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, 
            "Error: LTC388X device name %s Device is busy.\n"),
            soc_i2c_devname(unit, devno)));
        goto err_unlock;
    }
    sal_mutex_give(DEV_MUTEX(dev));

    devtype = soc_i2c_devtype(unit, devno);
    if (devtype == LTC3880_DEVICE_TYPE) {
        soc_i2c_devdesc_set(unit, devno, "Linear Tech LTC3880 Step_Down DC/DC "
            "Controller");
    } else if (devtype == LTC3882_DEVICE_TYPE) {
        soc_i2c_devdesc_set(unit, devno, "Linear Tech LTC3882 Step_Down DC/DC "
            "Controller");
    } else if (devtype == LTC3884_DEVICE_TYPE) {
        soc_i2c_devdesc_set(unit, devno, "Linear Tech LTC3884 Step_Down DC/DC "
            "Controller");
    } else if (devtype == LTM4676_DEVICE_TYPE) {
        soc_i2c_devdesc_set(unit, devno, "Linear Tech LTM4676 Step_Down DC/DC "
            "Controller");
    } else {
        soc_i2c_devdesc_set(unit, devno, devname);
    }
    
    LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit, 
            "ltc388x_init: %s, devNo=0x%x\n"), 
            soc_i2c_devname(unit,devno), devno));
    
    return rv;

err_unlock:
    sal_mutex_give(DEV_MUTEX(dev));
    sal_mutex_destroy(DEV_MUTEX(dev));
err_free_mem:
    sal_free(dev->priv_data);
    dev->priv_data = NULL;
err_return:
    return rv;
}

STATIC int 
ltc388x_deinit(int unit, int devno)
{
    int rv = SOC_E_NONE;
    i2c_device_t *dev = soc_i2c_device(unit, devno);

    if (dev != NULL) {
        if (dev->priv_data != NULL) {
            if (DEV_MUTEX(dev) != NULL) {
                sal_mutex_destroy(DEV_MUTEX(dev));
            }
            sal_free(dev->priv_data);
            dev->priv_data = NULL;
        }
    } else {
        rv = SOC_E_INTERNAL;
    }

    return rv;
}

STATIC int
ltc388x_check_device_id(int unit, int devno, char *id, char *model)
{
    int rv = SOC_E_NONE;
    i2c_saddr_t saddr;
    uint8 block_size = 0;
    uint8 block_data[256+1];

    saddr = soc_i2c_addr(unit, devno);
    if (id != NULL) {
        rv = soc_i2c_block_read(unit, saddr, PMBUS_CMD_MFR_ID, 
                                &block_size, block_data);
        if (rv == SOC_E_NONE) {
            block_data[block_size] = '\0';
            if (sal_strcmp((char *)block_data, id) != 0) {
                LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit, 
                    "I2C %s(0x%02x) MFR_ID(%d) is read "
                    "as [%s], not the expected value [%s]\n"), 
                                    soc_i2c_devname(unit, devno), saddr, 
                                    block_size, block_data, id));
                goto match_fail;
            }
        }
        else {
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit, 
                "Fail to get MFR_ID for I2C device addr 0x%02x\n"), saddr));
            goto match_fail; 
        }
    }
    
    if (model != NULL) {
        rv = soc_i2c_block_read(unit, saddr, PMBUS_CMD_MFR_MODEL, 
                                &block_size, block_data);
        if (rv == SOC_E_NONE) {
            block_data[block_size] = '\0';
            if (sal_strcmp((char *)block_data, model) != 0) {
                LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit, 
                                    "I2C %s(0x%02x) MFR_MODEL(%d) read "
                                    "as [%s], not the expected value [%s]\n"), 
                                    soc_i2c_devname(unit, devno), saddr, 
                                    block_size, block_data, model));
                goto match_fail;
            }
        }
        else {
            LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit, 
                "Fail to get MFR_MODEL for I2C device addr 0x%02x\n"), saddr));
            goto match_fail;
        }
    }

    return rv;
    
match_fail:
    return SOC_E_NOT_FOUND;
}

STATIC int
ltc3880_init(int unit, int devno, void* data, int len)
{
    SOC_IF_ERROR_RETURN(
        ltc388x_check_device_id(unit, devno, LTC_MFR_ID, LTC_3880_MFR_MODEL));

    return ltc388x_init(unit, devno, data, len);
}

STATIC int
ltc3882_init(int unit, int devno, void* data, int len)
{
    SOC_IF_ERROR_RETURN(
        ltc388x_check_device_id(unit, devno, LTC_MFR_ID, LTC_3882_MFR_MODEL));

    return ltc388x_init(unit, devno, data, len);
}

STATIC int
ltc3884_init(int unit, int devno, void* data, int len)
{
    SOC_IF_ERROR_RETURN(
        ltc388x_check_device_id(unit, devno, LTC_MFR_ID, LTC_3884_MFR_MODEL));

    return ltc388x_init(unit, devno, data, len);
}

STATIC int
ltm4676_init(int unit, int devno, void* data, int len)
{
    SOC_IF_ERROR_RETURN(
        ltc388x_check_device_id(unit, devno, LTC_MFR_ID, LTM_4676_MFR_MODEL));

    return ltc388x_init(unit, devno, data, len);
}


/* ltc 3880 voltage control Chip Driver callout */
i2c_driver_t _soc_i2c_ltc3880_driver = {
    0x0, 0x0, /* System assigned bytes */
    LTC3880_DEVICE_TYPE,
    ltc388x_read,
    ltc388x_write,
    ltc388x_ioctl,
    ltc3880_init,
    ltc388x_deinit,
};

/* ltc 3882 voltage control Chip Driver callout */
i2c_driver_t _soc_i2c_ltc3882_driver = {
    0x0, 0x0, /* System assigned bytes */
    LTC3882_DEVICE_TYPE,
    ltc388x_read,
    ltc388x_write,
    ltc388x_ioctl,
    ltc3882_init,
    ltc388x_deinit,
};

/* ltc 3884 voltage control Chip Driver callout */
i2c_driver_t _soc_i2c_ltc3884_driver = {
    0x0, 0x0, /* System assigned bytes */
    LTC3884_DEVICE_TYPE,
    ltc388x_read,
    ltc388x_write,
    ltc388x_ioctl,
    ltc3884_init,
    ltc388x_deinit,
};

/* ltm 4676 voltage control Chip Driver callout */
i2c_driver_t _soc_i2c_ltm4676_driver = {
    0x0, 0x0, /* System assigned bytes */
    LTM4676_DEVICE_TYPE,
    ltc388x_read,
    ltc388x_write,
    ltc388x_ioctl,
    ltm4676_init,
    ltc388x_deinit,
};

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * MAX127 multi-range 12-bit D-A converter chip which provides for
 * eight (8) analog input channels that are independently software
 * programmable via I2C bus.
 * See also: MAXIM MAX127 data sheet for more details.
 */
#include <sal/types.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>
/* Table 3: Range and Polarity Selection */
#define RPSEL_0_5     0x0
#define RPSEL_0_10    0x2
#define RPSEL_5_5     0x1
#define RPSEL_10_10   0x3

/* Table 4: Power Down and Clock */
#define PD_NORMAL     0x0
#define PD_STANDBY    0x2
#define PD_FULL_PD    0x3

/*
 * Make a MAX127 Control Byte
 * See Table 1: Control Byte Format
 */
#define MAX127_CONTROL(ch, rps, pds)\
  (0x80 | (ch) << 4 | (rps) << 2 | (pds))

/* Decimal point formatting */
#define INT_FRAC_3PT_3(x) (x) / 1000000, ((x) / 1000) % 1000

/* Transform raw HW register value to binary value */
#define MAX127_12BIT(x) ((((x) << 8) | ((x) >> 8)) >> 4);

/* Number of samples when reading A/D channel */
int soc_i2c_max127_iterations = 100;

 /*
 * Function: max127_range_str
 *
 * Purpose:
 *    Given an RPSEL value, return string representation of range.
 * Parameters:
 *    val - RPSEL HW value from A/D chip
 * Returns:
 *    String representing voltage range (0-5, 0-10, +/-5, +/-10)
 * Notes:
 *    None
 */
STATIC char *
max127_range_str(int rpsel)
{
    if (rpsel == RPSEL_0_5)
	return "(0-5)";
    if (rpsel == RPSEL_0_10)
	return "(0-10)";
    if (rpsel == RPSEL_5_5)
	return "(+/-5)";
    if (rpsel == RPSEL_10_10)
	return "(+/-10)";
    return "(unknown)";
}

/*
 * Function: max127_range_from_value
 *
 * Purpose:
 *    Given a raw +/-10 AD output value, return appropriate range.
 * Parameters:
 *    value - HW value from A/D chip
 * Returns:
 *    Range to use for best precision (0-5, 0-10, +/-5, +/-10)
 * Notes:
 *    Initially, we perform the conversion using +/-10 range selection.
 *    We then find out which range selection makes sense based on the
 *    output value.
 */
STATIC int
max127_range_from_value(uint16 value)
{
    int slack = 2;

    /* 2-complement to positive linear scale */
    value = (value + 2048) & 0xFFF;

    if (value > (3072 - slack))
	return RPSEL_0_10;
    if (value > (2048 + slack))
	return RPSEL_0_5;
    if (value > (1024 + slack))
	return RPSEL_5_5;
    return RPSEL_10_10;
}

/*
 * Function: max127_value
 *
 * Purpose:
 *    Convert binary value from HW to actual voltage.
 * Parameters:
 *    val - HW value from A/D chip
 *    range - current input range
 * Returns:
 *    Actual voltage in V
 * Notes:
 *    None
 */
#ifdef	COMPILER_HAS_DOUBLE

#define RPSEL_FS0 (1.2207 * 4.096)
#define RPSEL_FS1 (2.4414 * 4.096)
#define RPSCALE   4096.0

STATIC double
max127_value(uint16 val, uint16 range)
{
    double x = 0;

    if(val == 0)
	return 0.0;
    if(range == RPSEL_0_5){
	/* 	x = val / RPSCALE * RPSEL_FS0; */
	x = val * 0.00122;
    }
    else if (range == RPSEL_0_10)
	x = val / RPSCALE * RPSEL_FS1;
    else if(range == RPSEL_5_5){
	x = - 10.0/4096.0 * (4096 - val);
    }
    else if (range == RPSEL_10_10){
	x = - 20.0/4096.0 * (4096 - val);
    }
    return x;
}
#else
STATIC int
max127_value(uint16 val, uint16 range)
{
    int x;

    if (val == 0) {
	return 0;
    }

    switch (range) {
    case RPSEL_0_5:
        x = val * 5;
        break;
    case RPSEL_0_10:
        x = val * 10;
        break;
    case RPSEL_5_5:
        x = (((val + 2048) & 0xFFF) - 2048) * 10;
        break;
    case RPSEL_10_10:
        x = (((val + 2048) & 0xFFF) - 2048) * 20;
        break;
    default:
        x = 0;
        break;
    }

    /* Convert to uV with rounding and no overflow */
    x *= ((100 * 1000000) / 4096);
    x += 50;
    x /= 100;

    return x;
}
#endif

/*
 * Data: max127_vals
 * Purpose: Range and Polarity selection for each A/D channel.
 */
#ifdef COMPILER_HAS_DOUBLE
STATIC
max127_t max127_vals[] = {
    /* Initial conversion values: +/-10 */
    {0,10,-10,0,0,
      MAX127_CONTROL(I2C_ADC_CH0, RPSEL_10_10, PD_NORMAL), RPSEL_10_10 },
    {0,10,-10,0,0,
      MAX127_CONTROL(I2C_ADC_CH1, RPSEL_10_10, PD_NORMAL), RPSEL_10_10 },
    {0,10,-10,0,0,
      MAX127_CONTROL(I2C_ADC_CH2, RPSEL_10_10, PD_NORMAL), RPSEL_10_10 },
    {0,10,-10,0,0,
      MAX127_CONTROL(I2C_ADC_CH3, RPSEL_10_10, PD_NORMAL), RPSEL_10_10 },
    {0,10,-10,0,0,
      MAX127_CONTROL(I2C_ADC_CH4, RPSEL_10_10, PD_NORMAL), RPSEL_10_10 },
    {0,10,-10,0,0,
      MAX127_CONTROL(I2C_ADC_CH5, RPSEL_10_10, PD_NORMAL), RPSEL_10_10 },
    {0,10,-10,0,0,
      MAX127_CONTROL(I2C_ADC_CH6, RPSEL_10_10, PD_NORMAL), RPSEL_10_10 },
    {0,10,-10,0,0,
      MAX127_CONTROL(I2C_ADC_CH7, RPSEL_10_10, PD_NORMAL), RPSEL_10_10 }
} ;
#define N_MAX127_CHAN  COUNTOF(max127_vals)
#else
#define N_MAX127_CHAN  8
#endif

/*
 * Function: max127_calibrate
 *
 * Purpose: Update MAX127 control word with specified RPSEL value.
 *
 * Parameters:
 *    ch - MAX127 A/D channel number
 *    rpsel - range and polarity selection for MAX127 A/D conversion.
 *
 * Returns:
 *    None
 */
STATIC void
max127_calibrate(int ch, max127_t *data, int rpsel)
{
    data->cr = rpsel;
    data->cv = MAX127_CONTROL(ch, rpsel, PD_NORMAL);
}



extern uint32 soc_i2c_pci_read(int unit, uint32 addr);
extern int soc_i2c_pci_write(int unit, uint32 addr, uint32 data);


/*
 * Function: max127_conversion
 *
 * Purpose: Perform A/D conversions on a channel
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    ch - A/D channel number (0 <= channel <= 7)
 *    val - HW value from A/D chip for A/D conversion (raw HW value)
 *
 * Returns:
 *    None
 */
STATIC int
max127_conversion(int unit, int devno, int ch, max127_t *data, uint16 *val)
{
    uint8 saddr;

    saddr = soc_i2c_addr(unit, devno);

    /* CHi start conversion, read back data */
    if (soc_i2c_write_byte(unit, saddr, (uint8)data->cv) < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "unit %d i2c %s: could not configure channel %d\n"),
                 unit, soc_i2c_devname(unit,devno), ch));
        *val = 0;
        return -1;
    }

    soc_i2c_device(unit, devno)->tbyte++;

    /* Read back CHi conversion value */
    if (soc_i2c_read_word(unit, saddr, val) < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "unit %d i2c %s: could not read channel %d\n"),
                 unit, soc_i2c_devname(unit,devno), ch));
        *val = 0;
        return -1;
    }

    soc_i2c_device(unit, devno)->rbyte += 2;
    return 0;
}

STATIC void
max127_read_channel(int unit, int devno, int ch, max127_t *data)
{
    int i;
    uint16 val;
#ifdef COMPILER_HAS_DOUBLE
    double voltage;
#else
    int voltage;
#endif
    /* Reset sample data */
    data->val = 0;
#ifdef COMPILER_HAS_DOUBLE
    data->min = 10;
    data->max = -10;
#else
    data->min = 10000000; /* uV */
    data->max = -10000000; /* uV */
#endif

    /* Reset/calibrate scale to +/-10 */
    max127_calibrate(ch, data, RPSEL_10_10);

    /* Perform conversion on +/-10 scale */
    max127_conversion(unit, devno, ch, data, &val);

    /* Convert to 12-bit unsigned int for conversion value */
    val = MAX127_12BIT(val);

    /* Store new current range/scale */
    max127_calibrate(ch, data, max127_range_from_value(val));

    for (i = 0; i < soc_i2c_max127_iterations; val=0, i++) {

        /* Perform conversion on new scale */
        if (max127_conversion(unit, devno, ch, data, &val) < 0) {
            break;
        }

        /* Convert to 12-bit unsigned int for conversion value */
        val = MAX127_12BIT(val);

        /* Compute voltage from binary value */
        voltage = max127_value(val, data->cr);

        /* Store the running total */
        data->val += voltage;

        /* Compute new min */
        if (voltage < data->min) {
            data->min = voltage;
        }

        /* Compute new max */
        if (voltage > data->max) {
            data->max = voltage;
        }
    }

    /* Compute average and delta */
    if (i > 1) {
        data->val /= i;
    }
    data->delta = data->max - data->min;
}

/*
 * Function: max127_ioctl
 *
 * Purpose:
 *    Perform A/D conversion and report data.
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    ch - A/D channel
 *    data - buffer for storing A/D output data (max127_t)
 *    len - unused
 * Returns:
 *     SOC_E_NONE - no error
 * Notes:
 *    If a valid channel number is given, run conversions for
 *    that channel and return result in supplied buffer.
 *    If a negative channel number is given, simply run all 
 *    conversions and display results on the console.
 */
STATIC int
max127_ioctl(int unit, int devno, int cmd, void *data, int len)
{
    int i;
    max127_t maxdata;

    switch (cmd) {
    case I2C_ADC_DUMP_ALL:
        /* Just raw dump */
	for (i = 0; i < N_MAX127_CHAN; i++) {
#ifdef	COMPILER_HAS_DOUBLE
        max127_read_channel(unit, devno, i, &max127_vals[i]);
        LOG_CLI((BSL_META_U(unit,
                            "%s ch%d: %.3fV\t%s\t"
                            "%d samples range %.3f..%.3f (delta %.3f)\n"),
                 soc_i2c_devname(unit, devno), i,
                 max127_vals[i].val,
                 max127_range_str(max127_vals[i].cr),
                 soc_i2c_max127_iterations,
                 max127_vals[i].min,
                 max127_vals[i].max,
                 max127_vals[i].delta));
#else
        max127_t _data;
        max127_read_channel(unit, devno, i, &_data);
	    LOG_CLI((BSL_META_U(unit,
                                "%s ch%d: %d.%03dV\t%s\t"
                                "%d samples range %d.%03d..%d.%03d (delta %d.%03d)\n"),
                     soc_i2c_devname(unit, devno), i,
                     INT_FRAC_3PT_3(_data.val),
                     max127_range_str(_data.cr),
                     soc_i2c_max127_iterations,
                     INT_FRAC_3PT_3(_data.min),
                     INT_FRAC_3PT_3(_data.max),
                     INT_FRAC_3PT_3(_data.delta)));
#endif
        }
	break;
    case I2C_ADC_QUERY_CHANNEL:
        if (data) {
            sal_memset(&maxdata, 0, sizeof(maxdata));
            /* len is channel */
            max127_read_channel(unit, devno, len, &maxdata);
            ((i2c_adc_t*)data)->max = maxdata.max;
            ((i2c_adc_t*)data)->min = maxdata.min;
            ((i2c_adc_t*)data)->val = maxdata.val;
            ((i2c_adc_t*)data)->delta = maxdata.delta;
            ((i2c_adc_t*)data)->nsamples = soc_i2c_max127_iterations;
        } else {
            return SOC_E_PARAM;
        }
        break;
    case I2C_ADC_SET_SAMPLES:
        if (data) {
            soc_i2c_max127_iterations = *(int*)data;
        } else {
            return SOC_E_PARAM;
        }
        break;
    default:
        LOG_CLI((BSL_META_U(unit,
                            "Unit:%d Dev:%d Unsupported IOCTL Cmd %x\n"),
                 unit, devno, cmd));
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;

}

/*
 * Function: max127_init
 *
 * Purpose:
 *    Initialize the MAX127 A/D Chip
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    data - not used
 *    len - not used
 * Returns:
 *    SOC_E_NONE - no failure
 */
STATIC int
max127_init(int unit, int devno, void *data, int len)
{
    soc_i2c_devdesc_set(unit, devno, "MAX127 A/D Converter");
    return SOC_E_NONE;
}

/* NOT USED */
STATIC int
max127_read(int unit, int devno,
	    uint16 addr, uint8 *data, uint32* len)
{
    return SOC_E_NONE;
}

/* NOT USED */
STATIC int
max127_write(int unit, int devno,
	     uint16 addr, uint8 *data, uint32 len)
{
    return SOC_E_NONE;
}

/* MAX127 A-D converter driver callout */
i2c_driver_t _soc_i2c_max127_driver = {
    0x0, 0x0, /* System assigned bytes */
    MAX127_DEVICE_TYPE,
    max127_read,
    max127_write,
    max127_ioctl,
    max127_init,
    NULL,
};

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM56xx I2C Device Driver for Linear Technology 1427-50 DAC.
 * The LTC1427 DAC is used to control voltage outputs on various
 * SDK baseboards. See the LTC-1427-50 datasheet for more info.
 */
#include <sal/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
 #include <soc/util.h>

#ifdef	COMPILER_HAS_DOUBLE
#define I2C_DATA_T      double
#define I2C_DATA_F      "f"
#else
#define I2C_DATA_T      int
#define I2C_DATA_F      "d"
#endif

/* Calibration table in effect */
static dac_calibrate_t* dac_params;
static int dac_param_len;

STATIC int
ltc1427_read(int unit, int devno,
	  uint16 addr, uint8* data, uint32* len)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(devno);
    COMPILER_REFERENCE(addr);
    COMPILER_REFERENCE(data);
    COMPILER_REFERENCE(len);

    return SOC_E_NONE;
}

STATIC int
ltc1427_write(int unit, int devno,
	   uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    uint16 dw = 0;

    if(!data || (len <=0))
	return SOC_E_PARAM;

    /* Convert two bytes into single 16bit value */
    dw = data[0];
    dw <<= 8;
    dw |= data[1];

    dw = soc_ntohs(dw);
    dw &= ~0x8000; /* Make sure we never shut down DAC */
    rv = soc_i2c_write_word(unit, soc_i2c_addr(unit, devno), dw);

    soc_i2c_device(unit, devno)->tbyte +=2;
    return rv;
}

/*
 * NOTE NOTE NOTE:
 * All tables (dac_calibrate_t) passed to the ioctl() have size > 1
 * and the index is always within this range.
 */

#define LTC1427_SET_SELECT_MIN  0
#define LTC1427_SET_SELECT_MAX  1
#define LTC1427_SET_SELECT_MID  2

STATIC int
ltc1427_setmin_max(int unit, int devno, int set_sel, int idx)
{
    short data;
    int rv;

    /* set the default voltage */
    switch (set_sel) {
    case LTC1427_SET_SELECT_MAX:
        data = dac_params[idx].dac_max_hwval;
        break;
    case LTC1427_SET_SELECT_MID:
        data = dac_params[idx].dac_mid_hwval;
        break;
    case LTC1427_SET_SELECT_MIN:
        data = dac_params[idx].dac_min_hwval;
        break;
    default:
        return SOC_E_INTERNAL;
    }
    rv = ltc1427_write(unit, devno, 0, (uint8*)&data, sizeof(short));

    if (SOC_SUCCESS(rv)) {
	/* Keep last value since DAC is write-only device */
        dac_params[idx].dac_last_val = data;
    }
    return rv;
}

#define LTC1427_SETMAX(unit,devno, idx) \
   ltc1427_setmin_max(unit, devno, LTC1427_SET_SELECT_MAX, idx)
#define LTC1427_SETMIN(unit,devno, idx) \
   ltc1427_setmin_max(unit, devno, LTC1427_SET_SELECT_MIN, idx)
#define LTC1427_SETMID(unit,devno, idx) \
   ltc1427_setmin_max(unit, devno, LTC1427_SET_SELECT_MID, idx)

STATIC int
ltc1427_ioctl(int unit, int devno,
	   int opcode, void* data, int len)
{
    int rv = SOC_E_NONE;
    I2C_DATA_T fval,tmp;
    uint16 dac;
    dac_calibrate_t* params = NULL;

    /* length field is actually used as an index into the dac_params table*/
    if( !data || ( (dac_params != NULL) && (len > dac_param_len)))
	return SOC_E_PARAM;

    switch ( opcode ){

	/* Upload calibration table */
    case I2C_DAC_IOC_SET_CALTAB:
	params = (dac_calibrate_t*)data;
	dac_params = params;
	dac_param_len = len;
	break;

    case I2C_DAC_IOC_SETDAC_MIN:
	/* Set MIN voltage */
	rv = LTC1427_SETMIN(unit, devno, len);
	break;

    case I2C_DAC_IOC_SETDAC_MAX:
	/* Set MAX voltage */
	rv = LTC1427_SETMAX(unit, devno, len);
	break;

    case I2C_DAC_IOC_SETDAC_MID:
	/* Set mid-range voltage */
	rv = LTC1427_SETMID(unit, devno, len);
	break;

    case I2C_DAC_IOC_CALIBRATE_MAX:
	/* Set MAX output value (from ADC) */
	fval = *((I2C_DATA_T*)data);
        if (dac_params)
            dac_params[len].max = fval;
	break;

    case I2C_DAC_IOC_CALIBRATE_MIN:
	/* Set MIN output value (from ADC) */
	fval = *((I2C_DATA_T*)data);
        if (dac_params)
	    dac_params[len].min = fval;
	break;

    case I2C_DAC_IOC_CALIBRATE_STEP:
	/* Calibrate stepsize */

        if (dac_params) {
            dac_params[len].step =
                (dac_params[len].use_max ? -1 : 1) *
                (dac_params[len].max - dac_params[len].min) / 
                (dac_params[len].dac_max_hwval - dac_params[len].dac_min_hwval);
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "unit %d i2c %s: LTC1427 calibration on function %s:"
                                    "(max=%"I2C_DATA_F",min=%"I2C_DATA_F",step=%"I2C_DATA_F")\n"),
                         unit, soc_i2c_devname(unit,devno),
                         dac_params[len].name,
                         dac_params[len].max,
                         dac_params[len].min,
                         dac_params[len].step));
        }
	break;

    case I2C_DAC_IOC_SET_VOUT:
	/* Set output voltage */
	fval = *((I2C_DATA_T*)data);
        if (dac_params) {
            if ((fval < dac_params[len].min)||(fval > dac_params[len].max)){
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "unit %d i2c %s: calibration/range error :"
                                        "requested=%"I2C_DATA_F" (max=%"I2C_DATA_F"),"
                                        "min=%"I2C_DATA_F",step=%"I2C_DATA_F")\n"),
                             unit, soc_i2c_devname(unit,devno),
                             fval, dac_params[len].max,
                             dac_params[len].min,
                             dac_params[len].step));
                return SOC_E_PARAM;
            }
            /*
             * Core (A,B,PHY) : DACVAL  = VMax - VReq / Step
             * TurboRef: DACVAL = VReq - VMin / Step
             */
            if(dac_params[len].use_max)
                tmp = ((dac_params[len].max - fval) / dac_params[len].step);
            else
                tmp = ((fval - dac_params[len].min) / dac_params[len].step);
            dac = (uint16)tmp;

            dac &= LTC1427_VALID_BITS; /* Clear bits to make 10bit value */

            /* Show what we are doing, for now ... */
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "unit %d i2c %s: Set V_%s:"
                                    "request=%"I2C_DATA_F" dac=0x%x (max=%"I2C_DATA_F","
                                    "min=%"I2C_DATA_F",step=%"I2C_DATA_F")\n"),
                         unit, soc_i2c_devname(unit, devno),
                         dac_params[len].name,
                         fval,
                         dac,
                         dac_params[len].max,
                         dac_params[len].min,
                         dac_params[len].step));
            rv = ltc1427_write(unit, devno, 0, (uint8*)&dac, sizeof(short));
            /* Keep last value since DAC is write-only device */
            dac_params[len].dac_last_val = dac;
        }
	break;

    default:
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d i2c %s: ltc1427_ioctl: invalid opcode (%d)\n"),
                     unit, soc_i2c_devname(unit,devno),
                     opcode));
	break;
    }

    return rv;
}

STATIC int
ltc1427_init(int unit, int devno,
	  void* data, int len)
{
    dac_params = NULL;

    soc_i2c_devdesc_set(unit, devno, "Linear Technology LTC1427 DAC");

    return SOC_E_NONE;
}

/* LTC1427 Clock Chip Driver callout */
i2c_driver_t _soc_i2c_ltc1427_driver = {
    0x0, 0x0, /* System assigned bytes */
    LTC1427_DEVICE_TYPE,
    ltc1427_read,
    ltc1427_write,
    ltc1427_ioctl,
    ltc1427_init,
    NULL,
};

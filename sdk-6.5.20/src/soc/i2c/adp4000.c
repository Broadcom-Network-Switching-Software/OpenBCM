/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * I2C Device Driver for ADP4000 an integrated power control IC.
 */

#include <sal/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>

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
adp4000_read(int unit, int devno,
	  uint16 addr, uint8* data, uint32* len)
{
    
    int rv = SOC_E_NONE;
    uint8 saddr = soc_i2c_addr(unit, devno);

    if (!*len) {
        return SOC_E_NONE;
    }

    /* reads a single byte from a device, from a designated register*/
    if (*len == 1) {
        rv = soc_i2c_read_byte_data(unit, saddr, addr,data);
        soc_i2c_device(unit, devno)->rbyte++;   
    } else if (*len == 2) {
        rv = soc_i2c_read_word_data(unit, saddr, addr,(uint16 *)data);
        soc_i2c_device(unit, devno)->rbyte +=2;   
    } else {
        /* not supported for now */
    }

    return rv;
}

STATIC int
adp4000_write(int unit, int devno,
	   uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    uint8 saddr = soc_i2c_addr(unit, devno);

    if (len == 0) {
        /* simply writes command code to device */
        rv = soc_i2c_write_byte(unit, saddr, addr);
    } else if (len == 1) {
        rv = soc_i2c_write_byte_data(unit, saddr, addr,*data);
        soc_i2c_device(unit, devno)->tbyte++;   
    } else if (len == 2) {
        rv = soc_i2c_write_word_data(unit, saddr, addr,
             (data[1] << 8) | data[0]);
        soc_i2c_device(unit, devno)->tbyte += 2;   
    }
    return rv;
}

/*
 * NOTE NOTE NOTE:
 * All tables (dac_calibrate_t) passed to the ioctl() have size > 1
 * and the index is always within this range.
 */

#define ADP4000_SET_SELECT_MIN  0
#define ADP4000_SET_SELECT_MAX  1
#define ADP4000_SET_SELECT_MID  2

STATIC int
adp4000_setmin_max(int unit, int devno, int set_sel, int idx)
{
    uint8 data8;
    int rv;

    /* set the default voltage */
    switch (set_sel) {
    case ADP4000_SET_SELECT_MAX:
        data8 = (uint8)(dac_params[idx].dac_max_hwval);
        break;
    case ADP4000_SET_SELECT_MID:
        data8 = (uint8)(dac_params[idx].dac_mid_hwval);
        break;
    case ADP4000_SET_SELECT_MIN:
        data8 = (uint8)(dac_params[idx].dac_min_hwval);
        break;
    default:
        return SOC_E_INTERNAL;
    }
    rv = adp4000_write(unit, devno, I2C_ADP4000_CMD_VOUT_CMD,
                       &data8, 1);
    if (SOC_SUCCESS(rv)) {
	/* Keep last value since DAC is write-only device */
        dac_params[idx].dac_last_val = data8;
    }
    return rv;
}

#define ADP4000_SETMAX(unit,devno, idx) \
   adp4000_setmin_max(unit, devno, ADP4000_SET_SELECT_MAX, idx)
#define ADP4000_SETMIN(unit,devno, idx) \
   adp4000_setmin_max(unit, devno, ADP4000_SET_SELECT_MIN, idx)
#define ADP4000_SETMID(unit,devno, idx) \
   adp4000_setmin_max(unit, devno, ADP4000_SET_SELECT_MID, idx)


STATIC int
adp4000_ioctl(int unit, int devno,
	   int opcode, void* data, int len)
{
    int rv = SOC_E_NONE;
    I2C_DATA_T fval,tmp;
    uint8 dac;
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
	rv = ADP4000_SETMIN(unit, devno, len);
	break;

    case I2C_DAC_IOC_SETDAC_MAX:
	/* Set MAX voltage */
	rv = ADP4000_SETMAX(unit, devno, len);
	break;

    case I2C_DAC_IOC_SETDAC_MID:
	/* Set mid-range voltage */
	rv = ADP4000_SETMID(unit, devno, len);
	break;

    case I2C_DAC_IOC_CALIBRATE_MAX:
	/* Set MAX output value (from ADC) */
	fval = *((I2C_DATA_T*)data);
        if (dac_params) {
            dac_params[len].max = fval;
        }
	break;

    case I2C_DAC_IOC_CALIBRATE_MIN:
	/* Set MIN output value (from ADC) */
	fval = *((I2C_DATA_T*)data);
        if (dac_params) {
            dac_params[len].min = fval;
        }
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
                                "unit %d i2c %s: ADP4000 calibration on function %s:"
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
                                    "requested=%"I2C_DATA_F" (max=%"I2C_DATA_F","
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
        if(dac_params[len].use_max)  {
            tmp = ((dac_params[len].max - fval) / dac_params[len].step);
            tmp += dac_params[len].dac_max_hwval;
        } else {
            tmp = ((fval - dac_params[len].min) / dac_params[len].step);
            tmp += dac_params[len].dac_min_hwval;
        }
        dac = (uint8)tmp;

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
        rv = adp4000_write(unit, devno, I2C_ADP4000_CMD_VOUT_CMD, &dac, 1);
        /* Keep last value since DAC is write-only device */
        dac_params[len].dac_last_val = dac;
    }
	break;

    default:
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d i2c %s: adp4000_ioctl: invalid opcode (%d)\n"),
                     unit, soc_i2c_devname(unit, devno), opcode));
	break;
    }

    return rv;
}

STATIC int
adp4000_init(int unit, int devno,
	  void* data, int len)
{
    uint8 data8;
    uint32 datalen = 0;
    char *bbv_src = NULL;
    /* reset the device */
    data8 = 1 << 1;
    SOC_IF_ERROR_RETURN
        (adp4000_write(unit,devno,I2C_ADP4000_CMD_RESET,&data8, 1));

    /* clear all faults */
    /* coverity[callee_ptr_arith: FALSE] */
    SOC_IF_ERROR_RETURN
        (adp4000_write(unit,devno,I2C_ADP4000_CMD_CLEAR_FAULTS,&data8, 0));

    /* enable operation commands over the PMBus */
    datalen = 1;
    SOC_IF_ERROR_RETURN
        (adp4000_read(unit, devno, I2C_ADP4000_CMD_ON_OFF_CONFIG,&data8,
                      &datalen));
    data8 |= 0xf;
    SOC_IF_ERROR_RETURN
        (adp4000_write(unit,devno,I2C_ADP4000_CMD_ON_OFF_CONFIG,&data8, 1));

    /* set the default voltage */
    bbv_src = soc_property_get_str(unit, "bb_voltage_source");
    if (bbv_src && !sal_strcasecmp("core", bbv_src)) {
        data8 = (uint8)soc_property_get(unit, "bb_voltage_mid", ADP4000_MID_DACVAL);
    } else {
        data8 = ADP4000_MID_DACVAL;
    }
    SOC_IF_ERROR_RETURN
        (adp4000_write(unit,devno,I2C_ADP4000_CMD_VOUT_CMD,&data8, 1));


    /* bit3 1: control the ADP4000 output through VOUT command
    *      0: control the output via VID input pins which is controlled by
    *         a PCF8574 device
    */

    if (soc_property_get(unit, "voltage_control", 1)) {
        datalen = 1;
        SOC_IF_ERROR_RETURN
           (adp4000_read(unit,devno,I2C_ADP4000_CMD_CONFIG_1A,&data8,&datalen));
        data8 |= 1 << 3;
        SOC_IF_ERROR_RETURN
           (adp4000_write(unit,devno,I2C_ADP4000_CMD_CONFIG_1A,&data8, 1));
        datalen = 1;
        SOC_IF_ERROR_RETURN
           (adp4000_read(unit,devno,I2C_ADP4000_CMD_CONFIG_1B,&data8,&datalen));
        data8 |= 1 << 3;
        SOC_IF_ERROR_RETURN
           (adp4000_write(unit,devno,I2C_ADP4000_CMD_CONFIG_1B,&data8, 1));
    } else {
        datalen = 1;
        SOC_IF_ERROR_RETURN
           (adp4000_read(unit,devno,I2C_ADP4000_CMD_CONFIG_1A,&data8,&datalen));
        data8 &= ~(1 << 3);
        SOC_IF_ERROR_RETURN
           (adp4000_write(unit,devno,I2C_ADP4000_CMD_CONFIG_1A,&data8, 1));
        datalen = 1;
        SOC_IF_ERROR_RETURN
           (adp4000_read(unit,devno,I2C_ADP4000_CMD_CONFIG_1B,&data8,&datalen));
        data8 &= ~(1 << 3);
        SOC_IF_ERROR_RETURN
           (adp4000_write(unit,devno,I2C_ADP4000_CMD_CONFIG_1B,&data8, 1));
    }

    soc_i2c_devdesc_set(unit, devno, "ADP4000 Voltage Control");

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "adp4000_init: %s, devNo=0x%x\n"),
                 soc_i2c_devname(unit, devno), devno));
    return SOC_E_NONE;
}


/* adp4000 voltage control Chip Driver callout */
i2c_driver_t _soc_i2c_adp4000_driver = {
    0x0, 0x0, /* System assigned bytes */
    ADP4000_DEVICE_TYPE,
    adp4000_read,
    adp4000_write,
    adp4000_ioctl,
    adp4000_init,
    NULL,
};


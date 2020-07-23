/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        soc.c
 * Purpose:     Board driver SOC interfaces
 *
 * SOC interfaces are defined here; all other functions needed SOC
 * intefaces should use these functions.
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/i2c.h>
#include <soc/cmic.h>

#include <bcm/error.h>
#include <bcm_int/control.h>
#include <board_int/support.h>

/* device interfaces */

/* return on error except if BCM_E_UNAVAIL */
#define UOK(__rv__) \
  if (BCM_FAILURE(__rv__) && ((__rv__) != BCM_E_UNAVAIL)) { \
      return (__rv__); \
  }

/*
 * Function:
 *     board_num_devices
 * Purpose:
 *     Returns the number of local devices on the board.
 * Parameters:
 *     none
 * Returns:
 *     number of devices
 */
int
board_num_devices(void)
{
    return soc_ndev;
}

/*
 * Function:
 *     board_device_name
 * Purpose:
 *     Returns a string representing the name of the device
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     string
 */
const char *
board_device_name(int unit)
{
    return soc_dev_name(unit);
}

/*
 * Function:
 *     board_device_reset
 * Purpose:
 *     Architecture independent device reset
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_device_reset(int unit)
{
    
    UOK(soc_reset_init(unit));
    UOK(soc_misc_init(unit));
#if defined(BCM_ESW_SUPPORT)
    if (SOC_IS_ESW(unit)) {
        UOK(soc_mmu_init(unit));
    }
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *     board_device_modid_max
 * Purpose:
 *     Return the largest modid a device supports
 * Parameters:
 *      unit - (IN) BCM device number
 * Returns:
 *     modid
 */
int
board_device_modid_max(int unit)
{
    return SOC_MODID_MAX(unit);
}

/*
 * Function:
 *     board_device_info
 * Purpose:
 *     Returns device PCI info
 * Parameters:
 *     unit - (IN)  BCM device number
 *     info - (OUT) Device PCI info
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 * Notes:
 *     The function uses a BCM interface, but does not make any BCM
 *     calls.
 */
int
board_device_info(int unit, bcm_info_t *info)
{
    uint16 dev_id = 0;
    uint8 rev_id = 0;

    if (!SOC_UNIT_VALID(unit)) {
	return BCM_E_UNIT;
    }
    if (info == NULL) {
	return BCM_E_PARAM;
    }
    soc_cm_get_id(unit, &dev_id, &rev_id);
    info->vendor = SOC_PCI_VENDOR(unit);
    info->device = dev_id;
    info->revision = rev_id;
    info->capability = 0;
    
    return BCM_E_NONE;
}

/* LED processor */


/*
 * Function:
 *     board_led_program_write
 * Purpose:
 *     Write device LED program memory
 * Parameters:
 *     unit - (IN) BCM device number
 *     len  - (IN) program length
 *     pgm  - (IN) LED program
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_led_program_write(int unit, int len, uint8 *pgm)
{
    int i;

    if (len > CMIC_LED_PROGRAM_RAM_SIZE) {
        return BCM_E_PARAM;
    }

    for (i=0; i<len; i++) {
        soc_pci_write(unit, CMIC_LED_PROGRAM_RAM(i), pgm[i]);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     board_led_program_read
 * Purpose:
 *     Read device LED program memory
 * Parameters:
 *     unit   - (IN)  BCM device number
 *     maxlen - (IN)  maximum program length
 *     pgm    - (OUT) LED program
 *     actual - (OUT) actual program length
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_led_program_read(int unit, int maxlen, uint8 *pgm, int *actual)
{
    int i, rv;

    rv = BCM_E_PARAM;

    if (maxlen > CMIC_LED_PROGRAM_RAM_SIZE) {
        goto fail;
    }

    for (i=0; i<maxlen; i++) {
        pgm[i] = soc_pci_read(unit, CMIC_LED_PROGRAM_RAM(i));
    }

    if (maxlen > 0) {
        if (actual) {
            *actual = maxlen;
        }
        rv = BCM_E_NONE;
    } else {
        if (actual) {
            rv = BCM_E_NONE;
            *actual = CMIC_LED_PROGRAM_RAM_SIZE;
        }
    }

 fail:
    return rv;
}


/*
 * Function:
 *     board_led_data_write
 * Purpose:
 *     Write device LED data memory
 * Parameters:
 *     unit - (IN) BCM device number
 *     len  - (IN) program length
 *     data - (IN) LED data
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_led_data_write(int unit, int len, uint8 *data)
{
    int i;

    if (len > CMIC_LED_DATA_RAM_SIZE) {
        return BCM_E_PARAM;
    }

    for (i=0; i<len; i++) {
        soc_pci_write(unit, CMIC_LED_DATA_RAM(i), data[i]);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     board_led_data_read
 * Purpose:
 *     Read device LED data memory
 * Parameters:
 *     unit   - (IN)  BCM device number
 *     maxlen - (IN)  maximum data length
 *     pgm    - (OUT) LED data
 *     actual - (OUT) actual data length
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_led_data_read(int unit, int maxlen, uint8 *data, int *actual)
{
    int i, rv;

    rv = BCM_E_PARAM;

    if (maxlen > CMIC_LED_DATA_RAM_SIZE || !actual) {
        goto fail;
    }

    for (i=0; i<maxlen; i++) {
        data[i] = soc_pci_read(unit, CMIC_LED_DATA_RAM(i));
    }

    if (maxlen > 0) {
        if (actual) {
            *actual = maxlen;
        }
        rv = BCM_E_NONE;
    } else {
        if (actual) {
            rv = BCM_E_NONE;
            *actual = CMIC_LED_DATA_RAM_SIZE;
        }
    }

 fail:
    return rv;
}

/*
 * Function:
 *     board_led_enable_set
 * Purpose:
 *     Enable LED processor
 * Parameters:
 *     unit  - (IN) BCM device number
 *     value - (IN) TRUE to enable
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_led_enable_set(int unit, int value)
{
    uint32 ctrl;

    ctrl = soc_pci_read(unit, CMIC_LED_CTRL);

    if (value) {
        ctrl |= LC_LED_ENABLE;
    } else {
        ctrl &= ~LC_LED_ENABLE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     board_led_enable_get
 * Purpose:
 *     Get LED processor enable state
 * Parameters:
 *     unit  - (IN)  BCM device number
 *     value - (OUT) enable state
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_led_enable_get(int unit, int *value)
{
    uint32 ctrl;

    if (!value) {
        return BCM_E_PARAM;
    }

    ctrl = soc_pci_read(unit, CMIC_LED_CTRL);

    *value = ((ctrl & LC_LED_ENABLE) != 0);

    return BCM_E_NONE;
}


int
board_led_status_get(int unit, int ctl, int *value)
{
    if (!value) {
        return BCM_E_PARAM;
    }

    *value = soc_pci_read(unit, CMIC_LED_STATUS);

    return BCM_E_NONE;
}

/* I2C */
#if defined(INCLUDE_I2C)

#if defined(BROADCOM_DEBUG)

/*
 * Function:
 *     board_i2c_device_show_f
 * Purpose:
 *     board_i2c_device_show traverse callback
 * Parameters:
 *     unit      - (IN) BCM device number
 *     idx       - (IN) I2C device ordinal
 *     id        - (IN) I2C device ID
 *     name      - (IN) I2C device name
 *     desc      - (IN) I2C device description
 *     user_data - (IN) callback user data
 * Returns:
 *     void
 */
STATIC void
board_i2c_device_show_f(int unit,
                        int idx, int id, char *name, char *desc,
                        void *user_data)
{
    int *first = (int *)user_data;

    if (!*first) {
        LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                    (BSL_META_U(unit,
                    "Unit %d i2c devices:\n"),
                     unit));
        LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                    (BSL_META_U(unit,
                    "idx id  name     description\n")));
        LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                    (BSL_META_U(unit,
                                "--- --- -------- ---------------------------------\n")));
        *first = !*first;
    }
    LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                (BSL_META_U(unit,
                "%3d %3d %8s %s\n"),
                 idx, id, name, desc));
}

/*
 * Function:
 *     board_i2c_device_show
 * Purpose:
 *     Print all I2C devices on unit
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
void
board_i2c_device_show(int unit)
{
    int first;

    first = 0;
    
    board_i2c_device_traverse(unit, board_i2c_device_show_f, (void *)&first);
}
#endif /* BROADCOM_DEBUG */

/*
 * Function:
 *     board_i2c_device_init
 * Purpose:
 *     Attach and probe I2C devices on unit
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_i2c_device_init(int unit)
{
    if (!soc_i2c_is_attached(unit)) {
        BCM_IF_ERROR_RETURN(soc_i2c_attach(unit, 0, 0));
    }
    
    BCM_IF_ERROR_RETURN(soc_i2c_probe(unit));
#if defined(BROADCOM_DEBUG)
    board_i2c_device_show(unit);
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *     board_i2c_device_traverse
 * Purpose:
 *     Traverse all I2C devices on unit
 * Parameters:
 *     unit      - (IN) BCM device number
 *     func      - (IN) traverse callback function
 *     user_data - (IN) calback user data
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_i2c_device_traverse(int unit, board_i2c_device_cb func, void *user_data)
{
    int idx;
    i2c_device_t* i2c_dev = NULL;

    for (idx = 0; idx < soc_i2c_device_count(unit); idx++) {
        i2c_dev = soc_i2c_device(unit, idx) ;
        if (i2c_dev) {
            func(unit, idx,
                 i2c_dev->driver->id, i2c_dev->devname, i2c_dev->desc,
                 user_data);
        }
    }

    return BCM_E_NONE;
}

#endif

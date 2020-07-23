/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef INCLUDE_I2C

#include <sal/types.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/cm.h>
#include <soc/i2c.h>
#include <soc/bsc.h>

#define TEST_DATA	0xfe

/*
 * Function: soc_bsc_attach
 *
 * Purpose: I2C Bus attach routine, main entry point for I2C startup.
 * Initialize the I2C controller configuration for the specified
 * device.
 *
 * Parameters:
 *    unit - device number
 *
 * Returns:
 *	SOC_E_* if negative
 *	count of found devices if positive or zero
 *
 * Notes: Default is Interrupt mode, if both are selected Interrupt is
 *       chosen.
 */
int
soc_bsc_attach(int unit)
{
	soc_bsc_bus_t *bscbus;

	bscbus = BSCBUS(unit);
	if (bscbus == NULL) {
		bscbus = sal_alloc(sizeof(*bscbus), "bsc_bus");
		if (bscbus == NULL) {
			return SOC_E_MEMORY;
		}
		SOC_CONTROL(unit)->bsc_bus = bscbus;
		sal_memset(bscbus, 0, sizeof(*bscbus));
	}

	/* If not yet done, create synchronization semaphores/mutex */
	if (bscbus->bsc_mutex == NULL) {
		bscbus->bsc_mutex = sal_mutex_create("BSC Mutex");
		if (bscbus->bsc_mutex == NULL) {
			return SOC_E_MEMORY;
		}
	}

	/* Set semaphore timeout values */
	bscbus->flags = SOC_BSC_ATTACHED;

	/*
	 * Probe for BSC devices, update device list for detected devices ...
	 */
	return soc_bsc_probe(unit);
}

/*
 * Function: soc_bsc_detach
 *
 * Purpose: BSC detach routine: free resources used by BSC bus driver.
 *
 * Paremeters:
 *    unit - device number
 * Returns:
 *    SOC_E_NONE - no error
 * Notes:
 *    none
 */
int
soc_bsc_detach(int unit)
{
	soc_bsc_bus_t *bscbus = BSCBUS(unit);

	if (bscbus != NULL) {
		sal_free(bscbus);
		SOC_CONTROL(unit)->bsc_bus = NULL;
	}

	return SOC_E_NONE;
}
#endif /* INCLUDE_I2C */

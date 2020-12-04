/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:	example.c
 * Purpose:     To provide an example on how to add customer-specific APIs 
 *              by placing addtional code in the files in src/customer 
 *              directory
 */

/*
 * Here are the typical include files that might be needed
 */

/* 
 * Asserts really help making the code more robust and easy to debug
 */
#include <assert.h>

/*
 * SAL makes it portable across many platforms. For the driver "add-ons" only
 * the Core SAL is needed.
 */
#include <sal/core/libc.h>

/*
 * If you plan to add some code to BCM shell or another high-level application
 * you might need to use the Applications SAL
 */
#include <sal/appl/io.h>
#include <sal/appl/pci.h>

/*
 * If you plan to access chips at the low-level, you might need to include 
 * the SOC API declarations. This, however might make your code incompatible
 * with the future versions of BCM API
 */
#include <soc/register.h>
#include <soc/mem.h>
#include <soc/drv.h>

/*
 * The best way to go is to add code written on top of the BCM API -- the 
 * best API you can find, anyways!
 */
#include <bcm/port.h>
#include <bcm/vlan.h>
#include <bcm/error.h>
#include <bcm/link.h>

/*
 * And don't forget that you can place your own header files in the 
 * include/customer directory
 */
#include <customer/example.h>

/***************************************************************************/

/*
 * Function:   example_bcm_ok
 * Purpose:    A placeholder
 * Parameters: None
 * Returns:    BCM_E_NONE (always)
 *
 * Notes:      Please, use your company name as a prefix.
 */
int
example_bcm(void)
{
    return BCM_E_NONE;
}

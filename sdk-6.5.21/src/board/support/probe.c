/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        probe.c
 * Purpose:     Board Probe functions
 */

#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <bcm/init.h>
#include <bcm/error.h>
#include <board/board.h>
#include <board_int/support.h>

/*
 * Function:
 *     board_probe_get
 * Purpose:
 *     Returns an array of bcm_info_t corresponding to the devices on
 *     the board. If max_num is zero, then actual will contain the
 *     number of element required to be allocated for info. If max_num
 *     is greater than zero, then populate info up to max_num elements,
 *     returning the number populated in actual.
 * Parameters:
 *     max_num  - (IN)  maximum length of info, or 0 to request length
 *     info     - (OUT) array of bcm_info_t
 *     actual   - (OUT) length of array
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_probe_get(int max_num, bcm_info_t *info, int *actual)
{
    int unit, limit, rv, probed;

    limit = board_num_devices();

    /* handle query */
    if (max_num == 0) {
        if (actual) {
            *actual = limit;
        }
        return actual ? BCM_E_NONE : BCM_E_PARAM;
    }

    probed = 0;
    limit = limit < BCM_LOCAL_UNITS_MAX ? limit : BCM_LOCAL_UNITS_MAX;
    limit = max_num < limit ? max_num : limit;
    for (unit=0;  unit < limit; unit++ ) {
        rv = board_device_info(unit, &info[unit]);
        if (rv == BCM_E_UNAVAIL) {
            continue;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        } else {
            probed++;
        }
    }

    if (actual) {
        *actual = unit;
    }

    return probed ? BCM_E_NONE : BCM_E_NOT_FOUND;
}

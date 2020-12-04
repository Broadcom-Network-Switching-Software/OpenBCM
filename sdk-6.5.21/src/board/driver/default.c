/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        default.c
 * Purpose:     default board driver
 */

#include <sal/core/libc.h>
#include <bcm/error.h>
#include <board/board.h>
#include <board/manager.h>
#include <board/driver.h>

#define MAX_DESC 80

typedef struct default_private_data_s {
    char description[MAX_DESC];
} default_private_data_t;

STATIC default_private_data_t _default_private_data;

#define PRIVATE(field) (((default_private_data_t *)(driver->user_data))->field)

/*
 * Function:
 *     _default_description
 * Purpose:
 *     Return the board description string
 * Parameters:
 *     driver - (IN)  current driver
 * Returns:
 *     description string
 */
STATIC char *
_default_description(board_driver_t *driver)
{
    return PRIVATE(description);
}

/*
 * Function:
 *     _default_probe
 * Purpose:
 *     Test if board devices are appropriate for this board.
 *     In this case, all devices are appropriate.
 * Parameters:
 *      driver - (IN) board driver
 *      num    - (IN) number of devices; length of info array
 *      info   - (IN) array of bcm_info_t
 * Returns:
 *     BCM_E_NONE - all devices accepted
 */
STATIC int
_default_probe(board_driver_t *driver, int num, bcm_info_t *info)
{
    return BCM_E_NONE;
}

/*
 * Function:
 *     _default_start
 * Purpose:
 *     Start board according to flags
 * Parameters:
 *      driver - (IN) current driver
 *      flags  - (IN) start flags
 * Returns:
 *     BCM_E_NONE - success
 */
STATIC int
_default_start(board_driver_t *driver, uint32 flags)
{
    /* Reset user data */
    sal_memset(driver->user_data, 0, sizeof(default_private_data_t));

    /* start the board. */
    return BCM_E_NONE;
}

/*
 * Function:
 *     _default_stop
 * Purpose:
 *     Stop the board
 * Parameters:
 *      driver - (IN) current driver
 * Returns:
 *     BCM_E_NONE - success
 */
STATIC int
_default_stop(board_driver_t *driver)
{
    /* Reset user data */
    sal_memset(driver->user_data, 0, sizeof(default_private_data_t));

    return BCM_E_NONE;
}


board_driver_t default_board = {
    "default",
    BOARD_LOWEST_PRIORITY,
    (void *)&_default_private_data,
    0,
    NULL,
    0,
    NULL,
    _default_description,
    _default_probe,
    _default_start,
    _default_stop
};

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        builtins.c
 * Purpose:     board driver loader
 */

#include <sal/core/libc.h>
#include <bcm/error.h>
#include <board/board.h>
#include <board/driver.h>

/* Built in driver list */
STATIC board_driver_t *_board_builtin_driver[] = {
#if defined(INCLUDE_BOARD_GENERIC)
    &generic_board,
#endif /* INCLUDE_BOARD_GENERIC */
#if defined(INCLUDE_BOARD_BCM956504R48)
    &bcm956504r48_board,
#endif /* INCLUDE_BOARD_BCM956504R48 */
#if defined(INCLUDE_BOARD_BCM956504R24)
    &bcm956504r24_board,
#endif
#if defined(INCLUDE_BOARD_BCM988230)
    &bcm988230_board,
#endif /* INCLUDE_BOARD_BCM988230 */
    /* keep as last */
    &default_board
};


/*
 * Function:
 *     board_driver_builtins_add
 * Purpose:
 *     Registers all known SDK provided board drivers.
 * Parameters:
 *     none
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
int
board_driver_builtins_add(void)
{
    int i, rv = BCM_E_NOT_FOUND;

    for (i=0; i<COUNTOF(_board_builtin_driver); i++) {
        rv = board_driver_add(_board_builtin_driver[i]);
        if (BCM_FAILURE(rv) && rv != BCM_E_EXISTS) {
            break;
        }
    }

    return rv;
}

/*
 * Function:
 *     board_driver_builtins_delete
 * Purpose:
 *     Unregisters all known SDK provided board drivers.
 * Parameters:
 *     none
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
int
board_driver_builtins_delete(void)
{
    int i, rv = BCM_E_NOT_FOUND;

    for (i=0; i<COUNTOF(_board_builtin_driver); i++) {
        rv = board_driver_delete(_board_builtin_driver[i]);
        if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
            break;
        }
    }

    return rv;
}

/*
 * Function:
 *     board_driver_builtins_get
 * Purpose:
 *     Returns an array of all built in board drivers.
 * Parameters:
 *     max_num - (IN)  length of board driver array
 *     driver  - (OUT) array of pointers to board_driver_t
 *     actual  - (OUT) actual board driver array length
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
int
board_driver_builtins_get(int max_num,
                          board_driver_t **driver,
                          int *actual)
{
    if (!max_num) {
        if (actual) {
            *actual = COUNTOF(_board_builtin_driver);
        }
        return actual ? BCM_E_NONE : BCM_E_PARAM;
    } else {
        if (max_num < COUNTOF(_board_builtin_driver)) {
            max_num = COUNTOF(_board_builtin_driver);
        }
        sal_memcpy(driver, _board_builtin_driver,
                   max_num * sizeof(_board_builtin_driver[0]));
    }

    if (actual) {
        *actual = max_num;
    }

    return BCM_E_NONE;
}

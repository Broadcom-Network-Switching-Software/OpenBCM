/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        attrib.c
 * Purpose:     board attribute management
 */

#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <bcm/error.h>
#include <board/board.h>
#include <board/manager.h>
#include "mgr.h"


/*
 * Function:
 *     _attribute_match
 * Purpose:
 *     Match a requested attribute name with the registered one.
 *     Right now, just do a simple string match. It's possible
 *     to do something more complicated along the lines of the
 *     soc property suffixes, to pass additional data to the
 *     attribute function via the name field.
 *
 * Parameters:
 *      reqname - (IN) requested name
 *      regname - (IN) registered name
 * Returns:
 *      0 - match
 *     !0 - no match
 */
STATIC int
_attribute_match(char *reqname, char *regname)
{
    return !sal_strcmp(reqname, regname);
}

/*
 * Function:
 *     board_attribute_get
 * Purpose:
 *     Return a value for an attribute of the current board.
 * Parameters:
 *     attrib - (IN) board attribute id string
 *     value -  (OUT) pointer to attribute value
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_UNAVAIL - attribute not supported by board driver
 *     BCM_E_NOT_FOUND - attribute unknown to board manager
 *     BCM_E_XXX - failed
 */
int
board_attribute_get(char *name, int *value)
{
    int rv = BCM_E_FAIL;
    board_driver_t *driver;
    int i;

    BOARD_INIT;
    BOARD_LOCK;
    if ((driver = board_driver())) {
        for (i=0; i<driver->num_attribute; i++) {
            if (_attribute_match(name, driver->attribute[i].name)) {
                rv = BCM_E_UNAVAIL;
                if (driver->attribute[i].get) {
                    rv = driver->attribute[i].get(driver, name, value);
                }
                break;
            }
        }
    }
    BOARD_UNLOCK;

    return rv;
}

/*
 * Function:
 *     board_attribute_set
 * Purpose:
 *     Set attribute of the current board to the given value.
 * Parameters:
 *     attrib - (IN) board attribute id string
 *     value  - (IN) attribute value
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_UNAVAIL - attribute not supported by board driver
 *     BCM_E_NOT_FOUND - attribute unknown to board driver
 *     BCM_E_XXX - failed
 */
int
board_attribute_set(char *name, int value)
{
    int rv = BCM_E_FAIL;
    board_driver_t *driver;
    int i;

    BOARD_INIT;
    BOARD_LOCK;
    if ((driver = board_driver()) != NULL) {
        for (i=0; i<driver->num_attribute; i++) {
            if (_attribute_match(name, driver->attribute[i].name)) {
                rv = BCM_E_UNAVAIL;
                if (driver->attribute[i].set) {
                    rv = driver->attribute[i].set(driver, name, value);
                }
                break;
            }
        }
    }
    BOARD_UNLOCK;

    return rv;
}

/*
 * Function:
 *     board_attributes_get
 * Purpose:
 *     Return an array of all attributes known by the board driver up
 *     to 'max_num' number of attributes. The actual length is returned
 *     in 'actual'. If 'max_num' is zero, then 'actual' will return the
 *     length of the attribute array, and not write to 'attrib'.
 * Parameters:
 *     max_num - (IN)  length of attribute array
 *     attrib  - (OUT) array of board_attribute_t
 *     actual  - (OUT) actual attribute array length
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
int
board_attributes_get(int max_num, board_attribute_t *attribute, int *actual)
{
    int rv = BCM_E_UNAVAIL;
    board_driver_t *driver;

    BOARD_INIT;
    BOARD_LOCK;
    if ((driver = board_driver()) != NULL) {
        if (!max_num) {
            if (actual) {
                *actual = driver->num_attribute;
                rv = BCM_E_NONE;
            } else {
                rv = BCM_E_PARAM;
            }
        } else {
            if (attribute) {
                if (max_num < driver->num_attribute) {
                    max_num = driver->num_attribute;
                }
                sal_memcpy(attribute,
                           driver->attribute,
                           max_num*sizeof(board_attribute_t));
                if (actual) {
                    *actual = max_num;
                }
                rv = BCM_E_NONE;
            } else {
                rv = BCM_E_PARAM;
            }
        }
    }
    BOARD_UNLOCK;

    return rv;
}


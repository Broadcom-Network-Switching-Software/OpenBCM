/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        board.c
 * Purpose:     High level board system API
 *
 * These APIs are intended to be called by SDK applications
 */

#include <shared/bsl.h>

#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <bcm/error.h>
#include <board/board.h>
#include <board/manager.h>
#include <board_int/support.h>
#include "mgr.h"

typedef struct board_reg_s {
    board_driver_t *driver;
    struct board_reg_s *next;
} board_reg_t;

#define MAX_CONN 128
sal_mutex_t board_lock;

/* List of board registrations. Registrations are kept in
   high to low priority order. */
STATIC board_reg_t        *driver_reg;

/* Current board driver; or NULL if no driver attached. */
STATIC board_reg_t        *current;

/*
 * Function:
 *     board_driver
 * Purpose:
 *     Return the current board driver, or NULL if there is no
 *     current board driver.
 * Parameters:
 *     none
 * Returns:
 *     Pointer to board driver or NULL.
 */
board_driver_t *
board_driver(void)
{
    return current ? current->driver : NULL;
}

/*
 * Function:
 *     _board_find_by_name
 * Purpose:
 *     Return board driver registration by name, or NULL if there is no
 *     registered board driver by that name.
 * Parameters:
 *     name - (IN) board driver name
 * Returns:
 *     Pointer to board driver registration or NULL.
 */
board_reg_t *
_board_find_by_name(char *name)
{
    board_reg_t *entry;

    for (entry = driver_reg; entry != NULL; entry = entry->next) {
        if (!sal_strcmp(entry->driver->name, name)) {
            return entry;
        }
    }

    return NULL;
}

/*
 * Function:
 *     _board_insert
 * Purpose:
 *     Locate board registration insertion point in registration list
 * Parameters:
 *     driver  - (IN) driver structure
 *     insp    - (OUT) pointer to registration record insertion point
 *     none
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_EXISTS  - already registered
 * Notes:
 *     Insertion point is NULL if the insertion point is the head
 *     of the list, otherwise it is the record whose next pointer
 *     should point to the record being inserted.
 */
int
_board_insert(board_driver_t *driver, board_reg_t **insp)
{
    board_reg_t *entry;

    for (entry = driver_reg; entry != NULL; entry = entry->next) {

        if (!sal_strcmp(entry->driver->name, driver->name)) {
            return BCM_E_EXISTS;
        }

        if (driver->priority > entry->driver->priority) {
            /* priority is greater than what we are finding, so it
               must be inserted at the beginning */
            entry = NULL;
            break;
        } else if (entry->next == NULL ||
                   entry->next->driver->priority < driver->priority) {
            /* This is either the last entry, or before entries that have
               a lower priority, so insertion point is after this entry */
            break;
        }
    }

    *insp = entry;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _board_find
 * Purpose:
 *     Locate board registration and deletion point in registration list
 * Parameters:
 *     driver  - (IN) driver structure
 *     regp    - (OUT) pointer to registration record
 *     delp    - (OUT) pointer to registration record deletion point
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_NOT_FOUND  - not registered
 * Notes:
 *     Deletion point is NULL if the insertion point is the head
 *     of the list, otherwise it is the record whose next pointer
 *     points to the requested record.
 */
int
_board_find(board_driver_t *driver, board_reg_t **regp, board_reg_t **delp)
{
    board_reg_t *entry, *prev = NULL;

    for (entry = driver_reg; entry != NULL; entry = entry->next) {

        if (entry->driver == driver) {
            *regp = entry;
            if (delp) {
                *delp = prev;
            }
            return BCM_E_NONE;
        }
        prev = entry;
    }
    return BCM_E_NOT_FOUND;
}

/* Board Manager API */

/*
 * Function:
 *     board_init
 * Purpose:
 *     Initialize the board driver subsystem.
 * Parameters:
 *     none
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - board subsystem initialization failed
 */
int
board_init(void)
{
    int rv = BCM_E_INIT;

    rv = board_deinit();
    if (BCM_SUCCESS(rv)) {
        board_lock = sal_mutex_create("board-lock");
        if (board_lock != NULL) {
            rv = BCM_E_NONE;
        }
    }

    return rv;
}

/*
 * Function:
 *     board_deinit
 * Purpose:
 *     Uninitialize the board subsystem and release any allocated resources.
 * Parameters:
 *     none
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - board subsystem de-initialization failed
 */
int
board_deinit(void)
{
    if (board_lock) {
        sal_mutex_destroy(board_lock);
        board_lock = NULL;
    }
    driver_reg = NULL;
    current = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *     board_driver_add
 * Purpose:
 *
 *     Registers the board driver. driver->name must be unique among
 *     all the currently registered drivers. driver->priority is used
 *     to order the probing of board drivers, with drivers having
 *     higher priority values being probed before lower ones. Drivers
 *     with the same priority value are probed in registration order.
 *
 *     It is recommended that more specific board drivers be
 *     registered with a higher priority value than more generic board
 *     drivers. driver->name does not necessarily uniquely identify a
 *     board (although it does uniquely identify a board driver), as a
 *     specific board driver and a generic board driver may both probe
 *     successfully for the same board.
 * 
 * Parameters:
 *     driver  - (IN) driver structure
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_EXISTS - board driver by that name already registered
 *     BCM_E_XXX - other registration errors
 */
int
board_driver_add(board_driver_t *driver)
{
    board_reg_t *reg, *ins;
    int rv = BCM_E_INTERNAL;

    BOARD_INIT;
    BOARD_LOCK;
    /* find existing driver */
    rv = _board_insert(driver, &ins);
    if (BCM_SUCCESS(rv)) {
        reg = ALLOC(sizeof(board_reg_t));
        if (reg) {
            reg->driver = driver;

            if (!ins) {
                /* goes at beginning */
                reg->next = driver_reg;
                driver_reg = reg;
            } else {
                reg->next = ins->next;
                ins->next = reg;
            }
            
            rv = BCM_E_NONE;
        } else {
            rv = BCM_E_MEMORY;
        }
    }
    BOARD_UNLOCK;

    return rv;
}

/*
 * Function:
 *     board_driver_delete
 * Purpose:
 *     Unregisters a previously registered board driver.
 * Parameters:
 *     driver  - (IN) driver structure
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
int
board_driver_delete(board_driver_t *driver)
{
    board_reg_t *reg, *prev;
    int rv = BCM_E_INTERNAL;

    BOARD_INIT;
    BOARD_LOCK;
    /* Can't unregister an active board driver */
    if (current && current->driver == driver) {
        rv = BCM_E_BUSY;
    } else {
        /* find existing driver */
        rv = _board_find(driver, &reg, &prev);
        if (BCM_SUCCESS(rv)) {
            if (prev) {
                prev->next = reg->next;
            } else {
                driver_reg = reg->next;
            }

            FREE(reg);
            rv = BCM_E_NONE;
        }
    }
    BOARD_UNLOCK;
    return rv;
}

/*
 * Function:
 *     board_drivers_get
 * Purpose:
 *     Returns an array of all registered board drivers.
 * Parameters:
 *     max_num - (IN)  length of board driver array
 *     driver  - (OUT) array of pointers to board_driver_t
 *     actual  - (OUT) actual board driver array length
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
int
board_drivers_get(int max_num,
                  board_driver_t **driver,
                  int *actual)
{
    int count, copy;
    board_reg_t *entry;
    int rv = BCM_E_INTERNAL;

    BOARD_INIT;
    BOARD_LOCK;

    rv = BCM_E_PARAM;
    copy = max_num;
    count = 0;

    for (entry = driver_reg; entry != NULL; entry = entry->next) {
        if (copy-- > 0) {
            if (driver != NULL) {
                driver[count] = entry->driver;
            } else {
                /* driver must be non-NULL if copying out */
                break;
            }
        } else if (driver) {
            /* Destination at capacity */
            break;
        }
        count++;
    }

    if (actual) {
        *actual = count;
        rv = BCM_E_NONE;
    } else if (max_num > 0) {
        rv = BCM_E_NONE;
    }

    BOARD_UNLOCK;

    return rv;
}

/*
 * Function:
 *     _board_info
 * Purpose:
 *     Return an allocated array of bcm_info_t structures to devp,
 *     count to countp. Free array with sal_free() when done.
 * Parameters:
 *     countp  - (OUT)
 *     devp    - (OUT)
 * Returns:
 *     BCM_E_MEMORY - out of memory
 *     BCM_E_NONE - success
 */
STATIC int
_board_info(int *countp, bcm_info_t  **devp)
{
    int rv, count;
    bcm_info_t  *dev;

    rv = board_probe_get(0,NULL,&count);
    if (BCM_SUCCESS(rv)) {
        dev = ALLOC(sizeof(bcm_info_t) * count);
        if (dev) {
            rv = board_probe_get(count,dev,NULL);
            if (BCM_SUCCESS(rv)) {
                *countp = count;
                *devp = dev;
            } else {
                FREE(dev);
            }
        } else {
            rv = BCM_E_MEMORY;
        }
    }

    return rv;
}

/*
 * Function:
 *     board_probe
 * Purpose:
 *     Iterates across all registered board drivers, from the highest
 *     priority value to the lowest, and returns the name of the first
 *     board driver whose probe function returns BCM_E_NONE.
 * Parameters:
 *     none
 * Returns:
 *     !NULL - success
 *     NULL  - failed
 */
char *
board_probe(void)
{
    board_reg_t *reg;
    bcm_info_t  *dev;
    int count;
    int rv = BCM_E_INTERNAL;
    char *name = NULL;

    BOARD_INIT_NULL;
    BOARD_LOCK;
    rv = _board_info(&count, &dev);

    if (BCM_SUCCESS(rv)) {
#if defined(BROADCOM_DEBUG)
        {
            int i;

            LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                        (BSL_META("Board probe:\n")));
            for (i=0; i<count; i++) {
                LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                            (BSL_META("  %2d: %s\n"),
                             i, board_device_name(i)));
            }
        }
#endif

        /* For each set of registered drivers */
        for (reg=driver_reg; reg != NULL; reg=reg->next) {
            if (!reg->driver->probe) {
                LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                            (BSL_META("Board %s probe missing\n"),
                             reg->driver->name));
                continue;
            }
            rv = reg->driver->probe(reg->driver, count, dev);
            if (BCM_SUCCESS(rv)) {
                /* Board probe successful */
                name = reg->driver->name;
                break;
            }
        }

        FREE(dev);
    }
    BOARD_UNLOCK;

    return BCM_SUCCESS(rv) ? name : NULL;
}

/*
 * Function:
 *     board_find
 * Purpose:
 *     Find a registered board driver of the given name, returning
 *     BCM_E_NONE if the board driver is found. This function only
 *     tests if a board driver of the given name is registered; no
 *     board driver interfaces are called.
 * Parameters:
 *     name  - (IN) name of board driver to find
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
int
board_find(char *name)
{
    int rv;

    BOARD_INIT;
    BOARD_LOCK;
    rv = _board_find_by_name(name) ? BCM_E_NONE : BCM_E_FAIL;
    BOARD_UNLOCK;

    return rv;
}

/*
 * Function:
 *     board_attach
 * Purpose:
 *     Make the board driver of the given name the current board
 *     manager driver.
 * Parameters:
 *     name - (IN) name of board driver to attach
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
int
board_attach(char *name)
{
    board_reg_t *reg;
    int rv = BCM_E_INTERNAL;

    BOARD_INIT;

    if (!name) { return BCM_E_PARAM; }
    if (current) { return BCM_E_BUSY; }

    BOARD_LOCK;
    reg = _board_find_by_name(name);
    if (reg) {
        current = reg;
        rv = BCM_E_NONE;
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    BOARD_UNLOCK;

    return rv;

}

/*
 * Function:
 *     board_detach
 * Purpose:
 *     Detach the current board driver from the board manager.
 * Parameters:
 *     none
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
int
board_detach(void)
{
    int rv = BCM_E_FAIL;

    BOARD_INIT;
    BOARD_LOCK;
    if (current) {
        current = NULL;
        rv = BCM_E_NONE;
    }
    BOARD_UNLOCK;

    return rv;
}

/* Board Interface API */

/*
 * Function:
 *     board_name
 * Purpose:
 *     Returns the attached board name.
 * Parameters:
 *     none
 * Returns:
 *     NULL - no current driver
 *     !NULL - current board driver name
 */
char *
board_name(void)
{
    char *name;

    BOARD_INIT_NULL;
    BOARD_LOCK;
    name = current ? current->driver->name : NULL;
    BOARD_UNLOCK;

    return name;
}

/*
 * Function:
 *     board_description
 * Purpose:
 *     Returns the attached board description.  Board description strings
 *     are of indeterminate length, and may not be suitable for
 *     associative array keys, i.e., the same board driver may return
 *     different description strings for different hardware platforms.
 * Parameters:
 *     none
 * Returns:
 *     NULL - no current driver
 *     !NULL - current board driver name
 */
char *
board_description(void)
{
    char *desc = NULL;

    BOARD_INIT_NULL;
    BOARD_LOCK;
    if (current && current->driver->description) {
        desc = current->driver->description(current->driver);
    }
    BOARD_UNLOCK;

    return desc;
}

/*
 * Function:
 *     board_start
 * Purpose:
 *     Call the board driver start function with the given flags.
 *
 *      Flags:
 *
 *        BOARD_START_F_COLD_BOOT         Cold boot the board.
 *
 *        BOARD_START_F_WARM_BOOT         Warm boot the board.
 *
 *        BOARD_START_F_CLEAR             Clear devices on the board and
 *                                       preserve the board stacking
 *                                       configuration such as device modid
 *                                       and stack ports.
 * Parameters:
 *    flags - (IN) board start flags
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
int
board_start(uint32 flags)
{
    int rv = BCM_E_FAIL;

    BOARD_INIT;
    /* You can have one or the other but not both */
    if (flags & (BOARD_START_F_CLEAR|BOARD_START_F_WARM_BOOT)) {
        return BCM_E_PARAM;
    }

    BOARD_LOCK;
    if (current && current->driver->start) {
        rv = (current->driver->start(current->driver, flags));
    }
    BOARD_UNLOCK;

    return rv;
}

/*
 * Function:
 *     board_stop
 * Purpose:
 *     Call the board driver stop function.
 * Parameters:
 *     none
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
int
board_stop(void)
{
    int rv = BCM_E_FAIL;

    BOARD_INIT;
    BOARD_LOCK;
    if (current && current->driver->stop) {
        rv = current->driver->stop(current->driver);
    }
    BOARD_UNLOCK;

    return rv;
}

/*
 * Function:
 *     board_connections_get
 * Purpose:
 *     Return an array of connections up to max_num number of
 *     connections. The actual length is returned via 'actual'. If
 *     'max_num' is zero, then 'actual' will return the length of the
 *     connection array and not write to 'connection'.
 * Parameters:
 *     max_num    - (IN)  length of connection array
 *     connection - (OUT) array of board_connection_t
 *     actual     - (OUT) actual connection array length
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
int
board_connections_get(int max_num,
                     board_connection_t *connection,
                     int *actual)
{
    int rv = BCM_E_UNAVAIL;

    BOARD_INIT;
    BOARD_LOCK;
    if (current) {
        if (!max_num) {
            if (actual) {
                *actual = current->driver->num_connection;
                rv = BCM_E_NONE;
            } else {
                rv = BCM_E_PARAM;
            }
        } else {
            if (connection) {
                if (max_num < current->driver->num_connection) {
                    max_num = current->driver->num_connection;
                }
                sal_memcpy(connection,
                           current->driver->connection,
                           max_num*sizeof(board_connection_t));
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



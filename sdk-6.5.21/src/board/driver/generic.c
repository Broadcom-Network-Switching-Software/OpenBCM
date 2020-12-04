/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        generic.c
 * Purpose:     generic board driver suitable for single device boards
 *
 * Note: this driver is only intended for engineering use, not for production.
 */

#include <shared/bsl.h>

#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <bcm/error.h>
#include <bcm/stack.h>
#if defined(INCLUDE_LIB_STKTASK)
#include <appl/stktask/topo_brd.h>
#endif
#if defined(INCLUDE_LIB_DISCOVER)
#include <appl/discover/disc.h>
#endif

#include <board/board.h>
#include <board/manager.h>
#include <board/support.h>
#include <board/stack.h>
#include <board/driver.h>

#if defined (INCLUDE_BOARD_GENERIC)

#define MAX_DESC 80
#define MAX_UNIT 1
#define UNIT 0

#define PRIVATE(field) (((generic_private_data_t *)(driver->user_data))->field)

typedef struct generic_private_data_s {
    char description[MAX_DESC];
    bcm_info_t info;
    int modid;
    bcm_gport_t cpu;
} generic_private_data_t;

STATIC generic_private_data_t _generic_private_data;

/*
 * Function:
 *     _generic_modid_get
 * Purpose:
 *     Get board modid
 * Parameters:
 *     driver  - (IN)  current driver
 *     name    - (IN)  attribute name (ignored)
 *     modid   - (OUT) modid
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_PARAM - modid param NULL
 */
STATIC int
_generic_modid_get(board_driver_t *driver, char *name, int *modid)
{
    if (!modid) {
        return BCM_E_PARAM;
    }

    *modid = PRIVATE(modid);

    return BCM_E_NONE;
}


/*
 * Function:
 *     _generic_modid_set
 * Purpose:
 *     Set the board modid
 * Parameters:
 *     driver  - (IN)  current driver
 *     name    - (IN)  attribute name (ignored)
 *     modid   - (IN) modid
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_generic_modid_set(board_driver_t *driver, char *name, int modid)
{
    /* iterate across all units, setting modid. Handle even modid case. */
    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(UNIT, modid));
    PRIVATE(modid) = modid;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _generic_num_modid_get
 * Purpose:
 *     Get the number of modids required by this board
 * Parameters:
 *     driver  - (IN)  current driver (ignored)
 *     name    - (IN)  attribute name (ignored)
 *     modid   - (OUT) modid
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_generic_num_modid_get(board_driver_t *driver, char *name, int *num_modid)
{
    return bcm_stk_modid_count(UNIT, num_modid);
}

#if defined(INCLUDE_LIB_CPUDB)

/*
 * Function:
 *     _generic_stack_init
 * Purpose:
 *     Initialize cpudb and required stacking subsystems
 * Parameters:
 *     driver  - (IN)  current driver
 *     db_ref  - (OUT) stack CPUDB
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_generic_stack_init(board_driver_t *driver, cpudb_ref_t db_ref)
{
    BCM_IF_ERROR_RETURN(board_cpudb_get(driver, db_ref));
#if defined(INCLUDE_LIB_DISCOVER)
    disc_ttl_max_set(2);
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *     _generic_stack_program
 * Purpose:
 *     Program the board according to the CPUDB
 * Parameters:
 *     driver   - (IN) current driver
 *     db_ref   - (IN) stack CPUDB
 *     topo_cpu - (IN) stack TOPO_CPU (ignored)
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_generic_stack_program(board_driver_t *driver,
                      cpudb_ref_t db_ref,
                      topo_cpu_t *topo_cpu)
{
    BCM_IF_ERROR_RETURN
        (_generic_modid_set(driver, NULL, db_ref->local_entry->mod_ids[0]));

    /* Need to determine the criteria for this; don't know if FABRIC
     to too weak or too strong. */
    if (PRIVATE(info).capability & BCM_INFO_FABRIC) {
        BCM_IF_ERROR_RETURN
            (bcm_stk_modport_set(UNIT, PRIVATE(modid), PRIVATE(cpu)));
    }

    BCM_IF_ERROR_RETURN(board_stack_program(db_ref, 0));

    return BCM_E_NONE;
}


/*
 * Function:
 *     _generic_stack
 * Purpose:
 *     Manage stktask callbacks
 * Parameters:
 *     op        - (IN)     current driver
 *     db_ref    - (IN/OUT) stack CPUDB
 *     topo_cpu  - (IN)     stack TOPO_CPU (ignored)
 *     user_data - (IN)     user data
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_generic_stack(topo_board_control_t op,
              cpudb_ref_t db_ref,
              topo_cpu_t *topo_cpu,
              void *user_data)
{
    int rv = BCM_E_PARAM;
    board_driver_t *driver = (board_driver_t *)user_data;

    switch (op) {
    case topoBoardInit:
        rv = _generic_stack_init(driver, db_ref);
        break;

    case topoBoardProgram:
        rv = _generic_stack_program(driver, db_ref, topo_cpu);
        break;

    default:
        break;
        
    }

    return rv;
}
#endif

/*
 * Function:
 *     _generic_description
 * Purpose:
 *     Return the board description string
 * Parameters:
 *     driver - (IN)  current driver
 * Returns:
 *     description string
 */
STATIC char *
_generic_description(board_driver_t *driver)
{
    return PRIVATE(description);
}

/*
 * Function:
 *     _generic_probe
 * Purpose:
 *     Test if board devices are appropriate for this board.
 *     All single unit boards are appropriate.
 * Parameters:
 *      driver - (IN) board driver
 *      num    - (IN) number of devices; length of info array
 *      info   - (IN) array of bcm_info_t
 * Returns:
 *     BCM_E_NONE - devices match what is expected for this board
 *     BCM_E_XXX - failed
 */
STATIC int
_generic_probe(board_driver_t *driver, int num, bcm_info_t *info)
{
    int rv = BCM_E_FAIL;

    if (num == MAX_UNIT) {
        LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                    (BSL_META(__FILE__": accept\n")));
        rv = BCM_E_NONE;
    } else {
        LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                    (BSL_META(__FILE__": reject - num_units(%d) != %d\n"),
                     num, MAX_UNIT));
    }

    return rv;
}

/*
 * Function:
 *     _generic_start
 * Purpose:
 *     Start board according to flags
 * Parameters:
 *      driver - (IN) current driver
 *      flags  - (IN) start flags
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_generic_start(board_driver_t *driver, uint32 flags)
{

    /* init the unit */
    if (flags & BOARD_START_F_WARM_BOOT) {
        return BCM_E_UNAVAIL;
    } else {
        BCM_IF_ERROR_RETURN(board_device_init(UNIT));
        BCM_IF_ERROR_RETURN(board_port_init(UNIT));
#if defined(INCLUDE_I2C)
        BCM_IF_ERROR_RETURN(board_i2c_device_init(UNIT));
#endif
    }
    
    if (!(flags & (BOARD_START_F_CLEAR|BOARD_START_F_WARM_BOOT))) {

        /* Cold start */

        /* Reset user data */
        sal_memset(driver->user_data, 0, sizeof(generic_private_data_t));

        BCM_IF_ERROR_RETURN(bcm_info_get(UNIT, &PRIVATE(info)));

        sal_sprintf(PRIVATE(description),
                    "Generic %s board driver",
                    board_device_name(UNIT));

        BCM_IF_ERROR_RETURN
            (board_connections_discover
             (generic_board.num_connection,
              generic_board.connection,
              &generic_board.num_connection));

         
        /* Set initial CPU port and modid */
        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_get(UNIT, &PRIVATE(modid)));
        BCM_IF_ERROR_RETURN(board_device_cpu_port_get(UNIT, &PRIVATE(cpu)));

        /* Hook into stacking */
#if defined(INCLUDE_LIB_STKTASK) && defined(INCLUDE_LIB_CPUDB)
        BCM_IF_ERROR_RETURN
            (topo_board_register(_generic_stack, (void *)driver));
#endif
    } else if (flags & BOARD_START_F_CLEAR) {
        /* Reset */
        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(UNIT, PRIVATE(modid)));
    }

    LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                (BSL_META(__FILE__": started %s - %s\n"),
                 generic_board.name,
                 PRIVATE(description)));
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     _generic_stop
 * Purpose:
 *     Stop the board
 * Parameters:
 *      driver - (IN) current driver
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_generic_stop(board_driver_t *driver)
{
    BCM_IF_ERROR_RETURN(board_port_deinit(UNIT));
    BCM_IF_ERROR_RETURN(board_device_deinit(UNIT));

    /* Free connection data */
    sal_free(generic_board.connection);

    /* Reset user data */
    sal_memset(driver->user_data, 0, sizeof(generic_private_data_t));

    return BCM_E_NONE;
}

/* Supported attributes */
STATIC board_attribute_t _generic_attrib[] = {
    {
        BOARD_ATTRIBUTE_MODID,
        _generic_modid_get,
        _generic_modid_set,
    },
    {
        BOARD_ATTRIBUTE_NUM_MODID,
        _generic_num_modid_get,
        NULL
    }
};

STATIC board_connection_t _generic_conn[BCM_PBMP_PORT_MAX];

/* Board driver */
board_driver_t generic_board = {
    "generic",
    BOARD_GENERIC_PRIORITY,
    (void *)&_generic_private_data,
    COUNTOF(_generic_conn),
    _generic_conn,
    COUNTOF(_generic_attrib),
    _generic_attrib,
    _generic_description,
    _generic_probe,
    _generic_start,
    _generic_stop
};

#endif /* INCLUDE_BOARD_GENERIC */

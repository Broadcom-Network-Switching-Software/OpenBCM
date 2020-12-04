/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    phycommon.c
 * Purpose: common infrastructure for the various phy methods, 
 * callback registrations, etc.
 */
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

typedef struct phycommon_info_s {
    /*
     * Reset overloading tables
     */
    soc_port_phy_reset_cb_t  phy_reset_default;
    soc_port_phy_reset_cb_t  phy_reset;
    void                    *phy_reset_arg;
    /*
     * Medium change notification tables
     */
    soc_port_medium_status_cb_t  medium_status_cb;
    void                        *medium_status_cb_arg;
} phycommon_info_t;

phycommon_info_t *pi[SOC_MAX_NUM_DEVICES];

#define PHYCOMMON_INIT(unit) \
        if (pi[unit] == NULL) { return SOC_E_INIT; }

/* Port translation call-back function */
int (*soc_phy_cb_xlate_port_func)(int, int *);

/*
 * Function:
 *      soc_phy_common_detach
 * Description:
 *      De-allocates memory and initializes data structures needed for common
 *      PHY code
 * Parameter:
 *      unit -- unit number
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_MEMORY
 */
int
soc_phy_common_detach(int unit)
{
    sal_free(pi[unit]);
    pi[unit] = NULL;
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phy_common_init
 * Description:
 *      Allocates memory and initializes data structures needed for common
 *      PHY code
 * Parameter:
 *      unit -- unit number
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_MEMORY
 */
int
soc_phy_common_init(int unit)
{
    if (pi[unit] == NULL) {
        pi[unit] = sal_alloc(sizeof(phycommon_info_t) * SOC_MAX_NUM_PORTS,
                             "phycommon_info");
        if (pi[unit] == NULL) {
            return SOC_E_MEMORY;
        }
    }

    sal_memset(pi[unit], 0, sizeof(phycommon_info_t) * SOC_MAX_NUM_PORTS);
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phy_cb_xlate_port
 * Description:
 *      Provides port translation for API call-backs
 * Parameter:
 *      unit -- unit number
 *      port -- port number to be translated
 * Returns:
 *      SOC_E_xxx
 */
int
soc_phy_cb_xlate_port(int unit, int *port)
{
    if (soc_phy_cb_xlate_port_func != NULL) {
        return soc_phy_cb_xlate_port_func(unit, port);
    }
    return SOC_E_NONE;
}

/* 
 * Function:
 *      soc_port_phy_reset_register
 * Description:
 *      Register a callback function to be called whenever a PHY driver
 *      needs to perform a PHY reset 
 * Parameters:
 *      unit      - Device number
 *      port      - port number
 *      callback  - The callback function to call
 *      user_data - An opaque cookie to pass to callback function 
 *                  whenever it is called
 *      reg_default -- This parameter should be set to TRUE when a default 
 *                     reset function is being registered from _bcm_port_init
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_PARAM       -- Bad {unit, port} combination
 *      SOC_E_NOT_FOUND   -- The specified {unit, port, callback, user_data} 
 *                           combination have not been registered before
 * Notes:
 *      The function registers the reset function for the PHY. During the 
 *      system initialization, the function will be used to register the
 *      original, PHY-specific reset function (this will be a standard 
 *      phy_fe_ge_reset() or phy_null_reset function) with the reg_default
 *      parameter set to TRUE. The user_data parameter will be ignored in this
 *      case.
 *      Then the user can register another reset function if he wants to (with 
 *      reg_default = 0).
 *      This allows us to provide support for the customers that want to do 
 *      additional PHY programming at the reset time.
 *
 *      The curent implementation allows to register only one callback per 
 *      {unit, port}
 */
int
soc_phy_reset_register(int                      unit, 
                       soc_port_t               port, 
                       soc_port_phy_reset_cb_t  callback,
                       void                    *user_data,
                       int                      reg_default)
{
    int rv;
    phycommon_info_t *pip;

    PHYCOMMON_INIT(unit);

    pip = &pi[unit][port];

    if ((0 <= unit && unit < SOC_MAX_NUM_DEVICES) && 
        (0 <= port && port < SOC_MAX_NUM_PORTS) &&
        callback != NULL) {
        if (reg_default) {
            pip->phy_reset_default = callback;
            rv = SOC_E_NONE;
        } else {
            if (pip->phy_reset == NULL ) {
                pip->phy_reset     = callback;
                pip->phy_reset_arg = user_data;
                rv  = SOC_E_NONE;
            } else {
                if (pip->phy_reset == callback && 
                    pip->phy_reset_arg == user_data) {
                    rv = SOC_E_EXISTS;
                } else {
                    rv = SOC_E_FULL;
                }
            }
        }
    } else {
        rv = SOC_E_PARAM;
    }
    
    return (rv);
}

/* 
 * Function:
 *      soc_port_phy_reset_unregister
 * Description:
 *      Unregister a callback function to be called whenever a PHY driver
 *      needs to perform a PHY reset 
 * Parameters:
 *      unit      - Device number
 *      port      - port number
 *      callback  - The callback function to call
 *      user_data - An opaque cookie to pass to callback function 
 *                  whenever it is called
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_PARAM       -- Bad {unit, port} combination
 *      SOC_E_NOT_FOUND   -- The specified {unit, port, callback, user_data} 
 *                           combination have not been registered before
 *      SOC_E_EXISTS      -- The same callback function with the same parameter
 *                           have already been registered
 * Notes:
 *      This function verifies that the specified callback has really been
 *      registered before.
 */
int
soc_phy_reset_unregister(int                      unit, 
                         soc_port_t               port, 
                         soc_port_phy_reset_cb_t  callback,
                         void                    *user_data)
{
    int rv;
    phycommon_info_t *pip;

    PHYCOMMON_INIT(unit);

    pip = &pi[unit][port];


    if ((0 <= unit && unit < SOC_MAX_NUM_DEVICES) && 
        (0 <= port && port < SOC_MAX_NUM_PORTS)) {
        if ((pip->phy_reset == callback) &&
            (pip->phy_reset_arg == user_data)) {
            pip->phy_reset     = NULL;
            pip->phy_reset_arg = NULL;
            rv  = SOC_E_NONE;
        } else {
            rv = SOC_E_NOT_FOUND;
        }
    } else {
        rv = SOC_E_PARAM;
    }
    
    return (rv);
}

/* 
 * Function:
 *      soc_phy_reset
 * Description:
 *      This functions is called to perform the device reset using the reset
 *      callback function(s) previously registered with soc_phy_reset_register.
 * Parameters:
 *      unit -- Device number
 *      port -- Port number
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      During system initialization the code actually registers the original
 *      PHY reset function. It will be called unless the user had registered
 *      another reset function.
 */
int
soc_phy_reset(int unit, soc_port_t port)
{
    int rv;
    phycommon_info_t *pip;

    PHYCOMMON_INIT(unit);

    pip = &pi[unit][port];


    if ((0 <= unit && unit < SOC_MAX_NUM_DEVICES) && 
        (0 <= port && port < SOC_MAX_NUM_PORTS)) {
        if (pip->phy_reset == NULL) {
            if (pip->phy_reset_default != NULL) {
                rv = pip->phy_reset_default(unit, port, NULL);
            } else {
                rv = SOC_E_INTERNAL;
            }
        } else {
            soc_phy_cb_xlate_port(unit, &port);
            rv = pip->phy_reset(unit, port, pip->phy_reset_arg);
        }
    } else {
        rv = SOC_E_PARAM;
    }
    
    return (rv);
}

/* 
 * Function:
 *      soc_phy_medium_status_register
 * Description:
 *      Register a callback function to be called on medium change event
 * Parameters:
 *      unit      - Device number
 *      port      - port number
 *      callback  - The callback function to call
 *      user_data - An opaque cookie to pass to callback function 
 *                  whenever it is called
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_PARAM  -- NULL function pointer or bad {unit, port} combination
 *      SOC_E_FULL   -- Cannot register more than 1 callback per {unit, port}
 *      SOC_E_EXISTS -- The same callback function with the same parameter 
 *                      have already been registered
 * Notes:
 *      The current implementation supports only one callback function per
 *      {unit, port}
 */
int
soc_phy_medium_status_register(int                          unit, 
                               soc_port_t                   port, 
                               soc_port_medium_status_cb_t  callback,
                               void                        *user_data)
{
    int rv;
    phycommon_info_t *pip;

    PHYCOMMON_INIT(unit);

    pip = &pi[unit][port];


    if ((0 <= unit && unit < SOC_MAX_NUM_DEVICES) && 
        (0 <= port && port < SOC_MAX_NUM_PORTS) &&
        callback != NULL) {
        if (pip->medium_status_cb == NULL) {
            pip->medium_status_cb     = callback;
            pip->medium_status_cb_arg = user_data;
            rv = SOC_E_NONE;
        } else {
            if (pip->medium_status_cb == callback  &&
                pip->medium_status_cb_arg == user_data) {
                rv = SOC_E_EXISTS;
            } else {
                rv = SOC_E_FULL;
            }
        }
    } else {
        return SOC_E_PARAM;
    }
    
    return (rv);
}

/* 
 * Function:
 *      soc_phy_medium_status_unregister
 * Description:
 *      Unegister a callback function to be called on medium change event
 * Parameters:
 *      unit      - Device number
 *      port      - port number
 *      callback  - The callback function to call
 *      user_data - An opaque cookie to pass to callback function 
 *                  whenever it is called
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_PARAM       -- Bad {unit, port} combination
 *      SOC_E_NOT_FOUND   -- The specified {unit, port, callback, user_data} 
 *                           combination have not been registered before
 */
int
soc_phy_medium_status_unregister(int                          unit, 
                                 soc_port_t                   port, 
                                 soc_port_medium_status_cb_t  callback,
                                 void                        *user_data)
{
    int rv;
    phycommon_info_t *pip;

    PHYCOMMON_INIT(unit);

    pip = &pi[unit][port];

    if ((0 <= unit && unit < SOC_MAX_NUM_DEVICES) && 
        (0 <= port && port < SOC_MAX_NUM_PORTS)) {
        if ((pip->medium_status_cb == callback) &&
            (pip->medium_status_cb_arg == user_data)) {
            pip->medium_status_cb     = NULL;
            pip->medium_status_cb_arg = NULL;
            rv = SOC_E_NONE;
        } else {
            rv = SOC_E_NOT_FOUND;
        }
    } else {
        return SOC_E_PARAM;
    }
    
    return (rv);
}

/*
 * Function:
 *      soc_phy_medium_status_notify
 * Description:
 *      This function is used to invoke the callback(s) registered via 
 *      soc_phy_medium_status_register(). 
 * Parameters:
 *      unit   -- Device number
 *      port   -- Port number
 *      medium -- New active medium
 * Returns:
 *      Nothing
 */
void
soc_phy_medium_status_notify(int unit, soc_port_t port,
                             soc_port_medium_t medium)
{
    phycommon_info_t *pip;

    /*
     * Since this function returns void, we can't use PHYCOMMON_INIT check 
     * here. Replace it with an explicit one
     */
    if (pi[unit] == NULL) {
        return;
    }

    pip = &pi[unit][port];

    if ((0 <= unit && unit < SOC_MAX_NUM_DEVICES) && 
        (0 <= port && port < SOC_MAX_NUM_PORTS) &&
        pip->medium_status_cb != NULL) {
        pip->medium_status_cb(unit, port, medium, pip->medium_status_cb_arg);
    }
}

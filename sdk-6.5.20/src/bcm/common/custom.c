/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Custom callouts
 * Mostly useful for running customized composite apis over remote units.
 */

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/custom.h>

#include <bcm_int/control.h>

static bcm_custom_cb_t	_custom_unit_func[BCM_UNITS_MAX];
static void 	       *_custom_unit_user_data[BCM_UNITS_MAX];

int
bcm_common_custom_register(int unit,
                           bcm_custom_cb_t func,
                           void *user_data)
{
    if (unit >= BCM_CONTROL_MAX) {
        return BCM_E_UNIT;
    }
    _custom_unit_func[unit] = func;
    _custom_unit_user_data[unit] = user_data;
    return BCM_E_NONE;
}

int
bcm_common_custom_unregister(int unit)
{
    return bcm_custom_register(unit, NULL, NULL);
}

int
bcm_common_custom_port_set(int unit,
                           bcm_port_t port,
                           int type,
                           int len,
                           uint32 *args)
{
    bcm_custom_cb_t     func;
    void *user_data;

    func = NULL;
    user_data = NULL;
    if (unit >= BCM_CONTROL_MAX) {
        return BCM_E_UNIT;
    } else {
        func = _custom_unit_func[unit];
        user_data = _custom_unit_user_data[unit];
   }
    if (func == NULL) {
        return BCM_E_UNAVAIL;
    }
    return (*func)(unit, port, BCM_CUSTOM_SET, type, len, args, NULL, user_data);
}

int
bcm_common_custom_port_get(int unit,
                           bcm_port_t port,
                           int type,
                           int max_len,
                           uint32 *args,
                           int *actual_len)
{
    bcm_custom_cb_t     func;
    void *user_data;

    func = NULL;
    user_data = NULL;
    if (unit >= BCM_CONTROL_MAX) {
        return BCM_E_UNIT;
    } else {
        func = _custom_unit_func[unit];
        user_data = _custom_unit_user_data[unit];
    }
    if (func == NULL) {
        return BCM_E_UNAVAIL;
    }
    
    if (actual_len) {
        /* default actual length is max_len passed in; application
           callback can modify to the appropriate value */
        *actual_len = max_len;
    }
    return (*func)(unit, port, BCM_CUSTOM_GET, type,
                   max_len, args, actual_len, user_data);
}

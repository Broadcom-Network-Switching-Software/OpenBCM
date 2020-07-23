/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * extender structure initializers
 */
#include <sal/core/libc.h>
 
#include <bcm/stat.h>
#include <bcm/fcoe.h>

/* 
 * Function:
 *      bcm_fcoe_route_t_init
 * Purpose:
 *      Initialize the fcoe route struct
 * Parameters: 
 *      route - Pointer to the struct to be init'ed
 */
void
bcm_fcoe_route_t_init(bcm_fcoe_route_t *route)
{   
    if (route != NULL) {
        sal_memset(route, 0, sizeof(*route));
    }
    return;
}

/* 
 * Function:
 *      bcm_fcoe_vsan_t_init
 * Purpose:
 *      Initialize the fcoe vsan struct
 * Parameters: 
 *      vsan - Pointer to the struct to be init'ed
 */
void
bcm_fcoe_vsan_t_init(bcm_fcoe_vsan_t *vsan)
{   
    if (vsan != NULL) {
        sal_memset(vsan, 0, sizeof(*vsan));
    }
    return;
}

/* 
 * Function:
 *      bcm_fcoe_zone_entry_t_init
 * Purpose:
 *      Initialize the fcoe zone struct
 * Parameters: 
 *      zone - Pointer to the struct to be init'ed
 */
void
bcm_fcoe_zone_entry_t_init(bcm_fcoe_zone_entry_t *zone)
{   
    if (zone != NULL) {
        sal_memset(zone, 0, sizeof(*zone));
    }
    return;
}

/* 
 * Function:
 *      bcm_fcoe_intf_config_t_init
 * Purpose:
 *      Initialize a bcm_fcoe_intf_config_t_init struct
 * Parameters: 
 *      intf - Pointer to the struct to be init'ed
 */
void
bcm_fcoe_intf_config_t_init(bcm_fcoe_intf_config_t *intf)
{   
    if (intf != NULL) {
        sal_memset(intf, 0, sizeof *intf);
    }
    return;
}

/* 
 * Function:
 *      bcm_fcoe_vsan_vft_t_init
 * Purpose:
 *      Initialize a bcm_fcoe_vsan_vft_t struct
 * Parameters: 
 *      vft - Pointer to the struct to be init'ed
 */
void
bcm_fcoe_vsan_vft_t_init(bcm_fcoe_vsan_vft_t *vft)
{   
    if (vft != NULL) {
        sal_memset(vft, 0, sizeof *vft);
    }
    return;
}

/* 
 * Function:
 *      bcm_fcoe_vsan_translate_key_config_t_init
 * Purpose:
 *      Initialize a bcm_fcoe_vsan_translate_key_config_t struct
 * Parameters: 
 *      key_config - Pointer to the struct to be init'ed
 */
void
bcm_fcoe_vsan_translate_key_config_t_init(bcm_fcoe_vsan_translate_key_config_t 
                                          *key_config)
{   
    if (key_config != NULL) {
        sal_memset(key_config, 0, sizeof *key_config);
    }
    return;
}

/* 
 * Function:
 *      bcm_fcoe_vsan_action_set_t_init
 * Purpose:
 *      Initialize a fcoe_vsan_action_set_t struct
 * Parameters: 
 *      action_set - Pointer to the struct to be init'ed
 */
void
bcm_fcoe_vsan_action_set_t_init(bcm_fcoe_vsan_action_set_t *action_set)
{   
    if (action_set != NULL) {
        sal_memset(action_set, 0, sizeof *action_set);
    }
    return;
}

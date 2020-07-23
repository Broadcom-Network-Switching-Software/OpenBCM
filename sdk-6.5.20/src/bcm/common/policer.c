/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    policer.c
 * Purpose: Manages common POLICER functions
 */

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/debug.h>

#include <bcm/policer.h>

/*
 * Function:
 *      bcm_policer_config_t_init
 * Purpose: 
 *      Initialize the policer config struct
 * Parameters:
 *      pol_cfg - Pointer to the struct to be init'ed
 */

void
bcm_policer_config_t_init(bcm_policer_config_t *pol_cfg)
{
    if (pol_cfg != NULL) {
        sal_memset(pol_cfg, 0, sizeof(*pol_cfg));
    }
    return;
}

/*
 * Function:
 *      bcm_policer_group_mode_attr_selector_t_init
 * Purpose:
 *      Initialize the policer group attribute selector structure
 * Parameters:
 *      pol_group_mode_attr_selector - Pointer to the struct to be init'ed
 */

void
bcm_policer_group_mode_attr_selector_t_init(bcm_policer_group_mode_attr_selector_t
                                            *pol_group_mode_attr_selector)
{
    if (pol_group_mode_attr_selector != NULL) {
        sal_memset(pol_group_mode_attr_selector, 0,
                   sizeof(*pol_group_mode_attr_selector));
    }
    return;
}

/*
 * Function:
 *      bcm_policer_map_t_init
 * Purpose:
 *      Initialize the policer map struct
 * Parameters:
 *      offset_map - Pointer to the struct to be init'ed
 */
void
bcm_policer_map_t_init(bcm_policer_map_t *offset_map)
{
    if (offset_map != NULL) {
        sal_memset(offset_map, 0, sizeof(*offset_map));
    }
    return;
}

/*
 * Function:
 *      bcm_policer_global_meter_config_t_init
 * Purpose:
 *      Initialize the Global Meter Configuration structure
 * Parameters:
 *      config - Pointer to the struct to be init'ed
 */
void
bcm_policer_global_meter_config_t_init(bcm_policer_global_meter_config_t *config)
{
    if (config != NULL) {
        sal_memset(config, 0, sizeof(*config));
    }
    return;
}

/*
 * Function:
 * bcm_policer_primary_color_resolution_key_t_init
 * Purpose:
 * Initialize a policer_primary_color_resolution_key object struct.
 * Parameters:
 * policer_primary_color_resolution_key - pointer to object struct.
 * Returns:
 * NONE
 */
void bcm_policer_primary_color_resolution_key_t_init(
    bcm_policer_primary_color_resolution_key_t *policer_primary_color_resolution_key)
{
    if (policer_primary_color_resolution_key != NULL) {
        sal_memset(policer_primary_color_resolution_key, 0, sizeof(*policer_primary_color_resolution_key));
    }
    return;
}

/*
 * Function:
 * bcm_policer_primary_color_resolution_config_t_init
 * Purpose:
 * Initialize a policer_primary_color_resolution_config object struct.
 * Parameters:
 * policer_primary_color_resolution_config - pointer to object struct.
 * Returns:
 * NONE
 */
void bcm_policer_primary_color_resolution_config_t_init(
    bcm_policer_primary_color_resolution_config_t *policer_primary_color_resolution_config)
{
    if (policer_primary_color_resolution_config != NULL) {
        sal_memset(policer_primary_color_resolution_config, 0, sizeof(*policer_primary_color_resolution_config));
    }
    return;
}

/*
 * Function:
 * bcm_policer_engine_t_init
 * Purpose:
 * Initialize a policer_engine object struct.
 * Parameters:
 * policer_engine - pointer to object struct.
 * Returns:
 * NONE
 */
void bcm_policer_engine_t_init(
    bcm_policer_engine_t *policer_engine)
{
    if (policer_engine != NULL) {
        sal_memset(policer_engine, 0, sizeof(*policer_engine));
    }
    return;
}

/*
 * Function:
 * bcm_policer_database_attach_config_t_init
 * Purpose:
 * Initialize a policer_database_attach_config object struct.
 * Parameters:
 * policer_engine - pointer to object struct.
 * Returns:
 * NONE
 */
void bcm_policer_database_attach_config_t_init(
    bcm_policer_database_attach_config_t *policer_database_attach_config)
{
    if (policer_database_attach_config != NULL) {
        sal_memset(policer_database_attach_config, 0, sizeof(*policer_database_attach_config));
    }
    return;
}

/*
 * Function:
 * bcm_policer_database_config_t_init
 * Purpose:
 * Initialize a policer_database_config object struct.
 * Parameters:
 * policer_database_config - pointer to object struct.
 * Returns:
 * NONE
 */
void bcm_policer_database_config_t_init(
    bcm_policer_database_config_t *policer_database_config)
{
    if (policer_database_config != NULL) {
        sal_memset(policer_database_config, 0, sizeof(*policer_database_config));
    }
    return;
}

/*
 * Function:
 * bcm_policer_expansion_group_t_init
 * Purpose:
 * Initialize a policer_expansion_group object struct.
 * Parameters:
 * policer_expansion_group - pointer to object struct.
 * Returns:
 * NONE
 */
void bcm_policer_expansion_group_t_init(
    bcm_policer_expansion_group_t *policer_expansion_group)
{
    if (policer_expansion_group != NULL) {
        sal_memset(policer_expansion_group, 0, sizeof(*policer_expansion_group));
    }
    return;
}

/*
 * Function:
 * bcm_policer_color_resolution_t_init
 * Purpose:
 * Initialize a bcm_policer_color_resolution_t struct.
 * Parameters:
 * policer_color_resolution - pointer to object struct.
 * Returns:
 * NONE
 */
void bcm_policer_color_resolution_t_init(
    bcm_policer_color_resolution_t *policer_color_resolution)
{
    if (policer_color_resolution != NULL) {
        sal_memset(policer_color_resolution, 0, sizeof(*policer_color_resolution));
    }
    return;
}

/*
 * Function:
 * bcm_policer_group_info_t_init
 * Purpose:
 * Initialize a bcm_policer_group_info_t struct.
 * Parameters:
 * policer_group_info - pointer to object struct.
 * Returns:
 * NONE
 */
void bcm_policer_group_info_t_init(
    bcm_policer_group_info_t *policer_group_info)
{
    if (policer_group_info != NULL) {
        sal_memset(policer_group_info, 0, sizeof(*policer_group_info));
    }
    return;
}

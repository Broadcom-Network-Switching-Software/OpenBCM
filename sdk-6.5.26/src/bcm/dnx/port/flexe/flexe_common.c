/** \file flexe_common.c
 *
 * Common Functions for FlexE adapter layer
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_FLEXE

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/util.h>
#include "flexe_common.h"
/* } */

/*************
 * DEFINES   *
 *************/
/* { */
/* } */
/*************
 *  MACROS   *
 *************/
/* { */
/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */
/* } */
/*************
 * GLOBALS   *
 *************/
/* { */
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/**
 * \brief - Get number of instance according to speed
 *
 */
int
flexe_common_nof_instances_get(
    int unit,
    int speed)
{
    int nof_instances;

    if ((speed == 50000) || (speed == 100000))
    {
        nof_instances = 1;
    }
    else if (speed == 200000)
    {
        nof_instances = 2;
    }
    else
    {
        nof_instances = 4;
    }

    return nof_instances;
}
/**
 * \brief - Get FlexE instance number
 *
 */
shr_error_e
flexe_common_logical_phy_id_to_instance_num_get(
    int unit,
    int speed,
    uint8 logical_phy_id,
    uint8 instance_index,
    uint8 *instance_num)
{
    SHR_FUNC_INIT_VARS(unit);

    *instance_num = 0;
    if (logical_phy_id != 0)
    {
        if ((speed == 50000) || (speed == 100000))
        {
            *instance_num = logical_phy_id;
        }
        else if (speed == 200000)
        {
            *instance_num = (logical_phy_id << 1) + instance_index;
        }
        else
        {
            *instance_num = (logical_phy_id << 2) + instance_index;
        }
    }
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get FlexE PHY ID
 *
 */
shr_error_e
flexe_common_instance_num_to_logical_phy_id_get(
    int unit,
    int speed,
    int instance_num,
    int *logical_phy_id)
{
    SHR_FUNC_INIT_VARS(unit);

    *logical_phy_id = 0;

    if (instance_num != 0)
    {
        if ((speed == 50000) || (speed == 100000))
        {
            *logical_phy_id = instance_num;
        }
        else if (speed == 200000)
        {
            *logical_phy_id = (instance_num >> 1);
        }
        else
        {
            *logical_phy_id = (instance_num >> 2);
        }
    }
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get FlexE speed mode
 *
 */
int
flexe_common_speed_mode_get(
    int unit,
    int speed,
    flexe_core_port_speed_mode_t * speed_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (speed)
    {
        case 50000:
        {
            *speed_mode = flexe_core_port_speed_mode_50G;
            break;
        }
        case 100000:
        {
            *speed_mode = flexe_core_port_speed_mode_100G;
            break;
        }
        case 200000:
        {
            *speed_mode = flexe_core_port_speed_mode_200G;
            break;
        }
        case 400000:
        {
            *speed_mode = flexe_core_port_speed_mode_400G;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported speed.\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/* } */

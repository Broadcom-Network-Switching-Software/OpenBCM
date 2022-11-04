#ifndef _FLEXE_COMMON_H
#define _FLEXE_COMMON_H

#include <bcm_int/dnx/port/flexe/flexe_core.h>

/**
 * \brief - Get number of instance according to speed
 *
 */
int flexe_common_nof_instances_get(
    int unit,
    int speed);
/**
 * \brief - Get FlexE instance ID
 *
 */
int flexe_common_logical_phy_id_to_instance_num_get(
    int unit,
    int speed,
    uint8 logical_phy_id,
    uint8 instance_index,
    uint8 *instance_num);
/**
 * \brief - Get FlexE PHY ID
 *
 */
int flexe_common_instance_num_to_logical_phy_id_get(
    int unit,
    int speed,
    int instance_num,
    int *logical_phy_id);
/**
 * \brief - Get FlexE oh speed mode
 *
 */
int flexe_common_speed_mode_get(
    int unit,
    int speed,
    flexe_core_port_speed_mode_t * speed_mode);

#endif

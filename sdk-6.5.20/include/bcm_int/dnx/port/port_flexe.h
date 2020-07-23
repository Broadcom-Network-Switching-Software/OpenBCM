/** \file include/bcm_int/dnx/port/port_flexe.h
 * 
 *
 * Internal DNX Port APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PORT_FLEXE_H_INCLUDED_
/** { */
#define _PORT_FLEXE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <bcm/switch.h>

/**
 * FlexE mode
 */
typedef enum
{
    /*
     * FlexE is disable in the device
     */
    DNX_FLEXE_MODE_DISABLED = 0,
    /*
     * FlexE centralized mode
     */
    DNX_FLEXE_MODE_CENTRALIZED,
    /*
     * FlexE distributed mode
     */
    DNX_FLEXE_MODE_DISTRIBUTED,
} dnx_flexe_mode_e;

/**
 * FlexE NB TDM slots allocation mode for
 * 50G NRZ speed
 */
typedef enum
{
    /*
     * Always allocate single slot for 50G NRZ
     */
    DNX_FLEXE_ALLOC_MODE_SINGLE = 0,
    /*
     * Always allocate double slot for 50G NRZ
     */
    DNX_FLEXE_ALLOC_MODE_DOUBLE,
    /*
     * The number of slots can be modified
     * dynamically
     */
    DNX_FLEXE_ALLOC_MODE_DYNAMIC,
} dnx_flexe_nb_tdm_slot_alloc_mode_e;

typedef struct dnx_flexe_cal_info_s
{
    /*
     * Indicated all calendars in FEU has been updated
     */
    int feu_cal_is_updated;
    /*
     * Indicated all calendars in FLEXEWP has been updated
     */
    int flexewp_cal_is_updated;
} dnx_flexe_cal_info_t;

/**
 * \brief - Configure the FlexE configurations
 */
shr_error_e dnx_port_flexe_config_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    dnx_flexe_cal_info_t * cal_info);

/**
 * \brief - Configure FlexE calendar dynamically
 */
shr_error_e dnx_port_flexe_calendar_set(
    int unit,
    bcm_port_t port,
    uint32 flags);

/**
 * \brief - Init FlexE HW
 */
shr_error_e dnx_flexe_init(
    int unit);

/**
 * \brief - Configure the OAM alarm collection time step
 */
int dnx_flexe_oam_alarm_collection_timer_step_set(
    int unit,
    int step);

/**
 * \brief - Configure number of steps for OAM alarm
 *    collection period
 */
int dnx_flexe_oam_alarm_collection_step_count_set(
    int unit,
    int step_count);

/**
 * \brief - Get the OAM alarm collection time step
 */
int dnx_flexe_oam_alarm_collection_timer_step_get(
    int unit,
    int *step);

/**
 * \brief - Get number of steps for OAM alarm
 *    collection period
 */
int dnx_flexe_oam_alarm_collection_step_count_get(
    int unit,
    int *step_count);

/**
 * \brief - Configure the SAR Cell mode:
 *    28*66b or 29*66b
 */
int dnx_flexe_sar_cell_mode_set(
    int unit,
    bcm_switch_flexe_sar_cell_mode_t cell_mode);

/**
 * \brief - Get the SAR Cell mode:
 *    28*66b or 29*66b
 */
int dnx_flexe_sar_cell_mode_get(
    int unit,
    bcm_switch_flexe_sar_cell_mode_t * cell_mode);

/** } */
#endif /*_PORT_FLEXE_H_INCLUDED_*/

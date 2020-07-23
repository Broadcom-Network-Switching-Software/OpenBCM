/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  tomahawk3_flexport.h
*/


#ifndef SOC_TOMAHAWK3_FLEXPORT_H
#define SOC_TOMAHAWK3_FLEXPORT_H


/*! @file tomahawk3_flexport.h
 *   @brief FlexPort functions and structures for Tomahawk3.
 *   Details are shown below.
 */

#include <soc/flexport/flexport_common.h>
#include <soc/init/tomahawk3_flexport_defines.h>
#include <soc/init/tomahawk3_ep_init.h>
#include <soc/init/tomahawk3_tdm.h>
#include <soc/init/tomahawk3_port_init.h>
#include <soc/init/tomahawk3_idb_init.h>
#include <soc/flexport/tomahawk3_mmu_flexport.h>
#include <soc/flexport/tomahawk3_idb_flexport.h>
#include <soc/flexport/tomahawk3_cdmac_flexport.h>


/*! @struct soc_tomahawk3_flex_scratch_t
 *   @brief Scratch pad locals. Caller must not allocate. Callee allocates
 *          at entry and deallocates before return. This area is pointed
 *          to by the cookie in soc_port_schedule_state_t struct type.
 */
typedef struct soc_tomahawk3_flex_scratch_s {
/* Not used for now in TH3; user can add variables */
    int dummy;
} soc_tomahawk3_flex_scratch_t;


extern void *soc_tomahawk3_port_lane_info_alloc (void);


extern int soc_tomahawk3_set_port_lane_info (
    int         unit,
    const void *lane_info_ptr,
    int         port_idx,
    int         pgw,
    int         xlp
    );


extern int soc_tomahawk3_get_port_lane_info (
    int         unit,
    const void *lane_info_ptr,
    int *       port_idx,
    int *       pgw,
    int *       xlp
    );


extern int soc_tomahawk3_reconfigure_ports (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    );


extern int soc_tomahawk3_flex_start (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    );


extern int soc_tomahawk3_flex_end (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    );


extern int soc_tomahawk3_flex_top_port_down (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    );


extern int soc_tomahawk3_flex_top_port_up (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    );

extern int (*th3_flexport_phases[MAX_FLEX_PHASES]) (int unit,
                                                soc_port_schedule_state_t
                                                *port_schedule_state);

#endif /* SOC_TOMAHAWK3_FLEXPORT_H */

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk3_set_tdm.c
 * Purpose:
 * Requires:
 */


#include <shared/bsl.h>
#include <soc/tdm/core/tdm_top.h>

#include <soc/defs.h>
#include <soc/mem.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)

#include <soc/init/tomahawk3_set_tdm.h>


/*** START SDK API COMMON CODE ***/


/*! @fn int soc_tomahawk3_set_tdm_tbl(int unit, soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_resource Pointer to a soc_port_resource_t struct variable.
 *  @brief API to calculate TDM table
 */
int
soc_tomahawk3_set_tdm_tbl(int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int pipe;

    for (pipe = 0; pipe < SOC_MAX_NUM_PIPES; pipe++) {
        soc_tomahawk3_set_tdm_tbl_per_pipe(pipe, port_schedule_state);
    }

    return SOC_E_NONE;
}


/*! @fn void soc_tomahawk3_set_tdm_tbl_per_pipe(int pipe_num, soc_port_schedule_state_t *port_schedule_state)
 *  @param pipe_num pipe number 0:7
 *  @param port_resource Pointer to a soc_port_resource_t struct variable.
 *  @brief API to calculate TDM table pe rpipe
 */
void
soc_tomahawk3_set_tdm_tbl_per_pipe(int pipe_num, soc_port_schedule_state_t *port_schedule_state)
{
    int phy_port, slot;
    int port_slot_pos_table[_TH3_TDM_LENGTH];
    int port_sched_table[_TH3_TDM_LENGTH];
    int dev_port, dev_port_start, dev_port_end;
    soc_port_map_type_t *port_map;

    /* intialize port_sched_table[] with `NUM_EXT_PORTS*/
    for (slot = 0; slot < _TH3_TDM_LENGTH; slot++) {
        port_sched_table[slot] = SOC_MAX_NUM_PORTS;
    }

    if (pipe_num == 0) {
        dev_port_start =  1;
        dev_port_end   = _TH3_GDEV_PORTS_PER_PIPE;
    } else {
        dev_port_start = _TH3_DEV_PORTS_PER_PIPE * pipe_num;
        dev_port_end   = dev_port_start + _TH3_GDEV_PORTS_PER_PIPE - 1;
    }

    if (0 == port_schedule_state->is_flexport) { /* init time */
        port_map = &port_schedule_state->in_port_map;
    } else {                 /* flexport time */
        port_map = &port_schedule_state->out_port_map;
    }

    for (dev_port = dev_port_start; dev_port <= dev_port_end; dev_port++) {
        if (port_map->log_port_speed[dev_port] > 0) { /* port enabled */
            phy_port = port_map->port_l2p_mapping[dev_port];
            soc_tomahawk3_set_tdm_slot_pos_tbl(phy_port, 
                port_map->log_port_speed[dev_port], port_slot_pos_table);
            for (slot = 0; slot < _TH3_TDM_LENGTH; slot++) {
                if (port_slot_pos_table[slot] == 1) {
                   port_sched_table[slot] = phy_port;
                }
            }
        }
    }

    /* Special slot */
    port_sched_table[_TH3_TDM_LENGTH - 1] = SOC_MAX_NUM_PORTS + 1;

    /* Populate port_schedule_state calendars */
    port_schedule_state->tdm_ingress_schedule_pipe[pipe_num].tdm_schedule_slice[0].cal_len = _TH3_TDM_LENGTH;
    port_schedule_state->tdm_egress_schedule_pipe[pipe_num].tdm_schedule_slice[0].cal_len = _TH3_TDM_LENGTH;
    for (slot = 0; slot < _TH3_TDM_LENGTH; slot++) {
        port_schedule_state->tdm_ingress_schedule_pipe[pipe_num].tdm_schedule_slice[0].linerate_schedule[slot] =
                      port_sched_table[slot];
        port_schedule_state->tdm_egress_schedule_pipe[pipe_num].tdm_schedule_slice[0].linerate_schedule[slot] =
                      port_sched_table[slot];
    }
}



/*! @fn int soc_tomahawk3_set_tdm_slot_pos_tbl(int phy_port, int speed, int *slot_pos_tbl)
 */
int
soc_tomahawk3_set_tdm_slot_pos_tbl(int phy_port, int speed, int *slot_pos_tbl)
{
    int pm_mapping[_TH3_PORTS_PER_PBLK] = {0,4,2,6,1,5,3,7};
    int slot, subport, pm_num, num_50g_slots, pm_slot;

    for (slot = 0; slot < _TH3_TDM_LENGTH; slot++) {
        slot_pos_tbl[slot] = 0;
    }

    if (speed > 0) { /* port enabled */
        subport = (phy_port - 1) % _TH3_PORTS_PER_PBLK;
        pm_num = (phy_port - 1) / _TH3_PORTS_PER_PBLK;
        pm_num = pm_num % _TH3_PBLKS_PER_PIPE; /* that's local pm within a pipe 0:3 */
        num_50g_slots = (speed + SPEED_40G) / SPEED_50G;
        for (slot = 0; slot < num_50g_slots; slot++) {
            /* get the right slot position within pm */
            pm_slot = pm_mapping[subport + slot];
            slot_pos_tbl[(pm_slot * _TH3_PBLKS_PER_PIPE) + pm_num] = 1;
        }
    }

    return SOC_E_NONE;
}


/*** END SDK API COMMON CODE ***/

#endif /* BCM_TOMAHAWK3_SUPPORT */

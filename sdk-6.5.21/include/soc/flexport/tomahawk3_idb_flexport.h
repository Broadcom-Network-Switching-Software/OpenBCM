/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       tomahawk3_idb_flexport.h
 */



#ifndef _SOC_TOMAHAWK3_IDB_FLEXPORT_H_
#define _SOC_TOMAHAWK3_IDB_FLEXPORT_H_

#include <soc/init/tomahawk3_flexport_defines.h>
#include <soc/init/tomahawk3_tdm.h>


/*** START SDK API COMMON CODE ***/

extern int soc_tomahawk3_flex_set_epc_link_bmap(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int is_port_down);

extern int soc_tomahawk3_flex_set_dest_port_bmap(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int is_port_down);

extern int soc_tomahawk3_flex_dis_forwarding_traffic(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_flex_en_forwarding_traffic(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_flex_idb_invalidate_pnum_mapping_tbl(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_flex_idb_init_pnum_mapping_tbl(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_flex_idb_invalidate_idb_to_dev_pmap_tbl(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_flex_idb_init_idb_to_dev_pmap_tbl(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_flex_idb_clear_stats_new_ports(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_flex_idb_reconfigure(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_idb_flexport(int unit,
    soc_port_schedule_state_t *port_schedule_state);

/*** END SDK API COMMON CODE ***/

#endif




/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_EGR_DB_ACCESS_H__
#define __DNX_EGR_DB_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_egr_db_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_egr_queuing.h>


typedef int (*dnx_egr_db_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*dnx_egr_db_init_cb)(
    int unit);

typedef int (*dnx_egr_db_ps_allocation_bmap_set_cb)(
    int unit, uint32 allocation_bmap_idx_0, uint32 allocation_bmap_idx_1, uint32 allocation_bmap);

typedef int (*dnx_egr_db_ps_allocation_bmap_get_cb)(
    int unit, uint32 allocation_bmap_idx_0, uint32 allocation_bmap_idx_1, uint32 *allocation_bmap);

typedef int (*dnx_egr_db_ps_prio_mode_set_cb)(
    int unit, uint32 prio_mode_idx_0, uint32 prio_mode_idx_1, uint32 prio_mode);

typedef int (*dnx_egr_db_ps_prio_mode_get_cb)(
    int unit, uint32 prio_mode_idx_0, uint32 prio_mode_idx_1, uint32 *prio_mode);

typedef int (*dnx_egr_db_ps_if_idx_set_cb)(
    int unit, uint32 if_idx_idx_0, uint32 if_idx_idx_1, int if_idx);

typedef int (*dnx_egr_db_ps_if_idx_get_cb)(
    int unit, uint32 if_idx_idx_0, uint32 if_idx_idx_1, int *if_idx);

typedef int (*dnx_egr_db_interface_occ_set_cb)(
    int unit, uint32 interface_occ_idx_0, uint32 interface_occ_idx_1, int interface_occ);

typedef int (*dnx_egr_db_interface_occ_get_cb)(
    int unit, uint32 interface_occ_idx_0, uint32 interface_occ_idx_1, int *interface_occ);

typedef int (*dnx_egr_db_total_egr_if_credits_set_cb)(
    int unit, uint32 total_egr_if_credits_idx_0, int total_egr_if_credits);

typedef int (*dnx_egr_db_total_egr_if_credits_get_cb)(
    int unit, uint32 total_egr_if_credits_idx_0, int *total_egr_if_credits);

typedef int (*dnx_egr_db_egr_if_credits_configured_set_cb)(
    int unit, uint32 egr_if_credits_configured_idx_0, uint32 egr_if_credits_configured_idx_1, uint8 egr_if_credits_configured);

typedef int (*dnx_egr_db_egr_if_credits_configured_get_cb)(
    int unit, uint32 egr_if_credits_configured_idx_0, uint32 egr_if_credits_configured_idx_1, uint8 *egr_if_credits_configured);

typedef int (*dnx_egr_db_total_esb_queues_allocated_set_cb)(
    int unit, uint32 total_esb_queues_allocated_idx_0, int total_esb_queues_allocated);

typedef int (*dnx_egr_db_total_esb_queues_allocated_get_cb)(
    int unit, uint32 total_esb_queues_allocated_idx_0, int *total_esb_queues_allocated);

typedef int (*dnx_egr_db_nof_negative_compensation_ports_set_cb)(
    int unit, uint32 nof_negative_compensation_ports_idx_0, int nof_negative_compensation_ports);

typedef int (*dnx_egr_db_nof_negative_compensation_ports_get_cb)(
    int unit, uint32 nof_negative_compensation_ports_idx_0, int *nof_negative_compensation_ports);



typedef struct {
    dnx_egr_db_ps_allocation_bmap_set_cb set;
    dnx_egr_db_ps_allocation_bmap_get_cb get;
} dnx_egr_db_ps_allocation_bmap_cbs;

typedef struct {
    dnx_egr_db_ps_prio_mode_set_cb set;
    dnx_egr_db_ps_prio_mode_get_cb get;
} dnx_egr_db_ps_prio_mode_cbs;

typedef struct {
    dnx_egr_db_ps_if_idx_set_cb set;
    dnx_egr_db_ps_if_idx_get_cb get;
} dnx_egr_db_ps_if_idx_cbs;

typedef struct {
    dnx_egr_db_ps_allocation_bmap_cbs allocation_bmap;
    dnx_egr_db_ps_prio_mode_cbs prio_mode;
    dnx_egr_db_ps_if_idx_cbs if_idx;
} dnx_egr_db_ps_cbs;

typedef struct {
    dnx_egr_db_interface_occ_set_cb set;
    dnx_egr_db_interface_occ_get_cb get;
} dnx_egr_db_interface_occ_cbs;

typedef struct {
    dnx_egr_db_total_egr_if_credits_set_cb set;
    dnx_egr_db_total_egr_if_credits_get_cb get;
} dnx_egr_db_total_egr_if_credits_cbs;

typedef struct {
    dnx_egr_db_egr_if_credits_configured_set_cb set;
    dnx_egr_db_egr_if_credits_configured_get_cb get;
} dnx_egr_db_egr_if_credits_configured_cbs;

typedef struct {
    dnx_egr_db_total_esb_queues_allocated_set_cb set;
    dnx_egr_db_total_esb_queues_allocated_get_cb get;
} dnx_egr_db_total_esb_queues_allocated_cbs;

typedef struct {
    dnx_egr_db_nof_negative_compensation_ports_set_cb set;
    dnx_egr_db_nof_negative_compensation_ports_get_cb get;
} dnx_egr_db_nof_negative_compensation_ports_cbs;

typedef struct {
    dnx_egr_db_is_init_cb is_init;
    dnx_egr_db_init_cb init;
    dnx_egr_db_ps_cbs ps;
    dnx_egr_db_interface_occ_cbs interface_occ;
    dnx_egr_db_total_egr_if_credits_cbs total_egr_if_credits;
    dnx_egr_db_egr_if_credits_configured_cbs egr_if_credits_configured;
    dnx_egr_db_total_esb_queues_allocated_cbs total_esb_queues_allocated;
    dnx_egr_db_nof_negative_compensation_ports_cbs nof_negative_compensation_ports;
} dnx_egr_db_cbs;





extern dnx_egr_db_cbs dnx_egr_db;

#endif 

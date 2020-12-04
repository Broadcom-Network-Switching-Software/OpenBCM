/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE1600 DEFS H
 */
#ifndef _APPL_DIAG_DCMN_FABRIC_H_INCLUDED_
#define _APPL_DIAG_DCMN_FABRIC_H_INCLUDED_

#include <appl/diag/shell.h>

#include <appl/diag/dcmn/diag.h>

#include <bcm/fabric.h>

#define DIAG_DNX_FABRIC_THRESHOLDS_MAX 4

typedef struct diag_dnx_fabric_link_th_info_s {
    /*RX*/
    int th_types_rx_nof;
    bcm_fabric_link_threshold_type_t th_types_rx[DIAG_DNX_FABRIC_THRESHOLDS_MAX];
    uint32 values_rx[DIAG_DNX_FABRIC_THRESHOLDS_MAX];

    /*TX*/
    int th_types_tx_nof;
    bcm_fabric_link_threshold_type_t th_types_tx[DIAG_DNX_FABRIC_THRESHOLDS_MAX];
    uint32 values_tx[DIAG_DNX_FABRIC_THRESHOLDS_MAX];

    /*Middle stage*/
    int th_types_middle_nof;
    bcm_fabric_link_threshold_type_t th_types_middle[DIAG_DNX_FABRIC_THRESHOLDS_MAX];
    uint32 values_middle[DIAG_DNX_FABRIC_THRESHOLDS_MAX];
} diag_dnx_fabric_link_th_info_t;

typedef struct diag_dnx_fabric_link_config_s
{
    int enable;
    int speed;
    bcm_port_pcs_t pcs;
    int pcs_llfc_extract_cig;
    int pcs_error_detect;
    int pcs_llfc_low_latency;
    uint32 cl72;
    int pcp;
    bcm_fabric_link_remote_pipe_mapping_t pipe_mapping;
    uint32 rx_polarity;
    uint32 tx_polarity;
    uint32 lane_swap;
    int clk_freq;
} diag_dnx_fabric_link_config_t;

cmd_result_t diag_dnx_fabric_pack_usage(int unit, args_t *args);

cmd_result_t diag_dnx_fabric_reachability(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_connectivity(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_link(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_queues(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_thresholds(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_properties(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_link_config(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_mesh(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_traffic_profile(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_mesh_topology(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_rx_fifo(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_force(int unit, args_t *a);

void diag_dnx_fabric_reachability_usage(int unit);
void diag_dnx_fabric_connectivity_usage(int unit);
void diag_dnx_fabric_link_usage(int unit);
void diag_dnx_fabric_queues_usage(int unit);
void diag_dnx_fabric_thresholds_usage(int unit);
void diag_dnx_fabric_properties_usage(int unit);
void diag_dnx_fabric_link_config_usage(int unit);
void diag_dnx_fabric_mesh_usage(int unit);
void diag_dnx_fabric_traffic_profile_usage(int unit);
void diag_dnx_fabric_mesh_topology_usage(int unit);
void diag_dnx_fabric_rx_fifo_usage(int unit);
void diag_dnx_fabric_force_usage(int unit);

#endif /*!_APPL_DIAG_DCMN_FABRIC_H_INCLUDED_*/



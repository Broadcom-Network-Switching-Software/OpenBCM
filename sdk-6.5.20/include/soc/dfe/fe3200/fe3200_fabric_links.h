/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE3200 FABRIC LINKS H
 */
 
#ifndef _SOC_FE3200_FABRIC_LINKS_H_
#define _SOC_FE3200_FABRIC_LINKS_H_

#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dcmn/dcmn_defs.h>
#include <soc/error.h>
#include <soc/types.h>

soc_error_t soc_fe3200_fabric_links_cell_format_verify(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t val);
soc_error_t soc_fe3200_fabric_links_flow_status_control_cell_format_set(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t val);
soc_error_t soc_fe3200_fabric_links_flow_status_control_cell_format_get(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t *val);
soc_error_t soc_fe3200_fabric_links_weight_validate(int unit, int val);
soc_error_t soc_fe3200_fabric_links_weight_set(int unit, soc_port_t link, int is_prim, int val);
soc_error_t soc_fe3200_fabric_links_weight_get(int unit, soc_port_t link, int is_prim, int *val);
soc_error_t soc_fe3200_fabric_links_secondary_only_set(int unit, soc_port_t link, int val);
soc_error_t soc_fe3200_fabric_links_secondary_only_get(int unit, soc_port_t link, int *val);
soc_error_t soc_fe3200_fabric_link_repeater_enable_set(int unit, soc_port_t port, int enable, int empty_cell_size);
soc_error_t soc_fe3200_fabric_link_repeater_enable_get(int unit, soc_port_t port, int *enable, int *empty_cell_size);
soc_error_t soc_fe3200_fabric_links_pcp_enable_set(int unit, soc_port_t port, int enable);
soc_error_t soc_fe3200_fabric_links_pcp_enable_get(int unit, soc_port_t port, int *enable);
soc_error_t soc_fe3200_fabric_links_pipe_map_set(int unit, soc_port_t port, soc_dfe_fabric_link_remote_pipe_mapping_t pipe_map);
soc_error_t soc_fe3200_fabric_links_pipe_map_get(int unit, soc_port_t port, soc_dfe_fabric_link_remote_pipe_mapping_t *pipe_map);
soc_error_t soc_fe3200_fabric_links_repeater_nof_remote_pipe_set(int unit, int port, uint32 nof_remote_pipe);
soc_error_t soc_fe3200_fabric_links_repeater_nof_remote_pipe_get(int unit, int port, uint32 *nof_remote_pipe);
soc_error_t soc_fe3200_fabric_links_cell_format_get(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t *val);

#define SOC_FE3200_ASYMMETRICAL_FE13_QUAD_0 (26)
#define SOC_FE3200_ASYMMETRICAL_FE13_QUAD_1 (35)
#define SOC_FE3200_WFQ_WEIGHT_MAX      (127)

#endif


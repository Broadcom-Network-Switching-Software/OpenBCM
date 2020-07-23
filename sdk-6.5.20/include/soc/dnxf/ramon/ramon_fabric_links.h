/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RAMON FABRIC LINKS H
 */
 
#ifndef _SOC_RAMON_FABRIC_LINKS_H_
#define _SOC_RAMON_FABRIC_LINKS_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif 

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/error.h>
#include <soc/types.h>

shr_error_e soc_ramon_fabric_links_validate_link(int unit, soc_port_t link);
shr_error_e soc_ramon_fabric_links_flow_status_control_cell_format_set(int unit, soc_port_t link, soc_dnxf_fabric_link_cell_size_t val);
shr_error_e soc_ramon_fabric_links_flow_status_control_cell_format_get(int unit, soc_port_t link, soc_dnxf_fabric_link_cell_size_t *val);

shr_error_e soc_ramon_fabric_link_repeater_enable_set(int unit, soc_port_t port, int enable);
shr_error_e soc_ramon_fabric_link_repeater_enable_get(int unit, soc_port_t port, int *enable);
shr_error_e soc_ramon_fabric_links_pcp_enable_set(int unit, soc_port_t port, int enable);
shr_error_e soc_ramon_fabric_links_pcp_enable_get(int unit, soc_port_t port, int *enable);
shr_error_e soc_ramon_fabric_links_pipe_map_set(int unit, soc_port_t port, soc_dnxf_fabric_link_remote_pipe_mapping_t pipe_map);
shr_error_e soc_ramon_fabric_links_pipe_map_get(int unit, soc_port_t port, soc_dnxf_fabric_link_remote_pipe_mapping_t *pipe_map);
shr_error_e soc_ramon_fabric_links_link_integrity_down_set(int unit, soc_port_t port, int force_down);
shr_error_e soc_ramon_fabric_links_isolate_set(int unit, soc_port_t link, soc_dnxc_isolation_status_t val);
shr_error_e soc_ramon_fabric_links_isolate_get(int unit, soc_port_t link, soc_dnxc_isolation_status_t* val);
shr_error_e soc_ramon_fabric_links_llf_control_source_set(int unit, soc_port_t link, soc_dnxc_fabric_pipe_t val);
shr_error_e soc_ramon_fabric_links_llf_control_source_get(int unit, soc_port_t link, soc_dnxc_fabric_pipe_t* val);
shr_error_e soc_ramon_fabric_links_aldwp_config(int unit, soc_port_t port);
shr_error_e soc_ramon_fabric_links_aldwp_init(int unit);
shr_error_e soc_ramon_fabric_link_device_mode_get(int unit,soc_port_t port, int is_rx, soc_dnxf_fabric_link_device_mode_t *device_mode);
shr_error_e soc_ramon_fabric_link_retimer_calibrate(int unit, soc_port_t link_a, soc_port_t link_b);
shr_error_e soc_ramon_fabric_link_retimer_connect_set(int unit, soc_port_t link_a, soc_port_t link_b);
shr_error_e soc_ramon_fabric_link_retimer_connect_get(int unit, soc_port_t link_a, soc_port_t *link_b);

#define SOC_RAMON_ASYMMETRICAL_FE13_QUAD_0 (26)
#define SOC_RAMON_ASYMMETRICAL_FE13_QUAD_1 (35)
#define SOC_RAMON_WFQ_WEIGHT_MAX      (127)

#endif



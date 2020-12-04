/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE FABRIC H
 */
#ifndef _BCM_INT_DFE_FABRIC_H_
#define _BCM_INT_DFE_FABRIC_H_

#include <sal/types.h>
#include <bcm/types.h>

#include <bcm_int/dfe/dfe_fifo_type.h>
#include <bcm_int/dfe/dfe_multicast_id_map.h>

#include <soc/dcmn/fabric.h>

#include <soc/dfe/cmn/dfe_defs.h>


int _bcm_dfe_fabric_deinit(int unit);
int _bcm_dfe_fabric_link_flow_status_cell_format_set(int unit, bcm_port_t port, int cell_format);
int _bcm_dfe_fabric_link_flow_status_cell_format_get(int unit, bcm_port_t port, int *cell_format);
int _bcm_dfe_fabric_fifo_info_get(int unit, bcm_port_t port, bcm_fabric_pipe_t pipe_id, soc_dcmn_fabric_direction_t  direction, uint32 *fifo_type, bcm_fabric_pipe_t *pipe_get, uint32 *flags_get);
int _bcm_dfe_fabric_link_repeater_update(int unit, bcm_port_t port);
  
#endif /*_BCM_INT_DFE_FABRIC_H_*/

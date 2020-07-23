/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXF FABRIC SOURCE ROUTED CELL H
 */
 
#ifndef _SOC_DNXF_FABRIC_SOURCE_ROUTED_CELL_H_
#define _SOC_DNXF_FABRIC_SOURCE_ROUTED_CELL_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif


#include <bcm/fabric.h>
#include <soc/error.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxc/vsc256_fabric_cell.h>
#include <soc/dnxc/fabric.h>
#include <soc/dnxc/dnxc_fabric_source_routed_cell.h>



#define DNXF_CELL_NOF_BYTES_IN_UINT32                (4)
#define DNXF_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL    (3)







shr_error_e soc_dnxf_cpu2cpu_write(int unit, const dnxc_sr_cell_link_list_t  *sr_link_list, uint32 data_in_size, uint32 *data_in); 
shr_error_e soc_dnxf_route2sr_link_list(int unit, const soc_dnxc_fabric_route_t *route, dnxc_sr_cell_link_list_t *sr_link_list);

shr_error_e soc_dnxf_sr_cell_receive(int unit, dnxc_vsc256_sr_cell_t* cell);
shr_error_e soc_dnxf_sr_cell_payload_receive(int unit, uint32 flags, uint32 data_out_max_size, uint32 *data_out_size, uint32 *data_out);
shr_error_e soc_dnxf_sr_cell_send(int unit, uint32 flags, soc_dnxc_fabric_route_t* route, uint32 data_in_size, uint32 *data_in);


#endif 

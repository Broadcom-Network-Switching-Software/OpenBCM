/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE FABRIC SOURCE ROUTED CELL H
 */
 
#ifndef _SOC_DFE_FABRIC_SOURCE_ROUTED_CELL_H_
#define _SOC_DFE_FABRIC_SOURCE_ROUTED_CELL_H_


#include <bcm/fabric.h>
#include <soc/error.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dcmn/vsc256_fabric_cell.h>
#include <soc/dcmn/fabric.h>
#include <soc/dcmn/dcmn_fabric_source_routed_cell.h>



#define DFE_CELL_NOF_BYTES_IN_UINT32                (4)
#define DFE_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL    (3)







soc_error_t soc_dfe_cpu2cpu_write(int unit, const dcmn_sr_cell_link_list_t  *sr_link_list, uint32 data_in_size, uint32 *data_in); 
soc_error_t soc_dfe_route2sr_link_list(int unit, const soc_dcmn_fabric_route_t *route, dcmn_sr_cell_link_list_t *sr_link_list);

soc_error_t soc_dfe_sr_cell_format_type_get(int unit,  const vsc256_sr_cell_t* cell, soc_dfe_fabric_link_cell_size_t* vsc_format);
soc_error_t soc_dfe_sr_cell_receive(int unit, vsc256_sr_cell_t* cell);
soc_error_t soc_dfe_sr_cell_payload_receive(int unit, uint32 flags, uint32 data_out_max_size, uint32 *data_out_size, uint32 *data_out);
soc_error_t soc_dfe_sr_cell_send(int unit, uint32 flags, soc_dcmn_fabric_route_t* route, uint32 data_in_size, uint32 *data_in);


#endif 

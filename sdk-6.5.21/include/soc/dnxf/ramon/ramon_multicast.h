/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RAMON MULTICAST H
 */
 
#ifndef _SOC_RAMON_MULTICAST_H_
#define _SOC_RAMON_MULTICAST_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif 

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/error.h>

shr_error_e soc_ramon_multicast_mode_get(int unit, soc_dnxf_multicast_table_mode_t* multicast_mode);
shr_error_e soc_ramon_multicast_table_size_get(int unit, uint32* mc_table_size);
shr_error_e soc_ramon_multicast_table_entry_size_get(int unit, uint32* entry_size);
shr_error_e soc_ramon_multicast_egress_add(int unit, soc_multicast_t group, soc_gport_t port);
shr_error_e soc_ramon_multicast_egress_delete(int unit, soc_multicast_t group, soc_gport_t port);
shr_error_e soc_ramon_multicast_egress_delete_all(int unit, soc_multicast_t group);
shr_error_e soc_ramon_multicast_egress_get(int unit, soc_multicast_t group, int port_max, soc_gport_t *port_array, int *port_count);
shr_error_e soc_ramon_multicast_egress_set(int unit, soc_multicast_t group, int port_count, soc_gport_t *port_array);

#endif


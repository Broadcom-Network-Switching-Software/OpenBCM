/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __QAX_FABRIC_WITH_PACKET_TDM_INCLUDED__

#define __QAX_FABRIC_WITH_PACKET_TDM_INCLUDED__


#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/error.h>















int qax_fabric_with_packet_tdm_loopback_enable(int unit, int quad, int enable);
int qax_fabric_with_packet_tdm_link_get(int unit, int quad, int* link);
int qax_fabric_with_packet_tdm_enable_set(int unit, int quad, int enable);
int qax_fabric_with_packet_tdm_enable_get(int unit, int quad, int *enable);
int qax_fabric_with_packet_tdm_attach(int unit, int quad);



soc_error_t qax_fabric_with_packet_tdm_port_probe(int unit, int quad);



#include <soc/dpp/SAND/Utils/sand_footer.h>
#endif


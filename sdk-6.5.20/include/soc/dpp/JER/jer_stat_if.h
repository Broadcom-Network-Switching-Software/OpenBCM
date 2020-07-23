/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_stat_if.h
 */

#ifndef __JER_STAT_IF_INCLUDED__

#define __JER_STAT_IF_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/ARAD/arad_api_stat_if.h>

uint32 soc_jer_stat_if_init_set(int unit, ARAD_INIT_STAT_IF *stat_if);
uint32 soc_jer_stat_if_report_info_set(int unit, ARAD_INIT_STAT_IF *stat_if);
soc_error_t soc_jer_stat_if_queue_range_set(int unit, int core, int is_scrubber, int min_queue, int max_queue);
soc_error_t soc_jer_stat_if_queue_range_get(int unit, int core, int is_scrubber, int* min_queue, int* max_queue);
soc_error_t soc_jer_stat_if_billing_filter_egress_port_set(int unit, int egr_port, int enable);

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 


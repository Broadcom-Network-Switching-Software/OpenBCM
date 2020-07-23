/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_stat_if.h
 */

#ifndef __QAC_STAT_IF_INCLUDED__

#define __QAX_STAT_IF_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/ARAD/arad_api_stat_if.h>

#define QAX_STAT_IF_REPORT_DESC_THRESHOLD_MAX (0x7F0000)
#define QAX_STAT_IF_REPORT_THRESHOLD_IGNORED_VALUE (0xFFF)
#define QAX_SCRUBBER_TRESHOLDS_MANTISSA_NOF_BITS (8)
#define QAX_SCRUBBER_TRESHOLDS_EXP_NOF_BITS (8)
#define QAX_REPORT_QUEUE_SIZE_MAX (0x7FFF)

#define QUX_REPORT_QUEUE_SIZE_MAX (0x3FFF)

uint32 soc_qax_stat_if_init_set(int unit, ARAD_INIT_STAT_IF *stat_if);
uint32 soc_qax_stat_if_report_info_set(int unit, int nof_sif_ports, ARAD_INIT_STAT_IF *stat_if);
soc_error_t soc_qax_stat_if_queue_range_set(int unit, int core, int is_scrubber, int min_queue, int max_queue);
soc_error_t soc_qax_stat_if_queue_range_get(int unit, int core, int is_scrubber, int* min_queue, int* max_queue);

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 


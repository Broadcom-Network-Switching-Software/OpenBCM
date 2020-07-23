/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_STAT_IF_INCLUDED__

#define __ARAD_STAT_IF_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_stat_if.h>





#define ARAD_STAT_IF_REPORT_THRESHOLD_LINES                      15
#define ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MIN                   ((96*1024)-1)
#define ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MAX                   ((96*1024)-1) 
#define ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED                    0xFFFFFFFF
#define ARAD_STAT_IF_REPORT_SCRUBBER_DISABLE                     0
#define ARAD_STAT_IF_REPORT_WC_ID                                7

#ifdef BCM_88650_B0
#define ARAD_STAT_IF_REPORT_QSIZE_QUEUE_MIN                   (0)
#define ARAD_STAT_IF_REPORT_QSIZE_QUEUE_MAX                   ((96*1024)-1) 
#endif

#define JER_STAT_IF_REPORT_QSIZE_QUEUE_MAX                      ((64*1024)-1)
#define QAX_STAT_IF_REPORT_QSIZE_QUEUE_MAX                      ((32*1024)-1)
#define ARAD_STAT_IF_REPORT_BILLING_NO_IDLE_PERIOD               0
#define ARAD_STAT_IF_REPORT_BILLING_MAX_IDLE_PERIOD              0x0FF
#define ARAD_STAT_IF_REPORT_QSIZE_NO_IDLE_PERIOD                 0
#define ARAD_STAT_IF_REPORT_QSIZE_MAX_IDLE_PERIOD                0x03F

#define ARAD_STAT_IF_REPORT_DESC_THRESHOLD_MAX                   0x3fffff
#define ARAD_STAT_IF_REPORT_BDB_THRESHOLD_MAX                    0x7ffff
#define ARAD_STAT_IF_REPORT_UC_DRAM_THRESHOLD_MAX                0x3fffff























uint32
  arad_stat_if_init(
    SOC_SAND_IN  int                 unit
  );





uint32
  arad_stat_if_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_STAT_IF_INFO        *info
  );



uint32
  arad_stat_if_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_STAT_IF_INFO        *info
  );



uint32
  arad_stat_if_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_STAT_IF_INFO        *info
  );



uint32
  arad_stat_if_report_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_STAT_IF_REPORT_INFO *info
  );



uint32
  arad_stat_if_report_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_STAT_IF_REPORT_INFO *info
  );



uint32
  arad_stat_if_report_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_STAT_IF_REPORT_INFO *info
  );

uint32
 arad_stat_if_rate_limit_prd_get(
     SOC_SAND_IN  int                  unit,
     SOC_SAND_IN  ARAD_STAT_IF_REPORT_INFO   *info,
     SOC_SAND_OUT uint32                   *limit_prd_val
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


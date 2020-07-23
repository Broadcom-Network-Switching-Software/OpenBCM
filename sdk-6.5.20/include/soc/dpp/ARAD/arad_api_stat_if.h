/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_API_STAT_IF_INCLUDED__

#define __ARAD_API_STAT_IF_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_stat_if.h>
















#define ARAD_STAT_IF_PHASE_000                            SOC_TMC_STAT_IF_PHASE_000
#define ARAD_STAT_IF_PHASE_090                            SOC_TMC_STAT_IF_PHASE_090
#define ARAD_STAT_IF_PHASE_180                            SOC_TMC_STAT_IF_PHASE_180
#define ARAD_STAT_IF_PHASE_270                            SOC_TMC_STAT_IF_PHASE_270
#define ARAD_STAT_NOF_IF_PHASES                           SOC_TMC_STAT_NOF_IF_PHASES
typedef SOC_TMC_STAT_IF_PHASE                                  ARAD_STAT_IF_PHASE;

#define ARAD_STAT_IF_REPORT_MODE_BILLING                  SOC_TMC_STAT_IF_REPORT_MODE_BILLING
#define ARAD_STAT_IF_REPORT_MODE_FAP20V                   SOC_TMC_STAT_IF_REPORT_MODE_FAP20V
#define ARAD_STAT_NOF_IF_REPORT_MODES                     SOC_TMC_STAT_NOF_IF_REPORT_MODES
typedef SOC_TMC_STAT_IF_REPORT_MODE                            ARAD_STAT_IF_REPORT_MODE;

#define ARAD_STAT_IF_BILLING_MODE_EGR_Q_NUM               SOC_TMC_STAT_IF_BILLING_MODE_EGR_Q_NUM
#define ARAD_STAT_IF_BILLING_MODE_CUD                     SOC_TMC_STAT_IF_BILLING_MODE_CUD
#define ARAD_STAT_IF_BILLING_MODE_VSI_VLAN                SOC_TMC_STAT_IF_BILLING_MODE_VSI_VLAN
#define ARAD_STAT_IF_BILLING_MODE_BOTH_LIFS               SOC_TMC_STAT_IF_BILLING_MODE_BOTH_LIFS
typedef SOC_TMC_STAT_IF_BILLING_MODE                           ARAD_STAT_IF_BILLING_MODE;

#define ARAD_STAT_IF_FAP20V_MODE_Q_SIZE                   SOC_TMC_STAT_IF_FAP20V_MODE_Q_SIZE
#define ARAD_STAT_IF_FAP20V_MODE_PKT_SIZE                 SOC_TMC_STAT_IF_FAP20V_MODE_PKT_SIZE
typedef SOC_TMC_STAT_IF_FAP20V_MODE                            ARAD_STAT_IF_FAP20V_MODE;

#define ARAD_STAT_IF_MC_MODE_Q_NUM                        SOC_TMC_STAT_IF_MC_MODE_Q_NUM
#define ARAD_STAT_IF_MC_MODE_MC_ID                        SOC_TMC_STAT_IF_MC_MODE_MC_ID
typedef SOC_TMC_STAT_IF_MC_MODE                                ARAD_STAT_IF_MC_MODE;

#define ARAD_STAT_IF_CNM_MODE_DIS                         SOC_TMC_STAT_IF_CNM_MODE_DIS
#define ARAD_STAT_IF_CNM_MODE_EN                          SOC_TMC_STAT_IF_CNM_MODE_EN
typedef SOC_TMC_STAT_IF_CNM_MODE                               ARAD_STAT_IF_CNM_MODE;

#define ARAD_STAT_IF_PARITY_MODE_DIS                      SOC_TMC_STAT_IF_PARITY_MODE_DIS
#define ARAD_STAT_IF_PARITY_MODE_EN                       SOC_TMC_STAT_IF_PARITY_MODE_EN
typedef SOC_TMC_STAT_IF_PARITY_MODE                            ARAD_STAT_IF_PARITY_MODE;

#define ARAD_STAT_IF_BIST_EN_MODE_DIS                     SOC_TMC_STAT_IF_BIST_EN_MODE_DIS
#define ARAD_STAT_IF_BIST_EN_MODE_PATTERN                 SOC_TMC_STAT_IF_BIST_EN_MODE_PATTERN
#define ARAD_STAT_IF_BIST_EN_MODE_WALKING_ONE             SOC_TMC_STAT_IF_BIST_EN_MODE_WALKING_ONE
#define ARAD_STAT_IF_BIST_EN_MODE_PRBS                    SOC_TMC_STAT_IF_BIST_EN_MODE_PRBS
typedef SOC_TMC_STAT_IF_BIST_EN_MODE                           ARAD_STAT_IF_BIST_EN_MODE;

typedef SOC_TMC_STAT_IF_INFO                                   ARAD_STAT_IF_INFO;
typedef SOC_TMC_STAT_IF_BILLING_INFO                           ARAD_STAT_IF_REPORT_MODE_BILLING_INFO;
typedef SOC_TMC_STAT_IF_FAP20V_INFO                            ARAD_STAT_IF_REPORT_MODE_FAP20V_INFO;
typedef SOC_TMC_STAT_IF_REPORT_MODE_INFO                       ARAD_STAT_IF_REPORT_MODE_INFO;
typedef SOC_TMC_STAT_IF_REPORT_INFO                            ARAD_STAT_IF_REPORT_INFO;
typedef SOC_TMC_STAT_IF_BIST_PATTERN                           ARAD_STAT_IF_BIST_PATTERN;
typedef SOC_TMC_STAT_IF_BIST_INFO                              ARAD_STAT_IF_BIST_INFO;

#define ARAD_STAT_NOF_IF_BILLING_MODES                   SOC_TMC_STAT_NOF_IF_BILLING_MODES;
#define ARAD_STAT_NOF_IF_FAP20V_MODES                    SOC_TMC_STAT_NOF_IF_FAP20V_MODES;













uint32
  arad_stat_if_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_STAT_IF_INFO        *info
  );



uint32
  arad_stat_if_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_STAT_IF_INFO        *info
  );



uint32
  arad_stat_if_report_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_STAT_IF_REPORT_INFO *info
  );



uint32
  arad_stat_if_report_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_STAT_IF_REPORT_INFO *info
  );

void
  arad_ARAD_STAT_IF_INFO_clear(
    SOC_SAND_OUT ARAD_STAT_IF_INFO *info
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


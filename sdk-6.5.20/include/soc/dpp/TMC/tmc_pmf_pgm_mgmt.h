/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_PMF_PGM_MGMT_INCLUDED__

#define __SOC_TMC_PMF_PGM_MGMT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/TMC/tmc_api_ports.h>













typedef enum
{
  
  SOC_TMC_FP_PKT_HDR_TYPE_RAW = 0x1,
  
  SOC_TMC_FP_PKT_HDR_TYPE_FTMH = 0x2,
  
  SOC_TMC_FP_PKT_HDR_TYPE_TM = 0x4,
  
  SOC_TMC_FP_PKT_HDR_TYPE_TM_IS = 0x8,
  
  SOC_TMC_FP_PKT_HDR_TYPE_TM_PPH = 0x10,
  
  SOC_TMC_FP_PKT_HDR_TYPE_TM_IS_PPH = 0x20,
  
  SOC_TMC_FP_PKT_HDR_TYPE_ETH = 0x40,
  
  SOC_TMC_FP_PKT_HDR_TYPE_ETH_ETH = 0x80,
  
  SOC_TMC_FP_PKT_HDR_TYPE_IPV4_ETH = 0x100,
  
  SOC_TMC_FP_PKT_HDR_TYPE_IPV6_ETH = 0x200,
  
  SOC_TMC_FP_PKT_HDR_TYPE_MPLS1_ETH = 0x400,
  
  SOC_TMC_FP_PKT_HDR_TYPE_MPLS2_ETH = 0x800,
  
  SOC_TMC_FP_PKT_HDR_TYPE_MPLS3_ETH = 0x1000,
  
  SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH = 0x2000,
  
  SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH = 0x4000,
  
  SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH = 0x8000,
  
  SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH = 0x10000,
  
  SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH = 0x20000,
  
  SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH = 0x40000,
  
  SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH = 0x80000,
  
  SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH = 0x100000,
  
  SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH = 0x200000,
  
  SOC_TMC_FP_PKT_HDR_TYPE_IPV4_IPV4_ETH = 0x400000,
  
  SOC_TMC_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH = 0x800000,
  
  SOC_TMC_FP_PKT_HDR_TYPE_ETH_TRILL_ETH = 0x1000000,
  
  SOC_TMC_FP_PKT_HDR_TYPE_ETH_IPV4_ETH = 0x2000000,
  
  SOC_TMC_FP_PKT_HDR_TYPE_FC_ETH = 0x4000000,
  
  SOC_TMC_NOF_FP_PKT_HDR_TYPES = 25
}SOC_TMC_FP_PKT_HDR_TYPE;











#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


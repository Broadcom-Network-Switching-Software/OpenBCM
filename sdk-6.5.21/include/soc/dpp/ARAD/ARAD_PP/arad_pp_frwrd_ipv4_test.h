/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_FRWRD_IPV4_TEST_INCLUDED__

#define __ARAD_PP_FRWRD_IPV4_TEST_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_frwrd_ipv4.h>














typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 vrf;
  
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY key;

} ARAD_PP_DIAG_IPV4_TEST_VPN_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  ARAD_PP_DIAG_IPV4_TEST_VPN_KEY key;

} ARAD_PP_FRWRD_IPV4_TEST_LKUP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 seed;
  
  uint32 nof_routes;
  
  uint32 cache_change;
  
  uint32 nof_cached;
  
  uint32 defragment_rate;
  
  uint32 defragment_banks_bmp;
  
  uint8 test;
  
  uint8 hw_test;
  
  uint32 print_level;
  
  uint32 remove_prob;
  
  uint32 add_prob;
  
  uint32 fec_id;

} ARAD_PP_FRWRD_IPV4_TEST_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 vrf;

} ARAD_PP_FRWRD_IPV4_CLEAR_INFO;











uint32 
  arad_pp_frwrd_ipv4_test_init(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN ARAD_PP_FRWRD_IPV4_CLEAR_INFO *reset_info
  );


uint32
  arad_pp_frwrd_ipv4_test_clear_vrf(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_CLEAR_INFO *clear_info
  );



uint32 
  arad_pp_frwrd_ipv4_test_lpm_lkup_get(
    SOC_SAND_IN int                               unit,
    SOC_SAND_IN  ARAD_PP_DIAG_IPV4_TEST_VPN_KEY      *lpm_key,
    SOC_SAND_OUT  uint32                             *fec_ptr,
    SOC_SAND_OUT  uint8                              *found
  );


#if SOC_PPC_DEBUG_IS_LVL1

uint32
  arad_pp_frwrd_ipv4_test_print_mem(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  bnk_bmp,
    SOC_SAND_IN  uint32                  print_level
  );
#endif

uint32
  arad_pp_frwrd_ipv4_test_lookup(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_TEST_LKUP_INFO  *lkup_info
  );


uint32
  arad_pp_frwrd_ipv4_test_vrf(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  vrf_ndx,
    SOC_SAND_IN  uint32                  nof_iterations
  );
#if SOC_PPC_DEBUG_IS_LVL1

uint32
  arad_pp_frwrd_ipv4_test_run(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_TEST_INFO  *tes_info
  );
#endif

void
  ARAD_PP_FRWRD_IPV4_TEST_LKUP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_IPV4_TEST_LKUP_INFO *info
  );

void
  ARAD_PP_FRWRD_IPV4_TEST_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_IPV4_TEST_INFO *info
  );

void
  ARAD_PP_FRWRD_IPV4_CLEAR_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_IPV4_CLEAR_INFO *info
  );

void
  ARAD_PP_DIAG_IPV4_TEST_VPN_KEY_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_IPV4_TEST_VPN_KEY *info
  );

#if SOC_PPC_DEBUG_IS_LVL1
void
  ARAD_PP_FRWRD_IPV4_TEST_LKUP_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_TEST_LKUP_INFO *info
  );

void
  ARAD_PP_FRWRD_IPV4_TEST_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_TEST_INFO *info
  );

void
  ARAD_PP_FRWRD_IPV4_CLEAR_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_CLEAR_INFO *info
  );

void
  ARAD_PP_DIAG_IPV4_TEST_VPN_KEY_print(
    SOC_SAND_IN  ARAD_PP_DIAG_IPV4_TEST_VPN_KEY *info
  );

#endif



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


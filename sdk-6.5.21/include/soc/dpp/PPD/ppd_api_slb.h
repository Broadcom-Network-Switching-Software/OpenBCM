/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if !defined(__PPD_API_SLB_INCLUDED__) && !defined(PPD_API_SLB_INTERNAL_FUNCTIONS_ONLY)

#define __PPD_API_SLB_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_slb.h>

#include <soc/dpp/PPD/ppd_api_general.h>





uint32 
  soc_ppd_slb_set_global_cfg(
    SOC_SAND_IN     int                                    unit,
    SOC_SAND_IN     SOC_PPC_SLB_CONFIGURATION_ITEM *          configuration_item
  );

uint32 
  soc_ppd_slb_get_global_cfg(
    SOC_SAND_IN     int                                    unit,
    SOC_SAND_INOUT  SOC_PPC_SLB_CONFIGURATION_ITEM *          configuration_item
  );


uint32 
  soc_ppd_slb_traverse(
    SOC_SAND_IN     int                                    unit,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_MATCH_RULE * const * match_rules,
    SOC_SAND_IN     uint32                                    nof_match_rules,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_ACTION *             action,
    SOC_SAND_OUT    uint32 *                                  nof_matched_entries
  );


uint32
  soc_ppd_slb_get_block(
    SOC_SAND_IN     int                                    unit,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_MATCH_RULE * const * match_rules,
    SOC_SAND_IN     uint32                                    nof_match_rules,
    SOC_SAND_INOUT  SOC_SAND_TABLE_BLOCK_RANGE *              block_range,
    SOC_SAND_OUT    SOC_PPC_SLB_ENTRY_KEY *                   slb_keys,
    SOC_SAND_OUT    SOC_PPC_SLB_ENTRY_VALUE *                 slb_vals,
    SOC_SAND_OUT    uint32 *                                  nof_entries
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>

#elif defined(PPD_API_SLB_INTERNAL_FUNCTIONS_ONLY)

#ifndef PPD_API_SLB_INTERNAL_STD_IMPL
#error "PPD_API_SLB_INTERNAL_STD_IMPL must be defined."
#endif

#ifndef PPD_API_SLB_INTERNAL_PREFIX
#error "PPD_API_SLB_INTERNAL_PREFIX must be defined"
#endif

uint32 
  PPD_API_SLB_INTERNAL_PREFIX(slb_set_global_cfg)(
    SOC_SAND_IN     int                                    unit,
    SOC_SAND_IN     SOC_PPC_SLB_CONFIGURATION_ITEM *          configuration_item
  )
PPD_API_SLB_INTERNAL_STD_IMPL(slb_set_global_cfg, ( unit, configuration_item))

uint32 
  PPD_API_SLB_INTERNAL_PREFIX(slb_get_global_cfg)(
    SOC_SAND_IN     int                                    unit,
    SOC_SAND_INOUT  SOC_PPC_SLB_CONFIGURATION_ITEM *          configuration_item
  )
PPD_API_SLB_INTERNAL_STD_IMPL(slb_get_global_cfg, (unit, configuration_item))

uint32 
  PPD_API_SLB_INTERNAL_PREFIX(slb_traverse)(
    SOC_SAND_IN     int                                    unit,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_MATCH_RULE * const * match_rules,
    SOC_SAND_IN     uint32                                    nof_match_rules,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_ACTION *             action,
    SOC_SAND_OUT    uint32 *                                  nof_matched_entries
  )
PPD_API_SLB_INTERNAL_STD_IMPL(slb_traverse, (unit, match_rules, nof_match_rules, action, nof_matched_entries))

uint32
  PPD_API_SLB_INTERNAL_PREFIX(slb_get_block)(
    SOC_SAND_IN     int                                    unit,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_MATCH_RULE * const * match_rules,
    SOC_SAND_IN     uint32                                    nof_match_rules,
    SOC_SAND_INOUT  SOC_SAND_TABLE_BLOCK_RANGE *              block_range,
    SOC_SAND_OUT    SOC_PPC_SLB_ENTRY_KEY *                   slb_keys,
    SOC_SAND_OUT    SOC_PPC_SLB_ENTRY_VALUE *                 slb_vals,
    SOC_SAND_OUT    uint32 *                                  nof_entries
  )
PPD_API_SLB_INTERNAL_STD_IMPL(slb_get_block, (unit, match_rules, nof_match_rules, block_range, slb_keys, slb_vals, nof_entries))

#endif 


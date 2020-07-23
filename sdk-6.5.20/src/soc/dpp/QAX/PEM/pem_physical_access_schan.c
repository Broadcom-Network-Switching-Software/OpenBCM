/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * pemla S-Channel (internal command bus) support for qux
 */
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/core/boot.h>
#include <sal/core/dpc.h>

#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/hwstate/hw_log.h>
#include <soc/error.h>
#include <soc/cmic.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include <soc/schanmsg_internal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>

#include "pem_physical_access.h" 
#include <string.h>
#include <assert.h>
#include <stdlib.h>


int phy_pem_mem_write(int unit, phy_mem_t *mem, void *entry_data)
{
  const unsigned int context_mask = 0x1F;
  unsigned int orig_context;
  unsigned int new_mem_address;
  int is_mem ;
  uint32 addr, data_longs_to_write ;
  uint32 address_mask = 0xFFFF0000 ;

  SOCDNX_INIT_FUNC_DEFS;

  
  is_mem = ( (mem->mem_address & address_mask) != 0 ) ? 1 : 0;
  data_longs_to_write = BITS2WORDS(mem->mem_width_in_bits) ;
  addr = mem->mem_address + mem->row_index;

  
  if (is_mem && ((addr < 0x9E0000) || ((addr > 0xC5FFFF) && (addr < 0x1940000)) || (addr > 0x1C5FFFF))) 
  {
      ARAD_PP_FLP_PROG_NS_INFO ns_info;
      unsigned int j;
      orig_context = addr & context_mask;
      
      if (orig_context < ARAD_PP_FLP_PROG_NOF) {
          SOCDNX_IF_ERR_EXIT(arad_pp_flp_program_get_namespace_info(0, orig_context, &ns_info));
          
          for (j = 0; j < ns_info.count; ++j){
              new_mem_address = ((addr & ~context_mask) | ns_info.hw_prog[j]);
              SOCDNX_IF_ERR_EXIT(soc_direct_memreg_set(unit, mem->block_identifier, new_mem_address, data_longs_to_write, is_mem, (uint32 *)entry_data));
          }
      }
      goto exit;
  }
  
  SOCDNX_IF_ERR_EXIT(soc_direct_memreg_set(unit, mem->block_identifier, addr, data_longs_to_write, is_mem, (uint32 *)entry_data));

exit:
  SOCDNX_FUNC_RETURN;
}


int phy_pem_mem_read(int unit, phy_mem_t *mem, void *entry_data)
{
    const unsigned int context_mask = 0x1F;
    unsigned int orig_context;
    unsigned int new_mem_address;
    int is_mem ;
    uint32 addr, data_longs_to_read;
    uint32 address_mask = 0xFFFF0000;

    SOCDNX_INIT_FUNC_DEFS;

    
    is_mem = ( (mem->mem_address & address_mask) != 0 ) ? 1 : 0;
    data_longs_to_read = BITS2WORDS(mem->mem_width_in_bits) ;
    addr = mem->mem_address  + mem->row_index;

    
    if (is_mem && ((addr < 0x9E0000) || ((addr > 0xC5FFFF) && (addr < 0x1940000)) || (addr > 0x1C5FFFF)))
    {
      ARAD_PP_FLP_PROG_NS_INFO ns_info;
      orig_context = addr & context_mask;

      if (orig_context < ARAD_PP_FLP_PROG_NOF) {
          SOCDNX_IF_ERR_EXIT(arad_pp_flp_program_get_namespace_info(0, orig_context, &ns_info));
      
          new_mem_address = ((addr & ~context_mask) | ns_info.hw_prog[0]);
          SOCDNX_IF_ERR_EXIT(soc_direct_memreg_get(unit, mem->block_identifier, new_mem_address, data_longs_to_read, is_mem, (uint32 *)entry_data));
          goto exit;
      }      
    }
    
    SOCDNX_IF_ERR_EXIT(soc_direct_memreg_get(unit, mem->block_identifier, addr, data_longs_to_read, is_mem, (uint32 *)entry_data));

exit:
    SOCDNX_FUNC_RETURN;
}





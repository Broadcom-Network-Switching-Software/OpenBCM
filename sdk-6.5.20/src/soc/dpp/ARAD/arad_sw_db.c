#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_SWDB



#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/intr.h>
#include <soc/error.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_api_dram.h>



#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>

#include <soc/shared/sat.h>

#if defined(INCLUDE_KBP)
#include <soc/dpp/ARAD/arad_kbp.h>
#endif 
#include <soc/dpp/QAX/qax_multicast_imp.h>







#define ARAD_SW_DB_MULTISET_MAX_VAL_NOF_BYTES  (16)

#define ARAD_SW_DB_DRAM_DELETED_BUFF_NONE 0xffffffff

#define ARAD_SW_1ST_AVAILABLE_HW_QUEUE SOC_TMC_ITM_NOF_QT_STATIC
#define ARAD_SW_NOF_AVAILABLE_HW_QUEUE SOC_TMC_ITM_NOF_QT_NDXS_ARAD
#define ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit) (SOC_IS_JERICHO(unit) ? SOC_TMC_ITM_NOF_QT_STATIC :  2)

#define ARAD_SW_DB_QUEUE_TYPE_IS_DYNAMIC(unit, user_q_type) \
        (user_q_type >= ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit) && user_q_type < ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit) + ARAD_SW_DB_NOF_LEGAL_DYNAMIC_QUEUE_TYPES(unit))

#define ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_VALID   0x1
#define ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_SW_ONLY 0x2

uint8 Arad_sw_db_initialized = FALSE;












uint32
  arad_sw_db_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_SW_DB_INIT);

  SAL_GLOBAL_LOCK;
  if (Arad_sw_db_initialized)
  {
    goto exit;
  }
  for (unit = 0; unit < SOC_SAND_MAX_DEVICE; ++unit)
  {
    Arad_sw_db.arad_device_sw_db[unit] = NULL;
  }

  Arad_sw_db_initialized = TRUE;

exit:
  SAL_GLOBAL_UNLOCK;
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_init()",0,0);
}

void
  arad_sw_db_close(void)
{
  Arad_sw_db_initialized = FALSE;
}





uint32
  arad_sw_db_dev_egr_ports_init(
    SOC_SAND_IN int      unit
  )
{
  uint32
    res; 
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;
  int core;
  SOC_SAND_OCC_BM_PTR occ_bm_ptr;
     
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_DEV_EGR_PORTS_INITIALIZE);

  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);

  
  
  btmp_init_info.size = ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB - 1;


  
  res = soc_sand_occ_bm_create(
          unit,
          &btmp_init_info,
          &occ_bm_ptr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.chanif2chan_arb_occ.set(unit, occ_bm_ptr);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);

  btmp_init_info.size = ARAD_EGQ_NOF_IFCS - (ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB-1);

  
  res = soc_sand_occ_bm_create(
          unit,
          &btmp_init_info,
          &occ_bm_ptr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.nonchanif2sch_offset_occ.set(unit, occ_bm_ptr);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);

  

  
  
  
  for (core = 0; core < SOC_DPP_DEFS_MAX(NOF_CORES); core++) {

      btmp_init_info.size = SOC_DPP_DEFS_MAX(NOF_CHANNELIZED_CALENDARS);
      if ((core == 0) || (core == 1)) {
      } else {
          
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 57, exit);
      }

      
      res = soc_sand_occ_bm_create(
              unit,
              &btmp_init_info,
              &occ_bm_ptr
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);
      res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.channelized_cals_occ.set(unit, core, occ_bm_ptr);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);
  }

  

  
  
  
  for (core = 0; core < SOC_DPP_DEFS_MAX(NOF_CORES); core++) {

      btmp_init_info.size = SOC_DPP_DEFS_MAX(NOF_CHANNELIZED_CALENDARS);
      if ((core == 0) || (core == 1)) {
      } else {
          
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 57, exit);
      }

      
      res = soc_sand_occ_bm_create(
              unit,
              &btmp_init_info,
              &occ_bm_ptr
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);
      res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.modified_channelized_cals_occ.set(unit, core, occ_bm_ptr);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);
  }

  

  
  
  
  for (core = 0; core < SOC_DPP_DEFS_MAX(NOF_CORES); core++) {

      btmp_init_info.size = SOC_DPP_IMP_DEFS_MAX(NOF_CORE_INTERFACES);
      if ((core == 0) || (core == 1)) {
      } else {
          
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 59, exit);
      }

      
      res = soc_sand_occ_bm_create(
              unit,
              &btmp_init_info,
              &occ_bm_ptr
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.e2e_interfaces_occ.set(unit, core, occ_bm_ptr);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);
  }

  

  
  
  
  for (core = 0; core < SOC_DPP_DEFS_MAX(NOF_CORES); core++) {

      btmp_init_info.size = SOC_DPP_IMP_DEFS_MAX(NOF_CORE_INTERFACES);
      if ((core == 0) || (core == 1)) {
      } else {
          
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 61, exit);
      }

      
      res = soc_sand_occ_bm_create(
              unit,
              &btmp_init_info,
              &occ_bm_ptr
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);
      res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.modified_e2e_interfaces_occ.set(unit, core, occ_bm_ptr);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_dev_egr_ports_init()",0,0);
}

uint32
  arad_sw_db_reassembly_context_init(
    SOC_SAND_IN int      unit
  )
{
  uint32
    res; 
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;
  SOC_SAND_OCC_BM_PTR
    occ_bm_ptr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_REASSEMBLY_CONTEXT_INITIALIZE);

  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  
  
  btmp_init_info.size = SOC_DPP_DEFS_GET(unit, num_of_reassembly_context);

  if (SOC_IS_QUX(unit)) {
    
    btmp_init_info.size  -= 1;
  }

  
  res = soc_sand_occ_bm_create(
          unit,
          &btmp_init_info,
          &occ_bm_ptr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.reassembly_ctxt.alloc(unit);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.reassembly_ctxt.reassembly_ctxt_occ.set(unit, occ_bm_ptr);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_reassembly_context_init()",0,0);
}

uint32
  arad_sw_db_egr_ports_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core,
    SOC_SAND_IN  uint32                     base_q_pair,
    SOC_SAND_OUT ARAD_SW_DB_DEV_EGR_RATE    *val
  )
{
  soc_error_t rv;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
  }
  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
  }

  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.get(unit, use_core, base_q_pair, val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sch_port_rate_get()",0,0);
}

uint32
  arad_sw_db_sch_port_rate_get(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            base_q_pair,
   SOC_SAND_OUT uint32            *rate
   )
{
  soc_error_t rv;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
  }
  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
  }
  
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.sch_rates.get(unit, use_core, base_q_pair, rate);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sch_port_rate_get()",0,0);
}

uint32
  arad_sw_db_sch_port_rate_set(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            base_q_pair,
   SOC_SAND_IN  uint32            rate
   )
{
  soc_error_t rv;
  int use_core = core;

  int nof_cores = 1, i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
      nof_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
  }

  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
  }
  
  for(i=0 ; i < nof_cores ; i++) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.sch_rates.set(unit, use_core + i, base_q_pair, rate);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sch_port_rate_set()",0,0);
}

uint32
  arad_sw_db_sch_priority_port_rate_set( 
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            offset,
   SOC_SAND_IN  uint32            rate,
   SOC_SAND_IN  uint8             valid
   )
{
  soc_error_t rv;
  int use_core = core;

  int nof_cores = 1, i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
      nof_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
  }

  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
  }
  
  for(i=0 ; i < nof_cores ; i++) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority.priority_shaper_rate.set(unit, use_core + i, offset, rate);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority.valid.set(unit, use_core + i, offset, valid);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sch_priority_port_rate_set()",0,0);
}


uint32 
  arad_sw_db_sch_priority_port_rate_get(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            offset,
   SOC_SAND_OUT int               *rate,
   SOC_SAND_OUT uint8             *valid
   )
{
  soc_error_t rv;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
  }
  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 2, exit);
  }
  
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority.priority_shaper_rate.get(unit, use_core, offset, rate);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority.valid.get(unit, use_core, offset, valid);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sch_priority_port_rate_get()",0,0);
}


uint32
  arad_sw_db_sch_port_tcg_rate_set( 
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            offset,
   SOC_SAND_IN  uint32            rate,
   SOC_SAND_IN  uint8             valid
   )
{
  soc_error_t rv;
  int use_core = core;

  int nof_cores = 1, i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
      nof_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
  }

  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
  }
  
  for(i=0 ; i < nof_cores ; i++) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.tcg_shaper.tcg_shaper_rate.set(unit, use_core + i, offset, rate);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.tcg_shaper.valid.set(unit, use_core + i, offset, valid);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sch_priority_port_rate_set()",0,0);
}


uint32 
  arad_sw_db_sch_port_tcg_rate_get(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            offset,
   SOC_SAND_OUT int               *rate,
   SOC_SAND_OUT uint8             *valid
   )
{
  soc_error_t rv;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
  }
  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
  }
  
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.tcg_shaper.tcg_shaper_rate.get(unit, use_core, offset, rate);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.tcg_shaper.valid.get(unit, use_core, offset, valid);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sch_port_tcg_rate_get()",0,0);
}


uint32
  arad_sw_db_egq_port_rate_get(
   SOC_SAND_IN   int               unit,
   SOC_SAND_IN   int               core,
   SOC_SAND_IN   uint32            base_q_pair,
   SOC_SAND_OUT  uint32           *rate
   )
{
  soc_error_t rv;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
  }
  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 2, exit);
  }
  
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.egq_rates.get(unit, use_core, base_q_pair, rate);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_egq_port_rate_get()",0,0);
}

uint32
  arad_sw_db_egq_port_rate_set(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            base_q_pair,
   SOC_SAND_IN  uint32            rate
   )
{
  soc_error_t rv;
  int nof_cores = 1, i;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
      nof_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
  }

  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 2, exit);
  }

  for(i=0 ; i < nof_cores ; i++) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.egq_rates.set(unit, use_core+i, base_q_pair, rate);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_egq_port_rate_set()",0,0);
}

uint32
  arad_sw_db_is_port_valid_get(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            base_q_pair,
   SOC_SAND_OUT uint8             *is_valid
   )
{
  soc_error_t rv;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
  }
  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 2, exit);
  }
  
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.valid.get(unit, use_core, base_q_pair, is_valid);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_is_port_valid_get()",0,0);
}

uint32
  arad_sw_db_is_port_valid_set(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  int               core,
   SOC_SAND_IN  uint32            base_q_pair,
   SOC_SAND_IN  uint8             is_valid
   )
{
  soc_error_t rv;
  int nof_cores = 1, i;
  int use_core = core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
      nof_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
  }

  if(use_core < 0 || use_core > SOC_DPP_DEFS_GET(unit, nof_cores)){
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 2, exit);
  }

  for(i=0 ; i< nof_cores ; i++) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.valid.set(unit, use_core+i, base_q_pair, is_valid);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_is_port_valid_set()",0,0);
}

uint32
  arad_sw_db_device_tdm_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    tdm_context_map_id,
    res;
  uint8
    is_allocated;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tdm.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  if(!is_allocated) {
      res = sw_state_access[unit].dpp.soc.arad.tm.tdm.alloc(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }
  for (tdm_context_map_id = 0; tdm_context_map_id < ARAD_NOF_TDM_CONTEXT_MAP; tdm_context_map_id++)
  {
      res = sw_state_access[unit].dpp.soc.arad.tm.tdm.context_map.set(unit, tdm_context_map_id, ARAD_IF_ID_NONE);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
      res = sw_state_access[unit].dpp.soc.arad.tm.tdm.tdm_context_ref_count.set(unit, tdm_context_map_id, 0);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
   }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_device_tdm_init()",0,0);
}


uint32
  arad_sw_db_tm_init(
    SOC_SAND_IN int unit
  )
{
  uint8
    is_allocated;
  soc_error_t
    rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 5, exit);

  if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.alloc(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  }

  rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.is_simple_mode.set(unit, ARAD_SW_DB_QUEUE_TO_RATE_CLASS_MAPPING_IS_UNDEFINED);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
 
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_tm_init()",0,0);
}

uint32
  arad_sw_db_cnt_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(&(Arad_sw_db.arad_device_sw_db[unit]->cnt), ARAD_SW_DB_CNT, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_cnt_init()",0,0);
}
uint32
  arad_sw_db_dram_init(
    SOC_SAND_IN int unit
  )
{
  uint8
    is_allocated;
  soc_error_t
    rv;
  uint32
    deleted_buf_index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = sw_state_access[unit].dpp.soc.arad.tm.dram.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.dram.alloc(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  }

  for(deleted_buf_index=0; deleted_buf_index < ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++deleted_buf_index) {
    rv = sw_state_access[unit].dpp.soc.arad.tm.dram.dram_deleted_buff_list.set(unit, deleted_buf_index, ARAD_SW_DB_DRAM_DELETED_BUFF_NONE);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_dram_init()",0,0);
}

uint32
  arad_sw_db_tcam_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    bank_id,
    tcam_db_id;
  SOC_SAND_HASH_TABLE_PTR
    entry_id_to_location;
  SOC_SAND_HASH_TABLE_INIT_INFO
    entry_id_to_location_init_info;
  uint8
    is_allocated;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (!SOC_WARM_BOOT(unit)) {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3, exit);

      if(!is_allocated) {
          res = sw_state_access[unit].dpp.soc.arad.tm.tcam.alloc(unit);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
      }
      
      for (bank_id = 0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_id)
      {
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.valid.set(unit, bank_id, FALSE);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      }

      for (tcam_db_id = 0; tcam_db_id < ARAD_TCAM_MAX_NOF_LISTS; ++tcam_db_id)
      {
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.set(unit, tcam_db_id, FALSE);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
      }

  }

  
  sal_memset(&entry_id_to_location_init_info, 0x0, sizeof(entry_id_to_location_init_info));
  entry_id_to_location_init_info.prime_handle  = unit;
  entry_id_to_location_init_info.sec_handle    = 0;
  entry_id_to_location_init_info.table_size    = SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit);
  entry_id_to_location_init_info.table_width   = SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit);
  entry_id_to_location_init_info.key_size      = ARAD_TCAM_DB_HASH_TBL_KEY_SIZE * sizeof(uint8);
  entry_id_to_location_init_info.data_size     = ARAD_TCAM_DB_HASH_TBL_DATA_SIZE * sizeof(uint8);
  entry_id_to_location_init_info.get_entry_fun = NULL;
  entry_id_to_location_init_info.set_entry_fun = NULL;
  res = soc_sand_hash_table_create(unit, &entry_id_to_location, entry_id_to_location_init_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.db_location_tbl.alloc(unit, SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 120, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.global_location_tbl.alloc(unit, SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 130, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.entry_id_to_location.set(unit, entry_id_to_location);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 140, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_tcam_init()",0,0);
}

uint32
  arad_sw_db_tcam_mgmt_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    bank_id,
    prefix;
  uint8
    is_allocated;
  soc_error_t
    rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.alloc(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  }

  for (bank_id = 0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_id)
  {
    for (prefix = 0; prefix < ARAD_TCAM_NOF_PREFIXES; ++prefix)
    {
      rv = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.banks.prefix_db.set(unit, bank_id, prefix, ARAD_TCAM_MAX_NOF_LISTS);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
    }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_tcam_mgmt_init()",0,0);
}



STATIC uint32
  arad_sw_db_interrupts_init(
    SOC_SAND_IN int unit
  )
{
    int nof_interrupts;
    uint8 is_allocated;
    soc_error_t rv;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    rv = sw_state_access[unit].dpp.soc.arad.tm.interrupts.is_allocated(unit, &is_allocated);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 2, exit);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.interrupts.alloc(unit);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 2, exit);
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.interrupts.interrupt_data.is_allocated(unit, &is_allocated);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

    if(!is_allocated) {
        rv = soc_nof_interrupts(unit, &nof_interrupts);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 4, exit);
        rv = sw_state_access[unit].dpp.soc.arad.tm.interrupts.interrupt_data.alloc(unit, nof_interrupts);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_interrupts_init()",0,0); 
}


uint32
  arad_sw_db_buffer_get_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_IN  uint8                              *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_OUT uint8                              *data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_sand_os_memcpy(
    data,
    buffer + (offset * len),
    len
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_buffer_get_entry()",0,0);
}

uint32
  arad_sw_db_buffer_set_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_INOUT  uint8                           *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_IN  uint8                              *data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_sand_os_memcpy(
    buffer + (offset * len),
    data,
    len
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_buffer_set_entry()",0,0);
}

uint32 
  arad_sw_db_pmf_pls_init(
      SOC_SAND_IN  int             unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage
  )
{
    uint32
        is_tm,
        indx;
    soc_error_t
        rv;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_ERR_IF_BELOW_MIN(stage, 0, ARAD_PMF_LOW_LEVEL_STAGE_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_ABOVE_NOF(stage, SOC_PPC_NOF_FP_DATABASE_STAGES, ARAD_PMF_LOW_LEVEL_STAGE_OUT_OF_RANGE_ERR, 20, exit);
    for(indx = 0; indx < ARAD_PMF_NOF_LEVELS; ++ indx) 
    {
        for(is_tm = 0; is_tm < 2; ++is_tm) 
        {
            rv = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.level_index.set(unit, stage, is_tm, indx, indx);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_pmf_pls_init()",0,0);
}


uint32
  arad_sw_db_pmf_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res,
      fem_ndx,
    db_ndx,
     pmf_pgm_ndx;
  SOC_PPC_FP_DATABASE_STAGE            
      stage;
  SOC_PPC_FP_DATABASE_INFO
      db_info;
  SOC_PPC_FP_FEM_ENTRY
      fem_entry;
  uint8
    fem_pgm_id,
    is_allocated;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  if(!is_allocated) {
      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.alloc(unit, SOC_PPC_NOF_FP_DATABASE_STAGES);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  }
  for (stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; stage ++) {
      res = arad_sw_db_pmf_pls_init(unit, stage);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      SOC_PPC_FP_DATABASE_INFO_clear(&db_info);
      for (db_ndx = 0; db_ndx < SOC_PPC_FP_NOF_DBS; ++db_ndx)
      {
          res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.set(unit, stage, db_ndx, &db_info);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
      }

      SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
      for (fem_ndx = 0; fem_ndx < ARAD_PMF_LOW_LEVEL_NOF_FEMS; ++fem_ndx)
      {
          for (fem_pgm_id = 0 ; fem_pgm_id < ARAD_PMF_NOF_FEM_PGMS; fem_pgm_id++) 
          {
              res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.set(unit, stage, fem_pgm_id, fem_ndx, &fem_entry);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
          }
      }

      
      for(pmf_pgm_ndx = 0; pmf_pgm_ndx < ARAD_PMF_NOF_PROGS; pmf_pgm_ndx++)
      {
          res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.fem_pgm_per_pmf_prog.set
                                                (unit,stage,pmf_pgm_ndx,ARAD_PMF_FEM_PGM_INVALID);
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);            
      }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_pmf_initialize()",0,0);
}

uint32
  arad_sw_db_eg_encap_prge_tbl_nof_dynamic_entries_get(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint32 *nof_dynamic_members
  )
{
    *nof_dynamic_members = ARAD_EGR_PROG_EDITOR_PRGE_MEMORY_NOF_DATA_ENTRIES;

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "88650L23C", 0))
    {
        (*nof_dynamic_members)                      -= 2;
    }
    if (1 == soc_property_get(unit, spn_BCM886XX_ERSPAN_TUNNEL_ENABLE, 0) &&
        SOC_IS_ARADPLUS_AND_BELOW(unit))
    {
        (*nof_dynamic_members)                      -= (16 * 2);
    }

    if (SOC_IS_ROO_ENABLE(unit) && SOC_IS_ARADPLUS(unit)) 
    {
        (*nof_dynamic_members)                      -= (16);   
    }

    return SOC_SAND_OK;
}


uint32 
  arad_sw_db_eg_encap_prge_tbl_overlay_arp_entries_base_index_get(
     SOC_SAND_IN  int  unit, 
     SOC_SAND_OUT uint32 *overlay_arp_entry_base_index
     ) {
    *overlay_arp_entry_base_index = ARAD_EGR_PROG_EDITOR_PRGE_MEMORY_NOF_DATA_ENTRIES;

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "88650L23C", 0))
    {
        (*overlay_arp_entry_base_index)                      -= 2;
    }

    if (SOC_IS_ROO_ENABLE(unit) && SOC_IS_ARADPLUS(unit)) 
    {
        (*overlay_arp_entry_base_index)                      -= (16);   
    }

    return SOC_SAND_OK;
}


STATIC int
  arad_sw_db_multiset_by_type_get(
    SOC_SAND_IN  int                      unit,
	SOC_SAND_IN	 int				      core_id,
    SOC_SAND_IN  uint32                   multiset_type, 
    SOC_SAND_OUT SOC_SAND_MULTI_SET_PTR*  multi_set_info
  )
{

  
  
  if (multiset_type >= ARAD_NOF_SW_DB_MULTI_SETS)
  {
    return arad_pp_sw_db_multiset_by_type_get(unit, core_id, multiset_type, multi_set_info);
  }
}

uint32
  arad_sw_db_multiset_add(
    SOC_SAND_IN  int                unit,
	SOC_SAND_IN	 int				      core_id,
    SOC_SAND_IN  uint32                multiset_type, 
    SOC_SAND_IN  uint32                 *val,
    SOC_SAND_OUT  uint32                *data_indx,
    SOC_SAND_OUT  uint8               *first_appear,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint8
    tmp_val[ARAD_SW_DB_MULTISET_MAX_VAL_NOF_BYTES];
  uint8
    add_success;
  uint32
    member_size,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_MULTISET_ADD);

  res = arad_sw_db_multiset_by_type_get(unit, core_id, multiset_type, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_multi_set_get_member_size(unit,multi_set,&member_size) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);
  soc_sand_U32_to_U8(val,member_size,tmp_val);

  res = soc_sand_multi_set_member_add(
          unit,
          multi_set,
          (SOC_SAND_IN  SOC_SAND_MULTI_SET_KEY*)tmp_val,
          data_indx,
          first_appear,
          &add_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (add_success)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_multiset_add()",0,0);
}

uint32
  arad_sw_db_multiset_remove(
    SOC_SAND_IN  int       unit,
	SOC_SAND_IN	 int	   core_id,
    SOC_SAND_IN  uint32       multiset_type, 
    SOC_SAND_IN  uint32        *val,
    SOC_SAND_OUT  uint32       *data_indx,
    SOC_SAND_OUT  uint8      *last_appear
  )
{
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint8
    tmp_val[ARAD_SW_DB_MULTISET_MAX_VAL_NOF_BYTES];
  uint32
    val_lcl[1],
    res = SOC_SAND_OK;
  uint32
    member_size ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_MULTISET_REMOVE);

  res = arad_sw_db_multiset_by_type_get(unit, core_id,multiset_type, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  *val_lcl = *val;
  res = soc_sand_multi_set_get_member_size(unit,multi_set,&member_size) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);
  soc_sand_U32_to_U8(val_lcl,member_size,tmp_val);

  res = soc_sand_multi_set_member_remove(
          unit,
          multi_set,
          (SOC_SAND_IN SOC_SAND_MULTI_SET_KEY*)tmp_val,
          data_indx,
          last_appear
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_multiset_remove()",0,0);
}

uint32
  arad_sw_db_multiset_lookup(
    SOC_SAND_IN  int              unit,
	SOC_SAND_IN	 int	   		  core_id,
    SOC_SAND_IN  uint32              multiset_type, 
    SOC_SAND_IN  uint32               *val,
    SOC_SAND_OUT  uint32              *data_indx,
    SOC_SAND_OUT  uint32              *ref_count
  )
{
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint8
    tmp_val[ARAD_SW_DB_MULTISET_MAX_VAL_NOF_BYTES];
  uint32
    val_lcl[1],
    res = SOC_SAND_OK;
  uint32
    member_size ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_MULTISET_LOOKUP);

  res = arad_sw_db_multiset_by_type_get(unit, core_id, multiset_type, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  *val_lcl = *val;
  res = soc_sand_multi_set_get_member_size(unit,multi_set,&member_size) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);
  soc_sand_U32_to_U8(val_lcl,member_size,tmp_val);

  res = soc_sand_multi_set_member_lookup(
          unit,
          multi_set,
          (SOC_SAND_IN SOC_SAND_MULTI_SET_KEY*)&tmp_val,
          data_indx,
          ref_count
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_multiset_lookup()",0,0);
}

uint32
  arad_sw_db_multiset_add_by_index(
    SOC_SAND_IN  int                unit,
	SOC_SAND_IN	 int	   			core_id,
    SOC_SAND_IN  uint32                multiset_type, 
    SOC_SAND_IN  uint32                 *val,
    SOC_SAND_OUT  uint32                data_indx,
    SOC_SAND_OUT  uint8               *first_appear,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint8
    tmp_val[ARAD_SW_DB_MULTISET_MAX_VAL_NOF_BYTES];
  uint8
    add_success;
  uint32
    res = SOC_SAND_OK;
  uint32
    member_size ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_MULTISET_ADD_BY_INDEX);

  res = arad_sw_db_multiset_by_type_get(unit, core_id, multiset_type, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_multi_set_get_member_size(unit,multi_set,&member_size) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);
  soc_sand_U32_to_U8(val,member_size,tmp_val);
	
  res = soc_sand_multi_set_member_add_at_index(
          unit,
          multi_set,
          (SOC_SAND_IN	SOC_SAND_MULTI_SET_KEY*)tmp_val,
          data_indx,
          first_appear,
          &add_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
	
  if (add_success)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_multiset_add_by_index()",0,0);
}


uint32
  arad_sw_db_multiset_remove_by_index(
    SOC_SAND_IN  int                unit,
	SOC_SAND_IN	 int	   			core_id,
    SOC_SAND_IN  uint32                multiset_type, 
    SOC_SAND_IN  uint32                 data_indx,
    SOC_SAND_OUT  uint8               *last_appear
  )
{
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_MULTISET_REMOVE_BY_INDEX);

  res = arad_sw_db_multiset_by_type_get(unit, core_id, multiset_type, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_multi_set_member_remove_by_index(
          unit,
          multi_set,
          data_indx,
          last_appear
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_multiset_remove_by_index()",0,0);
}

uint32
  arad_sw_db_multiset_clear(
    SOC_SAND_IN  int                unit,
	SOC_SAND_IN	 int	   			core_id,
    SOC_SAND_IN  uint32                multiset_type 
  )
{
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_MULTISET_CLEAR);

  res = arad_sw_db_multiset_by_type_get(unit, core_id, multiset_type, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_multi_set_clear(
          unit,
          multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_multiset_clear()",0,0);
}

uint32
  arad_sw_db_multiset_get_enable_bit(
    SOC_SAND_IN  int                 	unit,
    SOC_SAND_IN  uint32                	core_id, 
	SOC_SAND_IN  uint32					tbl_offset,
	SOC_SAND_OUT uint8					*enable
  )
{
  uint32
    res;

	SOCDNX_INIT_FUNC_DEFS;

	res = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.config_meter_status.bit_get(
		unit,
		core_id,
		tbl_offset,
		enable
		);

	SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
	SOCDNX_FUNC_RETURN;
}


STATIC uint32 arad_sw_db_modport2sysport_init(
    SOC_SAND_IN int unit
  )
{
  soc_error_t rv;
  uint32 modport_idx;
  uint8 is_allocated;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = sw_state_access[unit].dpp.soc.arad.tm.modport2sysport.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.modport2sysport.alloc(unit, ARAD_NOF_MODPORT);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  }

  for(modport_idx = 0; modport_idx < ARAD_NOF_MODPORT; ++modport_idx) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.modport2sysport.set(unit, modport_idx, ARAD_SW_DB_MODPORT2SYSPORT_INVALID_SYSPORT);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, ARAD_GEN_ERR_NUM_CLEAR, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_modport2sysport_init()", 0, 0);
}



STATIC uint32 arad_sw_db_sysport2modport_init(
    SOC_SAND_IN int unit
  )
{
  soc_error_t rv;
  uint32 sysport_idx;
  ARAD_MODPORT_INFO modport;
  uint8 is_allocated;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = sw_state_access[unit].dpp.soc.arad.tm.sysport2modport.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.sysport2modport.alloc(unit, ARAD_NOF_SYS_PHYS_PORTS_GET(unit));
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  }

  for(sysport_idx = 0; sysport_idx < ARAD_NOF_SYS_PHYS_PORTS_GET(unit); ++sysport_idx) {
      modport.fap_id = ARAD_SW_DB_SYSPORT2MODPORT_INVALID_ID;
      modport.fap_port_id = ARAD_SW_DB_SYSPORT2MODPORT_INVALID_ID;
      rv = sw_state_access[unit].dpp.soc.arad.tm.sysport2modport.set(unit, sysport_idx, &modport);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, ARAD_GEN_ERR_NUM_CLEAR, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sysport2modport_init()", 0, 0);
}



STATIC uint32 arad_sw_db_queuequartet2sysport_init(
    SOC_SAND_IN int unit
  )
{
  soc_error_t rv;
  uint32 queue_quartet;
  uint8 is_allocated;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queuequartet2sysport.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queuequartet2sysport.alloc(unit, (SOC_DPP_DEFS_GET(unit, nof_queues_per_pipe)/4) * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  }

  for(queue_quartet = 0; queue_quartet < ((SOC_DPP_DEFS_GET(unit, nof_queues_per_pipe)/4) * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores); ++queue_quartet) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queuequartet2sysport.set(unit, queue_quartet, ARAD_SW_DB_MODPORT2SYSPORT_INVALID_SYSPORT);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, ARAD_GEN_ERR_NUM_CLEAR, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_sysport2modport_init()", 0, 0);
}

int
    arad_sw_db_sw_state_alloc(
        SOC_SAND_IN int     unit
  )
{
    soc_error_t rv;
    uint32 res;
    int core;
    uint8 is_allocated;
    
    SOCDNX_INIT_FUNC_DEFS;

    rv = sw_state_access[unit].dpp.soc.arad.tm.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.alloc(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    res = arad_sw_db_op_mode_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_sw_db_tm_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.sysport2basequeue.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.sysport2basequeue.alloc(unit, ARAD_NOF_SYS_PHYS_PORTS_GET(unit) * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.alloc(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    if (SOC_IS_ARADPLUS(unit)) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_plus.is_allocated(unit, &is_allocated);
        SOCDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_plus.alloc(unit);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.alloc(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    
    
    for (core = 0; core < SOC_DPP_DEFS_MAX(NOF_CORES); core++) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.erp_interface_id.set(unit, core, ARAD_IF_ID_NONE);
        SOCDNX_IF_ERR_EXIT(rv);
    }

#if defined(INCLUDE_KBP)
    rv = sw_state_access[unit].dpp.soc.arad.tm.kbp_info.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.kbp_info.alloc(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }
#endif 

    rv = sw_state_access[unit].dpp.soc.arad.tm.lag.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.lag.alloc(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.cell.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.cell.alloc(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    res = arad_sw_db_device_tdm_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_sw_db_dram_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_ref_count.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_ref_count.alloc(unit, SOC_TMC_ITM_NOF_QT_NDXS);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.alloc(unit, ARAD_SW_DB_NOF_DYNAMIC_QUEUE_TYPES);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.vsi.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.vsi.alloc(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    res = arad_sw_db_modport2sysport_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_sw_db_sysport2modport_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_sw_db_queuequartet2sysport_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_sw_db_interrupts_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    rv = sw_state_access[unit].dpp.soc.arad.tm.phy_ports_info.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.phy_ports_info.alloc(unit, SOC_MAX_NUM_PORTS);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.alloc(unit, SOC_MAX_NUM_PORTS);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.guaranteed_q_resource.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.guaranteed_q_resource.alloc(unit, SOC_DPP_DEFS_MAX(NOF_CORES));
        SOCDNX_IF_ERR_EXIT(rv);
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.egr_prog_editor.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.egr_prog_editor.alloc(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_sw_db_device_init(
    SOC_SAND_IN int     unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_DEVICE_INIT);

  ARAD_ALLOC_ANY_SIZE(Arad_sw_db.arad_device_sw_db[unit], ARAD_SW_DB_DEVICE, 1,"Arad_sw_db.arad_device_sw_db[unit]");

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, SOC_IS_QAX(unit) ? qax_mcds_multicast_init(unit) : dpp_mcds_multicast_init(unit));


	if(!SOC_WARM_BOOT(unit)) {
        res = arad_sw_db_tcam_init(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

        res = arad_sw_db_dev_egr_ports_init(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

	    res = arad_sw_db_tcam_mgmt_init(unit);
	    SOC_SAND_CHECK_FUNC_RESULT(res, 183, exit);

        res = arad_sw_db_pmf_initialize(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

	}

     res = arad_sw_db_cnt_init(unit);
     SOC_SAND_CHECK_FUNC_RESULT(res, 232, exit);

	if(!SOC_WARM_BOOT(unit)) {
        res = arad_sw_db_reassembly_context_init(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 281, exit);
    }

#if defined(BCM_WARM_BOOT_SUPPORT) && defined (BCM_SAT_SUPPORT)
    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SAT_ENABLE, 0)) {
        soc_sat_wb_init(unit);
    }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_arad_device_init()",unit,0);
}

uint32
  arad_sw_db_device_close(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_DEVICE_CLOSE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, SOC_IS_QAX(unit) ? qax_mcds_multicast_terminate(unit) : dpp_mcds_multicast_terminate(unit));
  ARAD_FREE_ANY_SIZE(Arad_sw_db.arad_device_sw_db[unit]);
  Arad_sw_db.arad_device_sw_db[unit] = NULL;

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_device_close()",0,0);
}

uint32
  arad_sw_db_cnt_buff_and_index_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint16                     proc_id,
    SOC_SAND_IN uint32                     *buff,
    SOC_SAND_IN uint32                     index
                                 )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         cnt.host_buff[proc_id],
                         (&buff)
                         );
    ARAD_SW_DB_FIELD_SET(res, 
                         unit,
                         cnt.buff_line_ndx[proc_id],
                         (&index)
                         );


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_cnt_buff_and_index_set()",0,0);
}
uint32
  arad_sw_db_cnt_buff_and_index_get(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint16                     proc_id,
    SOC_SAND_OUT uint32                     **buff,
    SOC_SAND_OUT uint32                     *index
                                 )
{
    
    *buff = Arad_sw_db.arad_device_sw_db[unit]->cnt.host_buff[proc_id];
    *index = Arad_sw_db.arad_device_sw_db[unit]->cnt.buff_line_ndx[proc_id];
    return *index;

}

uint32
  arad_sw_db_dram_deleted_buff_list_add(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     buff
  )
{
    int i, saved_indx=0, indx_saved=0;
    soc_error_t rv;
    uint32 dram_deleted_buff_list;
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);   
   
    for(i=0; i<ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++i) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.dram.dram_deleted_buff_list.get(unit, i, &dram_deleted_buff_list);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
        if(dram_deleted_buff_list == buff) {
            break;
        } else {
            if( (indx_saved == 0) && dram_deleted_buff_list == ARAD_SW_DB_DRAM_DELETED_BUFF_NONE) {
                saved_indx = i;
                indx_saved = 1;
            }
        }        
    }
 
    if(indx_saved == 1) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.dram.dram_deleted_buff_list.set(unit, saved_indx, buff);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    } 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_dram_deleted_buff_list_add()",0,0);
}

uint32
  arad_sw_db_dram_deleted_buff_list_remove(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     buff
  )
{
    int i;
    soc_error_t rv;
    uint32 dram_deleted_buff_list;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    for(i=0; i<ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++i) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.dram.dram_deleted_buff_list.get(unit, i, &dram_deleted_buff_list);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
        if(dram_deleted_buff_list == buff) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.dram.dram_deleted_buff_list.set(unit, i, ARAD_SW_DB_DRAM_DELETED_BUFF_NONE);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
            break;
        }      
    }    

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sw_db_dram_deleted_buff_list_remove()",0,0);
}

int
  arad_sw_db_dram_deleted_buff_list_get(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     buff,
    SOC_SAND_OUT uint32*     is_deleted
  )
{
    int i;
    soc_error_t rv;
    uint32 dram_deleted_buff_list;
    
    SOCDNX_INIT_FUNC_DEFS;  
    
    *is_deleted = 0;
    
    for(i=0; i<ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++i) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.dram.dram_deleted_buff_list.get(unit, i, &dram_deleted_buff_list);
        SOCDNX_IF_ERR_EXIT(rv);
        if(dram_deleted_buff_list == buff) {
            *is_deleted = 1;
            break;
        }      
    }    
    
exit:
  SOCDNX_FUNC_RETURN;
}

int 
  arad_sw_db_dram_deleted_buff_list_get_all(
    SOC_SAND_IN int    unit,
    SOC_SAND_OUT uint32*    buff_list_arr,
    SOC_SAND_IN uint32      arr_size,
    SOC_SAND_OUT uint32*    buff_list_num)
{
    int i;
    soc_error_t rv;
    uint32 dram_deleted_buff_list;
    
    SOCDNX_INIT_FUNC_DEFS;  

    *buff_list_num = 0;
    for(i=0; i<ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++i) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.dram.dram_deleted_buff_list.get(unit, i, &dram_deleted_buff_list);
        SOCDNX_IF_ERR_EXIT(rv);
        if(dram_deleted_buff_list != ARAD_SW_DB_DRAM_DELETED_BUFF_NONE) {
            buff_list_arr[*buff_list_num] = dram_deleted_buff_list;
            ++(*buff_list_num);
            if(*buff_list_num == arr_size) {
                break; 
            }
        }      
    }

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_sw_db_op_mode_init(
    SOC_SAND_IN int unit
  )
{
  uint8 is_allocated;
  soc_error_t rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SW_DB_REVISION_INIT);

  rv = sw_state_access[unit].dpp.soc.arad.tm.op_mode.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 25, exit);

  if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.op_mode.alloc(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 25, exit);
  }

  rv = sw_state_access[unit].dpp.soc.arad.tm.op_mode.is_petrab_in_system.set(unit, FALSE);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 35, exit);

  rv = sw_state_access[unit].dpp.soc.arad.tm.op_mode.tdm_mode.set(unit, ARAD_MGMT_TDM_MODE_PACKET);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_op_mode_init()",0,0);
}

void
  arad_sw_db_is_petrab_in_system_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_petrab_in_system
  )
{
  sw_state_access[unit].dpp.soc.arad.tm.op_mode.is_petrab_in_system.set(unit, is_petrab_in_system);
}

uint8
  arad_sw_db_is_petrab_in_system_get(
    SOC_SAND_IN int unit
  )
{
    uint8 is_petrab_in_system;
    sw_state_access[unit].dpp.soc.arad.tm.op_mode.is_petrab_in_system.get(unit, &is_petrab_in_system);
    return is_petrab_in_system;
}

void
  arad_sw_db_tdm_mode_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_MGMT_TDM_MODE tdm_mode
  )
{
    sw_state_access[unit].dpp.soc.arad.tm.op_mode.tdm_mode.set(unit, tdm_mode);
}

ARAD_MGMT_TDM_MODE
  arad_sw_db_tdm_mode_get(
    SOC_SAND_IN int unit
  )
{
    ARAD_MGMT_TDM_MODE tdm_mode;
    sw_state_access[unit].dpp.soc.arad.tm.op_mode.tdm_mode.get(unit, &tdm_mode);
    return tdm_mode;
}

void
  arad_sw_db_ilkn_tdm_dedicated_queuing_set(
     SOC_SAND_IN int unit,
     SOC_SAND_IN ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing
  )
{
    sw_state_access[unit].dpp.soc.arad.tm.op_mode.ilkn_tdm_dedicated_queuing.set(unit, ilkn_tdm_dedicated_queuing);
}

ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE
  arad_sw_db_ilkn_tdm_dedicated_queuing_get(
     SOC_SAND_IN int unit
  )
{
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    sw_state_access[unit].dpp.soc.arad.tm.op_mode.ilkn_tdm_dedicated_queuing.get(unit, &ilkn_tdm_dedicated_queuing);
    return ilkn_tdm_dedicated_queuing;
}

uint32
  arad_sw_db_tcam_db_forbidden_dbs_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 tcam_db_other,
    SOC_SAND_IN uint8  is_forbidden
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (is_forbidden) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.forbidden_dbs.bit_set(unit, tcam_db_id, tcam_db_other);
    }
    else {
        rv = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.forbidden_dbs.bit_clear(unit, tcam_db_id, tcam_db_other);
    }

    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_forbidden_dbs_set()",0,0);
}

uint32
  arad_sw_db_tcam_db_forbidden_dbs_get(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 tcam_db_other,
    SOC_SAND_OUT uint8 *is_forbidden
  )
{
    soc_error_t
        rv;
    uint8
        bit_val;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.forbidden_dbs.bit_get(unit, tcam_db_id, tcam_db_other, &bit_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

    *is_forbidden  = (bit_val > 0);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_tcam_db_forbidden_dbs_get()",0,0);
}






uint32
  arad_sw_db_fp_db_entry_bitmap_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE     stage,
    SOC_SAND_IN  uint32                     bank_index,
    SOC_SAND_IN  uint32                     entry_index,
    SOC_SAND_IN  uint8                      is_used
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(is_used) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.entry_bitmap.bit_set(unit, stage, bank_index, entry_index); 
    }
    else {
        rv = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.entry_bitmap.bit_clear(unit, stage, bank_index, entry_index); 
    }
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_fp_db_entry_bitmap_set()",0,0);
}

uint32
  arad_sw_db_fp_db_entry_bitmap_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE     stage,
    SOC_SAND_IN  uint32                     bank_index,
    SOC_SAND_IN  uint32                     entry_index,
    SOC_SAND_OUT uint8                      *is_used
  )
{
    soc_error_t
        rv;
    uint8
        bit_val;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.entry_bitmap.bit_get(unit, stage, bank_index, entry_index, &bit_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

    *is_used  = (bit_val > 0 );

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_fp_db_entry_bitmap_get()",0,0);
}
 
uint32
  arad_sw_db_fp_db_entry_bitmap_clear(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE     stage
  )
{
    soc_error_t
        rv;
    int i;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for (i = 0; i < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); i++) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.entry_bitmap.bit_range_clear(unit, stage, i, 0, SOC_DPP_DEFS_MAX_TCAM_NOF_LINES_IN_LONGS*32);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    }


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_fp_db_entry_bitmap_clear()",0,0);
}
 




uint32
  arad_sw_db_is_port_reserved_for_reassembly_context(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_OUT uint8  *is_reserved 
  )
{
  uint32
    port_reserved_reassembly_context;
  soc_error_t
    rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_ERR_IF_ABOVE_NOF(local_port, SOC_DPP_DEFS_GET(unit, nof_logical_ports), SOC_SAND_VALUE_ABOVE_MAX_ERR, 10, exit);
  SOC_SAND_CHECK_NULL_INPUT(is_reserved);

  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_reserved_reassembly_context.get(unit, local_port / SOC_SAND_NOF_BITS_IN_UINT32, &port_reserved_reassembly_context);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  *is_reserved = (port_reserved_reassembly_context >> (local_port % SOC_SAND_NOF_BITS_IN_UINT32)) & 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_is_port_reserved_for_reassembly_context()",local_port,0);
}

uint32
  arad_sw_db_set_port_reserved_for_reassembly_context(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint8   reserve 
  )
{
  soc_error_t
    rv;
  uint32 mask = 1 << (local_port % SOC_SAND_NOF_BITS_IN_UINT32);
  uint32 value;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_ERR_IF_ABOVE_NOF(local_port, SOC_DPP_DEFS_GET(unit, nof_logical_ports), SOC_SAND_VALUE_ABOVE_MAX_ERR, 10, exit);

  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_reserved_reassembly_context.get(unit, local_port / SOC_SAND_NOF_BITS_IN_UINT32, &value);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  if (reserve) {
    value |= mask;
  } else {
    value &= ~mask;
  }
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_reserved_reassembly_context.set(unit, local_port / SOC_SAND_NOF_BITS_IN_UINT32, value);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_set_port_reserved_for_reassembly_context()",local_port,reserve);
}


#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP 	 
int	 
arad_sw_db_sw_dump(int unit) 	 
{
    uint32                          i, j;
    int                             core;
    soc_error_t                     rv;
    uint16                          current_cell_ident;
    ARAD_INTERFACE_ID               context_map;
    ARAD_SW_DB_DEV_EGR_RATE         rates;
    ARAD_SW_DB_DEV_RATE             tcg_rate;
    ARAD_EGR_PROG_TM_PORT_PROFILE   ports_prog_editor_profile;
    ARAD_SW_DB_DEV_RATE             queue_rate;
    uint32                          calcal_length;
    ARAD_SW_DB_DEV_EGR_CHAN_ARB     chan_arb;

    SOCDNX_INIT_FUNC_DEFS;  
        

    LOG_VERBOSE(BSL_LS_SOC_SWDB,
                (BSL_META_U(unit,
                            "\n ARAD SOC TM:")));
    LOG_VERBOSE(BSL_LS_SOC_SWDB,
                (BSL_META_U(unit,
                            "\n ------------")));

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.cell.current_cell_ident.get(unit, &current_cell_ident));
    LOG_VERBOSE(BSL_LS_SOC_SWDB,
                (BSL_META_U(unit,
                            "\n current_cell_ident:   %u\n"),  current_cell_ident));

    for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); i++) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.calcal_length.get(unit, i, &calcal_length);
        SOCDNX_IF_ERR_EXIT(rv);
        LOG_VERBOSE(BSL_LS_SOC_SWDB,
                    (BSL_META_U(unit,
                                "\n calcal_length:        %u\n"),  calcal_length));
    }

    for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); i++) {
        for (j = 0; j < ARAD_EGR_NOF_Q_PAIRS; j++) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority_cal.queue_rate.get(unit, i, j, &queue_rate);
            SOCDNX_IF_ERR_EXIT(rv);
            if(queue_rate.valid) {
                LOG_VERBOSE(BSL_LS_SOC_SWDB,
                            (BSL_META_U(unit,
                                        "\n queue_rate (%03d): valid %hhu egq_rates %u egq_bursts %u\n"), 
                                        i, 
                             queue_rate.valid, 
                             queue_rate.egq_rates, 
                             queue_rate.egq_bursts));
            }
        }
    }

    for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); i++) {
        for (j = 0; j < SOC_DPP_DEFS_GET(unit, nof_channelized_calendars); j++) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.chan_arb.get(unit, i, j, &chan_arb);
            SOCDNX_IF_ERR_EXIT(rv);
            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "\n nof_calcal_instances (%02d):  %u\n"), i, chan_arb.nof_calcal_instances));
        }
    }

    for (i = 0; i < ARAD_NOF_FAP_PORTS; i++) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.ports_prog_editor_profile.get(unit, i, &ports_prog_editor_profile);
        SOCDNX_IF_ERR_EXIT(rv);
        if(ports_prog_editor_profile != 0) {
            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "\n ports_prog_editor_profile (%03d):  %hu\n"), i, ports_prog_editor_profile));
        }
    }

    for (i = 0; i < ARAD_NOF_LAG_GROUPS_MAX; i++) {
        uint8 in_use;
        rv = sw_state_access[unit].dpp.soc.arad.tm.lag.in_use.get(unit, i, &in_use);
        SOCDNX_IF_ERR_EXIT(rv);
        if(in_use != 0) {
            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "\n in_use (%04d):  %hhu\n"), i, in_use));
        }
    }

    for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_logical_ports); i++) {
        uint32  local_to_reassembly_context;
        rv = sw_state_access[unit].dpp.soc.arad.tm.lag.local_to_reassembly_context.get(unit, i, &local_to_reassembly_context);
        SOCDNX_IF_ERR_EXIT(rv);
        if(local_to_reassembly_context != i) {
            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "\n local_to_reassembly_context (%03d):  %u\n"), i, local_to_reassembly_context));
        }
    }

    for (i = 0; i < ARAD_NOF_TDM_CONTEXT_MAP; i++) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tdm.context_map.get(unit, i, &context_map));
        if(context_map != i) {
            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "\n tdm_context_map (%03d):  %u\n"), i, context_map));
        }
    }
 
    for (core = 0; core < SOC_DPP_DEFS_GET(unit, nof_cores); core++) {
        for (i = 0; i < ARAD_EGR_NOF_PS; i++) {
            for (j = 0; j < ARAD_NOF_TCGS; j++) {
                rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.tcg_cal.tcg_rate.get(unit, core, i, j, &tcg_rate);
                SOCDNX_IF_ERR_EXIT(rv);
                if(tcg_rate.valid) {
                    LOG_VERBOSE(BSL_LS_SOC_SWDB,
                                (BSL_META_U(unit,
                                            "\n eg_mult_nof_vlan_bitmaps (%02d, %01d): valid %hhu egq_rates %u egq_bursts %u\n"), 
                                            i, j, 
                                 tcg_rate.valid, 
                                 tcg_rate.egq_rates, 
                                 tcg_rate.egq_bursts));
                }
            }
        }
    }

    for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); i++) {
        for (j = 0; j < ARAD_EGR_NOF_BASE_Q_PAIRS; j++) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.get(unit, i, j, &rates);
            SOCDNX_IF_ERR_EXIT(rv);
            if(rates.valid) {
                LOG_VERBOSE(BSL_LS_SOC_SWDB,
                            (BSL_META_U(unit,
                                        "\n rates (%02d, %03d): valid %hhu sch_rates %u egq_rates %u egq_bursts %u\n"), 
                                        i, j, 
                             rates.valid, 
                             rates.sch_rates, 
                             rates.egq_rates, 
                             rates.egq_bursts));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif 

#define ARAD_DEVICE_NUMBER_BITS 11
#define ARAD_DEVICE_NUMBER_MASK 0x7ff


uint32 arad_sw_db_modport2sysport_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 fap_id,
    SOC_SAND_IN uint32 fap_port_id,
    SOC_SAND_IN ARAD_SYSPORT sysport
  )
{
  soc_error_t rv;
  uint32 modport;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_ERR_IF_ABOVE_NOF(fap_id, ARAD_NOF_FAPS_IN_SYSTEM, SOC_SAND_VALUE_ABOVE_MAX_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(fap_port_id, ARAD_NOF_FAP_PORTS, SOC_SAND_VALUE_ABOVE_MAX_ERR, 15, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(sysport, ARAD_NOF_SYS_PHYS_PORTS_GET(unit), SOC_SAND_VALUE_ABOVE_MAX_ERR, 20, exit);

  SOC_SAND_CHECK_NULL_PTR(Arad_sw_db.arad_device_sw_db[unit], SOC_SAND_ERR, exit);

  modport = fap_id | (fap_port_id << ARAD_DEVICE_NUMBER_BITS);
  SOC_SAND_ERR_IF_ABOVE_NOF(modport, ARAD_NOF_MODPORT, SOC_SAND_VALUE_ABOVE_MAX_ERR, 20, exit);
  rv = sw_state_access[unit].dpp.soc.arad.tm.modport2sysport.set(unit, modport, sysport);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_modport2sysport_set()", fap_id, fap_port_id);
}


uint32 arad_sw_db_sysport2modport_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_SYSPORT sysport,
    SOC_SAND_IN uint32 fap_id,
    SOC_SAND_IN uint32 fap_port_id
  )
{
  soc_error_t rv;
  ARAD_MODPORT_INFO modport;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_ERR_IF_ABOVE_NOF(fap_id, ARAD_NOF_FAPS_IN_SYSTEM, SOC_SAND_VALUE_ABOVE_MAX_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(fap_port_id, ARAD_NOF_FAP_PORTS, SOC_SAND_VALUE_ABOVE_MAX_ERR, 15, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(sysport, ARAD_NOF_SYS_PHYS_PORTS_GET(unit), SOC_SAND_VALUE_ABOVE_MAX_ERR, 20, exit);
  SOC_SAND_CHECK_NULL_PTR(Arad_sw_db.arad_device_sw_db[unit], SOC_SAND_ERR, exit);

  modport.fap_id = fap_id;
  modport.fap_port_id = fap_port_id;
  rv = sw_state_access[unit].dpp.soc.arad.tm.sysport2modport.set(unit, sysport, &modport);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_modport2sysport_set()", fap_id, fap_port_id);
}

uint32 arad_sw_db_modport2sysport_remove(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 fap_id,
    SOC_SAND_IN uint32 fap_port_id
  )
{
  uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  res = arad_sw_db_modport2sysport_set(unit,fap_id, fap_port_id, ARAD_SW_DB_MODPORT2SYSPORT_INVALID_SYSPORT);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_modport2sysport_remove()", fap_id, fap_port_id);
}


uint32 arad_sw_db_modport2sysport_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 fap_id,
    SOC_SAND_IN uint32 fap_port_id,
    SOC_SAND_OUT ARAD_SYSPORT *sysport
  )
{
  uint32 modport;
  ARAD_SYSPORT modport2sysport;
  soc_error_t rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(sysport);
  SOC_SAND_ERR_IF_ABOVE_NOF(fap_id, ARAD_NOF_FAPS_IN_SYSTEM, SOC_SAND_VALUE_ABOVE_MAX_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(fap_port_id, ARAD_NOF_FAP_PORTS, SOC_SAND_VALUE_ABOVE_MAX_ERR, 15, exit);
  SOC_SAND_CHECK_NULL_PTR(Arad_sw_db.arad_device_sw_db[unit], SOC_SAND_ERR, exit);

  modport = fap_id | (fap_port_id << ARAD_DEVICE_NUMBER_BITS);
  SOC_SAND_ERR_IF_ABOVE_NOF(modport, ARAD_NOF_MODPORT, SOC_SAND_VALUE_ABOVE_MAX_ERR, 20, exit);
  rv = sw_state_access[unit].dpp.soc.arad.tm.modport2sysport.get(unit, modport, &modport2sysport);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 30, exit);
  if (modport2sysport != ARAD_SW_DB_MODPORT2SYSPORT_INVALID_SYSPORT) {
      *sysport = modport2sysport;
  } else {
      *sysport = ARAD_SYS_PHYS_PORT_INVALID(unit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_modport2sysport_get()", fap_id, fap_port_id);
}


uint32 arad_sw_db_sysport2modport_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_SYSPORT sysport,
    SOC_SAND_OUT uint32 *fap_id,
    SOC_SAND_OUT uint32 *fap_port_id
  )
{
  ARAD_MODPORT_INFO modport;
  soc_error_t rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(fap_id);
  SOC_SAND_CHECK_NULL_INPUT(fap_port_id);
  SOC_SAND_CHECK_NULL_PTR(Arad_sw_db.arad_device_sw_db[unit], SOC_SAND_ERR, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(sysport, ARAD_NOF_SYS_PHYS_PORTS_GET(unit), SOC_SAND_VALUE_ABOVE_MAX_ERR, 10, exit);

  rv = sw_state_access[unit].dpp.soc.arad.tm.sysport2modport.get(unit, sysport, &modport);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 30, exit);
  *fap_id = modport.fap_id;
  *fap_port_id = modport.fap_port_id;

  if (*fap_id >= ARAD_NOF_FAPS_IN_SYSTEM ||
      *fap_port_id >= ARAD_NOF_FAP_PORTS) {
      *fap_id = *fap_port_id = ARAD_SW_DB_SYSPORT2MODPORT_INVALID_ID;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_modport2sysport_get()", fap_id, fap_port_id);
}

uint32
  arad_sw_db_sysport2queue_set(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN uint32          core_id,
    SOC_SAND_IN ARAD_SYSPORT    sysport,
    SOC_SAND_IN uint8           valid,
    SOC_SAND_IN uint8           sw_only,
    SOC_SAND_IN uint32          base_queue
   )
{
    uint8 valid_flags = 0;
    uint32 base_queue_lcl;
    int core_index = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid unit")));
    }
    
    if (ARAD_IS_VOQ_MAPPING_INDIRECT(unit) && ARAD_MAX_SYSTEM_PHYSICAL_PORT_ID_INDIRECT < sysport) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid sysport: %d"), sysport));
    }
    if (ARAD_IS_VOQ_MAPPING_DIRECT(unit) && ARAD_MAX_SYSTEM_PHYSICAL_PORT_ID < sysport){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid sysport: %d"), sysport));
    }
    if (core_id != SOC_CORE_ALL && core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid core ID: %d"), core_id));
    }

    valid_flags = 0;
    if (valid) {
        valid_flags |= ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_VALID;
    }
    if (sw_only) {
        valid_flags |= ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_SW_ONLY;
    } 
    SOC_DPP_ASSYMETRIC_CORES_ITER(core_id, core_index) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tm_info.sysport2basequeue.valid_flags.set(unit, (sysport * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) + core_index, valid_flags));
        base_queue_lcl = (valid) ? base_queue : ARAD_IPQ_DESTINATION_ID_INVALID_QUEUE(unit);
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tm_info.sysport2basequeue.base_queue.set(unit, (sysport * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) + core_index, base_queue_lcl));
    }

    exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_sw_db_sysport2queue_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  ARAD_SYSPORT    sysport,
    SOC_SAND_OUT uint8          *valid,
    SOC_SAND_OUT uint8          *sw_only,
    SOC_SAND_OUT uint32         *base_queue
   )
{
    int core_offset = 0;
    uint8 valid_flags = 0;
    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid unit")));
    }
    if (ARAD_IS_VOQ_MAPPING_INDIRECT(unit) && ARAD_MAX_SYSTEM_PHYSICAL_PORT_ID_INDIRECT < sysport) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid sysport: %d"), sysport));
    }
    if (ARAD_IS_VOQ_MAPPING_DIRECT(unit) && ARAD_MAX_SYSTEM_PHYSICAL_PORT_ID < sysport){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid sysport: %d"), sysport));
    }
    if (core_id != SOC_CORE_ALL && !SOC_DPP_CORE_MODE_IS_SYMMETRIC(unit)){
        if (core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid core_id: %d"),sysport));
        }
        core_offset = core_id;
    } else {
        core_offset = 0;
    }
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tm_info.sysport2basequeue.valid_flags.get(unit, (sysport * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) + core_offset, &valid_flags));
    
    *valid = (valid_flags & ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_VALID) ? TRUE : FALSE;
    *sw_only = (valid_flags & ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE_SW_ONLY) ? TRUE : FALSE;

    if (*valid) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tm_info.sysport2basequeue.base_queue.get(unit, (sysport * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) + core_offset, base_queue));
    } else {
        *base_queue = ARAD_IPQ_DESTINATION_ID_INVALID_QUEUE(unit);
    }
   
exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_sw_db_queuequartet2sysport_set(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN uint32          core_id,
    SOC_SAND_IN uint32          queue_quartet,
    SOC_SAND_IN ARAD_SYSPORT    sysport
   )
{
    int core_index = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid unit")));
    }
    
    if (ARAD_NOF_SYS_PHYS_PORTS_GET(unit) < sysport) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid sysport: %d"), sysport));
    }

    if (core_id != SOC_CORE_ALL && core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid core ID: %d"), core_id));
    }

    SOC_DPP_ASSYMETRIC_CORES_ITER(core_id, core_index) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tm_info.queuequartet2sysport.set(unit, (queue_quartet * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) + core_index, sysport));
    }

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_sw_db_queuequartet2sysport_get(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN uint32          core_id,
    SOC_SAND_IN uint32          queue_quartet,
    SOC_SAND_OUT ARAD_SYSPORT    *sysport
   )
{
    int core_index = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid unit")));
    }
    
    if (core_id != SOC_CORE_ALL && core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("invalid core ID: %d"), core_id));
    }

    SOC_DPP_ASSYMETRIC_CORES_ITER(core_id, core_index) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tm_info.queuequartet2sysport.get(unit, (queue_quartet * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) + core_index, sysport));
        if (*sysport == ARAD_SW_DB_MODPORT2SYSPORT_INVALID_SYSPORT) {
            *sysport = ARAD_SYS_PHYS_PORT_INVALID(unit);
        }
    }

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_sw_db_queue_type_ref_count_exchange(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  int            core,
    SOC_SAND_IN  uint8          orig_q_type,
    SOC_SAND_IN  uint8          new_q_type,
    SOC_SAND_IN  int            nof_additions)
{    
    soc_error_t
        rv;
    uint32 
        orig_q_type_ref_count, new_q_type_ref_count;
    int 
        nof_queue_remaped;
    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("Invalid unit: %d"), unit));
    }
    if((core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) && core != BCM_CORE_ALL){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("Core %d out of range"), core));
    } else if (core == BCM_CORE_ALL) {
        nof_queue_remaped = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores * nof_additions;
    } else {
        nof_queue_remaped = nof_additions;
    }
    if (orig_q_type >= SOC_TMC_ITM_NOF_QT_NDXS && orig_q_type != SOC_TMC_ITM_QT_NDX_INVALID) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("Invalid queue type: %d"), orig_q_type));
    }
    if (new_q_type >= SOC_TMC_ITM_NOF_QT_NDXS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("Invalid queue type: %d"), new_q_type));
    }

    if (orig_q_type != SOC_TMC_ITM_QT_NDX_INVALID) {
        
        rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_ref_count.get(unit, orig_q_type, &orig_q_type_ref_count);
        SOCDNX_IF_ERR_EXIT(rv);

        if (!orig_q_type_ref_count) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("No Queues mapped to queue type: %d"), orig_q_type));
        }
        orig_q_type_ref_count -= nof_queue_remaped;

        rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_ref_count.set(unit, orig_q_type, orig_q_type_ref_count);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_ref_count.get(unit, new_q_type, &new_q_type_ref_count);
    SOCDNX_IF_ERR_EXIT(rv);

    new_q_type_ref_count += nof_queue_remaped;

    rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_ref_count.set(unit, new_q_type, new_q_type_ref_count);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_sw_db_queue_type_map_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint8          user_q_type,  
    SOC_SAND_OUT uint8*         mapped_q_type 
  )
{
  soc_error_t rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(mapped_q_type);

  if (user_q_type == SOC_TMC_ITM_QT_NDX_INVALID) {
       *mapped_q_type = SOC_TMC_ITM_QT_NDX_INVALID;
  } else if (ARAD_SW_DB_QUEUE_TYPE_IS_DYNAMIC(unit, user_q_type)) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.get(unit,
        user_q_type - ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit),
        mapped_q_type);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
      if (!*mapped_q_type) {
          *mapped_q_type = ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE;
      }
  } else if (user_q_type >= SOC_TMC_ITM_PREDEFIEND_OFFSET &&
             (user_q_type < SOC_TMC_ITM_PREDEFIEND_OFFSET + SOC_TMC_ITM_NOF_QT_STATIC ||
              user_q_type == SOC_TMC_ITM_PREDEFIEND_OFFSET + SOC_TMC_ITM_QT_PUSH_Q_NDX)) {
      *mapped_q_type = user_q_type - SOC_TMC_ITM_PREDEFIEND_OFFSET;
  } else {
      SOC_SAND_SET_ERROR_CODE(ARAD_ITM_IPS_QT_RNG_OUT_OF_RANGE_ERR, 10, exit); 
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_queue_type_map_get()", unit, user_q_type);
}


uint32
  arad_sw_db_queue_type_map_get_alloc(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint8          user_q_type,  
    SOC_SAND_OUT uint8*         mapped_q_type 
  )
{
  uint8       q_type_map;
  soc_error_t rv;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(mapped_q_type);

  if (ARAD_SW_DB_QUEUE_TYPE_IS_DYNAMIC(unit, user_q_type)) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.get(unit,
        user_q_type - ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit),
        &q_type_map);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
      if (SOC_IS_JERICHO(unit)) {
          
          if (!q_type_map) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.set(
                    unit,
                    user_q_type - ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit),
                    user_q_type);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
            *mapped_q_type = user_q_type;
          } else {
              *mapped_q_type = q_type_map;
          }
      } else {
          if (!q_type_map) { 
              uint8 reverse_mapping[ARAD_SW_NOF_AVAILABLE_HW_QUEUE - 1 - ARAD_SW_1ST_AVAILABLE_HW_QUEUE] = {0};
              uint8 i, hw_q;
              for (i = ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit); i < ARAD_SW_NOF_AVAILABLE_HW_QUEUE - 1; ++i) {
                  rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.get(unit,
                    i - ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit),
                    &q_type_map);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 30, exit);
                  if ((hw_q = q_type_map)) {
                      hw_q -= ARAD_SW_1ST_AVAILABLE_HW_QUEUE;
                      if ((hw_q >= ARAD_SW_NOF_AVAILABLE_HW_QUEUE - 1 - ARAD_SW_1ST_AVAILABLE_HW_QUEUE) || reverse_mapping[hw_q]) {
                          SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 99, exit); 
                      }
                      reverse_mapping[hw_q] = 1;
                  }
              }
              hw_q = 0;
              for (i = 0; i < ARAD_SW_NOF_AVAILABLE_HW_QUEUE - 1 - ARAD_SW_1ST_AVAILABLE_HW_QUEUE; ++i) {
                  if (!reverse_mapping[i]) {
                      hw_q = i + ARAD_SW_1ST_AVAILABLE_HW_QUEUE;
                      break;
                  }
              }
              rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.set(unit,
                user_q_type - ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit),
                hw_q ? hw_q : ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE );
              SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 40, exit);
          }
          rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.get(unit,
            user_q_type - ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit),
            mapped_q_type);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 50, exit);
      }
  } else if ((user_q_type >= SOC_TMC_ITM_PREDEFIEND_OFFSET && user_q_type < SOC_TMC_ITM_PREDEFIEND_OFFSET + SOC_TMC_ITM_NOF_QT_STATIC) ||
             (user_q_type == SOC_TMC_ITM_QT_PUSH_Q_NDX + SOC_TMC_ITM_PREDEFIEND_OFFSET && !SOC_IS_ARADPLUS_AND_BELOW(unit))) {
      *mapped_q_type = user_q_type - SOC_TMC_ITM_PREDEFIEND_OFFSET;
  } else {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_IPS_QT_RNG_OUT_OF_RANGE_ERR, 10, exit); 
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_queue_type_map_get()", unit, user_q_type);
}


uint32
  arad_sw_db_queue_type_map_reverse_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint8          mapped_q_type,  
    SOC_SAND_OUT uint8*         user_q_type     
  )
{
  uint8       q_type_map;
  soc_error_t rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(user_q_type);

  if (mapped_q_type < SOC_TMC_ITM_NOF_QT_STATIC || mapped_q_type == SOC_TMC_ITM_QT_PUSH_Q_NDX) {
    *user_q_type = mapped_q_type + SOC_TMC_ITM_PREDEFIEND_OFFSET;
  } else if (SOC_IS_JERICHO(unit)) {
      *user_q_type = mapped_q_type;
  } else {
    uint8 i;
    if (mapped_q_type >= ARAD_SW_NOF_AVAILABLE_HW_QUEUE) {
        SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 20, exit); 
    }
    for (i = 0; i < ARAD_SW_DB_NOF_LEGAL_DYNAMIC_QUEUE_TYPES(unit); ++i) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.q_type_map.get(unit, i, &q_type_map);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
      if (q_type_map == mapped_q_type) {
        break;
      }
    }
    if (i >= ARAD_SW_DB_NOF_LEGAL_DYNAMIC_QUEUE_TYPES(unit)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 20, exit); 
    }
    *user_q_type = i + ARAD_SW_DB_1ST_DYNAMIC_QUEUE_TYPE(unit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sw_db_queue_type_map_reverse_get()", unit, mapped_q_type);
}

uint32 
    arad_sw_db_rate_class_ref_count_get(
       SOC_SAND_IN  int                         unit, 
       SOC_SAND_IN  int                         core_id, 
       SOC_SAND_IN  uint32                      is_ocb_only, 
       SOC_SAND_IN  uint32                      rate_class, 
       SOC_SAND_OUT uint32*                     ref_count)
{
    int rv = SOC_E_NONE;
    uint32 
        dram_mixed_ref_count = 0, dram_mixed_ref_count_sum = 0,
        ocb_only_ref_count = 0, ocb_only_ref_count_sum = 0;
    int core_index;
    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("Invalid unit: %d"), unit));
    }
    if((core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) && core_id != BCM_CORE_ALL){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("Core %d out of range"), core_id));
    }
    SOCDNX_NULL_CHECK(ref_count);

   for(core_index = ((core_id == _SHR_CORE_ALL || SOC_DPP_CORE_MODE_IS_SYMMETRIC(unit)) ? 0 : core_id);
        core_index < ((core_id == _SHR_CORE_ALL || SOC_DPP_CORE_MODE_IS_SYMMETRIC(unit) ? 1 : core_id + 1));
        core_index++)
   {
        rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.ref_count.get(unit, core_index, rate_class, &dram_mixed_ref_count);
        SOCDNX_IF_ERR_EXIT(rv);
        dram_mixed_ref_count_sum += dram_mixed_ref_count;
        rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.ocb_only_ref_count.get(unit, core_index, rate_class, &ocb_only_ref_count);
        SOCDNX_IF_ERR_EXIT(rv);
        ocb_only_ref_count_sum += ocb_only_ref_count;
    }
    if (is_ocb_only == TRUE) {
        *ref_count = ocb_only_ref_count_sum;
    } else if (is_ocb_only == FALSE) {
        *ref_count = dram_mixed_ref_count_sum - ocb_only_ref_count_sum;
    } else {
        *ref_count = dram_mixed_ref_count_sum;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_sw_db_tm_queue_to_rate_class_mapping_ref_count_exchange(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  int                         core, 
       SOC_SAND_IN  uint32                      is_ocb_only,
       SOC_SAND_IN  uint32                      old_rate_class,
       SOC_SAND_IN  uint32                      new_rate_class,
       SOC_SAND_IN  int                         nof_additions) 
{
    uint32 res = SOC_SAND_OK;
    uint32 new_ref_count, old_ref_count;
    int core_index;
    int nof_queue_remaped;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    if((core < 0 || core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) && core != BCM_CORE_ALL){
        res = SOC_E_PARAM;
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
    } else {
        nof_queue_remaped = nof_additions;
    }
    if (old_rate_class != new_rate_class) {
        SOC_DPP_CORES_ITER(core, core_index) {
            SOC_SAND_ERR_IF_ABOVE_MAX(old_rate_class, SOC_TMC_ITM_NOF_RATE_CLASSES ,ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 10, exit);
            SOC_SAND_ERR_IF_ABOVE_MAX(new_rate_class, SOC_TMC_ITM_NOF_RATE_CLASSES ,ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 20, exit);
            if (new_rate_class != SOC_TMC_ITM_NOF_RATE_CLASSES) {
                res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.ref_count.get(unit, core_index, new_rate_class, &new_ref_count);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

                SOC_SAND_ERR_IF_ABOVE_MAX(new_ref_count, ARAD_MAX_QUEUE_ID(unit), ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 40, exit);

                res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.ref_count.set(unit, core_index, new_rate_class, new_ref_count + nof_queue_remaped);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
            }

            if (old_rate_class != SOC_TMC_ITM_NOF_RATE_CLASSES) {
                res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.ref_count.get(unit, core_index, old_rate_class, &old_ref_count);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);

                res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.ref_count.set(unit, core_index, old_rate_class, old_ref_count - nof_queue_remaped);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);
            }

            if (is_ocb_only) {
                if (new_rate_class != SOC_TMC_ITM_NOF_RATE_CLASSES) {
                    res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.ocb_only_ref_count.get(unit, core_index, new_rate_class, &new_ref_count);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 130, exit);

                    SOC_SAND_ERR_IF_ABOVE_MAX(new_ref_count, ARAD_MAX_QUEUE_ID(unit), ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 140, exit);

                    res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.ocb_only_ref_count.set(unit, core_index, new_rate_class, new_ref_count + nof_queue_remaped);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 150, exit);
                }

                if (old_rate_class != SOC_TMC_ITM_NOF_RATE_CLASSES) {
                    res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.ocb_only_ref_count.get(unit, core_index, old_rate_class, &old_ref_count);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 160, exit);

                    res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.queue_to_rate_class_mapping.ocb_only_ref_count.set(unit, core_index, old_rate_class, old_ref_count - nof_queue_remaped);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 170, exit);
                }
            }
        }
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sw_db_tm_queue_to_rate_class_mapping_ref_count_exchange()",0,0);
}


uint32 arad_sw_db_egress_group_open_set(
    SOC_SAND_IN  int     unit, 
    SOC_SAND_IN  uint32  group_id,  
    SOC_SAND_IN  uint8   is_open    
)
{
    soc_error_t rv;
  
  SOCDNX_INIT_FUNC_DEFS;
  if (!SOC_UNIT_NUM_VALID(unit)) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid unit")));
  } else if (group_id >= SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("input too big")));
  }



  if(is_open) {
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_set(unit, group_id);
  } else {
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_clear(unit, group_id);
  }
  SOCDNX_IF_ERR_EXIT(rv);

exit:
  SOCDNX_FUNC_RETURN;
}



uint32 arad_sw_db_egress_group_open_set_all(
    SOC_SAND_IN  int     unit, 
    SOC_SAND_IN  uint8   is_open    
)
{
    uint32 i;
    uint8 cur_bit;
    soc_error_t rv;
  
    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid unit")));
    }

    for (i = 0; i < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids; ++i) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_get(unit, i , &cur_bit);
        SOCDNX_IF_ERR_EXIT(rv);
        if ((cur_bit ? 1: 0) != (is_open ? 1 : 0)) {
            if(is_open) {
                rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_set(unit, i);
            } else {
                rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_clear(unit, i);
            }
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
  SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 


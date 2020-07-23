
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

#include <soc/mem.h>

#define _ERR_MSG_MODULE_NAME BSL_SOC_SWDB

#include <shared/bsl.h>




#include <shared/swstate/access/sw_state_access.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>



#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_trap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_mirror.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_cos.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/JER/JER_PP/jer_pp_metering.h>




#define ARAD_PP_SW_DB_FREE_LIST_MAX_SIZE (256)

#define ARAD_WB_RECONFIG        (1) 





#define ARAD_PP_SW_DB_NULL_CHECK(unit, field) \
    if ((Arad_pp_sw_db.device[unit] == NULL) || (Arad_pp_sw_db.device[unit]->field == NULL)) { \
        return SOC_SAND_OK; \
    } 

#define ARAD_PP_SW_DB_ACTIVE_INS(cached, name)   \
  ((!cached)?(name):(name##_cache))



#define ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_PENDING_OP_TYPE_MASK 0x3
#define ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_PENDING_OP_TYPE_SHIFT 30

#define ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_HW_TARGET_TYPE_MASK 0x3
#define ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_HW_TARGET_TYPE_SHIFT (ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_PENDING_OP_TYPE_SHIFT - 2)

#define ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_MASK 0xFFFFFFF
#define ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_SHIFT 0



#define ARAD_PP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(payload) \
  (((payload) >> ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_SHIFT) & ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_MASK)

#define ARAD_PP_IPV4_LPM_MGR_PAT_DATA_TO_PENDING_OP_TYPE(payload) \
  (((payload) >> ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_PENDING_OP_TYPE_SHIFT) & ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_PENDING_OP_TYPE_MASK)

#define ARAD_PP_IPV4_LPM_MGR_PAT_DATA_TO_HW_TARGET_TYPE(payload) \
  (((payload) >> ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_HW_TARGET_TYPE_SHIFT) & ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_HW_TARGET_TYPE_MASK)











CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
    Arad_pp_procedure_desc_element_sw_db[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_SW_DB_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_SW_DB_GET_ERRS_PTR),
  

  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_SW_DB_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_SW_DB_DEVICE_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_SW_DB_DEVICE_CLOSE),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
    Arad_pp_error_desc_element_sw_db[] =
{
  
  

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};

 
ARAD_PP_SW_DB Arad_pp_sw_db;







uint32
  arad_pp_sw_db_l2_lif_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  arad_pp_sw_db_lif_ac_initialize(
    SOC_SAND_IN  int unit
  );


uint32
  arad_pp_sw_db_rif_to_lif_group_map_initialize(
     SOC_SAND_IN  int unit
  );

STATIC uint32
  arad_pp_sw_db_rif_to_lif_group_map_terminate(
     SOC_SAND_IN  int unit
  );



STATIC uint32
  arad_pp_sw_db_ecmp_info_initialize(
     SOC_SAND_IN  int unit
  );

STATIC uint32
  arad_pp_sw_db_ipv4_arr_mem_allocator_entry_set(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   uint32                            mem_id,
    SOC_SAND_IN  uint32                             entry_ndx,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY         *entry
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  ARAD_PP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  SOC_DPP_WB_ENGINE_VAR wb_var_index;
  int
    res;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);

  is_cached = ARAD_PP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(mem_id);
  mem_inst = ARAD_PP_ARR_MEM_ALLOCATOR_BANK_INST(mem_id);

  wb_var_index = lpm_mngr->init_info.mem_allocators[mem_inst].wb_var_index;


  if (!is_cached)
  {
      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, wb_var_index + WB_ENGINE_MEM_ALLOCATOR_ARRAY,
                                      entry,
                                      entry_ndx);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  }
  else
  {
      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, wb_var_index + WB_ENGINE_MEM_ALLOCATOR_ARRAY_CACHE,
                                      entry,
                                      entry_ndx);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_ipv4_arr_mem_allocator_entry_set()",0,0);
}

STATIC uint32
  arad_pp_sw_db_ipv4_arr_mem_allocator_entry_get(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   uint32                            mem_id,
    SOC_SAND_IN  uint32                             entry_ndx,
    SOC_SAND_OUT  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY        *entry
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;

  is_cached = ARAD_PP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(mem_id);
  mem_inst = ARAD_PP_ARR_MEM_ALLOCATOR_BANK_INST(mem_id);
    

  if (!is_cached)
  {
    soc_sand_os_memcpy(entry,&(ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.array[entry_ndx]), sizeof(*entry));
  }
  else
  {
    soc_sand_os_memcpy(entry,&(ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.array_cache[entry_ndx]), sizeof(*entry));
  }

  return SOC_SAND_OK;
}

STATIC uint32
  arad_pp_sw_db_ipv4_arr_mem_allocator_free_entry_set(
    SOC_SAND_IN   int                          unit,
    SOC_SAND_IN   uint32                          mem_id,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_PTR         free_list
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  ARAD_PP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  SOC_DPP_WB_ENGINE_VAR wb_var_index;
  int
    res;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);

  is_cached = ARAD_PP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(mem_id);
  mem_inst = ARAD_PP_ARR_MEM_ALLOCATOR_BANK_INST(mem_id);

  wb_var_index = lpm_mngr->init_info.mem_allocators[mem_inst].wb_var_index;



  if (!is_cached)
  {
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, wb_var_index + WB_ENGINE_MEM_ALLOCATOR_FREE_LIST, 
                                      &(free_list));
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, wb_var_index + WB_ENGINE_MEM_ALLOCATOR_FREE_LIST_CACHE, 
                                      &(free_list));
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_ipv4_arr_mem_allocator_free_entry_set()",0,0);
}

STATIC uint32
  arad_pp_sw_db_ipv4_arr_mem_allocator_free_entry_get(
    SOC_SAND_IN   int                          unit,
    SOC_SAND_IN   uint32                          mem_id,
    SOC_SAND_OUT  ARAD_PP_ARR_MEM_ALLOCATOR_PTR        *free_list
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;

  is_cached = ARAD_PP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(mem_id);
  mem_inst = ARAD_PP_ARR_MEM_ALLOCATOR_BANK_INST(mem_id);

  if (!is_cached)
  {
    *free_list = ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.free_list;
  }
  else
  {
    *free_list = ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.free_list_cache;
  }

  return SOC_SAND_OK;

}

uint32
  arad_pp_ipv4_lpm_test_mem_to_mem_allocator_get(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  uint32                      mem_id,
      SOC_SAND_IN  uint32                      memory_ndx,
      SOC_SAND_OUT ARAD_PP_ARR_MEM_ALLOCATOR_INFO   **mem_allocator
  )
{
  ARAD_PP_IPV4_LPM_MNGR_INIT_INFO
    *lpm_init_info;
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

  if (Arad_pp_sw_db.device[unit]->ipv4_info == NULL)
  {
    return SOC_SAND_ERR;
  }
  lpm_init_info = &Arad_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info;
  
  *mem_allocator = &lpm_init_info->mem_allocators[memory_ndx];
  
  return soc_sand_ret;
}





uint32
  arad_pp_ipv4_lpm_vrf_to_lpm_db(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      sec_handle,
    SOC_SAND_IN  uint32                      vrf_ndx,
    SOC_SAND_OUT SOC_SAND_PAT_TREE_INFO            **pat_tree
  )
{

  ARAD_PP_IPV4_LPM_MNGR_INIT_INFO
    *lpm_init_info;
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

  if (Arad_pp_sw_db.device[unit]->ipv4_info == NULL)
  {
    return SOC_SAND_ERR;
  }

  lpm_init_info = &Arad_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info;

  *pat_tree = &lpm_init_info->lpms[vrf_ndx];
  
  return soc_sand_ret;
}




STATIC uint32
  arad_pp_sw_db_ipv4_arr_mem_allocator_write(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             bank_id,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY         *data
  )
{
  uint32
    tbl_data;
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  ARAD_PP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  ARAD_PP_IPV4_LPM_MNGR_INIT_INFO
    *lpm_init_info;
  ARAD_PP_IPV4_LPM_MNGR_INST
    inst;
  int32
    ptr;
  uint8
    is_cached;
  uint32
    inst_id;
  uint32
    res = SOC_SAND_OK;
  SOC_DPP_WB_ENGINE_VAR wb_var_index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);
  lpm_init_info = &(lpm_mngr->init_info);

  is_cached = ARAD_PP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  inst_id = ARAD_PP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);

  wb_var_index = lpm_mngr->init_info.mem_allocators[inst_id].wb_var_index;


  if (!is_cached)
  {
    arad_pp_ipv4_lpm_entry_decode(
      lpm_mngr,
      inst_id,
      data,
      1,
      &tbl_data
    );

    res = arad_pp_ihb_lpm_tbl_set_unsafe(
            unit,
            inst_id,
            offset,
            1,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

 
  if (is_cached)
  {

      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, wb_var_index + WB_ENGINE_MEM_ALLOCATOR_MEM_SHADOW_CACHE, 
                                      &(data[0]), 
                                      2*offset);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, wb_var_index + WB_ENGINE_MEM_ALLOCATOR_MEM_SHADOW_CACHE, 
                                      &(data[1]), 
                                      2*offset + 1);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  }
  else
  {
      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, wb_var_index + WB_ENGINE_MEM_ALLOCATOR_MEM_SHADOW, 
                                      &(data[0]), 
                                      2*offset);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, wb_var_index + WB_ENGINE_MEM_ALLOCATOR_MEM_SHADOW, 
                                      &(data[1]), 
                                      2*offset + 1);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

  }
 

  if (lpm_init_info->flags & ARAD_PP_LPV4_LPM_SUPPORT_DEFRAG){
    
    if (inst_id < lpm_init_info->nof_banks - 1)
    {
      inst.arr[0] = data[0];
      inst.arr[1] = data[1];
      res = arad_pp_ipv4_lpm_mngr_row_to_base_addr(lpm_mngr,&inst,inst_id, &ptr);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      if (ptr != -1)
      {
        
        res = soc_sand_group_mem_ll_member_add(
                lpm_mngr->init_info.rev_ptrs[inst_id+1],
                ptr,
                offset
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
      }
    }
  }
  
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_ipv4_arr_mem_allocator_write()",0,0);
}


void
  arad_pp_sw_db_ipv4_arr_mem_inst_to_data(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             nof_entries,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY    *inst_data,
    SOC_SAND_OUT  uint32                            *tbl_data
    ){

  uint32
    ind;

  for(ind = 0; ind < nof_entries; ++ind)
  {
    tbl_data[ind] = inst_data [ind*2];
  }

  return;
}


STATIC uint32
  arad_pp_sw_db_ipv4_arr_mem_allocator_write_block(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             bank_id,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             nof_entries,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY         *data
  )
{
  uint32
    tbl_data[256],
    indx;
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  ARAD_PP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  ARAD_PP_IPV4_LPM_MNGR_INIT_INFO
    *lpm_init_info;
  ARAD_PP_IPV4_LPM_MNGR_INST
    inst;
  int32
    ptr;
  uint8
    is_cached;
  uint32
    inst_id;
  uint32
    res = SOC_SAND_OK;
  SOC_DPP_WB_ENGINE_VAR wb_var_index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);
  lpm_init_info = &(lpm_mngr->init_info);

  is_cached = ARAD_PP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  inst_id = ARAD_PP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);


  

  wb_var_index = lpm_mngr->init_info.mem_allocators[inst_id].wb_var_index;


  arad_pp_sw_db_ipv4_arr_mem_inst_to_data(unit,nof_entries,data, tbl_data);

  if (!is_cached)
  {
     

    res = arad_pp_ihb_lpm_tbl_set_unsafe(
            unit,
            inst_id,
            offset,
            nof_entries,
            tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

 

  if (is_cached)
  {
      res = SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit,
                                         wb_var_index + WB_ENGINE_MEM_ALLOCATOR_MEM_SHADOW_CACHE, 
                                         data, 
                                         2*offset,
                                         2*nof_entries);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
      res = SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit,
                                         wb_var_index + WB_ENGINE_MEM_ALLOCATOR_MEM_SHADOW, 
                                         data, 
                                         2*offset,
                                         2*nof_entries);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
   }


 

  if (lpm_init_info->flags & ARAD_PP_LPV4_LPM_SUPPORT_DEFRAG){
    
      
    if (inst_id < lpm_init_info->nof_banks - 1)
    {

        for (indx = 0; indx < nof_entries; ++indx) {
          inst.arr[0] = data[indx*2 + 0];
          inst.arr[1] = data[indx*2 + 1];
          res = arad_pp_ipv4_lpm_mngr_row_to_base_addr(lpm_mngr,&inst,inst_id, &ptr);
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
          if (ptr != -1)
          {
            
            res = soc_sand_group_mem_ll_member_add(
                    lpm_mngr->init_info.rev_ptrs[inst_id+1],
                    ptr,
                    offset+indx
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
          }
        }
    }
  }
  
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_ipv4_arr_mem_allocator_write()",0,0);
}




STATIC uint32
arad_pp_sw_db_ipv4_arr_mem_allocator_read(
  SOC_SAND_IN  int                             unit,
  SOC_SAND_IN  uint32                             bank_id,
  SOC_SAND_IN  uint32                             offset,
  SOC_SAND_OUT  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY         *data
  )
{
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  ARAD_PP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  
  uint32
    inst_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);

  inst_id = ARAD_PP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);

  if (ARAD_PP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id))
  {
      data[0] = lpm_mngr->init_info.mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow_cache[2*offset];
      data[1] = lpm_mngr->init_info.mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow_cache[2*offset+1];
  }
  else
  {
    data[0] = lpm_mngr->init_info.mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow[2*offset];
    data[1] = lpm_mngr->init_info.mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow[2*offset+1];
  }
  
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_ipv4_arr_mem_allocator_read()",0,0);

}



STATIC uint32
arad_pp_sw_db_ipv4_arr_mem_allocator_read_block(
  SOC_SAND_IN  int                             unit,
  SOC_SAND_IN  uint32                             bank_id,
  SOC_SAND_IN  uint32                             offset,
  SOC_SAND_IN  uint32                             nof_entries,
  SOC_SAND_OUT  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY         *data
  )
{
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  ARAD_PP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  
  uint32
    inst_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);

  inst_id = ARAD_PP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);

  if (ARAD_PP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id))
  {
      soc_sand_os_memcpy(data,lpm_mngr->init_info.mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow_cache+2*offset,sizeof(uint32)*nof_entries*2);    
  }
  else
  {
      soc_sand_os_memcpy(data,lpm_mngr->init_info.mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow+2*offset,sizeof(uint32)*nof_entries*2);    
  }
  
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_ipv4_arr_mem_allocator_read()",0,0);

}

STATIC uint32
  arad_pp_sw_db_align_entry_ptr(
    SOC_SAND_IN   uint32            entry_place, 
    SOC_SAND_IN   uint32            key, 
    SOC_SAND_IN   uint32            new_ptr
 )
{
  uint32
    bank_id;
  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY
    data[3];
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  ARAD_PP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  ARAD_PP_IPV4_LPM_MNGR_INST
    inst;
  uint32
    mem_inst;
  uint32
	  ret_val;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  
  bank_id = key & (SOC_SAND_BIT(31)|0x7);
  mem_inst = ARAD_PP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  unit = (key & SOC_SAND_RBIT(31)) >> 3;
  if (mem_inst == 0 )
  {
    return SOC_SAND_ERR;
  }
  bank_id -= 1;
  mem_inst -= 1;

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);

  
  res = arad_pp_sw_db_ipv4_arr_mem_allocator_read(unit,bank_id,entry_place,data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  inst.arr[0] = data[0];
  inst.arr[1] = data[1];
  
  ret_val = arad_pp_ipv4_lpm_mngr_update_base_addr(lpm_mngr,&inst,mem_inst,new_ptr);

  
  if (ret_val != -1)
  {
    data[0] = inst.arr[0];
    data[1] = inst.arr[1];
    res = arad_pp_sw_db_ipv4_arr_mem_allocator_write(unit,bank_id,entry_place,data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_align_entry_ptr()",0,0);
}


uint32
  arad_pp_sw_db_ipv4_lpm_mem_align(
    SOC_SAND_IN   int                      unit,
    SOC_SAND_IN   uint32                      bank_id,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY   *data,
    SOC_SAND_IN   uint32                      old_place,
    SOC_SAND_IN   uint32                      new_place
 )
{
  uint32
    key;
  uint32
    mem_inst;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  mem_inst = ARAD_PP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  
  if (mem_inst == 0)
  {
    goto exit;
  }

  key = bank_id;
  key |= (unit<<3);

  res = soc_sand_group_mem_ll_func_run(
            Arad_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_inst],
            old_place,   
            arad_pp_sw_db_align_entry_ptr,
            key,        
            new_place
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_ipv4_lpm_mem_align()",0,0);
}




uint32
  arad_pp_sw_db_pat_tree_root_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    tree_ndx,
    SOC_SAND_IN SOC_SAND_PAT_TREE_NODE_PLACE    root_place
  )
{
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  ARAD_PP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  uint32
    tree_id;
  uint8
    cached;
  SOC_DPP_WB_ENGINE_VAR wb_var_index;
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Arad_pp_sw_db.device[unit] == NULL)
  {
    return SOC_SAND_ERR;
  }

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);

  tree_id = SOC_SAND_PAT_TREE_TREE_INST(tree_ndx);
  cached = SOC_SAND_PAT_TREE_IS_CACHED_INST(tree_ndx);

  wb_var_index = lpm_mngr->init_info.lpms[tree_id].wb_var_index;
 

  if (!cached)
  {
      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_ROOT, 
                                      &(root_place), 
                                      tree_id);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);      
  }
  else
  {
      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_ROOT_CACHE, 
                                      &(root_place), 
                                      tree_id);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);      
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_pat_tree_root_set()",0,0);
}


uint32
  arad_pp_sw_db_pat_tree_root_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    tree_ndx,
    SOC_SAND_OUT SOC_SAND_PAT_TREE_NODE_PLACE    *root_place
  )
{
  uint32
    tree_id;
  uint8
    cached;

  if (Arad_pp_sw_db.device[unit] == NULL)
  {
    return SOC_SAND_ERR;
  }

  tree_id = SOC_SAND_PAT_TREE_TREE_INST(tree_ndx);
  cached = SOC_SAND_PAT_TREE_IS_CACHED_INST(tree_ndx);


  if (!cached)
  {
    *root_place = Arad_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.root;
  }
  else
  {
    *root_place = Arad_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.root_cache;
  }

  return SOC_SAND_OK;
}


uint32
  arad_pp_sw_db_pat_tree_node_set(
    SOC_SAND_IN int                          unit,
    SOC_SAND_IN uint32                          tree_ndx,
    SOC_SAND_IN uint32                          node_place,
    SOC_SAND_IN SOC_SAND_PAT_TREE_NODE                *node_info
  )
{
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  ARAD_PP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  uint32
    tree_id;
  uint8
    cached;
  SOC_DPP_WB_ENGINE_VAR wb_var_index;
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);

  tree_id = SOC_SAND_PAT_TREE_TREE_INST(tree_ndx);
  cached = SOC_SAND_PAT_TREE_IS_CACHED_INST(tree_ndx);

  wb_var_index = lpm_mngr->init_info.lpms[tree_id].wb_var_index;
 
  if (!cached)
  {
      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_TREE_MEMORY, 
                                      node_info, 
                                      node_place);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);      
  }
  else
  {
      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_TREE_MEMORY_CACHE, 
                                      node_info, 
                                      node_place);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);      
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_pat_tree_node_set()",0,0);
}


uint32
  arad_pp_sw_db_pat_tree_node_get(
    SOC_SAND_IN int                          unit,
    SOC_SAND_IN uint32                           tree_ndx,
    SOC_SAND_IN uint32                            node_place,
    SOC_SAND_OUT SOC_SAND_PAT_TREE_NODE                  *node_info
  )
{
  uint32
    tree_id;
  uint8
    cached;

  tree_id = SOC_SAND_PAT_TREE_TREE_INST(tree_ndx);
  cached = SOC_SAND_PAT_TREE_IS_CACHED_INST(tree_ndx);

  if (!cached)
  {
    soc_sand_os_memcpy(
      node_info,
      &Arad_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory[node_place],
      sizeof(*node_info)
    );
  }
  else
  {
    soc_sand_os_memcpy(
      node_info,
      &Arad_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory_cache[node_place],
      sizeof(*node_info)
    );
  }

  return SOC_SAND_OK;
}

uint32
  arad_pp_sw_db_pat_tree_node_ref_get(
    SOC_SAND_IN int                          unit,
    SOC_SAND_IN uint32                           tree_ndx,
    SOC_SAND_IN uint32                            node_place,
    SOC_SAND_OUT SOC_SAND_PAT_TREE_NODE                  **node_info
  )
{
  uint32
    tree_id;
  uint8
    cached;

  tree_id = SOC_SAND_PAT_TREE_TREE_INST(tree_ndx);
  cached = SOC_SAND_PAT_TREE_IS_CACHED_INST(tree_ndx);

  if (!cached)
  {
    *node_info = &Arad_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory[node_place];
  }
  else
  {
    *node_info = &Arad_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory_cache[node_place];
  }

  return SOC_SAND_OK;
}

uint32
  arad_pp_sw_db_ipv4_lpm_dma_buff_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    **dma_buff
  )
{
  *dma_buff = Arad_pp_sw_db.device[unit]->ipv4_info->cm_buf_array;
  return SOC_SAND_OK;
}

uint32
  arad_pp_sw_db_free_list_add(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            bank_id,
    SOC_SAND_IN  uint32             address
  )
{
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  uint32
    cur_size;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;
  cur_size = (ipv4_info->free_list_size);
  if (cur_size >= ARAD_PP_SW_DB_FREE_LIST_MAX_SIZE)
  {
    return SOC_SAND_ERR;
  }
  ipv4_info->free_list[cur_size].bank_id = (uint8)bank_id;
  ipv4_info->free_list[cur_size].address = (uint16)address;
  ++(ipv4_info->free_list_size);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_free_list_add()",0,0);
}

uint32
  arad_pp_sw_db_free_list_commit(
    SOC_SAND_IN  int            unit
  )
{
  uint32
    indx,
    bank_indx,
    address;
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;

    
  for (indx = 0; indx < ipv4_info->free_list_size; ++indx)
  {
    bank_indx = ipv4_info->free_list[indx].bank_id;
    address = ipv4_info->free_list[indx].address;

    res = arad_pp_arr_mem_allocator_free(
            &(ipv4_info->lpm_mngr.init_info.mem_allocators[bank_indx]),
            address
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  ipv4_info->free_list_size = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_free_list_commit()",0,0);
}

int
  arad_pp_sw_db_ipv4_is_vrf_exist(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   vrf_ndx,
    SOC_SAND_OUT uint8                    *is_vrf_exist
  )
{
    uint32 nof_vrfs;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;
    rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_vrfs.get(unit, &nof_vrfs);
    SOCDNX_IF_ERR_EXIT(rv);
    *is_vrf_exist = (vrf_ndx < nof_vrfs) ? TRUE : FALSE;

exit:
    SOCDNX_FUNC_RETURN;
}


int
  arad_pp_sw_db_ipv4_is_routing_enabled(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT uint8                 *is_routing_enabled
  )
{
    uint32 nof_vrfs;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    *is_routing_enabled = FALSE;
    if(SOC_IS_JERICHO(unit)) {
        *is_routing_enabled = TRUE;
        SOC_EXIT;
    }

    if (Arad_pp_sw_db.device[unit] != NULL && Arad_pp_sw_db.device[unit]->ipv4_info != NULL) {
        rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_vrfs.get(unit, &nof_vrfs);
        SOCDNX_IF_ERR_EXIT(rv);
        if(nof_vrfs > 0) {
            *is_routing_enabled = TRUE;
            SOC_EXIT;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_pp_sw_db_ipv4_is_shared_lpm_memory(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                   vrf_ndx,
    SOC_SAND_OUT uint8                  *shared
  )
{
  uint32
    res,
    nof_vrfs,
    flags;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Arad_pp_sw_db.device[unit] == NULL || Arad_pp_sw_db.device[unit]->ipv4_info == NULL){
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }

  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.ipv4_info.nof_vrfs.get(unit, &nof_vrfs);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  if (nof_vrfs == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_ABOVE_MAX_ERR, 30, exit);
  }

  sw_state_access[unit].dpp.soc.arad.pp.oper_mode.ipv4_info.flags.get(unit, &flags);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  *shared = (uint8)(flags & ARAD_PP_MGMT_IPV4_SHARED_ROUTES_MEMORY);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_ipv4_is_shared_lpm_memory()",0,0);
}



uint32
  arad_pp_sw_db_init()
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_PP_SW_DB_INIT);

  SAL_GLOBAL_LOCK;
  soc_dpp_wb_engine_Arad_pp_sw_db_get(&Arad_pp_sw_db);

  if (Arad_pp_sw_db.init)
  {
    goto exit;
  }

  for (unit = 0; unit < SOC_SAND_MAX_DEVICE; ++unit)
  {
    Arad_pp_sw_db.device[unit] = NULL;
  }

  Arad_pp_sw_db.init = TRUE;

exit:
  SAL_GLOBAL_UNLOCK;
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in init()",0,0);
}


int
  arad_pp_sw_db_frwrd_ip_init(
    SOC_SAND_IN int unit
  )
{
    SOC_SAND_HASH_TABLE_PTR hash_tbl;
    SOC_SAND_HASH_TABLE_INIT_INFO init_info;
    soc_error_t                 rv;

    SOCDNX_INIT_FUNC_DEFS;

    
    sal_memset(&init_info,0, sizeof(init_info)) ;
    init_info.prime_handle  = unit;
    init_info.sec_handle    = 0;
    init_info.table_size    = 2 * SOC_DPP_DEFS_MAX(NOF_TCAM_BIG_BANK_LINES) * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS; 
    init_info.table_width   = 2 * SOC_DPP_DEFS_MAX(NOF_TCAM_BIG_BANK_LINES) * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS;
    init_info.key_size      = ARAD_PP_IP_TCAM_KEY_LEN_BYTES;
    init_info.data_size     = sizeof(uint32);
    init_info.get_entry_fun = NULL;
    init_info.set_entry_fun = NULL;

    rv = soc_sand_hash_table_create(
            unit,
            &hash_tbl,
            init_info
          );
    SOCDNX_SAND_IF_ERR_RETURN(rv);

    if(!SOC_WARM_BOOT(unit)) {
        
	    rv = sw_state_access[unit].dpp.soc.arad.pp.frwrd_ip.alloc(unit);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = sw_state_access[unit].dpp.soc.arad.pp.frwrd_ip.route_key_to_entry_id.set(unit, hash_tbl);
        SOCDNX_IF_ERR_EXIT(rv);
#if defined(INCLUDE_KBP)
        if SOC_DPP_IS_ELK_ENABLE(unit) {
            rv = sw_state_access[unit].dpp.soc.arad.pp.frwrd_ip.location_tbl.alloc(unit, ARAD_KBP_NL_88650_MAX_NOF_ENTRIES + SOC_DPP_DEFS_MAX_NOF_ENTRY_IDS);
            SOCDNX_IF_ERR_EXIT(rv);
        }
#endif 

      
  }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
  arad_pp_sw_db_vtt_init(
    SOC_SAND_IN int unit
  )
{
    SOC_SAND_HASH_TABLE_PTR hash_tbl;
    SOC_SAND_HASH_TABLE_INIT_INFO init_info;
    soc_error_t                 rv;

    SOCDNX_INIT_FUNC_DEFS;

    
    sal_memset(&init_info,0, sizeof(init_info)) ;
    init_info.prime_handle  = unit;
    init_info.sec_handle    = 0;
    init_info.table_size    = 2 * SOC_DPP_DEFS_MAX(NOF_TCAM_BIG_BANK_LINES) * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS; 
    init_info.table_width   = 2 * SOC_DPP_DEFS_MAX(NOF_TCAM_BIG_BANK_LINES) * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS;
    init_info.key_size      = ARAD_PP_SW_DB_HASH_KEY_LEN_BYTES;
    init_info.data_size     = sizeof(uint32);
    init_info.get_entry_fun = NULL;
    init_info.set_entry_fun = NULL;

    rv = soc_sand_hash_table_create(unit, &hash_tbl, init_info);
    SOCDNX_SAND_IF_ERR_RETURN(rv);
  
    rv = sw_state_access[unit].dpp.soc.arad.pp.vtt.alloc(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = sw_state_access[unit].dpp.soc.arad.pp.vtt.isem_key_to_entry_id.set(unit, hash_tbl);
    SOCDNX_IF_ERR_EXIT(rv);


    
    sal_memset(&init_info,0, sizeof(init_info)) ;
    init_info.prime_handle  = unit;
    init_info.sec_handle    = 0;
    init_info.table_size    = 2 * SOC_DPP_DEFS_MAX(NOF_TCAM_BIG_BANK_LINES) * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS; 
    init_info.table_width   = 2 * SOC_DPP_DEFS_MAX(NOF_TCAM_BIG_BANK_LINES) * SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS;
    init_info.key_size      = ARAD_PP_SW_DB_HASH_KEY_LEN_BYTES;
    init_info.data_size     = sizeof(uint32);
    init_info.get_entry_fun = NULL;
    init_info.set_entry_fun = NULL;

    rv = soc_sand_hash_table_create(unit, &hash_tbl, init_info);
    SOCDNX_SAND_IF_ERR_RETURN(rv);
  
    rv = sw_state_access[unit].dpp.soc.arad.pp.vtt_mask.alloc(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = sw_state_access[unit].dpp.soc.arad.pp.vtt_mask.isem_key_to_entry_id.set(unit, hash_tbl);
    SOCDNX_IF_ERR_EXIT(rv);


exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
  arad_pp_sw_db_src_bind_init(
    SOC_SAND_IN int      unit
  )
{
    SOC_SAND_OCC_BM_PTR         occ_bm_ptr;
    SOC_SAND_OCC_BM_INIT_INFO   btmp_init_info;
    soc_error_t                 rv;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  
    
    btmp_init_info.size = ARAD_PP_SRC_BIND_IPV4_MAX_SPOOF_ID;

    
    rv = soc_sand_occ_bm_create(unit, &btmp_init_info, &occ_bm_ptr);
    SOCDNX_SAND_IF_ERR_RETURN(rv);

    rv = sw_state_access[unit].dpp.soc.arad.pp.arad_sw_db_src_binds.alloc(unit); 
    SOCDNX_IF_ERR_EXIT(rv);

    rv = sw_state_access[unit].dpp.soc.arad.pp.arad_sw_db_src_binds.srcbind_arb_occ.set(unit, occ_bm_ptr);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_pp_sw_db_device_init(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE    *oper_mode
  )
{
  uint32
    res;
  soc_error_t rv;

  SOC_SAND_SUCCESS_FAILURE      success;
  uint32                        prtcl_tbl_ndx = 0;
  uint32                        l2_next_prtcl_type[1];
  uint8                         first_appear;
  uint8                         is_allocated;
  int                           core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SW_DB_DEVICE_INIT);

  if(!SOC_WARM_BOOT(unit)) {
      rv = sw_state_access[unit].dpp.soc.arad.pp.pon_double_lookup.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

      if(!is_allocated) {
          rv = sw_state_access[unit].dpp.soc.arad.pp.pon_double_lookup.alloc(unit);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);
      }

      rv = sw_state_access[unit].dpp.soc.arad.pp.fec.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

      if(!is_allocated) {
          rv = sw_state_access[unit].dpp.soc.arad.pp.fec.alloc(unit);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 40, exit);
      }

#if ARAD_WB_RECONFIG
        {
            int idx;
            for(idx = 0; idx < SOC_DPP_DEFS_MAX(NOF_FECS); ++idx) {
                rv = sw_state_access[unit].dpp.soc.arad.pp.fec.fec_entry_type.set(unit, idx, SOC_PPC_NOF_FEC_TYPES_ARAD);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 41, exit);
            }
            for(idx = 0; idx < SOC_DPP_DEFS_GET(unit, nof_flp_programs); ++idx) {
                rv = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.set(unit, idx, ARAD_PP_FLP_MAP_PROG_NOT_SET);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 42, exit);
            }
            for(idx = 0; idx < (1 << SOC_DPP_DEFS_MAX(NOF_LEM_PREFIXES)); ++idx) {
                rv = sw_state_access[unit].dpp.soc.arad.pp.fec.lem_prefix_mapping.set(unit, idx, ARAD_PP_FLP_MAP_PROG_NOT_SET);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 43, exit);
            }

			rv = sw_state_access[unit].dpp.soc.arad.pp.fec.is_protected.bit_range_clear(unit, 0, ARAD_PP_FEC_PROTECT_BITMAP_SIZE);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 44, exit);
        }
#endif

      rv = sw_state_access[unit].dpp.soc.arad.pp.diag.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

      if(!is_allocated) {
          rv = sw_state_access[unit].dpp.soc.arad.pp.diag.alloc(unit);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 45, exit);
      }

      rv = sw_state_access[unit].dpp.soc.arad.pp.header_data.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

      if(!is_allocated) {
          rv = sw_state_access[unit].dpp.soc.arad.pp.header_data.alloc(unit);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 50, exit);
      }

      if (SOC_INFO(unit).oamp_port[0] > 0) {
          rv = sw_state_access[unit].dpp.soc.arad.pp.oam_my_mac_lsb.is_allocated(unit, &is_allocated);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

          if(!is_allocated) {
              
              rv = sw_state_access[unit].dpp.soc.arad.pp.oam_my_mac_lsb.alloc(unit);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 60, exit);

              
              rv = sw_state_access[unit].dpp.soc.arad.pp.oam_my_mac_lsb.ref_count.ptr_alloc(unit, SOC_DPP_DEFS_GET(unit, nof_cores));
              SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 60, exit);

              
              SOC_DPP_CORES_ITER(SOC_CORE_ALL, core){
                  rv = sw_state_access[unit].dpp.soc.arad.pp.oam_my_mac_lsb.ref_count.alloc(unit, core, ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT_NUM);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 60, exit);
              }
          }

      }

      rv = sw_state_access[unit].dpp.soc.arad.pp.ilm_info.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

      if(!is_allocated) {
          rv = sw_state_access[unit].dpp.soc.arad.pp.ilm_info.alloc(unit);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 30, exit);
      }

#if ARAD_WB_RECONFIG
      
      rv = sw_state_access[unit].dpp.soc.arad.pp.ilm_info.mask_inrif.set(unit, TRUE);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 35, exit);
      rv = sw_state_access[unit].dpp.soc.arad.pp.ilm_info.mask_port.set(unit, TRUE);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 37, exit);
#endif

      rv = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

      if(!is_allocated) {
          rv = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.alloc(unit);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 40, exit);
      }

      rv = sw_state_access[unit].dpp.soc.arad.pp.lif_cos.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

      if(!is_allocated) {
          rv = sw_state_access[unit].dpp.soc.arad.pp.lif_cos.alloc(unit);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 55, exit);
      }

      rv = sw_state_access[unit].dpp.soc.arad.pp.lif_table.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

      if(!is_allocated) {
          rv = sw_state_access[unit].dpp.soc.arad.pp.lif_table.alloc(unit);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 60, exit);
      }

      rv = sw_state_access[unit].dpp.soc.arad.pp.lag.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

      if(!is_allocated) {
          rv = sw_state_access[unit].dpp.soc.arad.pp.lag.alloc(unit);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 100, exit);
      }

      rv = sw_state_access[unit].dpp.soc.arad.pp.vrrp_info.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

      if(!is_allocated) {
          rv = sw_state_access[unit].dpp.soc.arad.pp.vrrp_info.alloc(unit);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 110, exit);
      }

      if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0)) {
          rv = sw_state_access[unit].dpp.soc.arad.pp.slb_config.is_allocated(unit, &is_allocated);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

          if(!is_allocated) {
              rv = sw_state_access[unit].dpp.soc.arad.pp.slb_config.alloc(unit);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 155, exit);
          }
      }

      rv = arad_pp_sw_db_llp_filter_initialize(unit);
      SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

      rv = arad_pp_sw_db_llp_vid_assign_initialize(unit);
      SOC_SAND_CHECK_FUNC_RESULT(rv, 31, exit);

      rv = arad_pp_sw_db_ecmp_info_initialize(unit);
      SOC_SAND_CHECK_FUNC_RESULT(rv, 1337, exit);

      if (SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode != SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE)
      {
          rv = arad_pp_sw_db_src_bind_init(unit);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);
      }

      rv = sw_state_access[unit].dpp.soc.arad.pp.oam.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

      if(!is_allocated) {
          res = sw_state_access[unit].dpp.soc.arad.pp.oam.alloc(unit); 
          SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);
      }

      rv = sw_state_access[unit].dpp.soc.arad.pp.bfd.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

      if(!is_allocated) {
          res = sw_state_access[unit].dpp.soc.arad.pp.bfd.alloc(unit); 
          SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);
      }
      
      rv = arad_pp_sw_db_frwrd_ip_init(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 280, exit);

      rv = arad_pp_sw_db_vtt_init(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 290, exit);

      rv = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.alloc(unit); 
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 300, exit);

#if ARAD_WB_RECONFIG
      
      res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.set(unit, oper_mode);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
#endif

  }

  ARAD_ALLOC( Arad_pp_sw_db.device[unit], ARAD_PP_SW_DB_DEVICE, 1,"Arad_pp_sw_db.device[unit]" );

  if (!SOC_WARM_BOOT(unit)) {
      res = arad_pp_sw_db_llp_trap_initialize(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      res = arad_pp_sw_db_llp_mirror_initialize(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      res = arad_pp_sw_db_eg_mirror_initialize(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);
  

      res = arad_pp_sw_db_eg_encap_initialize(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

      res = arad_pp_sw_db_llp_cos_initialize(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

      res = arad_pp_sw_db_eth_policer_initialize(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  {
    
    uint32
      nof_vrf_bits = soc_sand_log2_round_up(oper_mode->ipv4_info.nof_vrfs), 
      nof_banks = 6, 
      
      nof_mems = ARAD_PP_IPV4_LPM_NOF_MEMS,  
      nof_rows_per_mem[ARAD_PP_IPV4_LPM_NOF_MEMS] =  {16*1024, 32*1024, 32*1024, 32*1024, 32*1024, 32*1024}, 
      mem_to_bank[6] = {0,1,2,3,4,5};
    uint32
      vrf_bits,
      bits_per_depth,
      nof_bits,
      dpth_indx,
      nof_vrfs;

    uint32
      nof_bits_per_bank[6] = {14,6,5,6,5,6}, 
      bank_to_mem[6] = {0,1,2,3,4,5}; 
    ARAD_PP_IPV4_LPM_PXX_MODEL
      pxx_model = ARAD_PP_IPV4_LPM_PXX_MODEL_P6M; 

    
    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && (oper_mode->ipv4_info.nof_vrfs > 0))
    {

    
    nof_vrfs = oper_mode->ipv4_info.nof_vrfs;

    vrf_bits = soc_sand_log2_round_up(nof_vrfs);

    
    if(!oper_mode->ipv4_info.bits_in_phase_valid) {
        nof_bits = 32 - (14 - vrf_bits);
#if 0
        LOG_INFO(BSL_LS_SOC_SWDB,
                 (BSL_META_U(unit,
                             "\n\r %-6lu|"), nof_vrfs));
#endif

        for (dpth_indx = 1 ; dpth_indx < 5; ++dpth_indx)
        {
          bits_per_depth = SOC_SAND_DIV_ROUND_DOWN(nof_bits, 5 - dpth_indx + 1);
          nof_bits_per_bank[dpth_indx] = bits_per_depth;
          nof_bits -= bits_per_depth;
        }
        nof_bits_per_bank[5] = nof_bits;
        nof_bits_per_bank[0] = 14;
        if (nof_bits_per_bank[1] > 4 && nof_bits_per_bank[5]<=4)
        {
          nof_bits_per_bank[1]-=2;
          nof_bits_per_bank[5]+=2;
        }
        else if (nof_bits_per_bank[1] > 3 && nof_bits_per_bank[5] <=5)
          {
            nof_bits_per_bank[1]-=1;
            nof_bits_per_bank[5]+=1;
          }
    }
    else{
        for(dpth_indx=0;dpth_indx<ARAD_PP_MGMT_IPV4_LPM_BANKS;dpth_indx++){
            nof_bits_per_bank[dpth_indx] = oper_mode->ipv4_info.bits_in_phase[dpth_indx];
        }
    }

    res = arad_pp_sw_db_ipv4_initialize(
            unit,
            oper_mode->ipv4_info.nof_vrfs,
            oper_mode->ipv4_info.max_routes_in_vrf,
            nof_vrf_bits,
            nof_banks,
            nof_bits_per_bank,
            bank_to_mem,
            mem_to_bank,
            nof_mems,
            nof_rows_per_mem,
            pxx_model,
            oper_mode->ipv4_info.flags,
            0 
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else
    {
      Arad_pp_sw_db.device[unit]->ipv4_info = NULL;
    }

  }

  if (!SOC_WARM_BOOT(unit)) {
      res = arad_pp_sw_db_l2_lif_initialize(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      res = arad_pp_sw_db_lif_ac_initialize(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);
  }


  if (SOC_IS_ARADPLUS_A0(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
    res = arad_pp_sw_db_rif_to_lif_group_map_initialize(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  }

#ifdef BCM_88660_A0
  if (!SOC_WARM_BOOT(unit)) {
      
      if (SOC_IS_ARADPLUS(unit) && SOC_IS_ROO_ENABLE(unit)) {
        res = arad_pp_sw_db_overlay_arp_initialize(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 156, exit);
      }
  }
#endif 


  if ((!SOC_WARM_BOOT(unit))) {
      
      *l2_next_prtcl_type = ARAD_PARSER_ETHER_PROTO_6_1588_ETHER_TYPE;	
      prtcl_tbl_ndx = ARAD_PARSER_ETHER_PROTO_6_1588;
      res = arad_sw_db_multiset_add_by_index(
              unit,
			  ARAD_SW_DB_CORE_ANY,
              ARAD_PP_SW_DB_MULTI_SET_ETHER_TYPE,
              l2_next_prtcl_type,
              prtcl_tbl_ndx,
              &first_appear,
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

      *l2_next_prtcl_type = ARAD_PARSER_ETHER_PROTO_7_MPLS_MC_ETHER_TYPE;
      prtcl_tbl_ndx = ARAD_PARSER_ETHER_PROTO_7_MPLS_MC;	
      res = arad_sw_db_multiset_add_by_index(
              unit,
			  ARAD_SW_DB_CORE_ANY,
              ARAD_PP_SW_DB_MULTI_SET_ETHER_TYPE,
              l2_next_prtcl_type,
              prtcl_tbl_ndx,
              &first_appear,
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

      *l2_next_prtcl_type = ARAD_PARSER_ETHER_PROTO_1_8021x_TPID_TYPE;
      prtcl_tbl_ndx = ARAD_PARSER_ETHER_PROTO_1_8021x;
      res = arad_sw_db_multiset_add_by_index(
              unit,
              ARAD_SW_DB_CORE_ANY,
              ARAD_PP_SW_DB_MULTI_SET_ETHER_TYPE,
              l2_next_prtcl_type,
              prtcl_tbl_ndx,
              &first_appear,
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

       if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "EoE_enable", 0)){
           *l2_next_prtcl_type = ARAD_PARSER_ETHER_PROTO_3_EoE_TPID_TYPE;
          prtcl_tbl_ndx = ARAD_PARSER_ETHER_PROTO_3_EoE;	
          res = arad_sw_db_multiset_add_by_index(
                  unit,
				  ARAD_SW_DB_CORE_ANY,
                  ARAD_PP_SW_DB_MULTI_SET_ETHER_TYPE,
                  l2_next_prtcl_type,
                  prtcl_tbl_ndx,
                  &first_appear,
                  &success
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
          
          
           *l2_next_prtcl_type = ARAD_PARSER_ETHER_PROTO_5_EoE_TPID_TYPE;
          prtcl_tbl_ndx = ARAD_PARSER_ETHER_PROTO_5_EoE;	
          res = arad_sw_db_multiset_add_by_index(
                  unit,
				  ARAD_SW_DB_CORE_ANY,
                  ARAD_PP_SW_DB_MULTI_SET_ETHER_TYPE,
                  l2_next_prtcl_type,
                  prtcl_tbl_ndx,
                  &first_appear,
                  &success
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);	
       }
  }

 
  if (SOC_IS_ARADPLUS(unit) && !SOC_WARM_BOOT(unit) &&
      (soc_property_get(unit, spn_BCM886XX_VXLAN_VPN_LOOKUP_MODE, 0) ||
       soc_property_get(unit, spn_BCM886XX_L2GRE_VPN_LOOKUP_MODE, 0)))
  {
    res = sw_state_access[unit].dpp.soc.arad.pp.vdc_vsi_map.alloc(unit, SOC_DPP_DEFS_GET(unit, nof_epni_isid));
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 160, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_device_init()",0,0);
}


uint32
  arad_pp_sw_db_device_close(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SW_DB_DEVICE_CLOSE);
  
  
  if (Arad_pp_sw_db.device[unit] == NULL)
  {
    ARAD_DO_NOTHING_AND_EXIT;
  }

  res = arad_pp_sw_db_ipv4_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);


  if (SOC_IS_ARADPLUS_A0(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
    res = arad_pp_sw_db_rif_to_lif_group_map_terminate(unit);
	SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
  }


  ARAD_FREE_ANY_SIZE(Arad_pp_sw_db.device[unit]);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_device_close()",0,0);
}


int
  arad_pp_sw_db_multiset_by_type_get(
    SOC_SAND_IN  int                         unit,
	SOC_SAND_IN	 int						 core_id,
    SOC_SAND_IN  ARAD_PP_SW_DB_MULTI_SET     multiset,
	SOC_SAND_OUT SOC_SAND_MULTI_SET_PTR*     multi_set_info
  )
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    switch(multiset)
    {
    case ARAD_PP_SW_DB_MULTI_SET_L3_PROTOCOL:
      rv = sw_state_access[unit].dpp.soc.arad.pp.llp_trap.l3_protocols_multi_set.get(unit, multi_set_info);
      break;
    case ARAD_PP_SW_DB_MULTI_SET_LLP_MIRROR_PROFILE:
      rv = sw_state_access[unit].dpp.soc.arad.pp.llp_mirror.mirror_profile_multi_set.get(unit, multi_set_info);
      break;
    case ARAD_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE:
      rv = sw_state_access[unit].dpp.soc.arad.pp.eg_mirror.mirror_profile_multi_set.get(unit, multi_set_info);
      break;
    case ARAD_PP_SW_DB_MULTI_SET_ETHER_TYPE:
      rv = sw_state_access[unit].dpp.soc.arad.pp.llp_cos.ether_type_multi_set.get(unit, multi_set_info);
      break;
	case ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE:
		if ((core_id < 0) ||(core_id >= SOC_DPP_CONFIG(unit)->meter.nof_meter_cores)) {
			SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid Core %d for eth. profile multiset"),core_id));
		}
      rv = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.eth_meter_profile_multi_set.get(unit, core_id, multi_set_info);
      break;
	case ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE:
		if ((core_id < 0) ||(core_id >= SOC_DPP_CONFIG(unit)->meter.nof_meter_cores)) {
			SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid Core for eth. profile multiset")));
		}
      rv = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.global_meter_profile_multi_set.get(unit, core_id, multi_set_info);
      break;
    case ARAD_PP_SW_DB_MULTI_SET_L2_LIF_VLAN_COMPRESSION_RANGE:
      rv = sw_state_access[unit].dpp.soc.arad.pp.l2_lif.vlan_compression_range_multi_set.get(unit, multi_set_info);
      break;
    case ARAD_PP_SW_DB_MULTI_SET_L2_LIF_PROFILE:
      rv = sw_state_access[unit].dpp.soc.arad.pp.l2_lif_ac.ac_key_map_multi_set.get(unit, multi_set_info);
      break;
    case ARAD_PP_SW_DB_MULTI_SET_EG_ENCAP_PROG_DATA_ENTRY:
      rv = sw_state_access[unit].dpp.soc.arad.pp.eg_encap.prog_data.get(unit, multi_set_info);
      break;

    case ARAD_PP_SW_DB_MULTI_SET_ENTRY_OVERLAY_ARP_PROG_DATA_ENTRY: 
      rv = sw_state_access[unit].dpp.soc.arad.pp.overlay_arp.prog_data.get(unit, multi_set_info);
      break;

#if defined(BCM_88675_A0)
	case JER_PP_SW_DB_MULTI_SET_POLICER_SIZE_PROFILE:
	  if ((core_id < 0) ||(core_id >= SOC_DPP_CONFIG(unit)->meter.nof_meter_cores)) {
		  SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid Core for policer packet size profile multiset")));
	  }
      rv = sw_state_access[unit].dpp.soc.arad.pp.policer_size_profile.policer_size_profile_multi_set.get(unit, core_id, multi_set_info);
      break;
#endif 

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid multiset")));
    }
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
  arad_pp_sw_db_llp_filter_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint8 is_allocated;
  soc_error_t res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.pp.llp_filter.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

  if(!is_allocated) {
      res = sw_state_access[unit].dpp.soc.arad.pp.llp_filter.alloc(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_llp_filter_initialize()",0,0);
}


STATIC uint32
  arad_pp_sw_db_lpm_ll_member_entry_set(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   uint32                            bank_id,
    SOC_SAND_IN  uint32                             member_ndx,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY      *member_entry
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  ARAD_PP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  SOC_DPP_WB_ENGINE_VAR wb_var_index;
  int
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);


  is_cached = ARAD_PP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  mem_inst = ARAD_PP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);

  wb_var_index = lpm_mngr->init_info.rev_ptrs[mem_inst]->wb_var_index;


  if(is_cached) {
      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, wb_var_index + WB_ENGINE_GROUP_MEM_LL_MEMBERS_CACHE, 
                                      member_entry, 
                                      member_ndx);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  } else {
      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, wb_var_index + WB_ENGINE_GROUP_MEM_LL_MEMBERS, 
                                      member_entry, 
                                      member_ndx);      
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_lpm_ll_member_entry_set()",0,0);
}

STATIC uint32
  arad_pp_sw_db_lpm_ll_member_entry_get(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   uint32                            bank_id,
    SOC_SAND_IN  uint32                             member_ndx,
    SOC_SAND_OUT  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY      *member_entry
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  ARAD_PP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);

  is_cached = ARAD_PP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  mem_inst = ARAD_PP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  
  ARAD_COPY(
             member_entry,
             &(ARAD_PP_SW_DB_ACTIVE_INS(is_cached, lpm_mngr->init_info.rev_ptrs[mem_inst]->group_members_data.members)[member_ndx]),
             SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY,
             1
            );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_lpm_ll_member_entry_get()",0,0);
}


STATIC uint32
  arad_pp_sw_db_lpm_ll_group_entry_set(
    SOC_SAND_IN   int                          unit,
    SOC_SAND_IN   uint32                          bank_id,
    SOC_SAND_IN  uint32                           group_ndx,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY      *group_entry
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  ARAD_PP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  uint32
    res;
  SOC_DPP_WB_ENGINE_VAR wb_var_index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);

  is_cached = ARAD_PP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  mem_inst = ARAD_PP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);

  wb_var_index = lpm_mngr->init_info.rev_ptrs[mem_inst]->wb_var_index;

  if(is_cached) {
      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, wb_var_index + WB_ENGINE_GROUP_MEM_LL_GROUPS_CACHE, 
                                      group_entry, 
                                      group_ndx);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  } else {
      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, wb_var_index + WB_ENGINE_GROUP_MEM_LL_GROUPS, 
                                      group_entry, 
                                      group_ndx);      
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_lpm_ll_group_entry_set()",0,0);
}

STATIC uint32
  arad_pp_sw_db_lpm_ll_group_entry_get(
    SOC_SAND_IN   int                          unit,
    SOC_SAND_IN   uint32                          bank_id,
    SOC_SAND_IN  uint32                             group_ndx,
    SOC_SAND_OUT  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY      *group_entry
  )
{

  uint32
    mem_inst;
  uint8
    is_cached;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  is_cached = ARAD_PP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  mem_inst = ARAD_PP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  
  ARAD_COPY(
             group_entry,
             &(ARAD_PP_SW_DB_ACTIVE_INS(is_cached,Arad_pp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_inst]->group_members_data.groups)[group_ndx]),
             SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY,
             1
            );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_lpm_ll_group_entry_get()",0,0);
}


uint32
  arad_pp_sw_db_ipv4_initialize(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  nof_vrfs,
    SOC_SAND_IN uint32  *max_nof_routes,
    SOC_SAND_IN uint32  nof_vrf_bits,
    SOC_SAND_IN uint32  nof_banks,
    SOC_SAND_IN uint32  *nof_bits_per_bank,
    SOC_SAND_IN uint32  *bank_to_mem,
    SOC_SAND_IN uint32  *mem_to_bank,
    SOC_SAND_IN uint32  nof_mems,
    SOC_SAND_IN uint32  *nof_rows_per_mem, 
    SOC_SAND_IN ARAD_PP_IPV4_LPM_PXX_MODEL pxx_model,
    SOC_SAND_IN uint32  flags,
    SOC_SAND_IN SOC_SAND_PP_SYSTEM_FEC_ID  default_sys_fec
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    vrf_indx,
    max_depth=0,
    dpth_indx,
    bits_in_bank1;
  uint32
    indx;
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;
  ARAD_PP_IPV4_LPM_MNGR_INIT_INFO
    *lpm_init_info;
  uint8
    support_defrag;
  SOC_SAND_GROUP_MEM_LL_INFO
    *members_info;
  uint8
    is_allocated;

  uint32
      lpm_data,
      vrf_start_index,
      vrf_end_index;
  SOC_DPP_WB_ENGINE_VAR wb_engine_mem_allocator_vars [ARAD_PP_IPV4_LPM_NOF_MEMS] = 
      {
          SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_1,
          SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_2,
          SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_3,
          SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_4,
          SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_5,
          SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_ARR_MEM_ALLOCATOR_6          
      };
  SOC_DPP_WB_ENGINE_VAR wb_engine_group_mem_ll_vars [ARAD_PP_IPV4_LPM_NOF_MEMS] = 
      {
          -1,
          SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_1,
          SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_2,
          SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_3,
          SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_4,
          SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_GROUP_MEM_LL_5          
      };

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if ((!SOC_WARM_BOOT(unit))) {
      res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_pat.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 36, exit);

      if(!is_allocated) {
          res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_pat.alloc(unit);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 37, exit);
      }
      res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.is_allocated(unit, &is_allocated);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 38, exit);

      res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.alloc(unit); 
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 39, exit);
  }

  ARAD_ALLOC_ANY_SIZE(
    Arad_pp_sw_db.device[unit]->ipv4_info,
    ARAD_PP_SW_DB_IPV4_INFO,
    1,
    "ipv4_info pp_sw_db_ipv4"
  );

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;

  soc_sand_os_memset(
    ipv4_info,
    0x0,
    sizeof(*ipv4_info)
  );

  if (!SOC_WARM_BOOT(unit))
  {
      res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_vrfs.set(unit, nof_vrfs);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

      res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.default_fec.set(unit, default_sys_fec);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

       
      res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_lpm_entries_in_lpm.set(unit, 0);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }

  arad_pp_ARAD_PP_IPV4_LPM_MNGR_INFO_clear(&ipv4_info->lpm_mngr);

  ipv4_info->free_list_size = 0;
  ARAD_ALLOC(ipv4_info->free_list,ARAD_PP_IPV4_LPM_FREE_LIST_ITEM_INFO,ARAD_PP_SW_DB_FREE_LIST_MAX_SIZE, "ipv4_info->free_list");
  

  lpm_init_info = &(ipv4_info->lpm_mngr.init_info);

  lpm_init_info->prime_handle = unit;
  lpm_init_info->sec_handle = 0;
  lpm_init_info->nof_vrf_bits = nof_vrf_bits;
  lpm_init_info->nof_banks = nof_banks;

  SOC_SAND_MALLOC(lpm_init_info->nof_bits_per_bank, sizeof(*lpm_init_info->nof_bits_per_bank) * nof_banks, "nof_bits_per_bank");
  soc_sand_os_memcpy(lpm_init_info->nof_bits_per_bank, nof_bits_per_bank, sizeof(*lpm_init_info->nof_bits_per_bank) * nof_banks);
  
  SOC_SAND_MALLOC(lpm_init_info->bank_to_mem, sizeof(*lpm_init_info->bank_to_mem) * nof_banks,"bank_to_mem");
  soc_sand_os_memcpy(lpm_init_info->bank_to_mem, bank_to_mem, sizeof(*lpm_init_info->bank_to_mem) * nof_banks);


  SOC_SAND_MALLOC(lpm_init_info->mem_to_bank, sizeof(*lpm_init_info->mem_to_bank) * nof_mems,"mem_to_bank");
  soc_sand_os_memcpy(lpm_init_info->mem_to_bank, mem_to_bank, sizeof(*lpm_init_info->mem_to_bank) * nof_mems);

  lpm_init_info->nof_mems = nof_mems;
  
  SOC_SAND_MALLOC(lpm_init_info->nof_rows_per_mem, sizeof(*lpm_init_info->nof_rows_per_mem) * nof_mems, "nof_rows_per_mem");
  soc_sand_os_memcpy(lpm_init_info->nof_rows_per_mem, nof_rows_per_mem, sizeof(*lpm_init_info->nof_rows_per_mem) * nof_mems);
  
  lpm_init_info->mem_alloc_get_fun = arad_pp_ipv4_lpm_test_mem_to_mem_allocator_get;
  lpm_init_info->pat_tree_get_fun = arad_pp_ipv4_lpm_vrf_to_lpm_db;
  lpm_init_info->pxx_model = pxx_model;
  lpm_init_info->nof_lpms = nof_vrfs;

  lpm_init_info->nof_entries_for_hw_lpm_set_fun = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_lpm_entries_in_lpm.set;
  lpm_init_info->nof_entries_for_hw_lpm_get_fun = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_lpm_entries_in_lpm.get;
  lpm_init_info->max_nof_entries_for_hw_lpm = 10 * 1024;
  support_defrag = (uint8) flags & ARAD_PP_MGMT_IPV4_OP_MODE_SUPPORT_DEFRAG;
 
  SOC_SAND_MALLOC(lpm_init_info->lpms, sizeof(SOC_SAND_PAT_TREE_INFO) * lpm_init_info->nof_lpms, "lpm_init_info->lpms");
  sw_state_access[unit].dpp.soc.arad.pp.ipv4_pat.counter.set(unit, 0);
  if (!SOC_WARM_BOOT(unit)) {
      sw_state_access[unit].dpp.soc.arad.pp.ipv4_pat.nof.set(unit, lpm_init_info->nof_lpms);
      sw_state_access[unit].dpp.soc.arad.pp.ipv4_pat.occ_bm_hndl.alloc(unit, lpm_init_info->nof_lpms);
  }
  

  for (indx = 0; indx < lpm_init_info->nof_lpms; ++indx)
  {
      soc_sand_SAND_PAT_TREE_INFO_clear(&(lpm_init_info->lpms[indx]));
  }

  if (flags & ARAD_PP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE)
  {
    lpm_init_info->flags |= ARAD_PP_LPV4_LPM_SUPPORT_CACHE;
  }
  if (flags & ARAD_PP_MGMT_IPV4_OP_MODE_SUPPORT_DEFRAG)
  {
    lpm_init_info->flags |= ARAD_PP_LPV4_LPM_SUPPORT_DEFRAG;
  }
  if (flags & ARAD_PP_MGMT_IPV4_SHARED_ROUTES_MEMORY)
  {
    lpm_init_info->flags |= ARAD_PP_LPV4_LPM_SHARED_MEM;
  }
  
  for (indx = 0; indx < lpm_init_info->nof_lpms; ++indx)
  {
    lpm_init_info->lpms[indx].tree_size = max_nof_routes[indx]*2; 
    lpm_init_info->lpms[indx].node_set_fun = arad_pp_sw_db_pat_tree_node_set;
    lpm_init_info->lpms[indx].node_get_fun = arad_pp_sw_db_pat_tree_node_get;
    lpm_init_info->lpms[indx].node_ref_get_fun = NULL;
    lpm_init_info->lpms[indx].root_set_fun = arad_pp_sw_db_pat_tree_root_set;
    lpm_init_info->lpms[indx].root_get_fun = arad_pp_sw_db_pat_tree_root_get;
    lpm_init_info->lpms[indx].prime_handle = unit;
    lpm_init_info->lpms[indx].sec_handle = indx;
    lpm_init_info->lpms[indx].support_cache = (uint8) flags & ARAD_PP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE;

    lpm_init_info->lpms[indx].wb_var_index  = SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_LPMS;
    lpm_init_info->lpms[indx].wb_var_index  = SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO_LPMS;
    lpm_init_info->lpms[indx].pat_tree_data.memory_use_unit = unit;
  }
  
  SOC_SAND_MALLOC(lpm_init_info->mem_allocators, sizeof(ARAD_PP_ARR_MEM_ALLOCATOR_INFO) * lpm_init_info->nof_mems, "mem_allocators");

  for (indx = 0; indx < lpm_init_info->nof_mems; ++indx)
  {
    ARAD_PP_ARR_MEM_ALLOCATOR_INFO_clear(&(lpm_init_info->mem_allocators[indx]));
    lpm_init_info->mem_allocators[indx].entry_get_fun = arad_pp_sw_db_ipv4_arr_mem_allocator_entry_get;
    lpm_init_info->mem_allocators[indx].entry_set_fun = arad_pp_sw_db_ipv4_arr_mem_allocator_entry_set;
    lpm_init_info->mem_allocators[indx].free_set_fun = arad_pp_sw_db_ipv4_arr_mem_allocator_free_entry_set;
    lpm_init_info->mem_allocators[indx].free_get_fun = arad_pp_sw_db_ipv4_arr_mem_allocator_free_entry_get;
    lpm_init_info->mem_allocators[indx].write_fun = arad_pp_sw_db_ipv4_arr_mem_allocator_write;
    lpm_init_info->mem_allocators[indx].write_block_fun = arad_pp_sw_db_ipv4_arr_mem_allocator_write_block;
    lpm_init_info->mem_allocators[indx].read_fun = arad_pp_sw_db_ipv4_arr_mem_allocator_read;
    lpm_init_info->mem_allocators[indx].read_block_fun = arad_pp_sw_db_ipv4_arr_mem_allocator_read_block;
    lpm_init_info->mem_allocators[indx].entry_move_fun = arad_pp_sw_db_ipv4_lpm_mem_align;
    lpm_init_info->mem_allocators[indx].instance_prim_handle = unit;
    lpm_init_info->mem_allocators[indx].instance_sec_handle = indx;
    lpm_init_info->mem_allocators[indx].entry_size = 2; 
    lpm_init_info->mem_allocators[indx].nof_entries = lpm_init_info->nof_rows_per_mem[indx];
    lpm_init_info->mem_allocators[indx].support_caching = (uint8) flags & ARAD_PP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE;
    lpm_init_info->mem_allocators[indx].support_defragment = (indx==0)?FALSE:support_defrag;
    lpm_init_info->mem_allocators[indx].max_block_size = (indx==0)?1:((1 << nof_bits_per_bank[indx])*2);

    lpm_init_info->mem_allocators[indx].wb_var_index = wb_engine_mem_allocator_vars[indx];

  }
  
  if (lpm_init_info->flags & ARAD_PP_LPV4_LPM_SUPPORT_DEFRAG)
  {
    ARAD_ALLOC_ANY_SIZE(lpm_init_info->rev_ptrs,SOC_SAND_GROUP_MEM_LL_INFO*,lpm_init_info->nof_mems, "rev_ptrs pp_sw_db_ipv4");
    
    
    for (indx = 1; (uint32)indx < lpm_init_info->nof_mems; ++indx)
    {
      ARAD_ALLOC_ANY_SIZE(lpm_init_info->rev_ptrs[indx],SOC_SAND_GROUP_MEM_LL_INFO,1, "rev_ptrs[indx] pp_sw_db_ipv4");
      members_info = lpm_init_info->rev_ptrs[indx];
      soc_sand_SAND_GROUP_MEM_LL_INFO_clear(members_info);
      members_info->auto_remove = TRUE;
      members_info->group_set_fun = arad_pp_sw_db_lpm_ll_group_entry_set;
      members_info->group_get_fun = arad_pp_sw_db_lpm_ll_group_entry_get;
      members_info->member_set_fun = arad_pp_sw_db_lpm_ll_member_entry_set;
      members_info->member_get_fun = arad_pp_sw_db_lpm_ll_member_entry_get;
      members_info->nof_elements = lpm_init_info->nof_rows_per_mem[indx-1];

      members_info->nof_groups = lpm_init_info->nof_rows_per_mem[indx];; 
      members_info->instance_prim_handle = unit;
      members_info->instance_sec_handle = indx; 
      members_info->support_caching = (uint8)lpm_init_info->flags & ARAD_PP_LPV4_LPM_SUPPORT_CACHE;

      members_info->wb_var_index = wb_engine_group_mem_ll_vars[indx];


    }
  }
 
  res = arad_pp_ipv4_lpm_mngr_create(&ipv4_info->lpm_mngr, unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 
  
  for (dpth_indx = 1 ; dpth_indx < 5; ++dpth_indx)
  {
      if(nof_bits_per_bank[dpth_indx] > max_depth){
          max_depth = nof_bits_per_bank[dpth_indx];
      }
  }
  
  max_depth +=3;
  ipv4_info->cm_buf_entry_words = soc_mem_entry_words(unit, IHB_LPMm);
  ipv4_info->cm_buf_array = soc_cm_salloc(unit, ipv4_info->cm_buf_entry_words  * sizeof(uint32) * (1 << max_depth), "cm_lpm_buffer");

  

  res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_IPV4_INFO);
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  
  if (SOC_WARM_BOOT(unit))
  {
      goto exit;
  }

   
  
  for (indx = 0; indx < lpm_init_info->nof_mems; ++indx)
  {
      res = arad_pp_arr_mem_allocator_initialize(&(lpm_init_info->mem_allocators[indx]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }


  for(vrf_indx = 0; vrf_indx < nof_vrfs; ++vrf_indx)
  {
    arad_pp_ipv4_lpm_mngr_vrf_init(
      unit,
      &ipv4_info->lpm_mngr,
      vrf_indx,
      default_sys_fec
    );
  }




  if(SAL_BOOT_QUICKTURN)
  {
      
      bits_in_bank1 = ipv4_info->lpm_mngr.data_info.bit_depth_per_bank[1];

      vrf_indx = 0;
      vrf_start_index = vrf_indx << (bits_in_bank1 - nof_vrf_bits);
      vrf_end_index = ((vrf_indx + nof_vrfs-1) << (bits_in_bank1 - nof_vrf_bits))+ (uint32)(1 <<  (bits_in_bank1 - nof_vrf_bits));

      lpm_data = 0x70000000;

      for(indx = vrf_start_index; indx < vrf_end_index; ++indx)
      {
          lpm_init_info->mem_allocators[0].arr_mem_allocator_data.mem_shadow[2*indx] = lpm_data;
          lpm_init_info->mem_allocators[0].arr_mem_allocator_data.mem_shadow[2*indx+1] = 0;
      }

      res = arad_fill_partial_table_with_entry(unit,IHB_LPMm,0,0,MEM_BLOCK_ANY,vrf_start_index,vrf_end_index-1, &lpm_data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_ipv4_initialize()",0,0);
}


uint32
  arad_pp_sw_db_ipv4_terminate(
    SOC_SAND_IN  int unit
  )
{
  ARAD_PP_SW_DB_IPV4_INFO
    *ipv4_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  ARAD_PP_SW_DB_NULL_CHECK(unit, ipv4_info);

  ipv4_info = Arad_pp_sw_db.device[unit]->ipv4_info;
  if (ipv4_info == NULL)
  {
    return SOC_SAND_OK;
  }

  ARAD_FREE_ANY_SIZE(ipv4_info->free_list);


  arad_pp_ipv4_lpm_mngr_destroy(&ipv4_info->lpm_mngr);
  soc_sand_os_free_any_size(ipv4_info->lpm_mngr.init_info.nof_bits_per_bank);
  soc_sand_os_free_any_size(ipv4_info->lpm_mngr.init_info.bank_to_mem);
  soc_sand_os_free_any_size(ipv4_info->lpm_mngr.init_info.mem_to_bank);
  soc_sand_os_free_any_size(ipv4_info->lpm_mngr.init_info.nof_rows_per_mem);
  soc_sand_os_free_any_size(ipv4_info->lpm_mngr.init_info.lpms);
  soc_sand_os_free_any_size(ipv4_info->lpm_mngr.init_info.mem_allocators);
  soc_cm_sfree(unit, ipv4_info->cm_buf_array);

  ARAD_FREE_ANY_SIZE(Arad_pp_sw_db.device[unit]->ipv4_info);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_ipv4_terminate()",0,0);
}


uint32
  arad_pp_sw_db_ipv4_cache_mode_for_ip_type_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32                                     type,
    SOC_SAND_OUT uint8	                          	     *pending_op
)
{
  soc_error_t rv;
  uint32 bitmask = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (SOC_IS_JERICHO(unit)) {
      

      *pending_op = FALSE;
  } else {
      rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.cache_mode.get(
              unit,
             &bitmask
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

      *pending_op = (type & bitmask) ? TRUE : FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_ipv4_cache_mode_for_ip_type_get()",0,0);
}

uint32
  arad_pp_sw_db_ipv4_cache_vrf_modified_set(
    SOC_SAND_IN int                                  unit,
    SOC_SAND_IN  uint32                                  vrf_ndx,
    SOC_SAND_IN uint32                                   vrf_modified
)
{
  soc_error_t rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(vrf_modified) {
    rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_set(unit, vrf_ndx);
  } else{
    rv = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_clear(unit, vrf_ndx);
  }

  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_ipv4_cache_vrf_modified_set()",vrf_ndx,0);
}

uint32
  arad_pp_sw_db_ipv4_lpm_mngr_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr
  )
{
    ARAD_PP_SW_DB_FIELD_SET(
      unit,
      ipv4_info->lpm_mngr,
      lpm_mngr
    );
}

uint32
  arad_pp_sw_db_ipv4_lpm_mngr_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT  ARAD_PP_IPV4_LPM_MNGR_INFO  *lpm_mngr
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    nof_vrfs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Arad_pp_sw_db.device[unit] == NULL || Arad_pp_sw_db.device[unit]->ipv4_info == NULL){
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }

  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.ipv4_info.nof_vrfs.get(unit, &nof_vrfs);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  if (nof_vrfs == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_ABOVE_MAX_ERR, 30, exit);
  }

  res = soc_sand_os_memcpy(
      lpm_mngr,
      &(Arad_pp_sw_db.device[unit]->ipv4_info->lpm_mngr),
      sizeof(*lpm_mngr)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_ipv4_lpm_mngr_get()",0,0);
}





uint32
  arad_pp_sw_db_llp_trap_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;
  SOC_SAND_MULTI_SET_PTR
    multi_set_info;
  SOC_SAND_MULTI_SET_INIT_INFO
    multi_set_init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  

  
  
  soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
  multi_set_init_info.get_entry_fun    = NULL;
  multi_set_init_info.set_entry_fun    = NULL;
  multi_set_init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  multi_set_init_info.member_size      = sizeof(uint32);
  multi_set_init_info.nof_members      = ARAD_PP_LLP_TRAP_NOF_UD_L3_PROTOCOLS;
  multi_set_init_info.sec_handle       = 0;

  multi_set_init_info.prime_handle     = unit;

  res = soc_sand_multi_set_create(
    unit,
    &multi_set_info,
    multi_set_init_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  res = sw_state_access[unit].dpp.soc.arad.pp.llp_trap.alloc(unit);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.llp_trap.l3_protocols_multi_set.set(unit, multi_set_info);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_llp_trap_initialize()",0,0);
}




uint32
  arad_pp_sw_db_llp_mirror_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_MULTI_SET_PTR
    multi_set_info;
  SOC_SAND_MULTI_SET_INIT_INFO
    multi_set_init_info;
  SOC_PPC_PORT
    pp_port_ndx;
  int core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  res = sw_state_access[unit].dpp.soc.arad.pp.llp_mirror.alloc(unit);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
      for (pp_port_ndx = 0; pp_port_ndx < ARAD_PORT_NOF_PP_PORTS; ++pp_port_ndx)
      {
          res = sw_state_access[unit].dpp.soc.arad.pp.llp_mirror.llp_mirror_port_vlan.set(unit, core, pp_port_ndx, 0);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
      }
  }
  

  soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
  multi_set_init_info.get_entry_fun    = NULL;
  multi_set_init_info.set_entry_fun    = NULL;
  multi_set_init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  multi_set_init_info.member_size      = sizeof(uint32);
  multi_set_init_info.nof_members      = ARAD_PP_LLP_MIRROR_NOF_VID_MIRROR_INDICES;
  multi_set_init_info.sec_handle       = 0;

  multi_set_init_info.prime_handle     = unit;

  res = soc_sand_multi_set_create(unit, &multi_set_info, multi_set_init_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.llp_mirror.mirror_profile_multi_set.set(unit, multi_set_info);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_llp_mirror_initialize()",0,0);
}





uint32
  arad_pp_sw_db_llp_vid_assign_initialize(
    SOC_SAND_IN  int unit
  )
{
  soc_error_t
    res;
  uint8
    is_allocated;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  
  res = sw_state_access[unit].dpp.soc.arad.pp.llp_vid_assign.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

  if(!is_allocated) {
      res = sw_state_access[unit].dpp.soc.arad.pp.llp_vid_assign.alloc(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 133, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_llp_vid_assign_initialize()",0,0);
}




uint32
  arad_pp_sw_db_eg_mirror_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_MULTI_SET_PTR
    multi_set_info;
  SOC_SAND_MULTI_SET_INIT_INFO
    multi_set_init_info;
  SOC_PPC_PORT
    pp_port_ndx;
  int core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
  multi_set_init_info.get_entry_fun    = NULL;
  multi_set_init_info.set_entry_fun    = NULL;
  multi_set_init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  multi_set_init_info.member_size      = sizeof(uint32);
  multi_set_init_info.nof_members      = ARAD_PP_EG_MIRROR_NOF_VID_MIRROR_INDICES;
  multi_set_init_info.sec_handle       = 0;

  multi_set_init_info.prime_handle     = unit;

  res = soc_sand_multi_set_create(
    unit,
    &multi_set_info,
    multi_set_init_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.eg_mirror.alloc(unit);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.eg_mirror.mirror_profile_multi_set.set(unit, multi_set_info);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  
  SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
      for (pp_port_ndx = 0; pp_port_ndx < ARAD_PORT_NOF_PP_PORTS; ++pp_port_ndx)
      {
          res = sw_state_access[unit].dpp.soc.arad.pp.eg_mirror.eg_mirror_port_vlan.set(unit, core, pp_port_ndx, 0);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_eg_mirror_initialize()",0,0);
}


uint32
  arad_pp_sw_db_eg_encap_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_MULTI_SET_PTR
    multi_set_info;
  SOC_SAND_MULTI_SET_INIT_INFO
    multi_set_init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
  multi_set_init_info.get_entry_fun    = NULL;
  multi_set_init_info.set_entry_fun    = NULL;
  multi_set_init_info.max_duplications = ARAD_PP_EG_ENCAP_EEDB_SIZE(unit);
  multi_set_init_info.member_size      = sizeof(uint32) * ARAD_PP_EG_ENCAP_PROG_DATA_ENTRY_SIZE;

  res = arad_sw_db_eg_encap_prge_tbl_nof_dynamic_entries_get(unit,&(multi_set_init_info.nof_members));
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  multi_set_init_info.sec_handle       = 0;

  multi_set_init_info.prime_handle     = unit;

  res = soc_sand_multi_set_create(
    unit,
    &multi_set_info,
    multi_set_init_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.eg_encap.alloc(unit);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = sw_state_access[unit].dpp.soc.arad.pp.eg_encap.prog_data.set(unit, multi_set_info);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_eg_encap_initialize()",0,0);
}

uint8
  arad_pp_ipv4_lpm_mngr_pat_tree_node_data_is_identical_fun(
  SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_0,
  SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_1
  )
{
  if( (ARAD_PP_IPV4_LPM_MGR_PAT_DATA_TO_HW_TARGET_TYPE(node_info_0->data) != SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM) ||
      (ARAD_PP_IPV4_LPM_MGR_PAT_DATA_TO_HW_TARGET_TYPE(node_info_1->data) != SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM))
  {
    return TRUE;
  }

  if( (ARAD_PP_IPV4_LPM_MGR_PAT_DATA_TO_PENDING_OP_TYPE(node_info_0->data) == SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE) ||
    (ARAD_PP_IPV4_LPM_MGR_PAT_DATA_TO_PENDING_OP_TYPE(node_info_1->data) == SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE))
  {
    
    
    return TRUE;
  }

  if( ARAD_PP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(node_info_0->data) == ARAD_PP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(node_info_1->data))
  {
    return (uint8)(node_info_1->data == node_info_0->data && node_info_0->key == node_info_1->key && node_info_0->prefix == node_info_1->prefix);
  }

  return FALSE;
}

uint8
  arad_pp_ipv4_lpm_mngr_pat_node_is_skip_in_lpm_identical_data_query_fun(
    SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info
  )
{
  
  
  if( node_info->is_prefix &&
      ((ARAD_PP_IPV4_LPM_MGR_PAT_DATA_TO_HW_TARGET_TYPE(node_info->data) != SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM) ||
       (ARAD_PP_IPV4_LPM_MGR_PAT_DATA_TO_PENDING_OP_TYPE(node_info->data) == SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE)))
  {
    return TRUE;
  }

  return FALSE;
}





uint32
  arad_pp_sw_db_llp_cos_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;
  SOC_SAND_MULTI_SET_PTR
    multi_set_info;
  SOC_SAND_MULTI_SET_INIT_INFO
    multi_set_init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  

  

  soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
  multi_set_init_info.get_entry_fun    = NULL;
  multi_set_init_info.set_entry_fun    = NULL;
  multi_set_init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  multi_set_init_info.member_size      = sizeof(uint32);
  multi_set_init_info.nof_members      = ARAD_PP_LLP_TRAP_NOF_UD_ETHER_TYPE;
  multi_set_init_info.sec_handle       = 0;

  multi_set_init_info.prime_handle     = unit;

  res = soc_sand_multi_set_create(
    unit,
    &multi_set_info,
    multi_set_init_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  res = sw_state_access[unit].dpp.soc.arad.pp.llp_cos.alloc(unit);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = sw_state_access[unit].dpp.soc.arad.pp.llp_cos.ether_type_multi_set.set(unit, multi_set_info);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_llp_cos_initialize()",0,0);
}





uint32
  arad_pp_sw_db_eth_policer_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;
  SOC_SAND_MULTI_SET_PTR
    multi_set_info[2];
  SOC_SAND_MULTI_SET_INIT_INFO
    multi_set_init_info;
  int
	core_index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  res = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.alloc(unit, SOC_DPP_CONFIG(unit)->meter.nof_meter_cores);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = sw_state_access[unit].dpp.bcm.policer.alloc(unit, SOC_DPP_CONFIG(unit)->meter.nof_meter_cores);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
  multi_set_init_info.get_entry_fun    = NULL;
  multi_set_init_info.set_entry_fun    = NULL;
  multi_set_init_info.max_duplications = 0x600;
  multi_set_init_info.member_size      = sizeof(uint32);
  multi_set_init_info.nof_members      = ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_PROFILE_METER_PROFILE_NOF_MEMBER;
  multi_set_init_info.sec_handle       = 0;

  multi_set_init_info.prime_handle     = unit;


  ARAD_PP_MTR_CORES_ITER(core_index){
	  res = soc_sand_multi_set_create(
		unit,
		&multi_set_info[core_index],
		multi_set_init_info
		);
	  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
	  res = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.eth_meter_profile_multi_set.set(unit, core_index, multi_set_info[core_index]);
	  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }

  soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
  multi_set_init_info.get_entry_fun    = NULL;
  multi_set_init_info.set_entry_fun    = NULL;
  multi_set_init_info.max_duplications = 0x600;
  multi_set_init_info.member_size      = sizeof(uint32);
  multi_set_init_info.nof_members      = ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE_NOF_MEMBER;
  multi_set_init_info.sec_handle       = 0;

  multi_set_init_info.prime_handle     = unit;

  ARAD_PP_MTR_CORES_ITER(core_index){
	  res = soc_sand_multi_set_create(
	    unit,
	    &multi_set_info[core_index],
		multi_set_init_info
		);
	  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
	  res = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.global_meter_profile_multi_set.set(unit, core_index, multi_set_info[core_index]);
	  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }

#if defined(BCM_88675_A0)
  
  if (SOC_IS_JERICHO(unit)) {
	   res = sw_state_access[unit].dpp.soc.arad.pp.policer_size_profile.alloc(unit, SOC_DPP_CONFIG(unit)->meter.nof_meter_cores);
	   SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

	   soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
	   multi_set_init_info.get_entry_fun    = NULL;
	   multi_set_init_info.set_entry_fun    = NULL;
	   multi_set_init_info.max_duplications = 0x600;
	   multi_set_init_info.member_size      = sizeof(uint32);
	   multi_set_init_info.nof_members      = JER_PP_SW_DB_MULTI_SET_PACKET_SIZE_PROFILE_NOF_MEMBER;
	   multi_set_init_info.sec_handle       = 0;
	   multi_set_init_info.prime_handle     = unit;

	   ARAD_PP_MTR_CORES_ITER(core_index){
		   res = soc_sand_multi_set_create(
			   unit,
			   &multi_set_info[core_index],
			   multi_set_init_info
			   );
		   SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

		   res = sw_state_access[unit].dpp.soc.arad.pp.policer_size_profile.policer_size_profile_multi_set.set(unit, core_index, multi_set_info[core_index]);
		   SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
	   }
  }
#endif 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_eth_policer_initialize()",0,0);
}

int
  arad_pp_sw_db_eth_policer_config_status_bit_set(
    SOC_SAND_IN  int                   	unit,
    SOC_SAND_IN  uint32                	core_id, 
    SOC_SAND_IN  uint32                 bit_offset,
    SOC_SAND_IN  uint8                  status
  )
{
    soc_error_t rv = SOC_E_PARAM;

    SOCDNX_INIT_FUNC_DEFS;

	if(status) {
		rv = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.config_meter_status.bit_set(unit, core_id, bit_offset);
	} else {
		rv = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.config_meter_status.bit_clear(unit, core_id, bit_offset);
	}

    SOCDNX_SAND_IF_ERR_EXIT(rv);
  
exit:
    SOCDNX_FUNC_RETURN;
}







uint32
  arad_pp_sw_db_lif_ac_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;
  SOC_SAND_MULTI_SET_PTR
    multi_set_info;
  SOC_SAND_MULTI_SET_INIT_INFO
    multi_set_init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
  multi_set_init_info.get_entry_fun    = NULL;
  multi_set_init_info.set_entry_fun    = NULL;
  multi_set_init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  multi_set_init_info.member_size      = sizeof(uint32);
  multi_set_init_info.nof_members      = ARAD_PP_SW_DB_MULTI_SET_LIF_AC_PROFILE_NOF_MEMBERS(unit);
  multi_set_init_info.sec_handle       = 0;

  multi_set_init_info.prime_handle     = unit;

  res = soc_sand_multi_set_create(
    unit,
    &multi_set_info,
    multi_set_init_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.l2_lif_ac.alloc(unit);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.l2_lif_ac.ac_key_map_multi_set.set(unit, multi_set_info);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_lif_ac_initialize()",0,0);
}



uint32
  arad_pp_sw_db_l2_lif_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;
  SOC_SAND_MULTI_SET_PTR
    multi_set_info;
  SOC_SAND_MULTI_SET_INIT_INFO
    multi_set_init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;



  

  soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
  multi_set_init_info.get_entry_fun    = NULL;
  multi_set_init_info.set_entry_fun    = NULL;
  multi_set_init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  multi_set_init_info.member_size      = sizeof(uint32);
  multi_set_init_info.nof_members      = ARAD_PP_SW_DB_MULTI_SET_VLAN_COMPRESSION_RANGE_NOF_MEMBER;
  multi_set_init_info.sec_handle       = 0;

  multi_set_init_info.prime_handle     = unit;

  res = soc_sand_multi_set_create(
    unit,
    &multi_set_info,
    multi_set_init_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.l2_lif.alloc(unit);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.l2_lif.vlan_compression_range_multi_set.set(unit, multi_set_info);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_l2_lif()",0,0);
}

uint32
  arad_pp_sw_db_lif_table_entry_use_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID           lif_id,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_TYPE   entry_type,
    SOC_SAND_IN    uint32                 sub_type
  )
{
  uint32
    type_val;
  soc_error_t
    rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Arad_pp_sw_db.device[unit] == NULL)
  {
    return SOC_SAND_ERR;
  }
  if (lif_id > SOC_DPP_DEFS_GET(unit, nof_local_lifs) - 1 || entry_type >= SOC_SAND_BIT(SOC_PPC_NOF_LIF_ENTRY_TYPES))
  {
    return SOC_SAND_ERR;
  }

  if(entry_type == SOC_PPC_LIF_ENTRY_TYPE_EMPTY)
  {
    type_val = 0;

  }
  else
  {
    type_val = soc_sand_log2_round_down(entry_type);
  }
    
  
  rv = sw_state_access[unit].dpp.soc.arad.pp.lif_table.lif_use.bit_range_write(unit,
                    lif_id * ARAD_PP_SW_DB_TYPE_BITS,
                    0,
                    ARAD_PP_SW_DB_TYPE_BITS,
                    &type_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  type_val = sub_type;

  rv = sw_state_access[unit].dpp.soc.arad.pp.lif_table.lif_sub_use.bit_range_write(unit,
                    lif_id * ARAD_PP_SW_DB_TYPE_BITS,
                    0,
                    ARAD_PP_SW_DB_TYPE_BITS,
                    &type_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_lif_table_entry_use_set()",0,0);
}


uint32
  arad_pp_sw_db_lif_table_entry_use_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID             lif_id,
    SOC_SAND_OUT  SOC_PPC_LIF_ENTRY_TYPE   *entry_type,
    SOC_SAND_OUT  uint32                   *sub_type
  )
{
  soc_error_t
    rv;
  uint32
    type_val=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Arad_pp_sw_db.device[unit] == NULL)
  {
    return SOC_SAND_ERR;
  }
  if (lif_id > SOC_DPP_DEFS_GET(unit, nof_local_lifs) - 1)
  {
    return SOC_SAND_ERR;
  }

  
  rv = sw_state_access[unit].dpp.soc.arad.pp.lif_table.lif_use.bit_range_read(unit,
                    lif_id * ARAD_PP_SW_DB_TYPE_BITS,
                    0,
                    ARAD_PP_SW_DB_TYPE_BITS,
                    &type_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  if(type_val == 0)
  {
    *entry_type = SOC_PPC_LIF_ENTRY_TYPE_EMPTY;

  }
  else
  {
    *entry_type =  (SOC_PPC_LIF_ENTRY_TYPE)(1 << type_val);
  }

  rv = sw_state_access[unit].dpp.soc.arad.pp.lif_table.lif_sub_use.bit_range_read(unit,
                    lif_id * ARAD_PP_SW_DB_TYPE_BITS,
                    0,
                    ARAD_PP_SW_DB_TYPE_BITS,
                    &type_val);
  *sub_type = type_val;

  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_lif_table_entry_use_get()",0,0);
}






uint32
  arad_pp_sw_db_mact_flush_db_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 entry_index,
    SOC_SAND_IN uint32 flush_db_data[7]
  )
{
  uint32
    i;
  soc_error_t
      rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  for (i=0; i<7; i++) {
    rv = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.flush_db_data_arr.flush_db_data.set(unit, entry_index, i, flush_db_data[i]);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_mact_flush_db_set()",0,0);
}


uint32
  arad_pp_sw_db_mact_flush_db_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 entry_index,
    SOC_SAND_OUT uint32 flush_db_data[7]
  )
{
  int i;

  soc_error_t
      rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (i=0; i<7; i++) {
    rv = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.flush_db_data_arr.flush_db_data.get(unit, entry_index, i, &flush_db_data[i]);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_mact_flush_db_get()",0,0);
}


uint32
  arad_pp_sw_db_mact_traverse_flush_entry_use_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 entry_offset,
    SOC_SAND_IN uint8  in_use
  )
{
    soc_error_t
        rv;
    uint32 status;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.flush_entry_use.get(unit, &status);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    status |= (in_use & 1) << entry_offset;

    rv = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.flush_entry_use.set(unit, status);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_mact_traverse_flush_entry_use_set()",0,0);
}

uint32
  arad_pp_sw_db_mact_traverse_flush_entry_use_get(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  entry_offset,
    SOC_SAND_OUT uint8   *status
  )
{
    soc_error_t
        rv;
    uint32 status_lcl;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.flush_entry_use.get(unit, &status_lcl);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    *status = (status_lcl >> entry_offset) & 1;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_mact_traverse_flush_entry_use_get()",0,0);
}


uint32
  arad_pp_sw_db_flp_prog_app_to_index_get(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  uint8    app_id,
    SOC_SAND_OUT uint32   *prog_index
  )
{
    uint8 local_prog=0;
    uint8 tmp_prog=0;
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *prog_index = -1;
    for (local_prog = 0; local_prog < SOC_DPP_DEFS_GET(unit, nof_flp_programs); local_prog++) 
    {
        rv = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit,
                                       local_prog,
                                       &tmp_prog);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

        if (tmp_prog == app_id) {
            *prog_index = local_prog;
        }
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_flp_prog_app_to_index_get()",0,0);
}



uint32
  arad_pp_sw_db_vrrp_mac_entry_use_set(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32            reg_ndx,
    SOC_SAND_IN  uint32            bit_ndx,
    SOC_SAND_IN  uint8             entry_in_use
  )
{
    uint16 vrrp_mac_use_bit_map=0;
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    if ((reg_ndx >= ARAD_PP_VRRP_NOF_MAC_ADDRESSES/16) || (bit_ndx > 15)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
    }  
    rv = sw_state_access[unit].dpp.soc.arad.pp.vrrp_info.vrrp_mac_use_bit_map.get(unit, reg_ndx, &vrrp_mac_use_bit_map);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
    SOC_SAND_SET_BIT(vrrp_mac_use_bit_map, entry_in_use, bit_ndx);

    rv = sw_state_access[unit].dpp.soc.arad.pp.vrrp_info.vrrp_mac_use_bit_map.set(unit, reg_ndx, vrrp_mac_use_bit_map);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 30, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_vrrp_mac_entry_use_set()",0,0);
}

uint32
  arad_pp_sw_db_vrrp_mac_entry_use_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32            reg_ndx,
    SOC_SAND_IN  uint32            bit_ndx,
    SOC_SAND_OUT uint8             *entry_in_use
  )
{
    uint16 entry_is_set;
    uint16 vrrp_mac_use_bit_map=0;
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if ((reg_ndx >= ARAD_PP_VRRP_NOF_MAC_ADDRESSES/16) || (bit_ndx > 15)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
    }
    rv = sw_state_access[unit].dpp.soc.arad.pp.vrrp_info.vrrp_mac_use_bit_map.get(unit, reg_ndx, &vrrp_mac_use_bit_map);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
    entry_is_set = SOC_SAND_GET_BIT(vrrp_mac_use_bit_map, bit_ndx);
    *entry_in_use = (uint8)entry_is_set;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_vrrp_mac_entry_use_get()",0,0);
}

uint32
  arad_pp_sw_db_vdc_vsi_mapping_set(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32            vsi,
    SOC_SAND_IN  int               port_class,
    SOC_SAND_IN  uint32            isid
  )
{
    soc_error_t rv;
    uint32 vsi_map_info = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (vsi >= SOC_DPP_DEFS_GET(unit, nof_epni_isid)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
    }
    
    vsi_map_info = ARAD_PP_VDC_MAP_INFO_SET(port_class, isid);
    rv = sw_state_access[unit].dpp.soc.arad.pp.vdc_vsi_map.vdc_vsi_mapping_info.set(unit, vsi, vsi_map_info);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_vdc_vsi_mapping_set()",0,0);
}

uint32
  arad_pp_sw_db_vdc_vsi_mapping_get(
    SOC_SAND_IN   int               unit,
    SOC_SAND_IN   uint32            vsi,
    SOC_SAND_OUT  int               *port_class,
    SOC_SAND_OUT  uint32            *isid
  )
{
    uint32 vsi_map_info = 0;
    soc_error_t rv;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (vsi >= SOC_DPP_DEFS_GET(unit, nof_epni_isid)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
    }

    rv = sw_state_access[unit].dpp.soc.arad.pp.vdc_vsi_map.vdc_vsi_mapping_info.get(unit, vsi, &vsi_map_info);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    *port_class = ARAD_PP_VDC_MAP_PORT_VD_GET(vsi_map_info);
    *isid = ARAD_PP_VDC_MAP_ISID_GET(vsi_map_info);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_vdc_vsi_mapping_get()",0,0);
}




STATIC uint32
  arad_pp_sw_db_rif_to_lif_group_map_member_entry_set(
    SOC_SAND_IN int                              unit,
    SOC_SAND_IN uint32                              unused,
    SOC_SAND_IN uint32                              member_ndx,
    SOC_SAND_IN SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY  *member_entry
  )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MEM_LL + WB_ENGINE_GROUP_MEM_LL_MEMBERS, 
                                  member_entry, 
                                  member_ndx);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_rif_to_lif_group_map_member_entry_set()",0,0);
}

STATIC uint32
  arad_pp_sw_db_rif_to_lif_group_map_member_entry_get(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   uint32                            bank_id,
    SOC_SAND_IN  uint32                             member_ndx,
    SOC_SAND_OUT  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY      *member_entry
  )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_COPY(
             member_entry,
             &(ARAD_PP_SW_DB_ACTIVE_INS(0, Arad_pp_sw_db.device[unit]->rif_to_lif_group_map->group_info.group_members_data.members)[member_ndx]),
             SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY,
             1
            );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_rif_to_lif_group_map_member_entry_get()",0,0);
}

STATIC uint32
  arad_pp_sw_db_rif_to_lif_group_map_group_entry_set(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   uint32                            ignored,
    SOC_SAND_IN  uint32                             group_ndx,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY  *group_entry
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MEM_LL + WB_ENGINE_GROUP_MEM_LL_GROUPS, 
                                  group_entry, 
                                  group_ndx);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_rif_to_lif_group_map_group_entry_set()",0,0);
}

STATIC uint32
  arad_pp_sw_db_rif_to_lif_group_map_group_entry_get(
    SOC_SAND_IN   int                          unit,
    SOC_SAND_IN   uint32                          bank_id,
    SOC_SAND_IN  uint32                             group_ndx,
    SOC_SAND_OUT  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY      *group_entry
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_COPY(group_entry,
             &(ARAD_PP_SW_DB_ACTIVE_INS(0,Arad_pp_sw_db.device[unit]->rif_to_lif_group_map->group_info.group_members_data.groups)[group_ndx]),
             SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY,
             1
            );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_lpm_ll_group_entry_get()",0,0);
}

uint32
  arad_pp_sw_db_rif_to_lif_group_map_initialize(
     SOC_SAND_IN  int unit
  )
{
  uint32 res = SOC_SAND_OK;
  SOC_SAND_GROUP_MEM_LL_INFO *group_info;
  uint32 rif_idx;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_ALLOC_ANY_SIZE(
    Arad_pp_sw_db.device[unit]->rif_to_lif_group_map,
    ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MAP,
    1, "rif_to_lif_group_map");
 
  if(!SOC_WARM_BOOT(unit)) {
      res = sw_state_access[unit].dpp.soc.arad.pp.rif_to_lif_group_map.alloc(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

      res = sw_state_access[unit].dpp.soc.arad.pp.rif_to_lif_group_map.rif_urpf_mode.alloc(unit, SOC_DPP_CONFIG(unit)->l3.nof_rifs);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

      for (rif_idx = 0; rif_idx < SOC_DPP_CONFIG(unit)->l3.nof_rifs; rif_idx++) {
          res = sw_state_access[unit].dpp.soc.arad.pp.rif_to_lif_group_map.rif_urpf_mode.set(unit, rif_idx, SOC_PPC_FRWRD_FEC_RPF_MODE_NONE);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
      }
  }

  group_info = &(Arad_pp_sw_db.device[unit]->rif_to_lif_group_map->group_info);

  soc_sand_SAND_GROUP_MEM_LL_INFO_clear(group_info);
  group_info->auto_remove = TRUE;
  group_info->group_set_fun = arad_pp_sw_db_rif_to_lif_group_map_group_entry_set;
  group_info->group_get_fun = arad_pp_sw_db_rif_to_lif_group_map_group_entry_get;
  group_info->member_set_fun = arad_pp_sw_db_rif_to_lif_group_map_member_entry_set;
  group_info->member_get_fun = arad_pp_sw_db_rif_to_lif_group_map_member_entry_get;
  group_info->nof_elements = SOC_DPP_CONFIG(unit)->l2.nof_lifs;

  group_info->nof_groups = SOC_DPP_CONFIG(unit)->l3.nof_rifs;
  group_info->instance_prim_handle = unit;
  group_info->instance_sec_handle = 0; 
  group_info->support_caching = 0;

  res = soc_sand_group_mem_ll_create(group_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_RIF_TO_LIF_GROUP_MAP);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_rif_to_lif_group_map_initialize()",0,0);
}

uint32
  arad_pp_sw_db_rif_to_lif_group_map_terminate(
     SOC_SAND_IN  int unit
  )
{
  uint32 res = SOC_SAND_OK;
  SOC_SAND_GROUP_MEM_LL_INFO *group_info = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  ARAD_PP_SW_DB_NULL_CHECK(unit, rif_to_lif_group_map);
  
  group_info = &(Arad_pp_sw_db.device[unit]->rif_to_lif_group_map->group_info);  

  res = soc_sand_group_mem_ll_destroy(group_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ARAD_FREE_ANY_SIZE(Arad_pp_sw_db.device[unit]->rif_to_lif_group_map);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_rif_to_lif_group_map_terminate()",0,0);
}

uint32 
  arad_pp_sw_db_rif_to_lif_group_map_add_lif_to_rif(
     SOC_SAND_IN int            unit,
     SOC_SAND_IN uint32            rif,
     SOC_SAND_IN bcm_port_t        lif
     )
{
  uint32 res = SOC_SAND_OK;
  SOC_SAND_GROUP_MEM_LL_INFO *group_info = &(Arad_pp_sw_db.device[unit]->rif_to_lif_group_map->group_info);

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_sand_group_mem_ll_member_add(group_info, rif, lif);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_rif_to_lif_group_map_add_lif_to_rif()",0,0);
}

uint32 
  arad_pp_sw_db_rif_to_lif_group_map_remove_lif_from_rif(
     SOC_SAND_IN int            unit,
     SOC_SAND_IN bcm_port_t        lif
     )
{
  uint32 res = SOC_SAND_OK;
  SOC_SAND_GROUP_MEM_LL_INFO *group_info = &(Arad_pp_sw_db.device[unit]->rif_to_lif_group_map->group_info);
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_sand_group_mem_ll_member_remove(group_info, lif);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_rif_to_lif_group_map_remove_lif_from_rif()",0,0);
}

uint32
  arad_pp_sw_db_rif_to_lif_group_map_visit_lif_group(
     SOC_SAND_IN int unit,
     SOC_SAND_IN uint32 rif,
     SOC_SAND_IN ARAD_PP_SW_DB_RIF_TO_LIF_GROUP_MAP_LIF_VISITOR visitor,
     SOC_SAND_INOUT void *opaque
     )
{
  uint32 res = SOC_SAND_OK;
  SOC_SAND_GROUP_MEM_LL_INFO *group_info = &(Arad_pp_sw_db.device[unit]->rif_to_lif_group_map->group_info);

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_sand_group_mem_ll_func_run_pointer_param(group_info, rif, visitor, opaque);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_rif_to_lif_group_map_visit_lif_group()",0,0);

}

uint32
  arad_pp_sw_db_llp_mirror_port_vlan_is_exist_set(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  int          core_id,
    SOC_SAND_IN  SOC_PPC_PORT pp_port_ndx,
    SOC_SAND_IN  uint8        internal_vid_ndx,
    SOC_SAND_IN  uint8        is_exist
  )
{
    uint8 llp_mirror_port_vlan;
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = sw_state_access[unit].dpp.soc.arad.pp.llp_mirror.llp_mirror_port_vlan.get(unit,
                                   core_id,
                                   pp_port_ndx,
                                   &llp_mirror_port_vlan);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 5, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(pp_port_ndx, (ARAD_PP_NOF_PORTS-1), SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

    if (is_exist) {
        llp_mirror_port_vlan |=  (((uint8)1) << internal_vid_ndx);
    } else {
        llp_mirror_port_vlan &= ~(((uint8)1) << internal_vid_ndx);
    }
    rv = sw_state_access[unit].dpp.soc.arad.pp.llp_mirror.llp_mirror_port_vlan.set(unit,
                                   core_id,
                                   pp_port_ndx,
                                   llp_mirror_port_vlan);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_llp_mirror_port_vlan_is_exist_set()",0,0);  
}

uint32
  arad_pp_sw_db_llp_mirror_port_vlan_is_exist_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  int          core_id,
    SOC_SAND_IN  SOC_PPC_PORT pp_port_ndx,
    SOC_SAND_IN  uint8        internal_vid_ndx,
    SOC_SAND_OUT uint8        *is_exist
    )
{
    uint8 llp_mirror_port_vlan=0;
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = sw_state_access[unit].dpp.soc.arad.pp.llp_mirror.llp_mirror_port_vlan.get(unit,
                                   core_id,
                                   pp_port_ndx,
                                   &llp_mirror_port_vlan);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 5, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(pp_port_ndx, (ARAD_PP_NOF_PORTS-1), SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
    *is_exist = SOC_SAND_NUM2BOOL((llp_mirror_port_vlan >> internal_vid_ndx) & 1);  
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_llp_mirror_port_vlan_is_exist_get()",0,0);
}

uint32
  arad_pp_sw_db_eg_mirror_port_vlan_is_exist_set(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  int          core_id,
    SOC_SAND_IN  SOC_PPC_PORT pp_port_ndx,
    SOC_SAND_IN  uint8        internal_vid_ndx,
    SOC_SAND_IN  uint8        is_exist
  )
{
    uint8 eg_mirror_port_vlan=0;
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_ERR_IF_ABOVE_MAX(pp_port_ndx, (ARAD_PP_NOF_PORTS-1), SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);


    rv = sw_state_access[unit].dpp.soc.arad.pp.eg_mirror.eg_mirror_port_vlan.get(unit,
                                   core_id,
                                   pp_port_ndx,
                                   &eg_mirror_port_vlan);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
    if (is_exist) {
        eg_mirror_port_vlan |=  (((uint8)1) << internal_vid_ndx);
    } else {
        eg_mirror_port_vlan &= ~(((uint8)1) << internal_vid_ndx);
    }
   
    rv = sw_state_access[unit].dpp.soc.arad.pp.eg_mirror.eg_mirror_port_vlan.set(unit,
                                   core_id,
                                   pp_port_ndx,
                                   eg_mirror_port_vlan);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 30, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_eg_mirror_port_vlan_is_exist_set()",0,0);
}

uint32
  arad_pp_sw_db_eg_mirror_port_vlan_is_exist_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  int          core_id,
    SOC_SAND_IN  SOC_PPC_PORT pp_port_ndx,
    SOC_SAND_IN  uint8        internal_vid_ndx,
    SOC_SAND_OUT uint8        *is_exist
    )
{
    uint8 eg_mirror_port_vlan=0;
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_ERR_IF_ABOVE_MAX(pp_port_ndx, (ARAD_PP_NOF_PORTS-1), SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

    rv = sw_state_access[unit].dpp.soc.arad.pp.eg_mirror.eg_mirror_port_vlan.get(unit,
                                   core_id,
                                   pp_port_ndx,
                                   &eg_mirror_port_vlan);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);

    *is_exist = SOC_SAND_NUM2BOOL((eg_mirror_port_vlan >> internal_vid_ndx) & 1);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_eg_mirror_port_vlan_is_exist_get()",0,0);
}


STATIC uint32
  arad_pp_sw_db_ecmp_info_initialize(
     SOC_SAND_IN  int unit
  )
{
  uint8 is_allocated;
  soc_error_t rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = sw_state_access[unit].dpp.soc.arad.pp.ecmp_info.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

  if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.pp.ecmp_info.alloc(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);
  }

  rv = sw_state_access[unit].dpp.soc.arad.pp.ecmp_info.cur_size.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

  if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.pp.ecmp_info.cur_size.alloc(unit, SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);
  }

  rv = sw_state_access[unit].dpp.soc.arad.pp.ecmp_info.max_size.is_allocated(unit, &is_allocated);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);

  if(!is_allocated) {
      rv = sw_state_access[unit].dpp.soc.arad.pp.ecmp_info.max_size.alloc(unit, SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 32, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_ecmp_info_initialize()",0,0);
}


uint32
  arad_pp_sw_db_overlay_arp_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_MULTI_SET_PTR
    multi_set_info;
  SOC_SAND_MULTI_SET_INIT_INFO
    multi_set_init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
  multi_set_init_info.get_entry_fun    = NULL;
  multi_set_init_info.set_entry_fun    = NULL;
  multi_set_init_info.member_size      = sizeof(uint32) * ARAD_PP_EG_ENCAP_PROG_DATA_ENTRY_SIZE;

  multi_set_init_info.nof_members      = 16;
  multi_set_init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  multi_set_init_info.sec_handle       = 0;


  multi_set_init_info.prime_handle     = unit;

  res = soc_sand_multi_set_create(
    unit,
    &multi_set_info,
    multi_set_init_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.overlay_arp.alloc(unit);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.overlay_arp.prog_data.set(unit, multi_set_info);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_sw_db_overlay_arp_initialize()",0,0);
}


uint32
  arad_pp_sw_db_ecmp_cur_size_set(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       ecmp_fec_idx,
    SOC_SAND_IN  uint32       cur_size
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = sw_state_access[unit].dpp.soc.arad.pp.ecmp_info.cur_size.set(unit, ecmp_fec_idx, cur_size);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_ecmp_cur_size_set()",0,0);
}

uint32 
  arad_pp_sw_db_ecmp_cur_size_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       ecmp_fec_idx,
    SOC_SAND_OUT uint32       *size
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = sw_state_access[unit].dpp.soc.arad.pp.ecmp_info.cur_size.get(unit, ecmp_fec_idx, size);

    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_ecmp_cur_size_get()",0,0);
}

uint32
  arad_pp_sw_db_ecmp_max_size_set(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       ecmp_fec_idx,
    SOC_SAND_IN  uint32       max_size
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    rv = sw_state_access[unit].dpp.soc.arad.pp.ecmp_info.max_size.set(unit, ecmp_fec_idx, max_size);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_ecmp_max_size_set()",0,0);
}

uint32
  arad_pp_sw_db_ecmp_max_size_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       ecmp_fec_idx,
    SOC_SAND_OUT uint32       *max_size
  )
{
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = sw_state_access[unit].dpp.soc.arad.pp.ecmp_info.max_size.get(unit, ecmp_fec_idx, max_size);

    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sw_db_ecmp_max_size_get()",0,0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_sw_db_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_sw_db;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_sw_db_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_sw_db;
}



int
  arad_pp_sw_db_src_bind_spoof_id_ref_count_set(
    int               unit,
    uint16            spoof_id,
    uint8             increase
  )
{
  uint16 ref_count = 0;
  soc_error_t
        rv;
  SOCDNX_INIT_FUNC_DEFS;

  if (spoof_id >= ARAD_PP_SRC_BIND_IPV4_MAX_SPOOF_ID) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("spoof_id %d is out of limit"), spoof_id));
  }

  rv = sw_state_access[unit].dpp.soc.arad.pp.arad_sw_db_src_binds.spoof_id_ref_count.get(unit, spoof_id, &ref_count);
  SOCDNX_IF_ERR_EXIT(rv);

  if (increase) {
    ref_count++;
  } else {
    ref_count--;
  }

  rv = sw_state_access[unit].dpp.soc.arad.pp.arad_sw_db_src_binds.spoof_id_ref_count.set(unit,
                                   spoof_id,
                                   ref_count);
  SOCDNX_IF_ERR_EXIT(rv);
exit:
  SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 


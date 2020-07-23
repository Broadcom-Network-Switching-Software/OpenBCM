/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COMMON





#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_group_member_list.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_wb_engine.h>










#define SOC_SAND_GROUP_MEM_LL_CACHE_INST(_inst) (SOC_SAND_BIT(31)|(_inst))

#define SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info, _inst) \
  (gr_mem_info->group_members_data.cache_enabled)?SOC_SAND_GROUP_MEM_LL_CACHE_INST(_inst):_inst

















uint32
  soc_sand_group_mem_ll_create(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO      *gr_mem_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_GROUP_MEM_LL_CREATE);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info->group_get_fun);
  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info->group_set_fun);
  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info->member_get_fun);
  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info->member_set_fun);

  if (gr_mem_info->nof_groups == 0 || gr_mem_info->nof_groups == SOC_SAND_GROUP_MEM_LL_END )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GROUP_MEM_LL_NOF_GROUPS_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (gr_mem_info->nof_elements == 0 || gr_mem_info->nof_elements == SOC_SAND_GROUP_MEM_LL_END )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GROUP_MEM_LL_NOF_MEMBERS_OUT_OF_RANGE_ERR, 20, exit);
  }
 
  gr_mem_info->group_members_data.groups = 
       (SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY*) soc_sand_os_malloc_any_size(sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY) * gr_mem_info->nof_groups, "gr_mem_info->group_members_data.groups");
  if (!gr_mem_info->group_members_data.groups)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 30, exit);
  }
  gr_mem_info->group_members_data.members = 
       (SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY*) soc_sand_os_malloc_any_size(sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY) * gr_mem_info->nof_elements, "gr_mem_info->group_members_data.members");
  if (!gr_mem_info->group_members_data.members)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 40, exit);
  }
  
  res = soc_sand_os_memset(
          gr_mem_info->group_members_data.groups,
          0xFF,
          sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY) * gr_mem_info->nof_groups
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_sand_os_memset(
          gr_mem_info->group_members_data.members,
          0xFF,
          sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY) * gr_mem_info->nof_elements
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  if (gr_mem_info->support_caching)
  {
    gr_mem_info->group_members_data.groups_cache = 
          (SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY*) soc_sand_os_malloc_any_size(sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY) * gr_mem_info->nof_groups, "gr_mem_info->group_members_data.groups_cache");
    if (!gr_mem_info->group_members_data.groups_cache)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 130, exit);
    }
    gr_mem_info->group_members_data.members_cache = 
          (SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY*) soc_sand_os_malloc_any_size(sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY) * gr_mem_info->nof_elements, "gr_mem_info->group_members_data.members_cache");
    if (!gr_mem_info->group_members_data.members_cache)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 140, exit);
    }
    
    res = soc_sand_os_memset(
            gr_mem_info->group_members_data.groups_cache,
            0xFF,
            sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY) * gr_mem_info->nof_groups
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

    res = soc_sand_os_memset(
            gr_mem_info->group_members_data.members_cache,
            0xFF,
            sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY) * gr_mem_info->nof_elements
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_create()",0,0);
}



uint32
  soc_sand_group_mem_ll_destroy(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO      *gr_mem_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_GROUP_MEM_LL_DESTROY);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);

  

  res = soc_sand_os_free_any_size(
          gr_mem_info->group_members_data.groups
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_os_free_any_size(
          gr_mem_info->group_members_data.members
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (gr_mem_info->support_caching)
  {
    res = soc_sand_os_free_any_size(
            gr_mem_info->group_members_data.groups_cache
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

    res = soc_sand_os_free_any_size(
            gr_mem_info->group_members_data.members_cache
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_destroy()",0,0);
}



uint32
  soc_sand_group_mem_ll_clear(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO      *gr_mem_info
  )
{
  uint32
      res;
  int unit = BSL_UNIT_UNKNOWN;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_GROUP_MEM_LL_CLEAR);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);


  unit = (gr_mem_info->instance_prim_handle);

  
  if(gr_mem_info->wb_var_index == SOC_DPP_WB_ENGINE_VAR_NONE) {
      res = soc_sand_os_memset(
                               gr_mem_info->group_members_data.groups,
                               0xFF,
                               sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY) * gr_mem_info->nof_groups
                               );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      res = soc_sand_os_memset(
                               gr_mem_info->group_members_data.members,
                               0xFF,
                               sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY) * gr_mem_info->nof_elements
                               );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  } else {

      res = SOC_DPP_WB_ENGINE_MEMSET_ARR(unit, gr_mem_info->wb_var_index + WB_ENGINE_GROUP_MEM_LL_GROUPS, 0xFF);
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      res = SOC_DPP_WB_ENGINE_MEMSET_ARR(unit, gr_mem_info->wb_var_index + WB_ENGINE_GROUP_MEM_LL_MEMBERS, 0xFF);
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }


  if (gr_mem_info->support_caching)
  {
      if(gr_mem_info->wb_var_index == SOC_DPP_WB_ENGINE_VAR_NONE) {

          res = soc_sand_os_memset(
                                   gr_mem_info->group_members_data.groups_cache,
                                   0xFF,
                                   sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY) * gr_mem_info->nof_groups
                                   );
          SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

          res = soc_sand_os_memset(
                                   gr_mem_info->group_members_data.members_cache,
                                   0xFF,
                                   sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY) * gr_mem_info->nof_elements
                                   );
          SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

      } else {

          res = SOC_DPP_WB_ENGINE_MEMSET_ARR(unit, gr_mem_info->wb_var_index + WB_ENGINE_GROUP_MEM_LL_GROUPS_CACHE, 0xFF);
          SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

          res = SOC_DPP_WB_ENGINE_MEMSET_ARR(unit, gr_mem_info->wb_var_index + WB_ENGINE_GROUP_MEM_LL_MEMBERS_CACHE, 0xFF);
          SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

      }

  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_create()",0,0);
}


uint32
  soc_sand_group_mem_ll_cache_set(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO      *gr_mem_info,
    SOC_SAND_IN uint8                             enable
  )
{
  int unit = BSL_UNIT_UNKNOWN, res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_GROUP_MEM_LL_CLEAR);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);


  unit = (gr_mem_info->instance_prim_handle);

  if (!gr_mem_info->support_caching && enable)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }

  
  if (gr_mem_info->group_members_data.cache_enabled && enable)
  {
    goto exit;
  }
  
  if(gr_mem_info->wb_var_index == SOC_DPP_WB_ENGINE_VAR_NONE) {
      gr_mem_info->group_members_data.cache_enabled = enable;
  } else {
      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, gr_mem_info->wb_var_index + WB_ENGINE_GROUP_MEM_LL_CACHE_ENABLED, 
                                      &(enable));
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }
 
  if (enable)
  {
      if(gr_mem_info->wb_var_index == SOC_DPP_WB_ENGINE_VAR_NONE) {

          soc_sand_os_memcpy(gr_mem_info->group_members_data.groups_cache,gr_mem_info->group_members_data.groups,sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY) * gr_mem_info->nof_groups);
          soc_sand_os_memcpy(gr_mem_info->group_members_data.members_cache,gr_mem_info->group_members_data.members, sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY) * gr_mem_info->nof_elements);

      } else {

          res = SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit, gr_mem_info->wb_var_index + WB_ENGINE_GROUP_MEM_LL_GROUPS_CACHE, 
                                             gr_mem_info->group_members_data.groups,
                                             0,
                                             gr_mem_info->nof_groups);
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

          res = SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit, gr_mem_info->wb_var_index + WB_ENGINE_GROUP_MEM_LL_MEMBERS_CACHE, 
                                             gr_mem_info->group_members_data.members,
                                             0,
                                             gr_mem_info->nof_elements);
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_cache_set()",0,0);
}

uint32
  soc_sand_group_mem_ll_commit(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO      *gr_mem_info,
    SOC_SAND_IN uint32 flags
  )
{
  int unit = BSL_UNIT_UNKNOWN, res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_GROUP_MEM_LL_CLEAR);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);


  unit = (gr_mem_info->instance_prim_handle);

  if (!gr_mem_info->support_caching)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }
  
  if (!gr_mem_info->group_members_data.cache_enabled)
  {
    goto exit;
  }
 
  if(gr_mem_info->wb_var_index == SOC_DPP_WB_ENGINE_VAR_NONE) {

      soc_sand_os_memcpy(gr_mem_info->group_members_data.groups,gr_mem_info->group_members_data.groups_cache,sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY) * gr_mem_info->nof_groups);
      soc_sand_os_memcpy(gr_mem_info->group_members_data.members,gr_mem_info->group_members_data.members_cache, sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY) * gr_mem_info->nof_elements);

  } else {

      res = SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit, gr_mem_info->wb_var_index + WB_ENGINE_GROUP_MEM_LL_GROUPS, 
                                         gr_mem_info->group_members_data.groups_cache,
                                         0,
                                         gr_mem_info->nof_groups);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      res = SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit, gr_mem_info->wb_var_index + WB_ENGINE_GROUP_MEM_LL_MEMBERS, 
                                         gr_mem_info->group_members_data.members_cache,
                                         0,
                                         gr_mem_info->nof_elements);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_commit()",0,0);
}


uint32
  soc_sand_group_mem_ll_rollback(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO      *gr_mem_info,
    SOC_SAND_IN uint32 flags
  )
{
  int unit = BSL_UNIT_UNKNOWN, res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_GROUP_MEM_LL_CLEAR);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);


  unit = (gr_mem_info->instance_prim_handle);

  if (!gr_mem_info->support_caching)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }

  
  if (!gr_mem_info->group_members_data.cache_enabled)
  {
    goto exit;
  }
  
 
  if(gr_mem_info->wb_var_index == SOC_DPP_WB_ENGINE_VAR_NONE) {

      soc_sand_os_memcpy(gr_mem_info->group_members_data.groups_cache,gr_mem_info->group_members_data.groups,sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY) * gr_mem_info->nof_groups);
      soc_sand_os_memcpy(gr_mem_info->group_members_data.members_cache,gr_mem_info->group_members_data.members, sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY) * gr_mem_info->nof_elements);

  } else {

      res = SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit, gr_mem_info->wb_var_index + WB_ENGINE_GROUP_MEM_LL_GROUPS_CACHE, 
                                         gr_mem_info->group_members_data.groups,
                                         0, 
                                         gr_mem_info->nof_groups);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      res = SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit, gr_mem_info->wb_var_index + WB_ENGINE_GROUP_MEM_LL_MEMBERS_CACHE, 
                                         gr_mem_info->group_members_data.members, 
                                         0,
                                         gr_mem_info->nof_elements);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_rollback()",0,0);
}


uint32
  soc_sand_group_mem_ll_members_set(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO     *gr_mem_info,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ID     group_ndx,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_MEMBER_ID    *members,
    SOC_SAND_IN  uint32                       nof_members
  )
{
  uint32
    indx;
  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY
    grp_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY
    member_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ID
    prev,
    curr,
    next;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_GROUP_MEM_LL_MEMBERS_SET);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(members);

  if (group_ndx > gr_mem_info->nof_groups)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GROUP_MEM_LL_GROUP_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

  for (indx = 0; indx < nof_members; ++indx)
  {
    if (members[indx] > gr_mem_info->nof_elements)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GROUP_MEM_LL_MEMBER_ID_OUT_OF_RANGE_ERR, 20, exit);
    }
   
    res = gr_mem_info->member_get_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            members[indx],
            &member_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
   
    if (member_entry.group_id == group_ndx)
    {
      continue;
    }
   
    if (member_entry.group_id != SOC_SAND_GROUP_MEM_LL_END)
    {
     
      if (!gr_mem_info->auto_remove)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GROUP_MEM_LL_ILLEGAL_ADD_ERR, 40, exit);
      }
      else
      {
       
        res = soc_sand_group_mem_ll_member_remove(
                gr_mem_info,
                members[indx]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      }
    }
  }
  grp_entry.first_member = members[0];
  res = gr_mem_info->group_set_fun(
          gr_mem_info->instance_prim_handle,
          SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
          group_ndx,
          &grp_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  prev = SOC_SAND_GROUP_MEM_LL_END;
  member_entry.group_id = group_ndx;
  for (indx = 0; indx < nof_members; ++indx)
  {
    curr = members[indx];
    next = (indx + 1 < nof_members)? members[indx+1] : SOC_SAND_GROUP_MEM_LL_END;
    member_entry.next_member = next;
    member_entry.prev_member = prev;
    res = gr_mem_info->member_set_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            curr,
            &member_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    prev = curr;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_members_set()",0,0);
}



uint32
  soc_sand_group_mem_ll_member_add(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO     *gr_mem_info,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ID     group_ndx,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_MEMBER_ID    member_id
  )
{
  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY
    grp_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY
    member_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ID
    next;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_GROUP_MEM_LL_MEMBER_ADD);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);

  if (group_ndx > gr_mem_info->nof_groups)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GROUP_MEM_LL_GROUP_ID_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (member_id > gr_mem_info->nof_elements)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GROUP_MEM_LL_MEMBER_ID_OUT_OF_RANGE_ERR, 20, exit);
  }
 
  res = gr_mem_info->member_get_fun(
          gr_mem_info->instance_prim_handle,
          SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
          member_id,
          &member_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
 
  if (member_entry.group_id == group_ndx)
  {
    goto exit;
  }
 
  if (member_entry.group_id != SOC_SAND_GROUP_MEM_LL_END)
  {
   
    if (!gr_mem_info->auto_remove)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GROUP_MEM_LL_ILLEGAL_ADD_ERR, 40, exit);
    }
    else
    {
     
      res = soc_sand_group_mem_ll_member_remove(
              gr_mem_info,
              member_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }

  res = gr_mem_info->group_get_fun(
          gr_mem_info->instance_prim_handle,
          SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
          group_ndx,
          &grp_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

 
  next = grp_entry.first_member;
  grp_entry.first_member = member_id;
  res = gr_mem_info->group_set_fun(
          gr_mem_info->instance_prim_handle,
          SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
          group_ndx,
          &grp_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

 
  member_entry.prev_member = SOC_SAND_GROUP_MEM_LL_END;
  member_entry.next_member = next;
  member_entry.group_id = group_ndx;
  res = gr_mem_info->member_set_fun(
          gr_mem_info->instance_prim_handle,
          SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
          member_id,
          &member_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
 
  if (next != SOC_SAND_GROUP_MEM_LL_END)
  {
    res = gr_mem_info->member_get_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            next,
            &member_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    member_entry.prev_member = member_id;

    res = gr_mem_info->member_set_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            next,
            &member_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_member_add()",0,0);
}



uint32
  soc_sand_group_mem_ll_member_remove(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO     *gr_mem_info,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_MEMBER_ID    member_id
  )
{
  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY
    grp_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY
    member_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ID
    prev,
    next;
  SOC_SAND_GROUP_MEM_LL_GROUP_ID
    group_id;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_GROUP_MEM_LL_MEMBER_REMOVE);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);

  if (member_id > gr_mem_info->nof_elements)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GROUP_MEM_LL_MEMBER_ID_OUT_OF_RANGE_ERR, 10, exit);
  }
 
  res = gr_mem_info->member_get_fun(
          gr_mem_info->instance_prim_handle,
          SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
          member_id,
          &member_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  prev = member_entry.prev_member;
  next = member_entry.next_member;
  group_id = member_entry.group_id;
 
  if (group_id == SOC_SAND_GROUP_MEM_LL_END)
  {
    goto exit;
  }
 
  member_entry.group_id = SOC_SAND_GROUP_MEM_LL_END;
  member_entry.next_member = SOC_SAND_GROUP_MEM_LL_END;
  member_entry.prev_member = SOC_SAND_GROUP_MEM_LL_END;

  res = gr_mem_info->member_set_fun(
          gr_mem_info->instance_prim_handle,
          SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
          member_id,
          &member_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


 
  if (prev == SOC_SAND_GROUP_MEM_LL_END)
  {
    res = gr_mem_info->group_get_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            group_id,
            &grp_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    grp_entry.first_member = next;
     res = gr_mem_info->group_set_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            group_id,
            &grp_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
 
  if (prev != SOC_SAND_GROUP_MEM_LL_END)
  {
    res = gr_mem_info->member_get_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            prev,
            &member_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    member_entry.next_member = next;

    res = gr_mem_info->member_set_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            prev,
            &member_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  }
 
  if (next != SOC_SAND_GROUP_MEM_LL_END)
  {
    res = gr_mem_info->member_get_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            next,
            &member_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    member_entry.prev_member = prev;

    res = gr_mem_info->member_set_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            next,
            &member_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_member_remove()",0,0);
}



uint32
  soc_sand_group_mem_ll_members_get(
    SOC_SAND_INOUT  SOC_SAND_GROUP_MEM_LL_INFO        *gr_mem_info,
    SOC_SAND_IN     SOC_SAND_GROUP_MEM_LL_GROUP_ID    group_ndx,
    SOC_SAND_IN     uint32                      max_members,
    SOC_SAND_INOUT  SOC_SAND_GROUP_MEM_LL_MEMBER_ID   *iter,
    SOC_SAND_OUT    SOC_SAND_GROUP_MEM_LL_MEMBER_ID   *members,
    SOC_SAND_OUT    uint32                      *nof_members
  )
{
 uint32
    indx;
  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY
    grp_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY
    member_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ID
    tmp_iter;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_GROUP_MEM_LL_MEMBERS_GET);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);

  if (group_ndx > gr_mem_info->nof_groups)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GROUP_MEM_LL_GROUP_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

  tmp_iter = *iter;
  *nof_members = 0;

  res = gr_mem_info->group_get_fun(
          gr_mem_info->instance_prim_handle,
          SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
          group_ndx,
          &grp_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (tmp_iter == 0)
  {
    tmp_iter = grp_entry.first_member;
  }

  for (indx = 0; indx < max_members && tmp_iter != SOC_SAND_GROUP_MEM_LL_END ; ++indx)
  {
    res = gr_mem_info->member_get_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            tmp_iter,
            &member_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (member_entry.group_id == SOC_SAND_GROUP_MEM_LL_END)
    {
      tmp_iter = SOC_SAND_GROUP_MEM_LL_END;
      break;
    }
    members[indx] = tmp_iter;
    tmp_iter = member_entry.next_member;
    *nof_members += 1;
  }

  *iter = tmp_iter;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_members_get()",0,0);
}



uint32
  soc_sand_group_mem_ll_is_group_empty(
    SOC_SAND_INOUT  SOC_SAND_GROUP_MEM_LL_INFO        *gr_mem_info,
    SOC_SAND_IN     SOC_SAND_GROUP_MEM_LL_GROUP_ID    group_ndx,
    SOC_SAND_OUT    uint8                     *is_empty
  )
{
  SOC_SAND_GROUP_MEM_LL_MEMBER_ID
    iter=0;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ID
    member;
  uint32
    nof_members=0;
  uint32
    res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_GROUP_MEM_LL_IS_GROUP_EMPTY);

  res = soc_sand_group_mem_ll_members_get(
          gr_mem_info,
          group_ndx,
          1,
          &iter,
          &member,
          &nof_members
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *is_empty = (uint8)(nof_members == 0);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_is_group_empty()",0,0);
}

uint32
  soc_sand_group_mem_ll_member_get_group(
    SOC_SAND_INOUT  SOC_SAND_GROUP_MEM_LL_INFO        *gr_mem_info,
    SOC_SAND_IN     SOC_SAND_GROUP_MEM_LL_MEMBER_ID   member_ndx,
    SOC_SAND_OUT    SOC_SAND_GROUP_MEM_LL_GROUP_ID    *group_ndx,
    SOC_SAND_OUT    uint8                     *valid
  )
{
  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY
    member_entry;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_GROUP_MEM_LL_MEMBER_GET_GROUP);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);

  if (member_ndx > gr_mem_info->nof_elements)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GROUP_MEM_LL_MEMBER_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = gr_mem_info->member_get_fun(
          gr_mem_info->instance_prim_handle,
          SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
          member_ndx,
          &member_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  if( member_entry.group_id != SOC_SAND_GROUP_MEM_LL_END)
  {
    *group_ndx = member_entry.group_id;
    *valid = TRUE;
  }
  else
  {
    *valid = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_member_get_group()",0,0);
}




uint32
  soc_sand_group_mem_ll_group_clear(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO     *gr_mem_info,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ID     group_ndx
  )
{
  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY
    grp_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY
    member_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ID
    tmp_iter,
    next;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_GROUP_MEM_LL_MEMBER_CLEAR);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);

  if (group_ndx > gr_mem_info->nof_groups)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GROUP_MEM_LL_GROUP_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = gr_mem_info->group_get_fun(
          gr_mem_info->instance_prim_handle,
          SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
          group_ndx,
          &grp_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tmp_iter = grp_entry.first_member;
  grp_entry.first_member = SOC_SAND_GROUP_MEM_LL_END;

  res = gr_mem_info->group_set_fun(
          gr_mem_info->instance_prim_handle,
          SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
          group_ndx,
          &grp_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  while (tmp_iter != SOC_SAND_GROUP_MEM_LL_END)
  {
    res = gr_mem_info->member_get_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            tmp_iter,
            &member_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    next = member_entry.next_member;
   
    member_entry.group_id = SOC_SAND_GROUP_MEM_LL_END;
    member_entry.next_member = SOC_SAND_GROUP_MEM_LL_END;
    member_entry.prev_member = SOC_SAND_GROUP_MEM_LL_END;

    res = gr_mem_info->member_set_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            tmp_iter,
            &member_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    tmp_iter = next;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_group_clear()",0,0);
}


uint32
  soc_sand_group_mem_ll_get_size_for_save(
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_INFO      *gr_mem_info,
    SOC_SAND_OUT  uint32                   *size
  )
{
  uint32
    total_size=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(size);

  
  total_size += sizeof(SOC_SAND_GROUP_MEM_LL_INFO);

  total_size += gr_mem_info->nof_groups * sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY);
  total_size += gr_mem_info->nof_elements * sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY);
  if (gr_mem_info->group_members_data.cache_enabled)
  {
    total_size += gr_mem_info->nof_groups * sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY);
    total_size += gr_mem_info->nof_elements * sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY);
  }
  
  
  *size = total_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_save()",0,0);
}


uint32
  soc_sand_group_mem_ll_save(
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_INFO     *gr_mem_info,
    SOC_SAND_OUT uint8                    *buffer,
    SOC_SAND_IN  uint32                   buffer_size_bytes,
    SOC_SAND_OUT uint32                   *actual_size_bytes
  )
{
  uint8
    *cur_ptr = (uint8*)buffer;
  uint32
    cur_size,
    total_size=0;
  uint32
    res;
  SOC_SAND_GROUP_MEM_LL_INFO
    *info;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(buffer);
  SOC_SAND_CHECK_NULL_INPUT(actual_size_bytes);

  
  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,gr_mem_info,SOC_SAND_GROUP_MEM_LL_INFO,1);
  
  
  info = (SOC_SAND_GROUP_MEM_LL_INFO *) (cur_ptr - sizeof(SOC_SAND_GROUP_MEM_LL_INFO));
  soc_sand_os_memset(&info->group_members_data.groups, 0x0, sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY *));
  soc_sand_os_memset(&info->group_members_data.members, 0x0, sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY *));
  soc_sand_os_memset(&info->group_members_data.groups_cache, 0x0, sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY *));
  soc_sand_os_memset(&info->group_members_data.members_cache, 0x0, sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY *));  
  soc_sand_os_memset(&info->group_set_fun, 0x0, sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY_SET));
  soc_sand_os_memset(&info->group_get_fun, 0x0, sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY_GET));
  soc_sand_os_memset(&info->member_set_fun, 0x0, sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY_SET));
  soc_sand_os_memset(&info->member_get_fun, 0x0, sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY_GET)); 

  
  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,gr_mem_info->group_members_data.groups,SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY,gr_mem_info->nof_groups);
  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,gr_mem_info->group_members_data.members,SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY,gr_mem_info->nof_elements);
  if (gr_mem_info->group_members_data.cache_enabled)
  {
    SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,gr_mem_info->group_members_data.groups_cache,SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY,gr_mem_info->nof_groups);
    SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,gr_mem_info->group_members_data.members_cache,SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY,gr_mem_info->nof_elements);
  }
  
  *actual_size_bytes = total_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_save()",0,0);
}


uint32
  soc_sand_group_mem_ll_load(
    SOC_SAND_IN  uint8                            **buffer,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY_SET  group_set_fun,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY_GET  group_get_fun,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY_SET member_set_fun,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY_GET member_get_fun,
    SOC_SAND_OUT SOC_SAND_GROUP_MEM_LL_INFO             *gr_mem_info
  )
{
  SOC_SAND_IN uint8
    *cur_ptr = (SOC_SAND_IN uint8*)buffer[0];
  uint32
    cur_size;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(buffer);

  soc_sand_SAND_GROUP_MEM_LL_INFO_clear(gr_mem_info);

  
  soc_sand_os_memcpy(gr_mem_info, cur_ptr,sizeof(SOC_SAND_GROUP_MEM_LL_INFO));
  cur_ptr += sizeof(SOC_SAND_GROUP_MEM_LL_INFO);
  gr_mem_info->group_set_fun = group_set_fun;
  gr_mem_info->group_get_fun = group_get_fun;
  gr_mem_info->member_set_fun = member_set_fun;
  gr_mem_info->member_get_fun = member_get_fun;
  
  res = soc_sand_group_mem_ll_create(gr_mem_info);
  SOC_SAND_CHECK_FUNC_RESULT(res,20, exit);

  
  cur_size = sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY) * gr_mem_info->nof_groups;
  soc_sand_os_memcpy(gr_mem_info->group_members_data.groups,cur_ptr, cur_size);
  cur_ptr += cur_size;

  cur_size = sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY) * gr_mem_info->nof_elements;
  soc_sand_os_memcpy(gr_mem_info->group_members_data.members,cur_ptr,cur_size);
  cur_ptr += cur_size;
  
  
  if (gr_mem_info->group_members_data.cache_enabled)
  {
    cur_size = sizeof(SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY) * gr_mem_info->nof_groups;
    soc_sand_os_memcpy(gr_mem_info->group_members_data.groups_cache,cur_ptr, cur_size);
    cur_ptr += cur_size;

    cur_size = sizeof(SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY) * gr_mem_info->nof_elements;
    soc_sand_os_memcpy(gr_mem_info->group_members_data.members_cache,cur_ptr,cur_size);
    cur_ptr += cur_size;
  }

  *buffer = cur_ptr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_load()",0,0);
}



uint32
  soc_sand_group_mem_ll_group_print(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO     *gr_mem_info,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ID     group_ndx
  )
{
  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY
    grp_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY
    member_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ID
    tmp_iter;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_GROUP_MEM_LL_MEMBER_PRINT);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);

  if (group_ndx > gr_mem_info->nof_groups)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GROUP_MEM_LL_GROUP_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = gr_mem_info->group_get_fun(
          gr_mem_info->instance_prim_handle,
          SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
          group_ndx,
          &grp_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tmp_iter = grp_entry.first_member;
  LOG_CLI((BSL_META("%u: "), group_ndx));

  while (tmp_iter != SOC_SAND_GROUP_MEM_LL_END)
  {
    LOG_CLI((BSL_META("%u, "), tmp_iter));
    res = gr_mem_info->member_get_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            tmp_iter,
            &member_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    if (member_entry.group_id == SOC_SAND_GROUP_MEM_LL_END)
    {
      goto exit;
    }

    tmp_iter = member_entry.next_member;
  }
    LOG_CLI((BSL_META("\n")));
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_group_print()",0,0);
}





uint32
  soc_sand_group_mem_ll_func_run(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO     *gr_mem_info,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ID     group_ndx,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_ITER_FUN     func,
    SOC_SAND_INOUT  uint32                    param1,
    SOC_SAND_INOUT  uint32                    param2
  )
{
  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY
    grp_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY
    member_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ID
    tmp_iter;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_GROUP_MEM_LL_FUNC_RUN);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(func);


  if (group_ndx > gr_mem_info->nof_groups)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GROUP_MEM_LL_GROUP_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = gr_mem_info->group_get_fun(
          gr_mem_info->instance_prim_handle,
          SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
          group_ndx,
          &grp_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tmp_iter = grp_entry.first_member;

  while (tmp_iter != SOC_SAND_GROUP_MEM_LL_END)
  {
    res = gr_mem_info->member_get_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            tmp_iter,
            &member_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    if (member_entry.group_id == SOC_SAND_GROUP_MEM_LL_END)
    {
      goto exit;
    }
    res = func(
            tmp_iter,
            param1,
            param2
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    tmp_iter = member_entry.next_member;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_group_print()",0,0);
}

 
uint32
  soc_sand_group_mem_ll_func_run_pointer_param(
    SOC_SAND_INOUT    SOC_SAND_GROUP_MEM_LL_INFO                    *gr_mem_info,
    SOC_SAND_IN       SOC_SAND_GROUP_MEM_LL_GROUP_ID                group_ndx,
    SOC_SAND_IN       SOC_SAND_GROUP_MEM_LL_ITER_FUN_POINTER_PARAM  func,
    SOC_SAND_INOUT    void                                          *opaque
  )
{
  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY
    grp_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY
    member_entry;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ID
    tmp_iter;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_GROUP_MEM_LL_FUNC_RUN);

  SOC_SAND_CHECK_NULL_INPUT(gr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(func);


  if (group_ndx > gr_mem_info->nof_groups)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GROUP_MEM_LL_GROUP_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = gr_mem_info->group_get_fun(
          gr_mem_info->instance_prim_handle,
          SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
          group_ndx,
          &grp_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tmp_iter = grp_entry.first_member;

  while (tmp_iter != SOC_SAND_GROUP_MEM_LL_END)
  {
    res = gr_mem_info->member_get_fun(
            gr_mem_info->instance_prim_handle,
            SOC_SAND_GROUP_MEM_LL_ACTIVE_INST(gr_mem_info,gr_mem_info->instance_sec_handle),
            tmp_iter,
            &member_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    if (member_entry.group_id == SOC_SAND_GROUP_MEM_LL_END)
    {
      goto exit;
    }
    res = func(tmp_iter,opaque);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    tmp_iter = member_entry.next_member;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_group_mem_ll_func_run_pointer_param()",0,0);
}

void
  soc_sand_SAND_GROUP_MEM_LL_INFO_clear(
    SOC_SAND_GROUP_MEM_LL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_GROUP_MEM_LL_INFO));
  info->auto_remove   = TRUE;
  info->nof_elements  = 0;
  info->nof_groups    = 0;
  info->wb_var_index  = SOC_DPP_WB_ENGINE_VAR_NONE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>


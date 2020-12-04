/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_SAND_GROUP_MEMBER_LIST_INCLUDED__

#define __SOC_SAND_GROUP_MEMBER_LIST_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>






#define SOC_SAND_GROUP_MEM_LL_END SOC_SAND_U32_MAX












typedef  uint32 SOC_SAND_GROUP_MEM_LL_GROUP_ID;
typedef  uint32 SOC_SAND_GROUP_MEM_LL_MEMBER_ID;


typedef uint32  SOC_SAND_GROUP_MEM_LL_ID;


typedef struct
{
  SOC_SAND_GROUP_MEM_LL_MEMBER_ID first_member;
}SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY;


typedef struct
{
 
  SOC_SAND_GROUP_MEM_LL_MEMBER_ID next_member;
 
  SOC_SAND_GROUP_MEM_LL_MEMBER_ID prev_member;
 
  SOC_SAND_GROUP_MEM_LL_GROUP_ID group_id;

}SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY;




typedef
  uint32
    (*SOC_SAND_GROUP_MEM_LL_ITER_FUN)(
      SOC_SAND_IN     SOC_SAND_GROUP_MEM_LL_MEMBER_ID member,
      SOC_SAND_INOUT  uint32                    param1,
      SOC_SAND_INOUT  uint32                    param2
    );


typedef
  uint32
    (*SOC_SAND_GROUP_MEM_LL_ITER_FUN_POINTER_PARAM)(
      SOC_SAND_IN     SOC_SAND_GROUP_MEM_LL_MEMBER_ID member,
      SOC_SAND_INOUT  void                            *opaque
    );



typedef
  uint32
    (*SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY_SET)(
      SOC_SAND_IN  int                             prime_handle,
      SOC_SAND_IN  uint32                             sec_handle,
      SOC_SAND_IN  uint32                             member_ndx,
      SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY       *member_entry
    );



typedef
  uint32
    (*SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY_GET)(
      SOC_SAND_IN  int                             prime_handle,
      SOC_SAND_IN  uint32                             sec_handle,
      SOC_SAND_IN  uint32                             member_ndx,
      SOC_SAND_OUT  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY      *member_entry
    );



typedef
  uint32
    (*SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY_SET)(
      SOC_SAND_IN  int                             prime_handle,
      SOC_SAND_IN  uint32                             sec_handle,
      SOC_SAND_IN  uint32                             group_ndx,
      SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY       *group_entry
    );

typedef
  uint32
    (*SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY_GET)(
      SOC_SAND_IN  int                             prime_handle,
      SOC_SAND_IN  uint32                             sec_handle,
      SOC_SAND_IN  uint32                             group_ndx,
      SOC_SAND_OUT  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY      *group_entry
    );

typedef struct
{
  
  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY *groups;
  
  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY *members;
 
  uint8 cache_enabled;
 
  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY *groups_cache;
  
  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY *members_cache;

} SOC_SAND_GROUP_MEM_LL_T;

typedef SOC_SAND_GROUP_MEM_LL_T  *SOC_SAND_GROUP_MEM_LL_PTR;


typedef struct 
{
  SOC_SAND_MAGIC_NUM_VAR
 
  uint8 auto_remove;
 
  uint32 nof_groups;
 
  uint32 instance_prim_handle;
 
  uint32 instance_sec_handle;
 
  uint32 nof_elements;
 
  uint8  support_caching;
 
  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY_SET group_set_fun;
  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY_GET group_get_fun;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY_SET member_set_fun;
  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY_GET member_get_fun;

 
  int32 wb_var_index;

 
  SOC_SAND_GROUP_MEM_LL_T group_members_data;

} SOC_SAND_GROUP_MEM_LL_INFO;












uint32
  soc_sand_group_mem_ll_create(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO      *gr_mem_info
  );



uint32
  soc_sand_group_mem_ll_destroy(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO      *gr_mem_info
  );


uint32
  soc_sand_group_mem_ll_clear(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO      *gr_mem_info
  );


uint32
  soc_sand_group_mem_ll_cache_set(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO      *gr_mem_info,
    SOC_SAND_IN uint8                             enable
  );


uint32
  soc_sand_group_mem_ll_commit(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO      *gr_mem_info,
    SOC_SAND_IN uint32 flags
  );


uint32
  soc_sand_group_mem_ll_rollback(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO      *gr_mem_info,
    SOC_SAND_IN uint32 flags
  );


uint32
  soc_sand_group_mem_ll_members_set(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO     *gr_mem_info,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ID     group_ndx,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_MEMBER_ID    *members,
    SOC_SAND_IN  uint32                       nof_members
  );



uint32
  soc_sand_group_mem_ll_member_add(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO     *gr_mem_info,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ID     group_ndx,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_MEMBER_ID    member_id
  );



uint32
  soc_sand_group_mem_ll_member_remove(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO     *gr_mem_info,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_MEMBER_ID    member_id
  );

uint32
  soc_sand_group_mem_ll_members_get(
    SOC_SAND_INOUT  SOC_SAND_GROUP_MEM_LL_INFO        *gr_mem_info,
    SOC_SAND_IN     SOC_SAND_GROUP_MEM_LL_GROUP_ID    group_ndx,
    SOC_SAND_IN     uint32                      max_members,
    SOC_SAND_INOUT  SOC_SAND_GROUP_MEM_LL_MEMBER_ID   *iter,
    SOC_SAND_OUT    SOC_SAND_GROUP_MEM_LL_MEMBER_ID   *members,
    SOC_SAND_OUT    uint32                      *nof_members
  );



uint32
  soc_sand_group_mem_ll_member_exist(
    SOC_SAND_INOUT  SOC_SAND_GROUP_MEM_LL_INFO        *gr_mem_info,
    SOC_SAND_IN     SOC_SAND_GROUP_MEM_LL_GROUP_ID    group_ndx,
    SOC_SAND_IN     SOC_SAND_GROUP_MEM_LL_MEMBER_ID   member_ndx,
    SOC_SAND_OUT    uint8                     *exist
  );


uint32
  soc_sand_group_mem_ll_is_group_empty(
    SOC_SAND_INOUT  SOC_SAND_GROUP_MEM_LL_INFO        *gr_mem_info,
    SOC_SAND_IN     SOC_SAND_GROUP_MEM_LL_GROUP_ID    group_ndx,
    SOC_SAND_OUT    uint8                     *is_empty
  );

uint32
  soc_sand_group_mem_ll_member_get_group(
    SOC_SAND_INOUT  SOC_SAND_GROUP_MEM_LL_INFO        *gr_mem_info,
    SOC_SAND_IN     SOC_SAND_GROUP_MEM_LL_MEMBER_ID   member_ndx,
    SOC_SAND_OUT    SOC_SAND_GROUP_MEM_LL_GROUP_ID    *group_ndx,
    SOC_SAND_OUT    uint8                     *valid
  );



uint32
  soc_sand_group_mem_ll_group_clear(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO     *gr_mem_info,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ID     group_ndx
  );


uint32
  soc_sand_group_mem_ll_group_print(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO     *gr_mem_info,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ID     group_ndx
  );


uint32
  soc_sand_group_mem_ll_func_run(
    SOC_SAND_INOUT   SOC_SAND_GROUP_MEM_LL_INFO     *gr_mem_info,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ID     group_ndx,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_ITER_FUN     func,
    SOC_SAND_INOUT  uint32                    param1,
    SOC_SAND_INOUT  uint32                    param2
  );


uint32
  soc_sand_group_mem_ll_func_run_pointer_param(
    SOC_SAND_INOUT    SOC_SAND_GROUP_MEM_LL_INFO                    *gr_mem_info,
    SOC_SAND_IN       SOC_SAND_GROUP_MEM_LL_GROUP_ID                group_ndx,
    SOC_SAND_IN       SOC_SAND_GROUP_MEM_LL_ITER_FUN_POINTER_PARAM  func,
    SOC_SAND_INOUT    void                                          *opaque
  );

void
  soc_sand_SAND_GROUP_MEM_LL_INFO_clear(
    SOC_SAND_GROUP_MEM_LL_INFO *info
  );

uint32
  soc_sand_group_mem_ll_get_size_for_save(
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_INFO      *gr_mem_info,
    SOC_SAND_OUT  uint32                   *size
  );

uint32
  soc_sand_group_mem_ll_save(
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_INFO     *info,
    SOC_SAND_OUT uint8                    *buffer,
    SOC_SAND_IN  uint32                   buffer_size_bytes,
    SOC_SAND_OUT uint32                   *actual_size_bytes
  );

uint32
  soc_sand_group_mem_ll_load(
    SOC_SAND_IN  uint8                            **buffer,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY_SET  group_set_fun,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY_GET  group_get_fun,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY_SET member_set_fun,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY_GET member_get_fun,
    SOC_SAND_OUT SOC_SAND_GROUP_MEM_LL_INFO             *info
  );





#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_SAND_PAT_INCLUDED__

#define __SOC_SAND_PAT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>






#define SOC_SAND_PAT_TREE_NOF_NODE_CHILD 2
#define SOC_SAND_PAT_TREE_PREFIX_MAX_LEN 32
#define SOC_SAND_PAT_TREE_NULL SOC_SAND_U32_MAX
#define SOC_SAND_PAT_TREE_PREFIX_INVALID 255





#define SOC_SAND_PAT_TREE_FIND_IDENTICAL 1

#define SOC_SAND_PAT_TREE_FIND_LOAD_NODE 2
#define SOC_SAND_PAT_TREE_FIND_STORE_NODE  4

#define SOC_SAND_PAT_TREE_FIND_EXACT  8







#define SOC_SAND_PAT_TREE_ITER_IS_END(iter)    \
            (((iter)->arr[0] == SOC_SAND_PAT_TREE_NULL) && (((iter)->arr[1] == SOC_SAND_PAT_TREE_NULL)))

#define SOC_SAND_PAT_TREE_IS_CACHED_INST( _inst)   \
        (uint8)SOC_SAND_GET_BITS_RANGE(_inst,31,31)

#define SOC_SAND_PAT_TREE_TREE_INST( _inst)   \
  SOC_SAND_GET_BITS_RANGE(_inst,30,0)






typedef  uint32 SOC_SAND_PAT_TREE_KEY;
typedef uint32 SOC_SAND_PAT_TREE_NODE_PLACE;



typedef uint32  SOC_SAND_PAT_TREE_ID;


typedef enum
{
  
  SOC_SAND_PAT_TREE_ITER_TYPE_FAST=0,
  
  SOC_SAND_PAT_TREE_ITER_TYPE_KEY_PREFIX_ORDERED=1,
  
  SOC_SAND_PAT_TREE_ITER_TYPE_PREFIX_KEY_ORDERED=2
}SOC_SAND_PAT_TREE_ITER_TYPE;


#define SAND_PAT_TREE_NODE_FLAGS_PREFIX  (0x1)

typedef struct
{
  SOC_SAND_PAT_TREE_NODE_PLACE child[SOC_SAND_PAT_TREE_NOF_NODE_CHILD];
 
  SOC_SAND_PAT_TREE_KEY key;
 
  uint32 data;

 
  uint8 prefix;

  uint8 is_prefix;
}SOC_SAND_PAT_TREE_NODE;




typedef
  uint32
    (*SOC_SAND_PAT_TREE_SW_DB_TREE_ROOT_SET)(
      SOC_SAND_IN  int                     prime_handle,
      SOC_SAND_IN  uint32                     sec_handle,
      SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_PLACE     root_place
    );


typedef
  uint32
    (*SOC_SAND_PAT_TREE_SW_DB_TREE_ROOT_GET)(
      SOC_SAND_IN  int                     prime_handle,
      SOC_SAND_IN  uint32                     sec_handle,
      SOC_SAND_OUT  SOC_SAND_PAT_TREE_NODE_PLACE    *root_place
    );



typedef
  uint32
    (*SOC_SAND_PAT_TREE_SW_DB_TREE_NODE_INFO_SET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_IN SOC_SAND_PAT_TREE_NODE_PLACE    node_place,
      SOC_SAND_IN SOC_SAND_PAT_TREE_NODE          *node_info
    );



typedef
  uint32
    (*SOC_SAND_PAT_TREE_SW_DB_TREE_NODE_INFO_GET)(
      SOC_SAND_IN  int                 prime_handle,
      SOC_SAND_IN  uint32                 sec_handle,
      SOC_SAND_IN SOC_SAND_PAT_TREE_NODE_PLACE  node_place,
      SOC_SAND_OUT SOC_SAND_PAT_TREE_NODE       *node_info
    );

typedef
  uint32
    (*SOC_SAND_PAT_TREE_SW_DB_TREE_NODE_REF_GET)(
      SOC_SAND_IN  int                 prime_handle,
      SOC_SAND_IN  uint32                 sec_handle,
      SOC_SAND_IN SOC_SAND_PAT_TREE_NODE_PLACE  node_place,
      SOC_SAND_OUT SOC_SAND_PAT_TREE_NODE       **node_info
    );


typedef
  uint8
    (*SOC_SAND_PAT_TREE_NODE_IS_SKIP_IN_LPM_IDENTICAL_DATA_QUERY_FUN)(
      SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info
    );



typedef
  uint8
    (*SOC_SAND_PAT_TREE_NODE_DATA_IS_IDENTICAL_FUN)(
      SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_0,
      SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_1
  );



typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 
    uint32  val;
 
  uint8 prefix;
}SOC_SAND_PAT_TREE_NODE_KEY;


typedef struct {
  SOC_SAND_MAGIC_NUM_VAR

  SOC_SAND_PAT_TREE_NODE_PLACE root;
  
  SOC_SAND_PAT_TREE_NODE *tree_memory;
  
  SOC_SAND_OCC_BM_PTR memory_use ;
  
  SOC_SAND_OCC_BM_T *memory_use_ptr ;
  
  int               memory_use_unit ;

  uint8 cache_enabled;

  SOC_SAND_PAT_TREE_NODE *tree_memory_cache;
  SOC_SAND_PAT_TREE_NODE_PLACE root_cache;

  
  SOC_SAND_PAT_TREE_NODE_KEY cache_change_head;

  
  SOC_SAND_PAT_TREE_NODE_PLACE current_node_place;

} SOC_SAND_PAT_TREE_T;



typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
 
  uint32
    tree_size;
  SOC_SAND_PAT_TREE_SW_DB_TREE_NODE_INFO_SET node_set_fun;
  SOC_SAND_PAT_TREE_SW_DB_TREE_NODE_INFO_GET node_get_fun;
  SOC_SAND_PAT_TREE_SW_DB_TREE_NODE_REF_GET node_ref_get_fun;
  SOC_SAND_PAT_TREE_SW_DB_TREE_ROOT_SET root_set_fun;
  SOC_SAND_PAT_TREE_SW_DB_TREE_ROOT_GET root_get_fun;
  SOC_SAND_PAT_TREE_NODE_DATA_IS_IDENTICAL_FUN node_data_is_identical_fun;
  SOC_SAND_PAT_TREE_NODE_IS_SKIP_IN_LPM_IDENTICAL_DATA_QUERY_FUN node_is_skip_in_lpm_identical_data_query_fun;
  int prime_handle;
  uint32
    sec_handle;
  uint8
    support_cache;

 
  int32 wb_var_index;

 
  SOC_SAND_PAT_TREE_T
    pat_tree_data;
} SOC_SAND_PAT_TREE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 
    uint32  payload;
}SOC_SAND_PAT_TREE_NODE_DATA;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 
  SOC_SAND_PAT_TREE_NODE_KEY  key;
  SOC_SAND_PAT_TREE_NODE_DATA data;
  
  SOC_SAND_PAT_TREE_NODE_PLACE node_place;

}SOC_SAND_PAT_TREE_NODE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
    
  uint32    flags ;
  
  SOC_SAND_PAT_TREE_SW_DB_TREE_NODE_INFO_SET node_set_fun;
  SOC_SAND_PAT_TREE_SW_DB_TREE_NODE_INFO_GET node_get_fun;
  SOC_SAND_PAT_TREE_SW_DB_TREE_NODE_REF_GET node_ref_get_fun;
  SOC_SAND_PAT_TREE_SW_DB_TREE_ROOT_SET root_set_fun;
  SOC_SAND_PAT_TREE_SW_DB_TREE_ROOT_GET root_get_fun;
  SOC_SAND_PAT_TREE_NODE_DATA_IS_IDENTICAL_FUN node_data_is_identical_fun;
  SOC_SAND_PAT_TREE_NODE_IS_SKIP_IN_LPM_IDENTICAL_DATA_QUERY_FUN node_is_skip_in_lpm_identical_data_query_fun;
}SOC_SAND_PAT_TREE_LOAD_INFO;


typedef
  uint32
    (*SOC_SAND_PAT_TREE_FOREACH_FUN)(
      SOC_SAND_INOUT SOC_SAND_PAT_TREE_INFO *pat_tree,
      SOC_SAND_IN SOC_SAND_PAT_TREE_KEY key,
      SOC_SAND_IN uint8 prefix,
      SOC_SAND_INOUT uint32 *data,
      SOC_SAND_IN uint8 is_prefix,
      SOC_SAND_INOUT void* vparam
    );












uint32
  soc_sand_pat_tree_create(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info
  );


uint32
  soc_sand_pat_tree_destroy(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info
  );



uint32
  soc_sand_pat_tree_clear(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info
  );


uint32
  soc_sand_pat_tree_clear_nodes(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx
  );


uint32
  soc_sand_pat_tree_root_reset(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx
  );


uint32
  soc_sand_pat_tree_node_add(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_KEY     *node_key,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_DATA    *node_data,
    SOC_SAND_OUT  uint8                 *success
  );


uint32
  soc_sand_pat_tree_node_remove(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_KEY    *node_key
  );



uint32
  soc_sand_pat_tree_lpm_get(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO       *tree_info,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_KEY     *node_key,
    SOC_SAND_IN  uint8                  identical_payload,
    SOC_SAND_OUT  SOC_SAND_PAT_TREE_NODE_INFO   *lpm_info,
    SOC_SAND_OUT  uint8                 *found
  );


uint32
  soc_sand_pat_tree_node_at_place(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO        *tree_info_ndx,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_PLACE  node_place,
    SOC_SAND_OUT  SOC_SAND_PAT_TREE_NODE_INFO   *lpm_info,
    SOC_SAND_OUT  uint8                 *exist);



uint32
  soc_sand_pat_tree_head_key_of_changes(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO     *tree_info_ndx,
    SOC_SAND_OUT  SOC_SAND_PAT_TREE_NODE_KEY     *cached_head
  );



uint32
  soc_sand_pat_tree_get_block(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO       *tree_info,
    SOC_SAND_IN SOC_SAND_PAT_TREE_ITER_TYPE     iter_type,
    SOC_SAND_INOUT  SOC_SAND_U64                *iter,
    SOC_SAND_IN  uint32                   entries_to_scan,
    SOC_SAND_IN  uint32                   entries_to_get,
    SOC_SAND_OUT  SOC_SAND_PAT_TREE_NODE_INFO   *nodes,
    SOC_SAND_OUT uint32                   *nof_entries
  );



uint32
  soc_sand_pat_tree_get_size(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info,
    SOC_SAND_OUT  uint32                    *total_nodes,
    SOC_SAND_OUT  uint32                    *prefix_nodes,
    SOC_SAND_OUT  uint32                    *free_nodes
  );


uint32
  soc_sand_pat_tree_foreach(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO         *tree_info,
    SOC_SAND_IN     SOC_SAND_PAT_TREE_FOREACH_FUN  fun,
    SOC_SAND_INOUT     void                        *param
  );

uint32
  soc_sand_pat_tree_cache_set(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO        *tree_info_ndx,
    SOC_SAND_IN  uint8             cached
  );

uint32
  soc_sand_pat_tree_cache_commit(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO        *tree_info_ndx,
    SOC_SAND_IN  uint32               flags
  );

uint32
  soc_sand_pat_tree_cache_rollback(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO        *tree_info_ndx,
    SOC_SAND_IN  uint32               flags
  );



uint32
  soc_sand_pat_tree_get_size_for_save(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO              *tree_info,
    SOC_SAND_IN  uint32                        flags,
    SOC_SAND_OUT  uint32                       *size
  );


uint32
  soc_sand_pat_tree_save(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO     *tree_info,
    SOC_SAND_IN  uint32                flags,
    SOC_SAND_OUT uint8                 *buffer,
    SOC_SAND_IN  uint32                buffer_size_bytes,
    SOC_SAND_OUT uint32                *actual_size_bytes
  );


uint32
  soc_sand_pat_tree_load(
    SOC_SAND_IN  uint8                           **buffer,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_LOAD_INFO           *load_info,
    SOC_SAND_OUT SOC_SAND_PAT_TREE_INFO               *tree_info
  );  

void
  soc_sand_SAND_PAT_TREE_LOAD_INFO_clear(
    SOC_SAND_PAT_TREE_LOAD_INFO *info
  );
void
  soc_sand_SAND_PAT_TREE_INFO_clear(
    SOC_SAND_PAT_TREE_INFO *info
  );

#if SOC_SAND_DEBUG

uint32
  soc_sand_pat_tree_print(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_PLACE   node_place,
    SOC_SAND_IN  uint8                  including_glue,
    SOC_SAND_IN  uint8                  print_subtree
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_SAND_ARRAY_MEMORY_ALLOCATOR_INCLUDED__

#define __SOC_SAND_ARRAY_MEMORY_ALLOCATOR_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>







#define SOC_SAND_ARR_MEM_ALLOCATOR_NULL SOC_SAND_U32_MAX
#define SOC_SAND_ARR_MEM_ALLOCATOR_MIN_MALLOC_SIZE 2

#define SOC_SAND_ARR_MEM_ALLOCATOR_MAX_NOF_REQS 3






#define SOC_SAND_ARR_MEM_ALLOCATOR_ITER_END(iter)   \
        ((iter)->offset == SOC_SAND_ARR_MEM_ALLOCATOR_NULL)

#define SOC_SAND_ARR_MEM_ALLOCATOR_IS_CACHED_INST( _inst)   \
        (uint8)SOC_SAND_GET_BITS_RANGE(_inst,31,31)

#define SOC_SAND_ARR_MEM_ALLOCATOR_BANK_INST(_inst)   \
  SOC_SAND_GET_BITS_RANGE(_inst,30,0)







typedef  uint32 SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY;
typedef  uint32 SOC_SAND_ARR_MEM_ALLOCATOR_PTR;


typedef uint32  SOC_SAND_ARR_MEM_ALLOCATOR_ID;














typedef
  uint32
  (*SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SET)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  uint32                             entry_ndx,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY         *entry
  );



typedef
  uint32
    (*SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_GET)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  uint32                             entry_ndx,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY        *entry
  );



typedef
  uint32
  (*SOC_SAND_ARR_MEM_ALLOCATOR_ROW_WRITE)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY         *data
  );


typedef
  uint32
  (*SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_MOVE_CALL_BACK)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY         *data,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR           old_place,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR           new_place
  );


typedef
  uint32
  (*SOC_SAND_ARR_MEM_ALLOCATOR_ROW_READ)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY        *data
  );



typedef
  uint32
  (*SOC_SAND_ARR_MEM_ALLOCATOR_FREE_LIST_SET)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR           free_ptr
  );



typedef
  uint32
  (*SOC_SAND_ARR_MEM_ALLOCATOR_FREE_LIST_GET)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_PTR          *free_ptr
  );

typedef struct
{
 
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY *array;

  SOC_SAND_ARR_MEM_ALLOCATOR_PTR free_list;

  uint32  *mem_shadow;
 
  uint8 cache_enabled;

  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY *array_cache;

  SOC_SAND_ARR_MEM_ALLOCATOR_PTR free_list_cache;

  uint32 nof_updates;

  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY *update_indexes;

  uint32  *mem_shadow_cache;


} SOC_SAND_ARR_MEM_ALLOCATOR_T;


typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
 
  uint32 instance_prim_handle;
 
  uint32 instance_sec_handle;
 
  uint32 nof_entries;
 
  uint8 support_caching;
 
  uint8 support_defragment;
 
  uint32 max_block_size;
 
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SET entry_set_fun;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_GET entry_get_fun;
  SOC_SAND_ARR_MEM_ALLOCATOR_FREE_LIST_SET free_set_fun;
  SOC_SAND_ARR_MEM_ALLOCATOR_FREE_LIST_GET free_get_fun;
  SOC_SAND_ARR_MEM_ALLOCATOR_ROW_READ read_fun;
  SOC_SAND_ARR_MEM_ALLOCATOR_ROW_WRITE write_fun;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_MOVE_CALL_BACK entry_move_fun;

 
  SOC_SAND_ARR_MEM_ALLOCATOR_T arr_mem_allocator_data;

} SOC_SAND_ARR_MEM_ALLOCATOR_INFO;


typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
 
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR offset;
 
  uint32 block_size;

} SOC_SAND_ARR_MEM_ALLOCATOR_ITER;


typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
 
  uint32 block_size[SOC_SAND_ARR_MEM_ALLOCATOR_MAX_NOF_REQS];

  uint32 nof_reqs;

} SOC_SAND_ARR_MEM_ALLOCATOR_REQ_BLOCKS;


typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
 
  uint32 total_size;
 
  uint32 total_free;
 
  uint32 nof_fragments;
 
  uint32 max_free_block_size;

} SOC_SAND_ARR_MEM_ALLOCATOR_MEM_STATUS;


typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
 
  SOC_SAND_TABLE_BLOCK_RANGE             block_range;

} SOC_SAND_ARR_MEM_ALLOCATOR_DEFRAG_INFO;


typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 flags;

  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SET entry_set_fun;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_GET entry_get_fun;
  SOC_SAND_ARR_MEM_ALLOCATOR_FREE_LIST_SET free_set_fun;
  SOC_SAND_ARR_MEM_ALLOCATOR_FREE_LIST_GET free_get_fun;
  SOC_SAND_ARR_MEM_ALLOCATOR_ROW_READ read_fun;
  SOC_SAND_ARR_MEM_ALLOCATOR_ROW_WRITE write_fun;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_MOVE_CALL_BACK entry_move_fun;

} SOC_SAND_ARR_MEM_ALLOCATOR_LOAD_INFO;



uint32
  soc_sand_arr_mem_allocator_create(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info
  );


uint32
  soc_sand_arr_mem_allocator_clear(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info
  );


uint32
  soc_sand_arr_mem_allocator_destroy(
  SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info
  );


uint32
  soc_sand_arr_mem_allocator_cache_set(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_IN uint8                             enable
  );


uint32
  soc_sand_arr_mem_allocator_commit(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_IN uint32 flags
  );


uint32
  soc_sand_arr_mem_allocator_rollback(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_IN uint32 flags
  );


uint32
  soc_sand_arr_mem_allocator_malloc(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  uint32                            size,
    SOC_SAND_IN  uint32                            alignment,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_PTR         *ptr
  );



uint32
  soc_sand_arr_mem_allocator_free(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR          ptr
  );


uint32
  soc_sand_arr_mem_allocator_block_size(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR          ptr,
    SOC_SAND_OUT  uint32                           *size
  );

uint32
  soc_sand_arr_mem_allocator_write(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR          offset,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY       *data
  );



uint32
  soc_sand_arr_mem_allocator_read(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR          offset,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY        *data
  );



uint32
  soc_sand_arr_mem_allocator_defrag(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO       *arr_mem_info,
    SOC_SAND_INOUT SOC_SAND_ARR_MEM_ALLOCATOR_DEFRAG_INFO  *defrag_info
  );


uint32
  soc_sand_arr_mem_allocator_read_block(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_INOUT  SOC_SAND_ARR_MEM_ALLOCATOR_ITER       *iter,
    SOC_SAND_IN  uint32                             entries_to_read,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY        *data,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_PTR          *addresses,
    SOC_SAND_OUT  uint32                            *nof_entries
  );


uint32
  soc_sand_arr_mem_allocator_get_size_for_save(
    SOC_SAND_IN   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_IN  uint32                        flags,
    SOC_SAND_OUT  uint32                       *size
  );

uint32
  soc_sand_arr_mem_allocator_save(
    SOC_SAND_IN   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_IN  uint32                flags,
    SOC_SAND_OUT uint8                 *buffer,
    SOC_SAND_IN  uint32                buffer_size_bytes,
    SOC_SAND_OUT uint32                *actual_size_bytes
  );

uint32
  soc_sand_arr_mem_allocator_load(
    SOC_SAND_IN  uint8                           **buffer,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_LOAD_INFO  *load_info,
    SOC_SAND_OUT SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info
  );


uint32
  soc_sand_arr_mem_allocator_print_free(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info
  );

uint32
  soc_sand_arr_mem_allocator_print_free_by_order(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info
  );

uint32
  soc_sand_arr_mem_allocator_is_availabe_blocks(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
	  SOC_SAND_IN   SOC_SAND_ARR_MEM_ALLOCATOR_REQ_BLOCKS  *req_blocks,
	  SOC_SAND_OUT   uint8                         *available
  );

uint32
  soc_sand_arr_mem_allocator_mem_status_get(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
	  SOC_SAND_OUT   SOC_SAND_ARR_MEM_ALLOCATOR_MEM_STATUS  *mem_status
  );


void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_INFO_clear(
    SOC_SAND_ARR_MEM_ALLOCATOR_INFO *info
  );

void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_ITER_clear(
    SOC_SAND_ARR_MEM_ALLOCATOR_ITER *info
  );

void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_REQ_BLOCKS_clear(
    SOC_SAND_ARR_MEM_ALLOCATOR_REQ_BLOCKS *info
  );

void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_MEM_STATUS_clear(
    SOC_SAND_ARR_MEM_ALLOCATOR_MEM_STATUS *info
  );

void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_DEFRAG_INFO_clear(
    SOC_SAND_ARR_MEM_ALLOCATOR_DEFRAG_INFO *info
  );


void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_LOAD_INFO_clear(
    SOC_SAND_ARR_MEM_ALLOCATOR_LOAD_INFO *info
  );



#if SOC_SAND_DEBUG
void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_INFO_print(
    SOC_SAND_ARR_MEM_ALLOCATOR_INFO *info
  );

void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_ITER_print(
    SOC_SAND_ARR_MEM_ALLOCATOR_ITER *info
  );

void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_REQ_BLOCKS_print(
    SOC_SAND_IN SOC_SAND_ARR_MEM_ALLOCATOR_REQ_BLOCKS *info
  );

void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_MEM_STATUS_print(
    SOC_SAND_IN SOC_SAND_ARR_MEM_ALLOCATOR_MEM_STATUS *info
  );

void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_DEFRAG_INFO_print(
    SOC_SAND_IN SOC_SAND_ARR_MEM_ALLOCATOR_DEFRAG_INFO *info
  );
#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

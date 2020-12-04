/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_PCP_ARRAY_MEMORY_ALLOCATOR_INCLUDED__

#define __SOC_PCP_ARRAY_MEMORY_ALLOCATOR_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>








#define ARAD_PP_ARR_MEM_ALLOCATOR_NULL SOC_SAND_U32_MAX
#define ARAD_PP_ARR_MEM_ALLOCATOR_MIN_MALLOC_SIZE 2

#define ARAD_PP_ARR_MEM_ALLOCATOR_MAX_NOF_REQS 3






#define ARAD_PP_ARR_MEM_ALLOCATOR_ITER_END(iter)   \
        ((iter)->offset == ARAD_PP_ARR_MEM_ALLOCATOR_NULL)

#define ARAD_PP_ARR_MEM_ALLOCATOR_IS_CACHED_INST( _inst)   \
        (uint8)SOC_SAND_GET_BITS_RANGE(_inst,31,31)

#define ARAD_PP_ARR_MEM_ALLOCATOR_BANK_INST(_inst)   \
  SOC_SAND_GET_BITS_RANGE(_inst,30,0)








typedef enum
{
  ARAD_PP_ARR_MEM_ALLOCATOR_MALLOC = ARAD_PP_PROC_DESC_BASE_ARR_MEM_ALLOCATOR_FIRST,
  ARAD_PP_ARR_MEM_ALLOCATOR_CREATE,
  ARAD_PP_ARR_MEM_ALLOCATOR_INITIALIZE,
  ARAD_PP_ARR_MEM_ALLOCATOR_CLEAR,
  ARAD_PP_ARR_MEM_ALLOCATOR_DESTROY,
  ARAD_PP_ARR_MEM_ALLOCATOR_FREE,
  ARAD_PP_ARR_MEM_ALLOCATOR_WRITE,
  ARAD_PP_ARR_MEM_ALLOCATOR_READ,
  ARAD_PP_ARR_MEM_ALLOCATOR_READ_BLOCK,
  
  ARAD_PP_ARR_MEM_ALLOCATOR_PROCEDURE_DESC_LAST
} ARAD_PP_ARRAY_MEM_ALLOCATOR_PROCEDURE_DESC;

typedef enum
{
  ARAD_PP_ARR_MEM_ALLOCATOR_NOF_LINES_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_ARRAY_MEMORY_ALLOCATOR_FIRST,
  ARAD_PP_ARR_MEM_ALLOCATOR_MALLOC_SIZE_OUT_OF_RANGE_ERR,
  ARAD_PP_ARR_MEM_ALLOCATOR_POINTER_OF_RANGE_ERR,

  
  ARAD_PP_ARRAY_MEM_ALLOCATOR_ERR_LAST
} ARAD_PP_ARRAY_MEM_ALLOCATOR_ERR;


typedef  uint32 ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY;
typedef  uint32 ARAD_PP_ARR_MEM_ALLOCATOR_PTR;


typedef uint32  ARAD_PP_ARR_MEM_ALLOCATOR_ID;














typedef
  uint32
  (*ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY_SET)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  uint32                             entry_ndx,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY         *entry
  );



typedef
  uint32
    (*ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY_GET)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  uint32                             entry_ndx,
    SOC_SAND_OUT  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY        *entry
  );



typedef
  uint32
  (*ARAD_PP_ARR_MEM_ALLOCATOR_ROW_WRITE)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY         *data
  );

typedef
  uint32
  (*ARAD_PP_ARR_MEM_ALLOCATOR_ROWS_WRITE)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             nof_entries,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY         *data
  );



typedef
  uint32
  (*ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY_MOVE_CALL_BACK)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY         *data,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_PTR           old_place,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_PTR           new_place
  );


typedef
  uint32
  (*ARAD_PP_ARR_MEM_ALLOCATOR_ROW_READ)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_OUT  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY        *data
  );

typedef
  uint32
  (*ARAD_PP_ARR_MEM_ALLOCATOR_ROWS_READ)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             nof_entries,
    SOC_SAND_OUT  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY        *data
  );



typedef
  uint32
  (*ARAD_PP_ARR_MEM_ALLOCATOR_FREE_LIST_SET)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_PTR           free_ptr
  );



typedef
  uint32
  (*ARAD_PP_ARR_MEM_ALLOCATOR_FREE_LIST_GET)(
    SOC_SAND_IN  int                             prime_handle,
    SOC_SAND_IN  uint32                             sec_handle,
    SOC_SAND_OUT  ARAD_PP_ARR_MEM_ALLOCATOR_PTR          *free_ptr
  );

typedef struct
{
 
  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY *array;

  ARAD_PP_ARR_MEM_ALLOCATOR_PTR free_list;

  uint32  *mem_shadow;
 
  uint8 cache_enabled;

  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY *array_cache;

  ARAD_PP_ARR_MEM_ALLOCATOR_PTR free_list_cache;

  uint32 nof_updates;

  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY *update_indexes;

  uint32  *mem_shadow_cache;


} ARAD_PP_ARR_MEM_ALLOCATOR_T;


typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
 
  uint32 instance_prim_handle;
 
  uint32 instance_sec_handle;
 
  uint32 nof_entries;
 
  uint32 entry_size;
 
  uint8 support_caching;
 
  uint8 support_defragment;
 
  uint32 max_block_size;
 
  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY_SET entry_set_fun;
  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY_GET entry_get_fun;
  ARAD_PP_ARR_MEM_ALLOCATOR_FREE_LIST_SET free_set_fun;
  ARAD_PP_ARR_MEM_ALLOCATOR_FREE_LIST_GET free_get_fun;
  ARAD_PP_ARR_MEM_ALLOCATOR_ROW_READ read_fun;
  ARAD_PP_ARR_MEM_ALLOCATOR_ROW_WRITE write_fun;
  ARAD_PP_ARR_MEM_ALLOCATOR_ROWS_READ read_block_fun;
  ARAD_PP_ARR_MEM_ALLOCATOR_ROWS_WRITE write_block_fun;
  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY_MOVE_CALL_BACK entry_move_fun;


 
 int32 wb_var_index;

 
  ARAD_PP_ARR_MEM_ALLOCATOR_T arr_mem_allocator_data;

} ARAD_PP_ARR_MEM_ALLOCATOR_INFO;


typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
 
  ARAD_PP_ARR_MEM_ALLOCATOR_PTR offset;
 
  uint32 block_size;

} ARAD_PP_ARR_MEM_ALLOCATOR_ITER;


typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
 
  uint32 block_size[ARAD_PP_ARR_MEM_ALLOCATOR_MAX_NOF_REQS];

  uint32 nof_reqs;

} ARAD_PP_ARR_MEM_ALLOCATOR_REQ_BLOCKS;


typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
 
  uint32 total_size;
 
  uint32 total_free;
 
  uint32 nof_fragments;
 
  uint32 max_free_block_size;

} ARAD_PP_ARR_MEM_ALLOCATOR_MEM_STATUS;


typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
 
  SOC_SAND_TABLE_BLOCK_RANGE             block_range;

} ARAD_PP_ARR_MEM_ALLOCATOR_DEFRAG_INFO;




uint32
  arad_pp_arr_mem_allocator_create(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info
  );



uint32
  arad_pp_arr_mem_allocator_initialize(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info
  );


uint32
  arad_pp_arr_mem_allocator_clear(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info
  );


uint32
  arad_pp_arr_mem_allocator_destroy(
  SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info
  );


uint32
  arad_pp_arr_mem_allocator_cache_set(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_IN uint8                             enable
  );


uint32
  arad_pp_arr_mem_allocator_commit(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_IN uint32 flags
  );


uint32
  arad_pp_arr_mem_allocator_rollback(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_IN uint32 flags
  );


uint32
  arad_pp_arr_mem_allocator_malloc(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  uint32                            size,
    SOC_SAND_IN  uint32                            alignment,
    SOC_SAND_OUT  ARAD_PP_ARR_MEM_ALLOCATOR_PTR         *ptr
  );



uint32
  arad_pp_arr_mem_allocator_free(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_PTR          ptr
  );


uint32
  arad_pp_arr_mem_allocator_block_size(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_PTR          ptr,
    SOC_SAND_OUT  uint32                           *size
  );

uint32
  arad_pp_arr_mem_allocator_write(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_PTR          offset,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY       *data
  );

uint32
  arad_pp_arr_mem_allocator_write_chunk(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_PTR          offset,
    SOC_SAND_IN  uint32                                  nof_entries,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY       *data
  );



uint32
  arad_pp_arr_mem_allocator_read(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_PTR          offset,
    SOC_SAND_OUT  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY        *data
  );

uint32
  arad_pp_arr_mem_allocator_read_chunk(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  ARAD_PP_ARR_MEM_ALLOCATOR_PTR          offset,
    SOC_SAND_IN  uint32                                  nof_entries,
    SOC_SAND_OUT  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY        *data
  );


uint32
  arad_pp_arr_mem_allocator_defrag(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO       *arr_mem_info,
    SOC_SAND_INOUT ARAD_PP_ARR_MEM_ALLOCATOR_DEFRAG_INFO  *defrag_info
  );


uint32
  arad_pp_arr_mem_allocator_read_block(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_INOUT  ARAD_PP_ARR_MEM_ALLOCATOR_ITER       *iter,
    SOC_SAND_IN  uint32                             entries_to_read,
    SOC_SAND_OUT  ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY        *data,
    SOC_SAND_OUT  ARAD_PP_ARR_MEM_ALLOCATOR_PTR          *addresses,
    SOC_SAND_OUT  uint32                            *nof_entries
  );


uint32
  arad_pp_arr_mem_allocator_print_free(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info
  );

uint32
  arad_pp_arr_mem_allocator_print_free_by_order(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info
  );

uint32
  arad_pp_arr_mem_allocator_is_availabe_blocks(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
	  SOC_SAND_IN   ARAD_PP_ARR_MEM_ALLOCATOR_REQ_BLOCKS  *req_blocks,
	  SOC_SAND_OUT   uint8                         *available
  );

uint32
  arad_pp_arr_mem_allocator_mem_status_get(
    SOC_SAND_INOUT   ARAD_PP_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
	  SOC_SAND_OUT   ARAD_PP_ARR_MEM_ALLOCATOR_MEM_STATUS  *mem_status
  );


void
  ARAD_PP_ARR_MEM_ALLOCATOR_INFO_clear(
    ARAD_PP_ARR_MEM_ALLOCATOR_INFO *info
  );

void
  ARAD_PP_ARR_MEM_ALLOCATOR_ITER_clear(
    ARAD_PP_ARR_MEM_ALLOCATOR_ITER *info
  );

void
  ARAD_PP_ARR_MEM_ALLOCATOR_REQ_BLOCKS_clear(
    ARAD_PP_ARR_MEM_ALLOCATOR_REQ_BLOCKS *info
  );

void
  ARAD_PP_ARR_MEM_ALLOCATOR_MEM_STATUS_clear(
    ARAD_PP_ARR_MEM_ALLOCATOR_MEM_STATUS *info
  );

void
  ARAD_PP_ARR_MEM_ALLOCATOR_DEFRAG_INFO_clear(
    ARAD_PP_ARR_MEM_ALLOCATOR_DEFRAG_INFO *info
  );


#if SOC_SAND_DEBUG
void
  ARAD_PP_ARR_MEM_ALLOCATOR_INFO_print(
    ARAD_PP_ARR_MEM_ALLOCATOR_INFO *info
  );

void
  ARAD_PP_ARR_MEM_ALLOCATOR_ITER_print(
    ARAD_PP_ARR_MEM_ALLOCATOR_ITER *info
  );

void
  ARAD_PP_ARR_MEM_ALLOCATOR_REQ_BLOCKS_print(
    SOC_SAND_IN ARAD_PP_ARR_MEM_ALLOCATOR_REQ_BLOCKS *info
  );

void
  ARAD_PP_ARR_MEM_ALLOCATOR_MEM_STATUS_print(
    SOC_SAND_IN ARAD_PP_ARR_MEM_ALLOCATOR_MEM_STATUS *info
  );

void
  ARAD_PP_ARR_MEM_ALLOCATOR_DEFRAG_INFO_print(
    SOC_SAND_IN ARAD_PP_ARR_MEM_ALLOCATOR_DEFRAG_INFO *info
  );
#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

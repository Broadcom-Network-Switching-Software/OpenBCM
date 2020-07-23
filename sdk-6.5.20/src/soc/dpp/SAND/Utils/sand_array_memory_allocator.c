/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_array_memory_allocator.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>






#define SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END      0xFFFFF

#define SOC_SAND_ARR_MEM_ALLOCATOR_FREE_ENTRY_LEN      2

#define SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE_LSB      0
#define SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE_MSB      19
#define SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_LSB  20
#define SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_MSB  29
#define SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_PREV_PTR_LSB  0
#define SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_PREV_PTR_MSB  19
#define SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_SIZE 10
#define SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_USE_LSB      31
#define SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_USE_MSB      31



#define SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_COPY           (0x1)
#define SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_FREE           (0x2)
#define SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_UPDATE_CONT    (0x4)
#define SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_UPDATE_REF     (0x8)
#define SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_BLK_SIZE_VLD   (0x10)




#define SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_IS_USED(entry)   \
            SOC_SAND_GET_BITS_RANGE(entry,SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_USE_MSB,SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_USE_LSB)

#define SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(entry)   \
            SOC_SAND_GET_BITS_RANGE(entry,SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE_MSB,SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE_LSB)

#define SOC_SAND_ARR_MEM_ALLOCATOR_FREE_PREV_PTR(entry)   \
            SOC_SAND_GET_BITS_RANGE(entry,SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_PREV_PTR_MSB,SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_PREV_PTR_LSB)

#define SOC_SAND_ARR_MEM_ALLOCATOR_FREE_NEXT_PTR(entry)   \
  SOC_SAND_GET_BITS_RANGE(entry,SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_MSB,SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_LSB)


#define SOC_SAND_ARR_MEM_ALLOCATOR_GET_ALIGNED(addr,align)  \
        (addr == 0)? 0:((((addr) - 1)/(align) + 1) * (align))

#define SOC_SAND_ARR_MEM_ALLOCATOR_CACHE_INST(_inst) (SOC_SAND_BIT(31)|(_inst))

#define SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(_arr_mem_info, _inst) \
      (_arr_mem_info->arr_mem_allocator_data.cache_enabled)?SOC_SAND_ARR_MEM_ALLOCATOR_CACHE_INST(_inst):_inst
      

#define SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(_arr_mem_info)   (((_arr_mem_info)->support_defragment)?((_arr_mem_info)->nof_entries - 1 - (_arr_mem_info)->max_block_size):((_arr_mem_info)->nof_entries-1))






typedef struct
{
 
  uint8 use;
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR indx1;
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR indx2;
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR prev;
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR next;
  uint32 size;
} SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_INFO;
















STATIC uint32
  soc_sand_arr_mem_allocator_get_next_free(
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  uint8                down,
    SOC_SAND_INOUT  SOC_SAND_ARR_MEM_ALLOCATOR_PTR   *free_ptr
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    tmp_ptr_lsb,
    tmp_ptr_msb,
    tmp_ptr;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    entry;
  uint32
    block_size,
    next_ptr;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  tmp_ptr = *free_ptr;
  tmp_ptr_lsb = 0;
  tmp_ptr_msb = 0;

  res = arr_mem_info->entry_get_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          tmp_ptr,
          &entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  block_size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(entry);
  next_ptr = tmp_ptr + block_size - 1;

  if (down)
  {
    tmp_ptr_lsb = SOC_SAND_ARR_MEM_ALLOCATOR_FREE_NEXT_PTR(entry);
  }

  res = arr_mem_info->entry_get_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          next_ptr,
          &entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (down)
  {
    tmp_ptr_msb = SOC_SAND_ARR_MEM_ALLOCATOR_FREE_NEXT_PTR(entry);
  }
  else
  {
    *free_ptr = SOC_SAND_ARR_MEM_ALLOCATOR_FREE_PREV_PTR(entry);
    goto exit;
  }

  tmp_ptr = tmp_ptr_lsb;

  tmp_ptr |=
    SOC_SAND_SET_BITS_RANGE(
      tmp_ptr_msb,
      SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_SIZE * 2 - 1,
      SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_SIZE
    );

  *free_ptr = tmp_ptr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_get_next_free()",0,0);
}

STATIC uint32
  soc_sand_arr_mem_allocator_header_get(
    SOC_SAND_INOUT  SOC_SAND_ARR_MEM_ALLOCATOR_INFO        *arr_mem_info,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR            footer_ptr,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_PTR           *header_ptr
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    prev;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    entry;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(header_ptr);

  res = arr_mem_info->entry_get_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          footer_ptr,
          &entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  prev = SOC_SAND_ARR_MEM_ALLOCATOR_FREE_PREV_PTR(entry);

  if (prev == SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END)
  {
    res = arr_mem_info->free_get_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            header_ptr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
   res = soc_sand_arr_mem_allocator_get_next_free(
            arr_mem_info,
            TRUE,
            &prev
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    *header_ptr = prev;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_header_get()",0,0);
}


STATIC uint32
  soc_sand_arr_mem_allocator_set_next_free(
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR      block_ptr,
    SOC_SAND_IN  uint8                       down,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR      free_ptr
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    tmp_ptr;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    entry;
  uint32
    block_size,
    next_ptr;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  tmp_ptr = free_ptr;

  if (block_ptr == SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END && down)
  {
    res = arr_mem_info->free_set_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            free_ptr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    goto exit;
  }
  else if (block_ptr == SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END)
  {
	  goto exit;
  }

  res = arr_mem_info->entry_get_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          block_ptr,
          &entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  block_size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(entry);

  if (down)
  {
    res = soc_sand_set_field(
            &entry,
            SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_MSB,
            SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_LSB,
            tmp_ptr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arr_mem_info->entry_set_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          block_ptr,
          &entry
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  }
  next_ptr = block_ptr + block_size - 1;

  res = arr_mem_info->entry_get_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          next_ptr,
          &entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (down)
  {
    res = soc_sand_set_field(
            &entry,
            SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_MSB,
            SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_LSB,
            tmp_ptr >> SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_SIZE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    res = soc_sand_set_field(
            &entry,
            SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_PREV_PTR_MSB,
            SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_PREV_PTR_LSB,
            tmp_ptr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  res = arr_mem_info->entry_set_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          next_ptr,
          &entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_set_next_free()",0,0);
}


STATIC uint32
  soc_sand_arr_mem_allocator_remove_free_entry(
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_INOUT  SOC_SAND_ARR_MEM_ALLOCATOR_PTR   free_ptr
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    next,
    prev;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  next = free_ptr;
  res = soc_sand_arr_mem_allocator_get_next_free(
          arr_mem_info,
          TRUE,
          &next
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  prev = free_ptr;
  res = soc_sand_arr_mem_allocator_get_next_free(
          arr_mem_info,
          FALSE,
          &prev
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_arr_mem_allocator_set_next_free(
          arr_mem_info,
          next,
          FALSE,
          prev
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_arr_mem_allocator_set_next_free(
          arr_mem_info,
          prev,
          TRUE,
          next
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_remove_free_entry()",0,0);
}

STATIC uint32
  soc_sand_arr_mem_allocator_upadte_list(
    SOC_SAND_INOUT  SOC_SAND_ARR_MEM_ALLOCATOR_INFO        *arr_mem_info,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR            prev,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR            curr,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR            next
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);
 

  if (next != SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END)
  {
    res = soc_sand_arr_mem_allocator_set_next_free(
            arr_mem_info,
            next,
            FALSE,
            curr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
 
  if (prev != SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END)
  {
    res = soc_sand_arr_mem_allocator_set_next_free(
            arr_mem_info,
            prev,
            TRUE,
            curr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    res = arr_mem_info->free_set_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            curr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_upadte_list()",0,0);
}


STATIC uint32
  soc_sand_arr_mem_allocator_build_free_entry(
    SOC_SAND_IN  uint32                 size,
    SOC_SAND_IN   SOC_SAND_ARR_MEM_ALLOCATOR_PTR     next_ptr,
    SOC_SAND_IN   SOC_SAND_ARR_MEM_ALLOCATOR_PTR     prev_ptr,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY   entry[SOC_SAND_ARR_MEM_ALLOCATOR_FREE_ENTRY_LEN]
  )
{
  uint32
    free_ptr_part,
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(entry);


  for (indx = 0; indx < 2; ++indx)
  {
    if (indx == 0)
    {
      entry[indx] = size;
    }
    else
    {
      entry[indx] = prev_ptr;
    }

    free_ptr_part =
      SOC_SAND_GET_BITS_RANGE(
        next_ptr,
        (SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_SIZE * (indx + 1)) - 1,
        SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_SIZE * indx
      );
    entry[indx] |=
      SOC_SAND_SET_BITS_RANGE(
        free_ptr_part,
        SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_MSB,
        SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_NEXT_PTR_LSB
      );
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_build_free_entry()",0,0);
}

STATIC uint32
  soc_sand_arr_mem_allocator_build_used_entry(
    SOC_SAND_IN  uint32                 size,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY   *entry
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(entry);

  *entry = size;
  *entry |=
    SOC_SAND_SET_BITS_RANGE(
      TRUE,
      SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_USE_MSB,
      SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_USE_LSB
    );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_build_used_entry()",0,0);
}


STATIC
uint32
  soc_sand_arr_mem_allocator_malloc_entry(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR          prev_ptr,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR          free_block_ptr,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR          aligned_ptr,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR          next_ptr,
    SOC_SAND_IN  uint32                            alloced_size
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    new_prev,
    prev = prev_ptr,
    next = next_ptr,
    aligned = aligned_ptr,
    tmp_ptr = free_block_ptr,
    new_next;
  uint32
    block_size,
    tmp_size = alloced_size,
    align_free_size;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    free_entry[SOC_SAND_ARR_MEM_ALLOCATOR_FREE_ENTRY_LEN],
    align_free_entry[SOC_SAND_ARR_MEM_ALLOCATOR_FREE_ENTRY_LEN],
    used_entry;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_ARR_MEM_ALLOCATOR_MALLOC);
 
  res = arr_mem_info->entry_get_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          tmp_ptr,
          &(free_entry[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  block_size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(free_entry[0]);
  align_free_size = aligned - tmp_ptr;

 
  res = soc_sand_arr_mem_allocator_build_used_entry(
          tmp_size,
          &used_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = arr_mem_info->entry_set_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          aligned,
          &used_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = arr_mem_info->entry_set_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          aligned + tmp_size - 1,
          &used_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

 
 
  if (align_free_size != 0)
  {
   
    res = soc_sand_arr_mem_allocator_build_free_entry(
          align_free_size,
          next,
          prev,
          align_free_entry
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    res = arr_mem_info->entry_set_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            tmp_ptr,
            &(align_free_entry[0])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    res = arr_mem_info->entry_set_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            tmp_ptr + align_free_size - 1,
            &(align_free_entry[1])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    block_size -= align_free_size;
   
    prev = tmp_ptr;
   
    tmp_ptr = aligned;
  }
  block_size -= tmp_size;
 
  if (block_size > 0)
  {
    new_prev = tmp_ptr + tmp_size;
    new_next = tmp_ptr + tmp_size;
  }
  else
  {
    new_prev = prev;
    new_next = next;
  }
  if (prev == SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END)
  {
    res = arr_mem_info->free_set_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            new_next
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  }
  else
  {
    res = soc_sand_arr_mem_allocator_set_next_free(
            arr_mem_info,
            prev,
            TRUE,
            new_next
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  }

  if (next != SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END)
  {
    res = soc_sand_arr_mem_allocator_set_next_free(
            arr_mem_info,
            next,
            FALSE,
            new_prev
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  }

  if (block_size > 0)
  {
   
    if (align_free_size == 0)
    {
      res = soc_sand_set_field(
              &(free_entry[0]),
              SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE_MSB,
              SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE_LSB,
              block_size
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

      res = arr_mem_info->entry_set_fun(
              arr_mem_info->instance_prim_handle,
              SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
              tmp_ptr + tmp_size,
              &(free_entry[0])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
    }
    else 
    {
      
      res = soc_sand_arr_mem_allocator_build_free_entry(
              block_size,
              next,
              prev,
              free_entry
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

      res = arr_mem_info->entry_set_fun(
              arr_mem_info->instance_prim_handle,
              SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
              tmp_ptr + tmp_size,
              &(free_entry[0])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

      res = arr_mem_info->entry_set_fun(
              arr_mem_info->instance_prim_handle,
              SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
              tmp_ptr + tmp_size + block_size - 1,
              &(free_entry[1])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
    }
  }
  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_malloc()",0,0);
}


STATIC uint32
  soc_sand_arr_mem_allocator_move_block(
    SOC_SAND_INOUT  SOC_SAND_ARR_MEM_ALLOCATOR_INFO   *arr_mem_info,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR       from,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR       to,
    SOC_SAND_IN  uint32                         nof_entris,
    SOC_SAND_IN  uint32                         flags
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    next_free,
    prev_free;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    entry;
  uint32
    block_size=0,
    entry_index,
    from_indx,
    data[3],
    to_indx;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);


  
  if (flags & SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_BLK_SIZE_VLD)
  {
    block_size = nof_entris;
  }
  else
  {
    
    if (flags & (SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_UPDATE_REF|SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_UPDATE_CONT|SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_COPY))
    {
      res = arr_mem_info->entry_get_fun(
              arr_mem_info->instance_prim_handle,
              SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
              from,
              &entry
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      if (SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_IS_USED(entry))
      {
        block_size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(entry);
      }
      else 
      {
        block_size = 0;
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 40, exit);
      }
    }
 }
  if (flags & SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_COPY)
  {
    
    prev_free = to;
    res = soc_sand_arr_mem_allocator_get_next_free(
            arr_mem_info,
            FALSE,
            &prev_free
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    next_free = to;
    res = soc_sand_arr_mem_allocator_get_next_free(
            arr_mem_info,
            TRUE,
            &next_free
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    
    res = soc_sand_arr_mem_allocator_malloc_entry(arr_mem_info,prev_free,to,to,next_free,block_size);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  
  if (flags & SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_UPDATE_CONT)
  {
    from_indx = from;
    to_indx = to;

    for (entry_index = 0 ; entry_index < block_size; ++from_indx,++to_indx,++entry_index)
    {
      res = arr_mem_info->read_fun(
              arr_mem_info->instance_prim_handle,
              SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
              from_indx,
              data
            );
     SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      res = arr_mem_info->write_fun(
              arr_mem_info->instance_prim_handle,
              SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
              to_indx,
              data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      }
  }

  
  if (flags & SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_UPDATE_REF && arr_mem_info->entry_move_fun)
  {
    from_indx = from;
    to_indx = to;

    for (entry_index = 0 ; entry_index < block_size; ++from_indx,++to_indx,++entry_index)
    {
      
      res = arr_mem_info->read_fun(
              arr_mem_info->instance_prim_handle,
              SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
              to_indx,
              data
            );
     SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

     
      res = arr_mem_info->entry_move_fun(
              arr_mem_info->instance_prim_handle,
              SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
              data,
              from_indx,
              to_indx
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    }
  }
  
  if (flags & SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_FREE)
  {
    
    res = soc_sand_arr_mem_allocator_free(arr_mem_info,from);
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_move_block()",0,0);
}




STATIC uint32
  soc_sand_arr_mem_allocator_get_first_fit(
    SOC_SAND_INOUT  SOC_SAND_ARR_MEM_ALLOCATOR_INFO   *arr_mem_info,
    SOC_SAND_INOUT  uint32                      *size,
    SOC_SAND_IN  uint32                         alignment,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_PTR      *ptr,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_PTR      *aligned_ptr,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_PTR      *prev_ptr,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_PTR      *next_ptr
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    free_ptr,
    aligned_start;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    entry;
  uint32
    entry_size;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(ptr);

  *prev_ptr = SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END;
  *next_ptr = SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END;

  res = arr_mem_info->free_get_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          &free_ptr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  while (free_ptr != SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END)
  {
    res = arr_mem_info->entry_get_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            free_ptr,
            &entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    entry_size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(entry);
    if (entry_size >= *size)
    {
      aligned_start = SOC_SAND_ARR_MEM_ALLOCATOR_GET_ALIGNED(free_ptr,alignment);
     
      if (aligned_start - free_ptr == 1)
      {
        aligned_start += alignment;
      }
     
      if (entry_size >= *size + (aligned_start - free_ptr))
      {
        if (entry_size - (*size + (aligned_start - free_ptr)) == 1)
        {
          ++(*size);
        }
        *ptr = free_ptr;
        *next_ptr = *ptr;
        res = soc_sand_arr_mem_allocator_get_next_free(
                arr_mem_info,
                TRUE,
                next_ptr
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        *aligned_ptr = aligned_start;

        
        if (arr_mem_info->support_defragment)
        {
          if (*aligned_ptr + *size - 1 >= arr_mem_info->nof_entries - arr_mem_info->max_block_size)
          {
            *ptr = SOC_SAND_ARR_MEM_ALLOCATOR_NULL;
          }
        }
        
        goto exit;
      }
    }
    *prev_ptr = free_ptr;
    res = soc_sand_arr_mem_allocator_get_next_free(
            arr_mem_info,
            TRUE,
            &free_ptr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  *ptr = SOC_SAND_ARR_MEM_ALLOCATOR_NULL;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_get_first_fit()",0,0);
}



uint32
  soc_sand_arr_mem_allocator_create(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    entry[SOC_SAND_ARR_MEM_ALLOCATOR_FREE_ENTRY_LEN];
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_ARR_MEM_ALLOCATOR_CREATE);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info->entry_get_fun);
  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info->entry_set_fun);

  if (arr_mem_info->nof_entries < SOC_SAND_ARR_MEM_ALLOCATOR_MIN_MALLOC_SIZE || arr_mem_info->nof_entries - 1 > SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ARR_MEM_ALLOCATOR_NOF_LINES_OUT_OF_RANGE_ERR, 10, exit);
  }
 
  arr_mem_info->arr_mem_allocator_data.array = 
      (SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY*) soc_sand_os_malloc_any_size(sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY) * arr_mem_info->nof_entries, "arr_mem_info->arr_mem_allocator_data.array");
  if (!arr_mem_info->arr_mem_allocator_data.array)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 20, exit);
  }
 
  soc_sand_os_memset(arr_mem_info->arr_mem_allocator_data.array,0x0,sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY) * arr_mem_info->nof_entries);

 
  arr_mem_info->arr_mem_allocator_data.mem_shadow = (uint32*) soc_sand_os_malloc_any_size(sizeof(uint32) * arr_mem_info->nof_entries, "arr_mem_info->arr_mem_allocator_data.mem_shadow");
  if (!arr_mem_info->arr_mem_allocator_data.mem_shadow)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 20, exit);
  }
  soc_sand_os_memset(arr_mem_info->arr_mem_allocator_data.mem_shadow,0x0,sizeof(uint32) * arr_mem_info->nof_entries);



  if (arr_mem_info->support_caching)
  {
   
    arr_mem_info->arr_mem_allocator_data.array_cache = 
        (SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY*) soc_sand_os_malloc_any_size(sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY) * arr_mem_info->nof_entries, "arr_mem_info->arr_mem_allocator_data.array_cache");
    if (!arr_mem_info->arr_mem_allocator_data.array_cache)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 20, exit);
    }
   
    soc_sand_os_memset(arr_mem_info->arr_mem_allocator_data.array_cache,0x0,sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY) * arr_mem_info->nof_entries);

    
    arr_mem_info->arr_mem_allocator_data.update_indexes = 
        (SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY*) soc_sand_os_malloc_any_size(sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY) * arr_mem_info->nof_entries, "arr_mem_info->arr_mem_allocator_data.update_indexes");
    if (!arr_mem_info->arr_mem_allocator_data.update_indexes)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 20, exit);
    }
    arr_mem_info->arr_mem_allocator_data.nof_updates = 0;
   
    arr_mem_info->arr_mem_allocator_data.mem_shadow_cache = (uint32*) soc_sand_os_malloc_any_size(sizeof(uint32) * arr_mem_info->nof_entries, "mem_shadow_cache");
    if (!arr_mem_info->arr_mem_allocator_data.mem_shadow_cache)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 20, exit);
    }
    soc_sand_os_memset(arr_mem_info->arr_mem_allocator_data.mem_shadow_cache,0x0,sizeof(uint32) * arr_mem_info->nof_entries);
  }

  res = soc_sand_arr_mem_allocator_build_free_entry(
          SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info)+1,
          SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END,
          SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END,
          entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
 
  res = arr_mem_info->entry_set_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          0,
          &(entry[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = arr_mem_info->entry_set_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info),
          &(entry[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = arr_mem_info->free_set_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_create()",0,0);
}

uint32
  soc_sand_arr_mem_allocator_clear(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    entry[SOC_SAND_ARR_MEM_ALLOCATOR_FREE_ENTRY_LEN];
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_ARR_MEM_ALLOCATOR_CLEAR);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info->entry_get_fun);
  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info->entry_set_fun);

  if (arr_mem_info->nof_entries < SOC_SAND_ARR_MEM_ALLOCATOR_MIN_MALLOC_SIZE || arr_mem_info->nof_entries - 1 > SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ARR_MEM_ALLOCATOR_NOF_LINES_OUT_OF_RANGE_ERR, 10, exit);
  }
 
  if (arr_mem_info->support_caching && arr_mem_info->arr_mem_allocator_data.cache_enabled)
  {
    soc_sand_os_memset(arr_mem_info->arr_mem_allocator_data.array_cache,0x0,sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY) * arr_mem_info->nof_entries);
    soc_sand_os_memset(arr_mem_info->arr_mem_allocator_data.mem_shadow_cache,0x0,sizeof(uint32) * arr_mem_info->nof_entries);
    arr_mem_info->arr_mem_allocator_data.nof_updates = 0;
  }
  else
  {
    soc_sand_os_memset(arr_mem_info->arr_mem_allocator_data.array,0x0,sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY) * arr_mem_info->nof_entries);
    soc_sand_os_memset(arr_mem_info->arr_mem_allocator_data.mem_shadow,0x0,sizeof(uint32) * arr_mem_info->nof_entries);
  }
 
  res = soc_sand_arr_mem_allocator_build_free_entry(
          SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info)+1,
          SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END,
          SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END,
          entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
 
  res = arr_mem_info->entry_set_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          0,
          &(entry[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arr_mem_info->entry_set_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info),
          &(entry[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = arr_mem_info->free_set_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_clear()",0,0);
}

uint32
  soc_sand_arr_mem_allocator_destroy(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_ARR_MEM_ALLOCATOR_DESTROY);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);

  

  res = soc_sand_os_free_any_size(
    arr_mem_info->arr_mem_allocator_data.array
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_os_free_any_size(
    arr_mem_info->arr_mem_allocator_data.mem_shadow
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (arr_mem_info->support_caching)
  {
    res = soc_sand_os_free_any_size(
      arr_mem_info->arr_mem_allocator_data.array_cache
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    res = soc_sand_os_free_any_size(
      arr_mem_info->arr_mem_allocator_data.update_indexes
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      res = soc_sand_os_free_any_size(
        arr_mem_info->arr_mem_allocator_data.mem_shadow_cache
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_destroy()",0,0);
}

uint32
  soc_sand_arr_mem_allocator_cache_set(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_IN uint8                             enable
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_ARR_MEM_ALLOCATOR_MALLOC);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);
  if (!arr_mem_info->support_caching && enable)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }

  
  if (arr_mem_info->arr_mem_allocator_data.cache_enabled && enable)
  {
    goto exit;
  }
  
  arr_mem_info->arr_mem_allocator_data.cache_enabled = enable;

 
  if (enable)
  {
    arr_mem_info->arr_mem_allocator_data.free_list_cache = arr_mem_info->arr_mem_allocator_data.free_list;
    soc_sand_os_memcpy(arr_mem_info->arr_mem_allocator_data.array_cache,arr_mem_info->arr_mem_allocator_data.array,sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY) * arr_mem_info->nof_entries);
    soc_sand_os_memcpy(arr_mem_info->arr_mem_allocator_data.mem_shadow_cache,arr_mem_info->arr_mem_allocator_data.mem_shadow,sizeof(uint32) * arr_mem_info->nof_entries);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_cache_set()",0,0);
}


uint32
  soc_sand_arr_mem_allocator_commit(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_IN uint32                             flags
  )
{
  uint32
    indx,
    nof_updates,
    offset,
    data[3];
  uint8 
    cache_org=FALSE;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_ARR_MEM_ALLOCATOR_MALLOC);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);


  if (!arr_mem_info->support_caching)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }
  
  if (!arr_mem_info->arr_mem_allocator_data.cache_enabled)
  {
    goto exit;
  }

  cache_org = arr_mem_info->arr_mem_allocator_data.cache_enabled;

 
  arr_mem_info->arr_mem_allocator_data.cache_enabled = FALSE;

  nof_updates = arr_mem_info->arr_mem_allocator_data.nof_updates;
  
  for (indx = 0; indx < nof_updates; ++indx)
  {
    offset = arr_mem_info->arr_mem_allocator_data.update_indexes[indx];

    res = arr_mem_info->read_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_CACHE_INST(arr_mem_info->instance_sec_handle),
            offset,
            data
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = arr_mem_info->write_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            offset,
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  
  arr_mem_info->arr_mem_allocator_data.nof_updates = 0; 
  
 
  arr_mem_info->arr_mem_allocator_data.free_list = arr_mem_info->arr_mem_allocator_data.free_list_cache;
  soc_sand_os_memcpy(arr_mem_info->arr_mem_allocator_data.array,arr_mem_info->arr_mem_allocator_data.array_cache,sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY) * arr_mem_info->nof_entries);
  soc_sand_os_memcpy(arr_mem_info->arr_mem_allocator_data.mem_shadow,arr_mem_info->arr_mem_allocator_data.mem_shadow_cache,sizeof(uint32) * arr_mem_info->nof_entries);
  
exit:
  arr_mem_info->arr_mem_allocator_data.cache_enabled = cache_org;
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_commit()",0,0);
}


uint32
  soc_sand_arr_mem_allocator_rollback(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_IN uint32                             flags
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_ARR_MEM_ALLOCATOR_MALLOC);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);
  if (!arr_mem_info->support_caching)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }
  
  if (!arr_mem_info->arr_mem_allocator_data.cache_enabled)
  {
    goto exit;
  }
  arr_mem_info->arr_mem_allocator_data.nof_updates = 0;

  arr_mem_info->arr_mem_allocator_data.free_list_cache = arr_mem_info->arr_mem_allocator_data.free_list;
  soc_sand_os_memcpy(arr_mem_info->arr_mem_allocator_data.array_cache,arr_mem_info->arr_mem_allocator_data.array,sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY) * arr_mem_info->nof_entries);
  soc_sand_os_memcpy(arr_mem_info->arr_mem_allocator_data.mem_shadow_cache,arr_mem_info->arr_mem_allocator_data.mem_shadow,sizeof(uint32) * arr_mem_info->nof_entries);
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_rollback()",0,0);
}


uint32
  soc_sand_arr_mem_allocator_malloc(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  uint32                            size,
    SOC_SAND_IN  uint32                            alignment,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_PTR         *ptr
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    prev,
    aligned = 0,
    tmp_ptr = 0,
    next;
  uint32
    tmp_size = size,
    act_alignment;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_ARR_MEM_ALLOCATOR_MALLOC);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(ptr);

  if(tmp_size < SOC_SAND_ARR_MEM_ALLOCATOR_MIN_MALLOC_SIZE)
  {
    tmp_size = SOC_SAND_ARR_MEM_ALLOCATOR_MIN_MALLOC_SIZE;
  }

  if (tmp_size > arr_mem_info->nof_entries)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ARR_MEM_ALLOCATOR_MALLOC_SIZE_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (arr_mem_info->support_defragment && tmp_size > arr_mem_info->max_block_size )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ARR_MEM_ALLOCATOR_MALLOC_SIZE_OUT_OF_RANGE_ERR, 15, exit);
  }
  
  act_alignment = (alignment > 0)? alignment: 1;

  if (tmp_size % 2)
  {
	  tmp_size+=1;
  }
  
 
  res = soc_sand_arr_mem_allocator_get_first_fit(
          arr_mem_info,
          &tmp_size,
          act_alignment,
          &tmp_ptr,
          &aligned,
          &prev,
          &next
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  if (tmp_ptr == SOC_SAND_ARR_MEM_ALLOCATOR_NULL)
  {
    *ptr = SOC_SAND_ARR_MEM_ALLOCATOR_NULL;
    goto exit;
  }
 
  res = soc_sand_arr_mem_allocator_malloc_entry(arr_mem_info,prev,tmp_ptr,aligned,next,tmp_size);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *ptr = aligned;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_malloc()",0,0);
}

uint32
  soc_sand_arr_mem_allocator_free(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR          ptr
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    prev,
    next;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    free_entries[SOC_SAND_ARR_MEM_ALLOCATOR_FREE_ENTRY_LEN];
  uint32
    before_size,
    after_size,
    used_block_size;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    used_entry,
    prev_entry,
    next_entry;
  uint8
    after_free,
    before_free;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_ARR_MEM_ALLOCATOR_FREE);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);

  if (ptr > SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ARR_MEM_ALLOCATOR_POINTER_OF_RANGE_ERR, 10, exit);
  }


  res = arr_mem_info->entry_get_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          ptr,
          &used_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_IS_USED(used_entry))
  {
    
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ARR_MEM_ALLOCATOR_POINTER_OF_RANGE_ERR, 220, exit);
  }

  used_block_size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(used_entry);

  before_free = FALSE;
  after_free = FALSE;
  prev_entry = 0;
  next_entry = 0;
 

 
  if (ptr > 0)
  {
    res = arr_mem_info->entry_get_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            ptr - 1,
            &prev_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
   
    before_free = (uint8)!(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_IS_USED(prev_entry));
  }
  if (ptr + used_block_size <  SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info)+1)
  {
    res = arr_mem_info->entry_get_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            ptr + used_block_size,
            &next_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
   
    after_free = (uint8)!(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_IS_USED(next_entry));
  }

  after_size = 0;
  before_size = 0;
  prev = 0;
  next = 0;

  if (!after_free && !before_free)
  {
    prev = SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END;
    res = arr_mem_info->free_get_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            &next
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
  else if (after_free && before_free)
  {
    res = soc_sand_arr_mem_allocator_header_get(
              arr_mem_info,
              ptr - 1,
              &prev
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    res = arr_mem_info->entry_get_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            prev,
            &prev_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    before_size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(prev_entry);

    next = ptr + used_block_size;
    res = arr_mem_info->entry_get_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            next,
            &next_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    after_size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(next_entry);
   
    res = soc_sand_arr_mem_allocator_remove_free_entry(
            arr_mem_info,
            prev
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    prev = ptr + used_block_size;
    res = soc_sand_arr_mem_allocator_get_next_free(
            arr_mem_info,
            FALSE,
            &prev
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    next = ptr + used_block_size;
    res = soc_sand_arr_mem_allocator_get_next_free(
            arr_mem_info,
            TRUE,
            &next
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  }
  else
  {
    if (before_free)
    {
      res = soc_sand_arr_mem_allocator_header_get(
              arr_mem_info,
              ptr - 1,
              &prev
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

      next = prev;
      res = soc_sand_arr_mem_allocator_get_next_free(
              arr_mem_info,
              TRUE,
              &next
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
    }
    if (after_free)
    {
      next = ptr + used_block_size;
      prev = next;
      res = soc_sand_arr_mem_allocator_get_next_free(
              arr_mem_info,
              FALSE,
              &prev
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
    }
   
    if (after_free)
    {
      res = arr_mem_info->entry_get_fun(
              arr_mem_info->instance_prim_handle,
              SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
              next,
              &next_entry
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
      after_size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(next_entry);

      res = soc_sand_arr_mem_allocator_get_next_free(
                arr_mem_info,
                TRUE,
                &next
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
    }
   
    if (before_free)
    {
      res = arr_mem_info->entry_get_fun(
              arr_mem_info->instance_prim_handle,
              SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
              prev,
              &prev_entry
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

      before_size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(prev_entry);

      res = soc_sand_arr_mem_allocator_get_next_free(
              arr_mem_info,
              FALSE,
              &prev
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
    }
  }
 
  res = soc_sand_arr_mem_allocator_build_free_entry(
          before_size + used_block_size + after_size,
          next,
          prev,
          free_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

  res = arr_mem_info->entry_set_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          ptr - before_size,
          &(free_entries[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

  res = arr_mem_info->entry_set_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          ptr + after_size + used_block_size - 1,
          &(free_entries[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

  res = soc_sand_arr_mem_allocator_upadte_list(
          arr_mem_info,
          prev,
          ptr - before_size,
          next
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_free()",0,0);
}

uint32
  soc_sand_arr_mem_allocator_block_size(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR          ptr,
    SOC_SAND_OUT  uint32                           *size
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    used_entry;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_ARR_MEM_ALLOCATOR_FREE);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);

  if (ptr> SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ARR_MEM_ALLOCATOR_POINTER_OF_RANGE_ERR, 10, exit);
  }

  res = arr_mem_info->entry_get_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          ptr,
          &used_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(used_entry);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_array_set()",0,0);
}


uint32
  soc_sand_arr_mem_allocator_write(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR          offset,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY        *data
  )
{

  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_ARR_MEM_ALLOCATOR_WRITE);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);

  if (offset > SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ARR_MEM_ALLOCATOR_POINTER_OF_RANGE_ERR, 10, exit);
  }

  res = arr_mem_info->write_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          offset,
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
 
  if (arr_mem_info->arr_mem_allocator_data.cache_enabled)
  {
    arr_mem_info->arr_mem_allocator_data.update_indexes[arr_mem_info->arr_mem_allocator_data.nof_updates] = offset;
    ++arr_mem_info->arr_mem_allocator_data.nof_updates;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_write()",0,0);
}




uint32
  soc_sand_arr_mem_allocator_read(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_PTR          offset,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY        *data
  )
{

  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_ARR_MEM_ALLOCATOR_READ);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);

  if (offset> SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ARR_MEM_ALLOCATOR_POINTER_OF_RANGE_ERR, 10, exit);
  }

  res = arr_mem_info->read_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          offset,
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_read()",0,0);
}

STATIC uint32
  soc_sand_arr_mem_allocator_get_next_used_block(
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_INOUT  SOC_SAND_ARR_MEM_ALLOCATOR_ITER  *iter,
    SOC_SAND_IN  uint32                        pos
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    indx;
  uint32
    block_size,
    cur_pos=pos;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    entry;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  indx = iter->offset;
  block_size = 0;

  while (block_size == 0 && indx <= SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info))
  {
    res = arr_mem_info->entry_get_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            indx,
            &entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_IS_USED(entry))
    {
      if (!cur_pos)
      {
        block_size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(entry);
      }
      else 
      {
        --cur_pos;
        indx += SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(entry);
      } 

    }
    else 
    {
      indx += SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(entry);
    }
  }

  iter->block_size = block_size;
  if (indx > SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info))
  {
    iter->offset = SOC_SAND_ARR_MEM_ALLOCATOR_NULL;
  }
  else
  {
    iter->offset = indx;
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_get_next_free()",0,0);
}

STATIC uint32
  soc_sand_arr_mem_allocator_get_next_free_in_order(
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
    SOC_SAND_INOUT  SOC_SAND_ARR_MEM_ALLOCATOR_ITER  *iter
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    indx;
  uint32
    block_size;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    entry;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  indx = iter->offset;
  block_size = 1;

  while (indx <= SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info))
  {
    res = arr_mem_info->entry_get_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            indx,
            &entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_IS_USED(entry))
    {
      indx += SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(entry);
    }
    else 
    {
      block_size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(entry);
      break;
    }
  }

  iter->block_size = block_size;
  if (indx > SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info))
  {
    iter->offset = SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END;
  }
  else
  {
    iter->offset = indx;
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_get_next_free()",0,0);
}





uint32
  soc_sand_arr_mem_allocator_read_block(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_INOUT  SOC_SAND_ARR_MEM_ALLOCATOR_ITER       *iter,
    SOC_SAND_IN  uint32                             entries_to_read,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY        *data,
    SOC_SAND_OUT  SOC_SAND_ARR_MEM_ALLOCATOR_PTR          *addresses,
    SOC_SAND_OUT  uint32                            *nof_entries
  )
{
  uint32
    readen;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_ARR_MEM_ALLOCATOR_READ_BLOCK);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(iter);
  SOC_SAND_CHECK_NULL_INPUT(data);
  SOC_SAND_CHECK_NULL_INPUT(addresses);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  readen = 0;

  if (iter->offset > SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ARR_MEM_ALLOCATOR_POINTER_OF_RANGE_ERR, 10, exit);
  }

  for (; iter->offset <= SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info) && readen < entries_to_read; ++iter->offset )
  {
    if (iter->block_size == 0)
    {
      res = soc_sand_arr_mem_allocator_get_next_used_block(
              arr_mem_info,
              iter,
              0
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit)
    }
    if(SOC_SAND_ARR_MEM_ALLOCATOR_ITER_END(iter))
    {
      break;
    }
    res = arr_mem_info->read_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            iter->offset,
            &(data[readen])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    addresses[readen] = iter->offset;
    ++readen;
    --(iter->block_size);
  }
  *nof_entries = readen;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_read_block()",0,0);
}



uint32
  soc_sand_arr_mem_allocator_defrag(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO       *arr_mem_info,
    SOC_SAND_INOUT SOC_SAND_ARR_MEM_ALLOCATOR_DEFRAG_INFO  *defrag_info
  )
{
  uint32
    nof_defrags=0,
    nof_moves = 0,
    moved_to_end = 0,
    max_block_size=0;
  SOC_SAND_ARR_MEM_ALLOCATOR_ITER  
    cur_used,
    first_free_iter;
  uint8
    done = FALSE;
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    free_ptr,
    moved_to_ptr,
    move_from_ptr,
    reserved_mem_ptr;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    entry;
  uint32
    indx,
    prev_used;
  SOC_SAND_TABLE_BLOCK_RANGE             
    *block_range;
  uint32
    entry_size,
    pos=0,
    flags;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(defrag_info);


  max_block_size = arr_mem_info->max_block_size;
  block_range = &(defrag_info->block_range);

  if (!arr_mem_info->support_defragment)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }

  
  if (SOC_SAND_TBL_ITER_IS_END(&block_range->iter))
  {
    goto exit;
  }

  first_free_iter.offset = 0;
  res = soc_sand_arr_mem_allocator_get_next_free_in_order(arr_mem_info,&first_free_iter);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  free_ptr = first_free_iter.offset;

  if (free_ptr == SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END)
  {
    
    SOC_SAND_TBL_ITER_SET_END(&block_range->iter);
    goto exit;
  }
  
  
  entry_size = first_free_iter.block_size;

  
  reserved_mem_ptr = arr_mem_info->nof_entries - max_block_size;;
  moved_to_ptr = reserved_mem_ptr;

  flags = SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_UPDATE_CONT|SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_UPDATE_REF|
           SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_FREE|SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_BLK_SIZE_VLD;

  cur_used.offset = free_ptr + entry_size;
  cur_used.block_size = 0;
  pos = 0;
  
  while(entry_size < max_block_size/2) 
  {
    
    res = soc_sand_arr_mem_allocator_get_next_used_block(arr_mem_info,&cur_used,pos);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    if (cur_used.offset - free_ptr >= max_block_size/2 || cur_used.offset == SOC_SAND_ARR_MEM_ALLOCATOR_NULL)
    {
      break;
    }
   
   
    res = soc_sand_arr_mem_allocator_move_block(
            arr_mem_info,
            cur_used.offset, 
            moved_to_ptr, 
            cur_used.block_size,
            flags
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    
      entry = cur_used.block_size;
      res = arr_mem_info->entry_set_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            reserved_mem_ptr + moved_to_end,
            &entry
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

    ++moved_to_end;
    moved_to_ptr += cur_used.block_size;
    
    cur_used.offset += cur_used.block_size;
    if (cur_used.offset - free_ptr >= max_block_size/2 || cur_used.offset == SOC_SAND_ARR_MEM_ALLOCATOR_NULL)
    {
      break;
    }
  }

  flags = SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_FREE|SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_COPY|
          SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_UPDATE_CONT|SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_UPDATE_REF;
  
  prev_used = cur_used.offset;
  while (!done && nof_defrags < block_range->entries_to_act && nof_moves < block_range->entries_to_scan)
  {
    
    res = soc_sand_arr_mem_allocator_get_next_used_block(arr_mem_info,&cur_used,0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (cur_used.offset == SOC_SAND_ARR_MEM_ALLOCATOR_NULL)
    {
      done = TRUE;
      break;
    }

    res = soc_sand_arr_mem_allocator_move_block(arr_mem_info,cur_used.offset,free_ptr,0,flags);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    nof_moves+=cur_used.block_size;
    if (prev_used != cur_used.offset)
    {
      ++nof_defrags;
    }
    
    
    cur_used.offset += cur_used.block_size;
    if (cur_used.offset  > SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info))
    {
      done = TRUE;
      break;
    }
    prev_used = cur_used.offset;

    
    first_free_iter.offset += cur_used.block_size;
    res = soc_sand_arr_mem_allocator_get_next_free_in_order(arr_mem_info,&first_free_iter);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    free_ptr = first_free_iter.offset;
  }
  
  
  flags = SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_COPY|SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_UPDATE_CONT|
    SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_UPDATE_REF|SOC_SAND_ARR_MEM_ALLOCATOR_MOVE_BLK_SIZE_VLD;

  move_from_ptr = reserved_mem_ptr;
  moved_to_ptr = free_ptr;
  for (indx = 0; indx < moved_to_end; ++indx)
  {
    
    res = arr_mem_info->entry_get_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            reserved_mem_ptr + indx,
            &entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

    res = soc_sand_arr_mem_allocator_move_block(arr_mem_info,move_from_ptr,moved_to_ptr,entry,flags);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    move_from_ptr += entry;
    moved_to_ptr += entry;
  }
  
  if (done)
  {
    SOC_SAND_TBL_ITER_SET_END(&defrag_info->block_range.iter);
  }
  else
  {
    defrag_info->block_range.iter = moved_to_ptr;
  }
  


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_read_block()",0,0);
}


uint32
  soc_sand_arr_mem_allocator_print_free(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    free_ptr;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    entry;
  uint32
    entry_size;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);

  res = arr_mem_info->free_get_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          &free_ptr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  LOG_CLI((BSL_META_U(unit,
                      "free blocks:\n")));

  while (free_ptr != SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END)
  {
    res = arr_mem_info->entry_get_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            free_ptr,
            &entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    entry_size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(entry);
    LOG_CLI((BSL_META_U(unit,
                        "start:%u   end:%u \n"), free_ptr, free_ptr + entry_size));

    res = soc_sand_arr_mem_allocator_get_next_free(
            arr_mem_info,
            TRUE,
            &free_ptr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_print_free()",0,0);
}



uint32
  soc_sand_arr_mem_allocator_print_free_by_order(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_ITER  iter;
  uint32
    entry_size;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);

  iter.offset = 0;

  res = soc_sand_arr_mem_allocator_get_next_free_in_order(
          arr_mem_info,
          &iter
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  LOG_CLI((BSL_META_U(unit,
                      "free blocks by order:\n")));

  while (iter.offset != SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END)
  {
    entry_size = iter.block_size;
    LOG_CLI((BSL_META_U(unit,
                        "start:%u   end:%u \n"), iter.offset, iter.offset + entry_size));

    iter.offset += entry_size;
    res = soc_sand_arr_mem_allocator_get_next_free_in_order(
            arr_mem_info,
            &iter
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_print_free_by_order()",0,0);
}


uint32
  soc_sand_arr_mem_allocator_is_availabe_blocks(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
	  SOC_SAND_IN   SOC_SAND_ARR_MEM_ALLOCATOR_REQ_BLOCKS  *req_blocks,
	  SOC_SAND_OUT   uint8                         *available
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    free_ptr;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    entry;
  uint32
    block_indx = 0;
  uint32
    entry_size;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);

  res = arr_mem_info->free_get_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          &free_ptr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  while (free_ptr != SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END)
  {
    res = arr_mem_info->entry_get_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            free_ptr,
            &entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    entry_size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(entry);

	while(entry_size >= req_blocks->block_size[block_indx])
	{
		entry_size -= req_blocks->block_size[block_indx];
		block_indx++;
		if (block_indx >= req_blocks->nof_reqs)
		{
			*available = TRUE;
			goto exit;
		}
	}
    res = soc_sand_arr_mem_allocator_get_next_free(
            arr_mem_info,
            TRUE,
            &free_ptr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  *available = FALSE; 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_is_availabe_blocks()",0,0);
}


uint32
  soc_sand_arr_mem_allocator_mem_status_get(
    SOC_SAND_INOUT   SOC_SAND_ARR_MEM_ALLOCATOR_INFO     *arr_mem_info,
	  SOC_SAND_OUT   SOC_SAND_ARR_MEM_ALLOCATOR_MEM_STATUS  *mem_status
  )
{
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    free_ptr;
  SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
    entry;
  uint32
	  total_free = 0,
	  max_free_size = 0;
  uint32
    entry_size;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);

  soc_sand_SAND_ARR_MEM_ALLOCATOR_MEM_STATUS_clear(mem_status);

  res = arr_mem_info->free_get_fun(
          arr_mem_info->instance_prim_handle,
          SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
          &free_ptr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  while (free_ptr != SOC_SAND_ARR_MEM_ALLOCATOR_LIST_END)
  {
    res = arr_mem_info->entry_get_fun(
            arr_mem_info->instance_prim_handle,
            SOC_SAND_ARR_MEM_ALLOCATOR_ACTIVE_INST(arr_mem_info,arr_mem_info->instance_sec_handle),
            free_ptr,
            &entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    entry_size = SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SIZE(entry);
    total_free += entry_size;
	  if(entry_size > max_free_size)
	  {
		  max_free_size = entry_size;
	  }
    ++mem_status->nof_fragments;

    res = soc_sand_arr_mem_allocator_get_next_free(
            arr_mem_info,
            TRUE,
            &free_ptr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  mem_status->max_free_block_size = max_free_size;
  mem_status->total_free = total_free;
  mem_status->total_size = SOC_SAND_ARR_MEM_ALLOCATOR_LAST_INDEX(arr_mem_info) + 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_mem_status_get()",0,0);
}


uint32
  soc_sand_arr_mem_allocator_get_size_for_save(
    SOC_SAND_IN   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_IN  uint32                        flags,
    SOC_SAND_OUT  uint32                       *size
  )
{
  const SOC_SAND_ARR_MEM_ALLOCATOR_T
    *mem_data;
  uint32
    cur_size,
    total_size=0;


  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(size);


  
  mem_data = &(arr_mem_info->arr_mem_allocator_data);

  cur_size = sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_INFO);
  total_size += cur_size;

  
  cur_size = sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY) * arr_mem_info->nof_entries;
  total_size += cur_size;
  
  cur_size = sizeof(uint32) * arr_mem_info->nof_entries;
  total_size += cur_size;

  
  if (mem_data->cache_enabled)
  {
    
    cur_size = sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY) * arr_mem_info->nof_entries;
    total_size += cur_size;
    
    cur_size = sizeof(uint32) * arr_mem_info->nof_entries;
    total_size += cur_size;
    
    cur_size = sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY) * arr_mem_info->nof_entries;
    total_size += cur_size;
  }

  *size= total_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_get_size_for_save()",0,0);
}



uint32
  soc_sand_arr_mem_allocator_save(
    SOC_SAND_IN   SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info,
    SOC_SAND_IN  uint32                flags,
    SOC_SAND_OUT uint8                 *buffer,
    SOC_SAND_IN  uint32                buffer_size_bytes,
    SOC_SAND_OUT uint32                *actual_size_bytes
  )
{
  uint8
    *cur_ptr = (uint8*)buffer;
  uint32
    cur_size,
    total_size=0;
  uint32
    res;
  SOC_SAND_ARR_MEM_ALLOCATOR_INFO
    *info;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(buffer);
  SOC_SAND_CHECK_NULL_INPUT(actual_size_bytes);

  
  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,arr_mem_info,SOC_SAND_ARR_MEM_ALLOCATOR_INFO,1);

  
  info = (SOC_SAND_ARR_MEM_ALLOCATOR_INFO *) (cur_ptr - sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_INFO));
  soc_sand_os_memset(&info->arr_mem_allocator_data.array, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY *));
  soc_sand_os_memset(&info->arr_mem_allocator_data.mem_shadow, 0x0, sizeof(uint32 *));
  soc_sand_os_memset(&info->arr_mem_allocator_data.array_cache, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY *));
  soc_sand_os_memset(&info->arr_mem_allocator_data.mem_shadow_cache, 0x0, sizeof(uint32 *));
  soc_sand_os_memset(&info->arr_mem_allocator_data.update_indexes, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY *));  
  soc_sand_os_memset(&info->entry_set_fun, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_SET));
  soc_sand_os_memset(&info->entry_get_fun, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_GET));
  soc_sand_os_memset(&info->free_set_fun, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_FREE_LIST_SET));
  soc_sand_os_memset(&info->free_get_fun, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_FREE_LIST_GET));
  soc_sand_os_memset(&info->read_fun, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ROW_READ));
  soc_sand_os_memset(&info->write_fun, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ROW_WRITE));
  soc_sand_os_memset(&info->entry_move_fun, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY_MOVE_CALL_BACK));

  
  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,arr_mem_info->arr_mem_allocator_data.array,SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY,arr_mem_info->nof_entries);

  
  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,arr_mem_info->arr_mem_allocator_data.mem_shadow,uint32,arr_mem_info->nof_entries);

  if (arr_mem_info->arr_mem_allocator_data.cache_enabled)
  {
    
    SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,arr_mem_info->arr_mem_allocator_data.array_cache,SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY,arr_mem_info->nof_entries);
    
    SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,arr_mem_info->arr_mem_allocator_data.mem_shadow_cache,uint32,arr_mem_info->nof_entries);
    
    SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,arr_mem_info->arr_mem_allocator_data.update_indexes,SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY,arr_mem_info->nof_entries);
  }

  *actual_size_bytes = total_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_save()",0,0);
}

uint32
  soc_sand_arr_mem_allocator_load(
    SOC_SAND_IN  uint8                           **buffer,
    SOC_SAND_IN  SOC_SAND_ARR_MEM_ALLOCATOR_LOAD_INFO  *load_info,
    SOC_SAND_OUT SOC_SAND_ARR_MEM_ALLOCATOR_INFO      *arr_mem_info
  )
{
  SOC_SAND_IN uint8
    *cur_ptr = (SOC_SAND_IN uint8*)buffer[0];
  uint32
    res;
  SOC_SAND_ARR_MEM_ALLOCATOR_PTR
    free_list_holder;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(arr_mem_info);
  SOC_SAND_CHECK_NULL_INPUT(load_info);
  SOC_SAND_CHECK_NULL_INPUT(buffer);

  
  soc_sand_os_memcpy(arr_mem_info, cur_ptr, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_INFO));
  cur_ptr += sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_INFO);

  
  free_list_holder = arr_mem_info->arr_mem_allocator_data.free_list;
  
  
  arr_mem_info->entry_set_fun = load_info->entry_set_fun; 
  arr_mem_info->entry_get_fun = load_info->entry_get_fun; 
  arr_mem_info->free_set_fun = load_info->free_set_fun;
  arr_mem_info->free_get_fun = load_info->free_get_fun;
  arr_mem_info->read_fun = load_info->read_fun;
  arr_mem_info->write_fun = load_info->write_fun; 
  arr_mem_info->entry_move_fun = load_info->entry_move_fun; 

  
  res = soc_sand_arr_mem_allocator_create(arr_mem_info);
  SOC_SAND_CHECK_FUNC_RESULT(res,20, exit);

  
  arr_mem_info->arr_mem_allocator_data.free_list = free_list_holder;

  
  soc_sand_os_memcpy(arr_mem_info->arr_mem_allocator_data.array, cur_ptr, arr_mem_info->nof_entries * sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY));
  cur_ptr += arr_mem_info->nof_entries * sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY);

  
  soc_sand_os_memcpy(arr_mem_info->arr_mem_allocator_data.mem_shadow, cur_ptr, arr_mem_info->nof_entries * sizeof(uint32));
  cur_ptr += arr_mem_info->nof_entries * sizeof(uint32);

  if (arr_mem_info->arr_mem_allocator_data.cache_enabled)
  {
    
    soc_sand_os_memcpy(arr_mem_info->arr_mem_allocator_data.array_cache, cur_ptr, arr_mem_info->nof_entries * sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY));
    cur_ptr += arr_mem_info->nof_entries * sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY);
    
    soc_sand_os_memcpy(arr_mem_info->arr_mem_allocator_data.mem_shadow_cache, cur_ptr, arr_mem_info->nof_entries * sizeof(uint32));
    cur_ptr += arr_mem_info->nof_entries * sizeof(uint32);
    
    soc_sand_os_memcpy(arr_mem_info->arr_mem_allocator_data.update_indexes, cur_ptr, arr_mem_info->nof_entries * sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY));
    cur_ptr += arr_mem_info->nof_entries * sizeof(uint32);
  }
  *buffer = cur_ptr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_arr_mem_allocator_load()",0,0);
}



void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_INFO_clear(
    SOC_SAND_ARR_MEM_ALLOCATOR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_INFO));
  info->nof_entries = 0;
  info->instance_prim_handle = 0;
  info->instance_sec_handle = 0;
  info->entry_get_fun = 0;
  info->entry_set_fun = 0;
  info->support_caching = 0;
  info->support_defragment = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_ITER_clear(
    SOC_SAND_ARR_MEM_ALLOCATOR_ITER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_ITER));
  info->block_size = 0;
  info->offset = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_REQ_BLOCKS_clear(
    SOC_SAND_ARR_MEM_ALLOCATOR_REQ_BLOCKS *info
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_REQ_BLOCKS));

  for (ind = 0; ind < SOC_SAND_ARR_MEM_ALLOCATOR_MAX_NOF_REQS; ++ind)
  {
     info->block_size[ind] = 0;
  }
  info->nof_reqs = 0;
    
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_MEM_STATUS_clear(
    SOC_SAND_ARR_MEM_ALLOCATOR_MEM_STATUS *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_MEM_STATUS));

  info->total_free = 0;
  info->max_free_block_size = 0;
  info->nof_fragments = 0;
  info->total_size = 0;
    
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_DEFRAG_INFO_clear(
    SOC_SAND_ARR_MEM_ALLOCATOR_DEFRAG_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_DEFRAG_INFO));

  info->block_range.iter = 0;
  info->block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;
  info->block_range.entries_to_act = SOC_SAND_TBL_ITER_SCAN_ALL;
    
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_LOAD_INFO_clear(
    SOC_SAND_ARR_MEM_ALLOCATOR_LOAD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_ARR_MEM_ALLOCATOR_LOAD_INFO));
   
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_SAND_DEBUG
void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_INFO_print(
    SOC_SAND_ARR_MEM_ALLOCATOR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "nof_entries: %u\n\r"),info->nof_entries));
  LOG_CLI((BSL_META_U(unit,
                      "instance_prim_handle: %u\n\r"),info->instance_prim_handle));
  LOG_CLI((BSL_META_U(unit,
                      "instance_sec_handle: %u\n\r"),info->instance_sec_handle));
  LOG_CLI((BSL_META_U(unit,
                      "support_caching: %u\n\r"),info->support_caching));
  LOG_CLI((BSL_META_U(unit,
                      "support_defragment: %u\n\r"),info->support_defragment));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_ITER_print(
    SOC_SAND_ARR_MEM_ALLOCATOR_ITER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "block_size: %u\n\r"),info->block_size));
  LOG_CLI((BSL_META_U(unit,
                      "offset: %u\n\r"),info->offset));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_REQ_BLOCKS_print(
    SOC_SAND_IN SOC_SAND_ARR_MEM_ALLOCATOR_REQ_BLOCKS *info
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  for (ind = 0; ind < SOC_SAND_ARR_MEM_ALLOCATOR_MAX_NOF_REQS; ++ind)
  {
     LOG_CLI((BSL_META_U(unit,
                         "block_size[ind]: %u\n\r"),info->block_size[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "nof_reqs: %u\n\r"),info->nof_reqs));
    
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_MEM_STATUS_print(
    SOC_SAND_IN SOC_SAND_ARR_MEM_ALLOCATOR_MEM_STATUS *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  LOG_CLI((BSL_META_U(unit,
                      "total_free: %u\n\r"),info->total_free));
  LOG_CLI((BSL_META_U(unit,
                      "max_free_block_size: %u\n\r"),info->max_free_block_size));
  LOG_CLI((BSL_META_U(unit,
                      "nof_fragments: %u\n\r"),info->nof_fragments));
  LOG_CLI((BSL_META_U(unit,
                      "total_size: %u\n\r"),info->total_size));
    
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_ARR_MEM_ALLOCATOR_DEFRAG_INFO_print(
    SOC_SAND_IN SOC_SAND_ARR_MEM_ALLOCATOR_DEFRAG_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  LOG_CLI((BSL_META_U(unit,
                      "block_range.iter: %u\n\r"),info->block_range.iter));
  LOG_CLI((BSL_META_U(unit,
                      "block_range.entries_to_scan: %u\n\r"),info->block_range.entries_to_scan));
  LOG_CLI((BSL_META_U(unit,
                      "block_range.entries_to_act: %u\n\r"),info->block_range.entries_to_act));
    
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif 

#include <soc/dpp/SAND/Utils/sand_footer.h>

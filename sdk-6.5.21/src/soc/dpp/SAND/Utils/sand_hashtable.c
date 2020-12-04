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
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_hashtable.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/drv.h>

extern shr_sw_state_t *sw_state[BCM_MAX_NUM_UNITS];








#define SOC_SAND_HASH_TABLE_MAX_NOF_REHASH  50





#define HASH_TABLE_ACCESS          sw_state_access[unit].dpp.soc.sand.hash_table  
#define HASH_TABLE_ACCESS_DATA     HASH_TABLE_ACCESS.hashs_array.hash_data
#define HASH_TABLE_ACCESS_INFO     HASH_TABLE_ACCESS.hashs_array.init_info

#define SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(_hash_table_index,_err1,_err2) \
  { \
    uint8 bit_val ; \
    uint32 max_nof_hashs ; \
    res = HASH_TABLE_ACCESS.max_nof_hashs.get(unit, &max_nof_hashs) ; \
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err1, exit) ; \
    if (_hash_table_index >= max_nof_hashs) \
    { \
       \
      bit_val = 0 ; \
    } \
    else \
    { \
      res = HASH_TABLE_ACCESS.occupied_hashs.bit_get(unit, (int)_hash_table_index, &bit_val) ; \
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err1, exit) ; \
    } \
    if (bit_val == 0) \
    { \
       \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_FREE_FAIL, _err2, exit) ; \
    } \
  }

#define SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, _err1) \
  if ((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES)) \
  { \
     \
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MAX_NUM_DEVICES_OUT_OF_RANGE_ERR, _err1, exit); \
  }

#define SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(_hash_table_index,_handle) (_hash_table_index = _handle - 1)
#define SOC_SAND_HASH_TABLE_CONVERT_HASHTABLE_INDEX_TO_HANDLE(_handle,_hash_table_index) (_handle = _hash_table_index + 1)






typedef uint8 SOC_SAND_HASH_TABLE_KEY_TYPE ;


 

 
 

 

 
 







STATIC uint32
  soc_sand_hash_table_get_tmp_data_ptr_from_handle(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR      hash_table,
    SOC_SAND_OUT uint8                        **tmp_buf_ptr_ptr
  )
{
  uint32
    hash_table_index,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0) ;
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  *tmp_buf_ptr_ptr = sw_state[unit]->dpp.soc.sand.hash_table->hashs_array[hash_table_index]->hash_data.tmp_buf ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_get_tmp_data_ptr_from_handle()",0,0);
}



STATIC uint32
  soc_sand_hash_table_get_tmp2_data_ptr_from_handle(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR      hash_table,
    SOC_SAND_OUT uint8                        **tmp_buf2_ptr_ptr
  )
{
  uint32
    hash_table_index,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0) ;
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  *tmp_buf2_ptr_ptr = sw_state[unit]->dpp.soc.sand.hash_table->hashs_array[hash_table_index]->hash_data.tmp_buf2 ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_get_tmp2_data_ptr_from_handle()",0,0);
}


uint32
  soc_sand_hash_table_get_illegal_hashtable_handle(
    void
  )
{
  return ((uint32)(-1)) ;
}

uint32
sand_hashtable_default_set_entry_fun(
  SOC_SAND_IN    int prime_handle,
  SOC_SAND_IN    uint32 sec_handle,
  SOC_SAND_INOUT uint8  *buffer,
  SOC_SAND_IN    uint32 offset,
  SOC_SAND_IN    uint32 len,
  SOC_SAND_IN    uint8  *data
)
{
  sal_memcpy(
    buffer + (offset * len),
    data,
    len
  );
  return SOC_SAND_OK;
}


uint32
sand_hashtable_default_get_entry_fun(
  SOC_SAND_IN  int prime_handle,
  SOC_SAND_IN  uint32 sec_handle,
  SOC_SAND_IN  uint8  *buffer,
  SOC_SAND_IN  uint32 offset,
  SOC_SAND_IN  uint32 len,
  SOC_SAND_OUT uint8  *data
)
{
  sal_memcpy(
    data,
    buffer + (offset * len),
    len
  );
  return SOC_SAND_OK;
}


uint32
  soc_sand_hash_table_get_table_size(
    SOC_SAND_IN     int                              unit,
    SOC_SAND_IN    SOC_SAND_HASH_TABLE_PTR           hash_table,
    SOC_SAND_INOUT uint32                            *table_size_ptr
  )
{
  uint32
    hash_table_index,
    res,
    table_size ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  res = HASH_TABLE_ACCESS_INFO.table_size.get(unit, hash_table_index, &table_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  *table_size_ptr = table_size ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_get_table_size()",0,0);
}

 
uint32
  soc_sand_hash_table_simple_hash(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR     hash_table,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY     *const key,
    SOC_SAND_IN  uint32                      seed,
    SOC_SAND_OUT uint32                      *hash_val
  )
{
  uint32
    res,
    hash_table_index,
    indx,
    tmp,
    table_width,
    key_size ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(hash_val);

  tmp = 5381 ;
  res = HASH_TABLE_ACCESS_INFO.key_size.get(unit,hash_table_index,&key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = HASH_TABLE_ACCESS_INFO.table_width.get(unit,hash_table_index,&table_width) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  for (indx = 0 ; indx < key_size ; ++indx)
  {
    tmp = ((tmp  << 5 ) + tmp ) ^ key[indx]  ;
  }
  *hash_val = tmp % table_width ;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_simple_hash()",0,0);
}


uint32
  soc_sand_hash_table_simple_rehash(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR    hash_table,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY    *const key,
    SOC_SAND_IN  uint32                     seed,
    SOC_SAND_OUT uint32                     *hash_val
  )
{
  uint32
    res,
    hash_table_index,
    table_width ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0) ;
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(key) ;
  SOC_SAND_CHECK_NULL_INPUT(hash_val) ;

  res = HASH_TABLE_ACCESS_INFO.table_width.get(unit,hash_table_index,&table_width) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  *hash_val = (seed + 1) % table_width;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_simple_hash()",0,0);
}


STATIC uint32
  soc_sand_hash_table_find_entry(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR      hash_table,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_KEY      *const key,
    SOC_SAND_IN     uint8                        first_empty,
    SOC_SAND_IN     uint8                        alloc_by_index,
    SOC_SAND_OUT    uint32                       *entry,
    SOC_SAND_OUT    uint8                        *found,
    SOC_SAND_OUT    uint32                       *prev,
    SOC_SAND_OUT    uint8                        *first
  );






uint32
  soc_sand_hash_table_init(
    SOC_SAND_IN       int                          unit,
    SOC_SAND_IN       uint32                       max_nof_hashs
  )
{
  uint32 res ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_INIT) ;
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;

  res = HASH_TABLE_ACCESS.alloc(unit);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = HASH_TABLE_ACCESS.hashs_array.ptr_alloc(unit, max_nof_hashs);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  res = HASH_TABLE_ACCESS.max_nof_hashs.set(unit, max_nof_hashs);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  res = HASH_TABLE_ACCESS.in_use.set(unit, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  res = HASH_TABLE_ACCESS.occupied_hashs.alloc_bitmap(unit, max_nof_hashs);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_init()",0,0);
}


uint32
  soc_sand_hash_table_clear_all_tmps(
    SOC_SAND_IN int unit
  )
{
  uint32
    hash_table_index ;
  uint32
    max_buf_size,
    found,
    res,
    max_nof_hashs,
    key_size,
    data_size,
    ptr_size,
    in_use ;
  uint8
    bit_val ;
  uint8
    is_allocated ;
  int32
    offset ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;

  res = HASH_TABLE_ACCESS.in_use.get(unit, &in_use);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);

  res = HASH_TABLE_ACCESS.max_nof_hashs.get(unit, &max_nof_hashs);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);

  if (in_use >= max_nof_hashs)
  {
    
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 8, exit);
  }
  
  found = 0 ;
  offset = 0 ;
  for (hash_table_index = 0 ; hash_table_index < max_nof_hashs ; hash_table_index++)
  {
    res = HASH_TABLE_ACCESS.occupied_hashs.bit_get(unit, hash_table_index, &bit_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
    if (bit_val == 1)
    {
      
      found++ ;
      res = HASH_TABLE_ACCESS_DATA.tmp_buf.is_allocated(unit,hash_table_index,&is_allocated) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
      if (!is_allocated)
      {
        
        SOC_SAND_SET_ERROR_CODE(SOC_E_INTERNAL, 16, exit);
      }
      res = HASH_TABLE_ACCESS_DATA.tmp_buf2.is_allocated(unit,hash_table_index,&is_allocated) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
      if (!is_allocated)
      {
        
        SOC_SAND_SET_ERROR_CODE(SOC_E_INTERNAL, 20, exit);
      }
      res = HASH_TABLE_ACCESS_INFO.data_size.get(unit, hash_table_index, &data_size) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
      res = HASH_TABLE_ACCESS_INFO.key_size.get(unit, hash_table_index, &key_size) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit) ;
      res = HASH_TABLE_ACCESS_DATA.ptr_size.get(unit, hash_table_index, &ptr_size) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
      max_buf_size = data_size ;
      if (key_size > max_buf_size)
      {
        max_buf_size = key_size;
      }
      if (ptr_size > max_buf_size)
      {
        max_buf_size = ptr_size;
      }
      
      res = HASH_TABLE_ACCESS_DATA.tmp_buf.memset(unit,hash_table_index,offset,max_buf_size,0) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit) ;
      
      res = HASH_TABLE_ACCESS_DATA.tmp_buf2.memset(unit,hash_table_index,offset,max_buf_size,0) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit) ;
      if (found >= in_use)
      {
        
        break ;
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_clear_all_tmps()",0,0);
}


uint32
  soc_sand_hash_table_create(
    SOC_SAND_IN     int                               unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR           *hash_table_ptr,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_INIT_INFO     init_info
  )
{
  SOC_SAND_HASH_TABLE_PTR
    hash_table ;
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info ;
  uint32
    max_buf_size ;
  uint32
    found,
    res ;
  uint32
    hash_table_index,
    table_size,
    table_width,
    data_size,
    key_size,
    ptr_size,
    null_ptr,
    max_nof_hashs ;
  uint32
    in_use ;
  uint8
    bit_val ;
  int32
    offset ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_CREATE);

  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;

  SOC_SAND_CHECK_NULL_INPUT(hash_table_ptr) ;

  res = HASH_TABLE_ACCESS.in_use.get(unit, &in_use);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);

  res = HASH_TABLE_ACCESS.max_nof_hashs.get(unit, &max_nof_hashs);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);

  if (in_use >= max_nof_hashs)
  {
    
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 8, exit);
  }
  
  res = HASH_TABLE_ACCESS.in_use.set(unit, (in_use + 1));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  
  found = 0 ;
  for (hash_table_index = 0 ; hash_table_index < max_nof_hashs ; hash_table_index++)
  {
    res = HASH_TABLE_ACCESS.occupied_hashs.bit_get(unit, hash_table_index, &bit_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
    if (bit_val == 0)
    {
      
      found = 1 ;
      break ;
    }
  }
  if (!found)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 14, exit);
  }
  res = HASH_TABLE_ACCESS.occupied_hashs.bit_set(unit, hash_table_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);

  res = HASH_TABLE_ACCESS.hashs_array.alloc(unit, hash_table_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);
  
  SOC_SAND_HASH_TABLE_CONVERT_HASHTABLE_INDEX_TO_HANDLE(hash_table,hash_table_index) ;
  
  *hash_table_ptr = hash_table ;
  
  soc_sand_SAND_HASH_TABLE_INFO_clear(unit,hash_table) ;
  
  if (init_info.get_entry_fun != NULL || init_info.set_entry_fun != NULL )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 7, exit);
  }

  res = HASH_TABLE_ACCESS_INFO.set(unit, hash_table_index, &init_info) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit) ;

  res = HASH_TABLE_ACCESS_INFO.table_size.get(unit, hash_table_index, &table_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit) ;
  res = HASH_TABLE_ACCESS_INFO.table_width.get(unit, hash_table_index, &table_width) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  res = HASH_TABLE_ACCESS_INFO.key_size.get(unit, hash_table_index, &key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit) ;
  res = HASH_TABLE_ACCESS_INFO.data_size.get(unit, hash_table_index, &data_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit) ;

  if (table_size == 0 || table_width == 0 || key_size == 0 || data_size == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_HASH_TABLE_SIZE_OUT_OF_RANGE_ERR, 10, exit);
  }
  
  ptr_size = (soc_sand_log2_round_up(table_size + 1) + (SOC_SAND_NOF_BITS_IN_BYTE - 1)) / SOC_SAND_NOF_BITS_IN_BYTE ;
  null_ptr = SOC_SAND_BITS_MASK((ptr_size * SOC_SAND_NOF_BITS_IN_BYTE - 1),0) ;

  res = HASH_TABLE_ACCESS_DATA.ptr_size.set(unit, hash_table_index, ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 36, exit) ;
  res = HASH_TABLE_ACCESS_DATA.null_ptr.set(unit, hash_table_index, null_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit) ;
  
  offset = 0 ;
  res = HASH_TABLE_ACCESS_DATA.keys.alloc(unit,hash_table_index,table_size * key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 42, exit) ;
  res = HASH_TABLE_ACCESS_DATA.keys.memset(unit,hash_table_index,offset,table_size * key_size,0x00) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 46, exit) ;
  
  res = HASH_TABLE_ACCESS_DATA.next.alloc(unit,hash_table_index,table_size * ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit) ;
  res = HASH_TABLE_ACCESS_DATA.next.memset(unit,hash_table_index,offset,table_size * ptr_size,0xFF) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 54, exit) ;
  
  res = HASH_TABLE_ACCESS_DATA.lists_head.alloc(unit,hash_table_index,table_width * ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 58, exit) ;
  res = HASH_TABLE_ACCESS_DATA.lists_head.memset(unit,hash_table_index,offset,table_width * ptr_size,0xFF) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 62, exit) ;

  max_buf_size = data_size ;
  if (key_size > max_buf_size)
  {
    max_buf_size = key_size;
  }
  if (ptr_size > max_buf_size)
  {
    max_buf_size = ptr_size;
  }
  
  res = HASH_TABLE_ACCESS_DATA.tmp_buf.alloc(unit,hash_table_index,max_buf_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 82, exit) ;
  res = HASH_TABLE_ACCESS_DATA.tmp_buf.memset(unit,hash_table_index,offset,max_buf_size,0x00) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 86, exit) ;
  
  res = HASH_TABLE_ACCESS_DATA.tmp_buf2.alloc(unit,hash_table_index,max_buf_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 90, exit) ;
  res = HASH_TABLE_ACCESS_DATA.tmp_buf2.memset(unit,hash_table_index,offset,max_buf_size,0x00) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 94, exit) ;
  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  btmp_init_info.size = table_size;
  
  res = soc_sand_occ_bm_create(
          unit,
          &btmp_init_info,
          &(memory_use)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 98, exit);
  res = HASH_TABLE_ACCESS_DATA.memory_use.set(unit,hash_table_index,memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 102, exit) ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_create()",0,0);
}


uint32
  soc_sand_hash_table_destroy(
    SOC_SAND_IN     int                        unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR    hash_table
  )
{
  uint32
    res,
    hash_table_index ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;
  uint8
    bit_val ;
  uint32
    in_use ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_DESTROY);
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;
  
  res = HASH_TABLE_ACCESS.in_use.get(unit, &in_use);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
  if ((int)in_use < 0)
  {
    
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_FREE_FAIL, 12, exit);
  }
  
  res = HASH_TABLE_ACCESS.in_use.set(unit, (in_use - 1));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit) ;
  
  res = HASH_TABLE_ACCESS.occupied_hashs.bit_get(unit, hash_table_index, &bit_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  if (bit_val == 0)
  {
    
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_FREE_FAIL, 24, exit) ;
  }
  res = HASH_TABLE_ACCESS.occupied_hashs.bit_clear(unit, hash_table_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);

  res = HASH_TABLE_ACCESS_DATA.lists_head.free(unit,hash_table_index) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit) ;
  res = HASH_TABLE_ACCESS_DATA.next.free(unit,hash_table_index) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 36, exit) ;
  res = HASH_TABLE_ACCESS_DATA.keys.free(unit,hash_table_index) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit) ;
  res = HASH_TABLE_ACCESS_DATA.tmp_buf.free(unit,hash_table_index) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 44, exit) ;
  res = HASH_TABLE_ACCESS_DATA.tmp_buf2.free(unit,hash_table_index) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 48, exit) ;
  res = HASH_TABLE_ACCESS_DATA.memory_use.get(unit,hash_table_index,&memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 52, exit) ;

  res = soc_sand_occ_bm_destroy(unit,memory_use);
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  res = HASH_TABLE_ACCESS.hashs_array.free(unit,hash_table_index) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit) ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_destroy()",0,0);
}


uint32
  soc_sand_hash_table_entry_add(
    SOC_SAND_IN    int                        unit,
    SOC_SAND_IN    SOC_SAND_HASH_TABLE_PTR    hash_table,
    SOC_SAND_IN    SOC_SAND_HASH_TABLE_KEY    *const key,
    SOC_SAND_OUT   uint32                     *data_indx,
    SOC_SAND_OUT   uint8                      *success
  )
{
  uint8
    found,
    first;
  uint32
    key_size,
    entry_offset,
    prev_entry;
  uint32
    res,
    hash_table_index ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_ENTRY_ADD) ;
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(data_indx);
  SOC_SAND_CHECK_NULL_INPUT(success);
  
  res = soc_sand_hash_table_find_entry(
          unit,
          hash_table,
          key,
          TRUE,
          FALSE,
          &entry_offset,
          &found,
          &prev_entry,
          &first
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (found)
  {
    *data_indx = entry_offset;
  }
  if (entry_offset == SOC_SAND_HASH_TABLE_NULL)
  {
    *success = FALSE;
    *data_indx = SOC_SAND_HASH_TABLE_NULL;
    goto exit;
  }
  res = HASH_TABLE_ACCESS_INFO.key_size.get(unit, hash_table_index, &key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  
  res = HASH_TABLE_ACCESS_DATA.keys.memwrite(unit,hash_table_index,key,key_size * entry_offset,key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  *success = TRUE;
  *data_indx = entry_offset;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_entry_add()",0,0);
}

uint32
  soc_sand_hash_table_entry_add_at_index(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_PTR      hash_table,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_KEY      *const key,
    SOC_SAND_IN     uint32                       data_indx,
    SOC_SAND_OUT    uint8                        *success
  )
{
  uint8
    found,
    indx_in_use,
    first ;
  uint32
    entry_offset,
    prev_entry ;
  uint32
    old_index ;
  uint32
    res,
    hash_table_index,
    key_size ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_ENTRY_ADD);
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = HASH_TABLE_ACCESS_DATA.memory_use.get(unit,hash_table_index,&memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit) ;
  res = HASH_TABLE_ACCESS_INFO.key_size.get(unit, hash_table_index, &key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;

  
  res = soc_sand_hash_table_entry_lookup(
          unit,
          hash_table,
          key,
          &old_index,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  if (found && old_index != data_indx)
  {
    *success = FALSE;
    goto exit;
  }
  if (found && old_index == data_indx)
  {
    
    *success = TRUE;
    goto exit;
  }

  res = soc_sand_occ_bm_is_occupied(
          unit,
          memory_use,
          data_indx,
          &indx_in_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  if (indx_in_use && !found)
  {
    
    *success = FALSE;
    goto exit;
  }
  
  entry_offset = data_indx;
  
  res = soc_sand_hash_table_find_entry(
          unit,
          hash_table,
          key,
          TRUE,
          TRUE,
          &entry_offset,
          &found,
          &prev_entry,
          &first
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit) ;

  if (entry_offset == SOC_SAND_HASH_TABLE_NULL)
  {
    *success = FALSE;
    goto exit;
  }
  
  res = HASH_TABLE_ACCESS_DATA.keys.memwrite(unit,hash_table_index,key,key_size * entry_offset,key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit) ;
  *success = TRUE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_entry_add_at_index()",0,0);
}


uint32
  soc_sand_hash_table_entry_remove(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_PTR      hash_table,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_KEY      *const key
  )
{
  uint8
    found,
    first ;
  uint32
    entry_offset,
    prev_entry ;
  uint32
    res,
    null_ptr,
    hash_table_index,
    ptr_size ;
  uint8
    *next_entry ;
  uint8
    *tmp_buf_ptr ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_ENTRY_REMOVE);
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(key) ;
  
  res = soc_sand_hash_table_find_entry(
          unit,
          hash_table,
          key,
          FALSE,
          FALSE,
          &entry_offset,
          &found,
          &prev_entry,
          &first
        ) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  if (!found)
  {
    goto exit;
  }
  
  
  res = HASH_TABLE_ACCESS_DATA.ptr_size.get(unit, hash_table_index, &ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit) ;
  res = soc_sand_hash_table_get_tmp_data_ptr_from_handle(unit,hash_table,&tmp_buf_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
  
  res = HASH_TABLE_ACCESS_DATA.next.memread(unit,hash_table_index,tmp_buf_ptr,ptr_size * entry_offset,ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit) ;
  next_entry = tmp_buf_ptr ;
  
  if (first)
  {
    
    res = HASH_TABLE_ACCESS_DATA.lists_head.memwrite(unit,hash_table_index,next_entry,ptr_size * prev_entry,ptr_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  }
  else
  {
    
    res = HASH_TABLE_ACCESS_DATA.next.memwrite(unit,hash_table_index,next_entry,ptr_size * prev_entry,ptr_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit) ;
  }
  
  res = HASH_TABLE_ACCESS_DATA.memory_use.get(unit,hash_table_index,&memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit) ;
  res = soc_sand_occ_bm_occup_status_set(
          unit,
          memory_use,
          entry_offset,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  res = HASH_TABLE_ACCESS_DATA.null_ptr.get(unit, hash_table_index, &null_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit) ;

  res = soc_sand_U32_to_U8(
           &(null_ptr),
           ptr_size,
           tmp_buf_ptr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  
  res = HASH_TABLE_ACCESS_DATA.next.memwrite(unit,hash_table_index,tmp_buf_ptr,ptr_size * entry_offset,ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit) ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_entry_remove()",0,0);
}

uint32
  soc_sand_hash_table_entry_remove_by_index(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_PTR hash_table,
    SOC_SAND_IN     uint32                  data_indx
  )
{
  uint8
    *cur_key ;
  uint8
    in_use ;
  uint32
    res,
    key_size,
    table_size,
    hash_table_index ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;
  uint8
    *tmp_buf2_ptr = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_ENTRY_REMOVE_BY_INDEX);

  table_size = 0 ;
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  res = HASH_TABLE_ACCESS_INFO.table_size.get(unit, hash_table_index, &table_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = HASH_TABLE_ACCESS_INFO.key_size.get(unit, hash_table_index, &key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  res = HASH_TABLE_ACCESS_DATA.memory_use.get(unit,hash_table_index,&memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit) ;
  
  res = soc_sand_occ_bm_is_occupied(
          unit,
          memory_use,
          data_indx,
          &in_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (!in_use)
  {
    
    goto exit;
  }
  res = soc_sand_hash_table_get_tmp2_data_ptr_from_handle(unit,hash_table,&tmp_buf2_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
  
  res = HASH_TABLE_ACCESS_DATA.keys.memread(unit,hash_table_index,tmp_buf2_ptr,key_size * data_indx,key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  cur_key = tmp_buf2_ptr ;
  res = soc_sand_hash_table_entry_remove(
          unit,
          hash_table,
          cur_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_entry_remove_by_index()", data_indx, (hash_table) ? table_size : 0);
}


uint32
  soc_sand_hash_table_entry_lookup(
    SOC_SAND_IN     int                         unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR     hash_table,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_KEY     *const key,
    SOC_SAND_OUT    uint32                      *data_indx,
    SOC_SAND_OUT    uint8                       *found
  )
{
  uint8
    is_found,
    first ;
  uint32
    entry_offset,
    prev_entry ;
  uint32
    hash_table_index,
    res ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_ENTRY_LOOKUP);
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(data_indx);
  SOC_SAND_CHECK_NULL_INPUT(found);

  
  res = soc_sand_hash_table_find_entry(
          unit,
          hash_table,
          key,
          FALSE,
          FALSE,
          &entry_offset,
          &is_found,
          &prev_entry,
          &first
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  if (!is_found)
  {
    *found = FALSE;
    *data_indx = SOC_SAND_HASH_TABLE_NULL;
    goto exit;
  }
  *found = TRUE;
  *data_indx = entry_offset;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_entry_lookup()",0,0);
}


uint32
  soc_sand_hash_table_get_by_index(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR       hash_table,
    SOC_SAND_IN  uint32                        data_indx,
    SOC_SAND_OUT SOC_SAND_HASH_TABLE_KEY       *key,
    SOC_SAND_OUT uint8                         *found
  )
{
  uint32
    table_size,
    key_size,
    hash_table_index,
    res ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_ENTRY_LOOKUP);

  table_size = 0 ;
  hash_table_index = 0 ;
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  res = HASH_TABLE_ACCESS_INFO.table_size.get(unit, hash_table_index, &table_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = HASH_TABLE_ACCESS_INFO.key_size.get(unit, hash_table_index, &key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  res = HASH_TABLE_ACCESS_DATA.memory_use.get(unit,hash_table_index,&memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit) ;

  if (data_indx > table_size)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  }
  
  res = soc_sand_occ_bm_is_occupied(
          unit,
          memory_use,
          data_indx,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  if (!*found)
  {
    goto exit;
  }
  
  res = HASH_TABLE_ACCESS_DATA.keys.memread(unit,hash_table_index,key,key_size * data_indx,key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_entry_get_by_index()",data_indx,(hash_table_index)? table_size: 0) ;
}


uint32
  soc_sand_hash_table_clear(
    SOC_SAND_IN     int                        unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR    hash_table
  )
{
  uint32
    max_buf_size,
    data_size,
    ptr_size,
    table_width,
    table_size,
    key_size,
    hash_table_index,
    res ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;
  int32
    offset ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_CREATE);
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  res = HASH_TABLE_ACCESS_INFO.table_size.get(unit, hash_table_index, &table_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = HASH_TABLE_ACCESS_INFO.key_size.get(unit, hash_table_index, &key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  res = HASH_TABLE_ACCESS_DATA.memory_use.get(unit, hash_table_index, &memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit) ;
  res = HASH_TABLE_ACCESS_DATA.ptr_size.get(unit, hash_table_index, &ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  res = HASH_TABLE_ACCESS_INFO.table_width.get(unit, hash_table_index, &table_width) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit) ;
  res = HASH_TABLE_ACCESS_INFO.data_size.get(unit, hash_table_index, &data_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
  offset = 0 ;
  
  res = HASH_TABLE_ACCESS_DATA.keys.memset(unit,hash_table_index,offset,table_size * key_size,0x00) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit) ;
  
  res = HASH_TABLE_ACCESS_DATA.next.memset(unit,hash_table_index,offset,table_size * ptr_size,0xFF) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit) ;
  
  res = HASH_TABLE_ACCESS_DATA.lists_head.memset(unit,hash_table_index,offset,table_width * ptr_size,0xFF) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 34, exit) ;

  max_buf_size = data_size ;
  if (key_size > max_buf_size)
  {
    max_buf_size = key_size;
  }
  if (ptr_size > max_buf_size)
  {
    max_buf_size = ptr_size;
  }
  
  res = HASH_TABLE_ACCESS_DATA.tmp_buf.memset(unit,hash_table_index,offset,max_buf_size,0x00) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 38, exit) ;
  res = HASH_TABLE_ACCESS_DATA.tmp_buf2.memset(unit,hash_table_index,offset,max_buf_size,0x00) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 42, exit) ;

  

  res = soc_sand_occ_bm_clear(
          unit,
          memory_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,50, exit) ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_clear()",0,0) ;
}


uint32
  soc_sand_hash_table_get_next(
    SOC_SAND_IN     int                         unit,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_PTR     hash_table,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_ITER    *iter,
    SOC_SAND_OUT    SOC_SAND_HASH_TABLE_KEY     *const key,
    SOC_SAND_OUT    uint32                      *data_indx
  )
{
  uint32
    indx ;
  uint32
    res,
    table_size,
    key_size,
    hash_table_index ;
  uint8
    occupied ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_GET_NEXT) ;
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(iter);
  SOC_SAND_CHECK_NULL_INPUT(data_indx);

  res = HASH_TABLE_ACCESS_INFO.table_size.get(unit, hash_table_index, &table_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = HASH_TABLE_ACCESS_INFO.key_size.get(unit, hash_table_index, &key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  res = HASH_TABLE_ACCESS_DATA.memory_use.get(unit, hash_table_index, &memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit) ;
  
  for (indx = *iter; indx < table_size   ; ++indx)
  {
    res = soc_sand_occ_bm_is_occupied(
            unit,
            memory_use,
            indx,
            &occupied
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (occupied)
    {
      *data_indx = indx ;
      *iter = indx + 1 ;
      
      res = HASH_TABLE_ACCESS_DATA.keys.memread(unit,hash_table_index,key,key_size * indx,key_size) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
      goto exit ;
    }
  }
  *iter = SOC_SAND_U32_MAX ;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_get_next()",0,0) ;
}

uint32
  soc_sand_hash_table_get_size_for_save(
    SOC_SAND_IN   int                         unit,
    SOC_SAND_IN   SOC_SAND_HASH_TABLE_PTR     hash_table,
    SOC_SAND_OUT  uint32                      *size
  )
{
  uint32
    table_width,
    table_size,
    key_size,
    ptr_size,
    bmp_size,
    total_size ;
  uint32
    hash_table_index,
    res;
  SOC_SAND_OCC_BM_PTR
    memory_use ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0) ;

  total_size = 0 ;

  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(size) ;

  res = HASH_TABLE_ACCESS_INFO.table_size.get(unit, hash_table_index, &table_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = HASH_TABLE_ACCESS_INFO.key_size.get(unit, hash_table_index, &key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  res = HASH_TABLE_ACCESS_DATA.memory_use.get(unit, hash_table_index, &memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit) ;
  res = HASH_TABLE_ACCESS_DATA.ptr_size.get(unit, hash_table_index, &ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  res = HASH_TABLE_ACCESS_INFO.table_width.get(unit, hash_table_index, &table_width) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit) ;

  
  total_size += sizeof(SOC_SAND_HASH_TABLE_INIT_INFO);

  
 
  total_size += table_size * key_size ;

  total_size += table_size * ptr_size ;

  total_size += table_width * ptr_size ;

  

  res = soc_sand_occ_bm_get_size_for_save(
          unit,
          memory_use,
          &bmp_size
        ) ;
  SOC_SAND_CHECK_FUNC_RESULT(res,22, exit) ;
  total_size += bmp_size ;
  *size = total_size ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_get_size_for_save()",0,0);
}


uint32
  soc_sand_hash_table_save(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR     hash_table,
    SOC_SAND_OUT uint8                       *buffer,
    SOC_SAND_IN  uint32                      buffer_size_bytes,
    SOC_SAND_OUT uint32                      *actual_size_bytes
  )
{
  uint8
    *cur_ptr ;
  uint32
    cur_size, table_size,
    key_size, ptr_size,
    table_width,
    total_size ;
  uint32
    hash_table_index,
    res ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;
  int
    offset ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  cur_ptr = buffer ;
  total_size = 0 ;

  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(buffer);
  SOC_SAND_CHECK_NULL_INPUT(actual_size_bytes);

  res = HASH_TABLE_ACCESS_INFO.table_size.get(unit, hash_table_index, &table_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = HASH_TABLE_ACCESS_INFO.key_size.get(unit, hash_table_index, &key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  res = HASH_TABLE_ACCESS_DATA.memory_use.get(unit, hash_table_index, &memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit) ;
  res = HASH_TABLE_ACCESS_DATA.ptr_size.get(unit, hash_table_index, &ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  res = HASH_TABLE_ACCESS_INFO.table_width.get(unit, hash_table_index, &table_width) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit) ;
  offset = 0 ;
  
  res = HASH_TABLE_ACCESS_INFO.get(unit, hash_table_index, (SOC_SAND_HASH_TABLE_INIT_INFO *)cur_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  cur_ptr += sizeof(SOC_SAND_HASH_TABLE_INIT_INFO) ;

  
  res = HASH_TABLE_ACCESS_DATA.keys.memread(unit, hash_table_index, cur_ptr, offset,table_size * key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  cur_ptr += (table_size * key_size) ;

  res = HASH_TABLE_ACCESS_DATA.next.memread(unit, hash_table_index, cur_ptr, offset,table_size * ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  cur_ptr += (table_size * ptr_size) ;

  res = HASH_TABLE_ACCESS_DATA.lists_head.memread(unit, hash_table_index, cur_ptr, offset,table_width * ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  cur_ptr += (table_width * ptr_size) ;

  
  res = soc_sand_occ_bm_save(
          unit,
          memory_use,
          cur_ptr,
          buffer_size_bytes - total_size,
          &cur_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20, exit);
  cur_ptr += cur_size ;
  total_size += cur_size ;
  *actual_size_bytes = total_size ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_save()",0,0);
}

uint32
  soc_sand_hash_table_load(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint8                                 **buffer,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_SW_DB_ENTRY_SET     set_entry_fun,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_SW_DB_ENTRY_GET     get_entry_fun,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK  hash_function,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK  rehash_function,
    SOC_SAND_OUT SOC_SAND_HASH_TABLE_PTR               *hash_table_ptr
  )
{
  SOC_SAND_IN uint8
    *cur_ptr ;
  SOC_SAND_HASH_TABLE_PTR
    hash_table ;
  uint32
    hash_table_index, table_size,
    key_size, ptr_size,
    table_width,
    res ;
  SOC_SAND_HASH_TABLE_INIT_INFO
    init_info ;
  int
    offset ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;

  SOC_SAND_CHECK_NULL_INPUT(buffer) ;

  cur_ptr = (SOC_SAND_IN uint8 *)buffer[0] ;

  
  soc_sand_os_memcpy(&(init_info), cur_ptr, sizeof(SOC_SAND_HASH_TABLE_INIT_INFO)) ;
  cur_ptr += sizeof(SOC_SAND_HASH_TABLE_INIT_INFO) ;
  init_info.hash_function = hash_function ;
  init_info.rehash_function = rehash_function ;
  init_info.set_entry_fun = set_entry_fun ;
  init_info.get_entry_fun = get_entry_fun ;

  

  
  res = soc_sand_hash_table_create(unit, &hash_table, init_info);
  SOC_SAND_CHECK_FUNC_RESULT(res,20, exit);
  *hash_table_ptr = hash_table ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;

  res = HASH_TABLE_ACCESS_INFO.table_size.get(unit, hash_table_index, &table_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = HASH_TABLE_ACCESS_INFO.key_size.get(unit, hash_table_index, &key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  res = HASH_TABLE_ACCESS_DATA.memory_use.get(unit, hash_table_index, &memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit) ;
  res = HASH_TABLE_ACCESS_DATA.ptr_size.get(unit, hash_table_index, &ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  res = HASH_TABLE_ACCESS_INFO.table_width.get(unit, hash_table_index, &table_width) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit) ;
  offset = 0 ;

  

  res = HASH_TABLE_ACCESS_DATA.keys.memwrite(unit, hash_table_index, cur_ptr, offset,table_size * key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
  cur_ptr += (table_size * key_size) ;

  res = HASH_TABLE_ACCESS_DATA.next.memwrite(unit, hash_table_index, cur_ptr, offset,table_size * ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit) ;
  cur_ptr += (table_size * ptr_size) ;

  res = HASH_TABLE_ACCESS_DATA.lists_head.memwrite(unit, hash_table_index, cur_ptr, offset,table_width * ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  cur_ptr += (table_width * ptr_size) ;

  
  res = soc_sand_occ_bm_destroy(
          unit,
          memory_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,30, exit);

  
  res = soc_sand_occ_bm_load(
          unit,
          &cur_ptr,
          &memory_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,34, exit);
  res = HASH_TABLE_ACCESS_DATA.memory_use.set(unit, hash_table_index, memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 36, exit) ;
  *buffer = cur_ptr ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_load()",0,0);
}

uint32
  soc_sand_SAND_HASH_TABLE_INFO_clear(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR  hash_table
  )
{
  SOC_SAND_HASH_TABLE_INIT_INFO
    info ;
  uint32
    hash_table_index,
    res ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0) ;
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  soc_sand_os_memset((void *)&info, 0x0, (uint32)sizeof(SOC_SAND_HASH_TABLE_INIT_INFO)) ;
  res = HASH_TABLE_ACCESS_INFO.set(unit, hash_table_index, &info) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit) ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_SAND_HASH_TABLE_INFO_clear()", 0, 0) ;
}

#if SOC_SAND_DEBUG


uint32
  soc_sand_hash_table_print(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR hash_table
  )
{
  uint32
    indx ;
  uint32
    ptr_long,
    print_indx ;
  uint8
    *list_head,
    *cur_key,
    *next ;
  uint32
    table_width,
    ptr_size,
    null_ptr,
    key_size,
    hash_table_index,
    res ;
  uint8
    *tmp_buf_ptr ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_PRINT);
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  res = HASH_TABLE_ACCESS_INFO.key_size.get(unit, hash_table_index, &key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  res = HASH_TABLE_ACCESS_DATA.ptr_size.get(unit, hash_table_index, &ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  res = HASH_TABLE_ACCESS_INFO.table_width.get(unit, hash_table_index, &table_width) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit) ;
  res = HASH_TABLE_ACCESS_DATA.null_ptr.get(unit, hash_table_index, &null_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit) ;
  res = soc_sand_hash_table_get_tmp_data_ptr_from_handle(unit,hash_table,&tmp_buf_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
  
  for (indx = 0; indx < table_width   ; ++indx)
  {
    
    res = HASH_TABLE_ACCESS_DATA.lists_head.memread(unit,hash_table_index,tmp_buf_ptr,ptr_size * indx,ptr_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
    list_head = tmp_buf_ptr ;
    ptr_long = 0;

    res = soc_sand_U8_to_U32(
            list_head,
            ptr_size,
            &ptr_long
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (ptr_long == null_ptr)
    {
      continue ;
    }
    LOG_CLI((BSL_META_U(unit, " entry %u:  "), indx));

    while (ptr_long != null_ptr)
    {
      
      
      res = HASH_TABLE_ACCESS_DATA.keys.memread(unit,hash_table_index,tmp_buf_ptr,key_size * ptr_long,key_size) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
      cur_key = tmp_buf_ptr ;

      LOG_CLI((BSL_META_U(unit,"(0x"))) ;
      for(print_indx = 0; print_indx < key_size; ++print_indx)
      {
         LOG_CLI((BSL_META_U(unit,"%02x"), cur_key[key_size - print_indx - 1]));
      }
      LOG_CLI((BSL_META_U(unit,"--> %u)"), ptr_long - 1));
      LOG_CLI((BSL_META_U(unit,"\t")));
      
      
      res = HASH_TABLE_ACCESS_DATA.next.memread(unit,hash_table_index,tmp_buf_ptr,ptr_size * ptr_long,ptr_size) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
      next = tmp_buf_ptr ;
      ptr_long = 0;
      res = soc_sand_U8_to_U32(
              next,
              ptr_size,
              &ptr_long
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      LOG_CLI((BSL_META_U(unit,"ptr_long--> %u)"), ptr_long ));
    }
    LOG_CLI((BSL_META_U(unit,"\n")));
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_print()",0,0);
}


uint32
  soc_sand_SAND_HASH_TABLE_INFO_print(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR hash_table
  )
{
  uint32
    res,
    data_size,
    key_size,
    table_size,
    table_width,
    hash_table_index ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  res = HASH_TABLE_ACCESS_INFO.data_size.get(unit, hash_table_index, &data_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  res = HASH_TABLE_ACCESS_INFO.key_size.get(unit, hash_table_index, &key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit) ;
  res = HASH_TABLE_ACCESS_INFO.table_size.get(unit, hash_table_index, &table_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit) ;
  res = HASH_TABLE_ACCESS_INFO.table_width.get(unit, hash_table_index, &table_width) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit) ;

  LOG_CLI((BSL_META_U(unit,"init_info.data_size  : %u\n"),data_size)) ;
  LOG_CLI((BSL_META_U(unit,"init_info.key_size   : %u\n"),key_size)) ;
  LOG_CLI((BSL_META_U(unit,"init_info.table_size : %u\n"),table_size)) ;
  LOG_CLI((BSL_META_U(unit,"init_info.table_width: %u\n"),table_width)) ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_SAND_HASH_TABLE_INFO_print()", 0, 0);
}



#endif 



STATIC uint32
  soc_sand_hash_table_find_entry(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR      hash_table,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_KEY      *const key,
    SOC_SAND_IN     uint8                        get_first_empty,
    SOC_SAND_IN     uint8                        alloc_by_index,
    SOC_SAND_OUT    uint32                       *entry,
    SOC_SAND_OUT    uint8                        *found,
    SOC_SAND_OUT    uint32                       *prev_entry,
    SOC_SAND_OUT    uint8                        *first
  )
{
  uint8
    *cur_key,
    *next;
  uint32
    key_size,
    ptr_size,
    null_ptr,
    hash_val,
    ptr_long,
    next_node;
  uint8
    not_found,
    found_new;
  uint32
    res,
    hash_table_index ;
  uint8
    *tmp_buf_ptr ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_FIND_ENTRY);
  SOC_SAND_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_HASH_TABLE_CONVERT_HANDLE_TO_HASHTABLE_INDEX(hash_table_index,hash_table) ;
  SOC_SAND_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(hash_table);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = HASH_TABLE_ACCESS_DATA.ptr_size.get(unit, hash_table_index, &ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = HASH_TABLE_ACCESS_INFO.key_size.get(unit, hash_table_index, &key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit) ;
  res = HASH_TABLE_ACCESS_DATA.null_ptr.get(unit, hash_table_index, &null_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  res = HASH_TABLE_ACCESS_DATA.memory_use.get(unit,hash_table_index,&memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit) ;

  ptr_long = 0 ;
  *prev_entry = SOC_SAND_HASH_TABLE_NULL ;
  *first = TRUE ;
  
  res = soc_sand_hash_table_simple_hash(
      unit,
      hash_table,
      key,
      0,
      &hash_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  *prev_entry = hash_val;
  
  res = soc_sand_hash_table_get_tmp_data_ptr_from_handle(unit,hash_table,&tmp_buf_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
  
  res = HASH_TABLE_ACCESS_DATA.lists_head.memread(unit,hash_table_index,tmp_buf_ptr,ptr_size * hash_val,ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  
  ptr_long = 0;
  res = soc_sand_U8_to_U32(
                       tmp_buf_ptr,
                       ptr_size,
                       &ptr_long
                       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  if (ptr_long == null_ptr)
  {
    if (get_first_empty)
    {
      if (alloc_by_index)
      {
        
        res = soc_sand_occ_bm_occup_status_set(
                                               unit,
                                               memory_use,
                                               *entry,
                                               TRUE
                                               );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        found_new = TRUE;

        ptr_long = *entry;
      }
      else
      {
        res = soc_sand_occ_bm_alloc_next(
                                         unit,
                                         memory_use,
                                         &ptr_long,
                                         &found_new
                                         );
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      }
      if (!found_new)
      {
        ptr_long = SOC_SAND_HASH_TABLE_NULL;
        *entry = SOC_SAND_HASH_TABLE_NULL;
      }
      else
      {
        
        res = soc_sand_U32_to_U8(
                                 &ptr_long,
                                 ptr_size,
                                 tmp_buf_ptr
                                 );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
        
        res = HASH_TABLE_ACCESS_DATA.lists_head.memwrite(unit,hash_table_index,tmp_buf_ptr,ptr_size * hash_val,ptr_size) ;
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit) ;
        *entry = ptr_long ;
      }
      *found = FALSE ;
      goto exit ;
    }
    *found = FALSE ;
    *entry = SOC_SAND_HASH_TABLE_NULL ;
    goto exit ;
  }
  not_found = TRUE ;


  while (ptr_long != null_ptr)
  {
    
    
    res = HASH_TABLE_ACCESS_DATA.keys.memread(unit,hash_table_index,tmp_buf_ptr,key_size * ptr_long,key_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 74, exit) ;
    cur_key = tmp_buf_ptr ;
    not_found =
      (uint8)soc_sand_os_memcmp(
        cur_key,
        key,
        (key_size * sizeof(SOC_SAND_HASH_TABLE_KEY_TYPE))
      ) ;
    if (not_found == FALSE)
    {
      
      *found = TRUE ;
      *entry = ptr_long ;
      goto exit ;
    }
    
    *first = FALSE ;
    
    
    res = HASH_TABLE_ACCESS_DATA.next.memread(unit,hash_table_index,tmp_buf_ptr,ptr_size * ptr_long,ptr_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 76, exit) ;
    next = tmp_buf_ptr ;
    *prev_entry = ptr_long ;
    ptr_long = 0 ;
    res = soc_sand_U8_to_U32(
                         next,
                         ptr_size,
                         &ptr_long
                         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }
  
  *found = FALSE;
  
  if (get_first_empty)
  {
    if (alloc_by_index)
    {
      
      res = soc_sand_occ_bm_occup_status_set(
                                           unit,
                                           memory_use,
                                           *entry,
                                           TRUE
                                           );
      SOC_SAND_CHECK_FUNC_RESULT(res, 84, exit);
      found_new = TRUE;
      next_node = *entry;
    }
    else
    {
      res = soc_sand_occ_bm_alloc_next(
                                     unit,
                                     memory_use,
                                     &next_node,
                                     &found_new
                                     );
      SOC_SAND_CHECK_FUNC_RESULT(res, 88, exit);
    }
    if (!found_new)
    {
      *entry = SOC_SAND_HASH_TABLE_NULL ;
    }
    else
    {
      res = soc_sand_U32_to_U8(
                             &next_node,
                             ptr_size,
                             tmp_buf_ptr
                             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 92, exit);
      
      res = HASH_TABLE_ACCESS_DATA.next.memwrite(unit,hash_table_index,tmp_buf_ptr,ptr_size * (*prev_entry),ptr_size) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 96, exit) ;
      *entry = next_node ;
    }
  }
  else
  {
    *entry = SOC_SAND_HASH_TABLE_NULL ;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_find_entry()",0,0) ;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

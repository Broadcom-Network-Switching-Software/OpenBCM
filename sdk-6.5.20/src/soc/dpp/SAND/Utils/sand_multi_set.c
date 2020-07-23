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

#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/drv.h>


extern shr_sw_state_t *sw_state[BCM_MAX_NUM_UNITS];



#define SOC_SAND_MULTI_SET_HASH_WIDTH_FACTOR 1





#define MULTI_SET_ACCESS          sw_state_access[unit].dpp.soc.sand.multi_set  
#define MULTI_SET_ACCESS_DATA     MULTI_SET_ACCESS.multis_array.multiset_data
#define MULTI_SET_ACCESS_INFO     MULTI_SET_ACCESS.multis_array.init_info

#define SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(_multi_set_index,_err1,_err2) \
  { \
    uint8 bit_val ; \
    uint32 max_nof_multis ; \
    res = MULTI_SET_ACCESS.max_nof_multis.get(unit, &max_nof_multis) ; \
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err1, exit) ; \
    if (_multi_set_index >= max_nof_multis) \
    { \
       \
      bit_val = 0 ; \
    } \
    else \
    { \
      res = MULTI_SET_ACCESS.occupied_multis.bit_get(unit, (int)_multi_set_index, &bit_val) ; \
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err1, exit) ; \
    } \
    if (bit_val == 0) \
    { \
       \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_FREE_FAIL, _err2, exit) ; \
    } \
  }

#define SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, _err1) \
  if ((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES)) \
  { \
     \
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MAX_NUM_DEVICES_OUT_OF_RANGE_ERR, _err1, exit); \
  }

#define SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(_multi_set_index,_handle) (_multi_set_index = _handle - 1)
#define SOC_SAND_MULTI_SET_CONVERT_MULTISET_INDEX_TO_HANDLE(_handle,_multi_set_index) (_handle = _multi_set_index + 1)





 

 
 

 

 
 






uint32
  soc_sand_multi_set_get_illegal_multiset_handle(
    void
  )
{
  return ((uint32)(-1)) ;
}

uint32
  soc_sand_multi_set_get_member_size(
    SOC_SAND_IN    int                              unit,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_PTR           multi_set,
    SOC_SAND_INOUT uint32                           *member_size_ptr
  )
{
  uint32
    multi_set_index,
    res,
    member_size ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;

  res = MULTI_SET_ACCESS_INFO.member_size.get(unit, multi_set_index, &member_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  *member_size_ptr = member_size ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_get_member_size()",0,0);
}

uint32
  soc_sand_multi_set_default_get_entry(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_IN  uint8                             *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_OUT uint8                             *data
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_default_get_entry()",0,0);
}

uint32
  soc_sand_multi_set_default_set_entry(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_INOUT  uint8                          *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_IN  uint8                             *data
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_default_set_entry()",0,0);
}

STATIC uint32
  soc_sand_multi_set_member_add_internal(
    SOC_SAND_IN    int                       unit,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_PTR    multi_set,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_KEY    *const key,
    SOC_SAND_INOUT uint32                    *data_indx,
    SOC_SAND_IN    uint32                    nof_additions,
    SOC_SAND_OUT   uint8                     *first_appear,
    SOC_SAND_OUT   uint8                     *success
  ) ;


uint32
  soc_sand_multi_set_init(
    SOC_SAND_IN       int                          unit,
    SOC_SAND_IN       uint32                       max_nof_multis
  )
{
  uint32 res ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_INIT) ;
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;

  res = MULTI_SET_ACCESS.alloc(unit);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = MULTI_SET_ACCESS.multis_array.ptr_alloc(unit, max_nof_multis);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  res = MULTI_SET_ACCESS.max_nof_multis.set(unit, max_nof_multis);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  res = MULTI_SET_ACCESS.in_use.set(unit, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  res = MULTI_SET_ACCESS.occupied_multis.alloc_bitmap(unit, max_nof_multis);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_init()",0,0);
}


uint32
  soc_sand_multi_set_create(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_PTR        *multi_set_ptr,
    SOC_SAND_IN     SOC_SAND_MULTI_SET_INIT_INFO  init_info
  )
{
  SOC_SAND_HASH_TABLE_INIT_INFO
    hash_init ;
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint32
    res,
    multi_set_index,
    max_duplications,
    found,
    max_nof_multis ;
  uint32
    in_use ;
  uint8
    bit_val ;
  int
    prime_handle ;
  uint32
    member_size,
    sec_handle,
    nof_members ;
  SOC_SAND_MULTISET_SW_DB_ENTRY_GET
    get_entry_fun ;
  SOC_SAND_MULTISET_SW_DB_ENTRY_SET
    set_entry_fun ;
  uint32
    counter_size,
    global_counter ;
  SOC_SAND_HASH_TABLE_PTR
    hash_table ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_CREATE);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;

  SOC_SAND_CHECK_NULL_INPUT(multi_set_ptr);

  res = MULTI_SET_ACCESS.in_use.get(unit, &in_use);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);

  res = MULTI_SET_ACCESS.max_nof_multis.get(unit, &max_nof_multis);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);

  if (in_use >= max_nof_multis)
  {
    
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 8, exit);
  }
  
  res = MULTI_SET_ACCESS.in_use.set(unit, (in_use + 1));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  
  found = 0 ;
  for (multi_set_index = 0 ; multi_set_index < max_nof_multis ; multi_set_index++)
  {
    res = MULTI_SET_ACCESS.occupied_multis.bit_get(unit, multi_set_index, &bit_val);
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
  res = MULTI_SET_ACCESS.occupied_multis.bit_set(unit, multi_set_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);

  res = MULTI_SET_ACCESS.multis_array.alloc(unit, multi_set_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);
  
  SOC_SAND_MULTI_SET_CONVERT_MULTISET_INDEX_TO_HANDLE(multi_set,multi_set_index) ;
  
  *multi_set_ptr = multi_set ;
  
  if (init_info.get_entry_fun != NULL || init_info.set_entry_fun != NULL )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 22, exit);
  }
  if (init_info.nof_members == 0 || init_info.member_size == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 26, exit);
  }
  res = MULTI_SET_ACCESS_INFO.set(unit, multi_set_index, &init_info) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit) ;
  res = MULTI_SET_ACCESS_INFO.max_duplications.get(unit, multi_set_index, &max_duplications) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 34, exit) ;
  if (max_duplications == SOC_SAND_U32_MAX)
  {
    max_duplications = SOC_SAND_U32_MAX - 1;
    res = MULTI_SET_ACCESS_INFO.max_duplications.set(unit, multi_set_index, max_duplications) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 34, exit) ;
  }
  counter_size = 0 ;
  hash_table = soc_sand_hash_table_get_illegal_hashtable_handle() ;

  res = MULTI_SET_ACCESS_DATA.counter_size.set(unit, multi_set_index, counter_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 38, exit) ;
  res = MULTI_SET_ACCESS_DATA.hash_table.set(unit, multi_set_index, hash_table) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit) ;
  
  soc_sand_os_memset(&hash_init, 0x0, sizeof(SOC_SAND_HASH_TABLE_INIT_INFO));
  res = MULTI_SET_ACCESS_INFO.member_size.get(unit, multi_set_index, &member_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 42, exit) ;
  res = MULTI_SET_ACCESS_INFO.prime_handle.get(unit, multi_set_index, &prime_handle) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 44, exit) ;
  res = MULTI_SET_ACCESS_INFO.sec_handle.get(unit, multi_set_index, &sec_handle) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 46, exit) ;
  res = MULTI_SET_ACCESS_INFO.nof_members.get(unit, multi_set_index, &nof_members) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit) ;
  res = MULTI_SET_ACCESS_INFO.get_entry_fun.get(unit, multi_set_index, &get_entry_fun) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 54, exit) ;
  res = MULTI_SET_ACCESS_INFO.set_entry_fun.get(unit, multi_set_index, &set_entry_fun) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 58, exit) ;
  

  hash_init.data_size = member_size ;
  hash_init.key_size = member_size ;
  hash_init.prime_handle = prime_handle ;
  hash_init.sec_handle = sec_handle ;
  hash_init.table_size = nof_members ;
  hash_init.table_width = nof_members * SOC_SAND_MULTI_SET_HASH_WIDTH_FACTOR ;
  hash_init.get_entry_fun = get_entry_fun ;
  hash_init.set_entry_fun = set_entry_fun ;

  global_counter = 0 ;
  res = MULTI_SET_ACCESS_DATA.global_counter.set(unit, multi_set_index, global_counter) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 66, exit) ;
  res = soc_sand_hash_table_create(
            unit,
            &hash_table,
            hash_init
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit) ;
  res = MULTI_SET_ACCESS_DATA.hash_table.set(unit, multi_set_index, hash_table) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 71, exit) ;
  
  if (max_duplications > 1)
  {
    
    counter_size = (soc_sand_log2_round_up(max_duplications+1) + (SOC_SAND_NOF_BITS_IN_BYTE - 1)) / SOC_SAND_NOF_BITS_IN_BYTE;
    res = MULTI_SET_ACCESS_DATA.counter_size.set(unit, multi_set_index, counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 72, exit) ;
    res = MULTI_SET_ACCESS_DATA.ref_counter.alloc(unit, multi_set_index, nof_members * counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 74, exit) ;
    
    res = MULTI_SET_ACCESS_DATA.ref_counter.memset(unit, multi_set_index, 0, nof_members * counter_size, 0x00) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 78, exit) ;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_create()",0,0);
}


uint32
  soc_sand_multi_set_destroy(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     SOC_SAND_MULTI_SET_PTR  multi_set
  )
{
  uint32
    multi_set_index,
    res ;
  uint8
    bit_val ;
  uint32
    in_use ;
  SOC_SAND_HASH_TABLE_PTR
    hash_table ;
  uint32
    max_duplications,
    global_counter ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_DESTROY);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;
  
  res = MULTI_SET_ACCESS.in_use.get(unit, &in_use);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
  if ((int)in_use < 0)
  {
    
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_FREE_FAIL, 12, exit);
  }
  
  res = MULTI_SET_ACCESS.in_use.set(unit, (in_use - 1));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit) ;
  
  res = MULTI_SET_ACCESS.occupied_multis.bit_get(unit, multi_set_index, &bit_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  if (bit_val == 0)
  {
    
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_FREE_FAIL, 24, exit) ;
  }
  res = MULTI_SET_ACCESS.occupied_multis.bit_clear(unit, multi_set_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);

  res = MULTI_SET_ACCESS_DATA.hash_table.get(unit, multi_set_index, &hash_table) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit) ;
  res = soc_sand_hash_table_destroy(
          unit,
          hash_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit) ;
  res = MULTI_SET_ACCESS_INFO.max_duplications.get(unit, multi_set_index, &max_duplications) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit) ;
  
  if (max_duplications > 1)
  {
    res = MULTI_SET_ACCESS_DATA.ref_counter.free(unit, multi_set_index) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 44, exit) ;
  }
  global_counter = 0 ;
  res = MULTI_SET_ACCESS_DATA.global_counter.set(unit, multi_set_index, global_counter) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 48, exit) ;
  res = MULTI_SET_ACCESS.multis_array.free(unit,multi_set_index) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 52, exit) ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_destroy()",0,0);
}


uint32
  soc_sand_multi_set_member_add(
    SOC_SAND_IN    int                      unit,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_PTR   multi_set,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_KEY   *const key,
    SOC_SAND_OUT   uint32                   *data_indx,
    SOC_SAND_OUT   uint8                    *first_appear,
    SOC_SAND_OUT   uint8                    *success
  )
{
  int nof_additions ;
  uint32
    multi_set_index,
    res ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_MEMBER_ADD);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;

  nof_additions = 1 ;
  *data_indx = SOC_SAND_U32_MAX ;  
  res = soc_sand_multi_set_member_add_internal(
                unit,
                multi_set,
                key,
                data_indx,
                nof_additions,
                first_appear,
                success);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_add()",0,0);
}


uint32
  soc_sand_multi_set_member_add_at_index(
    SOC_SAND_IN    int                         unit,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_PTR      multi_set,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_KEY      *const key,
    SOC_SAND_IN    uint32                      data_indx,
    SOC_SAND_OUT   uint8                       *first_appear,
    SOC_SAND_OUT   uint8                       *success
  )
{
  uint32
    multi_set_index,
    res;
  const int nof_additions = 1 ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_MEMBER_ADD);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;

  res = soc_sand_multi_set_member_add_at_index_nof_additions(
                unit,
                multi_set,
                key,
                data_indx,
                nof_additions,
                first_appear,
                success);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_add()",0,0);
}


uint32
  soc_sand_multi_set_member_add_at_index_nof_additions(
    SOC_SAND_IN    int                         unit,
    SOC_SAND_INOUT SOC_SAND_MULTI_SET_PTR      multi_set,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_KEY      *const key,
    SOC_SAND_IN    uint32                      data_indx,
    SOC_SAND_IN    uint32                      nof_additions,
    SOC_SAND_OUT   uint8                       *first_appear,
    SOC_SAND_OUT   uint8                       *success
  )
{
  uint32
    multi_set_index,
    res;
  uint32
    data_index_ptr ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_MEMBER_ADD) ;
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;

  data_index_ptr = data_indx ;
  res = soc_sand_multi_set_member_add_internal(
              unit,
              multi_set,
              key,
              &data_index_ptr,
              nof_additions,
              first_appear,
              success);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_add_at_index_ref_count()",0,0);
}


STATIC uint32
  soc_sand_multi_set_member_add_internal(
    SOC_SAND_IN    int                       unit,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_PTR    multi_set,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_KEY    *const key,
    SOC_SAND_INOUT uint32                    *data_indx,
    SOC_SAND_IN    uint32                    nof_additions,
    SOC_SAND_OUT   uint8                     *first_appear,
    SOC_SAND_OUT   uint8                     *success
  )
{
  uint8
    tmp_cnt[sizeof(uint32)];
  uint32
    max_duplications,
    ref_count,
    counter_size,
    global_counter,
    global_max,
    found_index, 
    adjusted_additions ;
  uint8
    exist,
    with_id ;
  uint32
    multi_set_index,
    res;
  SOC_SAND_HASH_TABLE_PTR
    hash_table ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_MEMBER_ADD);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(data_indx);
  SOC_SAND_CHECK_NULL_INPUT(first_appear);
  SOC_SAND_CHECK_NULL_INPUT(success);

  with_id = (*data_indx != SOC_SAND_U32_MAX) ;
  if (nof_additions == 0)
  {
    *success = FALSE ;
    goto exit ;
  }

  
  res = MULTI_SET_ACCESS_INFO.max_duplications.get(unit, multi_set_index, &max_duplications) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = MULTI_SET_ACCESS_DATA.hash_table.get(unit, multi_set_index, &hash_table) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  if (max_duplications <= 1)
  {
    res = soc_sand_hash_table_entry_lookup(
            unit,
            hash_table,
            key,
            &found_index,
            &exist
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
    if (with_id)
    {
       
      if (exist && found_index != *data_indx)
      {
        *first_appear = FALSE;
        *success = FALSE;
        goto exit;
      }
    }
    else 
    {
      
      *data_indx = found_index;
    }
    if (exist && found_index != SOC_SAND_HASH_TABLE_NULL)
    {
      *first_appear = FALSE;
      *success = TRUE;
      goto exit;
    }
  }

  
  if (with_id) {
      found_index = *data_indx;

      res = soc_sand_hash_table_entry_add_at_index(
              unit,
              hash_table,
              key,
              found_index,
              success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
  }
  else
  {
    res = soc_sand_hash_table_entry_add(
              unit,
              hash_table,
              key,
              &found_index,
              success
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);
  }

  
  if (max_duplications <= 1)
  {
    *first_appear = TRUE;
    goto exit;
  }
  if(!*success || found_index == SOC_SAND_HASH_TABLE_NULL)
  {
      *success = FALSE;
      *first_appear = (with_id) ? FALSE : TRUE; 
      goto exit;
  }
  
  
  *data_indx = found_index;
  res = MULTI_SET_ACCESS_DATA.counter_size.get(unit, multi_set_index, &counter_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit) ;
  
  res = MULTI_SET_ACCESS_DATA.ref_counter.memread(unit,multi_set_index,tmp_cnt,(*data_indx) * counter_size, counter_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 34, exit) ;
  ref_count = 0;
  res = soc_sand_U8_to_U32(
          tmp_cnt,
          counter_size,
          &ref_count
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  
  if (nof_additions == SOC_SAND_U32_MAX) {
      adjusted_additions  = (max_duplications - ref_count);
  }
  else
  {
      adjusted_additions = nof_additions;
  }
  
  if ((ref_count + adjusted_additions) > SOC_SAND_BITS_MASK((SOC_SAND_NOF_BITS_IN_BYTE * counter_size - 1),0))
  {
    
    *success = FALSE;
    goto exit;
  }
  if ((ref_count + adjusted_additions) > max_duplications) {    
      *success = FALSE;
      goto exit;
  }
  res = MULTI_SET_ACCESS_DATA.global_counter.get(unit, multi_set_index, &global_counter) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 42, exit) ;
  
  global_max = 0 ;
  res = MULTI_SET_ACCESS_INFO.global_max.get(unit, multi_set_index, &global_max) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 44, exit) ;
  if ((int)global_max > 0)
  {
    if ((global_counter + adjusted_additions) > global_max)
    {
      
      res = _SHR_E_FULL ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit) ;
    }
  }
  if (ref_count == 0)
  {
    *first_appear = TRUE;
  }
  else
  {
    *first_appear = FALSE;
  }
  ref_count += adjusted_additions ;

  
  global_counter += adjusted_additions;
  res = MULTI_SET_ACCESS_DATA.global_counter.set(unit, multi_set_index, global_counter) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 46, exit) ;
  res = soc_sand_U32_to_U8(
          &ref_count,
          counter_size,
          tmp_cnt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  
  res = MULTI_SET_ACCESS_DATA.ref_counter.memwrite(unit,multi_set_index,tmp_cnt,(*data_indx) * counter_size, counter_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 54, exit) ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_add_at_index_ref_count()",0,0);
}


uint32
  soc_sand_multi_set_member_remove(
    SOC_SAND_IN    int                      unit,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_PTR   multi_set,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_KEY   *const key,
    SOC_SAND_OUT   uint32                   *data_indx,
    SOC_SAND_OUT   uint8                    *last_appear
  )
{
  uint8
    found ;
  uint32
    multi_set_index,
    res ;
  SOC_SAND_HASH_TABLE_PTR
    hash_table ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_MEMBER_REMOVE);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(key);
  res = MULTI_SET_ACCESS_DATA.hash_table.get(unit, multi_set_index, &hash_table) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  res = soc_sand_hash_table_entry_lookup(
          unit,
          hash_table,
          key,
          data_indx,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
  if (!found)
  {
    *data_indx = SOC_SAND_MULTI_SET_NULL ;
    *last_appear = FALSE ;
    goto exit ;
  }
  res = soc_sand_multi_set_member_remove_by_index_multiple(unit, multi_set, *data_indx, 1, last_appear);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_remove()",0,0);
}


uint32
  soc_sand_multi_set_member_remove_by_index(
    SOC_SAND_IN    int                       unit,
    SOC_SAND_INOUT SOC_SAND_MULTI_SET_PTR    multi_set,
    SOC_SAND_IN    uint32                    data_indx,
    SOC_SAND_OUT   uint8                     *last_appear
  )
{
  uint32
    multi_set_index,
    res ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_MEMBER_REMOVE_BY_INDEX);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;
  res = soc_sand_multi_set_member_remove_by_index_multiple(unit, multi_set, data_indx, 1, last_appear);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_remove_by_index()",0,0);
}


uint32
  soc_sand_multi_set_member_remove_by_index_multiple(
    SOC_SAND_IN    int                       unit,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_PTR    multi_set,
    SOC_SAND_IN    uint32                    data_indx,
    SOC_SAND_IN    uint32                    remove_amount,
    SOC_SAND_OUT   uint8                     *last_appear
  )
{
  uint32
    max_duplications,
    counter_size,
    global_counter,
    ref_count,
    adjusted_remove_amount ;
  uint8
    tmp_cnt[sizeof(uint32)] ;
  uint32
    multi_set_index,
    res ;
  SOC_SAND_HASH_TABLE_PTR
    hash_table ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_MEMBER_REMOVE_BY_INDEX);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(last_appear);
  ref_count = 0;
  res = MULTI_SET_ACCESS_INFO.max_duplications.get(unit, multi_set_index, &max_duplications) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = MULTI_SET_ACCESS_DATA.counter_size.get(unit, multi_set_index, &counter_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit) ;
  if (max_duplications > 1)
  {
    
    res = MULTI_SET_ACCESS_DATA.ref_counter.memread(unit,multi_set_index,tmp_cnt,data_indx * counter_size, counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 34, exit) ;
    soc_sand_U8_to_U32(
      tmp_cnt,
      counter_size,
      &ref_count
    );
    if (ref_count == 0)
    {
      *last_appear = TRUE;
      goto exit;
    }

    
    if ((remove_amount > ref_count))
    {
      adjusted_remove_amount = ref_count;
    }
    else
    {
      adjusted_remove_amount = remove_amount;
    }
    res = MULTI_SET_ACCESS_DATA.global_counter.get(unit, multi_set_index, &global_counter) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 38, exit) ;
    global_counter -= adjusted_remove_amount ;
    res = MULTI_SET_ACCESS_DATA.global_counter.set(unit, multi_set_index, global_counter) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 42, exit) ;
    ref_count -= adjusted_remove_amount;
    *last_appear = (ref_count == 0) ? TRUE : FALSE;
    res = soc_sand_U32_to_U8(
            &ref_count,
            counter_size,
            tmp_cnt
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);
    
    res = MULTI_SET_ACCESS_DATA.ref_counter.memwrite(unit,multi_set_index,tmp_cnt,data_indx * counter_size, counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 54, exit) ;
  } else {
      *last_appear = TRUE;
  }

  if (*last_appear || max_duplications <= 1)
  {
    res = MULTI_SET_ACCESS_DATA.hash_table.get(unit, multi_set_index, &hash_table) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 58, exit) ;
    res = soc_sand_hash_table_entry_remove_by_index(
            unit,
            hash_table,
            data_indx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_remove_by_index()",0,0);
}


uint32
  soc_sand_multi_set_member_lookup(
    SOC_SAND_IN    int                     unit,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_PTR  multi_set,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_KEY  *const key,
    SOC_SAND_OUT   uint32                  *data_indx,
    SOC_SAND_OUT   uint32                  *ref_count
  )
{
  uint8
    tmp_cnt[sizeof(uint32)];
  uint8
    found ;
  uint32
    ref_count_lcl[1];
  uint32
    max_duplications,
    counter_size ;
  uint32
    multi_set_index,
    res ;
  SOC_SAND_HASH_TABLE_PTR
    hash_table ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_MEMBER_LOOKUP) ;
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(key) ;
  SOC_SAND_CHECK_NULL_INPUT(data_indx) ;

  res = MULTI_SET_ACCESS_DATA.hash_table.get(unit, multi_set_index, &hash_table) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = soc_sand_hash_table_entry_lookup(
          unit,
          hash_table,
          key,
          data_indx,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
  if (!found)
  {
    *ref_count = 0;
    goto exit;
  }
  res = MULTI_SET_ACCESS_INFO.max_duplications.get(unit, multi_set_index, &max_duplications) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  if (max_duplications > 1)
  {
    *ref_count_lcl = 0;
    res = MULTI_SET_ACCESS_DATA.counter_size.get(unit, multi_set_index, &counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
    
    res = MULTI_SET_ACCESS_DATA.ref_counter.memread(unit,multi_set_index,tmp_cnt,(*data_indx) * counter_size, counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
    res = soc_sand_U8_to_U32(
            tmp_cnt,
            counter_size,
            ref_count_lcl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    *ref_count = *ref_count_lcl ;
  }
  else
  {
    *ref_count = 1 ;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_lookup()",0,0);
}


uint32
  soc_sand_multi_set_get_next(
    SOC_SAND_IN    int                      unit,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_PTR   multi_set,
    SOC_SAND_INOUT SOC_SAND_MULTI_SET_ITER  *iter,
    SOC_SAND_OUT   SOC_SAND_MULTI_SET_KEY   *key,
    SOC_SAND_OUT   uint32                   *data_indx,
    SOC_SAND_OUT   uint32                   *ref_count
  )
{
  uint32
    counter_size,
    max_duplications,
    tmp_ref_count;
  uint8
    tmp_cnt[sizeof(uint32)];
  uint32
    multi_set_index,
    res ;
  SOC_SAND_HASH_TABLE_PTR
    hash_table ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_MEMBER_LOOKUP);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;
  
  if (SOC_SAND_HASH_TABLE_ITER_IS_END(iter))
  {
    goto exit ;
  }
  res = MULTI_SET_ACCESS_DATA.hash_table.get(unit, multi_set_index, &hash_table) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = soc_sand_hash_table_get_next(
          unit,
          hash_table,
          iter,
          key,
          data_indx
        ) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (SOC_SAND_HASH_TABLE_ITER_IS_END(iter))
  {
    goto exit ;
  }
  tmp_ref_count = 1;
  res = MULTI_SET_ACCESS_INFO.max_duplications.get(unit, multi_set_index, &max_duplications) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  if (max_duplications > 1)
  {
    tmp_ref_count = 0;
    res = MULTI_SET_ACCESS_DATA.counter_size.get(unit, multi_set_index, &counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
    
    res = MULTI_SET_ACCESS_DATA.ref_counter.memread(unit,multi_set_index,tmp_cnt,(*data_indx) * counter_size, counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
    res = soc_sand_U8_to_U32(
           tmp_cnt,
           counter_size,
           &tmp_ref_count
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  *ref_count = tmp_ref_count;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_get_next()",0,0);
}


uint32
  soc_sand_multi_set_get_by_index(
    SOC_SAND_IN   int                      unit,
    SOC_SAND_IN   SOC_SAND_MULTI_SET_PTR   multi_set,
    SOC_SAND_IN   uint32                   data_indx,
    SOC_SAND_OUT  SOC_SAND_MULTI_SET_KEY   *key,
    SOC_SAND_OUT  uint32                   *ref_count
  )
{
  uint32
    counter_size,
    max_duplications,
    tmp_ref_count;
  uint8
    found;
  uint8
    tmp_cnt[sizeof(uint32)];
  uint32
    multi_set_index,
    res ;
  SOC_SAND_HASH_TABLE_PTR
    hash_table ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_MEMBER_LOOKUP);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;

  res = MULTI_SET_ACCESS_DATA.hash_table.get(unit, multi_set_index, &hash_table) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = soc_sand_hash_table_get_by_index(
          unit,
          hash_table,
          data_indx,
          key,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
  if (!found)
  {
    *ref_count = 0;
    goto exit;
  }
  tmp_ref_count = 1;
  res = MULTI_SET_ACCESS_INFO.max_duplications.get(unit, multi_set_index, &max_duplications) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  if (max_duplications > 1)
  {
    res = MULTI_SET_ACCESS_DATA.counter_size.get(unit, multi_set_index, &counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
    tmp_ref_count = 0 ;
    
    res = MULTI_SET_ACCESS_DATA.ref_counter.memread(unit,multi_set_index,tmp_cnt,data_indx * counter_size, counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
    res = soc_sand_U8_to_U32(
           tmp_cnt,
           counter_size,
           &tmp_ref_count
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  *ref_count = tmp_ref_count;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_get_next()",0,0);
}

uint32
  soc_sand_multi_set_clear(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_SAND_MULTI_SET_PTR   multi_set
  )
{
  uint32
    nof_members,
    counter_size,
    max_duplications ;
  uint32
    multi_set_index,
    res ;
  SOC_SAND_HASH_TABLE_PTR
    hash_table ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_CREATE);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;
  res = MULTI_SET_ACCESS_DATA.hash_table.get(unit, multi_set_index, &hash_table) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  
  res = soc_sand_hash_table_clear(
            unit,
            hash_table
          ) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit) ;
  
  res = MULTI_SET_ACCESS_INFO.max_duplications.get(unit, multi_set_index, &max_duplications) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  if (max_duplications > 1)
  {
    
    counter_size = (soc_sand_log2_round_up(max_duplications+1) + (SOC_SAND_NOF_BITS_IN_BYTE - 1)) / SOC_SAND_NOF_BITS_IN_BYTE;
    res = MULTI_SET_ACCESS_DATA.counter_size.set(unit, multi_set_index, counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
    res = MULTI_SET_ACCESS_INFO.nof_members.get(unit, multi_set_index, &nof_members) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
    
    res = MULTI_SET_ACCESS_DATA.ref_counter.memset(unit,multi_set_index,0,nof_members * counter_size, 0x00) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_clear()",0,0);
}

#if (0)

  
void
soc_sand_SAND_MULTI_SET_INFO_clear(
  SOC_SAND_INOUT SOC_SAND_MULTI_SET_INIT_INFO *init_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(*info));

  init_info->max_duplications = 1;
  init_info->get_entry_fun    = 0;
  init_info->set_entry_fun    = 0;
  init_info->nof_members      = 0;
  init_info->member_size      = 0;
  init_info->prime_handle     = 0;
  init_info->sec_handle       = 0;

  info->multiset_data.counter_size = 0;
  info->multiset_data.ref_counter  = 0;

  info->multiset_data.hash_table = soc_sand_hash_table_get_illegal_hashtable_handle() ;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif

#ifdef SOC_SAND_DEBUG



uint32
  soc_sand_multi_set_print(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_MULTI_SET_PTR       multi_set,
    SOC_SAND_IN  SOC_SAND_MULTI_SET_PRINT_VAL print_fun,
    SOC_SAND_IN  uint8                        clear_on_print
  )
{
  uint32
    data_indx ;
  SOC_SAND_HASH_TABLE_ITER
    iter ;
  uint8
    key[100] ;
  uint32
    max_duplications,
    counter_size,
    ref_count,
    org_ref_count ;
  uint8
    tmp_cnt[sizeof(uint32)] ;
  uint32
    multi_set_index,
    res ;
  SOC_SAND_HASH_TABLE_PTR
    hash_table ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_PRINT);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;
  res = MULTI_SET_ACCESS_DATA.hash_table.get(unit, multi_set_index, &hash_table) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  
  SOC_SAND_HASH_TABLE_ITER_SET_BEGIN(&iter);

  while (!SOC_SAND_HASH_TABLE_ITER_IS_END(&iter))
  {
    res = soc_sand_hash_table_get_next(
                                       unit,
                                       hash_table,
                                       &iter,
                                       key,
                                       &data_indx
                                       );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (SOC_SAND_HASH_TABLE_ITER_IS_END(&iter))
    {
      goto exit;
    }
    org_ref_count = 1;
    res = MULTI_SET_ACCESS_INFO.max_duplications.get(unit, multi_set_index, &max_duplications) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
    if (max_duplications > 1)
    {
      ref_count = 0 ;
      res = MULTI_SET_ACCESS_DATA.counter_size.get(unit, multi_set_index, &counter_size) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
      
      res = MULTI_SET_ACCESS_DATA.ref_counter.memread(unit,multi_set_index,tmp_cnt,data_indx * counter_size, counter_size) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
      res = soc_sand_U8_to_U32(
                               tmp_cnt,
                               counter_size,
                               &ref_count
                               );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      org_ref_count = ref_count ;
      if (clear_on_print)
      {
        ref_count = 0 ;
        res = soc_sand_U32_to_U8(
                                 &ref_count,
                                 counter_size,
                                 tmp_cnt
                                 );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        
        res = MULTI_SET_ACCESS_DATA.ref_counter.memwrite(unit,multi_set_index,tmp_cnt,data_indx * counter_size, counter_size) ;
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 44, exit) ;
      }
    }
    if (org_ref_count)
    {
      LOG_CLI((BSL_META_U(unit,"| %-8u|"), data_indx));
      print_fun(key) ;
      LOG_CLI((BSL_META_U(unit,"| %-8u|\n\r"), org_ref_count));
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_print()",0,0);
}

uint32
  soc_sand_multi_set_get_size_for_save(
    SOC_SAND_IN   int                         unit,
    SOC_SAND_IN   SOC_SAND_MULTI_SET_PTR      multi_set,
    SOC_SAND_OUT  uint32                      *size
  )
{
  uint32
    max_duplications,
    nof_members,
    counter_size,
    cur_size,
    total_size ;
  uint32
    multi_set_index,
    res ;
  SOC_SAND_HASH_TABLE_PTR
    hash_table ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;
  res = MULTI_SET_ACCESS_DATA.hash_table.get(unit, multi_set_index, &hash_table) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;

  total_size = 0 ;

  SOC_SAND_CHECK_NULL_INPUT(size);

  cur_size = sizeof(SOC_SAND_HASH_TABLE_INIT_INFO);
  total_size += cur_size;

  
  res = soc_sand_hash_table_get_size_for_save(
            unit,
            hash_table,
            &cur_size
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
  total_size += cur_size;
  
  res = MULTI_SET_ACCESS_INFO.max_duplications.get(unit, multi_set_index, &max_duplications) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  if (max_duplications > 1)
  {
    res = MULTI_SET_ACCESS_DATA.counter_size.get(unit, multi_set_index, &counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
    res = MULTI_SET_ACCESS_INFO.nof_members.get(unit, multi_set_index, &nof_members) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
    total_size += (nof_members * counter_size) ;
  }
  *size= total_size ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_get_size_for_save()",0,0);
}


uint32
  soc_sand_multi_set_save(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_SAND_MULTI_SET_PTR     multi_set,
    SOC_SAND_OUT uint8                      *buffer,
    SOC_SAND_IN  uint32                     buffer_size_bytes,
    SOC_SAND_OUT uint32                     *actual_size_bytes
  )
{
  uint8
    *cur_ptr ;
  uint32
    max_duplications,
    nof_members,
    counter_size,
    cur_size,
    total_size ;
  uint32
    multi_set_index,
    res ;
  SOC_SAND_HASH_TABLE_PTR
    hash_table ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;
  res = MULTI_SET_ACCESS_DATA.hash_table.get(unit, multi_set_index, &hash_table) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;

  SOC_SAND_CHECK_NULL_INPUT(actual_size_bytes) ;

  cur_ptr = (uint8*)buffer ;
  total_size = 0 ;

  
  
  res = MULTI_SET_ACCESS_INFO.get(unit,multi_set_index,(SOC_SAND_MULTI_SET_INIT_INFO *)cur_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  cur_ptr += sizeof(SOC_SAND_MULTI_SET_INIT_INFO) ;

  
  res = soc_sand_hash_table_save(
            unit,
            hash_table,
            cur_ptr,
            buffer_size_bytes - total_size,
            &cur_size
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  cur_ptr += cur_size ;
  total_size += cur_size ;
  
  res = MULTI_SET_ACCESS_INFO.max_duplications.get(unit, multi_set_index, &max_duplications) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  if (max_duplications > 1)
  {
    res = MULTI_SET_ACCESS_DATA.counter_size.get(unit, multi_set_index, &counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
    res = MULTI_SET_ACCESS_INFO.nof_members.get(unit, multi_set_index, &nof_members) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
    
    res = MULTI_SET_ACCESS_DATA.ref_counter.memread(unit,multi_set_index,cur_ptr, 0, nof_members * counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
    cur_ptr += (nof_members * counter_size) ;
    total_size += (nof_members * counter_size) ;
  }

  *actual_size_bytes = total_size ;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_save()",0,0);
}

uint32
  soc_sand_multi_set_load(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint8                                 **buffer,
    SOC_SAND_IN  SOC_SAND_MULTISET_SW_DB_ENTRY_SET     set_function,
    SOC_SAND_IN  SOC_SAND_MULTISET_SW_DB_ENTRY_GET     get_function,
    SOC_SAND_OUT SOC_SAND_MULTI_SET_PTR                *multi_set_ptr
  )
{
  SOC_SAND_IN uint8
    *cur_ptr ;
  uint32
    multi_set_index,
    res ;
  SOC_SAND_MULTI_SET_INIT_INFO
    init_info ;
  SOC_SAND_HASH_TABLE_PTR
    hash_table ;
  SOC_SAND_MULTI_SET_PTR
    multi_set ;
  uint32
    max_duplications,
    nof_members,
    counter_size ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;

  SOC_SAND_CHECK_NULL_INPUT(buffer);

  cur_ptr = (SOC_SAND_IN uint8*)buffer[0] ;

  
  soc_sand_os_memcpy(&(init_info), cur_ptr,sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
  cur_ptr += sizeof(SOC_SAND_MULTI_SET_INIT_INFO);
  init_info.set_entry_fun = set_function;
  init_info.get_entry_fun = get_function;

  
  res = soc_sand_multi_set_create(
          unit,
          multi_set_ptr,
          init_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20, exit);
  multi_set = *multi_set_ptr ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  res = MULTI_SET_ACCESS_DATA.hash_table.get(unit, multi_set_index, &hash_table) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = soc_sand_hash_table_destroy(
          unit,
          hash_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20, exit);

  
  res = soc_sand_hash_table_load(
            unit,
            &cur_ptr,
            set_function,
            get_function,
            NULL,
            NULL, 
            &(hash_table)
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  res = MULTI_SET_ACCESS_INFO.max_duplications.get(unit, multi_set_index, &max_duplications) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  if (max_duplications > 1)
  {
    res = MULTI_SET_ACCESS_DATA.counter_size.get(unit, multi_set_index, &counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
    res = MULTI_SET_ACCESS_INFO.nof_members.get(unit, multi_set_index, &nof_members) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
    
    res = MULTI_SET_ACCESS_DATA.ref_counter.memwrite(unit,multi_set_index,cur_ptr, 0, nof_members * counter_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
    cur_ptr += (nof_members * counter_size) ;
  }
  *buffer = cur_ptr ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_load()",0,0);
}

uint32
  soc_sand_SAND_MULTI_SET_INFO_print(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN SOC_SAND_MULTI_SET_PTR multi_set
  )
{
  uint32
    multi_set_index,
    res ;
  uint32
    max_duplications,
    nof_members,
    member_size,
    sec_handle,
    counter_size ;
  int
    prime_handle ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(multi_set_index,multi_set) ;
  SOC_SAND_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set_index,4,6) ;

  res = MULTI_SET_ACCESS_INFO.max_duplications.get(unit, multi_set_index, &max_duplications) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit) ;
  res = MULTI_SET_ACCESS_INFO.nof_members.get(unit, multi_set_index, &nof_members) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = MULTI_SET_ACCESS_INFO.member_size.get(unit, multi_set_index, &member_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit) ;
  res = MULTI_SET_ACCESS_DATA.counter_size.get(unit, multi_set_index, &counter_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  res = MULTI_SET_ACCESS_INFO.prime_handle.get(unit, multi_set_index, &prime_handle) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit) ;
  res = MULTI_SET_ACCESS_INFO.sec_handle.get(unit, multi_set_index, &sec_handle) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;

  LOG_CLI((BSL_META_U(unit,"init_info.max_duplications: %u\n"),max_duplications));
  LOG_CLI((BSL_META_U(unit,"init_info.nof_members: %u\n"),nof_members));
  LOG_CLI((BSL_META_U(unit,"init_info.member_size: %u\n"),member_size));
  LOG_CLI((BSL_META_U(unit,"multiset_data.counter_size: %u\n"),counter_size));
  LOG_CLI((BSL_META_U(unit,"init_info.prime_handle: %u\n"),prime_handle));
  LOG_CLI((BSL_META_U(unit,"init_info.sec_handle: %u\n"),sec_handle));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_SAND_MULTI_SET_INFO_print()", 0, 0);
}

#endif 

#include <soc/dpp/SAND/Utils/sand_footer.h>

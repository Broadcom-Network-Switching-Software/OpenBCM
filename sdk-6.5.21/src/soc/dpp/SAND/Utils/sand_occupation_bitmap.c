/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COMMON





#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/drv.h>

extern shr_sw_state_t *sw_state[BCM_MAX_NUM_UNITS];





#define  SOC_SAND_OCC_BM_ARRAY_MAP_LEN 256
#define  SOC_SAND_OCC_BM_NODE_IS_FULL  8





#define OCC_BITMAP_ACCESS                sw_state_access[unit].dpp.soc.sand.occ_bitmap  


#define SOC_SAND_OCC_BM_ACTIVE_INST_GET(_bmp_map_index, _level_indx, _char_indx, _cur_val,_err1,_err2,_err3,_err4,_err5) \
  { \
    uint8 cache_enabled ; \
    uint32 buffer_offset ; \
    res = OCC_BITMAP_ACCESS.dss_array.cache_enabled.get(unit, _bmp_map_index, &cache_enabled); \
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err1, exit); \
    if (cache_enabled) \
    { \
       \
      res = OCC_BITMAP_ACCESS.dss_array.levels_cache.get(unit, _bmp_map_index, _level_indx, &buffer_offset); \
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err2, exit); \
      res = OCC_BITMAP_ACCESS.dss_array.levels_cache_buffer.get(unit, _bmp_map_index, buffer_offset + _char_indx, &_cur_val); \
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err3, exit); \
    } \
    else \
    { \
       \
      res = OCC_BITMAP_ACCESS.dss_array.levels.get(unit, _bmp_map_index, _level_indx, &buffer_offset); \
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err4, exit); \
      res = OCC_BITMAP_ACCESS.dss_array.levels_buffer.get(unit, _bmp_map_index, buffer_offset + _char_indx, &_cur_val); \
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err5, exit); \
    } \
  }

#define SOC_SAND_OCC_BM_ACTIVE_INST_SET(_bmp_map_index, _level_indx, _char_indx, _cur_val,_err1,_err2,_err3,_err4,_err5) \
  { \
    uint8 cache_enabled ; \
    uint32 buffer_offset ; \
    res = OCC_BITMAP_ACCESS.dss_array.cache_enabled.get(unit, _bmp_map_index, &cache_enabled); \
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err1, exit); \
    if (cache_enabled) \
    { \
       \
      res = OCC_BITMAP_ACCESS.dss_array.levels_cache.get(unit, _bmp_map_index, _level_indx, &buffer_offset); \
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err2, exit); \
      res = OCC_BITMAP_ACCESS.dss_array.levels_cache_buffer.set(unit, _bmp_map_index, buffer_offset + _char_indx, _cur_val); \
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err3, exit); \
    } \
    else \
    { \
       \
      res = OCC_BITMAP_ACCESS.dss_array.levels.get(unit, _bmp_map_index, _level_indx, &buffer_offset); \
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err4, exit); \
      res = OCC_BITMAP_ACCESS.dss_array.levels_buffer.set(unit, _bmp_map_index, buffer_offset + _char_indx, _cur_val); \
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err5, exit); \
    } \
  }


#define SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(_bit_map_index,_err1,_err2) \
  { \
    uint8 bit_val ; \
    uint32 max_nof_dss ; \
    res = OCC_BITMAP_ACCESS.max_nof_dss.get(unit, &max_nof_dss) ; \
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err1, exit) ; \
    if (_bit_map_index >= max_nof_dss) \
    { \
       \
      bit_val = 0 ; \
    } \
    else \
    { \
      res = OCC_BITMAP_ACCESS.occupied_dss.bit_get(unit, (int)_bit_map_index, &bit_val) ; \
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err1, exit) ; \
    } \
    if (bit_val == 0) \
    { \
       \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_FREE_FAIL, _err2, exit) ; \
    } \
  }

#define SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, _err1) \
  if ((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES)) \
  { \
     \
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MAX_NUM_DEVICES_OUT_OF_RANGE_ERR, _err1, exit); \
  }


#define SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(_bit_map_index,_handle) (_bit_map_index = _handle - 1)
#define SOC_SAND_OCC_BM_CONVERT_BITMAP_INDEX_TO_HANDLE(_handle,_bit_map_index) (_handle = _bit_map_index + 1)













static
  uint8 Soc_sand_bitmap_val_to_first_one[SOC_SAND_OCC_BM_ARRAY_MAP_LEN] =
{
  SOC_SAND_OCC_BM_NODE_IS_FULL,
  0,
  1, 0,
  2, 0, 1, 0,
  3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};


static
  uint8 Soc_sand_bitmap_val_to_first_zero[SOC_SAND_OCC_BM_ARRAY_MAP_LEN] =
{
  0,
  1, 0,
  2, 0, 1, 0,
  3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  SOC_SAND_OCC_BM_NODE_IS_FULL
};


static
  uint8 Soc_sand_bitmap_val_to_last_zero[SOC_SAND_OCC_BM_ARRAY_MAP_LEN] =
{
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  3, 3, 3, 3, 3, 3, 3, 3,
  2, 2, 2, 2,
  1, 1,
  0,
  SOC_SAND_OCC_BM_NODE_IS_FULL
};





uint32
  soc_sand_occ_bm_init(
    SOC_SAND_IN       int                          unit,
    SOC_SAND_IN       uint32                       max_nof_dss
  )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_OCC_BM_INIT);

  res = OCC_BITMAP_ACCESS.alloc(unit);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = OCC_BITMAP_ACCESS.dss_array.ptr_alloc(unit, max_nof_dss);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  res = OCC_BITMAP_ACCESS.max_nof_dss.set(unit, max_nof_dss);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  res = OCC_BITMAP_ACCESS.in_use.set(unit, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  res = OCC_BITMAP_ACCESS.occupied_dss.alloc_bitmap(unit, max_nof_dss);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_init()",0,0);
}


uint32
  soc_sand_occ_bm_get_ptr_from_handle(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR          bit_map,
    SOC_SAND_OUT SOC_SAND_OCC_BM_T            **bit_map_ptr_ptr
  )
{
  uint32
    bit_map_index,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;

  *bit_map_ptr_ptr = sw_state[unit]->dpp.soc.sand.occ_bitmap->dss_array[bit_map_index] ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_get_handle_from_ptr()",0,0);
}

uint32
  soc_sand_occ_bm_create(
    SOC_SAND_IN       int                          unit,
    SOC_SAND_IN       SOC_SAND_OCC_BM_INIT_INFO   *init_info,
    SOC_SAND_OUT      SOC_SAND_OCC_BM_PTR         *bit_map
  )
{
  uint32
    found,
    cur_size,
    level_size,
    all_level_size,
    nof_levels,
    size;
  uint8
    bit_val,
    init_val;
  uint32
    indx,
    buffer_offset;
  uint32
    res;
  uint32
    in_use, max_nof_dss ;
  uint32
    bit_map_index ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_OCC_BM_CREATE);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_CHECK_NULL_INPUT(bit_map);
  SOC_SAND_CHECK_NULL_INPUT(init_info);
  
  size = init_info->size;
  if (init_info->init_val == FALSE)
  {
    init_val = 0X0;
  }
  else
  {
    init_val = 0XFF;
  }

  res = OCC_BITMAP_ACCESS.in_use.get(unit, &in_use);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);

  res = OCC_BITMAP_ACCESS.max_nof_dss.get(unit, &max_nof_dss);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);

  if (in_use >= max_nof_dss)
  {
    
    LOG_CLI((BSL_META_U(unit,"nof occupation bitmaps (%d) exceed the amount allowed per unit (%d). "),in_use, max_nof_dss)) ;
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 8, exit);
  }
  
  res = OCC_BITMAP_ACCESS.in_use.set(unit, (in_use + 1));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  
  found = 0 ;
  for (bit_map_index = 0 ; bit_map_index < max_nof_dss ; bit_map_index++)
  {
    res = OCC_BITMAP_ACCESS.occupied_dss.bit_get(unit, bit_map_index, &bit_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
    if (bit_val == 0)
    {
      
      found = 1 ;
      break ;
    }
  }
  if (!found)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 11, exit);
  }
  res = OCC_BITMAP_ACCESS.occupied_dss.bit_set(unit, bit_map_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit);

  res = OCC_BITMAP_ACCESS.dss_array.alloc(unit, bit_map_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);

  res = OCC_BITMAP_ACCESS.dss_array.init_val.set(unit, bit_map_index, init_info->init_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);

  res = OCC_BITMAP_ACCESS.dss_array.cache_enabled.set(unit, bit_map_index, FALSE);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  res = OCC_BITMAP_ACCESS.dss_array.support_cache.set(unit, bit_map_index, init_info->support_cache);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit);

  cur_size = SOC_SAND_NOF_BITS_IN_CHAR;
  nof_levels = 1;
  level_size = SOC_SAND_NOF_BITS_IN_CHAR; 
  
  while (cur_size < size)
  {
    cur_size *= SOC_SAND_NOF_BITS_IN_CHAR;
    ++nof_levels;
  }
  
  res = OCC_BITMAP_ACCESS.dss_array.levels.alloc(unit, bit_map_index, nof_levels);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);

  if (init_info->support_cache)
  {
    res = OCC_BITMAP_ACCESS.dss_array.support_cache.set(unit, bit_map_index, TRUE);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

    res = OCC_BITMAP_ACCESS.dss_array.levels_cache.alloc(unit, bit_map_index, nof_levels);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);
  }
  res = OCC_BITMAP_ACCESS.dss_array.levels_size.alloc(unit, bit_map_index, nof_levels);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 34, exit);
  
  all_level_size = 0;
  level_size = size;
  for (indx = nof_levels; indx > 0 ; --indx)
  {
      level_size = (level_size + (SOC_SAND_NOF_BITS_IN_CHAR - 1)) / SOC_SAND_NOF_BITS_IN_CHAR; 
      
      all_level_size += level_size;
  }
  if (init_info->support_cache)
  {
    res = OCC_BITMAP_ACCESS.dss_array.levels_cache_buffer.alloc(unit, bit_map_index, all_level_size);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 36, exit);
  }
  res = OCC_BITMAP_ACCESS.dss_array.levels_buffer.alloc(unit, bit_map_index, all_level_size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 38, exit);
  
  if (init_val) {
    res = OCC_BITMAP_ACCESS.dss_array.levels_buffer.memset(unit, bit_map_index, 0, all_level_size, init_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  }
  
  level_size    = size;
  buffer_offset = 0;
  for (indx = nof_levels; indx > 0 ; --indx)
  {
    res = OCC_BITMAP_ACCESS.dss_array.levels_size.set(unit, bit_map_index, indx - 1, level_size);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 42, exit);


    level_size = (level_size + (SOC_SAND_NOF_BITS_IN_CHAR - 1)) / SOC_SAND_NOF_BITS_IN_CHAR; 
    if (init_info->support_cache)
    {
      
      res = OCC_BITMAP_ACCESS.dss_array.levels_cache.set(unit, bit_map_index, indx - 1, buffer_offset);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 44, exit);
    }
    
    res = OCC_BITMAP_ACCESS.dss_array.levels.set(unit, bit_map_index, indx - 1, buffer_offset);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 46, exit);
    
    buffer_offset += level_size;

  }
  res = OCC_BITMAP_ACCESS.dss_array.nof_levels.set(unit, bit_map_index, nof_levels);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 48, exit);
  res = OCC_BITMAP_ACCESS.dss_array.size.set(unit, bit_map_index, size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
  res = OCC_BITMAP_ACCESS.dss_array.buffer_size.set(unit, bit_map_index, all_level_size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 52, exit);
  
  SOC_SAND_OCC_BM_CONVERT_BITMAP_INDEX_TO_HANDLE((*bit_map),bit_map_index) ;
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_create()",0,0);
}

uint32
  soc_sand_occ_bm_destroy(
    SOC_SAND_IN        int                   unit,
    SOC_SAND_OUT       SOC_SAND_OCC_BM_PTR   bit_map
  )
{
  uint32
    bit_map_index,
    res;
  uint8
    support_cache;
  uint8
    bit_val ;
  uint32
    in_use ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_OCC_BM_DESTROY);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 7) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;
  
  res = OCC_BITMAP_ACCESS.in_use.get(unit, &in_use);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);
  if ((int)in_use < 0)
  {
    
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_FREE_FAIL, 8, exit);
  }
  
  res = OCC_BITMAP_ACCESS.in_use.set(unit, (in_use - 1));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  
  res = OCC_BITMAP_ACCESS.occupied_dss.bit_get(unit, bit_map_index, &bit_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
  if (bit_val == 0)
  {
    
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_FREE_FAIL, 14, exit) ;
  }
  res = OCC_BITMAP_ACCESS.occupied_dss.bit_clear(unit, bit_map_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit);

  res = OCC_BITMAP_ACCESS.dss_array.levels.free(unit, bit_map_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  res = OCC_BITMAP_ACCESS.dss_array.levels_buffer.free(unit, bit_map_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit);

  res = OCC_BITMAP_ACCESS.dss_array.support_cache.get(unit, bit_map_index, &support_cache);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit);

  if (support_cache)
  {
    res = OCC_BITMAP_ACCESS.dss_array.levels_cache.free(unit, bit_map_index);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit);
    res = OCC_BITMAP_ACCESS.dss_array.levels_cache_buffer.free(unit, bit_map_index);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);
  }
  
  res = OCC_BITMAP_ACCESS.dss_array.levels_size.free(unit, bit_map_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  res = OCC_BITMAP_ACCESS.dss_array.free(unit, bit_map_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_destroy()",0,0);
}

uint32
  soc_sand_occ_bm_clear(
    SOC_SAND_IN        int                   unit,
    SOC_SAND_IN       SOC_SAND_OCC_BM_PTR    bit_map
  )
{
  uint32
    bit_map_index,
    level_indx,
    level_size, res, ii, buffer_offset ;
  uint8
    init_val ;
  uint32
    nof_levels ;
  uint8
    support_cache,
    cache_enabled ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_OCC_BM_CLEAR);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 8) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;

  res = OCC_BITMAP_ACCESS.dss_array.init_val.get(unit, bit_map_index, &init_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  if (init_val == FALSE)
  {
    init_val = 0X0;
  }
  else
  {
    init_val = 0XFF;
  }
  res = OCC_BITMAP_ACCESS.dss_array.support_cache.get(unit, bit_map_index, &support_cache);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
  res = OCC_BITMAP_ACCESS.dss_array.cache_enabled.get(unit, bit_map_index, &cache_enabled);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit);
  {
      res = OCC_BITMAP_ACCESS.dss_array.nof_levels.get(unit, bit_map_index, &nof_levels);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
      for (level_indx = 0; level_indx < nof_levels; ++level_indx )
      {
          res = OCC_BITMAP_ACCESS.dss_array.levels_size.get(unit, bit_map_index, level_indx, &level_size);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
          
          level_size = (level_size + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;

          if (support_cache && cache_enabled)
          {
              
              res = OCC_BITMAP_ACCESS.dss_array.levels_cache.get(unit, bit_map_index, level_indx, &buffer_offset);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
              for (ii = 0 ; ii < level_size ; ii++)
              {
                  res = OCC_BITMAP_ACCESS.dss_array.levels_cache_buffer.set(unit, bit_map_index, buffer_offset + ii, init_val);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit);
              }
          }
          else
          {
              
              res = OCC_BITMAP_ACCESS.dss_array.levels.get(unit, bit_map_index, level_indx, &buffer_offset);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit);
              for (ii = 0 ; ii < level_size ; ii++)
              {
                  res = OCC_BITMAP_ACCESS.dss_array.levels_buffer.set(unit, bit_map_index, buffer_offset + ii, init_val);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit);
              }
          }
      }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_clear()",0,0);

}

STATIC uint32
  soc_sand_occ_bm_get_next_helper(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR      bit_map,
    SOC_SAND_IN  uint8                    val,
    SOC_SAND_IN  uint32                   level_indx,
    SOC_SAND_IN  uint32                   char_indx,
    SOC_SAND_OUT  uint32                  *place,
    SOC_SAND_OUT  uint8                   *found
  )
{
  uint32
    cur_val,
    cur_bit,
    level_size;
  uint32
    bit_map_index,
    res;
  uint32
    nof_levels ;
  uint8
    cur_val_uint8 ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_OCC_BM_GET_NEXT_HELPER);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 7) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;

  SOC_SAND_OCC_BM_ACTIVE_INST_GET(bit_map_index,level_indx,char_indx,cur_val_uint8,8,10,12,14,16);

  cur_val = cur_val_uint8 ;
  res = OCC_BITMAP_ACCESS.dss_array.levels_size.get(unit, bit_map_index, level_indx, &level_size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  
  if (val)
  {
    cur_bit = Soc_sand_bitmap_val_to_first_one[cur_val];
  }
  else
  {
    cur_bit = Soc_sand_bitmap_val_to_first_zero[cur_val];
  }
 
  if (cur_bit == SOC_SAND_OCC_BM_NODE_IS_FULL || cur_bit + SOC_SAND_NOF_BITS_IN_CHAR * char_indx >= level_size)
  {
    *place = 0;
    *found = FALSE;
    goto exit;
  }
  
  res = OCC_BITMAP_ACCESS.dss_array.nof_levels.get(unit, bit_map_index, &nof_levels);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  if (level_indx == nof_levels - 1)
  {
    *place = cur_bit + SOC_SAND_NOF_BITS_IN_CHAR * char_indx;
    *found = TRUE;
    goto exit;
  }
 
  res = soc_sand_occ_bm_get_next_helper(
          unit,
          bit_map,
          val,
          level_indx + 1,
          cur_bit + SOC_SAND_NOF_BITS_IN_CHAR * char_indx,
          place,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_get_next_helper()",0,0);
}


STATIC uint32
  soc_sand_occ_bm_calc_mask(
    SOC_SAND_IN  uint32                     nof_levels,
    SOC_SAND_IN  uint32                     level_indx,
    SOC_SAND_IN  uint32                     char_indx,
    SOC_SAND_IN  uint32                     down_limit,
    SOC_SAND_IN  uint32                     up_limit,
    SOC_SAND_IN  uint8                    forward
  )
{
  uint32
    range_start,
    range_end,
    btmp_start,
    btmp_end,
    start,
    end,
    mask_val,
    indx;

  range_start = down_limit;
  range_end = up_limit;

  for (indx = nof_levels - 1; indx > level_indx; --indx )
  {
    range_start = (range_start) / SOC_SAND_NOF_BITS_IN_CHAR;
    range_end = (range_end ) / SOC_SAND_NOF_BITS_IN_CHAR;
  }

  btmp_start = char_indx * SOC_SAND_NOF_BITS_IN_CHAR;
  btmp_end = (char_indx + 1) * SOC_SAND_NOF_BITS_IN_CHAR - 1;

  if (range_start < btmp_start)
  {
    start = 0;
  }
  else if (range_start > btmp_end)
  {
    return SOC_SAND_U32_MAX;
  }
  else
  {
    start = range_start - btmp_start;
  }

  if (range_end > btmp_end)
  {
    end = SOC_SAND_NOF_BITS_IN_UINT32 - 1;
  }
  else if (range_end < btmp_start)
  {
    return SOC_SAND_U32_MAX;
  }
  else
  {
    end = range_end - btmp_start;
  }

  mask_val = 0;
  mask_val = SOC_SAND_ZERO_BITS_MASK(end,start);
  mask_val &= SOC_SAND_ZERO_BITS_MASK(SOC_SAND_NOF_BITS_IN_UINT32 - 1,SOC_SAND_NOF_BITS_IN_CHAR);

  return mask_val;
}



STATIC uint32
  soc_sand_occ_bm_get_next_range_helper(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR       bit_map,
    SOC_SAND_IN  uint8                     val,
    SOC_SAND_IN  uint32                     level_indx,
    SOC_SAND_IN  uint32                     char_indx,
    SOC_SAND_IN  uint32                     down_limit,
    SOC_SAND_IN  uint32                     up_limit,
    SOC_SAND_IN  uint8                     forward,
    SOC_SAND_OUT  uint32                   *place,
    SOC_SAND_OUT  uint8                    *found
  )
{
  uint32
    cur_val,
    cur_bit,
    char_iter,
    level_size,
    mask_val,
    nof_levels ;
  uint32
    bit_map_index,
    res;
  uint8
    cur_val_uint8 ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_OCC_BM_GET_NEXT_HELPER);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 7) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;

  res = OCC_BITMAP_ACCESS.dss_array.levels_size.get(unit, bit_map_index, level_indx, &level_size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit) ;
  res = OCC_BITMAP_ACCESS.dss_array.nof_levels.get(unit, bit_map_index, &nof_levels);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_OCC_BM_ACTIVE_INST_GET(bit_map_index,level_indx,char_indx,cur_val_uint8,12,14,16,18,20);
  cur_val = cur_val_uint8 ;

  mask_val = soc_sand_occ_bm_calc_mask(
               nof_levels,
               level_indx,
               char_indx,
               down_limit,
               up_limit,
               forward
             );

  cur_val |= mask_val;
  
  for (char_iter = 0; char_iter < SOC_SAND_NOF_BITS_IN_CHAR; ++char_iter)
  {
    if (forward)
    {
      
      if (val)
      {
        cur_bit = Soc_sand_bitmap_val_to_first_one[cur_val];
      }
      else
      {
        cur_bit = Soc_sand_bitmap_val_to_first_zero[cur_val];
      }
    }
    else
    {
      if (val)
      {
        cur_bit = Soc_sand_bitmap_val_to_last_zero[255 - cur_val];
      }
      else
      {
        cur_bit = Soc_sand_bitmap_val_to_last_zero[cur_val];
      }
    }
    
    if (cur_bit == SOC_SAND_OCC_BM_NODE_IS_FULL || cur_bit + SOC_SAND_NOF_BITS_IN_CHAR * char_indx >= level_size)
    {
      *place = 0;
      *found = FALSE;
      goto exit;
    }
    
    if (level_indx == nof_levels - 1)
    {
      *place = cur_bit + SOC_SAND_NOF_BITS_IN_CHAR * char_indx;
      *found = TRUE;
      goto exit;
    }
    
    cur_val |= SOC_SAND_BIT(cur_bit);
    
    res = soc_sand_occ_bm_get_next_range_helper(
            unit,
            bit_map,
            val,
            level_indx + 1,
            cur_bit + SOC_SAND_NOF_BITS_IN_CHAR * char_indx,
            down_limit,
            up_limit,
            forward,
            place,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (*found)
    {
      goto exit;
    }
  }
  *place = 0;
  *found = FALSE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_get_next_helper()",0,0);
}


uint32
  soc_sand_occ_bm_get_next(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR   bit_map,
    SOC_SAND_OUT uint32               *place,
    SOC_SAND_OUT uint8                *found
  )
{
  uint32
    bit_map_index,
    res;
  uint32
    size ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_OCC_BM_GET_NEXT);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 8) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;

  res = soc_sand_occ_bm_get_next_helper(
          unit,
          bit_map,
          0,
          0,
          0,
          place,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = OCC_BITMAP_ACCESS.dss_array.size.get(unit, bit_map_index, &size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
  if (*place >= size)
  {
    *place = 0;
    *found = FALSE;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_destroy()",0,0);
}



uint32
  soc_sand_occ_bm_get_next_in_range(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR       bit_map,
    SOC_SAND_IN  uint32                    start,
    SOC_SAND_IN  uint32                    end,
    SOC_SAND_IN  uint8                     forward,
    SOC_SAND_OUT uint32                   *place,
    SOC_SAND_OUT uint8                    *found
  )
{
  uint32
    bit_map_index,
    res;
  uint32
    size ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_OCC_BM_GET_NEXT);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 8) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;

  res = soc_sand_occ_bm_get_next_range_helper(
          unit,
          bit_map,
          0,
          0,
          0,
          start,
          end,
          forward,
          place,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = OCC_BITMAP_ACCESS.dss_array.size.get(unit, bit_map_index, &size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
  if (*place >= size)
  {
    *place = 0;
    *found = FALSE;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_destroy()",0,0);
}



uint32
  soc_sand_occ_bm_alloc_next(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR         bit_map,
    SOC_SAND_OUT  uint32                    *place,
    SOC_SAND_OUT  uint8                     *found
  )
{
  uint32
    bit_map_index,
    res;
  uint32
    size ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_OCC_BM_GET_NEXT);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 8) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;

  res = soc_sand_occ_bm_get_next(
          unit,
          bit_map,
          place,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = OCC_BITMAP_ACCESS.dss_array.size.get(unit, bit_map_index, &size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);

  if (*place >= size)
  {
    *place = 0;
    *found = FALSE;
    goto exit;
  }

  res = soc_sand_occ_bm_occup_status_set(
          unit,
          bit_map,
          *place,
          1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_alloc_next()",0,0);
}




uint32
  soc_sand_occ_bm_occup_status_set_helper(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_INOUT  SOC_SAND_OCC_BM_PTR     bit_map,
    SOC_SAND_IN  uint32               level_indx,
    SOC_SAND_IN  uint32               char_indx,
    SOC_SAND_IN  uint32               bit_indx,
    SOC_SAND_IN  uint8              occupied
  )
{
  uint32
    bit_map_index,
    old_val,
    new_val;
  uint32
    res;
  uint8 new_val_uint8;
  uint8 old_val_uint8;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_OCC_BM_OCCUP_STATUS_SET);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 7) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;
   
  SOC_SAND_OCC_BM_ACTIVE_INST_GET(bit_map_index,level_indx,char_indx,old_val_uint8,8,10,12,14,16) ;
  old_val = old_val_uint8 ; 
  new_val = old_val;

  res = soc_sand_set_field(
          &new_val,
          bit_indx,
          bit_indx,
          occupied
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  {
      
      new_val_uint8 = new_val ;
      SOC_SAND_OCC_BM_ACTIVE_INST_SET(bit_map_index,level_indx,char_indx,new_val_uint8,22,24,26,28,30) ;
  }
  
  if (level_indx == 0)
  {
    goto exit;
  }

  if ((occupied && old_val != 0xFF && new_val == 0xFF) ||
      (!occupied && old_val == 0xFF && new_val != 0xFF) )
  {
    res = soc_sand_occ_bm_occup_status_set_helper(
            unit,
            bit_map,
            level_indx - 1,
            char_indx / SOC_SAND_NOF_BITS_IN_CHAR,
            char_indx % SOC_SAND_NOF_BITS_IN_CHAR,
            occupied
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_occup_status_set_helper()",0,0);
}



uint32
  soc_sand_occ_bm_occup_status_set(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_INOUT  SOC_SAND_OCC_BM_PTR     bit_map,
    SOC_SAND_IN     uint32                  place,
    SOC_SAND_IN     uint8                   occupied
  )
{
  uint32
    bit_map_index,
    res;
  uint32
    nof_levels ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_OCC_BM_OCCUP_STATUS_SET);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 8) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;

  res = OCC_BITMAP_ACCESS.dss_array.nof_levels.get(unit, bit_map_index, &nof_levels);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_occ_bm_occup_status_set_helper(
          unit,
          bit_map,
          nof_levels - 1,
          place / SOC_SAND_NOF_BITS_IN_CHAR,
          place % SOC_SAND_NOF_BITS_IN_CHAR,
          occupied
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_destroy()",0,0);

}


uint32
  soc_sand_occ_bm_is_occupied(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR        bit_map,
    SOC_SAND_IN  uint32                     place,
    SOC_SAND_OUT uint8                     *occupied
  )
{
  uint32
    res,
    bit_map_index,
    char_indx,
    val,
    bit_indx;
  uint32
    nof_levels ;
  uint8
    val_uint8 ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_OCC_BM_GET_NEXT);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 8) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(occupied);

  res = OCC_BITMAP_ACCESS.dss_array.nof_levels.get(unit, bit_map_index, &nof_levels);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  char_indx = place / SOC_SAND_NOF_BITS_IN_CHAR;
  SOC_SAND_OCC_BM_ACTIVE_INST_GET(bit_map_index,nof_levels - 1 ,char_indx,val_uint8,4,6,8,10,12) ;
  val = val_uint8 ;
  bit_indx = place % SOC_SAND_NOF_BITS_IN_CHAR;

  val = SOC_SAND_GET_BITS_RANGE(
          val,
          bit_indx,
          bit_indx
      );

  *occupied = SOC_SAND_NUM2BOOL(val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_is_occupied()",0,0);
}

uint32
  soc_sand_occ_bm_cache_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR        bit_map,
    SOC_SAND_IN  uint8                      cached
  )
{
  uint32
    bit_map_index,
    level_indx,
    level_size, ii, res ;
  uint8
    tmp_uint8,
    support_cache,
    cache_enabled ;
  uint32
    nof_levels ;
  uint32
    levels_cache_offset,
    levels_offset ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 8) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;

  res = OCC_BITMAP_ACCESS.dss_array.support_cache.get(unit, bit_map_index, &support_cache);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  if (!support_cache && cached)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 12, exit);
  }
  res = OCC_BITMAP_ACCESS.dss_array.cache_enabled.get(unit, bit_map_index, &cache_enabled);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit);

  
  if (cache_enabled == cached)
  {
    goto exit;
  }
  {
      cache_enabled = cached;
      res = OCC_BITMAP_ACCESS.dss_array.cache_enabled.set(unit, bit_map_index, cache_enabled);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }
  
  res = OCC_BITMAP_ACCESS.dss_array.nof_levels.get(unit, bit_map_index, &nof_levels);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
  if (cached)
  {
      for (level_indx = 0; level_indx < nof_levels; ++level_indx )
      {      
          res = OCC_BITMAP_ACCESS.dss_array.levels_size.get(unit, bit_map_index, level_indx, &level_size);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
          level_size = (level_size + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
          res = OCC_BITMAP_ACCESS.dss_array.levels_cache.get(unit, bit_map_index, level_indx, &levels_cache_offset);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
          res = OCC_BITMAP_ACCESS.dss_array.levels.get(unit, bit_map_index, level_indx, &levels_offset);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit);

          for (ii = 0 ; ii < level_size ; ii++)
          {
              res = OCC_BITMAP_ACCESS.dss_array.levels_buffer.get(unit, bit_map_index, levels_offset + ii, &tmp_uint8);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit);
              res = OCC_BITMAP_ACCESS.dss_array.levels_cache_buffer.set(unit, bit_map_index, levels_cache_offset + ii, tmp_uint8);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit);
          }
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_cache_set()",0,0);
}

uint32
  soc_sand_occ_bm_cache_commit(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR        bit_map,
    SOC_SAND_IN  uint32                     flags
  )
{
  uint32
    bit_map_index,
    level_indx,
    level_size, ii, res;
  uint32
    levels_cache_offset,
    levels_offset ;
  uint8
    support_cache ;
  uint8
    tmp_uint8,
    cache_enabled ;
  uint32
    nof_levels ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 8) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;

  res = OCC_BITMAP_ACCESS.dss_array.support_cache.get(unit, bit_map_index, &support_cache);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  if (!support_cache)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 12, exit);
  }
  
  res = OCC_BITMAP_ACCESS.dss_array.cache_enabled.get(unit, bit_map_index, &cache_enabled);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit);
  if (!cache_enabled)
  {
    goto exit;
  }
  
  res = OCC_BITMAP_ACCESS.dss_array.nof_levels.get(unit, bit_map_index, &nof_levels);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
  {
      for (level_indx = 0; level_indx < nof_levels; ++level_indx )
      {
          res = OCC_BITMAP_ACCESS.dss_array.levels_size.get(unit, bit_map_index, level_indx, &level_size);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
          level_size = (level_size + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
          res = OCC_BITMAP_ACCESS.dss_array.levels_cache.get(unit, bit_map_index, level_indx, &levels_cache_offset);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
          res = OCC_BITMAP_ACCESS.dss_array.levels.get(unit, bit_map_index, level_indx, &levels_offset);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit);
          for (ii = 0 ; ii < level_size ; ii++)
          {
              res = OCC_BITMAP_ACCESS.dss_array.levels_cache_buffer.get(unit, bit_map_index, levels_cache_offset + ii, &tmp_uint8);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit);
              res = OCC_BITMAP_ACCESS.dss_array.levels_buffer.set(unit, bit_map_index, levels_offset + ii, tmp_uint8);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit);
          }
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_cache_commit()",0,0);
}

uint32
  soc_sand_occ_bm_cache_rollback(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR        bit_map,
    SOC_SAND_IN  uint32                     flags
  )
{
  uint32
    bit_map_index,
    level_indx,
    level_size,
    res;
  uint8
    support_cache ;
  uint8
    tmp_uint8,
    cache_enabled ;
  uint32
    nof_levels ;
  uint32
    ii,
    levels_cache_offset,
    levels_offset ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 8) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;

  res = OCC_BITMAP_ACCESS.dss_array.support_cache.get(unit, bit_map_index, &support_cache);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  if (!support_cache)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }
  
  res = OCC_BITMAP_ACCESS.dss_array.cache_enabled.get(unit, bit_map_index, &cache_enabled);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit);
  if (!cache_enabled)
  {
    goto exit;
  }
  
  res = OCC_BITMAP_ACCESS.dss_array.nof_levels.get(unit, bit_map_index, &nof_levels);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
  {
      for (level_indx = 0; level_indx < nof_levels; ++level_indx )
      {
          res = OCC_BITMAP_ACCESS.dss_array.levels_size.get(unit, bit_map_index, level_indx, &level_size);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
          level_size = (level_size + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
          res = OCC_BITMAP_ACCESS.dss_array.levels_cache.get(unit, bit_map_index, level_indx, &levels_cache_offset);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
          res = OCC_BITMAP_ACCESS.dss_array.levels.get(unit, bit_map_index, level_indx, &levels_offset);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit);
          for (ii = 0 ; ii < level_size ; ii++)
          {
              res = OCC_BITMAP_ACCESS.dss_array.levels_buffer.get(unit, bit_map_index, levels_offset + ii, &tmp_uint8);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit);
              res = OCC_BITMAP_ACCESS.dss_array.levels_cache_buffer.set(unit, bit_map_index, levels_cache_offset + ii, tmp_uint8);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit);
          }
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_cache_rollback()",0,0);
}

uint32
  soc_sand_occ_bm_get_size_for_save(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR          bit_map,
    SOC_SAND_OUT  uint32                     *size
  )
{
  uint32
    res,
    bit_map_index,
    level_indx,
    level_size;
  uint32
    cur_size = 0 ;
  uint32
    nof_levels ;
  uint8
    cache_enabled ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 8) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;

  cur_size += sizeof(uint8) * 2 ;
  cur_size += sizeof(uint32) * 1 ;

  
  res = OCC_BITMAP_ACCESS.dss_array.nof_levels.get(unit, bit_map_index, &nof_levels);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
  for (level_indx = 0; level_indx < nof_levels; ++level_indx )
  {
    res = OCC_BITMAP_ACCESS.dss_array.levels_size.get(unit, bit_map_index, level_indx, &level_size);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
    level_size = (level_size + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
    cur_size += level_size * sizeof(uint8);
  }
  res = OCC_BITMAP_ACCESS.dss_array.cache_enabled.get(unit, bit_map_index, &cache_enabled);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  if (cache_enabled)
  {
    for (level_indx = 0; level_indx < nof_levels; ++level_indx )
    {
      res = OCC_BITMAP_ACCESS.dss_array.levels_size.get(unit, bit_map_index, level_indx, &level_size);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
      level_size = (level_size + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
      cur_size += level_size * sizeof(uint8);
    }
  }

  *size = cur_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_get_size_for_save()",0,0);
}

uint32
  soc_sand_occ_bm_save(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR    bit_map,
    SOC_SAND_OUT uint8                 *buffer,
    SOC_SAND_IN  uint32                 buffer_size_bytes,
    SOC_SAND_OUT uint32                *actual_size_bytes
  )
{
  uint32
    res,
    bit_map_index,
    level_indx,
    level_size;
  uint8
    *cur_ptr = (uint8*)buffer;
  uint32
    ii, cur_size;
  uint8
    init_val ;
  uint32
    size ;
  uint8
    support_cache ;
  uint32
    nof_levels ;
  uint32
    levels_offset,
    levels_cache_offset ;
  uint8
    cache_enabled ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 7) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(bit_map_index,4,6) ;

  
  res = OCC_BITMAP_ACCESS.dss_array.init_val.get(unit, bit_map_index, &init_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit) ;
  soc_sand_os_memcpy(cur_ptr, &(init_val),sizeof(uint8));
  cur_ptr += sizeof(uint8);

  res = OCC_BITMAP_ACCESS.dss_array.size.get(unit, bit_map_index, &size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  soc_sand_os_memcpy(cur_ptr, &(size),sizeof(uint32));
  cur_ptr += sizeof(uint32); 

  res = OCC_BITMAP_ACCESS.dss_array.support_cache.get(unit, bit_map_index, &support_cache);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit) ;
  soc_sand_os_memcpy(cur_ptr, &(support_cache),sizeof(uint8));
  cur_ptr += sizeof(uint8);

  
  res = OCC_BITMAP_ACCESS.dss_array.nof_levels.get(unit, bit_map_index, &nof_levels);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
  for (level_indx = 0; level_indx < nof_levels; ++level_indx )
  {
    res = OCC_BITMAP_ACCESS.dss_array.levels_size.get(unit, bit_map_index, level_indx, &level_size);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
    level_size = (level_size + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
    cur_size = level_size * sizeof(uint8);
    res = OCC_BITMAP_ACCESS.dss_array.levels.get(unit, bit_map_index, level_indx, &levels_offset);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit);
    for (ii = 0 ; ii < cur_size ; ii++)
    {
      res = OCC_BITMAP_ACCESS.dss_array.levels_buffer.get(unit, bit_map_index, levels_offset + ii, cur_ptr);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit);
      cur_ptr++ ;
    }
  }
  res = OCC_BITMAP_ACCESS.dss_array.cache_enabled.get(unit, bit_map_index, &cache_enabled);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  if (cache_enabled)
  {
    for (level_indx = 0; level_indx < nof_levels; ++level_indx )
    {
      res = OCC_BITMAP_ACCESS.dss_array.levels_size.get(unit, bit_map_index, level_indx, &level_size);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit) ;
      level_size = (level_size + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
      cur_size = level_size * sizeof(uint8);
      res = OCC_BITMAP_ACCESS.dss_array.levels_cache.get(unit, bit_map_index, level_indx, &levels_cache_offset);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
      for (ii = 0 ; ii < cur_size ; ii++)
      {
        res = OCC_BITMAP_ACCESS.dss_array.levels_cache_buffer.get(unit, bit_map_index, levels_cache_offset + ii, cur_ptr);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);
        cur_ptr++ ;
      }
    }
  }
  *actual_size_bytes = cur_ptr - buffer;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_save()",0,0);
}

uint32
  soc_sand_occ_bm_load(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                           **buffer,
    SOC_SAND_OUT SOC_SAND_OCC_BM_PTR              *bit_map
  )
{
  SOC_SAND_OCC_BM_INIT_INFO
    bm_restored;
  uint32
    level_indx,
    level_size;
  SOC_SAND_IN uint8
    *cur_ptr = (SOC_SAND_IN uint8*)buffer[0];
  uint32
    cur_size;
  uint32
    res;
  uint32
    nof_levels ;
  uint32
    ii,
    bit_map_index,
    levels_offset,
    levels_cache_offset ;
  uint8
    cache_enabled ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 8) ;

  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&bm_restored);

  
  soc_sand_os_memcpy(&(bm_restored.init_val), cur_ptr, sizeof(uint8));
  cur_ptr += sizeof(uint8);

  soc_sand_os_memcpy(&(bm_restored.size), cur_ptr, sizeof(uint32));
  cur_ptr += sizeof(uint32);
 
  soc_sand_os_memcpy(&(bm_restored.support_cache), cur_ptr, sizeof(uint8));
  cur_ptr += sizeof(uint8);

  
  res = soc_sand_occ_bm_create(unit, &bm_restored, bit_map) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,(*bit_map)) ;

  
  res = OCC_BITMAP_ACCESS.dss_array.nof_levels.get(unit, bit_map_index, &nof_levels);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
  for (level_indx = 0; level_indx < nof_levels; ++level_indx )
  {
    res = OCC_BITMAP_ACCESS.dss_array.levels_size.get(unit, bit_map_index, level_indx, &level_size);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
    level_size = (level_size + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
    cur_size = level_size * sizeof(uint8) ;
    res = OCC_BITMAP_ACCESS.dss_array.levels.get(unit, bit_map_index, level_indx, &levels_offset);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit);
    for (ii = 0 ; ii < cur_size ; ii++)
    {
      res = OCC_BITMAP_ACCESS.dss_array.levels_buffer.set(unit, bit_map_index, levels_offset + ii, *cur_ptr);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit);
      cur_ptr++ ;
    }
  }
  res = OCC_BITMAP_ACCESS.dss_array.cache_enabled.get(unit, bit_map_index, &cache_enabled);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  if (cache_enabled)
  {
    for (level_indx = 0; level_indx < nof_levels; ++level_indx )
    {
      res = OCC_BITMAP_ACCESS.dss_array.levels_size.get(unit, bit_map_index, level_indx, &level_size);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit) ;
      level_size = (level_size + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
      cur_size = level_size * sizeof(uint8);
      res = OCC_BITMAP_ACCESS.dss_array.levels_cache.get(unit, bit_map_index, level_indx, &levels_cache_offset);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 34, exit);
      for (ii = 0 ; ii < cur_size ; ii++)
      {
        res = OCC_BITMAP_ACCESS.dss_array.levels_cache_buffer.set(unit, bit_map_index, levels_cache_offset + ii, *cur_ptr);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);
        cur_ptr++ ;
      }
    }
  }
    
  *buffer = cur_ptr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_load()",0,0);
}


void
  soc_sand_SAND_OCC_BM_INIT_INFO_clear(
    SOC_SAND_INOUT SOC_SAND_OCC_BM_INIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_OCC_BM_INIT_INFO));
  info->size          = 0;
  info->init_val      = FALSE;
  info->support_cache = FALSE;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



uint32
  soc_sand_occ_bm_get_buffer_size(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR          bit_map,
    SOC_SAND_OUT  uint32                      *buffer_size_p
  )
{
  uint32
    res;
  uint32
    bit_map_index,
    buffer_size ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 8) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE((bit_map_index),4,6) ;

  res = OCC_BITMAP_ACCESS.dss_array.buffer_size.get(unit, bit_map_index, &buffer_size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  *buffer_size_p = buffer_size ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_occ_bm_get_buffer_size()", 0, 0);
}

uint32
  soc_sand_occ_bm_get_support_cache(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR          bit_map,
    SOC_SAND_OUT  uint8                       *support_cache_p
  )
{
  uint32
    bit_map_index,
    res;
  uint8
    support_cache ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 8) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE((bit_map_index),4,6) ;

  res = OCC_BITMAP_ACCESS.dss_array.support_cache.get(unit, bit_map_index, &support_cache);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  *support_cache_p = support_cache ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_occ_bm_get_support_cache()", 0, 0);
}


uint32
  soc_sand_occ_bm_get_illegal_bitmap_handle(
    void
  )
{
  return ((uint32)(-1)) ;
}

uint32
  soc_sand_occ_is_bitmap_active(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR  bit_map,
    SOC_SAND_OUT uint8                *in_use
  )
{
  uint32
    bit_map_index,
    res;
  uint32
    max_nof_dss ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0) ;

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;

  *in_use = 0 ;
  res = OCC_BITMAP_ACCESS.max_nof_dss.get(unit, &max_nof_dss) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit) ;
  if (bit_map_index < max_nof_dss)
  {
    res = OCC_BITMAP_ACCESS.occupied_dss.bit_get(unit, (int)bit_map_index, in_use);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_is_bitmap_active()",0,0);
}


uint32
  soc_sand_occ_bm_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR  bit_map
  )
{
  uint32
    res,
    bit_map_index,
    char_indx,
    level_indx,
    level_size,
    levels_cache_offset,
    levels_offset,
    nof_levels,
    size,
    buffer_size;
  uint8
    level_bits ; 
  uint8
    init_val,
    support_cache,
    cache_enabled ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0) ;

  SOC_SAND_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit, 8) ;
  SOC_SAND_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(bit_map_index,bit_map) ;
  SOC_SAND_OCC_BM_VERIFY_BITMAP_IS_ACTIVE((bit_map_index),4,6) ;

  res = OCC_BITMAP_ACCESS.dss_array.size.get(unit, bit_map_index, &size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  LOG_CLI((BSL_META_U(unit,"size %u. "),size)) ;
  res = OCC_BITMAP_ACCESS.dss_array.init_val.get(unit, bit_map_index, &init_val) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  LOG_CLI((BSL_META_U(unit,"init_val %u. "),init_val)) ;
  res = OCC_BITMAP_ACCESS.dss_array.support_cache.get(unit, bit_map_index, &support_cache) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  LOG_CLI((BSL_META_U(unit,"support_cache %u. "),support_cache)) ;
  res = OCC_BITMAP_ACCESS.dss_array.cache_enabled.get(unit, bit_map_index, &cache_enabled) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  LOG_CLI((BSL_META_U(unit,"cache_enabled %u. "),cache_enabled)) ;
  res = OCC_BITMAP_ACCESS.dss_array.buffer_size.get(unit, bit_map_index, &buffer_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  LOG_CLI((BSL_META_U(unit,"buffer_size %u. "),buffer_size)) ;
  LOG_CLI((BSL_META_U(unit,"\n")));
  res = OCC_BITMAP_ACCESS.dss_array.nof_levels.get(unit, bit_map_index, &nof_levels) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  LOG_CLI((BSL_META_U(unit,"nof_levels %u. "),nof_levels)) ;
  LOG_CLI((BSL_META_U(unit,"Bare levels: "))) ;
  LOG_CLI((BSL_META_U(unit,"\n")));
  for (level_indx = 0 ; level_indx < nof_levels ; ++level_indx )
  {
    res = OCC_BITMAP_ACCESS.dss_array.levels_size.get(unit, bit_map_index, level_indx, &level_size);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit) ;
    level_size = (level_size + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
    LOG_CLI((BSL_META_U(unit,"level %u. level size %u:"),level_indx,level_size));
    res = OCC_BITMAP_ACCESS.dss_array.levels.get(unit, bit_map_index, level_indx, &levels_offset);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit);
    for (char_indx = 0; char_indx < level_size; ++char_indx )
    {
      res = OCC_BITMAP_ACCESS.dss_array.levels_buffer.get(unit, bit_map_index, levels_offset + char_indx, &level_bits);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
      LOG_CLI((BSL_META_U(unit,"%02x  "), level_bits)) ;
    }
    LOG_CLI((BSL_META_U(unit,"\n")));
  }
  if (cache_enabled)
  {
    LOG_CLI((BSL_META_U(unit,"Cache levels: "))) ;
    LOG_CLI((BSL_META_U(unit,"\n")));
    for (level_indx = 0; level_indx < nof_levels; ++level_indx )
    {
      res = OCC_BITMAP_ACCESS.dss_array.levels_size.get(unit, bit_map_index, level_indx, &level_size);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit) ;
      level_size = (level_size + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
      res = OCC_BITMAP_ACCESS.dss_array.levels_cache.get(unit, bit_map_index, level_indx, &levels_cache_offset);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 34, exit);
      for (char_indx = 0 ; char_indx < level_size ; char_indx++)
      {
        res = OCC_BITMAP_ACCESS.dss_array.levels_cache_buffer.get(unit, bit_map_index, levels_cache_offset + char_indx, &level_bits);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);
        LOG_CLI((BSL_META_U(unit,"%02x  "), level_bits)) ;
      }
      LOG_CLI((BSL_META_U(unit,"\n")));
    }
  }

  LOG_CLI((BSL_META_U(unit,"\n\n\n")));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_print()",0,0);

}
#if SOC_SAND_DEBUG


#if (0)




uint32
soc_sand_occ_bm_TEST_1(
  SOC_SAND_IN int unit,
  SOC_SAND_IN uint8 silent
)
{
  int32
    pass = TRUE;
  uint32
    indx,
    place;
  uint8
    found;
  SOC_SAND_OCC_BM_PTR
    bitmap;
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;
  uint32
    soc_sand_ret;

  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  btmp_init_info.size = 64;


  soc_sand_ret = soc_sand_occ_bm_create(
               unit,
               &btmp_init_info,
               &bitmap
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  for(indx = 0 ; indx < 66; ++indx)
  {
    soc_sand_ret = soc_sand_occ_bm_get_next(
                 unit,
                 bitmap,
                 &place,
                 &found
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u \n"),place));
      if (found)
      {
        soc_sand_occ_bm_print(unit,bitmap);
      }
    }

    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 unit,
                 bitmap,
                 place,
                 TRUE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }

  }

exit:
  if (bitmap)
  {
    soc_sand_ret = soc_sand_occ_bm_destroy(
               unit,
               bitmap
             );

    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "Failed in soc_sand_occ_bm_destroy\n")));
        pass = FALSE;
    }
  }
  if (!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "\n\r ********** soc_sand_occ_bm_TEST_1 : END **********\n")));
  }
  return pass;
}



uint32
  soc_sand_occ_bm_TEST_2(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
)
{
  int32
    pass = TRUE;
  uint32
    indx,
    place;
  uint32
    expected_places[5];
  uint8
    found;
  SOC_SAND_OCC_BM_PTR
    bitmap;
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;
  uint32
    soc_sand_ret;

  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  btmp_init_info.size = 64;

  soc_sand_ret = soc_sand_occ_bm_create(
               unit,
               &btmp_init_info,
               &bitmap
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  for(indx = 0 ; indx < 66; ++indx)
  {
    soc_sand_ret = soc_sand_occ_bm_get_next(
                 unit,
                 bitmap,
                 &place,
                 &found
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u \n"),place));
      if (found)
      {
        soc_sand_occ_bm_print(unit,bitmap);
      }
    }

    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 unit,
                 bitmap,
                 place,
                 TRUE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }
 
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               unit,
               bitmap,
               20,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
 
  soc_sand_ret = soc_sand_occ_bm_get_next(
             unit,
             bitmap,
             &place,
             &found
           );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  if (!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "found %u \n"),place));
    if (found)
    {
      soc_sand_occ_bm_print(unit,bitmap);
    }
  }

  if (place != 20 )
  {
    pass = FALSE;
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u instead of 20\n"),place));
    }
  }

  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               unit,
               bitmap,
               place,
               TRUE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
 
  expected_places [0] = 0;
  expected_places [1] = 15;
  expected_places [2] = 32;
  expected_places [3] = 63;
  expected_places [4] = 0;

  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               unit,
               bitmap,
               63,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               unit,
               bitmap,
               0,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               unit,
               bitmap,
               15,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               unit,
               bitmap,
               32,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

 
  for(indx = 0 ; indx < 5; ++indx)
  {
    soc_sand_ret = soc_sand_occ_bm_get_next(
                 unit,
                 bitmap,
                 &place,
                 &found
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u \n"),place));
      if (found)
      {
        soc_sand_occ_bm_print(unit,bitmap);
      }
    }
    if (place != expected_places[indx] )
    {
      pass = FALSE;
      if (!silent)
      {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "found %u instead of %u\n"),place, expected_places[indx]));
      }
    }

    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 unit,
                 bitmap,
                 place,
                 TRUE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }

exit:
  if (bitmap)
  {
    soc_sand_ret = soc_sand_occ_bm_destroy(
               unit,
               bitmap
             );

    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "Failed in soc_sand_occ_bm_destroy\n")));
        pass = FALSE;
    }
  }
  if (!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "\n\r ********** soc_sand_occ_bm_TEST_2 : END **********\n")));
  }
  return pass;
}



uint32
  soc_sand_occ_bm_TEST_3(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
)
{
  int32
    pass = TRUE;
  uint32
    indx,
    place;
  uint32
    expected_places[5];
  uint8
    found;
  SOC_SAND_OCC_BM_PTR
    bitmap;
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;
  uint32
    soc_sand_ret;


  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  btmp_init_info.size = 700;

  soc_sand_ret = soc_sand_occ_bm_create(
               unit,
               &btmp_init_info,
               &bitmap
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  for(indx = 0 ; indx < 702; ++indx)
  {
    soc_sand_ret = soc_sand_occ_bm_get_next(
                 unit,
                 bitmap,
                 &place,
                 &found
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u \n"),place));
      if (found)
      {
        soc_sand_occ_bm_print(unit,bitmap);
      }
    }
    if ((indx < 700 && place != indx) || (indx >= 700 && found))
    {
      pass = FALSE;
      if (!silent)
      {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "found %u instead of %u\n"),place, indx));
      }
    }

    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 unit,
                 bitmap,
                 place,
                 TRUE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }
 
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               unit,
               bitmap,
               200,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
 
  soc_sand_ret = soc_sand_occ_bm_get_next(
             unit,
             bitmap,
             &place,
             &found
           );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  if (!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "found %u \n"),place));
    if (found)
    {
      soc_sand_occ_bm_print(unit,bitmap);
    }
  }

  if (place != 200 )
  {
    pass = FALSE;
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u instead of 200\n"),place));
    }
  }

  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               unit,
               bitmap,
               place,
               TRUE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
 
  expected_places [0] = 0;
  expected_places [1] = 150;
  expected_places [2] = 320;
  expected_places [3] = 630;
  expected_places [4] = 0;

  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               unit,
               bitmap,
               630,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               unit,
               bitmap,
               0,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               unit,
               bitmap,
               150,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               unit,
               bitmap,
               320,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

 
  for(indx = 0 ; indx < 5; ++indx)
  {
    soc_sand_ret = soc_sand_occ_bm_get_next(
                 unit,
                 bitmap,
                 &place,
                 &found
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u \n"),place));
      if (found)
      {
        soc_sand_occ_bm_print(unit,bitmap);
      }
    }
    if (place != expected_places[indx] )
    {
      pass = FALSE;
      if (!silent)
      {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "found %u instead of %u\n"),place, expected_places[indx]));
      }
    }

    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 unit,
                 bitmap,
                 place,
                 TRUE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }

exit:
  if (bitmap)
  {
    soc_sand_ret = soc_sand_occ_bm_destroy(
               unit,
               bitmap
             );

    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "Failed in soc_sand_occ_bm_destroy\n")));
        pass = FALSE;
    }
  }
  if (!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "\n\r ********** soc_sand_occ_bm_TEST_3 : END **********\n")));
  }
  return pass;
}



uint32
  soc_sand_occ_bm_TEST_4(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
)
{
  int32
    pass = TRUE;
  uint32
    indx,
    place,
    free_place;
  uint8
    found;
  SOC_SAND_OCC_BM_PTR
    bitmap;
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;
  uint32
    soc_sand_ret;

  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  btmp_init_info.size = 700;

  soc_sand_ret = soc_sand_occ_bm_create(
               unit,
               &btmp_init_info,
               &bitmap
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  for(indx = 0 ; indx < 702; ++indx)
  {
    soc_sand_ret = soc_sand_occ_bm_get_next(
                 unit,
                 bitmap,
                 &place,
                 &found
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u \n"),place));
      if (found)
      {
        soc_sand_occ_bm_print(unit,bitmap);
      }
    }
    if ((indx < 700 && place != indx) || (indx >= 700 && found))
    {
      pass = FALSE;
      if (!silent)
      {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "found %u instead of %u\n"),place, indx));
      }
    }

    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 unit,
                 bitmap,
                 place,
                 TRUE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }
  for (indx = 0; indx < 700; ++indx)
  {
   
    free_place = indx;
    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 unit,
                 bitmap,
                 free_place,
                 FALSE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
   
    soc_sand_ret = soc_sand_occ_bm_get_next(
               unit,
               bitmap,
               &place,
               &found
             );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u \n"),place));
      if (found)
      {
        soc_sand_occ_bm_print(unit,bitmap);
      }
    }

    if (place != free_place )
    {
      pass = FALSE;
      if (!silent)
      {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "found %u instead of %u\n"),place,free_place));
      }
    }

    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 unit,
                 bitmap,
                 place,
                 TRUE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }
exit:
  if (bitmap)
  {
    soc_sand_ret = soc_sand_occ_bm_destroy(
               unit,
               bitmap
             );

    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "Failed in soc_sand_occ_bm_destroy\n")));
        pass = FALSE;
    }
  }
  if (!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "\n\r ********** soc_sand_occ_bm_TEST_4 : END **********\n")));
  }
  return pass;
}

uint32
  soc_sand_occ_bm_tests(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
)
{
  int32
    pass = TRUE;

  pass &= soc_sand_occ_bm_TEST_1(
               unit,
               silent
             );

  pass &= soc_sand_occ_bm_TEST_2(
               unit,
               silent
             );

  pass &= soc_sand_occ_bm_TEST_3(
               unit,
               silent
             );


  pass &= soc_sand_occ_bm_TEST_4(
               unit,
               silent
             );
  if (!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "\n\r ********** soc_sand_occ_bm_tests : END **********\n")));
  }
  return pass;
}


#endif


#endif 

#include <soc/dpp/SAND/Utils/sand_footer.h>

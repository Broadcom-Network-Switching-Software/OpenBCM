/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef SOC_SAND_SORTED_LIST_H_INCLUDED

#define SOC_SAND_SORTED_LIST_H_INCLUDED



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>





#define ENHANCED_SORTED_LIST_SEARCH 1


#define MAX_NOF_LISTS                     (200 * SOC_DPP_DEFS_GET(unit, nof_cores))
#define SOC_SAND_SORTED_LIST_NULL         SOC_SAND_U32_MAX
#define SOC_SAND_SORTED_LIST_HEADER_SIZE  160








#define SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,list) soc_sand_sorted_list_get_iter_begin_or_end(unit,list,1)

#define SOC_SAND_SORTED_LIST_ITER_END(unit,list)  soc_sand_sorted_list_get_iter_begin_or_end(unit,list,0)





typedef enum
{
    SOC_SAND_SORTED_LIST_CMP_FUNC_TYPE_DEFAULT,
    SOC_SAND_SORTED_LIST_CMP_FUNC_TYPE_TCAM,
    SOC_SAND_SORTED_LIST_CMP_FUNC_NOF_TYPES
} SOC_SAND_SORTED_LIST_CMP_FUNC_TYPE;



typedef uint8 SOC_SAND_SORTED_LIST_KEY;

typedef uint8 SOC_SAND_SORTED_LIST_DATA;



typedef uint32 SOC_SAND_SORTED_LIST_ITER;


typedef
  int32
    (*SOC_SAND_SORTED_LIST_KEY_CMP)(
      SOC_SAND_IN uint8                 *buffer1,
      SOC_SAND_IN uint8                 *buffer2,
      uint32                size
    );


typedef
  uint32
    (*SOC_SAND_SORTED_LIST_SW_DB_ENTRY_SET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_INOUT  uint8                 *buffer,
      SOC_SAND_IN  uint32                   offset,
      SOC_SAND_IN  uint32                   len,
      SOC_SAND_IN uint8                     *data
    );


typedef
  uint32
    (*SOC_SAND_SORTED_LIST_SW_DB_ENTRY_GET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_IN  uint8                    *buffer,
      SOC_SAND_IN  uint32                   offset,
      SOC_SAND_IN  uint32                   len,
      SOC_SAND_OUT uint8              *data
    );



typedef
  void
    (*SOC_SAND_SORTED_LIST_PRINT_VAL)(
      SOC_SAND_IN  uint8                    *buffer
    );


typedef struct {
  
  int prime_handle;
  
  uint32 sec_handle;
  
  uint32 list_size;
  
  uint32 key_size;
  
  uint32 data_size;

  SOC_SAND_SORTED_LIST_CMP_FUNC_TYPE cmp_func_type;

  SOC_SAND_SORTED_LIST_SW_DB_ENTRY_GET get_entry_fun;

  SOC_SAND_SORTED_LIST_SW_DB_ENTRY_SET set_entry_fun;

} SOC_SAND_SORTED_LIST_INIT_INFO ;


typedef struct
{
  
  SW_STATE_BUFF *keys;
  
  SW_STATE_BUFF *next;
  
  SW_STATE_BUFF *prev;
  
  SW_STATE_BUFF *data;
  
  PARSER_HINT_ARR uint32 *indices ;
  
  uint32 num_elements_on_indices ;
  
  uint32 ptr_size;
  
  SOC_SAND_OCC_BM_PTR memory_use ;
  
  SW_STATE_BUFF *tmp_data;
  
  SW_STATE_BUFF *tmp_key ;
  
  uint32  null_ptr ;

} SOC_SAND_SORTED_LIST_T ;

typedef struct
{
  SOC_SAND_SORTED_LIST_INIT_INFO  init_info ;
  SOC_SAND_SORTED_LIST_T          list_data ;
} SOC_SAND_SORTED_LIST_INFO ;



typedef uint32 SOC_SAND_SORTED_LIST_PTR ;

typedef struct soc_sand_sw_state_sorted_list_s
{
                      uint32                      max_nof_lists ;
                      uint32                      in_use ;
  PARSER_HINT_ARR_PTR SOC_SAND_SORTED_LIST_INFO   **lists_array ;
  PARSER_HINT_ARR     SHR_BITDCL                  *occupied_lists ;
} soc_sand_sw_state_sorted_list_t;










uint32
  soc_sand_sorted_list_init(
    SOC_SAND_IN       int                          unit,
    SOC_SAND_IN       uint32                       max_nof_lists
  ) ;

uint32
  soc_sand_sorted_list_clear_all_tmps(
    SOC_SAND_IN int unit
  ) ;

uint32
  soc_sand_sorted_list_create(
    SOC_SAND_IN     int                             unit,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_PTR        *sorted_list_ptr,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_INIT_INFO  init_info
  );


uint32
  soc_sand_sorted_list_clear(
    SOC_SAND_IN     int                            unit,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_PTR       sorted_list
  );


uint32
  soc_sand_sorted_list_destroy(
    SOC_SAND_IN     int                         unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR    sorted_list
  );


uint32
  soc_sand_sorted_list_entry_add(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_KEY*     const key,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_DATA*    const data,
    SOC_SAND_OUT    uint8                        *success
  ) ;


uint32
  soc_sand_sorted_list_entry_update(
    SOC_SAND_IN     int                               unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR          sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_ITER         iter,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_KEY*         const key,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_DATA*        const data
  );


uint32
  soc_sand_sorted_list_entry_add_by_iter(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_ITER     pos,
    SOC_SAND_IN     uint8                         before,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_KEY*     const key,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_DATA*    const data,
    SOC_SAND_OUT    uint8                         *success
  );


uint32
  soc_sand_sorted_list_entry_remove_by_iter(
    SOC_SAND_IN     int                             unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR        sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_ITER       iter
  );



uint32
  soc_sand_sorted_list_entry_lookup(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_KEY      *const key,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_DATA     *const data,
    SOC_SAND_OUT    uint8                         *found,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_ITER     *iter
  );


uint32
  soc_sand_sorted_list_entry_value(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR     sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_ITER    iter,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_KEY*    const key,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_DATA*   const data
  );


uint32
  soc_sand_sorted_list_get_next(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_ITER     *iter,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_KEY      *const key,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_DATA     *const data
  );


uint32
  soc_sand_sorted_list_get_prev(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_ITER     *iter,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_KEY      *const key,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_DATA     *const data
  );


uint32
  soc_sand_sorted_list_get_follow(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_ITER     *iter,
    SOC_SAND_OUT    uint8                         forward,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_KEY      *const key,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_DATA     *const data
  );


uint32
  soc_sand_sorted_list_get_size_for_save(
    SOC_SAND_IN   int                      unit,
    SOC_SAND_IN   SOC_SAND_SORTED_LIST_PTR sorted_list,
    SOC_SAND_OUT  uint32                   *size
  ) ;


uint32
  soc_sand_sorted_list_save(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_OUT uint8                         *buffer,
    SOC_SAND_IN  uint32                        buffer_size_bytes,
    SOC_SAND_OUT uint32                        *actual_size_bytes
  );



uint32
  soc_sand_sorted_list_load(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint8                                 **buffer,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_SW_DB_ENTRY_GET  get_entry_fun,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_SW_DB_ENTRY_SET  set_entry_fun,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY_CMP          cmp_key_fun,                      
    SOC_SAND_OUT SOC_SAND_SORTED_LIST_PTR              *sorted_list_ptr                     
  );


uint32
  soc_sand_sorted_list_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR          sorted_list,
    SOC_SAND_IN  char                              table_header[SOC_SAND_SORTED_LIST_HEADER_SIZE],
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PRINT_VAL    print_key,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PRINT_VAL    print_data
  ) ;

uint32
  soc_sand_print_indices(
    int                      unit,
    SOC_SAND_SORTED_LIST_PTR sorted_list
  ) ;

uint32
  soc_sand_print_list(
    int                      unit,
    SOC_SAND_SORTED_LIST_PTR sorted_list
  ) ;

#ifdef SOC_SAND_DEBUG





uint32
  soc_sand_SAND_SORTED_LIST_INFO_clear(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_SAND_SORTED_LIST_PTR info
  );

uint32
  soc_sand_SAND_SORTED_LIST_INFO_print(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_SAND_SORTED_LIST_PTR info
  );

#endif 


uint32
  soc_sand_sorted_list_get_iter_begin_or_end(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_IN   int                           get_begin
  ) ;



uint32
  soc_sand_sorted_list_find_higher_eq_key(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR  sorted_list,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY  *const key,
    SOC_SAND_OUT SOC_SAND_SORTED_LIST_ITER *iter,
    SOC_SAND_OUT uint8                     *found_equal
  ) ;


uint32
  soc_sand_sorted_list_find_lower_eq_key(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR  sorted_list,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY  *const key,
    SOC_SAND_OUT SOC_SAND_SORTED_LIST_ITER *iter,
    SOC_SAND_OUT uint32                    *index,
    SOC_SAND_OUT uint8                     *found_equal
  ) ;

uint32
  soc_sand_sorted_list_get_num_elements(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR sorted_list,
    SOC_SAND_OUT uint32                   *num_elements
  ) ;

uint32
  soc_sand_sorted_list_is_indices_enabled(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR sorted_list,
    SOC_SAND_OUT uint8                    *is_enabled
  ) ;

uint32
  soc_sand_sorted_list_get_iter_from_indices(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR  sorted_list,
    SOC_SAND_IN  uint32                    index_in_indices,
    SOC_SAND_OUT SOC_SAND_SORTED_LIST_ITER *iter
  ) ;

uint32
  soc_sand_sorted_list_get_index_from_iter(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR  sorted_list,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_ITER iter,
    SOC_SAND_OUT uint32                    *index_in_indices
  ) ;

#include <soc/dpp/SAND/Utils/sand_footer.h>
   

#endif



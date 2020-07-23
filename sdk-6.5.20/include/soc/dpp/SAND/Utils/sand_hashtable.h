/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef SOC_SAND_HASHTABLE_H_INCLUDED

#define SOC_SAND_HASHTABLE_H_INCLUDED



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>





#define MAX_NOF_HASHS             (200 * SOC_DPP_DEFS_GET(unit, nof_cores))
#define SOC_SAND_HASH_TABLE_NULL  SOC_SAND_U32_MAX







#define SOC_SAND_HASH_TABLE_ITER_SET_BEGIN(iter) ((*iter) = 0)

#define SOC_SAND_HASH_TABLE_ITER_IS_END(iter)    ((*iter) == SOC_SAND_U32_MAX)







typedef uint8 SOC_SAND_HASH_TABLE_KEY;

typedef uint8 *SOC_SAND_HASH_TABLE_DATA ;



typedef uint32 SOC_SAND_HASH_TABLE_ITER;

typedef uint32 SOC_SAND_HASH_TABLE_PTR ;


typedef
  uint32
    (*SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK)(
      SOC_SAND_IN  int                         unit,
      SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR     hash_table,
      SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY     *const key,
      SOC_SAND_IN  uint32                      seed,
      SOC_SAND_OUT uint32*                     hash_val
    );

typedef
  uint32
    (*SOC_SAND_HASH_MAP_SW_DB_ENTRY_SET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_INOUT  uint8                 *buffer,
      SOC_SAND_IN  uint32                   offset,
      SOC_SAND_IN  uint32                   len,
      SOC_SAND_IN uint8                     *data
    );

typedef
  uint32
    (*SOC_SAND_HASH_MAP_SW_DB_ENTRY_GET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_IN  uint8                    *buffer,
      SOC_SAND_IN  uint32                   offset,
      SOC_SAND_IN  uint32                   len,
      SOC_SAND_OUT uint8*                   const data
    );


typedef struct {
  
  int prime_handle;
  
  uint32 sec_handle;
  
  uint32 table_size;
  
  uint32 table_width;
  
  uint32 key_size;
  
  uint32 data_size;
  
  SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK hash_function;
  
  SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK rehash_function;

  SOC_SAND_HASH_MAP_SW_DB_ENTRY_GET get_entry_fun;

  SOC_SAND_HASH_MAP_SW_DB_ENTRY_SET set_entry_fun;

} SOC_SAND_HASH_TABLE_INIT_INFO ;

typedef struct
{
  
  SW_STATE_BUFF *lists_head;
  
  SW_STATE_BUFF *keys;
  
  SW_STATE_BUFF *next;
  
  uint32        ptr_size;
  
  SOC_SAND_OCC_BM_PTR memory_use ;
  
  uint32        null_ptr;
  
  SW_STATE_BUFF *tmp_buf ;
  SW_STATE_BUFF *tmp_buf2 ;

} SOC_SAND_HASH_TABLE_T;


typedef struct
{
  SOC_SAND_HASH_TABLE_INIT_INFO  init_info ;
  SOC_SAND_HASH_TABLE_T          hash_data ;
} SOC_SAND_HASH_TABLE_INFO ;




typedef struct soc_sand_sw_state_hash_table_s
{
                      uint32                      max_nof_hashs ;
                      uint32                      in_use ;
  PARSER_HINT_ARR_PTR SOC_SAND_HASH_TABLE_INFO   **hashs_array ;
  PARSER_HINT_ARR     SHR_BITDCL                  *occupied_hashs ;
} soc_sand_sw_state_hash_table_t ;









uint32
  soc_sand_hash_table_simple_hash(
                            SOC_SAND_IN  int                         unit,
                            SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR     hash_table,
                            SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY*    const key,
                            SOC_SAND_IN  uint32                seed,
                            SOC_SAND_OUT  uint32*              hash_val
                            ) ;

uint32
  soc_sand_hash_table_get_illegal_hashtable_handle(
    void
  ) ;


uint32
  soc_sand_hash_table_get_table_size(
    SOC_SAND_IN     int                              unit,
    SOC_SAND_IN    SOC_SAND_HASH_TABLE_PTR           hash_table,
    SOC_SAND_INOUT uint32                            *table_size_ptr
  ) ;


uint32
  soc_sand_hash_table_init(
    SOC_SAND_IN       int                          unit,
    SOC_SAND_IN       uint32                       max_nof_hashs
  ) ;

uint32
  soc_sand_hash_table_clear_all_tmps(
    SOC_SAND_IN int unit
  ) ;

uint32
  soc_sand_hash_table_create(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR           *hash_table,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_INIT_INFO     init_info
  );


uint32
  soc_sand_hash_table_destroy(
    SOC_SAND_IN     int                        unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR    hash_table
  ) ;


uint32
  soc_sand_hash_table_entry_add(
    SOC_SAND_IN    int                        unit,
    SOC_SAND_IN    SOC_SAND_HASH_TABLE_PTR    hash_table,
    SOC_SAND_IN    SOC_SAND_HASH_TABLE_KEY    *const key,
    SOC_SAND_OUT   uint32                     *data_indx,
    SOC_SAND_OUT   uint8                      *success
  ) ;

uint32
  soc_sand_hash_table_entry_add_at_index(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_PTR      hash_table,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_KEY      *const key,
    SOC_SAND_IN     uint32                       data_indx,
    SOC_SAND_OUT    uint8                        *success
  ) ;


uint32
  soc_sand_hash_table_entry_remove(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_PTR      hash_table,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_KEY      *const key
  ) ;


uint32
  soc_sand_hash_table_entry_remove_by_index(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_PTR hash_table,
    SOC_SAND_IN     uint32                  data_indx
  ) ;


uint32
  soc_sand_hash_table_entry_lookup(
    SOC_SAND_IN     int                         unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR     hash_table,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_KEY     *const key,
    SOC_SAND_OUT    uint32                      *data_indx,
    SOC_SAND_OUT    uint8                       *found
  ) ;

uint32
  soc_sand_hash_table_get_by_index(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR       hash_table,
    SOC_SAND_IN  uint32                        data_indx,
    SOC_SAND_OUT SOC_SAND_HASH_TABLE_KEY       *key,
    SOC_SAND_OUT uint8                         *found
  ) ;

uint32
  soc_sand_hash_table_clear(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR    hash_table
  ) ;


uint32
  soc_sand_hash_table_get_next(
    SOC_SAND_IN     int                         unit,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_PTR     hash_table,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_ITER    *iter,
    SOC_SAND_OUT    SOC_SAND_HASH_TABLE_KEY     *const key,
    SOC_SAND_OUT    uint32                      *data_indx
  );



uint32
  soc_sand_hash_table_get_size_for_save(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   SOC_SAND_HASH_TABLE_PTR        hash_table,
    SOC_SAND_OUT  uint32                         *size
  );


uint32
  soc_sand_hash_table_save(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR      hash_table,
    SOC_SAND_OUT uint8                        *buffer,
    SOC_SAND_IN  uint32                       buffer_size_bytes,
    SOC_SAND_OUT uint32                       *actual_size_bytes
  );



uint32
  soc_sand_hash_table_load(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint8                                 **buffer,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_SW_DB_ENTRY_SET     set_entry_fun,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_SW_DB_ENTRY_GET     get_entry_fun,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK  hash_function,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK  rehash_function,
    SOC_SAND_OUT SOC_SAND_HASH_TABLE_PTR               *hash_table_ptr
  );

uint32
  soc_sand_SAND_HASH_TABLE_INFO_clear(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR  hash_table
  ) ;


#ifdef SOC_SAND_DEBUG


uint32
  soc_sand_hash_table_print(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR hash_table
  );


uint32
  soc_sand_hash_table_tests(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
);

uint32
  soc_sand_SAND_HASH_TABLE_INFO_print(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR hash_table
  ) ;

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif



/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef SOC_SAND_MULTI_SET_H_INCLUDED

#define SOC_SAND_MULTI_SET_H_INCLUDED



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_hashtable.h>





#define MAX_NOF_MULTIS           (200 * SOC_DPP_DEFS_GET(unit, nof_cores))
#define SOC_SAND_MULTI_SET_NULL  SOC_SAND_U32_MAX






#define SOC_SAND_MULTI_SET_ITER_BEGIN(iter) (iter = 0)

#define SOC_SAND_MULTI_SET_ITER_END(iter)   (iter == SOC_SAND_U32_MAX)







typedef uint8 SOC_SAND_MULTI_SET_KEY ;

typedef uint8* SOC_SAND_MULTI_SET_DATA ;

typedef uint32 SOC_SAND_MULTI_SET_ITER ;

typedef uint32 SOC_SAND_MULTI_SET_PTR ;


typedef
  uint32
    (*SOC_SAND_MULTISET_SW_DB_ENTRY_SET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_INOUT  uint8                 *buffer,
      SOC_SAND_IN  uint32                   offset,
      SOC_SAND_IN  uint32                   len,
      SOC_SAND_IN uint8                     *data
    );


typedef
  uint32
    (*SOC_SAND_MULTISET_SW_DB_ENTRY_GET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_IN  uint8                    *buffer,
      SOC_SAND_IN  uint32                   offset,
      SOC_SAND_IN  uint32                   len,
      SOC_SAND_OUT uint8*                   data
    );


typedef
  void
    (*SOC_SAND_MULTI_SET_PRINT_VAL)(
      SOC_SAND_IN  uint8                    *buffer
    );

 
 typedef struct {
  
  int prime_handle ;
  
  uint32 sec_handle ;
  
  uint32 nof_members ;
  
  uint32 member_size ;
  
  uint32 max_duplications ;
  
  uint32 global_max ;
  SOC_SAND_MULTISET_SW_DB_ENTRY_GET get_entry_fun ;
  SOC_SAND_MULTISET_SW_DB_ENTRY_SET set_entry_fun ;
} SOC_SAND_MULTI_SET_INIT_INFO ;

typedef struct
{
  
  SW_STATE_BUFF           *ref_counter ;
  
  uint32                  counter_size ;
  
  uint32                  global_counter;
  
  SOC_SAND_HASH_TABLE_PTR hash_table ;

} SOC_SAND_MULTI_SET_T ;

typedef struct
{
  SOC_SAND_MULTI_SET_INIT_INFO   init_info ;
  SOC_SAND_MULTI_SET_T           multiset_data ;
} SOC_SAND_MULTI_SET_INFO ;


typedef struct soc_sand_sw_state_multi_set_s
{
	uint32                      max_nof_multis ;
	uint32                      in_use ;
	PARSER_HINT_ARR_PTR SOC_SAND_MULTI_SET_INFO   **multis_array ;
	PARSER_HINT_ARR     SHR_BITDCL                *occupied_multis ;
} soc_sand_sw_state_multi_set_t ;












uint32
  soc_sand_multi_set_get_member_size(
    SOC_SAND_IN    int                              unit,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_PTR           multi_set,
    SOC_SAND_INOUT uint32                           *member_size_ptr
  ) ;

uint32
  soc_sand_multi_set_init(
    SOC_SAND_IN       int                          unit,
    SOC_SAND_IN       uint32                       max_nof_multis
  ) ;

uint32
  soc_sand_multi_set_create(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_PTR        *multi_set_ptr,
    SOC_SAND_IN     SOC_SAND_MULTI_SET_INIT_INFO  init_info
  );


uint32
  soc_sand_multi_set_destroy(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_PTR  multi_set
  );


uint32
  soc_sand_multi_set_member_add(
    SOC_SAND_IN    int                     unit,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_PTR  multi_set,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_KEY  *const key,
    SOC_SAND_OUT   uint32                  *data_indx,
    SOC_SAND_OUT   uint8                   *first_appear,
    SOC_SAND_OUT   uint8                   *success
  );


uint32
  soc_sand_multi_set_member_add_at_index(
    SOC_SAND_IN    int                        unit,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_PTR     multi_set,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_KEY     *const key,
    SOC_SAND_IN    uint32                     data_indx,
    SOC_SAND_OUT   uint8                      *first_appear,
    SOC_SAND_OUT   uint8                      *success
  );


uint32
  soc_sand_multi_set_member_add_at_index_nof_additions(
    SOC_SAND_IN    int                         unit,
    SOC_SAND_INOUT SOC_SAND_MULTI_SET_PTR      multi_set,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_KEY      *const key,
    SOC_SAND_IN    uint32                      data_indx,
    SOC_SAND_IN    uint32                      nof_additions,
    SOC_SAND_OUT   uint8                       *first_appear,
    SOC_SAND_OUT   uint8                       *success
  );


uint32
  soc_sand_multi_set_member_remove(
    SOC_SAND_IN    int                      unit,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_PTR   multi_set,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_KEY   *const key,
    SOC_SAND_OUT   uint32                   *data_indx,
    SOC_SAND_OUT   uint8                    *last_appear
  );



uint32
  soc_sand_multi_set_member_remove_by_index(
    SOC_SAND_IN    int                       unit,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_PTR    multi_set,
    SOC_SAND_IN    uint32                    data_indx,
    SOC_SAND_OUT   uint8                     *last_appear
  );


uint32
  soc_sand_multi_set_member_remove_by_index_multiple(
    SOC_SAND_IN    int                      unit,
    SOC_SAND_INOUT SOC_SAND_MULTI_SET_PTR   multi_set,
    SOC_SAND_IN    uint32                   data_indx,
    SOC_SAND_IN    uint32                   remove_amt,
    SOC_SAND_OUT   uint8                    *last_appear
  );


uint32
  soc_sand_multi_set_member_lookup(
    SOC_SAND_IN    int                      unit,
    SOC_SAND_IN  SOC_SAND_MULTI_SET_PTR     multi_set,
    SOC_SAND_IN  SOC_SAND_MULTI_SET_KEY     *const key,
    SOC_SAND_OUT  uint32                    *data_indx,
    SOC_SAND_OUT  uint32                    *ref_count
  );


uint32
  soc_sand_multi_set_get_next(
    SOC_SAND_IN    int                      unit,
    SOC_SAND_IN    SOC_SAND_MULTI_SET_PTR   multi_set,
    SOC_SAND_INOUT SOC_SAND_MULTI_SET_ITER  *iter,
    SOC_SAND_OUT   SOC_SAND_MULTI_SET_KEY   *key,
    SOC_SAND_OUT   uint32                   *data_indx,
    SOC_SAND_OUT   uint32                   *ref_count
  );


uint32
  soc_sand_multi_set_get_by_index(
    SOC_SAND_IN   int                      unit,
    SOC_SAND_IN   SOC_SAND_MULTI_SET_PTR   multi_set,
    SOC_SAND_IN   uint32                   data_indx,
    SOC_SAND_OUT  SOC_SAND_MULTI_SET_KEY  *key,
    SOC_SAND_OUT  uint32                  *ref_count
  );


uint32
  soc_sand_multi_set_get_size_for_save(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   SOC_SAND_MULTI_SET_PTR        multi_set,
    SOC_SAND_OUT  uint32                        *size
  );


uint32
  soc_sand_multi_set_save(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_SAND_MULTI_SET_PTR      multi_set,
    SOC_SAND_OUT uint8                       *buffer,
    SOC_SAND_IN  uint32                      buffer_size_bytes,
    SOC_SAND_OUT uint32                      *actual_size_bytes
  );


uint32
  soc_sand_multi_set_load(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint8                                **buffer,
    SOC_SAND_IN  SOC_SAND_MULTISET_SW_DB_ENTRY_SET    set_function,
    SOC_SAND_IN  SOC_SAND_MULTISET_SW_DB_ENTRY_GET    get_function,
    SOC_SAND_OUT SOC_SAND_MULTI_SET_PTR               *multi_set
  );


uint32
  soc_sand_multi_set_clear(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     SOC_SAND_MULTI_SET_PTR  multi_set
  );

#ifdef SOC_SAND_DEBUG


uint32
  soc_sand_multi_set_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_SAND_MULTI_SET_PTR        multi_set,
    SOC_SAND_IN  SOC_SAND_MULTI_SET_PRINT_VAL  print_fun,
    SOC_SAND_IN  uint8                         clear_on_print
  );




uint32
  soc_sand_SAND_MULTI_SET_INFO_print(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN SOC_SAND_MULTI_SET_PTR multi_set
  ) ;

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


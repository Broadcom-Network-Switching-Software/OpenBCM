/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/
#ifndef SOC_SAND_OCCUPATION_BITMAP_H_INCLUDED

#define SOC_SAND_OCCUPATION_BITMAP_H_INCLUDED





#include <shared/swstate/sw_state.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>






#define MAX_NOF_DSS (500 * SOC_DPP_DEFS_GET(unit, nof_cores))
















typedef struct
{
  
  uint32 size;
  
  uint8  init_val;
  
  uint8  support_cache;

}SOC_SAND_OCC_BM_INIT_INFO;


typedef struct 
{
  
  PARSER_HINT_ARR uint32 *levels;
  
  PARSER_HINT_ARR uint32 *levels_size;
  
  uint32  nof_levels;
  
  uint32  size;
  
  uint8  init_val;
   
  uint8  support_cache;
  uint8  cache_enabled;
  
  PARSER_HINT_ARR uint32 *levels_cache;

  
  SW_STATE_BUFF *levels_buffer;
  SW_STATE_BUFF *levels_cache_buffer;

  
  uint32 buffer_size;

} SOC_SAND_OCC_BM_T;



typedef uint32  SOC_SAND_OCC_BM_PTR ;


typedef struct soc_sand_sw_state_occ_bitmap_s
{
                      uint32              max_nof_dss;
                      uint32              in_use;
  PARSER_HINT_ARR_PTR SOC_SAND_OCC_BM_T   **dss_array;
  PARSER_HINT_ARR     SHR_BITDCL          *occupied_dss;
} soc_sand_sw_state_occ_bitmap_t;









uint32
  soc_sand_occ_bm_init(
    SOC_SAND_IN       int                          unit,
    SOC_SAND_IN       uint32                       max_nof_dss
  ) ;


uint32
  soc_sand_occ_bm_get_ptr_from_handle(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR          bit_map,
    SOC_SAND_OUT SOC_SAND_OCC_BM_T            **bit_map_ptr_ptr
  ) ;


uint32
  soc_sand_occ_bm_create(
    SOC_SAND_IN       int                         unit,
    SOC_SAND_IN       SOC_SAND_OCC_BM_INIT_INFO  *info,
    SOC_SAND_OUT      SOC_SAND_OCC_BM_PTR        *bit_map
  );


uint32
  soc_sand_occ_bm_destroy(
    SOC_SAND_IN        int                   unit,
    SOC_SAND_OUT       SOC_SAND_OCC_BM_PTR   bit_map
  );


uint32
  soc_sand_occ_bm_clear(
    SOC_SAND_IN        int                   unit,
    SOC_SAND_OUT       SOC_SAND_OCC_BM_PTR   bit_map
  );


uint32
  soc_sand_occ_bm_get_next(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR   bit_map,
    SOC_SAND_OUT uint32               *place,
    SOC_SAND_OUT uint8                *found
  );


uint32
  soc_sand_occ_bm_get_next_in_range(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR       bit_map,
    SOC_SAND_IN  uint32                    start,
    SOC_SAND_IN  uint32                    end,
    SOC_SAND_IN  uint8                     forward,
    SOC_SAND_OUT uint32                   *place,
    SOC_SAND_OUT uint8                    *found
  );


uint32
  soc_sand_occ_bm_alloc_next(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR         bit_map,
    SOC_SAND_OUT  uint32                    *place,
    SOC_SAND_OUT  uint8                     *found
  );


uint32
  soc_sand_occ_bm_occup_status_set(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_INOUT  SOC_SAND_OCC_BM_PTR     bit_map,
    SOC_SAND_IN     uint32                  place,
    SOC_SAND_IN     uint8                   occupied
  );


uint32
  soc_sand_occ_bm_is_occupied(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR        bit_map,
    SOC_SAND_IN  uint32                     place,
    SOC_SAND_OUT uint8                     *occupied
  );

uint32
  soc_sand_occ_bm_cache_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR        bit_map,
    SOC_SAND_IN  uint8                      cached
  );

uint32
  soc_sand_occ_bm_cache_commit(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR        bit_map,
    SOC_SAND_IN  uint32                     flags
  );

uint32
  soc_sand_occ_bm_cache_rollback(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR        bit_map,
    SOC_SAND_IN  uint32                     flags
  );


uint32
  soc_sand_occ_bm_get_size_for_save(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR          bit_map,
    SOC_SAND_OUT  uint32                     *size
  );


uint32
  soc_sand_occ_bm_save(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR    bit_map,
    SOC_SAND_OUT uint8                 *buffer,
    SOC_SAND_IN  uint32                 buffer_size_bytes,
    SOC_SAND_OUT uint32                *actual_size_bytes
  );



uint32
  soc_sand_occ_bm_load(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                           **buffer,
    SOC_SAND_OUT  SOC_SAND_OCC_BM_PTR             *bit_map
  );
void
  soc_sand_SAND_OCC_BM_INIT_INFO_clear(
    SOC_SAND_INOUT SOC_SAND_OCC_BM_INIT_INFO *info
  );

uint32
  soc_sand_occ_bm_get_buffer_size(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR          bit_map,
    SOC_SAND_OUT  uint32                      *buffer_size_p
  ) ;
uint32
  soc_sand_occ_bm_get_support_cache(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR          bit_map,
    SOC_SAND_OUT  uint8                       *support_cache_p
  ) ;

uint32
   soc_sand_occ_bm_get_illegal_bitmap_handle(void) ;

uint32
  soc_sand_occ_is_bitmap_active(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR  bit_map,
    SOC_SAND_OUT uint8                *in_use
  ) ;


uint32
  soc_sand_occ_bm_print(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN SOC_SAND_OCC_BM_PTR bit_map
  );

#if SOC_SAND_DEBUG


uint32
  soc_sand_occ_bm_tests(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
);

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif


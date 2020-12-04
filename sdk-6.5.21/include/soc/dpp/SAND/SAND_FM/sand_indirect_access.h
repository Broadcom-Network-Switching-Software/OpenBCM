/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/
#ifndef SOC_SAND_INDIRECT_ACCESS_H
#define SOC_SAND_INDIRECT_ACCESS_H
#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>




#define SOC_SAND_MODULE_MS_BIT           31
#define SOC_SAND_MODULE_NUM_BITS         5
#define SOC_SAND_MODULE_LS_BIT           (SOC_SAND_MODULE_MS_BIT + 1 - SOC_SAND_MODULE_NUM_BITS)
#define SOC_SAND_MODULE_MASK   \
    (((uint32)SOC_SAND_BIT(SOC_SAND_MODULE_MS_BIT) - SOC_SAND_BIT(SOC_SAND_MODULE_LS_BIT)) + (uint32)SOC_SAND_BIT(SOC_SAND_MODULE_MS_BIT))
#define SOC_SAND_MODULE_SHIFT            SOC_SAND_MODULE_LS_BIT

#define SOC_SAND_RD_NOT_WR_MS_BIT        31
#define SOC_SAND_RD_NOT_WR_NUM_BITS      1
#define SOC_SAND_RD_NOT_WR_LS_BIT        (SOC_SAND_RD_NOT_WR_MS_BIT + 1 - SOC_SAND_RD_NOT_WR_NUM_BITS)
#define SOC_SAND_RD_NOT_WR_MASK   \
    (((uint32)SOC_SAND_BIT(SOC_SAND_RD_NOT_WR_MS_BIT) - SOC_SAND_BIT(SOC_SAND_RD_NOT_WR_LS_BIT)) + SOC_SAND_BIT(SOC_SAND_RD_NOT_WR_MS_BIT))



#define SOC_SAND_TBL_READ_NO_RVRS 0


#define SOC_SAND_TBL_READ_RVRS_BITS_WORD_MSB    31
#define SOC_SAND_TBL_READ_RVRS_BITS_WORD_LSB    30

#define SOC_SAND_TBL_READ_SIZE_BITS_WORD_MSB    11
#define SOC_SAND_TBL_READ_SIZE_BITS_WORD_LSB     0

#define SOC_SAND_GET_TBL_READ_RVRS_BITS_FROM_WORD(word)                                               \
          SOC_SAND_GET_FLD_FROM_PLACE(                                                                     \
            word,                                                                                 \
            SOC_SAND_TBL_READ_RVRS_BITS_WORD_LSB,                                                     \
            SOC_SAND_BITS_MASK(SOC_SAND_TBL_READ_RVRS_BITS_WORD_MSB, SOC_SAND_TBL_READ_RVRS_BITS_WORD_LSB)    \
          )


#define SOC_SAND_GET_TBL_READ_SIZE_BITS_FROM_WORD(word)                                               \
          SOC_SAND_GET_FLD_FROM_PLACE(                                                                     \
            word,                                                                                 \
            SOC_SAND_TBL_READ_SIZE_BITS_WORD_LSB,                                                     \
            SOC_SAND_BITS_MASK(SOC_SAND_TBL_READ_SIZE_BITS_WORD_MSB, SOC_SAND_TBL_READ_SIZE_BITS_WORD_LSB)    \
          )


#define SOC_SAND_TBL_WRITE_NO_RVRS 0


#define SOC_SAND_TBL_WRITE_RVRS_BITS_WORD_MSB   31
#define SOC_SAND_TBL_WRITE_RVRS_BITS_WORD_LSB   30

#define SOC_SAND_TBL_WRITE_REPT_BITS_WORD_MSB   29
#define SOC_SAND_TBL_WRITE_REPT_BITS_WORD_LSB   12

#define SOC_SAND_TBL_WRITE_SIZE_BITS_WORD_MSB   11
#define SOC_SAND_TBL_WRITE_SIZE_BITS_WORD_LSB    0

#define SOC_SAND_GET_TBL_WRITE_REPT_BITS_FROM_WORD(word)                                              \
          SOC_SAND_GET_FLD_FROM_PLACE(                                                                     \
            word,                                                                                 \
            SOC_SAND_TBL_WRITE_REPT_BITS_WORD_LSB,                                                    \
            SOC_SAND_BITS_MASK(SOC_SAND_TBL_WRITE_REPT_BITS_WORD_MSB, SOC_SAND_TBL_WRITE_REPT_BITS_WORD_LSB)  \
          )

#define SOC_SAND_GET_TBL_WRITE_SIZE_BITS_FROM_WORD(word)                                              \
          SOC_SAND_GET_FLD_FROM_PLACE(                                                                     \
            word,                                                                                 \
            SOC_SAND_TBL_WRITE_SIZE_BITS_WORD_LSB,                                                    \
            SOC_SAND_BITS_MASK(SOC_SAND_TBL_WRITE_SIZE_BITS_WORD_MSB, SOC_SAND_TBL_WRITE_SIZE_BITS_WORD_LSB)  \
          )

#define SOC_SAND_GET_TBL_WRITE_RVRS_BITS_FROM_WORD(word)                                              \
          SOC_SAND_GET_FLD_FROM_PLACE(                                                                     \
            word,                                                                                 \
            SOC_SAND_TBL_WRITE_RVRS_BITS_WORD_LSB,                                                    \
            SOC_SAND_BITS_MASK(SOC_SAND_TBL_WRITE_RVRS_BITS_WORD_MSB, SOC_SAND_TBL_WRITE_RVRS_BITS_WORD_LSB)  \
          )

#define SOC_SAND_SET_TBL_WRITE_REPT_BITS_IN_WORD(word)                                                \
          SOC_SAND_SET_FLD_IN_PLACE(                                                                       \
            word,                                                                                 \
            SOC_SAND_TBL_WRITE_REPT_BITS_WORD_LSB,                                                    \
            SOC_SAND_BITS_MASK(SOC_SAND_TBL_WRITE_REPT_BITS_WORD_MSB, SOC_SAND_TBL_WRITE_REPT_BITS_WORD_LSB)  \
          )

#define SOC_SAND_SET_TBL_WRITE_SIZE_BITS_IN_WORD(word)                                                \
          SOC_SAND_SET_FLD_IN_PLACE(                                                                       \
            word,                                                                                 \
            SOC_SAND_TBL_WRITE_SIZE_BITS_WORD_LSB,                                                    \
            SOC_SAND_BITS_MASK(SOC_SAND_TBL_WRITE_SIZE_BITS_WORD_MSB, SOC_SAND_TBL_WRITE_SIZE_BITS_WORD_LSB)  \
          )

#define SOC_SAND_SET_TBL_WRITE_RVRS_BITS_IN_WORD(word)                                                \
          SOC_SAND_SET_FLD_IN_PLACE(                                                                       \
            word,                                                                                 \
            SOC_SAND_TBL_WRITE_RVRS_BITS_WORD_LSB,                                                    \
            SOC_SAND_BITS_MASK(SOC_SAND_TBL_WRITE_RVRS_BITS_WORD_MSB, SOC_SAND_TBL_WRITE_RVRS_BITS_WORD_LSB)  \
          )

void 
  soc_sand_indirect_set_nof_repetitions_unsafe(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32           nof_repetitions  
  );


typedef struct
{
  uint32   offset;
  uint32    size; 
  
} SOC_SAND_INDIRECT_MEMORY_MAP;



typedef enum
{
  
  SOC_SAND_INDIRECT_DONT_TOUCH_MODULE_BITS = 0,

  
  SOC_SAND_INDIRECT_ERASE_MODULE_BITS = 1
} SOC_SAND_INDIRECT_MODULE_BITS;


typedef struct
{
  uint32    module_index;
  
  uint32   read_result_offset;
  uint32    word_size; 

  
  uint32 access_trigger;
  
  uint32 access_address;
  
  uint32 write_buffer_offset;

  
  SOC_SAND_INDIRECT_MODULE_BITS  module_bits;

} SOC_SAND_INDIRECT_MODULE_INFO;


typedef struct
{
  
  uint32   tables_prefix;
  uint32   tables_prefix_nof_bits;
  
  uint32    word_size; 
  uint32   read_result_offset;
   
  uint32   write_buffer_offset;

}SOC_SAND_INDIRECT_TABLES_INFO;

typedef struct
{
  
  SOC_SAND_INDIRECT_MODULE_INFO const* info_arr;

  
  SOC_SAND_INDIRECT_TABLES_INFO const* tables_info;
  
  SOC_SAND_INDIRECT_MEMORY_MAP const* memory_map_arr;
  
  uint32  info_arr_max_index;

}SOC_SAND_INDIRECT_MODULE;

extern SOC_SAND_INDIRECT_MODULE
  Soc_indirect_module_arr[SOC_SAND_MAX_DEVICE] ;
extern uint32
  Soc_sand_nof_repetitions[SOC_SAND_MAX_DEVICE];



typedef
  SOC_SAND_RET
    (*SOC_SAND_INDIRECT_WRITE_ACCESS_PTR)(
      SOC_SAND_IN     int   unit,
      SOC_SAND_IN     uint32* data_ptr,
      SOC_SAND_IN     uint32  offset,
      SOC_SAND_IN     uint32  size,
      SOC_SAND_IN     uint32   module_bits
    );


typedef
  SOC_SAND_RET
    (*SOC_SAND_INDIRECT_READ_ACCESS_PTR)(
      SOC_SAND_IN     int   unit,
      SOC_SAND_INOUT  uint32* result_ptr,
      SOC_SAND_IN     uint32  offset,
      SOC_SAND_IN     uint32  size,
      SOC_SAND_IN     uint32   module_bits
    );

typedef struct
{
  
  SOC_SAND_INDIRECT_WRITE_ACCESS_PTR indirect_write;

  
  SOC_SAND_INDIRECT_READ_ACCESS_PTR  indirect_read;
} SOC_SAND_INDIRECT_ACCESS;


SOC_SAND_RET
  soc_sand_indirect_set_access_hook(
    SOC_SAND_IN SOC_SAND_INDIRECT_ACCESS* indirect_access
  );

SOC_SAND_RET
  soc_sand_indirect_get_access_hook(
    SOC_SAND_OUT SOC_SAND_INDIRECT_ACCESS* indirect_access
  );



typedef
  SOC_SAND_RET
    (*SOC_SAND_TBL_WRITE_PTR)(
      SOC_SAND_IN     int   unit,
      SOC_SAND_IN     uint32    *data_ptr,
      SOC_SAND_IN     uint32    offset,
      SOC_SAND_IN     uint32    size,
      SOC_SAND_IN     uint32   module_id,
      SOC_SAND_IN     uint32    indirect_options
    );



typedef
  SOC_SAND_RET
    (*SOC_SAND_TBL_READ_PTR)(
      SOC_SAND_IN     int   unit,
      SOC_SAND_INOUT  uint32    *result_ptr,
      SOC_SAND_IN     uint32    offset,
      SOC_SAND_IN     uint32    size,
      SOC_SAND_IN     uint32   module_id,
      SOC_SAND_IN     uint32    indirect_options
    );

typedef struct
{
  
  SOC_SAND_TBL_WRITE_PTR write;

  
  SOC_SAND_TBL_READ_PTR  read;
} SOC_SAND_TBL_ACCESS;


SOC_SAND_RET
  soc_sand_tbl_hook_set(
    SOC_SAND_IN SOC_SAND_TBL_ACCESS* tbl
  );

SOC_SAND_RET
  soc_sand_tbl_hook_get(
    SOC_SAND_OUT SOC_SAND_TBL_ACCESS* tbl
  );


SOC_SAND_RET
  soc_sand_indirect_set_info(
    SOC_SAND_IN int          unit,
    SOC_SAND_IN SOC_SAND_INDIRECT_MODULE* indirect_module
  );


SOC_SAND_RET
  soc_sand_indirect_clear_info(
    SOC_SAND_IN int  unit
  );


SOC_SAND_RET
  soc_sand_indirect_clear_info_all(
    void
  );


SOC_SAND_RET
  soc_sand_indirect_check_request_legal(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32 offset,
    SOC_SAND_IN uint32 size
  );

SOC_SAND_RET
  soc_sand_indirect_verify_trigger_0(
    int  unit,
    uint32 offset,
    uint32 timeout 
  );

SOC_SAND_RET
  soc_sand_indirect_write_address(
    int  unit,
    uint32 offset,
    uint32  read_not_write, 
    uint32  module_bits
  );

SOC_SAND_RET
  soc_sand_indirect_write_value(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    uint32   offset,
    SOC_SAND_IN    uint32   *result_ptr,
    SOC_SAND_IN    uint32   size 
  );

SOC_SAND_RET
  soc_sand_indirect_assert_trigger_1(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    uint32   offset
  );

SOC_SAND_RET
  soc_sand_indirect_read_result(
    int  unit,
    uint32 *result_ptr,
    uint32 offset
  );

SOC_SAND_RET
  soc_sand_indirect_read_from_chip(
    SOC_SAND_IN     int   unit,
    SOC_SAND_INOUT  uint32* result_ptr,
    SOC_SAND_IN     uint32  offset,
    SOC_SAND_IN     uint32  size,
    SOC_SAND_IN     uint32   module_bits
  );



uint32
  soc_sand_tbl_read(
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT uint32      *result_ptr,
    SOC_SAND_IN  uint32      offset,
    SOC_SAND_IN  uint32     size,
    SOC_SAND_IN  uint32     module_id,
    SOC_SAND_IN  uint32     entry_nof_bytes
  );



uint32
  soc_sand_tbl_read_unsafe(
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT uint32      *result_ptr,
    SOC_SAND_IN  uint32      offset,
    SOC_SAND_IN  uint32     size,
    SOC_SAND_IN  uint32     module_id,
    SOC_SAND_IN  uint32     entry_nof_bytes
  );



uint32
  soc_sand_tbl_write(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32      *data_ptr,
    SOC_SAND_IN  uint32      offset,
    SOC_SAND_IN  uint32     size,
    SOC_SAND_IN  uint32     module_id,
    SOC_SAND_IN  uint32     entry_nof_bytes
  );



uint32
  soc_sand_tbl_write_unsafe(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32      *data_ptr,
    SOC_SAND_IN  uint32      offset,
    SOC_SAND_IN  uint32     size,
    SOC_SAND_IN  uint32     module_id,
    SOC_SAND_IN  uint32      indirect_options
  );



SOC_SAND_RET
  soc_sand_indirect_write_value_ind_info(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32             reverse_order,
    SOC_SAND_IN  uint32             word_size,
    SOC_SAND_IN  uint32             *result_ptr,
    SOC_SAND_IN  SOC_SAND_INDIRECT_MODULE_INFO *ind_info,
             uint32             *device_base_address
  );



uint32
  soc_sand_tbl_read_from_chip(
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT uint32      *result_ptr,
    SOC_SAND_IN  uint32      offset,
    SOC_SAND_IN  uint32     size,
    SOC_SAND_IN  uint32     module_id,
    SOC_SAND_IN  uint32     entry_nof_bytes
  );



uint32
  soc_sand_tbl_write_to_chip(
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT uint32      *data_ptr,
    SOC_SAND_IN  uint32      offset,
    SOC_SAND_IN  uint32     size,
    SOC_SAND_IN  uint32     module_id,
    SOC_SAND_IN  uint32     entry_nof_bytes
  );


SOC_SAND_RET
  soc_sand_indirect_read_low(
    SOC_SAND_IN     int   unit,
    SOC_SAND_INOUT  uint32* result_ptr,
    SOC_SAND_IN     uint32  offset,
    SOC_SAND_IN     uint32  size,
    SOC_SAND_IN     uint32   module_bits
  );



SOC_SAND_RET
  soc_sand_tbl_read_low(
    SOC_SAND_IN     int   unit,
    SOC_SAND_INOUT  uint32    *result_ptr,
    SOC_SAND_IN     uint32    offset,
    SOC_SAND_IN     uint32    size,
    SOC_SAND_IN     uint32   module_id,
    SOC_SAND_IN     uint32    indirect_options
  );



#define SOC_SAND_IND_GET_MODULE_BIT_FROM_OFFSET 0xFF

SOC_SAND_RET
  soc_sand_indirect_write_to_chip(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   uint32* data_ptr,
    SOC_SAND_IN   uint32  offset,
    SOC_SAND_IN   uint32  size,
    SOC_SAND_IN   uint32   module_bits
  ) ;


SOC_SAND_RET
  soc_sand_indirect_write_low(
    SOC_SAND_IN     int   unit,
    SOC_SAND_IN     uint32* data_ptr,
    SOC_SAND_IN     uint32  offset,
    SOC_SAND_IN     uint32  size,
    SOC_SAND_IN     uint32   module_bits
  );

SOC_SAND_RET
  soc_sand_tbl_write_low(
    SOC_SAND_IN     int   unit,
    SOC_SAND_IN     uint32    *data_ptr,
    SOC_SAND_IN     uint32    offset,
    SOC_SAND_IN     uint32    size,
    SOC_SAND_IN     uint32   module_id,
    SOC_SAND_IN     uint32    indirect_options
  );


SOC_SAND_RET
  soc_sand_indirect_read_modify_write(
    SOC_SAND_IN     int  unit,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 shift,
    SOC_SAND_IN     uint32 mask,
    SOC_SAND_IN     uint32 data_to_write
  ) ;





SOC_SAND_RET
  soc_sand_indirect_get_access_trigger(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32 offset,
    SOC_SAND_OUT uint32 *access_trigger
  );

SOC_SAND_RET
  soc_sand_indirect_get_word_size(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32 offset,
    SOC_SAND_OUT uint32  *entry_nof_bytes
  );

SOC_SAND_RET
  soc_sand_indirect_get_inner_struct(
    int unit,
    uint32  offset,
    uint32 module_bits,
    SOC_SAND_INDIRECT_MODULE_INFO *ind_info
  );

SOC_SAND_RET
  soc_sand_indirect_write_address_ind_info(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32             offset,
    SOC_SAND_IN  uint32              read_not_write,
    SOC_SAND_IN  SOC_SAND_INDIRECT_MODULE_INFO *ind_info,
    SOC_SAND_IN  uint32              module_bits,
             uint32             *device_base_address
  );


SOC_SAND_RET
  soc_sand_indirect_read_result_ind_info(
    SOC_SAND_OUT uint32             *result_ptr,
    SOC_SAND_IN  uint32             word_size,
    SOC_SAND_IN  uint32             reverse_order,
    SOC_SAND_IN  SOC_SAND_INDIRECT_MODULE_INFO *ind_info,
             uint32             *device_base_address
  );

#if SOC_SAND_DEBUG
void
  soc_sand_indirect_write_to_chip_print_when_write(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   uint32* data_ptr,
    SOC_SAND_IN   uint32  offset,
    SOC_SAND_IN   uint32  size,
    SOC_SAND_IN   uint32  word_size
  );
#endif

#ifdef  __cplusplus
}
#endif

#endif

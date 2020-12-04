/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/
#ifndef  SOC_SAND_DRIVER_FRAMEWORK_H
#define SOC_SAND_DRIVER_FRAMEWORK_H
#ifdef  __cplusplus
extern "C" {
#endif

#include <sal/types.h>
#include <sal/limits.h>
#include <sal/core/libc.h>
#include <sal/compiler.h>
#include <soc/defs.h>





#define SOC_SAND_DBG_LVL1 1
  
#define SOC_SAND_DBG_LVL2 2
  
#define SOC_SAND_DBG_LVL3 3

  
#define SOC_SAND_DEBUG SOC_SAND_DBG_LVL2


#define SOC_SAND_ALLOW_DRIVER_TO_PRINT_ERRORS (SOC_SAND_DEBUG >= SOC_SAND_DBG_LVL2)




#ifndef NULL
  #define NULL (void *)0
#endif

#ifndef TRUE
  #define TRUE 1
#endif

#ifndef FALSE
  #define FALSE 0
#endif



#define SOC_SAND_BOOL2NUM(b_val) ((b_val) == FALSE?0x0:0x1)
#define SOC_SAND_NUM2BOOL(n_val) ((uint8)((n_val) == 0x0?FALSE:TRUE))


#define SOC_SAND_BOOL2NUM_INVERSE(b_val) ((b_val) == TRUE?0x0:0x1)
#define SOC_SAND_NUM2BOOL_INVERSE(n_val) (uint8)((n_val) == 0x0?TRUE:FALSE)


#define SOC_SAND_RNG_COUNT(n_first, n_last) (((n_last) >= (n_first))?((n_last) - (n_first) + 1):((n_first) - (n_last) + 1))
#define SOC_SAND_RNG_FIRST(n_last, n_cnt)   ((((n_last) + 1) >= (n_cnt))?(((n_last) + 1) - (n_cnt)):0)
#define SOC_SAND_RNG_LAST(n_first, n_cnt)   ((n_first) + (n_cnt) - 1)
#define SOC_SAND_RNG_NEXT(n_curr)           ((n_curr) + 1)
#define SOC_SAND_RNG_PREV(n_curr)           (((n_curr) > 0)?((n_curr) - 1):0)


typedef
  unsigned short SOC_SAND_RET ;


#define SOC_SAND_ERR 1

#define SOC_SAND_OK  0

#define SOC_SAND_IN   const
#define SOC_SAND_OUT
#define SOC_SAND_INOUT

#define SOC_SAND_NOF_BITS_IN_BYTE 8

#define SOC_SAND_BIT_BYTE_SHIFT 3

#define SOC_SAND_TRANSLATE_BITS_TO_BYTES(_var)   \
           ((_var) >> SOC_SAND_BIT_BYTE_SHIFT)


#define SOC_SAND_INTERN_VAL_INVALID_32  0xffffffff


#define SOC_SAND_REG_VAL_INVALID   0xDEADBEAF


#define SOC_SAND_IS_INTERN_VAL_INVALID(intern_val)  \
  SOC_SAND_NUM2BOOL(intern_val == SOC_SAND_INTERN_VAL_INVALID_32)


#define SOC_SAND_BYTE_SWAP(x) ((((x) << 24)) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | (((x) >> 24)))


typedef enum
{
  SOC_SAND_DIRECT,
  SOC_SAND_INDIRECT
} SOC_SAND_ACCESS_METHOD ;

#ifndef SOC_SAND_BIT
  #define SOC_SAND_BIT(x) (1UL<<(x))
#endif

#ifndef SOC_SAND_RBIT
  #define SOC_SAND_RBIT(x) (~(1UL<<(x)))
#endif

#ifndef SOC_SAND_MAX_DEVICE
  #define SOC_SAND_MAX_DEVICE SOC_MAX_NUM_DEVICES
#endif


#define SOC_SAND_OFFSETOF(x,y)  ((uint32)((char*)(&(((x *)0)->y)) - (char*)0))




typedef enum
{
  SOC_SAND_PRINT_FLAVORS_SHORT =0,
  SOC_SAND_PRINT_FLAVORS_NO_ZEROS,
  SOC_SAND_PRINT_FLAVORS_ERRS_ONLY
}SOC_SAND_PRINT_FLAVORS;


typedef enum
{
  
  SOC_SAND_NOP = 0,

  
  SOC_SAND_OP_AND,

  
  SOC_SAND_OP_OR,

  
  SOC_SAND_NOF_SAND_OP

} SOC_SAND_OP;

typedef enum
{
  
  SOC_SAND_SUCCESS = 0,
  
  SOC_SAND_FAILURE_OUT_OF_RESOURCES = 1,
  
  SOC_SAND_FAILURE_OUT_OF_RESOURCES_2 = 2,
  
  SOC_SAND_FAILURE_OUT_OF_RESOURCES_3 = 3,
  
  SOC_SAND_FAILURE_REMOVE_ENTRY_FIRST = 10,
  
  SOC_SAND_FAILURE_INTERNAL_ERR = 12,
  
  SOC_SAND_FAILURE_UNKNOWN_ERR = 100
  
}SOC_SAND_SUCCESS_FAILURE;


#define SOC_SAND_NOF_SUCCESS_FAILURES (6)


#define SOC_SAND_SUCCESS2BOOL(success_status_) \
  (((success_status_) == SOC_SAND_SUCCESS)?TRUE:FALSE)

#define SOC_SAND_BOOL2SUCCESS(is_success_) \
  (((is_success_) == TRUE)?SOC_SAND_SUCCESS:SOC_SAND_FAILURE_UNKNOWN_ERR)

typedef enum
{
  
  SOC_SAND_OPERATION_TYPE_NONE = 0,
  
  SOC_SAND_OPERATION_TYPE_ADD = 1,
  
  SOC_SAND_OPERATION_TYPE_REMOVE = 2,
  
  SOC_SAND_OPERATION_TYPE_UPDATE = 3,
  
  SOC_SAND_NOF_OPERATION_TYPES = 4
}SOC_SAND_OPERATION_TYPE;



#define SOC_SAND_REG_MAX_BIT    31
#define SOC_SAND_REG_SIZE_BITS  32
#define SOC_SAND_REG_SIZE_BYTES 4


#define SOC_SAND_SET_FLD_IN_PLACE(val,shift,mask) (uint32)(((uint32)(val) << ((uint32)shift)) & (uint32)(mask))

#define SOC_SAND_GET_FLD_FROM_PLACE(val,shift,mask) (((uint32)(val) & (mask)) >> (shift))

#define SOC_SAND_BITS_MASK(ms_bit,ls_bit) \
    ( ((uint32)SOC_SAND_BIT(ms_bit)) - ((uint32)SOC_SAND_BIT(ls_bit)) + ((uint32)SOC_SAND_BIT(ms_bit)) )


#define SOC_SAND_ZERO_BITS_MASK(ms_bit,ls_bit)  (~(SOC_SAND_BITS_MASK(ms_bit, ls_bit)))

#define SOC_SAND_RBITS_MASK(ms_bit,ls_bit)      (SOC_SAND_ZERO_BITS_MASK(ms_bit, ls_bit))

#define SOC_SAND_SET_BITS_RANGE(val,ms_bit,ls_bit) \
 (((uint32)(val) << (ls_bit)) & (SOC_SAND_BITS_MASK(ms_bit,ls_bit)))

#define SOC_SAND_GET_BITS_RANGE(val,ms_bit,ls_bit) \
 (((uint32)(val) & (SOC_SAND_BITS_MASK(ms_bit,ls_bit))) >> (ls_bit))

#define SOC_SAND_GET_BIT(val,bit) (SOC_SAND_GET_BITS_RANGE(val,bit,bit)?(uint32)0x1:(uint32)0x0)


#define SOC_SAND_GET_BYTE_3(val) SOC_SAND_GET_FLD_FROM_PLACE(val,0,  0x000000FF)
#define SOC_SAND_GET_BYTE_2(val) SOC_SAND_GET_FLD_FROM_PLACE(val,8,  0x0000FF00)
#define SOC_SAND_GET_BYTE_1(val) SOC_SAND_GET_FLD_FROM_PLACE(val,16, 0x00FF0000)
#define SOC_SAND_GET_BYTE_0(val) SOC_SAND_GET_FLD_FROM_PLACE(val,24, 0xFF000000)

#define SOC_SAND_SET_BYTE_2(val) SOC_SAND_SET_FLD_IN_PLACE(val,8,  0x0000FF00)
#define SOC_SAND_SET_BYTE_1(val) SOC_SAND_SET_FLD_IN_PLACE(val,16, 0x00FF0000)

#define SOC_SAND_SET_BIT(reg,val,bit_i)         \
  reg = (val ? (reg | SOC_SAND_BIT(bit_i)) : (reg & SOC_SAND_RBIT(bit_i)));

#define SOC_SAND_APPLY_MASK(_reg, _enable, _mask)   \
    _reg = (_enable ? (_reg | _mask) : (_reg & ~_mask));


#define SOC_SAND_U8_MAX 0xff

#define SOC_SAND_U16_MAX 0xffff
#define SOC_SAND_I16_MAX 0x7fff

#define SOC_SAND_U32_MAX 0xffffffff

#define SOC_SAND_UINT_MAX SAL_UINT32_MAX


#define SOC_SAND_IGNORE_UNUSED_VAR(p)     (void)(p)


#define SOC_SAND_LOOP_FOREVER for(;;)


#define SOC_SAND_MIN_TCM_ACTIVATION_PERIOD  1

#define SOC_SAND_TRIGGER_TIMEOUT            1000



#define SOC_SAND_CALLBACK_BUF_SIZE          500

typedef int (*SOC_SAND_ERROR_HANDLER_PTR)(
  uint32,char *,char **,uint32,
  uint32,uint32,uint32,uint32,uint32) ;

typedef struct
{
  uint32 err_id ;
  char          error_txt[SOC_SAND_CALLBACK_BUF_SIZE] ;
  uint32 param_01 ;
  uint32 param_02 ;
  uint32 param_03 ;
  uint32 param_04 ;
  uint32 param_05 ;
  uint32 param_06 ;
} SOC_SAND_ERRORS_QUEUE_MESSAGE ;

typedef struct
{
  
  uint32 start;
  
  uint32 end;

}SOC_SAND_U32_RANGE;

#define SOC_SAND_TBL_ITER_SCAN_ALL        (0xFFFFFFFF)
#define SOC_SAND_TBL_ITER_SET_BEGIN(iter) ((*(iter)) = 0)
#define SOC_SAND_TBL_ITER_SET_END(iter) ((*(iter)) = 0xFFFFFFFF)
#define SOC_SAND_TBL_ITER_IS_BEGIN(iter) (*(iter) == 0)
#define SOC_SAND_TBL_ITER_IS_END(iter) (*(iter) == 0xFFFFFFFF)


typedef struct
{
  
  uint32 iter;
  
  uint32 entries_to_scan;
  
  uint32 entries_to_act;

}SOC_SAND_TABLE_BLOCK_RANGE;


#define ERRORS_MSG_QUEUE_NUM_MSGS       30


#define SOC_SAND_ERRORS_MSG_QUEUE_SIZE_MSG      sizeof(SOC_SAND_ERRORS_QUEUE_MESSAGE)



  extern uint32
      Soc_sand_errors_msg_queue_flagged;
  extern char  *Soc_sand_supplied_error_buffer ;
  extern uint32
    Soc_sand_supplied_error_handler_is_on;



SOC_SAND_RET
  soc_sand_error_handler(
    uint32 err_id,
    const char          *error_txt,
    uint32 param_01,
    uint32 param_02,
    uint32 param_03,
    uint32 param_04,
    uint32 param_05,
    uint32 param_06
  ) ;

SOC_SAND_RET
  soc_sand_invoke_user_error_handler(
    uint32 err_id,
    const char    *error_txt,
    uint32 param_01,
    uint32 param_02,
    uint32 param_03,
    uint32 param_04,
    uint32 param_05,
    uint32 param_06
  ) ;


SOC_SAND_RET
  soc_sand_set_user_error_handler(
    SOC_SAND_ERROR_HANDLER_PTR   user_error_handler,
    char                    *user_error_buffer
  ) ;

SOC_SAND_RET
  soc_sand_set_user_error_state(
    uint32 onFlag
  );



#define SOC_SAND_UINT32_ALIGN_MASK (SOC_SAND_BIT(0) | SOC_SAND_BIT(1))










int
  soc_sand_is_long_aligned(
    uint32 word_to_check
  ) ;

void
  soc_sand_check_driver_and_device(
    int  unit,
    uint32 *error_word
  ) ;

uint32
  soc_sand_get_index_of_max_member_in_array(
    SOC_SAND_IN     uint32                     array[],
    SOC_SAND_IN     uint32                    len
  );

SOC_SAND_RET
  soc_sand_set_field(
    SOC_SAND_INOUT  uint32    *reg_val,
    SOC_SAND_IN  uint32       ms_bit,
    SOC_SAND_IN  uint32       ls_bit,
    SOC_SAND_IN  uint32       field_val
  );

SOC_SAND_RET
  soc_sand_U8_to_U32(
    SOC_SAND_IN uint8     *u8_val,
    SOC_SAND_IN uint32    nof_bytes,
    SOC_SAND_OUT uint32   *u32_val
  );

SOC_SAND_RET
  soc_sand_U32_to_U8(
    SOC_SAND_IN uint32  *u32_val,
    SOC_SAND_IN uint32  nof_bytes,
    SOC_SAND_OUT uint8  *u8_val
  );




#if (defined(CPU) && ((CPU)==(PPC860)) && defined(__GNUC__))
  #define SOC_SAND_SYNC_IOS   __asm__ __volatile__ ("   eieio")
#elif defined(SOC_PPC_CPU) && defined(__GNUC__)
  #define SOC_SAND_SYNC_IOS   __asm__ __volatile__ ("   eieio")
#else
  #define SOC_SAND_SYNC_IOS
#endif

#ifndef SOC_SAND_SYNC_IOS
  #error "SOC_SAND_SYNC_IOS need to be defined." \
         "  Specifically, in Dune Reference System we use PPC860" \
         "  Consult with the CPU Data Sheet, or your BSP expert, for a similar ASM code."
#endif

void
  soc_sand_SAND_U32_RANGE_clear(
    SOC_SAND_OUT SOC_SAND_U32_RANGE *info
  );

void
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(
    SOC_SAND_OUT SOC_SAND_TABLE_BLOCK_RANGE *info
  );

#if SOC_SAND_DEBUG

int
  soc_sand_general_display_err(
    uint32 err_id,
    const char    *error_txt
  );


const char*
  soc_sand_SAND_OP_to_str(
    SOC_SAND_IN SOC_SAND_OP      soc_sand_op,
    SOC_SAND_IN uint32 short_format
  ) ;


void
  soc_sand_print_hex_buff(
    SOC_SAND_IN char*        buff,
    SOC_SAND_IN uint32 buff_byte_size,
    SOC_SAND_IN uint32 nof_bytes_per_line
  );


void
  soc_sand_print_bandwidth(
    SOC_SAND_IN uint32 bw_kbps,
    SOC_SAND_IN uint32  short_format
  );




const char*
  soc_sand_SAND_SUCCESS_FAILURE_to_string(
    SOC_SAND_IN  SOC_SAND_SUCCESS_FAILURE enum_val
  );

void
  soc_sand_SAND_U32_RANGE_print(
    SOC_SAND_IN  SOC_SAND_U32_RANGE *info
  );

void
  soc_sand_SAND_TABLE_BLOCK_RANGE_print(
    SOC_SAND_IN  SOC_SAND_TABLE_BLOCK_RANGE *info
  );




#endif


#ifdef  __cplusplus
}
#endif

#endif

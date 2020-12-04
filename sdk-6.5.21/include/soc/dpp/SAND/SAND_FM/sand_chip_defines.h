/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef SOC_SAND_CHIP_DEFINES_H
#define SOC_SAND_CHIP_DEFINES_H
#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_tcm.h>

  









typedef enum
{
  SOC_SAND_FAP_TYPE_NONE = 0,
  SOC_SAND_FAP10M_FAP_TYPE,
  SOC_SAND_FAP20V_FAP_TYPE,
  SOC_SAND_MAX_FAP_TYPE
} SOC_SAND_FMF_FAP_TYPE ;

typedef enum
{
  SOC_SAND_FE_TYPE_NONE = 0,
  SOC_SAND_FE200_FE_TYPE,
  SOC_SAND_MAX_FE_TYPE
} SOC_SAND_FMF_FE_TYPE ;

typedef enum
{
  SOC_SAND_PP_TYPE_NONE = 0,
  SOC_SAND_PP_MARVELL_PRESTERA,
  SOC_SAND_MAX_PP_TYPE
} SOC_SAND_FMF_PP_TYPE ;

typedef enum
{
  SOC_SAND_DEVICE_TYPE_NONE = 0,
  SOC_SAND_FE2_DEVICE,
  SOC_SAND_FE13_DEVICE,
  SOC_SAND_FAP_DEVICE,
  SOC_SAND_MAX_DEVICE_TYPE
} SOC_SAND_FMF_DEVICE_TYPE ;



#define SOC_SAND_FMF_NUM_DEVICE_TYPES       (SOC_SAND_MAX_DEVICE_TYPE - 1)

typedef union
{
  SOC_SAND_FMF_FAP_TYPE fap_type ;
  SOC_SAND_FMF_FE_TYPE  fe_type ;
    
  uint32      any_type ;
} SOC_SAND_FMF_DEVICE_SUB_TYPE ;

#define SOC_SAND_FMF_CHIP_ID_FROM_DEV_ID(unit) \
    (unit / SOC_SAND_FMF_NUM_DEVICE_TYPES)
#define SOC_SAND_FMF_DEV_TYPE_FROM_DEV_ID(unit) \
    ((unit % SOC_SAND_FMF_NUM_DEVICE_TYPES) + 1)


typedef enum
{
  SOC_SAND_DEV_FE200 = 0,
  SOC_SAND_DEV_FAP10M,
  SOC_SAND_DEV_FAP20V,
  SOC_SAND_DEV_TIMNA,
  SOC_SAND_DEV_HRP,
  SOC_SAND_DEV_FAP20M,
  SOC_SAND_DEV_PETRA,
  SOC_SAND_DEV_FAP21V,
  SOC_SAND_DEV_FE600,
  SOC_SAND_DEV_T20E,
  SOC_SAND_DEV_PB,
  SOC_SAND_DEV_MOA,
  SOC_SAND_DEV_PCP,
  SOC_SAND_DEV_ARAD,
  SOC_SAND_DEV_JERICHO,

  
  SOC_SAND_DEVICE_NOF_TYPES
} SOC_SAND_DEVICE_TYPE ;

  
#define SOC_SAND_FE200         SOC_SAND_DEV_FE200
#define SOC_SAND_FE600         SOC_SAND_DEV_FE600
#define SOC_SAND_FAP10M        SOC_SAND_DEV_FAP10M
#define SOC_SAND_FAP20V        SOC_SAND_DEV_FAP20V
#define SOC_SAND_FAP20M        SOC_SAND_DEV_FAP20M
#define SOC_SAND_FAP21V        SOC_SAND_DEV_FAP21V


typedef enum
{
  SOC_SAND_CR_256  = 256,
  SOC_SAND_CR_512  = 512,
  SOC_SAND_CR_1024 = 1024,
  SOC_SAND_CR_2048 = 2048,
  SOC_SAND_CR_4096 = 4096,
  SOC_SAND_CR_8192 = 8192
}SOC_SAND_CREDIT_SIZE ;



typedef enum
{
  SOC_SAND_DONT_CARE_ENTITY = 0,
  
  SOC_SAND_FE1_ENTITY = 1,
  
  SOC_SAND_FE2_ENTITY = 2,
  
  SOC_SAND_FE3_ENTITY = 3,
  
  SOC_SAND_FAP_ENTITY = 4,
  
  SOC_SAND_FOP_ENTITY = 5,
  
  SOC_SAND_FIP_ENTITY = 6,
  
  SOC_SAND_FE13_ENTITY = 7
} SOC_SAND_DEVICE_ENTITY ;



#define MAX_ELEMENT_OF_GROUP_ITEMS 10

#define MAX_REGS_IN_ONE_GROUP      256





#define MAX_INTERRUPT_CAUSE               256

#define SIZE_OF_BITSTRAEM_IN_UINT32S   (MAX_INTERRUPT_CAUSE>>5)

typedef unsigned short (*SOC_SAND_UNMASK_FUNC_PTR) (int, uint32) ;
typedef uint32   (*SOC_SAND_IS_BIT_AUTO_CLEAR_FUNC_PTR)(uint32) ;
typedef unsigned short (*SOC_SAND_IS_DEVICE_INTERRUPTS_MASKED)(int) ;
typedef unsigned short (*SOC_SAND_GET_DEVICE_INTERRUPTS_MASK)(int, uint32 *) ;
typedef unsigned short (*SOC_SAND_MASK_SPECIFIC_INTERRUPT_CAUSE)(int, uint32) ;

typedef unsigned short (*SOC_SAND_RESET_DEVICE_FUNC_PTR) (int, uint32) ;

typedef unsigned short (*SOC_SAND_IS_OFFSET_READ_OR_WRITE_PROTECT_FUNC_PTR) (uint32, uint32, uint32) ;

typedef struct
{
  
  uint32   valid_word ;
  
  uint32   *base_addr ;
  
  uint32   mem_size ;
  
  sal_mutex_t  mutex_id ;
  
  sal_thread_t  mutex_owner ;
  
  int32   mutex_counter ;
  
  struct soc_sand_tcm_callback_str    interrupt_callback_array[MAX_INTERRUPT_CAUSE] ;
  
  uint32   interrupt_bitstream[SIZE_OF_BITSTRAEM_IN_UINT32S] ;
  
  SOC_SAND_UNMASK_FUNC_PTR         unmask_ptr ;
  
  SOC_SAND_IS_BIT_AUTO_CLEAR_FUNC_PTR  is_bit_auto_clear_ptr ;
  
  SOC_SAND_IS_DEVICE_INTERRUPTS_MASKED is_device_interrupts_masked_ptr ;
  
  SOC_SAND_GET_DEVICE_INTERRUPTS_MASK  get_device_interrupts_mask_ptr ;
  
  SOC_SAND_MASK_SPECIFIC_INTERRUPT_CAUSE mask_specific_interrupt_cause_ptr ;
  
  uint32  device_is_between_isr_to_tcm ;
  int           device_interrupt_mask_counter ;
  
  SOC_SAND_RESET_DEVICE_FUNC_PTR     reset_device_ptr ;
  
  SOC_SAND_IS_OFFSET_READ_OR_WRITE_PROTECT_FUNC_PTR  is_read_write_protect_ptr ;
  
  SOC_SAND_DEVICE_TYPE logic_chip_type;
  uint32    chip_type ;
  uint32    dbg_ver ;
  uint32    chip_ver ;

  
  uint32   device_at_init ;
  
  uint32   magic ;
} SOC_SAND_DEVICE_DESC ;

#define SOC_SAND_DEVICE_DESC_VALID_WORD 0xFAEBDC65

typedef uint32 SOC_SAND_CHIP_HANDLE ;


typedef enum{
  SOC_SAND_ACTUAL_FAP_VALUE_1 = 0,
  SOC_SAND_ACTUAL_FAP_VALUE   = 5,
  SOC_SAND_ACTUAL_FIP_VALUE   = 1,
  SOC_SAND_ACTUAL_FOP_VALUE   = 4,
  SOC_SAND_ACTUAL_FE1_VALUE   = 6,
  SOC_SAND_ACTUAL_FE2_VALUE   = 3,
  SOC_SAND_ACTUAL_FE2_VALUE_1 = 7,
  SOC_SAND_ACTUAL_FE3_VALUE   = 2
} SOC_SAND_ENTITY_LEVEL_TYPE;

#define SOC_SAND_ACTUAL_BAD_VALUE               0xFF
#define SOC_SAND_ACTUAL_OUT_OF_RANGE            0xFFFFFFFF
#define SOC_SAND_REAL_ENTITY_VALUE_OUT_OF_RANGE 0xFFFFFFFF



char *
  soc_sand_entity_enum_to_str(
    SOC_SAND_IN SOC_SAND_DEVICE_ENTITY entity_type
  );


SOC_SAND_DEVICE_ENTITY
  soc_sand_entity_from_level(
    SOC_SAND_IN SOC_SAND_ENTITY_LEVEL_TYPE level_val
  );

#ifdef  __cplusplus
}
#endif

#endif

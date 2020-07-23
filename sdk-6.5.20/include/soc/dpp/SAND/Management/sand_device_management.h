/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/


#ifndef __SOC_SAND_DEVICE_MANAGEMENT_H_INCLUDED__

#define __SOC_SAND_DEVICE_MANAGEMENT_H_INCLUDED__
#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>



typedef struct
{
  
  uint32  ver_reg_offset;

  

  SOC_SAND_DEVICE_TYPE logic_chip_type;
  uint32    chip_type;
  uint32    chip_type_shift;
  uint32    chip_type_mask;

  
  uint32  dbg_ver_shift;
  uint32  dbg_ver_mask;

  
  uint32  chip_ver_shift;
  uint32  chip_ver_mask;

  
  uint8 cmic_skip_verif;

} SOC_SAND_DEV_VER_INFO;

extern uint8 Soc_register_with_id;

void
  soc_sand_clear_SAND_DEV_VER_INFO(
    SOC_SAND_OUT SOC_SAND_DEV_VER_INFO* ver_info
  );



SOC_SAND_RET
  soc_sand_device_register_with_id(SOC_SAND_IN uint8 enable);

SOC_SAND_RET
  soc_sand_device_register(
    uint32                 *base_address,
    uint32                 mem_size, 
    SOC_SAND_UNMASK_FUNC_PTR               unmask_func_ptr,
    SOC_SAND_IS_BIT_AUTO_CLEAR_FUNC_PTR    is_bit_ac_func_ptr,
    SOC_SAND_IS_DEVICE_INTERRUPTS_MASKED   is_dev_int_mask_func_ptr,
    SOC_SAND_GET_DEVICE_INTERRUPTS_MASK    get_dev_mask_func_ptr,
    SOC_SAND_MASK_SPECIFIC_INTERRUPT_CAUSE mask_specific_interrupt_cause_ptr,
    SOC_SAND_RESET_DEVICE_FUNC_PTR    reset_device_ptr,
    SOC_SAND_IS_OFFSET_READ_OR_WRITE_PROTECT_FUNC_PTR is_read_write_protect_ptr,
    SOC_SAND_DEV_VER_INFO             *ver_info,
    SOC_SAND_INDIRECT_MODULE          *indirect_module,
    uint32                 interrupt_mask_address,
    int                  *unit_ptr
  );

SOC_SAND_RET
  soc_sand_device_unregister(
    int unit
  ) ;


SOC_SAND_RET
  soc_sand_get_device_type(
    int      unit,
    SOC_SAND_DEVICE_TYPE *chip_type,
    uint32    *chip_ver,
    uint32    *dbg_ver
  ) ;
#ifdef SOC_SAND_DEBUG



const char*
  soc_sand_DEVICE_TYPE_to_str(
    SOC_SAND_IN SOC_SAND_DEVICE_TYPE dev_type
  );
#endif


#ifdef  __cplusplus
}
#endif


#endif

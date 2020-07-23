/* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/
#ifndef SOC_SAND_CHIP_DESCRIPTORS_H
#define SOC_SAND_CHIP_DESCRIPTORS_H

#ifdef  __cplusplus
extern "C" {
#endif
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>
#include <soc/dpp/SAND/Utils/sand_tcm.h>


extern  uint32
    Soc_sand_array_size;
extern SOC_SAND_DEVICE_DESC
    *Soc_sand_chip_descriptors;
extern uint32 Soc_sand_up_counter;
SOC_SAND_RET
  soc_sand_array_mutex_take(void);

SOC_SAND_RET
  soc_sand_array_mutex_give(void);

SOC_SAND_RET
 soc_sand_init_chip_descriptors(
   uint32 max_devices
 );

SOC_SAND_RET
 soc_sand_delete_chip_descriptors(
   void
 );

SOC_SAND_RET
 soc_sand_remove_chip_descriptor(
   uint32 index
 );

int
  soc_sand_add_chip_descriptor(
     uint32                  *base_address,
     uint32                  mem_size, 
     SOC_SAND_UNMASK_FUNC_PTR                unmask_func_ptr,
     SOC_SAND_IS_BIT_AUTO_CLEAR_FUNC_PTR     is_bit_ac_func_ptr,
     SOC_SAND_IS_DEVICE_INTERRUPTS_MASKED    is_dev_int_mask_func_ptr,
     SOC_SAND_GET_DEVICE_INTERRUPTS_MASK     get_dev_mask_func_ptr,
     SOC_SAND_MASK_SPECIFIC_INTERRUPT_CAUSE  mask_specific_interrupt_cause_ptr,
     SOC_SAND_RESET_DEVICE_FUNC_PTR     reset_device_ptr,
     SOC_SAND_IS_OFFSET_READ_OR_WRITE_PROTECT_FUNC_PTR is_read_write_protect_ptr,
     uint32                  chip_type,
     SOC_SAND_DEVICE_TYPE               logic_chip_type,
	 int                            handle
  );

SOC_SAND_RET
  soc_sand_register_event_callback(
    SOC_SAND_INOUT  SOC_SAND_TCM_CALLBACK func,   
    SOC_SAND_INOUT  uint32     *buffer,
    SOC_SAND_INOUT  uint32     size,   
    SOC_SAND_IN     uint32      index,  
    SOC_SAND_IN     uint32      cause,  
    SOC_SAND_INOUT  uint32     *handle
  );

SOC_SAND_RET
  soc_sand_unregister_event_callback(
    SOC_SAND_IN     uint32            handle
  );

SOC_SAND_RET
  soc_sand_combine_2_event_callback_handles(
    SOC_SAND_IN     uint32            handle_1,
    SOC_SAND_IN     uint32            handle_2,
    SOC_SAND_INOUT  uint32           *handle
  );

int
  soc_sand_take_chip_descriptor_mutex(
    uint32 index
  );

SOC_SAND_RET
  soc_sand_give_chip_descriptor_mutex(
    uint32 index
  );
SOC_SAND_RET
  soc_sand_set_chip_descriptor_ver_info(
    SOC_SAND_IN uint32    index,
    SOC_SAND_IN uint32   dbg_ver,
    SOC_SAND_IN uint32   chip_ver
  );

uint32
  *soc_sand_get_chip_descriptor_base_addr(
    uint32 index
  );

uint32
  soc_sand_get_chip_descriptor_memory_size(
    uint32 index
  );

soc_sand_tcm_callback_str_t
  *soc_sand_get_chip_descriptor_interrupt_callback_array(
    uint32 index
  );

uint32
  *soc_sand_get_chip_descriptor_interrupt_bitstream(
    uint32 index
  );

SOC_SAND_UNMASK_FUNC_PTR
  soc_sand_get_chip_descriptor_unmask_func(
    uint32 index
  );

SOC_SAND_IS_BIT_AUTO_CLEAR_FUNC_PTR
  soc_sand_get_chip_descriptor_is_bit_auto_clear_func(
    uint32 index
  );

SOC_SAND_IS_DEVICE_INTERRUPTS_MASKED
  soc_sand_get_chip_descriptor_is_device_interrupts_masked_func(
    uint32 index
  );

SOC_SAND_GET_DEVICE_INTERRUPTS_MASK
  soc_sand_chip_descriptor_get_interrupts_mask_func(
    uint32 index
  );

SOC_SAND_MASK_SPECIFIC_INTERRUPT_CAUSE
  soc_sand_chip_descriptor_get_mask_specific_interrupt_cause_func(
    uint32 index
  );

SOC_SAND_RESET_DEVICE_FUNC_PTR
  soc_sand_chip_descriptor_get_reset_device_func(
    uint32 index
  );

SOC_SAND_IS_OFFSET_READ_OR_WRITE_PROTECT_FUNC_PTR
  soc_sand_chip_descriptor_get_is_read_write_protect_func(
    uint32 index
  );

uint32
  soc_sand_device_is_between_isr_to_tcm(
    uint32 index
  );
void
  soc_sand_device_set_between_isr_to_tcm(
    uint32 index,
  uint32 level
  );
int
  soc_sand_device_get_interrupt_mask_counter(
    uint32 index
  );
void
  soc_sand_device_inc_interrupt_mask_counter(
    uint32 index
  );
void
  soc_sand_device_dec_interrupt_mask_counter(
    uint32 index
  );

uint32
  soc_sand_is_chip_descriptor_chip_ver_bigger_eq(
    uint32   index,
    uint32  chip_ver_bigger_eq
  );
uint32
  soc_sand_get_chip_descriptor_chip_ver(
    uint32 index
  );
uint32
  soc_sand_get_chip_descriptor_dbg_ver(
    uint32 index
  );

uint32
  soc_sand_get_chip_descriptor_chip_type(
    uint32 index
  );

#define SOC_SAND_DEVICE_TYPE_GET(unit) \
          soc_sand_get_chip_descriptor_logic_chip_type(unit)

uint32
  soc_sand_get_chip_descriptor_logic_chip_type(
    uint32 index
  );

void
  soc_sand_set_chip_descriptor_valid(
    uint32 index
  );

uint32
  soc_sand_is_chip_descriptor_valid(
    uint32 index
  );

SOC_SAND_RET
  soc_sand_set_chip_descriptor_device_at_init(
    uint32 index,
    uint32 val
  ) ;

SOC_SAND_RET
  soc_sand_get_chip_descriptor_device_at_init(
    uint32 index,
    uint32 *val
  ) ;

uint32
  soc_sand_get_chip_descriptor_magic(
    uint32 index
  );

sal_thread_t
  soc_sand_get_chip_descriptor_mutex_owner(
    uint32 index
  );

uint32
  soc_sand_is_any_chip_descriptor_taken(
    void
  );
uint32
  soc_sand_is_any_chip_descriptor_taken_by_task(
    sal_thread_t task_id
  );


#if SOC_SAND_DEBUG



void
  soc_sand_chip_descriptors_print(
    void
  );


#endif


#ifdef  __cplusplus
}
#endif


#endif

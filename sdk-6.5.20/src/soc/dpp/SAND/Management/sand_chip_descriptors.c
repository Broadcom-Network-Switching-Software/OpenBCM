/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/



#include <shared/bsl.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>
#include <soc/dpp/SAND/Management/sand_callback_handles.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_device_management.h>
#include <soc/dpp/SAND/Utils/sand_tcm.h>


#define FE200_REV_B_AS_REV_A  0


  SOC_SAND_DEVICE_DESC
    *Soc_sand_chip_descriptors = NULL ;

  uint32
    Soc_sand_array_size        = 0 ;

static
  sal_mutex_t
    Soc_sand_array_mutex       = 0 ;

  uint32 Soc_sand_up_counter = 0 ;

STATIC
  uint32
    soc_get_magic(
      void
    )
{
  Soc_sand_up_counter ++ ;
  return Soc_sand_up_counter ;
}

SOC_SAND_RET
  soc_sand_array_mutex_take()
{
  return soc_sand_os_mutex_take(Soc_sand_array_mutex, SOC_SAND_INFINITE_TIMEOUT);
}

SOC_SAND_RET
  soc_sand_array_mutex_give()
{
  return soc_sand_os_mutex_give(Soc_sand_array_mutex);
}



STATIC
  SOC_SAND_RET
    soc_sand_clear_chip_descriptor(
      uint32 index,
      uint32 init_flag
  )
{
  int
    iii ;
  SOC_SAND_RET
    ex ;
  uint32
      err = 0 ;
  
  Soc_sand_chip_descriptors[index].base_addr            = 0 ;
  Soc_sand_chip_descriptors[index].chip_type            = 0 ;
  Soc_sand_chip_descriptors[index].dbg_ver              = 0 ;
  Soc_sand_chip_descriptors[index].chip_ver             = 0 ;
  
  soc_sand_bitstream_clear(Soc_sand_chip_descriptors[index].interrupt_bitstream,
                        SIZE_OF_BITSTRAEM_IN_UINT32S
                      ) ;
  
  for (iii = 0 ; iii < MAX_INTERRUPT_CAUSE ; ++iii)
  {
    Soc_sand_chip_descriptors[index].interrupt_callback_array[iii].func   = NULL ;
    
    if (init_flag && Soc_sand_chip_descriptors[index].interrupt_callback_array[iii].buffer)
    {
      soc_sand_os_free(
        Soc_sand_chip_descriptors[index].interrupt_callback_array[iii].buffer
      ) ;
    }
    
    Soc_sand_chip_descriptors[index].interrupt_callback_array[iii].buffer = NULL ;
    Soc_sand_chip_descriptors[index].interrupt_callback_array[iii].size   = 0 ;
  }
  Soc_sand_chip_descriptors[index].unmask_ptr                      = NULL ;
  Soc_sand_chip_descriptors[index].is_bit_auto_clear_ptr           = NULL ;
  Soc_sand_chip_descriptors[index].is_device_interrupts_masked_ptr = NULL ;
  Soc_sand_chip_descriptors[index].get_device_interrupts_mask_ptr  = NULL ;
  Soc_sand_chip_descriptors[index].device_interrupt_mask_counter   = 0;
  Soc_sand_chip_descriptors[index].device_is_between_isr_to_tcm    = FALSE;
  Soc_sand_chip_descriptors[index].magic = soc_get_magic() ;
  Soc_sand_chip_descriptors[index].device_at_init = FALSE ;
  Soc_sand_chip_descriptors[index].valid_word = 0 ;
  
  if (init_flag)
  {
    soc_sand_os_mutex_give(Soc_sand_chip_descriptors[index].mutex_id);
    ex = soc_sand_os_mutex_delete(Soc_sand_chip_descriptors[index].mutex_id) ;
    if (ex != SOC_SAND_OK)
    {
      err = 1 ;
      goto exit ;
    }
  }
  Soc_sand_chip_descriptors[index].mutex_id      = 0 ;
  Soc_sand_chip_descriptors[index].mutex_owner   = 0 ;
  Soc_sand_chip_descriptors[index].mutex_counter = 0 ;
  ex = SOC_SAND_OK ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_CLEAR_CHIP_DESCRIPTOR,
        "General error in soc_sand_clear_chip_descriptor()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_RET
  soc_sand_init_chip_descriptors(
    uint32 max_devices
  )
{
  unsigned
    int
      iii ;
  SOC_SAND_RET
    ex ;
  uint32
    err = 0 ;
  ex = SOC_SAND_ERR ;
  
  if (Soc_sand_chip_descriptors || Soc_sand_array_mutex)
  {
    err = 1 ;
    goto exit ;
  }
  
  Soc_sand_chip_descriptors = soc_sand_os_malloc((max_devices * sizeof(SOC_SAND_DEVICE_DESC)), "Soc_sand_chip_descriptors") ;
  if (!Soc_sand_chip_descriptors)
  {
    err = 2 ;
    goto exit ;
  }
  
  Soc_sand_array_size = max_devices ;
  
  Soc_sand_array_mutex = soc_sand_os_mutex_create() ;
  if (!Soc_sand_array_mutex)
  {
    err = 3 ;
    goto exit ;
  }
  if (SOC_SAND_OK != soc_sand_os_mutex_take(Soc_sand_array_mutex, SOC_SAND_INFINITE_TIMEOUT))
  {
    err = 4 ;
    goto exit ;
  }
  
   for (iii = 0 ; iii < Soc_sand_array_size ; ++iii)
   {
     soc_sand_clear_chip_descriptor(iii,FALSE) ;
   }
   
   if (SOC_SAND_OK != soc_sand_os_mutex_give(Soc_sand_array_mutex))
   {
     err = 5 ;
     goto exit ;
   }
   ex = SOC_SAND_OK ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_INIT_CHIP_DESCRIPTORS,
        "General error in soc_sand_init_chip_descriptors()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_RET
 soc_sand_delete_chip_descriptors(
   void
 )
{
  unsigned
    int
      iii ;
  SOC_SAND_RET
    ex ;
  unsigned
    err = 0 ;
  ex = SOC_SAND_ERR ;
  
  if (!Soc_sand_chip_descriptors)
  {
    err = 1 ;
    goto exit ;
  }
  if (!Soc_sand_array_mutex)
  {
    err = 2 ;
    goto exit ;
  }
  
  
  for (iii=0 ; iii<Soc_sand_array_size ; ++iii)
  {
    if(soc_sand_is_chip_descriptor_valid(iii))
    {
      if (SOC_SAND_OK != soc_sand_take_chip_descriptor_mutex(iii)){
        err = 3 ;
        goto exit ;
      }
    }
  }
  
  if (SOC_SAND_OK != soc_sand_os_mutex_take(Soc_sand_array_mutex, SOC_SAND_INFINITE_TIMEOUT))
  {
    err = 4 ;
    goto exit ;
  }
  
  for (iii=0 ; iii<Soc_sand_array_size ; ++iii)
  {
    if(soc_sand_is_chip_descriptor_valid(iii))
    {
      
      soc_sand_clear_chip_descriptor(iii, TRUE) ;
    }
    else
    {
      soc_sand_clear_chip_descriptor(iii, FALSE) ;
    }
  }
  
  soc_sand_os_free(Soc_sand_chip_descriptors) ;
  Soc_sand_chip_descriptors = NULL ;
  Soc_sand_array_size       = 0 ;
  
  soc_sand_os_mutex_delete(Soc_sand_array_mutex) ;
  Soc_sand_array_mutex = 0 ;
  ex = SOC_SAND_OK ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_DELETE_CHIP_DESCRIPTORS,
        "General error in soc_sand_delete_chip_descriptors()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_RET
 soc_sand_remove_chip_descriptor(
   uint32 index
 )
{
  SOC_SAND_RET
    ex ;
  uint32
    err = 0 ;
  ex = SOC_SAND_ERR ;
  
  if (!Soc_sand_chip_descriptors || !Soc_sand_array_mutex)
  {
    err = 1 ;
    goto exit ;
  }
  
  if (index >= Soc_sand_array_size)
  {
    err = 2 ;
    goto exit ;
  }
  
  if (SOC_SAND_OK != soc_sand_os_mutex_take(Soc_sand_array_mutex, SOC_SAND_INFINITE_TIMEOUT))
  {
    err = 3 ;
    goto exit ;
  }
  
  soc_sand_clear_chip_descriptor(index, TRUE) ;
  
  if (SOC_SAND_OK != soc_sand_os_mutex_give(Soc_sand_array_mutex))
  {
    err = 4 ;
    goto exit ;
  }
  ex = SOC_SAND_OK ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_REMOVE_CHIP_DESCRIPTOR,
        "General error in soc_sand_remove_chip_descriptor()",err,0,0,0,0,0) ;
  return ex ;
}

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
  )
{
  int
    res ;
  uint32
    iii;
  sal_mutex_t
    mutex ;
  SOC_SAND_RET
    ex ;
  uint32
    err ;

  err = 0 ;
  ex = SOC_SAND_ERR ;
  
  iii = 0 ;
  res = 0 ;
  mutex = 0 ;
  
  if (!Soc_sand_chip_descriptors || !Soc_sand_array_mutex)
  {
    res = -1 ;
    goto exit ;
  }

  if (handle > 0)
  {
	  if (soc_sand_is_chip_descriptor_valid(handle))
      {
		  
		  res = -7;
		  goto exit;
	  }
	  else
	  {
		  iii = handle;
	  }
  }
  else
  {
	  for (iii=0 ; iii<Soc_sand_array_size ; ++iii)
	  {
		if (!soc_sand_is_chip_descriptor_valid(iii))
		{
		   break ;
		}
	  }
  }

  
  if (iii >= Soc_sand_array_size)
  {
    res = -3 ;
    goto exit ;
  }
 
  mutex = soc_sand_os_mutex_create() ;
  if (0 == mutex)
  {
    res = -6 ;
    goto exit ;
  }
  ex = SOC_SAND_OK ;
exit:
  if (0 == res)
  {
   Soc_sand_chip_descriptors[iii].mutex_id                          = mutex;
   Soc_sand_chip_descriptors[iii].mutex_id                          = mutex;
   Soc_sand_chip_descriptors[iii].base_addr                         = base_address;
   Soc_sand_chip_descriptors[iii].mem_size                          = mem_size;
   Soc_sand_chip_descriptors[iii].unmask_ptr                        = unmask_func_ptr;
   Soc_sand_chip_descriptors[iii].is_bit_auto_clear_ptr             = is_bit_ac_func_ptr;
   Soc_sand_chip_descriptors[iii].is_device_interrupts_masked_ptr   = is_dev_int_mask_func_ptr;
   Soc_sand_chip_descriptors[iii].get_device_interrupts_mask_ptr    = get_dev_mask_func_ptr;
   Soc_sand_chip_descriptors[iii].mask_specific_interrupt_cause_ptr = mask_specific_interrupt_cause_ptr;
   Soc_sand_chip_descriptors[iii].reset_device_ptr                  = reset_device_ptr;
   Soc_sand_chip_descriptors[iii].is_read_write_protect_ptr         = is_read_write_protect_ptr;
   Soc_sand_chip_descriptors[iii].logic_chip_type                   = logic_chip_type;
   Soc_sand_chip_descriptors[iii].chip_type                         = chip_type;
   Soc_sand_chip_descriptors[iii].magic                             = soc_get_magic();
   Soc_sand_chip_descriptors[iii].valid_word                        = 0;
   res = iii ;
  }
  err = (uint32)(-res) ;
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_ADD_CHIP_DESCRIPTOR,
        "General error in soc_sand_add_chip_descriptor()",err,0,0,0,0,0) ;
  return res ;
}





SOC_SAND_RET
  soc_sand_register_event_callback(
    SOC_SAND_INOUT  SOC_SAND_TCM_CALLBACK func,   
    SOC_SAND_INOUT  uint32     *buffer,
    SOC_SAND_INOUT  uint32     size,   
    SOC_SAND_IN     uint32      index,  
    SOC_SAND_IN     uint32      cause,  
    SOC_SAND_INOUT  uint32     *handle
  )
{
  uint32
      res ;
  SOC_SAND_RET
    ex = SOC_SAND_ERR ;
  uint32
    err = 0 ;
  
  if (index >= Soc_sand_array_size)
  {
    err = 1 ;
    goto exit ;
  }
  
  if (cause >= MAX_INTERRUPT_CAUSE)
  {
    err = 2 ;
    goto exit ;
  }
  
  if (!soc_sand_is_chip_descriptor_valid(index))
  {
    err = 3 ;
    goto exit ;
  }
  
  if (SOC_SAND_OK != soc_sand_take_chip_descriptor_mutex(index))
  {
    err = 4 ;
    goto exit ;
  }
  if (Soc_sand_chip_descriptors[index].interrupt_callback_array[cause].func)
  {
    if (SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(index))
    {
      err = 5 ;
      goto exit ;
    }
    err = 6 ;
    goto exit ;
  }
  
  Soc_sand_chip_descriptors[index].interrupt_callback_array[cause].func    = func ;
  Soc_sand_chip_descriptors[index].interrupt_callback_array[cause].buffer  = buffer ;
  Soc_sand_chip_descriptors[index].interrupt_callback_array[cause].size    = size ;
  if (SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(index))
  {
      err = 7 ;
      goto exit ;
  }
  
  res   = index ;
  res <<= 16 ;
  res  += cause ;
  *handle = res ;
  ex = SOC_SAND_OK ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_REGISTER_EVENT_CALLBACK,
        "General error in soc_sand_register_event_callback()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_RET
  soc_sand_unregister_event_callback(
    SOC_SAND_IN     uint32            handle
  )
{
  SOC_SAND_RET
    ex = SOC_SAND_ERR ;
  unsigned
    int
      index,
      cause1,
      cause2 ;
  unsigned
    err = 0 ;
  
  index = handle >> 16 ;
  cause1 = (handle << 24) >> 24 ;
  cause2 = (handle << 16) >> 24 ;
  
  if (index >= Soc_sand_array_size)
  {
    err = 1 ;
    goto exit ;
  }
  
  if (!soc_sand_is_chip_descriptor_valid(index))
  {
    err = 2 ;
    goto exit ;
  }
  
  if (SOC_SAND_OK != soc_sand_take_chip_descriptor_mutex(index))
  {
    err = 3 ;
    goto exit ;
  }
  
  soc_sand_chip_descriptor_get_mask_specific_interrupt_cause_func(index)(index, cause1);
  
  if (Soc_sand_chip_descriptors[index].interrupt_callback_array[cause1].buffer)
  {
    soc_sand_os_free(
      Soc_sand_chip_descriptors[index].interrupt_callback_array[cause1].buffer
    ) ;
  }
  
  Soc_sand_chip_descriptors[index].interrupt_callback_array[cause1].func    = NULL ;
  Soc_sand_chip_descriptors[index].interrupt_callback_array[cause1].buffer  = NULL ;
  Soc_sand_chip_descriptors[index].interrupt_callback_array[cause1].size    = 0 ;
  
  if (cause2)
  {
  
  soc_sand_chip_descriptor_get_mask_specific_interrupt_cause_func(index)(index, cause2);
    
    if (Soc_sand_chip_descriptors[index].interrupt_callback_array[cause2].buffer)
    {
      soc_sand_os_free(
        Soc_sand_chip_descriptors[index].interrupt_callback_array[cause2].buffer
      ) ;
    }
    
    Soc_sand_chip_descriptors[index].interrupt_callback_array[cause2].func    = NULL ;
    Soc_sand_chip_descriptors[index].interrupt_callback_array[cause2].buffer  = NULL ;
    Soc_sand_chip_descriptors[index].interrupt_callback_array[cause2].size    = 0 ;
  }
  if (soc_sand_give_chip_descriptor_mutex(index) != SOC_SAND_OK)
  {
    err = 4 ;
    goto exit ;
  }
  ex = SOC_SAND_OK ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_UNREGISTER_EVENT_CALLBACK,
        "General error in soc_sand_unregister_event_callback()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_RET
  soc_sand_combine_2_event_callback_handles(
    SOC_SAND_IN     uint32            handle_1,
    SOC_SAND_IN     uint32            handle_2,
    SOC_SAND_INOUT  uint32           *handle
  )
{
  SOC_SAND_RET
    ex ;
  unsigned
    int
      index_1,
      index_2,
      cause_2 ;
  uint32
    err = 0 ;
  
  index_1 = handle_1 >> 16 ;
  index_2 = handle_2 >> 16 ;
  cause_2 = (handle_2 << 16) >> 16 ;
  
  ex = SOC_SAND_OK ;
  if (!handle_1)
  {
    *handle = handle_2 ;
    goto exit ;
  }
  
  if (!handle_2)
  {
    *handle = handle_1 ;
    goto exit ;
  }

  if (index_1 != index_2)
  {
    
    err = 1 ;
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
  
  cause_2 <<= 8 ;
  *handle = handle_1 | cause_2 ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_COMBINE_2_EVENT_CALLBACK_HANDLES,
        "General error in soc_sand_combine_2_event_callback_handles()",err,0,0,0,0,0) ;
  return ex ;

}



uint32
  soc_sand_is_any_chip_descriptor_taken_by_task(
    sal_thread_t task_id
  )
{
  uint32
    chip_desc_taken_flag = FALSE,
    index_i;

  for(index_i = 0;index_i < Soc_sand_array_size; index_i++)
  {
    if(Soc_sand_chip_descriptors[index_i].mutex_owner == task_id)
    {
      chip_desc_taken_flag = TRUE;
      goto exit;
    }
  }
exit:
  return chip_desc_taken_flag;
}

uint32
  soc_sand_is_any_chip_descriptor_taken(
    void
  )
{
  uint32
    chip_desc_taken_flag = FALSE,
    index_i;

  for(index_i = 0;index_i < Soc_sand_array_size; index_i++)
  {
    if(soc_sand_is_chip_descriptor_valid(index_i))
    {
      chip_desc_taken_flag = TRUE;
      goto exit;
    }
  }
exit:
  return chip_desc_taken_flag;
}

sal_thread_t
  soc_sand_get_chip_descriptor_mutex_owner(
    uint32 index
  )
{
  sal_thread_t
    ex ;
  SOC_SAND_RET
    error_code ;
  uint32
    err = 0 ;
  error_code = SOC_SAND_OK ;
  
  if (index >= Soc_sand_array_size)
  {
    ex = (sal_thread_t)(-1) ;
    err = 1;
    error_code = SOC_SAND_ERR ;
    goto exit ;
  }
  
  ex = Soc_sand_chip_descriptors[index].mutex_owner ;
  
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_MUTEX_OWNER,
        "General error in soc_sand_get_chip_descriptor_mutex_owner()",err,0,0,0,0,0) ;
  return ex ;
}

STATIC
  SOC_SAND_RET
    soc_sand_set_chip_descriptor_mutex_owner(
      uint32 index,
      sal_thread_t         tid
    )
{
  SOC_SAND_RET
    ex = SOC_SAND_OK ;
  uint32
    err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    err = 1 ;
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
  Soc_sand_chip_descriptors[index].mutex_owner = tid ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_SET_CHIP_DESCRIPTOR_MUTEX_OWNER,
        "General error in soc_sand_set_chip_descriptor_mutex_owner()",err,0,0,0,0,0) ;
  return ex ;
}

STATIC
  int32
    soc_sand_get_chip_descriptor_mutex_counter(
      uint32 index
    )
{
  int32
    ex ;
  SOC_SAND_RET
    error_code ;
  uint32
    err = 0 ;
  error_code = SOC_SAND_OK ;
  if (index >= Soc_sand_array_size)
  {
    ex = -1 ;
    error_code = SOC_SAND_ERR ;
    goto exit ;
  }
  ex = Soc_sand_chip_descriptors[index].mutex_counter ;
exit:
  err = (uint32)(-ex) ;
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_MUTEX_COUNTER,
        "General error in soc_sand_get_chip_descriptor_mutex_counter()",err,0,0,0,0,0) ;
  return ex ;
}

STATIC
  SOC_SAND_RET
    soc_sand_set_chip_descriptor_mutex_counter(
      uint32 index,
      int32 cnt
    )
{
  SOC_SAND_RET
    ex = SOC_SAND_OK ;
  uint32
    err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    err = 1 ;
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
   Soc_sand_chip_descriptors[index].mutex_counter = cnt ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_SET_CHIP_DESCRIPTOR_MUTEX_COUNTER,
        "General error in soc_sand_set_chip_descriptor_mutex_counter()",err,0,0,0,0,0) ;
  return ex ;
}

STATIC
  SOC_SAND_RET
    soc_sand_inc_chip_descriptor_mutex_counter(
      uint32 index
    )
{
  SOC_SAND_RET
    ex = SOC_SAND_OK ;
  uint32
    err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    err = 1 ;
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
  
  Soc_sand_chip_descriptors[index].mutex_counter++ ;
  
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_INC_CHIP_DESCRIPTOR_MUTEX_COUNTER,
        "General error in soc_sand_inc_chip_descriptor_mutex_counter()",err,0,0,0,0,0) ;
  return ex ;
}

STATIC
  SOC_SAND_RET
    soc_sand_dec_chip_descriptor_mutex_counter(
      uint32 index
    )
{
  SOC_SAND_RET
    ex = SOC_SAND_OK ;
  uint32
    err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    err = 1 ;
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
  
  Soc_sand_chip_descriptors[index].mutex_counter-- ;
  if (Soc_sand_chip_descriptors[index].mutex_counter < 0)
  {
    err = 2 ;
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
  
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_DEC_CHIP_DESCRIPTOR_MUTEX_COUNTER,
        "General error in soc_sand_dec_chip_descriptor_mutex_counter()",err,0,0,0,0,0) ;
  return ex ;
}

int
  soc_sand_take_chip_descriptor_mutex(
    uint32 index
  )
{
  int
    ret ;
  SOC_SAND_RET
    ex ;
  uint32
      local_magic ;
  sal_thread_t
    current_task_id;

  ex = SOC_SAND_ERR ;
  
  current_task_id = soc_sand_os_get_current_tid();
  if (index >= Soc_sand_array_size)
  {
    ex = SOC_SAND_ILLEGAL_DEVICE_ID ;
    ret = -1 ;
    goto exit ;
  }
  
  if (!soc_sand_is_chip_descriptor_valid(index))
  {
    ex = SOC_SAND_ILLEGAL_CHIP_DESCRIPTOR ;
    ret = -2 ;
    goto exit ;
  }
  
  if (current_task_id == soc_sand_get_chip_descriptor_mutex_owner(index))
  {
    soc_sand_inc_chip_descriptor_mutex_counter(index) ;
    ret = 0 ;
    ex = SOC_SAND_OK ;
    goto exit ;
  }
  if(current_task_id == soc_sand_handles_delta_list_get_owner())
  {
    soc_sand_inc_chip_descriptor_mutex_counter(index) ;
    ret = -12;
    ex = SOC_SAND_CHIP_DESCRIPTORS_SEM_TAKE_ERR_0 ;
    goto exit ;
  }

  
  local_magic = soc_sand_get_chip_descriptor_magic(index) ;
  if (SOC_SAND_OK != soc_sand_os_mutex_take(Soc_sand_chip_descriptors[index].mutex_id, (long)SOC_SAND_INFINITE_TIMEOUT))
  {
    if (!soc_sand_is_chip_descriptor_valid(index))
    {
      
      ex = SOC_SAND_TRYING_TO_ACCESS_DELETED_DEVICE ;
      ret = -3 ;
      goto exit ;
    }
    else
    {
      
      ex = SOC_SAND_SEM_TAKE_FAIL ;
      ret = -4 ;
      goto exit ;
    }
  }
  
  if(soc_sand_get_chip_descriptor_magic(index) != local_magic)
  {
    soc_sand_os_mutex_give(Soc_sand_chip_descriptors[index].mutex_id) ;
    ex = SOC_SAND_SEM_CHANGED_ON_THE_FLY ;
    ret = -5 ;
    goto exit ;
  }
  
  soc_sand_set_chip_descriptor_mutex_owner(index, soc_sand_os_get_current_tid()) ;
  soc_sand_set_chip_descriptor_mutex_counter(index, 1) ;
  ret = 0 ;
  ex = SOC_SAND_OK ;
exit:
  SOC_SAND_ERROR_REPORT(
    ex,NULL,0,0,SOC_SAND_TAKE_CHIP_DESCRIPTOR_MUTEX,
    "General error in soc_sand_take_chip_descriptor_mutex()",
    -ret,0,0,0,0,0
  );
  return ret ;
}

SOC_SAND_RET
  soc_sand_give_chip_descriptor_mutex(
    uint32 index
  )
{
  SOC_SAND_RET
    ex = SOC_SAND_OK ;
  uint32
    err = 0 ;
  if (soc_sand_os_get_current_tid() != soc_sand_get_chip_descriptor_mutex_owner(index))
  {
    err = 1 ;
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
  soc_sand_dec_chip_descriptor_mutex_counter(index) ;
  if (soc_sand_get_chip_descriptor_mutex_counter(index) == 0)
  {
    soc_sand_set_chip_descriptor_mutex_owner(index, 0) ;
    soc_sand_os_mutex_give(Soc_sand_chip_descriptors[index].mutex_id) ;
  }
  else if (soc_sand_get_chip_descriptor_mutex_counter(index) < 0)
  {
    err = 2 ;
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_GIVE_CHIP_DESCRIPTOR_MUTEX,
        "General error in soc_sand_give_chip_descriptor_mutex()",err,0,0,0,0,0) ;
  return ex ;
}



SOC_SAND_RET
  soc_sand_set_chip_descriptor_ver_info(
    SOC_SAND_IN uint32    index,
    SOC_SAND_IN uint32   dbg_ver,
    SOC_SAND_IN uint32   chip_ver
  )
{
  SOC_SAND_RET
    error_code = SOC_SAND_OK ;
  uint32
    err = 0 ;


  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR ;
    err = 1 ;
    goto exit ;
  }


  Soc_sand_chip_descriptors[index].dbg_ver  = dbg_ver ;
  Soc_sand_chip_descriptors[index].chip_ver = chip_ver ;

exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_SET_CHIP_DESCRIPTOR_VER_INFO,
        "General error in soc_sand_set_chip_descriptor_ver_info()",err,0,0,0,0,0) ;
  return error_code ;
}

uint32
  *soc_sand_get_chip_descriptor_base_addr(
    uint32 index
  )
{
  uint32
      *ex ;
  SOC_SAND_RET
    error_code = SOC_SAND_OK ;
  uint32
      err ;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR ;
    err = 1 ;
    ex = 0 ;
    goto exit ;
  }
  ex = Soc_sand_chip_descriptors[index].base_addr ;
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_BASE_ADDR,
        "General error in soc_sand_get_chip_descriptor_base_addr()",err,0,0,0,0,0) ;
  return ex ;
}

uint32
  soc_sand_get_chip_descriptor_memory_size(
    uint32 index
  )
{
  uint32
      ex ;
  SOC_SAND_RET
    error_code = SOC_SAND_OK ;
  uint32
    err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR ;
    err = 1 ;
    ex = 0 ;
    goto exit ;
  }
  ex = Soc_sand_chip_descriptors[index].mem_size ;
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_MEMORY_SIZE,
        "General error in soc_sand_get_chip_descriptor_memory_size()",err,0,0,0,0,0) ;
  return ex ;
}

struct soc_sand_tcm_callback_str
  *soc_sand_get_chip_descriptor_interrupt_callback_array(
    uint32 index
   )
{
   struct
    soc_sand_tcm_callback_str
      *ex ;
  SOC_SAND_RET
    error_code = SOC_SAND_OK ;
  uint32
    err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR ;
    err = 1 ;
    ex = 0 ;
    goto exit ;
  }
  ex = Soc_sand_chip_descriptors[index].interrupt_callback_array ;
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_INTERRUPT_CALLBACK_ARRAY,
        "General error in soc_sand_get_chip_descriptor_interrupt_callback_array()",err,0,0,0,0,0) ;
  return ex ;
}

uint32
  *soc_sand_get_chip_descriptor_interrupt_bitstream(
    uint32 index
  )
{
  uint32
     *ex ;
  SOC_SAND_RET
    error_code = SOC_SAND_OK ;
  uint32
    err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR ;
    err = 1 ;
    ex = 0 ;
    goto exit ;
  }
  ex = Soc_sand_chip_descriptors[index].interrupt_bitstream ;
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_INTERRUPT_CALLBACK_ARRAY,
        "General error in soc_sand_get_chip_descriptor_interrupt_callback_array()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_UNMASK_FUNC_PTR
  soc_sand_get_chip_descriptor_unmask_func(
    uint32 index
  )
{
  SOC_SAND_UNMASK_FUNC_PTR ex;
  SOC_SAND_RET error_code;
  uint32 err;
  
  err = 0;
  error_code = SOC_SAND_OK;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR ;
    err = 1 ;
    ex = NULL ;
    goto exit ;
  }
  
  ex = Soc_sand_chip_descriptors[index].unmask_ptr ;
  
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_UNMASK_FUNC,
        "General error in soc_sand_get_chip_descriptor_unmask_func()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_IS_BIT_AUTO_CLEAR_FUNC_PTR
  soc_sand_get_chip_descriptor_is_bit_auto_clear_func(
    uint32 index
  )
{
  SOC_SAND_IS_BIT_AUTO_CLEAR_FUNC_PTR ex;
  SOC_SAND_RET error_code;
  uint32 err ;
  
  error_code = SOC_SAND_OK;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR;
    err = 1;
    ex = NULL;
    goto exit;
  }
  
  ex = Soc_sand_chip_descriptors[index].is_bit_auto_clear_ptr;
  
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_IS_BIT_AC_FUNC,
        "General error in soc_sand_get_chip_descriptor_is_bit_auto_clear_func()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_IS_DEVICE_INTERRUPTS_MASKED
  soc_sand_get_chip_descriptor_is_device_interrupts_masked_func(
    uint32 index
  )
{
  SOC_SAND_IS_DEVICE_INTERRUPTS_MASKED ex;
  SOC_SAND_RET error_code;
  uint32 err ;
  
  error_code = SOC_SAND_OK;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR;
    err = 1;
    ex = NULL;
    goto exit;
  }
  
  ex = Soc_sand_chip_descriptors[index].is_device_interrupts_masked_ptr;
  
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_IS_DEVICE_INT_MASKED_FUNC,
        "General error in soc_sand_get_chip_descriptor_is_device_interrupts_masked_func()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_GET_DEVICE_INTERRUPTS_MASK
  soc_sand_chip_descriptor_get_interrupts_mask_func(
    uint32 index
  )
{
  SOC_SAND_GET_DEVICE_INTERRUPTS_MASK ex;
  SOC_SAND_RET error_code;
  uint32 err ;
  
  error_code = SOC_SAND_OK;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR;
    err = 1;
    ex = NULL;
    goto exit;
  }
  
  ex = Soc_sand_chip_descriptors[index].get_device_interrupts_mask_ptr;
  
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_GET_INT_MASK_FUNC,
        "General error in soc_sand_chip_descriptor_get_interrupts_mask_func()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_MASK_SPECIFIC_INTERRUPT_CAUSE
  soc_sand_chip_descriptor_get_mask_specific_interrupt_cause_func(
    uint32 index
  )
{
  SOC_SAND_MASK_SPECIFIC_INTERRUPT_CAUSE ex;
  SOC_SAND_RET error_code;
  uint32 err ;
  
  error_code = SOC_SAND_OK;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR;
    err = 1;
    ex = NULL;
    goto exit;
  }
  
  ex = Soc_sand_chip_descriptors[index].mask_specific_interrupt_cause_ptr;
  
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_GET_INT_MASK_FUNC,
        "General error in soc_sand_chip_descriptor_get_interrupts_mask_func()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_RESET_DEVICE_FUNC_PTR
  soc_sand_chip_descriptor_get_reset_device_func(
    uint32 index
  )
{
  SOC_SAND_RESET_DEVICE_FUNC_PTR ex;
  SOC_SAND_RET error_code;
  uint32 err ;
  
  error_code = SOC_SAND_OK;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR;
    err = 1;
    ex = NULL;
    goto exit;
  }
  
  ex = Soc_sand_chip_descriptors[index].reset_device_ptr;
  
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_GET_INT_MASK_FUNC,
        "General error in soc_sand_chip_descriptor_get_reset_device_func()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_IS_OFFSET_READ_OR_WRITE_PROTECT_FUNC_PTR
  soc_sand_chip_descriptor_get_is_read_write_protect_func(
    uint32 index
  )
{
  SOC_SAND_IS_OFFSET_READ_OR_WRITE_PROTECT_FUNC_PTR ex;
  SOC_SAND_RET error_code;
  uint32 err;
  
  error_code = SOC_SAND_OK;
  err = 0;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR;
    err = 1;
    ex = NULL;
    goto exit;
  }
  
  ex = Soc_sand_chip_descriptors[index].is_read_write_protect_ptr;
  
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_GET_INT_MASK_FUNC,
        "General error in soc_sand_chip_descriptor_get_is_read_write_protect_func()",err,0,0,0,0,0) ;
  return ex;
}

uint32
  soc_sand_device_is_between_isr_to_tcm(
    uint32 index
  )
{
  uint32 ex;
  SOC_SAND_RET error_code;
  uint32 err ;
  
  error_code = SOC_SAND_OK;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR;
    err = 1;
    ex = FALSE;
    goto exit;
  }
  
  ex = Soc_sand_chip_descriptors[index].device_is_between_isr_to_tcm;
  
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_DEV_BETWEEN_ISR_AND_TCM,
        "General error in soc_sand_device_is_between_isr_to_tcm()",err,0,0,0,0,0) ;
  return ex ;
}
void
  soc_sand_device_set_between_isr_to_tcm(
    uint32 index,
  uint32 level
  )
{
  SOC_SAND_RET error_code;
  uint32 err ;
  
  error_code = SOC_SAND_OK;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR;
    err = 1;
    goto exit;
  }
  
  Soc_sand_chip_descriptors[index].device_is_between_isr_to_tcm = level;
  
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_SET_CHIP_DESCRIPTOR_DEV_BETWEEN_ISR_AND_TCM,
        "General error in soc_sand_device_set_between_isr_to_tcm()",err,0,0,0,0,0) ;
}

int
  soc_sand_device_get_interrupt_mask_counter(
    uint32 index
  )
{
  int ex;
  SOC_SAND_RET error_code;
  uint32 err ;
  
  error_code = SOC_SAND_OK;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR;
    err = 1;
    ex = 0;
    goto exit;
  }
  
  ex = Soc_sand_chip_descriptors[index].device_interrupt_mask_counter;
  
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_INT_MASK_COUNTER,
        "General error in soc_sand_device_get_interrupt_mask_counter()",err,0,0,0,0,0) ;
  return ex ;
}
void
  soc_sand_device_inc_interrupt_mask_counter(
    uint32 index
  )
{
  SOC_SAND_RET error_code;
  uint32 err ;
  
  error_code = SOC_SAND_OK;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR;
    err = 1;
    goto exit;
  }
  
  Soc_sand_chip_descriptors[index].device_interrupt_mask_counter ++;
  
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_INC_CHIP_DESCRIPTOR_INT_MASK_COUNTER,
        "General error in soc_sand_device_inc_interrupt_mask_counter()",err,0,0,0,0,0) ;
}
void
  soc_sand_device_dec_interrupt_mask_counter(
    uint32 index
  )
{
  SOC_SAND_RET error_code;
  uint32 err ;
  
  error_code = SOC_SAND_OK;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR;
    err = 1;
    goto exit;
  }
  
  Soc_sand_chip_descriptors[index].device_interrupt_mask_counter --;
  
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_DEC_CHIP_DESCRIPTOR_INT_MASK_COUNTER,
        "General error in soc_sand_device_dec_interrupt_mask_counter()",err,0,0,0,0,0) ;
}

uint32
  soc_sand_is_chip_descriptor_chip_ver_bigger_eq(
    uint32   index,
    uint32  chip_ver_bigger_eq
  )
{
  unsigned
    int
      ex ;
  SOC_SAND_RET
    error_code = SOC_SAND_OK ;
  uint32
    err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR ;
    err = 1 ;
    ex = FALSE ;
    goto exit ;
  }

#if FE200_REV_B_AS_REV_A
  
  if (Soc_sand_chip_descriptors[index].chip_type == 0xFE200)
  {
    ex = FALSE;
    goto exit;
  }
#endif

  if(Soc_sand_chip_descriptors[index].chip_ver >= chip_ver_bigger_eq)
  {
    ex = TRUE;
  }
  else
  {
    ex = FALSE;
  }

exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_IS_CHIP_DESCRIPTOR_CHIP_VER_BIGGER_EQ,
        "General error in soc_sand_is_chip_descriptor_chip_ver_bigger_eq()",err,0,0,0,0,0) ;
  return ex ;
}

#define SIMULATE_REV_B_ON_REV_A 0
uint32
  soc_sand_get_chip_descriptor_chip_ver(
    uint32 index
  )
{
  uint32
      ex ;
  SOC_SAND_RET
    error_code = SOC_SAND_OK ;
  uint32
      err ;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR ;
    err = 1 ;
    ex = 0 ;
    goto exit ;
  }
  ex = Soc_sand_chip_descriptors[index].chip_ver ;
#if SIMULATE_REV_B_ON_REV_A
  ex = 0x02;
#endif
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_CHIP_VER,
        "General error in soc_sand_get_chip_descriptor_chip_ver()",err,0,0,0,0,0) ;
  return ex ;
}

uint32
  soc_sand_get_chip_descriptor_dbg_ver(
    uint32 index
  )
{
  uint32
      ex ;
  SOC_SAND_RET
    error_code = SOC_SAND_OK ;
  uint32
      err ;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR ;
    err = 1 ;
    ex = 0 ;
    goto exit ;
  }
  ex = Soc_sand_chip_descriptors[index].dbg_ver ;
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_DBG_VER,
        "General error in soc_sand_get_chip_descriptor_dbg_ver()",err,0,0,0,0,0) ;
  return ex ;
}

uint32
  soc_sand_get_chip_descriptor_chip_type(
    uint32 index
  )
{
  uint32
      ex ;
  SOC_SAND_RET
    error_code = SOC_SAND_OK ;
  uint32
      err ;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR ;
    err = 1 ;
    ex = 0 ;
    goto exit ;
  }
  ex = Soc_sand_chip_descriptors[index].chip_type ;
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_CHIP_TYPE,
        "General error in soc_sand_get_chip_descriptor_chip_type()",err,0,0,0,0,0) ;
  return ex ;
}

uint32
  soc_sand_get_chip_descriptor_logic_chip_type(
    uint32 index
  )
{
  uint32
      ex ;
  SOC_SAND_RET
    error_code = SOC_SAND_OK ;
  uint32
      err ;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR ;
    err = 1 ;
    ex = 0 ;
    goto exit ;
  }
  ex = Soc_sand_chip_descriptors[index].logic_chip_type ;
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_CHIP_TYPE,
        "General error in soc_sand_get_chip_descriptor_logic_chip_type()",err,0,0,0,0,0) ;
  return ex ;
}

void
  soc_sand_set_chip_descriptor_valid(
    uint32 index
  )
{
  Soc_sand_chip_descriptors[index].valid_word = SOC_SAND_DEVICE_DESC_VALID_WORD ;
}

uint32
  soc_sand_is_chip_descriptor_valid(
    uint32 index
  )
{
  uint32
    ex ;
  SOC_SAND_RET
    error_code = SOC_SAND_OK ;
  uint32
      err ;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR ;
    err = 1 ;
    ex = FALSE ;
    goto exit ;
  }
  
  if (SOC_SAND_DEVICE_DESC_VALID_WORD == Soc_sand_chip_descriptors[index].valid_word)
  {
    ex = TRUE ;
  }
  else
  {
    ex = FALSE ;
  }
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_IS_CHIP_DESCRIPTOR_VALID,
        "General error in soc_sand_is_chip_descriptor_valid()",err,index,0,0,0,0) ;
  return ex ;
}

SOC_SAND_RET
  soc_sand_set_chip_descriptor_device_at_init(
    uint32 index,
    uint32 val
  )
{
  SOC_SAND_RET error_code;
  uint32 err ;
  error_code = SOC_SAND_OK;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR;
    err = 1;
    goto exit;
  }
  if (val)
  {
    Soc_sand_chip_descriptors[index].device_at_init = TRUE ;
  }
  else
  {
    Soc_sand_chip_descriptors[index].device_at_init = FALSE ;
  }
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_SET_CHIP_DESCRIPTOR_DEVICE_AT_INIT,
        "General error in soc_sand_set_chip_descriptor_device_at_init()",err,0,0,0,0,0) ;
  return (error_code) ;
}

SOC_SAND_RET
  soc_sand_get_chip_descriptor_device_at_init(
    uint32 index,
    uint32 *val
  )
{
  SOC_SAND_RET
    error_code = SOC_SAND_OK ;
  uint32
      err ;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR ;
    err = 1 ;
    goto exit ;
  }
  *val = Soc_sand_chip_descriptors[index].device_at_init ;
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_DEVICE_AT_INIT,
        "General error in soc_sand_get_chip_descriptor_device_at_init()",err,0,0,0,0,0) ;
  return (error_code) ;
}

uint32
  soc_sand_get_chip_descriptor_magic(
    uint32 index
  )
{
  uint32
    ex ;
  SOC_SAND_RET
    error_code = SOC_SAND_OK ;
  uint32
      err ;
  err = 0 ;
  if (index >= Soc_sand_array_size)
  {
    error_code = SOC_SAND_ERR ;
    err = 1 ;
    ex = 0 ;
    goto exit ;
  }
  ex = Soc_sand_chip_descriptors[index].magic ;
exit:
  SOC_SAND_ERROR_REPORT(error_code,NULL,0,0,SOC_SAND_GET_CHIP_DESCRIPTOR_MAGIC,
        "General error in soc_sand_get_chip_descriptor_magic()",err,0,0,0,0,0) ;
  return ex ;
}

#if SOC_SAND_DEBUG


void
  soc_sand_chip_descriptors_print(
    void
  )
{
  uint32
    descriptor_i;
  
  if ( (0    == Soc_sand_array_mutex) ||
       (NULL == Soc_sand_chip_descriptors)
     )
  {
    LOG_CLI((BSL_META("soc_sand_chip_descriptors(): do not have internal params (probably the driver was not started)\n\r")));
    goto exit;
  }
  
  if (SOC_SAND_OK != soc_sand_os_mutex_take(Soc_sand_array_mutex, SOC_SAND_INFINITE_TIMEOUT))
  {
    LOG_CLI((BSL_META("soc_sand_chip_descriptors(): soc_sand_os_mutex_take() failed\n\r")));
    goto exit ;
  }
  
  for (descriptor_i=0; descriptor_i<Soc_sand_array_size; descriptor_i++)
  {
    if (0 == Soc_sand_chip_descriptors[descriptor_i].valid_word)
    {
      
      continue;
    }
    if (FALSE == soc_sand_is_chip_descriptor_valid(descriptor_i))
    {
      
      LOG_CLI((BSL_META("device[%2u] descriptor not valid\n\r"), descriptor_i));
      continue;
    }
    LOG_CLI((BSL_META("device[%2u] %s (0x%08X), chip_ver:%2u dbg_ver:%2u, \n\r"
"            address:0x%08X. Mutex: counter-%lu, owner-%p\n\r"),
             descriptor_i,
             soc_sand_DEVICE_TYPE_to_str(
             Soc_sand_chip_descriptors[descriptor_i].logic_chip_type
             ),
             Soc_sand_chip_descriptors[descriptor_i].chip_type,
             Soc_sand_chip_descriptors[descriptor_i].chip_ver,
             Soc_sand_chip_descriptors[descriptor_i].dbg_ver,
             PTR_TO_INT(Soc_sand_chip_descriptors[descriptor_i].base_addr),
             (unsigned long)Soc_sand_chip_descriptors[descriptor_i].mutex_counter,
             (void *)(Soc_sand_chip_descriptors[descriptor_i].mutex_owner)
             ));
  }
  
  if (SOC_SAND_OK != soc_sand_os_mutex_give(Soc_sand_array_mutex))
  {
    LOG_CLI((BSL_META("soc_sand_chip_descriptors(): soc_sand_os_mutex_give() failed\n\r")));
    goto exit ;
  }
  
exit:
  return;
}




#endif


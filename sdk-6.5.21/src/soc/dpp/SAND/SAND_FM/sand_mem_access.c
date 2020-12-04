/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/




#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>



uint32
  Soc_interrupt_mask_address_arr[SOC_SAND_MAX_DEVICE] ;


SOC_SAND_RET
  soc_sand_mem_interrupt_mask_address_set(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32 interrupt_mask_address
  )
{
  if (unit < SOC_SAND_MAX_DEVICE)
  {
    Soc_interrupt_mask_address_arr[unit] = interrupt_mask_address ;
  }
  return SOC_SAND_OK ;
}


uint32
  soc_sand_mem_interrupt_mask_address_get(
    SOC_SAND_IN int  unit
  )
{
  uint32 ret ;
  ret = 0xFFFFFFFF ;

  if (unit < SOC_SAND_MAX_DEVICE)
  {
    ret = Soc_interrupt_mask_address_arr[unit] ;
  }

  return ret ;
}


SOC_SAND_RET
  soc_sand_mem_interrupt_mask_address_clear(
    SOC_SAND_IN int  unit
  )
{
  SOC_SAND_RET
    ex ;
  ex = soc_sand_mem_interrupt_mask_address_set(unit, 0xFFFFFFFF) ;
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_MEM_INTERRUPT_MASK_ADDRESS_CLEAR,
        "General error in soc_sand_mem_interrupt_mask_address_clear()",0,0,0,0,0,0) ;
  return ex ;
}


SOC_SAND_RET
  soc_sand_mem_interrupt_mask_address_clear_all(
    void
  )
{
  unsigned
    int
      device_i ;
  for (device_i=0 ; device_i<SOC_SAND_MAX_DEVICE ; device_i++)
  {
    soc_sand_mem_interrupt_mask_address_clear(device_i) ;
  }
  return SOC_SAND_OK ;
}



SOC_SAND_RET
  soc_sand_mem_read(
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT uint32    *result_ptr,
    SOC_SAND_IN  uint32    offset,
    SOC_SAND_IN  uint32     size,
    SOC_SAND_IN  uint32     indirect
  )
{
  uint32
    err;
  SOC_SAND_RET
    ex;
  int
    res;

  ex           = SOC_SAND_OK;
  res          = SOC_SAND_OK;

  soc_sand_initialize_error_word(SOC_SAND_MEM_READ,0,&err);
  soc_sand_check_driver_and_device(unit, &err);
  ex = soc_sand_get_error_code_from_error_word(err);
  if (ex != SOC_SAND_OK)
  {
    err = 1;
    goto exit;
  }
  
  if (NULL == result_ptr)
  {
    err = 14;
    ex = SOC_SAND_NULL_POINTER_ERR;
    goto exit;
  }

  res = soc_sand_take_chip_descriptor_mutex(unit) ;
  if (res)
  {
    if (0 < res)
    {
      ex = SOC_SAND_SEM_TAKE_FAIL;
      err = 4;
      goto exit;
    }
    if (0 > res)
    {
      ex = SOC_SAND_ILLEGAL_DEVICE_ID;
      err = 5;
      goto exit;
    }
  }
  

   ex = soc_sand_mem_read_unsafe(
          unit,
          result_ptr,
          offset,
          size,
          indirect
        );

  if (ex!=SOC_SAND_OK)
  {
    ex = SOC_SAND_MEM_READ_010 ;
    goto exit_semaphore;
   }

exit_semaphore:
   if (SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(unit))
   {
     ex = SOC_SAND_SEM_GIVE_FAIL ;
     err = 6 ;
     goto exit ;
  }

exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_MEM_READ,
        "General error in soc_sand_mem_read()",err, offset, size, indirect, 0,0) ;
  return ex ;
}

SOC_SAND_RET
  soc_sand_mem_read_unsafe(
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT uint32    *result_ptr,
    SOC_SAND_IN  uint32    offset,
    SOC_SAND_IN  uint32     size,
    SOC_SAND_IN  uint32     indirect
  )
{


  uint32
    *base_address;
  uint32
    err;
  SOC_SAND_RET
    ex;
  SOC_SAND_RET
    res;

  ex = SOC_SAND_OK;
  res = SOC_SAND_OK;


  soc_sand_initialize_error_word(SOC_SAND_MEM_READ_UNSAFE, 0, &err);

  ex = soc_sand_get_error_code_from_error_word(err);

  if (ex != SOC_SAND_OK)
  {
    err = 1;
    goto exit;
  }


  if (FALSE == indirect)
  {
    
    if (!soc_sand_is_long_aligned(offset))
    {
      ex = SOC_SAND_MEM_READ_US_001 ;
      err = 2;
      goto exit;
    }
    if (!soc_sand_is_long_aligned(size))
    {
      ex = SOC_SAND_MEM_READ_US_002 ;
      err = 2;
      goto exit;
    }
    
    if( soc_sand_mem_check_read_write_protect_get() &&
        soc_sand_chip_descriptor_get_is_read_write_protect_func(unit)
      )
    {
      if ( soc_sand_chip_descriptor_get_is_read_write_protect_func(unit)(
             TRUE , offset, size
           )
         )
      {
        ex = SOC_SAND_MEM_READ_US_003;
        err = 3;
        goto exit;
      }
    }
    
    if ((offset <= soc_sand_mem_interrupt_mask_address_get(unit)) &&
         ((offset + (size>>2)) > (soc_sand_mem_interrupt_mask_address_get(unit)))
       )
    {
      ex = SOC_SAND_MEM_READ_US_003;
      err = 3;
      goto exit;
    }
  }

  base_address = soc_sand_get_chip_descriptor_base_addr(unit);
  if (indirect)
  {
    res = soc_sand_indirect_read_from_chip(
            unit,
            result_ptr,
            offset,
            size,
            SOC_SAND_IND_GET_MODULE_BIT_FROM_OFFSET
          );
    if(SOC_SAND_OK != res)
    {
      ex = SOC_SAND_MEM_READ_US_005;
      err = 11;
      goto exit;
    }
  }
  else
  {
    res = soc_sand_physical_read_from_chip(
            result_ptr,
            base_address,
            offset,
            size
          );

    if (SOC_SAND_OK != res)
    {
      ex = SOC_SAND_MEM_READ_US_006;
      err = 12;
      goto exit;
    }
  }

exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_MEM_READ_UNSAFE,
    "General error in soc_sand_mem_read_unsafe()",err, offset, size, indirect, 0,0) ;

  return res ;
}




SOC_SAND_RET
  soc_sand_mem_write(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32 *result_ptr,
    SOC_SAND_IN uint32 offset,
    SOC_SAND_IN uint32  size,
    SOC_SAND_IN uint32  indirect
  )
{

  uint32
    err;
  SOC_SAND_RET
    ex;
  int
    res;

  ex = SOC_SAND_OK;
  res = SOC_SAND_OK;

  soc_sand_initialize_error_word(SOC_SAND_MEM_WRITE, 0, &err);

  soc_sand_check_driver_and_device(unit, &err);

  ex = soc_sand_get_error_code_from_error_word(err);

  if (ex != SOC_SAND_OK)
  {
    err = 1;
    goto exit;
  }

  res = soc_sand_take_chip_descriptor_mutex(unit) ;

  if (res)
  {
    if (0 < res)
    {
      ex = SOC_SAND_SEM_TAKE_FAIL ;
      err = 2 ;
      goto exit ;
    }
    if (0 > res)
    {
      ex = SOC_SAND_ILLEGAL_DEVICE_ID ;
      err = 3 ;
      goto exit ;
    }
  }
  

   ex = soc_sand_mem_write_unsafe(
          unit,
          result_ptr,
          offset,
          size,
          indirect
        );


   if (ex!=SOC_SAND_OK)
   {
      ex = SOC_SAND_MEM_WRITE_010 ;
      goto exit_semaphore;
   }

exit_semaphore:
  if (SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(unit))
  {
    ex = SOC_SAND_SEM_GIVE_FAIL ;
    err = 5;
    goto exit ;
  }

exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_MEM_WRITE,
        "General error in soc_sand_mem_write()",err, offset, size, indirect, 0,0) ;

  return ex ;
}




SOC_SAND_RET
  soc_sand_mem_write_unsafe(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32 *result_ptr,
    SOC_SAND_IN uint32 offset,
    SOC_SAND_IN uint32  size,
    SOC_SAND_IN uint32  indirect
  )
{

  uint32 *base_address, err;
  SOC_SAND_RET ex;


  
  ex = SOC_SAND_OK;

  soc_sand_initialize_error_word(SOC_SAND_MEM_WRITE_UNSAFE, 0, &err);
  ex = soc_sand_get_error_code_from_error_word(err);
  if (ex != SOC_SAND_OK)
  {
    err = 1;
    goto exit;
  }
  
  if (FALSE == indirect)
  {    
    
    if (!soc_sand_is_long_aligned(offset))
    {
      ex = SOC_SAND_MEM_WRITE_US_001 ;
      err = 2 ;
      goto exit ;
    }
    
    if( soc_sand_mem_check_read_write_protect_get() &&
        soc_sand_chip_descriptor_get_is_read_write_protect_func(unit)
      )
    {
      if ( soc_sand_chip_descriptor_get_is_read_write_protect_func(unit)(
             FALSE , offset, size
           )
         )
      {
        ex = SOC_SAND_MEM_WRITE_US_003;
        err = 3;
        goto exit;
      }
    }
    
    if (
     (offset <= soc_sand_mem_interrupt_mask_address_get(unit)) &&
         ((offset + (size>>2)) > (soc_sand_mem_interrupt_mask_address_get(unit)))
       )
    {
      ex = SOC_SAND_MEM_WRITE_US_003;
      err = 4;
      goto exit;
    }
  }
  

  base_address = soc_sand_get_chip_descriptor_base_addr(unit) ;

  if (indirect)
  {
    ex = soc_sand_indirect_write_to_chip(
           unit,
           result_ptr,
           offset,
           size,
           SOC_SAND_IND_GET_MODULE_BIT_FROM_OFFSET
         );


    if(SOC_SAND_OK != ex)
    {
      ex = SOC_SAND_MEM_WRITE_US_005;
      err = 5;
      goto exit ;
    }
  }
  else
  {
    ex = soc_sand_physical_write_to_chip(
          result_ptr,
          base_address,
          offset,
          size
         );

    if(SOC_SAND_OK != ex)
    {
      ex = SOC_SAND_MEM_WRITE_US_006 ;
      err = 6;
      goto exit ;
    }
  }
  
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_MEM_WRITE_UNSAFE,
        "General error in soc_sand_mem_write_unsafe()",err, offset, size, indirect, 0,0) ;

  return ex ;
}





SOC_SAND_RET
  soc_sand_read_field(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32 offset,
    SOC_SAND_IN  uint32 shift,
    SOC_SAND_IN  uint32 mask,
    SOC_SAND_OUT uint32* data
  )
{
  SOC_SAND_RET
    soc_sand_ret;
  uint32
    reg_val,
    exit_point;

  
  soc_sand_ret = SOC_SAND_OK;
  exit_point = 0;

  if(NULL == data)
  {
    soc_sand_ret = SOC_SAND_NULL_POINTER_ERR;
    exit_point = 10;
    goto exit;
  }

  
  if (SOC_SAND_OK != soc_sand_take_chip_descriptor_mutex(unit) )
  {
    soc_sand_ret = SOC_SAND_SEM_TAKE_FAIL;
    exit_point = 20;
    goto exit;
  }

  
  soc_sand_ret = soc_sand_mem_read(unit,
                           &reg_val,
                           offset,
                           sizeof(uint32),
                           FALSE) ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    exit_point = 30;
    goto exit_semaphore;
  }

  
  *data = SOC_SAND_GET_FLD_FROM_PLACE(reg_val, shift, mask) ;

exit_semaphore:
  
  if (SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(unit) )
  {
    soc_sand_ret = SOC_SAND_SEM_GIVE_FAIL;
    goto exit;
  }
exit:
  SOC_SAND_ERROR_REPORT(soc_sand_ret,NULL,0,0,SOC_SAND_READ_FIELD,
        "General error in soc_sand_read_field()",exit_point, unit, offset, shift, 0,0) ;
  return soc_sand_ret;
}



SOC_SAND_RET
  soc_sand_read_field_int(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32 offset,
    SOC_SAND_IN  uint32 shift,
    SOC_SAND_IN  uint32 mask,
    SOC_SAND_OUT uint32* data
  )
{
  SOC_SAND_RET
    soc_sand_ret;
  uint32
    field;
  soc_sand_ret = soc_sand_read_field(
    unit,
    offset,
    shift,
    mask,
    &field
  );
  *data = (uint32)field;
  return soc_sand_ret;
}


SOC_SAND_RET
  soc_sand_read_field_ex(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32 offset,
    SOC_SAND_IN  uint32 shift,
    SOC_SAND_IN  uint32 mask,
    SOC_SAND_IN  uint32  indirect,
    SOC_SAND_OUT uint32* data
  )
{
  SOC_SAND_RET
    soc_sand_ret;
  uint32
    reg_val,
    exit_point;

  
  soc_sand_ret = SOC_SAND_OK;
  exit_point = 0;

  if(NULL == data)
  {
    soc_sand_ret = SOC_SAND_NULL_POINTER_ERR;
    exit_point = 10;
    goto exit;
  }

  
  if (SOC_SAND_OK != soc_sand_take_chip_descriptor_mutex(unit) )
  {
    soc_sand_ret = SOC_SAND_SEM_TAKE_FAIL;
    exit_point = 20;
    goto exit;
  }

  
  soc_sand_ret = soc_sand_mem_read(unit,
                           &reg_val,
                           offset,
                           sizeof(uint32),
                           indirect) ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    exit_point = 30;
    goto exit_semaphore;
  }

  
  *data = SOC_SAND_GET_FLD_FROM_PLACE(reg_val, shift, mask) ;

exit_semaphore:
  
  if (SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(unit) )
  {
    soc_sand_ret = SOC_SAND_SEM_GIVE_FAIL;
    goto exit;
  }
exit:
  SOC_SAND_ERROR_REPORT(soc_sand_ret,NULL,0,0,SOC_SAND_READ_FIELD,
        "General error in soc_sand_read_field_ex()",exit_point, unit, offset, shift, 0,0) ;
  return soc_sand_ret;
}


SOC_SAND_RET
  soc_sand_write_field(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32 offset,
    SOC_SAND_IN  uint32 shift,
    SOC_SAND_IN  uint32 mask,
    SOC_SAND_IN  uint32 data_to_write
  )
{
  SOC_SAND_RET
    soc_sand_ret;
  uint32
    exit_point;

  
  soc_sand_ret = SOC_SAND_OK;
  exit_point = 0;


  
  if (SOC_SAND_OK != soc_sand_take_chip_descriptor_mutex(unit) )
  {
    soc_sand_ret = SOC_SAND_SEM_TAKE_FAIL;
    exit_point = 20;
    goto exit;
  }

  soc_sand_ret = soc_sand_read_modify_write(
               soc_sand_get_chip_descriptor_base_addr(unit),
               offset,
               shift,
               mask,
               data_to_write
             );
  if (soc_sand_ret != SOC_SAND_OK)
  {
    exit_point = 40;
    goto exit_semaphore;
  }

exit_semaphore:
  
  if (SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(unit) )
  {
    soc_sand_ret = SOC_SAND_SEM_GIVE_FAIL;
    goto exit;
  }
exit:
  SOC_SAND_ERROR_REPORT(soc_sand_ret,NULL,0,0,SOC_SAND_WRITE_FIELD,
        "General error in soc_sand_read_field()",exit_point, unit, offset, shift, 0,0) ;
  return soc_sand_ret;
}


SOC_SAND_RET
  soc_sand_write_field_ex(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32 offset,
    SOC_SAND_IN  uint32 shift,
    SOC_SAND_IN  uint32 mask,
    SOC_SAND_IN  uint32  indirect,
    SOC_SAND_IN  uint32 data_to_write
  )
{
  SOC_SAND_RET
    soc_sand_ret;
  uint32
    exit_point;

  
  soc_sand_ret = SOC_SAND_OK;
  exit_point = 0;


  
  if (SOC_SAND_OK != soc_sand_take_chip_descriptor_mutex(unit) )
  {
    soc_sand_ret = SOC_SAND_SEM_TAKE_FAIL;
    exit_point = 20;
    goto exit;
  }

  if (!indirect)
  {
    soc_sand_ret = soc_sand_read_modify_write(
                 soc_sand_get_chip_descriptor_base_addr(unit),
                 offset,
                 shift,
                 mask,
                 data_to_write
               );
    if (soc_sand_ret != SOC_SAND_OK)
    {
      exit_point = 30;
      goto exit_semaphore;
    }
  }
  else
  {
    soc_sand_ret = soc_sand_indirect_read_modify_write(
                 unit,
                 offset,
                 shift,
                 mask,
                 data_to_write
                 );
    if (soc_sand_ret != SOC_SAND_OK)
    {
      exit_point = 40;
      goto exit_semaphore;
    }
  }

exit_semaphore:
  
  if (SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(unit) )
  {
    soc_sand_ret = SOC_SAND_SEM_GIVE_FAIL;
    goto exit;
  }
exit:
  SOC_SAND_ERROR_REPORT(soc_sand_ret,NULL,0,0,SOC_SAND_WRITE_FIELD,
        "General error in soc_sand_write_field_ex()",exit_point, unit, offset, shift, 0,0) ;
  return soc_sand_ret;
}






uint32
  Soc_sand_mem_check_read_write_protect = TRUE;


uint32
  soc_sand_mem_check_read_write_protect_get(
    void
  )
{
  uint32
    read_write_protect;
  read_write_protect = Soc_sand_mem_check_read_write_protect;
  return read_write_protect;
}


void
  soc_sand_mem_check_read_write_protect_set(
    SOC_SAND_IN uint32 read_write_protect
  )
{
  Soc_sand_mem_check_read_write_protect = read_write_protect;
}


#if SOC_SAND_DEBUG


SOC_SAND_RET
  soc_sand_print_registers_array(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32 address_array[SOC_SAND_ADDRESS_ARRAY_MAX_SIZE],
    SOC_SAND_IN uint32 nof_address,
    SOC_SAND_IN uint32 print_options_bm
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    address_i,
    bit_i;
  uint32
    address,
    data;

  if(nof_address > SOC_SAND_ADDRESS_ARRAY_MAX_SIZE)
  {
    LOG_CLI((BSL_META_U(unit,
                        "nof_address > SOC_SAND_ADDRESS_ARRAY_MAX_SIZE\n\r")));
    ret = SOC_SAND_ERR;
    goto exit;
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\r\n\r")));
  for(address_i = 0;address_i<nof_address;address_i++)
  {
    address = address_array[address_i];
    soc_sand_mem_read(unit, &data, address*4, 4, FALSE);

    if((data != 0)                                ||
       !(print_options_bm & SOC_SAND_DONT_PRINT_ZEROS)
      )
    {
      LOG_CLI((BSL_META_U(unit,
                          "Address: 0x%x. Data: 0x%x"),
               address,
               data
               ));
      if(print_options_bm & SOC_SAND_PRINT_BITS)
      {
        LOG_CLI((BSL_META_U(unit,
                            " Bits:")));

        for(bit_i=0;bit_i<32;bit_i++)
        {
          if(data & SOC_SAND_BIT(bit_i))
          {
            LOG_CLI((BSL_META_U(unit,
                                "%d,"),bit_i));
          }
        }
      }
      LOG_CLI((BSL_META_U(unit,
                          ".\n\r")));
    }
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\r\n\r")));


exit:
  return ret;
}




SOC_SAND_RET
  soc_sand_print_block(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32 address_array[SOC_SAND_ADDRESS_ARRAY_MAX_SIZE],
    SOC_SAND_IN uint32 nof_address,
    SOC_SAND_IN uint32 print_options_bm
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    address_i,
    bit_i,
    same_as_prev = FALSE;
  uint32
    address,
    data;

  if(nof_address > SOC_SAND_ADDRESS_ARRAY_MAX_SIZE)
  {
    LOG_CLI((BSL_META_U(unit,
                        "nof_address > SOC_SAND_ADDRESS_ARRAY_MAX_SIZE\n\r")));
    ret = SOC_SAND_ERR;
    goto exit;
  }
  for(address_i = 0;address_i<nof_address;address_i++)
  {
    address = address_i ;
    data    = address_array[address_i];

    if(print_options_bm & SOC_SAND_PRINT_RANGES)
    {
      
      
      if((address_i + 1 == nof_address) ||
         (data != address_array[address_i + 1])
        )
      {
        same_as_prev = FALSE;
      }
      else
      {
        if(!same_as_prev)
        {
          same_as_prev = TRUE;
          if((data != 0)                                ||
             !(print_options_bm & SOC_SAND_DONT_PRINT_ZEROS)
            )
          {
            LOG_CLI((BSL_META_U(unit,
                                "  From 0x%04X To "),
                     address + SOC_SAND_GET_BITS_RANGE(print_options_bm, 
                     SOC_SAND_PRINT_START_ADDR_MSB, SOC_SAND_PRINT_START_ADDR_LSB)
                     ));
          }
        }
        if(address_i != (nof_address -1))
        {
          continue;
        }
      }
    }
    if((data != 0)                                      ||
       !(print_options_bm & SOC_SAND_DONT_PRINT_ZEROS)
      )
    {
      LOG_CLI((BSL_META_U(unit,
                          "  0x%04X: 0x%08X"),
               address + SOC_SAND_GET_BITS_RANGE(print_options_bm, 
               SOC_SAND_PRINT_START_ADDR_MSB, SOC_SAND_PRINT_START_ADDR_LSB),
               data
               ));
      if(print_options_bm & SOC_SAND_PRINT_BITS)
      {
        LOG_CLI((BSL_META_U(unit,
                            " Bits:")));

        for(bit_i=0;bit_i<32;bit_i++)
        {
          if(data & SOC_SAND_BIT(bit_i))
          {
            LOG_CLI((BSL_META_U(unit,
                                "%d,"),bit_i));
          }
        }
      }
      LOG_CLI((BSL_META_U(unit,
                          "\n\r")));
    }
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));


exit:
  return ret;
}





SOC_SAND_RET
  soc_sand_print_indirect_tables(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN SOC_SAND_INDIRECT_PRINT_INFO print_info[SOC_SAND_MAX_NOF_INDIRECT_TABLES],
    SOC_SAND_IN uint32            print_options_bm
  )
{
  uint32
    *regs_values = NULL,
    max_table_size=0,
    table_i;
  const char*
    proc_name = "soc_sand_print_indirect_tables";
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;


  for(table_i=0;print_info[table_i].addr != (uint32)-1;table_i++)
  {
    if(max_table_size < print_info[table_i].size * 4)
    {
      max_table_size = print_info[table_i].size * 4;
    }
  }
  regs_values = soc_sand_os_malloc(max_table_size,"regs_values print_indirect_tables");
  if(regs_values == NULL)
  {
    LOG_CLI((BSL_META_U(unit,
                        "%s - Failed to allocate memory: %x\n\r"), proc_name, max_table_size / 4));
    soc_sand_ret = SOC_SAND_MALLOC_FAIL;
    goto exit;
  }

  for(table_i=0;print_info[table_i].addr != (uint32)-1;table_i++)
  {
    soc_sand_ret = soc_sand_mem_read(
                   unit,
                   regs_values,
                   print_info[table_i].addr,
                   print_info[table_i].size*4,
                   TRUE
                 );
    if(soc_sand_ret != SOC_SAND_OK) {
        return soc_sand_ret;
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s (0x%08x):\n\r"),
             print_info[table_i].name,
             print_info[table_i].addr
             ));
    {

      soc_sand_print_block(
        unit,
        regs_values,
        print_info[table_i].size,
        print_options_bm
      );
    }

  }
exit:
  if(NULL != regs_values)
  {
    soc_sand_os_free(regs_values);
    regs_values = NULL;
  }
  return soc_sand_ret;
}
#endif


/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/




#include <soc/dpp/SAND/SAND_FM/sand_mem_access_callback.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>



uint32
  soc_sand_mem_read_callback(
    uint32 *buffer,
    uint32 size
  )
{
  SOC_SAND_MEM_READ_CALLBACK_STRUCT  *callback_struct;
  uint32 *new_buf;
  uint32 ex, no_err;
  int res;
  SOC_SAND_RET soc_sand_ret;

  ex = 0;
  no_err = ex;
  new_buf = NULL;
  callback_struct = NULL;
  soc_sand_ret = SOC_SAND_OK;
  
  if (sizeof(SOC_SAND_MEM_READ_CALLBACK_STRUCT) != size)
  {
    soc_sand_set_error_code_into_error_word(SOC_SAND_ERR_8001,&ex);
    soc_sand_set_severity_into_error_word  (1, &ex);
    goto exit;
  }
  
  callback_struct = (SOC_SAND_MEM_READ_CALLBACK_STRUCT  *)buffer;

  res = soc_sand_take_chip_descriptor_mutex(callback_struct->unit);
  if ( SOC_SAND_OK != res )
  {
    if (SOC_SAND_ERR == res)
    {
      soc_sand_set_error_code_into_error_word(SOC_SAND_SEM_TAKE_FAIL,  &ex);
      goto exit;
    }
    if ( 0 > res )
    {
      soc_sand_set_error_code_into_error_word(SOC_SAND_ILLEGAL_DEVICE_ID,  &ex);
      goto exit;
    }
  }
  
  soc_sand_ret = soc_sand_mem_read(
               callback_struct->unit,
               callback_struct->result_ptr,
               callback_struct->offset,
               callback_struct->size,
               callback_struct->indirect
             );
  soc_sand_set_error_code_into_error_word(soc_sand_ret, &ex);

  
  if ( SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(callback_struct->unit) )
  {
    soc_sand_set_error_code_into_error_word(SOC_SAND_SEM_TAKE_FAIL,&ex);
    goto exit;
  }
  
  if ( (ex != no_err) ||
      !(callback_struct->copy_of_result_is_valid) ||
       (callback_struct->copy_of_result_is_valid &&
        soc_sand_os_memcmp(
          callback_struct->copy_of_last_result_ptr,
          callback_struct->result_ptr,
          SOC_SAND_MIN(callback_struct->size, SOC_SAND_CALLBACK_BUF_SIZE)
       ))
     )
  {
    if (callback_struct->user_callback_proc)
    {
      res = callback_struct->user_callback_proc(
                                callback_struct->user_callback_id,
                                callback_struct->result_ptr,
                                &new_buf,
                                callback_struct->size,
                                ex,
                                callback_struct->unit,
                                callback_struct->offset,
                                callback_struct->soc_sand_tcm_callback_id,
                                0
                             );
      soc_sand_os_memcpy(
        callback_struct->copy_of_last_result_ptr,
        callback_struct->result_ptr,
        SOC_SAND_MIN(callback_struct->size, SOC_SAND_CALLBACK_BUF_SIZE)
      );
      callback_struct->copy_of_result_is_valid = TRUE;
      if (new_buf)
      {
        callback_struct->result_ptr =  new_buf;
      }
      
      if (SOC_SAND_OK != soc_sand_get_error_code_from_error_word(res))
      {
        soc_sand_set_error_code_into_error_word(SOC_SAND_ERR_8006,  &ex);
        callback_struct->copy_of_result_is_valid = FALSE;
        goto exit;
      }
    }
  }
  
exit:
  if (ex != no_err)
  {
    soc_sand_error_handler(ex, "error in soc_sand_mem_read_callback()", 0,0,0,0,0,0);
  }
  else
  {
  }
  return ex;
}


/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/


#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>


static
  SOC_SAND_ERROR_HANDLER_PTR
    Soc_sand_supplied_error_handler      = NULL ;

  char  *Soc_sand_supplied_error_buffer  = NULL ;

  uint32
    Soc_sand_supplied_error_handler_is_on = FALSE;

SOC_SAND_RET
  soc_sand_set_user_error_state(
    uint32 onFlag
  )
{
  SOC_SAND_RET
    ex=SOC_SAND_OK ;
  uint32
    err=0 ;

  if(!Soc_sand_supplied_error_handler && onFlag)
  {
    ex = SOC_SAND_ERR;
    goto exit;
  }

  Soc_sand_supplied_error_handler_is_on = onFlag;

exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_SET_USER_ERROR_HANDLER,
        "General error in soc_sand_set_user_error_handler()",err,0,0,0,0,0) ;
  return ex;
}


SOC_SAND_RET
  soc_sand_set_user_error_handler(
    SOC_SAND_ERROR_HANDLER_PTR  user_error_handler,
    char                    *user_error_buffer
  )
{
  SOC_SAND_RET
    ex ;
  uint32
    err ;
  ex = SOC_SAND_OK ;
  err = 0 ;
  Soc_sand_supplied_error_handler = user_error_handler ;
  Soc_sand_supplied_error_buffer  = user_error_buffer ;
  Soc_sand_supplied_error_handler_is_on = (Soc_sand_supplied_error_handler != NULL);
  if (Soc_sand_supplied_error_handler_is_on)
  {
    ex = soc_sand_init_errors_queue() ;
    if (ex != SOC_SAND_OK)
    {
      err = 1 ;
      goto exit ;
    }
  }
  else
  {
    
    ex = soc_sand_delete_errors_queue() ;
    if (ex != SOC_SAND_OK)
    {
      err = 2 ;
      goto exit ;
    }
  }
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_SET_USER_ERROR_HANDLER,
        "General error in soc_sand_set_user_error_handler()",err,0,0,0,0,0) ;
  return (ex) ;
}


  uint32
      Soc_sand_errors_msg_queue_flagged = FALSE ;
void
  soc_set_errors_q_flag(
    void
  )
{
  Soc_sand_errors_msg_queue_flagged = TRUE ;
  return ;
}
uint32
    soc_is_errors_q_flag_set(
      void
    )
{
  return (Soc_sand_errors_msg_queue_flagged) ;
}
void
  soc_reset_errors_q_flag(
    void
  )
{
  Soc_sand_errors_msg_queue_flagged = FALSE ;
  return ;
}

static
  void
    *Soc_sand_errors_msg_queue = (void *)0 ;

SOC_SAND_RET
  soc_sand_init_errors_queue(
    void
  )
{
  SOC_SAND_RET
    ex ;
  uint32
    err ;
  ex = SOC_SAND_OK ;
  err = 0 ;
  
  Soc_sand_errors_msg_queue =
    soc_sand_os_msg_q_create(
            ERRORS_MSG_QUEUE_NUM_MSGS,SOC_SAND_ERRORS_MSG_QUEUE_SIZE_MSG) ;
  if (!Soc_sand_errors_msg_queue)
  {
    ex = SOC_SAND_ERR ;
    err = 1 ;
    goto exit ;
  }
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_INIT_ERRORS_QUEUE,
        "General error in soc_sand_init_errors_queue()",err,0,0,0,0,0) ;
  return (ex) ;
}

SOC_SAND_RET
  soc_sand_delete_errors_queue(
    void
  )
{
  SOC_SAND_RET
    ex ;
  uint32
    err ;
  ex = SOC_SAND_OK ;
  err = 0 ;
  
  if (Soc_sand_errors_msg_queue)
  {
    ex = soc_sand_os_msg_q_delete(Soc_sand_errors_msg_queue) ;
    Soc_sand_errors_msg_queue = (void *)0 ;
    if (ex != SOC_SAND_OK)
    {
      err = 1 ;
      goto exit ;
    }
  }
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_DELETE_ERRORS_QUEUE,
        "General error in soc_sand_delete_errors_queue()",err,0,0,0,0,0) ;
  return (ex) ;
}

int
  soc_sand_is_errors_queue_msg(
    uint32 msg
  )
{
  int
    ret ;
  if (msg == SOC_SAND_NEW_ERROR_MESSAGE)
  {
    ret = 1 ;
  }
  else
  {
    ret = 0 ;
  }
  return (ret) ;
}

SOC_SAND_RET
  soc_sand_load_errors_queue(
    SOC_SAND_ERRORS_QUEUE_MESSAGE *errors_queue_message
  )
{
  SOC_SAND_RET
    ex;
  uint32
      err_x,
      err =0;
  int
    num_in_q ;
  ex = SOC_SAND_OK ;
  err = 0 ;
  soc_sand_initialize_error_word(SOC_SAND_LOAD_ERRORS_QUEUE,0,&err_x) ;
  if (Soc_sand_errors_msg_queue)
  {
    num_in_q = soc_sand_os_msg_q_num_msgs(Soc_sand_errors_msg_queue) ;
    if (num_in_q >= ERRORS_MSG_QUEUE_NUM_MSGS)
    {
      goto exit ;
    }
    ex =
      soc_sand_os_msg_q_send(
        Soc_sand_errors_msg_queue,(char *)errors_queue_message,
        sizeof(*errors_queue_message),SOC_SAND_NO_TIMEOUT,0) ;
    if (ex != SOC_SAND_OK)
    {
      err = 1 ;
      goto exit ;
    }
    if (!soc_is_errors_q_flag_set())
    {
      uint32
          msg ;
      soc_set_errors_q_flag() ;
      msg = SOC_SAND_NEW_ERROR_MESSAGE ;
      ex = soc_sand_tcm_send_message_to_q_from_task(msg) ;
      if (ex != SOC_SAND_OK)
      {
        err = 2 ;
        goto exit ;
      }
    }
  }
exit:





  if(err)
  {
    soc_sand_set_error_code_into_error_word(ex,&err_x) ;
    soc_sand_invoke_user_error_handler(
      err_x,"error in soc_sand_load_errors_queue",err,0,0,0,0,0);
  }
  return (ex) ;
}

SOC_SAND_RET
  soc_sand_unload_errors_queue(
    void
  )
{
  SOC_SAND_RET
    ex ;
  uint32
      err ;
  int
    mesg_received,
    num_in_q ;
  SOC_SAND_ERRORS_QUEUE_MESSAGE
    errors_queue_message ;
  ex = SOC_SAND_OK ;
  err = 0 ;
  if (Soc_sand_errors_msg_queue)
  {
    soc_reset_errors_q_flag() ;
    while (TRUE)
    {
      num_in_q = soc_sand_os_msg_q_num_msgs(Soc_sand_errors_msg_queue) ;
      if (num_in_q == 0)
      {
        goto exit ;
      }
      else if (num_in_q < 0)
      {
        ex = SOC_SAND_ERR ;
        err = 1 ;
        goto exit ;
      }
      mesg_received =
        soc_sand_os_msg_q_recv(
          Soc_sand_errors_msg_queue,
          (unsigned char *)&errors_queue_message,
          sizeof(errors_queue_message),
          (long)SOC_SAND_NO_TIMEOUT) ;

      ex = (mesg_received == 0)?SOC_SAND_OK:SOC_SAND_ERR;

      if (ex != SOC_SAND_OK)
      {
        err = 2 ;
        goto exit ;
      }
      ex =
        soc_sand_invoke_user_error_handler(
          errors_queue_message.err_id,
          errors_queue_message.error_txt,
          errors_queue_message. param_01,
          errors_queue_message.param_02,
          errors_queue_message.param_03,
          errors_queue_message.param_04,
          errors_queue_message.param_05,
          errors_queue_message.param_06
          ) ;
      if (ex != SOC_SAND_OK)
      {
        err = 3 ;
        goto exit ;
      }
    }
  }
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_UNLOAD_ERRORS_QUEUE,
        "General error in soc_sand_unload_errors_queue()",err,0,0,0,0,0) ;
  return (ex) ;
}



SOC_SAND_RET
  soc_sand_error_handler(
    uint32 err_id,
    const char    *error_txt,
    uint32 param_01,
    uint32 param_02,
    uint32 param_03,
    uint32 param_04,
    uint32 param_05,
    uint32 param_06
  )
{
  SOC_SAND_RET
    ex ;
#if ( SOC_SAND_ALLOW_DRIVER_TO_PRINT_ERRORS)  
  int
    ret;
#endif
  SOC_SAND_ERRORS_QUEUE_MESSAGE
    *errors_queue_message = NULL;
    ex = SOC_SAND_OK ;

#if ( SOC_SAND_ALLOW_DRIVER_TO_PRINT_ERRORS)
    if (!(soc_sand_no_error_printing_get()))
    {
      ret = soc_sand_general_display_err(err_id, error_txt);
      ex = (uint16)ret;
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("exit place: %d, params: %d %d %d\n\r"),param_01,param_02,param_03,param_04));
    }
#endif
  if (Soc_sand_supplied_error_handler_is_on)
  {
    uint32
        size ;
    errors_queue_message = sal_alloc(sizeof(SOC_SAND_ERRORS_QUEUE_MESSAGE), "soc_sand_error_handler.errors_queue_message");
    if(errors_queue_message == NULL) {
        ex = 1;
        goto exit; 
    }
    errors_queue_message->err_id = err_id ;
    size = sizeof(errors_queue_message->error_txt) ;
    soc_sand_os_strncpy(
      errors_queue_message->error_txt,error_txt,size) ;
    errors_queue_message->error_txt[size - 1] = 0 ;
    errors_queue_message->param_01 = param_01 ;
    errors_queue_message->param_02 = param_02 ;
    errors_queue_message->param_03 = param_03 ;
    errors_queue_message->param_04 = param_04 ;
    errors_queue_message->param_05 = param_05 ;
    errors_queue_message->param_06 = param_06 ;
    ex = soc_sand_load_errors_queue(errors_queue_message) ;
    if (ex != SOC_SAND_OK)
    {
      goto exit ;
    }
  }
exit:
  if(errors_queue_message) {
    sal_free(errors_queue_message);
  }




  return (ex) ;
}

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
  )
{
  SOC_SAND_RET
    ex ;
  char* new_err_description ;

  ex = SOC_SAND_OK ;
  if (Soc_sand_supplied_error_handler)
  {
    new_err_description = NULL ;
    if (Soc_sand_supplied_error_buffer)
    {
      soc_sand_os_strncpy(Soc_sand_supplied_error_buffer,error_txt,SOC_SAND_CALLBACK_BUF_SIZE) ;
      Soc_sand_supplied_error_buffer[SOC_SAND_CALLBACK_BUF_SIZE - 1] = 0 ;
    }
    ex = (SOC_SAND_RET)Soc_sand_supplied_error_handler(
            err_id,
            Soc_sand_supplied_error_buffer,
            &new_err_description,
            param_01,
            param_02,
            param_03,
            param_04,
            param_05,
            param_06
          ) ;
    if (new_err_description)
    {
      Soc_sand_supplied_error_buffer = new_err_description ;
    }
  }
  return ex ;
}

int
  soc_sand_is_long_aligned (
    uint32 word_to_check
  )
{
  if (word_to_check & SOC_SAND_UINT32_ALIGN_MASK)
  {
    return FALSE ;
  }
  return TRUE ;
}

void
  soc_sand_check_driver_and_device(
    int  unit,
    uint32 *error_word
  )
{
  if ( !soc_sand_general_get_driver_is_started() )
  {
    soc_sand_set_error_code_into_error_word(SOC_SAND_DRIVER_NOT_STARTED, error_word) ;
    goto exit ;
  }
  
  if ( !soc_sand_is_chip_descriptor_valid(unit) )
  {
    soc_sand_set_error_code_into_error_word(SOC_SAND_ILLEGAL_DEVICE_ID, error_word) ;
    goto exit ;
  }
  
exit:
  return ;
}

uint32
  soc_sand_get_index_of_max_member_in_array(
    SOC_SAND_IN     uint32                     array[],
    SOC_SAND_IN     uint32                    len
  )
{
  uint32
      index,
      index_of_max,
      max_val;

  index_of_max = 0;
  max_val = 0;

  for (index = 0; index < len; index++)
  {
    if (array[index] > max_val)
    {
      max_val = array[index];
      index_of_max = index;
    }
  }
  return index_of_max;
}

#if SOC_SAND_DEBUG



int
  soc_sand_general_display_err(
    uint32 err_id,
    const char    *error_txt
  )
{
  int
    ret=0;
  uint32
    module_id;
  module_id = SOC_SAND_GET_FLD_FROM_PLACE(err_id,SOC_SAND_MODULE_ID_SHIFT, SOC_SAND_MODULE_ID_MASK);
  switch(module_id)
  {
  case SOC_SAND_MODULE_IDENTIFIER:
    ret = soc_sand_disp_result(err_id);
    break;
  default:
    LOG_CLI((BSL_META("Failed to parse Error ID 0x%x \n\r"
                      " Error Text %s\n\r"),
             err_id,
             error_txt
             ));
  }
  return ret;
}


const char*
  soc_sand_SAND_OP_to_str(
    SOC_SAND_IN SOC_SAND_OP      soc_sand_op,
    SOC_SAND_IN uint32 short_format
  )
{
  const char
    *str;

  switch(soc_sand_op)
  {
  case SOC_SAND_NOP:
    if(short_format)
    {
      str = "NOP";
    }
    else
    {
      str = "SOC_SAND_NOP";
    }
    break;

  case SOC_SAND_OP_AND:
    if(short_format)
    {
      str = "AND";
    }
    else
    {
      str = "SOC_SAND_OP_AND";
    }
    break;

  case SOC_SAND_OP_OR:
    if(short_format)
    {
      str = "OR";
    }
    else
    {
      str = "SOC_SAND_OP_OR";
    }
    break;

  case SOC_SAND_NOF_SAND_OP:
    if(short_format)
    {
      str = "NOF_OP";
    }
    else
    {
      str = "SOC_SAND_NOF_SAND_OP";
    }
    break;

  default:
    str = "soc_sand_SAND_OP_to_str input parameters error (soc_sand_op)";
  }

  return str;
}



void
  soc_sand_print_hex_buff(
    SOC_SAND_IN char*        buff,
    SOC_SAND_IN uint32 buff_byte_size,
    SOC_SAND_IN uint32 nof_bytes_per_line
  )
{
  uint32
    byte_i;

  if(NULL == buff)
  {
    goto exit;
  }

  for (byte_i=0; byte_i<buff_byte_size; byte_i++)
  {
    if( (byte_i != 0)  &&
        (byte_i%4 == 0) &&
        ((byte_i%nof_bytes_per_line) != 0)
      )
    {
      LOG_CLI((BSL_META(" ")));
    }
    if( (byte_i != 0)  &&
        ((byte_i%nof_bytes_per_line) == 0)
      )
    {
      LOG_CLI((BSL_META("\n\r")));
    }
    if((byte_i%nof_bytes_per_line) == 0)
    {
      LOG_CLI((BSL_META("%3u-%3u:"),
               byte_i,
               SOC_SAND_MIN(byte_i + (nof_bytes_per_line-1), buff_byte_size-1)
               ));
    }

    LOG_CLI((BSL_META("%02X"), (buff[byte_i]&0xFF)));
  }
  LOG_CLI((BSL_META("\n\r")));

exit:
  return;
}


void
  soc_sand_print_bandwidth(
    SOC_SAND_IN uint32 bw_kbps,
    SOC_SAND_IN uint32  short_format
  )
{
  uint32
    tmp;

  tmp = bw_kbps;
  tmp /= 1000;

  LOG_CLI((BSL_META("%u Kbps,  %u.%03u Gbps"),
           bw_kbps,
           tmp/1000,
           tmp%1000
           ));

  if(!short_format)
  {
    LOG_CLI((BSL_META("\n\r")));
  }

  return;
}
#endif


SOC_SAND_RET
  soc_sand_set_field(
    SOC_SAND_INOUT  uint32    *reg_val,
    SOC_SAND_IN  uint32       ms_bit,
    SOC_SAND_IN  uint32       ls_bit,
    SOC_SAND_IN  uint32       field_val
  )
{
  uint32
    tmp_reg;
  SOC_SAND_RET
    soc_sand_ret;

  soc_sand_ret = SOC_SAND_OK;
  tmp_reg = *reg_val;

  
  if (ms_bit-ls_bit+1 > 32)
  {
    soc_sand_ret = SOC_SAND_BIT_STREAM_FIELD_SET_SIZE_RANGE_ERR;
    goto exit;
  }

  tmp_reg &= SOC_SAND_ZERO_BITS_MASK(ms_bit,ls_bit);

  tmp_reg |= SOC_SAND_SET_BITS_RANGE(
              field_val,
              ms_bit,
              ls_bit
            );

  *reg_val = tmp_reg;

exit:
  return soc_sand_ret;

}

SOC_SAND_RET
  soc_sand_U8_to_U32(
    SOC_SAND_IN uint8     *u8_val,
    SOC_SAND_IN uint32    nof_bytes,
    SOC_SAND_OUT uint32   *u32_val
  )
{
  uint32
    u8_indx,
    cur_u8_indx,
    u32_indx;
  SOC_SAND_IN uint8
    *cur_u8;
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

  if (!u8_val || !u32_val)
  {
    soc_sand_ret = SOC_SAND_ERR ;
    goto exit ;
  }

  cur_u8_indx = 0;
  u32_indx = 0;

  for ( cur_u8 = u8_val, u8_indx = 0; u8_indx < nof_bytes; ++u8_indx, ++cur_u8)
  {
     soc_sand_set_field(
       &(u32_val[u32_indx]),
       (cur_u8_indx + 1) * SOC_SAND_NOF_BITS_IN_BYTE - 1,
       cur_u8_indx * SOC_SAND_NOF_BITS_IN_BYTE,
       *cur_u8
     );

    cur_u8_indx++;
    if (cur_u8_indx >= sizeof(uint32))
    {
      cur_u8_indx = 0;
      ++u32_indx;
    }
  }
exit:
  return soc_sand_ret;
}

SOC_SAND_RET
  soc_sand_U32_to_U8(
    SOC_SAND_IN uint32  *u32_val,
    SOC_SAND_IN uint32  nof_bytes,
    SOC_SAND_OUT uint8  *u8_val
  )
{
  uint32
    u8_indx,
    cur_u8_indx;
  SOC_SAND_IN uint32
    *cur_u32;

  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

  if (!u8_val || !u32_val)
  {
    soc_sand_ret = SOC_SAND_ERR ;
    goto exit ;
  }

  cur_u8_indx = 0;
  for ( cur_u32 = u32_val, u8_indx = 0; u8_indx < nof_bytes; ++u8_indx)
  {
    u8_val[u8_indx] = (uint8)
      SOC_SAND_GET_BITS_RANGE(
        *cur_u32,
        (cur_u8_indx + 1) * SOC_SAND_NOF_BITS_IN_BYTE - 1,
        cur_u8_indx * SOC_SAND_NOF_BITS_IN_BYTE
       );

    ++cur_u8_indx;
    if (cur_u8_indx >= sizeof(uint32))
    {
      cur_u8_indx = 0;
      ++cur_u32;
    }
  }
exit:
  return soc_sand_ret;
}

void
  soc_sand_SAND_U32_RANGE_clear(
    SOC_SAND_OUT SOC_SAND_U32_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_U32_RANGE));
  info->start = 0;
  info->end = 0;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(
    SOC_SAND_OUT SOC_SAND_TABLE_BLOCK_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_TABLE_BLOCK_RANGE));
  info->iter = 0;
  info->entries_to_scan = 0;
  info->entries_to_act = 0;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_SAND_DEBUG

const char*
  soc_sand_SAND_SUCCESS_FAILURE_to_string(
    SOC_SAND_IN  SOC_SAND_SUCCESS_FAILURE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_SAND_SUCCESS:
    str = "SUCCESS";
  break;
  case SOC_SAND_FAILURE_OUT_OF_RESOURCES:
    str = "FAILURE_OUT_OF_RESOURCES";
  break;
  case SOC_SAND_FAILURE_OUT_OF_RESOURCES_2:
    str = "FAILURE_OUT_OF_RESOURCES_2";
  break;
  case SOC_SAND_FAILURE_OUT_OF_RESOURCES_3:
    str = "FAILURE_OUT_OF_RESOURCES_3";
  break;
  case SOC_SAND_FAILURE_REMOVE_ENTRY_FIRST:
    str = "FAILURE_REMOVE_ENTRY_FIRST";
  break;
  case SOC_SAND_FAILURE_INTERNAL_ERR:
    str = "FAILURE_INTERNAL_ERR";
  break;
  case SOC_SAND_FAILURE_UNKNOWN_ERR:
    str = "FAILURE_UNKNOWN_ERR";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

void
  soc_sand_SAND_U32_RANGE_print(
    SOC_SAND_IN  SOC_SAND_U32_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("%u - %u "),info->start, info->end));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_TABLE_BLOCK_RANGE_print(
    SOC_SAND_IN  SOC_SAND_TABLE_BLOCK_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META("iter: %u\n\r"),info->iter));
  LOG_CLI((BSL_META("entries_to_scan: %u\n\r"),info->entries_to_scan));
  LOG_CLI((BSL_META("entries_to_act: %u\n\r"),info->entries_to_act));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}




#endif 


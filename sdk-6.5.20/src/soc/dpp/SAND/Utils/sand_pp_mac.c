/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_pp_mac.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>




























uint32
  soc_sand_pp_mac_address_struct_to_long(
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac_add_struct,
    SOC_SAND_OUT uint32               mac_add_long[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S]
  )
{
  uint32
    tmp;
  uint32
    char_indx,
    long_indx,
    write_to;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_PP_MAC_ADDRESS_STRUCT_TO_LONG);

  write_to = 0;
  mac_add_long[0] = 0;
  mac_add_long[1] = 0;
  long_indx = 0;

  for (char_indx = 0; char_indx<SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++char_indx)
  {
    tmp = mac_add_struct->address[char_indx];
    mac_add_long[long_indx] |=
      SOC_SAND_SET_BITS_RANGE(
            tmp,
            (SOC_SAND_NOF_BITS_IN_CHAR * (write_to + 1) - 1),
            SOC_SAND_NOF_BITS_IN_CHAR * write_to
          );

    ++write_to;

    if (write_to >= sizeof(uint32))
    {
      long_indx = 1;
      write_to = 0;
    }
  }

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pp_mac_address_long_to_struct()",0,0);
}

uint32
  soc_sand_pp_mac_address_long_to_struct(
    SOC_SAND_IN  uint32            mac_add_long[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS    *mac_add_struct
  )
{
  uint32
    char_indx,
    long_indx,
    read_from;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_PP_MAC_ADDRESS_LONG_TO_STRUCT);

  read_from = 0;
  long_indx = 0; 

  for (char_indx = 0; char_indx<SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++char_indx)
  {
    mac_add_struct->address[char_indx] =
      (uint8) SOC_SAND_GET_BITS_RANGE(
        mac_add_long[long_indx],
        SOC_SAND_NOF_BITS_IN_CHAR * (read_from + 1) - 1,
        SOC_SAND_NOF_BITS_IN_CHAR * read_from
      );
    ++read_from;

    if (read_from >= sizeof(uint32))
    {
      long_indx = 1;
      read_from = 0;
    }
  }

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pp_mac_address_long_to_struct()",0,0);
}


uint32
  soc_sand_pp_mac_address_inc(
    SOC_SAND_INOUT  SOC_SAND_PP_MAC_ADDRESS   *mac
  )
{
  SOC_SAND_U64
    mac1_u64,
    mac2_u64;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_PP_MAC_ADDRESS_INC);

  
     
  res = soc_sand_pp_mac_address_struct_to_long(
          mac,
          mac1_u64.arr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  mac2_u64.arr[0] = 1;
  mac2_u64.arr[1] = 0;

  soc_sand_u64_add_u64(
          &mac1_u64,
          &mac2_u64
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
     
  res = soc_sand_pp_mac_address_long_to_struct(
          mac1_u64.arr,
          mac
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pp_mac_address_add()",0,0);
}

uint32
  soc_sand_pp_mac_address_add(
    SOC_SAND_INOUT  SOC_SAND_PP_MAC_ADDRESS   *mac1,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS      *mac2
  )
{
  SOC_SAND_U64
    mac1_u64,
    mac2_u64;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_PP_MAC_ADDRESS_ADD);

  
     
  res = soc_sand_pp_mac_address_struct_to_long(
          mac1,
          mac1_u64.arr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
     
  res = soc_sand_pp_mac_address_struct_to_long(
          mac2,
          mac2_u64.arr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  soc_sand_u64_add_u64(
          &mac1_u64,
          &mac2_u64
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
     
  res = soc_sand_pp_mac_address_long_to_struct(
          mac1_u64.arr,
          mac1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pp_mac_address_add()",0,0);
}


uint32
  soc_sand_pp_mac_address_are_equal(
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac1,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac2,
    SOC_SAND_OUT  uint8              *equal
  )
{
  SOC_SAND_U64
    mac1_u64,
    mac2_u64;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_PP_MAC_ADDRESS_ARE_EQUAL);

  
     
  res = soc_sand_pp_mac_address_struct_to_long(
          mac1,
          mac1_u64.arr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
     
  res = soc_sand_pp_mac_address_struct_to_long(
          mac2,
          mac2_u64.arr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *equal = (uint8) soc_sand_u64_are_equal(
            &mac1_u64,
            &mac2_u64
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pp_mac_address_are_equal()",0,0);
}

uint32
  soc_sand_pp_mac_address_is_smaller(
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac1,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac2,
    SOC_SAND_OUT  uint8              *is_smaller
  )
{
  SOC_SAND_U64
    mac1_u64,
    mac2_u64;
  uint32
    res;
  uint8
    is_bigger,
    are_equal;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_PP_MAC_ADDRESS_ARE_EQUAL);

  
     
  res = soc_sand_pp_mac_address_struct_to_long(
          mac1,
          mac1_u64.arr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
     
  res = soc_sand_pp_mac_address_struct_to_long(
          mac2,
          mac2_u64.arr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  is_bigger = SOC_SAND_NUM2BOOL(soc_sand_u64_is_bigger(&mac1_u64, &mac2_u64));
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  are_equal = SOC_SAND_NUM2BOOL(soc_sand_u64_are_equal(&mac1_u64, &mac2_u64));
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  *is_smaller = (is_bigger || are_equal) ? FALSE : TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pp_mac_address_is_smaller()",0,0);
}

uint32
  soc_sand_pp_mac_address_sub(
    SOC_SAND_INOUT  SOC_SAND_PP_MAC_ADDRESS   *mac1,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS      *mac2
  )
{
  SOC_SAND_U64
    mac1_u64,
    mac2_u64;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_PP_MAC_ADDRESS_SUB);

  
     
  res = soc_sand_pp_mac_address_struct_to_long(
          mac1,
          mac1_u64.arr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
     
  res = soc_sand_pp_mac_address_struct_to_long(
          mac2,
          mac2_u64.arr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  soc_sand_u64_subtract_u64(
          &mac1_u64,
          &mac2_u64
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
     
  res = soc_sand_pp_mac_address_long_to_struct(
          mac1_u64.arr,
          mac1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pp_mac_address_sub()",0,0);
}


uint32
  soc_sand_pp_mac_address_reverse(
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *in_mac_add,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS    *out_mac_add
  )
{
  uint8
    tmp;
  uint32
    char_indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_PP_MAC_ADDRESS_REVERSE);


  for (char_indx = 0; char_indx<SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++char_indx)
  {
    tmp = (in_mac_add->address)[char_indx];
    (out_mac_add->address)[SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - char_indx - 1] = tmp;
  }

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pp_mac_address_reverse()",0,0);
}

STATIC uint32
  soc_sand_pp_mac_get_char_num_val(
    SOC_SAND_IN char mac_char,
    SOC_SAND_OUT uint8  *num_val
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(mac_char == '0')
  {
    *num_val = 0;
  }
  else if(mac_char >= '1' && mac_char <='9')
  {
    *num_val = mac_char + 1 - '1';
  }
  else if(mac_char >= 'a' && mac_char <='f')
  {
    *num_val = mac_char + 10 - 'a';
  }
  else if(mac_char >= 'A' && mac_char <='F')
  {
    *num_val = mac_char + 10 - 'A';
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_PP_MAC_ADDRESS_ILLEGAL_STRING_ERR, 10, exit);
  }
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pp_mac_get_char_num_val()",0,0);
}


uint32
  soc_sand_pp_mac_address_string_parse(
    SOC_SAND_IN char               mac_string[SOC_SAND_PP_MAC_ADDRESS_STRING_LEN],
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS   *mac_addr
  )
{
  uint32
    byte_indx,
    char_indx,
    input_len;
  uint8
    num_val;
  uint32
    res;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_PP_MAC_ADDRESS_STRING_PARSE);

  input_len = soc_sand_os_strlen((const char*)mac_string);

  soc_sand_SAND_PP_MAC_ADDRESS_clear(mac_addr);

  byte_indx = 0;
  for(char_indx=input_len; char_indx > 0 ;char_indx-=2, ++byte_indx)
  {
    res = soc_sand_pp_mac_get_char_num_val(
            mac_string[char_indx - 1],
            &num_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    (mac_addr->address)[byte_indx] = (mac_addr->address)[byte_indx] + num_val;
    if (char_indx == 1 )
    {
      goto exit;
    }
    res = soc_sand_pp_mac_get_char_num_val(
            mac_string[char_indx - 2],
            &num_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    num_val <<= 4;
    mac_addr->address[byte_indx] = mac_addr->address[byte_indx] + num_val;

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pp_mac_address_string_parse()",0,0);
}


void
  soc_sand_SAND_PP_MAC_ADDRESS_clear(
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_PP_MAC_ADDRESS));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);

}

uint32
  SOC_SAND_PP_MAC_ADDRESS_verify(
    SOC_SAND_IN SOC_SAND_PP_MAC_ADDRESS *mac_addr
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(mac_addr);


  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_SAND_PP_MAC_ADDRESS_verify()",0,0);
}

#if SOC_SAND_DEBUG

void
  soc_sand_SAND_PP_MAC_ADDRESS_print(
    SOC_SAND_IN SOC_SAND_PP_MAC_ADDRESS *mac_addr
  )
{
  uint8
    tmp;
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(mac_addr);

  for (indx = SOC_SAND_PP_MAC_ADDRESS_NOF_U8; indx > 0; --indx)
  {
    tmp = mac_addr->address[indx - 1];
    LOG_CLI((BSL_META_U(unit,
                        "%02x"), tmp));
    if (indx > 1)
    {
      LOG_CLI((BSL_META_U(unit,
                          ":")));
    }
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_PP_MAC_ADDRESS_array_print(
    SOC_SAND_IN uint8       mac_address[SOC_SAND_PP_MAC_ADDRESS_NOF_U8]
  )
{
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(mac_address);

  for (indx = SOC_SAND_PP_MAC_ADDRESS_NOF_U8; indx > 0; --indx)
  {
    LOG_CLI((BSL_META_U(unit,
                        "%02x"), mac_address[indx - 1]));
    if (indx > 1)
    {
      LOG_CLI((BSL_META_U(unit,
                          ":")));
    }
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_sand_pp_mac_tests(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
  SOC_SAND_PP_MAC_ADDRESS
    mac_addr,
    rev_mac_addr;

  if (!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "\n\r ********** soc_sand_pp_mac_tests : START **********\n")));
  }

  soc_sand_pp_mac_address_string_parse((char*)"012345",&mac_addr);
  soc_sand_SAND_PP_MAC_ADDRESS_print(&mac_addr);
  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META_U(unit,
                       "\n")));
  soc_sand_pp_mac_address_reverse(&mac_addr,&rev_mac_addr);
  soc_sand_SAND_PP_MAC_ADDRESS_print(&rev_mac_addr);

  return;

}




#endif 

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#include <soc/dpp/SAND/Utils/sand_conv.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>


SOC_SAND_RET
  soc_sand_kbits_per_sec_to_clocks(
    SOC_SAND_IN       uint32    rate,     
    SOC_SAND_IN       uint32     credit,   
    SOC_SAND_IN       uint32    ticks_per_sec,
    SOC_SAND_OUT      uint32*   interval  
  )
{
  SOC_SAND_RET
    ex ;
  SOC_SAND_U64
    calc,
    calc2;
  uint32
    tmp;

  if (NULL == interval)
  {
    ex = SOC_SAND_NULL_POINTER_ERR ;
    goto exit ;
  }
  if (0 == rate)
  {
    
    ex = SOC_SAND_DIV_BY_ZERO_ERR ;
    goto exit ;
  }
  soc_sand_u64_multiply_longs((credit * SOC_SAND_NOF_BITS_IN_CHAR),
                          SOC_SAND_DIV_ROUND(ticks_per_sec,1000),
                          &calc);
  soc_sand_u64_devide_u64_long(&calc, rate, &calc2);
  if (soc_sand_u64_to_long(&calc2, &tmp))
  {
    
    ex = SOC_SAND_OVERFLOW_ERR ;
    goto exit ;
  }
  *interval = tmp;
  ex = SOC_SAND_OK ;
exit:
  return ex ;
}


SOC_SAND_RET
  soc_sand_clocks_to_kbits_per_sec(
    SOC_SAND_IN       uint32    interval, 
    SOC_SAND_IN       uint32     credit,   
    SOC_SAND_IN       uint32    ticks_per_sec,
    SOC_SAND_OUT      uint32*   rate      
  )
{
  SOC_SAND_RET
    ex ;
  SOC_SAND_U64
    calc,
    calc2;
  uint32
    tmp;

  if (NULL == rate)
  {
    ex = SOC_SAND_NULL_POINTER_ERR ;
    goto exit ;
  }
  if (0 == interval)
  {
    
    ex = SOC_SAND_DIV_BY_ZERO_ERR ;
    goto exit ;
  }
  soc_sand_u64_multiply_longs((credit * SOC_SAND_NOF_BITS_IN_CHAR),
                          SOC_SAND_DIV_ROUND(ticks_per_sec,1000),
                          &calc);
  soc_sand_u64_devide_u64_long(&calc, interval, &calc2);
  if (soc_sand_u64_to_long(&calc2, &tmp))
  {
    
    ex = SOC_SAND_OVERFLOW_ERR ;
    goto exit ;
  }
  *rate = tmp;
  ex = SOC_SAND_OK ;
exit:
  return ex ;
}



uint32
  soc_sand_ip_addr_numeric_to_string(
    SOC_SAND_IN uint32 ip_addr,
    SOC_SAND_IN uint8  short_format,
    SOC_SAND_OUT char   decimal_ip[SOC_SAND_IP_STR_SIZE]
  )
{
  const char
    *format ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_IP_ADDR_NUMERIC_TO_STRING);
  SOC_SAND_CHECK_NULL_INPUT(decimal_ip);

  if (short_format)
  {
    format = "%d.%d.%d.%d";
  }
  else
  {
    format = "%03d.%03d.%03d.%03d";
  }


  sal_sprintf(decimal_ip, format,
    (unsigned char)(((ip_addr)>>24)&0xFF),
    (unsigned char)(((ip_addr)>>16)&0xFF),
    (unsigned char)(((ip_addr)>>8)&0xFF),
    (unsigned char)((ip_addr)&0xFF)) ;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_ip_addr_numeric_to_string()",0,0);
}



uint32
  soc_sand_ip_addr_string_to_numeric(
    SOC_SAND_IN char   decimal_ip[SOC_SAND_IP_STR_SIZE],
    SOC_SAND_OUT uint32 *ip_addr
  )
{
  uint8
    curr_char;
  uint32
    index,
    dots_count;
  uint32
    val,
    tmp_addr;
  uint8
    illegal;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_IP_ADDR_STRING_TO_NUMERIC);
  SOC_SAND_CHECK_NULL_INPUT(decimal_ip);

  tmp_addr = 0;
  dots_count = 0;
  val = 0;
  illegal = FALSE;

  for (index = 0; index <SOC_SAND_IP_STR_SIZE; ++index)
  {
    curr_char = decimal_ip[index];

    if( curr_char == '\0')
    {
      tmp_addr |= val << 8 * ( 3 - dots_count);
      break;
    }
    if(curr_char == '.')
    {
      tmp_addr |= val << 8 * ( 3 - dots_count);
      val = 0;
      ++dots_count;
      if(dots_count > 3)
      {
        illegal = TRUE;
        break;
      }
    }
    else if(curr_char >= '0' && curr_char <= '9')
    {
      val = val * 10 + (curr_char - '0');
    }
    else
    {
      illegal = TRUE;
      break;
    }
    if (val > 0xFF)
    {
      illegal = TRUE;
      break;
    }
  }

  if (illegal || dots_count < 3)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_IP_FORMAT,10,exit);
  }

  *ip_addr = tmp_addr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_ip_addr_string_to_numeric()",0,0);

}

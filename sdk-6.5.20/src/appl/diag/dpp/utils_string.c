/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#include <appl/diag/dpp/utils_string.h>

#ifdef __DHRP_LINUX_HRP__
/* { */
  #include <stdio.h>
  #include <string.h>
  #include <ctype.h>
/* } */
#else
/* { */
  #include "stdio.h"
  #include "string.h"
  #include "ctype.h"
/* } */
#endif

/*****************************************************
*NAME
*  sprint_ip
*TYPE: PROC
*DATE: 17/FEB/2002
*FUNCTION:
*  Convert an unsigned long to ascii representation
*  of 4 decimal numbers separated by dots.
*CALLING SEQUENCE:
*  sprint_ip(decimal_ip,ip_addr)
*INPUT:
*  SOC_SAND_DIRECT:
*          char *decimal_ip -
*      Pointer to a char array. To contain the
*      resultant string. Must be 16 bytes long
*      at least.
*    const unsigned long ip_addr -
*      IP address in long representationc:
*      First decimal number is MS byte.
*    const unsigned int  short_format -
*      Indicator
*      TRUE  - IP will be 10.0.0.1
*      FALSE - IP will be 010.000.000.001
*  SOC_SAND_INDIRECT:
*    none.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    POINTER TO A CHAR ARRAY -
*      Points to the input buffer. Makes it easier
*      to use within 'printf'.
*  SOC_SAND_INDIRECT:
*    See decimal_ip.
*REMARKS:
*  Make sure input buffer is at least 16 bytes.
*SEE ALSO:
 */
char*
  sprint_ip(
          char          decimal_ip[MAX_IP_STRING],
    const unsigned long ip_addr,
    const unsigned int  short_format
  )
{
  char
    *ret,
    *format ;
  ret = decimal_ip ;

  if (short_format)
  {
    format = "%d.%d.%d.%d";
  }
  else
  {
    format = "%03d.%03d.%03d.%03d";
  }

/*
 * EXTRACTING BYTES FROM UNSIGNED LONG
 */
#define BYTE_NO_1(x) ((unsigned char)(((x)>>24)&0xFF))
#define BYTE_NO_2(x) ((unsigned char)(((x)>>16)&0xFF))
#define BYTE_NO_3(x) ((unsigned char)(((x)>>8)&0xFF))
#define BYTE_NO_4(x) ((unsigned char)((x)&0xFF))
  sprintf(decimal_ip, format,
                BYTE_NO_1(ip_addr),
                BYTE_NO_2(ip_addr),
                BYTE_NO_3(ip_addr),
                BYTE_NO_4(ip_addr)) ;

  return (ret) ;
}



/*****************************************************
*$NAME$
*  str_to_lower
*TYPE: PROC
*DATE: 20/JAN/2002
*FUNCTION:
*  Convert all letters on input string (assumed to
*  be in ram) to lower case.
*CALLING SEQUENCE:
*  str_to_lower(in_str)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *in_str -
*      Pointer to ASCII string (null terminated)
*      to convert.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      Length of input string (number of
*      characters tested).
*  SOC_SAND_INDIRECT:
*    Converted string.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  str_to_lower(
    char *in_str
  )
{
  int
    ret ;
  unsigned
    int
      ui,
      len ;
  ret = 0 ;
  len = strlen(in_str) ;
  for (ui = 0 ; ui < len ; ui++)
  {
    *in_str = tolower(*in_str) ;
    in_str++ ;
  }
  ret = ui ;
  return (ret);
}


/*****************************************************
*$NAME$
*  str_max_match
*TYPE: PROC
*DATE: 22/JAN/2002
*FUNCTION:
*  Given two strings (null terminated), get the number
*  of matching characters but not more than a specified
*  number.
*CALLING SEQUENCE:
*  str_max_match(in_str_01,in_str_02)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *in_str_01 -
*      Pointer to first ASCII string (null terminated).
*    char *in_str_02 -
*      Pointer to second ASCII string (null terminated).
*    unsigned int max_to_compare -
*      Maximal number of characters to compare on both
*      strings. Return value can not be larger than
*      this number.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned int -
*      Number of matching characters.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  Example: 'abcdse' and 'abcxd' will result in
*  getting '3'.
*SEE ALSO:
 */
unsigned
  int
    str_max_match(
      const char         *in_str_01,
      const char         *in_str_02,
      const unsigned int max_to_compare
    )
{
  int
    ret ;
  unsigned
    int
      ui,
      len,
      len1,
      len2 ;
  ret = 0 ;
  len1 = strlen(in_str_01) ;
  len2 = strlen(in_str_02) ;
  if (len1 < len2)
  {
    len = len1 ;
  }
  else
  {
    len = len2 ;
  }
  if (len > max_to_compare)
  {
    len = max_to_compare ;
  }
  for (ui = 0 ; ui < len ; ui++,in_str_01++,in_str_02++)
  {
    if (*in_str_01 == *in_str_02)
    {
      ret++ ;
    }
    else
    {
      break ;
    }
  }
  return (ret);
}

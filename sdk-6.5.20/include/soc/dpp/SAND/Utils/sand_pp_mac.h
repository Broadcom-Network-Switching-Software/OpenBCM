/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef  SOC_SAND_DRIVER_MAC_H
#define SOC_SAND_DRIVER_MAC_H



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>







#define  SOC_SAND_PP_MAC_ADDRESS_STRING_LEN 12

#define  SOC_SAND_PP_MAC_ADDRESS_NOF_U8 6

#define  SOC_SAND_PP_MAC_ADDRESS_NOF_BITS (SOC_SAND_PP_MAC_ADDRESS_NOF_U8 * SOC_SAND_NOF_BITS_IN_CHAR)

#define  SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S SOC_SAND_U64_NOF_UINT32S







#define SOC_SAND_PP_MAC_ADDRESS_IS_ZERO(_mac_)  \
    (((_mac_).address[0] | (_mac_).address[1] | (_mac_).address[2] | \
      (_mac_).address[3] | (_mac_).address[4] | (_mac_).address[5]) == 0) 







  typedef struct
  {
    SOC_SAND_MAGIC_NUM_VAR
    uint8  address[SOC_SAND_PP_MAC_ADDRESS_NOF_U8];
  }SOC_SAND_PP_MAC_ADDRESS;













uint32
  soc_sand_pp_mac_address_struct_to_long(
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac_add,
    SOC_SAND_OUT uint32            mac_add_U32[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S]
  );


uint32
  soc_sand_pp_mac_address_long_to_struct(
    SOC_SAND_IN  uint32            mac_add_long[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS    *mac_add_struct
  );



uint32
  soc_sand_pp_mac_address_inc(
    SOC_SAND_INOUT  SOC_SAND_PP_MAC_ADDRESS   *mac
  );


uint32
  soc_sand_pp_mac_address_are_equal(
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac1,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac2,
    SOC_SAND_OUT  uint8              *equal
  );


uint32
  soc_sand_pp_mac_address_is_smaller(
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac1,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac2,
    SOC_SAND_OUT  uint8              *is_smaller
  );


uint32
  soc_sand_pp_mac_address_add(
    SOC_SAND_INOUT  SOC_SAND_PP_MAC_ADDRESS   *mac1,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS      *mac2
  );



uint32
  soc_sand_pp_mac_address_sub(
    SOC_SAND_INOUT  SOC_SAND_PP_MAC_ADDRESS   *mac1,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS      *mac2
  );



uint32
  soc_sand_pp_mac_address_string_parse(
    SOC_SAND_IN char               mac_string[SOC_SAND_PP_MAC_ADDRESS_STRING_LEN],
    SOC_SAND_OUT  SOC_SAND_PP_MAC_ADDRESS   *mac_addr
  );



uint32
  soc_sand_pp_mac_address_reverse(
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *in_mac_add,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS    *out_mac_add
  );



void
  soc_sand_SAND_PP_MAC_ADDRESS_clear(
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS *mac_addr
  );

uint32
  SOC_SAND_PP_MAC_ADDRESS_verify(
    SOC_SAND_IN SOC_SAND_PP_MAC_ADDRESS *mac_addr
  );

#if SOC_SAND_DEBUG



void
  soc_sand_SAND_PP_MAC_ADDRESS_print(
    SOC_SAND_IN SOC_SAND_PP_MAC_ADDRESS *mac_addr
  );

void
  soc_sand_SAND_PP_MAC_ADDRESS_array_print(
    SOC_SAND_IN uint8       mac_address[SOC_SAND_PP_MAC_ADDRESS_NOF_U8]
  );


void
  soc_sand_pp_mac_tests(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );



#endif


#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif

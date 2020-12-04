/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef SOC_SAND_BITSTREAM_H_INCLUDED

#define SOC_SAND_BITSTREAM_H_INCLUDED
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>


#define SOC_SAND_BIT_STREAM_FIELD_SET_SIZE    (SOC_SAND_NOF_BITS_IN_UINT32)


SOC_SAND_RET
  soc_sand_bitstream_clear(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    size 
    );

SOC_SAND_RET
  soc_sand_bitstream_fill(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    size 
  );

SOC_SAND_RET
  soc_sand_bitstream_set(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    place,
    SOC_SAND_IN       uint32     bit_indicator
    );

SOC_SAND_RET
  soc_sand_bitstream_set_field(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    start_bit,
    SOC_SAND_IN       uint32    nof_bits,
    SOC_SAND_IN       uint32    field
    );

SOC_SAND_RET
  soc_sand_bitstream_get_field(
    SOC_SAND_IN       uint32    *bit_stream,
    SOC_SAND_IN       uint32    start_bit,
    SOC_SAND_IN       uint32    nof_bits,
    SOC_SAND_OUT      uint32    *field
    );

SOC_SAND_RET
  soc_sand_bitstream_set_bit(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    place
    );

SOC_SAND_RET
  soc_sand_bitstream_set_bit_range(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    start_place,
    SOC_SAND_IN       uint32    end_place
    );

SOC_SAND_RET
  soc_sand_bitstream_reset_bit(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    place
    );

SOC_SAND_RET
  soc_sand_bitstream_reset_bit_range(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    start_place,
    SOC_SAND_IN       uint32    end_place
  );

int
  soc_sand_bitstream_test_bit(
    SOC_SAND_IN       uint32    *bit_stream,
    SOC_SAND_IN       uint32    place
  );

int
  soc_sand_bitstream_test_and_reset_bit(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    place
  );

int
  soc_sand_bitstream_test_and_set_bit(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    place
  );

SOC_SAND_RET
  soc_sand_bitstream_bit_flip(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    place
  );

int
  soc_sand_bitstream_have_one_in_range(
    SOC_SAND_IN       uint32    *bit_stream,
    SOC_SAND_IN       uint32    start_place,
    SOC_SAND_IN       uint32    end_place
  );

int
  soc_sand_bitstream_have_one(
    SOC_SAND_IN       uint32    *bit_stream,
    SOC_SAND_IN       uint32    size 
  );

SOC_SAND_RET
  soc_sand_bitstream_or(
    SOC_SAND_INOUT    uint32    *bit_stream1,
    SOC_SAND_IN       uint32    *bit_stream2,
    SOC_SAND_IN       uint32    size 
  );

SOC_SAND_RET
  soc_sand_bitstream_and(
    SOC_SAND_INOUT    uint32    *bit_stream1,
    SOC_SAND_IN       uint32    *bit_stream2,
    SOC_SAND_IN       uint32    size 
  );

SOC_SAND_RET
  soc_sand_bitstream_xor(
    SOC_SAND_INOUT    uint32    *bit_stream1,
    SOC_SAND_IN       uint32    *bit_stream2,
    SOC_SAND_IN       uint32    size 
  );

uint32
  soc_sand_bitstream_parity(
    SOC_SAND_IN  uint32    *bit_stream,
    SOC_SAND_IN  uint32    start_bit,
    SOC_SAND_IN  uint32    nof_bits
  );

SOC_SAND_RET
  soc_sand_bitstream_not(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    size 
  );


uint32
  soc_sand_bitstream_get_nof_on_bits(
    SOC_SAND_IN       uint32    *bit_stream,
    SOC_SAND_IN       uint32    size 
  );



SOC_SAND_RET
  soc_sand_buff_xor(
    SOC_SAND_INOUT    unsigned  char    *buff1,
    SOC_SAND_IN       unsigned  char    *buff2,
    SOC_SAND_IN       uint32     size 
  );



uint32
  soc_sand_bitstream_set_any_field(
    SOC_SAND_IN  uint32    *input_buffer,
    SOC_SAND_IN  uint32    start_bit,
    SOC_SAND_IN  uint32    nof_bits,
    SOC_SAND_OUT uint32    *output_buffer
  );


uint32
  soc_sand_bitstream_get_any_field(
    SOC_SAND_IN  uint32    *input_buffer,
    SOC_SAND_IN  uint32    start_bit,
    SOC_SAND_IN  uint32    nof_bits,
    SOC_SAND_OUT uint32    *output_buffer
  );


SOC_SAND_RET
  soc_sand_bitsteam_u8_ms_byte_first_get_field(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint8  *input_buffer,
    SOC_SAND_IN  uint32 start_bit_msb,
    SOC_SAND_IN  uint32 nof_bits,
    SOC_SAND_OUT uint32 *output_value
  );



#if SOC_SAND_DEBUG




uint32
  soc_sand_bitstream_offline_test(uint32 silent);


void
  soc_sand_bitstream_print(
    SOC_SAND_IN uint32 *bit_stream,
    SOC_SAND_IN uint32 size 
  );

void
  soc_sand_bitstream_print_beautify_1(
    SOC_SAND_IN uint32 *bit_stream,
    SOC_SAND_IN uint32 size, 
    SOC_SAND_IN uint32 max_dec_digits,
    SOC_SAND_IN uint32 max_nof_printed_items
  );


SOC_SAND_RET
  soc_sand_buff_print_non_zero(
    SOC_SAND_IN unsigned char* buff,
    SOC_SAND_IN uint32 buff_size
  );


SOC_SAND_RET
  soc_sand_buff_print_all(
    SOC_SAND_IN unsigned char* buff,
    SOC_SAND_IN uint32 buff_size,
    uint32 nof_columns
  );


#endif 

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif

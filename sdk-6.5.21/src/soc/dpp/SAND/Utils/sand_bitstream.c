/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>




SOC_SAND_RET
  soc_sand_bitstream_clear(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    size 
  )
{
  uint32 iii;
  
  for (iii=0; iii<size; ++iii)
  {
    bit_stream[iii] = 0;
  }
  return SOC_SAND_OK;
}


SOC_SAND_RET
  soc_sand_bitstream_fill(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    size 
  )
{
  uint32 iii;
  
  for (iii=0; iii<size; ++iii)
  {
    bit_stream[iii] = 0xFFFFFFFF;
  }
  return SOC_SAND_OK;
}


SOC_SAND_RET
  soc_sand_bitstream_set(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    place,
    SOC_SAND_IN       uint32     bit_indicator
  )
{
  SOC_SAND_RET soc_sand_ret;

  if(bit_indicator)
  {
    soc_sand_ret = soc_sand_bitstream_set_bit(bit_stream, place);
  }
  else
  {
    soc_sand_ret = soc_sand_bitstream_reset_bit(bit_stream, place);
  }
  return soc_sand_ret;
}

SOC_SAND_RET
  soc_sand_bitstream_set_field(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    start_bit,
    SOC_SAND_IN       uint32    nof_bits,
    SOC_SAND_IN       uint32    field
  )
{
  SOC_SAND_RET
    soc_sand_ret;
  uint32
    bit_stream_bit_i,
    field_bit_i;

  soc_sand_ret = SOC_SAND_OK;

  
  if( nof_bits > SOC_SAND_BIT_STREAM_FIELD_SET_SIZE)
  {
    soc_sand_ret = SOC_SAND_BIT_STREAM_FIELD_SET_SIZE_RANGE_ERR;
    goto exit;
  }


  for( bit_stream_bit_i=start_bit, field_bit_i = 0;
       field_bit_i< nof_bits;
       ++bit_stream_bit_i, ++field_bit_i)
  {
    soc_sand_ret = soc_sand_bitstream_set(bit_stream, bit_stream_bit_i, (field>>field_bit_i)& 0x1);
    if( soc_sand_ret != SOC_SAND_OK )
    {
      goto exit;
    }
  }

exit:
  return soc_sand_ret;
}

SOC_SAND_RET
  soc_sand_bitstream_get_field(
    SOC_SAND_IN       uint32    *bit_stream,
    SOC_SAND_IN       uint32    start_bit,
    SOC_SAND_IN       uint32    nof_bits,
    SOC_SAND_OUT      uint32    *field
  )
{
  SOC_SAND_RET
    soc_sand_ret;
  uint32
    bit_stream_bit_i,
    field_bit_i;

  soc_sand_ret = SOC_SAND_OK;


  if(field == NULL)
  {
    soc_sand_ret = SOC_SAND_NULL_POINTER_ERR;
    goto exit;
  }

  
  if( nof_bits > SOC_SAND_BIT_STREAM_FIELD_SET_SIZE)
  {
    soc_sand_ret = SOC_SAND_BIT_STREAM_FIELD_SET_SIZE_RANGE_ERR;
    goto exit;
  }

  *field = 0;
  for( bit_stream_bit_i=start_bit, field_bit_i = 0;
       field_bit_i< nof_bits;
       ++bit_stream_bit_i, ++field_bit_i)
  {
    *field |= ((uint32)soc_sand_bitstream_test_bit(bit_stream, bit_stream_bit_i)) << field_bit_i;
  }

exit:
  return soc_sand_ret;
}

SOC_SAND_RET
  soc_sand_bitstream_set_bit(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    place
  )
{
  bit_stream[place>>5] |= SOC_SAND_BIT(place & 0x0000001F);
  return SOC_SAND_OK;
}

SOC_SAND_RET
  soc_sand_bitstream_set_bit_range(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    start_place,
    SOC_SAND_IN       uint32    end_place
  )
{
  uint32
    bit_i;

  for(bit_i = start_place; bit_i <= end_place; ++bit_i)
  {
    soc_sand_bitstream_set_bit(bit_stream, bit_i);
  }

  return SOC_SAND_OK;
}

SOC_SAND_RET
  soc_sand_bitstream_reset_bit(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    place
  )
{
  bit_stream[place>>5] &= SOC_SAND_RBIT(place & 0x0000001F);
  return SOC_SAND_OK;
}

SOC_SAND_RET
  soc_sand_bitstream_reset_bit_range(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    start_place,
    SOC_SAND_IN       uint32    end_place
  )
{
  uint32
    bit_i;

  for(bit_i = start_place; bit_i <= end_place; ++bit_i)
  {
    soc_sand_bitstream_reset_bit(bit_stream, bit_i);
  }

  return SOC_SAND_OK;
}

int
  soc_sand_bitstream_test_bit(
    SOC_SAND_IN  uint32    *bit_stream,
    SOC_SAND_IN  uint32    place
  )
{
  uint32 result;
  
  result = bit_stream[place>>5] & SOC_SAND_BIT(place & 0x0000001F);
  
  if (result)
  {
    return 1;
  }
  return 0;
}

int
  soc_sand_bitstream_test_and_reset_bit(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    place
  )
{
  int
    result;

  
  result = soc_sand_bitstream_test_bit(bit_stream, place);

  
  soc_sand_bitstream_reset_bit(bit_stream, place);

  return result;
}

int
  soc_sand_bitstream_test_and_set_bit(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    place
  )
{
  int
    result;

  
  result = soc_sand_bitstream_test_bit(bit_stream, place);

  
  soc_sand_bitstream_set_bit(bit_stream, place);

  return result;
}


SOC_SAND_RET
  soc_sand_bitstream_bit_flip(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    place
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

  bit_stream[place>>5] ^= SOC_SAND_BIT(place & 0x0000001F);

  return soc_sand_ret;
}


int
  soc_sand_bitstream_have_one_in_range(
    SOC_SAND_IN       uint32    *bit_stream,
    SOC_SAND_IN       uint32    start_place,
    SOC_SAND_IN       uint32    end_place
  )
{
  uint32
    bit_i;
  int
    result;

  result = FALSE;

  for(bit_i = start_place; bit_i <= end_place; ++bit_i)
  {
    if (soc_sand_bitstream_test_bit(bit_stream, bit_i))
    {
      result = TRUE;
      goto exit;
    }
  }

exit:
  return result;
}

int
  soc_sand_bitstream_have_one(
    SOC_SAND_IN       uint32    *bit_stream,
    SOC_SAND_IN       uint32    size 
  )
{
  uint32
    word_i;
  int
    result;
  const uint32
    *pointer;

  result = FALSE;

  pointer = bit_stream;
  for(word_i = 0; word_i < size; ++word_i, ++pointer)
  {
    if ( (*pointer) != 0)
    {
      result = TRUE;
      goto exit;
    }
  }

exit:
  return result;
}


SOC_SAND_RET
  soc_sand_bitstream_or(
    SOC_SAND_INOUT    uint32    *bit_stream1,
    SOC_SAND_IN       uint32    *bit_stream2,
    SOC_SAND_IN       uint32    size 
  )
{
  uint32 i;
  
  for(i=0; i<size; ++i)
  {
    bit_stream1[i] |= bit_stream2[i];
  }
  return SOC_SAND_OK;
}

SOC_SAND_RET
  soc_sand_bitstream_and(
    SOC_SAND_INOUT    uint32    *bit_stream1,
    SOC_SAND_IN       uint32    *bit_stream2,
    SOC_SAND_IN       uint32    size 
  )
{
  uint32 i;
  
  for(i=0; i<size; ++i)
  {
    bit_stream1[i] &= bit_stream2[i];
  }
  return SOC_SAND_OK;
}

SOC_SAND_RET
  soc_sand_bitstream_xor(
    SOC_SAND_INOUT    uint32    *bit_stream1,
    SOC_SAND_IN       uint32    *bit_stream2,
    SOC_SAND_IN       uint32    size 
  )
{
  uint32 i;
  
  for(i=0; i<size; ++i)
  {
    bit_stream1[i] ^= bit_stream2[i];
  }
  return SOC_SAND_OK;
}

uint32
  soc_sand_bitstream_parity(
    SOC_SAND_IN  uint32    *bit_stream,
    SOC_SAND_IN  uint32    start_bit,
    SOC_SAND_IN  uint32    nof_bits
  )
{
  uint32
    buffer = 0;
  uint32
    idx = 0;
  uint32
    parity = 0;
  uint32
    start_bit_aligned = start_bit / SOC_SAND_NOF_BITS_IN_UINT32;
  const uint32
    *bit_stream_aligned = bit_stream + start_bit_aligned;

  for (idx = start_bit_aligned; idx < start_bit_aligned + nof_bits; ++idx)
  {
    buffer = *(bit_stream_aligned + (idx / SOC_SAND_NOF_BITS_IN_UINT32));
    parity = (~((~parity) ^ (SOC_SAND_BIT(idx % SOC_SAND_NOF_BITS_IN_UINT32) & buffer))) & SOC_SAND_BIT(idx % SOC_SAND_NOF_BITS_IN_UINT32);
  }

  return parity;
}

SOC_SAND_RET
  soc_sand_bitstream_not(
    SOC_SAND_INOUT    uint32    *bit_stream,
    SOC_SAND_IN       uint32    size 
  )
{
  uint32 i;
  
  for(i=0; i<size; ++i)
  {
    bit_stream[i] ^= 0xFFFFFFFF;
  }
  return SOC_SAND_OK;
}


uint32
  soc_sand_bitstream_get_nof_on_bits(
    SOC_SAND_IN       uint32    *bit_stream,
    SOC_SAND_IN       uint32    size 
  )
{
  uint32
    nof_on_bits,
    i;

  nof_on_bits = 0;
  
  if (NULL == bit_stream)
  {
    goto exit;
  }

  for(i=0; i<size; ++i)
  {
    nof_on_bits += soc_sand_nof_on_bits_in_long(bit_stream[i]);
  }

exit:
  return nof_on_bits;
}

#if SOC_SAND_DEBUG



uint32
  soc_sand_bitstream_offline_test(uint32 silent)
{
  uint32
    bit_stream[2],
    field;
  uint32
    pass;

  pass = TRUE;

  soc_sand_bitstream_clear(bit_stream, 2);
  soc_sand_bitstream_set_field(bit_stream, 0, 4, 0xF);
  if(!silent)
  {
    soc_sand_bitstream_print(bit_stream, 2);
    soc_sand_bitstream_print_beautify_1(bit_stream, 2, 7, 100);
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("\r\n")));
  }

  soc_sand_bitstream_clear(bit_stream, 2);
  soc_sand_bitstream_set_field(bit_stream, 4, 4, 0xF);
  if(!silent)
  {
    soc_sand_bitstream_print(bit_stream, 2);
    soc_sand_bitstream_print_beautify_1(bit_stream, 2, 5, 100);
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("\r\n")));
  }

  soc_sand_bitstream_clear(bit_stream, 2);
  soc_sand_bitstream_set_field(bit_stream, 30, 4, 0xF);
  if(!silent)
  {
    soc_sand_bitstream_print(bit_stream, 2);
    soc_sand_bitstream_print_beautify_1(bit_stream, 2, 4, 100);
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("\r\n")));
  }


  soc_sand_bitstream_clear(bit_stream, 2);
  soc_sand_bitstream_set_field(bit_stream, 4, 16, 0x12345);
  soc_sand_bitstream_set_field(bit_stream, 20, 28, 0xFFFFFFFF);
  if(!silent)
  {
    soc_sand_bitstream_print(bit_stream, 2);
    soc_sand_bitstream_print_beautify_1(bit_stream, 2, 3, 20);
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("\r\n")));
  }

  soc_sand_bitstream_get_field(bit_stream, 8, 12, &field);
  if(field != 0x234)
  {
    pass &= 0;
  }

  return pass;
}



void
  soc_sand_bitstream_print(
    SOC_SAND_IN uint32 *bit_stream,
    SOC_SAND_IN uint32 size 
  )
{
  uint32
    line_i,
    not_lines;
  int
    long_i;
  const int
    nof_longs_per_line = 8;

  if (NULL == bit_stream)
  {
    LOG_CLI((BSL_META("soc_sand_bitstream_print received NULL ptr\n\r")));
    goto exit;
  }

  LOG_CLI((BSL_META("(Hex format, Maximum of %u bits per line.)\r\n"),
nof_longs_per_line*(int)SOC_SAND_NOF_BITS_IN_UINT32));
  
  not_lines = SOC_SAND_DIV_ROUND_UP(size, nof_longs_per_line);
  for(line_i=0; line_i< not_lines; ++line_i)
  {
    LOG_CLI((BSL_META("[%4u-%4u]"),
SOC_SAND_MIN( (line_i+1)*nof_longs_per_line*(int)SOC_SAND_NOF_BITS_IN_UINT32-1, size * (int)SOC_SAND_NOF_BITS_IN_UINT32 - 1),
             line_i*nof_longs_per_line*(int)SOC_SAND_NOF_BITS_IN_UINT32
             ));
    for(long_i = SOC_SAND_MIN( (line_i+1)*nof_longs_per_line - 1, size-1);
        long_i >= (int)(line_i*nof_longs_per_line);
        --long_i)
    {
      LOG_CLI((BSL_META("%08X"), bit_stream[long_i]));
    }
    LOG_CLI((BSL_META("\r\n")));
  }

exit:
  return;
}


uint32
  soc_sand_bitstream_set_any_field(
    SOC_SAND_IN  uint32    *input_buffer,
    SOC_SAND_IN  uint32    start_bit,
    SOC_SAND_IN  uint32    nof_bits,
    SOC_SAND_OUT uint32    *output_buffer
  )
{
  uint32
    end,
    nof_bits_iter_acc,
    nof_bits_iter,
    nof_words,
    iter;
  uint32
    res = SOC_SAND_OK;
  uint32
    field;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_BITSTREAM_SET_ANY_FIELD);

  SOC_SAND_CHECK_NULL_INPUT(output_buffer);
  SOC_SAND_CHECK_NULL_INPUT(input_buffer);

  end = nof_bits + start_bit - 1;
  if((end / SOC_SAND_NOF_BITS_IN_UINT32) == (start_bit / SOC_SAND_NOF_BITS_IN_UINT32))
  {
    field = SOC_SAND_GET_BITS_RANGE(*input_buffer, nof_bits - 1, 0);
    *(output_buffer + (start_bit / SOC_SAND_NOF_BITS_IN_UINT32)) &= SOC_SAND_ZERO_BITS_MASK(end % SOC_SAND_NOF_BITS_IN_UINT32, start_bit % SOC_SAND_NOF_BITS_IN_UINT32);
    *(output_buffer + (start_bit / SOC_SAND_NOF_BITS_IN_UINT32)) |= SOC_SAND_SET_BITS_RANGE(field, end % SOC_SAND_NOF_BITS_IN_UINT32, start_bit % SOC_SAND_NOF_BITS_IN_UINT32);
  }
  else
  {
    nof_words = SOC_SAND_DIV_ROUND_UP(nof_bits, SOC_SAND_NOF_BITS_IN_UINT32);
    for(
        iter = 0, nof_bits_iter_acc = nof_bits;
        iter < nof_words;
        ++iter, nof_bits_iter_acc -= SOC_SAND_NOF_BITS_IN_UINT32
       )
    {
      nof_bits_iter = nof_bits_iter_acc > SOC_SAND_NOF_BITS_IN_UINT32 ? SOC_SAND_NOF_BITS_IN_UINT32 : nof_bits_iter_acc;

      res = soc_sand_bitstream_set_field(
              output_buffer,
              start_bit + iter * SOC_SAND_NOF_BITS_IN_UINT32,
              nof_bits_iter,
              *(input_buffer + iter)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, iter, exit);
    }
  }

 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_bitstream_set_any_field()",0,0);
}


uint32
  soc_sand_bitstream_get_any_field(
    SOC_SAND_IN  uint32    *input_buffer,
    SOC_SAND_IN  uint32    start_bit,
    SOC_SAND_IN  uint32    nof_bits,
    SOC_SAND_OUT uint32    *output_buffer
  )
{
  uint32
    end,
    nof_bits_iter,
    nof_words,
    iter;
  uint32
    res = SOC_SAND_OK;
  uint32
    field;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_BITSTREAM_GET_ANY_FIELD);

  SOC_SAND_CHECK_NULL_INPUT(input_buffer);
  SOC_SAND_CHECK_NULL_INPUT(output_buffer);

  nof_words = SOC_SAND_DIV_ROUND_UP(nof_bits, SOC_SAND_NOF_BITS_IN_UINT32);

  end = nof_bits + start_bit - 1;
  if((end / SOC_SAND_NOF_BITS_IN_UINT32) == (start_bit / SOC_SAND_NOF_BITS_IN_UINT32))
  {
    
    
    field = SOC_SAND_GET_BITS_RANGE(*(input_buffer + (start_bit / SOC_SAND_NOF_BITS_IN_UINT32)), end % SOC_SAND_NOF_BITS_IN_UINT32, start_bit % SOC_SAND_NOF_BITS_IN_UINT32);

    *output_buffer = 0;
    *output_buffer |= SOC_SAND_SET_BITS_RANGE(field, nof_bits - 1, 0);
  }
  else
  {
    for(
        iter = 0, nof_bits_iter = nof_bits;
        iter < nof_words;
        ++iter, nof_bits_iter -= SOC_SAND_NOF_BITS_IN_UINT32
       )
    {
      res = soc_sand_bitstream_get_field(
              input_buffer,
              start_bit + iter * SOC_SAND_NOF_BITS_IN_UINT32,
              nof_bits_iter > SOC_SAND_NOF_BITS_IN_UINT32 ? SOC_SAND_NOF_BITS_IN_UINT32 : nof_bits_iter,
              output_buffer + iter
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_bitstream_get_any_field()",0,0);
}


void
  soc_sand_bitstream_print_beautify_1(
    SOC_SAND_IN uint32 *bit_stream,
    SOC_SAND_IN uint32 size, 
    SOC_SAND_IN uint32 max_dec_digits,
    SOC_SAND_IN uint32 max_nof_printed_items
  )
{
  uint32
    item_i,
    nof_on_bits,
    bit_i;
  const int
    nof_number_per_line = 8;
  char
    format[20]="";

  if (NULL == bit_stream)
  {
    LOG_CLI((BSL_META("soc_sand_bitstream_print_beautify_1 received NULL ptr\n\r")));
    goto exit;
  }

  if (max_dec_digits>10)
  {
    LOG_CLI((BSL_META("soc_sand_bitstream_print_beautify_1: received 'max_dec_digits>10'\n\r")));
    goto exit;
  }

  nof_on_bits = soc_sand_bitstream_get_nof_on_bits(bit_stream, size);
  LOG_CLI((BSL_META("Out of %u, %u are ON (%u item exists).\r\n"),
size*(int)SOC_SAND_NOF_BITS_IN_UINT32,
           nof_on_bits,
           nof_on_bits
           ));
  sal_sprintf(format, "%%-%uu ", max_dec_digits);
  
  if (nof_on_bits == 0)
  {
    
    goto exit;
  }

  LOG_CLI((BSL_META("[ ")));
  item_i=0;
  for (bit_i=0; bit_i<(size*SOC_SAND_NOF_BITS_IN_UINT32); bit_i++)
  {
    if( item_i >= max_nof_printed_items)
    {
      
      LOG_CLI((BSL_META(" .... More Items to print. Exist .... \r\n")));
      break;
    }
    if ( item_i >= nof_on_bits)
    {
      
      break;
    }
    if (soc_sand_bitstream_test_bit(bit_stream, bit_i) == FALSE)
    {
      continue;
    }
    item_i ++;
    LOG_CLI((format, bit_i)); 
    if ( (item_i%nof_number_per_line) == 0)
    {
      LOG_CLI((BSL_META("\r\n"
                        "  ")));
    }
  }
  LOG_CLI((BSL_META("]\r\n")));

exit:
  return;
}

SOC_SAND_RET
  soc_sand_buff_xor(
    SOC_SAND_INOUT    unsigned  char    *buff1,
    SOC_SAND_IN       unsigned  char    *buff2,
    SOC_SAND_IN       uint32     size 
  )
{
  uint32 i = 0;
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

  if (!buff1)
  {
    soc_sand_ret = 1;
    goto exit;
  }

  if (!buff2)
  {
    soc_sand_ret = 2;
    goto exit;
  }

  for(i=0; i<size; ++i)
  {
    buff1[i] ^= buff2[i];
  }

exit:
  return soc_sand_ret;
}

SOC_SAND_RET
  soc_sand_buff_print_non_zero(
    SOC_SAND_IN unsigned char* buff,
    SOC_SAND_IN uint32 buff_size
  )
{
  uint32
    curr_line = 0,
    curr_col = 0,
    rem_offset = 0,
    only_zer_this_line = TRUE,
    size_in_longs = 0,
    rem_from_long = 0,
    max_nof_lines = 0;
  const uint32
    max_columns = 6; 
  uint32
    curr_offset = 0,
    curr_long = 0;
  const uint32*
    buff_longs = NULL;

  unsigned char
    curr_char = 0;
  const unsigned char*
    buff_chars = NULL;
  char* str_ptr = NULL;
  char str_line[200] = "";

  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;
  char*
    func_name = "soc_sand_buff_print_non_zero";

  str_ptr = str_line;

  if (!buff)
  {
    soc_sand_ret = 1;
    goto exit;
  }

  if (!buff_size)
  {
    soc_sand_ret = 2;
    goto exit;
  }

  buff_longs = (const uint32* )buff;
  buff_chars = buff;

  rem_from_long = buff_size % sizeof(uint32);

  size_in_longs =
    SOC_SAND_DIV_ROUND_DOWN(buff_size, sizeof(uint32));

  if (!rem_from_long)
  {
    max_nof_lines = SOC_SAND_DIV_ROUND_UP(size_in_longs, max_columns);
  }
  else
  {
    max_nof_lines = SOC_SAND_DIV_ROUND_UP(size_in_longs + 1, max_columns);
  }

  for(curr_line = 0; curr_line < max_nof_lines; curr_line++)
  {
    
    only_zer_this_line = TRUE;

    soc_sand_ret =
      soc_sand_os_memset(
        str_line,
        0x0,
        200
       );

    if (soc_sand_ret)
    {
      soc_sand_ret = 7;
      goto exit;
    }

    str_ptr = str_line;

    str_ptr += sal_sprintf(str_ptr, "OFFSET LONGS: %8d ", curr_offset);
    

    for (curr_col = 0; curr_col < max_columns; curr_col++)
    {
      

      if (curr_offset < size_in_longs)
      {
        curr_long = (uint32)( *(buff_longs + curr_offset));
        if (curr_long)
        {
          only_zer_this_line = FALSE;
        }

        str_ptr += sal_sprintf(str_ptr, "0x%8.8X  ", curr_long);
      }
      else 
      {
        if (rem_from_long && (rem_offset < rem_from_long) )
        {
          str_ptr += sal_sprintf(str_ptr, "0x");
          for (rem_offset = 0; rem_offset < rem_from_long; rem_offset++)
          {
            curr_char = (unsigned char)( *(buff_chars + (curr_offset*sizeof(uint32) + rem_offset)));
            if (curr_char)
            {
              only_zer_this_line = FALSE;
            }
            str_ptr += sal_sprintf(str_ptr, "%X", curr_char);
          }
        }
      }

      curr_offset++; 
    }

    sal_sprintf(str_ptr, "\n\r");

    if (!only_zer_this_line)
    {
      LOG_CLI((str_line));
    }
  }

exit:
  if (soc_sand_ret)
  {
    LOG_CLI((BSL_META("ERROR: function %s exited with error num. %d\n\r"),
             func_name,
             soc_sand_ret
             ));
  }

  return soc_sand_ret;
}

SOC_SAND_RET
  soc_sand_buff_print_all(
    SOC_SAND_IN unsigned char* buff,
    SOC_SAND_IN uint32 buff_size,
    uint32 nof_columns
  )
{
  uint32
    curr_line = 0,
    curr_col = 0,
    max_nof_lines = 0;
  uint32
    max_columns = 1; 
  uint32
    curr_offset = 0;
  unsigned char
    curr_char = 0;
  const unsigned char*
    buff_chars = NULL;
  char* str_ptr = NULL;
  char str_line[200] = "";

  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;
  char*
    func_name = "soc_sand_buff_print_non_zero";

  str_ptr = str_line;

  if (!buff)
  {
    soc_sand_ret = 1;
    goto exit;
  }

  if (!buff_size)
  {
    soc_sand_ret = 2;
    goto exit;
  }

  if (nof_columns != 0)
  {
    max_columns = nof_columns * sizeof(uint32);
  }
  buff_chars = buff;


  max_nof_lines = SOC_SAND_DIV_ROUND_UP(buff_size, max_columns);

  LOG_CLI((BSL_META("\n\r"
                    "Buffer of %u Bytes, values given in hexa:\n\r"
                    "-----------------------------------------\n\r"),
           (uint32)buff_size
           ));

  for(curr_line = 0; curr_line < max_nof_lines; curr_line++)
  {
    

    soc_sand_ret =
      soc_sand_os_memset(
        str_line,
        0x0,
        200
       );

    if (soc_sand_ret)
    {
      soc_sand_ret = 7;
      goto exit;
    }

    str_ptr = str_line;

    str_ptr +=
      sal_sprintf(
        str_ptr,
        "OFFSET LONGS: %6d",
        (uint32)(curr_offset / sizeof(uint32)));
    

    for (curr_col = 0; curr_col < max_columns; curr_col++)
    {
      

      if (curr_offset < buff_size)
      {
        curr_char = (unsigned char)( *(buff_chars + curr_offset));
        if (((curr_offset) % sizeof(uint32)) == 0)
        {
            str_ptr += sal_sprintf(str_ptr, " %02X", curr_char);
        }
        else
        {
          str_ptr += sal_sprintf(str_ptr, "%02X", curr_char);
        }
     }
      curr_offset++; 
    }

    sal_sprintf(str_ptr, "\n\r");
    LOG_CLI((str_line));
  }

exit:
  if (soc_sand_ret)
  {
    LOG_CLI((BSL_META("ERROR: function %s exited with error num. %d\n\r"),
             func_name,
             soc_sand_ret
             ));
  }

  return soc_sand_ret;
}


SOC_SAND_RET
  soc_sand_bitsteam_u8_ms_byte_first_get_field(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint8  *input_buffer,
    SOC_SAND_IN  uint32 start_bit_msb,
    SOC_SAND_IN  uint32 nof_bits,
    SOC_SAND_OUT uint32 *output_value
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;
  uint32
    idx,
    buf_sizes=0,
    tmp_output_value[2]={0},
    first_byte_ndx,
    last_byte_ndx;
  uint8
    *tmp_output_value_u8_ptr = (uint8*)&tmp_output_value;
 

  
  if (nof_bits > SOC_SAND_BIT_STREAM_FIELD_SET_SIZE)
  {
    soc_sand_ret = SOC_SAND_BIT_STREAM_FIELD_SET_SIZE_RANGE_ERR;
    goto exit;
  }

  
  first_byte_ndx = start_bit_msb / SOC_SAND_NOF_BITS_IN_BYTE;
  last_byte_ndx = ((start_bit_msb + nof_bits - 1) / SOC_SAND_NOF_BITS_IN_BYTE);*output_value=0;

  for (idx = first_byte_ndx;
       idx <= last_byte_ndx;
       ++idx)
  {
    tmp_output_value_u8_ptr[last_byte_ndx - idx] = input_buffer[idx];
    buf_sizes += SOC_SAND_NOF_BITS_IN_BYTE;
  }

  

 #ifndef LE_HOST
  {
    tmp_output_value[0] = SOC_SAND_BYTE_SWAP(tmp_output_value[0]);
    
    if (last_byte_ndx > 4)
    {
      tmp_output_value[1] = SOC_SAND_BYTE_SWAP(tmp_output_value[1]);
    }
  }
#endif

  soc_sand_ret = soc_sand_bitstream_get_field(
    tmp_output_value,
    buf_sizes-(start_bit_msb%SOC_SAND_NOF_BITS_IN_BYTE+nof_bits),
    nof_bits,
    output_value
    );
  if( soc_sand_ret != SOC_SAND_OK )
  {
    goto exit;
  }

exit:
  return soc_sand_ret;
}


#endif

#include <soc/dpp/SAND/Utils/sand_footer.h>

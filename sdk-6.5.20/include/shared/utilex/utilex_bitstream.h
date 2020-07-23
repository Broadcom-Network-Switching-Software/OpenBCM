/** \file utilex_bitstream.h
 * 
 *
 * All common utilities related to bitstream.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef UTILEX_BITSTREAM_H_INCLUDED
/*
 * {
 */
#define UTILEX_BITSTREAM_H_INCLUDED

/*************
* INCLUDES  *
 */

#include <shared/shrextend/shrextend_debug.h>

/*
 * In 'utilex_bitstream_set_field'/'utilex_bitstream_get_field':
 * 'nof_bits' can be at most 32.
 */
#define UTILEX_BIT_STREAM_FIELD_SET_SIZE    (SAL_UINT32_NOF_BITS)
#define UTILEX_BIT_STREAM_FIELD_MAX_SIZE    (SAL_UINT32_MAX_BIT)

/**
 * \brief
 * Clears an array of bits composed of elements of 32-bits each (bit stream)
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream. \n
 *     \b As \b output - \n
 *       This procedure loads pointed array with zeros.
 *   \param [in] size -
 *     Array length (number of 32-bits elements)
 * \par DIRECT OUTPUT:
 *    Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   \b *bit_stream  - \n
 *   See 'bit_stream' in DIRECT INPUT
 */
shr_error_e utilex_bitstream_clear(
    uint32 *bit_stream,
    uint32 size);
/**
 * \brief
 * Fills a bit stream (array of uint32s) by '1's
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream
 *     \b As \b output - \n
 *       This procedure loads pointed array with ones ('1's).
 *   \param [in] size -
 *     Array length (number of 32-bits elements)
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   \b *bit_stream  - \n
 *   See 'bit_stream' in DIRECT INPUT
 */
shr_error_e utilex_bitstream_fill(
    uint32 *bit_stream,
    uint32 size);

/**
 * \brief
 * Set/clear 1 bit within the bit steream (uint32 array) as per 'bit_indicator'
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream \n
 *     \b As \b output - \n
 *       This procedure sets/clears selected bit (as per 'place' and 'bit_indicator')
 *       in pointed array.
 *   \param [in] place -
 *     Location of bit to load (counted in bits). BIT0 (LS bit) is at
 *     location '0', BIT1 (next bit) is at location '1',...,
 *     BIT32 is at at location '0' of the second uint32.
 *   \param [in] bit_indicator -
 *     Indicator (0 or non-zero) to the value to load into
 *     specified bit.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   \b *bit_stream  - \n
 *   See 'bit_stream' in DIRECT INPUT
 */
shr_error_e utilex_bitstream_set(
    uint32 *bit_stream,
    uint32 place,
    uint32 bit_indicator);

/**
 * \brief
 * Set field onto the bitstream
 *
 * \param [out] bit_stream  -
 *   Pointer to an array of uint32s, which functions as the bit stream.
 *   This procedure loaded the bit field contained in 'field' (as per
 *   'start_bit' and 'nof_bits') in pointed array.
 * \param [in] start_bit -
 *   The first bit number to set (the next one is 'start_bit+1'). See
 *   'place' in utilex_bitstream_set() on bit numbering. Note that
 *   bit number must not exceed length of the whole bit stream.
 * \param [in] nof_bits -
 *   Number of bits to set in 'bit_stream' and to take from 'field'.
 *   Range 0:32.
 * \param [in] field  -
 *   Field to set. The 'nof_bits' LS bits are used as field image
 *   and are copied into the bit stream.
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *  Example:
 * \code{.c}
 *  {
 *    uint32
 *      bit_stream[2];
 *
 *    utilex_bitstream_clear(bit_stream, 2); ==> bit_stream = {0x0, 0x0}
 *    utilex_bitstream_set_field(bit_stream, 4, 16, 0x12345); ==> bit_stream = {0x0, 0x23450}
 *  }
 * \endcode
 * \see
 *   UTILEX_BITSTREAM_FIELD_SIZE
 */
shr_error_e utilex_bitstream_set_field(
    uint32 *bit_stream,
    uint32 start_bit,
    uint32 nof_bits,
    uint32 field);

/**
 * \brief
 * Get field (some number of consequent bits) from the bitstream (array of uints32s)
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream
 *   \param [in] start_bit -
 *     The first bit number to get (the next one is 'start_bit+1')
 *   \param [in] nof_bits -
 *     Number of bits to get from 'bit_stream' and to take from 'field'.
 *     Range 0:32.
 *   \param [in] field -
 *     Pointer to memory of one 'uint32' to be loaded by this procedure. \n
 *     \b As \b output - \n
 *     This procedure loads pointed memory by filed value extracted from
 *     the bit stream..
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   \b *field -  \n
 *     See 'field' in DIRECT INPUT
 * \remark
 *   Example:
 * \code{.c}
 *  {
 *    uint32
 *      bit_stream[2],
 *      field;
 *
 *    bit_stream[0] = 0x23450;
 *    bit_stream[1] = 0x0;   ==> bit_stream = {0x0, 0x23450}
 *    utilex_bitstream_get_field(bit_stream, 8, 12, &field); ==> field is  0x234
 *  }
 * \endcode
 */
shr_error_e utilex_bitstream_get_field(
    uint32 *bit_stream,
    uint32 start_bit,
    uint32 nof_bits,
    uint32 *field);

/**
 * \brief
 * Set 1 bit in pointed uint32 array to 1
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s which functions as the bit stream.
 *     A location within this array is updated by this procedure. \n
 *     \b As \b output - \n
 *       This procedure updates selected bit on this uint32s array
 *   \param [in] place         -
 *     The number of the bit to set (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   \b *bit_stream  - \n
 *     See 'bit_stream' in DIRECT INPUT
 */
shr_error_e utilex_bitstream_set_bit(
    uint32 *bit_stream,
    uint32 place);

/**
 * \brief
 *  set range of bits in the uint32 array to 1
 *  The range is [start_place -- end_place]
 *  INCLUDING end_place.
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream \n
 *     \b As \b output - \n
 *       This procedure updates sets bits, from 'start place' to 'end place',
 *       to 1, on this uint32s array
 *   \param [in] start_place  -
 *     The start bit number to set (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 *   \param [in] end_place    -
 *     The end bit number to set (counted in bits)
 *     This bit is also get set.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   \b *bit_stream  - \n
 *   See 'bit_stream' in DIRECT INPUT above.
 */
shr_error_e utilex_bitstream_set_bit_range(
    uint32 *bit_stream,
    uint32 start_place,
    uint32 end_place);

/**
 * \brief
 *  Set 1 bit in the bitstream (uint32 array) to 0
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream \n
 *     \b As \b output - \n
 *       This procedure clears the specified bit (per 'place') on pointed
 *       bit_stream.
 *   \param [in] place         -
 *     The bit number to reset (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   \b *bit_stream  - \n
 *   See 'bit_stream' in DIRECT INPUT above
 */
shr_error_e utilex_bitstream_reset_bit(
    uint32 *bit_stream,
    uint32 place);

/**
 * \brief
 *  Set range of bits in the uint32 array to 0
 *  The range is [start_place -- end_place]
 *  INCLUDING end_place.
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream
 *     \b As \b output - \n
 *       This procedure clears specified bit ('start_place'->'end_place',
 *       including end) on pointed bit_stream
 *   \param [in] start_place  -
 *     The start bit number to reset (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 *   \param [in] end_place    -
 *     The end bit number to reset (counted in bits)
 *     This bit is also reset.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *  \b *bit_stream  - \n
 *  See 'bit_stream' in DIRECT INPUT above.
 */
shr_error_e utilex_bitstream_reset_bit_range(
    uint32 *bit_stream,
    uint32 start_place,
    uint32 end_place);

/**
 * \brief
 *  Return the value (0/1) of 1 bit in the bitstream  (uint32 array)
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which function as the bit stream
 *   \param [in] place         -
 *     The bit number to fetch (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT:
 *   int - bit value (0 / 1)
 * \par INDIRECT OUTPUT
 *   * None
 */
int utilex_bitstream_test_bit(
    uint32 *bit_stream,
    uint32 place);

/**
 * \brief
 *  Read the bit and auto-clear it.
 *  1. Return the value (0/1) of 1 bit in the uint32 array
 *  2. Set the bit to zero.
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream
 *     \b As \b output - \n
 *       This procedure clears specified bit (per 'place') on pointed bit_stream
 *   \param [in] place         -
 *     The bit number to fetch and reset (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT:
 *   Bit value (0 / 1)
 * \par INDIRECT OUTPUT:
 *   *bit_stream  -
 *  See 'bit_stream' in DIRECT INPUT above.
 */
int utilex_bitstream_test_and_reset_bit(
    uint32 *bit_stream,
    uint32 place);

/**
 * \brief
 *  Read the bit and auto-set it.
 *  1. Return the value (0/1) of 1 bit in the uint32 array
 *  2. Set the bit to one.
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream \n
 *     \b As \b output - \n
 *       This procedure asserts specified bit (per 'place') on pointed bit_stream
 *   \param [in] place         -
 *     The bit number to fetch and set (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT:
 *   Bit value (0 / 1)
 * \par INDIRECT OUTPUT
 *   *bit_stream  -
 *  See 'bit_stream' in DIRECT INPUT above.
 */
int utilex_bitstream_test_and_set_bit(
    uint32 *bit_stream,
    uint32 place);

/**
 * \brief
 *  Flip the value of specific bit (XOR with one)
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream \n
 *     \b As \b output - \n
 *       This procedure flips (0->1, 1->0) specified bit (per 'place') on
 *       pointed bit_stream
 *   \param [in] place         -
 *     The bit number to set (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT:
 *    Non-zero in case of an error
 * \par INDIRECT OUTPUT:
 *   *bit_stream  -
 *  See 'bit_stream' in DIRECT INPUT above.
 */
shr_error_e utilex_bitstream_bit_flip(
    uint32 *bit_stream,
    uint32 place);

/**
 * \brief
 *  Flip the value (XOR with one) of range of bits in the uint32 array
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream \n
 *     \b As \b output - \n
 *       This procedure flips (0->1, 1->0) bits, from 'start place' to 'end place',
 *       on this uint32s array
 *   \param [in] start_place  -
 *     The start bit number to flip (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 *   \param [in] end_place    -
 *     The end bit number to flip (counted in bits)
 *     This bit is also get flipped.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   \b *bit_stream  - \n
 *   See 'bit_stream' in DIRECT INPUT above.
 */
shr_error_e utilex_bitstream_bit_flip_range(
    uint32 *bit_stream,
    uint32 start_place,
    uint32 end_place);
/**
 * \brief
 *  Test whether specified range contains '1' or all bits are '0'.
 *  The range is [start_place -- end_place]
 *  INCLUDING end_place.
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream
 *   \param [in] start_place  -
 *     The start bit number (counted in bits).
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 *   \param [in] end_place    -
 *     The end bit number (counted in bits)
 *     This bit is also checked.
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT:
 *   TRUE if 1 was found within the bit range
 * \par INDIRECT OUTPUT
 *   * None
 * \sa utilex_bitstream_test_bit
 *     utilex_bitstream_have_one
 */
int utilex_bitstream_have_one_in_range(
    uint32 *bit_stream,
    uint32 start_place,
    uint32 end_place);

/**
 * \brief
 *  Test whether any bit is set in the whole bitstream.
 *  (work faster than utilex_bitstream_have_one_in_range())
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream
 *   \param [in] size   -
 *     Number of uint32s in bitstream array
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT:
 *   TRUE if 1 was found within the bit stream
 * \par INDIRECT OUTPUT
 *   * None
 * \sa utilex_bitstream_have_one_in_range
 */
int utilex_bitstream_have_one(
    uint32 *bit_stream,
    uint32 size                 /* in uint32s */
    );

/**
 * \brief
 *  'OR' 2 bitstreams with each other.
 *  The results goes into the first bit stream.
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream1  -
 *     Pointer to an array of uint32s, which functions as the bit stream. \n
 *     \b As \b output - \n
 *       Result of 'OR' operation is written into this pointed bit stream.
 *   \param [in] bit_stream2  -
 *     Pointer to an array of uint32s, which functions as the bit stream
 *   \param [in] size   -
 *     Number of uint32s in each of the bit streams
 * \par INDIRECT INPUT
 *   * *bit_stream1
 *   * *bit_stream2 \n
 *   The two bit streams to OR with each other.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT:
 *   \b *bit_stream1  - \n
 *   See 'bit_stream1' in DIRECT INPUT above.
 */
shr_error_e utilex_bitstream_or(
    uint32 *bit_stream1,
    uint32 *bit_stream2,
    uint32 size                 /* in uint32s */
    );

/**
 * \brief
 *  AND 2 bitstreams. Put the result in the first.
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream1  -
 *     Pointer to an array of uint32s, which functions as the bit stream \n
 *     \b As \b output - \n
 *       Result of 'AND' operation is written into this pointed bit stream.
 *   \param [in] bit_stream2  -
 *     Pointer to an array of uint32s, which functions as the bit stream
 *   \param [in] size   -
 *     Number of uint32s in each of the bit streams
 * \par INDIRECT INPUT
 *   * *bit_stream1
 *   * *bit_stream2 \n
 *   The two bit streams to AND with each other.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT:
 *   \b *bit_stream1  - \n
 *   See 'bit_stream1' in DIRECT INPUT above.
 */
shr_error_e utilex_bitstream_and(
    uint32 *bit_stream1,
    uint32 *bit_stream2,
    uint32 size                 /* in uint32s */
    );

/**
 * \brief
 *  XOR 2 bitstreams. Put the result in the first.
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream1  -
 *     Pointer to an array of uint32s, which functions as the bit stream
 *     \b As \b output - \n
 *       Result of 'XOR' operation is written into this pointed bit stream.
 *   \param [in] bit_stream2  -
 *     Pointer to an array of uint32s, which functions as the bit stream
 *   \param [in] size   -
 *     Number of uint32s in each of the bit streams
 * \par INDIRECT INPUT
 *   * *bit_stream1
 *   * *bit_stream2 \n
 *   The two bit streams to XOR with each other.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT:
 *   *bit_stream1  -
 *   See 'bit_stream1' in DIRECT INPUT above.
 */
shr_error_e utilex_bitstream_xor(
    uint32 *bit_stream1,
    uint32 *bit_stream2,
    uint32 size                 /* in uint32s */
    );

/**
 * \brief
 *  Get the parity of a bitstream (array of uint32s)
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream
 *   \param [in] start_bit -
 *     Specifies the bit from which parity is calculated.
 *     See 'place' in utilex_bitstream_set() on bit numbering. Note that
 *     bit number must not exceed length of the whole bit stream.
 *   \param [in] nof_bits -
 *     Specifies the number of bits from 'start_bit' on
 *     which parity is calculated.
 * \par DIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT:
 *    0 - numbers of 1 in the stream is even.
 *    1 - numbers of 1 in the stream is odd.
 * \par DIRECT OUTPUT
 *   * None
 */
uint32 utilex_bitstream_parity(
    uint32 *bit_stream,
    uint32 start_bit,
    uint32 nof_bits);

/**
 * \brief
 *  Do 'NOT' operation on the bitstream (array of uints)
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream \n
 *     \b As \b output - \n
 *       Result of 'NOT' operation is written into this pointed bit stream.
 *   \param [in] size   -
 *     Number of uint32s in the bit streams
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT INPUT:
 *   None
 * \par INDIRECT OUTPUT:
 *   \b *bit_stream1  - \n
 *   See 'bit_stream1' in DIRECT INPUT above.
 */
shr_error_e utilex_bitstream_not(
    uint32 *bit_stream,
    uint32 size);

/**
 * \brief
 *  Count the number of "ones" in the bit stream (array of uint32s).
 *  Examples:
 *   + 0x0000_0000 ==> will results with 0.
 *   + 0x0100_0000 ==> will results with 1.
 *   + 0x0100_0000 0x0C00_0000 ==> will results with 3.
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream
 *   \param [in] size   -
 *     Number of uint32s in the bit streams
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   The number of ones on the bitstream
 * \par INDIRECT OUTPUT:
 *   * None
 */
uint32 utilex_bitstream_get_nof_on_bits(
    uint32 *bit_stream,
    uint32 size);

/**
 * \brief
 *  Test function of utilex_bitstream.
 *  View prints and return value to indicate pass/fail
 *
 * \par DIRECT INPUT
 *   \param [in] silent  -
 *     Print bitstreams if not silent
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int- \n
 *     * TRUE  - PASS
 *     * FALSE - FAIL
 * \par INDIRECT OUTPUT:
 *   * None
 */
uint32 utilex_bitstream_offline_test(
    uint32 silent);

/**
 * \brief
 *  Printing utility. Print 'size' unit32s from input bitstream
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream
 *   \param [in] size   -
 *     Number of uint32s in the bit stream
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   * None
 * \par INDIRECT OUTPUT:
 *   Printed bitstream
 * \remark
 *   Use LOG_CLI for printing
 */
void utilex_bitstream_print(
    uint32 *bit_stream,
    uint32 size);

/**
 * \brief
 *  Having two buffers, this function writes the second buffer ('output_buffer'),
 *  starting from specified offset, the specified number of bits, taken
 *  from the first buffer ('input_buffer') starting (on the first buffer) from
 *  BIT0.
 *
 * \par DIRECT INPUT
 *   \param [in] input_buffer  -
 *     Pointer to input buffer from which the function reads,
 *     starting at BIT0
 *   \param [in] start_bit
 *     The number of the first bit to start the writing
 *     into the output buffer
 *   \param [in] nof_bits
 *     Number of bits to read / write. If '0' then this procedure returns
 *     quitely without error and without doing anything.
 *   \param [in] output_buffer  -
 *     Pointer to output buffer into which the function writes. \n
 *     \b As \b output - \n
 *       Updated (written into) output buffer. NOTE that writing starts at bit
 *       'start_bit' of '*output_buffer'
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT
 *   \b *output_buffer \n
 *     See 'output_buffer' in DIRECT INPUT above
 * \remark
 *   Example:
 * \code{.c}
 *  {
 *    shr_error_e error_e ;
 *    uint32 input_buffer[2] ;
 *    uint32 output_buffer[2] ;
 *    uint32 start_bit, nof_bits ;
 *
 *    start_bit = 8 ;
 *    nof_bits = 40 ;
 *    input_buffer[0] = 0xAABBCCDD ;
 *    input_buffer[1] = 0xEEFF0011 ;
 *    output_buffer[0] = 0xFFFFFFFF ;
 *    output_buffer[1] = 0xFFFFFFFF ;
 *    error_e =
 *      utilex_bitstream_set_any_field(input_buffer,start_bit,nof_bits,output_buffer) ;
 *    ==> At this point, input_buffer remains as it was (0xEEFF0011AABBCCDD) while
 *    ==> output_buffer now contains 0xFFFF11AABBCCDDFF
 *  }
 * \endcode
 */
shr_error_e utilex_bitstream_set_any_field(
    uint32 *input_buffer,
    uint32 start_bit,
    uint32 nof_bits,
    uint32 *output_buffer);

/**
 * \brief
 *  Having two buffers, the function reads the first buffer ('input_buffer')
 *  from specified offset and writes the data into the second buffer ('output_buffer')
 *  starting at BIT0.
 *  The number of bits read from first buffer equals the number of
 *  bits written into the second buffer.
 *  If some bits are written in part of a uint32 element (on the
 *  second buffer) then this function zeroes all other bits
 *  (from the end of the field to the end of the uint32 element)
 *
 * \par DIRECT INPUT
 *   \param [in] input_buffer  -
 *     Pointer to input buffer from which the function reads, starting
 *     from bit 'start_bit'.
 *   \param [in] start_bit
 *     The first bit to start the reading in the input buffer
 *   \param [in] nof_bits
 *     Number of bits to read / write
 *   \param [in] output_buffer
 *     Pointer to output buffer into which the function writes
 *     \b As \b output - \n
 *       Modified (written into) output buffer. Extracted data is written into
 *       output buffer starting from BIT0. Note that if the number of bits
 *       is not a multiple of 32, then the last uint32 will be filled with
 *       zeroes on the bits which have not been copied from 'input_buffer.
 * \par DIRECT OUTPUT:
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *output_buffer \n
 *   See 'output_buffer' in DIRECT INPUT.
 * \remark
 *   Example:
 * \code{.c}
 *  {
 *    shr_error_e error_e ;
 *    uint32 input_buffer[3] ;
 *    uint32 output_buffer[3] ;
 *    uint32 start_bit, nof_bits ;
 *
 *    start_bit = 8 ;
 *    nof_bits = 40 ;
 *    input_buffer[0] = 0xAABBCCDD ;
 *    input_buffer[1] = 0xEEFF1122 ;
 *    input_buffer[2] = 0x33445566 ;
 *    output_buffer[0] = 0xFFFFFFFF ;
 *    output_buffer[1] = 0xFFFFFFFF ;
 *    output_buffer[2] = 0xFFFFFFFF ;
 *    error_e =
 *      utilex_bitstream_get_any_field(input_buffer,start_bit,nof_bits,output_buffer) ;
 *    ==> At this point, input_buffer remains as it was (0x33445566EEFF1122AABBCCDD) while
 *    ==> output_buffer now contains 0xFFFFFFFF0000001122AABBCC
 *  }
 * \endcode
 */
shr_error_e utilex_bitstream_get_any_field(
    uint32 *input_buffer,
    uint32 start_bit,
    uint32 nof_bits,
    uint32 *output_buffer);

/**
 * \brief
 * Beautify printing utility.
 *  Prints the bit-stream as a GROUP of one-bit items. Each
 *  bit which is non-zero is marked by its location in the
 *  bitstream. The space assigned to display of each bit is
 *  a parameter (max_dec_digits) and the maxiaml number of bits
 *  to display is also a parameter.
 *
 * \par DIRECT INPUT
 *   \param [in] bit_stream  -
 *     Pointer to an array of uint32s, which functions as the bit stream
 *   \param [in] size   -
 *     Number of uint32s in the bit stream
 *   \param [in] max_dec_digits -
 *     Number of spaces assigned for each non-zero bit
 *     which is displayed.
 *   \param [in] max_nof_printed_items -
 *     Maximum number of items (non-zero bits) to print.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   * None
 * \par INDIRECT OUTPUT:
 *   Printed bitstream
 * \remark
 *   This procedures uses LOG_CLI for printing \n
 *   For example: \n
 *   If the bitstream is : \c 0x7FFDEB40EEFF0011AABBCCDD \n
 *   and 'max_dec_digits' is '3' and 'max_nof_printed_items' is '30' \n
 *   then the printout will be: \n
 * \code{.c}
 *     Out of 96, 57 are ON (57 item exists).
 *     [ 0   2   3   4   6   7   10  11
 *       14  15  16  17  19  20  21  23
 *       25  27  29  31  32  36  48  49
 *       50  51  52  53  54  55   .... More Items to print. Exist ....
 *     ]
 * \endcode
 */
void utilex_bitstream_print_beautify_1(
    uint32 *bit_stream,
    uint32 size,
    uint32 max_dec_digits,
    uint32 max_nof_printed_items);

/**
 * \brief
 *  Beautify printing utility.
 *  Prints buffer, which is assumed to be an array of uint32s,
 *  in hex format so that there are specified elements per
 *  line (nof_columns).
 *
 * \par DIRECT INPUT
 *   \param [in] unit  -
 *   \param [in] buff  -
 *     Pointer to an array of uint32s, which functions as the bit stream
 *   \param [in] buff_size   -
 *     Number of bytes in the bit stream. If
 *     it is not a multiple of sizeof(uint32) then
 *     the last uint32 is only partially displayed.
 *   \param [in] nof_columns -
 *     Item max printing size in Decimal.
 * \par INDIRECT INPUT:
 *   *buff  -
 *     Contents of array of uint32s to print
 * \par DIRECT OUTPUT:
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \par INDIRECT OUTPUT:
 *   Printed buff
 * \remark
 *   This procedures uses LOG_CLI for printing \n
 *   For example:                                                 \n
 *   If 'buff_size' is 18 and 'nof_columns' is 4 and the          \n
 *   bitstream is : \c 0xA55A0022CCDDEEFF8899AABB4455667700112233 \n
 * \code{.c}
 *   Buffer of 18 Bytes, values given in hexa:
 *   -----------------------------------------
 *   OFFSET LONGS:      0 00112233 44556677 8899AABB CCDDEEFF
 *   OFFSET LONGS:      4 A55A
 * \endcode
 */
shr_error_e utilex_bitstream_buff_print_all(
    int unit,
    unsigned char *buff,
    uint32 buff_size,
    uint32 nof_columns);

/**
 * \brief
 *  Having two char buffers, orders msB->lsB, this function reads
 *  the first buffer ('input_buffer') from offset (offset taken from
 *  msb) and writes it to the second buffer ('output_value').
 *
 * \par DIRECT INPUT
 *   \param [in] unit  -
 *   \param [in] input_buffer  -
 *     Pointer to an array of uint8s, which functions as the bit stream
 *   \param [in] start_bit_msb   -
 *     offset
 *   \param [in] nof_bits -
 *   \param [in] output_value  -
 *     Pointer to an array of uint32s, which is to contain the result \n
 *     \b As \b output - \n
*        Updated (written into) buffer with newly extracted bit stream.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error
 * \par INDIRECT OUTPUT:
 *   \b *output_value - \n
 *     See 'output_value' in DIRECT INPUT
 */
shr_error_e utilex_bitstream_u8_ms_byte_first_get_field(
    int unit,
    uint8 *input_buffer,
    uint32 start_bit_msb,
    uint32 nof_bits,
    uint32 *output_value);

/**
 * \brief
 *  Having a buffer, the function add 1 (arithmetically) to the
 *  buffer
 *  The 1 is added to the LSB (bit #0) of the buffer
 *  In case of overflow detection, an error will be returned
 * \param [in,out] bit_stream  -
 *   Pointer to an array of uint32s \n
 *   \b As \b input -\n
 *     Points to the bit stream array to act on \n
 *   \b As \b output -\n
 *     This procedure adds the '1' to the LS uint32 (bit_stream[0])
 * \param [in] size -
 *   The size of the array in words (uint32s)
 * \return
 *   Standard error handling.
 */
shr_error_e utilex_bitstream_add_one(
    uint32 *bit_stream,
    uint32 size);

/**
 * \brief
 *  Verify that all the bits outside of the mask are cleared.
 *  If not throw an error.
 *  For example:
 *  * data_bit_stream = 0x100 mask_bit_stream = 0xf00 will return _SHE_E_NONE
 *  * data_bit_stream = 0x1100 mask_bit_stream = 0xf00 will return _SHE_E_PARAM
 *
 * \par DIRECT INPUT
 *   \param [in] data_bit_stream  -
 *     Pointer to an array of uint32s
 *   \param [in] mask_bit_stream  -
 *     Pointer to an array of uint32s
 *   \param [in] size -
 *     The size of the array in words (uint32s)
 * \par INDIRECT INPUT:
 *   * See bit_stream above
 * \par DIRECT OUTPUT:
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT:
 *   * See bit_stream above
 */
shr_error_e utilex_bitstream_mask_verify(
    uint32 *data_bit_stream,
    uint32 *mask_bit_stream,
    uint32 size);

/**
 * \brief -
 * Given bitstream, return number of bits up to, and including, the most significant non-zero bit.
 * Note, the LS bit of the bitstream is the LS bit of data_bit_stream[0].
 *
 * Equivalent to the integer round up of log2(data_bit_stream)
 * If array is 0 return 0.
 * Examples:
 * {0x0, 0x1} : size is 1
 * {0x0, 0x10} : size is 5
 * {0x10, 0x0} : size is 37 (32+5)
 *
 * \par DIRECT_INPUT:
 *   \param [in] data_bit_stream -
 *     uint32 array with length size
 *   \param [in] size -
 *     The size of the array in words (uint32s), should be positive
 *   \param [out] bit_counter -
 *     number of bits in the array
 * \return
 *   Standard error handling.
 * \see
 */
shr_error_e utilex_bitstream_size_of_bitstream_in_bits(
    uint32 *data_bit_stream,
    uint32 size,
    uint32 *bit_counter);

/**
 * \brief - Utility function to swap the positions of two fields in a bitstream.
 *          The nof_msbits is used to get the MSB field from the bitstream,
 *          The nof_Lsbits is used to get the LSB field from the bitstream and
 *          then the positions of the two fields are swapped.
 *          The resulting will be:
 *          (field_lsb << nof_msb) | (field_msb >> nof_lsb)
 *          The sum nof_mbits + nof_lbits can not be more than 32.
 *
 * \param [in]  in_bit_stream - input bitstream, max size is 32 bits
 * \param [in]  nof_msbits - number of MSB that will be used to get the MSB field from the bitstream
 * \param [in]  nof_lsbits - number of LSB that will be used to get the LSB field from the bitstream
 * \param [out] out_bit_stream - output bitstream
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_bitstream_field_swap(
    uint32 *in_bit_stream,
    uint32 nof_msbits,
    uint32 nof_lsbits,
    uint32 *out_bit_stream);

/**
 * \brief
 *  Check if two bitstreams are equal.
 *
 * \par DIRECT INPUT
 *   \param [in] data_bit_stream1  -
 *     Pointer to an array of uint32s
 *   \param [in] data_bit_stream2  -
 *     Pointer to an array of uint32s
 *   \param [in] size -
 *     The size of the array in words (uint32s)
 *   \param [out] is_equal -
 *     Indication is the two bitstreams are equal
 * \par INDIRECT OUTPUT:
 *   * See bit_stream above
 */
void utilex_bitstream_compare(
    uint32 *data_bit_stream1,
    uint32 *data_bit_stream2,
    uint32 size,
    uint8 *is_equal);
/*
 * }
 */
/** UTILEX_BITSTREAM_H_INCLUDED */
#endif

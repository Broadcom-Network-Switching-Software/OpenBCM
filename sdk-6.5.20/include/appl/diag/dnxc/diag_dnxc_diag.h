/**
 *  \file diag_dnxc_diag.h 
 * structure, definition and utilities for diagnostic pack 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNXC_DIAG_H_INCLUDED
#define DIAG_DNXC_DIAG_H_INCLUDED
#include <soc/mcm/enum_max.h>
/* 
* { Local Defines
*/

#define BLOCK_MAX_INDEX (SOC_MAX_NUM_BLKS)   /* defined by MCM release */
#define NOF_BITS_PER_WORD (32)
#define FULL_32_BITS_MASK (0xffffffff)
#define ASCII_CODE_OF_0 (48)
#define ASCII_CODE_OF_9 (57)
#define BLOCK_NAME_LENGTH 6     /* Length of block name */
/*
 * Maximal number of characters assigned to uint64 when converted to string,
 * as decimal, including separateing commas, every 3 digits. Includes ending NULL
 * XX,XXX,XXX,XXX,XXX,XXX,XXX
 * Note that this definition may also be used, of course, as container for Hex
 * conversion.
 */
#define MAX_UINT64_SIZE_AS_STRING 27

/* 
* } Local Defines
*/

/**
 * \brief
 *   translate hex value to a string"
 * \param [in] val - hex value stored in the uint32 array
 * \param [in] nof_bit - number of bits this decimal value occupies 
 * \param [out] buf - buffer to store the string
 * \return 
 *      Standard error handling
 * \see
 *   MAX_UINT64_SIZE_AS_STRING
 */
shr_error_e format_value_hex_to_str(
    uint32 *val,
    int nof_bit,
    char *buf);

/**
 * \brief
 *   translate decimal (max 64 bits) or hex value  to a string"
 * \param [in] val - the value stored in the uint32 array
 * \param [in] nof_bit - number of bits this decimal value occupies 
 * \param [in] in_hex - indicate decimal or hex string is required 
 * \param [out] buf - buffer to store the string
 * \return 
 *      Standard error handling
 * \see
 *   MAX_UINT64_SIZE_AS_STRING
 */
shr_error_e format_value_to_str(
    uint32 *val,
    int nof_bit,
    int in_hex,
    char *buf);

/**
 * \brief
 *   add two values together and then translate the sum (decimal or hex) to a string
 *   the supported maximal value is 64 bits.
 * \param [in] val_1 - value 1 stored in the uint32 array
 * \param [in] val_2 - value 2 stored in the uint32 array
 * \param [in] nof_bit - number of bits this decimal value occupies 
 * \param [in] in_hex - indicate decimal or hex string is required 
 * \param [out] buf - buffer to store the string
 * \param [out] overflow - indicates if sum of val_1+val_2 has overflow 
 * \return 
 *      Standard error handling
 */
shr_error_e sum_value_to_str(
    uint32 *val_1,
    uint32 *val_2,
    int nof_bit,
    int in_hex,
    char *buf,
    int *overflow);

/**
 * \brief
 *   To start gtimer of the blocks specified by block index array
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks contained in block_ids array
 * \param [in] block_ids - an array to contain the blockes
 * \param [in] interval - indicate how long gtimer keeps enabled (usec)
 * \return
 *      Standard error handling
 */
shr_error_e sh_dnxc_diag_block_gtimer_start(
    int unit,
    int nof_block,
    int *block_ids,
    int interval);

/**
 * \brief
 *   To stop gtimer of the blocks specified by block index array
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks contained in block_ids array
 * \param [in] block_ids - an array to contain the blockes
 * \return
 *      Standard error handling
 */
shr_error_e sh_dnxc_diag_block_gtimer_stop(
    int unit,
    int nof_block,
    int *block_ids);

/**
 * \brief
 *   Check if the register match the blocks specified by "*match_n"
 * \param [in] unit - Unit #
 * \param [in] match_n - to specify block name that block should match with
 * \param [out] nof_block - to get number of blocks that this register matches with
 * \param [out] block_ids - an integer array to get all of block ID that this register matches with
 * \return
 *      Standard error handling
 */
shr_error_e counter_block_name_match(
    int unit,
    char *match_n,
    int *nof_block,
    int *block_ids);

/**
 * \brief
 *   To get the counter value and overflow value.
 * \param [in] unit - Unit #
 * \param [in] interval - indicate the interval of g-timer (second)
 * \param [in] reg - The register id of counter
 * \param [in] reg_value - The value of register
 * \param [in] cnt_field - The field of counter
 * \param [in] cnt_field_len - The field length of counter.
 * \param [out] fld_value - The counter value
 * \param [out] ovf_value - The overflow field value
 * \return
 *      Standard error handling
 */
shr_error_e sh_dnxc_get_cnt_and_ovf_value(
    int unit,
    uint32 interval,
    soc_reg_t reg,
    soc_reg_above_64_val_t reg_value,
    soc_field_t cnt_field,
    uint32 cnt_field_len,
    soc_reg_above_64_val_t fld_value,
    uint32 *ovf_value);
/* 
* } Local Functions
*/

/* 
* { Local Structures
*/

/* 
* } Local Structures
*/

extern sh_sand_man_t Sh_dnxc_diag_ser_man;
extern sh_sand_cmd_t Sh_dnxc_diag_ser_cmds[];
#endif /* DIAG_DNXC_DIAG_H_INCLUDED */

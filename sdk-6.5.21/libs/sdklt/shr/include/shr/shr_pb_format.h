/*! \file shr_pb_format.h
 *
 * Format APIs for print buffer support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_PB_FORMAT_H
#define SHR_PB_FORMAT_H

#include <sal/sal_types.h>

#include <shr/shr_bitop.h>
#include <shr/shr_pb.h>

/*!
 * \brief Format the value in uint32_t array to print buffer.
 *
 * Format the scalar data stored in uint32_t array \c val with
 * array size \c count to a print buffer \c pb.
 * With this format, each 32-bit word in a long scalar data
 * will be separated by an underscore for better readability.
 *
 * If \c bitwidth is non-zero, the format will be left-justified with
 * the leading zeros to match the specified bit width \c bitwidth.
 *
 * \param [out] pb Print buffer object which is already created.
 * \param [in] prefix Prefix string to be added if not NULL.
 * \param [in] val Pointer to the uint32_t array.
 * \param [in] count Array size.
 * \param [in] bitwidth Width in bit to specify the format string length.
 *
 * \return Pointer to the current string, or "<PBERR>" if error.
 */
const char *
shr_pb_format_uint32(shr_pb_t *pb, const char *prefix,
                     const uint32_t *val, int count, uint32_t bitwidth);

/*!
 * \brief Format the value in uint64_t array to print buffer.
 *
 * Format the scalar data stored in uint64_t array \c val with
 * array size \c count to a print buffer \c pb.
 * With this format, each 32-bit word in a long scalar data
 * will be separated by an underscore for better readability.
 *
 * If \c bitwidth is non-zero, the format will be left-justified with
 * the leading zeros to match the specified bit width \c bitwidth.
 *
 * \param [out] pb Print buffer object which is already created.
 * \param [in] prefix Prefix string to be added if not NULL.
 * \param [in] val Pointer to the uint64_t array.
 * \param [in] count Array size.
 * \param [in] bitwidth Width in bit to specify the format string length.
 *
 * \return Pointer to the current string, or "<PBERR>" if error.
 */
const char *
shr_pb_format_uint64(shr_pb_t *pb, const char *prefix,
                     const uint64_t *val, int count, uint32_t bitwidth);

/*!
 * \brief Format the bit array to print buffer.
 *
 * Format the bit list stored in SHR_BITDCL array \c bits with
 * array size \c count as a set of ranges into a print buffer \c pb.
 * With this format, each continuous range will be separated by comma.
 *
 * \param [out] pb Print buffer object which is already created.
 * \param [in] bits Pointer to the bit array.
 * \param [in] count Array size.
 *
 * \return Pointer to the current string.
 */
const char *
shr_pb_format_bit_list(shr_pb_t *pb, const SHR_BITDCL *bits, int count);

#endif  /* !SHR_PB_FORMAT_H */

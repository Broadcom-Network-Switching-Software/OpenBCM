/*! \file shr_crc.h
 *
 * This file contains APIs for CRC16 and CRC32 hash computing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_CRC_H
#define SHR_CRC_H

/*!
 * \brief Compute CRC16 for byte-aligned data.
 *
 * This function computes CRC16 for the given data stored in a buffer.
 * The polynomial used in the computing is (x ^ 16 + x ^ 15 + x ^ 2 + 1).
 * The data used for the CRC computing is byte-aligned.
 * \param [in] start Start value of the CRC computing.
 * \param [in] p Pointer where the given data is placed.
 * \param [in] n Number of bytes of the data.
 * \return The result of the CRC computing.
 */
extern uint16_t
shr_crc16(int start, uint8_t *p, int n);

/*!
 * \brief Compute CRC16 for non-byte-aligned data.
 *
 * This function computes CRC16 for the given data stored in a buffer.
 * The polynomial used in the computing is (x ^ 16 + x ^ 15 + x ^ 2 + 1).
 * The data used for the CRC computing is non-byte-aligned.
 * \param [in] crc Start value of the CRC computing
 * \param [in] data Pointer where the given data is placed.
 * \param [in] nbits Number of bits of the data.
 * \return The result of the CRC computing.
 */
extern uint16_t
shr_crc16b(int crc, uint8_t *data, int nbits);

/*!
 * \brief Compute CRC32 for byte-aligned data.
 *
 * This function computes CRC32 value for the given data stored in a buffer.
 * The polynomial used in the computing is (x ^ 32 + x ^ 28 + x ^ 23 + x ^ 22 +
                                            x ^ 16 + x ^ 12 + x ^ 11 + x ^ 10 +
                                            x ^ 8 + x ^ 7 + x ^ 5 + x ^ 4 +
                                            x ^ 2  + x ^ 1 + 1).
 * The data used for the CRC computing is byte aligned.
 * \param [in] crc Start value of the CRC computing.
 * \param [in] data Pointer where the given data is placed.
 * \param [in] len Number of bytes of the data.
 * \return the result of the CRC computing.
 */
extern uint32_t
shr_crc32(uint32_t crc, uint8_t *data, int len);

/*!
 * \brief Compute CRC32 for non-byte-aligned data.
 *
 * This function computes CRC32 value for the given data stored in a buffer.
 * The polynomial used in the computing is (x ^ 32 + x ^ 28 + x ^ 23 + x ^ 22 +
                                            x ^ 16 + x ^ 12 + x ^ 11 + x ^ 10 +
                                            x ^ 8 + x ^ 7 + x ^ 5 + x ^ 4 +
                                            x ^ 2  + x ^ 1 + 1).
 * The data used for the CRC computing is not-byte-aligned.
 * \param [in] crc Start value of the CRC computing.
 * \param [in] data Pointer where the given data is placed.
 * \param [in] nbits Number of bits of the data.
 * \return The result of the CRC computing.
 */
extern uint32_t
shr_crc32b(uint32_t crc, uint8_t *data, int nbits);

/*!
 * \brief Reverse the bits in an 16 bit short.
 *
 * This function reverses the bits in an 16 bit short.
 * \param [in] n The data to be reversed.
 * \return The result of the bit reversing.
 */
extern uint16_t
shr_bit_rev16(uint16_t n);

/*!
 * \brief Reverse the bits in each byte of a 32 bit long.
 *
 * This function reverses the bits in each byte of a 32 bit long data.
 * \param [in] n The data to be reversed.
 * \return The result of the bit reversing.
 */
extern uint32_t
shr_bit_rev_by_byte_word32(uint32_t n);

#endif  /* !SHR_CRC_H */

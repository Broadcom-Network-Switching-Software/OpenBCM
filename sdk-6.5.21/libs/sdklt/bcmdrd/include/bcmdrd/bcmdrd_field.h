/*! \file bcmdrd_field.h
 *
 * Field utility APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMDRD_FIELD_H
#define BCMDRD_FIELD_H

#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Get field (little endian word order).
 *
 * Extract field from multi-word entry in little endian order.
 *
 * \param [in] entbuf Entry buffer.
 * \param [in] sbit Start bit of field.
 * \param [in] ebit End bit of field.
 * \param [out] fbuf Field buffer for output.
 *
 * \return Pointer to \c fbuf.
 */
extern uint32_t *
bcmdrd_field_get(const uint32_t *entbuf,
                 int sbit, int ebit, uint32_t *fbuf);

/*!
 * \brief Set field (little endian word order).
 *
 * Encode field into multi-word entry in little endian order.
 *
 * \param [in] entbuf Entry buffer.
 * \param [in] sbit Start bit of field.
 * \param [in] ebit End bit of field.
 * \param [in] fbuf Field buffer.
 *
 * \return Nothing.
 */
extern void
bcmdrd_field_set(uint32_t *entbuf,
                 int sbit, int ebit, uint32_t *fbuf);

/*!
 * \brief Get 32-bit field (little endian word order).
 *
 * Extract a field of 32 or fewer bits from multi-word entry in little
 * endian order.
 *
 * \param [in] entbuf Entry buffer.
 * \param [in] sbit Start bit of field.
 * \param [in] ebit End bit of field.
 *
 * \return Field value.
 */
extern uint32_t
bcmdrd_field32_get(const uint32_t *entbuf,
                   int sbit, int ebit);

/*!
 * \brief Set 32-bit field (little endian word order).
 *
 * Encode a field of 32 or fewer bits into multi-word entry in little
 * endian order.
 *
 * \param [in] entbuf Entry buffer.
 * \param [in] sbit Start bit of field.
 * \param [in] ebit End bit of field.
 * \param [in] fval Field value.
 *
 * \return Nothing.
 */
extern void
bcmdrd_field32_set(uint32_t *entbuf,
                   int sbit, int ebit, uint32_t fval);

/*!
 * \brief Get field (big endian word order).
 *
 * Extract field from multi-word entry in big endian order.
 *
 * \param [in] entbuf Entry buffer.
 * \param [in] wsize Size of entry buffer in 32-bit words.
 * \param [in] sbit Start bit of field.
 * \param [in] ebit End bit of field.
 * \param [out] fbuf Field buffer for output.
 *
 * \return Pointer to \c fbuf.
 */
extern uint32_t *
bcmdrd_field_be_get(const uint32_t *entbuf, int wsize,
                    int sbit, int ebit, uint32_t *fbuf);

/*!
 * \brief Set field (big endian word order).
 *
 * Encode field into multi-word entry in big endian order.
 *
 * \param [in] entbuf Entry buffer.
 * \param [in] wsize Size of entry buffer in 32-bit words.
 * \param [in] sbit Start bit of field.
 * \param [in] ebit End bit of field.
 * \param [in] fbuf Field buffer.
 *
 * \return Nothing.
 */
extern void
bcmdrd_field_be_set(uint32_t *entbuf, int wsize,
                    int sbit, int ebit, uint32_t *fbuf);

/*!
 * \brief Get 32-bit field (big endian word order).
 *
 * Extract a field of 32 or fewer bits from multi-word entry in big
 * endian order.
 *
 * \param [in] entbuf Entry buffer.
 * \param [in] wsize Size of entry buffer in 32-bit words.
 * \param [in] sbit Start bit of field.
 * \param [in] ebit End bit of field.
 *
 * \return Field value.
 */
extern uint32_t
bcmdrd_field32_be_get(const uint32_t *entbuf, int wsize,
                      int sbit, int ebit);

/*!
 * \brief Set 32-bit field (big endian word order).
 *
 * Encode a field of 32 or fewer bits into multi-word entry in big
 * endian order.
 *
 * \param [in] entbuf Entry buffer.
 * \param [in] wsize Size of entry buffer in 32-bit words.
 * \param [in] sbit Start bit of field.
 * \param [in] ebit End bit of field.
 * \param [in] fval Field value.
 *
 * \return Nothing.
 */
extern void
bcmdrd_field32_be_set(uint32_t *entbuf, int wsize,
                      int sbit, int ebit, uint32_t fval);

#endif /* BCMDRD_FIELD_H */

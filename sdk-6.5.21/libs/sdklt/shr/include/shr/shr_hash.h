/*! \file shr_hash.h
 *
 * Various hash algorithms.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_HASH_H
#define SHR_HASH_H

#include <shr/shr_types.h>

/*!
 * \brief Calculate Unix ELF hash.
 *
 * This function is an implementation of the Unix ELF hash. It is an
 * efficient implementation that requires \c data_len loops.
 *
 * \param [in] data Data buffer for which to calculate a hash value.
 * \param [in] data_len Size of data buffer.
 *
 * \return Hash value for the data buffer.
 */
extern uint32_t
shr_hash_elf(const uint8_t *data, size_t data_len);

#endif /* SHR_HASH_H */

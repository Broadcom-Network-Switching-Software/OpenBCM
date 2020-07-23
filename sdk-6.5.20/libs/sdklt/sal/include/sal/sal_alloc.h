/*! \file sal_alloc.h
 *
 * Memory allocation API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_ALLOC_H
#define SAL_ALLOC_H

#include <sal/sal_types.h>

/*!
 * \brief Allocate a block of memory.
 *
 * The allocated memory is assumed to be uninitialized.
 *
 * \param [in] sz Number of bytes to allocate.
 * \param [in] s Text string for tracking purposes.
 *
 * \return Pointer to allocated memory block, or NULL on error.
 */
extern void *
sal_alloc(size_t sz, char *s);

/*!
 * \brief Free a block of memory.
 *
 * Free a block of memory allocated by \ref sal_alloc.
 *
 * \param [in] addr Pointer to memory block to be freed.
 *
 * \return Nothing.
 */
extern void
sal_free(void *addr);

#endif /* SAL_ALLOC_H */

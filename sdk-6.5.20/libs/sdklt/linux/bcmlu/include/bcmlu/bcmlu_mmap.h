/*! \file bcmlu_mmap.h
 *
 * System-specific wrapper for various mmap APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLU_MMAP_H
#define BCMLU_MMAP_H

#include <inttypes.h>

/*!
 * \brief Return name of mmap API function.
 *
 * This function is intended for debugging purposes only, and allows
 * the application to optionally print the name of the mmap API being
 * used.
 *
 * \return Name of mmap API being used.
 */
extern const char *
bcmlu_mmap_type_str(void);

/*!
 * \brief Private version of the mmap system call.
 *
 * Provided to allow us to map kernel memory using a different device
 * than /dev/mem and to support 64-bit physical addresses from 32-bit
 * applications.
 *
 * \param [in] fd File descriptor for kernel device.
 * \param [in] offset Physical address to map.
 * \param [in] size Size of memory to map.
 *
 * \retval 0 No errors.
 * \retval -1 Unable to map memory.
 */
extern void *
bcmlu_mmap(int fd, uint64_t offset, size_t size);

/*!
 * \brief Unmap kernel memory.
 *
 * Unmap memory previously mapped using \ref bcmlu_mmap.
 *
 * \param [in] addr Logical address of mapped memory.
 * \param [in] size Size of memory block to unmap.
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong.
 */
extern int
bcmlu_munmap(void *addr, size_t size);

#endif /* BCMLU_MMAP_H */

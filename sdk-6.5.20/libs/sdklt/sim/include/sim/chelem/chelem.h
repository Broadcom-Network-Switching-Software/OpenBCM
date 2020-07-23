/*! \file chelem.h
 *
 * CHip ELEMent API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef CHELEM_H
#define CHELEM_H

#include <sal/sal_libc.h>

/*!
 * \brief Clear all emulation chip memories.
 *
 * \return nothing.
 */
extern void
chelem_clear_all(void);


/*!
 * \brief Read emulation chip memory.
 *
 * \param [in] unit Device unit.
 * \param [in] addrx Address extension.
 * \param [in] addr Base address.
 * \param [out] data Buffer for data entry.
 * \param [in] size Size of buffer in bytes.
 *
 * \return 0.
 */
extern int
chelem_read(int unit, uint32_t addrx, uint32_t addr,
            void *data, size_t size);

/*!
 * \brief Write emulation chip memory.
 *
 * \param [in] unit Device unit.
 * \param [in] addrx Address extension.
 * \param [in] addr Base address.
 * \param [in] data Buffer for data entry.
 * \param [in] size Size of buffer in bytes.
 *
 * \return 0 if ok, otherwise -1.
 */
extern int
chelem_write(int unit, uint32_t addrx, uint32_t addr,
             void *data, size_t size);

#endif /* CHELEM_H */

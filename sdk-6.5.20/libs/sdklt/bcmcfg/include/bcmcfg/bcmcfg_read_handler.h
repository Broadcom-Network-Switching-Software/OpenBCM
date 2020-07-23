/*! \file bcmcfg_read_handler.h
 *
 * BCMCFG read handler registration utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCFG_READ_HANDLER_H
#define BCMCFG_READ_HANDLER_H

#include <bcmcfg/bcmcfg_types.h>

/*!
 * \brief Register BCMCFG read handler.
 *
 * Register the reader.
 *
 * \param [in]  handler    Reader handler.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_handler_register(const bcmcfg_read_handler_t *handler);

/*!
 * \brief Unregister BCMCFG read handler.
 *
 * Unregister the reader.
 *
 * \param [in]  handler    Reader handler.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_handler_unregister(const bcmcfg_read_handler_t *handler);

#endif /* BCMCFG_READ_HANDLER_H */

/*! \file bcmcfg_read_util.h
 *
 * Utility functions for bcmcfg reader.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCFG_READ_UTIL_H
#define BCMCFG_READ_UTIL_H

#include <sal/sal_types.h>
#include <bcmcfg/bcmcfg_types.h>

/*!
 * \brief Check if bcmcfg reader is in skip mode.
 *
 * If bcmcfg reader in skip mode, it won't process
 * YAML event until skip mode will be cleared.
 *
 * \param [in]  context Reader context.
 *
 * \retval true Skip mode.
 * \retval false Non skip mode.
 */
extern bool
bcmcfg_is_read_skip(bcmcfg_read_context_t *context);

/*!
 * \brief Set bcmcfg reader skip mode.
 *
 * \param [in]  context Reader context.
 */
extern void
bcmcfg_read_skip_set(bcmcfg_read_context_t *context);

/*!
 * \brief Clear bcmcfg reader skip mode.
 *
 * \param [in]  context Reader context.
 */
extern void
bcmcfg_read_skip_clear(bcmcfg_read_context_t *context);

/*!
 * \brief Set skip-document mode.
 *
 * Skip all events until next document start event.
 */
extern void
bcmcfg_read_skip_document_set(void);

/*!
 * \brief Clear skip-document mode.
 */
extern void
bcmcfg_read_skip_document_clear(void);

#endif /* BCMCFG_READ_UTIL_H */

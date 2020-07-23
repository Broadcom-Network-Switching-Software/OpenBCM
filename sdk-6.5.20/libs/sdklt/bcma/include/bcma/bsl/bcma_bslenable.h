/*! \file bcma_bslenable.h
 *
 * System log enable state database.
 *
 * This database is used by the BSL "check" hook to determine which
 * log messages should be sent to the BSL output hook.
 *
 * A CLI command can be used to manipulate the contents of the
 * database.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BSLENABLE_H
#define BCMA_BSLENABLE_H

#include <bsl/bsl_ext.h>

/*!
 * \brief Check if a source is valid for a given layer.
 *
 * BSL sources are shared across BSL layers, and this function can be
 * used by a CLI command to determine if a BSL source is valid for a
 * given layer.
 *
 * \param [in] layer BSL Layer.
 * \param [in] source BSL source.
 *
 * \retval 1 Source is valid for this layer.
 * \retval 0 Source is not valid for this layer.
 */
extern int
bcma_bslenable_source_valid(bsl_layer_t layer, bsl_source_t source);

/*!
 * \brief Set severity level for a given BSL layer and source.
 *
 * Any log message from the given BSL layer and source will be shown
 * if the severity is less than or equal to the currently configured
 * severity level.
 *
 * \param [in] layer BSL Layer.
 * \param [in] source BSL source.
 * \param [in] severity BSL severity.
 */
extern void
bcma_bslenable_set(bsl_layer_t layer, bsl_source_t source, bsl_severity_t severity);

/*!
 * \brief Get current severity level for a given BSL layer and source.
 *
 * This function is tpyically called by the BSL "check" hook to
 * determine if a log message should appear.
 *
 * \param [in] layer BSL Layer.
 * \param [in] source BSL source.
 *
 * \return BSL severity level for the given BSL layer and source.
 */
extern bsl_severity_t
bcma_bslenable_get(bsl_layer_t layer, bsl_source_t source);

/*!
 * \brief Set default severity level for the given BSL layer and source.
 *
 * \param [in] layer BSL Layer.
 * \param [in] source BSL source.
 *
 * \return Nothing.
 */
extern void
bcma_bslenable_reset(bsl_layer_t layer, bsl_source_t source);

/*!
 * \brief Set default severity level for all BSL layers and sources.
 *
 * \return Nothing.
 */
extern void
bcma_bslenable_reset_all(void);

/*!
 * \brief Initialize BSL enable database.
 *
 * This function will also set the default severity level for all BSL
 * layers and sources.
 *
 * \retval 0 No errors
 */
extern int
bcma_bslenable_init(void);

#endif /* BCMA_BSLENABLE_H */

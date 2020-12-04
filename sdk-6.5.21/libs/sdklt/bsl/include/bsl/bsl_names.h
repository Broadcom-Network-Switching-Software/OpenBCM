/*! \file bsl_names.h
 *
 * Broadcom System Log (BSL)
 *
 * BSL globals for EXTERNAL (application) usage.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BSL_NAMES_H
#define BSL_NAMES_H

#include <bsl/bsl_types.h>

/*!
 * \brief Convert numberic layer value to ASCII string.
 *
 * \param [in] layer Numeric layer value.
 *
 * \return Pointer to layer string or "<none>" if not found.
 */
extern const char *
bsl_layer2str(bsl_layer_t layer);

/*!
 * \brief Convert numberic source value to ASCII string.
 *
 * \param [in] source Numeric source value.
 *
 * \return Pointer to source string or "<none>" if not found.
 */
extern const char *
bsl_source2str(bsl_source_t source);

/*!
 * \brief Convert numberic severity value to ASCII string.
 *
 * \param [in] severity Numeric severity value.
 *
 * \return Pointer to severity string or "<none>" if not found.
 */
extern const char *
bsl_severity2str(bsl_severity_t severity);

#endif /* BSL_NAMES_H */

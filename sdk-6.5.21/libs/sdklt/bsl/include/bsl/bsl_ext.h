/*! \file bsl_ext.h
 *
 * Broadcom System Log (BSL)
 *
 * BSL functions for EXTERNAL (application) usage.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BSL_EXT_H
#define BSL_EXT_H

#include <bsl/bsl_types.h>

/*!
 * \brief Initialize a \ref bsl_config_t structure.
 *
 * \param [out] config Configuration structure.
 */
extern void
bsl_config_t_init(bsl_config_t *config);

/*!
 * \brief Initialize BSL logging system.
 *
 * \param [in] config BSL configuration structure.
 *
 * \return Always 0.
 */
extern int
bsl_init(bsl_config_t *config);

#endif /* BSL_EXT_H */

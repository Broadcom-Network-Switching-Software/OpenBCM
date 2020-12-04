/*! \file envcmd_internal.h
 *
 * CLI Environment commands.
 *
 * Definitions intended for library internal use only.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ENVCMD_INTERNAL_H
#define ENVCMD_INTERNAL_H

#include <bcma/env/bcma_env.h>

/*!
 * \brief Show list of environment variables.
 *
 * For internal use only.
 *
 * \param[in] eh Environment handle
 * \param[in] hdr Caption for list output
 * \param[in] scope Scope value for var list indication
 *
 * \return Nothing
 */
extern void
bcma_envcmd_show_var_list(bcma_env_handle_t eh, const char *hdr, int scope);

#endif /* ENVCMD_INTERNAL_H */

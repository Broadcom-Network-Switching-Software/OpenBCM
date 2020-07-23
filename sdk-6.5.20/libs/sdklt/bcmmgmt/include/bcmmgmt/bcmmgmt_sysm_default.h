/*! \file bcmmgmt_sysm_default.h
 *
 * BCMMGMT internal header file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMMGMT_SYSM_DEFAULT_H
#define BCMMGMT_SYSM_DEFAULT_H

#include <bcmmgmt/bcmmgmt.h>

/*!
 * \brief Get default component list.
 *
 * Get list of start functions for all SDK components that are
 * controlled by the System Manager.
 *
 * \return List of conponent start functions.
 */
extern const bcmmgmt_comp_list_t *
bcmmgmt_sysm_default_comp_list(void);

#endif /* BCMMGMT_SYSM_DEFAULT_H */

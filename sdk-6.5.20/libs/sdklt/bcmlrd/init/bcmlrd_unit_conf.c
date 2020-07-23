/*! \file bcmlrd_unit_conf.c
 *
 * \brief bcmlrd_unit_conf data.
 *
 * Per unit configuration data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_internal.h>

const bcmlrd_map_conf_rep_t *bcmlrd_unit_conf[BCMDRD_CONFIG_MAX_UNITS];

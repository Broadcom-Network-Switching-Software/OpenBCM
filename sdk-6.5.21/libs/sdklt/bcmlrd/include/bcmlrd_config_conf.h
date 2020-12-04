/*! \file bcmlrd_config_conf.h
 *
 * Logical table configuration inclusion and exclusion support within
 * BCMLRD can be specified as a combination of the following defines:
 *
 *     (1) #define BCMLRD_CONFIG_INCLUDE_<CONF> [1|0]
 *             -- Include or exclude the given configuration
 *
 * The value of BCMLRD_CONFIG_INCLUDE_CONF_DEFAULT is used for any
 * configurations which are left unspecified. Set this value to 1 or 0 to
 * include or exclude all configurations by default.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLRD_CONFIG_CONF_H
#define BCMLRD_CONFIG_CONF_H

/* This determines whether a configuration is included or excluded by default */
#ifndef BCMLRD_CONFIG_INCLUDE_CONF_DEFAULT
#define BCMLRD_CONFIG_INCLUDE_CONF_DEFAULT 1
#endif

#include <bcmlrd/chip/bcmlrd_config_conf.h>

#endif /* BCMLRD_CONFIG_CONF_H */

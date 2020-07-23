/*! \file bcmlrd_dev_conf_get.c
 *
 * Get LRD device configuration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_conf.h>

/* Current LRD device configuration */
bcmlrd_dev_conf_t const *bcmlrd_dev_conf = bcmlrd_default_dev_conf;

const bcmlrd_dev_conf_t*
bcmlrd_dev_conf_get(void)
{
    return bcmlrd_dev_conf;
}

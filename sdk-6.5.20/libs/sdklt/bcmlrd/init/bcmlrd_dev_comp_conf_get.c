/*! \file bcmlrd_dev_comp_conf_get.c
 *
 * Get LRD device component configuration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmlrd/bcmlrd_internal.h>

/* Current LRD device component configuration */
bcmlrd_dev_comp_conf_t const *bcmlrd_dev_comp_conf = bcmlrd_default_dev_comp_conf;

const bcmlrd_dev_comp_conf_t*
bcmlrd_dev_comp_conf_get(void)
{
    return bcmlrd_dev_comp_conf;
}


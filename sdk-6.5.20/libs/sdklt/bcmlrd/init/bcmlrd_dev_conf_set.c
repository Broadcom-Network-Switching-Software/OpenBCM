/*! \file bcmlrd_dev_conf_set.c
 *
 * Set LRD device configuration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_conf.h>

const bcmlrd_dev_conf_t*
bcmlrd_dev_conf_set(const bcmlrd_dev_conf_t *dev_conf)
{
    const bcmlrd_dev_conf_t *old = bcmlrd_dev_conf;
    bcmlrd_dev_conf = dev_conf;

    return old;
}


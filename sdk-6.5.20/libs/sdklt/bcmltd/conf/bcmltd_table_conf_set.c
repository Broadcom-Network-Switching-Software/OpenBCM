/*! \file bcmltd_table_conf_set.c
 *
 * Set BCMLTD mapped table configuration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmltd/bcmltd_internal.h>

const bcmltd_table_conf_t *
bcmltd_table_conf_set(const bcmltd_table_conf_t *table_conf)
{
    const bcmltd_table_conf_t *old = &bcmltd_default_table_conf;
    bcmltd_table_conf = table_conf;

    return old;
}


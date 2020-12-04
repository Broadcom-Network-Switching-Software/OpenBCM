/*! \file bcmltd_table_conf.c
 *
 * BCMLRD mapped table configuration for all units.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmltd/bcmltd_internal.h>

/*
  This is not static to allow the table configuration to be overridden
 for test purposes.
*/

const bcmltd_table_conf_t *
bcmltd_table_conf = &bcmltd_default_table_conf;


/*! \file bcmlrd_dev_match_id_db_get.c
 *
 * Get LRD device PCM configuration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmlrd/bcmlrd_internal.h>

/* Current LRD device PCM configuration */
bcmlrd_match_id_db_get_t *bcmlrd_dev_match_id_db_ptrs = bcmlrd_default_match_id_db_get_ptrs;

bcmlrd_match_id_db_get_t *bcmlrd_dev_match_id_db_get(void)
{
    return bcmlrd_dev_match_id_db_ptrs;
}


/*! \file bcmlrd_dev_match_id_data_all_get.c
 *
 * Get pointers to match id data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmlrd/bcmlrd_internal.h>

/* Current LRD device PCM configuration */
const bcmlrd_match_id_db_info_t **bcmlrd_dev_match_id_data_info_all =
    bcmlrd_default_match_id_data_info_all;

const bcmlrd_match_id_db_info_t **bcmlrd_dev_match_id_data_all_info_get(void)
{
    return bcmlrd_dev_match_id_data_info_all;
}


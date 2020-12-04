/*! \file bcm56990_b0_lrd_match_id_db_get.c
 *
 * Chip stub for LRD match ID DB get functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif

#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_types.h>

const bcmlrd_match_id_db_info_t bcm56990_b0_lrd_match_id_db_info = {
    .num_entries = 0,
    .db = NULL,
};

int
bcm56990_b0_lrd_match_id_db_get(int unit,
                                const bcmlrd_sid_t sid,
                                const bcmlrd_fid_t fid,
                                const bcmlrd_match_id_db_t **info);

int
bcm56990_b0_lrd_match_id_db_get(int unit,
                                const bcmlrd_sid_t sid,
                                const bcmlrd_fid_t fid,
                                const bcmlrd_match_id_db_t **info)

{
    return 0;
}


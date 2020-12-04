/*! \file bcm56780_a0_fp_em_bank.c
 *
 * API to initialize stage attributes for
 * Trident4-X9(56780_A0) device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56780_a0_fp.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56780_a0_fp_em_bank_lookup_id_get(int unit,
                                     bcmdrd_sid_t lts_sid,
                                     uint8_t *lookup_id)
{
    SHR_FUNC_ENTER(unit);

    switch (lts_sid) {
    case IFTA80_E2T_00_LTS_TCAM_0m:
        *lookup_id = 4;
        break;
    case IFTA80_E2T_00_LTS_TCAM_1m:
        *lookup_id = 5;
        break;
    case IFTA80_E2T_01_LTS_TCAM_0m:
        *lookup_id = 6;
        break;
    case IFTA80_E2T_01_LTS_TCAM_1m:
        *lookup_id = 7;
        break;
    case IFTA80_E2T_02_LTS_TCAM_0m:
        *lookup_id = 8;
        break;
    case IFTA80_E2T_02_LTS_TCAM_1m:
        *lookup_id = 9;
        break;
    case IFTA80_E2T_03_LTS_TCAM_0m:
        *lookup_id = 10;
        break;
    case IFTA80_E2T_03_LTS_TCAM_1m:
        *lookup_id = 11;
        break;
    case IFTA90_E2T_00_LTS_TCAM_0m:
        *lookup_id = 0;
        break;
    case IFTA90_E2T_00_LTS_TCAM_1m:
        *lookup_id = 1;
        break;
    case IFTA90_E2T_01_LTS_TCAM_0m:
        *lookup_id = 2;
        break;
    case IFTA90_E2T_01_LTS_TCAM_1m:
        *lookup_id = 3;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

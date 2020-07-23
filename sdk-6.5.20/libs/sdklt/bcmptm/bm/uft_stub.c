/*! \file uft_stub.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>

/*******************************************************************************
 * Public functions
 */

int
bcmptm_cth_uft_lt_move_depth_get(int unit,
                                 bcmltd_sid_t ltid,
                                 uint8_t *move_depth)
{
    return SHR_E_UNAVAIL;
}

int
bcmptm_cth_uft_bank_info_get_from_lt(int unit,
                                     bcmltd_sid_t ltid,
                                     int alpm_level,
                                     bcmptm_pt_banks_info_t *banks_info)
{
    return SHR_E_UNAVAIL;
}

int
bcmptm_cth_uft_bank_info_get_from_rmalpm(int unit, bcmdrd_sid_t ptsid,
                                         bcmptm_pt_banks_info_t *banks_info)
{
    return SHR_E_UNAVAIL;
}

int
bcmptm_cth_uft_bank_info_get_from_rmtcam(int unit, bcmdrd_sid_t ptsid,
                                         bcmptm_pt_banks_info_t *banks_info)
{
    return SHR_E_UNAVAIL;
}

int
bcmptm_cth_uft_bank_info_get_from_ptcache(int unit, bcmdrd_sid_t ptsid,
                                          bcmptm_pt_banks_info_t *banks_info)
{
    return SHR_E_UNAVAIL;
}

int
bcmptm_uft_bank_hw_attr_get_from_lt(int unit, bcmltd_sid_t ltid,
                                    uint32_t lookup_num,
                                    bcmptm_lt_lookup_banks_info_t *banks_info)
{
    return SHR_E_UNAVAIL;
}


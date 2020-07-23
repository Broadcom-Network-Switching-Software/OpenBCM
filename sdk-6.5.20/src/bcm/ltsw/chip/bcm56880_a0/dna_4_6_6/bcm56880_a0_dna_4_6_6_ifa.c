/*! \file bcm56880_a0_dna_4_6_6_ifa.c
 *
 * BCM56880_A0 DNA_4_6_6 IFA driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/ifa_int.h>
#include <bcm_int/ltsw/mbcm/ifa.h>
#include <bcm_int/ltsw/xfs/types.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_device_constants.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_IFA

/* R_IFA_2_PROTOCOL_NUMBER fields. */
static const bcmint_ifa_fld_t
r_ifa_2_protocol_number_flds[] = {
    [BCMINT_LTSW_IFA_FLD_R_IFA_2_PROTOCOL_NUMBER_VALUE] = {
        VALUEs
    },
};

static const bcmint_ifa_lt_t
bcm56880_a0_dna_4_6_6_ifa_lt[] = {
    [BCMINT_LTSW_IFA_LT_R_IFA_2_PROTOCOL_NUMBER] = {
        .name = R_IFA_2_PROTOCOL_NUMBERs,
        .fld_bmp =
            (1 << BCMINT_LTSW_IFA_FLD_R_IFA_2_PROTOCOL_NUMBER_VALUE),
        .flds = r_ifa_2_protocol_number_flds
    },
};

/******************************************************************************
 * Private functions
 */

static int
bcm56880_a0_dna_4_6_6_ifa_lt_db_get(int unit,
                                      bcmint_ifa_lt_db_t *lt_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_db, SHR_E_PARAM);

    /* Bitmap of LTs that valid on bcm56880 DNA_4_6_6. */
    lt_db->lt_bmp = (1 << BCMINT_LTSW_IFA_LT_R_IFA_2_PROTOCOL_NUMBER);

    lt_db->lts = bcm56880_a0_dna_4_6_6_ifa_lt;

exit:
    SHR_FUNC_EXIT();
}

static mbcm_ltsw_ifa_drv_t bcm56880_a0_dna_4_6_6_ltsw_ifa_drv = {
    /* IFA LT database */
    .ifa_lt_db_get = bcm56880_a0_dna_4_6_6_ifa_lt_db_get
};

/******************************************************************************
 * Public functions
 */
int
bcm56880_a0_dna_4_6_6_ltsw_ifa_drv_attach(int unit)
{
    return mbcm_ltsw_ifa_drv_set(unit,
                                 &bcm56880_a0_dna_4_6_6_ltsw_ifa_drv);
}

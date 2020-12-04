/*! \file bcm56990_b0_ptm_uft_be.c
 *
 * Chip specific functions for PTM UFT_BE
 *
 * This file contains the chip specific functions for PTM UFT_BE
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmdrd/chip/bcm56990_b0_defs.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>

#include "bcm56990_b0_ptm_uft_be.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_UFT


/*******************************************************************************
 * Typedefs
 */

/*! Device hash group id type. */
typedef enum group_id_e {
    /*! Not a valid group. */
    GROUP_INVALID = 0,

    /*! L2 features. */
    L2_HASH_GROUP = 1,

    /*! MPLS feature. */
    MPLS_HASH_GROUP = 2,

    /*! FP_EM feature. */
    FP_EM_HASH_GROUP = 3,

    /*! L3_TUNNEL feature. */
    L3_TUNNEL_HASH_GROUP = 4,

    /*! ALPM level 2 feature. */
    ALPM_LEVEL2_GROUP = 5,

    /*! ALPM level 3 feature. */
    ALPM_LEVEL3_GROUP = 6,

    /*! EGR_ADAPT feature. */
    EGR_ADAPT_GROUP = 7,

    /*! L2 feature using UFT banks. */
    L2_UFT_GROUP = 8,
} group_id_t;

static bcmdrd_sid_t grp_ptsid[9] = {
    INVALIDm,
    L2_ENTRY_SINGLEm,
    MPLS_ENTRY_DOUBLEm,
    EXACT_MATCH_4m,
    L3_TUNNEL_QUADm,
    L3_DEFIP_ALPM_LEVEL2m,
    L3_DEFIP_ALPM_LEVEL3m,
    EGR_ADAPT_SINGLEm,
    L2_ENTRY_UFT_SINGLEm,
};

static uint32_t primary_group[9] = {
    GROUP_INVALID,
    L2_UFT_GROUP,
    MPLS_HASH_GROUP,
    FP_EM_HASH_GROUP,
    L3_TUNNEL_HASH_GROUP,
    ALPM_LEVEL2_GROUP,
    ALPM_LEVEL3_GROUP,
    EGR_ADAPT_GROUP,
    L2_UFT_GROUP,
};

/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief Initialize device specific info
 *
 * \param [in] unit Device u.
 * \param [out] dev Device specific info.
 *
 * \return SHR_E_XXX.
 */
static int
bcm56990_b0_uft_be_device_info_init(int unit,
                                    const bcmptm_uft_var_drv_t *var_info,
                                    uft_be_dev_info_t *dev)
{
    SHR_FUNC_ENTER(unit);

    dev->pt_cnt = 9;
    dev->ptsid = grp_ptsid;
    dev->primary_grp = primary_group;

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcm56990_b0_uft_be_driver_register(int unit, uft_be_driver_t *drv)
{
    SHR_FUNC_ENTER(unit);

    if (drv) {
        drv->device_info_init  =  bcm56990_b0_uft_be_device_info_init;
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}


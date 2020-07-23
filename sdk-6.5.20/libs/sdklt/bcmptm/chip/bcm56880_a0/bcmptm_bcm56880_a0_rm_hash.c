/*! \file bcmptm_bcm56880_a0_rm_hash.c
 *
 * Chip specific functions for PTRM hash
 *
 * This file contains the chip specific functions for PTRM hash.
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
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmdrd/chip/bcm56880_a0_defs.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_internal.h>
#include "bcmptm_bcm56880_a0_rm_hash.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

/*******************************************************************************
 * Private variables
 */
static const uint32_t ifta90_e2t_00_b0_base_bkt_cnt[] = { 16384 };
static const uint32_t ifta90_e2t_00_b1_base_bkt_cnt[] = { 16384 };
static const uint32_t ifta90_e2t_01_b0_base_bkt_cnt[] = { 16384 };
static const uint32_t ifta90_e2t_01_b1_base_bkt_cnt[] = { 16384 };
static const uint32_t ifta90_e2t_02_b0_base_bkt_cnt[] = { 16384 };
static const uint32_t ifta90_e2t_02_b1_base_bkt_cnt[] = { 16384 };
static const uint32_t ifta90_e2t_03_b0_base_bkt_cnt[] = { 16384 };
static const uint32_t ifta90_e2t_03_b1_base_bkt_cnt[] = { 16384 };
static const uint32_t ifta80_e2t_00_b0_base_bkt_cnt[] = { 4096 };
static const uint32_t ifta80_e2t_00_b1_base_bkt_cnt[] = { 4096 };
static const uint32_t ifta80_e2t_01_b0_base_bkt_cnt[] = { 4096 };
static const uint32_t ifta80_e2t_01_b1_base_bkt_cnt[] = { 4096 };
static const uint32_t ifta80_e2t_02_b0_base_bkt_cnt[] = { 4096 };
static const uint32_t ifta80_e2t_02_b1_base_bkt_cnt[] = { 4096 };
static const uint32_t ifta80_e2t_03_b0_base_bkt_cnt[] = { 4096 };
static const uint32_t ifta80_e2t_03_b1_base_bkt_cnt[] = { 4096 };
static const uint32_t efta30_e2t_00_b0_base_bkt_cnt[] = { 4096 };
static const uint32_t efta30_e2t_00_b1_base_bkt_cnt[] = { 4096 };
static const uint32_t ifta40_e2t_01_b0_base_bkt_cnt[] = { 4096 };
static const uint32_t ifta40_e2t_01_b1_base_bkt_cnt[] = { 4096 };
static const uint32_t ifta40_e2t_00_b0_base_bkt_cnt[] = { 4096 };
static const uint32_t ifta40_e2t_00_b1_base_bkt_cnt[] = { 4096 };
static const uint32_t ifta30_e2t_00_b0_base_bkt_cnt[] = { 4096 };
static const uint32_t ifta30_e2t_00_b1_base_bkt_cnt[] = { 4096 };

static const bcmptm_rm_hash_table_info_t bcm56880_a0_hash_tbl_info[] = {
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA90_E2T_00_B0_QUADm, IFTA90_E2T_00_B0_SINGLEm, IFTA90_E2T_00_KEY_ATTRIBUTESm, IFTA90_E2T_00_B0_LPm, 1, ifta90_e2t_00_b0_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA90_E2T_00_B1_QUADm, IFTA90_E2T_00_B1_SINGLEm, IFTA90_E2T_00_KEY_ATTRIBUTESm, IFTA90_E2T_00_B1_LPm, 1, ifta90_e2t_00_b1_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA90_E2T_01_B0_QUADm, IFTA90_E2T_01_B0_SINGLEm, IFTA90_E2T_01_KEY_ATTRIBUTESm, IFTA90_E2T_01_B0_LPm, 1, ifta90_e2t_01_b0_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA90_E2T_01_B1_QUADm, IFTA90_E2T_01_B1_SINGLEm, IFTA90_E2T_01_KEY_ATTRIBUTESm, IFTA90_E2T_01_B1_LPm, 1, ifta90_e2t_01_b1_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA90_E2T_02_B0_QUADm, IFTA90_E2T_02_B0_SINGLEm, IFTA90_E2T_02_KEY_ATTRIBUTESm, IFTA90_E2T_02_B0_LPm, 1, ifta90_e2t_02_b0_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA90_E2T_02_B1_QUADm, IFTA90_E2T_02_B1_SINGLEm, IFTA90_E2T_02_KEY_ATTRIBUTESm, IFTA90_E2T_02_B1_LPm, 1, ifta90_e2t_02_b1_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA90_E2T_03_B0_QUADm, IFTA90_E2T_03_B0_SINGLEm, IFTA90_E2T_03_KEY_ATTRIBUTESm, IFTA90_E2T_03_B0_LPm, 1, ifta90_e2t_03_b0_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA90_E2T_03_B1_QUADm, IFTA90_E2T_03_B1_SINGLEm, IFTA90_E2T_03_KEY_ATTRIBUTESm, IFTA90_E2T_03_B1_LPm, 1, ifta90_e2t_03_b1_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA80_E2T_00_B0_QUADm, IFTA80_E2T_00_B0_SINGLEm, IFTA80_E2T_00_KEY_ATTRIBUTESm, IFTA80_E2T_00_B0_LPm, 1, ifta80_e2t_00_b0_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA80_E2T_00_B1_QUADm, IFTA80_E2T_00_B1_SINGLEm, IFTA80_E2T_00_KEY_ATTRIBUTESm, IFTA80_E2T_00_B1_LPm, 1, ifta80_e2t_00_b1_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA80_E2T_01_B0_QUADm, IFTA80_E2T_01_B0_SINGLEm, IFTA80_E2T_01_KEY_ATTRIBUTESm, IFTA80_E2T_01_B0_LPm, 1, ifta80_e2t_01_b0_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA80_E2T_01_B1_QUADm, IFTA80_E2T_01_B1_SINGLEm, IFTA80_E2T_01_KEY_ATTRIBUTESm, IFTA80_E2T_01_B1_LPm, 1, ifta80_e2t_01_b1_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA80_E2T_02_B0_QUADm, IFTA80_E2T_02_B0_SINGLEm, IFTA80_E2T_02_KEY_ATTRIBUTESm, IFTA80_E2T_02_B0_LPm, 1, ifta80_e2t_02_b0_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA80_E2T_02_B1_QUADm, IFTA80_E2T_02_B1_SINGLEm, IFTA80_E2T_02_KEY_ATTRIBUTESm, IFTA80_E2T_02_B1_LPm, 1, ifta80_e2t_02_b1_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA80_E2T_03_B0_QUADm, IFTA80_E2T_03_B0_SINGLEm, IFTA80_E2T_03_KEY_ATTRIBUTESm, IFTA80_E2T_03_B0_LPm, 1, ifta80_e2t_03_b0_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA80_E2T_03_B1_QUADm, IFTA80_E2T_03_B1_SINGLEm, IFTA80_E2T_03_KEY_ATTRIBUTESm, IFTA80_E2T_03_B1_LPm, 1, ifta80_e2t_03_b1_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, EFTA30_E2T_00_B0_QUADm, EFTA30_E2T_00_B0_SINGLEm, EFTA30_E2T_00_KEY_ATTRIBUTESm, EFTA30_E2T_00_B0_LPm, 1, efta30_e2t_00_b0_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, EFTA30_E2T_00_B1_QUADm, EFTA30_E2T_00_B1_SINGLEm, EFTA30_E2T_00_KEY_ATTRIBUTESm, EFTA30_E2T_00_B1_LPm, 1, efta30_e2t_00_b1_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA40_E2T_01_B0_QUADm, IFTA40_E2T_01_B0_SINGLEm, IFTA40_E2T_01_KEY_ATTRIBUTESm, IFTA40_E2T_01_B0_LPm, 1, ifta40_e2t_01_b0_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA40_E2T_01_B1_QUADm, IFTA40_E2T_01_B1_SINGLEm, IFTA40_E2T_01_KEY_ATTRIBUTESm, IFTA40_E2T_01_B1_LPm, 1, ifta40_e2t_01_b1_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA40_E2T_00_B0_QUADm, IFTA40_E2T_00_B0_SINGLEm, IFTA40_E2T_00_KEY_ATTRIBUTESm, IFTA40_E2T_00_B0_LPm, 1, ifta40_e2t_00_b0_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA40_E2T_00_B1_QUADm, IFTA40_E2T_00_B1_SINGLEm, IFTA40_E2T_00_KEY_ATTRIBUTESm, IFTA40_E2T_00_B1_LPm, 1, ifta40_e2t_00_b1_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA30_E2T_00_B0_QUADm, IFTA30_E2T_00_B0_SINGLEm, IFTA30_E2T_00_KEY_ATTRIBUTESm, IFTA30_E2T_00_B0_LPm, 1, ifta30_e2t_00_b0_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
{TRUE, BCMPTM_RM_HASH_TYPE_SHARED, IFTA30_E2T_00_B1_QUADm, IFTA30_E2T_00_B1_SINGLEm, IFTA30_E2T_00_KEY_ATTRIBUTESm, IFTA30_E2T_00_B1_LPm, 1, ifta30_e2t_00_b1_base_bkt_cnt, 4, RM_HASH_BM0|RM_HASH_BM1|RM_HASH_BM2, NULL, NULL },
};

/*******************************************************************************
 * Private Functions
 */


/*******************************************************************************
 * Public Functions
 */

int
bcmptm_bcm56880_a0_hash_table_info_get(int unit,
                                       const bcmptm_rm_hash_table_info_t **tbl_info,
                                       uint8_t *num_tbl_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_tbl_info, SHR_E_PARAM);

    *tbl_info = bcm56880_a0_hash_tbl_info;
    *num_tbl_info = COUNTOF(bcm56880_a0_hash_tbl_info);

exit:
    SHR_FUNC_EXIT();
}

/*! \file bcm56990_a0_uft.c
 *
 * BCM56990_A0 uft subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/uft.h>
#include <bcm_int/ltsw/xgs/uft.h>
#include <bcm_int/ltsw/uft_int.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_UFT

/******************************************************************************
 * Private functions
 */

/*!
* \brief Bank IDs mapping table.
*/
static shr_enum_map_t bcm56990_a0_bank_id_map[] = {
    { (char *)L2_BANK0s, 0 },
    { (char *)L2_BANK1s, 1 },
    { (char *)MPLS_BANK0s, 2 },
    { (char *)MPLS_BANK1s, 3 },
    { (char *)MPLS_BANK2s, 4 },
    { (char *)MPLS_BANK3s, 5 },
    { (char *)L3_TUNNEL_BANK0s, 6 },
    { (char *)L3_TUNNEL_BANK1s, 7 },
    { (char *)L3_TUNNEL_BANK2s, 8 },
    { (char *)L3_TUNNEL_BANK3s, 9 },
    { (char *)UFT_BANK0s, 10 },
    { (char *)UFT_BANK1s, 11 },
    { (char *)UFT_BANK2s, 12 },
    { (char *)UFT_BANK3s, 13 },
    { (char *)UFT_BANK4s, 14 },
    { (char *)UFT_BANK5s, 15 },
    { (char *)UFT_BANK6s, 16 },
    { (char *)UFT_BANK7s, 17 },
    { (char *)UFT_MINI_BANK0s, 18 },
    { (char *)UFT_MINI_BANK1s, 19 },
    { (char *)UFT_MINI_BANK2s, 20 },
    { (char *)UFT_MINI_BANK3s, 21 },
    { (char *)UFT_MINI_BANK4s, 22 },
    { (char *)UFT_MINI_BANK5s, 23 },
};

/*!
 * \brief Group IDs mapping table.
 */
static shr_enum_map_t bcm56990_a0_group_id_map[] = {
    { (char *)L2_HASH_GROUPs, 0 },
    { (char *)MPLS_HASH_GROUPs, 1 },
    { (char *)FP_EM_HASH_GROUPs, 2 },
    { (char *)L3_TUNNEL_HASH_GROUPs, 3 },
    { (char *)ALPM_LEVEL2_GROUPs, 4 },
    { (char *)ALPM_LEVEL3_GROUPs, 5 },
};

/*!
 * \brief Move depth switch controls to logical table key strings mapping table.
 */
static shr_enum_map_t bcm56990_a0_move_depth_map[] = {
    { (char *)L2_FDB_VLANs, bcmSwitchHashMultiMoveDepthL2 },
    { (char *)FP_EM_ENTRYs, bcmSwitchHashMultiMoveDepthExactMatch },
    { (char *)TNL_IPV4_DECAP_EMs, bcmSwitchHashMultiMoveDepthL3Tunnel },
    { (char *)TNL_IPV6_DECAP_EMs, bcmSwitchHashMultiMoveDepthL3Tunnel },
    { (char *)TNL_MPLS_DECAPs, bcmSwitchHashMultiMoveDepthMpls },
    { (char *)TNL_MPLS_DECAP_TRUNKs, bcmSwitchHashMultiMoveDepthMpls },
};

/*!
 * \brief Hash tables to logical tables mapping table.
 */
static bcmint_uft_table_map_t bcm56990_a0_table_map[] = {
    { (char *)L2_FDB_VLANs, -1, bcmHashTableL2 },
    { (char *)TNL_MPLS_DECAPs, -1, bcmHashTableMPLS },
    { (char *)TNL_IPV4_DECAP_EMs, -1, bcmHashTableL3Tunnel },
    { (char *)FP_EM_ENTRYs, -1, bcmHashTableExactMatch },
};


static bcmint_uft_var_db_t bcm56990_a0_uft_var_db = {
    .bank_id_map = bcm56990_a0_bank_id_map,
    .bank_id_map_count = COUNTOF(bcm56990_a0_bank_id_map),
    .group_id_map = bcm56990_a0_group_id_map,
    .group_id_map_count = COUNTOF(bcm56990_a0_group_id_map),
    .move_depth_map = bcm56990_a0_move_depth_map,
    .move_depth_map_count = COUNTOF(bcm56990_a0_move_depth_map),
    .table_map = bcm56990_a0_table_map,
    .table_map_count = COUNTOF(bcm56990_a0_table_map)
};

static int
bcm56990_a0_ltsw_uft_var_db_get(int unit, bcmint_uft_var_db_t **uft_var_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(uft_var_db, SHR_E_MEMORY);

    *uft_var_db = &bcm56990_a0_uft_var_db;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Uft sub driver functions for bcm56990_a0.
 */
static mbcm_ltsw_uft_drv_t bcm56990_a0_uft_sub_drv = {
    .uft_var_db_get = bcm56990_a0_ltsw_uft_var_db_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm56990_a0_uft_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_drv_set(unit, &bcm56990_a0_uft_sub_drv));

exit:
    SHR_FUNC_EXIT();
}

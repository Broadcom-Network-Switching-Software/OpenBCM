/*! \file bcm5699x_uft.c
 *
 * BCM5699x UFT subordinate driver.
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
static const shr_enum_map_t bcm5699x_bank_id_map[] = {
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
    { (char *)EGR_ADAPT_BANK0s, 10 },
    { (char *)EGR_ADAPT_BANK1s, 11 },
    { (char *)UFT_BANK0s, 12 },
    { (char *)UFT_BANK1s, 13 },
    { (char *)UFT_BANK2s, 14 },
    { (char *)UFT_BANK3s, 15 },
    { (char *)UFT_BANK4s, 16 },
    { (char *)UFT_BANK5s, 17 },
    { (char *)UFT_BANK6s, 18 },
    { (char *)UFT_BANK7s, 19 },
    { (char *)UFT_MINI_BANK0s, 20 },
    { (char *)UFT_MINI_BANK1s, 21 },
    { (char *)UFT_MINI_BANK2s, 22 },
    { (char *)UFT_MINI_BANK3s, 23 },
    { (char *)UFT_MINI_BANK4s, 24 },
    { (char *)UFT_MINI_BANK5s, 25 },
};

/*!
 * \brief Group IDs mapping table.
 */
static const shr_enum_map_t bcm5699x_group_id_map[] = {
    { (char *)L2_HASH_GROUPs, 0 },
    { (char *)MPLS_HASH_GROUPs, 1 },
    { (char *)FP_EM_HASH_GROUPs, 2 },
    { (char *)L3_TUNNEL_HASH_GROUPs, 3 },
    { (char *)ALPM_LEVEL2_GROUPs, 4 },
    { (char *)ALPM_LEVEL3_GROUPs, 5 },
    { (char *)EGR_ADAPT_GROUPs, 6 },
    { (char *)L2_UFT_GROUPs, 7 },
};

/*!
 * \brief Move depth switch controls to logical table key strings mapping table.
 */
static const shr_enum_map_t bcm5699x_move_depth_map[] = {
    { (char *)L2_VFI_FDBs, bcmSwitchHashMultiMoveDepthL2 },
    { (char *)FP_EM_ENTRYs, bcmSwitchHashMultiMoveDepthExactMatch },
    { (char *)TNL_IPV4_DECAP_EMs, bcmSwitchHashMultiMoveDepthL3Tunnel },
    { (char *)TNL_IPV6_DECAP_EMs, bcmSwitchHashMultiMoveDepthL3Tunnel },
    { (char *)TNL_MPLS_DECAPs, bcmSwitchHashMultiMoveDepthMpls },
    { (char *)TNL_MPLS_DECAP_TRUNKs, bcmSwitchHashMultiMoveDepthMpls },
};

/*!
 * \brief Hash tables to logical tables mapping table.
 */
static const bcmint_uft_table_map_t bcm5699x_table_map[] = {
    { (char *)L2_VFI_FDBs, -1, bcmHashTableL2 },
    { (char *)TNL_MPLS_DECAPs, -1, bcmHashTableMPLS },
    { (char *)TNL_IPV4_DECAP_EMs, -1, bcmHashTableL3Tunnel },
    { (char *)FP_EM_ENTRYs, -1, bcmHashTableExactMatch },
};

static bcmint_uft_var_db_t bcm5699x_uft_var_db = {
    .bank_id_map = bcm5699x_bank_id_map,
    .bank_id_map_count = COUNTOF(bcm5699x_bank_id_map),
    .group_id_map = bcm5699x_group_id_map,
    .group_id_map_count = COUNTOF(bcm5699x_group_id_map),
    .move_depth_map = bcm5699x_move_depth_map,
    .move_depth_map_count = COUNTOF(bcm5699x_move_depth_map),
    .table_map = bcm5699x_table_map,
    .table_map_count = COUNTOF(bcm5699x_table_map),
};

static int
bcm5699x_ltsw_uft_var_db_get(int unit, bcmint_uft_var_db_t **uft_var_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(uft_var_db, SHR_E_MEMORY);

    *uft_var_db = &bcm5699x_uft_var_db;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Uft sub driver functions for bcm5699x.
 */
static mbcm_ltsw_uft_drv_t bcm5699x_uft_sub_drv = {
    .uft_var_db_get = bcm5699x_ltsw_uft_var_db_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm5699x_uft_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_drv_set(unit, &bcm5699x_uft_sub_drv));

exit:
    SHR_FUNC_EXIT();
}

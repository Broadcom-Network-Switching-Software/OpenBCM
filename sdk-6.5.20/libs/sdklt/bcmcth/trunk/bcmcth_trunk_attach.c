/*! \file bcmcth_trunk_attach.c
 *
 * BCMCTH Trunk Driver attach/detach APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmcth/bcmcth_trunk.h>
#include <bcmcth/bcmcth_trunk_db.h>
#include <bcmcth/bcmcth_trunk_drv.h>
#include <bcmcth/bcmcth_trunk_util.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmcth/bcmcth_trunk_vp_dev.h>
#include <bcmcth/bcmcth_trunk_vp_grpmgr.h>
#include <bcmcth/bcmcth_trunk_vp.h>
#include <bcmcth/bcmcth_trunk_vp_imm.h>
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TRUNK

/*******************************************************************************
* Public functions
 */

/*!
 * \brief Device TRUNK_VP feature initialiation function.
 *
 * Initializes TRUNK_VP feature based on the device TRUNK_VP resolution mode.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_UNAVAIL When LRD mapping is unavailable for TRUNK_VP LTs.
 * \retval SHR_E_CONFIG Invalid TRUNK_VP mode is configured in config.yml file.
 */
int
bcmcth_trunk_vp_dev_init(int unit, bool warm)
{
    bcmcfg_feature_ctl_config_t feature_conf;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmcth_trunk_vp_dev_init.\n")));


    /* Initialize TRUNK_VP atomic transactions support feature status. */
    rv = bcmcfg_feature_ctl_config_get(&feature_conf);
    if (SHR_FAILURE(rv) || (feature_conf.enable_atomic_trans == 0)) {
        BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit) = FALSE;
    } else {
        BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit) = TRUE;
    }


    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "ATOMIC_TRANS=%s.\n"),
             BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit) ? "TRUE" : "FALSE"));

    /* Use HA memory to save TRUNK_VP software state. */
    BCMCTH_TRUNK_VP_USE_HA_MEM(unit) = TRUE;

    /* Use DMA engine for programming TRUNK_VP hardware tables. */
    BCMCTH_TRUNK_VP_USE_DMA(unit) = TRUE;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_drv_vp_info_init(unit, warm));

    /* Allocate memory to store LT Entry information. */
    BCMCTH_TRUNK_VP_ALLOC
        (BCMCTH_TRUNK_VP_LT_ENT_PTR(unit), sizeof(bcmcth_trunk_vp_lt_entry_t),
         "bcmcthTrunkVpDevLtEntInfo");

    /* Allocate memory to store TRUNK_VP entries in WAL information. */
    BCMCTH_TRUNK_VP_ALLOC(BCMCTH_TRUNK_VP_WALI_PTR(unit),
          sizeof(*BCMCTH_TRUNK_VP_WALI_PTR(unit)),
          "bcmcthTrunkVpWali");

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Device TRUNK_VP feature cleanup function.
 *
 * Releases resources allocated for TRUNK_VP feature.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_trunk_vp_dev_cleanup(int unit, bool warm)
{

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmcth_trunk_vp_dev_cleanup.\n")));

    /* Cleanup hardware specific information. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_trunk_drv_vp_cleanup(unit));

    /* Free memory allocated for LT Entry buffer. */
    if (BCMCTH_TRUNK_VP_LT_ENT_PTR(unit)) {
        BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_LT_ENT_PTR(unit));
    }

    /* Free memory allocated for TRUNK_VP WAL entries info. */
    if (BCMCTH_TRUNK_VP_WALI_PTR(unit)) {
       BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_WALI_PTR(unit));
    }
    exit:
        SHR_FUNC_EXIT();
}

int
bcmcth_trunk_attach(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_t *vmap = NULL;
    const bcmlrd_map_t *wmap = NULL;
    int rv = SHR_E_NONE;
    int rv1 = SHR_E_NONE;
    bcmcth_trunk_drv_var_t *drv_var = NULL;
    const bcmcth_trunk_vp_drv_var_t *vp_drv_var = NULL;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (SHR_FAILURE(bcmlrd_map_get(unit, TRUNKt, &map))
            && SHR_FAILURE(bcmlrd_map_get(unit, TRUNK_FASTt, &map))) {
        SHR_EXIT();
    }

    /* Initialize device-specific driver */
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_drv_init(unit));
    SHR_IF_ERR_EXIT
            (bcmcth_trunk_init(unit, warm));

    /* Get the TRUNK_VP driver */
    drv_var = bcmcth_trunk_drv_var_get(unit);

    if (drv_var != NULL) {
        vp_drv_var = drv_var->trunk_vp_var;
        SHR_NULL_CHECK(vp_drv_var, SHR_E_INTERNAL);
        /* Device-specific initialization */
        rv = bcmlrd_map_get(
               unit,
               vp_drv_var->ids->trunk_vp_ltid,
               &vmap);
        rv1 = bcmlrd_map_get(
                 unit,
                 vp_drv_var->ids->trunk_vp_weighted_ltid,
                 &wmap);
        if (SHR_SUCCESS(rv) || SHR_SUCCESS(rv1)) {
            if (vmap || wmap) {
                SHR_IF_ERR_VERBOSE_EXIT(bcmcth_trunk_vp_dev_init(unit, warm));
                BCMCTH_TRUNK_VP_CTRL(unit)->init = TRUE;
            }
            if (warm) {
                SHR_IF_ERR_EXIT(
                     bcmcth_trunk_vp_wb_itbm_group_alloc(unit));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_detach(int unit, bool warm)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_t *vmap = NULL;
    const bcmlrd_map_t *wmap = NULL;
    int rv = SHR_E_NONE;
    int rv1 = SHR_E_NONE;
    bcmcth_trunk_drv_var_t *drv_var= NULL;
    const bcmcth_trunk_vp_drv_var_t *vp_drv_var= NULL;

    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(bcmlrd_map_get(unit, TRUNKt, &map))
            && SHR_FAILURE(bcmlrd_map_get(unit, TRUNK_FASTt, &map))) {
        SHR_EXIT();
    }

    drv_var = bcmcth_trunk_drv_var_get(unit);

    if (drv_var != NULL) {
        vp_drv_var = drv_var->trunk_vp_var;
        SHR_NULL_CHECK(vp_drv_var, SHR_E_INTERNAL);

        rv = bcmlrd_map_get(
                    unit,
                    vp_drv_var->ids->trunk_vp_ltid,
                    &vmap);
        rv1 = bcmlrd_map_get(
                    unit,
                    vp_drv_var->ids->trunk_vp_weighted_ltid,
                    &wmap);
        if (SHR_SUCCESS(rv) || SHR_SUCCESS(rv1)) {
            if (vmap || wmap) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_trunk_vp_dev_cleanup(unit, warm));
            }
        }
    }

    SHR_IF_ERR_CONT
        (bcmcth_trunk_cleanup(unit));
exit:
    SHR_FUNC_EXIT();
}

/*! \file bcmecmp_dev.c
 *
 * This file contains device ECMP initialization and cleanup functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <sal/sal_time.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmecmp/bcmecmp_dev.h>
#include <bcmecmp/bcmecmp_group.h>
#include <bcmecmp/bcmecmp.h>
#include <bcmecmp/bcmecmp_common_imm.h>
/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*!
 * \brief Device ECMP feature initialiation function.
 *
 * Initializes ECMP feature based on the device ECMP resolution mode.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_UNAVAIL When LRD mapping is unavailable for ECMP LTs.
 * \retval SHR_E_CONFIG Invalid ECMP mode is configured in config.yml file.
 */
int
bcmecmp_dev_init(int unit, bool warm)
{
    uint64_t ecmp_mode = BCMECMP_MODE_COUNT; /* Device ECMP mode. */
    const bcmlrd_map_t *map = NULL;
    bcmcfg_feature_ctl_config_t feature_conf;
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_dev_init.\n")));


    /*
     * Retrieve ECMP_MODE value from ECMP_CONFIG device configuration logical
     * table (BCMCFG).
     */
    if (SHR_SUCCESS(bcmlrd_map_get(unit, ECMP_CONFIGt, &map))) {
        SHR_IF_ERR_EXIT
            (bcmcfg_field_get(unit,
                              ECMP_CONFIGt,
                              ECMP_CONFIGt_MODEf,
                              &ecmp_mode));
    } else {
        ecmp_mode = BCMECMP_MODE_FULL;
    }

    /* Update SW ECMP mode variable. */
    switch (ecmp_mode) {
        case BCMECMP_MODE_FULL:
            BCMECMP_MODE(unit) = BCMECMP_MODE_FULL;
            break;
        case BCMECMP_MODE_HIER:
            BCMECMP_MODE(unit) = BCMECMP_MODE_HIER;
            break;
        case BCMECMP_MODE_FAST:
            BCMECMP_MODE(unit) = BCMECMP_MODE_FAST;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Initialize ECMP atomic transactions support feature status. */
    rv = bcmcfg_feature_ctl_config_get(&feature_conf);
    if (SHR_FAILURE(rv) || (feature_conf.enable_atomic_trans == 0)) {
        BCMECMP_ATOMIC_TRANS(unit) = FALSE;
    } else {
        BCMECMP_ATOMIC_TRANS(unit) = TRUE;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "ATOMIC_TRANS=%s.\n"),
             BCMECMP_ATOMIC_TRANS(unit) ? "TRUE" : "FALSE"));

    /* Use HA memory to save ECMP software state. */
    BCMECMP_USE_HA_MEM(unit) = TRUE;

    /* Use DMA engine for programming ECMP hardware tables. */
    BCMECMP_USE_DMA(unit) = TRUE;

    /* Initialize hardware specific information. */
    if (BCMECMP_FNCALL_CHECK(unit, info_init)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (BCMECMP_FNCALL_EXEC(unit, info_init)(unit, warm));
    } else {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Allocate memory to store ECMP entries in WAL information. */
    BCMECMP_ALLOC(BCMECMP_WALI_PTR(unit), sizeof(*BCMECMP_WALI_PTR(unit)),
                  "bcmecmpWali");

    if (warm) {
        SHR_IF_ERR_EXIT
            (bcmecmp_wb_itbm_ecmp_group_add(unit));
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Device ECMP feature cleanup function.
 *
 * Releases resources allocated for ECMP feature.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmecmp_dev_cleanup(int unit, bool warm)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_dev_cleanup.\n")));

    /* Free memory allocated to store group next-hop members. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmecmp_groups_cleanup(unit));

    /* Cleanup hardware specific information. */
    if (BCMECMP_FNCALL_CHECK(unit, info_cleanup)) {
        rv = BCMECMP_FNCALL_EXEC(unit, info_cleanup)(unit);
        /*
         * Unavailable error code returned for ECMP LTs,
         * when ECMP LT's are not supported for the device variant.
         * Do not treat this condition as an error,
         * Set the return value to SHR_E_NONE and
         * cleanup the resources allocated for ECMP if allocated.
         */
         if (rv == SHR_E_UNAVAIL) {
             SHR_EXIT();
         }
         SHR_IF_ERR_MSG_EXIT(rv,
              (BSL_META_U(unit,
                          "ECMP info clean up failed with rv= %d\n"),
                          rv));
    }

    /* Free memory allocated for ECMP WAL entries info. */
    if (BCMECMP_WALI_PTR(unit)) {
        BCMECMP_FREE(BCMECMP_WALI_PTR(unit));
    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Device ECMP pre-configuration function.
 *
 * This function is called during warm start for ECMP groups pre-processing.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmecmp_dev_pre_config(int unit, bool warm)
{
    bcmecmp_control_t *ecmp_ctrl_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "%s.\n"), __func__));

    /*
     * Check if the ECMP control structure has been initialized for this unit.
     * If it has not been initialized, then exit this function as there is no
     * pre-config processing to be done for this feature.
     */
    ecmp_ctrl_ptr = bcmecmp_get_ecmp_control(unit);
    if (ecmp_ctrl_ptr == NULL || ecmp_ctrl_ptr->init == FALSE) {
        SHR_EXIT();
    }

    if (BCMECMP_FNCALL_CHECK(unit, rh_pre_config)) {
        SHR_IF_ERR_EXIT
            (BCMECMP_FNCALL_EXEC(unit, rh_pre_config)(unit));
    }

    if (BCMECMP_USE_INDEX_REMAP(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_grp_list_preconfig(unit));
    }

    if (warm && ecmp_ctrl_ptr->ecmp_drv
        && ecmp_ctrl_ptr->ecmp_drv->shr_grp_type_get
        && ecmp_ctrl_ptr->ecmp_info.grp_arr_sz) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_wb_itbm_group_preconfig(unit));
    }

    exit:
        SHR_FUNC_EXIT();
}

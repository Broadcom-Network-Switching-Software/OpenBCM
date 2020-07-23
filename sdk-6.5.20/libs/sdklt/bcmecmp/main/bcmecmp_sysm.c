/*! \file bcmecmp_sysm.c
 *
 * ECMP interface functions to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <sal/sal_alloc.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_sysm.h>
#include <bsl/bsl.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmecmp/bcmecmp_dev.h>
#include <bcmecmp/bcmecmp_sysm.h>
#include <bcmecmp/bcmecmp_group_util.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmecmp/bcmecmp_imm.h>
#include <bcmecmp/bcmecmp_db_internal.h>
#include <bcmecmp/bcmecmp_member_dest.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*******************************************************************************
 * Local definitions
 */
static bcmecmp_control_t *ecmp_ctrl[BCMECMP_NUM_UNITS_MAX] = {0};

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Allocates ECMP control structure.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmecmp_resources_alloc(int unit)
{
    uint32_t size = 0;

    SHR_FUNC_ENTER(unit);

    if (!ecmp_ctrl[unit]) {
        BCMECMP_ALLOC
            (ecmp_ctrl[unit],
             sizeof(bcmecmp_control_t),
             "bcmecmpSysEcmpCtrl");
        size = sizeof(bcmecmp_control_t);
        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "ECMP control is %"PRIu32".\n"),
                  size));

        /* Create the ECMP CTH control structure synchronization lock. */
        ecmp_ctrl[unit]->ecmp_lock = sal_mutex_create("bcmecmpCtrlLock");
        if (!ecmp_ctrl[unit]->ecmp_lock) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        BCMECMP_MODE(unit) = BCMECMP_MODE_COUNT;
    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Free ECMP control structure.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmecmp_resources_free(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (ecmp_ctrl[unit]) {
        if (ecmp_ctrl[unit]->ecmp_lock) {
            sal_mutex_destroy(ecmp_ctrl[unit]->ecmp_lock);
        }
        if (BCMECMP_USE_INDEX_REMAP(unit)) {
            (void)bcmecmp_grp_list_destroy(unit,
                                           BCMECMP_MEMBER_WEIGHT_LIST(unit));
            (void)bcmecmp_grp_list_destroy(unit,
                                           BCMECMP_NON_MEMBER_WEIGHT_LIST(unit));
        }
        BCMECMP_FREE(ecmp_ctrl[unit]);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup ECMP handler function software data structures.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmecmp_cleanup(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    if (!ecmp_ctrl[unit]) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_dev_cleanup(unit, warm));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_detach(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_resources_free(unit));

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize ECMP handler function software data structures.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmecmp_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_resources_alloc(unit));

    exit:
        if (SHR_FUNC_ERR()) {
            /*
             * Perform cleanup and return the actual error value for all other
             * error conditions.
             */
            bcmecmp_resources_free(unit);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Initializes ECMP component on this BCM unit.
 *
 * \param [in] inst_cat System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start status.
 * \param [out] blocking_comp Pointer to the name of the blocking component.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 * \retval SHR_SYSM_RV_BLOCKED On being blocked by another component.
 */
static shr_sysm_rv_t
bcmecmp_unit_init(shr_sysm_categories_t inst_cat,
                  int unit,
                  void *comp_data,
                  bool warm,
                  uint32_t *blocking_comp)
{
    int rv;
    int idx;
    uint32_t comp_dep[] = {
        BCMMGMT_CFG_COMP_ID,
        BCMMGMT_LTD_COMP_ID,
        BCMMGMT_BD_COMP_ID,
        BCMMGMT_PTM_COMP_ID,
        BCMMGMT_DI_COMP_ID,
        BCMMGMT_SER_COMP_ID
    };

    for (idx = 0; idx < COUNTOF(comp_dep); idx++) {
        if (!shr_sysm_is_comp_complete(comp_dep[idx], inst_cat, unit)) {
            *blocking_comp = comp_dep[idx];
            return SHR_SYSM_RV_BLOCKED;
        }
    }

    /* Initialization ECMP component. */
    rv = bcmecmp_init(unit, warm);
    if (SHR_FAILURE(rv)) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief ECMP IMM init
 *
 * register related ECMP LT event callback in IMM module
 *
 * \param [in] unit This is device unit number.
 *
 * \retval None.
 */
static int
bcmecmp_imm_init(int unit)
{
    size_t count = 0, actual = 0, idx = 0;
    bcmlrd_sid_t *tables = NULL;
    bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;
    bcmltd_sid_t ltd_sid = INVALIDm;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);
    ids = drv_var->ids;

    /* Initialize IMM drivers. */
    SHR_IF_ERR_EXIT
        (bcmlrd_custom_tables_count_get(unit,
                                        "bcmimm",
                                        "basic",
                                        &count));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "count=%u.\n"), (unsigned int)count));

    BCMECMP_ALLOC(tables, sizeof(*tables) * count, "bcmecmpTables");
    SHR_IF_ERR_EXIT
        (bcmlrd_custom_tables_get(unit,
                                  "bcmimm",
                                  "basic",
                                  count,
                                  tables,
                                  &actual));

    for (idx = 0; idx < actual; idx++) {
        switch (tables[idx]) {
            case ECMPt:
            case ECMP_FASTt:
            case ECMP_HIERARCHICALt:
                SHR_IF_ERR_EXIT
                    (bcmecmp_group_imm_register(unit, tables[idx]));
                break;

            case ECMP_OVERLAYt:
                SHR_IF_ERR_EXIT
                    (bcmecmp_overlay_group_imm_register(unit, tables[idx]));
                break;

            case ECMP_UNDERLAYt:
                SHR_IF_ERR_EXIT
                    (bcmecmp_underlay_group_imm_register(unit, tables[idx]));
                break;

            case ECMP_WEIGHTEDt:
                SHR_IF_ERR_EXIT
                    (bcmecmp_weighted_group_imm_register(unit, tables[idx]));
                break;

            case ECMP_WEIGHTED_UNDERLAYt:
                ltd_sid = ecmp_ctrl[unit]->ecmp_info.
                              group[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED].ltd_sid;
                if (ltd_sid == tables[idx]) {
                    SHR_IF_ERR_EXIT
                        (bcmecmp_weighted_underlay_group_imm_register
                            (unit, tables[idx]));
                } else {
                    SHR_IF_ERR_EXIT
                        (bcmecmp_weighted_group_imm_register(unit, tables[idx]));
                }
                break;

            case ECMP_WEIGHTED_OVERLAYt:
                SHR_IF_ERR_EXIT
                    (bcmecmp_weighted_overlay_group_imm_register
                         (unit, tables[idx]));
                break;

            case ECMP_LIMIT_CONTROLt:
                SHR_IF_ERR_EXIT
                    (bcmecmp_limit_imm_register(unit));
                break;

            default:
                if (ids == NULL) {
                    continue;
                } else {
                    if (tables[idx] == ids->ecmp_level0_ltid) {
                        SHR_IF_ERR_EXIT
                         (bcmecmp_lvl0_group_imm_register(
                                     unit, tables[idx]));
                        break;
                   } else if (tables[idx] == ids->ecmp_wlevel0_ltid) {
                        SHR_IF_ERR_EXIT
                         (bcmecmp_weighted_0_group_imm_register(
                                     unit, tables[idx]));

                    } else if (tables[idx] == ids->ecmp_level1_ltid) {
                        SHR_IF_ERR_EXIT
                         (bcmecmp_lvl1_group_imm_register(
                                     unit, tables[idx]));
                        break;
                    } else if (tables[idx] == ids->ecmp_wlevel1_ltid) {
                        SHR_IF_ERR_EXIT
                         (bcmecmp_weighted_1_group_imm_register(
                                     unit, tables[idx]));
                        break;

                    } else if (tables[idx] == ids->ecmp_mlevel0_ltid) {
                        SHR_IF_ERR_EXIT
                         (bcmecmp_member0_imm_register(
                                    unit, tables[idx]));
                        break;
                    } else if (tables[idx] == ids->ecmp_mlevel1_ltid) {
                        SHR_IF_ERR_EXIT
                         (bcmecmp_member1_imm_register(
                                    unit, tables[idx]));
                        break;
                    } else {
                        continue;
                    }
                }
        }
    }
exit:
    BCMECMP_FREE(tables);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configures ECMP resolution mode for this unit.
 *
 * \param [in] inst_cat System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start status.
 * \param [out] blocking_comp Pointer to the name of the blocking component.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 * \retval SHR_SYSM_RV_BLOCKED On being blocked by another component.
 */
static shr_sysm_rv_t
bcmecmp_unit_config(shr_sysm_categories_t inst_cat,
                    int unit,
                    void *comp_data,
                    bool warm,
                    uint32_t *blocking_comp)
{
    int rv;
    int idx;
    uint32_t comp_dep[] = {
        BCMMGMT_CFG_COMP_ID,
        BCMMGMT_LTD_COMP_ID,
        BCMMGMT_BD_COMP_ID,
        BCMMGMT_PTM_COMP_ID,
        BCMMGMT_DI_COMP_ID,
        BCMMGMT_SER_COMP_ID,
        BCMMGMT_IMM_FE_COMP_ID
    };

    for (idx = 0; idx < COUNTOF(comp_dep); idx++) {
        if (!shr_sysm_is_comp_complete(comp_dep[idx], inst_cat, unit)) {
            *blocking_comp = comp_dep[idx];
            return SHR_SYSM_RV_BLOCKED;
        }
    }

    rv = bcmecmp_attach(unit);
    if (SHR_FAILURE(rv)) {
        bcmecmp_cleanup(unit, warm);
        return SHR_SYSM_RV_ERROR;
    }

    rv = bcmecmp_dev_init(unit, warm);
    if (SHR_FAILURE(rv)) {
        bcmecmp_cleanup(unit, warm);
        /*
         * Unavailable error code returned for ECMP LTs,
         * when ECMP LT's are not supported for the device variant.
         * Do not treat this condition as an error,
         * Cleanup the resources allocated for ECMP
         * and set the return value to SHR_SYSM_RV_DONE.
         */
        if (rv == SHR_E_UNAVAIL) {
            return SHR_SYSM_RV_DONE;
        } else {
            return SHR_SYSM_RV_ERROR;
        }
    }

    /*
     * Program device ECMP resolution mode in hardware register, when booting
     * up in cold boot mode. Skip this step when booting up in warm boot mode,
     * as it's expected that the device was already programmed to desired
     * ECMP_MODE during previous cold boot initialization sequence.
     */
    if (BCMECMP_FNCALL_CHECK(unit, mode_config) && !warm) {
        rv = BCMECMP_FNCALL_EXEC(unit, mode_config)(unit);
        if (SHR_FAILURE(rv)) {
            bcmecmp_cleanup(unit, warm);
            return SHR_SYSM_RV_ERROR;
        }
    }

    rv = bcmecmp_imm_init(unit);
    if (SHR_FAILURE(rv)) {
        bcmecmp_cleanup(unit, warm);
        return SHR_SYSM_RV_ERROR;
    }

    if (ecmp_ctrl[unit]) {
        ecmp_ctrl[unit]->init = TRUE;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief ECMP pre-configuration function.
 *
 * \param [in] inst_cat System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start status.
 * \param [out] blocking_comp Pointer to the name of the blocking component.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 */
static shr_sysm_rv_t
bcmecmp_unit_pre_config(shr_sysm_categories_t inst_cat,
                        int unit,
                        void *comp_data,
                        bool warm,
                        uint32_t *blocking_comp)
{
    int rv;
    int idx;
    uint32_t comp_dep[] = {
        BCMMGMT_CFG_COMP_ID,
        BCMMGMT_LTD_COMP_ID,
        BCMMGMT_BD_COMP_ID,
        BCMMGMT_PTM_COMP_ID,
        BCMMGMT_DI_COMP_ID,
        BCMMGMT_SER_COMP_ID
    };

    for (idx = 0; idx < COUNTOF(comp_dep); idx++) {
        if (!shr_sysm_is_comp_complete(comp_dep[idx], inst_cat, unit)) {
            *blocking_comp = comp_dep[idx];
            return SHR_SYSM_RV_BLOCKED;
        }
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "warm = %s.\n"), warm ? "TRUE" : "FALSE"));

    if (warm) {
        rv = bcmecmp_dev_pre_config(unit, warm);
        if (SHR_FAILURE(rv)) {
            if (ecmp_ctrl[unit] && ecmp_ctrl[unit]->init) {
                ecmp_ctrl[unit]->init = FALSE;
            }
            return SHR_SYSM_RV_ERROR;
        }
    }

    if (!warm && ecmp_ctrl[unit]
       && ecmp_ctrl[unit]->ecmp_drv
       && ecmp_ctrl[unit]->ecmp_drv->memb_bank_set) {
        rv = bcmecmp_limit_control_pre_insert(unit);
        if (SHR_FAILURE(rv)) {
            if (ecmp_ctrl[unit] && ecmp_ctrl[unit]->init) {
                ecmp_ctrl[unit]->init = FALSE;
            }
            return SHR_SYSM_RV_ERROR;
        }
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief Shutsdown ECMP component on this BCM unit.
 *
 * \param [in] inst_cat System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start status.
 * \param [out] blocking_comp Pointer to the name of the blocking component.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 * \retval SHR_SYSM_RV_BLOCKED On being blocked by another component.
 */
static shr_sysm_rv_t
bcmecmp_unit_shutdown(shr_sysm_categories_t inst_cat,
                      int unit,
                      void *comp_data,
                      bool graceful,
                      uint32_t *blocking_comp)
{
    int rv;
    int idx;
    uint32_t comp_dep[] = {
        BCMMGMT_TRM_COMP_ID,
        BCMMGMT_LTM_COMP_ID
    };

    for (idx = 0; idx < COUNTOF(comp_dep); idx++) {
        if (!shr_sysm_is_comp_complete(comp_dep[idx], inst_cat, unit)) {
            *blocking_comp = comp_dep[idx];
            return SHR_SYSM_RV_BLOCKED;
        }
    }

    /* Cleanup resources allcoated for ECMP component. */
    rv = bcmecmp_cleanup(unit, graceful);
    if (SHR_FAILURE(rv)) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief ECMP component init.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmecmp_start(void)
{
    shr_sysm_cb_func_set_t  funcs;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    sal_memset(&funcs, 0, sizeof(funcs));

    /* Register ECMP component initialization function. */
    funcs.init = bcmecmp_unit_init;

    /* Register ECMP resolution mode hardware configuration function. */
    funcs.comp_config = bcmecmp_unit_config;

    /* Register ECMP pre-configuration function. */
    funcs.pre_config = bcmecmp_unit_pre_config;

    /* Register ECMP component shutdown/cleanup function. */
    funcs.shutdown = bcmecmp_unit_shutdown;

    /* Register ECMP component with System Manager as per-UNIT instance type. */
    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_ECMP_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &funcs,
                           NULL));
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Get BCM unit ECMP control structure pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Pointer to BCM unit's \c _u ECMP Control Structure.
 */
bcmecmp_control_t *bcmecmp_get_ecmp_control (int unit) {
   return ecmp_ctrl[unit];
}

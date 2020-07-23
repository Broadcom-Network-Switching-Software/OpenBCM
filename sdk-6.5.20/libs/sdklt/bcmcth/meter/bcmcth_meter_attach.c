/*! \file bcmcth_meter_attach.c
 *
 * Tunnel component interface to system manager.
 * This file implements the Tunnel interface to system manager.
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
#include <bcmevm/bcmevm_api.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_meter_fp_ing.h>
#include <bcmcth/bcmcth_meter_fp_egr.h>
#include <bcmcth/bcmcth_meter_fp_ing_imm.h>
#include <bcmcth/bcmcth_meter_fp_egr_imm.h>
#include <bcmcth/bcmcth_meter_sc.h>
#include <bcmcth/bcmcth_meter_sc_util.h>
#include <bcmcth/bcmcth_meter_l2_iif_sc.h>
#include <bcmcth/bcmcth_meter_fp_ing_action.h>
#include <bcmcth/bcmcth_meter_fp_egr_action.h>
#include <bcmcth/bcmcth_meter_drv.h>

#include <bcmcth/bcmcth_meter.h>

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*******************************************************************************
 * Private functions
 */

static int
bcmcth_meter_imm_register(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmcth_meter_fp_ing_imm_register(unit));
    SHR_IF_ERR_EXIT(bcmcth_meter_fp_egr_imm_register(unit));
    SHR_IF_ERR_EXIT(bcmcth_meter_l2_iif_sc_imm_register(unit));

exit:
    SHR_FUNC_EXIT();
}

static void
bcmcth_meter_imm_db_cleanup(int unit)
{
    bcmcth_meter_fp_ing_imm_db_cleanup(unit);
    bcmcth_meter_fp_egr_imm_db_cleanup(unit);
    bcmcth_meter_l2_iif_sc_imm_db_cleanup(unit);
}

/*!
 * \brief Initialize METER handler function software data structures.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Cold/Warmboot flag.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmcth_meter_init(int unit, bool warm)
{
    int rv = SHR_E_NONE;
    const bcmlrd_map_t *map = NULL;
    bcmcth_meter_fp_attrib_t *attr = NULL;
    bcmcth_meter_sc_drv_t *sc_drv = NULL;
    bcmcth_meter_action_drv_t *act_drv = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmcth_meter_fp_ing_attrib_get(unit, &attr);
    if (SHR_SUCCESS(rv) && attr != NULL) {
        rv = bcmlrd_map_get(unit, attr->ltid, &map);
        if (SHR_SUCCESS(rv) && map) {
            SHR_IF_ERR_EXIT(bcmcth_meter_fp_ing_init(unit, warm));
            if (bcmevm_register_published_event(unit,
                                                "FP_ING_COMMIT",
                                                bcmcth_meter_fp_event_cb)) {
                return SHR_SYSM_RV_ERROR;
            }

            if (bcmevm_register_published_event(unit,
                                                "FP_ING_ABORT",
                                                bcmcth_meter_fp_event_cb)) {
                return SHR_SYSM_RV_ERROR;
            }

            if (!warm) {
                SHR_IF_ERR_EXIT(bcmcth_meter_fp_ing_dev_config(unit));
            }
        }
    }

    rv = bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    if (SHR_SUCCESS(rv) && (attr != NULL)) {
        rv = bcmlrd_map_get(unit, attr->ltid, &map);
        if (SHR_SUCCESS(rv) && map) {
            SHR_IF_ERR_EXIT(bcmcth_meter_fp_egr_init(unit, warm));
            if (bcmevm_register_published_event(unit,
                                                "FP_EGR_COMMIT",
                                                bcmcth_meter_fp_event_cb)) {
                return SHR_SYSM_RV_ERROR;
            }

            if (bcmevm_register_published_event(unit,
                                                "FP_EGR_ABORT",
                                                bcmcth_meter_fp_event_cb)) {
                return SHR_SYSM_RV_ERROR;
            }
            if (!warm) {
                SHR_IF_ERR_EXIT(bcmcth_meter_fp_egr_dev_config(unit));
            }
        }
    }

    rv = bcmcth_meter_fp_ing_action_drv_get(unit, &act_drv);
    if (SHR_SUCCESS(rv) && (act_drv != NULL) && (act_drv->pdd_lt_info!= NULL)) {
        rv = bcmlrd_map_get(unit, act_drv->pdd_lt_info->ltid, &map);
        if (SHR_SUCCESS(rv) && map) {
            SHR_IF_ERR_EXIT(bcmcth_meter_fp_ing_action_init(unit, warm));
        }
    }

    rv = bcmcth_meter_fp_egr_action_drv_get(unit, &act_drv);
    if (SHR_SUCCESS(rv) && (act_drv != NULL) && (act_drv->pdd_lt_info != NULL)) {
        rv = bcmlrd_map_get(unit, act_drv->pdd_lt_info->ltid, &map);
        if (SHR_SUCCESS(rv) && map) {
            SHR_IF_ERR_EXIT(bcmcth_meter_fp_egr_action_init(unit, warm));
        }
    }

    rv = bcmcth_meter_sc_drv_get(unit, &sc_drv);
    if (SHR_SUCCESS(rv) && (sc_drv != NULL)) {
        rv = bcmlrd_map_get(unit, METER_STORM_CONTROLt, &map);
        if (SHR_SUCCESS(rv) && map) {
            SHR_IF_ERR_EXIT(bcmcth_meter_sc_init(unit, warm));
        }
    }

    SHR_IF_ERR_EXIT(bcmcth_meter_imm_register(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup METER handler function software data structures.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmcth_meter_cleanup(int unit)
{
    int                 rv = SHR_E_NONE;
    bcmcth_meter_fp_drv_t *drv = NULL;
    bcmcth_meter_sc_drv_t *sc_drv = NULL;
    bcmcth_meter_action_drv_t *act_drv = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmcth_meter_fp_ing_drv_get(unit, &drv);
    if (SHR_SUCCESS(rv) && drv != NULL) {
        SHR_IF_ERR_EXIT(bcmcth_meter_fp_ing_cleanup(unit));
    }

    rv = bcmcth_meter_fp_egr_drv_get(unit, &drv);
    if (SHR_SUCCESS(rv) && drv != NULL) {
        SHR_IF_ERR_EXIT(bcmcth_meter_fp_egr_cleanup(unit));
    }

    rv = bcmcth_meter_fp_ing_action_drv_get(unit, &act_drv);
    if (SHR_SUCCESS(rv) && (act_drv != NULL)) {
        SHR_IF_ERR_EXIT(bcmcth_meter_fp_ing_action_cleanup(unit));
    }

    rv = bcmcth_meter_fp_egr_action_drv_get(unit, &act_drv);
    if (SHR_SUCCESS(rv) && (act_drv != NULL)) {
        SHR_IF_ERR_EXIT(bcmcth_meter_fp_egr_action_cleanup(unit));
    }

    rv = bcmcth_meter_sc_drv_get(unit, &sc_drv);
    if (SHR_SUCCESS(rv) && (sc_drv != NULL)) {
        SHR_IF_ERR_EXIT(bcmcth_meter_sc_cleanup(unit));
    }

    bcmcth_meter_imm_db_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Attach METER drivers.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_meter_attach (int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmcth_meter_drv_init(unit));

    SHR_IF_ERR_EXIT(bcmcth_meter_init(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach METER drivers.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_meter_detach (int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmcth_meter_cleanup(unit));

    SHR_IF_ERR_EXIT(bcmcth_meter_drv_cleanup(unit));
exit:
    SHR_FUNC_EXIT();
}

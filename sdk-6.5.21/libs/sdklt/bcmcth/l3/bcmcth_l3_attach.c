/*! \file bcmcth_l3_attach.c
 *
 * L3 component interface to system manager.
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
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_l3_drv.h>
#include <bcmcth/bcmcth_l3_sysm.h>
#include <bcmcth/bcmcth_l3.h>
#include <bcmcth/bcmcth_l3_bank.h>
#include <bcmimm/bcmimm_int_comp.h>

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_L3

#define BCMCTH_L3_BANK_FIELD_COUNT_MAX 10

static shr_famm_hdl_t bcmcth_l3_bank_famm_hdl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Initialize device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 */
static int
bcmcth_l3_drv_attach(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT
        (bcmcth_l3_bank_drv_init(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clean up device driver.
 *
 * Release any resources allocated during attach.
 *
 * \param [in] unit Unit number.
 */
static int
bcmcth_l3_drv_detach(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Pre-insert the L3 LIMIT CONTROL LT with default values.
 *
 * \param [in] u Unit number.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
static int
bcmcth_l3_bank_control_pre_insert(int u)
{
    bcmltd_fields_t fields = {0};
    uint32_t f_cnt = 0;

    SHR_FUNC_ENTER(u);
    if (!bcmcth_l3_bank_famm_hdl[u]) {
        SHR_IF_ERR_EXIT(
            shr_famm_hdl_init(BCMCTH_L3_BANK_FIELD_COUNT_MAX,
                              &bcmcth_l3_bank_famm_hdl[u]));
    }
    fields.field = shr_famm_alloc(bcmcth_l3_bank_famm_hdl[u],
                                  BCMCTH_L3_BANK_FIELD_COUNT_MAX);
    if (fields.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    f_cnt = 0;
    fields.field[f_cnt  ]->id   = L3_LIMIT_CONTROLt_NHOP_OVERLAY_LIMITf;
    fields.field[f_cnt++]->data = 0;
    fields.field[f_cnt  ]->id   = L3_LIMIT_CONTROLt_EIF_OVERLAY_LIMITf;
    fields.field[f_cnt++]->data = 0;
    fields.count = f_cnt;
    SHR_IF_ERR_EXIT(
        bcmimm_entry_insert(u, L3_LIMIT_CONTROLt, &fields));
exit:
    if (fields.field) {
        shr_famm_free(bcmcth_l3_bank_famm_hdl[u], fields.field,
                      BCMCTH_L3_BANK_FIELD_COUNT_MAX);
    }
    if (bcmcth_l3_bank_famm_hdl[u]) {
        shr_famm_hdl_delete(bcmcth_l3_bank_famm_hdl[u]);
        bcmcth_l3_bank_famm_hdl[u] = 0;
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Initialize L3 handler function software data structures.
 *
 * \param [in] unit Unit number.
 * \param [in] cold Cold/Warmboot flag.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_l3_init(int unit, bool warm)
{
    int rv = 0;
    bcmcth_l3_bank_drv_t *drv = NULL;
    SHR_FUNC_ENTER(unit);

    if (bcmcth_l3_drv_attach(unit) != SHR_E_NONE) {
        SHR_IF_ERR_EXIT(SHR_SYSM_RV_ERROR);
    }

    (void) bcmcth_l3_imm_register(unit);

    rv = bcmcth_l3_bank_drv_get(unit, &drv);
    if (SHR_SUCCESS(rv) && (drv != NULL)) {
        SHR_IF_ERR_EXIT(bcmcth_l3_bank_control_init(unit, warm));
        if (!warm) {
            SHR_IF_ERR_EXIT
                (bcmcth_l3_bank_control_pre_insert(unit));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup tunnel handler function software data structures.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_l3_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmcth_l3_drv_detach(unit) != SHR_E_NONE) {
        SHR_IF_ERR_EXIT(SHR_SYSM_RV_ERROR);
    }

exit:
    SHR_FUNC_EXIT();
}

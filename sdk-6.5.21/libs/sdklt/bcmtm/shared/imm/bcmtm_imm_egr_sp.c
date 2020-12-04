/*! \file bcmtm_imm_egr_sp.c
 *
 * In-memory call back function for egress service pool.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/oobfc/bcmtm_oobfc_internal.h>
#include <bcmtm/bcmtm_utils_internal.h>
/*******************************************************************************
 * Local definitions
 */
/*! Egress service pool configuration. */
typedef struct bcmtm_egr_sp_cfg_s {

    /*! OOBFC service pool config. */
    bcmtm_oobfc_sp_cfg_t oobfc_sp_cfg;
} bcmtm_egr_sp_cfg_t;

#define BSL_LOG_MODULE BSL_LS_BCMTM_IMM
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Egress service pool configuration PT set.
 *
 * \param [in] unit Logical unit.
 * \param [in] ltid Logical table ID.
 * \param [in] sp_id Egress service pool ID.
 * \param [in] egr_sp_cfg Egress service pool configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_egr_sp_pt_set (int unit,
                     bcmltd_sid_t ltid,
                     uint8_t sp_id,
                     bcmtm_egr_sp_cfg_t *egr_sp_cfg)
{
    bcmtm_drv_t *drv;
    bcmtm_oobfc_drv_t oobfc_drv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&oobfc_drv, 0, sizeof(bcmtm_oobfc_drv_t));
    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));

    SHR_NULL_CHECK(drv->oobfc_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->oobfc_drv_get(unit, &oobfc_drv));
    /* update physical table. */
    if (oobfc_drv.egr_sp_set) {
        SHR_IF_ERR_EXIT
            (oobfc_drv.egr_sp_set(unit, ltid, sp_id,
                                  &(egr_sp_cfg->oobfc_sp_cfg)));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse egress service pool configuration.
 *
 * \param [in] unit Logical unit.
 * \param [in] data This is a linked list of the data fields in the modified
 * entry.
 * \param [out] egr_sp_cfg Egress service pool configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static void
bcmtm_egr_sp_cfg_parse (int unit,
                        const bcmltd_field_t *data,
                        bcmtm_egr_sp_cfg_t *egr_sp_cfg)
{
    uint32_t fid;
    uint64_t fval;
    bcmltd_field_t *fld = (bcmltd_field_t *)data;
    bcmtm_oobfc_sp_cfg_t *oobfc_sp_cfg = &(egr_sp_cfg->oobfc_sp_cfg);

    sal_memset(oobfc_sp_cfg, -1, sizeof(bcmtm_oobfc_sp_cfg_t));
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        switch (fid) {
            case TM_EGR_SERVICE_POOLt_OOBFC_MERGE_UC_Q_POOL_STATEf:
                oobfc_sp_cfg->ucq_merge = (int)fval;
                break;

            case TM_EGR_SERVICE_POOLt_OOBFC_MERGE_MC_Q_POOL_STATEf:
                oobfc_sp_cfg->mcq_merge = (int)fval;
                break;
        }
        fld = fld->next;
    }
}

/*!
 * \brief Egress service pool IMM callback function for staging.
 *
 * Handle TM_EGR_SERVICE_POOL imm logical table.
 *
 * \param [in] unit Logical unit.
 * \param [in] sid Logical table id.
 * \param [in] trans_id Transaction ID.
 * \param [in] event_reason This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [out] output_fields Read-only fields to be populated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_egr_sp_stage (int unit,
                    bcmltd_sid_t sid,
                    uint32_t trans_id,
                    bcmimm_entry_event_t event_reason,
                    const bcmltd_field_t *key,
                    const bcmltd_field_t *data,
                    void *context,
                    bcmltd_fields_t *output_fields)
{
    uint64_t fval = 0;
    uint8_t sp_id;
    bcmltd_fid_t fid;
    bcmtm_egr_sp_cfg_t egr_sp_cfg;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    /* TM_EGR_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf */
    fid = TM_EGR_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    sp_id = (uint8_t)fval;


    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_INSERT:
            bcmtm_egr_sp_cfg_parse(unit, data, &egr_sp_cfg);
            SHR_IF_ERR_EXIT
                (bcmtm_egr_sp_pt_set(unit, sid, sp_id, &egr_sp_cfg));
            break;
        case BCMIMM_ENTRY_DELETE:
            sal_memset(&egr_sp_cfg, 0, sizeof(bcmtm_egr_sp_cfg_t));
            SHR_IF_ERR_EXIT
                (bcmtm_egr_sp_pt_set(unit, sid, sp_id, &egr_sp_cfg));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * Public functions
 */
int
bcmtm_egr_sp_imm_register(int unit)
{
    /*! Inmemory callback function for TM_EGR_SERVICE_POOLt logical table. */
    bcmimm_lt_cb_t bcmtm_egr_sp_imm_cb = {
        /* staging function */
        .stage = bcmtm_egr_sp_stage,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_EGR_SERVICE_POOLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /*! Register callback functions for egress service pool settings. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_EGR_SERVICE_POOLt,
                                &bcmtm_egr_sp_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}

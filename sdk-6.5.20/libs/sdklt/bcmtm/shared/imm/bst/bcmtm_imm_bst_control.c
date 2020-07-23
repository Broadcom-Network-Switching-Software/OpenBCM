/*! \file bcmtm_imm_bst_control.c
 *
 * IMM APIs for BST CONTROL LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd_config.h>
#include <bcmtm/bst/bcmtm_bst_internal.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief BST control physical table update based on device type.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] bst_ctrl_cfg BST control configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error codes.
 */
static int
bcmtm_bst_control_pt_set(int unit,
                         bcmltd_sid_t ltid,
                         bcmtm_bst_control_cfg_t *bst_ctrl_cfg)
{
    bcmtm_drv_t *drv;
    bcmtm_bst_drv_t bst_drv;
    SHR_FUNC_ENTER(unit);

    sal_memset(&bst_drv, 0, sizeof(bcmtm_bst_drv_t));
    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));

    SHR_NULL_CHECK(drv->bst_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT
        (drv->bst_drv_get(unit, &bst_drv));

    if (bst_drv.bst_control_pt_set) {
        SHR_IF_ERR_EXIT
            (bst_drv.bst_control_pt_set(unit, ltid, bst_ctrl_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse the key and data fields
 *
 * \param [in] unit Unit number.
 * \param [in] data Array of Data fields
 * \param [out] bst_ctrl_cfg BST control configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static void
bcmtm_bst_control_lt_fields_parse(int unit,
                                  const bcmltd_field_t *data,
                                  bcmtm_bst_control_cfg_t *bst_ctrl_cfg)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        switch (fid) {
            case TM_BST_CONTROLt_TRACKING_MODEf:
                bst_ctrl_cfg->mode = (int)fval;
                break;
            case TM_BST_CONTROLt_CLEAR_ON_READf:
                bst_ctrl_cfg->clear_on_read = (bool)fval;
                break;
            case TM_BST_CONTROLt_SNAPSHOTf:
                bst_ctrl_cfg->snapshot = (bool)fval;
                break;
            default:
                break;
        }
        gen_field = gen_field->next;
    }
}

/*!
 * \brief TM BST control configuration imm entry lookup.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [out] bst_ctrl_cfg BST control configuration.
 */
static int
bcmtm_bst_control_imm_lkup(int unit,
                           bcmltd_sid_t ltid,
                           bcmtm_bst_control_cfg_t *bst_ctrl_cfg)
{
    bcmltd_fields_t out, in;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    /* only key */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 1, &in));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, TM_BST_CONTROLt_FIELD_COUNT, &out));

    in.count = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, TM_BST_CONTROLt_CLASSf, 0,
                              bst_ctrl_cfg->class));

    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        /* update bst control config. */
        bcmtm_bst_control_lt_fields_parse(unit, *(out.field), bst_ctrl_cfg);
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief BST Control callback function for staging.
 *
 * Handle the BST Control creation/modification parameters
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [out] output_fields Read only fields update to imm.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_bst_control_stage_callback(int unit,
                                 bcmltd_sid_t ltid,
                                 uint32_t trans_id,
                                 bcmimm_entry_event_t event_reason,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 void *context,
                                 bcmltd_fields_t *output_fields)
{
    uint64_t fval;
    bcmtm_bst_control_cfg_t bst_ctrl_cfg;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);
    if (output_fields) {
        output_fields->count = 0;
    }
    sal_memset(&bst_ctrl_cfg, 0, sizeof(bcmtm_bst_control_cfg_t));

    /* TM_BST_CONTROLt_CLASSf */
    fid = TM_BST_CONTROLt_CLASSf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    bst_ctrl_cfg.class = (uint8_t)fval;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmtm_bst_control_imm_lkup(unit, ltid, &bst_ctrl_cfg));
            /* Fall through */
        case BCMIMM_ENTRY_INSERT:
            bcmtm_bst_control_lt_fields_parse(unit, data, &bst_ctrl_cfg);
            break;
        case BCMIMM_ENTRY_DELETE:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT
        (bcmtm_bst_control_pt_set(unit, ltid, &bst_ctrl_cfg));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_imm_bst_control_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    bcmimm_lt_cb_t bcmtm_bst_control_imm_callback = {
        /*! Staging function. */
        .stage = bcmtm_bst_control_stage_callback,
    };

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for TM_BST_CONTROL LT.
     */
    rv = bcmlrd_map_get(unit, TM_BST_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 TM_BST_CONTROLt,
                                 &bcmtm_bst_control_imm_callback,
                                 NULL));
    }
exit:
    SHR_FUNC_EXIT();
}

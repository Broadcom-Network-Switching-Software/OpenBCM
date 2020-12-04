/*! \file bcmltx_l3mc_classid_or_dstmac_transform.c
 *
 * CLASS_ID/DST_MAC Transform Handler
 *
 * This file contains field transform information for
 * DST_MAC/CLASS_ID.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcml3/bcmltx_l3mc_classid_or_dstmac.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_L3

/*
 * \brief bcmltx_l3mc_classid_or_dstmac_transform is transform for CLASS_ID and DST_MAC
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              REPLACE_DST_MAC/CLASS_ID/DST_MAC.
 * \param [out] out             MAC_ADDRESS/L3_CLASS_ID.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_l3mc_classid_or_dstmac_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    uint64_t REPLACE_DST_MAC_field_val = 0;
    uint64_t DST_MAC_field_val = 0;
    uint64_t CLASS_ID_field_val = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "\t bcmltx_l3mc_classid_or_dstmac_transform\n")));

    /* No input no output */
    if (in->count == 0) {
       SHR_EXIT();
    }

    if (in->count != 3) {
       SHR_ERR_EXIT(SHR_E_PARAM);
    }

    REPLACE_DST_MAC_field_val = in->field[0]->data;
    DST_MAC_field_val   = in->field[1]->data;
    CLASS_ID_field_val   = in->field[2]->data;

    /* First  reverse argument[0] is always REPLACE_DST_MAC flag.
    * Second reverse argument[1] is DST_MAC.
    */
    out->field[0]->id   = arg->rfield[0];
    out->field[0]->data = REPLACE_DST_MAC_field_val;
    out->field[1]->id   = arg->rfield[1];
    if (REPLACE_DST_MAC_field_val) {
       out->field[1]->data = DST_MAC_field_val;
    } else {
       out->field[1]->data = CLASS_ID_field_val;
    }

exit:
    SHR_FUNC_EXIT();

}

/*
 * \brief bcmltx_l3mc_classid_or_dstmac_rev_transform is reverse
 * transform for CLASS_ID/DST_MAC
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              MAC_ADDRESS/L3_CLASS_ID.
 * \param [out] out             REPLACE_DST_MAC/CLASS_ID/DST_MAC.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_l3mc_classid_or_dstmac_rev_transform(int unit,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg)
{
    uint64_t REPLACE_DST_MAC_field_val = 0;
    uint64_t DST_MAC_field_val = 0;

    SHR_FUNC_ENTER(unit);

    if (in->count != 2) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* 1 indicates ECMP index, 0 indicates NEXT_HOP index. */
    REPLACE_DST_MAC_field_val = in->field[0]->data;
    DST_MAC_field_val = in->field[1]->data;

    /* First  reverse argument[0] is always ECMP_NHOP.
     * Second reverse argument[1] is alwasy NHOP_ID.
     * Third  reverse argument[2] is alwasy ECMP_ID.
     */
    out->field[0]->id = arg->rfield[0];
    out->field[0]->data = REPLACE_DST_MAC_field_val;
    if (REPLACE_DST_MAC_field_val) {
        out->field[1]->id = arg->rfield[1];
        out->field[1]->data = DST_MAC_field_val;
        out->field[2]->id = arg->rfield[2];
        out->field[2]->data = 0;
    } else {
        out->field[1]->id = arg->rfield[1];
        out->field[1]->data = 0;
        out->field[2]->id = arg->rfield[2];
        out->field[2]->data = DST_MAC_field_val;
    }

exit:
    SHR_FUNC_EXIT();

}

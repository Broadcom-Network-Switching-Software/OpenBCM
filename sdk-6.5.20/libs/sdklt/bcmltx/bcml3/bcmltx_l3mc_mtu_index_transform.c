/*! \file bcmltx_l3mc_mtu_index_transform.c
 *
 * TM_MC_GROUP_ID Transform Handler
 *
 * This file contains field transform information for
 * L3_MTU_VALUES index.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcml3/bcmltx_l3mc_mtu_index.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_L3

/*
 * \brief bcmltx_l3mc_mtu_index_transform is mc mtu index location
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              TM_MC_GROUP_ID.
 * \param [out] out             MTU_INDEX fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_l3mc_mtu_index_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    uint64_t tm_mc_group_id_field_val = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_l3mc_mtu_index_transform\n")));

    /* No input no output */
    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tm_mc_group_id_field_val = in->field[0]->data;

    /* First reverse argument[0] is always TM_MC_GROUP_ID */
    out->count = 0;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->data = tm_mc_group_id_field_val + L3_MC_MTU_OFFSET;
    out->field[out->count]->idx = 0;
    out->count++;

 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief bcmltx_l3mc_mtu_index_rev_transform is reverse transform for mc mtu index
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              MTU_INDEX.
 * \param [out] out             TM_MC_GROUP_ID fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_l3mc_mtu_index_rev_transform(int unit,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out,
                                    const bcmltd_transform_arg_t *arg)
{
    uint64_t mc_mtu_index_val = 0;

    SHR_FUNC_ENTER(unit);

    if (in->count != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* 0 indicates MTU_VALUE index */
    mc_mtu_index_val = in->field[0]->data;
    if (mc_mtu_index_val < L3_MC_MTU_OFFSET) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* First reverse argument[0] is always TM_MC_GROUP_ID. */
    out->count = 0;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->data = mc_mtu_index_val - L3_MC_MTU_OFFSET;
    out->count++;
exit:
    SHR_FUNC_EXIT();
}

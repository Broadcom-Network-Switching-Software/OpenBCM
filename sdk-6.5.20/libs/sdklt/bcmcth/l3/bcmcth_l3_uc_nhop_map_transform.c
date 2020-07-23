/*! \file bcmcth_l3_uc_nhop_map_transform.c
 *
 * L3 UC NHOP object map Transform Handler
 * This file contains field transform information for
 * L3_UN_NHOP LT fields NHOP_ID and L3_EIF_ID.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_l3_bank.h>
#include <bcmcth/bcmcth_l3_uc_nhop_map.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_L3

/*******************************************************************************
 * Public functions
 */

/*
 * \brief bcmcth_l3_uc_nhop_map_transform is forward transform
 * for L3_UC_NHOP.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  key             Input key values.
 * \param [in]  in              Input field values.
 * \param [out] out             Output field values.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmcth_l3_uc_nhop_map_transform(int unit,
                                const bcmltd_fields_t *key,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    uint32_t    field_count = 0;
    uint32_t    nhop_id = 0;
    int eif_id = 0;
    bool eif_valid = 0;
    bool is_ovly = 0;
    /*uint32_t ul_nhop_id = 0;*/

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    field_count = in->count;
    if (field_count == 0 || key->count == 0) {
        SHR_EXIT();
    }

    if ((field_count < 2) || (in->field == NULL) || (key->field == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((out == NULL) || (arg == NULL) || (arg->rfield_list == NULL) ||
        (arg->kfield_list == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    nhop_id = key->field[0]->data;
    eif_id = in->field[0]->data;
    eif_valid = in->field[1]->data;
    bcmcth_l3_bank_nhop_id_overlay(unit, nhop_id, &is_ovly);
    out->count = 0;
    if (is_ovly) {
        /*! U_INTF_NUM_VALID. */
        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->idx = 0;
        out->field[out->count]->data = 0;
        out->count++;
        /*! U_INTF_NUM. */
        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->idx = 0;
        out->field[out->count]->data = 0;
        out->count++;
        /*! O_INTF_NUM_VALID. */
        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->idx = 0;
        out->field[out->count]->data = eif_valid;
        out->count++;
        /*! O_INTF_NUM. */
        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->idx = 0;
        out->field[out->count]->data = eif_id;
        out->count++;
    } else {
        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->idx = 0;
        out->field[out->count]->data = eif_valid;
        out->count++;

        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->idx = 0;
        out->field[out->count]->data = eif_id;
        out->count++;

        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->idx = 0;
        out->field[out->count]->data = 0;
        out->count++;

        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->idx = 0;
        out->field[out->count]->data = 0;
        out->count++;
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief bcmcth_l3_uc_nhop_map_rev_transform is reverse transform
 * for l3_uc_nhop_map.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  key             Input key values.
 * \param [in]  in              Input field values.
 * \param [out] out             Output field values.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmcth_l3_uc_nhop_map_rev_transform(int unit,
                                    const bcmltd_fields_t *key,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out,
                                    const bcmltd_transform_arg_t *arg)
{
    uint32_t field_count = 0;
    int eif_id = 0;
    bool eif_valid = 0;
    int o_eif_id = 0;
    bool o_eif_valid = 0;
    bool is_ovly = 0;
    uint32_t nhop_id = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    field_count = in->count;

    if (field_count == 0 || key->count == 0) {
        SHR_EXIT();
    }

    if ((field_count < 4) || (in->field == NULL) || (key->field == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((out == NULL) || (arg == NULL) || (arg->rfield_list == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    nhop_id = key->field[0]->data;
    eif_valid = in->field[0]->data;
    eif_id = in->field[1]->data;
    o_eif_valid = in->field[2]->data;
    o_eif_id = in->field[3]->data;
    bcmcth_l3_bank_nhop_id_overlay(unit, nhop_id, &is_ovly);
    out->count = 0;
    if (is_ovly) {
        /*! L3_EIF_ID. */
        out->field[0]->data = o_eif_id;
        out->field[0]->id   = arg->rfield[out->count];
        out->field[0]->idx  = 0;
        out->count++;

        /*! L3_EIF_VALID. */
        out->field[1]->data = o_eif_valid;
        out->field[1]->id   = arg->rfield[out->count];
        out->field[1]->idx  = 0;
        out->count++;
    } else {
        /*! L3_EIF_ID. */
        out->field[0]->data = eif_id;
        out->field[0]->id   = arg->rfield[out->count];
        out->field[0]->idx  = 0;
        out->count++;

        /*! L3_EIF_VALID. */
        out->field[1]->data = eif_valid;
        out->field[1]->id   = arg->rfield[out->count];
        out->field[1]->idx  = 0;
        out->count++;
    }
exit:
    SHR_FUNC_EXIT();
}

/*! \file bcmcth_tnl_l3_eif_transform.c
 *
 * L3_EIF.TNL_ENCAP_ID and L3_EIF.L3_EIF_ID Transform Handler
 * This file contains field transform information for
 * L3_EIF LT fields L3_EIF_ID and TNL_ENCAP_ID.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_tnl_l3_intf.h>
#include <bcmcth/bcmcth_tnl_encap_id.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_TNL

/*******************************************************************************
 * Public functions
 */

/*
 * \brief bcmcth_tnl_encap_id_transform is forward transform
 * for TNL_ENCAP_ID.
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
bcmcth_tnl_encap_id_transform(int unit,
                              const bcmltd_fields_t *key,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg)
{
    uint32_t    field_count = 0;
    uint32_t    fid = 0, fval = 0;
    uint32_t    idx = 0;
    uint8_t     tnl_type = 0;
    uint32_t    tnl_ipv4_id = 0, tnl_ipv6_id = 0, tnl_mpls_id = 0;
    uint32_t    hw_tnl_idx = 0;
    uint32_t    l3_eif_id = 0;
    uint32_t    shift_bits = 0;
    bcmcth_tnl_l3_intf_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    field_count = in->count;
    if (field_count == 0 || key->count == 0) {
        SHR_EXIT();
    }

    if ((field_count < 2) || (in->field == NULL) || (key->field == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((out == NULL) || (arg == NULL) || (arg->value == NULL) ||
        (arg->rfield_list == NULL) ||
        (arg->kfield_list == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(bcmcth_tnl_l3_intf_state_get(unit, &state));
    l3_eif_id = key->field[0]->data;

    for (idx = 0; idx < field_count; idx++) {
        if (in->field[idx] == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        fid = in->field[idx]->id;
        fval = in->field[idx]->data;

        switch (fid) {
        case L3_EIFt_TNL_TYPEf:
            tnl_type = fval;
            state->intf[l3_eif_id].type = tnl_type;
            break;
        case L3_EIFt_TNL_IPV4_ENCAP_INDEXf:
            tnl_ipv4_id = fval;
            state->intf[l3_eif_id].tnl_ipv4_id = tnl_ipv4_id;
            break;
        case L3_EIFt_TNL_IPV6_ENCAP_INDEXf:
            tnl_ipv6_id = fval;
            state->intf[l3_eif_id].tnl_ipv6_id = tnl_ipv6_id;
            break;
        case L3_EIFt_TNL_MPLS_ENCAP_INDEXf:
            tnl_mpls_id = fval;
            state->intf[l3_eif_id].tnl_mpls_id = tnl_mpls_id;
            break;
        default:
            break;
        }
    }

    /*
     * Convert TNL_ENCAP_ID to EGR_L3_INTF.MPLS_TUNNEL_INDEX.
     */
    shift_bits = arg->value[0];
    if (tnl_type == BCMCTH_TNL_TYPE_IPV4) {
        hw_tnl_idx = (tnl_ipv4_id << shift_bits);
    } else if (tnl_type == BCMCTH_TNL_TYPE_IPV6) {
        hw_tnl_idx = ((tnl_ipv6_id * 2) << shift_bits);
    } else if (tnl_type == BCMCTH_TNL_TYPE_MPLS) {
        hw_tnl_idx = tnl_mpls_id;
    } else {
        hw_tnl_idx = 0;
    }

    out->count = 0;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->data = hw_tnl_idx;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief bcmcth_tnl_encap_id_rev_transform is reverse transform
 * for TNL_ENCAP_ID.
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
bcmcth_tnl_encap_id_rev_transform(int unit,
                                  const bcmltd_fields_t *key,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    uint32_t    field_count = 0;
    uint32_t    idx = 0, fid = 0;
    uint8_t     tnl_type = 0;
    uint32_t    tnl_ipv4_id = 0, tnl_ipv6_id = 0, tnl_mpls_id = 0;
    uint32_t    l3_eif_id = 0;
    bcmltd_field_t **flist = NULL;
    const bcmltd_field_desc_t *finfo = NULL;
    bcmcth_tnl_l3_intf_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    field_count = in->count;

    if (field_count == 0 || key->count == 0) {
        SHR_EXIT();
    }

    if ((field_count < 1) || (in->field == NULL) || (key->field == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((out == NULL) || (arg == NULL) || (arg->rfield_list == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(bcmcth_tnl_l3_intf_state_get(unit, &state));
    l3_eif_id = key->field[0]->data;

    tnl_type = state->intf[l3_eif_id].type;
    tnl_ipv4_id   = state->intf[l3_eif_id].tnl_ipv4_id;
    tnl_ipv6_id   = state->intf[l3_eif_id].tnl_ipv6_id;
    tnl_mpls_id   = state->intf[l3_eif_id].tnl_mpls_id;

    flist = out->field;
    finfo = arg->rfield_list->field_array;
    field_count = arg->rfield_list->field_num;

    out->count = 0;

    for (idx = 0; idx < field_count; idx++) {
        if (flist[idx] == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        fid = finfo[idx].field_id;
        flist[idx]->id = fid;
        switch (fid) {
        case L3_EIFt_TNL_TYPEf:
            flist[idx]->data = tnl_type;
            out->count++;
            break;
        case L3_EIFt_TNL_IPV4_ENCAP_INDEXf:
            flist[idx]->data = tnl_ipv4_id;
            out->count++;
            break;
        case L3_EIFt_TNL_IPV6_ENCAP_INDEXf:
            flist[idx]->data = tnl_ipv6_id;
            out->count++;
            break;
        case L3_EIFt_TNL_MPLS_ENCAP_INDEXf:
            flist[idx]->data = tnl_mpls_id;
            out->count++;
            break;
        default:
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

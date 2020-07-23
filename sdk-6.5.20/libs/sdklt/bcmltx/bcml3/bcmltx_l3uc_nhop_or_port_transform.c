/*! \file bcmltx_l3uc_nhop_or_port_transform.c
 *
 * ING_UNDERLAY_NHOP/IS_TRUNK/MODPORT/TRUNK_ID Transform Handler
 *
 * This file contains field transform information for
 * L3_UC_NHOP fields.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcml3/bcmltx_l3uc_nhop_or_port.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_L3

/*
 * \brief bcmltx_l3uc_nhop_or_port_transform is forward transform for
 * \ING_UNDERLAY_NHOP/IS_TRUNK/MODPORT/TRUNK_ID.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              ING_UNDERLAY_NHOP/IS_TRUNK/MODPORT/TRUNK_ID
 * \param [out] out             U_NHI_VALID/U_NHI/T/PORT_NUM/TGID fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_l3uc_nhop_or_port_transform(int unit,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg)
{
    uint64_t ul_nho_val = 0;
    bool ul_nho_valid = 0;
    uint64_t mod_port_val = 0;
    uint64_t trunk_id_val = 0;
    bool is_trunk = 0;
    uint32_t    fid = 0, fval = 0;
    uint32_t    idx = 0;

    SHR_FUNC_ENTER(unit);

    /* No input no output */
    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count < 2 || (in->field == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((out == NULL) || (arg == NULL) ||
        (arg->rfield_list == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (idx = 0; idx < in->count; idx++) {
        if (in->field[idx] == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        fid = in->field[idx]->id;
        fval = in->field[idx]->data;

        if (fid == L3_UC_NHOPt_ING_UNDERLAY_NHOP_VALIDf ||
            fid == TNL_MPLS_ENCAP_NHOPt_ING_UNDERLAY_NHOP_VALIDf ||
            fid == TNL_MPLS_TRANSIT_NHOPt_ING_UNDERLAY_NHOP_VALIDf) {
                ul_nho_valid = fval;
        } else if (fid == L3_UC_NHOPt_ING_UNDERLAY_NHOP_IDf ||
                 fid == TNL_MPLS_ENCAP_NHOPt_ING_UNDERLAY_NHOP_IDf ||
                 fid == TNL_MPLS_TRANSIT_NHOPt_ING_UNDERLAY_NHOP_IDf) {
                ul_nho_val = fval;
        } else if (fid == L3_UC_NHOPt_IS_TRUNKf ||
                fid == TNL_MPLS_ENCAP_NHOPt_IS_TRUNKf ||
                fid == TNL_MPLS_TRANSIT_NHOPt_IS_TRUNKf) {
                is_trunk = fval;
        } else if (fid == L3_UC_NHOPt_MODPORTf ||
                   fid == TNL_MPLS_ENCAP_NHOPt_MODPORTf ||
                   fid == TNL_MPLS_TRANSIT_NHOPt_MODPORTf) {
                mod_port_val = fval;
        } else if (fid == L3_UC_NHOPt_TRUNK_IDf ||
                   fid == TNL_MPLS_ENCAP_NHOPt_TRUNK_IDf ||
                   fid == TNL_MPLS_TRANSIT_NHOPt_TRUNK_IDf) {
                trunk_id_val = fval;
        }
    }

    out->count = 0;
    if (!ul_nho_valid) {
        /*! U_NHI_VALID. */
        out->field[out->count]->id = arg->rfield[0];
        out->field[out->count]->idx = 0;
        out->field[out->count]->data = 0;
        out->count++;

        /*! U_NHI. */

        /*! T. */
        out->field[out->count]->id = arg->rfield[2];
        out->field[out->count]->idx = 0;
        out->field[out->count]->data = is_trunk;
        out->count++;

        if (!is_trunk) {
            /*! PORT_NUM. */
            out->field[out->count]->id = arg->rfield[3];
            out->field[out->count]->idx = 0;
            out->field[out->count]->data = mod_port_val;
            out->count++;
        } else {
            /*! TGID. */
            out->field[out->count]->id = arg->rfield[4];
            out->field[out->count]->idx = 0;
            out->field[out->count]->data = trunk_id_val;
            out->count++;
        }
     } else {
         /*! U_NHI_VALID. */
         out->field[out->count]->id = arg->rfield[0];
         out->field[out->count]->idx = 0;
         out->field[out->count]->data = 1;
         out->count++;

         /*! U_NHI. */
         out->field[out->count]->id = arg->rfield[1];
         out->field[out->count]->idx = 0;
         out->field[out->count]->data = ul_nho_val;
         out->count++;
     }

 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief bcmltx_l3uc_nhop_or_port_rev_transform is reverse transform
 * \ING_UNDERLAY_NHOP/IS_TRUNK/MODPORT/TRUNK_ID.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              ING_UNDERLAY_NHOP/IS_TRUNK/MODPORT/TRUNK_ID
 * \param [out] out             U_NHI_VALID/U_NHI/T/PORT_NUM/TGID fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_l3uc_nhop_or_port_rev_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    uint64_t ul_nhop_val = 0;
    bool ul_nhop_valid = 0;
    uint64_t mod_port_val = 0;
    uint64_t trunk_id_val = 0;
    bool is_trunk = 0;

    SHR_FUNC_ENTER(unit);

    /* No input no output */
    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 5 || (in->field == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((out == NULL) || (arg == NULL) ||
        (arg->rfield_list == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    ul_nhop_valid = in->field[0]->data;
    ul_nhop_val = in->field[1]->data;
    is_trunk = in->field[2]->data;
    mod_port_val = in->field[3]->data;
    trunk_id_val = in->field[4]->data;

    out->count = 0;
    /*! ING_UNDERLAY_NHOP_VALID. */
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->field[out->count]->data = ul_nhop_valid;
    out->count++;
    if (!ul_nhop_valid) {
        /*! ING_UNDERLAY_NHOP_ID. */

        /*! IS_TRUNK. */
        out->field[out->count]->id = arg->rfield[2];
        out->field[out->count]->idx = 0;
        out->field[out->count]->data = is_trunk;
        out->count++;

        if (!is_trunk) {
            /*! MODPORT. */
            out->field[out->count]->id = arg->rfield[3];
            out->field[out->count]->idx = 0;
            out->field[out->count]->data = mod_port_val;
            out->count++;

            /*! TGID. */
        } else {
            /*! MODPORT. */

            /*! TGID. */
            out->field[out->count]->id = arg->rfield[4];
            out->field[out->count]->idx = 0;
            out->field[out->count]->data = trunk_id_val;
            out->count++;
        }
     } else {
        /*! ING_UNDERLAY_NHOP_ID. */
        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->idx = 0;
        out->field[out->count]->data = ul_nhop_val;
        out->count++;
     }

exit:
    SHR_FUNC_EXIT();
}

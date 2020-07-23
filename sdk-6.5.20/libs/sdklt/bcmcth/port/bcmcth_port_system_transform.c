/*! \file bcmcth_port_system_transform.c
 *
 * PORT_SYSTEM.L3_IIF_ID/VRF_ID and PORT_SYSTEM.PORT_SYSTEM_ID Transform Handler
 * This file contains field transform information for
 * PORT_SYSTEM LT fields PORT_SYSTEM_ID and VRF_ID/L3_IIF_ID.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_port_system.h>
#include <bcmcth/bcmcth_port_system_vrf_l3_iif.h>

#include <bcmcth/bcmcth_port_system_id.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_PORT

/*!
 * PORT_SYSTEM_ID - shared data between PORT_SYSTEM_ID
 * transform and PORT_SYSTEM.VRF_ID/L3_IIF_ID transform.
 */
uint32_t    bcmcth_port_system_id;

/*******************************************************************************
 * Public functions
 */

/*
 * \brief bcmcth_port_system_vrf_l3_iif_transform is forward transform
 * for L3_IIF_ID/VRF_ID.
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
bcmcth_port_system_vrf_l3_iif_transform(int unit,
                                        const bcmltd_fields_t *key,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    uint32_t    field_count = 0;
    uint32_t    fid = 0, fval = 0;
    uint32_t    idx = 0;
    uint8_t     op_mode = 0;
    uint32_t    l3_iif_id = 0, vrf_id = 0;
    uint32_t    vrf_l3_iif = 0;
    uint32_t    vlan_id = 0;
    uint32_t    port_system_id = 0;
    bcmcth_port_system_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    field_count = in->count;
    if ((field_count == 0) || (key->count == 0)) {
        SHR_EXIT();
    }

    if ((field_count < 2) || (in->field == NULL) || (key->field == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((out == NULL) || (arg == NULL) || (arg->rfield_list == NULL) ||
        (arg->kfield_list == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmcth_port_system_state_get(unit, &state));
    port_system_id = key->field[0]->data;
    for (idx = 0; idx < field_count; idx++) {
        if (in->field[idx] == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        fid = in->field[idx]->id;
        fval = in->field[idx]->data;

        switch (fid) {
        case PORT_SYSTEMt_OPERATING_MODEf:
            op_mode = fval;
            state->info[port_system_id].operation_mode = op_mode;
            break;
        case PORT_SYSTEMt_L3_IIF_IDf:
            l3_iif_id = fval;
            state->info[port_system_id].l3_iif_id = l3_iif_id;
            break;
        case PORT_SYSTEMt_VRF_IDf:
            vrf_id = fval;
            state->info[port_system_id].vrf_id = vrf_id;
            break;
        case PORT_SYSTEMt_VLAN_IDf:
            vlan_id = fval;
            state->info[port_system_id].vlan_id = vlan_id;
            break;
        default:
            break;
        }
    }

    if (op_mode == BCMCTH_PORT_SYSTEM_OP_MODE_VRF) {
        vrf_l3_iif = vrf_id;
    } else if (op_mode == BCMCTH_PORT_SYSTEM_OP_MODE_L3_IIF) {
        vrf_l3_iif = l3_iif_id;
    } else if (op_mode == BCMCTH_PORT_SYSTEM_OP_MODE_VLAN) {
        vrf_l3_iif = vlan_id;
    } else {
        vrf_l3_iif = 0;
    }

    out->count = 0;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->data = vrf_l3_iif;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief bcmcth_port_system_vrf_l3_iif_rev_transform is reverse transform
 * for L3_IIF_ID/VRF_ID.
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
bcmcth_port_system_vrf_l3_iif_rev_transform(int unit,
                                            const bcmltd_fields_t *key,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg)
{
    uint32_t    field_count = 0;
    uint32_t    idx = 0, fid = 0;
    uint8_t     op_mode = 0;
    uint32_t    l3_iif_id = 0, vrf_id = 0;
    uint32_t    vlan_id = 0;
    uint32_t    port_system_id = 0;
    bcmltd_field_t **flist = NULL;
    const bcmltd_field_desc_t *finfo = NULL;
    bcmcth_port_system_state_t *state = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    field_count = in->count;

    if ((field_count == 0) || (key->count == 0)) {
        SHR_EXIT();
    }

    if ((field_count < 1) || (in->field == NULL) || (key->field == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((out == NULL) || (arg == NULL) || (arg->rfield_list == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmcth_port_system_state_get(unit, &state));
    port_system_id = key->field[0]->data;

    op_mode = state->info[port_system_id].operation_mode;
    l3_iif_id = state->info[port_system_id].l3_iif_id;
    vrf_id = state->info[port_system_id].vrf_id;
    vlan_id = state->info[port_system_id].vlan_id;

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
        case PORT_SYSTEMt_OPERATING_MODEf:
            flist[idx]->data = op_mode;
            out->count++;
            break;
        case PORT_SYSTEMt_L3_IIF_IDf:
            flist[idx]->data = l3_iif_id;
            out->count++;
            break;
        case PORT_SYSTEMt_VRF_IDf:
            flist[idx]->data = vrf_id;
            out->count++;
            break;
        case PORT_SYSTEMt_VLAN_IDf:
            flist[idx]->data = vlan_id;
            out->count++;
            break;
        default:
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

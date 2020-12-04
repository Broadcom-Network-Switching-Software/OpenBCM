/*! \file bcmltx_l2_fdb_vlan_dest_v2.c
 *
 * L2_FDB_VLAN.DEST_TYPE/TRUNK_ID/MODID/MODPORT/L2_MC_GRP Transform Handler
 *
 * This file contains field transform information for
 * L2_FDB_VLAN.DEST_TYPE/TRUNK_ID/MODPORT/L2_MC_GRP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcml2/bcmltx_l2_fdb_vlan_dest_v2.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_L2

#define LT_FLD_IDX_DEST_TYPE     0
#define LT_FLD_IDX_TRUNK_ID      1
#define LT_FLD_IDX_MODPORT       2
#define LT_FLD_IDX_L2_MC_GRP_ID  3

#define PT_FLD_IDX_DEST_TYPE     0
#define PT_FLD_IDX_TGID          1
#define PT_FLD_IDX_PORT_NUM      2
#define PT_FLD_IDX_L2MC_PTR      3


typedef enum lt_dest_type_s {
    LT_PORT      = 0,
    LT_L2_MC_GRP = 1,
    LT_TRUNK     = 2
} lt_dest_type_t;
bcmltx_l2_mac_type_t bcmltx_l2_mac_type2;
/******************************************************************************
* Public functions
 */
/*
 * \brief L2_FDB_VLAN destination forward transform.
 *
 * \param [in]  unit Unit number.
 * \param [in]  in   L2_FDB_VLAN.DEST_TYPE,
 *                   MODPORT,TRUNK_ID,L2_MC_GRP_ID.
 * \param [out] out  L2X.T/TGID/PORT_NUM/L2MC_PTR fields.
 * \param [in]  arg  Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_l2_fdb_vlan_dest_v2_transform(int unit,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg)
{
    size_t i; /* logical in field index */
    int rv = SHR_E_NONE;
    uint64_t lt_dest_type_field_val = 0;
    uint64_t tgid_field_val = 0;
    uint64_t port_num_field_val = 0;
    uint64_t l2mc_ptr_field_val = 0;
    bcmltx_l2_mac_type_t *shared_data;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_l2_fdb_vlan_dest_v2_transform\n")));

    shared_data = arg->comp_data->user_data;
    if (in->count <= 0) {
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }

    for (i = 0; i < in->count; i++) {
        if (in->field[i]->id == arg->field[LT_FLD_IDX_DEST_TYPE]) {
            lt_dest_type_field_val = in->field[i]->data;
        } else if (in->field[i]->id == arg->field[LT_FLD_IDX_TRUNK_ID]) {
            tgid_field_val = in->field[i]->data;
        } else if (in->field[i]->id == arg->field[LT_FLD_IDX_MODPORT]) {
            port_num_field_val = in->field[i]->data;
        } else if (in->field[i]->id == arg->field[LT_FLD_IDX_L2_MC_GRP_ID]) {
            l2mc_ptr_field_val = in->field[i]->data;
        } else {
            rv = SHR_E_PARAM;
            SHR_ERR_EXIT(rv);
        }
    }

    if (shared_data->is_mc == TRUE) {
        if (lt_dest_type_field_val != LT_L2_MC_GRP) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        if (lt_dest_type_field_val == LT_L2_MC_GRP) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    out->count = 0;
    if (lt_dest_type_field_val == LT_PORT) {
        out->field[out->count]->id   = arg->rfield[PT_FLD_IDX_DEST_TYPE];
        out->field[out->count]->data = 0;
        out->count++;
        out->field[out->count]->id = arg->rfield[PT_FLD_IDX_PORT_NUM];
        out->field[out->count]->data = port_num_field_val;
        out->count++;
    } else if (lt_dest_type_field_val == LT_TRUNK) {
        out->field[out->count]->id = arg->rfield[PT_FLD_IDX_DEST_TYPE];
        out->field[out->count]->data = 1;
        out->count++;
        out->field[out->count]->id = arg->rfield[PT_FLD_IDX_TGID];
        out->field[out->count]->data = tgid_field_val;
        out->count++;
    } else {
        out->field[out->count]->id = arg->rfield[PT_FLD_IDX_DEST_TYPE];
        out->field[out->count]->data = 0;
        out->count++;
        out->field[out->count]->id = arg->rfield[PT_FLD_IDX_L2MC_PTR];
        out->field[out->count]->data = l2mc_ptr_field_val;
        out->count++;
    }

    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief L2_FDB_VLAN destination reverse transform.
 *
 * \param [in]  unit Unit number.
 * \param [in]  in   L2X.T/TGID/PORT_NUM/L2MC_PTR fields.
 * \param [out] out  L2_FDB_VLAN.DEST_TYPE,
 *                   MODPORT,TRUNK_ID,L2_MC_GRP_ID
 * \param [in]  arg  Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_l2_fdb_vlan_dest_v2_rev_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    size_t i;
    int rv = SHR_E_NONE;
    uint64_t dest_type_field_val = 0;
    uint64_t trunk_id_field_val = 0;
    uint64_t modport_field_val = 0;
    uint64_t l2_mc_grp_id_field_val = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "\t bcmltx_l2_fdb_vlan_dest_v2_rev_transform\n")));
    for (i = 0; i < in->count; i++) {
        if (in->field[i]->id == arg->field[PT_FLD_IDX_DEST_TYPE]) {
            dest_type_field_val = in->field[i]->data;
        } else if (in->field[i]->id == arg->field[PT_FLD_IDX_TGID]) {
            trunk_id_field_val = in->field[i]->data;
        } else if (in->field[i]->id == arg->field[PT_FLD_IDX_PORT_NUM]) {
            modport_field_val = in->field[i]->data;
        } else if (in->field[i]->id == arg->field[PT_FLD_IDX_L2MC_PTR]) {
            l2_mc_grp_id_field_val = in->field[i]->data;
        }else {
            rv = SHR_E_PARAM;
            SHR_ERR_EXIT(rv);
        }
    }

    out->count = 0;
    if (dest_type_field_val == 1) {
        out->field[out->count]->id = arg->rfield[LT_FLD_IDX_DEST_TYPE];
        out->field[out->count]->data = LT_TRUNK;
        out->count++;
        out->field[out->count]->id = arg->rfield[LT_FLD_IDX_TRUNK_ID];
        out->field[out->count]->data = trunk_id_field_val;
        out->count++;
        out->field[out->count]->id = arg->rfield[LT_FLD_IDX_MODPORT];
        out->field[out->count]->data = 0;
        out->count++;
        out->field[out->count]->id = arg->rfield[LT_FLD_IDX_L2_MC_GRP_ID];
        out->field[out->count]->data = 0;
        out->count++;
    } else if (dest_type_field_val == 0) {
        if (((bcmltx_l2_mac_type_t *)arg->comp_data->user_data)->is_mc == TRUE) {
            out->field[out->count]->id = arg->rfield[LT_FLD_IDX_DEST_TYPE];
            out->field[out->count]->data = LT_L2_MC_GRP;
            out->count++;
            out->field[out->count]->id = arg->rfield[LT_FLD_IDX_L2_MC_GRP_ID];
            out->field[out->count]->data = l2_mc_grp_id_field_val;
            out->count++;
            out->field[out->count]->id = arg->rfield[LT_FLD_IDX_TRUNK_ID];
            out->field[out->count]->data = 0;
            out->count++;
            out->field[out->count]->id = arg->rfield[LT_FLD_IDX_MODPORT];
            out->field[out->count]->data = 0;
            out->count++;
        } else {
            out->field[out->count]->id = arg->rfield[LT_FLD_IDX_DEST_TYPE];
            out->field[out->count]->data = LT_PORT;
            out->count++;
            out->field[out->count]->id = arg->rfield[LT_FLD_IDX_MODPORT];
            out->field[out->count]->data = modport_field_val;
            out->count++;
            out->field[out->count]->id = arg->rfield[LT_FLD_IDX_TRUNK_ID];
            out->field[out->count]->data = 0;
            out->count++;
            out->field[out->count]->id = arg->rfield[LT_FLD_IDX_L2_MC_GRP_ID];
            out->field[out->count]->data = 0;
            out->count++;
        }
    } else {
        rv = SHR_E_PARAM;
    }

    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

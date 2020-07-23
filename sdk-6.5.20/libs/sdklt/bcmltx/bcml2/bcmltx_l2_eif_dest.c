/*! \file bcmltx_l2_eif_dest.c
 *
 * L2_EIF_SYSTEM_DESTINATION.IS_TRUNK/SYSTEM_PORT/TRUNK_ID Transform Handler
 *
 * This file contains field transform information for
 * L2_EIF_SYSTEM_DESTINATION.IS_TRUNK/SYSTEM_PORT/TRUNK_ID.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcml2/bcmltx_l2_eif_dest.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_L2

#define LT_FLD_IDX_IS_TRUNK     0
#define LT_FLD_IDX_SYS_PORT     1
#define LT_FLD_IDX_TRUNK_ID     2

#define PT_FLD_IDX_DEST_TYPE    0
#define PT_FLD_IDX_SYS_DEST     1

/*
 * \brief L2_EIF_SYSTEM_DESTINATION.IS_TRUNK/SYSTEM_PORT/TRUNK_ID
 *  forward transform
 *
 * \param [in]  unit   Unit number.
 * \param [in]  in     L2_EIF_SYSTEM_DESTINATION.IS_TRUNK/SYSTEM_PORT/TRUNK_ID.
 * \param [out] out    IPOST_LAG_L2OIF.SYSTEM_DEST_TYPE/SYSTEM_DESTINATION.
 * \param [in]  arg    Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_l2_eif_dest_transform(int unit,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_transform_arg_t *arg)
{
    size_t i = 0; /* logical in field index */
    int rv = SHR_E_NONE;
    uint64_t is_trunk_field_val = 0;
    uint64_t tid_field_val = 0;
    uint64_t sys_port_field_val = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "\t bcml2_eif_dest_transform\n")));

    if (in->count <= 0) {
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }

    for (i = 0; i < in->count; i++) {
        if (in->field[i]->id == arg->field[LT_FLD_IDX_IS_TRUNK]) {
            is_trunk_field_val = in->field[i]->data;
        } else if (in->field[i]->id == arg->field[LT_FLD_IDX_TRUNK_ID]) {
            tid_field_val = in->field[i]->data;
        } else if (in->field[i]->id == arg->field[LT_FLD_IDX_SYS_PORT]) {
            sys_port_field_val = in->field[i]->data;
        } else {
            rv = SHR_E_PARAM;
            SHR_ERR_EXIT(rv);
        }
    }

    out->count = 0;
    if (is_trunk_field_val == 0) {
        out->field[out->count]->id   = arg->rfield[PT_FLD_IDX_DEST_TYPE];
        out->field[out->count]->data = 0;
        out->count++;
        out->field[out->count]->id   = arg->rfield[PT_FLD_IDX_SYS_DEST];
        out->field[out->count]->data = sys_port_field_val;
        out->count++;
    } else {
        out->field[out->count]->id   = arg->rfield[PT_FLD_IDX_DEST_TYPE];
        out->field[out->count]->data = 1;
        out->count++;
        out->field[out->count]->id   = arg->rfield[PT_FLD_IDX_SYS_DEST];
        out->field[out->count]->data = tid_field_val;
        out->count++;
   }

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief L2_EIF_SYSTEM_DESTINATION.IS_TRUNK/SYSTEM_PORT/TRUNK_ID forward transform
 *
 * \param [in]  unit  Unit number.
 * \param [in]  in    IPOST_LAG_L2OIF.SYSTEM_DEST_TYPE/SYSTEM_DESTINATION.
 * \param [out] out   L2_EIF_SYSTEM_DESTINATION.IS_TRUNK/SYSTEM_PORT/TRUNK_ID.
 * \param [in]  arg   Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_l2_eif_dest_rev_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg)
{
    size_t i = 0;
    int rv = SHR_E_NONE;
    uint64_t dest_type_field_val = 0;
    uint64_t sys_dest_field_val = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "\t bcml2_eif_dest_rev_transform\n")));
    for (i = 0; i < in->count; i++) {
        if (in->field[i]->id == arg->field[PT_FLD_IDX_DEST_TYPE]) {
            dest_type_field_val = in->field[i]->data;
        } else if (in->field[i]->id == arg->field[PT_FLD_IDX_SYS_DEST]) {
            sys_dest_field_val = in->field[i]->data;
        } else {
            rv = SHR_E_PARAM;
            SHR_ERR_EXIT(rv);
        }
    }

    out->count = 0;
    if (dest_type_field_val == 0) {
        out->field[out->count]->id   = arg->rfield[LT_FLD_IDX_IS_TRUNK];
        out->field[out->count]->data = 0;
        out->count++;
        out->field[out->count]->id   = arg->rfield[LT_FLD_IDX_SYS_PORT];
        out->field[out->count]->data = sys_dest_field_val;
        out->count++;
        out->field[out->count]->id   = arg->rfield[LT_FLD_IDX_TRUNK_ID];
        out->field[out->count]->data = 0;
        out->count++;
    } else {
        out->field[out->count]->id   = arg->rfield[LT_FLD_IDX_IS_TRUNK];
        out->field[out->count]->data = 1;
        out->count++;
        out->field[out->count]->id   = arg->rfield[LT_FLD_IDX_TRUNK_ID];
        out->field[out->count]->data = sys_dest_field_val;
        out->count++;
        out->field[out->count]->id   = arg->rfield[LT_FLD_IDX_SYS_PORT];
        out->field[out->count]->data = 0;
        out->count++;
    }

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT()
}

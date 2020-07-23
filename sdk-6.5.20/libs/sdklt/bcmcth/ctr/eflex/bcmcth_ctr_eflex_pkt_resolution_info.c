/*! \file bcmcth_ctr_eflex_pkt_resolution_info.c
 *
 * Enhanced flex counter packet resolution information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_alloc.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmcth/bcmcth_ctr_eflex.h>
#include <bcmcth/bcmcth_ctr_eflex_drv.h>
#include <bcmcth/bcmcth_ctr_eflex_pkt_resolution_info.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/******************************************************************************
 * Local definitions
 */

/*******************************************************************************
 * Public functions
 */

static int
pkt_resolution_info_entry_field_value_get(int unit,
                                          const bcmltd_fields_t *in,
                                          uint32_t fid,
                                          uint32_t *rval)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < in->count; i++) {
        if (in->field[i] == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (in->field[i]->id == fid) {
            *rval = in->field[i]->data;
            break;
        }
    }
    if (i == in->count) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_pkt_resolution_info_validate(int unit,
                                              bcmlt_opcode_t opcode,
                                              const bcmltd_fields_t *in,
                                              const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    if ((opcode != BCMLT_OPCODE_LOOKUP) &&
        (opcode != BCMLT_OPCODE_TRAVERSE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_pkt_resolution_info_insert(int unit,
                                            const bcmltd_op_arg_t *op_arg,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_generic_arg_t *arg)
{
    /* Read only table insertion is not allowed */
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_pkt_resolution_info_lookup(int unit,
                                            const bcmltd_op_arg_t *op_arg,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_generic_arg_t *arg)
{
    uint32_t sid = 0, fid = 0, pkt_bmp = 0;
    size_t count = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    SHR_NULL_CHECK(out, SHR_E_PARAM);

    sid = arg->sid;

    switch (sid) {
        case CTR_ING_EFLEX_PKT_RESOLUTION_INFOt:
            fid = \
                CTR_ING_EFLEX_PKT_RESOLUTION_INFOt_CTR_ING_EFLEX_PKT_RESOLUTION_INFO_IDf;
            SHR_IF_ERR_VERBOSE_EXIT
                (pkt_resolution_info_entry_field_value_get(unit,
                                                           in,
                                                           fid,
                                                           &pkt_bmp));
            out->field[count]->id = fid;
            out->field[count]->data = pkt_bmp;
            out->field[count]->idx = 0;
            count++;

            out->field[count]->id = \
                CTR_ING_EFLEX_PKT_RESOLUTION_INFOt_PKT_RESOLUTION_BITMAPf;
            out->field[count]->data = pkt_bmp;
            out->field[count]->idx = 0;
            count++;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_pkt_resolution_info_delete(int unit,
                                            const bcmltd_op_arg_t *op_arg,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_generic_arg_t *arg)
{
    /* Read only table delete operation not permitted */
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_pkt_resolution_info_update(int unit,
                                            const bcmltd_op_arg_t *op_arg,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_generic_arg_t *arg)
{
    /* Read only table update operation not permitted */
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_pkt_resolution_info_first(int unit,
                                           const bcmltd_op_arg_t *op_arg,
                                           const bcmltd_fields_t *in,
                                           bcmltd_fields_t *out,
                                           const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_ctr_eflex_pkt_resolution_info_lookup(unit,
                                                     op_arg,
                                                     out,
                                                     out,
                                                     arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_pkt_resolution_info_next(int unit,
                                          const bcmltd_op_arg_t *op_arg,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_generic_arg_t *arg)
{
    /* table_size = 1 doesn't need to walk to next. */
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

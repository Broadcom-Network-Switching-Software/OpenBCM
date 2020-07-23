/*! \file bcmlm_cth_link_state.c
 *
 * LM Custom Table Handler for LM_LINK_STATEt.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmevm/bcmevm_api.h>

#include <bcmlm/bcmlm_drv_internal.h>
#include <bcmlm/bcmlm_cth_link_state.h>
#include "bcmlm_internal.h"

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLM_IMM


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the value ragnge of the given field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] fid Field ID.
 * \param [out] min Field min value.
 * \param [out] max Field max value.
 *
 * retval SHR_E_NONE No errors.
 * retval !SHR_E_NONE Fail to get the value ragne.
 */
static int
lm_fld_value_range_get(int unit, uint32_t sid, uint32_t fid,
                       uint64_t *min, uint64_t *max)
{
    uint32_t num;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_min_get(unit, sid, fid, 1, min, &num));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_max_get(unit, sid, fid, 1, max, &num));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a field.
 *
 * \param [in] id Field ID.
 * \param [in] idx Field index.
 * \param [in] data Field data.
 *
 * return Pointer to newly allocated field on success or NULL on failure.
 */
static bcmltm_field_list_t *
lm_fld_alloc(uint32_t id, uint32_t idx, uint64_t data)
{
    bcmltm_field_list_t *fld;

    fld = (bcmltm_field_list_t *)shr_fmm_alloc();
    if (!fld) {
        return NULL;
    }

    sal_memset(fld, 0, sizeof(*fld));
    fld->id = id;
    fld->idx = idx;
    fld->data = data;

    return fld;
}

/*******************************************************************************
 * Public functions
 */

int
bcmlm_cth_link_state_validate(int unit,
                              bcmlt_opcode_t opcode,
                              const bcmltd_fields_t *in,
                              const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(opcode);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);

    SHR_FUNC_EXIT();
}

int
bcmlm_cth_link_state_insert(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_cth_link_state_delete(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_cth_link_state_update(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_cth_link_state_lookup(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    size_t fcnt, idx;
    shr_port_t port = (shr_port_t)-1;
    int rv, link_state, phy_link;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(arg);

    /* Get the port number */
    for (idx = 0; idx < in->count; idx++) {
        if (in->field[idx]->id == LM_LINK_STATEt_PORT_IDf) {
            port = in->field[idx]->data;
            break;
        }
    }

    /* Validate the port number */
    rv = bcmlm_ctrl_port_state_validate(unit, port);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get the link state of the port */
    SHR_IF_ERR_EXIT
        (bcmlm_ctrl_port_state_lookup(unit, port, &link_state, &phy_link));

    /* Fill the output fields */
    fcnt = 0;

    out->field[fcnt]->id = LM_LINK_STATEt_PORT_IDf;
    out->field[fcnt]->idx = 0;
    out->field[fcnt]->data = port;
    fcnt++;

    out->field[fcnt]->id = LM_LINK_STATEt_LINK_STATEf;
    out->field[fcnt]->idx = 0;
    out->field[fcnt]->data = link_state;
    fcnt++;

    out->field[fcnt]->id = LM_LINK_STATEt_PHY_LINKf;
    out->field[fcnt]->idx = 0;
    out->field[fcnt]->data = phy_link;
    fcnt++;

    out->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_cth_link_state_first(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t port, min, max;
    int rv;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(in);

    SHR_IF_ERR_EXIT
        (lm_fld_value_range_get(unit,
                                LM_LINK_STATEt, LM_LINK_STATEt_PORT_IDf,
                                &min, &max));

    port = min;
    while (port <= max) {
        flist = &fld;
        flds.field = &flist;
        flds.field[0]->id = LM_LINK_STATEt_PORT_IDf;
        flds.field[0]->data = port;
        flds.count = 1;
        rv = bcmlm_cth_link_state_lookup(unit, op_arg, &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_EXIT();
        }
        port++;
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_cth_link_state_next(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t port, min, max;
    uint32_t idx;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (lm_fld_value_range_get(unit,
                                LM_LINK_STATEt, LM_LINK_STATEt_PORT_IDf,
                                &min, &max));

    port = max;
    for (idx = 0; idx < in->count; idx++) {
        if (in->field[idx]->id == LM_LINK_STATEt_PORT_IDf) {
            port = in->field[idx]->data;
            break;
        }
    }
    port++;

    while (port <= max) {
        flist = &fld;
        flds.field = &flist;
        flds.field[0]->id = LM_LINK_STATEt_PORT_IDf;
        flds.field[0]->data = port;
        flds.count = 1;
        rv = bcmlm_cth_link_state_lookup(unit, op_arg, &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            /* Exit the current iteration once we get a valid entry. */
            SHR_EXIT();
        }
        port++;
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_link_state_tbl_notify(int unit, int port, int link)
{
    bcmltm_field_list_t *fld;

    SHR_FUNC_ENTER(unit);

    fld = lm_fld_alloc(LM_LINK_STATEt_PORT_IDf, 0, port);
    SHR_NULL_CHECK(fld, SHR_E_MEMORY);

    fld->next = lm_fld_alloc(LM_LINK_STATEt_LINK_STATEf, 0, link);
    SHR_NULL_CHECK(fld->next, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmevm_table_notify(unit, false,
                             LM_LINK_STATEt, BCMLT_OPCODE_UPDATE, fld));

exit:
    SHR_FUNC_EXIT();
}

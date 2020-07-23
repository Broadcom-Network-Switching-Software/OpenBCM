/*! \file bcmtm_lt_pipe_map_info.c
 *
 * Logical Table Custom Handlers for TM_PIPE_MAP_INFO.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 INCLUDES
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/lth_pmap/bcmtm_lt_pipe_map_info.h>

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CTH

/*******************************************************************************
* Public functions
 */

int
bcmtm_lt_pipe_map_info_validate(int unit,
                                bcmlt_opcode_t opcode,
                                const bcmltd_fields_t *in,
                                const bcmltd_generic_arg_t *arg)
{
    bcmlrd_sid_t sid = arg->sid;

    SHR_FUNC_ENTER(unit);

    switch (opcode) {
        case BCMLT_OPCODE_LOOKUP:
        case BCMLT_OPCODE_TRAVERSE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmtm_lt_field_validate(unit, opcode, sid, in));
            break;
        default:
            /* Insert, update, delete are not supported because of read-only. */
            SHR_ERR_EXIT(SHR_E_ACCESS);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pipe_map_info_insert(int unit,
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

    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate the list of port in a pipe.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe
 * \param [in] max_count maximum entries
 * \param [out] lport_list Logical port array
 * \param [out] count Actual number of entries
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Port calculation error.
 */
static int
bcmtm_lt_pipe_port_list_get(int unit,
                            int pipe, int max_count,
                            int *lport_list,
                            int *count)
{
    bcmpc_pport_t pport;
    int j, pipe_local, lport;
    bcmdrd_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lport_list, SHR_E_PARAM);
    SHR_NULL_CHECK(count, SHR_E_PARAM);
    j = 0;

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_ports_get(unit, &pbmp));

    BCMDRD_PBMP_ITER(pbmp, pport) {
        if (SHR_SUCCESS(bcmtm_pport_lport_get(unit, pport, &lport))) {
            if (SHR_SUCCESS(bcmtm_lport_pipe_get(unit, lport, &pipe_local))) {
                if (pipe != pipe_local) {
                    continue;
                }
                lport_list[j] = lport;
                j++;
                if (j >= max_count) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
        }
    }
    *count = j;
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pipe_map_info_lookup(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg)
{
#define MAX_PORTS_PER_PIPE 32
    int i, pipe, lport_list[MAX_PORTS_PER_PIPE], port_count = 0;
    int num_idx;
    bcmltd_fid_t fid;
    uint32_t valid_pipe_bmp;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(arg);

    SHR_NULL_CHECK(out, SHR_E_PARAM);

    sal_memset(lport_list, 0, sizeof(lport_list));
    pipe = in->field[TM_PIPE_MAP_INFOt_TM_PIPEf]->data;

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, &valid_pipe_bmp));
    if (((1 << pipe) & valid_pipe_bmp) == 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmtm_lt_pipe_port_list_get(unit, pipe, MAX_PORTS_PER_PIPE,
                                     lport_list, &port_count));
    num_idx = bcmlrd_field_idx_count_get(unit, TM_PIPE_MAP_INFOt,
                                            TM_PIPE_MAP_INFOt_PORT_IDf);
    out->count = 0;

    fid = TM_PIPE_MAP_INFOt_TM_PIPEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, out, fid, 0, pipe));

    fid = TM_PIPE_MAP_INFOt_PORT_CNTf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, out, fid, 0, port_count));

    fid = TM_PIPE_MAP_INFOt_PORT_IDf;
    for (i = 0; i < num_idx; i++) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, out, fid, i, lport_list[i]));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pipe_map_info_delete(int unit,
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

    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pipe_map_info_update(int unit,
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

    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pipe_map_info_first(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    uint64_t min = 0, max = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_field_value_range_get(unit, TM_PIPE_MAP_INFOt,
                                         TM_PIPE_MAP_INFOt_TM_PIPEf,
                                         &min, &max));

    /* prepare the in buffer and perform lookup */
    out->field[TM_PIPE_MAP_INFOt_TM_PIPEf]->data = min;
    SHR_IF_ERR_EXIT(
            bcmtm_lt_pipe_map_info_lookup(unit, op_arg, out, out, arg));
exit:
    SHR_FUNC_EXIT();


}

int
bcmtm_lt_pipe_map_info_next(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    uint64_t min, max;
    uint32_t pipe;
    uint32_t valid_pipe_bmp;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_field_value_range_get(unit, TM_PIPE_MAP_INFOt,
                                         TM_PIPE_MAP_INFOt_TM_PIPEf,
                                         &min, &max));

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, &valid_pipe_bmp));
    if (in) {
        pipe = ++(in->field[TM_PIPE_MAP_INFOt_TM_PIPEf]->data);
        for (; pipe <= max; pipe++) {
            if ((1 << pipe) & valid_pipe_bmp) {
                break;
            }
        }
    }

    if (pipe > max) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    in->field[TM_PIPE_MAP_INFOt_TM_PIPEf]->data = pipe;

    bcmtm_lt_pipe_map_info_lookup(unit, op_arg, in, out, arg);
exit:
    SHR_FUNC_EXIT();
}

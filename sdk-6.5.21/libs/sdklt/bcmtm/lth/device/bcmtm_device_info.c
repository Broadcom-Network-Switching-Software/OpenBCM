/*! \file bcmtm_device_info.c
 *
 * TM device information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmtm/bcmtm_device_info.h>
#include <bcmtm/bcmtm_utils_internal.h>
/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMTM_CTH


/*******************************************************************************
 * Public functions
 */
int
bcmtm_device_info_validate(int unit,
                        bcmlt_opcode_t opcode,
                        const bcmltd_fields_t *in,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    if (opcode != BCMLT_OPCODE_LOOKUP){
        SHR_ERR_EXIT(SHR_E_ACCESS);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_device_info_insert(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int
bcmtm_device_info_lookup(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    size_t fid = 0;
    const bcmlrd_field_data_t *field;
    uint64_t fval = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;
    while (fid < TM_DEVICE_INFOt_FIELD_COUNT) {
        if (SHR_E_UNAVAIL == bcmlrd_field_get(unit, arg->sid, fid, &field)) {
            fid++;
            continue;
        }
        switch(fid){
            case TM_DEVICE_INFOt_MAX_PKT_SIZEf:
                fval = bcmtm_device_info[unit].max_pkt_sz;
                break;
            case TM_DEVICE_INFOt_PKT_HDR_SIZEf:
                fval = bcmtm_device_info[unit].pkt_hdr_sz;
                break;
            case TM_DEVICE_INFOt_JUMBO_PKT_SIZEf:
                fval = bcmtm_device_info[unit].jumbo_pkt_sz;
                break;
            case TM_DEVICE_INFOt_DEFAULT_MTUf:
                fval = bcmtm_device_info[unit].default_mtu;
                break;
            case TM_DEVICE_INFOt_CELL_SIZEf:
                fval = bcmtm_device_info[unit].cell_sz;
                break;
            case TM_DEVICE_INFOt_NUM_CELLSf:
                fval = bcmtm_device_info[unit].num_cells;
                break;
            case TM_DEVICE_INFOt_NUM_PORT_PRI_GRPf:
                fval = bcmtm_device_info[unit].num_port_pg;
                break;
            case TM_DEVICE_INFOt_NUM_SERVICE_POOLf:
                fval = bcmtm_device_info[unit].num_sp;
                break;
            case TM_DEVICE_INFOt_NUM_Qf:
                fval = bcmtm_device_info[unit].num_q;
                break;
            case TM_DEVICE_INFOt_NUM_CPU_Qf:
                fval = bcmtm_device_info[unit].num_cpu_q;
                break;
            case TM_DEVICE_INFOt_NUM_PIPEf:
                fval = bcmtm_device_info[unit].num_pipe;
                break;
            case TM_DEVICE_INFOt_NUM_MC_REPL_RESOURCE_FREEf:
                fval = 0;
                SHR_IF_ERR_EXIT
                    (bcmtm_mc_free_resource_count_get(unit, &fval));
                break;
            case TM_DEVICE_INFOt_NUM_NHOP_SPARSE_MODEf:
                fval = bcmtm_device_info[unit].num_nhop_sparse;
                break;
            case TM_DEVICE_INFOt_NUM_NHOP_DENSE_MODEf:
                fval = bcmtm_device_info[unit].num_nhop_dense;
                break;
            case TM_DEVICE_INFOt_MAX_NUM_MC_REPLf:
                fval = bcmtm_device_info[unit].max_num_repl;
                break;
            case TM_DEVICE_INFOt_NUM_BUFFER_POOLf:
                fval = bcmtm_device_info[unit].num_buffer_pool;
                break;
            default:
                break;
        }
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, out, fid, 0, fval));
        fid++;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_device_info_delete(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int
bcmtm_device_info_update(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int
bcmtm_device_info_first(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_device_info_lookup(unit, op_arg, NULL, out, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_device_info_next(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

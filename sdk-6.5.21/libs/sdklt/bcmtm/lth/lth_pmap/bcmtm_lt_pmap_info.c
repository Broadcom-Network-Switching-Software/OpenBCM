/*! \file bcmtm_lt_pmap_info.c
 *
 * Logical Table Custom Handlers for TM_PORT_MAP_INFO.
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
#include <bcmtm/lth_pmap/bcmtm_lt_pmap_info.h>
#include <bcmtm/bcmtm_utils.h>

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CTH

/*******************************************************************************
* Public functions
 */

int
bcmtm_lt_pmap_info_validate(int unit,
                            bcmlt_opcode_t opcode,
                            const bcmltd_fields_t *in,
                            const bcmltd_generic_arg_t *arg)
{
    int lport, pport;
    bcmlrd_sid_t sid = arg->sid;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_lt_field_validate(unit, opcode, sid, in));

    lport = in->field[TM_PORT_MAP_INFOt_PORT_IDf]->data;

    switch (opcode) {
        case BCMLT_OPCODE_LOOKUP:
            /* Check if the logical port exists. */
            if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        case BCMLT_OPCODE_TRAVERSE:
            break;
        default:
            /* Insert, update, delete are not supported because of read-only. */
            SHR_ERR_EXIT(SHR_E_ACCESS);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pmap_info_insert(int unit,
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
bcmtm_lt_pmap_info_lookup(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    int lport, pport, mmu_port, mmu_chip_port, idb_port, pipe, uc_q_base,
        num_uc_q, mc_q_base, num_mc_q;
    size_t num_fields = 0, fid = 0;
    const bcmlrd_field_data_t *field;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out, SHR_E_PARAM);

    lport = in->field[TM_PORT_MAP_INFOt_PORT_IDf]->data;
    num_fields = TM_PORT_MAP_INFOt_FIELD_COUNT;

    out->count = 0;
    /* populating out buffer */
    while (fid <= num_fields) {
        if (SHR_E_UNAVAIL == bcmlrd_field_get(unit, arg->sid, fid, &field)) {
            fid++;
            continue;
        }
        switch(fid) {
            case TM_PORT_MAP_INFOt_PORT_IDf:
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, lport));
                break;
            case TM_PORT_MAP_INFOt_PC_PHYS_PORT_IDf:
                SHR_IF_ERR_EXIT
                    (bcmtm_lport_pport_get(unit, lport, &pport));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, pport));
                break;
            case TM_PORT_MAP_INFOt_TM_PIPEf:
                SHR_IF_ERR_EXIT
                    (bcmtm_lport_pipe_get(unit, lport, &pipe));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, pipe));
                break;
            case TM_PORT_MAP_INFOt_TM_LOCAL_PORTf:
                SHR_IF_ERR_EXIT
                    (bcmtm_lport_mmu_local_port_get(unit, lport, &idb_port));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, idb_port));
                break;
            case TM_PORT_MAP_INFOt_TM_SPARSE_GLOBAL_PORTf:
                SHR_IF_ERR_EXIT
                    (bcmtm_lport_mport_get(unit, lport, &mmu_port));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, mmu_port));
                break;
            case TM_PORT_MAP_INFOt_TM_COMPACT_GLOBAL_PORTf:
                SHR_IF_ERR_EXIT
                    (bcmtm_lport_mchip_port_get(unit, lport, &mmu_chip_port));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, mmu_chip_port));
                break;
            case TM_PORT_MAP_INFOt_BASE_UC_Qf:
                SHR_IF_ERR_EXIT
                    (bcmtm_lport_ucq_base_get(unit, lport, &uc_q_base));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, uc_q_base));
                break;
            case TM_PORT_MAP_INFOt_NUM_UC_Qf:
                SHR_IF_ERR_EXIT
                    (bcmtm_lport_num_ucq_get(unit, lport, &num_uc_q));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, num_uc_q));
                break;
            case TM_PORT_MAP_INFOt_BASE_MC_Qf:
                SHR_IF_ERR_EXIT
                    (bcmtm_lport_mcq_base_get(unit, lport, &mc_q_base));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, mc_q_base));
                break;
            case TM_PORT_MAP_INFOt_NUM_MC_Qf:
                SHR_IF_ERR_EXIT
                    (bcmtm_lport_num_mcq_get(unit, lport, &num_mc_q));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out, fid, 0, num_mc_q));
                break;
            default:
                break;
        }
        fid++;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pmap_info_delete(int unit,
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
bcmtm_lt_pmap_info_update(int unit,
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
bcmtm_lt_pmap_info_first(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    uint64_t min = 0, max = 0, lport;
    int pport;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_field_value_range_get(unit, TM_PORT_MAP_INFOt,
                                     TM_PORT_MAP_INFOt_PORT_IDf,
                                     &min, &max));

    for (lport = min; lport <= max; lport++) {
        if (SHR_SUCCESS (bcmtm_lport_pport_get (unit, lport, &pport))) {
            out->field[TM_PORT_MAP_INFOt_PORT_IDf]->data = lport;
            break;
        }
        if (lport == max) {
            return SHR_E_NOT_FOUND;
        }
    }

    /* Prepare the in buffer and perform lookup. */
    SHR_IF_ERR_EXIT(
        bcmtm_lt_pmap_info_lookup(unit, op_arg, out, out, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pmap_info_next(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t min, max;
    uint32_t lport;
    int pport;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_field_value_range_get(unit, TM_PORT_MAP_INFOt,
                                     TM_PORT_MAP_INFOt_PORT_IDf,
                                     &min, &max));

    if (in) {
        lport = ++(in->field[TM_PORT_MAP_INFOt_PORT_IDf]->data);
    } else {
        lport = min;
    }

    /* Stop traverse when exceeding the max logical port number. */
    while (lport <= max) {
        /* Exit the current iteration once we get a valid entry. */
        if (SHR_SUCCESS(bcmtm_lport_pport_get(unit, lport, &pport))) {
            flist = &fld;
            flds.field = &flist;
            flds.field[0]->id = TM_PORT_MAP_INFOt_PORT_IDf;
            flds.field[0]->data = lport;
            flds.count = 1;
            bcmtm_lt_pmap_info_lookup(unit, op_arg, &flds, out, arg);
            SHR_EXIT();
        } else {
            lport++;
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

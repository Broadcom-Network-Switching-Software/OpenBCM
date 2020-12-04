/*! \file bcmtm_scheduler_profile_qinfo.c
 *
 * APIs for showing scheduler profile queue information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmtm/sched_shaper/bcmtm_scheduler_internal.h>
#include <bcmtm/sched_shaper/bcmtm_scheduler_profile_q_info.h>

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CTH

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief TM scheduler profile queue information.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Profile ID.
 * \param [in] node_id Scheduler node ID.
 * \param [in] profile Scheduler profile.
 * \param [out]out Output parameters.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code in case of error.
 */
static int
bcmtm_scheduler_profile_q_info_populate(int unit,
                                        uint8_t profile_id,
                                        uint8_t node_id,
                                        bcmtm_scheduler_profile_t *profile,
                                        bcmltd_fields_t *out)
{
    uint32_t idx;
    int num_ucq, num_mcq;
    uint8_t ucq_offset = 0, mcq_offset = 0;
    bcmlrd_fid_t fid;
    uint64_t fval = 0;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; (node_id && idx < node_id); idx++) {
        ucq_offset += profile[idx].num_ucq;
        mcq_offset += profile[idx].num_mcq;
    }
    out->count = 0;
    fid = TM_SCHEDULER_PROFILE_Q_INFOt_TM_SCHEDULER_PROFILE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, out, fid, 0, profile_id));

    fid = TM_SCHEDULER_PROFILE_Q_INFOt_TM_SCHEDULER_NODE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, out, fid, 0, node_id));

    num_ucq = profile[node_id].num_ucq;
    fid = TM_SCHEDULER_PROFILE_Q_INFOt_NUM_TM_UC_Qf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, out, fid, 0, num_ucq));

    num_mcq = profile[node_id].num_mcq;
    fid = TM_SCHEDULER_PROFILE_Q_INFOt_NUM_TM_MC_Qf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, out, fid, 0, num_mcq));

    fid = TM_SCHEDULER_PROFILE_Q_INFOt_TM_MC_Q_IDf;
    if (num_mcq) {
        fval = mcq_offset;
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, out, fid, 0, fval));

    fid = TM_SCHEDULER_PROFILE_Q_INFOt_TM_UC_Q_IDf;
    for (idx = 0; idx < 2; idx++) {
        fval = 0;
        if ((num_ucq--) > 0) {
            fval = ucq_offset + idx;
        }
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, out, fid, idx,fval));
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_scheduler_profile_q_info_validate (int unit,
                                         bcmlt_opcode_t opcode,
                                         const bcmltd_fields_t *in,
                                         const bcmltd_generic_arg_t *arg)
{
    bcmltd_sid_t sid = TM_SCHEDULER_PROFILE_Q_INFOt;
    SHR_FUNC_ENTER(unit);

    switch(opcode) {
        case BCMLT_OPCODE_LOOKUP:
            /* Check for profile max min range. */
            SHR_IF_ERR_EXIT
                (bcmtm_lt_field_validate(unit, opcode, sid, in));
            break;
        case BCMLT_OPCODE_TRAVERSE:
            break;
        default:
            /* Insert, update and delete not supported for read-only table. */
            SHR_ERR_EXIT(SHR_E_ACCESS);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_profile_q_info_insert(int unit,
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
bcmtm_scheduler_profile_q_info_lookup(int unit,
                                      const bcmltd_op_arg_t *op_arg,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_generic_arg_t *arg)
{
    uint8_t profile_id, node_id;
    bcmtm_scheduler_profile_t profile[MAX_TM_SCHEDULER_NODE];
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    /* Get the profile ID and Node ID. */
    fid = TM_SCHEDULER_PROFILE_Q_INFOt_TM_SCHEDULER_PROFILE_IDf;
    profile_id = in->field[fid]->data;

    fid = TM_SCHEDULER_PROFILE_Q_INFOt_TM_SCHEDULER_NODE_IDf;
    node_id = in->field[fid]->data;

    sal_memset(profile, 0,
            sizeof(bcmtm_scheduler_profile_t) * MAX_TM_SCHEDULER_NODE);

    SHR_IF_ERR_EXIT(bcmtm_scheduler_profile_get(unit, profile_id, profile));

    if (profile[node_id].opcode != ENTRY_INVALID) {
        bcmtm_scheduler_profile_q_info_populate(unit, profile_id, node_id,
                                            profile, out);
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_profile_q_info_delete(int unit,
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
bcmtm_scheduler_profile_q_info_update(int unit,
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
bcmtm_scheduler_profile_q_info_first(int unit,
                                     const bcmltd_op_arg_t *op_arg,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(in);

    SHR_NULL_CHECK(out, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmtm_scheduler_profile_q_info_next(unit, op_arg, NULL, out, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_profile_q_info_next(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out,
                                    const bcmltd_generic_arg_t *arg)
{
    uint64_t profile_max, node_max;
    uint64_t profile_id, node_id;
    bcmlrd_fid_t fid;
    bcmtm_scheduler_profile_t profile[MAX_TM_SCHEDULER_NODE];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    fid = TM_SCHEDULER_PROFILE_Q_INFOt_TM_SCHEDULER_PROFILE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_value_range_get(unit, arg->sid, fid,
                                     &profile_id, &profile_max));
    fid = TM_SCHEDULER_PROFILE_Q_INFOt_TM_SCHEDULER_NODE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_value_range_get(unit, arg->sid, fid,
                                     &node_id, &node_max));
    if (in){
        fid = TM_SCHEDULER_PROFILE_Q_INFOt_TM_SCHEDULER_PROFILE_IDf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_get(unit, (bcmltd_fields_t *)in, fid, 0, &profile_id));
        fid = TM_SCHEDULER_PROFILE_Q_INFOt_TM_SCHEDULER_NODE_IDf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_get(unit, (bcmltd_fields_t *)in, fid, 0, &node_id));
        if ((profile_id == profile_max) && (node_id == node_max)) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        else if ( node_id == node_max) {
            profile_id++;
            node_id = 0;
        } else {
            node_id++;
        }
    }

    for ( ; node_id < node_max; node_id++) {
        sal_memset(profile, 0,
                sizeof(bcmtm_scheduler_profile_t) * MAX_TM_SCHEDULER_NODE);

        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_profile_get(unit, profile_id, profile));

        if (profile[node_id].opcode != ENTRY_INVALID ) {
            break;
        }
        if (node_id == (node_max - 1)) {
            profile_id++;
            node_id = 0;
        }
        if (profile_id > profile_max) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_profile_q_info_populate(unit, profile_id, node_id,
                                            profile, out));
exit:
    SHR_FUNC_EXIT();
}

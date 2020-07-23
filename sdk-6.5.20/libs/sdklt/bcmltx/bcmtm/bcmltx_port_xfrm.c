/*! \file bcmltx_port_xfrm.c
 *
 * APIs for Port transforms. This involves transform function for converting
 *  1. Logical port ID to physical port number
 *  2. Logical port ID to mmu local port number
 *  3. Logical port ID to mmu port number
 *  4. Logical port ID to chip port number
 *  5. Logical port ID to chip queue number.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmltx/bcmtm/bcmltx_obm_port_idx.h>
#include <bcmltx/bcmtm/bcmltx_pc_pm_idx.h>
#include <bcmltx/bcmtm/bcmltx_chip_port_idx.h>
#include <bcmltx/bcmtm/bcmltx_chip_q_num_idx.h>
#include <bcmltx/bcmtm/bcmltx_chip_port_sp_idx.h>
#include <bcmltx/bcmtm/bcmltx_port_tbl_sel_pg_idx.h>
#include <bcmltx/bcmtm/bcmltx_port_pg_pipe_idx.h>
#include <bcmltx/bcmtm/bcmltx_mmu_ing_local_port_pipe_idx.h>
#include <bcmltx/bcmtm/bcmltx_mmu_local_port_pipe_idx.h>
#include <bcmltx/bcmtm/bcmltx_mmu_port_idx.h>
#include <bcmltx/bcmtm/bcmltx_mmu_local_port_pipe_q_idx.h>

#define BSL_LOG_MODULE  BSL_LS_BCMLTX_TM
/*******************************************************************************
 * Private functions
 */
static int
bcmltx_field_list_set(int unit,
                      bcmltd_fields_t *flist,
                      bcmltd_fid_t fid,
                      uint32_t idx,
                      uint64_t val)
{
    size_t count =  flist->count;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(flist->field[count], SHR_E_INTERNAL);
    flist->field[count]->id = fid;
    flist->field[count]->data = val;
    flist->field[count]->idx = idx;
    flist->count++;
exit:
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * Public functions
 */
int
bcmltx_obm_port_idx_transform(int unit,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg)
{
    bcmtm_lport_t lport;
    int pport, pipe, tbl_id, idx;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    /* logical port ID. */
    lport = in->field[0]->data;
    if (!bcmtm_obm_port_validation(unit, lport)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        SHR_EXIT();
    }

    if (SHR_FAILURE(bcmtm_lport_pipe_get(unit, lport, &pipe))) {
        SHR_EXIT();
    }

    /* local physical port number. */
    pport = (pport - 1) % arg->value[1];

    /* Index */
    idx = pport % arg->value[0];

    if (bcmtm_lport_is_mgmt(unit, lport)) {
        tbl_id = 4;
        SHR_IF_ERR_EXIT
            (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, idx));
        SHR_IF_ERR_EXIT
            (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, 0));
    } else {
        SHR_IF_ERR_EXIT
            (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, idx));

        /* Instance */
        SHR_IF_ERR_EXIT
            (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, pipe));

        /* Table select */
        tbl_id = pport / arg->value[0];
    }
    SHR_IF_ERR_EXIT
        (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, tbl_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_obm_port_idx_rev_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    int  lport, pipe, tbl_id;
    bcmtm_pport_t pport;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    pipe = in->field[1]->data;
    tbl_id = in->field[2]->data;
    if (tbl_id == 4) {
        pport = pipe * (arg->value[1] + 1) + arg->value[1];
    } else {
        pport = (in->field[0]->data + (tbl_id * arg->value[0])) +
                (pipe * arg->value[1]) + 1;
    }

    SHR_IF_ERR_EXIT
        (bcmtm_pport_lport_get(unit, pport, &lport));

    SHR_IF_ERR_EXIT
        (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, lport));
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_pc_pm_idx_transform(int unit,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_transform_arg_t *arg)
{
    int pipe, tbl_id;
    uint32_t pm_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    /* get pipe number from Port macro id. */
    pm_id = in->field[0]->data - 1 ;
    pipe = pm_id / (arg->value[0]);

    /* Management port macro. */
    if (pm_id == arg->value[1]) {
        /* Maximum number of transform table for front panel ports */
        tbl_id = arg->value[0];
        SHR_IF_ERR_EXIT
            (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, 0));
        SHR_IF_ERR_EXIT
            (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, 0));
    } else {
        tbl_id = pm_id % (arg->value[0]);
        SHR_IF_ERR_EXIT
            (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, 0));
        SHR_IF_ERR_EXIT
            (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, pipe));
    }

    SHR_IF_ERR_EXIT
        (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, tbl_id));
exit:
    SHR_FUNC_EXIT();

}

int
bcmltx_pc_pm_idx_rev_transform(int unit,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_transform_arg_t *arg)
{
    int pipe, pm_id;
    uint32_t tbl_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    /* get pipe number from Port macro id. */
    pipe = in->field[1]->data;
    tbl_id = in->field[2]->data;
    pm_id = pipe * arg->value[0] + tbl_id + 1;

    /* Management port macro. */
    if (tbl_id == arg->value[0]) {
        pm_id = arg->value[1] + 1;
    }
    SHR_IF_ERR_EXIT
        (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, pm_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_chip_port_idx_transform(int unit,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_transform_arg_t *arg)
{
    bcmtm_lport_t lport;
    int chip_port, buffer_pool = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    /* logical port ID. */
    lport = in->field[0]->data;

    /* Populate only if buffer pool is passed.*/
    if (in->count == 2) {
        buffer_pool = in->field[1]->data;
    }
    if (SHR_FAILURE(bcmtm_lport_mchip_port_get(unit, lport, &chip_port))) {
        SHR_EXIT();
    }

    /* __INDEX */
    SHR_IF_ERR_EXIT
        (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, chip_port));

    /* __INSTANCE */
    SHR_IF_ERR_EXIT
        (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, buffer_pool));
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_chip_q_num_idx_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    bcmtm_lport_t lport;
    int chip_q_num, q_id, buffer_pool = 0;
    int q_base;

    SHR_FUNC_ENTER(unit);

    lport = in->field[0]->data;
    q_id = in->field[1]->data;

    /* Populate only if buffer pool is passed.*/
    if (in->count == 3) {
        buffer_pool = in->field[2]->data;
    }
    out->count = 0;

    if (arg->value[0]) {
        /* Multicast queue */
        if (SHR_FAILURE(bcmtm_lport_mcq_base_get(unit, lport, &q_base))) {
            SHR_EXIT();
        }
    } else {
        /* Unicast queue */
        if (SHR_FAILURE(bcmtm_lport_ucq_base_get(unit, lport, &q_base))) {
            SHR_EXIT();
        }
    }
    chip_q_num = q_base + q_id;

    /* __INDEX */
    SHR_IF_ERR_EXIT
        (bcmltx_field_list_set(unit, out, arg->rfield[out->count],
                              0, chip_q_num));
    /* __INSTANCE */
    SHR_IF_ERR_EXIT
        (bcmltx_field_list_set(unit, out, arg->rfield[out->count],
                              0, buffer_pool));
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_chip_port_sp_idx_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    bcmtm_lport_t lport;
    int chip_port, sp_id, idx, buffer_pool = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    lport = in->field[0]->data;
    sp_id = in->field[1]->data;
    /* Populate only if buffer pool is passed.*/
    if (in->count == 3) {
        buffer_pool = in->field[2]->data;
    }
    out->count = 0;


    if (SHR_FAILURE(bcmtm_lport_mchip_port_get(unit, lport, &chip_port))) {
        SHR_EXIT();
    }
    idx = (chip_port * arg->value[0]) + sp_id;

    /* chip port service pool index. */
    SHR_IF_ERR_EXIT
        (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, idx));

    /* __INSTANCE */
    SHR_IF_ERR_EXIT
        (bcmltx_field_list_set(unit, out, arg->rfield[out->count], 0, buffer_pool));

exit:
    SHR_FUNC_EXIT();

}

int
bcmltx_mmu_local_port_pipe_idx_transform(int unit,
                                         const bcmltd_fields_t *in,
                                         bcmltd_fields_t *out,
                                         const bcmltd_transform_arg_t *arg)
{
    uint32_t lport, pipe, mmu_local_port;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    if (in->count == 0) {
        SHR_EXIT();
    }

    /* PORT */
    lport = in->field[0]->data;

    if (SHR_FAILURE(bcmtm_lport_pipe_get(unit, lport, (int*)&pipe))) {
        SHR_EXIT();
    }

    if (SHR_FAILURE
        (bcmtm_lport_mmu_local_port_get(unit, lport, (int*)&mmu_local_port))) {
        SHR_EXIT();
    }

    /* __INDEX */
    out->count = 0;
    out->field[out->count]->data = mmu_local_port;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;

    /* __INSTANCE */
    out->field[out->count]->data = pipe;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_mmu_local_port_pipe_idx_rev_transform(int unit,
                                             const bcmltd_fields_t *in,
                                             bcmltd_fields_t *out,
                                             const bcmltd_transform_arg_t *arg)
{
    uint32_t pipe, mmu_local_port;
    int lport, mport;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* Getting the key field */

    mmu_local_port = in->field[0]->data;
    pipe = in->field[1]->data;

    mport = (arg->value[0] * pipe) + mmu_local_port;
    SHR_IF_ERR_EXIT
        (bcmtm_mport_lport_get(unit, mport, &lport));

    /* PORT_ID */
    out->count = 0;
    out->field[out->count]->data = lport;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_port_tbl_sel_pg_idx_transform(int unit,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg)
{
    bcmtm_lport_t lport;
    int index, mmu_chip_port;
    int pgid = 0, num_pg_port = 1, tbl_sel = -1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    if (in->count > 1) {
        tbl_sel = in->field[1]->data;
    }

    if (in->count > 2) {
        pgid = in->field[2]->data;
        num_pg_port = arg->value[0];
    }

    lport = in->field[0]->data;

    if (SHR_FAILURE(bcmtm_lport_mchip_port_get(unit, lport,
                                                 (int*)&mmu_chip_port))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    index = (mmu_chip_port * num_pg_port) + pgid;

    out->count = 0;
    /* ouput __INDEX */
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->data = index;
    out->count++;

    /* ouput __TABLE_SEL if applied */
    if (tbl_sel >= 0) {
        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->data = tbl_sel;
        out->count++;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_port_tbl_sel_pg_idx_rev_transform(int unit,
                                         const bcmltd_fields_t *in,
                                         bcmltd_fields_t *out,
                                         const bcmltd_transform_arg_t *arg)
{
    int lport;
    int pgid, index;
    int tbl_sel = 0, num_pg_port = -1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    if (in->count > 3) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (in->count > 1) {
        num_pg_port = arg->value[0];
    }

    index = in->field[0]->data;
    tbl_sel = in->field[1]->data;

    pgid = index % num_pg_port;
    SHR_IF_ERR_EXIT
        (bcmtm_mchip_port_lport_get(unit, index, &lport));

    out->count = 0;
    /* ouput port ID */
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->data = lport;
    out->count++;
    /* ouput table selector */
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->data = tbl_sel;
    out->count++;
    /* ouput priority group if applied */
    if (num_pg_port != -1) {
        out->field[out->count]->id   = arg->rfield[out->count];
        out->field[out->count]->data = pgid;
        out->count++;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_mmu_ing_local_port_pipe_idx_transform(int unit,
                                             const bcmltd_fields_t *in,
                                             bcmltd_fields_t *out,
                                             const bcmltd_transform_arg_t *arg)
{
    uint32_t lport, pipe, mmu_local_port;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    if (in->count == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* PORT */
    lport = in->field[0]->data;

    if (SHR_FAILURE(bcmtm_lport_pipe_get(unit, lport, (int*)&pipe))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    if (SHR_FAILURE
        (bcmtm_lport_mmu_local_port_get(unit, lport, (int*)&mmu_local_port))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    /* __INDEX */
    out->count = 0;
    out->field[out->count]->data = mmu_local_port;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;

    /* __INSTANCE */
    out->field[out->count]->data = pipe;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_mmu_ing_local_port_pipe_idx_rev_transform(int unit,
                                                 const bcmltd_fields_t *in,
                                                 bcmltd_fields_t *out,
                                                 const bcmltd_transform_arg_t *arg)
{
    uint32_t pipe, mmu_local_port;
    int lport, mport;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* Getting the key field */

    mmu_local_port = in->field[0]->data;
    pipe = in->field[1]->data;

    mport = (arg->value[0] * pipe) + mmu_local_port;
    SHR_IF_ERR_EXIT
        (bcmtm_mport_lport_get(unit, mport, &lport));

    /* PORT_ID */
    out->count = 0;
    out->field[out->count]->data = lport;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_mmu_local_port_pipe_q_idx_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg)
{
    bcmtm_lport_t lport;
    int pipe, mmu_local_port, num_uc_q, num_mc_q, uc_q_id, mc_q_id, local_q_id, q_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 4) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    lport = in->field[0]->data;
    q_type = in->field[1]->data;
    uc_q_id = in->field[2]->data;
    mc_q_id = in->field[3]->data;
    out->count = 0;

    if (SHR_FAILURE(bcmtm_lport_pipe_get(unit, lport, (int*)&pipe))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    if (SHR_FAILURE
        (bcmtm_lport_mmu_local_port_get(unit, lport, (int*)&mmu_local_port))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    if (q_type) {
        /* Multicast queue */
        if (SHR_FAILURE(bcmtm_lport_num_ucq_get(unit, lport, &num_uc_q))) {
            SHR_EXIT();
        }
        if (SHR_FAILURE(bcmtm_lport_num_mcq_get(unit, lport, &num_mc_q))) {
            SHR_EXIT();
        }
        local_q_id = num_uc_q + mc_q_id;
        if (local_q_id >= num_uc_q + num_mc_q) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        if (SHR_FAILURE(bcmtm_lport_num_ucq_get(unit, lport, &num_uc_q))) {
            SHR_EXIT();
        }
        /* Unicast queue */
        local_q_id = uc_q_id;
        if (local_q_id >= num_uc_q) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    out->count = 0;
    out->field[out->count]->data = mmu_local_port;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;

    out->field[out->count]->data = pipe;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;

    out->field[out->count]->data = local_q_id;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_mmu_local_port_pipe_q_idx_rev_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg)
{
    int lport, mport;
    int pipe, mmu_local_port, num_uc_q, uc_q_id, mc_q_id, local_q_id, q_type;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    mmu_local_port = in->field[0]->data;
    pipe = in->field[1]->data;
    local_q_id = in->field[2]->data;

    mport = (arg->value[0] * pipe) + mmu_local_port;
    SHR_IF_ERR_EXIT
        (bcmtm_mport_lport_get(unit, mport, &lport));

    SHR_IF_ERR_EXIT
        (bcmtm_lport_num_ucq_get(unit, lport, &num_uc_q));
    if (local_q_id >= num_uc_q) {
        q_type = 1;
        mc_q_id = local_q_id - num_uc_q;
        uc_q_id = 0;
    } else {
        q_type = 0;
        uc_q_id = local_q_id;
        mc_q_id = 0;
    }

    out->count = 0;
    out->field[out->count]->data = lport;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;

    out->field[out->count]->data = q_type;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;

    out->field[out->count]->data = uc_q_id;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;

    out->field[out->count]->data = mc_q_id;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_mmu_port_idx_transform(int unit,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg)
{
    uint32_t lport, mmu_port;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* PORT */
    lport = in->field[0]->data;

    if (SHR_FAILURE(bcmtm_lport_mport_get(unit, lport, (int*)&mmu_port))) {
        SHR_EXIT();
    }

    /* __PORT */
    out->count = 0;
    out->field[out->count]->data = mmu_port;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_mmu_port_idx_rev_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    int lport, mmu_port;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* PORT */
    mmu_port = in->field[0]->data;

    SHR_IF_ERR_EXIT
        (bcmtm_mport_lport_get(unit, mmu_port, &lport));

    /* __PORT */
    out->count = 0;
    out->field[out->count]->data = lport;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->count++;
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_port_pg_pipe_idx_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    bcmtm_lport_t lport;
    int mmu_local_port, num_pg_port, pgid, pipe, index;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    if (in->count != 2) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (arg->values != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    lport = in->field[0]->data;
    pgid = in->field[1]->data;
    num_pg_port = arg->value[0];

    if (SHR_FAILURE(bcmtm_lport_pipe_get(unit, lport, (int*)&pipe))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    if (SHR_FAILURE
        (bcmtm_lport_mmu_local_port_get(unit, lport, (int*)&mmu_local_port))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    index = (num_pg_port * mmu_local_port) + pgid;

    out->count = 0;
    /* ouput __INDEX */
    out->field[out->count]->id   = arg->rfield[0];
    out->field[out->count]->data = index;
    out->count++;
    /* ouput __INSTANCE */
    out->field[out->count]->id   = arg->rfield[1];
    out->field[out->count]->data = pipe;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_port_pg_pipe_idx_rev_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg)
{
    int lport;
    uint32_t mmu_local_port, num_pg_port, pgid, pipe, index;
    int mport;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    if (in->count != 2) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (arg->values != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    index = in->field[0]->data;
    pipe = in->field[1]->data;
    num_pg_port = arg->value[0];

    mmu_local_port = index / num_pg_port;
    pgid = index % num_pg_port;

    mport = (arg->value[1] * pipe) + mmu_local_port;
    SHR_IF_ERR_EXIT
        (bcmtm_mport_lport_get(unit, mport, &lport));

    out->count = 0;
    /* ouput PORT_ID */
    out->field[out->count]->id   = arg->rfield[0];
    out->field[out->count]->data = lport;
    out->count++;
    /* ouput TM_PRI_GRP_ID */
    out->field[out->count]->id   = arg->rfield[1];
    out->field[out->count]->data = pgid;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

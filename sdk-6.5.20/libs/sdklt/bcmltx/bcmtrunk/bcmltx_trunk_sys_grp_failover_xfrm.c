/*! \file bcmltx_trunk_sys_grp_failover_xfrm.c
 *
 * This file contains transform functions for
 * TRUNK_SYSTEM_FAILOVER LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmtrunk/bcmltx_trunk_sys_grp_failover_entry_add.h>
#include <bcmltx/bcmtrunk/bcmltx_trunk_sys_grp_failover_tbl_sel.h>
#include <bcmltx/bcmltx_util.h>

/* BSL Module TBD */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
/*
 * \brief TRUNK_SYSTEM_FAILOVER table select forward transform.
 *
 * \param [in]  unit   Unit number.
 * \param [in]  in     input field list.
 * \param [out] out    output field list.
 * \param [in]  arg    handler arguments.
 *
 * Transform to select from the
 * logical table index TRUNK_SYSTEM_FAILOVER.TRUNK_SYSTEM_ID
 * to list of physical tables per
 * system trunk ID.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_trunk_sys_grp_failover_tbl_sel_transform(
                              int unit,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg)
{
    bcmltd_fid_t ltfid;
    bcmltx_field_list_t *fptr = NULL;
    uint32_t system_trunk_id;
    uint8_t table_sel= 0; /* Offset of an element in out field list. */

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    /* TRUNK_SYSTEM_ID -- to -- TABLE_SEL. */
    ltfid = TRUNK_SYSTEM_FAILOVERt_TRUNK_SYSTEM_IDf;
    fptr = bcmltx_lta_find_field(in, ltfid, 0);
    if (fptr == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    system_trunk_id =  (uint32_t) fptr->data;

    out->field[table_sel]->data = system_trunk_id;
    out->field[table_sel]->id = arg->rfield[table_sel];
    out->field[table_sel]->idx = 0;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TRUNK_SYSTEM_FAILOVER table select reverse transform.
 *
 * \param [in]  unit   Unit number.
 * \param [in]  in     input field list.
 * \param [out] out    output field list.
 * \param [in]  arg    handler arguments.
 *
 * Reverse transform to select logical table index
 * TRUNK_SYSTEM_FAILOVER.TRUNK_SYSTEM_ID from physical table per
 * system trunk ID.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_trunk_sys_grp_failover_tbl_sel_rev_transform(
                             int unit,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_transform_arg_t *arg)
{
    uint32_t system_trunk_id;
    uint8_t table_sel= 0; /* index of the element in out field list*/

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    /* SYSTEM_TRUNK_ID */
    system_trunk_id = in->field[table_sel]->data;
    out->field[out->count]->data = system_trunk_id;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->idx = 0;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TRUNK_SYSTEM_FAILOVER entry forward transform.
 *
 * \param [in]  unit   Unit number.
 * \param [in]  in     input field list.
 * \param [out] out    output field list.
 * \param [in]  arg    handler arguments.
 *
 * Forward transform to map the TRUNK_SYSTEM_FAILOVER.FAILOVER_PORT_ID
 * array to physical table field list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_trunk_sys_grp_failover_entry_add_transform(
                             int unit,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_transform_arg_t *arg)
{
    uint8_t failover_cnt = 0;
    uint8_t array_idx = 0;
    uint8_t index_sel = 0; /* Offset of an element in out field list. */
    uint8_t mbase_o = 2; /* Offset of the member port in out field array. */
    uint8_t mbase_i = 1; /* Offset of the member port in in field array. */
    bcmltd_fid_t ltfid;
    bcmltx_field_list_t *fptr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    /* __INDEX. */
    /* System_trunk_id is programmed one entry
     * per register , there are 64 regiters
     * each with 0 entry , Hence __INDEX = 0.
     */
    out->field[index_sel]->data = 0;
    out->field[index_sel]->id = arg->rfield[index_sel];
    out->field[index_sel]->idx = 0;
    out->count++;

    /* Get the failover count. */
    ltfid = TRUNK_SYSTEM_FAILOVERt_FAILOVER_CNTf;
    fptr = bcmltx_lta_find_field(in, ltfid, 0);
    if (fptr == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    failover_cnt = (uint8_t) fptr->data;
    out->field[out->count]->idx = 0;
    out->field[out->count]->data = (failover_cnt - 1);
    out->field[out->count]->id = arg->rfield[1]; /* DST lag size field */
    out->count++;

    /* Failover ports.
     * PORT0, PORT1, PORT2, PORT3, PORT4, PORT6, PORT7
     */
    for(array_idx = 1; array_idx < in->count; array_idx++) {
        out->field[out->count]->idx = 0;
        out->field[out->count]->id = arg->rfield[mbase_o + (array_idx - 1)];
        out->field[out->count]->data = in->field[mbase_i + (array_idx - 1)]->data;
        out->count++;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TRUNK_SYSTEM_FAILOVER entry reverse transform.
 *
 * \param [in]  unit   Unit number.
 * \param [in]  in     input field list.
 * \param [out] out    output field list.
 * \param [in]  arg    handler arguments.
 *
 * Reverse transform to map the physical table field list to
 * TRUNK_SYSTEM_FAILOVER.FAILOVER_PORT_ID array.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_trunk_sys_grp_failover_entry_add_rev_transform(
                             int unit,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_transform_arg_t *arg)
{
    uint8_t failover_cnt = 0;
    uint8_t array_idx = 0;
    uint8_t idx = 0;
    uint8_t mbase_o = 2; /* Offset of the member port in out field array. */
    uint8_t mbase_i = 1; /* Offset of the member port in in field array. */
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    /* FAILVOER_CNT */
    failover_cnt = in->field[1]->data;
    out->field[out->count]->idx = 0;
    out->field[out->count]->data = (failover_cnt + 1);
    out->field[out->count]->id = arg->rfield[0];
    out->count++;

    /* FAILOVER_PORT_ID */
    for(idx = mbase_o, array_idx = 0;
        idx < in->count;
        idx++, array_idx++) {
        out->field[out->count]->idx = array_idx;
        out->field[out->count]->id = arg->rfield[mbase_i];
        out->field[out->count]->data = in->field[mbase_o + array_idx]->data;
        out->count++;
    }
exit:
    SHR_FUNC_EXIT()
}

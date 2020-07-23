/*! \file bcmtm_obm_port_usage.c
 *
 * TM OBM port usage counters.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/obm/bcmtm_obm_internal.h>
#include <bcmtm/obm/bcmtm_obm_port_usage.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CTH
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief validates the logical table operation to be performed.
 *
 * verifies if the operation requested is valid for the input list or not.
 *
 * \param [in] unit         Unit number.
 * \param [in] opcode       Opcode for the logical table (insert/delete/update/lookup).
 * \param [in] bmap_ret     tracks information about the operation
 *
 * \retval SHR_E_EXISTS     entry exists.
 * \retval SHR_E_NOT_FOUND  entry not found.
 */
static int
bcmtm_lt_opcode_validate(int unit,
                         bcmlt_opcode_t opcode,
                         int bmap_ret)
{
    SHR_FUNC_ENTER(unit);

    if ((opcode == BCMLT_OPCODE_INSERT) && (bmap_ret != 0)){
        SHR_ERR_EXIT(SHR_E_EXISTS);
    } else if ((opcode != BCMLT_OPCODE_INSERT) && (bmap_ret == 0)){
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief  Populates obm flow usage values to out list.
 *
 * \param [in] unit Unit number.
 * \param [in] out Out field list.
 * \param [in] lport Logical port ID.
 * \param [out] obm_fc_cfg Port flow control configuration populated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_port_usg_cfg_parse (int unit,
                              bcmltd_fields_t *out,
                              bcmtm_lport_t lport,
                              bcmtm_obm_usg_cfg_t *obm_usg_cfg)
{
    bcmlrd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    out->count = 0;

    fid = CTR_TM_OBM_PORT_USAGEt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, out, fid, 0, lport));

    fid = CTR_TM_OBM_PORT_USAGEt_BYTEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, out, fid, 0, obm_usg_cfg->byte));

    fid = CTR_TM_OBM_PORT_USAGEt_LOSSLESS0_BYTEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, out, fid, 0, obm_usg_cfg->lossless0_byte));

    fid = CTR_TM_OBM_PORT_USAGEt_LOSSLESS1_BYTEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, out, fid, 0, obm_usg_cfg->lossless1_byte));

    fid = CTR_TM_OBM_PORT_USAGEt_LOSSY_BYTEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, out, fid, 0, obm_usg_cfg->lossy_byte));

    fid = CTR_TM_OBM_PORT_USAGEt_MAX_USAGE_BYTEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, out, fid, 0, obm_usg_cfg->max_usage_byte));

    fid = CTR_TM_OBM_PORT_USAGEt_OPERATIONAL_STATEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, out, fid, 0, obm_usg_cfg->status));
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief  OBM port usage physical table configuration set
 * based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] lport Logical port number.
 * \param [in] sched_node_id Scheduler node ID.
 * \param [in] ltid Logical table ID.
 * \param [in] shaper_cfg Shaper configuration to be programmed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_port_usage_pt_set(int unit,
                            bcmltd_sid_t ltid,
                            bcmtm_lport_t lport,
                            uint64_t flags,
                            bcmtm_obm_usg_cfg_t *obm_usg_cfg)
{
    bcmtm_drv_t *drv;
    bcmtm_obm_drv_t obm_drv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&obm_drv, 0, sizeof(bcmtm_obm_drv_t));
    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->obm_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->obm_drv_get(unit, &obm_drv));

    /* update physical table. */
    if (obm_drv.port_usg_set) {
        SHR_IF_ERR_EXIT
            (obm_drv.port_usg_set(unit, ltid, lport, flags, obm_usg_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  OBM port usage physical table configuration get
 * based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] lport Logical port number.
 * \param [in] sched_node_id Scheduler node ID.
 * \param [in] ltid Logical table ID.
 * \param [in] shaper_cfg Shaper configuration to be programmed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_port_usage_pt_get(int unit,
                            bcmltd_sid_t ltid,
                            bcmtm_lport_t lport,
                            uint64_t flags,
                            bcmtm_obm_usg_cfg_t *obm_usg_cfg)
{
    bcmtm_drv_t *drv;
    bcmtm_obm_drv_t obm_drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));

    SHR_NULL_CHECK(drv->obm_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->obm_drv_get(unit, &obm_drv));

    /* update physical table. */
    if (obm_drv.port_usg_get) {
        SHR_IF_ERR_EXIT
            (obm_drv.port_usg_get(unit, ltid, lport, flags, obm_usg_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * Public functions
 */
int
bcmtm_obm_port_usage_validate(int unit,
                              bcmlt_opcode_t opcode,
                              const bcmltd_fields_t *in,
                              const bcmltd_generic_arg_t *arg)
{
    bcmlrd_sid_t ltid = arg->sid;
    bcmtm_lport_t lport = 0;
    int bmap_ret = 0;
    bcmtm_obm_usage_info_t *obm_info;
    bcmdrd_pbmp_t obm_pbmp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_lt_field_validate(unit, opcode, ltid, in));

    /* for traverse we need to validate only the field ranges */
    if (opcode == BCMLT_OPCODE_TRAVERSE) {
        SHR_EXIT();
    }

    /* logical port */
    lport = in->field[CTR_TM_OBM_PORT_USAGEt_PORT_IDf]->data;
    if (!bcmtm_obm_port_validation(unit, lport)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmtm_obm_usage_pbmp_info_get(unit, &obm_info));

    BCMDRD_PBMP_ASSIGN(obm_pbmp, obm_info->obm_usage);

    /* check if the entry is available. */
    bmap_ret =
        BCMDRD_PBMP_MEMBER(obm_pbmp, lport);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_lt_opcode_validate(unit, opcode, bmap_ret));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_obm_port_usage_insert(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    bcmtm_lport_t lport;
    bcmtm_obm_usage_info_t *obm_info;
    bcmdrd_pbmp_t obm_pbmp;
    int pport;
    uint32_t fval, id, idx;
    bcmlrd_sid_t ltid = arg->sid;
    bcmtm_obm_usg_cfg_t obm_usg_cfg;
    uint64_t flags;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(out);

    /* logical port ID */
    lport = (uint32_t) in->field[CTR_TM_OBM_PORT_USAGEt_PORT_IDf]->data;

    sal_memset(&obm_usg_cfg, 0, sizeof(bcmtm_obm_usg_cfg_t));
    /* validate if port map is valid. */

    SHR_IF_ERR_EXIT
        (bcmtm_obm_usage_pbmp_info_get(unit, &obm_info));
    BCMDRD_PBMP_ASSIGN(obm_pbmp, obm_info->obm_usage);

    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        SHR_EXIT();
    }

    for (idx = 0; idx < in->count; idx++) {
        id =  in->field[idx]->id;
        fval = in->field[idx]->data;
        if (id == CTR_TM_OBM_PORT_USAGEt_MAX_USAGE_BYTEf) {
            obm_usg_cfg.max_usage_byte = fval;
        }
    }
    flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_EXIT
        (bcmtm_obm_port_usage_pt_set(unit, ltid, lport, flags, &obm_usg_cfg));
exit:
    /* add to port bmap for inserted entries. */
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
       BCMDRD_PBMP_PORT_ADD(obm_pbmp, lport);
       BCMDRD_PBMP_ASSIGN(obm_info->obm_usage, obm_pbmp);
    }
    SHR_FUNC_EXIT();
}

int
bcmtm_obm_port_usage_lookup(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    bcmtm_lport_t lport;
    int pport;
    bcmlrd_sid_t ltid = arg->sid;
    bcmtm_obm_usg_cfg_t obm_usg_cfg;
    uint64_t flags;

    SHR_FUNC_ENTER(unit);
    lport = in->field[CTR_TM_OBM_PORT_USAGEt_PORT_IDf]->data;

    sal_memset(&obm_usg_cfg, 0, sizeof(bcmtm_obm_usg_cfg_t));
    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        obm_usg_cfg.status = PORT_INFO_UNAVAIL;
    } else {
        flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
        SHR_IF_ERR_EXIT
            (bcmtm_obm_port_usage_pt_get(unit, ltid, lport, flags, &obm_usg_cfg));
        obm_usg_cfg.status = VALID;
    }
    SHR_IF_ERR_EXIT
        (bcmtm_obm_port_usg_cfg_parse(unit, out, lport, &obm_usg_cfg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_obm_port_usage_delete(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    bcmtm_lport_t lport;
    bcmtm_obm_usage_info_t *obm_info;
    bcmdrd_pbmp_t obm_pbmp;
    int pport;
    bcmlrd_sid_t ltid = arg->sid;
    bcmtm_obm_usg_cfg_t obm_usg_cfg;
    uint64_t flags;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(out);

    /* logical port ID */
    lport = in->field[CTR_TM_OBM_PORT_USAGEt_PORT_IDf]->data;
    SHR_IF_ERR_EXIT
        (bcmtm_obm_usage_pbmp_info_get(unit, &obm_info));

    BCMDRD_PBMP_ASSIGN(obm_pbmp, obm_info->obm_usage);
    BCMDRD_PBMP_PORT_REMOVE(obm_pbmp, lport);
    BCMDRD_PBMP_ASSIGN(obm_info->obm_usage, obm_pbmp);

    sal_memset(&obm_usg_cfg, 0, sizeof(bcmtm_obm_usg_cfg_t));

    /* validate if port map is valid. */
    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        SHR_EXIT();
    }
    flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_EXIT
        (bcmtm_obm_port_usage_pt_set(unit, ltid, lport, flags, &obm_usg_cfg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_obm_port_usage_update(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    /* same as insert */
    SHR_IF_ERR_EXIT
        (bcmtm_obm_port_usage_insert(unit, op_arg, in, out, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_obm_port_usage_first(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(in);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_obm_port_usage_next(unit, op_arg, NULL, out, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_obm_port_usage_next(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    bcmtm_lport_t lport;
    bcmtm_obm_usage_info_t *obm_info;
    bcmdrd_pbmp_t obm_pbmp;
    uint64_t lport_min, lport_max;
    bcmlrd_sid_t ltid = arg->sid;
    bcmlrd_fid_t fid = CTR_TM_OBM_PORT_USAGEt_PORT_IDf;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_field_value_range_get(unit, ltid, fid, &lport_min, &lport_max));
    if (in) {
        lport = ++(in->field[fid]->data);
    } else {
        lport = lport_min;
    }

    SHR_IF_ERR_EXIT
        (bcmtm_obm_usage_pbmp_info_get(unit, &obm_info));
    BCMDRD_PBMP_ASSIGN(obm_pbmp, obm_info->obm_usage);

    for ( ; lport <= lport_max; lport++) {
        if (BCMDRD_PBMP_MEMBER(obm_pbmp, lport))
            break;
    }

    if (lport > lport_max) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    };
    out->field[0]->data = lport;
    out->field[0]->id = fid;
    out->field[0]->idx = 0;

    SHR_IF_ERR_EXIT
        (bcmtm_obm_port_usage_lookup(unit, op_arg, out, out, arg));
exit:
    SHR_FUNC_EXIT();
}


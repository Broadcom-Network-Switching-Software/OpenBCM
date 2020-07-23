/*! \file bcmpc_lth_pm_prop.c
 *
 * Logical Table Custom Handlers for PC_PM_PROP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmpc/bcmpc_lth_internal.h>
#include <bcmpc/bcmpc_lth_pm_prop.h>
#include <bcmpc/bcmpc_topo_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmlrd/bcmlrd_client.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_LTH

/*! Logical table ID for this handler. */
#define LTH_SID PC_PM_PROPt


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the enum value PC_PM_TYPE_T by PM name.
 *
 * \param unit Unit number.
 * \param pm_name PM name.
 *
 * \param [in] pm_name PM type name.
 *
 * \return Enum number PC_PM_TYPE_T for \c pm_name.
 */
static uint32_t
pc_pm_type_t_get(int unit, const char *pm_name)
{
    int rv;
    uint32_t value = 0;

    rv = bcmlrd_field_symbol_to_value(unit, LTH_SID, PC_PM_PROPt_PM_TYPEf,
                                      pm_name, &value);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Unknown PM Type: %s.\n"), pm_name));
    }

    return value;
}


/*******************************************************************************
 * Public functions
 */

int
bcmpc_lth_pm_prop_validate(int unit,
                           bcmlt_opcode_t opcode,
                           const bcmltd_fields_t *in,
                           const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_fields_validate(unit, LTH_SID, in));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_pm_prop_insert(int unit,
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
bcmpc_lth_pm_prop_lookup(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    uint64_t fval;
    int pm_id, fcnt = 0, idx, plane;
    bcmpc_topo_t pm_info;
    bcmpc_topo_type_t tinfo;
    bcmpc_vco_type_t bcmpc_vco_rate[2] = {BCMPC_VCO_RATE_NONE};
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PM_PROPt_PC_PM_IDf, &fval));
    pm_id = fval;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_get(unit, pm_id, &pm_info));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_type_get(unit, pm_info.tid, &tinfo));

    /* Get first physical port number on the PM. */
    for ( plane = 0; plane < BCMPC_NUM_PPORTS_PER_PM_MAX; plane++) {
        if (pm_info.port_num_lanes[plane]) {
            pport = pm_info.base_pport + plane;
            break;
        }
    }
    if (pport != BCMPC_INVALID_PPORT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_pm_vco_rate_get(unit, pport, bcmpc_vco_rate));
    }
    BCMPC_LTH_FIELD_ADD(out, PC_PM_PROPt_PC_PM_IDf, 0, pm_id, fcnt);
    fval = pc_pm_type_t_get(unit, tinfo.name);
    BCMPC_LTH_FIELD_ADD(out, PC_PM_PROPt_PM_TYPEf, 0, fval, fcnt);
    fval = tinfo.num_lanes;
    BCMPC_LTH_FIELD_ADD(out, PC_PM_PROPt_NUM_LANESf, 0, fval, fcnt);
    fval = tinfo.num_ports;
    BCMPC_LTH_FIELD_ADD(out, PC_PM_PROPt_NUM_PORTSf, 0, fval, fcnt);
    fval = tinfo.num_aggr;
    BCMPC_LTH_FIELD_ADD(out, PC_PM_PROPt_NUM_AGGRf, 0, fval, fcnt);
    fval = tinfo.num_plls;
    BCMPC_LTH_FIELD_ADD(out, PC_PM_PROPt_NUM_PLLf, 0, fval, fcnt);
    fval = tinfo.tvco_pll_index;
    BCMPC_LTH_FIELD_ADD(out, PC_PM_PROPt_TVCO_SOURCE_INDEXf, 0, fval, fcnt);
    for (idx = 0; idx < tinfo.num_plls; idx++) {
        BCMPC_LTH_FIELD_ADD(out, PC_PM_PROPt_VCO_RATEf, idx, bcmpc_vco_rate[idx], fcnt);
    }

    out->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_pm_prop_delete(int unit,
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
bcmpc_lth_pm_prop_update(int unit,
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
bcmpc_lth_pm_prop_first(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t min, max;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(in);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PM_PROPt_PC_PM_IDf,
                                         &min, &max));
    flist = &fld;
    flds.field = &flist;
    flds.field[0]->id = PC_PM_PROPt_PC_PM_IDf;
    flds.field[0]->data = min;
    flds.count = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_pm_prop_lookup(unit, op_arg, &flds, out, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_pm_prop_next(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t pm_id, min, max;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PM_PROPt_PC_PM_IDf,
                                         &min, &max));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PM_PROPt_PC_PM_IDf, &pm_id));

    pm_id++;

    /* Stop traverse when exceeding the max PM ID. */
    while (pm_id <= max) {
        /* Exit the current iteration once we get a valid entry. */
        flist = &fld;
        flds.field = &flist;
        flds.field[0]->id = PC_PM_PROPt_PC_PM_IDf;
        flds.field[0]->data = pm_id;
        flds.count = 1;
        rv = bcmpc_lth_pm_prop_lookup(unit, op_arg, &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_EXIT();
        }
        pm_id++;
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

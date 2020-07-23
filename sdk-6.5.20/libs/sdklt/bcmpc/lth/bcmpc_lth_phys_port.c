/*! \file bcmpc_lth_phys_port.c
 *
 * Logical Table Custom Handlers for PC_PHYS_PORT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmpc/bcmpc_topo_internal.h>
#include <bcmpc/bcmpc_lth_internal.h>
#include <bcmpc/bcmpc_lth_phys_port.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_LTH

/*! Logical table ID for this handler. */
#define LTH_SID PC_PHYS_PORTt


/*******************************************************************************
 * Public functions
 */

int
bcmpc_lth_phys_port_validate(int unit,
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
bcmpc_lth_phys_port_insert(int unit,
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
bcmpc_lth_phys_port_lookup(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    uint64_t fval;
    int rv, pm_id;
    bcmpc_pport_t pport;
    bcmpc_topo_t pm_info;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PHYS_PORTt_PC_PHYS_PORT_IDf, &fval));
    pport = fval;

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (rv == SHR_E_NOT_FOUND) {
        /*
         * If the given physical port number is not found in the topology
         * database, treat it as an invalid value.
         */
        SHR_IF_ERR_MSG_EXIT
            (rv,
             (BSL_META_U(unit, "Invalid PC_PHYS_PORT_ID value.\n")));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_get(unit, pm_id, &pm_info));

    out->field[0]->id = PC_PHYS_PORTt_PC_PHYS_PORT_IDf;
    out->field[0]->data = pport;
    out->field[1]->id = PC_PHYS_PORTt_PC_PM_IDf;
    out->field[1]->data = pm_id;
    out->field[2]->id = PC_PHYS_PORTt_PM_PHYS_PORTf;
    out->field[2]->data = pport - pm_info.base_pport;
    out->count = 3;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_phys_port_delete(int unit,
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
bcmpc_lth_phys_port_update(int unit,
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
bcmpc_lth_phys_port_first(int unit,
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
                                         PC_PHYS_PORTt_PC_PHYS_PORT_IDf,
                                         &min, &max));
    flist = &fld;
    flds.field = &flist;
    flds.field[0]->id = PC_PHYS_PORTt_PC_PHYS_PORT_IDf;
    flds.field[0]->data = min;
    flds.count = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_phys_port_lookup(unit, op_arg, &flds, out, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_phys_port_next(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t pport, min, max;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PHYS_PORTt_PC_PHYS_PORT_IDf,
                                         &min, &max));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PHYS_PORTt_PC_PHYS_PORT_IDf, &pport));

    pport++;

    /* Stop traverse when exceeding the max physical port number. */
    while (pport <= max) {
        /* Exit the current iteration once we get a valid entry. */
        flist = &fld;
        flds.field = &flist;
        flds.field[0]->id = PC_PHYS_PORTt_PC_PHYS_PORT_IDf;
        flds.field[0]->data = pport;
        flds.count = 1;
        rv = bcmpc_lth_phys_port_lookup(unit, op_arg, &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_EXIT();
        }
        pport++;
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}


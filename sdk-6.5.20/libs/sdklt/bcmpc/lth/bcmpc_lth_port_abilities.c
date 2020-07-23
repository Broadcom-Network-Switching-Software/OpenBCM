/*! \file bcmpc_lth_port_abilities.c
 *
 * Logical Table Custom Handlers for PC_PORT_ABILITIES.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmpc/bcmpc_lth_internal.h>
#include <bcmpc/bcmpc_lth_port_abilities.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_util_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_LTH

/*! Logical table ID for this handler. */
#define LTH_SID PC_PORT_ABILITIESt

/*! Data structure to save the data of PC_PORT_ABILITIESt entry. */
typedef struct pc_port_abilities_data_s {
    bcmpc_pm_port_ability_t port_ability;
    bcmpc_pmgr_port_abilities_t port_abilities_st;
} pc_port_abilities_data_t;


/*******************************************************************************
 * Private functions
 */

static void
pc_port_abilities_data_t_init(pc_port_abilities_data_t *port_ability_data)
{
    port_ability_data->port_ability.lport = BCMPC_INVALID_LPORT;
    port_ability_data->port_ability.ability_type = 0;
    bcmpc_pmgr_port_abilities_t_init(&(port_ability_data->port_abilities_st));
}

/*!
 * \brief LTA input fields parsing.
 *
 * Parse LTA input fields and save the data to pc_pm_port_abilities_data_t.
 *
 * \param [in] unit Unit number.
 * \param [in] in LTA input field array.
 * \param [out] port_ability Port ability data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c lane_st_data.
 */
static int
lt_fields_parse(int unit, const bcmltd_fields_t *in,
                pc_port_abilities_data_t *port_abilities_data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;
    bcmpc_pm_port_ability_t *port_ability = &port_abilities_data->port_ability;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < in->count; i++) {
        fid = in->field[i]->id;
        fval = in->field[i]->data;

        switch (fid) {
            case PC_PORT_ABILITIESt_PORT_IDf:
                port_ability->lport = fval;
                break;
            case PC_PORT_ABILITIESt_ABILITY_TYPEf:
                port_ability->ability_type = fval;
                break;
            default:
                continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_lth_field_validate(unit, LTH_SID, in->field[i]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the input keys of LT operation.
 *
 * \param [in] unit Unit number.
 * \param [in] port_abilities_data Port abilities status data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more keys are not given.
 * \retval SHR_E_PARAM One or more key values are invalid.
 */
static int
lt_key_fields_validate(int unit, pc_port_abilities_data_t *port_abilities_data)
{
    bcmpc_pm_port_ability_t *port_ability = &port_abilities_data->port_ability;
    bcmpc_lport_t lport;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    if (port_ability->lport == BCMPC_INVALID_LPORT) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "PORT_ID is not specified.\n")));
    }

    if (port_ability->ability_type > 1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "ABILITY_TYPE is not specified.\n")));
    }

    lport = port_ability->lport;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg)));

exit:
    SHR_FUNC_EXIT();
}



/*******************************************************************************
 * Public functions
 */

int
bcmpc_lth_port_abilities_validate(int unit,
                                  bcmlt_opcode_t opcode,
                                  const bcmltd_fields_t *in,
                                  const bcmltd_generic_arg_t *arg)
{
    pc_port_abilities_data_t port_abilities_st_data;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(arg);

    pc_port_abilities_data_t_init(&port_abilities_st_data);
    SHR_IF_ERR_VERBOSE_EXIT
        (lt_fields_parse(unit, in, &port_abilities_st_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_key_fields_validate(unit, &port_abilities_st_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_abilities_insert(int unit,
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
bcmpc_lth_port_abilities_lookup(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    int fcnt = 0, idx = 0;
    uint64_t fval;
    bcmpc_lport_t lport;
    bcmpc_pport_t pport;
    bcmpc_port_cfg_t pcfg;
    pc_port_abilities_data_t port_abil_data;
    bcmpc_pmgr_port_abilities_t *pabilities = &port_abil_data.port_abilities_st;
    bcmpc_pmgr_port_ability_type_t ability_type;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(arg);

    pc_port_abilities_data_t_init(&port_abil_data);

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_fields_parse(unit, in, &port_abil_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PORT_ABILITIESt_PORT_IDf, &fval));
    lport = fval;

    /* Check if the port is configured. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg)));

    SHR_IF_ERR_VERBOSE_EXIT(bcmpc_lth_field_get(unit, in,
                            PC_PORT_ABILITIESt_ABILITY_TYPEf, &fval));
    ability_type = fval;

    pport = bcmpc_lport_to_pport(unit, lport);
    if (pport != BCMPC_INVALID_PPORT) {
        bcmpc_pmgr_port_abilities_t_init(pabilities);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_port_abilities_get(unit, lport, pport, ability_type,
                                           pabilities));
    }

    BCMPC_LTH_FIELD_ADD(out, PC_PORT_ABILITIESt_PORT_IDf, 0, lport, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_ABILITIESt_ABILITY_TYPEf, 0,
                        ability_type, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_ABILITIESt_NUM_ABILITIESf, 0,
                        pabilities->num_ability, fcnt);

    for (idx = 0; idx < pabilities->num_ability; idx++) {
        BCMPC_LTH_FIELD_ADD(out, PC_PORT_ABILITIESt_SPEEDf, idx,
                        pabilities->port_abilities[idx].speed, fcnt);
    }
    for (idx = 0; idx < pabilities->num_ability; idx++) {
        BCMPC_LTH_FIELD_ADD(out, PC_PORT_ABILITIESt_NUM_LANESf, idx,
                        pabilities->port_abilities[idx].num_lanes, fcnt);
    }
    for (idx = 0; idx < pabilities->num_ability; idx++) {
        BCMPC_LTH_FIELD_ADD(out, PC_PORT_ABILITIESt_FEC_MODEf, idx,
                        pabilities->port_abilities[idx].fec_type, fcnt);
    }
    for (idx = 0; idx < pabilities->num_ability; idx++) {
        BCMPC_LTH_FIELD_ADD(out, PC_PORT_ABILITIESt_AUTONEG_MODEf, idx,
                        pabilities->port_abilities[idx].an_mode, fcnt);
    }
    for (idx = 0; idx < pabilities->num_ability; idx++) {
        BCMPC_LTH_FIELD_ADD(out, PC_PORT_ABILITIESt_MEDIUM_TYPEf, idx,
                        pabilities->port_abilities[idx].medium, fcnt);
    }
    for (idx = 0; idx < pabilities->num_ability; idx++) {
        BCMPC_LTH_FIELD_ADD(out, PC_PORT_ABILITIESt_PAUSE_TYPEf, idx,
                        pabilities->port_abilities[idx].pause, fcnt);
    }
    for (idx = 0; idx < pabilities->num_ability; idx++) {
        BCMPC_LTH_FIELD_ADD(out, PC_PORT_ABILITIESt_CHANNEL_TYPEf, idx,
                        pabilities->port_abilities[idx].channel, fcnt);
    }

    out->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_abilities_delete(int unit,
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
bcmpc_lth_port_abilities_update(int unit,
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
bcmpc_lth_port_abilities_first(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist[2], fld[2];
    uint64_t lport, pmin, pmax;
    uint64_t ability_type, amin, amax;
    int rv, fcnt;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(in);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PORT_ABILITIESt_PORT_IDf,
                                         &pmin, &pmax));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PORT_ABILITIESt_ABILITY_TYPEf,
                                         &amin, &amax));

    lport = pmin;
    ability_type = amin;

    while (lport <= pmax) {
        fcnt = 0;
        flds.field = flist;
        flds.field[fcnt] = &fld[fcnt];
        flds.field[fcnt]->id = PC_PORT_ABILITIESt_PORT_IDf;
        flds.field[fcnt]->data = lport;
        fcnt++;
        flds.field[fcnt] = &fld[fcnt];
        flds.field[fcnt]->id = PC_PORT_ABILITIESt_ABILITY_TYPEf;
        flds.field[fcnt]->data = ability_type;
        fcnt++;
        flds.count = fcnt;
        rv = bcmpc_lth_port_abilities_lookup(unit, op_arg, &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_EXIT();
        }
        lport++;
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_abilities_next(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg)
{
    int fcnt = 0;
    uint64_t fval;
    bcmltd_fields_t flds;
    bcmltd_field_t *flist[2], fld[2];
    uint64_t lport, pmax, pmin;
    uint64_t ability_type;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PORT_ABILITIESt_PORT_IDf, &fval));
    lport = fval;

    SHR_IF_ERR_VERBOSE_EXIT(bcmpc_lth_field_get(unit, in,
                                    PC_PORT_ABILITIESt_ABILITY_TYPEf, &fval));
    ability_type = fval;

    if (ability_type == 1) {
        lport++;
        ability_type = 0;
    } else {
        ability_type = 1;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PORT_ABILITIESt_PORT_IDf,
                                         &pmin, &pmax));

    /* Stop traverse when exceeding the max PM ID. */
    while (lport <= pmax) {
        fcnt = 0;
        flds.field = flist;
        flds.field[fcnt] = &fld[fcnt];
        flds.field[fcnt]->id = PC_PORT_ABILITIESt_PORT_IDf;
        flds.field[fcnt]->data = lport;
        fcnt++;
        flds.field[fcnt] = &fld[fcnt];
        flds.field[fcnt]->id = PC_PORT_ABILITIESt_ABILITY_TYPEf;
        flds.field[fcnt]->data = ability_type;
        fcnt++;
        flds.count = fcnt;
        rv = bcmpc_lth_port_abilities_lookup(unit, op_arg, &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_EXIT();
        }
        lport++;
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

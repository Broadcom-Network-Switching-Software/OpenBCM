/*! \file bcmpc_imm_lm_port_control.c
 *
 * LM_PORT_CONTROLt IMM handler.
 * We only need to 'read' LM_PORT_CONTROLt for getting the linkscan mode.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmimm/bcmimm_int_comp.h>

#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID LM_PORT_CONTROLt


/*******************************************************************************
 * Private Function
 */

/*!
 * \brief Convert the linkscan mode from LM to PC.
 *
 * \param [in] unit Unit number.
 * \param [in] ls_ls_mode Linkscan mode defined by LM_LINKSCAN_MODE_T.type.ltl.
 *
 * \return The corresponding linkscan mode value in bcmpc_ls_mode_t.
 */
static bcmpc_ls_mode_t
lm_ls_mode_to_pc(int unit, uint32_t lm_mode)
{
    int rv;
    const char *str_mode = NULL;

    rv = bcmlrd_field_value_to_symbol(unit, IMM_SID,
                                      LM_PORT_CONTROLt_LINKSCAN_MODEf,
                                      lm_mode, &str_mode);

    if (SHR_FAILURE(rv) || !str_mode) {
        return BCMPC_LS_MODE_DISABLED;
    } else if (sal_strcmp(str_mode, "NO_SCAN") == 0) {
        return BCMPC_LS_MODE_DISABLED;
    } else if (sal_strcmp(str_mode, "SOFTWARE") == 0) {
        return BCMPC_LS_MODE_SW;
    }

    return BCMPC_LS_MODE_OTHER;
}


/*******************************************************************************
 * DB schema handler
 */

/*!
 * \brief Convert the LT fields to the PC data structure.
 *
 * This is the parse callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert bcmltd_fields_t array \c flds to a bcmlm_port_ctrl_t \c key and a
 * bcmlm_port_ctrl_cfg_t \c data.
 *
 * \param [in] unit Unit number.
 * \param [in] flds LT field array.
 * \param [in] key_size Key size.
 * \param [out] key Key value from \c flds.
 * \param [in] data_size Data size.
 * \param [out] data Data value from \c flds.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
lm_port_ctrl_entry_parse(int unit, const bcmltd_fields_t *flds,
                         size_t key_size, void *key,
                         size_t data_size, void *data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;
    bcmpc_lport_t *lport = key;
    bcmpc_lm_mode_t *lm_mode = data;

    SHR_FUNC_ENTER(unit);

    if ((key && key_size != sizeof(*lport)) ||
        (data && data_size != sizeof(*lm_mode))) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fval = flds->field[i]->data;

        switch (fid) {
            case LM_PORT_CONTROLt_PORT_IDf:
                *lport = fval;
                break;
            case LM_PORT_CONTROLt_LINKSCAN_MODEf:
                lm_mode->ls_mode = lm_ls_mode_to_pc(unit, fval);
                break;
            case LM_PORT_CONTROLt_OVERRIDE_LINK_STATEf:
                lm_mode->override_link_state = (bool) fval;
                break;
            case LM_PORT_CONTROLt_MANUAL_SYNCf:
                lm_mode->manual_sync = (bool) fval;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert the PC data structure to the LT fields.
 *
 * This is the fill callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert a bcmlm_port_ctrl_t \c key and a bcmlm_port_ctrl_cfg_t \c data to
 * bcmltd_fields_t array \c flds.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 * \param [in] data_size Data size.
 * \param [in] data Data value.
 * \param [out] flds LT field array.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
lm_port_ctrl_entry_fill(int unit,
                      size_t key_size, void *key,
                      size_t data_size, void *data,
                      bcmltd_fields_t *flds)
{
    size_t fcnt = 0;
    bcmpc_lport_t *lport = key;

    SHR_FUNC_ENTER(unit);

    /*
     * PC only needs to LOOKUP LM_PORT_CONTROLt, so we only fill the key field
     * of the LT here.
     */
    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, LM_PORT_CONTROLt_PORT_IDf, 0, *lport, fcnt);
    }

    flds->count = fcnt;

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_lm_port_ctrl_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = lm_port_ctrl_entry_parse;
    db_hdl.entry_fill = lm_port_ctrl_entry_fill;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, NULL));

exit:
    SHR_FUNC_EXIT();
}

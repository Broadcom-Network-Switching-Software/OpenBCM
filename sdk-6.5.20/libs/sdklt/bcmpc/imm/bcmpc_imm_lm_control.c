/*! \file bcmpc_imm_lm_control.c
 *
 * LM_CONTROLt IMM handler.
 * We only need to 'read' LM_CONTROLt for getting the linkscan enable state.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


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
#define IMM_SID LM_CONTROLt


/*******************************************************************************
 * DB schema handler
 */

/*!
 * \brief Convert the LT fields to the PC data structure.
 *
 * This is the parse callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert bcmltd_fields_t array \c flds to a bcmlm_ctrl_t \c key and a
 * bcmlm_ctrl_cfg_t \c data.
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
lm_ctrl_entry_parse(int unit, const bcmltd_fields_t *flds,
                    size_t key_size, void *key,
                    size_t data_size, void *data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;
    uint32_t *ls_enable = data;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fval = flds->field[i]->data;

        switch (fid) {
            case LM_CONTROLt_SCAN_ENABLEf:
                *ls_enable = fval;
                break;
            default:
                break;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert the PC data structure to the LT fields.
 *
 * This is the fill callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert a bcmlm_ctrl_t \c key and a bcmlm_ctrl_cfg_t \c data to
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
lm_ctrl_entry_fill(int unit,
                   size_t key_size, void *key,
                   size_t data_size, void *data,
                   bcmltd_fields_t *flds)
{
    /*
     * PC only needs to LOOKUP LM_CONTROLt which is a keyless table.
     */
    return SHR_E_NONE;
}


/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_lm_ctrl_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = lm_ctrl_entry_parse;
    db_hdl.entry_fill = lm_ctrl_entry_fill;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, NULL));

exit:
    SHR_FUNC_EXIT();
}

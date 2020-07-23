/*! \file bcmpc_imm_skeleton.c
 *
 * BCMPC interface to in-memory table.
 *
 * There are two handlers in this file.
 * 1. IMM event handler, \ref bcmimm_lt_cb_t.
 *    The event handler is used to notify PC for the various LT operations.
 *
 * 2. IMM-based DB schema handler, \ref bcmpc_db_imm_schema_handler_t.
 *    The schema handler is used to translate IMM (or LT) field array to/from PC
 *    specific data type.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>

#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>

int bcmpc_imm_lt_register(int unit);

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_PORTt


/*******************************************************************************
 * DB schema handler
 */

static int
pc_lt_entry_parse(int unit, const bcmltd_fields_t *flds,
                  size_t key_size, void *key,
                  size_t data_size, void *data)
{
    return SHR_E_NONE;
}

static int
pc_lt_entry_fill(int unit,
                 size_t key_size, void *key,
                 size_t data_size, void *data,
                 bcmltd_fields_t *flds)
{
    return SHR_E_NONE;
}

static bcmpc_db_imm_schema_handler_t pc_db_imm_schema_hdl = {

    /*! Parse function. */
    .entry_parse = pc_lt_entry_parse,

    /*! Fill function. */
    .entry_fill = pc_lt_entry_fill
};


/*******************************************************************************
 * IMM event handler
 */

static int
pc_imm_lt_validate(int unit,
                   bcmltd_sid_t sid,
                   bcmimm_entry_event_t action,
                   const bcmltd_field_t *key_fields,
                   const bcmltd_field_t *data_fields,
                   void *context)
{
    return SHR_E_NONE;
}

static void
pc_imm_lt_abort(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                void *context)
{
    return;
}

static int
pc_imm_lt_stage(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key_fields,
                const bcmltd_field_t *data_fields,
                void *context,
                bcmltd_fields_t *output_fields)
{
    if (output_fields) {
        output_fields->count = 0;
    }
    return SHR_E_NONE;
}

static int
pc_imm_lt_commit(int unit,
                 bcmltd_sid_t sid,
                 uint32_t trans_id,
                 void *context)
{
    return SHR_E_NONE;
}

static bcmimm_lt_cb_t pc_imm_event_hdl = {

    /*! Validate function. */
    .validate = pc_imm_lt_validate,

    /*! Staging function. */
    .stage = pc_imm_lt_stage,

    /*! Commit function. */
    .commit = pc_imm_lt_commit,

    /*! Abort function. */
    .abort = pc_imm_lt_abort
};


/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_lt_register(int unit)
{
    SHR_FUNC_ENTER(unit);

    /*
     * To register the PC LT handler here.
     *
     * SHR_IF_ERR_EXIT
     *     (bcmpc_db_imm_schema_reg(unit, IMM_SID, &pc_db_imm_schema_hdl));
     *
     * SHR_IF_ERR_EXIT
     *     (bcmimm_lt_event_reg(unit, IMM_SID, &pc_imm_event_hdl, NULL));
     */
    COMPILER_REFERENCE(pc_db_imm_schema_hdl);
    COMPILER_REFERENCE(pc_imm_event_hdl);

    SHR_FUNC_EXIT();
}

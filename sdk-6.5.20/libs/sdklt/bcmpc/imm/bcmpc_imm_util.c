/*! \file bcmpc_imm_util.c
 *
 * BCMPC utility for IMM subcomponent.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>

#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM


/*******************************************************************************
 * Public functions
 */

int
bcmpc_imm_lt_field_get(int unit, const bcmltd_field_t *flds, uint32_t fid,
                       uint64_t *fval)
{
    const bcmltd_field_t *fld;

    SHR_FUNC_ENTER(unit);

    fld = flds;
    while (fld) {
        if (fld->id == fid) {
            *fval = fld->data;
            SHR_EXIT();
        }
        fld = fld->next;
    }

    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_imm_tbl_register(int unit, bcmltd_sid_t sid,
                      bcmpc_db_imm_schema_handler_t *db_hdl,
                      bcmimm_lt_cb_t *event_hdl)
{
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    /* Return E_NONE when there is no map to this table. */
    if (SHR_FAILURE(bcmlrd_map_get(unit, sid, &map))) {
        SHR_EXIT();
    }

    /* Register DB schema handler. */
    if (db_hdl) {
        SHR_IF_ERR_EXIT
            (bcmpc_db_imm_schema_reg(unit, sid, db_hdl));
    }

    /* Register IMM event handler. */
    if (event_hdl) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, sid, event_hdl, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}


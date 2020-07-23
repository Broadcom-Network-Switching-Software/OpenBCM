/*! \file bcmtm_imm_cos_q_cpu_map_info.c
 *
 * IMM interface for TM_COS_Q_CPU_MAP_INFO logical table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmcth/bcmcth_tm_drv.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */
static int
bcmtm_cos_q_cpu_map_info_validate(int unit,
                                 bcmltd_sid_t sid,
                                 bcmimm_entry_event_t event_reason,
                                 const bcmltd_field_t *key_fields,
                                 const bcmltd_field_t *data_fields,
                                 void *context)
{
    SHR_FUNC_ENTER(unit);

    if ((event_reason == BCMIMM_ENTRY_INSERT) ||
        (event_reason == BCMIMM_ENTRY_DELETE) ||
        (event_reason == BCMIMM_ENTRY_UPDATE)) {
        SHR_ERR_EXIT(SHR_E_DISABLED);
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmtm_cos_q_cpu_map_info_imm_register(int unit)
{
    bcmimm_lt_cb_t bcmtm_q_assign_imm_cb = {
        .validate = bcmtm_cos_q_cpu_map_info_validate,
    };

    const bcmlrd_map_t *map = NULL;
    bcmcth_tm_info_drv_t *drv;
    bcmcth_info_table_t data;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(bcmtm_cos_q_cpu_map_info_drv_get(unit, &drv))) {
        SHR_EXIT();
    }

    if (drv && drv->get_data) {
        SHR_IF_ERR_EXIT
            (drv->get_data(unit, &data));
    } else {
        SHR_EXIT();
    }

    rv = bcmlrd_map_get(unit, data.table_id, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, data.table_id,
                             &bcmtm_q_assign_imm_cb, NULL));

exit:
    SHR_FUNC_EXIT();
}

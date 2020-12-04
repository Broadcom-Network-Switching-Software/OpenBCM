/*! \file bcmcth_flex_qos_egr_info_imm.c
 *
 * FlexQoS interfaces to egress IMM info table.
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

#include <bcmcth/bcmcth_flex_qos_drv.h>
/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_FLEX_QOS

/*******************************************************************************
 * Private functions
 */

/*!
 * In-memory logical table validate callback function.
 */
static int
flex_qos_egr_info_tbl_cb(int unit,
                         bcmltd_sid_t sid,
                         bcmimm_entry_event_t event_reason,
                         const bcmltd_field_t *key_fields,
                         const bcmltd_field_t *data_fields,
                         void *context)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Insert, Update and Delete not supported for read-only logical tables */
    if ((event_reason == BCMIMM_ENTRY_INSERT) ||
        (event_reason == BCMIMM_ENTRY_DELETE) ||
        (event_reason == BCMIMM_ENTRY_UPDATE)) {
        rv = SHR_E_DISABLED;
        SHR_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

static bcmimm_lt_cb_t flex_qos_egr_info_tbl_callback = {

    /*! Validate function. */
    .validate = flex_qos_egr_info_tbl_cb,

    /*! Staging function. */
    .stage = NULL,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};


/*******************************************************************************
 * Public Functions
 */
int
bcmcth_flex_qos_egr_info_imm_register(int unit, int table_id)
{
    const bcmlrd_map_t *map = NULL;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, table_id, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 table_id,
                                 &flex_qos_egr_info_tbl_callback,
                                 NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

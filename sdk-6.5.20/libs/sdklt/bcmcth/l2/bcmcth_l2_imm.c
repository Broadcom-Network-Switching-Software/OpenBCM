/*! \file bcmcth_l2_imm.c
 *
 * L2 interfaces to IMM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_bitop.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmcth/bcmcth_util.h>
#include <bcmcth/bcmcth_l2.h>
#include <bcmcth/bcmcth_l2_util.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_L2


/*******************************************************************************
 * Private functions
 */
static int
l2_learn_control_stage_cb(int unit,
                          bcmltd_sid_t sid,
                          uint32_t trans_id,
                          bcmimm_entry_event_t entry_event,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          void *context,
                          bcmltd_fields_t *output_fields)
{
    l2_learn_control_info_t lt_info;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(sid);
    COMPILER_REFERENCE(context);

    if (output_fields) {
        output_fields->count = 0;
    }

    sal_memset(&lt_info, 0, sizeof(l2_learn_control_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_l2_drv_entry_parse(unit, key, data, &lt_info));

    switch (entry_event) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (SHR_BITGET(lt_info.fbmp, LEARN_CONTROL_REPORT)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_l2_learn_report_enable(unit, trans_id, lt_info.enable));
        }
        if (SHR_BITGET(lt_info.fbmp, LEARN_CONTROL_SLOW_POLL)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_l2_learn_slow_poll_enable(unit, lt_info.slow_poll));
        }
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_l2_learn_report_enable(unit, trans_id, 0));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


static int
l2_learn_data_validate_cb(int unit,
                          bcmltd_sid_t sid,
                          bcmimm_entry_event_t entry_event,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          void *context)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(sid);
    COMPILER_REFERENCE(key);
    COMPILER_REFERENCE(data);
    COMPILER_REFERENCE(context);

    switch (entry_event) {
    case BCMIMM_ENTRY_DELETE:
    case BCMIMM_ENTRY_LOOKUP:
        SHR_EXIT();
    default:
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
l2_learn_data_stage_cb(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       bcmimm_entry_event_t entry_event,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       void *context,
                       bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(sid);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(data);
    COMPILER_REFERENCE(context);

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (entry_event) {
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_l2_drv_entry_delete(unit, (void *)key));
        break;
    default:
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

static bool
bcmcth_l2_imm_mapped(int unit, const bcmdrd_sid_t sid)
{
    int rv;
    const bcmlrd_map_t *map = NULL;

    rv = bcmlrd_map_get(unit, sid, &map);
    if (SHR_SUCCESS(rv) &&
        map &&
        map->group &&
        map->group[0].dest.kind == BCMLRD_MAP_CUSTOM) {
        return TRUE;
    }
    return FALSE;
}

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_l2_learn_imm_register(int unit)
{
    bcmcth_l2_drv_t *drv = NULL;
    bcmimm_lt_cb_t cb;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);

    if (bcmcth_l2_imm_mapped(unit, drv->cfg_sid)) {
        sal_memset(&cb, 0, sizeof(cb));
        cb.stage = l2_learn_control_stage_cb;

        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, drv->cfg_sid, &cb, NULL));
    }

    if (bcmcth_l2_imm_mapped(unit, drv->state_sid)) {
        sal_memset(&cb, 0, sizeof(cb));
        cb.stage = l2_learn_data_stage_cb;
        cb.validate = l2_learn_data_validate_cb;

        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, drv->state_sid, &cb, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

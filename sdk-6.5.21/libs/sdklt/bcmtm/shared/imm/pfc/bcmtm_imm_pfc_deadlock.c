/*! \file bcmtm_imm_pfc_deadlock.c
 *
 * In-memory call back function implementation for PFC deadlock Logical tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>
#include <bcmtm/pfc/bcmtm_pfc_deadlock_internal.h>
#include <bcmltd/bcmltd_bitop.h>
#include <bcmtm/bcmtm_ha_internal.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>

/*******************************************************************************
 * Local definitions
 */
bcmtm_pfc_ddr_ctrl_t *bcmtm_pfc_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/*! Possible events for PFC Pri deadlock recovery state machine. */
typedef enum bcmtm_pfc_deadlock_event_e {
    BCMTM_PFC_EVENT_OFF,
    BCMTM_PFC_EVENT_ON,
    BCMTM_PFC_EVENT_DEADLOCK,
    BCMTM_PFC_EVENT_RECOVERY,
    BCMTM_PFC_EVENT_IDLE,
} bcmtm_pfc_deadlock_event_t;

#define VALID_OP                BCMLTD_COMMON_TM_PORT_ENTRY_STATE_T_T##_VALID
#define PORT_INFO_UNAVAIL_OP    BCMLTD_COMMON_TM_PORT_ENTRY_STATE_T_T##_PORT_INFO_UNAVAILABLE

#define IDLE                BCMLTD_COMMON_TM_PFC_DEADLOCK_RECOVERY_STATE_STATE_T_T##_IDLE
#define RECOVERY            BCMLTD_COMMON_TM_PFC_DEADLOCK_RECOVERY_STATE_STATE_T_T##_RECOVERY

#define IDLE_CONTROL        BCMLTD_COMMON_TM_PFC_DEADLOCK_RECOVERY_STATE_CONTROL_STATE_T_T##_IDLE

#define MANUAL_IDLE         BCMLTD_COMMON_TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_STATE_T_T##_IDLE
#define MANUAL_RECOVERY     BCMLTD_COMMON_TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_STATE_T_T##_RECOVERY
#define MANUAL_DEADLOCK     BCMLTD_COMMON_TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_STATE_T_T##_DEADLOCK

#define MANUAL_IDLE_CONTROL BCMLTD_COMMON_TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_CONTROL_STATE_T_T##_IDLE
#define MANUAL_RECOVERY_CONTROL BCMLTD_COMMON_TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_CONTROL_STATE_T_T##_RECOVERY

/* Protect the critical zone. */
#define PFC_DDR_INFO_LOCK(unit) \
    sal_mutex_take(bcmtm_pfc_ctrl[unit]->info_lock, SAL_MUTEX_FOREVER)

#define PFC_DDR_INFO_UNLOCK(unit) \
    sal_mutex_give(bcmtm_pfc_ctrl[unit]->info_lock)

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

#define GETLTFIELD(_field)                                      \
        TM_PFC_DEADLOCK_RECOVERYt_##_field##f

static int
bcmtm_pfc_deadlock_config_set(int unit,
                              bcmltd_sid_t ltid,
                              uint32_t fid,
                              bcmtm_lport_t lport,
                              uint32_t pri,
                              uint32_t val,
                              uint32_t *val_oper,
                              bool reset);

int
bcmtm_pfc_deadlock_recovery_state_populate_imm_entry(int unit ,
                                                     int lport,
                                                     uint32_t pri,
                                                     uint8_t state,
                                                     bool deadlock_hdlr,
                                                     bool port_up)
{
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    const bcmlrd_map_t *map = NULL;
    size_t num_fid;
    bcmltd_sid_t ltid = TM_PFC_DEADLOCK_RECOVERY_STATEt;
    int pport;
    int rv = 0;
    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    rv = bcmlrd_map_get(unit, ltid, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, ltid, &num_fid));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &in));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    SHR_IF_ERR_EXIT
        (bcmtm_default_entry_array_construct(unit, ltid, &in));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in,
                              TM_PFC_DEADLOCK_RECOVERY_STATEt_PORT_IDf,
                              0, lport));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in,
                              TM_PFC_DEADLOCK_RECOVERY_STATEt_PFC_PRIf,
                              0, pri));
    /*
     * TRUE indicates the function called from PFC deadlock event handling,
     * update recovery field.
     */
    if (deadlock_hdlr == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in,
                                  TM_PFC_DEADLOCK_RECOVERY_STATEt_STATEf,
                                  0, state));
    } else {
        /* Check port ID. */
        if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport)) ||
                               (port_up == FALSE)) {
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in,
                    TM_PFC_DEADLOCK_RECOVERY_STATEt_OPERATIONAL_STATEf,
                    0, PORT_INFO_UNAVAIL_OP));

        } else {
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in,
                    TM_PFC_DEADLOCK_RECOVERY_STATEt_OPERATIONAL_STATEf,
                    0, VALID_OP));
        }
    }

    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, FALSE, ltid, &in));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, ltid, &in));
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

int
bcmtm_pfc_deadlock_recovery_manual_state_populate_imm_entry(int unit,
                                                     int lport,
                                                     uint32_t pri,
                                                     uint8_t state,
                                                     bool deadlock_hdlr,
                                                     bool port_up)
{
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    const bcmlrd_map_t *map = NULL;
    size_t num_fid;
    bcmltd_sid_t ltid = TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt;
    int pport;
    int rv = 0;
    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    rv = bcmlrd_map_get(unit, ltid, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, ltid, &num_fid));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &in));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    SHR_IF_ERR_EXIT
        (bcmtm_default_entry_array_construct(unit, ltid, &in));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in,
                              TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt_PORT_IDf,
                              0, lport));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in,
                              TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt_PFC_PRIf,
                              0, pri));
    /*
     * TRUE indicates the function called from PFC deadlock event handling,
     * update recovery field.
     */
    if (deadlock_hdlr == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in,
                                  TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt_STATEf,
                                  0, state));
    } else {
        /* Check port ID. */
        if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport)) ||
                               (port_up == FALSE)) {
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in,
                    TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt_OPERATIONAL_STATEf,
                    0, PORT_INFO_UNAVAIL_OP));

        } else {
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in,
                    TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt_OPERATIONAL_STATEf,
                    0, VALID_OP));
        }
    }

    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, FALSE, ltid, &in));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, ltid, &in));
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}



int
bcmtm_pfc_deadlock_recovery_status_populate_imm_entry(int unit ,
                                                      int lport,
                                                      uint32_t pri,
                                                      bool port_up)
{
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    const bcmlrd_map_t *map = NULL;
    size_t num_fid;
    bcmltd_sid_t ltid = TM_PFC_DEADLOCK_RECOVERY_STATUSt;
    int pport;
    int rv = 0;
    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    rv = bcmlrd_map_get(unit, ltid, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, ltid, &num_fid));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &in));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    in.count = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in,
                              TM_PFC_DEADLOCK_RECOVERY_STATUSt_PORT_IDf,
                              0, lport));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in,
                              TM_PFC_DEADLOCK_RECOVERY_STATUSt_PFC_PRIf,
                              0, pri));

    /* Check port ID. */
    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport)) ||
                           (port_up == FALSE)) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in,
                TM_PFC_DEADLOCK_RECOVERY_STATUSt_OPERATIONAL_STATEf,
                0, PORT_INFO_UNAVAIL_OP));

        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in,
                 TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf, 0, 0));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmtm_pfc_deadlock_config_set(unit,
                 TM_PFC_DEADLOCK_RECOVERY_STATUSt,
                 TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf,
                 lport, pri, 0, 0, 0));
    } else {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in,
                TM_PFC_DEADLOCK_RECOVERY_STATUSt_OPERATIONAL_STATEf,
                0, VALID_OP));
    }

    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, FALSE, ltid, &in));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, ltid, &in));
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief State machine updates the PFC pri deadlock recovery states.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_pfc_deadlock_recovery_sm(int unit, bcmtm_lport_t lport,
                               uint32_t pri,
                               bcmtm_pfc_deadlock_event_t event)
{
    bcmtm_pfc_ddr_ctrl_t *pfcctrl = bcmtm_pfc_ctrl[unit];
    bcmtm_pfc_ddr_drv_t *pfc_ddr_drv = NULL;
    uint32_t state;
    int current_state = -1;
    bcmtm_drv_t *drv;
    bcmtm_pfc_ddr_pri_info_t *priinfo;
    bcmltd_fields_t out_state, in_state;
    bcmltd_sid_t sid_state = TM_PFC_DEADLOCK_RECOVERY_STATEt;
    size_t num_fid;
    bcmltd_fid_t fid_state;
    uint64_t fval;
    bool in_state_alloc = 0, out_state_alloc = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pfcctrl, SHR_E_INIT);

    priinfo = &(pfcctrl->pri_info[lport][pri]);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    pfc_ddr_drv =  drv->pfc_ddr_drv_get(unit);
    SHR_NULL_CHECK(pfc_ddr_drv, SHR_E_PARAM);

    if (pfc_ddr_drv->hw_recovery_begin == NULL ||
        pfc_ddr_drv->hw_recovery_end == NULL)  {
        SHR_EXIT();
    }

    sal_memset(&in_state, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out_state, 0, sizeof(bcmltd_fields_t));

    /* Allocate in field list. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in_state));
    in_state_alloc = 1;

    /* Allocate out field list. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, TM_PFC_DEADLOCK_RECOVERY_STATEt,
                                &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid , &out_state));
    out_state_alloc = 1;

    state = priinfo->state;
    if ((event == BCMTM_PFC_EVENT_RECOVERY) && (state == IDLE)) {
        SHR_IF_ERR_EXIT
            (pfc_ddr_drv->hw_recovery_begin(unit, lport, pri, 1));
        priinfo->state = RECOVERY;

        /* Initialize the recovery count. */
        priinfo->recovery_count =
                priinfo->recovery_count_cfg + 1;
    } else if ((event == BCMTM_PFC_EVENT_IDLE) &&
                (state == RECOVERY)) {
        priinfo->recovery_count = 0;
        SHR_IF_ERR_EXIT
            (pfc_ddr_drv->hw_recovery_end(unit, lport, pri));
        priinfo->state = IDLE;
    } else if (event == BCMTM_PFC_EVENT_ON) {
        priinfo->state = IDLE;
    } else if (event == BCMTM_PFC_EVENT_OFF) {
        if (state == RECOVERY) {
            priinfo->recovery_count = 0;
            SHR_IF_ERR_EXIT
                (pfc_ddr_drv->hw_recovery_end(unit, lport, pri));
        }
        priinfo->state = IDLE;
    }

    /* Read the recovey state entry if it exists. */
    in_state.field[0]->data = lport;
    in_state.field[1]->data = pri;
    sid_state = TM_PFC_DEADLOCK_RECOVERY_STATEt;
    fid_state = TM_PFC_DEADLOCK_RECOVERY_STATEt_STATEf;
    in_state.field[0]->id   =
              TM_PFC_DEADLOCK_RECOVERY_STATEt_PORT_IDf;
    in_state.field[1]->id   =
              TM_PFC_DEADLOCK_RECOVERY_STATEt_PFC_PRIf;

    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid_state,
                                              &in_state, &out_state))) {
        if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit,
                                   fid_state, 0, *(out_state.field), &fval))) {
                        current_state = (uint32_t)fval;
        }
    }

    /* LT notification. */
    if (current_state != priinfo->state) {
        SHR_IF_ERR_EXIT
            (bcmtm_pfc_deadlock_recovery_state_populate_imm_entry(unit,
                                   lport,  pri, priinfo->state,
                                   TRUE, FALSE));
    }
exit:
    if (in_state_alloc) {
        bcmtm_field_list_free(&in_state);
    }
    if (out_state_alloc) {
        bcmtm_field_list_free(&out_state);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief State machine updates the PFC pri deadlock recovery states.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_pfc_deadlock_recovery_manual_sm(int unit, bcmtm_lport_t lport,
                                      uint32_t pri,
                                      bcmtm_pfc_deadlock_event_t event)
{
    bcmtm_pfc_ddr_ctrl_t *pfcctrl = bcmtm_pfc_ctrl[unit];
    bcmtm_pfc_ddr_drv_t *pfc_ddr_drv = NULL;
    bcmtm_pfc_ddr_pri_info_t *priinfo;
    uint32_t state;
    int current_state = -1;
    bcmtm_drv_t *drv;
    bcmltd_fields_t out_state, in_state;
    bcmltd_sid_t sid_state = TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt;
    size_t num_fid;
    bcmltd_fid_t fid_state;
    uint64_t fval;
    bool in_state_alloc = 0, out_state_alloc = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));

    pfc_ddr_drv =  drv->pfc_ddr_drv_get(unit);
    SHR_NULL_CHECK(pfc_ddr_drv, SHR_E_PARAM);
    if (pfc_ddr_drv->hw_recovery_begin == NULL ||
        pfc_ddr_drv->hw_recovery_end == NULL)  {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(pfcctrl, SHR_E_INIT);
    priinfo = &(pfcctrl->pri_info[lport][pri]);
    state = priinfo->state;

    sal_memset(&in_state, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out_state, 0, sizeof(bcmltd_fields_t));

    /* Allocate in field list. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in_state));
    in_state_alloc = 1;

    /* Allocate out field list. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid_state, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid , &out_state));
    out_state_alloc = 1;

    if ((event == BCMTM_PFC_EVENT_DEADLOCK) && (state == MANUAL_IDLE)) {
        SHR_IF_ERR_EXIT
            (pfc_ddr_drv->hw_recovery_begin(unit, lport, pri, 0));
        priinfo->state = MANUAL_DEADLOCK;

    } else if ((event == BCMTM_PFC_EVENT_RECOVERY) && (state == MANUAL_DEADLOCK)) {
        SHR_IF_ERR_EXIT
            (pfc_ddr_drv->hw_recovery_begin(unit, lport, pri, 1));
        priinfo->state = MANUAL_RECOVERY;
    } else if ((event == BCMTM_PFC_EVENT_IDLE) && (state == MANUAL_RECOVERY)) {
        SHR_IF_ERR_EXIT
            (pfc_ddr_drv->hw_recovery_end(unit, lport, pri));
        priinfo->state = MANUAL_IDLE;
    } else if (event == BCMTM_PFC_EVENT_ON) {
        priinfo->state = MANUAL_IDLE;
    } else if (event == BCMTM_PFC_EVENT_OFF) {
        if (state == MANUAL_RECOVERY) {
            SHR_IF_ERR_EXIT
                (pfc_ddr_drv->hw_recovery_end(unit, lport, pri));
        }
        priinfo->state = MANUAL_IDLE;
    }

    /* Read the recovey state entry if it exists. */
    in_state.field[0]->data = lport;
    in_state.field[1]->data = pri;
    sid_state = TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt;
    fid_state = TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt_STATEf;
    in_state.field[0]->id   =
              TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt_PORT_IDf;
    in_state.field[1]->id   =
              TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt_PFC_PRIf;

    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid_state,
                                              &in_state, &out_state))) {
        if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit,
                                   fid_state, 0, *(out_state.field), &fval))) {
                        current_state = (uint32_t)fval;
        }
    }

    /* LT notification. */
    if (current_state != priinfo->state) {
        SHR_IF_ERR_EXIT
            (bcmtm_pfc_deadlock_recovery_manual_state_populate_imm_entry(unit,
                                   lport,  pri, priinfo->state,
                                   TRUE, FALSE));
    }
exit:
    if (in_state_alloc) {
        bcmtm_field_list_free(&in_state);
    }
    if (out_state_alloc) {
        bcmtm_field_list_free(&out_state);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief  Function implementing software recovery timer.
 *
 * This function triggers the pfc dead recovery end process when the recovery time
 * period expires.
 *
 * \param [in] unit Unit number.
 * \param [in] lport device port number.
 * \param [in] pri PFC priority.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_pfc_deadlock_recovery_timer_refresh(int unit, bcmtm_lport_t lport, uint32_t pri)
{
    bcmtm_drv_t *drv;
    bcmtm_pfc_ddr_drv_t *pfc_ddr_drv = NULL;
    bcmtm_pfc_ddr_ctrl_t *pfcctrl = bcmtm_pfc_ctrl[unit];
    bcmtm_pfc_ddr_info_t *pfcinfo;
    bcmtm_pfc_ddr_pri_info_t *priinfo;
    int recovery_exited = 0;
    uint32_t mask = 0, status = 0;

    SHR_FUNC_ENTER(unit);

    bcmtm_pfc_ddr_info_get(unit, &pfcinfo);
    SHR_NULL_CHECK(pfcctrl, SHR_E_INIT);
    SHR_NULL_CHECK(pfcinfo, SHR_E_INIT);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    pfc_ddr_drv =  drv->pfc_ddr_drv_get(unit);
    SHR_NULL_CHECK(pfc_ddr_drv, SHR_E_PARAM);

    priinfo = &(pfcctrl->pri_info[lport][pri]);

    priinfo->recovery_count--;

    /* In case of warm booting while recovery is in progress. */
    if (priinfo->recovery_count < 0) {
        priinfo->recovery_count = 0;
    }

    if (priinfo->recovery_count == 0) {
        if (priinfo->recovery_mode ==
            BCMLTD_COMMON_TM_PFC_DEADLOCK_RECOVERY_MODE_T_T_PFC_STATE_MODE) {

            SHR_NULL_CHECK(pfc_ddr_drv->hw_status_get, SHR_E_PARAM);
            SHR_NULL_CHECK(pfc_ddr_drv->hw_status_clear, SHR_E_PARAM);

            SHR_IF_ERR_EXIT
                (pfc_ddr_drv->hw_status_get(unit, lport, &mask, &status));
            if ((status & (1U << pri)) == 0)  {
                recovery_exited = 1;
                SHR_IF_ERR_EXIT
                     (bcmtm_pfc_deadlock_recovery_sm(unit, lport,
                                          pri, BCMTM_PFC_EVENT_IDLE));
            } else {
                /* Re-initalize the recovery count to continue in recovery mode */
                priinfo->recovery_count =
                          priinfo->recovery_count_cfg + 1;
                /* Re-enable hardware timer and interrupt */
                SHR_IF_ERR_EXIT
                    (pfc_ddr_drv->hw_status_clear(unit, lport, pri));
            }
        } else {
            recovery_exited = 1;
            SHR_IF_ERR_EXIT
                 (bcmtm_pfc_deadlock_recovery_sm(unit, lport,
                                          pri, BCMTM_PFC_EVENT_IDLE));
        }
    }
    if (recovery_exited == 0) {
        /* bcmtm_pfc_info[lport].recovery_count is set to value + 1, for the reason
         * first time i.e when recovery is initiated this is called immediately,
         * and before PFC_SCAN_INTERVAL ticks.
         */
        pfcinfo[lport].recovery_count_acc[pri]++;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_pfc_deadlock_recovery_update_imm_entry(int unit ,
                                             int lport,
                                             uint32_t pri,
                                             uint32_t fid,
                                             uint32_t val)
{
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    const bcmlrd_map_t *map = NULL;
    size_t num_fid;
    bcmltd_sid_t ltid = TM_PFC_DEADLOCK_RECOVERYt;
    int rv = 0;
    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    rv = bcmlrd_map_get(unit, ltid, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, ltid, &num_fid));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &in));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    in.count = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in,
                              TM_PFC_DEADLOCK_RECOVERYt_PORT_IDf,
                              0, lport));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in,
                              TM_PFC_DEADLOCK_RECOVERYt_PFC_PRIf,
                              0, pri));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, val));

    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, FALSE, ltid, &in));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, ltid, &in));
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Handles deadlock status monitor from hardware.
 *
 * This function also triggers the pfc dead recovery start when a port,
 * pfc priority is declared to be in dead lock by the hardware.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_pfc_deadlock_process(int unit)
{
    int mport = -1;
    bcmtm_pport_t pport = 0;
    int lport = 0;
    bcmdrd_pbmp_t pbmp;
    bcmtm_pfc_ddr_info_t *pfcinfo;
    uint32_t fval_status = 0, fval_mask = 0, pri = 0;
    bcmtm_pfc_ddr_ctrl_t *pfcctrl = bcmtm_pfc_ctrl[unit];
    bcmtm_drv_t *drv;
    bcmtm_pfc_ddr_drv_t *pfc_ddr_drv = NULL;
    bcmtm_pfc_ddr_pri_info_t priinfo;


    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pfcctrl, SHR_E_INIT);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    pfc_ddr_drv =  drv->pfc_ddr_drv_get(unit);
    SHR_NULL_CHECK(pfc_ddr_drv, SHR_E_PARAM);

    if (pfc_ddr_drv->hw_status_get == NULL) {
        SHR_EXIT();
    }
    bcmtm_pfc_ddr_info_get(unit, &pfcinfo);
    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_ports_get(unit, &pbmp));
    BCMDRD_PBMP_ITER(pbmp, pport) {
        if (SHR_FAILURE(bcmtm_pport_lport_get(unit, pport, &lport))) {
            continue;
        }

        /* PFC DDR is supported on front panel and management ports. */
        if ((bcmtm_lport_is_lb(unit, lport)) ||
               (bcmtm_lport_is_cpu(unit, lport)) ||
               (bcmtm_lport_is_rdb(unit, lport))) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (bcmtm_lport_mport_get(unit, lport, &mport));

       /* Read the PT interrupt mask and status registers. */
       SHR_IF_ERR_EXIT
           (pfc_ddr_drv->hw_status_get(unit, lport,
                                       &fval_mask, &fval_status));


       /* If the interrupt is not masked,
        * means the recovery is not started and we are seeing the interrupt
        * for the first time.
        */
        fval_status = fval_status & (~fval_mask);
        if (fval_status != 0) {
            for (pri = 0; pri < MAX_NUM_PFC_PRI; pri++) {
                 priinfo = pfcctrl->pri_info[lport][pri];
                if ((fval_status & (1U << pri)) &&
                        (priinfo.ddr_enable)) {
                    if (priinfo.manual_oper == 1) {
                        SHR_IF_ERR_EXIT
                             (bcmtm_pfc_deadlock_recovery_manual_sm(unit, lport,
                                         pri, BCMTM_PFC_EVENT_DEADLOCK));
                    } else {
                        SHR_IF_ERR_EXIT
                             (bcmtm_pfc_deadlock_recovery_sm(unit, lport,
                                          pri, BCMTM_PFC_EVENT_RECOVERY));
                    }
                }
            }
        }

        /* Update the count for priority already in Deadlock state and reset
         * those priority where recovery timer has expired.
         */
        for (pri = 0; pri < MAX_NUM_PFC_PRI; pri++) {
            priinfo = pfcctrl->pri_info[lport][pri];
            if ( (!priinfo.manual_oper) &&
                      (priinfo.state == RECOVERY)) {
                 SHR_IF_ERR_EXIT
                     (bcmtm_pfc_deadlock_recovery_timer_refresh(unit, lport, pri));
            }

            if (priinfo.manual !=  priinfo.manual_oper) {
                if (priinfo.state == IDLE) {
                    if (priinfo.manual) {
                        priinfo.manual_oper = 1;
                        SHR_IF_ERR_EXIT
                            (bcmtm_pfc_deadlock_recovery_manual_sm(unit, lport,
                                  pri, BCMTM_PFC_EVENT_ON));
                        SHR_IF_ERR_EXIT
                            (bcmtm_pfc_deadlock_recovery_sm(unit, lport,
                                  pri, BCMTM_PFC_EVENT_OFF));
                        SHR_IF_ERR_EXIT
                            (bcmtm_pfc_deadlock_recovery_update_imm_entry(unit,
                                  lport, pri,
                                  TM_PFC_DEADLOCK_RECOVERYt_MANUAL_RECOVERY_OPERf,
                                  TRUE));
                    } else {
                        priinfo.manual_oper = 0;
                        SHR_IF_ERR_EXIT
                            (bcmtm_pfc_deadlock_recovery_sm(unit, lport,
                                         pri, BCMTM_PFC_EVENT_ON));
                        SHR_IF_ERR_EXIT
                            (bcmtm_pfc_deadlock_recovery_manual_sm(unit, lport,
                                         pri, BCMTM_PFC_EVENT_OFF));
                        SHR_IF_ERR_EXIT
                            (bcmtm_pfc_deadlock_recovery_update_imm_entry(unit,
                                  lport, pri,
                                  TM_PFC_DEADLOCK_RECOVERYt_MANUAL_RECOVERY_OPERf,
                                  FALSE));
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Deadlock detection monitor polling thread.
 *
 * \param [in] arg Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static void
bcmtm_pfc_ddr_scan_thread(shr_thread_t tc, void *arg)
{
    int unit = (unsigned long)(arg);
    bcmtm_pfc_ddr_ctrl_t *ddrc = bcmtm_pfc_ctrl[unit];
    int rv = SHR_E_NONE;

    while (ddrc->state == ACTIVE) {
        shr_thread_sleep(tc, PFC_SCAN_INTERVAL);
        PFC_DDR_INFO_LOCK(unit);

        rv = bcmtm_pfc_deadlock_process(unit);
        PFC_DDR_INFO_UNLOCK(unit);
        if (rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LS_BCMTM_IMM,
                          (BSL_META("Error in handling PFC deadlock .\n")));
        }
    }
}

/*!
 * \brief Function to initiate the dead lock monitor polling thread.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_pfc_deadlock_monitor_enable(int unit, bool enable)
{
    bcmtm_pfc_ddr_ctrl_t *ddrc = bcmtm_pfc_ctrl[unit];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ddrc, SHR_E_INIT);

    if (enable) {
        /* PFC DDR thread is already running. */
        if (ddrc->pid) {
            SHR_EXIT();
        }

        ddrc->state = ACTIVE;
        ddrc->pid = shr_thread_start("bcmtmPfcDdrScan",
                              SAL_THREAD_PRIO_DEFAULT,
                              bcmtm_pfc_ddr_scan_thread,
                              ((void *)(unsigned long)(unit)));
        if (ddrc->pid == NULL) {
            ddrc->state = INVALID;
            SHR_IF_ERR_EXIT(SHR_E_FAIL);
        }
    } else {
        if (ddrc->pid == NULL) {
            /* PFC DDR thread is already killed. */
            SHR_EXIT();
        }
        ddrc->state = SHUTDOWN;
        /* Wait for thread to exit. */
        SHR_IF_ERR_EXIT(
            shr_thread_stop(ddrc->pid, PFC_SCAN_INTERVAL*5));
        ddrc->pid = NULL;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Function to load the dead lock monitor configuration from IMM.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_pfc_deadlock_monitor_info_load(int unit)
{
    bcmltd_fields_t out, in, out_state, in_state;
    bcmltd_sid_t sid = TM_PFC_DEADLOCK_RECOVERYt;
    bcmltd_sid_t sid_state = TM_PFC_DEADLOCK_RECOVERY_STATEt;
    size_t num_fid;
    const bcmlrd_map_t *map = NULL;
    int lport, pri = 0;
    bcmlrd_field_def_t  field_def;
    int rv = 0;
    bcmtm_pfc_ddr_ctrl_t *pfcctrl = bcmtm_pfc_ctrl[unit];
    bcmtm_pfc_ddr_pri_info_t *priinfo;
    bcmltd_fid_t fid, fid_state;
    uint64_t fval;
    bool in_alloc = 0, out_alloc = 0;
    bool in_state_alloc = 0, out_state_alloc = 0;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERYt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERY_STATEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(pfcctrl, SHR_E_INIT);

    sal_memset(&field_def, 0, sizeof(field_def));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_def_get(unit,
                                    sid,
                                    TM_PFC_DEADLOCK_RECOVERYt_PORT_IDf,
                                    &field_def));

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));
    sal_memset(&in_state, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out_state, 0, sizeof(bcmltd_fields_t));

    /* Allocate in field list. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in));
    in_alloc = 1;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in_state));
    in_state_alloc = 1;

    /* Allocate out field list. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid , &out));
    out_alloc = 1;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, TM_PFC_DEADLOCK_RECOVERY_STATEt,
                                &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid , &out_state));
    out_state_alloc = 1;

    for (lport = field_def.min.u16; lport <= field_def.max.u16; lport++) {
        for (pri = 0; pri < MAX_NUM_PFC_PRI; pri++) {
            in.field[0]->id   = TM_PFC_DEADLOCK_RECOVERYt_PORT_IDf;
            in.field[0]->data = lport;
            in.field[1]->id   = TM_PFC_DEADLOCK_RECOVERYt_PFC_PRIf;
            in.field[1]->data = pri;

            /* Read the configuration entry if it exists. */
            sid = TM_PFC_DEADLOCK_RECOVERYt;
            if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid, &in, &out))) {
                priinfo = &(pfcctrl->pri_info[lport][pri]);

                fid = TM_PFC_DEADLOCK_RECOVERYt_DEADLOCK_RECOVERYf;
                if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        *(out.field), &fval))) {
                   priinfo->ddr_enable = (uint32_t)fval;
                }

                fid = TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_TIMERf;
                if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        *(out.field), &fval))) {
                   priinfo->recovery_count_cfg = (uint32_t)fval;
                }

                fid = TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_MODEf;
                if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        *(out.field), &fval))) {
                   priinfo->recovery_mode = (uint32_t)fval;
                }

                fid = TM_PFC_DEADLOCK_RECOVERYt_MANUAL_RECOVERYf;
                if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        *(out.field), &fval))) {
                   priinfo->manual = (uint32_t)fval;
                }

                fid = TM_PFC_DEADLOCK_RECOVERYt_MANUAL_RECOVERY_OPERf;
                if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        *(out.field), &fval))) {
                   priinfo->manual_oper = (uint32_t)fval;
                }

                if (priinfo->ddr_enable == 0) {
                    continue;
                }

                /* Read the recovey state entry if it exists. */
                in_state.field[0]->data = lport;
                in_state.field[1]->data = pri;
                if (priinfo->manual_oper) {
                    sid_state = TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt;
                    fid_state = TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt_STATEf;
                    in_state.field[0]->id   =
                                TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt_PORT_IDf;
                    in_state.field[1]->id   =
                                TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt_PFC_PRIf;
                } else {
                    sid_state = TM_PFC_DEADLOCK_RECOVERY_STATEt;
                    fid_state = TM_PFC_DEADLOCK_RECOVERY_STATEt_STATEf;
                    in_state.field[0]->id   =
                                TM_PFC_DEADLOCK_RECOVERY_STATEt_PORT_IDf;
                    in_state.field[1]->id   =
                                TM_PFC_DEADLOCK_RECOVERY_STATEt_PFC_PRIf;
                }
                if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid_state,
                                              &in_state, &out_state))) {
                    if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit,
                                   fid_state, 0, *(out_state.field), &fval))) {
                        priinfo->state = (uint32_t)fval;
                    }
                }
            }
        }
    }
exit:
    if (in_alloc) {
        bcmtm_field_list_free(&in);
    }
    if (in_state_alloc) {
        bcmtm_field_list_free(&in_state);
    }
    if (out_alloc) {
        bcmtm_field_list_free(&out);
    }
    if (out_state_alloc) {
        bcmtm_field_list_free(&out_state);
    }
    SHR_FUNC_EXIT();
}

static int
bcmtm_pfc_deadlock_config_set(int unit,
                          bcmltd_sid_t ltid,
                          uint32_t fid,
                          bcmtm_lport_t lport,
                          uint32_t pri,
                          uint32_t val,
                          uint32_t *val_oper,
                          bool reset)
{
    bcmtm_pfc_ddr_info_t *pfcinfo;
    uint32_t idx = 0, pri_idx = 0;
    uint32_t ddr_monitor = 0;
    bcmtm_drv_t *drv;
    bcmtm_pfc_ddr_drv_t *pfc_ddr_drv = NULL;
    uint8_t state = 0;
    int ddr_lock  = 0;
    bcmtm_pfc_ddr_ctrl_t *pfcctrl = bcmtm_pfc_ctrl[unit];

    SHR_FUNC_ENTER(unit);
    bcmtm_pfc_ddr_info_get(unit, &pfcinfo);
    SHR_NULL_CHECK(pfcinfo, SHR_E_INIT);

    SHR_NULL_CHECK(pfcctrl, SHR_E_INIT);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    pfc_ddr_drv =  drv->pfc_ddr_drv_get(unit);

    SHR_NULL_CHECK(pfc_ddr_drv, SHR_E_PARAM);

    SHR_NULL_CHECK(pfc_ddr_drv->pt_set, SHR_E_INIT);
    if (ltid == TM_PFC_DEADLOCK_RECOVERY_STATUSt) {
        if (fid == TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf) {
            PFC_DDR_INFO_LOCK(unit);
            pfcinfo[lport].recovery_count_acc[pri] = val;
            PFC_DDR_INFO_UNLOCK(unit);
        } else {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {

        if (fid == TM_PFC_DEADLOCK_RECOVERYt_DEADLOCK_RECOVERYf) {
            PFC_DDR_INFO_LOCK(unit);
            ddr_lock = 1;
            pfcctrl->pri_info[lport][pri].ddr_enable = val;
            if (val == 1) {
                ddr_monitor = 1;
            } else {
                ddr_monitor = 0;
                for (idx = 0; idx < BCMDRD_CONFIG_MAX_PORTS; idx++) {
                    for (pri_idx = 0; pri_idx < MAX_NUM_PFC_PRI; pri_idx++) {
                        if (pfcctrl->pri_info[idx][pri_idx].ddr_enable != 0) {
                            ddr_monitor = 1;
                            break;
                        }
                    }
                }
            }
            PFC_DDR_INFO_UNLOCK(unit);
            ddr_lock = 0;
            SHR_IF_ERR_EXIT
                 (bcmtm_pfc_deadlock_monitor_enable(unit,
                                            (ddr_monitor == 1) ? true : false));
            PFC_DDR_INFO_LOCK(unit);
            ddr_lock = 1;
            if (val == 0) {
                if (pfcctrl->pri_info[lport][pri].manual_oper) {
                    SHR_IF_ERR_EXIT
                         (bcmtm_pfc_deadlock_recovery_manual_sm(unit, lport,
                                            pri, BCMTM_PFC_EVENT_OFF));
                } else {
                    SHR_IF_ERR_EXIT
                         (bcmtm_pfc_deadlock_recovery_sm(unit, lport,
                                            pri, BCMTM_PFC_EVENT_OFF));
                }
            } else {
                if (pfcctrl->pri_info[lport][pri].manual_oper) {
                    SHR_IF_ERR_EXIT
                        (bcmtm_pfc_deadlock_recovery_manual_sm(unit, lport,
                                            pri, BCMTM_PFC_EVENT_ON));
                } else {
                    SHR_IF_ERR_EXIT
                        (bcmtm_pfc_deadlock_recovery_sm(unit, lport,
                                            pri, BCMTM_PFC_EVENT_ON));
                }
            }
            PFC_DDR_INFO_UNLOCK(unit);
            ddr_lock = 0;
            SHR_IF_ERR_EXIT
                 (pfc_ddr_drv->pt_set(unit, fid, lport, pri, val));
        } else if (fid == TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_TIMERf) {
             PFC_DDR_INFO_LOCK(unit);
             pfcctrl->pri_info[lport][pri].recovery_count_cfg = val;
             PFC_DDR_INFO_UNLOCK(unit);
        } else if (fid == TM_PFC_DEADLOCK_RECOVERYt_DETECTION_TIMER_GRANULARITYf) {
             SHR_IF_ERR_EXIT
                 (pfc_ddr_drv->pt_set(unit, fid, lport, pri, val));
        } else if (fid == TM_PFC_DEADLOCK_RECOVERYt_DETECTION_TIMERf) {
            SHR_IF_ERR_EXIT
                (pfc_ddr_drv->pt_set(unit, fid, lport, pri, val));
        } else if (fid == TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_MODEf) {
            PFC_DDR_INFO_LOCK(unit);
            pfcctrl->pri_info[lport][pri].recovery_mode = val;
            PFC_DDR_INFO_UNLOCK(unit);
        } else if (fid == TM_PFC_DEADLOCK_RECOVERYt_MANUAL_RECOVERYf) {
            PFC_DDR_INFO_LOCK(unit);
            state =  pfcctrl->pri_info[lport][pri].state;
            if ((state == IDLE) || (reset == true)) {
                pfcctrl->pri_info[lport][pri].manual_oper = val;
                 if (val_oper) {
                     *val_oper = val;
                 }
            }
            pfcctrl->pri_info[lport][pri].manual = val;
            PFC_DDR_INFO_UNLOCK(unit);
        } else {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    }
exit:
    if (ddr_lock == 1) {
        PFC_DDR_INFO_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Resets the PFC Egress port physical table to default values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_pfc_deadlock_pt_reset(int unit,
                            bcmltd_sid_t ltid,
                            bcmtm_lport_t lport,
                            uint32_t pfc_pri)
{
    int mport;
    bcmltd_fid_t fid[] = {
        TM_PFC_DEADLOCK_RECOVERYt_DEADLOCK_RECOVERYf,
        TM_PFC_DEADLOCK_RECOVERYt_DETECTION_TIMERf,
        TM_PFC_DEADLOCK_RECOVERYt_DETECTION_TIMER_GRANULARITYf,
        TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_TIMERf,
        TM_PFC_DEADLOCK_RECOVERYt_MANUAL_RECOVERYf,
        TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_MODEf,
    };
    const bcmlrd_field_data_t  *field;
    int count;

    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
        SHR_EXIT();
    }

    /* Physical table update. */
    for (count = 0; count < COUNTOF(fid); count++) {
        if (SHR_E_UNAVAIL == bcmlrd_field_get(unit, ltid, fid[count], &field)) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (bcmtm_pfc_deadlock_config_set(unit, ltid, fid[count],
                                           lport, pfc_pri, 0, 0, 0));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  PFC deadlock recovery status function.
 *
 * \param [in]  unit  Unit number.
 * \param [in]  fid Logical table field ID.
 * \param [in]  lport Logical port number.
 * \param [in]  pfc_pri PFC priority
 * \param [out] val Logical table field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_pfc_deadlock_recovery_status_get(int unit,
                                       uint32_t fid,
                                       bcmtm_lport_t lport,
                                       uint32_t pfc_pri,
                                       uint32_t *val)
{
    bcmtm_pfc_ddr_info_t *pfcinfo;

    SHR_FUNC_ENTER(unit);

    bcmtm_pfc_ddr_info_get(unit, &pfcinfo);
    SHR_NULL_CHECK(pfcinfo, SHR_E_INIT);

    if (fid == TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf) {
        *val = pfcinfo[lport].recovery_count_acc[pfc_pri];
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_PFC_DEADLOCK_RECOVERY logical table callback function for staging.
 *
 * Handle TM_PFC_DEADLOCK_RECOVERY IMM table events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id LT Transaction Id
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] data. This is a linked list of extra added data fields (mostly
 * read only fields).
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmtm_pfc_ddr_stage(int unit,
                    bcmltd_sid_t ltid,
                    uint32_t trans_id,
                    bcmimm_entry_event_t event_reason,
                    const bcmltd_field_t *key,
                    const bcmltd_field_t *data,
                    void *context,
                    bcmltd_fields_t *out_fields)
{
    bcmtm_lport_t lport;
    bcmltd_fid_t fid;
    uint64_t fval = 0;
    uint32_t pfc_pri = 0, fval_oper = 0;
    int32_t mport = 0, count = 0;
    bcmltd_fid_t fid_list[] = {
        TM_PFC_DEADLOCK_RECOVERYt_DETECTION_TIMERf,
        TM_PFC_DEADLOCK_RECOVERYt_DETECTION_TIMER_GRANULARITYf,
        TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_TIMERf,
        TM_PFC_DEADLOCK_RECOVERYt_MANUAL_RECOVERYf,
        TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_MODEf,
        TM_PFC_DEADLOCK_RECOVERYt_DEADLOCK_RECOVERYf,
    };
    const bcmlrd_field_data_t  *field;

    SHR_FUNC_ENTER(unit);

    /* TM_PFC_DEADLOCK_RECOVERYt_PORT_IDf */
    fid = TM_PFC_DEADLOCK_RECOVERYt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (bcmtm_lport_t)fval;

    /* PFC DDR is supported on front panel and management ports. */
    if (bcmtm_lport_is_lb(unit, lport) || bcmtm_lport_is_rdb(unit, lport)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* TM_PFC_DEADLOCK_RECOVERYt_PFC_PRIf */
    fid = TM_PFC_DEADLOCK_RECOVERYt_PFC_PRIf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    pfc_pri = (uint32_t)fval;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(out_fields, SHR_E_PARAM);

            /* Initialize output_fields. */
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_size_check(unit, out_fields, 2));
            out_fields->count = 0;

            if (SHR_FAILURE(bcmtm_lport_mport_get(unit, lport,
                                                    &mport))) {
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out_fields,
                                 TM_PFC_DEADLOCK_RECOVERYt_OPERATIONAL_STATEf,
                                 0, PORT_INFO_UNAVAIL_OP));
                SHR_EXIT();
            } else if (!data) {
                /*
                 * Data field not present in the insert command.
                 * Initialize the entry and return.
                 */
                out_fields->count = 0;
                return SHR_E_NONE;
            } else {
                for (count = 0; count < COUNTOF(fid_list); count++) {
                    if (SHR_E_UNAVAIL == bcmlrd_field_get(unit, ltid,
                                    fid_list[count], &field)) {
                        continue;
                    }

                    if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit,
                                    fid_list[count], 0, data, &fval))) {
                        SHR_IF_ERR_EXIT
                            (bcmtm_pfc_deadlock_config_set(unit, ltid, fid_list[count],
                                         lport, pfc_pri, (uint32_t)fval,
                                         (fid_list[count] ==
                                            GETLTFIELD(MANUAL_RECOVERY)) ?
                                         &fval_oper : 0, false));

                        if (fid_list[count] == GETLTFIELD(MANUAL_RECOVERY)) {
                            SHR_IF_ERR_EXIT
                                (bcmtm_field_list_set(unit, out_fields,
                                          GETLTFIELD(MANUAL_RECOVERY_OPER),
                                          0, fval_oper));
                        }
                    }
                }
             }
             SHR_IF_ERR_EXIT
                 (bcmtm_field_list_set(unit, out_fields,
                                GETLTFIELD(OPERATIONAL_STATE),
                                 0, VALID_OP));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmtm_pfc_deadlock_pt_reset(unit, ltid, lport, pfc_pri));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * In-memory PFC_DEADLOCK_RECOVERY_STATUS lookup callback helper function.
 */
static int
bcmtm_pfc_ddr_status_lookup (int unit,
                             bcmltd_sid_t sid,
                             uint32_t trans_id,
                             void *context,
                             bcmimm_lookup_type_t lkup_type,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out)
{
    const bcmltd_fields_t *key = NULL;
    bcmtm_lport_t lport = 0;
    uint32_t val = 0, i = 0, pri = 0;

    SHR_FUNC_ENTER(unit);

    /* key fields are in the "out" parameter for traverse operations */
    key = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    if (key) {
        for (i = 0; i < key->count; i++) {
            /* Get the keys value. */
            if (key->field[i]->id == TM_PFC_DEADLOCK_RECOVERY_STATUSt_PORT_IDf) {
                lport = key->field[i]->data;
            } else if ( key->field[i]->id == TM_PFC_DEADLOCK_RECOVERY_STATUSt_PFC_PRIf) {
                pri =  key->field[i]->data;
            }
        }
    }
    if (out) {
        for (i = 0; i < out->count; i++) {
           /* Set RECOVERY_CNT value. */
           if (out->field[i]->id == TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf) {

               SHR_IF_ERR_VERBOSE_EXIT
                   (bcmtm_pfc_deadlock_recovery_status_get(unit,
                                           TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf,
                                           lport, pri, &val));
               out->field[i]->data = val;
               break;
           }
       }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief In-memory logical table staging callback function.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key_fields This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data_field This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [out] out_fields This output parameter can be used by the component
 * to add fields into the entry. Typically this should be used for read-only
 * type fields that otherwise can not be set by the application.
 * The component must set the count field of the \c out_fields parameter to
 * indicate the actual number of fields that were set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmtm_pfc_ddr_status_stage(int unit,
                         bcmltd_sid_t sid,
                         uint32_t trans_id,
                         bcmimm_entry_event_t event_reason,
                         const bcmltd_field_t *key_fields,
                         const bcmltd_field_t *data_fields,
                         void *context,
                         bcmltd_fields_t *out_fields)
{
    uint32_t pri, cnt;
    int32_t  mport;
    bcmltd_fid_t ltfid;
    bcmtm_lport_t lport;
    uint64_t fval;
    uint8_t opcode;
    SHR_FUNC_ENTER(unit);

    ltfid = TM_PFC_DEADLOCK_RECOVERY_STATUSt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, ltfid, 0, key_fields,
                                         &fval));
    lport = (bcmtm_lport_t) fval;

    ltfid = TM_PFC_DEADLOCK_RECOVERY_STATUSt_PFC_PRIf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, ltfid, 0, key_fields,
                                         &fval));
    pri = (int)fval;


    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:

            SHR_NULL_CHECK(out_fields, SHR_E_PARAM);

            /* Parse data field for input entry. */
            if (!data_fields) {
                /*
                 * Data field not present in the insert command.
                 * Initialize the entry and return.
                 */
                out_fields->count = 0;
                return SHR_E_NONE;
            }

            out_fields->count = 0;

            /* TM local port information.
             * Returns error if local port to physical port map is not available.
             * Updates opeartional state.
             */
            if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
                opcode = PORT_INFO_UNAVAIL_OP;
                ltfid = TM_PFC_DEADLOCK_RECOVERY_STATUSt_OPERATIONAL_STATEf;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out_fields, ltfid, 0, opcode));
                SHR_EXIT();
            }

            ltfid = TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf;
            SHR_IF_ERR_EXIT
               (bcmtm_fval_get_from_field_array(unit, ltfid, 0, data_fields,
                                                &fval));
            cnt = (uint32_t)fval;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmtm_pfc_deadlock_config_set(unit, sid,
                           TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf,
                           lport,
                           pri,
                           cnt,
                           0, 0));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_PFC_DEADLOCK_RECOVERY_STATE_CONTROL logical table callback function for staging.
 *
 * Handle TM_PFC_DEADLOCK_RECOVERY_STATE_CONTROL IMM table events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id LT Transaction Id
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] data. This is a linked list of extra added data fields (mostly
 * read only fields).
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmtm_pfc_ddr_state_control_stage(int unit,
                    bcmltd_sid_t ltid,
                    uint32_t trans_id,
                    bcmimm_entry_event_t event_reason,
                    const bcmltd_field_t *key,
                    const bcmltd_field_t *data,
                    void *context,
                    bcmltd_fields_t *out_fields)
{
    bcmtm_lport_t lport;
    bcmltd_fid_t fid;
    uint64_t fval = 0;
    uint32_t pri = 0;
    int32_t mport = 0;
    bcmtm_pfc_ddr_info_t *pfcinfo;
    bcmtm_pfc_ddr_drv_t *pfc_ddr_drv = NULL;
    bcmtm_drv_t *drv;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));

    bcmtm_pfc_ddr_info_get(unit, &pfcinfo);
    SHR_NULL_CHECK(pfcinfo, SHR_E_INIT);
    pfc_ddr_drv =  drv->pfc_ddr_drv_get(unit);
    SHR_NULL_CHECK(pfc_ddr_drv, SHR_E_PARAM);

    /* TM_PFC_DEADLOCK_RECOVERY_STATE_CONTROLt_PORT_IDf */
    fid = TM_PFC_DEADLOCK_RECOVERY_STATE_CONTROLt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (bcmtm_lport_t)fval;

    /* PFC DDR is supported on front panel and management ports. */
    if (bcmtm_lport_is_lb(unit, lport) || bcmtm_lport_is_rdb(unit, lport)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* TM_PFC_DEADLOCK_RECOVERY_STATE_CONTROLt_PFC_PRIf */
    fid = TM_PFC_DEADLOCK_RECOVERY_STATE_CONTROLt_PFC_PRIf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    pri = (uint32_t)fval;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(out_fields, SHR_E_PARAM);

            /* Initialize output_fields. */
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_size_check(unit, out_fields, 1));
            out_fields->count = 0;


            if (SHR_FAILURE(bcmtm_lport_mport_get(unit, lport,
                                                    &mport))) {
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out_fields,
                                 TM_PFC_DEADLOCK_RECOVERY_STATE_CONTROLt_OPERATIONAL_STATEf,
                                 0, PORT_INFO_UNAVAIL_OP));
                SHR_EXIT();

            } else if (!data) {
                /*
                 * Data field not present in the insert command.
                 * Initialize the entry and return.
                 */
                out_fields->count = 0;
                return SHR_E_NONE;
            } else {
                fid = TM_PFC_DEADLOCK_RECOVERY_STATE_CONTROLt_STATEf;
                if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                                    data, &fval))) {
                    /* Terminate the recovery in progress. */
                    PFC_DDR_INFO_LOCK(unit);
                    if (fval == IDLE_CONTROL) {
                        rv = bcmtm_pfc_deadlock_recovery_sm(unit, lport,
                                            pri, BCMTM_PFC_EVENT_IDLE);
                    } else {
                        SHR_EXIT();
                    }
                    if (rv != SHR_E_NONE) {
                        PFC_DDR_INFO_UNLOCK(unit);
                        SHR_ERR_EXIT(rv);
                    }

                    PFC_DDR_INFO_UNLOCK(unit);
                    SHR_IF_ERR_EXIT
                         (bcmtm_field_list_set(unit, out_fields,
                          TM_PFC_DEADLOCK_RECOVERY_STATE_CONTROLt_OPERATIONAL_STATEf,
                                           0, VALID_OP));
                }
            }
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_CONTROL logical table callback function for staging.
 *
 * Handle TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_CONTROL IMM table events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id LT Transaction Id
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] data. This is a linked list of extra added data fields (mostly
 * read only fields).
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmtm_pfc_ddr_manual_state_control_stage(int unit,
                    bcmltd_sid_t ltid,
                    uint32_t trans_id,
                    bcmimm_entry_event_t event_reason,
                    const bcmltd_field_t *key,
                    const bcmltd_field_t *data,
                    void *context,
                    bcmltd_fields_t *out_fields)
{
    bcmtm_lport_t lport;
    bcmltd_fid_t fid;
    uint64_t fval = 0;
    uint32_t pri = 0;
    int32_t mport = 0;
    bcmtm_pfc_ddr_info_t *pfcinfo;
    bcmtm_pfc_ddr_drv_t *pfc_ddr_drv = NULL;
    bcmtm_drv_t *drv;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));

    bcmtm_pfc_ddr_info_get(unit, &pfcinfo);
    SHR_NULL_CHECK(pfcinfo, SHR_E_INIT);
    pfc_ddr_drv =  drv->pfc_ddr_drv_get(unit);
    SHR_NULL_CHECK(pfc_ddr_drv, SHR_E_PARAM);

    /* TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_CONTROLt_PORT_IDf */
    fid = TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_CONTROLt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (bcmtm_lport_t)fval;

    /* PFC DDR is supported on front panel and management ports. */
    if (bcmtm_lport_is_lb(unit, lport) || bcmtm_lport_is_rdb(unit, lport)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_CONTROLt_PFC_PRIf */
    fid = TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_CONTROLt_PFC_PRIf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    pri = (uint32_t)fval;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(out_fields, SHR_E_PARAM);

            /* Initialize output_fields. */
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_size_check(unit, out_fields, 1));
            out_fields->count = 0;


            if (SHR_FAILURE(bcmtm_lport_mport_get(unit, lport,
                                                    &mport))) {
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out_fields,
                                 TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_CONTROLt_OPERATIONAL_STATEf,
                                 0, PORT_INFO_UNAVAIL_OP));
                SHR_EXIT();

            } else if (!data) {
                /*
                 * Data field not present in the insert command.
                 * Initialize the entry and return.
                 */
                out_fields->count = 0;
                return SHR_E_NONE;
            } else {
                fid = TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_CONTROLt_STATEf;
                if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                                    data, &fval))) {
                    /* Terminate the recovery in progress. */
                    PFC_DDR_INFO_LOCK(unit);
                    if (fval == MANUAL_IDLE_CONTROL) {
                        rv = bcmtm_pfc_deadlock_recovery_manual_sm(unit, lport,
                                            pri, BCMTM_PFC_EVENT_IDLE);
                    } else {
                        rv = bcmtm_pfc_deadlock_recovery_manual_sm(unit, lport,
                                         pri, BCMTM_PFC_EVENT_RECOVERY);
                    }
                    if (rv != SHR_E_NONE) {
                        PFC_DDR_INFO_UNLOCK(unit);
                        SHR_ERR_EXIT(rv);
                    }

                    PFC_DDR_INFO_UNLOCK(unit);
                    SHR_IF_ERR_EXIT
                         (bcmtm_field_list_set(unit, out_fields,
                          TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_CONTROLt_OPERATIONAL_STATEf,
                                           0, VALID_OP));
                }
            }
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_pfc_deadlock_imm_register(int unit)
{
    bcmimm_lt_cb_t tm_cb_handler;
    const bcmlrd_map_t *map = NULL;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    /*
     * Assign callback functions to IMM event callback handler.
     * commit and abort functions can be added if necessary.
     */

    sal_memset(&tm_cb_handler, 0, sizeof(tm_cb_handler));
    tm_cb_handler.stage    = bcmtm_pfc_ddr_stage;

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERYt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_PFC_DEADLOCK_RECOVERYt, &tm_cb_handler, NULL));

    /*
     * Assign callback functions to IMM event callback handler.
     * commit and abort functions can be added if necessary.
     */
    sal_memset(&tm_cb_handler, 0, sizeof(tm_cb_handler));
    tm_cb_handler.stage = bcmtm_pfc_ddr_status_stage;
    tm_cb_handler.lookup = bcmtm_pfc_ddr_status_lookup;

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERY_STATUSt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_PFC_DEADLOCK_RECOVERY_STATUSt,
                             &tm_cb_handler, NULL));

    /*
     * Assign callback functions to IMM event callback handler.
     * commit and abort functions can be added if necessary.
     */
    sal_memset(&tm_cb_handler, 0, sizeof(tm_cb_handler));
    tm_cb_handler.stage = bcmtm_pfc_ddr_state_control_stage;

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERY_STATE_CONTROLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_PFC_DEADLOCK_RECOVERY_STATE_CONTROLt,
                             &tm_cb_handler, NULL));

   /*
     * Assign callback functions to IMM event callback handler.
     * commit and abort functions can be added if necessary.
     */
    sal_memset(&tm_cb_handler, 0, sizeof(tm_cb_handler));
    tm_cb_handler.stage = bcmtm_pfc_ddr_manual_state_control_stage;

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_CONTROLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATE_CONTROLt,
                             &tm_cb_handler, NULL));
exit:
    SHR_FUNC_EXIT();
}


int
bcmtm_pfc_deadlock_recovery_status_imm_populate(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int lport, pri = 0;
    bcmltd_sid_t ltid = TM_PFC_DEADLOCK_RECOVERY_STATUSt;
    bcmlrd_field_def_t  field_def;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERY_STATUSt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    sal_memset(&field_def, 0, sizeof(field_def));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_def_get(unit,
                                    ltid,
                                    TM_PFC_DEADLOCK_RECOVERY_STATUSt_PORT_IDf,
                                    &field_def));

    for (lport = field_def.min.u16; lport <= field_def.max.u16; lport++) {
        if ((bcmtm_lport_is_fp(unit, (bcmtm_lport_t )lport)) ||
            (bcmtm_lport_is_mgmt(unit, (bcmtm_lport_t) lport))) {
            for (pri = 0; pri < MAX_NUM_PFC_PRI; pri++) {
                SHR_IF_ERR_EXIT
                    (bcmtm_pfc_deadlock_recovery_status_populate_imm_entry(unit,
                                                    (bcmtm_lport_t )lport, pri,
                                                     TRUE));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_pfc_deadlock_recovery_state_imm_populate(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int lport, pri = 0;
    bcmltd_sid_t ltid = TM_PFC_DEADLOCK_RECOVERY_STATEt;
    bcmlrd_field_def_t  field_def;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERY_STATEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    sal_memset(&field_def, 0, sizeof(field_def));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_def_get(unit,
                                    ltid,
                                    TM_PFC_DEADLOCK_RECOVERY_STATEt_PORT_IDf,
                                    &field_def));

    for (lport = field_def.min.u16; lport <= field_def.max.u16; lport++) {
        if ((bcmtm_lport_is_fp(unit, (bcmtm_lport_t )lport)) ||
            (bcmtm_lport_is_mgmt(unit, (bcmtm_lport_t) lport))) {
            for (pri = 0; pri < MAX_NUM_PFC_PRI; pri++) {
                SHR_IF_ERR_EXIT
                    (bcmtm_pfc_deadlock_recovery_state_populate_imm_entry(unit,
                                    (bcmtm_lport_t )lport, pri, IDLE,
                                     FALSE, TRUE));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_pfc_deadlock_recovery_manual_state_imm_populate(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int lport, pri = 0;
    bcmltd_sid_t ltid = TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt;
    bcmlrd_field_def_t  field_def;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    sal_memset(&field_def, 0, sizeof(field_def));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_def_get(unit,
                                    ltid,
                                    TM_PFC_DEADLOCK_RECOVERY_MANUAL_STATEt_PORT_IDf,
                                    &field_def));

    for (lport = field_def.min.u16; lport <= field_def.max.u16; lport++) {
        if ((bcmtm_lport_is_fp(unit, (bcmtm_lport_t )lport)) ||
            (bcmtm_lport_is_mgmt(unit, (bcmtm_lport_t) lport))) {
            for (pri = 0; pri < MAX_NUM_PFC_PRI; pri++) {
                SHR_IF_ERR_EXIT
                    (bcmtm_pfc_deadlock_recovery_manual_state_populate_imm_entry(unit,
                                    (bcmtm_lport_t )lport, pri, IDLE,
                                     FALSE, TRUE));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}


int
bcmtm_pfc_deadlock_monitor_cleanup(int unit)
{
    bcmtm_pfc_ddr_ctrl_t *pfcctrl = bcmtm_pfc_ctrl[unit];
    const bcmlrd_map_t *map = NULL;
    int rv;
    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERYt, &map);

    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(pfcctrl, SHR_E_INIT);

    if (pfcctrl->info_lock) {
        sal_mutex_destroy(pfcctrl->info_lock);
    }

    sal_free(pfcctrl);
    bcmtm_pfc_ctrl[unit] = NULL;
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_pfc_deadlock_monitor_stop(int unit)
{
    bcmtm_pfc_ddr_ctrl_t *ddrc = bcmtm_pfc_ctrl[unit];
    const bcmlrd_map_t *map = NULL;
    int rv;
    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERYt, &map);

    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(ddrc, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (bcmtm_pfc_deadlock_monitor_enable(unit, false));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_pfc_deadlock_monitor_init(int unit, bool warm)
{
    bcmtm_pfc_ddr_ctrl_t *pfcctrl = NULL;
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERYt, &map);

    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    pfcctrl = sal_alloc(sizeof(bcmtm_pfc_ddr_ctrl_t), "bcmtmPfcCtrl");

    if (!pfcctrl) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    sal_memset(pfcctrl, 0, sizeof(bcmtm_pfc_ddr_ctrl_t));

    pfcctrl->state = INIT;

    pfcctrl->info_lock = sal_mutex_create("bcmcthPfcDdrInfoLock");
    if (pfcctrl->info_lock == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    bcmtm_pfc_ctrl[unit] = pfcctrl;
exit:
    if (SHR_FUNC_ERR()) {
        if (pfcctrl && pfcctrl->info_lock) {
            sal_mutex_destroy(pfcctrl->info_lock);
            pfcctrl->info_lock = NULL;
            sal_free(pfcctrl);
        }

        bcmtm_pfc_ctrl[unit] = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcmtm_pfc_deadlock_recovery_populate_imm_entry(int unit ,
                                               int lport,
                                               uint32_t pri,
                                               bool up)
{
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    const bcmlrd_map_t *map = NULL;
    size_t num_fid;
    bcmltd_sid_t ltid = TM_PFC_DEADLOCK_RECOVERYt;
    int pport, count = 0;
    int rv = 0;
    bcmltd_fid_t fid;
    uint64_t fval;
    uint32_t val;
    bcmltd_fid_t fid_list[] = {
        TM_PFC_DEADLOCK_RECOVERYt_DETECTION_TIMERf,
        TM_PFC_DEADLOCK_RECOVERYt_DETECTION_TIMER_GRANULARITYf,
        TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_TIMERf,
        TM_PFC_DEADLOCK_RECOVERYt_MANUAL_RECOVERYf,
        TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_MODEf,
        TM_PFC_DEADLOCK_RECOVERYt_DEADLOCK_RECOVERYf,
    };
    bcmltd_fid_t reset_fid_list[] = {
        TM_PFC_DEADLOCK_RECOVERYt_DEADLOCK_RECOVERYf,
        TM_PFC_DEADLOCK_RECOVERYt_DETECTION_TIMERf,
        TM_PFC_DEADLOCK_RECOVERYt_DETECTION_TIMER_GRANULARITYf,
        TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_TIMERf,
        TM_PFC_DEADLOCK_RECOVERYt_MANUAL_RECOVERYf,
        TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_MODEf,
    };


    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    rv = bcmlrd_map_get(unit, ltid, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* PFC DDR is supported on front panel and management ports. */
    if (bcmtm_lport_is_lb(unit, lport) || bcmtm_lport_is_rdb(unit, lport)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, ltid, &num_fid));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &in));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    in.count = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in,
                              TM_PFC_DEADLOCK_RECOVERYt_PORT_IDf,
                              0, lport));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in,
                              TM_PFC_DEADLOCK_RECOVERYt_PFC_PRIf,
                              0, pri));

    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport)) ||
                               (up == FALSE)) {
            /* Port delete. */
            for (count = 0; count < COUNTOF(reset_fid_list); count++) {
                fid = reset_fid_list[count];
                if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        *(out.field), &fval))) {
                    SHR_IF_ERR_EXIT
                        (bcmtm_pfc_deadlock_config_set(unit,
                                                       ltid,
                                                       fid,
                                                       lport,
                                                       pri,
                                                       0, 0, true));
                }
            }
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in,
                       TM_PFC_DEADLOCK_RECOVERYt_OPERATIONAL_STATEf,
                       0, PORT_INFO_UNAVAIL_OP));

            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in,
                       TM_PFC_DEADLOCK_RECOVERYt_MANUAL_RECOVERY_OPERf,
                       0, 0));
       } else {
           /* Port add .*/
           for (count = 0; count < COUNTOF(fid_list); count++) {
                fid = fid_list[count];
                if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        *(out.field), &fval))) {
                    val = (uint32_t)fval;
                    SHR_IF_ERR_EXIT
                        (bcmtm_pfc_deadlock_config_set(unit,
                                                       ltid,
                                                       fid,
                                                       lport,
                                                       pri,
                                                       val,
                                                       0, false));
                    if (fid == TM_PFC_DEADLOCK_RECOVERYt_MANUAL_RECOVERYf) {
                        SHR_IF_ERR_EXIT
                            (bcmtm_field_list_set(unit, &in,
                                    TM_PFC_DEADLOCK_RECOVERYt_MANUAL_RECOVERY_OPERf,
                                    0, val));
                    }
                }
            }
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in,
                          TM_PFC_DEADLOCK_RECOVERYt_OPERATIONAL_STATEf,
                          0, 0));
       }
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, FALSE, ltid, &in));
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Function to start dead lock monitor polling thread.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot mode.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmtm_pfc_deadlock_monitor_run(int unit, bool warm)
{
    bcmtm_pfc_ddr_ctrl_t *ddrc = bcmtm_pfc_ctrl[unit];
    const bcmlrd_map_t *map = NULL;
    int lport = 0;
    int rv = 0, pri = 0;
    bcmtm_pport_t pport;
    bcmdrd_pbmp_t pbmp;


    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERYt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    SHR_NULL_CHECK(ddrc, SHR_E_INIT);

    if (warm) {
        SHR_IF_ERR_EXIT
            (bcmdrd_dev_valid_ports_get(unit, &pbmp));
        SHR_IF_ERR_EXIT
            (bcmtm_pfc_deadlock_monitor_info_load(unit));
        BCMDRD_PBMP_ITER(pbmp, pport) {
            if (SHR_SUCCESS(bcmtm_pport_lport_get(unit, pport, &lport))) {
                for (pri = 0; pri < MAX_NUM_PFC_PRI; pri++) {
                     if (ddrc->pri_info[lport][pri].ddr_enable != 0) {
                         bcmtm_pfc_deadlock_monitor_enable(unit, true);
                         break;
                     }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

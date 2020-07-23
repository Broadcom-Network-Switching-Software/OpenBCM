/*! \file link.c
 *
 * Linkscan Emulator
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcm/link.h>
#include <bcm/port.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/event_mgr.h>
#include <bcm_int/ltsw/link.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/port.h>

#include <shr/shr_debug.h>
#include <shr/shr_timeout.h>
#include <sal/sal_mutex.h>
#include <shr/shr_bitop.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCM_LINK

/* HA Sub component ID for bitmap of ports to skip during linkdown. */
#define BCMINT_LTSW_LINK_HA_SKIP_PBM 0

/* Linkscan mode. */
typedef enum ls_mode_e {
    LS_NO_SCAN = 0,
    LS_SOFTWARE = 1,
    LS_HARDWARE = 2,
    LS_OVERRIDE = 3
} ls_mode_t;

/* Mapping ls_mode_t to Linkscan mode string. */
static const char *linkscan_mode[] = {
    NO_SCANs,
    SOFTWAREs,
    HARDWAREs,
    OVERRIDEs
};

/* Data structure to save the info of lt lm_port_control. */
typedef struct ls_port_ctrl_s {
    /* Bitmap of fields to be operated. */
    uint32_t fld_flags;
#define FLD_MODE    0x1
#define FLD_LINK    0x2
#define FLD_NO_SYNC 0x4

    /* Key field port number. */
    int port;

    /* Linkscan mode. */
    ls_mode_t mode;

    /* Override link state */
    int ovr_link;

    /* Indicate whether a port UP/DOWN sequence should be triggered
     * when a link state change is detected.*/
    int no_sync;
} ls_port_ctrl_t;

/* Link state notification info. */
typedef struct link_state_notif_s {
    bcmlt_opcode_t op;
    int port;
    int link;
} link_state_notif_t;

/* Application callback list of link state notification. */
typedef struct link_appl_cb_s {
    struct link_appl_cb_s *next;
    bcm_linkscan_handler_t cb;
} link_appl_cb_t;

typedef struct link_ctrl_s {
    /* Link module initialized. */
    int inited;

    /* Mutex */
    sal_mutex_t lc_mutex;

    /* Ports to skip info during linkdown. */
    SHR_BITDCL *lc_pbm_info_skip;

    /* Link state callback registered via bcm_linkscan_register. */
    link_appl_cb_t *lc_cbs;

    /* Pbmp of lm_port_control. */
    bcm_pbmp_t lc_pbm_pc;

} link_ctrl_t;

static link_ctrl_t lc_info[BCM_MAX_NUM_UNITS] = {{0}};

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Get LM_CONTROL info.
 *
 * \param [in] unit Unit number.
 * \param [OUT] us Pointer to software polling interval in micro-seconds.
 *                 0 if not enabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 */
static int
lt_linkscan_control_get(int unit, int *us)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t enable, interval;
    int dunit, rv;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, LM_CONTROLs, &eh));

    rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    if (rv == SHR_E_NOT_FOUND) {
        *us = 0;
        SHR_EXIT();
    } else if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, SCAN_ENABLEs, &enable));

    if (enable) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, SCAN_INTERVALs, &interval));
        *us = (int)interval;
    } else {
        *us = 0;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set LM_CONTROL.
 *
 * \param [in] unit Unit number.
 * \param [in] us Specifies the software polling interval in micro-seconds.
 *                Disable the link scan if us is 0.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 */
static int
lt_linkscan_control_set(int unit, int us)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t enable, interval;
    int dunit, cur_us, cur_en;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    if (us == 0) {
        enable = 0;
    } else {
        enable = 1;
        us = (us > BCM_LINKSCAN_INTERVAL_MIN) ? us : BCM_LINKSCAN_INTERVAL_MIN;
    }

    SHR_IF_ERR_EXIT
        (lt_linkscan_control_get(unit, &cur_us));
    if (cur_us == us) {
        SHR_EXIT();
    }

    cur_en = (cur_us == 0) ? 0 : 1;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, LM_CONTROLs, &eh));

    if (cur_en != enable) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, SCAN_ENABLEs, enable));
    }

    interval = enable ? us : BCM_LINKSCAN_INTERVAL_DEFAULT;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, SCAN_INTERVALs, interval));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set logical table LM_PORT_CONTROL.
 *
 * \param [in] unit Unit number.
 * \param [in] op Operation code.
 * \param [in] ctrl Pointer to linkscan port control info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT BCMLT was not initialized.
 * \retval SHR_E_UNIT Invalid unit number or unit is not attached.
 * \retval SHR_E_PARAM Missing table name or entry handle = NULL.
 * \retval SHR_E_NOT_FOUND Table was not found.
 * \retval SHR_E_MEMORY Failed to allocate memory.
 */
static int
lt_port_control_set(int unit, bcmlt_opcode_t op, ls_port_ctrl_t *ctrl)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, LM_PORT_CONTROLs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, ctrl->port));

    if (ctrl->fld_flags & FLD_MODE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, LINKSCAN_MODEs,
                                          linkscan_mode[ctrl->mode]));
    }

    if (ctrl->fld_flags & FLD_LINK) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, OVERRIDE_LINK_STATEs, ctrl->ovr_link));
    }

    if (ctrl->fld_flags & FLD_NO_SYNC) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, MANUAL_SYNCs, ctrl->no_sync));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, op, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port control info.
 *
 * \param [in] unit Unit number.
 * \param [in/out] ctrl Pointer to linkscan port control info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT BCMLT was not initialized.
 * \retval SHR_E_UNIT Invalid unit number or unit is not attached.
 * \retval SHR_E_PARAM Missing table name or entry handle = NULL.
 * \retval SHR_E_NOT_FOUND Table was not found.
 * \retval SHR_E_MEMORY Failed to allocate memory.
 */
static int
lt_port_control_get(int unit, ls_port_ctrl_t *ctrl)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit, rv, idx;
    const char *cur_mode;
    uint64_t val;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, LM_PORT_CONTROLs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, ctrl->port));

    rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    if (rv == SHR_E_NOT_FOUND) {
        ctrl->mode = LS_NO_SCAN;
        ctrl->no_sync = 0;
        ctrl->ovr_link = 0;
        SHR_EXIT();
    } else if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(eh, LINKSCAN_MODEs, &cur_mode));

    for (idx = 0; idx < BCM_LINKSCAN_MODE_COUNT; idx++) {
        if (sal_strcmp(linkscan_mode[idx], cur_mode) == 0) {
            break;
        }
    }
    ctrl->mode = idx;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, MANUAL_SYNCs, &val));
    ctrl->no_sync = val;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, OVERRIDE_LINK_STATEs, &val));
    ctrl->ovr_link = val;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port link state from LM_LINK_STATE.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] logic_link Logical link state.
 * \param [out] phy_link PHY link state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT BCMLT was not initialized.
 * \retval SHR_E_UNIT Invalid unit number or unit is not attached.
 * \retval SHR_E_PARAM Missing table name or entry handle = NULL.
 * \retval SHR_E_NOT_FOUND Table was not found.
 * \retval SHR_E_MEMORY Failed to allocate memory.
 */
static int
lt_link_state_get(int unit, int port, int *logic_link, int *phy_link)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;
    uint64_t val;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, LM_LINK_STATEs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, port));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    if (logic_link) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, LINK_STATEs, &val));
        *logic_link = val;
    }

    if (phy_link) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, PHY_LINKs, &val));
        *phy_link = val;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse entry of logical table LM_LINK_STATE.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name Table name.
 * \param [in] entry_hdl Entry handle.
 * \param [out] notif_info Pointer to notification info.
 * \param [out] status Parser callback returned status, NULL for not care.
 *
 * \return none.
 */
static void
lt_link_state_parser(int unit, const char *lt_name, uint32_t entry_hdl,
                     void *notif_info, bcmi_ltsw_event_status_t *status)
{
    bcmlt_entry_handle_t eh = entry_hdl;
    bcmlt_unified_opcode_t op;
    link_state_notif_t *lsn = (link_state_notif_t *)notif_info;
    uint64_t fval;
    int rv;

    rv = bcmlt_entry_oper_get(eh, &op);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to parse opcode of %s event (%d).\n"),
                  lt_name, rv));
        return;
    }
    lsn->op = op.opcode.lt;

    rv = bcmlt_entry_field_get(eh, PORT_IDs, &fval);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to parse %s.%s (%d).\n"),
                  lt_name, PORT_IDs, rv));
        return;
    }
    lsn->port = fval;

    if ((lsn->op == BCMLT_OPCODE_DELETE) || (lsn->op == BCMLT_OPCODE_INSERT)) {
        *status = bcmiLtswEventStatusDismissNotif;
        return;
    }

    rv = bcmlt_entry_field_get(eh, LINK_STATEs, &fval);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to parse %s.%s (%d).\n"),
                  lt_name, LINK_STATEs, rv));
        return;
    }
    lsn->link = fval;

}

/*!
 * \brief Callback to process the link state event.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event name.
 * \param [in] notif_info Pointer to notification info.
 * \param [in] data User data.
 *
 * \return none.
 */
static void
link_state_ev_cb(int unit, const char *event, void *notif_info, void *data)
{
    link_state_notif_t *lsn = (link_state_notif_t *)notif_info;
    link_ctrl_t *lc = &lc_info[unit];
    bcm_port_info_t info;
    link_appl_cb_t *item, *next;
    int rv;
    bool skip_info = false;

    bcm_port_info_t_init(&info);

    if ((!lsn->link && SHR_BITGET(lc->lc_pbm_info_skip, lsn->port)) ||
        !BCM_PBMP_MEMBER(lc->lc_pbm_pc, lsn->port)) {
        skip_info = true;
    }

    if (!skip_info) {
        rv = bcm_ltsw_port_info_get(unit, lsn->port, &info);
        if (BCM_PBMP_MEMBER(lc->lc_pbm_pc, lsn->port) && SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Failed to get info of port %d (%d).\n"),
                      lsn->port, rv));
            return;
        }
    }

    info.linkstatus = lsn->link ? BCM_PORT_LINK_STATUS_UP :
                                  BCM_PORT_LINK_STATUS_DOWN;

    /* It is supposed no callback list change during processing callbacks.? */
    item = lc->lc_cbs;
    while (item) {
        next = item->next;
        if (item->cb) {
            item->cb(unit, lsn->port, &info);
        }
        item = next;
    }
}

/*!
 * \brief Initialize LM_PORT_CONTROL table for existing front panel ports.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Table was not found.
 */
static int
linkscan_port_control_init(int unit)
{
    bcm_pbmp_t pbmp_fp;
    bcm_port_t port;
    ls_port_ctrl_t ctrl = {0};

    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(pbmp_fp);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_PORT, &pbmp_fp));

    ctrl.fld_flags = FLD_MODE | FLD_LINK | FLD_NO_SYNC;
    ctrl.mode = LS_NO_SCAN;
    ctrl.no_sync = 1;
    ctrl.ovr_link = 0;

    BCM_PBMP_ITER(pbmp_fp, port) {
        ctrl.port = port;
        SHR_IF_ERR_EXIT
            (lt_port_control_set(unit, BCMLT_OPCODE_INSERT, &ctrl));
    }

    BCM_PBMP_ASSIGN(lc_info[unit].lc_pbm_pc, pbmp_fp);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover lc_info during warmboot.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Table was not found.
 */
static int
linkscan_ctrl_recover(int unit)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit, rv;
    uint64_t val;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, LM_PORT_CONTROLs, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, PORT_IDs, &val));

        BCM_PBMP_PORT_ADD(lc_info[unit].lc_pbm_pc, val);
    }

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT(rv);
    }
exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Internal HSDK functions
 */

int
bcmi_ltsw_link_port_info_skip_set(int unit, bcm_port_t port, int skip)
{
    link_ctrl_t *lc = &lc_info[unit];
    bcm_port_t  local_port;

    SHR_FUNC_ENTER(unit);

    if (!lc_info[unit].inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_port_local_get(unit, port, &local_port));
    } else {
        local_port = port;
    }

    sal_mutex_take(lc->lc_mutex, SAL_MUTEX_FOREVER);
    if (skip) {
        SHR_BITSET(lc->lc_pbm_info_skip, local_port);
    } else {
        SHR_BITCLR(lc->lc_pbm_info_skip, local_port);
    }
    sal_mutex_give(lc->lc_mutex);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_link_port_info_skip_get(int unit, bcm_port_t port, int *skip)
{
    link_ctrl_t *lc = &lc_info[unit];
    bcm_port_t  local_port;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(skip, SHR_E_PARAM);

    if (!lc_info[unit].inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_port_local_get(unit, port, &local_port));
    } else {
        local_port = port;
    }

    *skip = (SHR_BITGET(lc->lc_pbm_info_skip, local_port)) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_link_state_get(int unit, bcm_port_t port, int *link)
{
    bcm_port_t local_port;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(link, SHR_E_PARAM);

    if (!lc_info[unit].inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_port_local_get(unit, port, &local_port));
    } else {
        local_port = port;
    }

    rv = bcm_ltsw_linkscan_enable_port_get(unit, local_port);
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_EXIT
            (lt_link_state_get(unit, local_port, link, NULL));
    } else if (rv == SHR_E_DISABLED) {
        SHR_IF_ERR_EXIT
            (lt_link_state_get(unit, local_port, NULL, link));
    } else {
        SHR_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_link_port_update(int unit, bcm_port_t port, int link)
{
    bcm_port_t local_port;
    ls_port_ctrl_t cur_ctrl = {0};
    ls_port_ctrl_t ctrl = {0};
    shr_timeout_t to;
    int rv, logic_link, phy_link;

    SHR_FUNC_ENTER(unit);

    if (!lc_info[unit].inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_port_local_get(unit, port, &local_port));
    } else {
        local_port = port;
    }

    /* Save the current LM_PORT_CONTROL info. */
    cur_ctrl.port = local_port;
    cur_ctrl.fld_flags = FLD_MODE | FLD_LINK | FLD_NO_SYNC;
    SHR_IF_ERR_EXIT
        (lt_port_control_get(unit, &cur_ctrl));

    /* Set override link state to down to force a link event. */
    ctrl.port = local_port;
    ctrl.mode = LS_OVERRIDE;
    ctrl.ovr_link = link;
    ctrl.no_sync = 0;
    ctrl.fld_flags = FLD_MODE | FLD_LINK | FLD_NO_SYNC;
    rv = lt_port_control_set(unit, BCMLT_OPCODE_UPDATE, &ctrl);

    /* Wait for link down. */
    if (SHR_SUCCESS(rv)) {
        shr_timeout_init(&to, 1000000, 0);
        while (true) {
            SHR_IF_ERR_EXIT
                (lt_link_state_get(unit, port, &logic_link, &phy_link));
            if (logic_link == link) {
                break;
            }
            if (shr_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_BCM_LINK,
                    (BSL_META_U(unit, "port: %d, logic_link: %d, phy_link: %d\n"),
                        port, logic_link, phy_link));
                rv = SHR_E_TIMEOUT;
                break;
            }
        }
    }

    /* Recover linkscan port control. */
    SHR_IF_ERR_EXIT
        (lt_port_control_set(unit, BCMLT_OPCODE_UPDATE, &cur_ctrl));

    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_link_port_control_attach(int unit, bcm_port_t port)
{
    bcm_port_t local_port;
    ls_port_ctrl_t ctrl = {0};
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!lc_info[unit].inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_port_local_get(unit, port, &local_port));
    } else {
        local_port = port;
    }

    ctrl.fld_flags = FLD_MODE | FLD_LINK | FLD_NO_SYNC;
    ctrl.port = local_port;
    ctrl.mode = LS_NO_SCAN;
    ctrl.no_sync = 1;
    ctrl.ovr_link = 0;

    sal_mutex_take(lc_info[unit].lc_mutex, SAL_MUTEX_FOREVER);
    locked = true;

    SHR_IF_ERR_EXIT
        (lt_port_control_set(unit, BCMLT_OPCODE_INSERT, &ctrl));

    BCM_PBMP_PORT_ADD(lc_info[unit].lc_pbm_pc, port);

exit:
    if (locked) {
        sal_mutex_give(lc_info[unit].lc_mutex);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_link_port_control_detach(int unit, bcm_port_t port)
{
    bcm_port_t local_port;
    ls_port_ctrl_t ctrl = {0};
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!lc_info[unit].inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_port_local_get(unit, port, &local_port));
    } else {
        local_port = port;
    }

    ctrl.port = local_port;

    sal_mutex_take(lc_info[unit].lc_mutex, SAL_MUTEX_FOREVER);
    locked = true;

    SHR_IF_ERR_EXIT
        (lt_port_control_set(unit, BCMLT_OPCODE_DELETE, &ctrl));

    BCM_PBMP_PORT_REMOVE(lc_info[unit].lc_pbm_pc, port);

exit:
    if (locked) {
        sal_mutex_give(lc_info[unit].lc_mutex);
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Initialize the linkscan software module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_linkscan_init(int unit)
{
    link_ctrl_t *lc;
    int lport_max;
    uint32_t sz, alloc_sz;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    lc = &lc_info[unit];

    if (lc->inited) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_linkscan_detach(unit));
    }

    if (lc->lc_mutex == NULL) {
        lc->lc_mutex = sal_mutex_create("ltswlinkscanCtrlInfo");
        SHR_NULL_CHECK(lc->lc_mutex, SHR_E_MEMORY);
    }

    lport_max = bcmi_ltsw_dev_logic_port_num(unit);
    sz = SHR_BITALLOCSIZE(lport_max);
    alloc_sz = sz;
    lc->lc_pbm_info_skip = bcmi_ltsw_ha_mem_alloc(unit,
                                                  BCMI_HA_COMP_ID_LINK,
                                                  BCMINT_LTSW_LINK_HA_SKIP_PBM,
                                                  "bcmLinkSkipPbm",
                                                  &alloc_sz);
    SHR_NULL_CHECK(lc->lc_pbm_info_skip, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!bcmi_warmboot_get(unit)) {
        sal_memset(lc->lc_pbm_info_skip, 0, sz);

        SHR_IF_ERR_EXIT
            (linkscan_port_control_init(unit));
    } else {
        SHR_IF_ERR_EXIT
            (linkscan_ctrl_recover(unit));
    }

    sal_mutex_take(lc->lc_mutex, SAL_MUTEX_FOREVER);
    lc->inited = 1;
    sal_mutex_give(lc->lc_mutex);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Prepare linkscan module to detach specified unit.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_linkscan_detach(int unit)
{
    link_ctrl_t *lc;
    link_appl_cb_t *cb;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    lc = &lc_info[unit];

    if (!lc->inited || !lc->lc_mutex) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(lc->lc_mutex, SAL_MUTEX_FOREVER);
    lc->inited = 0;

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_EXIT
            (bcmi_lt_clear(unit, LM_PORT_CONTROLs));
    }

    SHR_IF_ERR_EXIT
        (lt_linkscan_control_set(unit, 0));

    if (lc->lc_cbs) {
        /* Unregister callback in event_mgr. */
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmi_lt_table_unsubscribe(unit, LM_LINK_STATEs, link_state_ev_cb),
             SHR_E_NOT_FOUND);
    }

    while(lc->lc_cbs != NULL) {
        cb = lc->lc_cbs;
        lc->lc_cbs = cb->next;
        sal_free(cb);
    }

    sal_mutex_give(lc->lc_mutex);

    sal_mutex_destroy(lc->lc_mutex);

    sal_memset(lc, 0, sizeof(link_ctrl_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable or disable link scan.
 *
 * Start or stop linkscan thread.
 *
 * \param [in] unit Unit number.
 * \param [in] us Specifies the software polling interval in micro-seconds.
 *                Disable the link scan if us is 0.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 */
int
bcm_ltsw_linkscan_enable_set(int unit, int us)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (!lc_info[unit].inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (lt_linkscan_control_set(unit, us));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the current linkscan mode.
 *
 * Retrieve the current linkscan mode.
 *
 * \param [in] unit Unit number.
 * \param [OUT] us Pointer to software polling interval in micro-seconds.
 *                 0 if not enabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 */
int
bcm_ltsw_linkscan_enable_get(int unit, int *us)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (!lc_info[unit].inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(us, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (lt_linkscan_control_get(unit, us));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the scanning mode for the specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] mode New scan mode for the specified port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 */
int
bcm_ltsw_linkscan_mode_set(int unit, bcm_port_t port, int mode)
{
    bcm_port_t local_port;
    ls_port_ctrl_t ctrl = {0};

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (!lc_info[unit].inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (mode < 0 || mode >= BCM_LINKSCAN_MODE_COUNT) {
        SHR_IF_ERR_EXIT(BCM_E_PARAM);
    }

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_port_local_get(unit, port, &local_port));
    } else {
        local_port = port;
    }

    sal_memset(&ctrl, 0, sizeof(ls_port_ctrl_t));
    ctrl.port = local_port;
    ctrl.fld_flags = FLD_MODE | FLD_NO_SYNC;
    if (mode == BCM_LINKSCAN_MODE_SW) {
        ctrl.mode = LS_SOFTWARE;
        ctrl.no_sync = 0;
    } else if (mode == BCM_LINKSCAN_MODE_HW) {
        ctrl.mode = LS_HARDWARE;
        ctrl.no_sync = 0;
    } else if (mode == BCM_LINKSCAN_MODE_OVERRIDE) {
        ctrl.mode = LS_OVERRIDE;
        ctrl.no_sync = 0;
    } else {
        ctrl.mode = LS_NO_SCAN;
        ctrl.no_sync = 1;
    }

    SHR_IF_ERR_EXIT
        (lt_port_control_set(unit, BCMLT_OPCODE_UPDATE, &ctrl));
exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the scanning mode for the specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] mode Pointer to current scan mode for the specified port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_FAIL Failed to get the scan mode.
 */
int
bcm_ltsw_linkscan_mode_get(int unit, bcm_port_t port, int *mode)
{
    bcm_port_t local_port;
    ls_port_ctrl_t ctrl = {0};

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (!lc_info[unit].inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(mode, SHR_E_PARAM);

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_port_local_get(unit, port, &local_port));
    } else {
        local_port = port;
    }

    ctrl.port = local_port;

    SHR_IF_ERR_EXIT
        (lt_port_control_get(unit, &ctrl));

    *mode = ctrl.mode;

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the scanning mode for the specified ports.
 *
 * \param [in] unit Unit number.
 * \param [in] pbm Port bit map.
 * \param [in] mode New scan mode for the specified port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 */
int
bcm_ltsw_linkscan_mode_set_pbm(int unit, bcm_pbmp_t pbm, int mode)
{
    bcm_port_t port;

    SHR_FUNC_ENTER(unit);

    BCM_PBMP_ITER(pbm, port) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_linkscan_mode_set(unit, port, mode));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the override link state for the specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] link Override link state for the specified port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 */
int
bcm_ltsw_linkscan_override_link_state_set(int unit, bcm_port_t port, int link)
{
    bcm_port_t loc_port;
    ls_port_ctrl_t ctrl;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (!lc_info[unit].inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_port_local_get(unit, port, &loc_port));
    } else {
        loc_port = port;
    }

    sal_memset(&ctrl, 0, sizeof(ls_port_ctrl_t));
    ctrl.port = loc_port;
    ctrl.fld_flags = FLD_LINK;
    ctrl.ovr_link = link;

    SHR_IF_ERR_EXIT
        (lt_port_control_set(unit, BCMLT_OPCODE_UPDATE, &ctrl));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the override link state for the specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] link Override link state for the specified port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 */
int
bcm_ltsw_linkscan_override_link_state_get(int unit, bcm_port_t port, int *link)
{
    bcm_port_t loc_port;
    ls_port_ctrl_t ctrl = {0};

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (!lc_info[unit].inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(link, SHR_E_PARAM);

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_port_local_get(unit, port, &loc_port));
    } else {
        loc_port = port;
    }

    ctrl.port = loc_port;
    SHR_IF_ERR_EXIT
        (lt_port_control_get(unit, &ctrl));

    *link = ctrl.ovr_link;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve if linkscan managing a specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \retval SHR_E_NONE Port is being scanned.
 * \retval SHR_E_DISABLED Port is not being scanned.
 * \retval SHR_E_PORT Port is invalid.
 */
int
bcm_ltsw_linkscan_enable_port_get(int unit, bcm_port_t port)
{
    int us, mode;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (!lc_info[unit].inited) {
        SHR_ERR_EXIT(SHR_E_DISABLED);
    }

    SHR_IF_ERR_EXIT
        (bcm_ltsw_linkscan_enable_get(unit, &us));

    if (us == 0) {
        SHR_ERR_EXIT(SHR_E_DISABLED);
    }

    SHR_IF_ERR_EXIT
        (bcm_ltsw_linkscan_mode_get(unit, port, &mode));

    if (mode == BCM_LINKSCAN_MODE_NONE) {
        SHR_ERR_EXIT(SHR_E_DISABLED);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Force a transient link down event on port.
 *
 * Force a transient link-down event to be recognized regardless of the
 * current state of the physical link. If the link is up at the time of
 * the call, both a link down and a subsequent link up event will be
 * recognized.  If traffic is flowing on the link at the time of the
 * call, the affect on the flowing traffic is undefined.  If the link is
 * down at the time of the call, no action will be taken.
 *
 * \param [in] unit Unit number.
 * \param [in] pbm Bit map of ports to trigger event on.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Link module not initialized.
 */
int
bcm_ltsw_link_change(int unit, bcm_pbmp_t pbm)
{
    int port, link, rv = SHR_E_NONE;
    int cur_mode[BCM_PBMP_PORT_MAX] = {0};
    int ovr_link[BCM_PBMP_PORT_MAX] = {0};
    bcm_pbmp_t pbm_down;
    shr_timeout_t to;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (!lc_info[unit].inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    /* Save the current linkscan mode and override link state. */
    BCM_PBMP_ITER(pbm, port) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_linkscan_mode_get(unit, port, &cur_mode[port]));
        SHR_IF_ERR_EXIT
            (bcm_ltsw_linkscan_override_link_state_get(unit, port,
                                                       &ovr_link[port]));
    }

    BCM_PBMP_ITER(pbm, port) {
        /* Set override link state to down to force a link-down event. */
        rv = bcm_ltsw_linkscan_override_link_state_set(unit, port, 0);
        if (SHR_FAILURE(rv)) {
            break;
        }
        if (cur_mode[port] != BCM_LINKSCAN_MODE_OVERRIDE) {
            rv = bcm_ltsw_linkscan_mode_set(unit, port,
                                            BCM_LINKSCAN_MODE_OVERRIDE);
            if (SHR_FAILURE(rv)) {
                break;
            }
        }
    }

    /* Wait for link down. */
    if (SHR_SUCCESS(rv)) {
        shr_timeout_init(&to, 1000000, 0);

        BCM_PBMP_CLEAR(pbm_down);

        while(true) {
            BCM_PBMP_ITER(pbm, port) {
                rv = bcmi_ltsw_link_state_get(unit, port, &link);
                if (SHR_FAILURE(rv)) {
                    break;
                }
                if (link == 0) {
                    BCM_PBMP_PORT_ADD(pbm_down, port);
                }
            }
            if (BCM_PBMP_EQ(pbm_down, pbm)) {
                break;
            }
            if (shr_timeout_check(&to)) {
                rv = SHR_E_TIMEOUT;
                break;
            }
        }
    }

    BCM_PBMP_ITER(pbm, port) {
        /* Recover linkscan mode. */
        SHR_IF_ERR_CONT
            (bcm_ltsw_linkscan_mode_set(unit, port, cur_mode[port]));
        /* Recover override link state. */
        SHR_IF_ERR_CONT
            (bcm_ltsw_linkscan_override_link_state_set(unit, port,
                                                       ovr_link[port]));
    }

    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register a handler to be called when a link status change is noticed.
 *
 * \param [in] unit Unit number.
 * \param [in] f Pointer to function to call when link status change is seen.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 */
int
bcm_ltsw_linkscan_register(int unit, bcm_linkscan_handler_t f)
{
    link_ctrl_t *lc;
    link_appl_cb_t *item;
    bool locked = false;
    bool first_cb = false;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    lc = &lc_info[unit];
    if (!lc->inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(lc->lc_mutex, SAL_MUTEX_FOREVER);
    locked = true;

    item = lc->lc_cbs;
    if (item) {
        while (item) {
            if (item->cb == f) {
                break;
            }
            item = item->next;
        }
        if (item) {
            SHR_IF_ERR_EXIT(SHR_E_EXISTS);
        }
    } else {
        first_cb = true;
    }

    item = sal_alloc(sizeof(link_appl_cb_t), "ltswlinkscanApplCB");
    SHR_NULL_CHECK(item, SHR_E_MEMORY);
    sal_memset(item, 0, sizeof(link_appl_cb_t));
    item->cb = f;
    item->next = lc->lc_cbs;
    lc->lc_cbs = item;

    sal_mutex_give(lc->lc_mutex);
    locked = false;

    if (first_cb) {

        SHR_IF_ERR_EXIT
            (bcmi_lt_table_parser_set(unit, LM_LINK_STATEs,
                                      lt_link_state_parser,
                                      sizeof(link_state_notif_t)));
        SHR_IF_ERR_EXIT
            (bcmi_lt_table_attrib_set(unit, LM_LINK_STATEs,
                                      (BCMI_LTSW_EVENT_HIGH_PRIORITY |
                                       BCMI_LTSW_EVENT_APPL_CALLBACK)));
        SHR_IF_ERR_EXIT
            (bcmi_lt_table_subscribe(unit, LM_LINK_STATEs, link_state_ev_cb,
                                     NULL));
    }

exit:
    if (locked) {
        sal_mutex_give(lc->lc_mutex);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove a previously registered handler from the callout list.
 *
 * \param [in] unit Unit number.
 * \param [in] f Pointer to function to be deregistered.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_NOT_FOUND Could not find matching handler.
 */
int
bcm_ltsw_linkscan_unregister(int unit, bcm_linkscan_handler_t f)
{
    link_ctrl_t *lc;
    link_appl_cb_t *item, *pre;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    lc = &lc_info[unit];
    if (!lc->inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(lc->lc_mutex, SAL_MUTEX_FOREVER);
    locked = true;

    item = lc->lc_cbs;
    while (item) {
        if (item->cb == f) {
            break;
        }
        pre = item;
        item = item->next;
    }
    if (!item) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    } else if (item == lc->lc_cbs) {
        lc->lc_cbs = item->next;
    } else {
        pre->next = item->next;
    }
    sal_free(item);

    sal_mutex_give(lc->lc_mutex);
    locked = false;

    if (!lc->lc_cbs) {
        SHR_IF_ERR_EXIT
            (bcmi_lt_table_unsubscribe(unit, LM_LINK_STATEs, link_state_ev_cb));
    }

exit:
    if (locked) {
        sal_mutex_give(lc->lc_mutex);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Wait for a set of links to recognize link up.
 *
 * \param [in] unit Unit number.
 * \param [in/out] pbm Bit map of ports to wait for link up. Mask of those link
 *                     up on return.
 * \param [in] us Number of microseconds to wait.
 *
 * \retval SHR_E_NONE All links are ready.
 * \retval SHR_E_INIT Link module not initialized.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_PARAM Invalid param.
 * \retval SHR_E_TIMEOUT Not all links ready in specified time.
 * \retval SHR_E_DISABLED Linkscan not running on one or more of the ports.
 */
int
bcm_ltsw_link_wait(int unit, bcm_pbmp_t *pbm, int us)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t link;
    int dunit, port, interval;
    bcm_pbmp_t pbm_up, pbm_wait;
    shr_timeout_t to;

    SHR_FUNC_ENTER(unit);

    if ((pbm == NULL) || (us < 0)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (!lc_info[unit].inited) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcm_ltsw_linkscan_enable_get(unit, &interval));
    if (interval <= 0) {
        SHR_IF_ERR_EXIT(SHR_E_DISABLED);
    }

    BCM_PBMP_ITER(*pbm, port) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_linkscan_enable_port_get(unit, port));
    }

    BCM_PBMP_CLEAR(pbm_up);
    BCM_PBMP_ASSIGN(pbm_wait, *pbm);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, LM_LINK_STATEs, &eh));

    shr_timeout_init(&to, (sal_usecs_t)us, 0);

    while (true) {

        BCM_PBMP_ITER(pbm_wait, port) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(eh, PORT_IDs, port));

            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(eh,
                                    BCMLT_OPCODE_LOOKUP,
                                    BCMLT_PRIORITY_NORMAL));

            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_get(eh, LINK_STATEs, &link));

            if (link) {
                BCM_PBMP_PORT_ADD(pbm_up, port);
            }

            SHR_IF_ERR_EXIT
                (bcmlt_entry_clear(eh));
        }
        if (BCM_PBMP_EQ(pbm_up, *pbm)) {
            break;
        }
        BCM_PBMP_REMOVE(pbm_wait, pbm_up);
        if (shr_timeout_check(&to)) {
            BCM_PBMP_AND(*pbm, pbm_up);
            SHR_IF_ERR_EXIT(SHR_E_TIMEOUT);
        }
    }

exit:
    if (eh!= BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

#if defined(BROADCOM_DEBUG)
/*!
 * \brief Display information about linkscan callbacks.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_linkscan_dump(int unit)
{
    link_ctrl_t *lc;
    link_appl_cb_t *item;

    SHR_FUNC_ENTER(unit);

    for (unit = 0; unit < BCM_MAX_NUM_UNITS; unit++) {
        lc = &lc_info[unit];
        if (!lc->inited) {
            LOG_CLI((BSL_META_U(unit,
                                "BCM linkscan not initialized for unit %d\n"), unit));
            continue;
        }

        LOG_CLI((BSL_META_U(unit,
                            "BCM linkscan callbacks for unit %d\n"), unit));
        for (item = lc->lc_cbs; item != NULL; item = item->next) {
            LOG_CLI((BSL_META_U(unit,
                                "\tFn %p\n"), (void *)item->cb));
        }
    }

    SHR_FUNC_EXIT();
}
#endif  /* BROADCOM_DEBUG */

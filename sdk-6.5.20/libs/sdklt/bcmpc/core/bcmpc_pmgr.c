/*! \file bcmpc_pmgr.c
 *
 * BCMPC Port Manager (PMGR) APIs
 *
 * The Port Manager is responsible for updating the hardware based on the
 * changes to the logical tables. The Port Manager does not write to hardware
 * directly, but relies on the PMGR drivers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <shr/shr_ha.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>

#include <bcmpc/bcmpc_topo_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_pmgr_drv_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_tm_internal.h>
#include <bcmpc/bcmpc_sec_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PMGR

/*!
 * \brief Get and check the port manager driver is null.
 *
 * Get and check the PMGR driver is null. If the PMGR driver, \c _drv, is NULL,
 * then exit the function.
 *
 * \param [in] _u Unit number.
 * \param [in] _p Device physical port.
 * \param [out] _drv Pmgr driver pointer.
 */
#define PMGR_DRV_GET(_u, _p, _drv) \
    do { \
        _drv = pmgr_drv_get(_u, _p); \
        if (_drv == NULL) { \
            SHR_EXIT(); \
        } \
    } while (0)

/*!
 * \brief Cached link state.
 *
 * This structure is used to detect the link transition and trigger the link
 * up/down procedure when linkscan is disabled or under forced link mode.
 *
 * A port will be in forced link mode when it is configured as MAC/PHY loopback
 * or disabled. When change to the forced link mode, PC is responsible to
 * trigger the link transition no matter linkscan is enabled or disabled.
 *
 * The \c forced_link means the port is in forced link mode or not.
 *
 * When forced_link is set, \c live_link shows the forced link state. e.g.
 * forced_link = 1 and live_link = 0 indicates the port is forced link down.
 *
 * When forced_link is clear, \c live_link means the current link state which
 * is got from the PHY.
 *
 * As for \c latched_link_down, it is used to trigger a link transition to down
 * when we update any port settings.
 */
typedef struct pmgr_link_state_s {

    /*! Port link state. 1 indicate Up, else DOWN. */
    bool link_state;

    /*! Forced link.  */
    bool forced_link;

    /*! Forced link status. */
    bool forced_status;

    /*! Port update operation done. */
    bool operation_done;

} pmgr_link_state_t;


/*! Software cached link state. */
static volatile pmgr_link_state_t
pmgr_link_state[BCMPC_NUM_UNITS_MAX][BCMPC_NUM_PPORTS_PER_CHIP_MAX];

/*! Utilities for accessing pmgr_link_state array. */
#define PMGR_LINK_STATE_OBJ(_u, _p) pmgr_link_state[_u][_p]
#define PMGR_LINK_STATE(_u, _p) pmgr_link_state[_u][_p].link_state
#define PMGR_LINK_FORCED(_u, _p) pmgr_link_state[_u][_p].forced_link
#define PMGR_LINK_FORCED_STATUS(_u, _p) pmgr_link_state[_u][_p].forced_status
#define PMGR_PORT_OPER_DONE(_u, _p) pmgr_link_state[_u][_p].operation_done

/*! HA link state. */
static bcmpc_ha_link_state_info_t *pmgr_ha_link_state[BCMPC_NUM_UNITS_MAX];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the port manager driver pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 *
 * \return Pointer of a port manager driver.
 */
static bcmpc_pmgr_drv_t*
pmgr_drv_get(int unit, bcmpc_pport_t pport)
{
    int rv;
    bcmpc_drv_t *pc_drv = NULL;

    /*
     * Suppose that a port is not within a real hardware
     * PortMacro if the PMAC driver is not specified.
     * For such kind of ports, e.g. CMIC port, we do not call
     * PMGR driver for the further hardware configuration.
     */
    rv = bcmpc_drv_get(unit, &pc_drv);
    if (SHR_FAILURE(rv)) {
        return NULL;
    }
    if (pc_drv->pmac_drv_get(unit, pport) == NULL) {
        return NULL;
    }

    /* Otherwise, we use the default PMGR driver. */
    return &bcmpc_pmgr_drv_default;
}

/*!
 * \brief Execute the PHY operation.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] op PHY operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_phy_op_exec(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->phy_op_exec, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->phy_op_exec(unit, pport, op));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Execute the PortMAC operation.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] op PortMAC operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_pmac_op_exec(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_port_cfg_t pcfg;
    bcmpc_lport_t lport;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pmac_op_exec, SHR_E_NO_HANDLER);

    if (sal_strcmp(op->name, "speed_set") == 0 &&
        op->param == BCMPC_OP_PARAM_NONE) {
        /* No speed value is specified, to sync MAC speed with PHY. */
        bcmpc_port_cfg_t_init(&pcfg);
        lport = bcmpc_pport_to_lport(unit, pport);
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg)),
             SHR_E_NOT_FOUND);

        op->param =  pcfg.speed;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pmac_op_exec(unit, pport, op));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Execute the port config profile.
 *
 * \ref bcmpc_cfg_profile_t.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] prof Config profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_profile_exec(int unit, bcmpc_pport_t pport, bcmpc_cfg_profile_t *prof)
{
    bcmpc_drv_t *pc_drv;
    bcmpc_operation_t *op;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    for (i = 0; i < prof->op_cnt; i++) {
        op = &prof->ops[i];

        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Execute operation: drv[%s], op[%s], prm[%d]\n"),
             op->drv, op->name, op->param));

        if (sal_strcmp(op->drv, "phy") == 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (pmgr_phy_op_exec(unit, pport, op));
        } else if (sal_strcmp(op->drv, "pmac") == 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (pmgr_pmac_op_exec(unit, pport, op));
        } else if (sal_strcmp(op->drv, "tm") == 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_tm_op_exec(unit, pport, op));
        } else if (sal_strcmp(op->drv, "sec") == 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_sec_op_exec(unit, pport, op));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update port local abilities.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] pabilities Port abilities being updated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_port_abilities_update(int unit, bcmpc_pport_t pport,
                           bcmpc_pmgr_port_abilities_t *pabilities)
{
    bcmpc_drv_t *pc_drv = NULL;
    bool found = 0;
    int idx, cur_idx, next_idx, abil_idx, num_nonabil, num_unsupport = 0;
    const bcmpc_dev_ability_entry_t  *unabilities;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    unabilities = pc_drv->dev_unsupported_abilities_get(unit, pport,
                                                        &num_nonabil);
    if (!num_nonabil) {
        return SHR_E_NONE;
    } else {
        SHR_NULL_CHECK(unabilities, SHR_E_FAIL);
    }

    for (idx = 0; idx < pabilities->num_ability; idx++) {
        for (abil_idx = 0; abil_idx < num_nonabil; abil_idx++) {
            found = 0;
            if ((pabilities->port_abilities[idx].speed ==
                                            unabilities[abil_idx].speed) &&
                (pabilities->port_abilities[idx].num_lanes ==
                                            unabilities[abil_idx].num_lanes) &&
                (pabilities->port_abilities[idx].fec_type ==
                                            unabilities[abil_idx].fec_type)) {
                num_unsupport++;
                found = 1;
            }
            if (found || (pabilities->port_abilities[idx].speed == 0)) {
                cur_idx = idx++;
                next_idx = idx;
                while (pabilities->port_abilities[next_idx].speed == 0) {
                    next_idx++;
                    if (next_idx >= pabilities->num_ability) break;
                }
                if (next_idx < pabilities->num_ability) {
                    idx = next_idx;
                    pabilities->port_abilities[cur_idx] =
                                          pabilities->port_abilities[idx];
                    pabilities->port_abilities[idx].speed = 0;

                    idx = cur_idx - 1;;
               } else {
                    break;
               }
            }
        }
    }
    pabilities->num_ability -= num_unsupport;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Wait for operation done to be successful.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] link  link status - 0 (down), 1 (up).
 * \param [out] status Return status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_port_oper_done_wait(int unit, bcmpc_pport_t pport, int link, int *status)
{
    bcmpc_drv_t *pc_drv = NULL;
    uint32_t timeout_in_usec = 0;
    uint32_t timeout_up = 0;
    uint32_t timeout_down = 0;
    uint32_t timeout_count = 0;
    uint32_t sleep_usecs = 0;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t local_pport = BCMPC_INVALID_PPORT;
    uint64_t port_u64;
    static bool print_once = true;
    bcmpc_lm_mode_t lm_mode;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /*
     * This function can be invoked in different contextx and when called,
     * there could be instances where the PC local databases are not
     * updated until this function returns.
     * Since LM component is being notified about the events here, the
     * notification could fail if the logical port to physical port map is
     * not valid in PC database which are returned to the other components.
     * Check the lport to pport map and if not valid, return success after
     * setting the physical port in forced mode and set the forced link
     * state.
     */
    lport = bcmpc_pport_to_lport(unit, pport);
    port_u64 = lport;
    local_pport = bcmpc_lport_to_pport(unit, lport);

    LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Waiting for link(%d)(%s)..Operation Done!!!\n"),
                        pport, link? "UP":"DOWN"));

    /*
     * If the link is already down, notifying ForceDown to LM will have
     * no effect, the link down sequence will not be initiated since the
     * link is already down. Mark the link as forced and return success.
     */
    if ((!link) || (local_pport == BCMPC_INVALID_PPORT)) {
        if (!PMGR_LINK_STATE(unit, pport)) {
            PMGR_LINK_FORCED(unit, pport) = 1;
            PMGR_LINK_FORCED_STATUS(unit, pport) = link;
            LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "link already (%s)(%d)..Return!!!\n"),
                     link? "UP":"DOWN", pport));
            SHR_EXIT();
        }
    } else if (link) {
        /*
         * If the link is already UP, notifying ForceUP to LM will have
         * no effect, the link UP sequence will not be initiated since the
         * link is already UP.
         * Mark the link as forced.
         * Set the port forced link status to UP.
         */
        if (PMGR_LINK_STATE(unit, pport)) {
            PMGR_LINK_FORCED(unit, pport) = 1;
            PMGR_LINK_FORCED_STATUS(unit, pport) = link;
            LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "link already (%s)(%d)..Return!!!\n"),
                     link? "UP":"DOWN", pport));
            SHR_EXIT();
        }
    }

    /*
     * Get the worst case value time duration to wait for the operation
     * to complete. This value is device dependent.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    if (pc_drv && pc_drv->link_sequence_exec_timeout_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_drv->link_sequence_exec_timeout_get(unit, &timeout_up,
                                                   &timeout_down));
    } else {
        timeout_up = 300000;
        timeout_down = 300000;
    }

    if (link) {
        timeout_in_usec = timeout_up;
    } else {
        timeout_in_usec = timeout_down;
    }

    /* For Simulator environments the timeout should be more. */
    if (!bcmdrd_feature_is_real_hw(unit)) {
        timeout_in_usec = timeout_in_usec * 10;
    }
    timeout_count = 1000;
    sleep_usecs = 1000;

    /* Set the operational done to 0, and then wait. */
    PMGR_PORT_OPER_DONE(unit, pport) = 0;

    /*
     * Force the link status to 0, also indicate that the port link is in
     * forced state.
     */
    PMGR_LINK_FORCED(unit, pport) = 1;
    PMGR_LINK_FORCED_STATUS(unit, pport) = link;

    LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit, "In Forced State Now!!\n")));

    if (link) {
        /*
         * Notify LM of force link up for the LM to initiate the link UP
         * sequence.
         */
        bcmevm_publish_event_notify(unit, "bcmpcEvLinkForceUp", port_u64);
    } else {
        /*
         * Notify LM of force link down before changing hardware, LM will
         * initiate the link DOWN sequence for the port.
         */
        bcmevm_publish_event_notify(unit, "bcmpcEvLinkForceDown", port_u64);
    }

    /*
     * If linkscan is enabled on the port then there is no need for special
     * handling as the LM will drive the link UP/DOWN sequence as required
     * based on the port enable/disable operation.
     * If linkscan is in MANUAL_SYNC mode, that is the user is driving the
     * link UP/DOWN sequence then special handling is required.
     *
     * Since manual_sync = 1, operation_done bit will be set by the linkscan
     * so need to skip the wait for operation_done.
     */
    sal_memset(&lm_mode, 0, sizeof(lm_mode));

    rv = bcmpc_linkscan_mode_get(unit, lport, &lm_mode);

    if ((rv == SHR_E_NOT_FOUND) || (lm_mode.manual_sync)) {
        SHR_EXIT();
    }

    do {
        if (PMGR_PORT_OPER_DONE(unit, pport)) {
            LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Operation Done(%d) (%d)!!!\n"),
                                pport, timeout_count));
            *status = SHR_E_NONE;
            break;
        }

        sal_usleep(sleep_usecs);

        if (print_once) {
            LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Waiting ..Operation Done!!!\n")));
            print_once = false;
        }

        timeout_count += sleep_usecs;
        if (timeout_count > timeout_in_usec) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Timeout for pport %0d: link(%s)\n"),
                                  pport, link? "UP": "DOWN"));
            *status = SHR_E_TIMEOUT;
            /* Undo the forced link state.  */
            PMGR_LINK_FORCED(unit, pport) = 0;
            PMGR_LINK_FORCED_STATUS(unit, pport) = 0;
            break;
        }
    } while (1);


    /*
     * If the operation either link UP/DOWN sequence execution times-out,
     * since the wait is much longer than the worst case time for the entire
     * sequence execution, which is done in LM thread context. It is a fair
     * guess to assume that the sequence execution failed for some reason.
     * The reason for the sequence execution failure cannot be captured as
     * this is done in the LM thread context and not in this context.
     * The operation_done should be set to 1, to ensure the same operation
     * can be retried.
     */
    if (*status != SHR_E_NONE) {
        /* Update the operation_done bit for this port. */
        PMGR_PORT_OPER_DONE(unit, pport) = 1;
    }

exit:
    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Logical port enable/disable set.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] lport Logical port number.
 * \param [in] cfg Port configuration.
 * \param [in] is_new 1 - New Logical Port
 *                    0 - Update to logcial port
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_port_enable_set(int unit, bcmpc_pport_t pport, bcmpc_lport_t lport,
                       bcmpc_port_cfg_t *cfg, bool is_new)
{
    uint64_t port_u64 = lport;
    bool is_forced_down = false;
    bool is_forced_up = false;
    bool is_forced_none = false;
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_port_cfg_t cfg_old;
    int status = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    bcmpc_port_cfg_t_init(&cfg_old);

    if (!is_new) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&cfg_old)));
    }

    /*
     * Port enable/disable set operations.
     * If this a transition operation, then need to notify the LM of
     * LM link UP/DOWN sequences to handle the port state configuration.
     */
    if (SHR_BITGET(cfg->fbmp, BCMPC_PORT_LT_FLD_ENABLE)) {
        if ((is_new) || (cfg->enable != cfg_old.enable)) {
            /*
             * The logic below handles the port enable/disable operation.
             * This is a new port add.
             */
            if (cfg->enable == 0) {
                is_forced_down = true;
            } else {
                /* If MAC loopback is enabled, force the link UP. */
                if ((!SHR_BITGET(cfg->fbmp, BCMPC_PORT_LT_FLD_LOOPBACK_MODE) &&
                    (cfg_old.lpbk_mode== BCMPC_PORT_LPBK_MAC)) ||
                    (SHR_BITGET(cfg->fbmp, BCMPC_PORT_LT_FLD_LOOPBACK_MODE) &&
                    (cfg->lpbk_mode== BCMPC_PORT_LPBK_MAC))) {
                    is_forced_up = true;
                } else {
                    is_forced_none = true;
                }
            }
        }
    }

    if (is_forced_down) {
        /*
         * This block of code is executed, when the port link is forced,
         * this could be due to MAC loopback being enabled on a logical port.
         * If MAC loopback is enabled on the port, then following set of
         * operations are executed on sequence.
         * 1. Notify LM with the event ForceLinkDown, this event will
         *    help the LM to initiate the link down sequence on the port.
         * 2. Set the operation done bit for this port to 0.
         * 3. After notifying the LM, the execution is blocked until the
         *    operation bit is set. If the operation done bit is not
         *    with in the timeout period, error is returned.
         * 4. For initiating the link down sequence the LM will invoke the
         *    PC function call, where the per-chip link down/up sequence
         *    is stored.
         * 5. At the end of the link down sequence execution, the
         *    operation done bit is set to 1.
         * 6. The port configuration function is called for setting up the
         *    loopback in the hardware.
         */

        /*
         * Set the operation_done bit to 0 and then wait for the link DOWN
         * sequence operation to be complete.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (pmgr_port_oper_done_wait(unit, pport, 0, &status));

        /*
         * Now wait for the operation done bit to be set to 1, indicating
         * that the link DOWN sequence was successfull executed.
         * The function always returns success, the value of "status"
         * indicates if the link DOWN sequence execution was a
         * success or timed-out.
         */
        if (status != SHR_E_NONE) {
            /*
             * The operation timed-out, undo the "ForceDown"
             * LM notification by notifying a "ForceNone" event.
             */
            bcmevm_publish_event_notify(unit, "bcmpcEvLinkForceNone",
                                        port_u64);

            SHR_ERR_EXIT(SHR_E_TIMEOUT);
        }
    }

    /* Set the port enable/disable, only set the PHY flags.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->port_enable_set(unit, pport, PM_PORT_ENABLE_PHY, cfg));

    if (is_forced_up) {
        /*
         * Now wait for the operation done bit to be set to 1, indicating
         * that the link UP sequence was successfull executed.
         * The function always returns success, the value of "status"
         * indicates if the link UP sequence execution was a
         * success or timed-out.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (pmgr_port_oper_done_wait(unit, pport, 1, &status));

        if (status != SHR_E_NONE) {
            /*
             * The operation timed-out, undo the "ForceUp"
             * LM notification by notifying a "ForceNone" event.
             */
            bcmevm_publish_event_notify(unit, "bcmpcEvLinkForceNone",
                                        port_u64);

            SHR_ERR_EXIT(SHR_E_TIMEOUT);
        }
    }

    /* Notify ForceNone event to LM if the flag is set. */
    if (is_forced_none) {
        bcmevm_publish_event_notify(unit, "bcmpcEvLinkForceNone", port_u64);

        /* Set the forced link to none.  */
        PMGR_LINK_FORCED(unit, pport) = 0;
        PMGR_LINK_FORCED_STATUS(unit, pport) = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Link force down and execute the link down sequence.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] link  link status - 0 (down), 1 (up).
 * \param [out] status Return status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_port_loopback_set(int unit, bcmpc_pport_t pport, bcmpc_lport_t lport,
                       bcmpc_port_cfg_t *cfg, bool is_new)
{
    uint64_t port_u64 = lport;
    bool is_forced_up = false;
    bool is_forced_down = false;
    bool is_forced_none = false;
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_port_cfg_t cfg_old;
    int status = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    bcmpc_port_cfg_t_init(&cfg_old);

    if (!is_new) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&cfg_old)));
    }

    /*
     * Loopback operation being performed, check if the transition
     * is from None -> Loopback or Loopback -> None. If this a
     * transition operation, then need to notify the LM of
     * LM link UP/DOWN sequences to handle the loopback configuration.
     */
    if (SHR_BITGET(cfg->fbmp, BCMPC_PORT_LT_FLD_LOOPBACK_MODE)) {
        if (is_new) {
            /*
             * The logic below handles the MAC loopback set operations.
             * Since this is a new port being inserted with
             * MAC loopback set, the link needs to be forced DOWN
             * followed by forced UP.
             */
            if (cfg->lpbk_mode == BCMPC_PORT_LPBK_MAC) {
                is_forced_down = true;
                is_forced_none = false;
                /*
                 * The link UP should only be forced if the
                 * port is in enabled state.
                 */
                if (cfg->enable) {
                    is_forced_up = true;
                } else {
                    is_forced_up = false;
                }
            }

            /*
             * The logic below handles the PHY loopback set operations.
             * Since this is a new port being inserted with
             * PHY loopback set, there is no need to execute link
             * DOWN/UP sequence.
             */
            if (cfg->lpbk_mode == BCMPC_PORT_LPBK_PHY_PMD) {
                is_forced_down = false;
                is_forced_up = false;
                is_forced_none = false;
            }

            /*
             * There is no need to explicitly handle loopback NONE set
             * operation. Since this is a new port being inserted with
             * loopback NONE, this does not have any significance
             * and its a no-operation. This LT operation can be ignored.
             * if (cfg->lpbk_mode == BCMPC_PORT_LPBK_NONE) {
             * }
             */
        } else {
            /* This is an loopback update operation. */
            if (cfg->lpbk_mode != cfg_old.lpbk_mode) {
                /*
                 * The logic below handles the MAC loopback set operations.
                 * When MAC loopback is set, first the link will be
                 * forced DOWN by notifying the LM with ForceDown event
                 * followed by configuring the MAC loopback in hardware
                 * followed by forced link UP sequence by notifying the
                 * LM with ForceUp event.
                 */
                if (cfg->lpbk_mode == BCMPC_PORT_LPBK_MAC) {
                    is_forced_down = true;
                    is_forced_none = false;

                    /*
                     * The link UP should only be forced if the
                     * port is in enabled state.
                     */
                    if ((cfg->enable == cfg_old.enable) &&
                        (cfg_old.enable == 0)) {
                        is_forced_up = false;
                    } else {
                        is_forced_up = true;
                    }
                }

                /*
                 * The logic below handles the PMD loopback set operations.
                 * It is always recommended to set the loopback configuration
                 * from none to either MAC or PHY.
                 * When PMD loopback is set, if the port is in MAC loopback
                 * 1. Notify LM to force the link DOWN on the port.
                 * 2. Remove the MAC loopback in hardware.
                 * 3. Set the PMD loopback in the hardware.
                 * 4. Notify LM with forceNone event on the port.
                 */
                if (cfg->lpbk_mode == BCMPC_PORT_LPBK_PHY_PMD) {
                    switch (cfg_old.lpbk_mode) {
                        case BCMPC_PORT_LPBK_MAC:
                            is_forced_down = true;
                            is_forced_up = false;
                            is_forced_none = true;
                            break;
                        case BCMPC_PORT_LPBK_NONE:
                        default:
                            is_forced_down = false;
                            is_forced_up = false;
                            is_forced_none = false;
                            break;
                    }
                }

                /*
                 * The logic below handles the loopback None set operations.
                 * When the port loopback is transitioning from MAC to NONE
                 * 1. Notify LM to force the link DOWN on the port.
                 * 2. Remove the MAC loopback in hardware.
                 * 3. Notify LM with forceNone event on the port.
                 *
                 * When the port loopback is transitioning from PHY to NONE
                 * 1. Remove the PHY loopback in hardware.
                 */
                if (cfg->lpbk_mode == BCMPC_PORT_LPBK_NONE) {
                    switch (cfg_old.lpbk_mode) {
                        case BCMPC_PORT_LPBK_MAC:
                            is_forced_down = true;
                            is_forced_up = false;
                            is_forced_none = true;
                            break;
                        case BCMPC_PORT_LPBK_PHY_PMD:
                        default:
                            is_forced_down = false;
                            is_forced_up = false;
                            is_forced_none = false;
                            break;
                    } /* switch */
                } /* if (cfg->lpbk_mode == BCMPC_PORT_LPBK_NONE) */
            } /* cfg->lpbk_mode != cfg_old.lpbk_mode */
        } /* !is_new */
    } /* SHR_BITGET(cfg->fbmp, BCMPC_PORT_LT_FLD_LOOPBACK_MODE) */

    if (is_forced_down) {
        /*
         * This block of code is executed, when the port link is forced,
         * this could be due to MAC loopback being enabled on a logical port.
         * If MAC loopback is enabled on the port, then following set of
         * operations are executed on sequence.
         * 1. Notify LM with the event ForceLinkDown, this event will
         *    help the LM to initiate the link down sequence on the port.
         * 2. Set the operation done bit for this port to 0.
         * 3. After notifying the LM, the execution is blocked until the
         *    operation bit is set. If the operation done bit is not
         *    with in the timeout period, error is returned.
         * 4. For initiating the link down sequence the LM will invoke the
         *    PC function call, where the per-chip link down/up sequence
         *    is stored.
         * 5. At the end of the link down sequence execution, the
         *    operation done bit is set to 1.
         * 6. The port configuration function is called for setting up the
         *    loopback in the hardware.
         */

        /*
         * Set the operation_done bit to 0 and then wait for the link DOWN
         * sequence operation to be complete.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (pmgr_port_oper_done_wait(unit, pport, 0, &status));

        /*
         * Now wait for the operation done bit to be set to 1, indicating
         * that the link DOWN sequence was successfull executed.
         * The function always returns success, the value of "status"
         * indicates if the link DOWN sequence execution was a
         * success or timed-out.
         */
        if (status != SHR_E_NONE) {
            /*
             * The operation timed-out, undo the "ForceDown"
             * LM notification by notifying a "ForceNone" event.
             */
            bcmevm_publish_event_notify(unit, "bcmpcEvLinkForceNone",
                                        port_u64);

            SHR_ERR_EXIT(SHR_E_TIMEOUT);
        }
    }

    /* Set the loopback in the hardware for the logical port. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->port_loopback_set(unit, pport, cfg));

    if (is_forced_up) {
        /*
         * Now wait for the operation done bit to be set to 1, indicating
         * that the link UP sequence was successfull executed.
         * The function always returns success, the value of "status"
         * indicates if the link UP sequence execution was a
         * success or timed-out.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (pmgr_port_oper_done_wait(unit, pport, 1, &status));

        if (status != SHR_E_NONE) {
            /*
             * The operation timed-out, undo the "ForceUp"
             * LM notification by notifying a "ForceNone" event.
             */
            bcmevm_publish_event_notify(unit, "bcmpcEvLinkForceNone",
                                        port_u64);

            SHR_ERR_EXIT(SHR_E_TIMEOUT);
        }
    }

    /* Notify ForceNone event to LM if the flag is set. */
    if (is_forced_none) {
        bcmevm_publish_event_notify(unit, "bcmpcEvLinkForceNone", port_u64);

        /* Set the forced link to none.  */
        PMGR_LINK_FORCED(unit, pport) = 0;
        PMGR_LINK_FORCED_STATUS(unit, pport) = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Apply the MAC settings provisioned in MAC_CONTROL LT settings
 *        to the hardware, when a port is added.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Physical port number.
 * \param [in] cfg Logical port configuration structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_port_mac_config_set(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                         bcmpc_port_cfg_t *cfg)
{
    int rv = SHR_E_NONE;
    bcmpc_mac_control_t mac_config, default_mac_config;
    SHR_FUNC_ENTER(unit);

    sal_memset(&mac_config, 0, sizeof(mac_config));
    sal_memset(&default_mac_config, 0, sizeof(mac_config));

    /* Perform a lookup on MAC_CONTROL to get the configuration. */
    rv = bcmpc_db_imm_entry_lookup(unit, PC_MAC_CONTROLt, P(&lport),
                                   P(&mac_config));

    /* If entry is not present then nothing to be set in the hardware. */
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    }

    /* Apply the settings to the hardware if a hardware change is required. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_mac_control_set(unit, lport, pport, &mac_config, default_mac_config));

exit:
    SHR_FUNC_EXIT();
}

static int
pmgr_egress_timestamp_mode_get(int unit, int pm_id, bcmpc_topo_t *topo_info,
                               void *cookie)
{
    bcmpc_topo_type_t topo_type_info;
    bcmpc_pport_t pport;
    bcmpc_drv_t *pc_drv = NULL;
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_pmgr_egr_timestamp_mode_t *timestamp_mode =
                        (bcmpc_pmgr_egr_timestamp_mode_t *) cookie;

    SHR_FUNC_ENTER(unit);

    /* Get Port Macro properties. */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_get(unit, topo_info->tid, &topo_type_info));

    if (!SHR_BITGET(topo_type_info.pm_feature,
                    BCMPC_FT_1588_EGRESS_TIMESTAMP)) {
        SHR_EXIT();
    }

    pport = topo_info->base_pport;

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_egress_timestamp_mode_get, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_egress_timestamp_mode_get(unit, pport,
                                                timestamp_mode));

exit:
    SHR_FUNC_EXIT();
}

static int
pmgr_egress_timestamp_mode_set(int unit, int pm_id, bcmpc_topo_t *topo_info,
                               void *cookie)
{
    bcmpc_topo_type_t topo_type_info;
    bcmpc_pport_t pport;
    bcmpc_drv_t *pc_drv = NULL;
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_pmgr_egr_timestamp_mode_t *timestamp_mode =
                        (bcmpc_pmgr_egr_timestamp_mode_t *) cookie;

    SHR_FUNC_ENTER(unit);

    /* Get Port Macro properties. */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_get(unit, topo_info->tid, &topo_type_info));

    if (!SHR_BITGET(topo_type_info.pm_feature,
                    BCMPC_FT_1588_EGRESS_TIMESTAMP)) {
        SHR_EXIT();
    }

    pport = topo_info->base_pport;

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_egress_timestamp_mode_set, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_egress_timestamp_mode_set(unit, pport,
                                                *timestamp_mode));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

void
bcmpc_pmgr_port_status_t_init(bcmpc_pmgr_port_status_t *st)
{
    sal_memset(st, 0, sizeof(*st));
}

int
bcmpc_pmgr_pm_port_enable(int unit, bcmpc_pport_t pport, int enable)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_port_enable, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_port_enable(unit, pport, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_port_cfg_set(int unit, bcmpc_pport_t pport, bcmpc_lport_t lport,
                        bcmpc_port_cfg_t *cfg, bool is_new)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_port_cfg_t cfg_old;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->port_cfg_set, SHR_E_NO_HANDLER);

    /*
     * We should not touch the hardware if the speed and num_lanes are not
     * specified.
     */
    if ((cfg->speed == 0) || (cfg->num_lanes == 0)) {
        SHR_EXIT();
    }

    bcmpc_port_cfg_t_init(&cfg_old);
    if (!is_new) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&cfg_old)));
    }

    /*
     * For the logical port configuration
     * 1. Apply the port disable operation first.
     * 2. Apply the port loopback configuration.
     * 3. Apply the port configuration setting.
     * 4. Apply the port enable operation if set.
     */

    /* Disable the port first, if set, before the port configuration. */
    if (SHR_BITGET(cfg->fbmp, BCMPC_PORT_LT_FLD_ENABLE)) {
        if (!cfg->enable) {
            SHR_IF_ERR_VERBOSE_EXIT
                (pmgr_port_enable_set(unit, pport, lport, cfg, is_new));
        }
    }

    /* Is it a loopback operation on the logical port. */
    if (SHR_BITGET(cfg->fbmp, BCMPC_PORT_LT_FLD_LOOPBACK_MODE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pmgr_port_loopback_set(unit, pport, lport, cfg, is_new));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->port_cfg_set(unit, pport, lport, cfg, is_new));

    /* LAG failover is being configured. */
    if (SHR_BITGET(cfg->fbmp, BCMPC_PORT_LT_FLD_LAG_FAILOVER)) {
        if (cfg->lag_failover != cfg_old.lag_failover) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_pmgr_failover_enable_set(unit, lport, pport,
                                                cfg->lag_failover));
        }
    }

    /* Enable the port, if set, after the port configuration is done. */
    if (SHR_BITGET(cfg->fbmp, BCMPC_PORT_LT_FLD_ENABLE)) {
        if (cfg->enable) {
            SHR_IF_ERR_VERBOSE_EXIT
                (pmgr_port_enable_set(unit, pport, lport, cfg, is_new));
        }
    }

    /*
     * If this is a new port which is being added, configure the MAC settings
     * provisioned in the PC_MAC_CONTROL LT to the hardware.
     */
    if (is_new) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pmgr_port_mac_config_set(unit, lport, pport, cfg));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_port_status_get(int unit, bcmpc_pport_t pport,
                           bcmpc_pmgr_port_status_t *st)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_lport_t lport;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->port_status_get, SHR_E_NO_HANDLER);

    sal_memset(st, 0, sizeof(*st));

    lport = bcmpc_pport_to_lport(unit, pport);
    /* Get the status from hardware. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->port_status_get(unit, lport, pport, st));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_link_status_get(int unit, bcmpc_pport_t pport,
                           bcmpc_pmgr_link_status_t *st)
{
    bcmpc_drv_t *pc_drv;
    bcmpc_pmgr_drv_t *pmgr_drv;
    int  cached_link = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));
    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->phy_link_state_get, SHR_E_NO_HANDLER);
    SHR_NULL_CHECK
        (pmgr_drv->phy_link_info_get, SHR_E_NO_HANDLER);
    SHR_NULL_CHECK
        (pmgr_drv->fault_state_get, SHR_E_NO_HANDLER);

    sal_memset(st, 0, sizeof(*st));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->fault_state_get(unit, pport, st));

    if (cached_link) {
        BCMPC_BIT_SET(*st, BCMPC_PMGR_LINK_EVENT_LINK);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->phy_link_info_get(unit, pport, st));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get link state from the hardware latched
 *  link state. This function returns the link state
 *  taking local and remote faults into consideration.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] link 1 if Link UP
 *                   0 if Link DOWN
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_pmgr_link_state_get(int unit, bcmpc_pport_t pport, int *link)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_pmgr_link_status_t st = 0;

    SHR_FUNC_ENTER(unit);

    *link = 0;

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->phy_link_state_get, SHR_E_NO_HANDLER);
    SHR_NULL_CHECK
        (pmgr_drv->fault_state_get, SHR_E_NO_HANDLER);

    /* Return port link forced state if the port is in forced state. */
    if (PMGR_LINK_FORCED(unit, pport)) {
        *link = PMGR_LINK_FORCED_STATUS(unit, pport);
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Link(%d) state(%s)\n"), pport,
             *link ? "UP" : "DOWN"));
    } else {
        /* Get the link status from hardware. */
        SHR_IF_ERR_VERBOSE_EXIT
            (pmgr_drv->phy_link_state_get(unit, pport, &st));
        SHR_IF_ERR_VERBOSE_EXIT
            (pmgr_drv->fault_state_get(unit, pport, &st));

        /* Consider PHY link state and MAC fault state. */
        if (BCMPC_BIT_GET(st, BCMPC_PMGR_LINK_EVENT_LINK) &&
            !BCMPC_BIT_GET(st, BCMPC_PMGR_LINK_EVENT_LOCAL_FAULT) &&
            !BCMPC_BIT_GET(st, BCMPC_PMGR_LINK_EVENT_REMOTE_FAULT)) {
            *link = 1;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_fault_state_get(int unit, bcmpc_pport_t pport,
                           int *rmt_fault)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_pmgr_link_status_t st = 0;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->fault_state_get, SHR_E_NO_HANDLER);

    /* Get the state from hardware. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->fault_state_get(unit, pport, &st));
    *rmt_fault = BCMPC_BIT_GET(st, BCMPC_PMGR_LINK_EVENT_REMOTE_FAULT);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_core_mode_set(int unit, bcmpc_pport_t pport,
                         bcmpc_pm_mode_t *mode)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->core_mode_set, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->core_mode_set(unit, pport, mode));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_phy_init(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg,
                    bcmpc_pm_core_cfg_t *ccfg, bool do_core_init)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->phy_init, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->phy_init(unit, pport, pcfg, ccfg, do_core_init));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_port_link_change(int unit, bcmpc_pport_t pport, int link)
{
    bcmpc_drv_t *pc_drv;
    bcmpc_cfg_profile_t *prof = NULL;
    bcmpc_lport_t lport;
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_port_cfg_t pcfg;
    bcmpc_port_cfg_t old_pcfg;
    uint32_t flovr_en = 0;
    uint32_t flovr_lpbk = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    lport = bcmpc_pport_to_lport(unit, pport);

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Link(%d) is changed to %s\n"), lport,
         link ? "UP" : "DOWN"));

    /* Is LAG failover enabled on the port. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_failover_enable_get(unit, lport, pport, &flovr_en));

    /*
     * If failover is enabled handle the following link states
     * flover_en - flover_lpbk - link -  action
     *     0     -    X        - Up   -  Call Link up seq.
     *     0     -    X        - Down -  Call Link down seq.
     *     1     -    X        - Down -  Skip link down seq.
     *     1     -    1        - Up   -  Skip link up seq,
     *                                   Remove flover_loopback
     *                                   toggle cdport_lag_state
     *     1     -    0        - Up   -  This means lag failover was enabled
     *                                   before the link came up.
     *                                   Call the link up seq.
     */
    if (flovr_en) {
        /* Get the port lag_failover loopback status. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_failover_loopback_get(unit, pport, &flovr_lpbk));
    }

    /*
     * Notify the TM about the port configuration, when
     * the port is up. This is required because auto-negotiation
     * could have been enabled on the port, when resolved the
     * speed of the port could be different from the configured
     * speed. Irrespective of whether auto-negotiation is enabled
     * on a port, notify the TM about the port configuration.
     */
    if (link && (!flovr_lpbk)) {
        bcmpc_port_cfg_t_init(&pcfg);
        bcmpc_port_cfg_t_init(&old_pcfg);

        PMGR_DRV_GET(unit, pport, pmgr_drv);
        pcfg.pport = pport;

        if (bcmdrd_feature_is_real_hw(unit)) {
            SHR_NULL_CHECK
                (pmgr_drv->port_speed_get, SHR_E_NO_HANDLER);

            /* Get the speed parameters from the hardware. */
            SHR_IF_ERR_VERBOSE_EXIT
                (pmgr_drv->port_speed_get(unit, lport, &pcfg));
            /* Assign physical port to port configuration. */
            old_pcfg.pport = pport;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_tm_port_update(unit, lport, &old_pcfg, &pcfg));
        }
    }

    /* If LAG failover is not enabled, call the link p or down sequence. */
    if (!flovr_en) {
        if (link == 0) {
            prof = pc_drv->port_down_prof_get(unit, pport);
        } else {
            prof = pc_drv->port_up_prof_get(unit, pport);
        }
    } else {
        /* Lag failover is enabled. Skip link down event. */
        if (link == 1) {
            /* Link is up, check lag_failover_loopback state. */
            if (!flovr_lpbk) {
                prof = pc_drv->port_up_prof_get(unit, pport);
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpc_pmgr_failover_loopback_remove(unit, pport));
            }
        }
    }

    if (!prof) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_profile_exec(unit, pport, prof));

exit:
    if (!SHR_FUNC_ERR()) {
        /*  Set the opeartion_done, if the port link is in forced mode. */
        if (PMGR_LINK_FORCED(unit, pport)) {
            LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Link(%d) is changed to %s\n"), lport,
                                link ? "UP" : "DOWN"));

            /* Update the operation_done bit for this port. */
            PMGR_PORT_OPER_DONE(unit, pport) = 1;
        }
        /* Update the link state in the PC local database. */
        PMGR_LINK_STATE(unit, pport) = link;
    }
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_port_suspend_set(int unit, bcmpc_pport_t pport, bool enable)
{
    bcmpc_drv_t *pc_drv;
    bcmpc_cfg_profile_t *prof;

    SHR_FUNC_ENTER(unit);

    /* Do nothing if link is down. */
    if (!PMGR_LINK_STATE(unit, pport)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    if (enable) {
        prof = pc_drv->port_suspend_prof_get(unit, pport);
    } else {
        prof = pc_drv->port_resume_prof_get(unit, pport);
    }

    if (!prof) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_profile_exec(unit, pport, prof));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_failover_loopback_get(int unit, bcmpc_pport_t pport,
                                 uint32_t *failover_loopback)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->failover_loopback_get, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->failover_loopback_get(unit, pport, failover_loopback));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_failover_enable_set(int unit, bcmpc_lport_t lport,
                               bcmpc_pport_t pport, uint32_t enable)
{
    bcmpc_drv_t *pc_drv;
    bcmpc_cfg_profile_t *prof = NULL;
    bcmpc_pmgr_drv_t *pmgr_drv;
    uint32_t failover_lpbk_status = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->failover_enable_set, SHR_E_NO_HANDLER);

    /*
     * When LAG failover is enabled on a port, when the port link
     * goes DOWN the hardware automatically enables LAG failover
     * loopback switching the traffic on that port back into the
     * switch. The LM detects the port link DOWN and intiates the
     * link DOWN sequence in PC component. PC component skips the
     * execution of link DOWN sequence if LAG failover is enabled
     * on the port and returns success to LM.
     * When the port link goes UP, LM detects and initiates the
     * port link UP sequence in PC component. PC component skips
     * the link UP sequence if the port is in LAG failover loopback
     * as the link DOWN sequence was not executed on this port
     * and only removes the LAG failover loopback on the port.
     *
     * Now if the application is disabling the LAG failover while
     * port is LAG failover loopback, then the LAG failover loopback
     * should be removed and also importantly run the link DOWN
     * since the link DOWN was not executed when the port switched
     * to LAG failover loopback. The block of code below takes care
     * of this requirement.
     */
    if (!enable) {
        /* Get the port lag_failover loopback status. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_failover_loopback_get(unit, pport,
                                              &failover_lpbk_status));

        /*
         * LAG failover loopback is enabled on the port, run the link DOWN
         * sequence as LAG failover is being disabled.
         */
        if (failover_lpbk_status) {
            prof = pc_drv->port_down_prof_get(unit, pport);
        }

        /* Execute the link DOWN sequence. */
        if (prof) {
            SHR_IF_ERR_VERBOSE_EXIT
                (pmgr_profile_exec(unit, pport, prof));
        }

        /* Remove the LAG failover loopback on the port. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_failover_loopback_remove(unit, pport));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->failover_enable_set(unit, pport, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_failover_enable_get(int unit, bcmpc_lport_t lport,
                               bcmpc_pport_t pport,
                               uint32_t *enable)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->failover_enable_get, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->failover_enable_get(unit, pport, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_failover_loopback_remove(int unit, bcmpc_pport_t pport)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->failover_loopback_remove, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->failover_loopback_remove(unit, pport));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_pm_port_attach(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg,
                          bcmpc_pm_core_cfg_t *ccfg, int enable)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pcfg->pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->port_attach, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->port_attach(unit, lport, pcfg, ccfg, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_pm_port_config_validate(int unit, bcmpc_port_cfg_t *pcfg,
                                   int is_first)
{
    int is_new = 0;
    bcmpc_lport_t lport;
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pcfg->pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_port_config_validate, SHR_E_NO_HANDLER);

    lport = bcmpc_pport_to_lport(unit, pcfg->pport);
    /* Check if the port already added or not. */
    if (lport == BCMPC_INVALID_LPORT) {
        is_new = 1;
    }

    /*
     * Function pm_vcos_validation need to read HW register to get current
     * PLL_diver value.
     * During simulation/emulation, if PHYSIM is not enabled,
     * the Serdes dispatch type is phymodDispatchTypeNull,
     * so the validation can not be done.
     * As a result, we bypass this function call if we are running
     * simulation/emulation without PHYSIM enabled.
     */
    if (bcmdrd_feature_is_real_hw(unit) ||
        bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pmgr_drv->pm_port_config_validate(unit, pcfg, is_first, is_new));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_pm_port_autoneg_advert_set(int unit, bcmpc_lport_t lport,
                                      bcmpc_autoneg_prof_t *an_cfg,
                                      bcmpc_port_cfg_t *pcfg)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pcfg->pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->port_autoneg_advert_set, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->port_autoneg_advert_set(unit, lport, an_cfg, pcfg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_mac_control_set(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                          bcmpc_mac_control_t *mctrl,
                          bcmpc_mac_control_t default_mctrl)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_mac_control_set, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_mac_control_set(unit, lport, pport, mctrl, default_mctrl));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_mac_control_get(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                           bcmpc_mac_control_t *mctrl)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_mac_control_get, SHR_E_NO_HANDLER);

    /* Get the status from hardware. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_mac_control_get(unit, lport, pport, mctrl));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_phy_control_set(int unit, bcmpc_port_lane_t *port_lane,
                           bcmpc_pport_t pport, bcmpc_phy_control_t *pctrl)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_phy_control_set, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_phy_control_set(unit, port_lane, pport, pctrl));

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_pmgr_phy_status_t_init(bcmpc_pmgr_phy_status_t *pst)
{
    sal_memset(pst, 0, sizeof(*pst));
}

int
bcmpc_pmgr_phy_status_get(int unit, bcmpc_port_lane_t *port_lane,
                          bcmpc_pmgr_phy_status_t *pst)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_pport_t pport;

    SHR_FUNC_ENTER(unit);

    pport = bcmpc_lport_to_pport(unit, port_lane->lport);
    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_phy_status_get, SHR_E_NO_HANDLER);

    sal_memset(pst, 0, sizeof(*pst));

    /* Get the status from hardware. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_phy_status_get(unit, port_lane, pport, pst));

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_pmgr_tx_taps_status_t_init(bcmpc_pmgr_tx_taps_status_t *taps_st)
{
    sal_memset(taps_st, 0, sizeof(*taps_st));
}

int
bcmpc_pmgr_tx_taps_status_get(int unit, bcmpc_port_lane_t *port_lane,
                              bcmpc_pmgr_tx_taps_status_t *taps_st)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_pport_t pport;

    SHR_FUNC_ENTER(unit);

    pport = bcmpc_lport_to_pport(unit, port_lane->lport);
    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_tx_taps_status_get, SHR_E_NO_HANDLER);

    sal_memset(taps_st, 0, sizeof(*taps_st));

    /* Get the status from hardware. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_tx_taps_status_get(unit, port_lane, pport, taps_st));

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_pmgr_pmd_firmware_status_t_init(bcmpc_pmgr_pmd_firmware_status_t *fm_st)
{
    sal_memset(fm_st, 0, sizeof(*fm_st));
}

int
bcmpc_pmgr_pmd_firmware_status_get(int unit, bcmpc_lport_t lport,
                                   bcmpc_pmgr_pmd_firmware_status_t *fm_st)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_pport_t pport;

    SHR_FUNC_ENTER(unit);

    pport = bcmpc_lport_to_pport(unit, lport);
    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_pmd_firmware_status_get, SHR_E_NO_HANDLER);

    sal_memset(fm_st, 0, sizeof(*fm_st));

    /* Get the status from hardware. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_pmd_firmware_status_get(unit, lport, pport, fm_st));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_tx_taps_config_set(int unit, bcmpc_port_lane_t *port_lane,
                              bcmpc_tx_taps_config_t *tx_cfg)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_pport_t pport;

    SHR_FUNC_ENTER(unit);

    pport = bcmpc_lport_to_pport(unit, port_lane->lport);
    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_tx_taps_config_set, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_tx_taps_config_set(unit, port_lane, pport, tx_cfg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_pmd_firmware_config_set(int unit, bcmpc_lport_t lport,
                                   bcmpc_pmd_firmware_config_t *fm_cfg)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_pport_t pport;

    SHR_FUNC_ENTER(unit);

    pport = bcmpc_lport_to_pport(unit, lport);

    /* Return if logical to physical port map unavailable. */
    if (pport == BCMPC_INVALID_PPORT) {
        SHR_EXIT();
    }

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_pmd_firmware_config_set, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_pmd_firmware_config_set(unit, lport, pport, fm_cfg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_pfc_control_set(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                          bcmpc_pfc_control_t *mctrl)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_pfc_set, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_pfc_set(unit, lport, pport, mctrl));

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_pmgr_pfc_status_t_init(bcmpc_pmgr_pfc_status_t *pst)
{
    sal_memset(pst, 0, sizeof(*pst));
}

int
bcmpc_pmgr_pfc_status_get(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                          bcmpc_pmgr_pfc_status_t *pst)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_pfc_get, SHR_E_NO_HANDLER);

    sal_memset(pst, 0, sizeof(*pst));

    /* Get the status from hardware. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_pfc_get(unit, lport, pport, pst));

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_pmgr_port_abilities_t_init(bcmpc_pmgr_port_abilities_t *pability)
{
    sal_memset(pability, 0, sizeof(*pability));
}

int
bcmpc_pmgr_port_abilities_get(int unit, bcmpc_lport_t lport,
                                  bcmpc_pport_t pport,
                                  pm_port_ability_type_t type,
                                  bcmpc_pmgr_port_abilities_t *port_abilities)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_drv_t *pc_drv = NULL;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_port_abilities_get, SHR_E_NO_HANDLER);

    sal_memset(port_abilities, 0, sizeof(*port_abilities));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_port_abilities_get(unit, lport, pport, type,
                                         port_abilities));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    /* Check if the device needs to update port local abilities. */
    if (pc_drv->dev_unsupported_abilities_get &&
                                             (type == PM_PORT_ABILITY_LOCAL)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pmgr_port_abilities_update(unit, pport, port_abilities));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_pm_port_detach(int unit, bcmpc_lport_t lport)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_pport_t pport;

    SHR_FUNC_ENTER(unit);
    pport = bcmpc_lport_to_pport(unit, lport);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_port_detach, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_port_detach(unit, pport));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_pm_tsc_power_on(int unit, bcmpc_pport_t pport)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_tsc_power_on, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_tsc_power_on(unit, pport));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_pm_vco_rate_get(int unit, bcmpc_pport_t pport,
                            bcmpc_vco_type_t *vco_rate)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_vco_rate_get, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_vco_rate_get(unit, pport, vco_rate));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_timesync_set(int unit, bcmpc_lport_t lport,
                             bcmpc_pport_t pport,
                             bcmpc_port_timesync_t *config)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_timesync_set, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_timesync_set(unit, lport, pport, config));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_port_sw_state_update(int unit, bcmpc_pport_t pport)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_port_sw_state_update, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_port_sw_state_update(unit, pport));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_pm_init(int unit, bcmpc_pport_t pport)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_init, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_init(unit, pport));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_port_rlm_status_get(int unit, bcmpc_pport_t pport,
                               bcmpc_pmgr_port_rlm_status_t *status)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->port_rlm_status_get, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->port_rlm_status_get(unit, pport, status));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_pm_core_attach(int unit, bcmpc_pport_t pport,
                          pm_vco_setting_t *vco_cfg, bcmpc_port_cfg_t *pcfg,
                          bcmpc_pm_core_cfg_t *ccfg)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_core_attach, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_core_attach(unit, pport, vco_cfg, pcfg, ccfg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_pm_vco_rate_calculate(int unit, bcmpc_pport_t pport,
                                  pm_vco_setting_t *vco_select)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_vco_rate_calculate, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_vco_rate_calculate(unit, pport, vco_select));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Populate the PC_PORT_INFOt LT with the operation status errors.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Physical port number.
 * \param [in] op_st Operation status returned for PM driver call.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_oper_status_table_update(int unit, bcmpc_lport_t lport,
                                    bcmpc_pport_t pport,
                                    bcmpc_port_oper_status_t *op_st)
{
    bcmpc_port_oper_status_t committed;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    sal_memset(&committed, 0, sizeof(bcmpc_port_oper_status_t));

    rv = (bcmpc_db_imm_entry_lookup(unit, PC_PORT_INFOt,
                                    sizeof(bcmpc_lport_t), &lport,
                                    sizeof(bcmpc_port_oper_status_t),
                                    &committed));

    if (rv == SHR_E_NOT_FOUND) {
        rv = (bcmpc_db_imm_entry_insert(unit, PC_PORT_INFOt,
                                        sizeof(bcmpc_lport_t), &lport,
                                        sizeof(bcmpc_port_oper_status_t),
                                        op_st));
    } else {
        rv = (bcmpc_db_imm_entry_update(unit, PC_PORT_INFOt,
                                        sizeof(bcmpc_lport_t), &lport,
                                        sizeof(bcmpc_port_oper_status_t),
                                        op_st));
    }
    SHR_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_port_enable_set(int unit, bcmpc_pport_t pport,
                           bcmpc_lport_t lport, uint32_t enable)
{
    bcmpc_port_cfg_t cfg;
    bool is_new = false;
    SHR_FUNC_ENTER(unit);

    if ((lport == BCMPC_INVALID_LPORT) || (pport == BCMPC_INVALID_PPORT)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    bcmpc_port_cfg_t_init(&cfg);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&cfg)));


    /* Set field to indicate port enable operation. */
    SHR_BITSET(cfg.fbmp, BCMPC_PORT_LT_FLD_ENABLE);
    cfg.enable = enable;

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_port_enable_set(unit, pport, lport, &cfg, is_new));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Get the TSC Core TX,RX polarity and TX/TX lane map settings.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] core_config TSC Core cofiguration data structure,
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_pm_core_config_get(int unit, bcmpc_pport_t pport,
                              bcmpc_pm_core_cfg_t *ccfg)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_core_config_get, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_core_config_get(unit, pport, ccfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Recover forced link state.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] port cofiguration data structure,
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_forced_link_state_recover(int unit, bcmpc_pport_t pport,
                                     bcmpc_port_cfg_t *pcfg)
{
    if ((pport == BCMPC_INVALID_PPORT) || (pport >= BCMDRD_CONFIG_MAX_PORTS)){
         LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Physical port %d is beyond the range\n"),
                                  pport));
         return SHR_E_PARAM;
    }
    if (pcfg->lpbk_mode == PM_PORT_LPBK_MAC) {
        PMGR_LINK_FORCED(unit, pport) = 1;
        if (pcfg->enable == 1) {
            /* force link UP */
            PMGR_LINK_STATE(unit, pport) = 1;
            PMGR_LINK_FORCED_STATUS(unit, pport) = 1;
        } else {
            /* force link DOWN */
            PMGR_LINK_STATE(unit, pport) = 0;
            PMGR_LINK_FORCED_STATUS(unit, pport) = 0;
        }
    }

    return SHR_E_NONE;
}

/*!
 * Set HA link state
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_ha_link_state_set(int unit)
{
    uint32_t i = 0;

    /* store PMGR_LINK_STATE into HA mem */
    for (i = 0; i < sizeof(pmgr_ha_link_state[unit]->link_state); i++) {
        pmgr_ha_link_state[unit]->link_state[i] = PMGR_LINK_STATE(unit, i);
    }

    return SHR_E_NONE;
}

/*!
 * Recover from HA link state
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_link_state_recover(int unit)
{
    uint32_t i = 0;

    /* recover PMGR_LINK_STATE from HA mem */
    for (i = 0; i < sizeof(pmgr_ha_link_state[unit]->link_state); i++) {
        PMGR_LINK_STATE(unit, i) = pmgr_ha_link_state[unit]->link_state[i];
    }

    return SHR_E_NONE;
}

/*!
 * Create HA link state buffer
 *
 * \param [in] unit Unit number.
 * \param [in] warm warmboot flag.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_ha_link_state_create(int unit, bool warm)
{
    uint32_t ha_alloc_size = 0, ha_req_size = 0;

    SHR_FUNC_ENTER(unit);

    /* Initialize link state specific HA information. */
    ha_req_size = sizeof(bcmpc_ha_link_state_info_t);
    ha_alloc_size = ha_req_size;

    /* alloc for HA link state info */
    pmgr_ha_link_state[unit] = shr_ha_mem_alloc(unit,
                                             BCMMGMT_PC_COMP_ID,
                                             BCMPC_HA_PORT_LINK_STATE_MODULE_ID,
                                             "bcmpcHaLinkStateInfo",
                                             &ha_alloc_size);

    SHR_NULL_CHECK(pmgr_ha_link_state[unit], SHR_E_MEMORY);

    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    if (!warm) {
        /* initialize HA link state with 0x0 for cold boot */
        sal_memset(&pmgr_ha_link_state[unit]->link_state, 0, ha_alloc_size);
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_PC_COMP_ID,
                                        BCMPC_HA_PORT_LINK_STATE_MODULE_ID,
                                        0, ha_req_size, 1,
                                        BCMPC_HA_LINK_STATE_INFO_T_ID));
    } else {
        /* recover PMGR_LINK_STATE during warmboot */
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_link_state_recover(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_port_tx_timestamp_info_get(int unit, bcmpc_lport_t lport,
                                 bcmpc_pmgr_port_timestamp_t *timestamp_info)
{
    bcmpc_drv_t *pc_drv = NULL;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bcmpc_pmgr_drv_t *pmgr_drv;
    bool is_internal = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Check if logical to physical port map exists. */
    pport = bcmpc_lport_to_pport(unit, lport);

    /* Return error is no logical to physical port map present. */
    if (pport == BCMPC_INVALID_PPORT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    /* Check if port is internal. */
    if (pc_drv && pc_drv->is_internal_port) {
        SHR_IF_ERR_EXIT
            (pc_drv->is_internal_port(unit, pport, &is_internal));
    }

    if (is_internal) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->port_tx_timestamp_info_get, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->port_tx_timestamp_info_get(unit, lport, pport,
                                              timestamp_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_egress_timestamp_mode_get(int unit,
                              bcmpc_pmgr_egr_timestamp_mode_t *timestamp_mode)
{
    SHR_FUNC_ENTER(unit);

    /*
     * Get egress timestamp from the first Port Macro which
     * supports the setting.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_traverse(unit, pmgr_egress_timestamp_mode_get,
                             timestamp_mode));
exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_egress_timestamp_mode_set(int unit,
                              bcmpc_pmgr_egr_timestamp_mode_t timestamp_mode)
{
    SHR_FUNC_ENTER(unit);

    /*
     * Set egress timestamp from the first Port Macro which
     * supports the setting.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_traverse(unit, pmgr_egress_timestamp_mode_set,
                             &timestamp_mode));
exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_fdr_control_set(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                           bcmpc_fdr_port_control_t *fdr_ctrl)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_port_fdr_control_set, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_port_fdr_control_set(unit, lport, pport, fdr_ctrl));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_fdr_stats_get(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                         bcmpc_fdr_port_stats_t *fdr_stats)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_port_fdr_stats_get, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_port_fdr_stats_get(unit, lport, pport, fdr_stats));

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_pmgr_port_diag_stats_t_init(bcmpc_pmgr_port_diag_stats_t *diag_stats)
{
    sal_memset(diag_stats, 0, sizeof(*diag_stats));
}

int
bcmpc_pmgr_port_diag_stats_get(int unit, bcmpc_lport_t lport,
                              bcmpc_pmgr_port_diag_stats_t *diag_stats)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_pport_t pport;

    SHR_FUNC_ENTER(unit);

    pport = bcmpc_lport_to_pport(unit, lport);
    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_port_diag_stats_get, SHR_E_NO_HANDLER);

    sal_memset(diag_stats, 0, sizeof(*diag_stats));

    /* Get the status from hardware. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_port_diag_stats_get(unit, pport, diag_stats));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_interrupt_process(int unit, bcmpc_lport_t lport,
                             bcmpc_intr_type_t intr_type,
                             uint32_t *is_handled)
{
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    /* Check if logical to physical port map exists. */
    pport = bcmpc_lport_to_pport(unit, lport);

    /* Return error is no logical to physical port map present. */
    if (pport == BCMPC_INVALID_PPORT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_port_interrupt_process, SHR_E_NO_HANDLER);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmgr_drv->pm_port_interrupt_process(unit, lport, pport,
                                             intr_type, is_handled));

exit:
    SHR_FUNC_EXIT();
}

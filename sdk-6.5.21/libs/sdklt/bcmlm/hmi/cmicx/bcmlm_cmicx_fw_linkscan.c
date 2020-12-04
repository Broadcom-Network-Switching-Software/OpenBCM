/*! \file bcmlm_cmicx_fw_linkscan.c
 *
 * Link Manager CMICx firmware linkscan handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>

#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>

#include <bcmlm/bcmlm_drv_internal.h>
#include <bcmlm/bcmlm_fw_linkscan.h>

#include <bcmdrd/bcmdrd_feature.h>

#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_m0ssq.h>
#include <bcmbd/bcmbd_m0ssq_mbox.h>

/*******************************************************************************
 * Local definitions.
 */

/* Debug log target definition. */
#define BSL_LOG_MODULE  BSL_LS_BCMLM_DRV

/* uC number for firmware linkscan. */
#define FWLINKSCAN_UC   0

/* MBOX message id for firmware linkscan. */
typedef enum ls_msgtype_e {

    /* Configure ports which enable firmware linkscan. */
    LS_HW_CONFIG = 1,

    /* Check if FW is alive by getting FW version. */
    LS_HW_HEARTBEAT,

    /* Pause firmware linkscan. */
    LS_PAUSE,

    /* Continue/Start firmware linkscan. */
    LS_CONTINUE,

    /* Event to notify link status change. */
    LS_LINK_STATUS_CHANGE

} ls_msgtype_t;

/* Physical port bit map for FW linkscan MBOX communication. */
#define LS_PHY_PBMP_PORT_MAX     384
#define LS_PHY_PBMP_WORD_MAX     ((LS_PHY_PBMP_PORT_MAX + 31) / 32)
typedef struct ls_phy_pbmp_s {
    uint32_t  pbits[LS_PHY_PBMP_WORD_MAX];
} ls_phy_pbmp_t;

/* Minimun physical port bit map. */
#define MIN_PBMP_SIZE  (sizeof(ls_phy_pbmp_t) > sizeof(bcmdrd_pbmp_t) ? \
                        sizeof(bcmdrd_pbmp_t) : sizeof(ls_phy_pbmp_t))

/* Condition to bypass firmware linkscan driver. */
#define FW_LINKSCAN_BYPASS_CHK(unit)   if (!bcmdrd_feature_is_real_hw(unit)) { \
                                           SHR_EXIT(); \
                                       }

/*******************************************************************************
 * Local variables.
 */

/* Linkscan firmware object */
static bcmlm_fw_drv_t bcmlm_fw_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Internal Functions.
 */
static int
cmicx_fw_linkscan_msg_handler(int unit,
                              bcmbd_m0ssq_mbox_msg_t *msg,
                              bcmbd_m0ssq_mbox_resp_t *resp)
{
    int size = MIN_PBMP_SIZE;
    bcmdrd_pbmp_t link_pbmp;
    bcmlm_fw_drv_t *fw_drv = &bcmlm_fw_drv[unit];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(msg, SHR_E_FAIL);

    switch (msg->id) {
    case LS_LINK_STATUS_CHANGE:

        /* Clear link pbmp. */
        BCMDRD_PBMP_CLEAR(link_pbmp);

        /* Update cached link pbmp. */
        sal_spinlock_lock(fw_drv->lock);
        sal_memcpy(&link_pbmp, msg->data, size);
        BCMDRD_PBMP_ASSIGN(fw_drv->link_stat, link_pbmp);
        sal_spinlock_unlock(fw_drv->lock);

        /* Notify LM state changed. */
        if (fw_drv->intr_func) {
            fw_drv->intr_func(fw_drv->unit, 0);
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Link Change\n")));
        break;
    default:
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Linkscan FW unknown message\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_fw_linkscan_init(int unit)
{
    bcmlm_fw_drv_t *fw_drv = (bcmlm_fw_drv_t *)&bcmlm_fw_drv[unit];
    uint32_t mbox_id, uc = FWLINKSCAN_UC;

    SHR_FUNC_ENTER(unit);

    FW_LINKSCAN_BYPASS_CHK(unit);

    if (fw_drv->fw_init) {
        SHR_EXIT();
    }

    /* Allocate a mbox for Linkscan. */
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_mbox_alloc(unit, "linkscan", &mbox_id));

    /* Download and start linkscan firmware. */
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_m0ssq_fw_linkscan_init(unit, uc));

    /* Install linkscan rx msg handler. */
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_mbox_msg_handler_set(unit, mbox_id,
                                          cmicx_fw_linkscan_msg_handler));

    /* Attach linkscan MBOX to uC0's swintr. */
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_mbox_uc_swintr_attach(unit, mbox_id, uc));

    fw_drv->unit = unit;
    fw_drv->mbox_id = mbox_id;
    fw_drv->fw_init = 1;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Linkscan FW init\n")));

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_fw_linkscan_cleanup(int unit)
{
    bcmlm_fw_drv_t *fw_drv = (bcmlm_fw_drv_t *)&bcmlm_fw_drv[unit];
    uint32_t uc = FWLINKSCAN_UC;

    SHR_FUNC_ENTER(unit);

    FW_LINKSCAN_BYPASS_CHK(unit);

    if (!fw_drv->fw_init) {
        SHR_EXIT();
    }

    /* Disable a uC software interrupt */
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_uc_swintr_enable_set(unit, uc, 0));

    /* Free mailbox resource. */
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_mbox_free(unit, fw_drv->mbox_id));

    fw_drv->fw_init = 0;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Linkscan FW cleanup\n")));

exit:
    SHR_FUNC_EXIT();

}

static int
cmicx_fw_linkscan_config(int unit, bcmdrd_pbmp_t pbm)
{
    bcmlm_fw_drv_t *fw_drv = (bcmlm_fw_drv_t *)&bcmlm_fw_drv[unit];
    bcmbd_m0ssq_mbox_msg_t msg;
    bcmbd_m0ssq_mbox_resp_t resp;
    ls_phy_pbmp_t ls_pbmp, ls_link_pbmp;
    int size = MIN_PBMP_SIZE;

    SHR_FUNC_ENTER(unit);

    FW_LINKSCAN_BYPASS_CHK(unit);

    if (!fw_drv->fw_init) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&ls_pbmp, 0, sizeof(ls_pbmp));
    sal_memcpy(&ls_pbmp, &pbm, size);

    msg.id = LS_HW_CONFIG;
    msg.datalen = sizeof(ls_phy_pbmp_t);
    msg.data = (uint32_t *) &ls_pbmp;
    resp.datalen = sizeof(ls_phy_pbmp_t);
    resp.data = (uint32_t *) &ls_link_pbmp;

    /* Send "configure" message to setup valid port bit map. */
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_mbox_msg_send(unit, fw_drv->mbox_id, &msg, &resp));

    /* Response contains current link status.
     * Copy the response (link status) to cached link status.
     */
    sal_memset(&pbm, 0, sizeof(bcmdrd_pbmp_t));
    sal_memcpy(&pbm, resp.data, size);
    BCMDRD_PBMP_ASSIGN(fw_drv->link_stat, pbm);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Linkscan FW config\n")));

exit:
    SHR_FUNC_EXIT();

}

static int
cmicx_fw_linkscan_link_get(int unit, bcmdrd_pbmp_t *pbm)
{
    bcmlm_fw_drv_t *fw_drv = (bcmlm_fw_drv_t *)&bcmlm_fw_drv[unit];

    SHR_FUNC_ENTER(unit);

    FW_LINKSCAN_BYPASS_CHK(unit);

    /* Get firmware link status from cached link status. */
    sal_spinlock_lock(fw_drv->lock);
    BCMDRD_PBMP_ASSIGN(*pbm, fw_drv->link_stat);
    sal_spinlock_unlock(fw_drv->lock);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Linkscan FW states get\n")));

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_fw_linkscan_intr_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_fw_linkscan_intr_func_set(int unit, bcmbd_intr_f intr_func)
{
    bcmlm_fw_drv_t *fw_drv = (bcmlm_fw_drv_t *)&bcmlm_fw_drv[unit];

    SHR_FUNC_ENTER(unit);

    fw_drv->intr_func = intr_func;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_fw_linkscan_stop(int unit)
{
    bcmlm_fw_drv_t *fw_drv = (bcmlm_fw_drv_t *)&bcmlm_fw_drv[unit];
    bcmbd_m0ssq_mbox_msg_t msg;
    bcmbd_m0ssq_mbox_resp_t resp;
    uint32_t ret, uc = FWLINKSCAN_UC;

    SHR_FUNC_ENTER(unit);

    FW_LINKSCAN_BYPASS_CHK(unit);

    if (!fw_drv->fw_init) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Disable a uC software interrupt */
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_uc_swintr_enable_set(unit, uc, 0));

    msg.id = LS_PAUSE;
    msg.datalen = sizeof(ret);
    msg.data = &ret;
    resp.datalen = 0;
    resp.data = &ret;

    /* Send "pause" message to FW. */
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_mbox_msg_send(unit, fw_drv->mbox_id, &msg, &resp));

    /* Purge the mailbox. */
    bcmbd_m0ssq_mbox_process_recv_msgs(unit, fw_drv->mbox_id);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Linkscan FW stop\n")));

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_fw_linkscan_start(int unit)
{
    bcmlm_fw_drv_t *fw_drv = (bcmlm_fw_drv_t *)&bcmlm_fw_drv[unit];
    bcmbd_m0ssq_mbox_msg_t msg;
    bcmbd_m0ssq_mbox_resp_t resp;
    uint32_t ret, uc = FWLINKSCAN_UC;

    SHR_FUNC_ENTER(unit);

    FW_LINKSCAN_BYPASS_CHK(unit);

    if (!fw_drv->fw_init) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Enable a uC software interrupt */
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_uc_swintr_enable_set(unit, uc, 1));

    msg.id = LS_CONTINUE;
    msg.datalen = sizeof(ret);
    msg.data = &ret;
    resp.datalen = 0;
    resp.data = &ret;

    /* Send "continue" message to FW. */
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_mbox_msg_send(unit, fw_drv->mbox_id, &msg, &resp));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Linkscan FW start\n")));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions.
 */
int
bcmlm_cmicx_fw_linkscan_drv_attach(int unit, bcmlm_drv_t *drv)
{
    bcmlm_fw_drv_t *fw_drv = (bcmlm_fw_drv_t *)&bcmlm_fw_drv[unit];

    fw_drv->lock = sal_spinlock_create("bcmlm_fw_stat_lock");
    if (!fw_drv->lock) {
        return SHR_E_MEMORY;
    }

    drv->hw_init = cmicx_fw_linkscan_init;
    drv->hw_cleanup = cmicx_fw_linkscan_cleanup;
    drv->hw_config = cmicx_fw_linkscan_config;
    drv->hw_link_get = cmicx_fw_linkscan_link_get;
    drv->hw_intr_clear = cmicx_fw_linkscan_intr_clear;
    drv->hw_intr_cb_set = cmicx_fw_linkscan_intr_func_set;
    drv->hw_scan_stop = cmicx_fw_linkscan_stop;
    drv->hw_scan_start = cmicx_fw_linkscan_start;

    return SHR_E_NONE;
}

int
bcmlm_cmicx_fw_linkscan_drv_detach(int unit, bcmlm_drv_t *drv)
{
    bcmlm_fw_drv_t *fw_drv = (bcmlm_fw_drv_t *)&bcmlm_fw_drv[unit];

    if (fw_drv->lock) {
        sal_spinlock_destroy(fw_drv->lock);
        fw_drv->lock = NULL;
    }

    return SHR_E_NONE;
}

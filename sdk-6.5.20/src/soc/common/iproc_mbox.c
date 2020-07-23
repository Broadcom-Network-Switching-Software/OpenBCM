/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: Host <-> M0SSQ communcation protocol
 */
#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <shared/alloc.h>
#include <shared/bsl.h>

#if defined(BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
#include <soc/drv.h>
#include <soc/cm.h>
#include <soc/cmic.h>
#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/iproc_fwconfig.h>

#define soc_iproc_mbox_write(unit, offset, val)    soc_m0ssq_sram_write32(unit, offset, val)
#define soc_iproc_mbox_read(unit, offset)          soc_m0ssq_sram_read32(unit, offset)

int soc_iproc_mbox_size_get(int unit, int ucnum)  {
    return M0SSQ_SHMEM_MBOX_SIZE;
}

int soc_iproc_mbox_base_get(int unit, int ucnum)  {
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        return IPROC_M0SSQ_SRAM_BASE_HX5 + M0SSQ_SRAM_FWREG_SIZE + (ucnum * soc_iproc_mbox_size_get(unit, ucnum));
    } else {
        return IPROC_M0SSQ_SRAM_BASE + M0SSQ_SRAM_FWREG_SIZE + (ucnum * soc_iproc_mbox_size_get(unit, ucnum));
    }
}

void _soc_iproc_fw_config(int unit)
{
    SOC_CONTROL(unit)->iproc_mbox_config.max_ucores = MAX_UCORES;
    SOC_CONTROL(unit)->iproc_mbox_config.nucores_enabled = MAX_UCORES_ENABLED;

    SOC_CONTROL(unit)->iproc_mbox_config.max_mboxs[0] = U0_MAX_MBOXS;
    SOC_CONTROL(unit)->iproc_mbox_config.max_mboxs[1] = U1_MAX_MBOXS;
    SOC_CONTROL(unit)->iproc_mbox_config.max_mboxs[2] = U2_MAX_MBOXS;
    SOC_CONTROL(unit)->iproc_mbox_config.max_mboxs[3] = U3_MAX_MBOXS;
}

int _soc_iproc_num_mbox_get(int unit, int ucnum)
{
    return SOC_CONTROL(unit)->iproc_mbox_config.max_mboxs[ucnum];
}

int _soc_iproc_num_ucore_get(int unit)
{
    return SOC_CONTROL(unit)->iproc_mbox_config.nucores_enabled;
}

/*
 * Function:
 *      soc_iproc_msgintr_handler
 * Purpose:
 *      Checks the mail box for messages ans call the registered handler.
 * Parameters:
 *      param - Parameter registered at the time of initialization.
 * Returns:
 *      SOC_E_NONE on success
 * NOTE:
 *      soc_iproc_msgintr_handler is added as a timer event handler when NVIC support is not enabled.
 */
int soc_iproc_msgintr_handler(int unit, void *param)
{
    int id, num_ids = 0, start_id = 0, end_id = 0;
    soc_iproc_mbox_info_t *chan;
    soc_iproc_mbox_ptr_t *mbox;
    soc_control_t   *soc = SOC_CONTROL(unit);
    uint32 max_ucores, ucnum;

    max_ucores = _soc_iproc_num_ucore_get(unit);

    for (ucnum = 0; ucnum < max_ucores; ucnum++) {
        num_ids = _soc_iproc_num_mbox_get(unit, ucnum);
        end_id = start_id + num_ids;
        for (id = start_id; id < end_id; id++) {
            if (soc->iproc_mbox_info[id][IPROC_MBOX_TYPE_RX].inuse == 0)
                continue;

            chan = &soc->iproc_mbox_info[id][IPROC_MBOX_TYPE_RX];
            mbox = IPROC_MBOX_PTR(chan);

            if (IPROC_MBOX_EMPTY(chan, mbox))
                continue;

            LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "Received iproc mbox %d message\n"),id));
            soc->stat.m0_intr++;

            if (soc->iproc_mbox_info[id][IPROC_MBOX_TYPE_RX].msg_handler != NULL)
                soc->iproc_mbox_info[id][IPROC_MBOX_TYPE_RX].msg_handler(soc->iproc_mbox_info[id][IPROC_MBOX_TYPE_RX].msg_handler_param);
        }
        start_id = end_id;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_iproc_mbox_exit
 * Purpose:
 *      Initialize the mail box data structure.
 * Parameters:
 *      unit - unit number.
 * Returns:
 *      SOC_E_NONE on success
 */
int soc_iproc_mbox_exit(int unit)
{
    int id, num_ids = 0, start_id = 0, end_id = 0;
    soc_control_t   *soc = SOC_CONTROL(unit);
    int ucnum, type;
    uint32 max_ucores;

    max_ucores = _soc_iproc_num_ucore_get(unit);

    for (ucnum = 0; ucnum < max_ucores; ucnum++) {
        num_ids = _soc_iproc_num_mbox_get(unit, ucnum);
        end_id = start_id + num_ids;
        for (id = start_id; id < end_id; id++) {
            for (type = 0; type < MAX_IPROC_MBOX_TYPES; type++) {
                if (soc->iproc_mbox_info[id][type].lock) {
                    sal_mutex_destroy(soc->iproc_mbox_info[id][type].lock);
                    soc->iproc_mbox_info[id][type].lock = NULL;
                }
                soc->iproc_mbox_info[id][type].msg_handler = NULL;
                soc->iproc_mbox_info[id][type].msg_handler_param = 0;
            }
        }
        start_id = end_id;
    }

    SOC_CONTROL(unit)->iproc_mbox_init_done = 0;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_mbox_init
 * Purpose:
 *      Initialize the mail box data structure.
 * Parameters:
 *      unit - unit number.
 * Returns:
 *      SOC_E_NONE on success
 */
int soc_iproc_mbox_init(int unit)
{
    int id, num_ids = 0, start_id = 0, end_id = 0;
    int ucnum, type;
    uint32 mbox_base;
    uint32 mbox_size, max_mboxs, max_ucores;
    char mutex_name[30];
    soc_control_t   *soc = SOC_CONTROL(unit);

    if (SOC_CONTROL(unit)->iproc_mbox_init_done) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "mbox init already done..skipping it\n")));
        return SOC_E_NONE;
    }

    max_ucores = _soc_iproc_num_ucore_get(unit);

    for (ucnum = 0; ucnum < max_ucores; ucnum++) {
        max_mboxs = _soc_iproc_num_mbox_get(unit, ucnum);
        mbox_base = soc_iproc_mbox_base_get(unit, ucnum);
        mbox_size = soc_iproc_mbox_size_get(unit, ucnum) / max_mboxs / MAX_IPROC_MBOX_TYPES;

        num_ids = max_mboxs;
        end_id = start_id + num_ids;
        for (id = start_id; id < end_id; id++) {
            for (type = 0; type < MAX_IPROC_MBOX_TYPES; type++) {
                soc->iproc_mbox_info[id][type].base = mbox_base;
                soc->iproc_mbox_info[id][type].limit = mbox_base + mbox_size;
                soc->iproc_mbox_info[id][type].unit = unit;
                soc->iproc_mbox_info[id][type].mbox = id;
                soc->iproc_mbox_info[id][type].maxent = mbox_size / 4;
                sal_snprintf(mutex_name, sizeof(mutex_name), "Iproc_Mbox%d-%d-%d", unit, id, type);
                soc->iproc_mbox_info[id][type].lock = sal_mutex_create(mutex_name);
                soc->iproc_mbox_info[id][type].ptr = (soc_iproc_mbox_ptr_t *) (long int)mbox_base;
                soc_iproc_setreg(unit, (uint32)(long int)&soc->iproc_mbox_info[id][type].ptr->head, IPROC_MBOX_START_IDX);
                soc_iproc_setreg(unit, (uint32)(long int)&soc->iproc_mbox_info[id][type].ptr->tail, IPROC_MBOX_START_IDX);

                soc->iproc_mbox_info[id][type].msg_handler = NULL;
                soc->iproc_mbox_info[id][type].msg_handler_param = 0;
                mbox_base += mbox_size;
                LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "id %d type %s mbox_base 0x%x \n"),
                                id, (type ? "TX" : "RX"), soc->iproc_mbox_info[id][type].base));
                LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "id %d type %s mbox_limit 0x%x \n"),
                                id, (type ? "TX" : "RX"), soc->iproc_mbox_info[id][type].limit));
            }
        }
        start_id = end_id;
    }

    max_mboxs = 0;
    /* Write the mailbox base and size into SRAM */
    for (ucnum = 0; ucnum < max_ucores; ucnum++) {
        max_mboxs |= _soc_iproc_num_mbox_get(unit, ucnum) << (ucnum * MAX_IPROC_MBOX_SHIFT);
        soc_m0ssq_tcam_write32(unit, ucnum, CPU_ID_OFFSET, ucnum);
        /* To fill device id. */
        if (!(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
            soc_m0ssq_tcam_write32(unit, ucnum, DEVID_ID_OFFSET, CMDEV(unit).dev.dev_id);
        }
        soc_m0ssq_sram_write32(unit, (U0_MBOX_BASE_OFFSET + (ucnum * 8)), soc_iproc_mbox_base_get(unit, ucnum));
        soc_m0ssq_sram_write32(unit, (U0_MBOX_SIZE_OFFSET + (ucnum * 8)), soc_iproc_mbox_size_get(unit, ucnum));
    }

    soc_m0ssq_sram_write32(unit, MAX_IPROC_MBOX_PER_CORE_OFFSET, max_mboxs);
    soc_m0ssq_sram_write32(unit, MBOX_INTERRUPT_MODE_OFFSET, MBOX_INTERRUPT_ENABLE);

    SOC_CONTROL(unit)->iproc_mbox_init_done = 1;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_mbox_alloc
 * Purpose:
 *      Allocate mail box id.
 * Parameters:
 *      unit - unit number.
 *      app_id - Application identifier
 * Returns:
 *      SOC_E_NONE on success
 *      IPROC_MBOX_ERR_xxx on Error
 */
int soc_iproc_mbox_alloc(int unit, int app_id)
{
    int type, ucnum, id;
    soc_control_t   *soc = SOC_CONTROL(unit);

    ucnum = UC_APPID_TO_UCNUM(app_id);
    id = UC_APPID_TO_MBOX(app_id);

    if ((ucnum >= MAX_UCORES) || (id >= NUM_MBOX_PER_UCORE)){
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "App id 0x%x Error\n"), app_id));
        return IPROC_MBOX_ERR_INVPARAM;
    }

    id = id + (ucnum * NUM_MBOX_PER_UCORE);

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "iproc mbox id %d\n"), id));

    for (type = 0; type < MAX_IPROC_MBOX_TYPES; type++) {
        if (soc->iproc_mbox_info[id][type].inuse) {
            LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "mbox %d inuse\n"),id));
            return IPROC_MBOX_ERR_ALLOC;
        }
    }

    for (type = 0; type < MAX_IPROC_MBOX_TYPES; type++) {
        soc->iproc_mbox_info[id][type].inuse = 1;
    }

    return id;
}

/*
 * Function:
 *      soc_iproc_mbox_free
 * Purpose:
 *      Free mail box id.
 * Parameters:
 *      unit - unit number.
 *      mbox_id - mail box identifier
 * Returns:
 *      SOC_E_NONE on success
 *      IPROC_MBOX_ERR_xxx on Error
 */
int soc_iproc_mbox_free(int unit, int mbox_id)
{
    int type;
    soc_control_t   *soc = SOC_CONTROL(unit);

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "iproc mbox id %d\n"), mbox_id));

    if (mbox_id >= MAX_IPROC_MBOXS) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Mbox id 0x%x Error\n"), mbox_id));
        return IPROC_MBOX_ERR_INVPARAM;
    }

    for (type = 0; type < MAX_IPROC_MBOX_TYPES; type++) {
        soc->iproc_mbox_info[mbox_id][type].inuse = 0;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_mbox_handler_register
 * Purpose:
 *      Register callback function to handle RX messages.
 * Parameters:
 *      unit - unit number.
 *      id - mailbox identifier
 *      hndlr - callback function to be called
 *      param - parameter to be passed to the callback function
 * Returns:
 *      SOC_E_NONE on success
 *      IPROC_MBOX_ERR_xxx on Error
 */
int soc_iproc_mbox_handler_register(int unit, int id, soc_iproc_msg_hndlr_t hndlr, void *param)
{
    soc_control_t   *soc = SOC_CONTROL(unit);

    if (id < MAX_IPROC_MBOXS) {
        soc->iproc_mbox_info[id][IPROC_MBOX_TYPE_RX].msg_handler = hndlr;
        soc->iproc_mbox_info[id][IPROC_MBOX_TYPE_RX].msg_handler_param = param;
    } else {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Invalid Mbox id 0x%x \n"),id));
        return IPROC_MBOX_ERR_INVPARAM;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_mbox_handler_unregister
 * Purpose:
 *      Unregister Rx mailbox callback function.
 * Parameters:
 *      unit - unit number.
 *      id - mailbox identifier
 * Returns:
 *      SOC_E_NONE on success
 *      IPROC_MBOX_ERR_xxx on Error
 */
int soc_iproc_mbox_handler_unregister(int unit, int id)
{
    soc_control_t   *soc = SOC_CONTROL(unit);

    if (id < MAX_IPROC_MBOXS) {
        soc->iproc_mbox_info[id][IPROC_MBOX_TYPE_RX].msg_handler = NULL;
        soc->iproc_mbox_info[id][IPROC_MBOX_TYPE_RX].msg_handler_param = 0;
    } else {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Invalid Mbox id %d \n"),id));
        return IPROC_MBOX_ERR_INVPARAM;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_data_send_nowait
 * Purpose:
 *      Send message using iproc mailbox.
 * Parameters:
 *      chan - pointer to tx mailbox
 *      msg - pointer to the message to be sent
 * Returns:
 *      SOC_E_NONE on success
 *      IPROC_MBOX_ERR_xxx on Error
 */
int soc_iproc_data_send_nowait(soc_iproc_mbox_info_t *chan, soc_iproc_mbox_msg_t *msg)
{
    soc_iproc_mbox_ptr_t *mboxptr;
    uint32 *src, *dst;
    int retval, unit;
    uint32 sidx, didx, nentries;
    soc_control_t   *soc;

    if (chan == NULL) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META("Mbox info pointer is NULL \n")));
        return IPROC_MBOX_ERR_INVPARAM;
    }

    unit = chan->unit;
    soc = SOC_CONTROL(unit);
    mboxptr = IPROC_MBOX_PTR(chan);
    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "Base: 0x%x, Head: %d, Tail: %d Sz: %d Id: %d\n"),
         IPROC_MBOX_BASE(chan), IPROC_MBOX_HEAD(chan, mboxptr), IPROC_MBOX_TAIL(chan, mboxptr),
         msg->size, msg->id));

    RESOURCE_LOCK(chan->lock, LOCK_FOREVER);
    if (IPROC_MBOX_FULL(chan, mboxptr, IPROC_MSG_SIZE(msg))) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Mbox 0x%x FULL \n"), IPROC_MBOX_BASE(chan)));
        retval = IPROC_MBOX_ERR_FULL;
        goto errexit;
    }

    nentries = msg->size + IPROC_MSG_HDRSZ;
    src = (uint32 *)msg;
    dst = (uint32 *)mboxptr;
    sidx = 0;
    didx = IPROC_MBOX_TAIL(chan, mboxptr);
    while (sidx < nentries) {
        soc_iproc_mbox_write(unit, ((unsigned long)&dst[didx] - soc_iproc_sram_membase_get(unit)), src[sidx]);
        didx++;
        sidx++;
        if (didx == IPROC_MBOX_MAX_ENT(chan)) {
            didx = IPROC_MBOX_START_IDX;
        }
    }

    soc->stat.m0_msg++;

    MOVE_IPROC_MBOX_TAIL(chan, mboxptr, nentries);
    RESOURCE_UNLOCK(chan->lock);
    return IPROC_MSG_SUCCESS;

errexit:
    RESOURCE_UNLOCK(chan->lock);
    return retval;
}

/*
 * Function:
 *      soc_iproc_data_recv
 * Purpose:
 *      Receive message using iproc mailbox.
 * Parameters:
 *      chan - pointer to rx mailbox
 *      msg - pointer to the message received
 * Returns:
 *      0 on success
 *      IPROC_MBOX_ERR_xxx on Error
 */
int soc_iproc_data_recv(soc_iproc_mbox_info_t *chan, soc_iproc_mbox_msg_t **msg)
{
    soc_iproc_mbox_ptr_t *mboxptr;
    soc_iproc_mbox_msg_t hdr;
    uint32 *src, *dst;
    int retval, unit;
    uint32 sidx, didx, nentries, cmdhead;
    soc_control_t   *soc;

    if (chan == NULL) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META("Mbox info pointer is NULL \n")));
        return IPROC_MBOX_ERR_INVPARAM;
    }

    unit = chan->unit;
    soc = SOC_CONTROL(unit);
    mboxptr = IPROC_MBOX_PTR(chan);

    RESOURCE_LOCK(chan->lock, LOCK_FOREVER);
    /* Check mbox is empty */
    if (IPROC_MBOX_EMPTY(chan, mboxptr)) {
        LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "Mbox %d EMPTY \n"),chan->mbox));
        retval = IPROC_MBOX_ERR_EMPTY;
        goto errexit;
    }

    src = (uint32 *)mboxptr;
    dst = (uint32 *)&hdr;
    /* Copy message Header */
    nentries = IPROC_MSG_HDRSZ;
    cmdhead = sidx = IPROC_MBOX_HEAD(chan, mboxptr);
    didx = 0;
    while (didx < nentries) {
        dst[didx] = soc_iproc_mbox_read(unit, ((unsigned long)&src[sidx] - soc_iproc_sram_membase_get(unit)));
        didx++;
        sidx++;
        if (sidx == IPROC_MBOX_MAX_ENT(chan)) {
            sidx = IPROC_MBOX_START_IDX;
        }
    }
    *msg = soc_iproc_alloc((hdr.size + IPROC_MSG_HDRSZ) * sizeof(uint32));
    if (*msg == NULL) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Mbox %d message alloc failed \n"),chan->mbox));
        retval = IPROC_MBOX_ERR_INTERNAL;
        goto errexit;
    }
    soc_iproc_memcpy(*msg, &hdr, (IPROC_MSG_HDRSZ * sizeof(uint32)));

    dst = (uint32 *) (*msg);
    didx = IPROC_MSG_HDRSZ;
    /* Copy message payload */
    nentries += hdr.size;
    while (didx < nentries) {
        dst[didx] = soc_iproc_mbox_read(unit, ((unsigned long)&src[sidx]- soc_iproc_sram_membase_get(unit)));
        didx++;
        sidx++;
        if (sidx == IPROC_MBOX_MAX_ENT(chan)) {
            sidx = IPROC_MBOX_START_IDX;
        }
    }

    soc->stat.m0_resp++;

    MOVE_IPROC_MBOX_HEAD(chan, mboxptr, nentries);
    RESOURCE_UNLOCK(chan->lock);
    return cmdhead;

errexit:
    RESOURCE_UNLOCK(chan->lock);
    return retval;
}

/*
 * Function:
 *      soc_iproc_data_send_wait
 * Purpose:
 *      Send message using iproc mailbox and wait for the response from M0.
 * Parameters:
 *      chan - pointer to tx mailbox
 *      msg - pointer to the message to be sent
 *      resp - pointer to the response to be received
 * Returns:
 *      0 on success
 *      IPROC_MBOX_ERR_xxx on Error
 */
int soc_iproc_data_send_wait(soc_iproc_mbox_info_t *chan, soc_iproc_mbox_msg_t *cmd, soc_iproc_mbox_msg_t *resp)
{
    soc_iproc_mbox_ptr_t *mboxptr;
    uint32 *src, *dst;
    int retval, unit;
    uint32 nentries, sidx, didx, cmdidx;
    int count = IPROC_MBOX_READ_RETRY_COUNT;
    soc_control_t   *soc;

    if (chan == NULL) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META("Mbox info pointer is NULL \n")));
        return IPROC_MBOX_ERR_INVPARAM;
    }

    unit = chan->unit;
    soc = SOC_CONTROL(unit);
    mboxptr = IPROC_MBOX_PTR(chan);
    RESOURCE_LOCK(chan->lock, LOCK_FOREVER);
    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "Base: 0x%x, Head: %d, Tail: %d Sz: %d Id: %d\n"),
         IPROC_MBOX_BASE(chan), IPROC_MBOX_HEAD(chan, mboxptr), IPROC_MBOX_TAIL(chan, mboxptr),
         cmd->size, cmd->id));

    if (IPROC_MBOX_FULL(chan, mboxptr, IPROC_MSG_SIZE(cmd))) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Mbox 0x%x FULL \n"), IPROC_MBOX_BASE(chan)));
        retval = IPROC_MBOX_ERR_FULL;
        goto errexit;
    }

    nentries = cmd->size + IPROC_MSG_HDRSZ;
    src = (uint32 *)cmd;
    dst = (uint32 *)mboxptr;
    sidx = 0;
    didx = cmdidx = IPROC_MBOX_TAIL(chan, mboxptr);

    while (sidx < nentries) {
        soc_iproc_mbox_write(unit, ((unsigned long)&dst[didx] - soc_iproc_sram_membase_get(unit)), src[sidx]);
        didx++;
        sidx++;
        if (didx == IPROC_MBOX_MAX_ENT(chan)) {
            didx = IPROC_MBOX_START_IDX;
        }
    }

    soc->stat.m0_msg++;
    MOVE_IPROC_MBOX_TAIL(chan, mboxptr, nentries);

    /* poll for response */
    src = (uint32 *)mboxptr;
    dst = (uint32 *)resp;
    while (count) {
        if ((soc_iproc_mbox_read(unit, (unsigned long)&src[cmdidx] - soc_iproc_sram_membase_get(unit)) & IPROC_RESP_READY) != 0) {
            break;
        }
        sal_usleep(IPROC_MBOX_SLEEP_DELAY);
        count--;
    }

    if (!count) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "No response for msg %d\n"), cmd->id));
        retval = IPROC_MBOX_ERR_INTERNAL;
        goto errexit;
    }
    sidx = cmdidx;
    didx = 0;
    while (didx < nentries) {
        dst[didx] = soc_iproc_mbox_read(unit, (unsigned long)&src[sidx] - soc_iproc_sram_membase_get(unit));
        didx++;
        sidx++;
        if (sidx == IPROC_MBOX_MAX_ENT(chan)) {
            sidx = IPROC_MBOX_START_IDX;
        }
    }

    soc->stat.m0_resp++;

    RESOURCE_UNLOCK(chan->lock);
    return IPROC_MSG_SUCCESS;

errexit:
    RESOURCE_UNLOCK(chan->lock);
    return retval;
}
#endif /* BCM_CMICX_SUPPORT */
#endif /* BCM_ESW_SUPPORT */

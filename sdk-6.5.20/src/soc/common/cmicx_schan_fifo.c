/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * CMCIX SCHAN FIFO low level driver
 */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/core/boot.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/error.h>
#include <sal/core/sync.h>
#include <soc/soc_schan_fifo_internal.h>
#include <soc/ccmdma.h>

#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>
#include <soc/intr_cmicx.h>
#include <soc/schanmsg.h>

/* CMIC base address */
#define CMIC_BASE_ADDR 0x3200000
#define HX5_CMIC_BASE_ADDR 0x10100000

/* This structure is used for the SCHAN FIFO interrupt control */

typedef enum {
    CHAN_FREE = 0,
    CHAN_CMD,
    CHAN_PROG,
    CHAN_EXEC
}schan_state_t;

typedef struct soc_schan_fifo_ch_s {
    sal_sem_t             schanIntr;
    int                   ch;
    schan_state_t         state;
}soc_schan_fifo_ch_t;

/* This struture is used for SCHAN FIFO driver control */
typedef struct soc_schan_fifo_control_s {
    int                        ch_map;
    sal_spinlock_t             lock; /* used in channel selection */
    int                        num;
    soc_schan_fifo_ch_t        chan[CMIC_SCHAN_FIFO_NUM_MAX];
    uint16                     *summary_buff[CMIC_SCHAN_FIFO_NUM_MAX];
    soc_schan_fifo_config_t    config;
}soc_schan_fifo_control_t;

STATIC soc_schan_fifo_control_t _soc_cmicx_schan_fifo[SOC_MAX_NUM_DEVICES];
#define SOC_SCHAN_FIFO_CONTROL(unit) (&_soc_cmicx_schan_fifo[unit])


/*
 * Dump SCHAN FIFO message for debugging
 */

STATIC void
_soc_cmicx_schan_fifo_dump(int unit, schan_fifo_cmd_t *cmd, int size)
{
    int i;

    for (i = 0 ; i < size; i++) {
        LOG_CLI((BSL_META_U(unit,
                 " DW[%2d]=0x%08x"), i, ((uint32 *)cmd)[i]));
    }
}

/*******************************************
* @function _soc_cmicx_schan_fifo_chstate_get
* purpose: Get channel state
*
* @param unit [in] unit #
* @param ch [in] channel number 0-1
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
STATIC schan_state_t
_soc_cmicx_schan_fifo_chstate_get(int unit, int ch)
{
    soc_schan_fifo_control_t *schan_fifo = SOC_SCHAN_FIFO_CONTROL(unit);

    return schan_fifo->chan[ch].state;
}

/*******************************************
* @function _soc_cmicx_schan_fifo_chstate_set
* purpose: Set channel state
*
* @param unit [in] unit #
* @param ch [in] channel number 0-1
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
STATIC void
_soc_cmicx_schan_fifo_chstate_set(int unit, int ch, schan_state_t st)
{
    soc_schan_fifo_control_t *schan_fifo = SOC_SCHAN_FIFO_CONTROL(unit);

    schan_fifo->chan[ch].state = st;

    LOG_VERBOSE(BSL_LS_SOC_SCHANFIFO,
                 (BSL_META_U(unit,
                             "channel = %d, state = %d\n"),
                             ch, st));

}

/*******************************************
* @function _soc_cmicx_schan_fifo_ch_start
* purpose: Start the channel
*
* @param unit [in] unit #
* @param ch [in] channel number 0-1
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
STATIC void
_soc_cmicx_schan_fifo_ch_start(int unit, int ch)
{
    soc_schan_fifo_control_t *schan_fifo = SOC_SCHAN_FIFO_CONTROL(unit);
    soc_schan_fifo_config_t *cfg = &schan_fifo->config;
    uint32 val;

    val = soc_pci_read(unit, CMIC_SCHAN_FIFO_CHx_CTRL(ch));
    soc_reg_field_set(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_CTRLr,
                               &val, STARTf, 1);
    if (cfg->intrEnb) {
        soc_cmic_intr_enable(unit, INTR_SCHAN_FIFO(ch));
    }
    LOG_VERBOSE(BSL_LS_SOC_SCHANFIFO,
                 (BSL_META_U(unit,
                             "CMIC_SCHAN_FIFO_CHx_CTRL= 0x%x\n"), val));
    soc_pci_write(unit, CMIC_SCHAN_FIFO_CHx_CTRL(ch), val);
    _soc_cmicx_schan_fifo_chstate_set(unit, ch, CHAN_EXEC);
}

/*******************************************
* @function _soc_cmicx_schan_fifo_ch_stop
* purpose: Stop the channel
*
* @param unit [in] unit #
* @param ch [in] channel number 0-1
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
STATIC void
_soc_cmicx_schan_fifo_ch_stop(int unit, int ch)
{
    uint32 val;

    val = soc_pci_read(unit, CMIC_SCHAN_FIFO_CHx_CTRL(ch));
    soc_reg_field_set(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_CTRLr,
                               &val, STARTf, 0);
    soc_pci_write(unit, CMIC_SCHAN_FIFO_CHx_CTRL(ch), val);
    _soc_cmicx_schan_fifo_chstate_set(unit, ch, CHAN_FREE);
}


/*******************************************
* @function _soc_cmicx_schan_fifo_get
* purpose get idle channel
*
* @param unit [in] unit #
* @param ch [out] channel number 0-1
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
STATIC int _soc_cmicx_schan_fifo_get(int unit, int *ch)
{
    int i;
    int rv = SOC_E_BUSY;
    soc_schan_fifo_control_t *schan_fifo = SOC_SCHAN_FIFO_CONTROL(unit);
    soc_schan_fifo_config_t *cfg = &schan_fifo->config;
    soc_timeout_t to;

    soc_timeout_init(&to, cfg->timeout, 100);

    do {
       sal_spinlock_lock(schan_fifo->lock);
       for ( i = 0; i < CMIC_SCHAN_FIFO_NUM_MAX; i++) {
           if (schan_fifo->ch_map & 0x01 << i) {
               rv = SOC_E_NONE;
               *ch = i;
               schan_fifo->ch_map &= ~(0x01 << i);
               break;
           }
       }
       sal_spinlock_unlock(schan_fifo->lock);
       if (rv == SOC_E_NONE)
          break;

    } while (!soc_timeout_check(&to));
    return rv;
}

/*******************************************
* @function _soc_cmicx_schan_fifo_put
* purpose get idle channel
*
* @param unit [in] unit #
* @param ch [in] channel number 0-1
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE

*
* @end
 */
STATIC int _soc_cmicx_schan_fifo_put(int unit, int ch)
{
   int rv = SOC_E_NONE;
   soc_schan_fifo_control_t *schan_fifo = SOC_SCHAN_FIFO_CONTROL(unit);

   if ((ch < 0) || (ch > CMIC_SCHAN_FIFO_NUM_MAX - 1)) {
       rv = SOC_E_PARAM;
       return rv;
   }
   sal_spinlock_lock(schan_fifo->lock);
   schan_fifo->ch_map |= (0x01 << ch);
   sal_spinlock_unlock(schan_fifo->lock);

   return rv;
}

/*******************************************
* @function _soc_cmicx_schan_fifo_cmic_err_handle
* purpose Handle schan cimicx error
*
* @param unit [in] unit
* @param schanStat [in] schan channel status
* @param ch [in] channel
*
* @returns SOC_E_XXX
* @returns SOC_E_NONE
*
* @end
 */

STATIC int
_soc_cmicx_schan_fifo_err_handle(int unit, uint32 schanStat, int ch)
{
    uint32 num;
    uint32 reg;

    if (!soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_STATUSr,
                              schanStat, ERRORf)) {
        return SOC_E_NONE;
    }

    num = soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_STATUSr,
                         schanStat, CURR_SBUS_CMD_NUMf);

    LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
              "ch = %d , cmd num = %u\n"), ch, num));

    if (soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_STATUSr,
                         schanStat, ERR_TYPE_AXI_SLAVE_ABORTf)) {
        LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "Axi Slave Abort.\n")));
    }

    if (soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_STATUSr,
                         schanStat, ERR_TYPE_AXI_RESP_ERRf)) {
        LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "AXI response error.\n")));
    }

    if (soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_STATUSr,
                         schanStat, ERR_TYPE_ACK_DATA_BEAT_GT20f)) {
        LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "Data beats was greater than 20.\n")));
    }

    if (soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_STATUSr,
                         schanStat, ERR_TYPE_SBUS_TIMEOUTf)) {
        LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "SBUS Transaction timed out.\n")));
    }

    if (soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_STATUSr,
                         schanStat, ERR_TYPE_CMD_DLEN_IS_INVALIDf)) {
        LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "SBUS Command DLEN is invalid.\n")));
    }

    if (soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_STATUSr,
                         schanStat, ERR_TYPE_CMD_OPCODE_IS_INVALIDf)) {
        LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "SBUS Command OPCODE is invalid.\n")));
    }

    if (soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_STATUSr,
                         schanStat, ERR_TYPE_RESP_MEM_ECC_ERRf)) {
        LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "ECC error was received from Response Memory.\n")));
    }

    if (soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_STATUSr,
                         schanStat, ERR_TYPE_CMD_MEM_ECC_ERRf)) {
        LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "ECC error was received from command Memory.\n")));
    }

    if (soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_STATUSr,
                         schanStat, ERR_TYPE_ACK_OPCODE_MISMATCHf)) {
        LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "ACK Opcode received was not proper.\n")));
    }

    if (soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_STATUSr,
                         schanStat, ERR_TYPE_MESSAGE_ERR_CODEf)) {
        LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "Message error code.\n")));
    }

    if (soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_STATUSr,
                         schanStat, ERR_TYPE_MESSAGE_ERRf)) {
        LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "Message error was received.\n")));
    }

    if (soc_reg_field_get(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_STATUSr,
                         schanStat, ERR_TYPE_NACKf)) {
        LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "NACK was received .\n")));
    }

    LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "SCHAN FIFO STATUS = 0x%x\n"), schanStat));

    reg = soc_pci_read(unit, CMIC_SCHAN_FIFO_CHx_CTRL(ch));
    LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "SCHAN FIFO CTRL = 0x%x\n"), reg));

    reg= soc_pci_read(unit, CMIC_SCHAN_FIFO_CHx_RESP_ADDR_LOWER(ch));
    LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "SCHAN FIFO RESP ADDR LOWER = 0x%x\n"), reg));

    reg= soc_pci_read(unit, CMIC_SCHAN_FIFO_CHx_RESP_ADDR_UPPER(ch));
    LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "SCHAN FIFO RESP ADDR UPPER = 0x%x\n"), reg));

    reg= soc_pci_read(unit, CMIC_SCHAN_FIFO_CHx_SUMMARY_ADDR_LOWER(ch));
    LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "SCHAN FIFO SUMMARY ADDR LOWER = 0x%x\n"), reg));

    reg= soc_pci_read(unit, CMIC_SCHAN_FIFO_CHx_SUMMARY_ADDR_UPPER(ch));
    LOG_ERROR(BSL_LS_SOC_SCHANFIFO, (BSL_META_U(unit,
                  "SCHAN FIFO SUMMARY ADDR UPPER = 0x%x\n"), reg));

    return SOC_E_FAIL;
}

/*******************************************
* @function _soc_cmicx_schan_fifo_intr_wait
* purpose On CMICX chips with schan interrupts
*
* @param unit [in] unit
* @param ch [in] channel number 0-4
*
* @returns SOC_E_XXX
* @returns SOC_E_XXX
*
* Commnet: Called when CMICX chips with schan interrupts
* enabled, wait on a schan interrupt.
* When schan interrupts are disabled,
* _soc_cmicx_schan_fifo_poll_wait is called instead.

* @end
 */
STATIC int
_soc_cmicx_schan_fifo_intr_wait(int unit, int ch)
{
    int rv = SOC_E_NONE;
    uint32 schanStat;
    soc_schan_fifo_ch_t    *int_data;
    soc_schan_fifo_control_t *schan_fifo = SOC_SCHAN_FIFO_CONTROL(unit);
    soc_schan_fifo_config_t *cfg = &schan_fifo->config;

    int_data = &schan_fifo->chan[ch];
    if (sal_sem_take(int_data->schanIntr,
                     cfg->timeout) != 0) {
        rv = SOC_E_TIMEOUT;
    }

    if (rv != SOC_E_TIMEOUT) {
        LOG_VERBOSE(BSL_LS_SOC_SCHANFIFO,
                 (BSL_META_U(unit,
                             "  Interrupt Done\n")));
        schanStat = soc_pci_read(unit, CMIC_SCHAN_FIFO_CHx_STATUS(ch));
        rv = _soc_cmicx_schan_fifo_err_handle(unit, schanStat, ch);
    }

    return rv;
}

/*******************************************
* @function _soc_cmicx_schan_fifo_poll_wait
* purpose On CMICX chips with no schan interrupts
*
* @param unit [in] unit
* @param msg [in] message <schan_msg_t>
* @param ch [in] channel number 0-4
*
* @returns SOC_E_XXX
* @returns SOC_E_XXX
*
* Commnet: Called when CMICX chips with schan interrupts
* disabled, wait on a schan interrupt.
* When schan interrupts are enabled,
* _soc_cmicx_schan_fifo_intr_wait is called instead.

* @end
 */

STATIC int
_soc_cmicx_schan_fifo_poll_wait(int unit, int ch)
{
    int rv = SOC_E_NONE;
    soc_timeout_t to;
    uint32 schanStat;
    soc_schan_fifo_control_t *schan_fifo = SOC_SCHAN_FIFO_CONTROL(unit);
    soc_schan_fifo_config_t *cfg = &schan_fifo->config;

    soc_timeout_init(&to, cfg->timeout, 100);

    while (((schanStat =
                   soc_pci_read(unit, CMIC_SCHAN_FIFO_CHx_STATUS(ch))) &
                         SCHAN_FIFO_MSG_DONE) == 0) {
        if (soc_timeout_check(&to)) {
            rv = SOC_E_TIMEOUT;
        }
    }
    /* Polling finished no timeout , check other errors */
    if (rv == SOC_E_NONE) {
        LOG_VERBOSE(BSL_LS_SOC_SCHANFIFO,
                 (BSL_META_U(unit,
                             "  Done in %d polls\n"), to.polls));
        rv = _soc_cmicx_schan_fifo_err_handle(unit, schanStat, ch);
    }

    return rv;
}
/*******************************************
* @function _soc_cmicx_schan_fifo_abort
* purposeResets the CMICX S-Channel interface.
*
* @param unit [in] unit #
* @param ch [in] channel number 0-1
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
STATIC void
_soc_cmicx_schan_fifo_abort(int unit, int ch)
{

    uint32 val;

    /* Skip abort if no schan fifo operation is in progress */
    if (((soc_pci_read(unit, CMIC_SCHAN_FIFO_CHx_STATUS(ch))) &
                         SCHAN_FIFO_MSG_DONE) == 0) {
        return;
    }

    val = soc_pci_read(unit, CMIC_SCHAN_FIFO_CHx_CTRL(ch));
    /* Toggle S-Channel abort bit in control register */
    soc_reg_field_set(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_CTRLr,
                               &val, STARTf, 1);
    soc_reg_field_set(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_CTRLr,
                               &val, ABORTf, 1);
    SDK_CONFIG_MEMORY_BARRIER;
    soc_pci_write(unit, CMIC_SCHAN_FIFO_CHx_CTRL(ch), val);
    SDK_CONFIG_MEMORY_BARRIER;

    if (SAL_BOOT_QUICKTURN) {
        /* Give Quickturn at least 2 cycles */
        sal_usleep(10 * MILLISECOND_USEC);
    }

    /* Wait for Abort to finish */
    if (SOC_FAILURE(_soc_cmicx_schan_fifo_poll_wait(unit, ch))) {
        LOG_ERROR(BSL_LS_SOC_SCHANFIFO,
              (BSL_META_U(unit,"Abort of channel [%d] Failed\n"), ch));
    }
}


/*******************************************
* @function _soc_cmicx_schan_fifo_control
* purpose API to control SCHAN FIFO
*
* @param unit [in] unit
* @param ctl [in] schan_fifo_ctl_t
* @param data [in, out] pointer to int
*
* @returns SOC_E_NONE
*
* @end
 */
STATIC int
_soc_cmicx_schan_fifo_control(int unit,
                 schan_fifo_ctl_t ctl, void *data)
{
    int rv = SOC_E_NONE;
    schan_fifo_alloc_t *alloc;

    switch (ctl) {
    case CTL_FIFO_ABORT:
        _soc_cmicx_schan_fifo_abort(unit, 0);
        _soc_cmicx_schan_fifo_abort(unit, 1);
    break;

    case CTL_FIFO_MAX_MSG_GET:
        *(int *)data = CMIC_SCHAN_FIFO_CMD_SIZE_MAX;
    break;
    case CTL_FIFO_RESP_ALLOC:
        alloc = (schan_fifo_alloc_t *)data;
        if ((alloc != NULL) && (alloc->num > 0)) {
            alloc->resp = soc_cm_salloc(unit,
                       (sizeof(schan_fifo_resp_t) * alloc->num),
                               "schan_fifo_response");
            if (alloc->resp == NULL) {
                rv = SOC_E_MEMORY;
            }
        } else {
            rv = SOC_E_PARAM;
        }
    break;
    case CTL_FIFO_RESP_FREE:
       if (data != NULL) {
            soc_cm_sfree(unit, data);
       } else {
           rv = SOC_E_PARAM;
       }
    break;
    default:
        LOG_VERBOSE(BSL_LS_SOC_SCHANFIFO,
              (BSL_META_U(unit,":undefined command = %d\n"), ctl));
        rv = SOC_E_PARAM;
    break;
    }

    return rv;
}

/*******************************************
* @function _soc_cmicx_schan_fifo_resp_config
* purpose Read response buffer configuration
*
* @param unit [in] unit
* @param ch [in] channel number 0-1
* @param msg [out] SCHAN FIFO message pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmicx_schan_fifo_resp_config(
         int unit,
         int ch,
         soc_schan_fifo_msg_t  *msg,
         uint32 *val)
{
    /* Set up Response Register */
    if (msg->resp != NULL) {
       /* write response Lo address */
        soc_pci_write(unit, CMIC_SCHAN_FIFO_CHx_RESP_ADDR_LOWER(ch),
                       PTR_TO_INT(soc_cm_l2p(unit, msg->resp)));
        /* write response Hi address */
        if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
            soc_pci_write(unit, CMIC_SCHAN_FIFO_CHx_RESP_ADDR_UPPER(ch),
                      (PTR_HI_TO_INT(soc_cm_l2p(unit, msg->resp)) |
                       CMIC_PCIE_SO_OFFSET));
        } else {
            soc_pci_write(unit, CMIC_SCHAN_FIFO_CHx_RESP_ADDR_UPPER(ch),
                      PTR_HI_TO_INT(soc_cm_l2p(unit, msg->resp)));
        }

        /* Enable the response write */
        soc_reg_field_set(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_CTRLr,
                               val, RESPONSE_WRITE_DISABLEf, 0);
   } else {
       /* disable the response write */
       soc_reg_field_set(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_CTRLr,
                               val, RESPONSE_WRITE_DISABLEf, 1);
   }

   return SOC_E_NONE;
}

/*******************************************
* @function _soc_cmicx_schan_fifo_cmd_complete
* purpose wait for command transfer via CCMDMA
*
* @param unit [in] unit
* @param ch [in] channel number 0-1
* @param msg [out] SCHAN FIFO message pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmicx_schan_fifo_cmd_complete(
         int unit,
         int ch)
{
    int rv = SOC_E_NONE;
    schan_state_t  state;

    state = _soc_cmicx_schan_fifo_chstate_get(unit, ch);
    LOG_VERBOSE(BSL_LS_SOC_SCHANFIFO,
                 (BSL_META_U(unit,
                             "  channel = %d, State = %d\n"), ch, state));

    switch(state) {
    case CHAN_CMD:
#ifdef BCM_CCMDMA_SUPPORT
        rv = soc_ccmdma_copy_wait(unit, ch);
#endif
        if (rv == SOC_E_NONE) {
            _soc_cmicx_schan_fifo_chstate_set(unit, ch, CHAN_PROG);
        } else {
            LOG_ERROR(BSL_LS_SOC_SCHANFIFO,
              (BSL_META_U(unit,"Error in CCMDMA command = %d\n"), rv));
            return rv;
        }
        state = CHAN_PROG;
        _soc_cmicx_schan_fifo_ch_start(unit, ch);
    break;
    case CHAN_PROG:
        _soc_cmicx_schan_fifo_ch_start(unit, ch);
    break;
    default:
        return rv;
    }

    return rv;

}

/*******************************************
* @function _soc_cmicx_schan_fifo_op_complete
* purpose This waits for either interrupt or poll
*
* @param handle [in] unit
* @param msg [in] SCHAN message pointer
* @param dwc_write [in] Number of messages to write
* @param dwc_read [in] Number of messages to read
* @param intr [in] Interrupt
* @param ch [in] channel number 0-4
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */

STATIC int _soc_cmicx_schan_fifo_op_complete(int unit,
                                 void *data, void *wparam, uint32 do_not_free_channel)
{
    int rv = SOC_E_NONE;
    int ch = ((soc_schan_fifo_wparam_t *)wparam)->ch;
    soc_schan_fifo_control_t *schan_fifo = SOC_SCHAN_FIFO_CONTROL(unit);
    soc_schan_fifo_config_t *cfg = &schan_fifo->config;

    LOG_VERBOSE(BSL_LS_SOC_SCHANFIFO,
                 (BSL_META_U(unit,
                             "  Enter: _soc_cmicx_schan_fifo_op_complete\n")));

    /* Current channel program is complete */
    _soc_cmicx_schan_fifo_cmd_complete(unit, ch);
    /*Set to start the other channel */
    _soc_cmicx_schan_fifo_cmd_complete(unit, (ch ^ 1));

    do {
        /* Wait for completion using either the interrupt or polling method */
        if (cfg->intrEnb) {
            rv = _soc_cmicx_schan_fifo_intr_wait(unit, ch);
        } else {
            rv = _soc_cmicx_schan_fifo_poll_wait(unit, ch);
        }

        if (rv != SOC_E_NONE) {
            break;
        }
    } while(0);

    if (rv == SOC_E_TIMEOUT) {
        /* Abort the curent operation */
        _soc_cmicx_schan_fifo_abort(unit, ch);
    }
    /* Release the channel */
    _soc_cmicx_schan_fifo_ch_stop(unit, ch);
    _soc_cmicx_schan_fifo_put(unit, ch);

    return rv;
}

/*******************************************
* @function _soc_cmicx_schan_fifo_op_prog
* purpose initiate the schan FIFO operation
*
* @param handle [in] unit
* @param data [in] SCHAN message pointer
* @param wparam [in, out] message parameter
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_soc_cmicx_schan_fifo_op_prog(
         int unit,
         void *data, void *wparam)

{
    int rv = SOC_E_NONE;
    soc_schan_fifo_msg_t  *msg = (soc_schan_fifo_msg_t *)data;
    soc_schan_fifo_control_t *schan_fifo = SOC_SCHAN_FIFO_CONTROL(unit);
    int ch, i;
    uint32 val;
#ifdef BCM_CCMDMA_SUPPORT
    sal_paddr_t *baddr;
#endif

    LOG_VERBOSE(BSL_LS_SOC_SCHANFIFO,
                 (BSL_META_U(unit,
                             "  Enter: _soc_cmicx_schan_fifo_op_prog\n")));

    /* Get the free channel */
    rv = _soc_cmicx_schan_fifo_get(unit, &ch);

    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_SCHANFIFO,
                 (BSL_META_U(unit,
                             "  Unable to assign channel\n")));
        return rv;

    }

    LOG_VERBOSE(BSL_LS_SOC_SCHANFIFO,
                 (BSL_META_U(unit,
                             "channel = %d, commands = %d, size =%u\n"),
                             ch, msg->num, (unsigned int)msg->size));
    ((soc_schan_fifo_wparam_t *)wparam)->ch = ch;

    /* Wait for cmd program to complete or start the other channel if programmed */
    _soc_cmicx_schan_fifo_cmd_complete(unit, (ch ^ 1));

    if (LOG_CHECK(BSL_LS_SOC_SCHAN | BSL_VERBOSE)) {
        LOG_VERBOSE(BSL_LS_SOC_SCHANFIFO,
                     (BSL_META_U(unit,
                            "SCHANFIFO %s: (unit %d)\n"),
                 soc_schan_op_name(msg->cmd->header.v4.opcode), unit));
        _soc_cmicx_schan_fifo_dump(unit, msg->cmd, msg->size);
    }

    if (schan_fifo->config.dma == 1) {
#ifdef BCM_CCMDMA_SUPPORT
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit))
    {
#ifdef PTRS_ARE_64BITS
        baddr = (sal_paddr_t *)((uint64)HX5_CMIC_BASE_ADDR +
                                CMIC_SCHAN_FIFO_CHx_COMMAND(ch, 0));
#else
        baddr = (sal_paddr_t *)(HX5_CMIC_BASE_ADDR +
                                CMIC_SCHAN_FIFO_CHx_COMMAND(ch, 0));
#endif
    }
    else
#endif
    {
#ifdef PTRS_ARE_64BITS
        baddr = (sal_paddr_t *)((uint64)CMIC_BASE_ADDR +
                                CMIC_SCHAN_FIFO_CHx_COMMAND(ch, 0));
#else
        baddr = (sal_paddr_t *)(CMIC_BASE_ADDR +
                                CMIC_SCHAN_FIFO_CHx_COMMAND(ch, 0));
#endif
    }

        LOG_VERBOSE(BSL_LS_SOC_SCHANFIFO,
                         (BSL_META_U(unit,
                          "Using CCMDMA\n")));

        rv = soc_ccmdma_copy(unit, (sal_paddr_t *)(msg->cmd), baddr,
                             0, 1, msg->size, 4, ch);

        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_SCHANFIFO,
                     (BSL_META_U(unit,
                                 "CCM DMA failed with error 0x%x\n"), rv));
            return rv;
        }
#endif
        _soc_cmicx_schan_fifo_chstate_set(unit, ch, CHAN_CMD);
    } else {
        for (i = 0; i < msg->size; i++) {
            val = ((uint32 *)msg->cmd)[i];
            soc_pci_write(unit, CMIC_SCHAN_FIFO_CHx_COMMAND(ch, i),
                                          val);
        }
        _soc_cmicx_schan_fifo_chstate_set(unit, ch, CHAN_PROG);
    }

    /* program number of commands and update interval
     * for update interval value -1, set it to max limit
     */
    val = soc_pci_read(unit, CMIC_SCHAN_FIFO_CHx_CTRL(ch));
    soc_reg_field_set(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_CTRLr,
                               &val, NUMB_OF_COMMANDSf, (msg->num));
    if ((msg->interval < 0) ||
           (msg->interval > SCHAN_FIFO_SUMMARY_UPDATE_INTERVAL)) {
        msg->interval = SCHAN_FIFO_SUMMARY_UPDATE_INTERVAL;
    }
    soc_reg_field_set(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_CTRLr,
                      &val, SUMMARY_UPDATE_INTERVALf, msg->interval);

    (void)_soc_cmicx_schan_fifo_resp_config(unit, ch, msg, &val);


    (void)soc_pci_write(unit, CMIC_SCHAN_FIFO_CHx_CTRL(ch), val);

   return rv;
}

/*******************************************
* @function _cmicx_schan_fifo_ch_done
* Interrupt handler for schan channel done
*
* @param unit [in] unit
* @param data [in] pointer provided during registration
*
*
* @end
 */
STATIC void
_cmicx_schan_fifo_ch_done(int unit, void *data)
{
    soc_schan_fifo_control_t *schan_fifo = SOC_SCHAN_FIFO_CONTROL(unit);
    soc_schan_fifo_config_t *cfg = &schan_fifo->config;
    soc_schan_fifo_ch_t *in_d = (soc_schan_fifo_ch_t *)data;

    soc_cmic_intr_disable(unit, INTR_SCHAN_FIFO(in_d->ch));
    if (cfg->intrEnb) {
        sal_sem_give(in_d->schanIntr);
    }
}

/*******************************************
* @function _cmicx_schan_fifo_intr_deinit
* purpose API to de-Initialize SCHAN interrupts
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_cmicx_schan_fifo_intr_deinit(int unit)
{
    int ch;
    soc_schan_fifo_ch_t    *int_data;
    for (ch = 0; ch < CMIC_SCHAN_FIFO_NUM_MAX; ch++) {
        int_data = &_soc_cmicx_schan_fifo[unit].chan[ch];
        if (int_data->schanIntr) {
             sal_sem_destroy(int_data->schanIntr);
        }
    }

    return SOC_E_NONE;
}

/*******************************************
* @function _cmicx_schan_fifo_intr_init
* purpose API to Initialize SCHAN interrupts
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
STATIC int
_cmicx_schan_fifo_intr_init(int unit)
{
    int ch;
    soc_cmic_intr_handler_t *handle;
    soc_cmic_intr_handler_t *hitr;
    soc_schan_fifo_ch_t    *int_data;
    int rv = SOC_E_NONE;


    /* Register interrupts */
    handle =
        sal_alloc(CMIC_SCHAN_FIFO_NUM_MAX * sizeof(soc_cmic_intr_handler_t),
                        "schan_fifo_interrupt");
    if (handle == NULL) {
        return SOC_E_MEMORY;
    }

    hitr = handle;
    for (ch = 0; ch < CMIC_SCHAN_FIFO_NUM_MAX; ch++) {
         int_data = &_soc_cmicx_schan_fifo[unit].chan[ch];
         int_data->schanIntr =
                         sal_sem_create("SCHAN_FIFO interrupt",
                                        sal_sem_BINARY, 0);
         if (int_data->schanIntr == NULL) {
             _cmicx_schan_fifo_intr_deinit(unit);
             rv = SOC_E_MEMORY;
             goto error;
         }
         int_data->ch = ch;
         hitr->num = INTR_SCHAN_FIFO(ch);
         hitr->intr_fn = _cmicx_schan_fifo_ch_done;
         hitr->intr_data = int_data;
         hitr++;
    }
    rv = soc_cmic_intr_register(unit, handle, CMIC_SCHAN_FIFO_NUM_MAX);
error:
    sal_free(handle);

    return rv;
}
/*******************************************
* @function _cmicx_schan_fifo_endian_config
* purpose Configure endianess of sbus dma descriptor
*
* @param unit [in] unit
* @param val [in] uint32  pointer
*
* @returns None
* @end
 */
STATIC void
_cmicx_schan_fifo_endian_config(int unit, uint32 *val)
{
    int  big_pio, big_packet, big_other;

    soc_endian_get(unit, &big_pio, &big_packet, &big_other);

    if (big_other) {
       soc_reg_field_set(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_CTRLr,
                               val, SUMMARY_ENDIANESSf, 1);
       soc_reg_field_set(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_CTRLr,
                               val, RESPONSE_ENDIANESSf, 1);
    }
}


/*******************************************
* @function _soc_cmicx_schan_fifo_deinit
* purpose API to de-Initialize SCHAN
*
* @param unit [in] unit
*
* @returns NONE
*
* @end
 */
STATIC void
_soc_cmicx_schan_fifo_deinit(int unit)
{
   int ch;
   soc_schan_fifo_control_t *schan_fifo = SOC_SCHAN_FIFO_CONTROL(unit);

   _cmicx_schan_fifo_intr_deinit(unit);

   /* reset all th schan channels */
   for (ch = 0 ; ch < CMIC_SCHAN_FIFO_NUM_MAX; ch++) {
       _soc_cmicx_schan_fifo_abort(unit, ch);
       if (schan_fifo->summary_buff[ch]) {
            soc_cm_sfree(unit, schan_fifo->summary_buff[ch]);
            schan_fifo->summary_buff[ch] = NULL;
       }
   }
   sal_spinlock_destroy(schan_fifo->lock);

}

/*******************************************
* @function soc_cmicx_schan_fifo_init
* purpose API to Initialize cmicx SCHAN FIFO
*
* @param unit [in] unit
* @param drv [out] soc_schan_fifo_drv_op_t pointer
* @param config [in] pointer to soc_schan_fifo_config_t
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int soc_cmicx_schan_fifo_init(int unit,
                                soc_schan_fifo_drv_op_t *drv,
                                soc_schan_fifo_config_t *config)

{
   int rv = SOC_E_NONE;
   uint32 val;
   soc_schan_fifo_control_t *schan_fifo = SOC_SCHAN_FIFO_CONTROL(unit);
   int ch, idx = 0;

   sal_memset(schan_fifo, 0, sizeof(soc_schan_fifo_control_t));

   /* Set CMIC_COMMON_POOL_SHARED_CONFIG Register,
    * SCHAN FIFO are sent through AXI master port of CMC0
    */
   val = soc_pci_read(unit, CMIC_COMMON_POOL_SHARED_CONFIG_OFFSET);
   soc_reg_field_set(unit, CMIC_COMMON_POOL_SHARED_CONFIGr,
                      &val, MAP_SCHAN_FIFO_MEMWR_REQf,
                      MAP_SCHAN_FIFO_MEMWR_REQ_CMC0);
   soc_pci_write(unit, CMIC_COMMON_POOL_SHARED_CONFIG_OFFSET, val);

   /* Programming WRR (Arbitration) within CMC. Configure WRR weight
    * for FIFO DMA channels
    */
   soc_pci_write(unit,
            CMIC_COMMON_POOL_SHARED_SCHAN_FIFO_WRITE_ARB_CTRL_OFFSET,
            SCHAN_FIFO_MEMWR_WRR_WEIGHT);

   /* This can be dependent upon CMIC Version */
   schan_fifo->num = CMIC_SCHAN_FIFO_NUM_MAX;

   schan_fifo->lock = sal_spinlock_create("schan_fifo Lock");

   if (schan_fifo->lock == NULL) {
        return SOC_E_MEMORY;
   }

   schan_fifo->ch_map = (0x01 << CMIC_SCHAN_FIFO_NUM_MAX) - 1;
   sal_memcpy(&schan_fifo->config,
                       config, sizeof(soc_schan_fifo_config_t));

   /* perform hardware initialization */

   for (ch = 0 ; ch < CMIC_SCHAN_FIFO_NUM_MAX; ch++) {
       /* Configure AXI ID for SCHAN FIFO */
       val = soc_pci_read(unit, CMIC_SCHAN_FIFO_CHx_CTRL(ch));
       soc_reg_field_set(unit, CMIC_COMMON_POOL_SCHAN_FIFO_0_CH0_CTRLr,
                          &val, AXI_IDf, SCHAN_FIFO_AXI_ID);
       _cmicx_schan_fifo_endian_config(unit, &val);
       soc_pci_write(unit, CMIC_SCHAN_FIFO_CHx_CTRL(ch), val);

       /* Set up summary Register */
       schan_fifo->summary_buff[ch] = soc_cm_salloc(unit,
                   (CMIC_SCHAN_FIFO_CMD_SIZE_MAX * 2), "schan_fifo_summary");
       if (schan_fifo->summary_buff[ch] == NULL) {
           rv = SOC_E_MEMORY;
           break;
       }

       /* write summary Lo address */
       soc_pci_write(unit, CMIC_SCHAN_FIFO_CHx_SUMMARY_ADDR_LOWER(ch),
                       PTR_TO_INT(soc_cm_l2p(unit, schan_fifo->summary_buff[ch])));
       /* write summary Hi address */
       if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
           soc_pci_write(unit, CMIC_SCHAN_FIFO_CHx_SUMMARY_ADDR_UPPER(ch),
                      (PTR_HI_TO_INT(soc_cm_l2p(unit, schan_fifo->summary_buff[ch])) |
                       CMIC_PCIE_SO_OFFSET));
       } else {
           soc_pci_write(unit, CMIC_SCHAN_FIFO_CHx_SUMMARY_ADDR_UPPER(ch),
                      PTR_HI_TO_INT(soc_cm_l2p(unit, schan_fifo->summary_buff[ch])));
       }
   }

   if (SOC_FAILURE(rv)) {
       _soc_cmicx_schan_fifo_deinit(unit);
       return rv;
   }

   /* interrupt initialization */
   rv = _cmicx_schan_fifo_intr_init(unit);

   if (SOC_FAILURE(rv)) {
       _soc_cmicx_schan_fifo_deinit(unit);
       return rv;
   }

    /* Initialize the SCHAN FIFO command memories */
    for (ch = 0; ch < CMIC_SCHAN_FIFO_NUM_MAX; ch++) {
        for (idx = 0;
             idx < (CMIC_SCHAN_FIFO_CMD_SIZE_MAX * (CMIC_SCHAN_WORDS(unit)));
             idx++) {
            soc_pci_write(unit, CMIC_SCHAN_FIFO_CHx_COMMAND(ch, idx), 0);
        }
    }

   drv->soc_schan_fifo_op_prog = _soc_cmicx_schan_fifo_op_prog;
   drv->soc_schan_fifo_op_complete = _soc_cmicx_schan_fifo_op_complete;
   drv->soc_schan_fifo_control = _soc_cmicx_schan_fifo_control;
   drv->soc_schan_fifo_deinit = _soc_cmicx_schan_fifo_deinit;

   return rv;
}


#endif /* BCM_CMICX_SUPPORT  */


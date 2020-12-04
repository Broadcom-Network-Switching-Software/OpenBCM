/*! \file bcmbd_cmicx_led.c
 *
 * CMICx LED host base driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmbd/bcmbd_led_internal.h>
#include <bcmbd/bcmbd_m0ssq.h>
#include <bcmbd/bcmbd_m0ssq_mbox.h>
#include <bcmbd/bcmbd_cmicx_led_internal.h>
#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd_cmicx_led.h>

/*******************************************************************************
 * Local definitions
 */

/* Max number of LED uC. */
#define CMICX_MAX_LEDUPS           (1)

/* M0 core to run LED firmware. */
#define LED_UC                     (0)

/* Address where customer LED FW is downloaded. */
#define LED_FW_OFFSET              (0x3800)

/* Disable CMICx software link overwrite feature. */
#define LED_SW_LINK_DISABLE        (1)

/* Size of portdata memory. */
#define CMICX_LED_CONTROL_DATA_SIZE (1024)

/*******************************************************************************
 * Local variables.
 */
/* CMICx-LED driver specific data. */
static bcmbd_cmicx_led_dev_t *cmicx_led_devs[BCMDRD_CONFIG_MAX_UNITS] = { NULL };

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/*******************************************************************************
 * CMICx-LED host driver to control LED FW by MBOX(mailbox).
 */
static int
cmicx_led_control_data_write(int unit, int led_uc, int offset, uint8_t data)
{
    bcmbd_m0ssq_mbox_msg_t msg;
    bcmbd_m0ssq_mbox_resp_t resp;
    bcmbd_led_control_data_t led_control_data;
    int rv;
    uint32_t rdata[1] = { 0 };
    bcmbd_cmicx_led_dev_t *cmicx_led_dev;

    SHR_FUNC_ENTER(unit);

    if (offset >= CMICX_LED_CONTROL_DATA_SIZE) {
        return SHR_E_PARAM;
    }

    cmicx_led_dev = cmicx_led_devs[unit];
    SHR_NULL_CHECK(cmicx_led_dev, SHR_E_INIT);

    led_control_data.offset = offset;
    led_control_data.data = data;

    /* Prepare the message. */
    msg.id = LED_MSG_CONTROL_DATA_WRITE;
    msg.data = (uint32_t *) &led_control_data;
    msg.datalen = sizeof(led_control_data); /* offset and data */

    /* Request response with payload. */
    resp.data = rdata;
    resp.datalen = sizeof(uint32_t);

    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_mbox_msg_send(unit, cmicx_led_dev->mbox_id,
                                   &msg, &resp));

    /* Check return code */
    rv = rdata[0];
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_led_control_data_read(int unit, int led_uc, int offset, uint8_t *data)
{
    bcmbd_m0ssq_mbox_msg_t msg;
    bcmbd_m0ssq_mbox_resp_t resp;
    bcmbd_led_control_data_t led_control_data;
    int rv;
    uint32_t rdata[1] = { 0 };
    bcmbd_cmicx_led_dev_t *cmicx_led_dev;

    SHR_FUNC_ENTER(unit);

    if (offset >= CMICX_LED_CONTROL_DATA_SIZE) {
        return SHR_E_PARAM;
    }

    cmicx_led_dev = cmicx_led_devs[unit];
    SHR_NULL_CHECK(cmicx_led_dev, SHR_E_INIT);

    led_control_data.offset = offset;
    led_control_data.data = 0;

    /* Prepare the message. */
    msg.id = LED_MSG_CONTROL_DATA_READ;
    msg.data = (uint32_t *) &led_control_data;
    msg.datalen = sizeof(led_control_data); /* offset and data */

    /* Request response with payload. */
    resp.data = rdata;
    resp.datalen = sizeof(uint32_t);

    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_mbox_msg_send(unit, cmicx_led_dev->mbox_id,
                                   &msg, &resp));

    /* Check return code */
    rv = rdata[0];
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    } else {
        *data = (uint8_t) (rdata[0] & 0xFF);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_led_enable(int unit, bool enable)
{
    bcmbd_m0ssq_mbox_msg_t msg;
    bcmbd_m0ssq_mbox_resp_t resp;
    uint32_t enable32;
    bcmbd_cmicx_led_dev_t *cmicx_led_dev;

    SHR_FUNC_ENTER(unit);

    cmicx_led_dev = cmicx_led_devs[unit];
    SHR_NULL_CHECK(cmicx_led_dev, SHR_E_INIT);
    if (enable) {
        enable32 = (LED_SW_LINK_DISABLE << 1) | (1);
    } else {
        enable32 = 0;
    }

    /* Prepare the message. */
    msg.id = LED_MSG_ENABLE;
    msg.datalen = sizeof(enable32);
    msg.data = &enable32;

    /* Request response without payload. */
    resp.data = NULL;
    resp.datalen = 0;

    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_mbox_msg_send(unit, cmicx_led_dev->mbox_id,
                                   &msg, &resp));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * CMICx-LED Driver.
 */
static int
cmicx_led_uc_num_get(int unit)
{
    return CMICX_MAX_LEDUPS;
}

static int
cmicx_led_uc_start_get(int unit, int led_uc, int *start)
{
    bcmbd_cmicx_led_dev_t *cmicx_led_dev;

    SHR_FUNC_ENTER(unit);

    cmicx_led_dev = cmicx_led_devs[unit];
    SHR_NULL_CHECK(cmicx_led_dev, SHR_E_FAIL);
    SHR_NULL_CHECK(start, SHR_E_PARAM);

    /* Get the LED enable state. */
    *start = cmicx_led_dev->enable;

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_led_uc_start_set(int unit, int led_uc, int start)
{
    bcmbd_cmicx_led_dev_t *cmicx_led_dev;
    bool startb = (start != 0);

    SHR_FUNC_ENTER(unit);

    cmicx_led_dev = cmicx_led_devs[unit];
    SHR_NULL_CHECK(cmicx_led_dev, SHR_E_FAIL);

    /* Make sure Broadcom LED firmware is running. */
    SHR_IF_ERR_EXIT(bcmbd_cmicx_m0ssq_fw_led_init(unit, LED_UC));

    /* Enable CMICx LED. */
    SHR_IF_ERR_EXIT(cmicx_led_enable(unit, startb));

    /* Store the state of LED enable. */
    cmicx_led_dev->enable = startb;

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_led_init(int unit)
{
    int port;
    bcmdrd_pbmp_t valid_ppbmp;
    bcmbd_led_dev_t *dev;
    bcmbd_cmicx_led_dev_t *cmicx_led_dev;
    uint32_t mbox_id;

    SHR_FUNC_ENTER(unit);

    /* Get LED driver-specific data. */
    SHR_IF_ERR_EXIT
        (bcmbd_led_dev_get(unit, &dev));

    /* Get valid physical port */
    SHR_IF_ERR_EXIT
        (bcmdrd_dev_phys_pbmp(unit, &valid_ppbmp));

    /* Allocate a MBOX for CMICx-LED */
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_mbox_alloc(unit, "led", &mbox_id));

    /* Let Broadcom LED firmware run. */
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_m0ssq_fw_led_init(unit, LED_UC));

    /* Allocate CMICx-LED driver-specific data */
    cmicx_led_dev = sal_alloc(sizeof(bcmbd_cmicx_led_dev_t), "bcmbdCmicxLedDev");
    SHR_NULL_CHECK(cmicx_led_dev, SHR_E_MEMORY);

    /* Reset the internal led port mapping  */
    for (port = 0; port < BCMDRD_CONFIG_MAX_PORTS; port++) {
        dev->pport_to_led_uc[port] = BCMBD_LED_UC_INVALID;
        dev->pport_to_led_uc_port[port] = 0;
    }

    /* Initialize the internal led port mapping  */
    BCMDRD_PBMP_ITER(valid_ppbmp, port) {
        dev->pport_to_led_uc_port[port] = port - 1;
        dev->pport_to_led_uc[port] = 0;
    }

    /* Offset of available "swdata" space is zero. */
    dev->swdata_start[0] = 0x0;

    /* Initialize CMICx-LED driver specific data. */
    cmicx_led_dev->enable = 0;
    cmicx_led_dev->mbox_id = mbox_id;
    cmicx_led_devs[unit] = cmicx_led_dev;

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_led_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Stop Broadcom LED firmware. */
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_m0ssq_fw_led_cleanup(unit, LED_UC));

    SHR_FREE(cmicx_led_devs[unit]);

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_led_fw_load(int unit, int led_uc, const uint8_t *buf, int len)
{
    /* Load Cusomer LED firmware. */
    return bcmbd_m0ssq_uc_fw_load(unit,
                                  LED_UC,
                                  LED_FW_OFFSET,
                                  buf, len);
}

static int
cmicx_led_pport_to_led_uc_port(int unit, int pport,
                               int *led_uc, int *led_uc_port)
{
    bcmbd_led_dev_t *dev = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_led_dev_get(unit, &dev));

    *led_uc = dev->pport_to_led_uc[pport];
    *led_uc_port = dev->pport_to_led_uc_port[pport];

    if (*led_uc == BCMBD_LED_UC_INVALID) {
        /* The map does not exist. */
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/* CMICx LED host base driver. */
static const bcmbd_led_drv_t bcmbd_cmicx_led_base_drv = {

    /*! Initialize LED driver. */
    .init = cmicx_led_init,

    /*! Clean up LED driver. */
    .cleanup = cmicx_led_cleanup,

    /*! LED firmware loader. */
    .fw_load = cmicx_led_fw_load,

    /*! Get LED uC number */
    .uc_num_get = cmicx_led_uc_num_get,

    /*! Stop/start LED uC. */
    .uc_start_set = cmicx_led_uc_start_set,

    /*! Get LED uC is started or not. */
    .uc_start_get = cmicx_led_uc_start_get,

    /*! Read LED uC swdata */
    .uc_swdata_read = cmicx_led_control_data_read,

    /*! Write LED uC swdata */
    .uc_swdata_write = cmicx_led_control_data_write,

    /*! Physical port to LED uc number and port */
    .pport_to_led_uc_port = cmicx_led_pport_to_led_uc_port,

};

/*******************************************************************************
 * Public Functions.
 */
const bcmbd_led_drv_t *
bcmbd_cmicx_led_base_drv_get(int unit)
{
    return &bcmbd_cmicx_led_base_drv;
}

int
bcmbd_cmicx_led_clk_freq_set(int unit, uint32_t src_clk_freq,
                             uint32_t led_clk_freq)
{
    uint32_t clk_half_period;
    U0_LED_CLK_DIV_CTRLr_t clk_div_ctrl;

    SHR_FUNC_ENTER(unit);

    /* LEDCLK_HALF_PERIOD
     *  = [(required LED clock period in sec)/2]
     *    *(M0SS clock frequency in Hz)]
     *
     *  Where M0SS freqency is 858MHz and
     *  Typical LED clock period is 200ns(5MHz) = 2*10^-7
     */
    clk_half_period = (src_clk_freq + (led_clk_freq / 2)) / led_clk_freq;
    clk_half_period = clk_half_period / 2;

    SHR_IF_ERR_EXIT
        (READ_U0_LED_CLK_DIV_CTRLr(unit, &clk_div_ctrl));
    U0_LED_CLK_DIV_CTRLr_LEDCLK_HALF_PERIODf_SET(clk_div_ctrl, clk_half_period);
    SHR_IF_ERR_EXIT
        (WRITE_U0_LED_CLK_DIV_CTRLr(unit, clk_div_ctrl));

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_cmicx_led_refresh_freq_set(int unit, uint32_t src_clk_freq,
                                 uint32_t refresh_freq)
{
    uint32_t refresh_period;
    U0_LED_REFRESH_CTRLr_t led_refresh_ctrl;

    SHR_FUNC_ENTER(unit);

    /* refresh period
     * = (required refresh period in sec)*(switch clock frequency in Hz)
     */
    refresh_period = (src_clk_freq + (refresh_freq / 2)) / refresh_freq;

    SHR_IF_ERR_EXIT
        (READ_U0_LED_REFRESH_CTRLr(unit, &led_refresh_ctrl));
    U0_LED_REFRESH_CTRLr_REFRESH_CYCLE_PERIODf_SET
         (led_refresh_ctrl, refresh_period);
    SHR_IF_ERR_EXIT
        (WRITE_U0_LED_REFRESH_CTRLr(unit, led_refresh_ctrl));

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_cmicx_led_last_port_set(int unit, uint32_t last_port)
{
    U0_LED_ACCU_CTRLr_t led_accu_ctrl;

    SHR_FUNC_ENTER(unit);

    /* Last port setting of LED */
    SHR_IF_ERR_EXIT
        (READ_U0_LED_ACCU_CTRLr(unit, &led_accu_ctrl));
    U0_LED_ACCU_CTRLr_LAST_PORTf_SET(led_accu_ctrl, last_port);
    SHR_IF_ERR_EXIT
        (WRITE_U0_LED_ACCU_CTRLr(unit, led_accu_ctrl));

exit:
    SHR_FUNC_EXIT();
}

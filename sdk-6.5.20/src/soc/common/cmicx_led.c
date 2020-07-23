/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: CMICx led module
 */
#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <shared/alloc.h>
#include <shared/bsl.h>

#if defined (BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
#include <soc/drv.h>
#include <soc/cm.h>
#include <soc/cmic.h>

#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/iproc_fwconfig.h>
#include <shared/cmicfw/cmicx_led.h>
#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#endif
#include <shared/binfs/binfs.h>
#define SOC_LED_MAX_PHY_PORTS  (SOC_MAX_NUM_PORTS)
#define CMICX_LED_UC           (0)
#define CMICX_LED_FW_OFFSET    (0x3800)

/* CMICx thread safty lock. */
#define CMICX_LED_LOCK(unit, led_uc) \
        sal_mutex_take(SOC_CONTROL(unit)->iproc_m0led_access_lock, sal_mutex_FOREVER);
#define CMICX_LED_UNLOCK(unit, led_uc) \
        sal_mutex_give(SOC_CONTROL(unit)->iproc_m0led_access_lock);

#define LED_SHMEM(unit) (SOC_CONTROL(unit)->led_shmem)
/*
 * Function:
 *      soc_cmicx_led_init
 * Purpose:
 *      Init led module.
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int
soc_cmicx_led_init(int unit)
{
    int rv = SOC_E_NONE;
    soc_control_t   *soc = SOC_CONTROL(unit);

    if(!SOC_CONTROL(unit)->iproc_m0led_init_done) {

        /* Get shared memory of firmware linkscan. */
        SOC_IF_ERROR_RETURN(soc_iproc_m0ssq_shmem_get(unit, "led", &LED_SHMEM(unit)));

        /* Get shared memory of firmware linkscan. */
        SOC_IF_ERROR_RETURN(soc_iproc_m0ssq_shmem_clear(LED_SHMEM(unit)));

        /* Allocate LED mailbox. */
        SOC_IF_ERROR_RETURN(SOC_CONTROL(unit)->led_mbox_id = soc_iproc_mbox_alloc(unit, U0_LED_APP));
        SOC_CONTROL(unit)->led_txmbox= &soc->iproc_mbox_info[SOC_CONTROL(unit)->led_mbox_id][IPROC_MBOX_TYPE_TX];
        SOC_CONTROL(unit)->led_rxmbox= &soc->iproc_mbox_info[SOC_CONTROL(unit)->led_mbox_id][IPROC_MBOX_TYPE_RX];

        /* Allocate CMICx-LED mutex lock. */
        SOC_CONTROL(unit)->iproc_m0led_access_lock = sal_mutex_create("SocLedAccess");

        /* Mark CMICx-LED initializtion done. */
        SOC_CONTROL(unit)->iproc_m0led_init_done = 1;
    }

    return rv;
}

/*
 * Function:
 *      soc_cmicx_led_deinit
 * Purpose:
 *      Cleanup led module.
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int
soc_cmicx_led_deinit(int unit)
{
    int rv = 0;

    if (SOC_CONTROL(unit)->iproc_m0led_init_done) {
        soc_iproc_m0ssq_shmem_free(LED_SHMEM(unit));
        soc_iproc_mbox_free(unit, SOC_CONTROL(unit)->led_mbox_id);
        sal_mutex_destroy(SOC_CONTROL(unit)->iproc_m0led_access_lock);
        SOC_CONTROL(unit)->iproc_m0led_init_done = 0;
    }

    return rv;
}

/*
 * Function:
 *      soc_cmicx_led_fw_dump
 * Purpose:
 *      Dump LED firmware.
 * Parameters:
 *      unit number
 *      led_uc Unused.
 *      len Length of LED FW.
 * Returns:
 *      None.
 */
void
soc_cmicx_led_fw_dump(int unit, int led_uc, int len) {

    soc_iproc_m0ssq_uc_fw_dump(unit, CMICX_LED_UC,
                               CUSTOM_HANDLER_ADDR, len);

}

/*
 * Function:
 *      soc_cmicx_led_link_status_remap
 * Purpose:
 *      Send LED_MSG_LNK_STS message to Cortex-M0 led application.
 * Parameters:
 *      unit number
 *      link_status -- link status for each port
 * Returns:
 *      SOC_E_xxx
 */
int
soc_cmicx_led_link_status_remap(int unit, int *port)
{

    /* Parameter check. */
    if (port == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

#ifdef BCM_TRIDENT3_SUPPORT
    /* LED processor captures the state for the two managment ports in
     * entries 128 and 130 in the accumulation RAM.  The first management
     * port will always have port number [66,129] in the SDK.  This is
     * decremented by one in the LED firmware, so there is no issue.  The
     * second management port reuses port [130,128] in the SDK, which can
     * also be used for PM31, subport 3.  This code differentiates that
     * special case and provides the needed por number for the second
     * management port
     */

    if (SOC_IS_TRIDENT3X(unit) && *port == 130) {
        if (IS_MANAGEMENT_PORT(unit, *port)) {
            *port = 131;
        }
    }
#endif /* BCM_TRIDENT3_SUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT

    /* Mapping xe1 (physical port 258) link status
     * to accumulation entry of physical port 259.
     * User should get port status/speed of xe1 (physical port 258) from the
     * entry of physical port 259 in accumulation RAM and speed array.
     */
     if (SOC_IS_TOMAHAWK3(unit) && *port == 258) {
         *port = 259;
     }
#endif /* BCM_TOMAHAWK3_SUPPORT */
     return SOC_E_NONE;
}

/*
 * Function:
 *      soc_cmicx_led_link_status
 * Purpose:
 *      Send LED_MSG_LNK_STS message to Cortex-M0 led application.
 * Parameters:
 *      unit number
 *      link_status -- link status for each port
 * Returns:
 *      SOC_E_xxx
 */
int
soc_cmicx_led_link_status(int unit, soc_led_link_status_t *link_status)
{
    int rv = SOC_E_NONE;
    int port;
    int led_port, led_uc;
    uint32 addr, value;
    soc_reg_t link_overwrite_reg[] = {
        U0_LED_SW_CNFG_LINK_31_0r,
        U0_LED_SW_CNFG_LINK_63_32r,
        U0_LED_SW_CNFG_LINK_95_64r,
        U0_LED_SW_CNFG_LINK_127_96r,
        U0_LED_SW_CNFG_LINK_159_128r,
        U0_LED_SW_CNFG_LINK_191_160r,
        U0_LED_SW_CNFG_LINK_223_192r,
        U0_LED_SW_CNFG_LINK_255_224r,
        U0_LED_SW_CNFG_LINK_287_256r,
        U0_LED_SW_CNFG_LINK_319_288r,
        U0_LED_SW_CNFG_LINK_351_320r,
        U0_LED_SW_CNFG_LINK_383_352r,
        U0_LED_SW_CNFG_LINK_415_384r,
        U0_LED_SW_CNFG_LINK_447_416r,
        U0_LED_SW_CNFG_LINK_479_448r,
        U0_LED_SW_CNFG_LINK_511_480r,
        U0_LED_SW_CNFG_LINK_543_512r,
        U0_LED_SW_CNFG_LINK_575_544r,
        U0_LED_SW_CNFG_LINK_607_576r,
        U0_LED_SW_CNFG_LINK_639_608r,
        U0_LED_SW_CNFG_LINK_671_640r,
        U0_LED_SW_CNFG_LINK_703_672r,
        U0_LED_SW_CNFG_LINK_735_704r,
        U0_LED_SW_CNFG_LINK_767_736r,
        U0_LED_SW_CNFG_LINK_799_768r,
        U0_LED_SW_CNFG_LINK_831_800r,
        U0_LED_SW_CNFG_LINK_863_832r,
        U0_LED_SW_CNFG_LINK_895_864r,
        U0_LED_SW_CNFG_LINK_927_896r,
        U0_LED_SW_CNFG_LINK_959_928r,
        U0_LED_SW_CNFG_LINK_991_960r,
        U0_LED_SW_CNFG_LINK_1023_992r
    };

    if(!SOC_CONTROL(unit)->iproc_m0led_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Init not done\n")));
        return SOC_E_FAIL;
    }

    /* Parameter check. */
    if (link_status == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

    if (link_status->port >= SOC_LED_MAX_PHY_PORTS) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Port exccess range.\n")));
        return SOC_E_PARAM;
    }

    /* Chip-specific port remap. */
    port = link_status->port;
    SOC_IF_ERROR_RETURN
        (soc_cmicx_led_link_status_remap(unit, &port));

    SOC_IF_ERROR_RETURN
        (soc_cmicx_led_port_to_uc_port_get(unit, port, &led_uc, &led_port));

    /* Set software link overwrite register. */
    addr = soc_reg_addr(unit, link_overwrite_reg[led_port / 32], REG_PORT_ANY, 0);
    soc_iproc_getreg(unit, addr, &value);
    value &= ~(1 << (led_port % 32));
    if (link_status->link_sts) {
        value |= (1 << (led_port % 32));
    }
    soc_iproc_setreg(unit, addr, value);

    return rv;
}

/*
 * Function:
 *      soc_cmicx_led_status_get
 * Purpose:
 *      Send LED_MSG_STATUS message to Cortex-M0 led application.
 * Parameters:
 *      unit number
 *      led_port_status -- get status for the given port
 * Returns:
 *      SOC_E_xxx
 */
int
soc_cmicx_led_status_get(int unit, soc_led_port_status_t *led_port_status)
{
    int rv = SOC_E_NONE;
    soc_iproc_mbox_msg_t *msg, *resp;
    int led_port, led_uc;

    if(!SOC_CONTROL(unit)->iproc_m0led_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Init not done\n")));
        return SOC_E_FAIL;
    }

    /* Parameter check. */
    if (led_port_status == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

    if (led_port_status->port >= SOC_LED_MAX_PHY_PORTS) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Port exccess range.\n")));
        return SOC_E_PARAM;
    }

    msg = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(soc_led_port_status_t));
    if (msg == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Mbox msg alloc failed\n")));
        return SOC_E_MEMORY;
    }

    resp = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(soc_led_port_status_t));
    if (resp == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Mbox resp alloc failed\n")));
        soc_iproc_free(msg);
        return SOC_E_MEMORY;
    }
    /*
     * For DNX, port is passed as physical port that is 0 base,
     * For Firelight, physical port start from 2.
     * but the port was taken as 1 base by M0 led app.
     * So tune it here.
     */
    if (SOC_IS_DNX(unit)) {
        soc_cmicx_led_port_to_uc_port_get(unit, led_port_status->port, &led_uc, &led_port);
        led_port_status->port = led_port + 1;
    } else if (SOC_IS_FIRELIGHT(unit)) {
        led_port_status->port -= 1;
    }

    /* Prepare the message */
    msg->id = LED_MSG_STATUS;
    msg->flags = IPROC_SYNC_MSG | IPROC_RESP_REQUIRED;
    msg->size = sizeof(soc_led_port_status_t) / sizeof(uint32);
    if (sizeof(soc_led_port_status_t) % sizeof(uint32)) {
        msg->size++;
    }

    /* Copy message data */
    sal_memcpy((void *)msg->data, (void *)led_port_status, sizeof(soc_led_port_status_t));

    rv = soc_iproc_data_send_wait(SOC_CONTROL(unit)->led_txmbox, msg, resp);
    if (rv == IPROC_MSG_SUCCESS) {
        if (IS_IPROC_RESP_READY(resp)) {
            if (IS_IPROC_RESP_SUCCESS(resp)) {
                /* get the status */
                led_port_status->status = resp->data[0];
                rv = SOC_E_NONE;
            }
            else {
                rv = resp->data[0];
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "Led msg id 0x%x failed, Error Code %d\n"), msg->id, rv));
            }
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "Led msg id 0x%x send failed, Error Code %d\n"), msg->id, rv));
    }
    soc_iproc_free(msg);
    soc_iproc_free(resp);
    return rv;
}

/*
 * Function:
 *      soc_cmicx_led_status_set
 * Purpose:
 *      Send LED_MSG_STATUS_SET message to Cortex-M0 led application.
 * Parameters:
 *      unit number
 *      led_port_status -- status to be set for the given port
 * Returns:
 *      SOC_E_xxx
 */
int
soc_cmicx_led_status_set(int unit, soc_led_port_status_t *led_port_status)
{
    int rv = SOC_E_NONE;
    soc_iproc_mbox_msg_t *msg, *resp;
    int led_port, led_uc;

    if(!SOC_CONTROL(unit)->iproc_m0led_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Init not done\n")));
        return SOC_E_FAIL;
    }

    /* Parameter check. */
    if (led_port_status == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

    msg = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(soc_led_port_status_t));
    if (msg == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Mbox msg alloc failed\n")));
        return SOC_E_MEMORY;
    }

    resp = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(soc_led_port_status_t));
    if (resp == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Mbox resp alloc failed\n")));
        soc_iproc_free(msg);
        return SOC_E_MEMORY;
    }

    /*
     * For DNX, port is passed as physical port that is 0 base,
     * For Firelight, physical port start from 2.
     * but the port was taken as 1 base by M0 led app.
     * So tune it here.
     */
    if (SOC_IS_DNX(unit)) {
        soc_cmicx_led_port_to_uc_port_get(unit, led_port_status->port, &led_uc, &led_port);
        led_port_status->port = led_port + 1;
    } else if (SOC_IS_FIRELIGHT(unit)) {
        led_port_status->port -= 1;
    }

    /* Prepare the message */
    msg->id = LED_MSG_STATUS_SET;
    msg->flags = IPROC_SYNC_MSG | IPROC_RESP_REQUIRED;
    msg->size = sizeof(soc_led_port_status_t) / sizeof(uint32);
    if (sizeof(soc_led_port_status_t) % sizeof(uint32)) {
        msg->size++;
    }

    /* Copy message data */
    sal_memcpy((void *)msg->data, (void *)led_port_status, sizeof(soc_led_port_status_t));

    rv = soc_iproc_data_send_wait(SOC_CONTROL(unit)->led_txmbox, msg, resp);
    if (rv == IPROC_MSG_SUCCESS) {
        if (IS_IPROC_RESP_READY(resp)) {
            if (IS_IPROC_RESP_SUCCESS(resp)) {
                rv = SOC_E_NONE;
            }
            else {
                rv = resp->data[0];
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "Led msg id 0x%x failed, Error Code %d\n"), msg->id, rv));
            }
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "Led msg id 0x%x send failed, Error Code %d\n"), msg->id, rv));
    }
    soc_iproc_free(msg);
    soc_iproc_free(resp);
    return rv;
}

/*
 * Function:
 *      soc_cmicx_led_enable
 * Purpose:
 *      Send LED_MSG_ENABLE message to Cortex-M0 led application.
 * Parameters:
 *      unit number
 *      enable -- enable/disable the led fw
 * Returns:
 *      SOC_E_xxx
 */
int
soc_cmicx_led_enable(int unit, uint32 enable)
{
    int rv = SOC_E_NONE;
    soc_iproc_mbox_msg_t *msg, *resp;
    soc_control_t   *soc = SOC_CONTROL(unit);

    if (!soc->iproc_m0led_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "LED init not done..skipping %s!\n"),
                     enable ? "start" : "stop"));
        return SOC_E_NONE;
    }

    msg = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(uint32));
    if (msg == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Mbox msg alloc failed\n")));
        return SOC_E_MEMORY;
    }

    resp = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(uint32));
    if (resp == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Mbox resp alloc failed\n")));
        soc_iproc_free(msg);
        return SOC_E_MEMORY;
    }

    /* Prepare the message */
    msg->id = LED_MSG_ENABLE;
    msg->flags = IPROC_SYNC_MSG | IPROC_RESP_REQUIRED;
    msg->size = 1;
    msg->data[0] = enable;

    rv = soc_iproc_data_send_wait(soc->led_txmbox, msg, resp);
    if (rv == IPROC_MSG_SUCCESS) {
        if (IS_IPROC_RESP_READY(resp)) {
            if (IS_IPROC_RESP_SUCCESS(resp)) {
                rv = SOC_E_NONE;
            }
            else {
                rv = resp->data[0];
              }
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "Led msg id 0x%x send failed, Error Code %d\n"), msg->id, rv));
    }
    soc_iproc_free(msg);
    soc_iproc_free(resp);
    return rv;
}

/*
 * Function:
 *      soc_cmicx_led_enable_get
 * Purpose:
 *      Check if LED firmware is enabled or not.
 * Parameters:
 *      unit Unit number
 *      enable Enable state of LED firmware.
 * Returns:
 *      SOC_E_xxx
 */
int
soc_cmicx_led_enable_get(int unit, int *enable) {

    soc_control_t *soc = SOC_CONTROL(unit);
    soc_timeout_t to;
    CMICX_LED_SIGNATUREr_t signature;
    CMICX_LED_CTRLr_t ctrl;

    if (!soc->iproc_m0led_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "LED init not done..skipping!\n")));
        return SOC_E_NONE;
    }

    /* Parameter check. */
    if (enable == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

    /* Check if CMICx-LED FW exist. */
    SOC_IF_ERROR_RETURN
        (READ_CMICX_LED_SIGNATUREr(LED_SHMEM(unit), &signature));

    soc_timeout_init(&to, 10000, 0);
    do {
        SOC_IF_ERROR_RETURN
            (READ_CMICX_LED_SIGNATUREr(LED_SHMEM(unit), &signature));

        if (CMICX_LED_SIGNATUREr_GET(signature) == CMICX_LED_SIGNATURE)
            break;

    } while (!soc_timeout_check(&to));

    if (soc_timeout_check(&to)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "LED is not running.\n")));
        return SOC_E_TIMEOUT;
    }

    /* Get customer LED FW start state. */
    SOC_IF_ERROR_RETURN
        (READ_CMICX_LED_CTRLr(LED_SHMEM(unit), &ctrl));
    *enable = CMICX_LED_CTRLr_STARTf_GET(ctrl);

    return SOC_E_NONE;

}

/*
 * Function:
 *      soc_cmicx_led_speed (obsoleted)
 * Purpose:
 *      Set port_speed array of custom_led.c.
 * Parameters:
 *      unit number
 *      led_port_speed -- speed for the given port
 * Returns:
 *      SOC_E_xxx
 */
int
soc_cmicx_led_speed(int unit, soc_led_port_speed_t *led_port_speed)
{
    int rv = SOC_E_NONE;
    int led_port, led_uc;
    uint8 val8;
    CMICX_LED_CONTROL_DATAr_t ctrl;

    if(!SOC_CONTROL(unit)->iproc_m0led_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Init not done\n")));
        return SOC_E_FAIL;
    }

    /* Parameter check. */
    if (led_port_speed == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

    if (led_port_speed->port >= SOC_LED_MAX_PHY_PORTS)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Port exccess range.\n")));
        return SOC_E_PARAM;
    }

    soc_cmicx_led_port_to_uc_port_get(unit, led_port_speed->port, &led_uc, &led_port);

    CMICX_LED_LOCK(unit, led_uc);

    rv = READ_CMICX_LED_CONTROL_DATAr(LED_SHMEM(unit), led_port / 4, &ctrl);
    if (SOC_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led control data read fail.\n")));
        CMICX_LED_UNLOCK(unit, led_uc);
        return rv;
    }


    /* Port speed is provided in Mbps */
    switch(led_port_speed->speed) {
        case 10:
            val8 = LED_SPD_10M;
            break;
        case 100:
            val8 = LED_SPD_100M;
            break;
        case 1000:
            val8 = LED_SPD_1000M;
            break;
        case 2500:
            val8 = LED_SPD_2500M;
            break;
        case 10000:
            val8 = LED_SPD_10G;
            break;
        case 25000:
            val8 = LED_SPD_25G;
            break;
        case 40000:
            val8 = LED_SPD_40G;
            break;
        case 50000:
            val8 = LED_SPD_50G;
            break;
        case 100000:
            val8 = LED_SPD_100G;
            break;
        case 200000:
            val8 = LED_SPD_200G;
            break;
        case 400000:
            val8 = LED_SPD_400G;
            break;
        default:
            val8 = LED_SPD_INVALID;
            break;
    }

    switch (led_port % 4) {
        case 0:
            CMICX_LED_CONTROL_DATAr_BYTE_0f_SET(ctrl, val8);
            break;
        case 1:
            CMICX_LED_CONTROL_DATAr_BYTE_1f_SET(ctrl, val8);
            break;
        case 2:
            CMICX_LED_CONTROL_DATAr_BYTE_2f_SET(ctrl, val8);
            break;
        case 3:
            CMICX_LED_CONTROL_DATAr_BYTE_3f_SET(ctrl, val8);
            break;
    }

    rv = WRITE_CMICX_LED_CONTROL_DATAr(LED_SHMEM(unit), (led_port / 4), ctrl);
    if (SOC_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led control data write fail.\n")));
    }

    CMICX_LED_UNLOCK(unit, led_uc);

    return rv;
}

/*
 * Function:
 *     soc_cmicx_led_uc_num_get
 * Purpose:
 *     Get number of LED microcontrollers.
 * Parameters:
 *     unit Unit number.
 *     led_uc_num Number of LED microcontrollers.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_cmicx_led_uc_num_get(int unit, int *led_uc_num)
{
    /* Parameter check. */
    if (led_uc_num == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

    *led_uc_num = 1;
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_cmicx_led_control_data_read
 * Purpose:
 *      Read data from led_control_data space.
 * Parameters:
 *      unit Unit number
 *      offset Offset within led_control_data.
 *      data Read result.
 *      len Read length in byte.
 * Returns:
 *      SOC_E_xxx
 */
int
soc_cmicx_led_control_data_read(int unit, int led_uc,
                                int offset, uint8 *data, int len)
{
    int i;
    int rv;
    CMICX_LED_CONTROL_DATAr_t ctrl;

    if(!SOC_CONTROL(unit)->iproc_m0led_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Init not done\n")));
        return SOC_E_FAIL;
    }

    /* Parameter check. */
    if (data == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

    if (offset >= CMICX_LED_CONTROL_DATA_SIZE) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Port exccess range.\n")));
        return SOC_E_PARAM;
    }

    CMICX_LED_LOCK(unit, led_uc);

    for (i = 0; i < len; i++) {

        rv = READ_CMICX_LED_CONTROL_DATAr(LED_SHMEM(unit), (offset + i) / 4, &ctrl);
        if (SOC_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led control data read fail.\n")));
            CMICX_LED_UNLOCK(unit, led_uc);
            return rv;
        }

        switch ((offset + i) % 4) {
            case 0:
                data[i] = CMICX_LED_CONTROL_DATAr_BYTE_0f_GET(ctrl);
                break;
            case 1:
                data[i] = CMICX_LED_CONTROL_DATAr_BYTE_1f_GET(ctrl);
                break;
            case 2:
                data[i] = CMICX_LED_CONTROL_DATAr_BYTE_2f_GET(ctrl);
                break;
            case 3:
                data[i] = CMICX_LED_CONTROL_DATAr_BYTE_3f_GET(ctrl);
                break;
        }

    }

    CMICX_LED_UNLOCK(unit, led_uc);

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_cmicx_led_control_data_write
 * Purpose:
 *     Write control data of LED firmware.
 * Parameters:
 *     unit Unit number
 *     offset Offset within led_control_data.
 *     data Write data.
 *     len Write length in byte.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_cmicx_led_control_data_write(int unit, int led_uc,
                                 int offset, const uint8 *data, int len)
{
    int i;
    int rv;
    CMICX_LED_CONTROL_DATAr_t ctrl;

    if(!SOC_CONTROL(unit)->iproc_m0led_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Init not done\n")));
        return SOC_E_FAIL;
    }

    if (offset >= CMICX_LED_CONTROL_DATA_SIZE) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Port exccess range.\n")));
        return SOC_E_PARAM;
    }

    CMICX_LED_LOCK(unit, led_uc);

    for (i = 0; i < len; i++) {

        rv = READ_CMICX_LED_CONTROL_DATAr(LED_SHMEM(unit), (offset + i) / 4, &ctrl);
        if (SOC_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led control data read fail.\n")));
            CMICX_LED_UNLOCK(unit, led_uc);
            return rv;
        }

        switch ((offset + i) % 4) {
            case 0:
                CMICX_LED_CONTROL_DATAr_BYTE_0f_SET(ctrl, data[i]);
                break;
            case 1:
                CMICX_LED_CONTROL_DATAr_BYTE_1f_SET(ctrl, data[i]);
                break;
            case 2:
                CMICX_LED_CONTROL_DATAr_BYTE_2f_SET(ctrl, data[i]);
                break;
            case 3:
                CMICX_LED_CONTROL_DATAr_BYTE_3f_SET(ctrl, data[i]);
                break;
        }

        rv = WRITE_CMICX_LED_CONTROL_DATAr(LED_SHMEM(unit), (offset + i) / 4, ctrl);
        if (SOC_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led control data write fail.\n")));
            CMICX_LED_UNLOCK(unit, led_uc);
            return rv;
        }
    }

    CMICX_LED_UNLOCK(unit, led_uc);

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_cmicx_led_start_set
 * Purpose:
 *     Start/stop LED firmware.
 * Parameters:
 *     unit Unit number.
 *     led_uc LED microcontroller number.
 *     start 1, request fw start and 0, request fw stop.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_cmicx_led_fw_start_set(int unit, int led_uc, int start)
{
    int enable = 0;
    int rv;

    if(!SOC_CONTROL(unit)->iproc_m0led_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Init not done\n")));
        return SOC_E_FAIL;
    }

    CMICX_LED_LOCK(unit, led_uc);

    start = (start != 0);

    rv = soc_cmicx_led_enable_get(unit, &enable);
    if (SOC_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led enable get fail.\n")));
        CMICX_LED_UNLOCK(unit, led_uc);
        return rv;
    }

    if (enable != start) {
        /* Enable CMICx LED. */
        rv = soc_cmicx_led_enable(unit, start);
        if (SOC_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led enable fail.\n")));
        }
    }

    CMICX_LED_UNLOCK(unit, led_uc);

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_cmicx_led_start_get
 * Purpose:
 *     Get if LED firmware is started or not.
 * Parameters:
 *     unit Unit number.
 *     led_uc LED microcontroller number.
 *     start 1, fw started and 0, fw stopped.
 *     len Length of firmware binary.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_cmicx_led_fw_start_get(int unit, int led_uc, int *start)
{
    int rv;
    int rst;

    CMICX_LED_LOCK(unit, led_uc);

    *start = 0;
    rv = soc_iproc_m0ssq_reset_ucore_get(unit, CMICX_LED_UC, &rst);

    if (SOC_SUCCESS(rv) && rst == 0) {
        rv = soc_cmicx_led_enable_get(unit, start);
    }

    CMICX_LED_UNLOCK(unit, led_uc);

    return rv;
}

/*
 * Function:
 *     soc_cmicx_led_fw_load
 * Purpose:
 *     Load LED firmware binary.
 * Parameters:
 *     unit Unit number
 *     led_uc LED microcontroller number.
 *     data Firmware binary content.
 *     len Length of firmware binary.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_cmicx_led_fw_load(int unit, int led_uc, const uint8 *data, int len)
{
    int rv;

    if (len >= CMICX_LED_FW_SIZE_MAX) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit, "LED Firmware sized exceed %d (%d)\n"),
                 CMICX_LED_FW_SIZE_MAX, len));

    }

    /* Parameter check. */
    if (data == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

    CMICX_LED_LOCK(unit, led_uc);

    rv = soc_iproc_m0ssq_uc_fw_load(unit, CMICX_LED_UC,
                                    CMICX_LED_FW_OFFSET, data, len);

    CMICX_LED_UNLOCK(unit, led_uc);

    return rv;
}

/*
 * Function:
 *     soc_cmicx_led_port_to_uc_port_get
 * Purpose:
 *     Map physical port to LED microcontroller number and port index.
 * Parameters:
 *     unit Unit number.
 *     port Physical port number.
 *     led_uc LED microcontroller number.
 *     led_uc_port Port index controlled by correponding LED microcontroller.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_cmicx_led_port_to_uc_port_get(int unit, int port, int *led_uc, int *led_uc_port)
{
#ifdef BCM_DNX_SUPPORT
    int logic_port,flexe_core_port;
    int rv;
    soc_info_t *si;

    si = &SOC_INFO(unit);
#endif
    /* Parameter check. */
    if (led_uc == NULL || led_uc_port == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }
    if (SOC_IS_FIRELIGHT(unit) && (port < 2)) {
        return SOC_E_PARAM;
    } else if (SOC_IS_ESW(unit) && (port < 1)) {
        return SOC_E_PARAM;
    } else if (port < 0) {
        return SOC_E_PARAM;
    }

    *led_uc = CMICX_LED_UC;
    *led_uc_port = port;

    if (SOC_IS_FIRELIGHT(unit)) {

        /* For Firelight led_uc_port = physical port - 2. */
        *led_uc_port = port - 2;
    } else if (SOC_IS_ESW(unit)) {

        /* For ESW chip led_uc_port = physical port - 1. */
        *led_uc_port = port - 1;
#ifdef BCM_DNX_SUPPORT
    } else if (SOC_IS_Q2A(unit)) {
        /* For flexe phy port.
         * Flexe Port 0 to led scan chain 56
         * Flexe Port 1 to led scan chain 57
         * ...
         * Flexe Port 7 to led scan chain 63
         * Hence uc port = 56 +  Flexe Port.
         */
        logic_port = si->port_p2l_mapping[port];
        if (IS_FLEXE_PHY_PORT(unit, logic_port))
        {
            rv = dnx_algo_port_flexe_core_port_get(unit, logic_port, &flexe_core_port);
            if (SOC_FAILURE(rv)) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                           (BSL_META_U(unit, "FlexE phy port(%d): get uc port error.\n"),logic_port));
                return rv;
            }
            *led_uc_port = 56 + flexe_core_port;
        }
#endif
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_cmicx_led_port_to_uc_port_set
 * Purpose:
 *     Configure the mapping from physical port to LED microcontroller number and port index.
 * Parameters:
 *     unit Unit number.
 *     port Physical port number.
 *     led_uc LED microcontroller number.
 *     led_uc_port Port index controlled by correponding LED microcontroller.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_cmicx_led_port_to_uc_port_set(int unit, int port, int led_uc, int led_uc_port)
{
    /* Unsupport in CMICx-LED. */
    return SOC_E_UNAVAIL;
}

int
soc_cmicx_led_linkscan_default_fw_load(int unit) {

    const uint8 *dp;
    int len;

    SOC_IF_ERROR_RETURN(binfs_file_data_get("linkscan_led_fw.bin", &dp, &len));
    SOC_IF_ERROR_RETURN(soc_iproc_m0ssq_uc_fw_load(unit, CMICX_LED_UC, 0, dp, len));

    SOC_IF_ERROR_RETURN(binfs_file_data_get("custom_led.bin", &dp, &len));
    SOC_IF_ERROR_RETURN(soc_iproc_m0ssq_uc_fw_load(unit, CMICX_LED_UC, CMICX_LED_FW_OFFSET, dp, len));

    return SOC_E_NONE;
}
#endif /*BCM_CMICX_SUPPORT*/
#endif /*BCM_ESW_SUPPORT*/

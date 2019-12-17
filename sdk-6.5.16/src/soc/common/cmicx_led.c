/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
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

/*
 * Function:
 *      soc_cmicx_led_mbox_init
 * Purpose:
 *      Initialize LED MBOX structure.
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int soc_cmicx_led_mbox_init(int unit)
{
    int rv = SOC_E_NONE;
    soc_control_t   *soc = SOC_CONTROL(unit);

    if(!SOC_CONTROL(unit)->iproc_m0led_init_done) {
        SOC_IF_ERROR_RETURN(SOC_CONTROL(unit)->led_mbox_id = soc_iproc_mbox_alloc(unit, U0_LED_APP));
        SOC_CONTROL(unit)->led_txmbox= &soc->iproc_mbox_info[SOC_CONTROL(unit)->led_mbox_id][IPROC_MBOX_TYPE_TX];
        SOC_CONTROL(unit)->led_rxmbox= &soc->iproc_mbox_info[SOC_CONTROL(unit)->led_mbox_id][IPROC_MBOX_TYPE_RX];
        SOC_CONTROL(unit)->iproc_m0led_init_done = 1;
    }

    return rv;
}

/*
 * Function:
 *      soc_cmicx_led_deinit
 * Purpose:
 *      Cleanup led mbox and handler.
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int soc_cmicx_led_deinit(int unit)
{
    int rv = 0;

    if (SOC_CONTROL(unit)->iproc_m0led_init_done) {
        soc_iproc_mbox_free(unit, SOC_CONTROL(unit)->led_mbox_id);
        SOC_CONTROL(unit)->iproc_m0led_init_done = 0;
    }

    return rv;
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
int soc_cmicx_led_link_status(int unit, soc_led_link_status_t *link_status)
{
    int rv = SOC_E_NONE;
    soc_iproc_mbox_msg_t *msg, *resp;

    if(!SOC_CONTROL(unit)->iproc_m0led_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Init not done\n")));
        return SOC_E_FAIL;
    }

    msg = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(soc_led_link_status_t));
    if (msg == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Mbox msg alloc failed\n")));
        return SOC_E_MEMORY;
    }

    resp = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(soc_led_link_status_t));
    if (resp == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Mbox resp alloc failed\n")));
        soc_iproc_free(msg);
        return SOC_E_MEMORY;
    }

    /*
     * For DNX, port is passed as physical port that is 0 base,
     * but the port was taken as 1 base by M0 led app.
     * So tune it here.
     */
    if (SOC_IS_DNX(unit))
    {
        link_status->port += 1;
    }

    /* Prepare the message */
    msg->id = LED_MSG_LNK_STS;
    msg->flags = IPROC_SYNC_MSG | IPROC_RESP_REQUIRED;
    msg->size = sizeof(soc_led_link_status_t) / sizeof(uint32);
    if (sizeof(soc_led_link_status_t) % sizeof(uint32)) {
        msg->size++;
    }

    /* Copy message data */
    sal_memcpy((void *)msg->data, (void *)link_status, sizeof(soc_led_link_status_t));

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
 *      soc_cmicx_led_status_get
 * Purpose:
 *      Send LED_MSG_STATUS message to Cortex-M0 led application.
 * Parameters:
 *      unit number
 *      led_port_status -- get status for the given port
 * Returns:
 *      SOC_E_xxx
 */
int soc_cmicx_led_status_get(int unit, soc_led_port_status_t *led_port_status)
{
    int rv = SOC_E_NONE;
    soc_iproc_mbox_msg_t *msg, *resp;

    if(!SOC_CONTROL(unit)->iproc_m0led_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Init not done\n")));
        return SOC_E_FAIL;
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
     * but the port was taken as 1 base by M0 led app.
     * So tune it here.
     */
    if (SOC_IS_DNX(unit))
    {
        led_port_status->port += 1;
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
int soc_cmicx_led_status_set(int unit, soc_led_port_status_t *led_port_status)
{
    int rv = SOC_E_NONE;
    soc_iproc_mbox_msg_t *msg, *resp;

    if(!SOC_CONTROL(unit)->iproc_m0led_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Init not done\n")));
        return SOC_E_FAIL;
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
     * but the port was taken as 1 base by M0 led app.
     * So tune it here.
     */
    if (SOC_IS_DNX(unit))
    {
        led_port_status->port += 1;
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
int soc_cmicx_led_enable(int unit, uint32 enable)
{
    int rv = SOC_E_NONE;
    soc_iproc_mbox_msg_t *msg, *resp;
    soc_control_t   *soc = SOC_CONTROL(unit);

    if (!SOC_CONTROL(unit)->iproc_m0led_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "LED init not done..skipping %s!\n"), enable ? "start" : "stop"));
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

    rv = soc_iproc_data_send_wait(SOC_CONTROL(unit)->led_txmbox, msg, resp);
    if (rv == IPROC_MSG_SUCCESS) {
        if (IS_IPROC_RESP_READY(resp)) {
            if (IS_IPROC_RESP_SUCCESS(resp)) {
                rv = SOC_E_NONE;
                soc->led_fw_enabled = enable;
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
 *      soc_cmicx_led_speed
 * Purpose:
 *      Send LED_MSG_SPEED message to Cortex-M0 led application.
 * Parameters:
 *      unit number
 *      led_port_speed -- speed for the given port
 * Returns:
 *      SOC_E_xxx
 */
int soc_cmicx_led_speed(int unit, soc_led_port_speed_t *led_port_speed)
{
    int rv = SOC_E_NONE;
    soc_iproc_mbox_msg_t *msg, *resp;

    if(!SOC_CONTROL(unit)->iproc_m0led_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Led Init not done\n")));
        return SOC_E_FAIL;
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

    /* Prepare the message */
    msg->id = LED_MSG_SPEED;
    msg->flags = IPROC_SYNC_MSG | IPROC_RESP_REQUIRED;
    msg->size = sizeof(soc_led_port_speed_t) / sizeof(uint32);
    if (sizeof(soc_led_port_speed_t) % sizeof(uint32)) {
        msg->size++;
    }

    /* Copy message data */
    sal_memcpy((void *)msg->data, (void *)led_port_speed, sizeof(soc_led_port_speed_t));

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

#endif /*BCM_CMICX_SUPPORT*/
#endif /*BCM_ESW_SUPPORT*/

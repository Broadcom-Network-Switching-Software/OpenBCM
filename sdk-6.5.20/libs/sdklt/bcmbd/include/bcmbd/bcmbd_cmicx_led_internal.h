/*! \file bcmbd_cmicx_led_internal.h
 *
 *  CMICx LED host base driver.
 *
 *    This driver is a generic part of CMICx-LED host driver.
 * This driver will
 *    - Download Broadcom CMICx-LED FW when LED driver initialization.
 *    - Download Customer CMICx-LED FW as API/CLI request.
 *    - Enable/Disable LED FW by mailbox driver.
 *    - Read/write LED FW's port data(swdata) by mailbox driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_LED_INTERNAL_H
#define BCMBD_CMICX_LED_INTERNAL_H

/*! Message id for CMICx LED mailbox. */
typedef enum bcmbd_led_msg_id_e {

    /*! (Obsolete) get port status. */
    LED_MSG_LNK_STS = 0,

    /*! (Obsolete) set software link pbmp. */
    LED_MSG_STATUS,

    /*! Enable LED FW. */
    LED_MSG_ENABLE,

    /*! (Obsolete) set port status. */
    LED_MSG_STATUS_SET,

    /*! (Obsolete) set port speed. */
    LED_MSG_SPEED,

    /*! LED FW led control data write. */
    LED_MSG_CONTROL_DATA_WRITE,

    /*! LED FW led control data read. */
    LED_MSG_CONTROL_DATA_READ,

} bcmbd_led_msg_id_t;

/*! Definition struct for LED mbox */
typedef struct bcmbd_led_control_data_s {

    /*! Offset of led control data. */
    uint32_t offset;

    /*! Data of led control data (only valid in bit0~bit7). */
    uint32_t data;

} bcmbd_led_control_data_t;

/*! CMICX-LED device-specific data. */
typedef struct bcmbd_cmicx_led_dev_s {

    /*! Flag indicates led enable/disable. */
    bool enable;

    /*! led mailbox id. */
    uint32_t mbox_id;

} bcmbd_cmicx_led_dev_t;

#endif /* BCMBD_CMICX_LED_INTERNAL_H */

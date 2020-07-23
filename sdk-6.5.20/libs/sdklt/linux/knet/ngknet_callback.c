/*! \file ngknet_callback.c
 *
 * Utility routines for NGKNET callbacks.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "ngknet_callback.h"

static struct ngknet_callback_ctrl callback_ctrl;

int
ngknet_callback_control_get(struct ngknet_callback_ctrl **cbc)
{
    *cbc = &callback_ctrl;

    return 0;
}

/*!
 * Call-back interfaces for other Linux kernel drivers.
 *
 * The Rx call-back allows an external module to modify packet contents
 * before it is handed off to the Linux network stack.
 *
 * The Tx call-back allows an external module to modify packet contents
 * before it is injected into the switch.
 */

int
ngknet_rx_cb_register(ngknet_rx_cb_f rx_cb)
{
    if (callback_ctrl.rx_cb != NULL) {
        return -1;
    }
    callback_ctrl.rx_cb = rx_cb;

    return 0;
}

int
ngknet_rx_cb_unregister(ngknet_rx_cb_f rx_cb)
{
    if (rx_cb == NULL || callback_ctrl.rx_cb != rx_cb) {
        return -1;
    }
    callback_ctrl.rx_cb = NULL;

    return 0;
}

int
ngknet_tx_cb_register(ngknet_tx_cb_f tx_cb)
{
    if (callback_ctrl.tx_cb != NULL) {
        return -1;
    }
    callback_ctrl.tx_cb = tx_cb;

    return 0;
}

int
ngknet_tx_cb_unregister(ngknet_tx_cb_f tx_cb)
{
    if (tx_cb == NULL || callback_ctrl.tx_cb != tx_cb) {
        return -1;
    }
    callback_ctrl.tx_cb = NULL;

    return 0;
}

EXPORT_SYMBOL(ngknet_rx_cb_register);
EXPORT_SYMBOL(ngknet_rx_cb_unregister);
EXPORT_SYMBOL(ngknet_tx_cb_register);
EXPORT_SYMBOL(ngknet_tx_cb_unregister);


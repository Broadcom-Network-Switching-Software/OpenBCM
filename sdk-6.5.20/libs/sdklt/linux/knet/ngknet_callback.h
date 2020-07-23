/*! \file ngknet_callback.h
 *
 * Data structure definitions for NGKNET callbacks.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef NGKNET_CALLBACK_H
#define NGKNET_CALLBACK_H

#include <linux/skbuff.h>
#include "ngknet_main.h"

/*!
 * \brief NGKNET callback description.
 */
struct ngknet_callback_desc {
    /*! Device number */
    int dev_no;

    /*! Device ID */
    uint32_t dev_id;

    /*! Device type string */
    const char *type_str;

    /*! Network interface private data */
    struct ngknet_private *priv;

    /*! Matched filter */
    struct ngknet_filter_s *filt;

    /*! Packet meta data */
    uint8_t *pmd;

    /*! Packet meta data length */
    int pmd_len;

    /*! Packet data length */
    int pkt_len;
};

#define NGKNET_SKB_CB(_skb) ((struct ngknet_callback_desc *)_skb->cb)

/*! Handle Rx packet */
typedef struct sk_buff *
(*ngknet_rx_cb_f)(struct sk_buff *skb);

/*! Handle Tx packet */
typedef struct sk_buff *
(*ngknet_tx_cb_f)(struct sk_buff *skb);

/*!
 * \brief NGKNET callback control.
 */
struct ngknet_callback_ctrl {
    /*! Handle Rx packet */
    ngknet_rx_cb_f rx_cb;

    /*! Handle Tx packet */
    ngknet_tx_cb_f tx_cb;
};

/*!
 * \brief Get callback control.
 *
 * \param [in] cbc Pointer to callback control.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_callback_control_get(struct ngknet_callback_ctrl **cbc);

/*!
 * \brief Register Rx callback.
 *
 * \param [rx_cb] rx_cb Rx callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_rx_cb_register(ngknet_rx_cb_f rx_cb);

/*!
 * \brief Unregister Rx callback.
 *
 * \param [rx_cb] rx_cb Rx callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_rx_cb_unregister(ngknet_rx_cb_f rx_cb);

/*!
 * \brief Register Tx callback.
 *
 * \param [tx_cb] tx_cb Tx callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_tx_cb_register(ngknet_tx_cb_f tx_cb);

/*!
 * \brief Unregister Tx callback.
 *
 * \param [tx_cb] tx_cb Tx callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_tx_cb_unregister(ngknet_tx_cb_f tx_cb);

#endif /* NGKNET_CALLBACK_H */


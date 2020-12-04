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

/*! PTP Rx/Tx config set */
typedef int
(*ngknet_ptp_config_set_cb_f)(struct ngknet_private *priv, int *value);

/*! PTP Rx/Tx HW timestamp get */
typedef int
(*ngknet_ptp_hwts_get_cb_f)(struct sk_buff *skb, uint64_t *ts);

/*! PTP Tx meta set */
typedef int
(*ngknet_ptp_meta_set_cb_f)(struct sk_buff *skb);

/*! PTP PHC index get */
typedef int
(*ngknet_ptp_phc_index_get_cb_f)(struct ngknet_private *priv, int *index);

/*! PTP device control */
typedef int
(*ngknet_ptp_dev_ctrl_cb_f)(struct ngknet_dev *dev, int cmd, char *data, int len);

/*!
 * \brief NGKNET callback control.
 */
struct ngknet_callback_ctrl {
    /*! Handle Rx packet */
    ngknet_rx_cb_f rx_cb;

    /*! Handle Tx packet */
    ngknet_tx_cb_f tx_cb;

    /*! PTP Rx config set */
    ngknet_ptp_config_set_cb_f ptp_rx_config_set_cb;

    /*! PTP Tx config set */
    ngknet_ptp_config_set_cb_f ptp_tx_config_set_cb;

    /*! PTP Rx HW timestamp get */
    ngknet_ptp_hwts_get_cb_f ptp_rx_hwts_get_cb;

    /*! PTP Tx HW timestamp get */
    ngknet_ptp_hwts_get_cb_f ptp_tx_hwts_get_cb;

    /*! PTP Tx meta set */
    ngknet_ptp_meta_set_cb_f ptp_tx_meta_set_cb;

    /*! PTP PHC index get */
    ngknet_ptp_phc_index_get_cb_f ptp_phc_index_get_cb;

    /*! PTP device control */
    ngknet_ptp_dev_ctrl_cb_f ptp_dev_ctrl_cb;
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
 * \param [in] rx_cb Rx callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_rx_cb_register(ngknet_rx_cb_f rx_cb);

/*!
 * \brief Unregister Rx callback.
 *
 * \param [in] rx_cb Rx callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_rx_cb_unregister(ngknet_rx_cb_f rx_cb);

/*!
 * \brief Register Tx callback.
 *
 * \param [in] tx_cb Tx callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_tx_cb_register(ngknet_tx_cb_f tx_cb);

/*!
 * \brief Unregister Tx callback.
 *
 * \param [in] tx_cb Tx callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_tx_cb_unregister(ngknet_tx_cb_f tx_cb);

/*!
 * \brief Register PTP Rx config set callback.
 *
 * \param [in] ptp_rx_config_set_cb Rx config set callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_rx_config_set_cb_register(ngknet_ptp_config_set_cb_f ptp_rx_config_set_cb);

/*!
 * \brief Unregister PTP Rx config set callback.
 *
 * \param [in] ptp_rx_config_set_cb Rx config set callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_rx_config_set_cb_unregister(ngknet_ptp_config_set_cb_f ptp_rx_config_set_cb);

/*!
 * \brief Register PTP Tx config set callback.
 *
 * \param [in] ptp_tx_config_set_cb Tx config set callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_tx_config_set_cb_register(ngknet_ptp_config_set_cb_f ptp_tx_config_set_cb);

/*!
 * \brief Unregister PTP Tx config set callback.
 *
 * \param [in] ptp_tx_config_set_cb Tx config set callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_tx_config_set_cb_unregister(ngknet_ptp_config_set_cb_f ptp_tx_config_set_cb);

/*!
 * \brief Register PTP Rx HW timestamp get callback.
 *
 * \param [in] ptp_rx_hwts_get_cb Rx HW timestamp get callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_rx_hwts_get_cb_register(ngknet_ptp_hwts_get_cb_f ptp_rx_hwts_get_cb);

/*!
 * \brief Unregister PTP Rx HW timestamp get callback.
 *
 * \param [in] ptp_rx_hwts_get_cb Rx HW timestamp get callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_rx_hwts_get_cb_unregister(ngknet_ptp_hwts_get_cb_f ptp_rx_hwts_get_cb);

/*!
 * \brief Register PTP Tx HW timestamp get callback.
 *
 * \param [in] ptp_tx_hwts_get_cb Tx HW timestamp get callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_tx_hwts_get_cb_register(ngknet_ptp_hwts_get_cb_f ptp_tx_hwts_get_cb);

/*!
 * \brief Unregister PTP Tx HW timestamp get callback.
 *
 * \param [in] ptp_tx_hwts_get_cb Tx HW timestamp get callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_tx_hwts_get_cb_unregister(ngknet_ptp_hwts_get_cb_f ptp_tx_hwts_get_cb);

/*!
 * \brief Register PTP Tx meta set callback.
 *
 * \param [in] ptp_tx_meta_set_cb Tx meta set callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_tx_meta_set_cb_register(ngknet_ptp_meta_set_cb_f ptp_tx_meta_set_cb);

/*!
 * \brief Unregister PTP Tx meta set callback.
 *
 * \param [in] ptp_tx_meta_set_cb Tx meta set callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_tx_meta_set_cb_unregister(ngknet_ptp_meta_set_cb_f ptp_tx_meta_set_cb);

/*!
 * \brief Register PTP PHC index get callback.
 *
 * \param [in] ptp_phc_index_get_cb PHC index get callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_phc_index_get_cb_register(ngknet_ptp_phc_index_get_cb_f ptp_phc_index_get_cb);

/*!
 * \brief Unregister PTP PHC index get callback.
 *
 * \param [in] ptp_phc_index_get_cb PHC index get callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_phc_index_get_cb_unregister(ngknet_ptp_phc_index_get_cb_f ptp_phc_index_get_cb);

/*!
 * \brief Register PTP device control callback.
 *
 * \param [in] ptp_dev_ctrl_cb Device control callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_dev_ctrl_cb_register(ngknet_ptp_dev_ctrl_cb_f ptp_dev_ctrl_cb);

/*!
 * \brief Unregister PTP device control callback.
 *
 * \param [in] ptp_dev_ctrl_cb Device control callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_dev_ctrl_cb_unregister(ngknet_ptp_dev_ctrl_cb_f ptp_dev_ctrl_cb);

#endif /* NGKNET_CALLBACK_H */


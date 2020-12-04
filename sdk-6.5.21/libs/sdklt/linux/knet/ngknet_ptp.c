/*! \file ngknet_ptp.c
 *
 * Utility routines for PTP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "ngknet_callback.h"
#include "ngknet_ptp.h"

int
ngknet_ptp_rx_config_set(struct net_device *ndev, int *filter)
{
    struct ngknet_private *priv = netdev_priv(ndev);
    struct ngknet_dev *dev = priv->bkn_dev;

    if (!dev->cbc->ptp_rx_config_set_cb) {
        return SHR_E_UNAVAIL;
    }

    /*
     * The expected Rx filter value is passed to the callback. The callback
     * should pass back the actual filter value by the paramter <filter>.
     * The callback can use priv->user_data to get other private parameters
     * such as phys_port, dev_type, etc, which should be introduced when the
     * netif is created.
     */
    return dev->cbc->ptp_rx_config_set_cb(priv, filter);
}

int
ngknet_ptp_tx_config_set(struct net_device *ndev, int type)
{
    struct ngknet_private *priv = netdev_priv(ndev);
    struct ngknet_dev *dev = priv->bkn_dev;

    if (!dev->cbc->ptp_tx_config_set_cb) {
        return SHR_E_UNAVAIL;
    }

    /*
     * The Tx type value is passed to the callback by the parameter <type>.
     * The callback should do the configuration according to the type.
     * The callback can use priv->user_data to get other private parameters
     * such as phys_port, dev_type, etc, which should be introduced when the
     * netif is created.
     */
    return dev->cbc->ptp_tx_config_set_cb(priv, &type);
}

int
ngknet_ptp_rx_hwts_get(struct net_device *ndev, struct sk_buff *skb, uint64_t *ts)
{
    struct ngknet_private *priv = netdev_priv(ndev);
    struct ngknet_dev *dev = priv->bkn_dev;
    struct ngknet_callback_desc *cbd = NGKNET_SKB_CB(skb);
    struct pkt_hdr *pkh = (struct pkt_hdr *)skb->data;

    if (!dev->cbc->ptp_rx_hwts_get_cb) {
        return SHR_E_UNAVAIL;
    }

    cbd->dev_no = dev->dev_no;
    cbd->dev_id = dev->pdma_dev.dev_id;
    cbd->priv = priv;
    cbd->pmd = skb->data + PKT_HDR_SIZE;
    cbd->pmd_len = pkh->meta_len;
    cbd->pkt_len = pkh->data_len;

    /*
     * The callback should get timestamp value for a Rx packet and return
     * by the parameter <ts>.
     * Some parameters have been consolidated to SKB as above. They can be
     * achieved by NGKNET_SKB_CB(skb). Specially more private paramters are
     * in NGKNET_SKB_CB(skb)->priv->user_data[].
     */
    return dev->cbc->ptp_rx_hwts_get_cb(skb, ts);
}

int
ngknet_ptp_tx_hwts_get(struct net_device *ndev, struct sk_buff *skb, uint64_t *ts)
{
    struct ngknet_private *priv = netdev_priv(ndev);
    struct ngknet_dev *dev = priv->bkn_dev;
    struct ngknet_callback_desc *cbd = NGKNET_SKB_CB(skb);
    struct pkt_hdr *pkh = (struct pkt_hdr *)skb->data;

    if (!dev->cbc->ptp_tx_hwts_get_cb) {
        return SHR_E_UNAVAIL;
    }

    cbd->dev_no = dev->dev_no;
    cbd->dev_id = dev->pdma_dev.dev_id;
    cbd->priv = priv;
    cbd->pmd = skb->data + PKT_HDR_SIZE;
    cbd->pmd_len = pkh->meta_len;
    cbd->pkt_len = pkh->data_len;

    /*
     * The callback should get timestamp value for a Tx packet and return
     * by the parameter <ts>.
     * For HWTSTAMP_TX_ONESTEP_SYNC type, the time value can be achieved and
     * returned immediately. Otherwise, for HWTSTAMP_TX_ON type, the callback
     * should wait till the time value can be available, i.e. the packet has
     * been tranmitted out from port.
     * Some parameters have been consolidated to SKB as above. They can be
     * achieved by NGKNET_SKB_CB(skb). Specially more private paramters are
     * in NGKNET_SKB_CB(skb)->priv->user_data[] such as phys_port, dev_type,
     * and so on.
     */
    return dev->cbc->ptp_tx_hwts_get_cb(skb, ts);
}

int
ngknet_ptp_tx_meta_set(struct net_device *ndev, struct sk_buff *skb)
{
    struct ngknet_private *priv = netdev_priv(ndev);
    struct ngknet_dev *dev = priv->bkn_dev;
    struct ngknet_callback_desc *cbd = NGKNET_SKB_CB(skb);
    struct pkt_hdr *pkh = (struct pkt_hdr *)skb->data;

    if (!dev->cbc->ptp_tx_meta_set_cb) {
        return SHR_E_UNAVAIL;
    }

    cbd->dev_no = dev->dev_no;
    cbd->dev_id = dev->pdma_dev.dev_id;
    cbd->priv = priv;
    cbd->pmd = skb->data + PKT_HDR_SIZE;
    cbd->pmd_len = pkh->meta_len;
    cbd->pkt_len = pkh->data_len;

    /*
     * The callback should configure the metadata <NGKNET_SKB_CB(skb)->pmd>
     * for HW timestamping according to the corresponding switch device.
     * Some parameters have been consolidated to SKB as above. They can be
     * achieved by NGKNET_SKB_CB(skb). Specially more private paramters are
     * in NGKNET_SKB_CB(skb)->priv->user_data[] such as phys_port, dev_type,
     * and so on.
     */
    return dev->cbc->ptp_tx_meta_set_cb(skb);
}

int
ngknet_ptp_phc_index_get(struct net_device *ndev, int *index)
{
    struct ngknet_private *priv = netdev_priv(ndev);
    struct ngknet_dev *dev = priv->bkn_dev;

    if (!dev->cbc->ptp_phc_index_get_cb) {
        return SHR_E_UNAVAIL;
    }

    /*
     * The callback should return the HPC index by the parameter <index>.
     * priv->user_data[] can be used to get other private parameters such as
     * phys_port, dev_type, etc, which should be introduced when the netif is
     * created.
     */
    return dev->cbc->ptp_phc_index_get_cb(priv, index);
}

int
ngknet_ptp_dev_ctrl(struct ngknet_dev *dev, int cmd, char *data, int len)
{
    if (!dev->cbc->ptp_dev_ctrl_cb) {
        return SHR_E_UNAVAIL;
    }

    /*
     * The callback is IOCTL dispatcher for PTP driver/module.
     * The parameter <cmd> is the command defined by the user. The parameter
     * <data> and <len> are used for interactions between the user application
     * and the driver for PTP device start/stop, enable/disable, set/get, and
     * so on.
     */
    return dev->cbc->ptp_dev_ctrl_cb(dev, cmd, data, len);
}


/*! \file ngknet_ptp.h
 *
 * Definitions and APIs declaration for PTP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef NGKNET_PTP_H
#define NGKNET_PTP_H

#include <linux/skbuff.h>
#include "ngknet_main.h"

/*!
 * \brief PTP Rx config set.
 *
 * \param [in] ndev Network device structure point.
 * \param [in] filter Rx filter.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_rx_config_set(struct net_device *ndev, int *filter);

/*!
 * \brief PTP Tx config set.
 *
 * \param [in] ndev Network device structure point.
 * \param [in] type Tx type.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_tx_config_set(struct net_device *ndev, int type);

/*!
 * \brief PTP Rx HW timestamping get.
 *
 * \param [in] ndev Network device structure point.
 * \param [in] skb Rx packet SKB.
 * \param [out] ts Timestamp value.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_rx_hwts_get(struct net_device *ndev, struct sk_buff *skb, uint64_t *ts);

/*!
 * \brief PTP Tx HW timestamping get.
 *
 * \param [in] ndev Network device structure point.
 * \param [in] skb Tx packet SKB.
 * \param [out] ts Timestamp value.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_tx_hwts_get(struct net_device *ndev, struct sk_buff *skb, uint64_t *ts);

/*!
 * \brief PTP Tx meta set.
 *
 * \param [in] ndev Network device structure point.
 * \param [in] skb Tx packet SKB.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_tx_meta_set(struct net_device *ndev, struct sk_buff *skb);

/*!
 * \brief PTP PHC index get.
 *
 * \param [in] ndev Network device structure point.
 * \param [out] index PHC index.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_phc_index_get(struct net_device *ndev, int *index);

/*!
 * \brief PTP device control.
 *
 * \param [in] dev NGKNET device structure point.
 * \param [in] cmd Command.
 * \param [in] data Data buffer.
 * \param [in] len Data length.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
ngknet_ptp_dev_ctrl(struct ngknet_dev *dev, int cmd, char *data, int len);

#endif /* NGKNET_PTP_H */


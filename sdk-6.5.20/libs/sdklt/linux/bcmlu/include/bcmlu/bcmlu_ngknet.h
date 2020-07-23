/*! \file bcmlu_ngknet.h
 *
 * Generic data structure and macro definitions for user interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLU_NGKNET_H
#define BCMLU_NGKNET_H

#include <lkm/ngknet_dev.h>

/*!
 * Maximum Network interface ID number.
 */
#define BCMLU_NETIF_MAX_IDS            NUM_VDEV_MAX

/*! Check if netif ID number is within the netif supported range.
 * 0 is used for device management.
 */
#define BCMLU_NETIF_VALID(_n) ((_n > 0) && (_n <= BCMLU_NETIF_MAX_IDS))

/*!
 * Maximum filter number.
 */
#define BCMLU_FILTER_MAX               NUM_FILTER_MAX

/*!
 * \brief Open device file(s) and check KNET version.
 *
 * Open files needed for device IOCTL.
 *
 * If successful, then check that the KNET kernel module is compatible
 * with the user mode interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Open device file failed.
 * \retval SHR_E_CONFIG Incompatible KNET kernel module.
 */
extern int
bcmlu_ngknet_dev_check(void);

/*!
 * \brief Set Rx rate limit.
 *
 * \param [in] rate_max Max Rx packets per second.
 * \param [in] set Set Rx rate limit if TRUE, or get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_rx_rate_limit(int *rate_max, int set);

/*!
 * \brief Initialize the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 * \param [in] dev_cfg Device configure structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_init(int unit, ngknet_dev_cfg_t *dev_cfg);

/*!
 * \brief Clean up the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_cleanup(int unit);

/*!
 * \brief Close open device files(s).
 *
 * Close any open files that have been used for device IOCTL.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmlu_ngknet_dev_close(void);

/*!
 * \brief Configure the kernel networking subsystem channel.
 *
 * \param [in] unit Device number.
 * \param [in] chan_cfg Channel configure structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_chan_config(int unit, ngknet_chan_cfg_t *chan_cfg);

/*!
 * \brief Query the kernel networking subsystem channel.
 *
 * \param [in] unit Device number.
 * \param [in] chan_cfg Channel configure structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_chan_query(int unit, ngknet_chan_cfg_t *chan_cfg);

/*!
 * \brief Suspend the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_suspend(int unit);

/*!
 * \brief Resume the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_resume(int unit);

/*!
 * \brief Configure RCPU header for the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 * \param [in] rcpu RCPU configure data structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_rcpu_config(int unit, struct ngknet_rcpu_hdr *rcpu);

/*!
 * \brief Get RCPU header for the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 * \param [in] rcpu RCPU configure data structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_rcpu_get(int unit, struct ngknet_rcpu_hdr *rcpu);

/*!
 * \brief Get the kernel networking subsystem information.
 *
 * \param [in] unit Device number.
 * \param [in] info Device information structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_info_get(int unit, struct bcmcnet_dev_info *info);

/*!
 * \brief Get the kernel networking subsystem stats.
 *
 * \param [in] unit Device number.
 * \param [in] stats Device stats structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_stats_get(int unit, struct bcmcnet_dev_stats *stats);

/*!
 * \brief Reset the kernel networking subsystem stats.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_stats_reset(int unit);

/*!
 * \brief Create a kernel network interface.
 *
 * \param [in] unit Device number.
 * \param [in] netif Network interface structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_netif_create(int unit, ngknet_netif_t *netif);

/*!
 * \brief Destroy a kernel network interface.
 *
 * \param [in] unit Device number.
 * \param [in] netif_id Network interface ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_netif_destroy(int unit, int netif_id);

/*!
 * \brief Get a kernel network interface.
 *
 * \param [in] unit Device number.
 * \param [in] netif_id Network interface ID.
 * \param [in] netif Network interface structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_netif_get(int unit, int netif_id, ngknet_netif_t *netif);

/*!
 * \brief Get the next kernel network interface.
 *
 * \param [in] unit Device number.
 * \param [in] netif Network interface structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_netif_get_next(int unit, ngknet_netif_t *netif);

/*!
 * \brief Set link status for a network interface.
 *
 * \param [in] unit Device number.
 * \param [in] netif_id Network interface ID.
 * \param [in] up Link status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_netif_link_set(int unit, int netif_id, int up);

/*!
 * \brief Create a kernel packet filter.
 *
 * \param [in] unit Device number.
 * \param [in] filter Filter structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_filter_create(int unit, ngknet_filter_t *filter);

/*!
 * \brief Destroy a kernel packet filter.
 *
 * \param [in] unit Device number.
 * \param [in] filter_id Filter ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_filter_destroy(int unit, int filter_id);

/*!
 * \brief Get a kernel packet filter.
 *
 * \param [in] unit Device number.
 * \param [in] filter_id Filter ID.
 * \param [in] filter Filter structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_filter_get(int unit, int filter_id, ngknet_filter_t *filter);

/*!
 * \brief Get the next kernel packet filter.
 *
 * \param [in] unit Device number.
 * \param [in] filter Filter structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_filter_get_next(int unit, ngknet_filter_t *filter);

#endif /* BCMLU_NGKNET_H */


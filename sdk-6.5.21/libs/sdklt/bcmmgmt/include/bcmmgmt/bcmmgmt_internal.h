/*! \file bcmmgmt_internal.h
 *
 * BCMMGMT internal header file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMMGMT_INTERNAL_H
#define BCMMGMT_INTERNAL_H

#include <bcmmgmt/bcmmgmt.h>

/*!
 * \brief Start the System Manager components.
 *
 * Register specified components with the System Manager and commence
 * system initialization.
 *
 * \param [in] warm Set to true to request warm-boot initialization.
 * \param [in] comp_list List of components to start.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int
bcmmgmt_sysm_components_start(bool warm, const bcmmgmt_comp_list_t *comp_list);

/*!
 * \brief Stop the System Manager components.
 *
 * This function should be called after all the units have been
 * detached and all the unit-based System Manager instances have
 * stopped.
 *
 * This function stops the generic category of the System Manager and
 * then clears the System Manager local states.
 *
 * \param [in] graceful Set to true to complete all committed transactions,
 *                      otherwise only transactions in progress will be
 *                      completed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more components failed to stop.
 */
extern int
bcmmgmt_sysm_components_stop(bool graceful);

/*!
 * \brief Initialize default packet buffer pools.
 *
 * \return SHR_E_NONE on success, otherwise error code.
 */
extern int
bcmmgmt_pkt_core_init(void);

/*!
 * \brief Clean up packet buffer pools.
 *
 * \return SHR_E_NONE on success, otherwise error code.
 */
extern int
bcmmgmt_pkt_core_cleanup(void);

/*!
 * \brief Initialize packet I/O for a device.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Packet I/O failed to initialize.
 */
extern int
bcmmgmt_pkt_dev_init(int unit, bool warm);

/*!
 * \brief Clean up packet I/O for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Packet I/O failed to initialize.
 */
extern int
bcmmgmt_pkt_dev_cleanup(int unit);

/*!
 * \brief Clean up packet I/O for a device.
 *
 * This function will not destroy netifs and packet I/O DMA channels.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Packet I/O failed to initialize.
 */
extern int
bcmmgmt_pkt_dev_cleanup_keep_netif(int unit);

/*!
 * \brief Shut down all core components of the SDK.
 *
 * This function performs the real work of \ref bcmmgmt_core_stop, but
 * accepts an extra parameter to determine whether to shutdown packet
 * I/O DMA pools (see \ref bcmmgmt_pkt_dev_cleanup_keep_netif). This
 * option is typically used to allow an O/S network driver (KNET) to
 * keep functioning even if the user mode part of the SDK is shut
 * down.
 *
 * \param [in] graceful Indicates if the shutdown should complete all
 *                      pending LT operations or simply shut down as
 *                      fast as possible.
 * \param [in] keep_netif Set to true to keep packet I/O running.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more core components did not start correctly.
 */
extern int
bcmmgmt_core_internal_stop(bool graceful, bool keep_netif);

/*!
 * \brief Detach a device.
 *
 * This function performs the real work of \ref bcmmgmt_dev_detach,
 * but accepts an extra parameter to determine whether to shutdown any
 * associated network interfaces and packet I/O DMA channels (see \ref
 * bcmmgmt_pkt_dev_cleanup_keep_netif). This option is typically used
 * to allow an O/S network driver (KNET) to keep functioning even if
 * the user mode part of the SDK is shut down.
 *
 * \param [in] unit Unit number.
 * \param [in] keep_netif Set to true to keep packet I/O running.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL The device failed to detach.
 */
extern int
bcmmgmt_dev_internal_detach(int unit, bool keep_netif);

/*!
 * \brief Execute shutdown callbacks.
 *
 * Execute all registered callback functions.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmmgmt_shutdown_cb_execute(int unit);

#endif /* BCMMGMT_INTERNAL_H */

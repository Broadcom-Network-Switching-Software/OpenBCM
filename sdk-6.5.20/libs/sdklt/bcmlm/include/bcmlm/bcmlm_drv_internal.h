/*! \file bcmlm_drv_internal.h
 *
 * Link Manager common driver definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLM_DRV_INTERNAL_H
#define BCMLM_DRV_INTERNAL_H

#include <bcmbd/bcmbd_intr.h>
#include <bcmlm/bcmlm_types.h>

/*!
 * \brief Initialize hardware linkscan.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors, otherwise failure in initializing hardware
 *         linkscan.
 */
typedef int (*ls_hw_init_f)(int unit);

/*!
 * \brief Clean up hardware linkscan.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors, otherwise failure in clean up hardware
 *         linkscan.
 */
typedef int (*ls_hw_cleanup_f)(int unit);

/*!
 * \brief Configure hardware linkscan.
 *
 * \param [in] unit Unit number.
 * \param [in] pbm Bitmap of ports to be enabled with hardware linkscan.
 *
 * \retval SHR_E_NONE No errors, otherwise failure in configuring hardware
 *         linkscan.
 */
typedef int (*ls_hw_config_f)(int unit, bcmdrd_pbmp_t pbm);

/*!
 * \brief Get link state from hardware linkscan.
 *
 * \param [in] unit Unit number.
 * \param [out] pbm Bitmap of ports in link up state.
 *
 * \retval SHR_E_NONE No errors, otherwise failure in getting link state.
 */
typedef int (*ls_hw_link_get_f)(int unit, bcmdrd_pbmp_t *pbm);

/*!
 *  \brief Clear hardware linkscan interrupt.
 *
 *  \param [in] unit Unit number.
 *
 *  \retval SHR_E_NONE No error.
 */
typedef int (*ls_hw_link_intr_clear_f)(int unit);

/*!
 * \brief Register callback for hardware linkscan interrupt.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_func Callback function.
 *
 * \retval SHR_E_NONE No errors, otherwise failure in registering callback.
 */
typedef int (*ls_hw_link_intr_func_set_f)(int unit, bcmbd_intr_f intr_func);

/*!
 * \brief Stop hardware linkscan.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT Failed to disable hardware linkscan.
 */
typedef int (*ls_hw_scan_stop_f)(int unit);

/*!
 * \brief Start hardware linkscan.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*ls_hw_scan_start_f)(int unit);

/*!
 * \brief Fault checking in linkscan is enabled or not.
 *
 * \param [in] unit Unit number.
 * \param [out] enabled Enabled or disabled.
 *
 * \retval SHR_E_NONE No errors, otherwise failure in getting device info.
 */
typedef int (*ls_sw_fault_check_enabled_f)(int unit, int *enabled);

/*!
 * \brief Check the port is valid and eligible for link scan.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \retval SHR_E_NONE No errors, otherwise port is invalid.
 */
typedef int (*ls_sw_port_validate_f)(int unit, shr_port_t port);

/*!
 * \brief Link Manager driver object
 *
 * The Link Manager driver object is used to initialize and access hardware
 * linkscan or firmware linkscan for a switch device, and access MAC/PHY via
 * Port Control for software linkscan or port update due to link state change.
 */
typedef struct bcmlm_drv_s {
    /*! Unit number */
    int unit;

    /*! Initialize hardware linkscan */
    ls_hw_init_f hw_init;

    /*! Clean up hardware linkscan */
    ls_hw_cleanup_f hw_cleanup;

    /*! Configure hardware linkscan */
    ls_hw_config_f hw_config;

    /*! Get link status from hardware linkscan */
    ls_hw_link_get_f hw_link_get;

    /*! Clear interrupt */
    ls_hw_link_intr_clear_f hw_intr_clear;

    /*! Register interrupt callback */
    ls_hw_link_intr_func_set_f hw_intr_cb_set;

    /*! Stop hardware linkscan */
    ls_hw_scan_stop_f hw_scan_stop;

    /*! Start hardware linkscan */
    ls_hw_scan_stop_f hw_scan_start;

    /*! Fault check is enabled or not */
    ls_sw_fault_check_enabled_f fault_check_enabled;

    /*! Check the port is valid and eligible for link scan */
    ls_sw_port_validate_f port_validate;
} bcmlm_drv_t;

/*!
 * \brief Initialize LM device driver.
 *
 * Install base driver functions and initialize device features.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmlm_drv_attach(int unit);

/*!
 * \brief Clean up LM driver.
 *
 * Release any resources allocated by \ref bcmlm_drv_attach.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmlm_drv_detach(int unit);

/*!
 * \brief Perform device specific initialization for linkscan.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmlm_dev_init(int unit);

/*!
 * \brief Perform device specific clean up for linkscan.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmlm_dev_cleanup(int unit);

/*!
 * \brief Hardware linkscan initialization.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT BCMLM driver is not initialized.
 * \retval SHR_E_UNAVAIL Hardware linkscan initialization is not supported.
 */
extern int
bcmlm_hw_linkscan_init(int unit);

/*!
 * \brief Configure hardware linkscan.
 *
 * \param [in] unit Unit number.
 * \param [in] hw Bitmap of ports to be enabled with hardware linkscan.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT BCMLM driver is not initialized.
 * \retval SHR_E_UNAVAIL Hardware linkscan config is not supported.
 */
extern int
bcmlm_hw_linkscan_config(int unit, bcmdrd_pbmp_t hw);

/*!
 * \brief Get link state from hardware linkscan.
 *
 * \param [in] unit Unit number.
 * \param [out] hw Bitmap of ports in link up state (phy_link + fault).
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT BCMLM driver is not initialized.
 * \retval SHR_E_UNAVAIL Get link from hardware linkscan is not supported.
 */
extern int
bcmlm_hw_linkscan_link_get(int unit, bcmdrd_pbmp_t *hw);

/*!
 *  \brief Clear hardware linkscan interrupt.
 *
 *  \param [in] unit Unit number.
 *
 *  \retval SHR_E_NONE No error.
 */
extern int
bcmlm_hw_linkscan_intr_clear(int unit);

/*!
 * \brief Register callback function for interrupt from hardware linkscan.
 *
 * \param [in] unit Unit number.
 * \param [in] cb Callback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT BCMLM driver is not initialized.
 * \retval SHR_E_UNAVAIL Register interrupt callback is not supported.
 */
extern int
bcmlm_hw_linkscan_intr_cb_set(int unit, bcmbd_intr_f cb);

/*!
 * \brief Fault checking in linkscan is enabled or not.
 *
 * \param [in] unit Unit number.
 * \param [out] enabled Enabled or disabled.
 *
 * \retval SHR_E_NONE No errors, otherwise failure in getting device info.
 */
extern int
bcmlm_linkscan_fault_check_enabled(int unit, int *enabled);

/*!
 * \brief Check if port is valid and eligible for link scan.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \retval SHR_E_NONE No errors. Otherwise port is ineligible for link scan.
 */
extern int
bcmlm_linkscan_port_validate(int unit, shr_port_t port);

/*!
 * \brief Get link state from MAC/PHY.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] link Link state (phy_link + fault).
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT BCMLM driver is not initialized.
 * \retval SHR_E_UNAVAIL Get link state from MAC/PHY is not supported.
 */
extern int
bcmlm_sw_linkscan_link_get(int unit, shr_port_t port, int *link);

/*!
 * \brief Get fault state from MAC.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] fault Fault state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT BCMLM driver is not initialized.
 * \retval SHR_E_UNAVAIL Get fault state from MAC is not supported.
 */
extern int
bcmlm_linkscan_fault_get(int unit, shr_port_t port, int *fault);

/*!
 * \brief Update port to make port characteristics in MAC and PHY match.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] link Link state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT BCMLM driver is not initialized.
 * \retval SHR_E_UNAVAIL Update port is not supported.
 */
extern int
bcmlm_port_update(int unit, shr_port_t port, int link);

#endif /* BCMLM_DRV_INTERNAL_H */

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_LINK_H
#define _SOC_LINK_H

#include <sal/types.h>

/* Callback function of hardware linkscan driver model. */
/*!
 * \brief Enable/disable fast link recovery per port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] enable 0: disable, !=0 enable.
 *
 * \retval SOC_E_NONE No errors.
 */
typedef int
(*linkscan_flr_enable_set_f)(int unit, int pport, int enable);

/*!
 * \brief Configure the timeout threshold per port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] ms Timeout threshold.
 *
 * \retval SOC_E_NONE No errors, otherwise failure in initializing hardware
 *         linkscan.
 */
typedef int
(*linkscan_flr_timeout_set_f) (int unit, int pport, uint32 ms);

/*!
 * \brief Get the FLR linkup count.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] ms Timeout threshold.
 *
 * \retval SHR_E_NONE No errors, otherwise failure in initializing hardware
 *         linkscan.
 */
typedef int
(*linkscan_flr_linkup_count_get_f) (int unit, int pport, uint32 *count);

/*!
 * \brief Enable FLR linkup callback.
 *
 * \param [in] unit Unit number.
 * \param [in] en Enable/disable.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int
(*linkscan_flr_linkup_cb_enable_set_f) (int unit, int en);

/*!
 * \brief Config loopback info to M0FW.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] enable 0: disable, !=0 enable.
 *
 * \retval SOC_E_NONE No errors.
 */
typedef int
(*linkscan_loopback_info_set_f)(int unit, int pport, int enable);


/*
 * Typedef: soc_led_driver_t
 * Purpose: Linkscan driver model.
 */

/* Hardware linkscan FLR driver. */
typedef struct {

    /* Enable FLR enable */
    linkscan_flr_enable_set_f        flr_enable_set;

    /* Set per port FLR timeout value.*/
    linkscan_flr_timeout_set_f       flr_timeout_set;

    /* Get per port FLR link up event counter. */
    linkscan_flr_linkup_count_get_f  flr_linkup_cnt_get;

    /* Enable FLR linkup callback function. */
    linkscan_flr_linkup_cb_enable_set_f flr_linkup_cb_en_set;

    /* Enable FLR linkup callback function. */
    linkscan_loopback_info_set_f loopback_info_set;

} soc_linkscan_drv_t;

/*
 *   SOC linkscan internal APIs.
 */
extern int
soc_linkscan_drv_set(int unit, soc_linkscan_drv_t *drv);

extern int
soc_linkscan_flr_support_set(int unit, soc_pbmp_t support_pbm);

extern int
soc_linkscan_loopback_info_set(int unit, int port, int enable);


extern int
soc_linkscan_driver_init(int unit);

extern int
soc_linkscan_flr_wb_restore(int unit);

/*
 * Driver internal definition.
 */
#define SOC_LINKSCAN_DRIVER(unit) (SOC_CONTROL(unit)->soc_linkscan_driver)

#define LINKSCAN_FLR_SUPPORT_PPBM(unit) \
        (SOC_CONTROL(unit)->hw_link_flr_support_ppbmp)

#define LINKSCAN_FLR_SUPPORT(unit, pport) \
        (SOC_PBMP_MEMBER(LINKSCAN_FLR_SUPPORT_PPBM(unit), pport))

#define LINKSCAN_DRIVER_CALL(_f, _a)  \
        ((SOC_LINKSCAN_DRIVER(unit) == NULL) ? SOC_E_INIT : \
        ((SOC_LINKSCAN_DRIVER(unit)->_f == NULL) ? SOC_E_UNAVAIL : \
        (SOC_LINKSCAN_DRIVER(unit)->_f _a)))

#define SOC_LINKSCAN_FLR_TIMEOUT_SET(_u, _p, _timeout) \
        LINKSCAN_DRIVER_CALL(flr_timeout_set, \
                             ((_u), (_p), (_timeout)))

#define SOC_LINKSCAN_FLR_ENABLE_SET(_u, _p, _enabled) \
        LINKSCAN_DRIVER_CALL(flr_enable_set, \
                             ((_u), (_p), (_enabled)))

#define SOC_LINKSCAN_FLR_LINKUP_CNT_GET(_u, _p, _cnt) \
        LINKSCAN_DRIVER_CALL(flr_linkup_cnt_get, \
                             ((_u), (_p), (_cnt)))

#define SOC_LINKSCAN_FLR_LINKUP_CALLBACK_EN_SET(_u, _en) \
        LINKSCAN_DRIVER_CALL(flr_linkup_cb_en_set, \
                             ((_u), (_en)))

#define SOC_LINKSCAN_LOOPBACK_INFO_SET(_u, _p, _enabled) \
        LINKSCAN_DRIVER_CALL(loopback_info_set, \
                             ((_u), (_p), (_enabled)))


/* To check if linkscan driver is supported. */
#define SOC_LINKSCAN_DRIVER_SUPPORT(unit) \
        (SOC_LINKSCAN_DRIVER(unit) != NULL)

#endif  /* _SOC_LINK_H */

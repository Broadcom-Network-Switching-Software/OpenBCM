/*! \file bcmbd_cmicx_led.h
 *
 * CMICx specific LED APIs and base driver.
 *
 * The base driver implement common steps of all CMICx-LED drivers.
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

#ifndef BCMBD_CMICX_LED_H
#define BCMBD_CMICX_LED_H

/*!
 * \brief Setting LED refresh rate.
 *
 * \param [in] unit Unit number.
 * \param [in] src_clk_freq Source clock frequency(Hz).
 * \param [in] refresh_freq LED refresh rate(Hz).
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicx_led_refresh_freq_set(int unit, uint32_t src_clk_freq,
                                  uint32_t refresh_freq);

/*!
 * \brief Setting LED output clock rate.
 *
 * \param [in] unit Unit number.
 * \param [in] src_clk_freq Source clock frequency(Hz).
 * \param [in] led_clk_freq LED output clock rate(Hz).
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicx_led_clk_freq_set(int unit, uint32_t src_clk_freq,
                              uint32_t led_clk_freq);

/*!
 * \brief Setting "last_port" of LED port chain for chip internal.
 *
 * \param [in] unit Unit number.
 * \param [in] last_port last_port setting for chip internal chain.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicx_led_last_port_set(int unit, uint32_t last_port);

#endif /* BCMBD_CMICX_LED_H */

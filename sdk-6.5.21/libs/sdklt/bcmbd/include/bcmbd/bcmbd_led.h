/*! \file bcmbd_led.h
 *
 * LED API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_LED_H
#define BCMBD_LED_H

#include <sal/sal_types.h>

/*******************************************************************************
 * BCMBD_LED public definition
 */

/*!
 * \brief Invalid LED controller number definition.
 */
#define BCMBD_LED_UC_INVALID       -1

/*******************************************************************************
 * BCMBD_LED public APIs
 */

/*!
 * \brief Device-specific LED initialization.
 *
 * This function is used to initialize the chip-specific LED
 * configuration, which may include both software and hardware
 * operations.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LED driver initialization failed.
 */
extern int
bcmbd_led_init(int unit);

/*!
 * \brief Device-specific LED driver cleanup.
 *
 * This function is used to clean up the LED driver resources
 * allocated by \ref led_init_f.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to clean up.
 */
extern int
bcmbd_led_cleanup(int unit);

/*!
 * \brief Get number of LED controllers for a device unit.
 *
 * \param [in] unit Unit number.
 * \return Number of LED controllers for a device unit.
 */
extern int
bcmbd_led_uc_num_get(int unit);

/*!
 *\brief Start a specified LED controller.
 *
 * \param [in]  unit Unit number.
 * \param [in]  led_uc LED controller number.
 *
 * \retval SHR_E_NONE Start a LED controller successfully.
 * \retval SHR_E_FAIL Fail to start a LED controller.
 */
extern int
bcmbd_led_uc_start(int unit, int led_uc);


/*!
 *\brief Stop a specified LED controller.
 *
 * \param [in]  unit Unit number.
 * \param [in]  led_uc LED controller number.
 *
 * \retval SHR_E_NONE Stop a LED controller successfully.
 * \retval SHR_E_FAIL Fail to stop a LED controller.
 */
extern int
bcmbd_led_uc_stop(int unit, int led_uc);

/*!
 *\brief Get LED controllers start or not.
 *
 * \param [in]  unit Unit number.
 * \param [in]  led_uc LED controller number.
 * \param [out] start Indicate LED controller start or not.
 *
 * \return SHR_E_NONE if successful.
 */
int
bcmbd_led_uc_start_get(int unit,
                       int led_uc,
                       int *start);

/*!
 * \brief Write data into "swdata" space.
 *
 *  "swdata" space contains
 *      1. Data or information from LED application on the host processor.
 *      2. Internal variable of LED firmware.
 *
 *  For CMICd, default location of "swdata" space could vary from chip
 *  to chip. Please refer to device-specific documentation for details.
 *
 * \param [in] unit Unit number.
 * \param [in] led_uc LED controller number.
 * \param [in] offset Offset in byte.
 * \param [in] value Write value.
 *
 * \retval SHR_E_NONE Write swdata successfully.
 * \retval SHR_E_FAIL Fail to write data into swdata space.
 */
int
bcmbd_led_uc_swdata_write(int unit,
                          int led_uc,
                          int offset,
                          uint8_t value);


/*!
 * \brief Read data from "swdata" space.
 *        and LED host application.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] led_uc LED controller number.
 * \param [in] offset Offset in byte.
 * \param [out] value Read result.
 *
 * \retval SHR_E_NONE Read data successfully.
 * \retval SHR_E_FAIL Fail to read data from swdata space.
 */
int
bcmbd_led_uc_swdata_read(int unit,
                         int led_uc,
                         int offset,
                         uint8_t *value);

/*!
 * \brief Get LED controller number and port number within a LED controller
 *        for physical port.
 *
 * For CMICd, all physical ports are controlled by multiple LED controllers.
 * For CMICx, all physical ports are controlled by one single LED controller.
 * Each LED controller will take care part of physical port.
 * Each physical port hardware status will be placed in LED controller RAM
 * with a offset(port).
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number
 * \param [out] led_uc LED controller number.
 * \param [out] led_uc_port Port number within a LED controller.

 * \retval SHR_E_NONE Get LED controller and port successfully.
 * \retval SHR_E_FAIL Fail to get LED controller and port.
 */
int
bcmbd_port_to_led_uc_port(int unit, int port,
                          int *led_uc,
                          int *led_uc_port);

#endif /* BCMBD_LED_H */

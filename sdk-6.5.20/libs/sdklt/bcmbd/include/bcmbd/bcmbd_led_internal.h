/*! \file bcmbd_led_internal.h
 *
 * Internal LED driver definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_LED_INTERNAL_H
#define BCMBD_LED_INTERNAL_H

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd_led.h>
#include <bcmbd/bcmbd_fwm.h>

/*******************************************************************************
 * LED internal data structure
 */
/*!
 * \brief Size of CMIC LED uC data ram.
 */
#define CMIC_LED_UC_DATA_RAM_SIZE  256

/*!
 * \brief Type defininition of internal control structure for BCMBD-LED module.
 */
typedef struct bcmbd_led_dev_s {

    /*! A map from phyiscal port to LED controller number */
    int pport_to_led_uc[BCMDRD_CONFIG_MAX_PORTS];

    /*! A map from phyiscal port to LED controller number */
    int pport_to_led_uc_port[BCMDRD_CONFIG_MAX_PORTS];

    /*! Starting address of "swdata" space */
    int swdata_start[BCMDRD_CONFIG_MAX_PORTS];

} bcmbd_led_dev_t;

/*******************************************************************************
 * LED driver function prototypes
 */
/*!
 * \brief Get maximun number of LED controller.
 *
 * \param [in] unit Unit number.
 * \return maximun LED controller number of a device unit.
 */
typedef int
(*led_uc_num_get_f)(int unit);


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
typedef int
(*led_init_f)(int unit);

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
typedef int
(*led_cleanup_f)(int unit);

/*!
 * \brief Load LED firmware into a LED controller.
 *
 * This function loads formware into the run-time memory of a given
 * LED controller. The function assumes that the LED controller is not
 * running.
 *
 * \param [in] unit Unit number.
 * \param [in] led_uc LED controller number.
 * \param [in] buf LED firmware buffer.
 * \param [in] len Length of firmware buffer.
 *
 * \retval SHR_E_NONE Firmware successfully loaded.
 * \retval SHR_E_FAIL Failed to load firmware.
 */
typedef int
(*led_fw_load_f)(int unit, int led_uc, const uint8_t *buf, int len);

/*!
 *\brief Start/Stop a LED controller.
 *
 * \param [in]  unit Unit number.
 * \param [in]  led_uc LED controller number.
 * \param [in]  start !0: start a controller.
 *                    0: stop a controller.
 *
 * \retval SHR_E_NONE Start/stop a LED controller successfully.
 * \retval SHR_E_FAIL Fail to start/stop a LED controller.
 */
typedef int
(*led_uc_start_set_f)(int unit, int led_uc, int start);


/*!
 *\brief Stop a specified LED controller.
 *
 * \param [in]   unit Unit number.
 * \param [in]   led_uc LED controller number.
 * \param [out]  start !0: start a controller.
 *                      0: stop a controller.
 *
 * \retval SHR_E_NONE Stop a LED controller successfully.
 * \retval SHR_E_FAIL Fail to stop a LED controller.
 */
typedef int
(*led_uc_start_get_f)(int unit, int led_uc, int *start);

/*!
 * \brief Write data into "swdata" space.
 *
 * "swdata" space contains:
 *     1. Data or information from LED application on the host processor.
 *     2. Internal variable of LED firmware.
 *
 * For CMICd, default location of "swdata" space could vary from chip
 * to chip. Please refer to device-specific documentation for details.
 *
 * \param [in] unit Unit number.
 * \param [in] led_uc LED controller number.
 * \param [in] offset Offset in byte.
 * \param [in] value Write value.
 *
 * \retval SHR_E_NONE Write swdata successfully.
 * \retval SHR_E_FAIL Fail to write data into swdata space.
 */
typedef int
(*led_uc_swdata_write_f)(int unit, int led_uc,
                         int offset, uint8_t value);


/*!
 * \brief Read data from "swdata" space.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] led_uc LED controller number.
 * \param [in] offset Offset in byte.
 * \param [out] value Read value.
 *
 * \retval SHR_E_NONE Write swdata successfully.
 * \retval SHR_E_FAIL Fail to write data into swdata space.
 */

typedef int
(*led_uc_swdata_read_f) (int unit, int led_uc,
                         int offset, uint8_t *value);

/*!
 * \brief Given a physical port number, get correponding LED controller
 *        number and LED controller port.
 *
 * For CMICd, all physical ports could be controlled by multiple LED controllers.
 * For CMICx, all physical ports are controlled by one single LED controller.
 *
 * This function is to get LED controller number and relative port within a
 * LED controller for a physical port.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical number
 * \param [out] led_uc LED controller number.
 * \param [out] port Port number within a LED controller.

 * \retval SHR_E_NONE Get LED controller and port successfully.
 * \retval SHR_E_FAIL Fail to get LED controller and port.
 */
typedef int
(*led_pport_to_led_uc_port_f) (int unit, int pport,
                               int *led_uc, int *led_uc_port);

/*! LED driver object. */
typedef struct bcmbd_led_drv_s {

    /*! Initialize LED driver. */
    led_init_f init;

    /*! Clean up LED driver. */
    led_cleanup_f cleanup;

    /*! LED firmware loader. */
    led_fw_load_f fw_load;

    /*! Get LED uC number */
    led_uc_num_get_f uc_num_get;

    /*! Stop/start LED uC. */
    led_uc_start_set_f uc_start_set;

    /*! Get LED uC is started or not. */
    led_uc_start_get_f uc_start_get;

    /*! Read "swdata" space of LED uC */
    led_uc_swdata_read_f uc_swdata_read;

    /*! Write "swdata" space of LED uC */
    led_uc_swdata_write_f uc_swdata_write;

    /*! Physical port to LED uc number and port */
    led_pport_to_led_uc_port_f pport_to_led_uc_port;

} bcmbd_led_drv_t;

/*!
 * \brief Install device-specific LED driver.
 *
 * Install device-specific LED driver into top-level LED API.
 *
 * Use \c led_drv = NULL to uninstall a driver.
 *
 * \param [in] unit Unit number.
 * \param [in] led_drv LED driver object.
 *
 * \retval 0 No errors
 */
extern int
bcmbd_led_drv_init(int unit, const bcmbd_led_drv_t *led_drv);

/*!
 * \brief Get device-specific data of LED driver.
 *
 * \param [in] unit Unit number.
 * \param [out] led_dev Device data of LED driver.
 *
 * \retval 0 No errors
 */
extern int
bcmbd_led_dev_get(int unit, bcmbd_led_dev_t **led_dev);

/*!
 * \brief Get CMICx-LED host base driver.
 *
 * CMICx-LED base driver provides basic methods to control CMICx-LED FW.
 *
 * \param [in] unit Unit number.
 *
 * \retval Pointer to the base driver.
 */
extern const bcmbd_led_drv_t *
bcmbd_cmicx_led_base_drv_get(int unit);

/*!
 * \brief Get CMICx-LED FWM processor base driver.
 *
 * CMICx-LED FWM procesor base driver provides basic methods to control
 * CMICx-LED FWM processor via the FWM APIs.
 *
 * \param [in] unit Unit number.
 *
 * \retval Pointer to the base driver.
 */
extern const bcmbd_fwm_proc_t *
bcmbd_cmicx_led_fwm_proc_base_drv_get(int unit);

/*!
 * \brief Load LED firmware (binary format) into a LED controller.
 *
 * This function loads firmware into the run-time memory of a given
 * LED controller. The function assumes that the LED controller is not
 * running.
 *
 * \param [in] unit Unit number.
 * \param [in] led_uc LED controller number.
 * \param [in] buf LED firmware buffer.
 * \param [in] len Length of firmware buffer.
 *
 * \retval SHR_E_NONE Firmware successfully loaded.
 * \retval SHR_E_FAIL Failed to load firmware.
 */
extern int
bcmbd_led_uc_fw_load(int unit, int led_uc, const uint8_t *buf, int len);

#endif /* BCMBD_LED_INTERNAL_H */

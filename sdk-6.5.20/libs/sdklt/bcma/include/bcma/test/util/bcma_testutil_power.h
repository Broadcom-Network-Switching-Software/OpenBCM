/*! \file bcma_testutil_power.h
 *
 * Power operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_POWER_H
#define BCMA_TESTUTIL_POWER_H

#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcma/test/util/bcma_testutil_packet.h>


/*!
 * \brief Add power chip specific help.
 *
 * \param [in] unit                 Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_power_help(int unit);


/*!
 * \brief Add power init.
 *
 * \param [in] unit                 Unit number.
 * \param [in] bp Param pointer.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_power_init(int unit, void *bp);

/*!
 * \brief Add port config.
 *
 * \param [in] unit                 Unit number.
 * \param [in] bp Param pointer.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_power_port_set(int unit, void *bp);


/*!
 * \brief Add set switch config for power scenario.
 *
 * \param [in] unit                 Unit number.
 * \param [in] bp Param pointer.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_power_config_set(int unit, void *bp);


/*!
 * \brief Add power run.
 *
 * \param [in] unit                 Unit number.
 * \param [in] bp Param pointer.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_power_txrx(int unit, void *bp);


/*!
 * \brief Power test help print API.
 *
 * \param [in] unit                 Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int
(*bcma_testutil_power_help_f)(int unit);


/*!
 * \brief Power init test.
 *
 * \param [in] unit                 Unit number.
 * \param [in] bp Param pointer.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int
(*bcma_testutil_power_init_f)(int unit, void *bp);


/*!
 * \brief Power port config API .
 *
 * \param [in] unit                 Unit number.
 * \param [in] bp Param pointer.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int
(*bcma_testutil_power_port_set_f)(int unit, void *bp);


/*!
 * \brief Power config API.
 *
 * \param [in] unit                 Unit number.
 * \param [in] bp Param pointer.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int
(*bcma_testutil_power_config_set_f)(int unit, void *bp);


/*!
 * \brief Power run test.
 *
 * \param [in] unit                 Unit number.
 * \param [in] bp Param pointer.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int
(*bcma_testutil_power_txrx_f)(int unit, void *bp);


/*! L3 operation functions. */
typedef struct bcma_testutil_power_op_s {
    /*! Power test help. */
    bcma_testutil_power_help_f power_help;

    /*! Power scenario test init. */
    bcma_testutil_power_init_f power_init;

    /*! Power scenario port config. */
    bcma_testutil_power_port_set_f power_port_set;

    /*! Generic switch config API. */
    bcma_testutil_power_config_set_f power_config_set;

    /*! Power scenario run. */
    bcma_testutil_power_txrx_f power_txrx;

} bcma_testutil_drv_power_op_t;


#endif /* BCMA_TESTUTIL_POWER_H */

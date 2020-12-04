/*! \file bcmdrd_port.h
 *
 * DRD port information management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMDRD_PORT_H
#define BCMDRD_PORT_H

#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Port information.
 *
 * This structure contains port type defintitions and fixed port
 * mappings for a device.
 */
typedef struct bcmdrd_port_info_s {
    int port; /*!< Physical port number. */
    bcmdrd_port_type_t ptype; /*!< Port category (or categories). */
    int lport; /*!< Logical port number. */
    int mport; /*!< MMU port number. */
} bcmdrd_port_info_t;

/*!
 * \brief Initialize port information array for a device
 *
 * The port information array must be terminated with an entry where
 * the physical port number is -1.
 *
 * \param [in] unit Unit number.
 * \param [in] pi Port information to use for this device.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmdrd_port_info_init(int unit, const bcmdrd_port_info_t *pi);

/*!
 * \brief Get port type for a physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 *
 * \return Port type.
 */
extern uint32_t
bcmdrd_port_type_get(int unit, int port);

/*!
 * \brief Get port type for a logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \return Port type.
 */
extern uint32_t
bcmdrd_lport_type_get(int unit, int lport);

/*!
 * \brief Get bitmap of ports of a given port type (or types).
 *
 * The \c ptype parameter may contain multiple port types OR'ed
 * together (\ref BCMDRD_PORT_TYPE_xxx).
 *
 * If a NULL pointer is passed in, the function will still return the
 * number of ports of the specified port types.
 *
 * \param [in] unit Unit nunmber.
 * \param [in] ptype Port type(s).
 * \param [out] pbmp Bitmap of port of the specified type(s).
 *
 * \return Number of ports of the specified type(s).
 */
extern int
bcmdrd_port_type_pbmp_get(int unit, bcmdrd_port_type_t ptype,
                          bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get logical port mapping for a physical port.
 *
 * The function will return -1 if the physical port is invalid or the
 * mapping is not fixed.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 *
 * \return Logical port number or -1 on error.
 */
extern int
bcmdrd_port_p2l(int unit, int port);

/*!
 * \brief Get physical port mapping for a logical port.
 *
 * The function will return -1 if the logical port is invalid or the
 * mapping is not fixed.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \return Physical port number or -1 on error.
 */
extern int
bcmdrd_port_l2p(int unit, int lport);


/*!
 * \brief Get MMU port mapping for a physical port.
 *
 * The function will return -1 if the physical port is invalid or the
 * mapping is not fixed.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 *
 * \return MMU port number or -1 on error.
 */
extern int
bcmdrd_port_p2m(int unit, int port);

/*!
 * \brief Get physical port mapping for an MMU port.
 *
 * The function will return -1 if the MMU port is invalid or the
 * mapping is not fixed.
 *
 * \param [in] unit Unit number.
 * \param [in] mport MMU port number.
 *
 * \return Physical port number or -1 on error.
 */
extern int
bcmdrd_port_m2p(int unit, int mport);

#endif /* BCMDRD_PORT_H */

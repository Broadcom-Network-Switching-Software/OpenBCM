/*! \file bcma_testutil_port.h
 *
 * Port operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_PORT_H
#define BCMA_TESTUTIL_PORT_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmpc/bcmpc_lport.h>

/*! Loopback type */
typedef enum testutil_traffic_lb_type_e {
    LB_TYPE_CPU = 0,
    LB_TYPE_MAC,
    LB_TYPE_PHY,
    LB_TYPE_EXT,
    LB_TYPE_AUTO,
    LB_TYPE_NONE
} testutil_port_lb_type_t;

/*!
 * \brief Traffic test port configuration procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] lb_type Loopback type
 * \param [in] pbmp Test port list.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_port_setup(int unit, testutil_port_lb_type_t lb_type,
                         bcmdrd_pbmp_t pbmp);

/*!
 * \brief Traffic test port claer procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] lb_type Loopback type
 * \param [in] pbmp Test port list.
 * \param [in] link_check Check link status.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_port_clear(int unit, testutil_port_lb_type_t lb_type,
                         bcmdrd_pbmp_t pbmp, int link_check);

/*!
 * \brief Get port link status.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 * \param [out] link The port link status.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_port_link_status_get(int unit, int port, uint64_t *link);

/*!
 * \brief Set mac control.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 * \param [in] rx_enable Whether to enable mac receive.
 * \param [in] tx_enable Whether to enable mac transmit.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_port_mac_control_set(int unit, int port, bool rx_enable, bool tx_enable);

/*!
 * \brief Set port mac oversize packet.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 * \param [in] oversize_pkt The length of mac oversize packet.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_port_oversize_pkt_set(int unit, int port, int oversize_pkt);

/*!
 * \brief Get port mac oversize packet.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 * \param [out] oversize_pkt The length of mac oversize packet.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_port_oversize_pkt_get(int unit, int port, int *oversize_pkt);

/*!
 * \brief Enable port timestamp action.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port.
 * \param [in] igr_ts Whether to add timestamp at ingress.
 * \param [in] egr_ts Whether to add timestamp at egress.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_port_timestamp_enable(int unit, int port, bool igr_ts, bool egr_ts);

/*!
 * \brief Disable port timestamp action.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_port_timestamp_disable(int unit, int port);


/*!
 * \brief Get port speed.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 * \param [out] speed The port speed.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_port_speed_get(int unit, int port, uint64_t *speed);

/*!
 * \brief The callback function to setup cpu port.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Input arguments.
 * \param [in] option Input options.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_port_cpu_setup_cb(int unit, void *bp, uint32_t option);
#endif /* BCMA_TESTUTIL_PORT_H */

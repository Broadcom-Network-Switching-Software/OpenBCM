/*! \file bcmpc_lport.h
 *
 * BCMPC Logical PORT (LPORT) APIs.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Port Control (BCMPC) module.
 *
 * The logical port library provides the functions to create switch logical
 * ports and associate the logical port number to a physical device port number.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_LPORT_H
#define BCMPC_LPORT_H

#include <bcmpc/bcmpc_types_internal.h>

/*! The max number of autoneg profiles that a physical port supports. */
#define  NUM_AUTONEG_PROFILES_MAX   (64)
/*!
 * \brief Globally available port operating modes.
 *
 * Any given logical port will only support a subset of these modes.
 *
 * 1000/100/10 Mbps copper ports will use MII if the internal PHY is
 * not a real copper PHY.
 *
 * Note that this enum should be same as PC_PORT_OPMODE_T.
 */
typedef enum bcmpc_port_opmode_e {

    /*! Ensure that zero is not interpreted as a valid mode. */
    BCMPC_PORT_OPMODE_INVALID = 0,

    /*! Special mode for selecting default lane configuration. */
    BCMPC_PORT_OPMODE_ANY,

    /*! Auto-negotiation mode. */
    BCMPC_PORT_OPMODE_AUTONEG,

    /*! Custom port mode, where parameters are read from a profile. */
    BCMPC_PORT_OPMODE_CUSTOM,

    /*! 10 Mbps copper mode (10BASE-T). */
    BCMPC_PORT_OPMODE_10M_T,

    /*! 10 Mbps copper half-duplex mode (10BASE-T HDX). */
    BCMPC_PORT_OPMODE_10M_T_HDX,

    /*! 100 Mbps copper mode (100BASE-T). */
    BCMPC_PORT_OPMODE_100M_T,

    /*! 100 Mbps copper half-duplex mode (100BASE-T HDX). */
    BCMPC_PORT_OPMODE_100M_T_HDX,

    /*! 100 Mbps fiber mode (100BASE-FX). */
    BCMPC_PORT_OPMODE_100M_FX,

    /*! 1000 Mbps copper mode (1000BASE-T). */
    BCMPC_PORT_OPMODE_1000M_T,

    /*! 1000 Mbps fiber mode (1000BASE-X). */
    BCMPC_PORT_OPMODE_1000M_X,

    /*! 2.5 Gbps mode (HiGig only). */
    BCMPC_PORT_OPMODE_2P5G,

    /*! Default 10 Gbps mode for this port type (typically XAUI). */
    BCMPC_PORT_OPMODE_10G,

    /*! 10 Gbps SFI mode. */
    BCMPC_PORT_OPMODE_10G_SFI,

    /*! 10 Gbps XFI mode. */
    BCMPC_PORT_OPMODE_10G_XFI,

    /*! 10 Gbps CX mode. */
    BCMPC_PORT_OPMODE_10G_CX,

    /*! 10 Gbps CR mode. */
    BCMPC_PORT_OPMODE_10G_CR,

    /*! 10 Gbps KR mode. */
    BCMPC_PORT_OPMODE_10G_KR,

    /*! 10 Gbps KX mode. */
    BCMPC_PORT_OPMODE_10G_KX,

    /*! 12 Gbps mode (HiGig only). */
    BCMPC_PORT_OPMODE_12G,

    /*! 13 Gbps mode (HiGig only). */
    BCMPC_PORT_OPMODE_13G,

    /*! Default 20 Gbps mode for this port type (typically RXAUI). */
    BCMPC_PORT_OPMODE_20G,

    /*! 20 Gbps CX mode (20GBASE-CX2). */
    BCMPC_PORT_OPMODE_20G_CX,

    /*! 20 Gbps CR mode (20GBASE-CR2). */
    BCMPC_PORT_OPMODE_20G_CR,

    /*! 20 Gbps KR mode (20GBASE-KR2). */
    BCMPC_PORT_OPMODE_20G_KR,

    /*! 21 Gbps mode (HiGig only). */
    BCMPC_PORT_OPMODE_21G,

    /*! 25 Gbps XFI mode. */
    BCMPC_PORT_OPMODE_25G_XFI,

    /*! 27 Gbps XFI mode (HiGig only). */
    BCMPC_PORT_OPMODE_27G_XFI,

    /*! Default 40 Gbps mode for this port type (typically (XLAUI). */
    BCMPC_PORT_OPMODE_40G,

    /*! 40 Gbps CR mode (40GBASE-CR4). */
    BCMPC_PORT_OPMODE_40G_CR,

    /*! 40 Gbps KR mode (40GBASE-KR4). */
    BCMPC_PORT_OPMODE_40G_KR,

    /*! 42 Gbps mode (HiGig only). */
    BCMPC_PORT_OPMODE_42G,

    /*! 50 Gbps KR mode. */
    BCMPC_PORT_OPMODE_50G_KR,

    /*! 53 Gbps KR mode (HiGig only). */
    BCMPC_PORT_OPMODE_53G_KR,

    /*! Default 100 Gbps mode for this port type (typically CAUI). */
    BCMPC_PORT_OPMODE_100G,

    /*! 100 Gbps CR mode. */
    BCMPC_PORT_OPMODE_100G_CR,

    /*! 106 Gbps KR mode (HiGig only). */
    BCMPC_PORT_OPMODE_106G_KR,

    /*! 120 Gbps mode (HiGig only). */
    BCMPC_PORT_OPMODE_120G,

    /*! 127 Gbps mode (HiGig only). */
    BCMPC_PORT_OPMODE_127G,

    /*! 200 Gbps mode. */
    BCMPC_PORT_OPMODE_200G,

    /*! 400 Gbps mode. */
    BCMPC_PORT_OPMODE_400G,

    /*! Must be the last. */
    BCMPC_PORT_OPMODE_COUNT

} bcmpc_port_opmode_t;


/*!
 * \brief Get the physical port number for a logical port.
 *
 * The function will return the physical port number for the given logical
 * port \c lport when the logical port has been inserted with valid settings
 * according to the current PM mode configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical device port.
 *
 * \return The physical port number, or BCMPC_INVALID_PPORT when errors.
 */
extern bcmpc_pport_t
bcmpc_lport_to_pport(int unit, bcmpc_lport_t lport);

/*!
 * \brief Get the logical port number from the given physical port.
 *
 * The function will return the logical port number for the given physical
 * port \c pport when the logical port has been inserted with the valid
 * settings accroding to the current PM mode configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 *
 * \return The physical port number, or BCMPC_INVALID_LPORT when errors.
 */
extern bcmpc_lport_t
bcmpc_pport_to_lport(int unit, bcmpc_pport_t pport);

/*!
 * \brief Bring port out of LAG failover loopback.
 *
 * Make the port back to normal operation from failover loopback state.
 *
 * \param [in] unit Unit number
 * \param [in] lport Logical device port.
 *
 * \retval SHR_E_NONE No errors
 */
extern int
bcmpc_port_failover_loopback_remove(int unit, bcmpc_lport_t lport);

/*!
 * \brief Get the speed value from the given port opmode.
 *
 * \param [in] unit Unit number.
 * \param [in] opmode Port operating mode.
 * \param [speed] speed Speed value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get the speed value for \c opmode.
 */
extern int
bcmpc_port_opmode_to_speed(int unit, bcmpc_port_opmode_t opmode,
                           uint32_t *speed);

#endif /* BCMPC_LPORT_H */

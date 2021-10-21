/** \file trap_tune.c
 * Tune trap module file
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files. {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm/rx.h>
#include "trap_tune.h"
#ifdef BCM_DNX2_SUPPORT
#include <src/bcm/dnx/rx/rx_trap_v1.h>
#endif
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_verify.h>

/*
 * }  Include files.
 */

/**
 * \brief -Configuration function for ingress traps.
 * The function receives the ingress trap type as parameter and configures default trap strength.
 * \param [in] unit - Unit ID
 * \param [in] flags - Specify input flags
 * \param [in] trap_type - Trap type
 * \param [in] trap_strength - Trap strength
 * \param [in] dest_port - Destination port
 * \param [in] snoop_strength - configure snoop strength
 * \param [in] snoop_command - configure snoop command
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * \see
 *  None.
 */

static shr_error_e
dnx_tune_trap_ingress_configure_init(
    int unit,
    int flags,
    bcm_rx_trap_t trap_type,
    int trap_strength,
    int dest_port,
    int snoop_strength,
    int snoop_command)
{
    bcm_rx_trap_config_t config;
    int trap_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, trap_type, &trap_id));
    bcm_rx_trap_config_t_init(&config);
    config.trap_strength = trap_strength;
    config.dest_port = dest_port;
    config.flags |= flags;
    config.snoop_strength = snoop_strength;
    config.snoop_cmnd = snoop_command;
    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   The function configures default ingress traps with default fwd strength and action
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * \see
 *  None.
 */

static shr_error_e
dnx_tune_trap_ingress_filters_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Initialize Ingress Same interface trap */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_UPDATE_DEST, bcmRxTrapSameInterface,
                     dnx_data_trap.strength.default_trap_strength_get(unit), BCM_GPORT_BLACK_HOLE, 0, 0));
    /** Initialize Ingress Default Drop trap */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_UPDATE_DEST, bcmRxTrapDfltDroppedPacket, 0, BCM_GPORT_BLACK_HOLE, 0, 0));
    /** Initialize trap for Explicit MC RPF Fail*/
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_UPDATE_DEST, bcmRxTrapMcExplicitRpfFail,
                     dnx_data_trap.strength.default_trap_strength_get(unit), BCM_GPORT_BLACK_HOLE, 0, 0));
    /** Initialize trap for MC RPF Fail */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_UPDATE_DEST, bcmRxTrapMcUseSipRpfFail,
                     dnx_data_trap.strength.default_trap_strength_get(unit), BCM_GPORT_BLACK_HOLE, 0, 0));
    /** Initialize trap for Loose UC RPF Fail */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_UPDATE_DEST, bcmRxTrapUcLooseRpfFail,
                     dnx_data_trap.strength.default_trap_strength_get(unit), BCM_GPORT_BLACK_HOLE, 0, 0));
    /** Initialize trap for Strict UC RPF Fail */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_UPDATE_DEST, bcmRxTrapUcStrictRpfFail,
                     dnx_data_trap.strength.default_trap_strength_get(unit), BCM_GPORT_BLACK_HOLE, 0, 0));
    /** Initialize trap for STP block */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_LEARN_DISABLE, bcmRxTrapStpStateBlock,
                     dnx_data_trap.strength.default_trap_strength_get(unit), BCM_GPORT_BLACK_HOLE, 0, 0));
    /** Initialize trap for STP learn, Use the same properties as block except for disabling learning */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_UPDATE_DEST, bcmRxTrapStpStateLearn,
                     dnx_data_trap.strength.default_trap_strength_get(unit), BCM_GPORT_BLACK_HOLE, 0, 0));
    /** In case of bcmRxTrapL2Learn0, do nothing except for creating the trap type */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init(unit, 0, bcmRxTrapL2Learn0, 0, 0, 0, 0));
    /** In case of bcmRxTrapL2Learn1, forward only, disable learning */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_LEARN_DISABLE, bcmRxTrapL2Learn1, 0, 0, 0, 0));
    /** In case of bcmRxTrapL2Learn2, learning SA and then drop */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_UPDATE_DEST, bcmRxTrapL2Learn2,
                     dnx_data_trap.strength.default_trap_strength_get(unit), BCM_GPORT_BLACK_HOLE, 0, 0));
    /** In case of bcmRxTrapL2Learn3, drop only */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_LEARN_DISABLE | BCM_RX_TRAP_UPDATE_DEST, bcmRxTrapL2Learn3,
                     dnx_data_trap.strength.default_trap_strength_get(unit), BCM_GPORT_BLACK_HOLE, 0, 0));
    /** Initialize bcmRxTrapLinkLayerVlanTagDiscard with default strength and drop action */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_UPDATE_DEST, bcmRxTrapLinkLayerVlanTagDiscard,
                     dnx_data_trap.strength.default_trap_strength_get(unit), BCM_GPORT_BLACK_HOLE, 0, 0));
    /** Initialize bcmRxTrapTerminatedVlanTagDiscard with default strength and drop action */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_UPDATE_DEST, bcmRxTrapTerminatedVlanTagDiscard,
                     dnx_data_trap.strength.default_trap_strength_get(unit), BCM_GPORT_BLACK_HOLE, 0, 0));

    /** Initialize trap for 1 plus 1 protected LIF when Failover path is invalid */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_UPDATE_DEST, bcmRxTrapFailover1Plus1Fail,
                     dnx_data_trap.strength.default_trap_strength_get(unit), BCM_GPORT_BLACK_HOLE, 0, 0));
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_UPDATE_DEST, bcmRxTrapMyMacAndIpDisabled,
                     dnx_data_trap.strength.default_trap_strength_get(unit), BCM_GPORT_BLACK_HOLE, 0, 0));
    /** Initialize trap for MPLS unknown label Fail */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_configure_init
                    (unit, BCM_RX_TRAP_UPDATE_DEST, bcmRxTrapMplsUnknownLabel, 0, BCM_GPORT_BLACK_HOLE, 0, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   For each of the EGRESS Filters initialize trap model.
 *   The filters are enabled on init, they have default forwarding strength.
 *   The packet hitting on the specific filter will be dropped.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * \see
 *  None.
 */
static shr_error_e
dnx_tune_trap_egress_filters_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX2_SUPPORT
    if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp))
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_v1_egress_filters_init(unit));
    }
    else
#endif
    {
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize PTP Drop trap
 *
 * \param [in] unit - The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_ptp_tune_trap_init(
    int unit)
{
    bcm_rx_trap_config_t config;
    int trap_id;
    SHR_FUNC_INIT_VARS(unit);

    /** configure drop trap for 1588 packets. */
    /** once the user ask to drop a specific message type, we set the Discard profile (=1) in the action table */
    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrap1588Discard, &trap_id));
    sal_memset(&config, 0, sizeof(config));
    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &config));

    config.trap_strength = dnx_data_trap.strength.max_strength_get(unit);
    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &config));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See trap_tune.h file
 */
shr_error_e
dnx_tune_trap_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initializations for handling Ingress Filters
     */
    SHR_IF_ERR_EXIT(dnx_tune_trap_ingress_filters_init(unit));

    /*
     * Initializations for handling Egress Filters
     */
    SHR_IF_ERR_EXIT(dnx_tune_trap_egress_filters_init(unit));

    /*
     * Initializations for handling PTP drop trap
     */
    SHR_IF_ERR_EXIT(dnx_ptp_tune_trap_init(unit));
exit:
    SHR_FUNC_EXIT;
}

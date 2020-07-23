/** \file trap_tune.c
 * Tune trap module file
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files. {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm/rx.h>
#include "trap_tune.h"
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_verify.h>

/*
 * }  Include files.
 */
/**
 * \brief -
 *   Initialize the trap model for for the ERPP Filters.
 *
 * \param [in] unit - Unit ID
 * \param [in] type - Trap type for the filter
 * \param [out] trap_gport - Trap Gport
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 * \see
 *  None.
 */

static shr_error_e
dnx_tune_trap_erpp_drop_profile_create(
    int unit,
    bcm_rx_trap_t type,
    bcm_gport_t * trap_gport)
{
    int trap_id;
    bcm_rx_trap_config_t trap_config;

    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.dest_port = BCM_GPORT_BLACK_HOLE;
    trap_config.flags |= BCM_RX_TRAP_UPDATE_DEST;

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, type, &trap_id));

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &trap_config));

    BCM_GPORT_TRAP_SET(*trap_gport, trap_id, dnx_data_trap.strength.default_trap_strength_get(unit),
                       trap_config.snoop_strength);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   For each of the ERPP Filters initialize trap model.
 *   The filters are enabled by default, they have default forwarding strength.
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
dnx_tune_trap_erpp_filters_init(
    int unit)
{
    bcm_gport_t trap_gport_drop = 0;
    uint8 unknown_da_enabled = FALSE;
    uint8 erpp_filters_non_separate_enablers;
    int system_headers_mode;
    SHR_FUNC_INIT_VARS(unit);

    unknown_da_enabled = dnx_data_trap.erpp.feature_get(unit, dnx_data_trap_erpp_unknown_da_trap_en);
    erpp_filters_non_separate_enablers =
        dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_erpp_filters_non_separate_enablers);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    SHR_IF_ERR_EXIT(dnx_tune_trap_erpp_drop_profile_create(unit, bcmRxTrapEgUserDefine, &trap_gport_drop));

    SHR_IF_ERR_EXIT(dnx_rx_trap_erpp_ser_handling_init(unit, BCM_GPORT_TRAP_GET_ID(trap_gport_drop)));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgRqpDiscard, trap_gport_drop));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgInvalidDestPort, trap_gport_drop));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapDssStacking, trap_gport_drop));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapLagMulticast, trap_gport_drop));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgHairPinFilter, trap_gport_drop));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpmcTtlErr, trap_gport_drop));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv4Ttl0, trap_gport_drop));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv4Ttl1, trap_gport_drop));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgExcludeSrc, trap_gport_drop));

    if (erpp_filters_non_separate_enablers == FALSE)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv4VersionError, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv4ChecksumError, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv4HeaderLengthError, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv4TotalLengthError, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv4HasOptions, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv4SipEqualDip, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv4DipZero, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv4SipIsMc, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set
                        (unit, 0, bcmRxTrapEgIpv6UnspecifiedDestination, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv6LoopbackAddress, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv6MulticastSource, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv6UnspecifiedSource, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv6LocalLinkDestination, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv6LocalSiteDestination, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv6LocalLinkSource, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv6LocalSiteSource, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set
                        (unit, 0, bcmRxTrapEgIpv6Ipv4CompatibleDestination, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv6Ipv4MappedDestination, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv6NextHeaderNull, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv6VersionError, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTcpSnFlagsZero, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTcpSnZeroFlagsSet, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTcpSynFin, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTcpEqualPorts, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set
                        (unit, 0, bcmRxTrapEgTcpFragmentIncompleteHeader, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTcpFragmentOffsetLt8, trap_gport_drop));
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgUdpEqualPorts, trap_gport_drop));
    }

    if (unknown_da_enabled && system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        /** Trap is not working in Jr2-A0(Jr2 System Headers mode) */
        SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgUnknownDa, trap_gport_drop));
    }

exit:
    SHR_FUNC_EXIT;
}

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
 *   For each of the ETPP Filters initialize trap model.
 *   The filters are enabled by default, they have default forwarding strength.
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
dnx_tune_trap_etpp_filters_init(
    int unit)
{
    bcm_gport_t trap_gport = 0;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Setting the Trap GPORT for DROP profile with default trap strength and 0 for snoop strength.
     */
    BCM_GPORT_TRAP_SET(trap_gport,
                       BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, dnx_data_trap.strength.default_trap_strength_get(unit), 0);

    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxPortNotVlanMember, trap_gport));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxSplitHorizonFilter, trap_gport));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxStpStateFail, trap_gport));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxProtectionPathUnexpected, trap_gport));

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
     * Initializations for handling ERPP Filters
     */
    SHR_IF_ERR_EXIT(dnx_tune_trap_erpp_filters_init(unit));
    /*
     * Initializations for handling ETPP Filters
     */
    SHR_IF_ERR_EXIT(dnx_tune_trap_etpp_filters_init(unit));
    /*
     * Initializations for handling PTP drop trap
     */
    SHR_IF_ERR_EXIT(dnx_ptp_tune_trap_init(unit));
exit:
    SHR_FUNC_EXIT;
}

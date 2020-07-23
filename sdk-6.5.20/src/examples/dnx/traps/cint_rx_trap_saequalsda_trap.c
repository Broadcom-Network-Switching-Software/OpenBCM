/* $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_rx_trap_saequalsda_trap.c
 * Purpose: Creates a simple configuration for SA=DA trap, to drop the matching packets.
 *          Destroy function will destroy the trap, which ID is held in a global variable for easy access.
 *
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_saequalsda_trap.c
 *
 * Main Function:
 *      cint_rx_trap_saequalsda_trap_main(unit,tc,dp,dest_port,fwd_header,meter_cmd,fwd_strength);
 * Destroy Function:
 *      cint_rx_trap_saequalsda_trap_destroy(unit);
 *
 *
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_saequalsda_trap.c
 * cint
 * cint_rx_trap_saequalsda_trap_main(0,3,2,202,4,4,7);
 */

int cint_rx_trap_saequalsda_trap_trap_id;

/**
 * \brief
 *
 *  This function creates and sets the SA=DA filter,
 *  the supplied paramters to the function are the trap actions.
 *
 * \param [in] unit         - The unit number.
 * \param [in] tc           - Traffic Class.
 * \param [in] dp           - Drop Precedence.
 * \param [in] dest_port    - Dest Port.
 * \param [in] fwd_header   - Forwarding header.
 * \param [in] meter_cmd    - Meter Command.
 * \param [in] fwd_strength - Trap Strength.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_saequalsda_trap_main(int unit,
    int tc,
    int dp,
    int dest_port,
    int fwd_header,
    int meter_cmd,
    int fwd_strength)
{
    bcm_rx_trap_config_t trap_config;
    int flags = 0;
    int rv;

    trap_config.flags = BCM_RX_TRAP_UPDATE_PRIO | BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_LEARN_DISABLE
                        | BCM_RX_TRAP_UPDATE_COLOR
                        | BCM_RX_TRAP_UPDATE_METER_CMD | BCM_RX_TRAP_TRAP | BCM_RX_TRAP_UPDATE_MAPPED_STRENGTH
                        | BCM_RX_TRAP_BYPASS_FILTERS;

    trap_config.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;

    trap_config.dest_port = dest_port;
    trap_config.color = dp;
    trap_config.prio = tc;
    trap_config.forwarding_header = fwd_header;
    trap_config.trap_strength = fwd_strength;
    trap_config.mapped_trap_strength = fwd_strength - 1;
    trap_config.meter_cmd = meter_cmd;

    rv = cint_utils_rx_trap_create_and_set(unit, flags, bcmRxTrapLinkLayerSaEqualsDa, trap_config, &cint_rx_trap_saequalsda_trap_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf(" Error (%d), in cint_utils_rx_trap_create_and_set\n",  rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *
 *  This function destroys the trap,
 *  that was created in the main function.
 *
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int cint_rx_trap_saequalsda_trap_destroy(int unit)
{
    int rv;

    rv = bcm_rx_trap_type_destroy(unit, cint_rx_trap_saequalsda_trap_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf(" Error (%d), in bcm_rx_trap_type_destroy\n",  rv);
        return rv;
    }

    return rv;
}

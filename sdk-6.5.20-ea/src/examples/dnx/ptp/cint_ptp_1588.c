/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~1588~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_ptp_1588.c
 * Purpose: 1588 protocol use examples
 *
 * BEFORE USING EXAMPLES IN THIS CINT:
 * ------------------------------------:
 * 1) Make sure the Makefile used includes PTP feature.
 * 2) Enable TS_PLL clock by enabling/un-commenting following soc property:
 *      phy_1588_dpll_frequency_lock=1
 *      (otherwise the TS clock would not run, as a result the CF won`t be updated)
 * 3) For using external components beside the SDK (for example: ARM), use the following Soc properties:
 *      num_queues_pci=40
 *      num_queues_uc1=8
 *      custom_feature_ptp_cosq_port=204
 *      ucode_port_204=CPU.40
 *      tm_port_header_type_in_204=ETH
 *      tm_port_header_type_out_204=ETH
 *    
 *    Note**: "ext_1588_mac_enable" Soc property, which were used in previous devices is not relevant for BCM88690 and above. 
 *            external mac can be enabled by API bcm_port_timesync_config_set, flag BCM_PORT_TIMESYNC_EXTERNAL_MAC_ENABLE.
 *
 * 4) In case the system contain more than one device, need to run Broadsync application (see 'Broadsync' section below).
 *
 * Provides the following functionalities:
 *
 * - 1588 one/two step TC (Transparent clock).
 * - trap/drop/snoop/fwd control over 1588 messages.
 *
 *    In transport clock mode, every fabric based system is observed by the network as a single transparent
 *    clock entity, with the residence time calculated from the moment the PTP packet enters the system through
 *    one port, to the time it exits through another.
 *    when 1 step TC is enabled:
 *       The system updates the correction field (CF) of Event messages
 *    when 2 step TC is enabled:
 *       The system records the TX time of Event messages in a FIFO (the application can later read the TX time from the FIFO,
 *       calculate residence time and update the relevant Follow up message)
 *
 *      Event Messages:
 *        1. Sync
 *        2. Delay_Req
 *        3. Pdelay_Req
 *        4. Pdelay_Resp
 *      General Messages:
 *        1. Announce
 *        2. System
 *        3. Follow_Up
 *        4. Delay_Resp
 *        5. Pdelay_Resp_Follow_Up
 *        6. Management
 *        7. Signaling
 *
 *    Supported 1588 encapsulations:
 *      follwing 1588 encapsulations are supported:
 *        1. 1588oE                    switched
 *        2. 1588oUDPoIPoE             switched/IP routed
 *        3. 1588oUDPoIPoIPoE          switched/IP routed/IP terminated
 *        4. 1588oUDPoIPoMPLSoE        switched/MPLS routed/MPLS terminated
 *        5. 1588oEoMPLSoE             switched/MPLS routed/MPLS terminated
 *      Note: For device BCM88690 and above more encapsulations are supported, as configured in identification_cam table. 
 *      See architucture Spec or driver implematation for details.
 *      It is user responsibility not to cross the max size (after forwarding) as mentione in architucture identification table.
 *
 *    CF update:
 *        the CF (8 bytes) update is done in bytes number 2,3 (zero based).
 *        additionally, least significant bit can be changed for internal use.
 *
 *    Broadsync:
 *     1588 TC (Transparent clock) relay on clock synchronization between devices in the system i.e. Broadsync,
 *     for Broadsync configuration see BCM shell "bs config" command (see example in cint_time.c).
 *
 *   Note: For modify the path delay per port, call bcm_port_control_set, type=bcmPortControl1588P2PDelay (nano-seconds)
 */
/*
 * how to run:
 *
 * For example: enable one step TC for unit 0 port 13, 14 (both In-port and Out-port need to be configured.
 *              the packet timestamp is saved at the ingress, the packet CF is updated at the egress):
 *
 * cint
 * int rv;
 * rv = ieee_1588_port_set(unit,13,1);
 * print rv;
 * rv = ieee_1588_port_set(unit,14,1);
 * print rv;
 *
 * For example: disable one step TC for unit 0 port 13:
 *
 * cint
 * int rv;
 * rv = ieee_1588_port_set(unit,13,0);
 * print rv;
 */

int uk_port = 204; /* uKernel port */


/*
 * enable = 1, enable  one step TC
 * enable = 0, disable one step TC
 */
int ieee_1588_port_set(int unit, bcm_gport_t port, int enable)
{
    int rv = BCM_E_NONE, config_count;
    bcm_port_timesync_config_t timesync_port_config;
    bcm_port_timesync_config_t *timesync_port_config_ptr;
    bcm_port_phy_timesync_config_t timesync_phy_config;

    /*
     * Configure 1588 in Port level
     */
    if (0 == enable)
    {
        timesync_port_config_ptr = NULL;
        config_count        = 0;
    }
    else
    {
        /* flags to enable 1 step TC                                                                   */
        timesync_port_config.flags = (BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP);
        /* to enable 2 step TC use folowing flags instead                                              */
        /* timesync_config.flags = (BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP); */

        /** for ukernel port, enable external mac */
        if(uk_port == port)
        {
            timesync_port_config.flags |= BCM_PORT_TIMESYNC_EXTERNAL_MAC_ENABLE;
            timesync_port_config.pkt_tocpu = 0;
        }
        timesync_port_config.pkt_drop  = 0;
        timesync_port_config.pkt_tocpu = 0;


        /* Another example to trap PDELAY_REQ message and drop PDELAY_RESP message use following        */
        /* timesync_config.pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_PDELAY_REQ;                               */
        /* timesync_config.pkt_drop  |= BCM_PORT_TIMESYNC_PKT_PDELAY_RESP;                              */
        /*                                                                                              */
        /* 2 notes:                                                                                     */
        /* 1) each 1588 message can be added to pkt_tocpu bitmask or pkt_drop, not both.                */
        /*    in case no bit is turned on the packet will be forwarded.                                 */
        /* 2) prior to trap 1588 message, the 1588 trap need to be cofigures with a port,               */
        /*    for example, following will trap 1588 messages raised in pkt_tocpu bitmask to port 200:   */
        /*      cint ../../../../src/examples/dnx/traps/cint_trap_utilities.c                           */
        /*      cint                                                                                    */
        /*      int rv;                                                                                 */
        /*      int trap_id_handle;                                                                     */
        /*      bcm_rx_trap_config_t config = { 0 };                                                    */
        /*      config.trap_strength = 15;                                                              */
        /*      config.flags = BCM_RX_TRAP_UPDATE_DEST;                                                 */
        /*      config.dest_port = 200;                                                                 */
        /*      rv = rx__trap_create_and_set(unit, 0, bcmRxTrap1588, &config, &trap_id_handle);         */
        
        timesync_port_config_ptr = &timesync_port_config;
        config_count        = 1;
    }

    rv =  bcm_port_timesync_config_set(unit, port, config_count, timesync_port_config_ptr);
    if (BCM_FAILURE(rv))
    {
        if(rv == BCM_E_UNAVAIL)
        {
            printf("Error, bcm_port_timesync_config_set 1588 is not supported!\n");
        }
        else
        {
            printf("Error, bcm_port_timesync_config_set rv = %d\n", rv);
        }
        return rv;
    }

    /*
     * Configure 1588 in PHY level
     */
    bcm_port_phy_timesync_config_t_init(&timesync_phy_config);

    if (enable)
    {
        timesync_phy_config.flags = (BCM_PORT_PHY_TIMESYNC_ENABLE | BCM_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE);
    }
    else
    {
        timesync_phy_config.flags = 0;
    }

    rv = bcm_port_enable_set(unit, port, 0);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_port_enable_set rv = %d\n", rv);
        return rv;
    }

    rv =  bcm_port_phy_timesync_config_set(unit, port, &timesync_phy_config);
    if (BCM_FAILURE(rv))
    {
        if(rv == BCM_E_UNAVAIL)
        {
            printf("Error, bcm_port_phy_timesync_config_set 1588 is not supported!\n");
        }
        else
        {
            printf("Error, bcm_port_phy_timesync_config_set rv = %d\n", rv);
        }
        return rv;
    }

    rv = bcm_port_enable_set(unit, port, 1);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_port_enable_set rv = %d\n", rv);
        return rv;
    }
    return rv;
}


/*
 * enable = 1, one step TC is enabled
 * enable = 0, one step TC is disabled
 */
int ieee_1588_port_get(int unit, bcm_gport_t port, int *enabled)
{
    int rv = BCM_E_NONE, array_count;
    int ptp_port_enable, ptp_phy_enable;
    bcm_port_timesync_config_t timesync_port_config;
    bcm_port_phy_timesync_config_t timesync_phy_config;

    /*
     * Get 1588 enable state in port level
     */
    rv =  bcm_port_timesync_config_get(unit, port, 1, &timesync_port_config, &array_count);
    if (BCM_FAILURE(rv))
    {
        if (rv == BCM_E_UNAVAIL)
        {
            printf("Error, bcm_port_timesync_config_set 1588 is not supported!\n");
        }
        else
        {
            printf("Error, bcm_port_timesync_config_get rv = %d\n", rv);
        }
        return rv;
    }
    ptp_port_enable = (0 == array_count) ? 0 : 1;

    /*
     * Get 1588 enable state in PHY level
     */
    rv = bcm_port_phy_timesync_config_get(unit, port, &timesync_phy_config);
    if (BCM_FAILURE(rv))
    {
        if (rv == BCM_E_UNAVAIL)
        {
            printf("Error, bcm_port_timesync_config_set 1588 is not supported!\n");
        }
        else
        {
            printf("Error, bcm_port_timesync_config_get rv = %d\n", rv);
        }
        return rv;
    }
    ptp_phy_enable = (timesync_phy_config.flags & BCM_PORT_PHY_TIMESYNC_ENABLE) ? 1 : 0;

    if ((ptp_port_enable == 1) && (ptp_phy_enable == 0))
    {
        *enabled = 0;
        printf("1588 is only enabled in port level, disabled in PHY level!\n");
    }
    else if ((ptp_port_enable == 0) && (ptp_phy_enable == 1))
    {
        *enabled = 0;
        printf("1588 is only enabled in PHY level, disabled in port level!\n");
    }
    else if ((ptp_port_enable == 0) && (ptp_phy_enable == 0))
    {
        *enabled = 0;
        printf("1588 is disabled in PHY level and port level!\n");
    }
    else
    {
        *enabled = 1;
    }
    return rv;
}

/* load cint ../../../../src/examples/dnx/traps/cint_trap_utilities.c */
/* used to trap the 1588 messages that defined in API bcm_port_timesync_config_set (config.pkt_tocpu) */
/* Usually, 1588 external mac will be defined for the dest_port, */
/* in API bcm_port_timesync_config_set (flag=BCM_PORT_TIMESYNC_EXTERNAL_MAC_ENABLE) */
int ieee_1588_trap(int unit, int dest_port)
{
    int rv = BCM_E_NONE;
    int trap_id_handle;
    bcm_rx_trap_config_t config = { 0 };
    config.trap_strength = 15;
    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = dest_port;
    rv = cint_utils_rx_trap_create_and_set(unit, 0, bcmRxTrap1588, &config, &trap_id_handle);
    if (rv != BCM_E_NONE) {
        printf("Error in cint_utils_rx_trap_create_and_set(%s) \n", bcm_errmsg(rv));
        return rv;
    }  
    return rv;
}

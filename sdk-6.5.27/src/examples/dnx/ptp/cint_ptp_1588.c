/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~1588~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
 *
 */
/*
 * how to run:
 *
 * For example: enable one step TC for unit 0 port 13, 14 (both In-port and Out-port need to be configured.
 *              the packet timestamp is saved at the ingress, the packet CF is updated at the egress):
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/ptp/cint_ptp_1588.c
  cint
  int rv;
  rv = ieee_1588_port_set(unit,13,1);
  print rv;
  rv = ieee_1588_port_set(unit,14,1);
  print rv;
 * Test Scenario - end
 *
 * For example: disable one step TC for unit 0 port 13:
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/ptp/cint_ptp_1588.c
  cint
  int rv;
  rv = ieee_1588_port_set(unit,13,0);
  print rv;
 * Test Scenario - end
 *
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
            rv = BCM_E_NONE;
        }
        BCM_IF_ERROR_RETURN_MSG(rv, "");
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
        if(rv == BCM_E_UNAVAIL)
        {
            printf("Error, bcm_port_enable_set is not supported!\n");
            rv = BCM_E_NONE;
        }
        BCM_IF_ERROR_RETURN_MSG(rv, "");
    }

    
    rv = ieee_1588_port_pcs_reset(unit, port, 1);
    if (BCM_FAILURE(rv))
    {
        printf("Error, ieee_1588_port_pcs_reset rv = %d\n", rv);
        return rv;
    }

    /* Make sure link down is detected by linkscan.
     * TSCBH gen1 PCS reset WAR will clear pcs latch status regiser in phymod timestamp enable API.
     * If without this delay, the link up status might not be changed when the port is disabled and enabled again. This will lead to timeysnc adjust not be called.
     */
    sal_sleep(1);
    rv =  bcm_port_phy_timesync_config_set(unit, port, &timesync_phy_config);
    if (BCM_FAILURE(rv))
    {
        if(rv == BCM_E_UNAVAIL)
        {
            printf("Error, bcm_port_phy_timesync_config_set 1588 is not supported!\n");
            rv = BCM_E_NONE;
        }
        BCM_IF_ERROR_RETURN_MSG(rv, "");
    }

    BCM_IF_ERROR_RETURN_MSG(ieee_1588_port_pcs_reset(unit, port, 0), "");

    rv = bcm_port_enable_set(unit, port, 1);
    if (BCM_FAILURE(rv))
    {
        if(rv == BCM_E_UNAVAIL)
        {
            printf("Error, bcm_port_enable_set is not supported!\n");
            rv = BCM_E_NONE;
        }
        BCM_IF_ERROR_RETURN_MSG(rv, "");
    }

    return BCM_E_NONE;
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
        BCM_IF_ERROR_RETURN_MSG(rv, "");
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
        BCM_IF_ERROR_RETURN_MSG(rv, "");
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
    return BCM_E_NONE;
}

/* load cint ../../../../src/examples/dnx/traps/cint_trap_utilities.c */
/* used to trap the 1588 messages that defined in API bcm_port_timesync_config_set (config.pkt_tocpu) */
/* Usually, 1588 external mac will be defined for the dest_port, */
/* in API bcm_port_timesync_config_set (flag=BCM_PORT_TIMESYNC_EXTERNAL_MAC_ENABLE) */
int ieee_1588_trap(int unit, int dest_port)
{
    int trap_id_handle;
    bcm_rx_trap_config_t config = { 0 };
    config.trap_strength = 15;
    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = dest_port;
    BCM_IF_ERROR_RETURN_MSG(cint_utils_rx_trap_create_and_set(unit, 0, bcmRxTrap1588, &config, &trap_id_handle), "");
    return BCM_E_NONE;
}

/*
 * Example of configure 1588 traffic flow from mhost 1 (R5 1) to physical ports via P2P Cross connect
 *
 * Main steps of configuration:
 *    1. Creating LIF-1, LIF-2, LIF-3 and LIF-4.
 *    2. P2P cross-connect LIF-1 to LIF-3; none symmetric, one direction.
 *    3. P2P cross-connect LIF-2 to LIF-4; none symmetric, one directionfrom.
 *
 * INPUT: unit  - unit
 *        inP1  - the port used by mhost 1 to inject 1588 traffic.
 *        outP1 - the physical port where 1588 traffic is sent out.
 *        outP2 - the physical port where 1588 traffic is sent out.
 *        vid1  - vid1 and vid2 are used to distinguish 1588 traffic to different physical ports.
 *        vid2  - see above.
 * For Example
 * Test Scenario - start
     cint ../../../../src/examples/dnx/ptp/cint_ptp_1588.c
     cint
     print ieee_1588_vswitch_cross_connect_ac_to_ac(0, 200, 13, 14, 24, 25);
     exit;
 *    # traffic to physical port 13 with vid 24
     tx 1 psrc=200 data=011b19000000aabbccaaaaa18100001888f70002002c18000000fffdc348ad2c00000000000000112233445566aa0001000100fc000000000009244638
 *    # traffic to physical port 14 with vid 25
     tx 1 psrc=200 data=011b19000000aabbccaaaaa18100001988f70002002c18000000fffdc348ad2c00000000000000112233445566aa0001000100fc000000000009244638
 * Test Scenario - end
 */
int
ieee_1588_vswitch_cross_connect_ac_to_ac(
    int unit,
    int inP1,
    int outP1,
    int outP2,
    int vid1,
    int vid2)
{
    char error_msg[200]={0,};
    bcm_gport_t inLif1;
    bcm_gport_t inLif2;
    bcm_gport_t outLif1;
    bcm_gport_t outLif2;
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t match_info;
    bcm_vswitch_cross_connect_t gports;

    /** Set vlan domain */
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", inP1);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, inP1, bcmPortClassId, inP1), error_msg);
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", outP1);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, outP1, bcmPortClassId, outP1), error_msg);
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", outP2);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, outP2, bcmPortClassId, outP2), error_msg);

    /** Creat InLIF1 */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    inLif1 = vlan_port.vlan_port_id;
    bcm_port_match_info_t_init(&match_info);
    match_info.match = BCM_PORT_MATCH_PORT_CVLAN;
    match_info.port = inP1;
    match_info.match_vlan = vid1;
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
    snprintf(error_msg, sizeof(error_msg), "(port = %d, vlan = %d)", inP1, vid1);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_match_add(unit, inLif1, &match_info), error_msg);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vid1, inP1, BCM_VLAN_GPORT_ADD_INGRESS_ONLY), error_msg);

    /** Creat InLIF2 */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    inLif2 = vlan_port.vlan_port_id;
    bcm_port_match_info_t_init(&match_info);
    match_info.match = BCM_PORT_MATCH_PORT_CVLAN;
    match_info.port = inP1;
    match_info.match_vlan = vid2;
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
    snprintf(error_msg, sizeof(error_msg), "(port = %d, vlan = %d)", inP1, vid2);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_match_add(unit, inLif2, &match_info), error_msg);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vid2, inP1, BCM_VLAN_GPORT_ADD_INGRESS_ONLY), error_msg);

    /** Creat OutLIF1 */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = outP1;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    outLif1 = vlan_port.vlan_port_id;

    /** Creat OutLIF2 */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = outP2;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    outLif2 = vlan_port.vlan_port_id;

    /** InLIF1 -> OutLIF1 */
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = inLif1;
    gports.port2 = outLif1;
    snprintf(error_msg, sizeof(error_msg), "(inLif1 0x%x -> outLif1 0x%x)", inLif1, outLif1);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &gports), error_msg);

    /** InLIF2 -> OutLIF2 */
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = inLif2;
    gports.port2 = outLif2;
    snprintf(error_msg, sizeof(error_msg), "(inLif2 0x%x -> outLif2 0x%x)", inLif2, outLif2);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &gports), error_msg);
    return BCM_E_NONE;
}


/*
 *  Example of configure PCS reset for port if there is only one port on TSCF core
 */
int ieee_1588_port_pcs_reset(int unit, bcm_port_t port, int reset)
{
    bcm_port_interface_info_t intf_info;
    bcm_port_mapping_info_t map_info;
    uint32 port_get_flags;

    uint32 cdu_nof                 = *(dnxc_data_get(unit, "nif", "eth", "cdu_nof", NULL));
    uint32 clu_nof                 = *(dnxc_data_get(unit, "nif", "eth", "clu_nof", NULL));

    if (!clu_nof)
    {
        printf("skip the pcs reset for non-clu devices\n");
        return BCM_E_NONE;
    }

    uint32 nof_cdu_lanes_in_pm     = *(dnxc_data_get(unit, "nif", "eth", "nof_cdu_lanes_in_pm", NULL));
    uint32 nof_clu_lanes_in_pm     = *(dnxc_data_get(unit, "nif", "eth", "nof_clu_lanes_in_pm", NULL));
    uint32 first_falcon_phy_id     = cdu_nof * nof_cdu_lanes_in_pm;

    /* Get the port interface type */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, port, port_get_flags, intf_info, map_info), "");

    if (intf_info.interface != BCM_PORT_IF_NIF_ETH)
    {
        printf("not a nif eth port, skip the pcs reset\n");
        return BCM_E_NONE;
    }

    /* PCS reset is only required for CLU port */
    if ( intf_info.phy_port < first_falcon_phy_id)
    {
        printf("not a clu port, skip the pcs reset\n");
        return BCM_E_NONE;
    }

    /* do PCS reset */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_phy_control_set(unit, port, BCM_PORT_PHY_CONTROL_TX_RESET, reset), "");
    sal_sleep(1);

    return BCM_E_NONE;
}

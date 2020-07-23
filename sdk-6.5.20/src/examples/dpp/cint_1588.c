/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~1588~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_1588.c
 * Purpose: 1588 protocol use examples
 *
 * BEFORE USING EXAMPLES IN THIS CINT:
 * ------------------------------------:
 * 1) 1588 is NOT SUPPORTED FOR ARAD A0, supported for ARAD B0 and above.
 * 2) Make sure the Makefile used includes PTP feature.
 * 3) Enable TS_PLL clock by enabling/un-commenting following soc property:
 *      num_queues_pci.BCM88650=40
 *      num_queues_uc1.BCM88650=8
 *      custom_feature_ptp_cosq_port.BCM88650=204
 *      custom_feature_ptp_udp_parser_enable=1
 *      ucode_port_204.BCM88650=CPU.40
 *      tm_port_header_type_in_204.BCM88650=ETH
 *      tm_port_header_type_out_204.BCM88650=ETH
 *      tm_port_header_type_in_0.BCM88650=INJECTED_2
 *      tm_port_header_type_out_0.BCM88650=TM
 *      ext_1588_mac_enable_0.BCM88650=1
 *      ext_1588_mac_enable_204.BCM88650=1
 *      phy_1588_dpll_frequency_lock.BCM88650=1
 *
 *    (otherwise the TS clock would not run, as a result the CF won`t be updated)
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
 *    CF update:
 *        the CF (8 bytes) update is done in bytes number 2,3 (zero based).
 *        additionally, least significant bit can be changed for internal use.
 *
 *    Broadsync:
 *     1588 TC (Transparent clock) relay on clock synchronization between devices in the system i.e. Broadsync,
 *     for Broadsync configuration see BCM shell "bs config" command (see example in cint_time.c).
 */
/*
 * how to run:
 *
 * For example: enable one step TC for unit 0 port 13, 14 (both In-port and Out-port need to be configured.
 *              the packet timestamp is saved at the ingress, the packet CF is updated at the egress):
 *
 * cint ../../../../src/examples/dpp/cint_1588.c
 * cint
 * int rv;
 * rv = ieee_1588_port_set(unit,13,1);
 * print rv;
 * rv = ieee_1588_port_set(unit,14,1);
 * print rv;
 *
 * For example: disable one step TC for unit 0 port 13:
 *
 * cint ../../../../src/examples/dpp/cint_1588.c
 * cint
 * int rv;
 * rv = ieee_1588_port_set(unit,13,0);
 * print rv;
 *
 * For example: PTP master running
 *  #Load ukernel image
 *  mcsload 0 BCM88660_A0_0_bfd_bhh.srec initmcs=1
 *  mcsload 1 BCM88660_A0_1_ptpfull.srec
 *  #PTP configuration
 *  ptp Register Events
 *  ptp Register Signal
 *  ptp Stack Create
 *  ptp Debug STA SYA
 *  ptp register arp
 *  ptp Clock Create NumPorts=1 ID=00:0e:03:ff:fe:00:01:09 P1=80 tr=y MaxUnicastSlaves=100
 *  ptp Port Configure 1 MAC=00:0e:03:00:01:09 IP=192.168.1.9 MC=0 AI=1 SI=-6 DI=-6 TM=mac32 Vlan=2
 *  ptp slave add IP=192.168.1.10 MAC=00:0e:03:00:01:10 SI=-6 DI=-6 AI=1
 *  s IPT_FORCE_LOCAL_OR_FABRIC FORCE_LOCAL=1
 *  cint ../../../../src/examples/dpp/cint_1588.c
 *  cint
 *  print ieee_1588_run(unit, 3, 2);
 *  exit;
 *  # Enable 1pps
 *  ptp signals set p=1 freq=1 pl=y width=100000000
 *
 * For example: PTP slave running
 *  #Load ukernel image
 *  mcsload 0 BCM88660_A0_0_bfd_bhh.srec initmcs=1
 *  mcsload 1 BCM88660_A0_1_ptpfull.srec
 *  #PTP configuration
 *  ptp Register Events
 *  ptp Register Signal
 *  ptp Stack Create
 *  ptp Debug STA SYA
 *  ptp register arp
 *  ptp Clock Create NumPorts=1 ID=00:0e:03:ff:fe:00:01:10 P1=128 MaxUnicastSlaves=100
 *  ptp Port Configure 1 MAC=00:0e:03:00:01:10 IP=192.168.1.10 MC=0 AI=1 SI=-6 DI=-6 TM=mac32 Vlan=2
 *  ptp master add static=n IP=192.168.1.9 MAC=00:b0:ae:03:58:6f SI=-6 DI=-6 AI=1
 *  s IPT_FORCE_LOCAL_OR_FABRIC FORCE_LOCAL=1
 *  cint ../../../../src/examples/dpp/cint_1588.c
 *  cint
 *  print ieee_1588_run(unit, 3, 2);
 *  exit;
 */



/*
 * enable = 1, enable  one step TC
 * enable = 0, disable one step TC
 */
int ieee_1588_port_set(int unit, bcm_gport_t port, int enable)
{
    int                           rv = BCM_E_NONE;
    bcm_port_timesync_config_t timesync_config;
    bcm_port_timesync_config_t *timesync_config_ptr;
    int config_count;

    if(0 == enable) {
        timesync_config_ptr = NULL;
        config_count        = 0;
    } else {

        /* flags to enable 1 step TC                                                                   */
        timesync_config.flags = (BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP);
        /* to enable 2 step TC use folowing flags instead                                              */
        /* timesync_config.flags = (BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP); */
        timesync_config.pkt_drop  = 0;
        timesync_config.pkt_tocpu = 0;

        /* for example to trap PDELAY_REQ message and drop PDELAY_RESP message use following            */
        /* timesync_config.pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_PDELAY_REQ;                               */
        /* timesync_config.pkt_drop  |= BCM_PORT_TIMESYNC_PKT_PDELAY_RESP;                              */
        /*                                                                                              */
        /* 2 notes:                                                                                     */
        /* 1) each 1588 message can be added to pkt_tocpu bitmask or pkt_drop, not both.                */
        /*    in case no bit is turned on the packet will be forwarded.                                 */
        /* 2) prior to trap 1588 message, the 1588 trap need to be cofigures with a port,               */
        /*    for example, following will trap 1588 messages raised in pkt_tocpu bitmask to port 200:   */
        /*      cint ../../../../src/examples/dpp/cint_rx_trap_fap.c                                    */
        /*      cint                                                                                    */
        /*      int rv;                                                                                 */
        /*      int trap_id_handle;                                                                     */
        /*      rv = set_rx_trap(unit, bcmRxTrap1588, 0, &trap_id_handle, 200);                            */
        /*      print rv;                                                                               */
        /*      print trap_id_handle;                                                                   */


        timesync_config_ptr = &timesync_config;
        config_count        = 1;
    }

    rv =  bcm_port_timesync_config_set(unit, port, config_count, timesync_config_ptr);
    if (rv != BCM_E_NONE) {
        if(rv == BCM_E_UNAVAIL) {
            printf("Error, bcm_port_timesync_config_set 1588 is not supported for ARAD A0\n");
        } else {
            printf("Error, bcm_port_timesync_config_set rv = %d\n", rv);
        }
        return rv;
    }
	/*bcm_port_control_set(unit, port, bcmPortControlMplsSpeculativeParse, 1);*/
    return rv;
}


/*
 * enable = 1, one step TC is enabled
 * enable = 0, one step TC is disabled
 */
int ieee_1588_port_get(int unit, bcm_gport_t port, int *enabled)
{
    int                           rv = BCM_E_NONE;
    bcm_port_timesync_config_t timesync_config;
    int array_count;

    rv =  bcm_port_timesync_config_get(unit, port, 1, timesync_config, &array_count);
    if (rv != BCM_E_NONE) {
        if(rv == BCM_E_UNAVAIL) {
            printf("Error, bcm_port_timesync_config_set 1588 is not supported for ARAD A0\n");
        } else {
            printf("Error, bcm_port_timesync_config_get rv = %d\n", rv);
        }
        return rv;
    }

    if(0 == array_count) {
        *enabled = 0;
        printf("1588 is DISABLED for port = %d \n", port);
    } else {
        *enabled = 1;
        printf("1588 is ENABLED  for port = %d \n", port);
    }

    return rv;
}

/*Support 1588 ingress disable and egress enable. The input parameter is ingress port*/
int ieee_1588_port_set_ingress_disable_and_egress_enable(int unit, bcm_gport_t port)
{
    int                           rv = BCM_E_NONE;
    bcm_port_timesync_config_t timesync_config;
    bcm_port_timesync_config_t *timesync_config_ptr;
    int config_count;

    /* flags to enable 1 step TC                                                                   */
    timesync_config.flags = (BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP);
    /* to enable 2 step TC use folowing flags instead                                              */
    /* timesync_config.flags = (BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP); */
    timesync_config.pkt_drop  = 0;
    timesync_config.pkt_tocpu = 0;

    /* for example to trap PDELAY_REQ message and drop PDELAY_RESP message use following            */
    /* timesync_config.pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_PDELAY_REQ;                               */
    /* timesync_config.pkt_drop  |= BCM_PORT_TIMESYNC_PKT_PDELAY_RESP;                              */
    /*                                                                                              */
    /* 2 notes:                                                                                     */
    /* 1) each 1588 message can be added to pkt_tocpu bitmask or pkt_drop, not both.                */
    /*    in case no bit is turned on the packet will be forwarded.                                 */
    /* 2) prior to trap 1588 message, the 1588 trap need to be cofigures with a port,               */
    /*    for example, following will trap 1588 messages raised in pkt_tocpu bitmask to port 200:   */
    /*      cint ../../../../src/examples/dpp/cint_rx_trap_fap.c                                    */
    /*      cint                                                                                    */
    /*      int rv;                                                                                 */
    /*      int trap_id_handle;                                                                     */
    /*      rv = set_rx_trap(unit, bcmRxTrap1588, 0, &trap_id_handle, 200);                            */
    /*      print rv;                                                                               */
    /*      print trap_id_handle;                                                                   */


    timesync_config_ptr = &timesync_config;
    config_count = 0;

    rv =  bcm_port_timesync_config_set(unit, port, config_count, timesync_config_ptr);
    if (rv != BCM_E_NONE) {
        if(rv == BCM_E_UNAVAIL) {
            printf("Error, bcm_port_timesync_config_set 1588 is not supported for ARAD A0\n");
        } else {
            printf("Error, bcm_port_timesync_config_set rv = %d\n", rv);
        }
        return rv;
    }

    return rv;
}



/* The port number is based on followed soc configuration
 *     num_queues_pci.BCM88375=40
 *     num_queues_uc1.BCM88375=8
 *     custom_feature_ptp_cosq_port.BCM88375=204
 */
int uc_port = 204;
int ieee_1588_trap(int unit, int port) {

    int trap_id=0;
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_group_t grp;
    bcm_gport_t gport;

    /* create trap */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_type_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Configure trap attribute to update destination */
    sal_memset(&trap_config, 0, sizeof(trap_config));
    trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP | BCM_RX_TRAP_REPLACE);
    trap_config.trap_strength = 0;
    trap_config.dest_port = uc_port;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_set (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    BCM_GPORT_TRAP_SET(gport, trap_id, 7, 0);

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4DstPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionTrap);

    rv = bcm_field_group_create(unit, qset, 2, &grp);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_group_action_set(unit, grp, aset);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_action_set (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_create(unit, grp, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_create for L4 port 319 (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, ent, port, 0xffffffff);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualify_InPort for L4 port 319 (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Port 319 is dedicated for 1588 protocol */
    rv = bcm_field_qualify_L4DstPort(unit, ent, 319, 0xffff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_L4DstPort for L4 port 319 (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, ent, bcmFieldActionTrap, gport, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_action_add for L4 port 319 (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_install for L4 port 319 (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_create(unit, grp, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_create for L4 port:320 (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, ent, port, 0xffffffff);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualify_InPort for L4 port:320 (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Port 320 is dedicated for 1588 protocol */
    rv = bcm_field_qualify_L4DstPort(unit, ent, 320, 0xffff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_L4DstPort for L4 port:320 (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, ent, bcmFieldActionTrap, gport, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_action_add for L4 port:320 (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_install for L4 port:320 (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

int ieee_1588_run(int unit, int port, int vlan) {
    int rv = BCM_E_NONE;
    rv = ieee_1588_port_set(unit, uc_port, 1);
    if (rv != BCM_E_NONE) {
        printf("Error in ieee_1588_port_set for port:%d (%s) \n", uc_port, bcm_errmsg(rv));
        return rv;
    }

    rv = ieee_1588_port_set(unit, port, 1);
    if (rv != BCM_E_NONE) {
        printf("Error in ieee_1588_port_set for port:%d (%s) \n", port, bcm_errmsg(rv));
        return rv;
    }

    rv = ieee_1588_trap(unit, port);
    if (rv != BCM_E_NONE) {
        printf("Error in ieee_1588_trap (%s) \n", port, bcm_errmsg(rv));
        return rv;
    }

    /* Setup general bridge service for vlan 2 */
    rv = bcm_vlan_create(unit, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_create vlan:%d (%s) \n", vlan, bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vlan, port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_gport_add vlan:%d port:%d(%s) \n", vlan, port, bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vlan, uc_port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_gport_add vlan:%d port:%d (%s) \n", vlan, port, bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* 1588oMPLS L2vpn */

/*

cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
cint ../../../../src/examples/dpp/utility/cint_utils_multicast.c
cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c
cint ../../../../src/examples/sand/utility/cint_sand_utils_mpls.c
cint ../../../../src/examples/dpp/utility/cint_utils_mpls_port.c
cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
cint ../../../../src/examples/dpp/internal/cint_spag_extended_vpws.c
cint ../../../../src/examples/dpp/cint_vswitch_metro_mp.c
cint ../../../../src/examples/dpp/cint_field_extended_fwd_tbl_entry_mngmt.c
cint ../../../../src/examples/dpp/cint_field_4_label_push.c
cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
cint ../../../../src/examples/dpp/cint_port_tpid.c
cint ../../../../src/examples/dpp/cint_multi_device_utils.c
cint ../../../../src/examples/dpp/cint_vswitch_cross_connect_p2p_multi_device.c
cint ../../../../src/examples/dpp/cint_qos_vpls.c
cint
print mpls_various_scenarios_l2vpn_ler_pop_1bl(0, 13, 14);
exit;
*/
int mpls_various_scenarios_l2vpn_ler_pop_1lbl(int unit, int port1, int port2){
    int rv = BCM_E_NONE;

    /* Create P2P connection */
    rv = run(&unit, 1/*nof_units*/,
            port1, 2/*pwe*/, 0/*protection1*/, 0,
            port2, 1/*vlan*/, 0/*protection2*/, 0, 0 /* facility or path protection type */);
    if (rv != BCM_E_NONE) {
       printf("Error, in run mpls_various_scenarios_l2vpn_ler_pop_1bl \n");
       return rv;
    }
    return rv;
}

int mpls_various_scenarios_l2vpn_ler_push_3lbl(int unit, int port1, int port2){
    int rv = BCM_E_NONE;

    /* Create P2P connection */
    rv = run(&unit, 1/*nof_units*/,
            port1, 1/*vlan*/, 0/*protection1*/, 0,
            port2, 2/*pwe*/, 0/*protection2*/, 0, 0 /* facility or path protection type */);
    if (rv != BCM_E_NONE) {
       printf("Error, in run mpls_various_scenarios_l2vpn_ler_pop_1bl \n");
       return rv;
    }
    return rv;
}

int mpls_various_scenarios_l2vpn_lsr_swap_push_2lbl(int unit, int port1, int port2){
    int rv = BCM_E_NONE;

    /* Create P2P connection */
    rv = run(&unit, 1/*nof_units*/,
            port1, 2/*vlan*/, 0/*protection1*/, 0,
            port2, 2/*pwe*/, 0/*protection2*/, 0, 0 /* facility or path protection type */);
    if (rv != BCM_E_NONE) {
       printf("Error, in run mpls_various_scenarios_l2vpn_ler_pop_1bl \n");
       return rv;
    }
    return rv;
}

int mpls_various_scenarios_l2vpn_ler_pop_2lbl(int unit, int uni_port, int nni_port){
    int rv = BCM_E_NONE;
    bcm_l2_station_t station;
    int station_id;
    bcm_mac_t my_mac={0x0,0x0,0x0,0x0,0x0,0x11};
    bcm_pbmp_t pbmp, ubmp;
    /* uni */
    int ac_vlan=100;
    bcm_vlan_port_t vlan_port_uni;
    /* nni */
    int nniVlan = 200;
    int outLabel1 = 1000;
    int outpwlabel1 = 5000;
    int inLabel1 = 2000;
    int inpwlabel1 = 6000;
    bcm_mpls_egress_label_t label_array[2];
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    uint8 g_nhop_mac1[6]={0x0,0x0,0x0,0x0,0x0,0x22};
    bcm_mpls_tunnel_switch_t entry;
    bcm_vswitch_cross_connect_t gports;

    rv = bcm_switch_control_set(0, bcmSwitchMplsPipeTunnelLabelExpSet, 0);
    if (rv != BCM_E_NONE) {
       printf("Error, in run bcm_switch_control_set \n");
       return rv;
    }

    sal_memcpy(station.dst_mac, my_mac, 6);
    sal_memset(station.dst_mac_mask, 0xff, 6);
    station.flags = 0;
    station.src_port_mask = 0;
    station.vlan_mask = 0;
    rv = bcm_l2_station_add(unit, &station_id, &station);
    if (rv != BCM_E_NONE) {
       printf("Error, in run bcm_l2_station_add \n");
       return rv;
    }

    /* uni port */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    rv = bcm_vlan_create(unit, ac_vlan);
    if (rv != BCM_E_NONE) {
       printf("Error, in run bcm_vlan_create \n");
       return rv;
    }
    BCM_PBMP_PORT_ADD(pbmp, uni_port);
    rv = bcm_vlan_port_add(unit, ac_vlan, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
       printf("Error, in run bcm_vlan_port_add \n");
       return rv;
    }
    bcm_vlan_port_t_init(&vlan_port_uni);
    vlan_port_uni.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port_uni.port = uni_port;
    vlan_port_uni.match_vlan = ac_vlan;
    vlan_port_uni.egress_vlan = ac_vlan;
    vlan_port_uni.vsi = 0;
    rv = bcm_vlan_port_create(unit, &vlan_port_uni);
    if (rv != BCM_E_NONE) {
       printf("Error, in run bcm_vlan_port_create \n");
       return rv;
    }
    printf("vlan_port_uni.vlan_port_id 0x%08x, AC global lif 0x%x\n", (vlan_port_uni.vlan_port_id),(vlan_port_uni.vlan_port_id)&0x3FFFF);


    /* nni port */
    bcm_l3_intf_t l3if;
    bcm_mpls_port_t mpls_port;

    print bcm_vlan_create(0, nniVlan);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, nni_port);
    rv = bcm_vlan_port_add(0, nniVlan, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
       printf("Error, in run bcm_vlan_port_add \n");
       return rv;
    }
    /* Create VSI RIF for MPLS tunnel application Used for MyMAC check
        *  If Packet Destination MAC=RIF MAC, termination is done
        */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    l3if.l3a_vid = nniVlan;
    l3if.l3a_ttl = 31;
    l3if.l3a_mtu = 1524;
    l3if.l3a_intf_id = nniVlan;
    sal_memcpy(l3if.l3a_mac_addr, my_mac, 6);
    rv = bcm_l3_intf_create(0, &l3if);
    if (rv != BCM_E_NONE) {
       printf("Error, in run bcm_vlan_port_add \n");
       return rv;
    }

    bcm_mpls_egress_label_t_init(&label_array[0]);
    label_array[0].tunnel_id = 0;
    label_array[0].label = outLabel1;
    label_array[0].exp = 4;
    label_array[0].ttl = 255;
    label_array[0].l3_intf_id = l3if.l3a_intf_id;
    label_array[0].flags =BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    rv = bcm_mpls_tunnel_initiator_create(0,0,1,label_array);
    if (rv != BCM_E_NONE) {
       printf("Error, in run bcm_mpls_tunnel_initiator_create \n");
       return rv;
    }
    printf("tunnel id 0x%08x, global lif 0x%x\n", (label_array[0].tunnel_id),(label_array[0].tunnel_id) & 0x1FFFFFFF);

    bcm_l3_egress_t_init(&l3eg);
    l3eg.intf = label_array[0].tunnel_id;
    l3eg.port = nni_port;
    l3eg.flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    sal_memcpy(l3eg.mac_addr, g_nhop_mac1, 6);
    l3eg.vlan = nniVlan;
    rv = bcm_l3_egress_create(0, 0, &l3eg, &l3egid);
    printf("Egress obj id 0x%08x, Fec entry index:%d\n", (l3egid),(l3egid) & 0x1FFFFFFF);
    printf("LL global lif 0x%x\n", (l3eg.encap_id) & 0x3FFFFFFF);

    /* Add entry for MPLS Termination
        * Termination is done on the packet based on its MPLS IN_LABEL
        */
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    entry.label = inLabel1;
    entry.port = nni_port;
    rv = bcm_mpls_tunnel_switch_create(0, &entry);
    if (rv != BCM_E_NONE) {
       printf("Error, in run bcm_mpls_tunnel_switch_create \n");
       return rv;
    }
    printf("tunnel id 0x%08x, global lif 0x%0x\n", (entry.tunnel_id),(entry.tunnel_id) & 0x3FFFFFF);

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = inpwlabel1;
    mpls_port.egress_tunnel_if = l3egid;
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL|BCM_MPLS_PORT_COUNTED;
    mpls_port.port = nni_port;
    mpls_port.egress_label.label = outpwlabel1;

    mpls_port.egress_label.flags |= (BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_TTL_COPY );

    rv = bcm_mpls_port_add(0, 0, &mpls_port);
    if (rv != BCM_E_NONE) {
       printf("Error, in run bcm_mpls_port_add \n");
       return rv;
    }
    printf("encap id (global lif) 0x%x\n", mpls_port.encap_id);
    printf("mpls_port_id 0x%0x\n", mpls_port.mpls_port_id);

    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = vlan_port_uni.vlan_port_id;
    print gports.port1;
    gports.port2 = mpls_port.mpls_port_id;
    print gports.port2;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
       printf("Error, in run bcm_vswitch_cross_connect_add \n");
       return rv;
    }

    return rv;
}

int mpls_various_scenarios_l2vpn_ler_push_2lbl(int unit, int port1, int port2){
    int rv = BCM_E_NONE;

    rv = mpls_various_scenarios_l2vpn_ler_pop_2lbl(unit, port1, port2);
    if (rv != BCM_E_NONE) {
       printf("Error, in run mpls_various_scenarios_l2vpn_ler_push_2lbl \n");
       return rv;
    }
    return rv;
}

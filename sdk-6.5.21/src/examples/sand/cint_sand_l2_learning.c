/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MACT Learning~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_l2_learning.c
 * Purpose: Example of how to set learning with up to two devices
 *
 * Notes:
 *   cint_sand_l2_learning.c is built up on the top of cint_l2_learning.c
 *   which supports dpp devices only, while this new one supports both dpp and 
 *   dnx devices. When test porting finished, cint_l2_learning can be removed 
 *   if it turn out no dpp specific test needs to use cint_l2_learning.
 * 
 * The devices support automatic (hardware) learning of L2 addresses across devices in the system. 
 * Learning can occur at the ingress or at the egress. With ingress learning (the default), 
 * a device learns the reachable MAC addresses at both the ingress and the egress. With egress 
 * learning, a device learns the reachable MAC addresses at the egress only.
 * Learning can be centralized or distributed. In centralized learning, all MACT events 
 * (Learn, Refresh, Aging) are sent to the CPU, which manages a global MAC table and distributes 
 * the changes to all devices. In distributed learning, each device manages its own local MAC table 
 * event and notifies all other devices of the changes via inband DSP packets.
 *
 * MAC Table Learning Modes:
 * MAC table learning is set by the following:
 * bcm_switch_control_set(unit, bcmSwitchL2LearnMode, BCM_L2_EGRESS_DIST);
 * 
 * Other modes are:
 *  -   BCM_L2_EGRESS_DIST
 *  -   BCM_L2_EGRESS_CENT
 *  -   BCM_L2_EGRESS_INDEPENDENT
 *  -   BCM_L2_INGRESS_DIST
 *  -   BCM_L2_INGRESS_CENT
 *
 * MACT Learning Message Encapsulation
 * To define an encapsulation of learning messages for all learning devices that are monitoring an 
 * Ethernet packet, use the following constructs:
 * bcm_l2_learn_msgs_config_set(unit, learn_msgs);
 *
 * bcm_l2_learn_msgs_config_s{ 
 *     uint32           flags; 
 *     bcm_gport_t      dest_port;     *Destination port*  
 *     bcm_multicast_t  dest_group;    *Destination group* 
 * *L2 encapsulation info*  
 *     bcm_vlan_t vlan;   
 *     uint16 vlan_prio;   
 *     uint16 ether_type;        *Ethernet II frame or 802.1Q*
 *     bcm_mac_t src_mac_addr;   *L2: SA* 
 *     bcm_mac_t dst_mac_addr;   *L2: DA* 
 *
 * } bcm_l2_learn_msgs_config_t 
 * 
 * flags =                                                                                                                               
 *  -   BCM_L2_LEARN_MSG_DEST_MULTICAST- The destination of the learning message is dest_group. 
 *      Otherwise, the destination is dest_port.
 *  -   BCM_L2_LEARN_MSG_ETH_ENCAP- The Learn DSP message is encapsulated with an Ethernet header.                                     
 *  -   BCM_L2_LEARN_MSG_LEARNING- This definition applies DSP learn messages to Learning devices.                                      
 *  -   BCM_L2_LEARN_MSG_SHADOW- This definition applies DSP learn messages to Shadow MACT 
 *      monitoring CPU MAC table aging rate.        
 *
 * CINT Usage:
 *  1.  Open multicast 4500.                                                                                                                
 *  2.  Add to the multicast port 79 of each attached device.                                                                               
 *      Note: Port 79 is an internal port used for sending and receiving learning messages.                                                         
 *  3.  Add a network port to the multicast, so that copies of the learning messages will be sent 
 *      to the multicast ports.                   
 *  4.  Call bcm_l2_learn_msgs_config_set() which will do the following:                                                                    
 *      a.  Configure the destination of learning messages to be multicast 4500.                                                                
 *      b.  Set the Ethernet header (SA,DA, Ether-type) of the learning messages.                                                               
 *  5.  Call bcm_switch_control_set(unit, bcmSwitchL2LearnMode, BCM_L2_EGRESS_DIST).                                                        
 *      a.  Set learning mode to be egress-distributed.                                                                                         
 *  6.  Send an Ethernet packet with unknown SA. This will cause the device to generate learning messages 
 *      and send them over multicast 4500.
 *
 * To Active Above Settings Run:
 *      BCM> cint examples/sand/cint_sand_l2_learning.c
 *      BCM> cint
 *      cint> l2_learning_run_with_defaults(unit, other_unit);
 * Pass other_unit as -1 if the system consists of only 1 device.
 * The first parameter should be the master unit.
 *
 * Scrip Adjustment
 *  You can adjust the following attribute of the application:
 *  -       Number of devices in system, calling l2_learning_run with one/two unit ids.
 *  -       ports[1] = 1;                       *network port to send learning information to*
 *  -       l2_learning_info1.nof_ports = 2;    *1 to send only to devices in the system*
 *                                              *2 to send also to network port*
 *  -       l2_learning_info1.mc_id = 4500;     *multicast group used for learning*
 *  Note: The learning port (for example, ports[0]) must be 79.
 * 
 * 
 *  BCM> examples/sand/utility/cint_sand_utils_multicast.c
 */

int learn_mode = 0;
uint8 _src_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x12, 0x55};
uint8 _dest_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x44, 0x88};

struct l2_learning_info_s {
    /* ports to send learning packets to */
    int ports[10];
    int nof_ports;/* <= 10 */
    /* mc-group to be used for this purpose */
    int mc_id;
};

l2_learning_info_s l2_learning_info1;

int
l2_learning_info_init(int unit, int port){
    bcm_gport_t olp;
    int count, rv = BCM_E_NONE;
    
    rv = l2__olp_port__get(unit, &olp);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_learning_run_single_unit\n");
        return rv;
    }
    
    l2_learning_info1.ports[0] = olp;
    l2_learning_info1.ports[1] = port;
    
    l2_learning_info1.nof_ports = 2;
    l2_learning_info1.mc_id = 4500;
    
    return BCM_E_NONE;
}


/*
 * if unit_2nd >= 0 than works on two units. Otherwise works on a single device
 */
int l2_learning_run(int unit, int unit_2nd){
    int rv;
    rv = l2_learning_run_single_unit(unit, unit_2nd);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_learning_run_single_unit\n");
        return rv;
    }
    
    if (unit_2nd >= 0) {
        rv = l2_learning_run_single_unit(unit_2nd, unit);
        if (rv != BCM_E_NONE) {
            printf("Error, l2_learning_run_single_unit\n");
            return rv;
        }
    }
    return BCM_E_NONE;
}

int l2_learning_run_single_unit(int unit, int other_unit){
    bcm_l2_learn_msgs_config_t learn_msgs;
    int rv;
    int cuds[20] = {0};
    int ports[20] = {0};
    int num_ports;
    int i = 0;
    int is_jericho2 = 0;
    int modid, core, tm_port;
    int count;
    bcm_gport_t olp[BCM_PIPES_MAX];
    /* This parameter is set when doing bring up to a new chip that not yet supporting multicast groups. Set it to 0 when multicast is supported */
    int multicast_supported = 1;

    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE) {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }

    if (!is_jericho2){
        rv = bcm_stk_modid_get(unit, &modid);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_stk_modid_get \n");
            return rv;
        }
    }
    
    num_ports=0;
    for (i = 0; i < l2_learning_info1.nof_ports; ++i) { 
        rv = get_core_and_tm_port_from_port(unit, l2_learning_info1.ports[i], &core, &tm_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in get_core_and_tm_port_from_port\n");
            return rc;
        }
        if (is_jericho2)
        {
            BCM_GPORT_SYSTEM_PORT_ID_SET(ports[num_ports], tm_port);
        }
        else
        {
            BCM_GPORT_MODPORT_SET(ports[num_ports], modid + core, tm_port);
        }

        num_ports++;
    }
    
    if (other_unit >= 0) {
        if (!is_jericho2){
            rv = bcm_stk_modid_get(other_unit, &modid);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_stk_modid_get \n");
                return rv;
            }
        }
        for (i = 0; i < l2_learning_info1.nof_ports; ++i) {
            if (is_jericho2)
            {
                BCM_GPORT_SYSTEM_PORT_ID_SET(ports[num_ports], l2_learning_info1.ports[i]);
            }
            else
            {
                BCM_GPORT_MODPORT_SET(ports[num_ports], modid, l2_learning_info1.ports[i]);
            }
            num_ports++;
        }
    }
    
    /* set distribution of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);

    if (multicast_supported)
    {
        /* open multicast group to be used for distribution of learning messages */
        rv = multicast__open_ingress_mc_group_with_gports(unit, l2_learning_info1.mc_id, ports, cuds, num_ports, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, open_ingress_mc_group_num \n");
            printf("mc_id=%d\n", l2_learning_info1.mc_id);
            printf("ports: \n");
            print ports;
            printf("cuds: \n");
            print cuds;
            printf("num_ports=%d\n", num_ports);
            return rv;
        }
        
        /* the following setting affects learning messages */
        learn_msgs.flags |= BCM_L2_LEARN_MSG_LEARNING | BCM_L2_LEARN_MSG_DEST_MULTICAST;
        learn_msgs.dest_group = l2_learning_info1.mc_id;
    }
    else /* multicast_supported */
    {
        learn_msgs.flags |= BCM_L2_LEARN_MSG_LEARNING;
        learn_msgs.dest_port = 240;
    }

    /* follow attributes set the encapsulation of the learning messages*/
    /* learning message encapsulated with ethernet header*/    
    learn_msgs.flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
    learn_msgs.ether_type = 0x120;
    sal_memcpy(learn_msgs.src_mac_addr, _src_mac_address, 6);
    sal_memcpy(learn_msgs.dst_mac_addr, _dest_mac_address, 6);
    /* default value of learn_msgs.vlan indicates packet is not tagged */
    /*learn_msgs.vlan/tpid/vlan_prio: not relevant as packet is not tagged*/
    rv = bcm_l2_learn_msgs_config_set(unit,&learn_msgs);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_msgs_config_set \n");
        return rv;
    }
    
    if (is_jericho2) 
    {
        rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, BCM_L2_INGRESS_DIST); 
    }
    else
    {
        rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, BCM_L2_EGRESS_DIST); 
    }

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }

    return 0;
  
}

int l2_learning_run_with_defaults(int unit, int other_unit, int port){      
    
    int rv;
    bcm_port_config_t config;
    bcm_pbmp_t ports;

    rv = bcm_port_config_get(unit, &config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_config_get \n");
        return rv;
    }

   

    rv = l2_learning_info_init(unit, port);
    if (rv != BCM_E_NONE) {
        printf("Error, in l2_learning_info_init, port=%, \n", port);
        return rv;
    }
    
    return l2_learning_run(unit, other_unit);
}

int l2_learning_with_dvapi(int unit, int other_unit, int setting){
    learn_mode = setting;
    return l2_learning_run_with_defaults(unit,other_unit);
}

int l2_learning_run_with_port(int unit, int other_unit, int port){
    int rv;
    rv = l2_learning_info_init(unit, port);
    if (rv != BCM_E_NONE) {
        printf("Error, in l2_learning_info_init, port=%, \n", port);
        return rv;
    }
    
    return l2_learning_run(unit, other_unit);
}



int changeMode(int unit, int setting) {
    int rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, setting);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }
}

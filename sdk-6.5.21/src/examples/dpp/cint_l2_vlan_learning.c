/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MACT Vlan Learning:~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_l2_vlan_learning.c
 * Purpose: Example of how to set l2 vlan learning with up to two devices.
 *
 * 
 * More details about Mact Learning can be found in cint_l2_learning.c
 * 
 * Test 1: 
 *  Run:
 *  cd ../../../src/examples/dpp/utility/
 *  cint cint_utils_l2.c
 *  cint cint_utils_multicast.c  
 *  cd -
 *  cd ../../../src/examples/dpp/
 *  cint cint_l2_vlan_learning.c
 *  cint
 *  l2_vlan_learning_run(unit, <port_1>, <port_2>);
 *  learning_mode_set(unit, 0);
 * 
 *  run packet:
 *      ethernet header with DA 33 and any SA
 *      vlan tag with vlan tag id 5
 *  packet's SA will be learned
 *
 *  stop running traffic and wait 1 minute
 *  SA will be deleted from Mac table
 * 
 *  Run:
 *  open_ingress_mc_group_num(0, 4500, 0, 1, <port_1>, <port_2>);
 *
 *  run same packet - this time there will be no learning
 *
 *  add a dynamic entry to Mac table
 *  wait 1 minute
 *  entry will not be deleted from Mac table
 * 
 * 
 * test 2:
 *  Run:
 *  cint ../sand/utility/cint_sand_utils_global.c
 *  cint cint_l2_vlan_learning.c
 *  cint
 *  l2_per_vlan_learning_run(unit, <port_1>, <port_2>);
 * 
 *  run packet 1:
 *      ethernet header with DA 33 and any SA
 *      vlan tag with vlan tag id 33
 *      from <port_1>
 *  packet's SA will be learned
 *  the packet will be sent to OLP
 * 
 *  run packet 2:
 *      ethernet header with DA 44 and any SA
 *      vlan tag with vlan tag id 44
 *      from <port_2>
 *  packet's SA will not be learned
 *  the packet will be sent to CPU
 */
 

/* SA and DA in learning msgs */
uint8 _src_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x12, 0x55};
uint8 _dest_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x44, 0x88};

uint8 _src_mac_address_2[6] = {0x00, 0x00, 0x00, 0x22, 0x12, 0x55};
uint8 _dest_mac_address_2[6] = {0x00, 0x00, 0x00, 0x22, 0x44, 0x88};

/* DA in incoming packets */
uint8 _dest_mac_1[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};
uint8 _dest_mac_2[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44};

struct l2_learning_info_s {
    /* ports to send learning packets to */
    int ports[10];
    int nof_ports;/* <= 10 */
    bcm_gport_t olp_port; /* olp */
    bcm_gport_t cpu_port; /* cpu */
    /* mc-group to be used for this purpose */
    int mc_id;
    bcm_vlan_t vlan;

    bcm_vlan_t vlan_1;
    bcm_vlan_t vlan_2;
};

l2_learning_info_s l2_learning_info;

int outPort = 4;
int outPort_2;

int
l2_learning_out_port_set(int oPort_1, int oPort_2){
    outPort = oPort_1;
    outPort_2 = oPort_2;
    return 0;
}

int
l2_learning_info_init(int unit, int port_1, int port_2){

    int count, rv = BCM_E_NONE;
    bcm_port_config_t port_config;
    bcm_gport_t olp, cpu;

    /* get the olp port */
	l2__olp_port__get(unit, &olp);

    l2_learning_info.olp_port =  olp;

    /* get the cpu port */
    rv = bcm_port_config_get(unit, &port_config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_config_get\n");
        print rv;
        return rv;
    }
    /* get the first cpu port */
    BCM_PBMP_ITER(port_config.cpu, cpu) {
        BCM_GPORT_LOCAL_SET(l2_learning_info.cpu_port, cpu);
        break;
    }

    /* send to 2 physical ports in order to monitor learning */
    l2_learning_info.ports[0] = port_1;
    l2_learning_info.ports[1] = port_2;

    l2_learning_info.nof_ports = 2;
    l2_learning_info.mc_id = 4501;

    l2_learning_info.vlan = 5;

    l2_learning_info.vlan_1 = 33;
    l2_learning_info.vlan_2 = 44;

    return BCM_E_NONE;
}


int 
l2_vlan_learning_run(int unit, int port_1, int port_2){

    bcm_l2_learn_msgs_config_t learn_msgs_mc, learn_msgs_cpu;
    int rv;
    int i;
    bcm_l2_addr_distribute_t distribution;
    int age_seconds;
    bcm_pbmp_t pbmp;
    bcm_l2_addr_t l2addr;

    l2_learning_info_init(unit, port_1, port_2);  
    /* create vlan */
    rv = bcm_vlan_create(unit, l2_learning_info.vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d\n", l2_learning_info.vlan);
        print rv;
        return rv;
    }

    /* add port to vlan */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_1);
    BCM_PBMP_PORT_ADD(pbmp, port_2);
    rv = bcm_vlan_port_add(unit, l2_learning_info.vlan, pbmp, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error, in bcm_vlan_port_add with vlan %d\n", l2_learning_info.vlan);
        print rv;
        return rv;
    }

    /* enter the DA's of the arriving packet and send them to port 4 */
    rv = l2_addr_add(unit, &l2addr, _dest_mac_1, outPort, l2_learning_info.vlan);
    if (BCM_FAILURE(rv)) {
        printf("Error, in l2_addr_add\n");
        return rv;
    }
    /* open multicast group to be used for distribution of learning messages */
    /* at first mc group will contain only olp */
    l2_learning_info.ports[l2_learning_info.nof_ports++] = l2_learning_info.olp_port;
	int cuds[3]={0};
	rv = multicast__open_ingress_mc_group_with_gports(unit, l2_learning_info.mc_id, l2_learning_info.ports, cuds, l2_learning_info.nof_ports, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_ingress_mc_group_with_local_ports \n");
        return rv;
    }

    /* set distribution of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs_mc);
    bcm_l2_learn_msgs_config_t_init(&learn_msgs_cpu);

    /* the following setting affects learning messages */
    learn_msgs_mc.flags = BCM_L2_LEARN_MSG_LEARNING;
	learn_msgs_cpu.flags = BCM_L2_LEARN_MSG_SHADOW;

	learn_msgs_cpu.dest_port = l2_learning_info.cpu_port;

    /* follow attributes set the destination port/group learning messages are distributed to */
    /* the learning messages are sent to MC group, 4500 */
    learn_msgs_mc.flags |= BCM_L2_LEARN_MSG_DEST_MULTICAST;
    learn_msgs_mc.dest_group = l2_learning_info.mc_id;
    
    /* learn_msgs_mc.dest_port  not relevant as BCM_L2_LEARN_MSG_DEST_MULTICAST is present */

    /* follow attributes set the encapsulation of the learning messages*/
    /* learning message encapsulated with ethernet header*/    
    learn_msgs_mc.flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
    learn_msgs_mc.ether_type = 0xab00;
    sal_memcpy(learn_msgs_mc.src_mac_addr, _src_mac_address, 6);
    sal_memcpy(learn_msgs_mc.dst_mac_addr, _dest_mac_address, 6);
    /* default value of learn_msgs_mc.vlan indicates packet is not tagged */
    /*learn_msgs_mc.vlan/tpid/vlan_prio: not relevant as packet is not tagged*/
    rv = bcm_l2_learn_msgs_config_set(unit, &learn_msgs_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_msgs_config_set for multicast \n");
        return rv;
    }

	/* queue for cpu distribution */
	learn_msgs_cpu.flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
    learn_msgs_cpu.ether_type = 0xab00;
    sal_memcpy(learn_msgs_cpu.src_mac_addr, _src_mac_address, 6);
    sal_memcpy(learn_msgs_cpu.dst_mac_addr, _dest_mac_address, 6);

    rv = bcm_l2_learn_msgs_config_set(unit, &learn_msgs_cpu);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_msgs_config_set for cpu \n");
        return rv;
    }

    bcm_l2_addr_distribute_t_init(&distribution);

    /* set learning */
    distribution.vid = l2_learning_info.vlan;
    distribution.flags = BCM_L2_ADDR_DIST_LEARN_EVENT | BCM_L2_ADDR_DIST_STATION_MOVE_EVENT | BCM_L2_ADDR_DIST_AGED_OUT_EVENT |
                             BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER;

    rv = bcm_l2_addr_msg_distribute_set(unit, &distribution);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, bcm_l2_addr_msg_distribute_set \n");
        return rv;
    }

    /* set aging, so there'll be refresh */
    age_seconds = 10;
    rv = bcm_l2_age_timer_set(unit, age_seconds);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, bcm_l2_age_timer_set \n");
        return rv;
    }

    return BCM_E_NONE;
}

int
learning_mode_set(int unit, int distributed){

    int rv, mode;

    if (distributed) {
        /* change learning mode, to be egress distributed, packet will be learned at egress 
         * note that if packet is dropped in ingress then it will not be learned 
         */
        mode = BCM_L2_EGRESS_DIST;
    }
    else {
        /* change learning mode, to be egress centralized */
        mode = BCM_L2_EGRESS_CENT;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, mode);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }

    return BCM_E_NONE;
}

int
l2_addr_add(int unit, bcm_l2_addr_t *l2addr, bcm_mac_t mac, bcm_gport_t port, uint16 vid) {

    int rv;

    bcm_l2_addr_t_init(l2addr,mac,vid);

    l2addr->port = port;
    l2addr->flags = BCM_L2_STATIC;

    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

int 
l2_per_vlan_learning_run(int unit, int port_1, int port_2){

    int rv;
	bcm_pbmp_t pbmp;
    bcm_l2_learn_msgs_config_t learn_msgs;
    bcm_l2_addr_distribute_t distribution;
    int age_seconds;
    bcm_l2_addr_t l2addr_1, l2addr_2;

    l2_learning_info_init(unit, port_1, port_2);


    /* create 2 vlans */
    rv = bcm_vlan_create(unit, l2_learning_info.vlan_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d\n", l2_learning_info.vlan_1);
        print rv;
        return rv;
    }

    rv = bcm_vlan_create(unit, l2_learning_info.vlan_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d\n", l2_learning_info.vlan_2);
        print rv;
        return rv;
    }

    /* add ports to vlans:
     * port_1 to vlan_1 and port_2 to vlan_2
     */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_1);

    rv = bcm_vlan_port_add(unit, l2_learning_info.vlan_1, pbmp, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error, in bcm_vlan_port_add with vlan %d\n", l2_learning_info.vlan_1);
        print rv;
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_2);

    rv = bcm_vlan_port_add(unit, l2_learning_info.vlan_2, pbmp, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error, in bcm_vlan_port_add with vlan %d\n", l2_learning_info.vlan_2);
        print rv;
        return rv;
    }

    /* enter the DA's of the arriving packets and send them to port 4 */
    rv = l2_addr_add(unit, &l2addr_1, _dest_mac_1, outPort, l2_learning_info.vlan_1);
    if (BCM_FAILURE(rv)) {
        printf("Error, in l2_addr_add\n");
        return rv;
    }
    rv = l2_addr_add(unit, &l2addr_2, _dest_mac_2, outPort, l2_learning_info.vlan_2);
    if (BCM_FAILURE(rv)) {
        printf("Error, in l2_addr_add\n");
        return rv;
    }

    /* set learning for vlan_1 - through olp (learn) */

    /* set format of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);
    /* the following setting affects learning messages */
    learn_msgs.flags = BCM_L2_LEARN_MSG_LEARNING;

    /* open multicast group to be used for distribution of learning messages */
	int cuds[1]={0};
	rv = multicast__open_ingress_mc_group_with_gports(unit, l2_learning_info.mc_id, l2_learning_info.ports, cuds, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_ingress_mc_group_with_local_ports \n");
        return rv;
    }

    /* follow attributes set the destination port/group learning messages are distributed to */
    /* the learning messages are sent to MC group, 4500 */
    learn_msgs.flags |= BCM_L2_LEARN_MSG_DEST_MULTICAST;
    learn_msgs.dest_group = l2_learning_info.mc_id;

    /* follow attributes set the encapsulation of the learning messages */
    /* learning message encapsulated with ethernet header */
    learn_msgs.flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
    learn_msgs.ether_type = 0xab00;
    sal_memcpy(learn_msgs.src_mac_addr, _src_mac_address, 6);
    sal_memcpy(learn_msgs.dst_mac_addr, _dest_mac_address, 6);
    /* default value of learn_msgs.vlan indicates packet is not tagged */
    /*learn_msgs.vlan/tpid/vlan_prio: not relevant as packet is not tagged*/
    rv = bcm_l2_learn_msgs_config_set(unit, &learn_msgs);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_msgs_config_set \n");
        return rv;
    }

    /* the following configuration is not needed because it is the same as the default configuration that is used 
     * only the default configuration is not per-vlan 
     */
    /*
    bcm_l2_addr_distribute_t_init(&distribution);
    distribution.vid = l2_learning_info.vlan_1;
    distribution.flags = BCM_L2_ADDR_DIST_LEARN_EVENT | BCM_L2_ADDR_DIST_STATION_MOVE_EVENT | BCM_L2_ADDR_DIST_AGED_OUT_EVENT |
                            BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER;

    rv = bcm_l2_addr_msg_distribute_set(unit, &distribution);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, bcm_l2_addr_msg_distribute_set \n");
        return rv;
    }
    */

    /* set learning for vlan_2 - through cpu (shadow) */
    /* set format of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);
    /* the following setting affects shadow messages */
    learn_msgs.flags = BCM_L2_LEARN_MSG_SHADOW;
    /* follow attributes set the destination port/group shadow messages are distributed to */
    /*learn_msgs.flags |= BCM_L2_LEARN_MSG_DEST_MULTICAST; - not needed as dest is port*/
    learn_msgs.dest_port = l2_learning_info.cpu_port;

    /* follow attributes set the encapsulation of the learning messages */
    /* learning message encapsulated with ethernet header */
    learn_msgs.flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
    learn_msgs.ether_type = 0xab00;
    learn_msgs.tpid = 0x8100;
    learn_msgs.vlan = l2_learning_info.vlan_2;
    sal_memcpy(learn_msgs.src_mac_addr, _src_mac_address_2, 6);
    sal_memcpy(learn_msgs.dst_mac_addr, _dest_mac_address_2, 6);
    /* default value of learn_msgs.vlan indicates packet is not tagged */
    /* learn_msgs.vlan/tpid/vlan_prio: not relevant as packet is not tagged*/
    rv = bcm_l2_learn_msgs_config_set(unit, &learn_msgs);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_msgs_config_set \n");
        return rv;
    }

    /* set learning mode to egress distributed */
    rv = learning_mode_set(unit, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, learning_mode_set \n");
        return rv;
    }

    /* set msgs distribution for vlan_2 - all events (including learn) will be sent to shadow */
    bcm_l2_addr_distribute_t_init(&distribution);
    distribution.vid = l2_learning_info.vlan_2;
    distribution.flags = BCM_L2_ADDR_DIST_LEARN_EVENT | BCM_L2_ADDR_DIST_STATION_MOVE_EVENT | BCM_L2_ADDR_DIST_AGED_OUT_EVENT |
                            BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER;

    rv = bcm_l2_addr_msg_distribute_set(unit, &distribution);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, bcm_l2_addr_msg_distribute_set \n");
        return rv;
    }

    /* set aging, so there'll be refresh */
    age_seconds = 10;
    rv = bcm_l2_age_timer_set(unit, age_seconds);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, bcm_l2_age_timer_set \n");
        return rv;
    }


    return BCM_E_NONE;
}



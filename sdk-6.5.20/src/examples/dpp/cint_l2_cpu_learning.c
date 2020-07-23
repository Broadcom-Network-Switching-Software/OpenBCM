/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_l2_cpu_learning.c
 * Purpose: Example of how to set l2 cpu learning with up to two devices.
 *          Example of how to enable cpu learn and handle learn and aged event.
 * 
 * Test1:
 *  Run: 
 *  cint cint_port_tpid.c 
 *  cint cint_l2_cpu_learning.c
 *  cint
 *  l2_cpu_learning_run(unit, port);
 * 
 *  run packet 1:
 *      ethernet header with DA 33 and any SA
 *      vlan tag with vlan tag id 33
 *      from any port
 * 
 *  run packet 2:
 *      ethernet header with DA 44 and any SA
 *      vlan tag with vlan tag id 44
 *      from any port
 *  
 *  both packets will be sent to CPU
 *
 * Test2:
 * 		BCM> examples/dpp/utility/cint_utils_vlan.c
 *      BCM> examples/dpp/cint_l2_cpu_learning.c
 *      BCM> examples/dpp/cint_l2_mact.c
 *      BCM> m IPT_FORCE_LOCAL_OR_FABRIC FORCE_LOCAL=1 FORCE_FABRIC=0
 *      BCM> cint
 *      cint> print l2_entry_control_set(unit, BCM_L2_INGRESS_CENT|BCM_L2_LEARN_CPU);
 *      cint> print l2_entry_event_handle_register(unit);
 */
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MACT Cpu Learning:~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* SA and DA in learning msgs */
uint8 _src_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x12, 0x55};
uint8 _dest_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x44, 0x88};

/* DA in incoming packets */
uint8 _dest_mac_1[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};
uint8 _dest_mac_2[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44};

struct l2_learning_info_s {
    bcm_gport_t cpu_port; /* cpu */
    bcm_vlan_t vlan_1;
    bcm_vlan_t vlan_2;
    int out_port;
};

l2_learning_info_s l2_learning_info;
int l2_learning_event_counter=0;

int
l2_learning_info_init(int unit, int port){

    int count, rv = BCM_E_NONE;
    bcm_port_config_t port_config;
    bcm_gport_t cpu;

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
    printf("cpu_port = 0x%x\n", l2_learning_info.cpu_port);

    l2_learning_info.vlan_1 = 33;
    l2_learning_info.vlan_2 = 44;
    l2_learning_info.out_port = port;

    return BCM_E_NONE;
}

/* set learning mode to egress_distributed (if distributed == 1) or egress_centralized (otherwise) */
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

/* Configure the DSP messages with vlan in the ethernet header */
int l2_cpu_learning_with_vlan(int unit, int port)
{
    int rv;
    bcm_l2_learn_msgs_config_t learn_msgs;
    bcm_l2_addr_distribute_t distribution;
    int age_seconds;
    bcm_l2_addr_t l2addr_1, l2addr_2;

    l2_learning_info_init(unit, port);

    /* create vlan 33 */
    rv = vlan__init_vlan(unit, l2_learning_info.vlan_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan__init_vlan with vlan#1 %d\n", l2_learning_info.vlan_1);
        print rv;
        return rv;
    }   

    /* enter the DA of the arriving packet and send it to port 4 */
    rv = l2_addr_add(unit, &l2addr_1, _dest_mac_1, l2_learning_info.out_port , l2_learning_info.vlan_1);
    if (BCM_FAILURE(rv)) {
        printf("Error, in l2_addr_add#1\n");
        return rv;
    }

    /* set learning through cpu with vlan in the DSP (shadow) */

    /* set format of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);
    learn_msgs.flags = BCM_L2_LEARN_MSG_SHADOW;
    /* follow attributes set the destination port/group shadow messages are distributed to */
    /*learn_msgs.flags |= BCM_L2_LEARN_MSG_DEST_MULTICAST; - not needed as dest is port*/
    learn_msgs.dest_port = l2_learning_info.cpu_port;

    /* follow attributes set the encapsulation of the learning messages */
    /* learning message encapsulated with ethernet header */
    learn_msgs.flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
    learn_msgs.ether_type = 0xab00;
    learn_msgs.tpid = 0x8100;
    learn_msgs.vlan = l2_learning_info.vlan_1;
    sal_memcpy(learn_msgs.src_mac_addr, _src_mac_address, 6);
    sal_memcpy(learn_msgs.dst_mac_addr, _dest_mac_address, 6);
    /* default value of learn_msgs.vlan indicates packet is not tagged */
    /*learn_msgs.vlan/tpid/vlan_prio: not relevant as packet is not tagged*/
    rv = bcm_l2_learn_msgs_config_set(unit, &learn_msgs);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_msgs_config_set \n");
        return rv;
    }

    /* set learning mode to ingress centralized */
    rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, BCM_L2_INGRESS_CENT);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }

	distribution.vid = l2_learning_info.vlan_1;
    distribution.flags = BCM_L2_ADDR_DIST_LEARN_EVENT | BCM_L2_ADDR_DIST_STATION_MOVE_EVENT | BCM_L2_ADDR_DIST_AGED_OUT_EVENT |
                             BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER;

    rv = bcm_l2_addr_msg_distribute_set(unit, &distribution);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, bcm_l2_addr_msg_distribute_set \n");
        return rv;
    }

    return BCM_E_NONE;
}

int 
l2_cpu_learning_run_olp_to_queue(int unit, int olpDestPort, int outPort){

    int rv;
    bcm_l2_learn_msgs_config_t learn_msgs;
    bcm_l2_addr_distribute_t distribution;
    int age_seconds;
    bcm_l2_addr_t l2addr_1, l2addr_2;
    bcm_gport_t olpDestQueue;

    l2_learning_info_init(unit, outPort);

    /* convert port to tm queue */
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(olpDestQueue, (olpDestPort*8) + 24);
    printf("cpu_port = 0x%x, queue = 0x%x\n", olpDestPort, olpDestQueue);

    /* create vlan - 33 */
    rv = vlan__init_vlan(unit, l2_learning_info.vlan_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan__init_vlan with vlan#1 %d\n", l2_learning_info.vlan_1);
        print rv;
        return rv;
    }

    /* enter the DA's of the arriving packets and send them to port 4 */
    rv = l2_addr_add(unit, &l2addr_1, _dest_mac_1, l2_learning_info.out_port , l2_learning_info.vlan_1);
    if (BCM_FAILURE(rv)) {
        printf("Error, in l2_addr_add#1\n");
        return rv;
    }

    /* set learning through cpu (shadow) */
    /* set format of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);
    learn_msgs.flags = BCM_L2_LEARN_MSG_SHADOW;
    /* set the destination tm queue to shadow messages */
    learn_msgs.dest_port = olpDestQueue;

    /* follow attributes set the encapsulation of the learning messages */
    /* learning message encapsulated with ethernet header */
    learn_msgs.flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
    learn_msgs.ether_type = 0xab00;
    sal_memcpy(learn_msgs.src_mac_addr, _src_mac_address, 6);
    sal_memcpy(learn_msgs.dst_mac_addr, _dest_mac_address, 6);
    rv = bcm_l2_learn_msgs_config_set(unit, &learn_msgs);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_msgs_config_set \n");
        return rv;
    }

    /* set learning mode to egress centralized */
    rv = learning_mode_set(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, learning_mode_set \n");
        return rv;
    }

    bcm_l2_addr_distribute_t_init(&distribution);

    /* set learning */
    distribution.vid = l2_learning_info.vlan_1;
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

int
l2_cpu_learning_run(int unit, int port){

    int rv;
    bcm_l2_learn_msgs_config_t learn_msgs;
    bcm_l2_addr_distribute_t distribution;
    int age_seconds;
    bcm_l2_addr_t l2addr_1, l2addr_2;

    l2_learning_info_init(unit, port);

    /* create 2 vlans - 33, 44 */
    rv = vlan__init_vlan(unit, l2_learning_info.vlan_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan__init_vlan with vlan#1 %d\n", l2_learning_info.vlan_1);
        print rv;
        return rv;
    }

    rv = vlan__init_vlan(unit, l2_learning_info.vlan_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan__init_vlan with vlan#2 %d\n", l2_learning_info.vlan_2);
        print rv;
        return rv;
    }

    /* enter the DA's of the arriving packets and send them to port 4 */
    rv = l2_addr_add(unit, &l2addr_1, _dest_mac_1, l2_learning_info.out_port , l2_learning_info.vlan_1);
    if (BCM_FAILURE(rv)) {
        printf("Error, in l2_addr_add#1\n");
        return rv;
    }

    rv = l2_addr_add(unit, &l2addr_2, _dest_mac_2, l2_learning_info.out_port , l2_learning_info.vlan_2);
    if (BCM_FAILURE(rv)) {
        printf("Error, in l2_addr_add#2\n");
        return rv;
    }

    /* set learning through cpu (shadow) */

    /* set format of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);
    learn_msgs.flags = BCM_L2_LEARN_MSG_SHADOW;
    /* follow attributes set the destination port/group shadow messages are distributed to */
    /*learn_msgs.flags |= BCM_L2_LEARN_MSG_DEST_MULTICAST; - not needed as dest is port*/
    learn_msgs.dest_port = l2_learning_info.cpu_port;

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

    /* set learning mode to egress centralized */
    rv = learning_mode_set(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, learning_mode_set \n");
        return rv;
    }

    bcm_l2_addr_distribute_t_init(&distribution);

    /* set learning */
    distribution.vid = l2_learning_info.vlan_1;
    distribution.flags = BCM_L2_ADDR_DIST_LEARN_EVENT | BCM_L2_ADDR_DIST_STATION_MOVE_EVENT | BCM_L2_ADDR_DIST_AGED_OUT_EVENT |
                             BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER;

    rv = bcm_l2_addr_msg_distribute_set(unit, &distribution);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, bcm_l2_addr_msg_distribute_set \n");
        return rv;
    }

    distribution.vid = l2_learning_info.vlan_2;
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

void l2_entry_event_handle(int unit,
	     bcm_l2_addr_t *l2addr,
	     int operation,
	     void *userdata)
{
    int rv = BCM_E_NONE;

    if (operation == BCM_L2_CALLBACK_LEARN_EVENT) {
        printf("BCM_L2_CALLBACK_LEARN_EVENT handle\n");
        l2_addr_print_entry(l2addr);
        rv = bcm_l2_addr_add(unit, l2addr);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2_addr_add\n");
            return rv;
        }
    } if (operation == BCM_L2_CALLBACK_MOVE_EVENT) {
        printf("BCM_L2_CALLBACK_MOVE_EVENT handle\n");
        l2_addr_print_entry(l2addr);
        l2addr->flags |= BCM_L2_STATIC;  /* This event handler is used by AT_Cint_l2_cpu_learning_move_static and should enter static entries */
        rv = bcm_l2_addr_add(unit, l2addr);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2_addr_add from move event\n");
            return rv;
        }
    } else if (operation == BCM_L2_CALLBACK_AGE_EVENT) {
        printf("BCM_L2_CALLBACK_AGE_EVENT handle\n");
        l2_addr_print_entry(l2addr);
        rv = bcm_l2_addr_delete(unit, l2addr->mac, l2addr->vid);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2_addr_add\n");
            return rv;
        }
    }
    l2_learning_event_counter = l2_learning_event_counter + 1;
}

int l2_entry_control_set(int unit, int flags) {
    int rv = BCM_E_NONE;
    int age_seconds = 60;
    bcm_switch_event_control_t event_control;

    rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_set\n");
        return rv;
    }

    /* Enabling aging to 60 sec */
    rv = bcm_l2_age_timer_set(unit, age_seconds);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_age_timer_set\n");
        return rv;
    }

    /* Deactivate log prints ARAD_INT_IHP_MACTEVENTREADY event */
    /*
    event_control.event_id = 439;
	event_control.index = 0;
	event_control.action = bcmSwitchEventLog; 
    rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_control, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_event_control_set\n");
        return rv;
    }
    */
    return rv;

}

int l2_entry_event_handle_register(int unit) {
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;

    rv = bcm_l2_addr_register(unit,l2_entry_event_handle, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_register \n");
        return rv;
    }

    return rv;
}
int l2_entry_event_handle_unregister(int unit) {
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;

    rv = bcm_l2_addr_unregister(unit,l2_entry_event_handle, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_register \n");
        return rv;
    }

    return rv;
}

int cpu_learning_set(int unit){
    bcm_error_t rv = BCM_E_NONE;
    rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, BCM_L2_INGRESS_CENT
                                | BCM_L2_LEARN_CPU);
    if (rv != BCM_E_NONE) {
        printf ("ERROR: bcm_switch_control_set failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }
    printf ("L2 learn mode set to CPU learning (%s)\n", bcm_errmsg(rv));
    rv = l2_entry_event_handle_register(unit);
    if (rv != BCM_E_NONE) {
        printf ("ERROR: l2_entry_event_handle_register failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }
    printf ("L2 callbacks registered. (%s)\n", bcm_errmsg(rv));
    return rv;
}

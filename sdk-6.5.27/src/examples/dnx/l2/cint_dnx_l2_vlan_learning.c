/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MACT Vlan Learning:~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * File: cint_dnx_l2_vlan_learning.c
 * Purpose: Example of how to set l2 vlan learning with up to two devices.
 *
 * Notes:
 *   cint_dnx_l2_vlan_learning.c is built up on the top of cint_l2_vlan_learning.c
 *
 * More details about Mact Learning can be found in cint_l2_learning.c
 *
 * Test 1:
 * Run:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_multicast.c
  cint ../../../../src/examples/dnx/cint_dnx_l2_vlan_learning.c
  cint
  learning_mode_set(0,0);
  l2_learning_out_port_set(202,203);
  l2_vlan_learning_run(0,200,201);
  exit;
 * Test Scenario - end
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

    /* to store the default learning message configure */
    bcm_l2_learn_msgs_config_t learn_msgs_mc_orig;
    bcm_l2_learn_msgs_config_t learn_msgs_cpu_orig;
};
l2_learning_info_s l2_learning_info;

int outPort = 4;
int outPort_2;

int l2_learning_out_port_set(int oPort_1, int oPort_2) {
    outPort = oPort_1;
    outPort_2 = oPort_2;
    return 0;
}

static int OLP_DEFAULT_CORE = 0;
/* Return the default OLP port */
int l2__olp_port__get(int unit, bcm_gport_t *port) {
    bcm_gport_t olp[BCM_PIPES_MAX];
    int count, rv = BCM_E_NONE;

    rv = bcm_port_internal_get(unit,BCM_PORT_INTERNAL_OLP,BCM_PIPES_MAX,olp,&count);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_internal_get\n");
        return rv;
    }
    if (count < 1) {
        printf("Error, No olp port configured\n");
        return BCM_E_NOF_FOUND;
    }

    *port = olp[OLP_DEFAULT_CORE];
    return rv;
}
int l2_learning_info_init(int unit, int port_1, int port_2) {
    int count;
    bcm_port_config_t port_config;
    bcm_gport_t olp, cpu;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value_in;
    bcm_switch_control_info_t value_out;
    bcm_gport_t logical_port;

    /* get the olp port */
    l2__olp_port__get(unit, &olp);

    l2_learning_info.olp_port =  olp;

    /* get the cpu port */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &port_config), "");
    /* get the first cpu port with out header type = ETH and in header type = INJECTED_2_PP */
    BCM_PBMP_ITER(port_config.cpu, cpu) {
        BCM_GPORT_LOCAL_SET(l2_learning_info.cpu_port, cpu);

        key.index = 2;      /* 1 is in, 2 is out */
        key.type = bcmSwitchPortHeaderType;
        BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_indexed_port_get(unit, l2_learning_info.cpu_port, key, &value_out), "out header type");

        key.index = 1;      /* 1 is in, 2 is out */
        BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_indexed_port_get(unit, l2_learning_info.cpu_port, key, &value_in), "in header type");

        if (value_out.value == BCM_SWITCH_PORT_HEADER_TYPE_ETH && value_in.value == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP) {
            break;
        }
    }

    /* send to 2 physical ports in order to monitor learning */
    l2_learning_info.ports[0] = port_1;
    l2_learning_info.ports[1] = port_2;
    l2_learning_info.nof_ports = 2;
    l2_learning_info.mc_id = 4501;
    l2_learning_info.vlan = 5;
    l2_learning_info.vlan_1 = 33;
    l2_learning_info.vlan_2 = 44;

    /* get learn message configuration */
    bcm_l2_learn_msgs_config_t_init(&l2_learning_info.learn_msgs_mc_orig);
    l2_learning_info.learn_msgs_mc_orig.flags = BCM_L2_LEARN_MSG_LEARNING;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_learn_msgs_config_get(unit, &l2_learning_info.learn_msgs_mc_orig), "for multicast");

    bcm_l2_learn_msgs_config_t_init(&l2_learning_info.learn_msgs_cpu_orig);
    l2_learning_info.learn_msgs_cpu_orig.flags = BCM_L2_LEARN_MSG_SHADOW;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_learn_msgs_config_get(unit, &l2_learning_info.learn_msgs_cpu_orig), "for cpu");

    return BCM_E_NONE;
}

int l2_vlan_learning_run(int unit, int port_1, int port_2) {
    char error_msg[200]={0,};
    int i, port_i;
    int cuds[10]={0};
    int age_seconds, meta_cycle;
    uint32 flags;
    bcm_pbmp_t pbmp;
    bcm_multicast_t mcid;
    bcm_l2_addr_t l2addr;
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t matchInfo;
    bcm_multicast_replication_t rep_array;
    bcm_l2_addr_distribute_t distribution;
    bcm_l2_learn_msgs_config_t learn_msgs_mc, learn_msgs_cpu;
    int aging_profiles_bug = 0;

    bcm_vlan_port_t_init(vlan_port);
    bcm_port_match_info_t_init(&matchInfo);
    bcm_multicast_replication_t_init(rep_array);
    aging_profiles_bug = *(dnxc_data_get(unit, "l2", "feature", "age_out_and_refresh_profile_selection", NULL));

    l2_learning_info_init(unit, port_1, port_2);
    /* create vlan */
    snprintf(error_msg, sizeof(error_msg), "with vlan %d", l2_learning_info.vlan);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, l2_learning_info.vlan), error_msg);

    /* add port to vlan */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_1);
    BCM_PBMP_PORT_ADD(pbmp, port_2);
    snprintf(error_msg, sizeof(error_msg), "with vlan %d", l2_learning_info.vlan);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, l2_learning_info.vlan, pbmp, pbmp), error_msg);

    /*
     * Open multicast group to be used as the default forwarding destination for vsi
     * Add all the ports assigned to vlan to this multicast group
     */
    mcid = l2_learning_info.vlan;
    BCM_IF_ERROR_RETURN_MSG(multicast__open_ingress_mc_group_with_gports(unit, mcid, l2_learning_info.ports, cuds, l2_learning_info.nof_ports, 0), "");

    /* enter the DA's of the arriving packet and send them to port 4 */
    bcm_l2_addr_t_init(&l2addr,_dest_mac_1,l2_learning_info.vlan);
    l2addr.port = outPort;
    l2addr.flags = BCM_L2_STATIC;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2addr), "");

    /* open multicast group to be used for distribution of learning messages */
    /* at first mc group will contain only olp */
    l2_learning_info.ports[l2_learning_info.nof_ports++] = l2_learning_info.olp_port;
    BCM_IF_ERROR_RETURN_MSG(multicast__open_ingress_mc_group_with_gports(unit, l2_learning_info.mc_id, l2_learning_info.ports, cuds, l2_learning_info.nof_ports, 0), "");

    /* set distribution of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs_mc);
    /* the following setting affects learning messages */
    learn_msgs_mc.flags = BCM_L2_LEARN_MSG_LEARNING;
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
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_learn_msgs_config_set(unit, &learn_msgs_mc), "for multicast");

    bcm_l2_learn_msgs_config_t_init(&learn_msgs_cpu);
    learn_msgs_cpu.flags = BCM_L2_LEARN_MSG_SHADOW;
    learn_msgs_cpu.dest_port = l2_learning_info.cpu_port;
    /* queue for cpu distribution */
    learn_msgs_cpu.flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
    learn_msgs_cpu.ether_type = 0xab00;
    sal_memcpy(learn_msgs_cpu.src_mac_addr, _src_mac_address, 6);
    sal_memcpy(learn_msgs_cpu.dst_mac_addr, _dest_mac_address, 6);
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_learn_msgs_config_set(unit, &learn_msgs_cpu), "for cpu");

    bcm_l2_addr_distribute_t_init(&distribution);
    /* set learning */
    distribution.vid = l2_learning_info.vlan;
    distribution.flags = BCM_L2_ADDR_DIST_LEARN_EVENT | BCM_L2_ADDR_DIST_STATION_MOVE_EVENT | BCM_L2_ADDR_DIST_AGED_OUT_EVENT | BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER;
    if (aging_profiles_bug)
    {
        distribution.flags = BCM_L2_ADDR_DIST_LEARN_EVENT | BCM_L2_ADDR_DIST_STATION_MOVE_EVENT | BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER;
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_msg_distribute_set(unit, &distribution), "");

        distribution.flags = BCM_L2_ADDR_DIST_AGED_OUT_EVENT | BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER;
        distribution.vid = -1;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_msg_distribute_set(unit, &distribution), "");

    /* Aging moved to be according to meta cycle and not age seconds. */
    meta_cycle = 1;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_age_timer_meta_cycle_set(unit, meta_cycle), "");

    return BCM_E_NONE;
}

int learning_mode_set(int unit, int distributed) {
    int mode;

    if (distributed) {
        mode = BCM_L2_INGRESS_DIST;
    }
    else {
        mode = BCM_L2_INGRESS_CENT;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit, bcmSwitchL2LearnMode, mode), "");

    return BCM_E_NONE;
}

int l2_per_vlan_learning_run(int unit, int port_1, int port_2) {
    char error_msg[200]={0,};
    bcm_pbmp_t pbmp;
    bcm_vlan_port_t vlan_port;
    bcm_l2_learn_msgs_config_t learn_msgs;
    bcm_l2_addr_distribute_t distribution;
    int age_seconds, meta_cycle;
    bcm_l2_addr_t l2addr_1, l2addr_2;
    int aging_profiles_bug = 0;

    bcm_vlan_port_t_init(vlan_port);
    aging_profiles_bug = *(dnxc_data_get(unit, "l2", "feature", "age_out_and_refresh_profile_selection", NULL));

    l2_learning_info_init(unit, port_1, port_2);

    /* create 2 vlans */
    snprintf(error_msg, sizeof(error_msg), "with vlan %d", l2_learning_info.vlan_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, l2_learning_info.vlan_1), error_msg);

    snprintf(error_msg, sizeof(error_msg), "with vlan %d", l2_learning_info.vlan_2);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, l2_learning_info.vlan_2), error_msg);

    /* add ports to vlans:
     * port_1 to vlan_1 and port_2 to vlan_2
     */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_1);
    snprintf(error_msg, sizeof(error_msg), "with vlan %d", l2_learning_info.vlan_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, l2_learning_info.vlan_1, pbmp, pbmp), error_msg);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_2);
    snprintf(error_msg, sizeof(error_msg), "with vlan %d", l2_learning_info.vlan_2);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, l2_learning_info.vlan_2, pbmp, pbmp), error_msg);

    /* enter the DA's of the arriving packets and send them to port 4 */
    bcm_l2_addr_t_init(&l2addr_1,_dest_mac_1,l2_learning_info.vlan_1);
    l2addr_1.port = outPort;
    l2addr_1.flags = BCM_L2_STATIC;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2addr_1), "");

    bcm_l2_addr_t_init(&l2addr_2,_dest_mac_2,l2_learning_info.vlan_2);
    l2addr_2.port = outPort;
    l2addr_2.flags = BCM_L2_STATIC;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2addr_2), "");

    /* open multicast group to be used for distribution of learning messages */
    int cuds[1]={0};
    BCM_IF_ERROR_RETURN_MSG(multicast__open_ingress_mc_group_with_gports(unit, l2_learning_info.mc_id, l2_learning_info.ports, cuds, 1, 0), "");

    /* set learning for vlan_1 - through olp (learn) */
    /* set format of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);
    /* the following setting affects learning messages */
    learn_msgs.flags = BCM_L2_LEARN_MSG_LEARNING;
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
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_learn_msgs_config_set(unit, &learn_msgs), "for olp");

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
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_learn_msgs_config_set(unit, &learn_msgs), "for cpu");

    /* set learning mode to egress centralized */
    BCM_IF_ERROR_RETURN_MSG(learning_mode_set(unit, 0), "");

    /* set msgs distribution for vlan_2 - all events (including learn) will be sent to shadow */
    bcm_l2_addr_distribute_t_init(&distribution);
    distribution.vid = l2_learning_info.vlan_2;
    distribution.flags = BCM_L2_ADDR_DIST_LEARN_EVENT | BCM_L2_ADDR_DIST_STATION_MOVE_EVENT | BCM_L2_ADDR_DIST_AGED_OUT_EVENT | BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER;
    if (aging_profiles_bug)
    {
        distribution.flags = BCM_L2_ADDR_DIST_LEARN_EVENT | BCM_L2_ADDR_DIST_STATION_MOVE_EVENT | BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER;
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_msg_distribute_set(unit, &distribution), "");

        distribution.vid = -1;
        distribution.flags = BCM_L2_ADDR_DIST_AGED_OUT_EVENT | BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_msg_distribute_set(unit, &distribution), "");

    /* set aging, so there'll be refresh */
    meta_cycle = 1;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_age_timer_meta_cycle_set(unit, meta_cycle), "");

    return BCM_E_NONE;
}

int l2_learning_configuration_restore(int unit) {

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_learn_msgs_config_set(unit, &l2_learning_info.learn_msgs_mc_orig), "for multicast");

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_learn_msgs_config_set(unit, &l2_learning_info.learn_msgs_cpu_orig), "for cpu");

    return BCM_E_NONE;
}

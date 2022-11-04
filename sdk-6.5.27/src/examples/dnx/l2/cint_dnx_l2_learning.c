/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MACT Learning~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * File: cint_dnx_l2_learning.c
 * Purpose: Example of how to set learning with up to two devices
 *
 * Notes:
 * cint_dnx_l2_learning.c is built up on the top of cint_l2_learning.c
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
 *  -   BCM_L2_INGRESS_DIST
 *  -   BCM_L2_INGRESS_CENT
 *  -   BCM_L2_INGRESS_CENT|BCM_L2_LEARN_CPU
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
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_multicast.c
  cint ../../../../src/examples/dnx/cint_dnx_l2_learning.c
  cint
  l2_learning_run_with_port(0,-1,1);
  exit;
 * Test Scenario - end
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

static int OLP_DEFAULT_CORE = 0;
/* Return the default OLP port */
int l2__olp_port__get(int unit, bcm_gport_t *port) {
    bcm_gport_t olp[BCM_PIPES_MAX];
    int count;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_internal_get(unit,BCM_PORT_INTERNAL_OLP,BCM_PIPES_MAX,olp,&count), "");
    if (count < 1) {
        printf("Error, No olp port configured\n");
        return BCM_E_NOF_FOUND;
    }

    *port = olp[OLP_DEFAULT_CORE];
    return BCM_E_NONE;
}

int
l2_learning_info_init(int unit, int port) {
    bcm_gport_t olp;
    int count;

    BCM_IF_ERROR_RETURN_MSG(l2__olp_port__get(unit, &olp), "");

    l2_learning_info1.ports[0] = olp;
    l2_learning_info1.ports[1] = port;
    l2_learning_info1.nof_ports = 2;
    l2_learning_info1.mc_id = 4500;

    return BCM_E_NONE;
}

/*
 * if unit_2nd >= 0 than works on two units. Otherwise works on a single device
 */
int l2_learning_run(int unit, int unit_2nd) {
    BCM_IF_ERROR_RETURN_MSG(l2_learning_run_single_unit(unit, unit_2nd), "");

    if (unit_2nd >= 0) {
        BCM_IF_ERROR_RETURN_MSG(l2_learning_run_single_unit(unit_2nd, unit), "");
    }
    return BCM_E_NONE;
}

int l2_learning_run_single_unit(int unit, int other_unit) {
    bcm_l2_learn_msgs_config_t learn_msgs;
    char error_msg[200]={0,};
    int cuds[20] = {0};
    int ports[20] = {0};
    int num_ports;
    int i = 0;
    int modid, core, tm_port;
    int count;
    bcm_gport_t olp[BCM_PIPES_MAX];
    /* This parameter is set when doing bring up to a new chip that not yet supporting multicast groups. Set it to 0 when multicast is supported */
    int multicast_supported = 1;

    num_ports=0;
    for (i = 0; i < l2_learning_info1.nof_ports; ++i) {
        BCM_IF_ERROR_RETURN_MSG(get_core_and_tm_port_from_port(unit, l2_learning_info1.ports[i], &core, &tm_port), "");
        BCM_GPORT_SYSTEM_PORT_ID_SET(ports[num_ports], tm_port);
        num_ports++;
    }

    if (other_unit >= 0) {
        for (i = 0; i < l2_learning_info1.nof_ports; ++i) {
            BCM_GPORT_SYSTEM_PORT_ID_SET(ports[num_ports], l2_learning_info1.ports[i]);
            num_ports++;
        }
    }

    /* set distribution of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);
    /* open multicast group to be used for distribution of learning messages */
    snprintf(error_msg, sizeof(error_msg), "mc_id=%d, num_ports=%d", l2_learning_info1.mc_id, num_ports);
    BCM_IF_ERROR_RETURN_MSG(multicast__open_ingress_mc_group_with_gports(unit, l2_learning_info1.mc_id, ports, cuds, num_ports, 0), error_msg);

    /* the following setting affects learning messages */
    learn_msgs.flags |= BCM_L2_LEARN_MSG_LEARNING | BCM_L2_LEARN_MSG_DEST_MULTICAST;
    learn_msgs.dest_group = l2_learning_info1.mc_id;
    /* follow attributes set the encapsulation of the learning messages*/
    /* learning message encapsulated with ethernet header*/
    learn_msgs.flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
    learn_msgs.ether_type = 0x120;
    sal_memcpy(learn_msgs.src_mac_addr, _src_mac_address, 6);
    sal_memcpy(learn_msgs.dst_mac_addr, _dest_mac_address, 6);
    /* default value of learn_msgs.vlan indicates packet is not tagged */
    /*learn_msgs.vlan/tpid/vlan_prio: not relevant as packet is not tagged*/
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_learn_msgs_config_set(unit,&learn_msgs), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit, bcmSwitchL2LearnMode, BCM_L2_INGRESS_DIST), "");

    return BCM_E_NONE;
}

int l2_learning_run_with_defaults(int unit, int other_unit, int port) {
    char error_msg[200]={0,};
    bcm_port_config_t config;
    bcm_pbmp_t ports;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &config), "");

    snprintf(error_msg, sizeof(error_msg), "port=%d", port);
    BCM_IF_ERROR_RETURN_MSG(l2_learning_info_init(unit, port), error_msg);

    BCM_IF_ERROR_RETURN_MSG(l2_learning_run(unit, other_unit), "");

    return BCM_E_NONE;
}

int l2_learning_with_dvapi(int unit, int other_unit, int setting) {
    learn_mode = setting;
    return l2_learning_run_with_defaults(unit,other_unit);
}

int l2_learning_run_with_port(int unit, int other_unit, int port) {
    char error_msg[200]={0,};

    snprintf(error_msg, sizeof(error_msg), "port=%d", port);
    BCM_IF_ERROR_RETURN_MSG(l2_learning_info_init(unit, port), error_msg);

    BCM_IF_ERROR_RETURN_MSG(l2_learning_run(unit, other_unit), "");

    return BCM_E_NONE;
}

int changeMode(int unit, int setting) {
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit, bcmSwitchL2LearnMode, setting), "");

    return BCM_E_NONE;
}

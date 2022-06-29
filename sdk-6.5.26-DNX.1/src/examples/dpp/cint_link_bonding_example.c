/*************************************************************************************************************************************************************************************************
 Test scenerio - example of 6 LBGs, 16 modems and 2 modem ports

  Purpose: Example shows how to dynamically add modem port, LBG port and LB control port,
           and setup the Link bonding application in a single device.

    normal port                                                                  modem port               modem port                                                               normal port
      |                      |-----------------------------------------------|       |      LB segments &     |        |--------------------------------------------|                     |
      |                      |               LBG 0 in egress                 |       |      LB control pkt    |        |            LBG 0 in ingress                |                     |
    port13  ---forward to--> | LBG port 20 & LB control port 30 ==> Modem 0  | ==> port15  ----Vlan 16---  port15 ==> | Modem  0 ==> LBG port 20 & control port 30 | ---forward to-->  port13
    port13  ---forward to--> | LBG port 20 & LB control port 31 ==> Modem 1  | ==> port15  ----Vlan  1---  port15 ==> | Modem  1 ==> LBG port 20 & control port 31 | ---forward to-->  port13
    port13  ---forward to--> | LBG port 20 & LB control port 32 ==> Modem 2  | ==> port15  ----Vlan  2---  port15 ==> | Modem  2 ==> LBG port 20 & control port 31 | ---forward to-->  port13
    port13  ---forward to--> | LBG port 20 & LB control port 33 ==> Modem 3  | ==> port15  ----Vlan  3---  port15 ==> | Modem  3 ==> LBG port 20 & control port 31 | ---forward to-->  port13
    port13  ---forward to--> | LBG port 20 & LB control port 34 ==> Modem 4  | ==> port15  ----Vlan  4---  port15 ==> | Modem  4 ==> LBG port 20 & control port 31 | ---forward to-->  port13
    port13  ---forward to--> | LBG port 20 & LB control port 35 ==> Modem 5  | ==> port15  ----Vlan  5---  port15 ==> | Modem  5 ==> LBG port 20 & control port 31 | ---forward to-->  port13
                             |-----------------------------------------------|                                        |--------------------------------------------|
                             |               LBG 1 in egress                 |                                        |            LBG 1 in ingress                |
    port13  ---forward to--> | LBG port 21 & LB control port 36 ==> Modem 6  | ==> port15  ----Vlan  6---  port15 ==> | Modem  6 ==> LBG port 21 & control port 36 | ---forward to-->  port13
    port13  ---forward to--> | LBG port 21 & LB control port 37 ==> Modem 7  | ==> port15  ----Vlan  7---  port15 ==> | Modem  7 ==> LBG port 21 & control port 37 | ---forward to-->  port13
                             |-----------------------------------------------|                                        |--------------------------------------------|
                             |               LBG 2 in egress                 |                                        |            LBG 2 in ingress                |
    port13  ---forward to--> | LBG port 22 & LB control port 38 ==> Modem 8  | ==> port16  ----Vlan  8---  port16 ==> | Modem  8 ==> LBG port 22 & control port 38 | ---forward to-->  port13
    port13  ---forward to--> | LBG port 22 & LB control port 39 ==> Modem 9  | ==> port16  ----Vlan  9---  port16 ==> | Modem  9 ==> LBG port 22 & control port 39 | ---forward to-->  port13
                             |-----------------------------------------------|                                        |--------------------------------------------|
                             |               LBG 3 in egress                 |                                        |            LBG 3 in ingress                |
    port13  ---forward to--> | LBG port 23 & LB control port 40 ==> Modem 10 | ==> port16  ----Vlan 10---  port16 ==> | Modem 10 ==> LBG port 23 & control port 40 | ---forward to-->  port13
    port13  ---forward to--> | LBG port 23 & LB control port 41 ==> Modem 11 | ==> port16  ----Vlan 11---  port16 ==> | Modem 11 ==> LBG port 23 & control port 41 | ---forward to-->  port13
                             |-----------------------------------------------|                                        |--------------------------------------------|
                             |               LBG 4 in egress                 |                                        |            LBG 4 in ingress                |
    port13  ---forward to--> | LBG port 24 & LB control port 42 ==> Modem 12 | ==> port16  ----Vlan 12---  port16 ==> | Modem 12 ==> LBG port 24 & control port 42 | ---forward to-->  port13
    port13  ---forward to--> | LBG port 24 & LB control port 43 ==> Modem 13 | ==> port16  ----Vlan 13---  port16 ==> | Modem 13 ==> LBG port 24 & control port 43 | ---forward to-->  port13
                             |-----------------------------------------------|                                        |--------------------------------------------|
                             |               LBG 5 in egress                 |                                        |            LBG 5 in ingress                |
    port13  ---forward to--> | LBG port 25 & LB control port 44 ==> Modem 14 | ==> port16  ----Vlan 14---  port16 ==> | Modem 14 ==> LBG port 25 & control port 44 | ---forward to-->  port13
    port13  ---forward to--> | LBG port 25 & LB control port 45 ==> Modem 15 | ==> port16  ----Vlan 15---  port16 ==> | Modem 15 ==> LBG port 25 & control port 45 | ---forward to-->  port13
                             |-----------------------------------------------|        |                       |       |--------------------------------------------|
                                                                                      |-----  Loopback  ------|

  Soc Properties:
    config add custom_feature_dynamic_port=1
    config add custom_feature_allow_modifications_during_traffic=1
    config add lb_buffer_size=2
    config add link_bonding_enable=1
    config add custom_feature_lb_num_of_entry=1
    config add custom_feature_lbi_en=1
    config add custom_feature_mac_fifo_start_tx_thrs=10

  How to run:
    cd ../../../../src/examples/dpp
    cint cint_link_bonding.c
    cint cint_link_bonding_example.c
    cint cint_dynamic_port_add_remove.c
    cint validate/cint_test_bcm_dynamic_port_add.c
    c
    print cint_dpp_link_bonding_example(0);
    exit;

  Traffic Test:
    - packet with DMAC 0x20 received on port13, is forwarded to LBG 0 (lbg port 20).
      In egress LB, the packet is segmented into segments.
      In ingress LB, the segments will be reassembled into original packet.
      At last, the packet will be forced forward to port13.
    - packets with DMAC 0x20, 0x21, 022, 0x23, 0x24, 0x25 received on port13, are forwarded to LBG 0(lbg port 20),
      LBG 1(lbg port 21), 2(lbg port 22), 3(lbg port 23), 4(lbg port 24) and 5(lbg port 25) respectively.
    - packets with DMAC 0x30 received on port13, is forward to LB control port 30, which connects to modem 0.
      in egress LB, no segmentation is done for control packets.
      in ingress LB, the packet with speicial TPID 0x8200 will be classified as control packet. control packet bypass the segment reorder.
      At last, the packet will be forced forward to port13.
    - packets with DMAC 0x30-0x45 received on port13, are forwarded to LB control port30-45 respectively.

  Note:
    - When enabling/disabling link bonding group(LBG) and changing speed of LB port,
      traffic is not allowed to enter related egress link bonding group.
    - Modem port, LBG port, LB control port can be configurated using SOC or dynamically add/remove using API
    - Below are SOCs for adding LB control port statically
      - ucode_port_<logical_port>=<interface_type>[<interface_id>][.<channel_num>]:core_<core-id>.<tm-port>
        The physical interface for LB control port, should be defined as a modem port. 
        The channel_num of LB control port should be greater than 0. 
      - port_priorities.<port_num> is used to configure number of priority of the control port, it must be 1.
      - otm_base_q_pair.<port_num> is used to configure base qpair of control port. 
        The 16 qpair for LB control ports should be consecutive. In other words, 
        the difference between the greatest ID and the smallest ID of the control qpair should be smaller than or equal to 16
    - Below are APIs for configuring LB Control traffic
      - Enable/disable LB control traffic by calling API bcm_lb_control_set with bcmLbControlTrafficEnable and arg=1/0
      - Set a special TPID to identify control packet in RX direction by calling API bcm_lb_control_set with bcmLbControlTPIDOfControlPackets
      - Map LB control port to Modem in TX direction by calling API bcm_lb_modem_to_port_map_set with flag BCM_LB_FLAG_CONTROL_TRAFFIC

***************************************************************************************************************************************************************************************************/

int verbose = 1;
char* dbg_prefix = "cint_dpp_link_bonding_example:";

/*
 * Add a Link Bonding modem port
 * - phy_port: phy port
 * - modem_port: modem port
 *
 */
int
cint_lb_modem_port_add (
    int unit,
    bcm_port_t phy_port,
    bcm_port_t modem_port)
{
    int rv = 0;
    uint32 flags = 0;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    printf("args <unit=%d>, <phyPort=%d>,<lbg_port=%d>\n", unit, phy_port, modem_port);

    /* Check if logic port exists */
    rv = bcm_port_get(unit, modem_port, flags, &interface_info, &mapping_info);
    if (rv == BCM_E_NONE) {
        printf("port (%d) exists. Skip add.\n", lbg_port);
        return rv;
    }

    /* Add modem port for Link Bonding */
    printf("Add modem port %d on phy_port %d", modem_port, phy_port);

    flags = BCM_PORT_ADD_MODEM_PORT;
    interface_info.interface = BCM_PORT_IF_XFI;
    interface_info.interface_id = 0;
    interface_info.num_lanes = 1;
    interface_info.phy_port = phy_port;

    mapping_info.tm_port = 0;
    mapping_info.channel = 0;
    mapping_info.core = 0;
    mapping_info.num_priorities = 0;
    rv = bcm_port_add(unit, modem_port, flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("Err(%d) in bcm_port_add with modem_port(%d)\n", modem_port);
        return rv;
    }

    return rv;
}

/*
 * Add a Link Bonding Group port
 * - lbg_id:     Link Bonding Group ID, in range of [0, 5]
 * - phy_port:   phy port
 * - lbg_port:   logic port for specific Link Bonding Group
 *
 */
int
cint_lb_lbg_port_add (
    int unit,
    bcm_lbg_t lbg_id,
    bcm_port_t phy_port,
    bcm_port_t lbg_port)
{
    int rv = 0;
    uint32 flags = 0;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int nof_port_priority = 8;

    bcm_port_t                        sys_port = 0;
    bcm_gport_t                       modport_gport, sysport_gport;

    printf("args <unit=%d>, <lbg_id=%d>,<phyPort=%d>,<lbg_port=%d>\n", unit, lbg_id, phy_port, lbg_port);

    /* Check if logic port exists */
    rv = bcm_port_get(unit, lbg_port, flags, &interface_info, &mapping_info);
    if (rv == BCM_E_NONE) {
        printf("LBG port (%d) exists. Skip add.\n", lbg_port);
        return rv;
    }

    /* Add logic port for Link Bonding Group */
    printf("Add lbg port %d on phy_port %d", lbg_port, phy_port);

    flags = BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID | BCM_PORT_ADD_DONT_PROBE;
    interface_info.interface = BCM_PORT_IF_LBG;
    interface_info.interface_id = lbg_id;
    interface_info.phy_port = phy_port;

    mapping_info.base_q_pair = lbg_id * nof_port_priority;
    mapping_info.tm_port = lbg_port;
    mapping_info.channel = 0;
    mapping_info.core = 0;
    mapping_info.num_priorities = nof_port_priority;
    rv = bcm_port_add(unit, lbg_port, flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("Err(%d) in bcm_port_add with lbg_port(%d)\n", lbg_port);
        return rv;
    }

    /* Add system port for logic port */
    BCM_GPORT_MODPORT_SET(modport_gport, 0, lbg_port);
    sys_port = lbg_port;
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sys_port);
    rv = bcm_stk_sysport_gport_set(unit, sysport_gport, modport_gport);
    if (rv != BCM_E_NONE) {
        printf("Err(%d) in bcm_stk_sysport_gport_set with lbg_port(%d)\n", lbg_port);
        return rv;
    }

    BCM_PBMP_PORT_ADD(lbg_pbmp, lbg_port);

    return rv;
}

/*
 * Add a Link Bonding control port for LB control traffic
 * - phy_port:     phy port, where modem port should be added before
 * - channel:      channel number on that phy port
 * - lb_ctrl_port: logic port for Link Bonding control traffic
 *
 * Note: base_q_pair of 16 LB control ports should be consecutive.
 *       each LB control port has one priority.
 */
int
cint_lb_control_port_add (
    int unit,
    bcm_port_t phy_port,
    uint32     base_q_pair,
    uint32     channel,
    bcm_port_t lb_ctrl_port)
{
    int rv = 0;
    uint32 flags = 0;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int nof_lb_ctrl_port_priority = 1;

    bcm_port_t                        sys_port = 0;
    bcm_gport_t                       modport_gport, sysport_gport;

    printf("args <unit=%d>, <channel=%d>,<phyPort=%d>,<lb_ctrl_port=%d>\n", unit, channel, phy_port, lb_ctrl_port);

    /* Check if logic port exists */
    rv = bcm_port_get(unit, lb_ctrl_port, flags, &interface_info, &mapping_info);
    if (rv == BCM_E_NONE) {
        printf("LBG port (%d) exists. Skip add.\n", lb_ctrl_port);
        return rv;
    }

    /* Add logic port for Link Bonding Group */
    printf("Add lb_ctrl_port %d on phy_port %d", lb_ctrl_port, phy_port);

    flags = BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID | BCM_PORT_ADD_CONFIG_CHANNELIZED;
    interface_info.interface = BCM_PORT_IF_XFI;
    interface_info.interface_id = 0;
    interface_info.phy_port = phy_port;

    mapping_info.base_q_pair = base_q_pair;
    mapping_info.tm_port = lb_ctrl_port;
    mapping_info.channel = channel;
    mapping_info.core = 0;
    mapping_info.num_priorities = nof_lb_ctrl_port_priority;
    rv = bcm_port_add(unit, lb_ctrl_port, flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("Err(%d) in bcm_port_add with lb_ctrl_port(%d)\n", lb_ctrl_port);
        return rv;
    }

    /*Enable Tx on control port*/
    print bcm_port_control_set(0, lb_ctrl_port, bcmPortControlTxEnable, 1);

    /* Add system port for logic port */
    BCM_GPORT_MODPORT_SET(modport_gport, 0, lb_ctrl_port);
    sys_port = lb_ctrl_port;
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sys_port);
    rv = bcm_stk_sysport_gport_set(unit, sysport_gport, modport_gport);
    if (rv != BCM_E_NONE) {
        printf("Err(%d) in bcm_stk_sysport_gport_set with lb_ctrl_port(%d)\n", lb_ctrl_port);
        return rv;
    }

    BCM_PBMP_PORT_ADD(lbg_pbmp, lb_ctrl_port);

    return rv;
}

/*
 * Below function provided an Link bonding example, where 6 Link Bonding group(LBG), 16 modem and 2 modem ports are configured.
 * The mapping of LBG and modem can be found in the head of this file.
 *
 */
int
cint_dpp_link_bonding_example (
    int unit)
{
    int rv = 0;
    int port_temp = 0;
    bcm_pbmp_t pbmp_add;
    int flags = 0;
    bcm_lb_direction_type_t direction;
    char shell_cmd[800];
    bcm_lb_format_type_t lbg_type = bcmLbFormatTypechannelize;
    bcm_lb_segment_mode_t seg_mode = bcmLbSegmentMode128;

    /* Modem port related variables - start */
    int NOF_MODEM_PORT = 2;
    bcm_port_t modem_phy_ports[2] = {2, 3};  /* 0 based */
    bcm_port_t modem_ports[2] = {15, 16};
    /* Modem port related variables - end */

    /* LBG related variables - start */
    int NOF_LBG = 6;
    bcm_lbg_t lbg_id_temp = 0;
    bcm_lbg_t  lbg_ids[6]          = { 0,  1,  2,  3,  4, 5};
    bcm_port_t lbg_ports[6]        = {20, 21, 22, 23, 24, 25};
    bcm_port_t lbg_phy_ports[6]    = {33, 35, 50, 52, 127, 0xFFFFFFFF};
    int        lbg_rates_kbits[6]  = {10000000, 10000000, 10000000, 10000000, 10000000, 10000000};
    /* LBG related variables - end */

    /* LB control port related variables - start*/
    int NOF_LB_CTRL_PORTS = 16;
    bcm_port_t lb_control_ports[16]        = {30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45};
    bcm_port_t lb_control_phy_ports[16]    = { 3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4};
    uint32     lb_control_base_q_pairs[16] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63};
    uint32     lb_control_nif_channel[16]  = { 1,  2,  3,  4,  5,  6,  7,  8,  1,  2,  3,  4,  5,  6,  7,  8};
    int lb_control_rate = 625000;
    /* LB control port related variables - end*/

    /* Modem related variables - start */
    int NOF_MODEM = 16;
    int modem_iter = 0;
    bcm_modem_t modem_id = 0;
    bcm_modem_t modem_id_s[16]             = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15};
    bcm_lbg_t   modem_to_lbg[16]           = { 0,  0,  0,  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5};
    bcm_port_t  modem_to_port[16]          = {15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16};
    bcm_port_t  modem_to_lb_ctrl_port[16]  = {30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45};
    /* Modem related variables - end */

    /* Packet format egress - start */
    uint32 outer_vlan_tpid = 0x8100;
    uint32 lbg_tpid = 0x88a8;
    uint32 lb_ctl_tpid = 0x8200;
    bcm_vlan_t  modem_vlan_s[16] = {16,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15};
    bcm_mac_t   seg_smac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44};
    bcm_mac_t   seg_dmac_s[16] = {
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x30},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x31},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x32},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x33},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x34},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x35},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x36},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x37},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x38},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x39},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x40},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x41},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x42},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x43},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x44},
        {0x00, 0x00, 0x00, 0x00, 0x01, 0x45}};
    int seg_hdr_type;
    /* Packet format egress - end */

    /* Forward Rule related - start */
    bcm_port_t dport_for_rx_data = 13;
    bcm_port_t dport_for_rx_ctrl = 13;
    bcm_vlan_t vlan_lbg_data = 1;
    bcm_mac_t  dmac_data_s[16] = {
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x20},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x21},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x22},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x23},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x24},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x25}};
    bcm_mac_t  dmac_control_s[16] = {
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x30},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x31},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x32},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x33},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x34},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x35},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x36},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x37},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x38},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x39},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x40},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x41},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x42},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x43},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x44},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x45}};
    /* Forward Rule related - end */

    /* Remove logic ports on phy ports which is for modem ports for LB */
    if (verbose == 1) printf("%s Start to Remove logic ports on phy ports which is for modem ports\n", dbg_prefix);
    flags = 0;
    for (port_temp =0; port_temp < NOF_MODEM_PORT; port_temp++) {
        rv = remove_port_full_example(unit, modem_ports[port_temp], flags);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in remove_port_full_example with ports(%d)\n", rv, modem_ports[port_temp]);
            return rv;
        }
    }

    /* Add modem ports for LB */
    if (verbose == 1) printf("%s Start to add modem ports for LB\n", dbg_prefix);
    for (port_temp =0; port_temp < NOF_MODEM_PORT; port_temp++) {
        rv = cint_lb_modem_port_add(unit, modem_phy_ports[port_temp]+1, modem_ports[port_temp]);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in cint_lb_modem_port_add with modem_ports(%d)\n", rv, modem_ports[port_temp]);
            return rv;
        }
    }


    /* Add logic ports for LBG */
    if (verbose == 1) printf("%s Start to add logic ports for LBG\n", dbg_prefix);
    for (port_temp =0; port_temp < NOF_LBG; port_temp++) {
        rv = cint_lb_lbg_port_add(unit, lbg_ids[port_temp], lbg_phy_ports[port_temp], lbg_ports[port_temp]);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in lbg_port_add with lbg_id(%d)\n", rv, lbg_ids[port_temp]);
            return rv;
        }
    }

    /* Add LB control port */
    if (verbose == 1) printf("%s Start to add LB control port\n", dbg_prefix);
    for (port_temp =0; port_temp < NOF_LB_CTRL_PORTS; port_temp++) {
        rv = cint_lb_control_port_add(unit, lb_control_phy_ports[port_temp], lb_control_base_q_pairs[port_temp], lb_control_nif_channel[port_temp], lb_control_ports[port_temp]);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in cint_lb_control_port_add with lb_control_ports(%d)\n", rv, lb_control_ports[port_temp]);
            return rv;
        }
    }

    /* Init port scheduler */
    if (verbose == 1) printf("Start to init port scheduler for LBG\n");
    print cint_port_init(0);

    /* Configure port rate for LBG */
    for (port_temp =0; port_temp < NOF_LBG; port_temp++) {
        if (verbose == 1) printf("%s Start to configure port rate (%d) kbits/sec for LB data port(%d)\n", dbg_prefix, lbg_rates_kbits[port_temp], lbg_ports[port_temp]);
        rv = config_port_rate(unit, lbg_ports[port_temp], lbg_rates_kbits[port_temp], 1);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in lbg_port_add with lbg_id(%d)\n", rv, lbg_ids[port_temp]);
            return rv;
        }
    }

    /* Configure port rate for LB control ports */
    for (port_temp = 0; port_temp < NOF_LB_CTRL_PORTS; port_temp++) {
        BCM_PBMP_CLEAR(pbmp_add);
        BCM_PBMP_PORT_SET(pbmp_add, lb_control_ports[port_temp]);
        if (verbose == 1) printf("%s Start to configure port rate (%d) kbits/sec for LB control port(%d)\n", dbg_prefix, lb_control_rate, lb_control_ports[port_temp]);
        rv = config_ports_rate(unit, pbmp_add, lb_control_rate, BCM_SWITCH_PORT_HEADER_TYPE_ETH, TRUE);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in config_ports_rate with lb control port(%d)\n", rv, lb_control_ports[port_temp]);
            return rv;
        }
    }

    /* Connect modem to LBG */
    flags = 0;
    direction = bcmLbDirectionBoth;
    for (modem_iter = 0; modem_iter < NOF_MODEM; modem_iter++) {
        if (verbose == 1) printf("%s Start to connect modem (%d) to LBG (%d)\n", dbg_prefix, modem_id_s[modem_iter], modem_to_lbg[modem_iter]);
        rv = bcm_lb_modem_to_lbg_map_set(unit, modem_id_s[modem_iter], direction, flags, modem_to_lbg[modem_iter]);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in bcm_lb_modem_to_lbg_map_set with modem_id(%d) lbg_id(%d)\n", rv, modem_id_s[modem_iter], modem_to_lbg[modem_iter]);
            return rv;
        }
    }

    /* Connect a specific modem to a NIF port */
    flags = 0;
    for (modem_iter = 0; modem_iter < NOF_MODEM; modem_iter++) {
        if (verbose == 1) printf("%s Start to connect modem (%d) to modem port (%d)\n", dbg_prefix, modem_id_s[modem_iter], modem_to_port[modem_iter]);
        rv = bcm_lb_modem_to_port_map_set(unit, modem_id_s[modem_iter], flags, modem_to_port[modem_iter]);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in bcm_lb_modem_to_port_map_set with modem_id(%d) NIF port(%d)\n", rv, modem_id_s[modem_iter], modem_to_port[modem_iter]);
            return rv;
        }
    }

    /* (Egress) Connect a Link Bonding control port to a modem */
    flags = BCM_LB_FLAG_CONTROL_TRAFFIC;
    for (modem_iter = 0; modem_iter < NOF_MODEM; modem_iter++) {
        if (verbose == 1) printf("%s Start to connect LB control port (%d) to modem (%d)\n", dbg_prefix, modem_to_lb_ctrl_port[modem_iter], modem_id_s[modem_iter]);
        rv = bcm_lb_modem_to_port_map_set(unit, modem_id_s[modem_iter], flags, modem_to_lb_ctrl_port[modem_iter]);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in bcm_lb_modem_to_port_map_set with modem_id(%d) LB control port(%d)\n", rv, modem_id_s[modem_iter], modem_to_lb_ctrl_port[modem_iter]);
            return rv;
        }
    }

    /* (Egress) Configure packet - global */
    flags = 0;
    bcm_lb_packet_config_t glb_pkt_egr;
    glb_pkt_egr.outer_vlan_tpid = outer_vlan_tpid;
    glb_pkt_egr.lbg_tpid = lbg_tpid;
    if (verbose == 1) printf("%s Start to configure LB packet: outer vlan tpid(%x) lbg tpid(%x)\n", dbg_prefix, outer_vlan_tpid, lbg_tpid);
    rv = bcm_lb_packet_config_set(unit, flags, &glb_pkt_egr);
    if (rv != BCM_E_NONE) {
        printf("Err(%d) in bcm_lb_packet_config_set with outer_vlan_tpid(%d) lbg_tpid(%d)\n", rv, outer_vlan_tpid, lbg_tpid);
        return rv;
    }

    /* (Egress) Configure packet - per modem */
    bcm_lb_modem_packet_config_t mdm_pkt_egr;
    seg_hdr_type = (lbg_type == bcmLbFormatTypechannelize) ? 3 : 2;
    for (modem_iter = 0; modem_iter < NOF_MODEM; modem_iter++) {
        mdm_pkt_egr.pkt_format = lbg_type;
        sal_memcpy(mdm_pkt_egr.dst_mac, seg_dmac_s[modem_iter], 6);
        mdm_pkt_egr.src_mac = seg_smac;
        mdm_pkt_egr.vlan = modem_vlan_s[modem_iter];
        mdm_pkt_egr.use_global_priority_map = 0;
        if (verbose == 1) printf("%s Start to configure LB packet for modem(%d) vlan(%x)\n", dbg_prefix, modem_id_s[modem_iter], modem_vlan_s[modem_iter]);
        rv = bcm_lb_modem_packet_config_set(unit, modem_id_s[modem_iter], flags, &mdm_pkt_egr);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in bcm_lb_modem_packet_config_set with modem_id(%d)\n", rv, modem_id_s[modem_iter]);
            return rv;
        }

        sprintf(shell_cmd, "mod EPNI_LBG_MODEM_CONFIG %d 1 HDR_TYPE=%d", modem_id_s[modem_iter], seg_hdr_type);
        printf("Execute bcm shell command=%s\n", shell_cmd);
        bshell(unit, shell_cmd);
    }

    /* (Egress) Configure segment mode to 128 Byte */
    flags = 0;
    direction = bcmLbDirectionTx;
    if (verbose == 1) {
        printf("%s Start to configure segment mode", dbg_prefix);
        print seg_mode;
    }
    for (lbg_id_temp = 0; lbg_id_temp < NOF_LBG; lbg_id_temp++) {
        rv = bcm_lb_control_set(unit, lbg_ids[lbg_id_temp], direction, flags, bcmLbControlSegmentationMode, seg_mode);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in bcm_lb_control_set with lbg_id seg mode", rv, lbg_ids[lbg_id_temp]);
            print seg_mode;
            return rv;
        }
    }

    /* (Egress) Configure lbg sheduler */
    flags = 0;
    int lbg_count = NOF_LBG;
    bcm_lb_lbg_weight_t lbg_weights[6];
    for (lbg_id_temp = 0; lbg_id_temp < NOF_LBG; lbg_id_temp++) {
        lbg_weights[lbg_id_temp].lbg_id = lbg_ids[lbg_id_temp];
        lbg_weights[lbg_id_temp].lbg_weight = 1;
    }
    if (verbose == 1) printf("%s Start to configure lbg sheduler\n", dbg_prefix);
    rv = bcm_lb_tx_sched_set(unit, flags, lbg_count, lbg_weights);
    if (rv != BCM_E_NONE) {
        printf("Err(%d) in bcm_lb_tx_sched_set\n");
        return rv;
    }

    /* (Egress) Enable LB Data traffic */
    flags = 0;
    for (lbg_id_temp = 0; lbg_id_temp < NOF_LBG; lbg_id_temp++) {
        if (verbose == 1) printf("%s Start to enable LB data traffic for lbg (%d) in egress\n", dbg_prefix, lbg_ids[lbg_id_temp]);
        rv = bcm_lb_enable_set(unit, lbg_ids[lbg_id_temp], flags, TRUE);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in bcm_lb_enable_set with lbg id(%d)\n", rv, lbg_ids[lbg_id_temp]);
            return rv;
        }
    }

    /* (Egress) Enable LB Control traffic */
    flags = BCM_LB_FLAG_GLOBAL;
    direction = bcmLbDirectionTx;
    if (verbose == 1) printf("%s Start to enable LB control traffic in egress\n", dbg_prefix);
    rv = bcm_lb_control_set(unit, 0, direction, flags, bcmLbControlTrafficEnable, TRUE);
    if (rv != BCM_E_NONE) {
        printf("Err(%d) in bcm_lb_control_set with bcmLbControlTrafficEnable\n", rv);
        return rv;
    }


    /* (Ingress) Map Link Bonding data traffic to modem */
    flags = 0;
    bcm_lb_rx_modem_map_index_t map_index;
    bcm_lb_rx_modem_map_config_t map_config;
    for (modem_iter = 0; modem_iter < NOF_MODEM; modem_iter++) {
        map_index.port = modem_to_port[modem_iter];
        map_index.vlan = modem_vlan_s[modem_iter];
        map_config.modem_id = modem_id_s[modem_iter];
        map_config.lbg_type = lbg_type;
        if (verbose == 1) printf("%s Start to map LB data traffic with vlan (%d) on port(%d) to modem(%d)\n", dbg_prefix, modem_vlan_s[modem_iter], modem_to_port[modem_iter], modem_id_s[modem_iter]);
        rv = bcm_lb_rx_modem_map_set(unit, &map_index, flags, &map_config);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in bcm_lb_rx_modem_map_set with port(%d) vlan(%d) modem(%d)\n",
                rv, map_index.port, map_index.vlan, map_config.modem_id);
            return rv;
        }
    }

    for (port_temp = 0; port_temp < NOF_MODEM_PORT; port_temp++) {
        sprintf(shell_cmd, "mod ILB_PORT_CONFIG_TABLE %d 1 ITEM_2_2=1", modem_phy_ports[port_temp]);
        printf("Execute bcm shell command=%s\n", shell_cmd);
        bshell(unit, shell_cmd);
    }

    /* (Ingress) Identify Link Bonding control traffic */
    flags = BCM_LB_FLAG_GLOBAL;
    direction = bcmLbDirectionRx;
    if (verbose == 1) printf("%s Start to configure special TPID(%d) for identifying control traffic in ingress\n", dbg_prefix, lb_ctl_tpid);
    rv = bcm_lb_control_set(unit, 0, direction, flags, bcmLbControlTPIDOfControlPackets, lb_ctl_tpid);
    if (rv != BCM_E_NONE) {
        printf("Err(%d) in bcm_lb_control_set with seg mode(%d)\n", rv, seg_mode);
        return rv;
    }

    /* Forward Rule: force LB data packet received on modem port(LBG ports) forward to dport_for_rx_data */
    if (verbose == 1) printf("%s Start to configure forward Rule for LB data packet received on LBG ports\n", dbg_prefix);
    for (port_temp =0; port_temp < NOF_LBG; port_temp++) {
        rv = bcm_port_force_forward_set(unit, lbg_ports[port_temp], dport_for_rx_data, 1);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in bcm_port_force_forward_set with src port(%d) dst port(%d)\n", rv, lbg_ports[port_temp], dport_for_rx_data);
            return rv;
        }
    }

    /* Forward Rule: force LB control packet received on modem port((LB control ports) forward to dport_for_rx_ctrl */
    if (verbose == 1) printf("%s Start to configure forward Rule for LB control packet received on LB control ports\n", dbg_prefix);
    for (port_temp =0; port_temp < NOF_LB_CTRL_PORTS; port_temp++) {
        rv = bcm_port_force_forward_set(unit, lb_control_ports[port_temp], dport_for_rx_ctrl, 1);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in bcm_lb_control_set with src port(%d) dst port(%d)\n", rv, lb_control_ports[port_temp], dport_for_rx_ctrl);
            return rv;
        }
    }

    /* Forward Rule: forward data packet to LBG ports, base on dmac and vlan */
    if (verbose == 1) printf("%s Start to configure forward Rule for LB data packet to LBG ports\n", dbg_prefix);
    for (port_temp =0; port_temp < NOF_LBG; port_temp++) {
        rv = l2_addr_add(unit, dmac_data_s[port_temp], vlan_lbg_data, lbg_ports[port_temp]);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in l2_addr_add with lbg port(%d) vlan(%d) \n", rv, lbg_ports[port_temp], vlan_lbg_data);
            return rv;
        }
    }

    /* Forward Rule: forward control packet to LB control ports, base on dmac and vlan */
    if (verbose == 1) printf("%s Start to configure forward Rule for LB control packet to LB control ports\n", dbg_prefix);
    for (port_temp =0; port_temp < NOF_LB_CTRL_PORTS; port_temp++) {
        rv = l2_addr_add(unit, dmac_control_s[port_temp], modem_vlan_s[port_temp], lb_control_ports[port_temp]);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in l2_addr_add with lbg port(%d) vlan(%d)\n", rv, lb_control_ports[port_temp], modem_vlan_s[modem_iter]);
            return rv;
        }
    }

    /* Vlan configuration for LBG port */
    if (verbose == 1) printf("%s Start to configure vlan port for LBG port\n", dbg_prefix);
    bcm_pbmp_t lbgpbmp,lbgubmp;
    BCM_PBMP_CLEAR(lbgpbmp);
    BCM_PBMP_CLEAR(lbgubmp);
    for (port_temp =0; port_temp < NOF_LBG; port_temp++) {
        BCM_PBMP_PORT_ADD(lbgpbmp, lbg_ports[port_temp]);
        BCM_PBMP_PORT_ADD(lbgubmp, lbg_ports[port_temp]);
    }
    rv = bcm_vlan_port_add(unit, vlan_lbg_data, lbgpbmp, lbgubmp);
    if (rv != BCM_E_NONE) {
        printf("Err(%d) in bcm_vlan_port_add for lbg data\n", rv);
        return rv;
    }

    /* Vlan configuration for LB control port */
    if (verbose == 1) printf("%s Start to configure vlan port for LB control port\n", dbg_prefix);
    bcm_pbmp_t pbmp,ubmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    for (modem_iter = 0; modem_iter < NOF_MODEM; modem_iter++) {
        BCM_PBMP_PORT_ADD(pbmp, lb_control_ports[modem_iter]);
        BCM_PBMP_PORT_ADD(ubmp, lb_control_ports[modem_iter]);

        rv = bcm_vlan_create(unit, modem_vlan_s[modem_iter]);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("Err(%d) in bcm_vlan_create with vlan(%d)\n", rv, modem_vlan_s[modem_iter]);
            return rv;
        }

        rv = bcm_vlan_port_add(unit, modem_vlan_s[modem_iter], pbmp, ubmp);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in bcm_vlan_port_add with vlan(%d)\n", rv, modem_vlan_s[modem_iter]);
            return rv;
        }
    }

    /* Set loopback on modem ports */
    if (verbose == 1) printf("%s Start to set loopback on modem ports\n", dbg_prefix);
    for (port_temp =0; port_temp < NOF_MODEM_PORT; port_temp++) {
        rv = bcm_port_loopback_set(unit, modem_ports[port_temp], BCM_PORT_LOOPBACK_PHY);
        if (rv != BCM_E_NONE) {
            printf("Err(%d) in bcm_port_loopback_set with port(%d)\n", rv, modem_ports[port_temp]);
            return rv;
        }
    }

    if (verbose == 1) printf("cint_dpp_link_bonding_example finished\n");
    return rv;
}

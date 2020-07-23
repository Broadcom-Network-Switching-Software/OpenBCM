/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : L3 IPv4 route
 *
 *  Usage    : BCM.0> cint l3_ipv4_mc_route_flexctr.c
 *
 *  config   : l3_ipv4_mc_route_flexctr_config.bcm
 *
 *  Log file : l3_ipv4_mc_route_flexctr_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below IPMC APIs:
 *  ========
 *   bcm_ipmc_stat_attach
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate L3 IPv4 mc route traffic configuration.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Create vlan and add port to vlan. Enable L3 feature per port.
 *
 *    2) Step2 - Configuration (Done in l3_route_mc_create())
 *    ======================================================
 *      a) Create egress objects used for multicast route destionation.
 *      b) Create a route.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify the result after send a packet.
 *      b) Expected Result:
 *         Packet raw data (64):
 *      0000: 0100 5e00 0102 0000 0000 0002 0800 4500
 *      0010: 002e 0000 4000 0a06 8dc1 0101 0101 e000
 *      0020: 0107 8181 5151 b1b2 b3b4 a1a2 a3a4 500f
 *      0030: 01f5 e3dd 0000 1112 1314 1516 1718 191a
 */
cint_reset();
int unit = 0;
bcm_port_t ingress_port, egress_port;

bcm_vlan_t vlan_in = 2, vlan_out = 3;
bcm_vrf_t vrf = 1;
bcm_mac_t router_mac_in = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
bcm_mac_t router_mac_out = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
bcm_pbmp_t pbmp, ubmp;
bcm_flexctr_action_t action = {0};
bcm_flexctr_index_operation_t *index_op = NULL;
bcm_flexctr_value_operation_t *value_a_op = NULL;
bcm_flexctr_value_operation_t *value_b_op = NULL;
uint32_t stat_counter_id;
int options = 0;
uint32 num_entries = 1;
uint32 counter_index = 0;
bcm_flexctr_counter_value_t counter_value;
int rv;

/*
 * This function checked if given port/index is already present
 * in the list so that same ports number is not generated again
*/
int checkPortAssigned(int *port_index_list,int no_entries, int index)
{
    int i=0;

    for (i= 0; i < no_entries; i++) {
        if (port_index_list[i] == index)
            return 1;
    }

    /* no entry found */
    return 0;
}

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
*/
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0,rv = 0, index = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;
    int tempports[BCM_PBMP_PORT_MAX];
    int port_index_list[num_ports];

    rv = bcm_port_config_get(unit,&configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i)) {
            tempports[port] = i;
            port++;
        }
    }

    if (( port == 0 ) || ( port < num_ports )) {
        printf("portNumbersGet() failed \n");
        return -1;
    }

    /* generating random ports */
    for (i= 0; i < num_ports; i++) {
        index = sal_rand()% port;
        if (checkPortAssigned(port_index_list, i, index) == 0)
        {
            port_list[i] = tempports[index];
            port_index_list[i] = index;
        } else {
            i = i - 1;
        }
    }

    return BCM_E_NONE;
}

/*
 * This function is pre-test setup.
 */
void test_setup()
{
    int port_list[2];
    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }
    ingress_port = port_list[0];
    egress_port = port_list[1];
    printf("Ingress port: %d\n",ingress_port);
    printf("Egress  port: %d\n",egress_port);

    /* Create ingress vlan and add port to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, 0); /* Added CPU port to same vlan. */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vlan_in));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan_in, pbmp, ubmp));

    /* Create egress vlan and add port to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    BCM_PBMP_PORT_ADD(pbmp, 0); /* Added CPU port to same vlan. */
    print bcm_vlan_create(unit, vlan_out);
    print bcm_vlan_port_add(unit, vlan_out, pbmp, ubmp);

    /* Set the default VLAN for the port */
    print bcm_port_untagged_vlan_set(unit, ingress_port, vlan_in);
    print bcm_port_untagged_vlan_set(unit, egress_port, vlan_out);
}

void l3_route_mc_create()
{
    /* Create L3 interface */
    bcm_l3_intf_t l3_intf_in, l3_intf_out;
    bcm_l3_intf_t_init(&l3_intf_in);
    sal_memcpy(l3_intf_in.l3a_mac_addr, router_mac_in, sizeof(router_mac_in));
    l3_intf_in.l3a_vid = vlan_in;
    print bcm_l3_intf_create(unit, &l3_intf_in);

    bcm_l3_intf_t_init(&l3_intf_out);
    sal_memcpy(l3_intf_out.l3a_mac_addr, router_mac_out, sizeof(router_mac_out));
    l3_intf_out.l3a_vid = vlan_out;
    print bcm_l3_intf_create(unit, &l3_intf_out);

    /* Create L3 ingress interface */
    bcm_l3_ingress_t l3_ingress;
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
    bcm_if_t ingress_if = 100;
    l3_ingress.vrf = vrf;
    l3_ingress.ipmc_intf_id = ingress_if;
    print bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if);

    /* Config vlan_id to l3_iif mapping */
    bcm_vlan_control_vlan_t vlan_ctrl;
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    bcm_vlan_control_vlan_get(unit, vlan_in, &vlan_ctrl);
    vlan_ctrl.ingress_if = ingress_if;
    print bcm_vlan_control_vlan_set(unit, vlan_in, vlan_ctrl);

    /* Create L3 egress object */
    bcm_l3_egress_t l3_egress;
    bcm_if_t egr_obj_id;
    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.port = egress_port;
    l3_egress.intf = l3_intf_out.l3a_intf_id;
    print bcm_l3_egress_create(unit, BCM_L3_IPMC | BCM_L3_KEEP_DSTMAC, &l3_egress, &egr_obj_id);
    print egr_obj_id;

    /* Create the multicast group. */
    uint32_t flags = BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID;
    bcm_multicast_t group;
    BCM_MULTICAST_L3_SET(group, 1);
    print bcm_multicast_create(unit, flags, &group);

    bcm_gport_t gegress_port, gport_cpu;
    bcm_if_t encap_id;
    print bcm_port_gport_get(unit, egress_port, &gegress_port);

    print bcm_multicast_egress_object_encap_get(unit, group, egr_obj_id, &encap_id);
    print bcm_multicast_egress_add(unit, group, gegress_port, encap_id);

    /* Create an IPv4 host SG entry. */
    bcm_ipmc_addr_t data;
    bcm_ipmc_addr_t_init(&data);
    data.s_ip_addr = 0x01010101;
    data.mc_ip_addr = 0xe0000107;
    data.mc_ip_mask = 0xFFFFFFFF;
    data.vrf = vrf;
    data.mtu = 1500;
    data.group = group;
    data.ing_intf = ingress_if;
    data.flags = BCM_IPMC_SOURCE_PORT_NOCHECK;
    print bcm_ipmc_add(unit, &data);

    action.index_num = 1;
    action.source = bcmFlexctrSourceIpmc;
    action.hint = 0;
    action.mode = bcmFlexctrCounterModeNormal;

    /* Counter index is always zero. */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectConstZero;
    index_op->mask_size[0] = 16;
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectConstZero;
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;

    /* Increase counter per packet. */
    value_a_op = &action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne;
    value_a_op->mask_size[0] = 16;
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;
    value_a_op->mask_size[1] = 1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeInc;

    /* Increase counter per packet bytes. */
    value_b_op = &action.operation_b;
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;

    rv = bcm_flexctr_action_create(unit, options, &action, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_ipmc_stat_attach(unit, &data, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_ipmc_stat_attach() - %d : %s.\n", rv, bcm_errmsg(rv));
    }
}

/*
 * This function does the following.
 * Transmit the below packet on ingress_port and verify
 */
void verify()
{
    char   str[512];
    uint64 in_pkt, out_pkt;

    /* Set ingress_port to mac loopback */
    print bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_MAC);
    print bcm_port_loopback_set(unit, egress_port, BCM_PORT_LOOPBACK_MAC);

    /* Trap loopbacked packet on egress_port to CPU */
    print bcm_port_learn_set(unit, egress_port, BCM_PORT_LEARN_CPU);

    /* Start traffic. */
    bshell(unit, "pw start");
    bshell(unit, "pw report all");

    /* Send pacekt to ingress_port */

    snprintf(str, 512, "tx 1 pbm=%d data=01005e00010200010203040508004500002e000040000a068dc101010101e000010781815151b1b2b3b4a1a2a3a4500f01f5e3dd00001112131415161718191a;sleep 5", ingress_port);
    printf("%s\n", str);
    bshell(unit, str);
    bshell(unit, "show c");

    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit,
                              stat_counter_id,
                              num_entries,
                              &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
        return rv;
    }
    print counter_value;
}

test_setup();
l3_route_mc_create();
verify();

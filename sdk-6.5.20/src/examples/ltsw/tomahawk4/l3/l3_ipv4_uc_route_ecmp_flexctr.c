/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : L3 IPv4 route
 *
 *  Usage    : BCM.0> cint l3_ipv4_route.c
 *
 *  config   : l3_ipv4_route_config.bcm
 *
 *  Log file : l3_ipv4_route_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below L3 APIs:
 *  ========
 *   bcm_l3_route_add
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate L3 IPv4 route traffic configuration.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Create vlan and add port to vlan. Enable L3 feature per port.
 *
 *    2) Step2 - Configuration (Done in l3_route_uc_create())
 *    ======================================================
 *      a) Create egress objects used for unicast route destionation.
 *      b) Create a route.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify the result after sens a packet.
 *      b) Expected Result:
 *         Packet raw data (82):
 *      0000: 0000 0000 fe02 0000 0000 0002 8100 000c
 *      0010: 0800 4500 003c 6762 0000 fe01 be76 0a3a
 *      0020: 4001 0a3a 4273 0000 550a 0001 0051 6162
 *      0030: 6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *      0040: 7374 7576 7761 6263 6465 6667 6869
 */

cint_reset();

int unit = 0;
bcm_port_t port_in = 1, port_out = 2, port_out2 = 3;
bcm_vlan_t vlan_in = 11, vlan_out = 12, vlan_out2 = 13;
bcm_vrf_t vrf = 1;
bcm_mac_t router_mac_in = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
bcm_mac_t router_mac_out = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
bcm_mac_t router_mac_out2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x03};
bcm_mac_t dst_mac = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x02};
bcm_mac_t dst_mac2 = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x03};
bcm_ip_t dip = 0x0a3a4273;
bcm_ip_t mask = 0xffffff00;
bcm_ip_t mask2 = 0xfffff000;
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

void test_setup()
{
    /* Enable IPv4/6 on PORT */
    print bcm_port_control_set(unit, port_in, bcmPortControlIP4, 1);
    /* Create ingress vlan and add port to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_in);
    print bcm_vlan_create(unit, vlan_in);
    print bcm_vlan_port_add(unit, vlan_in, pbmp, ubmp);

    /* Create egress vlan and add port to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_out);
    BCM_PBMP_PORT_ADD(pbmp, 0); /* Added CPU port to same vlan. */
    print bcm_vlan_create(unit, vlan_out);
    print bcm_vlan_port_add(unit, vlan_out, pbmp, ubmp);

    /* Create egress vlan2 and add port to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_out2);
    BCM_PBMP_PORT_ADD(pbmp, 0); /* Added CPU port to same vlan. */
    print bcm_vlan_create(unit, vlan_out2);
    print bcm_vlan_port_add(unit, vlan_out2, pbmp, ubmp);

    /* Set the default VLAN for the port */
    print bcm_port_untagged_vlan_set(unit, port_in, vlan_in);
    print bcm_port_untagged_vlan_set(unit, port_out, vlan_out);
    print bcm_port_untagged_vlan_set(unit, port_out2, vlan_out2);
}

void l3_route_uc_create()
{
    /* Create L3 interface */
    bcm_l3_intf_t l3_intf_in, l3_intf_out, l3_intf_out2;

    bcm_l3_intf_t_init(&l3_intf_in);
    sal_memcpy(l3_intf_in.l3a_mac_addr, router_mac_in, sizeof(router_mac_in));
    l3_intf_in.l3a_vid = vlan_in;
    print bcm_l3_intf_create(unit, &l3_intf_in);

    bcm_l3_intf_t_init(&l3_intf_out);
    sal_memcpy(l3_intf_out.l3a_mac_addr, router_mac_out, sizeof(router_mac_out));
    l3_intf_out.l3a_vid = vlan_out;
    print bcm_l3_intf_create(unit, &l3_intf_out);

    bcm_l3_intf_t_init(&l3_intf_out2);
    sal_memcpy(l3_intf_out2.l3a_mac_addr, router_mac_out2, sizeof(router_mac_out2));
    l3_intf_out2.l3a_vid = vlan_out2;
    print bcm_l3_intf_create(unit, &l3_intf_out2);

    /* Create L3 ingress interface */
    bcm_l3_ingress_t l3_ingress;
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
    bcm_if_t ingress_if = 0x1b2;
    l3_ingress.vrf = vrf;
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
    sal_memcpy(l3_egress.mac_addr, dst_mac, sizeof(dst_mac));
    l3_egress.port = port_out;
    l3_egress.intf = l3_intf_out.l3a_intf_id;
    print bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_id);
    print egr_obj_id;

    bcm_if_t egr_obj_id2;
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, dst_mac2, sizeof(dst_mac2));
    l3_egress.intf = l3_intf_out2.l3a_intf_id;
    l3_egress.port = port_out2;
    l3_egress.mtu = 1500;
    print bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_id2);
    print egr_obj_id2;

    /* Create ECMP Object */
    int ecmp_member_count = 2;
    bcm_l3_ecmp_member_t ecmp_member_array[2];
    bcm_l3_egress_ecmp_t ecmp_grp;

    bcm_l3_ecmp_member_t_init(&ecmp_member_array[0]);
    ecmp_member_array[0].egress_if = egr_obj_id2;
    bcm_l3_ecmp_member_t_init(&ecmp_member_array[1]);
    ecmp_member_array[1].egress_if = egr_obj_id;

    bcm_l3_egress_ecmp_t_init(&ecmp_grp);
    ecmp_grp.max_paths = 2;

    print bcm_l3_ecmp_create(unit, 0, &ecmp_grp, ecmp_member_count, ecmp_member_array);

    int ecmp_obj_id = ecmp_grp.ecmp_intf;
    print ecmp_obj_id;

    /* Install the route for DIP */
    bcm_l3_route_t route_info;
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_intf = ecmp_obj_id;
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask2;
    route_info.l3a_vrf = vrf;
    print bcm_l3_route_add(unit, &route_info);

    /* Config my station */
    bcm_l2_station_t l2_station;
    int station_id;
    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, router_mac_in, sizeof(router_mac_in));
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    l2_station.flags = BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6;
    print bcm_l2_station_add(unit, &station_id, &l2_station);

    action.index_num = 1;
    action.source = bcmFlexctrSourceEcmp;
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

    rv = bcm_l3_ecmp_stat_attach(unit, ecmp_obj_id, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_l3_ecmp_stat_attach() - %d : %s.\n", rv, bcm_errmsg(rv));
    }
/*
    rv = bcm_l3_intf_stat_attach(unit, l3_intf_out.l3a_intf_id, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_l3_intf_stat_attach() - %d : %s.\n", rv, bcm_errmsg(rv));
    }

    rv = bcm_l3_intf_stat_attach(unit, l3_intf_out2.l3a_intf_id, stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("ERROR: bcm_l3_intf_stat_attach() - %d : %s.\n", rv, bcm_errmsg(rv));
    }
*/
}
void verify()
{
    /* Traffic */
    bshell(unit, "pw start");
    bshell(unit, "pw report all");

    /* Set port_in to mac loopback */
    print bcm_port_loopback_set(unit, port_in, BCM_PORT_LOOPBACK_MAC);
    print bcm_port_loopback_set(unit, port_out, BCM_PORT_LOOPBACK_MAC);
    print bcm_port_loopback_set(unit, port_out2, BCM_PORT_LOOPBACK_MAC);

    /* Added a L2 address to trap loopbacked packet on port_out to CPU */
    bcm_l2_addr_t addr;
    bcm_l2_addr_t_init(&addr, dst_mac, vlan_out);
    addr.port=0;
    addr.flags |= BCM_L2_STATIC;
    print bcm_l2_addr_add(unit, &addr);

    /* Send pacekt to port_in */
    bshell(unit, "tx 1 pbm=cd0 data=0x000000000001002a1077770008004500003c67620000ff01bd760a3a40010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869");

    bshell(unit, "sleep 5");
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
l3_route_uc_create();
verify();

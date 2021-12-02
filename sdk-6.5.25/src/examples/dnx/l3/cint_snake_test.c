/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

/***************************
 * L3 Snake Functions
 **************************/

/*
 * Function:
 *      cint_l3_snake
 * Purpose:
 *      Configure l3 snake between given ports.
 *      
 * Test Scenario:
 *      L3 snake between ports.
 *
 * Parameters:
 *      unit            - unit #
 *      ports           - ports to snake between. First port must be the port connected to the traffic generator.
 *      nof_ports       - # of ports to snake between.
 *
 * Packet Configuration:
 *      The incoming packets should be configured as followed:
 *      DA = a0:00:00:00:00:e6
 *      SA = a0:00:00:00:00:e6
 *      vlan = 100 + ports[0]
 *      DIP = 32.54.67.124
 *      SIP = 2.1.1.1
 *
 * Cints:
 *      cint src/examples/sand/utility/cint_sand_utils_global.c
 *      cint src/examples/dnx/l3/cint_snake_test.c
 */
int cint_l3_snake(int unit, bcm_port_t *ports, int nof_ports) {
    int rc, rv=BCM_E_NONE;
    int i;  
    int egress_port;

    bcm_mac_t mac_l3_ingress;
    bcm_mac_t mac_l3_ingress100 = {0xa0, 0x0, 0x0, 0x0, 0x0, 0xe6};
    bcm_mac_t mac_l3_egress;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    int host = 0x2036437C;        /* 32.54.67.124 */
    int route;
    bcm_vlan_t vid[nof_ports];
    bcm_vlan_t vlan;
    uint32 is_kbp_lookup;

    bcm_l3_intf_t l3if[nof_ports];
    int ingress_intf[nof_ports];
    int egress_intf[nof_ports];

    for (i = 0; i < nof_ports; i++) { 
        vid[i] = 100 + ports[i];
        egress_port = ports[i];

        mac_l3_ingress = mac_l3_ingress100;
        mac_l3_egress = mac_l3_ingress100;
        vlan = vid[i];        

        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, ports[i]);
        rc = bcm_vlan_port_add (unit, vid[i], pbmp, ubmp);
        if (BCM_FAILURE(rc)) {
            printf("Error, bcm_l3_intf_create, rc = %d\n", rc);
        }

        bcm_l3_intf_t_init(&(l3if[i]));
        sal_memcpy(l3if[i].l3a_mac_addr, mac_l3_ingress, 6);
        l3if[i].l3a_vrf = vid[i];
        l3if[i].l3a_vid = vid[i];
        rc = bcm_l3_intf_create(unit, l3if[i]); 
        if (BCM_FAILURE(rc)) {
            printf("Error, bcm_l3_intf_create, rc = %d\n", rc);
        }
        ingress_intf[i] = l3if[i].l3a_intf_id;

        bcm_l3_egress_t l3eg;
        bcm_if_t l3egid;
		bcm_l3_egress_t_init(&l3eg);
        l3eg.mac_addr = mac_l3_egress;
        rc = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid);
        if (BCM_FAILURE(rc)) {
            printf("Error, create egress object, unit=%d, \n", unit);
            return rc;
        }

        l3eg.port = egress_port;
        l3eg.intf = ingress_intf[i];
        rc = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &l3egid);
        if (BCM_FAILURE(rc)) {
            printf("Error, create egress object, unit=%d, \n", unit);
            return rc;
        }

        egress_intf[i] = l3egid;
    }

    int mask = 0xFFFFFFFF << 8;
    for (i = 0; i < nof_ports; i++) { 
        bcm_l3_route_t l3rt;
        bcm_l3_route_t_init(&l3rt);

        route = host & mask;

        is_kbp_lookup = *(dnxc_data_get(unit, "elk", "application", "ipv4", NULL)) && *(dnxc_data_get(unit, "elk", "application", "split_rpf", NULL));;
        if (is_kbp_lookup)
        {
            l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
        }
        l3rt.l3a_subnet = route;
        l3rt.l3a_ip_mask = mask;
        l3rt.l3a_vrf = vid[i];
        l3rt.l3a_intf = egress_intf[(i+1)%nof_ports];

        rc = bcm_l3_route_add(unit, &l3rt);
        if (BCM_FAILURE(rc)) {
            printf ("bcm_l3_route_add failed: %d \n", rc);
        }
    }

    /* Loopback all ports except for the first one (which is connected to ixia) */
    for (i = 1; i < nof_ports; ++i) {
        print bcm_port_loopback_set(unit, ports[i], BCM_PORT_LOOPBACK_MAC);
    }

    printf("cint_l3_snake: PASS\n\n");
    return rv;
}


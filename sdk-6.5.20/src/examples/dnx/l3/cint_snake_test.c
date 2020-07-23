/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Function:
 *      cint_full_capacity_snake_test
 * Purpose:
 *      Configure snake test between device ports in order to get 720G through the device.
 *      
 * Test Scenario:
 *      This cint assuming 6 CAUI ports and 12 XE ports. The in_port is a XE port which is connected to a traffic generator which generate 10G traffic.
 *      1) The test send traffic from in_port to the first CAUI port.
 *      2) Each CAUI port send traffic to itself 10 times, so it reaches a total of 100G traffic that pass through it.
 *      3) After 10 times, each CAUI port forward traffic to the next CAUI port.
 *      4) The last CAUI port forward traffic to the first XE port.
 *      5) Each XE port forward traffic to the next XE port.
 *      6) The last XE port forward the traffic back to in_port.
 *      So all in all we got 10G that passes 10*(6 CAUI) + (12 XE) = 720G
 *
 *      XE (in_port) --> CAUI --> CAUI --> CAUI --> CAUI --> CAUI --> CAUI --> XE --> XE --> XE --> XE --> XE --> XE --> XE --> XE --> XE --> XE --> XE --> in_port
 *                       |---   EVERY CAUI PORT IS LOOPED 10 TIMES    ---|
 *                       -------------------------------------------------
 *
 * Parameters:
 *      unit            - unit #
 *      mac_address     - mac_address
 *      base_vid        - first vlan id
 *      in_port         - incomming traffic port
 *      caui_ports      - array of 6 caui ports
 *      nof_caui_ports  - number of caui ports
 *      xe_ports        - array of 11 xe ports
 *      nof_xe_ports    - number of xe ports
 *
 * Cints:
 *      cint src/examples/sand/utility/cint_sand_utils_global.c
 *      cint src/examples/dnx/l3/cint_snake_test.c
 */
int cint_full_capacity_snake_test(int unit, bcm_mac_t mac_address, bcm_vlan_t base_vid, bcm_port_t in_port, bcm_port_t *caui_ports, int nof_caui_ports, bcm_port_t *xe_ports, int nof_xe_ports)
{
    int rv;

    bcm_gport_t sys_port;
    bcm_vlan_t  curr_vid, next_vid;
    int         i, port_idx;
    bcm_gport_t voq_gport;
    bcm_cos_t   cosq;

    /* snake between caui ports */
    next_vid = base_vid;
    for (port_idx = 0; port_idx < nof_caui_ports; ++port_idx) {
        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port, caui_ports[port_idx]);
        /* run snake 10 times on each caui port */
        for (i = 0; i < 10; ++i) {
            curr_vid = next_vid;
            next_vid = curr_vid + 1;

            rv = egr_vlan_edit(unit, caui_ports[port_idx], caui_ports[port_idx], curr_vid, next_vid);
            if (BCM_FAILURE(rv)) {
                printf("egr_vlan_edit failed: port %d, curr_vid %d, next_vid %d.\n", caui_ports[port_idx], curr_vid, next_vid);
                return rv;
            }

            rv = l2_addr_add(unit, mac_address, curr_vid, sys_port, 0);     
            if (BCM_FAILURE(rv)) {
                printf("l2_addr_add failed: vid %d.\n", curr_vid);
                return rv;
            }
        }
    }

    /* snake between xe ports */
    for (port_idx = 0; port_idx < nof_xe_ports; ++port_idx) {
        curr_vid = next_vid;
        next_vid = curr_vid + 1;
        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port, xe_ports[port_idx]);

        rv = egr_vlan_edit(unit, xe_ports[port_idx], xe_ports[port_idx], curr_vid, next_vid);
        if (BCM_FAILURE(rv)) {
            printf("egr_vlan_edit failed: port %d, curr_vid %d, next_vid %d.\n", xe_ports[port_idx], curr_vid, next_vid);
            return rv;
        }

        rv = l2_addr_add(unit, mac_address, curr_vid, sys_port, 0);
        if (BCM_FAILURE(rv)) {
            printf("l2_addr_add failed: vid %d.\n", curr_vid);
            return rv;
        }
    }

    /* send traffic back to in_port */
    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port, in_port);
    rv = l2_addr_add(unit, mac_address, next_vid, sys_port, 0);
    if (BCM_FAILURE(rv)) {
        printf("l2_addr_add failed: vid %d.\n", next_vid);
        return rv;
    }

    /* loopback all ports */
    for (port_idx = 0; port_idx < nof_caui_ports; ++port_idx) {
        rv = bcm_port_loopback_set(unit, caui_ports[port_idx], BCM_PORT_LOOPBACK_PHY);   
        if (BCM_FAILURE(rv)) {
            printf("bcm_port_loopback_set failed: port %d.\n", caui_ports[port_idx]);
            return rv;
        }
        disable_same_if_filter_all(unit,caui_ports[port_idx]); 
    }
    for (port_idx = 0; port_idx < nof_xe_ports; ++port_idx) {
        rv = bcm_port_loopback_set(unit, xe_ports[port_idx], BCM_PORT_LOOPBACK_PHY);
        if (BCM_FAILURE(rv)) {
            printf("bcm_port_loopback_set failed: port %d.\n", xe_ports[port_idx]);
            return rv;
        }
    }

    /* qtype need to support 100G ports */
    for (i = 0; i < nof_caui_ports; ++i) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_gport, BCM_CORE_ALL, 32 + 8*i);
        for (cosq = 0; cosq < 8; cosq++){
            rv = bcm_cosq_gport_sched_set(unit, voq_gport, cosq, BCM_COSQ_DELAY_TOLERANCE_100G_LOW_DELAY, 0);
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_sched_set failed: vid %d.\n", curr_vid);
                return rv;
            }
        }
    }

    printf("cint_full_capacity_snake_test: PASS\n\n");
    return rv;
}


int cint_mgmt_full_capacity_snake_test(int fap_0_unit, int fap_1_unit, bcm_mac_t mac_address, bcm_vlan_t base_vid, bcm_port_t in_port, bcm_port_t *caui_ports, int nof_caui_ports, bcm_port_t *xe_ports, int nof_xe_ports)
{
    int rv;
    bcm_gport_t fap_0_sys_port, fap_1_sys_port;
    bcm_vlan_t  curr_vid, next_vid;
    int         i, port_idx;
    bcm_gport_t voq_gport;
    bcm_cos_t   cosq;

    /* snake between caui ports */
    next_vid = base_vid;
    for (port_idx = 0; port_idx < nof_caui_ports; ++port_idx) {
        BCM_GPORT_SYSTEM_PORT_ID_SET(fap_0_sys_port, caui_ports[port_idx]);
        BCM_GPORT_SYSTEM_PORT_ID_SET(fap_1_sys_port, caui_ports[port_idx] + 256);
        for (i = 0; i < 10; ++i) {
            /* Send traffic from fap_0 to fap_1_sys_port */
            curr_vid = next_vid;
            rv = l2_addr_add(fap_0_unit, mac_address, curr_vid, fap_1_sys_port, 0);     
            if (BCM_FAILURE(rv)) {
                printf("l2_addr_add failed: unit $fap_0_unit vid %d.\n", curr_vid);
                return rv;
            }

            /* Increment vid at fap_1 port egress */
            next_vid = curr_vid + 1;
            rv = egr_vlan_edit(fap_1_unit, caui_ports[port_idx], caui_ports[port_idx], curr_vid, next_vid);
            if (BCM_FAILURE(rv)) {
                printf("egr_vlan_edit failed: unit $fap_1_unit port %d, curr_vid %d, next_vid %d.\n", caui_ports[port_idx], curr_vid, next_vid);
                return rv;
            }

            /* Send traffic from fap_1 to fap_0_sys_port */
            curr_vid = next_vid;
            rv = l2_addr_add(fap_1_unit, mac_address, curr_vid, fap_0_sys_port, 0);     
            if (BCM_FAILURE(rv)) {
                printf("l2_addr_add failed: unit $fap_1_unit vid %d.\n", curr_vid);
                return rv;
            }

            /* Increment vid at fap_0 port egress */
            next_vid = curr_vid + 1;
            rv = egr_vlan_edit(fap_0_unit, caui_ports[port_idx], caui_ports[port_idx], curr_vid, next_vid);
            if (BCM_FAILURE(rv)) {
                printf("egr_vlan_edit failed: unit $fap_0_unit port %d, curr_vid %d, next_vid %d.\n", caui_ports[port_idx], curr_vid, next_vid);
                return rv;
            }
        }
    }

    /* snake between xe ports */
    for (port_idx = 0; port_idx < nof_xe_ports; ++port_idx) {
        BCM_GPORT_SYSTEM_PORT_ID_SET(fap_0_sys_port, xe_ports[port_idx]);
        BCM_GPORT_SYSTEM_PORT_ID_SET(fap_1_sys_port, xe_ports[port_idx] + 256);

        /* Send traffic from fap_0 to fap_1_sys_port */
        curr_vid = next_vid;
        rv = l2_addr_add(fap_0_unit, mac_address, curr_vid, fap_1_sys_port, 0);
        if (BCM_FAILURE(rv)) {
            printf("l2_addr_add failed: unit $fap_0_unit vid %d.\n", curr_vid);
            return rv;
        }

        /* Increment vid at fap_1 port egress */
        next_vid = curr_vid + 1;
        rv = egr_vlan_edit(fap_1_unit, xe_ports[port_idx], xe_ports[port_idx], curr_vid, next_vid);
        if (BCM_FAILURE(rv)) {
            printf("egr_vlan_edit failed: unit $fap_1_unit port %d, curr_vid %d, next_vid %d.\n", xe_ports[port_idx], curr_vid, next_vid);
            return rv;
        }

        /* Send traffic from fap_1 to fap_0_sys_port */
        curr_vid = next_vid;
        rv = l2_addr_add(fap_1_unit, mac_address, curr_vid, fap_0_sys_port, 0);
        if (BCM_FAILURE(rv)) {
            printf("l2_addr_add failed: unit $fap_0_unit vid %d.\n", curr_vid);
            return rv;
        }

        /* Increment vid at fap_0 port egress */
        next_vid = curr_vid + 1;
        rv = egr_vlan_edit(fap_0_unit, xe_ports[port_idx], xe_ports[port_idx], curr_vid, next_vid);
        if (BCM_FAILURE(rv)) {
            printf("egr_vlan_edit failed: unit $fap_0_unit port %d, curr_vid %d, next_vid %d.\n", xe_ports[port_idx], curr_vid, next_vid);
            return rv;
        }
    }

    /* send traffic to in_port counter port in fap_1 */
    BCM_GPORT_SYSTEM_PORT_ID_SET(fap_1_sys_port, in_port + 256);
    curr_vid = next_vid;
    rv = l2_addr_add(fap_0_unit, mac_address, curr_vid, fap_1_sys_port, 0);
    if (BCM_FAILURE(rv)) {
        printf("l2_addr_add failed: unit $fap_0_unit vid %d.\n", curr_vid);
        return rv;
    }
    next_vid = curr_vid + 1;
    rv = egr_vlan_edit(fap_1_unit, in_port, in_port, curr_vid, next_vid);
    if (BCM_FAILURE(rv)) {
        printf("egr_vlan_edit failed: unit $fap_1_unit port %d, curr_vid %d, next_vid %d.\n", in_port, curr_vid, next_vid);
        return rv;
    }

    /* send traffic back to in_port */
    BCM_GPORT_SYSTEM_PORT_ID_SET(fap_0_sys_port, in_port);
    rv = l2_addr_add(fap_1_unit, mac_address, next_vid, fap_0_sys_port, 0);
    if (BCM_FAILURE(rv)) {
        printf("l2_addr_add failed: unit $fap_0_unit vid %d.\n", curr_vid);
        return rv;
    }

    /* loopback all ports */
    for (port_idx = 0; port_idx < nof_caui_ports; ++port_idx) {
        rv = bcm_port_loopback_set(fap_0_unit, caui_ports[port_idx], BCM_PORT_LOOPBACK_PHY);   
        if (BCM_FAILURE(rv)) {
            printf("bcm_port_loopback_set failed: unit $fap_0_unit port %d.\n", caui_ports[port_idx]);
            return rv;
        }
        disable_same_if_filter_all(fap_0_unit,caui_ports[port_idx]); 
    }
    for (port_idx = 0; port_idx < nof_xe_ports; ++port_idx) {
        rv = bcm_port_loopback_set(fap_0_unit, xe_ports[port_idx], BCM_PORT_LOOPBACK_PHY);
        if (BCM_FAILURE(rv)) {
            printf("bcm_port_loopback_set failed: unit $fap_0_unit port %d.\n", xe_ports[port_idx]);
            return rv;
        }
    }
    for (port_idx = 0; port_idx < nof_caui_ports; ++port_idx) {
        rv = bcm_port_loopback_set(fap_1_unit, caui_ports[port_idx], BCM_PORT_LOOPBACK_PHY);   
        if (BCM_FAILURE(rv)) {
            printf("bcm_port_loopback_set failed: unit $fap_1_unit port %d.\n", caui_ports[port_idx]);
            return rv;
        }
        disable_same_if_filter_all(fap_1_unit,caui_ports[port_idx]); 
    }
    for (port_idx = 0; port_idx < nof_xe_ports; ++port_idx) {
        rv = bcm_port_loopback_set(fap_1_unit, xe_ports[port_idx], BCM_PORT_LOOPBACK_PHY);
        if (BCM_FAILURE(rv)) {
            printf("bcm_port_loopback_set failed: unit $fap_1_unit port %d.\n", xe_ports[port_idx]);
            return rv;
        }
    }
    rv = bcm_port_loopback_set(fap_1_unit, in_port, BCM_PORT_LOOPBACK_PHY);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_loopback_set failed: unit $fap_1_unit port %d.\n", in_port);
        return rv;
    }

    /* qtype need to support 100G ports */
    for (i = 0; i < nof_caui_ports; ++i) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_gport, BCM_CORE_ALL, 32 + 8*i);
        for (cosq = 0; cosq < 8; cosq++){
            rv = bcm_cosq_gport_sched_set(fap_1_unit, voq_gport, cosq, BCM_COSQ_DELAY_TOLERANCE_100G_LOW_DELAY, 0);
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_sched_set failed: unit $fap_1_unit vid %d.\n", curr_vid);
                return rv;
            }
        }
    }
    for (i = 0; i < nof_caui_ports; ++i) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_gport, BCM_CORE_ALL, 2080 + 8*i);
        for (cosq = 0; cosq < 8; cosq++){
            rv = bcm_cosq_gport_sched_set(fap_0_unit, voq_gport, cosq, BCM_COSQ_DELAY_TOLERANCE_100G_LOW_DELAY, 0);
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_sched_set failed: unit $fap_0_unit vid %d.\n", curr_vid);
                return rv;
            }
        }
    }

    printf("cint_mgmt_full_capacity_snake_test: PASS\n\n");
    return rv;
}


/***************************
 * L3 Snake Functions
 */

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


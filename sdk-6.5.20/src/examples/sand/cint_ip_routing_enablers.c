/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * This test verifies the IP routing enablers and the IN-RIF profile proper behavior by filling all the possible combination and
 * verifying that each IN-RIF is working according to it assign profile.
 * This test have two iterations, in the first iteration the IP routing enablers have 15 options (one option is a default option)
 * taking 4 bits and the other two bits (out of the 6 in-rif profile bits) are allocated for the RPF and VPN.
 * In the second iteration the number of IP routing enablers is 7 (3 bits) and the other 3 bits are allocated for the RPF, VPN
 * and L3 MCAST L2 options.
 * Tested enablers are IPV4UC, IPV4MC, IPV6UC and IPV6MC (first iteration only).
 * Tested IN-RIF profile features are RPF, VPN and L3 MCAST L2(second iteration only).
 */
int in_rif_enablers(int unit,int ipmc_group_start, int nof_of_fix_bits, int inPort, int outPort1, int outPort2, int outPort3,int supported_profiles,int nof_enablers)
{
    int rv= BCM_E_NONE,profileNumber ,fix_bits, enablers, i;
    int vrf, ipmc_group, ipmc_group_v6, nof_traps;
    int fecId, port2use, ipmc_l3mcastl2_mode, double_chekc;
    int trap_id;
    int in_rif_profile_size = 6;
    bcm_l3_intf_t intf;
    bcm_l3_ingress_t ingress_intf;
    bcm_ipmc_addr_t data;
    bcm_l2_addr_t l2_addr;
    bcm_mac_t mc_address         = {0x01, 0x00, 0x5E, 0x02, 0x01, 0x00};  /* 01:00:5E:02:01:00 */
    bcm_mac_t mc_address_v6      = {0x33, 0x33, 0x00, 0x02, 0x00, 0x00};  /* 33:33:00:02:00:00 */
    bcm_mac_t out_mac            = {0x00, 0x45, 0x45, 0x45, 0x45, 0x00};
    bcm_mac_t out_mac_vpn        = {0x00, 0x25, 0x25, 0x25, 0x25, 0x25};
    bcm_ip6_t ipv6_mc_addr       = {0xFF,0xFF,0x16,0,0x35,0,0x70,0,0,0,0xdb,0x7,0,2,0,0};
    bcm_ip6_t ipv6_mc_full_mask  = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ipv6_addr          = {0x16,0x16,0x16,0,0x35,0,0x70,0,0,0,0xdb,0x7,0,0,0,0};
    bcm_if_t l3egid;
    bcm_vlan_port_t port_vlan;
    bcm_l3_egress_t l3eg;
    bcm_l3_host_t l3host;
    uint32 IPMcV4             = 0xE0000100;
    uint32 IPaddr             = 0x22664500;
    uint32 VPN_IPaddr         = 0x33664500;
    uint32 VPN_IPmask         = 0xFFFFFF00;
    uint32 CLEAR_Pmask        = 0xFFFFFFFF;
    uint32 IPV4UC_DIS, IPV4MC_DIS, IPV6UC_DIS, IPV6MC_DIS, RPF, VPN, L2MCL3, flags;
    int out_port[3], encap_id;
    int trapId;
    bcm_if_t l3egid_null;
    bcm_multicast_replication_t replications;
    bcm_rx_trap_config_t trap_config;
    bcm_rx_trap_t traps[2] = {bcmRxTrapUcLooseRpfFail, bcmRxTrapMyMacAndIpDisabled};
    int profile_count = 0;
    sand_utils_l3_host_ipv6_s  host_ipv6_structure;
    sand_utils_l3_route_ipv6_s route_ipv6_structure;
    
    out_port[0] = outPort1;
    out_port[1] = outPort2;
    out_port[2] = outPort3;

    printf("nof_of_fix_bits %d nof_enablers %d \n",nof_of_fix_bits,nof_enablers);

    nof_traps = (is_device_or_above(unit,QUMRAN_AX) && !is_device_or_above(unit,JERICHO2)) ? 1 : 2;

    /*
     * Disabling the UC IPv4/6 in a RIF doesn't drop a UC packets that enters the RIF unless a trap is set.
     * In QAX a trap isn't needed any more.
     */
    for(i = 0; i < nof_traps; i++) {
        rv = bcm_rx_trap_type_create(unit, is_device_or_above(unit,JERICHO2) ? 0 : BCM_RX_TRAP_WITH_ID, traps[i], &trapId);
        if ((rv != BCM_E_NONE) && (rv != BCM_E_EXISTS)) {
            printf("Error, bcm_rx_trap_type_create \n");
            return rv;
        }

        bcm_rx_trap_config_t_init(&trap_config);
        trap_config.flags |= (BCM_RX_TRAP_UPDATE_DEST);
        trap_config.trap_strength = 5;
        trap_config.dest_port = BCM_GPORT_BLACK_HOLE;

        rv = bcm_rx_trap_set(unit, trapId, &trap_config);
        if ((rv != BCM_E_NONE) && (rv != BCM_E_PARAM)) {
            printf("Error, bcm_rx_trap_set \n");
            return rv;
        }
    }


    for (fix_bits = 0; (fix_bits <  (1 << nof_of_fix_bits)) && (profile_count < supported_profiles) ; fix_bits++)
    {
        RPF    = ( (fix_bits & 0x1) > 0) ? 1 : 0;
        VPN    = ( (fix_bits & 0x2) > 0) ? 1 : 0;
        L2MCL3 = ( (fix_bits & 0x4) > 0) ? 1 : 0;

        for(enablers = 0; (enablers < nof_enablers) && (profile_count < supported_profiles); enablers++)
        {

            IPV4UC_DIS = ( (enablers & 0x1) > 0) ? 1 : 0;
            IPV4MC_DIS = ( (enablers & 0x2) > 0) ? 1 : 0;
            IPV6UC_DIS = ( (enablers & 0x4) > 0) ? 1 : 0;
            IPV6MC_DIS = ( (enablers & 0x8) > 0) ? 1 : 0;

            profileNumber =  (fix_bits << (in_rif_profile_size - nof_of_fix_bits)) | enablers;

            printf("profileNumber 0x%x nof_of_fix_bits 0x%x fix_bits 0x%x enablers 0x%x RPF %d, VPN %d, L2MCL3 %d, \n",profileNumber,nof_of_fix_bits,fix_bits,enablers,RPF,VPN,L2MCL3);

            vrf = 300 + profileNumber;
            ipmc_group     = ipmc_group_start + profileNumber;
            ipmc_group_v6  = ipmc_group + 400;

            /* create IN-LIF */
            bcm_vlan_port_t_init(port_vlan);
            port_vlan.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            port_vlan.match_vlan = vrf;
            port_vlan.port = inPort;
            port_vlan.vsi = vrf;
            port_vlan.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
            rv = bcm_vlan_port_create(unit, &port_vlan);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_vlan_port_add\n");
                return rv;
            }

            /* IPV4 UC check
            *
            * Add a host that will forward a packet that passes through the matching VRF IN-RIF.
            * In case this IN-RIF profile has an IPV4UC disabled the packet doesn't expected to
            * be forwarded and the same goes if the IN-RIF has a URPF check.
            */

            bcm_l3_egress_t_init(&l3eg);

            out_mac[5] = profileNumber;
            sal_memcpy(l3eg.mac_addr, out_mac, 6);
            l3eg.vlan = 400 + profileNumber;
            rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null);
            if (rv != BCM_E_NONE) {
                printf ("bcm_l3_egress_create failed\n");
                return rv;
            }

            l3eg.intf = 300;
            l3eg.port = out_port[0];
            rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fecId);
            if (rv != BCM_E_NONE) {
                printf ("bcm_l3_egress_create failed\n");
                return rv;
            }

            bcm_l3_host_t_init(&l3host);
            l3host.l3a_ip_addr = IPaddr + profileNumber;
            l3host.l3a_vrf = vrf;
            l3host.l3a_intf = fecId;
            rv = bcm_l3_host_add(unit, &l3host);
            if (rv != BCM_E_NONE) {
                printf ("bcm_l3_host_add failed: %x \n", rv);
                return rv;
            }

            /* IPV4 MC check
             *
             * In case the IPV4 MC is enable the LPM <VRF, G, S, IN-RIF> entry is expected
             * to be hit and in case of bridge (IPV4 MC disable and L3 MCAST L2 is off ) the LEM<FID,DA> is expected.
             * In case IPV4 MC disable and L3 MCAST L2 is on an LEM<FID, DA> is expected.
             * Each entry direct to different port to verify correct behavior
             */
            flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
            if(IPV4MC_DIS == 1 && L2MCL3 == 1) {
                /* Jericho2 does not require multicast type */
                if (!is_device_or_above(unit, JERICHO2)) {
                    flags |= BCM_MULTICAST_TYPE_L3;
                }
                port2use = out_port[2];
            } else if (IPV4MC_DIS == 1 && L2MCL3 == 0) {
                if (!is_device_or_above(unit, JERICHO2)) {
                    flags |= BCM_MULTICAST_TYPE_L2;
                }
                port2use = out_port[1];
            } else {
                if (!is_device_or_above(unit, JERICHO2)) {
                    flags |= BCM_MULTICAST_TYPE_L3;
                }
                port2use = out_port[0];
            }

            rv = bcm_multicast_create(unit, flags, &ipmc_group);

            if (rv != BCM_E_NONE) {
                printf("Error, bcm_multicast_create \n");
                return rv;
            }

            bcm_multicast_replication_t_init(&replications);
            replications.port = port2use;
            replications.encap1 = vrf;
            flags = is_device_or_above(unit, JERICHO2) ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_INGRESS;
            rv = bcm_multicast_set(unit, ipmc_group, flags, 1, &replications);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_multicast_set\n");
                return rv;
            }

            if (is_device_or_above(unit, JERICHO2) && (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL))) {
                /* Configure the MC group to work with both cores */
                bcm_module_t mreps[2];
                uint32 max_nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
                mreps[0] = 0;
                mreps[1] = 1;
                rv = bcm_fabric_multicast_set(unit, ipmc_group, 0, max_nof_cores, mreps);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_fabric_multicast_set \n");
                    return rv;
                }
            }

            if (IPV4MC_DIS == 1 && L2MCL3 == 1) {
                /* Configurations relevant for LEM<FID,DIP> lookup*/
                bcm_ipmc_addr_t_init(&data);
                data.mc_ip_addr = IPMcV4 + profileNumber;
                data.mc_ip_mask = CLEAR_Pmask;
                data.s_ip_addr = 0x0;
                data.s_ip_mask = 0;
                data.vid = vrf;
                data.flags = BCM_IPMC_L2;
                data.group = ipmc_group;
                /* Creates the entry */
                rv = bcm_ipmc_add(unit,&data);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_ipmc_add \n");
                    return rv;
                }
            } else if(IPV4MC_DIS == 1) {
                /* Configurations relevant for LEM<FID,DA> lookup*/
                mc_address[5] = profileNumber;
                bcm_l2_addr_t_init(&l2_addr,mc_address,vrf);
                l2_addr.flags |= BCM_L2_MCAST;
                l2_addr.l2mc_group = ipmc_group;
                rv = bcm_l2_addr_add(unit,&l2_addr);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_l2_addr_add \n");
                    return rv;
                }
            } else {
                /* Configurations relevant for LPM <VRF, G, S, IN-RIF> lookup*/
                bcm_ipmc_addr_t_init(&data);
                data.mc_ip_addr = IPMcV4 + profileNumber;
                data.mc_ip_mask = 0xffffffff;
                data.s_ip_addr = 0xC0A8000B;
                data.s_ip_mask = 0xffffffff;
                data.vid = vrf;
                data.vrf = vrf;
                data.group = ipmc_group;
                rv = bcm_ipmc_add(unit,&data);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_ipmc_add \n");
                    return rv;
                }
            }

            /* IPV6 UC check
            *
            * Add a host that will forward a packet that passes through the matching VRF IN-RIF.
            * In case this IN-RIF profile has an IPV6UC disabled the packet doesn't expected to
            * be forwarded and the same goes if the IN-RIF has a URPF check.
            */

            bcm_l3_egress_t_init(&l3eg);

            out_mac[5] = 32 + profileNumber;
            sal_memcpy(l3eg.mac_addr, out_mac, 6);

            l3eg.vlan   = 500 + profileNumber;
            rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null);
            if (rv != BCM_E_NONE) {
                printf ("bcm_l3_egress_create failed\n");
                return rv;
            }

            l3eg.port = out_port[0];
            l3eg.intf = 300;
            rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fecId);
            if (rv != BCM_E_NONE) {
                printf ("bcm_l3_egress_create failed\n");
                return rv;
            }

            ipv6_addr[15] = profileNumber;
            sand_utils_l3_host_ipv6_s_common_init(unit, 0, &host_ipv6_structure, ipv6_addr, vrf, 0, 0, fecId);
            rv = sand_utils_l3_host_ipv6_add(unit, &host_ipv6_structure);
            if (rv != BCM_E_NONE) {
                printf("Error, in function sand_utils_l3_host_ipv6_add\n");
                return rv;
            }
            if (!is_device_or_above(unit, JERICHO2) && soc_property_get(unit, "enhanced_fib_scale_prefix_length", 0)) {
                bcm_ip6_t l3a_ip6_mask = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
                sand_utils_l3_route_ipv6_s_common_init(unit, 0, &route_ipv6_structure, ipv6_addr, l3a_ip6_mask, vrf, 0, 0, fecId);
                rv = sand_utils_l3_route_ipv6_add(unit, &route_ipv6_structure);
                if (rv != BCM_E_NONE) {
                    printf("Error, in function sand_utils_l3_route_ipv6_add\n");
                    return rv;
                }
            }

            /* IPV6 MC check
             *
             * In case the IPV6 MC is enable the LPM <VRF, G, IN-RIF> entry is expected
             * to be hit and in case of bridge (IPV6 MC disable ) the LEM<FID,DA> is expected.
             * Each entry direct to different port to verify correct behavior
             */
            flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
            if(IPV6MC_DIS == 1) {
                if (!is_device_or_above(unit, JERICHO2)) {
                    flags |= BCM_MULTICAST_TYPE_L2;
                }
                port2use = out_port[2];
            } else {
                if (!is_device_or_above(unit, JERICHO2)) {
                    flags |= BCM_MULTICAST_TYPE_L3;
                }
                port2use = out_port[1];
            }

            rv = bcm_multicast_create(unit, flags, &ipmc_group_v6);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_multicast_create \n");
                return rv;
            }

            bcm_multicast_replication_t_init(&replications);
            replications.port = port2use;
            replications.encap1 = vrf;
            flags = is_device_or_above(unit, JERICHO2) ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_INGRESS;
            rv = bcm_multicast_set(unit, ipmc_group_v6, flags, 1, &replications);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_multicast_set\n");
                return rv;
            }

            if (is_device_or_above(unit, JERICHO2) && (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL))) {
                /* Configure the MC group to work with both cores */
                bcm_module_t mreps[2];
                uint32 max_nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
                mreps[0] = 0;
                mreps[1] = 1;
                rv = bcm_fabric_multicast_set(unit, ipmc_group_v6, 0, max_nof_cores, mreps);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_fabric_multicast_set \n");
                    return rv;
                }
            }

            if(IPV6MC_DIS == 1) {
                /* Add LEM<FID,DA> lookup */
                mc_address_v6[5] = profileNumber;
                bcm_l2_addr_t_init(&l2_addr,mc_address_v6,vrf);
                l2_addr.flags |= BCM_L2_MCAST;
                l2_addr.l2mc_group = ipmc_group_v6;
                rv = bcm_l2_addr_add(unit,&l2_addr);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_l2_addr_add \n");
                    return rv;
                }
            } else {
                /* Add LPM <VRF, G, IN-RIF> lookup */
                bcm_ipmc_addr_t_init(&data);
                ipv6_mc_addr[15] = profileNumber;
                sal_memcpy(data.mc_ip6_addr, ipv6_mc_addr, 16);
                sal_memcpy(data.mc_ip6_mask, ipv6_mc_full_mask, 16);
                data.vid = vrf;
                data.flags |= BCM_IPMC_IP6;
                data.group = ipmc_group_v6;
                data.vid = vrf;
                data.vrf = vrf;
                rv = bcm_ipmc_add(unit,&data);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_ipmc_add \n");
                    return rv;
                }
            }

            profile_count++;
        }
    }

    /*
    * Add a single VPN entry that all the IN-RIF that have the global route flag can hit and the other can't
    */
    bcm_l3_route_t l3rt;

    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, out_mac_vpn, 6);
    l3eg.vlan   = 666;
    rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_egress_create failed\n");
        return rv;
    }

    l3eg.port = out_port[0];
    l3eg.intf = 300;
    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &fecId);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_egress_create failed\n");
        return rv;
    }

    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_subnet = VPN_IPaddr;
    l3rt.l3a_ip_mask = VPN_IPmask;
    l3rt.l3a_intf = fecId;
    l3rt.l3a_vrf = 0;
    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_route_add failed: %x \n", rv);
        return rv;
    }

    return rv;
}

/** A function to create the L3 intf and set the ingress enablers. */
int in_rif_enablers_rif_create(
        int unit,
        int profile_number,
        int vrf,
        int RPF,
        int IPV4UC_DIS,
        int IPV4MC_DIS,
        int IPV6UC_DIS,
        int IPV6MC_DIS,
        int VPN,
        int L2MCL3,
        int profile_count)
{

    int rv = 0;
    bcm_mac_t my_mac_address     = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_l3_intf_t intf;
    bcm_l3_ingress_t ingress_intf;
    /* create L3 interface */
    bcm_l3_intf_t_init(&intf);
    my_mac_address[5] = profile_number;
    sal_memcpy(intf.l3a_mac_addr, my_mac_address, 6);
    intf.l3a_vid = vrf;
    intf.l3a_vrf = vrf;
    rv = bcm_l3_intf_create(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }

    /* create ingress */
    bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags = BCM_L3_INGRESS_WITH_ID;
    if(IPV4UC_DIS) {ingress_intf.flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST;}
    if(IPV4MC_DIS) {ingress_intf.flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST;}
    if(IPV6UC_DIS) {ingress_intf.flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST;}
    if(IPV6MC_DIS) {ingress_intf.flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;}

    if(VPN == 1)   {ingress_intf.flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;}
    if(L2MCL3 == 1){ingress_intf.flags |= BCM_L3_INGRESS_L3_MCAST_L2;}

    ingress_intf.urpf_mode = (RPF == 0) ? bcmL3IngressUrpfDisable : bcmL3IngressUrpfLoose;

    ingress_intf.vrf= vrf;
    printf("profile_count %d IPV4UC_DIS %d, IPV4MC_DIS %d,IPV6UC_DIS %d,IPV6MC_DIS %d, Flags 0x%x\n",
                profile_count,IPV4UC_DIS,IPV4MC_DIS,IPV6UC_DIS,IPV6MC_DIS,ingress_intf.flags);

    printf("i=%d, flags 0x%x \n",profile_number,ingress_intf.flags);
    rv = bcm_l3_ingress_create(unit, &ingress_intf, &intf.l3a_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create \n");
        return rv;
    }
    return rv;
}

/** A function to delete the L3 interface */
int in_rif_enablers_rif_delete(
        int unit,
        int intf_id)
{

    int rv = 0;
    bcm_l3_intf_t intf;
    /* delete L3 interface */
    bcm_l3_intf_t_init(&intf);
    intf.l3a_intf_id = intf_id;
    rv = bcm_l3_intf_delete(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }

    return rv;
}

/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
/*
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../
 * cint src/examples/dnx/utility/cint_dnx_utils_global.c
 * cint src/examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint src/examples/dnx/l3/cint_dnx_ecmp_basic.c
 * cint src/examples/dnx/l3/cint_dnx_ecmp_eudp_example.c
 * cint
 * dnx_ecmp_example_eudp(0,200,201,100);
 * exit;
 *
 * * ETH-RIF packet *
 * tx 1 psrc=200 data=0x000c00020000000007000100080045000035000000008000fa45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * * Received packets on unit 0 should be: *
 * * Source port: 0, Destination port: 0 *
 * * Data: 0x0000cd1d00123456789a81000000080045000035000000007f00fb45c08001017fffff02 *
 *
 */


/*
 * Packets will be routed through different FEC from the in_port to the out_port and each FEC will have a different next hop MAC address.
 *
 * packet to send:
 *  - in port = in_port
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff02
 * expected:
 *  - out port = out_port
 *  - vlan = 100
 *  - DA = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}
 *  TTL decremented
 *
 */
int
dnx_ecmp_example_eudp(
    int unit,
    int in_port,
    int out_port,
    int ecmp_id)
{
    int rv;
    int iter;
    bcm_if_t intf_array[NOF_OF_MEMBERS_ECMP_GROUP];
    bcm_gport_t gport;
    bcm_l3_egress_ecmp_t ecmp;
    int user_profile_id;
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    char *proc_name;

    proc_name = "dnx_ecmp_example_eudp";
    cint_ecmp_info.in_port = in_port;
    cint_ecmp_info.out_port = out_port;

    get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &(cint_ecmp_info.fec_id3));
    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, cint_ecmp_info.in_port, cint_ecmp_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_ecmp_info.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set out_port\n");
        return rv;
    }

    /*
     * Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create_vrf(unit, cint_ecmp_info.intf_in, cint_ecmp_info.vrf, cint_ecmp_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties
     */
    ingr_itf.intf_id = cint_ecmp_info.intf_in;
    ingr_itf.vrf = cint_ecmp_info.vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set ingr_itf\n");
        return rv;
    }

    /*
     * Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create_vrf(unit, cint_ecmp_info.intf_out, 0, cint_ecmp_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    BCM_GPORT_LOCAL_SET(gport, cint_ecmp_info.out_port);

    /*
     * Create FECs for the ECMP group
     */
    for (iter = 0; iter < NOF_OF_MEMBERS_ECMP_GROUP; iter++)
    {
        /*
         * Create ARP and set its properties
         */
        rv = create_arp_plus_ac_type(unit, 0, cint_ecmp_info.arp_next_hop_mac, &cint_ecmp_info.encap_id1, 0);
        if (rv != BCM_E_NONE)
        {
           printf("%s(): Error, create_arp_plus_ac_type\n",proc_name);
           return rv;
        }

        /*
         * Create FEC and set its properties
         */

        printf("%s():. Create main FEC and set its properties.\r\n",proc_name);
        if (BCM_GPORT_IS_SET(out_port)) {
           gport = out_port;
        }
        else {
           BCM_GPORT_LOCAL_SET(gport, out_port);
        }

        int fec_flags2 = 0;
        if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
        {
            fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
        }

        rv = l3__egress_only_fec__create_inner(unit, cint_ecmp_info.fec_id3, cint_ecmp_info.intf_out, cint_ecmp_info.encap_id1, gport, 0,
                                               fec_flags2, 0, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create egress object FEC only\n",proc_name);
            return rv;
        }

        /*
         * Place the new FEC into the interfaces array of the ECMP
         * Increment the FEC number ad the encap number for the next FEC.
         * Set a different next hop MAC to identify different FEC selection.
         */
        intf_array[iter] = cint_ecmp_info.fec_id3;
        cint_ecmp_info.fec_id3++;
        cint_ecmp_info.encap_id1++;
        cint_ecmp_info.arp_next_hop_mac[5]++;
    }

    {
        /**
         * Implement equal cost group by setting equally distributed members  = 12.5% distribution per member
         * int ecmp_offsets_array[128] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 
         *                                0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7,
         *                                0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7,
         *                                0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7};
         */
        /*
         * Implement weighted cost group by setting differently distributed members
         * member 0 => 15.62%
         * member 1 => 15.62%
         * member 2, 3 => 10.93%
         * member 4, 5, 6, 7 => 11.7%
         */
        int wcmp_offsets_array[128] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 
                                          0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7,
                                          0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7,
                                          0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 4, 5, 6, 7};
        bcm_l3_egress_ecmp_t ecmp_profile;
        int count = 128;
        bcm_l3_egress_ecmp_t_init(&ecmp_profile);
        ecmp_profile.rpf_mode = bcmL3EcmpUrpfStrictEm;
        ecmp_profile.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
        printf("%s(): Create EUDP with weighted cost \n",proc_name);
        rv = bcm_l3_egress_ecmp_user_profile_create(unit, ecmp_profile, count, wcmp_offsets_array);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, bcm_l3_egress_ecmp_user_profile_create\n",proc_name);
            return rv;
        }
        user_profile_id = ecmp_profile.user_profile;
    }
    /*
     * Create an ECMP group using an User profile
     */
    int nof_ecmp_basic_mode = *dnxc_data_get(unit, "l3", "ecmp", "nof_ecmp_basic_mode", NULL);
    bcm_l3_egress_ecmp_t_init(&ecmp);

    /*
    * ECMP properties
    */
    ecmp.max_paths = 0;
    ecmp.flags = BCM_L3_WITH_ID;
    ecmp.ecmp_intf = ecmp_id;
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    /** If the ECMP group is in the extended range, add the needed flag. */
    if (ecmp_id >= nof_ecmp_basic_mode)
    {
        ecmp.ecmp_group_flags = BCM_L3_ECMP_EXTENDED;
    }
    /** create an ECMP, containing the FECs entries */
    ecmp.user_profile = user_profile_id;
    ecmp.ecmp_group_flags = BCM_L3_ECMP_USER_PROFILE;
    rv = bcm_l3_egress_ecmp_create(unit, &ecmp, 1, intf_array);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_l3_egress_ecmp_create failed: %d \n", rv);
        return rv;
    }
    /*
     * Add Route entry
     */
    rv = add_route_ipv4(unit, cint_ecmp_info.host, cint_ecmp_info.mask, cint_ecmp_info.vrf, ecmp_id);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_l3_route_add \n",proc_name);
        return rv;
    }

    return rv;
}

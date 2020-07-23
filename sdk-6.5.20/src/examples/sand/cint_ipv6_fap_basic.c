/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * IPv6 Example script
 */

/*
the cint creates four UC IPV6 entries and one MC IPV6 entry.

run:
cint ../sand/utility/cint_sand_utils_mpls.c
cint utility/cint_utils_l3.c
cint cint_qos_l3.c
cint cint_ipmc_example.c
cint cint_ipv6_fap.c
cint
print qos_map_l3(unit);
print bcm_l3_ip6(unit, <second_unit>, <in_port>, <out_port>);

for one device: call bcm_l3_ip6() with second_unit<0

for two devices: make sure that master device is unit 0 with modid 0 and slave device is unit 1 with modid 1

traffic example for two devices:

    run ethernet header with DA 0:0:0:0:55:34 (for UC) or 33:33:3:2:1:0 (for compatible-MC) and vlan tag id 17
    and IPV6 header with DIP according to the following:

    device 0:
    ---------
      <in_port>:    route  01001600350070000000db0700000000 : 100:1600:3500:7000:0:db07:0:0
                    mask   0000000000000000ffffffffffffffff : 0:0:0:0:ffff:ffff:ffff:ffff

                    route  01001600350064000000db0700000000 : 100:1600:3500:6400:0:db07:0:0
                    mask   0000000000000000fcffffffffffffff : 0:0:0:0:fcff:ffff:ffff:ffff

                    route  ff1e0d0c0b0a09080706050403020100 : ff1e:d0c:b0a:908:706:504:302:100 (comaptible-MC)

    device 1:
    ---------
      <in_port>:    route  01001600550078000000db0700000000 : 100:1600:5500:7800:0:db07:0:0
                    mask   0000000000000000f0ffffffffffffff : 0:0:0:0:f0ff:ffff:ffff:ffff

                    route  01001600780088000000db0700000000 : 100:1600:7800:8800:0:db07:0:0
                    mask   0000000000000000c0ffffffffffffff : 0:0:0:0:c0ff:ffff:ffff:ffff

                    route  ff1e0d0c0b0a09080706050403020100 : ff1e:d0c:b0a:908:706:504:302:100 (comaptible-MC)

UC packets from device 0 will be forwrded to <out_port> on device 1.
UC packets from device 1 will be forwrded to <out_port> on device 0.
MC packet from both devices will be forwrded to device 0 and to device 1 on ports <out_port>,<out_port+1>.

UC packets will arrive at out_port with:
    ethernet header with DA 0:0:0:0:cd:1d, SA 0:0:0:0:55:34 and vlan tag id 18
MC packets will arrive at out_port with:
    ethernet header with DA 0:0:0:0:cd:1d, SA 33:33:3:2:1:0 and vlan tag id 18
    IPV6 header with same DIP and hop_limit decremented

traffic example for single device:

run same packets from <in_port>. All 4 types of UC packets will arrive at <out_port>.
MC packets will arrive at ports <out_port>,<out_port+1> and will not arrive at <in_port>, because of same-interface filter.

to test IPV6 default route:
---------------------------
call:
ipv6_default_dest_set(unit, <mc>, 1, 14);

and run unknown UC/MC traffic (depends on whether <mc> is set or not).
this will set the default destination to be a trapm pointing at <out_port>, so the unknown traffic will go there.

now call:
ipv6_default_dest_set(unit, <mc>, 0, 14);

nad run again the same MC/UC IPV6 traffic.
this will set the default route to be VRF and all traffic will be dropped.

--------------------------------------------------------------------------------

The cint can be also used to test RIF TTL thershold:
In this example, same out_rif (=18) is created, but this time its TTL threshold is set as well.
If an IPV6 MC packet arrives with a hop-limit that is lower than the threshold, the packet will be dropped at egress.
calling sequence:
cint cint_qos_l3.c
cint cint_ipmc_example.c
cint cint_ipv6_fap.c
cint utility/cint_utils_l3.c
cint
print qos_map_l3(unit);
print bcm_l3_ip6_with_ttl(unit, -1, <in_port>, <out_port>, <ttl_threshold>);


The cint can be also used to test mtu by calling modify_l3_interface_ingress(int unit, int mtu, int out_vlan)
after bcm_l3_ip6. In this example out_vlan is set to 18, so to disable mtu use the following sequence:
cint cint_qos_l3.c
cint cint_ipmc_example.c
cint cint_ipv6_fap.c
cint utility/cint_utils_l3.c
cint
print qos_map_l3(unit);
print bcm_l3_ip6(unit,-1,13,14);
print modify_l3_interface_ingress(unit, 0, 18);
where out-vlan 18 is the outgoing interface



  Note: this cint also includes tests for 4 label push and protection

*/

void ipv6_create_mask(bcm_ip6_t *ip6_mask, uint32 prefix_length) {
    int i;

    for (i=15; i>=0; i--) {
        if (i < prefix_length/8) {
            ip6_mask[i] = 0xFF;
        } else if (i == prefix_length/8) {
            ip6_mask[i] = 0xFF - ((1 << (8 -(prefix_length % 8))) - 1);
        } else {
            ip6_mask[i] = 0X00;
        }
    }
}

/*
 * The purpose of this function is to make sure that the first byte of a ipv6 is 0xFF
 * to make sure that it is a valid IPv6 MC address.
 */

void ipv6_and_mask_mc(bcm_ip6_t *ip6_output, bcm_ip6_t *ip6, bcm_ip6_t *ip6_mask) {
    int i;
    /*Make sure that the first byte of the DIP is 0xFF so that it is a valid MC DIP.*/
    ip6_output[0] = 0xFF;
    for (i=1; i<16; i++) {
        ip6_output[i] = ip6[i] & ip6_mask[i];
    }
}

int ipv6_mc_default_route(int unit, int in_port, int out_port, int out_port2) {

    int rv = 0, i, flags;
    int vlan = 50, vrf = 100;
    bcm_l3_intf_t l3_intf;
    bcm_if_t l3_egress;
    bcm_ipmc_addr_t ipmc_addr;
    bcm_multicast_t ipmc_group;
    char *kbp_dev_type = soc_property_get_str(unit, spn_EXT_TCAM_DEV_TYPE);
    int is_kbp = kbp_dev_type ? (!(sal_strcmp("NONE", kbp_dev_type)) ? 0 : 1) : 0;

    bcm_mac_t my_mac = { 0x00, 0x0a, 0x00, 0x00, 0x0b, 0x00 };
    bcm_mac_t my_mac1 = { 0x00, 0x0a, 0x00, 0x00, 0x0b, 0x01 };
    bcm_ip6_t ip6_addr = { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11 };
    int prefix_length[3] = { 128, 64, 0 };
    bcm_multicast_replication_t replications[2];
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);
    /* prefix length 0 cannot be configured in JR2.
     * IPv6 address group is not multicast - expecting the first byte to be 0xFF, instead it is 0x00
     */
    if (is_device_or_above(unit, JERICHO2)) {
        prefix_length[2] = 32;
    }

    rv = in_port_intf_set(unit, in_port, vlan);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set\n");
        return rv;
    }

    rv = out_port_set(unit, out_port2);
    if ((rv != BCM_E_NONE) && (rv != BCM_E_EXISTS))
    {
        printf("Error, out_port_set\n");
        return rv;
    }

    /*
     * 2. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, vlan, my_mac1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    /*
     * 3. Set Incoming ETH-RIF properties
     */
    ingr_itf.intf_id = vlan;
    ingr_itf.vrf = vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    for(i = 0; i < 3; i++) {
        vlan = 100 + i * 100;
        ipmc_group = 6000 + vlan + i;

        bcm_l3_intf_t_init(&l3_intf);
        my_mac[5]++;
        sal_memcpy(l3_intf.l3a_mac_addr, my_mac, 6);
        l3_intf.l3a_vid = vlan;
        l3_intf.l3a_vrf = vrf;
        rv = bcm_l3_intf_create(unit, &l3_intf);
        if (rv) {
            printf("Error, bcm_l3_intf_create index %d\n", i);
            return rv;
        }

        flags = is_device_or_above(unit, JERICHO2) ? (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID) : (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID);
        rv = bcm_multicast_create(unit, flags, &ipmc_group);
        if (rv) {
            printf("Error, bcm_multicast_create index %d\n", i);
            return rv;
        }

        bcm_multicast_replication_t_init(&replications[0]);
        replications[0].encap1 = l3_intf.l3a_intf_id;
        replications[0].port = out_port;
        bcm_multicast_replication_t_init(&replications[1]);
        replications[1].encap1 = l3_intf.l3a_intf_id;
        replications[1].port = out_port2;

        flags = is_device_or_above(unit, JERICHO2) ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_INGRESS;
        rv = bcm_multicast_set(unit, ipmc_group, flags, 2, replications);
        if (rv) {
            printf("Error, bcm_multicast_set index %d\n", i);
            return rv;
        }

        bcm_ipmc_addr_t_init(&ipmc_addr);
        ipv6_create_mask(ipmc_addr.mc_ip6_mask, prefix_length[i]);
        ipv6_and_mask_mc(ipmc_addr.mc_ip6_addr, ip6_addr, ipmc_addr.mc_ip6_mask);
        ipmc_addr.vrf = vrf;
        ipmc_addr.group = ipmc_group;
        ipmc_addr.flags = BCM_IPMC_IP6;
        if (is_device_or_above(unit,JERICHO2) & is_kbp)
        {
            /* External lookup enabled on JR2 */
            ipmc_addr.flags |= BCM_IPMC_TCAM;
        }
        rv = bcm_ipmc_add(unit, ipmc_addr);
        if (rv) {
            printf("Error, bcm_ipmc_add index %d\n", i);
            return rv;
        }
    }

    return rv;
}

int ipv6_mc_default_route_entry_remove(int unit, int index) {

    int rv = 0;
    int vrf = 100;
    bcm_ipmc_addr_t ipmc_addr;
    bcm_ip6_t ip6_addr = { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11 };
    int prefix_length[3] = { 128, 64, 0 };
    char *kbp_dev_type = soc_property_get_str(unit, spn_EXT_TCAM_DEV_TYPE);
    int is_kbp = kbp_dev_type ? (!(sal_strcmp("NONE", kbp_dev_type)) ? 0 : 1) : 0;

    /* prefix length 0 cannot be configured in JR2.
     * IPv6 address group is not multicast - expecting the first byte to be 0xFF, instead it is 0x00
     */
    if (is_device_or_above(unit, JERICHO2)) {
        prefix_length[2] = 32;
    }

    if(index < 0 || index > 2) {
        return rv;
    }

    bcm_ipmc_addr_t_init(&ipmc_addr);
    ipv6_create_mask(ipmc_addr.mc_ip6_mask, prefix_length[index]);
    ipv6_and_mask_mc(ipmc_addr.mc_ip6_addr, ip6_addr, ipmc_addr.mc_ip6_mask);
    ipmc_addr.vrf = vrf;
    ipmc_addr.flags = BCM_IPMC_IP6;
    if (is_device_or_above(unit,JERICHO2) & is_kbp)
    {
        /* External lookup enabled on JR2 */
        ipmc_addr.flags |= BCM_IPMC_TCAM;
    }
    rv = bcm_ipmc_remove(unit, ipmc_addr);
    if (rv) {
        printf("Error, bcm_ipmc_add index %d\n", index);
        return rv;
    }

    return rv;
}

/*
 * This cint builds a basic IPV6 MC setup of 4 interfaces, 4 MC groups and 4 IPMC entries
 */
int NOF_BASIC_MC_GROUP = 4;
int compressed_sip = 0;
int bcm_l3_basic_ip6_mc(int unit, int port0, int port1, int port2, int port3) {
    bcm_l3_intf_t l3if[NOF_BASIC_MC_GROUP];
    bcm_l3_intf_t intfout;
    bcm_l3_egress_t l3egr;
    bcm_mac_t mac_l3_ingress_add = {0x00, 0x00, 0x00, 0x00, 0x55, 0x00};
    bcm_mac_t mac_l3_engress_add = {0x00, 0x00, 0x00, 0x00, 0x66, 0x00};
    int  vlan_bais_in    = 0x100;
    int  vlan_offset_in  = 0x100;

    int ipmc_index_offset;
    int ipmc_index_upper_bound;

    int i,j,found, rc;
    uint8 mac_last_byte[NOF_BASIC_MC_GROUP];/*All interfaces has the same my MAC/dest MAC differ only in the last byte */
    int exPorts[4] = {port0 , port1, port2 , port3};
    int ipmc_group[NOF_BASIC_MC_GROUP];
    int out_ports[NOF_BASIC_MC_GROUP] = {3,2,2,3};
    int flags, vlan;
    bcm_multicast_replication_t replications[NOF_BASIC_MC_GROUP];
    bcm_l3_ingress_t l3_ing_if;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;
    bcm_if_t encap_id;
    bcm_if_t l3egid_1;
    bcm_vlan_port_t port_vlan;

    int fecIds[3]    = {32769, 32771, 32773};

    if (is_device_or_above(unit, JERICHO2)) {
        ipmc_index_offset = 0;
        ipmc_index_upper_bound = *(dnxc_data_get(unit,  "multicast", "params", "max_ing_mc_groups", NULL));
    } else {
        ipmc_index_offset = diag_test_mc(unit,"get_nof_egress_bitmap_groups");
        ipmc_index_upper_bound = diag_test_mc(unit,"get_nof_ingress_mcids");
    }

    /*Set each interface parameters*/
    mac_last_byte[0] = 0x10;
    for (i=1; i < NOF_BASIC_MC_GROUP; i++) {
        mac_last_byte[i] = mac_last_byte[i-1]+0x20;
    }

    /*create different IPMC groups */
    sal_srand(sal_time());
    for (i=0; i < NOF_BASIC_MC_GROUP; i++) {  /* picking random values for the mc groups */
        found = 1;
        while (found){ /* verifying that we don't get a previous value */
            found = 0;
            ipmc_group[i] = (sal_rand() % ipmc_index_upper_bound) + ipmc_index_offset;
            for(j=0 ; j < i-1 ;j++) {
                if(ipmc_group[i] == ipmc_group[j]) {found = 1;}
            }
        }
    }

    for (i = 0; i < NOF_BASIC_MC_GROUP; i++) {
        /*Create L3 interface*/
        bcm_l3_intf_t_init(&l3if[i]);

        mac_l3_ingress_add[5] = mac_last_byte[i];
        sal_memcpy(l3if[i].l3a_mac_addr, mac_l3_ingress_add, 6);
        l3if[i].l3a_vid = l3if[i].l3a_vrf = vlan_bais_in + (i * vlan_offset_in);
        rc = bcm_l3_intf_create(unit, &l3if[i]);
        if ((rc != BCM_E_NONE) && (rc != BCM_E_EXISTS)) {
           return rc;
        }

        /* set L3 interface attributes */
        if (!is_device_or_above(unit, JERICHO2) && compressed_sip)
        {
            bcm_l3_ingress_t_init(&l3_ing_if);
            l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;
            l3_ing_if.intf_class = 1; /* RIF enable IPv6 MC SSM */
            l3_ing_if.vrf = vlan_bais_in + (i * vlan_offset_in);
            rc = bcm_l3_ingress_create(unit, &l3_ing_if, &l3if[i].l3a_intf_id);
            if (rc != BCM_E_NONE) {
                printf("Error in : bcm_l3_ingress_create\n");
                return rc;
            }
        }

        /* In the following 4 calls we create the L3 MC Group with an index taken from the main struct's data structure */
        flags = is_device_or_above(unit, JERICHO2) ? (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID) : (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID);
        rc = bcm_multicast_create(unit, flags, &(ipmc_group[i]));
        if (rc != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rc;
        }

        for (j = 0; j < out_ports[i]; j++) {
            bcm_multicast_replication_t_init(&replications[j]);
            replications[j].encap1 = l3if[i].l3a_intf_id;
            replications[j].port = exPorts[j + 1];
        }

        flags = is_device_or_above(unit, JERICHO2) ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_INGRESS;
        rc = bcm_multicast_set(unit, ipmc_group[i], flags, out_ports[i], replications);
        if (rc) {
            printf("Error, bcm_multicast_set index %d\n", i);
            return rc;
        }
    }

    bcm_ipmc_addr_t data;

    bcm_ip6_t route = {0xFF,0xFF,0x16,0,0x35,0,0x70,0,0,0,0xdb,0x7,0,0,0,0};
    bcm_ip6_t full_mask  = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    for (i = 0; i < NOF_BASIC_MC_GROUP; i++) {
        bcm_ipmc_addr_t_init(&data);
        sal_memcpy(data.mc_ip6_addr, route, 16);
        sal_memcpy(data.mc_ip6_mask, full_mask, 16);
        data.flags = BCM_IPMC_IP6;
        for (j = 0; j < 16; j++) {
            data.s_ip6_addr[j] = 0xA;
        }

        data.vid = data.vrf = vlan_bais_in + (i * vlan_offset_in);
        data.group = ipmc_group[i];

        /* Create IPv6 MC entries */
        rc = bcm_ipmc_add(unit, &data);
        if (rc != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip_sip \n");
            return rc;
        }

        /* Create IPv6 MC SSM entries */
        if (!is_device_or_above(unit, JERICHO2) && compressed_sip)
        {
            data.s_ip_addr = compressed_sip;
            rc = bcm_ipmc_add(unit, &data);
            if (rc != BCM_E_NONE) {
                printf("Error, create_forwarding_entry_dip_sip \n");
                return rc;
            }
        }
        route[15] += 0x10;

        /** Call API to define the cascaded lookup - compressed SIP result */
        if (!is_device_or_above(unit, JERICHO2) && compressed_sip)
        {
            sal_memcpy(data.s_ip6_mask, full_mask, 16);
            bcm_ipmc_addr_t config;
            bcm_ipmc_addr_t_init(&config);
            config.s_ip_addr = compressed_sip;
            config.s_ip6_addr = data.s_ip6_addr;
            config.s_ip6_mask = data.s_ip6_mask;
            config.vid = data.vid;
            rc = bcm_ipmc_config_add(unit, &config);
            if (rc != BCM_E_NONE)
            {
                printf("Error, bcm_ipmc_config_add \n");
                return rc;
            }
        }
    }

    return rc;
}

int ipv6_mc_traverse_index = 0;
int ipv6_mc_prefix_traverse(int unit, bcm_ipmc_addr_t *info, void *user_data)
{
    if (ipv6_mc_traverse_index > 128/4+1) {
        printf ("ipv6_mc_prefix_traverse, index out of range.\n");
        traverse_ipv6_mc_rc = BCM_E_FAIL;
        return traverse_ipv6_mc_rc;
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        BCM_MULTICAST_L3_SET(info->group,info->group);
    }

    if (info->group != l3rts_ipv6_mc_traverse[ipv6_mc_traverse_index].group) {
        printf ("ipv6_mc_prefix_traverse, l3a_intf: Expected 0x%x, entry 0x%x\n", l3rts_ipv6_mc_traverse[ipv6_mc_traverse_index].group, info->group);
        traverse_ipv6_mc_rc = BCM_E_FAIL;
    }

    if (info->vrf != l3rts_ipv6_mc_traverse[ipv6_mc_traverse_index].vrf) {
        printf ("ipv6_mc_prefix_traverse, l3a_vrf: Expected %d, entry %d\n", l3rts_ipv6_mc_traverse[ipv6_mc_traverse_index].vrf, info->vrf);
        traverse_ipv6_mc_rc = BCM_E_FAIL;
    }

    if (info->flags != l3rts_ipv6_mc_traverse[ipv6_mc_traverse_index].flags) {
        printf ("ipv6_mc_prefix_traverse, l3a_flags: Expected 0x%x, entry 0x%x\n", l3rts_ipv6_mc_traverse[ipv6_mc_traverse_index].flags, info->flags);
        traverse_ipv6_mc_rc = BCM_E_FAIL;
    }

    int i;
    for (i = (is_device_or_above(unit, JERICHO2) ? 2 : 0); i < 16; i++) {
        if (info->mc_ip6_addr[i] != l3rts_ipv6_mc_traverse[ipv6_mc_traverse_index].mc_ip6_addr[i]) {
            printf ("ipv6_mc_prefix_traverse, l3a_ip6_net[%d]: Expected 0x%x, entry 0x%x\n", i, l3rts_ipv6_mc_traverse[ipv6_mc_traverse_index].mc_ip6_addr[i], info->mc_ip6_addr[i]);
            traverse_ipv6_mc_rc = BCM_E_FAIL;
        }

        if (info->mc_ip6_mask[i] != l3rts_ipv6_mc_traverse[ipv6_mc_traverse_index].mc_ip6_mask[i]) {
            printf ("ipv6_mc_prefix_traverse, l3a_ip6_mask[%d]: Expected 0x%x, entry 0x%x\n", i, l3rts_ipv6_mc_traverse[ipv6_mc_traverse_index].mc_ip6_mask[i], info->mc_ip6_mask[i]);
            traverse_ipv6_mc_rc = BCM_E_FAIL;
        }

    }

    traverse_ipv6_mc_route_counter++;

    printf ("IPv6 MC Route traverse index: %d verified\n", ipv6_mc_traverse_index);
    ipv6_mc_traverse_index++;
    return 0;
}

/*
 *  The purpose of this cint is to create:
 *  33 interfaces (at the most) with different VRFs and MACs
 *  33 IPv6 MC routes with the same IPV6 DIP and VRF, but different prefix length.
 *     Each route is associated with a different MC group
 *     Each MC group points to a different interface, and up to 4 different ports
 */
int NOF_IPV6_MC_PREFIX_INTERFACES = 128/4+1; /* Max number of interfaces */
bcm_ipmc_addr_t l3rts_ipv6_mc_traverse[NOF_IPV6_MC_PREFIX_INTERFACES];
int l3rts_ipv6_mc_group[NOF_IPV6_MC_PREFIX_INTERFACES];
int traverse_ipv6_mc_rc = 0;
int traverse_ipv6_mc_route_counter = 0;

/* table_id 0 is KAPS, 1 is TCAM */
int ipv6_mc_prefix(int unit, int port0, int port1, int port2, int port3, int table_id) {
    int rc, i, j;
    bcm_l3_intf_t l3if;
    bcm_if_t l3egid;
    bcm_mac_t mac_l3_ingress_add = {0x00, 0x00, 0x00, 0x00, 0x55, 0x00};
    bcm_mac_t mac_l3_engress_add = {0x00, 0x00, 0x00, 0x00, 0x66, 0x00};
    int ipmc_index_offset;
    int ipmc_index_upper_bound;
    int found;
    char *kbp_dev_type = soc_property_get_str(unit, spn_EXT_TCAM_DEV_TYPE);
    int is_kbp = kbp_dev_type ? (!(sal_strcmp("NONE", kbp_dev_type)) ? 0 : 1) : 0;

    int vlan[NOF_IPV6_MC_PREFIX_INTERFACES];/*Each interface VLAN and VRF */
    uint8 mac_last_byte[NOF_IPV6_MC_PREFIX_INTERFACES];/*All interfaces has the same my MAC/dest MAC differ only in the last byte */
    int ports[NOF_IPV6_MC_PREFIX_INTERFACES];
    int flags;
    bcm_multicast_replication_t replications[4];

    if (is_device_or_above(unit, JERICHO2)) {
        ipmc_index_offset = 0;
        ipmc_index_upper_bound = *(dnxc_data_get(unit,  "multicast", "params", "max_ing_mc_groups", NULL));
    } else {
        ipmc_index_offset = diag_test_mc(unit,"get_nof_egress_bitmap_groups");
        ipmc_index_upper_bound = diag_test_mc(unit,"get_nof_ingress_mcids");
    }

    /*Set each interface parameters*/
    vlan[0] = 2000;
    mac_last_byte[0] = 0x10;
    for (i=1; i < NOF_IPV6_MC_PREFIX_INTERFACES; i++) {
        vlan[i] = vlan[i-1]+10;
        mac_last_byte[i] = (mac_last_byte[i-1]+0x5) % 0x100;
    }

    ports[0] = port0;
    ports[1] = port1;
    ports[2] = port2;
    ports[3] = port3;

    /*create different IPMC groups */
    sal_srand(sal_time());
    for (i=0; i < NOF_IPV6_MC_PREFIX_INTERFACES; i++) {  /* picking random values for the mc groups */
        found = 1;
        while (found){ /* verifying that we don't get a previous value */
            found = 0;
            l3rts_ipv6_mc_group[i] = (sal_rand() % ipmc_index_upper_bound) + ipmc_index_offset;
            if (i > 0 ) {
                for(j=0 ; j < i-1 ;j++) {
                    if(l3rts_ipv6_mc_group[i] == l3rts_ipv6_mc_group[j]) {found = 1;}
                }
            }
        }
    }

    for (i = 0; i < NOF_IPV6_MC_PREFIX_INTERFACES; i++) {
        /*Create L3 interface*/
        bcm_l3_intf_t_init(&l3if);

        mac_l3_ingress_add[5] = mac_last_byte[i];
        sal_memcpy(l3if.l3a_mac_addr, mac_l3_ingress_add, 6);
        l3if.l3a_vid = l3if.l3a_vrf = vlan[i];
        rc = bcm_l3_intf_create(unit, &l3if);
        if ((rc != BCM_E_NONE) && (rc != BCM_E_EXISTS)) {
           return rc;
        }

        /* In the following 4 calls we create the L3 MC Group with an index taken from the main struct's data structure */
        flags = is_device_or_above(unit, JERICHO2) ? (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID) : (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID);
        rc = bcm_multicast_create(unit, flags, &(l3rts_ipv6_mc_group[i]));
        if (rc != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rc;
        }

        for (j = 0; j < (i % 4) + 1; j++) {
            bcm_multicast_replication_t_init(&replications[j]);
            replications[j].encap1 = l3if.l3a_intf_id;
            replications[j].port = ports[j];
        }

        flags = is_device_or_above(unit, JERICHO2) ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_INGRESS;
        rc = bcm_multicast_set(unit, l3rts_ipv6_mc_group[i], flags, j, replications);
        if (rc) {
            printf("Error, bcm_multicast_set index %d\n", i);
            return rc;
        }
    }

    bcm_ip6_t ip6;
    ip6[0] = 0xFF;
    ip6[1] = 0x02;
    ip6[2] = 0x56;
    ip6[3] = 0x78;
    ip6[4] = 0x9a;
    ip6[5] = 0xbc;
    ip6[6] = 0xde;
    ip6[7] = 0xf0;
    ip6[8] = 0x10;
    ip6[9] = 0x20;
    ip6[10] = 0x30;
    ip6[11] = 0x40;
    ip6[12] = 0x50;
    ip6[13] = 0x60;
    ip6[14] = 0x70;
    ip6[15] = 0x80;
    bcm_ip6_t ip6_mask;

    int interface_counter = 0;
    int prefix_length;

    for (prefix_length=128; prefix_length >= (is_device_or_above(unit, JERICHO2) ? 0x10 : 0); prefix_length-=4) {
        bcm_ipmc_addr_t_init(&l3rts_ipv6_mc_traverse[interface_counter]);
        ipv6_create_mask(l3rts_ipv6_mc_traverse[interface_counter].mc_ip6_mask, prefix_length);
        ipv6_and_mask_mc(l3rts_ipv6_mc_traverse[interface_counter].mc_ip6_addr, ip6, l3rts_ipv6_mc_traverse[interface_counter].mc_ip6_mask);
        l3rts_ipv6_mc_traverse[interface_counter].vrf = vlan[0];
        l3rts_ipv6_mc_traverse[interface_counter].group = l3rts_ipv6_mc_group[interface_counter];
        l3rts_ipv6_mc_traverse[interface_counter].flags = is_device_or_above(unit, JERICHO2) ? BCM_IPMC_IP6 : (BCM_IPMC_SOURCE_PORT_NOCHECK | BCM_IPMC_IP6);
        if (is_device_or_above(unit,JERICHO2) && is_kbp)
        {
            /* External lookup enabled on JR2 */
            l3rts_ipv6_mc_traverse[interface_counter].flags |= BCM_IPMC_TCAM;
        }
        rc = bcm_ipmc_add(unit, l3rts_ipv6_mc_traverse[interface_counter]);
        if (rc != BCM_E_NONE) {
          printf ("bcm_ipmc_add failed: %d \n", rc);
          return rc;
        }
        /* Return the DIP to the state it's going to be found in the table.
         * If the first byte of the mask is 0x00, then first byte of DIP in the table will be 0x00.
         */
        l3rts_ipv6_mc_traverse[interface_counter].mc_ip6_addr[0] = ip6[0] & l3rts_ipv6_mc_traverse[interface_counter].mc_ip6_mask[0];
        interface_counter++;
    }

    ipv6_mc_traverse_index = 0;
    traverse_ipv6_mc_route_counter = 0;
    rc = bcm_ipmc_traverse(unit, BCM_IPMC_IP6, ipv6_mc_prefix_traverse, NULL);
    if (rc != BCM_E_NONE) {
      printf ("bcm_l3_route_traverse failed: %d \n", rc);
      return rc;
    }

    if ((traverse_ipv6_mc_rc != BCM_E_NONE) || (traverse_ipv6_mc_route_counter != interface_counter)) {
      printf ("bcm_l3_route_traverse failed: rc=%d, actual_counter=%d, expected counter=%d.\n", traverse_ipv6_mc_rc, traverse_ipv6_mc_route_counter, interface_counter);
      return traverse_ipv6_mc_rc;
    }

    return rc;
}


int ipv6_mc_prefix_destory(int unit) {
    int rc, i;

    for (i=0; i < NOF_IPV6_MC_PREFIX_INTERFACES; i++) {
        rc = bcm_multicast_destroy(unit, l3rts_ipv6_mc_group[i]);
        if (rc != BCM_E_NONE) {
            printf("Error, bcm_multicast_destroy \n");
            return rc;
        }
    }

    return rc;
}

int
ipv6_mc_ssm_pmf_example(
    int unit,
    bcm_port_t recycle_port)
{
    int rv = BCM_E_NONE;
    bcm_field_qset_t  udh_qset;
    bcm_field_aset_t udh_aset;
    bcm_field_group_config_t udh_group;
    bcm_field_group_t group = 10;
    bcm_field_entry_t entry1 = 0;
    bcm_field_entry_t entry2 = 0;
    int i = 0;
    bcm_field_extraction_field_t ext_field[2];
    bcm_field_extraction_action_t extract;
    int presel_id;
    bcm_field_entry_t presel_flags = 0;
    bcm_field_presel_set_t presel_set;
    bcm_gport_t dst_gport;
    int eg_mc_id = 6000;
    int ing_mc_id = 5000;
    bcm_gport_t dest_ports[1] = {0};
    int ing_mc_cud[1] = {0};

    dest_ports[0] = recycle_port;

    /* Add a recycle port */
    rv = bcm_port_control_set(unit, recycle_port, bcmPortControlOverlayRecycle, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set(bcmPortControlOverlayRecycle)\n");
        return rv;
    }
    /* Create an Ingress MC group and add the final port destinations to it so they will be scheduled */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID, &ing_mc_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_multicast_create (ingress)\n");
        return rv;
    }

    /* Add destination ports to the ingress MC Group */
    rv = bcm_multicast_ingress_set (unit, ing_mc_id, 1, dest_ports, ing_mc_cud);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_multicast_ingress_set\n");
        return rv;
    }

    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_presel_create_stage_id, rv = %d\n", rv);
            return rv;
        }
    } else {
        rv = bcm_field_presel_create(unit, &(presel_id));
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_presel_create, rv = %d\n", rv);
            return rv;
        }
    }

    presel_flags |= BCM_FIELD_QUALIFY_PRESEL;
    rv = bcm_field_qualify_Stage(unit, presel_id | presel_flags, bcmFieldStageIngress);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_Stage, rv = %d\n", rv);
        return rv;
    }

    rv = bcm_field_qualify_ForwardingType(unit, presel_id | presel_flags, bcmFieldForwardingTypeIp6Mcast);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_ForwardingType, rv = %d\n", rv);
        return rv;
    }

    rv = bcm_field_qualify_AppType(unit, presel_id | presel_flags, bcmFieldAppTypeIp6McastL3SsmCompressSip);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_AppType, rv = %d\n", rv);
        return rv;
    }

    /* Define the preselector-set */
    BCM_FIELD_PRESEL_INIT(presel_set);
    BCM_FIELD_PRESEL_ADD(presel_set, presel_id);

    BCM_FIELD_QSET_INIT(udh_qset);
    BCM_FIELD_QSET_ADD(udh_qset, bcmFieldQualifyL3DestRouteHit);
    BCM_FIELD_QSET_ADD(udh_qset, bcmFieldQualifyL3DestRouteValue);

    BCM_FIELD_ASET_INIT(udh_aset);
    BCM_FIELD_ASET_ADD(udh_aset, bcmFieldActionRedirect);
    BCM_FIELD_ASET_ADD(udh_aset, bcmFieldActionClassDestSet);

    bcm_field_group_config_t_init(&udh_group);
    udh_group.group = group;
    udh_group.priority = group;
    udh_group.mode = bcmFieldGroupModeDirectExtraction;
    udh_group.flags = BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_MODE
                      | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    udh_group.aset = udh_aset;
    udh_group.qset = udh_qset;
    udh_group.preselset = presel_set;
    rv = bcm_field_group_config_create(unit, &udh_group);
    if (rv != BCM_E_NONE) {
        printf("Error Field, bcm_field_group_config_create\n");
        return rv;
    }

    rv = bcm_field_entry_create(unit, udh_group.group, &entry1);
    if (rv != BCM_E_NONE) {
        printf("Error Field, bcm_field_entry_create\n");
        return rv;
    }

    /* Redirect packt to recycle port for Second pass */
    extract.action = bcmFieldActionRedirect;
    extract.bias = 0;

    bcm_field_extraction_field_t_init(ext_field);
    ext_field[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    ext_field[0].bits = 17;
    ext_field[0].lsb = 0;
    ext_field[0].value = ing_mc_id;
    ext_field[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    ext_field[1].bits = 2;
    ext_field[1].lsb = 0;
    ext_field[1].value = 2;       /* mc type */
    rv = bcm_field_direct_extraction_action_add(unit, entry1, extract, 2, &ext_field);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_direct_extraction_action_add failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_entry_install(unit, entry1);
    if (rv != BCM_E_NONE) {
        printf("Error Field, bcm_field_entry_install\n");
        return rv;
    }

    rv = bcm_field_entry_create(unit, udh_group.group, &entry2);
    if (rv != BCM_E_NONE) {
        printf("Error Field, bcm_field_entry_create\n");
        return rv;
    }

    /* set CH.iPMF-Opaque(16) to UDH */
    extract.action = bcmFieldActionClassDestSet;
    extract.bias = 0;

    bcm_field_extraction_field_t_init(ext_field);
    ext_field[0].flags = 0;
    ext_field[0].bits  = 16;
    ext_field[0].lsb = 0;
    ext_field[0].qualifier = bcmFieldQualifyL3DestRouteValue;
    rv = bcm_field_direct_extraction_action_add(unit, entry2, extract, 1, &ext_field);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_direct_extraction_action_add failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_entry_install(unit, entry2);
    if (rv != BCM_E_NONE) {
        printf("Error Field, bcm_field_entry_install\n");
        return rv;
    }

    /* for debug */
    rv = bcm_field_group_dump(unit, group);
    if (rv != BCM_E_NONE) {
        printf("Error Field, bcm_field_group_dump\n");
        return rv;
    }

    return rv;
}

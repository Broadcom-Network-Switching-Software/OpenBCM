/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Configuration:
 * 
 * cint ../../../../src/examples/dnx/multicast/cint_multicast_pp_basic.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int in_port = 200;
 * int out_port1 = 201;
 * int out_port2 = 202;
 * int out_port3 = 203;
 * int nof_replications = 2;
 * rv = multicast_pp_basic(unit,in_port,out_port1,out_port2,out_port3,nof_replications);
 * print rv;
 */

int multicast_pp_basic(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int nof_replications)
{
    int rv;
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 0x3000FF; /* Outgoing packet ETH-RIF */
    int vrf = 1;
    int idx;
    int replication_list[3] = {0x1500, 0x1501, 0x1502};
    bcm_mac_t intf_in_mac_address = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a }; /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x01, 0x00, 0x5E, 0x02, 0x02, 0x02 };
    uint32 host = 0xE0020202;
    uint32 sip = 0xc0800101;
    uint32 mask = 0xffffffff;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags = BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID;
    int mc_group = 5500;
    int encap_id = 0x1500;
    int nof_cuds;
    int label = 0x246;
    int out_ports[3] = {out_port1, out_port2, out_port3};
    int max_nof_outlifs = 3;
    bcm_multicast_replication_t replications[nof_replications];

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set intf_out\n");
        return rv;
    }
    rv = out_port_set(unit, out_port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set intf_out\n");
        return rv;
    }
    rv = out_port_set(unit, out_port3);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set intf_out\n");
        return rv;
    }
    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set ingress_rif\n");
        return rv;
    }

    /*
     * 5. Create out interface which will be used for the MPLS ARP.
     */
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("intf_eth_rif_create\n");
        return rv;
    }
    /*
     * 6. Create an ARP which is going to be used for the MPLS tunnel initiator.
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_mpls_encapsulation_basic_info.arp_id_push), arp_next_hop_mac,
            intf_out);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only, arp_id\n");
        return rv;
    }
    intf_out++;

    /*
     * 7. Create the MPLS tunnel and assign the tunnel ID to be the first in the replication array.
     */
    cint_mpls_encapsulation_basic_info.pushed_or_swapping_label = label;
    rv = mpls_encapsulation_basic_create_push_or_swap_tunnel(unit, 64);
    if (rv != BCM_E_NONE)
    {
        printf("Error, mpls_encapsulation_basic_create_push_or_swap_tunnel - (%d)\n", rv);
        return rv;
    }
    printf("TUNNEL ID = %d\n\n", cint_mpls_encapsulation_basic_info.push_tunnel_id);
    replication_list[0] = cint_mpls_encapsulation_basic_info.push_tunnel_id;

    /*
     * 8. Create multicast replications
     */
    for (idx = 0; idx < nof_replications; idx++)
    {
        /*
         * 8.1 Create an L3 interface
         */
        /*
         * in case intf_out == eth_rif_id it should be already created
         */
        if (intf_out != intf_in)
        {
            rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
            if (rv != BCM_E_NONE)
            {
                printf("intf_eth_rif_create\n");
                return rv;
            }
        }
        /*
         * 8.2 Create an ARP for this intf_out that will be used in the list of replications of the PPMC entry
         */
        rv = l3__egress_only_encap__create(unit, 0, &replication_list[1], arp_next_hop_mac, intf_out);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object ARP only, arp_id\n");
            return rv;
        }
        /*
         * 8.3 Set the replication.
         */
        set_multicast_replication(&replications[idx], out_ports[idx], intf_out);

        /*
         * 8.4 Create the PPMC entry for this replication
         */
        /** The number of outLIFs for an entry is determined as 2 for the first replication and 3 for all others */
        nof_cuds = 2;
        rv = bcm_multicast_encap_extension_create(unit, flags, intf_out, nof_cuds, replication_list);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function bcm_multicast_encap_extension_create, \n");
            return rv;
        }

        /*
         * 8.5 Set some changes for the next replication.
         */
        intf_out++;
        replication_list[1]++;
        arp_next_hop_mac[5]++;
        intf_out_mac_address[5]++;
    }

    /*
     * 9. Create a multicast group
     */
    rv = create_multicast(unit, replications, nof_replications, mc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function create_multicast, \n");
        return rv;
    }

    /*
     * 10. Create an IPMC host entry which results to the created MC group.
     */
    rv = add_ipv4_ipmc(unit, host, mask, sip, mask, intf_in, vrf, mc_group, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv4_ipmc to LEM table, \n");
        return rv;
    }

    return rv;
}

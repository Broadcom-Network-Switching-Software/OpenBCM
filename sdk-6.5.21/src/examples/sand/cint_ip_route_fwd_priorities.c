/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Utilities APIs
 */


int with_urpf_loose_check = 0;
int with_urpf_strict_check = 0;

/******* Run example ******/
/*
 * in order to run this example the cint "cint_ip_route_basic.c" has to be loaded.
 * This is a function that would set the configuration for forwarding priority check both
 * for IPv6 and IPv4.
 */
int
lem_kaps_priority_check_tests_config(
    int unit,
    int port1,
    int port2,
    int port3,
    int port4,
    int support_public_lookup)
{
    int rv;
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int vrf = 1;
    int encap_id = 900;         /* ARP-Link-layer */
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    int i, eg_id;
    int fec = 40961;
    bcm_l3_egress_t egress_l3_interface;
    int ports[4];
    int nof_fecs = 3;


    if (is_device_or_above(unit, JERICHO2))
    {
        /*
         * encap id for jr2 must be > 2k
         */
        encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;

        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range\n");
            return rv;
        }
    }
    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, port1, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * 2. Set Out-Port default properties
     */
    rv = out_port_set(unit, port2);
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
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * 4. Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &encap_id, arp_next_hop_mac, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    /*
     * create ingress interface which supports public lookup
     */
    bcm_l3_ingress_t ingress_l3_interface;
    bcm_l3_ingress_t_init(&ingress_l3_interface);
    if (support_public_lookup)
    {
        ingress_l3_interface.flags = BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_GLOBAL_ROUTE;
    }
    else
    {
        ingress_l3_interface.flags = BCM_L3_INGRESS_WITH_ID;
    }
    ingress_l3_interface.vrf = vrf;
    if (with_urpf_loose_check == 1)
    {
        ingress_l3_interface.urpf_mode = bcmL3IngressUrpfLoose;
    }
    else if (with_urpf_strict_check == 1)
    {
        ingress_l3_interface.urpf_mode = bcmL3IngressUrpfStrict;
    }

    rv = bcm_l3_ingress_create(unit, ingress_l3_interface, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }

    ports[0] = port1;
    ports[1] = port2;
    ports[2] = port3;
    ports[3] = port4;
    if (with_urpf_loose_check == 1 || with_urpf_strict_check == 1)
    {
        nof_fecs = 4;
    }

    /*
     * create 3 fecs with 3 different ports
     */
    for (i = 0; i < nof_fecs; i++)
    {
        bcm_l3_egress_t_init(&egress_l3_interface);
        eg_id = fec + 2 * i;
        egress_l3_interface.port = ports[i];
        if ((with_urpf_loose_check == 1 || with_urpf_strict_check == 1) && i > 0)
        {
            egress_l3_interface.intf = intf_in;
        }
        else
        {
            egress_l3_interface.intf = intf_out;
        }
        egress_l3_interface.encap_id = encap_id;
        bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &egress_l3_interface, &eg_id);
    }

    return rv;
}

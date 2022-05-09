/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * FL-L3-IPV4-MC
 */
/*
 *
 * Create 3 MC groups and their replications.
 * Create 7 out-RIFs and config different ttl tresholds for each one of them.
 * If packet TTL=21 is larger than the out-RIF TTL threshold 20, then the packet will not be filtered.
 * If the packet TTL is lower or equal the out_RIF treshold , the packet will be filtered.
 *
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/cint_ipmc_route_basic.c
 * cint ../../../../src/examples/sand/cint_ipmc_with_ttl.c
 * cint
 * int rv;
 * rv = run_with_ports_ipv4_mc_example_with_ttl_extended(0, 200, 201, 202, 203);
 * print rv;
 * exit;
 *
 * TTL = 26
 * tx 1 psrc=200 data=0x01005e60e00100000000000181000002080045000035000000001a111e66c0800101e0e0e001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * If packet TTL=26 is larger than the TTL threshold of the 3 Out-RIFs, then the received packets on unit 0 should be:
 * Source port: 200, Destination port: 201
 * Data: 0x01005e60e001000000111111810000140800450000350000000019111f66c0800101e0e0e001000102030405060708090a0b0c0d0e0f10111213141516171819
 * Source port: 200, Destination port: 202
 * Data: 0x01005e60e001000000111122810000150800450000350000000019111f66c0800101e0e0e001000102030405060708090a0b0c0d0e0f10111213141516171819
 * Source port: 200, Destination port: 203
 * Data: 0x01005e60e001000000111133810000160800450000350000000019111f66c0800101e0e0e001000102030405060708090a0b0c0d0e0f10111213141516171819
 *
 */
int
run_with_ports_ipv4_mc_example_with_ttl_extended(
    int unit,
    int iport,
    int oport1,
    int oport2,
    int oport3)
{
    uint32 rv = BCM_E_NONE;
    bcm_gport_t dst_gport;

    /* Set parameters: User can set different parameters. */
    bcm_ip_t mc_ip = 0xE0E0E001; /* 224.224.224.1 */
    bcm_ip_t src_ip = 0;
    bcm_ip_t mc_ip_2 = 0xE0E0E002; /* 224.224.224.2 */
    bcm_ip_t mc_ip_3 = 0xE0E0E003; /* 224.224.224.3 */
    int vlan = 2;
    int vrf = 2;
    int ipmc_index = 6000;
    int dest_local_port_id = iport;
    int source_local_port_id = oport1, ports[3];
    int nof_dest_ports = 3;
    int intf_out[3];
    int idx;
    bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0xab, 0x1d};
    bcm_mac_t mac_l3_out[3] = {{0x00, 0x00, 0x00, 0x11, 0x11, 0x11}, {0x00, 0x00, 0x00, 0x11, 0x11, 0x22}, {0x00, 0x00, 0x00, 0x11, 0x11, 0x33}};
    l3_ingress_intf ingress_itf;
    bcm_multicast_replication_t replications[3];
    l3_ingress_intf ingr_itf;

    ports[0] = oport1;
    ports[1] = oport2;
    ports[2] = oport3;

    /** 1. Set In-Port to In ETh-RIF
     ** 2. Set Out-Port*/
    rv = in_port_intf_set(unit, iport, vlan);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    rv = out_port_set(unit, oport1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set\n");
        return rv;
    }

    rv = out_port_set(unit, oport2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set\n");
        return rv;
    }

    rv = out_port_set(unit, oport3);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set\n");
        return rv;
    }
    /** 2. Create ETH-RIF and set its properties*/
    rv = intf_eth_rif_create(unit, vlan, mac_l3_ingress);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }
    /** 3. Set Incoming ETH-RIF properties*/
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = vlan;
    ingr_itf.vrf = vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ingr_eth_rif_create intf_out\n");
        return rv;
    }
    intf_out[0] = 20;
    intf_out[1] = 21;
    intf_out[2] = 22;
    for(idx = 0; idx < nof_dest_ports; idx++)
    {   /** The ttl treshold is equal to inf_out value*/
        rv = intf_eth_rif_create_with_ttl(unit, intf_out[idx], mac_l3_out[idx], intf_out[idx]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, intf_eth_rif_create intf_out=%d\n",intf_out[idx]);
            return rv;
        }

        set_multicast_replication(&replications[idx], ports[idx], intf_out[idx]);
    }
    rv = create_multicast(unit, replications, nof_dest_ports, ipmc_index);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function create_multicast, \n");
        return rv;
    }
    rv = add_ipv4_ipmc(unit, mc_ip, 0xFFFFFFFF, src_ip, 0, vlan, vrf, ipmc_index, 0, 0);
    if (rv != BCM_E_NONE){
        printf();
        return rv;
    }

    intf_out[0] = 30;
    intf_out[1] = 31;
    intf_out[2] = 32;
    ipmc_index++;
    for(idx = 0; idx < nof_dest_ports; idx++)
    {
        rv = intf_eth_rif_create_with_ttl(unit, intf_out[idx], mac_l3_out[idx], intf_out[idx]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, intf_eth_rif_create intf_out\n");
            return rv;
        }
        set_multicast_replication(&replications[idx], ports[idx], intf_out[idx]);
    }
    rv = create_multicast(unit, replications, nof_dest_ports, ipmc_index);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function create_multicast, \n");
        return rv;
    }
    rv = add_ipv4_ipmc(unit, mc_ip_2, 0xFFFFFFFF, src_ip, 0, vlan, vrf, ipmc_index, 0, 0);
    if (rv != BCM_E_NONE){
        printf();
        return rv;
    }

    ipmc_index++;
    intf_out[0] = 40;
    rv = intf_eth_rif_create_with_ttl(unit, intf_out[0], mac_l3_out[0], intf_out[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out. The last intf_out create\n");
        return rv;
    }
    set_multicast_replication(&replications[0], ports[0], intf_out[0]);

    rv = create_multicast(unit, replications, 1, ipmc_index);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function create_multicast, \n");
        return rv;
    }
    rv = add_ipv4_ipmc(unit, mc_ip_3, 0xFFFFFFFF, src_ip, 0, vlan, vrf, ipmc_index, 0, 0);
    if (rv != BCM_E_NONE){
        printf();
        return rv;
    }

    return rv;
}


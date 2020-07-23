/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Multi-Dev IPv4 UC~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_multi_device_ipv4_route_uc.c
 * Purpose: Demo IPv4 UC under multi device system
 *
 * Multi-Device system require the ingress/egress configuration separately and relatively. 
 * This cint demo how to configurer the IPv4 service under multi-device system.
 *  1. Create ARP and out-RIF configuration on egress device
 *  2. Create In-RIF and Forward configuration on ingress device
 *  3. Send IPv4 packet to verify the result

 * Example:
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_sand_multi_device_ipv4_uc.c
 * cint ../../src/examples/dpp/internal/systems/cint_utils_jr2-comp-jr1-mode.c
 * cint
 * initialize_required_system_tools(0);
 * cint_ipv4_route_info.mode=0/1/2;
 * multi_dev_ipv4_uc_example(0,0,1811939531,1811939530);
 * Send packets on unit 1 should be: 
 * tx 1 psrc=203 data=0x0000000000110000000000008100706408004500004c00000000403dfb71000000007fffff04000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 * Received packets on unit 2 should be: 
 *  Data: 0x00000022000000000011810000c808004500004c000000003f3dfc71000000007fffff04000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b000000000000 
 */


 struct cint_multi_dev_ipv4_route_info_s 
{
    int in_rif;                             /* access RIF */
    int out_rif;                            /* out RIF */
    int vrf;                                /*  VRF */
    bcm_mac_t my_mac;                       /* mac for Incoming L3 intf */
    bcm_mac_t next_hop_mac;                 /* next hop mac for outgoing station */
    bcm_ip_t dip;                           /* dip */

    int arp_encap_id;                       /*Next hop intf ID*/ 
    int level1_fec_id;                      /*FEC ID for routing into IP*/

    /*
     * FEC usage 
     * 0:1 level FEC with OutRIF+ARP+Dest (Format A)
     * 1:1 level FEC with Out-LIF+Dest (Format B)
     * 2:1 level FEC with EEI+Dest (Format C)
     */
    int mode;   
    int in_local_port;                           
    int out_local_port;  
    int mtu;

};
 
 
 cint_multi_dev_ipv4_route_info_s cint_ipv4_route_info=
{
    /*
    * In-RIF,
    */
    100,
    /*
    * Out-RIF,
    */
    200,

    /*
    * vrf,
    */
    1,
    /*
    * my_mac | next_hop_mac 
    */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x11},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x22},

    /*
    * dip
    */
    0x7FFFFF04, /* 127.255.255.4 */

};

/**
 * Convert system port to local port
*/

int
multi_dev_system_port_to_local(int unit, int sysport, int *local_port){
    int rv = BCM_E_NONE;
    int my_modid, gport_modid;
    int gport;
    int mod_port_id;


    rv = bcm_stk_sysport_gport_get(unit, sysport, &gport);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_sysport_gport_get\n");
        return rv;
    }
    printf("Sysport's gport is: 0x%x\n", gport);
    gport_modid = BCM_GPORT_MODPORT_MODID_GET(gport);
    mod_port_id = BCM_GPORT_MODPORT_PORT_GET(gport);
    printf("Required modid(core) is: 0x%x\n", gport_modid);
    printf("Sysport's local port is: 0x%x\n", mod_port_id);

    BCM_GPORT_LOCAL_SET(*local_port,mod_port_id);


    return rv;
}


/**
 * Main entrance for mpls check ver configuration
 * INPUT:
 *   ingress_unit- traffic incoming unit 
 *   egress_unit- traffic ougtoing unit 
 *   in_sys_port- traffic incoming port 
 *   out_sys_port- traffic outgoing port 
*/
 int
 multi_dev_ipv4_uc_init(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv;
    int local_port,is_local;

    /*
     * 0: Convert the sysport to local port
     */

    multi_dev_system_port_to_local(ingress_unit,in_sys_port,&cint_ipv4_route_info.in_local_port);


    multi_dev_system_port_to_local(egress_unit,out_sys_port,&cint_ipv4_route_info.out_local_port);



    /*
     * 1.EncapID, use the minimal valid 
     *   global Out-LIF for JR and JR2
     */
    BCM_L3_ITF_SET(cint_ipv4_route_info.arp_encap_id,BCM_L3_ITF_TYPE_LIF, 0x3000);

    /*
     *2.FEC ID
     */
    if (is_device_or_above(ingress_unit, JERICHO2)) {
        int fec_id;
        rv = get_first_fec_in_range_which_not_in_ecmp_range(ingress_unit, 0, &fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        BCM_L3_ITF_SET(cint_ipv4_route_info.level1_fec_id, BCM_L3_ITF_TYPE_FEC, fec_id);
    } else {
        BCM_L3_ITF_SET(cint_ipv4_route_info.level1_fec_id, BCM_L3_ITF_TYPE_FEC, 0x1000);
    }

    return rv;
}


/**
 * port init
 * INPUT:
 *   in_port    - traffic incoming port 
*/
 int
 multi_dev_ingress_port_configuration(
    int unit,
    int in_port,
    int in_rif)
{
    int rv;

    rv = in_port_intf_set(unit, in_port, in_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_ingress_port_configuration \n");
        return rv;
    }
    return rv;

}

/**
 * port egress init
 * INPUT:
 *   out_port - traffic outgoing port
*/
 int
 multi_dev_egress_port_configuration(
    int unit,
    int out_port)
{
    int rv;

    /*
     * 1. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf(" Error, out_port_set intf_out out_port %d\n",out_port);
        return rv;
    }
    return rv;

}

 
/**
 * L3 intf init
 * INPUT:
 *   in_port    - traffic incoming port 
*/
 int
 multi_dev_ipv4_uc_l3_intf_configuration(
    int unit,
    int in_port)
{
    int rv;
    bcm_l3_intf_t l3_intf;
    sand_utils_l3_eth_rif_s eth_rif_structure;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_ipv4_route_info.in_rif;
    rv = bcm_l3_intf_get(unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_ipv4_route_info.in_rif, 0, 0, cint_ipv4_route_info.my_mac, cint_ipv4_route_info.vrf);
        rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
         printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
         return rv;
        }
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_ipv4_route_info.out_rif;
    rv = bcm_l3_intf_get(unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_ipv4_route_info.out_rif, 0, 0, cint_ipv4_route_info.my_mac, cint_ipv4_route_info.vrf);
        rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
         printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
         return rv;
        }

    }    

    return rv;
}

/**
 * ARP adding
 * INPUT:
 *   in_port    - traffic incoming port 
 *   out_port - traffic outgoing port
*/
 int
 multi_dev_ipv4_uc_arp_configuration(
    int unit,
    int out_port)
{
    int rv;

    sand_utils_l3_arp_s arp_structure;

    /*
     * Configure native ARP entry
     */
    if (cint_ipv4_route_info.mtu)
    {
        /*
         * For JR2, set l3eg.mtu as none-zero, so the result type of eedb_arp has mtu_profile field 
         */    
        sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_ipv4_route_info.arp_encap_id, 0, 0 , cint_ipv4_route_info.next_hop_mac, cint_ipv4_route_info.out_rif);
        arp_structure.mtu = cint_ipv4_route_info.mtu;
    }
    else
    {
        sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_ipv4_route_info.arp_encap_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION , cint_ipv4_route_info.next_hop_mac, cint_ipv4_route_info.out_rif); 
    }
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }

    return rv;
}


/**
 * FEC entry adding
 * INPUT:
 *   in_port    - traffic incoming port 
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_ipv4_uc_fec_entry_configuration(
    int unit,
    int in_port,
    int out_sys_port)
{
    int rv;
    sand_utils_l3_fec_s fec_structure;


    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination = out_sys_port;

    switch (cint_ipv4_route_info.mode)
    {
        case 0:
            fec_structure.tunnel_gport = cint_ipv4_route_info.out_rif;
            fec_structure.tunnel_gport2 = cint_ipv4_route_info.arp_encap_id;
        break;

        case 1:
            fec_structure.tunnel_gport = cint_ipv4_route_info.arp_encap_id;
        break;

        case 2:
            fec_structure.tunnel_gport = cint_ipv4_route_info.arp_encap_id;
            fec_structure.flags= BCM_L3_ENCAP_SPACE_OPTIMIZED;
        break;
    }

    fec_structure.fec_id = cint_ipv4_route_info.level1_fec_id;
    fec_structure.allocation_flags = BCM_L3_WITH_ID;

    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    return rv;
}

/**
 * forward entry adding(ILM, IPv4 Route)
 * INPUT:
 *   unit    - traffic incoming unit 
*/
 int
 multi_dev_ipv4_uc_forward_entry_configuration(
    int unit)
{
    int rv;
    rv = add_host_ipv4(unit, cint_ipv4_route_info.dip, cint_ipv4_route_info.vrf, cint_ipv4_route_info.level1_fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }

    return rv;
}


/**
 * ingress side configuration
 * INPUT:
 *   in_port	- traffic incoming port 
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_ipv4_uc_ingress_configuration(
    int ingress_unit,
    int in_port,
    int out_sys_port)
{
    int rv;

    rv = multi_dev_ingress_port_configuration(ingress_unit,in_port, cint_ipv4_route_info.in_rif);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ingress_port_configuration\n");
     return rv;
    }

    rv = multi_dev_ipv4_uc_l3_intf_configuration(ingress_unit,in_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_l3_intf_configuration\n");
     return rv;
    }

    rv = multi_dev_ipv4_uc_fec_entry_configuration(ingress_unit,in_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_fec_entry_configuration\n");
     return rv;
    }

    rv = multi_dev_ipv4_uc_forward_entry_configuration(ingress_unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_forward_entry_configuration\n");
     return rv;
    }

    return rv;
}

/**
 * egress side configuration
 * INPUT:
 *   out_port - traffic outgoing port
*/
 int
 multi_dev_ipv4_uc_egress_configuration(
    int egress_unit,
    int out_port)
{
    int rv;

    rv = multi_dev_egress_port_configuration(egress_unit,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_egress_port_configuration\n");
     return rv;
    }

    rv = multi_dev_ipv4_uc_l3_intf_configuration(egress_unit,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_l3_intf_configuration\n");
     return rv;
    }

    rv = multi_dev_ipv4_uc_arp_configuration(egress_unit,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_arp_configuration\n");
     return rv;
    }

    return rv;
}



/**
 * Main entrance for IPv4 UC on multi-device
 * INPUT:
 *   in_sys_port	- traffic incoming port 
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_ipv4_uc_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv;

    rv = multi_dev_ipv4_uc_init(ingress_unit,egress_unit,in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_init\n");
     return rv;
    }


    rv = multi_dev_ipv4_uc_ingress_configuration(ingress_unit,cint_ipv4_route_info.in_local_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_ingress_configuration\n");
     return rv;
    }

    rv = multi_dev_ipv4_uc_egress_configuration(egress_unit,cint_ipv4_route_info.out_local_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_egress_configuration\n");
     return rv;
    }
    return rv;
}



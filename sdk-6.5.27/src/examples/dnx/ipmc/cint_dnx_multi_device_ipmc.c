/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Multi-Dev IPMC UC~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_dnx_multi_device_ipmc.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_multi_device_ipmc.c
 * Purpose: Demo IPMC under multi device system
 *
 * Multi-Device system require the ingress/egress configuration separately and relatively. 
 * This cint demo how to configurer the IPv4 service under multi-device system.
 *  1. Create ARP and out-RIF configuration on egress device
 *  2. Create In-RIF and Forward configuration on ingress device
 *  3. Send IPv4 packet to verify the result

 * Three MC mode is tested:
 *  1. Ingress MC;
 *  2. Fabric+egress MC;
 *  3. Ingres+egress MC;
 *
 *  Four replication will be received:
 *  1. Without arp information, DA is auto generated compatible multicast mac;
 *  2. With arp information, DA is next hop mac;
 *  3. In-RIF equal Out-RIF, fall-to-bridge if the incoming traffic da is compatible mac;
 *  4. routing into tunnel, with MPLS header and next hop DA;
 *  
 *  5. For egress MC(mode 2 and 3), a additional copy is received, packet carry MPLS header
 *     and UC DA, this copy is encapsulated by 2 Cuds.
 *
 * Test Scenario - start
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/examples/dnx/cint_dnx_ip_route_basic.c
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_mpls.c
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../src/examples/dnx/cint_dnx_ipmc_route_basic.c
 * cint ../../src/examples/dnx/cint_dnx_multi_device_ipmc.c
 * cint
 * initialize_required_system_tools(2);
 * cint_ipmc_route_info.mc_mode=1;
 * multi_dev_ipmc_example(2,1,1811940042,1811939528);
 * exit;
 *
 *  ETH-RIF packet 
 * tx 1 psrc=202 data=0x01005e0202020000070001008100006408004500003500000000400084b90a0a0a02e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 *  Data: 0x01005e0202020000070001008100006408004500003500000000400084b90a0a0a02e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 * 
 *  Data: 0x01005e020202000000000011810010c8080045000035000000003f0085b90a0a0a02e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 * 
 *  Data: 0x000000000022000000000011810010c8080045000035000000003f0085b90a0a0a02e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 * 
 *  Data: 0x000000000022000000000011810010c88847007d103f007d013f45000035000000003f0085b90a0a0a02e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 *  ETH-RIF packet 
 * tx 1 psrc=202 data=0x0000000000110000070001008100006408004500003500000000400084b90a0a0a02e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 1 should be: 
 * 
 *  Data: 0x01005e02020200000000001181001064080045000035000000003f0085b90a0a0a02e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 * 
 *  Data: 0x01005e020202000000000011810010c8080045000035000000003f0085b90a0a0a02e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 * 
 *  Data: 0x000000000022000000000011810010c8080045000035000000003f0085b90a0a0a02e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 * 
 *  Data: 0x000000000022000000000011810010c88847007d103f007d013f45000035000000003f0085b90a0a0a02e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
 *
 * Test Scenario - end
 */


 struct cint_multi_dev_ipmc_info_s 
{
    int in_rif;                                /* access RIF */
    int out_rif;                            /* out RIF */
    int vrf;                                /*  VRF */
    bcm_mac_t my_mac;                       /* mac for Incoming L3 intf */
    bcm_mac_t next_hop_mac;                 /* next hop mac for outgoing station */
    bcm_ip_t mc_group_ip;                           /* dip */
    bcm_ip_t host_ip;                           /* sip */

    int pwe_label[2];                          /*mpls label for pwe*/
    int mpls_tunnel_label[2];                  /*mpls label for tunnel*/

    int arp_encap_id;                       /*Next hop intf ID*/ 
    int pwe_encap_id[2];                       /*pwe encap ID*/ 
    int tunnel_encap_id[2];                    /*tunnel encap ID*/ 

    int mc_group;
    
    int mc_mode;                    /* 1: ingress mc; 2:ingress+egress mc;3:fabric+egress mc*/

    int in_sys_port;
    int out_sys_port;
    int in_local_port;                           
    int out_local_port;               

};
 
 
 cint_multi_dev_ipmc_info_s cint_ipmc_route_info=
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
    * mc_group_ip
    */
    0xE0020202 /** 224.2.2.2 */ ,

    /*
    * host_ip
    */
    0x0A0A0A02 /** 10.10.10.2 */ ,

    /*
    * pwe_label | mpls_tunnel_label
    */
    {1000,1001},
    {2000,2001},

    /*
    * arp_encap_id | pwe_encap_id | tunnel_encap_id
    */
    0x3000,
    {0x3001,0x3002},
    {0x10003,0x10004},

    /*
     * mc_group
     */
    6000,

    /*
     * mc_mode
     */
    1,


};

/**
 * Main entrance for mpls check ver configuration
 * INPUT:
 *   ingress_unit- traffic incoming unit 
 *   egress_unit- traffic ougtoing unit 
 *   in_sys_port- traffic incoming port 
 *   out_sys_port- traffic outgoing port 
*/
 int
 multi_dev_ipmc_init(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int local_port,is_local;


    cint_ipmc_route_info.in_sys_port = in_sys_port;
    cint_ipmc_route_info.out_sys_port = out_sys_port;
    /*
     * 0: Convert the sysport to local port
     */
    multi_dev_system_port_to_local(ingress_unit,in_sys_port,&cint_ipmc_route_info.in_local_port);

    multi_dev_system_port_to_local(egress_unit,out_sys_port,&cint_ipmc_route_info.out_local_port);

    /*
     * create Vlan first, for LIF creation
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(ingress_unit, cint_ipmc_route_info.in_rif), "");

    /*
     * Align the stpid as 0x9100
     */
    bcm_port_tpid_class_t port_tpid_class;
    bcm_port_tpid_class_t_init(&port_tpid_class);

    port_tpid_class.tpid1 = 0x8100;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.port =  cint_ipmc_route_info.in_local_port;
    /*stag*/
    port_tpid_class.tag_format_class_id = 4;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(ingress_unit, &port_tpid_class), "( oPort )");

    return BCM_E_NONE;
}


/**
 * L3 intf init
 * INPUT:
 *   in_port    - traffic incoming port 
*/
 int
 multi_dev_ipmc_l3_intf_configuration(
    int unit,
    int in_port)
{
    int rv;
    bcm_l3_intf_t l3_intf;
    dnx_utils_l3_eth_rif_s eth_rif_structure;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_ipmc_route_info.in_rif;
    rv = bcm_l3_intf_get(unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_ipmc_route_info.in_rif, 0, 0, cint_ipmc_route_info.my_mac, cint_ipmc_route_info.vrf);
        BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "core_native_eth_rif");
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_ipmc_route_info.out_rif;
    rv = bcm_l3_intf_get(unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_ipmc_route_info.out_rif, 0, 0, cint_ipmc_route_info.my_mac, cint_ipmc_route_info.vrf);
        BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "core_native_eth_rif");

    }    

    return BCM_E_NONE;
}

/**
 * ARP adding
 * INPUT:
 *   in_port    - traffic incoming port 
 *   out_port - traffic outgoing port
*/
 int
 multi_dev_ipmc_encapsulation_configuration(
    int unit,
    int out_port)
{
    int i = 0;

    dnx_utils_l3_arp_s arp_structure;

    /*
     * 1. Configure ARP entry
     */
    dnx_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_ipmc_route_info.arp_encap_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION , cint_ipmc_route_info.next_hop_mac, cint_ipmc_route_info.out_rif);
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_arp_create(unit, &arp_structure), "create AC egress object ARP only");
    cint_ipmc_route_info.arp_encap_id = arp_structure.l3eg_arp_id;

    /*
     * 2. mpls tunnel   
     *   tunnel1 pointing to ARP directly;
     *   tunnel2 no pointing to arp
     */
    bcm_mpls_egress_label_t label_array[2];
    
    for (i = 0; i < 2; i++) {
        bcm_mpls_egress_label_t_init(&label_array[0]);
        bcm_mpls_egress_label_t_init(&label_array[1]);
        
        label_array[0].label = cint_ipmc_route_info.mpls_tunnel_label[i];
        label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_WITH_ID|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
        label_array[0].action = BCM_MPLS_EGRESS_ACTION_PUSH;
        label_array[0].tunnel_id = cint_ipmc_route_info.tunnel_encap_id[i];

        label_array[1].label = cint_ipmc_route_info.mpls_tunnel_label[i] + 1;
        label_array[1].flags = (BCM_MPLS_EGRESS_LABEL_WITH_ID|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
        label_array[1].action = BCM_MPLS_EGRESS_ACTION_PUSH;
        label_array[1].tunnel_id = cint_ipmc_route_info.tunnel_encap_id[i];

        if (i == 0) {
            label_array[1].l3_intf_id = cint_ipmc_route_info.arp_encap_id;    
        }
        /** Create the mpls tunnel with one call.*/
        BCM_IF_ERROR_RETURN_MSG(sand_mpls_tunnel_initiator_create(unit, 0, 2, label_array), "");
        cint_ipmc_route_info.tunnel_encap_id[i] = label_array[1].tunnel_id;  
    }

    return BCM_E_NONE;
}

/**
 * L2 IN_LIF adding
 * INPUT:
 *   unit    - traffic incoming unit 
*/
 int
multi_dev_ipmc_l2_in_lif_configuration(
    int unit)
{
    bcm_vlan_port_t vlan_port;

    bcm_vlan_port_t_init(&vlan_port);

    /* set port attribures, key <port-vlan-vlan>*/
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = cint_ipmc_route_info.in_local_port;
    vlan_port.match_vlan = cint_ipmc_route_info.in_rif;    
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");


    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, cint_ipmc_route_info.in_rif, vlan_port.vlan_port_id), "");
    return BCM_E_NONE;
}

int multi_dev_erp_port_get(int unit, int *erp_sysport)
{
    int 
    count_erp = 0,
    nof_erp_ports,
    i;
    bcm_gport_t 
    erp_local_port_gport[2], erp_sysport_gport;
        
    nof_erp_ports = 2;
    
    BCM_IF_ERROR_RETURN_MSG(bcm_port_internal_get(unit, BCM_PORT_INTERNAL_EGRESS_REPLICATION, nof_erp_ports, &erp_local_port_gport, &count_erp), "");

    for (i = 0; i < count_erp; ++i) {
        printf("count_erp=%d, erp_local_port_gport=0x%x\n", count_erp, erp_local_port_gport[i]);
        
        BCM_IF_ERROR_RETURN_MSG(bcm_stk_gport_sysport_get(unit, erp_local_port_gport[i], &erp_sysport_gport), "");
        
        erp_sysport[i] = erp_sysport_gport;
            
        printf("count_erp=%d, erp_local_port_gport=0x%x, erp_sysport_gport=0x%x, *erp_sysport=0x%x,%d\n", count_erp, erp_local_port_gport[i], erp_sysport_gport, erp_sysport[i], erp_sysport[i]);
    }
    
    return BCM_E_NONE;
}

/*
 * Create an MC group
 * - replications - an array of replications.
 * - mc_group - the MC group
 * - mc_mode
 */
int
multi_dev_ipmc_create_multicast(
    int ingress_unit,
    int egress_unit,
    bcm_multicast_replication_t * replications,
    int nof_replications,
    bcm_multicast_t mc_group,
    int mc_mode)
{
    int create_flags = BCM_MULTICAST_WITH_ID;
    int ingress_create_flags = BCM_MULTICAST_WITH_ID;
    int egress_create_flags = BCM_MULTICAST_WITH_ID;
    int set_flags = 0;
    int mcdb_unit = 0;
    int mc_id_idx = mc_group;

    if (mc_mode == 1) {
        mcdb_unit = ingress_unit;
        set_flags = BCM_MULTICAST_INGRESS_GROUP;
        create_flags |= set_flags;
    } else if (mc_mode == 2) {
        mcdb_unit = egress_unit;
        ingress_create_flags |= BCM_MULTICAST_INGRESS_GROUP;
        egress_create_flags |= BCM_MULTICAST_EGRESS_GROUP;
        set_flags = BCM_MULTICAST_EGRESS_GROUP;
    } else {
        mcdb_unit = egress_unit;
        ingress_create_flags |= BCM_MULTICAST_EGRESS_GROUP;
        egress_create_flags |= BCM_MULTICAST_EGRESS_GROUP;
        set_flags = BCM_MULTICAST_EGRESS_GROUP;
    }
    /*
     * Ingress Mc, only create mc at ingress unit
    */
    if (mc_mode == 1) {
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(ingress_unit, create_flags, &mc_id_idx), "");
    } else {
        /*
         * In below scenarios: ingress group need to create:
         * 1: Ingress+Egress MC
         */
        if (mc_mode == 2) {
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(ingress_unit, ingress_create_flags, &mc_id_idx), "");
        }
        mc_id_idx = mc_group;
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(egress_unit, egress_create_flags, &mc_id_idx), "");

        /*
         *  Fabric+Egress Mc : Fabric MC creation
         */
        if (mc_mode == 3) {
            int fe_unit = 0;
            int fe_mc_id = mc_group;
            int dest_fap_id = 0;
            bcm_gport_t gport;
            int is_fabric_supported = 1;

            for (fe_unit = 0; fe_unit < 3;fe_unit++)
            {
                if ((fe_unit != ingress_unit) && (fe_unit != egress_unit))
                {
                    break;
                }   
            }
            is_fabric_supported = *(dnxc_data_get(egress_unit, "fabric", "general", "blocks_exist", NULL));

            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(fe_unit, BCM_MULTICAST_WITH_ID, &fe_mc_id), "");
            if (is_fabric_supported) {
                BCM_IF_ERROR_RETURN_MSG(bcm_stk_sysport_gport_get(egress_unit, cint_ipmc_route_info.out_sys_port, &gport), "");
                dest_fap_id = BCM_GPORT_MODPORT_MODID_GET(gport);
                BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_set(fe_unit, fe_mc_id, 0, 1, &dest_fap_id), "");
            }
        }

        /*
         * Ingress+Egress MC : 1 copy to ERP port
         */
        if (mc_mode == 2) {
            int erp_sys_port[2];
            int ii=0;
            bcm_multicast_replication_t erp_rep[2];
            int ingress_set_flag = BCM_MULTICAST_INGRESS_GROUP;
            
            multi_dev_erp_port_get(egress_unit,erp_sys_port);
 
            for (ii = 0; ii<2;ii++) {
                bcm_multicast_replication_t_init(&erp_rep[ii]);
                erp_rep[ii].port = erp_sys_port[ii];
                erp_rep[ii].encap1 = mc_group;
            }
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(ingress_unit, mc_group, ingress_set_flag, 2, &erp_rep), "");
        }
    }

    /*MCDB setting*/
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(mcdb_unit, mc_group, set_flags, nof_replications, replications), "");

    return BCM_E_NONE;
}


/**
 * forward entry adding(ILM, IPv4 Route)
 * INPUT:
 *   unit    - traffic incoming unit 
*/
 int
 multi_dev_ipmc_forward_entry_configuration(
    int ingress_unit,
    int egress_unit,
    int out_sys_port)
{
    int pp_db_replication_list[2];
    int pp_db_rep_index = 0x300000;
    int l3_mc_id;

    /*
     * 1. Add replication into the group
     */
    bcm_multicast_replication_t replications[5];
    int rep_count = 4;
    int out_port = 0;

    if (cint_ipmc_route_info.mc_mode == 1) {
       out_port = out_sys_port;
    } else {
       out_port = cint_ipmc_route_info.out_local_port;
    }

    /* 1st copy: to In-RIF*/
    bcm_multicast_replication_t_init(&replications[0]);
    replications[0].encap1 = cint_ipmc_route_info.in_rif;
    replications[0].port = out_port;

    /* 2nd copy: to Out-RIF1 without ARP*/
    bcm_multicast_replication_t_init(&replications[1]);
    replications[1].encap1 = cint_ipmc_route_info.out_rif;
    replications[1].port = out_port;

    /* 3rd copy: to Out-RIF2 with ARP*/
    bcm_multicast_replication_t_init(&replications[2]);
    replications[2].encap1 = cint_ipmc_route_info.arp_encap_id;
    replications[2].port = out_port;

    /* 4th copy: to vpn tunnel with one pointer */
    bcm_multicast_replication_t_init(&replications[3]);
    replications[3].encap1 = cint_ipmc_route_info.tunnel_encap_id[0];
    replications[3].port = out_port;

    /* 5th copy: to vpn tunnel with two pointers */
    if (cint_ipmc_route_info.mc_mode != 1) {
        bcm_multicast_replication_t_init(&replications[4]);
        replications[4].encap1 = cint_ipmc_route_info.tunnel_encap_id[1];
        replications[4].port = out_port;
        rep_count = 5;

        /* Use PP DB to carry 2 pointers*/
        BCM_L3_ITF_SET(pp_db_replication_list[0],BCM_L3_ITF_TYPE_LIF,cint_ipmc_route_info.tunnel_encap_id[1]);
        BCM_L3_ITF_SET(pp_db_replication_list[1],BCM_L3_ITF_TYPE_LIF,cint_ipmc_route_info.arp_encap_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_encap_extension_create(egress_unit,
                                 BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID,
                                 &pp_db_rep_index, 
                                 2, pp_db_replication_list), "");

        replications[4].encap1 = pp_db_rep_index;
    }

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ipmc_create_multicast(ingress_unit, egress_unit, replications,rep_count, cint_ipmc_route_info.mc_group,cint_ipmc_route_info.mc_mode), "");

    /*
     * 2. Add IPv4 IPMC entry
     */
    BCM_MULTICAST_L3_SET(l3_mc_id,cint_ipmc_route_info.mc_group);
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(ingress_unit, cint_ipmc_route_info.mc_group_ip, 0xFFFFFFFF, cint_ipmc_route_info.host_ip, 0xFFFFFFFF,
                       cint_ipmc_route_info.in_rif, cint_ipmc_route_info.vrf,cint_ipmc_route_info.mc_group, 0, 0), "to LEM table");

    return BCM_E_NONE;
}

/**
 * ingress side configuration
 * INPUT:
 *   in_port - traffic incoming port 
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_ipmc_ingress_configuration(
    int ingress_unit,
    int egress_unit,
    int in_port,
    int out_sys_port)
{

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ingress_port_configuration(ingress_unit,in_port,cint_ipmc_route_info.in_rif), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ipmc_l3_intf_configuration(ingress_unit,in_port), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ipmc_l2_in_lif_configuration(ingress_unit), "");


    BCM_IF_ERROR_RETURN_MSG(multi_dev_ipmc_forward_entry_configuration(ingress_unit,egress_unit,out_sys_port), "");

    return BCM_E_NONE;
}

/**
 * egress side configuration
 * INPUT:
 *   out_port - traffic outgoing port
*/
 int
 multi_dev_ipmc_egress_configuration(
    int egress_unit,
    int out_port)
{

    BCM_IF_ERROR_RETURN_MSG(multi_dev_egress_port_configuration(egress_unit,out_port), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ipmc_l3_intf_configuration(egress_unit,out_port), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ipmc_encapsulation_configuration(egress_unit,out_port), "");

    return BCM_E_NONE;
}



/**
 * Main entrance for IPv4 UC on multi-device
 * INPUT:
 *   in_sys_port - traffic incoming port 
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_ipmc_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ipmc_init(ingress_unit,egress_unit,in_sys_port,out_sys_port), "");

    BCM_IF_ERROR_RETURN_MSG(multi_dev_ipmc_ingress_configuration(ingress_unit,egress_unit,cint_ipmc_route_info.in_local_port,out_sys_port), "");
    
    BCM_IF_ERROR_RETURN_MSG(multi_dev_ipmc_egress_configuration(egress_unit,cint_ipmc_route_info.out_local_port), "");

    return BCM_E_NONE;
}

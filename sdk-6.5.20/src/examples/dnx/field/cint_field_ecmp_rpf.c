/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 /*
  * Configuration example start:
  *
  * cint;                                                                  
  * cint_reset();                                                          
  * exit;
  * cint ../../../src/examples/sand/utility/cint_sand_utils_global.c
  * cint ../../../src/examples/sand/utility/cint_sand_utils_l3.c
  * cint ../../../src/examples/sand/cint_ip_route_basic.c
  * cint ../../src/examples/dnx/field/cint_field_presel_fwd_layer.c
  * cint ../../../src/examples/dnx/field/cint_field_ecmp_rpf.c
  * cint ../../../src/examples/dnx/field/cint_field_ecmp_rpf_test.c
  * cint;
  * int unit = 0;
  * bcm_field_context_t context_id_ipmf3 = BCM_FIELD_CONTEXT_ID_DEFAULT;
  * int in_port_1 = 200;
  * int in_port_2 = 201;
  * int out_port = 202;
  * bcm_field_presel_t ipmf3_presel_id = 50;
  * bcm_field_stage_t ipmf3_stage = bcmFieldStageIngressPMF3;
  * bcm_field_layer_type_t fwd_layer = bcmFieldLayerTypeIp4;
  * field_presel_fwd_layer_main(unit, ipmf3_presel_id, ipmf3_stage, fwd_layer, &context_id_ipmf3, "ecmp_rpf_e2e_ctx");
  * field_ecmp_rpf_main(unit, context_id_ipmf3, in_port_1, in_port_2, out_port);
  *
  * Configuration example end
  *
  *   This cint creates a field group that performs a trap action and a field group the potentially invalidates it.
  *   Used as an example for unicast ECMP RPF in iPMF3.
 */
 
 /** NOF FECs in the ECMP group of the basic ECMP test. Represent different next hop ARPs, not diferent interfaces.*/
int ECMP_RPF_NOF_MEMBERS_ECMP_GROUP = 10;
struct cint_field_ecmp_rpf_pre_pmf_info_s
{
    int in_port_1;                                        /** Incoming port */
    int in_port_2;                                      /** Another incoming port */
    int out_port;                                       /** Outgoing port */
    int intf_in_1;                                        /** Incoming packet ETH-RIF */
    int intf_in_2;                                      /** Another incoming packet ETH-RIF */
    int intf_out;                                       /** Outgoing packet ETH-RIF */
    bcm_mac_t intf_in_mac_address_1;                      /** mac for in RIF */
    bcm_mac_t intf_in_mac_address_2;                    /** mac for second in RIF */
    bcm_mac_t intf_out_mac_address;                     /** mac for out RIF */
    bcm_mac_t arp_next_hop_mac;                         /** mac for out RIF */
    int encap_id;                                       /** encap ID for ECMP */
    int encap_id_fwd;                                   /** encap ID for out interface */
    bcm_ip_t host_dip;                                  /** destination route */
    bcm_ip_t host_sip;                                  /** route for the packet to come from for RPF */
    int vrf;                                            /** VRF */
    bcm_if_t fec_id;                                    /** FEC id */
    bcm_if_t fec_id_fwd;                                /** FEC id */
    bcm_if_t ecmp_id;                                   /** ECMP interface id */
    int is_consistent;                                  /** Indication for type of ECMP*/
};

cint_field_ecmp_rpf_pre_pmf_info_s cint_field_ecmp_rpf_pre_pmf_info = {
    /*
     * ports : in_port_1 | in_port_2 | out_port
     */
    200, 201, 202,
    /*
     * intf_in | intf_in_2 | intf_out
     */
    15, 16, 100,
    /*
     * intf_in_mac_address_1 | intf_in_mac_address_ext | intf_out_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}, {0x00, 0x12, 0x34, 0x56, 0x78, 0x9a},
    /*
     * arp_next_hop_mac
     */
    {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * encap_id
     */
    0x1384,
    /*
     * encap_id_fwd
     */
    0x1385,
    /*
     * host_dip
     */
    0x7fffff02 /** 127.255.255.2 */ ,
    /*
     * host_sip
     */
    0xc0800101 /** 192.128.1.1 */,
    /*
     * vrf
     */
    1,
    /*
     * fec_id
     */
    0xEA62,
    /*
     * fec_id_out
     */
    0xEA61,
    /*
     * ecmp_id
     */
    50,
    /*
     * is_consistent
     */
    0,
};

 struct cint_field_ecmp_rpf_pmf_info_s
 {
    bcm_field_group_t ecmp_lookup_fg_id;                /** The field group ID of the ECMP EM lookup.*/
    bcm_field_group_t trap_fg_id;                       /** The field group ID of the trap activation*/
    bcm_field_entry_t entry_handle_trap;                /** The entry of the trap activation*/
    int rpf_uc_ecmp_strict_trap;                        /** The ID of the user defined trap invoked by iPMF3.*/
 };
 
 cint_field_ecmp_rpf_pmf_info_s cint_field_ecmp_rpf_pmf_info = { 0 };
 
 /**
 * \brief
 *  Configure trap for strict unicast RPF.
 * \param [in] unit           - Device ID
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
 int field_ecmp_rpf_trap_pre_pmf(
    int unit)
 {
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    int rpf_uc_strict_trap = 0;
    /*
     * RPF types:
     */
    /*
     * bcmRxTrapUcStrictRpfFail, : UC-RPF-Mode is 'Strict' and OutRIF is not equal to packet InRIF .
     */

    /*
     * change dest port for trap
     */
    bcm_rx_trap_config_t_init(&config);
    config.flags |= (BCM_RX_TRAP_UPDATE_DEST);
    config.trap_strength = 7;
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    /*
     * set uc strict to drop packet
     */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUcStrictRpfFail, &rpf_uc_strict_trap);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
        printf("Error, in trap create, trap bcmRxTrapUcStrictRpfFail \n");
        return rv;
    }

    rv = bcm_rx_trap_set(unit, rpf_uc_strict_trap, &config);
    if (rv != BCM_E_NONE && rv != BCM_E_PARAM)
    {
        printf("Error, in trap set rpf_uc_strict_trap\n");
        return rv;
    }


     return rv;
     
 }

/**
 * \brief
 *  Configure user defined trap for ecmp RPF.
 * \param [in] unit           - Device ID
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
 int field_ecmp_rpf_trap_pmf(
    int unit)
 {
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;

    /*
     * change dest port for trap
     */
    bcm_rx_trap_config_t_init(&config);
    config.flags |= (BCM_RX_TRAP_UPDATE_DEST);
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    /*
     * set uc strict to drop packet
     */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &cint_field_ecmp_rpf_pmf_info.rpf_uc_ecmp_strict_trap);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in trap create, trap bcmRxTrapUcStrictRpfFail \n", rv);
        return rv;
    }

    rv = bcm_rx_trap_set(unit, cint_field_ecmp_rpf_pmf_info.rpf_uc_ecmp_strict_trap, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in trap set rpf_uc_ecmp_strict_trap (%d)\n", rv, cint_field_ecmp_rpf_pmf_info.rpf_uc_ecmp_strict_trap);
        return rv;
    }

    printf ("Created and set user defined trap %d", cint_field_ecmp_rpf_pmf_info.rpf_uc_ecmp_strict_trap);

     return rv;
     
 }

/**
 * \brief
 *  Destroy configured user defined trap.
 * \param [in] unit           - Device ID
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
 int field_ecmp_rpf_trap_pmf_destroy(
    int unit)
 {
    int rv = BCM_E_NONE;


    rv = bcm_rx_trap_type_destroy(unit, cint_field_ecmp_rpf_pmf_info.rpf_uc_ecmp_strict_trap);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_rx_trap_type_destroy rpf_uc_ecmp_strict_trap (%d) \n",
               rv, cint_field_ecmp_rpf_pmf_info.rpf_uc_ecmp_strict_trap);
        return rv;
    }

    return rv;
     
 }
 
 /**
 * \brief
 *  Configure an iPMF2 field group to change values to test iPMF3 ECMP RPF configuration.
 *  Writes the first 22 bits of the destination IP to INLIF0.
 *  Assumes IPV4 is the forwarding layer.
 * \param [in] unit           - Device ID
 * \param [in] in_port_1      - Port from which the packet enters
 * \param [in] in_port_2      - Another port from which the packet enters
 * \param [in] out_port       - Port from which the packet is expected to leave
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
 int field_ecmp_rpf_pre_pmf(
    int unit,
    int in_port_1,
    int in_port_2,
    int out_port)
 {
    int iter;
    bcm_if_t intf_array[ECMP_RPF_NOF_MEMBERS_ECMP_GROUP];
    bcm_gport_t gport_in_1;
    bcm_gport_t gport_in_2;
    bcm_gport_t gport_out;
    int fec_fwd = cint_field_ecmp_rpf_pre_pmf_info.fec_id_fwd;
    bcm_l3_egress_ecmp_t ecmp;
    sand_utils_l3_fec_s fec_structure;
    bcm_l3_ingress_t l3_ing_if;
    int rv = BCM_E_NONE;

    printf("Pre-PMF configuration.\n"); 

    cint_field_ecmp_rpf_pre_pmf_info.in_port_1 = in_port_1;
    cint_field_ecmp_rpf_pre_pmf_info.in_port_2 = in_port_2;
    cint_field_ecmp_rpf_pre_pmf_info.out_port = out_port;
    
    /*
     * Set In-Port 1 to In ETh-RIF 1
     */
    rv = in_port_intf_set(unit, cint_field_ecmp_rpf_pre_pmf_info.in_port_1, cint_field_ecmp_rpf_pre_pmf_info.intf_in_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }
    printf("In-port %d set to In Eth-RIF %d (first in port).\n", 
            cint_field_ecmp_rpf_pre_pmf_info.in_port_1, cint_field_ecmp_rpf_pre_pmf_info.intf_in_1);
    /*
     * Set In-Port 2 to In ETh-RIF 2
     */
    rv = in_port_intf_set(unit, cint_field_ecmp_rpf_pre_pmf_info.in_port_2, cint_field_ecmp_rpf_pre_pmf_info.intf_in_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }
    printf("In-port %d set to In Eth-RIF %d (second in port).\n", 
            cint_field_ecmp_rpf_pre_pmf_info.in_port_2, cint_field_ecmp_rpf_pre_pmf_info.intf_in_2);

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_field_ecmp_rpf_pre_pmf_info.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set out_port\n");
        return rv;
    }
    printf("Out-port %d set to default properties.\n", cint_field_ecmp_rpf_pre_pmf_info.out_port);

    /*
     * Create ETH-RIF (incoming RIF 1) and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_field_ecmp_rpf_pre_pmf_info.intf_in_1, cint_field_ecmp_rpf_pre_pmf_info.intf_in_mac_address_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }
    printf("Created In Eth-Rif %d with MAC address %d:%d:%d:%d:%d:%d.\n", 
            cint_field_ecmp_rpf_pre_pmf_info.intf_in_1,
            cint_field_ecmp_rpf_pre_pmf_info.intf_in_mac_address_1[0],
            cint_field_ecmp_rpf_pre_pmf_info.intf_in_mac_address_1[1],
            cint_field_ecmp_rpf_pre_pmf_info.intf_in_mac_address_1[2],
            cint_field_ecmp_rpf_pre_pmf_info.intf_in_mac_address_1[3],
            cint_field_ecmp_rpf_pre_pmf_info.intf_in_mac_address_1[4],
            cint_field_ecmp_rpf_pre_pmf_info.intf_in_mac_address_1[5]);

    /*
     * Set Incoming ETH-RIF 1 properties
     */
    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;
    l3_ing_if.vrf = cint_field_ecmp_rpf_pre_pmf_info.vrf;
    l3_ing_if.urpf_mode = bcmL3IngressUrpfStrict;

    rv = bcm_l3_ingress_create(unit, l3_ing_if, cint_field_ecmp_rpf_pre_pmf_info.intf_in_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ingress_create\n");
        return rc;
    }
    printf("In Eth-RIF %d set to VRF %d urpf_mode bcmL3IngressUrpfStrict.\n", 
           cint_field_ecmp_rpf_pre_pmf_info.intf_in_1, cint_field_ecmp_rpf_pre_pmf_info.vrf);

    /*
     * Create ETH-RIF (incoming RIF 2) and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_field_ecmp_rpf_pre_pmf_info.intf_in_2, cint_field_ecmp_rpf_pre_pmf_info.intf_in_mac_address_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }
    printf("Created In Eth-Rif %d with MAC address %d:%d:%d:%d:%d:%d.\n", 
            cint_field_ecmp_rpf_pre_pmf_info.intf_in_2,
            cint_field_ecmp_rpf_pre_pmf_info.intf_in_mac_address_2[0],
            cint_field_ecmp_rpf_pre_pmf_info.intf_in_mac_address_2[1],
            cint_field_ecmp_rpf_pre_pmf_info.intf_in_mac_address_2[2],
            cint_field_ecmp_rpf_pre_pmf_info.intf_in_mac_address_2[3],
            cint_field_ecmp_rpf_pre_pmf_info.intf_in_mac_address_2[4],
            cint_field_ecmp_rpf_pre_pmf_info.intf_in_mac_address_2[5]);

    /*
     * Set Incoming ETH-RIF 2 properties
     */
    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;
    l3_ing_if.vrf = cint_field_ecmp_rpf_pre_pmf_info.vrf;
    l3_ing_if.urpf_mode = bcmL3IngressUrpfStrict;

    rv = bcm_l3_ingress_create(unit, l3_ing_if, cint_field_ecmp_rpf_pre_pmf_info.intf_in_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ingress_create\n");
        return rc;
    }
    printf("In Eth-RIF %d set to VRF %d urpf_mode bcmL3IngressUrpfStrict.\n", 
           cint_field_ecmp_rpf_pre_pmf_info.intf_in_2, cint_field_ecmp_rpf_pre_pmf_info.vrf);

    /*
     * Create ETH-RIF (outgoing RIF) and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_field_ecmp_rpf_pre_pmf_info.intf_out, cint_field_ecmp_rpf_pre_pmf_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }
    printf("Created out Eth-Rif %d with MAC address %d:%d:%d:%d:%d:%d.\n", 
            cint_field_ecmp_rpf_pre_pmf_info.intf_out,
            cint_field_ecmp_rpf_pre_pmf_info.intf_out_mac_address[0],
            cint_field_ecmp_rpf_pre_pmf_info.intf_out_mac_address[1],
            cint_field_ecmp_rpf_pre_pmf_info.intf_out_mac_address[2],
            cint_field_ecmp_rpf_pre_pmf_info.intf_out_mac_address[3],
            cint_field_ecmp_rpf_pre_pmf_info.intf_out_mac_address[4],
            cint_field_ecmp_rpf_pre_pmf_info.intf_out_mac_address[5]);

    BCM_GPORT_LOCAL_SET(gport_in_1, cint_field_ecmp_rpf_pre_pmf_info.in_port_1);
    printf("Set GPORT %d from first in port %d.\n", gport_in_1, cint_field_ecmp_rpf_pre_pmf_info.in_port_1);
    
    BCM_GPORT_LOCAL_SET(gport_in_2, cint_field_ecmp_rpf_pre_pmf_info.in_port_2);
    printf("Set GPORT %d from second in port %d.\n", gport_in_2, cint_field_ecmp_rpf_pre_pmf_info.in_port_2);
    
    BCM_GPORT_LOCAL_SET(gport_out, cint_field_ecmp_rpf_pre_pmf_info.out_port);
    printf("Set GPORT %d from out port %d.\n", gport_out, cint_field_ecmp_rpf_pre_pmf_info.out_port);

    /*
     * Create FEC for destination.
     */
    rv = l3__egress_only_fec__create(unit, fec_fwd, cint_field_ecmp_rpf_pre_pmf_info.intf_out, cint_field_ecmp_rpf_pre_pmf_info.encap_id_fwd, gport_out, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, intf_out\n");
        return rv;
    }
     
     /*
      * Create FECs for the ECMP group
 */
     for (iter = 0; iter < ECMP_RPF_NOF_MEMBERS_ECMP_GROUP; iter++)
     {
         printf("FEC %d.\n", iter);
         /*
          * 5.1 Create ARP and set its properties
          */
         rv = l3__egress_only_encap__create(unit, 0, &cint_field_ecmp_rpf_pre_pmf_info.encap_id, cint_field_ecmp_rpf_pre_pmf_info.arp_next_hop_mac, 0);
         if (rv != BCM_E_NONE)
         {
             printf("Error, create egress object ARP only\n");
             return rv;
         }
         printf("Set ARP entry. encap_id %d, next hop MAC %d:%d:%d:%d:%d:%d.\n", 
                cint_field_ecmp_rpf_pre_pmf_info.encap_id,
                cint_field_ecmp_rpf_pre_pmf_info.arp_next_hop_mac[0],
                cint_field_ecmp_rpf_pre_pmf_info.arp_next_hop_mac[1],
                cint_field_ecmp_rpf_pre_pmf_info.arp_next_hop_mac[2],
                cint_field_ecmp_rpf_pre_pmf_info.arp_next_hop_mac[3],
                cint_field_ecmp_rpf_pre_pmf_info.arp_next_hop_mac[4],
                cint_field_ecmp_rpf_pre_pmf_info.arp_next_hop_mac[5]);
 
         /*
          * Create FEC and set its properties. for half the FECs, use the first in RIF, and for the other half
          * the second in RIF.
          */
         sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
         if (iter < (ECMP_RPF_NOF_MEMBERS_ECMP_GROUP / 2))
         {
             fec_structure.destination = gport_in_1;
             fec_structure.tunnel_gport = cint_field_ecmp_rpf_pre_pmf_info.intf_in_1;
         }
         else
         {
            fec_structure.destination = gport_in_2;
             fec_structure.tunnel_gport = cint_field_ecmp_rpf_pre_pmf_info.intf_in_2;
         }
         fec_structure.tunnel_gport2 = cint_field_ecmp_rpf_pre_pmf_info.encap_id;
         fec_structure.fec_id = cint_field_ecmp_rpf_pre_pmf_info.fec_id;
         fec_structure.allocation_flags = BCM_L3_WITH_ID;
         rv = sand_utils_l3_fec_create(unit, &fec_structure);
         if (rv != BCM_E_NONE)
         {
             printf("Error, create egress object FEC only\n");
             return rv;
         }
         printf("Created FEC.\n");
 
         /*
          * Place the new FEC into the interfaces array of the ECMP
          * Increment the FEC number ad the encap number for the next FEC.
          * Set a different next hop MAC to identify different FEC selection.
          */
         intf_array[iter] = cint_field_ecmp_rpf_pre_pmf_info.fec_id;
         cint_field_ecmp_rpf_pre_pmf_info.fec_id++;
         cint_field_ecmp_rpf_pre_pmf_info.encap_id++;
         cint_field_ecmp_rpf_pre_pmf_info.arp_next_hop_mac[5]++;
     }

     
    /*
     * Create an ECMP group
     */
    ecmp.flags = BCM_L3_WITH_ID;
    ecmp.ecmp_intf = cint_field_ecmp_rpf_pre_pmf_info.ecmp_id;
    ecmp.rpf_mode = bcmL3EcmpUrpfStrictEm;
    if (cint_field_ecmp_rpf_pre_pmf_info.is_consistent)
    {
        ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    }
    else
    {
        ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
    }
    /*
     * create an ECMP, containing the FECs entries 
     */
    rv = bcm_l3_egress_ecmp_create(unit, &ecmp, ECMP_RPF_NOF_MEMBERS_ECMP_GROUP, intf_array);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_l3_egress_ecmp_create failed: %d \n", rv);
        return rv;
    }
    printf("Created ECMP group.\n");

    /*
     * Add host entries
     */
    /** Host route for the destination.*/
    rv = add_host_ipv4(unit, cint_field_ecmp_rpf_pre_pmf_info.host_dip, cint_field_ecmp_rpf_pre_pmf_info.vrf, fec_fwd, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function internal_ip_host. \n");
        return rv;
    }
    printf("Added host entry for destination. host %u VRF %d FEC %d, encap_id %d, destination %d.\n",
           cint_field_ecmp_rpf_pre_pmf_info.host_dip,
           cint_field_ecmp_rpf_pre_pmf_info.vrf,
           fec_fwd,
           0,
           0);
    /** Host route for the source from the ECMP.*/
    rv = add_host_ipv4(unit, cint_field_ecmp_rpf_pre_pmf_info.host_sip, cint_field_ecmp_rpf_pre_pmf_info.vrf, cint_field_ecmp_rpf_pre_pmf_info.ecmp_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function internal_ip_host. \n");
        return rv;
    }
    printf("Added host entry for source. host %u VRF %d ecmp_id %d, encap_id %d, destination %d.\n",
           cint_field_ecmp_rpf_pre_pmf_info.host_sip,
           cint_field_ecmp_rpf_pre_pmf_info.vrf,
           cint_field_ecmp_rpf_pre_pmf_info.ecmp_id,
           0,
           0);
     
     return rv;
     
 }
 
 /**
 * \brief
 *  Clear configuration done by field_ecmp_rpf_pre_pmf(). Currently no performing any clearing.
 * \param [in] unit        - Device ID
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
 int field_ecmp_rpf_pre_pmf_destroy(int unit)
 {
     int rv = BCM_E_NONE;
     
     return rv;
     
 }
 
/**
 * \brief
 *  Configure ECMP RPF for a specific iPMF3 context.
 * \param [in] unit        - Device ID
 * \param [in] context_id  - Context to attach FG to. An iPMF3 context.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
 int field_ecmp_rpf_pmf(int unit, bcm_field_context_t context_id, uint8 is_mc)
 {
     bcm_field_group_info_t fg_info;
     bcm_field_group_attach_info_t attach_info;
     bcm_field_entry_info_t entry_info;
     bcm_gport_t trap_gport;
     int rv = BCM_E_NONE;
     int ii = 0;

     printf("PMF configuration.\n"); 
 
     bcm_field_group_info_t_init(&fg_info);
     fg_info.fg_type = bcmFieldGroupTypeExactMatch;
     fg_info.nof_quals = 2;
     fg_info.stage = bcmFieldStageIngressPMF3;
 
 
     fg_info.qual_types[0] = bcmFieldQualifyDstRpfGport;
     fg_info.qual_types[1] = bcmFieldQualifyInInterface;
 
     fg_info.nof_actions = 1;
 
     fg_info.action_types[0] = bcmFieldActionInvalidNext;
 
     rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_ecmp_rpf_pmf_info.ecmp_lookup_fg_id);
     if(rv != BCM_E_NONE)
     {
         printf("Error (%d) in bcm_field_group_add\n", rv);
         return rv;
     }
 
     bcm_field_group_attach_info_t_init(&attach_info);
 
     attach_info.key_info.nof_quals = fg_info.nof_quals;
     attach_info.payload_info.nof_actions = fg_info.nof_actions;
     for(ii=0; ii< fg_info.nof_quals; ii++)
     {
         attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
     }
     for(ii=0; ii<fg_info.nof_actions; ii++)
     {
         attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
     }
     attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_POSITION(0, 2);
 
     attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
     attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
 
     rv = bcm_field_group_context_attach(unit, 0, cint_field_ecmp_rpf_pmf_info.ecmp_lookup_fg_id, context_id, &attach_info);
     if(rv != BCM_E_NONE)
     {
         printf("Error (%d) in bcm_field_group_context_attach\n", rv);
         return rv;
     }
     
     printf("ECMP lookup FG_ID(%d) was attached to context (%d) \n", cint_field_ecmp_rpf_pmf_info.ecmp_lookup_fg_id, context_id);
 
     bcm_field_group_info_t_init(&fg_info);
     /* We use TCAM instead of direct extraction to qualify on the RPF_ECMF_MODE, which is 2 bits.*/
     fg_info.fg_type = bcmFieldGroupTypeTcam;
     fg_info.nof_quals = 0;
     fg_info.stage = bcmFieldStageIngressPMF3;

     fg_info.nof_quals = 1;
     fg_info.qual_types[0] = bcmFieldQualifyRpfEcmpMode;
 
     fg_info.nof_actions = 1;
 
     fg_info.action_types[0] = bcmFieldActionTrap;
 
     rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_ecmp_rpf_pmf_info.trap_fg_id);
     if(rv != BCM_E_NONE)
     {
         printf("Error (%d) in bcm_field_group_add\n", rv);
         return rv;
     }
 
     bcm_field_group_attach_info_t_init(&attach_info);
 
     attach_info.key_info.nof_quals = fg_info.nof_quals;
     attach_info.payload_info.nof_actions = fg_info.nof_actions;
     for(ii=0; ii< fg_info.nof_quals; ii++)
     {
         attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
     }
     for(ii=0; ii<fg_info.nof_actions; ii++)
     {
         attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
     }
     attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
     attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_POSITION(0, 3);
 
     rv = bcm_field_group_context_attach(unit, 0, cint_field_ecmp_rpf_pmf_info.trap_fg_id, context_id, &attach_info);
     if(rv != BCM_E_NONE)
     {
         printf("Error (%d) in bcm_field_group_context_attach\n", rv);
         return rv;
     }
     
     printf("Trap FG_ID(%d) was attached to context (%d) \n",cint_field_ecmp_rpf_pmf_info.trap_fg_id,context_id);

     bcm_field_entry_info_t_init(&entry_info);
     entry_info.nof_entry_quals = fg_info.nof_quals;
     entry_info.nof_entry_actions = fg_info.nof_actions;
     entry_info.entry_qual[0].type = fg_info.qual_types[0];
     entry_info.entry_qual[0].value[0] = (is_mc ? 3 : 2); /** ECMP_RPF_MODE STRICT_EM value*/
     entry_info.entry_qual[0].mask[0] = 0x3;
     entry_info.entry_action[0].type = fg_info.action_types[0];
     BCM_GPORT_TRAP_SET(trap_gport, cint_field_ecmp_rpf_pmf_info.rpf_uc_ecmp_strict_trap, 7, 0);
     entry_info.entry_action[0].value[0] = trap_gport;
 
     rv = bcm_field_entry_add(unit, 0, cint_field_ecmp_rpf_pmf_info.trap_fg_id, &entry_info, &cint_field_ecmp_rpf_pmf_info.entry_handle_trap);
     if(rv != BCM_E_NONE)
     {
         printf("Error (%d), in bcm_field_entry_add\n", rv);
         return rv;
     }
 
     
     return 0;
     
 }
 
 /**
 * \brief
 *  Clear configuration done by 
 * \param [in] unit        - Device ID
 * \param [in] context_id  - Context to detach FG from. An iPMF3 context.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
 int field_ecmp_rpf_pmf_destroy(int unit, bcm_field_context_t context_id)
 {
     bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
     int rv = BCM_E_NONE;
 
     rv = bcm_field_group_context_detach(unit,cint_field_ecmp_rpf_pmf_info.ecmp_lookup_fg_id,context_id);
     if(rv != BCM_E_NONE)
     {
         printf("Error (%d) in bcm_field_group_context_detach\n", rv);
         return rv;
     }   
     printf("Detach FG_ID(%d) from context (%d) \n",cint_field_ecmp_rpf_pmf_info.ecmp_lookup_fg_id,context_id);
     
     rv = bcm_field_group_delete(unit, cint_field_ecmp_rpf_pmf_info.ecmp_lookup_fg_id);
     if (rv != BCM_E_NONE)
     {
         printf("Error (%d), in bcm_field_group_delete\n", rv);
         return rv;
     }
     printf("Delete FG_ID(%d) \n",cint_field_ecmp_rpf_pmf_info.ecmp_lookup_fg_id);

     rv = bcm_field_entry_delete(unit, cint_field_ecmp_rpf_pmf_info.trap_fg_id, &entry_qual_info[0], cint_field_ecmp_rpf_pmf_info.entry_handle_trap);
     if(rv != BCM_E_NONE)
     {
         printf("Error (%d), in bcm_field_entry_delete\n", rv);
         return rv;
     }
 
     rv = bcm_field_group_context_detach(unit,cint_field_ecmp_rpf_pmf_info.trap_fg_id,context_id);
     if(rv != BCM_E_NONE)
     {
         printf("Error (%d) in bcm_field_group_context_detach\n", rv);
         return rv;
     }
     printf("Detach FG_ID(%d) from context (%d) \n",cint_field_ecmp_rpf_pmf_info.trap_fg_id,context_id);
     
     rv = bcm_field_group_delete(unit, cint_field_ecmp_rpf_pmf_info.trap_fg_id);
     if (rv != BCM_E_NONE)
     {
         printf("Error (%d), in bcm_field_group_delete\n", rv);
         return rv;
     }
     printf("Delete FG_ID(%d) \n",cint_field_ecmp_rpf_pmf_info.trap_fg_id);

     return rv;
     
 }

/**
 * \brief
 *  Adds an entry for an interface that is a part of an ECMP.
 * \param [in] unit        - Device ID
 * \param [in] context_id  - Context to attach FG to. An iPMF3 context.
 * \param [in] ecmp_id     - The ECMP ID to which the interface belong.
 * \param [in] intf_id     - The interface id of the interface for which the entry is added.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
 int field_ecmp_rpf_pmf_entry_add(int unit, bcm_field_context_t context_id, bcm_if_t ecmp_id, int intf_id)
 {
     bcm_field_entry_info_t entry_info;
     bcm_gport_t lif_l3_intf;
     int rv = BCM_E_NONE;

     printf("PMF configuration. Adding entry for ECMP ID %d interface %d. Context %d.\n", ecmp_id, intf_id, context_id); 

     bcm_field_entry_info_t_init(&entry_info);
     entry_info.nof_entry_quals = 2;
     entry_info.nof_entry_actions = 1;
     entry_info.entry_qual[0].type = bcmFieldQualifyDstRpfGport;
     entry_info.entry_qual[0].value[0] = ecmp_id;
     entry_info.entry_qual[1].type = bcmFieldQualifyInInterface;
     BCM_L3_ITF_SET(lif_l3_intf, BCM_L3_ITF_TYPE_RIF, intf_id);
     entry_info.entry_qual[1].value[0] = lif_l3_intf;
     entry_info.entry_action[0].type = bcmFieldActionInvalidNext;
     /* Invalidate_next needs no value.*/
 
     rv = bcm_field_entry_add(unit, 0, cint_field_ecmp_rpf_pmf_info.ecmp_lookup_fg_id, &entry_info, NULL);
     if(rv != BCM_E_NONE)
     {
         printf("Error (%d), in bcm_field_entry_add\n", rv);
         return rv;
     }
     
     return 0;
     
 }
 
 /**
 * \brief
 *  Clear configuration done by 
 * \param [in] unit        - Device ID
 * \param [in] context_id  - Context to detach FG from. An iPMF3 context.
 * \param [in] ecmp_id     - The ECMP ID to which the interface belong.
 * \param [in] intf_id     - The interface id of the interface for which the entry is deleted.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
 int field_ecmp_rpf_pmf_entry_delete(int unit, bcm_field_context_t context_id, bcm_if_t ecmp_id, int intf_id)
 {
    bcm_gport_t lif_l3_intf;
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    int rv = BCM_E_NONE;

    bcm_field_entry_qual_t_init(&(entry_qual_info[0]));
    entry_qual_info[0].type = bcmFieldQualifyDstRpfGport;
    entry_qual_info[0].value[0] = ecmp_id;
    entry_qual_info[1].type = bcmFieldQualifyInInterface;
    BCM_L3_ITF_SET(lif_l3_intf, BCM_L3_ITF_TYPE_RIF, intf_id);
    entry_qual_info[1].value[0] = lif_l3_intf;
    rv = bcm_field_entry_delete(unit, cint_field_ecmp_rpf_pmf_info.ecmp_lookup_fg_id, entry_qual_info, 0);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    printf("Deleted entry for ECMP ID %d interface %d. Context %d.\n", ecmp_id, intf_id, context_id);

    return rv;
    
 }
 
 /**
 * \brief
 *  Configure ECMP RPF.
 * \param [in] unit              - Device ID
 * \param [in] context_id_ipmf3  - iPMF3 Context to attach FG to
 * \param [in] in_port_1        - Port from which the packet enters
 * \param [in] in_port_2        - Another port from which the packet can enter
 * \param [in] out_port       - Port from which the packet is expected to leave
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
 int field_ecmp_rpf_main(int unit, bcm_field_context_t context_id_ipmf3, int in_port_1, int in_port_2, int out_port)
 {
     int rv = BCM_E_NONE;

    rv = field_ecmp_rpf_trap_pre_pmf(unit);
    if (rv != BCM_E_NONE && rv != BCM_E_PARAM)
    {
        printf("Error (%d), field_ecmp_rpf_trap\n", rv);
        return rv;
    }

    rv = field_ecmp_rpf_trap_pmf(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), field_ecmp_rpf_trap\n", rv);
        return rv;
    }

    rv = field_ecmp_rpf_pre_pmf(unit, in_port_1, in_port_2, out_port);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in field_ecmp_rpf_pre_pmf.\n", rv);
        return rv;
    }

    rv = field_ecmp_rpf_pmf(unit, context_id_ipmf3, FALSE);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in field_ecmp_rpf_pmf.\n", rv);
        return rv;
    }

    /* 
     * Add entries for the ECMP interfaces.
     */

    rv = field_ecmp_rpf_pmf_entry_add(unit, context_id_ipmf3, cint_field_ecmp_rpf_pre_pmf_info.ecmp_id, cint_field_ecmp_rpf_pre_pmf_info.intf_in_1);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in field_ecmp_rpf_pmf_entry_add interface %d.\n", rv, cint_field_ecmp_rpf_pre_pmf_info.intf_in_1);
        return rv;
    }

    rv = field_ecmp_rpf_pmf_entry_add(unit, context_id_ipmf3, cint_field_ecmp_rpf_pre_pmf_info.ecmp_id, cint_field_ecmp_rpf_pre_pmf_info.intf_in_2);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in field_ecmp_rpf_pmf_entry_add interface %d.\n", rv, cint_field_ecmp_rpf_pre_pmf_info.intf_in_2);
        return rv;
    } 
    
    return rv;
     
 }
 
/**
 * \brief
 *  Delete the field group and created actions and qualifiers.
 * \param [in] unit              - Device ID
 * \param [in] context_id_ipmf3  - iPMF3 Context to detach FG from
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int field_ecmp_rpf_destroy(int unit, bcm_field_context_t context_id_ipmf3)
{
    int rv = BCM_E_NONE;

    rv = field_ecmp_rpf_pmf_entry_delete(unit, context_id_ipmf3, cint_field_ecmp_rpf_pre_pmf_info.ecmp_id, cint_field_ecmp_rpf_pre_pmf_info.intf_in_1);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in field_ecmp_rpf_pmf_entry_add interface %d.\n", rv, cint_field_ecmp_rpf_pre_pmf_info.intf_in_1);
        return rv;
    }

    rv = field_ecmp_rpf_pmf_entry_delete(unit, context_id_ipmf3, cint_field_ecmp_rpf_pre_pmf_info.ecmp_id, cint_field_ecmp_rpf_pre_pmf_info.intf_in_2);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in field_ecmp_rpf_pmf_entry_add interface %d.\n", rv, cint_field_ecmp_rpf_pre_pmf_info.intf_in_2);
        return rv;
    } 

    rv = field_ecmp_rpf_pmf_destroy(unit, context_id_ipmf3);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in field_ecmp_rpf_pmf_destroy.\n", rv);
        return rv;
    }

    rv = field_ecmp_rpf_pre_pmf_destroy(unit);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in field_ecmp_rpf_pre_pmf_destroy.\n", rv);
        return rv;
    }

    rv = field_ecmp_rpf_trap_pmf_destroy(unit);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in field_ecmp_rpf_trap_pmf_destroy.\n", rv);
        return rv;
    }
     
     
    return rv;
}

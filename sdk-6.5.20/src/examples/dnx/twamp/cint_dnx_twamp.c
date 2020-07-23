/* 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/* 
 * The below CINT demonstrates TWAMP TX/RX and Reflector.
 * Available for 88690_A0 and above.
 *
 * FIXME: The TWAMP accelerated TX/RX will be supported by below:
 *     SDK-159225: TWAMP - implementation accelerated (with SAT) *     
 * How to run: 
 * load TWAMP Reflector soc properties
 *     tm_port_header_type_in_40=ETH
 *     tm_port_header_type_out_40=ETH
 *     ucode_port_40=RCY.0:core_0.40
 * 
 * cint sand/utility/cint_sand_utils_multicast.c
 * cint sand/utility/cint_sand_utils_vlan.c
 * cint sand/utility/cint_sand_utils_l3.c
 * cint sand/cint_ip_route_basic.c
 * cint dnx/crps/cint_crps_oam_config.c
 * cint dnx/twamp/cint_dnx_twamp_field.c 
 * cint dnx/twamp/cint_dnx_twamp.c
 * cint dnx/utility/cint_dnx_util_rch.c
 * cint 
 * int unit = 0; 
 * int in_port = 200;
 * int out_port = 201;
 * int rcy_port = 40;
 * int trap_port = 0;
 * twamp_reflector_example(unit, is_ipv6, in_port, out_portt, rcy_port); 
 * twamp_tx_rx_example(unit, in_port, out_portt, trap_port);
 *
 * TWAMP Reflector Scenario:
 * 1. TWAMP Reflector 1st pass:
 *     1.1 PMF lookup covered - {SIP, DIP, UDP.DST_PORT}.
 *           Action:
 *               bcmFieldActionTrap -
 *               bcmFieldActionOam -
 *                   OAM-Stamp-Offset = 32B, respective to UDP offset (8B + 24B)
 *                   OAM-Offset = 0
 *                   OAM-Sub-Type = TWAMP-REFLECTOR-1ST
 *                   OAM-Up-MEP = 0
 *               bcmFieldActionTtlSet - Set TTL as 255
 *               bcmFieldActionForwardingAdditionalInfo -don't decrement TTL 
 *    1.2 TERM - Terminate and rebuild TWAMP & UDP.
 *         Swap UDP ports, Calculate UDP checksum, Copy Orig TTL & Payload, Stamp RX-TS & Err-Estimate
 *    1.3 FWD - Rebuild FWD header
 *         Swap SIP/DIP, don't dec TTL
 *    1.4 TRAP - Construct RCH
 *
 * TWAMP Reflector 2nd pass:
 *    2.1 PMF lookup covered - {SIP, DIP, UDP.SRC_PORT}.
 *         Action:
 *             bcmFieldActionOam -
 *                 OAM-Stamp-Offset = 12B, respective to UDP offset (8B + 4B)
 *                 OAM-Offset = 0
 *                 OAM-Sub-Type = TWAMP-REFLECTOR-2ND
 *                 OAM-Up-MEP = 0
 *            bcmFieldActionStatId7 - Statistics ID, used for SEQ stamping
 *            bcmFieldActionStatProfile7 - Statistics profile, used for SEQ stamping
 *            bcmFieldActionForwardingAdditionalInfo -don't decrement TTL 
 *    2.2 TERM - Terminate and rebuild TWAMP & UDP.
 *          Calculate UDP checksum, Stamp TX-TS & SEQ    
 *
 *  Traffic
 *  1. Send: 
 *      tx 1 psrc=200 data=0x000c000200880000000255008100000a08004500004700000000ff119da20a00000214000002c351035e0033000000000002000000001122334443040000000000000000000000000000000000000000000000000000000000
 *      -----------------------------------------------------------------------
 *      | ETH: DA(000C00020088 ), SA(000000025500)                                                      |
 *      -----------------------------------------------------------------------
 *      | IPv4: SIP(10.0.0.2), DIP(20.0.0.2)                                                                       |
 *      -----------------------------------------------------------------------
 *      | UDP: SRC(50001), DST(862)                                                                               |
 *      -----------------------------------------------------------------------
 *      | TWAMP: Sender-SEQ(4), Sender-TS(8),  Sender-Error-Estimate(2), Pad Zero(27)  |
 *      ----------------------------------------------------------------------- 
 *     
 *  2. Receive: 
 *      0x00000000cd1d000c000200888100200a08004501004700000000ff119da1140000020a000002035ec3510033c7f700000004000000430c539e888506000000000042bdc54ad600000002000000001122334443040000ff000000000000000000000000000000000000
 *      -------------------------------------------------------------------------------------------
 *      | ETH: DA(00000000CD1D), SA(000C00020088)                                                                                        |
 *      -------------------------------------------------------------------------------------------
 *      | IPv4: SIP(20.0.0.2), DIP(10.0.0.2)                                                                                                          |
 *      -------------------------------------------------------------------------------------------
 *      | UDP: SRC(862), DST(50001)                                                                                                                  |
 *      -------------------------------------------------------------------------------------------
 *      | TWAMP: Reflector-SEQ(4), Reflector-TX-TS(8),  Reflector-Error-Estimate(2), MBZ(2), Reflector-RX-TS(8)  |
 *      | Sender-SEQ(4), Sender-TS(8),  Sender-Error-Estimate(2), MBZ(2), Sender-TTL(1)                                   |
 *      -------------------------------------------------------------------------------------------  
 *
 * TWAMP RX Scenario:
 *     PMF lookup covered - {SIP, DIP, UDP.DST_PORT}.
 *           Action:
 *               bcmFieldActionTrap -
 *                   Trap to CPU
 *               bcmFieldActionOam -
 *                   OAM-Stamp-Offset = 32B, respective to UDP offset (8B + 24B)
 *                   OAM-Offset = 0
 *                   OAM-Sub-Type = DM-NTP
 *                   OAM-Up-MEP = 0
 *               bcmFieldActionTtlSet - Set TTL as 255
 *               bcmFieldActionForwardingAdditionalInfo -don't decrement TTL 
 *
 *  Traffic
 *  1. Send: 
 *      tx 1 psrc=200 data=0x000c000100880000000255008100000a08004500004700000000ff119da2140000020a000002c351035e0033000000000001000000000000000000010000000000000000000000000001000000000000111100010000ff0000
 *     
 *  2. Receive (TWAMPoUDPoIPv4oETH1oUDH-BASE-J2oPPH_OUT_LIF_EXT-J2oFHEI-Trap5B-J2oPPH-J2oTSH-J2oASE-OAMoFTMH-LB-EXT-J2oFTMH-J2):
 *      0x0214006400000000a3e8cc17f3364369b50a9030448d0804040e000a00005007fa2083000246819510000100d0c8000c000100880000000255008100000a08004500004700000000ff119da2140000020a000002c351035e0033000000000001000000000000000000010000000000000000000000000001000000000000111100010000ff0000
 *
 * TWAMP TX Scenario:
 *    PMF lookup covered - {SIP, DIP, UDP.SRC_PORT}.
 *         Action:
 *             bcmFieldActionOam -
 *                 OAM-Stamp-Offset = 12B, respective to UDP offset (8B + 4B)
 *                 OAM-Offset = 0
 *                 OAM-Sub-Type = DM-NTP
 *                 OAM-Up-MEP = UP
 *    FWD - Forward by IP address.
 *
 *  Traffic
 *  1. Send: 
 *      tx 1 psrc=200 data=0x000c000200880000000255008100001408004500004700000000ff119da20a00000214000002c351035e0033000000000001000000000000000000010000000000000000000000000001000000000000111100010000ff0000
 *     
 *  2. Receive:
 *      0x00000000ad1d000c000200888100002808004500004700000000ff119da20a00000214000002c351035e003300000000000100000151ae2e6c5200010000000000000000000000000001000000000000111100010000ff0000
 */

struct twamp_basic_info_s{
    int is_reflector;               /* indicates the reflector */
    int in_port;                    /* incoming port */                       
    int out_port;                   /* outgoing port */
    int in_rif[2];                  /* in RIF */
    bcm_gport_t vlan_port_id[2];    /* vlan_port_id */
    int in_vrf[2];                  /* in VRF */      
    int out_rif[2];                 /* out RIF */               
    bcm_mac_t my_mac[2];            /* my mac */      
    bcm_mac_t nexp_hop_mac[2];      /* next hop mac */  
    bcm_ip_t dip[2];                /* IPv4 DIP */
    bcm_ip6_t dip6[2];              /* IPv6 DIP */
};

twamp_basic_info_s twamp_info = 
{
    /* is_reflector */
    0,
    /* in_port */
    0,
    /* out_port */
    0,
    /* in_rif */
    { 10, 20 },
    /* vlan_port_id */
    { 0, 0 },
    /* in_vrf */
    { 5, 15 },
    /* out_rif */
    { 30, 40 },
    /* my_mac */
    {{0x00, 0x0c, 0x00, 0x01, 0x00, 0x88}, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x88}},
    /* next_hop_mac */
    {{0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}, {0x00, 0x00, 0x00, 0x00, 0xad, 0x1d}},
    /* dip */
    {0x0a000002, 0x14000002},
    /* dip6 */
    {
        {0x20, 0x00, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE},
        {0x20, 0x00, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE}
    }    
};


/**
 * \brief
 *
 *  This function creates the TWAMP encapsulation.
 *
 * \param [in] unit - The unit number.  
 * \param [in] is_ipv6 - The flag indicates it is the ipv6.
 * \param [in] next_outlif - The RCH encapuslation outlif.
 * \param [out] recycle_entry_encap_id - The TWAMP encapuslation id.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int twamp_create_reflector(
    int unit,
    int is_ipv6,
    int next_outlif,
    int *twamp_encap_id)
{
    int rv = BCM_E_NONE;
    bcm_switch_reflector_data_t twamp_reflector;

    twamp_reflector.type = is_ipv6 ? bcmSwitchReflectorTwampIp6 : bcmSwitchReflectorTwampIp4;
    /* error_estimate(16) = s_flag(1) + z_flag(1) + scale(6) + multiplier(8)
        *  s_flag(1) = 1, z_flag(1) =0, scale(6) =5, multiplier(8) =6
        * error_estimate(16) = b1-0-000101-00000110 = b1000010100000110=0x8506
        */
    twamp_reflector.error_estimate = 0x8506;
    twamp_reflector.next_encap_id = next_outlif;

    rv = bcm_switch_reflector_create(unit, 0, twamp_encap_id, &twamp_reflector);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_reflector_create \n");
        return rv;
    }

    printf("recycle_entry_encap_id:0x%x, twamp_encap_id:0x%x\n", next_outlif, *twamp_encap_id);

    return rv;
}

/**
 * \brief
 *
 *  This function creates the TWAMP snoop.
 *
 * \param [in] unit - The unit number.  
 * \param [in] sat_port - The SAT port.
 * \param [out] action_profile_id - The action profile id.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int twamp_snoop_set(
    int unit,
    int sat_port,
    int *action_profile_id)
{
    int rv = BCM_E_NONE;
    bcm_mirror_destination_t mirror_dest;

    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.flags = BCM_MIRROR_DEST_IS_SNOOP;
    mirror_dest.gport = sat_port;
    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_mirror_destination_create \n");
        return rv;
    }

    *action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest.mirror_dest_id);

    return rv;
}


/**
 * \brief
 *
 *  This function creates the TWAMP CRPS.
 *
 * \param [in] unit - The unit number.  
 * \param [in] rcy_port - The recycle port.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int twamp_create_crps(
    int unit,
    int rcy_port)
{
    int rv = BCM_E_NONE;
    int core_id, tm_port;
    int command_id = 7;
    int engine = 3;
    int counter_base =0;
    int database_id = 0;
    uint32 total_nof_counters_get = 0;

    rv = get_core_and_tm_port_from_port(unit, rcy_port, &core_id, &tm_port);
    if (rv != BCM_E_NONE) {
       printf("Error, in get_core_and_tm_port_from_port\n");
       return rv;
    }
    
    rv = crps_oam_database_set(unit, core_id, bcmStatCounterInterfaceIngressOam, command_id, 1, &engine, counter_base, database_id, &total_nof_counters_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error, crps_oam_database_set \n");
        return rv;
    }
    
    return rv;
}

/**
 * \brief
 *
 *  This function creates the TWAMP service.
 *
 * \param [in] unit - The unit number.  
 * \param [in] in_port - The in port.
 * \param [in] out_port - The out port.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int twamp_service_setup(int unit, int in_port, int out_port) {
    int i, vrf, rv = BCM_E_NONE;    
    sand_utils_l3_eth_rif_s eth_rif_structure;
    sand_utils_l3_arp_s arp_structure;
    sand_utils_l3_fec_s fec_structure;
    bcm_ip6_t ipv6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00};    

    twamp_info.in_port = in_port;
    twamp_info.out_port = out_port;
    
    for (i = 0; i < 2; i++) {
        /* Add in_port to vlan membership */
        rv = bcm_vlan_gport_add(unit, twamp_info.in_rif[i], twamp_info.in_port, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("Error, bcm_vlan_gport_add %d\n", rv);
            return rv;
        }

        /* Set <port, vlan> -> LIF and LIF -> VSI */
        rv = vlan__vlan_port_vsi_create(unit, twamp_info.in_rif[i], twamp_info.in_port, twamp_info.in_rif[i], BCM_VLAN_INVALID, 0, &twamp_info.vlan_port_id[i]);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in_port_intf_set access_eth_rif %d\n", rv);
            return rv; 
        }

        /* Set RIF -> My-Mac  and RIF -> VRF */
        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, twamp_info.in_rif[i], 0, 0, twamp_info.my_mac[i], twamp_info.in_vrf[i]);
        rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create access_eth_rif %d\n", rv);
            return rv;
        } 

        /* Add out_port to vlan membership */
        rv = bcm_vlan_gport_add(unit, twamp_info.out_rif[i], twamp_info.out_port, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("Error, bcm_vlan_gport_add %d\n", rv);
            return rv;
        }

        /*** create arp ***/
        sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, 0, 0, 0, twamp_info.nexp_hop_mac[i], twamp_info.out_rif[i]);
        rv = sand_utils_l3_arp_create(unit, &arp_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_arp_create\n");
            return rv;
        }

        /*** create fec ***/
        sand_utils_l3_fec_s_common_init(unit, 0, 0, &fec_structure, 0, 0, 0, twamp_info.out_port, twamp_info.in_rif[i], arp_structure.l3eg_arp_id);
        rv = sand_utils_l3_fec_create(unit, &fec_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_fec_create\n");
            return rv;
        }
        printf("FEC created: %d\n",fec_structure.l3eg_fec_id);

        /*** Set the reflector 2nd pass VRF as 0 ***/
        if ((i == 1) && twamp_info.is_reflector) {
            vrf = 0;
        } else {
            vrf = twamp_info.in_vrf[i];
        }

        /*** create IPv4 l3 route  ***/
        rv = add_route_ipv4(unit, twamp_info.dip[i], 0xffffffff, vrf, fec_structure.l3eg_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_route_ipv4 \n");
            return rv;
        }

        /*** create IPv6 l3 route  ***/
        rv = add_route_ipv6(unit, twamp_info.dip6[i], ipv6_mask, vrf, fec_structure.l3eg_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_route_ipv4 \n");
            return rv;
        }
    }

    return rv;
}

/**
 * \brief
 *
 *  This function creates the TWAMP reflector application.
 *
 * \param [in] unit - The unit number.  
 * \param [in] is_ipv6 - The flag indicates it is the ipv6.
 * \param [in] in_port - The in port.
 * \param [in] out_port - The out port.
 * \param [in] rcy_port - The recycle port.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int twamp_reflector_example(int unit, int is_ipv6, int in_port, int out_port, int rcy_port) {
    int rv = BCM_E_NONE;    
    int trap_id = 0;    
    bcm_rx_trap_config_t config;
    int trap_strength = 7;
    bcm_gport_t gport_trap = 0;
    int recycle_entry_encap_id;
    int twamp_encap_id;
    bcm_ip_t local_ip_addr = twamp_info.dip[0];
    bcm_ip_t remote_ip_addr = twamp_info.dip[1];
    bcm_ip6_t local_ip6_addr;
    bcm_ip6_t remote_ip6_addr;

    sal_memcpy(&local_ip6_addr,&(twamp_info.dip6[0]),16);
    sal_memcpy(&remote_ip6_addr,&(twamp_info.dip6[1]),16);

    twamp_info.is_reflector = 1;
    
    /*** create TWAMP service ***/
    rv = twamp_service_setup(unit, in_port, out_port);
    if (rv != BCM_E_NONE) {
        printf("twamp_service_setup $rv");
    }

    /*** create RCH encap ***/
    rv = create_recycle_entry(unit, &recycle_entry_encap_id);
    if (rv != BCM_E_NONE) {
        printf("create_recycle_entry $rv");
    }

    /*** create TWAMP encap ***/
    rv = twamp_create_reflector(unit, is_ipv6, recycle_entry_encap_id, &twamp_encap_id);
    if (rv != BCM_E_NONE) {
        printf("twamp_create_reflector $rv");
    }

    /*** create CRPS used for SEQ ***/
    rv = twamp_create_crps(unit, rcy_port);
    if (rv != BCM_E_NONE) {
        printf("twamp_create_crps $rv");
    }

    /*** create trap ***/
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_rx_trap_type_create $rv");
    }
    bcm_rx_trap_config_t_init(&config);
    config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_ENCAP_ID;
    config.dest_port = rcy_port;
    config.trap_strength = 0;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(config.encap_id, twamp_encap_id);
    rv = bcm_rx_trap_set(unit, trap_id, &config);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }        
    BCM_GPORT_TRAP_SET(gport_trap, trap_id, trap_strength, 0);
    printf("gport_trap1:0x%x, trap_id:0x%x, config.encap_id:0x%x\n", gport_trap, trap_id, config.encap_id);

    /*** create PMF rule ***/
    rv = cint_field_twamp_reflector_rule(unit, is_ipv6, gport_trap, local_ip_addr, remote_ip_addr, &local_ip6_addr, &remote_ip6_addr);
    if (rv != BCM_E_NONE) {
        printf("cint_field_twamp_reflector_rule $rv");
    }

    return rv;
}

/**
 * \brief
 *
 *  This function creates the TWAMP reflector application.
 *
 * \param [in] unit - The unit number.  
 * \param [in] in_port - The in port.
 * \param [in] out_port - The out port.
 * \param [in] trap_port - The trap port.
 * \param [in] ctf_id - The ctf id.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int twamp_tx_rx_example(int unit, int in_port, int out_port, int trap_port, int ctf_id) {
    int rv = BCM_E_NONE;    
    int trap_id = 0;    
    bcm_rx_trap_config_t config;
    int trap_strength = 7;
    bcm_gport_t gport_trap = 0;
    bcm_ip_t local_ip_addr = twamp_info.dip[0];
    bcm_ip_t remote_ip_addr = twamp_info.dip[1];

    twamp_info.is_reflector = 0;

    /*** create TWAMP service ***/
    rv = twamp_service_setup(unit, in_port, out_port);
    if (rv != BCM_E_NONE) {
        printf("twamp_service_setup $rv");
    }      

    /*** create trap ***/
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_rx_trap_type_create $rv");
    }

    bcm_rx_trap_config_t_init(&config);
    config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    config.dest_port = trap_port;
    config.trap_strength = 0;
    rv = bcm_rx_trap_set(unit, trap_id, &config);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }    
    BCM_GPORT_TRAP_SET(gport_trap, trap_id, trap_strength, 0);
    printf("gport_trap:0x%x, trap_id:0x%x\n", gport_trap, trap_id);

    if (ctf_id != -1) {
        rv = bcm_sat_ctf_trap_add(unit, trap_id);
        if (rv != BCM_E_NONE) {
            printf("bcm_sat_ctf_trap_add $rv");
        }
    }
    /*** create PMF rule ***/
    rv = cint_field_twamp_tx_rx_rule(unit, gport_trap, ctf_id, local_ip_addr, remote_ip_addr);
    if (rv != BCM_E_NONE) {
        printf("cint_field_twamp_tx_rx_rule $rv");
    }

    return rv;
}



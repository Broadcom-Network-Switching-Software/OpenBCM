/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Example of a router with RPF scenario
 * Test Scenario
 *
 * ./bcm.user
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/sand/cint_ip_route_rpf_basic.c
 * cint
 * dnx_example_ipv4_rpf(unit, inP_loose, inP_strict, outP, vsi_loose, vsi_strict, vrf_loose, vrf_strict, vpbr_enabled)
 *                          with valid parameters;
 * exit;
 *
 *
 *
 * Example of a router with MC RPF scenario
 * Test Scenario
 *
 * ./bcm.user
 * cint src/examples/sand/cint_ipmc_route_basic.c
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/sand/cint_ip_route_rpf_basic.c
 * cint
 * dnx_example_ipv4_mc_rpf(unit, in_port1, in_port2, out_port1, out_port2, vid) with valid parameters;
 * exit;
 */

struct cint_ip_route_rpf_basic_info_s
{
    int in_port_loose;                                  /** incoming port */
    int in_port_strict;                                 /** incoming port */
    int out_port;                                       /** outgoing port */
    int intf_in_loose;                                  /** Incoming packet ETH-RIF */
    int intf_in_strict;                                 /** Incoming packet ETH-RIF */
    int intf_out;                                       /** Outgoing packet ETH-RIF */
    bcm_mac_t intf_in_mac_address;                      /** mac for in RIF */
    bcm_mac_t intf_out_mac_address;                     /** mac for out RIF */
    bcm_mac_t arp_next_hop_mac;                         /** mac for out RIF */
    int encap_id;                                       /** encap ID */
    bcm_ip_t host_dip;                                  /** host */
    bcm_ip_t host_sip;                                  /** Source IP */
    bcm_ip_t host_mc;                                   /** Dest Mc IP */
    int vrf_loose;                                      /** VRF loose */
    int vrf_strict;                                     /** VRF strict */
    bcm_if_t fec_id;                                    /** FEC id */
};

cint_ip_route_rpf_basic_info_s cint_rpf_route_info =
{
    /*
     * ports : in_port_loose | in_port_strict | out_port
     */
    200, 201, 202,
    /*
     * intf_in_loose | intf_in_strict | intf_out
     */
    15, 20, 100,
    /*
     * intf_in_mac_address | intf_out_mac_address | arp_next_hop_mac
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x12, 0x34, 0x56, 0x78, 0x9a}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * encap_id
     */
    0x1384,
    /*
     * host_dip
     */
    0x7fffff02 /** 127.255.255.2 */,
    /*
     * host_sip
     */
    0xc0800101 /** 192.128.1.1 */,
    /*
     * host_mc
     */
    0xe0020202 /** 224.2.2.2 */,
    /*
     * vrf_loose
     */
    1,
    /*
     * vrf_strict
     */
    2,
    /*
     * fec_id
     */
    0xA001,
};


/*
 * Utilities APIs
 */
/*
 * Set global uRPF for DPP device. Relevant only to DPP device.
 */

int
l3_dpp_uc_rpf_config(
    int *units_ids,
    int nof_units,
    bcm_l3_ingress_urpf_mode_t urpf_mode)
{
    int rv = BCM_E_NONE;
    int i, unit;
    int is_urpf_mode_per_rif;

    for (i = 0; i < nof_units; i++)
    {
        unit = units_ids[i];

        if (is_device_or_above(unit, JERICHO2)) {
            printf("Unavailable, setting global uRPF relevant only to DPP device\n");
            continue;
        }

        is_urpf_mode_per_rif = soc_property_get(unit , "bcm886xx_l3_ingress_urpf_enable",0);
        if (!is_urpf_mode_per_rif) {
            /* Set uRPF global configuration */
            rv =  bcm_switch_control_set(unit, bcmSwitchL3UrpfMode, urpf_mode);
            if (rv != BCM_E_NONE) {
                return rv;
            }
        }
    }

    return rv;
}

/*
 * Utilities APIs
 */
/*
 * Set RPF traps:
 */

int
l3_dnx_ip_rpf_config_traps(
    int *units_ids,
    int nof_units)
{
    int rv = BCM_E_NONE, i;
    bcm_rx_trap_config_t config;
    int flags = 0;
    int unit;
    int rpf_uc_strict_trap = 0, rpf_uc_loose_trap = 0;
    /*
     * RPF types:
     */
    /*
     * bcmRxTrapUcLooseRpfFail, : Forwarding Code is IPv4 UC and RPF FEC Pointer Valid is not set.
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
    for (i = 0; i < nof_units; i++)
    {
        unit = units_ids[i];

        rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapUcStrictRpfFail, &rpf_uc_strict_trap);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_rx_trap_type_get rpf_uc_strict_trap \n");
            return rv;
        }
        rv = bcm_rx_trap_set(unit, rpf_uc_strict_trap, &config);
        if (rv != BCM_E_NONE)
        {
            rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUcStrictRpfFail, &rpf_uc_strict_trap);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in trap create, trap bcmRxTrapUcStrictRpfFail \n");
                return rv;
            }
            rv = bcm_rx_trap_set(unit, rpf_uc_strict_trap, &config);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in trap set rpf_uc_strict_trap\n");
                return rv;
            }
        }
    }

    /*
     * set uc loose to drop packet
     */
    for (i = 0; i < nof_units; i++)
    {
        unit = units_ids[i];
        rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapUcLooseRpfFail, &rpf_uc_loose_trap);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_rx_trap_type_get rpf_uc_loose_trap \n");
            return rv;
        }
        rv = bcm_rx_trap_set(unit, rpf_uc_loose_trap, &config);
        if (rv != BCM_E_NONE)
        {
            rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUcLooseRpfFail, &rpf_uc_loose_trap);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in trap create, trap bcmRxTrapUcLooseRpfFail \n");
                return rv;
            }
            rv = bcm_rx_trap_set(unit, rpf_uc_loose_trap, &config);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in trap set rpf_uc_loose_trap\n");
                return rv;
            }
        }
    }
    return rv;
}

/*
 * This function sets the RPF traps for the case of MC RPF.
 * These include:
 *    - bcmRxTrapMcUseSipRpfFail - trap for packets that are failing a SIP-based RPF check.
 *    - bcmRxTrapMcExplicitRpfFail - trap for packets that are failing an Explicit RPF check.
 */
int
l3_dnx_ip_mc_rpf_config_traps(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    int rpf_mc_sip_trap = 0;
    int rpf_mc_explicit_trap = 0;
    /*
     * RPF types: bcmRxTrapMcUseSipRpfFail - Out-RIF is not equal to In-RIF when MC-RPF-Mode is 'Use-SIP-WITH-ECMP'.
     * bcmRxTrapMcExplicitRpfFail - RPF-Entry. Expected-InRIF is not equal to packet InRIF. Relevant when FEC-Entry.
     */

    /*
     * change dest port for trap
     */
    bcm_rx_trap_config_t_init(&config);
    config.flags |= (BCM_RX_TRAP_UPDATE_DEST);
    config.trap_strength = 7;
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    /*
     * set mc sip-based and explicit RPF to drop packet
     */
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapMcUseSipRpfFail, &rpf_mc_sip_trap);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_type_get rpf_mc_sip_trap \n");
        return rv;
    }
    rv = bcm_rx_trap_set(unit, rpf_mc_sip_trap, &config);
    if (rv != BCM_E_NONE)
    {
        rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapMcUseSipRpfFail, &rpf_mc_sip_trap);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in trap create, trap bcmRxTrapMcUseSipRpfFail \n");
            return rv;
        }
        rv = bcm_rx_trap_set(unit, rpf_mc_sip_trap, &config);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in trap set rpf_mc_sip_trap\n");
            return rv;
        }
    }
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapMcExplicitRpfFail, &rpf_mc_explicit_trap);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_type_get rpf_mc_explicit_trap \n");
        return rv;
    }
    rv = bcm_rx_trap_set(unit, rpf_mc_explicit_trap, &config);
    if (rv != BCM_E_NONE)
    {
        rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapMcExplicitRpfFail, &rpf_mc_explicit_trap);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in trap create, trap bcmRxTrapMcExplicitRpfFail \n");
            return rv;
        }
        rv = bcm_rx_trap_set(unit, rpf_mc_explicit_trap, &config);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in trap set rpf_mc_explicit_trap\n");
            return rv;
        }
    }

    return rv;
}

/*
 * Set In-Port default ETH-RIF:
 * - in_port: Incoming port ID
 * - eth_rif: ETH-RIF
 */
int
intf_ingress_rif_rpf_set(
    int unit,
    int eth_rif_id,
    int vrf,
    bcm_l3_ingress_urpf_mode_t urpf_mode)
{
    bcm_l3_ingress_t l3_ing_if;
    int rc;

    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;   /* must, as we update exist RIF */
    l3_ing_if.vrf = vrf;
    l3_ing_if.urpf_mode = urpf_mode;

    rc = bcm_l3_ingress_create(unit, l3_ing_if, eth_rif_id);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ingress_create\n");
        return rc;
    }

    return rc;
}

/**
 * Set In-Port to Eth-RIF, Create In-RIF and define it as RPF.
 *  - in_port - Incoming port ID
 *  - intf_in - Incoming interface ID
 *  - vrf - Virtual router instance
 *  - urpf_mode - UC RPF Mode - Strict or Loose
 */
int cint_ip_rpf_port_intf_set(
    int unit,
    int in_port,
    int intf_in,
    int vrf,
    bcm_l3_ingress_urpf_mode_t urpf_mode)
{
    int rv = BCM_E_NONE;
    /** Set In-Port to Eth-RIF */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }
    /** Create IN-RIF and set its properties */
    rv = intf_eth_rif_create(unit, intf_in, cint_rpf_route_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }
    /** Indicate RPF lookup for interface if urpf_mode is defined */
    if (urpf_mode != 0)
    {
        rv = intf_ingress_rif_rpf_set(unit, intf_in, vrf, urpf_mode);
        if (rv != BCM_E_NONE)
        {
            printf("Error, intf_ingress_rif_rpf_set intf_in vrf\n");
            return rv;
        }
    }
    else
    {
        l3_ingress_intf ingress_rif;
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = vrf;
        ingress_rif.intf_id = intf_in;
        rv = intf_ingress_rif_set(unit, &ingress_rif);
        if (rv != BCM_E_NONE)
        {
            printf("Error, intf_ingress_rif_set\n");
            return rv;
        }
    }

    return rv;
}
/*
 * Main API - configure interface and hosts
 *
 * This function is configure basic rpf check configuration for strict and loose modes when Packet is a IPv4 packet..
 *
 * in_port_loose : Incoming port on which Loose mode checks are done
 * in_port_strict : Incoming port on which Strict mode checks are done
 * vsi_loose : Incoming interface which configured as Loose mode.
 * vsi_strict : Incoming interface which configured as Strict mode.
 * vrf_loose,vrf_strict : VRF associated with vsi_loose, vsi_strict interfaces respectively.
 */
int
dnx_example_ipv4_rpf(
    int unit,
    int in_port_loose,
    int in_port_strict,
    int out_port,
    int vsi_loose,
    int vsi_strict,
    int vrf_loose,
    int vrf_strict,
    int vpbr_enabled)
{
    int rv;
    int fec;
    int fec_rpf_loose;
    int fec_rpf_strict;
    int fec_rpf_strict_kaps;
    int fec_rpf_strict_kaps_default_route;
    int updated_vrf_loose = vrf_loose;
    int updated_vrf_strict = vrf_strict;
    bcm_gport_t gport = 0;

    if (is_device_or_above(unit, JERICHO2))
    {
       rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_rpf_route_info.fec_id);
       if (rv != BCM_E_NONE)
       {
           printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
           return rv;
       }
    }

    fec = cint_rpf_route_info.fec_id;
    fec_rpf_loose = fec + 2;
    fec_rpf_strict = fec + 4;
    fec_rpf_strict_kaps = fec + 6;
    fec_rpf_strict_kaps_default_route = fec + 8;
    cint_rpf_route_info.encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;
    cint_rpf_route_info.intf_in_loose = vsi_loose;
    cint_rpf_route_info.intf_in_strict = vsi_strict;
    cint_rpf_route_info.in_port_loose = in_port_loose;
    cint_rpf_route_info.in_port_strict = in_port_strict;
    cint_rpf_route_info.out_port = out_port;


    /** Config traps  */
    rv = l3_dnx_ip_rpf_config_traps(&unit, 1);
    if (rv != BCM_E_NONE && rv != BCM_E_PARAM)
    {
        printf("Error, l3_dnx_ip_rpf_config_traps\n");
        return rv;
    }
    /*
     * Set In-Port to In ETh-RIF, Create IN-RIFs and set their properties
     * 1 - loose
     * 2 - strict
     */
    rv = cint_ip_rpf_port_intf_set(unit, cint_rpf_route_info.in_port_loose, cint_rpf_route_info.intf_in_loose,
                                      vrf_loose, bcmL3IngressUrpfLoose);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_ip_rpf_port_intf_set - Loose\n");
        return rv;
    }
    rv = cint_ip_rpf_port_intf_set(unit, cint_rpf_route_info.in_port_strict, cint_rpf_route_info.intf_in_strict,
                                      vrf_strict, bcmL3IngressUrpfStrict);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_ip_rpf_port_intf_set - Strict\n");
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_rpf_route_info.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set\n");
        return rv;
    }

    BCM_GPORT_LOCAL_SET(gport, cint_rpf_route_info.out_port);
    rv = l3__egress_only_fec__create(unit, fec_rpf_loose, cint_rpf_route_info.intf_in_loose, 0/*encap_id_rpf_loose*/, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, fec_rpf_loose\n");
        return rv;
    }

    rv = l3__egress_only_fec__create(unit, fec_rpf_strict, cint_rpf_route_info.intf_in_strict, 0/*encap_id_rpf_strict*/, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, intf_in_strict\n");
        return rv;
    }

    rv = l3__egress_only_fec__create(unit, fec_rpf_strict_kaps, cint_rpf_route_info.intf_in_strict, 0/*encap_id_rpf_strict*/, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, intf_in_strict\n");
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, fec_rpf_strict_kaps_default_route, cint_rpf_route_info.intf_in_strict, 0/*encap_id_rpf_strict*/, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, intf_in_strict\n");
        return rv;
    }
    /** Create OUT-LIF and set its properties: FEC, encap-id */
    rv = intf_eth_rif_create(unit, cint_rpf_route_info.intf_out, cint_rpf_route_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, fec, cint_rpf_route_info.intf_out, cint_rpf_route_info.encap_id, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, intf_out\n");
        return rv;
    }
    rv = l3__egress_only_encap__create(unit, 0, &cint_rpf_route_info.encap_id, cint_rpf_route_info.arp_next_hop_mac, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only, encap_id\n");
        return rv;
    }

    /**
     *  Add Route entries
     */
    if (vpbr_enabled)
    {
        /*
         * if this flag is set then we want to create host with different VRF,
         * and check if the PBR works and the VRF is updated and the packet is redirected to
         * the new one.
         * here we use vrf_loose + 1 and vrf_strict + 1
         */
        updated_vrf_loose = vrf_loose + 1;
        updated_vrf_strict = vrf_strict + 1;
    }
    /*
     * Loose mode entries
     */
    rv = add_host_ipv4(unit, cint_rpf_route_info.host_dip, updated_vrf_loose, fec, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host, dip, vrf_loose\n");
        return rv;
    }

    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit)) {
        rv = add_route_ipv4_rpf(unit, cint_rpf_route_info.host_sip, 0xffffffff, vrf_loose, fec);
    } else {
        rv = add_host_ipv4(unit, cint_rpf_route_info.host_sip, vrf_loose, fec, 0,0);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host, sip_1, vrf_loose, fec\n");
        return rv;
    }

    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit)) {
        rv = add_route_ipv4_rpf(unit, cint_rpf_route_info.host_sip + 1, 0xffffffff, vrf_loose, fec_rpf_loose);
    } else {
        rv = add_host_ipv4(unit, cint_rpf_route_info.host_sip + 1, vrf_loose, fec_rpf_loose, 0, 0);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host, sip_2, vrf_loose, fec_rpf_loose\n");
        return rv;
    }

    /*
	 * Strict mode entries
	 */
    rv = add_host_ipv4(unit, cint_rpf_route_info.host_dip, updated_vrf_strict, fec, 0,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host, dip, vrf_strict\n");
        return rv;
    }

    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit)) {
        rv = add_route_ipv4_rpf(unit, cint_rpf_route_info.host_sip, 0xffffffff, vrf_strict, fec);
    } else {
        rv = add_host_ipv4(unit, cint_rpf_route_info.host_sip, vrf_strict, fec, 0,0);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host, sip_1, vrf_strict, fec\n");
        return rv;
    }

    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit)) {
        rv = add_route_ipv4_rpf(unit, cint_rpf_route_info.host_sip + 1, 0xffffffff, vrf_strict, fec_rpf_strict);
    } else {
        rv = add_host_ipv4(unit, cint_rpf_route_info.host_sip + 1, vrf_strict, fec_rpf_strict, 0,0);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host, sip_2, vrf_strict, fec_rpf_strict\n");
        return rv;
    }

    return rv;
}

/*
 * Main API - configure interface and hosts
 *
 * This function is configure basic rpf check configuration for strict and loose modes when Packet is a IPv6 packet..
 *
 * in_port_loose : Incoming port on which Loose mode checks are done
 * in_port_strict : Incoming port on which Strict mode checks are done
 * vsi_loose : Incoming interface which configured as Loose mode.
 * vsi_strict : Incoming interface which configured as Strict mode.
 * vrf_loose,vrf_strict : VRF associated with vsi_loose, vsi_strict interfaces respectively.
 */
int
dnx_example_ipv6_rpf(
    int unit,
    int in_port_loose,
    int in_port_strict,
    int out_port,
    int vsi_loose,
    int vsi_strict,
    int vrf_loose,
    int vrf_strict,
    int vpbr_enabled)
{
    int rv;

    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_rpf_route_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }

    int fec = cint_rpf_route_info.fec_id;
    int fec_rpf_loose = fec + 2;
    int fec_rpf_strict = fec + 4;
    int fec_rpf_strict_kaps = fec + 6;
    int fec_rpf_strict_kaps_default_route = fec + 8;
    int updated_vrf_loose = vrf_loose;
    int updated_vrf_strict = vrf_strict;
    bcm_gport_t gport = 0;
    bcm_ip6_t host_dip = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x13 }; /*Dest IP address*/
    bcm_ip6_t host_sip_1 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbb, 0xaa }; /*Source IP address 1*/
    bcm_ip6_t host_sip_2 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcd, 0xdd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x78 }; /*Source IP address 2*/
    bcm_ip6_t full_mask = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; /*Full mask*/

    cint_rpf_route_info.encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;
    cint_rpf_route_info.intf_in_loose = vsi_loose;
    cint_rpf_route_info.intf_in_strict = vsi_strict;
    cint_rpf_route_info.in_port_loose = in_port_loose;
    cint_rpf_route_info.in_port_strict = in_port_strict;
    cint_rpf_route_info.out_port = out_port;

    /** Config traps  */
    rv = l3_dnx_ip_rpf_config_traps(&unit, 1);
    if (rv != BCM_E_NONE && rv != BCM_E_PARAM)
    {
        printf("Error, l3_dnx_ip_rpf_config_traps\n");
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF, Create IN-RIFs and set their properties
     * 1 - loose
     * 2 - strict
     */
    rv = cint_ip_rpf_port_intf_set(unit, cint_rpf_route_info.in_port_loose, cint_rpf_route_info.intf_in_loose,
                                      vrf_loose, bcmL3IngressUrpfLoose);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_ip_rpf_port_intf_set - Loose\n");
        return rv;
    }
    rv = cint_ip_rpf_port_intf_set(unit, cint_rpf_route_info.in_port_strict, cint_rpf_route_info.intf_in_strict,
                                      vrf_strict, bcmL3IngressUrpfStrict);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_ip_rpf_port_intf_set - Strict\n");
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_rpf_route_info.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set\n");
        return rv;
    }

    BCM_GPORT_LOCAL_SET(gport, cint_rpf_route_info.out_port);
    rv = l3__egress_only_fec__create(unit, fec_rpf_loose, cint_rpf_route_info.intf_in_loose, 0/*encap_id_rpf_loose*/, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, fec_rpf_loose\n");
        return rv;
    }

    rv = l3__egress_only_fec__create(unit, fec_rpf_strict, cint_rpf_route_info.intf_in_strict, 0/*encap_id_rpf_strict*/, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, intf_in_strict \n");
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, fec_rpf_strict_kaps, cint_rpf_route_info.intf_in_strict, 0/*encap_id_rpf_strict*/, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, intf_in_strict\n");
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, fec_rpf_strict_kaps_default_route, cint_rpf_route_info.intf_in_strict, 0/*encap_id_rpf_strict*/, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, intf_in_strict\n");
        return rv;
    }

    /** Create OUT-LIF and set its properties: FEC, encap-id */
    rv = intf_eth_rif_create(unit, cint_rpf_route_info.intf_out, cint_rpf_route_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, fec, cint_rpf_route_info.intf_out, cint_rpf_route_info.encap_id, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, intf_out\n");
        return rv;
    }
    rv = l3__egress_only_encap__create(unit, 0, &cint_rpf_route_info.encap_id, cint_rpf_route_info.arp_next_hop_mac,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only, encap_id\n");
        return rv;
    }

    /**
     *  Add Route entries
     */
    if(vpbr_enabled)
    {
        /*
         * if this flag is set then we want to create host with different VRF,
         * and check if the PBR works and the VRF is updated and the packet is redirected to
         * the new one.
         * here we use vrf_loose + 1 and vrf_strict + 1
         */
        updated_vrf_loose = vrf_loose + 1;
        updated_vrf_strict = vrf_strict + 1;
    }
    /*
     * Loose mode entries
     */

    rv = add_host_ipv6(unit, host_dip, updated_vrf_loose, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host, dip, vrf_loose\n");
        return rv;
    }

    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit)) {
        rv = add_route_ipv6_rpf(unit, host_sip_1, full_mask, vrf_loose, fec);
    } else {
        rv = add_host_ipv6(unit, host_sip_1, vrf_loose, fec);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host, sip_1, vrf_loose, fec\n");
        return rv;
    }

    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit)) {
        rv = add_route_ipv6_rpf(unit, host_sip_2, full_mask, vrf_loose, fec_rpf_loose);
    } else {
        rv = add_host_ipv6(unit, host_sip_2, vrf_loose, fec_rpf_loose);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host, sip_2, vrf_loose, fec_rpf_loose\n");
        return rv;
    }

    /** Strict mode entries */
    rv = add_host_ipv6(unit, host_dip, updated_vrf_strict, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host, dip, vrf_strict\n");
        return rv;
    }

    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit)) {
        rv = add_route_ipv6_rpf(unit, host_sip_1, full_mask, vrf_strict, fec);
    } else {
        rv = add_host_ipv6(unit, host_sip_1, vrf_strict, fec);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host, sip_1, vrf_strict, fec\n");
        return rv;
    }

    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit)) {
        rv = add_route_ipv6_rpf(unit, host_sip_2, full_mask, vrf_strict, fec_rpf_strict);
    } else {
        rv = add_host_ipv6(unit, host_sip_2, vrf_strict, fec_rpf_strict);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host, sip_2, vrf_strict, fec_rpf_strict\n");
        return rv;
    }

    return rv;
}

/*
 * Configure interface, routes and hosts for MC RPF.
 * The function considers both cases of MC RPF - Explicit and SIP-based.
 * It is expected that the packet will arrive on port in_port.
 * If packet forwarding and RPF check are successful, the packet will be replicated to out_port1 and out_port2.
 * If the packet fails RPF lookup, it should be dropped.
 *
 * It is expected that the input packet will have
 *      * DA = 01:00:5E:02:02:02
 *      * SA = 00:00:07:00:01:00
 *      * VID = rif (input)
 *      * DIP = 224.2.2.2
 *      * SIP = 192.128.1.1 / 192.128.1.2 / 192.128.1.3 / 192.128.1.4 / 192.128.1.5
 * Packets that have one of the first three SIPs are configured to lead to a SIP-based RPF.
 * Packets that have one of the latter two SIPs are configured to lead to an explicit RPF check.
 *
 * Packets with SIP 192.128.1.1 and 192.128.1.4 are configured to pass RPF check.
 */
int
dnx_example_ipv4_mc_rpf(
    int unit,
    int in_port1,
    int in_port2,
    int out_port1,
    int out_port2,
    int rif)
{
    /*
     * Define needed variables
     */
    int rv = BCM_E_NONE;
    int vrf = 15;
    int out_ports[2] = { out_port1, out_port2 };
    int idx;
    int lsb_mac = 5; /** Least significant byte of MAC address*/
    int nof_entries = 5;

    /** configure expected out destination MAC */
    bcm_mac_t next_hop_mac = { 0x01, 0x00, 0x5E, 0x02, 0x02, 0x02 };

    uint32 dip_mask = 0xFFFFFFFF;
    uint32 sip_mask = 0;
    int eth_rif_id = rif;

    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_rpf_route_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    int fecs[5] = { cint_rpf_route_info.fec_id, cint_rpf_route_info.fec_id + 2, cint_rpf_route_info.fec_id + 4,
                    cint_rpf_route_info.fec_id + 6, cint_rpf_route_info.fec_id + 8};
    int intf_out = 0x100;
    bcm_l3_intf_t intf;
    bcm_l3_ingress_t ingress_intf;
    bcm_multicast_replication_t replications[2];
    bcm_gport_t mc_gport_l;
    int ipmc_group = 6000;
    int encap_id = 6520;
    int rifs[5] = { rif, 0, 0, rif, rif + 2 };
    int in_rifs[5] = { rif, rif, rif + 1, rif, rif};
    uint32 flags[5] =
        { BCM_L3_MC_RPF_SIP_BASE, BCM_L3_MC_RPF_SIP_BASE, BCM_L3_MC_RPF_SIP_BASE, BCM_L3_MC_RPF_EXPLICIT, BCM_L3_MC_RPF_EXPLICIT };
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    cint_ipmc_info.out_ports[0] = out_port1;
    cint_ipmc_info.out_ports[1] = out_port2;

    /*
     * 1. Configure traps for failed RPF lookup - SIP-based and Explicit.
     */
    rv = l3_dnx_ip_mc_rpf_config_traps(unit);
    if (rv != BCM_E_NONE && rv != BCM_E_PARAM)
    {
        printf("Error, l3_dnx_ip_mc_rpf_config_traps\n");
        return rv;
    }

    /*
     * 2a. Set In-Port1 to In ETh-RIF
     */
    rv = cint_ip_rpf_port_intf_set(unit, in_port1, eth_rif_id, vrf, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_ip_rpf_port_intf_set\n");
        return rv;
    }
    /*
     * 2b. Set In-Port2 to In ETh-RIF
     */
    rv = cint_ip_rpf_port_intf_set(unit, in_port2, eth_rif_id + 1, vrf, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_ip_rpf_port_intf_set\n");
        return rv;
    }
	/*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, out_port1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set\n");
        return rv;
    }

    rv = out_port_set(unit, out_port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set\n");
        return rv;
    }

    /*
     * 3a. Set ingress interface
     */
    ingr_itf.intf_id = eth_rif_id;
    ingr_itf.vrf = vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * 3b. Set ingress interface
     */
    ingr_itf.intf_id = eth_rif_id + 1;
    ingr_itf.vrf = vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }
    /*
     * 4. Create a multicast group and its replications
     */
    rv = create_multicast_group_and_replications(unit, cint_ipmc_info.mc_group, cint_ipmc_info.out_ports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create_multicast_group_and_replications\n");
        return rv;
    }

    BCM_GPORT_MCAST_SET(mc_gport_l, ipmc_group);

    /*
     * 5. Define expected out DMAC
     */
    rv = l3__egress_only_encap__create(unit, 0, &encap_id, next_hop_mac, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only, encap_id\n");
        return rv;
    }

    /*
     * 6. Create FECs for SIP-based and Explicit RPF
     */
    for (idx = 0; idx < nof_entries; idx++)
    {
        rv = l3__egress_only_fec__create(unit, fecs[idx], rifs[idx], 0, mc_gport_l, flags[idx]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC inner, fecs[0]\n");
            return rv;
        }
    }

    /*
     * 7a. For SIP-based RPF we need to add an UC entry with the IP of the SRC IP
     */
    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit)) {
        rv = add_route_ipv4_rpf(unit, cint_rpf_route_info.host_sip, vrf, fecs[0]);
    } else {
        rv = add_host_ipv4(unit, cint_rpf_route_info.host_sip, vrf, fecs[0], 0, 0);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host, dip, vrf_strict\n");
        return rv;
    }

    /*
     * 7b. For SIP-based RPF we need to add an UC entry with the IP of the SRC IP
     */
    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit)) {
        rv = add_route_ipv4_rpf(unit, cint_rpf_route_info.host_sip + 2, vrf, fecs[2]);
    } else {
        rv = add_host_ipv4(unit, cint_rpf_route_info.host_sip + 2, vrf, fecs[2], 0, 0);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host, dip, vrf_strict\n");
        return rv;
    }
    /*
     * 8. Create four IPMC entries - two leading to SIP-based RPF, two leading to Explicit RPF.
     */
    for (idx = 0; idx < nof_entries; idx++)
    {
        rv = add_ipv4_ipmc(unit, cint_rpf_route_info.host_mc, dip_mask, 0, sip_mask, in_rifs[idx], vrf, 0, fecs[idx], 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function add_ipv4_ipmc - iter = %d \n", idx);
            return rv;
        }
        cint_rpf_route_info.host_mc++;
    }
    return rv;
}

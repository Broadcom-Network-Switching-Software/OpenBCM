/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
/*
 * Example of a router with RPF scenario
 * Test Scenario
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_rpf_basic.c
  cint
  dnx_example_ipv4_rpf(unit, inP_loose, inP_strict, outP, vsi_loose, vsi_strict, vrf_loose, vrf_strict, vpbr_enabled)
 *                          with valid parameters;
  exit;
 * Test Scenario - end
 *
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
    bcm_ip_t dip;                                       /** Destination IP */
    bcm_ip_t sip;                                       /** Source IP */
    bcm_ip_t mc_address;                                /** Dest Mc IP */
    int vrf_loose;                                      /** VRF loose */
    int vrf_strict;                                     /** VRF strict */
    bcm_if_t fec_id;                                    /** FEC id */
    int loose_rpf_with_no_payload;                      /** Use route entry with no payload for loose RPF */
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
     * dip
     */
    0x7fffff02 /** 127.255.255.2 */,
    /*
     * sip
     */
    0xc0800101 /** 192.128.1.1 */,
    /*
     * mc_address
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
    /*
     * loose_rpf_with_no_payload
     */
    0
};

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

        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(unit, 0, bcmRxTrapUcStrictRpfFail, &rpf_uc_strict_trap), "rpf_uc_strict_trap");
        rv = bcm_rx_trap_set(unit, rpf_uc_strict_trap, &config);
        if (rv != BCM_E_NONE)
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUcStrictRpfFail, &rpf_uc_strict_trap),
                "trap bcmRxTrapUcStrictRpfFail");
            BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, rpf_uc_strict_trap, &config), "rpf_uc_strict_trap");
        }
    }

    /*
     * set uc loose to drop packet
     */
    for (i = 0; i < nof_units; i++)
    {
        unit = units_ids[i];
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(unit, 0, bcmRxTrapUcLooseRpfFail, &rpf_uc_loose_trap), "rpf_uc_loose_trap");
        rv = bcm_rx_trap_set(unit, rpf_uc_loose_trap, &config);
        if (rv != BCM_E_NONE)
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUcLooseRpfFail, &rpf_uc_loose_trap),
                "trap bcmRxTrapUcLooseRpfFail");
            BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, rpf_uc_loose_trap, &config), "rpf_uc_loose_trap");
        }
    }
    return BCM_E_NONE;
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
    config.trap_strength = 15;
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    /*
     * set mc sip-based and explicit RPF to drop packet
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(unit, 0, bcmRxTrapMcUseSipRpfFail, &rpf_mc_sip_trap), "rpf_mc_sip_trap");
    rv = bcm_rx_trap_set(unit, rpf_mc_sip_trap, &config);
    if (rv != BCM_E_NONE)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapMcUseSipRpfFail, &rpf_mc_sip_trap),
            "trap bcmRxTrapMcUseSipRpfFail");
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, rpf_mc_sip_trap, &config), "rpf_mc_sip_trap");
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(unit, 0, bcmRxTrapMcExplicitRpfFail, &rpf_mc_explicit_trap),
        "rpf_mc_explicit_trap");
    rv = bcm_rx_trap_set(unit, rpf_mc_explicit_trap, &config);
    if (rv != BCM_E_NONE)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapMcExplicitRpfFail, &rpf_mc_explicit_trap),
            "trap bcmRxTrapMcExplicitRpfFail");
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, rpf_mc_explicit_trap, &config), "rpf_mc_explicit_trap");
    }

    return BCM_E_NONE;
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

    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;   /* must, as we update exist RIF */
    l3_ing_if.vrf = vrf;
    l3_ing_if.urpf_mode = urpf_mode;

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_ingress_create(unit, l3_ing_if, eth_rif_id), "");

    return BCM_E_NONE;
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

    /** Set In-Port to Eth-RIF */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, intf_in), "intf_in");

    /** Create IN-RIF and set its properties */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create_vrf(unit, intf_in, vrf, cint_rpf_route_info.intf_in_mac_address), "intf_in");
    /** Indicate RPF lookup for interface if urpf_mode is defined */
    if (urpf_mode != 0)
    {
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_rpf_set(unit, intf_in, vrf, urpf_mode), "intf_in vrf");
    }
    else
    {
        l3_ingress_intf ingress_rif;
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = vrf;
        ingress_rif.intf_id = intf_in;
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    return BCM_E_NONE;
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
    int fec_fwd = 0;
    int fec_rpf_loose = 0;
    int fec_rpf_strict = 0;
    int updated_vrf_loose = vrf_loose;
    int updated_vrf_strict = vrf_strict;
    int fec_rpf_strict_kaps;
    int fec_rpf_strict_kaps_default_route;
    bcm_gport_t gport = 0;

    /** Get FEC IDs ***/
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec_fwd), "");

    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 1, &fec_rpf_loose), "");

        fec_rpf_strict = fec_rpf_loose + 2;
    }
    else
    {
        fec_rpf_loose = fec_fwd + 2;
        fec_rpf_strict = fec_fwd + 4;
        fec_rpf_strict_kaps = fec_fwd + 6;
        fec_rpf_strict_kaps_default_route = fec_fwd + 8;
    }

    fec_rpf_loose = cint_rpf_route_info.loose_rpf_with_no_payload ? 0 : fec_rpf_loose;
    printf ("The FWD FEC is %d, the FEC loose is %d, the FEC strict is %d\n", fec_fwd, fec_rpf_loose, fec_rpf_strict);
    cint_rpf_route_info.encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;
    cint_rpf_route_info.intf_in_loose = vsi_loose;
    cint_rpf_route_info.intf_in_strict = vsi_strict;
    cint_rpf_route_info.in_port_loose = in_port_loose;
    cint_rpf_route_info.in_port_strict = in_port_strict;
    cint_rpf_route_info.out_port = out_port;

    /** Config traps  */
    rv = l3_dnx_ip_rpf_config_traps(&unit, 1);
    if (rv == BCM_E_PARAM) {
        rv = BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN_MSG(rv, "");
    /*
     * Set In-Port to In ETh-RIF, Create IN-RIFs and set their properties
     * 1 - loose
     * 2 - strict
     */
    BCM_IF_ERROR_RETURN_MSG(cint_ip_rpf_port_intf_set(unit, cint_rpf_route_info.in_port_loose, cint_rpf_route_info.intf_in_loose,
                                      vrf_loose, bcmL3IngressUrpfLoose), "Loose");
    BCM_IF_ERROR_RETURN_MSG(cint_ip_rpf_port_intf_set(unit, cint_rpf_route_info.in_port_strict, cint_rpf_route_info.intf_in_strict,
                                      vrf_strict, bcmL3IngressUrpfStrict), "Strict");

    /*
     * Set Out-Port default properties
     */

    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_rpf_route_info.out_port), "");

    gport = cint_rpf_route_info.out_port;
    if (BCM_GPORT_IS_SET(out_port)) {
       gport = cint_rpf_route_info.out_port;
    }
    else {
       BCM_GPORT_LOCAL_SET(gport, cint_rpf_route_info.out_port);
    }

    int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
    int rpf_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_RPF_ONLY : 0;

    if (!cint_rpf_route_info.loose_rpf_with_no_payload)
    {
        /** Create FEC*/
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec_rpf_loose, cint_rpf_route_info.intf_in_loose, 0/*encap_id_rpf_strict*/, gport, 0, rpf_flag, 0, 0, 0),
            "create egress object FEC only, fec_rpf_loose");
    }

    /** Create FEC*/
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec_rpf_strict, cint_rpf_route_info.intf_in_strict, 0/*encap_id_rpf_strict*/, gport, 0, rpf_flag, 0, 0, 0),
        "reate egress object FEC only, intf_in_strict");

    /** Create OUT-LIF and set its properties: FEC, encap-id */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create_vrf(unit, cint_rpf_route_info.intf_out, 1,cint_rpf_route_info.intf_out_mac_address), "intf_out");
    /** Create an ARP*/
    BCM_IF_ERROR_RETURN_MSG(create_arp_plus_ac_type(unit, 0,cint_rpf_route_info.arp_next_hop_mac, &cint_rpf_route_info.encap_id, 0),
        "create egress object ARP only, encap_id");

    /** Create FEC*/
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec_fwd, cint_rpf_route_info.intf_out,  cint_rpf_route_info.encap_id, gport, 0, fwd_flag, 0, 0, 0),
        "create egress object FEC only, intf_out");

    if (!(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)))
    {
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fec_rpf_strict_kaps, cint_rpf_route_info.intf_in_strict, 0/*encap_id_rpf_strict*/, gport, 0, 0),
            "create egress object FEC only, intf_in_strict");
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fec_rpf_strict_kaps_default_route, cint_rpf_route_info.intf_in_strict, 0/*encap_id_rpf_strict*/, gport, 0, 0),
            "create egress object FEC only, intf_in_strict");
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
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_rpf_route_info.dip, 0xffffffff, updated_vrf_loose, fec_fwd), "dip, vrf_loose");

    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit) || *dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv4_rpf(unit, cint_rpf_route_info.sip, 0xffffffff, vrf_loose, (cint_rpf_route_info.loose_rpf_with_no_payload ? 0 : fec_fwd)), "sip_1, vrf_loose, fec");
    } else {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_rpf_route_info.sip, vrf_loose, (cint_rpf_route_info.loose_rpf_with_no_payload ? 0 : fec_fwd), 0,0), "sip_1, vrf_loose, fec");
    }

    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit) || *dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv4_rpf(unit, cint_rpf_route_info.sip + 1, 0xffffffff, vrf_loose, fec_rpf_loose), "sip_2, vrf_loose, fec_rpf_loose");
    } else {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_rpf_route_info.sip + 1, vrf_loose, fec_rpf_loose, 0, 0), "sip_2, vrf_loose, fec_rpf_loose");
    }

    /*
     * Strict mode entries
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_rpf_route_info.dip, 0xffffffff, updated_vrf_strict, fec_fwd), "dip, vrf_strict");

    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit) || *dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv4_rpf(unit, cint_rpf_route_info.sip, 0xffffffff, vrf_strict, fec_fwd), "");
    } else {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_rpf_route_info.sip, vrf_strict, fec_fwd, 0,0), "");
    }
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4_rpf(unit, cint_rpf_route_info.sip, 0xffffffff, vrf_strict, fec_fwd), "sip_1, vrf_strict, fec");

    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit) || *dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv4_rpf(unit, cint_rpf_route_info.sip + 1, 0xffffffff, vrf_strict, fec_rpf_strict), "sip_2, vrf_strict, fec_rpf_strict");
    } else {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_rpf_route_info.sip + 1, vrf_strict, fec_rpf_strict, 0,0), "sip_2, vrf_strict, fec_rpf_strict");
    }

    return BCM_E_NONE;
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
    int fec_fwd;
    int fec_rpf_loose;
    int fec_rpf_strict;
    int fec_rpf_strict_kaps;
    int fec_rpf_strict_kaps_default_route;

    /** Get FEC IDs ***/
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec_fwd), "");

    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 1, &fec_rpf_loose), "");

        fec_rpf_strict = fec_rpf_loose + 2;
    }
    else
    {
        fec_rpf_loose = fec_fwd + 2;
        fec_rpf_strict = fec_fwd + 4;
        fec_rpf_strict_kaps = fec_fwd + 6;
        fec_rpf_strict_kaps_default_route = fec_fwd + 8;
    }

    fec_rpf_loose = cint_rpf_route_info.loose_rpf_with_no_payload ? 0 : fec_rpf_loose;

    int updated_vrf_loose = vrf_loose;
    int updated_vrf_strict = vrf_strict;
    bcm_gport_t gport = 0;
    bcm_ip6_t route_dip = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x13 }; /*Dest IP address*/
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
    if (rv == BCM_E_PARAM) {
        rv = BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN_MSG(rv, "");

    /*
     * Set In-Port to In ETh-RIF, Create IN-RIFs and set their properties
     * 1 - loose
     * 2 - strict
     */
    BCM_IF_ERROR_RETURN_MSG(cint_ip_rpf_port_intf_set(unit, cint_rpf_route_info.in_port_loose, cint_rpf_route_info.intf_in_loose,
                                      vrf_loose, bcmL3IngressUrpfLoose), "Loose");
    BCM_IF_ERROR_RETURN_MSG(cint_ip_rpf_port_intf_set(unit, cint_rpf_route_info.in_port_strict, cint_rpf_route_info.intf_in_strict,
                                      vrf_strict, bcmL3IngressUrpfStrict), "Strict");

    /*
     * Set Out-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_rpf_route_info.out_port), "");

    BCM_GPORT_LOCAL_SET(gport, cint_rpf_route_info.out_port);

    int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
    int rpf_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_RPF_ONLY : 0;

    /** Create FEC*/
    if (!cint_rpf_route_info.loose_rpf_with_no_payload)
    {
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec_rpf_loose, cint_rpf_route_info.intf_in_loose, 0/*encap_id_rpf_loose*/, gport, 0, rpf_flag, 0, 0, 0),
            "create egress object FEC only, fec_rpf_loose");
    }

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec_rpf_strict, cint_rpf_route_info.intf_in_strict, 0/*encap_id_rpf_loose*/, gport, 0, rpf_flag, 0, 0, 0),
        "create egress object FEC only, intf_in_strict");

    /** Create OUT-LIF and set its properties: FEC, encap-id */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create_vrf(unit, cint_rpf_route_info.intf_out, 1, cint_rpf_route_info.intf_out_mac_address), "intf_out");
    /** Create an ARP*/
    BCM_IF_ERROR_RETURN_MSG(create_arp_plus_ac_type(unit, 0,cint_rpf_route_info.arp_next_hop_mac, &cint_rpf_route_info.encap_id, 0),
        "create egress object ARP only, encap_id");
    /** Create FEC*/
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec_fwd, cint_rpf_route_info.intf_out,  cint_rpf_route_info.encap_id, gport, 0, fwd_flag, 0, 0, 0),
        "create egress object FEC only, intf_out");

    if (!(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)))
    {
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fec_rpf_strict_kaps, cint_rpf_route_info.intf_in_strict, 0/*encap_id_rpf_strict*/, gport, 0, 0),
            "create egress object FEC only, intf_in_strict");
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fec_rpf_strict_kaps_default_route, cint_rpf_route_info.intf_in_strict, 0/*encap_id_rpf_strict*/, gport, 0, 0),
            "create egress object FEC only, intf_in_strict");
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

    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, route_dip, full_mask, updated_vrf_loose, fec_fwd), "dip, vrf_loose");

    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit) || *dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6_rpf(unit, host_sip_1, full_mask, vrf_loose, (cint_rpf_route_info.loose_rpf_with_no_payload ? 0 : fec_fwd)), "sip_1, vrf_loose, fec");
    } else {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, host_sip_1, vrf_loose, (cint_rpf_route_info.loose_rpf_with_no_payload ? 0 : fec_fwd)), "sip_1, vrf_loose, fec");
    }


    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit) || *dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6_rpf(unit, host_sip_2, full_mask, vrf_loose, fec_rpf_loose), "sip_2, vrf_loose, fec_rpf_loose");
    } else {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, host_sip_2, vrf_loose, fec_rpf_loose), "sip_2, vrf_loose, fec_rpf_loose");
    }

    /** Strict mode entries */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, route_dip, full_mask, updated_vrf_strict, fec_fwd), "dip, vrf_strict");

    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit) || *dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6_rpf(unit, host_sip_1, full_mask, vrf_strict, fec_fwd), "sip_1, vrf_strict, fec");
    } else {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, host_sip_1, vrf_strict, fec_fwd), "sip_1, vrf_strict, fec");
    }

    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit) || *dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6_rpf(unit, host_sip_2, full_mask, vrf_strict, fec_rpf_strict), "sip_2, vrf_strict, fec_rpf_strict");
    } else {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, host_sip_2, vrf_strict, fec_rpf_strict), "sip_2, vrf_strict, fec_rpf_strict");
    }

    return BCM_E_NONE;
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
    int idx, start_ind;
    int lsb_mac = 5; /** Least significant byte of MAC address*/
    int sip_based_nof_entries = 3;
    int explicit_nof_entries = 2;
    char error_msg[200]={0,};

    /** configure expected out destination MAC */
    bcm_mac_t next_hop_mac = { 0x01, 0x00, 0x5E, 0x02, 0x02, 0x02 };

    uint32 dip_mask = 0xFFFFFFFF;
    uint32 sip_mask = 0;
    int eth_rif_id = rif;
    int intf_out = 0x100;
    int encap_id = 6520;
    int rifs_sip_based[3] = { rif, 0, 0 };
    int rifs_explicit[2] = { rif, rif + 2 };
    int in_rifs_sip_based[3] = { rif, rif, rif + 1 };
    int in_rifs_explicit[3] = { rif, rif };

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_rpf_route_info.fec_id), "");
    int fecs_sip_based[3] = { cint_rpf_route_info.fec_id, cint_rpf_route_info.fec_id + 2, cint_rpf_route_info.fec_id + 4 };
    int fecs_explicit[2] = { cint_rpf_route_info.fec_id + 6, cint_rpf_route_info.fec_id + 8 };

    bcm_l3_intf_t intf;
    bcm_l3_ingress_t ingress_intf;
    bcm_multicast_replication_t replications[2];
    bcm_gport_t mc_gport_l;
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    cint_ipmc_info.out_ports[0] = out_port1;
    cint_ipmc_info.out_ports[1] = out_port2;

    /*
     * 1. Configure traps for failed RPF lookup - SIP-based and Explicit.
     */
    rv = l3_dnx_ip_mc_rpf_config_traps(unit);
    if (rv == BCM_E_PARAM) {
        rv = BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN_MSG(rv, "");

    /*
     * 2a. Set In-Port1 to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(cint_ip_rpf_port_intf_set(unit, in_port1, eth_rif_id, vrf, 0), "");
    /*
     * 2b. Set In-Port2 to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(cint_ip_rpf_port_intf_set(unit, in_port2, eth_rif_id + 1, vrf, 0), "");
    /*
     * Set Out-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port1), "");

    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port2), "");

    /*
     * 3a. Set ingress interface
     */
    ingr_itf.intf_id = eth_rif_id;
    ingr_itf.vrf = vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "intf_out");

    /*
     * 3b. Set ingress interface
     */
    ingr_itf.intf_id = eth_rif_id + 1;
    ingr_itf.vrf = vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "intf_out");
    /*
     * 4. Create a multicast group and its replications
     */
    BCM_IF_ERROR_RETURN_MSG(create_multicast_group_and_replications(unit, cint_ipmc_info.mc_group, cint_ipmc_info.out_ports), "");

    BCM_GPORT_MCAST_SET(mc_gport_l, cint_ipmc_info.mc_group);

    /*
     * 5. Define expected out DMAC
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &encap_id, next_hop_mac, 0), "create egress object ARP only, encap_id");

    /*
     * 6. Create FECs for SIP-based and Explicit RPF
     */
    int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
    if (*dnxc_data_get(unit, "l3", "fec", "mc_rpf_sip_based_supported", NULL) == 1)
    {
        for (idx = 0; idx < sip_based_nof_entries; idx++)
        {
            BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fecs_sip_based[idx], rifs_sip_based[idx], 0, mc_gport_l, BCM_L3_MC_RPF_SIP_BASE, fwd_flag),
                "create egress object FEC inner, fecs_sip_based[0]");
        }
    }
    for (idx = 0; idx < explicit_nof_entries; idx++)
    {
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fecs_explicit[idx], rifs_explicit[idx], 0, mc_gport_l, BCM_L3_MC_RPF_EXPLICIT, fwd_flag),
            "create egress object FEC inner, fecs_explicit[0]");
    }
    /*
     * 7a. For SIP-based RPF we need to add an UC entry with the IP of the SRC IP
     */
    if (*dnxc_data_get(unit, "l3", "fec", "mc_rpf_sip_based_supported", NULL) == 1)
    {
        if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit)) {
            BCM_IF_ERROR_RETURN_MSG(add_route_ipv4_rpf(unit, cint_rpf_route_info.sip, vrf, fecs_sip_based[0]), "dip, vrf_strict");
        } else {
            BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_rpf_route_info.sip, vrf, fecs_sip_based[0], 0, 0), "dip, vrf_strict");
        }

        /*
         * 7b. For SIP-based RPF we need to add an UC entry with the IP of the SRC IP
         */
        if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit)) {
            BCM_IF_ERROR_RETURN_MSG(add_route_ipv4_rpf(unit, cint_rpf_route_info.sip + 2, vrf, fecs_sip_based[2]), "dip, vrf_strict");
        } else {
            BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_rpf_route_info.sip + 2, vrf, fecs_sip_based[2], 0, 0), "dip, vrf_strict");
        }
    }
    /*
     * 8. Create five IPMC entries - three leading to SIP-based RPF, two leading to Explicit RPF.
     */
    for (idx = 0; idx < sip_based_nof_entries; idx++)
    {
        if (*dnxc_data_get(unit, "l3", "fec", "mc_rpf_sip_based_supported", NULL) == 1)
        {
            snprintf(error_msg, sizeof(error_msg), "iter = %d", idx);
            BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_rpf_route_info.mc_address, dip_mask, 0, sip_mask, in_rifs_sip_based[idx], vrf, 0, fecs_sip_based[idx], 0), error_msg);
        }
        cint_rpf_route_info.mc_address++;
    }
    for (idx = 0; idx < explicit_nof_entries; idx++)
    {
        snprintf(error_msg, sizeof(error_msg), "iter = %d", idx);
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_rpf_route_info.mc_address, dip_mask, 0, sip_mask, in_rifs_explicit[idx], vrf, 0, fecs_explicit[idx], 0), error_msg);
        cint_rpf_route_info.mc_address++;
    }
    return BCM_E_NONE;
}

/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_stat_pp_mpls_termination.c Purpose: Statistics collection on 4 mpls tunnels.
 */

/*
 * File: cint_stat_pp_mpls_termination.c
 * Purpose: Example of ingress lifs statistics
 *
 * Configure tunnels and statistics on 4 mpls incoming labels
 *
 * The cint includes:
 * - Main function to configure mpls tunnels with statistics
 * - Structs with configurations used to configure the statistics profiles (crps and pp)
 * - A struct with mpls termination required configuration
 *
 * Usage:
 * ------
 *
 * Test Scenario:
 *--------------
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_ip_route_hierarchical_fec.c
 * cint ../../src/examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/examples/dnx/crps/cint_crps_irpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_erpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_etpp_config.c
 * cint ../../src/examples/dnx/crps/cint_crps_etpp_config.c
 * cint ../../src/examples/src/examples/dnx/stat_pp/cint_stat_pp_mpls_termination.c
 * cint
 * cint_stat_pp_ingress_example(0,200,201);
 * exit;
 *
 * Sent packet:
 * tx 1 psrc=201 data=0000000000000000000000000000000080c8000c000200000011000001128100000884701770040017710400177204001773140450000350000000080005874a000011a0a1a1a2000102030405060708090a0b0c0d0e0f1011121314151617181911b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 00000000CD1D000C0002000181000032080045000035000000007F005974A0000011A0A1A1A2000102030405060708090A0B
 * CRPS verify: 1 packet received on 4 mpls tunnels
 */

struct cint_stat_pp_mpls_4_labels_term_info_s
{
    int in_port;                /* incoming port */
    int out_port;               /* outgoing port */
    int intf_in;                /* in RIF */
    int intf_out;               /* out RIF */
    bcm_mac_t intf_in_mac_address;      /* mac for tin RIF */
    bcm_mac_t intf_out_mac_address;     /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac; /* mac for next hop */
    int vrf;                    /* VRF */
    int arp_id;                 /* Id for ARP entry */
    bcm_ip_t dip;               /* dip 1 */
    uint32 mask;                /* mask for dip */
    int fec_id;                 /* fec id for termination scenario */
    bcm_mpls_label_t     termination_stack[4];      /* Label to be terminated */
    bcm_gport_t tunnel_ids[4];   /* Tunnel_id created of the label */
    bcm_if_t tunnel_if;         /* RIF, not relevant only for Jericho2 devices */
    int metadata;                /* stat metadata for header type MPLS */
};

cint_stat_pp_mpls_4_labels_term_info_s cint_stat_pp_mpls_4_labels_term_info =
    /*
     * ports : in_port | out_port
     */
{ 200, 201,
    /*
     * intf_in | intf_out
     */
    40, 50,
    /*
     * intf_in_mac_address | intf_out_mac_address | arp_next_hop_mac |
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * vrf
     */
    100,
    /*
     * arp_id
     */
    9000,
    /*
     * dip
     */
    0xA0A1A1A2 /* 160.161.161.162 */ ,
    /*
     * mask
     */
    0xfffffff0,
    /*
     * fec_id
     */
    0x2000c35B,
    /*
     * termination_stack
     */
    {6000, 6001, 6002, 6003},
    /*
     * tunnel_term_id stack
     */
    {-1, -1, -1, -1},
    /*
     * tunnel_if
     */
    20,
    /*
     * metadata
     */
    0
};

/*
 * Statistics configurations (see src/examples/dnx/stat_pp/cint_stat_pp_config.c)
 */
cint_stat_pp_config_info_s tunnel_1_statistics_config =
{
        /**database_id */
        1,
        /**crps_engine_id */
        1,
        /**pp_profile */
        1,
        /**counter_command */
        1,
        /**crps_start */
        0,
        /**crps_end */
        100,
        /**type_id*/
        0,
        /**stat_id*/
        1,
        /**expansion_flag*/
        24,
        /**tunnel_metadata_size*/
        0,
        /**tunnel_metadata_shift*/
        0,
};

cint_stat_pp_config_info_s tunnel_2_statistics_config =
{
        /**database_id */
        2,
        /**crps_engine_id */
        2,
        /**pp_profile */
        2,
        /**counter_command */
        2,
        /**crps_start */
        0,
        /**crps_end */
        100,
        /**type_id*/
        0,
        /**stat_id*/
        2,
        /**expansion_flag*/
        5,
        /**tunnel_metadata_size*/
        0,
        /**tunnel_metadata_shift*/
        0,
};

cint_stat_pp_config_info_s tunnel_3_statistics_config =
{
        /**database_id */
        3,
        /**crps_engine_id */
        3,
        /**pp_profile */
        3,
        /**counter_command */
        3,
        /**crps_start */
        0,
        /**crps_end */
        100,
        /**type_id*/
        0,
        /**stat_id*/
        3,
        /**expansion_flag*/
        5,
        /**tunnel_metadata_size*/
        0,
        /**tunnel_metadata_shift*/
        0,
};

cint_stat_pp_config_info_s tunnel_4_statistics_config =
{
        /**database_id */
        4,
        /**crps_engine_id */
        4,
        /**pp_profile */
        4,
        /**counter_command */
        4,
        /**crps_start */
        0,
        /**crps_end */
        100,
        /**type_id*/
        0,
        /**stat_id*/
        4,
        /**expansion_flag*/
        5,
        /**tunnel_metadata_size*/
        0,
        /**tunnel_metadata_shift*/
        0,
};

/*
 * Configure mpls tunnels
 */
int
cint_stat_pp_mpls_4_labels_term_build_termination_stack(
    int unit,
    bcm_mpls_label_t terminated_label,
    bcm_gport_t * tunnel_id)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    bcm_if_t l3_intf_id;
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);
    bcm_mpls_tunnel_switch_t_init(&entry);

    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    entry.label = terminated_label;
    entry.flags2 |= BCM_MPLS_SWITCH2_STAT_ENABLE;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    *tunnel_id = entry.tunnel_id;

    /*
     * Set Incoming Tunnel-RIF properties
     */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, entry.tunnel_id);
    ingr_itf.intf_id = l3_intf_id;

    ingr_itf.vrf = cint_stat_pp_mpls_4_labels_term_info.vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set\n");
    }

    return rv;
}


/*
 * main function.
 * Calls:
 * cint_stat_pp_config_crps()
 * cint_stat_pp_create_pp_profile()
 * Configures mpls tunnels
 */
int
cint_stat_pp_mpls_termination_main(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    bcm_stat_pp_info_t stat_info;
    bcm_stat_pp_metadata_info_t metadata_info;
    int num_labels = 4;
    int i;

    cint_stat_pp_mpls_4_labels_term_info.in_port = port1;
    cint_stat_pp_mpls_4_labels_term_info.out_port = port2;


    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_stat_pp_mpls_4_labels_term_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    /** Counfigure statistics */
    rv = cint_stat_pp_config_main(unit, tunnel_1_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for tunnel_1_statistics_config\n");
        return rv;
    }

    rv = cint_stat_pp_config_main(unit, tunnel_2_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for tunnel_2_statistics_config\n");
        return rv;
    }

    rv = cint_stat_pp_config_main(unit, tunnel_3_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for tunnel_3_statistics_config\n");
        return rv;
    }

    rv = cint_stat_pp_config_main(unit, tunnel_4_statistics_config, bcmStatCounterInterfaceIngressReceivePp, 0, BCM_CORE_ALL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("cint_stat_pp_config_main failed for tunnel_4_statistics_config\n");
        return rv;
    }

    /*
     * Set metadata when header type is MPLS.
     *
     * IRPP metadata has 16 bits, each statistics profile can choose to update part of the metadata.
     *
     * In this example, IRPP metadata is constructed as follows:
     *     cint_stat_pp_mpls_4_labels_term_info.metadata[tunnel_1_statistics_config.tunnel_metadata_size-1:0] << tunnel_1_statistics_config.tunnel_metadata_shift |
     *     cint_stat_pp_mpls_4_labels_term_info.metadata[tunnel_2_statistics_config.tunnel_metadata_size-1:0] << tunnel_2_statistics_config.tunnel_metadata_shift |
     *     cint_stat_pp_mpls_4_labels_term_info.metadata[tunnel_3_statistics_config.tunnel_metadata_size-1:0] << tunnel_3_statistics_config.tunnel_metadata_shift |
     *     cint_stat_pp_mpls_4_labels_term_info.metadata[tunnel_4_statistics_config.tunnel_metadata_size-1:0] << tunnel_4_statistics_config.tunnel_metadata_shift
     *
     * Tunnel 1 selects IRPP metadata for statistics expansion(expansion_flag for tunnel 1 is 24), counter set size is 24.
     * It selects 10 LSB bits out of 16 IRPP metadata, then maps it to counter set offset. The mapping is done as follows:
     *     counter_set_offset = metadata[9:0] % 24
     *
     * See crps_irpp_expansion_builder in cint_crps_irpp_config.c
     */
    metadata_info.flags = BCM_STAT_PP_METADATA_INGRESS | BCM_STAT_PP_METADATA_TUNNEL;
    metadata_info.header_type = bcmStatPpMetadataHeaderTypeMpls;
    metadata_info.metadata = cint_stat_pp_mpls_4_labels_term_info.metadata;
    rv = bcm_stat_pp_metadata_set(unit, &metadata_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_stat_pp_metadata_set\n");
        return rv;
    }

    /*
     * Set In-Port to In ETH-RIF
     */
    rv = in_port_intf_set(unit, cint_stat_pp_mpls_4_labels_term_info.in_port, cint_stat_pp_mpls_4_labels_term_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_stat_pp_mpls_4_labels_term_info.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set intf_out\n");
        return rv;
    }

    /*
     * Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_stat_pp_mpls_4_labels_term_info.intf_in,
                             cint_stat_pp_mpls_4_labels_term_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    rv = intf_eth_rif_create(unit, cint_stat_pp_mpls_4_labels_term_info.intf_out,
                             cint_stat_pp_mpls_4_labels_term_info.intf_out_mac_address);

    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * Configure ARP entry
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_stat_pp_mpls_4_labels_term_info.arp_id),
                                       cint_stat_pp_mpls_4_labels_term_info.arp_next_hop_mac,
                                       cint_stat_pp_mpls_4_labels_term_info.intf_out);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    /*
     * Configure a fec entry
     */
    rv = l3__egress_only_fec__create(unit, cint_stat_pp_mpls_4_labels_term_info.fec_id,
                                     cint_stat_pp_mpls_4_labels_term_info.intf_out, cint_stat_pp_mpls_4_labels_term_info.arp_id,
                                     cint_stat_pp_mpls_4_labels_term_info.out_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    /*
     * Mpls tunnels
     */
    for (i = 0; i < num_labels; i++)
    {
        rv = cint_stat_pp_mpls_4_labels_term_build_termination_stack(unit, cint_stat_pp_mpls_4_labels_term_info.termination_stack[i], &cint_stat_pp_mpls_4_labels_term_info.tunnel_ids[i]);
    }

    /*
     * Set statistics for each tunnel in the lif table
     */
    stat_info.stat_pp_profile = tunnel_1_statistics_config.pp_profile;
    stat_info.stat_id = tunnel_1_statistics_config.stat_id;
    rv = bcm_gport_stat_set(unit, cint_stat_pp_mpls_4_labels_term_info.tunnel_ids[0], BCM_CORE_ALL, bcmStatCounterInterfaceIngressReceivePp, stat_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_gport_stat_set, tunnel_1\n");
        return rv;
    }

    stat_info.stat_pp_profile = tunnel_2_statistics_config.pp_profile;
    stat_info.stat_id = tunnel_2_statistics_config.stat_id;
    rv = bcm_gport_stat_set(unit, cint_stat_pp_mpls_4_labels_term_info.tunnel_ids[1], BCM_CORE_ALL, bcmStatCounterInterfaceIngressReceivePp, stat_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_gport_stat_set, tunnel_2\n");
        return rv;
    }

    stat_info.stat_pp_profile = tunnel_3_statistics_config.pp_profile;
    stat_info.stat_id = tunnel_3_statistics_config.stat_id;
    rv = bcm_gport_stat_set(unit, cint_stat_pp_mpls_4_labels_term_info.tunnel_ids[2], BCM_CORE_ALL, bcmStatCounterInterfaceIngressReceivePp, stat_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_gport_stat_set, tunnel_3\n");
        return rv;
    }

    stat_info.stat_pp_profile = tunnel_4_statistics_config.pp_profile;
    stat_info.stat_id = tunnel_4_statistics_config.stat_id;
    rv = bcm_gport_stat_set(unit, cint_stat_pp_mpls_4_labels_term_info.tunnel_ids[3], BCM_CORE_ALL, bcmStatCounterInterfaceIngressReceivePp, stat_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_gport_stat_set, tunnel_4\n");
        return rv;
    }

    /*
     * Create a l3 forwarding entry
     */
    rv = add_host_ipv4(unit, cint_stat_pp_mpls_4_labels_term_info.dip, cint_stat_pp_mpls_4_labels_term_info.vrf, cint_stat_pp_mpls_4_labels_term_info.fec_id, 0     /* arp
                                                                                                                                                         * id                                                                                                                                                         */ , 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function internal_ip_route, \n");
    }

    return rv;
}

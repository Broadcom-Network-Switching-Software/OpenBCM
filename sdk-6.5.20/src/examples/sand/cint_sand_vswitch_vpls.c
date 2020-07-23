/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_sand_vswitch_vpls.c
 * Purpose: Example of MC in VPLS VSwitch
 */

/*
 * File: cint_sand_vswitch_vpls.c
 * Purpose: Example of VPN flooding domain setup with AC ports and PWE ports.
 *
 * This example creates a VPN, a list of logical AC ports, a list of logical PWE ports and adds all
 * the logical ports to the VPN.
 * vswitch_vpls_params_init shows an example of creating 2 AC ports and 2 PWE ports, and connecting
 * them to VPN 100. See figure below:
 * +-----------------------------------------+-------------------------------------------------------------+
 * |                                         :                                                             |
 * |                                  NWK 0  :  NWK 1                                                      |
 * |                                         :                                                             |
 * |                                         :                        Native ETH VID: 100                  |
 * |                                         :                        VC label: 3456                       |
 * |                                         :                        MPLS label: 3333                     |
 * |                                         :                        VID: 30                              |
 * |                                +--------+-------+                RIF MAC:  00:0C:00:02:00:01          |
 * |                                |                |                Next Hop: 00:00:00:00:CD:1D          |
 * |   AC 1    <-------------------->                <-------------------->   PWE 1                        |
 * |                                |                |                                                     |
 * |      VID: 100                  |    VSwitch     |                                                     |
 * |                                |    VPN: 100    |                                                     |
 * |   AC 2    <-------------------->                <-------------------->   PWE 2                        |
 * |                                |                |                 Native ETH VID 100                  |
 * |                                +--------+-------+                 VC label: 3457                      |
 * |                                         :                         MPLS label: 3334                    |
 * |                                         :                         MPLS label: 3335                    |
 * |                                         :                         VID: 31                             |
 * |                                         :                         RIF MAC:  00:0C:00:02:00:02         |
 * |                                         :                         Next Hop: 00:00:00:00:CD:1E         |
 * |                                         :                                                             |
 * |                                         :                                                             |
 * +-----------------------------------------+-------------------------------------------------------------+
 *
 * If vswitch_vpls_params.test_split_horizon is set, both PWEs are set to network group 1, and packets from
 * one will be filtered from egressing to the other.
 *
 * Usage:
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../../../src/examples/sand/cint_sand_vswitch_vpls.c
 * cint
 * vswitch_vpls_params.test_split_horizon=1;
 * print vswitch_vpls_main(0,200,201,202,203);
 *
 * Example ingress packets and egress data for this example:
 *
 * 1: pwe -> mc
 * tx 1 psrc=202 data=0x000c0002000100000000cd1d8100001e884700d0504000d8014000110000011200010203040581000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x0000011200010203040581000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 * Source port: 0, Destination port: 0
 * Data: 0x0000011200010203040581000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 *
 * 2: ac -> pwe (learned)
 * tx 1 psrc=201 data=0x00010203040500110000011281000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x0000cd1d000c000200018100001e884700d0500000d8010000010203040500110000011281000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 *
 * 3: ac -> mc
 * tx 1 psrc=200 data=0x00000000011200010203040681000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x0000cd1e000c000200028100001f884700d060ff00d070ff00d811ff00000000011200010203040681000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 * Source port: 0, Destination port: 0
 * Data: 0x0000cd1d000c000200018100001e884700d050ff00d801ff00000000011200010203040681000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 * Source port: 0, Destination port: 0
 * Data: 0x0000011200010203040681000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 *
 * 1: pwe -> ac (learned)
 * tx 1 psrc=202 data=0x000c0002000100000000cd1d8100001e884700d0504000d8014000010203040600000000011281000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x0203040600000000011281000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 */

/*
 * "Defines"
 */
int VSWITCH_VPLS_MAX_AC_PORTS = 4;
int VSWITCH_VPLS_MAX_PWE_PORTS = 4;

/*
 * Global settings
 */

struct vswitch_vpls_pwe_info_s
{
    l2_port_properties_s pwe_port;
    mpls_tunnel_initiator_create_s mpls_encap_tunnel[MAX_NOF_TUNNELS];
    mpls_tunnel_switch_create_s mpls_term_tunnel[MAX_NOF_TUNNELS];
    mpls_port_add_s pwe_encap;
    mpls_port_add_s pwe_term;
    int pwe_mc_rep_idx;
};

struct vswitch_vpls_params_s
{
    /*
     * Flooding domain 
     */
    int vpn;

    /*
     * List of AC ports 
     */
    int ac_ports_nof;
    l2_port_properties_s ac_ports[VSWITCH_VPLS_MAX_AC_PORTS];

    /*
     * flag to decide if split horizon should be set up for PWE ports.
     */
    int test_split_horizon;
    /*
     * PWE group ID for split horizon
     */
    int pwe_network_group_id;
    /*
     * Default native AC, to assign network group if Split horizon is tested 
     */
    bcm_vlan_port_t default_native_ac;

    /*
     * List of PWE ports 
     */
    int pwe_ports_nof;
    vswitch_vpls_pwe_info_s pwe_params[VSWITCH_VPLS_MAX_PWE_PORTS];
};

/*
 * Global setup configuration struct
 */
vswitch_vpls_params_s vswitch_vpls_params;
/*
 * If vswitch_vpls_params_init function should not be called in vswitch_vpls_main,
 * set this flag to 1.
 */
int vswitch_vpls_params_initialized = 0;

/*
 * Setup for example application. See file header for description
 */
int
vswitch_vpls_params_init(
    int unit,
    vswitch_vpls_params_s * params,
    int ac_port_1,
    int ac_port_2,
    int pwe_port_1,
    int pwe_port_2)
{
    int rv = BCM_E_NONE;
    int fec_id = 10001;

    if (is_device_or_above(unit, JERICHO2))
    {
       rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec_id);
       if (rv != BCM_E_NONE)
       {
           printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
           return rv;
       }
    }


    /*
     * Set group ID for split horizon
     */
    params->pwe_network_group_id = 1;

    /*
     * pointers for later
     */
    vswitch_vpls_pwe_info_s *pwe_param = NULL;
    mpls_tunnel_initiator_create_s *tunnel_init = NULL;
    mpls_tunnel_switch_create_s *tunnel_term = NULL;
    mpls_port_add_s *pwe_enc = NULL;
    mpls_port_add_s *pwe_term = NULL;
    /*
     * Global parameters
     */
    params->vpn = 100;
    bcm_mac_t rif1_mac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };
    bcm_mac_t rif2_mac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x02 };
    bcm_mac_t mpls1_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };
    bcm_mac_t mpls2_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e };

    /*
     * AC parameters
     */
    params->ac_ports_nof = 2;
        /** AC1 **/
    params->ac_ports[0].port = ac_port_1;
        /** AC2 **/
    params->ac_ports[1].port = ac_port_2;

    /*
     * PWE parameters
     */
    params->pwe_ports_nof = 2;
    /**
     * PWE1 {
 */
    pwe_param = &params->pwe_params[0];
        /** Port **/
    pwe_param->pwe_port.intf = 30;
    pwe_param->pwe_port.mac_addr = rif1_mac;
    pwe_param->pwe_port.next_hop = mpls1_next_hop_mac;
    pwe_param->pwe_port.port = pwe_port_1;
    pwe_param->pwe_port.arp = 9001;
        /** Tunnel initiator **/
    mpls_tunnel_initiator_create_s_init(pwe_param->mpls_encap_tunnel);
    tunnel_init = &pwe_param->mpls_encap_tunnel[0];
    tunnel_init->label[0] = 3333;
    tunnel_init->num_labels = 1;
    tunnel_init->encap_access = bcmEncapAccessTunnel2;
    tunnel_init->flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    tunnel_init->tunnel_id = 0;
    if (!is_device_or_above(unit, JERICHO2)) {
        tunnel_init->flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_EXP_SET;
    }
        /** Tunnel termination **/
    mpls_tunnel_switch_create_s_init(pwe_param->mpls_term_tunnel);
    tunnel_term = &pwe_param->mpls_term_tunnel[0];
    tunnel_term->action = BCM_MPLS_SWITCH_ACTION_POP;
    tunnel_term->label = tunnel_init->label[0];
        /** PWE encapsulation **/
    pwe_enc = &pwe_param->pwe_encap;
    mpls_port_add_s_init(pwe_encap);
    pwe_enc->flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    pwe_enc->flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
    pwe_enc->label = 3456;
    pwe_enc->label_flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    pwe_enc->mpls_port_id = soc_property_get(0, "max_rif_id", 8192) + 0x1000;
    /** In JR1, encap_id is needed in case of EGRESS_ONLY*/
    if (!is_device_or_above(unit, JERICHO2)) {
        pwe_enc->encap_id = pwe_enc->mpls_port_id;
        pwe_enc->flags |= BCM_MPLS_PORT_ENCAP_WITH_ID;
    }
    pwe_enc->encap_access = bcmEncapAccessTunnel1;
    if (params->test_split_horizon)
    {
        pwe_enc->nwk_group_valid = 1;
        pwe_enc->pwe_nwk_group_id = params->pwe_network_group_id;
    }
        /** PWE termination **/
    pwe_term = &pwe_param->pwe_term;
    mpls_port_add_s_init(pwe_term);
    pwe_term->criteria = BCM_MPLS_PORT_MATCH_LABEL;
    pwe_term->flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    pwe_term->flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;
    pwe_term->match_label = pwe_enc->label;
    pwe_term->egress_tunnel_if = fec_id; /* learned FEC */
    fec_id +=2;
    pwe_term->forwarding_domain = params->vpn;
    if (!is_device_or_above(unit, JERICHO2)) {
        pwe_term->flags2 |= BCM_MPLS_PORT2_LEARN_ENCAP;
    }
    if (params->test_split_horizon)
    {
        pwe_term->nwk_group_valid = 1;
        pwe_term->pwe_nwk_group_id = params->pwe_network_group_id;
    }
    /*
     * MC Replication Idx 
     */
    pwe_param->pwe_mc_rep_idx = 0x303001;
    /**
     * }
     */
     /**
     * PWE2 {
 */
    pwe_param = &params->pwe_params[1];
        /** Port **/
    pwe_param->pwe_port.intf = 31;
    pwe_param->pwe_port.mac_addr = rif2_mac;
    pwe_param->pwe_port.next_hop = mpls2_next_hop_mac;
    pwe_param->pwe_port.port = pwe_port_2;
    pwe_param->pwe_port.arp = 9002;
        /** Tunnel initiator **/
    mpls_tunnel_initiator_create_s_init(pwe_param->mpls_encap_tunnel);
    tunnel_init = &pwe_param->mpls_encap_tunnel[0];
    tunnel_init->label[0] = 3335;       /* First added label (inner) */
    tunnel_init->label[1] = 3334;       /* Second added label (outer) */
    tunnel_init->num_labels = 2;
    tunnel_init->encap_access = bcmEncapAccessTunnel2;
    tunnel_init->flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    tunnel_init->tunnel_id = 0;
    if (!is_device_or_above(unit, JERICHO2)) {
        tunnel_init->flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_EXP_SET;
    }
        /** Tunnel termination **/
    mpls_tunnel_switch_create_s_init(pwe_param->mpls_term_tunnel);
    tunnel_term = &pwe_param->mpls_term_tunnel[0];
    tunnel_term->action = BCM_MPLS_SWITCH_ACTION_POP;
    tunnel_term->label = tunnel_init->label[0];
    tunnel_term = &pwe_param->mpls_term_tunnel[1];
    tunnel_term->action = BCM_MPLS_SWITCH_ACTION_POP;
    tunnel_term->label = tunnel_init->label[1];
        /** PWE encapsulation **/
    pwe_enc = &pwe_param->pwe_encap;
    mpls_port_add_s_init(pwe_encap);
    pwe_enc->flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    pwe_enc->flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
    pwe_enc->label = 3457;
    pwe_enc->label_flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    pwe_enc->mpls_port_id = soc_property_get(0, "max_rif_id", 8192) + 0x1001;
    /** In JR1, encap_id is needed in case of EGRESS_ONLY*/
    if (!is_device_or_above(unit, JERICHO2)) {
        pwe_enc->encap_id = pwe_enc->mpls_port_id;
        pwe_enc->flags |= BCM_MPLS_PORT_ENCAP_WITH_ID;
    }
    pwe_enc->encap_access = bcmEncapAccessTunnel1;
    if (params->test_split_horizon)
    {
        pwe_enc->nwk_group_valid = 1;
        pwe_enc->pwe_nwk_group_id = params->pwe_network_group_id;
    }
        /** PWE termination **/
    pwe_term = &pwe_param->pwe_term;
    mpls_port_add_s_init(pwe_term);
    pwe_term->criteria = BCM_MPLS_PORT_MATCH_LABEL;
    pwe_term->flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;
    pwe_term->match_label = pwe_enc->label;
    pwe_term->flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
    pwe_term->egress_tunnel_if = fec_id; /* learned FEC */
    pwe_term->forwarding_domain = params->vpn;
    pwe_term->failover_id = 0;
    if (!is_device_or_above(unit, JERICHO2)) {
        pwe_term->flags2 |= BCM_MPLS_PORT2_LEARN_ENCAP;
    }
    if (params->test_split_horizon)
    {
        pwe_term->nwk_group_valid = 1;
        pwe_term->pwe_nwk_group_id = params->pwe_network_group_id;
    }
    /** MC Replication Idx **/
    pwe_param->pwe_mc_rep_idx = 0x303002;
    /**
     * }
 */
    return rv;
}

/*
 * Main flow. See file header for description.
 */
int
vswitch_vpls_main(
    int unit,
    int ac_port_1,
    int ac_port_2,
    int pwe_port_1,
    int pwe_port_2)
{
    int rv = BCM_E_NONE;
    int ac_idx, pwe_idx;
    int ac_intf = 100;

    if (!vswitch_vpls_params_initialized)
    {
        /*
         * Init parameters
         */
        rv = vswitch_vpls_params_init(unit, &vswitch_vpls_params, ac_port_1, ac_port_2, pwe_port_1, pwe_port_2);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vswitch_vpls_params_init\n",rv);
            return rv;
        }
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        rv = bcm_port_tpid_add(unit, ac_port_1, 0x8100, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_port_tpid_add\n");
            return rv;
        }
        rv = bcm_port_tpid_add(unit, ac_port_2, 0x8100, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_port_tpid_add\n");
            return rv;
        }
        rv = bcm_port_tpid_add(unit, pwe_port_1, 0x8100, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_port_tpid_add\n");
            return rv;
        }
        rv = bcm_port_tpid_add(unit, pwe_port_2, 0x8100, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_port_tpid_add\n");
            return rv;
        }
    }

    /*
     * Create VPN
     */
    rv = vswitch_vpls_vpn_create(unit, vswitch_vpls_params.vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vswitch_vpls_vpn_create\n");
        return rv;
    }
    if (vswitch_vpls_params.test_split_horizon)
    {
        /*
         * Default egress native AC with network group for the PWEs
         */
        bcm_vlan_port_t_init(vswitch_vpls_params.default_native_ac);
        vswitch_vpls_params.default_native_ac.criteria = BCM_VLAN_PORT_MATCH_NONE;
        vswitch_vpls_params.default_native_ac.flags = BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_NATIVE
            | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        vswitch_vpls_params.default_native_ac.egress_network_group_id = vswitch_vpls_params.pwe_network_group_id;
        /*
         * Create default native AC for split-horizon
         */
        rv = bcm_vlan_port_create(unit, &vswitch_vpls_params.default_native_ac);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_port_create default_native_ac\n");
            return rv;
        }
    }
    /*
     * Create ACs
     */
    for (ac_idx = 0; ac_idx < vswitch_vpls_params.ac_ports_nof; ++ac_idx)
    {
        vswitch_vpls_params.ac_ports[ac_idx].intf = ac_intf;
        l2_port_properties_s *ac_port = &vswitch_vpls_params.ac_ports[ac_idx];
        rv = vswitch_vpls_ac_create(unit, ac_port, vswitch_vpls_params.vpn);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vswitch_vpls_ac_create %d\n", rv, ac_idx);
            return rv;
        }
        /*
         * Add AC to VPN
         */
        rv = vswitch_vpls_vpn_vlan_port_add(unit, vswitch_vpls_params.vpn, ac_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vswitch_vpls_vpn_vlan_port_add %d\n", rv, ac_idx);
            return rv;
        }
    }
    /*
     * Create PWEs
     */
    for (pwe_idx = 0; pwe_idx < vswitch_vpls_params.pwe_ports_nof; ++pwe_idx)
    {
        vswitch_vpls_pwe_info_s *pwe_param = &vswitch_vpls_params.pwe_params[pwe_idx];
        rv = vswitch_vpls_pwe_create(unit, pwe_param);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vswitch_vpls_pwe_create pwe_idx=%d\n", rv, pwe_idx);
            return rv;
        }
        if (vswitch_vpls_params.test_split_horizon && is_device_or_above(unit, JERICHO2))
        {
            /*
             * Connect PWE to default native AC to receive network group ID for Split horizon
             */
            bcm_port_match_info_t match_info;
            match_info.match = BCM_PORT_MATCH_PORT;
            match_info.flags |= BCM_PORT_MATCH_NATIVE | BCM_PORT_MATCH_EGRESS_ONLY;
            match_info.port = pwe_param->pwe_encap.mpls_port_id;
            rv = bcm_port_match_add(unit, vswitch_vpls_params.default_native_ac.vlan_port_id, &match_info);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_port_match_add\n");
                return rv;
            }
        }
        /*
         * Add pwe to VPN
         */
        rv = vswitch_vpls_vpn_pwe_add(unit, vswitch_vpls_params.vpn, pwe_param);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vswitch_vpls_vpn_pwe_add pwe_idx=%d\n", rv, pwe_idx);
            return rv;
        }
    }

    if (is_device_or_above(unit, JERICHO2))
    {
        /*
         * JR2 IOP: use following to translate the 15bit global fec_id to 18bit local one.
         */
        int is_jericho2_mode = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));
        if (!is_jericho2_mode) {
            rv = fec_id_alignment_field_wa_add(unit, ac_port_1, BCM_L3_ITF_VAL_GET(vswitch_vpls_params.pwe_params[0].pwe_term.egress_tunnel_if));
            if (rv != BCM_E_NONE)
            {
                printf("Error, fec_id_alignment_field_wa_entry_add\n");
                return rv;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Creats an AC port
 */
int
vswitch_vpls_ac_create(
    int unit,
    l2_port_properties_s * ac_port,
    int vid)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    /*
     * Init port
     */
    rv = vpls_configure_port_properties(unit, ac_port, 0 /* create_rif */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_configure_port_properties\n", rv);
        return rv;
    }
    /*
     * add port, according to port_vlan_vlan
     */
    bcm_vlan_port_t_init(&vlan_port);
    /*
     * set port attribures, key <port-vlan>
     */
    vlan_port.criteria = is_device_or_above(unit, JERICHO2) ? BCM_VLAN_PORT_MATCH_PORT_CVLAN : BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = ac_port->port;
    vlan_port.match_vlan = vid;
    vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : vid;
    vlan_port.vsi = 0;
    vlan_port.flags = 0;
    if (ac_port->nwk_group_valid)
    {
        vlan_port.ingress_network_group_id = ac_port->ac_nwk_group_id;
        vlan_port.egress_network_group_id = ac_port->ac_nwk_group_id;
    }

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    ac_port->vlan_port_id = vlan_port.vlan_port_id;
    ac_port->encap_id = vlan_port.encap_id;
    printf("ac_port->vlan_port_id = %d\n", ac_port->vlan_port_id);

    return rv;
}

/*
 * Creates a PWE port with the carrying MPLS tunnels and next hop ARP
 * and forward information (FEC) to be learned.
 */
int
vswitch_vpls_pwe_create(
    int unit,
    vswitch_vpls_pwe_info_s * pwe_param)
{
    int rv = BCM_E_NONE;
    /*
     * Init port
     */
    rv = vpls_configure_port_properties(unit, &pwe_param->pwe_port, 1 /* create_rif */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_configure_port_properties pwe_port\n", rv);
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = vpls_create_l3_interfaces(unit, &pwe_param->pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_l3_interfaces\n", rv);
        return rv;
    }
    /*
     * Configure an ARP entries
     */
    rv = vpls_create_arp_entry(unit, &pwe_param->pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_arp_entry\n", rv);
        return rv;
    }
    pwe_param->mpls_encap_tunnel[0].l3_intf_id = pwe_param->pwe_port.arp;
    /*
     * Configure a tunnel initiator.
     */
    rv = vpls_create_mpls_tunnel(unit, pwe_param->mpls_encap_tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_mpls_tunnel\n", rv);
        return rv;
    }
    pwe_param->pwe_port.tunnel_id = pwe_param->mpls_encap_tunnel[0].tunnel_id;
    /*
     * Configure fec entry
     */
    if (pwe_param->pwe_term.egress_tunnel_if)
    {
        int outlif_in_fec = pwe_param->mpls_encap_tunnel[0].tunnel_id;
        uint32 fec_flags = 0;
        if (mpls_tunnel_in_eei ||
            !is_device_or_above(unit, JERICHO2) ||
            is_device_or_above(unit, JERICHO2) && (soc_property_get(unit, "system_headers_mode", 1) == 0))
        {
            fec_flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
        }

        rv = vpls_create_fec_entry(unit, pwe_param->pwe_term.egress_tunnel_if,
                                   outlif_in_fec, pwe_param->pwe_port.port, fec_flags);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vpls_create_fec_entry\n", rv);
            return rv;
        }
    }
    /*
     * Configure a termination label for the ingress flow
     */
    rv = vpls_create_termination_stack(unit, pwe_param->mpls_term_tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_termination_stack\n", rv);
        return rv;
    }
    /*
     * configure PWE tunnel - egress flow
     */
    rv = vpls_mpls_port_add_encapsulation(unit, &pwe_param->pwe_encap);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mpls_port_add_encapsulation\n", rv);
        return rv;
    }
    pwe_param->pwe_term.flags |= BCM_MPLS_PORT_WITH_ID;
    pwe_param->pwe_term.mpls_port_id = pwe_param->pwe_encap.mpls_port_id;
    pwe_param->pwe_term.encap_id = pwe_param->pwe_encap.encap_id;
    /*
     * configure PWE tunnel - ingress flow
     */
     rv = vpls_mpls_port_add_termination(unit, &pwe_param->pwe_term);
     if (rv != BCM_E_NONE)
     {
         printf("Error(%d), in vpls_mpls_port_add_termination\n", rv);
         return rv;
     }

    return BCM_E_NONE;
}

/******************
 * VPN utilities
 */

/**
 * Create VPN
 */
int
vswitch_vpls_vpn_create(
    int unit,
    bcm_vpn_t vpn_id)
{
    int rv = BCM_E_NONE;
    bcm_vlan_t vid = vpn_id;
    bcm_mpls_vpn_config_t info;
    int system_headers_mode;

    /*
     * Create MC group
     */
    int mc_id = vpn_id;
    rv = multicast__open_mc_group(unit, &mc_id, (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2));
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d) in multicast__open_mc_group mc_id=%d\n", rv, mc_id);
        return rv;
    }

    /** JR2: Create fabric MC for each core in case of egress Multicast. */
    system_headers_mode = soc_property_get(unit, "system_headers_mode", 1);
    if (egress_mc && is_device_or_above(unit, JERICHO2) && (system_headers_mode == 0)
        && (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)))
    {
        int nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
        bcm_module_t mreps[2];
        mreps[0] = 0;
        mreps[1] = 1; /* Ignore it in single core devices*/
        rv = bcm_fabric_multicast_set(unit, mc_id, 0, nof_cores, mreps);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_fabric_multicast_set \n");
            return rv;
        }
    }

    /*
     * destroy before create, just to be sure it will not fail
     */
    bcm_mpls_vpn_id_destroy(unit, vpn_id);

    /*
     * Create MPLS VPN
     */
    bcm_mpls_vpn_config_t_init(&info);
    info.broadcast_group = mc_id;
    info.vpn = vpn_id;
    info.flags = BCM_MPLS_VPN_VPLS | BCM_MPLS_VPN_WITH_ID;
    info.broadcast_group = info.vpn;
    info.unknown_multicast_group = info.vpn;
    info.unknown_unicast_group = info.vpn;
    rv = bcm_mpls_vpn_id_create(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d) in bcm_vswitch_create vpn_id=%d\n", rv, vid);
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * Add AC to VPN
 */
int
vswitch_vpls_vpn_vlan_port_add(
    int unit,
    bcm_vpn_t vpn_id,
    l2_port_properties_s * ac_port)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;
    int mc_id = vpn_id;

    rv = bcm_vswitch_port_add(unit, vpn_id, ac_port->vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    BCM_GPORT_LOCAL_SET(gport, ac_port->port);
    rv = multicast__add_multicast_entry(unit, mc_id, &gport, &ac_port->encap_id, 1 /* nof_mc_entries */ , egress_mc);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in multicast__add_multicast_entry\n", rv);
        return rv;
    }
    return BCM_E_NONE;
}

/*
 * Add PWE to VPN
 */
int
vswitch_vpls_vpn_pwe_add(
    int unit,
    bcm_vpn_t vpn_id,
    vswitch_vpls_pwe_info_s * pwe_param)
{

    int rv = BCM_E_NONE;
    int mc_id = vpn_id;
    bcm_multicast_replication_t rep;
    int cuds[2];
    uint32 flags = egress_mc ? BCM_MULTICAST_EGRESS_GROUP : BCM_MULTICAST_INGRESS_GROUP;

    bcm_multicast_replication_t_init(&rep);
    rep.port = pwe_param->pwe_port.port;
    rep.encap1 = pwe_param->pwe_mc_rep_idx;
    if (!is_device_or_above(unit, JERICHO2)) {
        rep.encap1 = pwe_param->pwe_encap.encap_id;
        rep.encap2 = BCM_L3_ITF_VAL_GET(pwe_param->mpls_encap_tunnel[0].tunnel_id);
        rep.flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID;
        flags = egress_mc ? 0 : BCM_MULTICAST_INGRESS;
    } 
    rv = bcm_multicast_add(unit, mc_id, flags, 1, &rep);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), bcm_multicast_add: port = %d, encap1 = 0x%08X, encap2 = 0x%08X,\n", rv, rep.port, rep.encap1, rep.encap2);
        return rv;
    }

    /*
     * Add mpls encapsulation as outlif2 to PPMC table in JR2
     * Key: mc_rep_idx
     * Result: PWE encap_id, MPLS encap_id
     */
    if (is_device_or_above(unit, JERICHO2)) {
        BCM_L3_ITF_SET(cuds[0], BCM_L3_ITF_TYPE_LIF, pwe_param->pwe_encap.encap_id);
        cuds[1] = pwe_param->mpls_encap_tunnel[0].tunnel_id;
        rv = bcm_multicast_encap_extension_create(unit, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID, &(pwe_param->pwe_mc_rep_idx),
                2, cuds);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_multicast_encap_extension_create\n");
            return rv;
        }
    }

    return BCM_E_NONE;
}

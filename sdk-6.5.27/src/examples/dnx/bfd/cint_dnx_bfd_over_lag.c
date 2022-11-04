/*
 *
 * File: cint_dnx_bfd_over_lag.c
 * Purpose: Example of setting client/server BFD endpoint over LAG. 
 *
 * Usage:
 *
 * bfd over IPv6 tx/rx
 *
 * ./bcm.user
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_mpls.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_mpls_port.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan_translate.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_oam.c
 * cint ../../../../src/examples/dnx/oam/cint_dnx_oam.c
 * cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../../../src/examples/dnx/bfd/cint_dnx_bfd.c
 * cint ../../../../src/examples/dnx/bfd/cint_dnx_bfd_over_lag.c
 * cint
 * dnx_bfd_ep_over_lag_info.is_bfd_ipv6=1;
 * dnx_bfd_endpoint_over_lag_init(200,200,2,1,202,201);"
 * print dnx_bfd_server_example(1,2);
 *
 * Injection: 
 * Received packets on unit 1 should be: 
 *   Data: 0x00000000cd1d000c000200009100003286dd6ff00000002011ff0000000000000000123400000000ff1300000000000000000000ffffc0800101c00112b00020694920c2d0180003000400010002000000020000000300000000 
 *
 * Receive trapping
 * Sending BFD, expecting timeout events 
 *   tx 1 psrc=203 data=0x000c0002000000000000ffff8100006486dd60000000002011ff00000000000000000000ffffc08001010000000000000000123400000000ff13c00112b0002036a520c8011800010002000300040000000a0000000a00000190
 *
 */

const int MAX_NOF_PORTS_OVER_LAG = 4;

/* 
 * example varaibles. Varibables marked as IN can be changed before running the test to change configuration. Variables marked as OUT
 * will be populated throughout the tests's run.
 */
struct dnx_bfd_ep_over_lag_config_s
{
    /* Ports to be configured over the lag. */
    bcm_port_t trunk_ports[MAX_NOF_PORTS_OVER_LAG];
     /*unit associated with ports in bcm_port_t. */
    int trunk_ports_units[MAX_NOF_PORTS_OVER_LAG];
    /* Port to which the packets will be sent. */
    bcm_port_t eg_port;
    /* Port to which the packets will be sent in the second unit. */
    bcm_port_t eg_port2;
    /* BFD over IPv6 */
    uint8 is_bfd_ipv6;
    /* BFD over MPLS */
    uint8 is_bfd_mpls;
    /* SBFD initiator endpoint */
    uint8 is_sbfd_init;
    uint8 is_bfd_echo;
    /* When bfd_endpoint_id is not -1, create bfd endpoint with given id */
    int bfd_endpoint_id;
    uint32 local_discr;
    uint32 remote_discr;
    uint32 decouple_local_discr;
    /* When set to 1, classification in bfd server will be configured in addition to classification in bfd client. */
    uint32 server_classification;
    /* When set to 1, support N:1 bfd server-to-clinet mode, should set decouple_local_discr = 1.*/
    uint32 multi_servers;
};

dnx_bfd_ep_over_lag_config_s dnx_bfd_ep_over_lag_info = {
    /*
     * trunk_ports
     */
    {0, 0, 0, 0},
    /*
     * trunk_ports_units
     */
    {0, 0, 0, 0},
    /*
     * eg_port, eg_port2
     */
    0, 0,
    /*
     * is_bfd_ipv6,is_bfd_mpls,is_sbfd_init,is_bfd_echo
     */
    0, 0, 0, 0,
    /*
     * bfd_endpoint_id
     */
    -1,
    /*
     * local_discr
     */
    0x30004,
    /*
     * remote_discr
     */
    0x10002,
    /*
     * decouple_local_discr
     */
    0,
    /*
     * server_classification
     */
    0,
    /*
     * multi_servers
     */
    0
};

/*
 * The structure will hold the results for returned values
 */
struct dnx_bfd_ep_over_lag_result_s
{
    /* System ports matching port-unit */
    bcm_port_t system_ports[MAX_NOF_PORTS_OVER_LAG];
    /* Trunk gport over which the vlan port will be created. */
    bcm_gport_t trunk_gport;
    bcm_bfd_endpoint_t server_endpoint_id;
    bcm_bfd_endpoint_t client_endpoint_id;
    bcm_bfd_endpoint_t server_client_endpoint_id;
    /* server endpoint in client unit */
    bcm_bfd_endpoint_t client_server_endpoint_id;
    /* server classification in client unit */
    bcm_bfd_endpoint_t client_endpoint_id_2;
    int client_ep_remote_gport;
};

dnx_bfd_ep_over_lag_result_s dnx_bfd_ep_over_lag_result;

/*
 * dnx_bfd_endpoint_over_lag_init
 *
 * Inits the test variables to default values.
 *
 * Parameteres: 
 *  bcm_port_t  trunk_port_1-3  - (IN) 3 ports to be configured as one LAG (trunk).
 *  bcm_port_t  eg_port         - (IN) A port to be cross connected to the lag for testing. If you wish to skip the test and just set
 *                                      the configuration, set to -1.
 *
 * Retruns:
 *  BCM_E_NONE : In any case.
 *
 */
int dnx_bfd_endpoint_over_lag_init(bcm_port_t trunk_port1, bcm_port_t trunk_port2, int trunk_port1_unit,int trunk_port_2_unit , bcm_port_t eg_port, bcm_port_t eg_port2)
{
    dnx_bfd_ep_over_lag_info.trunk_ports[trunk_port1_unit]  = trunk_port1;
    dnx_bfd_ep_over_lag_info.trunk_ports[trunk_port_2_unit]  = trunk_port2;
    dnx_bfd_ep_over_lag_info.trunk_ports_units[trunk_port1_unit]  = trunk_port1_unit;
    dnx_bfd_ep_over_lag_info.trunk_ports_units[trunk_port_2_unit]  = trunk_port_2_unit;
    dnx_bfd_ep_over_lag_info.eg_port  = eg_port;
    dnx_bfd_ep_over_lag_info.eg_port2  = eg_port2;

    return BCM_E_NONE;
}

/**
 *
 * @param server_unit
 * @param client_unit
 * @param port1
 *
 * @return int
 */
int dnx_bfd_server_example(int server_unit, int client_unit)
{
    bcm_bfd_endpoint_info_t client_ep, server_ep;
    int server_oamp_port= 232; /* proper apllication must be used so that this will actually be configured as the server OAMP port*/
    bcm_gport_t remote_gport;
    bcm_field_presel_t  presel_id;
    bcm_field_layer_type_t fwd_layer;
    bcm_ip6_t *src_ipv6;
    void *context_name;
    char dest_char[8];
    bcm_field_context_t  cint_bfd_context_id_ipmf1;
    bcm_mpls_tunnel_switch_t tunnel_switch;
    int server_tunnel_id, client_tunnel_id;
    int next_hop_encap_id, next_hop_encap_id2;
    bcm_ip6_t src_ipv6_tmp = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xff, 0x13};
    int server_solo_trap_id;
    bcm_rx_trap_config_t server_solo_trap_config;
    int server_is_oamp_v2, client_is_oamp_v2;

    /** Step 0: Create LAG.  */
    BCM_IF_ERROR_RETURN_MSG(dnx_bfd_over_lag_trunk_create(server_unit,client_unit), "");

    if(dnx_bfd_ep_over_lag_info.is_bfd_mpls)
    {
        /* Here bfd_in_port / bfd_out_port are not used */
        BCM_IF_ERROR_RETURN_MSG(dnx_bfd_mpls_example_init(server_unit, dnx_bfd_gbl_config.bfd_in_port[0], dnx_bfd_gbl_config.bfd_out_port[0]), "");

        server_tunnel_id = dnx_bfd_gbl_result.encap_id;

        BCM_IF_ERROR_RETURN_MSG(dnx_bfd_mpls_example_init(client_unit, dnx_bfd_gbl_config.bfd_in_port[0], dnx_bfd_gbl_config.bfd_out_port[0]), "");

        client_tunnel_id = dnx_bfd_gbl_result.encap_id;
    }
    else
    {
        if(dnx_bfd_ep_over_lag_info.is_bfd_ipv6)
        {
            cint_ip_route_info.intf_out_mac_address[0]=0;
            cint_ip_route_info.intf_out_mac_address[1]=0x0c;
            cint_ip_route_info.intf_out_mac_address[2]=0;
            cint_ip_route_info.intf_out_mac_address[3]=0x02;
            cint_ip_route_info.intf_out_mac_address[4]=0;
            cint_ip_route_info.intf_out_mac_address[5]=0;
            cint_ip_route_info.intf_out=50;
            BCM_IF_ERROR_RETURN_MSG(basic_example_ipv6(client_unit,dnx_bfd_ep_over_lag_info.eg_port, dnx_bfd_ep_over_lag_result.trunk_gport,0,0,1), "");

            BCM_IF_ERROR_RETURN_MSG(basic_example_ipv6(server_unit,dnx_bfd_ep_over_lag_info.eg_port2, dnx_bfd_ep_over_lag_result.trunk_gport,0,0,1), "");
            BCM_L3_ITF_SET(next_hop_encap_id,BCM_L3_ITF_TYPE_LIF,0x1384);
        }
        else
        {
            /* Step 1: init BFD and set up IP routing on LAG
             *  port configured above
             */
            BCM_IF_ERROR_RETURN_MSG(dnx_bfd_ipv4_example_init(server_unit, dnx_bfd_ep_over_lag_result.trunk_gport, dnx_bfd_ep_over_lag_info.eg_port2), "");

            next_hop_encap_id = dnx_bfd_gbl_result.encap_id;

            /** init remote_gport again since 1st step change the value*/
            dnx_bfd_endpoint_info_glb.remote_gport = BCM_GPORT_INVALID;
            BCM_IF_ERROR_RETURN_MSG(dnx_bfd_ipv4_example_init(client_unit, dnx_bfd_ep_over_lag_result.trunk_gport, dnx_bfd_ep_over_lag_info.eg_port), "");

            next_hop_encap_id2 = dnx_bfd_gbl_result.encap_id;
        }
    }

    /* Step 2: set a trap with the destination set to the server
     *  side OAMP.
     */
    int trap_code;
    bcm_rx_trap_config_t trap_remote_oamp;
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(client_unit, 0, bcmRxTrapUserDefine, &trap_code), "");
    bcm_rx_trap_config_t_init(&trap_remote_oamp);
    trap_remote_oamp.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    trap_remote_oamp.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
    /* Update the forwarding offset to be ETH1+Ipv4+UDP */
    if (dnx_bfd_ep_over_lag_info.is_bfd_echo) {
        trap_remote_oamp.forwarding_header = bcmRxTrapForwardingHeaderThirdHeader;
    } else {
        trap_remote_oamp.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;
    }
    /* Set the destination*/

    int my_modid;
    int remote_oamp_sysport;
    bcm_gport_t oamp_gport[2];
    int count;

    int modid_count, actual_modid_count;
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_modid_count(server_unit, &modid_count), "");
    bcm_stk_modid_config_t modid_array[modid_count]; 

    BCM_IF_ERROR_RETURN_MSG(bcm_stk_modid_config_get_all(server_unit, modid_count, modid_array, &actual_modid_count), "");

    my_modid = modid_array[0].modid;

    appl_dnx_logical_port_to_sysport_get(server_unit,my_modid,server_oamp_port,&remote_oamp_sysport);
    BCM_GPORT_SYSTEM_PORT_ID_SET(&trap_remote_oamp.dest_port, remote_oamp_sysport);

    trap_remote_oamp.flags|= (BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX | BCM_RX_TRAP_UPDATE_FORWARDING_HEADER);
    if(dnx_bfd_ep_over_lag_info.is_bfd_mpls)
    {
        trap_remote_oamp.egress_forwarding_index = 5; /* bfd index */
        trap_remote_oamp.forwarding_header = 5; /* bfd index */
    }
    else
    {
        trap_remote_oamp.egress_forwarding_index = 3; /* bfd index */
        trap_remote_oamp.forwarding_header = 3; /* bfd index */
    }

    server_is_oamp_v2 = *(dnxc_data_get(server_unit, "oam", "oamp", "oamp_v2", NULL));
    client_is_oamp_v2 = *(dnxc_data_get(client_unit, "oam", "oamp", "oamp_v2", NULL));
    if(server_is_oamp_v2)
    {
        /* Server unit is OAMP v2 - configure outLIF for trap */
        bcm_rx_trap_type_get(server_unit, /* flags */ 0, bcmRxTrapOamBfdIpv4, &server_solo_trap_id);
        bcm_rx_trap_get(server_unit, server_solo_trap_id, &server_solo_trap_config);
        trap_remote_oamp.flags |= BCM_RX_TRAP_UPDATE_ENCAP_ID;
        trap_remote_oamp.encap_id = server_solo_trap_config.encap_id;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(client_unit, trap_code, trap_remote_oamp), "");
    printf("Trap created trap_code=%d \n", trap_code);

    /* For BFD Echo - Need to configure PMF to trap packet in client and forward to server-oamp */
    if(dnx_bfd_ep_over_lag_info.is_bfd_echo)
    {
        bcm_field_presel_t cint_bfd_ipv6_echo_presel_id = 11;
        bcm_field_presel_t cint_bfd_echo_presel_id = 10;

        bcm_ip6_t src_ipv6_tmp = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xff, 0x13};
        context_name = &dest_char[0];
        if (dnx_bfd_ep_over_lag_info.is_bfd_ipv6) {
            presel_id = cint_bfd_ipv6_echo_presel_id;
            fwd_layer = bcmFieldLayerTypeIp6;
            sal_strncpy(context_name, "ipv6 hit", sizeof(dest_char));
            src_ipv6 = &src_ipv6_tmp;
        }
        else
        {
            presel_id = cint_bfd_echo_presel_id;
            fwd_layer = bcmFieldLayerTypeIp4;
            sal_strncpy(context_name, "ipv4 hit", sizeof(dest_char));
            src_ipv6 = NULL;
        }
        BCM_IF_ERROR_RETURN_MSG(field_presel_fwd_layer_main(client_unit,presel_id, bcmFieldStageIngressPMF1, fwd_layer, &cint_bfd_context_id_ipmf1, context_name), "Ingress");

        BCM_IF_ERROR_RETURN_MSG(cint_field_bfd_echo_main(client_unit, cint_bfd_context_id_ipmf1, dnx_bfd_ep_over_lag_info.is_bfd_ipv6), "");

        /* Use trap code with server-oamp as dest_port */
        cint_field_echo_trap_id = trap_code;
        BCM_IF_ERROR_RETURN_MSG(cint_field_bfd_echo_entry_add(client_unit, 0x7fffff03, src_ipv6, dnx_bfd_ep_over_lag_info.local_discr & 0xFFFF, dnx_bfd_ep_over_lag_info.local_discr & 0xFFFF), "");
    }

    /** Step 3: set up the server side endpoint */
    bcm_bfd_endpoint_info_t_init(&server_ep);
    if(dnx_bfd_ep_over_lag_info.is_bfd_mpls)
    {
        server_ep.type = bcmBFDTunnelTypeMpls;
        server_ep.egress_if = server_tunnel_id;
        if(!dnx_bfd_ep_over_lag_info.is_bfd_ipv6) {
            server_ep.dst_ip_addr = 0;
            server_ep.src_ip_addr = 0x30F0701;
        }
        server_ep.label = dnx_bfd_endpoint_info_glb.mpls_label;
        server_ep.egress_label.ttl = 0xa;
        server_ep.egress_label.exp = 1;
        server_ep.ip_ttl = 1;
    }
    else
    {
        server_ep.type = bcmBFDTunnelTypeUdp;
        if(!server_is_oamp_v2 || (!dnx_bfd_ep_over_lag_info.is_sbfd_init && !dnx_bfd_ep_over_lag_info.is_bfd_echo))
        {
            server_ep.flags = BCM_BFD_ENDPOINT_MULTIHOP;
        }
        server_ep.egress_if = next_hop_encap_id;
        server_ep.ip_ttl = 255;
        if(!dnx_bfd_ep_over_lag_info.is_bfd_ipv6)
        {
            if (dnx_bfd_ep_over_lag_info.is_bfd_echo)
            {
                server_ep.flags |= BCM_BFD_ECHO;
                server_ep.dst_ip_addr = 0x030F0701;
                server_ep.src_ip_addr = 0x7fffff03;
            }
            else
            {
                server_ep.src_ip_addr = 0x030F0701;
                server_ep.dst_ip_addr = 0x7fffff03;
            }
        }
    }
    if(dnx_bfd_ep_over_lag_info.is_bfd_ipv6)
    {
        server_ep.flags |= BCM_BFD_ENDPOINT_IPV6;
        if (!server_is_oamp_v2)
        {
            server_ep.ipv6_extra_data_index = 8192;
        }

        bcm_ip6_t src_ipv6_tmp = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xff, 0x13};
        bcm_ip6_t dst_ipv6_tmp = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x80, 0x01, 0x01};
        if (dnx_bfd_ep_over_lag_info.is_bfd_echo)
        {
            server_ep.flags |= BCM_BFD_ECHO;
            sal_memcpy(server_ep.src_ip6_addr, src_ipv6_tmp, sizeof(dst_ipv6_tmp));
            sal_memcpy(server_ep.dst_ip6_addr, dst_ipv6_tmp, sizeof(src_ipv6_tmp));
        }
        else
        {
            sal_memcpy(server_ep.src_ip6_addr, src_ipv6_tmp, sizeof(src_ipv6_tmp));
            sal_memcpy(server_ep.dst_ip6_addr, dst_ipv6_tmp, sizeof(dst_ipv6_tmp));
        }
    }
    server_ep.flags |= BCM_BFD_ENDPOINT_IN_HW;
    server_ep.ip_tos = 255;
    server_ep.udp_src_port = 0xC001;
    server_ep.int_pri = 1;
    server_ep.bfd_period = 100;
    server_ep.local_min_tx = 2;
    server_ep.local_min_rx = 223;
    if (!dnx_bfd_ep_over_lag_info.is_bfd_echo)
    {
        server_ep.local_state = 3;
        server_ep.remote_discr = 0x10002;
    }

    server_ep.local_flags = 2;
    server_ep.local_detect_mult = 208;
    server_ep.remote_detect_mult = 30;
    server_ep.flags |= BCM_BFD_ENDPOINT_HW_ACCELERATION_SET;
    server_ep.local_discr = dnx_bfd_ep_over_lag_info.decouple_local_discr ? (dnx_bfd_ep_over_lag_info.local_discr - 1) : dnx_bfd_ep_over_lag_info.local_discr;
    if(dnx_bfd_ep_over_lag_info.decouple_local_discr && !server_is_oamp_v2)
    {
        server_ep.flags |= BCM_BFD_ENDPOINT_WITH_ID;
        server_ep.id = 4;
    }

    BCM_GPORT_TRAP_SET(remote_gport, trap_code, 7, 0);
    server_ep.remote_gport = remote_gport;
    port_to_system_port(client_unit, dnx_bfd_ep_over_lag_info.trunk_ports[client_unit], &server_ep.tx_gport);

    if(dnx_bfd_ep_over_lag_info.is_sbfd_init)
    {
        server_ep.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR;
        /* Some number outside 49152 - 65535 range since
        * SBFD does not have limitations on UDP src port range */
        server_ep.udp_src_port = 20020;
        /* local_min_rx and local_min_echo needs to be zero */
        server_ep.local_min_rx = 0;
        server_ep.local_min_echo = 0;
        /* TTL should be set to 255 - if no MPLS tunnel */
        if (!dnx_bfd_ep_over_lag_info.is_bfd_mpls) {
            server_ep.ip_ttl = 255;
        }
        /* local_state set to UP */
        server_ep.local_state = 3;
        /* Local flag should have demand bit (bit 1) set
         * P | F | C | A | D | M
         * 0 | 0 | 0 | 0 | 1 | 0
         */
        server_ep.local_flags = 0x2;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_bfd_endpoint_create(server_unit, &server_ep), "");
    dnx_bfd_ep_over_lag_result.server_endpoint_id = server_ep.id;
    printf("server_endpoint_id = 0x%x\r\n", dnx_bfd_ep_over_lag_result.server_endpoint_id);

    BCM_IF_ERROR_RETURN_MSG(register_events(server_unit), "");

    if(dnx_bfd_ep_over_lag_info.multi_servers && dnx_bfd_ep_over_lag_info.decouple_local_discr)
    {
        bcm_bfd_endpoint_info_t_init(&server_ep);
        if(dnx_bfd_ep_over_lag_info.is_bfd_mpls)
        {
            server_ep.type = bcmBFDTunnelTypeMpls;
            server_ep.egress_if = server_tunnel_id;
            if(!dnx_bfd_ep_over_lag_info.is_bfd_ipv6)
            {
                server_ep.dst_ip_addr = 0;
                server_ep.src_ip_addr = 0x30F0701;
            }
            server_ep.label = dnx_bfd_endpoint_info_glb.mpls_label;
            server_ep.egress_label.ttl = 0xa;
            server_ep.egress_label.exp = 1;
            server_ep.ip_ttl = 1;
        }
        else
        {
            server_ep.type = bcmBFDTunnelTypeUdp;
            server_ep.flags = BCM_BFD_ENDPOINT_MULTIHOP;
            server_ep.egress_if = next_hop_encap_id;
            server_ep.ip_ttl = 255;
            if(!dnx_bfd_ep_over_lag_info.is_bfd_ipv6)
            {
                if (dnx_bfd_ep_over_lag_info.is_bfd_echo1)
                {
                     server_ep.flags |= BCM_BFD_ECHO;
                     server_ep.dst_ip_addr = 0x030F0701;
                     server_ep.src_ip_addr = 0x7fffff03;
                }
                else
                {
                     server_ep.src_ip_addr = 0x030F0701;
                     server_ep.dst_ip_addr = 0x7fffff03;
                }
            }
        }
        if(dnx_bfd_ep_over_lag_info.is_bfd_ipv6)
        {
            server_ep.flags |= BCM_BFD_ENDPOINT_IPV6;
            if (!server_is_oamp_v2)
            {
                server_ep.ipv6_extra_data_index = 8192;
            }

            bcm_ip6_t src_ipv6_tmp = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xff, 0x13};
            bcm_ip6_t dst_ipv6_tmp = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x80, 0x01, 0x01};
            if (dnx_bfd_ep_over_lag_info.is_bfd_echo)
            {
                 server_ep.flags |= BCM_BFD_ECHO;
                 sal_memcpy(server_ep.src_ip6_addr, src_ipv6_tmp, sizeof(dst_ipv6_tmp));
                 sal_memcpy(server_ep.dst_ip6_addr, dst_ipv6_tmp, sizeof(src_ipv6_tmp));
            }
            else
            {
                 sal_memcpy(server_ep.src_ip6_addr, src_ipv6_tmp, sizeof(src_ipv6_tmp));
                 sal_memcpy(server_ep.dst_ip6_addr, dst_ipv6_tmp, sizeof(dst_ipv6_tmp));
            }
        }
        server_ep.flags |= BCM_BFD_ENDPOINT_IN_HW;
        server_ep.ip_tos = 255;
        server_ep.udp_src_port = 0xC001;
        server_ep.int_pri = 1;
        server_ep.bfd_period = 100;
        server_ep.local_min_tx = 2;
        server_ep.local_min_rx = 223;
        if (!dnx_bfd_ep_over_lag_info.is_bfd_echo)
        {
           server_ep.local_state = 3;
           server_ep.remote_discr = 0x10002;
        }
        server_ep.local_flags = 2;
        server_ep.local_detect_mult = 208;
        server_ep.remote_detect_mult = 30;
        server_ep.flags |= BCM_BFD_ENDPOINT_HW_ACCELERATION_SET;
        server_ep.local_discr = dnx_bfd_ep_over_lag_info.local_discr;
        server_ep.flags |= BCM_BFD_ENDPOINT_WITH_ID;
        if(server_is_oamp_v2)
        {
            server_ep.id = dnx_bfd_ep_over_lag_result.server_endpoint_id;
            if(!client_is_oamp_v2)
            {
                server_ep.ipv6_extra_data_index = server_ep.id + 8192;  /* To avoid being in the same "bank" */
            }
        }
        else
        {
            server_ep.id = 4;
        }

        BCM_GPORT_TRAP_SET(remote_gport, trap_code, 7, 0);
        server_ep.remote_gport = remote_gport;
        BCM_GPORT_TRUNK_SET(server_ep.tx_gport, dnx_bfd_ep_over_lag_result.trunk_gport); /* port that the traffic will be transmitted on: LAG port */

        if(dnx_bfd_ep_over_lag_info.is_sbfd_init)
        {
            server_ep.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR;
            /* Some number outside 49152 - 65535 range since
            * SBFD does not have limitations on UDP src port range */
            server_ep.udp_src_port = 20020;
            /* local_min_rx and local_min_echo needs to be zero */
            server_ep.local_min_rx = 0;
            server_ep.local_min_echo = 0;
            /* TTL should be set to 255 - if no MPLS tunnel */
            if (!dnx_bfd_ep_over_lag_info.is_bfd_mpls)
            {
                server_ep.ip_ttl = 255;
            }
            /* local_state set to UP */
            server_ep.local_state = 3;
            /* Local flag should have demand bit (bit 1) set
             * P | F | C | A | D | M
             * 0 | 0 | 0 | 0 | 1 | 0
             */
            server_ep.local_flags = 0x2;
        }

        BCM_IF_ERROR_RETURN_MSG(bcm_bfd_endpoint_create(client_unit, &server_ep), "");
        dnx_bfd_ep_over_lag_result.client_server_endpoint_id = server_ep.id;
        printf("client_server_endpoint_id = 0x%x\r\n", dnx_bfd_ep_over_lag_result.client_server_endpoint_id);
    }

    if (!dnx_bfd_ep_over_lag_info.is_bfd_echo) {
        /* Client side config is not required for BFD echo, since PMF does the rx classification */
        /** Step 4: Set up the client side  */
        bcm_bfd_endpoint_info_t_init(&client_ep);
        client_ep.flags = BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_MULTIHOP;
        if(dnx_bfd_ep_over_lag_info.is_bfd_ipv6) {
            client_ep.flags |= BCM_BFD_ENDPOINT_IPV6;
            if(!client_is_oamp_v2)
            {
                client_ep.ipv6_extra_data_index = 0x10000;
            }
        }

        client_ep.id = server_ep.id;
        client_ep.remote_gport = remote_gport;
        client_ep.type = bcmBFDTunnelTypeUdp;
        client_ep.local_discr = dnx_bfd_ep_over_lag_info.decouple_local_discr ? (dnx_bfd_ep_over_lag_info.local_discr - 1) : dnx_bfd_ep_over_lag_info.local_discr;
        if(dnx_bfd_ep_over_lag_info.decouple_local_discr || server_is_oamp_v2)
        {
            client_ep.flags |= BCM_BFD_ENDPOINT_WITH_ID;
        }
        client_ep.udp_src_port = 0;
        if(!dnx_bfd_ep_over_lag_info.is_bfd_ipv6) {
           client_ep.src_ip_addr = 0x30F0701;
        }
        else{
           sal_memcpy(client_ep.src_ip6_addr, src_ipv6_tmp, sizeof(src_ipv6_tmp));
        }
        if(dnx_bfd_ep_over_lag_info.is_sbfd_init) {
            client_ep.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR;
            client_ep.udp_src_port = 20020;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_bfd_endpoint_create(client_unit, &client_ep), "");
        dnx_bfd_ep_over_lag_result.client_endpoint_id = client_ep.id;
        printf("client_endpoint_id = 0x%x\r\n", dnx_bfd_ep_over_lag_result.client_endpoint_id);

        /*
         * If classifier in server side was required, 
         * add an endpoint in server unit. 
         * Packets received by the server unit will also be trapped to OAMP.
         */
        if(dnx_bfd_ep_over_lag_info.server_classification) {
           if(!dnx_bfd_ep_over_lag_info.is_bfd_ipv6) {
                BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(server_unit,0/* flags */ , bcmRxTrapOamBfdIpv4, &trap_code), "");

                bcm_rx_trap_config_t trap_config;
                BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_get(server_unit,trap_code, &trap_config), "");
                if (*(dnxc_data_get(server_unit, "headers", "system_headers", "system_headers_mode_jericho", NULL))) {
                     trap_config.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
                     trap_config.forwarding_header = 3;
                }
                else
                {
                      trap_config.flags|= (BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX | BCM_RX_TRAP_UPDATE_FORWARDING_HEADER);
                      trap_config.egress_forwarding_index = 3; /* bfd index */
                      trap_config.forwarding_header = 3; /* bfd index */
                }
                BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(server_unit, trap_code, trap_config), "");
            }
            else
            {
                if(!server_is_oamp_v2)
                {
                    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(server_unit, 0, bcmRxTrapUserDefine, &trap_code), "");
                }
                BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(server_unit, trap_code, trap_remote_oamp), "");
            }

            BCM_GPORT_TRAP_SET(remote_gport, trap_code, 7, 0);
            client_ep.remote_gport = remote_gport;

            bcm_bfd_endpoint_info_t_init(&client_ep);
            client_ep.flags = BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_WITH_ID | BCM_BFD_ENDPOINT_MULTIHOP;
            client_ep.id = server_ep.id;
            client_ep.remote_gport = remote_gport;
            client_ep.type = bcmBFDTunnelTypeUdp;
            client_ep.udp_src_port = 0;

            /** We create a MEP with the same local_disc, so it's client entry of the same MEP */
            client_ep.local_discr = dnx_bfd_ep_over_lag_info.decouple_local_discr ? (dnx_bfd_ep_over_lag_info.local_discr - 1) : dnx_bfd_ep_over_lag_info.local_discr;
            if(!dnx_bfd_ep_over_lag_info.is_bfd_ipv6) {
               client_ep.src_ip_addr = 0x30F0701;
            }
            else
            {
               client_ep.flags |= BCM_BFD_ENDPOINT_IPV6;
               sal_memcpy(client_ep.src_ip6_addr, src_ipv6_tmp, sizeof(src_ipv6_tmp));
            }

            if(dnx_bfd_ep_over_lag_info.is_sbfd_init) {
                client_ep.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR;
                client_ep.udp_src_port = 20020;
            }

            BCM_IF_ERROR_RETURN_MSG(bcm_bfd_endpoint_create(server_unit, &client_ep), "");

            dnx_bfd_ep_over_lag_result.server_client_endpoint_id = client_ep.id;
            printf("server_client_endpoint_id = 0x%x\r\n", dnx_bfd_ep_over_lag_result.server_client_endpoint_id);
        }

        if(dnx_bfd_ep_over_lag_info.multi_servers && dnx_bfd_ep_over_lag_info.decouple_local_discr)
        {
            if(!dnx_bfd_ep_over_lag_info.is_bfd_ipv6)
            {
                BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(client_unit,0/* flags */ , bcmRxTrapOamBfdIpv4, &trap_code), "");

                bcm_rx_trap_config_t trap_config;
                BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_get(server_unit,trap_code, &trap_config), "");
                trap_config.flags|= (BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX | BCM_RX_TRAP_UPDATE_FORWARDING_HEADER);
                trap_config.egress_forwarding_index = 3; /* bfd index */
                trap_config.forwarding_header = 3; /* bfd index */
                BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(server_unit, trap_code, trap_config), "");
            }

            BCM_GPORT_TRAP_SET(remote_gport, trap_code, 7, 0);
            dnx_bfd_ep_over_lag_result.client_ep_remote_gport = remote_gport;

            bcm_bfd_endpoint_info_t_init(&client_ep);
            client_ep.flags = BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_MULTIHOP;
            if(dnx_bfd_ep_over_lag_info.is_bfd_ipv6)
            {
                client_ep.flags |= BCM_BFD_ENDPOINT_IPV6;
                if(!client_is_oamp_v2)
                {
                    client_ep.ipv6_extra_data_index = 0x10000;
                }
                sal_memcpy(client_ep.src_ip6_addr, src_ipv6_tmp, sizeof(src_ipv6_tmp));
            }
            else
            {
                client_ep.src_ip_addr = 0x30F0701;
            }

            client_ep.id = dnx_bfd_ep_over_lag_result.client_server_endpoint_id;
            /* "remote_id" is necessary when using client endpoints with the same ID to different server devices */
            client_ep.remote_id = client_unit;
            client_ep.remote_gport = remote_gport;
            client_ep.type = bcmBFDTunnelTypeUdp;
            client_ep.local_discr = dnx_bfd_ep_over_lag_info.local_discr;
            client_ep.flags |= BCM_BFD_ENDPOINT_WITH_ID;

            if(dnx_bfd_ep_over_lag_info.is_sbfd_init)
            {
                client_ep.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR;
                client_ep.udp_src_port = 20020;
            }
            BCM_IF_ERROR_RETURN_MSG(bcm_bfd_endpoint_create(client_unit, &client_ep), "");
            dnx_bfd_ep_over_lag_result.client_endpoint_id_2 = client_ep.id;
            printf("client_endpoint_id_2 = 0x%x\r\n", dnx_bfd_ep_over_lag_result.client_endpoint_id_2);
        }
    }

    return BCM_E_NONE;
}

/*
 * bfd_over_lag_trunk_create
 *
 * Create a trunk, then set it to be round robin.
 *
 * Parameters:
 *
 * int unit - (IN) Device to be configured. 
 * bcm_trunk_t dnx_bfd_ep_over_lag_info.trunk_id - (OUT)Created lag.
 *
 * Returns:
 * BCM_E_NONE:  If the trunk was created successfully.
 * BCM_E_*:     If something went wrong. 
 */
int dnx_bfd_over_lag_trunk_create(int unit1,int unit2){

    bcm_trunk_member_t  member;
    bcm_trunk_info_t trunk_info;
    bcm_port_t  port;
    int flags = 0;
    bcm_trunk_t trunk_id;
    bcm_gport_t trunk_gport;
    bcm_gport_t gport;
    bcm_gport_t temp_gport;
    bcm_switch_control_info_t value;
    bcm_switch_control_key_t key;

    bcm_trunk_member_t_init(member);
    /*
     * Configure Trunk
     */
    bcm_trunk_info_t_init(trunk_info);

    BCM_TRUNK_ID_SET(trunk_id, 0, trunk_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_trunk_create(unit1, flags, &trunk_id), "");
    printf("Trunk id created is = 0x%x\n", trunk_id);

    BCM_IF_ERROR_RETURN_MSG(bcm_trunk_create(unit2, flags, &trunk_id), "");
    printf("Trunk id created is = 0x%x\n", trunk_id);

    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);
    printf("Trunk gport is: 0x%x\n",trunk_gport);

    dnx_bfd_ep_over_lag_result.trunk_gport = trunk_gport;

    bcm_trunk_member_t_init(member);
    port = dnx_bfd_ep_over_lag_info.trunk_ports[unit1];
    trunk_info.psc= BCM_TRUNK_PSC_PORTFLOW; 
    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, dnx_bfd_ep_over_lag_info.trunk_ports[unit1]);
    member.gport = gport;

    value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    key.type = bcmSwitchPortHeaderType;
    key.index = 2;
    /** Set trunk header type to match members header types */
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_indexed_port_set(unit1, trunk_gport, key, value), "(LAG gport)");

    value.value = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP;
    key.type = bcmSwitchPortHeaderType;
    key.index = 1;
    /** Set trunk header type to match members header types */
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_indexed_port_set(unit1, trunk_gport, key, value), "(LAG gport)");

    /* Finish trunk setting*/
    /* If the trunk already has member ports, they will be replaced by the new list of ports contained in the member array*/
    BCM_IF_ERROR_RETURN_MSG(bcm_trunk_set(unit1, trunk_id, &trunk_info, 1, member), "");

    /* Save the gport to global structure*/
    dnx_bfd_ep_over_lag_result.system_ports[unit1] = member.gport;

    bcm_trunk_member_t_init(member);
    port = dnx_bfd_ep_over_lag_info.trunk_ports[unit2];
    trunk_info.psc= BCM_TRUNK_PSC_PORTFLOW; 
    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, dnx_bfd_ep_over_lag_info.trunk_ports[unit2]);
    member.gport = gport;

    value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    key.type = bcmSwitchPortHeaderType;
    key.index = 2;
    /** Set trunk header type to match members header types */
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_indexed_port_set(unit2, trunk_gport, key, value), "(LAG gport)");

    value.value = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP;
    key.type = bcmSwitchPortHeaderType;
    key.index = 1;
    /** Set trunk header type to match members header types */
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_indexed_port_set(unit2, trunk_gport, key, value), "(LAG gport)");

    /* Finish trunk setting*/
    /* If the trunk already has member ports, they will be replaced by the new list of ports contained in the member array*/
    BCM_IF_ERROR_RETURN_MSG(bcm_trunk_set(unit2, trunk_id, &trunk_info, 1, member), "");

    /* Save the gport to global structure*/
    dnx_bfd_ep_over_lag_result.system_ports[unit2] = member.gport;

    return BCM_E_NONE;
}



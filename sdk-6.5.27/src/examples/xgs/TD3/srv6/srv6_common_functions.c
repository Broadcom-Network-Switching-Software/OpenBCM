int
do_srv6_decap_global_setting(int unit,
                       int *tunnel_adapt_tbl_valid,
                       int *srh_action,
                       int *non_srh_action,
                       int *dip_prefix_len,
                       int *match_payload_ovid,
                       int *match_next_header)
{
    int i, rv;
    int tunnel_adapt_tbl_cnt = 3;
    bcm_switch_control_t srh_act[3] = {
        bcmSwitchSrv6Adapt1SrhAction,
        bcmSwitchSrv6Adapt2SrhAction,
        bcmSwitchSrv6Adapt4SrhAction
    };
    bcm_switch_control_t non_srh_act[3] = {
        bcmSwitchSrv6Adapt1NonSrhAction,
        bcmSwitchSrv6Adapt2NonSrhAction,
        bcmSwitchSrv6Adapt4NonSrhAction
    };
    bcm_switch_control_t dip_pfx_len[3] = {
        bcmSwitchSrv6Adapt1DipPrefix1Len,
        bcmSwitchSrv6Adapt2DipPrefix1Len,
        bcmSwitchSrv6Adapt4DipPrefix1Len
    };
    bcm_switch_control_t mch_pld_ovid[2] = {
        bcmSwitchSrv6Adapt1MatchPayloadOvidEnable,
        bcmSwitchSrv6Adapt2MatchPayloadOvidEnable
    };
    bcm_switch_control_t mch_nxt_hdr[3] = {
        bcmSwitchSrv6Adapt1MatchNextHeaderEnable,
        bcmSwitchSrv6Adapt2MatchNextHeaderEnable,
        bcmSwitchSrv6Adapt4MatchNextHeaderEnable
    };

    /* Enable L3 egress mode */
    rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);

    /* Station move packet copy to cpu */
    if (BCM_SUCCESS(rv)) {
        rv = bcm_switch_control_set(unit, bcmSwitchL2StaticMoveToCpu, TRUE);
    }

    for (i = 0; i < tunnel_adapt_tbl_cnt; i++) {
        if (tunnel_adapt_tbl_valid[i]) {
            if (srh_action) {
                if (BCM_SUCCESS(rv)) {
                    rv = bcm_switch_control_set(unit, srh_act[i], srh_action[i]);
                }
            }
            if (non_srh_action) {
                if (BCM_SUCCESS(rv)) {
                    rv = bcm_switch_control_set(unit, non_srh_act[i], non_srh_action[i]);
                }
            }
            if (dip_prefix_len) {
                if (BCM_SUCCESS(rv)) {
                    rv = bcm_switch_control_set(unit, dip_pfx_len[i], dip_prefix_len[i]);
                }
            }
            if (match_payload_ovid && i < 2) {
                if (BCM_SUCCESS(rv)) {
                    rv = bcm_switch_control_set(unit, mch_pld_ovid[i], match_payload_ovid[i]);
                }
            }
            if (match_next_header) {
                if (BCM_SUCCESS(rv)) {
                    rv = bcm_switch_control_set(unit, mch_nxt_hdr[i], match_next_header[i]);
                }
            }
        }
    }

    return rv;

}

int
do_srv6_encap_global_setting(int unit)
{
    int rv;

    /* Enable L3 egress mode */
    rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);

    /* Station move packet copy to cpu */
    if (BCM_SUCCESS(rv)) {
        rv = bcm_switch_control_set(unit, bcmSwitchL2StaticMoveToCpu, TRUE);
    }

    if (BCM_SUCCESS(rv)) {
        rv = bcm_vlan_control_set(unit, bcmVlanTranslate, TRUE);
    }

    return rv;

}

/* Create vlan and add array of ports to vlan */
int
vlan_create_add_ports(int unit, int vid, int count, bcm_port_t *ports)
{
    bcm_pbmp_t pbmp, upbmp;
    int i;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    for (i = 0; i < count; i++) {
        BCM_PBMP_PORT_ADD(pbmp, ports[i]);
    }

    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

/* Remove given port from vlan */
int
vlan_create_remove_port(int unit, int vid, bcm_port_t port)
{
    bcm_pbmp_t pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    return bcm_vlan_port_remove(unit, vid, pbmp);
}

int
add_to_l2_table(int unit, bcm_mac_t mac, bcm_vlan_t vid, int port)
{
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, mac, vid);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = port;
    return bcm_l2_addr_add(unit, &l2_addr);
}

int
add_to_l3_defip6_table(int unit, bcm_vrf_t vrf_id, bcm_if_t intf, bcm_ip6_t ip6, bcm_ip6_t mask6)
{
    int rv;

    bcm_l3_route_t rinfo;
    bcm_l3_route_t_init(rinfo);
    rinfo.l3a_flags = BCM_L3_IP6;
    rinfo.l3a_vrf     = vrf_id;
    rinfo.l3a_intf    = intf;
    rinfo.l3a_ip6_net = ip6;
    rinfo.l3a_ip6_mask = mask6;
    rv = bcm_l3_route_add(unit, &rinfo);

    return rv;
}

int
add_to_l3_host6_table(int unit, bcm_vrf_t vrf_id, bcm_if_t intf, bcm_ip6_t ip6)
{
    int rv;

    bcm_l3_host_t hinfo;
    bcm_l3_host_t_init(hinfo);
    hinfo.l3a_flags = BCM_L3_IP6;
    hinfo.l3a_vrf     = vrf_id;
    hinfo.l3a_intf    = intf;
    hinfo.l3a_ip6_addr = ip6;
    rv = bcm_l3_host_add(unit, &hinfo);

    return rv;
}

int
prepare_packet_trace_for_tx(int unit)
{
    bshell(unit, "pw start");
    bshell(unit, "pw report +raw");
    bshell(unit, "clear c");
    return 0;
}

int
prepare_port_for_tx(int unit, bcm_port_t port)
{
    int rv;

    rv = bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC);

    return rv;
}

int
create_l3_interface(int unit, bcm_mac_t local_mac, int vid, bcm_vrf_t vrf, uint32 flow_handle,
                    uint32 flow_option, bcm_if_t *intf_id)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    l3_intf.l3a_vrf = vrf;
    l3_intf.flow_handle = flow_handle;
    l3_intf.flow_option_handle = flow_option;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    *intf_id = l3_intf.l3a_intf_id;

    return rv;
}

int
create_l3_intf_vrf(int unit, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id, bcm_vrf_t *vrf_id)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags       = BCM_L3_ADD_TO_ARL;
    l3_intf.l3a_intf_flags  = BCM_L3_INTF_OVERLAY;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    *intf_id = l3_intf.l3a_intf_id;

    /* assign vrf for intf if required */
    if (BCM_SUCCESS(rv) && vrf_id) {
        bcm_vlan_control_vlan_t vlan_ctrl;
        bcm_vlan_control_vlan_t_init(&vlan_ctrl);
        vlan_ctrl.vrf = *vrf_id;
        rv = bcm_vlan_control_vlan_selective_set(unit, *intf_id,
                         BCM_VLAN_CONTROL_VLAN_VRF_MASK, &vlan_ctrl);
    }

    return rv;
}

int
create_egr_obj(int unit, uint32 flags, uint32 flags2, uint32 flow_handle, uint32 flow_option,
               bcm_if_t l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
               int vid, bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags2 = flags2;
    l3_egress.flow_handle = flow_handle;
    l3_egress.flow_option_handle = flow_option;
    l3_egress.intf  = l3_if;
    sal_memcpy(l3_egress.mac_addr, nh_mac, sizeof(nh_mac));
    l3_egress.vlan = vid;
    l3_egress.port = gport;
    return bcm_l3_egress_create(unit, flags, &l3_egress, egr_obj_id);
}

int
port_srv6_enable(int unit, bcm_gport_t port, int srv6_enable)
{
    int rv;
    rv = bcm_port_control_set(unit, port, bcmPortControlSrv6Enable, srv6_enable);

    return rv;
}

int
send_packet(int unit, bcm_port_t tx_port, char *pkt_data)
{
    int rv;
    char cmd_str[1024];

    if (tx_port < 32) {
        sprintf(cmd_str, "tx 1 pbm=0x%x data=%s", 1 << tx_port, pkt_data);
    } else if (tx_port < 64) {
        sprintf(cmd_str, "tx 1 pbm=0x%x00000000 data=%s", 1 << tx_port, pkt_data);
    } else if (tx_port < 96) {
        sprintf(cmd_str, "tx 1 pbm=0x%x0000000000000000 data=%s", 1 << tx_port, pkt_data);
    } else if (tx_port < 128) {
        sprintf(cmd_str, "tx 1 pbm=0x%x000000000000000000000000 data=%s", 1 << tx_port, pkt_data);
    } else if (tx_port < 160) {
        sprintf(cmd_str, "tx 1 pbm=0x%x00000000000000000000000000000000 data=%s", 1 << tx_port, pkt_data);
    } else if (tx_port < 192) {
        sprintf(cmd_str, "tx 1 pbm=0x%x0000000000000000000000000000000000000000 data=%s", 1 << tx_port, pkt_data);
    } else {
        printf("Wrong port number (%d).\n", tx_port);
        rv = BCM_E_PARAM;
    }

    if (BCM_SUCCESS(rv)) {
        bshell(unit, cmd_str);
    }

    return rv;
}

int
flow_match_dip6_intf_add(int unit, char *table_type, bcm_ip6_t match_dip6, bcm_if_t match_intf, int tunnel_action)
{
    int rv;

    uint32 flow_handle;
    uint32 match_dip_flow_option;
    bcm_flow_match_config_t info;

    rv = bcm_flow_handle_get(unit, "SRV6", &flow_handle);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    rv = bcm_flow_option_id_get(unit, flow_handle, table_type, &match_dip_flow_option);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    bcm_flow_match_config_t_init(&info);
    info.flow_handle    = flow_handle;
    info.flow_option    = match_dip_flow_option;
    info.criteria       = BCM_FLOW_MATCH_CRITERIA_DIP;
    info.dip6           = match_dip6;
    info.intf_id        = match_intf;
    info.tunnel_action  = tunnel_action;
    info.valid_elements = BCM_FLOW_MATCH_DIP_V6_VALID |
                          BCM_FLOW_MATCH_TNL_ACTION_VALID |
                          BCM_FLOW_MATCH_IIF_VALID;

    rv = bcm_flow_match_add(unit, &info, 0, NULL);

    return rv;
}

int
flow_tunnel_term_create(int unit, char *table_type, bcm_ip6_t dip6, bcm_if_t intf_id)
{
    int rv;

    uint32 flow_handle;
    uint32 term_flow_option;
    bcm_flow_tunnel_terminator_t ttinfo;

    rv = bcm_flow_handle_get(unit, "SRV6", &flow_handle);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    rv = bcm_flow_option_id_get(unit, flow_handle, table_type, &term_flow_option);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    ttinfo.flow_handle = flow_handle;
    ttinfo.flow_option = term_flow_option;
    ttinfo.valid_elements = BCM_FLOW_TUNNEL_TERM_DIP6_VALID |
                            BCM_FLOW_TUNNEL_TERM_INTF_ID_VALID;
    ttinfo.dip6 = dip6;
    ttinfo.intf_id = intf_id;
    ttinfo.type = bcmTunnelTypeL2Flex;
    rv = bcm_flow_tunnel_terminator_create(unit, &ttinfo, 0, NULL);

    return rv;
}

int
flow_vpn_group_create(int unit, bcm_vpn_t *vpn, bcm_multicast_t *mcast, uint32 flags, uint32 egress_service_tpid, bcm_vlan_t egress_service_vlan)
{
    int rv = 0;
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_FLOW, mcast);
    if (rv != BCM_E_NONE) {
        printf("bcm_multicast_create() %s mc group=%d\n", bcm_errmsg(rv), *mcast);
    }

    bcm_flow_vpn_config_t vpn_info;
    bcm_flow_vpn_config_t_init(&vpn_info);

    vpn_info.broadcast_group = *mcast;
    vpn_info.unknown_multicast_group = *mcast;
    vpn_info.unknown_unicast_group = *mcast;

    if (flags == 0) {
        vpn_info.flags = BCM_FLOW_VPN_ELAN | BCM_FLOW_VPN_WITH_ID;
    } else {
        vpn_info.flags = flags;
    }
    if (egress_service_tpid != 0) {
        vpn_info.egress_service_tpid = egress_service_tpid;
        vpn_info.egress_service_vlan = egress_service_vlan;
    }

    rv = bcm_flow_vpn_create(unit, vpn, &vpn_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_flow_vpn_create() %s vpn=%d\n", bcm_errmsg(rv), *vpn);
    }

    return rv;
}

int
create_flow_acc_vp(int unit, bcm_vpn_t vpn, uint32 flags, bcm_gport_t *vp)
{
    bcm_flow_port_t flow_port;
    bcm_error_t rv = BCM_E_NONE;

    bcm_flow_port_t_init(&flow_port);
    flow_port.flags = flags;
    rv = bcm_flow_port_create(unit, vpn, &flow_port);
    *vp = flow_port.flow_port_id;

    return rv;
}

int
create_flow_net_vp(int unit, bcm_vpn_t vpn, uint32 flags, bcm_gport_t *vp)
{
    bcm_flow_port_t flow_port;
    bcm_error_t rv = BCM_E_NONE;

    bcm_flow_port_t_init(&flow_port);
    flow_port.flags = flags;
    rv = bcm_flow_port_create(unit, vpn, &flow_port);
    *vp = flow_port.flow_port_id;

    return rv;
}

int l3_egress_create(int unit, uint32 flags, uint32 flags2, bcm_if_t intf, bcm_gport_t port,
                     bcm_mac_t mac_addr, bcm_if_t *egr_obj_id)
{
    int rv = 0;
    bcm_l3_egress_t egr_t;
    bcm_l3_egress_t_init(&egr_t);
    egr_t.port = port;
    egr_t.intf = intf;
    egr_t.flags = flags;
    egr_t.mac_addr = mac_addr;

    rv = bcm_l3_egress_create(unit, 0, egr_t, egr_obj_id);
    if (rv != 0) {
        printf("bcm_l3_egress_create() %s egr_obj_id=0x%x\n", bcm_errmsg(rv), *egr_obj_id);
    }

    return rv;
}


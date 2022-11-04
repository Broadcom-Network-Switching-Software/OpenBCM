cint_reset();

uint32 ifa20_proto = 253; /* Experimental protocol number */
int max_payload_len = 1500;
uint32 opaque_val = 0xc1;

/* VxLan tunnel initiation setup */
bcm_mac_t remote_mac_network = "00:00:00:00:00:02";
bcm_mac_t local_mac_network = "00:00:00:00:22:22";

bcm_mac_t payload_remote_mac = "00:00:00:00:aa:aa";
bcm_mac_t payload_local_mac = "00:00:00:00:bb:bb";
bcm_flow_tunnel_terminator_t tnl_term;

uint32 vnid = 0x12345;
bcm_vlan_t vid_acc = 21;
bcm_vlan_t vid_network = 22;
bcm_if_t intf_id_network = 2;
bcm_vrf_t vrf = 0;
bcm_multicast_t bcast_group = 0;    /* For VXLAN access and network VPs */

bcm_port_t port_acc;
bcm_port_t port_network_1;
bcm_port_t port_network_2;
bcm_port_t port_network_3;
bcm_port_t port_network_4;
bcm_gport_t gport_acc = BCM_GPORT_INVALID;
bcm_gport_t gport_network_1 = BCM_GPORT_INVALID;
bcm_gport_t gport_network_2 = BCM_GPORT_INVALID;
bcm_gport_t gport_network_3 = BCM_GPORT_INVALID;
bcm_gport_t gport_network_4 = BCM_GPORT_INVALID;

bcm_ip_t tnl_local_ip = 0x0a0a0a01;
bcm_ip_t tnl_remote_ip = 0xC0A80101;   /* 192.168.1.1 */
uint16 udp_dst_port = 8472;            /* Destination UDP port */
uint16 udp_src_port = 0xffff;          /* Source UDP port */

/* FP qualifying configuration */
bcm_ip_t acc_src_ip = 0x0a0a0101;
bcm_ip_t acc_src_ip_mask = 0xffffffff;
bcm_ip_t acc_dst_ip = 0x0a0a0200;
bcm_ip_t acc_dst_ip_mask = 0xffffffff;
uint16 acc_udp_sport = 0x3e;
uint16 acc_udp_sport_mask = 0xffff;
uint16 acc_udp_dport = 0x1111;
uint16 acc_udp_dport_mask = 0xffff;
uint8 acc_proto = 0x11;
uint8 acc_proto_mask = 0xff;

/* S-FLOW sampling configuration */
bcm_gport_t mgport;
bcm_mirror_destination_t mdest;
int mport = 65; /* Mirroring to internal loopback port LB0 */

/*
 *    Checks if given port/index is already present
 *    in the list so that same port number is not generated again
 */

int
checkPortAssigned(int *port_index_list,int no_entries, int index)
{
    int i = 0;

    for (i = 0; i < no_entries; i++) {
        if (port_index_list[i] == index)
            return 1;
    }

    /* no entry found */
    return 0;
}

/*
 *    Provides required number of ports
 */

int
portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, index = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;
    int tempports[BCM_PBMP_PORT_MAX];
    int port_index_list[num_ports];

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &configP));

    ports_pbmp = configP.e;
    for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i)) {
            tempports[port] = i;
            port++;
        }
    }

    if (( port == 0 ) || ( port < num_ports )) {
        printf("portNumbersGet() failed \n");
        return -1;
    }

    /* generating random ports */
    for (i= 0; i < num_ports; i++) {
        index = sal_rand() % port;
        if (checkPortAssigned(port_index_list, i, index) == 0) {
            port_list[i] = tempports[index];
            port_index_list[i] = index;
        } else {
            i = i - 1;
        }
    }

    return BCM_E_NONE;
}

/*
 *    Global IFA configuration
 */

int
ifa_global_setting(int unit, bcm_gport_t gport)
{
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2HeaderType, ifa20_proto));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2MetaDataFirstLookupType, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2MetaDataSecondLookupType, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFAMetaDataAdaptTable, BCM_SWITCH_IFA_METADATA_ADAPT1_SINGLE));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, 0, bcmPortControlIP4, 1));

    return BCM_E_NONE;
}

/* VxLAN global configuration */
int
vxlan_global_setting(int unit)
{

    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_acc, bcmPortControlVxlanEnable, 0));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_acc, bcmPortControlVxlanTunnelbasedVnId, 0));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_network_1, bcmPortControlVxlanEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_network_1, bcmPortControlVxlanTunnelbasedVnId, 0));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_network_2, bcmPortControlVxlanEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_network_2, bcmPortControlVxlanTunnelbasedVnId, 0));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_network_3, bcmPortControlVxlanEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_network_3, bcmPortControlVxlanTunnelbasedVnId, 0));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_network_4, bcmPortControlVxlanEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_network_4, bcmPortControlVxlanTunnelbasedVnId, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchVxlanUdpDestPortSet, udp_dst_port));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchVxlanTunnelMissToCpu, 1));
    BCM_IF_ERROR_RETURN(bcm_vlan_control_set(unit, bcmVlanTranslate, 1));
    BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, port_acc, bcmVlanTranslateIngressEnable, 1));

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,bcmSwitchVxlanEntropyEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEntropy, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,bcmSwitchVxlanUdpSrcPortSelect, bcmSwitchTunnelUdpSrcPortHashEntropy));
    return BCM_E_NONE;
}
bcm_error_t trunk_rtag7_config(int unit)
{
    int flags;
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_switch_control_get(unit, bcmSwitchHashControl, &flags);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_get(bcmSwitchHashControl) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    flags |= (BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE |
              BCM_HASH_CONTROL_TRUNK_NUC_SRC |
              BCM_HASH_CONTROL_TRUNK_NUC_DST |
              BCM_HASH_CONTROL_TRUNK_NUC_MODPORT);
    rv = bcm_switch_control_set(unit, bcmSwitchHashControl, flags);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashControl) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Only use L2 fields for hashing even for IP packets */
    rv = bcm_switch_control_get(unit,bcmSwitchHashSelectControl, &flags);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_get(bcmSwitchHashSelectControl) : %s\n", bcm_errmsg(rv));
      return rv;
    }
    flags |=(BCM_HASH_FIELD0_DISABLE_IP4|
             BCM_HASH_FIELD1_DISABLE_IP4|
             BCM_HASH_FIELD0_DISABLE_IP6|
             BCM_HASH_FIELD1_DISABLE_IP6);
    rv = bcm_switch_control_set(unit, bcmSwitchHashSelectControl, flags);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashSelectControl) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Block A - L2 packet field selection (RTAG7_HASH_FIELD_BMAP3) */
    rv = bcm_switch_control_set(unit, bcmSwitchHashL2Field0,
            BCM_HASH_FIELD_SRCMOD |
            BCM_HASH_FIELD_SRCPORT |
            BCM_HASH_FIELD_VLAN |
            BCM_HASH_FIELD_ETHER_TYPE |
            BCM_HASH_FIELD_MACDA_LO |
            BCM_HASH_FIELD_MACDA_MI |
            BCM_HASH_FIELD_MACDA_HI |
            BCM_HASH_FIELD_MACSA_LO |
            BCM_HASH_FIELD_MACSA_MI |
            BCM_HASH_FIELD_MACSA_HI);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashL2Field0) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Block B - L2 packet field selection */
    rv = bcm_switch_control_set(unit, bcmSwitchHashL2Field1,
            BCM_HASH_FIELD_SRCMOD |
            BCM_HASH_FIELD_SRCPORT |
            BCM_HASH_FIELD_VLAN |
            BCM_HASH_FIELD_ETHER_TYPE |
            BCM_HASH_FIELD_MACDA_LO |
            BCM_HASH_FIELD_MACDA_MI |
            BCM_HASH_FIELD_MACDA_HI |
            BCM_HASH_FIELD_MACSA_LO |
            BCM_HASH_FIELD_MACSA_MI |
            BCM_HASH_FIELD_MACSA_HI);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashL2Field1) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x33333333);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashSeed0) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555);
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashSeed1) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Configure HASH A and HASH B functions */
    rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config,
            BCM_HASH_FIELD_CONFIG_CRC32LO); /* BCM_HASH_FIELD_CONFIG_CRC32LO */
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashField0Config) : %s\n", bcm_errmsg(rv));
      return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchHashField1Config,
            BCM_HASH_FIELD_CONFIG_CRC16); /* BCM_HASH_FIELD_CONFIG_CRC32HI */
    if(BCM_FAILURE(rv))
    {
      printf("\nError executing bcm_switch_control_set(bcmSwitchHashField1Config) : %s\n", bcm_errmsg(rv));
      return rv;
    }


    return BCM_E_NONE;
}

/* VxLAN configuration */
int
vxlan_config(int unit)
{
    bcm_error_t rv;
    bcm_flow_encap_config_t einfo;
    uint32 flow_handle;
    bcm_pbmp_t pbmp, upbmp;
    uint32 flags;
    bcm_flow_vpn_config_t vpn_info;
    bcm_flow_port_t flow_port;
    bcm_l3_intf_t l3_intf;
    bcm_if_t egr_obj_network;

    bcm_l3_egress_t l3_egress;
    bcm_vpn_t vpn;
    int acc_flow_port;
    int net_flow_port;
    bcm_l2_addr_t l2_addr;

    rv = vxlan_global_setting(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in vxlan global setting %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_acc, &gport_acc));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_network_1, &gport_network_1));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_network_2, &gport_network_2));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_network_3, &gport_network_3));
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_network_4, &gport_network_4));

    bcm_trunk_info_t t_info;
    bcm_trunk_t tid = 8;
    int tflags = BCM_TRUNK_FLAG_WITH_ID;
    int nports = 4;
    bcm_trunk_member_t  members[4];

    print bcm_trunk_init(unit); //do this during trunk init code
    print bcm_trunk_create(unit, tflags, &tid);

    print bcm_trunk_info_t_init(&t_info);

    t_info.dlf_index  = BCM_TRUNK_UNSPEC_INDEX;
    t_info.mc_index   = BCM_TRUNK_UNSPEC_INDEX;
    t_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;
    t_info.psc        = BCM_TRUNK_PSC_PORTFLOW;

    print bcm_trunk_member_t_init(&members[0]);
    print bcm_trunk_member_t_init(&members[1]);
    print bcm_trunk_member_t_init(&members[2]);
    print bcm_trunk_member_t_init(&members[3]);

    members[0].gport = gport_network_1;
    members[1].gport = gport_network_2;
    members[2].gport = gport_network_3;
    members[3].gport = gport_network_4;
    print bcm_trunk_set(unit, tid, &t_info, nports, members);

    bcm_gport_t gtid;
    BCM_GPORT_TRUNK_SET(gtid, tid);
    print gtid;

    /* VLAN settings */
    rv = bcm_vlan_create(unit, vid_acc);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in vlan create %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_acc);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid_acc, pbmp, upbmp));

    rv = bcm_vlan_create(unit, vid_network);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in vlan create%s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_network_1);
    BCM_PBMP_PORT_ADD(pbmp, port_network_2);
    BCM_PBMP_PORT_ADD(pbmp, port_network_3);
    BCM_PBMP_PORT_ADD(pbmp, port_network_4);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid_network, pbmp, upbmp));

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags = BCM_L3_WITH_ID;
    l3_intf.l3a_intf_id = intf_id_network;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac_network, sizeof(local_mac_network));
    l3_intf.l3a_vid = vid_network;
    l3_intf.l3a_vrf = vrf;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("Error in l3 intf create %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Egress object for network port */
    bcm_l3_egress_t_init(&l3_egress);
    /* l3_egress.flags = BCM_L3_TGID; */
    l3_egress.flags2 = BCM_L3_FLOW_ONLY | BCM_L3_FLAGS2_IFA_DST_PORT;
    l3_egress.intf = intf_id_network;
    sal_memcpy(l3_egress.mac_addr,  remote_mac_network, sizeof(remote_mac_network));
    l3_egress.vlan = vid_network;
    l3_egress.port = gtid;
    rv = bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_network);
    if (BCM_FAILURE(rv)) {
        printf("Error in l3 egress create %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* VPN create */
    /* Multicast group for non-UC */
    bcm_flow_vpn_config_t_init(&vpn_info);
    flags = BCM_MULTICAST_TYPE_FLOW;
    rv = bcm_multicast_create(unit, flags, &bcast_group);
    vpn_info.flags = BCM_FLOW_VPN_ELAN;
    vpn_info.broadcast_group = bcast_group;
    vpn_info.unknown_multicast_group = bcast_group;
    vpn_info.unknown_unicast_group = bcast_group;
    rv = bcm_flow_vpn_create(unit, &vpn, &vpn_info);
    if (BCM_FAILURE(rv)) {
        printf("Error in flow vpn create %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("vpn: 0x%x\n",vpn);

    /* access VP creation */
    bcm_flow_port_t_init(&flow_port);
    rv = bcm_flow_port_create(unit, vpn, &flow_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in acc flow port create %s\n", bcm_errmsg(rv));
        return rv;
    }

    acc_flow_port = flow_port.flow_port_id;
    printf("acc_flow_port: 0x%x\n",acc_flow_port);

    /* network VP creation */
    bcm_flow_port_t_init(&flow_port);
    flow_port.flags = BCM_FLOW_PORT_NETWORK;
    rv = bcm_flow_port_create(unit, vpn, &flow_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in network flow port create %s\n", bcm_errmsg(rv));
        return rv;
    }

    net_flow_port = flow_port.flow_port_id;
    printf("net_flow_port: 0x%x\n",net_flow_port);

    rv = bcm_flow_handle_get(unit, "CLASSIC_VXLAN", &flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("Error in flow handle get for vxlan %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* access port side */
    bcm_flow_match_config_t info;
    bcm_flow_match_config_t_init(&info);

    /* generic: port match criteria, flow_handle must be 0, error check */
    /* port/vlan criteria */
    info.criteria = BCM_FLOW_MATCH_CRITERIA_PORT_VLAN;
    info.port = gport_acc;
    info.flow_port = acc_flow_port;
    info.vlan = vid_acc;
    info.valid_elements = BCM_FLOW_MATCH_PORT_VALID |
    BCM_FLOW_MATCH_VLAN_VALID |
    BCM_FLOW_MATCH_FLOW_PORT_VALID;
    rv = bcm_flow_match_add(unit, &info, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("Error in flow match add %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* network port side */
    /* flow_port encap set. Set the flow type matching the tunnel */
    bcm_flow_port_encap_t peinfo;
    bcm_flow_port_encap_t_init(&peinfo);

    peinfo.flow_handle = flow_handle;
    peinfo.class_id = 0x3;
    peinfo.flow_port = net_flow_port;
    peinfo.egress_if = egr_obj_network;
    peinfo.valid_elements = BCM_FLOW_PORT_ENCAP_PORT_VALID |
    BCM_FLOW_PORT_ENCAP_CLASS_ID_VALID |
    BCM_FLOW_PORT_ENCAP_EGRESS_IF_VALID;
    rv = bcm_flow_port_encap_set(unit, &peinfo, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("Error in flow port encap set %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* encap add, VFI-->VNID */
    bcm_flow_encap_config_t_init(&einfo);

    einfo.flow_handle = flow_handle;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_VFI;
    einfo.vnid = vnid;
    einfo.vpn = vpn;
    einfo.valid_elements = BCM_FLOW_ENCAP_VNID_VALID | BCM_FLOW_ENCAP_VPN_VALID;
    rv = bcm_flow_encap_add(unit, &einfo, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("Error in flow encap add %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* set the key type on the dvp for the given encap criteria */
    rv = bcm_vlan_control_port_set(unit, net_flow_port, bcmVlanPortTranslateEgressKey,
                                   bcmVlanTranslateEgressKeyVpn);
    if (BCM_FAILURE(rv)) {
        printf("Error in vlan control port set %s\n", bcm_errmsg(rv));
        return rv;
    }


    /* create a vxlan IP tunnel for the given flow port */
    bcm_flow_tunnel_initiator_t tiinfo;

    bcm_flow_tunnel_initiator_t_init(&tiinfo);
    tiinfo.flow_handle = flow_handle;
    tiinfo.valid_elements = BCM_FLOW_TUNNEL_INIT_DIP_VALID |
    BCM_FLOW_TUNNEL_INIT_SIP_VALID |
    BCM_FLOW_TUNNEL_INIT_UDP_DPORT_VALID |
    BCM_FLOW_TUNNEL_INIT_UDP_SPORT_VALID;
    tiinfo.flow_port = net_flow_port;
    tiinfo.type = bcmTunnelTypeVxlan;
    tiinfo.udp_src_port = udp_src_port;
    tiinfo.udp_dst_port = udp_dst_port;
    tiinfo.sip = tnl_local_ip;
    tiinfo.dip = tnl_remote_ip;
    rv = bcm_flow_tunnel_initiator_create(unit, &tiinfo, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("Error in flow tunnel initiator create %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* L2 address setup */
    bcm_l2_addr_t_init(&l2_addr, payload_remote_mac, vpn);
    l2_addr.flags = BCM_L2_STATIC;
    sal_memcpy(l2_addr.mac, payload_remote_mac, sizeof(payload_remote_mac));
    l2_addr.vid = vpn;
    l2_addr.port = net_flow_port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (BCM_FAILURE(rv)) {
        printf("Error in l2 addr add %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Mirroring and SFLOW configuration */

int
mirror_config(int unit)
{
    bcm_error_t rv;

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, mport, &mgport));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, mgport, bcmPortControlInternalLoopback, 1)); /* Disable src mod/src port override. */
    BCM_IF_ERROR_RETURN(bcm_port_vlan_member_set(unit, mgport, 0)); /* Disable egress vlan checks. */

    /* Configuring mirror destination for flex samples to int lb port */
    bcm_mirror_destination_t_init(&mdest);
    mdest.gport = mgport;
    mdest.flags2 = BCM_MIRROR_DEST_FLAGS2_IFA;
    rv = bcm_mirror_destination_create(unit, &mdest);
    if(BCM_FAILURE(rv)) {
        printf("\nError in mirror destination create: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_mirror_port_dest_add(unit, 0, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_SFLOW, mdest.mirror_dest_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in mirror port add: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    /* NOTE : On cancun 6.6.1 and later version, the user IFA2.0 application should call switch control
	   bcmSwitchIFA2Enable after bcm_mirror_port_dest_add API */
	BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2Enable, 1));
    /* Setting Flex Sample destination to mirror */
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_acc, bcmPortControlSampleFlexDest, BCM_PORT_CONTROL_SAMPLE_DEST_MIRROR));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_acc, bcmPortControlSampleFlexRate, 1));

    return BCM_E_NONE;
}

/* IFA first pass FP configuration */

int
ifa_first_pass(int unit)
{
    bcm_field_group_config_t group_config_pass_1;
    bcm_field_entry_t entry_pass_1;
    bcm_error_t rv;

    bcm_field_group_config_t_init(&group_config_pass_1);

    BCM_FIELD_QSET_INIT(group_config_pass_1.qset);
    BCM_FIELD_ASET_INIT(group_config_pass_1.aset);

    BCM_FIELD_QSET_ADD(group_config_pass_1.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config_pass_1.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config_pass_1.qset, bcmFieldQualifySrcIp);
    BCM_FIELD_QSET_ADD(group_config_pass_1.qset, bcmFieldQualifyDstIp);
    BCM_FIELD_QSET_ADD(group_config_pass_1.qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(group_config_pass_1.qset, bcmFieldQualifyL4SrcPort);
    BCM_FIELD_QSET_ADD(group_config_pass_1.qset, bcmFieldQualifyL4DstPort);

    BCM_FIELD_ASET_ADD(group_config_pass_1.aset, bcmFieldActionEgressFlowControlEnable);
    BCM_FIELD_ASET_ADD(group_config_pass_1.aset, bcmFieldActionLoopbackType);
    BCM_FIELD_ASET_ADD(group_config_pass_1.aset, bcmFieldActionLoopbackSubtype);
    BCM_FIELD_ASET_ADD(group_config_pass_1.aset, bcmFieldActionIngSampleEnable);
    BCM_FIELD_ASET_ADD(group_config_pass_1.aset, bcmFieldActionStatGroup);
    BCM_FIELD_ASET_ADD(group_config_pass_1.aset, bcmFieldActionCopyToCpu);

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config_pass_1));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config_pass_1.group, &entry_pass_1));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcIp(unit, entry_pass_1, acc_src_ip, acc_src_ip_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstIp(unit, entry_pass_1, acc_dst_ip, acc_dst_ip_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IpProtocol(unit, entry_pass_1, acc_proto, acc_proto_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_L4SrcPort(unit, entry_pass_1, acc_udp_sport, acc_udp_sport_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_L4DstPort(unit, entry_pass_1, acc_udp_dport, acc_udp_dport_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry_pass_1, port_acc, -1));

    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_1, bcmFieldActionEgressFlowControlEnable, bcmFieldEgressFlowControlLoopbackRedirect, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_1, bcmFieldActionLoopbackSubtype, bcmFieldLoopbackSubtypeIfa, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_1, bcmFieldActionLoopbackType, bcmFieldLoopbackTypeRedirect, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_1, bcmFieldActionIngSampleEnable, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry_pass_1));

    return BCM_E_NONE;
}

/* IFA second pass FP configuration */

int
ifa_second_pass(int unit)
{
    bcm_field_group_config_t group_config_pass_2;
    bcm_field_entry_t  entry_pass_2;
    bcm_error_t rv;

    bcm_field_group_config_t_init(&group_config_pass_2);
    BCM_FIELD_QSET_INIT(group_config_pass_2.qset);
    BCM_FIELD_ASET_INIT(group_config_pass_2.aset);

    BCM_FIELD_QSET_ADD(group_config_pass_2.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config_pass_2.qset, bcmFieldQualifyLoopbackSubtype);
    BCM_FIELD_QSET_ADD(group_config_pass_2.qset, bcmFieldQualifyIpProtocol);

    BCM_FIELD_ASET_ADD(group_config_pass_2.aset, bcmFieldActionEncapIfaMetadataHdr);
    BCM_FIELD_ASET_ADD(group_config_pass_2.aset, bcmFieldActionRedirectPort);
    BCM_FIELD_ASET_ADD(group_config_pass_2.aset, bcmFieldActionEgressFlowControlEnable);
    BCM_FIELD_ASET_ADD(group_config_pass_2.aset, bcmFieldActionEgressFlowEncapEnable);
    BCM_FIELD_ASET_ADD(group_config_pass_2.aset, bcmFieldActionStatGroup);
    BCM_FIELD_ASET_ADD(group_config_pass_2.aset, bcmFieldActionCopyToCpu);

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config_pass_2));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config_pass_2.group, &entry_pass_2));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_LoopbackSubtype(unit,entry_pass_2, bcmFieldLoopbackSubtypeIfa));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IpProtocol(unit, entry_pass_2, acc_proto, acc_proto_mask));

    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_2, bcmFieldActionRedirectPort, 0 , 65)); /* Redirect to loopback port */
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_2, bcmFieldActionEncapIfaMetadataHdr, opaque_val, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_2, bcmFieldActionEgressFlowControlEnable, bcmFieldEgressFlowControlIfaProbeSecondPass, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_2, bcmFieldActionEgressFlowEncapEnable, bcmFieldEgressFlowEncapIntReply, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry_pass_2));

    return BCM_E_NONE;
}

/* IFA third pass FP configuration */

int
ifa_third_pass(int unit)
{
    bcm_field_group_config_t group_config_pass_3;
    bcm_field_entry_t  entry_pass_3;
    bcm_error_t rv;

    bcm_field_group_config_t_init(&group_config_pass_3);
    BCM_FIELD_QSET_INIT(group_config_pass_3.qset);
    BCM_FIELD_ASET_INIT(group_config_pass_3.aset);

    BCM_FIELD_QSET_ADD(group_config_pass_3.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config_pass_3.qset, bcmFieldQualifyIfaVersion2);
    BCM_FIELD_QSET_ADD(group_config_pass_3.qset, bcmFieldQualifyPktType);

    BCM_FIELD_QSET_ADD(group_config_pass_3.qset,bcmFieldQualifyDstPort);
    BCM_FIELD_QSET_ADD(group_config_pass_3.qset,bcmFieldQualifyL4SrcPort);

    BCM_FIELD_ASET_ADD(group_config_pass_3.aset, bcmFieldActionAssignEditCtrlId);
    BCM_FIELD_ASET_ADD(group_config_pass_3.aset, bcmFieldActionAssignExtractionCtrlId);
    BCM_FIELD_ASET_ADD(group_config_pass_3.aset, bcmFieldActionStatGroup);
    BCM_FIELD_ASET_ADD(group_config_pass_3.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config_pass_3.aset, bcmFieldActionRedirectPort);

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config_pass_3));

    /* For second trunk group member */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config_pass_3.group, &entry_pass_3));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_IfaVersion2(unit, entry_pass_3, 1, -1));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_PktType(unit, entry_pass_3, bcmFieldPktTypeUdpIfa));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstPort(unit, entry_pass_3, 0,-1,8,-1));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_L4SrcPort(unit, entry_pass_3, 0, 3));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_3,bcmFieldActionRedirectPort, 0,port_network_1));


    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_3, bcmFieldActionAssignExtractionCtrlId, 0xee, 0xff));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_3, bcmFieldActionAssignEditCtrlId, 0xe0, 0xff));
    //BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_3, bcmFieldActionCopyToCpu, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry_pass_3));


    /* For second trunk group member */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config_pass_3.group, &entry_pass_3));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_IfaVersion2(unit, entry_pass_3, 1, -1));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_PktType(unit, entry_pass_3, bcmFieldPktTypeUdpIfa));


    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstPort(unit, entry_pass_3, 0,-1,8,-1));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_L4SrcPort(unit, entry_pass_3, 1, 3));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_3, bcmFieldActionRedirectPort, 0, port_network_2));


    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_3, bcmFieldActionAssignExtractionCtrlId, 0xee, 0xff)); /* Redirect to loopback port */
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_3, bcmFieldActionAssignEditCtrlId, 0xe0, 0xff));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry_pass_3));

    /* For third trunk group member */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config_pass_3.group, &entry_pass_3));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_IfaVersion2(unit, entry_pass_3, 1, -1));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_PktType(unit, entry_pass_3, bcmFieldPktTypeUdpIfa));


    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstPort(unit, entry_pass_3, 0,-1,8,-1));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_L4SrcPort(unit, entry_pass_3, 2, 3));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_3, bcmFieldActionRedirectPort, 0, port_network_3));


    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_3, bcmFieldActionAssignExtractionCtrlId, 0xee, 0xff)); /* Redirect to loopback port */
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_3, bcmFieldActionAssignEditCtrlId, 0xe0, 0xff));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry_pass_3));

    /* For fourth trunk group member */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config_pass_3.group, &entry_pass_3));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_IfaVersion2(unit, entry_pass_3, 1, -1));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_PktType(unit, entry_pass_3, bcmFieldPktTypeUdpIfa));


    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstPort(unit, entry_pass_3, 0,-1,8,-1));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_L4SrcPort(unit, entry_pass_3, 3, 3));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_3, bcmFieldActionRedirectPort, 0, port_network_4));


    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_3, bcmFieldActionAssignExtractionCtrlId, 0xee, 0xff)); /* Redirect to loopback port */
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_3, bcmFieldActionAssignEditCtrlId, 0xe0, 0xff));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry_pass_3));

    return BCM_E_NONE;
}

/* IFA first + second pass EVXLT configuration */

int
ifa_evxlt(int unit)
{
    /* Disabling vlan checks for looped back packet */
    uint32 flow_handle_ifa = 0;
    bcm_flow_option_id_t flow_option_ifa = 0;
    bcm_flow_encap_config_t einfo;

    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IFA", &flow_handle_ifa));
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle_ifa, "LOOKUP_LOOPBACK_SUBTYPE_ASSIGN_DISABLE_VLAN_CHECK_ACTION", &flow_option_ifa));

    bcm_flow_match_config_t minfo;
    bcm_flow_match_config_t_init(&minfo);
    minfo.flow_handle = flow_handle_ifa;
    minfo.flow_option = flow_option_ifa;
    minfo.criteria = BCM_FLOW_MATCH_CRITERIA_FLEX;
    BCM_IF_ERROR_RETURN(bcm_flow_match_add(unit, &minfo, 0, NULL));

    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle_ifa, "LOOKUP_LOOPBACK_HDR_DGPP_ASSIGN_LOCAL_DGPP", &flow_option_ifa));

    bcm_flow_field_id_t field_id = 0;
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(0, flow_handle_ifa, "IFA_LOOPBACK_DEST", &field_id));

    /* Configuring IFA hdr and MD hdr [second pass] on EVXLT2 table */
    uint32 flow_handle_hdr_md = 0;
    bcm_flow_option_id_t encapA_flow_option= 0, encapB_flow_option = 0;

    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IFA", &flow_handle_hdr_md));
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle_hdr_md, "IFA_BASE_MD_HEADER_ASSIGN_LOOKUP", &encapA_flow_option));

    bcm_flow_encap_config_t_init(&einfo);
    bcm_flow_logical_field_t lfield[3];

    /* Insert BASE_HEADER, METADATA_HEADER */
    einfo.flow_handle = flow_handle_hdr_md;
    einfo.flow_option = encapA_flow_option;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;

    /* IFA header */
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "DIRECT_ASSIGNMENT_10_ACTION_SET", &lfield[0].id));
    lfield[0].value = 0x2f110228;

    /* IFA MD header */
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "DIRECT_ASSIGNMENT_11_ACTION_SET", &lfield[1].id));
    lfield[1].value = 0x33229a00;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "OPAQUE4", &lfield[2].id));
    lfield[2].value = opaque_val;

    einfo.valid_elements = BCM_FLOW_ENCAP_FLEX_DATA_VALID | BCM_FLOW_ENCAP_FLEX_KEY_VALID;

    BCM_IF_ERROR_RETURN(bcm_flow_encap_add(unit, &einfo, 3, lfield));


    /*
     * Configuring Metadata [third pass] info on EVXLT1 table
     * DGPP is used to lookup EVXLT1
     */
    /* Adapt_1 single Lookup 1 for network_port_1 */
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle_hdr_md, "IFA_MD_LOOKUP_1_ADAPT1_SINGLE", &encapB_flow_option));

    /* Insert METADATA - gport_network_1 */
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle_hdr_md;
    einfo.flow_option = encapB_flow_option;
    einfo.flow_port = gport_network_1;

    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "OPAQUE_DATA", &lfield[0].id));
    lfield[0].value = 0xaaaaa;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "LNS", &lfield[1].id));
    lfield[1].value = 1;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "DEVICE_ID", &lfield[2].id));
    lfield[2].value = 0xDDDDD;

    einfo.valid_elements = BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_FLEX_DATA_VALID | BCM_FLOW_ENCAP_FLEX_KEY_VALID;

    BCM_IF_ERROR_RETURN(bcm_flow_encap_add(unit, &einfo, 3, lfield));

    /* Configure Adapt1 single Lookup2 */
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle_hdr_md, "IFA_MD_LOOKUP_2_ADAPT1_SINGLE", &encapB_flow_option));

    /* Insert METADATA */
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle_hdr_md;
    einfo.flow_option = encapB_flow_option;
    einfo.flow_port = gport_network_1;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "MMU_STAT_0", &lfield[0].id));
    lfield[0].value = 0x900ddeed;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "MMU_STAT_1", &lfield[1].id));
    lfield[1].value = 0x900dfeed;

    einfo.valid_elements = BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_FLEX_DATA_VALID | BCM_FLOW_ENCAP_FLEX_KEY_VALID;

    BCM_IF_ERROR_RETURN(bcm_flow_encap_add(unit, &einfo, 2, lfield));

    /* Adapt_1 single Lookup 1 for network_port_2 */
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle_hdr_md, "IFA_MD_LOOKUP_1_ADAPT1_SINGLE", &encapB_flow_option));

    /* Insert METADATA - gport_network_1 */
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle_hdr_md;
    einfo.flow_option = encapB_flow_option;
    einfo.flow_port = gport_network_2;

    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "OPAQUE_DATA", &lfield[0].id));
    lfield[0].value = 0xaaaaa;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "LNS", &lfield[1].id));
    lfield[1].value = 1;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "DEVICE_ID", &lfield[2].id));
    lfield[2].value = 0xDDDDD;

    einfo.valid_elements = BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_FLEX_DATA_VALID | BCM_FLOW_ENCAP_FLEX_KEY_VALID;

    BCM_IF_ERROR_RETURN(bcm_flow_encap_add(unit, &einfo, 3, lfield));

    /* Configure Adapt1 single Lookup2 */
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle_hdr_md, "IFA_MD_LOOKUP_2_ADAPT1_SINGLE", &encapB_flow_option));

    /* Insert METADATA */
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle_hdr_md;
    einfo.flow_option = encapB_flow_option;
    einfo.flow_port = gport_network_2;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "MMU_STAT_0", &lfield[0].id));
    lfield[0].value = 0x900ddeed;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "MMU_STAT_1", &lfield[1].id));
    lfield[1].value = 0x900dfeed;

    einfo.valid_elements = BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_FLEX_DATA_VALID | BCM_FLOW_ENCAP_FLEX_KEY_VALID;

    BCM_IF_ERROR_RETURN(bcm_flow_encap_add(unit, &einfo, 2, lfield));

    return BCM_E_NONE;
}

/* Timing configuration for timestamps */

int
time_config(int unit)
{
    bcm_time_init(unit);
    bcm_time_interface_t intf;
    bcm_error_t rv;

    bcm_time_interface_t_init(&intf);
    intf.id = 0;
    intf.flags |= BCM_TIME_ENABLE;

    rv = bcm_time_interface_add(unit, intf);
    if (BCM_FAILURE(rv)) {
        printf("Error in time interface add %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_time_ts_counter_t counter;
    bcm_time_tod_t tod;
    uint64 secs, nano_secs, div;
    uint32 stages;

    bcm_time_ts_counter_get(unit, &counter);
    stages = BCM_TIME_STAGE_EGRESS;
    tod.time_format = counter.time_format;
    tod.ts_adjustment_counter_ns = counter.ts_counter_ns;

    /* Get seconds from the timestamp value */
    secs = counter.ts_counter_ns;
    COMPILER_64_SET(div, 0, 1000000000);
    COMPILER_64_UDIV_64(secs, div);
    tod.ts.seconds = secs;

    /* Get nanoseconds from the timestamp value */
    nano_secs = counter.ts_counter_ns;
    COMPILER_64_UMUL_32(secs, 1000000000);
    COMPILER_64_SUB_64(nano_secs, secs);
    tod.ts.nanoseconds = COMPILER_64_LO(nano_secs);

    rv = bcm_time_tod_set(unit, stages, &tod);
    if (BCM_FAILURE(rv)) {
        printf("Error in tod set %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 *    Configures the feature and the required set up to verify it
 */

int test_setup(int unit)
{
    bcm_gport_t gport0, gport1;
    bcm_module_t module = 0;
    bcm_error_t rv;
    int port_list[5];
    int lb_mode = BCM_PORT_LOOPBACK_MAC;

    rv = portNumbersGet(unit, port_list, 5);
    if (BCM_FAILURE(rv)) {
        printf("Error in portNumbersGet %s\n", bcm_errmsg(rv));
        return rv;
    }

    port_acc = port_list[0];
    port_network_1 = port_list[1];
    port_network_2 = port_list[2];
    port_network_3 = port_list[3];
    port_network_4 = port_list[4];
    printf("Choosing port %d as ingress port, %d as egress port1 and %d as egress port2\n", port_acc, port_network_1, port_network_2);

    /* Set port mac loopback mode */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_acc, lb_mode));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_network_1, lb_mode));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_network_2, lb_mode));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_network_3, lb_mode));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_network_4, lb_mode));

    BCM_GPORT_MODPORT_SET(gport0, module, port_acc);

    rv = ifa_global_setting(unit, gport0);
    if (BCM_FAILURE(rv)) {
        printf("Error in IFA global settings %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vxlan_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in vxlan config %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv =trunk_rtag7_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in rtag7 config %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = mirror_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in mirror configuration %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifa_first_pass(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in IFA first pass configuration %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifa_second_pass(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in IFA second pass configuration %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifa_third_pass(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in IFA second pass configuration %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifa_evxlt(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in IFA EVXLT configuration %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = time_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in time config %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 *    Transmit the below packet on port_acc and verify the functionality of
 *    the feature
 */
int test_verify(int unit)
{
    char str[512];

    snprintf(str, 512, "port %d discard=all", port_network_1);
    bshell(unit, str);
    snprintf(str, 512, "port %d discard=all", port_network_2);
    bshell(unit, str);
    snprintf(str, 512, "port %d discard=all", port_network_3);
    bshell(unit, str);
    snprintf(str, 512, "port %d discard=all", port_network_4);
    bshell(unit, str);
    snprintf(str, 512, "dmirror %d mode=ingress destport=cpu0", port_acc);
    bshell(unit, str);
    snprintf(str, 512, "dmirror %d mode=ingress destport=cpu0", port_network_1);
    bshell(unit, str);
    snprintf(str, 512, "dmirror %d mode=ingress destport=cpu0", port_network_2);
    bshell(unit, str);
    snprintf(str, 512, "dmirror %d mode=ingress destport=cpu0", port_network_3);
    bshell(unit, str);
    snprintf(str, 512, "dmirror %d mode=ingress destport=cpu0", port_network_4);
    bshell(unit, str);
    bshell(unit,"pw start");
    bshell(unit,"pw report +raw");

    /* Send a packet to ingress port*/
    printf("Packet comes into ingress port %d\n", port_acc);
    snprintf(str, 512, "tx 1 pbm=%d  data=00000000aaaa00000000d1c1810000150800453a006e000000003f1164310a0a01010a0a0200003e1111005a0000000000000001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132330001259ffb7af83b;sleep 5", port_acc);
    printf("%s\n", str);
    bshell(unit, str);

    printf("Packet comes into ingress port %d\n", port_acc);
    snprintf(str, 512, "tx 1 pbm=%d  data=00000000aaaa00000000c1c1810000150800453a006e000000003f1164310a0a01010a0a0200003e1111005a0000000000000001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132330001259ffb7af83b;sleep 5", port_acc);
    printf("%s\n", str);
    bshell(unit, str);

    snprintf(str, 512, "tx 1 pbm=%d  data=00000000aaaa00000000bebe810000150800453a006e000000003f1164310a0a01010a0a0200003e1111005a0000000000000001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132330001259ffb7af83b;sleep 5", port_acc);
    printf("%s\n", str);
    bshell(unit, str);

    snprintf(str, 512, "tx 1 pbm=%d  data=00000000aaaa00000000cebe810000150800453a006e000000003f1164310a0a01010a0a0200003e1111005a0000000000000001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132330001259ffb7af83b;sleep 5", port_acc);
    printf("%s\n", str);
    bshell(unit, str);

    return BCM_E_NONE;
}

/*
 *    This functions does the following
 *    a)test setup [configures the feature]
 *    b)test verify [verifies the configured feature]
 */
int test_execute()
{
    bcm_error_t rv;
    int unit = 0;

    /*bshell(unit, "config show; a ; version");*/

    rv = test_setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("testSetup() failed, %s.\n", bcm_errmsg(rv));
        return -1;
    }
    printf("Completed test setup successfully.\n");

    rv = test_verify(unit);
    if (BCM_FAILURE(rv)) {
        printf("testVerify() failed\n");
        return -1;
    }
    printf("Completed test verify successfully.\n");

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print test_execute();
}



/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * File: cint_pon_l3_anti_spoofing.c
 *
 * Purpose: An example of how to use BCM APIs to implement L3 anti-spoofing(L3 Source Binding) function in pon appplication service.
 *          After enable anti-spoofing function, system will permit/deny traffic pass base source bind information 
 *          of InLifs and SIPs, denied packets which are not matched.
 *
 *          Here is the binding information, and the mode is decided by IN-lIF bind configuration at  
 *          |----------------|---------------------------------------------------------------|
 *          |     Mode       |     binding configuration                                     |
 *          |----------------|---------------------------------------------------------------|
 *          | IPV4           | SIP, InLif and flag isn't BCM_L3_SOURCE_BIND_IP6              |
 *          |                | and custom_feature_l3_source_bind_anti_spoofing_ipv4=1        |
 *          |----------------|---------------------------------------------------------------|
 *          | IPV6           | SIP, InLif and flag isn't BCM_L3_SOURCE_BIND_IP4              |
 *          |                | and custom_feature_l3_source_bind_anti_spoofing_ipv6=1        |
 *          |----------------|---------------------------------------------------------------|
 * Calling sequence:
 *
 * Initialization:
 *  1. Set the following port configureations to config-sand.bcm
 *        ucode_port_128.BCM88650=10GBase-R15
 *        ucode_port_4.BCM88650=10GBase-R14
 *  2. Add the following PON application and anti-spoofing enabling configureations to config-sand.bcm
 *        pon_application_support_enabled_2=TRUE
 *        l3_source_bind_mode=IP
 *        custom_feature_l3_source_bind_anti_spoofing_ipv4=1
 *        custom_feature_l3_source_bind_anti_spoofing_ipv6=1
 *  3. Enable pon service:
 *        - call pon_l3_anti_spoofing_app()
 *  4. Set IPV4, IPV6 source bind configuration, details:
 *        IPV4 :   SIP-10.10.10.10, InLif
 *        IPV6 :   SIP-2000::3, InLif
 *        - call pon_anti_spoofing_enable()
 *  5. Enable FP to drop un-matched traffic.
 *        - call pon_l3_anti_spoofing_drop()
 *  6. Clean all above configuration.
 *        - call pon_anti_spoofing_app_clean()
 *
 * To Activate Above Settings:
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/pon/cint_pon_utils.c
 *      BCM> cint examples/dpp/pon/cint_pon_anti_spoofing.c
 *      BCM> cint examples/dpp/pon/cint_pon_l3_anti_spoofing.c
 *      BCM> cint
 *      cint> pon_l3_anti_spoofing_app(unit, pon_port, nni_port);
 *      cint> pon_anti_spoofing_enable(unit);
 *      cint> pon_l3_anti_spoofing_drop(unit);
 *
 * To clean all above configuration:
 *      cint> pon_anti_spoofing_app_clean(unit);
 */
 

/*********************************************************************************
* Setup pon anti-spoofing test model(1:1)
*     PON Port 2  <------------------------------->  VRF  <---> NNI Port 200
*     Pon 2 Tunnel-ID 100 clvan 10 ---------------|  15  |----------- SVLAN 10
*     add source binding info:
*         InLif SIPv4
*         InLif SIPv6 by bcm_l3_source_bind.
*     add two entries of route:
*         IPv4 host route vrf 143.255.255.2 -> NNI port
*         IPv6 host route vrf 2000::4 -> NNI port
* steps
 */
int pon_l3_anti_spoofing_app(int unit, bcm_port_t pon, bcm_port_t nni)
{
    int i = 0;
    int rv = 0;
    bcm_if_t encap_id = 0;
    int action_id = 0;
    create_l3_intf_s in_intf;
    create_l3_intf_s out_intf;
    int vrf = 0xf;
    int fec = 0;
    bcm_failover_t failover_id_fec;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x11, 0x00, 0x00, 0x00, 0x11 };     /* my-MAC - trerminating L2 UNI->NNI */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x11, 0x00, 0x00, 0x00, 0x22 };    /* my-MAC - trerminating L2 NNI->UNI */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac - DA from UNI->NNI */
    bcm_mac_t arp_next_hop_mac_in = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x2d };     /* next_hop_mac - DA from NNI->UNI */
    uint32 route = 0x7fffff00;         /* PON->NNI subnet route */
    uint32 host = 0x7fffff02;          /* PON->NNI host DIP */
    uint32 mask = 0xfffffff0;
    uint32 route_nni2uni = 0x8fffff00;
    uint32 host_nni2uni = 0x8fffff02;  /* NNI->PON host DIP */
    uint32 mask_nni2uni = 0xfffffff0;
    /* IPv6 */
    bcm_ip6_t route_ip6 =
        { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04 };
    bcm_ip6_t route_sip6 =
        { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03 };
    bcm_ip6_t mask_sip6 = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 };
    bcm_ip6_t mask_ip6 = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0 };
    bcm_ip6_t route_ip6_nni2uni =
        { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04 };
    bcm_ip6_t mask_ip6_nni2uni = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0 };

    pon_anti_spoofing_init(unit, pon, nni);

    pon_anti_spoofing_info.vsi = 0x500; 
    rv = vswitch_create_with_id(unit, pon_anti_spoofing_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create PON LIF */
    rv = pon_lif_create(unit, 
                        pon_anti_spoofing_info.pon_port, 
                        match_otag,
                        0, 0,
                        pon_anti_spoofing_info.tunnel_id,
                        pon_anti_spoofing_info.tunnel_id,
                        pon_anti_spoofing_info.pon_cvlan,
                        0, 0, 0,
                        &(pon_anti_spoofing_info.pon_gport));
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_lif_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add PON LIF to vswitch */
    rv = bcm_vswitch_port_add(unit, 
                              pon_anti_spoofing_info.vsi, 
                              pon_anti_spoofing_info.pon_gport
                             );
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add returned %s\n", bcm_errmsg(rv));
        return rv;
    }
    /*For downstream*/
    rv = multicast_vlan_port_add(unit, 
                                 pon_anti_spoofing_info.vsi+pon_anti_spoofing_info.lif_offset, 
                                 pon_anti_spoofing_info.pon_port, 
                                 pon_anti_spoofing_info.pon_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multicast_vlan_port_add returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set PON LIF ingress VLAN editor */
    rv = pon_port_ingress_vt_set(unit,
                                 otag_to_o_i_tag,
                                 pon_anti_spoofing_info.nni_svlan,
                                 pon_anti_spoofing_info.pon_cvlan,
                                 pon_anti_spoofing_info.pon_gport,
                                 0, &action_id
                                 );
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_port_ingress_vt_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }                                             
    /* Set PON LIF egress VLAN editor */
    rv = pon_port_egress_vt_set(unit,
                                otag_to_o_i_tag,
                                pon_anti_spoofing_info.tunnel_id,
                                pon_anti_spoofing_info.pon_cvlan,
                                0, 
                                pon_anti_spoofing_info.pon_gport
                               );
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_port_egress_vt_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }
    /*
     * Create PON ETH-RIF and set its properties 
     */
    in_intf.vsi = pon_anti_spoofing_info.vsi;
    in_intf.my_global_mac = intf_in_mac_address;
    in_intf.my_lsb_mac = intf_in_mac_address;
    in_intf.urpf_mode = bcmL3IngressUrpfDisable;
    in_intf.vrf = vrf;
    in_intf.vrf_valid = 1;
    rv =  l3__intf_rif__create(unit, &in_intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    create_l3_egress_s l3_eg;
    sal_memcpy(l3_eg.next_hop_mac_addr, arp_next_hop_mac, 6);
    l3_eg.out_tunnel_or_rif = pon_anti_spoofing_info.tunnel_id; 
    l3_eg.vlan   = pon_anti_spoofing_info.pon_cvlan;
    l3_eg.out_gport   = pon_anti_spoofing_info.pon_gport;
    rv = l3__egress__create(unit,&l3_eg);
    if (rv != BCM_E_NONE) {
        printf ("l3__egress__create failed: %d \n", rc);
    }
    fec = l3_eg.fec_id;

    rv = add_route_ipv4(unit, route, mask, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv4, \n");
        return rv;
    }

    rv = add_host_ipv4(unit, host, vrf, fec, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4\n");
        return rv;
    }
    /* IPv6 */
    /* Add IPv6 route entry - PON-->NNI */
    rv = add_route_ipv6(unit, route_ip6, mask_ip6, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv4, \n");
        return rv;
    }

    /* Create NNI LIF */
    rv = nni_lif_create(unit,
                       pon_anti_spoofing_info.nni_port,
                       match_o_i_tag,
                       0,
                       pon_anti_spoofing_info.nni_svlan,
                       pon_anti_spoofing_info.pon_cvlan,
                       0,
                       &(pon_anti_spoofing_info.nni_gport),
                       &encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, nni_lif_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    if (soc_property_get(unit , "local_switching_enable",0))
    {
        rv = bcm_port_control_set(unit, pon_anti_spoofing_info.nni_gport, bcmPortControlLocalSwitching, 1);
        if (rv != BCM_E_NONE)
        {
            printf("Error, nni_lif_create returned %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /*For upstream*/
    rv = multicast_vlan_port_add(unit, 
                              pon_anti_spoofing_info.vsi,
                              pon_anti_spoofing_info.nni_port,
                              pon_anti_spoofing_info.nni_gport);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, multicast_vlan_port_add returned %s nni_gport\n", bcm_errmsg(rv));
        return rv;
    }

    /* add to vswitch */
    rv = bcm_vswitch_port_add(unit,
                           pon_anti_spoofing_info.vsi,
                           pon_anti_spoofing_info.nni_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add returned %s nni_gport\n", bcm_errmsg(rv));
        return rv;
    }

    out_intf.vsi = pon_anti_spoofing_info.nni_svlan;
    out_intf.my_global_mac = intf_out_mac_address;
    out_intf.my_lsb_mac = intf_out_mac_address;
    out_intf.urpf_mode = bcmL3IngressUrpfDisable;
    out_intf.vrf = vrf;
    out_intf.vrf_valid = 1;
    rv =  l3__intf_rif__create(unit, &out_intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    if (!is_device_or_above(unit,JERICHO_B0)) {
        /* create failover id*/
        rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_id_fec);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_failover_create 1\n");
            return rv;
        }
     /*   printf("Failover id: 0x%x\n", failover_id_fec);*/
    }
    create_l3_egress_s l3_eg_in;
    sal_memcpy(l3_eg_in.next_hop_mac_addr, arp_next_hop_mac_in, 6);
    l3_eg_in.out_tunnel_or_rif = pon_anti_spoofing_info.pon_cvlan; 
    l3_eg_in.vlan   = pon_anti_spoofing_info.pon_cvlan;
    l3_eg_in.out_gport = nni;
    l3_eg_in.failover_id = failover_id_fec;
    l3_eg_in.failover_if_id = 0;

    if (!is_device_or_above(unit,JERICHO_B0)) {
        /* Create protected FEC */
        rv = l3__egress__create(unit, &l3_eg_in);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
          return rv;
        }
        l3_eg_in.failover_if_id = l3_eg_in.fec_id;
        l3_eg_in.arp_encap_id = 0;
    }
    
    /* primary FEC */
    rv = l3__egress__create(unit, &l3_eg_in);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec = l3_eg_in.fec_id;

    rv = add_route_ipv4(unit, route_nni2uni, mask_nni2uni, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv4, \n");
        return rv;
    }

    rv = add_host_ipv4(unit, host_nni2uni, vrf, fec, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4, \n");
        return rv;
    }

    /* Add IPv6 route entry - NNI-->UNI */
    rv = add_route_ipv6(unit, route_ip6_nni2uni, mask_ip6_nni2uni, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route_ipv4, \n");
        return rv;
    }

    /* Set the multicast group offset to flood downstream packets in multicast group(vsi+lif_offset) */
    bcm_port_control_set(unit, pon_anti_spoofing_info.nni_gport, bcmPortControlFloodUnknownUcastGroup, pon_anti_spoofing_info.lif_offset);
    bcm_port_control_set(unit, pon_anti_spoofing_info.nni_gport, bcmPortControlFloodUnknownMcastGroup, pon_anti_spoofing_info.lif_offset);
    bcm_port_control_set(unit, pon_anti_spoofing_info.nni_gport, bcmPortControlFloodBroadcastGroup, pon_anti_spoofing_info.lif_offset);

    /* Will set 4 bind configuration in this PON-LIF gport */
    for (i = 0; i < 4; i++)
    {
        src_bind[i].port = pon_anti_spoofing_info.pon_gport;
    }
    bind_entries_num = 4;

    return rv;
}

/*********************************************************************************
* Create a field group to drop un-matched traffic.
*
* Steps
*     1. Create a field group
*     2. Attach a action set to field group
*     3. Add entries to drop/pass traffic
 */
int pon_l3_anti_spoofing_drop(int unit)
{
    bcm_field_aset_t aset;
    bcm_field_entry_t *ent[4];
    bcm_field_group_config_t *grp;
    bcm_field_AppType_t app_type[4] = {bcmFieldAppTypeIp4UcastRpf, bcmFieldAppTypeIp6UcastRpf,
        bcmFieldAppTypeIp4UcastRpf, bcmFieldAppTypeIp6UcastRpf};
    int group_priority = 10;
    int result;
    int auxRes;
    int i = 0;

    grp = &(pon_anti_spoofing_info.grp);
    for (i = 0; i < 4; i++)
    {
        ent[i] = &(pon_anti_spoofing_info.ent[i]);
    }

    bcm_field_group_config_t_init(grp);
    grp->group = -1;

    /* Define the QSET
     *  bcmFieldQualifyInPort: Single Input Port
     *  bcmFieldQualifyAppType: Application type, to see if L3 source bind lookup is done
     *  bcmFieldQualifyL3SrcRouteHit: L3 source lookup success(LPM look up result)
     *  bcmFieldQualifyStageIngress: Field Processor pipeline ingress stage
     */
    BCM_FIELD_QSET_INIT(grp->qset);
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyAppType);
    if (is_device_or_above(unit, JERICHO))
    {
        BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyL3SrcRouteHit);
    }
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyStageIngress);

    /*  Create the Field group */
    grp->priority = group_priority;
    result = bcm_field_group_config_create(unit, grp);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_create returned %s\n", bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    /*
     *  Define the ASET Field Group
     */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

    /*  Attach the action set */
    result = bcm_field_group_action_set(unit, grp->group, aset);
    if (BCM_E_NONE != result) 
    {
        printf("Error in bcm_field_group_action_set returned %s\n", bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    /*
     *  Add 4 entries to the Field group.
     *  Match the packets on the l3 source bind is done and look up result.
     *    1. Lookup is done for DHCPv4 and LPM(JERICHO) look up result is not-found:
     *       drop
     *    2. Lookup is done for DHCPv6 and LPM(JERICHO) look up result is not-found:
     *       drop
     *    3. lookup is done for Staticv4 and LPM(JERICHO) look up result is not-found:
     *       drop
     *    4. lookup is done for Staticv6 and LPM(JERICHO) lookup result is not-found:
     *       drop
     */
    for (i = 0 ; i < 4; i++)
    {
        result = bcm_field_entry_create(unit, grp->group, ent[i]);
        if (BCM_E_NONE != result)
        {
            printf("Error %d in bcm_field_entry_create returned %s\n", i, bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
    }

    /* IPV4/6 DHCP */
    for (i = 0 ; i < 2; i++)
    {
        result = bcm_field_qualify_AppType(unit, *(ent[i]), app_type[i]);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[0] in bcm_field_qualify_AppType returned %s\n", bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
        result = bcm_field_qualify_L3SrcRouteHit(unit, *(ent[i]), 0x0, 0x01);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[0] in bcm_field_qualify_L2DestHit returned %s\n", bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
        result = bcm_field_action_add(unit, *(ent[i]), bcmFieldActionDrop, 0, 0);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[2] in bcm_field_action_add returned %s\n", bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }

        result = bcm_field_entry_prio_set(unit, *(ent[i]), (10 - i));
        if (result) {
            printf("bcm_field_entry_prio_set *(ent[0]) failure\n");
            return result;
        }
    }

    /* IPV4 STATIC */
    i=2;
    result = bcm_field_qualify_AppType(unit, *(ent[i]), app_type[i]);
    if (BCM_E_NONE != result)
    {
        printf("Error ent[%d] in bcm_field_qualify_AppType returned %s\n", i, bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }
    if (is_device_or_above(unit, JERICHO))
    {
        result = bcm_field_qualify_L3SrcRouteHit(unit, *(ent[i]), 0x0, 0x01);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[%d] in bcm_field_qualify_L3SrcRouteHit returned %s\n", i, bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
    }
    result = bcm_field_action_add(unit, *(ent[i]), bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result)
    {
        printf("Error ent[%d] in bcm_field_action_add returned %s\n", i, bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    result = bcm_field_entry_prio_set(unit, *(ent[i]), (10-i));
    if (result) {
        printf("bcm_field_entry_prio_set *(ent[%d]) failure\n", i);
        return result;
    }

    /* IPV6 STATIC */
    i = 3;
    result = bcm_field_qualify_AppType(unit, *(ent[i]), app_type[i]);
    if (BCM_E_NONE != result)
    {
        printf("Error ent[%d] in bcm_field_qualify_AppType returned %s\n", i, bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }
    if (is_device_or_above(unit, JERICHO))
    {
        result = bcm_field_qualify_L3SrcRouteHit(unit, *(ent[i]), 0x0, 0x01);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[%d] in bcm_field_qualify_L3SrcRouteHit returned %s\n", i, bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
    }
    result = bcm_field_action_add(unit, *(ent[i]), bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result)
    {
        printf("Error ent[%d] in bcm_field_action_add returned %s\n", i, bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    result = bcm_field_entry_prio_set(unit, *(ent[i]), (10-i));
    if (result) {
        printf("bcm_field_entry_prio_set *(ent[%d]) failure\n", i);
        return result;
    }

    /* install */
    result = bcm_field_group_install(unit, grp->group);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_install returned %s\n", bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    return result;
}


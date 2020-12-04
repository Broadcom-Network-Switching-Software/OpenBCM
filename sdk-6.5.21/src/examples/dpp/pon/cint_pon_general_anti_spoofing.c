/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * File: cint_pon_general_anti_spoofing.c
 *
 * Purpose: An example of how to use BCM APIs to implement general anti-spoofing(host & subnet anti-spoofing) function in pon MP and P2P application.
 *              After enable anti-spoofing function, system will permit/deny traffic pass base source bind information  of pon gport. 
 *              SAs of denied packets are not leant.
 *
 *          Here is the binding information, and the mode is decided by IN-lIF bind configuration at  
 *          |-----------------------|---------------------------------------------------------------|
 *          |     Mode                              |     binding configuration                                                                               |
 *          |-----------------------|---------------------------------------------------------------|
 *          | IPV4(static)  ARP REQ up/down      | SIP and subnet and flag isn't BCM_L3_SOURCE_BIND_IP6                   |
 *          |-----------------------|---------------------------------------------------------------|
 *          | IPV4 DHCP                         | SIP, SA and flag isn't BCM_L3_SOURCE_BIND_IP6                                         |
 *          |-----------------------|---------------------------------------------------------------|
 *          | IPV6  static                        | SIPV6 and subnet and flag is BCM_L3_SOURCE_BIND_IP6                              |
 *          |-----------------------|---------------------------------------------------------------|
 *          | IPV6 DHCP                         | SIPV6, SA and flag is BCM_L3_SOURCE_BIND_IP6                  |
 *          |-----------------------|---------------------------------------------------------------|
 *
 * Calling sequence:
 *
 * Initialization:
 *  1. Set the following port configureations to config-sand.bcm
 *        ucode_port_128.BCM88650=10GBase-R13
 *        ucode_port_3.BCM88650=10GBase-R12
 *  2. Add the following PON application and anti-spoofing enabling configureations to config-sand.bcm
 *        pon_application_support_enabled_3.BCM88650=TRUE
 *        l3_source_bind_mode=IP
 *        l3_source_bind_subnet_mode=IP
 *        custom_feature_l3_source_bind_arp_relay=3
 *  3. Configure number of Virtual Routing and Forwarding ID for the devic
 *        ipv4_num_vrfs = 4096
 *  4. Enable pon service:PON port will recognize tunnel Id(100) CTAG(10),NNI port will recognize STAG(100), CTAG(10).
 *        - call pon_anti_spoofing_app()
 *  5. Set IPV4, IPV6 source bind configuration, details:
 *        IPV4 static host:   SIP-10.10.10.10
 *        IPV4 static subnet: SIP-11.10.10.10/24
 *        IPV4 DHCP: SIP-10.10.10.11, SA-00:00:01:00:00:01
 *        IPV6 static host:   SIP-2000::2
 *        IPV6 static subnet: SIP-3000::2/64
 *        IPV6 DHCP: SIP-5000::5, SA-00:00:03:00:00:01
 *        - call pon_anti_spoofing_enable()
 *  6. Enable FP to drop un-matched traffic.
 *        - call pon_anti_spoofing_compare_fg_set() to drop IPV4 static and ARP REQ un-matched traffic in PON port
 *        - call pon_anti_spoofing_arp_drop() to drop ARP REQ un-matched traffic in NNI port
 *        - call pon_anti_spoofing_drop() to drop IPV4 DHCP, IPV6 static and IPV6 DHCP  un-matched traffic in PON port
 *  7. Clean all above configuration.
 *        - call pon_anti_spoofing_compare_fg_destroy()
 *        - call pon_anti_spoofing_arp_drop_destory()
 *        - call pon_anti_spoofing_drop_destory()
 *        - call pon_anti_spoofing_app_clean()
 *  8. Traffic and expect behavior:
 *       VLAN tag: outer vid-100 and inner-vid 10
 *      1) PON port, ethernet(Not IP and ARP) packet, SA-any, passed .
 *      2) PON port, ARP reply packet, SA-any, passed.
 *      3) PON port, IPv4 packet, SIP-10.10.10.10, SA-any, passed.
 *      4) PON port, IPv4 packet, SIP-11.10.10.10, SA-any, passed.
 *      5) PON port, IPv4 packet, SIP-10.10.10.11, SA-00:00:01:00:00:01, passed.
 *      6) PON port, IPv4 packet, SIP-10.10.10.9, SA-any, dropped
 *      7) PON port, IPv4 packet, SIP-10.10.10.11, SA-00:00:01:00:00:01, dropped.
 *      8) PON port, ARP REQ packet, ARP.SPA-10.10.10.10, passed.
 *      9) PON port, ARP REQ packet, ARP.SPA-11.10.10.10, passed.
 *      10) PON port, ARP REQ packet, ARP.SPA-9.9.9.9, dropped
 *      11) PON port, IPv6 packet, SIP-2000::2, SA-any, passed.
 *      12) PON port, IPv6 packet, SIP-3000::2, SA-any, passed.
 *      13) PON port, IPv6 packet, SIP-5000::5, SA-00:00:03:00:00:01, passed.
 *      14) PON port, IPv6 packet, SIP-4000::9, SA-any, dropped
 *      15) PON port, IPv6 packet, SIP-5000::5, SA-00:00:03:00:00:02, dropped.
 *      ----------------------NNI port------------------------------
 *      16) NNI port, ethernet(Not IP and ARP) packet, SA-any, passed .
 *      17) NNI port, ARP reply packet, SA-any, passed.
 *      18) NNI port, ARP REQ packet, ARP.TPA-10.10.10.10, passed.
 *      19) NNI port, ARP REQ packet, ARP.TPA-11.10.10.10, passed.
 *      20) NNI port, ARP REQ packet, ARP.TPA-9.9.9.9, dropped
 *
 * To Activate Above Settings:
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/pon/cint_pon_utils.c
 *      BCM> cint examples/dpp/pon/cint_pon_general_anti_spoofing.c
 *      BCM> cint
 *   PON N:1(MP)application:
 *      cint> pon_anti_spoofing_app(unit, pon_port, nni_port);
 *      cint> pon_anti_spoofing_enable(unit);
 *      cint> print pon_anti_spoofing_compare_fg_set(unit);
 *      cint> print pon_anti_spoofing_arp_drop(unit);
 *      cint> print pon_anti_spoofing_drop(unit);
 *
 *
 * To clean all above configuration:
 *      cint> pon_anti_spoofing_compare_fg_destroy(unit);
 *      cint> pon_anti_spoofing_arp_drop_destory(unit);
 *      cint> pon_anti_spoofing_drop_destory(unit);
 *      cint> pon_anti_spoofing_app_clean(unit);
 *
 *   PON 1:1(P2P)application:
 *      cint> pon_anti_spoofing_p2p_app(unit, pon_port, nni_port);
 *      cint> pon_anti_spoofing_enable(unit);
 *      cint> print pon_anti_spoofing_compare_fg_set(unit);
 *      cint> print pon_anti_spoofing_arp_drop(unit);
 *      cint> print pon_anti_spoofing_drop(unit);
 *
 *
 * To clean all above configuration:
 *      cint> pon_anti_spoofing_compare_fg_destroy(unit);
 *      cint> pon_anti_spoofing_arp_drop_destory(unit);
 *      cint> pon_anti_spoofing_drop_destory(unit);
 *      cint> pon_anti_spoofing_p2p_app_clean(unit);
 */
 
struct pon_anti_spoof_info_s {
    bcm_vlan_t  vsi;
    bcm_gport_t pon_gport;
    bcm_gport_t nni_gport;
    bcm_port_t pon_port;
    bcm_port_t nni_port;
    int tunnel_id;
    int nni_svlan;
    int pon_cvlan;
    int lif_offset;
    bcm_field_group_config_t grp;
    bcm_field_entry_t ent[4];
};

pon_anti_spoof_info_s pon_anti_spoofing_info;
bcm_l3_source_bind_t src_bind[6];

int arp_check_enable;
char* src_bind_mode;
char* src_bind_subnet_mode;
/* Initialize PON application configurations.
 * Example: pon_anti_spoofing_init(0, 4, 128);
 */
int pon_anti_spoofing_init(int unit, bcm_port_t pon_port, bcm_port_t nni_port)
{
    bcm_ip6_t sip1;
    int i = 0;

    arp_check_enable = soc_property_get(unit,"custom_feature_l3_source_bind_arp_relay", 0);
    src_bind_mode = soc_property_get_str(unit,"l3_source_bind_mode");
    src_bind_subnet_mode = soc_property_get_str(unit,"l3_source_bind_subnet_mode");
     
    sal_memset(&pon_anti_spoofing_info, 0, sizeof(pon_anti_spoofing_info));
    pon_anti_spoofing_info.vsi = 100;
    pon_anti_spoofing_info.pon_port = pon_port;
    pon_anti_spoofing_info.nni_port = nni_port;
    pon_anti_spoofing_info.tunnel_id = 100;
    pon_anti_spoofing_info.nni_svlan = 100;
    pon_anti_spoofing_info.pon_cvlan = 10;

    /* Disable trunk in NNI port */
    pon_app_init(unit, pon_port, nni_port, 0, 0);

    for (i = 0; i < 6; i++) 
    {
        bcm_l3_source_bind_t_init(&(src_bind[i]));
    }

    /* src_bind[0] -- ipv4 static host */
    /* 10.10.10.10 */
    src_bind[0].ip = 0xA0A0A0A;

    /* src_bind[1] -- ipv4 static subnet */
    /* 11.10.10.10/24 */
    src_bind[1].ip = 0x0B0A0A0A;
    src_bind[1].ip_mask = 0xFFFFFF00;/*255.255.255.0*/
    src_bind[1].flags = BCM_L3_SOURCE_BIND_USE_MASK;

    /* src_bind[0] -- ipv4 dhcp */
    /* 10.10.10.11 */
    src_bind[2].ip = 0xA0A0A0B;
    src_bind[2].mac[0] = 0x0;
    src_bind[2].mac[1] = 0x0;
    src_bind[2].mac[2] = 0x1;
    src_bind[2].mac[3] = 0x0;
    src_bind[2].mac[4] = 0x0;
    src_bind[2].mac[5] = 0x1;

    /* src_bind[2] -- ipv6 static host */
    src_bind[3].ip6[15]= 0x02;   
    src_bind[3].ip6[0] = 0x20;
    src_bind[3].flags = BCM_L3_SOURCE_BIND_IP6;

    /* src_bind[3] -- ipv6 static subnet */
    src_bind[4].flags = (BCM_L3_SOURCE_BIND_IP6 | BCM_L3_SOURCE_BIND_USE_MASK);
    src_bind[4].ip6[15]= 0x02;
    src_bind[4].ip6[0] = 0x30;
    src_bind[4].ip6_mask[0] = 0xFF;
    src_bind[4].ip6_mask[1] = 0xFF;
    src_bind[4].ip6_mask[2] = 0xFF;
    src_bind[4].ip6_mask[3] = 0xFF;
    src_bind[4].ip6_mask[4] = 0xFF;
    src_bind[4].ip6_mask[5] = 0xFF;
    src_bind[4].ip6_mask[6] = 0xFF;
    src_bind[4].ip6_mask[7] = 0xFF;

    /* src_bind[5] -- ipv6 dhcp */
    src_bind[5].ip6[15]= 0x05;   
    src_bind[5].ip6[0] = 0x50;
    src_bind[5].mac[0] = 0x0;
    src_bind[5].mac[1] = 0x0;
    src_bind[5].mac[2] = 0x3;
    src_bind[5].mac[3] = 0x0;
    src_bind[5].mac[4] = 0x0;
    src_bind[5].mac[5] = 0x1;
    src_bind[5].flags |= BCM_L3_SOURCE_BIND_IP6;
    
}

/*********************************************************************************
* Setup pon anti-spoofing test model(N:1)
*     PON Port 1 or 7  <------------------------------->  VSI  <---> NNI Port 128
*     Pon 1 Tunnel-ID 100 clvan 10 --------------------|---4096  |----------- SVLAN 100 cvlan 10
*                                                 
* steps
 */
int pon_anti_spoofing_app(int unit, bcm_port_t pon, bcm_port_t nni)
{
    int i = 0;
    int rv = 0;
    bcm_if_t encap_id = 0;
    int action_id = 0;

    pon_anti_spoofing_init(unit, pon, nni);

    rv = bcm_vlan_create(unit,pon_anti_spoofing_info.vsi);
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_vlan_create vid:%d %s\n", pon_anti_spoofing_info.vsi, bcm_errmsg(rv));
       return rv;
    }
    rv = open_ingress_mc_group(unit, pon_anti_spoofing_info.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, open_ingress_mc_group %d %s\n", pon_anti_spoofing_info.mc_group, bcm_errmsg(rv));
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
                                 pon_anti_spoofing_info.vsi, 
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

    /* Set LIF profile by gport, IP anti-spoofing PMF will be hit when input AC's LIF-profile is 1 */
    rv = bcm_port_class_set(unit, pon_anti_spoofing_info.pon_gport, bcmPortClassFieldIngress, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set returned bcmPortClassFieldIngress %s gport:%08x\n", bcm_errmsg(rv), pon_anti_spoofing_info.pon_gport);
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

    /* Will set 4 bind configuration in this PON-LIF gport */
    for (i = 0; i < 6; i++)
    {
        src_bind[i].port = pon_anti_spoofing_info.pon_gport;
    }

    return rv;
}

/*********************************************************************************
* Setup pon anti-spoofing test model(1:1)
*     PON Port 1 or 7  <----------------> cross connect  <--------> NNI Port 128
*     Pon 1 Tunnel-ID 100 clvan 10 --------------------|---cross connect gport |----------- SVLAN 100 cvlan 10
*                                                 
* steps
 */
int pon_anti_spoofing_p2p_app(int unit, bcm_port_t pon, bcm_port_t nni)
{
    int i = 0;
    int rv = 0;
    bcm_if_t encap_id = 0;
    int action_id = 0;
    bcm_vswitch_cross_connect_t gports;

    pon_anti_spoofing_init(unit, pon, nni);

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

    /* Set LIF profile by gport, IP anti-spoofing PMF will be hit when input AC's LIF-profile is 1 */
    rv = bcm_port_class_set(unit, pon_anti_spoofing_info.pon_gport, bcmPortClassFieldIngress, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set returned bcmPortClassFieldIngress %s gport:%08x\n", bcm_errmsg(rv), pon_anti_spoofing_info.pon_gport);
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

	/* Cross connect the 2 LIFs */
    bcm_vswitch_cross_connect_t_init(&gports);
	gports.port1 = pon_anti_spoofing_info.pon_gport;
	gports.port2 = pon_anti_spoofing_info.nni_gport;
	rv = bcm_vswitch_cross_connect_add(unit, &gports);
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_vswitch_cross_connect_add\n");
		print rv;
	}

    /* Will set 6 bind configuration in this PON-LIF gport */
    for (i = 0; i < 6; i++)
    {
        src_bind[i].port = pon_anti_spoofing_info.pon_gport;
    }

    return rv;
}


/*********************************************************************************
* Enable anti-spoofing function and set IPV4, IPV6 source bind configuration.
*
* Steps
*     1. Call bcm_l3_source_bind_enable_set() to enable anti-spoofing function
*        in PON-LIF gport.
*     2. Call bcm_l3_source_bind_add() to set bind configuration in this PON-LIF gport.
 */
int pon_anti_spoofing_enable(int unit)
{
    int rv = 0;
    int i = 0;

    /* Enable anti-spoffing in this PON-LIF gport */
    rv = bcm_l3_source_bind_enable_set(unit,
                                       pon_anti_spoofing_info.pon_gport,
                                       1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_source_bind_enable_set returned %s pon_gport:%d\n", 
                bcm_errmsg(rv), pon_anti_spoofing_info.pon_gport);
        return rv;
    }

    /* Add 6 bind configuration 
     * IPV4 STATIC host/subnet
	 * IPV4 DHCP host
	 * IPV6 STATIC host/subnet
	 * IPV6 DHCP  
	 * in this PON-LIF gport
	 */
   if ( sal_strcmp(src_bind_mode, "IPV4") == 0 )
    {
        /* src_bind[0] -- ipv4 static host */
        rv = bcm_l3_source_bind_add(unit, &(src_bind[0]));
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_source_bind_add returned %s src_bind[%d]\n", bcm_errmsg(rv), i);
            return rv;
        }
        /* src_bind[2] -- ipv4 DHCP */
        rv = bcm_l3_source_bind_add(unit, &(src_bind[2]));
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_source_bind_add returned %s src_bind[%d]\n", bcm_errmsg(rv), i);
            return rv;
        }

    } else if (sal_strcmp(src_bind_mode, "IPV6") == 0 )
    {
        /* src_bind[3] -- ipv6 static host */
        rv = bcm_l3_source_bind_add(unit,&(src_bind[3]));
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_source_bind_add returned %s src_bind[%d]\n", bcm_errmsg(rv), i);
            return rv;
        } 
        /* src_bind[5] -- ipv6 DHCP */
        rv = bcm_l3_source_bind_add(unit,&(src_bind[5]));
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_source_bind_add returned %s src_bind[%d]\n", bcm_errmsg(rv), i);
            return rv;
        } 
               
    } 
    
    if ( sal_strcmp(src_bind_subnet_mode, "IPV4") == 0 )
    {
        /* src_bind[1] -- ipv4 static subnet */
        rv = bcm_l3_source_bind_add(unit, &(src_bind[1]));
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_source_bind_add returned %s src_bind[%d]\n", bcm_errmsg(rv), i);
            return rv;
        }
    
    } else if (sal_strcmp(src_bind_subnet_mode, "IPV6") == 0 )
    {
        /* src_bind[4] -- ipv6 static subnet */
        rv = bcm_l3_source_bind_add(unit,&(src_bind[4]));
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_source_bind_add returned %s src_bind[%d]\n", bcm_errmsg(rv), i);
            return rv;
        } 
    } 
    
    if (sal_strcmp(src_bind_mode, "IP") == 0 && sal_strcmp(src_bind_subnet_mode, "IP") == 0)
    { 
		for (i = 0; i < 6; i++) 
		{
			rv = bcm_l3_source_bind_add(unit, 
										&(src_bind[i]));
			if (rv != BCM_E_NONE)
			{
				printf("Error, bcm_l3_source_bind_add returned %s src_bind[%d]\n", bcm_errmsg(rv), i);
				return rv;
			}
		}
	}
    return rv;
}

/*********************************************************************************
* Delete IPV4, IPV6 source bind configuration and disable anti-spoofing function.
*
* Steps
*     1. Call bcm_l3_source_bind_delete() to delete bind configuration 
*        in this PON-LIF gport.
*     2. Call bcm_l3_source_bind_enable_set() to disable anti-spoofing function
*        in PON-LIF gport.
 */
int pon_anti_spoofing_disable(int unit)
{
    int rv = 0;
    int i = 0;


    /* Delete 6 bind configuration 
	 * IPV4 STATIC host/subnet
	 * IPV4 DHCP host
	 * IPV6 STATIC host/subnet
	 * IPV6 DHCP  
	 * in this PON-LIF gport
	 */
    if ( sal_strcmp(src_bind_mode, "IPV4") == 0 )
    {
        /* src_bind[0] -- ipv4 static host */
        rv = bcm_l3_source_bind_delete(unit,&(src_bind[0]));
        if (rv != BCM_E_NONE) 
        {
            printf("Error, bcm_l3_source_bind_delete returned %s src_bind[0]\n", bcm_errmsg(rv));
            return rv;
        }
        /* src_bind[2] -- ipv4 DHCP */
        rv = bcm_l3_source_bind_delete(unit,&(src_bind[2]));
        if (rv != BCM_E_NONE) 
        {
            printf("Error, bcm_l3_source_bind_delete returned %s src_bind[2]\n", bcm_errmsg(rv));
            return rv;
        }

    } else if (sal_strcmp(src_bind_mode, "IPV6") == 0 )
    {
        /* src_bind[3] -- ipv6 static host */
        rv = bcm_l3_source_bind_delete(unit,&(src_bind[3]));
        if (rv != BCM_E_NONE) 
        {
            printf("Error, bcm_l3_source_bind_delete returned %s src_bind[3]\n", bcm_errmsg(rv));
            return rv;
        }
        /* src_bind[5] -- ipv6 DHCP */
        rv = bcm_l3_source_bind_delete(unit,&(src_bind[5]));
        if (rv != BCM_E_NONE) 
        {
            printf("Error, bcm_l3_source_bind_delete returned %s src_bind[5]\n", bcm_errmsg(rv));
            return rv;
        }   
    }
    
    if ( sal_strcmp(src_bind_subnet_mode, "IPV4") == 0 )
    {
        /* src_bind[1] -- ipv4 static subnet */
        rv = bcm_l3_source_bind_delete(unit,&(src_bind[1]));
        if (rv != BCM_E_NONE) 
        {
            printf("Error, bcm_l3_source_bind_delete returned %s src_bind[1]\n", bcm_errmsg(rv));
            return rv;
        }
    
    }  else if (sal_strcmp(src_bind_subnet_mode, "IPV6") == 0 )
    {
        /* src_bind[4] -- ipv6 static subnet */
        rv = bcm_l3_source_bind_delete(unit,&(src_bind[4]));
        if (rv != BCM_E_NONE) 
        {
            printf("Error, bcm_l3_source_bind_delete returned %s src_bind[4]\n", bcm_errmsg(rv));
            return rv;
        }
    } 
    
    if (sal_strcmp(src_bind_mode, "IP") == 0 && sal_strcmp(src_bind_subnet_mode, "IP") == 0)
    {    
		for (i = 0; i < 6; i++)
		{
			rv = bcm_l3_source_bind_delete(unit, 
										   &(src_bind[i]));
			if (rv != BCM_E_NONE) 
			{
				printf("Error, bcm_l3_source_bind_delete returned %s src_bind[%d]\n", bcm_errmsg(rv), i);
				return rv;
			}
		}
	}
    /* Disable anti-spoffing in this gport */
    rv = bcm_l3_source_bind_enable_set(unit,
                                       pon_anti_spoofing_info.pon_gport,
                                       0);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_l3_source_bind_enable_set returned %s pon_gport:%d\n", 
                bcm_errmsg(rv), pon_anti_spoofing_info.pon_gport);
        return rv;
    }

    return rv;
}

/*********************************************************************************
* Create a field group to drop un-matched traffic for IPv4 DHCP,  IPv6 DHCP and Static.
*
* Steps
*     1. Create a field group
*     2. Attach a action set to field group
*     3. Add entries to drop traffic
 */
int pon_anti_spoofing_drop(int unit)
{
    bcm_field_aset_t aset;
    bcm_field_entry_t *ent[3];
    bcm_field_group_config_t *grp;
    bcm_field_AppType_t app_type[4] = {bcmFieldAppTypeIp4MacSrcBind, bcmFieldAppTypeIp6MacSrcBind,
        bcmFieldAppTypeIp6SrcBind, bcmFieldAppTypeIp4SrcBind};
    int group_priority = 10;
    int result;
    int auxRes;
    int i = 0;

    grp = &(pon_anti_spoofing_info.grp);
    for (i = 0; i < 3; i++)
    {
        ent[i] = &(pon_anti_spoofing_info.ent[i]);
    }

    bcm_field_group_config_t_init(grp);
    grp->group = -1;

    /* Define the QSET
     *  bcmFieldQualifyInPort: Single Input Port
     *  bcmFieldQualifyAppType: Application type, to see if L3 source bind lookup is done
     *  bcmFieldQualifyL2DestHit/bcmFieldQualifyL3SrcRouteHit: L2 Destination/L3 source lookup success(LEM/LPM look up result)
     *  bcmFieldQualifyIpmcHit:   IP Multicast lookup hit(TCAM look up result)
     *  bcmFieldQualifyStageIngress: Field Processor pipeline ingress stage
     */
    BCM_FIELD_QSET_INIT(grp->qset);
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyAppType);
    if (is_device_or_above(unit, JERICHO))
    {
        BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyL3SrcRouteHit);
    }
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyL2DestHit);
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyIpmcHit);
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
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDoNotLearn);

    /*  Attach the action set */
    result = bcm_field_group_action_set(unit, grp->group, aset);
    if (BCM_E_NONE != result) 
    {
        printf("Error in bcm_field_group_action_set returned %s\n", bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    /*
     *  Add 3 entries to the Field group.
     *  Match the packets on the l3 source bind is done and look up result.
     *    1. Lookup is done for DHCPv4 and LEM look up result is not-found:
     *       drop
     *    2. Lookup is done for DHCPv6 and LEM look up result is not-found:
     *       drop
     *    3. lookup is done for Staticv6 and TCAM(ARAD+)/LPM(JERICHO) lookup result is not-found:
     *       drop
     */
    for (i = 0 ; i < 3; i++)
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
        /* i=0: bcmFieldAppTypeIp4MacSrcBind
         * i=1: bcmFieldAppTypeIp6MacSrcBind
         */
        if ((i == 0) && sal_strcmp(src_bind_mode, "IPV6") == 0 && sal_strcmp(src_bind_subnet_mode, "IPV6") == 0)
        {
            continue;
        }

        if ((i == 1) && sal_strcmp(src_bind_mode, "IPV4") == 0 && sal_strcmp(src_bind_subnet_mode, "IPV4") == 0)
        {
            continue;
        }

        result = bcm_field_qualify_AppType(unit, *(ent[i]), app_type[i]);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[0] in bcm_field_qualify_AppType returned %s\n", bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
        result = bcm_field_qualify_L2DestHit(unit, *(ent[i]), 0x0, 0x01);
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

        result = bcm_field_action_add(unit, *(ent[i]), bcmFieldActionDoNotLearn, 0, 0);
        if (BCM_E_NONE != result)
        {
            printf("Error, bcm_field_action_add ent[2] - bcmFieldActionDoNotLearn result:%s\n", bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
        result = bcm_field_entry_prio_set(unit, *(ent[i]), (10 - i));
        if (result) {
            printf("bcm_field_entry_prio_set *(ent[0]) failure\n");
            return result;
        }
    }

    /* IPV6 STATIC */
    if (sal_strcmp(src_bind_mode, "IPV6") == 0 || sal_strcmp(src_bind_subnet_mode, "IP") == 0)
    {
        i = 2;
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
        else
        {
            result = bcm_field_qualify_IpmcHit(unit, *(ent[i]), 0x0, 0x01);
            if (BCM_E_NONE != result)
            {
                printf("Error ent[%d] in bcm_field_qualify_IpmcHit returned %s\n", i, bcm_errmsg(result));
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

        result = bcm_field_action_add(unit, *(ent[i]), bcmFieldActionDoNotLearn, 0, 0);
        if (BCM_E_NONE != result)
        {
            printf("Error, bcm_field_action_add ent[%d] - bcmFieldActionDoNotLearn result:%s\n", i, bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
        result = bcm_field_entry_prio_set(unit, *(ent[i]), (10-i));
        if (result) {
            printf("bcm_field_entry_prio_set *(ent[%d]) failure\n", i);
            return result;
        }
    }

    /* install */
    result = bcm_field_group_install(unit, grp->group);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_install returned %s\n", bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    /*
     *  Everything went well.
     */
    return result;

}

/*********************************************************************************
* Delete the field group which is create by pon_anti_spoofing_drop().
*
* Steps
*     1. Delete  and destory entries
*     2. Destory field group
 */
int pon_anti_spoofing_drop_destory(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int i = 0;

    for (i = 0; i < 3; i++)
    {
        rv = bcm_field_entry_remove(unit, pon_anti_spoofing_info.ent[i]);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_field_entry_remove %d returned %s\n", i, bcm_errmsg(rv));
            return rv;
        }

        rv = bcm_field_entry_destroy(unit, pon_anti_spoofing_info.ent[i]);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_field_entry_destroy %d returned %s\n", i, bcm_errmsg(rv));
            return rv;
        }
    }
    
    rv = bcm_field_group_destroy(unit, pon_anti_spoofing_info.grp.group);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_group_destroy returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*********************************************************************************
* Create a field group to drop un-matched traffic for arp request in downstream.
*
* Steps
*     1. Create a field group
*     2. Attach a action set to field group
*     3. Add entries to drop traffic
 */
bcm_field_entry_t arp_ent;
bcm_field_group_config_t arp_grp;

int pon_anti_spoofing_arp_drop(int unit)
{
    if (arp_check_enable != 0)
    { 
		bcm_field_aset_t aset;
		bcm_field_entry_t *ent[1];
		bcm_field_group_config_t *grp;
		int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
		int result;
		int auxRes;
		int i = 0;
		bcm_field_data_qualifier_t dq_arp_opcode;
		uint8 dqArpCodeData[2];
		uint8 dqArpCodeMask[2];

		grp = &arp_grp;
		ent[0] = &arp_ent;

		bcm_field_group_config_t_init(grp);
		grp->group = -1;

		/* 
		 * Define the Data qualifier: 
		 * Take the 6th and 7th byte of the ARP header 
		 * We assume that the ARP header is just after the L2 header 
		 * To ensure this, entries will be match with EtherType = 0x0806 
		 */
		bcm_field_data_qualifier_t_init(&dq_arp_opcode); /* ARP Opcode */
		dq_arp_opcode.offset_base = bcmFieldDataOffsetBaseFirstHeader; /* Consider the ARP header following Eth */
		dq_arp_opcode.offset = 6; /* Start at the 6th bytes from the start of ARP Header */
		dq_arp_opcode.length = 2; /* 2 bytes */
		result = bcm_field_data_qualifier_create(unit, &dq_arp_opcode);
		if (BCM_E_NONE != result) {
			printf("Error in bcm_field_data_qualifier_create\n");
			auxRes = bcm_field_data_qualifier_destroy(unit, dq_arp_opcode.qual_id);
			return result;
		}

		/* Define the QSET
		 *  bcmFieldQualifyInPort: Single Input Port
		 *  bcmFieldQualifyEtherType: ethernet type, to see input packet whether is arp.
		 *  bcmFieldQualifyL2DestHit: L2 Destination lookup success(LEM look up result)
		 *  bcmFieldQualifyL3DestRouteHit: L3 Destination lookup route table hit(LPM look up result)
		 *  bcmFieldQualifyStageIngress: Field Processor pipeline ingress stage
		 */
		BCM_FIELD_QSET_INIT(grp->qset);
		BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyInPort);
		BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyInterfaceClassVPort);
		BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyEtherType);
		BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyL2DestHit);
		BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyL3DestRouteHit);
		BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyStageIngress);

		/* Add the Data qualifier to the QSET */
		result = bcm_field_qset_data_qualifier_add(unit, &(grp->qset), dq_arp_opcode.qual_id);
		if (BCM_E_NONE != result) {
			printf("Error in bcm_field_qset_data_qualifier_add\n");
			auxRes = bcm_field_data_qualifier_destroy(unit, dq_arp_opcode.qual_id);
			return result;
		}

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
		BCM_FIELD_ASET_ADD(aset, bcmFieldActionDoNotLearn);

		/*  Attach the action set */
		result = bcm_field_group_action_set(unit, grp->group, aset);
		if (BCM_E_NONE != result) 
		{
			printf("Error in bcm_field_group_action_set returned %s\n", bcm_errmsg(result));
			auxRes = bcm_field_group_destroy(unit, grp->group);
			return result;
		}

		result = bcm_field_entry_create(unit, grp->group, ent[0]);
		if (BCM_E_NONE != result)
		{
			printf("Error %d in bcm_field_entry_create returned %s\n", i, bcm_errmsg(result));
			auxRes = bcm_field_group_destroy(unit, grp->group);
			return result;
		}

		/* ARP EtherType */
		result = bcm_field_qualify_EtherType(unit, *(ent[0]), 0x0806, 0xFFFF);
		if (BCM_E_NONE != result) {
			printf("Error ent[0] in bcm_field_qualify_EtherType %s\n", bcm_errmsg(result));
			auxRes = bcm_field_group_destroy(unit, grp->group);
			return result;
		}

		/* ARP.Opcode = ARP-Request (0x1) */
		dqArpCodeData[0] = 0x0;
		dqArpCodeData[1] = 0x1;
		dqArpCodeMask[0] = 0xFF;
		dqArpCodeMask[1] = 0xFF;
		result = bcm_field_qualify_data(unit,
										*(ent[0]),
										dq_arp_opcode.qual_id,
										&(dqArpCodeData[0]),
										&(dqArpCodeMask[0]),
										2 /* length here always in bytes */);
		if (BCM_E_NONE != result) {
			printf("Error in bcm_field_qualify_data\n");
			auxRes = bcm_field_group_destroy(unit, grp->group);
			return result;
		}

		result = bcm_field_qualify_InterfaceClassVPort(unit, *(ent[0]), 0x0, 0x1);
		if (BCM_E_NONE != result) {
			printf("Error ent[0] in bcm_field_qualify_InterfaceClassVPort %s\n", bcm_errmsg(result));
			auxRes = bcm_field_group_destroy(unit, grp->group);
			return result;
		}

		/* Not found in LEM lookup result */
		result = bcm_field_qualify_L2DestHit(unit, *(ent[0]), 0x0, 0x01);
		if (BCM_E_NONE != result)
		{
			printf("Error ent[0] in bcm_field_qualify_L2DestHit returned %s\n", bcm_errmsg(result));
			auxRes = bcm_field_group_destroy(unit, grp->group);
			return result;
		}

		/* Not found in LPM lookup result */
		result = bcm_field_qualify_L3DestRouteHit(unit, *(ent[0]), 0x0, 0x01);
		if (BCM_E_NONE != result)
		{
			printf("Error ent[0] in bcm_petra_field_qualify_IpmcHit returned %s\n", bcm_errmsg(result));
			auxRes = bcm_field_group_destroy(unit, grp->group);
			return result;
		}

		result = bcm_field_action_add(unit, *(ent[0]), bcmFieldActionDrop, 0, 0);
		if (BCM_E_NONE != result)
		{
			printf("Error ent[0] in bcm_field_action_add returned %s\n", bcm_errmsg(result));
			auxRes = bcm_field_group_destroy(unit, grp->group);
			return result;
		}

		result = bcm_field_action_add(unit, *(ent[0]), bcmFieldActionDoNotLearn, 0, 0);
		if (BCM_E_NONE != result)
		{
			printf("Error, bcm_field_action_add ent[0] - bcmFieldActionDoNotLearn result:%s\n", bcm_errmsg(result));
			auxRes = bcm_field_group_destroy(unit, grp->group);
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

		/*
		 *  Everything went well.
		 */
		return result;
	}
}

/*********************************************************************************
* Delete the field group which is create by pon_anti_spoofing_arp_drop().
*
* Steps
*     1. Delete  and destory entries
*     2. Destory field group
 */
int pon_anti_spoofing_arp_drop_destory(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_field_entry_remove(unit, arp_ent);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_entry_remove returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_destroy(unit, arp_ent);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_entry_destroy returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_group_destroy(unit, arp_grp.group);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_group_destroy returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

bcm_field_presel_set_t psset_up_static;
int presel_id_up_static = 0;
/* Create a pselect for IP anti-spoofing static mode in upstream.
 * params:
 *    unit: device number
 *    in_pbmp: bitmap for PON port want to enable IP spoofing static mode.
 */
int pon_anti_spoofing_IPv4_static_pselect_set(/* in */int unit, 
                                                           /* in */bcm_pbmp_t in_pbmp)
{
    int result;
    int auxRes;
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int i;
    int port_num = get_local_port_number(unit);
    int presel_flags = 0;
          
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id_up_static);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id_up_static | presel_flags);
            return result;
        }
    } else {
        result = bcm_field_presel_create_id(unit, presel_id_up_static);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id_up_static);
            return result;
        }
    }

    /* 
     * Set the preselector to be on the ports
     */
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_ASSIGN(pbm, in_pbmp);
    for(i=0; i<port_num; i++)
    { 
        BCM_PBMP_PORT_ADD(pbm_mask, i);
    }

    result = bcm_field_qualify_Stage(unit, presel_id_up_static | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_Stage\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_up_static | presel_flags);
        return result;
    }
    
    result = bcm_field_qualify_InPorts(unit, presel_id_up_static | BCM_FIELD_QUALIFY_PRESEL | presel_flags, pbm, pbm_mask);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_InPorts\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_up_static | presel_flags);
        return result;
    }
  
    /* Select on LIF-Profile[0] = 0x1 */
    result = bcm_field_qualify_InterfaceClassVPort(unit, presel_id_up_static | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0x1, 0x1);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_InterfaceClassVPort\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_up_static | presel_flags);
        return result;
    }

    /* Select on  IPV4 Source bind */
    /*result = bcm_field_qualify_AppType(unit, presel_id_up_static | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp4SrcBind);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_AppType bcmFieldAppTypeIp4SrcBind\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_up_static);
        return result;
    }*/

    BCM_FIELD_PRESEL_INIT(psset_up_static);
    BCM_FIELD_PRESEL_ADD(psset_up_static, presel_id_up_static);

    return result;
}


bcm_field_group_config_t grp_tcam, grp_de, grp_cascaded;
bcm_field_entry_t ent_tcam, ent_de, ent_cascaded[2];
int is_cascaded = 1;

/* ***************************************************************** */
/* 
 * pon_anti_spoofing_compare_fg_set_a(): 
 *  
 * 1. Create the first field group for compare in the FP: 
 *    TCAM Database of 80 bits.    
 * 2. Set the QSET of to be user defined.    
 */
/* ***************************************************************** */
int pon_anti_spoofing_compare_fg_set_a(/* in */  int unit,
                                       /* in */  bcm_field_presel_set_t psset,
                                       /* in */  int group_priority) 
{
  int result;
  bcm_field_aset_t aset, aset_cascaded;
  bcm_field_data_qualifier_t data_qual;
  int index = 0;
  bcm_field_data_qualifier_t dq_arp_opcode;
  uint8 dqArpCodeData[2];
  uint8 dqArpCodeMask[2];

  /* 
   * Define the first Field Group (LSB bits of key 7).
   */
  bcm_field_group_config_t_init(&grp_tcam);
  grp_tcam.group = (-1);
  grp_tcam.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | 
      BCM_FIELD_GROUP_CREATE_WITH_PRESELSET |
      BCM_FIELD_GROUP_CREATE_IS_EQUAL;
  grp_tcam.mode = bcmFieldGroupModeAuto;
  grp_tcam.priority = group_priority;
  grp_tcam.preselset = psset;

  /* 
   * Define the QSET. 
   * No need to use IsEqual as qualifier for this field group.
   */
  BCM_FIELD_QSET_INIT(grp_tcam.qset);
  BCM_FIELD_QSET_ADD(grp_tcam.qset, bcmFieldQualifyStageIngress);

  if (is_cascaded) 
  {
      /* Set cascaded field size */
      result = bcm_field_control_set(unit,
                                     bcmFieldControlCascadedKeyWidth,
                                     16 /* bits in cascaded key */);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_control_set\n");
          return result;
      }

      /* Add cascaded qualifier to field group */
      BCM_FIELD_QSET_ADD(grp_tcam.qset, bcmFieldQualifyCascadedKeyValue);

     /* Pad with 4 zeros so that next field will start at bit 20
         * This must be done before creating hte next field, since 
         * both are located in the key according to the order of 
         * their creation.
         */
      bcm_field_data_qualifier_t_init(&data_qual); 
      data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | 
          BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
      data_qual.offset = 0;  
      data_qual.qualifier = bcmFieldQualifyConstantZero; 
      data_qual.length = 4; 
      data_qual.stage = bcmFieldStageIngress;
      result = bcm_field_data_qualifier_create(unit, &data_qual);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }

      result = bcm_field_qset_data_qualifier_add(unit, &grp_tcam.qset, data_qual.qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          return result;
      }
  }

  /* 1st compared key: IN_LIF
 */
  bcm_field_data_qualifier_t_init(&data_qual); 
  data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | 
      BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;  
  data_qual.offset = 0;  
  data_qual.qualifier = bcmFieldQualifyInVPort; 
  data_qual.length = 16; 
  data_qual.stage = bcmFieldStageIngress;
  result = bcm_field_data_qualifier_create(unit, &data_qual);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_data_qualifier_create\n");
      return result;
  }

  result = bcm_field_qset_data_qualifier_add(unit, &grp_tcam.qset, data_qual.qual_id);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qset_data_qualifier_add\n");
      return result;
  }

  /* Pad with 4 zeros so that next field will start at bit 20
   * This must be done before creating hte next field, since 
   * both are located in the key according to the order of 
   * their creation.
   */
  bcm_field_data_qualifier_t_init(&data_qual); 
  data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | 
      BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
  data_qual.offset = 0;  
  data_qual.qualifier = bcmFieldQualifyConstantZero; 
  data_qual.length = 4; 
  data_qual.stage = bcmFieldStageIngress;
  result = bcm_field_data_qualifier_create(unit, &data_qual);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_data_qualifier_create\n");
      return result;
  }
  
  result = bcm_field_qset_data_qualifier_add(unit, &grp_tcam.qset, data_qual.qual_id);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qset_data_qualifier_add\n");
      return result;
  }

  /* 2nd compared key: IN_LIF  */  
  bcm_field_data_qualifier_t_init(&data_qual); 
  data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | 
      BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;  
  data_qual.offset = 0;  
  data_qual.qualifier = bcmFieldQualifyInVPort; 
  data_qual.length = 16; 
  data_qual.stage = bcmFieldStageIngress;
  result = bcm_field_data_qualifier_create(unit, &data_qual);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_data_qualifier_create\n");
      return result;
  }

  result = bcm_field_qset_data_qualifier_add(unit, &grp_tcam.qset, data_qual.qual_id);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qset_data_qualifier_add\n");
      return result;
  }

  /*
   *  Define the ASET - use counter 0.
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);

  /*  Create the Field group with type TCAM */
  result = bcm_field_group_config_create(unit, &grp_tcam);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create for grp_tcam.group %d\n", grp_tcam.group);
      return result;
  }

  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, grp_tcam.group, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set for group %d\n", grp_tcam.group);
    return result;
  }

  if (is_cascaded) 
  {
      /* Create initial field group if cascaded */
      bcm_field_group_config_t_init(&grp_cascaded);
      grp_cascaded.group = (-1);
      grp_cascaded.flags |= (BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET);
      grp_cascaded.mode = bcmFieldGroupModeAuto;
      grp_cascaded.priority = BCM_FIELD_GROUP_PRIO_ANY;
      grp_cascaded.preselset = psset;

      /* 
       * Define the Data qualifier: 
       * Take the 6th and 7th byte of the ARP header 
       * We assume that the ARP header is just after the L2 header 
       * To ensure this, entries will be match with EtherType = 0x0806 
       */
      bcm_field_data_qualifier_t_init(&dq_arp_opcode); /* ARP Opcode */
      dq_arp_opcode.offset_base = bcmFieldDataOffsetBaseFirstHeader; /* Consider the ARP header following Eth */
      dq_arp_opcode.offset = 6; /* Start at the 6th bytes from the start of ARP Header */
      dq_arp_opcode.length = 2; /* 2 bytes */
      result = bcm_field_data_qualifier_create(unit, &dq_arp_opcode);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }

      BCM_FIELD_QSET_INIT(grp_cascaded.qset);
      BCM_FIELD_QSET_ADD(grp_cascaded.qset, bcmFieldQualifyStageIngress);
      BCM_FIELD_QSET_ADD(grp_cascaded.qset, bcmFieldQualifyAppType);
      BCM_FIELD_QSET_ADD(grp_cascaded.qset, bcmFieldQualifyEtherType);
      /* Add the Data qualifier to the QSET */
      result = bcm_field_qset_data_qualifier_add(unit, &(grp_cascaded.qset), dq_arp_opcode.qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          return result;
      }

      result = bcm_field_group_config_create(unit, &grp_cascaded);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_config_create for grp_cascaded.group %d\n", grp_cascaded.group);
          return result;
      }
      
      BCM_FIELD_ASET_INIT(aset_cascaded);
      BCM_FIELD_ASET_ADD(aset_cascaded, bcmFieldActionCascadedKeyValueSet);

      result = bcm_field_group_action_set(unit, grp_cascaded.group, aset_cascaded);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_action_set for group %d\n", grp_cascaded.group);
          return result;
      }
  }

  result = bcm_field_entry_create(unit, grp_tcam.group, &ent_tcam);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  if (is_cascaded) 
  {
      result = bcm_field_qualify_CascadedKeyValue(unit, ent_tcam, 1, 1);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_CascadedKeyValue\n");
          return result;
      }
  }

  result = bcm_field_entry_install(unit, ent_tcam);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_install\n");
      return result;
  }

  if (is_cascaded) 
  {
      for (index = 0; index < 2; index++) 
      {
          result = bcm_field_entry_create(unit, grp_cascaded.group, &(ent_cascaded[index]));
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_entry_create\n");
              return result;
          }
      }

      index = 0;
      if (sal_strcmp(src_bind_mode, "IPV6") != 0) 
      { 
		  result = bcm_field_qualify_AppType(unit, ent_cascaded[index], bcmFieldAppTypeIp4SrcBind);
		  if (BCM_E_NONE != result) {
			  printf("Error in bcm_field_qualify_InPort\n");
			  return result;
		  }
      }
      result = bcm_field_action_add(unit, ent_cascaded[index], bcmFieldActionCascadedKeyValueSet, 1, 1);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_action_add\n");
          return result;
      }

      result = bcm_field_entry_install(unit, ent_cascaded[index]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_install\n");
          return result;
      }

      if (arp_check_enable != 0)
      {
      index++;
      result = bcm_field_qualify_EtherType(unit, ent_cascaded[index], 0x0806, 0xFFFF);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_EtherType\n");
          return result;
      }
      /* ARP.Opcode = ARP-Request (0x1) */
      dqArpCodeData[0] = 0x0;
      dqArpCodeData[1] = 0x1;
      dqArpCodeMask[0] = 0xFF;
      dqArpCodeMask[1] = 0xFF;
      result = bcm_field_qualify_data(unit,
                                      ent_cascaded[index],
                                      dq_arp_opcode.qual_id,
                                      &(dqArpCodeData[0]),
                                      &(dqArpCodeMask[0]),
                                      2 /* length here always in bytes */);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_data\n");
          return result;
      }

      result = bcm_field_action_add(unit, ent_cascaded[index], bcmFieldActionCascadedKeyValueSet, 1, 1);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_action_add\n");
          return result;
      }
      
      result = bcm_field_entry_install(unit, ent_cascaded[index]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_install\n");
          return result;
      }
	}  
  }

  printf("\n\n**** (A) pon_anti_spoofing_compare_fg_set_a for group %d Complete!!\n\n", grp_tcam.group);
  
  return result;

} /* compare_field_group_set_a */


/* ***************************************************************** */
/* 
 * pon_anti_spoofing_compare_fg_set_b(): 
 *  
 * 1. Create Second field group for compare in the FP: 
 *    Direct Extraction Database of 80 bits. 
 * 2. Set the QSET of to be user defined + bcmFieldQualifyIsEqualValue.
 * 3. Set the action of the Direct Extraction database to 
 *    be according to result of the compare operation
 *    (located in the 5 MSB bits of Key 7). 
 */
/* ***************************************************************** */
int pon_anti_spoofing_compare_fg_set_b(/* in */  int unit,
                                       /* in */  bcm_field_presel_set_t psset,
                                       /* in */  int group_priority)
{
  int result;
  int dq_idx;
  uint8 data_mask;
  bcm_field_aset_t aset;
  bcm_field_extraction_action_t extract;
  bcm_field_extraction_field_t is_equal[2];
  bcm_field_data_qualifier_t data_qual;
  
  /* 
   * Define the second Field Group (MSB bits of key 7).
   */
  bcm_field_group_config_t_init(&grp_de);
  grp_de.group = (-1);
  grp_de.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | 
      BCM_FIELD_GROUP_CREATE_WITH_PRESELSET |
      BCM_FIELD_GROUP_CREATE_IS_EQUAL;
  grp_de.mode = bcmFieldGroupModeDirectExtraction;
  grp_de.priority = group_priority;
  grp_de.preselset = psset;

  /* 
   * Define the QSET - use InLIF as qualifier. 
   */
  BCM_FIELD_QSET_INIT(grp_de.qset);
  BCM_FIELD_QSET_ADD(grp_de.qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(grp_de.qset, bcmFieldQualifyIsEqualValue);

 /* If cascaded the compare key is as follows:
   * 80b LSB : < 20b = 0, 20b = InLIF, 20b = InLIF,  20b = ARP REQ & bcmFieldAppTypeIp4SrcBind > 
   * 80b MSB : < 20b = 0, 20b = LPM result, 20b = LEM result, 20b = 0> 
   */
  if (is_cascaded) 
  {
    for (dq_idx = 0; dq_idx < 2; dq_idx++) 
    {
       bcm_field_data_qualifier_t_init(&data_qual); 
       data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | 
           BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
       data_qual.offset = 0;
       if (dq_idx == 0) {
           data_qual.qualifier = bcmFieldQualifyConstantZero; 
           data_qual.length = 10; 
       } else {
           data_qual.qualifier = bcmFieldQualifyConstantZero; 
           data_qual.length = 10; 
       }
       data_qual.stage = bcmFieldStageIngress;
       result = bcm_field_data_qualifier_create(unit, &data_qual);
       if (BCM_E_NONE != result) {
           printf("Error in bcm_field_data_qualifier_create\n");
           return result;
       }

       result = bcm_field_qset_data_qualifier_add(unit, &grp_de.qset, data_qual.qual_id);
       if (BCM_E_NONE != result) {
           printf("Error in bcm_field_qset_data_qualifier_add\n");
           return result;
       }
    }
  }

  for (dq_idx = 0; dq_idx < 2; dq_idx++) 
  {
      bcm_field_data_qualifier_t_init(&data_qual); 
      data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | 
          BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
      data_qual.offset = 0;
      if (dq_idx == 0) {
          data_qual.qualifier = bcmFieldQualifyL3SrcRouteValue; 
          data_qual.length = 15; 
      } else {
          data_qual.qualifier = bcmFieldQualifyConstantZero; 
          data_qual.length = 5; 
      }
      data_qual.stage = bcmFieldStageIngress;
      result = bcm_field_data_qualifier_create(unit, &data_qual);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }

      result = bcm_field_qset_data_qualifier_add(unit, &grp_de.qset, data_qual.qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          return result;
      }
  }

  for (dq_idx = 0; dq_idx < 2; dq_idx++) 
  {
      bcm_field_data_qualifier_t_init(&data_qual); 
      data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | 
          BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
      data_qual.offset = 0;  

      if (dq_idx == 0) {
          data_qual.qualifier = bcmFieldQualifyL2DestValue; 
          data_qual.length = 16; 
      } else {
          data_qual.qualifier = bcmFieldQualifyConstantZero; 
          data_qual.length = 4; 
      }
      data_qual.stage = bcmFieldStageIngress;
      result = bcm_field_data_qualifier_create(unit, &data_qual);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }

      result = bcm_field_qset_data_qualifier_add(unit, &grp_de.qset, data_qual.qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          return result;
      }
  }
 
  /*
   *  Define the ASET.
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

  /*  Create the Field group with type Direct Extraction */
  result = bcm_field_group_config_create(unit, &grp_de);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create for group %d\n", grp_de.group);
      return result;
  }

  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, grp_de.group, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set for group %d\n", grp_de.group);
      return result;
  }

  /* 
   * Create the Direct Extraction entry:
   *  1. create the entry
   *  2. Construct the action:
   *     bit 0 = action valid 
   *     bits 3:0 = IsEqual
   *  3. Install entry (HW configuration)
   */
  result = bcm_field_entry_create(unit, grp_de.group, &ent_de);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  /* 
   * IsEqualValue Qualifier: 
   *    IsEqualValue qualifier has 4 bits, each indicated 
   *    match of 20 bits in the compare key. The LSB bit 
   *    corresponds to the LSB 20 compared bits, and so on. 
   * In this example:
   *    Look at different bits in
   *    order to use compare result for Drop  
   */
  data_mask = is_cascaded ? 7 : 3;
  result = bcm_field_qualify_IsEqualValue(unit, ent_de, 0, data_mask);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_IsEqualValue\n");
      return result;
  }

  bcm_field_extraction_action_t_init(&extract);
  bcm_field_extraction_field_t_init(&is_equal[0]);
  bcm_field_extraction_field_t_init(&is_equal[1]);

  extract.action = bcmFieldActionDrop;
  extract.bias = 0;

  /* First extraction structure indicates action is valid */
  is_equal[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
  is_equal[0].bits  = 1;
  is_equal[0].value = 1;
  is_equal[0].qualifier = bcmFieldQualifyConstantOne;

  if (!is_device_or_above(unit,JERICHO_PLUS)) {
      /* second extraction structure indicates to use IsEqual qualifier */
      is_equal[1].flags = 0;
      is_equal[1].bits  = 4;
      is_equal[1].lsb   = 0;
      is_equal[1].qualifier = bcmFieldQualifyIsEqualValue;
  }

  result = bcm_field_direct_extraction_action_add(unit,
                                                  ent_de,
                                                  extract,
                                                  is_device_or_above(unit,JERICHO_PLUS)? 1 : 2 /* count */,
                                                  &is_equal);

  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_direct_extraction_action_add\n");
      return result;
  }

  /* Write entry to HW */
  result = bcm_field_entry_install(unit, ent_de);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_install\n");
      return result;
  } 
  
  printf("\n\n**** (B) pon_anti_spoofing_compare_fg_set_b for group %d Complete!!\n\n", grp_de.group);

  return result;

} /* compare_field_group_set_b */


/* ***************************************************************** */
/*
  * This cint creates two 80 bit field groups for comparison 
  * before the second cycle. Both FGs are created in the 
  * second cycle, using the same key. The result of the 
  * comparison is written to the MSB bits of same FG's key, 
  * and can be used for direct extraction in the second cycle. 
  * Can be also combined with cascaded. 
  *  
  * pon_anti_spoofing_compare_fg_set(int unit); 
  *  
  * Clean: 
  * pon_anti_spoofing_compare_fg_destroy(int unit);  
 */
/* ***************************************************************** */
int pon_anti_spoofing_compare_fg_set(int unit) 
{
  int result;
  int group_priority_a = BCM_FIELD_GROUP_PRIO_ANY,
      group_priority_b = BCM_FIELD_GROUP_PRIO_ANY;
  bcm_field_group_t 
      group_a = 2,
      group_b = 3;
  bcm_pbmp_t in_pbmp;

  BCM_PBMP_CLEAR(in_pbmp);
  /* Can add other PON port use BCM_PBMP_PORT_ADD */
  BCM_PBMP_PORT_ADD(in_pbmp, pon_anti_spoofing_info.pon_port);

  result = pon_anti_spoofing_IPv4_static_pselect_set(unit, in_pbmp);
  if (BCM_E_NONE != result)
  {
      bcm_field_presel_destroy(unit, presel_id_up_static);
      printf("Error in pon_anti_spoofing_IPv4_static_pselect_set\n");
      return result;
  }

  /* Create LSB field group  for IPv4 static in upstream */
  result = pon_anti_spoofing_compare_fg_set_a(
                unit, 
                psset_up_static,
                group_priority_a
            );
  if (BCM_E_NONE != result) {
      printf("Error in pon_anti_spoofing_compare_fg_set_a for IPv4 static in upstream\n");
      return result;
  } 

  /* Create MSB field group for IPv4 static in upstream */
  result = pon_anti_spoofing_compare_fg_set_b(
                unit, 
                psset_up_static,
                group_priority_b
           );
  if (BCM_E_NONE != result) {
      printf("Error in pon_anti_spoofing_compare_fg_set_b for IPv4 static in upstream\n");
      return result;
  }

  return result;

} /* compare_field_group_example */

/* ***************************************************************** */
/*
 *  Destroy configuration of this cint
 */
/* ***************************************************************** */
int pon_anti_spoofing_compare_fg_destroy(int unit) 
{
  int result, index;

  if (is_cascaded) 
  {
      for (index = 0; index < 2; index++) 
      {
		  result = bcm_field_entry_destroy(unit, ent_cascaded[index]);
		  if (BCM_E_NONE != result) {
			  printf("Error in bcm_field_entry_destroy entry %d\n", ent_cascaded[index]);
			  return result;
		  }
      }

      result = bcm_field_group_destroy(unit, grp_cascaded.group);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_destroy group %d\n", grp_cascaded.group);
          return result;
      }
  }

  result = bcm_field_entry_destroy(unit, ent_tcam);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_destroy entry %d\n", ent_tcam);
      return result;
  }

  result = bcm_field_group_destroy(unit, grp_tcam.group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_destroy group %d\n", grp_tcam.group);
      return result;
  }

  result = bcm_field_entry_destroy(unit, ent_de);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_destroy entry %d\n", ent_de);
      return result;
  }

  result = bcm_field_group_destroy(unit, grp_de.group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_destroy group %d\n", grp_de.group);
      return result;
  }

  result = bcm_field_presel_destroy(unit, presel_id_up_static);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_presel_destroy group %d\n", grp_de.group);
      return result;
  }

  return result;

} /* compare_field_group_destroy */


/*********************************************************************************
* clean all configuration. include FG, anti-spoofing and pon service
*
* Steps
*     1. clean FG configuration
*     2. clean anti-spoofing configuration
*     3. clean pon service configuration
 */
int pon_anti_spoofing_app_clean(int unit)
{
    int rv = 0;

    rv = pon_anti_spoofing_drop_destory(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_anti_spoofing_drop_destory returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = pon_anti_spoofing_disable(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_anti_spoofing_disable returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Delete pon gport */
    rv = vswitch_delete_port(unit, pon_anti_spoofing_info.vsi, pon_anti_spoofing_info.pon_gport);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, vswitch_delete_port returned %s remove pon gport failed\n", bcm_errmsg(rv));
        return rv;
    }

    /* Delete nni gport */
    rv = vswitch_delete_port(unit, pon_anti_spoofing_info.vsi, pon_anti_spoofing_info.nni_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_delete_port returned %s remove nni gport failed\n", bcm_errmsg(rv));
        return rv;
    }

    /*1. delete multicast group*/
    rv = bcm_multicast_destroy(unit, pon_anti_spoofing_info.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_destroy \n");
         return rv;
    }

    /*2. destroy vswitch */
    rv = bcm_vlan_destroy(unit,pon_anti_spoofing_info.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_destroy \n");
        return rv;
    }

    return rv;
}

/*
 * clean up the configurations of 1:1 sercies.
 */
int pon_anti_spoofing_p2p_app_clean(int unit)
{
    int rv;
    bcm_vswitch_cross_connect_t gports;

    /* Delete the cross connected LIFs */
    gports.port1 = pon_anti_spoofing_info.pon_gport;
    gports.port2 = pon_anti_spoofing_info.nni_gport;
    rv = bcm_vswitch_cross_connect_delete(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_delete\n");
        print rv;
        return rv;
    }

    /* Delete PON LIF */
    rv = bcm_vlan_port_destroy(unit, pon_anti_spoofing_info.pon_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy \n");
        return rv;
    }

    /* Delete NNI LIF */
    rv = bcm_vlan_port_destroy(unit, pon_anti_spoofing_info.nni_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy \n");
        return rv;
    }

    return rv;        
}

int ipv4_dhcp_entries = 0;
int ipv4_static_entries = 0;

int add_sav_host_entry_stress_test(int unit, int is_dhcp)
{
    bcm_l3_source_bind_t src_bind_ent;
    bcm_l3_source_bind_t_init(&src_bind_ent);
    int rv = 0;
    int i = 0;
    int nof_entries = 0;

    rv =  bcm_l3_source_bind_enable_set(unit, pon_anti_spoofing_info.pon_gport,1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_source_bind_enable_set returned %s when enable anti-spoofing per LIF\n", bcm_errmsg(rv));
        return rv;
    }

    src_bind_ent.ip = 16843009;
    if (is_dhcp)
    {
        src_bind_ent.mac[0] = 0x0;
        src_bind_ent.mac[1] = 0x0;
        src_bind_ent.mac[2] = 0x1;
        src_bind_ent.mac[3] = 0x0;
        src_bind_ent.mac[4] = 0x0;
        src_bind_ent.mac[5] = 0x1;
        nof_entries = ((SOC_DPP_DEFS_GET_NOF_LOCAL_LIFS(unit)/SOC_DPP_DEFS_GET_NOF_ISEM_TABLES(unit)) > (32*1024) ? (32*1024) : (SOC_DPP_DEFS_GET_NOF_LOCAL_LIFS(unit)/SOC_DPP_DEFS_GET_NOF_ISEM_TABLES(unit)));
        /* NNI LIF take 1 SEMB entry */
        nof_entries--;
    }
    else
    {
        nof_entries = (32*1024);
    }
    src_bind_ent.port = pon_anti_spoofing_info.pon_gport;

    for(i = 0; i < nof_entries; i++)
    {
        src_bind_ent.ip +=i;
        rv = bcm_l3_source_bind_add(unit, &(src_bind_ent));
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_source_bind_add returned %s src_bind[%d]\n", bcm_errmsg(rv), i);
            if (is_dhcp)
            {
                ipv4_dhcp_entries = i;
            }
            else
            {
                ipv4_static_entries = i;
            }
            return rv;
        }
    }
    if (is_dhcp)
    {
        ipv4_dhcp_entries = i;
    }
    else
    {
        ipv4_static_entries = i;
    }

    return rv;
}

int del_sav_host_entry_stress_test(int unit, int is_dhcp)
{
    bcm_l3_source_bind_t src_bind_ent;
    bcm_l3_source_bind_t_init(&src_bind_ent);
    int rv = 0;
    int i = 0;
    int nof_entries = 0;

    src_bind_ent.ip = 16843009;
    if (is_dhcp)
    {
        src_bind_ent.mac[0] = 0x0;
        src_bind_ent.mac[1] = 0x0;
        src_bind_ent.mac[2] = 0x1;
        src_bind_ent.mac[3] = 0x0;
        src_bind_ent.mac[4] = 0x0;
        src_bind_ent.mac[5] = 0x1;
        nof_entries = ipv4_dhcp_entries;
    }
    else
    {
        nof_entries = ipv4_static_entries;
    }

    src_bind_ent.port = pon_anti_spoofing_info.pon_gport;

    for(i = 0; i < nof_entries; i++)
    {
        src_bind_ent.ip +=i;
        rv = bcm_l3_source_bind_delete(unit, &(src_bind_ent));
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_source_bind_delete returned %s src_bind[%d]\n", bcm_errmsg(rv), i);
            return rv;
        }
    }

    rv = bcm_l3_source_bind_enable_set(0, pon_anti_spoofing_info.pon_gport,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_source_bind_enable_set returned %s when disable anti-spoofing per LIF\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}


int pon_anti_spoofing_ipv4_host_stress_dvapi(int unit)
{

    int rv = 0;

    rv = add_sav_host_entry_stress_test(unit, 1);
    if (rv != BCM_E_FULL && rv != BCM_E_NONE)
    {
        printf("Error, dhcp add_sav_dhcp_entry_stress_test\n");
        return rv;
    }

    printf("ipv4_dhcp_entries:%d\n", ipv4_dhcp_entries);

    rv = add_sav_host_entry_stress_test(unit, 0);
    if (rv != BCM_E_FULL && rv != BCM_E_NONE)
    {
        printf("Error, static add_sav_dhcp_entry_stress_test\n");
        return rv;
    }
    printf("ipv4_static_entries:%d\n", ipv4_static_entries);

    rv = del_sav_host_entry_stress_test(unit, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, dhcp del_sav_dhcp_entry_stress_test\n");
        return rv;
    }
    ipv4_dhcp_entries = 0;

    rv = del_sav_host_entry_stress_test(unit, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, static del_sav_dhcp_entry_stress_test\n");
        return rv;
    }
    ipv4_static_entries = 0;

    return rv;
}


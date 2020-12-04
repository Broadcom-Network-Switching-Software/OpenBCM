/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_ip_tunnel_encap_basic.c Purpose: Various examples for IPTunnels.
 */

/*
 * Global flag. If set then 'virtual egress pointed' object is NOT created for
 * ARP entry (global lif is created and info is loaded into GLEM).
 * By default, 'virtual egress pointed' objects are used.
 * See ip_tunnel_encap_basic().
 */
int Virtual_egress_pointed_disabled = 0;
/*
 *
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/cint_ip_tunnel_encap_basic.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int provider_port = 201;
 * int access_port = 200;
 * rv = ip_tunnel_encap_basic(unit,access_port,provider_port);
 * print rv;
 *
 *
 *  Scenarios configured in this cint:
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  1) Basic IP Tunnel encapsulation
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  A GRE4oIPv4oETH tunnel header is being encapsulated.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ###############################################################################################################################
 *  Receiving packet on access port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         ||      IPv4 SIP     ||      IPv4 DIP      ||
 *   |    |00:0C:00:02:00:56||00:50:00:00:12:34||       1.2.3.4     ||    127.255.255.03  ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Sending packet on provider port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+
 *   |    |      DA         ||      SA         || IPv4 tunnel SIP || IPv4 tunnel DIP ||  GRE  ||  IPv4 SIP   ||   IPv4 DIP     ||
 *   |    |00:02:00:00:CD:1D||00:0C:00:02:00:56||    161.0.0.17   ||    160.0.0.17   ||   4   ||   1.2.3.4   || 127.255.255.03 ||
 *   |    |                 ||                 ||    A0.00.00.11  ||    A1.00.00.11  ||  800  || 01.02.03.04 ||  7f.ff.ff.03   ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  ###############################################################################################################################
 */
struct cint_ip_tunnel_basic_info_s
{
    int eth_rif_intf_provider;            /* out RIF */
    int eth_rif_intf_access;              /* in RIF */
    bcm_mac_t intf_access_mac_address;    /* mac for in RIF */
    bcm_mac_t tunnel_next_hop_mac;        /* mac for next hop */
    int access_vrf;                       /* VRF, resolved as Tunnel-InLif property*/
    int tunnel_fec_id;                    /* FEC id */
    int tunnel_arp_id;                    /* Id for ARP entry */
    bcm_ip_t route_dip;                   /* Route DIP */
    uint32 route_dip_mask;                /* mask for route DIP */
    bcm_ip_t tunnel_dip;                  /* tunnel DIP */
    bcm_ip_t tunnel_sip;                  /* tunnel SIP */
    bcm_tunnel_type_t tunnel_type;        /* tunnel type */
    int provider_vlan;                    /* VID assigned to the outgoing packet */
    int tunnel_ttl;                       /* Ip tunnel header TTL */
    int tunnel_dscp;                      /* Ip tunnel header differentiated services code point */
    int gre4_lb_key_enable;               /* Enable LB-Key on GRE IP tunnel */
    bcm_if_t tunnel_intf;                 /* Outlif ID */
    uint32 svtag_enable;                  /* enable SVTAG lookup */
};

cint_ip_tunnel_basic_info_s cint_ip_tunnel_basic_info =
{
    /*
     * eth_rif_intf_provider | eth_rif_intf_access
     */
       100,                    15,
    /*
     * intf_access_mac_address             | tunnel_next_hop_mac
     */
       {0x00, 0x0c, 0x00, 0x02, 0x00, 0x56}, {0x00, 0x02, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * access_vrf
     */
       1,
    /*
     * tunnel_fec_id
     */
       45000,
    /*
     * tunnel_arp_id
     */
       0,
    /*
     * route_dip
     */
       0x7fffff03 /* 127.255.255.03 */,
    /*
     * route_dip_mask
     */
       0xfffffff0,
    /*
     * tunnel DIP
     */
       0xA1000011 /* 161.0.0.17 */,
    /*
     * tunnel SIP
     */
       0xA0000011 /* 160.0.0.17 */,
    /*
     * tunnel type
     */
       bcmTunnelTypeGreAnyIn4,
    /*
     * provider vlan
     */
       100,
    /*
     * tunnel_ttl
     */
       64,
    /*
     * tunnel_dscp
     */
       10,
    /*
     * gre4_lb_key_enable
     */
       0,
    /*
     * tunnel_intf
     */
       0x17453,
    /*
     * svtag_enable
     */
       0
};

int g_tunnel_id;

int
ip_tunnel_encap_vxlan_gpe_basic(
    int unit,
    int access_port,
    int provider_port)
{
    int rv = BCM_E_NONE;
    cint_ip_tunnel_basic_info.tunnel_type = bcmTunnelTypeVxlanGpe;
    rv = ip_tunnel_encap_basic(unit, access_port, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_encap_basic \n");
        return rv;
    }
    rv = vxlan_network_domain_management_l3_egress(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vxlan_network_domain_config_add \n");
        return rv;
    }
    return rv;

}

int
vxlan_network_domain_management_l3_egress(
    int unit)
{
    int rv;
    bcm_vxlan_network_domain_config_t config;
    bcm_vxlan_network_domain_config_t_init(&config);
    /*
     * Update network domain for the created VNI->VRF
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING;
    config.vrf              = cint_ip_tunnel_basic_info.access_vrf;
    config.vni              = 5000;
    config.network_domain   = 0;

    rv = bcm_vxlan_network_domain_config_add(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vxlan_network_domain_config_add \n");
        return rv;
    }
    return rv;
}

/**update l3 fodo entry qos profile for IP forward header remark*/
int
vxlan_network_domain_management_l3_egress_update_qos(int unit, int qos_map_id)
{
    int rv;
    bcm_vxlan_network_domain_config_t config;
    bcm_vxlan_network_domain_config_t_init(&config);
    /*
     * Update network domain for the created VNI->VRF
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING;
    config.vrf              = cint_ip_tunnel_basic_info.access_vrf;
    config.network_domain   = 0;

    rv = bcm_vxlan_network_domain_config_get (unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vxlan_network_domain_config_add \n");
        return rv;
    }
    /**replace qos map id*/
    config.flags            |= BCM_VXLAN_NETWORK_DOMAIN_CONFIG_REPLACE;
    config.qos_map_id = qos_map_id;
    rv = bcm_vxlan_network_domain_config_add(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vxlan_network_domain_config_add \n");
        return rv;
    }
    return rv;
}

/*
 * Get indication on whether 'virtual egress pointed' objects are created for
 * ARP entry (If 'false' then global lif is created and info is loaded into GLEM).
 * See ip_tunnel_encap_basic().
 */
int
is_virtual_egress_pointed_enabled(int unit)
{
    int ret;
    ret = !Virtual_egress_pointed_disabled;
    return (ret);
}
/*
 * Set control to do create 'virtual egress pointed' objects for ARP entries (If 
 * 'enable_virtual_egress_pointed' is set to 'false' then global lif is created
 * and info is loaded into GLEM).
 * See ip_tunnel_encap_basic(). See AT_Dnx_Cint_l3_ip_tunnel_gre4_encapsulation_egress_bringup()
 */
int
set_enable_virtual_egress_pointed(int unit, int enable_virtual_egress_pointed)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "set_enable_virtual_egress_pointed";
    Virtual_egress_pointed_disabled = !enable_virtual_egress_pointed;
    printf("\r\n");
    printf("%s(): 'virtual egress pointed' is now %s \r\n", proc_name, (Virtual_egress_pointed_disabled ? "Disabled" : "Enabled"));
    printf("\r\n");
    return (rv);
}
/*
 * Main function for basic ip  tunnel encapsulation scenarios in Jericho 2
 * The function ip_tunnel_encap_basic implements the basic tunnel encapsulation scenario.
 * Inputs:
 * unit - relevant unit;
 * access_port - incoming port;
 * provider_port - outgoing port;
 * Note that l3__egress_only_encap__create() is always invoked with flags set
 * to not NOT load GLEM when creating ARP entry
 */
int
ip_tunnel_encap_basic(
    int unit,
    int access_port,
    int provider_port)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    int device_is_qux;
    char *proc_name;
    l3_ingress_intf ingr_itf;
    uint32 allocation_flags;
    int fec;

    l3_ingress_intf_init(&ingr_itf);
    allocation_flags = 0;
    proc_name = "ip_tunnel_encap_basic";

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2 device_is_jericho2 \n", proc_name);
        return rv;
    }

    rv = is_qux_only(unit, &device_is_qux);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2 device_is_jericho2 \n", proc_name);
        return rv;
    }    

    if(device_is_jericho2)
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        cint_ip_tunnel_basic_info.tunnel_fec_id += fec;
    }
    else if (device_is_qux)
    {
        cint_ip_tunnel_basic_info.tunnel_fec_id = 22500;
    }

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, access_port, cint_ip_tunnel_basic_info.eth_rif_intf_access);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set\n", proc_name);
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set provider_port\n", proc_name);
        return rv;
    }

    /*
     * Create routing interface for the routing's IP.
     * We use it as ingress ETH-RIF to perform ETH termination (my-mac procedure),
     * to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_access,
                             cint_ip_tunnel_basic_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_access\n", proc_name);
        return rv;
    }

    /*
     * Create egress routing interface used for routing after the tunnel encapsulation.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_provider,
                             cint_ip_tunnel_basic_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_provider\n", proc_name);
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     */
    ingr_itf.intf_id = cint_ip_tunnel_basic_info.eth_rif_intf_access;
    ingr_itf.vrf = cint_ip_tunnel_basic_info.access_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set eth_rif_intf_access\n", proc_name);
    }
    /*
     * Create ARP entry and set its properties.
     */
    if (is_virtual_egress_pointed_enabled(unit))
    {
        /*
         * Always set set flags to NOT load GLEM when creating ARP entry
         */
        allocation_flags |= BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED;
    }
    rv = l3__egress_only_encap__create(unit, allocation_flags,&(cint_ip_tunnel_basic_info.tunnel_arp_id),
                                       cint_ip_tunnel_basic_info.tunnel_next_hop_mac,
                                       cint_ip_tunnel_basic_info.provider_vlan);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel initiator
     * and set next-lif-pointer to the ARP entry
     */
    rv = tunnel_initiator_create(unit, cint_ip_tunnel_basic_info.tunnel_arp_id, &cint_ip_tunnel_basic_info.tunnel_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function tunnel_initiator_configure \n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF
     */
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.tunnel_fec_id, cint_ip_tunnel_basic_info.tunnel_intf,
                                     0, provider_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
    }

    /*
     * Add Host entry
     */
    int fec_id;
    BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_ip_tunnel_basic_info.tunnel_fec_id);
    rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.route_dip,
                       cint_ip_tunnel_basic_info.access_vrf, fec_id, 0,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4, \n");
        return rv;
    }

    return rv;
}

int
tunnel_initiator_create(
    int unit,
    bcm_if_t arp_itf,
    bcm_if_t *tunnel_intf)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;
    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
        return rv;
    }

    bcm_l3_intf_t_init(&l3_intf);

    /* Create IP tunnel initiator for encapsulating GRE4oIPv4 tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip        = cint_ip_tunnel_basic_info.tunnel_dip;
    tunnel_init.sip        = cint_ip_tunnel_basic_info.tunnel_sip;
    tunnel_init.flags      = 0;
    tunnel_init.dscp       = cint_ip_tunnel_basic_info.tunnel_dscp;
    tunnel_init.type       = cint_ip_tunnel_basic_info.tunnel_type;
    tunnel_init.ttl        = cint_ip_tunnel_basic_info.tunnel_ttl;
    tunnel_init.l3_intf_id = arp_itf;
    if (device_is_jericho2)
    {
        tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        if(cint_ip_tunnel_basic_info.svtag_enable)
        {
            tunnel_init.flags |= BCM_TUNNEL_INIT_SVTAG_ENABLE;
        }
    }
    else
    {
        /*In Jr2, vlan is an ARP property */
        tunnel_init.dscp_sel = bcmTunnelDscpAssign;
        tunnel_init.vlan = cint_ip_tunnel_basic_info.provider_vlan;
    }
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */

    if (*tunnel_intf > 0)
    {
        tunnel_init.flags |= BCM_TUNNEL_WITH_ID;
        BCM_GPORT_TUNNEL_ID_SET(tunnel_init.tunnel_id, *tunnel_intf);
    }

    rv = bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }

    *tunnel_intf = l3_intf.l3a_intf_id;
    g_tunnel_id = *tunnel_intf;

    return rv;
}

/*IP tunnel qos update main function*/
int ip_tunnel_encap_basic_qos_update(int unit, int qos_map_id, int qos_model, int dscp, int ecn_eligible)
{
    int rv = BCM_E_NONE;
    int flags;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not intersting */
    bcm_l3_intf_t l3_intf;
    bcm_tunnel_initiator_t tunnel_init;

    bcm_tunnel_initiator_t_init(&tunnel_init);
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = g_tunnel_id;
    rv= bcm_tunnel_initiator_get(unit, &l3_intf, &tunnel_init);
    if (rv) {
        printf("error bcm_tunnel_initiator_get()\n");
        return rv;
    }
    if(dscp != -1)
    {
      tunnel_init.dscp = dscp;  
    }
    if(ecn_eligible)
    {
        tunnel_init.egress_qos_model.egress_ecn = bcmQosEgressEcnModelEligible;
    }
    tunnel_init.egress_qos_model.egress_qos = qos_model;
    tunnel_init.flags |= (BCM_TUNNEL_WITH_ID |  BCM_TUNNEL_REPLACE);
    rv = bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }
    /*update ARP qos map id in order to remark IP tunnle TOS*/
    if (qos_map_id != -1)
    {
        bcm_l3_egress_t_init(&l3eg);
        l3eg.encap_id = cint_ip_tunnel_basic_info.tunnel_arp_id;
        rv = bcm_l3_egress_get(unit,cint_ip_tunnel_basic_info.tunnel_arp_id,&l3eg);
        if (rv) {
            printf("error bcm_l3_egress_get()\n");
            return rv;
        }
        l3eg.qos_map_id = qos_map_id;
        flags = BCM_L3_REPLACE | BCM_L3_WITH_ID | BCM_L3_EGRESS_ONLY;
        rv = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid_null);
        if (rv) {
            printf("error bcm_l3_egress_create()\n");
            return rv;
        }
    }

    return rv;
}


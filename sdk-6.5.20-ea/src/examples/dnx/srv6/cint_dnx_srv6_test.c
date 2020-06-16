/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * This CINT use below illustration as an example
 *  +-+-+-+-+-+-+-+
 *  |      ARP    |
 *  +-+-+-+-+-+-+-+
 *  | SRv6 tunnel |
 *  +-+-+-+-+-+-+-+
 *  |  SRH base   |
 *  +-+-+-+-+-+-+-+
 *  |     SID0    |  -> it's called last SID or final SID in configuration
 *  +-+-+-+-+-+-+-+
 *  |     SID1    |
 *  +-+-+-+-+-+-+-+
 *  |     SID2    |
 *  +-+-+-+-+-+-+-+
 *  |     SID3    |
 *  +-+-+-+-+-+-+-+
 *  |   IP/ETH    |
 *  +-+-+-+-+-+-+-+
*/

/*
 * Required Cints:
 * cint ../../../../src/examples/dnx/utility/cint_dnx_util_srv6.c
 */


struct srv6_stat_s
{
    int stat_id;
    int pp_profile_id;
    int counter_command;
    int engine_id;
    int database_id;
    int is_etpp;
};

srv6_stat_s srv6_stat_properties =
{
    /**SRH base LIF stat_id */
    1,
    /**pp_profile_id*/
    1,
    /**counter_command*/
    1,
    /**engine_id*/
    1,
    /**database_id*/
    1,
    /**ETPP or IRPP stat */
    0
};

/* Init struc for replace tests */
bcm_srv6_sid_initiator_info_t sid_encap_info;
bcm_srv6_srh_base_initiator_info_t srh_base_encap_info;
bcm_gport_t srv6_tunnel_id;
bcm_gport_t last_sid_tunnel_id;
bcm_gport_t egress_native_ac;

/* Init new SIDs for replace test */
bcm_ip6_t new_SID[4] = 
{{ 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0xee, 0xee, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 },
 { 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0xee, 0xee, 0xff, 0xff, 0x11, 0x11, 0x11, 0x11 },
 { 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0xee, 0xee, 0xff, 0xff, 0x22, 0x22, 0x22, 0x22 },
 { 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0xee, 0xee, 0xff, 0xff, 0x33, 0x33, 0x33, 0x33 }};

/* Global VLAN action id */
int eve_action_id = 3;

/* Global vars for egress qos test */
int egress_qos_model;
int egress_ttl_model;
int egress_qos_val;
int egress_ttl_val;

/*
 *    Configure CRPS entry and bind stat PP profile to LIF
 */
int
srv6_stat_config(int unit, bcm_gport_t gport_id) {

    int rv = BCM_E_NONE;
    bcm_stat_pp_info_t stat_info;
    bcm_stat_pp_profile_info_t pp_profile;
    int pp_profile_id;

    cint_stat_pp_config_info_default.pp_profile = srv6_stat_properties.pp_profile_id;
    cint_stat_pp_config_info_default.counter_command = srv6_stat_properties.counter_command;
    cint_stat_pp_config_info_default.crps_engine_id = srv6_stat_properties.engine_id;
    cint_stat_pp_config_info_default.database_id = srv6_stat_properties.database_id;
    cint_stat_pp_config_info_default.stat_id = srv6_stat_properties.stat_id;
    cint_stat_pp_config_info_default.expansion_flag = 1;

    /* Configure crps entry */
    rv = cint_stat_pp_config_crps(unit, cint_stat_pp_config_info_default,
                                  (srv6_stat_properties.is_etpp?bcmStatCounterInterfaceEgressTransmitPp:bcmStatCounterInterfaceIngressReceivePp)
                                  , BCM_CORE_ALL);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_stat_pp_config_crps\n");
        return rv;
    }

    /** Configure pp profile */
    rv = cint_stat_pp_create_pp_profile(unit, cint_stat_pp_config_info_default,
                                        (srv6_stat_properties.is_etpp?bcmStatCounterInterfaceEgressTransmitPp:bcmStatCounterInterfaceIngressReceivePp)
                                        , &pp_profile, &pp_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_stat_pp_create_pp_profile\n");
        return rv;
    }

    /** Bind profile to lif */
    stat_info.stat_pp_profile = pp_profile_id;
    stat_info.stat_id = srv6_stat_properties.stat_id;
    rv = bcm_gport_stat_set(unit, gport_id, BCM_CORE_ALL,
                            (srv6_stat_properties.is_etpp?bcmStatCounterInterfaceEgressTransmitPp:bcmStatCounterInterfaceIngressReceivePp)
                            , stat_info);
    if (rv != BCM_E_NONE)
    {
         printf("Error in cint_stat_pp_create_gport_stat_object\n");
    }

    return rv;
}

/*
 *    Configuring ISR (Ingress SRv6 Tunnel)
 *    This will create encapsulation of SRH,SIDs and IPv4/IPv6/Bridge onto the packet
 *    The difference between this cint with srv6_ingress_tunnel_config is that:
 *        Only one global out lif is used, in egress, the EEDB encapsulation is:
 *        SRH Base -> SID0 -> SID1 -> SID2 -> SID3 -> SRv6 Tunnel -> ARP+AC
 */
int
srv6_ingress_tunnel_test(
                            int unit,
                            int in_port,             /** Incoming port of SRV6 Ingress Tunnel */
                            int out_port,            /** Outgoing port of SRV6 Ingress Tunnel */
                            int nof_sids)
{

    int rv;
    bcm_mac_t dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x30};
    char *proc_name = "srv6_ingress_tunnel_test";

    if (nof_sids < 2 || nof_sids > 4)
    {
        printf("%s(): Error, cannot use nof_sids (%d), it has to be 2 to 4 !\n",proc_name, nof_sids);
        return BCM_E_PARAM;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */

    /** SRV6 Ingress Tunnel Eth RIF based on Port only */
    rv = in_port_intf_set(unit, in_port, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port of SRV6 Ingress Tunnel %d\n",proc_name,out_port);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */

    /** SRV6 Ingress Tunnel My-MAC */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS], cint_srv6_tunnel_info.intf_in_mac_address[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
        return rv;
    }

    /** SRV6 Ingress Tunnel out-port SA  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */

    l3_ingress_intf ingress_rif;

    /** SRV6 Ingress Tunnel VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[INGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /*
     * 5. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     *    We use same port out for the End-Points, therefore the VLAN ID is what separates these cases.
     */

    uint32 flags2 = 0;

    /* allow 2 TAG add */
    flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION | BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS;

    /** SRV6 Ingress Tunnel ARP create, with VLAN ID of expected in End-Point-1*/
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /*
     * 6. SRV6 FEC Entries
     */

    /*
     * Update FEC IDS
     */
    rv = srv6_update_fecs_values(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, srv6_get_fec_values\n");
        return rv;
    }

    bcm_gport_t gport;

    int srv6_basic_lif_encode;
    int encoded_fec1;
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, out_port);

    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], srv6_basic_lif_encode, 0, gport, 0,
                                           0, 0,&encoded_fec1);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC_1 only\n",proc_name);
        return rv;
    }

    /*
     * We don't need 2nd FEC here since in egress SRH base points to the 1st SID
     */

    /*
     * 7. Add route entry - we work with specific DIP addresses, so we don't need to add best match route entries.
     */

    /*
     * 8. Add IPv4 host entry
     */

    /** SRV6 Tunnel Ingress Forwarding to FEC to bring out IPV6, SRH*/
    add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], encoded_fec1, 0, 0);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4() for SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /*
     * 9. Add IPv6 host entry
     */
    rv = add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, cint_srv6_tunnel_info.vrf_in[INGRESS], encoded_fec1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv6: vrf = %d, fec = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.vrf_in[INGRESS], encoded_fec1, rv);
        return rv;
    }

    /*
     * 10. Add MACT entry
     */
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);

    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, dmac, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    l2_addr.port = gport;
    l2_addr.flags = BCM_L2_STATIC;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function bcm_l2_addr_add\n", proc_name);
        return rv;
    }

    /*
     * 11. Configuring ISR SRv6 Tunnel
          creating EEDB entries for SRH Base -> SIDs[0..2] -> IPv6 Tunnel (SRv6 Type)
     */

    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeSR6;
    tunnel_init_set.l3_intf_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS];
    tunnel_init_set.flags = BCM_TUNNEL_INIT_STAT_ENABLE; /* Enable PP STAT */
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);

    /* SRv6 tunnel QoS & TTL, initialize as uniform */
    tunnel_init_set.ttl = egress_ttl_val;
    tunnel_init_set.dscp = egress_qos_val;
    tunnel_init_set.egress_qos_model.egress_qos = egress_qos_model;
    tunnel_init_set.egress_qos_model.egress_ttl = egress_ttl_model;

    rv = bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set);

    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_tunnel_initiator_create for IPv6 Tunnel \n");
       return rv;
    }

    /* Save SRv6 tunnel ID for replace purpose */
    srv6_tunnel_id = tunnel_init_set.tunnel_id;

    /*
     * define the x3 SIDs  EEDB entries (linked to one another) and then link to an IPv6 EEDB entry
     */

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t sid_info;

    /** to store next SID GPORT and init for last SID (sid_idx = 0 in below config) to IPv6 Tunnel */
    bcm_gport_t next_sid_tunnel_id = tunnel_init_set.tunnel_id;

    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */

    int sid_idx;

    for (sid_idx = 0; sid_idx < nof_sids; sid_idx++)
    {

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (sid_idx != 0)
        {
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /** if this is the final SID to add, this SID is the first SID in list and has the global-LIF to the SID-LL*/
        if (sid_idx == nof_sids - 1)
        {
            /** we will pass the global-out-LIF id for 1st SID */
            sid_info.flags = BCM_SRV6_SID_INITIATOR_WITH_ID;

            /** convert 1st SID global-LIF id to GPORT */
            BCM_GPORT_TUNNEL_ID_SET(sid_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_FIRST_SID_GLOBAL_LIF]);
        }
        else
        {
            /** don't use any special flags */
            sid_info.flags = 0;

            /** must set the tunnel id to 0 if not using WITH_ID flag */
            sid_info.tunnel_id = 0;
        }


        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0 - sid_idx], 16);

        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_idx;

        /** next_encap_id set to IPv6 interface - convert it from GPORT to l3_int */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        rv = bcm_srv6_sid_initiator_create(unit, &sid_info);

        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_dnx_srv6_sid_initiator_create for SID%d \n", sid_idx);
           return rv;
        }

        /** save the last sid encap id , used for replace */
        if (sid_idx == 0) {
            last_sid_tunnel_id = sid_info.tunnel_id;
        }
    }

    /*
     * define the SRH Base EEDB entry
     */

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;

    /** we will pass the global-out-LIF id*/
    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);

    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids;

    /* Set SRH base next_encap_id to be 1st SID */
    BCM_GPORT_TUNNEL_ID_SET(gport, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_FIRST_SID_GLOBAL_LIF]);
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, gport);

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);

    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_dnx_srv6_srh_base_initiator_create \n");
       return rv;
    }

    
    /*
     * 12. Add egress native AC for native EVE
     */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS;
    vlan_port.vsi = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    vlan_port.port = srh_base_info.tunnel_id;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }

    /* Save native ac for EVE */
    egress_native_ac = vlan_port.vlan_port_id;

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/* Used in replace test */
int
srv6_sid_encap_create(int unit) {
    int rv;
    rv = bcm_srv6_sid_initiator_create(unit, &sid_encap_info);

    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_dnx_srv6_sid_initiator_create for SID%d \n", sid_encap_info.sid);
       return rv;
    }
}

/* Used in replace test */
int
srv6_srh_base_encap_create(int unit) {
    int rv;
    rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_encap_info);

    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_srv6_srh_base_initiator_create\n");
       return rv;
    }
}

/*
 *    Configuring ESR (Egress SRv6 Tunnel), two modes
 *       USP: 1st PASS, direct traffic to RCY port, terminate SRv6 tunnel & SRH
 *            2nd PASS, FWD based on ETH/IPv4/IPv6
 *       PSP: enable SOC appl_param_psp_reduced_mode, terminate SRH & forward
 */
int
srv6_egress_tunnel_test(
    int unit,
    int in_port,
    int rch_port,
    int out_port)
{
    int rv;
    int in_vlan = 15;
    int out_vlan = 100;
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int vrf = 1;                /* RCH VRF 2nd Pass is equal here to 1st Pass VRF */
    int vsi = 15;
    int encap_id = 0x1384;
    int fec;
    int encoded_fec;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 host = 0x7fffff02;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    bcm_ip6_t default_ipv6_route ={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    bcm_ip6_t default_ipv6_mask = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    char *proc_name;

    proc_name = "srv6_egress_tunnel_test";

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, vsi);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,out_port);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,intf_in);
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 5. Create ARP and set its properties
     */

    printf("%s(): Going to call l3__egress_only_encap__create_inner() with encap_id 0x%08X, flags2 0x%08X\n",
                proc_name, encap_id, flags2);
    rv = l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, out_vlan, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }

    /*
     * 6. Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */

    /* Get FEC ID according to the current static FEC allocation  */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    printf("%s(): Create main FEC and set its properties.\r\n",proc_name);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create_inner(unit, fec, intf_out , encap_id, gport, 0,
                                           0, 0,&encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n",proc_name);
        return rv;
    }
    printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);

    /*
     * 7. Add IPv4 host entry
     */
    rv = add_host_ipv4(unit, host, vrf, encoded_fec, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
        return rv;
    }

    /*
     * 8. Add IPv6 fwd entries, for PSP serves as next SID lookup, for USP serves as 2nd pass lookup
     */
    rv = add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, vrf, encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv6: vrf = %d, fec = %d, err_id = %d\n", proc_name, vrf, encoded_fec, rv);
        return rv;
    }

    /* route */
     rv = add_route_ipv6(unit, cint_ip_route_basic_ipv6_route, cint_ip_route_basic_ipv6_mask, vrf, encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_route_ipv6: vrf = %d, fec = %d, err_id = %d\n", proc_name, vrf, encoded_fec, rv);
        return rv;
    }

    /* default route */
     rv = add_route_ipv6(unit, default_ipv6_route, default_ipv6_mask, vrf, encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_route_ipv6: vrf = %d, fec = %d, err_id = %d\n", proc_name, vrf, encoded_fec, rv);
        return rv;
    }

    /*
     * 9. Add MACT entry
     */
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, cint_srv6_tunnel_info.l2_termination_mact_fwd_address, vsi);
    l2_addr.port = out_port;
    l2_addr.flags = BCM_L2_STATIC;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function bcm_l2_addr_add\n", proc_name);
        return rv;
    }

    /*
     * 10. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    rv = out_port_set(unit, rch_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set recycle_port \n");
        return rv;
    }


    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    bcm_srv6_extension_terminator_mapping_t terminator_info;
    int sid_idx;

    terminator_info.flags = 0;

    /** Convert Port number to GPORT */
    BCM_GPORT_LOCAL_SET(terminator_info.port, rch_port);

    int min_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "min_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));

    /** create LIF entries for all possible nof_sids in an SRv6 packet that would lead to RCY port + RCH Encapsulation*/
    for (sid_idx = min_nof_terminated_usp_sids; sid_idx <= max_nof_terminated_usp_sids; sid_idx++)
    {
        /*
         * Create RCH encapsulation per each NOF SIDs, to take into account relevant:
         * -recycle_header_extension_length (bytes to remove on 2nd pass at RCH termination)
         * -additional_egress_termination_size (additional bytes to remove on 1st pass egress - additional to parsing start offset value)
         */
        int recycle_entry_encap_id;
        /** Create entry, which will be built the recycle header for the copy going on the 2nd pass*/
        rv = srv6_create_extended_termination_recycle_entry(unit, sid_idx, &recycle_entry_encap_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, srv6_create_extended_termination_recycle_entry \n");
            return rv;
        }

        /** Convert from L3_ITF to GPORT */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(terminator_info.encap_id , recycle_entry_encap_id);

        terminator_info.nof_sids = sid_idx;

        /** call the nof_sids API to map to RCH port and encapsulation */
        rv = bcm_srv6_extension_terminator_add(unit, &terminator_info);

        if (rv != BCM_E_NONE)
        {
             printf("Error, bcm_srv6_extension_terminator_add for nof_sids:%d\n", sid_idx);
             return rv;
        }

     } /** of for sid_idx*/


    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

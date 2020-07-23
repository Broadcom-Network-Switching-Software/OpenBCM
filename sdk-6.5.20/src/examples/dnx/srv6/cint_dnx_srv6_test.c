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
 *        SRH Base -> SID3 -> SID2 -> SID1 -> SID0 -> SRv6 Tunnel -> ARP+AC
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
    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_port_translation_t port_trans;
    int ive_edit_profile = 5;
    int action_id = 6;
    int new_outer_vid = 10;
    int new_inner_vid = 9;
    int tag_format = 8; /* default 0x8100 */
    int in_ac;

    char *proc_name = "srv6_ingress_tunnel_test";

    if (nof_sids < 2 || nof_sids > 4)
    {
        printf("%s(): Error, cannot use nof_sids (%d), it has to be 2 to 4 !\n",proc_name, nof_sids);
        return BCM_E_PARAM;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.vsi = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vlan_port_create\n", proc_name);
        return rc;
    }
    in_ac = vlan_port.vlan_port_id;

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

    /*
     * 13. Add ingress AC IVE
     */
    /* set edit profile for ingress/egress LIF */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = new_outer_vid;
    port_trans.new_inner_vlan = new_inner_vid;
    port_trans.gport = in_ac;
    port_trans.vlan_edit_class_id = ive_edit_profile;
    port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    /* Create action ID */
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /* Set translation action class (map edit_profile & tag_format to action_id) */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = ive_edit_profile;
    action_class.tag_format_class_id = tag_format;
    action_class.vlan_translation_action_id = action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

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

    /** Configure RCH port of Extended Termination type */
    rv = bcm_port_control_set(unit, rch_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_control_set for rch_port %d\n",proc_name, rch_port);
        return rv;
    }

    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    bcm_srv6_extension_terminator_mapping_t terminator_info;
    int sid_idx;

    terminator_info.flags = 0;

    /** Convert Port number to GPORT */
    BCM_GPORT_LOCAL_SET(terminator_info.port, rch_port);

    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;

    /** create LIF entries for all possible nof_sids in an SRv6 packet that would lead to RCY port + RCH Encapsulation*/
    for (sid_idx = 0; sid_idx < nof_2pass_recycle_entries ; sid_idx++)
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

        terminator_info.nof_sids = sid_idx + max_nof_terminated_usp_sids_1pass + 1;

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

/*
 *    Configuring SRv6 usid endpoint function
 */
int
srv6_usid_endpoint_example(
                    int unit,
                    int in_port,        /** Incoming port of SRV6 End Point */
                    int out_port,       /** Outgoing port of SRV6 End Point */
                    int srh_present,    /** If set, SRH header is present in uSID packet */
                    int tunnel_type)
{
    int rv;
    char *proc_name = "srv6_usid_endpoint_example";
    bcm_ip6_t ipv6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /*
     * 0. Set legal FEC IDs
     */
    rv = dnx_srv6_update_fecs_id(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, dnx_srv6_update_fecs_id\n",proc_name);
        return rv;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    /** End-Point-1 Eth RIF based on Port + VLAN ID so that we can re-use same port */
    rv = in_port_vid_intf_set(unit, in_port, cint_srv6_tunnel_info.tunnel_vid[END_POINT_1_VID], cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set in_port\n",proc_name);
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
    /** End-Point-1 My-MAC - set to SRV6 Ingress Tunnel next hop MAC */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1], cint_srv6_tunnel_info.intf_in_mac_address[END_POINT_1]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1]);
        return rv;
    }

    /** End-Point-1 out-port SA  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[END_POINT_1], cint_srv6_tunnel_info.intf_out_mac_address[END_POINT_1]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /** Egress My-MAC - set to End-Point-2 next hop MAC */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], rv);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */

    l3_ingress_intf ingress_rif;
    /** End-Point-1 VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[END_POINT_1];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create End-Point-1\n",proc_name);
        return rv;
    }

    /*
     * 5. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     *    We use same port out for the End-Points, therefore the VLAN ID is what separates these cases.
     */

    uint32 flags2 = 0;

    flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;

    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[END_POINT_1], cint_srv6_tunnel_info.arp_next_hop_mac[END_POINT_1], cint_srv6_tunnel_info.tunnel_vid[EGRESS_VID], 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in End-Point-1\n",proc_name);
        return rv;
    }

    /*
     * 6. SRV6 FEC Entries
     */
    bcm_gport_t gport;
    /** End-Point-1 FEC Entry - tying end-point-1 OUT-RIF with its OUT_Port, (ARP - DA and VLAN ID) */
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[END_POINT_1], cint_srv6_tunnel_info.tunnel_arp_id[END_POINT_1], gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf
            ("%s(): Error, create egress object FEC only for End-Point-1: fec = %d, intf_out = %d, encap_id = %d, out_port = %d, err_id = %d\n",
             proc_name, cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[END_POINT_1], cint_srv6_tunnel_info.tunnel_arp_id[END_POINT_1], out_port, rv);
        return rv;
    }

    /*
     * 7. Add route and host entry
     */
    /** add End-Point-1's next IP (u-SID) as host to do forwarding on to FEC */
    rv = add_route_ipv6(unit, cint_srv6_tunnel_info.tunnel_ip6_dip[END_POINT_1_DIP], ipv6_mask, cint_srv6_tunnel_info.vrf_in[END_POINT_1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function %s: vrf = %d, fec = %d, err_id = %d \n", proc_name, vrf, fec, rv);
        return rv;
    }

    /*
     * 8. End-Point configurations
     */
    /** End-Point-1: Add MyDIP of current End-Point, getting same VRF for all End-Points */
    srv6_tunnel_termination(unit, END_POINT_1_DIP, 0, srh_present, cint_srv6_tunnel_info.vrf_in[END_POINT_1], tunnel_type);

    /** End-Point-2: Add MyDIP of current End-Point, getting same VRF for all End-Points */
    srv6_tunnel_termination(unit, END_POINT_2_DIP, 0, srh_present, cint_srv6_tunnel_info.vrf_in[END_POINT_1], tunnel_type);

    /** End-Point-3: Add MyDIP of current End-Point, getting same VRF for all End-Points */
    srv6_tunnel_termination(unit, END_POINT_3_DIP, 0, srh_present, cint_srv6_tunnel_info.vrf_in[END_POINT_1], tunnel_type);

    /** End-Point-4: Add MyDIP of current End-Point, getting same VRF for all End-Points */
    srv6_tunnel_termination(unit, END_POINT_4_DIP, 0, srh_present, cint_srv6_tunnel_info.vrf_in[END_POINT_1], tunnel_type);

    /** End-Point-5: Add MyDIP of current End-Point, getting same VRF for all End-Points */
    srv6_tunnel_termination(unit, END_POINT_5_DIP, 0, srh_present, cint_srv6_tunnel_info.vrf_in[END_POINT_1], tunnel_type);

    printf("%s(): Exit\r\n",proc_name);
    return 0;
}

/*
 *    Configuring SRv6 usid egress usp cross connect
 */
int
srv6_usid_egress_cross_connect(
                    int unit,
                    int in_port,      /** Incoming port of SRV6 Egress */
                    int rch_port,     /** Rycling port number for 1st Pass USP in Egress */
                    int out_port,     /** Outgoing port of SRV6 Egress */
                    int srh_present)  /** If set, SRH header is present in uSID packet */
{
    int rv;
    char *proc_name = "srv6_usid_egress_cross_connect";
    bcm_tunnel_type_t tunnel_type;

    if (srh_present)
    {
        tunnel_type = bcmTunnelTypeSR6;
    }
    else
    {
        tunnel_type = bcmTunnelTypeIpAnyIn6;
    }

    /*
     * 0. Set legal FEC IDs
     */
    rv = dnx_srv6_update_fecs_id(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, dnx_srv6_update_fecs_id\n",proc_name);
        return rv;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    /** Egress Eth RIF based on Port + VLAN ID */
    rv = in_port_vid_intf_set(unit, in_port, cint_srv6_tunnel_info.tunnel_vid[EGRESS_VID], cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set in_port = %d, intf_in = %d, err_id = %d\n", proc_name, in_port, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS],
               rv);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    /** Egress out_port */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_intf_set out_port = %d, err_id = %d\n", proc_name, out_port, rv);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    /** Egress My-MAC - set to End-Point-2 next hop MAC */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], rv);
        return rv;
    }

    /** RCH Eth_Rif My MAC  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[RCH], cint_srv6_tunnel_info.intf_in_mac_address[RCH]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[RCH]);
        return rv;
    }

    /** Egress out-port SA  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], cint_srv6_tunnel_info.intf_out_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], rv);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf ingress_rif;
    /** Egress Tunnel - 1st pass VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[EGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, vrf = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.vrf_in[EGRESS], rv);
        return rv;
    }

    /** Egress Tunnel - RCH - 2nd pass VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[RCH];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[RCH];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 5. Create ARP and set its properties
     */
    /** Egress ARP create, with VLAN ID of Egress Tunnel, however there we will look on only the Port */
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[EGRESS], cint_srv6_tunnel_info.tunnel_vid[EGRESS_OUT_VID], 0,0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only: encap_id = %d, vsi = %d, err_id = %d\n", proc_name,
                cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], cint_srv6_tunnel_info.tunnel_vid[EGRESS_OUT_VID], rv);
        return rv;
    }

    /*
     * 6. SRV6 FEC Entries
     */

    bcm_gport_t gport;
    /*
     * On 1st Pass Egress, no FEC entry  because no more SIDs left to enter FWD on and then FEC,
     * in VTT5 there's LIF P2P which gives the destination right away
     */
    /** Egress 2nd Pass FEC Entry - due to Forwarding on IPv4 Layer above SRv6  */
    int egress_encoded_fec1;
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS_OUT_VID], cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], gport, 0, 0, 0,&egress_encoded_fec1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n",proc_name);
        return rv;
    }

    /*
     * 7. Cross connect, no host/route entry needed
     */
    /*
     * 8. Configure the SRv6 ESR USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */
    {

        /** Set Out-Port default properties for the recycled packets*/
        rv = out_port_set(unit, rch_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, out_port_set recycle_port \n");
            return rv;
        }

        /** Configure RCH port of Extended Termination type */
        rv = bcm_port_control_set(unit, rch_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, bcm_port_control_set for rch_port %d\n",proc_name, rch_port);
            return rv;
        }

        /** define the structure which holds the mapping between nof_sids to RCH port and encap_id */
        bcm_srv6_extension_terminator_mapping_t extension_terminator_info;

        int sid_idx; /** sid_idx is used to fill LIF table for all possible number of SIDs in an SRv6 packet */

        extension_terminator_info.flags = 0;
        /** Convert Port number to GPORT */
        BCM_GPORT_LOCAL_SET(extension_terminator_info.port, rch_port);


        int max_sid_idx;
        max_sid_idx = *dnxc_data_get(unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL);
        int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp_1pass", NULL));
        int nof_2pass_recycle_entries = max_sid_idx - max_nof_terminated_usp_sids_1pass;

        /** create LIF entries for all possible nof_sids in an SRv6 packet that would lead to RCY port + RCH Encapsulation*/
        for (sid_idx = 0; sid_idx < nof_2pass_recycle_entries; sid_idx++)
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
            BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(extension_terminator_info.encap_id , recycle_entry_encap_id);

            extension_terminator_info.nof_sids = sid_idx + max_nof_terminated_usp_sids_1pass + 1;

            /** call the nof_sids API to map to RCH port and encapsulation */
            rv = bcm_srv6_extension_terminator_add(unit, &extension_terminator_info);

            if (rv != BCM_E_NONE)
            {
                 printf("Error, bcm_srv6_extension_terminator_add for nof_sids:%d\n", sid_idx);
                 return rv;
            }

         } /** of for sid_idx*/
    }

    /*
     * 9. Egress extended termination
     */
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_tunnel_terminator_t tunnel_term_set;

    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = tunnel_type;
    tunnel_term_set.flags = BCM_TUNNEL_TERM_MICRO_SEGMENT_ID | BCM_TUNNEL_TERM_CROSS_CONNECT;
    if(srh_present) {
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_EXTENDED_TERMINATION;
    }
    sal_memcpy(tunnel_term_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[EGRESS_DIP], 16);
    sal_memcpy(tunnel_term_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = cint_srv6_tunnel_info.vrf_in[EGRESS];
    /* Following is for configuring for the IPv6 LIF, to increase the strength of its TTL
     * so that terminated IPv6 header's TTL would get to sysh, from which the IPv6 TTL decrease
     * would decrease from
     */
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d, end-point-%d\n", rv, end_point_id);
        return rv;
    }

    /*
     * 10. Add cross connect
     */
    bcm_vswitch_cross_connect_t cc_gports;
    bcm_vswitch_cross_connect_t_init(&cc_gports);
    BCM_GPORT_FORWARD_PORT_SET(cc_gports.port2, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]);
    cc_gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    cc_gports.port1 = tunnel_term_set.tunnel_id;
    rv = bcm_vswitch_cross_connect_add(unit, &cc_gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        return rv;
    }

    printf("%s(): Exit\r\n",proc_name);
    return 0;
}

/*
 * Configuring ESR USP and multicast to access & SRv6
 */
int
srv6_egress_tunnel_split_horizon(
    int unit,
    int in_port_egress, /** Incoming port of SRV6 Egress */
    int out_port_egress, /** Outgoing port of SRV6 Egress */
    int out_port, /** Outgoing port of SRV6 Ingress Tunnel */
    int nof_sids)
{
    int rv;
    int vrf = 1;                /* RCH VRF 2nd Pass is equal here to 1st Pass VRF */
    bcm_gport_t gport;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    int sid_idx;
    int mc_id = cint_srv6_tunnel_info.l2_termination_vsi;

    char *proc_name = "srv6_egress_tunnel_split_horizon";

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port_egress, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties
     */
    /* Egress tunnel out port */
    rv = out_port_set(unit, out_port_egress);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,out_port_egress);
        return rv;
    }

    /* Ingress tunnel out port */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,out_port);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
        return rv;
    }

    /* Egress tunnel out rif */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], cint_srv6_tunnel_info.intf_out_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /* Ingress tunnel out rif */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 5. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    rv = out_port_set(unit, cint_srv6_tunnel_info.recycle_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set recycle_port \n");
        return rv;
    }

    /** Configure RCH port of Extended Termination type */
    rv = bcm_port_control_set(unit, cint_srv6_tunnel_info.recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_control_set for rch_port %d\n",proc_name, cint_srv6_tunnel_info.recycle_port);
        return rv;
    }

    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    bcm_srv6_extension_terminator_mapping_t terminator_info;
    terminator_info.flags = 0;

    /** Convert Port number to GPORT */
    BCM_GPORT_LOCAL_SET(terminator_info.port, cint_srv6_tunnel_info.recycle_port);

    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;

    /** create LIF entries for all possible nof_sids in an SRv6 packet that would lead to RCY port + RCH Encapsulation*/
    for (sid_idx = 0; sid_idx < nof_2pass_recycle_entries ; sid_idx++)
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

        terminator_info.nof_sids = sid_idx + max_nof_terminated_usp_sids_1pass + 1;

        /** call the nof_sids API to map to RCH port and encapsulation */
        rv = bcm_srv6_extension_terminator_add(unit, &terminator_info);

        if (rv != BCM_E_NONE)
        {
             printf("Error, bcm_srv6_extension_terminator_add for nof_sids:%d\n", sid_idx);
             return rv;
        }

     } /** of for sid_idx*/

     /*
     * 6. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     */

    /** SRV6 Ingress Tunnel ARP create, with VLAN ID of expected in End-Point-1*/
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /*
     * 7. No need to create FEC, since it's multicast
     */

    /*
     * 8. Configuring ISR SRv6 Tunnel
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
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);

    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_dnx_srv6_srh_base_initiator_create \n");
       return rv;
    }

    /*
     * 9. Create multicast group and add entries
     */
    rv = multicast__open_mc_group(unit, &mc_id, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in multicast__open_mc_group\n");
        return rv;
    }

    bcm_gport_t ports[2];
    int encaps[2];

    /** egress out port */
    BCM_GPORT_MODPORT_SET(ports[0], unit, out_port_egress);
    encaps[0] = 0;

    /** ingress tunnel */
    BCM_GPORT_MODPORT_SET(ports[1], unit, out_port);
    encaps[1] = cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF];

    rv = multicast__add_multicast_entry(unit, mc_id, ports, encaps, 2, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in multicast__add_multicast_entry\n");
        return rv;
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}


/*
 * Configuring ESR USP and IPMC forward
 */
int
srv6_egress_tunnel_ipmc_fwd(
    int unit,
    int in_port,
    int out_port_1,
    int out_port_2)
{
    int rv;
    int vrf = 1;                /* RCH VRF 2nd Pass is equal here to 1st Pass VRF */
    bcm_gport_t gport;
    int encap_id = 0x1384;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    int sid_idx;
    uint32 dip_mask = 0xFFFFFFFF;
    uint32 sip_mask = 0x0;
    bcm_ip6_t ipv6_mc = {0xFF, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13};
    bcm_ip6_t full_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t sip = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    char *proc_name = "srv6_egress_tunnel_split_horizon";

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties
     */
    /* Egress tunnel out port */
    rv = out_port_set(unit, out_port_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,out_port_1);
        return rv;
    }

    rv = out_port_set(unit, out_port_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,out_port_2);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
        return rv;
    }

    /* Egress tunnel out rif */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], cint_srv6_tunnel_info.intf_out_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 5. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    rv = out_port_set(unit, cint_srv6_tunnel_info.recycle_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set recycle_port \n");
        return rv;
    }

    /** Configure RCH port of Extended Termination type */
    rv = bcm_port_control_set(unit, cint_srv6_tunnel_info.recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_control_set for rch_port %d\n",proc_name, cint_srv6_tunnel_info.recycle_port);
        return rv;
    }

    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    bcm_srv6_extension_terminator_mapping_t terminator_info;
    terminator_info.flags = 0;

    /** Convert Port number to GPORT */
    BCM_GPORT_LOCAL_SET(terminator_info.port, cint_srv6_tunnel_info.recycle_port);

    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;

    /** create LIF entries for all possible nof_sids in an SRv6 packet that would lead to RCY port + RCH Encapsulation*/
    for (sid_idx = 0; sid_idx < nof_2pass_recycle_entries ; sid_idx++)
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

        terminator_info.nof_sids = sid_idx + max_nof_terminated_usp_sids_1pass + 1;

        /** call the nof_sids API to map to RCH port and encapsulation */
        rv = bcm_srv6_extension_terminator_add(unit, &terminator_info);

        if (rv != BCM_E_NONE)
        {
             printf("Error, bcm_srv6_extension_terminator_add for nof_sids:%d\n", sid_idx);
             return rv;
        }

     } /** of for sid_idx*/

    /*
     * 6. Create ARP and set its properties
     */
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[EGRESS], 
                                            cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }

    /*
     * 7. No need to create FEC, since it's multicast
     */

    /*
     * 8. Create multicast group and add entries
     */
    int flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
    rv = bcm_multicast_create(unit, flags, &cint_ipmc_info.mc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    bcm_multicast_replication_t rep_array;
    bcm_multicast_replication_t_init(&rep_array);
    rep_array.port = out_port_1;
    rep_array.encap1 = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    rv = bcm_multicast_add(unit, cint_ipmc_info.mc_group, BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_add in Port= %d\n", out_port_1);
        return rv;
    }

    rep_array.port = out_port_2;
    rep_array.encap1 = cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS];
    rv = bcm_multicast_add(unit, cint_ipmc_info.mc_group, BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_add in Port= %d\n", out_port_2);
        return rv;
    }

    /* Add IPMC entry */
    rv = add_ipv4_ipmc(unit, cint_ipmc_info.mc_host_dip, dip_mask, cint_ipmc_info.host_sip, sip_mask,
                       0, vrf, cint_ipmc_info.mc_group, 0,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv4_ipmc: vrf = %d, mc group = %d\n", vrf, cint_ipmc_info.mc_group);
        return rv;
    }

     /* Add IPv6 MC entry */
    rv = add_ipv6_ipmc(unit, ipv6_mc, full_mask, sip, sip, 0, vrf, 0, 0, cint_ipmc_info.mc_group, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv6_ipmc: vrf = %d, mc group = %dn", vrf, cint_ipmc_info.mc_group);
        return rv;
    }

    return rv;
}

/*
 * Configuring ISR and IPMC forward
 */
int
srv6_ingress_tunnel_ipmc_fwd(
                            int unit,
                            int in_port,
                            int out_port_1,
                            int out_port_2,
                            int nof_sids)
{
    int rv;
    bcm_gport_t gport;
    int in_vlan = 15;
    uint32 dip_mask = 0xFFFFFFFF;
    uint32 sip_mask = 0x0;
    bcm_ip6_t ipv6_mc = {0xFF, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13};
    bcm_ip6_t full_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t sip = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    int recycle_port = 40;
    bcm_gport_t out_ac;

    char *proc_name = "srv6_ingress_tunnel_ipmc_fwd";

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

    /* Create AC to match port+vlan and set ingress network group */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.match_vlan = in_vlan;
    vlan_port.vsi = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    vlan_port.port = in_port;
    vlan_port.ingress_network_group_id = 2;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port of SRV6 Ingress Tunnel %d\n",proc_name,out_port_1);
        return rv;
    }

    /* Create VSI */
    rv = bcm_vlan_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create VLAN %d\n", cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
        return rv;
    }

    /* Create out AC for bridge fallback egress split horizon */
    /* use EEDB, since for J2C, "esem" "feature" "etps_properties_assignment" -> out_lif profile use EES, not ETPS*/
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    vlan_port.egress_network_group_id = 2;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }
    out_ac=vlan_port.vlan_port_id;

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
    ingress_rif.flags |= BCM_L3_INGRESS_IPMC_BRIDGE_FALLBACK; /* enable RIF bridge fallback */
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
    flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;

    /** SRV6 Ingress Tunnel ARP create, with VLAN ID of expected in End-Point-1*/
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /*
     * 6. No need to create FEC, since it's multicast
     */

    /*
     * 7. Configuring ISR SRv6 Tunnel
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
    /* BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, gport); */

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
     * 8. Multicast group and entry
     */
    /* Create multicast group */
    int flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;

    rv = bcm_multicast_create(unit, flags, &cint_ipmc_info.mc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);

    bcm_multicast_replication_t rep_array;
    bcm_multicast_replication_t_init(&rep_array);
    rep_array.port = out_port_2;
    rep_array.encap1 = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]; /* fall to bridge */

    rv = bcm_multicast_add(unit, cint_ipmc_info.mc_group, BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_add in Port= %d\n", out_port_2);
        return rv;
    }

    bcm_if_t rep_idx = 0x300100;

    rep_array.port = out_port_1;
    rep_array.encap1 = rep_idx;

    rv = bcm_multicast_add(unit, cint_ipmc_info.mc_group, BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_add in Port= %d\n", out_port_1);
        return rv;
    }

    /* Add PPMC entry */
    int cuds[2];
    BCM_L3_ITF_SET(cuds[0], BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);
    BCM_L3_ITF_SET(cuds[1], BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_FIRST_SID_GLOBAL_LIF]);
    rv = bcm_multicast_encap_extension_create(unit, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID, &rep_idx, 2, cuds);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_multicast_encap_extension_create\n");
        return rv;
    }

    /* Add IPMC entry */
    rv = add_ipv4_ipmc(unit, cint_ipmc_info.mc_host_dip, dip_mask, cint_ipmc_info.host_sip, sip_mask,
                       0, cint_srv6_tunnel_info.vrf_in[INGRESS], cint_ipmc_info.mc_group, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv4_ipmc\n");
        return rv;
    }

    /* Add IPv6 MC entry */
    rv = add_ipv6_ipmc(unit, ipv6_mc, full_mask, sip, sip, 0, cint_srv6_tunnel_info.vrf_in[INGRESS], 0, 0, cint_ipmc_info.mc_group, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv6_ipmc\n");
        return rv;
    }

    /*
     * 9. Multicast for bridge fallback
     */
    /* Create multicast group */
    cint_ipmc_info.mc_group = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    rv = bcm_multicast_create(unit, flags, &cint_ipmc_info.mc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Add copies */
    bcm_multicast_replication_t replications[3];
    set_multicast_replication(&replications[0], out_port_1, -1);
    set_multicast_replication(&replications[1], out_port_2, (out_ac & 0xfffff)); /* dropped by split horizon */
    set_multicast_replication(&replications[2], in_port, 0); /* dropped by same interface filter */

    rv = bcm_multicast_add(unit, cint_ipmc_info.mc_group, BCM_MULTICAST_INGRESS_GROUP, 3, replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }

    /* Configure split horizon */
    bcm_switch_network_group_config_t network_group_config;
    bcm_switch_network_group_config_t_init(&network_group_config);
    network_group_config.dest_network_group_id = 2;
    network_group_config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
    rv = bcm_switch_network_group_config_set(unit, 2, &network_group_config);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_switch_network_group_config_set\n");
        return rv;
    }

    if (*dnxc_data_get(unit, "l3", "feature", "mc_bridge_fallback", NULL)) {
        /* use field processor to perform bridge fallback:
           for ipvx routing packets with no hit in forwarding, and for vsi profile:
           fwd to recycle port and build recycle header, update vsi and pop lif */
        rv = bcm_port_control_set(unit, recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppBridgeFallback);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_port_control_set with type bcmPortControlRecycleApp\n");
            return rv;
        }

        /* Create a control lif and set it's in_lif orientation as required*/
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_RECYCLE;
        vlan_port.ingress_network_group_id = 2;
        vlan_port.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
        vlan_port.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
        vlan_port.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;

        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
            return rv;
        }

        /* Create recycle entry */
        bcm_l2_egress_t recycle_entry;
        bcm_l2_egress_t_init(&recycle_entry);
        recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
        recycle_entry.recycle_app = bcmL2EgressRecycleAppDropAndContinue;
        recycle_entry.vlan_port_id = vlan_port.vlan_port_id;

        rv = bcm_l2_egress_create(unit, &recycle_entry);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l2_egress_create \n");
            return rv;
        }

        /* configure vsi profile for vsis */
        int vsi_profile = 1; 
        rv = vlan__control_vlan_vsi_profile_set(unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS], vsi_profile);
        if (rv != BCM_E_NONE)
        {
            printf("Error, vlan__control_vlan_vsi_profile_set\n");
            return rv;
        }

        rv = cint_field_bridge_fallback_main(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, cint_field_bridge_fallback_main\n");
            return rv;
        }

        rv = cint_field_bridge_fallback_entry_add(unit,
            recycle_port, vsi_profile, BCM_L3_ITF_VAL_GET(recycle_entry.encap_id));
        if (rv != BCM_E_NONE)
        {
            printf("Error, cint_field_bridge_fallback_entry_add\n");
            return rv;
        }
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/*
 * Configuring egress tunnel ROO
 */
int
srv6_egress_tunnel_roo(
                            int unit,
                            int core_port,
                            int access_port)
{
    int rv;
    bcm_gport_t gport;
    int flags;
    int vrf = 1;
    int native_vlan = 30;
    int out_vlan = 103;
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    bcm_mac_t native_eth_rif_mac = {0x00, 0x00, 0x00, 0x00, 0xAA, 0xBB};

    char *proc_name = "srv6_egress_tunnel_roo";

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    /* SRV6 Egress Tunnel in port */
    rv = in_port_intf_set(unit, core_port, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }

    /*
     * 2. Egress tunnel out port
     */
    rv = out_port_set(unit, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set of SRV6 Ingress Tunnel %d\n",proc_name,core_port);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    /* SRV6 Egress Tunnel in RIF */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
        return rv;
    }

    /* native RIF */
    rv = intf_eth_rif_create(unit, native_vlan, native_eth_rif_mac);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,native_vlan);
        return rv;
    }

    /* SRV6 Egress Tunnel out RIF  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], cint_srv6_tunnel_info.intf_out_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf ingress_rif;

    /* Egress tunnel in RIF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /* native RIF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = native_vlan;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /*
     * 5. Create ARP and set its properties
     */

    /* Access side ARP */
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[EGRESS], out_vlan, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }

    /*
     * 6. SRV6 FEC Entries
     */

    /*
     * Get FEC ID
     */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    /* Create access FEC, out of overlay */
    flags = 0;
    BCM_GPORT_LOCAL_SET(gport, access_port);
    rv = l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID], cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], 
                                     0, gport, flags);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create access FEC\n",proc_name);
        return rv;
    }

    /*
     * 7. Add route entry, routing into overlay
     */

    /* Add IPv4 host entry */
    add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, vrf, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID], 0, 0);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4() for SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /* Add IPv6 host entry */
    rv = add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, vrf, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv6\n", proc_name);
        return rv;
    }

    /*
     * 8. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    rv = out_port_set(unit, cint_srv6_tunnel_info.recycle_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set recycle_port \n");
        return rv;
    }

    /** Configure RCH port of Extended Termination type */
    rv = bcm_port_control_set(unit, cint_srv6_tunnel_info.recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_control_set for rch_port %d\n",proc_name, cint_srv6_tunnel_info.recycle_port);
        return rv;
    }

    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    bcm_srv6_extension_terminator_mapping_t terminator_info;
    int sid_idx;

    terminator_info.flags = 0;

    /** Convert Port number to GPORT */
    BCM_GPORT_LOCAL_SET(terminator_info.port, cint_srv6_tunnel_info.recycle_port);

    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;

    /** create LIF entries for all possible nof_sids in an SRv6 packet that would lead to RCY port + RCH Encapsulation*/
    for (sid_idx = 0; sid_idx < nof_2pass_recycle_entries ; sid_idx++)
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

        terminator_info.nof_sids = sid_idx + max_nof_terminated_usp_sids_1pass + 1;

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

/*
 * Configuring ESR USP and into MPLS tunnel
 */
int
srv6_egress_tunnel_into_mpls(
                            int unit,
                            int core_port_1,
                            int core_port_2,
                            int nof_tunnel,
                            int is_p2p)
{
    int rv;
    bcm_gport_t gport;
    int flags;
    int vrf = 1;
    bcm_vswitch_cross_connect_t cc_gports;
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    bcm_gport_t gport_fec;

    char *proc_name = "srv6_egress_tunnel_into_mpls";

    /* Configure SRv6 tunnel to be P2P */
    if (is_p2p)
    {
        bcm_tunnel_terminator_t_init(&tunnel_term_set);
        tunnel_term_set.type = bcmTunnelTypeSR6;
        sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
        sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
        sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
        sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
        tunnel_term_set.vrf = vrf;
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
        tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_CROSS_CONNECT;

        rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
        if(rv != BCM_E_NONE)
        {
            printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
            return rv;
        }

        srv6_term_tunnel_id = tunnel_term_set.tunnel_id;

        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = vrf;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        rv = intf_ingress_rif_set(unit, &ingress_rif);
        if (rv != BCM_E_NONE)
        {
            printf("Error intf_ingress_rif_set. rv = %d \n", rv);
            return rv;
        }
    }

    /* In this cint, all ports, RIF, ARP, FEC configurations are done in mpls_util_main */

    /*
     * 1. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    rv = out_port_set(unit, cint_srv6_tunnel_info.recycle_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set recycle_port \n");
        return rv;
    }

    /** Configure RCH port of Extended Termination type */
    rv = bcm_port_control_set(unit, cint_srv6_tunnel_info.recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_control_set for rch_port %d\n",proc_name, cint_srv6_tunnel_info.recycle_port);
        return rv;
    }

    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    bcm_srv6_extension_terminator_mapping_t terminator_info;
    int sid_idx;

    terminator_info.flags = 0;

    /** Convert Port number to GPORT */
    BCM_GPORT_LOCAL_SET(terminator_info.port, cint_srv6_tunnel_info.recycle_port);

    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;

    /** create LIF entries for all possible nof_sids in an SRv6 packet that would lead to RCY port + RCH Encapsulation*/
    for (sid_idx = 0; sid_idx < nof_2pass_recycle_entries ; sid_idx++)
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

        terminator_info.nof_sids = sid_idx + max_nof_terminated_usp_sids_1pass + 1;

        /** call the nof_sids API to map to RCH port and encapsulation */
        rv = bcm_srv6_extension_terminator_add(unit, &terminator_info);

        if (rv != BCM_E_NONE)
        {
             printf("Error, bcm_srv6_extension_terminator_add for nof_sids:%d\n", sid_idx);
             return rv;
        }
     } /** of for sid_idx*/

    /*
     * 2. Configure MPLS tunnel encapsulation
     */
    rv = mpls_deep_stack_ipv6_example(unit,core_port_1,core_port_2,nof_tunnel,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, mpls_deep_stack_ipv6_example\n");
        return rv;
    }

    /* Add cross connect entry for P2P */
    if (is_p2p)
    {
        BCM_GPORT_FORWARD_PORT_SET(gport_fec, mpls_util_entity[0].fecs[0].fec_id);

        bcm_vswitch_cross_connect_t_init(&cc_gports);
        cc_gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        cc_gports.port1 = tunnel_term_set.tunnel_id;
        cc_gports.port2 = gport_fec;
        rv = bcm_vswitch_cross_connect_add(unit, &cc_gports);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vswitch_cross_connect_add\n");
            return rv;
        }
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/*
 * Configuring ESR USP and into VPLS tunnel
 */
int
srv6_egress_tunnel_into_vpls(
                            int unit,
                            int core_port_1,
                            int core_port_2,
                            int is_p2p)
{
    int rv;
    bcm_gport_t gport;
    int flags;
    int vrf = 1;
    cint_srv6_tunnel_info.l2_termination_vsi = 5;
    bcm_vswitch_cross_connect_t cc_gports;
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    bcm_gport_t gport_fec;
    int fec_id;

    char *proc_name = "srv6_egress_tunnel_into_vpls";

    /* Configure SRv6 tunnel to be P2P */
    if (is_p2p)
    {
        bcm_tunnel_terminator_t_init(&tunnel_term_set);
        tunnel_term_set.type = bcmTunnelTypeEthSR6;
        sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
        sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
        sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
        sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
        tunnel_term_set.vrf = vrf;
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
        tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_CROSS_CONNECT;
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;

        rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
        if(rv != BCM_E_NONE)
        {
            printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
            return rv;
        }

        srv6_term_tunnel_id = tunnel_term_set.tunnel_id;
    }

    /*
     * 1. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    rv = out_port_set(unit, cint_srv6_tunnel_info.recycle_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set recycle_port \n");
        return rv;
    }

    /** Configure RCH port of Extended Termination type */
    rv = bcm_port_control_set(unit, cint_srv6_tunnel_info.recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_control_set for rch_port %d\n",proc_name, cint_srv6_tunnel_info.recycle_port);
        return rv;
    }

    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    bcm_srv6_extension_terminator_mapping_t terminator_info;
    int sid_idx;

    terminator_info.flags = 0;

    /** Convert Port number to GPORT */
    BCM_GPORT_LOCAL_SET(terminator_info.port, cint_srv6_tunnel_info.recycle_port);

    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;

    /** create LIF entries for all possible nof_sids in an SRv6 packet that would lead to RCY port + RCH Encapsulation*/
    for (sid_idx = 0; sid_idx < nof_2pass_recycle_entries ; sid_idx++)
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

        terminator_info.nof_sids = sid_idx + max_nof_terminated_usp_sids_1pass + 1;

        /** call the nof_sids API to map to RCH port and encapsulation */
        rv = bcm_srv6_extension_terminator_add(unit, &terminator_info);

        if (rv != BCM_E_NONE)
        {
             printf("Error, bcm_srv6_extension_terminator_add for nof_sids:%d\n", sid_idx);
             return rv;
        }
     } /** of for sid_idx*/

    /*
     * 2. Configure VPLS basic
     */
    rv = vpls_main(unit,core_port_1,core_port_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vpls_main\n");
        return rv;
    }

    /* In this cint,out port, ARP, FEC, MACT are configured in vpls_main, only need to create in port and RIF here*/

    /*
     * 3. Set In-Port to In ETh-RIF
     */
    /* SRV6 Egress Tunnel in port */
    rv = in_port_intf_set(unit, core_port_1, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }

    /*
     * 4. Create ETH-RIF and set its properties
     */
    /* SRV6 Egress Tunnel in RIF */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
        return rv;
    }

    /*
     * 5. Set Incoming ETH-RIF properties
     */
    /* Egress tunnel in RIF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /* 
     *Add cross connect entry for P2P
     *SRv6 tunnel P2P only supports port or FEC, so here create a new FEC pointing to PWE encap
     * and set PWE's next pointer to MPLS tunnel
     */
    if (is_p2p)
    {

        /* Replace PWE encap with next pointer to MPLS tunnel */
        pwe_encap.flags |=BCM_MPLS_PORT_REPLACE;
        pwe_encap.egress_tunnel_if = mpls_encap_tunnel[0].tunnel_id;
        rv = vpls_mpls_port_add_encapsulation(unit, &pwe_encap);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vpls_mpls_port_add_encapsulation\n", rv);
            return rv;
        }

        /* Create FEC pointing to PWE */
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }

        fec_id = fec_id + 10;
        /* Create access FEC, out of overlay */
        flags = 0;
        BCM_GPORT_LOCAL_SET(gport, core_port_2);
        rv = l3__egress_only_fec__create(unit, fec_id, pwe_encap.encap_id | 0x40000000, 0, gport, flags);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create access FEC\n",proc_name);
            return rv;
        }

        bcm_vswitch_cross_connect_t_init(&cc_gports);
        BCM_GPORT_FORWARD_PORT_SET(cc_gports.port2, fec_id);
        cc_gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        cc_gports.port1 = tunnel_term_set.tunnel_id;
        rv = bcm_vswitch_cross_connect_add(unit, &cc_gports);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vswitch_cross_connect_add\n");
            return rv;
        }
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/*
 * Configuring ESR USP and into SRv6 tunnel
 */
int
srv6_egress_tunnel_into_srv6(
                            int unit,
                            int core_port_1,
                            int core_port_2,
                            int next_protocol_is_l2,
                            int is_p2p,
                            int nof_sids)
{
    int rv;
    int vrf = 1;                /* RCH VRF 2nd Pass is equal here to 1st Pass VRF */
    bcm_gport_t gport;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    bcm_vswitch_cross_connect_t cc_gports;
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_tunnel_terminator_t tunnel_term_set;
    bcm_mac_t dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x30};
    int sid_idx;

    char *proc_name = "srv6_egress_tunnel_into_srv6";

    /* Configure SRv6 tunnel to be P2P */
    if (is_p2p)
    {
        bcm_tunnel_terminator_t_init(&tunnel_term_set);
        tunnel_term_set.type = bcmTunnelTypeEthSR6;
        sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
        sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
        sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
        sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
        tunnel_term_set.vrf = vrf;
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
        tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_CROSS_CONNECT;
        if (next_protocol_is_l2) {
            tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
            tunnel_term_set.type = bcmTunnelTypeEthSR6;
        }

        rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
        if(rv != BCM_E_NONE)
        {
            printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
            return rv;
        }

        srv6_term_tunnel_id = tunnel_term_set.tunnel_id;

        if (!next_protocol_is_l2) {
            l3_ingress_intf_init(&ingress_rif);
            ingress_rif.vrf = vrf;
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
            rv = intf_ingress_rif_set(unit, &ingress_rif);
            if (rv != BCM_E_NONE)
            {
                printf("Error intf_ingress_rif_set. rv = %d \n", rv);
                return rv;
            }
        }
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, core_port_1, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties
     */
    /* Ingress tunnel out port */
    rv = out_port_set(unit, core_port_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,core_port_2);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    /* Egress tunnel in rif */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
        return rv;
    }

    /* Ingress tunnel out rif */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 5. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    rv = out_port_set(unit, cint_srv6_tunnel_info.recycle_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set recycle_port \n");
        return rv;
    }

    /** Configure RCH port of Extended Termination type */
    rv = bcm_port_control_set(unit, cint_srv6_tunnel_info.recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_control_set for rch_port %d\n",proc_name, cint_srv6_tunnel_info.recycle_port);
        return rv;
    }

    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    bcm_srv6_extension_terminator_mapping_t terminator_info;
    terminator_info.flags = 0;

    /** Convert Port number to GPORT */
    BCM_GPORT_LOCAL_SET(terminator_info.port, cint_srv6_tunnel_info.recycle_port);

    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;

    /** create LIF entries for all possible nof_sids in an SRv6 packet that would lead to RCY port + RCH Encapsulation*/
    for (sid_idx = 0; sid_idx < nof_2pass_recycle_entries ; sid_idx++)
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

        terminator_info.nof_sids = sid_idx + max_nof_terminated_usp_sids_1pass + 1;

        /** call the nof_sids API to map to RCH port and encapsulation */
        rv = bcm_srv6_extension_terminator_add(unit, &terminator_info);

        if (rv != BCM_E_NONE)
        {
             printf("Error, bcm_srv6_extension_terminator_add for nof_sids:%d\n", sid_idx);
             return rv;
        }

     } /** of for sid_idx*/

     /*
     * 6. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     */

    /** SRV6 Ingress Tunnel ARP create */
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /*
     * 6. SRV6 FEC Entries
     */

     /* Update FEC IDS */
    rv = srv6_update_fecs_values(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, srv6_get_fec_values\n");
        return rv;
    }

    int srv6_basic_lif_encode;
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, core_port_2);

    rv = l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], srv6_basic_lif_encode, 0, gport, 0);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC_1 only\n",proc_name);
        return rv;
    }

    /*
     * We don't need 2nd FEC here since in egress SRH base points to the 1st SID
     */

    if (!is_p2p) {
        /*
         * 8. Add host entries
         */
        /** IPv4 host */
        rv = add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], 
                           cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], 0, 0);

        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv4() for SRV6 Ingress Tunnel\n",proc_name);
            return rv;
        }

        /** IPv6 host */
        rv = add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, cint_srv6_tunnel_info.vrf_in[INGRESS], 
                           cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv6\n", proc_name);
            return rv;
        }

        /*
         * 9. Add MACT entry
         */
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);

        bcm_l2_addr_t l2_addr;
        bcm_l2_addr_t_init(&l2_addr, dmac, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
        l2_addr.port = gport;
        l2_addr.flags = BCM_L2_STATIC;
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function bcm_l2_addr_add\n", proc_name);
            return rv;
        }
    }

    /*
     * 10. Configuring ISR SRv6 Tunnel
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
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);

    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_dnx_srv6_srh_base_initiator_create \n");
       return rv;
    }

    /* Add cross connect entry for P2P */
    if (is_p2p)
    {
        bcm_vswitch_cross_connect_t_init(&cc_gports);
        BCM_GPORT_FORWARD_PORT_SET(cc_gports.port2, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);
        cc_gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        cc_gports.port1 = tunnel_term_set.tunnel_id;
        rv = bcm_vswitch_cross_connect_add(unit, &cc_gports);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vswitch_cross_connect_add\n");
            return rv;
        }
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/*
 * Configuring ISR after MPLS termination
 */
int
srv6_ingress_tunnel_with_mpls_termination(
                            int unit,
                            int core_port_1,
                            int core_port_2,
                            int nof_sids)
{
    int rv;
    int vrf = 100; /* use value in init_default_mpls_params */
    bcm_gport_t gport;
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0,0,0xFF,0x55};
    int sid_idx;

    char *proc_name = "srv6_ingress_tunnel_with_mpls_termination";


    /* In this cint, in port, RIF, are done in mpls_util_main */
    /*
     * 1. Create RIF
     */

    /* Ingress tunnel out rif */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 2. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     */

    /** SRV6 Ingress Tunnel ARP create */
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /*
     * 3. SRV6 FEC Entries
     */

     /* Update FEC IDS */
    rv = srv6_update_fecs_values(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, srv6_get_fec_values\n");
        return rv;
    }

    int srv6_basic_lif_encode;
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, core_port_2);

    rv = l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], srv6_basic_lif_encode, 0, gport, 0);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC_1 only\n",proc_name);
        return rv;
    }

    /*
     * We don't need 2nd FEC here since in egress SRH base points to the 1st SID
     */

    /*
     * 4. Add host entries
     */
    /** IPv4 host */
    rv = add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, vrf, 
                       cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4() for SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /** IPv6 host */
    rv = add_host_ipv6(unit, ip6_dip, vrf, 
                       cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv6\n", proc_name);
        return rv;
    }

    /*
     * 5. Configuring ISR SRv6 Tunnel
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
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);

    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_dnx_srv6_srh_base_initiator_create \n");
       return rv;
    }

    /*
     * 6. Configure MPLS tunnel termination
     */
    rv = mpls_util_main(unit,core_port_1,core_port_2);
    if (rv) {
        printf("Error, create_mpls_tunnel\n");
        return rv;
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}


/*
 * Configuring ISR after VPLS termination
 */
int
srv6_ingress_tunnel_with_vpls_termination(
                            int unit,
                            int core_port_1,
                            int core_port_2,
                            int is_p2p,
                            int nof_sids)
{
    int rv;
    bcm_gport_t gport;
    bcm_vswitch_cross_connect_t cc_gports;
    int vsi = 5; /* use value in vpls_main */
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    bcm_mac_t dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x30};
    int sid_idx;

    char *proc_name = "srv6_ingress_tunnel_with_vpls_termination";

    /* In this cint, in port, RIF, are done in vpls_main */
    /*
     * 1. Create RIF
     */

    /* Ingress tunnel out rif */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 2. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     */

    /** SRV6 Ingress Tunnel ARP create */
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /*
     * 3. SRV6 FEC Entries
     */

     /* Update FEC IDS */
    rv = srv6_update_fecs_values(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, srv6_get_fec_values\n");
        return rv;
    }

    int srv6_basic_lif_encode;
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, core_port_2);

    rv = l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], srv6_basic_lif_encode, 0, gport, 0);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC_1 only\n",proc_name);
        return rv;
    }

    /*
     * We don't need 2nd FEC here since in egress SRH base points to the 1st SID
     */
    if (!is_p2p) {
        /*
         * 4. Add MACT entry
         */
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);

        bcm_l2_addr_t l2_addr;
        bcm_l2_addr_t_init(&l2_addr, dmac, vsi);
        l2_addr.port = gport;
        l2_addr.flags = BCM_L2_STATIC;
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function bcm_l2_addr_add\n", proc_name);
            return rv;
        }
    }

    /*
     * 5. Configuring ISR SRv6 Tunnel
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
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);

    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_dnx_srv6_srh_base_initiator_create \n");
       return rv;
    }

    /*
     * 6. Configure VPLS tunnel termination
     */
    params_set = 1;
    init_default_vpls_params(unit);
    if (is_p2p)
    {
        pwe_term.flags2 |= BCM_MPLS_PORT2_CROSS_CONNECT;
        vpn = 0;
    }

    ac_port.port = core_port_2;
    pwe_port.port = core_port_1;

    /* In port */
    rv = vpls_configure_port_properties(unit, pwe_port, 1 /* create_rif */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vpls_configure_port_properties pwe_port\n");
        return rv;
    }

    /*
     * create vlan based on the vsi (vpn) 
     */
    if (!is_p2p)
    {
        rv = bcm_vlan_create(unit, vpn);
        if (rv != BCM_E_NONE)
        {
            printf("Error, failed to create VLAN\n");
            return rv;
        }
        pwe_term.forwarding_domain = vpn;
    }

    /* In RIF */
    rv = vpls_create_l3_interfaces(unit, &pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vpls_create_l3_interfaces\n");
        return rv;
    }

    /* Configure a termination label for the ingress flow */
    rv = vpls_create_termination_stack(unit, mpls_term_tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vpls_create_termination_stack\n");
        return rv;
    }

    /* configure PWE tunnel - ingress flow */
    rv = vpls_mpls_port_add_termination(unit, &pwe_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vpls_mpls_port_add_termination\n");
        return rv;
    }

    /* 
     *Add cross connect entry for P2P
     */
    if (is_p2p)
    {
        bcm_vswitch_cross_connect_t_init(&cc_gports);
        BCM_GPORT_FORWARD_PORT_SET(cc_gports.port2, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);
        cc_gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        cc_gports.port1 = pwe_term.mpls_port_id;
        rv = bcm_vswitch_cross_connect_add(unit, &cc_gports);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vswitch_cross_connect_add\n");
            return rv;
        }
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/* ~~~~~~~~~~~~~~~~~~  BFDv6 Max Sessions(back-to-back) ~~~~~~~~~~~~~~~~~~ */
/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.

 * SOC Properties:
 * #===============================
 * # OAM SOC
 * oam_enable=1
 * num_oamp_ports=1
 * tm_port_header_type_out_232.BCM88650=CPU
 * tm_port_header_type_out_0.BCM88650=CPU
 * ucode_port_40.BCM88650=RCY.0
 * oam_rcy_port=40
 * tm_port_header_type_in_40.BCM88650=TM
 * tm_port_header_type_out_40.BCM88650=ETH
 * counter_engine_source_0.BCM88650=INGRESS_OAM
 * counter_engine_source_1.BCM88650=EGRESS_OAM
 * counter_engine_statistics_0.BCM88650=ALL
 * counter_engine_statistics_1.BCM88650=ALL
 * counter_engine_format_0.BCM88650=PACKETS
 * counter_engine_format_1.BCM88650=PACKETS
 * phy_1588_dpll_frequency_lock.BCM88650=1
 * phy_1588_dpll_phase_initial_lo.BCM88650=0x40000000
 * phy_1588_dpll_phase_initial_hi.BCM88650=0x10000000
 * bcm886xx_next_hop_mac_extension_enable.BCM88650=0
 * mplstp_g8113_channel_type=0x8902
 * bcm886xx_ipv6_tunnel_enable=0
 *
 * #---------------------
 * # BFD IPv6 SOC
 * bfd_enable=1
 * mcs_load_uc0=1
 * bfd_ipv6_enable=1
 * bfd_ipv6_trap_port=204
 * num_queues_pci=16
 * num_queues_uc0=8
 * bfd_simple_password_keys=8
 * bfd_sha1_keys=8
 * port_priorities_13.BCM88650=2
 * port_priorities_14.BCM88650=2
 * port_priorities_204.BCM88650=8
 * ucode_port_204.BCM88650=CPU.16:core_0.204
 * tm_port_header_type_in_204.BCM88650=INJECTED_2_PP
 * tm_port_header_type_out_204.BCM88650=CPU
 * bfd_num_sessions=1001
 * bfd_echo_enabled=1
 *
 * Test Environment:
 *     Unit 1 port 14 ------ Unit 2 port 13
 *
 *
 *   cd ../../../../
 *   cint src/examples/sand/utility/cint_sand_utils_global.c
 *   cint src/examples/sand/utility/cint_sand_utils_mpls.c
 *   cint src/examples/dpp/utility/cint_utils_l3.c
 *   cint src/examples/dpp/utility/cint_utils_vlan.c
 *   cint src/examples/dpp/cint_ip_route.c
 *   cint src/examples/sand/cint_sand_bfd_ipv6.c
 *   cint src/examples/dpp/cint_field_bfd_ipv6_single_hop.c
 *   cint src/examples/dpp/cint_bfd_ipv6_max_sessions.c
 *   c
 *
 * Test 64@3.3ms + 256@10ms
 *  on unit 1:
 *   print execute_on_unit1(unit,64,256,14,1,1,0,0,bcmBFDAuthTypeKeyedSHA1,3);
 *  on unit 2:
 *   print execute_on_unit2(unit,64,256,13,1,1,0,0,bcmBFDAuthTypeKeyedSHA1,3);
 *
 * Test 120 session @ 3.3ms
 *  on unit 1:
 *   print sessions_create_unit1(unit,120,14,1,1,0,0,10,5010,bcmBFDAuthTypeSimplePassword,3300,3300,50000,3,0,0);
 *  on unit 2:
 *   print sessions_create_unit2(unit,120,13,1,1,0,0,5010,10,bcmBFDAuthTypeSimplePassword,3300,3300,50000,3,0,0);
 *
 * Enable BFD Event Register
 *   print bfd_ipv6_event_register(unit);
 */

int unit=0;
int session_at_10=256;
int session_at_33=64;
int i;
int scale_test = 0;
int is_micro_bfd = 0;
int is_echo_ep = 0;
int bfd_remote_port = BCM_GPORT_INVALID;
int user_defined_trap_code;
int punt_rate = 0;


/*
* bfd_num_sessions defined the max sessions, if this soc is not configured, the default value is 256.
*/
int max_session=soc_property_get(unit, "bfd_num_sessions", 0);
if (max_session == 0) {
    max_session = 256;
}
bcm_bfd_endpoint_info_t bfd_endpoint_info[max_session];

struct bfd_over_ipv6_ep_create_info_s {
    int                     is_single_hop;
    int                     is_acc;
    int                     is_passive;
    int                     is_demand;
    int                     is_echo;
    int                     session;
    int                     bfd_encap_id;
    bcm_bfd_endpoint_t      endpoint_id;
    bcm_bfd_tunnel_type_t   type;
    bcm_ip6_t               src_ipv6;
    bcm_ip6_t               dst_ipv6;
    uint32                  udp_src_port;
    bcm_bfd_auth_type_t     auth_type;
    uint32                  auth_index;
    uint32                  local_disc;
    uint32                  remote_disc;
    bcm_cos_t               priority;
    uint32                  local_min_tx;
    uint32                  local_min_rx;
};

bfd_over_ipv6_ep_create_info_s bfdv6_endpoint_1;    /* The tests's variables. */
sal_memset(&bfdv6_endpoint_1, 0, sizeof(bfdv6_endpoint_1));
int bcm_bfd_cb_new(int unit, uint32 flags, bcm_bfd_event_types_t events_array, bcm_bfd_endpoint_t epid, void *user_data) {
    bcm_bfd_endpoint_info_t ep;
    bcm_bfd_endpoint_info_t_init(&ep);

    int rv = 0;
    int event;

    rv = bcm_bfd_endpoint_get(unit, epid, &ep);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_get (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    printf("endpoint:%d\n", epid);
    for (event = bcmBFDEventStateChange; event < bcmBFDEventCount; event++) {
        if (BCM_BFD_EVENT_TYPE_GET(events_array, event)) {
            /* log the number of event */
            bfd_event_count[event]++;
            printf("    event:%d\n", event);

            switch (event) {
            case bcmBFDEventStateChange:
                print "bcmBFDEventStateChange";
                print "============id=========";
                print ep.id;
                print "============local=========";
                print ep.local_state;
                print "============remote=========";
                print ep.remote_state;
                break;
            case bcmBFDEventRemoteStateDiag:
                print "bcmBFDEventRemoteStateDiag";
                break;
            case bcmBFDEventSessionDiscriminatorChange:
                print "bcmBFDEventSessionDiscriminatorChange";
                break;
            case bcmBFDEventAuthenticationChange:
                print "bcmBFDEventAuthenticationChange";
                break;
            case bcmBFDEventParameterChange:
                print "bcmBFDEventParameterChange";
                break;
            case bcmBFDEventSessionError:
                print "bcmBFDEventSessionError";
                break;
            case bcmBFDEventEndpointRemote:
                print "bcmBFDEventEndpointRemote";
                break;
            case bcmBFDEventEndpointRemoteUp:
                print "bcmBFDEventEndpointRemoteUp";
                break;
            case bcmBFDEventEndpointTimeout:
                print "bcmBFDEventEndpointTimeout";
                break;
            case bcmBFDEventEndpointTimein:
                print "bcmBFDEventEndpointTimein";
                break;
            case bcmBFDEventEndpointTimeoutEarly:
                print "bcmBFDEventEndpointTimeoutEarly";
                break;
            case bcmBFDEventEndpointFlagsChange:
                print "bcmBFDEventEndpointFlagsChange";
                break;
            default:
                print "=================Error: Unknown event, why=================";
                print event;
                break;
           }
        }
    }
    print "***************Events Ends***************";
    return rv;
}

void print_ipv6_addr(char *type, bcm_ip6_t a)
{
  printf("%s = ",type);
  printf("%x:", ((a[0] << 8) | a[1]));
  printf("%x:", ((a[2] << 8) | a[3]));
  printf("%x:", ((a[4] << 8) | a[5]));
  printf("%x:", ((a[6] << 8) | a[7]));
  printf("%x:", ((a[8] << 8) | a[9]));
  printf("%x:", ((a[10] << 8) | a[11]));
  printf("%x:", ((a[12] << 8) | a[13]));
  printf("%x\n", ((a[14] << 8) | a[15]));
}

/*
 * port_vlan_translation_action_set
 */
int port_vlan_translation_action_set(int unit, int in_vlan, int out_vlan, int out_vsi){
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t action_get;
    bcm_vlan_port_t out_vlan_port;

    int is_adv_vt;
    int rv;

    is_adv_vt = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

    rv = bcm_port_class_set(unit, bfd_out_port[0], bcmPortClassId, bfd_out_port[0]);
    if (rv != BCM_E_NONE) {
        printf("fail set port(%d) class\n", bfd_out_port[0]);
      return rv;
    }

    bcm_vlan_port_t_init(&out_vlan_port);
    out_vlan_port.flags |= (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE);
    out_vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    out_vlan_port.vsi = out_vsi;
    out_vlan_port.port = bfd_out_port[0];
    out_vlan_port.match_vlan = out_vsi;
    out_vlan_port.egress_vlan = 33;
    rv =  bcm_vlan_port_create(unit, &out_vlan_port);
    if (rv != BCM_E_NONE) {
        printf("fail create port(%d) vlan(%d)\n", bfd_out_port[0], in_vlan);
        return rv;
    } else {
        printf("\n\n successful create port(%d) vlan(%d)\n", bfd_out_port[0], in_vlan);
    }

    /* set port translation info*/
    if (is_adv_vt) {
        printf("\n is_adv_vt is %d  \n", is_adv_vt);

        rv = vlan_port_translation_set(unit, out_vlan, 0, out_vlan_port.vlan_port_id, g_eve_edit_utils.edit_profile, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_translation_set\n");
            return rv;
        }

        /* Check if action ID exists*/
        bcm_vlan_action_set_t_init(&action_get);
        rv = bcm_vlan_translate_action_id_get( unit,
                                               BCM_VLAN_ACTION_SET_EGRESS,
                                               g_eve_edit_utils.action_id,
                                               &action_get);
        if (rv != BCM_E_NOT_FOUND) {
            printf("Action ID exists, will not create action id \n");
            return rv;
        }  else {
            /* Create action IDs*/
            rv = bcm_vlan_translate_action_id_create( unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &g_eve_edit_utils.action_id);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_id_create\n");
                return rv;
            }

            /* Set translation action 1. outer action, set TPID 0x8100. */
            bcm_vlan_action_set_t_init(&action);
            action.dt_outer = bcmVlanActionAdd;
            action.dt_inner = bcmVlanActionNone;
            action.dt_outer_pkt_prio = bcmVlanActionAdd;
            action.outer_tpid = 0x8100;
            rv = bcm_vlan_translate_action_id_set( unit,
                                                   BCM_VLAN_ACTION_SET_EGRESS,
                                                   g_eve_edit_utils.action_id,
                                                   &action);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_id_set\n");
                return rv;
            }

        }


        /* set action class */
        rv = vlan_default_translate_action_class_set(unit, g_eve_edit_utils.action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_default_translate_action_class_set\n");
            return rv;
        }

    } else {
        bcm_vlan_action_set_t action;
        bcm_vlan_action_set_t_init(&action);
        action.ut_outer = bcmVlanActionAdd;
        action.ut_outer_pkt_prio = bcmVlanActionAdd;
        action.new_outer_vlan = out_vlan;
        action.priority = 0;

        rv = bcm_vlan_translate_egress_action_add(unit, out_vlan_port.vlan_port_id, BCM_VLAN_NONE, BCM_VLAN_NONE, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_egress_action_add in unit %d sys_port %d \n", unit, bfd_out_port[0]);
        }
    }


    return rv;
}
/*
 *creating l3 interface - ingress/egress
 */
int bfd_ipv6_intf_init(int unit, int in_vlan, int out_vlan, int out_vsi, bcm_mac_t mac_address, bcm_mac_t next_hop_mac, bcm_ip6_t src_ipv6, bcm_ip6_t dst_ipv6){

    int rv;
    int flags = 0;

    int vrf = 0;
    int ing_intf_in;
    int ing_intf_out;
    int l3_eg_int;
    int fec;
    int encap_id;
    int i;
    int session_id = in_vlan - 2;

    /*** create ingress router interface ***/
    rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, in_vlan, bfd_in_port[0], 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", bfd_in_port[0], in_vlan);
      return rv;
    }

    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    intf.mtu_valid = 1;
    intf.mtu = 0;
    intf.mtu_forwarding = 0;

    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, out_vlan, bfd_out_port[0], 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", bfd_out_port[0], out_vlan);
      return rv;
    }

    intf.vsi = out_vsi;

    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    l3_eg_int = ing_intf_out;

    /*** Create egress object1 ***/
    create_l3_egress_s l3eg;
    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac , 6);
    l3eg.allocation_flags = flags;
    l3eg.l3_flags = flags;
    l3eg.out_tunnel_or_rif = l3_eg_int;
    l3eg.out_gport = bfd_out_port[0];
    l3eg.vlan = out_vsi;
    l3eg.fec_id = fec;
    l3eg.arp_encap_id = encap_id;

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec = l3eg.fec_id;
    encap_id = l3eg.arp_encap_id;
    bfd_encap_id = encap_id;

    rv = add_ipv6_host(unit, &src_ipv6, vrf, fec, bfd_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, sysport=%d, in unit %d\n", sysport, unit);
    }

    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, in unit %d \n", fec, unit);
        printf("next hop mac at encap-id %08x, in unit %d\n", bfd_encap_id, unit);
    }

    return rv;
}


int bfd_ipv6_ep_create(int unit, bfd_over_ipv6_ep_create_info_s bfdv6_ep) {

    int rv;
    /*soc property "bfd_echo_enabled" is not applied for BFD IPv6 Echo */
    bfdv6_ep.is_echo = is_echo_ep;
    int session = bfdv6_ep.session;

    /* Adding BFDoIPV6 one hop endpoint */
    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info[session]);

    bfd_endpoint_info[session].flags = BCM_BFD_ENDPOINT_WITH_ID | BCM_BFD_ENDPOINT_IPV6;
    if (!bfdv6_ep.is_single_hop) {
        bfd_endpoint_info[session].flags |= BCM_BFD_ENDPOINT_MULTIHOP;
    }
    if (is_micro_bfd == 1) {
        printf("Created MICRO BFD endpoint\n");
        bfd_endpoint_info[session].flags |= BCM_BFD_ENDPOINT_MICRO_BFD;
    }
    if (bfdv6_ep.is_acc) {
        bfd_endpoint_info[session].flags |= BCM_BFD_ENDPOINT_IN_HW;
    }
    if (bfdv6_ep.is_passive) {
         bfd_endpoint_info[session].flags |= BCM_BFD_ENDPOINT_PASSIVE;
    }
    if (bfdv6_ep.is_demand) {
        bfd_endpoint_info[session].flags |= BCM_BFD_ENDPOINT_DEMAND;
    }
   if (bfdv6_ep.is_echo) {
        printf(">>>> Create Echo Endpoint\n");
        bfd_endpoint_info[session].flags |= BCM_BFD_ECHO;
        bfd_endpoint_info[session].local_min_echo = bfdv6_ep.local_min_rx;
    }

    bcm_stk_sysport_gport_get(unit,bfd_out_port[0], &bfd_endpoint_info[session].tx_gport );
    bfd_endpoint_info[session].id = bfdv6_ep.endpoint_id;
    bfd_endpoint_info[session].type = bfdv6_ep.type;
    bfd_endpoint_info[session].ip_ttl = 255;
    bfd_endpoint_info[session].ip_tos = 0xab;
    sal_memcpy(&(bfd_endpoint_info[session].src_ip6_addr), bfdv6_ep.src_ipv6, 16);
    sal_memcpy(&(bfd_endpoint_info[session].dst_ip6_addr), bfdv6_ep.dst_ipv6, 16);
    bfd_endpoint_info[session].udp_src_port = bfdv6_ep.udp_src_port;
    bfd_endpoint_info[session].auth_index = bfdv6_ep.auth_index;
    bfd_endpoint_info[session].auth = bcmBFDAuthTypeNone;
    bfd_endpoint_info[session].local_discr = bfdv6_ep.local_disc;
    bfd_endpoint_info[session].local_state = bcmBFDStateDown;
    bfd_endpoint_info[session].local_min_tx = bfdv6_ep.local_min_tx;
    bfd_endpoint_info[session].local_min_rx = bfdv6_ep.local_min_rx;
    bfd_endpoint_info[session].local_detect_mult = 3;
    bfd_endpoint_info[session].remote_discr = bfdv6_ep.remote_disc;
    bfd_endpoint_info[session].egress_if = bfdv6_ep.bfd_encap_id;
    bfd_endpoint_info[session].auth = bfdv6_ep.auth_type;
    bfd_endpoint_info[session].int_pri = bfdv6_ep.priority;
    if (bfdv6_ep.auth_type != bcmBFDAuthTypeNone) {
        bfd_endpoint_info[session].auth_index = bfdv6_ep.auth_index;
    }

    if (punt_rate) {
        bfd_endpoint_info[session].sampling_ratio = punt_rate;
    }

    if (bfd_remote_port != BCM_GPORT_INVALID) {
        /* remote_gport field must be a trap */
        BCM_GPORT_TRAP_SET(bfd_endpoint_info[session].remote_gport, user_defined_trap_code, 7, 0); /*Taken from default values*/
        printf(">>>>>>>>>>>> Session %d - remote_gport is %d\n",session, bfd_endpoint_info[session].remote_gport);
    }

    if ( !scale_test) {
        print bfdv6_ep;
        print &bfdv6_ep;
    }
    rv = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info[session]);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    if(verbose >= 1) {
        printf("Created an endpoint with  \n   ***  endpoint_id 0x%x \n", bfd_endpoint_info[session].id);
        printf("   *** local_disc is %d\n",  bfd_endpoint_info[session].local_discr);
        printf("   *** remote_discr is %d\n",  bfd_endpoint_info[session].remote_discr);
        printf("   *** udp_src_port is %d\n", bfd_endpoint_info[session].udp_src_port);
        printf("   *** local_mix_tx is %d, local_min_rx is %d\n", bfd_endpoint_info[session].local_min_tx,bfd_endpoint_info[session].local_min_rx );
        print_ipv6_addr("   *** src_ip6_addr",bfd_endpoint_info[session].src_ip6_addr);
        print_ipv6_addr("   *** dst_ip6_addr",bfd_endpoint_info[session].dst_ip6_addr);
        printf("   *** priority is %d\n", bfd_endpoint_info[session].int_pri);
        if (bfdv6_ep.is_echo) {
            printf("   *** local_min_echo is %d\n", bfd_endpoint_info[session].local_min_echo);
        }
        if (bfd_remote_port != BCM_GPORT_INVALID) {
            printf("   *** remote_gport is %d\n", bfd_endpoint_info[session].remote_gport);
        }

        /*printf("   *** authentication type is %d\n", bfd_endpoint_info[session].auth);*/
        if (bfdv6_ep.auth_type != bcmBFDAuthTypeNone) {
            printf("   *** authentication index is %d\n", bfd_endpoint_info[session].auth_index);
        }
    }
    return rv;
}


/*
* BFD session Event register
*/
int bfd_ipv6_event_register(int unit) {
    int rv;

   /* bcm_bfd_ipv6_events_register(unit);*/
    bcm_bfd_event_types_t events;
    BCM_BFD_EVENT_TYPE_CLEAR_ALL(events);
    BCM_BFD_EVENT_TYPE_SET_ALL(events);

    rv=bcm_bfd_event_register(unit, events, bcm_bfd_cb_new, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_event_register (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

int bfd_ipv6_event_unregister(int unit) {
    int rv;

   /* bcm_bfd_ipv6_events_register(unit);*/
    bcm_bfd_event_types_t events;
    BCM_BFD_EVENT_TYPE_CLEAR_ALL(events);
    /*BCM_BFD_EVENT_TYPE_SET_ALL(events);*/
    BCM_BFD_EVENT_TYPE_SET(events, bcmBFDEventStateChange);
    BCM_BFD_EVENT_TYPE_SET(events, bcmBFDEventRemoteStateDiag);
    BCM_BFD_EVENT_TYPE_SET(events, bcmBFDEventSessionDiscriminatorChange);
    BCM_BFD_EVENT_TYPE_SET(events, bcmBFDEventAuthenticationChange);
    BCM_BFD_EVENT_TYPE_SET(events, bcmBFDEventParameterChange);
    BCM_BFD_EVENT_TYPE_SET(events, bcmBFDEventSessionError);
    BCM_BFD_EVENT_TYPE_SET(events, bcmBFDEventEndpointTimeout);
    BCM_BFD_EVENT_TYPE_SET(events, bcmBFDEventEndpointFlagsChange);

    rv=bcm_bfd_event_unregister(unit, events, bcm_bfd_cb_new);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_event_unregister (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    return rv;
}
int bfd_ipv6_event_count_clear_all(int unit) {
    int event_i;
    for (event_i = 0; event_i < bcmBFDEventCount; event_i++) {
        bfd_event_count[event_i] = 0;
    }

    return BCM_E_NONE;

}

int bfd_ipv6_ep_state_print(int unit, int epid) {
    int rv=0;
    bcm_bfd_endpoint_info_t ep;
    bcm_bfd_endpoint_info_t_init(&ep);

    int id=0x800000 + epid;
    rv=bcm_bfd_endpoint_get(unit, id, &ep);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_get (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    print id;
    print ep.local_state;
    print ep.remote_state;
    print ep.udp_src_port;
    print_ipv6_addr("   *** src_ip6_addr",ep.src_ip6_addr);
    print_ipv6_addr("   *** dst_ip6_addr",ep.dst_ip6_addr);

    return rv;
}


int bfd_ipv6_ep_state_up(int unit, int epid) {
    int rv=0;
    bcm_bfd_endpoint_info_t ep;
    bcm_bfd_endpoint_info_t_init(&ep);

    int id=0x800000 + epid;
    rv=bcm_bfd_endpoint_get(unit, id, &ep);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_get (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if (ep.local_state != bcmBFDStateUp || ep.remote_state != bcmBFDStateUp) {
        printf("\n\nState of endpoint %d is NOT UP:         \n", epid);
        print ep.local_state ;
        print ep.remote_state ;
        return epid;
    }

    return rv;
}

int bfd_ipv6_ep_destroy(int unit, int epid) {
    int rv=0;
    int id=0x800000 + epid;

    rv=bcm_bfd_endpoint_destroy(unit, id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_destroy (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    return rv;

}


int bfd_ipv6_ep_stat_get(int unit, int epid, int seconds, bcm_bfd_endpoint_stat_t *stat) {
    int rv;
    int id=0x800000+epid;

    rv=bcm_bfd_endpoint_stat_get(unit, id, stat, 1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_stat_get (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    sal_sleep(seconds);

    rv=bcm_bfd_endpoint_stat_get(unit, id, stat, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_stat_get (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

int sessions_recreate(int unit, int endpoint_id, int port, int is_single_hop,
                            int is_acc, int is_passive, int is_demand, uint32 local_disc, uint32 remote_disc, bcm_bfd_auth_type_t auth_type,
                            uint32 local_min_tx, uint32 local_min_rx, uint32 udp_src_port, bcm_cos_t priority)
{
    int in_vlan = 2;
    int out_vlan = 200;
    int rv;
    int j;
    int out_vsi = 11;

    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};

    bcm_ip6_t dst_ipv6 = {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x02,0x00,0x02};
    bcm_ip6_t src_ipv6 = {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01};

    /* BFD event count */
    uint32 bfd_event_count[array_num] = {0};

    /* BFD in/out Port Configuration */
    rv=bfd_ipv6_service_init(unit,port,0,port,0,0,-1);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_service_init\n");
        print rv;
        return rv;
    }

    /* BFD AuthType Configuration */
    if (auth_type == bcmBFDAuthTypeSimplePassword) {
        printf("Step: bfd_ipv6_auth_sp_set\n");
        rv = bfd_ipv6_auth_sp_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    } else if (auth_type == bcmBFDAuthTypeKeyedSHA1) {
        printf("Step: bfd_ipv6_auth_sha1_set\n");
        rv = bfd_ipv6_auth_sha1_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sha1_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

   /* bcm_bfd_ipv6_events_register(unit);*/
    bcm_bfd_event_types_t events;
    BCM_BFD_EVENT_TYPE_CLEAR_ALL(events);
    BCM_BFD_EVENT_TYPE_SET_ALL(events);

    rv=bcm_bfd_event_register(unit, events, bcm_bfd_cb_new, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_event_register (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Re-Create BFD IPv6 Endpoint  */
    printf("\n### Re-create: endpoint %d at tx %d rx %d ###\n", endpoint_id,local_min_tx,local_min_rx);

    j=endpoint_id;
    in_vlan = 2 + j;
    out_vlan = 200+ j;
    out_vsi = 11+ j;

    src_ipv6[15]+=j;
    dst_ipv6[15]+=j;
    udp_src_port+=j;
    local_disc+=j;
    remote_disc+=j;

    /*  init the endpoint  */
    sal_memset(&bfdv6_endpoint_1, 0, sizeof(bfdv6_endpoint_1));
    bfdv6_endpoint_1.endpoint_id=endpoint_id;
    bfdv6_endpoint_1.src_ipv6 = src_ipv6;
    bfdv6_endpoint_1.dst_ipv6 = dst_ipv6;
    bfdv6_endpoint_1.is_single_hop = is_single_hop;
    bfdv6_endpoint_1.is_acc = is_acc;
    bfdv6_endpoint_1.is_passive = is_passive;
    bfdv6_endpoint_1.is_demand = is_demand;
    bfdv6_endpoint_1.local_disc = local_disc;
    bfdv6_endpoint_1.remote_disc = remote_disc;
    bfdv6_endpoint_1.auth_type = auth_type;
    bfdv6_endpoint_1.local_min_tx=local_min_tx;
    bfdv6_endpoint_1.local_min_rx=local_min_rx;
    bfdv6_endpoint_1.udp_src_port=udp_src_port;
    bfdv6_endpoint_1.priority=priority;


    /* Port vlan translation configuration*/
    rv=port_vlan_translation_action_set(unit, in_vlan, out_vlan, out_vsi);
    if (rv != BCM_E_NONE) {
        printf("Error in port_vlan_translation_action_set (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* ip entry create */
    rv=bfd_ipv6_intf_init(unit,in_vlan,out_vlan,out_vsi,mac_address,next_hop_mac,src_ipv6,dst_ipv6);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_intf_init \n");
        print rv;
        return rv;
    }

    bfdv6_endpoint_1.bfd_encap_id=bfd_encap_id;

    rv=bfd_ipv6_ep_create(unit,bfdv6_endpoint_1);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_create \n");
        print rv;
        return rv;
    }


    return rv;
}

int bfd_ipv6_ep_create_example(int unit, int port1, int port2, int is_init, bfd_over_ipv6_ep_create_info_s bfdv6_ep, int is_inject ) {

    int in_vlan = 2;
    int out_vlan = 200;
    int out_vsi = 11;
    int rv;

    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};

    if (bfdv6_ep.priority == 0 ) {
        bfdv6_ep.priority = 5;
    }
    if (bfdv6_ep.local_min_tx == 0 ) {
        bfdv6_ep.local_min_tx = 10000;
    }
    if (bfdv6_ep.local_min_rx == 0 ) {
        bfdv6_ep.local_min_rx = 10000;
    }

    /* BFD event count */
    uint32 bfd_event_count[array_num] = {0};

    if (is_init == 1) {
        /* BFD in/out Port Configuration */
        rv=bfd_ipv6_service_init(unit,port1,0,port2,0,0,-1);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_service_init\n");
            print rv;
            return rv;
        }
        /* Port vlan translation configuration*/
        rv=port_vlan_translation_action_set(unit, in_vlan, out_vlan, out_vsi);
        if (rv != BCM_E_NONE) {
            printf("Error in port_vlan_translation_action_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    /* BFD AuthType Configuration */
    if (bfdv6_ep.auth_type == bcmBFDAuthTypeSimplePassword) {
        printf("Step: bfd_ipv6_auth_sp_set\n");
        rv = bfd_ipv6_auth_sp_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    } else if (bfdv6_ep.auth_type == bcmBFDAuthTypeKeyedSHA1) {
        printf("Step: bfd_ipv6_auth_sha1_set\n");
        rv = bfd_ipv6_auth_sha1_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sha1_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    /* ip entry create */
    rv=bfd_ipv6_intf_init(unit,in_vlan,out_vlan,out_vsi,mac_address,next_hop_mac,bfdv6_ep.src_ipv6,bfdv6_ep.dst_ipv6);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_intf_init \n");
        print rv;
        return rv;
    }

    /* bfd ipv6 endpoint create(session=0) */
    bfdv6_ep.session=0;
    bfdv6_ep.bfd_encap_id=bfd_encap_id;

    rv=bfd_ipv6_ep_create(unit,bfdv6_ep);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_create \n");
        print rv;
        return rv;
    }

    /* Verify the endpoint is created as expected or not */

    printf("\n\nVerify if the endpoint %d(0x%x)is configured correctly.\n", bfdv6_ep.endpoint_id, bfd_endpoint_info[0].id);

    bcm_bfd_endpoint_info_t bfd_endpoint_test_info;
    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_test_info);

    rv = bcm_bfd_endpoint_get(unit, bfd_endpoint_info[0].id, &bfd_endpoint_test_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Here &bfd_endpoint_info[0] and &bfd_endpoint_test_info are compared */
    rv = cmp_structs(unit, &bfd_endpoint_info[0], &bfd_endpoint_test_info,bcmBFDTunnelTypeUdp ) ;
    if (rv != 0) {
        printf("%d wrong fields in BFDoIPV6\n", rv);
        printf("Failed in cmp_structs, some parameter is not updated for endpoint %d(%x). \n", bfdv6_ep.endpoint_id, bfd_endpoint_info[0].id);
        return rv;
    } else {
        printf("\n\nEndpoint info check ok!\n");
    }
    return rv;


    if (is_init == 1) {
        rv=bfd_ipv6_single_hop_field_action(unit, is_inject, 0);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_single_hop_field_action \n");
            print rv;
            return rv;
        }
    }

    return rv;

}

/*print execute_on_unit1(unit,64,256,14,1,1,0,0,bcmBFDAuthTypeNone,3); */
int execute_on_unit1(int unit, int session_at_33, int session_at_10, int port, int is_single_hop, int is_acc, int is_passive, int is_demand, bcm_bfd_auth_type_t auth_type, bcm_cos_t priority)
{
    int in_vlan = 2;
    int out_vlan = 200;
    int rv;
    int out_vsi = 11;
    int in_vlan_tmp, out_vlan_tmp, out_vsi_tmp;

    max_session=session_at_33+session_at_10;

    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};

    /* BFD event count */
    uint32 bfd_event_count[array_num] = {0};

    uint32 local_disc = 5010;
    uint32 remote_disc = 10;

    bcm_ip6_t dst_ipv6 = {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x02,0x00,0x02};
    bcm_ip6_t src_ipv6 = {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01};

    /* BFD in/out Port Configuration */
    rv=bfd_ipv6_service_init(unit,port,0,port,0,0,-1);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_service_init\n");
        print rv;
        return rv;
    }

    /* BFD AuthType Configuration */
    if (auth_type == bcmBFDAuthTypeSimplePassword) {
        printf("Step: bfd_ipv6_auth_sp_set\n");
        rv = bfd_ipv6_auth_sp_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    } else if (auth_type == bcmBFDAuthTypeKeyedSHA1) {
        printf("Step: bfd_ipv6_auth_sha1_set\n");
        rv = bfd_ipv6_auth_sha1_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sha1_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

   /* bcm_bfd_ipv6_events_register(unit);*/
    bcm_bfd_event_types_t events;
    BCM_BFD_EVENT_TYPE_CLEAR_ALL(events);
    BCM_BFD_EVENT_TYPE_SET_ALL(events);

    rv=bcm_bfd_event_register(unit, events, bcm_bfd_cb_new, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_event_register (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    int endpoint_id=0;
    int udp_src_port=50000;
    uint32 min_tx_rx;

    /* Create BFD IPv6 ENDPOINT@3.3ms */
    for (i = 0; i < session_at_33; i++) {
        printf("\n\n************ Created %d endpoint @3.3ms ************ \n", i);
        min_tx_rx=3300;

        in_vlan_tmp=in_vlan++;
        out_vlan_tmp=out_vlan++;
        out_vsi_tmp=out_vsi++;
       /*
        printf("\n\n    in_vlan_tmp in this session is (%d) \n", in_vlan_tmp);
        printf("    out_vlan_tmp in this session is (%d) \n", out_vlan_tmp);
        printf("    out_vsi_tmp in this session is (%d) \n", out_vsi_tmp);
        */
        /* Port vlan translation configuration*/
        rv=port_vlan_translation_action_set(unit, in_vlan_tmp, out_vlan_tmp, out_vsi_tmp);
        if (rv != BCM_E_NONE) {
            printf("Error in port_vlan_translation_action_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
        /* ip entry create */
        rv=bfd_ipv6_intf_init(unit,in_vlan_tmp,out_vlan_tmp,out_vsi_tmp,mac_address,next_hop_mac,src_ipv6,dst_ipv6);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_intf_init \n");
            print rv;
            return rv;
        }

        rv=bfd_ipv6_ep_create(unit,i,bfd_encap_id, src_ipv6, dst_ipv6, endpoint_id++, is_single_hop, is_acc, is_passive, is_demand, udp_src_port++, auth_type, 0, local_disc + i, remote_disc + i, priority, min_tx_rx, min_tx_rx);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create \n");
            print rv;
            return rv;
        }

        src_ipv6[15]+=1;
        dst_ipv6[15]+=1;
    }

    src_ipv6[14]+=1;
    dst_ipv6[14]+=1;

    /* Create BFD IPv6 ENDPOINT@10ms */
    for (i = session_at_33; i < max_session; i++) {
        printf("\n\n************ Created %d endpoint @10ms ************ \n", i);
        min_tx_rx=10000;

        in_vlan_tmp=in_vlan++;
        out_vlan_tmp=out_vlan++;
        out_vsi_tmp=out_vsi++;

        /* Port vlan translation configuration*/
        rv=port_vlan_translation_action_set(unit, in_vlan_tmp, out_vlan_tmp, out_vsi_tmp);
        if (rv != BCM_E_NONE) {
            printf("Error in port_vlan_translation_action_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }

        /* ip entry create */
        rv=bfd_ipv6_intf_init(unit,in_vlan_tmp,out_vlan_tmp,out_vsi_tmp,mac_address,next_hop_mac,src_ipv6,dst_ipv6);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_intf_init \n");
            print rv;
            return rv;
        }

        rv=bfd_ipv6_ep_create(unit,i,bfd_encap_id, src_ipv6, dst_ipv6, endpoint_id++, is_single_hop, is_acc, is_passive, is_demand, udp_src_port++, auth_type, 0, local_disc + i, remote_disc + i, priority, min_tx_rx, min_tx_rx);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create \n");
            print rv;
            return rv;
        }

        src_ipv6[15]+=1;
        dst_ipv6[15]+=1;
    }

    rv=bfd_ipv6_single_hop_field_action(unit, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_single_hop_field_action \n");
        print rv;
        return rv;
    }
    return rv;
}


/*print execute_on_unit2(unit,64,256,13,1,1,0,0,bcmBFDAuthTypeNone,3); */
int execute_on_unit2(int unit, int session_at_33, int session_at_10, int port, int is_single_hop, int is_acc, int is_passive, int is_demand, bcm_bfd_auth_type_t auth_type, bcm_cos_t priority)
{
    int in_vlan = 200;
    int out_vlan = 2;
    int rv;
    int out_vsi = 11;
    int in_vlan_tmp, out_vlan_tmp, out_vsi_tmp;

    max_session=session_at_33+session_at_10;

    bcm_mac_t next_hop_mac = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
    bcm_mac_t mac_address  = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};

    /* BFD event count */
    uint32 bfd_event_count[array_num] = {0};
    uint32 local_disc = 10;
    uint32 remote_disc = 5010;

    bcm_ip6_t src_ipv6= {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x02,0x00,0x02};
    bcm_ip6_t dst_ipv6 = {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01};

    /* BFD in/out Port Configuration */
    rv=bfd_ipv6_service_init(unit,port,0,port,0,0,-1);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_service_init\n");
        print rv;
        return rv;
    }

    /* BFD AuthType Configuration */
    if (auth_type == bcmBFDAuthTypeSimplePassword) {
        printf("Step: bfd_ipv6_auth_sp_set\n");
        rv = bfd_ipv6_auth_sp_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    } else if (auth_type == bcmBFDAuthTypeKeyedSHA1) {
        printf("Step: bfd_ipv6_auth_sha1_set\n");
        rv = bfd_ipv6_auth_sha1_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sha1_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

   /* bcm_bfd_ipv6_events_register(unit);*/
    bcm_bfd_event_types_t events;
    BCM_BFD_EVENT_TYPE_CLEAR_ALL(events);
    BCM_BFD_EVENT_TYPE_SET_ALL(events);

    rv=bcm_bfd_event_register(unit, events, bcm_bfd_cb_new, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_event_register (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    int endpoint_id=0;
    int udp_src_port=50000;
    uint32 min_tx_rx;

    for (i = 0; i < session_at_33; i++) {
        printf("\n\n************ Created %d endpoint @3.3ms ************ \n", i);
        min_tx_rx=3300;
        in_vlan_tmp=in_vlan++;
        out_vlan_tmp=out_vlan++;
        out_vsi_tmp=out_vsi++;
        /* Port vlan translation configuration*/
        rv=port_vlan_translation_action_set(unit, in_vlan_tmp, out_vlan_tmp, out_vsi_tmp);
        if (rv != BCM_E_NONE) {
            printf("Error in port_vlan_translation_action_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }

        /* ip entry create */
        rv=bfd_ipv6_intf_init(unit,in_vlan_tmp,out_vlan_tmp,out_vsi_tmp,mac_address,next_hop_mac,src_ipv6,dst_ipv6);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_intf_init \n");
            print rv;
            return rv;
        }

        rv=bfd_ipv6_ep_create(unit,i,bfd_encap_id, src_ipv6, dst_ipv6, endpoint_id++, is_single_hop, is_acc, is_passive, is_demand, udp_src_port++, auth_type, 0, local_disc + i, remote_disc + i, priority, min_tx_rx, min_tx_rx);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create \n");
            print rv;
            return rv;
        }
        src_ipv6[15]+=1;
        dst_ipv6[15]+=1;
    }

    src_ipv6[14]+=1;
    dst_ipv6[14]+=1;

    /* BFD SESSIONS @10ms */
    for (i = session_at_33; i < max_session; i++) {
        printf("\n\n************ Created %d endpoint @10ms ************ \n", i);
        min_tx_rx=10000;

        in_vlan_tmp=in_vlan++;
        out_vlan_tmp=out_vlan++;
        out_vsi_tmp=out_vsi++;
        /* Port vlan translation configuration*/
        rv=port_vlan_translation_action_set(unit, in_vlan_tmp, out_vlan_tmp, out_vsi_tmp);
        if (rv != BCM_E_NONE) {
            printf("Error in port_vlan_translation_action_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
        /* ip entry create */
        rv=bfd_ipv6_intf_init(unit,in_vlan_tmp,out_vlan_tmp,out_vsi_tmp,mac_address,next_hop_mac,src_ipv6,dst_ipv6);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_intf_init \n");
            print rv;
            return rv;
        }

        rv=bfd_ipv6_ep_create(unit,i,bfd_encap_id, src_ipv6, dst_ipv6, endpoint_id++, is_single_hop, is_acc, is_passive, is_demand, udp_src_port++, auth_type, 0, local_disc + i, remote_disc + i, priority, min_tx_rx, min_tx_rx);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create \n");
            print rv;
            return rv;
        }

        src_ipv6[15]+=1;
        dst_ipv6[15]+=1;
   }

    rv=bfd_ipv6_single_hop_field_action(unit, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_single_hop_field_action \n");
        print rv;
        return rv;
    }
    return rv;

}



/*print sessions_create_unit1(unit,120,14,1,1,0,0,10,5010,bcmBFDAuthTypeSimplePassword,3300,3300,50000,3,0,0);*/
int sessions_create_unit1(int unit, int session, int port, int is_single_hop, int is_acc, int is_passive, int is_demand, uint32 local_disc, uint32 remote_disc,
                            bcm_bfd_auth_type_t auth_type, uint32 local_min_tx, uint32 local_min_rx, uint32 udp_src_port, bcm_cos_t priority,
                            int event_on, int is_inject)
{
    int in_vlan = 2;
    int out_vlan = 200;
    int rv;
    int out_vsi = 11;
    int in_vlan_tmp, out_vlan_tmp, out_vsi_tmp;
    int fixed_src_ipv6;
    uint8 use_localdiscr_as_sess_id = 0;

    if (!is_single_hop && session > 16) {
        printf(">>>> For Multihop endpoints, support max 16 src ipv6 address, run the test with fixed src ipv6 address");
        fixed_src_ipv6 =1;
    }

    use_localdiscr_as_sess_id   = soc_property_get(unit,
                              "bfd_use_local_discriminator_as_session_id", 0);

    /*  init the endpoint  */
    sal_memset(&bfdv6_endpoint_1, 0, sizeof(bfdv6_endpoint_1));
    bfdv6_endpoint_1.session=session;
    bfdv6_endpoint_1.src_ipv6 = src_ipv6;
    bfdv6_endpoint_1.dst_ipv6 = dst_ipv6;
    bfdv6_endpoint_1.is_single_hop = is_single_hop;
    bfdv6_endpoint_1.is_acc = is_acc;
    bfdv6_endpoint_1.is_passive = is_passive;
    bfdv6_endpoint_1.is_demand = is_demand;
    bfdv6_endpoint_1.local_disc = local_disc;
    bfdv6_endpoint_1.remote_disc = remote_disc;
    bfdv6_endpoint_1.auth_type = auth_type;
    bfdv6_endpoint_1.local_min_tx=local_min_tx;
    bfdv6_endpoint_1.local_min_rx=local_min_rx;
    bfdv6_endpoint_1.udp_src_port=udp_src_port;
    bfdv6_endpoint_1.priority=priority;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};

    /* BFD event count */
    uint32 bfd_event_count[array_num] = {0};

    printf(">>> bfd_remote_port is %d", bfd_remote_port);
    /* BFD in/out Port Configuration */
    rv=bfd_ipv6_service_init(unit,port,0,port,0,0,bfd_remote_port);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_service_init\n");
        return rv;
    }

    /* BFD AuthType Configuration */
    if (auth_type == bcmBFDAuthTypeSimplePassword) {
        printf("Step: bfd_ipv6_auth_sp_set\n");
        rv = bfd_ipv6_auth_sp_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    } else if (auth_type == bcmBFDAuthTypeKeyedSHA1) {
        printf("Step: bfd_ipv6_auth_sha1_set\n");
        rv = bfd_ipv6_auth_sha1_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sha1_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    if (event_on == 1) {
        rv=bfd_ipv6_event_register(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_event_register (%s) \n",bcm_errmsg(rv));
            return rv;
        }
        printf ("\n\n ### BFD EVENT REGISTER is ON... ###\n");
    } else {
        printf ("\n\n ### BFD EVENT REGISTER is OFF... ###\n");
    }

    int endpoint_id=0;
    if (use_localdiscr_as_sess_id) {
       endpoint_id = local_disc; 
    }
    bfdv6_endpoint_1.endpoint_id=endpoint_id;
    if (session > 10) {
        verbose=0;
    }

    if (bfd_remote_port != BCM_GPORT_INVALID) {
        /* remote_gport field must be a trap */
        int trap_code;
        bcm_rx_trap_config_t trap_cpu;
        rv =  bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_code);
        if (rv != BCM_E_NONE) {
           printf("Error in bcm_rx_trap_type_create (%s) \n",bcm_errmsg(rv));
           return rv;
        }
        bcm_rx_trap_config_t_init(&trap_cpu);
        trap_cpu.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
        trap_cpu.dest_port = bfd_remote_port;
        if (bfd_fwd_off_en) {
            trap_cpu.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_cpu.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;
        }
        rv = bcm_rx_trap_set(unit, trap_code, &trap_cpu);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_rx_trap_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
        printf(" >>> Trap created trap_code=%d \n",trap_code);
        user_defined_trap_code = trap_code;
    }
    /* Create BFD IPv6 Endpoint  */
    for (i = 0; i < session; i++) {

        bfdv6_endpoint_1.session=i;

        printf("\n### Create: session %d at tx %d rx %d ###\n", i,local_min_tx,local_min_rx);
        if (i>=253 && !fixed_src_ipv6) {
           src_ipv6[14] = 0x01;
           printf(">>>> src_ipv6[14] is %x\n",src_ipv6[14]);
        }
        if (i>=254) {
           dst_ipv6[14] = 0x01;
           printf(">>>> dst_ipv6[14] is %x\n",dst_ipv6[14]);
        }
        in_vlan_tmp = in_vlan++;
        out_vlan_tmp = out_vlan++;
        out_vsi_tmp = out_vsi++;
        /* Port vlan translation configuration*/
        rv=port_vlan_translation_action_set(unit, in_vlan_tmp, out_vlan_tmp, out_vsi_tmp);
        if (rv != BCM_E_NONE) {
            printf("Error in port_vlan_translation_action_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }

        /* ip entry create */
        rv=bfd_ipv6_intf_init(unit,in_vlan_tmp,out_vlan_tmp,out_vsi_tmp,mac_address,next_hop_mac,src_ipv6,dst_ipv6);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_intf_init \n");
            return rv;
        }

        bfdv6_endpoint_1.bfd_encap_id = bfd_encap_id;

        /*
        print bfdv6_endpoint_1;
        */
        rv=bfd_ipv6_ep_create(unit, bfdv6_endpoint_1);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create \n");
            return rv;
        }

        bfdv6_endpoint_1.endpoint_id++;
        bfdv6_endpoint_1.udp_src_port++;
        bfdv6_endpoint_1.local_disc++;
        bfdv6_endpoint_1.remote_disc++;

        if (!fixed_src_ipv6) {
            src_ipv6[15]+=1;
        }
        dst_ipv6[15]+=1;
        bfdv6_endpoint_1.src_ipv6=src_ipv6;
        bfdv6_endpoint_1.dst_ipv6=dst_ipv6;
    }

    printf("\n\n ### bfd_ipv6_single_hop_field_action ### \n");
    rv=bfd_ipv6_single_hop_field_action(unit, is_inject, user_defined_trap_code);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_single_hop_field_action \n");
        print rv;
        return rv;
    }
    return rv;
}

/*print sessions_create_unit2(unit,120,13,1,1,0,0,5010,10,bcmBFDAuthTypeSimplePassword,3300,3300,50000,3,0,0);*/
int sessions_create_unit2(int unit, int session, int port, int is_single_hop, int is_acc, int is_passive, int is_demand, uint32 local_disc, uint32 remote_disc,
                            bcm_bfd_auth_type_t auth_type, uint32 local_min_tx, uint32 local_min_rx, uint32 udp_src_port, bcm_cos_t priority,
                            int event_on, int is_inject)
{
    int in_vlan = 200;
    int out_vlan = 2;
    int rv;
    int out_vsi = 11;
    int in_vlan_tmp, out_vlan_tmp, out_vsi_tmp;

    bcm_ip6_t src_ipv6 = {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01};
    bcm_ip6_t dst_ipv6= {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x02,0x00,0x02};

    /*  init the endpoint  */
    sal_memset(&bfdv6_endpoint_1, 0, sizeof(bfdv6_endpoint_1));
    bfdv6_endpoint_1.session=session;
    bfdv6_endpoint_1.src_ipv6 = src_ipv6;
    bfdv6_endpoint_1.dst_ipv6 = dst_ipv6;
    bfdv6_endpoint_1.is_single_hop = is_single_hop;
    bfdv6_endpoint_1.is_acc = is_acc;
    bfdv6_endpoint_1.is_passive = is_passive;
    bfdv6_endpoint_1.is_demand = is_demand;
    bfdv6_endpoint_1.local_disc = local_disc;
    bfdv6_endpoint_1.remote_disc = remote_disc;
    bfdv6_endpoint_1.auth_type = auth_type;
    bfdv6_endpoint_1.local_min_tx=local_min_tx;
    bfdv6_endpoint_1.local_min_rx=local_min_rx;
    bfdv6_endpoint_1.udp_src_port=udp_src_port;
    bfdv6_endpoint_1.priority=priority;

    bcm_mac_t next_hop_mac = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
    bcm_mac_t mac_address  = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};

    /* BFD event count */
    uint32 bfd_event_count[array_num] = {0};

    /* BFD in/out Port Configuration */
    rv=bfd_ipv6_service_init(unit,port,0,port,0,0,-1);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_service_init\n");
        print rv;
        return rv;
    }

    /* BFD AuthType Configuration */
    if (auth_type == bcmBFDAuthTypeSimplePassword) {
        printf("Step: bfd_ipv6_auth_sp_set\n");
        rv = bfd_ipv6_auth_sp_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    } else if (auth_type == bcmBFDAuthTypeKeyedSHA1) {
        printf("Step: bfd_ipv6_auth_sha1_set\n");
        rv = bfd_ipv6_auth_sha1_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sha1_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    if (event_on == 1) {
       /* BFD EVENT REGISTER */
        rv=bfd_ipv6_event_register(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_event_register (%s) \n",bcm_errmsg(rv));
            return rv;
        }
        printf ("\n\n ### BFD EVENT REGISTER is ON... ###\n");
    } else {
        printf ("\n\n ### BFD EVENT REGISTER is OFF... ###\n");
    }

    int endpoint_id=0;
    bfdv6_endpoint_1.endpoint_id=endpoint_id;
    if (session > 10) {
        verbose=0;
    }

    /*  Create BFD Endpoint */
    for (i = 0; i < session; i++) {
        bfdv6_endpoint_1.session=i;
        printf("\n### Create: session %d at tx %d rx %d ###\n", i,local_min_tx,local_min_rx);
        if (i>255 && src_ipv6[14]==0x00) {
           src_ipv6[14]+=1;
        }

        in_vlan_tmp=in_vlan++;
        out_vlan_tmp=out_vlan++;
        out_vsi_tmp=out_vsi++;

        /* Port vlan translation configuration*/
        rv=port_vlan_translation_action_set(unit, in_vlan_tmp, out_vlan_tmp, out_vsi_tmp);
        if (rv != BCM_E_NONE) {
            printf("Error in port_vlan_translation_action_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }

        /* ip entry create */
        rv=bfd_ipv6_intf_init(unit,in_vlan_tmp,out_vlan_tmp,out_vsi_tmp,mac_address,next_hop_mac,src_ipv6,dst_ipv6);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_intf_init \n");
            print rv;
            return rv;
        }
        bfdv6_endpoint_1.bfd_encap_id=bfd_encap_id;

        rv=bfd_ipv6_ep_create(unit, bfdv6_endpoint_1);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create \n");
            print rv;
            return rv;
        }

        bfdv6_endpoint_1.endpoint_id++;
        bfdv6_endpoint_1.udp_src_port++;
        bfdv6_endpoint_1.local_disc++;
        bfdv6_endpoint_1.remote_disc++;

        src_ipv6[15]+=1;
        dst_ipv6[15]+=1;
        bfdv6_endpoint_1.src_ipv6=src_ipv6;
        bfdv6_endpoint_1.dst_ipv6=dst_ipv6;
    }

    printf("\n\n ### bfd_ipv6_single_hop_field_action ### \n");
    rv=bfd_ipv6_single_hop_field_action(unit, is_inject, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_single_hop_field_action\n");
        print rv;
        return rv;
    }

    return rv;
}


int bfd_ipv6_ep_id_test(int unit, int port, int is_single_hop, int is_acc, int is_passive, int is_demand, bcm_bfd_auth_type_t auth_type, uint32 auth_index, int is_inject ) {

    int rv;
    verbose=0;

    /*  init the endpoint  */
    sal_memset(&bfdv6_endpoint_1, 0, sizeof(bfdv6_endpoint_1));
    bfdv6_endpoint_1.udp_src_port=50000;
    bfdv6_endpoint_1.local_disc = 5010;
    bfdv6_endpoint_1.remote_disc = 10;
    bfdv6_endpoint_1.src_ipv6 = src_ipv6;
    bfdv6_endpoint_1.dst_ipv6 = dst_ipv6;
    bfdv6_endpoint_1.is_single_hop = is_single_hop;
    bfdv6_endpoint_1.is_acc = is_acc;
    bfdv6_endpoint_1.is_passive = is_passive;
    bfdv6_endpoint_1.is_demand = is_demand;
    bfdv6_endpoint_1.auth_type = auth_type;
    bfdv6_endpoint_1.auth_index = auth_index;


    /* select random endpoint id to test */
    int endpoint_id_zero=0;
    int endpoint_id_rand=sal_rand()%16;

    /* 1. endpoint_id_zero */
    printf("\n\n--------------- %d -------------- \n", endpoint_id_zero);
    bfdv6_endpoint_1.endpoint_id=endpoint_id_zero;
    print bfdv6_endpoint_1;
    rv=bfd_ipv6_ep_create_example(unit,port,port,1,bfdv6_endpoint_1,is_inject);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_create_example for endpoint id %d\n", endpoint_id_zero);
        print rv;
        return rv;
    }

    /* 1.1 create the existing entry */
    printf("\nTest: create an existing entry \n");
    rv=bfd_ipv6_ep_create_example(unit,port,port,0,bfdv6_endpoint_1,is_inject);
    if (rv != BCM_E_EXISTS) {
        printf("Error in bfd_ipv6_ep_create_example for endpoint id %d\n", endpoint_id_zero);
        print rv;
        return rv;
    }

    /* 1.2 destroy the existing entry */
    printf("\nTest: destroy an existing entry \n");
    rv=bfd_ipv6_ep_destroy(unit,endpoint_id_zero);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_destroy for endpoint id %d\n", endpoint_id_zero);
        print rv;
        return rv;
    }
    /* 1.3 destroy the non-existing entry */
    printf("\nTest: destroy the non-existing entry \n");
    rv=bfd_ipv6_ep_destroy(unit,endpoint_id_zero);
    if (rv != BCM_E_NOT_FOUND) {
        printf("Error in bfd_ipv6_ep_destroy for endpoint id %d\n", endpoint_id_zero);
        print rv;
        return rv;
    }

    /* 2. Test endpoint id out-of-range */
    int endpoint_id_invalid=-1;
    int endpoint_id_out_of_range=2047;
    bfdv6_endpoint_1.endpoint_id=endpoint_id_invalid;
    /* 2.1 */
    printf("\n\n--------------- %d -------------- \n", endpoint_id_invalid);
    rv=bfd_ipv6_ep_create_example(unit,port,port,0,bfdv6_endpoint_1, is_inject);
    if (rv != BCM_E_PARAM) {
        printf("Error in bfd_ipv6_ep_create_example for endpoint id %d\n", endpoint_id_invalid);
        print rv;
        return rv;
    }
    /* 2.2 */
    bfdv6_endpoint_1.endpoint_id=endpoint_id_out_of_range;
    printf("\n\n--------------- %d -------------- \n", endpoint_id_out_of_range);
    rv=bfd_ipv6_ep_create_example(unit,port,port,0,bfdv6_endpoint_1, is_inject);
    if (rv != BCM_E_PARAM) {
        printf("Error in bfd_ipv6_ep_create_example for endpoint id %d\n", endpoint_id_out_of_range);
        print rv;
        return rv;
    }

    /* 3. endpoint_id_rand */
    bfdv6_endpoint_1.endpoint_id=endpoint_id_rand;
    printf("\n\n--------------- %d -------------- \n", endpoint_id_rand);
    rv=bfd_ipv6_ep_create_example(unit,port,port,0,bfdv6_endpoint_1, is_inject);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_create_example for endpoint id %d\n", endpoint_id_rand);
        print rv;
        return rv;
    }

    rv=bfd_ipv6_ep_destroy(unit,endpoint_id_rand);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_destroy for endpoint id %d\n", endpoint_id_rand);
        print rv;
        return rv;
    }

    return rv;
}


int bfd_ipv6_ep_udpport_test(int unit, int port, int is_single_hop, int is_acc, int is_passive, int is_demand, bcm_bfd_auth_type_t auth_type, uint32 auth_index, int is_inject ) {
    int rv;
    int endpoint_id=0;
    verbose=0;

    /* init the endpoint */
    sal_memset(&bfdv6_endpoint_1, 0, sizeof(bfdv6_endpoint_1));
    bfdv6_endpoint_1.endpoint_id=endpoint_id;
    bfdv6_endpoint_1.local_disc=5010;
    bfdv6_endpoint_1.remote_disc=10;
    bfdv6_endpoint_1.src_ipv6 = src_ipv6;
    bfdv6_endpoint_1.dst_ipv6 = dst_ipv6;
    bfdv6_endpoint_1.is_single_hop = is_single_hop;
    bfdv6_endpoint_1.is_acc = is_acc;
    bfdv6_endpoint_1.is_passive = is_passive;
    bfdv6_endpoint_1.is_demand = is_demand;
    bfdv6_endpoint_1.auth_type = auth_type;
    bfdv6_endpoint_1.auth_index = auth_index;
    /* select random endpoint id to test */
    int udp_src_port_low=49152;
    int udp_src_port_high=65535;

    /* 1. udp_src_port_low */
    bfdv6_endpoint_1.udp_src_port=udp_src_port_low;
    printf("\n\n--------------- udp_src_port : %d -------------- \n", udp_src_port_low);
    rv=bfd_ipv6_ep_create_example(unit,port,port,1,bfdv6_endpoint_1, is_inject);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_create_example for endpoint %d, udp_src_port %d \n", endpoint_id, udp_src_port_low);
        print rv;
        return rv;
    }

    rv=bfd_ipv6_ep_destroy(unit,endpoint_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_destroy for endpoint id %d\n", endpoint_id);
        print rv;
        return rv;
    }

    /* 2. udp_src_port_high */
    bfdv6_endpoint_1.udp_src_port=udp_src_port_high;
    printf("\n\n--------------- udp_src_port : %d -------------- \n", udp_src_port_high);
    rv=bfd_ipv6_ep_create_example(unit,port,port,0,bfdv6_endpoint_1, is_inject);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_create_example for endpoint %d, udp_src_port %d \n", endpoint_id, udp_src_port_high);
        print rv;
        return rv;
    }

    rv=bfd_ipv6_ep_destroy(unit,endpoint_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_destroy for endpoint id %d\n", endpoint_id);
        print rv;
        return rv;
    }
    return rv;
}


int bfd_ipv6_ep_auth_index_test(int unit, int port, int is_single_hop, int is_acc, int is_passive, int is_demand, int is_inject ) {
    int rv;
    int endpoint_id=0;

    int is_init=1;
    uint32 auth_index;
    verbose=0;

    /* init the endpoint */
    sal_memset(&bfdv6_endpoint_1, 0, sizeof(bfdv6_endpoint_1));
    bfdv6_endpoint_1.endpoint_id=endpoint_id;
    bfdv6_endpoint_1.local_disc=5010;
    bfdv6_endpoint_1.remote_disc=10;
    bfdv6_endpoint_1.udp_src_port = 50000;
    bfdv6_endpoint_1.src_ipv6 = src_ipv6;
    bfdv6_endpoint_1.dst_ipv6 = dst_ipv6;
    bfdv6_endpoint_1.is_single_hop = is_single_hop;
    bfdv6_endpoint_1.is_acc = is_acc;
    bfdv6_endpoint_1.is_passive = is_passive;
    bfdv6_endpoint_1.is_demand = is_demand;

    /* 1. bcmBFDAuthTypeKeyedSHA1 */
    bfdv6_endpoint_1.auth_type = bcmBFDAuthTypeKeyedSHA1;
    for ( auth_index=0; auth_index < auth_sha1_num; auth_index++) {
        bfdv6_endpoint_1.auth_index = auth_index;
        printf("\n\n--------------- auth_index : %d -------------- \n", auth_index);
        rv=bfd_ipv6_ep_create_example(unit,port,port,is_init,bfdv6_endpoint_1, is_inject);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create_example with bcmBFDAuthTypeKeyedSHA1 for endpoint %d, auth_index %d \n", endpoint_id, auth_index);
            print rv;
            return rv;
        }

        is_init=0;

        rv=bfd_ipv6_ep_destroy(unit,endpoint_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_destroy for endpoint id %d\n", endpoint_id);
            print rv;
            return rv;
        }
    }

    /* 2. bcmBFDAuthTypeSimplePassword */
    bfdv6_endpoint_1.auth_type = bcmBFDAuthTypeSimplePassword;
    for (auth_index=0; auth_index < auth_sp_num; auth_index++) {
        bfdv6_endpoint_1.auth_index = auth_index;
        printf("\n\n--------------- auth_index : %d -------------- \n", auth_index);
        rv=bfd_ipv6_ep_create_example(unit,port,port,is_init,bfdv6_endpoint_1, is_inject);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create_example with bcmBFDAuthTypeSimplePassword for endpoint %d, auth_index %d \n", endpoint_id, auth_index);
            print rv;
            return rv;
        }

        rv=bfd_ipv6_ep_destroy(unit,endpoint_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_destroy for endpoint id %d\n", endpoint_id);
            print rv;
            return rv;
        }
    }
    return rv;
}


int multiple_sessions_event_check_example(int unit, int port1, int port2,int is_init, bfd_over_ipv6_ep_create_info_s bfdv6_ep, int is_inject ) {

    int in_vlan = 2;
    int out_vlan = 200;
    int rv;
    int out_vsi = 11;
    int session = bfdv6_ep.session;
    int endpoint_id = bfdv6_ep.endpoint_id;

    int in_vlan_tmp, out_vlan_tmp, out_vsi_tmp;

    if (bfdv6_ep.priority == 0 ) {
        bfdv6_ep.priority = 5;
    }
    if (bfdv6_ep.local_min_tx == 0 ) {
        bfdv6_ep.local_min_tx = 10000;
    }
    if (bfdv6_ep.local_min_rx == 0 ) {
        bfdv6_ep.local_min_rx = 10000;
    }
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};

    /* BFD event count */
    uint32 bfd_event_count[array_num] = {0};


    if (is_init == 1) {
        /* BFD in/out Port Configuration */
        rv=bfd_ipv6_service_init(unit,port1,0,port2,0,0,-1);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_service_init\n");
            print rv;
            return rv;
        }
    }

  /* BFD AuthType Configuration */
    if (bfdv6_ep.auth_type == bcmBFDAuthTypeSimplePassword) {
        printf("Step: bfd_ipv6_auth_sp_set\n");
        rv = bfd_ipv6_auth_sp_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    } else if (bfdv6_ep.auth_type == bcmBFDAuthTypeKeyedSHA1) {
        printf("Step: bfd_ipv6_auth_sha1_set\n");
        rv = bfd_ipv6_auth_sha1_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sha1_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    /* Enable BFD Event */
    rv=bcm_bfd_ipv6_events_register(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_events_register (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf ("\n\n ### BFD EVENT REGISTER is ON... ###\n");

    if (session > 10) {
        verbose=0;
    }
    /* Create BFD IPv6 Endpoint  */
    for (i = 0; i < session; i++) {
        bfdv6_ep.session=i;

        printf("\n### Create: session %d at tx %d rx %d ###\n", i,bfdv6_ep.local_min_tx,bfdv6_ep.local_min_rx);
        if (i>255 && src_ipv6[14]==0x00) {
           src_ipv6[14]+=1;
        }

        in_vlan_tmp=in_vlan++;
        out_vlan_tmp=out_vlan++;
        out_vsi_tmp=out_vsi++;

        /* Port vlan translation configuration*/
        rv=port_vlan_translation_action_set(unit, in_vlan_tmp, out_vlan_tmp, out_vsi_tmp);
        if (rv != BCM_E_NONE) {
            printf("Error in port_vlan_translation_action_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }

        /* ip entry create */
        rv=bfd_ipv6_intf_init(unit,in_vlan_tmp,out_vlan_tmp,out_vsi_tmp,mac_address,next_hop_mac,src_ipv6,dst_ipv6);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_intf_init \n");
            print rv;
            return rv;
        }

        bfdv6_ep.bfd_encap_id=bfd_encap_id;

        rv=bfd_ipv6_ep_create(unit,bfdv6_ep);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create \n");
            print rv;
            return rv;
        }
        bfdv6_ep.endpoint_id++;
        bfdv6_ep.udp_src_port++;
        bfdv6_ep.local_disc++;
        bfdv6_ep.remote_disc++;

        src_ipv6[15]+=1;
        dst_ipv6[15]+=1;
        bfdv6_ep.src_ipv6=src_ipv6;
        bfdv6_ep.dst_ipv6=dst_ipv6;
    }

    printf("\n\n ### bfd_ipv6_single_hop_field_action ### \n");
    rv=bfd_ipv6_single_hop_field_action(unit, is_inject, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_single_hop_field_action \n");
        print rv;
        return rv;
    }
    return rv;
}




int bfd_scale_sessions_create_and_delete(int unit, int session, int port, int is_single_hop, int is_acc, int is_passive, int is_demand, uint32 local_disc, uint32 remote_disc,
                            bcm_bfd_auth_type_t auth_type, uint32 local_min_tx, uint32 local_min_rx, uint32 udp_src_port, bcm_cos_t priority,
                            int event_on, int is_inject)
{
    int in_vlan = 2;
    int out_vlan = 200;
    int rv;
    int out_vsi = 11;
    int in_vlan_tmp, out_vlan_tmp, out_vsi_tmp;
    int loop = 0, loop_count = 10;

    /* Used to avoid logs*/
    scale_test = 1;

    /*  init the endpoint  */
    sal_memset(&bfdv6_endpoint_1, 0, sizeof(bfdv6_endpoint_1));
    bfdv6_endpoint_1.session=session;
    bfdv6_endpoint_1.src_ipv6 = src_ipv6;
    bfdv6_endpoint_1.dst_ipv6 = dst_ipv6;
    bfdv6_endpoint_1.is_single_hop = is_single_hop;
    bfdv6_endpoint_1.is_acc = is_acc;
    bfdv6_endpoint_1.is_passive = is_passive;
    bfdv6_endpoint_1.is_demand = is_demand;
    bfdv6_endpoint_1.local_disc = local_disc;
    bfdv6_endpoint_1.remote_disc = remote_disc;
    bfdv6_endpoint_1.auth_type = auth_type;
    bfdv6_endpoint_1.local_min_tx=local_min_tx;
    bfdv6_endpoint_1.local_min_rx=local_min_rx;
    bfdv6_endpoint_1.udp_src_port=udp_src_port;
    bfdv6_endpoint_1.priority=priority;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};

    /* BFD event count */
    uint32 bfd_event_count[array_num] = {0};

    /* BFD in/out Port Configuration */
    rv=bfd_ipv6_service_init(unit,port,0,port,0,0,-1);
    if (rv != BCM_E_NONE) {
        scale_test = 0;
        printf("Error in bfd_ipv6_service_init\n");
        print rv;
        return rv;
    }

    /* BFD AuthType Configuration */
    if (auth_type == bcmBFDAuthTypeSimplePassword) {
        printf("Step: bfd_ipv6_auth_sp_set\n");
        rv = bfd_ipv6_auth_sp_set(unit);
        if (rv != BCM_E_NONE) {
            scale_test = 0;
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    } else if (auth_type == bcmBFDAuthTypeKeyedSHA1) {
        printf("Step: bfd_ipv6_auth_sha1_set\n");
        rv = bfd_ipv6_auth_sha1_set(unit);
        if (rv != BCM_E_NONE) {
            scale_test = 0;
            printf("Error in bfd_ipv6_auth_sha1_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    if (event_on == 1) {
        rv=bfd_ipv6_event_register(unit);
        if (rv != BCM_E_NONE) {
            scale_test = 0;
            printf("Error in bfd_ipv6_event_register (%s) \n",bcm_errmsg(rv));
            return rv;
        }
        printf ("\n\n ### BFD EVENT REGISTER is ON... ###\n");
    } else {
        printf ("\n\n ### BFD EVENT REGISTER is OFF... ###\n");
    }

    int endpoint_id=0;
    bfdv6_endpoint_1.endpoint_id=endpoint_id;
    if (session > 10) {
        verbose=0;
    }
    /* Create BFD IPv6 Endpoint  */
    for (i = 0; i < session; i++) {

        bfdv6_endpoint_1.session=i;

        printf("\n### Create: session %d at tx %d rx %d ###\n", i,local_min_tx,local_min_rx);
        if (i>255 && src_ipv6[14]==0x00) {
           src_ipv6[14]+=1;
        }

        in_vlan_tmp=in_vlan++;
        out_vlan_tmp=out_vlan++;
        out_vsi_tmp=out_vsi++;
        /* Port vlan translation configuration*/
        rv=port_vlan_translation_action_set(unit, in_vlan_tmp, out_vlan_tmp, out_vsi_tmp);
        if (rv != BCM_E_NONE) {
            scale_test = 0;
            printf("Error in port_vlan_translation_action_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }

        /* ip entry create */
        rv=bfd_ipv6_intf_init(unit,in_vlan_tmp,out_vlan_tmp,out_vsi_tmp,mac_address,next_hop_mac,src_ipv6,dst_ipv6);
        if (rv != BCM_E_NONE) {
            scale_test = 0;
            printf("Error in bfd_ipv6_intf_init \n");
            print rv;
            return rv;
        }

        bfdv6_endpoint_1.bfd_encap_id=bfd_encap_id;

        rv=bfd_ipv6_ep_create(unit, bfdv6_endpoint_1);
        if (rv != BCM_E_NONE) {
            scale_test = 0;
            printf("Error in bfd_ipv6_ep_create \n");
            print rv;
            return rv;
        }

        bfdv6_endpoint_1.endpoint_id++;
        bfdv6_endpoint_1.udp_src_port++;
        bfdv6_endpoint_1.local_disc++;
        bfdv6_endpoint_1.remote_disc++;

        src_ipv6[15]+=1;
        dst_ipv6[15]+=1;
        bfdv6_endpoint_1.src_ipv6=src_ipv6;
        bfdv6_endpoint_1.dst_ipv6=dst_ipv6;
    }

    printf("\n\n ### bfd_ipv6_single_hop_field_action ### \n");
    rv=bfd_ipv6_single_hop_field_action(unit, is_inject, 0);
    if (rv != BCM_E_NONE) {
        scale_test = 0;
        printf("Error in bfd_ipv6_single_hop_field_action \n");
        print rv;
        return rv;
    }

    rv = bcm_bfd_endpoint_destroy_all(unit);
    if (rv != BCM_E_NONE) {
         scale_test = 0;
         printf("\nError in bcm_bfd_endpoint_destroy_all\n");
         print rv;
         return rv;
    }

    for (loop = 0; loop <= loop_count; loop++) {
        printf("\nLoop $loop - create $session sessions\n");
        for (i = 0; i <session ;  i++) {
            rv = sessions_recreate(unit, i, port, is_single_hop, is_acc, is_passive, is_demand, local_disc, remote_disc ,
                auth_type, local_min_tx, local_min_rx, udp_src_port, priority);
            if (rv != BCM_E_NONE) {
                scale_test = 0;
                printf("\nError in sessions_recreate \n");
                print rv;
                return rv;
            }
        }
        printf("\nLoop $loop - delete all\n");
        rv = bcm_bfd_endpoint_destroy_all(unit);
        if (rv != BCM_E_NONE) {
            scale_test = 0;
            printf("\nError in bcm_bfd_endpoint_destroy_all\n");
            print rv;
            return rv;
        }
    }
    scale_test = 0;
    return rv;
}


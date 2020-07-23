/* 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/* 
 * This test demonstrates Routing Over Trill unicast ingress and egress scenarios.
 * 
 * Terminology:
 * RBB: RBridge Bridge. RBridge for the bridging case. 
 *      
 * RBR: RBridge Router. RBridge for the routing case.
 *      The RBR has the RBridge functionalities + the routing functionalities.
 * Provider Port: switch's physical port at the trill campus side. Trill campus is the provider network. 
 * Access Port: switch's physical port at the customer side. 
 * 
 * Network diagram
 * We configure "local RBR"
 *                                ______________________________________________
 *                               |                                             |
 *                            ___|____                  ________           ____|____ 
 *   __________   access     | Local  |    designated  | Transit|         | Remote |     __________  
 *  | Router A |___vlan______| RBR    |______vlan______| RB     |_ ...  __| RBR    |____| Router B |
 *  |          |             |________|                |________|         |________|    |          | 
 *  ------------                 |                                             |        ------------                         
 *     |                         |                                             |           |         
 *     |                         |              TRILL CAMPUS                   |           |         
 *     |                         |_____________________________________________|           |         
 *    host10                                                                              host90     
 *    host11                                                                              host91     
 *  
 *  
 *  
 *  
 *  
 *  
 * Ingress Trill: Traffic from host10 to host90
 *  
 *    Purpose: - check lookup in host table result as trill tunnel (encapsulate Trill header and Ethernet header)
 *             and native Ethernet encapsulation. 
 *    Send
 *             ----------------------------------------------
 *        eth: |    DA         |     SA      | VLAN         |
 *             ----------------------------------------------
 *             | local_RBR_mac | routerA_mac |  access_vlan |  
 *             ---------------------------------------------
 *                 ------------------------- 
 *             ip: |   SIP     | DIP        | 
 *                 ------------------------- 
 *                 | host10_ip |  host90_ip | 
 *                 -------------------------
 *    Receive:
 *             -----------------------------------------------------
 *        eth: |    DA           |     SA        |      VLAN         |
 *             -----------------------------------------------------
 *             | transit_RB_mac  | local_RBR_mac |  designated_vlan  |  
 *             -----------------------------------------------------
 *                    --------------------------------------------
 *             Trill: |   Ingress Nick     | Egress Nick         | 
 *                    -------------------------------------------- 
 *                    | local_RBR_nickname | remote_RBR_nickname | 
 *                    --------------------------------------------
 *                                 -----------------------------------------------      
 *                     native eth: |    DA          |     SA        | VLAN       |     
 *                                 -----------------------------------------------      
 *                                 | remote_RBR_mac | local_RBR_mac | global_vpn |     
 *                                 -----------------------------------------------      
 *                                    -------------------------      
 *                         native ip: |   SIP     | DIP        |     
 *                                    -------------------------      
 *                                    | host10_ip |  host90_ip |     
 *                                    -------------------------      
 *
 * Packet flow:
 *   - get the VSI from the VLAN
 *   - DA = myMac, so do routing,
 *   - get the inRif (= vsi)
 *   - packet is forwarded according to DIP and VRF (inRif.vrf)
 *   - result encapsulate with ethernet header (native LL) and Trill encapsulation (Trill header + ethernet header)
 *     - result of forwarding contains information to build native LL: arp entry (LL eedb) and outRif.
 *       (DA = arp_entry.DA SA= outRif.myMac  VLAN= VSI (=outRif))
 *     - result of forwarding contains intormation to build Trill encapsulation: fec.
 *       - Fec is resolved and contain Trill eedb entry.
 *       - Trill header: ingress nickname = trill_eedb.ingress_nickname, egress_nickname = trill_eedb.egress_nickname, m = trill_eedb.m
 *       - ethernet header: trill eedb entry point to an arp entry (LL eedb entry):
 *         DA = arp_entry.DA SA= arp_entry.SA VLAN= arp_entry.vlan
 * 
 * 
 *Egress Trill: Traffic from host91 to host11: 
 * 
 *Purpose: 
 * - check Trill termination (terminate Ethernet header and Trill header)
 * - check 2nd my mac termination
 * - check packet is routed 
 * - check packet is encapsulated with new ethernet header
 * 
 *    Send:
 *             ------------------------------------------------------
 *        eth: |    DA          |     SA          |      VLAN        |
 *             ------------------------------------------------------
 *             | local_RBR_mac  | transit_RB_mac  |  designated_vlan |  
 *             ------------------------------------------------------
 *                    --------------------------------------------
 *             Trill: |   Ingress Nick       | Egress Nick        | 
 *                    --------------------------------------------
 *                    | remote_RBR_nickname  | local_RBR_nickname | 
 *                    --------------------------------------------
 *                                 ------------------------------------------------      
 *                     native eth: |    DA         |     SA          | VLAN       |     
 *                                 ------------------------------------------------      
 *                                 | local_RBR_mac | remote_RBR_mac  | global_vpn |     
 *                                 ------------------------------------------------      
 *                                    -------------------------      
 *                         native ip: |   SIP     | DIP        |     
 *                                    -------------------------      
 *                                    | host91_ip |  host11_ip |     
 *                                    -------------------------
 * 
 *    Receive:
 *             ----------------------------------------------
 *        eth: |    DA       |     SA      | VLAN           |
 *             ----------------------------------------------
 *             | routerA_mac | local_RBR_mac |  access_vlan |  
 *             ---------------------------------------------
 *                 ------------------------- 
 *             ip: |   SIP     | DIP        | 
 *                 ------------------------- 
 *                 | host91_ip |  host11_ip | 
 *                 -------------------------
 * 
 *Packet flow: 
 * - get the VSI from the VLAN
 * - DA = myMac, terminate ethernet header, so do routing,
 * - egress Trill nickname is "my nickname" so terminate trill header,
 * - skip ethernet is enabled for my nickname, so terminate the ethernet header 
 * - VL mode, so get VSI from vlan at inner ethernet.
 * - get the inRif (inRif=VSI)
 * - packet is forwarded according to DIP and VRF (inRif.vrf)
 * - do routing
 * 
 * 
 * Limitations:
 * - RIF is <=4k, so VSI (or VPN) (inRif=VSI) is also limited to 4k.
 *   At egress trill for the VPN resolution (after trill termination), vpn is limited to 4k.
 * - In arad+, local RBR nickname is unique for routing and must be different than RBB nickname.
 *   In arad+, skip ethernet (skip ethernet header after terminating Trill header) is performed per local nickname. 
 * 
 * 
 * 
 * Configuration:
 * 
 * soc properties:
 * bcm886xx_auxiliary_table_mode=1 (for arad/+ devices only)
 * #enable Trill. (allow VL and FGL mode)
 * trill_mode=2
 * #enable ROO for Trill
 * bcm886xx_roo_enable=1
 * 
 *  
 * cint src/examples/dpp/utility/cint_utils_l3.c 
 * cint src/examples/dpp/cint_ip_route.c  
 * cint src/examples/dpp/cint_port_tpid.c 
 * cint src/examples/dpp/cint_pmf_trill_2pass_snoop.c 
 * cint src/examples/dpp/cint_pmf_2pass_snoop.c 
 * cint src/examples/dpp/cint_trill.c 
 * cint src/examples/dpp/cint_trill_roo.c
 * cint
 * int unit=0;
 * int accessP = 200;
 * int providerP = 201;
 * trill_roo_run_default_with_ports(accessP, providerP); 
*/ 


int host90_ip = 0x5A5A5A5A; /* 90.90.90.90 */ 
int host11_ip = 0x0B0B0B0B; /* 11.11.11.11 */
int trill_eg_qos_id = -1;

struct cint_trill_roo_info_s {
    int access_port; /* access port */
    int provider_port; /* port at trill provider network side*/
    bcm_mac_t my_mac; /* my mac */
    bcm_mac_t router_a_mac; /* router a mac (access side), next hop for egress trill packet */
    bcm_mac_t transit_rb_mac; /* transit rb mac, next hop for ingress trill packet */
    bcm_mac_t remote_rbr_mac; /* remote rb router. Next hop for native IP. For ingress trill packet */
    int designated_vlan; /* vlan at provider side */
    int access_vlan;     /* vlan at access side */
    int global_vpn;    /* vlan of the native ethernet, identify the service inside the Trill campus */
    int local_rbr_nickname; /* my nickname */  
    int remote_rbr_nickname; /* nickname for remote RBR */  
    int native_ll_outlif; /* native LL, interface of type encap: outlif (native arp eedb index) */
}; 

cint_trill_roo_info_s cint_trill_roo_info = 
{
/* ports :
   access port | provider port */
200,            201 ,          
/* my mac                             | router_a mac                       | transit_RB_mac                        | remote rbr */
{0x00, 0x00, 0x00, 0x00, 0x11, 0x11}, {0x00, 0x00, 0x00, 0x00, 0xAA, 0xAA},  {0x00, 0x00, 0x00, 0x00, 0x22, 0x22}, {0x00, 0x00, 0x00, 0x00, 0x99, 0x99},
/* designated vlan | access vlan | global vpn */
10,                  20,           30,
/* nicknames:
   my_nickname | remote rb nickname  | native LL outlif*/
0x1234,          0xAA11,               0x3006
};

/*remark DSCP to new TOS 35*/
int qos_map_egress_dscp_profile(int unit)
{
    bcm_qos_map_t l3_eg_map;
    int dscp, dp;
    int rv;

    /* Create QOS (Remark) profile ID */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS, &trill_eg_qos_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create QoS ID, bcm_qos_map_create with flag BCM_QOS_MAP_EGRESS\n");
        return rv;
    }
    printf("created Remark-profile-id =0x%08x, \n", trill_eg_qos_id);

    /* set QoS mapping for L3 in egress:
       map Out-DSCP-EXP (TOS) = In-DSCP-EXP + DP */
    for (dscp = 0; dscp < 256; dscp++) {
        for (dp =0; dp <4; dp++) {
            bcm_qos_map_t_init(&l3_eg_map);
            l3_eg_map.int_pri = dscp; /* in-DSCP-EXP */
            l3_eg_map.color = dp;
            l3_eg_map.remark_int_pri = 35; /* TOS */

            rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3, l3_eg_map, trill_eg_qos_id);
            if (rv != BCM_E_NONE) {
                printf("Error, set egress QoS mapping, bcm_qos_map_add, dscp=%d \n", dscp);
                return rv;
            }
        }
    }
    printf("Set egress QoS mapping for L3\n");

    return rv;
}


int 
trill_roo_info_init(int unit, int accessPort, int providerPort) {
    cint_trill_roo_info.access_port = accessPort; 
    cint_trill_roo_info.provider_port = providerPort; 
}

/* convert from egress trill gport to l3 intf object
 * gport and l3_if encoding: 
 * egress trill gport format:  {gport type[31:26] gport_sub_type[25:22] out-lif[17:0]}
 * l3_if:                      { type[32:29]  value[28:0]}
 * 
 * Conversion:
 * l3_if.type: encap_type, l3_if.value: gport.out-lif 
 */
int 
trill_roo_egress_trill_gport_to_l3_intf(bcm_gport_t egress_trill_gport,  bcm_if_t* l3_if) {
    int out_lif_mask = 0x3FFFF; 
    int itf_encap = 0x2; 
    int itf_encap_shift = 29; 
    /* l3_if.type: encap_type, l3_if.value: gport.out-lif */
    *l3_if = (egress_trill_gport & out_lif_mask) | (itf_encap << itf_encap_shift); 
}

/* 
 * add remote rbridge router:
 * Allocates a Trill EEDB entry  
 * Allocates an arp entry (LL EEDB entry) for Trill. 
 * Allocates a Trill FEC, and point it to the allocated Trill EEDB entry.
 * params:
 * my_mac: SA in LL encapsulation
 * next_hop: DA in LL encapsulation
 * outer_vlan: VLAN in LL encapsulation
 * network_port: destination port for Trill FEC
 * my_nickname: ingress-nickname for Trill encapsulation.
 *              Must already exist as local trill port.
 *              See local trill configuration (config_local_rbridge function)
 * nickname: egress-nickname for Trill encapsulation
 * fec_if_id: Returned value: l3 intf of type fec which contain trill fec. 
 */ 
int
trill_roo_add_remote_rbridge_router(int unit, 
                                    bcm_mac_t my_mac, 
                                    bcm_mac_t next_hop_mac, 
                                    bcm_vlan_t outer_vlan, 
                                    int network_port, 
                                    int my_nickname, 
                                    int nickname, 
                                    bcm_if_t *fec_if_id) {
    int rv = 0;
    
    /*** build Trill Next hop encapsulation ***/

    /* In ROO, the overlay LL encapsulation is built with a different API call 
       (bcm_l2_egress_create instead of bcm_l3_egress create) */
    bcm_l2_egress_t l2_egress_overlay;
    bcm_l2_egress_t_init(&l2_egress_overlay);

    l2_egress_overlay.dest_mac   = next_hop_mac; /* next hop */
    l2_egress_overlay.src_mac    = my_mac;       /* my-mac */
    l2_egress_overlay.outer_vlan = outer_vlan;   /* PCP DEI (0) + outer_vlan */
    l2_egress_overlay.ethertype  = trill_ethertype; /* trill ethertype, from cint_trill.c */
    l2_egress_overlay.outer_tpid = 0x8100;       /* outer tpid */

    /* For QAX devices, use bcm_l3_egress API. 
     * QAX note: 
     * We provide roo__overlay_link_layer_create and
     * roo__overlay_eve to help to migrate from Jericho to QAX  
     */
    rv = roo__overlay_link_layer_create(unit, &l2_egress_overlay,
                                        0, /* l3 flags */
                                        &(l2_egress_overlay.encap_id),
                                        0);
    if (verbose >= 2) {
        printf("Trill Next hop encapsulation created: 0x%x \n", l2_egress_overlay.encap_id);
    }

    /*** build Trill encapsulation ***/
    /* contains all information for trill header and point to LL encapsulation */
    bcm_trill_port_t trill_port; 
    bcm_trill_port_t_init(&trill_port);
    trill_port.egress_if  =  l2_egress_overlay.encap_id; /* l3_itf object, represent Trill LL */
    trill_port.virtual_name = my_nickname; /* my nickname */
    trill_port.name       =  nickname;  
    trill_port.flags      = BCM_TRILL_PORT_EGRESS; 

    rv = bcm_trill_port_add(unit, &trill_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_add, \n");
        return rv;
    }
    
    if (verbose >= 2) {
        printf("Trill encapsulation created: 0x%x \n", trill_port.trill_port_id);
    }

    if( trill_eg_qos_id != -1) {
        rv = bcm_qos_port_map_set(unit, trill_port.trill_port_id, -1, trill_eg_qos_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_qos_port_map_set fail\n");
        }
    }
    /*** build Trill FEC ***/
    create_l3_egress_s l3_egress_fec; 

    l3_egress_fec.out_gport = network_port; 
    trill_roo_egress_trill_gport_to_l3_intf(trill_port.trill_port_id, &(l3_egress_fec.out_tunnel_or_rif)); /* fec point to trill eedb */ 
    
    rv = l3__egress_only_fec__create(unit,&l3_egress_fec); 
    if (rv != BCM_E_NONE) {
        printf("Error, in l3__egress_only_fec__create, \n");
        return rv;
    }
    *fec_if_id = l3_egress_fec.fec_id;
    
    if (verbose >= 2) {
        printf("Build trill FEC: 0x%x \n", *fec_if_id); 
    }

    if (is_device_or_above(unit, JERICHO)) {
        /* create ecmp: ECMP points to ip tunnel FEC 
         * Jericho note: 
         * host entry can hold up to 12b of FEC id (4k ids)                                                    
         * In Jericho, ECMP is also 4k.                                                                        
         * Consequently, to add a host entry in jericho, we'll have to add a ECMP entry instead of the FEC. */  
        
        bcm_l3_egress_ecmp_t ecmp; 
        int ecmp_nof_paths = 1; 

        bcm_l3_egress_ecmp_t_init(&ecmp); 
        ecmp.max_paths = ecmp_nof_paths; 
        rv = bcm_l3_egress_ecmp_create(unit, &ecmp, 
                                       ecmp_nof_paths, /* nof paths */
                                       fec_if_id /* interface object of type FEC */
                                       ); 
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_egress_ecmp_create \n");
            return rv; 
        }
        *fec_if_id = ecmp.ecmp_intf;
        if (verbose >= 2) {
            printf("Build trill ECMP: 0x%x \n", *fec_if_id); 
        }
    }

    return rv;
}
 
int
trill_roo_run(int unit) {
    int rv = BCM_E_NONE;
    create_l3_intf_s intf;
 
    if (verbose >= 2) {
        printf("trill_roo_run \n");
    }

    /* Initialize trill module (SW related databases) */
    rv = bcm_trill_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_init\n");
        return rv;
    }
    
    /* Global config:
       - Hop count
       - EtherType
       - Acceptable frame type */
    rv = global_config(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in global_config\n");
        return rv;
    }

    /* port config
     * for egress Trill, designated vlan must be configured per port, otherwise the packet is trapped.
     */
    rv = port_config(unit, cint_trill_roo_info.provider_port, cint_trill_roo_info.designated_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config \n");
        return rv;
    }
                    
    /* Create RIF for IP routing
     * - inRif allow packet to be routed if packet.DA = rif.my_mac (for egress Trill)
       - outRif helps to build the native LL: SA and VLAN (for ingress Trill) */
    intf.my_global_mac = cint_trill_roo_info.my_mac;  /* my mac (global value) */
    intf.my_lsb_mac = cint_trill_roo_info.my_mac;  /* my mac (global value) */
    intf.vsi = cint_trill_roo_info.global_vpn; /* vlan = vsi = rif = l3_if object */
    if (trill_eg_qos_id != -1) {
        intf.qos_map_valid = 1;
        intf.qos_map_id = trill_eg_qos_id;
    }
    rv = l3__intf_rif__create(unit, &intf);
                                      
    if (rv != BCM_E_NONE) {
          printf("Fail  l3__intf_rif_with_id__create");
          return rv;
    }

    
    /* Add TRILL Virtual Port for local RB (configures my nickname)
     * - trill local port helps to build the trill header (for ingress Trill)
     * - trill local port terminates trill headers (for egress Trill)
     * - only for Arad+: trill local port skip native ethernet (for egress Trill) */
    rv = config_local_rbridge(unit, cint_trill_roo_info.local_rbr_nickname, BCM_TRILL_PORT_TERM_ETHERNET);
    if (rv != BCM_E_NONE) {
        printf("Error, in config_local_rbridge \n");
        return rv;
    }

    /* Trill only configuration. See cint_trill.c:l3_intf_create.
     * For egress Trill packets,
     * - inRif allow packet to be routed if packet.DA = rif.my_mac (for egress Trill)*/
    bcm_if_t if_id; 
    rv = l3_intf_create(unit, cint_trill_roo_info.my_mac, cint_trill_roo_info.designated_vlan, &l3_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in l3_intf_create: vsi=%d\n",cint_trill_roo_info.designated_vlan );
        return rv;
    }


    /* 
     * For ingress Trill packets. 
     * Add next hop for native IP:                                                             .
     * Allocate native arp entry (LL eedb entry for the native IP).
     *
     * Jericho note: nof bits for native outlif is limited in host_add api. (14b in arad, 15 in jericho)
     *                 so specify the outlif id to be sure it fits.
     */
    int native_ll_encap_id; 
    create_l3_egress_s l3_egress_encap;
    l3_egress_encap.out_gport = cint_trill_roo_info.provider_port; /* outgoing port */
    l3_egress_encap.vlan = cint_trill_roo_info.global_vpn; /* vlan at the LL */
    l3_egress_encap.next_hop_mac_addr = cint_trill_roo_info.remote_rbr_mac;  /* next hop mac */
    if (is_device_or_above(unit, JERICHO)) {
        l3_egress_encap.arp_encap_id = cint_trill_roo_info.native_ll_outlif;
    }

    /* qax note: BCM_L3_NATIVE_ENCAP is for QAX and above devices:
     * indicate we build a native LL.
     * (eedb entry is allocated in a native LL eedb phase)
     */
    if (is_device_or_above(unit,JERICHO_PLUS)) {
        l3_egress_encap.l3_flags = BCM_L3_NATIVE_ENCAP;
    }

    rv = l3__egress_only_encap__create(unit, &l3_egress_encap);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_encap__create\n");
      return rv;
    } 
    native_ll_encap_id = l3_egress_encap.arp_encap_id; /* Contains LL eedb index (l3_if object. Type encap) */
    if (verbose >= 2) {
        printf("Trill Native link layer encapsulation created: 0x%x \n", native_ll_encap_id);
    }

    /* Add remote RBridge: tril header and overlay LL header
     * - build the trill header
     * - build the LL header for trill
     * - return trill FEC */
    bcm_if_t fec_if_id = 0; 
    cint_trill_roo_info.native_ll_outlif = native_ll_encap_id;
    rv = trill_roo_add_remote_rbridge_router(unit, 
                                             cint_trill_roo_info.my_mac, /* Trill next hop: SA */
                                             cint_trill_roo_info.transit_rb_mac, /* Trill next hop: DA */
                                             cint_trill_roo_info.designated_vlan, /* Trill next hop: outer vlan */
                                             cint_trill_roo_info.provider_port, /* network port */
                                             cint_trill_roo_info.local_rbr_nickname, /* Trill: ingress nickname */
                                             cint_trill_roo_info.remote_rbr_nickname, /* Trill: egress nickname */
                                             &fec_if_id /* if_id object for trill fec */
                                             ); 
    if (rv != BCM_E_NONE) {
        printf("Error, trill_roo_add_remote_rbridge_router\n");
        return rv;
    } 

    if (verbose >= 2) {
        printf("remote rbridge router configured \n"); 
    }


    /* access side configuration */

    /* create RIF for IP routing
     * - inRif allow the packet to be routed (for ingress Trill)
     * - outRif helps to build the native LL: SA and VLAN (for egress Trill) */    
    intf.my_global_mac = cint_trill_roo_info.my_mac;  /* my mac (global value) */
    intf.my_lsb_mac =  cint_trill_roo_info.my_mac;  /* my mac */
    intf.vsi = cint_trill_roo_info.access_vlan; /* l3_if object = rif = vsi = access vlan */
    rv = l3__intf_rif__create(unit, &intf); 
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
      return rv;
    } 

    /* For egress Trill packet
     * create FEC for LL
       create arp entry (LL eedb entry) */
    int ll_intf; 
    int ll_encap_id; 
    create_l3_egress_s l3_egress;
    l3_egress.out_gport = cint_trill_roo_info.access_port;  /* outgoing port */
    l3_egress.vlan  = cint_trill_roo_info.access_vlan; /* vlan at the LL: access vlan */
    l3_egress.next_hop_mac_addr = cint_trill_roo_info.router_a_mac;  /* next hop mac */
    rv = l3__egress__create(unit, &l3_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
      return rv;
    } 
    ll_intf = l3_egress.fec_id; /* Contain the fec_id  (l3_if object. Type FEC) */
    ll_encap_id = l3_egress.arp_encap_id; /* Contains LL eedb index (l3_if object. Type encap) */
    if (verbose >= 2) {
        printf("Trill Egress FEC: 0x%x, LL-EEP: 0x%x \n", ll_intf, ll_encap_id);
    }    

    /* Add entry in host table for ingress Trill packets */
    bcm_l3_host_t l3_host;      
    bcm_l3_host_t_init(l3_host);  
    /* key of host entry */
    l3_host.l3a_vrf = 0;              /* router interface */ 
    l3_host.l3a_ip_addr = host90_ip;  /* ip host entry */
    /* data of host entry */
    /* native outrif and arp pointer are saved in host table */
    /* overlay tunnel: forward gport (fec gport), using l3_if object of type fec */
    BCM_GPORT_FORWARD_PORT_SET(l3_host.l3a_port_tgid, fec_if_id); 
    l3_host.l3a_intf = cint_trill_roo_info.global_vpn;  /* l3_if object, for native out rif */
    l3_host.encap_id = native_ll_encap_id;  /* native arp pointer: encap/eei entry */
  
    bcm_l3_host_add(unit, &l3_host);         
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_host_add\n");
      return rv;
    } 

    /* Add entry in host table for egress Trill packets */
    bcm_l3_host_t_init(l3_host);  
    /* key of host entry */
    l3_host.l3a_vrf = 0;           /* router interface */
    l3_host.l3a_ip_addr = host11_ip;  /* ip host entry */
    /* data of host entry */
    l3_host.l3a_intf = ll_intf; /* routing fec */
    rv = bcm_l3_host_add(unit, &l3_host);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_host_add\n");
        return rv;
    }


    return rv;
}


int 
trill_roo_run_default_with_ports(int unit, int accessPort, int providerPort) {
    trill_roo_info_init(unit, 
                        accessPort, /* access port */
                        providerPort  /* provider port */
                       ); 
    verbose = 2; 
    return trill_roo_run(unit);
}
/*remark trill ROO dscp to 35 with qos remark profile*/
int
trill_roo_qos_run(int unit, int accessPort, int providerPort){
    int rv = BCM_E_NONE;
    create_l3_intf_s intf;

    verbose = 2;
    if (verbose >= 2) {
        printf("trill_roo_qos_run \n");
    }

    rv = qos_map_egress_dscp_profile(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, qos_map_egress_dscp_profile fail\n");
        return rv;
    }

    return trill_roo_run_default_with_ports(unit, accessPort, providerPort);
}

